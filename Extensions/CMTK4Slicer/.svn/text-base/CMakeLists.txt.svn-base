cmake_minimum_required (VERSION 2.8)
project (CMTK4Slicer)

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

if (${UNIX})
  if ( "${CMAKE_SYSTEM_PROCESSOR}" STREQUAL x86_64 )
    set (LOCAL_CMAKE_BUILD_OPTIONS -C ${CMAKE_CURRENT_BINARY_DIR}/CMTK-prefix/src/CMTK/config/gcc-x86_64-sse.cmake)
  else ( "${CMAKE_SYSTEM_PROCESSOR}" STREQUAL x86_64 )
    set (LOCAL_CMAKE_BUILD_OPTIONS -C ${CMAKE_CURRENT_BINARY_DIR}/CMTK-prefix/src/CMTK/config/gcc-p4-sse.cmake)
  endif ( "${CMAKE_SYSTEM_PROCESSOR}" STREQUAL x86_64 )
endif (${UNIX})

if (${APPLE})
  SET(LOCAL_CMAKE_BUILD_OPTIONS ${LOCAL_CMAKE_BUILD_OPTIONS} -DCMTK_USE_OPENMP:BOOL=OFF)
endif (${APPLE})

# this is not optimal but should work for now
if (${MSVC})
   set(LOCAL_CMAKE_BUILD_OPTIONS ${LOCAL_CMAKE_BUILD_OPTIONS} -DCMAKE_CXX_FLAGS:STRING=/arch:SSE2)
   set(LOCAL_CMAKE_BUILD_OPTIONS ${LOCAL_CMAKE_BUILD_OPTIONS} -DCMTK_USE_OPENMP:BOOL=ON)
endif (${MSVC})

message("######### LOCAL_CMAKE_BUILD_OPTIONS:            ${LOCAL_CMAKE_BUILD_OPTIONS}")


#############################################################################################

set(PROJECTNAME CMTK)
ExternalProject_Add(${PROJECTNAME}
  SVN_REPOSITORY "http://www.nitrc.org/svn/cmtk/tags/Slicer3"
#  SVN_REPOSITORY "http://www.nitrc.org/svn/cmtk/trunk/core"
  SVN_USERNAME "slicerbot"
  SVN_PASSWORD "slicer"
  CMAKE_ARGS
    ${LOCAL_CMAKE_BUILD_OPTIONS}
    -DCMAKE_INSTALL_PREFIX:PATH=${PROJECTNAME}-install
    -DCMAKE_BUILD_TYPE:STRING=Release
)

install(
  DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/${PROJECTNAME}-prefix/src/${PROJECTNAME}-build/${PROJECTNAME}-install/bin/
  DESTINATION ${Slicer3_INSTALL_PLUGINS_BIN_DIR}
  FILES_MATCHING PATTERN "*"
  PERMISSIONS OWNER_EXECUTE OWNER_WRITE OWNER_READ
              GROUP_EXECUTE GROUP_READ
              WORLD_EXECUTE WORLD_READ
)

install(
  DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/${PROJECTNAME}-prefix/src/${PROJECTNAME}-build/${PROJECTNAME}-install/lib/
  DESTINATION ${Slicer3_INSTALL_PLUGINS_BIN_DIR}
  FILES_MATCHING PATTERN "*"
  PERMISSIONS OWNER_EXECUTE OWNER_WRITE OWNER_READ
              GROUP_EXECUTE GROUP_READ
              WORLD_EXECUTE WORLD_READ
)

