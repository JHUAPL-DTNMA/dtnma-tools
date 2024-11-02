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

void refda_reporting_ctx_init(refda_reporting_ctx_t *obj, refda_runctx_t *parent)
{
    CHKVOID(obj);
    obj->parent = parent;
    ari_list_init(obj->items);
}

void refda_reporting_ctx_deinit(refda_reporting_ctx_t *obj)
{
    CHKVOID(obj);
    ari_list_clear(obj->items);
    obj->parent = NULL;
}

int refda_reporting_ctrl(refda_runctx_t *runctx, const ari_t *target, ari_t *result)
{
    ari_t rptset;

    ari_rptset_t *rpts = ari_init_rptset(&rptset);
    ari_set_copy(&(rpts->nonce), runctx->nonce);
    refda_agent_nowtime(runctx->agent, &(rpts->reftime));
    {
        ari_report_t *rpt = ari_report_list_push_back_new(rpts->reports);
        ari_set_td(&(rpt->reltime), (struct timespec) { .tv_sec = 0 });
        ari_set_copy(&(rpt->source), target);

        ari_list_push_back_move(rpt->items, result);
    }
    CACE_LOG_DEBUG("generated an execution report");

    agent_ari_queue_push_move(runctx->agent->rptgs, &rptset);
    sem_post(&(runctx->agent->rptgs_sem));

    return 0;
}

static int refda_reporting_item_val(refda_runctx_t *parent, ari_t *rpt_item, const ari_t *rptt_item)
{
    ari_set_copy(rpt_item, rptt_item);
    return 0;
}

static int refda_reporting_item_lit(refda_runctx_t *parent, ari_t *rpt_item, const ari_t *rptt_item)
{
    int retval = 0;
    if (!amm_type_match(parent->agent->expr_type, rptt_item) && false) // FIXME need to implement typedefs
    {
        CACE_LOG_WARNING("Attempted reporting on a non-EXPR literal");
        retval = REFDA_REPORTING_ERR_BAD_TYPE;
    }
    else
    {
        CACE_LOG_DEBUG("Reporting on item literal");
        retval = refda_reporting_item_val(parent, rpt_item, rptt_item);
    }
    return retval;
}

static int refda_reporting_item_ref(refda_runctx_t *parent, ari_t *rpt_item, const ari_t *rptt_item)
{
    CACE_LOG_DEBUG("Reporting on item reference");
    int retval = 0;

    cace_amm_lookup_t deref;
    cace_amm_lookup_init(&deref);

    int res = cace_amm_lookup_deref(&deref, &(parent->agent->objs), rptt_item);
    CACE_LOG_DEBUG("Lookup result %d", res);
    if (res)
    {
        retval = REFDA_REPORTING_ERR_DEREF_FAILED;
    }

    if (!retval)
    {
        switch (rptt_item->as_ref.objpath.ari_type)
        {
            case ARI_TYPE_CONST:
            case ARI_TYPE_VAR:
            case ARI_TYPE_EDD:
            {
                refda_valprod_ctx_t prodctx;
                refda_valprod_ctx_init(&prodctx, parent, &deref);
                retval = refda_valprod_run(&prodctx);
                if (!retval)
                {
                    // execute the produced value as a target
                    retval = refda_reporting_item_lit(parent, rpt_item, &(prodctx.value));
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
static int refda_reporting_rptt_val(refda_reporting_ctx_t *rptctx, const ari_t *value)
{
    ari_list_it_t rptt_it;
    for (ari_list_it(rptt_it, value->as_lit.value.as_ac->items); !ari_list_end_p(rptt_it); ari_list_next(rptt_it))
    {
        const ari_t *rptt_item = ari_list_cref(rptt_it);
        ari_t        rpt_item  = ARI_INIT_UNDEFINED;

        int res = 0;
        if (rptt_item->is_ref)
        {
            // item is a reference to be produced
            res = refda_reporting_item_ref(rptctx->parent, &rpt_item, rptt_item);
        }
        else
        {
            // item is an expression to be evaluated
            res = refda_reporting_item_lit(rptctx->parent, &rpt_item, rptt_item);
        }

        if (res)
        {
            // failures in individual items result in undefined value
            CACE_LOG_WARNING("reporting failed for a single item with result %d", res);
            ari_reset(&rpt_item);
        }
        else
        {
            if (cace_log_is_enabled_for(LOG_DEBUG))
            {
                string_t buf;
                string_init(buf);
                ari_text_encode(buf, &rpt_item, ARI_TEXT_ENC_OPTS_DEFAULT);
                CACE_LOG_DEBUG("report item result %s", string_get_cstr(buf));
                string_clear(buf);
            }
        }

        ari_list_push_back_move(rptctx->items, &rpt_item);
    }

    return 0;
}

static int refda_reporting_rptt_lit(refda_reporting_ctx_t *rptctx, const ari_t *value)
{
    int retval = 0;
    if (!amm_type_match(rptctx->parent->agent->rptt_type, value) && false) // FIXME need to implement typedefs
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
static int refda_reporting_rptt_ref(refda_reporting_ctx_t *rptctx, const ari_t *target)
{
    CACE_LOG_DEBUG("Reporting on RPTT reference");
    int retval = 0;

    cace_amm_lookup_t deref;
    cace_amm_lookup_init(&deref);

    int res = cace_amm_lookup_deref(&deref, &(rptctx->parent->agent->objs), target);
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
                refda_valprod_ctx_init(&prodctx, rptctx->parent, &deref);
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

    refda_reporting_ctx_t rptctx;
    refda_reporting_ctx_init(&rptctx, runctx);

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
        refda_reporting_gen(runctx->agent, target, rptctx.items);
    }

    refda_reporting_ctx_deinit(&rptctx);
    return retval;
}

int refda_reporting_gen(refda_agent_t *agent, const ari_t *src, ari_list_t items)
{
    ari_t rptset;

    ari_rptset_t *rpts = ari_init_rptset(&rptset);
    ari_set_null(&(rpts->nonce));
    refda_agent_nowtime(agent, &(rpts->reftime));
    {
        ari_report_t *rpt = ari_report_list_push_back_new(rpts->reports);
        ari_set_td(&(rpt->reltime), (struct timespec) { .tv_sec = 0 });
        ari_set_copy(&(rpt->source), src);

        ari_list_move(rpt->items, items);
        ari_list_init(items);

        if (cace_log_is_enabled_for(LOG_DEBUG))
        {
            string_t buf;
            string_init(buf);
            ari_text_encode(buf, src, ARI_TEXT_ENC_OPTS_DEFAULT);
            CACE_LOG_DEBUG("Generated a report for source %s with %d items", string_get_cstr(buf),
                           ari_list_size(rpt->items));
            string_clear(buf);
        }
    }

    agent_ari_queue_push_move(agent->rptgs, &rptset);
    sem_post(&(agent->rptgs_sem));
    return 0;
}
