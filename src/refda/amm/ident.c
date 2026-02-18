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
#include "ident.h"
#include "cace/ari/text.h"
#include "cace/util/logging.h"

void refda_amm_ident_base_init(refda_amm_ident_base_t *obj)
{
    CHKVOID(obj);
    cace_ari_init(&(obj->name));
    cace_amm_lookup_init(&obj->deref);
    obj->ident = NULL;
}

void refda_amm_ident_base_deinit(refda_amm_ident_base_t *obj)
{
    CHKVOID(obj);
    cace_amm_lookup_deinit(&obj->deref);
    obj->ident = NULL;
    cace_ari_deinit(&(obj->name));
}

void refda_amm_ident_base_init_set(refda_amm_ident_base_t *obj, const refda_amm_ident_base_t *src)
{
    refda_amm_ident_base_init(obj);
    refda_amm_ident_base_set(obj, src);
}

void refda_amm_ident_base_set(refda_amm_ident_base_t *obj, const refda_amm_ident_base_t *src)
{
    if (obj == src)
    {
        return;
    }
    CHKVOID(obj);
    CHKVOID(src);
    cace_ari_set_copy(&(obj->name), &(src->name));
    cace_amm_lookup_set(&obj->deref, &src->deref);
    obj->ident = src->ident;
}

void refda_amm_ident_base_set_move(refda_amm_ident_base_t *obj, refda_amm_ident_base_t *src)
{
    if (obj == src)
    {
        return;
    }
    CHKVOID(obj);
    CHKVOID(src);
    cace_ari_set_move(&(obj->name), &(src->name));
    cace_amm_lookup_set_move(&obj->deref, &src->deref);
    obj->ident = src->ident;
}

void refda_amm_ident_base_get_str(m_string_t out, const refda_amm_ident_base_t *obj, bool append)
{
    m_string_t buf;
    m_string_init(buf);
    cace_ari_text_encode(buf, &obj->name, CACE_ARI_TEXT_ENC_OPTS_DEFAULT);
    if (append)
    {
        m_string_cat(out, buf);
        m_string_clear(buf);
    }
    else
    {
        m_string_move(out, buf);
    }
}

int refda_amm_ident_base_populate(refda_amm_ident_base_t *obj, const cace_ari_t *ref, const cace_amm_obj_store_t *objs)
{
    CHKERR1(obj);

    if (ref)
    {
        cace_ari_set_copy(&obj->name, ref);
    }

    int res = cace_amm_lookup_deref(&obj->deref, objs, &obj->name);
    if (res)
    {
        m_string_t buf;
        m_string_init(buf);
        cace_ari_text_encode(buf, &obj->name, CACE_ARI_TEXT_ENC_OPTS_DEFAULT);
        CACE_LOG_WARNING("Lookup failed with status %d for reference %s", res, m_string_get_cstr(buf));
        m_string_clear(buf);

        obj->ident = NULL;
    }
    else
    {
        obj->ident = obj->deref.obj->app_data.ptr;
    }

    return res;
}

void refda_amm_ident_desc_init(refda_amm_ident_desc_t *obj)
{
    obj->abstract = false;
    refda_amm_ident_base_list_init(obj->bases);
    cace_amm_lookup_list_init(obj->derived);
    cace_amm_user_data_init(&(obj->user_data));
}

void refda_amm_ident_desc_deinit(refda_amm_ident_desc_t *obj)
{
    cace_amm_user_data_deinit(&(obj->user_data));
    cace_amm_lookup_list_clear(obj->derived);
    refda_amm_ident_base_list_clear(obj->bases);
    obj->abstract = false;
}
