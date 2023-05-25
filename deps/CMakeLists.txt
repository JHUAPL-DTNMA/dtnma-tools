
# OSAL has its own CMake target
set(OSAL_SYSTEM_OSTYPE "posix")
set(OSAL_SYSTEM_BSPTYPE "generic-linux")
set(OSAL_CONFIG_DEBUG_PRINTF ON)
#set(OSAL_INCLUDEDIR "${CMAKE_CURRENT_SOURCE_DIR}/osal/src/bsp/pc-linux/config/")
add_subdirectory(osal)
target_compile_definitions(osal_posix_impl PRIVATE OSAL_CONFIG_DEBUG_PERMISSIVE_MODE)

# QCBOR has its own CMake target
add_subdirectory(QCBOR)

# Unity has its own CMake target, and we add testing functions to that
if(BUILD_TESTING)
  add_subdirectory(unity)
  find_program(RUBY_BIN ruby REQUIRED)
  if(NOT RUBY_BIN)
    message(FATAL_ERROR "Missing ruby")
  endif()
  set(RUBY_BIN ${RUBY_BIN} PARENT_SCOPE)

  set(GENERATOR_FN "generate_test_runner.rb")
  find_file(UNITY_GENERATOR_BIN ${GENERATOR_FN} PATHS "${CMAKE_CURRENT_SOURCE_DIR}/unity/auto" NO_DEFAULT_PATH)
  if(NOT UNITY_GENERATOR_BIN)
#    message(FATAL_ERROR "Missing Unity ${GENERATOR_FN} in ${CMAKE_CURRENT_SOURCE_DIR}/unity/auto/")
    set(UNITY_GENERATOR_BIN "${CMAKE_CURRENT_SOURCE_DIR}/unity/auto/${GENERATOR_FN}")
  endif()
  set(UNITY_GENERATOR_BIN ${UNITY_GENERATOR_BIN} PARENT_SCOPE)

  function(add_unity_test)
    set(options OPTIONAL )
    set(oneValueArgs TARGET SOURCE)
    set(multiValueArgs )
    cmake_parse_arguments(
      UNITYTEST "${options}" "${oneValueArgs}"
      "${multiValueArgs}" ${ARGN}
    )
    get_filename_component(BASENAME "${UNITYTEST_SOURCE}" NAME_WE)
    if(NOT UNITYTEST_TARGET)
      set(UNITYTEST_TARGET "${BASENAME}")
    endif()
    get_filename_component(ABSOLUTE_SOURCE ${UNITYTEST_SOURCE} ABSOLUTE)

    message(STATUS "Adding unit test ${UNITYTEST_TARGET} from ${ABSOLUTE_SOURCE}")

    set(RUNNER_FILE "${CMAKE_CURRENT_BINARY_DIR}/${BASENAME}_Runner.c")
    add_custom_command(
      OUTPUT "${RUNNER_FILE}"
      DEPENDS "${ABSOLUTE_SOURCE}"
      COMMAND ${RUBY_BIN} ${UNITY_GENERATOR_BIN} "${ABSOLUTE_SOURCE}" "${RUNNER_FILE}" --main_name=unity_main
    )
    add_executable(${BASENAME} ${ABSOLUTE_SOURCE} ${RUNNER_FILE})
    target_link_libraries(${BASENAME} PUBLIC unity)

    add_test(NAME ${BASENAME}
      COMMAND "${BASENAME}"
    )
  endfunction()
endif(BUILD_TESTING)

# Test with pre-existing ION ADMs
if(BUILD_TESTING)
  # TBD
endif(BUILD_TESTING)
