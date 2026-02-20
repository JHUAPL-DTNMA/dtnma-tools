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
#include "oper_eval_ctx.h"
#include <cace/ari/text.h>
#include <cace/util/logging.h>
#include <cace/util/defs.h>

void refda_oper_eval_ctx_init(refda_oper_eval_ctx_t *obj)
{
    CHKVOID(obj);
    obj->evalctx = NULL;
    obj->deref   = NULL;
    obj->oper    = NULL;
    cace_ari_itemized_init(&(obj->operands));
    cace_ari_init(&(obj->result));
}

void refda_oper_eval_ctx_deinit(refda_oper_eval_ctx_t *obj)
{
    CHKVOID(obj);
    cace_ari_itemized_deinit(&(obj->operands));
    cace_ari_deinit(&(obj->result));
    obj->oper    = NULL;
    obj->deref   = NULL;
    obj->evalctx = NULL;
}

int refda_oper_eval_ctx_populate(refda_oper_eval_ctx_t *obj, const cace_amm_lookup_t *deref,
                                 const refda_amm_oper_desc_t *oper, refda_eval_ctx_t *eval)
{
    CHKERR1(obj);
    CHKERR1(deref);
    CHKERR1(oper);
    CHKERR1(eval);
    obj->evalctx = eval;
    obj->deref   = deref;
    obj->oper    = oper;

    cace_ari_itemized_reset(&(obj->operands));

    const size_t operand_size = cace_amm_named_type_array_size(oper->operand_types);
    if (cace_ari_list_size(eval->stack) < operand_size)
    {
        return 2;
    }

    cace_ari_array_resize(obj->operands.ordered, operand_size);

    int failcnt = 0;

    cace_amm_named_type_array_it_t typ_it;
    cace_ari_array_it_t            val_it;
    for (cace_amm_named_type_array_it(typ_it, oper->operand_types), cace_ari_array_it(val_it, obj->operands.ordered);
         !cace_amm_named_type_array_end_p(typ_it); cace_amm_named_type_array_next(typ_it), cace_ari_array_next(val_it))
    {
        const cace_amm_named_type_t *typ = cace_amm_named_type_array_cref(typ_it);

        cace_ari_t orig = CACE_ARI_INIT_UNDEFINED;
        cace_ari_list_pop_back_move(&orig, eval->stack);

        cace_ari_t *operand = cace_ari_array_ref(val_it);
        cace_named_ari_ptr_dict_set_at(obj->operands.named, m_string_get_cstr(typ->name), operand);

        int res = cace_amm_type_convert(&(typ->typeobj), operand, &orig);
        if (res)
        {
            if (cace_log_is_enabled_for(LOG_WARNING))
            {
                m_string_t buf;
                m_string_init(buf);
                cace_ari_text_encode(buf, &orig, CACE_ARI_TEXT_ENC_OPTS_DEFAULT);
                CACE_LOG_WARNING("failed to convert operand \"%s\" code %d from value %s", m_string_get_cstr(typ->name),
                                 res, m_string_get_cstr(buf));
                m_string_clear(buf);
            }

            failcnt += 1;
        }
        cace_ari_deinit(&orig);
    }

    return failcnt ? 3 : 0;
}

bool refda_oper_eval_ctx_has_aparam_undefined(const refda_oper_eval_ctx_t *ctx)
{
    CHKFALSE(ctx);
    return ctx->deref->aparams.any_undefined;
}

const cace_ari_t *refda_oper_eval_ctx_get_aparam_index(const refda_oper_eval_ctx_t *ctx, size_t index)
{
    CHKNULL(ctx);
    return cace_ari_array_cget(ctx->deref->aparams.ordered, index);
}

const cace_ari_t *refda_oper_eval_ctx_get_aparam_name(const refda_oper_eval_ctx_t *ctx, const char *name)
{
    CHKNULL(ctx);
    return *cace_named_ari_ptr_dict_cget(ctx->deref->aparams.named, name);
}

bool refda_oper_eval_ctx_has_operand_undefined(const refda_oper_eval_ctx_t *ctx)
{
    CHKFALSE(ctx);
    return ctx->operands.any_undefined;
}

const cace_ari_t *refda_oper_eval_ctx_get_operand_index(const refda_oper_eval_ctx_t *ctx, size_t index)
{
    CHKNULL(ctx);
    return cace_ari_array_cget(ctx->operands.ordered, index);
}

const cace_ari_t *refda_oper_eval_ctx_get_operand_name(const refda_oper_eval_ctx_t *ctx, const char *name)
{
    CHKNULL(ctx);
    return *cace_named_ari_ptr_dict_cget(ctx->operands.named, name);
}

void refda_oper_eval_ctx_set_result_copy(refda_oper_eval_ctx_t *ctx, const cace_ari_t *value)
{
    cace_ari_set_copy(&(ctx->result), value);
}

void refda_oper_eval_ctx_set_result_move(refda_oper_eval_ctx_t *ctx, cace_ari_t *value)
{
    CHKVOID(ctx);
    cace_ari_set_move(&(ctx->result), value);
}
