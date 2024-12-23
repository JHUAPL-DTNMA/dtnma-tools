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
#include "ref.h"
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
 * ari_reg_t structure or a "literal" ARI, in which case the
 * structure can be interpreted as a type/name/value.
 *
 * The use of a separate type field is redundant in that type
 * information is already captured in both the "regular" ARI
 * flag and the TYPE part of a literal ARI. However, extracting
 * the type information makes processing simpler and any other
 * method of distinguishing a regular and literal ARI would
 * likely use up at least a byte of space anyway.
 */

typedef struct ari_s
{
    bool is_ref;
    union
    {
        ari_lit_t as_lit;
        ari_ref_t as_ref;
    };
} ari_t;

/** A static value to initialize an ARI to the undefined value.
 * This can be used as an alternative to ari_init() for static initialization.
 */
#define ARI_INIT_UNDEFINED                                            \
    (ari_t)                                                           \
    {                                                                 \
        .is_ref = false, .as_lit = {.prim_type = ARI_PRIM_UNDEFINED } \
    }

#define ARI_INIT_NULL                                            \
    (ari_t)                                                      \
    {                                                            \
        .is_ref = false, .as_lit = {.prim_type = ARI_PRIM_NULL } \
    }

/** Initialize an ARI to the undefined value.
 *
 * @param[out] ari The value to modify.
 */
void ari_init(ari_t *ari);

/** Initialize an ARI to be a literal value which requires further state
 * initialization.
 *
 * @param[out] ari The value to modify.
 * @return A pointer to the literal struct to set state in.
 */
ari_lit_t *ari_init_lit(ari_t *ari);

/** Initialize an ARI to be an object reference value which requires further state
 * initialization.
 *
 * @param[out] ari The value to modify.
 * @return A pointer to the object reference struct to set state in.
 */
ari_ref_t *ari_init_objref(ari_t *ari);

/** Initialize an ARI with copy semantics.
 *
 * @param[in,out] ari The value to modify.
 * @param src The source to deep copy from.
 */
int ari_init_copy(ari_t *ari, const ari_t *src);

/** Initialize an ARI with move semantics.
 *
 * @param[in,out] ari The value to modify.
 * @param[in,out] src The source to move from and reset.
 */
int ari_init_move(ari_t *ari, ari_t *src);

/** De-initialize an ARI.
 * This recurses into data and containers as necessary.
 *
 * @param[in,out] ari The value to de-initialize.
 * @post The ARI is left as the undefined value.
 */
int ari_deinit(ari_t *ari);

/** Reset an initialized ARI to the undefined value.
 * This is equivalent to calling ari_deinit() and ari_init() in sequence.
 *
 * @param[in,out] ari The value to reset.
 * @post The ARI is left as the undefined value.
 */
void ari_reset(ari_t *ari);

/** Set an ARI to be an object reference value which requires further state
 * setting.
 *
 * @param[in,out] ari The value to modify.
 * @return A pointer to the object reference struct to set state in.
 */
ari_ref_t *ari_set_objref(ari_t *ari);

/** Copy an ARI value into another ARI.
 *
 * @param[in,out] ari The ARI to modify. The previous value is de-initialized prior to copy.
 * @param src The source to deep copy from.
 */
int ari_set_copy(ari_t *ari, const ari_t *src);

/** Move an ARI value into another ARI.
 *
 * @param[in,out] ari The ARI to modify. The previous value is de-initialized prior to move.
 * @param[in,out] src The source to move from and reset.
 * @post Source ARI is left as the undefined value.
 */
int ari_set_move(ari_t *ari, ari_t *src);

#ifdef __cplusplus
}
#endif

#endif /* CACE_ARI_BASE_H_ */
