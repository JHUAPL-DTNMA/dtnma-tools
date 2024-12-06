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
#include "semtype_cnst.h"

void amm_semtype_cnst_init(amm_semtype_cnst_t *obj)
{
    CHKVOID(obj);
    obj->type = AMM_SEMTYPE_CNST_INVALID;
}

void amm_semtype_cnst_deinit(amm_semtype_cnst_t *obj)
{
    CHKVOID(obj);
    switch (obj->type)
    {
        case AMM_SEMTYPE_CNST_INVALID:
            break;
        case AMM_SEMTYPE_CNST_STRLEN:
            cace_amm_range_size_deinit(&(obj->as_strlen));
            break;
        case AMM_SEMTYPE_CNST_TEXTPAT:
            regfree(&(obj->as_textpat));
            break;
        case AMM_SEMTYPE_CNST_RANGE_INT64:
            cace_amm_range_int64_deinit(&(obj->as_range_int64));
            break;
    }
    obj->type = AMM_SEMTYPE_CNST_INVALID;
}

cace_amm_range_size_t *amm_semtype_cnst_set_strlen(amm_semtype_cnst_t *obj)
{
    CHKNULL(obj);
    amm_semtype_cnst_deinit(obj);

    obj->type                  = AMM_SEMTYPE_CNST_STRLEN;
    cace_amm_range_size_t *cfg = &(obj->as_strlen);
    cace_amm_range_size_init(cfg);

    return cfg;
}

regex_t *amm_semtype_cnst_set_textpat(amm_semtype_cnst_t *obj, const char *pat)
{
    CHKNULL(obj);
    amm_semtype_cnst_deinit(obj);

    obj->type    = AMM_SEMTYPE_CNST_TEXTPAT;
    regex_t *cfg = &(obj->as_textpat);
    int      res = regcomp(cfg, pat, REG_NOSUB | REG_EXTENDED);
    if (res)
    {
        obj->type = AMM_SEMTYPE_CNST_INVALID;
        return NULL;
    }

    return cfg;
}

cace_amm_range_int64_t *amm_semtype_cnst_set_range_int64(amm_semtype_cnst_t *obj)
{
    CHKNULL(obj);
    amm_semtype_cnst_deinit(obj);

    obj->type                   = AMM_SEMTYPE_CNST_RANGE_INT64;
    cace_amm_range_int64_t *cfg = &(obj->as_range_int64);
    cace_amm_range_int64_init(cfg);

    return cfg;
}

bool amm_semtype_cnst_is_valid(const amm_semtype_cnst_t *obj, const ari_t *val)
{
    bool retval = false;
    switch (obj->type)
    {
        case AMM_SEMTYPE_CNST_INVALID:
            break;
        case AMM_SEMTYPE_CNST_STRLEN:
        {
            if (val->is_ref)
            {
                return false;
            }
            size_t len = 0;
            switch (val->as_lit.prim_type)
            {
                case ARI_PRIM_TSTR:
                    // ignore terminating null
                    len = ari_cget_tstr(val)->len - 1;
                    break;
                case ARI_PRIM_BSTR:
                    len = ari_cget_bstr(val)->len;
                    break;
                default:
                    return false;
            }
            const cace_amm_range_size_t *cfg = &(obj->as_strlen);

            retval = cace_amm_range_size_contains(cfg, len);
            break;
        }
        case AMM_SEMTYPE_CNST_TEXTPAT:
        {
            const cace_data_t *data = ari_cget_tstr(val);
            if (!data)
            {
                return false;
            }
            const regex_t *cfg = &(obj->as_textpat);

            int res = regexec(cfg, (const char *)(data->ptr), 0, NULL, 0);

            retval = (res == 0);
            break;
        }
        case AMM_SEMTYPE_CNST_RANGE_INT64:
        {
            int64_t intval;
            if (ari_get_vast(val, &intval))
            {
                return false;
            }
            const cace_amm_range_int64_t *cfg = &(obj->as_range_int64);

            retval = cace_amm_range_int64_contains(cfg, intval);
            break;
        }
    }
    return retval;
}
