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
#include "reporting.h"
#include "cace/util/defs.h"
#include "cace/util/logging.h"

int refda_reporting_ctrl(refda_runctx_t *runctx, const ari_t *target, ari_t *result)
{
    ari_t rptset;

    ari_rptset_t *rpts = ari_init_rptset(&rptset);
    ari_set_copy(&(rpts->nonce), runctx->nonce);
    refda_agent_nowtime(runctx->agent, &(rpts->reftime));
    {
        ari_report_t *rpt = ari_report_list_push_back_new(rpts->reports);
        ari_set_td(&(**rpt).reltime, (struct timespec) { .tv_sec = 0 });
        ari_set_copy(&(**rpt).source, target);

        ari_t *item = ari_list_push_back_new((**rpt).items);
        ari_set_copy(item, result);
    }
    CACE_LOG_DEBUG("generated an execution report");

    agent_ari_queue_push_move(runctx->agent->rptgs, &rptset);
    sem_post(&(runctx->agent->rptgs_sem));

    return 0;
}

int refda_reporting_target(refda_agent_t *agent, const ari_t *ari)
{
    return 1;
}
