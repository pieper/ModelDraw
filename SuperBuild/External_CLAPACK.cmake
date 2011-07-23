
set(CLAPACK_version 3.2.1)
#set(CLAPACK_file "http://www.netlib.org/clapack/clapack-${clapack_version}-CMAKE.tgz")
# Since the netlib.org server has been down several time, especially when the nightly dashboard 
# started, we added a copy of the archive to slicer3 lib mirrors.
set(CLAPACK_file http://svn.slicer.org/Slicer3-lib-mirrors/trunk/clapack-${CLAPACK_version}-CMAKE.tgz)

# The CLAPACK external project for Titan

set(CLAPACK_source "${CMAKE_CURRENT_BINARY_DIR}/CLAPACK")
set(CLAPACK_binary "${CMAKE_CURRENT_BINARY_DIR}/CLAPACK-build")

# turn off the warnings for CLAPACK on windows
string(REPLACE "/W3" "/W0" CMAKE_CXX_FLAGS_CLAPACK "${CMAKE_CXX_FLAGS}")
string(REPLACE "/W4" "/W0" CMAKE_CXX_FLAGS_CLAPACK
  "${CMAKE_CXX_FLAGS_CLAPACK}")
string(REPLACE "/W3" "/W0" CMAKE_C_FLAGS_CLAPACK "${CMAKE_C_FLAGS}")
string(REPLACE "/W4" "/W0" CMAKE_C_FLAGS_CLAPACK
  "${CMAKE_C_FLAGS_CLAPACK}")
  
#
# To fix compilation problem: relocation R_X86_64_32 against `a local symbol' can not be
# used when making a shared object; recompile with -fPIC
# See http://www.cmake.org/pipermail/cmake/2007-May/014350.html
#
if(CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64")
  set(CMAKE_C_FLAGS_CLAPACK "${CMAKE_C_FLAGS_CLAPACK} -fPIC")
endif()
  
ExternalProject_Add(CLAPACK
  DOWNLOAD_DIR ${CMAKE_CURRENT_BINARY_DIR}
  SOURCE_DIR ${CLAPACK_source}
  BINARY_DIR ${CLAPACK_binary}
  URL ${CLAPACK_file}
  CMAKE_GENERATOR ${gen}
  CMAKE_ARGS
    -DCMAKE_CXX_FLAGS:STRING=${CMAKE_CXX_FLAGS_CLAPACK}
    -DCMAKE_C_FLAGS:STRING=${CMAKE_C_FLAGS_CLAPACK}
    -DBUILD_SHARED_LIBS:BOOL=OFF
    -DCMAKE_BUILD_TYPE:STRING=${CMAKE_BUILD_TYPE}
  INSTALL_COMMAND ""
  DEPENDS 
    ${CLAPACK_DEPENDENCIES}
  )
set(CLAPACK_DIR "${CLAPACK_binary}" CACHE PATH 
  "CLAPACK binary directory" FORCE)
mark_as_advanced(CLAPACK_DIR)
