#include "obj_ns.h"
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
    string_tree_set_init(ns->feature_supp);
    cace_amm_obj_ns_ctr_dict_init(ns->object_types);
}

void cace_amm_obj_ns_deinit(cace_amm_obj_ns_t *ns)
{
    cace_amm_obj_ns_ctr_dict_clear(ns->object_types);
    string_tree_set_clear(ns->feature_supp);
    m_string_clear(ns->name);
}

cace_amm_obj_desc_t *cace_amm_obj_ns_add_obj(cace_amm_obj_ns_t *ns, ari_type_t obj_type, const char *name,
                                             bool has_enum, int64_t intenum)
{
    cace_amm_obj_ns_ctr_t *ctr = cace_amm_obj_ns_ctr_dict_safe_get(ns->object_types, obj_type);

    cace_amm_obj_desc_t **found = cace_amm_obj_desc_by_name_get(ctr->obj_by_name, name);
    if (found)
    {
        CACE_LOG_WARNING("ignoring duplicate object name: %s", name);
        return NULL;
    }
    if (has_enum)
    {
        found = cace_amm_obj_desc_by_enum_get(ctr->obj_by_enum, intenum);
        if (found)
        {
            CACE_LOG_WARNING("ignoring duplicate object enum: %" PRId64, intenum);
            return NULL;
        }
    }

    cace_amm_obj_desc_t *obj = cace_amm_obj_desc_list_push_back_new(ctr->obj_list);
    string_set_str(obj->name, name);
    obj->has_enum = has_enum;
    obj->intenum  = intenum;

    cace_amm_obj_desc_by_name_set_at(ctr->obj_by_name, name, obj);
    if (has_enum)
    {
        cace_amm_obj_desc_by_enum_set_at(ctr->obj_by_enum, intenum, obj);
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
