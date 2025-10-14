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

#include "agents.h"
#include <cace/ari/text_util.h>
#include <cace/util/defs.h>
#include <cace/util/logging.h>
#include <sys/stat.h>

void refdm_agent_init(refdm_agent_t *obj)
{
    CHKVOID(obj);
    m_string_init(obj->eid);
#if !(defined(HAVE_MYSQL) || defined(HAVE_POSTGRESQL))
    cace_ari_list_init(obj->rptsets);
#endif

    pthread_mutex_init(&(obj->log_mutex), NULL);
    obj->log_fd       = NULL;
    obj->log_fd_cnt   = 0;
    obj->log_file_num = 0;
}

void refdm_agent_deinit(refdm_agent_t *obj)
{
    CHKVOID(obj);

    if (obj->log_fd)
    {
        fclose(obj->log_fd);
        obj->log_fd = NULL;
    }

    pthread_mutex_destroy(&(obj->log_mutex));
#if !(defined(HAVE_MYSQL) || defined(HAVE_POSTGRESQL))
    cace_ari_list_clear(obj->rptsets);
#endif
    m_string_clear(obj->eid);
}

void refdm_agent_rotate_log(refdm_agent_t *agent, const refdm_agent_autologging_cfg_t *cfg, bool force)
{
    static char agent_autologging_sep = '_';
    char        filepath[1024];

    pthread_mutex_lock(&agent->log_mutex);

    if (cfg->enabled)
    {
        if (agent->log_fd != NULL)
        {
            // Roate log if cnt has been reset to < 0, or if it exceeds defined limit
            if (force || (agent->log_fd_cnt < 0) || ((cfg->limit > 0) && (agent->log_fd_cnt > cfg->limit)))
            {
                fclose(agent->log_fd);
                agent->log_fd = NULL;
            }
            else
            {
                return; // Keep using the open file
            }
        }

        // Ensure EID is encoded to filesystem-compatible character set
        m_string_t eid_path_seg;
        m_string_init(eid_path_seg);
        {
            cace_data_t eid_bytes;
            cace_data_init_view(&eid_bytes, m_string_size(agent->eid) + 1,
                                (cace_data_ptr_t)m_string_get_cstr(agent->eid));
            cace_uri_percent_encode(eid_path_seg, &eid_bytes, NULL);
        }

        // Create sub-directories if required (first file only)
        if (cfg->agent_dirs)
        {
            agent_autologging_sep = '/';

            if (agent->log_fd_cnt == 0)
            {
                int res = snprintf(filepath, sizeof(filepath), "%s/%s", cfg->dir, m_string_get_cstr(eid_path_seg));
                if (res >= (int)sizeof(filepath))
                {
                    CACE_LOG_ERR("Failed to write file path, got size %d", res);
                }
                else
                {
#if (defined(VXWORKS) || defined(mingw))
                    mkdir(filepath);
#else
                    // This will fail if directory already exists, which is acceptable
                    mkdir(filepath, 0777);
#endif
                }
            }
        }
        int res = snprintf(filepath, sizeof(filepath), "%s/%s%c%d.log", cfg->dir, m_string_get_cstr(eid_path_seg),
                           agent_autologging_sep, // Set to "/" to use separate directories per agent
                           agent->log_file_num);
        if (res >= (int)sizeof(filepath))
        {
            CACE_LOG_ERR("Failed to write file path, got size %d", res);
        }
        else
        {
            agent->log_fd = fopen(filepath, "a");
            if (agent->log_fd != NULL)
            {
                agent->log_fd_cnt = 0;
                agent->log_file_num++;
            }
            else
            {
                CACE_LOG_ERR("Failed to open report log file (%s) for agent %s", filepath,
                             m_string_get_cstr(agent->eid));
            }
        }

        m_string_clear(eid_path_seg);
    }
    else if (agent->log_fd != NULL)
    {
        fclose(agent->log_fd);
        agent->log_fd = NULL;
    }
    pthread_mutex_unlock(&agent->log_mutex);
}
