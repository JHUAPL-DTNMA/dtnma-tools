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
#include "semtype.h"
#include "lookup.h"
#include "cace/ari/algo.h"
#include "cace/ari/text.h"
#include "cace/util/defs.h"
#include "cace/util/logging.h"
#include "cace/config.h"
#include <m-dict.h>

static bool cace_amm_semtype_use_constraints(const cace_amm_semtype_use_t *semtype, const cace_ari_t *val)
{
    cace_amm_semtype_cnst_array_it_t it;
    for (cace_amm_semtype_cnst_array_it(it, semtype->constraints); !cace_amm_semtype_cnst_array_end_p(it);
         cace_amm_semtype_cnst_array_next(it))
    {
        const cace_amm_semtype_cnst_t *cnst = cace_amm_semtype_cnst_array_cref(it);
        if (!cace_amm_semtype_cnst_is_valid(cnst, val))
        {
            CACE_LOG_WARNING("type use constraint type %d failed", cnst->type);
            // first failure wins
            return false;
        }
    }
    // no constraints are valid also
    return true;
}

int cace_amm_type_set_use_from_name(cace_amm_type_t *type, const cace_amm_lookup_t *deref,
                                    const cace_amm_obj_store_t *store _U_)
{
    cace_amm_semtype_use_t *semtype = cace_amm_type_set_use(type);
    {
        cace_ari_t **pval = cace_named_ari_ptr_dict_get(deref->aparams.named, "name");
        if (!pval)
        {
            CACE_LOG_ERR("No name parameter");
            return 3;
        }
        cace_ari_set_copy(&(semtype->name), *pval);
    }
    // TODO add constraints
    return 0;
}

static void cace_amm_semtype_use_name(const cace_amm_type_t *self, cace_ari_t *name)
{
    const cace_amm_semtype_use_t *semtype = self->as_semtype;
    CHKVOID(semtype);

    cace_ari_ref_t *ref = cace_ari_set_objref(name);
    cace_ari_objpath_set_textid(&(ref->objpath), "ietf", "amm-semtype", CACE_ARI_TYPE_IDENT, "type-use");

    cace_ari_tree_t params;
    cace_ari_tree_init(params);
    {
        cace_ari_t key = CACE_ARI_INIT_UNDEFINED;
        cace_ari_set_tstr(&key, "name", false);
        cace_ari_t *val = cace_ari_tree_safe_get(params, key);
        cace_ari_deinit(&key);

        cace_ari_set_copy(val, &(semtype->name));
    }
#if 0
    {
        cace_ari_t key = CACE_ARI_INIT_UNDEFINED;
        cace_ari_set_tstr(&key, "resolved", false);
        cace_ari_t *val = cace_ari_tree_safe_get(params, key);
        cace_ari_deinit(&key);
        cace_amm_type_get_name(semtype->base, val);
    }
#endif
    cace_ari_params_set_am(&(ref->params), params);
}

static cace_amm_type_match_res_t cace_amm_semtype_use_match(const cace_amm_type_t *self, const cace_ari_t *ari)
{
    if (cace_ari_is_undefined(ari))
    {
        return CACE_AMM_TYPE_MATCH_UNDEFINED;
    }

    const cace_amm_semtype_use_t *semtype = self->as_semtype;
    CHKRET(semtype, CACE_AMM_TYPE_MATCH_NEGATIVE);
    const cace_amm_type_t *base = semtype->base;
    if (!base)
    {
        CACE_LOG_CRIT("base type pointer invalid, binding must have failed");
        return CACE_AMM_TYPE_MATCH_NEGATIVE;
    }

    cace_amm_type_match_res_t got = cace_amm_type_match(base, ari);
    if (got == CACE_AMM_TYPE_MATCH_NEGATIVE)
    {
        return got;
    }

    return cace_amm_type_match_pos_neg(cace_amm_semtype_use_constraints(semtype, ari));
}

static int cace_amm_semtype_use_convert(const cace_amm_type_t *self, cace_ari_t *out, const cace_ari_t *in)
{
    const cace_amm_semtype_use_t *semtype = self->as_semtype;
    CHKERR1(semtype);
    const cace_amm_type_t *base = semtype->base;
    if (!base)
    {
        CACE_LOG_CRIT("base type pointer invalid, binding must have failed");
        return CACE_AMM_ERR_CONVERT_NULLFUNC;
    }

    int res = cace_amm_type_convert(base, out, in);
    CHKERRVAL(res);

    if (!cace_amm_semtype_use_constraints(semtype, out))
    {
        return CACE_AMM_ERR_CONVERT_FAILED_CONSTRAINT;
    }

    return 0;
}

cace_amm_semtype_use_t *cace_amm_type_set_use(cace_amm_type_t *type)
{
    CHKNULL(type);
    cace_amm_type_reset(type);

    type->ari_name   = cace_amm_semtype_use_name;
    type->match      = cace_amm_semtype_use_match;
    type->convert    = cace_amm_semtype_use_convert;
    type->type_class = CACE_AMM_TYPE_USE;

    cace_amm_semtype_use_t *semtype = CACE_MALLOC(sizeof(cace_amm_semtype_use_t));
    cace_amm_semtype_use_init(semtype);

    type->as_semtype        = semtype;
    type->as_semtype_deinit = (cace_amm_semtype_deinit_f)cace_amm_semtype_use_deinit;

    return semtype;
}

cace_amm_semtype_use_t *cace_amm_type_set_use_ref(cace_amm_type_t *type, const cace_ari_t *name)
{
    CHKNULL(type);
    CHKNULL(name);
    cace_amm_semtype_use_t *semtype = cace_amm_type_set_use(type);
    if (semtype)
    {
        cace_ari_set_copy(&(semtype->name), name);
    }
    return semtype;
}

cace_amm_semtype_use_t *cace_amm_type_set_use_ref_move(cace_amm_type_t *type, cace_ari_t *name)
{
    CHKNULL(type);
    CHKNULL(name);
    cace_amm_semtype_use_t *semtype = cace_amm_type_set_use(type);
    if (semtype)
    {
        cace_ari_set_move(&(semtype->name), name);
    }
    return semtype;
}

int cace_amm_type_set_use_builtin(cace_amm_type_t *type, cace_ari_type_t ari_type)
{
    cace_ari_t typeref = CACE_ARI_INIT_UNDEFINED;
    cace_ari_set_aritype_text(&typeref, ari_type);
    cace_amm_semtype_use_t *semtype = cace_amm_type_set_use_ref_move(type, &typeref);

    semtype->base = cace_amm_type_get_builtin(ari_type);

    return semtype->base ? 0 : 3;
}

int cace_amm_type_set_ulist_from_name(cace_amm_type_t *type, const cace_amm_lookup_t *deref,
                                      const cace_amm_obj_store_t *store)
{
    cace_amm_semtype_ulist_t *semtype = cace_amm_type_set_ulist(type);
    {
        cace_ari_t **pval = cace_named_ari_ptr_dict_get(deref->aparams.named, "item-type");
        if (!pval)
        {
            CACE_LOG_ERR("No item-type parameter");
            return 3;
        }
        cace_amm_type_set_name(&(semtype->item_type), *pval, store);
    }
    return 0;
}

static void cace_amm_semtype_ulist_name(const cace_amm_type_t *self, cace_ari_t *name)
{
    const cace_amm_semtype_ulist_t *semtype = self->as_semtype;
    CHKVOID(semtype);

    cace_ari_ref_t *ref = cace_ari_set_objref(name);
    cace_ari_objpath_set_textid(&(ref->objpath), "ietf", "amm-semtype", CACE_ARI_TYPE_IDENT, "ulist");

    cace_ari_tree_t params;
    cace_ari_tree_init(params);
    {
        cace_ari_t key = CACE_ARI_INIT_UNDEFINED;
        cace_ari_set_tstr(&key, "item-type", false);
        cace_ari_t *val = cace_ari_tree_safe_get(params, key);
        cace_ari_deinit(&key);

        cace_amm_type_get_name(&(semtype->item_type), val);
    }
    // FIXME add other parameters
    cace_ari_params_set_am(&(ref->params), params);
}

static cace_amm_type_match_res_t cace_amm_semtype_ulist_match(const cace_amm_type_t *self, const cace_ari_t *ari)
{
    if (cace_ari_is_undefined(ari))
    {
        return CACE_AMM_TYPE_MATCH_UNDEFINED;
    }

    const cace_amm_semtype_ulist_t *semtype = self->as_semtype;
    CHKRET(semtype, CACE_AMM_TYPE_MATCH_NEGATIVE);

    const struct cace_ari_ac_s *val = cace_ari_cget_ac(ari);
    if (!val)
    {
        CACE_LOG_DEBUG("value is not an AC");
        return CACE_AMM_TYPE_MATCH_NEGATIVE;
    }

    // overall size constraints
    const size_t valsize = cace_ari_list_size(val->items);
    if (semtype->size.has_min)
    {
        if (valsize < semtype->size.i_min)
        {
            CACE_LOG_DEBUG("AC size too small: %zu", valsize);
            return CACE_AMM_TYPE_MATCH_NEGATIVE;
        }
    }
    if (semtype->size.has_max)
    {
        if (valsize > semtype->size.i_max)
        {
            CACE_LOG_DEBUG("AC size too large: %zu", valsize);
            return CACE_AMM_TYPE_MATCH_NEGATIVE;
        }
    }

    // per-item check
    cace_ari_list_it_t val_it;
    for (cace_ari_list_it(val_it, val->items); !cace_ari_list_end_p(val_it); cace_ari_list_next(val_it))
    {
        const cace_ari_t *val_item = cace_ari_list_cref(val_it);

        cace_amm_type_match_res_t got = cace_amm_type_match(&(semtype->item_type), val_item);
        if (got == CACE_AMM_TYPE_MATCH_NEGATIVE)
        {
            return got;
        }
    }

    return CACE_AMM_TYPE_MATCH_POSITIVE;
}

static int cace_amm_semtype_ulist_convert(const cace_amm_type_t *self, cace_ari_t *out, const cace_ari_t *in)
{
    const cace_amm_semtype_ulist_t *semtype = self->as_semtype;
    CHKERR1(semtype);

    const struct cace_ari_ac_s *inval = cace_ari_cget_ac(in);
    if (!inval)
    {
        return CACE_AMM_ERR_CONVERT_BADVALUE;
    }

    // overall size constraints
    const size_t valsize = cace_ari_list_size(inval->items);
    if (semtype->size.has_min)
    {
        if (valsize < semtype->size.i_min)
        {
            return CACE_AMM_ERR_CONVERT_BADVALUE;
        }
    }
    if (semtype->size.has_max)
    {
        if (valsize > semtype->size.i_max)
        {
            return CACE_AMM_ERR_CONVERT_BADVALUE;
        }
    }

    cace_ari_ac_t *out_ac = cace_ari_set_ac(out, NULL);

    int retval = 0;

    // input and output have exact same size
    cace_ari_list_it_t inval_it;
    for (cace_ari_list_it(inval_it, inval->items); !cace_ari_list_end_p(inval_it); cace_ari_list_next(inval_it))
    {
        const cace_ari_t *in_item = cace_ari_list_cref(inval_it);

        cace_ari_t out_item = CACE_ARI_INIT_UNDEFINED;
        // actual conversion
        int res = cace_amm_type_convert(&(semtype->item_type), &out_item, in_item);
        if (res)
        {
            retval = res;
            cace_ari_deinit(&out_item);
            break;
        }

        cace_ari_list_push_back_move(out_ac->items, &out_item);
    }

    return retval;
}

cace_amm_semtype_ulist_t *cace_amm_type_set_ulist(cace_amm_type_t *type)
{
    CHKNULL(type);
    cace_amm_type_reset(type);

    type->ari_name   = cace_amm_semtype_ulist_name;
    type->match      = cace_amm_semtype_ulist_match;
    type->convert    = cace_amm_semtype_ulist_convert;
    type->type_class = CACE_AMM_TYPE_ULIST;

    cace_amm_semtype_ulist_t *semtype = CACE_MALLOC(sizeof(cace_amm_semtype_ulist_t));
    cace_amm_semtype_ulist_init(semtype);

    type->as_semtype        = semtype;
    type->as_semtype_deinit = (cace_amm_semtype_deinit_f)cace_amm_semtype_ulist_deinit;

    return semtype;
}

/** Match a sub-sequence of an AC using an input iterator.
 */
static cace_amm_type_match_res_t cace_amm_semtype_seq_match_it(const cace_amm_semtype_seq_t *seq,
                                                               cace_ari_list_it_t            val_it)
{
    // iterate until the sequence limit is hit or there are no more
    size_t used = 0;
    while (!cace_ari_list_end_p(val_it) && (!seq->size.has_max || (used < seq->size.i_max)))
    {
        const cace_ari_t *val_item = cace_ari_list_cref(val_it);

        // actual match
        cace_amm_type_match_res_t got = cace_amm_type_match(&(seq->item_type), val_item);
        if (got != CACE_AMM_TYPE_MATCH_POSITIVE)
        {
            // don't fail here, just stop matching this sequence
            break;
        }

        ++used;
        cace_ari_list_next(val_it);
    }

    // not enough matched
    if (seq->size.has_min && (used < seq->size.i_min))
    {
        return CACE_AMM_TYPE_MATCH_NEGATIVE;
    }

    return CACE_AMM_TYPE_MATCH_POSITIVE;
}

/** Convert a sub-sequence of an AC using an input iterator.
 */
static bool cace_amm_semtype_seq_convert_it(const cace_amm_semtype_seq_t *seq, cace_ari_list_t out,
                                            cace_ari_list_it_t inval_it)
{
    // iterate until the sequence limit is hit or there are no more
    size_t used = 0;
    while (!cace_ari_list_end_p(inval_it) && (!seq->size.has_max || (used < seq->size.i_max)))
    {
        const cace_ari_t *in_item = cace_ari_list_cref(inval_it);

        cace_ari_t out_item = CACE_ARI_INIT_UNDEFINED;
        // actual conversion
        int res = cace_amm_type_convert(&(seq->item_type), &out_item, in_item);
        if (res)
        {
            // don't fail here, just stop matching this sequence
            break;
        }

        ++used;
        cace_ari_list_push_back_move(out, &out_item);
        cace_ari_list_next(inval_it);
    }

    // not enough matched
    if (seq->size.has_min && (used < seq->size.i_min))
    {
        return false;
    }

    return true;
}

int cace_amm_type_set_dlist_from_name(cace_amm_type_t *type, const cace_amm_lookup_t *deref,
                                      const cace_amm_obj_store_t *store)
{
    cace_amm_semtype_dlist_t *semtype = cace_amm_type_set_dlist(type, 0);
    {
        cace_ari_t **pval = cace_named_ari_ptr_dict_get(deref->aparams.named, "item-types");
        if (!pval)
        {
            CACE_LOG_ERR("No item-types parameter");
            return 3;
        }
        const cace_ari_ac_t *pval_ac = cace_ari_cget_ac(*pval);
        if (!pval_ac)
        {
            CACE_LOG_ERR("No item-types parameter as AC");
            return 3;
        }

        // grow the types list as needed
        cace_ari_list_it_t it;
        for (cace_ari_list_it(it, pval_ac->items); !cace_ari_list_end_p(it); cace_ari_list_next(it))
        {
            const cace_ari_t *pval_item = cace_ari_list_cref(it);

            cace_amm_type_t *typeobj = cace_amm_type_array_push_new(semtype->types);
            cace_amm_type_set_name(typeobj, pval_item, store);
        }
    }
    return 0;
}

static void cace_amm_semtype_dlist_name(const cace_amm_type_t *self, cace_ari_t *name)
{
    const cace_amm_semtype_dlist_t *semtype = self->as_semtype;
    CHKVOID(semtype);

    cace_ari_ref_t *ref = cace_ari_set_objref(name);
    cace_ari_objpath_set_textid(&(ref->objpath), "ietf", "amm-semtype", CACE_ARI_TYPE_IDENT, "dlist");

    cace_ari_tree_t params;
    cace_ari_tree_init(params);
    {
        cace_ari_t key = CACE_ARI_INIT_UNDEFINED;
        cace_ari_set_tstr(&key, "item-types", false);
        cace_ari_t *val = cace_ari_tree_safe_get(params, key);
        cace_ari_deinit(&key);

        {
            cace_ari_ac_t name_ac;
            cace_ari_ac_init(&name_ac);

            cace_amm_type_array_it_t typ_it;
            for (cace_amm_type_array_it(typ_it, semtype->types); !cace_amm_type_array_end_p(typ_it);
                 cace_amm_type_array_next(typ_it))
            {
                // each type in the list takes off one or more items
                const cace_amm_type_t *typ_item = cace_amm_type_array_cref(typ_it);

                cace_ari_t *name_item = cace_ari_list_push_back_new(name_ac.items);
                cace_amm_type_get_name(typ_item, name_item);
            }
            cace_ari_set_ac(val, &name_ac);
        }
    }
    cace_ari_params_set_am(&(ref->params), params);
}

static cace_amm_type_match_res_t cace_amm_semtype_dlist_match(const cace_amm_type_t *self, const cace_ari_t *ari)
{
    if (cace_ari_is_undefined(ari))
    {
        return CACE_AMM_TYPE_MATCH_UNDEFINED;
    }

    const cace_amm_semtype_dlist_t *semtype = self->as_semtype;
    CHKRET(semtype, CACE_AMM_TYPE_MATCH_NEGATIVE);

    const struct cace_ari_ac_s *val = cace_ari_cget_ac(ari);
    if (!val)
    {
        return CACE_AMM_TYPE_MATCH_NEGATIVE;
    }

    // try to consume all value items
    cace_ari_list_it_t val_it;
    cace_ari_list_it(val_it, val->items);

    cace_amm_type_array_it_t typ_it;
    for (cace_amm_type_array_it(typ_it, semtype->types); !cace_amm_type_array_end_p(typ_it);
         cace_amm_type_array_next(typ_it))
    {
        // each type in the list takes off one or more items
        const cace_amm_type_t *typ_item = cace_amm_type_array_cref(typ_it);

        if (typ_item->type_class == CACE_AMM_TYPE_SEQ)
        {
            cace_amm_semtype_seq_t *seq = typ_item->as_semtype;

            cace_amm_type_match_res_t got = cace_amm_semtype_seq_match_it(seq, val_it);
            if (got == CACE_AMM_TYPE_MATCH_NEGATIVE)
            {
                return got;
            }
        }
        else
        {
            // not enough values
            if (cace_ari_list_end_p(val_it))
            {
                return CACE_AMM_TYPE_MATCH_NEGATIVE;
            }
            const cace_ari_t *val_item = cace_ari_list_cref(val_it);

            // actual match
            cace_amm_type_match_res_t got = cace_amm_type_match(typ_item, val_item);
            if (got == CACE_AMM_TYPE_MATCH_NEGATIVE)
            {
                return got;
            }

            cace_ari_list_next(val_it);
        }
    }

    // too many input values
    if (!cace_ari_list_end_p(val_it))
    {
        return CACE_AMM_TYPE_MATCH_NEGATIVE;
    }

    return CACE_AMM_TYPE_MATCH_POSITIVE;
}

static int cace_amm_semtype_dlist_convert(const cace_amm_type_t *self, cace_ari_t *out, const cace_ari_t *in)
{
    const cace_amm_semtype_dlist_t *semtype = self->as_semtype;
    CHKERR1(semtype);

    const struct cace_ari_ac_s *inval = cace_ari_cget_ac(in);
    if (!inval)
    {
        return CACE_AMM_ERR_CONVERT_BADVALUE;
    }

    cace_ari_ac_t *out_ac = cace_ari_set_ac(out, NULL);

    int retval = 0;

    cace_ari_list_it_t inval_it;
    cace_ari_list_it(inval_it, inval->items);

    cace_amm_type_array_it_t typ_it;
    for (cace_amm_type_array_it(typ_it, semtype->types); !cace_amm_type_array_end_p(typ_it);
         cace_amm_type_array_next(typ_it))
    {
        // each type in the list takes off one or more items
        const cace_amm_type_t *typ_item = cace_amm_type_array_cref(typ_it);

        if (typ_item->type_class == CACE_AMM_TYPE_SEQ)
        {
            cace_amm_semtype_seq_t *seq = typ_item->as_semtype;
            if (!cace_amm_semtype_seq_convert_it(seq, out_ac->items, inval_it))
            {
                retval = CACE_AMM_ERR_CONVERT_BADVALUE;
                break;
            }
        }
        else
        {
            // not enough values
            if (cace_ari_list_end_p(inval_it))
            {
                retval = CACE_AMM_ERR_CONVERT_BADVALUE;
                break;
            }
            const cace_ari_t *in_item = cace_ari_list_cref(inval_it);

            cace_ari_t out_item = CACE_ARI_INIT_UNDEFINED;
            // actual conversion
            int res = cace_amm_type_convert(typ_item, &out_item, in_item);
            if (res)
            {
                retval = res;
                cace_ari_deinit(&out_item);
                break;
            }

            cace_ari_list_push_back_move(out_ac->items, &out_item);
            cace_ari_list_next(inval_it);
        }
    }

    // too many input values (and no earlier error)
    if (!retval && !cace_ari_list_end_p(inval_it))
    {
        retval = CACE_AMM_ERR_CONVERT_BADVALUE;
    }

    return retval;
}

cace_amm_semtype_dlist_t *cace_amm_type_set_dlist(cace_amm_type_t *type, size_t num_types)
{
    CHKNULL(type);
    cace_amm_type_reset(type);

    type->ari_name   = cace_amm_semtype_dlist_name;
    type->match      = cace_amm_semtype_dlist_match;
    type->convert    = cace_amm_semtype_dlist_convert;
    type->type_class = CACE_AMM_TYPE_DLIST;

    cace_amm_semtype_dlist_t *semtype = CACE_MALLOC(sizeof(cace_amm_semtype_dlist_t));
    cace_amm_semtype_dlist_init(semtype);
    cace_amm_type_array_resize(semtype->types, num_types);

    type->as_semtype        = semtype;
    type->as_semtype_deinit = (cace_amm_semtype_deinit_f)cace_amm_semtype_dlist_deinit;

    return semtype;
}

static void cace_amm_semtype_umap_name(const cace_amm_type_t *self, cace_ari_t *name)
{
    const cace_amm_semtype_umap_t *semtype = self->as_semtype;
    CHKVOID(semtype);

    cace_ari_ref_t *ref = cace_ari_set_objref(name);
    cace_ari_objpath_set_textid(&(ref->objpath), "ietf", "amm-semtype", CACE_ARI_TYPE_IDENT, "ulist");

    cace_ari_tree_t params;
    cace_ari_tree_init(params);
    {
        cace_ari_t key = CACE_ARI_INIT_UNDEFINED;
        cace_ari_set_tstr(&key, "key-type", false);
        cace_ari_t *val = cace_ari_tree_safe_get(params, key);
        cace_ari_deinit(&key);

        cace_amm_type_get_name(&(semtype->key_type), val);
    }
    {
        cace_ari_t key = CACE_ARI_INIT_UNDEFINED;
        cace_ari_set_tstr(&key, "value-type", false);
        cace_ari_t *val = cace_ari_tree_safe_get(params, key);
        cace_ari_deinit(&key);

        cace_amm_type_get_name(&(semtype->val_type), val);
    }

    cace_ari_params_set_am(&(ref->params), params);
}

static cace_amm_type_match_res_t cace_amm_semtype_umap_match(const cace_amm_type_t *self, const cace_ari_t *ari)
{
    if (cace_ari_is_undefined(ari))
    {
        return CACE_AMM_TYPE_MATCH_UNDEFINED;
    }

    const cace_amm_semtype_umap_t *semtype = self->as_semtype;
    CHKRET(semtype, CACE_AMM_TYPE_MATCH_NEGATIVE);

    const struct cace_ari_am_s *val = cace_ari_cget_am(ari);
    if (!val)
    {
        return CACE_AMM_TYPE_MATCH_NEGATIVE;
    }

    // per-item check
    cace_ari_tree_it_t val_it;
    for (cace_ari_tree_it(val_it, val->items); !cace_ari_tree_end_p(val_it); cace_ari_tree_next(val_it))
    {
        const cace_ari_tree_itref_t *val_item = cace_ari_tree_cref(val_it);

        cace_amm_type_match_res_t got = cace_amm_type_match(&(semtype->key_type), val_item->key_ptr);
        if (got == CACE_AMM_TYPE_MATCH_NEGATIVE)
        {
            return got;
        }
        got = cace_amm_type_match(&(semtype->val_type), val_item->value_ptr);
        if (got == CACE_AMM_TYPE_MATCH_NEGATIVE)
        {
            return got;
        }
    }

    return CACE_AMM_TYPE_MATCH_POSITIVE;
}

static int cace_amm_semtype_umap_convert(const cace_amm_type_t *self, cace_ari_t *out, const cace_ari_t *in)
{
    const cace_amm_semtype_umap_t *semtype = self->as_semtype;
    CHKERR1(semtype);

    const struct cace_ari_am_s *inval = cace_ari_cget_am(in);
    if (!inval)
    {
        return CACE_AMM_ERR_CONVERT_BADVALUE;
    }

    cace_ari_am_t outval;
    cace_ari_am_init(&outval);
    int retval = 0;

    // input and output have exact same size
    cace_ari_tree_it_t inval_it;
    for (cace_ari_tree_it(inval_it, inval->items); !cace_ari_tree_end_p(inval_it); cace_ari_tree_next(inval_it))
    {
        const cace_ari_tree_itref_t *in_item = cace_ari_tree_cref(inval_it);
        cace_ari_t                   out_key = CACE_ARI_INIT_UNDEFINED;
        cace_ari_t                   out_val = CACE_ARI_INIT_UNDEFINED;

        int res = cace_amm_type_convert(&(semtype->key_type), &out_key, in_item->key_ptr);
        if (res)
        {
            retval = res;
            break;
        }

        res = cace_amm_type_convert(&(semtype->val_type), &out_val, in_item->value_ptr);
        if (res)
        {
            retval = res;
            break;
        }

        // FIXME is there a more efficient way?
        cace_ari_tree_set_at(outval.items, out_key, out_val);
        cace_ari_deinit(&out_key);
        cace_ari_deinit(&out_val);
    }

    // always pass ownership to the output value
    cace_ari_set_am(out, &outval);
    return retval;
}

cace_amm_semtype_umap_t *cace_amm_type_set_umap(cace_amm_type_t *type)
{
    CHKNULL(type);
    cace_amm_type_reset(type);

    type->ari_name   = cace_amm_semtype_umap_name;
    type->match      = cace_amm_semtype_umap_match;
    type->convert    = cace_amm_semtype_umap_convert;
    type->type_class = CACE_AMM_TYPE_UMAP;

    cace_amm_semtype_umap_t *semtype = CACE_MALLOC(sizeof(cace_amm_semtype_umap_t));
    cace_amm_semtype_umap_init(semtype);

    type->as_semtype        = semtype;
    type->as_semtype_deinit = (cace_amm_semtype_deinit_f)cace_amm_semtype_umap_deinit;

    return semtype;
}

int cace_amm_type_set_umap_name(cace_amm_type_t *type, const cace_amm_lookup_t *deref,
                                const cace_amm_obj_store_t *store)
{
    cace_amm_semtype_umap_t *semtype = cace_amm_type_set_umap(type);
    {
        cace_ari_t **pval = cace_named_ari_ptr_dict_get(deref->aparams.named, "key-type");
        if (!pval)
        {
            CACE_LOG_ERR("No key-type parameter");
            return 3;
        }
        cace_amm_type_set_name(&(semtype->key_type), *pval, store);
    }
    {
        cace_ari_t **pval = cace_named_ari_ptr_dict_get(deref->aparams.named, "value-type");
        if (!pval)
        {
            CACE_LOG_ERR("No value-type parameter");
            return 3;
        }
        cace_amm_type_set_name(&(semtype->val_type), *pval, store);
    }
    return 0;
}

int cace_amm_type_set_umap_from_name(cace_amm_type_t *type, const cace_amm_lookup_t *deref,
                                     const cace_amm_obj_store_t *store)
{
    cace_amm_semtype_umap_t *semtype = cace_amm_type_set_umap(type);
    {
        cace_ari_t **pval = cace_named_ari_ptr_dict_get(deref->aparams.named, "key-type");
        if (!pval)
        {
            CACE_LOG_ERR("No key-type parameter");
            return 3;
        }
        cace_amm_type_set_name(&(semtype->key_type), *pval, store);
    }
    {
        cace_ari_t **pval = cace_named_ari_ptr_dict_get(deref->aparams.named, "value-type");
        if (!pval)
        {
            CACE_LOG_ERR("No value-type parameter");
            return 3;
        }
        cace_amm_type_set_name(&(semtype->val_type), *pval, store);
    }
    return 0;
}

int cace_amm_type_set_tblt_from_name(cace_amm_type_t *type, const cace_amm_lookup_t *deref,
                                     const cace_amm_obj_store_t *store)
{
    cace_amm_semtype_tblt_t *semtype = cace_amm_type_set_tblt_size(type, 0);
    {
        cace_ari_t **pval = cace_named_ari_ptr_dict_get(deref->aparams.named, "columns");
        if (!pval)
        {
            CACE_LOG_ERR("No columns parameter");
            return 3;
        }
        const cace_ari_tbl_t *pval_tbl = cace_ari_get_tbl(*pval);
        if (!pval_tbl)
        {
            CACE_LOG_ERR("No columns parameter as TBL");
            return 3;
        }
        if (pval_tbl->ncols != 2)
        {
            CACE_LOG_ERR("No columns parameter as valid TBL");
            return 3;
        }

        cace_ari_array_it_t tbl_it;
        cace_ari_array_it(tbl_it, pval_tbl->items);
        while (!cace_ari_array_end_p(tbl_it))
        {
            const cace_ari_t *name_item = cace_ari_array_cref(tbl_it);
            cace_ari_array_next(tbl_it);
            if (cace_ari_array_end_p(tbl_it))
            {
                CACE_LOG_ERR("Inconsistent items in TBL");
                break;
            }

            const cace_ari_t *datatype_item = cace_ari_array_cref(tbl_it);
            cace_ari_array_next(tbl_it);

            cace_amm_named_type_t *col_item = cace_amm_named_type_array_push_new(semtype->columns);
            {
                const char *cstr = cace_ari_cget_tstr_cstr(name_item);
                if (cstr)
                {
                    m_string_set_cstr(col_item->name, cstr);
                }
            }
            cace_amm_type_set_name(&(col_item->typeobj), datatype_item, store);
        }
    }
    // TODO add constraints
    return 0;
}

static void cace_amm_semtype_tblt_name(const cace_amm_type_t *self, cace_ari_t *name)
{
    const cace_amm_semtype_tblt_t *semtype = self->as_semtype;
    CHKVOID(semtype);

    cace_ari_ref_t *ref = cace_ari_set_objref(name);
    cace_ari_objpath_set_textid(&(ref->objpath), "ietf", "amm-semtype", CACE_ARI_TYPE_IDENT, "tblt");

    cace_ari_tree_t params;
    cace_ari_tree_init(params);
    {
        cace_ari_t key = CACE_ARI_INIT_UNDEFINED;
        cace_ari_set_tstr(&key, "columns", false);
        cace_ari_t *val = cace_ari_tree_safe_get(params, key);
        cace_ari_deinit(&key);

        cace_ari_tbl_t col_table;
        cace_ari_tbl_init(&col_table);
        cace_ari_tbl_reset(&col_table, 2, 0);

        cace_amm_named_type_array_it_t it;
        for (cace_amm_named_type_array_it(it, semtype->columns); !cace_amm_named_type_array_end_p(it);
             cace_amm_named_type_array_next(it))
        {
            const cace_amm_named_type_t *col_item = cace_amm_named_type_array_ref(it);

            cace_ari_array_t row;
            cace_ari_array_init(row);
            cace_ari_array_resize(row, 2);

            // name
            cace_ari_set_tstr(cace_ari_array_get(row, 0), m_string_get_cstr(col_item->name), true);
            // datatype
            cace_amm_type_get_name(&col_item->typeobj, cace_ari_array_get(row, 1));

            cace_ari_tbl_move_row_array(&col_table, row);
        }

        cace_ari_set_tbl(val, &col_table);
    }
    // FIXME add other parameters
    cace_ari_params_set_am(&(ref->params), params);
}

static cace_amm_type_match_res_t cace_amm_semtype_tblt_match(const cace_amm_type_t *self, const cace_ari_t *ari)
{
    if (cace_ari_is_undefined(ari))
    {
        return CACE_AMM_TYPE_MATCH_UNDEFINED;
    }

    const cace_amm_semtype_tblt_t *semtype = self->as_semtype;
    CHKRET(semtype, CACE_AMM_TYPE_MATCH_NEGATIVE);

    const struct cace_ari_tbl_s *val = cace_ari_cget_tbl(ari);
    if (!val)
    {
        return CACE_AMM_TYPE_MATCH_NEGATIVE;
    }

    if (val->ncols != cace_amm_named_type_array_size(semtype->columns))
    {
        CACE_LOG_DEBUG("TBLT needs %zu columns, value has %zu columns",
                       cace_amm_named_type_array_size(semtype->columns), val->ncols);
        return CACE_AMM_TYPE_MATCH_NEGATIVE;
    }

    cace_amm_named_type_array_it_t col_it;
    cace_amm_named_type_array_it(col_it, semtype->columns);

    cace_ari_array_it_t val_it;
    for (cace_ari_array_it(val_it, val->items); !cace_ari_array_end_p(val_it);
         cace_ari_array_next(val_it), cace_amm_named_type_array_next(col_it))
    {
        const cace_ari_t *val_item = cace_ari_array_cref(val_it);

        // wrap back around to first column
        if (cace_amm_named_type_array_end_p(col_it))
        {
            cace_amm_named_type_array_it(col_it, semtype->columns);
        }
        const cace_amm_named_type_t *col     = cace_amm_named_type_array_ref(col_it);
        const cace_amm_type_t       *typeobj = &(col->typeobj);

        const cace_amm_type_match_res_t got = cace_amm_type_match(typeobj, val_item);

        if (cace_log_is_enabled_for(LOG_DEBUG))
        {
            cace_ari_t ariname = CACE_ARI_INIT_UNDEFINED;
            cace_amm_type_get_name(typeobj, &ariname);

            m_string_t buf;
            m_string_init(buf);
            cace_ari_text_encode(buf, &ariname, CACE_ARI_TEXT_ENC_OPTS_DEFAULT);
            CACE_LOG_DEBUG("TBLT match for column %s, type %s", m_string_get_cstr(col->name), m_string_get_cstr(buf));
            m_string_clear(buf);
            cace_ari_deinit(&ariname);

            m_string_init(buf);
            cace_ari_text_encode(buf, val_item, CACE_ARI_TEXT_ENC_OPTS_DEFAULT);
            CACE_LOG_DEBUG("for value %s match %d", m_string_get_cstr(buf), (int)got);
            m_string_clear(buf);
        }
        if (got == CACE_AMM_TYPE_MATCH_NEGATIVE)
        {
            return CACE_AMM_TYPE_MATCH_NEGATIVE;
        }
    }

    return CACE_AMM_TYPE_MATCH_POSITIVE;
}

static int cace_amm_semtype_tblt_convert(const cace_amm_type_t *self, cace_ari_t *out, const cace_ari_t *in)
{
    const cace_amm_semtype_tblt_t *semtype = self->as_semtype;
    CHKERR1(semtype);

    const struct cace_ari_tbl_s *inval = cace_ari_cget_tbl(in);
    if (!inval)
    {
        return CACE_AMM_ERR_CONVERT_BADVALUE;
    }

    if (inval->ncols != cace_amm_named_type_array_size(semtype->columns))
    {
        return CACE_AMM_ERR_CONVERT_BADVALUE;
    }

    const size_t nrows = cace_ari_array_size(inval->items) / inval->ncols;

    cace_ari_tbl_t *outval = cace_ari_set_tbl(out, NULL);
    cace_ari_tbl_reset(outval, inval->ncols, nrows);

    int retval = 0;

    cace_amm_named_type_array_it_t col_it;
    cace_amm_named_type_array_it(col_it, semtype->columns);

    // input and output have exact same size
    cace_ari_array_it_t inval_it, outval_it;
    for (cace_ari_array_it(inval_it, inval->items), cace_ari_array_it(outval_it, outval->items);
         !cace_ari_array_end_p(inval_it);
         cace_ari_array_next(inval_it), cace_ari_array_next(outval_it), cace_amm_named_type_array_next(col_it))
    {
        const cace_ari_t *in_item  = cace_ari_array_cref(inval_it);
        cace_ari_t       *out_item = cace_ari_array_ref(outval_it);

        if (cace_amm_named_type_array_end_p(col_it))
        {
            cace_amm_named_type_array_it(col_it, semtype->columns);
        }
        const cace_amm_type_t *typeobj = &(cace_amm_named_type_array_ref(col_it)->typeobj);

        int res = cace_amm_type_convert(typeobj, out_item, in_item);
        if (res)
        {
            retval = res;
            break;
        }
    }

    return retval;
}

cace_amm_semtype_tblt_t *cace_amm_type_set_tblt_size(cace_amm_type_t *type, size_t num_cols)
{
    CHKNULL(type);
    cace_amm_type_reset(type);

    type->ari_name   = cace_amm_semtype_tblt_name;
    type->match      = cace_amm_semtype_tblt_match;
    type->convert    = cace_amm_semtype_tblt_convert;
    type->type_class = CACE_AMM_TYPE_TBLT;

    cace_amm_semtype_tblt_t *semtype = CACE_MALLOC(sizeof(cace_amm_semtype_tblt_t));
    cace_amm_semtype_tblt_init(semtype);
    cace_amm_named_type_array_resize(semtype->columns, num_cols);

    type->as_semtype        = semtype;
    type->as_semtype_deinit = (cace_amm_semtype_deinit_f)cace_amm_semtype_tblt_deinit;

    return semtype;
}

int cace_amm_type_set_union_from_name(cace_amm_type_t *type, const cace_amm_lookup_t *deref,
                                      const cace_amm_obj_store_t *store)
{
    cace_amm_semtype_union_t *semtype = cace_amm_type_set_union_size(type, 0);
    {
        cace_ari_t **pval = cace_named_ari_ptr_dict_get(deref->aparams.named, "choices");
        if (!pval)
        {
            CACE_LOG_ERR("No choices parameter");
            return 3;
        }
        const cace_ari_ac_t *pval_ac = cace_ari_cget_ac(*pval);
        if (!pval_ac)
        {
            CACE_LOG_ERR("No choices parameter as AC");
            return 3;
        }

        // grow the choices list as needed
        cace_ari_list_it_t it;
        for (cace_ari_list_it(it, pval_ac->items); !cace_ari_list_end_p(it); cace_ari_list_next(it))
        {
            const cace_ari_t *pval_item = cace_ari_list_cref(it);

            cace_amm_type_t *typeobj = cace_amm_type_array_push_new(semtype->choices);
            cace_amm_type_set_name(typeobj, pval_item, store);
        }
    }
    return 0;
}

static void cace_amm_semtype_union_name(const cace_amm_type_t *self, cace_ari_t *name)
{
    const cace_amm_semtype_union_t *semtype = self->as_semtype;
    CHKVOID(semtype);

    cace_ari_ref_t *ref = cace_ari_set_objref(name);
    cace_ari_objpath_set_textid(&(ref->objpath), "ietf", "amm-semtype", CACE_ARI_TYPE_IDENT, "union");

    cace_ari_tree_t params;
    cace_ari_tree_init(params);
    {
        cace_ari_t key = CACE_ARI_INIT_UNDEFINED;
        cace_ari_set_tstr(&key, "choices", false);
        cace_ari_t *val = cace_ari_tree_safe_get(params, key);
        cace_ari_deinit(&key);

        {
            cace_ari_ac_t *name_ac = cace_ari_set_ac(val, NULL);

            cace_amm_type_array_it_t it;
            for (cace_amm_type_array_it(it, semtype->choices); !cace_amm_type_array_end_p(it);
                 cace_amm_type_array_next(it))
            {
                const cace_amm_type_t *choice    = cace_amm_type_array_ref(it);
                cace_ari_t            *name_item = cace_ari_list_push_back_new(name_ac->items);
                cace_amm_type_get_name(choice, name_item);
            }
        }
    }
    cace_ari_params_set_am(&(ref->params), params);
}

static cace_amm_type_match_res_t cace_amm_semtype_union_match(const cace_amm_type_t *self, const cace_ari_t *ari)
{
    if (cace_ari_is_undefined(ari))
    {
        return CACE_AMM_TYPE_MATCH_UNDEFINED;
    }

    const cace_amm_semtype_union_t *semtype = self->as_semtype;
    CHKRET(semtype, CACE_AMM_TYPE_MATCH_NEGATIVE);

    cace_amm_type_array_it_t it;
    for (cace_amm_type_array_it(it, semtype->choices); !cace_amm_type_array_end_p(it); cace_amm_type_array_next(it))
    {
        const cace_amm_type_t *choice = cace_amm_type_array_ref(it);

        cace_amm_type_match_res_t got = cace_amm_type_match(choice, ari);
        if (got == CACE_AMM_TYPE_MATCH_POSITIVE)
        {
            return got;
        }
    }

    return CACE_AMM_TYPE_MATCH_NEGATIVE;
}

static int cace_amm_semtype_union_convert(const cace_amm_type_t *self, cace_ari_t *out, const cace_ari_t *in)
{
    const cace_amm_semtype_union_t *semtype = self->as_semtype;
    CHKERR1(semtype);
    CACE_LOG_DEBUG("type union with %d choices", cace_amm_type_array_size(semtype->choices));

    const cace_amm_type_t *found = NULL;
    // First try matching without conversion
    cace_amm_type_array_it_t it;
    for (cace_amm_type_array_it(it, semtype->choices); !found && !cace_amm_type_array_end_p(it);
         cace_amm_type_array_next(it))
    {
        const cace_amm_type_t *choice = cace_amm_type_array_ref(it);
        if (cace_amm_type_match(choice, in) == CACE_AMM_TYPE_MATCH_POSITIVE)
        {
            cace_ari_set_copy(out, in);
            found = choice;
        }
    }
    // FIXME: add match-only option
    //  Then try more strict conversion
    for (cace_amm_type_array_it(it, semtype->choices); !found && !cace_amm_type_array_end_p(it);
         cace_amm_type_array_next(it))
    {
        const cace_amm_type_t *choice = cace_amm_type_array_ref(it);

        int res = cace_amm_type_convert(choice, out, in);
        // first valid conversion wins
        if (res == 0)
        {
            found = choice;
        }
    }

    if (found)
    {
        if (cace_log_is_enabled_for(LOG_DEBUG))
        {
            cace_ari_t ariname = CACE_ARI_INIT_UNDEFINED;
            cace_amm_type_get_name(found, &ariname);

            m_string_t buf;
            m_string_init(buf);
            cace_ari_text_encode(buf, &ariname, CACE_ARI_TEXT_ENC_OPTS_DEFAULT);
            CACE_LOG_DEBUG("type union converted for choice %s", m_string_get_cstr(buf));
            m_string_clear(buf);
            cace_ari_deinit(&ariname);

            m_string_init(buf);
            cace_ari_text_encode(buf, out, CACE_ARI_TEXT_ENC_OPTS_DEFAULT);
            CACE_LOG_DEBUG("got value %s", m_string_get_cstr(buf));
            m_string_clear(buf);
        }
        return 0;
    }
    else
    {
        CACE_LOG_DEBUG("type union found no choice");
        return CACE_AMM_ERR_CONVERT_NOCHOICE;
    }
}

cace_amm_semtype_union_t *cace_amm_type_set_union_size(cace_amm_type_t *type, size_t num_choices)
{
    CHKNULL(type);
    cace_amm_type_reset(type);

    type->ari_name   = cace_amm_semtype_union_name;
    type->match      = cace_amm_semtype_union_match;
    type->convert    = cace_amm_semtype_union_convert;
    type->type_class = CACE_AMM_TYPE_UNION;

    cace_amm_semtype_union_t *semtype = CACE_MALLOC(sizeof(cace_amm_semtype_union_t));
    cace_amm_semtype_union_init(semtype);
    cace_amm_type_array_resize(semtype->choices, num_choices);

    type->as_semtype        = semtype;
    type->as_semtype_deinit = (cace_amm_semtype_deinit_f)cace_amm_semtype_union_deinit;

    return semtype;
}

int cace_amm_type_set_seq_from_name(cace_amm_type_t *type, const cace_amm_lookup_t *deref,
                                    const cace_amm_obj_store_t *store)
{
    cace_amm_semtype_seq_t *semtype = cace_amm_type_set_seq(type);
    {
        cace_ari_t **pval = cace_named_ari_ptr_dict_get(deref->aparams.named, "item-type");
        if (!pval)
        {
            CACE_LOG_ERR("No item-type parameter");
            return 3;
        }
        cace_amm_type_set_name(&(semtype->item_type), *pval, store);
    }
    return 0;
}

static void cace_amm_semtype_seq_name(const cace_amm_type_t *self, cace_ari_t *name)
{
    const cace_amm_semtype_seq_t *semtype = self->as_semtype;
    CHKVOID(semtype);

    cace_ari_ref_t *ref = cace_ari_set_objref(name);
    cace_ari_objpath_set_textid(&(ref->objpath), "ietf", "amm-semtype", CACE_ARI_TYPE_IDENT, "seq");

    cace_ari_tree_t params;
    cace_ari_tree_init(params);
    {
        cace_ari_t key = CACE_ARI_INIT_UNDEFINED;
        cace_ari_set_tstr(&key, "item-type", false);
        cace_ari_t *val = cace_ari_tree_safe_get(params, key);
        cace_ari_deinit(&key);

        cace_amm_type_get_name(&semtype->item_type, val);
    }
    cace_ari_params_set_am(&(ref->params), params);
}

cace_amm_semtype_seq_t *cace_amm_type_set_seq(cace_amm_type_t *type)
{
    CHKNULL(type);
    cace_amm_type_reset(type);

    type->ari_name = cace_amm_semtype_seq_name;
    // not usable as a direct semantic type, see cace_amm_semtype_dlist_match() and cace_amm_semtype_dlist_convert()
    type->match      = NULL;
    type->convert    = NULL;
    type->type_class = CACE_AMM_TYPE_SEQ;

    cace_amm_semtype_seq_t *semtype = CACE_MALLOC(sizeof(cace_amm_semtype_seq_t));
    cace_amm_semtype_seq_init(semtype);

    type->as_semtype        = semtype;
    type->as_semtype_deinit = (cace_amm_semtype_deinit_f)cace_amm_semtype_seq_deinit;

    return semtype;
}
