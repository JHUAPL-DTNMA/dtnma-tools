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
#ifndef CACE_AMM_OBJ_DESC_H_
#define CACE_AMM_OBJ_DESC_H_

#include "idseg_val.h"
#include "user_data.h"
#include "parameters.h"
#include "status.h"
#include "cace/ari.h"
#include "cace/util/nocase.h"

#ifdef __cplusplus
extern "C" {
#endif

/** A generic object descriptor common to all AMM object types in an Agent.
 */
typedef struct cace_amm_obj_desc_s
{
    /// Identifier for this object within the NS-type container
    cace_amm_idseg_val_t obj_id;

    /// Formal parameters of this object instance, which may be empty
    cace_amm_formal_param_list_t fparams;

    /// The status of this object, which can change over time
    cace_amm_status_t status;

    /** Object-type-specific application data.
     */
    cace_amm_user_data_t app_data;

#ifdef __cplusplus
    cace_amm_obj_desc_s(const cace_amm_obj_desc_s &)            = delete;
    cace_amm_obj_desc_s &operator=(const cace_amm_obj_desc_s &) = delete;
#endif

} cace_amm_obj_desc_t;

void cace_amm_obj_desc_init(cace_amm_obj_desc_t *obj);

void cace_amm_obj_desc_deinit(cace_amm_obj_desc_t *obj);

#define M_OPL_cace_amm_obj_desc_t() \
    (INIT(API_2(cace_amm_obj_desc_init)), INIT_SET(0), SET(0), CLEAR(API_2(cace_amm_obj_desc_deinit)))

#ifdef __cplusplus
} // extern C
#endif

#endif /* CACE_AMM_OBJ_DESC_H_ */
