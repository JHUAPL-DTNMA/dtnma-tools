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

#ifndef REFDA_EVAL_H_
#define REFDA_EVAL_H_

#include "agent.h"
#include "runctx.h"
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

/** Implement the evaluation procedure from Section TBD of @cite ietf-dtn-amm-01.
 *
 * @param[in] agent The agent state for ARI lookup.
 * @param[out] result The single result value from the evaluation.
 * This ARI must be initialized before the call and will be valid if the
 * return code is zero but must be deinitialized regardless.
 * @param[in] expr The literal-value EXPR to evaluate.
 * @return Zero if successful.
 */
int refda_eval_target(refda_runctx_t *runctx, cace_ari_t *result, const cace_ari_t *expr);

/** Evaluate given conditional ARI expression and return a boolean ARI indicating whether
 * the condition was true or false.
 *
 * @param[in] runctx The run context.
 * @param[out] result The single result value from the evaluation.
 * This ARI must be initialized before the call and will be valid if the
 * return code is zero but must be deinitialized regardless.
 * @param[in] condition The ARI to dereference, if necessary, and evaluate.
 * @return Zero if successful.
 */
int refda_eval_condition(refda_runctx_t *runctx, cace_ari_t *result, const cace_ari_t *condition);

#ifdef __cplusplus
} // extern C
#endif

#endif /* REFDA_EVAL_H_ */
