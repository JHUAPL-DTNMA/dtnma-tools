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
 ** \file nm_mgr_rx.c
 **
 ** File Name: nm_mgr_rx.c
 **
 **
 ** Subsystem:
 **          Network Manager Daemon: Receive Thread
 **
 ** Description: This file implements the management receive thread that
 ** 		     accepts information from DTNMP agents.
 **
 ** Notes:
 **
 ** Assumptions:
 **
 ** Modification History:
 **  MM/DD/YY  AUTHOR          DESCRIPTION
 **  --------  ------------    ---------------------------------------------
 **  08/31/11  V. Ramachandran Initial Implementation (JHU/APL)
 **  08/19/13  E. Birrane      Documentation clean up and code review comments. (JHU/APL)
 **  08/21/16  E. Birrane      Update to AMP v02 (Secure DTN - NASA: NNX14CS58P)
 **  10/07/18  E. Birrane      Update to AMP v0.5 (JHU/APL)
 *****************************************************************************/

#include "ingress.h"
#include "mgr.h"
#include "agents.h"
#if defined(HAVE_MYSQL) || defined(HAVE_POSTGRESQL)
#include "nm_sql.h"
#endif
#include <cace/ari/text.h>
#include <cace/util/daemon_run.h>
#include <cace/util/logging.h>

/** Handle a received RPTSET value.
 *
 * @param[in] mgr The manager to operate under.
 * @param[in] agent The agent object associated with this reception.
 * @param[in,out] val The value to move from.
 */
static void handle_recv(refdm_mgr_t *mgr, refdm_agent_t *agent, cace_ari_t *val)
{
#if defined(HAVE_MYSQL) || defined(HAVE_POSTGRESQL)
    /* Copy the message group to the database tables */
    refdm_db_insert_rptset(val, agent);
#else
    // local daemon storage
    cace_ari_set_copy(cace_ari_list_push_back_new(agent->rptsets), val);
#endif

    {
        bool wrote = false;
        pthread_mutex_lock(&agent->log_mutex);
        if (agent->log_fd && mgr->agent_log_cfg.rx_rpt)
        {
            string_t buf;
            string_init(buf);
            cace_ari_text_encode(buf, val, CACE_ARI_TEXT_ENC_OPTS_DEFAULT);
            CACE_LOG_INFO("Received value from %s with %s", string_get_cstr(agent->eid), string_get_cstr(buf));
            fprintf(agent->log_fd, "Received value from %s with %s", string_get_cstr(agent->eid), string_get_cstr(buf));
            string_clear(buf);

            agent->log_fd_cnt++;
            wrote = true;
        }
        if (agent->log_fd && wrote)
        {
            // Flush file after we've written set
            fflush(agent->log_fd);
        }
        pthread_mutex_unlock(&agent->log_mutex);
    }

    // And check for file rotation (we won't break up a set between files)
    refdm_agent_rotate_log(agent, &mgr->agent_log_cfg, false);
}

void *refdm_ingress_worker(void *arg)
{
    refdm_mgr_t *mgr = arg;
    CACE_LOG_INFO("Worker started");

    cace_ari_list_t values;
    cace_ari_list_init(values);
    cace_amm_msg_if_metadata_t meta;
    cace_amm_msg_if_metadata_init(&meta);

    /*
     * mgr->running controls the overall execution of threads in the
     * NM Agent.
     */
    while (cace_daemon_run_get(&mgr->running))
    {
        cace_ari_list_reset(values);
        int recv_res = mgr->mif.recv(values, &meta, &mgr->running, mgr->mif.ctx);
        // process received items even if failed status

        if (cace_log_is_enabled_for(LOG_INFO))
        {
            string_t buf;
            string_init(buf);
            cace_ari_text_encode(buf, &meta.src, CACE_ARI_TEXT_ENC_OPTS_DEFAULT);
            CACE_LOG_INFO("Message from %s has %zd ARIs", m_string_get_cstr(buf), cace_ari_list_size(values));
            string_clear(buf);
        }

        // Only handle text form endpoints
        const cace_data_t *tstr = cace_ari_cget_tstr(&meta.src);

        if (!cace_ari_list_empty_p(values) && tstr)
        {
            const char *eid = (const char *)tstr->ptr;
            CACE_LOG_DEBUG("Recording reports from agent at %s", eid);

            // might be unknown and NULL
            refdm_agent_t *agent = refdm_mgr_agent_get_eid(mgr, eid);
            if (!agent)
            {
                agent = refdm_mgr_agent_add(mgr, eid);
            }

            cace_ari_list_it_t val_it;
            /* For each received ARI, validate it */
            for (cace_ari_list_it(val_it, values); !cace_ari_list_end_p(val_it); cace_ari_list_next(val_it))
            {
                cace_ari_t *val = cace_ari_list_ref(val_it);
                if (!cace_ari_get_rptset(val))
                {
                    CACE_LOG_ERR("Ignoring input ARI that is not an RPTSET");
                    // item is left in list for later deinit
                    continue;
                }

                handle_recv(mgr, agent, val);
                atomic_fetch_add(&mgr->instr.num_rptset_recv, 1);
            }
        }

        if (recv_res)
        {
            CACE_LOG_INFO("Got mif.recv result=%d, stopping", recv_res);

            // flush the input queue but keep the daemon running
            break;
        }
    }

    cace_amm_msg_if_metadata_deinit(&meta);
    cace_ari_list_clear(values);

    CACE_LOG_INFO("Worker stopped");
    return NULL;
}
