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
#include "daemon_run.h"
#include "logging.h"
#include <errno.h>

/**
 * @brief daemon_run_init Initialize the daemon run object.
 * @param dr The object to initialize.
 * @return Zero if successful, nonzero otherwise.
 */
int cace_daemon_run_init(cace_daemon_run_t *dr)
{
    int ret = sem_init(&(dr->stop), 0, 0);
    if (ret)
    {
        CACE_LOG_ERR("Failed to create mutex: %d", errno);
        return 1;
    }
    cace_daemon_run_get(dr);
    return 0;
}

/**
 * @brief daemon_run_cleanup Deinitialize the object.
 * @param dr The object to inspect.
 */
void cace_daemon_run_cleanup(cace_daemon_run_t *dr)
{
    if (sem_destroy(&(dr->stop)))
    {
        CACE_LOG_ERR("Failed delete mutex: %d", errno);
    }
}

/**
 * @brief daemon_run_stop Signal the daemon to stop running.
 * @param dr The object to inspect.
 */
void cace_daemon_run_stop(cace_daemon_run_t *dr)
{
    CACE_LOG_INFO("Stopping daemon");
    if (sem_post(&(dr->stop)))
    {
        CACE_LOG_ERR("Failed give mutex: %d", errno);
    }
}

/**
 * @brief daemon_run_get Determine the daemon running status.
 * This function is thread safe.
 * @param dr The object to inspect.
 * @return True if the daemon continues to run.
 */
bool cace_daemon_run_get(cace_daemon_run_t *dr)
{
    int val;
    if (sem_getvalue(&(dr->stop), &val))
    {
        CACE_LOG_ERR("Failed in get: %d", errno);
        return false;
    }
#if 0
    CACE_LOG_DEBUG("Value for %p: %d", dr, val);
#endif
    return val == 0;
}

bool cace_daemon_run_wait(cace_daemon_run_t *dr)
{
    if (sem_wait(&(dr->stop)))
    {
        if (errno != EINTR)
        {
            CACE_LOG_ERR("Failed in wait: %d", errno);
            return false;
        }
    }
    return true;
}
