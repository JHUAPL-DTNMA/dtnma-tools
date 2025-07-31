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
#include <cace/amm/semtype_cnst.h>
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

static void check_cnst(const cace_amm_semtype_cnst_t *cnst, const char *inhex, bool expect)
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

    res = cace_amm_semtype_cnst_is_valid(cnst, &val);
    TEST_ASSERT_EQUAL_INT_MESSAGE(expect, res, "cace_amm_semtype_cnst_is_valid() failed");

    cace_ari_deinit(&val);
}

TEST_CASE("F7", false)             // ari:undefined
TEST_CASE("F6", false)             // ari:null
TEST_CASE("F4", false)             // ari:false
TEST_CASE("0A", false)             // ari:10
TEST_CASE("626869", false)         // ari:"hi"
TEST_CASE("6474657374", true)      // ari:"test"
TEST_CASE("66746F6F626967", false) // ari:"toobig"
TEST_CASE("426869", false)         // ari:'hi'
TEST_CASE("4474657374", true)      // ari:'test'
TEST_CASE("46746F6F626967", false) // ari:'toobig'
void test_amm_semtype_cnst_strlen_1intvl_finite(const char *inhex, bool expect)
{
    cace_amm_semtype_cnst_t cnst;
    cace_amm_semtype_cnst_init(&cnst);
    cace_amm_range_size_t *range = cace_amm_semtype_cnst_set_strlen(&cnst);
    TEST_ASSERT_NOT_NULL(range);
    {
        cace_amm_range_intvl_size_t intvl;
        cace_amm_range_intvl_size_set_finite(&intvl, 3, 5);
        cace_amm_range_intvl_size_seq_push(range->intvls, intvl);
    }

    check_cnst(&cnst, inhex, expect);
    cace_amm_semtype_cnst_deinit(&cnst);
}

TEST_CASE("626869", false)     // ari:"hi"
TEST_CASE("6474657374", false) // ari:"test"
void test_amm_semtype_cnst_strlen_empty(const char *inhex, bool expect)
{
    cace_amm_semtype_cnst_t cnst;
    cace_amm_semtype_cnst_init(&cnst);
    cace_amm_range_size_t *range = cace_amm_semtype_cnst_set_strlen(&cnst);
    TEST_ASSERT_NOT_NULL(range);

    check_cnst(&cnst, inhex, expect);
    cace_amm_semtype_cnst_deinit(&cnst);
}

TEST_CASE("626869", true)         // ari:"hi"
TEST_CASE("6474657374", true)     // ari:"test"
TEST_CASE("66746F6F626967", true) // ari:"toobig"
void test_amm_semtype_cnst_strlen_1intvl_infinite(const char *inhex, bool expect)
{
    cace_amm_semtype_cnst_t cnst;
    cace_amm_semtype_cnst_init(&cnst);
    cace_amm_range_size_t *range = cace_amm_semtype_cnst_set_strlen(&cnst);
    TEST_ASSERT_NOT_NULL(range);
    {
        cace_amm_range_intvl_size_t intvl;
        cace_amm_range_intvl_size_set_infinite(&intvl);
        cace_amm_range_intvl_size_seq_push(range->intvls, intvl);
    }

    check_cnst(&cnst, inhex, expect);
    cace_amm_semtype_cnst_deinit(&cnst);
}

TEST_CASE("626869", true)          // ari:"hi"
TEST_CASE("6474657374", true)      // ari:"test"
TEST_CASE("66746F6F626967", false) // ari:"toobig"
void test_amm_semtype_cnst_strlen_1intvl_lowindef(const char *inhex, bool expect)
{
    cace_amm_semtype_cnst_t cnst;
    cace_amm_semtype_cnst_init(&cnst);
    cace_amm_range_size_t *range = cace_amm_semtype_cnst_set_strlen(&cnst);
    TEST_ASSERT_NOT_NULL(range);
    {
        cace_amm_range_intvl_size_t intvl;
        cace_amm_range_intvl_size_set_infinite(&intvl);
        intvl.has_max = true;
        intvl.i_max   = 5;
        cace_amm_range_intvl_size_seq_push(range->intvls, intvl);
    }

    check_cnst(&cnst, inhex, expect);
    cace_amm_semtype_cnst_deinit(&cnst);
}

TEST_CASE("626869", false)        // ari:"hi"
TEST_CASE("6474657374", true)     // ari:"test"
TEST_CASE("66746F6F626967", true) // ari:"toobig"
void test_amm_semtype_cnst_strlen_1intvl_highindef(const char *inhex, bool expect)
{
    cace_amm_semtype_cnst_t cnst;
    cace_amm_semtype_cnst_init(&cnst);
    cace_amm_range_size_t *range = cace_amm_semtype_cnst_set_strlen(&cnst);
    TEST_ASSERT_NOT_NULL(range);
    {
        cace_amm_range_intvl_size_t intvl;
        cace_amm_range_intvl_size_set_infinite(&intvl);
        intvl.has_min = true;
        intvl.i_min   = 4;
        cace_amm_range_intvl_size_seq_push(range->intvls, intvl);
    }

    check_cnst(&cnst, inhex, expect);
    cace_amm_semtype_cnst_deinit(&cnst);
}

TEST_CASE("626869", true)         // ari:"hi"
TEST_CASE("6474657374", false)    // ari:"test"
TEST_CASE("66746F6F626967", true) // ari:"toobig"
void test_amm_semtype_cnst_strlen_2intvl_finite(const char *inhex, bool expect)
{
    cace_amm_semtype_cnst_t cnst;
    cace_amm_semtype_cnst_init(&cnst);
    cace_amm_range_size_t *range = cace_amm_semtype_cnst_set_strlen(&cnst);
    TEST_ASSERT_NOT_NULL(range);
    {
        cace_amm_range_intvl_size_t intvl;
        cace_amm_range_intvl_size_set_finite(&intvl, 0, 3);
        cace_amm_range_intvl_size_seq_push(range->intvls, intvl);
        cace_amm_range_intvl_size_set_finite(&intvl, 5, 10);
        cace_amm_range_intvl_size_seq_push(range->intvls, intvl);
    }

    check_cnst(&cnst, inhex, expect);
    cace_amm_semtype_cnst_deinit(&cnst);
}

TEST_CASE("[a-z]+", "F7", false)           // ari:undefined
TEST_CASE("[a-z]+", "F6", false)           // ari:null
TEST_CASE("[a-z]+", "F4", false)           // ari:false
TEST_CASE("[a-z]+", "426869", false)       // ari:'hi'
TEST_CASE("[a-z]+", "29", false)           // ari:-10
TEST_CASE("[a-z]+", "0A", false)           // ari:10
TEST_CASE("[a-z]+", "60", false)           // ari:""
TEST_CASE("[a-z]+", "626869", true)        // ari:"hi"
TEST_CASE("[a-z]+", "6568654C4C6F", false) // ari:"heLLo"
void test_amm_semtype_cnst_textpat(const char *pat, const char *inhex, bool expect)
{
    cace_amm_semtype_cnst_t cnst;
    cace_amm_semtype_cnst_init(&cnst);
    int res = cace_amm_semtype_cnst_set_textpat(&cnst, pat);
#if defined(PCRE_FOUND)
    TEST_ASSERT_EQUAL_INT(0, res);
    check_cnst(&cnst, inhex, expect);
#else  /* PCRE_FOUND */
    TEST_ASSERT_EQUAL_INT(100, res);
#endif /* PCRE_FOUND */

    cace_amm_semtype_cnst_deinit(&cnst);
}

TEST_CASE("F7", false)     // ari:undefined
TEST_CASE("F6", false)     // ari:null
TEST_CASE("F4", false)     // ari:false
TEST_CASE("626869", false) // ari:"hi"
TEST_CASE("426869", false) // ari:'hi'
TEST_CASE("29", false)     // ari:-10
TEST_CASE("25", false)     // ari:-6
TEST_CASE("24", true)      // ari:-5
TEST_CASE("00", true)      // ari:0
TEST_CASE("05", true)      // ari:5
TEST_CASE("06", false)     // ari:6
TEST_CASE("0A", false)     // ari:10
void test_amm_semtype_cnst_range_int64_1intvl_finite(const char *inhex, bool expect)
{
    cace_amm_semtype_cnst_t cnst;
    cace_amm_semtype_cnst_init(&cnst);
    cace_amm_range_int64_t *range = cace_amm_semtype_cnst_set_range_int64(&cnst);
    TEST_ASSERT_NOT_NULL(range);
    {
        cace_amm_range_intvl_int64_t intvl;
        cace_amm_range_intvl_int64_set_finite(&intvl, -5, 5);
        cace_amm_range_intvl_int64_seq_push(range->intvls, intvl);
    }

    check_cnst(&cnst, inhex, expect);
    cace_amm_semtype_cnst_deinit(&cnst);
}

/** Mock callback for this test function.
 *
 */
bool test_amm_semtype_cnst_ident_base_callback(const cace_amm_obj_ref_t *req_base, const cace_ari_t *val, void *check_data _U_)
{
    const cace_ari_objpath_t *path = cace_ari_cget_objpath_type(val);
    if (!path)
    {
        return false;
    }
    if (!(path->has_ari_type) || (path->ari_type != CACE_ARI_TYPE_IDENT))
    {
        return false;
    }

    // Treat any ARI in the same org as valid
    return cace_ari_idseg_equal(&req_base->ref.as_ref.objpath.org_id, &path->org_id);
}

TEST_CASE("8419FFFF022004", true)            // ari://65535/2/-1/4
TEST_CASE("8419FFFF022104", false)           // ari://65535/2/-2/4
TEST_CASE("84676578616D706C65022004", false) // ari://example/2/-1/4
void test_amm_semtype_cnst_ident_base(const char *inhex, bool expect)
{
    cace_amm_semtype_cnst_t cnst;
    cace_amm_semtype_cnst_init(&cnst);
    struct cace_amm_semtype_cnst_ident_base_s *cfg = cace_amm_semtype_cnst_set_ident_base(&cnst);
    TEST_ASSERT_NOT_NULL(cfg);

    // Required base
    cace_ari_set_objref_path_intid(&(cfg->base.ref), 65535, 1, CACE_ARI_TYPE_IDENT, 40);

    // fake binding
    cfg->check      = test_amm_semtype_cnst_ident_base_callback;
    cfg->check_data = NULL;

    check_cnst(&cnst, inhex, expect);
    cace_amm_semtype_cnst_deinit(&cnst);
}
