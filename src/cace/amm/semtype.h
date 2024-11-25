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
#include <m-array.h>

#ifdef __cplusplus
extern "C" {
#endif

/// Configuration for an augmented use of another type
typedef struct
{
    /// Name of the type used
    ari_t name;
    /** The type object being used, which is bound based on #name.
     * This is always a reference to an externally-owned object.
     */
    const amm_type_t *base;

    // FIXME Other constraints and attributes TBD
} amm_semtype_use_t;

static inline void amm_semtype_use_init(amm_semtype_use_t *obj)
{
    ari_init(&(obj->name));
    obj->base = NULL;
}

static inline void amm_semtype_use_deinit(amm_semtype_use_t *obj)
{
    ari_deinit(&(obj->name));
    obj->base = NULL;
}

/** A closed interval of size_t values with optional minimum and maximum.
 */
typedef struct
{
    /// True if this interval has a finite minimum
    bool has_min;
    /// The minimum value of the interval, valid if #has_max is true
    size_t i_min;
    /// True if this interval has a finite maximum
    bool has_max;
    /// The maximum value of the interval, valid if #has_max is true
    size_t i_max;
} amm_semtype_size_intvl_t;

static inline void amm_semtype_size_intvl_init(amm_semtype_size_intvl_t *obj)
{
    *obj = (amm_semtype_size_intvl_t) {
        .has_min = false,
        .has_max = false,
    };
}

/// Configuration for a uniform list within an AC
typedef struct
{
    /** The type for each item of the list.
     * All type references are fully recursively resolved.
     * The type object is owned by this column.
     */
    amm_type_t item_type;

    /** Constraint on the number of items.
     */
    amm_semtype_size_intvl_t size;

} amm_semtype_ulist_t;

static inline void amm_semtype_ulist_init(amm_semtype_ulist_t *obj)
{
    amm_type_init(&(obj->item_type));
    amm_semtype_size_intvl_init(&(obj->size));
}

static inline void amm_semtype_ulist_deinit(amm_semtype_ulist_t *obj)
{
    amm_type_deinit(&(obj->item_type));
}

/// Configuration for a uniform list within an AM
typedef struct
{
    /** The type for each key of the map.
     * All type references are fully recursively resolved.
     * The type object is owned by this column.
     */
    amm_type_t key_type;
    /** The type for each value of the map.
     * All type references are fully recursively resolved.
     * The type object is owned by this column.
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

/// Configuration of a table template column
typedef struct
{
    /// The unique name of the column.
    string_t name;

    /** The type of the column.
     * All type references are fully recursively resolved.
     * The type object is owned by this column.
     */
    amm_type_t typeobj;
} amm_semtype_tblt_col_t;

void amm_semtype_tblt_col_init(amm_semtype_tblt_col_t *obj);

void amm_semtype_tblt_col_deinit(amm_semtype_tblt_col_t *obj);

/// OPLIST for the amm_semtype_tblt_col_s
#define M_OPL_amm_semtype_tblt_col_t() \
    (INIT(API_2(amm_semtype_tblt_col_init)), CLEAR(API_2(amm_semtype_tblt_col_deinit)))

/// @cond Doxygen_Suppress
ARRAY_DEF(amm_semtype_tblt_col_array, amm_semtype_tblt_col_t)
/// @endcond

/// Configuration for a table template
typedef struct
{
    /** The ordered list of types in this union.
     * Ownership of these objects is managed by the container.
     */
    amm_semtype_tblt_col_array_t columns;
} amm_semtype_tblt_t;

static inline void amm_semtype_tblt_init(amm_semtype_tblt_t *obj)
{
    amm_semtype_tblt_col_array_init(obj->columns);
}

static inline void amm_semtype_tblt_deinit(amm_semtype_tblt_t *obj)
{
    amm_semtype_tblt_col_array_clear(obj->columns);
}

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

/** Create a use type based on a type reference.
 * A use type adds annotations and constraints onto a base type.
 *
 * @param[out] type The type to initialize and populate.
 * @param[in] name The ARITYPE literal or TYPEDEF reference value.
 */
int amm_type_set_use_ref(amm_type_t *type, const ari_t *name);

/** Create a use type based on a base type object.
 * A use type adds annotations and constraints onto a base type.
 *
 * @param[out] type The type to initialize and populate.
 * @param[in] base The base type to create a use of.
 */
int amm_type_set_use_direct(amm_type_t *type, const amm_type_t *base);

/** Create a uniform list semantic type.
 *
 * @param[out] type The type to initialize and populate.
 * @return Non-NULL upon success.
 */
amm_semtype_ulist_t *amm_type_set_ulist(amm_type_t *type);

/** Create a uniform map semantic type.
 *
 * @param[out] type The type to initialize and populate.
 * @return Non-NULL upon success.
 */
amm_semtype_umap_t *amm_type_set_umap(amm_type_t *type);

/** Create a table template based on a set of typed columns.
 *
 * @param[out] type The type to initialize and populate.
 * @param num_cols The number of columns to initialize.
 * @return Non-NULL upon success.
 */
amm_semtype_tblt_t *amm_type_set_tblt_size(amm_type_t *type, size_t num_cols);

/** Create a union type based on a choice of other type objects.
 * A union type contains a list of underlying types to choose from.
 *
 * @param[out] type The type to initialize and populate.
 * @param num_choices The number of choices to initialize.
 * @return Non-NULL upon success.
 */
amm_semtype_union_t *amm_type_set_union_size(amm_type_t *type, size_t num_choices);

#ifdef __cplusplus
}
#endif

#endif /* CACE_AMM_SEMTYPE_H_ */
