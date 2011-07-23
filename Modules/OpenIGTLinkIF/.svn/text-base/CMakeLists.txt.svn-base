PROJECT(OpenIGTLinkIF)

CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
IF(COMMAND CMAKE_POLICY)
  CMAKE_POLICY(SET CMP0003 NEW)
ENDIF(COMMAND CMAKE_POLICY)
MARK_AS_ADVANCED(CMAKE_BACKWARDS_COMPATIBILITY)



# --------------------------------------------------------------------------
# Find Slicer3

if(NOT Slicer3_SOURCE_DIR)
  find_package(Slicer3 REQUIRED)
  include(${Slicer3_USE_FILE})
  slicer3_set_default_install_prefix_for_external_projects()
endif(NOT Slicer3_SOURCE_DIR)


# --------------------------------------------------------------------------
# Check if version 2 protocol is available
# Features that depnd on OpenIGTLink Protocol 2 are only available 
# if the OpenIGTLink library is compiled with OpenIGTLink_PROTOCOL_VERSION_2
# option.

if (OpenIGTLink_PROTOCOL_VERSION EQUAL 2)
  ADD_DEFINITIONS(-DOpenIGTLinkIF_USE_VERSION_2)
  ADD_DEFINITIONS(-DOpenIGTLinkIF_LIBRARY_VERSION="${OpenIGTLink_VERSION_MAJOR}-${OpenIGTLink_VERSION_MINOR}-${OpenIGTLink_VERSION_PATCH} for protocol ver. 2")
else (OpenIGTLink_PROTOCOL_VERSION EQUAL 2)
  ADD_DEFINITIONS(-DOpenIGTLinkIF_LIBRARY_VERSION="${OpenIGTLink_VERSION_MAJOR}-${OpenIGTLink_VERSION_MINOR}-${OpenIGTLink_VERSION_PATCH} for protocol ver. 1")
endif (OpenIGTLink_PROTOCOL_VERSION EQUAL 2)




# --------------------------------------------------------------------------
# Module Revision Number (for development purpose)
#find_package(Subversion)
#if (Subversion_FOUND)
#  Subversion_WC_INFO(${CMAKE_HOME_DIRECTORY} OpenIGTLinkIF)
#  #set(OpenIGTLinkIF_REVISION "${OpenIGTLinkIF_WC_REVISION}")
#  ADD_DEFINITIONS(-DOpenIGTLinkIF_REVISION=${OpenIGTLinkIF_WC_REVISION})
#else (Subversion_FOUND)
#  ADD_DEFINITIONS(-DOpenIGTLinkIF_REVISION="Unknown")
#endif (Subversion_FOUND)


# --------------------------------------------------------------------------
# Include dirs

include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_BINARY_DIR}
  ${Slicer3_Libs_INCLUDE_DIRS}
  ${Slicer3_Base_INCLUDE_DIRS}
  ${OpenIGTLink_INCLUDE_DIRS}
  )

#
# Configure include file
#
configure_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/vtkOpenIGTLinkIFConfigure.h.in 
  ${CMAKE_CURRENT_BINARY_DIR}/vtkOpenIGTLinkIFConfigure.h
  )

file(GLOB headers 
  "${CMAKE_CURRENT_SOURCE_DIR}/*.h"
  )

install(FILES 
  ${headers} 
  "${CMAKE_CURRENT_BINARY_DIR}/vtkOpenIGTLinkIFConfigure.h"
  DESTINATION ${Slicer3_INSTALL_MODULES_INCLUDE_DIR}/${PROJECT_NAME} COMPONENT Development
  )


# --------------------------------------------------------------------------
# Sources

set(OpenIGTLinkIF_SRCS 
  vtkOpenIGTLinkIFLogic.cxx
  vtkOpenIGTLinkIFGUI.cxx
  #vtkIGTLConnector.cxx
  vtkIGTLCircularBuffer.cxx
  vtkIGTLToMRMLBase.cxx
  vtkIGTLToMRMLLinearTransform.cxx
  vtkIGTLToMRMLImage.cxx
  vtkIGTLToMRMLPosition.cxx
  vtkMRMLIGTLConnectorNode.cxx
  vtkIGTLTestWindow.cxx
)

if (OpenIGTLink_PROTOCOL_VERSION EQUAL 2)
  set(OpenIGTLinkIF_SRCS 
    ${OpenIGTLinkIF_SRCS}
    vtkIGTLRemoteDataListWindow.cxx
    vtkMRMLIGTLQueryNode.cxx
    vtkMRMLImageMetaListNode.cxx
    vtkIGTLToMRMLImageMetaList.cxx
    #vtkMRMLIGTLImageMetaListQueryNode.cxx
    vtkIGTLToMRMLTrackingData.cxx
    vtkMRMLIGTLTrackingDataBundleNode.cxx
    vtkIGTLTrackingDataControllerWindow.cxx
    )
endif (OpenIGTLink_PROTOCOL_VERSION EQUAL 2)


# --------------------------------------------------------------------------
# Wrapping

include("${VTK_CMAKE_DIR}/vtkWrapTcl.cmake")
vtk_wrap_tcl3(OpenIGTLinkIF OpenIGTLinkIF_TCL_SRCS "${OpenIGTLinkIF_SRCS}" "")



#---------------------------------------------------------------------------
# Add Loadable Module support

generatelm(OpenIGTLinkIF_SRCS OpenIGTLinkIF.txt)

# --------------------------------------------------------------------------
# Build and install the library

set(lib_name OpenIGTLinkIF)

add_library(${lib_name}
  ${OpenIGTLinkIF_SRCS} 
  ${OpenIGTLinkIF_TCL_SRCS}
  )
slicer3_set_modules_output_path(${lib_name})


target_link_libraries(${lib_name}
  ${Slicer3_Libs_LIBRARIES}
  ${Slicer3_Base_LIBRARIES}
  ${KWWidgets_LIBRARIES}
  ${ITK_LIBRARIES}
  ${OpenIGTLink_LIBRARIES}
  )
slicer3_install_modules(${lib_name})

# --------------------------------------------------------------------------
# Testing

if(BUILD_TESTING)
   ADD_SUBDIRECTORY(Testing)
endif(BUILD_TESTING)
