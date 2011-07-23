#ifndef __vtkCUDAWin32Header_h
        #define __vtkCUDAWin32Header_h

        #include <vtkCUDAConfigure.h>

        #if defined(WIN32) && !defined(VTKSLICER_STATIC)
                #if defined(vtkCUDA_EXPORTS)
                        #define VTK_CUDA_EXPORT __declspec( dllexport ) 
                #else
                        #define VTK_CUDA_EXPORT __declspec( dllimport ) 
                #endif
        #else
                #define VTK_CUDA_EXPORT 
        #endif
#endif
