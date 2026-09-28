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
/** @file
 * @ingroup group_cace_util
 * Definitions for managing POSIX worker threads within a process.
 */
#ifndef CACE_UTIL_THREADSET_H_
#define CACE_UTIL_THREADSET_H_

#include "cace/config.h"

#include <m-list.h>

#include <pthread.h>

/** @struct cace_threadset_t 
* A list of thread handles used for work threads.
* @sa cace_threadset_start(), cace_threadset_join()
*/
/// @cond Doxygen_Suppress
// GCOV_EXCL_START
M_LIST_DEF(cace_threadset, pthread_t)
// GCOV_EXCL_STOP
/// @endcond

/**
 * A thread descriptor used to spawn and join work threads.
 */
typedef struct
{
    /** The actual work function compatible with POSIX thread APIs.
     */
    void *(*func)(void *);
    /// The distinct short name to associate with the thread instance
    const char *name;
} cace_threadinfo_t;

/** Start a set of work threads.
 */
int cace_threadset_start(cace_threadset_t tset, const cace_threadinfo_t *info, size_t count, void *arg);

/** Join a set of work threads.
 * The thread work functions must be returned from by separate control.
 */
int cace_threadset_join(cace_threadset_t tset);

#endif /* CACE_UTIL_THREADSET_H_ */
