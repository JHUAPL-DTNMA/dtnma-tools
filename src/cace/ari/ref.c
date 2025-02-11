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
/** @file
 * @ingroup ari
 */
#include "ref.h"
#include "containers.h"
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

void cace_ari_date_init(cace_ari_date_t *obj)
{
    obj->valid = false;
    memset(&obj->parts, 0, sizeof(struct tm));
}

void cace_ari_date_deinit(cace_ari_date_t *obj)
{
    obj->valid = false;
}

void cace_ari_date_copy(cace_ari_date_t *obj, const cace_ari_date_t *src)
{
    // plain old data
    obj->valid = src->valid;
    obj->parts = src->parts;
}

int cace_ari_date_cmp(const cace_ari_date_t *left, const cace_ari_date_t *right)
{
    if (!(left->valid) && !(right->valid))
    {
        return 0;
    }
    if (left->valid != right->valid)
    {
        return (left->valid) ? 1 : -1;
    }

    int part_cmp = M_CMP_DEFAULT(left->parts.tm_year, right->parts.tm_year);
    if (part_cmp)
    {
        return part_cmp;
    }
    part_cmp = M_CMP_DEFAULT(left->parts.tm_mon, right->parts.tm_mon);
    if (part_cmp)
    {
        return part_cmp;
    }
    part_cmp = M_CMP_DEFAULT(left->parts.tm_mday, right->parts.tm_mday);
    return part_cmp;
}

int cace_ari_date_from_text(cace_ari_date_t *obj, const char *text)
{
    CHKERR1(obj);
    CHKERR1(text);

    cace_data_t rev_view;
    cace_data_init_view_cstr(&rev_view, text);
    int res    = cace_date_decode(&obj->parts, &rev_view);
    obj->valid = (res == 0);
    cace_data_deinit(&rev_view);
    return res;
}

void cace_ari_objpath_init(cace_ari_objpath_t *obj)
{
    CHKVOID(obj);
    cace_ari_idseg_init(&(obj->org_id));
    cace_ari_idseg_init(&(obj->model_id));
    cace_ari_date_init(&(obj->model_rev));
    cace_ari_idseg_init(&(obj->type_id));
    cace_ari_idseg_init(&(obj->obj_id));
    obj->has_ari_type = false;
    obj->ari_type     = CACE_ARI_TYPE_NULL;
}

void cace_ari_objpath_deinit(cace_ari_objpath_t *obj)
{
    CHKVOID(obj);
    cace_ari_idseg_deinit(&(obj->org_id));
    cace_ari_idseg_deinit(&(obj->model_id));
    cace_ari_date_deinit(&(obj->model_rev));
    cace_ari_idseg_deinit(&(obj->type_id));
    cace_ari_idseg_deinit(&(obj->obj_id));
    obj->has_ari_type = false;
}

void cace_ari_objpath_copy(cace_ari_objpath_t *obj, const cace_ari_objpath_t *src)
{
    CHKVOID(obj);
    CHKVOID(src);

    cace_ari_idseg_copy(&(obj->org_id), &(src->org_id));
    cace_ari_idseg_copy(&(obj->model_id), &(src->model_id));
    cace_ari_date_copy(&(obj->model_rev), &(src->model_rev));
    cace_ari_idseg_copy(&(obj->type_id), &(src->type_id));
    cace_ari_idseg_copy(&(obj->obj_id), &(src->obj_id));

    obj->has_ari_type = src->has_ari_type;
    obj->ari_type     = src->ari_type;
}

static bool cace_ari_valid_type_for_objpath(cace_ari_type_t type)
{
    return (type < CACE_ARI_TYPE_NULL && type != CACE_ARI_TYPE_OBJECT);
}

int cace_ari_objpath_derive_type(cace_ari_objpath_t *path)
{
    CHKERR1(path);

    path->has_ari_type = false;
    switch (path->type_id.form)
    {
        case CACE_ARI_IDSEG_NULL:
            break;
        case CACE_ARI_IDSEG_TEXT:
        {
            cace_ari_type_t found;
            const char     *name = string_get_cstr(path->type_id.as_text);
            if (!cace_ari_type_from_name(&found, name))
            {
                path->has_ari_type = true;
                path->ari_type     = found;
                if (!cace_ari_valid_type_for_objpath(found))
                {
                    return 3; // Invalid ARI
                }
            }
            break;
        }
        case CACE_ARI_IDSEG_INT:
        {
            if (!cace_ari_valid_type_for_objpath(path->type_id.as_int))
            {
                return 3; // Invalid ARI
            }

            // validate the ID by getting a static name
            if ((path->type_id.as_int < 0) && cace_ari_type_to_name(path->type_id.as_int))
            {
                path->has_ari_type = true;
                path->ari_type     = path->type_id.as_int;
            }
            break;
        }
    }

    return path->has_ari_type ? 0 : 2;
}

void cace_ari_objpath_set_textid(cace_ari_objpath_t *path, const char *org_id, const char *model_id,
                                 cace_ari_type_t type_id, const char *obj_id)
{
    cace_ari_objpath_set_textid_opt(path, org_id, model_id, &type_id, obj_id);
}

void cace_ari_objpath_set_textid_opt(cace_ari_objpath_t *path, const char *org_id, const char *model_id,
                                     const cace_ari_type_t *type_id, const char *obj_id)
{
    CHKVOID(path);
    cace_ari_objpath_deinit(path);

    if (org_id)
    {
        path->org_id.form = CACE_ARI_IDSEG_TEXT;
        string_t *value   = &(path->org_id.as_text);
        string_init_set_str(*value, org_id);
    }
    if (model_id)
    {
        path->model_id.form = CACE_ARI_IDSEG_TEXT;
        string_t *value     = &(path->model_id.as_text);
        string_init_set_str(*value, model_id);
    }
    if (type_id)
    {
        // FIXME better way to handle this?
        const char *type_name = cace_ari_type_to_name(*type_id);
        path->type_id.form    = CACE_ARI_IDSEG_TEXT;
        string_t *value       = &(path->type_id.as_text);
        string_init_set_str(*value, type_name);

        path->has_ari_type = true;
        path->ari_type     = *type_id;
    }
    if (obj_id)
    {
        path->obj_id.form = CACE_ARI_IDSEG_TEXT;
        string_t *value   = &(path->obj_id.as_text);
        string_init_set_str(*value, obj_id);
    }
}

void cace_ari_objpath_set_intid(cace_ari_objpath_t *path, cace_ari_int_id_t org_id, cace_ari_int_id_t model_id,
                                cace_ari_type_t type_id, cace_ari_int_id_t obj_id)
{
    cace_ari_objpath_set_intid_opt(path, &org_id, &model_id, &type_id, &obj_id);
}

void cace_ari_objpath_set_intid_opt(cace_ari_objpath_t *path, const cace_ari_int_id_t *org_id,
                                    const cace_ari_int_id_t *model_id, const cace_ari_type_t *type_id,
                                    const cace_ari_int_id_t *obj_id)
{
    CHKVOID(path);
    cace_ari_objpath_deinit(path);

    if (org_id)
    {
        path->org_id.form   = CACE_ARI_IDSEG_INT;
        path->org_id.as_int = *org_id;
    }
    if (model_id)
    {
        path->model_id.form   = CACE_ARI_IDSEG_INT;
        path->model_id.as_int = *model_id;
    }
    if (type_id)
    {
        path->type_id.form   = CACE_ARI_IDSEG_INT;
        path->type_id.as_int = *type_id;

        path->has_ari_type = true;
        path->ari_type     = *type_id;
    }
    if (obj_id)
    {
        path->obj_id.form   = CACE_ARI_IDSEG_INT;
        path->obj_id.as_int = *obj_id;
    }
}

int cace_ari_params_deinit(cace_ari_params_t *obj)
{
    CHKERR1(obj);
    switch (obj->state)
    {
        case CACE_ARI_PARAMS_NONE:
            break;
        case CACE_ARI_PARAMS_AC:
            CHKERR1(obj->as_ac);
            cace_ari_ac_deinit(obj->as_ac);
            CACE_FREE(obj->as_ac);
            break;
        case CACE_ARI_PARAMS_AM:
            CHKERR1(obj->as_am);
            cace_ari_am_deinit(obj->as_am);
            CACE_FREE(obj->as_am);
            break;
    }
    obj->state = CACE_ARI_PARAMS_NONE;
    return 0;
}

int cace_ari_params_copy(cace_ari_params_t *obj, const cace_ari_params_t *src)
{
    CHKERR1(obj);
    CHKERR1(src);

    cace_ari_params_deinit(obj);

    switch (src->state)
    {
        case CACE_ARI_PARAMS_NONE:
            break;
        case CACE_ARI_PARAMS_AC:
        {
            cace_ari_ac_t *ctr = CACE_MALLOC(sizeof(cace_ari_ac_t));
            if (ctr)
            {
                cace_ari_ac_init(ctr);
                cace_ari_list_set(ctr->items, src->as_ac->items);
                obj->state = src->state;
                obj->as_ac = ctr;
            }
            else
            {
                return 2;
            }
        }
        break;
        case CACE_ARI_PARAMS_AM:
        {
            cace_ari_am_t *ctr = CACE_MALLOC(sizeof(cace_ari_am_t));
            if (ctr)
            {
                cace_ari_am_init(ctr);
                cace_ari_tree_set(ctr->items, src->as_am->items);
                obj->state = src->state;
                obj->as_am = ctr;
            }
            else
            {
                return 2;
            }
        }
        break;
    }

    return 0;
}

void cace_ari_params_set_ac(cace_ari_params_t *obj, struct cace_ari_list_s *src)
{
    CHKVOID(obj);
    cace_ari_params_deinit(obj);

    cace_ari_ac_t *ctr = CACE_MALLOC(sizeof(cace_ari_ac_t));
    if (ctr)
    {
        obj->state = CACE_ARI_PARAMS_AC;
        obj->as_ac = ctr;
        cace_ari_list_init_move(ctr->items, src);
    }
}

void cace_ari_params_set_am(cace_ari_params_t *obj, struct cace_ari_tree_s *src)
{
    CHKVOID(obj);
    cace_ari_params_deinit(obj);

    cace_ari_am_t *ctr = CACE_MALLOC(sizeof(cace_ari_am_t));
    if (ctr)
    {
        obj->state = CACE_ARI_PARAMS_AM;
        obj->as_am = ctr;
        cace_ari_tree_init_move(ctr->items, src);
    }
}

void cace_ari_ref_deinit(cace_ari_ref_t *obj)
{
    CHKVOID(obj);
    cace_ari_objpath_deinit(&(obj->objpath));
    cace_ari_params_deinit(&(obj->params));
}

int cace_ari_ref_copy(cace_ari_ref_t *obj, const cace_ari_ref_t *src)
{
    cace_ari_objpath_copy(&(obj->objpath), &(src->objpath));
    cace_ari_params_copy(&(obj->params), &(src->params));
    return 0;
}
