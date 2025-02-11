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
#include "eval.h"
#include "eval_ctx.h"
#include "oper_eval_ctx.h"
#include "valprod.h"
#include "amm/oper.h"
#include "cace/ari/text.h"
#include "cace/amm/lookup.h"
#include "cace/util/logging.h"
#include "cace/util/defs.h"

/** Expand a reference to a literal value matching SIMPLE or EXPR typedef
 *
 */
static int refda_eval_expand(refda_runctx_t *runctx, refda_eval_item_t out, const cace_ari_t *in)
{
    int retval = 0;
    if (in->is_ref)
    {
        cace_amm_lookup_t deref;
        cace_amm_lookup_init(&deref);

        int res = cace_amm_lookup_deref(&deref, &(runctx->agent->objs), in);
        CACE_LOG_DEBUG("Lookup result %d", res);
        if (res)
        {
            retval = REFDA_EVAL_ERR_DEREF_FAILED;
        }
        else
        {
            switch (deref.obj_type)
            {
                case CACE_ARI_TYPE_CONST:
                case CACE_ARI_TYPE_VAR:
                case CACE_ARI_TYPE_EDD:
                {
                    refda_valprod_ctx_t prodctx;
                    refda_valprod_ctx_init(&prodctx, runctx, in, &deref);
                    retval = refda_valprod_run(&prodctx);
                    if (!retval)
                    {
                        // push the produced value as a target
                        refda_eval_item_move_value(out, prodctx.value);
                        cace_ari_init(&prodctx.value);
                    }
                    else
                    {
                        retval = REFDA_EVAL_ERR_PROD_FAILED;
                    }
                    refda_valprod_ctx_deinit(&prodctx);
                    cace_amm_lookup_deinit(&deref);
                    break;
                }
                case CACE_ARI_TYPE_OPER:
                case CACE_ARI_TYPE_TYPEDEF:
                    // leave these references in place
                    refda_eval_item_move_deref(out, deref);
                    break;
                default:
                    cace_amm_lookup_deinit(&deref);
                    retval = REFDA_EVAL_ERR_BAD_TYPE;
                    break;
            }
        }
    }
    else
    {
        // FIXME check literal type(s)
        refda_eval_item_set_value(out, *in);
    }

    return retval;
}

static int refda_eval_oper(const cace_amm_lookup_t *deref, refda_eval_ctx_t *ctx)
{
    const refda_amm_oper_desc_t *desc = deref->obj->app_data.ptr;
    CHKRET(desc->evaluate, 2);
    CHKRET(cace_amm_type_is_valid(&(desc->res_type)), 3);

    refda_oper_eval_ctx_t operctx;
    refda_oper_eval_ctx_init(&operctx);

    int res = refda_oper_eval_ctx_populate(&operctx, deref, desc, ctx);
    if (res)
    {
        CACE_LOG_WARNING("Failed to populate an OPER evaluation context, code %d", res);
        refda_oper_eval_ctx_deinit(&operctx);
        return 2;
    }

    if (cace_log_is_enabled_for(LOG_DEBUG))
    {
        CACE_LOG_DEBUG("Evaluating OPER %s with %d operands", string_get_cstr(deref->obj->name),
                       cace_ari_array_size(operctx.operands.ordered));
    }
    (desc->evaluate)(&operctx);
    if (cace_log_is_enabled_for(LOG_DEBUG))
    {
        string_t buf;
        string_init(buf);
        cace_ari_text_encode(buf, &(operctx.result), CACE_ARI_TEXT_ENC_OPTS_DEFAULT);
        CACE_LOG_DEBUG("evaluation finished with result %s", string_get_cstr(buf));
        string_clear(buf);
    }

    int retval = 0;
    {
        // force result type
        cace_ari_t tmp;
        cace_ari_init(&tmp);
        res = cace_amm_type_convert(&(desc->res_type), &tmp, &(operctx.result));
        if (res)
        {
            cace_ari_deinit(&tmp);
            retval = 4;
        }
        else
        {
            cace_ari_list_push_back_move(ctx->stack, &tmp);
        }
    }

    refda_oper_eval_ctx_deinit(&operctx);
    return retval;
}

int refda_eval_target(refda_runctx_t *runctx, cace_ari_t *result, const cace_ari_t *ari)
{
    CHKERR1(runctx);
    CHKERR1(ari);

    if (cace_log_is_enabled_for(LOG_DEBUG))
    {
        string_t buf;
        string_init(buf);
        cace_ari_text_encode(buf, ari, CACE_ARI_TEXT_ENC_OPTS_DEFAULT);
        CACE_LOG_DEBUG("Evaluation input %s", string_get_cstr(buf));
        string_clear(buf);
    }

    /* FIXME: handle target of object reference for rule/ctrl conditions.
     * This is not reachable for reporting alone.
    cace_ari_t tgt_exp;
    cace_ari_init(&tgt_exp);
    if (refda_eval_expand(runctx, &tgt_exp, ari))
    {
        return REFDA_EVAL_ERR_BAD_TYPE;
    }
    */

    const cace_ari_ac_t *ac = cace_ari_cget_ac(ari);
    if (!ac)
    {
        return REFDA_EVAL_ERR_BAD_TYPE;
    }

    refda_eval_ctx_t eval_ctx;
    refda_eval_ctx_init(&eval_ctx, runctx);

    int retval = 0;
    {
        // Expansion phase of the procedure
        cace_ari_list_it_t it;
        for (cace_ari_list_it(it, ac->items); !cace_ari_list_end_p(it) && !retval; cace_ari_list_next(it))
        {
            const cace_ari_t *in_item = cace_ari_list_cref(it);

            refda_eval_item_t *exp_item = refda_eval_list_push_back_new(eval_ctx.input);

            int res = refda_eval_expand(runctx, *exp_item, in_item);
            if (res)
            {
                // stop early if expansion fails
                retval = res;
            }
        }
    }

    if (!retval)
    {
        // Reduction phase of the procedure
        refda_eval_list_it_t in_it;
        for (refda_eval_list_it(in_it, eval_ctx.input); !refda_eval_list_end_p(in_it) && !retval;
             refda_eval_list_next(in_it))
        {
            refda_eval_item_t *item = refda_eval_list_ref(in_it);

            cace_ari_t *as_value = refda_eval_item_get_value(*item);
            if (as_value)
            {
                cace_ari_list_push_back_move(eval_ctx.stack, as_value);
            }
            else
            {
                const cace_amm_lookup_t *as_obj = refda_eval_item_get_deref(*item);
                switch (as_obj->obj_type)
                {
                    case CACE_ARI_TYPE_OPER:
                    {
                        int res = refda_eval_oper(as_obj, &eval_ctx);
                        CACE_LOG_DEBUG("Evaluation return code %d", res);
                        if (res)
                        {
                            // stop early if reduction fails
                            retval = res;
                        }
                        break;
                    }
                    default:
                        break;
                }
            }
        }
    }

    if (!retval)
    {
        // overall result is the single value left on the stack
        CACE_LOG_DEBUG("Evaluation ends with %d stack items", cace_ari_list_size(eval_ctx.stack));
        if (cace_ari_list_size(eval_ctx.stack) == 1)
        {
            cace_ari_set_copy(result, cace_ari_list_front(eval_ctx.stack));
            if (cace_log_is_enabled_for(LOG_DEBUG))
            {
                string_t buf;
                string_init(buf);
                cace_ari_text_encode(buf, result, CACE_ARI_TEXT_ENC_OPTS_DEFAULT);
                CACE_LOG_DEBUG("Evaluation result %s", string_get_cstr(buf));
                string_clear(buf);
            }
        }
        else
        {
            retval = REFDA_EVAL_ERR_NON_SINGLE;
        }
    }

    refda_eval_ctx_deinit(&eval_ctx);
    return retval;
}
