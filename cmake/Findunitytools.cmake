# Defines the function add_unity_test() for generating test executables.
# This requires the cmake variable UNITY_ROOT to point to the source tree.
#
message(STATUS "Searching for Unity tools in ${UNITY_ROOT}")

find_program(RUBY_BIN ruby REQUIRED)
if(NOT RUBY_BIN)
  message(FATAL_ERROR "Missing ruby")
endif()
set(RUBY_BIN ${RUBY_BIN} PARENT_SCOPE)

find_file(UNITY_GENERATOR_BIN "generate_test_runner.rb" 
  PATHS "${UNITY_ROOT}/auto" 
  NO_DEFAULT_PATH
  REQUIRED
)
set(UNITY_GENERATOR_BIN ${UNITY_GENERATOR_BIN} PARENT_SCOPE)

find_file(UNITY_PARSER_BIN "parse_output.rb" 
  PATHS "${UNITY_ROOT}/auto" 
  NO_DEFAULT_PATH
  REQUIRED
)

function(add_unity_test)
  set(options OPTIONAL )
  set(oneValueArgs TARGET)
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
  
  message(STATUS "Adding unit test ${UNITYTEST_TARGET} from ${ABSOLUTE_SOURCE}")
  
  set(RUNNER_FILE "${CMAKE_CURRENT_BINARY_DIR}/${BASENAME}_Runner.c")
  add_custom_command(
    OUTPUT "${RUNNER_FILE}"
    DEPENDS "${ABSOLUTE_SOURCE}"
    COMMAND ${RUBY_BIN} ${UNITY_GENERATOR_BIN} "${ABSOLUTE_SOURCE}" "${RUNNER_FILE}"
      --main_name=unity_main
  )
  add_executable(${BASENAME} ${UNITYTEST_SOURCE} ${RUNNER_FILE})
  target_link_libraries(${BASENAME} PUBLIC unity)
  
  add_test(NAME ${BASENAME}
    COMMAND "${BASENAME}"
  )
endfunction()
