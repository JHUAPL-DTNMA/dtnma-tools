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
#include "refdm/mgr.h"
#include <cace/amp/ion_bp.h>
#include <cace/util/logging.h>
#include <cace/util/defs.h>
#include <signal.h>
#include <unistd.h>

#if defined(HAVE_LIBSYSTEMD)
#include <systemd/sd-daemon.h>
#endif

/// Per-process state
static refdm_mgr_t mgr;

static void daemon_signal_handler(int signum)
{
    CACE_LOG_DEBUG("Received signal %d", signum);
    CACE_LOG_INFO("Signaling shutdown");
    cace_daemon_run_stop(&mgr.running);
}

static void show_usage(const char *argv0)
{
    fprintf(stderr, "Usage: %s {-h} -a <listen-path>\n", argv0);
}

int main(int argc, char *argv[])
{
    // keep track of failure state
    int retval = 0;

    cace_openlog();
    refdm_mgr_init(&mgr);

    /* Process Command Line Arguments. */
    int        log_limit = LOG_WARNING;
    m_string_t eid;
    m_string_init(eid);
    {
        {
            int opt;
            while ((opt = getopt(argc, argv, ":hl:a:")) != -1)
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
                        string_set_str(eid, optarg);
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
    CACE_LOG_DEBUG("Manager starting up with log limit %d", log_limit);

    // check arguments
    if (!retval && m_string_empty_p(eid))
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
        if (cace_amp_ion_bp_state_bind(&app, eid))
        {
            CACE_LOG_ERR("Failed to bind to ION EID %s", m_string_get_cstr(eid));
            retval = 4;
        }
    }

    if (!retval)
    {
        m_string_set(mgr.own_eid, eid);
        CACE_LOG_DEBUG("Running as endpoint %s", string_get_cstr(mgr.own_eid));
        mgr.mif.send = cace_amp_ion_bp_send;
        mgr.mif.recv = cace_amp_ion_bp_recv;
        mgr.mif.ctx  = &app;
    }
    m_string_clear(eid);

    if (!retval)
    {
        /* Register signal handlers. */
        struct sigaction act;
        memset(&act, 0, sizeof(struct sigaction));
        act.sa_handler = daemon_signal_handler;
        sigaction(SIGINT, &act, NULL);
        sigaction(SIGTERM, &act, NULL);
    }

    /* Start manager threads. */
    if (!retval)
    {
        if (refdm_mgr_start(&mgr))
        {
            CACE_LOG_ERR("Manager startup failed");
            retval = 2;
        }
        else
        {
            CACE_LOG_INFO("Manager startup completed");
        }
    }

#if defined(HAVE_LIBSYSTEMD)
    sd_notify(0, "READY=1");
#endif
    CACE_LOG_INFO("READY");

    if (!retval)
    {
        // Block until stopped
        cace_daemon_run_wait(&mgr.running);
        CACE_LOG_INFO("Manager is shutting down");
    }

#if defined(HAVE_LIBSYSTEMD)
    sd_notify(0, "STOPPING=1");
#endif

    /* Join threads and wait for them to complete. */
    if (!retval)
    {
        if (refdm_mgr_stop(&mgr))
        {
            CACE_LOG_ERR("Manager stop failed");
            retval = 4;
        }
        else
        {
            CACE_LOG_INFO("Manager stopped");
        }
    }

    /* Cleanup. */
    CACE_LOG_DEBUG("Cleaning manager resources");
    refdm_mgr_deinit(&mgr);
    cace_amp_ion_bp_state_deinit(&app);
    bp_detach();

    CACE_LOG_DEBUG("Manager shutdown completed");
    cace_closelog();
    return retval;
}
