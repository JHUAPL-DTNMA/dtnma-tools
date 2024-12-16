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
ARRAY_DEF(amm_semtype_cnst_array, amm_semtype_cnst_t)
/// @endcond

/// Configuration for an augmented use of another type
typedef struct
{
    /// Name of the type used
    ari_t name;
    /** The type object being used, which is bound based on #name.
     * This is always a reference to an externally-owned object.
     */
    const amm_type_t *base;

    /** Optional constraints on this use.
     * The order matches the source ADM but is not significant to the outcome.
     */
    amm_semtype_cnst_array_t constraints;

} amm_semtype_use_t;

static inline void amm_semtype_use_init(amm_semtype_use_t *obj)
{
    ari_init(&(obj->name));
    obj->base = NULL;
    amm_semtype_cnst_array_init(obj->constraints);
}

static inline void amm_semtype_use_deinit(amm_semtype_use_t *obj)
{
    amm_semtype_cnst_array_clear(obj->constraints);
    ari_deinit(&(obj->name));
    obj->base = NULL;
}

/** Create a use type based on a type reference.
 * A use type adds annotations and constraints onto a base type.
 *
 * @param[out] type The type to initialize and populate.
 * @param[in] name The ARITYPE literal or TYPEDEF reference value.
 */
int amm_type_set_use_ref(amm_type_t *type, const ari_t *name);
/** @overload
 * Set the name with move semantics.
 */
int amm_type_set_use_ref_move(amm_type_t *type, ari_t *name);

/** Create a use type based on a base type object.
 * A use type adds annotations and constraints onto a base type.
 *
 * @param[out] type The type to initialize and populate.
 * @param[in] base The base type to create a use of.
 */
int amm_type_set_use_direct(amm_type_t *type, const amm_type_t *base);

/// Configuration for a uniform list within an AC
typedef struct
{
    /** The type for each item of the list.
     * All type references are fully recursively resolved.
     * The type object is owned by this semtype.
     */
    amm_type_t item_type;

    /** Constraint on the number of items.
     */
    cace_amm_range_intvl_size_t size;

} amm_semtype_ulist_t;

static inline void amm_semtype_ulist_init(amm_semtype_ulist_t *obj)
{
    amm_type_init(&(obj->item_type));
    cace_amm_range_intvl_size_set_infinite(&(obj->size));
}

static inline void amm_semtype_ulist_deinit(amm_semtype_ulist_t *obj)
{
    cace_amm_range_intvl_size_set_infinite(&(obj->size));
    amm_type_deinit(&(obj->item_type));
}

/** Create a uniform list semantic type.
 *
 * @param[out] type The type to initialize and populate.
 * @return Non-NULL upon success.
 */
amm_semtype_ulist_t *amm_type_set_ulist(amm_type_t *type);

/// Configuration for a diverse list within an AC
typedef struct
{
    /** The ordered list of semantic types for each sub-sequence of
     * the list.
     * All type references are fully recursively resolved.
     * The type object is owned by this semtype.
     */
    amm_type_array_t types;

} amm_semtype_dlist_t;

static inline void amm_semtype_dlist_init(amm_semtype_dlist_t *obj)
{
    amm_type_array_init(obj->types);
}

static inline void amm_semtype_dlist_deinit(amm_semtype_dlist_t *obj)
{
    amm_type_array_clear(obj->types);
}

/** Create a diverse list semantic type.
 *
 * @param[out] type The type to initialize and populate.
 * @param num_cols The number of types to initialize.
 * @return Non-NULL upon success.
 */
amm_semtype_dlist_t *amm_type_set_dlist(amm_type_t *type, size_t num_types);

/// Configuration for a uniform list within an AM
typedef struct
{
    /** The type for each key of the map.
     * All type references are fully recursively resolved.
     * The type object is owned by this semtype.
     */
    amm_type_t key_type;
    /** The type for each value of the map.
     * All type references are fully recursively resolved.
     * The type object is owned by this semtype.
     */
    amm_type_t val_type;

} amm_semtype_umap_t;

static inline void amm_semtype_umap_init(amm_semtype_umap_t *obj)
{
    amm_type_init(&(obj->key_type));
    amm_type_init(&(obj->val_type));
}

static inline void amm_semtype_umap_deinit(amm_semtype_umap_t *obj)
{
    amm_type_deinit(&(obj->val_type));
    amm_type_deinit(&(obj->key_type));
}

/** Create a uniform map semantic type.
 *
 * @param[out] type The type to initialize and populate.
 * @return Non-NULL upon success.
 */
amm_semtype_umap_t *amm_type_set_umap(amm_type_t *type);

/// Configuration for a table template
typedef struct
{
    /** The ordered list of types in this union.
     * Ownership of these objects is managed by the container.
     */
    amm_named_type_array_t columns;
} amm_semtype_tblt_t;

static inline void amm_semtype_tblt_init(amm_semtype_tblt_t *obj)
{
    amm_named_type_array_init(obj->columns);
}

static inline void amm_semtype_tblt_deinit(amm_semtype_tblt_t *obj)
{
    amm_named_type_array_clear(obj->columns);
}

/** Create a table template based on a set of typed columns.
 *
 * @param[out] type The type to initialize and populate.
 * @param num_cols The number of columns to initialize.
 * @return Non-NULL upon success.
 */
amm_semtype_tblt_t *amm_type_set_tblt_size(amm_type_t *type, size_t num_cols);

/// Configuration for a union of other types
typedef struct
{
    /** The ordered list of types in this union.
     * Ownership of these objects is managed by the container.
     */
    amm_type_array_t choices;
} amm_semtype_union_t;

static inline void amm_semtype_union_init(amm_semtype_union_t *obj)
{
    amm_type_array_init(obj->choices);
}

static inline void amm_semtype_union_deinit(amm_semtype_union_t *obj)
{
    amm_type_array_clear(obj->choices);
}

/** Create a union type based on a choice of other type objects.
 * A union type contains a list of underlying types to choose from.
 *
 * @param[out] type The type to initialize and populate.
 * @param num_choices The number of choices to initialize.
 * @return Non-NULL upon success.
 */
amm_semtype_union_t *amm_type_set_union_size(amm_type_t *type, size_t num_choices);

/** Configuration for a sub-sequence list within an AC.
 * This is similar to an amm_semtype_ulist_t but does not capture the
 * containing AC so sequences can be concatenated.
 */
typedef struct
{
    /** The type for each item of the list.
     * All type references are fully recursively resolved.
     * The type object is owned by this semtype.
     */
    amm_type_t item_type;

    /** Constraint on the number of items.
     */
    cace_amm_range_intvl_size_t size;

} amm_semtype_seq_t;

static inline void amm_semtype_seq_init(amm_semtype_seq_t *obj)
{
    amm_type_init(&(obj->item_type));
    cace_amm_range_intvl_size_set_infinite(&(obj->size));
}

static inline void amm_semtype_seq_deinit(amm_semtype_seq_t *obj)
{
    cace_amm_range_intvl_size_set_infinite(&(obj->size));
    amm_type_deinit(&(obj->item_type));
}

/** Create a sub-sequence list semantic type.
 *
 * @param[out] type The type to initialize and populate.
 * @return Non-NULL upon success.
 */
amm_semtype_seq_t *amm_type_set_seq(amm_type_t *type);

#ifdef __cplusplus
}
#endif

#endif /* CACE_AMM_SEMTYPE_H_ */
