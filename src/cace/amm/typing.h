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
 * This file contains definitions for AMM typing of ARI values.
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
#include <m-array.h>

#ifdef __cplusplus
extern "C" {
#endif

#define CACE_AMM_ERR_CONVERT_NULLFUNC          2
#define CACE_AMM_ERR_CONVERT_BADVALUE          3
#define CACE_AMM_ERR_CONVERT_NOCHOICE          4
#define CACE_AMM_ERR_CONVERT_FAILED_CONSTRAINT 5

// Forward declaration to allow recursive references
struct amm_type_s;

/// A typedef representing an AMM semantic type.
typedef struct amm_type_s amm_type_t;

#define AMM_TYPE_INIT_INVALID                                          \
    (amm_type_t)                                                       \
    {                                                                  \
        .match = NULL, .convert = NULL, .type_class = AMM_TYPE_INVALID \
    }

/** Initialize a type object to a default, invalid state.
 *
 * @param[out] type The type to initialize.
 */
void amm_type_init(amm_type_t *type);

/** Free any resources associated with a semantic type.
 *
 * @param[in,out] type The object to de-initialize.
 */
void amm_type_deinit(amm_type_t *type);

/** Reset to the default invalid state.
 *
 * @param[in,out] type The object to reset.
 */
void amm_type_reset(amm_type_t *type);

/// M*LIB OPLIST for the amm_type_t
#define M_OPL_amm_type_t() (INIT(API_2(amm_type_init)), CLEAR(API_2(amm_type_deinit)), RESET(API_2(amm_type_reset)))

/** A pointer to amm_type_t with ownership semantics.
 *
 * The M_SHARED_PTR_RELAXED_DEF cannot be used here because it requires a
 * concrete type defintion and cannot be used with forward-declared types.
 */
typedef struct
{
    /// Pointer to the managed object
    amm_type_t *obj;
} amm_typeptr_t;

/** Initialize a type pointer to an allocated and initialized type object.
 *
 * @param[out] ptr The type to initialize.
 */
void amm_typeptr_init(amm_typeptr_t *ptr);

/** Free any allocated type object.
 *
 * @param[in,out] ptr The object to de-initialize.
 */
void amm_typeptr_deinit(amm_typeptr_t *ptr);

/** Take ownership of a pointed-to object.
 *
 * @param[in,out] ptr The object to set.
 * @param[in] obj The object to own.
 */
void amm_typeptr_take(amm_typeptr_t *ptr, amm_type_t *obj);

/// OPLIST for the amm_typeptr_t
#define M_OPL_amm_typeptr_t() (INIT(API_2(amm_typeptr_init)), CLEAR(API_2(amm_typeptr_deinit)))

/// Configuration for a built-in type
struct amm_type_builtin_s
{
    /// The ARI type corresponding to this built-in
    ari_type_t ari_type;
};

typedef void (*amm_semtype_deinit_f)(void *semtype);

/// Result status for type matching
typedef enum
{
    /// The result was not positive or negative
    AMM_TYPE_MATCH_NOINFO,
    /// The result was a negative (non-match)
    AMM_TYPE_MATCH_NEGATIVE,
    /// The result was a positive (match)
    AMM_TYPE_MATCH_POSITIVE,
    AMM_TYPE_MATCH_UNDEFINED,
} amm_type_match_res_t;

/** Return either a postive or negative match depending on a condition.
 */
static inline amm_type_match_res_t amm_type_match_pos_neg(bool cond)
{
    return cond ? AMM_TYPE_MATCH_POSITIVE : AMM_TYPE_MATCH_NEGATIVE;
}

/** Descriptor for each built-in (ARI type) and semantic type within the AMM.
 * Users of this struct must treat it as opaque and not access any individual
 * members directly, instead use amm_type_set_* functions to set its state
 * and other functions to access its state.
 *
 * Both #match and #convert should be non-null for any type.
 * Details about AMM typing are discussed in the page @ref amm_typing.
 * This includes the valid possibility of circular references.
 */
struct amm_type_s
{
    /** Get an ARI representation of a name for this type.
     *
     * @param[in] self Pointer to the associated type object.
     * @param[out] name The already initialized output object.
     */
    void (*ari_name)(const amm_type_t *self, ari_t *name);

    /** Determine if a specific value matches this type.
     *
     * @param[in] self Pointer to the associated type object.
     * @param[in] ari The value to check.
     * @return True if the value matches this type.
     */
    amm_type_match_res_t (*match)(const amm_type_t *self, const ari_t *ari);

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
        /// An initialized but not valid type
        AMM_TYPE_INVALID,
        /// A built-in type using the #as_builtin member
        AMM_TYPE_BUILTIN,
        /// An augmented use of another type using the #as_semtype member
        AMM_TYPE_USE,
        /// A uniform list using the #as_semtype member
        AMM_TYPE_ULIST,
        /// A diverse list using the #as_semtype member
        AMM_TYPE_DLIST,
        /// A uniform map using the #as_semtype member
        AMM_TYPE_UMAP,
        /// A table template using the #as_semtype member
        AMM_TYPE_TBLT,
        /// A union type using the #as_semtype member
        AMM_TYPE_UNION,
        /// A sub-sequence using the #as_semtype member
        AMM_TYPE_SEQ,
    } type_class;
    union
    {
        /// Valid when #type_class is amm_type_s::AMM_TYPE_BUILTIN
        struct amm_type_builtin_s as_builtin;
        /** Non-null for all other amm_type_s values.
         * Cast to specific internal configuration struct for each class of
         * semantic type.
         */
        void *as_semtype;
    };

    // De-initializing function for #as_semtype when it is valid
    amm_semtype_deinit_f as_semtype_deinit;
};

/** Get a built-in type object.
 *
 * @param ari_type The associated literal type to lookup.
 * @return A stable non-null pointer to a built-in type object or
 * a null pointer if not found.
 */
const amm_type_t *amm_type_get_builtin(ari_type_t ari_type);

/** Determine if a type object is valid.
 *
 * @return True if the object is valid.
 */
bool amm_type_is_valid(const amm_type_t *type);

/** @struct amm_type_array_t
 * An array of type structs.
 */
/// @cond Doxygen_Suppress
ARRAY_DEF(amm_type_array, amm_type_t)
/// @endcond

/** Get a human-friendly name for a type object.
 *
 * @param[in] type The type to name.
 * @param[out] name The generated name.
 * @return True if a name was set.
 */
bool amm_type_get_name(const amm_type_t *type, ari_t *name);

/** Determine if a type (built-in or semantic) matches a specific value.
 *
 * @param[in] type The type to check against.
 * @param[in] ari The value to check.
 * This struct must be initialized.
 * @return AMM_TYPE_MATCH_POSITIVE if the type fully matches the value.
 */
amm_type_match_res_t amm_type_match(const amm_type_t *type, const ari_t *ari);

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
