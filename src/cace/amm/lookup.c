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
#include "lookup.h"
#include "cace/util/defs.h"

void cace_amm_lookup_init(cace_amm_lookup_t *res)
{
    res->ref = NULL;
    res->ns  = NULL;
    res->obj = NULL;
    cace_amm_actual_param_set_init(&(res->aparams));
}

void cace_amm_lookup_deinit(cace_amm_lookup_t *res)
{
    cace_amm_actual_param_set_deinit(&(res->aparams));
    res->ns  = NULL;
    res->obj = NULL;
    res->ref = NULL;
}

int cace_amm_lookup_deref(cace_amm_lookup_t *res, const cace_amm_obj_store_t *store, const ari_t *ref)
{
    CHKERR1(res);
    CHKERR1(store);
    CHKERR1(ref);
    CHKERR1(ref->is_ref);

    // reset state
    res->ref = ref;
    res->ns  = NULL;
    res->obj = NULL;
    cace_amm_actual_param_set_reset(&(res->aparams));

    const ari_objpath_t *path = &(ref->as_ref.objpath);
    // no possible match in this store
    if (!(path->has_ari_type))
    {
        return 2;
    }

    switch (path->ns_id.form)
    {
        case ARI_IDSEG_INT:
            res->ns = cace_amm_obj_store_find_ns_enum(store, path->ns_id.as_int);
            break;
        case ARI_IDSEG_TEXT:
            res->ns = cace_amm_obj_store_find_ns_name(store, string_get_cstr(path->ns_id.as_text));
            break;
        default:
            break;
    }
    if (!(res->ns))
    {
        return 3;
    }

    switch (path->obj_id.form)
    {
        case ARI_IDSEG_INT:
            res->obj = cace_amm_obj_ns_find_obj_enum(res->ns, path->ari_type, path->obj_id.as_int);
            break;
        case ARI_IDSEG_TEXT:
            res->obj = cace_amm_obj_ns_find_obj_name(res->ns, path->ari_type, string_get_cstr(path->obj_id.as_text));
            break;
        default:
            break;
    }
    if (!(res->obj))
    {
        return 4;
    }

    int pop_res = cace_amm_actual_param_set_populate(&(res->aparams), res->obj->fparams, &(ref->as_ref.params));
    if (pop_res)
    {
        return 5 + pop_res;
    }

    return 0;
}
