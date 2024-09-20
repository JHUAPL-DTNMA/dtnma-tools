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

#ifndef REFDA_REPORTING_H_
#define REFDA_REPORTING_H_

#include "runctx.h"

/// Error result when an invalid type is present
#define REFDA_REPORTING_ERR_BAD_TYPE 3
/// Error result when dereferencing fails
#define REFDA_REPORTING_ERR_DEREF_FAILED 4
/// Error result when value production fails
#define REFDA_REPORTING_ERR_PROD_FAILED 5

/** Generate a RPTSET for the conclusion of a CTRL exectuion.
 *
 * @param[in] runctx The context for RPTSET aggregation.
 * @param[in] target The original execution target being reported on.
 * @param[in,out] result The result value to move into the report.
 * @return Zero if successful.
 */
int refda_reporting_ctrl(refda_runctx_t *runctx, const ari_t *target, ari_t *result);

/** Implement the reporting procedure from Section TBD of @cite ietf-dtn-amm-01.
 *
 * @param[in] runctx The context for reporting.
 * @param[in] target The ARI to generate a report for.
 * @return Zero if successful.
 */
int refda_reporting_target(refda_runctx_t *runctx, const ari_t *target);

#endif /* REFDA_REPORTING_H_ */