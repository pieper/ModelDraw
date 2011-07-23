/*==========================================================================

  Portions (c) Copyright 2008 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $HeadURL: $
  Date:      $Date: $
  Version:   $Revision: $

==========================================================================*/

#ifndef __vtkUDPServerGUI_h
#define __vtkUDPServerGUI_h

#ifdef WIN32
#include "vtkUDPServerWin32Header.h"
#endif

#include "vtkSlicerModuleGUI.h"
#include "vtkCallbackCommand.h"
#include "vtkSlicerInteractorStyle.h"

#include "vtkUDPServerLogic.h"
#include "vtkMRMLUDPServerNode.h"

#include <string>

class vtkKWPushButton;
class vtkKWEntry;
class vtkKWMultiColumnListWithScrollbars;
class vtkKWRadioButtonSet;

class VTK_UDPServer_EXPORT vtkUDPServerGUI : public vtkSlicerModuleGUI
{
 public:

  vtkTypeRevisionMacro ( vtkUDPServerGUI, vtkSlicerModuleGUI );

  //----------------------------------------------------------------
  // Set/Get Methods
  //----------------------------------------------------------------

  // Description: 
  // Get the categorization of the module.
  const char *GetCategory() const { return "IGT"; }

  vtkGetObjectMacro ( Logic, vtkUDPServerLogic );
  void SetModuleLogic ( vtkSlicerLogic *logic )
  { 
    this->SetLogic ( vtkObjectPointer (&this->Logic), logic );
  }

 protected:
  //----------------------------------------------------------------
  // Constructor / Destructor (proctected/private) 
  //----------------------------------------------------------------

  vtkUDPServerGUI ( );
  virtual ~vtkUDPServerGUI ( );

 private:
  vtkUDPServerGUI ( const vtkUDPServerGUI& ); // Not implemented.
  void operator = ( const vtkUDPServerGUI& ); //Not implemented.

 public:
  //----------------------------------------------------------------
  // New method, Initialization etc.
  //----------------------------------------------------------------

  static vtkUDPServerGUI* New ();
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
  void BuildGUIForServerFrame();
  void BuildGUIForDataFrame();

  //----------------------------------------------------------------
  // Update routines
  //----------------------------------------------------------------

  void UpdateAll();
  int UpdateDataFrame(char* data);

 protected:
  
  //----------------------------------------------------------------
  // Message
  //----------------------------------------------------------------
  //BTX
  static const int BUFFSIZE = 255;
  bool ServerConnected;
  char buffer[BUFFSIZE];
  bool received;
  //Structure to save data message
  struct probeData{
    std::string date;
    std::string time;
    int smoothedCount;
    int betaCount;
    int gammaCount;
  };
  probeData pd;
  //ETX
  
  //----------------------------------------------------------------
  // UDPServerNode
  //----------------------------------------------------------------
  vtkMRMLUDPServerNode* svrNode;
  
  //----------------------------------------------------------------
  // Timer
  //----------------------------------------------------------------
  
  int TimerFlag;
  int TimerInterval;

  //----------------------------------------------------------------
  // GUI widgets
  //----------------------------------------------------------------

  vtkKWPushButton* ConnectButton;
  vtkKWEntry*      PortEntry;
  vtkKWMultiColumnListWithScrollbars* DataTable;
  vtkKWEntry*      UpdateEntry;
  vtkKWPushButton* ClearListButton;
  vtkKWRadioButtonSet* ProbeTypeButtonSet;

  //----------------------------------------------------------------
  // Logic Values
  //----------------------------------------------------------------

  vtkUDPServerLogic *Logic;
  vtkCallbackCommand *DataCallbackCommand;
  int                        CloseScene;

};

#endif
