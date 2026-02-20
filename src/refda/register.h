/*
 * Copyright (c) 2011-2026 The Johns Hopkins University Applied Physics
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
#ifndef REFDA_REGISTER_H_
#define REFDA_REGISTER_H_

#include "amm/ident.h"
#include "amm/typedef.h"
#include "amm/const.h"
#include "amm/var.h"
#include "amm/edd.h"
#include "amm/ctrl.h"
#include "amm/oper.h"
#include "amm/sbr.h"
#include "amm/tbr.h"
#include "cace/amm/idseg_ref.h"
#include "cace/amm/obj_ns.h"

#ifdef __cplusplus
extern "C" {
#endif

/** Register an IDENT object with optional user data.
 *
 * @param[in] ns The namespace (ADM or ODM) to register into.
 * The pointer must be non-null.
 * @param obj_id The identifiers for the registered object.
 * The values are copied so these are pure references.
 * @param[in,out] desc The IDENT descriptor to move from.
 * If this pointer is null, a default IDENT descriptor is allocated.
 * @return A non-null pointer to the created object descriptor if successful
 * or NULL if failed.
 */
cace_amm_obj_desc_t *refda_register_ident(cace_amm_obj_ns_t *ns, const cace_amm_idseg_ref_t obj_id,
                                          refda_amm_ident_desc_t *desc);

/// @overload
cace_amm_obj_desc_t *refda_register_typedef(cace_amm_obj_ns_t *ns, const cace_amm_idseg_ref_t obj_id,
                                            refda_amm_typedef_desc_t *desc);

/// @overload
cace_amm_obj_desc_t *refda_register_const(cace_amm_obj_ns_t *ns, const cace_amm_idseg_ref_t obj_id,
                                          refda_amm_const_desc_t *desc);

/// @overload
cace_amm_obj_desc_t *refda_register_var(cace_amm_obj_ns_t *ns, const cace_amm_idseg_ref_t obj_id,
                                        refda_amm_var_desc_t *desc);

/// @overload
cace_amm_obj_desc_t *refda_register_edd(cace_amm_obj_ns_t *ns, const cace_amm_idseg_ref_t obj_id,
                                        refda_amm_edd_desc_t *desc);

/// @overload
cace_amm_obj_desc_t *refda_register_ctrl(cace_amm_obj_ns_t *ns, const cace_amm_idseg_ref_t obj_id,
                                         refda_amm_ctrl_desc_t *desc);

/// @overload
cace_amm_obj_desc_t *refda_register_oper(cace_amm_obj_ns_t *ns, const cace_amm_idseg_ref_t obj_id,
                                         refda_amm_oper_desc_t *desc);

/// @overload
cace_amm_obj_desc_t *refda_register_sbr(cace_amm_obj_ns_t *ns, const cace_amm_idseg_ref_t obj_id,
                                        refda_amm_sbr_desc_t *desc);

/// @overload
cace_amm_obj_desc_t *refda_register_tbr(cace_amm_obj_ns_t *ns, const cace_amm_idseg_ref_t obj_id,
                                        refda_amm_tbr_desc_t *desc);

/** Helper function to append a named parameter to an object descriptor.
 *
 * @param[in,out] obj The object to add a parameter onto.
 * @param[in] name The name of the new parameter.
 * @return The new parameter definition.
 */
cace_amm_formal_param_t *refda_register_add_param(cace_amm_obj_desc_t *obj, const char *name);

#ifdef __cplusplus
} // extern C
#endif

#endif /* REFDA_REGISTER_H_ */
