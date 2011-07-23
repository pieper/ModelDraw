/*=========================================================================

  Copyright Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   vtkITK
  Module:    $HeadURL: http://svn.slicer.org/Slicer3/trunk/Libs/vtkITK/vtkITK.h $
  Date:      $Date: 2010-01-22 14:45:00 -0500 (Fri, 22 Jan 2010) $
  Version:   $Revision: 11747 $

==========================================================================*/
//
/// use an ifdef on Slicer3_VTK5 to flag code that won't
/// compile on vtk4.4 and before
//
#if ( (VTK_MAJOR_VERSION >= 5) || ( VTK_MAJOR_VERSION == 4 && VTK_MINOR_VERSION >= 5 ) )
#define Slicer3_VTK5
#endif

#include "vtkITKWin32Header.h"
#include "vtkITKNumericTraits.h"
