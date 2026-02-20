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
 * This file contains algorithmic functions to operate on ARIs recursively.
 */
#ifndef CACE_ARI_ALGO_H_
#define CACE_ARI_ALGO_H_

#include "base.h"

#ifdef __cplusplus
extern "C" {
#endif

/** Context used for cace_ari_visitor_t functions.
 */
typedef struct
{
    /** The parent value of the one being visited.
     * If this is a literal with a container type, the visited value is a
     * member of that container.
     * If this is an object reference, the visited value is a parameter.
     */
    const cace_ari_t *parent;

    /// True if the parent is an AM literal and this is a map key
    bool is_map_key;

    /// User-supplied pointer to cace_ari_visit()
    void *user_data;
} cace_ari_visit_ctx_t;

/** Function pointers to implement the cace_ari_visit() behavior.
 */
typedef struct
{

    /** Called when visiting each ARI.
     * For containers this is called before any contained values.
     *
     * @param[in] ari The value being visited.
     * @param[in] ctx Visitor context information.
     * @return Zero to continue iterating, or non-zero to stop immediately.
     */
    int (*visit_ari)(cace_ari_t *ari, const cace_ari_visit_ctx_t *ctx);

    /// @overload
    int (*visit_ref)(cace_ari_ref_t *obj, const cace_ari_visit_ctx_t *ctx);

    /// @overload
    int (*visit_objpath)(cace_ari_objpath_t *obj, const cace_ari_visit_ctx_t *ctx);

    /** @overload
     * This is called before and in addition to any visiting of container
     * contents.
     */
    int (*visit_lit)(cace_ari_lit_t *obj, const cace_ari_visit_ctx_t *ctx);

} cace_ari_visitor_t;

/** Visit an ARI recursively.
 *
 * @param[in] ari The value to visit.
 * @param[in] visitor The visitor descriptor.
 * @param[in] user_data Data passed to the cace_ari_visitor_t functions.
 * @return Zero upon success, or non-zero if one of the visitor functions
 * returned non-zero.
 */
int cace_ari_visit(cace_ari_t *ari, const cace_ari_visitor_t *visitor, void *user_data);

/** Context used for cace_ari_translator_t functions.
 */
typedef struct
{
    /** The parent value of the one being translated.
     * If this is a literal with a container type, the visited value is a
     * member of that container.
     * If this is an object reference, the visited value is a parameter.
     */
    const cace_ari_t *parent;

    /// True if the parent is an AM literal and this is a map key
    bool is_map_key;

    /// User-supplied pointer to cace_ari_translate()
    void *user_data;
} cace_ari_translate_ctx_t;

typedef enum
{
    /** Indicates that the callback has failed and translation should
     * be stopped as having failed.
     */
    CACE_ARI_TRANSLATE_FAILURE,
    /** Indicates that the callback has fully translated the value and
     * the output is in its final form.
     * @note This result is useful for leaf values and will not recurse into
     * containers.
     */
    CACE_ARI_TRANSLATE_FINAL,
    /** Indicates that the callback has not modified the output and
     * the default recursive translation needs to be performed.
     */
    CACE_ARI_TRANSLATE_DEFAULT
} cace_ari_translate_result_t;

/** Function pointers to implement the cace_ari_translate() behavior.
 * For any input value, the #map_ari is first called (with fallback behavior)
 * followed by either #map_objpath or #map_lit depending on the ARI type.
 */
typedef struct
{
    /** Called when translating each ARI.
     * If not provided, the CACE_ARI_TRANSLATE_DEFAULT result is assumed,
     * which will copy the literal / reference structure and recurse into
     * contained values or given parameters respectively.
     *
     * @pre The @c out value is already initialized.
     * @param[out] out The produced value.
     * @param[in] in The value being mapped.
     * @param[in] ctx Visitor context information.
     * @return Zero to continue iterating, or non-zero to stop immediately.
     */
    cace_ari_translate_result_t (*map_ari)(cace_ari_t *out, const cace_ari_t *in, const cace_ari_translate_ctx_t *ctx);

    /**
     * This is used only when #map_ari returns CACE_ARI_TRANSLATE_DEFAULT.
     * If pointer is NULL, the standard cace_ari_objpath_copy() will be used.
     *
     * @pre The @c out value is already initialized.
     * @param[out] out The produced value.
     * @param[in] in The value being mapped.
     * @param[in] ctx Visitor context information.
     * @return Zero to continue iterating, or non-zero to stop immediately.
     */
    int (*map_objpath)(cace_ari_objpath_t *out, const cace_ari_objpath_t *in, const cace_ari_translate_ctx_t *ctx);

    /**
     * This is used only when #map_ari returns CACE_ARI_TRANSLATE_DEFAULT.
     * If pointer is NULL, the standard cace_ari_lit_copy() will be used.
     *
     * @pre The @c out value is already initialized.
     * @param[out] out The produced value.
     * @param[in] in The value being mapped.
     * @param[in] ctx Visitor context information.
     * @return Zero to continue iterating, or non-zero to stop immediately.
     */
    int (*map_lit)(cace_ari_lit_t *out, const cace_ari_lit_t *in, const cace_ari_translate_ctx_t *ctx);

} cace_ari_translator_t;

/** Translate from an input ARI into an output of the same structure.
 *
 * @pre The @c out value is already initialized.
 * @param[out] out The result of translation.
 * @param[in] in The value to translate.
 * @param[in] translator The translator descriptor.
 * @param[in] user_data Data passed to the cace_ari_translator_t functions.
 * @return Zero upon success, or non-zero if one of the translator functions
 * returned non-zero.
 */
int cace_ari_translate(cace_ari_t *out, const cace_ari_t *in, const cace_ari_translator_t *translator, void *user_data);

/** Interface for M*LIB @c HASH operation.
 * This uses the cace_ari_visit() internally to walk an ARI tree.
 *
 * @param[in] ari The object to hash, which contains nested state.
 * @return The hash value.
 */
size_t cace_ari_hash(const cace_ari_t *ari);

/** Compare two ARIs for ordering.
 *
 * @param left One value to compare.
 * @param right Other value to compare.
 * @return -1 if the left value is less than, +1 if greater than, or
 * 0 if they are by-value equal.
 */
int cace_ari_cmp(const cace_ari_t *left, const cace_ari_t *right);

/** Determine if two ARIs have identical value.
 *
 * @param left One value to compare.
 * @param right Other value to compare.
 * @return True if the two are by-value equal.
 */
bool cace_ari_equal(const cace_ari_t *left, const cace_ari_t *right);

/** An M*LIB compatible debug text output function.
 * This encodes to text with default options.
 */
void cace_ari_get_str(m_string_t out, const cace_ari_t obj, bool append);

/// Default OPLIST for cace_ari_t
#define M_OPL_cace_ari_t()                                                                                  \
    (INIT(API_2(cace_ari_init)), INIT_SET(API_6(cace_ari_init_copy)), INIT_MOVE(API_6(cace_ari_init_move)), \
     CLEAR(API_2(cace_ari_deinit)), RESET(API_2(cace_ari_reset)), SET(API_6(cace_ari_set_copy)),            \
     MOVE(API_6(cace_ari_set_move)), HASH(API_2(cace_ari_hash)), CMP(API_6(cace_ari_cmp)),                  \
     EQUAL(API_6(cace_ari_equal)), GET_STR(cace_ari_get_str))

#ifdef __cplusplus
}
#endif

#endif /* CACE_ARI_ALGO_H_ */
