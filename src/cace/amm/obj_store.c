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
#include "cace/util/logging.h"

void cace_amm_obj_store_init(cace_amm_obj_store_t *store)
{
    cace_amm_obj_ns_list_init(store->ns_list);
    cace_amm_obj_ns_by_enum_init(store->ns_by_enum);
    cace_amm_obj_ns_by_name_init(store->ns_by_name);
}

void cace_amm_obj_store_deinit(cace_amm_obj_store_t *store)
{
    cace_amm_obj_ns_by_name_clear(store->ns_by_name);
    cace_amm_obj_ns_by_enum_clear(store->ns_by_enum);
    cace_amm_obj_ns_list_clear(store->ns_list);
}

cace_amm_obj_ns_t *cace_amm_obj_store_add_ns(cace_amm_obj_store_t *store, const char *name, bool has_enum,
                                             int64_t intenum)
{
    cace_amm_obj_ns_t **found = cace_amm_obj_ns_by_name_get(store->ns_by_name, name);
    if (found)
    {
        CACE_LOG_WARNING("ignoring duplicate object name: %s", name);
        return NULL;
    }
    if (has_enum)
    {
        found = cace_amm_obj_ns_by_enum_get(store->ns_by_enum, intenum);
        if (found)
        {
            CACE_LOG_WARNING("ignoring duplicate object enum: %" PRId64, intenum);
            return NULL;
        }
    }

    cace_amm_obj_ns_t *ns = cace_amm_obj_ns_list_push_back_new(store->ns_list);
    string_set_str(ns->name, name);
    ns->has_enum = has_enum;
    ns->intenum  = intenum;

    cace_amm_obj_ns_by_name_set_at(store->ns_by_name, name, ns);
    if (has_enum)
    {
        cace_amm_obj_ns_by_enum_set_at(store->ns_by_enum, intenum, ns);
    }

    return ns;
}

cace_amm_obj_ns_t *cace_amm_obj_store_find_ns_name(const cace_amm_obj_store_t *store, const char *name)
{
    cace_amm_obj_ns_t **found = cace_amm_obj_ns_by_name_get(store->ns_by_name, name);
    if (!found)
    {
        return NULL;
    }
    return *found;
}

cace_amm_obj_ns_t *cace_amm_obj_store_find_ns_enum(const cace_amm_obj_store_t *store, int64_t intenum)
{
    cace_amm_obj_ns_t **found = cace_amm_obj_ns_by_enum_get(store->ns_by_enum, intenum);
    if (!found)
    {
        return NULL;
    }
    return *found;
}
