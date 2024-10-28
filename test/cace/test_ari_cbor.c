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

TEST_CASE("example-adm-a@2024-06-25", false, 0, NULL,
          "8378186578616D706C652D61646D2D6140323032342D30362D3235F6F6")             // ari://example-adm-a@2024-06-25/
TEST_CASE("example-adm-a", false, 0, NULL, "836D6578616D706C652D61646D2D61F6F6")    // ari://example-adm-a/
TEST_CASE("!example-odm-b", false, 0, NULL, "836E216578616D706C652D6F646D2D62F6F6") // ari://!example-odm-b/
TEST_CASE("adm", false, 0, NULL, "836361646DF6F6")                                  // ari://adm/
TEST_CASE(NULL, true, ARI_TYPE_CONST, "hi", "83F621626869")                         // "./CONST/hi
TEST_CASE("adm", true, ARI_TYPE_CONST, "hi", "836361646D21626869")                  // ari://adm/CONST/hi
TEST_CASE("test", true, ARI_TYPE_CONST, "that", "836474657374216474686174")         // ari://test/CONST/that
TEST_CASE("test@1234", true, ARI_TYPE_CONST, "that", "8369746573744031323334216474686174") // ari://test@1234/CONST/that
TEST_CASE("!test", true, ARI_TYPE_CONST, "that", "83652174657374216474686174")             // ari://!test/CONST/that
void test_ari_cbor_encode_objref_path_text(const char *ns_id, bool has_type, ari_type_t type_id, const char *obj_id,
                                           const char *expect)
{
    ari_t ari    = ARI_INIT_UNDEFINED;
    bool  has_ns = ns_id != NULL, has_obj = obj_id != NULL;
    ari_set_objref_path_textid_opt(&ari, has_ns ? ns_id : NULL, has_type ? &type_id : NULL, has_obj ? obj_id : NULL);

    check_encoding(&ari, expect);

    ari_deinit(&ari);
}

TEST_CASE(true, 18, false, 0, false, 0, "8312F6F6")               // ari://18/
TEST_CASE(true, 65536, false, 0, false, 0, "831A00010000F6F6")    // ari://65536/
TEST_CASE(true, -20, false, 0, false, 0, "8333F6F6")              // ari://-20/
TEST_CASE(false, 0, true, ARI_TYPE_IDENT, true, 34, "83F6201822") // ./IDENT/34
TEST_CASE(true, 18, true, ARI_TYPE_IDENT, true, 34, "8312201822") // ari://18/IDENT/34
void test_ari_cbor_encode_objref_path_int(bool has_ns, int64_t ns_id, bool has_type, ari_type_t type_id, bool has_obj,
                                          int64_t obj_id, const char *expect)
{
    ari_t ari = ARI_INIT_UNDEFINED;
    ari_set_objref_path_intid_opt(&ari, has_ns ? &ns_id : NULL, has_type ? &type_id : NULL, has_obj ? &obj_id : NULL);

    check_encoding(&ari, expect);

    ari_deinit(&ari);
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

TEST_CASE("836361646D21626869", "adm", ARI_TYPE_CONST, "hi")                         // ari://adm/CONST/hi
TEST_CASE("836474657374216474686174", "test", ARI_TYPE_CONST, "that")                // "ari://test/CONST/that
TEST_CASE("8369746573744031323334216474686174", "test@1234", ARI_TYPE_CONST, "that") // ari://test@1234/CONST/that
TEST_CASE("83652174657374216474686174", "!test", ARI_TYPE_CONST, "that")             // ari://!test/CONST/that
TEST_CASE("846474657374226474686174811822", "test", ARI_TYPE_CTRL, "that")           // ari://test/CTRL/that(34)
void test_ari_cbor_decode_objref_path_text(const char *hexval, const char *ns_id, ari_type_t type_id,
                                           const char *obj_id)
{
    ari_t ari = ARI_INIT_UNDEFINED;

    check_decoding(&ari, hexval);
    TEST_ASSERT_TRUE(ari.is_ref);
    TEST_ASSERT_EQUAL_STRING(ns_id, ari.as_ref.objpath.ns_id.as_text);
    TEST_ASSERT_EQUAL_INT(type_id, ari.as_ref.objpath.type_id.as_int);
    TEST_ASSERT_EQUAL_STRING(obj_id, ari.as_ref.objpath.obj_id.as_text);

    ari_deinit(&ari);
}

TEST_CASE("8312201822", 18, ARI_TYPE_IDENT, 34)
TEST_CASE("8402220481626869", 2, ARI_TYPE_CTRL, 4) // ari://2/CTRL/4(hi)
void test_ari_cbor_decode_objref_path_int(const char *hexval, int64_t ns_id, ari_type_t type_id, int64_t obj_id)
{
    ari_t ari = ARI_INIT_UNDEFINED;

    check_decoding(&ari, hexval);
    TEST_ASSERT_TRUE(ari.is_ref);
    TEST_ASSERT_EQUAL_INT(ns_id, ari.as_ref.objpath.ns_id.as_int);
    TEST_ASSERT_EQUAL_INT(type_id, ari.as_ref.objpath.type_id.as_int);
    TEST_ASSERT_EQUAL_INT(obj_id, ari.as_ref.objpath.obj_id.as_int);

    ari_deinit(&ari);
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

TEST_CASE("8402202020")
TEST_CASE("A0")             // bad major type
TEST_CASE("821182A0820417") // AC with item having bad major type
// TEST_CASE("836474657374226474686174") // ari://test/CTRL/that
TEST_CASE("8364746573740A6474686174") // ari://test/TEXTSTR/that
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
TEST_CASE("8212A303F50A626869626F6804")     // ari:/AM/(3=true,10=hi,oh=4) AM key ordering
TEST_CASE("8464746573742A6474686174811822") // ari://test/-11/that(34)
TEST_CASE("F5") // ari:true
TEST_CASE("F4") // ari:false
TEST_CASE("1904D2") // ari:1234
TEST_CASE("626869") // ari:hi
TEST_CASE("686869207468657265") // ari:%22hi%20there%22
TEST_CASE("426869") // ari:h'6869'
TEST_CASE("8200F6") // ari:/NULL/null
TEST_CASE("8201F4") // ari:/BOOL/false
TEST_CASE("8201F5") // ari:/BOOL/true
TEST_CASE("82040A") // ari:/INT/10
TEST_CASE("820429") // ari:/INT/-10
TEST_CASE("8208F94900") // ari:/REAL32/10
TEST_CASE("8208FB4024333333333333") // ari:/REAL32/10.1
TEST_CASE("8208FB3FB999999999999A") // ari:/REAL32/0.1
TEST_CASE("8208F97E00") // ari:/REAL32/NaN
TEST_CASE("8209F97C00") // ari:/REAL64/+Infinity
TEST_CASE("8209F9FC00") // ari:/REAL64/-Infinity
TEST_CASE("820B426869") // ari:/BYTESTR/h'6869'
TEST_CASE("820A626869") // ari:/TEXTSTR/hi
TEST_CASE("820A686869207468657265") // ari:/TEXTSTR/%22hi%20there%22
TEST_CASE("820E626869") // ari:/LABEL/hi
TEST_CASE("820C1A2B450625") // ari:/TP/20230102T030405Z
TEST_CASE("821180") // ari:/AC/()
TEST_CASE("8211816161") // ari:/AC/(a)
TEST_CASE("821183616161626163") // ari:/AC/(a,b,c)
TEST_CASE("821182F6820417") // ari:/AC/(null,/INT/23)
TEST_CASE("821182F6821183F7820417821180") // ari:/AC/(null,/AC/(undefined,/INT/23,/AC/()))
TEST_CASE("8212A0") // ari:/AM/()
TEST_CASE("8212A303F50A626869626F6804") // ari:/AM/(3=true,10=hi,oh=4)
TEST_CASE("82138403010203") // ari:/TBL/c=3;(1,2,3)
TEST_CASE("82138703010203040506") // ari:/TBL/c=3;(1,2,3)(4,5,6)
TEST_CASE("82138100") // ari:/TBL/c=0;
TEST_CASE("82138101") // ari:/TBL/c=1;
TEST_CASE("821481F6") // ari:/EXECSET/n=null;()
TEST_CASE("8214821904D283647465737422626869") // ari:/EXECSET/n=1234;(//test/CTRL/hi)
TEST_CASE("8214834268698364746573742262686983647465737422626568") // ari:/EXECSET/n=h'6869';(//test/CTRL/hi,//test/CTRL/eh)
TEST_CASE("8215831904D21903E8850083647465737422626869F603426869") // ari:/RPTSET/n=1234;r=/TP/20000101T001640Z;(t=/TD/PT0S;s=//test/CTRL/hi;(null,3,h'6869'))
TEST_CASE("8215831904D21A2B450625850083647465737422626869F603426869") // ari:/RPTSET/n=1234;r=/TP/20230102T030405Z;(t=/TD/PT0S;s=//test/CTRL/hi;(null,3,h'6869'))
TEST_CASE("836474657374216474686174") // ari://test/CONST/that
TEST_CASE("8369746573744031323334216474686174") // ari://test@1234/CONST/that
TEST_CASE("83652174657374216474686174") // ari://!test/CONST/that
TEST_CASE("846474657374226474686174811822") // ari://test/CTRL/that(34)
TEST_CASE("8402220481626869") // ari://2/CTRL/4(hi)
TEST_CASE("820F410A") // ari:/CBOR/h'0A'
TEST_CASE("820F4BA164746573748203F94480") // ari:/CBOR/h'A164746573748203F94480'
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
