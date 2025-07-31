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
#ifndef REFDA_AMM_SBR_H_
#define REFDA_AMM_SBR_H_

#include <cace/amm/typing.h>
#include <cace/ari.h>
#include <cace/util/defs.h>

#ifdef __cplusplus
extern "C" {
#endif

/** A SBR descriptor.
 * This defines the properties of a SBR in an Agent and includes common
 * object metadata.
 */
typedef struct
{
    /** Action in the form of a Macro (MAC). When triggered, the action execution
     * SHALL be executed in accordance with Section 6.6 in an execution context with no parameters.
     */
    cace_ari_t action;

    /** An SBR definition SHALL include a condition in the form of an Expression (EXPR).
     * The condition SHALL be evaluated in accordance with Section 6.7 in an evaluation
     * context with no parameters. The result of the condition SHALL be converted to a
     *  BOOL value after evaluation and used to determine when to execute the action of the SBR.
     */
    cace_ari_t condition;

    /** An SBR definition SHALL include a minimum execution interval in the form of a non-negative TD value.
     * The interval MAY be zero to indicate that there is no minimum. This is not a limit on the interval of
     * evaluations of the condition. This value can be used to limit potentially high processing loads on an Agent.
     */
    cace_ari_t min_interval;

    /** Maximum execution count in the form of a non-negative UVAST value.
     * The count sentinel value zero SHALL be interpreted as having no maximum.
     * This is not a limit on the number of evaluations of the condition.
     */
    cace_ari_uvast max_exec_count;

    /** Initial value for rule's enabled state. If not provided, the initial enabled state SHALL be true.
     */
    bool init_enabled;

    /** Current enabled state of the rule.
     */
    bool enabled;

    /** Track state of current number of executions since the rule was enabled.
     */
    cace_ari_uvast exec_count;

} refda_amm_sbr_desc_t;

void refda_amm_sbr_desc_init(refda_amm_sbr_desc_t *obj);

void refda_amm_sbr_desc_deinit(refda_amm_sbr_desc_t *obj);

/** Determine if a SBR has reached its maximum execution count
 *
 * @param[in] obj SBR object to check
 * @return True if maximum has been reached, false otherwise
 */
bool refda_amm_sbr_desc_reached_max_exec_count(refda_amm_sbr_desc_t *obj);

#ifdef __cplusplus
} // extern C
#endif

#endif /* REFDA_AMM_SBR_H_ */
