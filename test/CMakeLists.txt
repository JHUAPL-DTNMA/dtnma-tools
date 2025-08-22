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
set(UNITY_ROOT "${CMAKE_SOURCE_DIR}/deps/unity")
find_package(unitytools)
set(UNITY_GENERATOR_BIN "${UNITY_ROOT}/auto/generate_test_runner.rb")
set(UNITY_GENERATOR_BIN ${UNITY_GENERATOR_BIN} PARENT_SCOPE)

set(UNITY_PARSER_BIN "${UNITY_ROOT}/auto/parse_output.rb")

find_program(RUBY_BIN ruby REQUIRED)
if(NOT RUBY_BIN)
  message(FATAL_ERROR "Missing ruby")
endif()
set(RUBY_BIN ${RUBY_BIN} PARENT_SCOPE)


find_program(MEMCHECK_CMD valgrind)
message(STATUS "Using valgrind memcheck for tests: ${MEMCHECK_CMD}")
set(MEMCHECK_OPTIONS
  "--tool=memcheck"
  "--trace-children=yes"
  "--track-origins=yes"
  "--leak-check=full" "--show-leak-kinds=all"
  "--suppressions=${CMAKE_SOURCE_DIR}/memcheck.supp"
  "--error-exitcode=2"
)
# Arguments as list into global scope for Findunitytools.cmake
set(TEST_EXEC_PREFIX "${MEMCHECK_CMD}" ${MEMCHECK_OPTIONS})

function(add_unity_test)
  set(options OPTIONAL )
  set(oneValueArgs TARGET MAIN_NAME)
  set(multiValueArgs SOURCE)
  cmake_parse_arguments(
    UNITYTEST "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN}
  )
  list(GET UNITYTEST_SOURCE 0 MAIN_SOURCE)
  get_filename_component(BASENAME ${MAIN_SOURCE} NAME_WE)
  get_filename_component(ABSOLUTE_SOURCE ${MAIN_SOURCE} ABSOLUTE)

  if(NOT UNITYTEST_TARGET)
    set(UNITYTEST_TARGET "${BASENAME}")
  endif()
  
  set(GEN_PARAMS "--use_param_tests=1")
  if(UNITYTEST_MAIN_NAME)
      list(APPEND GEN_PARAMS "--main_name=${MAIN_NAME}")
  endif()
  
  message(STATUS "Adding unit test ${UNITYTEST_TARGET} from ${ABSOLUTE_SOURCE}")
  
  set(RUNNER_FILE "${CMAKE_CURRENT_BINARY_DIR}/${BASENAME}_Runner.c")
  add_custom_command(
    OUTPUT "${RUNNER_FILE}"
    DEPENDS "${ABSOLUTE_SOURCE}"
    COMMAND ${RUBY_BIN} ${UNITY_GENERATOR_BIN} "${ABSOLUTE_SOURCE}" "${RUNNER_FILE}" ${GEN_PARAMS}
  )
  add_executable(${BASENAME} ${UNITYTEST_SOURCE} ${RUNNER_FILE})
  target_compile_definitions(${BASENAME} PRIVATE 
    UNITY_INCLUDE_PRINT_FORMATTED
    UNITY_INCLUDE_FLOAT
    UNITY_INCLUDE_DOUBLE
  )
  target_link_libraries(${BASENAME} PUBLIC unity::framework)

  add_test(
    NAME ${BASENAME}
    COMMAND ${TEST_EXEC_PREFIX} "${CMAKE_CURRENT_BINARY_DIR}/${BASENAME}"
  )
endfunction()

include_directories(
  BEFORE
  ${CMAKE_BINARY_DIR}/src
  ${CMAKE_SOURCE_DIR}/src
  ${CMAKE_SOURCE_DIR}/test
)

add_subdirectory(util)
add_subdirectory(cace)
add_subdirectory(refda)
