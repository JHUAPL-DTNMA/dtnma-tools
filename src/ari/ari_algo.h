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
 * This file contains algorithmic functions to operate on ARIs recursively.
 */
#ifndef ARI_ARI_ALGO_H_
#define ARI_ARI_ALGO_H_

#include "ari_base.h"

#ifdef __cplusplus
extern "C" {
#endif

/** Visitor context used for ari_visitor_t functions.
 */
typedef struct
{
    /** The parent value of the one being visited.
     * If this is a literal with a container type, the visited value is a
     * member of that container.
     * If this is an object reference, the visited value is a parameter.
     */
    const ari_t *parent;

    /// True if the parent is an AM literal and this is a map key
    bool is_map_key;

    /// User-supplied pointer to ari_visit()
    void *user_data;
} ari_visit_ctx_t;

/** Function pointers to implement the ari_visit() behavior.
 */
typedef struct
{

    /** Called when visiting each ARI.
     * For containers this is called before any contained values.
     *
     * @param[in] ari The value being visited.
     * @param[in] user_data Any user data supplied to ari_visit().
     * @return Zero to continue iterating, or non-zero to stop immediately.
     */
    int (*visit_ari)(const ari_t *ari, const ari_visit_ctx_t *ctx);

    /// @overload
    int (*visit_ref)(const ari_ref_t *obj, const ari_visit_ctx_t *ctx);

    /// @overload
    int (*visit_objpath)(const ari_objpath_t *obj, const ari_visit_ctx_t *ctx);

    /** @overload
     * This is called before and in addition to any visiting of container
     * contents.
     */
    int (*visit_lit)(const ari_lit_t *obj, const ari_visit_ctx_t *ctx);

} ari_visitor_t;

/** Visit an ARI recursively.
 *
 * @param[in] ari The value to visit.
 * @param[in] visitor The visitor descriptor.
 * @param[in] user_data Data passed to the ari_visitor_t functions.
 * @return Zero upon success, or non-zero if one of the visitor functions
 * returned non-zero.
 */
int ari_visit(const ari_t *ari, const ari_visitor_t *visitor, void *user_data);

/** Function pointers to implement the ari_translate() behavior.
 * For any input value, the #map_ari is first called (with fallback behavior)
 * followed by either #map_objpath or #map_lit depending on the ARI type.
 *
 */
typedef struct
{

    /** Called when translating each ARI.
     * For containers this is called before any contained values.
     * If not provided, the literal/objref tag will be copied.
     *
     * @pre The @c out value is already initialized.
     * @param[out] out The produced value.
     * @param[in] in The value being mapped.
     * @param[in] user_data Any user data supplied to ari_translate().
     * @return Zero to continue iterating, or non-zero to stop immediately.
     */
    int (*map_ari)(ari_t *out, const ari_t *in, void *user_data);

    /** @overload
     * If not provided, the standard ari_objpath_copy() will be used.
     */
    int (*map_objpath)(ari_objpath_t *out, const ari_objpath_t *in, void *user_data);

    /** @overload
     * If not provided, the standard ari_lit_copy() will be used.
     */
    int (*map_lit)(ari_lit_t *out, const ari_lit_t *in, void *user_data);

} ari_translator_t;

/** Translate from an input ARI into an output of the same structure.
 *
 * @pre The @c out value is already initialized.
 * @param[out] out The result of translation.
 * @param[in] in The value to translate.
 * @param[in] translator The translator descriptor.
 * @param[in] user_data Data passed to the ari_translator_t functions.
 * @return Zero upon success, or non-zero if one of the translator functions
 * returned non-zero.
 */
int ari_translate(ari_t *out, const ari_t *in, const ari_translator_t *translator, void *user_data);

/** Interface for M*LIB @c HASH operation.
 * This uses the ari_visit() internally to walk an ARI tree.
 *
 * @param[in] ari The object to hash, which contains nested state.
 * @return The hash value.
 */
size_t ari_hash(const ari_t *ari);

/** Determine if two ARIs have identical value.
 *
 * @param left One value to compare.
 * @param right Other value to compare.
 * @return True if the two are by-value equal.
 */
bool ari_equal(const ari_t *left, const ari_t *right);

#ifdef __cplusplus
}
#endif

#endif /* ARI_ARI_ALGO_H_ */
