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
#include <cace/amm/parameters.h>
#include <cace/amm/semtype.h>
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

static void check_normalize(cace_ari_itemized_t *aparams, const cace_amm_formal_param_list_t fparams, const char *inhex,
                            int expect_res)
{
    m_string_t intext;
    m_string_init_set_cstr(intext, inhex);
    cace_data_t indata;
    cace_data_init(&indata);
    int res = cace_base16_decode(&indata, intext);
    m_string_clear(intext);
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, res, "cace_base16_decode() failed");

    cace_ari_t inval = CACE_ARI_INIT_UNDEFINED;
    res              = cace_ari_cbor_decode(&inval, &indata, NULL, NULL);
    cace_data_deinit(&indata);
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, res, "cace_ari_cbor_decode() failed");
    TEST_ASSERT_TRUE(inval.is_ref);

    res = cace_amm_actual_param_set_populate(aparams, fparams, &(inval.as_ref.params));
    cace_ari_deinit(&inval);
    TEST_ASSERT_EQUAL_INT_MESSAGE(expect_res, res, "cace_amm_actual_param_set_populate() disagrees");

    // all formal parameters accounted for
    const size_t formal_size = cace_amm_formal_param_list_size(fparams);
    TEST_ASSERT_EQUAL_INT(formal_size, cace_ari_array_size(aparams->ordered));
    TEST_ASSERT_EQUAL_INT(formal_size, cace_named_ari_ptr_dict_size(aparams->named));
}

TEST_CASE("8419FFFF022004", 0)           // ari://65535/2/-1/4
TEST_CASE("8519FFFF02200480", 0)         // ari://65535/2/-1/4() special case empty params
TEST_CASE("8519FFFF02200481626869", 3)   // ari://65535/2/-1/4(hi) too many params
TEST_CASE("8519FFFF022004A1010A", 3)     // ari://65535/2/-1/4(1=10) too many params
TEST_CASE("8519FFFF022004A16268690A", 3) // ari://65535/2/-1/4(hi=10) too many params
void test_fparam_empty(const char *inhex, int expect_res)
{
    cace_amm_formal_param_list_t fparams;
    cace_amm_formal_param_list_init(fparams);

    cace_ari_itemized_t aparams;
    cace_ari_itemized_init(&aparams);
    check_normalize(&aparams, fparams, inhex, expect_res);
    TEST_ASSERT_EQUAL(false, aparams.any_undefined);

    cace_ari_itemized_deinit(&aparams);
    cace_amm_formal_param_list_clear(fparams);
}

TEST_CASE("8419FFFF022004", 0)         // ari://65535/2/-1/4
TEST_CASE("8519FFFF02200480", 0)       // ari://65535/2/-1/4() special case empty params
TEST_CASE("8519FFFF02200481F5", 0)     // ari://65535/2/-1/4(true)
TEST_CASE("8519FFFF02200481626869", 0) // ari://65535/2/-1/4(hi) implicit cast to bool
TEST_CASE("8519FFFF02200481F6", 0)     // ari://65535/2/-1/4(null) failing cast to bool
void test_fparam_one_bool(const char *inhex, int expect_res)
{
    cace_amm_formal_param_list_t fparams;
    cace_amm_formal_param_list_init(fparams);
    {
        cace_amm_formal_param_t *fparam = cace_amm_formal_param_list_push_back_new(fparams);

        fparam->index = 0;
        m_string_set_cstr(fparam->name, "hi");
        TEST_ASSERT_EQUAL_INT(0, cace_amm_type_set_use_builtin(&(fparam->typeobj), CACE_ARI_TYPE_BOOL));
        cace_ari_set_bool(&(fparam->defval), false);
    }

    cace_ari_itemized_t aparams;
    cace_ari_itemized_init(&aparams);
    check_normalize(&aparams, fparams, inhex, expect_res);
    TEST_ASSERT_EQUAL(false, aparams.any_undefined);

    cace_ari_itemized_deinit(&aparams);
    cace_amm_formal_param_list_clear(fparams);
}

TEST_CASE("8419FFFF022004", 0, true)          // ari://65535/2/-1/4
TEST_CASE("8519FFFF02200480", 0, true)        // ari://65535/2/-1/4() special case empty params
TEST_CASE("8519FFFF02200481F5", 0, false)     // ari://65535/2/-1/4(true)
TEST_CASE("8519FFFF02200481626869", 0, false) // ari://65535/2/-1/4(hi) implicit cast to bool
TEST_CASE("8519FFFF02200481F6", 0, false)     // ari://65535/2/-1/4(null) implicit cast to bool
void test_fparam_one_bool_nodefault(const char *inhex, int expect_res, bool expect_undefined)
{
    cace_amm_formal_param_list_t fparams;
    cace_amm_formal_param_list_init(fparams);
    {
        cace_amm_formal_param_t *fparam = cace_amm_formal_param_list_push_back_new(fparams);

        fparam->index = 0;
        m_string_set_cstr(fparam->name, "hi");
        TEST_ASSERT_EQUAL_INT(0, cace_amm_type_set_use_builtin(&(fparam->typeobj), CACE_ARI_TYPE_BOOL));
    }

    cace_ari_itemized_t aparams;
    cace_ari_itemized_init(&aparams);
    check_normalize(&aparams, fparams, inhex, expect_res);
    TEST_ASSERT_EQUAL(expect_undefined, aparams.any_undefined);

    cace_ari_itemized_deinit(&aparams);
    cace_amm_formal_param_list_clear(fparams);
}

TEST_CASE("8419FFFF022004", 0)           // ari://65535/2/-1/4
TEST_CASE("8519FFFF02200480", 0)         // ari://65535/2/-1/4() special case empty params
TEST_CASE("8519FFFF0220048101", 0)       // ari://65535/2/-1/4(1)
TEST_CASE("8519FFFF0220048120", 0)       // ari://65535/2/-1/4(-1)
TEST_CASE("8519FFFF022004A1000A", 0)     // ari://65535/2/-1/4(0=10) by index
TEST_CASE("8519FFFF022004A16268690A", 0) // ari://65535/2/-1/4(hi=10) by name
TEST_CASE("8519FFFF02200481626869", 2)   // ari://65535/2/-1/4(hi) implicit cast failure
TEST_CASE("8519FFFF022004820102", 3)     // ari://65535/2/-1/4(1,2) too many params
TEST_CASE("8519FFFF022004A1020A", 3)     // ari://65535/2/-1/4(2=10) too many params
void test_fparam_one_int(const char *inhex, int expect_res)
{
    cace_amm_formal_param_list_t fparams;
    cace_amm_formal_param_list_init(fparams);
    {
        cace_amm_formal_param_t *fparam = cace_amm_formal_param_list_push_back_new(fparams);

        fparam->index = 0;
        m_string_set_cstr(fparam->name, "hi");
        TEST_ASSERT_EQUAL_INT(0, cace_amm_type_set_use_builtin(&(fparam->typeobj), CACE_ARI_TYPE_INT));
        cace_ari_set_int(&(fparam->defval), 10);
    }

    cace_ari_itemized_t aparams;
    cace_ari_itemized_init(&aparams);
    check_normalize(&aparams, fparams, inhex, expect_res);
    TEST_ASSERT_EQUAL(false, aparams.any_undefined);

    cace_ari_itemized_deinit(&aparams);
    cace_amm_formal_param_list_clear(fparams);
}

TEST_CASE("8519FFFF012205818419FFFF012301", 0)                     // ari://65535/2/-1/4(//65535/1/EDD/1)
TEST_CASE("8519FFFF012205818419FFFF01236A73775F76657273696F6E", 0) // ari://65535/2/-1/4(//65535/1/EDD/sw_version)
void test_fparam_one_object(const char *inhex, int expect_res)
{
    cace_amm_formal_param_list_t fparams;
    cace_amm_formal_param_list_init(fparams);
    {
        cace_amm_formal_param_t *fparam = cace_amm_formal_param_list_push_back_new(fparams);

        fparam->index = 0;
        m_string_set_cstr(fparam->name, "ref");
        TEST_ASSERT_EQUAL_INT(0, cace_amm_type_set_use_builtin(&(fparam->typeobj), CACE_ARI_TYPE_OBJECT));
    }

    cace_ari_itemized_t aparams;
    cace_ari_itemized_init(&aparams);
    check_normalize(&aparams, fparams, inhex, expect_res);
    TEST_ASSERT_EQUAL(false, aparams.any_undefined);

    cace_ari_itemized_deinit(&aparams);
    cace_amm_formal_param_list_clear(fparams);
}
