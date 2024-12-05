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

static bool amm_semtype_use_constraints(const amm_semtype_use_t *semtype, const ari_t *val)
{
    amm_semtype_cnst_array_it_t it;
    for (amm_semtype_cnst_array_it(it, semtype->constraints); !amm_semtype_cnst_array_end_p(it); amm_semtype_cnst_array_next(it))
    {
        const amm_semtype_cnst_t *cnst = amm_semtype_cnst_array_cref(it);
        if (!amm_semtype_cnst_is_valid(cnst, val))
        {
            CACE_LOG_WARNING("type use constraint %d failed", cnst->type);
            // first failure wins
            return false;
        }
    }
    // no constraints are valid also
    return true;
}

static bool amm_semtype_use_match(const amm_type_t *self, const ari_t *ari)
{
    const amm_semtype_use_t *semtype = self->as_semtype;
    CHKFALSE(semtype);
    const amm_type_t *base = semtype->base;
    CHKFALSE(base);

    if (!amm_type_match(base, ari))
    {
        return false;
    }

    return amm_semtype_use_constraints(semtype, ari);
}

static int amm_semtype_use_convert(const amm_type_t *self, ari_t *out, const ari_t *in)
{
    const amm_semtype_use_t *semtype = self->as_semtype;
    CHKFALSE(semtype);
    const amm_type_t *base = semtype->base;
    CHKERR1(base);

    int res = amm_type_convert(base, out, in);
    CHKERRVAL(res);

    if (!amm_semtype_use_constraints(semtype, out))
    {
        return CACE_AMM_ERR_CONVERT_FAILED_CONSTRAINT;
    }

    return 0;
}

int amm_type_set_use_ref(amm_type_t *type, const ari_t *name)
{
    ari_t tmp;
    ari_init_copy(&tmp, name);
    return amm_type_set_use_ref_move(type, &tmp);
}

int amm_type_set_use_ref_move(amm_type_t *type, ari_t *name)
{
    CHKERR1(type);
    CHKERR1(name);
    amm_type_reset(type);

    type->match      = amm_semtype_use_match;
    type->convert    = amm_semtype_use_convert;
    type->type_class = AMM_TYPE_USE;

    amm_semtype_use_t *semtype = ARI_MALLOC(sizeof(amm_semtype_use_t));
    amm_semtype_use_init(semtype);
    ari_set_move(&(semtype->name), name);

    type->as_semtype        = semtype;
    type->as_semtype_deinit = (amm_semtype_deinit_f)amm_semtype_use_deinit;

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
    semtype->base = base;

    type->as_semtype        = semtype;
    type->as_semtype_deinit = (amm_semtype_deinit_f)amm_semtype_use_deinit;

    return 0;
}

static bool amm_semtype_ulist_match(const amm_type_t *self, const ari_t *ari)
{
    const amm_semtype_ulist_t *semtype = self->as_semtype;

    const struct ari_ac_s *val = ari_cget_ac(ari);
    if (!val)
    {
        return false;
    }

    // overall size constraints
    const size_t valsize = ari_list_size(val->items);
    if (semtype->size.has_min)
    {
        if (valsize < semtype->size.i_min)
        {
            return false;
        }
    }
    if (semtype->size.has_max)
    {
        if (valsize > semtype->size.i_max)
        {
            return false;
        }
    }

    // per-item check
    ari_list_it_t val_it;
    for (ari_list_it(val_it, val->items); !ari_list_end_p(val_it); ari_list_next(val_it))
    {
        const ari_t *val_item = ari_list_cref(val_it);

        if (!amm_type_match(&(semtype->item_type), val_item))
        {
            return false;
        }
    }
    return true;
}

static int amm_semtype_ulist_convert(const amm_type_t *self, ari_t *out, const ari_t *in)
{
    const amm_semtype_ulist_t *semtype = self->as_semtype;

    const struct ari_ac_s *inval = ari_cget_ac(in);
    if (!inval)
    {
        return CACE_AMM_ERR_CONVERT_BADVALUE;
    }

    // overall size constraints
    const size_t valsize = ari_list_size(inval->items);
    if (semtype->size.has_min)
    {
        if (valsize < semtype->size.i_min)
        {
            return CACE_AMM_ERR_CONVERT_BADVALUE;
        }
    }
    if (semtype->size.has_max)
    {
        if (valsize > semtype->size.i_max)
        {
            return CACE_AMM_ERR_CONVERT_BADVALUE;
        }
    }

    ari_ac_t outval;
    ari_ac_init(&outval);
    int retval = 0;

    // input and output have exact same size
    ari_list_it_t inval_it;
    for (ari_list_it(inval_it, inval->items); !ari_list_end_p(inval_it); ari_list_next(inval_it))
    {
        const ari_t *in_item = ari_list_cref(inval_it);

        ari_t out_item = ARI_INIT_UNDEFINED;
        // actual conversion
        int res = amm_type_convert(&(semtype->item_type), &out_item, in_item);
        if (res)
        {
            retval = res;
            ari_deinit(&out_item);
            break;
        }

        ari_list_push_back_move(outval.items, &out_item);
    }

    // always pass ownership to the output value
    ari_set_ac(out, &outval);
    return retval;
}

amm_semtype_ulist_t *amm_type_set_ulist(amm_type_t *type)
{
    CHKNULL(type);
    amm_type_reset(type);

    type->match      = amm_semtype_ulist_match;
    type->convert    = amm_semtype_ulist_convert;
    type->type_class = AMM_TYPE_ULIST;

    amm_semtype_ulist_t *semtype = ARI_MALLOC(sizeof(amm_semtype_ulist_t));
    amm_semtype_ulist_init(semtype);

    type->as_semtype        = semtype;
    type->as_semtype_deinit = (amm_semtype_deinit_f)amm_semtype_ulist_deinit;

    return semtype;
}

/** Match a sub-sequence of an AC using an input iterator.
 */
static bool amm_semtype_seq_match_it(const amm_semtype_seq_t *seq, ari_list_it_t val_it)
{
    // iterate until the sequence limit is hit or there are no more
    size_t used = 0;
    while (!ari_list_end_p(val_it) && (!seq->size.has_max || (used < seq->size.i_max)))
    {
        const ari_t *val_item = ari_list_cref(val_it);

        // actual match
        if (!amm_type_match(&(seq->item_type), val_item))
        {
            // don't fail here, just stop matching this sequence
            break;
        }

        ++used;
        ari_list_next(val_it);
    }

    // not enough matched
    if (seq->size.has_min && (used < seq->size.i_min))
    {
        return false;
    }

    return true;
}

/** Convert a sub-sequence of an AC using an input iterator.
 */
static bool amm_semtype_seq_convert_it(const amm_semtype_seq_t *seq, ari_list_t out, ari_list_it_t inval_it)
{
    // iterate until the sequence limit is hit or there are no more
    size_t used = 0;
    while (!ari_list_end_p(inval_it) && (!seq->size.has_max || (used < seq->size.i_max)))
    {
        const ari_t *in_item = ari_list_cref(inval_it);

        ari_t out_item = ARI_INIT_UNDEFINED;
        // actual conversion
        int res = amm_type_convert(&(seq->item_type), &out_item, in_item);
        if (res)
        {
            // don't fail here, just stop matching this sequence
            break;
        }

        ++used;
        ari_list_push_back_move(out, &out_item);
        ari_list_next(inval_it);
    }

    // not enough matched
    if (seq->size.has_min && (used < seq->size.i_min))
    {
        return false;
    }

    return true;
}

static bool amm_semtype_dlist_match(const amm_type_t *self, const ari_t *ari)
{
    const amm_semtype_dlist_t *semtype = self->as_semtype;

    const struct ari_ac_s *val = ari_cget_ac(ari);
    if (!val)
    {
        return false;
    }

    // try to consume all value items
    ari_list_it_t val_it;
    ari_list_it(val_it, val->items);

    amm_type_array_it_t typ_it;
    for (amm_type_array_it(typ_it, semtype->types); !amm_type_array_end_p(typ_it); amm_type_array_next(typ_it))
    {
        // each type in the list takes off one or more items
        const amm_type_t *typ_item = amm_type_array_cref(typ_it);

        if (typ_item->type_class == AMM_TYPE_SEQ)
        {
            amm_semtype_seq_t *seq = typ_item->as_semtype;
            if (!amm_semtype_seq_match_it(seq, val_it))
            {
                return false;
            }
        }
        else
        {
            // not enough values
            if (ari_list_end_p(val_it))
            {
                return false;
            }
            const ari_t *val_item = ari_list_cref(val_it);

            // actual match
            if (!amm_type_match(typ_item, val_item))
            {
                return false;
            }

            ari_list_next(val_it);
        }
    }

    // too many input values
    if (!ari_list_end_p(val_it))
    {
        return false;
    }

    return true;
}

static int amm_semtype_dlist_convert(const amm_type_t *self, ari_t *out, const ari_t *in)
{
    const amm_semtype_dlist_t *semtype = self->as_semtype;
    M_ASSERT(semtype);

    const struct ari_ac_s *inval = ari_cget_ac(in);
    if (!inval)
    {
        return CACE_AMM_ERR_CONVERT_BADVALUE;
    }

    ari_ac_t outval;
    ari_ac_init(&outval);
    int retval = 0;

    ari_list_it_t inval_it;
    ari_list_it(inval_it, inval->items);

    amm_type_array_it_t typ_it;
    for (amm_type_array_it(typ_it, semtype->types); !amm_type_array_end_p(typ_it); amm_type_array_next(typ_it))
    {
        // each type in the list takes off one or more items
        const amm_type_t *typ_item = amm_type_array_cref(typ_it);

        if (typ_item->type_class == AMM_TYPE_SEQ)
        {
            amm_semtype_seq_t *seq = typ_item->as_semtype;
            if (!amm_semtype_seq_convert_it(seq, outval.items, inval_it))
            {
                retval = CACE_AMM_ERR_CONVERT_BADVALUE;
                break;
            }
        }
        else
        {
            // not enough values
            if (ari_list_end_p(inval_it))
            {
                retval = CACE_AMM_ERR_CONVERT_BADVALUE;
                break;
            }
            const ari_t *in_item = ari_list_cref(inval_it);

            ari_t out_item = ARI_INIT_UNDEFINED;
            // actual conversion
            int res = amm_type_convert(typ_item, &out_item, in_item);
            if (res)
            {
                retval = res;
                ari_deinit(&out_item);
                break;
            }

            ari_list_push_back_move(outval.items, &out_item);
            ari_list_next(inval_it);
        }
    }

    // too many input values (and no earlier error)
    if (!retval && !ari_list_end_p(inval_it))
    {
        retval = CACE_AMM_ERR_CONVERT_BADVALUE;
    }

    // always pass ownership to the output value
    ari_set_ac(out, &outval);
    return retval;
}

amm_semtype_dlist_t *amm_type_set_dlist(amm_type_t *type, size_t num_types)
{
    CHKNULL(type);
    amm_type_reset(type);

    type->match      = amm_semtype_dlist_match;
    type->convert    = amm_semtype_dlist_convert;
    type->type_class = AMM_TYPE_DLIST;

    amm_semtype_dlist_t *semtype = ARI_MALLOC(sizeof(amm_semtype_dlist_t));
    amm_semtype_dlist_init(semtype);
    amm_type_array_resize(semtype->types, num_types);

    type->as_semtype        = semtype;
    type->as_semtype_deinit = (amm_semtype_deinit_f)amm_semtype_dlist_deinit;

    return semtype;
}

static bool amm_semtype_umap_match(const amm_type_t *self, const ari_t *ari)
{
    const amm_semtype_umap_t *semtype = self->as_semtype;

    const struct ari_am_s *val = ari_cget_am(ari);
    if (!val)
    {
        return false;
    }

    // per-item check
    ari_tree_it_t val_it;
    for (ari_tree_it(val_it, val->items); !ari_tree_end_p(val_it); ari_tree_next(val_it))
    {
        const ari_tree_itref_t *val_item = ari_tree_cref(val_it);

        if (!amm_type_match(&(semtype->key_type), val_item->key_ptr))
        {
            return false;
        }
        if (!amm_type_match(&(semtype->val_type), val_item->value_ptr))
        {
            return false;
        }
    }
    return true;
}

static int amm_semtype_umap_convert(const amm_type_t *self, ari_t *out, const ari_t *in)
{
    const amm_semtype_umap_t *semtype = self->as_semtype;

    const struct ari_am_s *inval = ari_cget_am(in);
    if (!inval)
    {
        return CACE_AMM_ERR_CONVERT_BADVALUE;
    }

    ari_am_t outval;
    ari_am_init(&outval);
    int retval = 0;

    // input and output have exact same size
    ari_tree_it_t inval_it;
    for (ari_tree_it(inval_it, inval->items); !ari_tree_end_p(inval_it); ari_tree_next(inval_it))
    {
        const ari_tree_itref_t *in_item = ari_tree_cref(inval_it);
        ari_t                   out_key = ARI_INIT_UNDEFINED;
        ari_t                   out_val = ARI_INIT_UNDEFINED;

        int res = amm_type_convert(&(semtype->key_type), &out_key, in_item->key_ptr);
        if (res)
        {
            retval = res;
            break;
        }

        res = amm_type_convert(&(semtype->val_type), &out_val, in_item->value_ptr);
        if (res)
        {
            retval = res;
            break;
        }

        // FIXME is there a more efficient way?
        ari_tree_set_at(outval.items, out_key, out_val);
        ari_deinit(&out_key);
        ari_deinit(&out_val);
    }

    // always pass ownership to the output value
    ari_set_am(out, &outval);
    return retval;
}

amm_semtype_umap_t *amm_type_set_umap(amm_type_t *type)
{
    CHKNULL(type);
    amm_type_reset(type);

    type->match      = amm_semtype_umap_match;
    type->convert    = amm_semtype_umap_convert;
    type->type_class = AMM_TYPE_UMAP;

    amm_semtype_umap_t *semtype = ARI_MALLOC(sizeof(amm_semtype_umap_t));
    amm_semtype_umap_init(semtype);

    type->as_semtype        = semtype;
    type->as_semtype_deinit = (amm_semtype_deinit_f)amm_semtype_umap_deinit;

    return semtype;
}

static bool amm_semtype_tblt_match(const amm_type_t *self, const ari_t *ari)
{
    const amm_semtype_tblt_t *semtype = self->as_semtype;

    const struct ari_tbl_s *val = ari_cget_tbl(ari);
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
        const ari_t *val_item = ari_array_cref(val_it);

        if (amm_semtype_tblt_col_array_end_p(col_it))
        {
            amm_semtype_tblt_col_array_it(col_it, semtype->columns);
        }
        const amm_type_t *typeobj = &(amm_semtype_tblt_col_array_ref(col_it)->typeobj);

        if (!amm_type_match(typeobj, val_item))
        {
            return false;
        }
    }
    return true;
}

static int amm_semtype_tblt_convert(const amm_type_t *self, ari_t *out, const ari_t *in)
{
    const amm_semtype_tblt_t *semtype = self->as_semtype;

    const struct ari_tbl_s *inval = ari_cget_tbl(in);
    if (!inval)
    {
        return CACE_AMM_ERR_CONVERT_BADVALUE;
    }

    if (inval->ncols != amm_semtype_tblt_col_array_size(semtype->columns))
    {
        return CACE_AMM_ERR_CONVERT_BADVALUE;
    }

    ari_tbl_t    outval;
    const size_t nrows = ari_array_size(inval->items) / inval->ncols;
    ari_tbl_init(&outval, inval->ncols, nrows);
    int retval = 0;

    amm_semtype_tblt_col_array_it_t col_it;
    amm_semtype_tblt_col_array_it(col_it, semtype->columns);

    // input and output have exact same size
    ari_array_it_t inval_it, outval_it;
    for (ari_array_it(inval_it, inval->items), ari_array_it(outval_it, outval.items); !ari_array_end_p(inval_it);
         ari_array_next(inval_it), ari_array_next(outval_it), amm_semtype_tblt_col_array_next(col_it))
    {
        const ari_t *in_item  = ari_array_cref(inval_it);
        ari_t       *out_item = ari_array_ref(outval_it);

        if (amm_semtype_tblt_col_array_end_p(col_it))
        {
            amm_semtype_tblt_col_array_it(col_it, semtype->columns);
        }
        const amm_type_t *typeobj = &(amm_semtype_tblt_col_array_ref(col_it)->typeobj);

        int res = amm_type_convert(typeobj, out_item, in_item);
        if (res)
        {
            retval = res;
            break;
        }
    }

    // always pass ownership to the output value
    ari_set_tbl(out, &outval);
    return retval;
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
    amm_semtype_tblt_col_array_resize(semtype->columns, num_cols);

    type->as_semtype        = semtype;
    type->as_semtype_deinit = (amm_semtype_deinit_f)amm_semtype_tblt_deinit;

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

amm_semtype_union_t *amm_type_set_union_size(amm_type_t *type, size_t num_choices)
{
    CHKNULL(type);
    amm_type_reset(type);

    type->match      = amm_semtype_union_match;
    type->convert    = amm_semtype_union_convert;
    type->type_class = AMM_TYPE_UNION;

    amm_semtype_union_t *semtype = ARI_MALLOC(sizeof(amm_semtype_union_t));
    amm_semtype_union_init(semtype);
    amm_type_array_resize(semtype->choices, num_choices);

    type->as_semtype        = semtype;
    type->as_semtype_deinit = (amm_semtype_deinit_f)amm_semtype_union_deinit;

    return semtype;
}

amm_semtype_seq_t *amm_type_set_seq(amm_type_t *type)
{
    CHKNULL(type);
    amm_type_reset(type);

    // not usable as a direct semantic type, see amm_semtype_dlist_match() and amm_semtype_dlist_convert()
    type->match      = NULL;
    type->convert    = NULL;
    type->type_class = AMM_TYPE_SEQ;

    amm_semtype_seq_t *semtype = ARI_MALLOC(sizeof(amm_semtype_seq_t));
    amm_semtype_seq_init(semtype);

    type->as_semtype        = semtype;
    type->as_semtype_deinit = (amm_semtype_deinit_f)amm_semtype_seq_deinit;

    return semtype;
}
