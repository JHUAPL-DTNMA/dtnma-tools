/*
 * Copyright (c) 2011-2026 The Johns Hopkins University Applied Physics
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
#ifndef CACE_AMM_NAMED_TYPE_H_
#define CACE_AMM_NAMED_TYPE_H_

#include "typing.h"
#include <m-string.h>
#include <m-array.h>

#ifdef __cplusplus
extern "C" {
#endif

/// A named semantic type
typedef struct
{
    /// The unique name of the column.
    m_string_t name;

    /** The type of the column.
     * All type references are fully recursively resolved.
     * The type object is owned by this column.
     */
    cace_amm_type_t typeobj;
} cace_amm_named_type_t;

void cace_amm_named_type_init(cace_amm_named_type_t *obj);

void cace_amm_named_type_deinit(cace_amm_named_type_t *obj);

/// OPLIST for the cace_amm_named_type_s
#define M_OPL_cace_amm_named_type_t() (INIT(API_2(cace_amm_named_type_init)), CLEAR(API_2(cace_amm_named_type_deinit)))

/** @struct cace_amm_named_type_array_t
 * An array of cace_amm_named_type_t instances.
 */
/// @cond Doxygen_Suppress
M_ARRAY_DEF(cace_amm_named_type_array, cace_amm_named_type_t)
/// @endcond

#ifdef __cplusplus
}
#endif

#endif /* CACE_AMM_NAMED_TYPE_H_ */
