/*==========================================================================

  Portions (c) Copyright 2008 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $HeadURL: http://svn.slicer.org/Slicer3/trunk/Modules/OpenIGTLinkIF/vtkIGTLToMRMLBase.h $
  Date:      $Date: 2009-01-05 13:28:20 -0500 (Mon, 05 Jan 2009) $
  Version:   $Revision: 8267 $

==========================================================================*/

/*
 * ZFrame Fiducial Registration Algorithm
 * The method and code is originally developoed by Simon DiMaio from BWH.
 */


#ifndef __vtkZFrameRobotToImageRegistration_h
#define __vtkZFrameRobotToImageRegistration_h

#include "vtkObject.h"
#include "vtkProstateNavWin32Header.h" 

#include "vtkRobotToImageRegistration.h"

#include <vector>
#include <string>

#include "ZLinAlg.h"
#include "newmatap.h"
#include "newmat.h"

class VTK_PROSTATENAV_EXPORT vtkZFrameRobotToImageRegistration : public vtkRobotToImageRegistration
{

 public:

  static vtkZFrameRobotToImageRegistration *New();
  vtkTypeRevisionMacro(vtkZFrameRobotToImageRegistration,vtkRobotToImageRegistration);

  void PrintSelf(ostream& os, vtkIndent indent);

  virtual int DoRegistration();

  vtkSetObjectMacro( ZFrameBaseOrientation, vtkMatrix4x4 );
  vtkGetObjectMacro( ZFrameBaseOrientation, vtkMatrix4x4 );

  void SetSliceRange(int param1, int param2);

 protected:
  vtkZFrameRobotToImageRegistration();
  ~vtkZFrameRobotToImageRegistration();

  // Methods related to finding the fiducial artifacts in the MR images.
  void Init(int xsize, int ysize);


  int  ZFrameRegistration(vtkMRMLScalarVolumeNode* volumeNode,
                          vtkMRMLLinearTransformNode* transformNode, int slindex_s, int slindex_e);

 protected:

  vtkMatrix4x4 * ZFrameBaseOrientation;

  short  *InputImage;
  //BTX
  Matrix SourceImage;
  //ETX

  int SliceRangeLow;
  int SliceRangeHigh;

};


#endif //__vtkZFrameRobotToImageRegistration_h
