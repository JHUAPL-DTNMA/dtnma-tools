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
#include "refda/adm/ietf_amm.h"
#include "refda/adm/ietf_amm_base.h"
#include "refda/adm/ietf_amm_semtype.h"
#include "refda/adm/ietf_dtnma_agent.h"
#include "refda/adm/ietf_dtnma_agent_acl.h"
#include "cace/util/logging.h"
#include "cace/util/defs.h"
#include "cace/ari/text_util.h"
#include "cace/ari/text.h"
#include "cace/ari/time_util.h"
#include "cace/ari/cbor.h"
#include <sys/poll.h>
#include <signal.h>
#include <unistd.h>

/// Per-process state
static refda_agent_t agent;

static void daemon_signal_handler(int signum)
{
    CACE_LOG_DEBUG("Received signal %d", signum);
    CACE_LOG_INFO("Signaling shutdown");
    cace_daemon_run_stop(&agent.running);
}

static int stdout_send(const cace_ari_list_t data, const cace_amm_msg_if_metadata_t *meta _U_, void *ctx _U_)
{
    int retval = 0;
    CACE_LOG_DEBUG("Sending message with %d ARIs", cace_ari_list_size(data));

    cace_ari_list_it_t ait;
    for (cace_ari_list_it(ait, data); !cace_ari_list_end_p(ait); cace_ari_list_next(ait))
    {
        cace_data_t outbin;
        cace_data_init(&outbin);
        if (cace_ari_cbor_encode(&outbin, cace_ari_list_cref(ait)))
        {
            CACE_LOG_ERR("Failed to binary encode ARI");
            retval = 3;
        }

        string_t outhex;
        string_init(outhex);
        if (cace_base16_encode(outhex, &outbin, true))
        {
            CACE_LOG_ERR("Failed to base-16 encode ARI");
            retval = 4;
        }
        CACE_LOG_DEBUG("encoded ARI item to base-16: %s", string_get_cstr(outhex));

        if (!retval)
        {
            if (fputs(string_get_cstr(outhex), stdout) <= 0)
            {
                retval = 2;
            }

            if (fputs(" ", stdout) <= 0)
            {
                retval = 2;
            }
        }

        string_clear(outhex);
        cace_data_deinit(&outbin);
    }

    if (!cace_ari_list_empty_p(data))
    {
        if (fputs("\n", stdout) <= 0)
        {
            retval = 2;
        }
        fflush(stdout);
        CACE_LOG_DEBUG("flushed stdout");
    }

    return retval;
}

static int stdin_recv(cace_ari_list_t data, cace_amm_msg_if_metadata_t *meta, cace_daemon_run_t *running, void *ctx _U_)
{
    CHKERR1(data);
    CHKERR1(meta);
    CHKERR1(running);

    static const char *arisep = " \f\n\r\t\v"; // Identical to isspace()

    static const char *src = "stdin";
    cace_ari_set_tstr(&meta->src, src, false);
    cace_get_system_time(&meta->timestamp);

    // Watch stdin (fd 0) for input, assuming whole-lines are given
    struct pollfd pfds[] = {
        { .fd = fileno(stdin), .events = POLLIN | POLLERR | POLLHUP },
    };
    struct pollfd *poll_stdin = pfds + 0;

    while (true)
    {
        // Wait up to 1 second
        int res = poll(pfds, sizeof(pfds) / sizeof(struct pollfd), 1000);
        if (res < 0)
        {
            return CACE_AMM_MSG_IF_RECV_END;
        }
        else if (res == 0)
        {
            // nothing ready, but maybe daemon is shutting down
            if (!cace_daemon_run_get(running))
            {
                CACE_LOG_DEBUG("returning due to running state change");
                return CACE_AMM_MSG_IF_RECV_END;
            }
            continue;
        }

        if (poll_stdin->revents & POLLIN)
        {
            // assume that if something is ready to read that a whole line will come
            char  *lineptr = NULL;
            size_t got     = 0;
            res            = getline(&lineptr, &got, stdin);
            if (res < 0)
            {
                CACE_LOG_DEBUG("returning due to end of input %d", res);
                free(lineptr);
                return CACE_AMM_MSG_IF_RECV_END;
            }
            else
            {
                CACE_LOG_DEBUG("read line with %zu characters", got);
                char *curs = lineptr;
                char *end  = lineptr + got;

                int lineret = 0;
                while (curs < end)
                {
                    size_t plen = strcspn(curs, arisep);
                    if (plen == 0)
                    {
                        // no more
                        break;
                    }
                    // skip over optional prefix
                    if (strncasecmp(curs, "0x", 2) == 0)
                    {
                        curs += 2;
                        plen -= 2;
                    }

                    string_t inhex;
                    string_init(inhex);
                    m_string_set_cstrn(inhex, curs, plen);
                    CACE_LOG_DEBUG("decoding ARI item from base-16: %s", m_string_get_cstr(inhex));

                    cace_data_t inbin;
                    cace_data_init(&inbin);
                    if (cace_base16_decode(&inbin, inhex))
                    {
                        CACE_LOG_ERR("Failed to base-16 decode input %s", curs);
                        lineret = 1;
                    }

                    cace_ari_t item;
                    cace_ari_init(&item);
                    if (!lineret)
                    {
                        size_t used;
                        char  *errm;
                        res = cace_ari_cbor_decode(&item, &inbin, &used, &errm);
                        if (res)
                        {
                            CACE_LOG_ERR("Failed to binary decode ARI: %s", errm);
                            CACE_FREE(errm);
                            // continue to try next part
                            lineret = 2;
                        }
                    }

                    if (!lineret)
                    {
                        if (cace_log_is_enabled_for(LOG_DEBUG))
                        {
                            string_t buf;
                            string_init(buf);
                            cace_ari_text_encode(buf, &item, CACE_ARI_TEXT_ENC_OPTS_DEFAULT);
                            CACE_LOG_DEBUG("decoded ARI item: %s", string_get_cstr(buf));
                            string_clear(buf);
                        }

                        cace_ari_list_push_back_move(data, &item);
                    }
                    else
                    {
                        cace_ari_deinit(&item);
                    }

                    cace_data_deinit(&inbin);
                    string_clear(inhex);

                    curs += plen;
                }

                free(lineptr);

                CACE_LOG_DEBUG("decoded %d ARI items in the line", cace_ari_list_size(data));
                if (!cace_ari_list_empty_p(data))
                {
                    // stop when something received
                    break;
                }
            }
        }
        if (poll_stdin->revents & (POLLERR | POLLHUP))
        {
            // input has closed
            CACE_LOG_DEBUG("returning due to hangup");
            return CACE_AMM_MSG_IF_RECV_END;
        }
    }

    return 0;
}

static void show_usage(const char *argv0)
{
    fprintf(stderr, "Usage: %s {-h} {-l <log-level>} -a <agent EID>\n", argv0);
}

int main(int argc, char *argv[])
{
    // keep track of failure state
    int retval = 0;

    cace_openlog();
    refda_agent_init(&agent);

    /* Process Command Line Arguments. */
    int log_limit = LOG_WARNING;
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
                        string_set_str(agent.agent_eid, optarg);
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

    if (!retval)
    {
        CACE_LOG_DEBUG("Running as endpoint %s", string_get_cstr(agent.agent_eid));
        agent.mif.send = stdout_send;
        agent.mif.recv = stdin_recv;
    }

    // ADM initialization
    refda_adm_ietf_amm_init(&agent);
    refda_adm_ietf_amm_base_init(&agent);
    refda_adm_ietf_amm_semtype_init(&agent);
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
    CACE_LOG_INFO("READY");

    if (!retval)
    {
        // stdio uses non-specific EIDs
        if (refda_agent_send_hello(&agent, "any"))
        {
            CACE_LOG_ERR("Agent hello failed");
            retval = 3;
        }
        else
        {
            CACE_LOG_INFO("Sent hello report");
        }
    }

    if (!retval)
    {
        // Block until stopped
        cace_daemon_run_wait(&agent.running);
        CACE_LOG_INFO("Agent is shutting down");
    }

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

    CACE_LOG_DEBUG("Agent shutdown completed");
    cace_closelog();
    return retval;
}
