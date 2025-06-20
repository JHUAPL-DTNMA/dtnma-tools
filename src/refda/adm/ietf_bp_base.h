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
 * This is the header for the implementation of the
 * ADM module "ietf-bp-base" for the C-language reference DA.
 * This contains defines for each enumeration in the ADM and
 * declarations of module-level initialization functions.
 */

#ifndef REFDA_ADM_IETF_BP_BASE_H_
#define REFDA_ADM_IETF_BP_BASE_H_

#include "refda/agent.h"
#include <cace/util/defs.h>

/*   START CUSTOM INCLUDES HERE  */
/*             TODO              */
/*   STOP CUSTOM INCLUDES HERE  */

#ifdef __cplusplus
extern "C" {
#endif

/// Enumeration of the ADM itself
#define REFDA_ADM_IETF_BP_BASE_ENUM_ADM 5

/*
 * Enumerations for IDENT objects
 */
/// For ./IDENT/display-bstr-eid
#define REFDA_ADM_IETF_BP_BASE_ENUM_OBJID_IDENT_DISPLAY_BSTR_EID 1
/// For ./IDENT/bp-endpoint
#define REFDA_ADM_IETF_BP_BASE_ENUM_OBJID_IDENT_BP_ENDPOINT 0
/// For ./IDENT/bp-eid-pattern
#define REFDA_ADM_IETF_BP_BASE_ENUM_OBJID_IDENT_BP_EID_PATTERN 2

/*
 * Enumerations for TYPEDEF objects
 */
/// For ./TYPEDEF/eid-text
#define REFDA_ADM_IETF_BP_BASE_ENUM_OBJID_TYPEDEF_EID_TEXT 0
/// For ./TYPEDEF/eid-cbor
#define REFDA_ADM_IETF_BP_BASE_ENUM_OBJID_TYPEDEF_EID_CBOR 1

/** Initializer for the ADM module ietf-bp-base.
 * @param[in,out] agent The agent to register this namespace and its
 * objects within.
 * @return Zero upon success.
 */
int refda_adm_ietf_bp_base_init(refda_agent_t *agent);

#ifdef __cplusplus
}
#endif

#endif /* REFDA_ADM_IETF_BP_BASE_H_ */
