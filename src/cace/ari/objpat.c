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
