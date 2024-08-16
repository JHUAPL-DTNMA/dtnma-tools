/*
 * Copyright (c) 2011-2023 The Johns Hopkins University Applied Physics
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
#include "base.h"
#include "containers.h"
#include "cace/util.h"
#include <inttypes.h>

int ari_lit_deinit(ari_lit_t *obj)
{
    switch (obj->prim_type)
    {
        case ARI_PRIM_TSTR:
        case ARI_PRIM_BSTR:
            if (cace_data_deinit(&(obj->value.as_data)))
            {
                return 2;
            }
            break;
        default:
            // handle ARI type
            break;
    }

    if (obj->has_ari_type)
    {
        switch (obj->ari_type)
        {
            case ARI_TYPE_AC:
                CHKERR1(obj->value.as_ac);
                ari_ac_deinit(obj->value.as_ac);
                M_MEMORY_DEL(obj->value.as_ac);
                break;
            case ARI_TYPE_AM:
                CHKERR1(obj->value.as_am);
                ari_am_deinit(obj->value.as_am);
                M_MEMORY_DEL(obj->value.as_am);
                break;
            case ARI_TYPE_TBL:
                CHKERR1(obj->value.as_tbl);
                ari_tbl_deinit(obj->value.as_tbl);
                M_MEMORY_DEL(obj->value.as_tbl);
                break;
            case ARI_TYPE_EXECSET:
                CHKERR1(obj->value.as_execset);
                ari_execset_deinit(obj->value.as_execset);
                M_MEMORY_DEL(obj->value.as_execset);
                break;
            case ARI_TYPE_RPTSET:
                CHKERR1(obj->value.as_rptset);
                ari_rptset_deinit(obj->value.as_rptset);
                M_MEMORY_DEL(obj->value.as_rptset);
                break;
            default:
                // do nothing
                break;
        }
    }
    return 0;
}

void ari_idseg_init(ari_idseg_t *idseg)
{
    memset(idseg, 0, sizeof(ari_idseg_t));
    idseg->form = ARI_IDSEG_NULL;
}

int ari_idseg_init_from_text(ari_idseg_t *idseg, string_t text)
{
    const char *instr = string_get_cstr(text);

    // text IDs are disjoint from numeric IDs
    if ((instr[0] == '-') || isdigit(instr[0]))
    {
        idseg->form   = ARI_IDSEG_INT;
        idseg->as_int = strtoll(instr, NULL, 0);
        string_clear(text);
    }
    else
    {
        idseg->form     = ARI_IDSEG_TEXT;
        string_t *value = &(idseg->as_text);
        string_init_move(*value, text);
    }

    return 0;
}

void ari_idseg_deinit(ari_idseg_t *obj)
{
    CHKVOID(obj);
    if (obj->form == ARI_IDSEG_TEXT)
    {
        string_clear(obj->as_text);
    }
}

void ari_idseg_copy(ari_idseg_t *obj, const ari_idseg_t *src)
{
    CHKVOID(obj);
    CHKVOID(src);
    obj->form = src->form;
    switch (src->form)
    {
        case ARI_IDSEG_NULL:
            break;
        case ARI_IDSEG_TEXT:
            string_set(obj->as_text, src->as_text);
            break;
        case ARI_IDSEG_INT:
            obj->as_int = src->as_int;
            break;
    }
}

size_t ari_idseg_hash(const ari_idseg_t *obj)
{
    CHKRET(obj, 0);

    M_HASH_DECL(accum);
    M_HASH_UP(accum, M_HASH_DEFAULT(obj->form));
    switch (obj->form)
    {
        case ARI_IDSEG_NULL:
            break;
        case ARI_IDSEG_TEXT:
            M_HASH_UP(accum, string_hash(obj->as_text));
            break;
        case ARI_IDSEG_INT:
            M_HASH_UP(accum, M_HASH_DEFAULT(obj->as_int));
            break;
    }
    return M_HASH_FINAL(accum);
}

int ari_idseg_cmp(const ari_idseg_t *left, const ari_idseg_t *right)
{
    CHKRET(left, -2);
    CHKRET(right, -2);

    if (left->form != right->form)
    {
        return M_CMP_DEFAULT(left->form, right->form);
    }
    switch (left->form)
    {
        case ARI_IDSEG_NULL:
            return 0;
        case ARI_IDSEG_INT:
            return M_CMP_DEFAULT(left->as_int, right->as_int);
        case ARI_IDSEG_TEXT:
            return string_cmp(left->as_text, right->as_text);
        default:
            return -2;
    }
}

bool ari_idseg_equal(const ari_idseg_t *left, const ari_idseg_t *right)
{
    CHKFALSE(left);
    CHKFALSE(right);
    if (left->form != right->form)
    {
        return false;
    }
    switch (left->form)
    {
        case ARI_IDSEG_NULL:
            return true;
        case ARI_IDSEG_TEXT:
            return string_equal_p(left->as_text, right->as_text);
        case ARI_IDSEG_INT:
            return left->as_int == right->as_int;
        default:
            return false;
    }
}

void ari_objpath_init(ari_objpath_t *obj)
{
    memset(obj, 0, sizeof(ari_objpath_t));
}

void ari_objpath_deinit(ari_objpath_t *obj)
{
    ari_idseg_deinit(&(obj->ns_id));
    ari_idseg_deinit(&(obj->type_id));
    ari_idseg_deinit(&(obj->obj_id));
}

void ari_objpath_copy(ari_objpath_t *obj, const ari_objpath_t *src)
{
    ari_idseg_copy(&(obj->ns_id), &(src->ns_id));
    ari_idseg_copy(&(obj->type_id), &(src->type_id));
    ari_idseg_copy(&(obj->obj_id), &(src->obj_id));

    obj->has_ari_type = src->has_ari_type;
    obj->ari_type     = src->ari_type;
}

int ari_objpath_derive_type(ari_objpath_t *path)
{
    CHKERR1(path);

    path->has_ari_type = false;
    switch (path->type_id.form)
    {
        case ARI_IDSEG_NULL:
            break;
        case ARI_IDSEG_TEXT:
        {
            ari_type_t  found;
            const char *name = string_get_cstr(path->type_id.as_text);
            if (!ari_type_from_name(&found, name))
            {
                path->has_ari_type = true;
                path->ari_type     = found;
            }
            break;
        }
        case ARI_IDSEG_INT:
        {
            // validate the ID by getting a static name
            if ((path->type_id.as_int < 0) && ari_type_to_name(path->type_id.as_int))
            {
                path->has_ari_type = true;
                path->ari_type     = path->type_id.as_int;
            }
            break;
        }
    }

    return path->has_ari_type ? 0 : 2;
}

int ari_params_deinit(ari_params_t *obj)
{
    CHKERR1(obj);
    switch (obj->state)
    {
        case ARI_PARAMS_NONE:
            break;
        case ARI_PARAMS_AC:
            CHKERR1(obj->as_ac);
            ari_ac_deinit(obj->as_ac);
            M_MEMORY_DEL(obj->as_ac);
            break;
        case ARI_PARAMS_AM:
            CHKERR1(obj->as_am);
            ari_am_deinit(obj->as_am);
            M_MEMORY_DEL(obj->as_am);
            break;
    }
    return 0;
}

int ari_params_copy(ari_params_t *obj, const ari_params_t *src)
{
    CHKERR1(obj);
    CHKERR1(src);

    ari_params_deinit(obj);

    switch (src->state)
    {
        case ARI_PARAMS_NONE:
            break;
        case ARI_PARAMS_AC:
        {
            ari_ac_t *ctr = M_MEMORY_ALLOC(ari_ac_t);
            obj->state    = src->state;
            obj->as_ac    = ctr;
            ari_list_init_set(ctr->items, src->as_ac->items);
        }
        break;
        case ARI_PARAMS_AM:
        {
            ari_am_t *ctr = M_MEMORY_ALLOC(ari_am_t);
            obj->state    = src->state;
            obj->as_am    = ctr;
            ari_tree_init_set(ctr->items, src->as_am->items);
        }
        break;
    }

    return 0;
}

void ari_ref_deinit(ari_ref_t *obj)
{
    CHKVOID(obj);
    ari_objpath_deinit(&(obj->objpath));
    ari_params_deinit(&(obj->params));
}

static int ari_ref_copy(ari_ref_t *obj, const ari_ref_t *src)
{
    ari_objpath_copy(&(obj->objpath), &(src->objpath));
    ari_params_copy(&(obj->params), &(src->params));
    return 0;
}

/** Reset the state of a value struct.
 *
 * @param ari The struct to reset.
 */
static void ari_reset(ari_t *ari)
{
    memset(ari, 0, sizeof(ari_t));
}

static void ari_copy_shallow(ari_t *ari, const ari_t *src)
{
    memcpy(ari, src, sizeof(ari_t));
}

static int ari_copy_deep(ari_t *ari, const ari_t *src)
{
    int retval = 0;
    if (src->is_ref)
    {
        retval = ari_ref_copy(ari_init_objref(ari), &(src->as_ref));
        CHKERRVAL(retval);
    }
    else
    {
        retval = ari_lit_copy(ari_init_lit(ari), &(src->as_lit));
        CHKERRVAL(retval);
    }
    return retval;
}

int ari_lit_copy(ari_lit_t *lit, const ari_lit_t *src)
{
    lit->has_ari_type = src->has_ari_type;
    lit->ari_type     = src->ari_type;
    lit->prim_type    = src->prim_type;

    switch (src->prim_type)
    {
        case ARI_PRIM_TSTR:
        case ARI_PRIM_BSTR:
            if (cace_data_init_set(&(lit->value.as_data), &(src->value.as_data)))
            {
                return 2;
            }
            break;
        case ARI_PRIM_OTHER:
            // copied below based on ari_type
            break;
        default:
            // simple copy
            lit->value = src->value;
            break;
    }

    if (src->has_ari_type)
    {
        switch (src->ari_type)
        {
            case ARI_TYPE_TP:
            case ARI_TYPE_TD:
                lit->value = src->value;
                break;
            case ARI_TYPE_AC:
            {
                ari_ac_t *ctr    = M_MEMORY_ALLOC(ari_ac_t);
                lit->value.as_ac = ctr;
                ari_list_init_set(ctr->items, src->value.as_ac->items);
                break;
            }
            case ARI_TYPE_AM:
            {
                ari_am_t *ctr    = M_MEMORY_ALLOC(ari_am_t);
                lit->value.as_am = ctr;
                ari_tree_init_set(ctr->items, src->value.as_am->items);
                break;
            }
            case ARI_TYPE_TBL:
            {
                ari_tbl_t *ctr    = M_MEMORY_ALLOC(ari_tbl_t);
                lit->value.as_tbl = ctr;
                ctr->ncols        = src->value.as_tbl->ncols;
                ari_array_init_set(ctr->items, src->value.as_tbl->items);
                break;
            }
            case ARI_TYPE_EXECSET:
            {
                ari_execset_t *ctr    = M_MEMORY_ALLOC(ari_execset_t);
                lit->value.as_execset = ctr;
                ari_init_copy(&(ctr->nonce), &(src->value.as_execset->nonce));
                ari_list_init_set(ctr->targets, src->value.as_execset->targets);
                break;
            }
            case ARI_TYPE_RPTSET:
            {
                ari_rptset_t *ctr    = M_MEMORY_ALLOC(ari_rptset_t);
                lit->value.as_rptset = ctr;
                ari_init_copy(&(ctr->nonce), &(src->value.as_rptset->nonce));
                ari_init_copy(&(ctr->reftime), &(src->value.as_rptset->reftime));
                ari_report_list_init_set(ctr->reports, src->value.as_rptset->reports);
                break;
            }
            default:
                break;
        }
    }

    return 0;
}

static void ari_deinit_parts(ari_t *ari)
{
    if (ari->is_ref)
    {
        ari_ref_deinit(&(ari->as_ref));
    }
    else
    {
        ari_lit_deinit(&(ari->as_lit));
    }
}

void ari_init(ari_t *ari)
{
    CHKVOID(ari);
    ari_reset(ari);
}

ari_lit_t *ari_init_lit(ari_t *ari)
{
    CHKNULL(ari);
    ari_reset(ari);
    ari->is_ref = false;
    return &(ari->as_lit);
}

ari_ref_t *ari_init_objref(ari_t *ari)
{
    CHKNULL(ari);
    ari_reset(ari);
    ari->is_ref = true;
    return &(ari->as_ref);
}

int ari_init_copy(ari_t *ari, const ari_t *src)
{
    CHKERR1(ari);
    CHKERR1(src);
    if (ari == src)
    {
        return 0;
    }

    ari_copy_deep(ari, src);
    return 0;
}

int ari_init_move(ari_t *ari, ari_t *src)
{
    CHKERR1(ari);
    CHKERR1(src);

    if (ari == src)
    {
        return 0;
    }

    ari_copy_shallow(ari, src);
    // reset the state of the src (not deinit)
    ari_reset(src);
    return 0;
}

int ari_deinit(ari_t *ari)
{
    CHKERR1(ari);

    ari_deinit_parts(ari);
    ari_reset(ari);
    return 0;
}

int ari_set_copy(ari_t *ari, const ari_t *src)
{
    CHKERR1(ari);
    CHKERR1(src);
    if (ari == src)
    {
        return 0;
    }

    ari_deinit_parts(ari);
    ari_copy_deep(ari, src);
    return 0;
}

int ari_set_move(ari_t *ari, ari_t *src)
{
    CHKERR1(ari);
    CHKERR1(src);
    if (ari == src)
    {
        return 0;
    }

    ari_deinit_parts(ari);
    ari_copy_shallow(ari, src);
    // reset the state of the src (not deinit)
    ari_reset(src);
    return 0;
}
