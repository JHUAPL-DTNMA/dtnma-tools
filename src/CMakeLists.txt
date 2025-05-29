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

# All directories will be reflected in installed include path
include_directories(
  BEFORE
  ${CMAKE_CURRENT_BINARY_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}
)

add_subdirectory(cace)

if(BUILD_AGENT)
    add_subdirectory(refda)
    add_subdirectory(refda-stdio)
    if(TRANSPORT_UNIX_SOCKET)
        add_subdirectory(refda-socket)
    endif(TRANSPORT_UNIX_SOCKET)
    if(TRANSPORT_ION_BP)
        add_subdirectory(refda-ion)
    endif(TRANSPORT_ION_BP)
endif(BUILD_AGENT)

if(BUILD_MANAGER)
    add_subdirectory(refdm)
    if(TRANSPORT_UNIX_SOCKET)
        add_subdirectory(refdm-socket)
    endif(TRANSPORT_UNIX_SOCKET)
    if(TRANSPORT_ION_BP)
        add_subdirectory(refdm-ion)
    endif(TRANSPORT_ION_BP)
    if(TRANSPORT_PROXY_SOCKET)
        add_subdirectory(refdm-proxy)
    endif(TRANSPORT_PROXY_SOCKET)
endif(BUILD_MANAGER)

# Just the ION BP app proxy
if(BUILD_ION_PROXY)
    add_subdirectory(ion-app-proxy)
endif(BUILD_ION_PROXY)
