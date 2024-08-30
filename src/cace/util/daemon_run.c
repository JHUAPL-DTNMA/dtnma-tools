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
#include "daemon_run.h"
#include "shared/platform.h"
#include "shared/utils/debug.h"

/**
 * @brief daemon_run_init Initialize the daemon run object.
 * @param dr The object to initialize.
 * @return Zero if successful, nonzero otherwise.
 */
int daemon_run_init(daemon_run_t *dr)
{
  int ret = sem_init(&(dr->stop), 0, 0);
  if (ret)
  {
    AMP_DEBUG_ERR("daemon_run_init", "Failed to create mutex");
    return 1;
  }
  daemon_run_get(dr);
  return 0;
}

/**
 * @brief daemon_run_cleanup Deinitialize the object.
 * @param dr The object to inspect.
 */
void daemon_run_cleanup(daemon_run_t *dr)
{
  if (sem_destroy(&(dr->stop)))
  {
    AMP_DEBUG_ERR("daemon_run_cleanup", "Failed delete mutex");
  }
}

/**
 * @brief daemon_run_stop Signal the daemon to stop running.
 * @param dr The object to inspect.
 */
void daemon_run_stop(daemon_run_t *dr)
{
  AMP_DEBUG_INFO("daemon_run_stop", "Stopping daemon");
  if (sem_post(&(dr->stop)))
  {
    AMP_DEBUG_ERR("daemon_run_stop", "Failed give mutex");
  }
}

/**
 * @brief daemon_run_get Determine the daemon running status.
 * This function is thread safe.
 * @param dr The object to inspect.
 * @return True if the daemon continues to run.
 */
bool daemon_run_get(daemon_run_t *dr)
{
  int val;
  if (sem_getvalue(&(dr->stop), &val))
  {
    AMP_DEBUG_ERR("daemon_run_get", "Failed check mutex");
    return false;
  }
  AMP_DEBUG_INFO("daemon_run_get", "Value for %p: %d", dr, val);
  return val == 0;
}

bool daemon_run_wait(daemon_run_t *dr)
{
  if (sem_wait(&(dr->stop)))
  {
    AMP_DEBUG_ERR("daemon_run_get", "Failed check mutex");
    return false;
  }
  return true;
}
