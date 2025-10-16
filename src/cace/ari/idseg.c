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
/** @file
 * @ingroup ari
 */
#include "idseg.h"
#include "text_util.h"
#include "cace/util/defs.h"
#include <inttypes.h>

void cace_ari_idseg_init(cace_ari_idseg_t *idseg)
{
    memset(idseg, 0, sizeof(cace_ari_idseg_t));
    idseg->form = CACE_ARI_IDSEG_NULL;
}

void cace_ari_idseg_deinit(cace_ari_idseg_t *obj)
{
    CHKVOID(obj);
    if (obj->form == CACE_ARI_IDSEG_TEXT)
    {
        string_clear(obj->as_text);
    }
}

void cace_ari_idseg_copy(cace_ari_idseg_t *obj, const cace_ari_idseg_t *src)
{
    CHKVOID(obj);
    CHKVOID(src);
    obj->form = src->form;
    switch (src->form)
    {
        case CACE_ARI_IDSEG_NULL:
            break;
        case CACE_ARI_IDSEG_TEXT:
            string_set(obj->as_text, src->as_text);
            break;
        case CACE_ARI_IDSEG_INT:
            obj->as_int = src->as_int;
            break;
    }
}

size_t cace_ari_idseg_hash(const cace_ari_idseg_t *obj)
{
    CHKRET(obj, 0);

    M_HASH_DECL(accum);
    M_HASH_UP(accum, M_HASH_DEFAULT(obj->form));
    switch (obj->form)
    {
        case CACE_ARI_IDSEG_NULL:
            break;
        case CACE_ARI_IDSEG_TEXT:
            M_HASH_UP(accum, string_hash(obj->as_text));
            break;
        case CACE_ARI_IDSEG_INT:
            M_HASH_UP(accum, M_HASH_DEFAULT(obj->as_int));
            break;
    }
    return M_HASH_FINAL(accum);
}

int cace_ari_idseg_cmp(const cace_ari_idseg_t *left, const cace_ari_idseg_t *right)
{
    CHKRET(left, -2);
    CHKRET(right, -2);

    if (left->form != right->form)
    {
        return M_CMP_DEFAULT(left->form, right->form);
    }
    switch (left->form)
    {
        case CACE_ARI_IDSEG_NULL:
            return 0;
        case CACE_ARI_IDSEG_INT:
            return M_CMP_DEFAULT(left->as_int, right->as_int);
        case CACE_ARI_IDSEG_TEXT:
            return string_cmp(left->as_text, right->as_text);
        default:
            return -2;
    }
}

bool cace_ari_idseg_equal(const cace_ari_idseg_t *left, const cace_ari_idseg_t *right)
{
    CHKFALSE(left);
    CHKFALSE(right);
    if (left->form != right->form)
    {
        return false;
    }
    switch (left->form)
    {
        case CACE_ARI_IDSEG_NULL:
            return true;
        case CACE_ARI_IDSEG_TEXT:
            return string_equal_p(left->as_text, right->as_text);
        case CACE_ARI_IDSEG_INT:
            return left->as_int == right->as_int;
        default:
            return false;
    }
}

void cace_ari_idseg_init_text(cace_ari_idseg_t *idseg, string_t text)
{
    idseg->form     = CACE_ARI_IDSEG_TEXT;
    string_t *value = &(idseg->as_text);
    string_init_move(*value, text);
}

void cace_ari_idseg_derive_form(cace_ari_idseg_t *idseg)
{
    CHKVOID(idseg);
    if (idseg->form != CACE_ARI_IDSEG_TEXT)
    {
        // nothing to do
        return;
    }

    const char *instr = string_get_cstr(idseg->as_text);
    // text IDs are disjoint from numeric IDs
    if ((instr[0] == '-') || isdigit(instr[0]))
    {
        char             *end;
        cace_ari_int_id_t tmp = strtoll(instr, &end, 0);

        m_string_clear(idseg->as_text);

        idseg->form   = CACE_ARI_IDSEG_INT;
        idseg->as_int = tmp;
    }
}
