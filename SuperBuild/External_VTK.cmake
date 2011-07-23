#-----------------------------------------------------------------------------
# Get and build VTK
#

# Sanity checks
if(DEFINED VTK_DIR AND NOT EXISTS ${VTK_DIR})
  message(FATAL_ERROR "VTK_DIR variable is defined but corresponds to non-existing directory")
endif()

if(DEFINED VTK_SOURCE_DIR AND NOT EXISTS ${VTK_SOURCE_DIR})
  message(FATAL_ERROR "VTK_SOURCE_DIR variable is defined but corresponds to non-existing directory")
endif()

set(proj VTK)

if(NOT DEFINED VTK_DIR OR NOT DEFINED VTK_SOURCE_DIR)
#  message(STATUS "Adding project:${proj}")

  set(VTK_WRAP_TCL OFF)
  set(VTK_WRAP_PYTHON OFF)

  if(Slicer3_USE_KWWIDGETS)
    list(APPEND VTK_DEPENDENCIES tcl)
    if (NOT WIN32)
      list(APPEND VTK_DEPENDENCIES tk)
    endif(NOT WIN32)
    set(VTK_WRAP_TCL ON)
  endif(Slicer3_USE_KWWIDGETS)

  if (Slicer3_USE_PYTHONQT)
    set(VTK_WRAP_PYTHON ON)
    list(APPEND VTK_DEPENDENCIES python)
  endif(Slicer3_USE_PYTHONQT)

  set(VTK_PYTHON_ARGS)
  if(Slicer3_USE_PYTHON OR Slicer3_USE_PYTHONQT)
    set(VTK_PYTHON_ARGS
      -DPYTHON_INCLUDE_DIR:PATH=${slicer_PYTHON_INCLUDE}
      -DPYTHON_LIBRARY:FILEPATH=${slicer_PYTHON_LIBRARY}
      )
  endif(Slicer3_USE_PYTHON OR Slicer3_USE_PYTHONQT)

  # On Mac, since:
  #    - Qt can't be build with X11 support
  #    - KWWidgets only support X11
  # the case where both Slicer3_USE_QT and Slicer3_USE_KWWIDGETS are ON is *NOT* supported

  set(VTK_QT_ARGS)
  if(NOT APPLE)
    if(Slicer3_USE_QT)
      set(VTK_QT_ARGS
        -DDESIRED_QT_VERSION:STRING=4
        -DVTK_USE_GUISUPPORT:BOOL=ON
        -DVTK_USE_QVTK_QTOPENGL:BOOL=ON
        -DVTK_USE_QT:BOOL=ON
        -DQT_QMAKE_EXECUTABLE:FILEPATH=${QT_QMAKE_EXECUTABLE}
        )
    endif(Slicer3_USE_QT)
  else()
    if(Slicer3_USE_KWWIDGETS AND NOT Slicer3_USE_QT)
      set(VTK_QT_ARGS
        -DCMAKE_SHARED_LINKER_FLAGS:STRING="-Wl,-dylib_file,/System/Library/Frameworks/OpenGL.framework/Versions/A/Libraries/libGL.dylib:/System/Library/Frameworks/OpenGL.framework/Versions/A/Libraries/libGL.dylib"
        -DCMAKE_EXE_LINKER_FLAGS:STRING="-Wl,-dylib_file,/System/Library/Frameworks/OpenGL.framework/Versions/A/Libraries/libGL.dylib:/System/Library/Frameworks/OpenGL.framework/Versions/A/Libraries/libGL.dylib"
        -DOPENGL_INCLUDE_DIR:PATH=/usr/X11R6/include
        -DVTK_USE_CARBON:BOOL=OFF
        -DVTK_USE_COCOA:BOOL=OFF
        -DVTK_USE_X:BOOL=ON
        -DVTK_USE_RPATH:BOOL=ON
        -DVTK_USE_GUISUPPORT:BOOL=OFF
        -DVTK_USE_QVTK_QTOPENGL:BOOL=OFF
        -DVTK_USE_QT:BOOL=OFF
        )
    elseif(NOT Slicer3_USE_KWWIDGETS AND Slicer3_USE_QT)
      set(VTK_QT_ARGS
        -DVTK_USE_CARBON:BOOL=OFF
        -DVTK_USE_COCOA:BOOL=ON # Default to Cocoa, VTK/CMakeLists.txt will enable Carbon and disable cocoa if needed
        -DVTK_USE_X:BOOL=OFF
        -DVTK_USE_RPATH:BOOL=ON
        -DDESIRED_QT_VERSION:STRING=4
        -DVTK_USE_GUISUPPORT:BOOL=ON
        -DVTK_USE_QVTK_QTOPENGL:BOOL=ON
        -DVTK_USE_QT:BOOL=ON
        -DQT_QMAKE_EXECUTABLE:FILEPATH=${QT_QMAKE_EXECUTABLE}
        )
    elseif(Slicer3_USE_KWWIDGETS AND Slicer3_USE_QT)
      MESSAGE(FATAL_ERROR "Case where Slicer3_USE_QT and Slicer3_USE_KWWIDGETS are ON is not supported on MAC")
    endif()
  endif()

  # Disable Tk when Python wrapping is enabled
  if (Slicer3_USE_PYTHONQT)
    list(APPEND VTK_QT_ARGS -DVTK_USE_TK:BOOL=OFF)
  endif()

  set(slicer_TCL_LIB)
  set(slicer_TK_LIB)
  set(slicer_TCLSH)
  set(VTK_TCL_ARGS)
  if(VTK_WRAP_TCL)
    if(WIN32)
      set(slicer_TCL_LIB ${CMAKE_BINARY_DIR}/tcl-build/lib/tcl84.lib)
      set(slicer_TK_LIB ${CMAKE_BINARY_DIR}/tcl-build/lib/tk84.lib)
      set(slicer_TCLSH ${CMAKE_BINARY_DIR}/tcl-build/bin/tclsh.exe)
    elseif(APPLE)
      set(slicer_TCL_LIB ${CMAKE_BINARY_DIR}/tcl-build/lib/libtcl8.4.dylib)
      set(slicer_TK_LIB ${CMAKE_BINARY_DIR}/tcl-build/lib/libtk8.4.dylib)
      set(slicer_TCLSH ${CMAKE_BINARY_DIR}/tcl-build/bin/tclsh84)
    else()
      set(slicer_TCL_LIB ${CMAKE_BINARY_DIR}/tcl-build/lib/libtcl8.4.so)
      set(slicer_TK_LIB ${CMAKE_BINARY_DIR}/tcl-build/lib/libtk8.4.so)
      set(slicer_TCLSH ${CMAKE_BINARY_DIR}/tcl-build/bin/tclsh84)
    endif()
    set(VTK_TCL_ARGS
      -DTCL_INCLUDE_PATH:PATH=${CMAKE_BINARY_DIR}/tcl-build/include
      -DTK_INCLUDE_PATH:PATH=${CMAKE_BINARY_DIR}/tcl-build/include
      -DTCL_LIBRARY:FILEPATH=${slicer_TCL_LIB}
      -DTK_LIBRARY:FILEPATH=${slicer_TK_LIB}
      -DTCL_TCLSH:FILEPATH=${slicer_TCLSH}
      )
  endif()
  
  ExternalProject_Add(${proj}
    SOURCE_DIR ${CMAKE_BINARY_DIR}/${proj}
    BINARY_DIR ${proj}-build
    GIT_REPOSITORY "${git_protocol}://github.com/pieper/SlicerVTK.git"
    CMAKE_GENERATOR ${gen}
    CMAKE_ARGS
      -DCMAKE_CXX_FLAGS:STRING=${CMAKE_CXX_FLAGS}
      -DCMAKE_C_FLAGS:STRING=${CMAKE_C_FLAGS}
      -DBUILD_SHARED_LIBS:BOOL=ON
      -DCMAKE_BUILD_TYPE:STRING=${CMAKE_BUILD_TYPE}
      -DBUILD_EXAMPLES:BOOL=OFF
      -DBUILD_TESTING:BOOL=OFF
      -DVTK_USE_PARALLEL:BOOL=ON
      -DVTK_DEBUG_LEAKS:BOOL=${Slicer3_USE_VTK_DEBUG_LEAKS}
      -DVTK_WRAP_TCL:BOOL=${VTK_WRAP_TCL}
      -DVTK_USE_RPATH:BOOL=ON
      ${VTK_TCL_ARGS}
      -DVTK_WRAP_PYTHON:BOOL=${VTK_WRAP_PYTHON}
      ${VTK_PYTHON_ARGS}
      ${VTK_QT_ARGS}
      ${VTK_MAC_ARGS}
    INSTALL_COMMAND ""
    DEPENDS 
      ${VTK_DEPENDENCIES}
    )
  set(VTK_DIR ${CMAKE_BINARY_DIR}/${proj}-build)
  set(VTK_SOURCE_DIR ${CMAKE_BINARY_DIR}/${proj})

else()
  # The project is provided using VTK_DIR and VTK_SOURCE_DIR, nevertheless since other 
  # project may depend on VTK, let's add an 'empty' one
  ExternalProject_Add(${proj}
    SOURCE_DIR ${CMAKE_BINARY_DIR}/${proj}
    BINARY_DIR ${proj}-build
    DOWNLOAD_COMMAND ""
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ""
    INSTALL_COMMAND ""
    DEPENDS 
      ${VTK_DEPENDENCIES}
    )
endif()
