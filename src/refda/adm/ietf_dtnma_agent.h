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
#include "refda/agent.h"
#include <cace/util/defs.h>

/*   START CUSTOM INCLUDES HERE  */
/*             TODO              */
/*   STOP CUSTOM INCLUDES HERE  */
#ifndef REFDA_ADM_IETF_DTNMA_AGENT_H_
#define REFDA_ADM_IETF_DTNMA_AGENT_H_

#ifdef __cplusplus
extern "C" {
#endif

/// Enumeration of the ADM itself
#define REFDA_ADM_IETF_DTNMA_AGENT_ENUM_ADM 1

/*
 * Enumerations for CONST objects
 */
/// For ./CONST/hello
#define REFDA_ADM_IETF_DTNMA_AGENT_ENUM_OBJID_CONST_HELLO 0

/*
 * Enumerations for EDD objects
 */
/// For ./EDD/sw-vendor
#define REFDA_ADM_IETF_DTNMA_AGENT_ENUM_OBJID_EDD_SW_VENDOR 0
/// For ./EDD/sw-version
#define REFDA_ADM_IETF_DTNMA_AGENT_ENUM_OBJID_EDD_SW_VERSION 1
/// For ./EDD/capability
#define REFDA_ADM_IETF_DTNMA_AGENT_ENUM_OBJID_EDD_CAPABILITY 2
/// For ./EDD/num-msg-rx
#define REFDA_ADM_IETF_DTNMA_AGENT_ENUM_OBJID_EDD_NUM_MSG_RX 3
/// For ./EDD/num-msg-rx-failed
#define REFDA_ADM_IETF_DTNMA_AGENT_ENUM_OBJID_EDD_NUM_MSG_RX_FAILED 4
/// For ./EDD/num-msg-tx
#define REFDA_ADM_IETF_DTNMA_AGENT_ENUM_OBJID_EDD_NUM_MSG_TX 5
/// For ./EDD/num-exec-started
#define REFDA_ADM_IETF_DTNMA_AGENT_ENUM_OBJID_EDD_NUM_EXEC_STARTED 6
/// For ./EDD/num-exec-succeeded
#define REFDA_ADM_IETF_DTNMA_AGENT_ENUM_OBJID_EDD_NUM_EXEC_SUCCEEDED 7
/// For ./EDD/num-exec-failed
#define REFDA_ADM_IETF_DTNMA_AGENT_ENUM_OBJID_EDD_NUM_EXEC_FAILED 8
/// For ./EDD/exec-running
#define REFDA_ADM_IETF_DTNMA_AGENT_ENUM_OBJID_EDD_EXEC_RUNNING 9
/// For ./EDD/typedef-list
#define REFDA_ADM_IETF_DTNMA_AGENT_ENUM_OBJID_EDD_TYPEDEF_LIST 10
/// For ./EDD/var-list
#define REFDA_ADM_IETF_DTNMA_AGENT_ENUM_OBJID_EDD_VAR_LIST 11
/// For ./EDD/sbr-list
#define REFDA_ADM_IETF_DTNMA_AGENT_ENUM_OBJID_EDD_SBR_LIST 12
/// For ./EDD/tbr-list
#define REFDA_ADM_IETF_DTNMA_AGENT_ENUM_OBJID_EDD_TBR_LIST 13

/*
 * Enumerations for CTRL objects
 */
/// For ./CTRL/if-then-else
#define REFDA_ADM_IETF_DTNMA_AGENT_ENUM_OBJID_CTRL_IF_THEN_ELSE 0
/// For ./CTRL/catch
#define REFDA_ADM_IETF_DTNMA_AGENT_ENUM_OBJID_CTRL_CATCH 1
/// For ./CTRL/wait-for
#define REFDA_ADM_IETF_DTNMA_AGENT_ENUM_OBJID_CTRL_WAIT_FOR 2
/// For ./CTRL/wait-until
#define REFDA_ADM_IETF_DTNMA_AGENT_ENUM_OBJID_CTRL_WAIT_UNTIL 3
/// For ./CTRL/wait-cond
#define REFDA_ADM_IETF_DTNMA_AGENT_ENUM_OBJID_CTRL_WAIT_COND 4
/// For ./CTRL/inspect
#define REFDA_ADM_IETF_DTNMA_AGENT_ENUM_OBJID_CTRL_INSPECT 5
/// For ./CTRL/report-on
#define REFDA_ADM_IETF_DTNMA_AGENT_ENUM_OBJID_CTRL_REPORT_ON 6
/// For ./CTRL/var-reset
#define REFDA_ADM_IETF_DTNMA_AGENT_ENUM_OBJID_CTRL_VAR_RESET 7
/// For ./CTRL/var-store
#define REFDA_ADM_IETF_DTNMA_AGENT_ENUM_OBJID_CTRL_VAR_STORE 8
/// For ./CTRL/ensure-var
#define REFDA_ADM_IETF_DTNMA_AGENT_ENUM_OBJID_CTRL_ENSURE_VAR 9
/// For ./CTRL/discard-var
#define REFDA_ADM_IETF_DTNMA_AGENT_ENUM_OBJID_CTRL_DISCARD_VAR 10

/*
 * Enumerations for OPER objects
 */
/// For ./OPER/negate
#define REFDA_ADM_IETF_DTNMA_AGENT_ENUM_OBJID_OPER_NEGATE 0
/// For ./OPER/add
#define REFDA_ADM_IETF_DTNMA_AGENT_ENUM_OBJID_OPER_ADD 1
/// For ./OPER/sub
#define REFDA_ADM_IETF_DTNMA_AGENT_ENUM_OBJID_OPER_SUB 2
/// For ./OPER/multiply
#define REFDA_ADM_IETF_DTNMA_AGENT_ENUM_OBJID_OPER_MULTIPLY 3
/// For ./OPER/divide
#define REFDA_ADM_IETF_DTNMA_AGENT_ENUM_OBJID_OPER_DIVIDE 4
/// For ./OPER/remainder
#define REFDA_ADM_IETF_DTNMA_AGENT_ENUM_OBJID_OPER_REMAINDER 5
/// For ./OPER/bit-not
#define REFDA_ADM_IETF_DTNMA_AGENT_ENUM_OBJID_OPER_BIT_NOT 6
/// For ./OPER/bit-and
#define REFDA_ADM_IETF_DTNMA_AGENT_ENUM_OBJID_OPER_BIT_AND 7
/// For ./OPER/bit-or
#define REFDA_ADM_IETF_DTNMA_AGENT_ENUM_OBJID_OPER_BIT_OR 8
/// For ./OPER/bit-xor
#define REFDA_ADM_IETF_DTNMA_AGENT_ENUM_OBJID_OPER_BIT_XOR 9
/// For ./OPER/bool-not
#define REFDA_ADM_IETF_DTNMA_AGENT_ENUM_OBJID_OPER_BOOL_NOT 10
/// For ./OPER/bool-and
#define REFDA_ADM_IETF_DTNMA_AGENT_ENUM_OBJID_OPER_BOOL_AND 11
/// For ./OPER/bool-or
#define REFDA_ADM_IETF_DTNMA_AGENT_ENUM_OBJID_OPER_BOOL_OR 12
/// For ./OPER/bool-xor
#define REFDA_ADM_IETF_DTNMA_AGENT_ENUM_OBJID_OPER_BOOL_XOR 13
/// For ./OPER/compare-eq
#define REFDA_ADM_IETF_DTNMA_AGENT_ENUM_OBJID_OPER_COMPARE_EQ 14
/// For ./OPER/compare-ne
#define REFDA_ADM_IETF_DTNMA_AGENT_ENUM_OBJID_OPER_COMPARE_NE 15
/// For ./OPER/compare-gt
#define REFDA_ADM_IETF_DTNMA_AGENT_ENUM_OBJID_OPER_COMPARE_GT 16
/// For ./OPER/compare-ge
#define REFDA_ADM_IETF_DTNMA_AGENT_ENUM_OBJID_OPER_COMPARE_GE 17
/// For ./OPER/compare-lt
#define REFDA_ADM_IETF_DTNMA_AGENT_ENUM_OBJID_OPER_COMPARE_LT 18
/// For ./OPER/compare-le
#define REFDA_ADM_IETF_DTNMA_AGENT_ENUM_OBJID_OPER_COMPARE_LE 19
/// For ./OPER/tbl-filter
#define REFDA_ADM_IETF_DTNMA_AGENT_ENUM_OBJID_OPER_TBL_FILTER 20

/** Initializer for the ADM ietf-dtnma-agent.
 * @param[in,out] agent The agent to register this namespace and its
 * objects within.
 * @return Zero upon success.
 */
int refda_adm_ietf_dtnma_agent_init(refda_agent_t *agent);

#ifdef __cplusplus
}
#endif

#endif /* REFDA_ADM_IETF_DTNMA_AGENT_H_ */