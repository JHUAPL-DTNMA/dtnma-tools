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
#include "proxy_msg.h"
#include "cace/ari/cbor.h"
#include "cace/ari/text.h"
#include "cace/util/logging.h"
#include "cace/util/defs.h"
#include <qcbor/qcbor.h>
#include <sys/socket.h>
#include <errno.h>

int cace_amp_proxy_msg_send(int sock_fd, const cace_ari_t *dst, const uint8_t *data_ptr, size_t data_len)
{
    CHKERR1(sock_fd >= 0);
    CHKERR1(dst);
    CHKERR1(data_ptr);
    int result = 0;

    // message header is just the ultimate peer EID as an ARI
    m_bstring_t msgbuf;
    m_bstring_init(msgbuf);
    {
        cace_data_t outbin;
        cace_data_init(&outbin);
        if (cace_ari_cbor_encode(&outbin, dst))
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
        // concatenate ADU data
        m_bstring_push_back_bytes(msgbuf, data_len, data_ptr);

        const size_t   msg_size  = m_bstring_size(msgbuf);
        const uint8_t *msg_begin = m_bstring_view(msgbuf, 0, msg_size);

        int flags = 0;
        CACE_LOG_INFO("Sending socket datagram with %zd octets", msg_size);
        ssize_t got = send(sock_fd, msg_begin, msg_size, flags);

        if (got != (ssize_t)msg_size)
        {
            CACE_LOG_ERR("failed send()");
            result = 3;
        }
    }
    m_bstring_clear(msgbuf);

    return result;
}

int cace_amp_proxy_msg_recv(int sock_fd, cace_ari_t *src, m_bstring_t data)
{
    CHKERR1(sock_fd >= 0);
    CHKERR1(src);

    int result = 0;

    m_bstring_t msgbuf;
    m_bstring_init(msgbuf);

    while (true)
    {
        // first peek at message size
        int     flags = MSG_PEEK | MSG_TRUNC;
        ssize_t got   = recv(sock_fd, NULL, 0, flags);
        if (got == 0)
        {
            CACE_LOG_INFO("ignorning empty recv() message");
            break;
        }
        else if (got < 0)
        {
            CACE_LOG_WARNING("ignoring failed recv() with errno %d", errno);
            result = 3;
            break;
        }
        CACE_LOG_INFO("Peeked socket datagram with %zd octets", got);

        {
            m_bstring_resize(msgbuf, got);
            const size_t msg_size  = m_bstring_size(msgbuf);
            uint8_t     *msg_begin = m_bstring_acquire_access(msgbuf, 0, msg_size);

            flags = 0;
            got   = recv(sock_fd, msg_begin, msg_size, flags);
            m_bstring_release_access(msgbuf);
            if (got <= 0)
            {
                CACE_LOG_WARNING("ignoring failed recv() with errno %d", errno);
                result = 3;
                break;
            }
        }
        CACE_LOG_INFO("Received socket datagram with %zd octets", got);

        // Decode the proxy header
        const uint8_t *msg_begin = m_bstring_view(msgbuf, 0, got);
        size_t         head_len;
        {
            cace_data_t view;
            cace_data_init_view(&view, got, (uint8_t *)msg_begin);

            int ret = cace_ari_cbor_decode(src, &view, &head_len, NULL);
            if (ret)
            {
                CACE_LOG_ERR("Peer EID decoding error code %d", ret);
                result = 3;
            }
            cace_data_deinit(&view);
            if (result)
            {
                continue;
            }
        }

        {
            // copy by value into result
            const size_t data_size = got - head_len;
            m_bstring_push_back_bytes(data, data_size, msg_begin + head_len);

            if (cace_log_is_enabled_for(LOG_INFO))
            {
                m_string_t buf;
                m_string_init(buf);
                cace_ari_text_encode(buf, src, CACE_ARI_TEXT_ENC_OPTS_DEFAULT);
                CACE_LOG_INFO("Received message with peer %s data length %zd", m_string_get_cstr(buf), data_size);
                m_string_clear(buf);
            }
            // got valid message
            break;
        }
    }

    m_bstring_clear(msgbuf);
    return result;
}
