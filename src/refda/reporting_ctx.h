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
    refda_runctx_t *parent;

    /** Dereference result which led to this reporting.
     * This will never be null.
     */
//    const cace_amm_lookup_t *deref;

    /** Storage for the items of a report layer.
     * This is initialized as empty and is pushed back as items are added.
     */
    ari_list_t items;
} refda_reporting_ctx_t;

/** Initialize a context based on an object reference ARI and
 * a target object's formal parameters.
 *
 * @param[out] obj The context to initialize.
 * @param[in] parent The parent runtime context.
 * @param[in] deref The dereference result.
 * The result must outlive this context.
 */
void refda_reporting_ctx_init(refda_reporting_ctx_t *obj, refda_runctx_t *parent);

void refda_reporting_ctx_deinit(refda_reporting_ctx_t *obj);

#ifdef __cplusplus
} // extern C
#endif

#endif /* REFDA_REPORTING_CTX_H_ */
