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

typedef bool     ari_bool;
typedef uint8_t  ari_byte;
typedef uint32_t ari_uint;
typedef int32_t  ari_int;
typedef uint64_t ari_uvast;
typedef int64_t  ari_vast;
typedef float    ari_real32;
typedef double   ari_real64;

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
 * ::ari_prim_val_u union.
 */
enum ari_prim_type_e
{
    /** A singleton primitive type with no specific value.
     * This special case has no corresponding ::ari_type_t enumeration.
     */
    ARI_PRIM_UNDEFINED = 0,
    /// A singleton primitive associated with ::ARI_TYPE_NULL
    ARI_PRIM_NULL,
    /// A selector for ari_prim_val_u::as_bool associated with ::ARI_TYPE_BOOL
    ARI_PRIM_BOOL,
    /// A selector for ari_prim_val_u::as_uint64 associated with ::ARI_TYPE_BYTE ::ARI_TYPE_UINT and ::ARI_TYPE_UVAST
    ARI_PRIM_UINT64,
    /// A selector for ari_prim_val_u::as_int64 associated with ::ARI_TYPE_INT and ::ARI_TYPE_VAST
    ARI_PRIM_INT64,
    /// A selector for ari_prim_val_u::as_float64 associated with ::ARI_TYPE_REAL32 and ::ARI_TYPE_REAL64
    ARI_PRIM_FLOAT64,
    /// A selector for ari_prim_val_u::as_data associated with ::ARI_TYPE_TEXTSTR
    ARI_PRIM_TSTR,
    /// A selector for ari_prim_val_u::as_data associated with ::ARI_TYPE_BYTESTR
    ARI_PRIM_BSTR,
    /// A selector for ari_prim_val_u::as_data associated with ::ARI_TYPE_TP and ::ARI_TYPE_TD
    ARI_PRIM_TIMESPEC,
    /// Some other type which requires an ari_lit_t::ari_type identifier
    ARI_PRIM_OTHER,
};

/** Possible values types within an ari_lit_t::value.
 */
union ari_prim_val_u
{
    // Primitive values in this group
    /// Used when ari_lit_t::prim_type is ::ARI_PRIM_BOOL
    ari_bool as_bool;
    /// Used when ari_lit_t::prim_type is ::ARI_PRIM_UINT64
    uint64_t as_uint64;
    /// Used when ari_lit_t::prim_type is ::ARI_PRIM_INT64 or ::ARI_TYPE_ARITYPE
    int64_t as_int64;
    /// Used when ari_lit_t::prim_type is ::ARI_PRIM_FLOAT64
    ari_real64 as_float64;
    /** Used when ari_lit_t::prim_type is ::ARI_PRIM_TSTR or ::ARI_PRIM_BSTR,
     * or when ari_lit_t::ari_type is ::ARI_TYPE_LABEL or ::ARI_TYPE_CBOR.
     */
    cace_data_t as_data;

    /** Used when ari_lit_t::prim_type is ::ARI_PRIM_TIMESPEC.
     * For ::ARI_TYPE_TP this timespec represents the offset from the
     * DTN Time epoch of @cite rfc9171.
     */
    struct timespec as_timespec;

    // Container values in this group
    /// Used when ari_lit_t::ari_type is ::ARI_TYPE_AC
    struct ari_ac_s *as_ac;
    /// Used when ari_lit_t::ari_type is ::ARI_TYPE_AM
    struct ari_am_s *as_am;
    /// Used when ari_lit_t::ari_type is ::ARI_TYPE_TBL
    struct ari_tbl_s *as_tbl;
    /// Used when ari_lit_t::ari_type is ::ARI_TYPE_EXECSET
    struct ari_execset_s *as_execset;
    /// Used when ari_lit_t::ari_type is ::ARI_TYPE_RPTSET
    struct ari_rptset_s *as_rptset;
};

/** Represent a literal-value ARI.
 *
 */
typedef struct
{
    /// True if #ari_type is valid
    bool has_ari_type;
    /// Derived ARI type of this value
    ari_type_t ari_type;

    /// Primitive type present in the union
    enum ari_prim_type_e prim_type;
    /// The specific literal value keyed by #prim_type or #ari_type
    union ari_prim_val_u value;
} ari_lit_t;

/** De-initialize a literal ARI struct.
 * This recurses into data and containers as necessary.
 *
 * @param[in,out] obj The object to affect.
 * @return Zero if successful.
 */
int ari_lit_deinit(ari_lit_t *obj);

/** Copy a literal struct by-value.
 *
 * @param[in,out] obj The object to affect.
 * @param[in] src The source to copy from, recursively if necessary.
 * @return Zero if successful.
 */
int ari_lit_copy(ari_lit_t *obj, const ari_lit_t *src);

#ifdef __cplusplus
}
#endif

#endif /* CACE_ARI_LIT_H_ */
