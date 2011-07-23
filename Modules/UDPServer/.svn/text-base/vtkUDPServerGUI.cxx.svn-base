/*==========================================================================

  Portions (c) Copyright 2008 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $HeadURL: $
  Date:      $Date: $
  Version:   $Revision: $

==========================================================================*/

#include "vtkObject.h"
#include "vtkObjectFactory.h"

#include "vtkUDPServerGUI.h"
#include "vtkSlicerApplication.h"
#include "vtkSlicerModuleCollapsibleFrame.h"
#include "vtkSlicerSliceControllerWidget.h"
#include "vtkSlicerSliceGUI.h"
#include "vtkSlicerSlicesGUI.h"

#include "vtkSlicerColor.h"
#include "vtkSlicerTheme.h"

#include "vtkKWTkUtilities.h"
#include "vtkKWWidget.h"
#include "vtkKWFrameWithLabel.h"
#include "vtkKWFrame.h"
#include "vtkKWLabel.h"
#include "vtkKWEvent.h"
#include "vtkKWEntry.h"
#include "vtkKWMultiColumnListWithScrollbars.h"
#include "vtkKWMultiColumnList.h"
#include "vtkKWPushButton.h"
#include "vtkCornerAnnotation.h"
#include "vtkKWRadioButtonSet.h"
#include "vtkKWRadioButton.h"

#include <vector>
#include <sstream>
#include <list>
#include <stdio.h>
#include <stdlib.h>

//---------------------------------------------------------------------------
vtkStandardNewMacro (vtkUDPServerGUI );
vtkCxxRevisionMacro ( vtkUDPServerGUI, "$Revision: 1.0 $");
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
vtkUDPServerGUI::vtkUDPServerGUI ( )
{

  //----------------------------------------------------------------
  // Logic values
  this->Logic = NULL;
  this->DataCallbackCommand = vtkCallbackCommand::New();
  this->DataCallbackCommand->SetClientData( reinterpret_cast<void *> (this) );
  this->DataCallbackCommand->SetCallback(vtkUDPServerGUI::DataCallback);
  
  this->svrNode = NULL; 

  //----------------------------------------------------------------
  // GUI widgets
  this->ConnectButton = NULL;
  this->PortEntry = NULL;
  this->DataTable = NULL;
  this->UpdateEntry = NULL;
  this->ClearListButton = NULL;
  this->ProbeTypeButtonSet = NULL;

  //----------------------------------------------------------------
  //Message Variables
  this->ServerConnected = 0;
  this->TimerFlag = 0;
  this->received = 0;
  this->TimerInterval = 1000;  // in ms
}

//---------------------------------------------------------------------------
vtkUDPServerGUI::~vtkUDPServerGUI ( )
{

  //---------------------------------------------------------------- 
  // Stop the server before leaving
  
  if (this->GetLogic()->GetServerStopFlag())
    {
      this->GetLogic()->Stop();
    }

  //----------------------------------------------------------------
  // Remove Callbacks

  if (this->DataCallbackCommand)
    {
    this->DataCallbackCommand->Delete();
    }

  //----------------------------------------------------------------
  // Remove Observers

  this->RemoveGUIObservers();

  //----------------------------------------------------------------
  // Remove GUI widgets

  if (this->ConnectButton)
    {
    this->ConnectButton->SetParent(NULL);
    this->ConnectButton->Delete();
    }

  if (this->PortEntry)
    {
    this->PortEntry->SetParent(NULL);
    this->PortEntry->Delete();
    }

  if (this->UpdateEntry)
    {
    this->UpdateEntry->SetParent(NULL);
    this->UpdateEntry->Delete();
    }

  if (this->DataTable)
    {
    this->DataTable->SetParent(NULL);
    this->DataTable->Delete();
    }
  
  if (this->ClearListButton)
    {
    this->ClearListButton->SetParent(NULL);
    this->ClearListButton->Delete();
    }

  if (this->ProbeTypeButtonSet)
    {
    this->ProbeTypeButtonSet->SetParent(NULL);
    this->ProbeTypeButtonSet->Delete();
    }

  if (this->svrNode)
    {
    this->svrNode->Delete();
    }

  //----------------------------------------------------------------
  // Unregister Logic class

  this->SetModuleLogic ( NULL );

}


//---------------------------------------------------------------------------
void vtkUDPServerGUI::Init()
{
  //Register new node type to the scene
  vtkMRMLScene* scene = this->GetMRMLScene();
  vtkMRMLUDPServerNode* sNode = vtkMRMLUDPServerNode::New();
  scene->RegisterNodeClass(sNode);
  sNode->Delete();
}


//---------------------------------------------------------------------------
void vtkUDPServerGUI::Enter()
{

}

//---------------------------------------------------------------------------
void vtkUDPServerGUI::Exit ( )
{
  // Fill in
}


//---------------------------------------------------------------------------
void vtkUDPServerGUI::PrintSelf ( ostream& os, vtkIndent indent )
{
  this->vtkObject::PrintSelf ( os, indent );

  os << indent << "UDPServerGUI: " << this->GetClassName ( ) << "\n";
  os << indent << "Logic: " << this->GetLogic ( ) << "\n";
}


//---------------------------------------------------------------------------
void vtkUDPServerGUI::RemoveGUIObservers ( )
{
  //vtkSlicerApplicationGUI *appGUI = this->GetApplicationGUI();

  if (this->ConnectButton)
    {
    this->ConnectButton
      ->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }

  if (this->PortEntry)
    {
    this->PortEntry
      ->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }

  if (this->UpdateEntry)
    {
    this->UpdateEntry
      ->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }
  /* 
  if (this->DataTable)
    {
    this->DataTable
      ->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }
  */

  if (this->ClearListButton)
    {
    this->ClearListButton
      ->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }

  if (this->ProbeTypeButtonSet)
    {
    this->ProbeTypeButtonSet
      ->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }

  this->RemoveLogicObservers();

}


//---------------------------------------------------------------------------
void vtkUDPServerGUI::AddGUIObservers ( )
{
  this->RemoveGUIObservers();

  //vtkSlicerApplicationGUI *appGUI = this->GetApplicationGUI();

  //----------------------------------------------------------------
  // MRML

  vtkIntArray* events = vtkIntArray::New();
  events->InsertNextValue(vtkMRMLScene::NodeAddedEvent);
  //events->InsertNextValue(vtkMRMLScene::NodeRemovedEvent);
  events->InsertNextValue(vtkMRMLScene::SceneCloseEvent);
  
  if (this->GetMRMLScene() != NULL)
    {
    this->SetAndObserveMRMLSceneEvents(this->GetMRMLScene(), events);
    }
  events->Delete();

  //----------------------------------------------------------------
  // GUI Observers

if(this->ConnectButton)
{
  this->ConnectButton
    ->AddObserver(vtkKWPushButton::InvokedEvent, (vtkCommand *)this->GUICallbackCommand);
}

if(this->PortEntry)
{
  this->PortEntry
     ->AddObserver(vtkKWEntry::EntryValueChangedEvent, (vtkCommand *)this->GUICallbackCommand);
}

if(this->UpdateEntry)
{
  this->UpdateEntry
     ->AddObserver(vtkKWEntry::EntryValueChangedEvent, (vtkCommand *)this->GUICallbackCommand);
}

if(this->ClearListButton)
{
  this->ClearListButton
    ->AddObserver(vtkKWPushButton::InvokedEvent, (vtkCommand *)this->GUICallbackCommand);
}

if(this->ProbeTypeButtonSet)
{
  this->ProbeTypeButtonSet
    ->AddObserver(vtkKWPushButton::InvokedEvent, (vtkCommand *)this->GUICallbackCommand);
}

  this->AddLogicObservers();
}


//---------------------------------------------------------------------------
void vtkUDPServerGUI::RemoveLogicObservers ( )
{
  if (this->GetLogic())
    {
    this->GetLogic()->RemoveObservers(vtkCommand::ModifiedEvent,
                                      (vtkCommand *)this->LogicCallbackCommand);
    }
}

//---------------------------------------------------------------------------
void vtkUDPServerGUI::AddLogicObservers ( )
{
  this->RemoveLogicObservers();  

  if (this->GetLogic())
    {
    this->GetLogic()->AddObserver(vtkUDPServerLogic::StatusUpdateEvent,
                                  (vtkCommand *)this->LogicCallbackCommand);
    }
}

//---------------------------------------------------------------------------
void vtkUDPServerGUI::HandleMouseEvent(vtkSlicerInteractorStyle *style)
{
}


//---------------------------------------------------------------------------
void vtkUDPServerGUI::ProcessGUIEvents(vtkObject *caller,
                                         unsigned long event, void *callData)
{
  const char *eventName = vtkCommand::GetStringFromEventId(event);

  if (strcmp(eventName, "LeftButtonPressEvent") == 0)
    {
    vtkSlicerInteractorStyle *style = vtkSlicerInteractorStyle::SafeDownCast(caller);
    HandleMouseEvent(style);
    return;
    }

  if (this->ConnectButton == vtkKWPushButton::SafeDownCast(caller) 
      && event == vtkKWPushButton::InvokedEvent)
    {
    std::cerr << "Connect/Disconnect Button has been pressed." << std::endl;
    //Register type of Probe
    if(this->ProbeTypeButtonSet->GetWidget(0)->GetSelectedState() == 1)
      {
      //Rejection Probe selected
      this->GetLogic()->ProbeType = 0;
      }
    else
      {
      //Single probe selected
      this->GetLogic()->ProbeType = 1;
      }

    //this->ServerConnected = !(this->ServerConnected);
    if ( this->GetLogic()->GetServerStopFlag() && this->PortEntry )//this->ServerConnected == 1 && this->PortEntry)
      {
      //Call server connect loop to connect
      this->GetLogic()->Start(this->PortEntry->GetValueAsInt());

        this->ConnectButton->SetText("Disconnect");
        //Add a node to the scene
        if (svrNode == NULL)
          {
          svrNode = vtkMRMLUDPServerNode::New();
          this->GetMRMLScene()->AddNode(svrNode);
          this->GetMRMLScene()->Modified();
          }
        //Start Timer to update collected values
        this->TimerFlag = 1;
        this->TimerInterval = this->UpdateEntry->GetValueAsInt();
        ProcessTimerEvents();
        
      }
    else
      {
      this->GetLogic()->Stop();
      if (this->GetLogic()->GetServerStopFlag())
        {
        this->ConnectButton->SetText("Connect");
     /*
        if(svrNode)
       {
       svrNode->Delete();
       }
     */
        this->TimerFlag = 0;
        }
      }
    }
  else if (this->PortEntry == vtkKWEntry::SafeDownCast(caller)
      && event == vtkKWEntry::EntryValueChangedEvent)
    {
    std::cerr << "Port has been modified." << std::endl;
    this->GetLogic()->SetPort(this->PortEntry->GetValueAsInt());
    }
  else if (this->UpdateEntry == vtkKWEntry::SafeDownCast(caller)
      && event == vtkKWEntry::EntryValueChangedEvent)
    {
    std::cerr << "Update Entry has been modified." << std::endl;
    this->TimerInterval = this->UpdateEntry->GetValueAsInt();
    } 
  else if (this->ClearListButton == vtkKWPushButton::SafeDownCast(caller) 
      && event == vtkKWPushButton::InvokedEvent && this->DataTable)
    {
    this->DataTable->GetWidget()->DeleteAllRows();
    }

}

void vtkUDPServerGUI::DataCallback(vtkObject *caller, 
                                     unsigned long eid, void *clientData, void *callData)
{
  vtkUDPServerGUI *self = reinterpret_cast<vtkUDPServerGUI *>(clientData);
  vtkDebugWithObjectMacro(self, "In vtkUDPServerGUI DataCallback");
  self->UpdateAll();
}


//---------------------------------------------------------------------------
void vtkUDPServerGUI::ProcessLogicEvents ( vtkObject *caller,
                                             unsigned long event, void *callData )
{

  if (this->GetLogic() == vtkUDPServerLogic::SafeDownCast(caller))
    {
    if (event == vtkUDPServerLogic::StatusUpdateEvent)
      {
      //this->UpdateDeviceStatus();
      }
    }
}


//---------------------------------------------------------------------------
void vtkUDPServerGUI::ProcessMRMLEvents ( vtkObject *caller,
                                            unsigned long event, void *callData )
{
 // -----------------------------------------
  // Adding a new node
  /*
  if (event == vtkMRMLScene::NodeAddedEvent)
    {
    vtkObject* obj = (vtkObject*)callData;
    vtkMRMLNode* node = vtkMRMLNode::SafeDownCast(obj);
  */  
    //Check to see if node is UDPServerNode
    /*if (node && strcmp(node->GetNodeTagName(), "UDPServerNode") == 0)
      {
      //Make new node active
      this->svrNode = vtkMRMLUDPServerNode::SafeDownCast(node);
      }*/
  // }
    
}

//---------------------------------------------------------------------------
void vtkUDPServerGUI::ProcessTimerEvents()
{
  if (this->TimerFlag)
    {
    // -----------------------------------------
    // Check incoming new data

      if((this->GetLogic()->GetServerStopFlag() == false) && (this->GetLogic()->GetBytesReceived() > 0))
     {
        char* data;
        data = this->GetLogic()->GetImportedData();
        // if (this->GetLogic()->GetBytesReceived())
        //  {
            //Place data in the Data Frame List
        this->UpdateDataFrame(data);
        //  }
     }
    // update timer
    vtkKWTkUtilities::CreateTimerHandler(vtkKWApplication::GetMainInterp(), 
                                         this->TimerInterval,
                                         this, "ProcessTimerEvents");        
    }
}

//---------------------------------------------------------------------------
void vtkUDPServerGUI::BuildGUI ( )
{

  // MODULE GUI FRAME 
  // create a page
  this->UIPanel->AddPage ( "UDPServer", "UDPServer", NULL );

  //Call Build GUI methods
  BuildGUIForHelpFrame();
  BuildGUIForServerFrame();
  BuildGUIForDataFrame();
}

//--------------------------------------------------------------------------
void vtkUDPServerGUI::BuildGUIForHelpFrame ()
{
  // Define your help text here.
  const char *help = 
    "See "
    "<a>http://www.slicer.org/slicerWiki/index.php/Modules:UDPServer</a> for details.";
  const char *about =
    "This work is supported by NCIGT, NA-MIC.";

  vtkKWWidget *page = this->UIPanel->GetPageWidget ( "UDPServer" );
  this->BuildHelpAndAboutFrame (page, help, about);
}


//---------------------------------------------------------------------------
void vtkUDPServerGUI::BuildGUIForServerFrame()
{

  vtkSlicerApplication *app = (vtkSlicerApplication *)this->GetApplication();
  vtkKWWidget *page = this->UIPanel->GetPageWidget ("UDPServer");
  
  vtkSlicerModuleCollapsibleFrame *conBrowsFrame = vtkSlicerModuleCollapsibleFrame::New();

  conBrowsFrame->SetParent(page);
  conBrowsFrame->Create();
  conBrowsFrame->SetLabelText("Server Connection");
  //conBrowsFrame->CollapseFrame();
  app->Script ("pack %s -side top -anchor nw -fill x -padx 2 -pady 2 -in %s",
               conBrowsFrame->GetWidgetName(), page->GetWidgetName());

  // -----------------------------------------
  // Server Frame

  vtkKWFrameWithLabel *frame = vtkKWFrameWithLabel::New();
  frame->SetParent(conBrowsFrame->GetFrame());
  frame->Create();
  frame->SetLabelText ("Server Configuration");
  this->Script ( "pack %s -side top -fill x -expand y -anchor w -padx 2 -pady 2",
                 frame->GetWidgetName() );

  // -----------------------------------------
  // Port Entry Textbox
  
  vtkKWFrame *portFrame = vtkKWFrame::New();
  portFrame->SetParent(frame->GetFrame());
  portFrame->Create();
  
  app->Script ("pack %s -fill both -expand true", 
               portFrame->GetWidgetName());

  vtkKWLabel *portLabel = vtkKWLabel::New();
  portLabel->SetParent(portFrame);
  portLabel->Create();
  portLabel->SetWidth(16);
  portLabel->SetText("Port: ");

  this->PortEntry = vtkKWEntry::New();
  this->PortEntry->SetParent(portFrame);
  this->PortEntry->Create();
  this->PortEntry->SetWidth(6);
  this->PortEntry->SetRestrictValueToInteger();
  this->PortEntry->SetValueAsInt(3000);

  app->Script("pack %s %s -side left -anchor w -fill x -padx 2 -pady 2",
              portLabel->GetWidgetName() , this->PortEntry->GetWidgetName());
              
  portLabel->Delete();
  portFrame->Delete();

  // -----------------------------------------
  // Update Rate Entry Textbox
  
  vtkKWFrame *updateFrame = vtkKWFrame::New();
  updateFrame->SetParent(frame->GetFrame());
  updateFrame->Create();

  app->Script ( "pack %s -fill both -expand true",
                updateFrame->GetWidgetName());

  vtkKWLabel* updateLabel = vtkKWLabel::New();
  updateLabel->SetParent(updateFrame);
  updateLabel->Create();
  updateLabel->SetWidth(16);
  updateLabel->SetText("Update Rate (ms): ");

  this->UpdateEntry = vtkKWEntry::New();
  this->UpdateEntry->SetParent(updateFrame);
  this->UpdateEntry->Create();
  this->UpdateEntry->SetWidth(6);
  this->UpdateEntry->SetRestrictValueToInteger();
  this->UpdateEntry->SetValueAsInt(1000);
  
  app->Script("pack %s %s -side left -anchor w -fill x -padx 2 -pady 2",
              updateLabel->GetWidgetName() , this->UpdateEntry->GetWidgetName());
  
  updateLabel->Delete();
  updateFrame->Delete();

  //-------------------------------------------
  // Probe Type Selection
  vtkKWFrame *probeFrame = vtkKWFrame::New();
  probeFrame->SetParent(frame->GetFrame());
  probeFrame->Create();
  app->Script ( "pack %s -fill both -expand true",
                probeFrame->GetWidgetName());

  vtkKWLabel *probeLabel = vtkKWLabel::New();
  probeLabel->SetParent(probeFrame);
  probeLabel->Create();
  probeLabel->SetWidth(15);
  probeLabel->SetText("Probe Type: ");

  this->ProbeTypeButtonSet = vtkKWRadioButtonSet::New();
  this->ProbeTypeButtonSet->SetParent(probeFrame);
  this->ProbeTypeButtonSet->Create();
  this->ProbeTypeButtonSet->PackHorizontallyOn();
  this->ProbeTypeButtonSet->SetMaximumNumberOfWidgetsInPackingDirection(2);
  this->ProbeTypeButtonSet->UniformColumnsOn();
  this->ProbeTypeButtonSet->UniformRowsOn();

  this->ProbeTypeButtonSet->AddWidget(0);
  this->ProbeTypeButtonSet->GetWidget(0)->SetText("Rejection Probe");
  this->ProbeTypeButtonSet->GetWidget(0)->SelectedStateOn();
  this->ProbeTypeButtonSet->AddWidget(1);
  this->ProbeTypeButtonSet->GetWidget(1)->SetText("Single Probe");

  app->Script("pack %s %s -side left -anchor w -fill x -padx 2 -pady 2",
              probeLabel->GetWidgetName() , this->ProbeTypeButtonSet->GetWidgetName());

  probeFrame->Delete();
  probeLabel->Delete();
  //--------------------------------------------
  // Connect Button
  vtkKWFrame *connectFrame = vtkKWFrame::New();
  connectFrame->SetParent(frame->GetFrame());
  connectFrame->Create();
  
  app->Script ( "pack %s -padx 2 -pady 2",
                connectFrame->GetWidgetName());
  
  this->ConnectButton = vtkKWPushButton::New( );
  this->ConnectButton->SetParent(connectFrame);
  this->ConnectButton->Create( );
  this->ConnectButton->SetText("Connect");
  this->ConnectButton->SetWidth(12);

  app->Script("pack %s -side left -anchor w -fill x -padx 2 -pady 2",
              this->ConnectButton->GetWidgetName());

  connectFrame->Delete();
  frame->Delete();
  conBrowsFrame->Delete();
}

//----------------------------------------------------------------------------
void vtkUDPServerGUI::BuildGUIForDataFrame()
{
  vtkSlicerApplication *app = (vtkSlicerApplication *)this->GetApplication();
  vtkKWWidget *page = this->UIPanel->GetPageWidget("UDPServer");
  
  vtkSlicerModuleCollapsibleFrame *dataBrowsFrame = vtkSlicerModuleCollapsibleFrame::New();

  dataBrowsFrame->SetParent(page);
  dataBrowsFrame->Create();
  dataBrowsFrame->SetLabelText("Data Frame");
  app->Script ("pack %s -side top -anchor nw -fill x -padx 2 -pady 2 -in %s",
               dataBrowsFrame->GetWidgetName(), page->GetWidgetName());

  // -----------------------------------------
  // Data Table Widget

  vtkKWFrame *listFrame = vtkKWFrame::New();
  listFrame->SetParent(dataBrowsFrame->GetFrame());
  listFrame->Create();
  
  app->Script ( "pack %s -padx 2 -pady 2",
                listFrame->GetWidgetName());

  this->DataTable = vtkKWMultiColumnListWithScrollbars::New();
  this->DataTable->SetParent(listFrame);
  this->DataTable->Create();
  this->DataTable->SetHeight(1);
  this->DataTable->GetWidget()->SetSelectionTypeToRow();
  this->DataTable->GetWidget()->SetSelectionModeToSingle();
  this->DataTable->GetWidget()->MovableRowsOff();
  this->DataTable->GetWidget()->MovableColumnsOff();
  this->DataTable->GetWidget()->RejectInput();

  const char* labels[] =
    { "Date", "Time", "Smoothed", "Beta", "Gamma"};
  const int widths[] = 
    { 8, 8, 11, 7, 7 };

  for (int col = 0; col < 5; col ++)
    {
    this->DataTable->GetWidget()->AddColumn(labels[col]);
    this->DataTable->GetWidget()->SetColumnWidth(col, widths[col]);
    this->DataTable->GetWidget()->SetColumnAlignmentToLeft(col);
    this->DataTable->GetWidget()->ColumnEditableOff(col);
    this->DataTable->GetWidget()->SetColumnEditWindowToSpinBox(col);
    }
  this->DataTable->GetWidget()->SetColumnEditWindowToCheckButton(0);
  
   app->Script ("pack %s -fill both -expand true",  
               this->DataTable->GetWidgetName());
  
  listFrame->Delete();
  
  //--------------------------------------------
  // Clear List Button
  
  vtkKWFrame *clearFrame = vtkKWFrame::New();
  clearFrame->SetParent(dataBrowsFrame->GetFrame());
  clearFrame->Create();
  
  app->Script ( "pack %s -padx 2 -pady 2",
                clearFrame->GetWidgetName());
  
  this->ClearListButton = vtkKWPushButton::New( );
  this->ClearListButton->SetParent(clearFrame);
  this->ClearListButton->Create( );
  this->ClearListButton->SetText("Clear List");
  this->ClearListButton->SetWidth(12);

  app->Script("pack %s -side left -anchor w -fill x -padx 2 -pady 2",
              this->ClearListButton->GetWidgetName());

  clearFrame->Delete();  
  dataBrowsFrame->Delete();
}
  
//----------------------------------------------------------------------------
void vtkUDPServerGUI::UpdateAll()
{
}

//----------------------------------------------------------------------------
int vtkUDPServerGUI::UpdateDataFrame(char* data)
{
  //Check if data is in tact
  if (data == NULL)
    {
    return 1;
    }

  //--------------------------------------------------------------------------
  // Decompose data into members of probeData structure
  //--------------------------------------------------------------------------
  
  //std::cerr << "About to decompose data" << std::endl;
  
  //Create stream object from data pointer
  std::string buffdata (data);

  if (buffdata.length() < 10)  //check length of string
    {
    return 1;
    }
  std::cerr << buffdata << std::endl;
  std::istringstream iss (data,std::istringstream::in);
  
  // create a list 'vars' which will hold the variables extracted from 'iss'
  std::list< std::string > vars;

  // while no error on 'iss'
  while(iss)
    {
    // 'aVar' will hold the next word read from 'in'
    std::string aVar;

    // get all text from 'iss' up to the next comma, or end of data into 'aVar'
    getline(iss, aVar, ',');

    // Copy of 'aWord' in to the 'words' list
    if (iss)
      {
      vars.push_back(aVar);
      }
    }
    
  // convert all entries in 'vars' into variables
  std::list< std::string >::iterator it;
  
  it = vars.begin();
  // Date variable
  pd.date = *(it);
  // Time variable
  pd.time = *(++it);

  // For Rejection probe type
  if (this->GetLogic()->ProbeType == 0)
    {
    // Smoothed counts
    pd.smoothedCount = atoi((*(++it)).c_str());
    // Beta counts
    pd.betaCount = atoi((*(++it)).c_str());
    //Gamma counts
    pd.gammaCount = atoi((*(++it)).c_str());
    }

  //Single probe type
  if (this->GetLogic()->ProbeType == 1)
    {
    // Smoothed counts
    pd.smoothedCount = atoi((*(++it)).c_str());
    // Beta counts
    pd.betaCount = 0;
    //Gamma counts
    pd.gammaCount = 0;
    }
  //----------------------------------------------------------------------------------------
  // Update Dataframe with data from pd structure
  //----------------------------------------------------------------------------------------
  
  //std::cerr << "About to upload data onto Data Frame" << std::endl;
  this->DataTable->GetWidget()->AddRow();
  int row = (this->DataTable->GetWidget()->GetNumberOfRows())-1;
  this->DataTable->GetWidget()->SelectRow(row);
  //Date column
  this->DataTable->GetWidget()->SetCellText(row, 0, pd.date.c_str());
  //Time column
  this->DataTable->GetWidget()->SetCellText(row, 1, pd.time.c_str());
  //Smoothed Counts column
  char sc[15];
  sprintf(sc, "%d", pd.smoothedCount);
  //itoa(pd.smoothedCount, sc, 10);
  this->DataTable->GetWidget()->SetCellText(row, 2, sc);
  //Beta counts column
  char bc[15];
  sprintf(bc, "%d", pd.betaCount);
  //itoa(pd.betaCount, bc, 10);
  this->DataTable->GetWidget()->SetCellText(row, 3, bc);
  //Gamma Counts Column
  char gc[15];
  sprintf(gc, "%d", pd.gammaCount);
  //itoa(pd.gammaCount, gc, 10);
  this->DataTable->GetWidget()->SetCellText(row, 4, gc);
  
  // Make sure the last row is selected and always shown
  this->DataTable->GetWidget()->SeeRow(this->DataTable->GetWidget()->GetNumberOfRows());  

  //Update Node with values
  this->svrNode->SetSmoothedCounts(pd.smoothedCount);
  this->svrNode->SetBetaCounts(pd.betaCount);
  this->svrNode->SetGammaCounts(pd.gammaCount);
  this->svrNode->SetDate(pd.date);
  this->svrNode->SetTime(pd.time);
  
  return 0;
}
