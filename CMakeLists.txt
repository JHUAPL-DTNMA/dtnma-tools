#[[
Copyright (c) 2023 The Johns Hopkins University Applied Physics
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
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

# Ingest the git tag as project version name
find_package(Git)
execute_process(
    COMMAND ${GIT_EXECUTABLE} describe
    RESULT_VARIABLE GIT_TAG_EXIT
    OUTPUT_VARIABLE GIT_TAG_REV
)
if(NOT GIT_TAG_EXIT EQUAL 0)
    set(GIT_TAG_REV "v0.0.0")
    message(WARNING "No git tag found, marking as ${GIT_TAG_REV}")
endif()
# Make version compatible with CMake needs
STRING(REGEX REPLACE [[^v([0-9]+\.[0-9]+\.[0-9]+)]] [[\1]] GIT_TAG_VERS ${GIT_TAG_REV})

project(nm
    LANGUAGES C
    VERSION ${GIT_TAG_VERS}
)

option(BUILD_DOCS "Enable autodoc building" OFF)
option(BUILD_TESTING "Enable test fixtures and libraries" ON)
option(BUILD_AGENT "Build the Agent library and executable" ON)
option(BUILD_MANAGER "Build the Manager library and executable" ON)

# Language options
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)
add_compile_options(
    -Wall
    -Wextra
    #-Wpedantic
    $<$<COMPILE_LANGUAGE:C>:-Werror=implicit-function-declaration>
    $<$<COMPILE_LANGUAGE:C>:-Werror=incompatible-pointer-types>
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
add_definitions(
  -DHAVE_POSIX_MEMALIGN=${HAVE_POSIX_MEMALIGN}
  -DHAVE_MEMALIGN=${HAVE_MEMALIGN}
)

# Testing config
include(CTest)

include(FindPkgConfig)
add_subdirectory(deps)

# Libaries in deps but built separately
find_package(ION)
find_package(QCBOR)
find_package(MLIB)
if(BUILD_MANAGER)
  # All these are optional
  pkg_search_module(MYSQLCLIENT mysqlclient IMPORTED_TARGET)
  find_package(civetweb)
  find_package(cJSON)
endif(BUILD_MANAGER)

#set(BUILD_SHARED_LIBS ON)
add_subdirectory(src)

if(BUILD_TESTING)
  add_subdirectory(test)
endif(BUILD_TESTING)

if(BUILD_DOCS)
  add_subdirectory(docs)
endif(BUILD_DOCS)
