
# OSAL has its own CMake target
set(OSAL_SYSTEM_OSTYPE "posix")
set(OSAL_SYSTEM_BSPTYPE "generic-linux")
set(OSAL_CONFIG_DEBUG_PRINTF ON)
#set(OSAL_INCLUDEDIR "${CMAKE_CURRENT_SOURCE_DIR}/osal/src/bsp/pc-linux/config/")
add_subdirectory(osal)
target_compile_definitions(osal_posix_impl PRIVATE OSAL_CONFIG_DEBUG_PERMISSIVE_MODE)

# QCBOR has its own CMake target
#add_subdirectory(QCBOR)

# Unity has its own CMake target, and we add testing functions to that
if(BUILD_TESTING)
  add_subdirectory(unity)
endif(BUILD_TESTING)
