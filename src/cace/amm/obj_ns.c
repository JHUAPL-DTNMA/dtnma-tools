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
#include "obj_ns.h"
#include "cace/ari/text.h"
#include "cace/util/logging.h"

void cace_amm_obj_ns_ctr_init(cace_amm_obj_ns_ctr_t *obj)
{
    cace_amm_obj_desc_list_init(obj->obj_list);
    cace_amm_obj_desc_by_enum_init(obj->obj_by_enum);
    cace_amm_obj_desc_by_name_init(obj->obj_by_name);
}

void cace_amm_obj_ns_ctr_deinit(cace_amm_obj_ns_ctr_t *obj)
{
    cace_amm_obj_desc_by_name_clear(obj->obj_by_name);
    cace_amm_obj_desc_by_enum_clear(obj->obj_by_enum);
    cace_amm_obj_desc_list_clear(obj->obj_list);
}

void cace_amm_obj_ns_init(cace_amm_obj_ns_t *ns)
{
    m_string_init(ns->name);
    m_string_init(ns->revision);
    string_tree_set_init(ns->feature_supp);
    cace_amm_obj_ns_ctr_dict_init(ns->object_types);
}

void cace_amm_obj_ns_deinit(cace_amm_obj_ns_t *ns)
{
    cace_amm_obj_ns_ctr_dict_clear(ns->object_types);
    string_tree_set_clear(ns->feature_supp);
    m_string_clear(ns->revision);
    m_string_clear(ns->name);
}

cace_amm_obj_desc_t *cace_amm_obj_ns_add_obj(cace_amm_obj_ns_t *ns, ari_type_t obj_type, const cace_amm_obj_id_t obj_id)
{
    if (cace_log_is_enabled_for(LOG_DEBUG))
    {
        ari_t ref = ARI_INIT_UNDEFINED;
        ari_set_objref_path_textid(&ref, string_get_cstr(ns->name), obj_type, obj_id.name);

        string_t buf;
        string_init(buf);
        ari_text_encode(buf, &ref, ARI_TEXT_ENC_OPTS_DEFAULT);
        CACE_LOG_DEBUG("registering object at %s", string_get_cstr(buf));
        string_clear(buf);

        ari_deinit(&ref);
    }
    cace_amm_obj_ns_ctr_t *ctr = cace_amm_obj_ns_ctr_dict_safe_get(ns->object_types, obj_type);

    cace_amm_obj_desc_t **found = cace_amm_obj_desc_by_name_get(ctr->obj_by_name, obj_id.name);
    if (found)
    {
        CACE_LOG_WARNING("ignoring duplicate object name: %s", obj_id.name);
        return NULL;
    }
    if (obj_id.has_enum)
    {
        found = cace_amm_obj_desc_by_enum_get(ctr->obj_by_enum, obj_id.intenum);
        if (found)
        {
            CACE_LOG_WARNING("ignoring duplicate object enum: %" PRId64, obj_id.intenum);
            return NULL;
        }
    }

    cace_amm_obj_desc_t *obj = cace_amm_obj_desc_list_push_back_new(ctr->obj_list);
    string_set_str(obj->name, obj_id.name);
    obj->has_enum = obj_id.has_enum;
    obj->intenum  = obj_id.intenum;

    cace_amm_obj_desc_by_name_set_at(ctr->obj_by_name, string_get_cstr(obj->name), obj);
    if (obj->has_enum)
    {
        cace_amm_obj_desc_by_enum_set_at(ctr->obj_by_enum, obj->intenum, obj);
    }

    return obj;
}

cace_amm_obj_desc_t *cace_amm_obj_ns_find_obj_name(const cace_amm_obj_ns_t *ns, ari_type_t obj_type, const char *name)
{
    const cace_amm_obj_ns_ctr_t *ctr = cace_amm_obj_ns_ctr_dict_get(ns->object_types, obj_type);
    if (!ctr)
    {
        return NULL;
    }
    cace_amm_obj_desc_t **found = cace_amm_obj_desc_by_name_get(ctr->obj_by_name, name);
    if (!found)
    {
        return NULL;
    }
    return *found;
}

cace_amm_obj_desc_t *cace_amm_obj_ns_find_obj_enum(const cace_amm_obj_ns_t *ns, ari_type_t obj_type, int64_t intenum)
{
    const cace_amm_obj_ns_ctr_t *ctr = cace_amm_obj_ns_ctr_dict_get(ns->object_types, obj_type);
    if (!ctr)
    {
        return NULL;
    }
    cace_amm_obj_desc_t **found = cace_amm_obj_desc_by_enum_get(ctr->obj_by_enum, intenum);
    if (!found)
    {
        return NULL;
    }
    return *found;
}
