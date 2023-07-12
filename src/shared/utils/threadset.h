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

#ifndef SRC_SHARED_UTILS_THREADSET_H_
#define SRC_SHARED_UTILS_THREADSET_H_

#include <pthread.h>
#include <m-list.h>

LIST_DEF(list_thread, pthread_t)

typedef struct {
  void* (*func)(void*);
  const char *name;
} threadinfo_t;

int threadset_start(list_thread_t *tset, const threadinfo_t *info, size_t count, void *arg);

int threadset_join(list_thread_t *tset);


#endif /* SRC_SHARED_UTILS_THREADSET_H_ */
