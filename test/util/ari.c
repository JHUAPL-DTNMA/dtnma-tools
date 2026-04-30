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

#include "ari.h"
#include <cace/ari/text_util.h>
#include <cace/ari/text.h>
#include <cace/ari/cbor.h>
#include <cace/util/logging.h>
#include <cace/util/defs.h>

int test_util_ari_decode(cace_ari_t *ari, const char *inhex)
{
    CHKERR1(ari);
    CHKERR1(inhex);

    m_string_t intext;
    m_string_init_set_cstr(intext, inhex);
    cace_data_t indata;
    cace_data_init(&indata);
    int res = cace_base16_decode(&indata, intext);
    m_string_clear(intext);
    if (res)
    {
        CACE_LOG_ERR("cace_base16_decode() failed");
        return 2;
    }

    res = cace_ari_cbor_decode(ari, &indata, NULL, NULL);
    cace_data_deinit(&indata);
    if (res)
    {
        CACE_LOG_ERR("cace_ari_cbor_decode() failed");
        return 3;
    }
    return 0;
}

int test_util_ari_encode(m_string_t outhex, const cace_ari_t *ari)
{
    CHKERR1(outhex);
    CHKERR1(ari);
    m_string_init(outhex);

    cace_data_t outdata;
    cace_data_init(&outdata);
    int res = cace_ari_cbor_encode(&outdata, ari);
    if (res)
    {
        CACE_LOG_ERR("ari_cbor_encode() failed");
        cace_data_deinit(&outdata);
        return 2;
    }

    res = cace_base16_encode(outhex, &outdata, true);
    cace_data_deinit(&outdata);
    if (res)
    {
        CACE_LOG_ERR("cace_base16_decode() failed");
        return 2;
    }

    return 0;
}

bool test_util_ari_equal(const cace_ari_t *expect, const cace_ari_t *result)
{
    const bool equal = cace_ari_equal(expect, result);
    if (!equal)
    {
        {
            m_string_t bufe;
            m_string_init(bufe);
            cace_ari_text_encode(bufe, expect, CACE_ARI_TEXT_ENC_OPTS_DEFAULT);

            m_string_t buf;
            m_string_init(buf);
            cace_ari_text_encode(buf, result, CACE_ARI_TEXT_ENC_OPTS_DEFAULT);
            printf("expected %s got %s\n", m_string_get_cstr(bufe), m_string_get_cstr(buf));
            m_string_clear(buf);
            m_string_clear(bufe);
        }
        {
            m_string_t buf;
            m_string_init(buf);
            test_util_ari_encode(buf, result);
            printf("got value as cborhex %s\n", m_string_get_cstr(buf));
            m_string_clear(buf);
        }
    }
    return equal;
}
