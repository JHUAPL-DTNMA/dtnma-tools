# Imports targets for using ION libraries:
#  ION::ICI for libici
#  ION::BP for libbp
#
find_path(ICI_HEADER NAMES sdr.h REQUIRED)
find_library(ICI_LIB NAMES ici REQUIRED)

add_library(ION::ICI SHARED IMPORTED)
set_target_properties(ION::ICI PROPERTIES
  INTERFACE_INCLUDE_DIRECTORIES "${ICI_HEADER}"
  IMPORTED_LOCATION "${ICI_LIB}"
)


find_path(BP_HEADER NAMES bp.h REQUIRED)
find_library(BP_LIB NAMES bp REQUIRED)

add_library(ION::BP SHARED IMPORTED)
set_target_properties(ION::BP PROPERTIES
  INTERFACE_INCLUDE_DIRECTORIES "${BP_HEADER}"
  IMPORTED_LOCATION "${BP_LIB}"
)
target_link_libraries(ION::BP INTERFACE ION::ICI)
