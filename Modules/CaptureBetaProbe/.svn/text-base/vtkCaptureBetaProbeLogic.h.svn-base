/*==========================================================================

  Portions (c) Copyright 2008 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $HeadURL: $
  Date:      $Date: $
  Version:   $Revision: $

==========================================================================*/

// .NAME vtkCaptureBetaProbeLogic - slicer logic class for Locator module 
// .SECTION Description
// This class manages the logic associated with tracking device for
// IGT. 


#ifndef __vtkCaptureBetaProbeLogic_h
#define __vtkCaptureBetaProbeLogic_h

#include "vtkCaptureBetaProbeWin32Header.h"

#include "vtkSlicerBaseLogic.h"
#include "vtkSlicerModuleLogic.h"
#include "vtkSlicerApplication.h"
#include "vtkCallbackCommand.h"

#include "vtkMRMLSliceNode.h"

class vtkIGTLConnector;
class vtkMatrix3x3;
class vtkMatrix4x4;
class vtkCollection;
class vtkVector3d;

class VTK_CaptureBetaProbe_EXPORT vtkCaptureBetaProbeLogic : public vtkSlicerModuleLogic 
{
 public:
  //BTX
  enum {  // Events
    //LocatorUpdateEvent      = 50000,
    StatusUpdateEvent       = 50001,
  };
  //ETX

 public:
  
  static vtkCaptureBetaProbeLogic *New();
  
  vtkTypeRevisionMacro(vtkCaptureBetaProbeLogic,vtkObject);
  void PrintSelf(ostream&, vtkIndent);

  void PivotCalibration(vtkCollection* PivotingMatrix, double AveragePcal[3]);

 protected:
  
  vtkCaptureBetaProbeLogic();
  ~vtkCaptureBetaProbeLogic();

  void operator=(const vtkCaptureBetaProbeLogic&);
  vtkCaptureBetaProbeLogic(const vtkCaptureBetaProbeLogic&);

  static void DataCallback(vtkObject*, unsigned long, void *, void *);
  void UpdateAll();

  vtkCallbackCommand *DataCallbackCommand;

 private:


};

#endif


  
