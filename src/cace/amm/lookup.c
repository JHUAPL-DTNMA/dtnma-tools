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
#include "lookup.h"
#include "cace/util/defs.h"

void cace_amm_lookup_init(cace_amm_lookup_t *res)
{
    CHKVOID(res);
    res->ns  = NULL;
    res->obj = NULL;
    cace_ari_itemized_init(&(res->aparams));
}

void cace_amm_lookup_deinit(cace_amm_lookup_t *res)
{
    CHKVOID(res);
    cace_ari_itemized_deinit(&(res->aparams));
    res->ns       = NULL;
    res->obj      = NULL;
    res->obj_type = CACE_ARI_TYPE_NULL;
}

void cace_amm_lookup_init_set(cace_amm_lookup_t *res, const cace_amm_lookup_t *src)
{
    CHKVOID(res);
    CHKVOID(src);
    res->ns       = src->ns;
    res->obj      = src->obj;
    res->obj_type = src->obj_type;
    cace_ari_itemized_init_set(&(res->aparams), &(src->aparams));
}

void cace_amm_lookup_init_move(cace_amm_lookup_t *res, cace_amm_lookup_t *src)
{
    CHKVOID(res);
    CHKVOID(src);
    res->ns       = src->ns;
    src->ns       = NULL;
    res->obj      = src->obj;
    src->obj      = NULL;
    res->obj_type = src->obj_type;
    src->obj_type = CACE_ARI_TYPE_NULL;
    cace_ari_itemized_init_move(&(res->aparams), &(src->aparams));
}

void cace_amm_lookup_set(cace_amm_lookup_t *res, const cace_amm_lookup_t *src)
{
    cace_amm_lookup_deinit(res);
    cace_amm_lookup_init_set(res, src);
}

void cace_amm_lookup_set_move(cace_amm_lookup_t *res, cace_amm_lookup_t *src)
{
    cace_amm_lookup_deinit(res);
    cace_amm_lookup_init_move(res, src);
}

int cace_amm_lookup_deref(cace_amm_lookup_t *res, const cace_amm_obj_store_t *store, const cace_ari_t *ref)
{
    CHKERR1(res);
    CHKERR1(store);
    CHKERR1(ref);
    CHKERR1(ref->is_ref);

    // reset state
    res->ns       = NULL;
    res->obj      = NULL;
    res->obj_type = CACE_ARI_TYPE_NULL;
    cace_ari_itemized_reset(&(res->aparams));

    const cace_ari_objpath_t *path = &(ref->as_ref.objpath);
    // no possible match in this store
    if (!(path->has_ari_type))
    {
        return 2;
    }

    cace_amm_obj_org_t *org = NULL;
    switch (path->org_id.form)
    {
        case CACE_ARI_IDSEG_INT:
            org = cace_amm_obj_store_find_org_enum(store, path->org_id.as_int);
            break;
        case CACE_ARI_IDSEG_TEXT:
            org = cace_amm_obj_store_find_org_name(store, m_string_get_cstr(path->org_id.as_text));
            break;
        default:
            break;
    }
    if (!org)
    {
        return 3;
    }
    switch (path->model_id.form)
    {
        case CACE_ARI_IDSEG_INT:
            res->ns = cace_amm_obj_org_find_ns_enum(org, path->model_id.as_int);
            break;
        case CACE_ARI_IDSEG_TEXT:
            res->ns = cace_amm_obj_org_find_ns_name(org, m_string_get_cstr(path->model_id.as_text));
            break;
        default:
            break;
    }
    if (!(res->ns))
    {
        return 4;
    }

    // FIXME handle model_rev

    switch (path->obj_id.form)
    {
        case CACE_ARI_IDSEG_INT:
            res->obj = cace_amm_obj_ns_find_obj_enum(res->ns, path->ari_type, path->obj_id.as_int);
            break;
        case CACE_ARI_IDSEG_TEXT:
            res->obj = cace_amm_obj_ns_find_obj_name(res->ns, path->ari_type, string_get_cstr(path->obj_id.as_text));
            break;
        default:
            break;
    }
    if (!(res->obj))
    {
        return 6;
    }

    res->obj_type = path->ari_type;

    int pop_res = cace_amm_actual_param_set_populate(&(res->aparams), res->obj->fparams, &(ref->as_ref.params));
    if (pop_res)
    {
        return 7 + pop_res;
    }

    return 0;
}
