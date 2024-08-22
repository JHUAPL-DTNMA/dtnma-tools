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
set(UNITY_ROOT "${CMAKE_SOURCE_DIR}/deps/unity")
find_package(unitytools)


add_unity_test(SOURCE "cace/test_ari_cbor.c")
target_link_libraries(test_ari_cbor PUBLIC cace)

add_unity_test(SOURCE "cace/test_ari_text.c")
target_link_libraries(test_ari_text PUBLIC cace)

add_unity_test(SOURCE "cace/test_ari_text_util.c")
target_link_libraries(test_ari_text_util PUBLIC cace)

add_unity_test(SOURCE "cace/test_ari_text_cbor_loopback.c")
target_link_libraries(test_ari_text_cbor_loopback PUBLIC cace)

add_unity_test(SOURCE "cace/test_ari_algo.c")
target_link_libraries(test_ari_algo PUBLIC cace)

add_unity_test(SOURCE "cace/test_amm_typing.c")
target_link_libraries(test_amm_typing PUBLIC cace)

add_unity_test(SOURCE "cace/test_amm_parameters.c")
target_link_libraries(test_amm_parameters PUBLIC cace)

add_unity_test(SOURCE "cace/test_amm_const.c")
target_link_libraries(test_amm_const PUBLIC cace)


if(FALSE) # Disabled for test

add_unity_test(SOURCE "test_rda.c" thunk.c MAIN_NAME unity_main)
target_link_libraries(test_rda PUBLIC nmagent)

endif(FALSE)
