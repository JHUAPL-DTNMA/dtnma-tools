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
#include "parameters.h"
#include "cace/ari/text.h"
#include "cace/util/defs.h"
#include "cace/util/logging.h"

void cace_amm_formal_param_init(cace_amm_formal_param_t *obj)
{
    CHKVOID(obj);
    memset(obj, 0, sizeof(cace_amm_formal_param_t));

    m_string_init(obj->name);
    cace_amm_type_init(&(obj->typeobj));
    obj->defval = CACE_ARI_INIT_UNDEFINED;
}

void cace_amm_formal_param_deinit(cace_amm_formal_param_t *obj)
{
    CHKVOID(obj);
    cace_ari_deinit(&(obj->defval));
    cace_amm_type_deinit(&(obj->typeobj));
    m_string_clear(obj->name);

    memset(obj, 0, sizeof(cace_amm_formal_param_t));
}

static int normalize_key(cace_ari_t *out, const cace_ari_t *in)
{
    CHKERR1(out);
    CHKERR1(in);
    if (in->is_ref)
    {
        return 2;
    }

    if (in->as_lit.prim_type == CACE_ARI_PRIM_TSTR)
    {
        // case-fold text
        cace_ari_init_copy(out, in);
        cace_data_t *data = &(out->as_lit.value.as_data);
        for (size_t ix = 0; ix < data->len; ++ix)
        {
            data->ptr[ix] = tolower(data->ptr[ix]);
        }
    }
    else
    {
        // otherwise force to uvast
        const cace_amm_type_t *typeobj = cace_amm_type_get_builtin(CACE_ARI_TYPE_UVAST);
        cace_ari_init(out);
        if (cace_amm_type_convert(typeobj, out, in))
        {
            CACE_LOG_WARNING("Failed to convert key to UVAST type");
            return 3;
        }
    }
    return 0;
}

int cace_amm_actual_param_set_populate(cace_ari_itemized_t *obj, const cace_amm_formal_param_list_t fparams,
                                       const cace_ari_params_t *gparams)
{
    CHKERR1(obj);
    CHKERR1(fparams);
    CHKERR1(gparams);
    int retval = 0;

    obj->any_undefined = false;

    cace_amm_formal_param_list_it_t fit;
    switch (gparams->state)
    {
        case CACE_ARI_PARAMS_NONE:
        {
            CACE_LOG_DEBUG("given parameters absent");
            cace_ari_array_resize(obj->ordered, cace_amm_formal_param_list_size(fparams));
            size_t pix = 0;

            for (cace_amm_formal_param_list_it(fit, fparams); !cace_amm_formal_param_list_end_p(fit);
                 cace_amm_formal_param_list_next(fit), ++pix)
            {
                const cace_amm_formal_param_t *fparam = cace_amm_formal_param_list_cref(fit);

                cace_ari_t *aparam = cace_ari_array_get(obj->ordered, pix);
                cace_named_ari_ptr_dict_set_at(obj->named, m_string_get_cstr(fparam->name), aparam);

                // No given parameter, take default even if it is undefined value
                cace_ari_set_copy(aparam, &(fparam->defval));

                if (cace_ari_is_undefined(aparam))
                {
                    obj->any_undefined = true;
                }
            }
            break;
        }
        case CACE_ARI_PARAMS_AC:
        {
            cace_ari_list_t   *gparam_list = &(gparams->as_ac->items);
            cace_ari_list_it_t gparam_it;
            cace_ari_list_it(gparam_it, *gparam_list);

            cace_ari_array_resize(obj->ordered, cace_amm_formal_param_list_size(fparams));
            size_t pix = 0;

            for (cace_amm_formal_param_list_it(fit, fparams); !cace_amm_formal_param_list_end_p(fit);
                 cace_amm_formal_param_list_next(fit), ++pix)
            {
                const cace_amm_formal_param_t *fparam = cace_amm_formal_param_list_cref(fit);
                CACE_LOG_DEBUG("converting formal parameter #%d \"%s\"", fparam->index,
                               m_string_get_cstr(fparam->name));
                if (cace_log_is_enabled_for(LOG_DEBUG))
                {
                    cace_ari_t ariname = CACE_ARI_INIT_UNDEFINED;
                    const bool valid   = cace_amm_type_get_name(&(fparam->typeobj), &ariname);

                    if (valid)
                    {
                        m_string_t buf;
                        m_string_init(buf);
                        cace_ari_text_encode(buf, &ariname, CACE_ARI_TEXT_ENC_OPTS_DEFAULT);
                        CACE_LOG_DEBUG("  type %s", m_string_get_cstr(buf));
                        m_string_clear(buf);
                    }
                    else
                    {
                        CACE_LOG_DEBUG("  type unavailable");
                    }
                    cace_ari_deinit(&ariname);
                }

                cace_ari_t *aparam = cace_ari_array_get(obj->ordered, pix);
                cace_named_ari_ptr_dict_set_at(obj->named, m_string_get_cstr(fparam->name), aparam);

                if (cace_ari_list_end_p(gparam_it))
                {
                    // no given parameter
                    CACE_LOG_DEBUG("  given parameter absent");
                    cace_ari_set_copy(aparam, &(fparam->defval));
                }
                else
                {
                    const cace_ari_t *gparam = cace_ari_list_cref(gparam_it);
                    if (cace_log_is_enabled_for(LOG_DEBUG))
                    {
                        m_string_t buf;
                        m_string_init(buf);
                        cace_ari_text_encode(buf, gparam, CACE_ARI_TEXT_ENC_OPTS_DEFAULT);
                        CACE_LOG_DEBUG("  given parameter %s", m_string_get_cstr(buf));
                        m_string_clear(buf);
                    }
                    if (cace_amm_type_convert(&(fparam->typeobj), aparam, gparam))
                    {
                        CACE_LOG_WARNING("  given parameter failed to convert");
                        retval = 2;
                        break;
                    }
                    if (cace_log_is_enabled_for(LOG_DEBUG))
                    {
                        m_string_t buf;
                        m_string_init(buf);
                        cace_ari_text_encode(buf, aparam, CACE_ARI_TEXT_ENC_OPTS_DEFAULT);
                        CACE_LOG_DEBUG("  actual parameter %s", m_string_get_cstr(buf));
                        m_string_clear(buf);
                    }

                    cace_ari_list_next(gparam_it);
                }

                if (cace_ari_is_undefined(aparam))
                {
                    obj->any_undefined = true;
                }
            }

            // remaining unused actuals
            if (!retval && !cace_ari_list_end_p(gparam_it))
            {
                retval = 3;
            }

            break;
        }
        case CACE_ARI_PARAMS_AM:
        {
            cace_ari_tree_t *gparam_map = &(gparams->as_am->items);

            // keep track of used keys and normalize
            cace_ari_tree_t norm_map;
            cace_ari_tree_init(norm_map);
            {
                cace_ari_tree_it_t git;
                for (cace_ari_tree_it(git, *gparam_map); !cace_ari_tree_end_p(git); cace_ari_tree_next(git))
                {
                    const cace_ari_tree_subtype_ct *pair = cace_ari_tree_cref(git);

                    cace_ari_t norm_key;
                    if (normalize_key(&norm_key, pair->key_ptr))
                    {
                        retval = 4;
                        break;
                    }
                    cace_ari_tree_set_at(norm_map, norm_key, *(pair->value_ptr));
                    cace_ari_deinit(&norm_key);
                }
            }
            if (retval)
            {
                break;
            }

            cace_ari_array_resize(obj->ordered, cace_amm_formal_param_list_size(fparams));
            size_t pix = 0;

            for (cace_amm_formal_param_list_it(fit, fparams); !cace_amm_formal_param_list_end_p(fit) && !retval;
                 cace_amm_formal_param_list_next(fit), ++pix)
            {
                const cace_amm_formal_param_t *fparam = cace_amm_formal_param_list_cref(fit);

                cace_ari_t *aparam = cace_ari_array_get(obj->ordered, pix);
                cace_named_ari_ptr_dict_set_at(obj->named, m_string_get_cstr(fparam->name), aparam);

                // try integer key
                cace_ari_t key_uvast;
                cace_ari_init(&key_uvast);
                cace_ari_set_uvast(&key_uvast, fparam->index);
                const cace_ari_t *gparam_uvast = cace_ari_tree_cget(norm_map, key_uvast);

                // try text key
                cace_ari_t key_tstr;
                cace_ari_init(&key_tstr);
                cace_ari_set_tstr(&key_tstr, m_string_get_cstr(fparam->name), false);
                const cace_ari_t *gparam_tstr = cace_ari_tree_cget(norm_map, key_tstr);

                if (gparam_uvast && gparam_tstr)
                {
                    // both key forms given
                    retval = 4;
                }
                else
                {
                    const cace_ari_t *gparam = gparam_uvast ? gparam_uvast : gparam_tstr;
                    if (!gparam)
                    {
                        // no given parameter
                        cace_ari_set_copy(aparam, &(fparam->defval));
                    }
                    else
                    {
                        if (cace_amm_type_convert(&(fparam->typeobj), aparam, gparam))
                        {
                            retval = 2;
                        }
                    }

                    if (cace_ari_is_undefined(aparam))
                    {
                        obj->any_undefined = true;
                    }
                }

                if (gparam_uvast)
                {
                    cace_ari_tree_erase(norm_map, key_uvast);
                }
                cace_ari_deinit(&key_uvast);

                if (gparam_tstr)
                {
                    cace_ari_tree_erase(norm_map, key_tstr);
                }
                cace_ari_deinit(&key_tstr);
            }

            // remaining unused actuals
            if (!retval && !cace_ari_tree_empty_p(norm_map))
            {
                retval = 3;
            }
            cace_ari_tree_clear(norm_map);

            break;
        }
    }

    return retval;
}
