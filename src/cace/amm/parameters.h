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
#ifndef CACE_AMM_PARAMETERS_H_
#define CACE_AMM_PARAMETERS_H_

#include "typing.h"
#include "cace/ari.h"
#include "cace/ari/itemized.h"
#include "cace/config.h"

#ifdef __cplusplus
extern "C" {
#endif

/** Definition of a formal parameter (from an ADM).
 * These are used to resolve given parameters into actual parameters.
 */
typedef struct
{
    /// The unique name of the parameter.
    string_t name;
    /// The list index (ordinal) of the parameter.
    size_t index;

    /** The type of the parameter.
     * All type references are fully recursively resolved.
     * The type object is owned by this formal parameter.
     */
    cace_amm_type_t typeobj;
    /// Optional default value, or the undefined value
    cace_ari_t defval;
} cace_amm_formal_param_t;

void cace_amm_formal_param_init(cace_amm_formal_param_t *obj);

void cace_amm_formal_param_deinit(cace_amm_formal_param_t *obj);

#define M_OPL_cace_amm_formal_param_t() \
    (INIT(API_2(cace_amm_formal_param_init)), CLEAR(API_2(cace_amm_formal_param_deinit)))

/// @cond Doxygen_Suppress
DEQUE_DEF(cace_amm_formal_param_list, cace_amm_formal_param_t)
/// @endcond

/** Populate actual parameters.
 *
 * @param[in,out] obj The struct to initialize.
 * @param[in] fparams Formal parameters to normalize to.
 * @param[in] gparams Given parameters to normalize from.
 * @return Zero upon success.
 */
int cace_amm_actual_param_set_populate(cace_ari_itemized_t *obj, const cace_amm_formal_param_list_t fparams,
                                       const cace_ari_params_t *gparams);

#ifdef __cplusplus
} // extern C
#endif

#endif /* CACE_AMM_PARAMETERS_H_ */
