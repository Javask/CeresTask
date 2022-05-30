cmake_minimum_required(VERSION 3.18)

project(CeresTaskTests)

#add Catch2
CPMAddPackage("gh:catchorg/Catch2@3.0.1")

file(GLOB_RECURSE CERESTASK_TEST_FILES CONFIGURE_DEPENDS "${CMAKE_CURRENT_LIST_DIR}/*.h"
							   "${CMAKE_CURRENT_LIST_DIR}/*.cpp")


add_executable(CeresTaskTests ${CERESTASK_TEST_FILES})
target_link_libraries(CeresTaskTests $<TARGET_NAME_IF_EXISTS:Catch2::Catch2WithMain> CeresTask::CeresTask Utilities::Utilities CeresLog::CeresLog)
add_dependencies(CeresTaskTests CeresTask::CeresTask)
set_target_properties(CeresTaskTests PROPERTIES CXX_STANDARD 20 COMPILE_OPTIONS "${CERES_CXX_FLAGS}")


add_test(CeresTaskTests ${EXECUTABLE_OUTPUT_PATH}/CeresTaskTests)