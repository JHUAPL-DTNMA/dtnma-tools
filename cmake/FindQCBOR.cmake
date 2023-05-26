# Imports targets for using QCBOR libraries:
#  qcbor
#
find_path(QCBOR_HEADER NAMES qcbor/qcbor.h REQUIRED)
find_library(QCBOR_LIB NAMES qcbor REQUIRED)

add_library(qcbor SHARED IMPORTED)
set_target_properties(qcbor PROPERTIES
  INTERFACE_INCLUDE_DIRECTORIES "${QCBOR_HEADER}"
  IMPORTED_LOCATION "${QCBOR_LIB}"
)
