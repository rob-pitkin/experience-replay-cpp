add_executable(replay_buffer_tests hello_test.cpp)

target_link_libraries(replay_buffer_tests PRIVATE gtest_main)

include(GoogleTest)
gtest_discover_tests(replay_buffer_tests)
