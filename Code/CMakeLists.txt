
cmake_minimum_required(VERSION 3.9)

set(CMAKE_CXX_STANDARD 17)

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/v1.15.2.zip
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

project(UniversalCan VERSION 1.0)
set(UniversalCan_VERSION 1.0)

include(CTest)

# Function to prepend the subdirectory to source files in subdirectories
FUNCTION(PREPEND var )
   SET(listVar "")
   FOREACH(f ${${var}})
       LIST(APPEND listVar "${CMAKE_CURRENT_SOURCE_DIR}/${f}")
   ENDFOREACH(f)
   SET(${var} "${listVar}" PARENT_SCOPE)
ENDFUNCTION(PREPEND)

# After a normal build, we can specify the location of various outputs of the
# build. We put executables and static libraries outside the build directory in
# bin/ and lib/, respectively.
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "bin")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "lib")

# Find LAPACK on the system. This is mostly for demonstration.
# find_package(LAPACK REQUIRED)

# Include source code and headers. This calls the CMakeLists.txt in each
# subdirectory. These can define their own libraries, executables, etc. as targets, 
# but here we define all exportable targets in the root CMakeLists.txt.
add_subdirectory(src)
add_subdirectory(include)

# Add the test directory. It is optional and can be disabled during with
#   cmake -DBUILD_TESTING=OFF ..
# To run unit tests produced here, we only need to run:
#   make test
# or
#   ctest 
# In case your tests are printing to console, you can view their output to
# stdout with:
#   ctest -V

if(BUILD_TESTING)
    add_subdirectory(tests)
endif()

# Add the library UniversalCan as a target, with the contents of src/ and include/
# as dependencies.
add_executable(ProtocolMapper src/ProtocolMapper.cpp ${UniversalCan_SRC} ${UniversalCan_INC})

# These variables slightly modify the install location to allow for version
# specific installations.
set(UniversalCan_INCLUDE_DEST "include/UniversalCan-${UniversalCan_VERSION}")
set(UniversalCan_LIB_DEST "lib/UniversalCan-${UniversalCan_VERSION}")


# generator expressions are needed for the include directories, since installing headers changes the include path
# Specify that UniversalCan requires the files located in the include/ directory at
# compile time. This would normally look like 
#   target_include_directories(UniversalCan PUBLIC include/)
# PUBLIC means that other libraries including UniversalCan should also include the
# directory include/.
# However, there is a catch. If we are installing the project in
# CMAKE_INSTALL_PREFIX, we can't specify include/ in the build directory: we have 
# copied the contents of include to CMAKE_INSTALL_PREFIX/include and we would
# like  other projects to include this directory instead of include/. The follow
# CMake command handles this. $<BUILD_INTERFACE:...> and
# $<INSTALL_INTERFACE:...> are macros whose values change depending on if we are
# simply building the code or if we are installing it.
target_include_directories(ProtocolMapper PUBLIC
   # headers to include when building from source
   $<BUILD_INTERFACE:${UniversalCan_SOURCE_DIR}/include> 
   $<BUILD_INTERFACE:${UniversalCan_BINARY_DIR}/include> 

   # headers to include when installing  (implicitly prefixes with ${CMAKE_INSTALL_PREFIX}).
   $<INSTALL_INTERFACE:include> 
   )

# Specify that UniversalCan requires LAPACK to link properly. Ideally, LAPACK would
# specify LAPACK::LAPACK for linking so that we can avoid using the variables.
# However, each package is different and one must check the documentation to 
# see what variables are defined.
target_link_libraries(ProtocolMapper ${LAPACK_LIBRARIES})

# Write a "version file" in case some wants to only load a particular version of
# UniversalCan @BUG this fails at the moment.
include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    UniversalCanConfigVersion.cmake
    VERSION ${UniversalCan_VERSION}
    COMPATIBILITY AnyNewerVersion
    )

