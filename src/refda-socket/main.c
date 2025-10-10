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
#include "refda/adm/ietf.h"
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
    fprintf(stderr, "Usage: %s {-h} {-l <log-level>} {-s <startup-file>} -a <listen-EID> {-m <hello-EID>}\n", argv0);
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
    m_string_t own_eid;
    m_string_init(own_eid);
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
                        m_string_set_cstr(own_eid, optarg);
                        break;
                    case 'm':
                        m_string_set_cstr(hello_eid, optarg);
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
    if (!retval && m_string_empty_p(own_eid))
    {
        fprintf(stderr, "A socket endpoint URI must be supplied");
        retval = 1;
    }

    cace_amp_socket_state_t sock;
    cace_amp_socket_state_init(&sock);
    if (!retval)
    {
        const char *sock_path = cace_amp_socket_strip_scheme(m_string_get_cstr(own_eid));
        if (cace_amp_socket_state_bind(&sock, sock_path))
        {
            retval = 4;
        }
    }

    if (!retval)
    {
        m_string_set(agent.agent_eid, own_eid);
        CACE_LOG_DEBUG("Running as endpoint %s", string_get_cstr(agent.agent_eid));
        agent.mif.send = cace_amp_socket_send;
        agent.mif.recv = cace_amp_socket_recv;
        agent.mif.ctx  = &sock;
    }
    m_string_clear(own_eid);

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
            retval = 2;
        }
        else
        {
            CACE_LOG_INFO("ADM reference binding succeeded");
        }
    }
    if (!retval)
    {
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
    if (!retval)
    {
        if (refda_agent_start(&agent))
        {
            CACE_LOG_ERR("Agent startup failed");
            retval = 2;
        }
        else
        {
            CACE_LOG_INFO("Agent startup completed");
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
            // synthesize macro
            cace_ari_t     target = CACE_ARI_INIT_UNDEFINED;
            cace_ari_ac_t *tgt_ac = cace_ari_set_ac(&target, NULL);

            if (cace_ari_macrofile_read(startup_file, tgt_ac->items))
            {
                retval = 3;
            }
            fclose(startup_file);

            if (refda_agent_startup_exec(&agent, &target))
            {
                retval = 3;
            }
        }
#else  // defined(ARI_TEXT_PARSE)
        CACE_LOG_CRIT("This build of REFDA and CACE is not able to parse text ARIs");
        retval = 3;
#endif // defined(ARI_TEXT_PARSE)
    }
    m_string_clear(startup_exec);

    if (!retval && !m_string_empty_p(hello_eid))
    {
        cace_ari_t      target = CACE_ARI_INIT_UNDEFINED;
        // reference ari:/ietf/dtnma-agent/CTRL/report-on
        cace_ari_ref_t *tgt_ref =
            cace_ari_set_objref_path_intid(&target, REFDA_ADM_IETF_ENUM, REFDA_ADM_IETF_DTNMA_AGENT_ENUM_ADM,
                                           CACE_ARI_TYPE_CTRL, REFDA_ADM_IETF_DTNMA_AGENT_ENUM_OBJID_CTRL_REPORT_ON);
        assert(tgt_ref);
        cace_ari_ac_t *param_ac = cace_ari_params_set_ac(&tgt_ref->params, NULL);
        assert(param_ac);
        {
            cace_ari_t *item = cace_ari_list_push_back_new(param_ac->items);
            // reference ari:/ietf/dtnma-agent/CONST/hello
            cace_ari_set_objref_path_intid(item, REFDA_ADM_IETF_ENUM, REFDA_ADM_IETF_DTNMA_AGENT_ENUM_ADM,
                                           CACE_ARI_TYPE_CONST, REFDA_ADM_IETF_DTNMA_AGENT_ENUM_OBJID_CONST_HELLO);
        }
        {
            cace_ari_t *item = cace_ari_list_push_back_new(param_ac->items);
            cace_ari_ac_t *item_ac = cace_ari_set_ac(item, NULL);
            {
                cace_ari_t *mgr_ident = cace_ari_list_push_back_new(item_ac->items);
                cace_ari_set_tstr(mgr_ident, m_string_get_cstr(hello_eid), false);
            }
        }

        if (refda_agent_startup_exec(&agent, &target))
        {
            retval = 3;
        }
    }
    m_string_clear(hello_eid);

    refda_agent_enable_exec(&agent);

    if (!retval)
    {
        // Block until stopped
        cace_daemon_run_wait(&agent.running);
    }

#if defined(HAVE_LIBSYSTEMD)
    sd_notify(0, "STOPPING=1");
#endif
    CACE_LOG_INFO("Agent is shutting down");

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
