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
#ifndef REFDA_AMM_TBR_H_
#define REFDA_AMM_TBR_H_

#include <cace/amm/typing.h>
#include <cace/ari.h>
#include <cace/util/defs.h>

#ifdef __cplusplus
extern "C" {
#endif

/** A TBR descriptor.
 * This defines the properties of a TBR in an Agent and includes common
 * object metadata.
 */
typedef struct
{
    /** Action in the form of a Macro (MAC), which must be a valid execution target.
     * When triggered, the action SHALL be executed in an execution context with no parameters.
     */
    cace_ari_t action;

    /** Start time in the form of a TIME (TP or TD) value.
     * A relative start time SHALL be interpreted relative to the absolute time at which the
     * Agent is initialized (for ADM rules) or the rule is created (for ODM rules).
     * The start time MAY be the relative time zero to indicate that the TBR is always active.
     * This is not a limit on the interval of evaluations of the condition.
     */

    cace_ari_t start_time;

    /** Time period in the form of a positive TD value.
     * The period SHALL NOT be zero but any non-zero small period is valid.
     */

    cace_ari_t period;

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

    /** Determine whether the rule has been marked obsolete
     */
    bool obsolete;

    /** Track state of current number of executions since the rule was enabled.
     */
    cace_ari_uvast exec_count;

    /** State field to track the absolute time used as reference for a relative start time.
     * For ADM rules this is when the Agent is initialized
     * For ODM rules this is when the rule was created
     */
    struct timespec absolute_start_time;

} refda_amm_tbr_desc_t;

void refda_amm_tbr_desc_init(refda_amm_tbr_desc_t *obj);

void refda_amm_tbr_desc_deinit(refda_amm_tbr_desc_t *obj);

/** Determine if a TBR has reached its maximum execution count
 *
 * @param[in] obj TBR object to check
 * @return True if maximum has been reached, false otherwise
 */
bool refda_amm_tbr_desc_reached_max_exec_count(refda_amm_tbr_desc_t *obj);

#ifdef __cplusplus
} // extern C
#endif

#endif /* REFDA_AMM_TBR_H_ */
