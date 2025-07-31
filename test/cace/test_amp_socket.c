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
#include <cace/amp/socket.h>
#include <cace/ari/text_util.h>
#include <cace/ari/cbor.h>
#include <cace/util/logging.h>
#include <sys/stat.h>
#include <errno.h>
#include <unistd.h>
#include <dirent.h>
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

/// Socket path for each test run
static m_string_t tmp_path;

void setUp(void)
{
    m_string_init(tmp_path);

    char  tmp_template[] = "/tmp/testXXXXXX";
    char *got            = mkdtemp(tmp_template);
    TEST_ASSERT_NOT_NULL(got);
    m_string_set_cstr(tmp_path, got);
}

static int rmdir_recurse(const char *path)
{
    if (chdir(path))
    {
        CACE_LOG_ERR("failed to chdir() on: %s", path);
        return 2;
    }

    DIR *dir = opendir(path);
    if (!dir)
    {
        CACE_LOG_ERR("failed to opendir() on: %s", path);
        return 2;
    }

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL)
    {
        if ((strcmp(entry->d_name, ".") == 0) || (strcmp(entry->d_name, "..") == 0))
        {
            continue;
        }

        if (entry->d_type == DT_DIR)
        {
            m_string_t subpath;
            m_string_init_printf(subpath, "%s/%s", path, entry->d_name);
            int res = rmdir_recurse(m_string_get_cstr(subpath));
            m_string_clear(subpath);
            if (res)
            {
                CACE_LOG_ERR("failed to recurse on %s/%s", path, entry->d_name);
            }
        }
        else
        {
            int res = unlink(entry->d_name);
            if (res)
            {
                CACE_LOG_ERR("failed to unlink file %s/%s", path, entry->d_name);
            }
        }
    }
    if (closedir(dir))
    {
        CACE_LOG_ERR("failed to closedir() on: %s", path);
        return 2;
    }

    int res = rmdir(m_string_get_cstr(tmp_path));
    if (res)
    {
        CACE_LOG_ERR("failed to rmdir() on: %s", path);
    }

    return 0;
}

void tearDown(void)
{
    TEST_ASSERT_EQUAL_INT(0, rmdir_recurse(m_string_get_cstr(tmp_path)));
    m_string_clear(tmp_path);
}

void test_bind_valid(void)
{
    cace_amp_socket_state_t state;
    cace_amp_socket_state_init(&state);

    m_string_t sock_path;
    m_string_init_printf(sock_path, "%s/test.sock", m_string_get_cstr(tmp_path));
    TEST_ASSERT_EQUAL_INT(0, cace_amp_socket_state_bind(&state, sock_path));

    struct stat info;
    TEST_ASSERT_EQUAL_INT(0, stat(m_string_get_cstr(sock_path), &info));
    TEST_ASSERT_EQUAL(S_IFSOCK, info.st_mode & S_IFMT);

    cace_amp_socket_state_unbind(&state);
    TEST_ASSERT_EQUAL_INT(-1, stat(m_string_get_cstr(sock_path), &info));
    TEST_ASSERT_EQUAL_INT(ENOENT, errno);

    m_string_clear(sock_path);
    cace_amp_socket_state_deinit(&state);
}

void test_bind_path_toolong(void)
{
    cace_amp_socket_state_t state;
    cace_amp_socket_state_init(&state);

    m_string_t sock_path;
    m_string_init_printf(
        sock_path, "%s/test-with-long-long-long-long-long-long-long-long-long-long-long-long-long-long-long-name.sock",
        m_string_get_cstr(tmp_path));
    TEST_ASSERT_EQUAL_INT(1, cace_amp_socket_state_bind(&state, sock_path));

    struct stat info;
    TEST_ASSERT_EQUAL_INT(-1, stat(m_string_get_cstr(sock_path), &info));
    TEST_ASSERT_EQUAL_INT(ENOENT, errno);

    m_string_clear(sock_path);
    cace_amp_socket_state_deinit(&state);
}
