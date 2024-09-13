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
#include "parameters.h"
#include "cace/util/defs.h"

void cace_amm_formal_param_init(cace_amm_formal_param_t *obj)
{
    CHKVOID(obj);
    memset(obj, 0, sizeof(cace_amm_formal_param_t));
    obj->defval = ARI_INIT_UNDEFINED;
}

void cace_amm_formal_param_deinit(cace_amm_formal_param_t *obj)
{
    CHKVOID(obj);
    ari_deinit(&(obj->defval));
    memset(obj, 0, sizeof(cace_amm_formal_param_t));
}

void cace_amm_actual_param_set_init(cace_amm_actual_param_set_t *obj)
{
    CHKVOID(obj);
    ari_array_init(obj->ordered);
    named_ari_ptr_dict_init(obj->named);
}

void cace_amm_actual_param_set_deinit(cace_amm_actual_param_set_t *obj)
{
    CHKVOID(obj);
    named_ari_ptr_dict_clear(obj->named);
    ari_array_clear(obj->ordered);
}

void cace_amm_actual_param_set_reset(cace_amm_actual_param_set_t *obj)
{
    CHKVOID(obj);
    ari_array_reset(obj->ordered);
    named_ari_ptr_dict_reset(obj->named);
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
        cace_data_t *data = &(out->as_lit.value.as_data);
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

int cace_amm_actual_param_set_populate(cace_amm_actual_param_set_t *obj, const cace_amm_formal_param_list_t fparams,
                                       const ari_params_t *gparams)
{
    CHKERR1(obj);
    CHKERR1(fparams);
    CHKERR1(gparams);
    int retval = 0;

    cace_amm_formal_param_list_it_t fit;
    switch (gparams->state)
    {
        case ARI_PARAMS_NONE:
        {
            ari_array_resize(obj->ordered, cace_amm_formal_param_list_size(fparams));
            size_t pix = 0;

            for (cace_amm_formal_param_list_it(fit, fparams); !cace_amm_formal_param_list_end_p(fit);
                 cace_amm_formal_param_list_next(fit), ++pix)
            {
                const cace_amm_formal_param_t *fparam = cace_amm_formal_param_list_cref(fit);

                ari_t *aparam = ari_array_get(obj->ordered, pix);
                named_ari_ptr_dict_set_at(obj->named, string_get_cstr(fparam->name), aparam);

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

            ari_array_resize(obj->ordered, cace_amm_formal_param_list_size(fparams));
            size_t pix = 0;

            for (cace_amm_formal_param_list_it(fit, fparams); !cace_amm_formal_param_list_end_p(fit);
                 cace_amm_formal_param_list_next(fit), ++pix)
            {
                const cace_amm_formal_param_t *fparam = cace_amm_formal_param_list_cref(fit);

                ari_t *aparam = ari_array_get(obj->ordered, pix);
                named_ari_ptr_dict_set_at(obj->named, string_get_cstr(fparam->name), aparam);

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

            ari_array_resize(obj->ordered, cace_amm_formal_param_list_size(fparams));
            size_t pix = 0;

            for (cace_amm_formal_param_list_it(fit, fparams); !cace_amm_formal_param_list_end_p(fit) && !retval;
                 cace_amm_formal_param_list_next(fit), ++pix)
            {
                const cace_amm_formal_param_t *fparam = cace_amm_formal_param_list_cref(fit);

                ari_t *aparam = ari_array_get(obj->ordered, pix);
                named_ari_ptr_dict_set_at(obj->named, string_get_cstr(fparam->name), aparam);

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
