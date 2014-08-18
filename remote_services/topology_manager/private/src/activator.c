/**
 *Licensed to the Apache Software Foundation (ASF) under one
 *or more contributor license agreements.  See the NOTICE file
 *distributed with this work for additional information
 *regarding copyright ownership.  The ASF licenses this file
 *to you under the Apache License, Version 2.0 (the
 *"License"); you may not use this file except in compliance
 *with the License.  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *Unless required by applicable law or agreed to in writing,
 *software distributed under the License is distributed on an
 *"AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 *specific language governing permissions and limitations
 *under the License.
 */
/*
 * dependency_activator.c
 *
 *  \date       Sep 29, 2011
 *  \author    	<a href="mailto:celix-dev@incubator.apache.org">Apache Celix Project Team</a>
 *  \copyright	Apache License, Version 2.0
 */

#include <stdio.h>
#include <stdlib.h>

#include "constants.h"
#include "bundle_activator.h"
#include "service_tracker.h"
#include "service_registration.h"

#include "topology_manager.h"
#include "endpoint_listener.h"
#include "remote_constants.h"
#include "listener_hook_service.h"

struct activator {
	bundle_context_pt context;

	topology_manager_pt manager;

	service_tracker_pt remoteServiceAdminTracker;
	service_listener_pt serviceListener;

	service_registration_pt endpointListenerService;
	service_registration_pt hook;
};

static celix_status_t bundleActivator_createRSATracker(struct activator *activator, service_tracker_pt *tracker);
static celix_status_t bundleActivator_createServiceListener(struct activator *activator, service_listener_pt *listener);

celix_status_t bundleActivator_create(bundle_context_pt context, void **userData) {
	celix_status_t status = CELIX_SUCCESS;
	struct activator *activator = NULL;

	activator = malloc(sizeof(struct activator));
	if (!activator) {
		return CELIX_ENOMEM;
	}

	activator->context = context;
	activator->endpointListenerService = NULL;
	activator->hook = NULL;
	activator->manager = NULL;
	activator->remoteServiceAdminTracker = NULL;
	activator->serviceListener = NULL;

	status = topologyManager_create(context, &activator->manager);
	if (status == CELIX_SUCCESS) {
		status = bundleActivator_createRSATracker(activator, &activator->remoteServiceAdminTracker);
		if (status == CELIX_SUCCESS) {
			status = bundleActivator_createServiceListener(activator, &activator->serviceListener);
			if (status == CELIX_SUCCESS) {
				*userData = activator;
			}
		}
	}

	return status;
}

static celix_status_t bundleActivator_createRSATracker(struct activator *activator, service_tracker_pt *tracker) {
	celix_status_t status = CELIX_SUCCESS;

	service_tracker_customizer_pt customizer = NULL;

	status = serviceTrackerCustomizer_create(activator->manager, topologyManager_rsaAdding,
			topologyManager_rsaAdded, topologyManager_rsaModified, topologyManager_rsaRemoved, &customizer);

	if (status == CELIX_SUCCESS) {
		status = serviceTracker_create(activator->context, "remote_service_admin", customizer, tracker);
	}

	return status;
}

static celix_status_t bundleActivator_createServiceListener(struct activator *activator, service_listener_pt *listener) {
	celix_status_t status = CELIX_SUCCESS;

	*listener = malloc(sizeof(*listener));
	if (!*listener) {
		return CELIX_ENOMEM;
	}

	(*listener)->handle = activator->manager;
	(*listener)->serviceChanged = topologyManager_serviceChanged;

	return status;
}

celix_status_t bundleActivator_start(void * userData, bundle_context_pt context) {
	celix_status_t status = CELIX_SUCCESS;
	struct activator *activator = userData;

	endpoint_listener_pt endpointListener = malloc(sizeof(*endpointListener));
	endpointListener->handle = activator->manager;
	endpointListener->endpointAdded = topologyManager_addImportedService;
	endpointListener->endpointRemoved = topologyManager_removeImportedService;

	char *uuid = NULL;
	status = bundleContext_getProperty(activator->context, (char *)OSGI_FRAMEWORK_FRAMEWORK_UUID, &uuid);
	if (!uuid) {
		printf("TOPOLOGY_MANAGER: no framework UUID defined?!\n");
		return CELIX_ILLEGAL_STATE;
	}

	size_t len = 14 + strlen(OSGI_FRAMEWORK_OBJECTCLASS) + strlen(OSGI_RSA_ENDPOINT_FRAMEWORK_UUID) + strlen(uuid);
	char *scope = malloc(len);
	if (!scope) {
		return CELIX_ENOMEM;
	}

	sprintf(scope, "(&(%s=*)(!(%s=%s)))", OSGI_FRAMEWORK_OBJECTCLASS, OSGI_RSA_ENDPOINT_FRAMEWORK_UUID, uuid);
	scope[len] = 0;

	printf("TOPOLOGY_MANAGER: endpoint listener scope is %s\n", scope);

	properties_pt props = properties_create();
	properties_set(props, (char *) OSGI_ENDPOINT_LISTENER_SCOPE, scope);

	// We can release the scope, as properties_set makes a copy of the key & value...
	free(scope);

	bundleContext_registerService(context, (char *) OSGI_ENDPOINT_LISTENER_SERVICE, endpointListener, props, &activator->endpointListenerService);

	listener_hook_service_pt hook = malloc(sizeof(*hook));
	hook->handle = activator->manager;
	hook->added = topologyManager_listenerAdded;
	hook->removed = topologyManager_listenerRemoved;

	bundleContext_registerService(context, (char *) OSGI_FRAMEWORK_LISTENER_HOOK_SERVICE_NAME, hook, NULL, &activator->hook);

	bundleContext_addServiceListener(context, activator->serviceListener, "(service.exported.interfaces=*)");
	serviceTracker_open(activator->remoteServiceAdminTracker);

	return status;
}

celix_status_t bundleActivator_stop(void * userData, bundle_context_pt context) {
	celix_status_t status = CELIX_SUCCESS;
	struct activator *activator = userData;

	serviceTracker_close(activator->remoteServiceAdminTracker);

	bundleContext_removeServiceListener(context, activator->serviceListener);

	serviceRegistration_unregister(activator->hook);
	serviceRegistration_unregister(activator->endpointListenerService);

	return status;
}

celix_status_t bundleActivator_destroy(void * userData, bundle_context_pt context) {
	struct activator *activator = userData;
	if (!activator || !activator->manager) {
		return CELIX_BUNDLE_EXCEPTION;
	}

	return topologyManager_destroy(activator->manager);
}
