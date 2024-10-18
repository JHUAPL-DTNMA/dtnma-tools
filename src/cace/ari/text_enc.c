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
    ari_text_enc_opts_t *opts;
} ari_text_enc_state_t;

/** Additional safe characters for ARI as defined in
 * Section 4.1 of @cite ietf-dtn-ari-00.
 */
static const char uri_safe[] = "!'+:@";

/** Perform percent encoding from a temporary buffer.
 *
 * @param[out] out The text to append to.
 * @param[in,out] buf The buffer to move from and clear.
 */
static int ari_text_percent_helper(string_t out, string_t buf)
{
    cace_data_t view;
    cace_data_init_view(&view, string_size(buf) + 1, (cace_data_ptr_t)string_get_cstr(buf));

    int retval = uri_percent_encode(out, &view, uri_safe);
    string_clear(buf);
    return retval;
}

static int ari_text_encode_stream(ari_text_enc_state_t *state, const ari_t *ari);

int ari_text_encode(string_t text, const ari_t *ari, ari_text_enc_opts_t opts)
{
    CHKERR1(text);
    CHKERR1(ari);

    ari_text_enc_state_t state = {
        .depth = 0,
        .opts  = &opts,
    };
    string_init(state.out);

    if (ari_text_encode_stream(&state, ari))
    {
        return 2;
    }

    string_move(text, state.out);
    return 0;
}

static int ari_text_encode_ac(ari_text_enc_state_t *state, const ari_ac_t *ctr)
{
    ++(state->depth);
    string_push_back(state->out, '(');

    int  retval = 0;
    bool sep    = false;

    ari_list_it_t item_it;
    for (ari_list_it(item_it, ctr->items); !ari_list_end_p(item_it); ari_list_next(item_it))
    {
        if (sep)
        {
            string_push_back(state->out, ',');
        }
        sep = true;

        const ari_t *item = ari_list_cref(item_it);

        int ret = ari_text_encode_stream(state, item);
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

static int ari_text_encode_am(ari_text_enc_state_t *state, const ari_am_t *ctr)
{
    ++(state->depth);
    string_push_back(state->out, '(');

    int  retval = 0;
    bool sep    = false;

    ari_tree_it_t item_it;
    for (ari_tree_it(item_it, ctr->items); !ari_tree_end_p(item_it); ari_tree_next(item_it))
    {
        if (sep)
        {
            string_push_back(state->out, ',');
        }
        sep = true;

        const ari_tree_itref_t *pair = ari_tree_cref(item_it);

        int ret = ari_text_encode_stream(state, pair->key_ptr);
        if (ret)
        {
            retval = 2;
            break;
        }

        string_push_back(state->out, '=');

        ret = ari_text_encode_stream(state, pair->value_ptr);
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

static int ari_text_encode_tbl(ari_text_enc_state_t *state, const ari_tbl_t *ctr)
{
    string_cat_printf(state->out, "c=%zu;", ctr->ncols);

    if (ctr->ncols == 0)
    {
        // nothing to do
        return 0;
    }

    ++(state->depth);
    const size_t nrows = ari_array_size(ctr->items) / ctr->ncols;

    int retval = 0;

    ari_array_it_t item_it;
    ari_array_it(item_it, ctr->items);
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

            const ari_t *item = ari_array_cref(item_it);

            int ret = ari_text_encode_stream(state, item);
            if (ret)
            {
                retval = 2;
                break;
            }

            ari_array_next(item_it);
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

static int ari_text_encode_execset(ari_text_enc_state_t *state, const ari_execset_t *ctr)
{
    {
        ari_text_enc_opts_t saveopts = *(state->opts);
        state->opts->scheme_prefix   = ARI_TEXT_SCHEME_NONE;

        string_cat_str(state->out, "n=");
        ari_text_encode_stream(state, &(ctr->nonce));
        string_push_back(state->out, ';');

        *(state->opts) = saveopts;
    }

    ++(state->depth);
    string_push_back(state->out, '(');

    int  retval = 0;
    bool sep    = false;

    ari_list_it_t item_it;
    for (ari_list_it(item_it, ctr->targets); !ari_list_end_p(item_it); ari_list_next(item_it))
    {
        if (sep)
        {
            string_push_back(state->out, ',');
        }
        sep = true;

        const ari_t *item = ari_list_cref(item_it);

        int ret = ari_text_encode_stream(state, item);
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

static int ari_text_encode_report(ari_text_enc_state_t *state, const ari_report_t rpt)
{
    string_push_back(state->out, '(');
    {
        ari_text_enc_opts_t saveopts = *(state->opts);
        state->opts->scheme_prefix   = ARI_TEXT_SCHEME_NONE;

        string_cat_str(state->out, "t=");
        ari_text_encode_stream(state, &(rpt->reltime));
        string_push_back(state->out, ';');

        string_cat_str(state->out, "s=");
        ari_text_encode_stream(state, &(rpt->source));
        string_push_back(state->out, ';');

        *(state->opts) = saveopts;
    }

    string_push_back(state->out, '(');

    int  retval = 0;
    bool sep    = false;

    ari_list_it_t item_it;
    for (ari_list_it(item_it, rpt->items); !ari_list_end_p(item_it); ari_list_next(item_it))
    {
        if (sep)
        {
            string_push_back(state->out, ',');
        }
        sep = true;

        const ari_t *item = ari_list_cref(item_it);

        int ret = ari_text_encode_stream(state, item);
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

static int ari_text_encode_rptset(ari_text_enc_state_t *state, const ari_rptset_t *ctr)
{
    ++(state->depth);

    {
        ari_text_enc_opts_t saveopts = *(state->opts);
        state->opts->scheme_prefix   = ARI_TEXT_SCHEME_NONE;

        string_cat_str(state->out, "n=");
        ari_text_encode_stream(state, &(ctr->nonce));
        string_push_back(state->out, ';');

        string_cat_str(state->out, "r=");
        ari_text_encode_stream(state, &(ctr->reftime));
        string_push_back(state->out, ';');

        *(state->opts) = saveopts;
    }

    int retval = 0;

    ari_report_list_it_t rpt_it;
    for (ari_report_list_it(rpt_it, ctr->reports); !ari_report_list_end_p(rpt_it); ari_report_list_next(rpt_it))
    {
        const ari_report_t *rpt = ari_report_list_cref(rpt_it);

        int ret = ari_text_encode_report(state, *rpt);
        if (ret)
        {
            retval = 2;
            break;
        }
    }

    --(state->depth);
    return retval;
}

static void ari_text_encode_prefix(ari_text_enc_state_t *state)
{
    switch (state->opts->scheme_prefix)
    {
        case ARI_TEXT_SCHEME_NONE:
            return;
        case ARI_TEXT_SCHEME_FIRST:
            if (state->depth > 0)
            {
                return;
            }
            break;
        case ARI_TEXT_SCHEME_ALL:
            break;
    }

    string_cat_str(state->out, "ari:");
}

static void ari_text_encode_idseg(string_t text, const ari_idseg_t *obj);

static void ari_text_encode_aritype(string_t text, enum ari_text_aritype_e show, const ari_type_t val,
                                    const ari_idseg_t *idseg)
{
    const char *name;
    switch (show)
    {
        case ARI_TEXT_ARITYPE_TEXT:
            name = ari_type_to_name(val);
            break;
        case ARI_TEXT_ARITYPE_INT:
            name = NULL;
            break;
        default:
            if (idseg)
            {
                ari_text_encode_idseg(text, idseg);
                return;
            }
            else
            {
                name = ari_type_to_name(val);
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

static int ari_text_encode_lit(ari_text_enc_state_t *state, const ari_lit_t *obj)
{
    ari_text_encode_prefix(state);

    if (obj->has_ari_type)
    {
        string_push_back(state->out, '/');
        ari_text_encode_aritype(state->out, state->opts->show_ari_type, obj->ari_type, NULL);
        string_push_back(state->out, '/');

        switch (obj->ari_type)
        {
            case ARI_TYPE_TP:
                if (state->opts->time_text)
                {
                    // never use separators
                    if (utctime_encode(state->out, &(obj->value.as_timespec), false))
                    {
                        return 2;
                    }
                }
                else
                {
                    if (decfrac_encode(state->out, &(obj->value.as_timespec)))
                    {
                        return 2;
                    }
                }
                break;
            case ARI_TYPE_TD:
                if (state->opts->time_text)
                {
                    if (timeperiod_encode(state->out, &(obj->value.as_timespec)))
                    {
                        return 2;
                    }
                }
                else
                {
                    if (decfrac_encode(state->out, &(obj->value.as_timespec)))
                    {
                        return 2;
                    }
                }
                break;
            case ARI_TYPE_AC:
                ari_text_encode_ac(state, obj->value.as_ac);
                break;
            case ARI_TYPE_AM:
                ari_text_encode_am(state, obj->value.as_am);
                break;
            case ARI_TYPE_TBL:
                ari_text_encode_tbl(state, obj->value.as_tbl);
                break;
            case ARI_TYPE_EXECSET:
                ari_text_encode_execset(state, obj->value.as_execset);
                break;
            case ARI_TYPE_RPTSET:
                ari_text_encode_rptset(state, obj->value.as_rptset);
                break;
            default:
                // Fall through to primitives below
                break;
        }
    }

    switch (obj->prim_type)
    {
        case ARI_PRIM_UNDEFINED:
            string_cat_str(state->out, "undefined");
            break;
        case ARI_PRIM_NULL:
            string_cat_str(state->out, "null");
            break;
        case ARI_PRIM_BOOL:
            if (obj->value.as_bool)
            {
                string_cat_str(state->out, "true");
            }
            else
            {
                string_cat_str(state->out, "false");
            }
            break;
        case ARI_PRIM_UINT64:
            if (ari_uint64_encode(state->out, obj->value.as_uint64, (int)(state->opts->int_base)))
            {
                return 2;
            }
            break;
        case ARI_PRIM_INT64:
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
            if (ari_uint64_encode(state->out, absval, (int)(state->opts->int_base)))
            {
                return 2;
            }
            break;
        }
        case ARI_PRIM_FLOAT64:
            ari_float64_encode(state->out, obj->value.as_float64, state->opts->float_form);
            break;
        case ARI_PRIM_TSTR:
        {
            if (state->opts->text_identity && ari_text_is_identity(&(obj->value.as_data)))
            {
                string_cat_str(state->out, (const char *)(obj->value.as_data.ptr));
            }
            else
            {
                string_t buf;
                string_init(buf);
                string_push_back(buf, '"');
                slash_escape(buf, &(obj->value.as_data), '"');
                string_push_back(buf, '"');

                ari_text_percent_helper(state->out, buf);
            }
            break;
        }
        case ARI_PRIM_BSTR:
            switch (state->opts->bstr_form)
            {
                case ARI_TEXT_BSTR_RAW:
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
                        slash_escape(buf, &terminated, '\'');
                        string_push_back(buf, '\'');

                        ari_text_percent_helper(state->out, buf);
                    }
                    else
                    {
                        // this value cannot be represented as text
                        string_cat_str(state->out, "h'");
                        base16_encode(state->out, &(obj->value.as_data), true);
                        string_push_back(state->out, '\'');
                    }

                    cace_data_deinit(&terminated);
                    break;
                }
                case ARI_TEXT_BSTR_BASE16:
                    // no need to percent encode
                    string_cat_str(state->out, "h'");
                    base16_encode(state->out, &(obj->value.as_data), true);
                    string_push_back(state->out, '\'');
                    break;
                case ARI_TEXT_BSTR_BASE64URL:
                    // no need to percent encode
                    string_cat_str(state->out, "b64'");
                    base64_encode(state->out, &(obj->value.as_data), true);
                    string_push_back(state->out, '\'');
                    break;
            }
            break;
        case ARI_PRIM_TIMESPEC:
        case ARI_PRIM_OTHER:
            // already handled above
            break;
    }
    return 0;
}

static void ari_text_encode_idseg(string_t text, const ari_idseg_t *obj)
{
    switch (obj->form)
    {
        case ARI_IDSEG_NULL:
            break;
        case ARI_IDSEG_TEXT:
            string_cat(text, obj->as_text);
            break;
        case ARI_IDSEG_INT:
            string_cat_printf(text, "%" PRId64, obj->as_int);
            break;
    }
}

int ari_text_encode_objpath(string_t text, const ari_objpath_t *path, enum ari_text_aritype_e show)
{
    CHKERR1(text);
    CHKERR1(path);

    if (path->ns_id.form != ARI_IDSEG_NULL)
    {
        string_cat_str(text, "//");
        ari_text_encode_idseg(text, &(path->ns_id));
    }
    else
    {
        string_cat_str(text, ".");
    }

    string_push_back(text, '/');
    if (path->type_id.form == ARI_IDSEG_NULL)
    {
        // case for a namespace reference only
        return 0;
    }

    if (path->has_ari_type)
    {
        ari_text_encode_aritype(text, show, path->ari_type, &(path->type_id));
    }
    else
    {
        ari_text_encode_idseg(text, &(path->type_id));
    }

    string_push_back(text, '/');
    ari_text_encode_idseg(text, &(path->obj_id));

    return 0;
}

static int ari_text_encode_objref(ari_text_enc_state_t *state, const ari_ref_t *obj)
{
    // no scheme for path-only URI Reference form
    if (obj->objpath.ns_id.form != ARI_IDSEG_NULL)
    {
        ari_text_encode_prefix(state);
    }

    if (ari_text_encode_objpath(state->out, &(obj->objpath), state->opts->show_ari_type))
    {
        return 2;
    }

    switch (obj->params.state)
    {
        case ARI_PARAMS_NONE:
            // nothing to do
            break;
        case ARI_PARAMS_AC:
            ari_text_encode_ac(state, obj->params.as_ac);
            break;
        case ARI_PARAMS_AM:
            ari_text_encode_am(state, obj->params.as_am);
            break;
    }
    return 0;
}

static int ari_text_encode_stream(ari_text_enc_state_t *state, const ari_t *ari)
{
    if (ari->is_ref)
    {
        const ari_ref_t *obj = &(ari->as_ref);
        ari_text_encode_objref(state, obj);
    }
    else
    {
        const ari_lit_t *obj = &(ari->as_lit);
        ari_text_encode_lit(state, obj);
    }

    return 0;
}
