
#ifndef __vtkPharmacokineticsWin32Header_h
#define __vtkPharmacokineticsWin32Header_h

#include <vtkPharmacokineticsConfigure.h>

#if defined(WIN32) && !defined(VTKSLICER_STATIC)
#if defined(Pharmacokinetics_EXPORTS)
#define VTK_Pharmacokinetics_EXPORT __declspec( dllexport ) 
#else
#define VTK_Pharmacokinetics_EXPORT __declspec( dllimport ) 
#endif
#else
#define VTK_Pharmacokinetics_EXPORT 
#endif
#endif
