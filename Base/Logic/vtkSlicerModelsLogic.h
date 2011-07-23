/*=auto=========================================================================

  Portions (c) Copyright 2005 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $RCSfile: vtkSlicerModelsLogic.h,v $
  Date:      $Date: 2010-04-24 10:47:21 -0400 (Sat, 24 Apr 2010) $
  Version:   $Revision: 12897 $

=========================================================================auto=*/

///  vtkSlicerModelsLogic - slicer logic class for volumes manipulation
/// 
/// This class manages the logic associated with reading, saving,
/// and changing propertied of the volumes


#ifndef __vtkSlicerModelsLogic_h
#define __vtkSlicerModelsLogic_h

#include <stdlib.h>

#include "vtkSlicerBaseLogic.h"
#include "vtkSlicerLogic.h"

#include "vtkMRML.h"
#include "vtkMRMLModelNode.h"


class VTK_SLICER_BASE_LOGIC_EXPORT vtkSlicerModelsLogic : public vtkSlicerLogic 
{
  public:
  
  /// The Usual vtk class functions
  static vtkSlicerModelsLogic *New();
  vtkTypeRevisionMacro(vtkSlicerModelsLogic,vtkObject);
  void PrintSelf(ostream& os, vtkIndent indent);

  /// 
  /// The currently active mrml volume node 
  vtkGetObjectMacro (ActiveModelNode, vtkMRMLModelNode);
  void SetActiveModelNode (vtkMRMLModelNode *ActiveModelNode);

  /// 
  /// Create new mrml model node and
  /// read it's polydata from a specified file
  vtkMRMLModelNode* AddModel (const char* filename);

  /// 
  /// Create model nodes and
  /// read their polydata from a specified directory
  int AddModels (const char* dirname, const char* suffix );

  /// 
  /// Write model's polydata  to a specified file
  int SaveModel (const char* filename, vtkMRMLModelNode *modelNode);

  /// 
  /// Read in a scalar overlay and add it to the model node
  int AddScalar(const char* filename, vtkMRMLModelNode *modelNode);

  ///
  /// Transfor models's polydata
  static void TransformModel(vtkMRMLTransformNode *tnode, 
                              vtkMRMLModelNode *modelNode, 
                              int transformNormals,
                              vtkMRMLModelNode *modelOut);

  /// 
  /// Update logic state when MRML scene chenges
  virtual void ProcessMRMLEvents ( vtkObject * /*caller*/, 
                                  unsigned long /*event*/, 
                                  void * /*callData*/ );    
protected:
  vtkSlicerModelsLogic();
  ~vtkSlicerModelsLogic();
  vtkSlicerModelsLogic(const vtkSlicerModelsLogic&);
  void operator=(const vtkSlicerModelsLogic&);

  /// 
  //
  vtkMRMLModelNode *ActiveModelNode;
};

#endif

