#include "register.h"

cace_amm_obj_desc_t *refda_register_ident(cace_amm_obj_ns_t *ns, const cace_amm_obj_id_t obj_id,
                                          refda_amm_ident_desc_t *desc)
{
    cace_amm_obj_desc_t *obj = cace_amm_obj_ns_add_obj(ns, ARI_TYPE_IDENT, obj_id);
    if (!obj)
    {
        if (desc)
        {
            refda_amm_ident_desc_deinit(desc);
            ARI_FREE(desc);
        }
        return NULL;
    }

    if (!desc)
    {
        desc = ARI_MALLOC(sizeof(refda_amm_ident_desc_t));
        refda_amm_ident_desc_init(desc);
    }
    cace_amm_user_data_set_from(&(obj->app_data), desc, true, (cace_amm_user_data_deinit_f)refda_amm_ident_desc_deinit);

    return obj;
}

cace_amm_obj_desc_t *refda_register_typedef(cace_amm_obj_ns_t *ns, const cace_amm_obj_id_t obj_id,
                                            refda_amm_typedef_desc_t *desc)
{
    cace_amm_obj_desc_t *obj = cace_amm_obj_ns_add_obj(ns, ARI_TYPE_TYPEDEF, obj_id);
    if (!obj)
    {
        if (desc)
        {
            refda_amm_typedef_desc_deinit(desc);
            ARI_FREE(desc);
        }
        return NULL;
    }

    if (!desc)
    {
        desc = ARI_MALLOC(sizeof(refda_amm_typedef_desc_t));
        refda_amm_typedef_desc_init(desc);
    }
    cace_amm_user_data_set_from(&(obj->app_data), desc, true,
                                (cace_amm_user_data_deinit_f)refda_amm_typedef_desc_deinit);

    return obj;
}

cace_amm_obj_desc_t *refda_register_edd(cace_amm_obj_ns_t *ns, const cace_amm_obj_id_t obj_id,
                                        refda_amm_edd_desc_t *desc)
{
    cace_amm_obj_desc_t *obj = cace_amm_obj_ns_add_obj(ns, ARI_TYPE_EDD, obj_id);
    if (!obj)
    {
        if (desc)
        {
            refda_amm_edd_desc_deinit(desc);
            ARI_FREE(desc);
        }
        return NULL;
    }

    if (!desc)
    {
        desc = ARI_MALLOC(sizeof(refda_amm_edd_desc_t));
        refda_amm_edd_desc_init(desc);
    }
    cace_amm_user_data_set_from(&(obj->app_data), desc, true, (cace_amm_user_data_deinit_f)refda_amm_edd_desc_deinit);

    return obj;
}

cace_amm_obj_desc_t *refda_register_ctrl(cace_amm_obj_ns_t *ns, const cace_amm_obj_id_t obj_id,
                                         refda_amm_ctrl_desc_t *desc)
{
    cace_amm_obj_desc_t *obj = cace_amm_obj_ns_add_obj(ns, ARI_TYPE_CTRL, obj_id);
    if (!obj)
    {
        if (desc)
        {
            refda_amm_ctrl_desc_deinit(desc);
            ARI_FREE(desc);
        }
        return NULL;
    }

    if (!desc)
    {
        desc = ARI_MALLOC(sizeof(refda_amm_ctrl_desc_t));
        refda_amm_ctrl_desc_init(desc);
    }
    cace_amm_user_data_set_from(&(obj->app_data), desc, true, (cace_amm_user_data_deinit_f)refda_amm_ctrl_desc_deinit);

    return obj;
}
