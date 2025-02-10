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
#include "obj_store.h"
#include "cace/ari/text_util.h"
#include "cace/util/logging.h"
#include "cace/util/defs.h"

void cace_amm_obj_store_init(cace_amm_obj_store_t *store)
{
    cace_amm_obj_ns_list_init(store->ns_list);
    cace_amm_obj_org_list_init(store->org_list);
    cace_amm_obj_org_by_enum_init(store->org_by_enum);
    cace_amm_obj_org_by_name_init(store->org_by_name);
}

void cace_amm_obj_store_deinit(cace_amm_obj_store_t *store)
{
    cace_amm_obj_org_by_name_clear(store->org_by_name);
    cace_amm_obj_org_by_enum_clear(store->org_by_enum);
    cace_amm_obj_org_list_clear(store->org_list);
    cace_amm_obj_ns_list_clear(store->ns_list);
}

cace_amm_obj_ns_t *cace_amm_obj_store_add_ns(cace_amm_obj_store_t *store, const cace_amm_idseg_ref_t org_id,
                                             const cace_amm_idseg_ref_t model_id, const char *revision)
{
    CHKNULL(org_id.name && org_id.has_intenum);
    CHKNULL(model_id.name && model_id.has_intenum);
    CHKNULL(revision);

    // Get or add the organization container
    cace_amm_obj_org_t *org = NULL;
    {
        cace_amm_obj_org_t **org_found = cace_amm_obj_org_by_name_get(store->org_by_name, org_id.name);
        if (org_found)
        {
            org = *org_found;
        }
        else
        {
            cace_amm_obj_org_ptr_t *org_ptr = cace_amm_obj_org_ptr_new();

            org = cace_amm_obj_org_ptr_ref(org_ptr);
            cace_amm_idseg_val_set_fromref(&org->org_id, &org_id);

            cace_amm_obj_org_list_push_back(store->org_list, org_ptr);

            cace_amm_obj_org_by_name_set_at(store->org_by_name, m_string_get_cstr(org->org_id.name), org);
            if (org->org_id.has_intenum)
            {
                cace_amm_obj_org_by_enum_set_at(store->org_by_enum, org->org_id.intenum, org);
            }
        }
    }

    {
        // Precondition check for duplicate
        cace_amm_obj_ns_t **ns_found = cace_amm_obj_ns_by_name_get(org->ns_by_name, model_id.name);
        if (ns_found)
        {
            CACE_LOG_WARNING("ignoring duplicate model name: %s", model_id.name);
            return NULL;
        }

        if (model_id.has_intenum)
        {
            ns_found = cace_amm_obj_ns_by_enum_get(org->ns_by_enum, model_id.intenum);
            if (ns_found)
            {
                CACE_LOG_WARNING("ignoring duplicate model enum: %" PRId64, model_id.intenum);
                return NULL;
            }
        }
    }

    cace_amm_obj_ns_ptr_t **ns_ptr = cace_amm_obj_ns_list_push_back_new(store->ns_list);
    cace_amm_obj_ns_t      *ns     = cace_amm_obj_ns_ptr_ref(*ns_ptr);

    cace_amm_idseg_val_set_fromref(&ns->org_id, &org_id);
    cace_amm_idseg_val_set_fromref(&ns->model_id, &model_id);
    cace_ari_date_from_text(&ns->model_rev, revision);
    if (!(ns->model_rev.valid))
    {
        CACE_LOG_ERR("Failed to decode date text: %s", revision);
    }

    cace_amm_obj_ns_by_name_set_at(org->ns_by_name, model_id.name, ns);
    if (model_id.has_intenum)
    {
        cace_amm_obj_ns_by_enum_set_at(org->ns_by_enum, model_id.intenum, ns);
    }

    return ns;
}

cace_amm_obj_org_t *cace_amm_obj_store_find_org_name(const cace_amm_obj_store_t *store, const char *name)
{
    cace_amm_obj_org_t **found = cace_amm_obj_org_by_name_get(store->org_by_name, name);
    if (!found)
    {
        return NULL;
    }
    return *found;
}

cace_amm_obj_org_t *cace_amm_obj_store_find_org_enum(const cace_amm_obj_store_t *store, cace_ari_int_id_t intenum)
{
    cace_amm_obj_org_t **found = cace_amm_obj_org_by_enum_get(store->org_by_enum, intenum);
    if (!found)
    {
        return NULL;
    }
    return *found;
}
