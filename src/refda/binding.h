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
#ifndef REFDA_BINDING_H_
#define REFDA_BINDING_H_

#include <cace/amm/obj_desc.h>
#include <cace/amm/obj_store.h>
#include <cace/ari/type.h>

#ifdef __cplusplus
extern "C" {
#endif

/** Context to bind an individual object.
 *
 */
typedef struct
{
    /// The store in which the objects reside
    const cace_amm_obj_store_t *store;
    /// The namespace of the object being bound
    cace_amm_obj_ns_t *ns;
} refda_binding_ctx_t;

/** Perform a type binding on a semantic type object.
 *
 * @param[in] ctx The context for the object.
 * @param[in,out] typeobj The object to bind.
 * @return Zero if successful.
 * Otherwise, the number of individual bindings which have failed.
 */
int refda_binding_typeobj(const refda_binding_ctx_t *ctx, cace_amm_type_t *typeobj);

/** Perform a type binding into an object store.
 *
 * @param[in] ctx The context for the object.
 * @param[in,out] obj The object to bind.
 * @return Zero if successful.
 * Otherwise, the number of individual bindings which have failed.
 */
int refda_binding_ident(const refda_binding_ctx_t *ctx, cace_amm_obj_desc_t *obj);

/// @overload
int refda_binding_typedef(const refda_binding_ctx_t *ctx, cace_amm_obj_desc_t *obj);

/// @overload
int refda_binding_const(const refda_binding_ctx_t *ctx, cace_amm_obj_desc_t *obj);

/// @overload
int refda_binding_var(const refda_binding_ctx_t *ctx, cace_amm_obj_desc_t *obj);

/// @overload
int refda_binding_edd(const refda_binding_ctx_t *ctx, cace_amm_obj_desc_t *obj);

/// @overload
int refda_binding_ctrl(const refda_binding_ctx_t *ctx, cace_amm_obj_desc_t *obj);

/// @overload
int refda_binding_oper(const refda_binding_ctx_t *ctx, cace_amm_obj_desc_t *obj);

/// @overload
int refda_binding_sbr(const refda_binding_ctx_t *ctx, cace_amm_obj_desc_t *obj);

/// @overload
int refda_binding_tbr(const refda_binding_ctx_t *ctx, cace_amm_obj_desc_t *obj);

/** Perform a type binding into an object store.
 *
 * @param obj_type The object type being bound.
 * @param[in,out] obj The object to bind.
 * @param[in] store The object store to lookup within.
 * @return Zero if successful.
 * Otherwise, the number of individual bindings which have failed.
 */
int refda_binding_obj(const refda_binding_ctx_t *ctx, cace_ari_type_t obj_type, cace_amm_obj_desc_t *obj);

#ifdef __cplusplus
} // extern C
#endif

#endif /* REFDA_BINDING_H_ */
