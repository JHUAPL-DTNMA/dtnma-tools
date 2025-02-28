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

#ifndef REFDA_EVAL_CTX_H_
#define REFDA_EVAL_CTX_H_

#include "runctx.h"
#include <cace/ari.h>
#include <cace/amm/lookup.h>
#include <m-variant.h>

#ifdef __cplusplus
extern "C" {
#endif

/// @cond Doxygen_Suppress
M_VARIANT_DEF2(refda_eval_item, (value, cace_ari_t), (deref, cace_amm_lookup_t))
/// OPLIST for the refda_eval_item_t
#define M_OPL_refda_eval_item_t() M_VARIANT_OPLIST(refda_eval_item, M_OPL_cace_ari_t(), M_OPL_cace_amm_lookup_t())

M_DEQUE_DEF(refda_eval_list, refda_eval_item_t)
/// @endcond

/** Context for evaluation activities, including for OPER objects.
 */
typedef struct
{
    /** Parent running context.
     * This will never be null.
     */
    refda_runctx_t *parent;

    /** Storage for the input expanded list.
     */
    refda_eval_list_t input;

    /** Storage for the evaluation stack.
     * Top of stack is at the back.
     */
    cace_ari_list_t stack;
} refda_eval_ctx_t;

/** Initialize a context based on an object reference ARI and
 * a target object's formal parameters.
 *
 * @param[out] obj The context to initialize.
 * @param[in] parent The parent runtime context.
 */
void refda_eval_ctx_init(refda_eval_ctx_t *obj, refda_runctx_t *parent);

void refda_eval_ctx_deinit(refda_eval_ctx_t *obj);

#ifdef __cplusplus
} // extern C
#endif

#endif /* REFDA_EVAL_CTX_H_ */
