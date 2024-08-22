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
#ifndef CACE_AMM_PARAMETERS_H_
#define CACE_AMM_PARAMETERS_H_

#include "cace/ari.h"
#include "typing.h"

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
    /// The fully recursively resolved type of the parameter.
    const amm_type_t *typeobj;
    /// Optional default value, or the undefined value
    ari_t defval;
} ari_formal_param_t;

void ari_formal_param_init(ari_formal_param_t *obj);

void ari_formal_param_deinit(ari_formal_param_t *obj);

#define M_OPL_ari_formal_param_t() (INIT(API_2(ari_formal_param_init)), CLEAR(API_2(ari_formal_param_deinit)))

/// @cond Doxygen_Suppress
DEQUE_DEF(ari_formal_param_list, ari_formal_param_t)
/// Dictionary from formal parameter name to external ARI
M_DICT_DEF2(named_ari_ptr_dict, const char *, M_CSTR_OPLIST, ari_t *, M_PTR_OPLIST)
/// @endcond

/** An actual parameter set normalized from given parameters
 * based on formal parameters.
 */
typedef struct
{
    /// Lookup parameter by index
    ari_list_t ordered;
    /// Lookup parameter by name
    named_ari_ptr_dict_t named;
} ari_actual_param_set_t;

void ari_actual_param_set_init(ari_actual_param_set_t *obj);

void ari_actual_param_set_deinit(ari_actual_param_set_t *obj);

/** Populate actual parameters.
 *
 * @param[out] obj The struct to initialize.
 * @param[in] fparams Formal parameters to normalize to.
 * @param[in] gparams Given parameters to normalize from.
 * @return Zero upon success.
 */
int ari_actual_param_set_populate(ari_actual_param_set_t *obj, const ari_formal_param_list_t fparams,
                                  const ari_params_t *gparams);

#ifdef __cplusplus
} // extern C
#endif

#endif /* CACE_AMM_PARAMETERS_H_ */
