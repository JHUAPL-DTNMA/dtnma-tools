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
 ** \file nm_mgr.c
 **
 ** File Name: nm_mgr.c
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
 **  10/06/18  E. Birrane     Update to AMP v0.5 (JHU/APL)
 *****************************************************************************/

#include "mgr.h"

#include "ingress.h"
#ifdef USE_CIVETWEB
#include "nm_rest.h"
#endif
#include <cace/util/logging.h>
#include <cace/util/defs.h>

#if defined(HAVE_MYSQL) || defined(HAVE_POSTGRESQL)
#include "nm_sql.h"
#endif

void refdm_mgr_init(refdm_mgr_t *mgr)
{
    CHKVOID(mgr);

    memset(mgr, 0, sizeof(refdm_mgr_t));
    mgr->mgr_ui_mode = MGR_UI_DEFAULT;

    mgr->agent_log_cfg = (refdm_agent_autologging_cfg_t) {
        // Defaults (nominal, disabled on startup)
        .enabled    = 0,  // Disabled by default
        .rx_rpt     = 0,  // Log Parsed Report on receipt
        .limit      = 50, // Number of reports per file before rotation
        .agent_dirs = 0,  // Create discrete sub-folders per agent
        .dir        = "." // root log directory will be the working directory mgr started from as default
    };

    cace_daemon_run_init(&(mgr->running));
    cace_threadset_init(mgr->threads);
    refdm_agent_list_init(mgr->agent_list);
    refdm_agent_dict_init(mgr->agent_dict);
    pthread_mutex_init(&(mgr->agent_mutex), NULL);


#if defined(CIVETWEB_FOUND)
    mgr->rest_listen_port = 8089;
    mgr->rest             = NULL;
#endif
#if defined(HAVE_MYSQL) || defined(HAVE_POSTGRESQL)
    db_mgr_sql_init(mgr);
	db_mgt_init(mgr->sql_info, 0, 1);
#endif
}

void refdm_mgr_deinit(refdm_mgr_t *mgr)
{
    CHKVOID(mgr);

#if defined(HAVE_MYSQL) || defined(HAVE_POSTGRESQL)
    db_mgt_close();
#endif

    pthread_mutex_destroy(&(mgr->agent_mutex));
    refdm_agent_dict_clear(mgr->agent_dict);
    {
        refdm_agent_list_it_t it;
        for (refdm_agent_list_it(it, mgr->agent_list); !refdm_agent_list_end_p(it); refdm_agent_list_next(it))
        {
            refdm_agent_t *agent = *refdm_agent_list_ref(it);
            refdm_agent_deinit(agent);
            CACE_FREE(agent);
        }
    }
    refdm_agent_list_clear(mgr->agent_list);
    cace_threadset_clear(mgr->threads);
    cace_daemon_run_cleanup(&(mgr->running));
}

int refdm_mgr_start(refdm_mgr_t *mgr)
{
    cace_threadinfo_t threadinfo[3] = {
        { &refdm_ingress_worker, "refdm_ingress" },
        //        { &ui_thread, "nm_mgr_ui" },
        { NULL, NULL },
    };
#if defined(HAVE_MYSQL) || defined(HAVE_POSTGRESQL)
    threadinfo[2] = (cace_threadinfo_t) { &db_mgt_daemon, "nm_mgr_db" };
#endif
    if (cace_threadset_start(mgr->threads, threadinfo, sizeof(threadinfo) / sizeof(cace_threadinfo_t), mgr))
    {
        return 2;
    }

#ifdef USE_CIVETWEB
    refdm_nm_rest_start(&(mgr->rest), mgr);
#endif

    return 0;
}

int refdm_mgr_stop(refdm_mgr_t *mgr)
{
    /* Notify threads */
    cace_daemon_run_stop(&mgr->running);

#ifdef USE_CIVETWEB
    refdm_nm_rest_stop(mgr->rest);
    mgr->rest = NULL;
#endif

    cace_threadset_join(mgr->threads);

    return 0;
}

refdm_agent_t *refdm_mgr_agent_add(refdm_mgr_t *mgr, const char *agent_eid)
{
    CHKNULL(mgr);
    CHKNULL(agent_eid);

    if (pthread_mutex_lock(&(mgr->agent_mutex)))
    {
        CACE_LOG_ERR("failed to lock mutex");
        return NULL;
    }

    refdm_agent_t *agent = NULL;
    if (!refdm_agent_dict_get(mgr->agent_dict, agent_eid))
    {
        // agent does not already exist
        agent = CACE_MALLOC(sizeof(refdm_agent_t));
        refdm_agent_init(agent);
        string_set_str(agent->eid, agent_eid);

        // key is pointer to own member data
        CACE_LOG_INFO("adding agent for %s", string_get_cstr(agent->eid));
        refdm_agent_list_push_back(mgr->agent_list, agent);
        refdm_agent_dict_set_at(mgr->agent_dict, string_get_cstr(agent->eid), agent);
    }

    if (pthread_mutex_unlock(&(mgr->agent_mutex)))
    {
        CACE_LOG_ERR("failed to unlock mutex");
    }

    if (agent)
    {
        refdm_agent_rotate_log(agent, &mgr->agent_log_cfg, true);
    }

    return agent;
}

refdm_agent_t *refdm_mgr_agent_get_eid(refdm_mgr_t *mgr, const char *eid)
{
    CHKNULL(mgr);
    CHKNULL(eid);

    if (pthread_mutex_lock(&(mgr->agent_mutex)))
    {
        CACE_LOG_ERR("failed to lock mutex");
        return NULL;
    }

    refdm_agent_t **got = refdm_agent_dict_get(mgr->agent_dict, eid);

    if (pthread_mutex_unlock(&(mgr->agent_mutex)))
    {
        CACE_LOG_ERR("failed to unlock mutex");
    }

    return got ? *got : NULL;
}

refdm_agent_t *refdm_mgr_agent_get_index(refdm_mgr_t *mgr, size_t index)
{
    CHKNULL(mgr);

    if (pthread_mutex_lock(&(mgr->agent_mutex)))
    {
        CACE_LOG_ERR("failed to lock mutex");
        return NULL;
    }

    refdm_agent_t **got = refdm_agent_list_get(mgr->agent_list, index);

    if (pthread_mutex_unlock(&(mgr->agent_mutex)))
    {
        CACE_LOG_ERR("failed to unlock mutex");
    }

    return got ? *got : NULL;
}

void refdm_mgr_clear_reports(refdm_mgr_t *mgr _U_, refdm_agent_t *agent)
{
    cace_ari_list_reset(agent->rptsets);
}
