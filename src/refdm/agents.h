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
/*****************************************************************************
 **
 ** File Name: agents.h
 **
 ** Subsystem:
 **          Network Manager Application
 **
 ** Description: All Agent-related processing for a manager.
 **
 ** Notes:
 **
 ** Assumptions:
 **
 ** Modification History:
 **  MM/DD/YY  AUTHOR          DESCRIPTION
 **  --------  ------------    ---------------------------------------------
 **  10/06/18  E. Birrane      Initial Implementation (JHU/APL)
 *****************************************************************************/

#ifndef REFDM_AGENTS_H_
#define REFDM_AGENTS_H_

#include <cace/cace_data.h>
#include <cace/ari.h>
#include <m-string.h>
#include <pthread.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Data structure representing a managed remote agent.
 **/
typedef struct
{
    /// Endpoint ID (opaque URI) for this agent
    m_string_t eid;

#if ! (defined(HAVE_MYSQL) || defined(HAVE_POSTGRESQL))
    /// Received RPTSET values
    cace_ari_list_t rptsets;
#endif

    /// Mutex for #log_fd and related data
    pthread_mutex_t log_mutex;
    FILE           *log_fd;
    int             log_fd_cnt;
    int             log_file_num;
} refdm_agent_t;

void refdm_agent_init(refdm_agent_t *obj);

void refdm_agent_deinit(refdm_agent_t *obj);

#define M_OPL_refdm_agent_t() (INIT(API_2(refdm_agent_init)), CLEAR(API_2(refdm_agent_deinit)))

/**
 * Global Configuration Settings for Automatic Logging
 */
typedef struct
{
    bool enabled;
    bool rx_rpt;     // Log all reports to file upon receipt
    int  limit;      // Number of entries (reports+tables) per file
    int  agent_dirs; // If true, create discrete directories for each agent
    char dir[32];    // directory to save report logs to (or place sub-directories in)

} refdm_agent_autologging_cfg_t;

/** Common log file management.
 */
void refdm_agent_rotate_log(refdm_agent_t *agent, const refdm_agent_autologging_cfg_t *cfg, bool force);

#ifdef __cplusplus
}
#endif

#endif /* REFDM_AGENTS_H_ */
