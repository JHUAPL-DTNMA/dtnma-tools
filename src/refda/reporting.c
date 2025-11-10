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
#include "reporting.h"
#include "reporting_ctx.h"
#include "valprod.h"
#include "eval.h"
#include "cace/ari/text.h"
#include "cace/util/defs.h"
#include "cace/util/logging.h"

int refda_reporting_ctrl(refda_runctx_t *runctx, const cace_ari_t *target, cace_ari_t *result)
{
    if (cace_ari_is_undefined(&runctx->mgr_ident))
    {
        // nothing to do
        return 0;
    }

    refda_msgdata_t msg;
    refda_msgdata_init(&msg);
    cace_ari_set_copy(&msg.ident, &runctx->mgr_ident);

    cace_ari_rptset_t *rpts = cace_ari_set_rptset(&msg.value);
    cace_ari_set_copy(&(rpts->nonce), &(runctx->nonce));
    refda_agent_nowtime(runctx->agent, &(rpts->reftime));
    {
        cace_ari_report_t *rpt = cace_ari_report_list_push_back_new(rpts->reports);
        cace_ari_set_td(&(rpt->reltime), (struct timespec) { .tv_sec = 0 });
        cace_ari_set_copy(&(rpt->source), target);

        cace_ari_list_push_back_move(rpt->items, result);
    }
    CACE_LOG_DEBUG("generated an execution report");

    refda_msgdata_queue_push_move(runctx->agent->rptgs, &msg);
    sem_post(&(runctx->agent->rptgs_sem));

    return 0;
}

/** Treat any object reference as a value-producing activity, with the
 * produced value reported on directly.
 */
static int refda_reporting_item_ref(refda_runctx_t *runctx, cace_ari_t *prod_val, const cace_ari_t *rptt_item)
{
    CACE_LOG_DEBUG("Reporting on item reference");
    int retval = 0;

    cace_amm_lookup_t deref;
    cace_amm_lookup_init(&deref);

    int res = cace_amm_lookup_deref(&deref, &(runctx->agent->objs), rptt_item);
    CACE_LOG_DEBUG("Lookup result %d", res);
    if (res)
    {
        retval = REFDA_REPORTING_ERR_DEREF_FAILED;
    }

    if (!retval)
    {
        switch (deref.obj_type)
        {
            case CACE_ARI_TYPE_CONST:
            case CACE_ARI_TYPE_VAR:
            case CACE_ARI_TYPE_EDD:
            {
                refda_valprod_ctx_t prodctx;
                refda_valprod_ctx_init(&prodctx, runctx, rptt_item, &deref);
                retval = refda_valprod_run(&prodctx);
                if (!retval)
                {
                    // include the produced value directly
                    cace_ari_set_move(prod_val, &(prodctx.value));
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

/** Actually iterate through an RPTT and produce items.
 */
static int refda_reporting_rptt_val(refda_reporting_ctx_t *rptctx, const cace_ari_t *value)
{
    const cace_ari_ac_t *tgt_ac = cace_ari_cget_ac(value);
    CHKERR1(tgt_ac);

    cace_ari_list_it_t rptt_it;
    for (cace_ari_list_it(rptt_it, tgt_ac->items); !cace_ari_list_end_p(rptt_it); cace_ari_list_next(rptt_it))
    {
        const cace_ari_t *rptt_item = cace_ari_list_cref(rptt_it);

        // intermediate item from the template
        cace_ari_t inter_item = CACE_ARI_INIT_UNDEFINED;
        if (rptt_item->is_ref)
        {
            // item is a reference to be produced, then possibly evaluated
            if (refda_reporting_item_ref(rptctx->runctx, &inter_item, rptt_item))
            {
                // failures in individual items result in undefined value
                CACE_LOG_WARNING("failed to dereference item");
                cace_ari_reset(&inter_item);
            }
        }
        else
        {
            // just use the template item
            cace_ari_set_copy(&inter_item, rptt_item);
        }
        if (cace_log_is_enabled_for(LOG_DEBUG))
        {
            m_string_t buf;
            m_string_init(buf);
            cace_ari_text_encode(buf, &inter_item, CACE_ARI_TEXT_ENC_OPTS_DEFAULT);
            CACE_LOG_DEBUG("intermediate item %s", m_string_get_cstr(buf));
            m_string_clear(buf);
        }

        cace_ari_t rpt_item = CACE_ARI_INIT_UNDEFINED;
        if (CACE_AMM_TYPE_MATCH_POSITIVE == cace_amm_type_match(rptctx->runctx->agent->expr_type, &inter_item))
        {
            // item is an expression to be evaluated
            CACE_LOG_DEBUG("Reporting on expression");
            if (refda_eval_target(rptctx->runctx, &rpt_item, &inter_item))
            {
                CACE_LOG_WARNING("reporting failed to evaluate expression");
                cace_ari_reset(&rpt_item);
            }
        }
        else
        {
            // intermediate is directly reported
            CACE_LOG_DEBUG("Reporting on literal");
            cace_ari_set_move(&rpt_item, &inter_item);
        }
        if (cace_log_is_enabled_for(LOG_DEBUG))
        {
            m_string_t buf;
            m_string_init(buf);
            cace_ari_text_encode(buf, &rpt_item, CACE_ARI_TEXT_ENC_OPTS_DEFAULT);
            CACE_LOG_DEBUG("reporting intermediate %s", m_string_get_cstr(buf));
            m_string_clear(buf);
        }
        cace_ari_deinit(&inter_item);

        cace_ari_list_push_back_move(rptctx->items, &rpt_item);
    }

    return 0;
}

static int refda_reporting_rptt_lit(refda_reporting_ctx_t *rptctx, const cace_ari_t *value)
{
    int retval = 0;
    if (CACE_AMM_TYPE_MATCH_POSITIVE != cace_amm_type_match(rptctx->runctx->agent->rptt_type, value))
    {
        CACE_LOG_WARNING("Attempted reporting on a non-RPTT literal");
        retval = REFDA_REPORTING_ERR_BAD_TYPE;
    }
    else
    {
        CACE_LOG_DEBUG("Reporting on RPTT literal");
        retval = refda_reporting_rptt_val(rptctx, value);
    }
    return retval;
}

/** Report on an arbitrary object reference.
 */
static int refda_reporting_rptt_ref(refda_reporting_ctx_t *rptctx, const cace_ari_t *target)
{
    CACE_LOG_DEBUG("Reporting on RPTT reference");
    int retval = 0;

    cace_amm_lookup_t deref;
    cace_amm_lookup_init(&deref);

    int res = cace_amm_lookup_deref(&deref, &(rptctx->runctx->agent->objs), target);
    CACE_LOG_DEBUG("Lookup result %d", res);
    if (res)
    {
        retval = REFDA_REPORTING_ERR_DEREF_FAILED;
    }

    if (!retval)
    {
        switch (deref.obj_type)
        {
            case CACE_ARI_TYPE_CONST:
            case CACE_ARI_TYPE_VAR:
            case CACE_ARI_TYPE_EDD:
            {
                refda_valprod_ctx_t prodctx;
                refda_valprod_ctx_init(&prodctx, rptctx->runctx, target, &deref);
                retval = refda_valprod_run(&prodctx);
                if (!retval)
                {
                    // execute the produced value as a target
                    retval = refda_reporting_rptt_lit(rptctx, &(prodctx.value));
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

int refda_reporting_target(refda_runctx_t *runctx, const cace_ari_t *target, const cace_ari_t *destination)
{
    CHKERR1(runctx);
    CHKERR1(target);
    if (!cace_ari_not_undefined(destination))
    {
        // nothing to do
        CACE_LOG_WARNING("attempted to report to undefined manager");
        return 1;
    }

    if (cace_log_is_enabled_for(LOG_DEBUG))
    {
        m_string_t dest_buf;
        m_string_init(dest_buf);
        cace_ari_text_encode(dest_buf, destination, CACE_ARI_TEXT_ENC_OPTS_DEFAULT);

        m_string_t buf;
        m_string_init(buf);
        cace_ari_text_encode(buf, target, CACE_ARI_TEXT_ENC_OPTS_DEFAULT);
        CACE_LOG_DEBUG("Reporting to %s for target %s", m_string_get_cstr(dest_buf), m_string_get_cstr(buf));
        m_string_clear(buf);
        m_string_clear(dest_buf);
    }

    refda_reporting_ctx_t rptctx;
    refda_reporting_ctx_init(&rptctx, runctx, destination);

    int retval = 0;
    if (target->is_ref)
    {
        retval = refda_reporting_rptt_ref(&rptctx, target);
    }
    else
    {
        retval = refda_reporting_rptt_lit(&rptctx, target);
    }

    if (!retval)
    {
        refda_reporting_gen(runctx->agent, destination, target, rptctx.items);
    }

    refda_reporting_ctx_deinit(&rptctx);
    return retval;
}

int refda_reporting_gen(refda_agent_t *agent, const cace_ari_t *destination, const cace_ari_t *src,
                        cace_ari_list_t items)
{
    if (!cace_ari_not_undefined(destination))
    {
        // nothing to do
        CACE_LOG_WARNING("attempted to report to undefined destination");
        return 1;
    }

    refda_msgdata_t msg;
    refda_msgdata_init(&msg);
    cace_ari_set_copy(&msg.ident, destination);

    cace_ari_rptset_t *rpts = cace_ari_set_rptset(&msg.value);
    cace_ari_set_null(&(rpts->nonce));
    refda_agent_nowtime(agent, &(rpts->reftime));
    {
        cace_ari_report_t *rpt = cace_ari_report_list_push_back_new(rpts->reports);
        cace_ari_set_td(&(rpt->reltime), (struct timespec) { .tv_sec = 0 });
        cace_ari_set_copy(&(rpt->source), src);

        cace_ari_list_move(rpt->items, items);
        cace_ari_list_init(items);

        if (cace_log_is_enabled_for(LOG_INFO))
        {
            m_string_t buf;
            m_string_init(buf);
            cace_ari_text_encode(buf, src, CACE_ARI_TEXT_ENC_OPTS_DEFAULT);

            m_string_t mgr_buf;
            m_string_init(mgr_buf);
            cace_ari_text_encode(mgr_buf, &msg.ident, CACE_ARI_TEXT_ENC_OPTS_DEFAULT);

            CACE_LOG_INFO("Generated a report destined to %s from source %s with %d items", m_string_get_cstr(mgr_buf),
                          m_string_get_cstr(buf), cace_ari_list_size(rpt->items));
            m_string_clear(mgr_buf);
            m_string_clear(buf);
        }
    }

    refda_msgdata_queue_push_move(agent->rptgs, &msg);
    sem_post(&(agent->rptgs_sem));
    return 0;
}
