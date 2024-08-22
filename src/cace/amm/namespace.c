#include "namespace.h"

void cace_amm_namespace_desc_init(cace_amm_namespace_desc_t *obj)
{
    m_string_init(obj->name);
    string_tree_set_init(obj->feature_supp);

    cace_amm_const_desc_list_init(obj->const_list);
    cace_amm_const_desc_by_enum_init(obj->const_by_enum);
    cace_amm_const_desc_by_name_init(obj->const_by_name);

    cace_amm_var_desc_list_init(obj->var_list);
    cace_amm_var_desc_by_enum_init(obj->var_by_enum);
    cace_amm_var_desc_by_name_init(obj->var_by_name);

    cace_amm_edd_desc_list_init(obj->edd_list);
    cace_amm_edd_desc_by_enum_init(obj->edd_by_enum);
    cace_amm_edd_desc_by_name_init(obj->edd_by_name);
}

void cace_amm_namespace_desc_deinit(cace_amm_namespace_desc_t *obj)
{
    cace_amm_edd_desc_by_name_clear(obj->edd_by_name);
    cace_amm_edd_desc_by_enum_clear(obj->edd_by_enum);
    cace_amm_edd_desc_list_clear(obj->edd_list);

    cace_amm_var_desc_by_name_clear(obj->var_by_name);
    cace_amm_var_desc_by_enum_clear(obj->var_by_enum);
    cace_amm_var_desc_list_clear(obj->var_list);

    cace_amm_const_desc_by_name_clear(obj->const_by_name);
    cace_amm_const_desc_by_enum_clear(obj->const_by_enum);
    cace_amm_const_desc_list_clear(obj->const_list);

    string_tree_set_clear(obj->feature_supp);
    m_string_clear(obj->name);
}
