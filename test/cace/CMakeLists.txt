#[[
Copyright (c) 2011-2025 The Johns Hopkins University Applied Physics
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

if(BUILD_UNITTEST AND NOT BUILD_FUZZING)
  add_unity_test(SOURCE "test_ari_cbor.c")
  target_link_libraries(test_ari_cbor PUBLIC cace)
  
  if(ARI_TEXT_PARSE)
  add_unity_test(SOURCE "test_ari_text.c")
  target_link_libraries(test_ari_text PUBLIC cace)
  
  add_unity_test(SOURCE "test_ari_text_util.c")
  target_link_libraries(test_ari_text_util PUBLIC cace)
  
  add_unity_test(SOURCE "test_ari_roundtrip.c")
  target_link_libraries(test_ari_roundtrip PUBLIC cace)
  endif(ARI_TEXT_PARSE)
  
  add_unity_test(SOURCE "test_ari_algo.c")
  target_link_libraries(test_ari_algo PUBLIC cace)
  
  add_unity_test(SOURCE "test_amm_semtype_cnst.c")
  target_link_libraries(test_amm_semtype_cnst PUBLIC cace)
  
  add_unity_test(SOURCE "test_amm_typing.c")
  target_link_libraries(test_amm_typing PUBLIC cace)
  
  add_unity_test(SOURCE "test_amm_parameters.c")
  target_link_libraries(test_amm_parameters PUBLIC cace)
  
  add_unity_test(SOURCE "test_amm_obj_ns.c")
  target_link_libraries(test_amm_obj_ns PUBLIC cace)
  
  add_unity_test(SOURCE "test_amm_lookup.c")
  target_link_libraries(test_amm_lookup PUBLIC cace)
  
  add_unity_test(SOURCE "test_amp_socket.c")
  target_link_libraries(test_amp_socket PUBLIC cace)
  
  # Test just being able to compile and link a C++ user with the cace library
  add_executable(test_cace_cpp)
  target_sources(test_cace_cpp PRIVATE test_cace_cpp.cpp)
  target_link_libraries(test_cace_cpp PUBLIC cace)
endif(BUILD_UNITTEST AND NOT BUILD_FUZZING)

if(BUILD_FUZZING)
  add_fuzz_test(SOURCE "fuzz_cace_ari_cbor.cpp" RUNS_COUNT 1000000)
  target_link_libraries(fuzz_cace_ari_cbor PUBLIC cace)

  add_fuzz_test(SOURCE "fuzz_cace_ari_uri.cpp" RUNS_COUNT 300000 EXTRA_ARGS "-only_ascii=1")
  target_link_libraries(fuzz_cace_ari_uri PUBLIC cace)
endif(BUILD_FUZZING)
