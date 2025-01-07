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

# NM Manager library without concrete messaging interface
set(CIVETWEB_FOUND ${civetweb_FOUND})
configure_file("config.h.in" "config.h")
set(HFILES
  ${CMAKE_CURRENT_BINARY_DIR}/config.h
  instr.h
  agents.h
  ingress.h
#  metadata.h
#  nm_mgr_sql.h
  mgr.h
)
set(CFILES
  instr.c
  agents.c
  ingress.c
#  metadata.c
#  nm_mgr_sql.c
  mgr.c
)
if(REFDM_UI_CLI)
  list(APPEND HFILES
    nm_mgr_print.h
    nm_mgr_ui.h
    ui_input.h
  )
  list(APPEND CFILES
    nm_mgr_print.c
    nm_mgr_ui.c
    ui_input.c
  )
endif(REFDM_UI_CLI)
if(civetweb_FOUND)
  list(APPEND HFILES nm_rest.h)
  list(APPEND CFILES nm_rest.c)
endif(civetweb_FOUND)

add_library(refdm)

target_sources(refdm PRIVATE ${HFILES})
target_sources(refdm PRIVATE ${CFILES})
target_include_directories(
  refdm PUBLIC
  $<INSTALL_INTERFACE:$<INSTALL_PREFIX>/include>
)
target_link_libraries(refdm PUBLIC cace)

if(MYSQLCLIENT_FOUND)
  message(STATUS "Manager using MySQL client")
#FIXME  target_compile_definitions(refdm PUBLIC HAVE_MYSQL)
  target_link_libraries(refdm PUBLIC PkgConfig::MYSQLCLIENT)
elseif(PostgreSQL_FOUND)
  message(STATUS "Manager using POSTGRES client")
#FIXME  target_compile_definitions(refdm PUBLIC HAVE_POSTGRESQL)
  target_link_libraries(refdm PUBLIC PostgreSQL::PostgreSQL)
else()
  message(WARNING "Manager not using any database client")
endif()

if(cJSON_FOUND)
  message(STATUS "Manager using cJSON")
  target_compile_definitions(refdm PUBLIC USE_JSON)
  target_link_libraries(refdm PUBLIC cjson)
endif(cJSON_FOUND)

if(civetweb_FOUND)
  message(STATUS "Manager using civetweb")
  target_compile_definitions(refdm PUBLIC USE_CIVETWEB NO_SSL)
  target_link_libraries(refdm PUBLIC civetweb)
endif(civetweb_FOUND)


include(GNUInstallDirs)
install(TARGETS refdm)
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
