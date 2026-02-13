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
 * This just verifies the ability for a C++11 executable to link with the
 * refda library.
 */
#include <refda/agent.h>
#include <refda/adm/ietf_amm.h>
#include <refda/adm/ietf_amm_base.h>
#include <refda/adm/ietf_amm_semtype.h>
#include <refda/adm/ietf_network_base.h>
#include <refda/adm/ietf_dtnma_agent.h>
#include <cace/util/logging.h>
#include <cace/util/defs.h>
#include <iostream>

/// Per-process state
static refda_agent_t agent;

static int dummy_send(const cace_ari_list_t data, const cace_amm_msg_if_metadata_t *meta, void *ctx _U_)
{
    CHKERR1(data);
    CHKERR1(meta);
    CACE_LOG_DEBUG("Sending message with %d ARIs", cace_ari_list_size(data));
    return 0;
}

static int dummy_recv(cace_ari_list_t data, cace_amm_msg_if_metadata_t *meta, cace_daemon_run_t *running, void *ctx _U_)
{
    CHKERR1(data);
    CHKERR1(meta);
    CHKERR1(running);
    CACE_LOG_DEBUG("returning due to hangup");
    return 2;
}

int main(int argc _U_, char *argv[] _U_)
{
    // keep track of failure state
    int retval = 0;

    cace_openlog();
    refda_agent_init(&agent);

    int log_limit = LOG_INFO;
    cace_log_set_least_severity(log_limit);
    CACE_LOG_DEBUG("Agent starting up with log limit %d", log_limit);

    agent.mif.send = dummy_send;
    agent.mif.recv = dummy_recv;

    // ADM initialization
    refda_adm_ietf_amm_init(&agent);
    refda_adm_ietf_amm_base_init(&agent);
    refda_adm_ietf_amm_semtype_init(&agent);
    refda_adm_ietf_network_base_init(&agent);
    refda_adm_ietf_dtnma_agent_init(&agent);

    /* Start agent threads. */
    if (!retval)
    {
        int failures = refda_agent_bindrefs(&agent);
        if (failures)
        {
            // Warn but continue on
            CACE_LOG_ERR("ADM reference binding failed for %d type references", failures);
            retval = 2;
        }
        else
        {
            CACE_LOG_INFO("ADM reference binding succeeded");
        }
    }
    if (!retval)
    {
        if (refda_agent_init_objs(&agent))
        {
            CACE_LOG_ERR("Agent object initialization failed");
            retval = 2;
        }
        else
        {
            CACE_LOG_INFO("Agent object initialization completed");
        }
    }
    if (!retval)
    {
        if (refda_agent_start(&agent))
        {
            CACE_LOG_ERR("Agent startup failed");
            retval = 2;
        }
        else
        {
            CACE_LOG_INFO("Agent startup completed");
        }
    }

    CACE_LOG_INFO("READY");
    refda_agent_enable_exec(&agent);

    if (!retval)
    {
        // Block until stopped
        cace_daemon_run_wait(&agent.running);
    }

    CACE_LOG_INFO("Agent is shutting down");

    /* Join threads and wait for them to complete. */
    if (!retval)
    {
        if (refda_agent_stop(&agent))
        {
            CACE_LOG_ERR("Agent stop failed");
            retval = 4;
        }
        else
        {
            CACE_LOG_INFO("Agent stopped");
        }
    }

    /* Cleanup. */
    CACE_LOG_DEBUG("Cleaning Agent Resources");
    refda_agent_deinit(&agent);

    CACE_LOG_DEBUG("Agent shutdown completed");
    cace_closelog();
    return retval;
}
