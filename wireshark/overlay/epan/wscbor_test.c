/* wscbor_test.c
 * Wireshark CBOR API tests
 * Copyright 2021, Brian Sipos <brian.sipos@gmail.com>
 *
 * Wireshark - Network traffic analyzer
 * By Gerald Combs <gerald@wireshark.org>
 * Copyright 1998 Gerald Combs
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */
/* Parts of this work was performed for the Jet Propulsion
 * Laboratory, California Institute of Technology, sponsored by
 * the United States Government under the prime contract
 * 80NM0018D0004 between the Caltech and NASA under
 * subcontract 1658085.
 */

#include "config.h"
#undef G_DISABLE_ASSERT

#include <stdio.h>
#include <string.h>
#include <glib.h>
#include <math.h>

#include "wscbor.h"
#include <epan/wmem_scopes.h>
#include <epan/exceptions.h>
#include <wsutil/wmem/wmem_list.h>
#include <wsutil/wmem/wmem_map.h>

#include <ws_diag_control.h>

static wmem_allocator_t *test_scope;

typedef struct
{
    // Raw bytes
    gint enc_len;
    const guint8 *enc;
    // Members of cbor_chunk_t
    gint head_length;
    gint data_length;
    guint8 type_major;
    guint64 head_value;
    // Explicit floating point decoding
    double float_value;
} example_s;

DIAG_OFF_PEDANTIC
example_s ex_uint0 = {1, (const guint8 *)"\x01", 1, 1, CBOR_TYPE_UINT, 1, NAN};
example_s ex_uint8 = {2, (const guint8 *)"\x18\x64", 2, 2, CBOR_TYPE_UINT, 100, NAN};
example_s ex_uint16 = {3, (const guint8 *)"\x19\x27\x10", 3, 3, CBOR_TYPE_UINT, 10000, NAN};
example_s ex_uint32 = {5, (const guint8 *)"\x1A\x00\x0F\x42\x40", 5, 5, CBOR_TYPE_UINT, 1000000, NAN};
example_s ex_uint64 = {9, (const guint8 *)"\x1B\x00\x00\x00\xE8\xD4\xA5\x10\x00", 9, 9, CBOR_TYPE_UINT, 1000000000000, NAN};
example_s ex_nint0 = {1, (const guint8 *)"\x21", 1, 1, CBOR_TYPE_NEGINT, 1, NAN};
example_s ex_nint8 = {2, (const guint8 *)"\x38\x64", 2, 2, CBOR_TYPE_NEGINT, 100, NAN};
example_s ex_nint16 = {3, (const guint8 *)"\x39\x27\x10", 3, 3, CBOR_TYPE_NEGINT, 10000, NAN};
example_s ex_nint32 = {5, (const guint8 *)"\x3A\x00\x0F\x42\x40", 5, 5, CBOR_TYPE_NEGINT, 1000000, NAN};
example_s ex_nint64 = {9, (const guint8 *)"\x3B\x00\x00\x00\xE8\xD4\xA5\x10\x00", 9, 9, CBOR_TYPE_NEGINT, 1000000000000, NAN};
example_s ex_bstr = {3, (const guint8 *)"\x42\x68\x69", 1, 3, CBOR_TYPE_BYTESTRING, 2, NAN};
example_s ex_bstr_indef = {6, (const guint8 *)"\x5F\x41\x68\x41\x69\xFF", 1, 6, CBOR_TYPE_BYTESTRING, 0, NAN};
example_s ex_tstr = {3, (const guint8 *)"\x62\x68\x69", 1, 3, CBOR_TYPE_STRING, 2, NAN};
example_s ex_tstr_indef = {6, (const guint8 *)"\x7F\x61\x68\x61\x69\xFF", 1, 6, CBOR_TYPE_STRING, 0, NAN};
example_s ex_float16 = {3, (const guint8 *)"\xF9\x3C\x00", 3, 3, CBOR_TYPE_FLOAT_CTRL, 0x3C00, 1.0};
example_s ex_float32 = {5, (const guint8 *)"\xFA\x47\xC3\x50\x00", 5, 5, CBOR_TYPE_FLOAT_CTRL, 0x47C35000, 100000.0};
example_s ex_float64 = {9, (const guint8 *)"\xFB\x3F\xF1\x99\x99\x99\x99\x99\x9A", 9, 9, CBOR_TYPE_FLOAT_CTRL, 0x3FF199999999999A, 1.1};
example_s ex_false = {1, (const guint8 *)"\xF4", 1, 1, CBOR_TYPE_FLOAT_CTRL, CBOR_CTRL_FALSE, NAN};
example_s ex_true = {1, (const guint8 *)"\xF5", 1, 1, CBOR_TYPE_FLOAT_CTRL, CBOR_CTRL_TRUE, NAN};
example_s ex_null = {1, (const guint8 *)"\xF6", 1, 1, CBOR_TYPE_FLOAT_CTRL, CBOR_CTRL_NULL, NAN};
example_s ex_undef = {1, (const guint8 *)"\xF7", 1, 1, CBOR_TYPE_FLOAT_CTRL, CBOR_CTRL_UNDEF, NAN};
example_s ex_break = {1, (const guint8 *)"\xFF", 1, 1, CBOR_TYPE_FLOAT_CTRL, 0, NAN};

example_s ex_uint_overflow = {9, (const guint8 *)"\x1B\x80\x00\x00\x00\x00\x00\x00\x00", 1, 9, CBOR_TYPE_UINT, 0x8000000000000000, NAN};
example_s ex_nint_overflow = {9, (const guint8 *)"\x3B\x80\x00\x00\x00\x00\x00\x00\x00", 1, 9, CBOR_TYPE_NEGINT, 0x8000000000000000, NAN};
example_s ex_bstr_overflow = {11, (const guint8 *)"\x5B\x80\x00\x00\x00\x00\x00\x00\x00\x00\x00", 1, 9, CBOR_TYPE_BYTESTRING, 0x8000000000000000, NAN};
example_s ex_bstr_short = {2, (const guint8 *)"\x42\x68", 1, 3, CBOR_TYPE_BYTESTRING, 2, NAN};
example_s ex_tstr_short = {2, (const guint8 *)"\x62\x68", 1, 3, CBOR_TYPE_STRING, 2, NAN};
DIAG_ON_PEDANTIC

static const example_s * all_examples[] = {
    &ex_uint0, &ex_uint8, &ex_uint16, &ex_uint32, &ex_uint64,
    &ex_nint0, &ex_nint8, &ex_nint16, &ex_nint32, &ex_nint64,
    &ex_bstr, &ex_bstr_indef,
    &ex_tstr, &ex_tstr_indef,
    &ex_float16, &ex_float32, &ex_float64,
    &ex_false, &ex_true, &ex_null, &ex_undef, &ex_break
};

/*
 * These test are organized in order of the appearance, in wscbor.h, of
 * the basic functions that they test.  This makes it easier to
 * get a quick understanding of both the testing and the organization
 * of the header.
 */

/* WSCBOR TESTING FUNCTIONS (/wscbor/) */

static void
wscbor_test_chunk_read_simple(void)
{
    for (size_t ex_ix = 0; ex_ix < (sizeof(all_examples) / sizeof(example_s*)); ++ex_ix) {
        const example_s *ex = all_examples[ex_ix];
        printf("simple #%zu\n", ex_ix);

        tvbuff_t *tvb = tvb_new_real_data(ex->enc, ex->enc_len, ex->enc_len);
        gint offset = 0;

        wscbor_chunk_t *chunk = wscbor_chunk_read(test_scope, tvb, &offset);
        g_assert_nonnull(chunk);
        g_assert_cmpuint(wscbor_has_errors(chunk), ==, 0);

        g_assert_cmpuint(chunk->head_length, ==, ex->head_length);
        g_assert_cmpuint(chunk->data_length, ==, ex->data_length);
        g_assert_cmpuint(wmem_list_count(chunk->tags), ==, 0);
        g_assert_cmpuint(chunk->type_major, ==, ex->type_major);
        g_assert_cmpuint(chunk->head_value, ==, ex->head_value);

        wscbor_chunk_free(chunk);
        tvb_free(tvb);
    }
}

static void
wscbor_test_chunk_read_simple_tags(void)
{
    const guint8 *const tags = (const guint8 *)"\xC1\xD8\xC8";
    tvbuff_t *tvb_tags = tvb_new_real_data(tags, 3, 3);

    for (size_t ex_ix = 0; ex_ix < (sizeof(all_examples) / sizeof(example_s*)); ++ex_ix) {
        const example_s *ex = all_examples[ex_ix];
        printf("simple #%zu\n", ex_ix);

        tvbuff_t *tvb_item = tvb_new_real_data(ex->enc, ex->enc_len, ex->enc_len);
        tvbuff_t *tvb = tvb_new_composite();
        tvb_composite_append(tvb, tvb_tags);
        tvb_composite_append(tvb, tvb_item);
        tvb_composite_finalize(tvb);
        gint offset = 0;

        wscbor_chunk_t *chunk = wscbor_chunk_read(test_scope, tvb, &offset);
        g_assert_nonnull(chunk);
        g_assert_cmpuint(wscbor_has_errors(chunk), ==, 0);

        g_assert_cmpuint(chunk->head_length, ==, ex->head_length + 3);
        g_assert_cmpuint(chunk->data_length, ==, ex->data_length + 3);
        g_assert_cmpuint(wmem_list_count(chunk->tags), ==, 2);
        {
            wmem_list_frame_t *frm = wmem_list_head(chunk->tags);
            g_assert_nonnull(frm);
            {
                const wscbor_tag_t *tag = wmem_list_frame_data(frm);
                g_assert_cmpuint(tag->value, ==, 1);
            }
            frm = wmem_list_frame_next(frm);
            g_assert_nonnull(frm);
            {
                const wscbor_tag_t *tag = wmem_list_frame_data(frm);
                g_assert_cmpuint(tag->value, ==, 200);
            }
            frm = wmem_list_frame_next(frm);
            g_assert_null(frm);
        }
        g_assert_cmpuint(chunk->type_major, ==, ex->type_major);
        g_assert_cmpuint(chunk->head_value, ==, ex->head_value);

        wscbor_chunk_free(chunk);
        tvb_free(tvb_item);
    }

    tvb_free(tvb_tags);
}

static void
wscbor_test_chunk_read_invalid(void)
{
    tvbuff_t *tvb = tvb_new_real_data((const guint8 *)"\x00\x01\x02\xC1", 4, 2);
    gint offset = 2;

    { // last valid item
        wscbor_chunk_t *chunk = wscbor_chunk_read(test_scope, tvb, &offset);
        g_assert_nonnull(chunk);
        g_assert_cmpuint(wscbor_has_errors(chunk), ==, 0);
        g_assert_cmpuint(chunk->type_major, ==, CBOR_TYPE_UINT);
        g_assert_cmpuint(chunk->head_value, ==, 2);
        wscbor_chunk_free(chunk);
    }
    g_assert_cmpint(offset, ==, 3);
    { // Tag without item
        volatile gulong caught = 0;
        TRY {
            wscbor_chunk_read(test_scope, tvb, &offset);
            g_assert_true(FALSE);
        }
        CATCH_BOUNDS_ERRORS {
            caught = exc->except_id.except_code;
        }
        ENDTRY;
        g_assert_cmpuint(caught, ==, ReportedBoundsError);
    }
    g_assert_cmpint(offset, ==, 4);
    { // Read past the end
        volatile gulong caught = 0;
        TRY {
            wscbor_chunk_read(test_scope, tvb, &offset);
            g_assert_true(FALSE);
        }
        CATCH_BOUNDS_ERRORS {
            caught = exc->except_id.except_code;
        }
        ENDTRY;
        g_assert_cmpuint(caught, ==, ReportedBoundsError);
    }
    g_assert_cmpint(offset, ==, 4);

    tvb_free(tvb);
}

static void
wscbor_test_is_indefinite_break(void)
{
    for (size_t ex_ix = 0; ex_ix < (sizeof(all_examples) / sizeof(example_s*)); ++ex_ix) {
        const example_s *ex = all_examples[ex_ix];
        printf("simple #%zu\n", ex_ix);

        tvbuff_t *tvb = tvb_new_real_data(ex->enc, ex->enc_len, ex->enc_len);
        gint offset = 0;

        wscbor_chunk_t *chunk = wscbor_chunk_read(test_scope, tvb, &offset);
        g_assert_nonnull(chunk);
        g_assert_cmpuint(wscbor_has_errors(chunk), ==, 0);

        // this test never modifies the chunk
        const gboolean val = wscbor_is_indefinite_break(chunk);
        if (memcmp(ex->enc, "\xFF", 1) == 0) {
            g_assert_true(val);
            g_assert_cmpuint(wscbor_has_errors(chunk), ==, 0);
        }
        else {
            g_assert_false(val);
            g_assert_cmpuint(wscbor_has_errors(chunk), ==, 0);
        }

        wscbor_chunk_free(chunk);
        tvb_free(tvb);
    }
}

static void
wscbor_test_skip_next_item_simple(void)
{
    // skip simple items
    for (size_t ex_ix = 0; ex_ix < (sizeof(all_examples) / sizeof(example_s*)); ++ex_ix) {
        const example_s *ex = all_examples[ex_ix];
        printf("simple #%zu\n", ex_ix);

        tvbuff_t *tvb = tvb_new_real_data(ex->enc, ex->enc_len, ex->enc_len);
        gint offset = 0;
        gboolean valid = wscbor_skip_next_item(test_scope, tvb, &offset);
        // break is well-formed but not valid on its own
        g_assert_cmpint(valid, ==, (ex != &ex_break));
        g_assert_cmpint(offset, ==, ex->data_length);

        tvb_free(tvb);
    }
}

static void
wscbor_test_skip_next_item_multiple(void)
{
    tvbuff_t *tvb = tvb_new_real_data((const guint8 *)"\x00\x01\x02\x03", 4, 4);
    gint offset = 2;

    gboolean valid = wscbor_skip_next_item(test_scope, tvb, &offset);
    g_assert_true(valid);
    g_assert_cmpint(offset, ==, 3);

    valid = wscbor_skip_next_item(test_scope, tvb, &offset);
    g_assert_true(valid);
    g_assert_cmpint(offset, ==, 4);

    { // Read past the end
        volatile gulong caught = 0;
        TRY {
            wscbor_skip_next_item(test_scope, tvb, &offset);
            g_assert_true(FALSE);
        }
        CATCH_BOUNDS_ERRORS {
            caught = exc->except_id.except_code;
        }
        ENDTRY;
        g_assert_cmpuint(caught, ==, ReportedBoundsError);
    }
    g_assert_cmpint(offset, ==, 4);

    tvb_free(tvb);
}

static void
wscbor_test_require_major_type(void)
{
    for (size_t ex_ix = 0; ex_ix < (sizeof(all_examples) / sizeof(example_s*)); ++ex_ix) {
        const example_s *ex = all_examples[ex_ix];
        printf("simple #%zu\n", ex_ix);

        tvbuff_t *tvb = tvb_new_real_data(ex->enc, ex->enc_len, ex->enc_len);
        gint offset = 0;

        wscbor_chunk_t *chunk = wscbor_chunk_read(test_scope, tvb, &offset);
        g_assert_nonnull(chunk);
        g_assert_cmpuint(wscbor_has_errors(chunk), ==, 0);

        g_assert_true(wscbor_require_major_type(chunk, ex->type_major));
        g_assert_cmpuint(wscbor_has_errors(chunk), ==, 0);

        // any other type
        g_assert_false(wscbor_require_major_type(chunk, ex->type_major + 1));
        g_assert_cmpuint(wscbor_has_errors(chunk), ==, 1);

        wscbor_chunk_free(chunk);
        tvb_free(tvb);
    }
}

static void
wscbor_test_require_boolean_simple(void)
{
    for (size_t ex_ix = 0; ex_ix < (sizeof(all_examples) / sizeof(example_s*)); ++ex_ix) {
        const example_s *ex = all_examples[ex_ix];
        printf("simple #%zu\n", ex_ix);

        tvbuff_t *tvb = tvb_new_real_data(ex->enc, ex->enc_len, ex->enc_len);
        gint offset = 0;

        wscbor_chunk_t *chunk = wscbor_chunk_read(test_scope, tvb, &offset);
        g_assert_nonnull(chunk);
        g_assert_cmpuint(wscbor_has_errors(chunk), ==, 0);

        const gboolean *val = wscbor_require_boolean(test_scope, chunk);
        if ((ex->type_major == CBOR_TYPE_FLOAT_CTRL)
                && ((ex->head_value == CBOR_CTRL_FALSE) || (ex->head_value == CBOR_CTRL_TRUE))) {
            g_assert_nonnull(val);
            g_assert_cmpuint(wscbor_has_errors(chunk), ==, 0);
            g_assert_cmpint(*val, ==, ex->head_value == CBOR_CTRL_TRUE);
        }
        else {
            g_assert_null(val);
            g_assert_cmpuint(wscbor_has_errors(chunk), ==, 1);
        }

        wscbor_chunk_free(chunk);
        tvb_free(tvb);
    }
}

static void
wscbor_test_require_int64_simple(void)
{
    for (size_t ex_ix = 0; ex_ix < (sizeof(all_examples) / sizeof(example_s*)); ++ex_ix) {
        const example_s *ex = all_examples[ex_ix];
        printf("simple #%zu\n", ex_ix);

        tvbuff_t *tvb = tvb_new_real_data(ex->enc, ex->enc_len, ex->enc_len);
        gint offset = 0;

        wscbor_chunk_t *chunk = wscbor_chunk_read(test_scope, tvb, &offset);
        g_assert_nonnull(chunk);
        g_assert_cmpuint(wscbor_has_errors(chunk), ==, 0);

        const gint64 *val = wscbor_require_int64(test_scope, chunk);
        if (ex->type_major == CBOR_TYPE_UINT) {
            g_assert_nonnull(val);
            g_assert_cmpuint(wscbor_has_errors(chunk), ==, 0);
            g_assert_cmpint(*val, ==, ex->head_value);
        }
        else if (ex->type_major == CBOR_TYPE_NEGINT) {
            g_assert(val);
            g_assert_cmpuint(wscbor_has_errors(chunk), ==, 0);
            g_assert_cmpint(*val, ==, -1 - ex->head_value);
        }
        else {
            g_assert_null(val);
            g_assert_cmpuint(wscbor_has_errors(chunk), ==, 1);
        }

        wscbor_chunk_free(chunk);
        tvb_free(tvb);
    }
}

static void
wscbor_test_require_int64_overflow(void)
{
    const example_s * examples[] = {
        &ex_uint_overflow, &ex_nint_overflow,
    };
    for (size_t ex_ix = 0; ex_ix < (sizeof(examples) / sizeof(example_s*)); ++ex_ix) {
        const example_s *ex = examples[ex_ix];
        printf("simple #%zu\n", ex_ix);

        tvbuff_t *tvb = tvb_new_real_data(ex->enc, ex->enc_len, ex->enc_len);
        gint offset = 0;

        wscbor_chunk_t *chunk = wscbor_chunk_read(test_scope, tvb, &offset);
        g_assert_nonnull(chunk);
        g_assert_cmpuint(wscbor_has_errors(chunk), ==, 0);
        g_assert_cmpuint(chunk->type_major, ==, ex->type_major);
        g_assert_cmpuint(chunk->head_value, ==, ex->head_value);

        const gint64 *val = wscbor_require_int64(test_scope, chunk);
        if (ex->type_major == CBOR_TYPE_UINT) {
            g_assert_nonnull(val);
            g_assert_cmpuint(wscbor_has_errors(chunk), ==, 1);
            g_assert_cmpint(*val, ==, G_MAXINT64);
        }
        else if (ex->type_major == CBOR_TYPE_NEGINT) {
            g_assert_nonnull(val);
            g_assert_cmpuint(wscbor_has_errors(chunk), ==, 1);
            g_assert_cmpint(*val, ==, G_MININT64);
        }
        else {
            g_assert_null(val);
            g_assert_cmpuint(wscbor_has_errors(chunk), ==, 1);
        }

        wscbor_chunk_free(chunk);
        tvb_free(tvb);
    }
}

static void
wscbor_test_require_float64_simple(void)
{
    for (size_t ex_ix = 0; ex_ix < (sizeof(all_examples) / sizeof(example_s*)); ++ex_ix) {
        const example_s *ex = all_examples[ex_ix];
        printf("simple #%zu\n", ex_ix);

        tvbuff_t *tvb = tvb_new_real_data(ex->enc, ex->enc_len, ex->enc_len);
        gint offset = 0;

        wscbor_chunk_t *chunk = wscbor_chunk_read(test_scope, tvb, &offset);
        g_assert_nonnull(chunk);
        g_assert_cmpuint(wscbor_has_errors(chunk), ==, 0);

        const double *val = wscbor_require_float64(test_scope, chunk);
        if ((ex->type_major == CBOR_TYPE_FLOAT_CTRL)
            && !isnan(ex->float_value)) {
            g_assert_nonnull(val);
            g_assert_cmpuint(wscbor_has_errors(chunk), ==, 0);
            g_assert_cmpfloat(*val, ==, ex->float_value);
        }
        else {
            g_assert_null(val);
            g_assert_cmpuint(wscbor_has_errors(chunk), ==, 1);
        }

        wscbor_chunk_free(chunk);
        tvb_free(tvb);
    }
}

static void
wscbor_test_require_tstr_simple(void)
{
    for (size_t ex_ix = 0; ex_ix < (sizeof(all_examples) / sizeof(example_s*)); ++ex_ix) {
        const example_s *ex = all_examples[ex_ix];
        printf("simple #%zu\n", ex_ix);

        tvbuff_t *tvb = tvb_new_real_data(ex->enc, ex->enc_len, ex->enc_len);
        gint offset = 0;

        wscbor_chunk_t *chunk = wscbor_chunk_read(test_scope, tvb, &offset);
        g_assert_nonnull(chunk);
        g_assert_cmpuint(wscbor_has_errors(chunk), ==, 0);

        const char *val = wscbor_require_tstr(test_scope, chunk);
        if (ex->type_major == CBOR_TYPE_STRING) {
            g_assert_nonnull(val);
            g_assert_cmpuint(wscbor_has_errors(chunk), ==, 0);
            if (ex->head_value > 0) {
                // only works because this is Latin-1 text
                g_assert_cmpmem(val, (int)strlen(val), ex->enc + ex->head_length, (int)ex->head_value);
            }
        }
        else {
            g_assert_null(val);
            g_assert_cmpuint(wscbor_has_errors(chunk), ==, 1);
        }

        wscbor_chunk_free(chunk);
        tvb_free(tvb);
    }
}

static void
wscbor_test_require_tstr_short(void)
{
    const example_s * examples[] = {
        &ex_tstr_short,
    };
    tvbuff_t *tvb = NULL;
    wscbor_chunk_t *chunk = NULL;

    for (size_t ex_ix = 0; ex_ix < (sizeof(examples) / sizeof(example_s*)); ++ex_ix) {
        const example_s *ex = examples[ex_ix];
        printf("simple #%zu\n", ex_ix);

        tvb = tvb_new_real_data(ex->enc, ex->enc_len, ex->enc_len);
        gint offset = 0;

        chunk = wscbor_chunk_read(test_scope, tvb, &offset);
        g_assert_nonnull(chunk);
        g_assert_cmpuint(wscbor_has_errors(chunk), ==, 0);
        g_assert_cmpuint(chunk->type_major, ==, ex->type_major);
        g_assert_cmpuint(chunk->head_value, ==, ex->head_value);

        volatile gulong caught = 0;
        TRY {
            wscbor_require_tstr(test_scope, chunk);
            g_assert_true(FALSE);
        }
        CATCH_BOUNDS_ERRORS {
            caught = exc->except_id.except_code;
        }
        ENDTRY;
        g_assert_cmpuint(caught, ==, ContainedBoundsError);

        wscbor_chunk_free(chunk);
        tvb_free(tvb);
    }
}

static void
wscbor_test_require_bstr_simple(void)
{
    for (size_t ex_ix = 0; ex_ix < (sizeof(all_examples) / sizeof(example_s*)); ++ex_ix) {
        const example_s *ex = all_examples[ex_ix];
        printf("simple #%zu\n", ex_ix);

        tvbuff_t *tvb = tvb_new_real_data(ex->enc, ex->enc_len, ex->enc_len);
        gint offset = 0;

        wscbor_chunk_t *chunk = wscbor_chunk_read(test_scope, tvb, &offset);
        g_assert_nonnull(chunk);
        g_assert_cmpuint(wscbor_has_errors(chunk), ==, 0);

        tvbuff_t *val = wscbor_require_bstr(test_scope, chunk);
        if (ex->type_major == CBOR_TYPE_BYTESTRING) {
            g_assert_nonnull(val);
            g_assert_cmpuint(wscbor_has_errors(chunk), ==, 0);
            if (ex->head_value > 0) {
                g_assert_cmpint(tvb_reported_length(val), ==, ex->head_value);
                g_assert_cmpuint(tvb_captured_length(val), ==, ex->head_value);

                const gint buflen = tvb_reported_length(val);
                void *buf = tvb_memdup(test_scope, val, 0, buflen);
                g_assert_nonnull(buf);
                g_assert_cmpmem(buf, (int)buflen, ex->enc + ex->head_length, (int)ex->head_value);
            }
        }
        else {
            g_assert_null(val);
            g_assert_cmpuint(wscbor_has_errors(chunk), ==, 1);
        }

        wscbor_chunk_free(chunk);
        tvb_free(tvb);
    }
}

static void
wscbor_test_require_bstr_short(void)
{
    const example_s * examples[] = {
        &ex_bstr_short,
    };
    tvbuff_t *tvb = NULL;
    wscbor_chunk_t *chunk = NULL;
    tvbuff_t *val = NULL;

    for (size_t ex_ix = 0; ex_ix < (sizeof(examples) / sizeof(example_s*)); ++ex_ix) {
        const example_s *ex = examples[ex_ix];
        printf("simple #%zu\n", ex_ix);

        tvb = tvb_new_real_data(ex->enc, ex->enc_len, ex->enc_len);
        gint offset = 0;

        chunk = wscbor_chunk_read(test_scope, tvb, &offset);
        g_assert_nonnull(chunk);
        g_assert_cmpuint(wscbor_has_errors(chunk), ==, 0);
        g_assert_cmpuint(chunk->type_major, ==, ex->type_major);
        g_assert_cmpuint(chunk->head_value, ==, ex->head_value);

        // no exception, but truncated captured length
        val = wscbor_require_bstr(test_scope, chunk);
        g_assert_nonnull(val);
        g_assert_cmpuint(wscbor_has_errors(chunk), ==, 0);
        g_assert_cmpint(tvb_reported_length(val), ==, ex->head_value);
        g_assert_cmpuint(tvb_captured_length(val), <, ex->head_value);

        volatile gulong caught = 0;
        TRY {
            const gint buflen = tvb_reported_length(val);
            tvb_memdup(test_scope, val, 0, buflen);
            g_assert_true(FALSE);
        }
        CATCH_BOUNDS_ERRORS {
            caught = exc->except_id.except_code;
        }
        ENDTRY;
        g_assert_cmpuint(caught, ==, ContainedBoundsError);

        wscbor_chunk_free(chunk);
        tvb_free(tvb);
    }
}

static void
wscbor_test_require_bstr_overflow(void)
{
    const example_s * examples[] = {
        &ex_bstr_overflow,
    };
    for (size_t ex_ix = 0; ex_ix < (sizeof(examples) / sizeof(example_s*)); ++ex_ix) {
        const example_s *ex = examples[ex_ix];
        printf("simple #%zu\n", ex_ix);

        tvbuff_t *tvb = tvb_new_real_data(ex->enc, ex->enc_len, ex->enc_len);
        gint offset = 0;

        wscbor_chunk_t *chunk = wscbor_chunk_read(test_scope, tvb, &offset);
        g_assert_nonnull(chunk);
        g_assert_cmpuint(wscbor_has_errors(chunk), ==, 1);
        g_assert_cmpuint(chunk->type_major, ==, ex->type_major);
        g_assert_cmpuint(chunk->head_value, ==, ex->head_value);

        const tvbuff_t *val = wscbor_require_bstr(test_scope, chunk);
        g_assert_nonnull(val);
        g_assert_cmpuint(wscbor_has_errors(chunk), ==, 1);
        g_assert_cmpuint(tvb_reported_length(val), ==, G_MAXINT);
        g_assert_cmpuint(tvb_captured_length(val), ==, 2);

        wscbor_chunk_free(chunk);
        tvb_free(tvb);
    }
}

int
main(int argc, char **argv)
{
    int result;

    g_test_init(&argc, &argv, NULL);

    g_test_add_func("/wscbor/chunk_read/simple", wscbor_test_chunk_read_simple);
    g_test_add_func("/wscbor/chunk_read/simple_tags", wscbor_test_chunk_read_simple_tags);
    g_test_add_func("/wscbor/chunk_read/invalid", wscbor_test_chunk_read_invalid);
    g_test_add_func("/wscbor/is_indefinite_break", wscbor_test_is_indefinite_break);
    g_test_add_func("/wscbor/skip_next_item/simple", wscbor_test_skip_next_item_simple);
    g_test_add_func("/wscbor/skip_next_item/multiple", wscbor_test_skip_next_item_multiple);
    g_test_add_func("/wscbor/require_major_type", wscbor_test_require_major_type);
    g_test_add_func("/wscbor/require_boolean/simple", wscbor_test_require_boolean_simple);
    g_test_add_func("/wscbor/require_int64/simple", wscbor_test_require_int64_simple);
    g_test_add_func("/wscbor/require_int64/overflow", wscbor_test_require_int64_overflow);
    g_test_add_func("/wscbor/require_float64/simple", wscbor_test_require_float64_simple);
    g_test_add_func("/wscbor/require_tstr/simple", wscbor_test_require_tstr_simple);
    g_test_add_func("/wscbor/require_tstr/short", wscbor_test_require_tstr_short);
    g_test_add_func("/wscbor/require_bstr/simple", wscbor_test_require_bstr_simple);
    g_test_add_func("/wscbor/require_bstr/short", wscbor_test_require_bstr_short);
    g_test_add_func("/wscbor/require_bstr/overflow", wscbor_test_require_bstr_overflow);
    wmem_init_scopes();

    test_scope = wmem_allocator_new(WMEM_ALLOCATOR_STRICT);
    //cannot use: wscbor_init();
    result = g_test_run();
    //none needed: wscbor_cleanup();
    wmem_destroy_allocator(test_scope);
    wmem_cleanup_scopes();

    return result;
}

/*
 * Editor modelines  -  https://www.wireshark.org/tools/modelines.html
 *
 * Local variables:
 * c-basic-offset: 4
 * tab-width: 8
 * indent-tabs-mode: nil
 * End:
 *
 * vi: set shiftwidth=4 tabstop=8 expandtab:
 * :indentSize=4:tabSize=8:noTabs=true:
 */
