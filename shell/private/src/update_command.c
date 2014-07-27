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
 * start_command.c
 *
 *  \date       Aug 20, 2010
 *  \author    	<a href="mailto:celix-dev@incubator.apache.org">Apache Celix Project Team</a>
 *  \copyright	Apache License, Version 2.0
 *
 */
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include <curl/easy.h>

#include "command_impl.h"
#include "array_list.h"
#include "bundle_context.h"
#include "bundle.h"
#include "update_command.h"

void updateCommand_execute(command_pt command, char * line, void (*out)(char *), void (*err)(char *));
celix_status_t updateCommand_download(command_pt command, char * url, char **inputFile);
size_t updateCommand_writeData(void *ptr, size_t size, size_t nmemb, FILE *stream);

command_pt updateCommand_create(bundle_context_pt context) {
	command_pt command = (command_pt) malloc(sizeof(*command));
	command->bundleContext = context;
	command->name = "update";
	command->shortDescription = "update bundle.";
	command->usage = "update <id> [<URL>]";
	command->executeCommand = updateCommand_execute;
	return command;
}

void updateCommand_destroy(command_pt command) {
	free(command);
}


void updateCommand_execute(command_pt command, char * line, void (*out)(char *), void (*err)(char *)) {
    bundle_pt bundle = NULL;
	char delims[] = " ";
	char * sub = NULL;
	char outString[256];

	sub = strtok(line, delims);
	sub = strtok(NULL, delims);

	if (sub == NULL) {
		err("Incorrect number of arguments.\n");
		sprintf(outString, "%s\n", command->usage);
		out(outString);
	} else {
		long id = atol(sub);
		bundleContext_getBundleById(command->bundleContext, id, &bundle);
		if (bundle != NULL) {
			char inputFile[256];
			sub = strtok(NULL, delims);
			inputFile[0] = '\0';
			if (sub != NULL) {
				char *test = inputFile;
				printf("URL: %s\n", sub);

				if (updateCommand_download(command, sub, &test) == CELIX_SUCCESS) {
					printf("Update bundle with stream\n");
					bundle_update(bundle, inputFile);
				} else {
					char error[256];
					sprintf(error, "Unable to download from %s\n", sub);
					err(error);
				}
			} else {
				bundle_update(bundle, NULL);
			}
		} else {
			err("Bundle id is invalid.\n");
		}
	}
}

celix_status_t updateCommand_download(command_pt command, char * url, char **inputFile) {
	CURL *curl;
	CURLcode res;
	curl = curl_easy_init();
	if (curl) {
		FILE *fp = NULL;
		tmpnam(*inputFile);
		printf("Temp file: %s\n", *inputFile);
		fp = fopen(*inputFile, "wb+");
		curl_easy_setopt(curl, CURLOPT_URL, url);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, updateCommand_writeData);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
		//curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0);
		//curl_easy_setopt(curl, CURLOPT_PROGRESSFUNCTION, updateCommand_downloadProgress);
		res = curl_easy_perform(curl);
		printf("Error: %d\n", res);
		/* always cleanup */
		curl_easy_cleanup(curl);
		fclose(fp);
	}
	if (res != CURLE_OK) {
		*inputFile[0] = '\0';
		return CELIX_ILLEGAL_STATE;
	} else {
		return CELIX_SUCCESS;
	}
}

size_t updateCommand_writeData(void *ptr, size_t size, size_t nmemb, FILE *stream) {
    size_t written = fwrite(ptr, size, nmemb, stream);
    return written;
}
