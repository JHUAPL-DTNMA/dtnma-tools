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
/** @file
 * @ingroup ari
 * This file contains the definitions, prototypes, constants, and
 * other information necessary for the identification and
 * processing of AMM Resource Identifiers (ARIs). Every object in
 * the AMM can be uniquely identified using an ARI.
 */
#ifndef CACE_ARI_BASE_H_
#define CACE_ARI_BASE_H_

#include "type.h"
#include "lit.h"
#include "ref.h"
#include "cace/config.h"
#include "cace/cace_data.h"
#include <m-string.h>
#include <m-list.h>
#include <m-dict.h>
#include <time.h>
#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Defines a general-purpose ARI structure.
 *
 * The ARI being captured here can be either a "regular" ARI,
 * in which case the structure should be interpreted as a
 * cace_ari_reg_t structure or a "literal" ARI, in which case the
 * structure can be interpreted as a type/name/value.
 *
 * The use of a separate type field is redundant in that type
 * information is already captured in both the "regular" ARI
 * flag and the TYPE part of a literal ARI. However, extracting
 * the type information makes processing simpler and any other
 * method of distinguishing a regular and literal ARI would
 * likely use up at least a byte of space anyway.
 */

typedef struct cace_ari_s
{
    bool is_ref;
    union
    {
        cace_ari_lit_t as_lit;
        cace_ari_ref_t as_ref;
    };
} cace_ari_t;

/** A static value to initialize an ARI to the undefined value.
 * This can be used as an alternative to cace_ari_init() for static initialization.
 */
#define CACE_ARI_INIT_UNDEFINED                                            \
    (cace_ari_t)                                                           \
    {                                                                      \
        .is_ref = false, .as_lit = {.prim_type = CACE_ARI_PRIM_UNDEFINED } \
    }

#define CACE_ARI_INIT_NULL                                            \
    (cace_ari_t)                                                      \
    {                                                                 \
        .is_ref = false, .as_lit = {.prim_type = CACE_ARI_PRIM_NULL } \
    }

/** Initialize an ARI to the undefined value.
 *
 * @param[out] ari The value to modify.
 */
void cace_ari_init(cace_ari_t *ari);

/** Initialize an ARI to be a literal value which requires further state
 * initialization.
 *
 * @param[out] ari The value to modify.
 * @return A pointer to the literal struct to set state in.
 */
cace_ari_lit_t *cace_ari_init_lit(cace_ari_t *ari);

/** Initialize an ARI to be an object reference value which requires further state
 * initialization.
 *
 * @param[out] ari The value to modify.
 * @return A pointer to the object reference struct to set state in.
 */
cace_ari_ref_t *cace_ari_init_objref(cace_ari_t *ari);

/** Initialize an ARI with copy semantics.
 *
 * @param[in,out] ari The value to modify.
 * @param src The source to deep copy from.
 */
int cace_ari_init_copy(cace_ari_t *ari, const cace_ari_t *src);

/** Initialize an ARI with move semantics.
 *
 * @param[in,out] ari The value to modify.
 * @param[in,out] src The source to move from and reset.
 */
int cace_ari_init_move(cace_ari_t *ari, cace_ari_t *src);

/** De-initialize an ARI.
 * This recurses into data and containers as necessary.
 *
 * @param[in,out] ari The value to de-initialize.
 * @post The ARI is left as the undefined value.
 */
int cace_ari_deinit(cace_ari_t *ari);

/** Reset an initialized ARI to the undefined value.
 * This is equivalent to calling cace_ari_deinit() and cace_ari_init() in sequence.
 *
 * @param[in,out] ari The value to reset.
 * @post The ARI is left as the undefined value.
 */
void cace_ari_reset(cace_ari_t *ari);

/** Set an ARI to be an object reference value which requires further state
 * setting.
 *
 * @param[in,out] ari The value to modify.
 * @return A pointer to the object reference struct to set state in.
 */
cace_ari_ref_t *cace_ari_set_objref(cace_ari_t *ari);

/** Copy an ARI value into another ARI.
 *
 * @param[in,out] ari The ARI to modify. The previous value is de-initialized prior to copy.
 * @param src The source to deep copy from.
 */
int cace_ari_set_copy(cace_ari_t *ari, const cace_ari_t *src);

/** Move an ARI value into another ARI.
 *
 * @param[in,out] ari The ARI to modify. The previous value is de-initialized prior to move.
 * @param[in,out] src The source to move from and reset.
 * @post Source ARI is left as the undefined value.
 */
int cace_ari_set_move(cace_ari_t *ari, cace_ari_t *src);

#ifdef __cplusplus
}
#endif

#endif /* CACE_ARI_BASE_H_ */
