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
#ifndef REFDA_AMM_CONST_H_
#define REFDA_AMM_CONST_H_

#include "refda/valprod.h"

#ifdef __cplusplus
extern "C" {
#endif

/** A CONST descriptor.
 * This defines the properties of a CONST in an Agent and includes common
 * object metadata.
 */
typedef struct
{
    /** Storage for the constant value.
     * This is type-converted at initialization time so the parent
     * struct has no explicit ::amm_type_t because it is assumed that the
     * value has been converted externally.
     */
    ari_t value;

} refda_amm_const_desc_t;

void refda_amm_const_desc_init(refda_amm_const_desc_t *obj);

void refda_amm_const_desc_deinit(refda_amm_const_desc_t *obj);

/** Perform value production procedure on a CONST.
 *
 * @param obj The object to produce from.
 * @param ctx The production context.
 * @return Zero upon success.
 */
int refda_amm_const_desc_produce(const refda_amm_const_desc_t *obj, refda_valprod_ctx_t *ctx);

#ifdef __cplusplus
} // extern C
#endif

#endif /* REFDA_AMM_CONST_H_ */
