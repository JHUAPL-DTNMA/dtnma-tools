#[[
Copyright (c) 2011-2025 The Johns Hopkins University Applied Physics
Laboratory LLC.

This file is part of the Delay-Tolerant Networking Management
Architecture (DTNMA) Tools package.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at
    http://www.apache.org/licenses/LICENSE-2.0
Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
]]
cmake_minimum_required(VERSION 3.10)

# PROJECT_VERSION may be specified as a parameter to allow an application
# to define a project version independent of the git tags

option(BUILD_SHARED_LIBS "Build using shared libraries" ON)
option(BUILD_DOCS_API "Enable API documentation building" OFF)
option(BUILD_DOCS_MAN "Enable manpage building" OFF)
option(BUILD_AGENT "Build the Agent library and executable" ON)
option(BUILD_MANAGER "Build the Manager library and executable" ON)
option(BUILD_ION_PROXY "Build the ION BP proxy executable" ON)
option(TRANSPORT_UNIX_SOCKET "Enable transport bindings for UNIX datagram sockets" ON)
option(TRANSPORT_PROXY_SOCKET "Enable transport bindings for BP-proxy sockets" ON)
option(TRANSPORT_ION_BP "Enable transport bindings for ION BP" ON)
option(ARI_TEXT_PARSE "Build ARI text-form parsing capability" ON)
option(ENABLE_LUT_CACHE "Enable runtime lookup caching" ON)
option(REFDM_UI_CLI "Enable text UI CLI for refdm" OFF)
option(BUILD_TESTING "Enable test fixtures and libraries" OFF)
option(TEST_MEMCHECK "Enable test runtime memory checking" OFF)
option(TEST_COVERAGE "Enable test runtime coverage logging" OFF)


set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

if(NOT PROJECT_VERSION)
  # Ingest the git tag as project version name
  find_package(Git)
  execute_process(
    COMMAND ${GIT_EXECUTABLE} describe --tags --long --dirty
      RESULT_VARIABLE GIT_TAG_EXIT
      OUTPUT_VARIABLE GIT_TAG_REV
      OUTPUT_STRIP_TRAILING_WHITESPACE
      ERROR_QUIET
  )
  if(NOT GIT_TAG_EXIT EQUAL 0)
      execute_process(
          COMMAND ${GIT_EXECUTABLE} describe --always --dirty
          OUTPUT_VARIABLE GIT_COMMIT
          OUTPUT_STRIP_TRAILING_WHITESPACE
      )
      set(GIT_TAG_REV "v0.0.0-0-g${GIT_COMMIT}")
      message(WARNING "No git tag found, marking as ${GIT_TAG_REV}")
  endif()
  # Make version compatible with CMake needs
  STRING(REGEX REPLACE [[^v([0-9]+\.[0-9]+\.[0-9]+).*]] [[\1]] GIT_TAG_VERS ${GIT_TAG_REV})
  STRING(REGEX REPLACE [[^v[0-9\.]*-(.+)]] [[\1]] GIT_TAG_MOD ${GIT_TAG_REV})
  STRING(REPLACE "-" "." GIT_TAG_MOD ${GIT_TAG_MOD})
  message(STATUS "Using version marking ${GIT_TAG_VERS} - ${GIT_TAG_MOD}")

  set(PROJECT_VERSION ${GIT_TAG_VERS})
endif(NOT PROJECT_VERSION)

project(dtnma-tools
    VERSION ${PROJECT_VERSION}
)

# Language options
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)
# Force specific POSIX compatibility version
add_definitions(
  -D_DEFAULT_SOURCE
  -D_XOPEN_SOURCE
  -D_POSIX_C_SOURCE=200809L
)
# CXX used for compilation tests
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Generic warn/error options
add_compile_options(
    -Wall
    -Wextra
    -Wpedantic
    $<$<COMPILE_LANGUAGE:C>:-Werror=implicit-function-declaration>
    $<$<COMPILE_LANGUAGE:C>:-Werror=incompatible-pointer-types>
)
add_compile_options(
    -ffunction-sections
    -fdata-sections
    -fno-omit-frame-pointer
    $<$<COMPILE_LANGUAGE:CXX>:-fno-exceptions>
)
if(CMAKE_C_COMPILER_ID MATCHES "GNU")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0")
    set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -O0")
    add_compile_options(
        $<$<COMPILE_LANGUAGE:C>:-Werror=discarded-qualifiers>
        $<$<COMPILE_LANGUAGE:CXX>:-fno-enforce-eh-specs>
        $<$<COMPILE_LANGUAGE:CXX>:-fnothrow-opt>
    )
elseif(CMAKE_C_COMPILER_ID MATCHES "Clang")
    set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -gdwarf-4")
endif()

# Library dependencies
include(CheckSymbolExists)
check_symbol_exists(posix_memalign "stdlib.h" HAVE_POSIX_MEMALIGN)
check_symbol_exists(memalign "malloc.h" HAVE_MEMALIGN)
check_symbol_exists(timespec_get "time.h" HAVE_TIMESPEC_GET)
check_symbol_exists(clock_gettime "time.h" HAVE_CLOCK_GETTIME)
check_symbol_exists(getopt_long "getopt.h" HAVE_GETOPT_LONG)


if(BUILD_TESTING)
  if(TEST_MEMCHECK)
    find_program(MEMCHECK_CMD valgrind)
    message(STATUS "Using valgrind memcheck for tests: ${MEMCHECK_CMD}")
    set(MEMCHECK_OPTIONS
      "--tool=memcheck"
      "--trace-children=yes"
      "--track-origins=yes"
      "--leak-check=full" "--show-leak-kinds=all"
      "--suppressions=${CMAKE_CURRENT_SOURCE_DIR}/memcheck.supp"
      "--error-exitcode=2"
    )
    # Arguments as list into global scope for Findunitytools.cmake
    set(TEST_EXEC_PREFIX "${MEMCHECK_CMD}" ${MEMCHECK_OPTIONS})
  endif(TEST_MEMCHECK)
  if(TEST_COVERAGE)
    include(CodeCoverage)
    append_coverage_compiler_flags()

    set(COVERAGE_EXCLUDES
      "${CMAKE_CURRENT_SOURCE_DIR}/deps/*"
      "${CMAKE_CURRENT_SOURCE_DIR}/testroot/*"
      "${CMAKE_CURRENT_BINARY_DIR}/test/*"
    )
    set(GCOVR_ADDITIONAL_ARGS
    )
    setup_target_for_coverage_gcovr_xml(
      NAME coverage-cace-xml
      BASE_DIRECTORY "${CMAKE_SOURCE_DIR}/src/cace"
      BINARY_DIRECTORY "${CMAKE_BINARY_DIR}/src/cace"
    )
    setup_target_for_coverage_gcovr_html(
      NAME coverage-cace-html
      BASE_DIRECTORY "${CMAKE_SOURCE_DIR}/src/cace"
      BINARY_DIRECTORY "${CMAKE_BINARY_DIR}/src/cace"
    )
    setup_target_for_coverage_gcovr_xml(
      NAME coverage-refda-xml
      BASE_DIRECTORY "${CMAKE_SOURCE_DIR}/src/refda"
      BINARY_DIRECTORY "${CMAKE_BINARY_DIR}/src/refda"
    )
    setup_target_for_coverage_gcovr_html(
      NAME coverage-refda-html
      BASE_DIRECTORY "${CMAKE_SOURCE_DIR}/src/refda"
      BINARY_DIRECTORY "${CMAKE_BINARY_DIR}/src/refda"
    )
  endif(TEST_COVERAGE)

  include(CTest)
  set(CMAKE_CTEST_ARGUMENTS
    --output-junit testresults.xml
    --output-on-failure
  )
endif(BUILD_TESTING)

include(FindPkgConfig)

# Libaries in deps but built separately
find_package(QCBOR REQUIRED)
find_package(MLIB REQUIRED)
find_package(timespec REQUIRED)
# Optional dependencies
pkg_search_module(PCRE libpcre2-8 IMPORTED_TARGET)
message(STATUS "Found PCRE version ${PCRE_VERSION}")
pkg_search_module(LIBSYSTEMD libsystemd IMPORTED_TARGET)
message(STATUS "Found libsystemd version ${LIBSYSTEMD_VERSION}")

if(BUILD_ION_PROXY OR TRANSPORT_ION_BP)
    find_package(ION REQUIRED)
    message(STATUS "Found ION ${ION_FOUND}")
endif(BUILD_ION_PROXY OR TRANSPORT_ION_BP)
if(ARI_TEXT_PARSE)
    find_package(FLEX REQUIRED)
    message(STATUS "Found FLEX version ${FLEX_VERSION}")
    find_package(BISON REQUIRED)
    message(STATUS "Found BISON version ${BISON_VERSION}")
endif(ARI_TEXT_PARSE)

if(BUILD_MANAGER)
    # All these are optional
    pkg_search_module(MYSQLCLIENT mysqlclient IMPORTED_TARGET)
    message(STATUS "Found MySQLClient version ${MYSQLCLIENT_VERSION}")
    find_package(PostgreSQL)
    message(STATUS "Found PostgreSQL version ${PostgreSQL_VERSION}")
    find_package(civetweb)
    message(STATUS "Found civetweb version ${civetweb_VERSION}")
    find_package(cJSON)
endif(BUILD_MANAGER)

add_subdirectory(src)

if(BUILD_TESTING)
    add_subdirectory(test)
endif(BUILD_TESTING)

if(BUILD_DOCS_API OR BUILD_DOCS_MAN)
    add_subdirectory(docs)
endif(BUILD_DOCS_API OR BUILD_DOCS_MAN)
