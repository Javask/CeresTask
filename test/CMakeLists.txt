cmake_minimum_required(VERSION 3.18)

project(CeresTaskTests)

#add Catch2
CPMAddPackage("gh:catchorg/Catch2@2.13.8")

file(GLOB_RECURSE CERESTASK_TEST_FILES CONFIGURE_DEPENDS "${CMAKE_CURRENT_LIST_DIR}/*.h"
							   "${CMAKE_CURRENT_LIST_DIR}/*.cpp")


add_executable(CeresTaskTests ${CERESTASK_TEST_FILES})
target_link_libraries(CeresTaskTests Catch2::Catch2 CeresTask::CeresTask)
add_dependencies(CeresTaskTests Catch2::Catch2 CeresTask::CeresTask)
set_target_properties(CeresTaskTests PROPERTIES CXX_STANDARD 20)


enable_testing()
add_test(CeresTaskTests CeresLogTests)