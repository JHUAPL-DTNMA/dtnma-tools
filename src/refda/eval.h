/*
 * Copyright (c) 2011-2026 The Johns Hopkins University Applied Physics
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

#ifndef REFDA_EVAL_H_
#define REFDA_EVAL_H_

#include "agent.h"
#include "runctx.h"
#include "eval_ctx.h"
#include <cace/ari.h>

#ifdef __cplusplus
extern "C" {
#endif

/// Error result when an invalid type is present
#define REFDA_EVAL_ERR_BAD_TYPE 3
/// Error result when dereferencing fails
#define REFDA_EVAL_ERR_DEREF_FAILED 4
/// Error result when value production fails
#define REFDA_EVAL_ERR_PROD_FAILED 5
/// Error result when the evaluation does not resolve to a single value
#define REFDA_EVAL_ERR_NON_SINGLE 6

/** Perform the expansion portion of the evaluation procedure from either an
 * inline expression (literal value) or produced expression (from reference value).
 *
 * @pre The @c refda_agent_s::objs_mutex must already be locked.
 * @param[in] ctx The evaluation context, which must already be initialized.
 * @param[in] target The literal-value EXPR to evaluate or reference-value
 * to produce and evaluate.
 * @return Zero if successful.
 */
int refda_eval_expand_target(refda_eval_ctx_t *ctx, const cace_ari_t *target);

/** Perform the expansion portion of the evaluation procedure from
 * a literal value expression.
 *
 * @pre The @c refda_agent_s::objs_mutex must already be locked.
 * @param[in] ctx The evaluation context, which must already be initialized.
 * @param[in] expr The literal-value EXPR to evaluate.
 * @return Zero if successful.
 */
int refda_eval_expand_expr(refda_eval_ctx_t *ctx, const cace_ari_t *expr);

/** Implement the reduction portion of the evaluation procedure.
 *
 * @param[in] ctx The evaluation context, which must already be expanded into.
 * @param[out] result The single result value from the evaluation.
 * This ARI must be initialized before the call and will be valid if the
 * return code is zero but must be deinitialized regardless.
 * @return Zero if successful.
 */
int refda_eval_reduce(refda_eval_ctx_t *ctx, cace_ari_t *result);

/** A shortcut to fully evaluate an expression.
 * This function performs selective @c refda_agent_s::objs_mutex locking.
 *
 * @param[in] runctx The running context for evaluation.
 * @param[out] result The single result value from the evaluation.
 * This ARI must be initialized before the call and will be valid if the
 * return code is zero but must be deinitialized regardless.
 * @param[in] target The literal-value EXPR to evaluate or reference-value
 * to produce and evaluate.
 * @return Zero if successful.
 */
int refda_eval_target(refda_runctx_t *runctx, cace_ari_t *result, const cace_ari_t *target);

/** Function to substitute a literal or reference value
 * within a cace_ari_translator_t::map_ari function.
 * If the input is a reference and it is in an AC list (i.e. an expr), it will
 * be wrapped in @c //ietf/dtnma-agent/oper/ref to preserve references in an expression.
 *
 * @param[out] out The output value to be replaced.
 * @param[in] value The input value to do the replacing as a copy.
 * @param[in] ctx The context to know the parent value and depth.
 */
void refda_eval_label_subst(cace_ari_t *out, const cace_ari_t *value, const cace_ari_translate_ctx_t *ctx);

/** A shortcut to substitute LABEL values and then evaluate an expression.
 * This function performs selective @c refda_agent_s::objs_mutex locking.
 *
 * @param[in] runctx The running context for evaluation.
 * @param[out] result The single result value from the evaluation.
 * This ARI must be initialized before the call and will be valid if the
 * return code is zero but must be deinitialized regardless.
 * @param[in] target The literal-value EXPR to evaluate or reference-value
 * to produce and evaluate.
 * @param[in] translator The target value substitution translator.
 * @param[in] user_data Pointer for the @c translator to use.
 * @return Zero if successful.
 */
int refda_eval_filter(refda_runctx_t *runctx, cace_ari_t *result, const cace_ari_t *target,
                      const cace_ari_translator_t *translator, void *user_data);

/** This is a semi-internal function for evaluating a single operator in
 * an existing evaluation context.
 *
 * @param[in,out] ctx The evaluation context already populated with
 * refda_eval_ctx_t::stack values.
 * At the start of evaluation any named operands are popped from the stack
 * At the completion of evaluation the result value is pushed onto the stack.
 * This function does not use the refda_eval_ctx_t::input value.
 * @param[in] deref The dereferenced operator with any needed actual parameters.
 * @return Zero if successful.
 */
int refda_eval_oper(refda_eval_ctx_t *ctx, const cace_amm_lookup_t *deref);

#ifdef __cplusplus
} // extern C
#endif

#endif /* REFDA_EVAL_H_ */
