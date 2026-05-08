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
 * @ingroup cace
 * This file contains preprocessor utility definitions shared among the
 * CACE library.
 */
#ifndef CACE_MUTEX_H_
#define CACE_MUTEX_H_

#include "logging.h"
#include "defs.h"

#ifdef __cplusplus
extern "C" {
#endif

/** Lock a mutex and terminate if failed.
 * @warning This macro is internal.
 * @param func The locking function
 * @param mutex A pointer to the mutex
 */
#define CACE_INTERNAL_LOCK(func, mutex)                                     \
    do                                                                      \
    {                                                                       \
        CACE_LOG_DEBUG("locking mutex %s", #mutex);                         \
        int res = (func)((mutex));                                          \
        if (UNLIKELY(res != 0))                                             \
        {                                                                   \
            fprintf(stderr, "failed to lock mutex %s got %d", #mutex, res); \
            assert(res == 0);                                               \
        }                                                                   \
    }                                                                       \
    while (false)

/** Unlock a mutex and terminate if failed.
 * @warning This macro is internal.
 * @param func The unlocking function
 * @param mutex A pointer to the mutex
 */
#define CACE_INTERNAL_UNLOCK(func, mutex)                                     \
    do                                                                        \
    {                                                                         \
        CACE_LOG_DEBUG("unlocking mutex %s", #mutex);                         \
        int res = (func)((mutex));                                            \
        if (UNLIKELY(res != 0))                                               \
        {                                                                     \
            fprintf(stderr, "failed to unlock mutex %s got %d", #mutex, res); \
            assert(res == 0);                                                 \
        }                                                                     \
    }                                                                         \
    while (false)

/** Lock a mutex and terminate if failed.
 */
#define CACE_MUTEX_LOCK(mutex) CACE_INTERNAL_LOCK(pthread_mutex_lock, mutex)

/** Unlock a mutex and terminate if failed.
 */
#define CACE_MUTEX_UNLOCK(mutex) CACE_INTERNAL_UNLOCK(pthread_mutex_unlock, mutex)

#ifdef __cplusplus
} // extern C
#endif

#endif /* CACE_MUTEX_H_ */
