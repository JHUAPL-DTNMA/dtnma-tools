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
 * This file contains functions for internal API to access ARI values
 * in a consistent way.
 */
#ifndef CACE_ARI_ACCESS_H_
#define CACE_ARI_ACCESS_H_

#include "base.h"
#include "containers.h"

#ifdef __cplusplus
extern "C" {
#endif

/** Determine if a value is the @c undefined value.
 *
 * @param[in] ari The value to check, or a NULL pointer.
 * @return True if the ARI is valid and is the undefined value.
 * @sa ARI_UNDEFINED cace_ari_init()
 */
bool cace_ari_is_undefined(const cace_ari_t *ari);

/** Determine if a value is not the @c undefined value.
 * This can be useful as a CTRL or OPER precondition.
 *
 * @param[in] ari The value to check, or a NULL pointer.
 * @return True if the ARI is valid and is not the undefined value.
 * @sa ARI_UNDEFINED cace_ari_init()
 */
bool cace_ari_not_undefined(const cace_ari_t *ari);

/** Set the ARI value to be undefined.
 *
 * @param[in,out] ari The value to set, which should not be NULL pointer.
 */
void cace_ari_set_undefined(cace_ari_t *ari);

/** Determine if this is a typed or untyped null value.
 *
 * @param[in] ari The value to check.
 * @sa cace_ari_set_null()
 */
bool cace_ari_is_null(const cace_ari_t *ari);

/** Set the ARI value to be untyped null.
 *
 * @param[in,out] ari The value to set.
 */
void cace_ari_set_null(cace_ari_t *ari);

/** Determine if this ARI contains a primitive boolean type.
 *
 * @param[in] ari Non-null pointer to the ARI to read.
 * @return True if the primitive is boolean.
 */
bool cace_ari_is_bool(const cace_ari_t *ari);

/** Extract a boolean value, if present, from an ARI.
 * This works for both typed and untyped literal values.
 *
 * @param[in] ari Non-null pointer to the ARI to read.
 * @param[out] out The value to output.
 * @return Zero upon success.
 */
int cace_ari_get_bool(const cace_ari_t *ari, cace_ari_bool *out);

/** Extract an integer value, if present, from an ARI.
 * This works for both typed and untyped literal values and will coerce signed and unsigned values to the desired type
 * where possible.
 *
 * @param[in] ari Non-null pointer to the ARI to read.
 * @param[out] out The value to output.
 * @return Zero upon success.
 */
int cace_ari_get_int(const cace_ari_t *ari, cace_ari_int *out);

/// @overload
int cace_ari_get_uint(const cace_ari_t *ari, cace_ari_uint *out);

/// @overload
int cace_ari_get_byte(const cace_ari_t *ari, cace_ari_byte *out);

/// @overload
int cace_ari_get_vast(const cace_ari_t *ari, cace_ari_vast *out);

/// @overload
int cace_ari_get_uvast(const cace_ari_t *ari, cace_ari_uvast *out);

/** @overload
 * The output in this case is in the DTN time epoch.
 */
int cace_ari_get_tp(const cace_ari_t *ari, struct timespec *out);

/** @overload
 * The output in this case is in the POSIX time epoch.
 */
int cace_ari_get_tp_posix(const cace_ari_t *ari, struct timespec *out);

/// @overload
int cace_ari_get_td(const cace_ari_t *ari, struct timespec *out);

/** Determine if this is a typed literal of a specific type.
 *
 * @param[in] ari The value to check.
 * @param typ The type to compare with.
 * @return True if the value is of that type.
 */
bool cace_ari_is_lit_typed(const cace_ari_t *ari, cace_ari_type_t typ);

/** Require an ARITYPE literal value.
 *
 * @param[in] ari The ARI to read.
 * @return Pointer to the contained type value, if present, otherwise NULL.
 */
const int64_t *cace_ari_get_aritype(const cace_ari_t *ari);

/** Set an ARI as an untyped literal value.
 *
 * @param[in,out] ari The ARI to set the value on.
 * This must have been initialized.
 * @param src The primitive value to set to.
 */
void cace_ari_set_prim_bool(cace_ari_t *ari, cace_ari_bool src);

/// @overload
void cace_ari_set_prim_uint64(cace_ari_t *ari, uint64_t src);

/// @overload
void cace_ari_set_prim_int64(cace_ari_t *ari, int64_t src);

/// @overload
void cace_ari_set_prim_float64(cace_ari_t *ari, cace_ari_real64 src);

/** Set an ARI as a typed literal value.
 *
 * @param[in,out] ari The ARI to set the value on.
 * This must have been initialized.
 * @param src The value to set to.
 */
void cace_ari_set_bool(cace_ari_t *ari, cace_ari_bool src);

/// @overload
void cace_ari_set_byte(cace_ari_t *ari, cace_ari_byte src);

/// @overload
void cace_ari_set_int(cace_ari_t *ari, cace_ari_int src);

/// @overload
void cace_ari_set_uint(cace_ari_t *ari, cace_ari_uint src);

/// @overload
void cace_ari_set_vast(cace_ari_t *ari, cace_ari_vast src);

/// @overload
void cace_ari_set_uvast(cace_ari_t *ari, cace_ari_uvast src);

/// @overload
void cace_ari_set_real32(cace_ari_t *ari, cace_ari_real32 src);

/// @overload
void cace_ari_set_real64(cace_ari_t *ari, cace_ari_real64 src);

/// @overload
void cace_ari_set_tstr(cace_ari_t *ari, const char *buf, bool copy);

/// @overload
void cace_ari_set_bstr(cace_ari_t *ari, cace_data_t *src, bool copy);

/** @overload
 * The input in this case is in the DTN time epoch.
 */
void cace_ari_set_tp(cace_ari_t *ari, struct timespec dtntime);
/** @overload
 * The input in this case is in the POSIX time epoch.
 */
void cace_ari_set_tp_posix(cace_ari_t *ari, struct timespec ptime);

/// @overload
void cace_ari_set_td(cace_ari_t *ari, struct timespec delta);

/// @overload
void cace_ari_set_aritype(cace_ari_t *ari, cace_ari_type_t type);
/// @overload
void cace_ari_set_aritype_text(cace_ari_t *ari, cace_ari_type_t type);

/** Set the ARI as an AC, optionally moving values from an external source.
 *
 * @param[in,out] ari The ARI value to modify.
 * @param[in,out] src An optional existing value struct to move from.
 * @return The new AC value struct.
 */
struct cace_ari_ac_s *cace_ari_set_ac(cace_ari_t *ari, struct cace_ari_ac_s *src);

/** Set the ARI as an AM, optionally moving values from an external source.
 *
 * @param[in,out] ari The ARI value to modify.
 * @param[in,out] src An optional existing value struct to move from.
 * @return The new AC value struct.
 */
struct cace_ari_am_s *cace_ari_set_am(cace_ari_t *ari, struct cace_ari_am_s *src);

/** Set the ARI as a TBL, optionally moving values from an external source.
 *
 * @param[in,out] ari The ARI value to modify.
 * @param[in,out] src An optional existing value struct to move from.
 * @return The new AC value struct.
 */
struct cace_ari_tbl_s *cace_ari_set_tbl(cace_ari_t *ari, struct cace_ari_tbl_s *src);

/** Require a TEXTSTR value and get the pointer to its storage.
 *
 * @param[in] ari The ARI to read.
 * @return Pointer to the contained data, if present, otherwise NULL.
 * @notice This data will always have a terminating null byte.
 */
const cace_data_t *cace_ari_cget_tstr(const cace_ari_t *ari);
/** @overload
 * This form casts to const C-string pointer.
 */
const char *cace_ari_cget_tstr_cstr(const cace_ari_t *ari);
/** @overload
 * This form casts to mutable C-string pointer.
 */
const char *cace_ari_get_tstr_cstr(cace_ari_t *ari);

/// @overload
const cace_data_t *cace_ari_cget_bstr(const cace_ari_t *ari);

/** Require an AC value and extract a pointer to its item list.
 *
 * @param[in] ari The ARI to read.
 * @return Pointer to the contained AC struct, if present, otherwise NULL.
 */
struct cace_ari_ac_s *cace_ari_get_ac(cace_ari_t *ari);
/// @overload
const struct cace_ari_ac_s *cace_ari_cget_ac(const cace_ari_t *ari);

/** Require an AM value and extract a pointer to its item list.
 *
 * @param[in] ari The ARI to read.
 * @return Pointer to the contained AM struct, if present, otherwise NULL.
 */
struct cace_ari_am_s *cace_ari_get_am(cace_ari_t *ari);
/// @overload
const struct cace_ari_am_s *cace_ari_cget_am(const cace_ari_t *ari);

struct cace_ari_tbl_s *cace_ari_get_tbl(cace_ari_t *ari);
/// @overload
const struct cace_ari_tbl_s *cace_ari_cget_tbl(const cace_ari_t *ari);

/** Require an EXECSET value and extract a pointer to its struct.
 *
 * @param[in] ari The ARI to read.
 * @return Pointer to the contained struct, if present, otherwise NULL.
 */
struct cace_ari_execset_s *cace_ari_get_execset(cace_ari_t *ari);
/// @overload
const struct cace_ari_execset_s *cace_ari_cget_execset(const cace_ari_t *ari);

/** Set an ARI as an EXECSET literal value.
 * Any previous value is de-initialized.
 *
 * @param[out] ari The value to set.
 * @return The new execset struct.
 */
struct cace_ari_execset_s *cace_ari_set_execset(cace_ari_t *ari);

/** Require an RPTSET value and extract a pointer to its struct.
 *
 * @param[in] ari The ARI to read.
 * @return Pointer to the contained struct, if present, otherwise NULL.
 */
struct cace_ari_rptset_s *cace_ari_get_rptset(cace_ari_t *ari);
/// @overload
const struct cace_ari_rptset_s *cace_ari_cget_rptset(const cace_ari_t *ari);

/** Set an ARI as an RPTSET literal value.
 * Any previous value is de-initialized.
 *
 * @param[out] ari The value to set.
 * @return The new rptset struct.
 */
struct cace_ari_rptset_s *cace_ari_set_rptset(cace_ari_t *ari);

/** Convenience setter.
 */
static inline cace_ari_ref_t *cace_ari_set_objref_path_intid(cace_ari_t *ari, cace_ari_int_id_t org_id,
                                                             cace_ari_int_id_t model_id, cace_ari_type_t type_id,
                                                             cace_ari_int_id_t obj_id)
{
    cace_ari_ref_t *ref = cace_ari_set_objref(ari);
    cace_ari_objpath_set_intid(&(ref->objpath), org_id, model_id, type_id, obj_id);
    return ref;
}
/// @overload
static inline cace_ari_ref_t *cace_ari_set_objref_path_textid(cace_ari_t *ari, const char *org_id, const char *model_id,
                                                              cace_ari_type_t type_id, const char *obj_id)
{
    cace_ari_ref_t *ref = cace_ari_set_objref(ari);
    cace_ari_objpath_set_textid(&(ref->objpath), org_id, model_id, type_id, obj_id);
    return ref;
}

#ifdef __cplusplus
}
#endif

#endif /* CACE_ARI_ACCESS_H_ */
