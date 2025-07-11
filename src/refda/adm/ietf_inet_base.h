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
 * ADM module "ietf-inet-base" for the C-language reference DA.
 * This contains defines for each enumeration in the ADM and
 * declarations of module-level initialization functions.
 */

#ifndef REFDA_ADM_IETF_INET_BASE_H_
#define REFDA_ADM_IETF_INET_BASE_H_

#include "refda/agent.h"
#include <cace/util/defs.h>

/*   START CUSTOM INCLUDES HERE  */
/*             TODO              */
/*   STOP CUSTOM INCLUDES HERE  */

#ifdef __cplusplus
extern "C" {
#endif

/// Enumeration of the ADM itself
#define REFDA_ADM_IETF_INET_BASE_ENUM_ADM 4

/*
 * Enumerations for IDENT objects
 */
/// For ./IDENT/display-bstr-ipaddress
#define REFDA_ADM_IETF_INET_BASE_ENUM_OBJID_IDENT_DISPLAY_BSTR_IPADDRESS 2
/// For ./IDENT/ip-endpoint
#define REFDA_ADM_IETF_INET_BASE_ENUM_OBJID_IDENT_IP_ENDPOINT 0
/// For ./IDENT/ip-transport
#define REFDA_ADM_IETF_INET_BASE_ENUM_OBJID_IDENT_IP_TRANSPORT 1
/// For ./IDENT/ip-vlsm-pattern
#define REFDA_ADM_IETF_INET_BASE_ENUM_OBJID_IDENT_IP_VLSM_PATTERN 3

/*
 * Enumerations for TYPEDEF objects
 */
/// For ./TYPEDEF/ipv4-address
#define REFDA_ADM_IETF_INET_BASE_ENUM_OBJID_TYPEDEF_IPV4_ADDRESS 0
/// For ./TYPEDEF/ipv6-address
#define REFDA_ADM_IETF_INET_BASE_ENUM_OBJID_TYPEDEF_IPV6_ADDRESS 1
/// For ./TYPEDEF/ip-address
#define REFDA_ADM_IETF_INET_BASE_ENUM_OBJID_TYPEDEF_IP_ADDRESS 2
/// For ./TYPEDEF/zone
#define REFDA_ADM_IETF_INET_BASE_ENUM_OBJID_TYPEDEF_ZONE 4
/// For ./TYPEDEF/port-number
#define REFDA_ADM_IETF_INET_BASE_ENUM_OBJID_TYPEDEF_PORT_NUMBER 3
/// For ./TYPEDEF/domain-name
#define REFDA_ADM_IETF_INET_BASE_ENUM_OBJID_TYPEDEF_DOMAIN_NAME 5

/** Initializer for the ADM module ietf-inet-base.
 * @param[in,out] agent The agent to register this namespace and its
 * objects within.
 * @return Zero upon success.
 */
int refda_adm_ietf_inet_base_init(refda_agent_t *agent);

#ifdef __cplusplus
}
#endif

#endif /* REFDA_ADM_IETF_INET_BASE_H_ */
