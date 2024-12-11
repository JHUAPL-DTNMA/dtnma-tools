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

#ifndef REFDA_OPER_EVAL_CTX_H_
#define REFDA_OPER_EVAL_CTX_H_

#include "agent.h"
#include "eval_ctx.h"
#include "refda/amm/oper.h"
#include <cace/ari.h>

#ifdef __cplusplus
extern "C" {
#endif

/** Context for OPER evaluation activities.
 */
typedef struct refda_oper_eval_ctx_s
{
    /** Parent production context.
     * This will never be null.
     */
    refda_eval_ctx_t *evalctx;

    /** Dereference result for OPER which led to this evaluation.
     * This will never be NULL.
     */
    const cace_amm_lookup_t *deref;

    /** Descriptor for the EDD being produced from.
     * This will never be null.
     */
    const refda_amm_oper_desc_t *oper;

    /** Operands which have already been popped from the stack and
     * converted to the OPER-specific operand types.
     * The order of these operands is the same as the OPER definition.
     */
    ari_array_t operands;

    /** Storage for the evaluation result.
     * This is initialized as undefined and must be set to any other value
     * to indicate successful evaluation.
     */
    ari_t result;

} refda_oper_eval_ctx_t;

void refda_oper_eval_ctx_init(refda_oper_eval_ctx_t *obj);

void refda_oper_eval_ctx_deinit(refda_oper_eval_ctx_t *obj);

/** Populate a context based on an object reference ARI and
 * a target object's formal parameters.
 *
 * @param[out] obj The context to initialize.
 * @param[in] deref The lookup dereference.
 * @[aram[in] oper The OPER descriptor to evaluate with.
 * @param[in,out] eval The evaluation state to pop operands from.
 * The result must outlive this context.
 * @return Zero if successful.
 */
int refda_oper_eval_ctx_populate(refda_oper_eval_ctx_t *obj, const cace_amm_lookup_t *deref,
                                 const refda_amm_oper_desc_t *oper, refda_eval_ctx_t *eval);

const ari_t *refda_oper_eval_ctx_get_aparam_index(refda_oper_eval_ctx_t *ctx, size_t index);

const ari_t *refda_oper_eval_ctx_get_aparam_name(refda_oper_eval_ctx_t *ctx, const char *name);

const ari_t *refda_oper_eval_ctx_get_operand_index(refda_oper_eval_ctx_t *ctx, size_t index);

const ari_t *refda_oper_eval_ctx_get_operand_name(refda_oper_eval_ctx_t *ctx, const char *name);

void refda_oper_eval_ctx_set_result_copy(refda_oper_eval_ctx_t *ctx, const ari_t *value);

void refda_oper_eval_ctx_set_result_move(refda_oper_eval_ctx_t *ctx, ari_t *value);

#ifdef __cplusplus
} // extern C
#endif

#endif /* REFDA_OPER_EVAL_CTX_H_ */
