project(AtlasCreatorCxx)

cmake_minimum_required(VERSION 2.4)
if(COMMAND cmake_policy)
  cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)

# --------------------------------------------------------------------------
# Find Slicer3

if(NOT Slicer3_SOURCE_DIR)
  find_package(Slicer3 REQUIRED)
  include(${Slicer3_USE_FILE})
  slicer3_set_default_install_prefix_for_external_projects()
endif(NOT Slicer3_SOURCE_DIR)

#-----------------------------------------------------------------------------
# avoid linker bug in Mac OS 10.5
# see:
# http://wiki.finkproject.org/index.php/Fink:Packaging:Preparing_for_10.5#OpenGL_Bug
#
if(APPLE)
  set(CMAKE_SHARED_LINKER_FLAGS "-Wl,-dylib_file,/System/Library/Frameworks/OpenGL.framework/Versions/A/Libraries/libGL.dylib:/System/Library/Frameworks/OpenGL.framework/Versions/A/Libraries/libGL.dylib")
  set(CMAKE_EXE_LINKER_FLAGS "-Wl,-dylib_file,/System/Library/Frameworks/OpenGL.framework/Versions/A/Libraries/libGL.dylib:/System/Library/Frameworks/OpenGL.framework/Versions/A/Libraries/libGL.dylib")
endif(APPLE)


# --------------------------------------------------------------------------
# Include dirs

subDirs(
  CongealingCLI
)

include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_BINARY_DIR}
  ${Slicer3_Libs_INCLUDE_DIRS}
  ${Slicer3_Base_INCLUDE_DIRS}
  ${vtkSlicerVolumesModuleLogic_SOURCE_DIR}
  ${vtkSlicerVolumesModuleLogic_BINARY_DIR}  
  )

configure_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/vtkAtlasCreatorCxxModuleConfigure.h.in 
  ${CMAKE_CURRENT_BINARY_DIR}/vtkAtlasCreatorCxxModuleConfigure.h
  )

file(GLOB headers "${CMAKE_CURRENT_SOURCE_DIR}/*.h")

install(FILES 
  ${headers}
  "${CMAKE_CURRENT_BINARY_DIR}/vtkAtlasCreatorCxxModuleConfigure.h"
  DESTINATION ${Slicer3_INSTALL_MODULES_INCLUDE_DIR}/${PROJECT_NAME} COMPONENT Development
  )

# --------------------------------------------------------------------------
# Sources

set(AtlasCreatorCxxModule_SRCS
  vtkAtlasCreatorCxxModuleLogic.cxx
  vtkMRMLAtlasCreatorNode.cxx
  vtkImageKilianDistanceTransform.cxx
  vtkImagePCAFilter.cxx
  vtkImagePCATraining.cxx
  )

include("${VTK_CMAKE_DIR}/vtkWrapTcl.cmake")
vtk_wrap_tcl3(AtlasCreatorCxxModule 
  AtlasCreatorCxxModule_TCL_SRCS 
  "${AtlasCreatorCxxModule_SRCS}" "")

#---------------------------------------------------------------------------
# Add Loadable Module support

generatelm(AtlasCreatorCxxModule_SRCS 
  AtlasCreatorCxxModule.xml)

# --------------------------------------------------------------------------
# Build and install the library

set(lib_name AtlasCreatorCxxModule)
add_library(${lib_name}
  ${AtlasCreatorCxxModule_SRCS}
  ${AtlasCreatorCxxModule_TCL_SRCS}
  
  )
slicer3_set_modules_output_path(${lib_name})


target_link_libraries(${lib_name}
  ${Slicer3_Libs_LIBRARIES}
  ${Slicer3_Base_LIBRARIES}
  ${KWWidgets_LIBRARIES}
  ${ITK_LIBRARIES}
  vtkSlicerVolumesModuleLogic  
  )


slicer3_install_modules(${lib_name})

# --------------------------------------------------------------------------
# Testing 

if(BUILD_TESTING)
  ADD_SUBDIRECTORY(Testing)
endif(BUILD_TESTING)


