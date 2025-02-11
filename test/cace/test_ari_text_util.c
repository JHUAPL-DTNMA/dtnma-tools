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
 * Test the ari_text_util.h interfaces.
 */
#include <cace/ari/text_util.h>
#include <unity.h>
#include <string.h>

#define TEST_CASE(...)

TEST_CASE("", false) // no first character
TEST_CASE("asdf", true)
TEST_CASE("123", false) // bad first character
TEST_CASE("abc123", true)
TEST_CASE("_hi", true)
TEST_CASE("_hi.-", true)
TEST_CASE("-_hi.-", false) // bad first character
TEST_CASE("hi&", false)    // bad later character
void test_cace_ari_text_is_identity(const char *text, bool expect)
{
    cace_data_t in_text;
    cace_data_init_view(&in_text, strlen(text) + 1, (cace_data_ptr_t)text);

    TEST_ASSERT_EQUAL(expect, cace_ari_text_is_identity(&in_text));

    cace_data_deinit(&in_text);
}

TEST_CASE("", "", "")
TEST_CASE("hi", "", "hi")
TEST_CASE("h i", "", "h%20i")
TEST_CASE("h$i", "", "h%24i")
TEST_CASE("h$i", "$", "h$i")
void test_cace_uri_percent_encode_valid(const char *text, const char *safe, const char *expect)
{
    cace_data_t in_text;
    cace_data_init_view(&in_text, strlen(text) + 1, (cace_data_ptr_t)text);

    string_t out_text;
    string_init(out_text);
    TEST_ASSERT_EQUAL_INT(0, cace_uri_percent_encode(out_text, &in_text, safe));

    TEST_ASSERT_EQUAL_STRING(expect, string_get_cstr(out_text));

    string_clear(out_text);
    cace_data_deinit(&in_text);
}

TEST_CASE("", "")
TEST_CASE("hi", "hi")
TEST_CASE("h%20i", "h i")
TEST_CASE("h%7ei", "h~i")
void test_cace_uri_percent_decode_valid(const char *text, const char *expect)
{
    cace_data_t in_text;
    cace_data_init_view(&in_text, strlen(text) + 1, (cace_data_ptr_t)text);

    string_t out_text;
    string_init(out_text);
    TEST_ASSERT_EQUAL_INT(0, cace_uri_percent_decode(out_text, &in_text));

    TEST_ASSERT_EQUAL_STRING(expect, string_get_cstr(out_text));

    string_clear(out_text);
    cace_data_deinit(&in_text);
}

TEST_CASE("%")
TEST_CASE("%1")
void test_cace_uri_percent_decode_invalid(const char *text)
{
    cace_data_t in_data;
    cace_data_init_view(&in_data, strlen(text) + 1, (cace_data_ptr_t)text);

    string_t out_text;
    string_init(out_text);
    TEST_ASSERT_NOT_EQUAL_INT(0, cace_uri_percent_decode(out_text, &in_data));
    string_clear(out_text);
    cace_data_deinit(&in_data);
}

TEST_CASE("", '"', "")
TEST_CASE("hi", '"', "hi")
TEST_CASE("h\"i", '"', "h\\\"i")
TEST_CASE("h'i", '"', "h'i")
TEST_CASE("h \b\f\n\r\ti", '\"', "h \\b\\f\\n\\r\\ti")
TEST_CASE("hi\u1234", '"', "hi\\u1234")
TEST_CASE("hi\U0001D11E", '"', "hi\\uD834\\uDD1E")
TEST_CASE("h'i", '\'', "h\\'i")
TEST_CASE("hi\u1234", '\'', "hi\\u1234")
void test_cace_slash_escape_valid(const char *text, const char quote, const char *expect)
{
    cace_data_t in_text;
    cace_data_init_view(&in_text, strlen(text) + 1, (cace_data_ptr_t)text);

    string_t out_text;
    string_init(out_text);
    TEST_ASSERT_EQUAL_INT(0, cace_slash_escape(out_text, &in_text, quote));

    if (expect)
    {
        TEST_ASSERT_EQUAL_STRING(expect, string_get_cstr(out_text));
    }
    else
    {
        TEST_ASSERT_EQUAL_INT(0, string_size(out_text));
    }
    string_clear(out_text);
    cace_data_deinit(&in_text);
}

TEST_CASE("", NULL)
TEST_CASE("hi", "hi")
TEST_CASE("h\\'i", "h'i")
TEST_CASE("h\\\"i", "h\"i")
TEST_CASE("h \\b\\f\\n\\r\\ti", "h \b\f\n\r\ti")
TEST_CASE("hi\\u1234", "hi\u1234")
TEST_CASE("hi\\uD834\\uDD1E", "hi\U0001D11E")
void test_cace_slash_unescape_valid(const char *text, const char *expect)
{
    cace_data_t in_text;
    cace_data_init_view(&in_text, strlen(text) + 1, (cace_data_ptr_t)text);

    string_t out_text;
    string_init(out_text);
    TEST_ASSERT_EQUAL_INT(0, cace_slash_unescape(out_text, &in_text));

    if (expect)
    {
        TEST_ASSERT_EQUAL_STRING(expect, string_get_cstr(out_text));
    }
    else
    {
        TEST_ASSERT_EQUAL_INT(0, string_size(out_text));
    }
    string_clear(out_text);
    cace_data_deinit(&in_text);
}

TEST_CASE("\\")
void test_cace_slash_unescape_invalid(const char *text)
{
    cace_data_t in_data;
    cace_data_init_view(&in_data, strlen(text) + 1, (cace_data_ptr_t)text);

    string_t out_text;
    string_init(out_text);
    TEST_ASSERT_NOT_EQUAL_INT(0, cace_slash_unescape(out_text, &in_data));
    string_clear(out_text);
    cace_data_deinit(&in_data);
}

TEST_CASE("", 0, false, "")
TEST_CASE("hi", 2, false, "6869")
void test_cace_base16_encode(const char *data, size_t data_len, bool uppercase, const char *expect)
{
    cace_data_t src;
    cace_data_init_view(&src, data_len, (cace_data_ptr_t)data);

    string_t out;
    string_init(out);
    TEST_ASSERT_EQUAL_INT(0, cace_base16_encode(out, &src, uppercase));

    TEST_ASSERT_EQUAL_STRING(expect, string_get_cstr(out));
    string_clear(out);
    cace_data_deinit(&src);
}

TEST_CASE("", NULL, 0)
TEST_CASE("00", "\x00", 1)
TEST_CASE("6869", "hi", 2)
void test_cace_base16_decode_valid(const char *text, const char *expect, size_t expect_len)
{
    string_t in_text;
    string_init_set_str(in_text, text);
    cace_data_t out_data;
    cace_data_init(&out_data);
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, cace_base16_decode(&out_data, in_text), "cace_base16_decode() failed");

    if (expect)
    {
        cace_data_t expect_data;
        cace_data_init_view(&expect_data, expect_len, (cace_data_ptr_t)expect);
        TEST_ASSERT_TRUE(out_data.owned);
        TEST_ASSERT_EQUAL_INT(expect_data.len, out_data.len);
        TEST_ASSERT_EQUAL_MEMORY(expect_data.ptr, out_data.ptr, out_data.len);
        cace_data_deinit(&expect_data);
    }
    else
    {
        TEST_ASSERT_FALSE(out_data.owned);
        TEST_ASSERT_EQUAL_INT(0, out_data.len);
        TEST_ASSERT_NULL(out_data.ptr);
    }
    cace_data_deinit(&out_data);
    string_clear(in_text);
}

TEST_CASE("1")
TEST_CASE("asd")
void test_cace_base16_decode_invalid(const char *text)
{
    string_t in_text;
    string_init_set_str(in_text, text);
    cace_data_t out_data;
    cace_data_init(&out_data);
    TEST_ASSERT_NOT_EQUAL_INT(0, cace_base16_decode(&out_data, in_text));
    cace_data_deinit(&out_data);
    string_clear(in_text);
}

// vectors from Section 10 of RFC 4648
TEST_CASE("", 0, false, "")
TEST_CASE("f", 1, false, "Zg==")
TEST_CASE("fo", 2, false, "Zm8=")
TEST_CASE("foo", 3, false, "Zm9v")
TEST_CASE("foob", 4, false, "Zm9vYg==")
TEST_CASE("fooba", 5, false, "Zm9vYmE=")
TEST_CASE("foobar", 6, false, "Zm9vYmFy")
// example from Section 9 of RFC 4648
TEST_CASE("\x14\xfb\x9c\x03\xd9\x7e", 6, false, "FPucA9l+")
// random cases to use last two characters of alphabet
TEST_CASE("\xc1\x04\xc4\xcf\xb7\x77\x0f\xf0\xbe\xba\xa2\xe9\x5f\xbc\x2c\x18", 16, false, "wQTEz7d3D/C+uqLpX7wsGA==")
TEST_CASE("\xc1\x04\xc4\xcf\xb7\x77\x0f\xf0\xbe\xba\xa2\xe9\x5f\xbc\x2c\x18", 16, true, "wQTEz7d3D_C-uqLpX7wsGA==")
void test_cace_base64_encode(const char *data, size_t data_len, bool useurl, const char *expect)
{
    cace_data_t src;
    cace_data_init_view(&src, data_len, (cace_data_ptr_t)data);

    string_t out;
    string_init(out);
    TEST_ASSERT_EQUAL_INT(0, cace_base64_encode(out, &src, useurl));

    TEST_ASSERT_EQUAL_STRING(expect, string_get_cstr(out));
    string_clear(out);
    cace_data_deinit(&src);
}

TEST_CASE("", NULL, 0)
TEST_CASE("Zg==", "f", 1)
TEST_CASE("Zm8=", "fo", 2)
TEST_CASE("Zm9v", "foo", 3)
TEST_CASE("Zm9vYg==", "foob", 4)
TEST_CASE("Zm9vYmE=", "fooba", 5)
TEST_CASE("Zm9vYmFy", "foobar", 6)
// excess padding
TEST_CASE("Zm9vYmFy====", "foobar", 6)
TEST_CASE("====", NULL, 0)
// random cases to use last two characters of alphabet
TEST_CASE("wQTEz7d3D/C+uqLpX7wsGA==", "\xc1\x04\xc4\xcf\xb7\x77\x0f\xf0\xbe\xba\xa2\xe9\x5f\xbc\x2c\x18", 16)
TEST_CASE("wQTEz7d3D_C-uqLpX7wsGA==", "\xc1\x04\xc4\xcf\xb7\x77\x0f\xf0\xbe\xba\xa2\xe9\x5f\xbc\x2c\x18", 16)
void test_cace_base64_decode_valid(const char *text, const char *expect, size_t expect_len)
{
    string_t in_text;
    string_init_set_str(in_text, text);
    cace_data_t out_data;
    cace_data_init(&out_data);
    TEST_ASSERT_EQUAL_INT(0, cace_base64_decode(&out_data, in_text));

    if (expect)
    {
        cace_data_t expect_data;
        cace_data_init_view(&expect_data, expect_len, (cace_data_ptr_t)expect);
        TEST_ASSERT_TRUE(out_data.owned);
        TEST_ASSERT_EQUAL_INT(expect_data.len, out_data.len);
        TEST_ASSERT_EQUAL_MEMORY(expect_data.ptr, out_data.ptr, out_data.len);
        cace_data_deinit(&expect_data);
    }
    else
    {
        TEST_ASSERT_FALSE(out_data.owned);
        TEST_ASSERT_EQUAL_INT(0, out_data.len);
        TEST_ASSERT_NULL(out_data.ptr);
    }
    cace_data_deinit(&out_data);
    string_clear(in_text);
}

TEST_CASE("AB")
void test_cace_base64_decode_invalid(const char *text)
{
    string_t in_text;
    string_init_set_str(in_text, text);
    cace_data_t out_data;
    cace_data_init(&out_data);
    TEST_ASSERT_NOT_EQUAL_INT(0, cace_base64_decode(&out_data, in_text));
    cace_data_deinit(&out_data);
    string_clear(in_text);
}

TEST_CASE(20, 0, true, "2000-01-01T00:00:20Z")
TEST_CASE(20, 0, false, "20000101T000020Z")
TEST_CASE(20, 1e3, false, "20000101T000020.000001Z")
TEST_CASE(20, 50e6, false, "20000101T000020.05Z")
TEST_CASE(20, 999999000, false, "20000101T000020.999999Z")
TEST_CASE(20, 999999999, false, "20000101T000020.999999999Z") // really valid?
void test_cace_utctime_encode_valid(time_t in_sec, long in_nsec, bool usesep, const char *expect)
{
    struct timespec in = { .tv_sec = in_sec, .tv_nsec = in_nsec };

    string_t out_text;
    string_init(out_text);
    TEST_ASSERT_EQUAL_INT(0, cace_utctime_encode(out_text, &in, usesep));
    TEST_ASSERT_EQUAL_STRING(expect, string_get_cstr(out_text));

    string_clear(out_text);
}

TEST_CASE("2000-01-01T00:00:20Z", 20, 0)
TEST_CASE("20000101T000020Z", 20, 0)
TEST_CASE("20000101T000020.000000Z", 20, 0)
TEST_CASE("20000101T000020.05Z", 20, 50e6)
TEST_CASE("20000101T000020.000001Z", 20, 1e3)
TEST_CASE("20000101T010020Z", 3620, 0)
TEST_CASE("20200101T000000Z", 631152000, 0)
void test_cace_utctime_decode_valid(const char *text, time_t expect_sec, long expect_nsec)
{
    cace_data_t in_data;
    cace_data_init_view(&in_data, strlen(text) + 1, (cace_data_ptr_t)text);

    struct timespec out;
    TEST_ASSERT_EQUAL_INT(0, cace_utctime_decode(&out, &in_data));

    struct timespec expect = { .tv_sec = expect_sec, .tv_nsec = expect_nsec };
    TEST_ASSERT_EQUAL(expect.tv_sec, out.tv_sec);
    TEST_ASSERT_EQUAL(expect.tv_nsec, out.tv_nsec);

    cace_data_deinit(&in_data);
}

TEST_CASE("test")
TEST_CASE("20200101")
TEST_CASE("20200101T000000")
TEST_CASE("20200101T000000Zother")
void test_cace_utctime_decode_invalid(const char *text)
{
    cace_data_t in_data;
    cace_data_init_view(&in_data, strlen(text) + 1, (cace_data_ptr_t)text);

    struct timespec out;
    TEST_ASSERT_NOT_EQUAL_INT(0, cace_utctime_decode(&out, &in_data));

    cace_data_deinit(&in_data);
}

TEST_CASE(0, 0, "PT0S")
TEST_CASE(20, 0, "PT20S")
TEST_CASE(20, 1e3, "PT20.000001S")
TEST_CASE(20, 50e6, "PT20.05S")
TEST_CASE(20, 999999000, "PT20.999999S")
TEST_CASE(3610, 0, "PT1H10S")
TEST_CASE(24 * 3600 + 2 * 3600 + 3 * 60 + 4, 500e6, "P1DT2H3M4.5S")
void test_cace_timeperiod_encode_valid(time_t in_sec, long in_nsec, const char *expect)
{
    struct timespec in = { .tv_sec = in_sec, .tv_nsec = in_nsec };

    string_t out_text;
    string_init(out_text);
    TEST_ASSERT_EQUAL_INT(0, cace_timeperiod_encode(out_text, &in));
    TEST_ASSERT_EQUAL_STRING(expect, string_get_cstr(out_text));

    string_clear(out_text);
}

TEST_CASE("PT", 0, 0)
TEST_CASE("PT0S", 0, 0)
TEST_CASE("PT10S", 10, 0)
TEST_CASE("-PT10S", -10, 0)
TEST_CASE("PT1M", 60, 0)
TEST_CASE("PT1H", 3600, 0)
TEST_CASE("P1DT", 24 * 3600, 0)
TEST_CASE("P1DT0.1S", 24 * 3600, 100e6)
TEST_CASE("P1DT2H3M4.5S", 24 * 3600 + 2 * 3600 + 3 * 60 + 4, 500e6)
void test_cace_timeperiod_decode_valid(const char *text, time_t expect_sec, long expect_nsec)
{
    cace_data_t in_data;
    cace_data_init_view(&in_data, strlen(text) + 1, (cace_data_ptr_t)text);

    struct timespec out;
    TEST_ASSERT_EQUAL_INT(0, cace_timeperiod_decode(&out, &in_data));

    struct timespec expect = { .tv_sec = expect_sec, .tv_nsec = expect_nsec };
    TEST_ASSERT_EQUAL(expect.tv_sec, out.tv_sec);
    TEST_ASSERT_EQUAL(expect.tv_nsec, out.tv_nsec);

    cace_data_deinit(&in_data);
}

TEST_CASE("test")
TEST_CASE("20200101")
TEST_CASE("PT0Sother")
void test_cace_timeperiod_decode_invalid(const char *text)
{
    cace_data_t in_data;
    cace_data_init_view(&in_data, strlen(text) + 1, (cace_data_ptr_t)text);

    struct timespec out;
    TEST_ASSERT_NOT_EQUAL_INT(0, cace_timeperiod_decode(&out, &in_data));

    cace_data_deinit(&in_data);
}

TEST_CASE(0, 'f', "0.000000")
TEST_CASE(0, 'g', "0")
TEST_CASE(0, 'e', "0.000000e+00")
TEST_CASE(0, 'a', "0x0p+0")
TEST_CASE(10, 'f', "10.000000")
TEST_CASE(10, 'g', "10")
TEST_CASE(10, 'e', "1.000000e+01")
TEST_CASE(10, 'a', "0x1.4p+3")
TEST_CASE(1.1e6, 'f', "1100000.000000")
TEST_CASE(1.1e6, 'g', "1.1e+06")
TEST_CASE(1.1e6, 'e', "1.100000e+06")
TEST_CASE(1.1e6, 'a', "0x1.0c8ep+20")
void test_cace_ari_float64_encode(double in, char form, const char *expect)
{
    string_t out_text;
    string_init(out_text);
    TEST_ASSERT_EQUAL_INT(0, cace_ari_float64_encode(out_text, in, form));
    TEST_ASSERT_EQUAL_STRING(expect, string_get_cstr(out_text));
    string_clear(out_text);
}

TEST_CASE(20, 0, "20")
TEST_CASE(20, 1, "20.000000001")
TEST_CASE(20, 1e3, "20.000001")
TEST_CASE(20, 50e6, "20.05")
TEST_CASE(20, 999999000, "20.999999")
TEST_CASE(20, 999999999, "20.999999999")
void test_cace_decfrac_encode_valid(time_t in_sec, long in_nsec, const char *expect)
{
    struct timespec in = { .tv_sec = in_sec, .tv_nsec = in_nsec };

    string_t out_text;
    string_init(out_text);
    TEST_ASSERT_EQUAL_INT(0, cace_decfrac_encode(out_text, &in));
    TEST_ASSERT_EQUAL_STRING(expect, string_get_cstr(out_text));

    string_clear(out_text);
}

TEST_CASE("20.0", 20, 0)
TEST_CASE("20.", 20, 0)
TEST_CASE("20", 20, 0)
TEST_CASE("20.000001", 20, 1e3)
TEST_CASE("20.05", 20, 50e6)
TEST_CASE("20.999999", 20, 999999000)
void test_cace_decfrac_decode_valid(const char *text, time_t expect_sec, long expect_nsec)
{
    cace_data_t in_data;
    cace_data_init_view(&in_data, strlen(text) + 1, (cace_data_ptr_t)text);

    struct timespec out;
    TEST_ASSERT_EQUAL_INT(0, cace_decfrac_decode(&out, &in_data));

    struct timespec expect = { .tv_sec = expect_sec, .tv_nsec = expect_nsec };
    TEST_ASSERT_EQUAL(expect.tv_sec, out.tv_sec);
    TEST_ASSERT_EQUAL(expect.tv_nsec, out.tv_nsec);

    cace_data_deinit(&in_data);
}

TEST_CASE("non")
TEST_CASE("10.2.3")
TEST_CASE("20.0000000001")
void test_cace_decfrac_decode_invalid(const char *text)
{
    cace_data_t in_data;
    cace_data_init_view(&in_data, strlen(text) + 1, (cace_data_ptr_t)text);

    struct timespec out;
    TEST_ASSERT_NOT_EQUAL_INT(0, cace_decfrac_decode(&out, &in_data));

    cace_data_deinit(&in_data);
}
