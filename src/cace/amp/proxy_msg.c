/*
 * Copyright (c) 2025 The Johns Hopkins University Applied Physics
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
    int result = 0;

    // message header is just the ultimate destination EID
    m_bstring_t msgbuf;
    m_bstring_init(msgbuf);
    {
        QCBOREncodeContext enc;
        if (cace_ari_cbor_encode_stream(&enc, dst))
        {
            result = 2;
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
        m_bstring_clear(msgbuf);

        if (got != (ssize_t)msg_size)
        {
            CACE_LOG_ERR("failed send()");
            result = 1;
        }
    }

    return result;
}

int cace_amp_proxy_msg_recv(int sock_fd, cace_ari_t *src, m_bstring_t data)
{
    // first peek at message size
    int     flags = MSG_PEEK | MSG_TRUNC;
    ssize_t got   = recv(sock_fd, NULL, 0, flags);
    if (got == 0)
    {
        CACE_LOG_INFO("ignorning empty recv() message");
        return 2;
    }
    else if (got < 0)
    {
        CACE_LOG_WARNING("ignoring failed recv() with errno %d", errno);
        return 3;
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
        if (got <= 0)
        {
            CACE_LOG_WARNING("ignoring failed recv() with errno %d", errno);
            result = 4;
        }
    }
    CACE_LOG_INFO("Received socket datagram with %zd octets", got);

    const uint8_t *msg_begin = m_bstring_view(msgbuf, 0, got);
    size_t         head_len;
    if (!result)
    {
        QCBORDecodeContext dec;
        QCBORDecode_Init(&dec, (UsefulBufC) { .ptr = msg_begin, .len = got }, QCBOR_DECODE_MODE_NORMAL);

        int ret = cace_ari_cbor_decode_stream(&dec, src);
        if (ret)
        {
            CACE_LOG_ERR("Source EID decoding error code %d", ret);
            result = 5;
        }
        else
        {
            // source is not valid type
            if (!cace_ari_cget_tstr(src) || !(src->is_ref))
            {
                CACE_LOG_ERR("Source EID is not the right type");
                result = 6;
            }
        }

        head_len = QCBORDecode_Tell(&dec);
    }

    if (!result)
    {
        // copy by value into result
        const size_t data_size = got - head_len;
        m_bstring_push_back_bytes(data, data_size, msg_begin + head_len);

        string_t buf;
        string_init(buf);
        cace_ari_text_encode(buf, src, CACE_ARI_TEXT_ENC_OPTS_DEFAULT);
        CACE_LOG_ERR("Received message with source %s data length %zd", m_string_get_cstr(buf), data_size);
        string_clear(buf);
    }

    m_bstring_clear(msgbuf);
    return result;
}
