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
#include "timeline.h"
#include <timespec.h>

int refda_timeline_event_cmp(const refda_timeline_event_t *lt, const refda_timeline_event_t *rt)
{
    if (!lt)
    {
        return 1;
    }
    if (!rt)
    {
        return -1;
    }
    return timespec_cmp(lt->ts, rt->ts);
}

const refda_timeline_event_t *refda_timeline_front(refda_timeline_t line)
{
    refda_timeline_it_t it;
    refda_timeline_it(it, line);
    if (refda_timeline_end_p(it))
    {
        return NULL;
    }
    return refda_timeline_cref(it);
}
