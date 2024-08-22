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
#ifndef CACE_AMM_OBJ_BASE_H_
#define CACE_AMM_OBJ_BASE_H_

#include "cace/ari.h"
#include "parameters.h"
#include "cace/nocase.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
    /// Pointer to the opaque user data being managed
    void *ptr;

    /** An optional cleanup function for the #ptr.
     *
     * @param ptr The user data being de-initialized.
     */
    void (*deinit)(void *ptr);
} cace_amm_user_data_t;

/** A generic object descriptor common to all AMM object types in an Agent.
 */
typedef struct
{
    /// Indication of whether this object has an enumeration assigned
    bool has_enum;
    /// Optional integer enumeration for this object if #has_enum is true
    int64_t intenum;
    /// Mandatory name for this object
    string_t name;

    /// Formal parameters of this object instance, which may be empty
    ari_formal_param_list_t fparams;
} cace_amm_obj_desc_t;

void cace_amm_obj_desc_init(cace_amm_obj_desc_t *obj);

void cace_amm_obj_desc_deinit(cace_amm_obj_desc_t *obj);

/** General descriptor for this AMM object type.
 */
#define CACE_AMM_OBJTYPE_BASE_MEMBER cace_amm_obj_desc_t base;

/** Define OPLIST for AMM object type.
 *
 * @param otname The object type name (e.g. "edd")
 */
#define CACE_AMM_OBJTYPE_OPLIST(otname) \
    (INIT(API_2(M_C3(cace_amm_, otname, _desc_init))), CLEAR(API_2(M_C3(cace_amm_, otname, _desc_deinit))))

/** Define AMM object instance containers.
 *
 * The @c _list container is used to actually store and manage the lifetime of
 * object descriptors.
 *
 * The @c _by_enum and @c _by_name dictionaries are used for lookup from integer
 * or text (C-string) identifiers respectively to pointer-to-descriptor.
 *
 * The text name lookup is case-insensitive.
 *
 * @param otname The object type name (e.g. "edd")
 */
#define CACE_AMM_OBJTYPE_DEFINE_STORAGE(otname)                                                                       \
    DEQUE_DEF(M_C3(cace_amm_, otname, _desc_list), M_C3(cace_amm_, otname, _desc_t), CACE_AMM_OBJTYPE_OPLIST(otname)) \
    M_DICT_DEF2(M_C3(cace_amm_, otname, _desc_by_enum), int64_t, M_BASIC_OPLIST, M_C3(cace_amm_, otname, _desc_t) *,  \
                M_PTR_OPLIST)                                                                                         \
    M_DICT_DEF2(M_C3(cace_amm_, otname, _desc_by_name), const char *, M_CSTR_NOCASE_OPLIST,                           \
                M_C3(cace_amm_, otname, _desc_t) *, M_PTR_OPLIST)

/** Declare AMM object container uses within an ADM.
 *
 * @param otname The object type name (e.g. "edd")
 */
#define CACE_AMM_OBJTYPE_USE_STORAGE(otname)                        \
    M_C3(cace_amm_, otname, _desc_list_t) M_C(otname, _list);       \
    M_C3(cace_amm_, otname, _desc_by_enum_t) M_C(otname, _by_enum); \
    M_C3(cace_amm_, otname, _desc_by_name_t) M_C(otname, _by_name);

#ifdef __cplusplus
} // extern C
#endif

#endif /* CACE_AMM_OBJ_BASE_H_ */
