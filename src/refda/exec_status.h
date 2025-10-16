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

#ifndef REFDA_EXEC_STATUS_H_
#define REFDA_EXEC_STATUS_H_

#include <m-atomic.h>
#include <semaphore.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
    /// A semaphore which has a value when the target is finished executing
    sem_t finished;
    /// Once #finished has a value, this is an indicator that a CTRL failed
    atomic_bool failed;
} refda_exec_status_t;

#include <cace/util/defs.h>

static inline void refda_exec_status_init(refda_exec_status_t *obj)
{
    CHKVOID(obj);
    sem_init(&obj->finished, 0, 0);
    atomic_store(&obj->failed, false);
}

static inline void refda_exec_status_deinit(refda_exec_status_t *obj)
{
    CHKVOID(obj);
    sem_destroy(&obj->finished);
}

/** Wait for the finished semaphore and take it.
 *
 * @param[in] obj The status to wait on.
 * @return True if there was a failure.
 */
static inline bool refda_exec_status_wait(refda_exec_status_t *obj)
{
    CHKRET(obj, true);
    sem_wait(&obj->finished);
    return atomic_load(&obj->failed);
}

#ifdef __cplusplus
} // extern C
#endif

#endif /* REFDA_EXEC_STATUS_H_ */
