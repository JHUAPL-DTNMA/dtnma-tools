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
#include "valprod.h"
#include "amm/oper.h"
#include "cace/ari/text.h"
#include "cace/amm/lookup.h"
#include "cace/util/logging.h"
#include "cace/util/defs.h"

/** Expand a reference to a literal value matching SIMPLE or EXPR typedef
 *
 */
static int refda_eval_expand(refda_runctx_t *runctx, refda_eval_item_t out, const ari_t *in)
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
                case ARI_TYPE_CONST:
                case ARI_TYPE_VAR:
                case ARI_TYPE_EDD:
                {
                    refda_valprod_ctx_t prodctx;
                    refda_valprod_ctx_init(&prodctx, runctx, &deref);
                    retval = refda_valprod_run(&prodctx);
                    if (!retval)
                    {
                        // push the produced value as a target
                        refda_eval_item_move_value(out, prodctx.value);
                        ari_init(&prodctx.value);
                    }
                    else
                    {
                        retval = REFDA_EVAL_ERR_PROD_FAILED;
                    }
                    refda_valprod_ctx_deinit(&prodctx);
                    cace_amm_lookup_deinit(&deref);
                    break;
                }
                case ARI_TYPE_OPER:
                case ARI_TYPE_TYPEDEF:
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

int refda_eval_target(refda_runctx_t *runctx, ari_t *result, const ari_t *ari)
{
    CHKERR1(runctx);
    CHKERR1(ari);

    /* FIXME: handle target of object reference for rule/ctrl conditions.
     * This is not reachable for reporting alone.
    ari_t tgt_exp;
    ari_init(&tgt_exp);
    if (refda_eval_expand(runctx, &tgt_exp, ari))
    {
        return REFDA_EVAL_ERR_BAD_TYPE;
    }
    */

    const ari_ac_t *ac = ari_get_ac(ari);
    if (!ac)
    {
        return REFDA_EVAL_ERR_BAD_TYPE;
    }
    CACE_LOG_DEBUG("Evaluating EXPR with %d items", ari_list_size(ac->items));

    refda_eval_ctx_t eval_ctx;
    refda_eval_ctx_init(&eval_ctx, runctx);

    int retval = 0;
    {
        // Expansion phase of the procedure
        ari_list_it_t it;
        for (ari_list_it(it, ac->items); !ari_list_end_p(it) && !retval; ari_list_next(it))
        {
            const ari_t *in_item = ari_list_cref(it);

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

            ari_t *as_value = refda_eval_item_get_value(*item);
            if (as_value)
            {
                ari_list_push_back_move(eval_ctx.stack, as_value);
            }
            else
            {
                const cace_amm_lookup_t *as_obj = refda_eval_item_get_deref(*item);
                switch (as_obj->obj_type)
                {
                    case ARI_TYPE_OPER:
                    {
                        const refda_amm_oper_desc_t *desc = as_obj->obj->app_data.ptr;
                        CACE_LOG_DEBUG("Evaluating OPER %s", string_get_cstr(as_obj->obj->name));

                        eval_ctx.deref = as_obj;
                        int res        = refda_amm_oper_desc_evaluate(desc, &eval_ctx);
                        eval_ctx.deref = NULL;
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
        CACE_LOG_DEBUG("Evaluation ends with %d stack items", ari_list_size(eval_ctx.stack));
        if (ari_list_size(eval_ctx.stack) == 1)
        {
            ari_set_copy(result, ari_list_front(eval_ctx.stack));
            if (cace_log_is_enabled_for(LOG_DEBUG))
            {
                string_t buf;
                string_init(buf);
                ari_text_encode(buf, result, ARI_TEXT_ENC_OPTS_DEFAULT);
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
