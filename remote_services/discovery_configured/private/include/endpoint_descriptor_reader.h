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
 * endpoint_descriptor_reader.h
 *
 *  \date       26 Jul 2014
 *  \author     <a href="mailto:celix-dev@incubator.apache.org">Apache Celix Project Team</a>
 *  \copyright  Apache License, Version 2.0
 */

#ifndef ENDPOINT_DESCRIPTOR_READER_H_
#define ENDPOINT_DESCRIPTOR_READER_H_

#include "celix_errno.h"
#include "array_list.h"

typedef struct endpoint_descriptor_reader *endpoint_descriptor_reader_pt;

celix_status_t endpointDescriptorReader_create(endpoint_descriptor_reader_pt *reader);
celix_status_t endpointDescriptorReader_destroy(endpoint_descriptor_reader_pt reader);

celix_status_t endpointDescriptorReader_parseDocument(endpoint_descriptor_reader_pt reader, char *document, array_list_pt *endpoints);


#endif /* ENDPOINT_DESCRIPTOR_READER_H_ */
