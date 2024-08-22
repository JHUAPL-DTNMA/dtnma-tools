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
#ifndef CACE_AMM_EDD_H_
#define CACE_AMM_EDD_H_

#include "obj_base.h"
#include "runctx.h"

#ifdef __cplusplus
extern "C" {
#endif

/** An EDD descriptor.
 * This defines the properties of an EDD in an Agent and includes common
 * object metadata.
 */
typedef struct cace_amm_edd_desc_s
{
    CACE_AMM_OBJTYPE_BASE_MEMBER

    /** The fully recursively resolved type for the produced value.
     * The type object is owned by this descriptor.
     */
    amm_type_t *typeobj;

    /** Value production callback for this object.
     *
     * @param[in] obj Pointer to this descriptor.
     * @param[in,out] ctx The production context, including result storage.
     */
    void (*produce)(const struct cace_amm_edd_desc_s *obj, cace_amm_valprod_ctx_t *ctx);

} cace_amm_edd_desc_t;

void cace_amm_edd_desc_init(cace_amm_edd_desc_t *obj);

void cace_amm_edd_desc_deinit(cace_amm_edd_desc_t *obj);

/** Perform value production procedure on an EDD.
 *
 * @param obj The object to produce from.
 * @param ctx The production context.
 * @return Zero upon success.
 */
int cace_amm_edd_desc_produce(const cace_amm_edd_desc_t *obj, cace_amm_valprod_ctx_t *ctx);

/// Define functions and structures for ::cace_amm_edd_desc_t use
CACE_AMM_OBJTYPE_DEFINE_STORAGE(edd)

#ifdef __cplusplus
} // extern C
#endif

#endif /* CACE_AMM_EDD_H_ */
