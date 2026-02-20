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
 * This file contains the declarations, prototypes, constants necessary
 * to store reference patterns and match them with objects.
 */
#ifndef CACE_ARI_OBJPAT_H_
#define CACE_ARI_OBJPAT_H_

#include "base.h"
#include "cace/util/range.h"
#include "cace/config.h"
#include <m-string.h>
#include <m-variant.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/** @struct cace_ari_objpat_part
 * A variant with value of field:
 *  - `special` with value of true to indicate wildcard.
 *  - `range_int64` with value of an entire range to match id-int.
 *  - `text` with value of single text string to match id-text.
 */
/// @cond Doxygen_Suppress
// GCOV_EXCL_START
M_VARIANT_DEF2(cace_ari_objpat_part, (special, bool), (range_int64, cace_util_range_int64_t), (text, m_string_t))
// GCOV_EXCL_STOP
/// @endcond

/** A itemized (indexed and named) set of ARIs.
 */
typedef struct cace_ari_objpat_s
{
    /// Pattern for organization-id segment
    cace_ari_objpat_part_t org_pat;
    /// Pattern for model-id segment
    cace_ari_objpat_part_t model_pat;
    /// Pattern for object-type segment
    cace_ari_objpat_part_t type_pat;
    /// Pattern for object-id segment
    cace_ari_objpat_part_t obj_pat;
} cace_ari_objpat_t;

/** Initialize a new empty set.
 * @param[in,out] obj The set to
 */
void cace_ari_objpat_init(cace_ari_objpat_t *obj);

/** Initializer with copy semantics.
 */
void cace_ari_objpat_init_set(cace_ari_objpat_t *obj, const cace_ari_objpat_t *src);

/** Initializer with move semantics.
 */
void cace_ari_objpat_init_move(cace_ari_objpat_t *obj, cace_ari_objpat_t *src);

/** State de-initializer.
 */
void cace_ari_objpat_deinit(cace_ari_objpat_t *obj);

/** Clear out any parameters present.
 *
 * @param[in,out] obj The struct to clear.
 */
void cace_ari_objpat_reset(cace_ari_objpat_t *obj);

cace_ari_objpat_t *cace_ari_lit_init_objpat(cace_ari_lit_t *lit);

#ifdef __cplusplus
} // extern C
#endif

#endif /* CACE_AMM_OBJPAT_H_ */
