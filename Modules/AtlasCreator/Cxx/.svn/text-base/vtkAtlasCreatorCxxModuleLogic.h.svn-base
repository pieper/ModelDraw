/*=auto=========================================================================

 Portions (c) Copyright 2008 Brigham and Women's Hospital (BWH) All Rights Reserved.

 See Doc/copyright/copyright.txt
 or http://www.slicer.org/copyright/copyright.txt for details.

 Program:   3D Slicer
 Module:    $RCSfile: vtkExampleLoadableGuiLessModuleLogic.h,v $
 Date:      $Date: 2006/03/19 17:12:29 $
 Version:   $Revision: 1.3 $

 =========================================================================auto=*/
#ifndef __vtkAtlasCreatorCxxModuleLogic_h
#define __vtkAtlasCreatorCxxModuleLogic_h

#include "vtkMRMLVolumeNode.h"

#include "vtkSlicerModuleLogic.h"
#include "vtkSlicerVolumesLogic.h"
#include "vtkSlicerApplicationLogic.h"

#include "vtkAtlasCreatorCxxModule.h"

class VTK_ATLASCREATORCXXMODULE_EXPORT vtkAtlasCreatorCxxModuleLogic: public vtkSlicerModuleLogic
{
public:
  static vtkAtlasCreatorCxxModuleLogic *New();
  vtkTypeMacro(vtkAtlasCreatorCxxModuleLogic,vtkSlicerModuleLogic);
  void PrintSelf(ostream& os, vtkIndent indent);

  virtual void ProcessMrmlEvents(vtkObject *caller, unsigned long event, void *callData){};

  void RegisterNodes();

  // Guesses the background intensity value of a volume
  double GuessRegistrationBackgroundLevel(vtkMRMLVolumeNode* volumeNode);

  // Adds a file as a volumeNode to the mrmlScene
  vtkMRMLScalarVolumeNode
      * AddArchetypeScalarVolume(const char* filename, const char* volname, vtkSlicerApplicationLogic* appLogic, vtkMRMLScene* mrmlScene);

protected:
  vtkAtlasCreatorCxxModuleLogic();
  virtual ~vtkAtlasCreatorCxxModuleLogic();
  vtkAtlasCreatorCxxModuleLogic(const vtkAtlasCreatorCxxModuleLogic&);
  void operator=(const vtkAtlasCreatorCxxModuleLogic&);

  //BTX
  template<class T>
  static T GuessRegistrationBackgroundLevel(vtkImageData* imageData);
  //ETX

};

#endif

