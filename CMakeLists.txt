cmake_minimum_required(VERSION 3.13)
project(color4 CXX)
set(CMAKE_CXX_STANDARD_REQUIRED 14)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# import gtest

add_subdirectory(third_party/googletest)
set(googletest_DRIVER ${CMAKE_CURRENT_BINARY_DIR}/googletest_driver.cc)
file(WRITE ${googletest_DRIVER}
	"#include\"gtest/gtest.h\"\n\n"
	"int main(int argc, char* argv[]) {\n"
	"\t::testing::InitGoogleTest(&argc, argv);\n"
	"\treturn RUN_ALL_TESTS();\n"
	"}\n")

# import cityhash

add_library(cityhash STATIC third_party/cityhash/src/city.cc)
set(cityhash_GEN ${CMAKE_BINARY_DIR}/third_party/cityhash)
configure_file(cmake/cityhash_config.h.in ${cityhash_GEN}/config.h)
target_include_directories(cityhash PUBLIC ${cityhash_GEN})
target_include_directories(cityhash PUBLIC third_party/cityhash/src)

# import color4

file(GLOB_RECURSE color4_TEST_FILES
     LIST_DIRECTORIES false
     src/*_test.cc)

include(GoogleTest)
enable_testing()
add_executable(color4_test ${googletest_DRIVER} ${color4_TEST_FILES})
target_link_libraries(color4_test gtest gtest_main cityhash)
gtest_discover_tests(color4_test)

add_executable(color4_benchmark benchmark.cc)
target_link_libraries(color4_benchmark cityhash)
target_include_directories(color4_benchmark PUBLIC src)

