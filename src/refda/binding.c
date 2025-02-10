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
#include "binding.h"
#include "amm/ident.h"
#include "amm/typedef.h"
#include "amm/const.h"
#include "amm/var.h"
#include "amm/edd.h"
#include "amm/ctrl.h"
#include "amm/oper.h"
#include <cace/amm/parameters.h>
#include <cace/amm/lookup.h>
#include <cace/amm/semtype.h>
#include <cace/ari.h>
#include <cace/ari/text.h>
#include <cace/util/defs.h>
#include <cace/util/logging.h>

static int refda_binding_typeobj(cace_amm_type_t *typeobj, const cace_amm_obj_store_t *store);

static int refda_binding_semtype_use(cace_amm_semtype_use_t *semtype, const cace_amm_obj_store_t *store)
{
    // do not rebind
    if (semtype->base)
    {
        return 0;
    }

    if (cace_log_is_enabled_for(LOG_DEBUG))
    {
        string_t buf;
        string_init(buf);
        cace_ari_text_encode(buf, &(semtype->name), CACE_ARI_TEXT_ENC_OPTS_DEFAULT);
        CACE_LOG_DEBUG("Binding use of %s", string_get_cstr(buf));
        string_clear(buf);
    }

    int failcnt = 0;
    if (semtype->name.is_ref)
    {
        cace_amm_lookup_t deref;
        cace_amm_lookup_init(&deref);

        if (!cace_amm_lookup_deref(&deref, store, &(semtype->name)))
        {
            if (deref.obj_type == CACE_ARI_TYPE_TYPEDEF)
            {
                refda_amm_typedef_desc_t *desc = deref.obj->app_data.ptr;
                if (desc)
                {
                    semtype->base = &(desc->typeobj);
                }
                else
                {
                    CACE_LOG_WARNING("Binding failed because object has no TYPEDEF descriptor");
                    failcnt = 1;
                }
            }
            else
            {
                CACE_LOG_WARNING("Binding failed because object is not a TYPEDEF");
                failcnt = 1;
            }
        }
        else
        {
            CACE_LOG_WARNING("Binding failed because TYPEDEF lookup failed");
            failcnt = 1;
        }

        cace_amm_lookup_deinit(&deref);
    }
    else
    {
        const int64_t *aritype = cace_ari_get_aritype(&(semtype->name));
        if (aritype)
        {
            semtype->base = cace_amm_type_get_builtin((cace_ari_type_t)*aritype);
        }
        else
        {
            CACE_LOG_WARNING("Binding failed because literal is not an ARITYPE");
            failcnt = 1;
        }
    }

    CACE_LOG_DEBUG("bound to %p class %d", semtype->base, (semtype->base ? (int)(semtype->base->type_class) : -1));
    return failcnt;
}

static int refda_binding_semtype_ulist(cace_amm_semtype_ulist_t *semtype, const cace_amm_obj_store_t *store)
{
    return refda_binding_typeobj(&(semtype->item_type), store);
}

static int refda_binding_semtype_dlist(cace_amm_semtype_dlist_t *semtype, const cace_amm_obj_store_t *store)
{
    int failcnt = 0;

    cace_amm_type_array_it_t it;
    for (cace_amm_type_array_it(it, semtype->types); !cace_amm_type_array_end_p(it); cace_amm_type_array_next(it))
    {
        cace_amm_type_t *typeobj = cace_amm_type_array_ref(it);

        failcnt += refda_binding_typeobj(typeobj, store);
    }
    return failcnt;
}

static int refda_binding_semtype_umap(cace_amm_semtype_umap_t *semtype, const cace_amm_obj_store_t *store)
{
    int failcnt = 0;
    failcnt += refda_binding_typeobj(&(semtype->key_type), store);
    failcnt += refda_binding_typeobj(&(semtype->val_type), store);
    return failcnt;
}

static int refda_binding_semtype_tblt(cace_amm_semtype_tblt_t *semtype, const cace_amm_obj_store_t *store)
{
    int failcnt = 0;

    cace_amm_named_type_array_it_t it;
    for (cace_amm_named_type_array_it(it, semtype->columns); !cace_amm_named_type_array_end_p(it);
         cace_amm_named_type_array_next(it))
    {
        cace_amm_named_type_t *col = cace_amm_named_type_array_ref(it);

        failcnt += refda_binding_typeobj(&(col->typeobj), store);
    }
    return failcnt;
}

static int refda_binding_semtype_union(cace_amm_semtype_union_t *semtype, const cace_amm_obj_store_t *store)
{
    int failcnt = 0;

    cace_amm_type_array_it_t it;
    for (cace_amm_type_array_it(it, semtype->choices); !cace_amm_type_array_end_p(it); cace_amm_type_array_next(it))
    {
        cace_amm_type_t *choice = cace_amm_type_array_ref(it);

        failcnt += refda_binding_typeobj(choice, store);
    }
    return failcnt;
}

static int refda_binding_semtype_seq(cace_amm_semtype_seq_t *semtype, const cace_amm_obj_store_t *store)
{
    return refda_binding_typeobj(&(semtype->item_type), store);
}

static int refda_binding_typeobj(cace_amm_type_t *typeobj, const cace_amm_obj_store_t *store)
{
    switch (typeobj->type_class)
    {
        case CACE_AMM_TYPE_INVALID:
            CACE_LOG_WARNING("Binding failed due to default-initialized typeobj");
            return 1;
        case CACE_AMM_TYPE_USE:
            return refda_binding_semtype_use(typeobj->as_semtype, store);
        case CACE_AMM_TYPE_ULIST:
            return refda_binding_semtype_ulist(typeobj->as_semtype, store);
        case CACE_AMM_TYPE_DLIST:
            return refda_binding_semtype_dlist(typeobj->as_semtype, store);
        case CACE_AMM_TYPE_UMAP:
            return refda_binding_semtype_umap(typeobj->as_semtype, store);
        case CACE_AMM_TYPE_TBLT:
            return refda_binding_semtype_tblt(typeobj->as_semtype, store);
        case CACE_AMM_TYPE_UNION:
            return refda_binding_semtype_union(typeobj->as_semtype, store);
        case CACE_AMM_TYPE_SEQ:
            return refda_binding_semtype_seq(typeobj->as_semtype, store);
        default:
            CACE_LOG_WARNING("Binding failed due to invalid typeobj %d", typeobj->type_class);
            return 1;
    }
}

static int refda_binding_fparams(cace_amm_formal_param_list_t fparams, const cace_amm_obj_store_t *store)
{
    int failcnt = 0;

    cace_amm_formal_param_list_it_t fit;
    for (cace_amm_formal_param_list_it(fit, fparams); !cace_amm_formal_param_list_end_p(fit);
         cace_amm_formal_param_list_next(fit))
    {
        cace_amm_formal_param_t *fparam = cace_amm_formal_param_list_ref(fit);
        CACE_LOG_DEBUG("Binding formal parameter \"%s\" (index %zd)", string_get_cstr(fparam->name), fparam->index);
        failcnt += refda_binding_typeobj(&(fparam->typeobj), store);
    }

    return failcnt;
}

static int refda_binding_ident_bases(refda_amm_ident_base_list_t bases, const cace_amm_obj_store_t *store)
{
    int failcnt = 0;

    refda_amm_ident_base_list_it_t it;
    for (refda_amm_ident_base_list_it(it, bases); !refda_amm_ident_base_list_end_p(it);
         refda_amm_ident_base_list_next(it))
    {
        refda_amm_ident_base_t *base = refda_amm_ident_base_list_ref(it);

        if (cace_log_is_enabled_for(LOG_DEBUG))
        {
            string_t buf;
            string_init(buf);
            cace_ari_text_encode(buf, &(base->name), CACE_ARI_TEXT_ENC_OPTS_DEFAULT);
            CACE_LOG_DEBUG("Binding IDENT base of %s", string_get_cstr(buf));
            string_clear(buf);
        }

        cace_amm_lookup_t deref;
        cace_amm_lookup_init(&deref);

        if (!cace_amm_lookup_deref(&deref, store, &(base->name)))
        {
            if (deref.obj_type == CACE_ARI_TYPE_IDENT)
            {
                refda_amm_ident_desc_t *desc = deref.obj->app_data.ptr;
                if (desc)
                {
                    base->ident = desc;
                }
                else
                {
                    CACE_LOG_WARNING("Binding failed because object has no IDENT descriptor");
                    failcnt += 1;
                }
            }
            else
            {
                CACE_LOG_WARNING("Binding failed because object is not an IDENT");
                failcnt += 1;
            }
        }
        else
        {
            CACE_LOG_WARNING("Binding failed because IDENT lookup failed");
            failcnt += 1;
        }

        cace_amm_lookup_deinit(&deref);
    }

    return failcnt;
}

int refda_binding_ident(cace_amm_obj_desc_t *obj, const cace_amm_obj_store_t *store)
{
    CHKERR1(obj);
    CHKERR1(store);
    refda_amm_ident_desc_t *desc = obj->app_data.ptr;
    CHKERR1(desc);

    int failcnt = 0;
    failcnt += refda_binding_fparams(obj->fparams, store);
    failcnt += refda_binding_ident_bases(desc->bases, store);
    return failcnt;
}

int refda_binding_typedef(cace_amm_obj_desc_t *obj, const cace_amm_obj_store_t *store)
{
    CHKERR1(obj);
    CHKERR1(store);
    refda_amm_typedef_desc_t *desc = obj->app_data.ptr;
    CHKERR1(desc);

    int failcnt = 0;
    failcnt += refda_binding_typeobj(&(desc->typeobj), store);
    return failcnt;
}

int refda_binding_const(cace_amm_obj_desc_t *obj, const cace_amm_obj_store_t *store)
{
    CHKERR1(obj);
    CHKERR1(store);
    refda_amm_const_desc_t *desc = obj->app_data.ptr;
    CHKERR1(desc);

    int failcnt = 0;
    failcnt += refda_binding_fparams(obj->fparams, store);
    return failcnt;
}

int refda_binding_var(cace_amm_obj_desc_t *obj, const cace_amm_obj_store_t *store)
{
    CHKERR1(obj);
    CHKERR1(store);
    refda_amm_var_desc_t *desc = obj->app_data.ptr;
    CHKERR1(desc);

    int failcnt = 0;
    failcnt += refda_binding_typeobj(&(desc->val_type), store);
    failcnt += refda_binding_fparams(obj->fparams, store);
    return failcnt;
}

int refda_binding_edd(cace_amm_obj_desc_t *obj, const cace_amm_obj_store_t *store)
{
    CHKERR1(obj);
    CHKERR1(store);
    refda_amm_var_desc_t *desc = obj->app_data.ptr;
    CHKERR1(desc);

    int failcnt = 0;
    failcnt += refda_binding_typeobj(&(desc->val_type), store);
    failcnt += refda_binding_fparams(obj->fparams, store);
    return failcnt;
}

int refda_binding_ctrl(cace_amm_obj_desc_t *obj, const cace_amm_obj_store_t *store)
{
    CHKERR1(obj);
    CHKERR1(store);
    refda_amm_ctrl_desc_t *desc = obj->app_data.ptr;
    CHKERR1(desc);

    int failcnt = 0;
    if (cace_amm_type_is_valid(&(desc->res_type)))
    {
        // optional
        failcnt += refda_binding_typeobj(&(desc->res_type), store);
    }
    failcnt += refda_binding_fparams(obj->fparams, store);
    return failcnt;
}

int refda_binding_oper(cace_amm_obj_desc_t *obj, const cace_amm_obj_store_t *store)
{
    CHKERR1(obj);
    CHKERR1(store);
    refda_amm_oper_desc_t *desc = obj->app_data.ptr;
    CHKERR1(desc);

    int failcnt = 0;
    // FIXME operand types
    failcnt += refda_binding_typeobj(&(desc->res_type), store);
    failcnt += refda_binding_fparams(obj->fparams, store);
    return failcnt;
}

int refda_binding_obj(cace_ari_type_t obj_type, cace_amm_obj_desc_t *obj, const cace_amm_obj_store_t *store)
{
    CACE_LOG_DEBUG("Binding object ./%s/%s", cace_ari_type_to_name(obj_type), string_get_cstr(obj->obj_id.name));
    switch (obj_type)
    {
        case CACE_ARI_TYPE_IDENT:
            return refda_binding_ident(obj, store);
        case CACE_ARI_TYPE_TYPEDEF:
            return refda_binding_typedef(obj, store);
        case CACE_ARI_TYPE_CONST:
            return refda_binding_const(obj, store);
        case CACE_ARI_TYPE_VAR:
            return refda_binding_var(obj, store);
        case CACE_ARI_TYPE_EDD:
            return refda_binding_edd(obj, store);
        case CACE_ARI_TYPE_CTRL:
            return refda_binding_ctrl(obj, store);
        case CACE_ARI_TYPE_OPER:
            return refda_binding_oper(obj, store);
        default:
            CACE_LOG_WARNING("Binding failed due to invalid obj-type %d", obj_type);
            return 1;
    }
}
