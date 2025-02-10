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
#include "text.h"
#include "text_util.h"
#include "cace/util/defs.h"
#include <inttypes.h>

typedef struct
{
    /// Output string stream
    string_t out;
    /// Current nesting depth. The top ARI is depth zero.
    int depth;
    /// Original encoding options
    cace_ari_text_enc_opts_t *opts;
} cace_ari_text_enc_state_t;

/** Additional safe characters for ARI as defined in
 * Section 4.1 of @cite ietf-dtn-ari-00.
 */
static const char uri_safe[] = "!'+:@";

/** Perform percent encoding from a temporary buffer.
 *
 * @param[out] out The text to append to.
 * @param[in,out] buf The buffer to move from and clear.
 */
static int cace_ari_text_percent_helper(string_t out, string_t buf)
{
    cace_data_t view;
    cace_data_init_view(&view, string_size(buf) + 1, (cace_data_ptr_t)string_get_cstr(buf));

    int retval = cace_uri_percent_encode(out, &view, uri_safe);
    string_clear(buf);
    return retval;
}

static int cace_ari_text_encode_stream(cace_ari_text_enc_state_t *state, const cace_ari_t *ari);

int cace_ari_text_encode(string_t text, const cace_ari_t *ari, cace_ari_text_enc_opts_t opts)
{
    CHKERR1(text);
    CHKERR1(ari);

    cace_ari_text_enc_state_t state = {
        .depth = 0,
        .opts  = &opts,
    };
    string_init(state.out);

    if (cace_ari_text_encode_stream(&state, ari))
    {
        return 2;
    }

    string_move(text, state.out);
    return 0;
}

static int cace_ari_text_encode_ac(cace_ari_text_enc_state_t *state, const cace_ari_ac_t *ctr)
{
    ++(state->depth);
    string_push_back(state->out, '(');

    int  retval = 0;
    bool sep    = false;

    cace_ari_list_it_t item_it;
    for (cace_ari_list_it(item_it, ctr->items); !cace_ari_list_end_p(item_it); cace_ari_list_next(item_it))
    {
        if (sep)
        {
            string_push_back(state->out, ',');
        }
        sep = true;

        const cace_ari_t *item = cace_ari_list_cref(item_it);

        int ret = cace_ari_text_encode_stream(state, item);
        if (ret)
        {
            retval = 2;
            break;
        }
    }

    --(state->depth);
    string_push_back(state->out, ')');
    return retval;
}

static int cace_ari_text_encode_am(cace_ari_text_enc_state_t *state, const cace_ari_am_t *ctr)
{
    ++(state->depth);
    string_push_back(state->out, '(');

    int  retval = 0;
    bool sep    = false;

    cace_ari_tree_it_t item_it;
    for (cace_ari_tree_it(item_it, ctr->items); !cace_ari_tree_end_p(item_it); cace_ari_tree_next(item_it))
    {
        if (sep)
        {
            string_push_back(state->out, ',');
        }
        sep = true;

        const cace_ari_tree_itref_t *pair = cace_ari_tree_cref(item_it);

        int ret = cace_ari_text_encode_stream(state, pair->key_ptr);
        if (ret)
        {
            retval = 2;
            break;
        }

        string_push_back(state->out, '=');

        ret = cace_ari_text_encode_stream(state, pair->value_ptr);
        if (ret)
        {
            retval = 2;
            break;
        }
    }

    --(state->depth);
    string_push_back(state->out, ')');
    return retval;
}

static int cace_ari_text_encode_tbl(cace_ari_text_enc_state_t *state, const cace_ari_tbl_t *ctr)
{
    string_cat_printf(state->out, "c=%zu;", ctr->ncols);

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
        string_push_back(state->out, '(');

        bool sep = false;
        for (size_t col_ix = 0; col_ix < ctr->ncols; ++col_ix)
        {
            if (sep)
            {
                string_push_back(state->out, ',');
            }
            sep = true;

            const cace_ari_t *item = cace_ari_array_cref(item_it);

            int ret = cace_ari_text_encode_stream(state, item);
            if (ret)
            {
                retval = 2;
                break;
            }

            cace_ari_array_next(item_it);
        }

        string_push_back(state->out, ')');
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

        string_cat_str(state->out, "n=");
        cace_ari_text_encode_stream(state, &(ctr->nonce));
        string_push_back(state->out, ';');

        *(state->opts) = saveopts;
    }

    ++(state->depth);
    string_push_back(state->out, '(');

    int  retval = 0;
    bool sep    = false;

    cace_ari_list_it_t item_it;
    for (cace_ari_list_it(item_it, ctr->targets); !cace_ari_list_end_p(item_it); cace_ari_list_next(item_it))
    {
        if (sep)
        {
            string_push_back(state->out, ',');
        }
        sep = true;

        const cace_ari_t *item = cace_ari_list_cref(item_it);

        int ret = cace_ari_text_encode_stream(state, item);
        if (ret)
        {
            retval = 2;
            break;
        }
    }

    --(state->depth);
    string_push_back(state->out, ')');
    return retval;
}

static int cace_ari_text_encode_report(cace_ari_text_enc_state_t *state, const cace_ari_report_t *rpt)
{
    string_push_back(state->out, '(');
    {
        cace_ari_text_enc_opts_t saveopts = *(state->opts);
        state->opts->scheme_prefix        = CACE_ARI_TEXT_SCHEME_NONE;

        string_cat_str(state->out, "t=");
        cace_ari_text_encode_stream(state, &(rpt->reltime));
        string_push_back(state->out, ';');

        string_cat_str(state->out, "s=");
        cace_ari_text_encode_stream(state, &(rpt->source));
        string_push_back(state->out, ';');

        *(state->opts) = saveopts;
    }

    string_push_back(state->out, '(');

    int  retval = 0;
    bool sep    = false;

    cace_ari_list_it_t item_it;
    for (cace_ari_list_it(item_it, rpt->items); !cace_ari_list_end_p(item_it); cace_ari_list_next(item_it))
    {
        if (sep)
        {
            string_push_back(state->out, ',');
        }
        sep = true;

        const cace_ari_t *item = cace_ari_list_cref(item_it);

        int ret = cace_ari_text_encode_stream(state, item);
        if (ret)
        {
            retval = 2;
            break;
        }
    }

    string_push_back(state->out, ')');
    string_push_back(state->out, ')');
    return retval;
}

static int cace_ari_text_encode_rptset(cace_ari_text_enc_state_t *state, const cace_ari_rptset_t *ctr)
{
    ++(state->depth);

    {
        cace_ari_text_enc_opts_t saveopts = *(state->opts);
        state->opts->scheme_prefix        = CACE_ARI_TEXT_SCHEME_NONE;

        string_cat_str(state->out, "n=");
        cace_ari_text_encode_stream(state, &(ctr->nonce));
        string_push_back(state->out, ';');

        string_cat_str(state->out, "r=");
        cace_ari_text_encode_stream(state, &(ctr->reftime));
        string_push_back(state->out, ';');

        *(state->opts) = saveopts;
    }

    int retval = 0;

    cace_ari_report_list_it_t rpt_it;
    for (cace_ari_report_list_it(rpt_it, ctr->reports); !cace_ari_report_list_end_p(rpt_it);
         cace_ari_report_list_next(rpt_it))
    {
        const cace_ari_report_t *rpt = cace_ari_report_list_cref(rpt_it);

        int ret = cace_ari_text_encode_report(state, rpt);
        if (ret)
        {
            retval = 2;
            break;
        }
    }

    --(state->depth);
    return retval;
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

    string_cat_str(state->out, "ari:");
}

static bool cace_ari_text_encode_idseg(string_t text, const cace_ari_idseg_t *obj);

static void cace_ari_text_encode_aritype(string_t text, enum cace_ari_text_aritype_e show, const cace_ari_type_t val,
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
        string_cat_str(text, name);
    }
    else
    {
        string_cat_printf(text, "%" PRId64, val);
    }
}

static int cace_ari_text_encode_lit(cace_ari_text_enc_state_t *state, const cace_ari_lit_t *obj)
{
    cace_ari_text_encode_prefix(state);

    if (obj->has_ari_type)
    {
        string_push_back(state->out, '/');
        cace_ari_text_encode_aritype(state->out, state->opts->show_ari_type, obj->ari_type, NULL);
        string_push_back(state->out, '/');

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
            default:
                // Fall through to primitives below
                break;
        }
    }

    switch (obj->prim_type)
    {
        case CACE_ARI_PRIM_UNDEFINED:
            string_cat_str(state->out, "undefined");
            break;
        case CACE_ARI_PRIM_NULL:
            string_cat_str(state->out, "null");
            break;
        case CACE_ARI_PRIM_BOOL:
            if (obj->value.as_bool)
            {
                string_cat_str(state->out, "true");
            }
            else
            {
                string_cat_str(state->out, "false");
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
            uint64_t absval;
            if (obj->value.as_int64 < 0)
            {
                absval = -obj->value.as_int64;
                string_push_back(state->out, '-');
            }
            else
            {
                absval = obj->value.as_int64;
            }
            if (cace_ari_uint64_encode(state->out, absval, (int)(state->opts->int_base)))
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
                string_t buf;
                string_init(buf);
                string_push_back(buf, '"');
                cace_slash_escape(buf, &(obj->value.as_data), '"');
                string_push_back(buf, '"');

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
                        string_t buf;
                        string_init(buf);
                        string_push_back(buf, '\'');
                        cace_slash_escape(buf, &terminated, '\'');
                        string_push_back(buf, '\'');

                        cace_ari_text_percent_helper(state->out, buf);
                    }
                    else
                    {
                        // this value cannot be represented as text
                        string_cat_str(state->out, "h'");
                        cace_base16_encode(state->out, &(obj->value.as_data), true);
                        string_push_back(state->out, '\'');
                    }

                    cace_data_deinit(&terminated);
                    break;
                }
                case CACE_ARI_TEXT_BSTR_BASE16:
                    // no need to percent encode
                    string_cat_str(state->out, "h'");
                    cace_base16_encode(state->out, &(obj->value.as_data), true);
                    string_push_back(state->out, '\'');
                    break;
                case CACE_ARI_TEXT_BSTR_BASE64URL:
                    // no need to percent encode
                    string_cat_str(state->out, "b64'");
                    cace_base64_encode(state->out, &(obj->value.as_data), true);
                    string_push_back(state->out, '\'');
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

static bool cace_ari_text_encode_idseg(string_t text, const cace_ari_idseg_t *obj)
{
    switch (obj->form)
    {
        case CACE_ARI_IDSEG_NULL:
            break;
        case CACE_ARI_IDSEG_TEXT:
            string_cat(text, obj->as_text);
            return true;
        case CACE_ARI_IDSEG_INT:
            string_cat_printf(text, "%" PRId64, obj->as_int);
            return true;
    }
    return false;
}

int cace_ari_text_encode_objpath(string_t text, const cace_ari_objpath_t *path, enum cace_ari_text_aritype_e show)
{
    CHKERR1(text);
    CHKERR1(path);

    if (path->org_id.form != CACE_ARI_IDSEG_NULL)
    {
        string_cat_str(text, "//");
        cace_ari_text_encode_idseg(text, &(path->org_id));
    }
    else
    {
        if (path->org_id.form != CACE_ARI_IDSEG_NULL)
        {
            string_cat_str(text, "..");
        }
        else
        {
            string_cat_str(text, ".");
        }
    }
    string_push_back(text, '/');

    if (cace_ari_text_encode_idseg(text, &(path->org_id)))
    {
        if (path->model_rev.valid)
        {
            string_cat_str(text, "@");
            cace_date_encode(text, &(path->model_rev.parts), true);
        }

        string_cat_str(text, "/");
    }

    if (path->has_ari_type)
    {
        cace_ari_text_encode_aritype(text, show, path->ari_type, &(path->type_id));
        string_push_back(text, '/');
    }
    else if (cace_ari_text_encode_idseg(text, &(path->type_id)))
    {
        string_push_back(text, '/');
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
        cace_ari_text_encode_lit(state, obj);
    }

    return 0;
}
