/*
 * Copyright (c) 2011-2025 The Johns Hopkins University Applied Physics
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

#ifndef REFDA_REPORTING_CTX_H_
#define REFDA_REPORTING_CTX_H_

#include "runctx.h"

#ifdef __cplusplus
extern "C" {
#endif

/** Context for reporting activities.
 */
typedef struct
{
    /** Parent running context.
     * This will never be null.
     */
    refda_runctx_t *runctx;

    /** Storage for the items of a report layer.
     * This is initialized as empty and is pushed back as items are added.
     */
    cace_ari_list_t items;
} refda_reporting_ctx_t;

/** Initialize a context based on an parent runtime context and a destination
 * .
 *
 * @param[out] obj The context to initialize.
 * @param[in] runctx The parent runtime context.
 * @param[in] mgr_ident The destination manager.
 */
void refda_reporting_ctx_init(refda_reporting_ctx_t *obj, const refda_runctx_t *runctx, const cace_ari_t *mgr_ident);

void refda_reporting_ctx_deinit(refda_reporting_ctx_t *obj);

#ifdef __cplusplus
} // extern C
#endif

#endif /* REFDA_REPORTING_CTX_H_ */
