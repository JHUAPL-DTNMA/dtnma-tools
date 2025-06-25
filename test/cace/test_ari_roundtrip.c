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
#include <cace/ari/cbor.h>
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
TEST_CASE("ari:/REAL64/Infinity")
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
TEST_CASE("ari:/AM/(3=true,10=hi,oh=4)") // AM key ordering
TEST_CASE("ari:/TBL/c=3;")
TEST_CASE("ari:/TBL/c=3;(1,2,3)")
TEST_CASE("ari:/EXECSET/n=null;()")
TEST_CASE("ari:/EXECSET/n=1234;(//example/test/CTRL/hi)")
TEST_CASE("ari:/EXECSET/n=h'6869';(//example/test/CTRL/hi,//example/test/CTRL/eh)")
TEST_CASE("ari:/RPTSET/n=1234;r=/TP/20230102T030405Z;")
TEST_CASE("ari:/RPTSET/n=1234;r=/TP/20230102T030405Z;(t=/TD/PT0S;s=//example/test/CTRL/hi;())")
TEST_CASE("ari:/RPTSET/n=1234;r=/TP/20230102T030405Z;(t=/TD/PT0S;s=//example/test/CTRL/hi;(null,3,h'6869'))")
TEST_CASE("ari://example/test/CTRL/that")
TEST_CASE("ari://example/test/CTRL/that(34)")
TEST_CASE("ari://65535/2/IDENT/4(hi)")
void test_ari_roundtrip_text_cbor(const char *intext)
{
    cace_ari_text_enc_opts_t opts = CACE_ARI_TEXT_ENC_OPTS_DEFAULT;

    cace_ari_t ari_dn;
    cace_ari_init(&ari_dn);
    {
        string_t inbuf;
        string_init_set_str(inbuf, intext);
        int res = cace_ari_text_decode(&ari_dn, inbuf, &errm);
        string_clear(inbuf);
        if (res && errm)
        {
            TEST_FAIL_MESSAGE(errm);
        }
        TEST_ASSERT_EQUAL_INT_MESSAGE(0, res, "cace_ari_text_decode() failed");
    }
    cace_ari_t ari_up;
    cace_ari_init(&ari_up);
    {
        cace_data_t buf;
        cace_data_init(&buf);
        TEST_ASSERT_EQUAL_INT_MESSAGE(0, cace_ari_cbor_encode(&buf, &ari_dn), "cace_ari_cbor_encode() failed");

        TEST_ASSERT_GREATER_THAN(0, buf.len);
        if (true)
        {
            string_t msg;
            string_init(msg);
            cace_base16_encode(msg, &buf, true);
            TEST_PRINTF("Encoded hex: %s", string_get_cstr(msg));
            string_clear(msg);
        }

        int res = cace_ari_cbor_decode(&ari_up, &buf, NULL, &errm);
        cace_data_deinit(&buf);
        if (res && errm)
        {
            TEST_FAIL_MESSAGE(errm);
        }
        TEST_ASSERT_EQUAL_INT_MESSAGE(0, res, "cace_ari_cbor_decode() failed");
    }

    {
        string_t outtext;
        string_init(outtext);
        int res = cace_ari_text_encode(outtext, &ari_dn, opts);
        TEST_ASSERT_EQUAL_INT_MESSAGE(0, res, "cace_ari_text_encode() failed");

        TEST_ASSERT_EQUAL_STRING(intext, string_get_cstr(outtext));
        string_clear(outtext);
    }

    if (true)
    {
        // optional checks
        TEST_ASSERT_EQUAL_INT_MESSAGE(cace_ari_hash(&ari_dn), cace_ari_hash(&ari_up), "ari_hash() mismatch");
        TEST_ASSERT_TRUE_MESSAGE(cace_ari_equal(&ari_dn, &ari_up), "ari_equal() mismatch");
    }

    cace_ari_deinit(&ari_up);
    cace_ari_deinit(&ari_dn);
}

// Values from draft-ietf-dtn-ari
TEST_CASE("8214841904d28519ffff01220c8af7f6f5f40a29fa497424006268696a24"
          "2e3f21272009402b3a4268698519ffff0122187b981a8200f68201f58201"
          "f482020a82043903e782051903e882063a000f423f82071a000f42408208"
          "fa501502f98209fb4415af1d78b58c40820a626869820a6a242e3f212720"
          "09402b3a820b426869820b426869820b426869820c82201b00000001df45"
          "1d83820c82201b00000001df451d83820d82211a00057e45820d82211a00"
          "057e45820e646e616d65820e1904d2820f42187b82100582118202048212"
          "a20102030482138502010203048519ffff01221904d2a2008419ffff0220"
          "0c018419ffff022b15")
void test_ari_roundtrip_cbor_text(const char *inhex)
{
    cace_ari_text_enc_opts_t opts = CACE_ARI_TEXT_ENC_OPTS_DEFAULT;

    cace_data_t indata;
    cace_data_init(&indata);
    {
        string_t inbuf;
        string_init_set_str(inbuf, inhex);
        TEST_ASSERT_EQUAL_INT(0, cace_base16_decode(&indata, inbuf));
        string_clear(inbuf);
    }

    cace_ari_t ari_dn;
    cace_ari_init(&ari_dn);
    {
        int res = cace_ari_cbor_decode(&ari_dn, &indata, NULL, &errm);
        if (res && errm)
        {
            TEST_FAIL_MESSAGE(errm);
        }
        TEST_ASSERT_EQUAL_INT_MESSAGE(0, res, "cace_ari_cbor_decode() failed");
    }

    cace_ari_t ari_up;
    cace_ari_init(&ari_up);
    {
        string_t text;
        string_init(text);
        int res = cace_ari_text_encode(text, &ari_dn, opts);
        TEST_ASSERT_EQUAL_INT_MESSAGE(0, res, "cace_ari_text_encode() failed");

        TEST_ASSERT_GREATER_THAN(0, m_string_size(text));
        if (true)
        {
            TEST_PRINTF("Encoded text: %s", string_get_cstr(text));
        }

        res = cace_ari_text_decode(&ari_up, text, &errm);
        string_clear(text);
        if (res && errm)
        {
            TEST_FAIL_MESSAGE(errm);
        }
        TEST_ASSERT_EQUAL_INT_MESSAGE(0, res, "cace_ari_text_decode() failed");
    }

    cace_data_t outdata;
    cace_data_init(&outdata);
    {
        TEST_ASSERT_EQUAL_INT_MESSAGE(0, cace_ari_cbor_encode(&outdata, &ari_up), "cace_ari_cbor_encode() failed");

        TEST_ASSERT_EQUAL_INT(indata.len, outdata.len);
        TEST_ASSERT_EQUAL_MEMORY(indata.ptr, outdata.ptr, indata.len);
    }

    if (true)
    {
        // optional checks
        TEST_ASSERT_EQUAL_INT_MESSAGE(cace_ari_hash(&ari_dn), cace_ari_hash(&ari_up), "ari_hash() mismatch");
        TEST_ASSERT_TRUE_MESSAGE(cace_ari_equal(&ari_dn, &ari_up), "ari_equal() mismatch");
    }

    cace_ari_deinit(&ari_up);
    cace_ari_deinit(&ari_dn);
    cace_data_deinit(&outdata);
    cace_data_deinit(&indata);
}

#endif /* ARI_TEXT_PARSE */
