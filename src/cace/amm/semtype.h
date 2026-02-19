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
 * This file contains definitions for AMM semantic typing of ARI values.
 */
#ifndef CACE_AMM_SEMTYPE_H_
#define CACE_AMM_SEMTYPE_H_

#include "typing.h"
#include "named_type.h"
#include "range.h"
#include "semtype_cnst.h"
#include <m-array.h>

#ifdef __cplusplus
extern "C" {
#endif

/// @cond Doxygen_Suppress
M_ARRAY_DEF(cace_amm_semtype_cnst_array, cace_amm_semtype_cnst_t)
/// @endcond

/// Configuration for an augmented use of another type
typedef struct
{
    /// Name of the type used
    cace_ari_t name;
    /** The type object being used, which is bound based on #name.
     * This is always a reference to an externally-owned object.
     */
    const cace_amm_type_t *base;

    /** Optional constraints on this use.
     * The order matches the source ADM but is not significant to the outcome.
     */
    cace_amm_semtype_cnst_array_t constraints;

} cace_amm_semtype_use_t;

static inline void cace_amm_semtype_use_init(cace_amm_semtype_use_t *obj)
{
    cace_ari_init(&(obj->name));
    obj->base = NULL;
    cace_amm_semtype_cnst_array_init(obj->constraints);
}

static inline void cace_amm_semtype_use_deinit(cace_amm_semtype_use_t *obj)
{
    cace_amm_semtype_cnst_array_clear(obj->constraints);
    cace_ari_deinit(&(obj->name));
    obj->base = NULL;
}

typedef struct cace_amm_lookup_s    cace_amm_lookup_t;
typedef struct cace_amm_obj_store_s cace_amm_obj_store_t;

/** Create a default initialized type use.
 * A use type adds annotations and constraints onto a base type.
 *
 * @param[out] type The type to initialize and populate.
 * @param[in] name The ARITYPE literal or TYPEDEF reference value.
 */
cace_amm_semtype_use_t *cace_amm_type_set_use(cace_amm_type_t *type);

/** Read a named config from its serialized name.
 *
 * @param[in,out] type The object to populate.
 * @param[in] deref The name to draw parameters from.
 * @return Zero if successful.
 */
int cace_amm_type_set_use_from_name(cace_amm_type_t *type, const cace_amm_lookup_t *deref,
                                    const cace_amm_obj_store_t *store);

/** Create a use type based on a type reference.
 *
 * @note This does not actually dereference the name, it only stores it.
 *
 * @param[out] type The type to initialize and populate.
 * @param[in] name The ARITYPE literal or TYPEDEF reference value.
 * @return Non-NULL upon success.
 */
cace_amm_semtype_use_t *cace_amm_type_set_use_ref(cace_amm_type_t *type, const cace_ari_t *name);
/** @overload
 * Set the name with move semantics.
 */
cace_amm_semtype_use_t *cace_amm_type_set_use_ref_move(cace_amm_type_t *type, cace_ari_t *name);

/** Create a use type based on a base type object.
 * A use type adds annotations and constraints onto a base type.
 * @warning This is intended only for internal out-of-agent testing.
 *
 * @param[out] type The type to initialize and populate.
 * @param ari_type The builtin type to create a use of.
 * @return Zero if successful.
 */
int cace_amm_type_set_use_builtin(cace_amm_type_t *type, cace_ari_type_t ari_type);

/// Configuration for a uniform list within an AC
typedef struct
{
    /** The type for each item of the list.
     * All type references are fully recursively resolved.
     * The type object is owned by this semtype.
     */
    cace_amm_type_t item_type;

    /** Constraint on the number of items.
     */
    cace_amm_range_intvl_size_t size;

} cace_amm_semtype_ulist_t;

static inline void cace_amm_semtype_ulist_init(cace_amm_semtype_ulist_t *obj)
{
    cace_amm_type_init(&(obj->item_type));
    cace_amm_range_intvl_size_set_infinite(&(obj->size));
}

static inline void cace_amm_semtype_ulist_deinit(cace_amm_semtype_ulist_t *obj)
{
    cace_amm_range_intvl_size_set_infinite(&(obj->size));
    cace_amm_type_deinit(&(obj->item_type));
}

/** Create a uniform list semantic type.
 *
 * @param[out] type The type to initialize and populate.
 * @return Non-NULL upon success.
 */
cace_amm_semtype_ulist_t *cace_amm_type_set_ulist(cace_amm_type_t *type);

int cace_amm_type_set_ulist_from_name(cace_amm_type_t *type, const cace_amm_lookup_t *deref,
                                      const cace_amm_obj_store_t *store);

/// Configuration for a diverse list within an AC
typedef struct
{
    /** The ordered list of semantic types for each sub-sequence of
     * the list.
     * All type references are fully recursively resolved.
     * The type object is owned by this semtype.
     */
    cace_amm_type_array_t types;

} cace_amm_semtype_dlist_t;

static inline void cace_amm_semtype_dlist_init(cace_amm_semtype_dlist_t *obj)
{
    cace_amm_type_array_init(obj->types);
}

static inline void cace_amm_semtype_dlist_deinit(cace_amm_semtype_dlist_t *obj)
{
    cace_amm_type_array_clear(obj->types);
}

/** Create a diverse list semantic type.
 *
 * @param[out] type The type to initialize and populate.
 * @param num_cols The number of types to initialize.
 * @return Non-NULL upon success.
 */
cace_amm_semtype_dlist_t *cace_amm_type_set_dlist(cace_amm_type_t *type, size_t num_types);

/** Read a diverse list config from its serialized name.
 *
 * @param[in,out] type The object to populate.
 * @param[in] deref The name to draw parameters from.
 * @return Zero if successful.
 */
int cace_amm_type_set_dlist_from_name(cace_amm_type_t *type, const cace_amm_lookup_t *deref,
                                      const cace_amm_obj_store_t *store);

/// Configuration for a uniform list within an AM
typedef struct
{
    /** The type for each key of the map.
     * All type references are fully recursively resolved.
     * The type object is owned by this semtype.
     */
    cace_amm_type_t key_type;
    /** The type for each value of the map.
     * All type references are fully recursively resolved.
     * The type object is owned by this semtype.
     */
    cace_amm_type_t val_type;

} cace_amm_semtype_umap_t;

static inline void cace_amm_semtype_umap_init(cace_amm_semtype_umap_t *obj)
{
    cace_amm_type_init(&(obj->key_type));
    cace_amm_type_init(&(obj->val_type));
}

static inline void cace_amm_semtype_umap_deinit(cace_amm_semtype_umap_t *obj)
{
    cace_amm_type_deinit(&(obj->val_type));
    cace_amm_type_deinit(&(obj->key_type));
}

/** Create a uniform map semantic type.
 *
 * @param[out] type The type to initialize and populate.
 * @return Non-NULL upon success.
 */
cace_amm_semtype_umap_t *cace_amm_type_set_umap(cace_amm_type_t *type);

/** Read a uniform map config from its serialized name.
 *
 * @param[in,out] type The object to populate.
 * @param[in] deref The name to draw parameters from.
 * @return Zero if successful.
 */
int cace_amm_type_set_umap_from_name(cace_amm_type_t *type, const cace_amm_lookup_t *deref,
                                     const cace_amm_obj_store_t *store);

/// Configuration for a table template
typedef struct
{
    /** The ordered list of types in this union.
     * Ownership of these objects is managed by the container.
     */
    cace_amm_named_type_array_t columns;
} cace_amm_semtype_tblt_t;

static inline void cace_amm_semtype_tblt_init(cace_amm_semtype_tblt_t *obj)
{
    cace_amm_named_type_array_init(obj->columns);
}

static inline void cace_amm_semtype_tblt_deinit(cace_amm_semtype_tblt_t *obj)
{
    cace_amm_named_type_array_clear(obj->columns);
}

/** Create a table template based on a set of typed columns.
 *
 * @param[out] type The type to initialize and populate.
 * @param num_cols The number of columns to initialize.
 * @return Non-NULL upon success.
 */
cace_amm_semtype_tblt_t *cace_amm_type_set_tblt_size(cace_amm_type_t *type, size_t num_cols);

/** Read a table template config from its serialized name.
 *
 * @param[in,out] type The object to populate.
 * @param[in] deref The name to draw parameters from.
 * @return Zero if successful.
 */
int cace_amm_type_set_tblt_from_name(cace_amm_type_t *type, const cace_amm_lookup_t *deref,
                                     const cace_amm_obj_store_t *store);

/// Configuration for a union of other types
typedef struct
{
    /** The ordered list of types in this union.
     * Ownership of these objects is managed by the container.
     */
    cace_amm_type_array_t choices;
} cace_amm_semtype_union_t;

static inline void cace_amm_semtype_union_init(cace_amm_semtype_union_t *obj)
{
    cace_amm_type_array_init(obj->choices);
}

static inline void cace_amm_semtype_union_deinit(cace_amm_semtype_union_t *obj)
{
    cace_amm_type_array_clear(obj->choices);
}

/** Create a union type based on a choice of other type objects.
 * A union type contains a list of underlying types to choose from.
 *
 * @param[out] type The type to initialize and populate.
 * @param num_choices The number of choices to initialize.
 * @return Non-NULL upon success.
 */
cace_amm_semtype_union_t *cace_amm_type_set_union_size(cace_amm_type_t *type, size_t num_choices);

/** Read a union config from its serialized name.
 *
 * @param[in,out] type The object to populate.
 * @param[in] deref The name to draw parameters from.
 * @return Zero if successful.
 */
int cace_amm_type_set_union_from_name(cace_amm_type_t *type, const cace_amm_lookup_t *deref,
                                      const cace_amm_obj_store_t *store);

/** Configuration for a sub-sequence list within an AC.
 * This is similar to an cace_amm_semtype_ulist_t but does not capture the
 * containing AC so sequences can be concatenated.
 */
typedef struct
{
    /** The type for each item of the list.
     * All type references are fully recursively resolved.
     * The type object is owned by this semtype.
     */
    cace_amm_type_t item_type;

    /** Constraint on the number of items.
     */
    cace_amm_range_intvl_size_t size;

} cace_amm_semtype_seq_t;

static inline void cace_amm_semtype_seq_init(cace_amm_semtype_seq_t *obj)
{
    cace_amm_type_init(&(obj->item_type));
    cace_amm_range_intvl_size_set_infinite(&(obj->size));
}

static inline void cace_amm_semtype_seq_deinit(cace_amm_semtype_seq_t *obj)
{
    cace_amm_range_intvl_size_set_infinite(&(obj->size));
    cace_amm_type_deinit(&(obj->item_type));
}

/** Create a sub-sequence list semantic type.
 *
 * @param[out] type The type to initialize and populate.
 * @return Non-NULL upon success.
 */
cace_amm_semtype_seq_t *cace_amm_type_set_seq(cace_amm_type_t *type);

/** Read a sub-sequence from its serialized name.
 *
 * @param[in,out] type The object to populate.
 * @param[in] deref The name to draw parameters from.
 * @return Zero if successful.
 */
int cace_amm_type_set_seq_from_name(cace_amm_type_t *type, const cace_amm_lookup_t *deref,
                                    const cace_amm_obj_store_t *store);

#ifdef __cplusplus
}
#endif

#endif /* CACE_AMM_SEMTYPE_H_ */
