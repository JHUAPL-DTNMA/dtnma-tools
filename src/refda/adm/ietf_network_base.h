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
 * This is the header for the implementation of the
 * ADM module "ietf-network-base" for the C-language reference DA.
 * This contains defines for each enumeration in the ADM and
 * declarations of module-level initialization functions.
 */

#ifndef REFDA_ADM_IETF_NETWORK_BASE_H_
#define REFDA_ADM_IETF_NETWORK_BASE_H_

#include "refda/agent.h"
#include <cace/util/defs.h>

/*   START CUSTOM INCLUDES HERE  */
/*             TODO              */
/*   STOP CUSTOM INCLUDES HERE  */

#ifdef __cplusplus
extern "C" {
#endif

/// Enumeration of the ADM itself
#define REFDA_ADM_IETF_NETWORK_BASE_ENUM_ADM 26

/*
 * Enumerations for IDENT objects
 */
/// For ./IDENT/display-bstr-uuid
#define REFDA_ADM_IETF_NETWORK_BASE_ENUM_OBJID_IDENT_DISPLAY_BSTR_UUID 17
/// For ./IDENT/display-bstr-oid
#define REFDA_ADM_IETF_NETWORK_BASE_ENUM_OBJID_IDENT_DISPLAY_BSTR_OID 18
/// For ./IDENT/abstract-endpoint
#define REFDA_ADM_IETF_NETWORK_BASE_ENUM_OBJID_IDENT_ABSTRACT_ENDPOINT 0
/// For ./IDENT/abstract-endpoint-pattern
#define REFDA_ADM_IETF_NETWORK_BASE_ENUM_OBJID_IDENT_ABSTRACT_ENDPOINT_PATTERN 1
/// For ./IDENT/uri-regexp-pattern
#define REFDA_ADM_IETF_NETWORK_BASE_ENUM_OBJID_IDENT_URI_REGEXP_PATTERN 2

/*
 * Enumerations for TYPEDEF objects
 */
/// For ./TYPEDEF/uri
#define REFDA_ADM_IETF_NETWORK_BASE_ENUM_OBJID_TYPEDEF_URI 0
/// For ./TYPEDEF/uuid
#define REFDA_ADM_IETF_NETWORK_BASE_ENUM_OBJID_TYPEDEF_UUID 5
/// For ./TYPEDEF/oid
#define REFDA_ADM_IETF_NETWORK_BASE_ENUM_OBJID_TYPEDEF_OID 4
/// For ./TYPEDEF/endpoint
#define REFDA_ADM_IETF_NETWORK_BASE_ENUM_OBJID_TYPEDEF_ENDPOINT 1
/// For ./TYPEDEF/endpoint-or-uri
#define REFDA_ADM_IETF_NETWORK_BASE_ENUM_OBJID_TYPEDEF_ENDPOINT_OR_URI 3
/// For ./TYPEDEF/endpoint-pattern
#define REFDA_ADM_IETF_NETWORK_BASE_ENUM_OBJID_TYPEDEF_ENDPOINT_PATTERN 2

/** Initializer for the ADM module ietf-network-base.
 * @param[in,out] agent The agent to register this namespace and its
 * objects within.
 * @return Zero upon success.
 */
int refda_adm_ietf_network_base_init(refda_agent_t *agent);

#ifdef __cplusplus
}
#endif

#endif /* REFDA_ADM_IETF_NETWORK_BASE_H_ */
