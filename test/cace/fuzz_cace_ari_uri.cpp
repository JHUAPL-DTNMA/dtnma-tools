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
#include "fuzz_common.h"
#include <cace/config.h>
#include <cace/ari.h>
#include <cace/ari/text.h>
#include <cace/ari/text_util.h>
#include <cace/util/logging.h>
#include <cace/util/defs.h>
#include <qcbor/qcbor_decode.h>
#include <m-bstring.h>
#include <stdio.h>

extern "C" int LLVMFuzzerInitialize(int *argc _U_, char ***argv _U_)
{
    cace_openlog();
    cace_log_set_least_severity(LOG_DEBUG);
    return 0;
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    int retval = 0;

    m_bstring_t buf;
    m_bstring_init(buf);
    if (size)
    {
        m_bstring_push_back_bytes(buf, size, data);
    }
    m_bstring_push_back(buf, '\0');

    size_t      buf_size = m_bstring_size(buf);
    const char *buf_ptr  = (const char *)m_bstring_view(buf, 0, buf_size);

    if (!m_str1ng_utf8_valid_str_p(buf_ptr))
    {
        m_bstring_clear(buf);
        return -1;
    }

    FILE *tmp = fmemopen((void *)buf_ptr, buf_size, "rb");
    if (!tmp)
    {
        return 2;
    }

    while (!retval)
    {
        char  *line_buf = NULL;
        size_t line_len = 0;
        int    res      = getline(&line_buf, &line_len, tmp);
        if (res < 0)
        {
            // end of file
            free(line_buf);
            break;
        }

        m_string_t in_text;
        m_string_init_set_cstr(in_text, line_buf);
        free(line_buf);

        cace_ari_t val = CACE_ARI_INIT_UNDEFINED;
        {
            char *errm = NULL;
            int   res  = cace_ari_text_decode(&val, in_text, &errm);
            if (res)
            {
                CACE_LOG_ERR("Failure: %s", errm);
                CACE_FREE(errm);
                retval = -1;
            }
        }

        m_string_t out_text;
        m_string_init(out_text);
        if (!retval)
        {
            cace_ari_text_enc_opts_t opts = CACE_ARI_TEXT_ENC_OPTS_DEFAULT;

            int res = cace_ari_text_encode(out_text, &val, opts);
            EXPECT_EQ(0, res);
        }

        // no test assertions here becuase there are man options that change
        // the URI text form

        m_string_clear(out_text);
        cace_ari_deinit(&val);
        m_string_clear(in_text);
    }

    fclose(tmp);
    m_bstring_clear(buf);

    return retval;
}
