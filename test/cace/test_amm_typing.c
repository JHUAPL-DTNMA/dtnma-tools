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
#include <cace/amm/semtype.h>
#include <cace/amm/typing.h>
#include <cace/ari/text_util.h>
#include <cace/ari/cbor.h>
#include <unity.h>

// Allow this macro
#define TEST_CASE(...)

static void check_match(const cace_amm_type_t *type, const char *inhex, cace_amm_type_match_res_t expect)
{
    string_t intext;
    string_init_set_str(intext, inhex);
    cace_data_t indata;
    cace_data_init(&indata);
    int res = cace_base16_decode(&indata, intext);
    string_clear(intext);
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, res, "cace_base16_decode() failed");

    cace_ari_t val = CACE_ARI_INIT_UNDEFINED;
    res            = cace_ari_cbor_decode(&val, &indata, NULL, NULL);
    cace_data_deinit(&indata);
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, res, "cace_ari_cbor_decode() failed");

    cace_amm_type_match_res_t got = cace_amm_type_match(type, &val);
    TEST_ASSERT_EQUAL_INT_MESSAGE(expect, got, "cace_amm_type_match() failed");

    cace_ari_deinit(&val);
}

static void check_convert(const cace_amm_type_t *type, const char *inhex, const char *expecthex)
{
    string_t intext;
    string_init_set_str(intext, inhex);
    cace_data_t indata;
    cace_data_init(&indata);
    int res = cace_base16_decode(&indata, intext);
    string_clear(intext);
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, res, "cace_base16_decode() failed");

    cace_ari_t inval = CACE_ARI_INIT_UNDEFINED;
    res              = cace_ari_cbor_decode(&inval, &indata, NULL, NULL);
    cace_data_deinit(&indata);
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, res, "cace_ari_cbor_decode() failed");

    cace_ari_t outval = CACE_ARI_INIT_UNDEFINED;
    res               = cace_amm_type_convert(type, &outval, &inval);
    cace_ari_deinit(&inval);
    if (expecthex)
    {
        TEST_ASSERT_EQUAL_INT_MESSAGE(0, res, "cace_amm_type_convert() failed");
    }
    else
    {
        TEST_ASSERT_NOT_EQUAL_INT_MESSAGE(0, res, "cace_amm_type_convert() succeeded");
        return;
    }

    cace_data_t outdata;
    cace_data_init(&outdata);
    res = cace_ari_cbor_encode(&outdata, &outval);
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, res, "cace_ari_cbor_encode() failed");

    string_t outhex;
    string_init(outhex);
    res = cace_base16_encode(outhex, &outdata, true);
    cace_data_deinit(&outdata);
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, res, "cace_base16_encode() failed");

    TEST_ASSERT_EQUAL_STRING(expecthex, string_get_cstr(outhex));
    string_clear(outhex);
    cace_ari_deinit(&outval);
}

TEST_CASE(CACE_ARI_TYPE_LITERAL)
TEST_CASE(CACE_ARI_TYPE_NULL)
TEST_CASE(CACE_ARI_TYPE_BOOL)
TEST_CASE(CACE_ARI_TYPE_BYTE)
TEST_CASE(CACE_ARI_TYPE_INT)
TEST_CASE(CACE_ARI_TYPE_UINT)
TEST_CASE(CACE_ARI_TYPE_VAST)
TEST_CASE(CACE_ARI_TYPE_UVAST)
TEST_CASE(CACE_ARI_TYPE_REAL32)
TEST_CASE(CACE_ARI_TYPE_REAL64)
TEST_CASE(CACE_ARI_TYPE_TEXTSTR)
TEST_CASE(CACE_ARI_TYPE_BYTESTR)
TEST_CASE(CACE_ARI_TYPE_TP)
TEST_CASE(CACE_ARI_TYPE_TD)
TEST_CASE(CACE_ARI_TYPE_LABEL)
TEST_CASE(CACE_ARI_TYPE_CBOR)
TEST_CASE(CACE_ARI_TYPE_ARITYPE)
TEST_CASE(CACE_ARI_TYPE_AC)
TEST_CASE(CACE_ARI_TYPE_AM)
TEST_CASE(CACE_ARI_TYPE_TBL)
TEST_CASE(CACE_ARI_TYPE_EXECSET)
TEST_CASE(CACE_ARI_TYPE_RPTSET)
TEST_CASE(CACE_ARI_TYPE_OBJECT)
TEST_CASE(CACE_ARI_TYPE_IDENT)
TEST_CASE(CACE_ARI_TYPE_TYPEDEF)
TEST_CASE(CACE_ARI_TYPE_CONST)
TEST_CASE(CACE_ARI_TYPE_VAR)
TEST_CASE(CACE_ARI_TYPE_EDD)
TEST_CASE(CACE_ARI_TYPE_CTRL)
TEST_CASE(CACE_ARI_TYPE_OPER)
TEST_CASE(CACE_ARI_TYPE_SBR)
TEST_CASE(CACE_ARI_TYPE_TBR)
void test_amm_type_get_name(cace_ari_type_t type)
{
    const cace_amm_type_t *typeobj = cace_amm_type_get_builtin(type);
    TEST_ASSERT_NOT_NULL(typeobj);

    cace_ari_t name = CACE_ARI_INIT_UNDEFINED;
    TEST_ASSERT_TRUE(cace_amm_type_get_name(typeobj, &name));

    TEST_ASSERT_TRUE(cace_ari_is_lit_typed(&name, CACE_ARI_TYPE_ARITYPE));
    const cace_data_t *data = cace_ari_cget_tstr(&name);
    TEST_ASSERT_NOT_NULL(data);
    TEST_PRINTF("got name %s", (const char *)(data->ptr));
}

TEST_CASE("F7", CACE_AMM_TYPE_MATCH_UNDEFINED)                    // ari:undefined
TEST_CASE("F6", CACE_AMM_TYPE_MATCH_POSITIVE)                     // ari:null
TEST_CASE("F4", CACE_AMM_TYPE_MATCH_NEGATIVE)                     // ari:false
TEST_CASE("F5", CACE_AMM_TYPE_MATCH_NEGATIVE)                     // ari:true
TEST_CASE("0A", CACE_AMM_TYPE_MATCH_NEGATIVE)                     // ari:10
TEST_CASE("8201F5", CACE_AMM_TYPE_MATCH_NEGATIVE)                 // ari:/BOOL/true
TEST_CASE("82041864", CACE_AMM_TYPE_MATCH_NEGATIVE)               // ari:/INT/100 matches
TEST_CASE("82061864", CACE_AMM_TYPE_MATCH_NEGATIVE)               // ari:/VAST/100 explicitly not an INT
TEST_CASE("8519FFFF02200481626869", CACE_AMM_TYPE_MATCH_NEGATIVE) // ari://65535/2/-1/4(hi)
void test_amm_type_match_null(const char *inhex, cace_amm_type_match_res_t expect)
{
    const cace_amm_type_t *type = cace_amm_type_get_builtin(CACE_ARI_TYPE_NULL);
    check_match(type, inhex, expect);
}

TEST_CASE("F7", CACE_AMM_TYPE_MATCH_UNDEFINED)                    // ari:undefined
TEST_CASE("F6", CACE_AMM_TYPE_MATCH_NEGATIVE)                     // ari:null
TEST_CASE("F4", CACE_AMM_TYPE_MATCH_NEGATIVE)                     // ari:false
TEST_CASE("F5", CACE_AMM_TYPE_MATCH_NEGATIVE)                     // ari:true
TEST_CASE("0A", CACE_AMM_TYPE_MATCH_POSITIVE)                     // ari:10
TEST_CASE("82041864", CACE_AMM_TYPE_MATCH_POSITIVE)               // ari:/INT/100 matches
TEST_CASE("82061864", CACE_AMM_TYPE_MATCH_NEGATIVE)               // ari:/VAST/100 explicitly not an INT
TEST_CASE("FA49864700", CACE_AMM_TYPE_MATCH_POSITIVE)             // ari:1.1e+06
TEST_CASE("8208FA49864700", CACE_AMM_TYPE_MATCH_NEGATIVE)         // ari:/REAL32/1.1e+06 explicitly not an INT
TEST_CASE("8519FFFF02200481626869", CACE_AMM_TYPE_MATCH_NEGATIVE) // ari://65535/2/-1/4(hi)
void test_amm_type_match_int(const char *inhex, cace_amm_type_match_res_t expect)
{
    const cace_amm_type_t *type = cace_amm_type_get_builtin(CACE_ARI_TYPE_INT);
    check_match(type, inhex, expect);
}

TEST_CASE("F7", CACE_AMM_TYPE_MATCH_UNDEFINED)                    // ari:undefined
TEST_CASE("F6", CACE_AMM_TYPE_MATCH_NEGATIVE)                     // ari:null
TEST_CASE("F4", CACE_AMM_TYPE_MATCH_NEGATIVE)                     // ari:false
TEST_CASE("F5", CACE_AMM_TYPE_MATCH_NEGATIVE)                     // ari:true
TEST_CASE("0A", CACE_AMM_TYPE_MATCH_NEGATIVE)                     // ari:10
TEST_CASE("82041864", CACE_AMM_TYPE_MATCH_NEGATIVE)               // ari:/INT/100
TEST_CASE("82061864", CACE_AMM_TYPE_MATCH_NEGATIVE)               // ari:/VAST/100
TEST_CASE("FA49864700", CACE_AMM_TYPE_MATCH_NEGATIVE)             // ari:1.1e+06
TEST_CASE("8519FFFF02200481626869", CACE_AMM_TYPE_MATCH_POSITIVE) // ari://65535/2/-1/4(hi)
void test_amm_type_match_ident(const char *inhex, cace_amm_type_match_res_t expect)
{
    const cace_amm_type_t *type = cace_amm_type_get_builtin(CACE_ARI_TYPE_IDENT);
    check_match(type, inhex, expect);
}

TEST_CASE("F7", CACE_AMM_TYPE_MATCH_UNDEFINED)                    // ari:undefined
TEST_CASE("F6", CACE_AMM_TYPE_MATCH_NEGATIVE)                     // ari:null
TEST_CASE("F4", CACE_AMM_TYPE_MATCH_NEGATIVE)                     // ari:false
TEST_CASE("F5", CACE_AMM_TYPE_MATCH_NEGATIVE)                     // ari:true
TEST_CASE("0A", CACE_AMM_TYPE_MATCH_POSITIVE)                     // ari:10
TEST_CASE("82041864", CACE_AMM_TYPE_MATCH_POSITIVE)               // ari:/INT/100 matches
TEST_CASE("82061864", CACE_AMM_TYPE_MATCH_NEGATIVE)               // ari:/VAST/100 explicitly not an INT
TEST_CASE("8519FFFF02200481626869", CACE_AMM_TYPE_MATCH_NEGATIVE) // ari://65535/2/-1/4(hi)
void test_amm_type_match_semtype_use_1(const char *inhex, cace_amm_type_match_res_t expect)
{
    cace_amm_type_t mytype;
    cace_amm_type_init(&mytype);
    {
        const cace_amm_type_t *base = cace_amm_type_get_builtin(CACE_ARI_TYPE_INT);
        TEST_ASSERT_EQUAL_INT(0, cace_amm_type_set_use_direct(&mytype, base));
    }

    check_match(&mytype, inhex, expect);
    cace_amm_type_deinit(&mytype);
}

TEST_CASE("F7", CACE_AMM_TYPE_MATCH_UNDEFINED)                    // ari:undefined
TEST_CASE("F6", CACE_AMM_TYPE_MATCH_NEGATIVE)                     // ari:null
TEST_CASE("F4", CACE_AMM_TYPE_MATCH_NEGATIVE)                     // ari:false
TEST_CASE("F5", CACE_AMM_TYPE_MATCH_NEGATIVE)                     // ari:true
TEST_CASE("82040A", CACE_AMM_TYPE_MATCH_NEGATIVE)                 // ari:/INT/10
TEST_CASE("82118101", CACE_AMM_TYPE_MATCH_NEGATIVE)               // ari:/AC/(1)
TEST_CASE("8211820102", CACE_AMM_TYPE_MATCH_POSITIVE)             // ari:/AC/(1,2)
TEST_CASE("821183010203", CACE_AMM_TYPE_MATCH_POSITIVE)           // ari:/AC/(1,2,3)
TEST_CASE("82118201F5", CACE_AMM_TYPE_MATCH_NEGATIVE)             // ari:/AC/(1,true)
TEST_CASE("8212A10102", CACE_AMM_TYPE_MATCH_NEGATIVE)             // ari:/AM/(1=2)
TEST_CASE("82138102", CACE_AMM_TYPE_MATCH_NEGATIVE)               // ari:/TBL/c=2;
TEST_CASE("8519FFFF02200481626869", CACE_AMM_TYPE_MATCH_NEGATIVE) // ari://65535/2/-1/4(hi)
void test_amm_type_match_semtype_ulist_1(const char *inhex, cace_amm_type_match_res_t expect)
{
    cace_amm_type_t mytype;
    cace_amm_type_init(&mytype);
    {
        cace_amm_semtype_ulist_t *semtype = cace_amm_type_set_ulist(&mytype);
        TEST_ASSERT_NOT_NULL(semtype);

        cace_amm_type_set_use_direct(&(semtype->item_type), cace_amm_type_get_builtin(CACE_ARI_TYPE_INT));

        semtype->size.has_min = true;
        semtype->size.i_min   = 2;
    }

    check_match(&mytype, inhex, expect);
    cace_amm_type_deinit(&mytype);
}

TEST_CASE("F7", CACE_AMM_TYPE_MATCH_UNDEFINED)                    // ari:undefined
TEST_CASE("F6", CACE_AMM_TYPE_MATCH_NEGATIVE)                     // ari:null
TEST_CASE("F4", CACE_AMM_TYPE_MATCH_NEGATIVE)                     // ari:false
TEST_CASE("F5", CACE_AMM_TYPE_MATCH_NEGATIVE)                     // ari:true
TEST_CASE("82040A", CACE_AMM_TYPE_MATCH_NEGATIVE)                 // ari:/INT/10
TEST_CASE("82118101", CACE_AMM_TYPE_MATCH_NEGATIVE)               // ari:/AC/(1) too few items
TEST_CASE("8211820102", CACE_AMM_TYPE_MATCH_NEGATIVE)             // ari:/AC/(1,2) bad item type
TEST_CASE("82118201F5", CACE_AMM_TYPE_MATCH_POSITIVE)             // ari:/AC/(1,true)
TEST_CASE("82118301F503", CACE_AMM_TYPE_MATCH_NEGATIVE)           // ari:/AC/(1,true,3) too many items
TEST_CASE("8212A10102", CACE_AMM_TYPE_MATCH_NEGATIVE)             // ari:/AM/(1=2)
TEST_CASE("82138102", CACE_AMM_TYPE_MATCH_NEGATIVE)               // ari:/TBL/c=2;
TEST_CASE("8519FFFF02200481626869", CACE_AMM_TYPE_MATCH_NEGATIVE) // ari://65535/2/-1/4(hi)
void test_amm_type_match_semtype_dlist_2item(const char *inhex, cace_amm_type_match_res_t expect)
{
    // diverse list of int and bool
    cace_amm_type_t mytype;
    cace_amm_type_init(&mytype);
    {
        cace_amm_semtype_dlist_t *semtype = cace_amm_type_set_dlist(&mytype, 2);
        TEST_ASSERT_NOT_NULL(semtype);
        {
            cace_amm_type_t *typ = cace_amm_type_array_get(semtype->types, 0);
            TEST_ASSERT_NOT_NULL(typ);
            cace_amm_type_set_use_direct(typ, cace_amm_type_get_builtin(CACE_ARI_TYPE_INT));
        }
        {
            cace_amm_type_t *typ = cace_amm_type_array_get(semtype->types, 1);
            TEST_ASSERT_NOT_NULL(typ);
            cace_amm_type_set_use_direct(typ, cace_amm_type_get_builtin(CACE_ARI_TYPE_BOOL));
        }
    }

    check_match(&mytype, inhex, expect);
    cace_amm_type_deinit(&mytype);
}

TEST_CASE("F7", CACE_AMM_TYPE_MATCH_UNDEFINED)                    // ari:undefined
TEST_CASE("F6", CACE_AMM_TYPE_MATCH_NEGATIVE)                     // ari:null
TEST_CASE("F4", CACE_AMM_TYPE_MATCH_NEGATIVE)                     // ari:false
TEST_CASE("F5", CACE_AMM_TYPE_MATCH_NEGATIVE)                     // ari:true
TEST_CASE("82040A", CACE_AMM_TYPE_MATCH_NEGATIVE)                 // ari:/INT/10
TEST_CASE("82118101", CACE_AMM_TYPE_MATCH_NEGATIVE)               // ari:/AC/(1) too few items
TEST_CASE("8211820102", CACE_AMM_TYPE_MATCH_NEGATIVE)             // ari:/AC/(1,2) bad item type
TEST_CASE("82118201F5", CACE_AMM_TYPE_MATCH_POSITIVE)             // ari:/AC/(1,true)
TEST_CASE("82118301F5F4", CACE_AMM_TYPE_MATCH_POSITIVE)           // ari:/AC/(1,true,false)
TEST_CASE("82118401F5F4F5", CACE_AMM_TYPE_MATCH_NEGATIVE)         // ari:/AC/(1,true,false,true)
TEST_CASE("82118301F503", CACE_AMM_TYPE_MATCH_NEGATIVE)           // ari:/AC/(1,true,3) unmatched items
TEST_CASE("8212A10102", CACE_AMM_TYPE_MATCH_NEGATIVE)             // ari:/AM/(1=2)
TEST_CASE("82138102", CACE_AMM_TYPE_MATCH_NEGATIVE)               // ari:/TBL/c=2;
TEST_CASE("8519FFFF02200481626869", CACE_AMM_TYPE_MATCH_NEGATIVE) // ari://65535/2/-1/4(hi)
void test_amm_type_match_semtype_dlist_seq_minmax(const char *inhex, cace_amm_type_match_res_t expect)
{
    // diverse list of int and seq-of-bool
    cace_amm_type_t mytype;
    cace_amm_type_init(&mytype);
    {
        cace_amm_semtype_dlist_t *semtype = cace_amm_type_set_dlist(&mytype, 2);
        TEST_ASSERT_NOT_NULL(semtype);
        {
            cace_amm_type_t *typ = cace_amm_type_array_get(semtype->types, 0);
            TEST_ASSERT_NOT_NULL(typ);
            cace_amm_type_set_use_direct(typ, cace_amm_type_get_builtin(CACE_ARI_TYPE_INT));
        }
        {
            cace_amm_type_t *typ = cace_amm_type_array_get(semtype->types, 1);
            TEST_ASSERT_NOT_NULL(typ);
            cace_amm_semtype_seq_t *seq = cace_amm_type_set_seq(typ);

            cace_amm_type_set_use_direct(&(seq->item_type), cace_amm_type_get_builtin(CACE_ARI_TYPE_BOOL));
            seq->size.has_min = true;
            seq->size.i_min   = 1;
            seq->size.has_max = true;
            seq->size.i_max   = 2;
        }
    }

    check_match(&mytype, inhex, expect);
    cace_amm_type_deinit(&mytype);
}

TEST_CASE("F7", CACE_AMM_TYPE_MATCH_UNDEFINED)                    // ari:undefined
TEST_CASE("F6", CACE_AMM_TYPE_MATCH_NEGATIVE)                     // ari:null
TEST_CASE("F4", CACE_AMM_TYPE_MATCH_NEGATIVE)                     // ari:false
TEST_CASE("F5", CACE_AMM_TYPE_MATCH_NEGATIVE)                     // ari:true
TEST_CASE("82040A", CACE_AMM_TYPE_MATCH_NEGATIVE)                 // ari:/INT/10
TEST_CASE("8211820102", CACE_AMM_TYPE_MATCH_NEGATIVE)             // ari:/AC/(1,2)
TEST_CASE("82118201F5", CACE_AMM_TYPE_MATCH_NEGATIVE)             // ari:/AC/(1,true)
TEST_CASE("8212A10102", CACE_AMM_TYPE_MATCH_NEGATIVE)             // ari:/AM/(1=2)
TEST_CASE("8212A101F5", CACE_AMM_TYPE_MATCH_POSITIVE)             // ari:/AM/(1=true)
TEST_CASE("82138102", CACE_AMM_TYPE_MATCH_NEGATIVE)               // ari:/TBL/c=2;
TEST_CASE("8519FFFF02200481626869", CACE_AMM_TYPE_MATCH_NEGATIVE) // ari://65535/2/-1/4(hi)
void test_amm_type_match_semtype_umap_1(const char *inhex, cace_amm_type_match_res_t expect)
{
    cace_amm_type_t mytype;
    cace_amm_type_init(&mytype);
    {
        cace_amm_semtype_umap_t *semtype = cace_amm_type_set_umap(&mytype);
        TEST_ASSERT_NOT_NULL(semtype);

        cace_amm_type_set_use_direct(&(semtype->key_type), cace_amm_type_get_builtin(CACE_ARI_TYPE_INT));
        cace_amm_type_set_use_direct(&(semtype->val_type), cace_amm_type_get_builtin(CACE_ARI_TYPE_BOOL));
    }

    check_match(&mytype, inhex, expect);
    cace_amm_type_deinit(&mytype);
}

TEST_CASE("F6", CACE_AMM_TYPE_MATCH_NEGATIVE)           // ari:null
TEST_CASE("8211820102", CACE_AMM_TYPE_MATCH_NEGATIVE)   // ari:/AC/(1,2)
TEST_CASE("82118201F5", CACE_AMM_TYPE_MATCH_NEGATIVE)   // ari:/AC/(1,true)
TEST_CASE("82138102", CACE_AMM_TYPE_MATCH_POSITIVE)     // ari:/TBL/c=2;
TEST_CASE("8213830201F5", CACE_AMM_TYPE_MATCH_POSITIVE) // ari:/TBL/c=2;(1,true)
TEST_CASE("821383020103", CACE_AMM_TYPE_MATCH_NEGATIVE) // ari:/TBL/c=2;(1,3)
void test_amm_type_match_semtype_tblt_1(const char *inhex, cace_amm_type_match_res_t expect)
{
    cace_amm_type_t mytype;
    cace_amm_type_init(&mytype);
    {
        cace_amm_semtype_tblt_t *semtype = cace_amm_type_set_tblt_size(&mytype, 2);
        TEST_ASSERT_NOT_NULL(semtype);
        {
            cace_amm_named_type_t *col = cace_amm_named_type_array_get(semtype->columns, 0);
            TEST_ASSERT_NOT_NULL(col);
            cace_amm_type_set_use_direct(&(col->typeobj), cace_amm_type_get_builtin(CACE_ARI_TYPE_INT));
        }
        {
            cace_amm_named_type_t *col = cace_amm_named_type_array_get(semtype->columns, 1);
            TEST_ASSERT_NOT_NULL(col);
            cace_amm_type_set_use_direct(&(col->typeobj), cace_amm_type_get_builtin(CACE_ARI_TYPE_BOOL));
        }
    }

    check_match(&mytype, inhex, expect);
    cace_amm_type_deinit(&mytype);
}

TEST_CASE("F7", CACE_AMM_TYPE_MATCH_UNDEFINED)                    // ari:undefined
TEST_CASE("F6", CACE_AMM_TYPE_MATCH_POSITIVE)                     // ari:null
TEST_CASE("F4", CACE_AMM_TYPE_MATCH_NEGATIVE)                     // ari:false
TEST_CASE("F5", CACE_AMM_TYPE_MATCH_NEGATIVE)                     // ari:true
TEST_CASE("0A", CACE_AMM_TYPE_MATCH_POSITIVE)                     // ari:10
TEST_CASE("82041864", CACE_AMM_TYPE_MATCH_POSITIVE)               // ari:/INT/100 matches
TEST_CASE("82061864", CACE_AMM_TYPE_MATCH_NEGATIVE)               // ari:/VAST/100 explicitly not an INT
TEST_CASE("8519FFFF02200481626869", CACE_AMM_TYPE_MATCH_NEGATIVE) // ari://65535/2/-1/4(hi)
void test_amm_type_match_semtype_union_1(const char *inhex, cace_amm_type_match_res_t expect)
{
    cace_amm_type_t mytype;
    cace_amm_type_init(&mytype);
    {
        cace_amm_semtype_union_t *semtype = cace_amm_type_set_union_size(&mytype, 2);
        TEST_ASSERT_NOT_NULL(semtype);
        {
            cace_amm_type_t *choice = cace_amm_type_array_get(semtype->choices, 0);
            TEST_ASSERT_NOT_NULL(choice);
            cace_amm_type_set_use_direct(choice, cace_amm_type_get_builtin(CACE_ARI_TYPE_INT));
        }
        {
            cace_amm_type_t *choice = cace_amm_type_array_get(semtype->choices, 1);
            TEST_ASSERT_NOT_NULL(choice);
            cace_amm_type_set_use_direct(choice, cace_amm_type_get_builtin(CACE_ARI_TYPE_NULL));
        }
    }

    check_match(&mytype, inhex, expect);
    cace_amm_type_deinit(&mytype);
}

TEST_CASE("F7", "F7")                         // ari:undefined
TEST_CASE("F6", "8200F6")                     // ari:null
TEST_CASE("F5", "8200F6")                     // ari:true
TEST_CASE("0A", "8200F6")                     // ari:10
TEST_CASE("82041864", "8200F6")               // ari:/INT/100
TEST_CASE("82061864", "8200F6")               // ari:/VAST/100
TEST_CASE("821181F6", "8200F6")               // ari:/AC/(undefined)
TEST_CASE("8519FFFF02200481626869", "8200F6") // ari://65535/2/-1/4(hi)
void test_amm_type_convert_null(const char *inhex, const char *expecthex)
{
    const cace_amm_type_t *type = cace_amm_type_get_builtin(CACE_ARI_TYPE_NULL);
    check_convert(type, inhex, expecthex);
}

TEST_CASE("F7", "F7")                         // ari:undefined
TEST_CASE("F6", "8201F4")                     // ari:null
TEST_CASE("F5", "8201F5")                     // ari:true
TEST_CASE("00", "8201F4")                     // ari:0
TEST_CASE("0A", "8201F5")                     // ari:10
TEST_CASE("820400", "8201F4")                 // ari:/INT/0
TEST_CASE("82041864", "8201F5")               // ari:/INT/100
TEST_CASE("82061864", "8201F5")               // ari:/VAST/100
TEST_CASE("821181F6", "8201F5")               // ari:/AC/(undefined)
TEST_CASE("8519FFFF02200481626869", "8201F5") // ari://65535/2/-1/4(hi)
void test_amm_type_convert_bool(const char *inhex, const char *expecthex)
{
    const cace_amm_type_t *type = cace_amm_type_get_builtin(CACE_ARI_TYPE_BOOL);
    check_convert(type, inhex, expecthex);
}

TEST_CASE("F7", "F7")                     // ari:undefined
TEST_CASE("F5", NULL)                     // ari:true
TEST_CASE("F4", NULL)                     // ari:false
TEST_CASE("00", "820200")                 // ari:0
TEST_CASE("0A", "82020A")                 // ari:10
TEST_CASE("820400", "820200")             // ari:/INT/0
TEST_CASE("82041864", "82021864")         // ari:/INT/100
TEST_CASE("82061864", "82021864")         // ari:/VAST/100
TEST_CASE("82061904D2", NULL)             // ari:/VAST/1234
TEST_CASE("821181F6", NULL)               // ari:/AC/(undefined)
TEST_CASE("8519FFFF02200481626869", NULL) // ari://65535/2/-1/4(hi)
void test_amm_type_convert_byte(const char *inhex, const char *expecthex)
{
    const cace_amm_type_t *type = cace_amm_type_get_builtin(CACE_ARI_TYPE_BYTE);
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
TEST_CASE("8519FFFF02200481626869", NULL)     // ari://65535/2/-1/4(hi)
void test_amm_type_convert_int(const char *inhex, const char *expecthex)
{
    const cace_amm_type_t *type = cace_amm_type_get_builtin(CACE_ARI_TYPE_INT);
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
TEST_CASE("8519FFFF02200481626869", NULL)     // ari://65535/2/-1/4(hi)
void test_amm_type_convert_real64(const char *inhex, const char *expecthex)
{
    const cace_amm_type_t *type = cace_amm_type_get_builtin(CACE_ARI_TYPE_REAL64);
    check_convert(type, inhex, expecthex);
}

TEST_CASE("F7", "F7")                                         // ari:undefined
TEST_CASE("F6", NULL)                                         // ari:null
TEST_CASE("F5", NULL)                                         // ari:true
TEST_CASE("F4", NULL)                                         // ari:false
TEST_CASE("00", NULL)                                         // ari:0
TEST_CASE("0A", NULL)                                         // ari:10
TEST_CASE("820400", NULL)                                     // ari:/INT/0
TEST_CASE("82041864", NULL)                                   // ari:/INT/100
TEST_CASE("82061864", NULL)                                   // ari:/VAST/100
TEST_CASE("FA49864700", NULL)                                 // ari:1.1e+06
TEST_CASE("8208FA49864700", NULL)                             // ari:/REAL32/1.1e+06
TEST_CASE("821181F6", NULL)                                   // ari:/AC/(undefined)
TEST_CASE("8519FFFF02200481626869", "8519FFFF02200481626869") // ari://65535/2/-1/4(hi)
void test_amm_type_convert_ident(const char *inhex, const char *expecthex)
{
    const cace_amm_type_t *type = cace_amm_type_get_builtin(CACE_ARI_TYPE_IDENT);
    check_convert(type, inhex, expecthex);
}

TEST_CASE("820C1A2B450625", "820C1A2B450625")             // ari:/TP/20230102T030405Z
TEST_CASE("82041864", "820C1864")                         // ari:/INT/100 -> ari:/TP/100
TEST_CASE("8208FB40593F34D6A161E5", "820C82231A000F68D4") // ari:/REAL64/100.9876
TEST_CASE("8209FA7F000000", NULL)                         // REAL64 exceeds INT64 MAX
TEST_CASE("8209FAEF000000", NULL)                         // REAL64 less than UINT64 MAX
TEST_CASE("820D1864", NULL)                               // ari:/TD/100
TEST_CASE("82043863", "820C3863")                         // ari:/INT/-100
TEST_CASE("8209FBC0593F34D6A161E5", "820C82233A000F68D3") // ari:/REAL64/-100.9876
TEST_CASE("8209F97C00", NULL)                             // ari:/real64/Infinity
TEST_CASE("8209F97E00", NULL)                             // ari:/real64/NaN
void test_amm_type_convert_tp(const char *inhex, const char *expecthex)
{
    const cace_amm_type_t *type = cace_amm_type_get_builtin(CACE_ARI_TYPE_TP);
    check_convert(type, inhex, expecthex);
}

TEST_CASE("820D822018CD", "820D822018CD")                 // ari:/TD/PT20.5S
TEST_CASE("82041864", "820D1864")                         // ari:/INT/100 -> ari:/TD/100
TEST_CASE("8208FB40593F34D6A161E5", "820D82231A000F68D4") // ari:/REAL64/100.9876
TEST_CASE("8209FA7F000000", NULL)                         // REAL64 exceeds INT64 MAX
TEST_CASE("8209FAEF000000", NULL)                         // REAL64 less than UINT64 MAX
TEST_CASE("820C1864", NULL)                               // ari:/TP/100
TEST_CASE("82043863", "820D3863")                         // ari:/INT/-100
TEST_CASE("8208FBC0593F34D6A161E5", "820D82233A000F68D3") // ari:/REAL32/-100.9876
TEST_CASE("8209F97C00", NULL)                             // ari:/real64/Infinity
TEST_CASE("8209F97E00", NULL)                             // ari:/real64/NaN
void test_amm_type_convert_td(const char *inhex, const char *expecthex)
{
    const cace_amm_type_t *type = cace_amm_type_get_builtin(CACE_ARI_TYPE_TD);
    check_convert(type, inhex, expecthex);
}
