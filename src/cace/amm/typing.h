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
 * This file contains definitions for AMM typing of ARI values.
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
struct cace_amm_type_s;

/// A typedef representing an AMM semantic type.
typedef struct cace_amm_type_s cace_amm_type_t;

#define CACE_AMM_TYPE_INIT_INVALID                                          \
    (cace_amm_type_t)                                                       \
    {                                                                       \
        .match = NULL, .convert = NULL, .type_class = CACE_AMM_TYPE_INVALID \
    }

/** Initialize a type object to a default, invalid state.
 *
 * @param[out] type The type to initialize.
 */
void cace_amm_type_init(cace_amm_type_t *type);

/** Free any resources associated with a semantic type.
 *
 * @param[in,out] type The object to de-initialize.
 */
void cace_amm_type_deinit(cace_amm_type_t *type);

/** Reset to the default invalid state.
 *
 * @param[in,out] type The object to reset.
 */
void cace_amm_type_reset(cace_amm_type_t *type);

/// M*LIB OPLIST for the cace_amm_type_t
#define M_OPL_cace_amm_type_t()                                                                \
    (INIT(API_2(cace_amm_type_init)), INIT_SET(0), CLEAR(API_2(cace_amm_type_deinit)), SET(0), \
     RESET(API_2(cace_amm_type_reset)))

/// Configuration for a built-in type
struct cace_amm_type_builtin_s
{
    /// The ARI type corresponding to this built-in
    cace_ari_type_t ari_type;
};

typedef void (*cace_amm_semtype_deinit_f)(void *semtype);

/// Result status for type matching
typedef enum
{
    /// The result was not positive or negative
    CACE_AMM_TYPE_MATCH_NOINFO,
    /// The result was a negative (non-match)
    CACE_AMM_TYPE_MATCH_NEGATIVE,
    /// The result was a positive (match)
    CACE_AMM_TYPE_MATCH_POSITIVE,
    /// The input is the undefined value and was not checked
    CACE_AMM_TYPE_MATCH_UNDEFINED,
} cace_amm_type_match_res_t;

/** Return either a positive or negative match depending on a condition.
 */
static inline cace_amm_type_match_res_t cace_amm_type_match_pos_neg(bool cond)
{
    return cond ? CACE_AMM_TYPE_MATCH_POSITIVE : CACE_AMM_TYPE_MATCH_NEGATIVE;
}

/// Possible classes of AMM types
typedef enum
{
    /// An initialized but not valid type
    CACE_AMM_TYPE_INVALID,
    /// A built-in type using the #as_builtin member
    CACE_AMM_TYPE_BUILTIN,
    /// An augmented use of another type using the #as_semtype member
    CACE_AMM_TYPE_USE,
    /// A uniform list using the #as_semtype member
    CACE_AMM_TYPE_ULIST,
    /// A diverse list using the #as_semtype member
    CACE_AMM_TYPE_DLIST,
    /// A uniform map using the #as_semtype member
    CACE_AMM_TYPE_UMAP,
    /// A table template using the #as_semtype member
    CACE_AMM_TYPE_TBLT,
    /// A union type using the #as_semtype member
    CACE_AMM_TYPE_UNION,
    /// A sub-sequence using the #as_semtype member
    CACE_AMM_TYPE_SEQ,
} cace_amm_type_class_t;

/** Descriptor for each built-in (ARI type) and semantic type within the AMM.
 * Users of this struct must treat it as opaque and not access any individual
 * members directly, instead use cace_amm_type_set_* functions to set its state
 * and other functions to access its state.
 *
 * Both #match and #convert should be non-null for any type.
 * Details about AMM typing are discussed in the page @ref cace_amm_typing.
 * This includes the valid possibility of circular references.
 */
struct cace_amm_type_s
{
    /** Get an ARI representation of a name for this type.
     *
     * @param[in] self Pointer to the associated type object.
     * @param[out] name The already initialized output object.
     */
    void (*ari_name)(const cace_amm_type_t *self, cace_ari_t *name);

    /** Determine if a specific value matches this type.
     *
     * @param[in] self Pointer to the associated type object.
     * @param[in] ari The value to check.
     * @return True if the value matches this type.
     */
    cace_amm_type_match_res_t (*match)(const cace_amm_type_t *self, const cace_ari_t *ari);

    /** Convert a value to this type if possible.
     * It is expected that many input values will not be convertible to any
     * given type, which is indicated by the return value.
     *
     * @param[in] self Pointer to the associated type object.
     * @param[out] out The value to convert into.
     * @param[in] in The value to convert from.
     * @return Zero if the conversion is successful.
     */
    int (*convert)(const cace_amm_type_t *self, cace_ari_t *out, const cace_ari_t *in);

    /// Determine which of the following union member is valid
    cace_amm_type_class_t type_class;
    union
    {
        /// Valid when #type_class is ::CACE_AMM_TYPE_BUILTIN
        struct cace_amm_type_builtin_s as_builtin;
        /** Non-null for all other cace_amm_type_s values.
         * Cast to specific internal configuration struct for each class of
         * semantic type.
         */
        void *as_semtype;
    };

    // De-initializing function for #as_semtype when it is valid
    cace_amm_semtype_deinit_f as_semtype_deinit;
};

/** Get a built-in type object.
 *
 * @param ari_type The associated literal type to lookup.
 * @return A stable non-null pointer to a built-in type object or
 * a null pointer if not found.
 */
const cace_amm_type_t *cace_amm_type_get_builtin(cace_ari_type_t ari_type);

/** Determine if a type object is valid.
 *
 * @return True if the object is valid.
 */
bool cace_amm_type_is_valid(const cace_amm_type_t *type);

/** @struct cace_amm_type_array_t
 * An array of type structs.
 */
/// @cond Doxygen_Suppress
ARRAY_DEF(cace_amm_type_array, cace_amm_type_t)
/// @endcond

/** Get an ARI name for a type object.
 * This is the inverse of cace_amm_type_set_name().
 *
 * @param[in] type The type to name.
 * @param[out] name The generated name.
 * @return True if a name was set.
 */
bool cace_amm_type_get_name(const cace_amm_type_t *type, cace_ari_t *name);

// forward declare
struct cace_amm_obj_store_s;

/** Set a type object from an ARI name.
 * This is the inverse of cace_amm_type_get_name().
 *
 * @note This function alone does not do any name binding based on referenced
 * objects. That must be done in the associated agent (see @ref refda/binding.h).
 *
 * @param[in,out] type The type to replace based on the name.
 * @param[in] name The input type name.
 * @param[in] store Object store for reference lookup.
 * @return Zero if successful.
 */
int cace_amm_type_set_name(cace_amm_type_t *type, const cace_ari_t *name, const struct cace_amm_obj_store_s *store);

/** Determine if a type (built-in or semantic) matches a specific value.
 *
 * @param[in] type The type to check against.
 * @param[in] ari The value to check.
 * This struct must be initialized.
 * @return AMM_TYPE_MATCH_POSITIVE if the type fully matches the value.
 */
cace_amm_type_match_res_t cace_amm_type_match(const cace_amm_type_t *type, const cace_ari_t *ari);

/** Force a value to be converted to a specific type (built-in or semantic).
 *
 * @param[in] type The type to convert to.
 * @param[out] out The converted value (valid if the return is zero).
 * This struct must already be initialized.
 * @param[in] in The value to convert.
 * This struct must be initialized.
 * @return Zero if the conversion is successful.
 */
int cace_amm_type_convert(const cace_amm_type_t *type, cace_ari_t *out, const cace_ari_t *in);

/** Validate that typed literals agree with their values.
 * This performs comparisons of valid cace_ari_lit_t::prim_type and valid
 * cace_ari_lit_t::value within each ARI type.
 */
bool cace_amm_builtin_validate(const cace_ari_t *ari);

#ifdef __cplusplus
}
#endif

#endif /* CACE_AMM_TYPING_H_ */
