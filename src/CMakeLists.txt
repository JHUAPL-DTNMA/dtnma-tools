#[[
Copyright (c) 2011-2023 The Johns Hopkins University Applied Physics
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

# ARI handling library
configure_file("cace/config.h.in" "cace/config.h")
set(HFILES
  "${CMAKE_CURRENT_BINARY_DIR}/cace/config.h"
  "${CMAKE_CURRENT_SOURCE_DIR}/cace/logging.h"
  "${CMAKE_CURRENT_SOURCE_DIR}/cace/cace_data.h"
  "${CMAKE_CURRENT_SOURCE_DIR}/cace/ari.h"
  "${CMAKE_CURRENT_SOURCE_DIR}/cace/ari/type.h"
  "${CMAKE_CURRENT_SOURCE_DIR}/cace/ari/base.h"
  "${CMAKE_CURRENT_SOURCE_DIR}/cace/ari/algo.h"
  "${CMAKE_CURRENT_SOURCE_DIR}/cace/ari/containers.h"
  "${CMAKE_CURRENT_SOURCE_DIR}/cace/ari/access.h"
  "${CMAKE_CURRENT_SOURCE_DIR}/cace/ari/cbor.h"
  "${CMAKE_CURRENT_SOURCE_DIR}/cace/ari/text_util.h"
  "${CMAKE_CURRENT_SOURCE_DIR}/cace/ari/text.h"
  "${CMAKE_CURRENT_SOURCE_DIR}/cace/amm/typing.h"
  "${CMAKE_CURRENT_SOURCE_DIR}/cace/amm/parameters.h"
  "${CMAKE_CURRENT_SOURCE_DIR}/cace/amm/runctx.h"
  "${CMAKE_CURRENT_SOURCE_DIR}/cace/amm/obj_desc.h"
  "${CMAKE_CURRENT_SOURCE_DIR}/cace/amm/const.h"
  "${CMAKE_CURRENT_SOURCE_DIR}/cace/amm/var.h"
  "${CMAKE_CURRENT_SOURCE_DIR}/cace/amm/edd.h"
  "${CMAKE_CURRENT_SOURCE_DIR}/cace/amm/ctrl.h"
  "${CMAKE_CURRENT_SOURCE_DIR}/cace/amm/namespace.h"
)
set(CFILES
  "cace/logging_stderr.c"
  "cace/cace_data.c"
  "cace/ari/type.c"
  "cace/ari/base.c"
  "cace/ari/algo.c"
  "cace/ari/containers.c"
  "cace/ari/access.c"
  "cace/ari/cbor.c"
  "cace/ari/text_util.c"
  "cace/ari/text_enc.c"
  "cace/amm/typing.c"
  "cace/amm/parameters.c"
  "cace/amm/runctx.c"
  "cace/amm/obj_desc.c"
  "cace/amm/const.c"
  "cace/amm/var.c"
  "cace/amm/edd.c"
  "cace/amm/ctrl.c"
  "cace/amm/namespace.c"
)
if(ARI_TEXT_PARSE)
    BISON_TARGET(
        AriStrParse ${CMAKE_CURRENT_SOURCE_DIR}/cace/ari/text_str.yac
        ${CMAKE_CURRENT_BINARY_DIR}/cace/ari/text_str_parse.c
        COMPILE_FLAGS "-p cace_ari_text_str_"
    )
    FLEX_TARGET(
        AriStrScan ${CMAKE_CURRENT_SOURCE_DIR}/cace/ari/text_str.lex
        ${CMAKE_CURRENT_BINARY_DIR}/cace/ari/text_str_scan.c
        DEFINES_FILE ${CMAKE_CURRENT_BINARY_DIR}/cace/ari/text_str_scan.h
    )
    ADD_FLEX_BISON_DEPENDENCY(AriStrScan AriStrParse)
    list(APPEND HFILES ${BISON_AriStrParse_OUTPUT_HEADER} ${FLEX_AriStrScan_OUTPUT_HEADER})
    list(APPEND CFILES ${BISON_AriStrParse_OUTPUT_SOURCE} ${FLEX_AriStrScan_OUTPUTS})

    BISON_TARGET(
        AriValParse ${CMAKE_CURRENT_SOURCE_DIR}/cace/ari/text_val.yac
        ${CMAKE_CURRENT_BINARY_DIR}/cace/ari/text_val_parse.c
        COMPILE_FLAGS "-p cace_ari_text_val_"
    )
    FLEX_TARGET(
        AriValScan ${CMAKE_CURRENT_SOURCE_DIR}/cace/ari/text_val.lex
        ${CMAKE_CURRENT_BINARY_DIR}/cace/ari/text_val_scan.c
        DEFINES_FILE ${CMAKE_CURRENT_BINARY_DIR}/cace/ari/text_val_scan.h
    )
    ADD_FLEX_BISON_DEPENDENCY(AriValScan AriValParse)
    list(APPEND HFILES ${BISON_AriValParse_OUTPUT_HEADER} ${FLEX_AriValScan_OUTPUT_HEADER})
    list(APPEND CFILES ${BISON_AriValParse_OUTPUT_SOURCE} ${FLEX_AriValScan_OUTPUTS})
endif(ARI_TEXT_PARSE)

add_library(cace)
target_sources(cace PUBLIC ${HFILES})
target_sources(cace PRIVATE ${CFILES})
target_include_directories(
  cace PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
  $<INSTALL_INTERFACE:$<INSTALL_PREFIX>/include/cace>
)
target_compile_definitions(cace PRIVATE
  -D_XOPEN_SOURCE
  -D_POSIX_C_SOURCE=200809L
)
target_link_libraries(cace PUBLIC ${FLEX_LIBRARIES})
target_link_libraries(cace PUBLIC MLIB::mlib)
target_link_libraries(cace PUBLIC qcbor)
target_link_libraries(cace PUBLIC m)

add_executable(cace_ari)
target_sources(cace_ari PRIVATE cace/cace_ari.c)
target_link_libraries(cace_ari PUBLIC cace)


if(FALSE) # Disabled for test

# Shared library for agent and manager
set(HFILES
  "shared/nm.h"
  "shared/platform.h"
  "shared/adm/adm.h"
  "shared/platform.h"
  "shared/utils/cbor_utils.h"
  "shared/utils/db.h"
  "shared/utils/daemon_run.h"
  "shared/utils/nm_types.h"
  "shared/utils/rhht.h"
  "shared/utils/threadset.h"
  "shared/utils/utils.h"
  "shared/utils/vector.h"
  "shared/primitives/ari.h"
  "shared/primitives/blob.h"
  "shared/primitives/ctrl.h"
  "shared/primitives/edd_var.h"
  "shared/primitives/expr.h"
  "shared/primitives/report.h"
  "shared/primitives/rules.h"
  "shared/primitives/table.h"
  "shared/primitives/time.h"
  "shared/primitives/tnv.h"
  "shared/msg/msg.h"
  "shared/msg/msg_if.h"
)
set(CFILES
  "shared/adm/adm.c"
  "shared/utils/cbor_utils.c"
  "shared/utils/daemon_run.c"
  "shared/utils/nm_types.c"
  "shared/utils/db.c"
  "shared/utils/rhht.c"
  "shared/utils/threadset.c"
  "shared/utils/utils.c"
  "shared/utils/vector.c"
  "shared/primitives/ari.c"
  "shared/primitives/blob.c"
  "shared/primitives/ctrl.c"
  "shared/primitives/edd_var.c"
  "shared/primitives/expr.c"
  "shared/primitives/report.c"
  "shared/primitives/rules.c"
  "shared/primitives/table.c"
  "shared/primitives/time.c"
  "shared/primitives/tnv.c"
  "shared/msg/msg.c"
  "shared/msg/msg_if.c"
)
add_library(nmcommon ${CFILES} ${HFILES})
target_include_directories(nmcommon PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}
)
target_compile_definitions(nmcommon PUBLIC
  USE_MALLOC=1
  _GNU_SOURCE
  AMP_VERSION=8
)
target_link_libraries(nmcommon PUBLIC m)
target_link_libraries(nmcommon PUBLIC osal)
target_link_libraries(nmcommon PUBLIC MLIB::mlib)
target_link_libraries(nmcommon PUBLIC qcbor)
install(
  TARGETS nmcommon
  RUNTIME
)

if(BUILD_AGENT)
  # NM Agent library without concrete messaging interface
  set(HFILES
    agent/ingest.h
    agent/instr.h
    agent/lcc.h
    agent/ldc.h
    agent/nmagent.h
    agent/rda.h
  )
  set(CFILES
    agent/ingest.c
    agent/instr.c
    agent/lcc.c
    agent/ldc.c
    agent/nmagent.c
    agent/rda.c
  )
  add_library(nmagent ${CFILES} ${HFILES})
  target_link_libraries(nmagent PUBLIC nmcommon)
  target_include_directories(nmagent PUBLIC
      ${CMAKE_CURRENT_SOURCE_DIR}/agent
  )

  # ION-independent ADM implementation
  set(HFILES
    indep_adms/shared/adm/adm_amp_agent.h
  )
  set(CFILES
    indep_adms/agent/adm_amp_agent_agent.c
    indep_adms/agent/adm_amp_agent_impl.c
  )
  add_library(indep_adms ${CFILES} ${HFILES})
  target_link_libraries(indep_adms nmagent)
  target_include_directories(indep_adms PRIVATE
      ${CMAKE_CURRENT_SOURCE_DIR}/indep_adms/shared/adm
  )
  target_include_directories(indep_adms PUBLIC
      ${CMAKE_CURRENT_SOURCE_DIR}/indep_adms
  )

  set(HFILES
    ion_if/ion_if.h
  )
  set(CFILES
    ion_if/ion_if.c
    ion_if/nm_agent.c
    ion_if/agent/adm_ion_admin_agent.c
    ion_if/agent/adm_ion_admin_impl.c
    ion_if/agent/adm_ion_ipn_admin_agent.c
    ion_if/agent/adm_ion_ipn_admin_impl.c
    ion_if/agent/adm_ion_ltp_admin_agent.c
    ion_if/agent/adm_ion_ltp_admin_impl.c
    ion_if/agent/adm_ionsec_admin_agent.c
    ion_if/agent/adm_ionsec_admin_impl.c
    ion_if/agent/adm_ltp_agent_agent.c
    ion_if/agent/adm_ltp_agent_impl.c
    ion_if/bpv7/adm/adm_bp_agent_agent.c
    ion_if/bpv7/adm/adm_bp_agent_impl.c
#    ion_if/bpv7/adm/adm_bpsec_agent.c
#    ion_if/bpv7/adm/adm_bpsec_impl.c
    ion_if/bpv7/adm/adm_ion_bp_admin_agent.c
    ion_if/bpv7/adm/adm_ion_bp_admin_impl.c
  )
  add_executable(ion_nm_agent ${CFILES} ${HFILES})
  target_link_libraries(ion_nm_agent nmagent)
  target_link_libraries(ion_nm_agent indep_adms)
  target_link_libraries(ion_nm_agent ION::BP)
  target_link_libraries(ion_nm_agent ION::LTP)
  target_include_directories(ion_nm_agent PRIVATE
      ${CMAKE_CURRENT_SOURCE_DIR}/ion_if/shared/adm
  )

  # Agent that uses hex-strings in stdin/stdout as transport
  set(HFILES
  )
  set(CFILES
    stdio_if/stdio_agent.c
  )
  add_executable(stdio_agent ${CFILES} ${HFILES})
  target_link_libraries(stdio_agent nmagent)
  target_link_libraries(stdio_agent indep_adms)

  install(
    TARGETS nmagent ion_nm_agent stdio_agent
    RUNTIME
  )
endif(BUILD_AGENT)

if(BUILD_MANAGER)
  # NM Manager library without concrete messaging interface
  set(HFILES
    mgr/agents.h
    mgr/metadata.h
    mgr/nm_mgr_print.h
    mgr/nm_mgr_rx.h
    mgr/nm_mgr_sql.h
    mgr/nm_mgr_ui.h
    mgr/nmmgr.h
    mgr/ui_input.h
  )
  set(CFILES
    mgr/agents.c
    mgr/metadata.c
    mgr/nm_mgr_print.c
    mgr/nm_mgr_rx.c
    mgr/nm_mgr_sql.c
    mgr/nm_mgr_ui.c
    mgr/nmmgr.c
    mgr/ui_input.c
  )
  if(civetweb_FOUND)
    list(APPEND HFILES mgr/nm_rest.h)
    list(APPEND CFILES mgr/nm_rest.c)
  endif(civetweb_FOUND)
  
  add_library(nmmgr ${CFILES} ${HFILES})
  if(MYSQLCLIENT_FOUND)
    message(STATUS "Manager using MySQL client")
    target_compile_definitions(nmmgr PUBLIC HAVE_MYSQL)
    target_link_libraries(nmmgr PUBLIC PkgConfig::MYSQLCLIENT)
  elseif(PostgreSQL_FOUND)
    message(STATUS "Manager using POSTGRES client")
    target_compile_definitions(nmmgr PUBLIC HAVE_POSTGRESQL)
    target_link_libraries(nmmgr PUBLIC PostgreSQL::PostgreSQL)
  endif(MYSQLCLIENT_FOUND)
  if(cJSON_FOUND)
    message(STATUS "Manager using cJSON")
    target_compile_definitions(nmmgr PUBLIC USE_JSON)
    target_link_libraries(nmmgr PUBLIC cjson)
  endif(cJSON_FOUND)
  if(civetweb_FOUND)
    message(STATUS "Manager using civetweb")
    target_compile_definitions(nmmgr PUBLIC USE_CIVETWEB NO_SSL)
    target_link_libraries(nmmgr PUBLIC civetweb)
  endif(civetweb_FOUND)
  target_link_libraries(nmmgr PUBLIC nmcommon)
  target_include_directories(nmmgr PUBLIC
      ${CMAKE_CURRENT_SOURCE_DIR}/mgr
  )

  set(HFILES
    ion_if/ion_if.h
  )
  set(CFILES
    ion_if/ion_if.c
    ion_if/nm_mgr.c
  )
  

  
  add_executable(ion_nm_mgr ${CFILES} ${HFILES})
  target_link_libraries(ion_nm_mgr nmmgr)
  target_link_libraries(ion_nm_mgr ION::BP)

  install(
    TARGETS nmmgr ion_nm_mgr
    RUNTIME
  )
  
endif(BUILD_MANAGER)

endif(FALSE)
