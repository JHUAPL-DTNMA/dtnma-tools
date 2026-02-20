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

/** @file
 * This is the header for the implementation of the
 * ADM module "ietf-alarms" for the C-language reference DA.
 * This contains defines for each enumeration in the ADM and
 * declarations of module-level initialization functions.
 */

#ifndef REFDA_ADM_IETF_ALARMS_H_
#define REFDA_ADM_IETF_ALARMS_H_

#include "refda/agent.h"
#include <cace/util/defs.h>

/*   START CUSTOM INCLUDES HERE  */
/*             TODO              */
/*   STOP CUSTOM INCLUDES HERE  */

#ifdef __cplusplus
extern "C" {
#endif

/// Enumeration of the ADM itself
#define REFDA_ADM_IETF_ALARMS_ENUM_ADM 4

/*
 * Enumerations for IDENT objects
 */
/// For ./IDENT/resource
#define REFDA_ADM_IETF_ALARMS_ENUM_OBJID_IDENT_RESOURCE 0
/// For ./IDENT/category
#define REFDA_ADM_IETF_ALARMS_ENUM_OBJID_IDENT_CATEGORY 1

/*
 * Enumerations for TYPEDEF objects
 */
/// For ./TYPEDEF/severity
#define REFDA_ADM_IETF_ALARMS_ENUM_OBJID_TYPEDEF_SEVERITY 0
/// For ./TYPEDEF/manager-state
#define REFDA_ADM_IETF_ALARMS_ENUM_OBJID_TYPEDEF_MANAGER_STATE 1
/// For ./TYPEDEF/alarm-filter
#define REFDA_ADM_IETF_ALARMS_ENUM_OBJID_TYPEDEF_ALARM_FILTER 2
/// For ./TYPEDEF/shelf-tblt
#define REFDA_ADM_IETF_ALARMS_ENUM_OBJID_TYPEDEF_SHELF_TBLT 3

/*
 * Enumerations for EDD objects
 */
/// For ./EDD/alarm-list
#define REFDA_ADM_IETF_ALARMS_ENUM_OBJID_EDD_ALARM_LIST 0
/// For ./EDD/resource-inventory
#define REFDA_ADM_IETF_ALARMS_ENUM_OBJID_EDD_RESOURCE_INVENTORY 1
/// For ./EDD/category-inventory
#define REFDA_ADM_IETF_ALARMS_ENUM_OBJID_EDD_CATEGORY_INVENTORY 2
/// For ./EDD/shelf-list
#define REFDA_ADM_IETF_ALARMS_ENUM_OBJID_EDD_SHELF_LIST 3

/*
 * Enumerations for CTRL objects
 */
/// For ./CTRL/purge-alarms
#define REFDA_ADM_IETF_ALARMS_ENUM_OBJID_CTRL_PURGE_ALARMS 2
/// For ./CTRL/compress-alarms
#define REFDA_ADM_IETF_ALARMS_ENUM_OBJID_CTRL_COMPRESS_ALARMS 3
/// For ./CTRL/set-alarms-manager-state
#define REFDA_ADM_IETF_ALARMS_ENUM_OBJID_CTRL_SET_ALARMS_MANAGER_STATE 4
/// For ./CTRL/ensure-shelf
#define REFDA_ADM_IETF_ALARMS_ENUM_OBJID_CTRL_ENSURE_SHELF 0
/// For ./CTRL/discard-shelf
#define REFDA_ADM_IETF_ALARMS_ENUM_OBJID_CTRL_DISCARD_SHELF 1

/** Initializer for the ADM module ietf-alarms.
 * @param[in,out] agent The agent to register this namespace and its
 * objects within.
 * @return Zero upon success.
 */
int refda_adm_ietf_alarms_init(refda_agent_t *agent);

#ifdef __cplusplus
}
#endif

#endif /* REFDA_ADM_IETF_ALARMS_H_ */
