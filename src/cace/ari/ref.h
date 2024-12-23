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
 * This file contains the definitions for ARI object reference paths
 * and their segments.
 */
#ifndef CACE_ARI_REF_H_
#define CACE_ARI_REF_H_

#include "type.h"
#include "cace/config.h"
#include <m-string.h>
#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

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

/** The entire object path as part of a full obj_ref_t.
 * Keeping this separate allows using just the identity portion for searching.
 */
typedef struct
{
    /// The namespace-id segment of the path
    ari_idseg_t ns_id;
    /// The object-type segment of the path
    ari_idseg_t type_id;
    /// The object-id segment of the path
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

/** Set the ARI as an object reference with a specific text-named path.
 *
 * @param[in,out] path The path to modify.
 * @param[in] ns_id The namespace path segment.
 * @param type_id The object type path segment.
 * @param[in] obj_id The object ID path segment.
 */
void ari_objpath_set_textid(ari_objpath_t *path, const char *ns_id, ari_type_t type_id, const char *obj_id);

/** Set the ARI as an object reference with a specific text-named path.
 *
 * @param[in,out] path The path to modify.
 * @param[in] ns_id The namespace path segment.
 * @param type_id The object type path segment.
 * @param[in] obj_id The object ID path segment.
 */
void ari_objpath_set_textid(ari_objpath_t *path, const char *ns_id, ari_type_t type_id, const char *obj_id);

/** Set the ARI as an object reference with a specific text-named path.
 *
 * @param[in,out] path The path to modify.
 * @param[in] ns_id The namespace path segment, or NULL for none.
 * @param type_id The object type path segment, or NULL for none. The pointed-to lifetime does not need to outlast this
 * function call.
 * @param[in] obj_id The object ID path segment, or NULL for none.
 */
void ari_objpath_set_textid_opt(ari_objpath_t *path, const char *ns_id, const ari_type_t *type_id, const char *obj_id);

/** Set the ARI as an object reference with a specific integer-enumerated path.
 *
 * @param[in,out] path The path to modify.
 * @param ns_id The namespace path segment.
 * @param type_id The object type path segment.
 * @param obj_id The object ID path segment.
 */
void ari_objpath_set_intid(ari_objpath_t *path, int64_t ns_id, ari_type_t type_id, int64_t obj_id);

/** Set the ARI as an object reference with a specific integer-enumerated path.
 *
 * @param[in,out] path The path to modify.
 * @param ns_id The namespace path segment, or NULL for none. The pointed-to lifetime does not need to outlast this
 * function call.
 * @param type_id The object type path segment, or NULL for none. The pointed-to lifetime does not need to outlast this
 * function call.
 * @param obj_id The object ID path segment, or NULL for none. The pointed-to lifetime does not need to outlast this
 * function call.
 */
void ari_objpath_set_intid_opt(ari_objpath_t *path, const int64_t *ns_id, const ari_type_t *type_id,
                               const int64_t *obj_id);

// forward declarations
struct ari_ac_s;
struct ari_am_s;
struct ari_list_s;
struct ari_tree_s;

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

/** Set the parameters to a specific state.
 *
 * @param[in,out] obj The state to modify, which must already be
 * initialized.
 * @param[in,out] src An optional existing value struct to move from.
 */
void ari_params_set_ac(ari_params_t *obj, struct ari_list_s *src);
/// @overload
void ari_params_set_am(ari_params_t *obj, struct ari_tree_s *src);

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

int ari_ref_copy(ari_ref_t *obj, const ari_ref_t *src);

#ifdef __cplusplus
}
#endif

#endif /* CACE_ARI_REF_H_ */
