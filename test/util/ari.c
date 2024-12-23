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

#include "ari.h"
#include <cace/ari/text_util.h>
#include <cace/ari/cbor.h>
#include <cace/util/logging.h>
#include <cace/util/defs.h>

int test_util_ari_decode(ari_t *ari, const char *inhex)
{
    CHKERR1(ari);
    CHKERR1(inhex);

    string_t intext;
    string_init_set_str(intext, inhex);
    cace_data_t indata;
    cace_data_init(&indata);
    int res = base16_decode(&indata, intext);
    string_clear(intext);
    if (res)
    {
        CACE_LOG_ERR("base16_decode() failed");
        return 2;
    }

    res = ari_cbor_decode(ari, &indata, NULL, NULL);
    cace_data_deinit(&indata);
    if (res)
    {
        CACE_LOG_ERR("ari_cbor_decode() failed");
        return 3;
    }
    return 0;
}

int test_util_ari_encode(m_string_t outhex, const ari_t *ari)
{
    CHKERR1(outhex);
    CHKERR1(ari);
    m_string_init(outhex);

    cace_data_t outdata;
    cace_data_init(&outdata);
    int res = ari_cbor_encode(&outdata, ari);
    if (res)
    {
        CACE_LOG_ERR("ari_cbor_encode() failed");
        cace_data_deinit(&outdata);
        return 2;
    }

    res = base16_encode(outhex, &outdata, true);
    cace_data_deinit(&outdata);
    if (res)
    {
        CACE_LOG_ERR("base16_decode() failed");
        return 2;
    }

    return 0;
}
