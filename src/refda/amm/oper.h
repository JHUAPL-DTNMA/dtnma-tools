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
#ifndef REFDA_AMM_OPER_H_
#define REFDA_AMM_OPER_H_

#include <cace/amm/typing.h>
#include <cace/amm/named_type.h>
#include <cace/ari.h>

#ifdef __cplusplus
extern "C" {
#endif

// forward declaration for callback reference
struct refda_oper_eval_ctx_s;
typedef struct refda_oper_eval_ctx_s refda_oper_eval_ctx_t;

/** An operator (OPER) descriptor.
 * This defines the properties of a OPER in an Agent and includes common
 * object metadata.
 */
typedef struct
{
    /** Types for each of the operands in original order.
     */
    amm_named_type_array_t operand_types;

    /** Required type for the result value.
     * All type references are fully recursively resolved.
     * The type object is owned by this descriptor.
     */
    amm_type_t res_type;

    /** Evaluation callback for this object.
     *
     * @param[in,out] ctx The evaluation context, including extracted
     * parameters and result placeholder.
     * A successful evaluation will set a result value.
     */
    void (*evaluate)(refda_oper_eval_ctx_t *ctx);
} refda_amm_oper_desc_t;

void refda_amm_oper_desc_init(refda_amm_oper_desc_t *obj);

void refda_amm_oper_desc_deinit(refda_amm_oper_desc_t *obj);

#ifdef __cplusplus
} // extern C
#endif

#endif /* REFDA_AMM_OPER_H_ */
