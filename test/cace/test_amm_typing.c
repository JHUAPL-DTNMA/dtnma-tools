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
#include <cace/amm/typing.h>
#include <cace/ari/text_util.h>
#include <cace/ari/cbor.h>
#include <unity.h>

// Allow this macro
#define TEST_CASE(...)

static void check_match(const amm_type_t *type, const char *inhex, bool expect)
{
    string_t intext;
    string_init_set_str(intext, inhex);
    cace_data_t indata;
    cace_data_init(&indata);
    int res = base16_decode(&indata, intext);
    string_clear(intext);
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, res, "base16_decode() failed");

    ari_t val = ARI_INIT_UNDEFINED;
    res       = ari_cbor_decode(&val, &indata, NULL, NULL);
    cace_data_deinit(&indata);
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, res, "ari_cbor_decode() failed");

    res = amm_type_match(type, &val);
    TEST_ASSERT_EQUAL_INT_MESSAGE(expect, res, "amm_type_match() failed");

    ari_deinit(&val);
}

static void check_convert(const amm_type_t *type, const char *inhex, const char *expecthex)
{
    string_t intext;
    string_init_set_str(intext, inhex);
    cace_data_t indata;
    cace_data_init(&indata);
    int res = base16_decode(&indata, intext);
    string_clear(intext);
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, res, "base16_decode() failed");

    ari_t inval = ARI_INIT_UNDEFINED;
    res         = ari_cbor_decode(&inval, &indata, NULL, NULL);
    cace_data_deinit(&indata);
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, res, "ari_cbor_decode() failed");

    ari_t outval = ARI_INIT_UNDEFINED;
    res          = amm_type_convert(type, &outval, &inval);
    ari_deinit(&inval);
    if (expecthex)
    {
        TEST_ASSERT_EQUAL_INT_MESSAGE(0, res, "amm_type_convert() failed");
    }
    else
    {
        TEST_ASSERT_NOT_EQUAL_INT_MESSAGE(0, res, "amm_type_convert() succeeded");
        return;
    }

    cace_data_t outdata;
    cace_data_init(&outdata);
    res = ari_cbor_encode(&outdata, &outval);
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, res, "ari_cbor_encode() failed");

    string_t outhex;
    string_init(outhex);
    res = base16_encode(outhex, &outdata, true);
    cace_data_deinit(&outdata);
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, res, "base16_encode() failed");

    TEST_ASSERT_EQUAL_STRING(expecthex, string_get_cstr(outhex));
    string_clear(outhex);
    ari_deinit(&outval);
}

TEST_CASE("F7", false)               // ari:undefined
TEST_CASE("F6", true)                // ari:null
TEST_CASE("F4", false)               // ari:false
TEST_CASE("F5", false)               // ari:true
TEST_CASE("0A", false)               // ari:10
TEST_CASE("8201F5", false)           // ari:/BOOL/true
TEST_CASE("82041864", false)         // ari:/INT/100 matches
TEST_CASE("82061864", false)         // ari:/VAST/100 explicitly not an INT
TEST_CASE("8402200481626869", false) // ari://2/-1/4(hi)
void test_amm_type_match_null(const char *inhex, bool expect)
{
    const amm_type_t *type = amm_type_get_builtin(ARI_TYPE_NULL);
    check_match(type, inhex, expect);
}

TEST_CASE("F7", false)               // ari:undefined
TEST_CASE("F6", false)               // ari:null
TEST_CASE("F4", false)               // ari:false
TEST_CASE("F5", false)               // ari:true
TEST_CASE("0A", true)                // ari:10
TEST_CASE("82041864", true)          // ari:/INT/100 matches
TEST_CASE("82061864", false)         // ari:/VAST/100 explicitly not an INT
TEST_CASE("FA49864700", true)        // ari:1.1e+06
TEST_CASE("8208FA49864700", false)   // ari:/REAL32/1.1e+06 explicitly not an INT
TEST_CASE("8402200481626869", false) // ari://2/-1/4(hi)
void test_amm_type_match_int(const char *inhex, bool expect)
{
    const amm_type_t *type = amm_type_get_builtin(ARI_TYPE_INT);
    check_match(type, inhex, expect);
}

TEST_CASE("F7", false)              // ari:undefined
TEST_CASE("F6", false)              // ari:null
TEST_CASE("F4", false)              // ari:false
TEST_CASE("F5", false)              // ari:true
TEST_CASE("0A", false)              // ari:10
TEST_CASE("82041864", false)        // ari:/INT/100
TEST_CASE("82061864", false)        // ari:/VAST/100
TEST_CASE("FA49864700", false)      // ari:1.1e+06
TEST_CASE("8402200481626869", true) // ari://2/-1/4(hi)
void test_amm_type_match_ident(const char *inhex, bool expect)
{
    const amm_type_t *type = amm_type_get_builtin(ARI_TYPE_IDENT);
    check_match(type, inhex, expect);
}

TEST_CASE("F7", false)               // ari:undefined
TEST_CASE("F6", false)               // ari:null
TEST_CASE("F4", false)               // ari:false
TEST_CASE("F5", false)               // ari:true
TEST_CASE("0A", true)                // ari:10
TEST_CASE("82041864", true)          // ari:/INT/100 matches
TEST_CASE("82061864", false)         // ari:/VAST/100 explicitly not an INT
TEST_CASE("8402200481626869", false) // ari://2/-1/4(hi)
void test_amm_type_match_semtype_use_1(const char *inhex, bool expect)
{
    amm_type_t mytype;
    amm_type_init(&mytype);
    {
        const amm_type_t *base = amm_type_get_builtin(ARI_TYPE_INT);
        TEST_ASSERT_EQUAL_INT(0, amm_type_set_use_direct(&mytype, base));
    }

    check_match(&mytype, inhex, expect);
    amm_type_deinit(&mytype);
}

TEST_CASE("F7", false)               // ari:undefined
TEST_CASE("F6", true)                // ari:null
TEST_CASE("F4", false)               // ari:false
TEST_CASE("F5", false)               // ari:true
TEST_CASE("0A", true)                // ari:10
TEST_CASE("82041864", true)          // ari:/INT/100 matches
TEST_CASE("82061864", false)         // ari:/VAST/100 explicitly not an INT
TEST_CASE("8402200481626869", false) // ari://2/-1/4(hi)
void test_amm_type_match_semtype_union_1(const char *inhex, bool expect)
{
    amm_type_t mytype;
    amm_type_init(&mytype);
    {
        TEST_ASSERT_EQUAL_INT(0, amm_type_set_union_size(&mytype, 2));
        {
            amm_type_t *choice = amm_type_set_union_get(&mytype, 0);
            TEST_ASSERT_NOT_NULL(choice);
            amm_type_set_use_direct(choice, amm_type_get_builtin(ARI_TYPE_INT));
        }
        {
            amm_type_t *choice = amm_type_set_union_get(&mytype, 1);
            TEST_ASSERT_NOT_NULL(choice);
            amm_type_set_use_direct(choice, amm_type_get_builtin(ARI_TYPE_NULL));
        }
    }

    check_match(&mytype, inhex, expect);
    amm_type_deinit(&mytype);
}

TEST_CASE("F7", "F7")                   // ari:undefined
TEST_CASE("F6", "8200F6")               // ari:null
TEST_CASE("F5", "8200F6")               // ari:true
TEST_CASE("0A", "8200F6")               // ari:10
TEST_CASE("82041864", "8200F6")         // ari:/INT/100
TEST_CASE("82061864", "8200F6")         // ari:/VAST/100
TEST_CASE("821181F6", "8200F6")         // ari:/AC/(undefined)
TEST_CASE("8402200481626869", "8200F6") // ari://2/-1/4(hi)
void test_amm_type_convert_null(const char *inhex, const char *expecthex)
{
    const amm_type_t *type = amm_type_get_builtin(ARI_TYPE_NULL);
    check_convert(type, inhex, expecthex);
}

TEST_CASE("F7", "F7")                   // ari:undefined
TEST_CASE("F6", "8201F4")               // ari:null
TEST_CASE("F5", "8201F5")               // ari:true
TEST_CASE("00", "8201F4")               // ari:0
TEST_CASE("0A", "8201F5")               // ari:10
TEST_CASE("820400", "8201F4")           // ari:/INT/0
TEST_CASE("82041864", "8201F5")         // ari:/INT/100
TEST_CASE("82061864", "8201F5")         // ari:/VAST/100
TEST_CASE("821181F6", "8201F5")         // ari:/AC/(undefined)
TEST_CASE("8402200481626869", "8201F5") // ari://2/-1/4(hi)
void test_amm_type_convert_bool(const char *inhex, const char *expecthex)
{
    const amm_type_t *type = amm_type_get_builtin(ARI_TYPE_BOOL);
    check_convert(type, inhex, expecthex);
}

TEST_CASE("F7", "F7")               // ari:undefined
TEST_CASE("F5", NULL)               // ari:true
TEST_CASE("F4", NULL)               // ari:false
TEST_CASE("00", "820200")           // ari:0
TEST_CASE("0A", "82020A")           // ari:10
TEST_CASE("820400", "820200")       // ari:/INT/0
TEST_CASE("82041864", "82021864")   // ari:/INT/100
TEST_CASE("82061864", "82021864")   // ari:/VAST/100
TEST_CASE("82061904D2", NULL)       // ari:/VAST/1234
TEST_CASE("821181F6", NULL)         // ari:/AC/(undefined)
TEST_CASE("8402200481626869", NULL) // ari://2/-1/4(hi)
void test_amm_type_convert_byte(const char *inhex, const char *expecthex)
{
    const amm_type_t *type = amm_type_get_builtin(ARI_TYPE_BYTE);
    check_convert(type, inhex, expecthex);
}

TEST_CASE("F7", "F7")                         // ari:undefined
TEST_CASE("F6", NULL)                         // ari:null
TEST_CASE("F5", NULL)                         // ari:true
TEST_CASE("F4", NULL)                         // ari:false
TEST_CASE("00", "820400")                     // ari:0
TEST_CASE("0A", "82040A")                     // ari:10
TEST_CASE("820400", "820400")                 // ari:/INT/0
TEST_CASE("82041864", "82041864")             // ari:/INT/100
TEST_CASE("82061864", "82041864")             // ari:/VAST/100
TEST_CASE("FA49864700", "82041A0010C8E0")     // ari:1.1e+06
TEST_CASE("8208FA49864700", "82041A0010C8E0") // ari:/REAL32/1.1e+06
TEST_CASE("821181F6", NULL)                   // ari:/AC/(undefined)
TEST_CASE("8402200481626869", NULL)           // ari://2/-1/4(hi)
void test_amm_type_convert_int(const char *inhex, const char *expecthex)
{
    const amm_type_t *type = amm_type_get_builtin(ARI_TYPE_INT);
    check_convert(type, inhex, expecthex);
}

TEST_CASE("F7", "F7")                         // ari:undefined
TEST_CASE("F6", NULL)                         // ari:null
TEST_CASE("F5", NULL)                         // ari:true
TEST_CASE("F4", NULL)                         // ari:false
TEST_CASE("00", "8209F90000")                 // ari:0
TEST_CASE("0A", "8209F94900")                 // ari:10
TEST_CASE("820400", "8209F90000")             // ari:/INT/0
TEST_CASE("82041864", "8209F95640")           // ari:/INT/100
TEST_CASE("82061864", "8209F95640")           // ari:/VAST/100
TEST_CASE("FA49864700", "8209FA49864700")     // ari:1.1e+06
TEST_CASE("8208FA49864700", "8209FA49864700") // ari:/REAL32/1.1e+06
TEST_CASE("821181F6", NULL)                   // ari:/AC/(undefined)
TEST_CASE("8402200481626869", NULL)           // ari://2/-1/4(hi)
void test_amm_type_convert_real64(const char *inhex, const char *expecthex)
{
    const amm_type_t *type = amm_type_get_builtin(ARI_TYPE_REAL64);
    check_convert(type, inhex, expecthex);
}

TEST_CASE("F7", "F7")                             // ari:undefined
TEST_CASE("F6", NULL)                             // ari:null
TEST_CASE("F5", NULL)                             // ari:true
TEST_CASE("F4", NULL)                             // ari:false
TEST_CASE("00", NULL)                             // ari:0
TEST_CASE("0A", NULL)                             // ari:10
TEST_CASE("820400", NULL)                         // ari:/INT/0
TEST_CASE("82041864", NULL)                       // ari:/INT/100
TEST_CASE("82061864", NULL)                       // ari:/VAST/100
TEST_CASE("FA49864700", NULL)                     // ari:1.1e+06
TEST_CASE("8208FA49864700", NULL)                 // ari:/REAL32/1.1e+06
TEST_CASE("821181F6", NULL)                       // ari:/AC/(undefined)
TEST_CASE("8402200481626869", "8402200481626869") // ari://2/-1/4(hi)
void test_amm_type_convert_ident(const char *inhex, const char *expecthex)
{
    const amm_type_t *type = amm_type_get_builtin(ARI_TYPE_IDENT);
    check_convert(type, inhex, expecthex);
}

TEST_CASE("820C1A2B450625", "820C1A2B450625") // ari:/TP/20230102T030405Z
TEST_CASE("82041864", "820C1864") // ari://INT/100 -> ari://TP/100
TEST_CASE("8208FB40593F34D6A161E5", "820C82231A000F68D4") // ari://REAL64/100.9876
TEST_CASE("8209FA7F000000", NULL) // REAL64 exceeds INT64 MAX
TEST_CASE("8209FAEF000000", NULL) // REAL64 less than UINT64 MAX
TEST_CASE("820D1864", NULL) // ari://TD/100
TEST_CASE("82043863", "820C3863") // ari://INT/-100 
TEST_CASE("8209FBC0593F34D6A161E5", "820C82233A000F68D3") // ari://REAL64/-100.9876
TEST_CASE("8209F97C00", NULL) // ari://real64/Infinity
TEST_CASE("8209F97E00", NULL) // ari://real64/NaN
void test_amm_type_convert_tp(const char *inhex, const char *expecthex)
{
    const amm_type_t *type = amm_type_get_builtin(ARI_TYPE_TP);
    check_convert(type, inhex, expecthex);
}

TEST_CASE("820D822018CD", "820D822018CD") // ari:/TD/PT20.5S
TEST_CASE("82041864", "820D1864") // ari://INT/100 -> ari://TD/100
TEST_CASE("8208FB40593F34D6A161E5", "820D82231A000F68D4") // ari://REAL64/100.9876
TEST_CASE("8209FA7F000000", NULL) // REAL64 exceeds INT64 MAX
TEST_CASE("8209FAEF000000", NULL) // REAL64 less than UINT64 MAX
TEST_CASE("820C1864", NULL) // ari://TP/100
TEST_CASE("82043863", "820D3863") // ari://INT/-100 
TEST_CASE("8208FBC0593F34D6A161E5", "820D82233A000F68D3") // ari://REAL32/-100.9876
TEST_CASE("8209F97C00", NULL) // ari://real64/Infinity
TEST_CASE("8209F97E00", NULL) // ari://real64/NaN
void test_amm_type_convert_td(const char *inhex, const char *expecthex)
{
    const amm_type_t *type = amm_type_get_builtin(ARI_TYPE_TD);
    check_convert(type, inhex, expecthex);
}
