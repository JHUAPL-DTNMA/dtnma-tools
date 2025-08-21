cmake_minimum_required(VERSION 3.10)

option(BUILD_SHARED_LIBS "Build using shared libraries" OFF)
option(BUILD_TESTING "Enable test fixtures and libraries" OFF)

project(
    timespec
    LANGUAGES C
    VERSION 0.0.0
)

# Language options
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)
# Force specific POSIX compatibility version
add_definitions(
  "-D_POSIX_C_SOURCE=200809L"
)

add_library(timespec)
target_sources(timespec PUBLIC ${CMAKE_CURRENT_LIST_DIR}/timespec/timespec.h)
target_sources(timespec PRIVATE ${CMAKE_CURRENT_LIST_DIR}/timespec/timespec.c)
target_include_directories(
  timespec
  PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/timespec/
)
target_compile_definitions(
  timespec
  PRIVATE
    -D_XOPEN_SOURCE
    -D_POSIX_C_SOURCE=200809L
)

set_target_properties(
    timespec PROPERTIES
    PUBLIC_HEADER ${CMAKE_CURRENT_LIST_DIR}/timespec/timespec.h
)
include(GNUInstallDirs)
install(
    TARGETS timespec
    PUBLIC_HEADER DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/timespec"
)

if(BUILD_TESTING)
  add_executable(test-timespec)
  target_sources(test-timespec PRIVATE ${CMAKE_CURRENT_LIST_DIR}/timespec/timespec.h ${CMAKE_CURRENT_LIST_DIR}/timespec/timespec.c)
  target_compile_definitions(
    test-timespec
    PRIVATE
      -DTEST
  )
  
  include(CTest)
  set(CMAKE_CTEST_ARGUMENTS
    --verbose
  )
  add_test(
    NAME test-timespec
    COMMAND test-timespec
  )
endif(BUILD_TESTING)
