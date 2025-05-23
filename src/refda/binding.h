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
#ifndef REFDA_BINDING_H_
#define REFDA_BINDING_H_

#include <cace/amm/obj_desc.h>
#include <cace/amm/obj_store.h>
#include <cace/ari/type.h>

#ifdef __cplusplus
extern "C" {
#endif

/** Perform a type binding into an object store.
 *
 * @param[in,out] obj The object to bind.
 * @param[in] store The object store to lookup within.
 * @return Zero if successful.
 * Otherwise, the number of individual bindings which have failed.
 */
int refda_binding_ident(cace_amm_obj_desc_t *obj, const cace_amm_obj_store_t *store);

/// @overload
int refda_binding_typedef(cace_amm_obj_desc_t *obj, const cace_amm_obj_store_t *store);

/// @overload
int refda_binding_const(cace_amm_obj_desc_t *obj, const cace_amm_obj_store_t *store);

/// @overload
int refda_binding_var(cace_amm_obj_desc_t *obj, const cace_amm_obj_store_t *store);

/// @overload
int refda_binding_edd(cace_amm_obj_desc_t *obj, const cace_amm_obj_store_t *store);

/// @overload
int refda_binding_ctrl(cace_amm_obj_desc_t *obj, const cace_amm_obj_store_t *store);

/// @overload
int refda_binding_oper(cace_amm_obj_desc_t *obj, const cace_amm_obj_store_t *store);

/// @overload
int refda_binding_sbr(cace_amm_obj_desc_t *obj, const cace_amm_obj_store_t *store);

/// @overload
int refda_binding_tbr(cace_amm_obj_desc_t *obj, const cace_amm_obj_store_t *store);

/** Perform a type binding into an object store.
 *
 * @param obj_type The object type being bound.
 * @param[in,out] obj The object to bind.
 * @param[in] store The object store to lookup within.
 * @return Zero if successful.
 * Otherwise, the number of individual bindings which have failed.
 */
int refda_binding_obj(cace_ari_type_t obj_type, cace_amm_obj_desc_t *obj, const cace_amm_obj_store_t *store);

#ifdef __cplusplus
} // extern C
#endif

#endif /* REFDA_BINDING_H_ */
