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

add_subdirectory(src)

file(GLOB_RECURSE color4_TEST_FILES
     LIST_DIRECTORIES false
     src/*_test.cc)

include(GoogleTest)
enable_testing()
add_executable(color4_test ${googletest_DRIVER} ${color4_TEST_FILES})
target_link_libraries(color4_test gtest gtest_main)
gtest_discover_tests(color4_test)

