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
 * This file contains the definitions for literal-value ARI sub-structure.
 */
#ifndef CACE_ARI_LIT_H_
#define CACE_ARI_LIT_H_

#include "type.h"
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

typedef bool     cace_ari_bool;
typedef uint8_t  cace_ari_byte;
typedef uint32_t cace_ari_uint;
typedef int32_t  cace_ari_int;
typedef uint64_t cace_ari_uvast;
typedef int64_t  cace_ari_vast;
typedef float    cace_ari_real32;
typedef double   cace_ari_real64;

/** Convenience definition of the DTN Time epoch offset from POSIX time epoch.
 * This is in units of seconds.
 * DTN Time epoch is defined in @cite rfc9171 at 2000-01-01T00:00:00Z
 */
#define CACE_ARI_DTN_EPOCH 946684800L

/** The value of CACE_ARI_DTN_EPOCH in a `time_t`.
 */
extern const time_t cace_ari_dtn_epoch;

/** The value of CACE_ARI_DTN_EPOCH in a `struct timespec`.
 */
extern const struct timespec cace_ari_dtn_epoch_timespec;

/** These are enumerations for internal switching of the
 * ::cace_ari_prim_val_u union.
 */
enum cace_ari_prim_type_e
{
    /** A singleton primitive type with no specific value.
     * This special case has no corresponding ::cace_ari_type_t enumeration.
     */
    CACE_ARI_PRIM_UNDEFINED = 0,
    /// A singleton primitive associated with ::CACE_ARI_TYPE_NULL
    CACE_ARI_PRIM_NULL,
    /// A selector for cace_ari_prim_val_u::as_bool associated with ::CACE_ARI_TYPE_BOOL
    CACE_ARI_PRIM_BOOL,
    /// A selector for cace_ari_prim_val_u::as_uint64 associated with ::CACE_ARI_TYPE_BYTE ::CACE_ARI_TYPE_UINT and
    /// ::CACE_ARI_TYPE_UVAST
    CACE_ARI_PRIM_UINT64,
    /// A selector for cace_ari_prim_val_u::as_int64 associated with ::CACE_ARI_TYPE_INT and ::CACE_ARI_TYPE_VAST
    CACE_ARI_PRIM_INT64,
    /// A selector for cace_ari_prim_val_u::as_float64 associated with ::CACE_ARI_TYPE_REAL32 and ::CACE_ARI_TYPE_REAL64
    CACE_ARI_PRIM_FLOAT64,
    /// A selector for cace_ari_prim_val_u::as_data associated with ::CACE_ARI_TYPE_TEXTSTR
    CACE_ARI_PRIM_TSTR,
    /// A selector for cace_ari_prim_val_u::as_data associated with ::CACE_ARI_TYPE_BYTESTR
    CACE_ARI_PRIM_BSTR,
    /// A selector for cace_ari_prim_val_u::as_data associated with ::CACE_ARI_TYPE_TP and ::CACE_ARI_TYPE_TD
    CACE_ARI_PRIM_TIMESPEC,
    /// Some other type which requires an cace_ari_lit_t::ari_type identifier
    CACE_ARI_PRIM_OTHER,
};

/** Possible values types within an cace_ari_lit_t::value.
 */
union cace_ari_prim_val_u
{
    // Primitive values in this group
    /// Used when cace_ari_lit_t::prim_type is ::CACE_ARI_PRIM_BOOL
    cace_ari_bool as_bool;
    /// Used when cace_ari_lit_t::prim_type is ::CACE_ARI_PRIM_UINT64
    uint64_t as_uint64;
    /// Used when cace_ari_lit_t::prim_type is ::CACE_ARI_PRIM_INT64 or ::CACE_ARI_TYPE_ARITYPE
    int64_t as_int64;
    /// Used when cace_ari_lit_t::prim_type is ::CACE_ARI_PRIM_FLOAT64
    cace_ari_real64 as_float64;
    /** Used when cace_ari_lit_t::prim_type is ::CACE_ARI_PRIM_TSTR or ::CACE_ARI_PRIM_BSTR,
     * or when cace_ari_lit_t::ari_type is ::CACE_ARI_TYPE_LABEL or ::CACE_ARI_TYPE_CBOR.
     */
    cace_data_t as_data;

    /** Used when cace_ari_lit_t::prim_type is ::CACE_ARI_PRIM_TIMESPEC.
     * For ::CACE_ARI_TYPE_TP this timespec represents the offset from the
     * DTN Time epoch of @cite rfc9171.
     */
    struct timespec as_timespec;

    // Container values in this group
    /// Used when cace_ari_lit_t::ari_type is ::CACE_ARI_TYPE_AC
    struct cace_ari_ac_s *as_ac;
    /// Used when cace_ari_lit_t::ari_type is ::CACE_ARI_TYPE_AM
    struct cace_ari_am_s *as_am;
    /// Used when cace_ari_lit_t::ari_type is ::CACE_ARI_TYPE_TBL
    struct cace_ari_tbl_s *as_tbl;
    /// Used when cace_ari_lit_t::ari_type is ::CACE_ARI_TYPE_EXECSET
    struct cace_ari_execset_s *as_execset;
    /// Used when cace_ari_lit_t::ari_type is ::CACE_ARI_TYPE_RPTSET
    struct cace_ari_rptset_s *as_rptset;
};

/** Represent a literal-value ARI.
 *
 */
typedef struct
{
    /// True if #ari_type is valid
    bool has_ari_type;
    /// Derived ARI type of this value
    cace_ari_type_t ari_type;

    /// Primitive type present in the union
    enum cace_ari_prim_type_e prim_type;
    /// The specific literal value keyed by #prim_type or #ari_type
    union cace_ari_prim_val_u value;
} cace_ari_lit_t;

/** De-initialize a literal ARI struct.
 * This recurses into data and containers as necessary.
 *
 * @param[in,out] obj The object to affect.
 * @return Zero if successful.
 */
int cace_ari_lit_deinit(cace_ari_lit_t *obj);

/** Copy a literal struct by-value.
 *
 * @param[in,out] obj The object to affect.
 * @param[in] src The source to copy from, recursively if necessary.
 * @return Zero if successful.
 */
int cace_ari_lit_copy(cace_ari_lit_t *obj, const cace_ari_lit_t *src);

#ifdef __cplusplus
}
#endif

#endif /* CACE_ARI_LIT_H_ */
