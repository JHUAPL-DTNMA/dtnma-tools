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
 * ADM module "iana-display-hints" for the C-language reference DA.
 * This contains defines for each enumeration in the ADM and
 * declarations of module-level initialization functions.
 */

#ifndef REFDA_ADM_IANA_DISPLAY_HINTS_H_
#define REFDA_ADM_IANA_DISPLAY_HINTS_H_

#include "refda/agent.h"
#include <cace/util/defs.h>

/*   START CUSTOM INCLUDES HERE  */
/*             TODO              */
/*   STOP CUSTOM INCLUDES HERE  */

#ifdef __cplusplus
extern "C" {
#endif

/// Enumeration of the ADM itself
#define REFDA_ADM_IANA_DISPLAY_HINTS_ENUM_ADM 0

/*
 * Enumerations for IDENT objects
 */
/// For ./IDENT/base-integer
#define REFDA_ADM_IANA_DISPLAY_HINTS_ENUM_OBJID_IDENT_BASE_INTEGER 1
/// For ./IDENT/display-int-dec
#define REFDA_ADM_IANA_DISPLAY_HINTS_ENUM_OBJID_IDENT_DISPLAY_INT_DEC 2
/// For ./IDENT/display-int-bin
#define REFDA_ADM_IANA_DISPLAY_HINTS_ENUM_OBJID_IDENT_DISPLAY_INT_BIN 3
/// For ./IDENT/display-int-hex
#define REFDA_ADM_IANA_DISPLAY_HINTS_ENUM_OBJID_IDENT_DISPLAY_INT_HEX 4
/// For ./IDENT/base-float
#define REFDA_ADM_IANA_DISPLAY_HINTS_ENUM_OBJID_IDENT_BASE_FLOAT 5
/// For ./IDENT/display-float-dec
#define REFDA_ADM_IANA_DISPLAY_HINTS_ENUM_OBJID_IDENT_DISPLAY_FLOAT_DEC 6
/// For ./IDENT/display-float-exp
#define REFDA_ADM_IANA_DISPLAY_HINTS_ENUM_OBJID_IDENT_DISPLAY_FLOAT_EXP 7
/// For ./IDENT/display-float-hex
#define REFDA_ADM_IANA_DISPLAY_HINTS_ENUM_OBJID_IDENT_DISPLAY_FLOAT_HEX 8
/// For ./IDENT/base-bstr
#define REFDA_ADM_IANA_DISPLAY_HINTS_ENUM_OBJID_IDENT_BASE_BSTR 9
/// For ./IDENT/display-bstr-text
#define REFDA_ADM_IANA_DISPLAY_HINTS_ENUM_OBJID_IDENT_DISPLAY_BSTR_TEXT 10
/// For ./IDENT/display-bstr-base16
#define REFDA_ADM_IANA_DISPLAY_HINTS_ENUM_OBJID_IDENT_DISPLAY_BSTR_BASE16 11
/// For ./IDENT/display-bstr-base64
#define REFDA_ADM_IANA_DISPLAY_HINTS_ENUM_OBJID_IDENT_DISPLAY_BSTR_BASE64 12
/// For ./IDENT/display-bstr-ipaddress
#define REFDA_ADM_IANA_DISPLAY_HINTS_ENUM_OBJID_IDENT_DISPLAY_BSTR_IPADDRESS 16
/// For ./IDENT/display-bstr-uuid
#define REFDA_ADM_IANA_DISPLAY_HINTS_ENUM_OBJID_IDENT_DISPLAY_BSTR_UUID 17
/// For ./IDENT/base-time
#define REFDA_ADM_IANA_DISPLAY_HINTS_ENUM_OBJID_IDENT_BASE_TIME 13
/// For ./IDENT/display-time-text
#define REFDA_ADM_IANA_DISPLAY_HINTS_ENUM_OBJID_IDENT_DISPLAY_TIME_TEXT 14
/// For ./IDENT/display-time-dec
#define REFDA_ADM_IANA_DISPLAY_HINTS_ENUM_OBJID_IDENT_DISPLAY_TIME_DEC 15

/** Initializer for the ADM module iana-display-hints.
 * @param[in,out] agent The agent to register this namespace and its
 * objects within.
 * @return Zero upon success.
 */
int refda_adm_iana_display_hints_init(refda_agent_t *agent);

#ifdef __cplusplus
}
#endif

#endif /* REFDA_ADM_IANA_DISPLAY_HINTS_H_ */
