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
#include <cace/ari/text_util.h>
#include <cace/ari/text.h>
#include <cace/ari/cbor.h>
#include <cace/util/logging.h>
#include <cace/util/defs.h>
#include <sys/poll.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <signal.h>
#include <unistd.h>
#include <errno.h>

/// Per-process state
static refdm_mgr_t mgr;

static void daemon_signal_handler(int signum)
{
    CACE_LOG_DEBUG("Received signal %d", signum);
    CACE_LOG_INFO("Signaling shutdown");
    daemon_run_stop(&mgr.running);
}

static int sock_send(const ari_list_t data, const cace_amm_msg_if_metadata_t *meta, void *ctx)
{
    const int *sock_fd = ctx;
    CHKERR1(sock_fd);

    int retval = 0;
    CACE_LOG_DEBUG("Sending message with %d ARIs", ari_list_size(data));

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
        CACE_LOG_DEBUG("encoded ARI item to base-16: %s", string_get_cstr(outhex));

        if (!retval)
        {
            struct sockaddr_un daddr;
            daddr.sun_family = AF_UNIX;
            strncpy(daddr.sun_path, (const char *)meta->dest.ptr, meta->dest.len);

            int flags = 0;
            if (sendto(*sock_fd, string_get_cstr(outhex), string_size(outhex), flags, (struct sockaddr *)&daddr,
                       sizeof(daddr))
                <= 0)
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
        CACE_LOG_DEBUG("flushed stdout");
    }

    return retval;
}

static int sock_recv(ari_list_t data, cace_amm_msg_if_metadata_t *meta, daemon_run_t *running, void *ctx)
{
    CHKERR1(data);
    CHKERR1(meta);
    CHKERR1(running);
    const int *sock_fd = ctx;
    CHKERR1(sock_fd);

    // Watch stdin (fd 0) for input, assuming whole-lines are given
    struct pollfd pfds[] = {
        { .fd = *sock_fd, .events = POLLIN | POLLERR | POLLHUP },
    };
    struct pollfd *poll_sock = pfds + 0;

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
            if (!daemon_run_get(running))
            {
                CACE_LOG_DEBUG("returning due to running state change");
                return CACE_AMM_MSG_IF_RECV_END;
            }
            continue;
        }

        if (poll_sock->revents & POLLIN)
        {
            int     flags = MSG_PEEK | MSG_TRUNC;
            ssize_t got   = recvfrom(poll_sock->fd, NULL, 0, flags, NULL, NULL);
            if (got <= 0)
            {
                CACE_LOG_WARNING("ignoring failed recvfrom() with status %d", errno);
                continue;
            }
            CACE_LOG_DEBUG("peeked datagram with %zd octets", got);

            cace_data_t buf;
            cace_data_init(&buf);
            cace_data_resize(&buf, got);

            struct sockaddr_un saddr;
            saddr.sun_family = AF_UNIX;

            socklen_t saddr_len = sizeof(saddr);

            flags = 0;
            got   = recvfrom(poll_sock->fd, buf.ptr, buf.len, flags, (struct sockaddr *)&saddr, &saddr_len);
            if (got < 0)
            {
                CACE_LOG_WARNING("ignoring failed recvfrom() with status %d", errno);
                cace_data_deinit(&buf);
                return 3;
            }
            {
                m_string_t eid;
                m_string_init(eid);
                m_string_printf(eid, "file:%s", saddr.sun_path);
                CACE_LOG_DEBUG("read datagram with %zd octets from %s", got, string_get_cstr(eid));
                cace_data_copy_from_cstr(&meta->src, string_get_cstr(eid));
                m_string_clear(eid);
            }

            while (buf.len > 0)
            {
                ari_t item;
                ari_init(&item);

                int         res;
                size_t      used;
                const char *errm = NULL;
                res              = ari_cbor_decode(&item, &buf, &used, &errm);
                if (used)
                {
                    // chop off used data
                    cace_data_extend_front(&buf, -used);
                }

                if (res)
                {
                    CACE_LOG_ERR("Failed to decode CBOR ARI (err %d): %s\n", res, errm);
                }
                if (errm)
                {
                    ARI_FREE((char *)errm);
                }
                if (res)
                {
                    ari_deinit(&item);
                    return 3;
                }

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
            cace_data_deinit(&buf);

            CACE_LOG_DEBUG("decoded %d ARI items in the datagram", ari_list_size(data));
            if (!ari_list_empty_p(data))
            {
                // stop when something received
                break;
            }
        }
        if (poll_sock->revents & (POLLERR | POLLHUP))
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
    fprintf(stderr, "Usage: %s {-h} -a <listen-socket>\n", argv0);
}

int main(int argc, char *argv[])
{
    // keep track of failure state
    int retval = 0;

    cace_openlog();
    refdm_mgr_init(&mgr);

    /* Process Command Line Arguments. */
    int        log_limit = LOG_WARNING;
    m_string_t sock_path;
    m_string_init(sock_path);
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
                        string_set_str(sock_path, optarg);
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
    if (!retval && m_string_empty_p(sock_path))
    {
        fprintf(stderr, "A manager socket path must be supplied");
        retval = 1;
    }

    int sock_fd = 0;
    if (!retval)
    {
        // Set up listen socket
        sock_fd = socket(AF_UNIX, SOCK_DGRAM, 0);

        struct sockaddr_un laddr;
        laddr.sun_family = AF_UNIX;
        strncpy(laddr.sun_path, string_get_cstr(sock_path), string_size(sock_path) + 1);
        CACE_LOG_DEBUG("Binding to socket %s", laddr.sun_path);

        unlink(string_get_cstr(sock_path));

        int res = bind(sock_fd, (struct sockaddr *)&laddr, sizeof(laddr));
        if (res)
        {
            CACE_LOG_ERR("Failed to bind to socket %s with error %d", laddr.sun_path, errno);
            retval = 4;
        }
    }

    if (!retval)
    {
        m_string_printf(mgr.own_eid, "file:%s", string_get_cstr(sock_path));

        CACE_LOG_DEBUG("Running as endpoint %s", string_get_cstr(mgr.own_eid));
        mgr.mif.send = sock_send;
        mgr.mif.recv = sock_recv;
        mgr.mif.ctx  = &sock_fd;
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
    CACE_LOG_INFO("READY");

    if (!retval)
    {
        // Block until stopped
        daemon_run_wait(&mgr.running);
        CACE_LOG_INFO("Manager is shutting down");
    }

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
    if (unlink(string_get_cstr(sock_path)))
    {
        CACE_LOG_WARNING("Failed to remove socket %s with error %d", string_get_cstr(sock_path), errno);
    }

    CACE_LOG_DEBUG("Manager shutdown completed");
    cace_closelog();
    return retval;
}
