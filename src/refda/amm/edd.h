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
#ifndef REFDA_AMM_EDD_H_
#define REFDA_AMM_EDD_H_

#include "refda/valprod.h"
#include "cace/amm/typing.h"

#ifdef __cplusplus
extern "C" {
#endif

// Forward declaration
struct refda_amm_edd_desc_s;
typedef struct refda_amm_edd_desc_s refda_amm_edd_desc_t;

/** Value production callback for an EDD object.
 * @note The success or failure of production is indicated by the result value
 * being set or not within the @c ctx parameter.
 *
 * @param[in] obj Pointer to the EDD descriptor.
 * @param[in,out] ctx The production context, including result storage.
 */
typedef void (*refda_amm_edd_produce_f)(const refda_amm_edd_desc_t *obj, refda_valprod_ctx_t *ctx);

/** An Externally Defined Data (EDD) descriptor.
 * This defines the properties of an EDD in an Agent and includes common
 * object metadata.
 */
struct refda_amm_edd_desc_s
{
    /** The required type for the produced value.
     * All type references are fully recursively resolved.
     * The type object is owned by this descriptor.
     */
    amm_type_t prod_type;

    /** Value production callback for this object.
     */
    refda_amm_edd_produce_f produce;
};

void refda_amm_edd_desc_init(refda_amm_edd_desc_t *obj);

void refda_amm_edd_desc_deinit(refda_amm_edd_desc_t *obj);

/** Perform value production procedure on an EDD.
 *
 * @param obj The object to produce from.
 * @param ctx The production context.
 * @return Zero upon success.
 */
int refda_amm_edd_desc_produce(const refda_amm_edd_desc_t *obj, refda_valprod_ctx_t *ctx);

#ifdef __cplusplus
} // extern C
#endif

#endif /* REFDA_AMM_EDD_H_ */
