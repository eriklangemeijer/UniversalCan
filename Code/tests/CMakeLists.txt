cmake_minimum_required(VERSION 3.9)
set(CMAKE_CXX_STANDARD 11)

set(UniversalCan_TEST_SRC
    AllTests.cpp
    ModifierFunctionTests.cpp
)
set(UniversalCan_TEST_HEADER
)
PREPEND(UniversalCan_TEST_SRC)
list(APPEND UniversalCan_TEST_HEADER ${UniversalCan_INC})

include(CTest)
include(GoogleTest)
add_executable(TestUniversalCan ${UniversalCan_TEST_SRC} ${UniversalCan_SRC} ${UniversalCan_TEST_HEADER})

target_link_libraries(
    TestUniversalCan PRIVATE
    gtest
    gtest_main
)
enable_testing()
gtest_discover_tests(TestUniversalCan)

target_include_directories(TestUniversalCan PUBLIC
   # headers to include when building from source
   $<BUILD_INTERFACE:${UniversalCan_SOURCE_DIR}/include> 
   $<BUILD_INTERFACE:${UniversalCan_BINARY_DIR}/include> 

   # headers to include when installing  (implicitly prefixes with ${CMAKE_INSTALL_PREFIX}).
   $<INSTALL_INTERFACE:include> 
   )

target_link_libraries(TestUniversalCan PRIVATE pugixml)