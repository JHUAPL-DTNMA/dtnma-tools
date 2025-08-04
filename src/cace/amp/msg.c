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
#include "msg.h"
#include <cace/ari/cbor.h>
#include <cace/ari/text.h>
#include <cace/util/logging.h>
#include <cace/util/defs.h>
#include <qcbor/qcbor_spiffy_decode.h>

int cace_amp_msg_encode(m_bstring_t msgbuf, const cace_ari_list_t items)
{
    int retval = 0;

    // manually encoded CBOR value 1
    m_bstring_push_back(msgbuf, '\x01');

    cace_data_t outbin;
    cace_data_init(&outbin);

    cace_ari_list_it_t ait;
    for (cace_ari_list_it(ait, items); !cace_ari_list_end_p(ait); cace_ari_list_next(ait))
    {
        const cace_ari_t *item = cace_ari_list_cref(ait);

        if (cace_log_is_enabled_for(LOG_DEBUG))
        {
            string_t buf;
            string_init(buf);
            cace_ari_text_encode(buf, item, CACE_ARI_TEXT_ENC_OPTS_DEFAULT);
            CACE_LOG_DEBUG("encoding ARI item: %s", string_get_cstr(buf));
            string_clear(buf);
        }

        if (cace_ari_cbor_encode(&outbin, item))
        {
            CACE_LOG_ERR("Failed to binary encode ARI");
            retval = 2;
            break;
        }

        m_bstring_push_back_bytes(msgbuf, outbin.len, outbin.ptr);
        // leave outbin initialized, will be overwritten next value
    }
    cace_data_deinit(&outbin);
    return retval;
}

int cace_amp_msg_decode(cace_ari_list_t items, const uint8_t *msgbuf_ptr, size_t msgbuf_len)
{
    int retval = 0;

    size_t remain = msgbuf_len;
    size_t offset = 0;

    // ensure version number is present and correct
    {
        QCBORDecodeContext dec;
        UsefulBufC         indata = { .ptr = msgbuf_ptr, .len = remain };
        QCBORDecode_Init(&dec, indata, QCBOR_DECODE_MODE_MAP_AS_ARRAY);

        uint64_t versnum;
        QCBORDecode_GetUInt64(&dec, &versnum);

        size_t     used;
        QCBORError err = QCBORDecode_PartialFinish(&dec, &used);
        switch (err)
        {
            case QCBOR_SUCCESS:
            case QCBOR_ERR_EXTRA_BYTES:
                // extra data after header is expected
                break;
            default:
                CACE_LOG_ERR("Failed to decode AMP version number, error %d", err);
                retval = 3;
                break;
        }

        if (!retval && (versnum != 1))
        {
            CACE_LOG_ERR("Incompatible AMP version number: %" PRIu64, versnum);
            retval = 3;
        }

        offset += used;
        remain -= used;
    }

    while (!retval && (remain > 0))
    {
        cace_data_t view;
        // not really mutable, but needed for the view interface
        cace_data_init_view(&view, remain, (uint8_t *)msgbuf_ptr + offset);

        cace_ari_t item;
        cace_ari_init(&item);

        int         res;
        size_t      used;
        char *errm = NULL;

        res = cace_ari_cbor_decode(&item, &view, &used, &errm);
        cace_data_deinit(&view);
        if (used)
        {
            // make progress even if decoding failed
            offset += used;
            remain -= used;
        }
        if (res)
        {
            CACE_LOG_ERR("Failed to decode CBOR ARI (err %d): %s\n", res, errm);
            CACE_FREE(errm);

            cace_ari_deinit(&item);
            retval = 3;
            break;
        }

        if (cace_log_is_enabled_for(LOG_DEBUG))
        {
            string_t buf;
            string_init(buf);
            cace_ari_text_encode(buf, &item, CACE_ARI_TEXT_ENC_OPTS_DEFAULT);
            CACE_LOG_DEBUG("decoded ARI item: %s", string_get_cstr(buf));
            string_clear(buf);
        }

        cace_ari_list_push_back_move(items, &item);
    }

    return retval;
}
