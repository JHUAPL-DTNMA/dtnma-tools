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
#ifndef CACE_AMM_OBJ_DESC_H_
#define CACE_AMM_OBJ_DESC_H_

#include "cace/ari.h"
#include "parameters.h"
#include "cace/nocase.h"

#ifdef __cplusplus
extern "C" {
#endif

/** Signature for cace_amm_user_data_t::deinit function.
 *
 * @param ptr The user data being de-initialized.
 */
typedef void (*cace_amm_user_data_deinit_f)(void *ptr);

typedef struct
{
    /// Pointer to the opaque user data being managed
    void *ptr;

    /// An optional cleanup function for the #ptr.
    cace_amm_user_data_deinit_f deinit;
} cace_amm_user_data_t;

void cace_amm_user_data_init(cace_amm_user_data_t *obj);

void cace_amm_user_data_deinit(cace_amm_user_data_t *obj);

void cace_amm_user_data_set_from(cace_amm_user_data_t *obj, void *ptr, cace_amm_user_data_deinit_f deinit);

/** A generic object descriptor common to all AMM object types in an Agent.
 */
typedef struct
{
    /// Indication of whether this object has an enumeration assigned
    bool has_enum;
    /// Optional integer enumeration for this object if #has_enum is true
    int64_t intenum;
    /// Mandatory name for this object
    string_t name;

    /// Formal parameters of this object instance, which may be empty
    ari_formal_param_list_t fparams;

    /** Object-type-specific application data.
     */
    cace_amm_user_data_t app_data;

} cace_amm_obj_desc_t;

void cace_amm_obj_desc_init(cace_amm_obj_desc_t *obj);

void cace_amm_obj_desc_deinit(cace_amm_obj_desc_t *obj);

#define M_OPL_cace_amm_obj_desc_t() (INIT(API_2(cace_amm_obj_desc_init)), CLEAR(API_2(cace_amm_obj_desc_deinit)))

#ifdef __cplusplus
} // extern C
#endif

#endif /* CACE_AMM_OBJ_DESC_H_ */
