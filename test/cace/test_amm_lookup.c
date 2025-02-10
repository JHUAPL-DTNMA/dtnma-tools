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
#include <cace/amm/lookup.h>
#include <cace/amm/semtype.h>
#include <cace/ari/text_util.h>
#include <cace/ari/cbor.h>
#include <cace/util/logging.h>
#include <unity.h>

// Allow this macro
#define TEST_CASE(...)

/** Object store for all test functions.
 * @sa suiteSetUp(), suiteTearDown()
 */
static cace_amm_obj_store_t store;

void suiteSetUp(void)
{
    cace_openlog();
    cace_amm_obj_store_init(&store);

    // initial population
    cace_amm_obj_ns_t *adm = cace_amm_obj_store_add_ns(&store, cace_amm_idseg_ref_withenum("example", 65535),
                                                       cace_amm_idseg_ref_withenum("adm", 25), "2025-02-10");
    TEST_ASSERT_NOT_NULL(adm);
    cace_amm_obj_desc_t *obj;

    obj = cace_amm_obj_ns_add_obj(adm, CACE_ARI_TYPE_IDENT, cace_amm_idseg_ref_withenum("noparam", 0));
    TEST_ASSERT_NOT_NULL(obj);

    obj = cace_amm_obj_ns_add_obj(adm, CACE_ARI_TYPE_IDENT, cace_amm_idseg_ref_withenum("withparam", 1));
    TEST_ASSERT_NOT_NULL(obj);
    {
        cace_amm_formal_param_t *fparam = cace_amm_formal_param_list_push_back_new(obj->fparams);

        fparam->index = 0;
        string_set_str(fparam->name, "hi");

        cace_amm_type_set_use_direct(&(fparam->typeobj), cace_amm_type_get_builtin(CACE_ARI_TYPE_INT));
    }

    obj = cace_amm_obj_ns_add_obj(adm, CACE_ARI_TYPE_TYPEDEF, cace_amm_idseg_ref_withenum("semtype", 0));
    TEST_ASSERT_NOT_NULL(obj);
}

int suiteTearDown(int failures)
{
    cace_amm_obj_store_deinit(&store);
    cace_closelog();
    return failures;
}

static void check_lookup(const char *inhex, int expect_cbor_decode, int expect_res)
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
    TEST_ASSERT_EQUAL_INT_MESSAGE(expect_cbor_decode, res, "cace_ari_cbor_decode() failed");

    if (expect_cbor_decode)
    {
        cace_ari_deinit(&inval);
        return;
    }

    TEST_ASSERT_TRUE(inval.is_ref);

    cace_amm_lookup_t result;
    cace_amm_lookup_init(&result);
    res = cace_amm_lookup_deref(&result, &store, &inval);
    TEST_ASSERT_EQUAL_INT_MESSAGE(expect_res, res, "cace_amm_lookup_deref() disagrees");

    if (res == 0)
    {
        TEST_ASSERT_NOT_NULL(result.ns);
        TEST_ASSERT_NOT_NULL(result.obj);

        // all formal parameters accounted for
        const size_t formal_size = cace_amm_formal_param_list_size(result.obj->fparams);
        TEST_ASSERT_EQUAL_INT(formal_size, cace_ari_array_size(result.aparams.ordered));
        TEST_ASSERT_EQUAL_INT(formal_size, cace_named_ari_ptr_dict_size(result.aparams.named));
    }

    cace_amm_lookup_deinit(&result);
    cace_ari_deinit(&inval);
}

TEST_CASE("8318192000", 0, 0)     // ari://25/-1/0 found
TEST_CASE("830A2004", 0, 3)       // ari://10/-1/4 missing NS
TEST_CASE("83181939FF0004", 3, 2) // ari://25/-65281/4 unknown obj-type
TEST_CASE("8318192004", 0, 4)     // ari://25/-1/4 missing obj-id
TEST_CASE("8318192001", 0, 0)     // ari://25/-1/1 found, default parameter
TEST_CASE("8418192001810A", 0, 0) // ari://25/-1/1(10)
TEST_CASE("841819200181F6", 0, 7) // ari://25/-1/1(null) bad parameter
void test_lookup_deref(const char *inhex, int expect_cbor_decode, int expect_res)
{
    check_lookup(inhex, expect_cbor_decode, expect_res);
}
