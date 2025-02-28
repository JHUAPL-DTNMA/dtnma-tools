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
#ifndef REFDA_AMM_TBR_H_
#define REFDA_AMM_TBR_H_

#include <cace/amm/typing.h>
#include <cace/ari.h>

#ifdef __cplusplus
extern "C" {
#endif

/** A TBR descriptor.
 * This defines the properties of a TBR in an Agent and includes common
 * object metadata.
 */
typedef struct
{
/** action in the form of a Macro (MAC). When triggered, the action execution
 * SHALL be executed in accordance with Section 6.6 in an execution context with no parameters.
*/

TBD action;

/** Start time in the form of a TIME (Section 4.2.2) value.
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
cace_ari_t max_exec_count;

     /** Enabled state of the rule
      */
    cace_ari_t enabled;

//    /** The required type for the stored value and result value.
//     * This type will not change during the lifetime of the TBR.
//     * All type references are fully recursively resolved.
//     * The type object is owned by this descriptor.
//     */
//    cace_amm_type_t val_type;
//
//    /** Storage for the value.
//     * This is initialized as undefined and must be set to any other value
//     * to indicate successful production.
//     */
//    cace_ari_t value;
//
//    /** Optional initializer value.
//     * This initializer will not change during the lifetime of the TBR.
//     * This is set to the undefined value if not used.
//     */
//    cace_ari_t init_val;
} refda_amm_tbr_desc_t;

void refda_amm_tbr_desc_init(refda_amm_tbr_desc_t *obj);

void refda_amm_tbr_desc_deinit(refda_amm_tbr_desc_t *obj);

#ifdef __cplusplus
} // extern C
#endif

#endif /* REFDA_AMM_TBR_H_ */
