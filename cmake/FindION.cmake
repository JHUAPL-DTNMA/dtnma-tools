# Imports targets for using ION libraries:
#  ION::ICI for libici
#  ION::BP for libbp
#  ION::LTP for libltp
#
include(CheckIncludeFile)

find_path(
  ICI_HEADER
  NAMES sdr.h
  REQUIRED
)
find_library(ICI_LIB NAMES ici REQUIRED)

add_library(ION::ICI SHARED IMPORTED)
target_include_directories(ION::ICI INTERFACE
  ${ICI_HEADER}
)
set_target_properties(ION::ICI PROPERTIES
  IMPORTED_LOCATION "${ICI_LIB}"
)


find_path(
  BP_HEADER
  NAMES bp.h 
  REQUIRED
)
check_include_file("${BP_HEADER}/bp.h" BP_HEADER_WORKS)
find_path(
  BP_PRIVATE
  NAMES bpP.h
  REQUIRED
)
check_include_file("${BP_HEADER}/bpP.h" BPP_HEADER_WORKS)
find_library(BP_LIB NAMES bp REQUIRED)

add_library(ION::BP SHARED IMPORTED)
target_include_directories(ION::BP INTERFACE
  ${BP_HEADER}
)
set_target_properties(ION::BP PROPERTIES
  IMPORTED_LOCATION "${BP_LIB}"
)
target_link_libraries(ION::BP INTERFACE ION::ICI)


find_path(
  LTP_HEADER
  NAMES ltp.h
  REQUIRED
)
check_include_file("${LTP_HEADER}/ltp.h" LTP_HEADER_WORKS)
find_library(LTP_LIB NAMES ltp REQUIRED)

add_library(ION::LTP SHARED IMPORTED)
target_include_directories(ION::LTP INTERFACE
  ${LTP_HEADER}
)
set_target_properties(ION::LTP PROPERTIES
  IMPORTED_LOCATION "${LTP_LIB}"
)
target_link_libraries(ION::LTP INTERFACE ION::ICI)
