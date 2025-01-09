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
#include "socket.h"
#include "msg.h"
#include <cace/util/logging.h>
#include <cace/util/defs.h>
#include <m-bstring.h>
#include <sys/poll.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <signal.h>
#include <strings.h>
#include <unistd.h>
#include <errno.h>

#define URI_PREFIX "file:"

static void cace_amp_sock_state_unbind(cace_amp_socket_state_t *state);

void cace_amp_socket_state_init(cace_amp_socket_state_t *state)
{
    CHKVOID(state);
    m_string_init(state->path);
    state->sock_fd = 0;
}

void cace_amp_socket_state_deinit(cace_amp_socket_state_t *state)
{
    CHKVOID(state);
    cace_amp_sock_state_unbind(state);
    m_string_clear(state->path);
}

int cace_amp_socket_state_bind(cace_amp_socket_state_t *state, const m_string_t sock_path)
{
    m_string_set(state->path, sock_path);

    // Set up listen socket
    state->sock_fd = socket(AF_UNIX, SOCK_DGRAM, 0);

    struct sockaddr_un laddr;
    laddr.sun_family = AF_UNIX;
    strncpy(laddr.sun_path, string_get_cstr(state->path), string_size(state->path) + 1);
    CACE_LOG_DEBUG("Binding to socket %s", laddr.sun_path);

    // preemptive unlink
    unlink(string_get_cstr(state->path));

    int res = bind(state->sock_fd, (struct sockaddr *)&laddr, sizeof(laddr));
    if (res)
    {
        CACE_LOG_ERR("Failed to bind to socket %s with error %d", laddr.sun_path, errno);
        cace_amp_sock_state_unbind(state);
        return 1;
    }

    return 0;
}

int cace_amp_socket_send(const ari_list_t data, const cace_amm_msg_if_metadata_t *meta, void *ctx)
{
    cace_amp_socket_state_t *state = ctx;
    CHKERR1(state);

    int retval = 0;
    CACE_LOG_DEBUG("Sending message with %d ARIs", ari_list_size(data));

    const char *dst_ptr = (const char *)meta->dest.ptr;
    if (dst_ptr[meta->dest.len - 1] != '\0')
    {
        CACE_LOG_ERR("given dest that is not text");
        return 1;
    }
    const size_t prefix_len = strlen(URI_PREFIX);
    size_t       dst_len    = strlen(dst_ptr);
    if ((dst_len < prefix_len) || (strncasecmp(dst_ptr, URI_PREFIX, prefix_len) != 0))
    {
        CACE_LOG_ERR("given dest that is not a \"file\" scheme: %s", dst_ptr);
        return 1;
    }
    dst_ptr += strlen(URI_PREFIX);
    dst_len -= prefix_len;

    m_bstring_t msgbuf;
    m_bstring_init(msgbuf);
    if (cace_amp_msg_encode(msgbuf, data))
    {
        retval = 2;
    }

    if (!retval)
    {
        struct sockaddr_un daddr;
        daddr.sun_family = AF_UNIX;
        strncpy(daddr.sun_path, dst_ptr, dst_len);

        const size_t   msg_size  = m_bstring_size(msgbuf);
        const uint8_t *msg_begin = m_bstring_view(msgbuf, 0, msg_size);

        CACE_LOG_DEBUG("sending datagram with %zd octets to %s", msg_size, daddr.sun_path);
        int     flags = 0;
        ssize_t got   = sendto(state->sock_fd, msg_begin, msg_size, flags, (struct sockaddr *)&daddr, sizeof(daddr));
        if (got < 0)
        {
            CACE_LOG_WARNING("failed sendto() with errno %d", errno);
            retval = 3;
        }
        else if (got < (ssize_t)msg_size)
        {
            CACE_LOG_ERR("failed sendto(), sent only %z of %z", got, msg_size);
            retval = 4;
        }
    }

    m_bstring_clear(msgbuf);

    return retval;
}

static void cace_amp_sock_state_unbind(cace_amp_socket_state_t *state)
{
    if (state->sock_fd)
    {
        close(state->sock_fd);
        state->sock_fd = 0;
    }

    if (unlink(string_get_cstr(state->path)))
    {
        CACE_LOG_WARNING("Failed to remove socket %s with errno %d", string_get_cstr(state->path), errno);
    }
}

int cace_amp_socket_recv(ari_list_t data, cace_amm_msg_if_metadata_t *meta, daemon_run_t *running, void *ctx)
{
    cace_amp_socket_state_t *state = ctx;
    CHKERR1(state);
    CHKERR1(data);
    CHKERR1(meta);
    CHKERR1(running);

    // Watch stdin (fd 0) for input, assuming whole-lines are given
    struct pollfd pfds[] = {
        { .fd = state->sock_fd, .events = POLLIN | POLLERR | POLLHUP },
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
                CACE_LOG_WARNING("ignoring failed recvfrom() with errno %d", errno);
                continue;
            }
            CACE_LOG_DEBUG("peeked datagram with %zd octets", got);

            m_bstring_t msgbuf;
            m_bstring_init(msgbuf);
            m_bstring_resize(msgbuf, got);

            struct sockaddr_un saddr;
            saddr.sun_family = AF_UNIX;

            const size_t msg_size  = m_bstring_size(msgbuf);
            uint8_t     *msg_begin = m_bstring_acquire_access(msgbuf, 0, msg_size);

            socklen_t saddr_len = sizeof(saddr);

            flags = 0;
            got   = recvfrom(poll_sock->fd, msg_begin, msg_size, flags, (struct sockaddr *)&saddr, &saddr_len);
            m_bstring_release_access(msgbuf);
            if (got < 0)
            {
                CACE_LOG_WARNING("ignoring failed recvfrom() with errno %d", errno);
                m_bstring_clear(msgbuf);
                continue;
            }
            {
                m_string_t eid;
                m_string_init(eid);
                m_string_printf(eid, URI_PREFIX "%s", saddr.sun_path);
                CACE_LOG_DEBUG("read datagram with %zd octets from %s", got, string_get_cstr(eid));
                cace_data_copy_from_cstr(&meta->src, string_get_cstr(eid));
                m_string_clear(eid);
            }

            if (cace_amp_msg_decode(data, msgbuf))
            {
                m_bstring_clear(msgbuf);
                continue;
            }
            m_bstring_clear(msgbuf);

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
