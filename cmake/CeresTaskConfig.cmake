include(GNUInstallDirs)

install(TARGETS CeresTask
    EXPORT CeresTask-export
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

install(EXPORT CeresTask-targets
  FILE
    CeresTaskTargets.cmake
  NAMESPACE
    CeresTask::
  DESTINATION
    ${CMAKE_INSTALL_LIBDIR}/cmake/CeresTask
)

get_filename_component(CeresTask_CMAKE_DIR "${CMAKE_CURRENT_LIST_FILE}" PATH)
include(CMakeFindDependencyMacro)

if(NOT TARGET CeresTask::CeresTask)
    include("${CeresTask_CMAKE_DIR}/CeresTaskTargets.cmake")
endif()