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
 * ADM module "ietf-amm" for the C-language reference DA.
 * This contains defines for each enumeration in the ADM and
 * declarations of module-level initialization functions.
 */

#ifndef REFDA_ADM_IETF_AMM_H_
#define REFDA_ADM_IETF_AMM_H_

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
#define REFDA_ADM_IETF_AMM_ORG_NAME "ietf"
/// Enumeration of the organization
#define REFDA_ADM_IETF_AMM_ORG_ENUM 1
/// Text name of the model
#define REFDA_ADM_IETF_AMM_MODEL_NAME "amm"
/// Enumeration of the model
#define REFDA_ADM_IETF_AMM_MODEL_ENUM 0
/// Revision date for the model
#define REFDA_ADM_IETF_AMM_MODEL_REVISION "2025-07-03"
/// @deprecated use REFDA_ADM_IETF_AMM_MODEL_ENUM directly
#define REFDA_ADM_IETF_AMM_ENUM_ADM REFDA_ADM_IETF_AMM_MODEL_ENUM

/** Initializer for the ADM module ietf-amm.
 * @param[in,out] agent The agent to register this namespace and its
 * objects within.
 * @return Zero upon success.
 */
int refda_adm_ietf_amm_init(refda_agent_t *agent);

#ifdef __cplusplus
} /* extern C */
#endif

#endif /* REFDA_ADM_IETF_AMM_H_ */
/*  STOP GENERATED SOURCE HERE */
