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
/** @file
 * @ingroup ari
 * This file contains the definitions for segments containing int/text
 * identifiers.
 */
#ifndef CACE_ARI_IDSEG_H_
#define CACE_ARI_IDSEG_H_

#include "type.h"
#include "cace/config.h"
#include <m-string.h>
#include <stdint.h>
#include <stdbool.h>
#include <time.h>

#ifdef __cplusplus
extern "C" {
#endif

/// Integer type for object reference parts
typedef int64_t cace_ari_int_id_t;

/** Represent an identifier component of an ARI.
 * It is used by object reference ARI for component values.
 */
typedef struct
{
    /// Control which union member to use
    enum
    {
        /// Default null value
        CACE_ARI_IDSEG_NULL = 0,
        /// A selector for cace_ari_idseg_t::as_int
        CACE_ARI_IDSEG_INT,
        /// A selector for cace_ari_idseg_t::as_text
        CACE_ARI_IDSEG_TEXT,
    } form;
    /// Choice of identifier value
    union
    {
        /// Integer enumeration. Must fit within 64-bit signed int.
        cace_ari_int_id_t as_int;
        /// Text string data
        m_string_t as_text;
    };
} cace_ari_idseg_t;

/** Initialize a null ID segment.
 *
 * @param[out] obj The segment to initialize.
 */
void cace_ari_idseg_init(cace_ari_idseg_t *obj);

void cace_ari_idseg_deinit(cace_ari_idseg_t *obj);

void cace_ari_idseg_copy(cace_ari_idseg_t *obj, const cace_ari_idseg_t *src);

size_t cace_ari_idseg_hash(const cace_ari_idseg_t *obj);

int cace_ari_idseg_cmp(const cace_ari_idseg_t *left, const cace_ari_idseg_t *right);

bool cace_ari_idseg_equal(const cace_ari_idseg_t *left, const cace_ari_idseg_t *right);

/** Initialize an ID segment from any text.
 *
 * @param[out] obj The segment to initialize.
 * @param[in,out] text The text to take from and clear.
 */
void cace_ari_idseg_init_text(cace_ari_idseg_t *obj, string_t text);

/** Derive an integer value if the segment contains text that matches an integer pattern.
 * @param[in,out] idset The object to update.
 */
void cace_ari_idseg_derive_form(cace_ari_idseg_t *idset);

#ifdef __cplusplus
}
#endif

#endif /* CACE_ARI_IDSEG_H_ */
