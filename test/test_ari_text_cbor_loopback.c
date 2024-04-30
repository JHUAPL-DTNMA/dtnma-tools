/*
 * Copyright (c) 2011-2023 The Johns Hopkins University Applied Physics
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
#include <ari/ari_text.h>
#include <ari/ari_text_util.h>
#include <ari/ari_cbor.h>
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

#if defined(ARI_TEXT_PARSE)

TEST_CASE("ari:undefined")
TEST_CASE("ari:null")
TEST_CASE("ari:true")
TEST_CASE("ari:false")
TEST_CASE("ari:1234")
TEST_CASE("ari:1.1e+06")
TEST_CASE("ari:1.1e+10")
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
TEST_CASE("ari:/TEXTSTR/%22hi%20there%22")
TEST_CASE("ari:/LABEL/hi")
TEST_CASE("ari:/TP/20230102T030405Z")
TEST_CASE("ari:/TD/PT20.5S")
TEST_CASE("ari:/AC/()")
TEST_CASE("ari:/AC/(null,/INT/23)")
TEST_CASE("ari:/AM/()")
TEST_CASE("ari:/AM/(1=true)")
TEST_CASE("ari:/TBL/c=3;")
TEST_CASE("ari:/TBL/c=3;(1,2,3)")
TEST_CASE("ari:/EXECSET/n=null;()")
TEST_CASE("ari:/EXECSET/n=1234;(//test/CTRL/hi)")
TEST_CASE("ari:/EXECSET/n=h'6869';(//test/CTRL/hi,//test/CTRL/eh)")
TEST_CASE("ari:/RPTSET/n=1234;r=/TP/20230102T030405Z;")
TEST_CASE("ari:/RPTSET/n=1234;r=/TP/20230102T030405Z;(t=/TD/PT0S;s=//test/CTRL/hi;())")
TEST_CASE("ari:/RPTSET/n=1234;r=/TP/20230102T030405Z;(t=/TD/PT0S;s=//test/CTRL/hi;(null,3,h'6869'))")
TEST_CASE("ari://test/this/that")
TEST_CASE("ari://test/this/that(34)")
TEST_CASE("ari://2/IDENT/4(hi)")
void test_ari_text_loopback(const char *intext)
{
    ari_text_enc_opts_t opts = ARI_TEXT_ENC_OPTS_DEFAULT;

    ari_t ari_dn;
    ari_init(&ari_dn);
    {
        string_t inbuf;
        string_init_set_str(inbuf, intext);
        int res = ari_text_decode(&ari_dn, inbuf, &errm);
        string_clear(inbuf);
        if (res && errm)
        {
            TEST_FAIL_MESSAGE(errm);
        }
        TEST_ASSERT_EQUAL_INT_MESSAGE(0, res, "ari_text_decode() failed");
    }
    ari_t ari_up;
    ari_init(&ari_up);
    {
        ari_data_t buf;
        ari_data_init(&buf);
        TEST_ASSERT_EQUAL_INT_MESSAGE(0, ari_cbor_encode(&buf, &ari_dn), "ari_cbor_encode() failed");

        TEST_ASSERT_GREATER_THAN(0, buf.len);
        if (true)
        {
            string_t msg;
            string_init_printf(msg, "Encoded hex: ");
            base16_encode(msg, &buf, true);
            TEST_MESSAGE(string_get_cstr(msg));
            string_clear(msg);
        }

        int res = ari_cbor_decode(&ari_up, &buf, NULL, &errm);
        ari_data_deinit(&buf);
        if (res && errm)
        {
            TEST_FAIL_MESSAGE(errm);
        }
        TEST_ASSERT_EQUAL_INT_MESSAGE(0, res, "ari_cbor_decode() failed");
    }

    {
        string_t outtext;
        string_init(outtext);
        int res = ari_text_encode(outtext, &ari_dn, opts);
        TEST_ASSERT_EQUAL_INT_MESSAGE(0, res, "ari_text_encode() failed");

        TEST_ASSERT_EQUAL_STRING(intext, string_get_cstr(outtext));
        string_clear(outtext);
    }

    if (true)
    {
        // optional checks
        TEST_ASSERT_EQUAL_INT_MESSAGE(ari_hash(&ari_dn), ari_hash(&ari_up), "ari_hash() mismatch");
        TEST_ASSERT_TRUE_MESSAGE(ari_equal(&ari_dn, &ari_up), "ari_equal() mismatch");
    }

    ari_deinit(&ari_up);
    ari_deinit(&ari_dn);
}

#endif /* ARI_TEXT_PARSE */
