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
#include "semtype_cnst.h"
#include "cace/util/logging.h"

void cace_amm_semtype_cnst_init(cace_amm_semtype_cnst_t *obj)
{
    CHKVOID(obj);
    obj->type = AMM_SEMTYPE_CNST_INVALID;
}

void cace_amm_semtype_cnst_deinit(cace_amm_semtype_cnst_t *obj)
{
    CHKVOID(obj);
    switch (obj->type)
    {
        case AMM_SEMTYPE_CNST_INVALID:
            break;
        case AMM_SEMTYPE_CNST_STRLEN:
            cace_amm_range_size_deinit(&(obj->as_strlen));
            break;
#if defined(PCRE_FOUND)
        case AMM_SEMTYPE_CNST_TEXTPAT:
            pcre2_code_free(obj->as_textpat);
            obj->as_textpat = NULL;
            break;
#endif /* PCRE_FOUND */
        case AMM_SEMTYPE_CNST_RANGE_INT64:
            cace_amm_range_int64_deinit(&(obj->as_range_int64));
            break;
    }
    obj->type = AMM_SEMTYPE_CNST_INVALID;
}

cace_amm_range_size_t *cace_amm_semtype_cnst_set_strlen(cace_amm_semtype_cnst_t *obj)
{
    CHKNULL(obj);
    cace_amm_semtype_cnst_deinit(obj);

    obj->type                  = AMM_SEMTYPE_CNST_STRLEN;
    cace_amm_range_size_t *cfg = &(obj->as_strlen);
    cace_amm_range_size_init(cfg);

    return cfg;
}

int cace_amm_semtype_cnst_set_textpat(cace_amm_semtype_cnst_t *obj, const char *pat)
{
    CHKERR1(obj);
    CHKERR1(pat);
#if defined(PCRE_FOUND)
    cace_amm_semtype_cnst_deinit(obj);

    const int   opts        = PCRE2_ANCHORED | PCRE2_ENDANCHORED;
    int         errorcode   = 0;
    PCRE2_SIZE  erroroffset = 0;
    pcre2_code *cfg         = pcre2_compile((PCRE2_SPTR8)pat, strlen(pat), opts, &errorcode, &erroroffset, NULL);
    if (!cfg)
    {
        CACE_LOG_ERR("Failed to compile regex pattern (error %d at %z): %s", errorcode, erroroffset, pat);
        return 2;
    }

    obj->type       = AMM_SEMTYPE_CNST_TEXTPAT;
    obj->as_textpat = cfg;

    return 0;
#else  /* PCRE_FOUND */
    return 100;
#endif /* PCRE_FOUND */
}

cace_amm_range_int64_t *cace_amm_semtype_cnst_set_range_int64(cace_amm_semtype_cnst_t *obj)
{
    CHKNULL(obj);
    cace_amm_semtype_cnst_deinit(obj);

    obj->type                   = AMM_SEMTYPE_CNST_RANGE_INT64;
    cace_amm_range_int64_t *cfg = &(obj->as_range_int64);
    cace_amm_range_int64_init(cfg);

    return cfg;
}

bool cace_amm_semtype_cnst_is_valid(const cace_amm_semtype_cnst_t *obj, const cace_ari_t *val)
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
                case CACE_ARI_PRIM_TSTR:
                    // ignore terminating null
                    len = cace_ari_cget_tstr(val)->len - 1;
                    break;
                case CACE_ARI_PRIM_BSTR:
                    len = cace_ari_cget_bstr(val)->len;
                    break;
                default:
                    return false;
            }
            const cace_amm_range_size_t *cfg = &(obj->as_strlen);

            retval = cace_amm_range_size_contains(cfg, len);
            break;
        }
#if defined(PCRE_FOUND)
        case AMM_SEMTYPE_CNST_TEXTPAT:
        {
            const cace_data_t *data = cace_ari_cget_tstr(val);
            if (!data)
            {
                return false;
            }
            const pcre2_code *cfg = obj->as_textpat;

            pcre2_match_data *md   = pcre2_match_data_create_from_pattern(cfg, NULL);
            const int         opts = 0;
            // ignore terminating null
            int res = pcre2_match(cfg, (PCRE2_SPTR8)(data->ptr), data->len - 1, 0, opts, md, NULL);
#if 0
            {
                char buf[128];
                pcre2_get_error_message(res, buf, sizeof(buf));
                CACE_LOG_DEBUG("Match regex result %d (%s) for: %s", res, buf, (const char *)(data->ptr));
            }
#endif
            pcre2_match_data_free(md);

            retval = (res > 0);
            break;
        }
#endif /* PCRE_FOUND */
        case AMM_SEMTYPE_CNST_RANGE_INT64:
        {
            int64_t intval;
            if (cace_ari_get_vast(val, &intval))
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
