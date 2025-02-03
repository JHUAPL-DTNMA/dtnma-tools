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
/** @file
 * @ingroup ari
 */
#include "containers.h"
#include "cace/util/defs.h"
#include <inttypes.h>

/// CMP operation not defined by M*LIB
static int ari_list_cmp(const ari_list_t left, const ari_list_t right)
{
    const size_t lt_size = ari_list_size(left);
    const size_t rt_size = ari_list_size(right);

    int part_cmp = M_CMP_DEFAULT(lt_size, rt_size);
    if (part_cmp)
    {
        return part_cmp;
    }

    ari_list_it_t lt_it, rt_it;
    for (ari_list_it(lt_it, left), ari_list_it(rt_it, right); !ari_list_end_p(lt_it) && !ari_list_end_p(rt_it);
         ari_list_next(lt_it), ari_list_next(rt_it))
    {
        part_cmp = ari_cmp(ari_list_cref(lt_it), ari_list_cref(rt_it));
        if (part_cmp)
        {
            return part_cmp;
        }
    }

    return 0;
}

/// CMP operation not defined by M*LIB
static int ari_report_list_cmp(const ari_report_list_t left, const ari_report_list_t right)
{
    const size_t lt_size = ari_report_list_size(left);
    const size_t rt_size = ari_report_list_size(right);

    int part_cmp = M_CMP_DEFAULT(lt_size, rt_size);
    if (part_cmp)
    {
        return part_cmp;
    }

    ari_report_list_it_t lt_it, rt_it;
    for (ari_report_list_it(lt_it, left), ari_report_list_it(rt_it, right);
         !ari_report_list_end_p(lt_it) && !ari_report_list_end_p(rt_it);
         ari_report_list_next(lt_it), ari_report_list_next(rt_it))
    {
        part_cmp = ari_report_cmp(ari_report_list_cref(lt_it), ari_report_list_cref(rt_it));
        if (part_cmp)
        {
            return part_cmp;
        }
    }

    return 0;
}

void ari_ac_init(ari_ac_t *obj)
{
    CHKVOID(obj);
    ari_list_init(obj->items);
}

void ari_ac_deinit(ari_ac_t *obj)
{
    CHKVOID(obj);
    ari_list_clear(obj->items);
}

int ari_ac_cmp(const ari_ac_t *left, const ari_ac_t *right)
{
    CHKRET(left, -2);
    CHKRET(right, -2);

    return ari_list_cmp(left->items, right->items);
}

bool ari_ac_equal(const ari_ac_t *left, const ari_ac_t *right)
{
    CHKFALSE(left);
    CHKFALSE(right);
    return ari_list_equal_p(left->items, right->items);
}

void ari_am_init(ari_am_t *obj)
{
    CHKVOID(obj);
    ari_tree_init(obj->items);
}

void ari_am_deinit(ari_am_t *obj)
{
    CHKVOID(obj);
    ari_tree_clear(obj->items);
}

int ari_am_cmp(const ari_am_t *left, const ari_am_t *right)
{
    CHKRET(left, -2);
    CHKRET(right, -2);

    const size_t lt_size = ari_tree_size(left->items);
    const size_t rt_size = ari_tree_size(right->items);

    int part_cmp = M_CMP_DEFAULT(lt_size, rt_size);
    if (part_cmp)
    {
        return part_cmp;
    }

    ari_tree_it_t lt_it, rt_it;
    for (ari_tree_it(lt_it, left->items), ari_tree_it(rt_it, right->items);
         !ari_tree_end_p(lt_it) && !ari_tree_end_p(rt_it); ari_tree_next(lt_it), ari_tree_next(rt_it))
    {
        const ari_tree_subtype_ct *lt_pair = ari_tree_cref(lt_it);
        const ari_tree_subtype_ct *rt_pair = ari_tree_cref(rt_it);

        part_cmp = ari_cmp(lt_pair->key_ptr, rt_pair->key_ptr);
        if (part_cmp)
        {
            return part_cmp;
        }

        part_cmp = ari_cmp(lt_pair->value_ptr, rt_pair->value_ptr);
        if (part_cmp)
        {
            return part_cmp;
        }
    }

    return 0;
}

bool ari_am_equal(const ari_am_t *left, const ari_am_t *right)
{
    CHKFALSE(left);
    CHKFALSE(right);
    return ari_tree_equal_p(left->items, right->items);
}

void ari_tbl_init(ari_tbl_t *obj, size_t ncols, size_t nrows)
{
    CHKVOID(obj);
    obj->ncols = ncols;
    ari_array_init(obj->items);
    if (ncols && nrows)
    {
        ari_array_resize(obj->items, ncols * nrows);
    }
}

void ari_tbl_deinit(ari_tbl_t *obj)
{
    CHKVOID(obj);
    ari_array_clear(obj->items);
}

bool ari_tbl_cmp(const ari_tbl_t *left, const ari_tbl_t *right)
{
    int part_cmp = M_CMP_BASIC(left->ncols, right->ncols);
    if (part_cmp)
    {
        return part_cmp;
    }

    ari_array_it_t lt_it, rt_it;
    for (ari_array_it(lt_it, left->items), ari_array_it(rt_it, right->items);
         !ari_array_end_p(lt_it) && !ari_array_end_p(rt_it); ari_array_next(lt_it), ari_array_next(rt_it))
    {
        part_cmp = ari_cmp(ari_array_cref(lt_it), ari_array_cref(rt_it));
        if (part_cmp)
        {
            return part_cmp;
        }
    }

    return 0;
}

bool ari_tbl_equal(const ari_tbl_t *left, const ari_tbl_t *right)
{
    CHKFALSE(left);
    CHKFALSE(right);
    return ((left->ncols == right->ncols) && ari_array_equal_p(left->items, right->items));
}

int ari_tbl_move_row_ac(ari_tbl_t *obj, ari_ac_t *row)
{
    CHKERR1(obj);
    CHKERR1(row);
    if (ari_list_size(row->items) != obj->ncols)
    {
        return 2;
    }

    while (!ari_list_empty_p(row->items))
    {
        ari_t item;
        ari_init(&item);
        ari_list_pop_front(&item, row->items);
        ari_array_push_move(obj->items, &item);
        ari_deinit(&item);
    }
    return 0;
}

int ari_tbl_move_row_array(ari_tbl_t *obj, ari_array_t row)
{
    CHKERR1(obj);
    CHKERR1(row);
    if (ari_array_size(row) != obj->ncols)
    {
        return 2;
    }

    ari_array_splice(obj->items, row);
    return 0;
}

void ari_execset_init(ari_execset_t *obj)
{
    ari_init(&(obj->nonce));
    ari_list_init(obj->targets);
}
void ari_execset_deinit(ari_execset_t *obj)
{
    ari_deinit(&(obj->nonce));
    ari_list_clear(obj->targets);
}

int ari_execset_cmp(const ari_execset_t *left, const ari_execset_t *right)
{
    int part_cmp = ari_cmp(&(left->nonce), &(right->nonce));
    if (part_cmp)
    {
        return part_cmp;
    }

    return ari_list_cmp(left->targets, right->targets);
}

bool ari_execset_equal(const ari_execset_t *left, const ari_execset_t *right)
{
    return (ari_equal(&(left->nonce), &(right->nonce)) && ari_list_equal_p(left->targets, right->targets));
}

void ari_report_init(ari_report_t *obj)
{
    ari_init(&(obj->reltime));
    ari_init(&(obj->source));
    ari_list_init(obj->items);
}

void ari_report_init_copy(ari_report_t *obj, const ari_report_t *src)
{
    ari_init_copy(&(obj->reltime), &(src->reltime));
    ari_init_copy(&(obj->source), &(src->source));
    ari_list_init_set(obj->items, src->items);
}

void ari_report_init_move(ari_report_t *obj, ari_report_t *src)
{
    ari_init_move(&(obj->reltime), &(src->reltime));
    ari_init_move(&(obj->source), &(src->source));
    ari_list_init_move(obj->items, src->items);
}

void ari_report_deinit(ari_report_t *obj)
{
    ari_deinit(&(obj->reltime));
    ari_deinit(&(obj->source));
    ari_list_clear(obj->items);
}

void ari_report_set_copy(ari_report_t *obj, const ari_report_t *src)
{
    ari_set_copy(&(obj->source), &(src->source));
    ari_list_set(obj->items, src->items);
}

void ari_report_set_move(ari_report_t *obj, ari_report_t *src)
{
    ari_set_move(&(obj->reltime), &(src->reltime));
    ari_set_move(&(obj->source), &(src->source));
    ari_list_move(obj->items, src->items);
}

int ari_report_cmp(const ari_report_t *left, const ari_report_t *right)
{
    int part_cmp = ari_cmp(&(left->reltime), &(right->reltime));
    if (part_cmp)
    {
        return part_cmp;
    }

    part_cmp = ari_cmp(&(left->source), &(right->source));
    if (part_cmp)
    {
        return part_cmp;
    }

    return ari_list_cmp(left->items, right->items);
}

bool ari_report_equal(const ari_report_t *left, const ari_report_t *right)
{
    return (ari_equal(&(left->reltime), &(right->reltime)) && ari_equal(&(left->source), &(right->source))
            && ari_list_equal_p(left->items, right->items));
}

void ari_rptset_init(ari_rptset_t *obj)
{
    ari_init(&(obj->nonce));
    ari_init(&(obj->reftime));
    ari_report_list_init(obj->reports);
}

void ari_rptset_deinit(ari_rptset_t *obj)
{
    ari_deinit(&(obj->nonce));
    ari_deinit(&(obj->reftime));
    ari_report_list_clear(obj->reports);
}

int ari_rptset_cmp(const ari_rptset_t *left, const ari_rptset_t *right)
{
    int part_cmp = ari_cmp(&(left->nonce), &(right->nonce));
    if (part_cmp)
    {
        return part_cmp;
    }

    part_cmp = ari_cmp(&(left->reftime), &(right->reftime));
    if (part_cmp)
    {
        return part_cmp;
    }

    return ari_report_list_cmp(left->reports, right->reports);
}

bool ari_rptset_equal(const ari_rptset_t *left, const ari_rptset_t *right)
{
    return (ari_equal(&(left->nonce), &(right->nonce)) && ari_equal(&(left->reftime), &(right->reftime))
            && ari_report_list_equal_p(left->reports, right->reports));
}

void ari_lit_init_container(ari_lit_t *lit, ari_type_t ctype)
{
    CHKVOID(lit);
    lit->has_ari_type = true;
    lit->ari_type     = ctype;
    lit->prim_type    = ARI_PRIM_OTHER;

    switch (ctype)
    {
        case ARI_TYPE_AC:
        {
            ari_ac_t *ctr = M_MEMORY_ALLOC(ari_ac_t);
            ari_ac_init(ctr);
            lit->value.as_ac = ctr;
            break;
        }
        case ARI_TYPE_AM:
        {
            ari_am_t *ctr = M_MEMORY_ALLOC(ari_am_t);
            ari_am_init(ctr);
            lit->value.as_am = ctr;
            break;
        }
        case ARI_TYPE_TBL:
        {
            ari_tbl_t *ctr = M_MEMORY_ALLOC(ari_tbl_t);
            ari_tbl_init(ctr, 0, 0);
            lit->value.as_tbl = ctr;
            break;
        }
        case ARI_TYPE_EXECSET:
        {
            ari_execset_t *ctr = M_MEMORY_ALLOC(ari_execset_t);
            ari_execset_init(ctr);
            lit->value.as_execset = ctr;
            break;
        }
        case ARI_TYPE_RPTSET:
        {
            ari_rptset_t *ctr = M_MEMORY_ALLOC(ari_rptset_t);
            ari_rptset_init(ctr);
            lit->value.as_rptset = ctr;
            break;
        }
        default:
            break;
    }
}
