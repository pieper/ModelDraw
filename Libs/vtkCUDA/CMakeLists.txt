PROJECT(vtkCUDA)

CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
IF(COMMAND CMAKE_POLICY)
  CMAKE_POLICY(SET CMP0003 NEW)
ENDIF(COMMAND CMAKE_POLICY)
MARK_AS_ADVANCED(CMAKE_BACKWARDS_COMPATIBILITY)

find_package(VTK REQUIRED)
include(${VTK_USE_FILE})

INCLUDE(${CMAKE_ROOT}/Modules/TestBigEndian.cmake)
TEST_BIG_ENDIAN(CMAKE_WORDS_BIGENDIAN)

## ONLY BUILD THIS IF YOU WANT CUDA SUPPORT TO BE ENABLED
IF(CUDA_SUPPORT_ENABLED)

  INCLUDE(${CMAKE_SOURCE_DIR}/CMake/cuda/FindCuda.cmake)

  # TODO: fix for windows
  #configure_file(
  #${CMAKE_CURRENT_SOURCE_DIR}/vtkCUDAConfigure.h.in 
  #${CMAKE_CURRENT_BINARY_DIR}/vtkCUDAConfigure.h
  #)

  # --------------------------------------------------------------------------
  # Include dirs

  include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
    )

  # TODO: install headers 
  file(GLOB headers "${CMAKE_CURRENT_SOURCE_DIR}/*.h")
  install(FILES 
    ${headers} 
    "${CMAKE_CURRENT_BINARY_DIR}/vtkCUDAConfigure.h"
    DESTINATION include/${PROJECT_NAME} COMPONENT Development
    )

  # --------------------------------------------------------------------------
  # Sources
    SET(vtkCUDA_SRCS
      vtkCudaVolumeMapper.cxx
      vtkCudaVolumeInformationHandler.cxx
      vtkCudaRendererInformationHandler.cxx
      vtkCudaMemoryTexture.cxx
    )
    SET(vtkCUDA_ADDITIONAL_HEADERS
        vtkCUDA.h
        vtkCUDAWin32Header.h
        CUDA_renderBase.h
        CUDA_typeRange.h
        cudaRendererInformation.h
        cudaVolumeInformation.h
        )
    
    SET(vtkCUDA_CUDA_SRCS
        CUDA_renderBase.cu
        CUDA_renderRayCastComposite.h
        CUDA_renderRayCastCompositeShaded.h
        CUDA_interpolation.h
        CUDA_matrix_math.h
        CUDA_zbuffer_math.h
    )

  #-----------------------------------------------
  # Include dirs

  INCLUDE_DIRECTORIES(
    ${FOUND_CUT_INCLUDE}
    ${FOUND_CUDA_NVCC_INCLUDE}
    ${CUDA_INSTALL_PREFIX}/include
    ${CudaSupport_SOURCE_DIR}
    ${CudaSupport_BINARY_DIR}
    ${vtkCUDA_SOURCE_DIR}
    ${vtkCUDA_BINARY_DIR}
    ${VTK_INCLUDE_DIRS}
    )

   CUDA_INCLUDE_DIRECTORIES( ${CMAKE_CURRENT_SOURCE_DIR} 
                             ${FOUND_CUT_INCLUDE}
                             ${FOUND_CUDA_NVCC_INCLUDE}
                             ${VTK_INCLUDE_DIRS}
                             ${CudaSupport_SOURCE_DIR}

   )

  # --------------------------------------------------------------------------
  # Wrapping
  IF (VTK_WRAP_TCL)
    include("${VTK_CMAKE_DIR}/vtkWrapTcl.cmake")
    vtk_wrap_tcl3(vtkCUDA vtkCUDA_TCL_SRCS "${vtkCUDA_SRCS}" "")
  ENDIF (VTK_WRAP_TCL)

  # --------------------------------------------------------------------------
  # Build the library

  set(lib_name vtkCUDA)


  IF (VTK_WRAP_TCL)
    SET(VTK_LIBRARIES vtkCommonTCL vtkFilteringTCL vtkIOTCL vtkImagingTCL vtkGraphicsTCL)
  ELSE (VTK_WRAP_TCL)
    SET(VTK_LIBRARIES vtkCommon vtkFiltering vtkIO vtkImaging vtkGraphics )
  ENDIF (VTK_WRAP_TCL)


  ## Set the Libraries we link
  SET(vtkCUDA_LIBRARIES
    ${VTK_LIBRARIES}
    ${CUDA_TARGET_LINK}
    CudaSupport
   )

  CUDA_ADD_LIBRARY(${lib_name}
    ${vtkCUDA_SRCS}
    ${vtkCUDA_ADDITIONAL_HEADERS}
    ${vtkCUDA_TCL_SRCS}
    ${vtkCUDA_CUDA_SRCS}
    )

  TARGET_LINK_LIBRARIES(${lib_name}
    ${vtkCUDA_LIBRARIES}
    )

  IF(BUILD_SHARED_LIBS)
    INSTALL(TARGETS ${lib_name}
        RUNTIME DESTINATION bin COMPONENT RuntimeLibraries 
        LIBRARY DESTINATION lib COMPONENT RuntimeLibraries
        ARCHIVE DESTINATION lib COMPONENT Development)
  ENDIF(BUILD_SHARED_LIBS)

ENDIF(CUDA_SUPPORT_ENABLED)
