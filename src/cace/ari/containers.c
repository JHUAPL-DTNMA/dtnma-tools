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
/** @file
 * @ingroup ari
 */
#include "containers.h"
#include "cace/util/defs.h"
#include <inttypes.h>

/// CMP operation not defined by M*LIB
static int cace_ari_list_cmp(const cace_ari_list_t left, const cace_ari_list_t right)
{
    const size_t lt_size = cace_ari_list_size(left);
    const size_t rt_size = cace_ari_list_size(right);

    int part_cmp = M_CMP_DEFAULT(lt_size, rt_size);
    if (part_cmp)
    {
        return part_cmp;
    }

    cace_ari_list_it_t lt_it, rt_it;
    for (cace_ari_list_it(lt_it, left), cace_ari_list_it(rt_it, right);
         !cace_ari_list_end_p(lt_it) && !cace_ari_list_end_p(rt_it);
         cace_ari_list_next(lt_it), cace_ari_list_next(rt_it))
    {
        part_cmp = cace_ari_cmp(cace_ari_list_cref(lt_it), cace_ari_list_cref(rt_it));
        if (part_cmp)
        {
            return part_cmp;
        }
    }

    return 0;
}

/// CMP operation not defined by M*LIB
static int cace_ari_report_list_cmp(const cace_ari_report_list_t left, const cace_ari_report_list_t right)
{
    const size_t lt_size = cace_ari_report_list_size(left);
    const size_t rt_size = cace_ari_report_list_size(right);

    int part_cmp = M_CMP_DEFAULT(lt_size, rt_size);
    if (part_cmp)
    {
        return part_cmp;
    }

    cace_ari_report_list_it_t lt_it, rt_it;
    for (cace_ari_report_list_it(lt_it, left), cace_ari_report_list_it(rt_it, right);
         !cace_ari_report_list_end_p(lt_it) && !cace_ari_report_list_end_p(rt_it);
         cace_ari_report_list_next(lt_it), cace_ari_report_list_next(rt_it))
    {
        part_cmp = cace_ari_report_cmp(cace_ari_report_list_cref(lt_it), cace_ari_report_list_cref(rt_it));
        if (part_cmp)
        {
            return part_cmp;
        }
    }

    return 0;
}

void cace_ari_ac_init(cace_ari_ac_t *obj)
{
    CHKVOID(obj);
    cace_ari_list_init(obj->items);
}

void cace_ari_ac_deinit(cace_ari_ac_t *obj)
{
    CHKVOID(obj);
    cace_ari_list_clear(obj->items);
}

int cace_ari_ac_cmp(const cace_ari_ac_t *left, const cace_ari_ac_t *right)
{
    CHKRET(left, -2);
    CHKRET(right, -2);

    return cace_ari_list_cmp(left->items, right->items);
}

bool cace_ari_ac_equal(const cace_ari_ac_t *left, const cace_ari_ac_t *right)
{
    CHKFALSE(left);
    CHKFALSE(right);
    return cace_ari_list_equal_p(left->items, right->items);
}

void cace_ari_am_init(cace_ari_am_t *obj)
{
    CHKVOID(obj);
    cace_ari_tree_init(obj->items);
}

void cace_ari_am_deinit(cace_ari_am_t *obj)
{
    CHKVOID(obj);
    cace_ari_tree_clear(obj->items);
}

int cace_ari_am_cmp(const cace_ari_am_t *left, const cace_ari_am_t *right)
{
    CHKRET(left, -2);
    CHKRET(right, -2);

    const size_t lt_size = cace_ari_tree_size(left->items);
    const size_t rt_size = cace_ari_tree_size(right->items);

    int part_cmp = M_CMP_DEFAULT(lt_size, rt_size);
    if (part_cmp)
    {
        return part_cmp;
    }

    cace_ari_tree_it_t lt_it, rt_it;
    for (cace_ari_tree_it(lt_it, left->items), cace_ari_tree_it(rt_it, right->items);
         !cace_ari_tree_end_p(lt_it) && !cace_ari_tree_end_p(rt_it);
         cace_ari_tree_next(lt_it), cace_ari_tree_next(rt_it))
    {
        const cace_ari_tree_subtype_ct *lt_pair = cace_ari_tree_cref(lt_it);
        const cace_ari_tree_subtype_ct *rt_pair = cace_ari_tree_cref(rt_it);

        part_cmp = cace_ari_cmp(lt_pair->key_ptr, rt_pair->key_ptr);
        if (part_cmp)
        {
            return part_cmp;
        }

        part_cmp = cace_ari_cmp(lt_pair->value_ptr, rt_pair->value_ptr);
        if (part_cmp)
        {
            return part_cmp;
        }
    }

    return 0;
}

bool cace_ari_am_equal(const cace_ari_am_t *left, const cace_ari_am_t *right)
{
    CHKFALSE(left);
    CHKFALSE(right);
    return cace_ari_tree_equal_p(left->items, right->items);
}

void cace_ari_tbl_init(cace_ari_tbl_t *obj, size_t ncols, size_t nrows)
{
    CHKVOID(obj);
    obj->ncols = ncols;
    cace_ari_array_init(obj->items);
    if (ncols && nrows)
    {
        cace_ari_array_resize(obj->items, ncols * nrows);
    }
}

void cace_ari_tbl_deinit(cace_ari_tbl_t *obj)
{
    CHKVOID(obj);
    cace_ari_array_clear(obj->items);
}

bool cace_ari_tbl_cmp(const cace_ari_tbl_t *left, const cace_ari_tbl_t *right)
{
    // column count exactly equal
    int part_cmp = M_CMP_BASIC(left->ncols, right->ncols);
    if (part_cmp)
    {
        return part_cmp;
    }

    const size_t lt_size = cace_ari_array_size(left->items);
    const size_t rt_size = cace_ari_array_size(right->items);

    part_cmp = M_CMP_DEFAULT(lt_size, rt_size);
    if (part_cmp)
    {
        return part_cmp;
    }

    cace_ari_array_it_t lt_it, rt_it;
    for (cace_ari_array_it(lt_it, left->items), cace_ari_array_it(rt_it, right->items);
         !cace_ari_array_end_p(lt_it) && !cace_ari_array_end_p(rt_it);
         cace_ari_array_next(lt_it), cace_ari_array_next(rt_it))
    {
        part_cmp = cace_ari_cmp(cace_ari_array_cref(lt_it), cace_ari_array_cref(rt_it));
        if (part_cmp)
        {
            return part_cmp;
        }
    }

    return 0;
}

bool cace_ari_tbl_equal(const cace_ari_tbl_t *left, const cace_ari_tbl_t *right)
{
    CHKFALSE(left);
    CHKFALSE(right);
    return ((left->ncols == right->ncols) && cace_ari_array_equal_p(left->items, right->items));
}

size_t cace_ari_tbl_num_rows(const cace_ari_tbl_t *obj)
{
    if (!obj)
    {
        return 0;
    }

    return cace_ari_array_size(obj->items) / obj->ncols;
}

int cace_ari_tbl_move_row_ac(cace_ari_tbl_t *obj, cace_ari_ac_t *row)
{
    CHKERR1(obj);
    CHKERR1(row);
    if (cace_ari_list_size(row->items) != obj->ncols)
    {
        return 2;
    }

    while (!cace_ari_list_empty_p(row->items))
    {
        cace_ari_t item;
        cace_ari_init(&item);
        cace_ari_list_pop_front(&item, row->items);
        cace_ari_array_push_move(obj->items, &item);
        cace_ari_deinit(&item);
    }
    return 0;
}

int cace_ari_tbl_move_row_array(cace_ari_tbl_t *obj, cace_ari_array_t row)
{
    CHKERR1(obj);
    CHKERR1(row);
    if (cace_ari_array_size(row) != obj->ncols)
    {
        return 2;
    }

    cace_ari_array_splice(obj->items, row);
    return 0;
}

void cace_ari_execset_init(cace_ari_execset_t *obj)
{
    cace_ari_init(&(obj->nonce));
    cace_ari_list_init(obj->targets);
}
void cace_ari_execset_deinit(cace_ari_execset_t *obj)
{
    cace_ari_deinit(&(obj->nonce));
    cace_ari_list_clear(obj->targets);
}

int cace_ari_execset_cmp(const cace_ari_execset_t *left, const cace_ari_execset_t *right)
{
    int part_cmp = cace_ari_cmp(&(left->nonce), &(right->nonce));
    if (part_cmp)
    {
        return part_cmp;
    }

    return cace_ari_list_cmp(left->targets, right->targets);
}

bool cace_ari_execset_equal(const cace_ari_execset_t *left, const cace_ari_execset_t *right)
{
    return (cace_ari_equal(&(left->nonce), &(right->nonce)) && cace_ari_list_equal_p(left->targets, right->targets));
}

void cace_ari_report_init(cace_ari_report_t *obj)
{
    cace_ari_init(&(obj->reltime));
    cace_ari_init(&(obj->source));
    cace_ari_list_init(obj->items);
}

void cace_ari_report_init_copy(cace_ari_report_t *obj, const cace_ari_report_t *src)
{
    cace_ari_init_copy(&(obj->reltime), &(src->reltime));
    cace_ari_init_copy(&(obj->source), &(src->source));
    cace_ari_list_init_set(obj->items, src->items);
}

void cace_ari_report_init_move(cace_ari_report_t *obj, cace_ari_report_t *src)
{
    cace_ari_init_move(&(obj->reltime), &(src->reltime));
    cace_ari_init_move(&(obj->source), &(src->source));
    cace_ari_list_init_move(obj->items, src->items);
}

void cace_ari_report_deinit(cace_ari_report_t *obj)
{
    cace_ari_deinit(&(obj->reltime));
    cace_ari_deinit(&(obj->source));
    cace_ari_list_clear(obj->items);
}

void cace_ari_report_set_copy(cace_ari_report_t *obj, const cace_ari_report_t *src)
{
    cace_ari_set_copy(&(obj->source), &(src->source));
    cace_ari_list_set(obj->items, src->items);
}

void cace_ari_report_set_move(cace_ari_report_t *obj, cace_ari_report_t *src)
{
    cace_ari_set_move(&(obj->reltime), &(src->reltime));
    cace_ari_set_move(&(obj->source), &(src->source));
    cace_ari_list_move(obj->items, src->items);
}

int cace_ari_report_cmp(const cace_ari_report_t *left, const cace_ari_report_t *right)
{
    int part_cmp = cace_ari_cmp(&(left->reltime), &(right->reltime));
    if (part_cmp)
    {
        return part_cmp;
    }

    part_cmp = cace_ari_cmp(&(left->source), &(right->source));
    if (part_cmp)
    {
        return part_cmp;
    }

    return cace_ari_list_cmp(left->items, right->items);
}

bool cace_ari_report_equal(const cace_ari_report_t *left, const cace_ari_report_t *right)
{
    return (cace_ari_equal(&(left->reltime), &(right->reltime)) && cace_ari_equal(&(left->source), &(right->source))
            && cace_ari_list_equal_p(left->items, right->items));
}

void cace_ari_rptset_init(cace_ari_rptset_t *obj)
{
    cace_ari_init(&(obj->nonce));
    cace_ari_init(&(obj->reftime));
    cace_ari_report_list_init(obj->reports);
}

void cace_ari_rptset_deinit(cace_ari_rptset_t *obj)
{
    cace_ari_deinit(&(obj->nonce));
    cace_ari_deinit(&(obj->reftime));
    cace_ari_report_list_clear(obj->reports);
}

int cace_ari_rptset_cmp(const cace_ari_rptset_t *left, const cace_ari_rptset_t *right)
{
    int part_cmp = cace_ari_cmp(&(left->nonce), &(right->nonce));
    if (part_cmp)
    {
        return part_cmp;
    }

    part_cmp = cace_ari_cmp(&(left->reftime), &(right->reftime));
    if (part_cmp)
    {
        return part_cmp;
    }

    return cace_ari_report_list_cmp(left->reports, right->reports);
}

bool cace_ari_rptset_equal(const cace_ari_rptset_t *left, const cace_ari_rptset_t *right)
{
    return (cace_ari_equal(&(left->nonce), &(right->nonce)) && cace_ari_equal(&(left->reftime), &(right->reftime))
            && cace_ari_report_list_equal_p(left->reports, right->reports));
}

void cace_ari_lit_init_container(cace_ari_lit_t *lit, cace_ari_type_t ctype)
{
    CHKVOID(lit);
    lit->has_ari_type = true;
    lit->ari_type     = ctype;
    lit->prim_type    = CACE_ARI_PRIM_OTHER;

    switch (ctype)
    {
        case CACE_ARI_TYPE_AC:
        {
            cace_ari_ac_t *ctr = M_MEMORY_ALLOC(cace_ari_ac_t);
            cace_ari_ac_init(ctr);
            lit->value.as_ac = ctr;
            break;
        }
        case CACE_ARI_TYPE_AM:
        {
            cace_ari_am_t *ctr = M_MEMORY_ALLOC(cace_ari_am_t);
            cace_ari_am_init(ctr);
            lit->value.as_am = ctr;
            break;
        }
        case CACE_ARI_TYPE_TBL:
        {
            cace_ari_tbl_t *ctr = M_MEMORY_ALLOC(cace_ari_tbl_t);
            cace_ari_tbl_init(ctr, 0, 0);
            lit->value.as_tbl = ctr;
            break;
        }
        case CACE_ARI_TYPE_EXECSET:
        {
            cace_ari_execset_t *ctr = M_MEMORY_ALLOC(cace_ari_execset_t);
            cace_ari_execset_init(ctr);
            lit->value.as_execset = ctr;
            break;
        }
        case CACE_ARI_TYPE_RPTSET:
        {
            cace_ari_rptset_t *ctr = M_MEMORY_ALLOC(cace_ari_rptset_t);
            cace_ari_rptset_init(ctr);
            lit->value.as_rptset = ctr;
            break;
        }
        default:
            break;
    }
}
