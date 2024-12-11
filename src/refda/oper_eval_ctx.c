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
#include "oper_eval_ctx.h"
#include <cace/util/logging.h>
#include <cace/util/defs.h>

void refda_oper_eval_ctx_init(refda_oper_eval_ctx_t *obj)
{
    obj->evalctx = NULL;
    obj->deref   = NULL;
    obj->oper    = NULL;
    cace_ari_itemized_init(&(obj->operands));
    ari_init(&(obj->result));
}

void refda_oper_eval_ctx_deinit(refda_oper_eval_ctx_t *obj)
{
    cace_ari_itemized_deinit(&(obj->operands));
    ari_deinit(&(obj->result));
    obj->oper    = NULL;
    obj->deref   = NULL;
    obj->evalctx = NULL;
}

int refda_oper_eval_ctx_populate(refda_oper_eval_ctx_t *obj, const cace_amm_lookup_t *deref,
                                 const refda_amm_oper_desc_t *oper, refda_eval_ctx_t *eval)
{
    CHKERR1(obj);
    obj->evalctx = eval;
    obj->deref   = deref;
    obj->oper    = oper;

    cace_ari_itemized_reset(&(obj->operands));

    const size_t operand_size = amm_named_type_array_size(oper->operand_types);
    if (ari_list_size(eval->stack) < operand_size)
    {
        return 2;
    }

    ari_array_resize(obj->operands.ordered, operand_size);

    int                       failcnt = 0;
    amm_named_type_array_it_t typ_it;
    ari_array_it_t            val_it;
    for (amm_named_type_array_it(typ_it, oper->operand_types), ari_array_it(val_it, obj->operands.ordered);
         !amm_named_type_array_end_p(typ_it); amm_named_type_array_next(typ_it), ari_array_next(val_it))
    {
        const amm_named_type_t *typ = amm_named_type_array_cref(typ_it);

        ari_t orig = ARI_INIT_UNDEFINED;
        ari_list_pop_back_move(&orig, eval->stack);

        ari_t *operand = ari_array_ref(val_it);
        named_ari_ptr_dict_set_at(obj->operands.named, m_string_get_cstr(typ->name), operand);

        int    res     = amm_type_convert(&(typ->typeobj), operand, &orig);
        ari_deinit(&orig);
        if (res)
        {
            CACE_LOG_WARNING("failed to convert operand \"%s\" code %d", m_string_get_cstr(typ->name), res);
            failcnt += 1;
        }
    }

    return failcnt ? 3 : 0;
}

const ari_t *refda_oper_eval_ctx_get_aparam_index(refda_oper_eval_ctx_t *ctx, size_t index)
{
    return ari_array_cget(ctx->deref->aparams.ordered, index);
}

const ari_t *refda_oper_eval_ctx_get_aparam_name(refda_oper_eval_ctx_t *ctx, const char *name)
{
    return *named_ari_ptr_dict_cget(ctx->deref->aparams.named, name);
}

const ari_t *refda_oper_eval_ctx_get_operand_index(refda_oper_eval_ctx_t *ctx, size_t index)
{
    return ari_array_cget(ctx->operands.ordered, index);
}

const ari_t *refda_oper_eval_ctx_get_operand_name(refda_oper_eval_ctx_t *ctx, const char *name)
{
    return *named_ari_ptr_dict_cget(ctx->operands.named, name);
}

void refda_oper_eval_ctx_set_result_copy(refda_oper_eval_ctx_t *ctx, const ari_t *value)
{
    ari_set_copy(&(ctx->result), value);
}

void refda_oper_eval_ctx_set_result_move(refda_oper_eval_ctx_t *ctx, ari_t *value)
{
    ari_set_move(&(ctx->result), value);
}
