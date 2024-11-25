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

/** Determine if this is the @c undefined value.
 * @sa ARI_UNDEFINED ari_init()
 */
bool ari_is_undefined(const ari_t *ari);

/** Force the ARI value to be undefined.
 */
void ari_set_undefined(ari_t *ari);

bool ari_is_null(const ari_t *ari);

void ari_set_null(ari_t *ari);

/** Determine if this ARI contains a primitive boolean type.
 *
 * @param ari Non-null pointer to the ARI to read.
 * @return True if the primitive is boolean.
 */
bool ari_is_bool(const ari_t *ari);

/** Extract a boolean value, if present, from an ARI.
 *
 * @param ari Non-null pointer to the ARI to read.
 * @param[out] out The value to output.
 * @return Zero upon success.
 */
int ari_get_bool(const ari_t *ari, ari_bool *out);

void ari_set_bool(ari_t *ari, ari_bool src);

/** Set an ARI as an untyped literal value.
 *
 * @param[in,out] ari The ARI to set the value on.
 * This must have been initialized.
 * @param src The primitive value to set to.
 */
void ari_set_prim_uint64(ari_t *ari, uint64_t src);

/// @overload
void ari_set_prim_int64(ari_t *ari, int64_t src);

/// @overload
void ari_set_prim_float64(ari_t *ari, ari_real64 src);

int ari_get_int(ari_t *ari, ari_int *out);

void ari_set_int(ari_t *ari, ari_int src);

int ari_get_uint(ari_t *ari, ari_uint *out);

void ari_set_uint(ari_t *ari, ari_uint src);

void ari_set_vast(ari_t *ari, ari_vast src);

void ari_set_uvast(ari_t *ari, ari_uvast src);

int ari_get_uvast(const ari_t *ari, ari_uvast *out);

void ari_set_real64(ari_t *ari, ari_real64 src);

void ari_set_tstr(ari_t *ari, const char *buf, bool copy);

void ari_set_bstr(ari_t *ari, cace_data_t *src, bool copy);

void ari_set_tp(ari_t *ari, struct timespec dtntime);

void ari_set_td(ari_t *ari, struct timespec delta);

/** Require an AC value and extract a pointer to its item list.
 *
 * @param[in] ari The ARI to read.
 * @return Pointer to the contained AC struct, if present, otherwise NULL.
 */
struct ari_ac_s *ari_get_ac(const ari_t *ari);

/** Set the ARI as an AC, optionally moving values from an external source.
 *
 * @param[in,out] ari The ARI value to modify.
 * @param[in,out] src An optional existing value struct to move from.
 */
void ari_set_ac(ari_t *ari, struct ari_ac_s *src);

void ari_set_am(ari_t *ari, struct ari_am_s *src);

struct ari_tbl_s *ari_get_tbl(const ari_t *ari);

void ari_set_tbl(ari_t *ari, struct ari_tbl_s *src);

/** Require an EXECSET value and extract a pointer to its struct.
 *
 * @param[in] ari The ARI to read.
 * @return Pointer to the contained struct, if present, otherwise NULL.
 */
struct ari_execset_s *ari_get_execset(const ari_t *ari);

/** Set an ARI as an EXECSET literal value.
 * Any previous value is de-initialized.
 *
 * @param[out] ari The value to set.
 * @return The new execset struct.
 */
struct ari_execset_s *ari_set_execset(ari_t *ari);

/** Require an RPTSET value and extract a pointer to its struct.
 *
 * @param[in] ari The ARI to read.
 * @return Pointer to the contained struct, if present, otherwise NULL.
 */
struct ari_rptset_s *ari_get_rptset(const ari_t *ari);

/** Set an ARI as an RPTSET literal value.
 * Any previous value is de-initialized.
 *
 * @param[out] ari The value to set.
 * @return The new rptset struct.
 */
struct ari_rptset_s *ari_set_rptset(ari_t *ari);

/** Set the ARI as an object reference with a specific text-named path.
 *
 * @param[in,out] ari The ARI value to modify.
 * @param[in] ns_id The namespace path segment.
 * @param type_id The object type path segment.
 * @param[in] obj_id The object ID path segment.
 */
void ari_set_objref_path_textid(ari_t *ari, const char *ns_id, ari_type_t type_id, const char *obj_id);

/** Set the ARI as an object reference with a specific text-named path.
 *
 * @param[in,out] ari The ARI value to modify.
 * @param[in] ns_id The namespace path segment, or NULL for none.
 * @param type_id The object type path segment, or NULL for none. The pointed-to lifetime does not need to outlast this
 * function call.
 * @param[in] obj_id The object ID path segment, or NULL for none.
 */
void ari_set_objref_path_textid_opt(ari_t *ari, const char *ns_id, const ari_type_t *type_id, const char *obj_id);

/** Set the ARI as an object reference with a specific integer-enumerated path.
 *
 * @param[in,out] ari The ARI value to modify.
 * @param ns_id The namespace path segment.
 * @param type_id The object type path segment.
 * @param obj_id The object ID path segment.
 */
void ari_set_objref_path_intid(ari_t *ari, int64_t ns_id, ari_type_t type_id, int64_t obj_id);

/** Set the ARI as an object reference with a specific integer-enumerated path.
 *
 * @param[in,out] ari The ARI value to modify.
 * @param ns_id The namespace path segment, or NULL for none. The pointed-to lifetime does not need to outlast this
 * function call.
 * @param type_id The object type path segment, or NULL for none. The pointed-to lifetime does not need to outlast this
 * function call.
 * @param obj_id The object ID path segment, or NULL for none. The pointed-to lifetime does not need to outlast this
 * function call.
 */
void ari_set_objref_path_intid_opt(ari_t *ari, const int64_t *ns_id, const ari_type_t *type_id, const int64_t *obj_id);

/** Set just the parameters of an object reference ARI.
 *
 * @param[in,out] ari The ARI value to modify, which must already be
 * initialized as an object reference.
 * @param[in,out] src An optional existing value struct to move from.
 */
void ari_set_objref_params_ac(ari_t *ari, struct ari_ac_s *src);
/// @overload
void ari_set_objref_params_am(ari_t *ari, struct ari_am_s *src);

#ifdef __cplusplus
}
#endif

#endif /* CACE_ARI_ACCESS_H_ */
