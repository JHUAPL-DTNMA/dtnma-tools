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

/** Represent a decoded date value.
 */
typedef struct
{
    /// True if #parts is valid
    bool valid;
    /// Decoded date-only parts
    struct tm parts;
} cace_ari_date_t;

void cace_ari_date_init(cace_ari_date_t *obj);
void cace_ari_date_deinit(cace_ari_date_t *obj);
void cace_ari_date_copy(cace_ari_date_t *obj, const cace_ari_date_t *src);
int  cace_ari_date_cmp(const cace_ari_date_t *left, const cace_ari_date_t *right);

/** A helper function to decode date text.
 *
 * @param[out] obj The object to decode into, setting its cace_ari_date_t#valid state depending on decoding success.
 * @param[in] text The text to decode.
 */
int cace_ari_date_from_text(cace_ari_date_t *obj, const char *text);

/** The entire object path as part of a full obj_ref_t.
 * Keeping this separate allows using just the identity portion for searching.
 */
typedef struct
{
    /// The organization-id segment of the path
    cace_ari_idseg_t org_id;
    /// The model-id segment of the path
    cace_ari_idseg_t model_id;
    /// Date from 1970-01-01 epoch
    cace_ari_date_t model_rev;
    /// The object-type segment of the path
    cace_ari_idseg_t type_id;
    /// The object-id segment of the path
    cace_ari_idseg_t obj_id;

    /// True if #ari_type is valid
    bool has_ari_type;
    /// Derived ARI type of the referenced object
    cace_ari_type_t ari_type;
} cace_ari_objpath_t;

void cace_ari_objpath_init(cace_ari_objpath_t *obj);
void cace_ari_objpath_deinit(cace_ari_objpath_t *obj);
void cace_ari_objpath_copy(cace_ari_objpath_t *obj, const cace_ari_objpath_t *src);

/** Derive the ARI Type of a referenced object.
 * This sets the cace_ari_objpath_t::ari_type based on the cace_ari_objpath_t::type_id
 * value.
 *
 * @param[in,out] path The object to update.
 * @return Zero upon success.
 * @post Also marks cace_ari_objpath_t::has_ari_type to indicate success.
 */
int cace_ari_objpath_derive_type(cace_ari_objpath_t *path);

/** Set the ARI as an object reference with a specific text-named path.
 *
 * @param[in,out] path The path to modify.
 * @param[in] org_id The organization ID.
 * @param[in] model_id The model ID.
 * @param type_id The object type ID.
 * @param[in] obj_id The object ID.
 */
void cace_ari_objpath_set_textid(cace_ari_objpath_t *path, const char *org_id, const char *model_id,
                                 cace_ari_type_t type_id, const char *obj_id);

/** Set the ARI as an object reference with a specific text-named path.
 * For all pointer arguments, the pointed-to lifetime does not need to outlast this function call.
 * Pointed-to values are copied into the path.
 *
 * @param[in,out] path The path to modify.
 * @param[in] org_id The organization ID, or NULL for none.
 * @param[in] model_id The model ID, or NULL for none.
 * @param type_id The object type ID, or NULL for none.
 * @param[in] obj_id The object ID, or NULL for none.
 */
void cace_ari_objpath_set_textid_opt(cace_ari_objpath_t *path, const char *org_id, const char *model_id,
                                     const cace_ari_type_t *type_id, const char *obj_id);

/** Set the ARI as an object reference with a specific integer-enumerated path.
 *
 * @param[in,out] path The path to modify.
 * @param[in] org_id The organization ID.
 * @param[in] model_id The model ID.
 * @param type_id The object type ID.
 * @param[in] obj_id The object ID.
 */
void cace_ari_objpath_set_intid(cace_ari_objpath_t *path, cace_ari_int_id_t org_id, cace_ari_int_id_t model_id,
                                cace_ari_type_t type_id, cace_ari_int_id_t obj_id);

/** Set the ARI as an object reference with a specific integer-enumerated path.
 * For all pointer arguments, the pointed-to lifetime does not need to outlast this function call.
 * Pointed-to values are copied into the path.
 *
 * @param[in,out] path The path to modify.
 * @param[in] org_id The organization ID, or NULL for none.
 * @param[in] model_id The model ID, or NULL for none.
 * @param type_id The object type ID, or NULL for none.
 * @param[in] obj_id The object ID, or NULL for none.
 */
void cace_ari_objpath_set_intid_opt(cace_ari_objpath_t *path, const cace_ari_int_id_t *org_id,
                                    const cace_ari_int_id_t *model_id, const cace_ari_type_t *type_id,
                                    const cace_ari_int_id_t *obj_id);

// forward declarations
struct cace_ari_ac_s;
struct cace_ari_am_s;
struct cace_ari_list_s;
struct cace_ari_tree_s;

/// Indicate the presence of parameters in cace_ari_params_t
enum cace_ari_param_state_e
{
    CACE_ARI_PARAMS_NONE,
    CACE_ARI_PARAMS_AC,
    CACE_ARI_PARAMS_AM,
};

/** Given parameter storage for an cace_ari_ref_t.
 */
typedef struct
{
    /// Determine which member in the union is present
    enum cace_ari_param_state_e state;
    /// The actual parameter data
    union
    {
        /// Used when #state is ::CACE_ARI_PARAMS_AC
        struct cace_ari_ac_s *as_ac;
        /// Used when #state is ::CACE_ARI_PARAMS_AM
        struct cace_ari_am_s *as_am;
    };
} cace_ari_params_t;

int cace_ari_params_deinit(cace_ari_params_t *obj);

int cace_ari_params_copy(cace_ari_params_t *obj, const cace_ari_params_t *src);

/** Set the parameters to a specific state.
 *
 * @param[in,out] obj The state to modify, which must already be
 * initialized.
 * @param[in,out] src An optional existing value struct to move from.
 */
void cace_ari_params_set_ac(cace_ari_params_t *obj, struct cace_ari_list_s *src);
/// @overload
void cace_ari_params_set_am(cace_ari_params_t *obj, struct cace_ari_tree_s *src);

/** Represent an object reference or namespace reference ARI in accordance with
 * @cite ietf-dtn-ari-06.
 *
 * A object reference ARI identifies an object whose state is not itself captured in the identifier.
 * Put another way, a reference ARI is one what identifies anything other than a literal value.
 *
 * This struct is always contained within a ::cace_ari_s.
 */
typedef struct
{
    /// Path of the object being referenced
    cace_ari_objpath_t objpath;
    /// Optional parameters for a reference
    cace_ari_params_t params;
} cace_ari_ref_t;

void cace_ari_ref_deinit(cace_ari_ref_t *obj);

int cace_ari_ref_copy(cace_ari_ref_t *obj, const cace_ari_ref_t *src);

#ifdef __cplusplus
}
#endif

#endif /* CACE_ARI_REF_H_ */
