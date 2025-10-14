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
#include "obj_ns.h"
#include "obj_org.h"
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
    cace_amm_idseg_val_init(&(ns->org_id));
    cace_amm_idseg_val_init(&(ns->model_id));
    cace_ari_date_init(&ns->model_rev);
    string_tree_set_init(ns->feature_supp);
    cace_amm_obj_ns_ctr_dict_init(ns->object_types);
    ns->obsolete = false;
}

void cace_amm_obj_ns_deinit(cace_amm_obj_ns_t *ns)
{
    cace_amm_obj_ns_ctr_dict_clear(ns->object_types);
    string_tree_set_clear(ns->feature_supp);
    cace_ari_date_deinit(&ns->model_rev);
    cace_amm_idseg_val_deinit(&(ns->model_id));
    cace_amm_idseg_val_deinit(&(ns->org_id));
}

cace_amm_obj_desc_t *cace_amm_obj_ns_add_obj(cace_amm_obj_ns_t *ns, cace_ari_type_t obj_type,
                                             const cace_amm_idseg_ref_t obj_id)
{
    if (cace_log_is_enabled_for(LOG_DEBUG))
    {
        cace_ari_t ref = CACE_ARI_INIT_UNDEFINED;
        cace_ari_objpath_set_textid(&(cace_ari_set_objref(&ref)->objpath), m_string_get_cstr(ns->org_id.name),
                                    m_string_get_cstr(ns->model_id.name), obj_type, obj_id.name);

        m_string_t buf;
        m_string_init(buf);
        cace_ari_text_encode(buf, &ref, CACE_ARI_TEXT_ENC_OPTS_DEFAULT);
        CACE_LOG_DEBUG("registering object at %s", m_string_get_cstr(buf));
        m_string_clear(buf);

        cace_ari_deinit(&ref);
    }
    cace_amm_obj_ns_ctr_t *ctr =
        cace_amm_obj_ns_ctr_ptr_ref(*cace_amm_obj_ns_ctr_dict_safe_get(ns->object_types, obj_type));

    cace_amm_obj_desc_t **found = cace_amm_obj_desc_by_name_get(ctr->obj_by_name, obj_id.name);
    if (found)
    {
        CACE_LOG_WARNING("ignoring duplicate object name: %s", obj_id.name);
        return NULL;
    }
    if (obj_id.has_intenum)
    {
        found = cace_amm_obj_desc_by_enum_get(ctr->obj_by_enum, obj_id.intenum);
        if (found)
        {
            CACE_LOG_WARNING("ignoring duplicate object enum: %" PRId64, obj_id.intenum);
            return NULL;
        }
    }

    cace_amm_obj_desc_ptr_t **ptr = cace_amm_obj_desc_list_push_back_new(ctr->obj_list);
    cace_amm_obj_desc_t      *obj = cace_amm_obj_desc_ptr_ref(*ptr);
    cace_amm_idseg_val_set_fromref(&obj->obj_id, &obj_id);

    cace_amm_obj_desc_by_name_set_at(ctr->obj_by_name, m_string_get_cstr(obj->obj_id.name), obj);
    if (obj->obj_id.has_intenum)
    {
        cace_amm_obj_desc_by_enum_set_at(ctr->obj_by_enum, obj->obj_id.intenum, obj);
    }

    return obj;
}

cace_amm_obj_desc_t *cace_amm_obj_ns_find_obj_name(const cace_amm_obj_ns_t *ns, cace_ari_type_t obj_type,
                                                   const char *name)
{
    cace_amm_obj_ns_ctr_ptr_t *const *ctr_ptr = cace_amm_obj_ns_ctr_dict_cget(ns->object_types, obj_type);
    if (!ctr_ptr)
    {
        return NULL;
    }
    const cace_amm_obj_ns_ctr_t *ctr = cace_amm_obj_ns_ctr_ptr_cref(*ctr_ptr);
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

cace_amm_obj_desc_t *cace_amm_obj_ns_find_obj_enum(const cace_amm_obj_ns_t *ns, cace_ari_type_t obj_type,
                                                   cace_ari_int_id_t intenum)
{
    cace_amm_obj_ns_ctr_ptr_t *const *ctr_ptr = cace_amm_obj_ns_ctr_dict_cget(ns->object_types, obj_type);
    if (!ctr_ptr)
    {
        return NULL;
    }
    const cace_amm_obj_ns_ctr_t *ctr = cace_amm_obj_ns_ctr_ptr_cref(*ctr_ptr);
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

bool cace_amm_obj_ns_is_odm(const cace_amm_obj_ns_t *ns)
{
    CHKFALSE(ns);

    if ((ns->model_id.has_intenum && (ns->model_id.intenum < 0)) || m_string_start_with_str_p(ns->model_id.name, "!"))
    {
        return true;
    }

    return false;
}
