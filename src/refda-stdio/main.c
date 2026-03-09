/*
 * Copyright (c) 2011-2026 The Johns Hopkins University Applied Physics
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
#include "refda/loader.h"
#include "cace/util/logging.h"
#include "cace/util/defs.h"
#include "cace/ari/text_util.h"
#include "cace/ari/text.h"
#include <cace/ari/macrofile.h>
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

        m_string_t outhex;
        m_string_init(outhex);
        if (cace_base16_encode(outhex, &outbin, true))
        {
            CACE_LOG_ERR("Failed to base-16 encode ARI");
            retval = 4;
        }
        CACE_LOG_DEBUG("encoded ARI item to base-16: %s", m_string_get_cstr(outhex));

        if (!retval)
        {
            if (fputs(m_string_get_cstr(outhex), stdout) <= 0)
            {
                retval = 2;
            }

            if (fputs(" ", stdout) <= 0)
            {
                retval = 2;
            }
        }

        m_string_clear(outhex);
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
            char  *lineptr   = NULL;
            size_t linealloc = 0;

            res = getline(&lineptr, &linealloc, stdin);
            if (res < 0)
            {
                CACE_LOG_DEBUG("returning due to end of input %d", res);
                free(lineptr);
                return CACE_AMM_MSG_IF_RECV_END;
            }
            else
            {
                CACE_LOG_DEBUG("read line with %zu characters", res);
                char *curs = lineptr;
                char *end  = lineptr + res;

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

                    m_string_t inhex;
                    m_string_init(inhex);
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
                            m_string_t buf;
                            m_string_init(buf);
                            cace_ari_text_encode(buf, &item, CACE_ARI_TEXT_ENC_OPTS_DEFAULT);
                            CACE_LOG_DEBUG("decoded ARI item: %s", m_string_get_cstr(buf));
                            m_string_clear(buf);
                        }

                        cace_ari_list_push_back_move(data, &item);
                    }
                    else
                    {
                        cace_ari_deinit(&item);
                    }

                    cace_data_deinit(&inbin);
                    m_string_clear(inhex);

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

    m_string_t startup_exec;
    m_string_init(startup_exec);
    {
        {
            int opt;
            while ((opt = getopt(argc, argv, ":hl:s:a:")) != -1)
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
                        m_string_set_cstr(agent.agent_eid, optarg);
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
        CACE_LOG_DEBUG("Running as endpoint %s", m_string_get_cstr(agent.agent_eid));
        agent.mif.send = stdout_send;
        agent.mif.recv = stdin_recv;
    }

    if (!retval)
    {
        if (refda_loader_basemods(&agent))
        {
            CACE_LOG_ERR("Failed to load base ADMs");
            retval = 5;
        }
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

    if (!retval)
    {
        // Block until stopped
        cace_daemon_run_wait(&agent.running);
    }

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

    CACE_LOG_DEBUG("Agent shutdown completed");
    cace_closelog();
    return retval;
}
