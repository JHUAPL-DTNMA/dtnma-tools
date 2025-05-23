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

/** @file
 * @ingroup amp
 * Provide an ION BP adapter for AMP messaging.
 */
#include "ion_bp.h"
#include "msg.h"
#include "cace/ari/text.h"
#include "cace/util/logging.h"
#include "cace/util/defs.h"

#if defined(ION_FOUND)
#include <bp.h>

void cace_amp_ion_bp_state_init(cace_amp_ion_bp_state_t *state)
{
    CHKVOID(state);
    m_string_init(state->eid);
}

void cace_amp_ion_bp_state_deinit(cace_amp_ion_bp_state_t *state)
{
    CHKVOID(state);
    cace_amp_ion_bp_state_unbind(state);
    m_string_clear(state->eid);
}

int cace_amp_ion_bp_state_bind(cace_amp_ion_bp_state_t *state, const m_string_t eid)
{
    CHKVOID(state);
    int res = bp_open((char *)m_string_get_cstr(eid), &(state->sap));
    if (!res)
    {
        m_string_set(state->eid, eid);
    }
    return res;
}

void cace_amp_ion_bp_state_unbind(cace_amp_ion_bp_state_t *state)
{
    CHKVOID(state);
    if (!m_string_empty_p(state->eid))
    {
        bp_close(state->sap);
    }
    m_string_reset(state->eid);
}

int cace_amp_ion_bp_send(const cace_ari_list_t data, const cace_amm_msg_if_metadata_t *meta, void *ctx)
{
    CHKERR1(data);
    CHKERR1(meta);
    cace_amp_ion_bp_state_t *state = ctx;
    CHKERR1(state);

    char *dest_eid = NULL;
    {
        const cace_data_t *dest_data = cace_ari_cget_tstr(&meta->dest);
        if (dest_data)
        {
            dest_eid = (char *)(dest_data->ptr);
        }
        else
        {
            string_t buf;
            string_init(buf);
            cace_ari_text_encode(buf, &meta->dest, CACE_ARI_TEXT_ENC_OPTS_DEFAULT);
            CACE_LOG_ERR("This transport can only send to text URI destinations, not %s", m_string_get_cstr(buf));
            string_clear(buf);

            return 6;
        }
    }

    Sdr sdr = bp_get_sdr();
    if (sdr_begin_xn(sdr) < 0)
    {
        CACE_LOG_ERR("Unable to start transaction");
        return 2;
    }

    int retval = 0;
    CACE_LOG_DEBUG("Sending message with %d ARIs", cace_ari_list_size(data));

    // FIXME a copy could be avoided if this encoder wrote to SDR directly
    m_bstring_t msgbuf;
    m_bstring_init(msgbuf);
    if (cace_amp_msg_encode(msgbuf, data))
    {
        retval = 3;
    }
    const size_t   msg_size  = m_bstring_size(msgbuf);
    const uint8_t *msg_begin = m_bstring_view(msgbuf, 0, msg_size);

    Object extent;
    if (!retval)
    {
        extent = sdr_malloc(sdr, msg_size);
        if (extent)
        {
            sdr_write(sdr, extent, (char *)msg_begin, msg_size);
        }
        else
        {
            CACE_LOG_ERR("Can't write to SDR object");
            retval = 4;
        }
    }
    if (sdr_end_xn(sdr) < 0)
    {
        CACE_LOG_ERR("Can't close transaction");
    }

    Object content;
    if (!retval)
    {
        content = ionCreateZco(ZcoSdrSource, extent, 0, msg_size, BP_STD_PRIORITY, 0, ZcoOutbound, NULL);
        if (content == 0 || content == (Object)ERROR)
        {
            CACE_LOG_ERR("Zero-Copy Object creation failed");
            retval = 5;
        }
    }

    if (!retval)
    {
        CACE_LOG_DEBUG("sending datagram with %zd octets to %s", msg_size, dest_eid);
        Object newBundle = 0;

        int res = bp_send(state->sap,
                          dest_eid,           // recipient
                          NULL,               // report-to
                          300,                // lifetime in seconds
                          BP_STD_PRIORITY,    // Class-of-Service / Priority
                          NoCustodyRequested, // Custody Switch
                          0,                  // SRR Flags
                          0,                  // ACK Requested
                          NULL,               // Extended COS
                          content,            // ADU
                          &newBundle          // New Bundle
        );
        if (res != 1)
        {
            CACE_LOG_ERR("Send failed (%d) to %s", res, dest_eid);
            retval = 7;
        }
    }
    return retval;
}

int cace_amp_ion_bp_recv(cace_ari_list_t data, cace_amm_msg_if_metadata_t *meta, cace_daemon_run_t *running, void *ctx)
{
    CHKERR1(data);
    CHKERR1(meta);
    CHKERR1(running);
    cace_amp_ion_bp_state_t *state = ctx;
    CHKERR1(state);

    Sdr sdr = bp_get_sdr();
    int res;

    int retval = 0;

    BpDelivery dlv;
    memset(&dlv, 0, sizeof(BpDelivery));

    while (!retval && (dlv.result != BpPayloadPresent))
    {
        // FIXME Timeout is required to check agent running status periodically
        static const int timeout = 5;
        if ((res = bp_receive(state->sap, &dlv, timeout)) < 0)
        {
            CACE_LOG_ERR("bp_receive failed, result: %d.", res);
            retval = 3;
        }
        switch (dlv.result)
        {
            case BpEndpointStopped:
                /* The endpoint stopped? Panic.*/
                CACE_LOG_INFO("Endpoint stopped");
                retval = CACE_AMM_MSG_IF_RECV_END;
                break;

            case BpPayloadPresent:
                /* Clear to process the payload. */
                CACE_LOG_INFO("Payload present");
                break;

            default:
                // nothing ready, but maybe daemon is shutting down
                if (!cace_daemon_run_get(running))
                {
                    CACE_LOG_DEBUG("returning due to running state change");
                    retval = CACE_AMM_MSG_IF_RECV_END;
                    break;
                }
                continue;
        }
    }

    if (!retval)
    {
        if (sdr_begin_xn(sdr) < 0)
        {
            CACE_LOG_ERR("sdr_begin_xn failed");
            retval = 4;
        }
    }

    m_bstring_t msgbuf;
    m_bstring_init(msgbuf);
    if (!retval)
    {
        cace_ari_set_tstr(&meta->src, dlv.bundleSourceEid, true);

        const vast adu_len = zco_source_data_length(sdr, dlv.adu);
        if (adu_len)
        {
            m_bstring_resize(msgbuf, adu_len);
            uint8_t *msg_begin = m_bstring_acquire_access(msgbuf, 0, adu_len);

            ZcoReader reader;
            zco_start_receiving(dlv.adu, &reader);
            vast got = zco_receive_source(sdr, &reader, adu_len, (char *)msg_begin);
            m_bstring_release_access(msgbuf);
            if (got != adu_len)
            {
                CACE_LOG_ERR("zco_receive_source() wanted %ll but got %ll", adu_len, got);
                // continue on
            }
            CACE_LOG_DEBUG("read ADU with %zd octets from %s", got, dlv.bundleSourceEid);
        }

        if (sdr_end_xn(sdr) < 0)
        {
            CACE_LOG_ERR("sdr_end_xn() failed");
            retval = 5;
        }
    }

    if (!retval)
    {
        const size_t   msgbuf_len = m_bstring_size(msgbuf);
        const uint8_t *msgbuf_ptr = m_bstring_view(msgbuf, 0, msgbuf_len);
        if (cace_amp_msg_decode(data, msgbuf_ptr, msgbuf_len))
        {
            retval = 6;
        }
        CACE_LOG_DEBUG("decoded %d ARI items in the datagram", cace_ari_list_size(data));
    }
    m_bstring_clear(msgbuf);

    bp_release_delivery(&dlv, 1);

    return retval;
}

#endif /* ION_FOUND */
