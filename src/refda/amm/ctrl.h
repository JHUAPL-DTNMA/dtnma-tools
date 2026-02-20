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
#ifndef REFDA_AMM_CTRL_H_
#define REFDA_AMM_CTRL_H_

#include "cace/amm/typing.h"

#ifdef __cplusplus
extern "C" {
#endif

// forward declaration for callback reference
typedef struct refda_ctrl_exec_ctx_s refda_ctrl_exec_ctx_t;

/** A control (CTRL) descriptor.
 * This defines the properties of a CTRL in an Agent and includes common
 * object metadata.
 */
typedef struct
{
    /** An optional type for the result value.
     * This type will not change during the lifetime of the CTRL.
     * All type references are fully recursively resolved.
     * The type object is owned by this descriptor.
     */
    cace_amm_type_t res_type;

    /** Execution callback for this object.
     *
     * @param[in,out] ctx The execution context, including result storage.
     * @return Zero upon success, or any other value for failure.
     */
    void (*execute)(refda_ctrl_exec_ctx_t *ctx);
} refda_amm_ctrl_desc_t;

void refda_amm_ctrl_desc_init(refda_amm_ctrl_desc_t *obj);

void refda_amm_ctrl_desc_deinit(refda_amm_ctrl_desc_t *obj);

#ifdef __cplusplus
} // extern C
#endif

#endif /* REFDA_AMM_CTRL_H_ */
