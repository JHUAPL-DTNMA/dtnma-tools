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
#include "refda/agent.h"
#include "refda/ingress.h"
#include "refda/adm/ietf_amm.h"
#include "refda/adm/ietf_amm_base.h"
#include "refda/adm/ietf_amm_semtype.h"
#include "refda/adm/ietf_network_base.h"
#include "refda/adm/ietf_dtnma_agent.h"
#include "refda/adm/ietf_dtnma_agent_acl.h"
#include <cace/amp/socket.h>
#include <cace/ari/text.h>
#include <cace/ari/macrofile.h>
#include <cace/util/logging.h>
#include <cace/util/defs.h>
#include <signal.h>
#include <unistd.h>

#if defined(HAVE_LIBSYSTEMD)
#include <systemd/sd-daemon.h>
#endif

/// Per-process state
static refda_agent_t agent;

static void daemon_signal_handler(int signum)
{
    CACE_LOG_DEBUG("Received signal %d", signum);
    CACE_LOG_INFO("Signaling shutdown");
    cace_daemon_run_stop(&agent.running);
}

static void show_usage(const char *argv0)
{
    fprintf(stderr, "Usage: %s {-h} {-l <log-level>} {-s <startup-file>} -a <listen-path> {-m <hello-path>}\n", argv0);
}

int main(int argc, char *argv[])
{
    // keep track of failure state
    int retval = 0;

    cace_openlog();
    refda_agent_init(&agent);

    /* Process Command Line Arguments. */
    int log_limit = LOG_WARNING;

    m_string_t startup_exec;
    m_string_init(startup_exec);
    m_string_t sock_path;
    m_string_init(sock_path);
    m_string_t hello_eid;
    m_string_init(hello_eid);
    {
        {
            int opt;
            while ((opt = getopt(argc, argv, ":hl:s:a:m:")) != -1)
            {
                switch (opt)
                {
                    case 'l':
                        if (cace_log_get_severity(&log_limit, optarg))
                        {
                            show_usage(argv[0]);
                            retval = 1;
                        }
                        break;
                    case 's':
                        m_string_set_cstr(startup_exec, optarg);
                        break;
                    case 'a':
                        m_string_set_cstr(sock_path, optarg);
                        break;
                    case 'm':
                        m_string_printf(hello_eid, "file:%s", optarg);
                        break;
                    case 'h':
                    default:
                        show_usage(argv[0]);
                        retval = 1;
                        break;
                }
            }
        }
    }
    cace_log_set_least_severity(log_limit);
    CACE_LOG_DEBUG("Agent starting up with log limit %d", log_limit);

    // check arguments
    if (!retval && m_string_empty_p(sock_path))
    {
        fprintf(stderr, "A socket path must be supplied");
        retval = 1;
    }

    cace_amp_socket_state_t sock;
    cace_amp_socket_state_init(&sock);
    if (!retval)
    {
        if (cace_amp_socket_state_bind(&sock, sock_path))
        {
            retval = 4;
        }
    }

    if (!retval)
    {
        m_string_printf(agent.agent_eid, "file:%s", string_get_cstr(sock_path));
        CACE_LOG_DEBUG("Running as endpoint %s", string_get_cstr(agent.agent_eid));
        agent.mif.send = cace_amp_socket_send;
        agent.mif.recv = cace_amp_socket_recv;
        agent.mif.ctx  = &sock;
    }
    m_string_clear(sock_path);

    if (!retval)
    {
        // ADM initialization
        refda_adm_ietf_amm_init(&agent);
        refda_adm_ietf_amm_base_init(&agent);
        refda_adm_ietf_amm_semtype_init(&agent);
        refda_adm_ietf_network_base_init(&agent);
        refda_adm_ietf_dtnma_agent_init(&agent);
        refda_adm_ietf_dtnma_agent_acl_init(&agent);
#if 0
  dtn_bp_agent_init();
  dtn_ion_ionadmin_init();
  dtn_ion_ipnadmin_init();
  dtn_ion_ionsecadmin_init();
  dtn_ion_ltpadmin_init();
  dtn_ltp_agent_init();
  dtn_ion_bpadmin_init();
#ifdef BUILD_BPv6
  dtn_sbsp_init();
#else
//  dtn_bpsec_init();
#endif
#endif
    }

    if (!retval)
    {
        /* Register signal handlers. */
        struct sigaction act;
        memset(&act, 0, sizeof(struct sigaction));
        act.sa_handler = daemon_signal_handler;
        sigaction(SIGINT, &act, NULL);
        sigaction(SIGTERM, &act, NULL);
    }

    /* Start agent threads. */
    if (!retval)
    {
        int failures = refda_agent_bindrefs(&agent);
        if (failures)
        {
            // Warn but continue on
            CACE_LOG_ERR("ADM reference binding failed for %d type references", failures);
        }
        else
        {
            CACE_LOG_INFO("ADM reference binding succeeded");
        }

        if (refda_agent_start(&agent))
        {
            CACE_LOG_ERR("Agent startup failed");
            retval = 2;
        }
        else
        {
            CACE_LOG_INFO("Agent startup completed");
        }

        if (refda_agent_init_objs(&agent))
        {
            CACE_LOG_ERR("Agent object initialization failed");
            retval = 2;
        }
        else
        {
            CACE_LOG_INFO("Agent object initialization completed");
        }
    }

#if defined(HAVE_LIBSYSTEMD)
    sd_notify(0, "READY=1");
#endif
    CACE_LOG_INFO("READY");

    if (!retval && !m_string_empty_p(startup_exec))
    {
#if defined(ARI_TEXT_PARSE)
        CACE_LOG_INFO("Executing startup targets from %s", m_string_get_cstr(startup_exec));
        FILE *startup_file = fopen(m_string_get_cstr(startup_exec), "r");
        if (!startup_file)
        {
            retval = 3;
        }
        else
        {
            // synthesize execset with one target
            cace_ari_t run = CACE_ARI_INIT_UNDEFINED;

            cace_ari_execset_t *execset = cace_ari_set_execset(&run);
            cace_ari_set_uint(&execset->nonce, 1);
            cace_ari_ac_t *tgt_ac = cace_ari_set_ac(cace_ari_list_push_back_new(execset->targets), NULL);

            if (cace_ari_macrofile_read(startup_file, tgt_ac->items))
            {
                retval = 3;
            }
            fclose(startup_file);

            CACE_LOG_DEBUG("Waiting on startup execution");
            refda_msgdata_t item;
            refda_msgdata_init(&item);
            cace_ari_set_move(&item.value, &run);

            refda_msgdata_queue_push_move(agent.execs, &item);
            sem_post(&agent.execs_sem);

            while (true)
            {
                sem_wait(&(agent.self_rptgs_sem));
                if (!refda_msgdata_queue_pop(&item, agent.self_rptgs))
                {
                    // shouldn't happen
                    CACE_LOG_CRIT("failed to pop from self_rptgs queue");
                    retval = 3;
                }
                else
                {
                    cace_ari_rptset_t *rptset = cace_ari_get_rptset(&item.value);
                    if (rptset)
                    {
                        const cace_ari_report_t *rpt = cace_ari_report_list_front(rptset->reports);
                        if (cace_ari_is_undefined(cace_ari_list_front(rpt->items)))
                        {
                            CACE_LOG_ERR("startup execution failed");
                            retval = 3;
                        }
                    }
                    else
                    {
                        cace_ari_uint nonce;
                        if (cace_ari_get_uint(&item.value, &nonce) || (nonce != 1))
                        {
                            CACE_LOG_ERR("type or nonce mismatch, expected 1 got %u", nonce);
                            retval = 3;
                        }
                        // finished with whole sequence
                        break;
                    }
                    refda_msgdata_deinit(&item);
                }
            }
            CACE_LOG_INFO("Finished startup execution");
        }
#else  // defined(ARI_TEXT_PARSE)
        CACE_LOG_CRIT("This build of REFDA and CACE is not able to parse text ARIs");
        retval = 3;
#endif // defined(ARI_TEXT_PARSE)
    }
    m_string_clear(startup_exec);

    if (!retval && !m_string_empty_p(hello_eid))
    {
        CACE_LOG_DEBUG("Sending hello report to %s", m_string_get_cstr(hello_eid));
        if (refda_agent_send_hello(&agent, m_string_get_cstr(hello_eid)))
        {
            CACE_LOG_ERR("Agent hello failed");
            retval = 3;
        }
        else
        {
            CACE_LOG_INFO("Sent hello report");
        }
    }
    m_string_clear(hello_eid);

    if (!retval)
    {
        // Block until stopped
        cace_daemon_run_wait(&agent.running);
        CACE_LOG_INFO("Agent is shutting down");
    }

#if defined(HAVE_LIBSYSTEMD)
    sd_notify(0, "STOPPING=1");
#endif

    /* Join threads and wait for them to complete. */
    if (!retval)
    {
        if (refda_agent_stop(&agent))
        {
            CACE_LOG_ERR("Agent stop failed");
            retval = 4;
        }
        else
        {
            CACE_LOG_INFO("Agent stopped");
        }
    }

    /* Cleanup. */
    CACE_LOG_DEBUG("Cleaning Agent Resources");
    refda_agent_deinit(&agent);
    cace_amp_socket_state_deinit(&sock);

    CACE_LOG_DEBUG("Agent shutdown completed");
    cace_closelog();
    return retval;
}
