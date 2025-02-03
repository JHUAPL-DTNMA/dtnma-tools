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
#ifndef CACE_UTIL_DAEMON_RUN_H_
#define CACE_UTIL_DAEMON_RUN_H_

#include <stdbool.h>
#include <semaphore.h>

#ifdef __cplusplus
extern "C" {
#endif

/** State for daemon run/stop signalling.
 */
typedef struct daemon_run
{
    sem_t stop;
} cace_daemon_run_t;

int  cace_daemon_run_init(cace_daemon_run_t *dr);
void cace_daemon_run_cleanup(cace_daemon_run_t *dr);
void cace_daemon_run_stop(cace_daemon_run_t *dr);
bool cace_daemon_run_get(cace_daemon_run_t *dr);

/** Wait until the running state is stopped.
 * @param dr The run object.
 * @return True if successful.
 */
bool cace_daemon_run_wait(cace_daemon_run_t *dr);

#ifdef __cplusplus
}
#endif

#endif /* CACE_UTIL_DAEMON_RUN_H_ */
