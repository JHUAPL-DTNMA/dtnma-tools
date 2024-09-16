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
#include "valprod.h"
#include "cace/ari/text.h"
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

/** Report on an arbitrary object reference.
 */
static int refda_reporting_ref(refda_runctx_t *runctx, const ari_t *target)
{
    int retval = 0;

    cace_amm_lookup_t deref;
    cace_amm_lookup_init(&deref);

    int res = cace_amm_lookup_deref(&deref, &(runctx->agent->objs), target);
    CACE_LOG_DEBUG("Lookup result %d", res);
    if (res)
    {
        retval = REFDA_REPORTING_ERR_DEREF_FAILED;
    }

    if (!retval)
    {
        switch (target->as_ref.objpath.ari_type)
        {
            case ARI_TYPE_CONST:
            case ARI_TYPE_VAR:
            case ARI_TYPE_EDD:
            {
                refda_valprod_ctx_t prodctx;
                refda_valprod_ctx_init(&prodctx, runctx, &deref);
                retval = refda_valprod_run(&prodctx);
                if (!retval)
                {
                    // execute the produced value as a target
                    retval = refda_reporting_target(runctx, &(prodctx.value));
                }
                refda_valprod_ctx_deinit(&prodctx);
                break;
            }
            default:
                retval = REFDA_REPORTING_ERR_BAD_TYPE;
                break;
        }
    }

    cace_amm_lookup_deinit(&deref);

    return retval;
}

int refda_reporting_target(refda_runctx_t *runctx, const ari_t *target)
{
    CHKERR1(runctx);
    CHKERR1(target);

    if (cace_log_is_enabled_for(LOG_DEBUG))
    {
        string_t buf;
        string_init(buf);
        ari_text_encode(buf, target, ARI_TEXT_ENC_OPTS_DEFAULT);
        CACE_LOG_DEBUG("Reporting on target %s", string_get_cstr(buf));
        string_clear(buf);
    }

    int retval = 0;
    if (target->is_ref)
    {
        CACE_LOG_DEBUG("Reporting on reference");
        retval = refda_reporting_ref(runctx, target);
    }
    else
    {
        if (!amm_type_match(runctx->agent->mac_type, target) && false) // FIXME need to implement typedefs
        {
            CACE_LOG_WARNING("Attempted reporting on a non-RPTT literal");
            retval = REFDA_REPORTING_ERR_BAD_TYPE;
        }
        else
        {
            CACE_LOG_DEBUG("Reporting on RPTT");
            //            retval = refda_reporting_rptt(runctx, target);
        }
    }

    ari_t rptset;

    ari_rptset_t *rpts = ari_init_rptset(&rptset);
    ari_set_null(&(rpts->nonce));
    refda_agent_nowtime(runctx->agent, &(rpts->reftime));
    {
        ari_report_t *rpt = ari_report_list_push_back_new(rpts->reports);
        ari_set_td(&(**rpt).reltime, (struct timespec) { .tv_sec = 0 });
        ari_set_copy(&(**rpt).source, target);

        ari_t *item = ari_list_push_back_new((**rpt).items);
        //        ari_set_copy(item, result);
    }
    CACE_LOG_DEBUG("generated an execution report");

    agent_ari_queue_push_move(runctx->agent->rptgs, &rptset);
    sem_post(&(runctx->agent->rptgs_sem));
    return 0;
}
