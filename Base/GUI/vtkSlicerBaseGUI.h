/*=auto=========================================================================

  Portions (c) Copyright 2005 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $RCSfile: vtkSlicerBaseGUI.h,v $
  Date:      $Date: 2010-02-15 16:34:06 -0500 (Mon, 15 Feb 2010) $
  Version:   $Revision: 12141 $

=========================================================================auto=*/

/*
 * This is needed for loading slicer code as module.
 * Added here to isolate the changes to the main files in case
 * further mods are needed.
 * - sp 2002-04-19
 */


//
/// use an ifdef on Slicer3_VTK5 to flag code that won't
/// compile on vtk4.4 and before
//
#if ( (VTK_MAJOR_VERSION >= 5) || ( VTK_MAJOR_VERSION == 4 && VTK_MINOR_VERSION >= 5 ) )
#define Slicer3_VTK5
#endif

#include "vtkSlicerBaseGUIWin32Header.h"
