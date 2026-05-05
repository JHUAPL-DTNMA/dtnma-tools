/*
 * Copyright (c) 2011-2026 The Johns Hopkins University Applied Physics
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
#include "idseg_val.h"

void cace_amm_idseg_val_init(cace_amm_idseg_val_t *obj)
{
    CHKVOID(obj);
    m_string_init(obj->name);
    obj->has_intenum = false;
    obj->intenum     = 0;
}

void cace_amm_idseg_val_deinit(cace_amm_idseg_val_t *obj)
{
    CHKVOID(obj);
    m_string_clear(obj->name);
    obj->has_intenum = false;
    obj->intenum     = 0;
}

void cace_amm_idseg_val_set(cace_amm_idseg_val_t *obj, const cace_amm_idseg_val_t *src)
{
    CHKVOID(obj);
    CHKVOID(src);
    m_string_set(obj->name, src->name);
    obj->has_intenum = src->has_intenum;
    obj->intenum     = src->intenum;
}

void cace_amm_idseg_val_set_fromref(cace_amm_idseg_val_t *obj, const cace_amm_idseg_ref_t *src)
{
    CHKVOID(obj);
    CHKVOID(src);
    m_string_set_cstr(obj->name, src->name);
    obj->has_intenum = src->has_intenum;
    obj->intenum     = src->intenum;
}

bool cace_amm_idseg_val_match_int(const cace_amm_idseg_val_t *obj, cace_ari_int_id_t id)
{
    CHKRET(obj, false);
    return (obj->has_intenum && (obj->intenum == id));
}

void cace_ari_idseg_set_from_val(cace_ari_idseg_t *obj, const cace_amm_idseg_val_t *src)
{
    cace_ari_idseg_deinit(obj);
    if (src->has_intenum)
    {
        obj->form   = CACE_ARI_IDSEG_INT;
        obj->as_int = src->intenum;
    }
    else
    {
        obj->form = CACE_ARI_IDSEG_TEXT;
        m_string_init_set(obj->as_text, src->name);
    }
}
