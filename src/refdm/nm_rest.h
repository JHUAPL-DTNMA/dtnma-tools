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

#include "mgr.h"

#ifdef __cplusplus
extern "C" {
#endif

/** Begin operation of the REST service.
 *
 * @param[out] ctx The context to store into.
 * @return Zero if successful.
 * @sa nm_rest_stop()
 */
int nm_rest_start(struct mg_context **ctx, refdm_mgr_t *mgr);

/** Stop operation of the REST service.
 *
 * @param[in] ctx The context to stop for.
 * @sa nm_rest_start()
 */
void nm_rest_stop(struct mg_context *ctx);

// Standard HTTP Status Codes
#define HTTP_OK                 200
#define HTTP_NO_CONTENT         204
#define HTTP_BAD_REQUEST        400
#define HTTP_FORBIDDEN          403
#define HTTP_NOT_FOUND          404
#define HTTP_METHOD_NOT_ALLOWED 405
#define HTTP_UNSUP_MEDIA_TYPE   415
#define HTTP_UNPROCESSABLE_CNT  422
#define HTTP_INTERNAL_ERROR     500
#define HTTP_NOT_IMPLEMENTED    501
#define HTTP_NO_SERVICE         503

#ifdef __cplusplus
}
#endif

#endif /* _NM_REST_H_ */
