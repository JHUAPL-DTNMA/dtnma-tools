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
 * This file contains definitions constraints on semantic type use.
 */
#ifndef CACE_AMM_SEMTYPE_USE_H_
#define CACE_AMM_SEMTYPE_USE_H_

#include "range.h"
#include "cace/config.h"
#include "cace/ari.h"
#if defined(PCRE_FOUND)
#include <pcre2.h>
#endif /* PCRE_FOUND */

#ifdef __cplusplus
extern "C" {
#endif

/** Types of constraints on a amm_semtype_use_t.
 * This library does not implement constraints for:
 *  * text pattern regular expressions
 *  * integer enumerated values
 *  * integer bitwise values
 *  * CBOR CDDL
 */
enum amm_semtype_cnst_type_e
{
    /// An initial invalid type with no associated value
    AMM_SEMTYPE_CNST_INVALID,
    /// Text- or byte-string length range
    AMM_SEMTYPE_CNST_STRLEN,
#if defined(PCRE_FOUND)
    /// Text-string pattern expression
    AMM_SEMTYPE_CNST_TEXTPAT,
#endif /* PCRE_FOUND */
    /// A signed 64-bit integer multi-interval range
    AMM_SEMTYPE_CNST_RANGE_INT64,
};

/** A single constraint on a amm_semtype_use_t
 */
typedef struct amm_semtype_cnst_s
{
    /// The type of constraint present
    enum amm_semtype_cnst_type_e type;

    union
    {
        /// Used when #type is ::AMM_SEMTYPE_CNST_STRLEN
        cace_amm_range_size_t as_strlen;
#if defined(PCRE_FOUND)
        /// Used when #type is ::AMM_SEMTYPE_CNST_TEXTPAT
        pcre2_code *as_textpat;
#endif /* PCRE_FOUND */
        /// Used when #type is ::AMM_SEMTYPE_CNST_RANGE_INT64
        cace_amm_range_int64_t as_range_int64;
    };
} amm_semtype_cnst_t;

void amm_semtype_cnst_init(amm_semtype_cnst_t *obj);

void amm_semtype_cnst_deinit(amm_semtype_cnst_t *obj);

/** Configure a constraint on text-string or byte-string size.
 * This applies to ARI_TYPE_TEXTSTR and ARI_TYPE_BYTESTR as well as untyped
 * primitive text- and byte-strings.
 *
 * @param[in,out] obj The struct to set the state of.
 * @return The specific parameters for this constraint type.
 */
cace_amm_range_size_t *amm_semtype_cnst_set_strlen(amm_semtype_cnst_t *obj);

#if defined(PCRE_FOUND)

/** Configure a constraint on text-string regular expression pattern.
 * This applies to ARI_TYPE_TEXTSTR as well as untyped
 * primitive text strings.
 *
 * @note The @c pat parameter is the start- and end-anchored PCRE pattern,
 * which conforms to the I-Regexp standard of RFC 9485 @cite rfc9485.
 * This pattern does not need to include leading "\A(?" or trailing ")\z" parts.
 *
 *
 * @param[in,out] obj The struct to set the state of.
 * @param[in] pat The regular expression to compile.
 * @return The specific parameters for this constraint type.
 */
pcre2_code *amm_semtype_cnst_set_textpat(amm_semtype_cnst_t *obj, const char *pat);

#endif /* PCRE_FOUND */

/** Configure a constraint on integer values based on signed 64-bit ranges.
 * This applies to ARI_TYPE_BYTE, ARI_TYPE_INT, ARI_TYPE_UINT, ARI_TYPE_VAST,
 * and a limited domain of ARI_TYPE_UVAST as well as untyped primitive
 * integer values.
 *
 * @param[in,out] obj The struct to set the state of.
 * @return The specific parameters for this constraint type.
 */
cace_amm_range_int64_t *amm_semtype_cnst_set_range_int64(amm_semtype_cnst_t *obj);

/** Determine if a specific value is valid according to a constraint.
 *
 * @param[in] obj The constraint to check against.
 * @param[in] val The value to check.
 * @return True if the value is valid.
 */
bool amm_semtype_cnst_is_valid(const amm_semtype_cnst_t *obj, const ari_t *val);

/// M*LIB OPLIST for amm_semtype_cnst_t
#define M_OPL_amm_semtype_cnst_t() (INIT(API_2(amm_semtype_cnst_init)), CLEAR(API_2(amm_semtype_cnst_deinit)))

#ifdef __cplusplus
}
#endif

#endif /* CACE_AMM_SEMTYPE_USE_H_ */
