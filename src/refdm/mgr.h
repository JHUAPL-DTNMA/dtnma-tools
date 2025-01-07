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
/*****************************************************************************
 ** \file nm_mgr.h
 **
 ** File Name: nm_mgr.h
 **
 ** Subsystem:
 **          Network Manager Application
 **
 ** Description: This file implements the DTNMP Manager user interface
 **
 ** Notes:
 **
 ** Assumptions:
 **
 ** Modification History:
 **  MM/DD/YY  AUTHOR          DESCRIPTION
 **  --------  ------------    ---------------------------------------------
 **  09/01/11  V. Ramachandran Initial Implementation (JHU/APL)
 **  08/19/13  E. Birrane      Documentation clean up and code review comments. (JHU/APL)
 **  08/21/16  E. Birrane      Update to AMP v02 (Secure DTN - NASA: NNX14CS58P)
 **  10/06/18   E. Birrane     Update to AMP v0.5 (JHU/APL)
 *****************************************************************************/

#ifndef REFDM_MGR_H_
#define REFDM_MGR_H_

#include "agents.h"
#include "instr.h"
#include "refdm/config.h"
#include <cace/amm/msg_if.h>
#include <cace/util/daemon_run.h>
#include <cace/util/threadset.h>
#include <m-dict.h>
#include <m-string.h>

#ifdef __cplusplus
extern "C" {
#endif

// Forward declarations
struct mg_context;
struct refdm_db_s;

typedef enum mgr_ui_mode_enum
{
    MGR_UI_STANDARD,  // Standard Shell-Based UI
    MGR_UI_NCURSES,   // NCURSES-Based UI (currently a compile-time flag mutually exclusive with MGR_UI_STANDARD)
    MGR_UI_AUTOMATOR, // Special Altenrative UI Optimized for Automation
} mgr_ui_mode_enum;

#ifdef USE_NCURSES
#define MGR_UI_DEFAULT MGR_UI_NCURSES
#else
#define MGR_UI_DEFAULT MGR_UI_STANDARD
#endif

M_DICT_DEF2(refdm_agent_dict, const char *, M_CSTR_OPLIST, refdm_agent_t *, M_PTR_OPLIST)

typedef struct refdm_mgr_s
{
    /// Manager endpoint ID as URI text
    m_string_t own_eid;

    // FIXME: this is not thread safe storage
    mgr_ui_mode_enum mgr_ui_mode;
    /// Message logging options
    refdm_agent_autologging_cfg_t agent_log_cfg;

    /// Running state
    daemon_run_t running;
    /// Messaging configuration
    cace_amm_msg_if_t mif;
    /// Instrumentation counters
    refdm_instr_t instr;
    /// Threads associated with the mgr
    threadset_t threads;

    /// Agent state storage
    refdm_agent_dict_t agents;
    /// Access control for #agents
    pthread_mutex_t agents_mutex;

#if defined(CIVETWEB_FOUND)
    /// HTTP server state, managed by a background thread
    struct mg_context *rest;
#endif
#if defined(HAVE_MYSQL) || defined(HAVE_POSTGRESQL)
    /// SQL client state, managed by a background thread
    struct refdm_db_s *sql_info;
#endif

} refdm_mgr_t;

void refdm_mgr_init(refdm_mgr_t *mgr);

void refdm_mgr_deinit(refdm_mgr_t *mgr);

int refdm_mgr_start(refdm_mgr_t *mgr);

int refdm_mgr_stop(refdm_mgr_t *mgr);

refdm_agent_t *refdm_mgr_agent_add(refdm_mgr_t *mgr, const char *agent_eid);

refdm_agent_t *refdm_mgr_agent_get(refdm_mgr_t *mgr, const char *agent_eid);

#ifdef __cplusplus
}
#endif

#endif /* REFDM_MGR_H_ */
