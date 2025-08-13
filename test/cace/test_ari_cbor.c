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
/** @file
 * Test the cace_ari_cbor.h interfaces.
 *
 * As a shortcut to producing expected binary contents, use commands similar to:
 * @code{.sh}
 *  echo "42" | diag2cbor.rb | xxd -i
 * @endcode
 */
#include <cace/ari/cbor.h>
#include <cace/ari/text.h>
#include <cace/ari/text_util.h>
#include <cace/amm/typing.h>
#include <cace/util/logging.h>
#include <unity.h>

// Allow this macro
#define TEST_CASE(...)

void suiteSetUp(void)
{
    cace_openlog();
}

int suiteTearDown(int failures)
{
    cace_closelog();
    return failures;
}

/// Resource cleanup for failure messages
static char *errm = NULL;

void tearDown(void)
{
    if (errm)
    {
        CACE_FREE(errm);
        errm = NULL;
    }
}

static void check_encoding(const cace_ari_t *ari, const char *expect_hex)
{
    cace_data_t buf;
    cace_data_init(&buf);

    TEST_ASSERT_EQUAL_INT_MESSAGE(0, cace_ari_cbor_encode(&buf, ari), "cace_ari_cbor_encode() failed");

    string_t outhex;
    string_init(outhex);
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, cace_base16_encode(outhex, &buf, true), "cace_ari_cbor_encode() failed");

    TEST_ASSERT_EQUAL_STRING_MESSAGE(expect_hex, string_get_cstr(outhex), "Mismatch in encoded data");

    string_clear(outhex);
    cace_data_deinit(&buf);
}

void test_cace_ari_cbor_encode_lit_prim_undef(void)
{
    cace_ari_t ari = CACE_ARI_INIT_UNDEFINED;
    // no value is undefined

    check_encoding(&ari, "F7");

    cace_ari_deinit(&ari);
}
void test_cace_ari_cbor_encode_lit_prim_null(void)
{
    cace_ari_t ari = CACE_ARI_INIT_UNDEFINED;
    cace_ari_set_null(&ari);

    check_encoding(&ari, "F6");

    cace_ari_deinit(&ari);
}
void test_cace_ari_cbor_encode_lit_prim_uint(void)
{
    cace_ari_t ari = CACE_ARI_INIT_UNDEFINED;
    cace_ari_set_prim_uint64(&ari, 1234);

    check_encoding(&ari, "1904D2");

    cace_ari_deinit(&ari);
}

void test_cace_ari_cbor_encode_lit_prim_text_nocopy(void)
{
    cace_ari_t ari = CACE_ARI_INIT_UNDEFINED;
    cace_ari_set_tstr(&ari, "test", false);

    check_encoding(&ari, "6474657374");

    cace_ari_deinit(&ari);
}
void test_cace_ari_cbor_encode_lit_prim_text_copy(void)
{
    cace_ari_t ari = CACE_ARI_INIT_UNDEFINED;
    cace_ari_set_tstr(&ari, "test", true);

    check_encoding(&ari, "6474657374");

    cace_ari_deinit(&ari);
}

void test_cace_ari_cbor_encode_lit_typed_tp(void)
{
    const struct timespec delta = {
        .tv_sec = 30,
    };
    cace_ari_t ari = CACE_ARI_INIT_UNDEFINED;
    cace_ari_set_tp(&ari, delta);

    check_encoding(&ari, "820C181E");

    cace_ari_deinit(&ari);
}

void test_cace_ari_cbor_encode_lit_typed_td(void)
{
    const struct timespec delta = {
        .tv_sec = 30,
    };
    cace_ari_t ari = CACE_ARI_INIT_UNDEFINED;
    cace_ari_set_td(&ari, delta);

    check_encoding(&ari, "820D181E");

    cace_ari_deinit(&ari);
}

void test_cace_ari_cbor_encode_lit_typed_ac_empty(void)
{
    cace_ari_t ari = CACE_ARI_INIT_UNDEFINED;
    cace_ari_set_ac(&ari, NULL);

    check_encoding(&ari, "821180");

    cace_ari_deinit(&ari);
}

void test_cace_ari_cbor_encode_lit_typed_ac_1item(void)
{
    {
        cace_ari_ac_t acinit;
        cace_ari_ac_init(&acinit);
        {
            cace_ari_t *item = cace_ari_list_push_back_new(acinit.items);
            cace_ari_set_null(item);
        }

        cace_ari_t ari = CACE_ARI_INIT_UNDEFINED;
        cace_ari_set_ac(&ari, &acinit);

        check_encoding(&ari, "821181F6");

        cace_ari_deinit(&ari);
    }
}

TEST_CASE("example", "adm-a", "2024-06-25", false, 0, NULL,
          "85676578616D706C656561646D2D61D903EC6A323032342D30362D3235F6F6") // ari://example/adm-a@2024-06-25/
TEST_CASE("example", "adm-a", NULL, false, 0, NULL, "84676578616D706C656561646D2D61F6F6")    // ari://example/adm-a/
TEST_CASE("example", "!odm-b", NULL, false, 0, NULL, "84676578616D706C6566216F646D2D62F6F6") // ari://example/!odm-b/
TEST_CASE(NULL, NULL, NULL, true, CACE_ARI_TYPE_CONST, "hi", "84F6F621626869")               // "./CONST/hi
TEST_CASE("example", "adm", NULL, true, CACE_ARI_TYPE_CONST, "hi",
          "84676578616D706C656361646D21626869") // ari://example/adm/CONST/hi
TEST_CASE("example", "test", NULL, true, CACE_ARI_TYPE_CONST, "that",
          "84676578616D706C656474657374216474686174") // ari://example/test/CONST/that
TEST_CASE(
    "example", "test", "2024-06-25", true, CACE_ARI_TYPE_CONST, "that",
    "85676578616D706C656474657374D903EC6A323032342D30362D3235216474686174") // ari://example/test@2024-06-25/CONST/that
TEST_CASE("example", "!test", NULL, true, CACE_ARI_TYPE_CONST, "that",
          "84676578616D706C65652174657374216474686174") // ari://example/!test/CONST/that
void test_cace_ari_cbor_encode_objref_path_text(const char *org_id, const char *model_id, const char *model_rev,
                                                bool has_type, cace_ari_type_t type_id, const char *obj_id,
                                                const char *expect_hex)
{
    cace_ari_t ari = CACE_ARI_INIT_UNDEFINED;
    {
        cace_ari_objpath_t *path = &(cace_ari_set_objref(&ari)->objpath);
        cace_ari_objpath_set_textid_opt(path, org_id, model_id, has_type ? &type_id : NULL, obj_id);
        cace_ari_date_from_text(&path->model_rev, model_rev);
    }

    check_encoding(&ari, expect_hex);

    cace_ari_deinit(&ari);
}

TEST_CASE(true, 65535, true, 18, false, 0, false, 0, "8419FFFF12F6F6")                    // ari://65535/18/
TEST_CASE(true, 65535, true, -20, false, 0, false, 0, "8419FFFF33F6F6")                   // ari://65535/-20/
TEST_CASE(true, -15, true, 6, false, 0, false, 0, "842E06F6F6")                           // ari://-15/6/
TEST_CASE(false, 0, false, 0, true, CACE_ARI_TYPE_IDENT, true, 34, "84F6F6201822")        // ./IDENT/34
TEST_CASE(true, 65535, true, 18, true, CACE_ARI_TYPE_IDENT, true, 34, "8419FFFF12201822") // ari://65535/18/IDENT/34
void test_cace_ari_cbor_encode_objref_path_int(bool has_org, cace_ari_int_id_t org_id, bool has_model,
                                               cace_ari_int_id_t model_id, bool has_type, cace_ari_type_t type_id,
                                               bool has_obj, cace_ari_int_id_t obj_id, const char *expect_hex)
{
    cace_ari_t ari = CACE_ARI_INIT_UNDEFINED;
    cace_ari_objpath_set_intid_opt(&(cace_ari_set_objref(&ari)->objpath), has_org ? &org_id : NULL,
                                   has_model ? &model_id : NULL, has_type ? &type_id : NULL, has_obj ? &obj_id : NULL);

    check_encoding(&ari, expect_hex);

    cace_ari_deinit(&ari);
}

static void check_decoding(cace_ari_t *ari, const char *inhex)
{
    string_t intext;
    string_init_set_str(intext, inhex);
    cace_data_t indata;
    cace_data_init(&indata);
    TEST_ASSERT_EQUAL_INT(0, cace_base16_decode(&indata, intext));
    string_clear(intext);

    const size_t inlen = indata.len;
    size_t       used;
    int          res = cace_ari_cbor_decode(ari, &indata, &used, &errm);
    cace_data_deinit(&indata);
    if (res && errm)
    {
        TEST_FAIL_MESSAGE(errm);
    }
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, res, "cace_ari_cbor_decode() failed");
    TEST_ASSERT_EQUAL_INT_MESSAGE(inlen, used, "cace_ari_cbor_decode() did not use all data");
    {
        m_string_t debug;
        m_string_init(debug);
        cace_ari_text_encode(debug, ari, CACE_ARI_TEXT_ENC_OPTS_DEFAULT);
        TEST_PRINTF("decoded to %s", m_string_get_cstr(debug));
        m_string_clear(debug);
    }

    TEST_ASSERT_TRUE_MESSAGE(cace_amm_builtin_validate(ari), "cace_amm_builtin_validate() failed");
}

TEST_CASE("84676578616D706C656361646D21626869", "example", "adm", NULL, CACE_ARI_TYPE_CONST,
          "hi") // ari://example/adm/CONST/hi
TEST_CASE("84676578616D706C656474657374216474686174", "example", "test", NULL, CACE_ARI_TYPE_CONST,
          "that") // ari://example/test/CONST/that
TEST_CASE("85676578616D706C656474657374D903EC6A323032342D30362D3235216474686174", "example", "test", "2024-06-25",
          CACE_ARI_TYPE_CONST,
          "that") // ari://example/test@2024-06-25/CONST/that
TEST_CASE("84676578616D706C65652174657374216474686174", "example", "!test", NULL, CACE_ARI_TYPE_CONST,
          "that") // ari://example/!test/CONST/that
TEST_CASE("85676578616D706C656474657374226474686174811822", "example", "test", NULL, CACE_ARI_TYPE_CTRL,
          "that") // ari://example/test/CTRL/that(34)
void test_cace_ari_cbor_decode_objref_path_text(const char *hexval, const char *org_id, const char *model_id,
                                                const char *model_rev, cace_ari_type_t type_id, const char *obj_id)
{
    cace_ari_t ari = CACE_ARI_INIT_UNDEFINED;

    check_decoding(&ari, hexval);
    TEST_ASSERT_TRUE(ari.is_ref);

    TEST_ASSERT_EQUAL(CACE_ARI_IDSEG_TEXT, ari.as_ref.objpath.org_id.form);
    TEST_ASSERT_EQUAL_STRING(org_id, m_string_get_cstr(ari.as_ref.objpath.org_id.as_text));

    TEST_ASSERT_EQUAL(CACE_ARI_IDSEG_TEXT, ari.as_ref.objpath.model_id.form);
    TEST_ASSERT_EQUAL_STRING(model_id, m_string_get_cstr(ari.as_ref.objpath.model_id.as_text));

    {
        cace_ari_date_t expect_rev;
        cace_ari_date_init(&expect_rev);
        if (model_rev)
        {
            TEST_ASSERT_EQUAL_INT(0, cace_ari_date_from_text(&expect_rev, model_rev));
        }
        TEST_ASSERT_EQUAL_INT(0, cace_ari_date_cmp(&expect_rev, &ari.as_ref.objpath.model_rev));
        cace_ari_date_init(&expect_rev);
    }

    TEST_ASSERT_EQUAL(CACE_ARI_IDSEG_INT, ari.as_ref.objpath.type_id.form);
    TEST_ASSERT_EQUAL_INT(type_id, ari.as_ref.objpath.type_id.as_int);

    TEST_ASSERT_EQUAL(CACE_ARI_IDSEG_TEXT, ari.as_ref.objpath.obj_id.form);
    TEST_ASSERT_EQUAL_STRING(obj_id, m_string_get_cstr(ari.as_ref.objpath.obj_id.as_text));

    cace_ari_deinit(&ari);
}

TEST_CASE("8419FFFF12201822", 65535, 18, CACE_ARI_TYPE_IDENT, 34)    // ari://65535/18/IDENT/34
TEST_CASE("8519FFFF02220481626869", 65535, 2, CACE_ARI_TYPE_CTRL, 4) // ari://65535/2/CTRL/4(hi)
void test_cace_ari_cbor_decode_objref_path_int(const char *hexval, cace_ari_int_id_t org_id, cace_ari_int_id_t model_id,
                                               cace_ari_type_t type_id, cace_ari_int_id_t obj_id)
{
    cace_ari_t ari = CACE_ARI_INIT_UNDEFINED;

    check_decoding(&ari, hexval);
    TEST_ASSERT_TRUE(ari.is_ref);

    TEST_ASSERT_EQUAL(CACE_ARI_IDSEG_INT, ari.as_ref.objpath.org_id.form);
    TEST_ASSERT_EQUAL_INT(org_id, ari.as_ref.objpath.org_id.as_int);

    TEST_ASSERT_EQUAL(CACE_ARI_IDSEG_INT, ari.as_ref.objpath.model_id.form);
    TEST_ASSERT_EQUAL_INT(model_id, ari.as_ref.objpath.model_id.as_int);

    TEST_ASSERT_FALSE(ari.as_ref.objpath.model_rev.valid);

    TEST_ASSERT_EQUAL(CACE_ARI_IDSEG_INT, ari.as_ref.objpath.type_id.form);
    TEST_ASSERT_EQUAL_INT(type_id, ari.as_ref.objpath.type_id.as_int);

    TEST_ASSERT_EQUAL(CACE_ARI_IDSEG_INT, ari.as_ref.objpath.obj_id.form);
    TEST_ASSERT_EQUAL_INT(obj_id, ari.as_ref.objpath.obj_id.as_int);

    cace_ari_deinit(&ari);
}

// ari:/RPTSET/n=1234;r=/TP/20000101T001640Z;(t=/TD/PT0S;s=//example/test/CTRL/hi;(null,3,h'6869'))
TEST_CASE("8215831904D21903E8850084676578616D706C65647465737422626869F603426869", 1234, 1000, 0, 1)
TEST_CASE("8215831904D282211904D2850084676578616D706C65647465737422626869F603426869", 1234, 12, 340000000, 1)
void test_cace_ari_cbor_decode_rptset(const char *hexval, int expect_nonce, time_t expect_tv_sec, long expect_tv_nsec,
                                      int expect_reports)
{
    cace_ari_t ari = CACE_ARI_INIT_UNDEFINED;

    check_decoding(&ari, hexval);

    const cace_ari_rptset_t *set = cace_ari_get_rptset(&ari);
    TEST_ASSERT_NOT_NULL(set);
    TEST_ASSERT_EQUAL_INT(set->nonce.as_lit.value.as_int64, expect_nonce);

    const struct timespec *tm = &(set->reftime.as_lit.value.as_timespec);
    TEST_ASSERT_EQUAL_INT(tm->tv_sec, expect_tv_sec);
    TEST_ASSERT_EQUAL_INT(tm->tv_nsec, expect_tv_nsec);

    const cace_ari_report_list_t *reports = &(set->reports);
    TEST_ASSERT_EQUAL_INT(cace_ari_report_list_size(*reports), expect_reports);

    cace_ari_deinit(&ari);
}

TEST_CASE("82158282041904D21903E8", 1234, 1000, 0)
void test_cace_ari_cbor_encode_rptset(const char *expect_hexval, int nonce, time_t tv_sec, long tv_nsec)
{
    cace_ari_t ari = CACE_ARI_INIT_UNDEFINED;

    struct timespec tm = { .tv_sec = tv_sec, .tv_nsec = tv_nsec };

    cace_ari_rptset_t *set = cace_ari_set_rptset(&ari);
    cace_ari_set_int(&set->nonce, nonce);
    cace_ari_set_tp(&set->reftime, tm);

    check_encoding(&ari, expect_hexval);
    cace_ari_deinit(&ari);
}

void test_cace_ari_cbor_decode_lit_prim_undef(void)
{
    cace_ari_t ari = CACE_ARI_INIT_UNDEFINED;

    check_decoding(&ari, "F7");
    TEST_ASSERT_FALSE(ari.is_ref);
    TEST_ASSERT_FALSE(ari.as_lit.has_ari_type);
    TEST_ASSERT_EQUAL_INT(CACE_ARI_PRIM_UNDEFINED, ari.as_lit.prim_type);

    cace_ari_deinit(&ari);
}

void test_cace_ari_cbor_decode_lit_prim_null(void)
{
    cace_ari_t ari = CACE_ARI_INIT_UNDEFINED;

    check_decoding(&ari, "F6");
    TEST_ASSERT_FALSE(ari.is_ref);
    TEST_ASSERT_FALSE(ari.as_lit.has_ari_type);
    TEST_ASSERT_EQUAL_INT(CACE_ARI_PRIM_NULL, ari.as_lit.prim_type);

    cace_ari_deinit(&ari);
}

TEST_CASE("F4", false)
TEST_CASE("F5", true)
void test_cace_ari_cbor_decode_lit_prim_bool(const char *inhex, bool expect)
{
    cace_ari_t ari = CACE_ARI_INIT_UNDEFINED;

    check_decoding(&ari, inhex);
    TEST_ASSERT_FALSE(ari.is_ref);
    TEST_ASSERT_FALSE(ari.as_lit.has_ari_type);
    TEST_ASSERT_EQUAL_INT(CACE_ARI_PRIM_BOOL, ari.as_lit.prim_type);
    TEST_ASSERT_EQUAL_INT(expect, ari.as_lit.value.as_bool);

    cace_ari_deinit(&ari);
}

TEST_CASE("3B7FFFFFFFFFFFFFFF", -0x8000000000000000)
TEST_CASE("29", -10)
TEST_CASE("20", -1)
TEST_CASE("00", 0)
TEST_CASE("01", 1)
TEST_CASE("0a", 10)
TEST_CASE("1904D2", 1234)
TEST_CASE("1B0000000100000000", 4294967296)
TEST_CASE("1B7FFFFFFFFFFFFFFF", 0x7FFFFFFFFFFFFFFF)
void test_cace_ari_cbor_decode_lit_prim_int64(const char *inhex, int64_t expect)
{
    cace_ari_t ari = CACE_ARI_INIT_UNDEFINED;

    check_decoding(&ari, inhex);
    TEST_ASSERT_FALSE(ari.is_ref);
    TEST_ASSERT_FALSE(ari.as_lit.has_ari_type);
    TEST_ASSERT_EQUAL_INT(CACE_ARI_PRIM_INT64, ari.as_lit.prim_type);
    TEST_ASSERT_EQUAL_INT(expect, ari.as_lit.value.as_int64);

    cace_ari_deinit(&ari);
}

TEST_CASE("1B8000000000000000", 0x8000000000000000)
TEST_CASE("1BFFFFFFFFFFFFFFFF", 0xFFFFFFFFFFFFFFFF)
void test_cace_ari_cbor_decode_lit_prim_uint64(const char *inhex, uint64_t expect)
{
    cace_ari_t ari = CACE_ARI_INIT_UNDEFINED;

    check_decoding(&ari, inhex);
    TEST_ASSERT_FALSE(ari.is_ref);
    TEST_ASSERT_FALSE(ari.as_lit.has_ari_type);
    TEST_ASSERT_EQUAL_INT(CACE_ARI_PRIM_UINT64, ari.as_lit.prim_type);
    TEST_ASSERT_EQUAL_INT(expect, ari.as_lit.value.as_uint64);

    cace_ari_deinit(&ari);
}

TEST_CASE("F93E00", 1.5)
TEST_CASE("F97E00", (cace_ari_real32)NAN)
void test_cace_ari_cbor_decode_lit_prim_float32(const char *inhex, cace_ari_real32 expect)
{
    cace_ari_t ari = CACE_ARI_INIT_UNDEFINED;

    check_decoding(&ari, inhex);
    TEST_ASSERT_FALSE(ari.is_ref);
    TEST_ASSERT_FALSE(ari.as_lit.has_ari_type);
    TEST_ASSERT_EQUAL_INT(CACE_ARI_PRIM_FLOAT64, ari.as_lit.prim_type);
    TEST_ASSERT_EQUAL_FLOAT(expect, ari.as_lit.value.as_float64);

    cace_ari_deinit(&ari);
}

TEST_CASE("F90000", 0.0)
TEST_CASE("F93E00", 1.5)
TEST_CASE("F97E00", (cace_ari_real64)NAN)
TEST_CASE("F97C00", (cace_ari_real64)INFINITY)
TEST_CASE("F9FC00", (cace_ari_real64)-INFINITY)
void test_cace_ari_cbor_decode_lit_prim_float64(const char *inhex, cace_ari_real64 expect)
{
    cace_ari_t ari = CACE_ARI_INIT_UNDEFINED;

    check_decoding(&ari, inhex);
    TEST_ASSERT_FALSE(ari.is_ref);
    TEST_ASSERT_FALSE(ari.as_lit.has_ari_type);
    TEST_ASSERT_EQUAL_INT(CACE_ARI_PRIM_FLOAT64, ari.as_lit.prim_type);
    TEST_ASSERT_EQUAL_DOUBLE(expect, ari.as_lit.value.as_float64);

    cace_ari_deinit(&ari);
}
TEST_CASE("60", "")
TEST_CASE("626869", "hi")
void test_cace_ari_cbor_decode_lit_prim_tstr(const char *inhex, const char *expect)
{
    cace_ari_t ari = CACE_ARI_INIT_UNDEFINED;

    check_decoding(&ari, inhex);
    TEST_ASSERT_FALSE(ari.is_ref);
    TEST_ASSERT_FALSE(ari.as_lit.has_ari_type);
    TEST_ASSERT_EQUAL_INT(CACE_ARI_PRIM_TSTR, ari.as_lit.prim_type);
    if (expect)
    {
        TEST_ASSERT_TRUE(ari.as_lit.value.as_data.owned);
        TEST_ASSERT_GREATER_THAN_INT(0, ari.as_lit.value.as_data.len);
        TEST_ASSERT_EQUAL_STRING(expect, ari.as_lit.value.as_data.ptr);
    }
    else
    {
        TEST_ASSERT_FALSE(ari.as_lit.value.as_data.owned);
        TEST_ASSERT_EQUAL_INT(0, ari.as_lit.value.as_data.len);
        TEST_ASSERT_NULL(ari.as_lit.value.as_data.ptr);
    }

    cace_ari_deinit(&ari);
}

TEST_CASE("40", NULL, 0)
TEST_CASE("426869", "hi", 2)
void test_cace_ari_cbor_decode_lit_prim_bstr(const char *inhex, const char *expect, size_t expect_len)
{
    cace_ari_t ari = CACE_ARI_INIT_UNDEFINED;

    check_decoding(&ari, inhex);
    TEST_ASSERT_FALSE(ari.is_ref);
    TEST_ASSERT_FALSE(ari.as_lit.has_ari_type);
    TEST_ASSERT_EQUAL_INT(CACE_ARI_PRIM_BSTR, ari.as_lit.prim_type);
    if (expect)
    {
        TEST_ASSERT_TRUE(ari.as_lit.value.as_data.owned);
        TEST_ASSERT_GREATER_OR_EQUAL_INT(0, ari.as_lit.value.as_data.len);
        TEST_ASSERT_EQUAL_MEMORY(expect, ari.as_lit.value.as_data.ptr, expect_len);
    }
    else
    {
        TEST_ASSERT_FALSE(ari.as_lit.value.as_data.owned);
        TEST_ASSERT_EQUAL_INT(0, ari.as_lit.value.as_data.len);
        TEST_ASSERT_NULL(ari.as_lit.value.as_data.ptr);
    }

    cace_ari_deinit(&ari);
}

void test_cace_ari_cbor_decode_lit_typed_null(void)
{
    cace_ari_t ari = CACE_ARI_INIT_UNDEFINED;

    check_decoding(&ari, "8200F6");
    TEST_ASSERT_FALSE(ari.is_ref);
    TEST_ASSERT_TRUE(ari.as_lit.has_ari_type);
    TEST_ASSERT_EQUAL_INT(CACE_ARI_TYPE_NULL, ari.as_lit.ari_type);
    TEST_ASSERT_EQUAL_INT(CACE_ARI_PRIM_NULL, ari.as_lit.prim_type);

    cace_ari_deinit(&ari);
}

TEST_CASE("8201F4", false)
TEST_CASE("8201F5", true)
void test_cace_ari_cbor_decode_lit_typed_bool(const char *inhex, bool expect)
{
    cace_ari_t ari = CACE_ARI_INIT_UNDEFINED;

    check_decoding(&ari, inhex);
    TEST_ASSERT_FALSE(ari.is_ref);
    TEST_ASSERT_TRUE(ari.as_lit.has_ari_type);
    TEST_ASSERT_EQUAL_INT(CACE_ARI_TYPE_BOOL, ari.as_lit.ari_type);
    TEST_ASSERT_EQUAL_INT(CACE_ARI_PRIM_BOOL, ari.as_lit.prim_type);
    TEST_ASSERT_EQUAL_INT(expect, ari.as_lit.value.as_bool);

    cace_ari_deinit(&ari);
}

TEST_CASE("820200", CACE_ARI_TYPE_BYTE, 0)
TEST_CASE("82021864", CACE_ARI_TYPE_BYTE, 100)
TEST_CASE("82041864", CACE_ARI_TYPE_INT, 100)
TEST_CASE("82051864", CACE_ARI_TYPE_UINT, 100)
TEST_CASE("82061864", CACE_ARI_TYPE_VAST, 100)
TEST_CASE("82071864", CACE_ARI_TYPE_UVAST, 100)
void test_cace_ari_cbor_decode_lit_typed_int64(const char *inhex, cace_ari_type_t typ, int64_t expect)
{
    cace_ari_t ari = CACE_ARI_INIT_UNDEFINED;

    check_decoding(&ari, inhex);
    TEST_ASSERT_FALSE(ari.is_ref);
    TEST_ASSERT_TRUE(ari.as_lit.has_ari_type);
    TEST_ASSERT_EQUAL_INT(typ, ari.as_lit.ari_type);
    TEST_ASSERT_EQUAL_INT(CACE_ARI_PRIM_INT64, ari.as_lit.prim_type);
    TEST_ASSERT_EQUAL_INT(expect, ari.as_lit.value.as_int64);

    cace_ari_deinit(&ari);
}

TEST_CASE("8209F93E00", true)
void test_cace_ari_cbor_decode_lit_typed_real64(const char *inhex, bool expect)
{
    cace_ari_t ari = CACE_ARI_INIT_UNDEFINED;

    check_decoding(&ari, inhex);
    TEST_ASSERT_FALSE(ari.is_ref);
    TEST_ASSERT_TRUE(ari.as_lit.has_ari_type);
    TEST_ASSERT_EQUAL_INT(CACE_ARI_TYPE_REAL64, ari.as_lit.ari_type);
    TEST_ASSERT_EQUAL_INT(CACE_ARI_PRIM_FLOAT64, ari.as_lit.prim_type);
    TEST_ASSERT_EQUAL_INT(expect, ari.as_lit.value.as_float64);

    cace_ari_deinit(&ari);
}

TEST_CASE("820C82200C", 1, 200000000)
TEST_CASE("820C82080C", 1200000000, 0)
TEST_CASE("820C82070C", 120000000, 0)
TEST_CASE("820C82280C", 0, 12)
void test_cace_ari_cbor_decode_tp(const char *inhex, time_t expect_sec, long expect_nsec)
{
    cace_ari_t ari = CACE_ARI_INIT_UNDEFINED;

    check_decoding(&ari, inhex);
    TEST_ASSERT_FALSE(ari.is_ref);
    TEST_ASSERT_TRUE(ari.as_lit.has_ari_type);
    TEST_ASSERT_EQUAL_INT(CACE_ARI_TYPE_TP, ari.as_lit.ari_type);
    TEST_ASSERT_EQUAL_INT(CACE_ARI_PRIM_TIMESPEC, ari.as_lit.prim_type);
    TEST_ASSERT_EQUAL_INT(expect_sec, ari.as_lit.value.as_timespec.tv_sec);
    TEST_ASSERT_EQUAL_INT(expect_nsec, ari.as_lit.value.as_timespec.tv_nsec);
}

TEST_CASE("8519FFFF02200520")                   // bad parameter type
TEST_CASE("8619FFFF02200580182D")               // extra item after parameters
TEST_CASE("A0")                                 // bad major type
TEST_CASE("821182A0820417")                     // AC with item having bad major type
TEST_CASE("8364746573740A6474686174")           // ari://test/TEXTSTR/that
TEST_CASE("820C82290C")                         // TP with decimal fraction exponent of -10
TEST_CASE("820C820A0C")                         // TP with decimal fraction exponent of 10
TEST_CASE("820EFB3FF3333333333333")             // ari:/LABEL/1.2
TEST_CASE("821386030102030405")                 // ari:/TBL/c=3;(1,2,3)(4,5)
TEST_CASE("821380")                             // ari:/TBL/
TEST_CASE("8213816474657374")                   // ari:/TBL/test
TEST_CASE("8214816474657374")                   // ari:/EXECSET/n=test;()
TEST_CASE("82148120")                           // ari:/EXECSET/n=-1;()
TEST_CASE("82158264746573741A2B450625")         // ari:/RPTSET/n=test;r=725943845;
TEST_CASE("821582FB3FF33333333333331A2B450625") // ari:/RPTSET/n=1.2;r=725943845;
// ari:/RPTSET/n=1234;r=test;(t=/TD/PT0S;s=//test/CTRL/hi;(null,3,h'6869'))
TEST_CASE("8215831904D26474657374850083647465737422626869F603426869")
// ari:/RPTSET/n=1234;r=/REAL64/1.0;(t=/TD/PT0S;s=//test/CTRL/hi;(null,3,h'6869'))
TEST_CASE("8215831904D28209F93C00850083647465737422626869F603426869")
void test_cace_ari_cbor_decode_failure(const char *inhex)
{
    string_t intext;
    string_init_set_str(intext, inhex);
    cace_data_t indata;
    cace_data_init(&indata);
    TEST_ASSERT_EQUAL_INT(0, cace_base16_decode(&indata, intext));
    string_clear(intext);

    const size_t inlen = indata.len;
    cace_ari_t   ari   = CACE_ARI_INIT_UNDEFINED;
    size_t       used;
    int          res = cace_ari_cbor_decode(&ari, &indata, &used, &errm);
    cace_data_deinit(&indata);
    cace_ari_deinit(&ari);

    TEST_ASSERT_NOT_EQUAL_INT(0, res);
    TEST_ASSERT_NOT_NULL_MESSAGE(errm, "decode failure must provide a message");
    TEST_MESSAGE(errm);

    TEST_ASSERT_EQUAL_INT_MESSAGE(inlen, used, "cace_ari_cbor_decode() did not use all data");
}

TEST_CASE("0001") // too much data
void test_cace_ari_cbor_decode_partial(const char *inhex)
{
    string_t intext;
    string_init_set_str(intext, inhex);
    cace_data_t indata;
    cace_data_init(&indata);
    TEST_ASSERT_EQUAL_INT(0, cace_base16_decode(&indata, intext));
    string_clear(intext);

    const size_t inlen = indata.len;
    cace_ari_t   ari   = CACE_ARI_INIT_UNDEFINED;
    size_t       used;
    int          res = cace_ari_cbor_decode(&ari, &indata, &used, &errm);
    cace_data_deinit(&indata);
    if (res && errm)
    {
        TEST_FAIL_MESSAGE(errm);
    }
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, res, "cace_ari_cbor_decode() failed");

    TEST_ASSERT_LESS_THAN_INT_MESSAGE(inlen, used, "cace_ari_cbor_decode() used all data");
}

TEST_CASE("820001")                 // ari:/NULL/1
TEST_CASE("820101")                 // ari:/BOOL/1
TEST_CASE("820220")                 // ari:/BYTE/-1
TEST_CASE("8212A182040AF5")         // ari:/AM/(/INT/10=true), no typed keys
TEST_CASE("8202190100")             // ari:/BYTE/256
TEST_CASE("82043A80000000")         // ari:/INT/-2147483649
TEST_CASE("82041A80000000")         // ari:/INT/2147483648
TEST_CASE("820520")                 // ari:/UINT/-1
TEST_CASE("82051B0000000100000000") // ari:/UINT/4294967296
TEST_CASE("82061B8000000000000000") // ari:/VAST/0x8000000000000000
TEST_CASE("820720")                 // ari:/UVAST/-1
TEST_CASE("8208FBC7EFFFFFE091FF3D") // ari:/REAL32/-3.40282347E+38
TEST_CASE("8208FB47EFFFFFE091FF3D") // ari:/REAL32/3.40282347E+38
void test_cace_ari_cbor_decode_invalid(const char *inhex)
{
    string_t intext;
    string_init_set_str(intext, inhex);
    cace_data_t indata;
    cace_data_init(&indata);
    TEST_ASSERT_EQUAL_INT(0, cace_base16_decode(&indata, intext));
    string_clear(intext);

    const size_t inlen = indata.len;
    cace_ari_t   ari   = CACE_ARI_INIT_UNDEFINED;
    size_t       used;
    int          res = cace_ari_cbor_decode(&ari, &indata, &used, &errm);
    cace_data_deinit(&indata);
    if (res && errm)
    {
        TEST_FAIL_MESSAGE(errm);
    }
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, res, "cace_ari_cbor_decode() failed");

    TEST_ASSERT_FALSE_MESSAGE(cace_amm_builtin_validate(&ari), "cace_amm_builtin_validate() succeeded");
    cace_ari_deinit(&ari);

    TEST_ASSERT_EQUAL_INT_MESSAGE(inlen, used, "cace_ari_cbor_decode() did not use all data");
}

TEST_CASE("F7") // ari:undefined
TEST_CASE("F6") // ari:null
TEST_CASE("8201F4")
TEST_CASE("8201F5")
TEST_CASE("82041864")
TEST_CASE("82051864")
TEST_CASE("82061864")
TEST_CASE("82071864")
TEST_CASE("8212A303F50A626869626F6804")                       // ari:/AM/(3=true,10=hi,oh=4) AM key ordering
TEST_CASE("F5")                                               // ari:true
TEST_CASE("F4")                                               // ari:false
TEST_CASE("1904D2")                                           // ari:1234
TEST_CASE("626869")                                           // ari:hi
TEST_CASE("686869207468657265")                               // ari:%22hi%20there%22
TEST_CASE("426869")                                           // ari:h'6869'
TEST_CASE("8200F6")                                           // ari:/NULL/null
TEST_CASE("8201F4")                                           // ari:/BOOL/false
TEST_CASE("8201F5")                                           // ari:/BOOL/true
TEST_CASE("82040A")                                           // ari:/INT/10
TEST_CASE("820429")                                           // ari:/INT/-10
TEST_CASE("8208F94900")                                       // ari:/REAL32/10
TEST_CASE("8208FB4024333333333333")                           // ari:/REAL32/10.1
TEST_CASE("8208FB3FB999999999999A")                           // ari:/REAL32/0.1
TEST_CASE("8208F97E00")                                       // ari:/REAL32/NaN
TEST_CASE("8209F97C00")                                       // ari:/REAL64/Infinity
TEST_CASE("8209F9FC00")                                       // ari:/REAL64/-Infinity
TEST_CASE("820B426869")                                       // ari:/BYTESTR/h'6869'
TEST_CASE("820A626869")                                       // ari:/TEXTSTR/hi
TEST_CASE("820A686869207468657265")                           // ari:/TEXTSTR/%22hi%20there%22
TEST_CASE("820E626869")                                       // ari:/LABEL/hi
TEST_CASE("820E01")                                           // ari:/LABEL/1
TEST_CASE("820C1A2B450625")                                   // ari:/TP/20230102T030405Z
TEST_CASE("821180")                                           // ari:/AC/()
TEST_CASE("8211816161")                                       // ari:/AC/(a)
TEST_CASE("821183616161626163")                               // ari:/AC/(a,b,c)
TEST_CASE("821182F6820417")                                   // ari:/AC/(null,/INT/23)
TEST_CASE("821182F6821183F7820417821180")                     // ari:/AC/(null,/AC/(undefined,/INT/23,/AC/()))
TEST_CASE("8212A0")                                           // ari:/AM/()
TEST_CASE("8212A303F50A626869626F6804")                       // ari:/AM/(3=true,10=hi,oh=4)
TEST_CASE("82138403010203")                                   // ari:/TBL/c=3;(1,2,3)
TEST_CASE("82138703010203040506")                             // ari:/TBL/c=3;(1,2,3)(4,5,6)
TEST_CASE("82138100")                                         // ari:/TBL/c=0;
TEST_CASE("82138101")                                         // ari:/TBL/c=1;
TEST_CASE("821481F6")                                         // ari:/EXECSET/n=null;()
TEST_CASE("8214821904D284676578616D706C65647465737422626869") // ari:/EXECSET/n=1234;(//example/test/CTRL/hi)
TEST_CASE(
    "82148342686984676578616D706C6564746573742262686984676578616D706C65647465737422626568") // ari:/EXECSET/n=h'6869';(//example/test/CTRL/hi,//example/test/CTRL/eh)
TEST_CASE(
    "8215831904D21903E8850084676578616D706C65647465737422626869F603426869") // ari:/RPTSET/n=1234;r=/TP/20000101T001640Z;(t=/TD/PT0S;s=//example/test/CTRL/hi;(null,3,h'6869'))
TEST_CASE(
    "8215831904D21A2B450625850084676578616D706C65647465737422626869F603426869") // ari:/RPTSET/n=1234;r=/TP/20230102T030405Z;(t=/TD/PT0S;s=//example/test/CTRL/hi;(null,3,h'6869'))
TEST_CASE("84676578616D706C656474657374216474686174") // ari://example/test/CONST/that
// ari://example/test@2024-06-25/
TEST_CASE("85676578616D706C656474657374D903EC6A323032342D30362D3235F6F6")
// ari://example/test@2024-06-25/CONST/that
TEST_CASE("85676578616D706C656474657374D903EC6A323032342D30362D3235216474686174")
TEST_CASE("84676578616D706C65652174657374216474686174")     // ari://example/!test/CONST/that
TEST_CASE("85676578616D706C6564746573742A6474686174811822") // ari://example/test/CTRL/that(34)
TEST_CASE("8519FFFF02220481626869")                         // ari://65535/2/CTRL/4(hi)
TEST_CASE("820F410A")                                       // ari:/CBOR/h'0A'
TEST_CASE("820F4BA164746573748203F94480")                   // ari:/CBOR/h'A164746573748203F94480'
void test_cace_ari_cbor_loopback(const char *inhex)
{
    string_t intext;
    string_init_set_str(intext, inhex);
    cace_data_t indata;
    cace_data_init(&indata);
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, cace_base16_decode(&indata, intext), "cace_base16_decode() failed");
    string_clear(intext);

    cace_ari_t ari = CACE_ARI_INIT_UNDEFINED;
    int        res = cace_ari_cbor_decode(&ari, &indata, NULL, &errm);
    if (res && errm)
    {
        TEST_FAIL_MESSAGE(errm);
    }
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, res, "cace_ari_cbor_decode() failed");
    {
        m_string_t debug;
        m_string_init(debug);
        cace_ari_text_encode(debug, &ari, CACE_ARI_TEXT_ENC_OPTS_DEFAULT);
        TEST_PRINTF("decoded to %s", m_string_get_cstr(debug));
        m_string_clear(debug);
    }

    cace_data_t outdata;
    cace_data_init(&outdata);
    res = cace_ari_cbor_encode(&outdata, &ari);
    cace_ari_deinit(&ari);
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, res, "cace_ari_cbor_encode() failed");
    {
        m_string_t debug;
        m_string_init(debug);
        cace_base16_encode(debug, &outdata, true);
        TEST_PRINTF("encoded to %s", m_string_get_cstr(debug));
        m_string_clear(debug);
    }

    TEST_ASSERT_EQUAL_INT(indata.len, outdata.len);
    TEST_ASSERT_EQUAL_MEMORY(indata.ptr, outdata.ptr, indata.len);
    cace_data_deinit(&outdata);
    cace_data_deinit(&indata);
}
