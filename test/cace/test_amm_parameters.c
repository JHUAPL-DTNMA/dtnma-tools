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
#include <cace/amm/parameters.h>
#include <cace/ari/text_util.h>
#include <cace/ari/cbor.h>
#include <unity.h>

// Allow this macro
#define TEST_CASE(...)

static void check_normalize(ari_actual_param_set_t *aparams, const ari_formal_param_list_t fparams, const char *inhex,
                            int expect_res)
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
    TEST_ASSERT_TRUE(inval.is_ref);

    res = ari_actual_param_set_populate(aparams, fparams, &(inval.as_ref.params));
    ari_deinit(&inval);
    TEST_ASSERT_EQUAL_INT_MESSAGE(expect_res, res, "ari_actual_param_set_populate() disagrees");

    // all formal parameters accounted for
    const size_t formal_size = ari_formal_param_list_size(fparams);
    TEST_ASSERT_EQUAL_INT(formal_size, ari_list_size(aparams->ordered));
    TEST_ASSERT_EQUAL_INT(formal_size, named_ari_ptr_dict_size(aparams->named));
}

TEST_CASE("83022004", 0)           // ari://2/-1/4
TEST_CASE("8402200480", 0)         // ari://2/-1/4() special case empty params
TEST_CASE("8402200481626869", 3)   // ari://2/-1/4(hi) too many params
TEST_CASE("84022004A1010A", 3)     // ari://2/-1/4(1=10) too many params
TEST_CASE("84022004A16268690A", 3) // ari://2/-1/4(hi=10) too many params
void test_fparam_empty(const char *inhex, int expect_res)
{
    ari_formal_param_list_t fparams;
    ari_formal_param_list_init(fparams);

    ari_actual_param_set_t aparams;
    ari_actual_param_set_init(&aparams);
    check_normalize(&aparams, fparams, inhex, expect_res);

    ari_actual_param_set_deinit(&aparams);
    ari_formal_param_list_clear(fparams);
}

TEST_CASE("83022004", 0)         // ari://2/-1/4
TEST_CASE("8402200480", 0)       // ari://2/-1/4() special case empty params
TEST_CASE("8402200481F5", 0)     // ari://2/-1/4(true)
TEST_CASE("8402200481626869", 0) // ari://2/-1/4(hi) implicit cast to bool
void test_fparam_one_bool(const char *inhex, int expect_res)
{
    ari_formal_param_list_t fparams;
    ari_formal_param_list_init(fparams);
    {
        ari_formal_param_t *fparam = ari_formal_param_list_push_back_new(fparams);
        fparam->index              = 0;
        string_set_str(fparam->name, "hi");
        fparam->typeobj = amm_type_get_builtin(ARI_TYPE_BOOL);

        ari_init(&fparam->defval);
    }

    ari_actual_param_set_t aparams;
    ari_actual_param_set_init(&aparams);
    check_normalize(&aparams, fparams, inhex, expect_res);

    ari_actual_param_set_deinit(&aparams);
    ari_formal_param_list_clear(fparams);
}

TEST_CASE("83022004", 0)           // ari://2/-1/4
TEST_CASE("8402200480", 0)         // ari://2/-1/4() special case empty params
TEST_CASE("840220048101", 0)       // ari://2/-1/4(1)
TEST_CASE("840220048120", 0)       // ari://2/-1/4(-1)
TEST_CASE("84022004A1000A", 0)     // ari://2/-1/4(0=10) by index
TEST_CASE("84022004A16268690A", 0) // ari://2/-1/4(hi=10) by name
TEST_CASE("8402200481626869", 2)   // ari://2/-1/4(hi) implicit cast failure
TEST_CASE("84022004820102", 3)     // ari://2/-1/4(1,2) too many params
TEST_CASE("84022004A1020A", 3)     // ari://2/-1/4(2=10) too many params
void test_fparam_one_int(const char *inhex, int expect_res)
{
    ari_formal_param_list_t fparams;
    ari_formal_param_list_init(fparams);
    {
        ari_formal_param_t *fparam = ari_formal_param_list_push_back_new(fparams);
        fparam->index              = 0;
        string_set_str(fparam->name, "hi");
        fparam->typeobj = amm_type_get_builtin(ARI_TYPE_INT);

        ari_init(&fparam->defval);
    }

    ari_actual_param_set_t aparams;
    ari_actual_param_set_init(&aparams);
    check_normalize(&aparams, fparams, inhex, expect_res);

    ari_actual_param_set_deinit(&aparams);
    ari_formal_param_list_clear(fparams);
}
