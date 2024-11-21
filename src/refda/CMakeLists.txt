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


set(HFILES
    "${CMAKE_CURRENT_SOURCE_DIR}/agent.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/msgdata.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/ingress.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/egress.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/register.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/runctx.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/valprod.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/exec.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/exec_ctx.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/eval.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/eval_ctx.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/reporting.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/reporting_ctx.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/amm/ident.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/amm/typedef.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/amm/const.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/amm/var.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/amm/edd.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/amm/ctrl.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/amm/oper.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/adm/ietf.h"
)
set(CFILES
    "agent.c"
    "msgdata.c"
    "ingress.c"
    "egress.c"
    "register.c"
    "runctx.c"
    "valprod.c"
    "exec.c"
    "exec_ctx.c"
    "eval.c"
    "eval_ctx.c"
    "reporting.c"
    "reporting_ctx.c"
    "amm/ident.c"
    "amm/typedef.c"
    "amm/const.c"
    "amm/var.c"
    "amm/edd.c"
    "amm/ctrl.c"
    "amm/oper.c"
    "adm/ietf_amm.c"
    "adm/ietf_dtnma_agent.c"
)
add_library(refda)
target_sources(refda PUBLIC ${HFILES})
target_sources(refda PRIVATE ${CFILES})
target_include_directories(
  refda PUBLIC
  $<INSTALL_INTERFACE:$<INSTALL_PREFIX>/include>
)
target_link_libraries(refda PUBLIC cace)

install(TARGETS refda)
install(
    DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
    FILE_PERMISSIONS OWNER_READ GROUP_READ WORLD_READ
    FILES_MATCHING PATTERN "*.h"
)
