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
 * ADM module "ietf-dtnma-agent-acl" for the C-language reference DA.
 * This contains defines for each enumeration in the ADM and
 * declarations of module-level initialization functions.
 */

#ifndef REFDA_ADM_IETF_DTNMA_AGENT_ACL_H_
#define REFDA_ADM_IETF_DTNMA_AGENT_ACL_H_

#include "refda/agent.h"
#include <cace/util/defs.h>

/*   START CUSTOM INCLUDES HERE  */
/*             TODO              */
/*   STOP CUSTOM INCLUDES HERE  */

#ifdef __cplusplus
extern "C" {
#endif

/// Enumeration of the ADM itself
#define REFDA_ADM_IETF_DTNMA_AGENT_ACL_ENUM_ADM 2

/*
 * Enumerations for IDENT objects
 */
/// For ./IDENT/permission
#define REFDA_ADM_IETF_DTNMA_AGENT_ACL_ENUM_OBJID_IDENT_PERMISSION 0
/// For ./IDENT/execute
#define REFDA_ADM_IETF_DTNMA_AGENT_ACL_ENUM_OBJID_IDENT_EXECUTE 10
/// For ./IDENT/produce
#define REFDA_ADM_IETF_DTNMA_AGENT_ACL_ENUM_OBJID_IDENT_PRODUCE 11
/// For ./IDENT/modify-var
#define REFDA_ADM_IETF_DTNMA_AGENT_ACL_ENUM_OBJID_IDENT_MODIFY_VAR 12
/// For ./IDENT/create-odm
#define REFDA_ADM_IETF_DTNMA_AGENT_ACL_ENUM_OBJID_IDENT_CREATE_ODM 13
/// For ./IDENT/delete-odm
#define REFDA_ADM_IETF_DTNMA_AGENT_ACL_ENUM_OBJID_IDENT_DELETE_ODM 14
/// For ./IDENT/create-object
#define REFDA_ADM_IETF_DTNMA_AGENT_ACL_ENUM_OBJID_IDENT_CREATE_OBJECT 15
/// For ./IDENT/delete-object
#define REFDA_ADM_IETF_DTNMA_AGENT_ACL_ENUM_OBJID_IDENT_DELETE_OBJECT 16

/*
 * Enumerations for TYPEDEF objects
 */
/// For ./TYPEDEF/entry-id
#define REFDA_ADM_IETF_DTNMA_AGENT_ACL_ENUM_OBJID_TYPEDEF_ENTRY_ID 2
/// For ./TYPEDEF/optional-entry-id
#define REFDA_ADM_IETF_DTNMA_AGENT_ACL_ENUM_OBJID_TYPEDEF_OPTIONAL_ENTRY_ID 3
/// For ./TYPEDEF/permission-list
#define REFDA_ADM_IETF_DTNMA_AGENT_ACL_ENUM_OBJID_TYPEDEF_PERMISSION_LIST 0

/*
 * Enumerations for VAR objects
 */
/// For ./VAR/default-access
#define REFDA_ADM_IETF_DTNMA_AGENT_ACL_ENUM_OBJID_VAR_DEFAULT_ACCESS 1

/*
 * Enumerations for EDD objects
 */
/// For ./EDD/access-list
#define REFDA_ADM_IETF_DTNMA_AGENT_ACL_ENUM_OBJID_EDD_ACCESS_LIST 2
/// For ./EDD/current-group-id
#define REFDA_ADM_IETF_DTNMA_AGENT_ACL_ENUM_OBJID_EDD_CURRENT_GROUP_ID 3
/// For ./EDD/group-list
#define REFDA_ADM_IETF_DTNMA_AGENT_ACL_ENUM_OBJID_EDD_GROUP_LIST 4

/*
 * Enumerations for CTRL objects
 */
/// For ./CTRL/ensure-access
#define REFDA_ADM_IETF_DTNMA_AGENT_ACL_ENUM_OBJID_CTRL_ENSURE_ACCESS 3
/// For ./CTRL/discard-access
#define REFDA_ADM_IETF_DTNMA_AGENT_ACL_ENUM_OBJID_CTRL_DISCARD_ACCESS 4
/// For ./CTRL/ensure-group
#define REFDA_ADM_IETF_DTNMA_AGENT_ACL_ENUM_OBJID_CTRL_ENSURE_GROUP 5
/// For ./CTRL/ensure-group-members
#define REFDA_ADM_IETF_DTNMA_AGENT_ACL_ENUM_OBJID_CTRL_ENSURE_GROUP_MEMBERS 7
/// For ./CTRL/discard-group
#define REFDA_ADM_IETF_DTNMA_AGENT_ACL_ENUM_OBJID_CTRL_DISCARD_GROUP 6

/** Initializer for the ADM module ietf-dtnma-agent-acl.
 * @param[in,out] agent The agent to register this namespace and its
 * objects within.
 * @return Zero upon success.
 */
int refda_adm_ietf_dtnma_agent_acl_init(refda_agent_t *agent);

#ifdef __cplusplus
}
#endif

#endif /* REFDA_ADM_IETF_DTNMA_AGENT_ACL_H_ */
