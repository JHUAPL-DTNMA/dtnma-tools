#[[
Copyright (c) 2011-2024 The Johns Hopkins University Applied Physics
Laboratory LLC.

This file is part of the Delay-Tolerant Networking Management
Architecture (DTNMA) Tools package.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at
    http://www.apache.org/licenses/LICENSE-2.0
Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
]]

include(GNUInstallDirs)

# ARI handling library
configure_file("config.h.in" "config.h")
set(HFILES
    "${CMAKE_CURRENT_BINARY_DIR}/config.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/cace_data.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/util/defs.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/util/logging.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/util/threadset.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/util/daemon_run.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/ari.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/ari/type.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/ari/base.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/ari/algo.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/ari/containers.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/ari/access.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/ari/cbor.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/ari/text_util.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/ari/text.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/amm/typing.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/amm/numeric.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/amm/parameters.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/amm/obj_desc.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/amm/obj_ns.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/amm/obj_store.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/amm/lookup.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/amm/msg_if.h"
)
set(CFILES
    "cace_data.c"
    "util/logging_stderr.c"
    "util/threadset.c"
    "util/daemon_run.c"
    "ari/type.c"
    "ari/base.c"
    "ari/algo.c"
    "ari/containers.c"
    "ari/access.c"
    "ari/cbor.c"
    "ari/text_util.c"
    "ari/text_enc.c"
    "amm/typing.c"
    "amm/numeric.c"
    "amm/parameters.c"
    "amm/obj_desc.c"
    "amm/obj_ns.c"
    "amm/obj_store.c"
    "amm/lookup.c"
    "amm/msg_if.c"
)
if(ARI_TEXT_PARSE)
    BISON_TARGET(
        AriStrParse ${CMAKE_CURRENT_SOURCE_DIR}/ari/text_str.yac
        ${CMAKE_CURRENT_BINARY_DIR}/ari/text_str_parse.c
        COMPILE_FLAGS "-p cace_ari_text_str_"
    )
    FLEX_TARGET(
        AriStrScan ${CMAKE_CURRENT_SOURCE_DIR}/ari/text_str.lex
        ${CMAKE_CURRENT_BINARY_DIR}/ari/text_str_scan.c
        DEFINES_FILE ${CMAKE_CURRENT_BINARY_DIR}/ari/text_str_scan.h
    )
    ADD_FLEX_BISON_DEPENDENCY(AriStrScan AriStrParse)
    #list(APPEND HFILES ${BISON_AriStrParse_OUTPUT_HEADER} ${FLEX_AriStrScan_OUTPUT_HEADER})
    list(APPEND CFILES ${BISON_AriStrParse_OUTPUT_SOURCE} ${FLEX_AriStrScan_OUTPUTS})

    BISON_TARGET(
        AriValParse ${CMAKE_CURRENT_SOURCE_DIR}/ari/text_val.yac
        ${CMAKE_CURRENT_BINARY_DIR}/ari/text_val_parse.c
        COMPILE_FLAGS "-p cace_ari_text_val_"
    )
    FLEX_TARGET(
        AriValScan ${CMAKE_CURRENT_SOURCE_DIR}/ari/text_val.lex
        ${CMAKE_CURRENT_BINARY_DIR}/ari/text_val_scan.c
        DEFINES_FILE ${CMAKE_CURRENT_BINARY_DIR}/ari/text_val_scan.h
    )
    ADD_FLEX_BISON_DEPENDENCY(AriValScan AriValParse)
    #list(APPEND HFILES ${BISON_AriValParse_OUTPUT_HEADER} ${FLEX_AriValScan_OUTPUT_HEADER})
    list(APPEND CFILES ${BISON_AriValParse_OUTPUT_SOURCE} ${FLEX_AriValScan_OUTPUTS})
endif(ARI_TEXT_PARSE)

add_library(cace)
target_sources(cace PUBLIC ${HFILES})
target_sources(cace PRIVATE ${CFILES})
target_include_directories(
  cace PUBLIC
  $<INSTALL_INTERFACE:$<INSTALL_PREFIX>/include>
)
target_compile_definitions(cace PRIVATE
  -D_XOPEN_SOURCE
  -D_POSIX_C_SOURCE=200809L
)
target_link_libraries(cace PUBLIC ${FLEX_LIBRARIES})
target_link_libraries(cace PUBLIC MLIB::mlib)
target_link_libraries(cace PUBLIC qcbor)
target_link_libraries(cace PUBLIC m)


# Stand-alone ARI manipulation tool
add_executable(cace_ari)
target_sources(cace_ari PRIVATE cace_ari.c)
target_link_libraries(cace_ari PUBLIC cace)


install(
    TARGETS cace cace_ari
)
install(
    DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
    FILE_PERMISSIONS OWNER_READ GROUP_READ WORLD_READ
    FILES_MATCHING PATTERN "*.h"
)
install(
    DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
    FILE_PERMISSIONS OWNER_READ GROUP_READ WORLD_READ
    FILES_MATCHING PATTERN "*.h"
)
