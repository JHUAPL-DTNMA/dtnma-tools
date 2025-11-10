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
#include "macrofile.h"
#include "text.h"
#include "cace/util/logging.h"
#include "cace/util/defs.h"

#if defined(ARI_TEXT_PARSE)

int cace_ari_macrofile_read(FILE *file, cace_ari_list_t items)
{
    CHKERR1(file);
    CHKERR1(items);

    size_t lineno = 0;
    while (true)
    {
        // assume that if something is ready to read that a whole line will come
        char  *lineptr   = NULL;
        size_t linealloc = 0;
        char  *errm      = NULL;

        int res = getline(&lineptr, &linealloc, file);
        if (res < 0)
        {
            CACE_LOG_DEBUG("returning due to end of input %d", res);
            free(lineptr);
            break;
        }

        ++lineno;
        CACE_LOG_DEBUG("read line %zu with %zd characters", lineno, res);
        if ((res > 0) && (lineptr[0] == '#'))
        {
            free(lineptr);
            continue;
        }

        cace_ari_t *item = cace_ari_list_push_back_new(items);
        if (cace_ari_text_decode_cstr(item, lineptr, res + 1, &errm))
        {
            CACE_LOG_ERR("Failed decoding one startup item on line %zu: %s", lineno, errm);
            CACE_FREE(errm);

            cace_ari_list_pop_back(NULL, items);
        }
        free(lineptr);
    }
    return 0;
}

#endif /* ARI_TEXT_PARSE */
