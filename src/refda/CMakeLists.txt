#[[
Copyright (c) 2011-2026 The Johns Hopkins University Applied Physics
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

set(HFILES
    "${CMAKE_CURRENT_SOURCE_DIR}/agent.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/endpoint.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/acl.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/alarms.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/instr.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/loader.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/binding.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/msgdata.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/ingress.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/egress.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/register.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/runctx.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/edd_prod_ctx.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/valprod.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/ctrl_exec_ctx.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/exec.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/exec_item.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/exec_seq.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/exec_proc.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/oper_eval_ctx.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/eval.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/eval_ctx.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/reporting.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/reporting_ctx.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/timeline.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/amm/ident.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/amm/typedef.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/amm/const.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/amm/var.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/amm/edd.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/amm/ctrl.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/amm/oper.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/adm/ietf_amm.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/adm/ietf_amm_base.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/adm/ietf_amm_semtype.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/adm/ietf_network_base.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/adm/ietf_dtnma_agent.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/adm/ietf_dtnma_agent_acl.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/adm/ietf_alarms.h"
)
set(CFILES
    "agent.c"
    "endpoint.c"
    "acl.c"
    "alarms.c"
    "instr.c"
    "loader.c"
    "binding.c"
    "msgdata.c"
    "ingress.c"
    "egress.c"
    "register.c"
    "runctx.c"
    "edd_prod_ctx.c"
    "valprod.c"
    "ctrl_exec_ctx.c"
    "exec.c"
    "exec_item.c"
    "exec_seq.c"
    "exec_proc.c"
    "oper_eval_ctx.c"
    "eval.c"
    "eval_ctx.c"
    "reporting.c"
    "reporting_ctx.c"
    "timeline.c"
    "amm/ident.c"
    "amm/typedef.c"
    "amm/const.c"
    "amm/var.c"
    "amm/edd.c"
    "amm/ctrl.c"
    "amm/oper.c"
    "amm/sbr.c"
    "amm/tbr.c"
    "adm/ietf_amm.c"
    "adm/ietf_amm_base.c"
    "adm/ietf_amm_semtype.c"
    "adm/ietf_network_base.c"
    "adm/ietf_dtnma_agent.c"
    "adm/ietf_dtnma_agent_acl.c"
    "adm/ietf_alarms.c"
)
add_library(refda)
target_sources(refda PUBLIC ${HFILES})
target_sources(refda PRIVATE ${CFILES})
target_include_directories(
  refda PUBLIC
  $<INSTALL_INTERFACE:$<INSTALL_PREFIX>/include>
)
target_link_libraries(refda PUBLIC cace)

install(
    TARGETS refda
    RUNTIME
        COMPONENT runtime
    LIBRARY
        COMPONENT runtime
        NAMELINK_COMPONENT devel
    ARCHIVE
        COMPONENT devel
)
install(
    DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
    FILE_PERMISSIONS OWNER_READ GROUP_READ WORLD_READ
    FILES_MATCHING PATTERN "*.h"
)
