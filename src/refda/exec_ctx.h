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

#ifndef REFDA_EXEC_CTX_H_
#define REFDA_EXEC_CTX_H_

#include "agent.h"
#include "runctx.h"
#include <cace/amm/lookup.h>
#include <cace/ari.h>

#ifdef __cplusplus
extern "C" {
#endif

/** Context for execution activities.
 */
typedef struct
{
    /** Parent running context.
     * This will never be null.
     */
    refda_runctx_t *parent;

    /** Dereference result for CTRL which led to this execution.
     * This will never be null.
     */
    const cace_amm_lookup_t *deref;

    /** Storage for an optional result value.
     * This is initialized as undefined and may be set to any other value
     * to indicate that a result is produced.
     */
    ari_t result;
} refda_exec_ctx_t;

/** Initialize a context based on an object reference ARI and
 * a target object's formal parameters.
 *
 * @param[out] obj The context to initialize.
 * @param[in] parent The parent runtime context.
 * @param[in] deref The dereference result.
 * The result must outlive this context.
 */
void refda_exec_ctx_init(refda_exec_ctx_t *obj, refda_runctx_t *parent, const cace_amm_lookup_t *deref);

void refda_exec_ctx_deinit(refda_exec_ctx_t *obj);

#ifdef __cplusplus
} // extern C
#endif

#endif /* REFDA_EXEC_CTX_H_ */
