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
#include "objpat.h"
#include "cace/util/logging.h"
#include "cace/util/defs.h"

static int cace_amm_objpat_set_from_single(cace_amm_objpat_set_t obj, const cace_ari_t *val)
{
    const cace_ari_objpat_t *pat = cace_ari_cget_objpat(val);
    if (!pat)
    {
        CACE_LOG_ERR("Given non-OBJPAT value");
        return 2;
    }
    else
    {
        cace_amm_objpat_set_push_back(obj, *pat);
        return 0;
    }
}

int cace_amm_objpat_set_from_value(cace_amm_objpat_set_t obj, const cace_ari_t *val)
{
    cace_amm_objpat_set_reset(obj);

    const cace_ari_ac_t *as_ac = cace_ari_cget_ac(val);
    if (as_ac)
    {
        cace_ari_list_it_t sub_it;
        for (cace_ari_list_it(sub_it, as_ac->items); !cace_ari_list_end_p(sub_it); cace_ari_list_next(sub_it))
        {
            const cace_ari_t *sub_item = cace_ari_list_cref(sub_it);
            cace_amm_objpat_set_from_single(obj, sub_item);
        }
        return 0;
    }
    else
    {
        return cace_amm_objpat_set_from_single(obj, val);
    }
}

static bool cace_amm_objpat_part_idseg_match(const cace_ari_objpat_part_t *part, const cace_amm_idseg_val_t *id)
{
    const cace_util_range_int64_t *range_int64 = NULL;
    const m_string_t              *text        = NULL;
    if (cace_ari_objpat_part_cget_special(*part))
    {
        return true;
    }
    else if ((range_int64 = cace_ari_objpat_part_cget_range_int64(*part)))
    {
        if (id->has_intenum)
        {
            return cace_util_range_int64_contains(*range_int64, id->intenum);
        }
    }
    else if ((text = cace_ari_objpat_part_cget_text(*part)))
    {
        return m_string_equal_p(*text, id->name);
    }
    return false;
}

static bool cace_amm_objpat_part_type_match(const cace_ari_objpat_part_t *part, cace_ari_type_t obj_type)
{
    const cace_util_range_int64_t *range_int64 = NULL;
    const m_string_t              *text        = NULL;
    if (cace_ari_objpat_part_cget_special(*part))
    {
        return true;
    }
    else if ((range_int64 = cace_ari_objpat_part_cget_range_int64(*part)))
    {
        return cace_util_range_int64_contains(*range_int64, obj_type);
    }
    else if ((text = cace_ari_objpat_part_cget_text(*part)))
    {
        return m_string_equal_cstr_p(*text, cace_ari_type_to_name(obj_type));
    }
    return false;
}

bool cace_amm_objpat_set_match(const cace_amm_objpat_set_t obj, const cace_amm_lookup_t *deref)
{
    if (!deref || !(deref->ns) || !(deref->obj))
    {
        return false;
    }

    cace_amm_objpat_set_it_t it;
    for (cace_amm_objpat_set_it(it, obj); !cace_amm_objpat_set_end_p(it); cace_amm_objpat_set_next(it))
    {
        const cace_ari_objpat_t *pat = cace_amm_objpat_set_cref(it);

        bool is_match = (cace_amm_objpat_part_idseg_match(&pat->org_pat, &deref->ns->org_id)
                         && cace_amm_objpat_part_idseg_match(&pat->model_pat, &deref->ns->model_id)
                         && cace_amm_objpat_part_type_match(&pat->org_pat, deref->obj_type)
                         && cace_amm_objpat_part_idseg_match(&pat->obj_pat, &deref->obj->obj_id));
        // any match stops early
        if (is_match)
        {
            return true;
        }
    }
    return false;
}
