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
#ifndef CACE_AMM_OBJ_BASE_H_
#define CACE_AMM_OBJ_BASE_H_

#include "cace/ari.h"
#include "parameters.h"
#include "cace/casestr.h"

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
 * The text name lookup is case-insensitive.
 *
 * @param otname The object type name (e.g. "edd")
 */
#define CACE_AMM_OBJTYPE_DEFINE_STORAGE(otname)                                                                       \
    DEQUE_DEF(M_C3(cace_amm_, otname, _desc_list), M_C3(cace_amm_, otname, _desc_t), CACE_AMM_OBJTYPE_OPLIST(otname)) \
    M_DICT_DEF2(M_C3(cace_amm_, otname, _desc_by_enum), int64_t, M_BASIC_OPLIST, M_C3(cace_amm_, otname, _desc_t) *,  \
                M_PTR_OPLIST)                                                                                         \
    M_DICT_DEF2(M_C3(cace_amm_, otname, _desc_by_name), const char *, M_CASESTR_OPLIST,                               \
                M_C3(cace_amm_, otname, _desc_t) *, M_PTR_OPLIST)

/** Declare AMM object container uses within an ADM.
 *
 * @param otname The object type name (e.g. "edd")
 */
#define CACE_AMM_OBJTYPE_USE_STORAGE(otname)                        \
    M_C3(cace_amm_, otname, _desc_list_t) M_C(otname, _list);       \
    M_C3(cace_amm_, otname, _desc_by_enum_t) M_C(otname, _by_enum); \
    M_C3(cace_amm_, otname, _desc_by_name_t) M_C(otname, _by_name);

typedef struct
{
    /** Original path dereferenced to the object being produced from.
     * All path segments are in their original form.
     */
    ari_objpath_t objpath;
    /** Actual parameters normalized for this object from the given parameters.
     */
    ari_actual_param_set_t aparams;
    /** Storage for the produced value.
     * This is initialized as undefined and must be set to any other value
     * to indicate successful production.
     */
    ari_t value;
} cace_amm_edd_ctx_t;

/** A CONST descriptor.
 * This defines the properties of a CONST in an Agent and includes common
 * object metadata.
 */
typedef struct
{
    CACE_AMM_OBJTYPE_BASE_MEMBER

    /** Storage for the constant value.
     * This is initialized as undefined and must be set to any other value
     * to indicate successful production.
     */
    ari_t value;

} cace_amm_const_desc_t;

static inline void cace_amm_const_desc_init(cace_amm_const_desc_t *obj)
{
    cace_amm_obj_desc_init(&(obj->base));
    obj->value = ARI_INIT_UNDEFINED;
}

static inline void cace_amm_const_desc_deinit(cace_amm_const_desc_t *obj)
{
    cace_amm_obj_desc_deinit(&(obj->base));
    memset(obj, 0, sizeof(*obj));
}

/// Define functions and structures for ::cace_amm_const_desc_t use
CACE_AMM_OBJTYPE_DEFINE_STORAGE(const)

/** An EDD descriptor.
 * This defines the properties of an EDD in an Agent and includes common
 * object metadata.
 */
typedef struct
{
    CACE_AMM_OBJTYPE_BASE_MEMBER

    /** Value production callback for this object.
     *
     * @param ctx The production context, including result storage.
     */
    void (*produce)(cace_amm_edd_ctx_t *ctx);
} cace_amm_edd_desc_t;

static inline void cace_amm_edd_desc_init(cace_amm_edd_desc_t *obj)
{
    cace_amm_obj_desc_init(&(obj->base));
    obj->produce = NULL;
}

static inline void cace_amm_edd_desc_deinit(cace_amm_edd_desc_t *obj)
{
    cace_amm_obj_desc_deinit(&(obj->base));
    memset(obj, 0, sizeof(*obj));
}

/// Define functions and structures for ::cace_amm_edd_desc_t use
CACE_AMM_OBJTYPE_DEFINE_STORAGE(edd)

typedef struct
{
    CACE_AMM_OBJTYPE_USE_STORAGE(const)
    CACE_AMM_OBJTYPE_USE_STORAGE(edd)
} cace_amm_adm_desc_t;

#ifdef __cplusplus
} // extern C
#endif

#endif /* CACE_AMM_OBJ_BASE_H_ */
