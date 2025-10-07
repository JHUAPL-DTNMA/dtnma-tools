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

#ifndef REFDA_REPORTING_H_
#define REFDA_REPORTING_H_

#include "runctx.h"

#ifdef __cplusplus
extern "C" {
#endif

/// Error result when an invalid type is present
#define REFDA_REPORTING_ERR_BAD_TYPE 3
/// Error result when dereferencing fails
#define REFDA_REPORTING_ERR_DEREF_FAILED 4
/// Error result when value production fails
#define REFDA_REPORTING_ERR_PROD_FAILED 5
/// Error result when value EXPR evaluation fails
#define REFDA_REPORTING_ERR_EVAL_FAILED 6

/** Generate a RPTSET for the conclusion of a CTRL execution.
 *
 * @param[in] runctx The context for RPTSET aggregation.
 * @param[in] target The original execution target being reported on.
 * @param[in,out] result The result value to move into the report.
 * @return Zero if successful.
 */
int refda_reporting_ctrl(refda_runctx_t *runctx, const cace_ari_t *target, cace_ari_t *result);

/** Implement the reporting procedure from Section TBD of @cite ietf-dtn-amm-01.
 *
 * @param[in] runctx The context for reporting.
 * @param[in] target The ARI to generate a report for.
 * @param[in] destination A destination endpoint to report to.
 * A null value or pointer sends to the manager in refda_runctx_t::mgr_ident.
 * @return Zero if successful.
 */
int refda_reporting_target(refda_runctx_t *runctx, const cace_ari_t *target, const cace_ari_t *destination);

/** Generate and queue a one-report RPTSET value.
 *
 * @param[in] agent The agent doing the reporting.
 * @param[in] destination The identity of the manager to send to.
 * @param[in,out] items The item list for the new report, which is moved out of.
 */
int refda_reporting_gen(refda_agent_t *agent, const cace_ari_t *destination, const cace_ari_t *src,
                        cace_ari_list_t items);

#ifdef __cplusplus
} // extern C
#endif

#endif /* REFDA_REPORTING_H_ */
