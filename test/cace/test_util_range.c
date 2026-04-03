/*
 * Copyright (c) 2011-2026 The Johns Hopkins University Applied Physics
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
#include <cace/util/range.h>
#include <stdbool.h>
#include <unity.h>

#define TEST_CASE(...)

TEST_CASE(-11, false)
TEST_CASE(-10, true)
TEST_CASE(0, true)
TEST_CASE(6, true)
TEST_CASE(7, false)
void test_range_intvl_int64_contains(int64_t val, bool expect)
{
    cace_util_range_intvl_int64_t intvl;
    cace_util_range_intvl_int64_set_finite(&intvl, -10, 6);

    TEST_ASSERT_EQUAL(expect, cace_util_range_intvl_int64_contains(&intvl, val));
}

TEST_CASE(-11, false)
TEST_CASE(-10, true)
TEST_CASE(0, true)
TEST_CASE(6, true)
TEST_CASE(7, false)
TEST_CASE(8, true)
TEST_CASE(12, true)
TEST_CASE(13, false)
void test_range_int64_contains(int64_t val, bool expect)
{
    cace_util_range_int64_t range;
    cace_util_range_int64_init(range);
    {
        cace_util_range_intvl_int64_t intvl;
        cace_util_range_intvl_int64_set_finite(&intvl, -10, 6);
        cace_util_range_int64_push(range, intvl);
    }
    {
        cace_util_range_intvl_int64_t intvl;
        cace_util_range_intvl_int64_set_finite(&intvl, 8, 12);
        cace_util_range_int64_push(range, intvl);
    }

    TEST_ASSERT_EQUAL(expect, cace_util_range_int64_contains(range, val));

    cace_util_range_int64_clear(range);
}

void test_range_int64_equal_cmp_hash(void)
{
    cace_util_range_int64_t val_a;
    cace_util_range_int64_init(val_a);

    cace_util_range_int64_t val_b;
    cace_util_range_int64_init(val_b);

    TEST_ASSERT_TRUE(cace_util_range_int64_equal_p(val_a, val_b));
    TEST_ASSERT_EQUAL_INT(0, cace_util_range_int64_cmp(val_a, val_b));
    TEST_ASSERT_EQUAL_size_t(cace_util_range_int64_hash(val_a), cace_util_range_int64_hash(val_b));

    {
        cace_util_range_intvl_int64_t intvl;
        cace_util_range_intvl_int64_set_singleton(&intvl, 0);
        // push by value
        cace_util_range_int64_push(val_a, intvl);

        TEST_ASSERT_FALSE(cace_util_range_int64_equal_p(val_a, val_b));
        TEST_ASSERT_NOT_EQUAL_INT(0, cace_util_range_int64_cmp(val_a, val_b));
        TEST_ASSERT_NOT_EQUAL_size_t(cace_util_range_int64_hash(val_a), cace_util_range_int64_hash(val_b));

        cace_util_range_int64_push(val_b, intvl);
    }
    TEST_ASSERT_TRUE(cace_util_range_int64_equal_p(val_a, val_b));
    TEST_ASSERT_EQUAL_INT(0, cace_util_range_int64_cmp(val_a, val_b));
    TEST_ASSERT_EQUAL_size_t(cace_util_range_int64_hash(val_a), cace_util_range_int64_hash(val_b));

    {
        cace_util_range_intvl_int64_t intvl;
        cace_util_range_intvl_int64_set_singleton(&intvl, 2);
        cace_util_range_int64_push(val_a, intvl);
        cace_util_range_int64_push(val_b, intvl);
    }
    TEST_ASSERT_TRUE(cace_util_range_int64_equal_p(val_a, val_b));
    TEST_ASSERT_EQUAL_INT(0, cace_util_range_int64_cmp(val_a, val_b));
    TEST_ASSERT_EQUAL_size_t(cace_util_range_int64_hash(val_a), cace_util_range_int64_hash(val_b));

    cace_util_range_int64_clear(val_a);
    cace_util_range_int64_clear(val_b);
}

void test_range_int64_hash_alter(void)
{
    cace_util_range_int64_t range;
    cace_util_range_int64_init(range);

    size_t hash_empty = cace_util_range_int64_hash(range);
    TEST_ASSERT_EQUAL_size_t(0, hash_empty);

    {
        cace_util_range_intvl_int64_t intvl;
        cace_util_range_intvl_int64_set_singleton(&intvl, 0);
        cace_util_range_int64_push(range, intvl);
    }
    size_t hash_one = cace_util_range_int64_hash(range);
    TEST_ASSERT_NOT_EQUAL_size_t(hash_empty, hash_one);

    {
        cace_util_range_intvl_int64_t intvl;
        cace_util_range_intvl_int64_set_singleton(&intvl, 2);
        cace_util_range_int64_push(range, intvl);
    }
    size_t hash_two = cace_util_range_int64_hash(range);
    TEST_ASSERT_NOT_EQUAL_size_t(hash_empty, hash_two);
    TEST_ASSERT_NOT_EQUAL_size_t(hash_one, hash_two);

    cace_util_range_int64_clear(range);
}
