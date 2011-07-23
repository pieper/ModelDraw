/*=auto=========================================================================

  Portions (c) Copyright 2005 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $RCSfile: vtkMRMLTransRectalProstateRobotNode.h,v $
  Date:      $Date: 2006/03/19 17:12:29 $
  Version:   $Revision: 1.3 $

=========================================================================auto=*/
#ifndef __vtkMRMLTransRectalRobotNode_h
#define __vtkMRMLTransRectalRobotNode_h

#include "vtkOpenIGTLinkIFWin32Header.h"
#include "vtkMRML.h"
#include "vtkMRMLRobotNode.h"
#include "vtkMRMLStorageNode.h"
#include "vtkSmartPointer.h"
#include "vtkLineSource.h"
#include "vtkTransRectalFiducialCalibrationAlgo.h"
#include "vtkObject.h"
#include "vtkProstateNavWin32Header.h" 

class vtkPolyData;
class vtkMRMLFiducialListNode;
class vtkMRMLScalarVolumeNode;

class VTK_PROSTATENAV_EXPORT vtkMRMLTransRectalProstateRobotNode : public vtkMRMLRobotNode
{
 public:

  //----------------------------------------------------------------
  // Standard methods for MRML nodes
  //----------------------------------------------------------------

  static vtkMRMLTransRectalProstateRobotNode *New();
  vtkTypeMacro(vtkMRMLTransRectalProstateRobotNode,vtkMRMLRobotNode);
  
  // Description:
  // Initialize the robot
  virtual int Init(vtkSlicerApplication* app, const char* moduleShareDir);

  void PrintSelf(ostream& os, vtkIndent indent);

  virtual vtkMRMLTransRectalProstateRobotNode* CreateNodeInstance();

  // Description:
  // Set node attributes
  virtual void ReadXMLAttributes( const char** atts);

  // Description:
  // Write this node's information to a MRML file in XML format.
  virtual void WriteXML(ostream& of, int indent);

  // Description:
  // Copy the node's attributes to this object
  virtual void Copy(vtkMRMLNode *node);

  virtual void UpdateReferenceID(const char *oldID, const char *newID);
  virtual void UpdateReferences();
  virtual void UpdateScene(vtkMRMLScene *scene);

  virtual void RemoveChildNodes();

  // Description:
  // Get node XML tag name (like Volume, Model)
  virtual const char* GetNodeTagName()
    {return "TransRectalProstateRobot";};

  virtual const char* GetWorkflowStepsString()
    {return "FiducialCalibration PointTargetingWithoutOrientation PointVerification"; };

  // method to propagate events generated in mrml
  virtual void ProcessMRMLEvents ( vtkObject *caller, unsigned long event, void *callData );

  virtual bool GetNeedleDirectionAtTarget(vtkProstateNavTargetDescriptor *targetDesc, NeedleDescriptorStruct *needle, double* needleDirection);
  virtual bool ShowRobotAtTarget(vtkProstateNavTargetDescriptor *targetDesc, NeedleDescriptorStruct *needle);
  //BTX
  virtual std::string GetTargetInfoText(vtkProstateNavTargetDescriptor *targetDesc, NeedleDescriptorStruct *needle);
  //ETX

  // Description:
  void RemoveAllCalibrationMarkers();

  void ResetCalibrationData();
  const TRProstateBiopsyCalibrationData& GetCalibrationData() { return this->CalibrationData; }

  void SetAndObserveCalibrationPointListNodeID(const char *childNodeID);
  vtkGetStringMacro(CalibrationPointListNodeID);
  vtkMRMLFiducialListNode* GetCalibrationPointListNode();

  void SetAndObserveCalibrationVolumeNodeID(const char *childNodeID);  
  vtkMRMLScalarVolumeNode* GetCalibrationVolumeNode();

  //BTX
  void SetCalibrationInputs(const char *calibVolRef, double thresh[CALIB_MARKER_COUNT], double fidDimsMm[3], double radiusMm, double initialAngle, bool enableAutomaticCenterpointAdjustment);
  //ETX
 
 virtual const char* GetRobotModelId() {return GetRobotModelNodeRef(); };
 virtual bool GetRobotBaseTransform(vtkMatrix4x4* transform);

 virtual const char* GetWorkspaceObjectModelId() { return GetWorkspaceModelNodeRef(); };

 void SetModelAxesVisible(bool visible);
 vtkGetMacro(ModelAxesVisible, bool);

 vtkGetStringMacro(CalibrationVolumeNodeID);

 vtkGetMacro(EnableAutomaticMarkerCenterpointAdjustment, bool);

 vtkGetStringMacro(CalibrationStatusDescription);

 double GetMarkerSegmentationThreshold(int i);
 vtkGetVectorMacro(MarkerDimensionsMm, double, 3);
 vtkGetMacro(MarkerRadiusMm, double);
 vtkGetMacro(RobotInitialAngle, double);

 virtual void ApplyTransform(vtkMatrix4x4* transformMatrix); 

 protected:
  //----------------------------------------------------------------
  // Constructor and destroctor
  //----------------------------------------------------------------
  
  vtkMRMLTransRectalProstateRobotNode();
  virtual ~vtkMRMLTransRectalProstateRobotNode();
  vtkMRMLTransRectalProstateRobotNode(const vtkMRMLTransRectalProstateRobotNode&);
  void operator=(const vtkMRMLTransRectalProstateRobotNode&);  

  const char* AddModelNode(const char* nodeName, double colorR=0.7, double colorG=0.7, double colorB=0.7); 
    
  vtkGetStringMacro(RobotModelNodeRef);
  vtkSetReferenceStringMacro(RobotModelNodeRef); 
  vtkMRMLModelNode* GetRobotModelNode();    

  vtkGetStringMacro(WorkspaceModelNodeRef);
  vtkSetReferenceStringMacro(WorkspaceModelNodeRef); 
  vtkMRMLModelNode* GetWorkspaceModelNode();    


 protected:

  vtkSetStringMacro(CalibrationStatusDescription);  

  void UpdateRobotModel();
  void UpdateRobotModelAxes();
  void UpdateRobotModelProbe();
  void UpdateRobotModelMarker(int markerInd, vtkImageData *imagedata, vtkMatrix4x4* ijkToRAS);
  void UpdateRobotModelNeedle(vtkProstateNavTargetDescriptor *targetDesc, NeedleDescriptorStruct *needle);

  void UpdateWorkspaceModel();

  void UpdateCalibration();

  virtual void TransformCalibrationData(vtkMatrix4x4 *transformMatrix);
  virtual void TransformRobotModelMarkers(vtkMatrix4x4* transformMatrix);

  ///////////////////////////////
  // persistent data

  bool EnableAutomaticMarkerCenterpointAdjustment;
  double MarkerSegmentationThreshold[CALIB_MARKER_COUNT];
  double MarkerDimensionsMm[3];
  double MarkerRadiusMm;
  double RobotInitialAngle;

  bool ModelAxesVisible;
  char *RobotModelNodeRef;
  
  char *WorkspaceModelNodeRef;

  TRProstateBiopsyCalibrationData CalibrationData;
  char *CalibrationStatusDescription;

  // Description:
  // CalibrationPointListNode is used for displaying 4 fiducial points that defines the calibration marker initial positions
  vtkSetReferenceStringMacro(CalibrationPointListNodeID);
  char *CalibrationPointListNodeID;
  vtkMRMLFiducialListNode* CalibrationPointListNode;

  vtkSetReferenceStringMacro(CalibrationVolumeNodeID);
  char *CalibrationVolumeNodeID;
  vtkMRMLScalarVolumeNode* CalibrationVolumeNode;

  ///////////////////////////////
  // temporarily stored data (cached for performance reasons only)

  // Robot model components
  vtkPolyData* ModelMarkers[CALIB_MARKER_COUNT];
  vtkPolyData* ModelAxes;
  vtkPolyData* ModelProbe;
  vtkPolyData* ModelNeedle;  
  
};

#endif

