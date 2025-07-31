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
#ifndef REFDA_AMM_IDENT_H_
#define REFDA_AMM_IDENT_H_

#include "cace/amm/obj_ref.h"
#include "cace/amm/obj_ns.h"
#include "cace/amm/user_data.h"

#ifdef __cplusplus
extern "C" {
#endif

M_DEQUE_DEF(cace_amm_obj_desc_ptr_list, cace_amm_obj_desc_t *, M_PTR_OPLIST)

/** An IDENT descriptor.
 * This defines the properties of an IDENT in an Agent.
 */
typedef struct refda_amm_ident_desc_s
{
    /** All base IDENT objects for this object, not including transitive bases.
     * This list will not change during the lifetime of the IDENT,
     * but the object descriptor pointers will be NULL until the
     * binding activity in the Agent.
     */
    cace_amm_obj_ref_list_t bases;

    /** All derived IDENT objects from this object, not including transitive derivations.
     * This list will start out empty from the model registration
     * and be populated during binding activity in the Agent.
     */
    cace_amm_obj_desc_ptr_list_t derived;

    /** Optional ADM data associated with this object.
     */
    cace_amm_user_data_t user_data;

} refda_amm_ident_desc_t;

void refda_amm_ident_desc_init(refda_amm_ident_desc_t *obj);

void refda_amm_ident_desc_deinit(refda_amm_ident_desc_t *obj);

#ifdef __cplusplus
} // extern C
#endif

#endif /* REFDA_AMM_IDENT_H_ */
