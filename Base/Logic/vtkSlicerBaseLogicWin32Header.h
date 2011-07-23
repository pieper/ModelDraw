/*=auto=========================================================================

  Portions (c) Copyright 2005 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $RCSfile: vtkSlicerBaseLogicWin32Header.h,v $
  Date:      $Date: 2010-02-15 16:35:35 -0500 (Mon, 15 Feb 2010) $
  Version:   $Revision: 12142 $

=========================================================================auto=*/
///  vtkSlicerBaseLogicWin32Header - manage Windows system differences
/// 
/// The vtkSlicerBaseLogicWin32Header captures some system differences between Unix
/// and Windows operating systems. 

#ifndef __vtkSlicerBaseLogicWin32Header_h
#define __vtkSlicerBaseLogicWin32Header_h

#include <vtkSlicerBaseLogicConfigure.h>

#if defined(WIN32) && !defined(VTKSLICER_STATIC)
#if defined(SlicerBaseLogic_EXPORTS)
#define VTK_SLICER_BASE_LOGIC_EXPORT __declspec( dllexport ) 
#else
#define VTK_SLICER_BASE_LOGIC_EXPORT __declspec( dllimport ) 
#endif
#else
#define VTK_SLICER_BASE_LOGIC_EXPORT
#endif

#endif
