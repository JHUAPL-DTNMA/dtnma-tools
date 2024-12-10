# Imports targets for using timespec library:
#  timespec::timespec
#

find_path(
  TIMESPEC_INCLUDE_DIR
  "timespec.h"
  REQUIRED
)
find_library(TIMESPEC_LIB NAMES timespec REQUIRED)

add_library(timespec::timespec SHARED IMPORTED)
target_include_directories(timespec::timespec INTERFACE ${TIMESPEC_INCLUDE_DIR})
set_target_properties(timespec::timespec PROPERTIES
  IMPORTED_LOCATION "${TIMESPEC_LIB}"
)
