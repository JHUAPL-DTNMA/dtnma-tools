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
#include "proxy_if.h"
#include "shared/utils/utils.h"
#include <qcbor/qcbor.h>
#include <sys/socket.h>
#include <errno.h>

int proxy_if_send(int sock_fd, const eid_t *dst, const uint8_t *data_ptr, size_t data_len)
{
  int result = AMP_OK;

  // message header is just the source EID
  m_bstring_t msgbuf;
  m_bstring_init(msgbuf);
  {
    QCBOREncodeContext enc;
    // largest header size possible
    const size_t head_alloc = 5 + strlen(dst->name);
    m_bstring_resize(msgbuf, head_alloc);
    uint8_t     *msg_begin = m_bstring_acquire_access(msgbuf, 0, head_alloc);

    QCBOREncode_Init(&enc, (UsefulBuf){ .ptr = msg_begin, .len = head_alloc });
    QCBOREncode_AddSZString(&enc, dst->name);
    size_t used;
    QCBORError err = QCBOREncode_FinishGetSize(&enc, &used);
    if (err != QCBOR_SUCCESS)
    {
      AMP_DEBUG_ERR(__func__, "failed CBOR encode");
      result = AMP_FAIL;
    }
    m_bstring_release_access(msgbuf);
    m_bstring_resize(msgbuf, used);
  }

  if (result == AMP_OK)
  {
    // concatenate ADU data
    m_bstring_push_back_bytes(msgbuf, data_len, data_ptr);

    const size_t msg_size  = m_bstring_size(msgbuf);
    const uint8_t     *msg_begin = m_bstring_view(msgbuf, 0, msg_size);

    int flags = 0;
    AMP_DEBUG_INFO(__func__, "Sending socket datagram with %zd octets", msg_size);
    ssize_t got = send(sock_fd, msg_begin, msg_size, flags);
    m_bstring_clear(msgbuf);

    if (got != (ssize_t)msg_size)
    {
      AMP_DEBUG_ERR(__func__, "failed send()");
      result = AMP_FAIL;
    }
  }

  return result;
}

int proxy_if_recv(int sock_fd, eid_t *src, m_bstring_t data)
{
  // first peek at message size
  int     flags = MSG_PEEK | MSG_TRUNC;
  ssize_t got   = recv(sock_fd, NULL, 0, flags);
  if (got == 0)
  {
    AMP_DEBUG_INFO(__func__, "empty recv() message");
    return AMP_FAIL;
  }
  else if (got < 0)
  {
    AMP_DEBUG_WARN(__func__, "ignoring failed recv() with errno %d", errno);
    return AMP_FAIL;
  }
  AMP_DEBUG_INFO(__func__, "Peeked socket datagram with %zd octets", got);

  int result = AMP_OK;

  m_bstring_t msgbuf;
  m_bstring_init(msgbuf);
  {
    m_bstring_resize(msgbuf, got);
    const size_t msg_size  = m_bstring_size(msgbuf);
    uint8_t     *msg_begin = m_bstring_acquire_access(msgbuf, 0, msg_size);

    flags = 0;
    got = recv(sock_fd, msg_begin, msg_size, flags);
    m_bstring_release_access(msgbuf);
    if (got <= 0)
    {
      AMP_DEBUG_WARN(__func__, "ignoring failed recv() with errno %d", errno);
      result = AMP_FAIL;
    }
  }
  AMP_DEBUG_INFO(__func__, "Received socket datagram with %zd octets", got);

  const uint8_t *msg_begin = m_bstring_view(msgbuf, 0, got);
  size_t head_len;
  if (result == AMP_OK)
  {
    QCBORDecodeContext dec;

    QCBORDecode_Init(&dec, (UsefulBufC){ .ptr = msg_begin, .len = got }, QCBOR_DECODE_MODE_NORMAL);

    QCBORItem decitem;
    QCBORError err = QCBORDecode_GetNext(&dec, &decitem);
    if (decitem.uDataType != QCBOR_TYPE_TEXT_STRING)
    {
      AMP_DEBUG_ERR(__func__, "CBOR decoding error code %d", err);
      result = AMP_FAIL;
    }
    else
    {
      // source is not null-terminated
      if (sizeof(src->name) < decitem.val.string.len + 1)
      {
        AMP_DEBUG_ERR(__func__, "source EID is too large to fit");
        result = AMP_FAIL;
      }
      else
      {
        memcpy(src->name, decitem.val.string.ptr, decitem.val.string.len);
        src->name[decitem.val.string.len] = '\0';
      }
    }

    head_len = UsefulInputBuf_Tell(&(dec.InBuf));
    err = QCBORDecode_Finish(&dec);
    switch (err) {
      case QCBOR_SUCCESS:
      case QCBOR_ERR_EXTRA_BYTES:
        break;
      default:
        AMP_DEBUG_ERR(__func__, "CBOR decoding error code %d", err);
        result = AMP_FAIL;
    }
  }

  if (result == AMP_OK)
  {
    // copy by value into result
    const size_t data_size = got - head_len;
    m_bstring_push_back_bytes(data, data_size, msg_begin + head_len);
    AMP_DEBUG_ERR(__func__, "Received message with source %s data length %zd", src->name, data_size);
  }

  m_bstring_clear(msgbuf);
  return result;
}
