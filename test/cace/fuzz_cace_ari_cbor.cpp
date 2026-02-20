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
#include "fuzz_common.h"
#include <cace/config.h>
#include <cace/ari.h>
#include <cace/ari/text.h>
#include <cace/ari/cbor.h>
#include <cace/ari/text_util.h>
#include <cace/util/logging.h>
#include <cace/util/defs.h>
#include <qcbor/qcbor_decode.h>

extern "C" int LLVMFuzzerInitialize(int *argc _U_, char ***argv _U_)
{
    cace_openlog();
    cace_log_set_least_severity(LOG_CRIT);
    return 0;
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    int retval = 0;

    cace_data_t in_data;
    cace_data_init_view(&in_data, size, (cace_data_ptr_t)data);

    cace_ari_t val = CACE_ARI_INIT_UNDEFINED;
    {
        size_t used;
        char  *errm = NULL;
        int res         = cace_ari_cbor_decode(&val, &in_data, &used, &errm);
        if (res)
        {
            CACE_LOG_ERR("Failure: %s", errm);
            CACE_FREE(errm);
            retval = -1;
        }
        if (used != size)
        {
            retval = -1;
        }
    }

    cace_data_t out_data;
    cace_data_init(&out_data);
    if (!retval)
    {
        int res = cace_ari_cbor_encode(&out_data, &val);
        EXPECT_EQ(0, res);
    }

    if (!retval)
    {
        // output may be a subset
        // CBOR tags on input will not be carried
        if (size >= out_data.len)
        {
            if (0 != ::memcmp(data, out_data.ptr, out_data.len))
            {
                retval = -1;
            }
        }
    }

    cace_data_deinit(&out_data);
    cace_ari_deinit(&val);
    cace_data_deinit(&in_data);
    return retval;
}
