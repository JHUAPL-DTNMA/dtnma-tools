
add_unity_test(SOURCE "test_rda.c")
target_link_libraries(test_rda PUBLIC nmagent)
target_link_libraries(test_rda PUBLIC ION::ICI)
