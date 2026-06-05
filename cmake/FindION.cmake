# Sets ION_FOUND based on finding all libraries
# Imports targets for using ION libraries:
#  ION::ICI for libici
#  ION::BP for libbp
#  ION::LTP for libltp
#
set(FPHSA_NAME_MISMATCHED ON)
include(FindPkgConfig)
include(CheckIncludeFile)
include(CMakePushCheckState)

# pkgconfig to steal flags/defines if possible
pkg_check_modules(ION_PKG ion)
set(FPHSA_NAME_MISMATCHED OFF)
if(ION_PKG_FOUND)
    # only defines here
    list(FILTER ION_PKG_CFLAGS INCLUDE REGEX "^-D")
    list(JOIN ION_PKG_CFLAGS " " ION_PKG_OPTIONS)
    message(STATUS "using ion.pc CFLAGS: ${ION_PKG_OPTIONS}")
else(ION_PKG_FOUND)
    set(ION_PKG_OPTIONS "")
endif(ION_PKG_FOUND)
cmake_push_check_state()
set(CMAKE_REQUIRED_FLAGS ${ION_PKG_OPTIONS})

#
# ION::ICI library
#
find_path(
    ICI_HEADER
    NAMES sdr.h
)
find_library(ICI_LIB NAMES ici)
message(STATUS "Found ION ici at ${ICI_HEADER} and ${ICI_LIB}")
if(ICI_HEADER AND ICI_LIB)
    check_include_file("${ICI_HEADER}/sdr.h" SDR_HEADER_WORKS)

    add_library(ION::ICI SHARED IMPORTED)
    target_compile_definitions(ION::ICI INTERFACE ${ION_PKG_CFLAGS})
    target_include_directories(ION::ICI INTERFACE
        ${ICI_HEADER}
    )
    set_target_properties(ION::ICI PROPERTIES
        IMPORTED_LOCATION "${ICI_LIB}"
    )
endif()

#
# ION::BP library
#
find_path(
    BP_HEADER
    NAMES bp.h 
)
find_library(BP_LIB NAMES bp)
message(STATUS "Found ION bp at ${BP_HEADER} and ${BP_LIB}")
if(BP_HEADER AND BP_LIB)
    check_include_file("${BP_HEADER}/bp.h" BP_HEADER_WORKS)

    add_library(ION::BP SHARED IMPORTED)
    target_include_directories(ION::BP INTERFACE
        ${BP_HEADER}
    )
    set_target_properties(ION::BP PROPERTIES
        IMPORTED_LOCATION "${BP_LIB}"
    )
    target_link_libraries(ION::BP INTERFACE ION::ICI)
endif()

#
# ION::LTP library
#
find_path(
  LTP_HEADER
  NAMES ltp.h
)
find_library(LTP_LIB NAMES ltp)
message(STATUS "Found ION ltp at ${LTP_HEADER} and ${LTP_LIB}")
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

cmake_pop_check_state()

if(TARGET ION::ICI AND TARGET ION::BP)
    set(ION_FOUND true)
else()
    set(ION_FOUND false)
endif()
