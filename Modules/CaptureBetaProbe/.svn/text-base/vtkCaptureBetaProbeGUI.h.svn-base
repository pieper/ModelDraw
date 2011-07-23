/*==========================================================================

  Portions (c) Copyright 2008 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $HeadURL: $
  Date:      $Date: $
  Version:   $Revision: $

==========================================================================*/

#ifndef __vtkCaptureBetaProbeGUI_h
#define __vtkCaptureBetaProbeGUI_h

#ifdef WIN32
#include "vtkCaptureBetaProbeWin32Header.h"
#endif

#include <iostream>
#include <fstream>

#include "vtkSlicerModuleGUI.h"
#include "vtkCallbackCommand.h"
#include "vtkSlicerInteractorStyle.h"

#include "vtkCaptureBetaProbeLogic.h"

#include "vtkMRMLUDPServerNode.h"

class vtkKWPushButton;
class vtkSlicerNodeSelectorWidget;
class vtkKWLabel;
class vtkKWFileBrowserDialog;

//class vtkMRMLUDPServerNode;
class vtkMRMLLinearTransformNode;
class vtkMatrix4x4;
class vtkCollection;

class VTK_CaptureBetaProbe_EXPORT vtkCaptureBetaProbeGUI : public vtkSlicerModuleGUI
{
 public:

  vtkTypeRevisionMacro ( vtkCaptureBetaProbeGUI, vtkSlicerModuleGUI );

  //----------------------------------------------------------------
  // Set/Get Methods
  //----------------------------------------------------------------

  vtkGetObjectMacro ( Logic, vtkCaptureBetaProbeLogic );
  void SetModuleLogic ( vtkSlicerLogic *logic )
  { 
    this->SetLogic ( vtkObjectPointer (&this->Logic), logic );
  } 

  // NH added the following line on Jan 9 2011 in response to RK's request to move the module under IGT
  // LC should review the following line, and upon approval, should remove this comment.

  const char *GetCategory() const { return "IGT";}


  // NH addition end.

  void SetContinuousMode(bool val){this->continuous_mode = val;};
  bool GetContinuousMode(){return this->continuous_mode;};

  vtkGetMacro(PivotCalibrationRunning,bool);
  vtkSetMacro(PivotCalibrationRunning,bool);

  void Capture_Data();
  void Capture_Tracker_Position();

 protected:
  //----------------------------------------------------------------
  // Constructor / Destructor (proctected/private) 
  //----------------------------------------------------------------

  vtkCaptureBetaProbeGUI ( );
  virtual ~vtkCaptureBetaProbeGUI ( );

 private:
  vtkCaptureBetaProbeGUI ( const vtkCaptureBetaProbeGUI& ); // Not implemented.
  void operator = ( const vtkCaptureBetaProbeGUI& ); //Not implemented.

 public:
  //----------------------------------------------------------------
  // New method, Initialization etc.
  //----------------------------------------------------------------

  static vtkCaptureBetaProbeGUI* New ();
  void Init();
  virtual void Enter ( );
  virtual void Exit ( );
  void PrintSelf (ostream& os, vtkIndent indent );

  //----------------------------------------------------------------
  // Observer Management
  //----------------------------------------------------------------

  virtual void AddGUIObservers ( );
  virtual void RemoveGUIObservers ( );
  void AddLogicObservers ( );
  void RemoveLogicObservers ( );

  //----------------------------------------------------------------
  // Event Handlers
  //----------------------------------------------------------------

  virtual void ProcessLogicEvents ( vtkObject *caller, unsigned long event, void *callData );
  virtual void ProcessGUIEvents ( vtkObject *caller, unsigned long event, void *callData );
  virtual void ProcessMRMLEvents ( vtkObject *caller, unsigned long event, void *callData );
  void ProcessTimerEvents();
  void HandleMouseEvent(vtkSlicerInteractorStyle *style);
  static void DataCallback(vtkObject *caller, 
                           unsigned long eid, void *clientData, void *callData);
  
  //----------------------------------------------------------------
  // Build Frames
  //----------------------------------------------------------------

  virtual void BuildGUI ( );
  void BuildGUIForHelpFrame();
  void BuildGUIForPivotCalibration();
  void BuildGUIForCapturingDataFromBetaProbe();

  //----------------------------------------------------------------
  // Update routines
  //----------------------------------------------------------------

  void UpdateAll();


 protected:
  
  //----------------------------------------------------------------
  // File
  //----------------------------------------------------------------

  ofstream BetaProbeCountsWithTimestamp;

  //----------------------------------------------------------------
  // Timer
  //----------------------------------------------------------------
  
  int TimerFlag;
  int TimerInterval;

  //----------------------------------------------------------------
  // GUI widgets
  //----------------------------------------------------------------

  vtkKWPushButton* Capture;
  vtkKWPushButton* Start_Button;
  vtkKWPushButton* Stop_Button;
  vtkKWPushButton* SelectFile;
  vtkKWPushButton* CloseFile;
  vtkSlicerNodeSelectorWidget* CounterNode;
  vtkSlicerNodeSelectorWidget* TrackerNode;
  vtkKWLabel* Capture_status;
  vtkKWFileBrowserDialog* FileSelector;

  vtkMRMLUDPServerNode* Counts;
  vtkMRMLLinearTransformNode* Probe_Position;
  vtkMatrix4x4* Probe_Matrix;




  vtkKWPushButton* StartPivotCalibration;
  vtkKWPushButton* StopPivotCalibration;

  vtkCollection* PivotingMatrix;

  //----------------------------------------------------------------
  // Logic Values
  //----------------------------------------------------------------

  vtkCaptureBetaProbeLogic *Logic;
  vtkCallbackCommand *DataCallbackCommand;
  int                        CloseScene;

  bool continuous_mode;
  bool PivotCalibrationRunning;
};



#endif
