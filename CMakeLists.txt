#[[
Copyright (c) 2011-2024 The Johns Hopkins University Applied Physics
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

option(BUILD_SHARED_LIBS "Build using shared libraries" ON)
option(BUILD_DOCS "Enable autodoc building" OFF)
option(BUILD_AGENT "Build the Agent library and executable" ON)
option(BUILD_MANAGER "Build the Manager library and executable" OFF) # FIXME default to ON
option(ARI_TEXT_PARSE "Build ARI text-form parsing capability" ON)
option(ENABLE_LUT_CACHE "Enable runtime lookup caching" ON)
option(BUILD_TESTING "Enable test fixtures and libraries" OFF)
option(TEST_MEMCHECK "Enable test runtime memory checking" OFF)
option(TEST_COVERAGE "Enable test runtime coverage logging" OFF)


set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

# Ingest the git tag as project version name
find_package(Git)
execute_process(
    COMMAND ${GIT_EXECUTABLE} describe --always --tags --dirty
    RESULT_VARIABLE GIT_TAG_EXIT
    OUTPUT_VARIABLE GIT_TAG_REV
    OUTPUT_STRIP_TRAILING_WHITESPACE
)
if(NOT GIT_TAG_EXIT EQUAL 0)
    set(GIT_TAG_REV "v0.0.0")
    message(WARNING "No git tag found, marking as ${GIT_TAG_REV}")
endif()
# Make version compatible with CMake needs
STRING(REGEX REPLACE [[^v([0-9]+\.[0-9]+\.[0-9]+).*]] [[\1]] GIT_TAG_VERS "${GIT_TAG_REV}")
message(STATUS "Using git label ${GIT_TAG_REV} to version ${GIT_TAG_VERS}")

project(nm
    LANGUAGES C
    VERSION ${GIT_TAG_VERS}
)

# Language options
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)
# Generic warn/error options
add_compile_options(
    -Wall
    -Wextra
    -Wpedantic
    $<$<COMPILE_LANGUAGE:C>:-Werror=implicit-function-declaration>
    $<$<COMPILE_LANGUAGE:C>:-Werror=incompatible-pointer-types>
    $<$<COMPILE_LANGUAGE:C>:-Werror=discarded-qualifiers>
)
add_compile_options(
    -ffunction-sections
    -fdata-sections
    -fno-omit-frame-pointer
    $<$<COMPILE_LANGUAGE:CXX>:-fno-exceptions>
    $<$<COMPILE_LANGUAGE:CXX>:-fno-enforce-eh-specs>
    $<$<COMPILE_LANGUAGE:CXX>:-fnothrow-opt>
)
IF(CMAKE_COMPILER_IS_GNUCC)
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0")
    set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -O0")
ENDIF(CMAKE_COMPILER_IS_GNUCC)

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
    --verbose
  )
endif(BUILD_TESTING)

include(FindPkgConfig)

# Libaries in deps but built separately
find_package(ION)
find_package(QCBOR)
find_package(MLIB)
if(ARI_TEXT_PARSE)
  find_package(FLEX REQUIRED)
  find_package(BISON REQUIRED)
endif(ARI_TEXT_PARSE)
if(BUILD_MANAGER)
  # All these are optional
  pkg_search_module(MYSQLCLIENT mysqlclient IMPORTED_TARGET)
  find_package(PostgreSQL)
  find_package(civetweb)
  find_package(cJSON)
endif(BUILD_MANAGER)

add_subdirectory(src)

if(BUILD_TESTING)
  add_subdirectory(test)
endif(BUILD_TESTING)

if(BUILD_DOCS)
  add_subdirectory(docs)
endif(BUILD_DOCS)
