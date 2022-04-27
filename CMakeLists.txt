cmake_minimum_required(VERSION 3.18)

project(CeresTask VERSION 1.0.0 LANGUAGES CXX)

option(CERESTASK_BUILD_OWN_PACKAGES "Build all dependencies from scratch." OFF)
if(NOT CERESTASK_BUILD_OWN_PACKAGES)
  set(CPM_USE_LOCAL_PACKAGES ON)
else()
  message(STATUS "Building CeresTask dependencies from scratch...")
  set(CPM_USE_LOCAL_PACKAGES OFF)
endif()


include(${CMAKE_CURRENT_LIST_DIR}/cmake/CPM.cmake)

set(LIBRARY_OUTPUT_PATH "${CMAKE_BINARY_DIR}")
set(EXECUTABLE_OUTPUT_PATH "${CMAKE_BINARY_DIR}")

CPMAddPackage(NAME CeresLog 
              GITHUB_REPOSITORY Javask/CeresLog
              GIT_TAG v1.0.0-alpha6
              OPTIONS
                "CERESLOG_BUILD_TESTS OFF")


set(CMAKE_CXX_FLAGS_ASAN "-fsanitize=address -fno-omit-frame-pointer -g -Wall")
set(CMAKE_EXE_LINKER_FLAGS_ASAN  "-fsanitize=address -g -Wall")


if(MSVC)
  set(CERES_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4 /EHsc")
else()
  set(CERES_CXX_FLAGS ${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic)
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_definitions(-DCERESTASK_DEBUG)
endif()

if(UNIX)
    add_definitions(-DCERESTASK_PLATFORM_LINUX)
elseif(WIN32)
    add_definitions(-DCERESTASK_PLATFORM_WINDOWS)
endif()

file(GLOB_RECURSE CERESTASK_SOURCE_FILES CONFIGURE_DEPENDS "${CMAKE_CURRENT_LIST_DIR}/src/*.cpp")
add_library(CeresTask STATIC ${CERESTASK_SOURCE_FILES})
set_target_properties(CeresTask PROPERTIES CXX_STANDARD 20 COMPILE_OPTIONS "${CERES_CXX_FLAGS}")
target_link_libraries(CeresTask CeresLog::CeresLog)

if(UNIX)
    target_link_libraries(CeresTask pthread)
endif()

target_include_directories(CeresTask
    PUBLIC 
        $<INSTALL_INTERFACE:include>    
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

add_library(CeresTask::CeresTask ALIAS CeresTask)

option(CERESTASK_BUILD_TESTS "Build CeresTask Tests" ON)
if(CERESTASK_BUILD_TESTS)
  enable_testing()
  add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/test/")
endif()


