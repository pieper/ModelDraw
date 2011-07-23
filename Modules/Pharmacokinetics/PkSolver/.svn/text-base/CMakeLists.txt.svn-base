Project(PkSolver)

#
# Configure include file
#
configure_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/PkSolverConfigure.h.in 
  ${CMAKE_CURRENT_BINARY_DIR}/PkSolverConfigure.h
  )

include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_BINARY_DIR}
 )


ADD_LIBRARY( PkSolver PkSolver.cxx )
slicer3_set_modules_output_path(PkSolver)

TARGET_LINK_LIBRARIES( PkSolver ITKBasicFilters ITKCommon ITKIO ITKNumerics )

SUBDIRS ( exec )

install(FILES
    PkSolver.h
    PkSolverWin32Header.h
  "${CMAKE_CURRENT_BINARY_DIR}/PkSolverConfigure.h"
  DESTINATION ${Slicer3_INSTALL_MODULES_INCLUDE_DIR}/${PROJECT_NAME} COMPONENT Development
  )
    

install(TARGETS PkSolver
  RUNTIME DESTINATION ${Slicer3_INSTALL_MODULES_BIN_DIR} COMPONENT RuntimeLibraries 
  LIBRARY DESTINATION ${Slicer3_INSTALL_MODULES_LIB_DIR} COMPONENT RuntimeLibraries
  ARCHIVE DESTINATION ${Slicer3_INSTALL_MODULES_LIB_DIR} COMPONENT Development
  )

slicer3_install_modules(PkSolver)

