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
 * Test the ari_cbor.h interfaces.
 *
 * As a shortcut to producing expected binary contents, use commands similar to:
 *  echo "42" | diag2cbor.rb | xxd -i
 */
#include <cace/ari/cbor.h>
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

static void check_encoding(const ari_t *ari, const char *expect_hex)
{
    cace_data_t buf;
    cace_data_init(&buf);

    TEST_ASSERT_EQUAL_INT_MESSAGE(0, ari_cbor_encode(&buf, ari), "ari_cbor_encode() failed");

    string_t outhex;
    string_init(outhex);
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, base16_encode(outhex, &buf, true), "ari_cbor_encode() failed");

    TEST_ASSERT_EQUAL_STRING_MESSAGE(expect_hex, string_get_cstr(outhex), "Mismatch in encoded data");

    string_clear(outhex);
    cace_data_deinit(&buf);
}

void test_ari_cbor_encode_lit_prim_undef(void)
{
    ari_t ari = ARI_INIT_UNDEFINED;
    // no value is undefined

    check_encoding(&ari, "F7");

    ari_deinit(&ari);
}
void test_ari_cbor_encode_lit_prim_null(void)
{
    ari_t ari = ARI_INIT_UNDEFINED;
    ari_set_null(&ari);

    check_encoding(&ari, "F6");

    ari_deinit(&ari);
}
void test_ari_cbor_encode_lit_prim_uint(void)
{
    ari_t ari = ARI_INIT_UNDEFINED;
    ari_set_prim_uint64(&ari, 1234);

    check_encoding(&ari, "1904D2");

    ari_deinit(&ari);
}

void test_ari_cbor_encode_lit_prim_text_nocopy(void)
{
    ari_t ari = ARI_INIT_UNDEFINED;
    ari_set_tstr(&ari, "test", false);

    check_encoding(&ari, "6474657374");

    ari_deinit(&ari);
}
void test_ari_cbor_encode_lit_prim_text_copy(void)
{
    ari_t ari = ARI_INIT_UNDEFINED;
    ari_set_tstr(&ari, "test", true);

    check_encoding(&ari, "6474657374");

    ari_deinit(&ari);
}

void test_ari_cbor_encode_lit_typed_tp(void)
{
    const struct timespec delta = {
        .tv_sec = 30,
    };
    ari_t ari = ARI_INIT_UNDEFINED;
    ari_set_tp(&ari, delta);

    check_encoding(&ari, "820C181E");

    ari_deinit(&ari);
}

void test_ari_cbor_encode_lit_typed_td(void)
{
    const struct timespec delta = {
        .tv_sec = 30,
    };
    ari_t ari = ARI_INIT_UNDEFINED;
    ari_set_td(&ari, delta);

    check_encoding(&ari, "820D181E");

    ari_deinit(&ari);
}

void test_ari_cbor_encode_lit_typed_ac_empty(void)
{
    ari_t ari = ARI_INIT_UNDEFINED;
    ari_set_ac(&ari, NULL);

    check_encoding(&ari, "821180");

    ari_deinit(&ari);
}

void test_ari_cbor_encode_lit_typed_ac_1item(void)
{
    {
        ari_ac_t acinit;
        ari_ac_init(&acinit);
        {
            ari_t *item = ari_list_push_back_new(acinit.items);
            ari_set_null(item);
        }

        ari_t ari = ARI_INIT_UNDEFINED;
        ari_set_ac(&ari, &acinit);

        check_encoding(&ari, "821181F6");

        ari_deinit(&ari);
    }
}

static void check_decoding(ari_t *ari, const char *inhex)
{
    string_t intext;
    string_init_set_str(intext, inhex);
    cace_data_t indata;
    cace_data_init(&indata);
    TEST_ASSERT_EQUAL_INT(0, base16_decode(&indata, intext));
    string_clear(intext);

    const size_t inlen = indata.len;
    size_t       used;
    int          res = ari_cbor_decode(ari, &indata, &used, &errm);
    cace_data_deinit(&indata);
    if (res && errm)
    {
        TEST_FAIL_MESSAGE(errm);
    }
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, res, "ari_cbor_decode() failed");
    TEST_ASSERT_EQUAL_INT_MESSAGE(inlen, used, "ari_cbor_decode() did not use all data");

    TEST_ASSERT_TRUE_MESSAGE(amm_builtin_validate(ari), "amm_builtin_validate() failed");
}

void test_ari_cbor_decode_lit_prim_undef(void)
{
    ari_t ari = ARI_INIT_UNDEFINED;

    check_decoding(&ari, "F7");
    TEST_ASSERT_FALSE(ari.is_ref);
    TEST_ASSERT_FALSE(ari.as_lit.has_ari_type);
    TEST_ASSERT_EQUAL_INT(ARI_PRIM_UNDEFINED, ari.as_lit.prim_type);

    ari_deinit(&ari);
}

void test_ari_cbor_decode_lit_prim_null(void)
{
    ari_t ari = ARI_INIT_UNDEFINED;

    check_decoding(&ari, "F6");
    TEST_ASSERT_FALSE(ari.is_ref);
    TEST_ASSERT_FALSE(ari.as_lit.has_ari_type);
    TEST_ASSERT_EQUAL_INT(ARI_PRIM_NULL, ari.as_lit.prim_type);

    ari_deinit(&ari);
}

TEST_CASE("F4", false)
TEST_CASE("F5", true)
void test_ari_cbor_decode_lit_prim_bool(const char *inhex, bool expect)
{
    ari_t ari = ARI_INIT_UNDEFINED;

    check_decoding(&ari, inhex);
    TEST_ASSERT_FALSE(ari.is_ref);
    TEST_ASSERT_FALSE(ari.as_lit.has_ari_type);
    TEST_ASSERT_EQUAL_INT(ARI_PRIM_BOOL, ari.as_lit.prim_type);
    TEST_ASSERT_EQUAL_INT(expect, ari.as_lit.value.as_bool);

    ari_deinit(&ari);
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
void test_ari_cbor_decode_lit_prim_int64(const char *inhex, int64_t expect)
{
    ari_t ari = ARI_INIT_UNDEFINED;

    check_decoding(&ari, inhex);
    TEST_ASSERT_FALSE(ari.is_ref);
    TEST_ASSERT_FALSE(ari.as_lit.has_ari_type);
    TEST_ASSERT_EQUAL_INT(ARI_PRIM_INT64, ari.as_lit.prim_type);
    TEST_ASSERT_EQUAL_INT(expect, ari.as_lit.value.as_int64);

    ari_deinit(&ari);
}

TEST_CASE("1B8000000000000000", 0x8000000000000000)
TEST_CASE("1BFFFFFFFFFFFFFFFF", 0xFFFFFFFFFFFFFFFF)
void test_ari_cbor_decode_lit_prim_uint64(const char *inhex, uint64_t expect)
{
    ari_t ari = ARI_INIT_UNDEFINED;

    check_decoding(&ari, inhex);
    TEST_ASSERT_FALSE(ari.is_ref);
    TEST_ASSERT_FALSE(ari.as_lit.has_ari_type);
    TEST_ASSERT_EQUAL_INT(ARI_PRIM_UINT64, ari.as_lit.prim_type);
    TEST_ASSERT_EQUAL_INT(expect, ari.as_lit.value.as_uint64);

    ari_deinit(&ari);
}

/*
TEST_CASE("F93E00", 1.5)
TEST_CASE("F97E00", (ari_real32)NAN)
void test_ari_cbor_decode_lit_prim_float32(const char *inhex, ari_real32 expect)
{
  ari_t ari = ARI_UNDEFINED;

  check_decoding(&ari, inhex);
  TEST_ASSERT_FALSE(ari.is_ref);
  TEST_ASSERT_FALSE(ari.as_lit.has_ari_type);
  TEST_ASSERT_EQUAL_INT(ARI_PRIM_FLOAT32, ari.as_lit.prim_type);
  TEST_ASSERT_EQUAL_FLOAT(expect, ari.as_lit.value.as_float32);

  ari_deinit(&ari);
}
*/

TEST_CASE("F90000", 0.0)
TEST_CASE("F93E00", 1.5)
TEST_CASE("F97E00", (ari_real64)NAN)
TEST_CASE("F97C00", (ari_real64)INFINITY)
TEST_CASE("F9FC00", (ari_real64)-INFINITY)
void test_ari_cbor_decode_lit_prim_float64(const char *inhex, ari_real64 expect)
{
    ari_t ari = ARI_INIT_UNDEFINED;

    check_decoding(&ari, inhex);
    TEST_ASSERT_FALSE(ari.is_ref);
    TEST_ASSERT_FALSE(ari.as_lit.has_ari_type);
    TEST_ASSERT_EQUAL_INT(ARI_PRIM_FLOAT64, ari.as_lit.prim_type);
    TEST_ASSERT_EQUAL_DOUBLE(expect, ari.as_lit.value.as_float64);

    ari_deinit(&ari);
}
TEST_CASE("60", "")
TEST_CASE("626869", "hi")
void test_ari_cbor_decode_lit_prim_tstr(const char *inhex, const char *expect)
{
    ari_t ari = ARI_INIT_UNDEFINED;

    check_decoding(&ari, inhex);
    TEST_ASSERT_FALSE(ari.is_ref);
    TEST_ASSERT_FALSE(ari.as_lit.has_ari_type);
    TEST_ASSERT_EQUAL_INT(ARI_PRIM_TSTR, ari.as_lit.prim_type);
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

    ari_deinit(&ari);
}

TEST_CASE("40", NULL, 0)
TEST_CASE("426869", "hi", 2)
void test_ari_cbor_decode_lit_prim_bstr(const char *inhex, const char *expect, size_t expect_len)
{
    ari_t ari = ARI_INIT_UNDEFINED;

    check_decoding(&ari, inhex);
    TEST_ASSERT_FALSE(ari.is_ref);
    TEST_ASSERT_FALSE(ari.as_lit.has_ari_type);
    TEST_ASSERT_EQUAL_INT(ARI_PRIM_BSTR, ari.as_lit.prim_type);
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

    ari_deinit(&ari);
}

void test_ari_cbor_decode_lit_typed_null(void)
{
    ari_t ari = ARI_INIT_UNDEFINED;

    check_decoding(&ari, "8200F6");
    TEST_ASSERT_FALSE(ari.is_ref);
    TEST_ASSERT_TRUE(ari.as_lit.has_ari_type);
    TEST_ASSERT_EQUAL_INT(ARI_TYPE_NULL, ari.as_lit.ari_type);
    TEST_ASSERT_EQUAL_INT(ARI_PRIM_NULL, ari.as_lit.prim_type);

    ari_deinit(&ari);
}

TEST_CASE("8201F4", false)
TEST_CASE("8201F5", true)
void test_ari_cbor_decode_lit_typed_bool(const char *inhex, bool expect)
{
    ari_t ari = ARI_INIT_UNDEFINED;

    check_decoding(&ari, inhex);
    TEST_ASSERT_FALSE(ari.is_ref);
    TEST_ASSERT_TRUE(ari.as_lit.has_ari_type);
    TEST_ASSERT_EQUAL_INT(ARI_TYPE_BOOL, ari.as_lit.ari_type);
    TEST_ASSERT_EQUAL_INT(ARI_PRIM_BOOL, ari.as_lit.prim_type);
    TEST_ASSERT_EQUAL_INT(expect, ari.as_lit.value.as_bool);

    ari_deinit(&ari);
}

TEST_CASE("820200", ARI_TYPE_BYTE, 0)
TEST_CASE("82021864", ARI_TYPE_BYTE, 100)
TEST_CASE("82041864", ARI_TYPE_INT, 100)
TEST_CASE("82051864", ARI_TYPE_UINT, 100)
TEST_CASE("82061864", ARI_TYPE_VAST, 100)
TEST_CASE("82071864", ARI_TYPE_UVAST, 100)
void test_ari_cbor_decode_lit_typed_int64(const char *inhex, ari_type_t typ, int64_t expect)
{
    ari_t ari = ARI_INIT_UNDEFINED;

    check_decoding(&ari, inhex);
    TEST_ASSERT_FALSE(ari.is_ref);
    TEST_ASSERT_TRUE(ari.as_lit.has_ari_type);
    TEST_ASSERT_EQUAL_INT(typ, ari.as_lit.ari_type);
    TEST_ASSERT_EQUAL_INT(ARI_PRIM_INT64, ari.as_lit.prim_type);
    TEST_ASSERT_EQUAL_INT(expect, ari.as_lit.value.as_int64);

    ari_deinit(&ari);
}

TEST_CASE("8209F93E00", true)
void test_ari_cbor_decode_lit_typed_real64(const char *inhex, bool expect)
{
    ari_t ari = ARI_INIT_UNDEFINED;

    check_decoding(&ari, inhex);
    TEST_ASSERT_FALSE(ari.is_ref);
    TEST_ASSERT_TRUE(ari.as_lit.has_ari_type);
    TEST_ASSERT_EQUAL_INT(ARI_TYPE_REAL64, ari.as_lit.ari_type);
    TEST_ASSERT_EQUAL_INT(ARI_PRIM_FLOAT64, ari.as_lit.prim_type);
    TEST_ASSERT_EQUAL_INT(expect, ari.as_lit.value.as_float64);

    ari_deinit(&ari);
}

TEST_CASE("A0")             // bad major type
TEST_CASE("821182A0820417") // AC with item having bad major type
void test_ari_cbor_decode_failure(const char *inhex)
{
    string_t intext;
    string_init_set_str(intext, inhex);
    cace_data_t indata;
    cace_data_init(&indata);
    TEST_ASSERT_EQUAL_INT(0, base16_decode(&indata, intext));
    string_clear(intext);

    const size_t inlen = indata.len;
    ari_t        ari   = ARI_INIT_UNDEFINED;
    size_t       used;
    int          res = ari_cbor_decode(&ari, &indata, &used, &errm);
    cace_data_deinit(&indata);
    ari_deinit(&ari);

    TEST_ASSERT_NOT_EQUAL_INT(0, res);
    TEST_ASSERT_NOT_NULL_MESSAGE(errm, "decode failure must provide a message");
    TEST_MESSAGE(errm);

    TEST_ASSERT_EQUAL_INT_MESSAGE(inlen, used, "ari_cbor_decode() did not use all data");
}

TEST_CASE("0001") // too much data
void test_ari_cbor_decode_partial(const char *inhex)
{
    string_t intext;
    string_init_set_str(intext, inhex);
    cace_data_t indata;
    cace_data_init(&indata);
    TEST_ASSERT_EQUAL_INT(0, base16_decode(&indata, intext));
    string_clear(intext);

    const size_t inlen = indata.len;
    ari_t        ari   = ARI_INIT_UNDEFINED;
    size_t       used;
    int          res = ari_cbor_decode(&ari, &indata, &used, &errm);
    cace_data_deinit(&indata);
    if (res && errm)
    {
        TEST_FAIL_MESSAGE(errm);
    }
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, res, "ari_cbor_decode() failed");

    TEST_ASSERT_LESS_THAN_INT_MESSAGE(inlen, used, "ari_cbor_decode() used all data");
}

TEST_CASE("820001")         // ari:/NULL/1
TEST_CASE("820101")         // ari:/BOOL/1
TEST_CASE("820220")         // ari:/BYTE/-1
TEST_CASE("8212A182040AF5") // ari:/AM/(/INT/10=true), no typed keys
void test_ari_cbor_decode_invalid(const char *inhex)
{
    string_t intext;
    string_init_set_str(intext, inhex);
    cace_data_t indata;
    cace_data_init(&indata);
    TEST_ASSERT_EQUAL_INT(0, base16_decode(&indata, intext));
    string_clear(intext);

    const size_t inlen = indata.len;
    ari_t        ari   = ARI_INIT_UNDEFINED;
    size_t       used;
    int          res = ari_cbor_decode(&ari, &indata, &used, &errm);
    cace_data_deinit(&indata);
    if (res && errm)
    {
        TEST_FAIL_MESSAGE(errm);
    }
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, res, "ari_cbor_decode() failed");

    TEST_ASSERT_FALSE_MESSAGE(amm_builtin_validate(&ari), "amm_builtin_validate() succeeded");
    ari_deinit(&ari);

    TEST_ASSERT_EQUAL_INT_MESSAGE(inlen, used, "ari_cbor_decode() did not use all data");
}

TEST_CASE("F7") // ari:undefined
TEST_CASE("F6") // ari:null
TEST_CASE("8201F4")
TEST_CASE("8201F5")
TEST_CASE("82041864")
TEST_CASE("82051864")
TEST_CASE("82061864")
TEST_CASE("82071864")
TEST_CASE("8212A303F50A626869626F6804")             // ari:/AM/(3=true,10=hi,oh=4) AM key ordering
TEST_CASE("84647465737464746869736474686174811822") // ari://test/this/that(34)
void test_ari_cbor_loopback(const char *inhex)
{
    string_t intext;
    string_init_set_str(intext, inhex);
    cace_data_t indata;
    cace_data_init(&indata);
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, base16_decode(&indata, intext), "base16_decode() failed");
    string_clear(intext);

    ari_t ari = ARI_INIT_UNDEFINED;
    int   res = ari_cbor_decode(&ari, &indata, NULL, &errm);
    if (res && errm)
    {
        TEST_FAIL_MESSAGE(errm);
    }
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, res, "ari_cbor_decode() failed");

    cace_data_t outdata;
    cace_data_init(&outdata);
    res = ari_cbor_encode(&outdata, &ari);
    ari_deinit(&ari);
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, res, "ari_cbor_encode() failed");

    TEST_ASSERT_EQUAL_INT(indata.len, outdata.len);
    TEST_ASSERT_EQUAL_MEMORY(indata.ptr, outdata.ptr, indata.len);
    cace_data_deinit(&outdata);
    cace_data_deinit(&indata);
}