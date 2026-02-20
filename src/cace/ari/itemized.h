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
/** @file
 * @ingroup ari
 * This file contains the definitions, prototypes, constants necessary
 * to store and lookup an itemized set of ARIs.
 * See ::cace_ari_itemized_t.
 */
#ifndef CACE_ARI_ITEMIZED_H_
#define CACE_ARI_ITEMIZED_H_

#include "base.h"
#include "containers.h"
#include "cace/config.h"
#include <m-dict.h>

#ifdef __cplusplus
extern "C" {
#endif

/** @struct cace_ari_itemized_dict
 * Dictionary from case-sensitive name to external ARI pointer
 */
/// @cond Doxygen_Suppress
// GCOV_EXCL_START
M_DICT_DEF2(cace_named_ari_ptr_dict, const char *, M_CSTR_OPLIST, cace_ari_t *, M_PTR_OPLIST)
// GCOV_EXCL_STOP
/// @endcond

/** A itemized (indexed and named) set of ARIs.
 */
typedef struct
{
    /// Lookup by ordinal index
    cace_ari_array_t ordered;
    /// Lookup by (case-sensitive) text name
    cace_named_ari_ptr_dict_t named;
    /// Indication of any item being the undefined value
    bool any_undefined;
} cace_ari_itemized_t;

/** Initialize a new empty set.
 * @param[in,out] obj The set to
 */
void cace_ari_itemized_init(cace_ari_itemized_t *obj);

/** Initializer with copy semantics.
 */
void cace_ari_itemized_init_set(cace_ari_itemized_t *obj, const cace_ari_itemized_t *src);

/** Initializer with move semantics.
 */
void cace_ari_itemized_init_move(cace_ari_itemized_t *obj, cace_ari_itemized_t *src);

/** State de-initializer.
 */
void cace_ari_itemized_deinit(cace_ari_itemized_t *obj);

/** Clear out any parameters present.
 *
 * @param[in,out] obj The struct to clear.
 */
void cace_ari_itemized_reset(cace_ari_itemized_t *obj);

#ifdef __cplusplus
} // extern C
#endif

#endif /* CACE_AMM_PARAMETERS_H_ */
