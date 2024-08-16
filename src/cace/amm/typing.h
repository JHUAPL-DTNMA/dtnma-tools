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
 * This file contains definitions for AMM semantic typing of ARI values.
 */
/** @page amm_typing AMM Typing
 * Types within the AMM extend beyond the built-in types of ARI values into
 * "semantic types" as defined in Section 3.3 of the AMM @cite ietf-dtn-adm-00.
 *
 * The struct amm_type_s is used to represent both built-in ARI types as well
 * as different forms of semantic typing built upon the built-in types.
 */
#ifndef CACE_AMM_TYPING_H_
#define CACE_AMM_TYPING_H_

#include "cace/ari.h"
#include "cace/cace_data.h"
#include <m-deque.h>

#ifdef __cplusplus
extern "C" {
#endif

#define CACE_AMM_ERR_CONVERT_NULLFUNC 2
#define CACE_AMM_ERR_CONVERT_BADVALUE 3

// Forward declaration to allow recursive references
struct amm_type_s;

/// A typedef representing an AMM semantic type.
typedef struct amm_type_s amm_type_t;

/** @struct amm_typeptr_list_t
 * A set of pointers to other type structs.
 * These are all references and have no ownership logic.
 * This includes the valid possibility of circular references.
 */
/// @cond Doxygen_Suppress
DEQUE_DEF(amm_typeptr_list, const amm_type_t *, M_PTR_OPLIST)
/// @endcond

/// Configuration for a built-in type
struct amm_type_builtin_s
{
    /// The ARI type corresponding to this built-in
    ari_type_t ari_type;
};

/// Configuration for an augmented use of another type
struct amm_type_use_s
{
    /// The type being used
    const amm_type_t *base;
    // FIXME Other attributes TBD
};

/// Configuration for a union of other types
struct amm_type_union_s
{
    /// The ordered list of types in this union
    amm_typeptr_list_t choices;
};

/** Descriptor for each built-in (ARI type) and semantic type within the AMM.
 * Both #match and #convert should be non-null for any type.
 * Details about AMM typing are discussed in the page @ref amm_typing.
 */
struct amm_type_s
{
    /** Determine if a specific value matches this type.
     *
     * @param[in] self Pointer to the associated type object.
     * @param[in] ari The value to check.
     * @return True if the value matches this type.
     */
    bool (*match)(const amm_type_t *self, const ari_t *ari);

    /** Convert a value to this type if possible.
     * It is expected that many input values will not be convertible to any
     * given type, which is indicated by the return value.
     *
     * @param[in] self Pointer to the associated type object.
     * @param[out] out The value to convert into.
     * @param[in] in The value to convert from.
     * @return Zero if the conversion is successful.
     */
    int (*convert)(const amm_type_t *self, ari_t *out, const ari_t *in);

    /// Determine which of the following union member is valid
    enum amm_type_class_e
    {
        /// A built-in type using the #as_builtin member
        AMM_TYPE_BUILTIN,
        /// An augmented use of another type using the #as_use member
        AMM_TYPE_USE,
        /// A union type using the #as_union member
        AMM_TYPE_UNION,
    } type_class;
    union
    {
        /// Valid when #type_class is amm_type_s::AMM_TYPE_BUILTIN
        struct amm_type_builtin_s as_builtin;
        /// Valid when #type_class is amm_type_s::AMM_TYPE_USE
        struct amm_type_use_s as_use;
        /// Valid when #type_class is amm_type_s::AMM_TYPE_UNION
        struct amm_type_union_s as_union;
    };
};

/** Get a built-in type object.
 *
 * @param ari_type The associated literal type to lookup.
 * @return A stable non-null pointer to a built-in type object or
 * a null pointer if not found.
 */
const amm_type_t *amm_type_get_builtin(ari_type_t ari_type);

/** Create a use type based on a base type object.
 * A use type adds annotations and constraints onto a base type.
 *
 * @param[out] type The type to initialize and populate.
 * @param[in] base The base type to create a use of.
 */
int amm_type_init_use(amm_type_t *type, const amm_type_t *base);

/** Create a union type based on a choice of other type objects.
 * A union type contains a list of underlying types to choose from.
 *
 * @param[out] type The type to initialize and populate.
 * @param[in] choices A null-terminated array of pointers to the types
 * to create the union for.
 */
int amm_type_init_union(amm_type_t *type, const amm_type_t **choices);

/** Free any resources associated with a semantic type.
 *
 * @param[in,out] type The object to de-initialize.
 * @return Zero upon success.
 */
void amm_type_deinit(amm_type_t *type);

/** Determine if a type (built-in or semantic) matches a specific value.
 *
 * @param[in] type The type to check against.
 * @param[in] ari The value to check.
 * This struct must be initialized.
 * @return True if the type fully matches the value.
 */
bool amm_type_match(const amm_type_t *type, const ari_t *ari);

/** Force a value to be converted to a specific type (built-in or semantic).
 *
 * @param[in] type The type to convert to.
 * @param[out] out The converted value (valid if the return is zero).
 * This struct must already be initialized.
 * @param[in] in The value to convert.
 * This struct must be initialized.
 * @return Zero if the conversion is successful.
 */
int amm_type_convert(const amm_type_t *type, ari_t *out, const ari_t *in);

/** Validate that typed literals agree with their values.
 * This performs comparisons of valid ari_lit_t::prim_type and valid
 * ari_lit_t::value within each ARI type.
 */
bool amm_builtin_validate(const ari_t *ari);

#ifdef __cplusplus
}
#endif

#endif /* CACE_AMM_TYPING_H_ */
