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
#include "obj_org.h"
#include "cace/util/logging.h"
#include "cace/util/defs.h"

void cace_amm_obj_org_init(cace_amm_obj_org_t *org)
{
    cace_amm_idseg_val_init(&(org->org_id));
    cace_amm_obj_ns_by_enum_init(org->ns_by_enum);
    cace_amm_obj_ns_by_name_init(org->ns_by_name);
}

void cace_amm_obj_org_deinit(cace_amm_obj_org_t *org)
{
    cace_amm_obj_ns_by_name_clear(org->ns_by_name);
    cace_amm_obj_ns_by_enum_clear(org->ns_by_enum);
    cace_amm_idseg_val_deinit(&(org->org_id));
}

cace_amm_obj_ns_t *cace_amm_obj_org_find_ns_name(const cace_amm_obj_org_t *org, const char *name)
{
    cace_amm_obj_ns_t **found = cace_amm_obj_ns_by_name_get(org->ns_by_name, name);
    if (!found)
    {
        return NULL;
    }
    return *found;
}

cace_amm_obj_ns_t *cace_amm_obj_org_find_ns_enum(const cace_amm_obj_org_t *org, cace_ari_int_id_t intenum)
{
    cace_amm_obj_ns_t **found = cace_amm_obj_ns_by_enum_get(org->ns_by_enum, intenum);
    if (!found)
    {
        return NULL;
    }
    return *found;
}
