
# OSAL has its own CMake target
set(OSAL_SYSTEM_OSTYPE "posix")
set(OSAL_SYSTEM_BSPTYPE "generic-linux")
set(OSAL_CONFIG_DEBUG_PRINTF ON)
#set(OSAL_INCLUDEDIR "${CMAKE_CURRENT_SOURCE_DIR}/osal/src/bsp/pc-linux/config/")
add_subdirectory(osal)
target_compile_definitions(osal_posix_impl PRIVATE OSAL_CONFIG_DEBUG_PERMISSIVE_MODE)
