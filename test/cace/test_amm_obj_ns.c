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
#include <cace/amm/obj_ns.h>
#include <cace/ari/text_util.h>
#include <cace/ari/cbor.h>
#include <cace/util/logging.h>
#include <cace/ari/text.h>
#include <cace/util/defs.h>
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

void test_obj_ns_find_obj(void)
{
    cace_amm_obj_ns_t ns;
    cace_amm_obj_ns_init(&ns);

    cace_amm_obj_desc_t *ident1 =
        cace_amm_obj_ns_add_obj(&ns, CACE_ARI_TYPE_IDENT, cace_amm_idseg_ref_withenum("name", 1));
    TEST_ASSERT_NOT_NULL(ident1);

    cace_amm_obj_desc_t *ident2 =
        cace_amm_obj_ns_add_obj(&ns, CACE_ARI_TYPE_IDENT, cace_amm_idseg_ref_withenum("other", 2));
    TEST_ASSERT_NOT_NULL(ident2);

    cace_amm_obj_desc_t *found;
    found = cace_amm_obj_ns_find_obj_enum(&ns, CACE_ARI_TYPE_CONST, 0);
    TEST_ASSERT_NULL(found);
    found = cace_amm_obj_ns_find_obj_name(&ns, CACE_ARI_TYPE_CONST, "asdf");
    TEST_ASSERT_NULL(found);
    found = cace_amm_obj_ns_find_obj_enum(&ns, CACE_ARI_TYPE_IDENT, 0);
    TEST_ASSERT_NULL(found);
    found = cace_amm_obj_ns_find_obj_name(&ns, CACE_ARI_TYPE_IDENT, "asdf");
    TEST_ASSERT_NULL(found);

    found = cace_amm_obj_ns_find_obj_enum(&ns, CACE_ARI_TYPE_IDENT, 1);
    TEST_ASSERT_NOT_NULL(found);
    TEST_ASSERT_EQUAL(ident1, found);
    found = cace_amm_obj_ns_find_obj_name(&ns, CACE_ARI_TYPE_IDENT, "name");
    TEST_ASSERT_NOT_NULL(found);
    TEST_ASSERT_EQUAL(ident1, found);

    found = cace_amm_obj_ns_find_obj_enum(&ns, CACE_ARI_TYPE_IDENT, 4);
    TEST_ASSERT_NULL(found);

    cace_amm_obj_ns_deinit(&ns);
}

void test_obj_ns_add_obj_duplicate(void)
{
    cace_amm_obj_ns_t ns;
    cace_amm_obj_ns_init(&ns);

    cace_amm_obj_desc_t *ident1 =
        cace_amm_obj_ns_add_obj(&ns, CACE_ARI_TYPE_IDENT, cace_amm_idseg_ref_withenum("name", 1));
    TEST_ASSERT_NOT_NULL(ident1);

    cace_amm_obj_desc_t *dupe;
    dupe = cace_amm_obj_ns_add_obj(&ns, CACE_ARI_TYPE_IDENT, cace_amm_idseg_ref_withenum("name", 2));
    TEST_ASSERT_NULL(dupe);
    dupe = cace_amm_obj_ns_add_obj(&ns, CACE_ARI_TYPE_IDENT, cace_amm_idseg_ref_noenum("name"));
    TEST_ASSERT_NULL(dupe);
    dupe = cace_amm_obj_ns_add_obj(&ns, CACE_ARI_TYPE_IDENT, cace_amm_idseg_ref_withenum("other", 1));
    TEST_ASSERT_NULL(dupe);

    cace_amm_obj_ns_deinit(&ns);
}
