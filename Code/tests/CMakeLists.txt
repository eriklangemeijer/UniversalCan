cmake_minimum_required(VERSION 3.9)
set(CMAKE_CXX_STANDARD 11)

set(UniversalCan_TEST_SRC
    AllTests.cpp
)
# set(UniversalCan_TEST_HEADER
#     catch.hpp
# )

PREPEND(UniversalCan_TEST_SRC)

include(GoogleTest)
add_executable(TestUniversalCan ${UniversalCan_TEST_SRC} ${UniversalCan_TEST_HEADER})

target_link_libraries(
    TestUniversalCan
    gtest
    gtest_main
)
enable_testing()
add_test(NAME TestUniversalCan  COMMAND TestUniversalCan)
gtest_discover_tests(TestUniversalCan)
# target_link_libraries(TestUniversalCan UniversalCan)
