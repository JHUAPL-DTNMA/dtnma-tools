/*
 * Copyright (c) 2023 The Johns Hopkins University Applied Physics
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
#include "cace/util/logging.h"
#include "cace/util/defs.h"
#include "cace/ari/text_util.h"
#include "cace/ari/text.h"
#include "cace/ari/cbor.h"
#include <sys/poll.h>
#include <signal.h>
#include <unistd.h>

#define MAX_HEXMSG_SIZE 10240

static refda_agent_t agent;

static void daemon_signal_handler(int signum)
{
    CACE_LOG_DEBUG("Received signal %d", signum);
    CACE_LOG_INFO("Signaling shutdown");
    daemon_run_stop(&agent.running);
}

static int stdout_send(const ari_list_t data, const cace_amm_msg_if_metadata_t *meta _U_, void *ctx _U_)
{
    int retval = 0;
    CACE_LOG_DEBUG("Sendig message with %d ARIs", ari_list_size(data));

    ari_list_it_t ait;
    for (ari_list_it(ait, data); !ari_list_end_p(ait); ari_list_next(ait))
    {
        cace_data_t outbin;
        cace_data_init(&outbin);
        if (ari_cbor_encode(&outbin, ari_list_cref(ait)))
        {
            CACE_LOG_ERR("Failed to binary encode ARI");
            retval = 3;
        }

        string_t outhex;
        string_init(outhex);
        if (base16_encode(outhex, &outbin, true))
        {
            CACE_LOG_ERR("Failed to base-16 encode ARI");
            retval = 4;
        }

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

    if (!ari_list_empty_p(data))
    {
        if (fputs("\n", stdout) <= 0)
        {
            retval = 2;
        }
        fflush(stdout);
    }

    return retval;
}

static int stdin_recv(ari_list_t data, cace_amm_msg_if_metadata_t *meta, daemon_run_t *running, void *ctx _U_)
{
    CHKERR1(data);
    CHKERR1(meta);
    CHKERR1(running);

    static const char *arisep = " \f\n\r\t\v"; // Identical to isspace()

    // Watch stdin (fd 0) for input, assuming whole-lines are given
    struct pollfd pfds[] = {
        { .fd = fileno(stdin), .events = POLLIN },
    };

    while (true)
    {
        // Wait up to 1 second
        int res = poll(pfds, sizeof(pfds) / sizeof(struct pollfd), 1000);
        if (res < 0)
        {
            return 2;
            break;
        }
        else if (res == 0)
        {
            // nothing ready, but maybe daemon is shutting down
            if (!daemon_run_get(running))
            {
                return 0;
            }
            continue;
        }

        if (pfds[0].revents & POLLIN)
        {
            // assume that if something is ready to read that a whole line will come
            char  *lineptr = NULL;
            size_t got;
            res = getline(&lineptr, &got, stdin);
            if (res < 0)
            {
                CACE_LOG_DEBUG("returning due to end of input %d", res);
                free(lineptr);
                return 2;
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

                    curs[plen] = '\0'; // clobber separator
                    CACE_LOG_DEBUG("decoding ARI item from base-16: %s", curs);

                    string_t inhex;
                    string_init_set_str(inhex, curs);
                    cace_data_t inbin;
                    cace_data_init(&inbin);
                    if (base16_decode(&inbin, inhex))
                    {
                        CACE_LOG_ERR("Failed to base-16 decode ARI %s", curs);
                        lineret = 1;
                    }

                    ari_t item;
                    ari_init(&item);
                    if (!lineret)
                    {
                        size_t      used;
                        const char *errm;
                        res = ari_cbor_decode(&item, &inbin, &used, &errm);
                        if (res)
                        {
                            CACE_LOG_ERR("Failed to binary decode ARI: %s", errm);
                            // continue to try next part
                            lineret = 2;
                        }
                        if (errm)
                        {
                            ARI_FREE((char *)errm);
                        }
                    }

                    if (!lineret)
                    {
                        if (cace_log_is_enabled_for(LOG_DEBUG))
                        {
                            string_t buf;
                            string_init(buf);
                            ari_text_encode(buf, &item, ARI_TEXT_ENC_OPTS_DEFAULT);
                            CACE_LOG_DEBUG("decoded ARI item: %s", string_get_cstr(buf));
                            string_clear(buf);
                        }

                        ari_list_push_back_move(data, &item);
                    }
                    else
                    {
                        ari_deinit(&item);
                    }

                    cace_data_deinit(&inbin);
                    string_clear(inhex);

                    curs += plen;
                }

                free(lineptr);
            }
        }

        CACE_LOG_DEBUG("decoded %d ARI items in the line", ari_list_size(data));
        if (!ari_list_empty_p(data))
        {
            // stop when something received
            break;
        }
    }

    return 0;
}

static void show_usage(const char *argv0)
{
    fprintf(stderr, "Usage: %s {-h} -a <agent EID>\n", argv0);
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
            while ((opt = getopt(argc, argv, "hl:a:")) != -1)
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
//    amp_agent_init();
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

    if (!retval)
    {
        if (refda_agent_send_hello(&agent))
        {
            CACE_LOG_ERR("Agent hello failed");
            retval = 3;
        }
        else
        {
            CACE_LOG_INFO("Sent hello report");
        }
    }

#if 1
    {
        ari_list_t data;
        ari_list_init(data);
        cace_amm_msg_if_metadata_t metadata;
        cace_amm_msg_if_metadata_init(&metadata);
        stdin_recv(data, &metadata, &agent.running, NULL);
        stdout_send(data, &metadata, NULL);
        cace_amm_msg_if_metadata_deinit(&metadata);
        ari_list_clear(data);
    }
#else
    if (!retval)
    {
        // Block until stopped
        daemon_run_wait(&agent.running);
        CACE_LOG_INFO("Agent is shutting down");
    }
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

    CACE_LOG_DEBUG("Agent shutdown completed");
    cace_closelog();
    return retval;
}
