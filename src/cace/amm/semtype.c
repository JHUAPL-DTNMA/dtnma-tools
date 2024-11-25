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
#include "semtype.h"
#include "cace/ari/algo.h"
#include "cace/util/defs.h"
#include "cace/util/logging.h"
#include "cace/config.h"
#include <m-dict.h>

void amm_semtype_tblt_col_init(amm_semtype_tblt_col_t *obj)
{
    CHKVOID(obj);
    string_init(obj->name);
    amm_type_init(&(obj->typeobj));
}

void amm_semtype_tblt_col_deinit(amm_semtype_tblt_col_t *obj)
{
    CHKVOID(obj);
    amm_type_deinit(&(obj->typeobj));
    string_clear(obj->name);
}

static bool amm_semtype_use_match(const amm_type_t *self, const ari_t *ari)
{
    const amm_semtype_use_t *semtype = self->as_semtype;
    CHKFALSE(semtype);
    const amm_type_t *base = semtype->base;
    CHKFALSE(base);
    CHKFALSE(base->match);

    if (!base->match(base, ari))
    {
        return false;
    }

    // FIXME add constraint checking

    return true;
}

static int amm_semtype_use_convert(const amm_type_t *self, ari_t *out, const ari_t *in)
{
    const amm_semtype_use_t *semtype = self->as_semtype;
    CHKFALSE(semtype);
    const amm_type_t *base = semtype->base;
    CHKERR1(base);
    CHKRET(base->convert, CACE_AMM_ERR_CONVERT_NULLFUNC);

    int res = base->convert(base, out, in);
    CHKERRVAL(res);

    // FIXME add constraint checking

    return 0;
}

int amm_type_set_use_ref(amm_type_t *type, const ari_t *name)
{
    CHKERR1(type);
    CHKERR1(name);
    amm_type_reset(type);

    type->match      = amm_semtype_use_match;
    type->convert    = amm_semtype_use_convert;
    type->type_class = AMM_TYPE_USE;

    amm_semtype_use_t *semtype = ARI_MALLOC(sizeof(amm_semtype_use_t));
    amm_semtype_use_init(semtype);
    type->as_semtype = semtype;
    ari_set_copy(&(semtype->name), name);

    return 0;
}

int amm_type_set_use_direct(amm_type_t *type, const amm_type_t *base)
{
    CHKERR1(type);
    CHKERR1(base);
    amm_type_reset(type);

    type->match      = amm_semtype_use_match;
    type->convert    = amm_semtype_use_convert;
    type->type_class = AMM_TYPE_USE;

    amm_semtype_use_t *semtype = ARI_MALLOC(sizeof(amm_semtype_use_t));
    amm_semtype_use_init(semtype);
    type->as_semtype = semtype;
    semtype->base    = base;

    return 0;
}

static bool amm_semtype_tblt_match(const amm_type_t *self, const ari_t *ari)
{
    const amm_semtype_tblt_t *semtype = self->as_semtype;

    struct ari_tbl_s *val = ari_get_tbl(ari);
    if (!val)
    {
        return false;
    }

    if (val->ncols != amm_semtype_tblt_col_array_size(semtype->columns))
    {
        return false;
    }

    amm_semtype_tblt_col_array_it_t col_it;
    amm_semtype_tblt_col_array_it(col_it, semtype->columns);

    ari_array_it_t val_it;
    for (ari_array_it(val_it, val->items); !ari_array_end_p(val_it);
         ari_array_next(val_it), amm_semtype_tblt_col_array_next(col_it))
    {
        const ari_t *item = ari_array_cref(val_it);

        if (amm_semtype_tblt_col_array_end_p(col_it))
        {
            amm_semtype_tblt_col_array_it(col_it, semtype->columns);
        }
        const amm_type_t *typeobj = &(amm_semtype_tblt_col_array_ref(col_it)->typeobj);

        if (!(typeobj->match(typeobj, item)))
        {
            return false;
        }
    }
    return true;
}

static int amm_semtype_tblt_convert(const amm_type_t *self, ari_t *out, const ari_t *in)
{
    const amm_semtype_tblt_t *semtype = self->as_semtype;

    struct ari_tbl_s *inval = ari_get_tbl(in);
    if (!inval)
    {
        return CACE_AMM_ERR_CONVERT_BADVALUE;
    }

    if (inval->ncols != amm_semtype_tblt_col_array_size(semtype->columns))
    {
        return CACE_AMM_ERR_CONVERT_BADVALUE;
    }

    struct ari_tbl_s outval;

    amm_semtype_tblt_col_array_it_t col_it;
    amm_semtype_tblt_col_array_it(col_it, semtype->columns);

    ari_array_it_t val_it;
    for (ari_array_it(val_it, inval->items); !ari_array_end_p(val_it);
         ari_array_next(val_it), amm_semtype_tblt_col_array_next(col_it))
    {
        const ari_t *item = ari_array_cref(val_it);

        if (amm_semtype_tblt_col_array_end_p(col_it))
        {
            amm_semtype_tblt_col_array_it(col_it, semtype->columns);
        }
        const amm_type_t *typeobj = &(amm_semtype_tblt_col_array_ref(col_it)->typeobj);

        if (!(typeobj->match(typeobj, item)))
        {
            return false;
        }
    }

    ari_set_tbl(out, &outval);
    return 0;
}

amm_semtype_tblt_t *amm_type_set_tblt_size(amm_type_t *type, size_t num_cols)
{
    CHKNULL(type);
    amm_type_reset(type);

    type->match      = amm_semtype_tblt_match;
    type->convert    = amm_semtype_tblt_convert;
    type->type_class = AMM_TYPE_TBLT;

    amm_semtype_tblt_t *semtype = ARI_MALLOC(sizeof(amm_semtype_tblt_t));
    amm_semtype_tblt_init(semtype);
    type->as_semtype = semtype;
    amm_semtype_tblt_col_array_resize(semtype->columns, num_cols);

    return semtype;
}

static bool amm_semtype_union_match(const amm_type_t *self, const ari_t *ari)
{
    const amm_semtype_union_t *semtype = self->as_semtype;

    amm_type_array_it_t it;
    for (amm_type_array_it(it, semtype->choices); !amm_type_array_end_p(it); amm_type_array_next(it))
    {
        const amm_type_t *choice = amm_type_array_ref(it);
        if (amm_type_match(choice, ari))
        {
            return true;
        }
    }
    return false;
}

static int amm_semtype_union_convert(const amm_type_t *self, ari_t *out, const ari_t *in)
{
    const amm_semtype_union_t *semtype = self->as_semtype;

    amm_type_array_it_t it;
    for (amm_type_array_it(it, semtype->choices); !amm_type_array_end_p(it); amm_type_array_next(it))
    {
        const amm_type_t *choice = amm_type_array_ref(it);
        int               res    = amm_type_convert(choice, out, in);
        // first valid conversion wins
        if (res == 0)
        {
            return 0;
        }
    }
    return CACE_AMM_ERR_CONVERT_NOCHOICE;
}

amm_semtype_union_t * amm_type_set_union_size(amm_type_t *type, size_t num_choices)
{
    CHKNULL(type);
    amm_type_reset(type);

    type->match      = amm_semtype_union_match;
    type->convert    = amm_semtype_union_convert;
    type->type_class = AMM_TYPE_UNION;

    amm_semtype_union_t *semtype = ARI_MALLOC(sizeof(amm_semtype_union_t));
    amm_semtype_union_init(semtype);
    type->as_semtype = semtype;
    amm_type_array_resize(semtype->choices, num_choices);

    return semtype;
}
