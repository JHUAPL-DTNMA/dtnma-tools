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
#include "cace/util/defs.h"
#include <inttypes.h>

void ari_idseg_init(ari_idseg_t *idseg)
{
    memset(idseg, 0, sizeof(ari_idseg_t));
    idseg->form = ARI_IDSEG_NULL;
}

int ari_idseg_init_from_text(ari_idseg_t *idseg, string_t text)
{
    const char *instr = string_get_cstr(text);

    // text IDs are disjoint from numeric IDs
    if ((instr[0] == '-') || isdigit(instr[0]))
    {
        idseg->form   = ARI_IDSEG_INT;
        idseg->as_int = strtoll(instr, NULL, 0);
        string_clear(text);
    }
    else
    {
        idseg->form     = ARI_IDSEG_TEXT;
        string_t *value = &(idseg->as_text);
        string_init_move(*value, text);
    }

    return 0;
}

void ari_idseg_deinit(ari_idseg_t *obj)
{
    CHKVOID(obj);
    if (obj->form == ARI_IDSEG_TEXT)
    {
        string_clear(obj->as_text);
    }
}

void ari_idseg_copy(ari_idseg_t *obj, const ari_idseg_t *src)
{
    CHKVOID(obj);
    CHKVOID(src);
    obj->form = src->form;
    switch (src->form)
    {
        case ARI_IDSEG_NULL:
            break;
        case ARI_IDSEG_TEXT:
            string_set(obj->as_text, src->as_text);
            break;
        case ARI_IDSEG_INT:
            obj->as_int = src->as_int;
            break;
    }
}

size_t ari_idseg_hash(const ari_idseg_t *obj)
{
    CHKRET(obj, 0);

    M_HASH_DECL(accum);
    M_HASH_UP(accum, M_HASH_DEFAULT(obj->form));
    switch (obj->form)
    {
        case ARI_IDSEG_NULL:
            break;
        case ARI_IDSEG_TEXT:
            M_HASH_UP(accum, string_hash(obj->as_text));
            break;
        case ARI_IDSEG_INT:
            M_HASH_UP(accum, M_HASH_DEFAULT(obj->as_int));
            break;
    }
    return M_HASH_FINAL(accum);
}

int ari_idseg_cmp(const ari_idseg_t *left, const ari_idseg_t *right)
{
    CHKRET(left, -2);
    CHKRET(right, -2);

    if (left->form != right->form)
    {
        return M_CMP_DEFAULT(left->form, right->form);
    }
    switch (left->form)
    {
        case ARI_IDSEG_NULL:
            return 0;
        case ARI_IDSEG_INT:
            return M_CMP_DEFAULT(left->as_int, right->as_int);
        case ARI_IDSEG_TEXT:
            return string_cmp(left->as_text, right->as_text);
        default:
            return -2;
    }
}

bool ari_idseg_equal(const ari_idseg_t *left, const ari_idseg_t *right)
{
    CHKFALSE(left);
    CHKFALSE(right);
    if (left->form != right->form)
    {
        return false;
    }
    switch (left->form)
    {
        case ARI_IDSEG_NULL:
            return true;
        case ARI_IDSEG_TEXT:
            return string_equal_p(left->as_text, right->as_text);
        case ARI_IDSEG_INT:
            return left->as_int == right->as_int;
        default:
            return false;
    }
}

void ari_objpath_init(ari_objpath_t *obj)
{
    memset(obj, 0, sizeof(ari_objpath_t));
}

void ari_objpath_deinit(ari_objpath_t *obj)
{
    ari_idseg_deinit(&(obj->ns_id));
    ari_idseg_deinit(&(obj->type_id));
    ari_idseg_deinit(&(obj->obj_id));
}

void ari_objpath_copy(ari_objpath_t *obj, const ari_objpath_t *src)
{
    ari_idseg_copy(&(obj->ns_id), &(src->ns_id));
    ari_idseg_copy(&(obj->type_id), &(src->type_id));
    ari_idseg_copy(&(obj->obj_id), &(src->obj_id));

    obj->has_ari_type = src->has_ari_type;
    obj->ari_type     = src->ari_type;
}

static bool ari_valid_type_for_objpath(ari_type_t type)
{
    return (type < ARI_TYPE_NULL && type != ARI_TYPE_OBJECT);
}

int ari_objpath_derive_type(ari_objpath_t *path)
{
    CHKERR1(path);

    path->has_ari_type = false;
    switch (path->type_id.form)
    {
        case ARI_IDSEG_NULL:
            break;
        case ARI_IDSEG_TEXT:
        {
            ari_type_t  found;
            const char *name = string_get_cstr(path->type_id.as_text);
            if (!ari_type_from_name(&found, name))
            {
                path->has_ari_type = true;
                path->ari_type     = found;
                if (!ari_valid_type_for_objpath(found))
                {
                    return 3; // Invalid ARI
                }
            }
            break;
        }
        case ARI_IDSEG_INT:
        {
            if (!ari_valid_type_for_objpath(path->type_id.as_int))
            {
                return 3; // Invalid ARI
            }

            // validate the ID by getting a static name
            if ((path->type_id.as_int < 0) && ari_type_to_name(path->type_id.as_int))
            {
                path->has_ari_type = true;
                path->ari_type     = path->type_id.as_int;
            }
            break;
        }
    }

    return path->has_ari_type ? 0 : 2;
}

void ari_objpath_set_textid(ari_objpath_t *path, const char *ns_id, ari_type_t type_id, const char *obj_id)
{
    ari_objpath_set_textid_opt(path, ns_id, &type_id, obj_id);
}

void ari_objpath_set_textid_opt(ari_objpath_t *path, const char *ns_id, const ari_type_t *type_id, const char *obj_id)
{
    CHKVOID(path);
    ari_objpath_deinit(path);

    if (ns_id)
    {
        path->ns_id.form = ARI_IDSEG_TEXT;
        string_t *value  = &(path->ns_id.as_text);
        string_init_set_str(*value, ns_id);
    }
    if (type_id)
    {
        // FIXME better way to handle this?
        const char *type_name = ari_type_to_name(*type_id);
        path->type_id.form    = ARI_IDSEG_TEXT;
        string_t *value       = &(path->type_id.as_text);
        string_init_set_str(*value, type_name);

        path->has_ari_type = true;
        path->ari_type     = *type_id;
    }
    if (obj_id)
    {
        path->obj_id.form = ARI_IDSEG_TEXT;
        string_t *value   = &(path->obj_id.as_text);
        string_init_set_str(*value, obj_id);
    }
}

void ari_objpath_set_intid(ari_objpath_t *path, int64_t ns_id, ari_type_t type_id, int64_t obj_id)
{
    ari_objpath_set_intid_opt(path, &ns_id, &type_id, &obj_id);
}

void ari_objpath_set_intid_opt(ari_objpath_t *path, const int64_t *ns_id, const ari_type_t *type_id,
                               const int64_t *obj_id)
{
    CHKVOID(path);
    ari_objpath_deinit(path);

    if (ns_id)
    {
        path->ns_id.form   = ARI_IDSEG_INT;
        path->ns_id.as_int = *ns_id;
    }
    if (type_id)
    {
        path->type_id.form   = ARI_IDSEG_INT;
        path->type_id.as_int = *type_id;

        path->has_ari_type = true;
        path->ari_type     = *type_id;
    }
    if (obj_id)
    {
        path->obj_id.form   = ARI_IDSEG_INT;
        path->obj_id.as_int = *obj_id;
    }
}

int ari_params_deinit(ari_params_t *obj)
{
    CHKERR1(obj);
    switch (obj->state)
    {
        case ARI_PARAMS_NONE:
            break;
        case ARI_PARAMS_AC:
            CHKERR1(obj->as_ac);
            ari_ac_deinit(obj->as_ac);
            ARI_FREE(obj->as_ac);
            break;
        case ARI_PARAMS_AM:
            CHKERR1(obj->as_am);
            ari_am_deinit(obj->as_am);
            ARI_FREE(obj->as_am);
            break;
    }
    obj->state = ARI_PARAMS_NONE;
    return 0;
}

int ari_params_copy(ari_params_t *obj, const ari_params_t *src)
{
    CHKERR1(obj);
    CHKERR1(src);

    ari_params_deinit(obj);

    switch (src->state)
    {
        case ARI_PARAMS_NONE:
            break;
        case ARI_PARAMS_AC:
        {
            ari_ac_t *ctr = ARI_MALLOC(sizeof(ari_ac_t));
            if (ctr)
            {
                ari_ac_init(ctr);
                ari_list_set(ctr->items, src->as_ac->items);
                obj->state = src->state;
                obj->as_ac = ctr;
            }
            else
            {
                return 2;
            }
        }
        break;
        case ARI_PARAMS_AM:
        {
            ari_am_t *ctr = ARI_MALLOC(sizeof(ari_am_t));
            if (ctr)
            {
                ari_am_init(ctr);
                ari_tree_set(ctr->items, src->as_am->items);
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

void ari_params_set_ac(ari_params_t *obj, struct ari_list_s *src)
{
    CHKVOID(obj);
    ari_params_deinit(obj);

    ari_ac_t *ctr = ARI_MALLOC(sizeof(ari_ac_t));
    if (ctr)
    {
        obj->state = ARI_PARAMS_AC;
        obj->as_ac = ctr;
        ari_list_init_move(ctr->items, src);
    }
}

void ari_params_set_am(ari_params_t *obj, struct ari_tree_s *src)
{
    CHKVOID(obj);
    ari_params_deinit(obj);

    ari_am_t *ctr = ARI_MALLOC(sizeof(ari_am_t));
    if (ctr)
    {
        obj->state = ARI_PARAMS_AM;
        obj->as_am = ctr;
        ari_tree_init_move(ctr->items, src);
    }
}

void ari_ref_deinit(ari_ref_t *obj)
{
    CHKVOID(obj);
    ari_objpath_deinit(&(obj->objpath));
    ari_params_deinit(&(obj->params));
}

int ari_ref_copy(ari_ref_t *obj, const ari_ref_t *src)
{
    ari_objpath_copy(&(obj->objpath), &(src->objpath));
    ari_params_copy(&(obj->params), &(src->params));
    return 0;
}
