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
#ifndef TEST_UTIL_AGENT_H_
#define TEST_UTIL_AGENT_H_

#include <refda/agent.h>
#include <refda/adm/ietf_amm.h>
#include <refda/adm/ietf_amm_base.h>
#include <refda/adm/ietf_amm_semtype.h>
#include <refda/adm/ietf_network_base.h>
#include <refda/adm/ietf_dtnma_agent.h>
#include <refda/adm/ietf_dtnma_agent_acl.h>

#ifdef __cplusplus
extern "C" {
#endif

/// Register critical base ADMs to the agent
void test_util_agent_crit_adms(refda_agent_t *agent);

/// Add access permissions to agent group 0
void test_util_agent_permission(refda_agent_t *agent, cace_ari_int_id_t obj_id);

/// Add group for specific URI pattern
void test_util_group_add(refda_agent_t *agent, refda_acl_id_t group_id, const char *uri_pattern);

/// Add access permission for specific group
void test_util_group_permission(refda_agent_t *agent, refda_acl_id_t group_id, cace_ari_int_id_t obj_id);

/** Execute a target in the main test thread.
 * This assumes the target does not contain any deferred callbacks.
 *
 * @param agent The agent to execute within.
 * @param[in] target The target to expand, execute, and check success for.
 */
void test_util_agent_check_execute(refda_agent_t *agent, const cace_ari_t *target);

#ifdef __cplusplus
} // extern C
#endif

#endif /* TEST_UTIL_AGENT_H_ */
