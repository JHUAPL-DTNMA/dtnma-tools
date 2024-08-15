#include "parameters.h"
#include "util.h"

// DICT_SET_DEF(ari_seen_keys, ari_t)

void ari_formal_param_init(ari_formal_param_t *obj)
{
    CHKVOID(obj);
    memset(obj, 0, sizeof(ari_formal_param_t));
    obj->defval = ARI_INIT_UNDEFINED;
}

void ari_formal_param_deinit(ari_formal_param_t *obj)
{
    CHKVOID(obj);
    ari_deinit(&(obj->defval));
    memset(obj, 0, sizeof(ari_formal_param_t));
}

static int normalize_key(ari_t *out, const ari_t *in)
{
    CHKERR1(out);
    CHKERR1(in);
    if (in->is_ref)
    {
        return 2;
    }

    if (in->as_lit.prim_type == ARI_PRIM_TSTR)
    {
        // case-fold text
        ari_init_copy(out, in);
        ari_data_t *data = &(out->as_lit.value.as_data);
        for (size_t ix = 0; ix < data->len; ++ix)
        {
            data->ptr[ix] = tolower(data->ptr[ix]);
        }
    }
    else
    {
        // otherwise force to uvast
        const amm_type_t *typeobj = amm_type_get_builtin(ARI_TYPE_UVAST);
        ari_init(out);
        if (amm_type_convert(typeobj, out, in))
        {
            return 3;
        }
    }
    return 0;
}

int ari_actual_param_set_init(ari_actual_param_set_t *obj, const ari_formal_param_list_t fparams,
                              const ari_params_t *gparams)
{
    CHKERR1(obj);
    CHKERR1(fparams);
    CHKERR1(gparams);

    ari_list_init(obj->ordered);
    named_ari_ptr_dict_init(obj->named);

    //    ari_list_it_t oit;
    ari_formal_param_list_it_t fit;
    int                        retval = 0;
    switch (gparams->state)
    {
        case ARI_PARAMS_NONE:
        {
            for (ari_formal_param_list_it(fit, fparams); !ari_formal_param_list_end_p(fit);
                 ari_formal_param_list_next(fit))
            {
                const ari_formal_param_t *fparam = ari_formal_param_list_cref(fit);

                ari_t *aparam = ari_list_push_back_new(obj->ordered);
                named_ari_ptr_dict_set_at(obj->named, fparam->name, aparam);

                // FIXME don't care if it's undefined or not
                ari_set_copy(aparam, &(fparam->defval));
            }
            break;
        }
        case ARI_PARAMS_AC:
        {
            ari_list_t   *gparam_list = &(gparams->as_ac->items);
            ari_list_it_t gparam_it;
            ari_list_it(gparam_it, *gparam_list);

            for (ari_formal_param_list_it(fit, fparams); !ari_formal_param_list_end_p(fit);
                 ari_formal_param_list_next(fit))
            {
                const ari_formal_param_t *fparam = ari_formal_param_list_cref(fit);

                ari_t *aparam = ari_list_push_back_new(obj->ordered);
                named_ari_ptr_dict_set_at(obj->named, fparam->name, aparam);

                if (ari_list_end_p(gparam_it))
                {
                    // no given parameter
                    ari_set_copy(aparam, &(fparam->defval));
                }
                else
                {
                    const ari_t *gparam = ari_list_cref(gparam_it);
                    if (amm_type_convert(fparam->typeobj, aparam, gparam))
                    {
                        retval = 2;
                        break;
                    }

                    ari_list_next(gparam_it);
                }
            }

            // remaining unused actuals
            if (!retval && !ari_list_end_p(gparam_it))
            {
                retval = 3;
            }

            break;
        }
        case ARI_PARAMS_AM:
        {
            ari_tree_t *gparam_map = &(gparams->as_am->items);

            // keep track of used keys and normalize
            ari_tree_t norm_map;
            ari_tree_init(norm_map);
            {
                ari_tree_it_t git;
                for (ari_tree_it(git, *gparam_map); !ari_tree_end_p(git); ari_tree_next(git))
                {
                    const ari_tree_subtype_ct *pair = ari_tree_cref(git);

                    ari_t norm_key;
                    if (normalize_key(&norm_key, pair->key_ptr))
                    {
                        retval = 2;
                        break;
                    }
                    ari_tree_set_at(norm_map, norm_key, *(pair->value_ptr));
                    ari_deinit(&norm_key);
                }
            }
            if (retval)
            {
                break;
            }

            for (ari_formal_param_list_it(fit, fparams); !ari_formal_param_list_end_p(fit) && !retval;
                 ari_formal_param_list_next(fit))
            {
                const ari_formal_param_t *fparam = ari_formal_param_list_cref(fit);

                ari_t *aparam = ari_list_push_back_new(obj->ordered);
                named_ari_ptr_dict_set_at(obj->named, fparam->name, aparam);

                // try integer key
                ari_t key_uvast;
                ari_init(&key_uvast);
                ari_set_uvast(&key_uvast, fparam->index);
                const ari_t *gparam_uvast = ari_tree_cget(norm_map, key_uvast);

                // try text key
                ari_t key_tstr;
                ari_init(&key_tstr);
                ari_set_tstr(&key_tstr, string_get_cstr(fparam->name), false);
                const ari_t *gparam_tstr = ari_tree_cget(norm_map, key_tstr);

                if (gparam_uvast && gparam_tstr)
                {
                    // both key forms given
                    retval = 4;
                }
                else
                {
                    const ari_t *gparam = gparam_uvast ? gparam_uvast : gparam_tstr;
                    if (!gparam)
                    {
                        // no given parameter
                        ari_set_copy(aparam, &(fparam->defval));
                    }
                    else
                    {
                        if (amm_type_convert(fparam->typeobj, aparam, gparam))
                        {
                            retval = 2;
                        }
                    }
                }

                if (gparam_uvast)
                {
                    ari_tree_erase(norm_map, key_uvast);
                }
                ari_deinit(&key_uvast);

                if (gparam_tstr)
                {
                    ari_tree_erase(norm_map, key_tstr);
                }
                ari_deinit(&key_tstr);
            }

            // remaining unused actuals
            if (!retval && !ari_tree_empty_p(norm_map))
            {
                retval = 3;
            }
            ari_tree_clear(norm_map);

            break;
        }
    }

    return retval;
}

void ari_actual_param_set_deinit(ari_actual_param_set_t *obj)
{
    named_ari_ptr_dict_clear(obj->named);
    ari_list_clear(obj->ordered);
}
