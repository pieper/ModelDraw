#ifndef SLICER3HELPER_H
#define SLICER3HELPER_H

#include "vtkSlicerApplicationLogic.h"
#include "vtkSlicerApplication.h"
#include "vtkDataIOManagerLogic.h"
#include "vtkHTTPHandler.h"
#include "vtkSRBHandler.h"
#include "vtkXNATHandler.h"
#include "vtkHIDHandler.h"
#include "vtkXNDHandler.h"
#include "vtkSlicerXNATPermissionPrompterWidget.h"

class Slicer3Helper {
 public:
  static void AddDataIOToScene(vtkMRMLScene* mrmlScene, vtkSlicerApplication *app, vtkSlicerApplicationLogic *appLogic,  vtkDataIOManagerLogic *dataIOManagerLogic);
  static void RemoveDataIOFromScene(vtkMRMLScene* mrmlScene, vtkDataIOManagerLogic *dataIOManagerLogic);
  static const char* GetSvnRevision();
  static const char* GetTemporaryDirectory();
};

#endif
