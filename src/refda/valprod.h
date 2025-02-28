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
#ifndef REFDA_VALPROD_H_
#define REFDA_VALPROD_H_

#include "runctx.h"

#ifdef __cplusplus
extern "C" {
#endif

/** Context for value production activities.
 */
typedef struct
{
    /** Parent running context.
     * This will never be null.
     */
    refda_runctx_t *parent;

    /** Pointer to the reference which led to this production.
     */
    const cace_ari_t *ref;

    /** Dereference result which led to this production.
     * This will never be null.
     */
    const cace_amm_lookup_t *deref;

    /** Storage for the produced value.
     * This is initialized as undefined and must be set to any other value
     * to indicate successful production.
     */
    cace_ari_t value;
} refda_valprod_ctx_t;

/** Initialize a context based on an object reference ARI and
 * a target object's formal parameters.
 *
 * @param[out] ctx The context to initialize.
 * @param[in] parent The parent runtime context.
 * @param[in] ref The reference target.
 * @param[in] deref The dereference result.
 * The result must outlive this context.
 */
void refda_valprod_ctx_init(refda_valprod_ctx_t *ctx, refda_runctx_t *parent, const cace_ari_t *ref,
                            const cace_amm_lookup_t *deref);

void refda_valprod_ctx_deinit(refda_valprod_ctx_t *ctx);

/** Perform an object-type-specific procedure to produce a value.
 *
 * @param[in,out] ctx The production context to use.
 * @return Zero if successful.
 */
int refda_valprod_run(refda_valprod_ctx_t *ctx);

#ifdef __cplusplus
} // extern C
#endif

#endif /* REFDA_VALPROD_H_ */
