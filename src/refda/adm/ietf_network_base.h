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

/*  START GENERATED SOURCE HERE */
/** @file
 * This is the header for the implementation of the
 * ADM module "ietf-network-base" for the C-language reference DA.
 * This contains defines for each enumeration in the ADM and
 * declarations of module-level initialization functions.
 */

#ifndef REFDA_ADM_IETF_NETWORK_BASE_H_
#define REFDA_ADM_IETF_NETWORK_BASE_H_

#include "refda/agent.h"

#include "cace/util/defs.h"

/*   START CUSTOM INCLUDES HERE */
/*             TODO              */
/*   STOP CUSTOM INCLUDES HERE  */

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Defines for the ADM itself
 */
/// Text name of the organization
#define REFDA_ADM_IETF_NETWORK_BASE_ORG_NAME "ietf"
/// Enumeration of the organization
#define REFDA_ADM_IETF_NETWORK_BASE_ORG_ENUM 1
/// Text name of the model
#define REFDA_ADM_IETF_NETWORK_BASE_MODEL_NAME "network-base"
/// Enumeration of the model
#define REFDA_ADM_IETF_NETWORK_BASE_MODEL_ENUM 26
/// Revision date for the model
#define REFDA_ADM_IETF_NETWORK_BASE_MODEL_REVISION "2026-04-02"
/// @deprecated use REFDA_ADM_IETF_NETWORK_BASE_MODEL_ENUM directly
#define REFDA_ADM_IETF_NETWORK_BASE_ENUM_ADM REFDA_ADM_IETF_NETWORK_BASE_MODEL_ENUM

/*
 * Enumerations for IDENT objects
 */
/// For ./IDENT/display-bstr-uuid
#define REFDA_ADM_IETF_NETWORK_BASE_ENUM_OBJID_IDENT_DISPLAY_BSTR_UUID 17
/// For ./IDENT/display-bstr-oid
#define REFDA_ADM_IETF_NETWORK_BASE_ENUM_OBJID_IDENT_DISPLAY_BSTR_OID 18
/// For ./IDENT/abstract-endpoint
#define REFDA_ADM_IETF_NETWORK_BASE_ENUM_OBJID_IDENT_ABSTRACT_ENDPOINT 0

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

/** Initializer for the ADM module ietf-network-base.
 * @param[in,out] agent The agent to register this namespace and its
 * objects within.
 * @return Zero upon success.
 */
int refda_adm_ietf_network_base_init(refda_agent_t *agent);

#ifdef __cplusplus
} /* extern C */
#endif

#endif /* REFDA_ADM_IETF_NETWORK_BASE_H_ */
/*  STOP GENERATED SOURCE HERE */
