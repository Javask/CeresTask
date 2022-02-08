cmake_minimum_required(VERSION 3.22)

project(CeresTask VERSION 1.0.0 LANGUAGES CXX)
include(${CMAKE_CURRENT_LIST_DIR}/cmake/CPM.cmake)

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
set_target_properties(CeresTask PROPERTIES CXX_STANDARD 20)

target_include_directories(CeresTask
    PUBLIC 
        $<INSTALL_INTERFACE:include>    
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

add_library(CeresTask::CeresTask ALIAS CeresTask)
add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/test/")


