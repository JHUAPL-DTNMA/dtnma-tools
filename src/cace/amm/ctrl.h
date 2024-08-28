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
#ifndef CACE_AMM_CTRL_H_
#define CACE_AMM_CTRL_H_

//#include "obj_base.h"
#include "runctx.h"

#ifdef __cplusplus
extern "C" {
#endif

// forward declaration for callback reference
struct cace_amm_ctrl_desc_s;
typedef struct cace_amm_ctrl_desc_s cace_amm_ctrl_desc_t;

/** A control (CTRL) descriptor.
 * This defines the properties of a CTRL in an Agent and includes common
 * object metadata.
 */
struct cace_amm_ctrl_desc_s
{
    /** An optional type for the result value.
     * All type references are fully recursively resolved.
     * The type object is owned by this descriptor.
     */
    amm_type_t res_type;

    /** Execution callback for this object.
     *
     * @param[in] obj Pointer to this descriptor.
     * @param[in,out] ctx The production context, including result storage.
     * @return Zero upon success, or any other value for failure.
     */
    int (*execute)(const cace_amm_ctrl_desc_t *obj, cace_amm_exec_ctx_t *ctx);
};

void cace_amm_ctrl_desc_init(cace_amm_ctrl_desc_t *obj);

void cace_amm_ctrl_desc_deinit(cace_amm_ctrl_desc_t *obj);

/** Perform the execution procedure on a CTRL.
 *
 * @param obj The object to execute.
 * @param ctx The execution context.
 * @return Zero upon success.
 */
int cace_amm_ctrl_desc_execute(const cace_amm_ctrl_desc_t *obj, cace_amm_exec_ctx_t *ctx);

#ifdef __cplusplus
} // extern C
#endif

#endif /* CACE_AMM_CTRL_H_ */
