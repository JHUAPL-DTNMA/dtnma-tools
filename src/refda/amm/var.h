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
#ifndef REFDA_AMM_VAR_H_
#define REFDA_AMM_VAR_H_

#include "runctx.h"

#ifdef __cplusplus
extern "C" {
#endif

/** A VAR descriptor.
 * This defines the properties of a VAR in an Agent and includes common
 * object metadata.
 */
typedef struct
{
    /** The required type for the stored value and result value.
     * All type references are fully recursively resolved.
     * The type object is owned by this descriptor.
     */
    amm_type_t val_type;

    /** Storage for the value.
     * This is initialized as undefined and must be set to any other value
     * to indicate successful production.
     */
    ari_t value;

} refda_amm_var_desc_t;

void refda_amm_var_desc_init(refda_amm_var_desc_t *obj);

void refda_amm_var_desc_deinit(refda_amm_var_desc_t *obj);

/** Perform value production procedure on a VAR.
 *
 * @param obj The object to produce from.
 * @param ctx The production context.
 * @return Zero upon success.
 */
int refda_amm_var_desc_produce(const refda_amm_var_desc_t *obj, refda_amm_valprod_ctx_t *ctx);

#ifdef __cplusplus
} // extern C
#endif

#endif /* REFDA_AMM_VAR_H_ */
