set(UNITY_ROOT "${CMAKE_SOURCE_DIR}/deps/unity")
find_package(unitytools)

add_unity_test(SOURCE "test_rda.c" thunk.c)
target_link_libraries(test_rda PUBLIC nmagent)
