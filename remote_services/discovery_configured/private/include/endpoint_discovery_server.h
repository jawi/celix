/**
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 *  KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */
/*
 * endpoint_discovery_server.h
 *
 * \date		Aug 12, 2014
 * \author		<a href="mailto:celix-dev@incubator.apache.org">Apache Celix Project Team</a>
 * \copyright	Apache License, Version 2.0
 */

#ifndef ENDPOINT_DISCOVERY_SERVER_H_
#define ENDPOINT_DISCOVERY_SERVER_H_

#include "celix_errno.h"
#include "discovery.h"

typedef struct endpoint_discovery_server *endpoint_discovery_server_pt;

/**
 * Creates and starts a new instance of an endpoint discovery server.
 *
 * @param discovery [in] the discovery service itself;
 * @param context [in] the bundle context;
 * @param server [out] the pointer to the created instance.
 * @return CELIX_SUCCESS when successful.
 */
celix_status_t endpointDiscoveryServer_create(discovery_pt discovery, bundle_context_pt context, endpoint_discovery_server_pt *server);

/**
 * Stops and destroys a given instance of an endpoint discovery server.
 *
 * @param server [in] the pointer to the instance to destroy.
 * @return CELIX_SUCCESS when successful.
 */
celix_status_t endpointDiscoveryServer_destroy(endpoint_discovery_server_pt server);

/**
 * Adds a given endpoint description to expose through the given discovery server.
 *
 * @param server [in] the endpoint discovery server to expose the endpoint through;
 * @param endpoint [in] the endpoint description to expose.
 * @return CELIX_SUCCESS when successful.
 */
celix_status_t endpointDiscoveryServer_addEndpoint(endpoint_discovery_server_pt server, endpoint_description_pt endpoint);

/**
 * Removes a given endpoint description from exposure through the given discovery server.
 *
 * @param server [in] the endpoint discovery server to remove the endpoint from;
 * @param endpoint [in] the endpoint description to remove.
 * @return CELIX_SUCCESS when successful.
 */
celix_status_t endpointDiscoveryServer_removeEndpoint( endpoint_discovery_server_pt server, endpoint_description_pt endpoint);

#endif /* ENDPOINT_DISCOVERY_SERVER_H_ */
