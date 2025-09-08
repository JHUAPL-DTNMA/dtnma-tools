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
#include "proxy_cli.h"
#include "msg.h"
#include "cace/ari/cbor.h"
#include "cace/ari/text.h"
#include "cace/util/logging.h"
#include "cace/util/defs.h"
#include <qcbor/qcbor.h>
#include <m-bstring.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <errno.h>

void cace_amp_proxy_cli_state_init(cace_amp_proxy_cli_state_t *state)
{
    CHKVOID(state);
    m_string_init(state->path);
    pthread_mutex_init(&state->sock_mutex, NULL);
    state->sock_fd = -1;
}

void cace_amp_proxy_cli_state_deinit(cace_amp_proxy_cli_state_t *state)
{
    CHKVOID(state);
    pthread_mutex_destroy(&state->sock_mutex);
    m_string_clear(state->path);
}

/** Check socket state and reconnect if necessary.
 * @pre This occurs before holding the mutex lock.
 * @return The socket FD to use outside of the mutex lock.
 */
static int cace_amp_proxy_cli_real_connect(cace_amp_proxy_cli_state_t *state)
{
    int ret = -1;
    pthread_mutex_lock(&state->sock_mutex);
    if (state->sock_fd >= 0)
    {
        ret = state->sock_fd;
    }
    else
    {
        struct sockaddr_un laddr;
        laddr.sun_family = AF_UNIX;
        char *sun_end    = stpncpy(laddr.sun_path, m_string_get_cstr(state->path), sizeof(laddr.sun_path));
        if (sun_end - laddr.sun_path >= (ssize_t)sizeof(laddr.sun_path))
        {
            CACE_LOG_ERR("given path that is too long to fit in sockaddr_un");
            ret = -2;
        }

        for (int try_ix = 0; (ret == -1) && (try_ix < 600); ++try_ix)
        {
            state->sock_fd = socket(AF_UNIX, SOCK_SEQPACKET, 0);
            if (state->sock_fd == -1)
            {
                CACE_LOG_ERR("Failed to create socket %d", errno);
                return -1;
            }

            CACE_LOG_INFO("Connecting to socket %s", laddr.sun_path);
            int res = connect(state->sock_fd, (struct sockaddr *)&laddr, sizeof(laddr));
            if (res)
            {
                CACE_LOG_ERR("Failed to connect socket %s with errno %d", laddr.sun_path, errno);
                close(state->sock_fd);
                state->sock_fd = -2;
            }
            else
            {
                ret = state->sock_fd;
                break;
            }

            // linear back-off
            sleep(try_ix);
        }
    }

    pthread_mutex_unlock(&state->sock_mutex);
    return ret;
}

int cace_amp_proxy_cli_state_connect(cace_amp_proxy_cli_state_t *state, const m_string_t sock_path)
{
    CHKERR1(state);
    pthread_mutex_lock(&state->sock_mutex);
    m_string_set(state->path, sock_path);
    pthread_mutex_unlock(&state->sock_mutex);

    // just try the connection
    int sock_fd = cace_amp_proxy_cli_real_connect(state);
    return (sock_fd < 0);
}

void cace_amp_proxy_cli_state_disconnect(cace_amp_proxy_cli_state_t *state)
{
    CHKVOID(state);
    pthread_mutex_lock(&state->sock_mutex);
    const char *path = m_string_get_cstr(state->path);

    if (state->sock_fd >= 0)
    {
        CACE_LOG_DEBUG("Unbinding from socket %s", path);
        shutdown(state->sock_fd, SHUT_RDWR);
        close(state->sock_fd);
        state->sock_fd = -1;
    }

    m_string_reset(state->path);
    pthread_mutex_unlock(&state->sock_mutex);
}

int cace_amp_proxy_cli_send(const cace_ari_list_t data, const cace_amm_msg_if_metadata_t *meta, void *ctx)
{
    CHKERR1(meta);
    cace_amp_proxy_cli_state_t *state = ctx;
    CHKERR1(state);

    int sock_fd = cace_amp_proxy_cli_real_connect(state);

    int result = 0;

    // proxy header is just the source EID
    m_bstring_t msgbuf;
    m_bstring_init(msgbuf);
    {
        cace_data_t outbin;
        cace_data_init(&outbin);
        if (cace_ari_cbor_encode(&outbin, &meta->dest))
        {
            result = 2;
        }
        else
        {
            m_bstring_push_back_bytes(msgbuf, outbin.len, outbin.ptr);
            cace_data_deinit(&outbin);
        }
    }

    if (!result)
    {
        if (cace_amp_msg_encode(msgbuf, data))
        {
            CACE_LOG_ERR("failed message encode");
            result = 3;
        }
    }

    if (!result)
    {
        const size_t   msg_size  = m_bstring_size(msgbuf);
        const uint8_t *msg_begin = m_bstring_view(msgbuf, 0, msg_size);

        {
            string_t buf;
            string_init(buf);
            cace_ari_text_encode(buf, &meta->dest, CACE_ARI_TEXT_ENC_OPTS_DEFAULT);
            CACE_LOG_INFO("Sending proxy datagram with %zd octets to %s", msg_size, m_string_get_cstr(buf));
            string_clear(buf);
        }

        int     flags = 0;
        ssize_t got   = send(sock_fd, msg_begin, msg_size, flags);

        if (got != (ssize_t)msg_size)
        {
            CACE_LOG_ERR("failed send()");
            cace_amp_proxy_cli_state_disconnect(state);
            result = 4;
        }
    }
    m_bstring_clear(msgbuf);

    return result;
}

int cace_amp_proxy_cli_recv(cace_ari_list_t data, cace_amm_msg_if_metadata_t *meta, cace_daemon_run_t *running,
                            void *ctx)
{
    CHKERR1(meta);
    CHKERR1(running);
    cace_amp_proxy_cli_state_t *state = ctx;
    CHKERR1(state);

    int sock_fd = cace_amp_proxy_cli_real_connect(state);

    // first peek at message size
    int     flags = MSG_PEEK | MSG_TRUNC;
    ssize_t got   = recv(sock_fd, NULL, 0, flags);
    if (got == 0)
    {
        CACE_LOG_INFO("empty recv() message");
        return CACE_AMM_MSG_IF_RECV_END;
    }
    else if (got < 0)
    {
        CACE_LOG_WARNING("ignoring failed recv() with errno %d", errno);
        cace_amp_proxy_cli_state_disconnect(state);
        return CACE_AMM_MSG_IF_RECV_END;
    }
    CACE_LOG_INFO("Peeked socket datagram with %zd octets", got);

    int result = 0;

    m_bstring_t msgbuf;
    m_bstring_init(msgbuf);
    {
        m_bstring_resize(msgbuf, got);
        const size_t msg_size  = m_bstring_size(msgbuf);
        uint8_t     *msg_begin = m_bstring_acquire_access(msgbuf, 0, msg_size);

        flags = 0;
        got   = recv(sock_fd, msg_begin, msg_size, flags);
        m_bstring_release_access(msgbuf);
        if (got < 0)
        {
            CACE_LOG_WARNING("ignoring failed recv() with errno %d", errno);
            result = CACE_AMM_MSG_IF_RECV_END;
        }
        else if (got == 0)
        {
            if (!cace_daemon_run_get(running))
            {
                CACE_LOG_DEBUG("returning due to running state change");
                result = CACE_AMM_MSG_IF_RECV_END;
            }
        }
    }
    CACE_LOG_INFO("Received socket datagram with %zd octets", got);

    // Decode the proxy header
    const uint8_t *msg_begin = m_bstring_view(msgbuf, 0, got);
    size_t         head_len;
    if (!result)
    {
        cace_data_t view;
        cace_data_init_view(&view, got, (uint8_t *)msg_begin);
        if (cace_ari_cbor_decode(&meta->src, &view, &head_len, NULL))
        {
            result = 3;
        }
        cace_data_deinit(&view);
    }

    if (!result)
    {
        // view past the proxy header
        const size_t   msgbuf_len = got - head_len;
        const uint8_t *msgbuf_ptr = m_bstring_view(msgbuf, head_len, msgbuf_len);
        if (cace_amp_msg_decode(data, msgbuf_ptr, msgbuf_len))
        {
            CACE_LOG_ERR("failed message decode");
            result = 4;
        }

        string_t buf;
        string_init(buf);
        cace_ari_text_encode(buf, &meta->src, CACE_ARI_TEXT_ENC_OPTS_DEFAULT);
        CACE_LOG_INFO("Received proxy datagram with %zd octets to %s", got, m_string_get_cstr(buf));
        string_clear(buf);
    }

    m_bstring_clear(msgbuf);
    return result;
}
