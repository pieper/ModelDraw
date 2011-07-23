
#-----------------------------------------------------------------------------
# Get and build teem

set(proj teem)

if(WIN32)
  set(teem_ZLIB_LIBRARY ${VTK_DIR}/bin/${CMAKE_CFG_INTDIR}/vtkzlib.lib)
  set(teem_PNG_LIBRARY ${VTK_DIR}/bin/${CMAKE_CFG_INTDIR}/vtkpng.lib)
elseif(APPLE)
  set(teem_ZLIB_LIBRARY ${VTK_DIR}/bin/libvtkzlib.dylib)
  set(teem_PNG_LIBRARY ${VTK_DIR}/bin/libvtkpng.dylib)
else()
  set(teem_ZLIB_LIBRARY ${VTK_DIR}/bin/libvtkzlib.so)
  set(teem_PNG_LIBRARY ${VTK_DIR}/bin/libvtkpng.so)
endif()

ExternalProject_Add(${proj}
  SVN_REPOSITORY "http://teem.svn.sourceforge.net/svnroot/teem/teem/tags/1.10.0"
  UPDATE_COMMAND ""
  SOURCE_DIR teem
  BINARY_DIR teem-build
  CMAKE_GENERATOR ${gen}
  CMAKE_ARGS
    -DCMAKE_CXX_FLAGS:STRING=${CMAKE_CXX_FLAGS}
    -DCMAKE_C_FLAGS:STRING=${CMAKE_C_FLAGS}
    -DCMAKE_BUILD_TYPE:STRING=${CMAKE_BUILD_TYPE}
    -DCMAKE_VERBOSE_MAKEFILE:BOOL=OFF
    -DBUILD_SHARED_LIBS:BOOL=ON
    -DBUILD_TESTING:BOOL=ON
    -DTeem_PTHREAD:BOOL=OFF
    -DTeem_BZIP2:BOOL=OFF
    -DTeem_ZLIB:BOOL=ON
    -DTeem_PNG:BOOL=ON
    -DTeem_VTK_MANGLE:BOOL=ON
    -DTeem_VTK_TOOLKITS_IPATH:FILEPATH=${VTK_DIR}
    -DZLIB_INCLUDE_DIR:PATH=${VTK_SOURCE_DIR}/Utilities
    -DTeem_VTK_ZLIB_MANGLE_IPATH:PATH=${VTK_SOURCE_DIR}/Utilities/vtkzlib
    -DTeem_ZLIB_DLLCONF_IPATH:PATH=${VTK_DIR}/Utilities
    -DZLIB_LIBRARY:FILEPATH=${teem_ZLIB_LIBRARY}
    -DPNG_PNG_INCLUDE_DIR:PATH=${VTK_SOURCE_DIR}/Utilities/vtkpng
    -DTeem_PNG_DLLCONF_IPATH:PATH=${VTK_DIR}/Utilities
    -DPNG_LIBRARY:FILEPATH=${teem_PNG_LIBRARY}
    -DTeem_USE_LIB_INSTALL_SUBDIR:BOOL=ON
  INSTALL_COMMAND ""
  DEPENDS 
    ${teem_DEPENDENCIES}
)
