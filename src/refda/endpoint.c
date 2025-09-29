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
#include "endpoint.h"
#include "agent.h"
#include "cace/util/logging.h"
#include "cace/util/defs.h"
#if defined(PCRE_FOUND)
#include <pcre2.h>
#endif /* PCRE_FOUND */

bool refda_endpoint_pat_match(const refda_agent_t *agent, const cace_ari_t *endpoint, const refda_amm_ident_base_t *pat)
{
    if (!pat || !(pat->deref.ns) || !(pat->deref.obj) || (pat->deref.obj_type != CACE_ARI_TYPE_IDENT))
    {
        return false;
    }

    bool is_match = false;
    if ((pat->deref.ns->org_id.intenum == 1) && (pat->deref.ns->model_id.intenum == 26)
        && (pat->deref.obj->obj_id.intenum == 2))
    {
        // Logic for //ietf/network-base/ident/uri-regexp-pattern
        const char *pattern = cace_ari_cget_tstr_cstr(cace_ari_array_cget(pat->deref.aparams.ordered, 0));
        // match only text string values
        const char *value = cace_ari_cget_tstr_cstr(endpoint);
        if (!pattern)
        {
            CACE_LOG_ERR("Must provide a pattern parameter");
        }
        else if (pattern && value)
        {
#if defined(PCRE_FOUND)
            const int   opts        = PCRE2_ANCHORED | PCRE2_ENDANCHORED;
            int         errorcode   = 0;
            PCRE2_SIZE  erroroffset = 0;
            pcre2_code *cfg =
                pcre2_compile((PCRE2_SPTR8)pattern, strlen(pattern), opts, &errorcode, &erroroffset, NULL);
            if (!cfg)
            {
                CACE_LOG_ERR("Failed to compile regex pattern (error %d at %z): %s", errorcode, erroroffset, pattern);
            }
            else
            {
                pcre2_match_data *md   = pcre2_match_data_create_from_pattern(cfg, NULL);
                const int         opts = 0;
                // ignore terminating null
                int res = pcre2_match(cfg, (PCRE2_SPTR8)value, strlen(value), 0, opts, md, NULL);
                CACE_LOG_DEBUG("Matching pattern %s with value %s, result %d", pattern, value, res);
                if (res > 0)
                {
                    is_match = true;
                }
                pcre2_match_data_free(md);

                pcre2_code_free(cfg);
            }
#else  /* PCRE_FOUND */
            CACE_LOG_ERR("Cannot evaluate uri-regexp-pattern without PCRE")
#endif /* PCRE_FOUND */
        }
    }
    else if ((pat->deref.ns->org_id.intenum == 1) && (pat->deref.ns->model_id.intenum == 5)
             && (pat->deref.obj->obj_id.intenum == 2))
    {
        // Logic for //ietf/bp-base/ident/bp-eid-pattern

        // TODO implement this with EID patterns
    }
    else if ((pat->deref.ns->org_id.intenum == 1) && (pat->deref.ns->model_id.intenum == 4)
             && (pat->deref.obj->obj_id.intenum == 3))
    {
        // Logic for //ietf/inet-base/ident/ip-vlsm-pattern
        const cace_data_t *base     = cace_ari_cget_bstr(cace_ari_array_cget(pat->deref.aparams.ordered, 0));
        const cace_ari_t  *p_prefix = cace_ari_array_cget(pat->deref.aparams.ordered, 1);
        cace_ari_uint      prefix;
        int                res = cace_ari_get_uint(p_prefix, &prefix);
        if (!base)
        {
            CACE_LOG_ERR("Must provide a base parameter");
        }
        else if (res)
        {
            CACE_LOG_ERR("Must provide a prefix parameter");
        }
        else
        {
            // match only text string values
            cace_amm_lookup_t endp_deref;
            res = cace_amm_lookup_deref(&endp_deref, &agent->objs, endpoint);
            if (res)
            {
                CACE_LOG_ERR("Lookup failed with status %d", res);
            }
            else
            {
                // TODO implement this with CIDR patterns
            }
        }
    }
    // else if ...

    return is_match;
}
