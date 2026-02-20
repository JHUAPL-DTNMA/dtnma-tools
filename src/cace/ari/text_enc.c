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
#include "text.h"
#include "text_util.h"
#include "access.h"
#include "objpat.h"
#include "cace/util/defs.h"
#include "cace/util/logging.h"
#include <inttypes.h>

typedef struct
{
    /// Output string stream
    m_string_t out;
    /// Current nesting depth. The top ARI is depth zero.
    int depth;
    /// Original encoding options
    cace_ari_text_enc_opts_t *opts;
} cace_ari_text_enc_state_t;

/** Additional safe characters for ARI text and byte strings as defined in
 * Section 4.1 of @cite ietf-dtn-ari-00.
 */
static const char uri_safe[] = "'";

/** Perform percent encoding from a temporary buffer.
 *
 * @param[out] out The text to append to.
 * @param[in,out] buf The buffer to move from and clear.
 */
static int cace_ari_text_percent_helper(m_string_t out, m_string_t buf)
{
    cace_data_t view;
    cace_data_init_view(&view, m_string_size(buf) + 1, (cace_data_ptr_t)m_string_get_cstr(buf));

    int retval = cace_uri_percent_encode(out, &view, uri_safe);
    m_string_clear(buf);
    return retval;
}

static int cace_ari_text_encode_stream(cace_ari_text_enc_state_t *state, const cace_ari_t *ari);

int cace_ari_text_encode(m_string_t text, const cace_ari_t *ari, cace_ari_text_enc_opts_t opts)
{
    CHKERR1(text);
    CHKERR1(ari);

    cace_ari_text_enc_state_t state = {
        .depth = 0,
        .opts  = &opts,
    };
    m_string_init(state.out);

    if (cace_ari_text_encode_stream(&state, ari))
    {
        return 2;
    }

    m_string_move(text, state.out);
    return 0;
}

static int cace_ari_text_encode_ac(cace_ari_text_enc_state_t *state, const cace_ari_ac_t *ctr)
{
    ++(state->depth);
    m_string_push_back(state->out, '(');

    int  retval = 0;
    bool sep    = false;

    cace_ari_list_it_t item_it;
    for (cace_ari_list_it(item_it, ctr->items); !cace_ari_list_end_p(item_it); cace_ari_list_next(item_it))
    {
        const cace_ari_t *item = cace_ari_list_cref(item_it);

        if (sep)
        {
            m_string_push_back(state->out, ',');
        }
        sep = true;

        int ret = cace_ari_text_encode_stream(state, item);
        if (ret)
        {
            retval = 2;
            break;
        }
    }

    --(state->depth);
    m_string_push_back(state->out, ')');
    return retval;
}

static int cace_ari_text_encode_am(cace_ari_text_enc_state_t *state, const cace_ari_am_t *ctr)
{
    ++(state->depth);
    m_string_push_back(state->out, '(');

    int  retval = 0;
    bool sep    = false;

    cace_ari_tree_it_t item_it;
    for (cace_ari_tree_it(item_it, ctr->items); !cace_ari_tree_end_p(item_it); cace_ari_tree_next(item_it))
    {
        const cace_ari_tree_itref_t *pair = cace_ari_tree_cref(item_it);

        if (sep)
        {
            m_string_push_back(state->out, ',');
        }
        sep = true;

        int ret = cace_ari_text_encode_stream(state, pair->key_ptr);
        if (ret)
        {
            retval = 2;
            break;
        }

        m_string_push_back(state->out, '=');

        ret = cace_ari_text_encode_stream(state, pair->value_ptr);
        if (ret)
        {
            retval = 2;
            break;
        }
    }

    --(state->depth);
    m_string_push_back(state->out, ')');
    return retval;
}

static int cace_ari_text_encode_tbl(cace_ari_text_enc_state_t *state, const cace_ari_tbl_t *ctr)
{
    m_string_cat_printf(state->out, "c=%zu;", ctr->ncols);

    if (ctr->ncols == 0)
    {
        // nothing to do
        return 0;
    }

    ++(state->depth);
    const size_t nrows = cace_ari_array_size(ctr->items) / ctr->ncols;

    int retval = 0;

    cace_ari_array_it_t item_it;
    cace_ari_array_it(item_it, ctr->items);
    for (size_t row_ix = 0; row_ix < nrows; ++row_ix)
    {
        m_string_push_back(state->out, '(');

        bool sep = false;
        for (size_t col_ix = 0; col_ix < ctr->ncols; ++col_ix)
        {
            const cace_ari_t *item = cace_ari_array_cref(item_it);

            if (sep)
            {
                m_string_push_back(state->out, ',');
            }
            sep = true;

            int ret = cace_ari_text_encode_stream(state, item);
            if (ret)
            {
                retval = 2;
                break;
            }

            cace_ari_array_next(item_it);
        }

        m_string_push_back(state->out, ')');
        if (retval)
        {
            break;
        }
    }

    --(state->depth);
    return retval;
}

static int cace_ari_text_encode_execset(cace_ari_text_enc_state_t *state, const cace_ari_execset_t *ctr)
{
    {
        cace_ari_text_enc_opts_t saveopts = *(state->opts);
        state->opts->scheme_prefix        = CACE_ARI_TEXT_SCHEME_NONE;

        m_string_cat_cstr(state->out, "n=");
        cace_ari_text_encode_stream(state, &(ctr->nonce));
        m_string_push_back(state->out, ';');

        *(state->opts) = saveopts;
    }

    ++(state->depth);
    m_string_push_back(state->out, '(');

    int  retval = 0;
    bool sep    = false;

    cace_ari_list_it_t item_it;
    for (cace_ari_list_it(item_it, ctr->targets); !cace_ari_list_end_p(item_it); cace_ari_list_next(item_it))
    {
        const cace_ari_t *item = cace_ari_list_cref(item_it);

        if (sep)
        {
            m_string_push_back(state->out, ',');
        }
        sep = true;

        int ret = cace_ari_text_encode_stream(state, item);
        if (ret)
        {
            retval = 2;
            break;
        }
    }

    --(state->depth);
    m_string_push_back(state->out, ')');
    return retval;
}

static int cace_ari_text_encode_report(cace_ari_text_enc_state_t *state, const cace_ari_report_t *rpt)
{
    {
        cace_ari_text_enc_opts_t saveopts = *(state->opts);
        state->opts->scheme_prefix        = CACE_ARI_TEXT_SCHEME_NONE;

        m_string_cat_cstr(state->out, "t=");
        cace_ari_text_encode_stream(state, &(rpt->reltime));
        m_string_push_back(state->out, ';');

        m_string_cat_cstr(state->out, "s=");
        cace_ari_text_encode_stream(state, &(rpt->source));
        m_string_push_back(state->out, ';');

        *(state->opts) = saveopts;
    }

    m_string_push_back(state->out, '(');

    int  retval = 0;
    bool sep    = false;

    cace_ari_list_it_t item_it;
    for (cace_ari_list_it(item_it, rpt->items); !cace_ari_list_end_p(item_it); cace_ari_list_next(item_it))
    {
        const cace_ari_t *item = cace_ari_list_cref(item_it);

        if (sep)
        {
            m_string_push_back(state->out, ',');
        }
        sep = true;

        int ret = cace_ari_text_encode_stream(state, item);
        if (ret)
        {
            retval = 2;
            break;
        }
    }

    m_string_push_back(state->out, ')');
    return retval;
}

static int cace_ari_text_encode_rptset(cace_ari_text_enc_state_t *state, const cace_ari_rptset_t *ctr)
{
    ++(state->depth);

    {
        cace_ari_text_enc_opts_t saveopts = *(state->opts);
        state->opts->scheme_prefix        = CACE_ARI_TEXT_SCHEME_NONE;

        m_string_cat_cstr(state->out, "n=");
        cace_ari_text_encode_stream(state, &(ctr->nonce));
        m_string_push_back(state->out, ';');

        m_string_cat_cstr(state->out, "r=");
        cace_ari_text_encode_stream(state, &(ctr->reftime));
        m_string_push_back(state->out, ';');

        *(state->opts) = saveopts;
    }

    m_string_push_back(state->out, '(');

    int  retval = 0;
    bool sep    = false;

    cace_ari_report_list_it_t rpt_it;
    for (cace_ari_report_list_it(rpt_it, ctr->reports); !cace_ari_report_list_end_p(rpt_it);
         cace_ari_report_list_next(rpt_it))
    {
        const cace_ari_report_t *rpt = cace_ari_report_list_cref(rpt_it);

        if (sep)
        {
            m_string_push_back(state->out, ',');
        }
        sep = true;

        int ret = cace_ari_text_encode_report(state, rpt);
        if (ret)
        {
            retval = 2;
            break;
        }
    }

    m_string_push_back(state->out, ')');

    --(state->depth);
    return retval;
}

static void cace_ari_text_encode_objpat_part(cace_ari_text_enc_state_t *state, const cace_ari_objpat_part_t part)
{
    m_string_push_back(state->out, '(');

    const cace_util_range_int64_t *range_int64 = NULL;
    const m_string_t              *text        = NULL;
    if (cace_ari_objpat_part_cget_special(part))
    {
        m_string_push_back(state->out, '*');
    }
    else if ((range_int64 = cace_ari_objpat_part_cget_range_int64(part)))
    {
        bool sep = false;

        cace_util_range_int64_it_t it;
        for (cace_util_range_int64_it(it, *range_int64); !cace_util_range_int64_end_p(it);
             cace_util_range_int64_next(it))
        {
            if (sep)
            {
                m_string_push_back(state->out, ',');
            }
            sep = true;

            const cace_util_range_intvl_int64_t *intvl = cace_util_range_int64_cref(it);
            if (intvl->has_min && intvl->has_max && (intvl->i_min == intvl->i_max))
            {
                // FIXME decimal
                cace_ari_int64_encode(state->out, intvl->i_min, 10);
            }
            else
            {
                if (intvl->has_min)
                {
                    cace_ari_int64_encode(state->out, intvl->i_min, 10);
                }
                m_string_cat_cstr(state->out, "..");
                if (intvl->has_max)
                {
                    cace_ari_int64_encode(state->out, intvl->i_max, 10);
                }
            }
        }
    }
    else if ((text = cace_ari_objpat_part_cget_text(part)))
    {
        m_string_cat(state->out, *text);
    }
    else
    {
        CACE_LOG_ERR("Invalid OBJPAT part state");
    }

    m_string_push_back(state->out, ')');
}

static void cace_ari_text_encode_prefix(cace_ari_text_enc_state_t *state)
{
    switch (state->opts->scheme_prefix)
    {
        case CACE_ARI_TEXT_SCHEME_NONE:
            return;
        case CACE_ARI_TEXT_SCHEME_FIRST:
            if (state->depth > 0)
            {
                return;
            }
            break;
        case CACE_ARI_TEXT_SCHEME_ALL:
            break;
    }

    m_string_cat_cstr(state->out, "ari:");
}

static bool cace_ari_text_encode_idseg(m_string_t text, const cace_ari_idseg_t *obj);

static void cace_ari_text_encode_aritype(m_string_t text, enum cace_ari_text_aritype_e show, const cace_ari_type_t val,
                                         const cace_ari_idseg_t *idseg)
{
    const char *name;
    switch (show)
    {
        case CACE_ARI_TEXT_ARITYPE_TEXT:
            name = cace_ari_type_to_name(val);
            break;
        case CACE_ARI_TEXT_ARITYPE_INT:
            name = NULL;
            break;
        default:
            if (idseg)
            {
                cace_ari_text_encode_idseg(text, idseg);
                return;
            }
            else
            {
                name = cace_ari_type_to_name(val);
            }
            break;
    }

    if (name)
    {
        m_string_cat_cstr(text, name);
    }
    else
    {
        m_string_cat_printf(text, "%" PRId64, val);
    }
}

static int cace_ari_text_encode_lit(cace_ari_text_enc_state_t *state, const cace_ari_lit_t *obj, const cace_ari_t *ari)
{
    cace_ari_text_encode_prefix(state);

    if (obj->has_ari_type)
    {
        m_string_push_back(state->out, '/');
        cace_ari_text_encode_aritype(state->out, state->opts->show_ari_type, obj->ari_type, NULL);
        m_string_push_back(state->out, '/');

        switch (obj->ari_type)
        {
            case CACE_ARI_TYPE_TP:
                if (state->opts->time_text)
                {
                    // never use separators
                    if (cace_utctime_encode(state->out, &(obj->value.as_timespec), false))
                    {
                        return 2;
                    }
                }
                else
                {
                    if (cace_decfrac_encode(state->out, &(obj->value.as_timespec)))
                    {
                        return 2;
                    }
                }
                break;
            case CACE_ARI_TYPE_TD:
                if (state->opts->time_text)
                {
                    if (cace_timeperiod_encode(state->out, &(obj->value.as_timespec)))
                    {
                        return 2;
                    }
                }
                else
                {
                    if (cace_decfrac_encode(state->out, &(obj->value.as_timespec)))
                    {
                        return 2;
                    }
                }
                break;
            case CACE_ARI_TYPE_AC:
                cace_ari_text_encode_ac(state, obj->value.as_ac);
                break;
            case CACE_ARI_TYPE_AM:
                cace_ari_text_encode_am(state, obj->value.as_am);
                break;
            case CACE_ARI_TYPE_TBL:
                cace_ari_text_encode_tbl(state, obj->value.as_tbl);
                break;
            case CACE_ARI_TYPE_EXECSET:
                cace_ari_text_encode_execset(state, obj->value.as_execset);
                break;
            case CACE_ARI_TYPE_RPTSET:
                cace_ari_text_encode_rptset(state, obj->value.as_rptset);
                break;
            case CACE_ARI_TYPE_OBJPAT:
                cace_ari_text_encode_objpat_part(state, obj->value.as_objpat->org_pat);
                cace_ari_text_encode_objpat_part(state, obj->value.as_objpat->model_pat);
                cace_ari_text_encode_objpat_part(state, obj->value.as_objpat->type_pat);
                cace_ari_text_encode_objpat_part(state, obj->value.as_objpat->obj_pat);
                break;
            default:
                // Fall through to primitives below
                break;
        }
    }

    switch (obj->prim_type)
    {
        case CACE_ARI_PRIM_UNDEFINED:
            m_string_cat_cstr(state->out, "undefined");
            break;
        case CACE_ARI_PRIM_NULL:
            m_string_cat_cstr(state->out, "null");
            break;
        case CACE_ARI_PRIM_BOOL:
            if (obj->value.as_bool)
            {
                m_string_cat_cstr(state->out, "true");
            }
            else
            {
                m_string_cat_cstr(state->out, "false");
            }
            break;
        case CACE_ARI_PRIM_UINT64:
            if (cace_ari_uint64_encode(state->out, obj->value.as_uint64, (int)(state->opts->int_base)))
            {
                return 2;
            }
            break;
        case CACE_ARI_PRIM_INT64:
        {
            if (cace_ari_int64_encode(state->out, obj->value.as_int64, (int)(state->opts->int_base)))
            {
                return 2;
            }
            break;
        }
        case CACE_ARI_PRIM_FLOAT64:
            cace_ari_float64_encode(state->out, obj->value.as_float64, state->opts->float_form);
            break;
        case CACE_ARI_PRIM_TSTR:
        {
            if (state->opts->text_identity && cace_ari_text_is_identity(&(obj->value.as_data)))
            {
                m_string_cat_cstr(state->out, (const char *)(obj->value.as_data.ptr));
            }
            else
            {
                m_string_t buf;
                m_string_init(buf);
                m_string_push_back(buf, '"');
                cace_slash_escape(buf, &(obj->value.as_data), '"');
                m_string_push_back(buf, '"');

                cace_ari_text_percent_helper(state->out, buf);
            }
            break;
        }
        case CACE_ARI_PRIM_BSTR:
            switch (state->opts->bstr_form)
            {
                case CACE_ARI_TEXT_BSTR_RAW:
                {
                    // force null termination for this output mode
                    cace_data_t terminated;
                    cace_data_init_set(&terminated, &(obj->value.as_data));
                    cace_data_append_byte(&terminated, 0x0);

                    if (cace_data_is_utf8(&terminated))
                    {
                        // leave outer quotes non-percent-encoded
                        m_string_push_back(state->out, '\'');

                        m_string_t buf;
                        m_string_init(buf);
                        cace_slash_escape(buf, &terminated, '\'');
                        cace_ari_text_percent_helper(state->out, buf);

                        m_string_push_back(state->out, '\'');
                    }
                    else
                    {
                        // this value cannot be represented as text
                        m_string_cat_cstr(state->out, "h'");
                        cace_base16_encode(state->out, &(obj->value.as_data), true);
                        m_string_push_back(state->out, '\'');
                    }

                    cace_data_deinit(&terminated);
                    break;
                }
                case CACE_ARI_TEXT_BSTR_BASE16:
                    // no need to percent encode
                    m_string_cat_cstr(state->out, "h'");
                    cace_base16_encode(state->out, &(obj->value.as_data), true);
                    m_string_push_back(state->out, '\'');
                    break;
                case CACE_ARI_TEXT_BSTR_BASE64URL:
                    // no need to percent encode
                    m_string_cat_cstr(state->out, "b64'");
                    cace_base64_encode(state->out, &(obj->value.as_data), true, false);
                    m_string_push_back(state->out, '\'');
                    break;
            }
            break;
        case CACE_ARI_PRIM_TIMESPEC:
        case CACE_ARI_PRIM_OTHER:
            // already handled above
            break;
    }
    return 0;
}

static bool cace_ari_text_encode_idseg(m_string_t text, const cace_ari_idseg_t *obj)
{
    switch (obj->form)
    {
        case CACE_ARI_IDSEG_NULL:
            break;
        case CACE_ARI_IDSEG_TEXT:
            m_string_cat(text, obj->as_text);
            return true;
        case CACE_ARI_IDSEG_INT:
            m_string_cat_printf(text, "%" PRId64, obj->as_int);
            return true;
    }
    return false;
}

int cace_ari_text_encode_objpath(m_string_t text, const cace_ari_objpath_t *path, enum cace_ari_text_aritype_e show)
{
    CHKERR1(text);
    CHKERR1(path);

    if (path->org_id.form != CACE_ARI_IDSEG_NULL)
    {
        m_string_cat_cstr(text, "//");
        cace_ari_text_encode_idseg(text, &(path->org_id));
    }
    else
    {
        if (path->model_id.form != CACE_ARI_IDSEG_NULL)
        {
            m_string_cat_cstr(text, "..");
        }
        else
        {
            m_string_cat_cstr(text, ".");
        }
    }
    m_string_push_back(text, '/');

    if (cace_ari_text_encode_idseg(text, &(path->model_id)))
    {
        if (path->model_rev.valid)
        {
            m_string_push_back(text, '@');
            cace_date_encode(text, &(path->model_rev.parts), true);
        }

        m_string_push_back(text, '/');
    }

    if (path->has_ari_type)
    {
        cace_ari_text_encode_aritype(text, show, path->ari_type, &(path->type_id));
        m_string_push_back(text, '/');
    }
    else if (cace_ari_text_encode_idseg(text, &(path->type_id)))
    {
        m_string_push_back(text, '/');
    }

    // may encode nothing
    cace_ari_text_encode_idseg(text, &(path->obj_id));

    return 0;
}

static int cace_ari_text_encode_objref(cace_ari_text_enc_state_t *state, const cace_ari_ref_t *obj)
{
    // no scheme for path-only URI Reference form
    if (obj->objpath.org_id.form != CACE_ARI_IDSEG_NULL)
    {
        cace_ari_text_encode_prefix(state);
    }

    if (cace_ari_text_encode_objpath(state->out, &(obj->objpath), state->opts->show_ari_type))
    {
        return 2;
    }

    switch (obj->params.state)
    {
        case CACE_ARI_PARAMS_NONE:
            // nothing to do
            break;
        case CACE_ARI_PARAMS_AC:
            cace_ari_text_encode_ac(state, obj->params.as_ac);
            break;
        case CACE_ARI_PARAMS_AM:
            cace_ari_text_encode_am(state, obj->params.as_am);
            break;
    }
    return 0;
}

static int cace_ari_text_encode_stream(cace_ari_text_enc_state_t *state, const cace_ari_t *ari)
{
    if (ari->is_ref)
    {
        const cace_ari_ref_t *obj = &(ari->as_ref);
        cace_ari_text_encode_objref(state, obj);
    }
    else
    {
        const cace_ari_lit_t *obj = &(ari->as_lit);
        cace_ari_text_encode_lit(state, obj, ari);
    }

    return 0;
}
