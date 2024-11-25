# Imports targets for using ION libraries:
#  ION::ICI for libici
#  ION::BP for libbp
#  ION::LTP for libltp
#
include(CheckIncludeFile)

find_path(
    ICI_HEADER
    NAMES sdr.h
)
find_library(ICI_LIB NAMES ici)

if(ICI_HEADER AND ICI_LIB)
    add_library(ION::ICI SHARED IMPORTED)
    target_include_directories(ION::ICI INTERFACE
        ${ICI_HEADER}
    )
    set_target_properties(ION::ICI PROPERTIES
        IMPORTED_LOCATION "${ICI_LIB}"
    )
endif()

find_path(
    BP_HEADER
    NAMES bp.h 
)
find_path(
    BP_PRIVATE
    NAMES bpP.h
)
find_library(BP_LIB NAMES bp)

if(BP_HEADER AND BP_PRIVATE AND BP_LIB)
    check_include_file("${BP_HEADER}/bp.h" BP_HEADER_WORKS)
    check_include_file("${BP_HEADER}/bpP.h" BPP_HEADER_WORKS)

    add_library(ION::BP SHARED IMPORTED)
    target_include_directories(ION::BP INTERFACE
        ${BP_HEADER}
    )
    set_target_properties(ION::BP PROPERTIES
        IMPORTED_LOCATION "${BP_LIB}"
    )
    target_link_libraries(ION::BP INTERFACE ION::ICI)
endif()

find_path(
  LTP_HEADER
  NAMES ltp.h
)
find_library(LTP_LIB NAMES ltp)

if(LTP_HEADER AND LTP_LIB)
    check_include_file("${LTP_HEADER}/ltp.h" LTP_HEADER_WORKS)

    add_library(ION::LTP SHARED IMPORTED)
    target_include_directories(ION::LTP INTERFACE
        ${LTP_HEADER}
    )
    set_target_properties(ION::LTP PROPERTIES
        IMPORTED_LOCATION "${LTP_LIB}"
    )
    target_link_libraries(ION::LTP INTERFACE ION::ICI)
endif()

if (NOT ICI_HEADER OR NOT BP_HEADER OR NOT LTP_HEADER)
    set(ION_FOUND false)
endif()