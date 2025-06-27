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
 * ADM module "ietf-amm-base" for the C-language reference DA.
 * This contains defines for each enumeration in the ADM and
 * declarations of module-level initialization functions.
 */

#ifndef REFDA_ADM_IETF_AMM_BASE_H_
#define REFDA_ADM_IETF_AMM_BASE_H_

#include "refda/agent.h"
#include <cace/util/defs.h>

/*   START CUSTOM INCLUDES HERE  */
/*             TODO              */
/*   STOP CUSTOM INCLUDES HERE  */

#ifdef __cplusplus
extern "C" {
#endif

/// Enumeration of the ADM itself
#define REFDA_ADM_IETF_AMM_BASE_ENUM_ADM 25

/*
 * Enumerations for IDENT objects
 */
/// For ./IDENT/display-hint
#define REFDA_ADM_IETF_AMM_BASE_ENUM_OBJID_IDENT_DISPLAY_HINT 0
/// For ./IDENT/display-hint-integer
#define REFDA_ADM_IETF_AMM_BASE_ENUM_OBJID_IDENT_DISPLAY_HINT_INTEGER 1
/// For ./IDENT/display-hint-float
#define REFDA_ADM_IETF_AMM_BASE_ENUM_OBJID_IDENT_DISPLAY_HINT_FLOAT 2
/// For ./IDENT/display-hint-bstr
#define REFDA_ADM_IETF_AMM_BASE_ENUM_OBJID_IDENT_DISPLAY_HINT_BSTR 3
/// For ./IDENT/display-hint-time
#define REFDA_ADM_IETF_AMM_BASE_ENUM_OBJID_IDENT_DISPLAY_HINT_TIME 4
/// For ./IDENT/bstr-ari-pattern
#define REFDA_ADM_IETF_AMM_BASE_ENUM_OBJID_IDENT_BSTR_ARI_PATTERN 5

/*
 * Enumerations for TYPEDEF objects
 */
/// For ./TYPEDEF/type-ref
#define REFDA_ADM_IETF_AMM_BASE_ENUM_OBJID_TYPEDEF_TYPE_REF 0
/// For ./TYPEDEF/integer
#define REFDA_ADM_IETF_AMM_BASE_ENUM_OBJID_TYPEDEF_INTEGER 1
/// For ./TYPEDEF/float
#define REFDA_ADM_IETF_AMM_BASE_ENUM_OBJID_TYPEDEF_FLOAT 2
/// For ./TYPEDEF/numeric
#define REFDA_ADM_IETF_AMM_BASE_ENUM_OBJID_TYPEDEF_NUMERIC 3
/// For ./TYPEDEF/primitive
#define REFDA_ADM_IETF_AMM_BASE_ENUM_OBJID_TYPEDEF_PRIMITIVE 4
/// For ./TYPEDEF/time
#define REFDA_ADM_IETF_AMM_BASE_ENUM_OBJID_TYPEDEF_TIME 5
/// For ./TYPEDEF/simple
#define REFDA_ADM_IETF_AMM_BASE_ENUM_OBJID_TYPEDEF_SIMPLE 6
/// For ./TYPEDEF/nested
#define REFDA_ADM_IETF_AMM_BASE_ENUM_OBJID_TYPEDEF_NESTED 7
/// For ./TYPEDEF/any
#define REFDA_ADM_IETF_AMM_BASE_ENUM_OBJID_TYPEDEF_ANY 8
/// For ./TYPEDEF/value-obj
#define REFDA_ADM_IETF_AMM_BASE_ENUM_OBJID_TYPEDEF_VALUE_OBJ 9
/// For ./TYPEDEF/nonce
#define REFDA_ADM_IETF_AMM_BASE_ENUM_OBJID_TYPEDEF_NONCE 10
/// For ./TYPEDEF/counter32
#define REFDA_ADM_IETF_AMM_BASE_ENUM_OBJID_TYPEDEF_COUNTER32 11
/// For ./TYPEDEF/counter64
#define REFDA_ADM_IETF_AMM_BASE_ENUM_OBJID_TYPEDEF_COUNTER64 12
/// For ./TYPEDEF/gauge32
#define REFDA_ADM_IETF_AMM_BASE_ENUM_OBJID_TYPEDEF_GAUGE32 13
/// For ./TYPEDEF/gauge64
#define REFDA_ADM_IETF_AMM_BASE_ENUM_OBJID_TYPEDEF_GAUGE64 14
/// For ./TYPEDEF/timestamp
#define REFDA_ADM_IETF_AMM_BASE_ENUM_OBJID_TYPEDEF_TIMESTAMP 15
/// For ./TYPEDEF/eval-tgt
#define REFDA_ADM_IETF_AMM_BASE_ENUM_OBJID_TYPEDEF_EVAL_TGT 16
/// For ./TYPEDEF/expr-item
#define REFDA_ADM_IETF_AMM_BASE_ENUM_OBJID_TYPEDEF_EXPR_ITEM 17
/// For ./TYPEDEF/expr
#define REFDA_ADM_IETF_AMM_BASE_ENUM_OBJID_TYPEDEF_EXPR 18
/// For ./TYPEDEF/exec-tgt
#define REFDA_ADM_IETF_AMM_BASE_ENUM_OBJID_TYPEDEF_EXEC_TGT 19
/// For ./TYPEDEF/exec-item
#define REFDA_ADM_IETF_AMM_BASE_ENUM_OBJID_TYPEDEF_EXEC_ITEM 20
/// For ./TYPEDEF/mac
#define REFDA_ADM_IETF_AMM_BASE_ENUM_OBJID_TYPEDEF_MAC 21
/// For ./TYPEDEF/rpt-tgt
#define REFDA_ADM_IETF_AMM_BASE_ENUM_OBJID_TYPEDEF_RPT_TGT 22
/// For ./TYPEDEF/rptt-item
#define REFDA_ADM_IETF_AMM_BASE_ENUM_OBJID_TYPEDEF_RPTT_ITEM 23
/// For ./TYPEDEF/rptt
#define REFDA_ADM_IETF_AMM_BASE_ENUM_OBJID_TYPEDEF_RPTT 24
/// For ./TYPEDEF/ari-pattern
#define REFDA_ADM_IETF_AMM_BASE_ENUM_OBJID_TYPEDEF_ARI_PATTERN 25

/** Initializer for the ADM module ietf-amm-base.
 * @param[in,out] agent The agent to register this namespace and its
 * objects within.
 * @return Zero upon success.
 */
int refda_adm_ietf_amm_base_init(refda_agent_t *agent);

#ifdef __cplusplus
}
#endif

#endif /* REFDA_ADM_IETF_AMM_BASE_H_ */
