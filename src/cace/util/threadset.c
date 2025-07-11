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

#include "threadset.h"
#include "logging.h"
#include "defs.h"
#include <errno.h>

int cace_threadset_start(cace_threadset_t tset, const cace_threadinfo_t *info, size_t count, void *arg)
{
    for (const cace_threadinfo_t *it = info; it < info + count; ++it)
    {
        if (!(it->func))
        {
            continue;
        }
        pthread_t thr;
        int       res = pthread_create(&thr, NULL, it->func, arg);
        if (res)
        {
            CACE_LOG_ERR("Unable to create pthread %s, errno = %s", it->name, strerror(errno));
            return 2;
        }
        cace_threadset_push_back(tset, thr);
        CACE_LOG_INFO("Started thread %s", it->name);

#ifdef _GNU_SOURCE
        if (it->name)
        {
            pthread_setname_np(thr, it->name);
        }
#endif /* _GNU_SOURCE */
    }

    return 0;
}

int cace_threadset_join(cace_threadset_t tset)
{
    while (!cace_threadset_empty_p(tset))
    {
        pthread_t thr;
        cace_threadset_pop_back(&thr, tset);

        if (pthread_join(thr, NULL))
        {
            CACE_LOG_ERR("Unable to join pthread %s, errno = %s", "name", strerror(errno));
            return 2;
        }
    }
    return 0;
}
