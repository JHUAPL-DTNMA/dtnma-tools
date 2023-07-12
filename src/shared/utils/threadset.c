/*
 * Copyright (c) 2023 The Johns Hopkins University Applied Physics
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

#include <errno.h>
#include "threadset.h"
#include "debug.h"
#include "utils.h"

int threadset_start(list_thread_t *tset, const threadinfo_t *info, size_t count, void *arg)
{
  for (const threadinfo_t *it = info; it < info + count; ++it)
  {
    if (!(it->func))
    {
      continue;
    }
    pthread_t thr;
    int res = pthread_create(&thr, NULL, it->func, arg);
    if (res)
    {
      AMP_DEBUG_ERR("threadset_start","Unable to create pthread %s, errno = %s",
                    it->name, strerror(errno));
      return AMP_SYSERR;
    }
    list_thread_push_back(*tset, thr);
    AMP_DEBUG_INFO("threadset_start", "Started thread %s", it->name);

    if (it->name)
    {
      pthread_setname_np(thr, it->name);
    }
  }

  return AMP_OK;
}

int threadset_join(list_thread_t *tset)
{
  while (!list_thread_empty_p(*tset))
  {
    pthread_t thr;
    list_thread_pop_back(&thr, *tset);
    if (pthread_join(thr, NULL))
    {
        AMP_DEBUG_ERR("threadset_join","Unable to join pthread %s, errno = %s",
                      "name", strerror(errno));
        return AMP_SYSERR;
    }
  }
  return AMP_OK;
}
