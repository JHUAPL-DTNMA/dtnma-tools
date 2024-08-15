/*
 * Copyright (c) 2011-2023 The Johns Hopkins University Applied Physics
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
#ifndef ARI_ARI_BASE_H_
#define ARI_ARI_BASE_H_

#include "ari_type.h"
#include "ari_data.h"
#include "ari_config.h"
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

/** Represent an identifier component of an ARI.
 * It is used by object reference ARI for component values.
 */
typedef struct
{
    /// Control which union member to use
    enum
    {
        /// Default null value
        ARI_IDSEG_NULL = 0,
        /// A selector for ari_idseg_t::as_int
        ARI_IDSEG_INT,
        /// A selector for ari_idseg_t::as_text
        ARI_IDSEG_TEXT,
    } form;
    /// Choice of identifier value
    union
    {
        /// Integer enumeration. Must fit within 64-bit signed int.
        int64_t as_int;
        /// Text string data
        string_t as_text;
    };
} ari_idseg_t;

/** Initialize a null ID segment.
 *
 * @param[out] idseg The segment to initialize.
 */
void ari_idseg_init(ari_idseg_t *idseg);

/** Initialize an ID segment from any text.
 *
 * @param[out] idseg The segment to initialize.
 * @param[in,out] text The text to take from and clear.
 * @return Zero upon success.
 */
int ari_idseg_init_from_text(ari_idseg_t *idseg, string_t text);

void ari_idseg_deinit(ari_idseg_t *obj);

void ari_idseg_copy(ari_idseg_t *obj, const ari_idseg_t *src);

size_t ari_idseg_hash(const ari_idseg_t *obj);

int ari_idseg_cmp(const ari_idseg_t *left, const ari_idseg_t *right);

bool ari_idseg_equal(const ari_idseg_t *left, const ari_idseg_t *right);

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
    ari_data_t as_data;

    /// Used when ari_lit_t::prim_type is ::ARI_PRIM_TIMESPEC
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
 */
int ari_lit_deinit(ari_lit_t *obj);

int ari_lit_copy(ari_lit_t *lit, const ari_lit_t *src);

/** The entire object path as part of a full obj_ref_t.
 * Keeping this separate allows using just the identity portion for searching.
 */
typedef struct
{
    ari_idseg_t ns_id;
    ari_idseg_t type_id;
    ari_idseg_t obj_id;

    /// True if #ari_type is valid
    bool has_ari_type;
    /// Derived ARI type of the referenced object
    ari_type_t ari_type;
} ari_objpath_t;

void ari_objpath_init(ari_objpath_t *obj);
void ari_objpath_deinit(ari_objpath_t *obj);
void ari_objpath_copy(ari_objpath_t *obj, const ari_objpath_t *src);

/** Derive the ARI Type of a referenced object.
 * This sets the ari_objpath_t::ari_type based on the ari_objpath_t::type_id
 * value.
 *
 * @param[in,out] path The object to update.
 * @return Zero upon success.
 * @post Also marks ari_objpath_t::has_ari_type to indicate success.
 */
int ari_objpath_derive_type(ari_objpath_t *path);

/// Indicate the presence of parameters in ari_params_t
enum ari_param_state_e
{
    ARI_PARAMS_NONE,
    ARI_PARAMS_AC,
    ARI_PARAMS_AM,
};

/** Given parameter storage for an ari_ref_t.
 */
typedef struct
{
    /// Determine which member in the union is present
    enum ari_param_state_e state;
    /// The actual parameter data
    union
    {
        /// Used when #state is ::ARI_PARAMS_AC
        struct ari_ac_s *as_ac;
        /// Used when #state is ::ARI_PARAMS_AM
        struct ari_am_s *as_am;
    };
} ari_params_t;

int ari_params_deinit(ari_params_t *obj);

int ari_params_copy(ari_params_t *obj, const ari_params_t *src);

/** Represent an object reference ARI.
 *
 * A regular ARI identifies an object whose value is not itself captured in the
 * identifier. Put another way, a regular ARI is one what identifies anything'
 * other than a literal value.
 *
 * Nicknames, Issuers, and Tags are stored in databases on the Agent and the
 * Manager and the ARI structure stores an index into these databases. This is
 * done for space compression. For example, a nickname may be up to 64 bits in
 * length whereas an index into the NN database can be 16 bits. This results in
 * an average savings of 18 bytes for ARIs that use nicknames, issuers, and
 * 64-bit tags.
 *
 * The name field is an unparsed bytestring whose value is determined by naming
 * rules for the ADM or user that defines the object being identified.
 *
 * \todo: Nicknames, Issuers, Tags, and Names might all more efficiently by
 *        stored as an index into a tree structure, such as a radix tree.
 */
typedef struct
{
    /// Path of the object being referenced
    ari_objpath_t objpath;
    /// Optional parameters for a reference
    ari_params_t params;
} ari_ref_t;

void ari_ref_deinit(ari_ref_t *obj);

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
 * @param[in,out] ari The value to modify.
 */
void ari_init(ari_t *ari);

/** @overload
 * The source data is moved into the value.
 *
 * @param[in,out] ari The value to modify.
 * @return A pointer to the literal struct.
 */
ari_lit_t *ari_init_lit(ari_t *ari);

/** @overload
 * The source data is moved into the value.
 *
 * @param[in,out] ari The value to modify.
 * @return A pointer to the object reference struct.
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

int ari_set_copy(ari_t *ari, const ari_t *src);

int ari_set_move(ari_t *ari, ari_t *src);

#ifdef __cplusplus
}
#endif

#endif /* ARI_ARI_BASE_H_ */
