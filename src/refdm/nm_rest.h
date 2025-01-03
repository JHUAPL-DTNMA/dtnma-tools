/*
 * Copyright (c) 2011-2024 The Johns Hopkins University Applied Physics
 * Laboratory LLC.
 *
 * This file is part of the Delay-Tolerant Networking Management
 * Architecture (DTNMA) Tools package.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *     http://www.apache.org/licenses/LICENSE-2.0
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifndef __NM_REST_H__
#define __NM_REST_H__

#include "nmmgr.h"

#ifdef __cplusplus
extern "C" {
#endif

// TODO: Allow these to be configurable
#ifdef NO_SSL
#define PORT      "8089"
#define HOST_INFO "http://localhost:8089"
#else
#define PORT      "8089r,8843s"
#define HOST_INFO "https://localhost:8843"
#endif

int  nm_rest_start(nmmgr_t *mgr);
void nm_rest_stop(void);

// Standard HTTP Status Codes
#define HTTP_OK                 200
#define HTTP_NO_CONTENT         204
#define HTTP_BAD_REQUEST        400
#define HTTP_FORBIDDEN          403
#define HTTP_NOT_FOUND          404
#define HTTP_METHOD_NOT_ALLOWED 405
#define HTTP_INTERNAL_ERROR     500
#define HTTP_NOT_IMPLEMENTED    501
#define HTTP_NO_SERVICE         503

#ifdef __cplusplus
}
#endif

#endif /* _NM_REST_H_ */
