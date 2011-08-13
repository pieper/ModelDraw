cmake_minimum_required (VERSION 2.8)
project (ANTS4Slicer)

message("APPLE :                         ${APPLE}")
message("BORLAND :                       ${BORLAND}")
message("CMAKE_CL_64 :                   ${CMAKE_CL_64}")
message("CMAKE_COMPILER_2005 :           ${CMAKE_COMPILER_2005}")
message("CMAKE_HOST_APPLE :              ${CMAKE_HOST_APPLE}")
message("CMAKE_HOST_SYSTEM :             ${CMAKE_HOST_SYSTEM}")
message("CMAKE_HOST_SYSTEM_NAME :        ${CMAKE_HOST_SYSTEM_NAME}")
message("CMAKE_HOST_SYSTEM_PROCESSOR :   ${CMAKE_HOST_SYSTEM_PROCESSOR}")
message("CMAKE_HOST_SYSTEM_VERSION :     ${CMAKE_HOST_SYSTEM_VERSION}")
message("CMAKE_HOST_UNIX :               ${CMAKE_HOST_UNIX}")
message("CMAKE_HOST_WIN32 :              ${CMAKE_HOST_WIN32}")
message("CMAKE_SYSTEM :                  ${CMAKE_SYSTEM}")
message("CMAKE_SYSTEM_NAME :             ${CMAKE_SYSTEM_NAME}")
message("CMAKE_SYSTEM_PROCESSOR(target): ${CMAKE_SYSTEM_PROCESSOR}")
message("CMAKE_SYSTEM_VERSION :          ${CMAKE_SYSTEM_VERSION}")
message("CYGWIN :                        ${CYGWIN}")
message("UNIX :                          ${UNIX}")
message("WIN32 :                         ${WIN32}")
message("MSVC :                          ${MSVC}")

# --------------------------------------------------------------------------
# Find Slicer3

if(NOT Slicer3_SOURCE_DIR)
  find_package(Slicer3 REQUIRED)
  include(${Slicer3_USE_FILE})
  slicer3_set_default_install_prefix_for_external_projects()
endif(NOT Slicer3_SOURCE_DIR)

#http://cmake.org/gitweb?p=cmake.git;a=blob;f=Modules/ExternalProject.cmake
SET(CMAKE_MODULE_PATH
  ${CMAKE_CURRENT_SOURCE_DIR}/CMake
  ${CMAKE_MODULE_PATH}
  )
include(ExternalProject)

message("######### Slicer3_INSTALL_PLUGINS_BIN_DIR:      ${Slicer3_INSTALL_PLUGINS_BIN_DIR}")
message("######### CMAKE_CURRENT_BINARY_DIR:             ${CMAKE_CURRENT_BINARY_DIR}")
message("######### CMAKE_CURRENT_SOURCE_DIR:             ${CMAKE_CURRENT_SOURCE_DIR}")
message("######### LOCAL_CMAKE_BUILD_OPTIONS:            ${LOCAL_CMAKE_BUILD_OPTIONS}")


#############################################################################################

set(ITKv4_REPOSITORY git://itk.org/ITK.git)
set(ITKv4_GIT_TAG ITK_Slicer)

set(PROJECTNAME ITK4)
ExternalProject_Add(${PROJECTNAME}
  GIT_REPOSITORY ${ITKv4_REPOSITORY}
#  GIT_TAG ${ITKv4_GIT_TAG}
  CMAKE_COMMAND /usr/bin/cmake
  UPDATE_COMMAND git submodule update --init
  CMAKE_ARGS
    ${LOCAL_CMAKE_BUILD_OPTIONS}
    -DCMAKE_INSTALL_PREFIX:PATH=${PROJECTNAME}-install
    -DCMAKE_BUILD_TYPE:STRING=Debug
    -DITK_BUILD_ALL_MODULES:BOOL=ON
    -DBUILD_TESTING:BOOL=ON
    -DBUILD_SHARED_LIBS:BOOL=ON
)


set(PROJECTNAME ANTSExternal)
ExternalProject_Add(${PROJECTNAME}
  DEPENDS ITK4
  SVN_REPOSITORY "https://advants.svn.sourceforge.net/svnroot/advants"
  CMAKE_COMMAND /usr/bin/cmake
  CMAKE_ARGS
    ${LOCAL_CMAKE_BUILD_OPTIONS}
    -DCMAKE_INSTALL_PREFIX:PATH=${PROJECTNAME}-install
    -DCMAKE_BUILD_TYPE:STRING=Debug
    -DITK_DIR:PATH=${CMAKE_CURRENT_BINARY_DIR}/ITK4-prefix/src/ITK4-build
)

install(
  DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/${PROJECTNAME}-prefix/src/${PROJECTNAME}-build/${PROJECTNAME}-install/bin/
  DESTINATION ${Slicer3_INSTALL_PLUGINS_BIN_DIR}
  FILES_MATCHING PATTERN "*"
  PERMISSIONS OWNER_EXECUTE OWNER_WRITE OWNER_READ
              GROUP_EXECUTE GROUP_READ
              WORLD_EXECUTE WORLD_READ
)

#install(
#  DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/${PROJECTNAME}-prefix/src/${PROJECTNAME}-build/${PROJECTNAME}-install/lib/
#  DESTINATION ${Slicer3_INSTALL_PLUGINS_BIN_DIR}
#  FILES_MATCHING PATTERN "*"
#  PERMISSIONS OWNER_EXECUTE OWNER_WRITE OWNER_READ
#              GROUP_EXECUTE GROUP_READ
#              WORLD_EXECUTE WORLD_READ
#)

