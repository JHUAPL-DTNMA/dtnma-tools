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
 * Test the ari_text.h interfaces.
 */
#include <cace/ari/text.h>
#include <cace/ari/text_util.h>
#include <cace/amm/typing.h>
#include <unity.h>

// Allow this macro
#define TEST_CASE(...)

/// Resource cleanup for failure messages
static const char *errm = NULL;

void tearDown(void)
{
    if (errm)
    {
        M_MEMORY_FREE((char *)errm);
        errm = NULL;
    }
}

/** Check a single call to ari_text_encode().
 */
static void check_encode(const ari_t *ari, const char *expect, const ari_text_enc_opts_t opts)
{
    string_t got;
    string_init(got);

    int res = ari_text_encode(got, ari, opts);
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, res, "ari_text_encode() failed");
    // include terminating null
    //  const size_t expect_len = strlen(expect) + 1;
    //  TEST_ASSERT_EQUAL_MESSAGE(expect_len, buf.len, "Mismatch in encoded length");
    TEST_ASSERT_EQUAL_STRING_MESSAGE(expect, string_get_cstr(got), "Mismatch in encoded data");

    string_clear(got);
}

void test_ari_text_encode_lit_prim_undefined(void)
{
    ari_t ari = ARI_INIT_UNDEFINED;
    // no value is undefined

    const char *expect = "ari:undefined";
    check_encode(&ari, expect, ARI_TEXT_ENC_OPTS_DEFAULT);

    ari_deinit(&ari);
}

void test_ari_text_encode_lit_prim_null(void)
{
    ari_t ari = ARI_INIT_UNDEFINED;
    ari_set_null(&ari);

    const char *expect = "ari:null";
    check_encode(&ari, expect, ARI_TEXT_ENC_OPTS_DEFAULT);

    ari_deinit(&ari);
}

TEST_CASE(0, ARI_TEXT_INT_BASE10, "ari:0")
TEST_CASE(0, ARI_TEXT_INT_BASE2, "ari:0b0")
TEST_CASE(0, ARI_TEXT_INT_BASE16, "ari:0x0")
TEST_CASE(1234, ARI_TEXT_INT_BASE10, "ari:1234")
TEST_CASE(1234, ARI_TEXT_INT_BASE2, "ari:0b10011010010")
TEST_CASE(1234, ARI_TEXT_INT_BASE16, "ari:0x4D2")
TEST_CASE(-1234, ARI_TEXT_INT_BASE10, "ari:-1234")
TEST_CASE(-1234, ARI_TEXT_INT_BASE2, "ari:-0b10011010010")
TEST_CASE(-1234, ARI_TEXT_INT_BASE16, "ari:-0x4D2")
void test_ari_text_encode_lit_prim_int(int64_t value, enum ari_int_base_e base, const char *expect)
{
    ari_t ari = ARI_INIT_UNDEFINED;
    ari_set_prim_int64(&ari, value);

    ari_text_enc_opts_t opts = ARI_TEXT_ENC_OPTS_DEFAULT;
    opts.int_base            = base;
    check_encode(&ari, expect, opts);
    ari_deinit(&ari);
}

TEST_CASE(0, ARI_TEXT_INT_BASE10, "ari:0")
TEST_CASE(0, ARI_TEXT_INT_BASE2, "ari:0b0")
TEST_CASE(0, ARI_TEXT_INT_BASE16, "ari:0x0")
TEST_CASE(1234, ARI_TEXT_INT_BASE10, "ari:1234")
TEST_CASE(1234, ARI_TEXT_INT_BASE2, "ari:0b10011010010")
TEST_CASE(1234, ARI_TEXT_INT_BASE16, "ari:0x4D2")
TEST_CASE(0xFFFFFFFFFFFFFFFF, ARI_TEXT_INT_BASE16, "ari:0xFFFFFFFFFFFFFFFF")
void test_ari_text_encode_lit_prim_uint(uint64_t value, enum ari_int_base_e base, const char *expect)
{
    ari_t ari = ARI_INIT_UNDEFINED;
    ari_set_prim_uint64(&ari, value);

    ari_text_enc_opts_t opts = ARI_TEXT_ENC_OPTS_DEFAULT;
    opts.int_base            = base;
    check_encode(&ari, expect, opts);
    ari_deinit(&ari);
}

TEST_CASE(1.1, 'f', "ari:1.100000")
TEST_CASE(1.1, 'g', "ari:1.1")
TEST_CASE(1.1e2, 'g', "ari:110")
TEST_CASE(1.1e2, 'a', "ari:0x1.b8p+6")
TEST_CASE(1.1e+10, 'g', "ari:1.1e+10")
TEST_CASE(10, 'e', "ari:1.000000e+01")
TEST_CASE(10, 'a', "ari:0x1.4p+3")
TEST_CASE((ari_real64)NAN, ' ', "ari:NaN")
TEST_CASE((ari_real64)INFINITY, ' ', "ari:+Infinity")
TEST_CASE((ari_real64)-INFINITY, ' ', "ari:-Infinity")
void test_ari_text_encode_lit_prim_float64(ari_real64 value, char form, const char *expect)
{
    ari_t ari = ARI_INIT_UNDEFINED;
    ari_set_prim_float64(&ari, value);

    ari_text_enc_opts_t opts = ARI_TEXT_ENC_OPTS_DEFAULT;
    opts.float_form          = form;
    check_encode(&ari, expect, opts);
    ari_deinit(&ari);
}

TEST_CASE("test", false, true, "ari:test")
TEST_CASE("test", false, false, "ari:%22test%22")
TEST_CASE("test", true, true, "ari:test")
TEST_CASE("\\'\'", true, true, "ari:%22%5C''%22")
TEST_CASE("':!@$%^&*()-+[]{},./?", true, true, "ari:%22':!@%24%25%5E%26%2A%28%29-+%5B%5D%7B%7D%2C.%2F%3F%22")
TEST_CASE("_-~The quick brown fox", true, true, "ari:%22_-~The%20quick%20brown%20fox%22")
TEST_CASE("hi\u1234", false, false, "ari:%22hi%5Cu1234%22")
TEST_CASE("hi\U0001D11E", false, false, "ari:%22hi%5CuD834%5CuDD1E%22")
void test_ari_text_encode_lit_prim_tstr(const char *value, bool copy, bool text_identity, const char *expect)
{
    ari_t ari = ARI_INIT_UNDEFINED;
    ari_set_tstr(&ari, value, copy);

    ari_text_enc_opts_t opts = ARI_TEXT_ENC_OPTS_DEFAULT;
    opts.text_identity       = text_identity;
    check_encode(&ari, expect, opts);
    ari_deinit(&ari);
}

TEST_CASE("", 0, ARI_TEXT_BSTR_RAW, "ari:''")
TEST_CASE("test", 4, ARI_TEXT_BSTR_RAW, "ari:'test'")
TEST_CASE("hi\u1234", 5, ARI_TEXT_BSTR_RAW, "ari:'hi%5Cu1234'")
TEST_CASE("hi\U0001D11E", 6, ARI_TEXT_BSTR_RAW, "ari:'hi%5CuD834%5CuDD1E'")
// below is special case of early null character that cannot be shown raw
TEST_CASE("\x68\x00\x69", 3, ARI_TEXT_BSTR_RAW, "ari:h'680069'")
TEST_CASE("", 0, ARI_TEXT_BSTR_BASE16, "ari:h''")
TEST_CASE("", 0, ARI_TEXT_BSTR_BASE64URL, "ari:b64''")
TEST_CASE("f", 1, ARI_TEXT_BSTR_BASE64URL, "ari:b64'Zg=='")
// examples from Section 10 of RFC 4648
TEST_CASE("foobar", 6, ARI_TEXT_BSTR_BASE16, "ari:h'666F6F626172'")
TEST_CASE("foobar", 6, ARI_TEXT_BSTR_BASE64URL, "ari:b64'Zm9vYmFy'")
void test_ari_text_encode_lit_prim_bstr(const char *data, size_t data_len, enum ari_bstr_form_e form,
                                        const char *expect)
{
    ari_t ari = ARI_INIT_UNDEFINED;
    {
        cace_data_t src;
        cace_data_init_view(&src, data_len, (cace_data_ptr_t)data);
        ari_set_bstr(&ari, &src, true);
    }

    ari_text_enc_opts_t opts = ARI_TEXT_ENC_OPTS_DEFAULT;
    opts.bstr_form           = form;
    check_encode(&ari, expect, opts);
    ari_deinit(&ari);
}

void test_ari_text_encode_lit_typed_ac_empty(void)
{
    ari_t ari = ARI_INIT_UNDEFINED;
    ari_set_ac(&ari, NULL);
    {
        const char *expect = "ari:/AC/()";
        check_encode(&ari, expect, ARI_TEXT_ENC_OPTS_DEFAULT);
    }
    ari_deinit(&ari);
}

void test_ari_text_encode_lit_typed_ac_1item(void)
{
    {
        ari_ac_t ctr;
        ari_ac_init(&ctr);
        {
            ari_t *item = ari_list_push_back_new(ctr.items);
            ari_set_null(item);
        }

        ari_t ari = ARI_INIT_UNDEFINED;
        ari_set_ac(&ari, &ctr);
        {
            const char *expect = "ari:/AC/(null)";
            check_encode(&ari, expect, ARI_TEXT_ENC_OPTS_DEFAULT);
        }
        ari_deinit(&ari);
    }
}

void test_ari_text_encode_lit_typed_ac_2item(void)
{
    {
        ari_ac_t ctr;
        ari_ac_init(&ctr);
        {
            ari_t *item = ari_list_push_back_new(ctr.items);
            ari_set_null(item);
        }
        {
            ari_t *item = ari_list_push_back_new(ctr.items);
            ari_set_bool(item, false);
        }

        ari_t ari = ARI_INIT_UNDEFINED;
        ari_set_ac(&ari, &ctr);
        {
            const char *expect = "ari:/AC/(null,false)";
            check_encode(&ari, expect, ARI_TEXT_ENC_OPTS_DEFAULT);
        }
        ari_deinit(&ari);
    }
}

void test_ari_text_encode_lit_typed_am_2item(void)
{
    {
        ari_am_t ctr;
        ari_am_init(&ctr);
        {
            ari_t key;
            ari_init(&key);
            ari_set_prim_uint64(&key, 1);
            ari_t *val = ari_tree_safe_get(ctr.items, key);
            ari_set_null(val);
            ari_deinit(&key);
        }
        {
            ari_t key;
            ari_init(&key);
            ari_set_prim_uint64(&key, 2);
            ari_t *val = ari_tree_safe_get(ctr.items, key);
            ari_set_bool(val, false);
            ari_deinit(&key);
        }

        ari_t ari = ARI_INIT_UNDEFINED;
        ari_set_am(&ari, &ctr);
        {
            const char *expect = "ari:/AM/(1=null,2=false)";
            check_encode(&ari, expect, ARI_TEXT_ENC_OPTS_DEFAULT);
        }
        ari_deinit(&ari);
    }
}

void test_ari_text_encode_lit_typed_execset_2tgt(void)
{
    {
        ari_t          ari;
        ari_execset_t *ctr = ari_init_execset(&ari);
        {
            ari_set_prim_uint64(&(ctr->nonce), 12345678);
        }
        {
            ari_t *item = ari_list_push_back_new(ctr->targets);
            ari_set_objref_path_textid(item, "adm", ARI_TYPE_CTRL, "one");
        }
        {
            ari_t *item = ari_list_push_back_new(ctr->targets);
            ari_set_objref_path_textid(item, "adm", ARI_TYPE_CTRL, "two");
        }

        {
            const char *expect = "ari:/EXECSET/n=12345678;(//adm/CTRL/one,//adm/CTRL/two)";
            check_encode(&ari, expect, ARI_TEXT_ENC_OPTS_DEFAULT);
        }
        ari_deinit(&ari);
    }
}

TEST_CASE("adm", ARI_TYPE_CONST, "hi", "ari://adm/CONST/hi")
TEST_CASE("18", ARI_TYPE_IDENT, "34", "ari://18/IDENT/34")
void test_ari_text_encode_objref_text(const char *ns_id, ari_type_t type_id, const char *obj_id, const char *expect)
{
    ari_t ari = ARI_INIT_UNDEFINED;
    ari_set_objref_path_textid(&ari, ns_id, type_id, obj_id);

    ari_text_enc_opts_t opts = ARI_TEXT_ENC_OPTS_DEFAULT;
    check_encode(&ari, expect, opts);
    ari_deinit(&ari);
}

TEST_CASE(18, ARI_TYPE_IDENT, 34, "ari://18/IDENT/34")
void test_ari_text_encode_objref_int(int64_t ns_id, ari_type_t type_id, int64_t obj_id, const char *expect)
{
    ari_t ari = ARI_INIT_UNDEFINED;
    ari_set_objref_path_intid(&ari, ns_id, type_id, obj_id);

    ari_text_enc_opts_t opts = ARI_TEXT_ENC_OPTS_DEFAULT;
    check_encode(&ari, expect, opts);
    ari_deinit(&ari);
}

TEST_CASE("adm", "ari://adm/")
TEST_CASE("example-adm-a@2024-06-25", "ari://example-adm-a@2024-06-25/")
TEST_CASE("example-adm-a", "ari://example-adm-a/")
TEST_CASE("!example-odm-b", "ari://!example-odm-b/")
TEST_CASE("65536", "ari://65536/")
TEST_CASE("-20", "ari://-20/")
void test_ari_text_encode_nsref_text(const char *ns_id, const char *expect)
{
    ari_t      ari = ARI_INIT_UNDEFINED;
    ari_ref_t *ref = ari_init_objref(&ari);
    {
        ref->objpath.ns_id.form = ARI_IDSEG_TEXT;
        string_t *value         = &(ref->objpath.ns_id.as_text);
        string_init_set_str(*value, ns_id);
    }

    ari_text_enc_opts_t opts = ARI_TEXT_ENC_OPTS_DEFAULT;
    check_encode(&ari, expect, opts);
    ari_deinit(&ari);
}

TEST_CASE(18, "ari://18/")
TEST_CASE(65536, "ari://65536/")
TEST_CASE(-20, "ari://-20/")
void test_ari_text_encode_nsref_int(int64_t ns_id, const char *expect)
{
    ari_t      ari = ARI_INIT_UNDEFINED;
    ari_ref_t *ref = ari_init_objref(&ari);
    {
        ref->objpath.ns_id.form   = ARI_IDSEG_INT;
        ref->objpath.ns_id.as_int = ns_id;
    }

    ari_text_enc_opts_t opts = ARI_TEXT_ENC_OPTS_DEFAULT;
    check_encode(&ari, expect, opts);
    ari_deinit(&ari);
}

TEST_CASE(ARI_TYPE_CONST, "hi", "./CONST/hi")
TEST_CASE(ARI_TYPE_IDENT, "34", "./IDENT/34")
void test_ari_text_encode_ariref(ari_type_t type_id, const char *obj_id, const char *expect)
{
    ari_t      ari = ARI_INIT_UNDEFINED;
    ari_ref_t *ref = ari_init_objref(&ari);
    {
        ref->objpath.ns_id.form = ARI_IDSEG_NULL;
    }
    {
        const char *type_name     = ari_type_to_name(type_id);
        ref->objpath.type_id.form = ARI_IDSEG_TEXT;
        string_t *value           = &(ref->objpath.type_id.as_text);
        string_init_set_str(*value, type_name);
    }
    {
        ref->objpath.obj_id.form = ARI_IDSEG_TEXT;
        string_t *value          = &(ref->objpath.obj_id.as_text);
        string_init_set_str(*value, obj_id);
    }

    ref->objpath.has_ari_type = true;
    ref->objpath.ari_type     = type_id;

    ari_text_enc_opts_t opts = ARI_TEXT_ENC_OPTS_DEFAULT;
    opts.scheme_prefix       = ARI_TEXT_SCHEME_NONE;
    check_encode(&ari, expect, opts);
    ari_deinit(&ari);
}

#if defined(ARI_TEXT_PARSE)

/** Check a single call to ari_text_decode().
 */
static void check_decode(ari_t *ari, const char *text)
{
    string_t inbuf;
    string_init_set_str(inbuf, text);
    int ret = ari_text_decode(ari, inbuf, &errm);
    string_clear(inbuf);
    if (ret && errm)
    {
        TEST_FAIL_MESSAGE(errm);
    }
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, ret, "ari_text_decode() failed");
}

TEST_CASE("undefined")
TEST_CASE("UnDefinEd")
void test_ari_text_decode_lit_prim_undefined(const char *text)
{
    ari_t ari = ARI_INIT_UNDEFINED;
    check_decode(&ari, text);
    TEST_ASSERT_FALSE(ari.is_ref);
    TEST_ASSERT_FALSE(ari.as_lit.has_ari_type);
    TEST_ASSERT_EQUAL_INT(ARI_PRIM_UNDEFINED, ari.as_lit.prim_type);
    ari_deinit(&ari);
}

TEST_CASE("null")
TEST_CASE("NULL")
TEST_CASE("nUlL")
void test_ari_text_decode_lit_prim_null(const char *text)
{
    ari_t ari = ARI_INIT_UNDEFINED;
    check_decode(&ari, text);
    TEST_ASSERT_FALSE(ari.is_ref);
    TEST_ASSERT_FALSE(ari.as_lit.has_ari_type);
    TEST_ASSERT_EQUAL_INT(ARI_PRIM_NULL, ari.as_lit.prim_type);
    ari_deinit(&ari);
}

TEST_CASE("false", false)
TEST_CASE("true", true)
TEST_CASE("TRUE", true)
void test_ari_text_decode_lit_prim_bool(const char *text, ari_bool expect)
{
    ari_t ari = ARI_INIT_UNDEFINED;
    check_decode(&ari, text);
    TEST_ASSERT_FALSE(ari.is_ref);
    TEST_ASSERT_FALSE(ari.as_lit.has_ari_type);
    TEST_ASSERT_EQUAL_INT(ARI_PRIM_BOOL, ari.as_lit.prim_type);
    TEST_ASSERT_EQUAL(expect, ari.as_lit.value.as_bool);
    ari_deinit(&ari);
}

TEST_CASE("-0x8000000000000000", -0x8000000000000000)
TEST_CASE("-0x7FFFFFFFFFFFFFFF", -0x7FFFFFFFFFFFFFFF)
TEST_CASE("-4294967297", -4294967297)
TEST_CASE("-10", -10)
TEST_CASE("-0x10", -0x10)
TEST_CASE("-1", -1)
TEST_CASE("+0", 0)
TEST_CASE("+10", 10)
TEST_CASE("+0b1010", 10)
TEST_CASE("+0X10", 0x10)
TEST_CASE("+4294967296", 4294967296)
TEST_CASE("+0x7FFFFFFFFFFFFFFF", 0x7FFFFFFFFFFFFFFF)
TEST_CASE("0", 0)
TEST_CASE("-0", 0)
TEST_CASE("+0", 0)
TEST_CASE("10", 10)
TEST_CASE("0b1010", 10)
TEST_CASE("0B1010", 10)
TEST_CASE("0B0111111111111111111111111111111111111111111111111111111111111111", 0x7FFFFFFFFFFFFFFF)
TEST_CASE("0x10", 0x10)
TEST_CASE("4294967296", 4294967296)
TEST_CASE("0x7FFFFFFFffFFFFFF", 0x7FFFFFFFFFFFFFFF)
void test_ari_text_decode_lit_prim_int64(const char *text, int64_t expect)
{
    ari_t ari = ARI_INIT_UNDEFINED;
    check_decode(&ari, text);
    TEST_ASSERT_FALSE(ari.is_ref);
    TEST_ASSERT_FALSE(ari.as_lit.has_ari_type);
    TEST_ASSERT_EQUAL_INT(ARI_PRIM_INT64, ari.as_lit.prim_type);
    TEST_ASSERT_EQUAL_INT(expect, ari.as_lit.value.as_int64);
    ari_deinit(&ari);
}

TEST_CASE("0x8000000000000000", 0x8000000000000000)
TEST_CASE("0xFFFFFFFFFFFFFFFF", 0xFFFFFFFFFFFFFFFF)
void test_ari_text_decode_lit_prim_uint64(const char *text, uint64_t expect)
{
    ari_t ari = ARI_INIT_UNDEFINED;
    check_decode(&ari, text);
    TEST_ASSERT_FALSE(ari.is_ref);
    TEST_ASSERT_FALSE(ari.as_lit.has_ari_type);
    TEST_ASSERT_EQUAL_INT(ARI_PRIM_UINT64, ari.as_lit.prim_type);
    TEST_ASSERT_EQUAL_INT(expect, ari.as_lit.value.as_uint64);
    ari_deinit(&ari);
}

TEST_CASE("ari:/BYTE/0", 0)
TEST_CASE("ari:/BYTE/0xff", 255)
TEST_CASE("ari:/BYTE/0b10000000", 128)
void test_ari_text_decode_lit_typed_byte(const char *text, uint64_t expect)
{
    ari_t ari = ARI_INIT_UNDEFINED;
    check_decode(&ari, text);
    TEST_ASSERT_FALSE(ari.is_ref);
    TEST_ASSERT_TRUE(ari.as_lit.has_ari_type);
    TEST_ASSERT_EQUAL_INT(ARI_TYPE_BYTE, ari.as_lit.ari_type);
    TEST_ASSERT_EQUAL_INT(ARI_PRIM_INT64, ari.as_lit.prim_type);
    TEST_ASSERT_EQUAL(expect, ari.as_lit.value.as_int64);
    ari_deinit(&ari);
}

TEST_CASE("ari:/INT/0", 0)
TEST_CASE("ari:/INT/1234", 1234)
TEST_CASE("ari:/INT/-0xff", -255)
TEST_CASE("ari:/INT/0b10000000", 128)
void test_ari_text_decode_lit_typed_int(const char *text, uint64_t expect)
{
    ari_t ari = ARI_INIT_UNDEFINED;
    check_decode(&ari, text);
    TEST_ASSERT_FALSE(ari.is_ref);
    TEST_ASSERT_TRUE(ari.as_lit.has_ari_type);
    TEST_ASSERT_EQUAL_INT(ARI_TYPE_INT, ari.as_lit.ari_type);
    TEST_ASSERT_EQUAL_INT(ARI_PRIM_INT64, ari.as_lit.prim_type);
    TEST_ASSERT_EQUAL(expect, ari.as_lit.value.as_int64);
    ari_deinit(&ari);
}

TEST_CASE("ari:/UINT/-0", 0)
TEST_CASE("ari:/UINT/0xff", 255)
TEST_CASE("ari:/UINT/0b10000000", 128)
void test_ari_text_decode_lit_typed_uint(const char *text, uint64_t expect)
{
    ari_t ari = ARI_INIT_UNDEFINED;
    check_decode(&ari, text);
    TEST_ASSERT_FALSE(ari.is_ref);
    TEST_ASSERT_TRUE(ari.as_lit.has_ari_type);
    TEST_ASSERT_EQUAL_INT(ARI_TYPE_UINT, ari.as_lit.ari_type);
    TEST_ASSERT_EQUAL_INT(ARI_PRIM_INT64, ari.as_lit.prim_type);
    TEST_ASSERT_EQUAL(expect, ari.as_lit.value.as_uint64);
    ari_deinit(&ari);
}

TEST_CASE("ari:/VAST/-0", 0)
TEST_CASE("ari:/VAST/0xff", 255)
TEST_CASE("ari:/VAST/0b10000000", 128)
void test_ari_text_decode_lit_typed_vast(const char *text, uint64_t expect)
{
    ari_t ari = ARI_INIT_UNDEFINED;
    check_decode(&ari, text);
    TEST_ASSERT_FALSE(ari.is_ref);
    TEST_ASSERT_TRUE(ari.as_lit.has_ari_type);
    TEST_ASSERT_EQUAL_INT(ARI_TYPE_VAST, ari.as_lit.ari_type);
    TEST_ASSERT_EQUAL_INT(ARI_PRIM_INT64, ari.as_lit.prim_type);
    TEST_ASSERT_EQUAL(expect, ari.as_lit.value.as_int64);
    ari_deinit(&ari);
}

TEST_CASE("ari:/UVAST/0x8000000000000000", 0x8000000000000000)
TEST_CASE("ari:/UVAST/0xFFFFFFFFFFFFFFFF", 0xFFFFFFFFFFFFFFFF)
void test_ari_text_decode_lit_typed_uvast(const char *text, uint64_t expect)
{
    ari_t ari = ARI_INIT_UNDEFINED;
    check_decode(&ari, text);
    TEST_ASSERT_FALSE(ari.is_ref);
    TEST_ASSERT_TRUE(ari.as_lit.has_ari_type);
    TEST_ASSERT_EQUAL_INT(ARI_TYPE_UVAST, ari.as_lit.ari_type);
    TEST_ASSERT_EQUAL_INT(ARI_PRIM_UINT64, ari.as_lit.prim_type);
    TEST_ASSERT_EQUAL(expect, ari.as_lit.value.as_uint64);
    ari_deinit(&ari);
}

TEST_CASE("1.1", 1.1)
TEST_CASE("1.1e2", 1.1e2)
TEST_CASE("1.1e+10", 1.1e+10)
TEST_CASE("0x1.4p+3", 10)
TEST_CASE("NaN", (ari_real64)NAN)
TEST_CASE("nan", (ari_real64)NAN)
TEST_CASE("infinity", (ari_real64)INFINITY)
TEST_CASE("+Infinity", (ari_real64)INFINITY)
TEST_CASE("-Infinity", (ari_real64)-INFINITY)
void test_ari_text_decode_lit_prim_float64(const char *text, ari_real64 expect)
{
    ari_t ari = ARI_INIT_UNDEFINED;
    check_decode(&ari, text);
    TEST_ASSERT_FALSE(ari.is_ref);
    TEST_ASSERT_FALSE(ari.as_lit.has_ari_type);
    TEST_ASSERT_EQUAL_INT(ARI_PRIM_FLOAT64, ari.as_lit.prim_type);
    TEST_ASSERT_EQUAL_DOUBLE(expect, ari.as_lit.value.as_float64);
    ari_deinit(&ari);
}

TEST_CASE("ari:/REAL32/0", 0.0)
TEST_CASE("ari:/REAL32/-0.", 0.0)
TEST_CASE("ari:/REAL32/0.255", 0.255)
TEST_CASE("ari:/REAL32/0xF", 15.0)
TEST_CASE("ari:/REAL32/0xF.", 15.0)
TEST_CASE("ari:/REAL32/0xfF", 255.0)
TEST_CASE("ari:/REAL32/0xfF.ff", 255.255)
TEST_CASE("ari:/REAL32/0xfF.ffp0", 255.255)
TEST_CASE("ari:/REAL32/0xfF.ffp+0", 255.255)
TEST_CASE("ari:/REAL32/0x1.b8p+6", 1.1e2)
TEST_CASE("ari:/REAL32/0x1p+6", 64)
void test_ari_text_decode_lit_typed_float32(const char *text, ari_real32 expect)
{
    ari_t ari = ARI_INIT_UNDEFINED;
    check_decode(&ari, text);
    TEST_ASSERT_FALSE(ari.is_ref);
    TEST_ASSERT_TRUE(ari.as_lit.has_ari_type);
    TEST_ASSERT_EQUAL_INT(ARI_TYPE_REAL32, ari.as_lit.ari_type);
    TEST_ASSERT_EQUAL_INT(ARI_PRIM_FLOAT64, ari.as_lit.prim_type);
    TEST_ASSERT_EQUAL(expect, ari.as_lit.value.as_float64);
    ari_deinit(&ari);
}

TEST_CASE("ari:/REAL64/0", 0.0)
TEST_CASE("ari:/REAL64/-0.", 0.0)
TEST_CASE("ari:/REAL64/0.255", 0.255)
TEST_CASE("ari:/REAL64/0xF", 15.0)
TEST_CASE("ari:/REAL64/0xF.", 15.0)
TEST_CASE("ari:/REAL64/0xfF", 255.0)
TEST_CASE("ari:/REAL64/0xfF.ff", 255.255)
TEST_CASE("ari:/REAL64/0xfF.ffp0", 255.255)
TEST_CASE("ari:/REAL64/0xfF.ffp+0", 255.255)
TEST_CASE("ari:/REAL64/0x1.b8p+6", 1.1e2)
TEST_CASE("ari:/REAL64/0x1p+6", 64)
TEST_CASE("ari:/REAL64/-3.40282347E+38", -3.40282347E+38)
TEST_CASE("ari:/REAL64/3.40282347E+38", 3.40282347e38)
void test_ari_text_decode_lit_typed_float64(const char *text, ari_real64 expect)
{
    ari_t ari = ARI_INIT_UNDEFINED;
    check_decode(&ari, text);
    TEST_ASSERT_FALSE(ari.is_ref);
    TEST_ASSERT_TRUE(ari.as_lit.has_ari_type);
    TEST_ASSERT_EQUAL_INT(ARI_TYPE_REAL64, ari.as_lit.ari_type);
    TEST_ASSERT_EQUAL_INT(ARI_PRIM_FLOAT64, ari.as_lit.prim_type);
    TEST_ASSERT_EQUAL(expect, ari.as_lit.value.as_float64);
    ari_deinit(&ari);
}

TEST_CASE("label", "label")
TEST_CASE("\"\"", NULL)
TEST_CASE("\"hi\"", "hi")
TEST_CASE("\"h%20i\"", "h i")
TEST_CASE("\"h%5c%22i\"", "h\"i")
void test_ari_text_decode_lit_prim_tstr(const char *text, const char *expect)
{
    ari_t ari = ARI_INIT_UNDEFINED;
    check_decode(&ari, text);
    TEST_ASSERT_FALSE(ari.is_ref);
    TEST_ASSERT_FALSE(ari.as_lit.has_ari_type);
    TEST_ASSERT_EQUAL_INT(ARI_PRIM_TSTR, ari.as_lit.prim_type);

    if (expect)
    {
        cace_data_t expect_data;
        cace_data_init_view(&expect_data, strlen(expect) + 1, (cace_data_ptr_t)expect);
        TEST_ASSERT_TRUE(ari.as_lit.value.as_data.owned);
        TEST_ASSERT_EQUAL_INT(expect_data.len, ari.as_lit.value.as_data.len);
        TEST_ASSERT_EQUAL_STRING(expect_data.ptr, ari.as_lit.value.as_data.ptr);
        cace_data_deinit(&expect_data);
    }
    else
    {
        TEST_ASSERT_FALSE(ari.as_lit.value.as_data.owned);
        TEST_ASSERT_EQUAL_INT(0, ari.as_lit.value.as_data.len);
        TEST_ASSERT_NULL(ari.as_lit.value.as_data.ptr);
    }
    ari_deinit(&ari);
}

TEST_CASE("ari:/TEXTSTR/label", "label", 6)
TEST_CASE("ari:/TEXTSTR/\"\"", NULL, 0)
TEST_CASE("ari:/TEXTSTR/\"hi\"", "hi", 3)
TEST_CASE("ari:/TEXTSTR/\"h%20i\"", "h i", 4)
TEST_CASE("ari:/TEXTSTR/\"h%5c%22i\"", "h\"i", 4)
TEST_CASE("ari:/TEXTSTR/%22h%5c%22i%22", "h\"i", 4)
TEST_CASE("ari:/TEXTSTR/%22!@-+.:'%22", "!@-+.:'", 8)
TEST_CASE("ari:/TEXTSTR/%22%5C%22'%22", "\"'", 3)
TEST_CASE("ari:/TEXTSTR/%22''%22", "''", 3)
TEST_CASE("ari:/TEXTSTR/%22%5C''%22", "''", 3) // Silently drops \ for unknown 2-char escape seq
TEST_CASE("ari:/TEXTSTR/%22a%5Cu0000test%22", "atest", 6)
void test_ari_text_decode_lit_typed_tstr(const char *text, const char *expect, int expect_len)
{
    ari_t ari = ARI_INIT_UNDEFINED;
    check_decode(&ari, text);
    TEST_ASSERT_FALSE(ari.is_ref);
    TEST_ASSERT_TRUE(ari.as_lit.has_ari_type);
    TEST_ASSERT_EQUAL_INT(ARI_PRIM_TSTR, ari.as_lit.prim_type);

    if (expect)
    {
        cace_data_t expect_data;
        cace_data_init_view(&expect_data, strlen(expect) + 1, (cace_data_ptr_t)expect);
        TEST_ASSERT_TRUE(ari.as_lit.value.as_data.owned);
        TEST_ASSERT_EQUAL_INT(expect_data.len, ari.as_lit.value.as_data.len);
        TEST_ASSERT_EQUAL_INT(expect_len, ari.as_lit.value.as_data.len);
        TEST_ASSERT_EQUAL_STRING(expect_data.ptr, ari.as_lit.value.as_data.ptr);
        cace_data_deinit(&expect_data);
    }
    else
    {
        TEST_ASSERT_FALSE(ari.as_lit.value.as_data.owned);
        TEST_ASSERT_EQUAL_INT(0, ari.as_lit.value.as_data.len);
        TEST_ASSERT_NULL(ari.as_lit.value.as_data.ptr);
    }
    ari_deinit(&ari);
}

TEST_CASE("''", NULL, 0)
TEST_CASE("'hi'", "hi", 2)
TEST_CASE("'hi%20there'", "hi there", 8)
TEST_CASE("'h%5C'i'", "h'i", 3)
TEST_CASE("h'6869'", "hi", 2)
TEST_CASE("ari:h'5C0069'", "\\\0i", 3)
// examples from Section 10 of RFC 4648
TEST_CASE("ari:h'666F6F626172'", "foobar", 6)
TEST_CASE("ari:b64'Zm9vYmFy'", "foobar", 6)
TEST_CASE("ari:b64'Zg%3d%3d'", "f", 1)
// ignoring spaces
TEST_CASE("ari:h'%20666%20F6F626172'", "foobar", 6)
TEST_CASE("ari:b64'Zm9v%20YmFy'", "foobar", 6)
void test_ari_text_decode_lit_prim_bstr(const char *text, const char *expect, size_t expect_len)
{
    ari_t ari = ARI_INIT_UNDEFINED;
    check_decode(&ari, text);
    TEST_ASSERT_FALSE(ari.is_ref);
    TEST_ASSERT_FALSE(ari.as_lit.has_ari_type);
    TEST_ASSERT_EQUAL_INT(ARI_PRIM_BSTR, ari.as_lit.prim_type);

    if (expect)
    {
        cace_data_t expect_data;
        cace_data_init_view(&expect_data, expect_len, (cace_data_ptr_t)expect);
        TEST_ASSERT_TRUE(ari.as_lit.value.as_data.owned);
        TEST_ASSERT_EQUAL_INT(expect_data.len, ari.as_lit.value.as_data.len);
        TEST_ASSERT_EQUAL_MEMORY(expect_data.ptr, ari.as_lit.value.as_data.ptr, ari.as_lit.value.as_data.len);
        cace_data_deinit(&expect_data);
    }
    else
    {
        TEST_ASSERT_FALSE(ari.as_lit.value.as_data.owned);
        TEST_ASSERT_EQUAL_INT(0, ari.as_lit.value.as_data.len);
        TEST_ASSERT_NULL(ari.as_lit.value.as_data.ptr);
    }
    ari_deinit(&ari);
}

TEST_CASE("ari:/CBOR/h''", "")
TEST_CASE("ari:/CBOR/h'A164746573748203F94480'", "A164746573748203F94480")
TEST_CASE("ari:/CBOR/h'0064746573748203F94480'", "0064746573748203F94480")
TEST_CASE("ari:/CBOR/h'A1%2064%2074%2065%2073%2074%2082%2003%20F9%2044%20%2080'", "A164746573748203F94480")
void test_ari_text_decode_lit_typed_cbor(const char *text, const char *expect_hex)
{
    string_t expect_text;
    string_init_set_str(expect_text, expect_hex);
    cace_data_t expect_data;
    cace_data_init(&expect_data);
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, base16_decode(&expect_data, expect_text), "base16_decode() failed");

    ari_t ari = ARI_INIT_UNDEFINED;
    check_decode(&ari, text);

    TEST_ASSERT_FALSE(ari.is_ref);
    TEST_ASSERT_EQUAL_INT(ARI_TYPE_CBOR, ari.as_lit.ari_type);
    TEST_ASSERT_EQUAL_INT(ARI_PRIM_BSTR, ari.as_lit.prim_type);

    if (expect_data.len > 0)
    {
        TEST_ASSERT_TRUE(ari.as_lit.value.as_data.owned);
        TEST_ASSERT_EQUAL_INT(expect_data.len, ari.as_lit.value.as_data.len);
        TEST_ASSERT_EQUAL_MEMORY(expect_data.ptr, ari.as_lit.value.as_data.ptr, ari.as_lit.value.as_data.len);
    }
    else
    {
        TEST_ASSERT_FALSE(ari.as_lit.value.as_data.owned);
        TEST_ASSERT_EQUAL_INT(0, ari.as_lit.value.as_data.len);
        TEST_ASSERT_NULL(ari.as_lit.value.as_data.ptr);
    }
    ari_deinit(&ari);
    cace_data_deinit(&expect_data);
    string_clear(expect_text);
}

TEST_CASE("ari:/NULL/null")
TEST_CASE("ari:/0/null")
void test_ari_text_decode_lit_typed_null(const char *text)
{
    ari_t ari = ARI_INIT_UNDEFINED;
    check_decode(&ari, text);
    TEST_ASSERT_FALSE(ari.is_ref);
    TEST_ASSERT_TRUE(ari.as_lit.has_ari_type);
    TEST_ASSERT_EQUAL_INT(ARI_TYPE_NULL, ari.as_lit.ari_type);
    TEST_ASSERT_EQUAL_INT(ARI_PRIM_NULL, ari.as_lit.prim_type);
    ari_deinit(&ari);
}

TEST_CASE("ari:/BOOL/false", false)
TEST_CASE("ari:/BOOL/true", true)
TEST_CASE("ari:/1/true", true)
void test_ari_text_decode_lit_typed_bool(const char *text, ari_bool expect)
{
    ari_t ari = ARI_INIT_UNDEFINED;
    check_decode(&ari, text);
    TEST_ASSERT_FALSE(ari.is_ref);
    TEST_ASSERT_TRUE(ari.as_lit.has_ari_type);
    TEST_ASSERT_EQUAL_INT(ARI_TYPE_BOOL, ari.as_lit.ari_type);
    TEST_ASSERT_EQUAL_INT(ARI_PRIM_BOOL, ari.as_lit.prim_type);
    TEST_ASSERT_EQUAL(expect, ari.as_lit.value.as_bool);
    ari_deinit(&ari);
}

TEST_CASE("ari:/TP/2000-01-01T00:00:20Z", 20, 0)
TEST_CASE("ari:/TP/20000101T000020Z", 20, 0)
TEST_CASE("ari:/TP/20000101T000020.5Z", 20, 500e6)
TEST_CASE("ari:/TP/20.5", 20, 500e6)
TEST_CASE("ari:/TP/20.500", 20, 500e6)
TEST_CASE("ari:/TP/20.000001", 20, 1e3)
TEST_CASE("ari:/TP/20.000000001", 20, 1)
void test_ari_text_decode_lit_typed_tp(const char *text, time_t expect_sec, long expect_nsec)
{
    ari_t ari = ARI_INIT_UNDEFINED;
    check_decode(&ari, text);
    TEST_ASSERT_FALSE(ari.is_ref);
    TEST_ASSERT_TRUE(ari.as_lit.has_ari_type);
    TEST_ASSERT_EQUAL_INT(ARI_TYPE_TP, ari.as_lit.ari_type);
    TEST_ASSERT_EQUAL_INT(ARI_PRIM_TIMESPEC, ari.as_lit.prim_type);

    struct timespec expect = { .tv_sec = expect_sec, .tv_nsec = expect_nsec };
    TEST_ASSERT_EQUAL(expect.tv_sec, ari.as_lit.value.as_timespec.tv_sec);
    TEST_ASSERT_EQUAL(expect.tv_nsec, ari.as_lit.value.as_timespec.tv_nsec);

    ari_deinit(&ari);
}

TEST_CASE("ari:/TD/PT1M", 60, 0)
TEST_CASE("ari:/TD/PT20S", 20, 0)
TEST_CASE("ari:/TD/PT20.5S", 20, 500e6)
TEST_CASE("ari:/TD/20.5", 20, 500e6)
TEST_CASE("ari:/TD/20.500", 20, 500e6)
TEST_CASE("ari:/TD/20.000001", 20, 1e3)
TEST_CASE("ari:/TD/20.000000001", 20, 1)
TEST_CASE("ari:/TD/+PT1M", 60, 0)
TEST_CASE("ari:/TD/-PT1M", -60, 0)
TEST_CASE("ari:/TD/-P1DT", -(24 * 60 * 60), 0)
TEST_CASE("ari:/TD/PT", 0, 0)
void test_ari_text_decode_lit_typed_td(const char *text, time_t expect_sec, long expect_nsec)
{
    ari_t ari = ARI_INIT_UNDEFINED;
    check_decode(&ari, text);
    TEST_ASSERT_FALSE(ari.is_ref);
    TEST_ASSERT_TRUE(ari.as_lit.has_ari_type);
    TEST_ASSERT_EQUAL_INT(ARI_TYPE_TD, ari.as_lit.ari_type);
    TEST_ASSERT_EQUAL_INT(ARI_PRIM_TIMESPEC, ari.as_lit.prim_type);

    struct timespec expect = { .tv_sec = expect_sec, .tv_nsec = expect_nsec };
    TEST_ASSERT_EQUAL(expect.tv_sec, ari.as_lit.value.as_timespec.tv_sec);
    TEST_ASSERT_EQUAL(expect.tv_nsec, ari.as_lit.value.as_timespec.tv_nsec);

    ari_deinit(&ari);
}

TEST_CASE("ari:/AC/()", 0, ARI_TYPE_NULL, ARI_PRIM_NULL)
TEST_CASE("ari:/AC/(23)", 1, ARI_TYPE_NULL, ARI_PRIM_INT64)
TEST_CASE("ari:/AC/(/INT/23)", 1, ARI_TYPE_INT, ARI_PRIM_INT64)
TEST_CASE("ari:/AC/(\"hi%2C%20there%21\")", 1, ARI_TYPE_NULL, ARI_PRIM_TSTR)
void test_ari_text_decode_lit_typed_ac(const char *text, size_t expect_count, int expect_ari_type1,
                                       int expect_prim_type1)
{
    ari_t ari = ARI_INIT_UNDEFINED;
    check_decode(&ari, text);
    TEST_ASSERT_FALSE(ari.is_ref);
    TEST_ASSERT_TRUE(ari.as_lit.has_ari_type);
    TEST_ASSERT_EQUAL_INT(ARI_TYPE_AC, ari.as_lit.ari_type);
    TEST_ASSERT_EQUAL_INT(ARI_PRIM_OTHER, ari.as_lit.prim_type);

    TEST_ASSERT_EQUAL(ari_list_size(ari.as_lit.value.as_ac->items), expect_count);

    if (expect_count > 0)
    {
        ari_t *a = ari_list_get(ari.as_lit.value.as_ac->items, 0);
        TEST_ASSERT_EQUAL(expect_ari_type1, a->as_lit.ari_type);
        TEST_ASSERT_EQUAL(expect_prim_type1, a->as_lit.prim_type);
    }

    ari_deinit(&ari);
}

TEST_CASE("ari:/AM/()", 0)
TEST_CASE("ari:/AM/(undefined=1,undefined=/INT/2,1=a)", 2)
TEST_CASE("ari:/AM/(a=/AM/(),b=/AM/(),c=/AM/())", 3)
void test_ari_text_decode_lit_typed_am(const char *text, size_t expect_count)
{
    ari_t ari = ARI_INIT_UNDEFINED;
    check_decode(&ari, text);
    TEST_ASSERT_FALSE(ari.is_ref);
    TEST_ASSERT_TRUE(ari.as_lit.has_ari_type);
    TEST_ASSERT_EQUAL_INT(ARI_TYPE_AM, ari.as_lit.ari_type);
    TEST_ASSERT_EQUAL_INT(ARI_PRIM_OTHER, ari.as_lit.prim_type);

    TEST_ASSERT_EQUAL(ari_tree_size(ari.as_lit.value.as_am->items), expect_count);

    ari_deinit(&ari);
}

TEST_CASE("ari:/TBL/c=0;()()()", 0, 0)
TEST_CASE("ari:/TBL/c=2;(1,2)", 2, 2)
TEST_CASE("ari:/TBL/c=003;(1,2,3)(4,5,6)", 3, 6)
TEST_CASE("ari:/TBL/C=1;(/INT/4)(/TBL/c=0;)(\"%20\")", 1, 3)
void test_ari_text_decode_lit_typed_tbl(const char *text, size_t expect_cols, size_t expect_items)
{
    ari_t ari = ARI_INIT_UNDEFINED;
    check_decode(&ari, text);
    TEST_ASSERT_FALSE(ari.is_ref);
    TEST_ASSERT_TRUE(ari.as_lit.has_ari_type);
    TEST_ASSERT_EQUAL_INT(ARI_TYPE_TBL, ari.as_lit.ari_type);
    TEST_ASSERT_EQUAL_INT(ARI_PRIM_OTHER, ari.as_lit.prim_type);

    TEST_ASSERT_EQUAL(ari.as_lit.value.as_tbl->ncols, expect_cols);
    TEST_ASSERT_EQUAL(ari_array_size(ari.as_lit.value.as_tbl->items), expect_items);

    ari_deinit(&ari);
}

TEST_CASE("ari:/EXECSET/n=null;()", ARI_PRIM_NULL, 0)
TEST_CASE("ari:/EXECSET/N=null;()", ARI_PRIM_NULL, 0)
TEST_CASE("ari:/EXECSET/N=0xabcd;()", ARI_PRIM_INT64, 0)
TEST_CASE("ari:/EXECSET/N=/UINT/0B0101;()", ARI_PRIM_INT64, 0)
TEST_CASE("ari:/EXECSET/n=1234;(//test/CTRL/hi)", ARI_PRIM_INT64, 1)
TEST_CASE("ari:/EXECSET/n=h'6869';(//test/CTRL/hi,//test/CTRL/eh)", ARI_PRIM_BSTR, 2)
void test_ari_text_decode_lit_typed_execset(const char *text, enum ari_prim_type_e expect_n, size_t expect_count)
{
    ari_t ari = ARI_INIT_UNDEFINED;
    check_decode(&ari, text);
    TEST_ASSERT_FALSE(ari.is_ref);
    TEST_ASSERT_TRUE(ari.as_lit.has_ari_type);
    TEST_ASSERT_EQUAL_INT(ARI_TYPE_EXECSET, ari.as_lit.ari_type);
    TEST_ASSERT_EQUAL_INT(ARI_PRIM_OTHER, ari.as_lit.prim_type);

    const ari_execset_t *ctr = ari.as_lit.value.as_execset;
    TEST_ASSERT_FALSE(ctr->nonce.is_ref);
    TEST_ASSERT_EQUAL_INT(expect_n, ctr->nonce.as_lit.prim_type);
    TEST_ASSERT_EQUAL_INT(expect_count, ari_list_size(ctr->targets));

    ari_deinit(&ari);
}

TEST_CASE("ari:/RPTSET/n=null;r=725943845;", ARI_PRIM_NULL, 0)
TEST_CASE("ari:/RPTSET/n=1234;r=725943845;(t=0;s=//test/CTRL/hi;())", ARI_PRIM_INT64, 1)
TEST_CASE("ari:/RPTSET/n=1234;r=725943845;(t=0.0;s=//test/CTRL/hi;())", ARI_PRIM_INT64, 1)
TEST_CASE("ari:/RPTSET/n=1234;r=/TP/725943845;(t=/TD/0;s=//test/CTRL/hi;())", ARI_PRIM_INT64, 1)
TEST_CASE("ari:/RPTSET/n=1234;r=/TP/725943845.000;(t=/TD/0;s=//test/CTRL/hi;())", ARI_PRIM_INT64, 1)
TEST_CASE("ari:/RPTSET/n=1234;r=/TP/20230102T030405Z;(t=/TD/0;s=//test/CTRL/hi;())", ARI_PRIM_INT64, 1)
TEST_CASE("ari:/RPTSET/n=h'6869';r=/TP/725943845;(t=/TD/0;s=//test/CTRL/hi;())(t=/TD/1;s=//test/CTRL/eh;())",
          ARI_PRIM_BSTR, 2)
void test_ari_text_decode_lit_typed_rptset(const char *text, enum ari_prim_type_e expect_n, size_t expect_count)
{
    ari_t ari = ARI_INIT_UNDEFINED;
    check_decode(&ari, text);
    TEST_ASSERT_FALSE(ari.is_ref);
    TEST_ASSERT_TRUE(ari.as_lit.has_ari_type);
    TEST_ASSERT_EQUAL_INT(ARI_TYPE_RPTSET, ari.as_lit.ari_type);
    TEST_ASSERT_EQUAL_INT(ARI_PRIM_OTHER, ari.as_lit.prim_type);

    const ari_rptset_t *ctr = ari.as_lit.value.as_rptset;

    TEST_ASSERT_FALSE(ctr->nonce.is_ref);
    TEST_ASSERT_EQUAL_INT(expect_n, ctr->nonce.as_lit.prim_type);

    TEST_ASSERT_FALSE(ctr->reftime.is_ref);
    switch (ctr->reftime.as_lit.prim_type)
    {
        case ARI_PRIM_INT64:
            TEST_ASSERT_EQUAL_INT(725943845, ctr->reftime.as_lit.value.as_int64);
            break;
        case ARI_PRIM_TIMESPEC:
            TEST_ASSERT_EQUAL_INT(725943845, ctr->reftime.as_lit.value.as_timespec.tv_sec);
            TEST_ASSERT_EQUAL_INT(0, ctr->reftime.as_lit.value.as_timespec.tv_nsec);
            break;
        default:
            TEST_FAIL_MESSAGE("reftime has invalid type");
            break;
    }

    TEST_ASSERT_EQUAL_INT(expect_count, ari_report_list_size(ctr->reports));

    ari_deinit(&ari);
}

TEST_CASE("ari://test/const/hi", ARI_TYPE_CONST)
TEST_CASE("ari://test/ctrl/hi", ARI_TYPE_CTRL)
TEST_CASE("ari://test/IDENT/hi", ARI_TYPE_IDENT)
TEST_CASE("ari://test/TYPEDEF/hi", ARI_TYPE_TYPEDEF)
TEST_CASE("ari://test/CONST/hi", ARI_TYPE_CONST)
TEST_CASE("ari://test/VAR/hi", ARI_TYPE_VAR)
TEST_CASE("ari://test/EDD/hi", ARI_TYPE_EDD)
TEST_CASE("ari://test/CTRL/hi", ARI_TYPE_CTRL)
TEST_CASE("ari://test/OPER/hi", ARI_TYPE_OPER)
TEST_CASE("ari://test/SBR/hi", ARI_TYPE_SBR)
TEST_CASE("ari://test/TBR/hi", ARI_TYPE_TBR)
TEST_CASE("ari://test/ident/hi", ARI_TYPE_IDENT)
TEST_CASE("ari://test/typedef/hi", ARI_TYPE_TYPEDEF)
TEST_CASE("ari://test/const/hi", ARI_TYPE_CONST)
TEST_CASE("ari://test/var/hi", ARI_TYPE_VAR)
TEST_CASE("ari://test/edd/hi", ARI_TYPE_EDD)
TEST_CASE("ari://test/ctrl/hi", ARI_TYPE_CTRL)
TEST_CASE("ari://test/CtRl/hi", ARI_TYPE_CTRL)
TEST_CASE("ari://test/oper/hi", ARI_TYPE_OPER)
TEST_CASE("ari://test/sbr/hi", ARI_TYPE_SBR)
TEST_CASE("ari://test/tbr/hi", ARI_TYPE_TBR)
TEST_CASE("ari://adm/const/hi", ARI_TYPE_CONST)
TEST_CASE("ari://adm/CONST/hi", ARI_TYPE_CONST)
TEST_CASE("ari://adm/-2/hi", ARI_TYPE_CONST)
void test_ari_text_decode_objref(const char *text, ari_type_t expect_type)
{
    ari_t ari = ARI_INIT_UNDEFINED;
    check_decode(&ari, text);
    TEST_ASSERT_TRUE(ari.is_ref);
    TEST_ASSERT_TRUE(ari.as_ref.objpath.has_ari_type);
    TEST_ASSERT_EQUAL_INT(expect_type, ari.as_ref.objpath.ari_type);

    TEST_ASSERT_NOT_EQUAL_INT(ARI_IDSEG_NULL, ari.as_ref.objpath.ns_id.form);
    TEST_ASSERT_NOT_EQUAL_INT(ARI_IDSEG_NULL, ari.as_ref.objpath.type_id.form);
    TEST_ASSERT_NOT_EQUAL_INT(ARI_IDSEG_NULL, ari.as_ref.objpath.obj_id.form);

    ari_deinit(&ari);
}

TEST_CASE("ari://test/LITERAL/hi")
TEST_CASE("ari://test/NULL/hi")
TEST_CASE("ari://test/BOOL/hi")
TEST_CASE("ari://test/BYTE/hi")
TEST_CASE("ari://test/INT/hi")
TEST_CASE("ari://test/UINT/hi")
TEST_CASE("ari://test/VAST/hi")
TEST_CASE("ari://test/UVAST/hi")
TEST_CASE("ari://test/REAL32/hi")
TEST_CASE("ari://test/REAL64/hi")
TEST_CASE("ari://test/TEXTSTR/hi")
TEST_CASE("ari://test/BYTESTR/hi")
TEST_CASE("ari://test/TP/hi")
TEST_CASE("ari://test/TD/hi")
TEST_CASE("ari://test/LABEL/hi")
TEST_CASE("ari://test/CBOR/hi")
TEST_CASE("ari://test/ARITYPE/hi")
TEST_CASE("ari://test/AC/hi")
TEST_CASE("ari://test/AM/hi")
TEST_CASE("ari://test/TBL/hi")
TEST_CASE("ari://test/EXECSET/hi")
TEST_CASE("ari://test/RPTSET/hi")
TEST_CASE("ari://test/OBJECT/hi")
TEST_CASE("ari://test/literal/hi")
TEST_CASE("ari://test/null/hi")
TEST_CASE("ari://test/bool/hi")
TEST_CASE("ari://test/byte/hi")
TEST_CASE("ari://test/int/hi")
TEST_CASE("ari://test/uint/hi")
TEST_CASE("ari://test/vast/hi")
TEST_CASE("ari://test/uvast/hi")
TEST_CASE("ari://test/real32/hi")
TEST_CASE("ari://test/real64/hi")
TEST_CASE("ari://test/textstr/hi")
TEST_CASE("ari://test/bytestr/hi")
TEST_CASE("ari://test/tp/hi")
TEST_CASE("ari://test/td/hi")
TEST_CASE("ari://test/label/hi")
TEST_CASE("ari://test/cbor/hi")
TEST_CASE("ari://test/aritype/hi")
TEST_CASE("ari://test/ac/hi")
TEST_CASE("ari://test/am/hi")
TEST_CASE("ari://test/tbl/hi")
TEST_CASE("ari://test/execset/hi")
TEST_CASE("ari://test/rptset/hi")
TEST_CASE("ari://test/object/hi")
void test_ari_text_decode_objref_invalid(const char *intext)
{
    ari_t    ari = ARI_INIT_UNDEFINED;
    string_t inbuf;
    string_init_set_str(inbuf, intext);
    int ret = ari_text_decode(&ari, inbuf, &errm);
    string_clear(inbuf);
    ari_deinit(&ari);

    TEST_ASSERT_NOT_EQUAL_INT(0, ret);
    TEST_ASSERT_NOT_NULL_MESSAGE(errm, "decode failure must provide a message");
    TEST_MESSAGE(errm);
}

TEST_CASE("ari://adm")
TEST_CASE("ari://adm/")
TEST_CASE("ari://18")
TEST_CASE("ari://18/")
TEST_CASE("ari://65536/")
TEST_CASE("ari://-20/")
TEST_CASE("ari://example-adm-a@2024-06-25/")
TEST_CASE("ari://example-adm-a/")
TEST_CASE("ari://!example-odm-b/")
void test_ari_text_decode_nsref(const char *text)
{
    ari_t ari = ARI_INIT_UNDEFINED;
    check_decode(&ari, text);
    TEST_ASSERT_TRUE(ari.is_ref);
    TEST_ASSERT_FALSE(ari.as_ref.objpath.has_ari_type);
    TEST_ASSERT_NOT_EQUAL_INT(ARI_IDSEG_NULL, ari.as_ref.objpath.ns_id.form);
    TEST_ASSERT_EQUAL_INT(ARI_IDSEG_NULL, ari.as_ref.objpath.type_id.form);
    TEST_ASSERT_EQUAL_INT(ARI_IDSEG_NULL, ari.as_ref.objpath.obj_id.form);

    ari_deinit(&ari);
}

TEST_CASE("ari:./CTRL/do_thing", ARI_TYPE_CTRL)
TEST_CASE("ari:./CTRL/otherobj(%22a%20param%22,/UINT/10)", ARI_TYPE_CTRL)
TEST_CASE("ari:./-2/30", ARI_TYPE_CONST)
TEST_CASE("./CTRL/do_thing", ARI_TYPE_CTRL)
TEST_CASE("./CTRL/otherobj(%22a%20param%22,/UINT/10)", ARI_TYPE_CTRL)
TEST_CASE("./-2/30", ARI_TYPE_CONST)
void test_ari_text_decode_ariref(const char *text, ari_type_t expect_type)
{
    ari_t ari = ARI_INIT_UNDEFINED;
    check_decode(&ari, text);
    TEST_ASSERT_TRUE(ari.is_ref);
    TEST_ASSERT_EQUAL_INT(expect_type, ari.as_ref.objpath.ari_type);
    TEST_ASSERT_EQUAL_INT(ARI_IDSEG_NULL, ari.as_ref.objpath.ns_id.form);
    TEST_ASSERT_NOT_EQUAL_INT(ARI_IDSEG_NULL, ari.as_ref.objpath.type_id.form);
    TEST_ASSERT_NOT_EQUAL_INT(ARI_IDSEG_NULL, ari.as_ref.objpath.obj_id.form);

    ari_deinit(&ari);
}

TEST_CASE("ari:undefined")
TEST_CASE("ari:null")
TEST_CASE("ari:true")
TEST_CASE("ari:false")
TEST_CASE("ari:1234")
TEST_CASE("ari:hi")
TEST_CASE("ari:%22hi%20there%22")
TEST_CASE("ari:h'6869'")
TEST_CASE("ari:/NULL/null")
TEST_CASE("ari:/BOOL/false")
TEST_CASE("ari:/BOOL/true")
TEST_CASE("ari:/INT/10")
TEST_CASE("ari:/INT/-10")
TEST_CASE("ari:/REAL32/10")
TEST_CASE("ari:/REAL32/10.1")
TEST_CASE("ari:/REAL32/0.1")
TEST_CASE("ari:/REAL32/NaN")
TEST_CASE("ari:/REAL64/+Infinity")
TEST_CASE("ari:/REAL64/-Infinity")
TEST_CASE("ari:/BYTESTR/h'6869'")
TEST_CASE("ari:/TEXTSTR/hi")
TEST_CASE("ari:/TEXTSTR/%22hi%20there%22")
TEST_CASE("ari:/LABEL/hi")
TEST_CASE("ari:/TP/20230102T030405Z")
TEST_CASE("ari:/AC/()")
TEST_CASE("ari:/AC/(a)")
TEST_CASE("ari:/AC/(a,b,c)")
TEST_CASE("ari:/AC/(null,/INT/23)")
TEST_CASE("ari:/AC/(null,/AC/(undefined,/INT/23,/AC/()))")
TEST_CASE("ari:/AM/()")
TEST_CASE("ari:/AM/(1=true)")
TEST_CASE("ari:/AM/(3=true,10=hi,oh=4)") // AM key ordering
TEST_CASE("ari:/TBL/c=3;(1,2,3)")
TEST_CASE("ari:/TBL/c=3;(1,2,3)(4,5,6)")
TEST_CASE("ari:/TBL/c=0;")
TEST_CASE("ari:/TBL/c=1;")
TEST_CASE("ari:/EXECSET/n=null;()")
TEST_CASE("ari:/EXECSET/n=1234;(//test/CTRL/hi)")
TEST_CASE("ari:/EXECSET/n=h'6869';(//test/CTRL/hi,//test/CTRL/eh)")
TEST_CASE("ari:/RPTSET/n=1234;r=/TP/20000101T001640Z;(t=/TD/PT0S;s=//test/CTRL/hi;(null,3,h'6869'))")
TEST_CASE("ari:/RPTSET/n=1234;r=/TP/20230102T030405Z;(t=/TD/PT0S;s=//test/CTRL/hi;(null,3,h'6869'))")
TEST_CASE("ari://test/CONST/that")      // ADM path
TEST_CASE("ari://test@1234/CONST/that") // ADM revision
TEST_CASE("ari://!test/CONST/that")     // ODM path
TEST_CASE("ari://test/CTRL/that(34)")
TEST_CASE("ari://2/CTRL/4(hi)")
TEST_CASE("./CTRL/do_thing")
TEST_CASE("ari:/CBOR/h'0A'")
TEST_CASE("ari:/CBOR/h'A164746573748203F94480'")
void test_ari_text_loopback(const char *intext)
{
    ari_t    ari = ARI_INIT_UNDEFINED;
    string_t inbuf;
    string_init_set_str(inbuf, intext);
    int ret = ari_text_decode(&ari, inbuf, &errm);
    string_clear(inbuf);
    if (ret && errm)
    {
        TEST_FAIL_MESSAGE(errm);
    }
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, ret, "ari_text_decode() failed");

    string_t outtext;
    string_init(outtext);
    ret = ari_text_encode(outtext, &ari, ARI_TEXT_ENC_OPTS_DEFAULT);
    ari_deinit(&ari);
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, ret, "ari_text_encode() failed");

    TEST_ASSERT_EQUAL_STRING(intext, string_get_cstr(outtext));
    string_clear(outtext);
}

TEST_CASE("ari:/null/null", "ari:/NULL/null")
TEST_CASE("ari:/bool/false", "ari:/BOOL/false")
TEST_CASE("ari:/int/10", "ari:/INT/10")
TEST_CASE("ari:/uint/10", "ari:/UINT/10")
TEST_CASE("ari:/vast/10", "ari:/VAST/10")
TEST_CASE("ari:/uvast/10", "ari:/UVAST/10")
TEST_CASE("ari:/real32/10", "ari:/REAL32/10")
TEST_CASE("ari:/real64/+Infinity", "ari:/REAL64/+Infinity")
TEST_CASE("ari:/bytestr/h'6869'", "ari:/BYTESTR/h'6869'")
TEST_CASE("ari:/textstr/hi", "ari:/TEXTSTR/hi")
TEST_CASE("ari:/label/hi", "ari:/LABEL/hi")
TEST_CASE("ari:/tp/20230102T030405Z", "ari:/TP/20230102T030405Z")
TEST_CASE("ari:/ac/()", "ari:/AC/()")
TEST_CASE("ari:/am/()", "ari:/AM/()")
TEST_CASE("ari:/tbl/c=3;(1,2,3)", "ari:/TBL/c=3;(1,2,3)")
TEST_CASE("ari:/execset/n=null;()", "ari:/EXECSET/n=null;()")
TEST_CASE("ari:/rptset/n=1234;r=1000;(t=0;s=//test/ctrl/hi;(null,3,h'6869'))",
          "ari:/RPTSET/n=1234;r=/TP/20000101T001640Z;(t=/TD/PT0S;s=//test/CTRL/hi;(null,3,h'6869'))")
TEST_CASE("ari:/rptset/n=1234;r=/TP/1000;(t=/TD/0;s=//test/CTRL/hi;(null,3,h'6869'))",
          "ari:/RPTSET/n=1234;r=/TP/20000101T001640Z;(t=/TD/PT0S;s=//test/CTRL/hi;(null,3,h'6869'))")
TEST_CASE("ari:/rptset/n=1234;r=/TP/1000;(t=0;s=//test/CTRL/hi;(null,3,h'6869'))",
          "ari:/RPTSET/n=1234;r=/TP/20000101T001640Z;(t=/TD/PT0S;s=//test/CTRL/hi;(null,3,h'6869'))")
TEST_CASE("ari:/rptset/n=1234;r=/TP/1000;(t=100.5;s=//test/CTRL/hi;(null,3,h'6869'))",
          "ari:/RPTSET/n=1234;r=/TP/20000101T001640Z;(t=/TD/PT1M40.5S;s=//test/CTRL/hi;(null,3,h'6869'))")
TEST_CASE("ari:/rptset/n=1234;r=1000;(t=/TD/0;s=//test/CTRL/hi;(null,3,h'6869'))",
          "ari:/RPTSET/n=1234;r=/TP/20000101T001640Z;(t=/TD/PT0S;s=//test/CTRL/hi;(null,3,h'6869'))")
TEST_CASE("ari:/rptset/n=1234;r=1000.0;(t=/TD/0;s=//test/CTRL/hi;(null,3,h'6869'))",
          "ari:/RPTSET/n=1234;r=/TP/20000101T001640Z;(t=/TD/PT0S;s=//test/CTRL/hi;(null,3,h'6869'))")
TEST_CASE("ari:/rptset/n=1234;r=/UVAST/1000;(t=/TD/0;s=//test/CTRL/hi;(null,3,h'6869'))",
          "ari:/RPTSET/n=1234;r=/TP/20000101T001640Z;(t=/TD/PT0S;s=//test/CTRL/hi;(null,3,h'6869'))")
TEST_CASE("ari:/rptset/n=1234;r=/UVAST/0b1000;(t=/TD/0;s=//test/CTRL/hi;(null,3,h'6869'))",
          "ari:/RPTSET/n=1234;r=/TP/20000101T000008Z;(t=/TD/PT0S;s=//test/CTRL/hi;(null,3,h'6869'))")
TEST_CASE("ari:/rptset/n=1234;r=/TP/1000.987654321;(t=/TD/0;s=//test/CTRL/hi;(null,3,h'6869'))",
          "ari:/RPTSET/n=1234;r=/TP/20000101T001640.987654321Z;(t=/TD/PT0S;s=//test/CTRL/hi;(null,3,h'6869'))")
TEST_CASE("ari:/rptset/n=1234;r=1000.9876543210987654321;(t=/TD/0;s=//test/CTRL/hi;(null,3,h'6869'))",
          "ari:/RPTSET/n=1234;r=/TP/20000101T001640.987654321Z;(t=/TD/PT0S;s=//test/CTRL/hi;(null,3,h'6869'))")
TEST_CASE("ari://test", "ari://test/")  // always trailing slash
TEST_CASE("ari:./ctrl/hi", "./CTRL/hi") // scheme elided
void test_ari_text_reencode(const char *intext, const char *expect_outtext)
{
    ari_t    ari = ARI_INIT_UNDEFINED;
    string_t inbuf;
    string_init_set_str(inbuf, intext);
    int ret = ari_text_decode(&ari, inbuf, &errm);
    string_clear(inbuf);
    if (ret && errm)
    {
        TEST_FAIL_MESSAGE(errm);
    }
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, ret, "ari_text_decode() failed");

    string_t outtext;
    string_init(outtext);
    ret = ari_text_encode(outtext, &ari, ARI_TEXT_ENC_OPTS_DEFAULT);
    ari_deinit(&ari);
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, ret, "ari_text_encode() failed");

    TEST_ASSERT_EQUAL_STRING(expect_outtext, string_get_cstr(outtext));
    string_clear(outtext);
}

TEST_CASE("-0x8FFFFFFFFFFFFFFF")
TEST_CASE("-0x1FFFFFFFFFFFFFFFF")
TEST_CASE("ari:/OTHERNAME/0")
TEST_CASE("ari:/UNDEFINED/undefined")
TEST_CASE("ari:/NULL/fae")
TEST_CASE("ari:/NULL/undefined")
TEST_CASE("ari:/NULL/10")
TEST_CASE("ari:/BOOL/fae")
TEST_CASE("ari:/BOOL/3")
TEST_CASE("ari:/TEXTSTR/1")
TEST_CASE("ari:/BYTESTR/1")
TEST_CASE("ari:/AC/")
TEST_CASE("ari:/AC/(a,")
TEST_CASE("ari:/AC/(,,,)")
TEST_CASE("ari:/AM/")
TEST_CASE("ari:/TBL/")
TEST_CASE("ari:/TBL/c=hi;")
TEST_CASE("ari:/TBL/c=5;(1,2)")
TEST_CASE("ari:/TBL/(1,2,3)")
TEST_CASE("ari:/TBL/c=aaa;c=2;(1,2)")
TEST_CASE("ari:/TBL/c=2;c=2;(1,2)")
TEST_CASE("ari:/EXECSET/()")
TEST_CASE("ari:/EXECSET/g=null;()")
TEST_CASE("ari:/EXECSET/n=undefined;()")
TEST_CASE("ari:/EXECSET/n=1;")
TEST_CASE("ari:/EXECSET/n=1;n=2;()")
TEST_CASE("ari://./object/hi")
TEST_CASE("./object/hi")
void test_ari_text_decode_failure(const char *intext)
{
    ari_t    ari = ARI_INIT_UNDEFINED;
    string_t inbuf;
    string_init_set_str(inbuf, intext);
    int ret = ari_text_decode(&ari, inbuf, &errm);
    string_clear(inbuf);
    ari_deinit(&ari);

    TEST_ASSERT_NOT_EQUAL_INT(0, ret);
    TEST_ASSERT_NOT_NULL_MESSAGE(errm, "decode failure must provide a message");
    TEST_MESSAGE(errm);
}

TEST_CASE("ari:/BYTE/-1")
TEST_CASE("ari:/BYTE/256")
TEST_CASE("ari:/INT/-2147483649")
TEST_CASE("ari:/INT/2147483648")
TEST_CASE("ari:/UINT/-1")
TEST_CASE("ari:/UINT/4294967296")
TEST_CASE("ari:/VAST/0x8000000000000000")
TEST_CASE("ari:/UVAST/-1")
TEST_CASE("ari:/REAL32/-3.40282347E+38")
TEST_CASE("ari:/REAL32/3.40282347E+38")
TEST_CASE("ari:/AM/(/INT/10=true)") // no typed keys
void test_ari_text_decode_invalid(const char *intext)
{
    ari_t    ari = ARI_INIT_UNDEFINED;
    string_t inbuf;
    string_init_set_str(inbuf, intext);
    int ret = ari_text_decode(&ari, inbuf, &errm);
    string_clear(inbuf);
    if (ret && errm)
    {
        TEST_FAIL_MESSAGE(errm);
    }
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, ret, "ari_text_decode() failed");

    TEST_ASSERT_FALSE_MESSAGE(amm_builtin_validate(&ari), "amm_builtin_validate() succeeded");
    ari_deinit(&ari);
}

#endif /* ARI_TEXT_PARSE */
