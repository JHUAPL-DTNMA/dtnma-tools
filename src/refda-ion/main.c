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
#include "refda/adm/ietf_amm.h"
#include "refda/adm/ietf_dtnma_agent.h"
#include <cace/amp/ion_bp.h>
#include <cace/util/logging.h>
#include <cace/util/defs.h>
#include <bp.h>
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
    fprintf(stderr, "Usage: %s {-h} {-l <log-level>} -a <listen-EID> {-m <hello-EID>}\n", argv0);
}

int main(int argc, char *argv[])
{
    // keep track of failure state
    int retval = 0;

    cace_openlog();
    refda_agent_init(&agent);

    /* Process Command Line Arguments. */
    int log_limit = LOG_WARNING;

    m_string_t own_eid;
    m_string_init(own_eid);
    m_string_t hello_eid;
    m_string_init(hello_eid);
    {
        {
            int opt;
            while ((opt = getopt(argc, argv, ":hl:a:m:")) != -1)
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
                    case 'a':
                        string_set_str(own_eid, optarg);
                        break;
                    case 'm':
                        string_set_str(hello_eid, optarg);
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
        fprintf(stderr, "An EID URI must be supplied");
        retval = 1;
    }

    // Attach to ION endpoint
    if (!retval)
    {
        if (bp_attach())
        {
            retval = 4;
        }
    }

    cace_amp_ion_bp_state_t app;
    cace_amp_ion_bp_state_init(&app);
    if (!retval)
    {
        if (cace_amp_ion_bp_state_bind(&app, own_eid))
        {
            CACE_LOG_ERR("Failed to bind to ION EID %s", m_string_get_cstr(own_eid));
            retval = 4;
        }
    }

    if (!retval)
    {
        m_string_set(agent.agent_eid, own_eid);
        CACE_LOG_DEBUG("Running as endpoint %s", string_get_cstr(agent.agent_eid));
        agent.mif.send = cace_amp_ion_bp_send;
        agent.mif.recv = cace_amp_ion_bp_recv;
        agent.mif.ctx  = &app;
    }
    m_string_clear(own_eid);

    if (!retval)
    {
        // ADM initialization
        refda_adm_ietf_amm_init(&agent);
        refda_adm_ietf_dtnma_agent_init(&agent);
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
            CACE_LOG_WARNING("ADM reference binding failed for %d type references", failures);
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

    if (!retval && !m_string_empty_p(hello_eid))
    {
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
    cace_amp_ion_bp_state_deinit(&app);
    bp_detach();

    CACE_LOG_DEBUG("Agent shutdown completed");
    cace_closelog();
    return retval;
}
