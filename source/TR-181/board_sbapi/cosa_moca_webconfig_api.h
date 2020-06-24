/*
 * If not stated otherwise in this file or this component's Licenses.txt file the
 * following copyright and licenses apply:
 *
 * Copyright 2016 RDK Management
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
*/

#ifndef  _COSA_MOCA_WEBCONFIG_API_H
#define  _COSA_MOCA_WEBCONFIG_API_H

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <ctype.h>
#include "ansc_status.h"
#include "cosa_moca_param.h"
#include "webconfig_framework.h"

#define SUBDOC_COUNT 1

#ifdef WEBCFG_TEST_SIM

#define NACK_SIMULATE_FILE "/tmp/sim_nack"
#define TIMEOUT_SIMULATE_FILE "/tmp/sim_timeout"

#endif

uint32_t getBlobVersion(char* subdoc);
int setBlobVersion(char* subdoc,uint32_t version);
void webConfigFrameworkInit() ;
pErr Process_Moca_WebConfigRequest(void *Data);
int rollback_moca_conf() ;
void freeResources_moca(void *arg);
#endif
