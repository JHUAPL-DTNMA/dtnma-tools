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
#ifndef REFDA_AMM_CTRL_H_
#define REFDA_AMM_CTRL_H_

#include "refda/exec.h"
#include "refda/exec_ctx.h"

#ifdef __cplusplus
extern "C" {
#endif

// forward declaration for callback reference
struct refda_amm_ctrl_desc_s;
typedef struct refda_amm_ctrl_desc_s refda_amm_ctrl_desc_t;

/** A control (CTRL) descriptor.
 * This defines the properties of a CTRL in an Agent and includes common
 * object metadata.
 */
struct refda_amm_ctrl_desc_s
{
    /** An optional type for the result value.
     * All type references are fully recursively resolved.
     * The type object is owned by this descriptor.
     */
    amm_type_t res_type;

    /** Execution callback for this object.
     *
     * @param[in] obj Pointer to this descriptor.
     * @param[in,out] ctx The execution context, including result storage.
     * @return Zero upon success, or any other value for failure.
     */
    int (*execute)(const refda_amm_ctrl_desc_t *obj, refda_exec_ctx_t *ctx);
};

void refda_amm_ctrl_desc_init(refda_amm_ctrl_desc_t *obj);

void refda_amm_ctrl_desc_deinit(refda_amm_ctrl_desc_t *obj);

/** Perform the execution procedure on a CTRL.
 *
 * @param obj The object to execute.
 * @param ctx The execution context.
 * @return Zero upon success.
 */
int refda_amm_ctrl_desc_execute(const refda_amm_ctrl_desc_t *obj, refda_exec_ctx_t *ctx);

#ifdef __cplusplus
} // extern C
#endif

#endif /* REFDA_AMM_CTRL_H_ */
