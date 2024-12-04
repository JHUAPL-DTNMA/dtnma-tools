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
 * This file contains the definitions, prototypes, constants, and
 * other information necessary for the identification and
 * processing of AMM Resource Identifiers (ARIs). Every object in
 * the AMM can be uniquely identified using an ARI.
 */
#include "cbor.h"
#include "cace/util/defs.h"
#include <qcbor/qcbor_spiffy_decode.h>

int ari_cbor_encode(cace_data_t *buf, const ari_t *ari)
{
    CHKERR1(buf);
    CHKERR1(ari);

    QCBOREncodeContext encoder;
    QCBOREncode_Init(&encoder, SizeCalculateUsefulBuf);
    ari_cbor_encode_stream(&encoder, ari);
    size_t needlen;
    if (QCBOR_SUCCESS != QCBOREncode_FinishGetSize(&encoder, &needlen))
    {
        return 2;
    }

    cace_data_copy_from(buf, needlen, NULL);
    QCBOREncode_Init(&encoder, (UsefulBuf) { buf->ptr, buf->len });
    ari_cbor_encode_stream(&encoder, ari);
    UsefulBufC encdata;
    if (QCBOR_SUCCESS != QCBOREncode_Finish(&encoder, &encdata))
    {
        return 3;
    }
    if (encdata.len != needlen)
    {
        // FIXME warn about this
    }

    return 0;
}

static int ari_cbor_encode_idseg(QCBOREncodeContext *enc, const ari_idseg_t *obj)
{
    switch (obj->form)
    {
        case ARI_IDSEG_NULL:
            QCBOREncode_AddNULL(enc);
            break;
        case ARI_IDSEG_TEXT:
        {
            // trim off trailing null
            const UsefulBufC buf = { .ptr = string_get_cstr(obj->as_text), .len = string_size(obj->as_text) };
            QCBOREncode_AddText(enc, buf);
            break;
        }
        case ARI_IDSEG_INT:
            QCBOREncode_AddInt64(enc, obj->as_int);
            break;
    }
    return 0;
}

static int ari_cbor_decode_idseg(QCBORDecodeContext *dec, ari_idseg_t *obj)
{
    QCBORItem decitem;
    QCBORDecode_VPeekNext(dec, &decitem);
    if (QCBORDecode_GetError(dec))
    {
        return 2;
    }

    switch (decitem.uDataType)
    {
        case QCBOR_TYPE_NULL:
        {
            obj->form = ARI_IDSEG_NULL;
            break;
        }
        case QCBOR_TYPE_TEXT_STRING:
        {
            obj->form = ARI_IDSEG_TEXT;
            UsefulBufC buf;
            QCBORDecode_GetTextString(dec, &buf);
            // add trailing null
            string_init_printf(obj->as_text, "%.*s", buf.len, buf.ptr);
            break;
        }
        case QCBOR_TYPE_INT64:
            obj->form = ARI_IDSEG_INT;
            QCBORDecode_GetInt64(dec, &(obj->as_int));
            break;

        default:
            return 3;
    }
    return 0;
}

static const int64_t nsec_scale = (int64_t)1e9;

static int ari_cbor_encode_timespec(QCBOREncodeContext *enc, const struct timespec *ts)
{
    if (ts->tv_nsec)
    {
        // naive encoding without check for truncation
        int64_t exp = -9;
        int64_t val = ts->tv_sec * nsec_scale + ts->tv_nsec;
        while (val && (val % 10 == 0))
        {
            ++exp;
            val /= 10;
        }

        QCBOREncode_OpenArray(enc);
        QCBOREncode_AddInt64(enc, exp);
        QCBOREncode_AddInt64(enc, val);
        QCBOREncode_CloseArray(enc);
    }
    else
    {
        // pure integer seconds
        QCBOREncode_AddInt64(enc, ts->tv_sec);
    }
    return 0;
}

static int ari_cbor_decode_timespec(QCBORDecodeContext *dec, struct timespec *ts)
{
    QCBORItem decitem;
    QCBORDecode_VPeekNext(dec, &decitem);
    if (QCBORDecode_GetError(dec))
    {
        return 2;
    }

    if (decitem.uDataType == QCBOR_TYPE_ARRAY)
    {
        // a decimal fraction
        int64_t exp;
        int64_t val;

        QCBORDecode_EnterArray(dec, NULL);
        QCBORDecode_GetInt64(dec, &exp);
        QCBORDecode_GetInt64(dec, &val);
        QCBORDecode_ExitArray(dec);
        if (QCBORDecode_GetError(dec))
        {
            return 2;
        }

        // Restrict exponent per ARI spec
        if (exp > 9 || exp < -9)
        {
            return 3;
        }

        // normalize to 1e-9 seconds
        while (exp > -9)
        {
            --exp;
            val *= 10;
        }
        *ts = (struct timespec) {
            .tv_sec  = (val / nsec_scale),
            .tv_nsec = (val % nsec_scale),
        };
    }
    else
    {
        // pure integer seconds
        int64_t secs;
        QCBORDecode_GetInt64(dec, &secs);
        if (QCBORDecode_GetError(dec))
        {
            return 2;
        }

        *ts = (struct timespec) {
            .tv_sec = secs,
        };
    }
    return 0;
}

static int ari_cbor_encode_ac(QCBOREncodeContext *enc, const ari_ac_t *obj)
{
    int retval = 0;
    QCBOREncode_OpenArray(enc);

    ari_list_it_t it;
    for (ari_list_it(it, obj->items); !ari_list_end_p(it); ari_list_next(it))
    {
        const ari_t *item = ari_list_cref(it);
        if (ari_cbor_encode_stream(enc, item))
        {
            retval = 2;
            break;
        }
    }

    QCBOREncode_CloseArray(enc);
    return retval;
}

static int ari_cbor_decode_ac(QCBORDecodeContext *dec, ari_ac_t *obj)
{
    QCBORDecode_EnterArray(dec, NULL);
    if (QCBORDecode_GetError(dec))
    {
        return 2;
    }

    int retval = 0;
    while (true)
    {
        ari_t ari       = ARI_INIT_UNDEFINED;
        int   parse_res = ari_cbor_decode_stream(dec, &ari);

        int  dec_res = QCBORDecode_GetAndResetError(dec);
        bool atend   = dec_res == QCBOR_ERR_NO_MORE_ITEMS;
        if (!atend)
        {
            if (dec_res)
            {
                // not well-formed CBOR
                retval = 2;
            }
            else if (parse_res)
            {
                // well-formed CBOR but not valid ARI
                retval = 3;
            }
        }

        if (retval)
        {
            ari_deinit(&ari);
            break;
        }
        if (atend)
        {
            break;
        }

        // push only after fully reading
        ari_t *item = ari_list_push_back_new(obj->items);
        ari_set_move(item, &ari);
    }
    QCBORDecode_ExitArray(dec);
    return retval;
}

static int ari_cbor_encode_am(QCBOREncodeContext *enc, const ari_am_t *obj)
{
    int retval = 0;
    QCBOREncode_OpenMap(enc);

    ari_tree_it_t it;
    for (ari_tree_it(it, obj->items); !ari_tree_end_p(it); ari_tree_next(it))
    {
        const ari_tree_subtype_ct *pair = ari_tree_cref(it);
        if (ari_cbor_encode_stream(enc, pair->key_ptr))
        {
            retval = 2;
            break;
        }
        if (ari_cbor_encode_stream(enc, pair->value_ptr))
        {
            retval = 2;
            break;
        }
    }

    QCBOREncode_CloseMap(enc);
    return retval;
}

static int ari_cbor_decode_am(QCBORDecodeContext *dec, ari_am_t *obj)
{
    int retval = 0;

    // map as array of key+value
    QCBORDecode_EnterArray(dec, NULL);
    if (QCBORDecode_GetError(dec))
    {
        return 2;
    }

    while (true)
    {
        ari_t key     = ARI_INIT_UNDEFINED;
        int   key_res = ari_cbor_decode_stream(dec, &key);

        ari_t value     = ARI_INIT_UNDEFINED;
        int   value_res = ari_cbor_decode_stream(dec, &value);

        int  dec_res = QCBORDecode_GetAndResetError(dec);
        bool atend   = dec_res == QCBOR_ERR_NO_MORE_ITEMS;
        if (!atend)
        {
            if (dec_res)
            {
                // not well-formed CBOR
                retval = 2;
            }
            else if (key_res || value_res)
            {
                // well-formed CBOR but not valid ARI
                retval = 3;
            }
        }

        if (retval)
        {
            ari_deinit(&key);
            ari_deinit(&value);
            break;
        }
        if (atend)
        {
            break;
        }

        // push only after fully reading
        ari_t *val = ari_tree_safe_get(obj->items, key);
        ari_set_move(val, &value);
        ari_deinit(&key);
    }

    QCBORDecode_ExitArray(dec);
    return retval;
}

static int ari_cbor_encode_tbl(QCBOREncodeContext *enc, const ari_tbl_t *obj)
{
    int retval = 0;
    QCBOREncode_OpenArray(enc);

    QCBOREncode_AddUInt64(enc, obj->ncols);

    ari_array_it_t it;
    for (ari_array_it(it, obj->items); !ari_array_end_p(it); ari_array_next(it))
    {
        const ari_t *item = ari_array_cref(it);
        if (ari_cbor_encode_stream(enc, item))
        {
            retval = 2;
            break;
        }
    }

    QCBOREncode_CloseArray(enc);
    return retval;
}

static int ari_cbor_decode_tbl(QCBORDecodeContext *dec, ari_tbl_t *obj)
{
    int retval = 0;

    QCBORDecode_EnterArray(dec, NULL);
    if (QCBORDecode_GetError(dec))
    {
        return 2;
    }

    QCBORDecode_GetUInt64(dec, &(obj->ncols));
    if (QCBORDecode_GetError(dec))
    {
        return 2;
    }

    while (true)
    {
        ari_t ari       = ARI_INIT_UNDEFINED;
        int   parse_res = ari_cbor_decode_stream(dec, &ari);

        int  dec_res = QCBORDecode_GetAndResetError(dec);
        bool atend   = dec_res == QCBOR_ERR_NO_MORE_ITEMS;
        if (!atend)
        {
            if (dec_res)
            {
                // not well-formed CBOR
                retval = 2;
            }
            else if (parse_res)
            {
                // well-formed CBOR but not valid ARI
                retval = 3;
            }
        }

        if (retval)
        {
            ari_deinit(&ari);
            break;
        }
        if (atend)
        {
            break;
        }

        // push only after fully reading
        ari_array_push_move(obj->items, &ari);
    }

    if (obj->ncols > 0 && ari_array_size(obj->items) % obj->ncols)
    {
        retval = 3; // Invalid ARI due to incomplete row data
    }

    QCBORDecode_ExitArray(dec);
    return retval;
}

static int ari_cbor_encode_execset(QCBOREncodeContext *enc, const ari_execset_t *obj)
{
    int retval = 0;
    QCBOREncode_OpenArray(enc);

    if (ari_cbor_encode_stream(enc, &(obj->nonce)))
    {
        return 2;
    }

    ari_list_it_t it;
    for (ari_list_it(it, obj->targets); !ari_list_end_p(it); ari_list_next(it))
    {
        const ari_t *item = ari_list_cref(it);
        if (ari_cbor_encode_stream(enc, item))
        {
            retval = 2;
            break;
        }
    }

    QCBOREncode_CloseArray(enc);
    return retval;
}

static int ari_cbor_decode_execset(QCBORDecodeContext *dec, ari_execset_t *obj)
{
    int retval = 0;

    QCBORDecode_EnterArray(dec, NULL);
    if (QCBORDecode_GetError(dec))
    {
        return 2;
    }

    if (ari_cbor_decode_stream(dec, &(obj->nonce)))
    {
        return 2;
    }

    if (obj->nonce.is_ref)
    {
        return 3;
    }
    else
    {
        switch (obj->nonce.as_lit.prim_type)
        {
            case ARI_PRIM_NULL:
            case ARI_PRIM_UINT64:
            case ARI_PRIM_BSTR:
                break;
            case ARI_PRIM_INT64:
                if (obj->nonce.as_lit.value.as_int64 < 0)
                {
                    return 3;
                }
                break;
            default:
                return 3;
        }
    }

    while (true)
    {
        ari_t ari       = ARI_INIT_UNDEFINED;
        int   parse_res = ari_cbor_decode_stream(dec, &ari);

        int  dec_res = QCBORDecode_GetAndResetError(dec);
        bool atend   = dec_res == QCBOR_ERR_NO_MORE_ITEMS;
        if (!atend)
        {
            if (dec_res)
            {
                // not well-formed CBOR
                retval = 2;
            }
            else if (parse_res)
            {
                // well-formed CBOR but not valid ARI
                retval = 3;
            }
        }

        if (retval)
        {
            ari_deinit(&ari);
            break;
        }
        if (atend)
        {
            break;
        }

        // push only after fully reading
        ari_t *item = ari_list_push_back_new(obj->targets);
        ari_set_move(item, &ari);
    }

    QCBORDecode_ExitArray(dec);
    return retval;
}

static int ari_cbor_encode_report(QCBOREncodeContext *enc, const ari_report_t *obj)
{
    int retval = 0;
    QCBOREncode_OpenArray(enc);

    // FIXME check type
    if (ari_cbor_encode_timespec(enc, &(obj->reltime.as_lit.value.as_timespec)))
    {
        return 2;
    }

    if (ari_cbor_encode_stream(enc, &(obj->source)))
    {
        return 2;
    }

    ari_list_it_t it;
    for (ari_list_it(it, obj->items); !ari_list_end_p(it); ari_list_next(it))
    {
        const ari_t *item = ari_list_cref(it);
        if (ari_cbor_encode_stream(enc, item))
        {
            retval = 2;
            break;
        }
    }

    QCBOREncode_CloseArray(enc);
    return retval;
}

static int ari_cbor_decode_report(QCBORDecodeContext *dec, ari_report_t *obj)
{
    int retval = 0;

    QCBORDecode_EnterArray(dec, NULL);
    if (QCBORDecode_GetError(dec))
    {
        return 2;
    }

    {
        struct timespec ts;
        if (ari_cbor_decode_timespec(dec, &ts))
        {
            return 2;
        }
        ari_set_td(&(obj->reltime), ts);
    }

    if (ari_cbor_decode_stream(dec, &(obj->source)))
    {
        return 2;
    }

    while (true)
    {
        ari_t ari       = ARI_INIT_UNDEFINED;
        int   parse_res = ari_cbor_decode_stream(dec, &ari);

        int  dec_res = QCBORDecode_GetAndResetError(dec);
        bool atend   = dec_res == QCBOR_ERR_NO_MORE_ITEMS;
        if (!atend)
        {
            if (dec_res)
            {
                // not well-formed CBOR
                retval = 2;
            }
            else if (parse_res)
            {
                // well-formed CBOR but not valid ARI
                retval = 3;
            }
        }

        if (retval)
        {
            ari_deinit(&ari);
            break;
        }
        if (atend)
        {
            break;
        }

        // push only after fully reading
        ari_t *item = ari_list_push_back_new(obj->items);
        ari_set_move(item, &ari);
    }

    QCBORDecode_ExitArray(dec);
    return retval;
}

static int ari_cbor_encode_rptset(QCBOREncodeContext *enc, const ari_rptset_t *obj)
{
    int retval = 0;
    QCBOREncode_OpenArray(enc);

    if (ari_cbor_encode_stream(enc, &(obj->nonce)))
    {
        return 2;
    }

    // FIXME check type
    if (ari_cbor_encode_timespec(enc, &(obj->reftime.as_lit.value.as_timespec)))
    {
        return 2;
    }

    ari_report_list_it_t it;
    for (ari_report_list_it(it, obj->reports); !ari_report_list_end_p(it); ari_report_list_next(it))
    {
        const ari_report_t *rpt = ari_report_list_cref(it);
        if (ari_cbor_encode_report(enc, rpt))
        {
            retval = 2;
            break;
        }
    }

    QCBOREncode_CloseArray(enc);
    return retval;
}

static int ari_cbor_decode_rptset(QCBORDecodeContext *dec, ari_rptset_t *obj)
{
    int retval = 0;

    QCBORDecode_EnterArray(dec, NULL);
    if (QCBORDecode_GetError(dec))
    {
        return 2;
    }

    if (ari_cbor_decode_stream(dec, &(obj->nonce)))
    {
        return 2;
    }

    if (obj->nonce.is_ref)
    {
        return 3;
    }
    else
    {
        switch (obj->nonce.as_lit.prim_type)
        {
            case ARI_PRIM_NULL:
            case ARI_PRIM_UINT64:
            case ARI_PRIM_BSTR:
                break;
            case ARI_PRIM_INT64:
                if (obj->nonce.as_lit.value.as_int64 < 0)
                {
                    return 3;
                }
                break;
            default:
                return 3;
        }
    }

    {
        struct timespec ts;
        if (ari_cbor_decode_timespec(dec, &ts))
        {
            return 2;
        }
        ari_set_tp(&(obj->reftime), ts);
    }

    while (true)
    {
        ari_report_t rpt;
        ari_report_init(&rpt);
        int parse_res = ari_cbor_decode_report(dec, &rpt);

        int  dec_res = QCBORDecode_GetAndResetError(dec);
        bool atend   = dec_res == QCBOR_ERR_NO_MORE_ITEMS;
        if (!atend)
        {
            if (dec_res)
            {
                // not well-formed CBOR
                retval = 2;
            }
            else if (parse_res)
            {
                // well-formed CBOR but not valid ARI
                retval = 3;
            }
        }

        if (retval || atend)
        {
            ari_report_deinit(&rpt);
            break;
        }

        // push only after fully reading
        ari_report_list_push_back_move(obj->reports, &rpt);
    }

    QCBORDecode_ExitArray(dec);
    return retval;
}

static int ari_cbor_encode_primval(QCBOREncodeContext *enc, const ari_lit_t *obj)
{
    switch (obj->prim_type)
    {
        case ARI_PRIM_UNDEFINED:
            QCBOREncode_AddUndef(enc);
            break;
        case ARI_PRIM_NULL:
            QCBOREncode_AddNULL(enc);
            break;
        case ARI_PRIM_BOOL:
            QCBOREncode_AddBool(enc, obj->value.as_bool);
            break;
        case ARI_PRIM_UINT64:
            QCBOREncode_AddUInt64(enc, obj->value.as_uint64);
            break;
        case ARI_PRIM_INT64:
            QCBOREncode_AddInt64(enc, obj->value.as_int64);
            break;
        case ARI_PRIM_FLOAT64:
            QCBOREncode_AddDouble(enc, obj->value.as_float64);
            break;
        case ARI_PRIM_TSTR:
        {
            // trim off trailing null
            const UsefulBufC buf = { .ptr = obj->value.as_data.ptr, .len = obj->value.as_data.len - 1 };
            QCBOREncode_AddText(enc, buf);
            break;
        }
        case ARI_PRIM_BSTR:
        {
            const UsefulBufC buf = { .ptr = obj->value.as_data.ptr, .len = obj->value.as_data.len };
            QCBOREncode_AddBytes(enc, buf);
            break;
        }
        case ARI_PRIM_TIMESPEC:
            if (ari_cbor_encode_timespec(enc, &(obj->value.as_timespec)))
            {
                return 2;
            }
            break;
        case ARI_PRIM_OTHER:
            // This must have an ari_lit_t::ari_type
            return 1;
    }
    return 0;
}

int ari_cbor_encode_stream(QCBOREncodeContext *enc, const ari_t *ari)
{
    CHKERR1(enc);
    CHKERR1(ari);
    if (ari->is_ref)
    {
        const ari_ref_t *obj = &(ari->as_ref);

        QCBOREncode_OpenArray(enc);
        ari_cbor_encode_idseg(enc, &(obj->objpath.ns_id));
        if (obj->objpath.has_ari_type)
        {
            QCBOREncode_AddInt64(enc, obj->objpath.ari_type);
        }
        else
        {
            ari_cbor_encode_idseg(enc, &(obj->objpath.type_id));
        }
        ari_cbor_encode_idseg(enc, &(obj->objpath.obj_id));
        switch (obj->params.state)
        {
            case ARI_PARAMS_NONE:
                // absense of a value, not a null/undefined
                break;
            case ARI_PARAMS_AC:
                ari_cbor_encode_ac(enc, obj->params.as_ac);
                break;
            case ARI_PARAMS_AM:
                ari_cbor_encode_am(enc, obj->params.as_am);
                break;
        }
        QCBOREncode_CloseArray(enc);
    }
    else
    {
        const ari_lit_t *obj = &(ari->as_lit);

        if (!(obj->has_ari_type))
        {
            // no enclosing array, and only a subset of types allowed
            // per `lit-notype` rule in Section 5.2 of @ref ietf-dtn-ari-00
            if (ari_cbor_encode_primval(enc, obj))
            {
                return 2;
            }
        }
        else
        {
            QCBOREncode_OpenArray(enc);
            QCBOREncode_AddInt64(enc, obj->ari_type);

            switch (obj->ari_type)
            {
                case ARI_TYPE_AC:
                    if (ari_cbor_encode_ac(enc, obj->value.as_ac))
                    {
                        return 2;
                    }
                    break;
                case ARI_TYPE_AM:
                    if (ari_cbor_encode_am(enc, obj->value.as_am))
                    {
                        return 2;
                    }
                    break;
                case ARI_TYPE_TBL:
                    if (ari_cbor_encode_tbl(enc, obj->value.as_tbl))
                    {
                        return 2;
                    }
                    break;
                case ARI_TYPE_EXECSET:
                    if (ari_cbor_encode_execset(enc, obj->value.as_execset))
                    {
                        return 2;
                    }
                    break;
                case ARI_TYPE_RPTSET:
                    if (ari_cbor_encode_rptset(enc, obj->value.as_rptset))
                    {
                        return 2;
                    }
                    break;
                default:
                    // simple typed literals
                    if (ari_cbor_encode_primval(enc, obj))
                    {
                        return 2;
                    }
                    break;
            }

            QCBOREncode_CloseArray(enc);
        }
    }
    return 0;
}

static int ari_cbor_decode_primval(QCBORDecodeContext *dec, ari_lit_t *lit)
{
    QCBORItem decitem;
    QCBORDecode_VGetNext(dec, &decitem);

    switch (decitem.uDataType)
    {
        case QCBOR_TYPE_UNDEF:
            lit->prim_type = ARI_PRIM_UNDEFINED;
            break;
        case QCBOR_TYPE_NULL:
            lit->prim_type = ARI_PRIM_NULL;
            break;
        case QCBOR_TYPE_TRUE:
            lit->prim_type     = ARI_PRIM_BOOL;
            lit->value.as_bool = true;
            break;
        case QCBOR_TYPE_FALSE:
            lit->prim_type     = ARI_PRIM_BOOL;
            lit->value.as_bool = false;
            break;
        case QCBOR_TYPE_INT64:
            lit->prim_type      = ARI_PRIM_INT64;
            lit->value.as_int64 = decitem.val.int64;
            break;
        case QCBOR_TYPE_UINT64:
            lit->prim_type       = ARI_PRIM_UINT64;
            lit->value.as_uint64 = decitem.val.uint64;
            break;
        case QCBOR_TYPE_FLOAT:
            lit->prim_type        = ARI_PRIM_FLOAT64;
            lit->value.as_float64 = decitem.val.fnum;
            break;
        case QCBOR_TYPE_DOUBLE:
            lit->prim_type        = ARI_PRIM_FLOAT64;
            lit->value.as_float64 = decitem.val.dfnum;
            break;
        case QCBOR_TYPE_TEXT_STRING:
            lit->prim_type = ARI_PRIM_TSTR;
            cace_data_init(&(lit->value.as_data));
            cace_data_copy_from(&(lit->value.as_data), decitem.val.string.len, (cace_data_ptr_t)decitem.val.string.ptr);
            cace_data_append_byte(&(lit->value.as_data), 0);
            break;
        case QCBOR_TYPE_BYTE_STRING:
            lit->prim_type = ARI_PRIM_BSTR;
            cace_data_init(&(lit->value.as_data));
            cace_data_copy_from(&(lit->value.as_data), decitem.val.string.len, (cace_data_ptr_t)decitem.val.string.ptr);
            break;
    }
    return 0;
}

static int ari_cbor_decode_label(QCBORDecodeContext *dec, ari_lit_t *lit)
{
    QCBORItem decitem;
    QCBORDecode_VGetNext(dec, &decitem);

    switch (decitem.uDataType)
    {
        case QCBOR_TYPE_INT64:
            lit->prim_type      = ARI_PRIM_INT64;
            lit->value.as_int64 = decitem.val.int64;
            break;
        case QCBOR_TYPE_UINT64:
            lit->prim_type       = ARI_PRIM_UINT64;
            lit->value.as_uint64 = decitem.val.uint64;
            break;
        case QCBOR_TYPE_TEXT_STRING:
            lit->prim_type = ARI_PRIM_TSTR;
            cace_data_init(&(lit->value.as_data));
            cace_data_copy_from(&(lit->value.as_data), decitem.val.string.len, (cace_data_ptr_t)decitem.val.string.ptr);
            cace_data_append_byte(&(lit->value.as_data), 0);
            break;
        default:
            return 3;
    }
    return 0;
}

int ari_cbor_decode(ari_t *ari, const cace_data_t *buf, size_t *used, const char **errm)
{
    CHKERR1(ari);
    CHKERR1(buf);

    if (!buf->ptr || !buf->len)
    {
        if (used)
        {
            *used = 0;
        }
        return 2;
    }

    QCBORDecodeContext dec;
    UsefulBufC         indata = { .ptr = buf->ptr, .len = buf->len };
    QCBORDecode_Init(&dec, indata, QCBOR_DECODE_MODE_MAP_AS_ARRAY);

    ari_deinit(ari);
    int parse_res = ari_cbor_decode_stream(&dec, ari);
    if (used)
    {
        uint32_t pos = QCBORDecode_Tell(&dec);
        // work around a weird behavior of QCBORDecode_Tell()
        if (pos == UINT32_MAX)
        {
            pos = (uint32_t)indata.len;
        }
        *used = pos;
    }
    int dec_res = QCBORDecode_Finish(&dec);

    if (errm)
    {
        if (parse_res)
        {
            string_t err;
            string_init_printf(err, "parser error %d", parse_res);
            *errm = string_clear_get_str(err);
        }
        else if (dec_res)
        {
            string_t err;
            string_init_printf(err, "decoder error %d: %s", dec_res, qcbor_err_to_str(dec_res));
            *errm = string_clear_get_str(err);
        }
        else
        {
            *errm = NULL;
        }
    }

    if (parse_res)
    {
        return 3;
    }
    switch (dec_res)
    {
        case QCBOR_SUCCESS:
            break;
        case QCBOR_ERR_EXTRA_BYTES:
            if (!used)
            {
                return 4;
            }
            break;
        default:
            return 3;
    }
    return 0;
}

int ari_cbor_decode_stream(QCBORDecodeContext *dec, ari_t *ari)
{
    QCBORItem decitem;
    QCBORDecode_VPeekNext(dec, &decitem);
    if (QCBORDecode_GetError(dec))
    {
        return 3;
    }

    int retval = 0;
    if (decitem.uDataType == QCBOR_TYPE_ARRAY)
    {
        // either a typed literal or an object reference
        QCBORDecode_EnterArray(dec, &decitem);
        if (QCBORDecode_GetError(dec))
        {
            return 3;
        }
        if (decitem.val.uCount == 2)
        {
            ari_lit_t *obj = ari_init_lit(ari);
            // literal value state
            *obj = (ari_lit_t) {
                .has_ari_type = true,
            };

            int64_t type_id;
            QCBORDecode_GetInt64(dec, &type_id);
            if ((type_id < INT32_MIN) || (type_id > INT32_MAX))
            {
                return 3;
            }
            obj->ari_type = (int32_t)type_id;

            switch (obj->ari_type)
            {
                case ARI_TYPE_TP:
                case ARI_TYPE_TD:
                    obj->prim_type = ARI_PRIM_TIMESPEC;
                    if (ari_cbor_decode_timespec(dec, &(obj->value.as_timespec)))
                    {
                        retval = 3;
                    }
                    break;
                case ARI_TYPE_LABEL:
                    if (ari_cbor_decode_label(dec, obj))
                    {
                        retval = 3;
                    }
                    break;

                case ARI_TYPE_AC:
                    ari_lit_init_container(obj, ARI_TYPE_AC);
                    if (ari_cbor_decode_ac(dec, obj->value.as_ac))
                    {
                        retval = 3;
                    }
                    break;
                case ARI_TYPE_AM:
                    ari_lit_init_container(obj, ARI_TYPE_AM);
                    if (ari_cbor_decode_am(dec, obj->value.as_am))
                    {
                        retval = 3;
                    }
                    break;
                case ARI_TYPE_TBL:
                    ari_lit_init_container(obj, ARI_TYPE_TBL);
                    if (ari_cbor_decode_tbl(dec, obj->value.as_tbl))
                    {
                        retval = 3;
                    }
                    break;
                case ARI_TYPE_EXECSET:
                    ari_lit_init_container(obj, ARI_TYPE_EXECSET);
                    if (ari_cbor_decode_execset(dec, obj->value.as_execset))
                    {
                        retval = 3;
                    }
                    break;
                case ARI_TYPE_RPTSET:
                    ari_lit_init_container(obj, ARI_TYPE_RPTSET);
                    if (ari_cbor_decode_rptset(dec, obj->value.as_rptset))
                    {
                        retval = 3;
                    }
                    break;
                default:
                    // simple typed literals
                    if (ari_cbor_decode_primval(dec, obj))
                    {
                        retval = 3;
                    }
                    break;
            }
        }
        else if (decitem.val.uCount > 2)
        {
            ari_ref_t *obj = ari_init_objref(ari);

            ari_cbor_decode_idseg(dec, &(obj->objpath.ns_id));
            ari_cbor_decode_idseg(dec, &(obj->objpath.type_id));
            ari_cbor_decode_idseg(dec, &(obj->objpath.obj_id));
            int err = ari_objpath_derive_type(&(obj->objpath));

            // Validate AMM object type
            if (err)
            {
                return 3;
            }

            obj->params.state = ARI_PARAMS_NONE;
            if (decitem.val.uCount > 3)
            {
                // some parameters present
                QCBORDecode_VPeekNext(dec, &decitem);
                if (QCBORDecode_GetError(dec))
                {
                    return 3;
                }

                // temporary holding structure
                ari_lit_t params;
                switch (decitem.uDataType)
                {
                    case QCBOR_TYPE_ARRAY:
                        ari_lit_init_container(&params, ARI_TYPE_AC);
                        if (ari_cbor_decode_ac(dec, params.value.as_ac))
                        {
                            retval = 3;
                        }
                        obj->params.state = ARI_PARAMS_AC;
                        obj->params.as_ac = params.value.as_ac;
                        break;
                    case QCBOR_TYPE_MAP_AS_ARRAY:
                        ari_lit_init_container(&params, ARI_TYPE_AM);
                        if (ari_cbor_decode_am(dec, params.value.as_am))
                        {
                            retval = 3;
                        }
                        obj->params.state = ARI_PARAMS_AM;
                        obj->params.as_am = params.value.as_am;
                        break;

                    default:
                        retval = 4;
                        break;
                }
            }
        }

        QCBORDecode_ExitArray(dec);
    }
    else if (decitem.uDataType == QCBOR_TYPE_MAP_AS_ARRAY)
    {
        // skip the value anyway
        QCBORDecode_VGetNextConsume(dec, &decitem);
        retval = 4;
    }
    else
    {
        // otherwise this is a single primitive value
        ari_lit_t *obj = ari_init_lit(ari);
        *obj           = (ari_lit_t) {
                      .has_ari_type = false,
        };

        if (ari_cbor_decode_primval(dec, obj))
        {
            retval = 3;
        }
    }

    if (retval)
    {
        ari_deinit(ari);
    }
    return retval;
}
