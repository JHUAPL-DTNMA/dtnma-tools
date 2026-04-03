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
/** @file
 * @ingroup ari
 * This file contains the definitions for reference patterns.
 */
#include "objpat.h"

int cace_ari_objpat_part_cmp(const cace_ari_objpat_part_t left, const cace_ari_objpat_part_t right)
{
    if (cace_ari_objpat_part_empty_p(left))
    {
        if (cace_ari_objpat_part_empty_p(right))
        {
            return 0;
        }
        else
        {
            return -1;
        }
    }
    if (cace_ari_objpat_part_cget_special(left))
    {
        if (cace_ari_objpat_part_cget_special(right))
        {
            return 0;
        }
        else
        {
            return -1;
        }
    }

    const cace_util_range_int64_t *lt_range, *rt_range;
    if ((lt_range = cace_ari_objpat_part_cget_range_int64(left)))
    {
        if ((rt_range = cace_ari_objpat_part_cget_range_int64(right)))
        {
            return cace_util_range_int64_cmp(*lt_range, *rt_range);
        }
        else if (cace_ari_objpat_part_cget_special(right))
        {
            return 1;
        }
        else
        {
            return -1;
        }
    }

    const m_string_t *lt_text, *rt_text;
    if ((lt_text = cace_ari_objpat_part_cget_text(left)))
    {
        if ((rt_text = cace_ari_objpat_part_cget_text(right)))
        {
            return m_string_cmp(*lt_text, *rt_text);
        }
        else if ((cace_ari_objpat_part_cget_special(right) || cace_ari_objpat_part_cget_range_int64(right)))

        {
            return 1;
        }
        else
        {
            return -1;
        }
    }

    // Not really reachable
    return 0;
}

void cace_ari_objpat_init(cace_ari_objpat_t *obj)
{
    CHKVOID(obj);
    cace_ari_objpat_part_init(obj->org_pat);
    cace_ari_objpat_part_init(obj->model_pat);
    cace_ari_objpat_part_init(obj->type_pat);
    cace_ari_objpat_part_init(obj->obj_pat);
}

void cace_ari_objpat_init_set(cace_ari_objpat_t *obj, const cace_ari_objpat_t *src)
{
    CHKVOID(obj);
    CHKVOID(src);
    if (obj == src)
    {
        return;
    }
    cace_ari_objpat_part_init_set(obj->org_pat, src->org_pat);
    cace_ari_objpat_part_init_set(obj->model_pat, src->model_pat);
    cace_ari_objpat_part_init_set(obj->type_pat, src->type_pat);
    cace_ari_objpat_part_init_set(obj->obj_pat, src->obj_pat);
}

void cace_ari_objpat_deinit(cace_ari_objpat_t *obj)
{
    CHKVOID(obj);
    cace_ari_objpat_part_clear(obj->org_pat);
    cace_ari_objpat_part_clear(obj->model_pat);
    cace_ari_objpat_part_clear(obj->type_pat);
    cace_ari_objpat_part_clear(obj->obj_pat);
}
void cace_ari_objpat_set(cace_ari_objpat_t *obj, const cace_ari_objpat_t *src)
{
    CHKVOID(obj);
    CHKVOID(src);
    if (obj == src)
    {
        return;
    }
    cace_ari_objpat_part_set(obj->org_pat, src->org_pat);
    cace_ari_objpat_part_set(obj->model_pat, src->model_pat);
    cace_ari_objpat_part_set(obj->type_pat, src->type_pat);
    cace_ari_objpat_part_set(obj->obj_pat, src->obj_pat);
}

int cace_ari_objpat_cmp(const cace_ari_objpat_t *left, const cace_ari_objpat_t *right)
{
    // compare in order
    int res = cace_ari_objpat_part_cmp(left->org_pat, right->org_pat);
    if (res)
    {
        return res;
    }
    res = cace_ari_objpat_part_cmp(left->model_pat, right->model_pat);
    if (res)
    {
        return res;
    }
    res = cace_ari_objpat_part_cmp(left->type_pat, right->type_pat);
    if (res)
    {
        return res;
    }
    res = cace_ari_objpat_part_cmp(left->obj_pat, right->obj_pat);
    return res;
}

bool cace_ari_objpat_equal(const cace_ari_objpat_t *left, const cace_ari_objpat_t *right)
{
    CHKFALSE(left);
    CHKFALSE(right);
    return (cace_ari_objpat_part_equal_p(left->org_pat, right->org_pat)
            && cace_ari_objpat_part_equal_p(left->model_pat, right->model_pat)
            && cace_ari_objpat_part_equal_p(left->type_pat, right->type_pat)
            && cace_ari_objpat_part_equal_p(left->obj_pat, right->obj_pat));
}

size_t cace_ari_objpat_hash(const cace_ari_objpat_t *obj)
{
    CHKRET(obj, 0);

    M_HASH_DECL(accum);

    M_HASH_UP(accum, cace_ari_objpat_part_hash(obj->org_pat));
    M_HASH_UP(accum, cace_ari_objpat_part_hash(obj->model_pat));
    M_HASH_UP(accum, cace_ari_objpat_part_hash(obj->type_pat));
    M_HASH_UP(accum, cace_ari_objpat_part_hash(obj->obj_pat));

    accum = M_HASH_FINAL(accum);
    return accum;
}

cace_ari_objpat_t *cace_ari_lit_init_objpat(cace_ari_lit_t *lit)
{
    CHKNULL(lit);
    lit->has_ari_type = true;
    lit->ari_type     = CACE_ARI_TYPE_OBJPAT;
    lit->prim_type    = CACE_ARI_PRIM_OTHER;

    cace_ari_objpat_t *pat = CACE_MALLOC(sizeof(cace_ari_objpat_t));
    cace_ari_objpat_init(pat);

    lit->value.as_objpat = pat;

    return pat;
}
