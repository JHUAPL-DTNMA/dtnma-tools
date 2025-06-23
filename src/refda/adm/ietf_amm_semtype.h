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
 * ADM module "ietf-amm-semtype" for the C-language reference DA.
 * This contains defines for each enumeration in the ADM and
 * declarations of module-level initialization functions.
 */

#ifndef REFDA_ADM_IETF_AMM_SEMTYPE_H_
#define REFDA_ADM_IETF_AMM_SEMTYPE_H_

#include "refda/agent.h"
#include <cace/util/defs.h>

/*   START CUSTOM INCLUDES HERE  */
/*             TODO              */
/*   STOP CUSTOM INCLUDES HERE  */

#ifdef __cplusplus
extern "C" {
#endif

/// Enumeration of the ADM itself
#define REFDA_ADM_IETF_AMM_SEMTYPE_ENUM_ADM 24

/*
 * Enumerations for IDENT objects
 */
/// For ./IDENT/semtype
#define REFDA_ADM_IETF_AMM_SEMTYPE_ENUM_OBJID_IDENT_SEMTYPE 1
/// For ./IDENT/type-use
#define REFDA_ADM_IETF_AMM_SEMTYPE_ENUM_OBJID_IDENT_TYPE_USE 2
/// For ./IDENT/ulist
#define REFDA_ADM_IETF_AMM_SEMTYPE_ENUM_OBJID_IDENT_ULIST 3
/// For ./IDENT/dlist
#define REFDA_ADM_IETF_AMM_SEMTYPE_ENUM_OBJID_IDENT_DLIST 4
/// For ./IDENT/umap
#define REFDA_ADM_IETF_AMM_SEMTYPE_ENUM_OBJID_IDENT_UMAP 5
/// For ./IDENT/tblt
#define REFDA_ADM_IETF_AMM_SEMTYPE_ENUM_OBJID_IDENT_TBLT 6
/// For ./IDENT/tblt-col
#define REFDA_ADM_IETF_AMM_SEMTYPE_ENUM_OBJID_IDENT_TBLT_COL 7
/// For ./IDENT/union
#define REFDA_ADM_IETF_AMM_SEMTYPE_ENUM_OBJID_IDENT_UNION 8
/// For ./IDENT/seq
#define REFDA_ADM_IETF_AMM_SEMTYPE_ENUM_OBJID_IDENT_SEQ 9

/*
 * Enumerations for TYPEDEF objects
 */
/// For ./TYPEDEF/semtype
#define REFDA_ADM_IETF_AMM_SEMTYPE_ENUM_OBJID_TYPEDEF_SEMTYPE 25

/** Initializer for the ADM module ietf-amm-semtype.
 * @param[in,out] agent The agent to register this namespace and its
 * objects within.
 * @return Zero upon success.
 */
int refda_adm_ietf_amm_semtype_init(refda_agent_t *agent);

#ifdef __cplusplus
}
#endif

#endif /* REFDA_ADM_IETF_AMM_SEMTYPE_H_ */
