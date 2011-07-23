/*==========================================================================

  Portions (c) Copyright 2008 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $HeadURL: $
  Date:      $Date: $
  Version:   $Revision: $

==========================================================================*/

#include "vtkProstateNavStepVerification.h"

#include "vtkObject.h"

#include "vtkProstateNavGUI.h"
#include "vtkProstateNavLogic.h"

#include "vtkKWMatrixWidget.h"
#include "vtkKWMatrixWidgetWithLabel.h"

#include "vtkSlicerFiducialsGUI.h"
#include "vtkSlicerFiducialsLogic.h"
#include "vtkMRMLSelectionNode.h"
#include "vtkSlicerNodeSelectorWidget.h"

#include "vtkMRMLLinearTransformNode.h"
#include "igtlMath.h"

#include "vtkMRMLRobotNode.h"

#include "ProstateNavMath.h"

////

#include "vtkProstateNavTargetDescriptor.h"
#include "vtkSlicerApplication.h"
#include "vtkSlicerApplicationLogic.h"
#include "vtkMRMLFiducialListNode.h"
#include "vtkSlicerSliceLogic.h"
#include "vtkMRMLSliceNode.h"
#include "vtkSlicerVolumesGUI.h"
#include "vtkMRMLInteractionNode.h"

#include "vtkKWFrame.h"
#include "vtkKWWizardWidget.h"
#include "vtkKWWizardWorkflow.h"
#include "vtkKWFrame.h"
#include "vtkKWEntry.h"
#include "vtkKWLabel.h"
#include "vtkKWEntryWithLabel.h"
#include "vtkKWEntrySet.h"
#include "vtkKWMessageDialog.h"
#include "vtkKWText.h"
#include "vtkKWPushButton.h"
#include "vtkKWMenuButton.h"
#include "vtkKWMenuButtonWithLabel.h"
#include "vtkKWMultiColumnList.h"
#include "vtkKWMultiColumnListWithScrollbars.h"
#include "vtkKWCheckButton.h"

const char TARGET_INDEX_ATTR[]="TARGET_IND";

#define DELETE_IF_NULL_WITH_SETPARENT_NULL(obj) \
  if (obj) \
    { \
    obj->SetParent(NULL); \
    obj->Delete(); \
    obj = NULL; \
    };

// Definition of target list columns
enum
{
  COL_NAME = 0,
  COL_X,
  COL_Y,
  COL_Z,
  COL_NEEDLE,
  COL_OVERALL_ERROR,
  COL_LR_ERROR,
  COL_AP_ERROR,
  COL_IS_ERROR,
  COL_COUNT // all valid columns should be inserted above this line
};
static const char* COL_LABELS[COL_COUNT] = { "Name", "R", "A", "S", "Needle", "Overall Error", "RL Error", "AP Error", "SI Error" };
static const int COL_WIDTHS[COL_COUNT] =   { 8,      6,   6,   6,   6,        6,               6,          6,          6 };

static const char NEEDLE_TIP_LABEL[]="V-Needle tip";
static const char NEEDLE_BASE_LABEL[]="V-Needle base";

//----------------------------------------------------------------------------
vtkStandardNewMacro(vtkProstateNavStepVerification);
vtkCxxRevisionMacro(vtkProstateNavStepVerification, "$Revision: 1.1 $");

//----------------------------------------------------------------------------
vtkProstateNavStepVerification::vtkProstateNavStepVerification()
{
  this->SetTitle("Verification");
  this->SetDescription("Measure needle distance from target.");

  this->MainFrame=NULL;
  
  // TargetPlanning frame
  this->VolumeSelectionFrame=NULL;
  this->LoadVerificationVolumeButton=NULL;
  this->VolumeSelectorWidget=NULL;
  this->ShowWorkspaceButton=NULL;
  this->ShowRobotButton=NULL;

  // TargetList frame
  this->TargetListFrame=NULL;
  this->TargetList=NULL;
  this->VerifyButton=NULL;
  this->ClearButton=NULL;

  // TargetControl frame
  this->VerificationControlFrame=NULL;
  this->Message=NULL;

  this->TitleBackgroundColor[0] = 0.8;
  this->TitleBackgroundColor[1] = 0.8;
  this->TitleBackgroundColor[2] = 0.8;

  this->ProcessingCallback = false;

  this->VerificationPointListNode=NULL;

  this->TargetIndexUnderVerification=-1;
}

//----------------------------------------------------------------------------
vtkProstateNavStepVerification::~vtkProstateNavStepVerification()
{
  RemoveGUIObservers();

  DELETE_IF_NULL_WITH_SETPARENT_NULL(MainFrame);
  
  // TargetPlanning
  DELETE_IF_NULL_WITH_SETPARENT_NULL(VolumeSelectionFrame);
  DELETE_IF_NULL_WITH_SETPARENT_NULL(LoadVerificationVolumeButton);  
  DELETE_IF_NULL_WITH_SETPARENT_NULL(ShowWorkspaceButton);
  DELETE_IF_NULL_WITH_SETPARENT_NULL(ShowRobotButton);
  DELETE_IF_NULL_WITH_SETPARENT_NULL(VolumeSelectorWidget);

  // TargetList frame
  DELETE_IF_NULL_WITH_SETPARENT_NULL(TargetListFrame);
  DELETE_IF_NULL_WITH_SETPARENT_NULL(TargetList);
  DELETE_IF_NULL_WITH_SETPARENT_NULL(VerifyButton);
  DELETE_IF_NULL_WITH_SETPARENT_NULL(ClearButton);

  // TargetControl frame
  DELETE_IF_NULL_WITH_SETPARENT_NULL(VerificationControlFrame);

  DELETE_IF_NULL_WITH_SETPARENT_NULL(Message);

  if (this->VerificationPointListNode!=NULL)
  {
    this->VerificationPointListNode->Delete();
    this->VerificationPointListNode=NULL;
  }

  this->MonitorFiducialNodes=true;
}

//----------------------------------------------------------------------------
void vtkProstateNavStepVerification::SetVerificationPointListNode(vtkMRMLFiducialListNode *node)
{
  vtkMRMLScene* scene=NULL;
  if (this->GetLogic())
  {
    if (this->GetLogic()->GetApplicationLogic())
    {
      if (this->GetLogic()->GetApplicationLogic()->GetMRMLScene())
      {
        scene=this->GetLogic()->GetApplicationLogic()->GetMRMLScene();
      }
    }
  }

  if (this->VerificationPointListNode!=NULL && scene!=NULL)
  {
    scene->RemoveNode(this->VerificationPointListNode);
  }

  vtkSmartPointer<vtkIntArray> events = vtkSmartPointer<vtkIntArray>::New();
  events->InsertNextValue(vtkCommand::ModifiedEvent);
  events->InsertNextValue(vtkMRMLScene::NodeAddedEvent);
  events->InsertNextValue(vtkMRMLFiducialListNode::DisplayModifiedEvent);
  events->InsertNextValue(vtkMRMLFiducialListNode::FiducialModifiedEvent);
  if (this->MRMLObserverManager)
  {
    this->MRMLObserverManager->SetAndObserveObjectEvents(vtkObjectPointer(&this->VerificationPointListNode),node,events);
  }
  else
  {
    vtkSetMRMLNodeMacro(this->VerificationPointListNode, node);
  }
  if (this->VerificationPointListNode!=NULL && scene!=NULL)
  {
    scene->AddNode(this->VerificationPointListNode);
  }  
}

//----------------------------------------------------------------------------
void vtkProstateNavStepVerification::ShowUserInterface()
{
  this->Superclass::ShowUserInterface();

  this->ShowVolumeSelectionFrame();
  this->ShowTargetListFrame();
  this->ShowVerificationControlFrame();

  if (GetLogic()!=NULL)
  {
    GetLogic()->SetMouseInteractionMode(vtkMRMLInteractionNode::ViewTransform);
  }

  vtkSmartPointer<vtkMRMLFiducialListNode> verifNode=vtkSmartPointer<vtkMRMLFiducialListNode>::New();
  verifNode->SetName("Verification");

  // This is just a temporary node for showing points in the GUI
  verifNode->SetSaveWithScene(false);
  verifNode->SetHideFromEditors(true);
  
  SetVerificationPointListNode(verifNode);

  this->AddGUIObservers();
  this->AddMRMLObservers();
  
  UpdateGUI();
}

//----------------------------------------------------------------------------
void vtkProstateNavStepVerification::ShowVolumeSelectionFrame()
{
  vtkKWWidget *parent = this->GetGUI()->GetWizardWidget()->GetClientArea();

  if (!this->VolumeSelectionFrame)
    {
    this->VolumeSelectionFrame = vtkKWFrame::New();
    }
  if (!this->VolumeSelectionFrame->IsCreated())
    {
    this->VolumeSelectionFrame->SetParent(parent);
    this->VolumeSelectionFrame->Create();
    }

  this->Script("pack %s -side top -anchor nw -padx 0 -pady 2",
    this->VolumeSelectionFrame->GetWidgetName());

  if (!this->LoadVerificationVolumeButton)
    {
    this->LoadVerificationVolumeButton = vtkKWPushButton::New();
    }
  if (!this->LoadVerificationVolumeButton->IsCreated())
    {
    this->LoadVerificationVolumeButton->SetParent(this->VolumeSelectionFrame);
    this->LoadVerificationVolumeButton->Create();
    this->LoadVerificationVolumeButton->SetBorderWidth(2);
    this->LoadVerificationVolumeButton->SetReliefToRaised();       
    this->LoadVerificationVolumeButton->SetHighlightThickness(2);
    this->LoadVerificationVolumeButton->SetBackgroundColor(0.85,0.85,0.85);
    this->LoadVerificationVolumeButton->SetActiveBackgroundColor(1,1,1);        
    this->LoadVerificationVolumeButton->SetText( "Load volume");
    this->LoadVerificationVolumeButton->SetBalloonHelpString("Click to load a volume. Need to additionally select the volume to make it the current verification volume.");
    }

  if (!this->ShowRobotButton)
    {
    this->ShowRobotButton = vtkKWCheckButton::New();
    } 
  if (!this->ShowRobotButton->IsCreated()) 
    {
    this->ShowRobotButton->SetParent(this->VolumeSelectionFrame);
    this->ShowRobotButton->Create();
    this->ShowRobotButton->SelectedStateOff();
    this->ShowRobotButton->SetText("Show Robot");
    this->ShowRobotButton->SetBalloonHelpString("Show the robot");
    }

  if (!this->ShowWorkspaceButton)
    {
    this->ShowWorkspaceButton = vtkKWCheckButton::New();
    } 
  if (!this->ShowWorkspaceButton->IsCreated())
    {
    this->ShowWorkspaceButton->SetParent(this->VolumeSelectionFrame);
    this->ShowWorkspaceButton->Create();
    this->ShowWorkspaceButton->SelectedStateOff();
    this->ShowWorkspaceButton->SetText("Show Workspace");
    this->ShowWorkspaceButton->SetBalloonHelpString("Show workspace of the robot");
    } 

  if (!this->VolumeSelectorWidget)
    {
    this->VolumeSelectorWidget = vtkSlicerNodeSelectorWidget::New();
    }
  if (!this->VolumeSelectorWidget->IsCreated())
    {
    this->VolumeSelectorWidget->SetParent(this->VolumeSelectionFrame);
    this->VolumeSelectorWidget->Create();
    this->VolumeSelectorWidget->SetBorderWidth(2);  
    this->VolumeSelectorWidget->SetNodeClass("vtkMRMLVolumeNode", NULL, NULL, NULL);
    this->VolumeSelectorWidget->SetMRMLScene(this->GetLogic()->GetApplicationLogic()->GetMRMLScene());
    this->VolumeSelectorWidget->SetNoneEnabled(true);
    this->VolumeSelectorWidget->GetWidget()->GetWidget()->IndicatorVisibilityOff();
    this->VolumeSelectorWidget->GetWidget()->GetWidget()->SetWidth(24);
    this->VolumeSelectorWidget->SetLabelText( "Verification Volume: ");
    this->VolumeSelectorWidget->SetBalloonHelpString("Select the targeting volume from the current scene.");
    }

  this->Script("grid %s -row 0 -column 0 -padx 2 -pady 2 -sticky ew", this->LoadVerificationVolumeButton->GetWidgetName());
  this->Script("grid %s -row 0 -column 1 -padx 2 -pady 2 -sticky e", this->ShowRobotButton->GetWidgetName());
  this->Script("grid %s -row 0 -column 2 -padx 2 -pady 2 -sticky e", this->ShowWorkspaceButton->GetWidgetName());

  this->Script("grid %s -row 1 -column 0 -columnspan 3 -padx 2 -pady 2 -sticky ew", this->VolumeSelectorWidget->GetWidgetName());

}


//----------------------------------------------------------------------------
void vtkProstateNavStepVerification::ShowTargetListFrame()
{
  vtkKWWidget *parent = this->GetGUI()->GetWizardWidget()->GetClientArea();

  if (!this->TargetListFrame)
    {
    this->TargetListFrame = vtkKWFrame::New();
    }
  if (!this->TargetListFrame->IsCreated())
    {
    this->TargetListFrame->SetParent(parent);
    this->TargetListFrame->Create();
    }
  this->Script("pack %s -side top -anchor nw -expand n -padx 2 -pady 2",
               this->TargetListFrame->GetWidgetName());

  if (!this->TargetList)
    {
    this->TargetList = vtkKWMultiColumnListWithScrollbars::New();
    this->TargetList->SetParent(this->TargetListFrame);
    this->TargetList->Create();
    this->TargetList->SetHeight(1);
    this->TargetList->GetWidget()->SetSelectionTypeToRow();
    this->TargetList->GetWidget()->SetSelectionBackgroundColor(1,0,0);
    this->TargetList->GetWidget()->MovableRowsOff();
    this->TargetList->GetWidget()->MovableColumnsOff();

    for (int col = 0; col < COL_COUNT; col ++)
      {
      this->TargetList->GetWidget()->AddColumn(COL_LABELS[col]);
      this->TargetList->GetWidget()->SetColumnWidth(col, COL_WIDTHS[col]);
      this->TargetList->GetWidget()->SetColumnAlignmentToLeft(col);
      this->TargetList->GetWidget()->ColumnEditableOff(col);
      }

    this->TargetList->GetWidget()->SetColumnEditWindowToCheckButton(0);

    }
  this->Script( "pack %s -side top -anchor nw -expand n -padx 2 -pady 2",
                this->TargetList->GetWidgetName());

  if (!this->VerifyButton)
    {
    this->VerifyButton = vtkKWPushButton::New();
    this->VerifyButton->SetParent (this->TargetListFrame);
    this->VerifyButton->Create();
    this->VerifyButton->SetText("Verify target");
    this->VerifyButton->SetBalloonHelpString("Move the robot to the position");
    }

  if (!this->ClearButton)
    {
    this->ClearButton = vtkKWPushButton::New();
    this->ClearButton->SetParent (this->TargetListFrame);
    this->ClearButton->Create();
    this->ClearButton->SetText("Clear target verification");
    this->ClearButton->SetBalloonHelpString("Clear verification data for the selected target");
    }

  this->Script("pack %s %s -side left -anchor nw -expand n -padx 2 -pady 2",
               this->VerifyButton->GetWidgetName(),
               this->ClearButton->GetWidgetName());


}


//----------------------------------------------------------------------------
void vtkProstateNavStepVerification::ShowVerificationControlFrame()
{
  vtkKWWidget *parent = this->GetGUI()->GetWizardWidget()->GetClientArea();

  if (!this->VerificationControlFrame)
    {
    this->VerificationControlFrame = vtkKWFrame::New();
    }
  if (!this->VerificationControlFrame->IsCreated())
    {
    this->VerificationControlFrame->SetParent(parent);
    this->VerificationControlFrame->Create();
    }
  this->Script("pack %s -side top -anchor nw -expand y -fill x -padx 0 -pady 2",
               this->VerificationControlFrame->GetWidgetName());

  if(!this->Message)
    {
    this->Message = vtkKWText::New();
    }
  if(!this->Message->IsCreated())
    {
    this->Message->SetParent(this->VerificationControlFrame);
    this->Message->Create();
    this->Message->SetText("Select needle type, then click on image to add a target");      
    this->Message->SetBackgroundColor(0.7, 0.7, 0.95);
    this->Message->SetHeight(6);
    this->Message->SetWrapToWord();
    this->Message->ReadOnlyOn();
    this->Message->SetBorderWidth(2);
    this->Message->SetReliefToGroove();
    this->Message->SetFont("times 12 bold");
    //this->Message->SetForegroundColor(0.0, 1.0, 0.0);
    }
  this->Script("pack %s -side top -anchor nw -expand y -fill x -padx 2 -pady 6", 
                this->Message->GetWidgetName());

}


//----------------------------------------------------------------------------
void vtkProstateNavStepVerification::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}


//----------------------------------------------------------------------------
void vtkProstateNavStepVerification::ProcessGUIEvents(vtkObject *caller,
                                          unsigned long event, void *callData)
{

  // -----------------------------------------------------------------
  // Verify Button Pressed

  if (this->VerifyButton == vtkKWPushButton::SafeDownCast(caller)
    && event == vtkKWPushButton::InvokedEvent)
  {
    StartVerification();
  }

  // -----------------------------------------------------------------
  // Clear Button Pressed

  else if (this->ClearButton == vtkKWPushButton::SafeDownCast(caller)
    && event == vtkKWPushButton::InvokedEvent)
  {    
    this->VerificationPointListNode->RemoveAllFiducials();
    vtkProstateNavTargetDescriptor* targetDesc=NULL;
    if (this->TargetIndexUnderVerification>=0)
    {
      targetDesc=this->GetProstateNavManager()->GetTargetDescriptorAtIndex(this->TargetIndexUnderVerification);      
    }
    else
    {
      targetDesc=this->GetProstateNavManager()->GetTargetDescriptorAtIndex(this->GetProstateNavManager()->GetCurrentTargetIndex());
    }
    if (targetDesc!=NULL)
    {
      targetDesc->SetTargetValidated(false);
      targetDesc->SetErrorDistanceFromNeedleLine(0);
      targetDesc->SetErrorVector(0,0,0);
      UpdateTargetListGUI();
    }
    StopVerification();
  }

  // -----------------------------------------------------------------
  // Load volume button Pressed

  if (this->LoadVerificationVolumeButton && this->LoadVerificationVolumeButton == vtkKWPushButton::SafeDownCast(caller) && (event == vtkKWPushButton::InvokedEvent))
    {
    this->GetApplication()->Script("::LoadVolume::ShowDialog");
    }

  // show workspace button
  if (this->ShowWorkspaceButton && this->ShowWorkspaceButton == vtkKWCheckButton::SafeDownCast(caller) && (event == vtkKWCheckButton::SelectedStateChangedEvent))
    {
      this->ShowWorkspaceModel(this->ShowWorkspaceButton->GetSelectedState() == 1);
    }

  // show robot button
  if (this->ShowRobotButton && this->ShowRobotButton == vtkKWCheckButton::SafeDownCast(caller) && (event == vtkKWCheckButton::SelectedStateChangedEvent))
    {
      this->ShowRobotModel(this->ShowRobotButton->GetSelectedState() == 1);
    }

  // -----------------------------------------------------------------
  // Volume selector
  
  if (this->VolumeSelectorWidget == vtkSlicerNodeSelectorWidget::SafeDownCast(caller) &&
    event == vtkSlicerNodeSelectorWidget::NodeSelectedEvent ) 
  {
    vtkMRMLScalarVolumeNode *volume = vtkMRMLScalarVolumeNode::SafeDownCast(this->VolumeSelectorWidget->GetSelected());
    if (volume != NULL)
    {
      this->GetGUI()->GetLogic()->SelectVolumeInScene(volume, VOL_VERIFICATION);
    }
  }
}


//----------------------------------------------------------------------------
void vtkProstateNavStepVerification::ProcessMRMLEvents(vtkObject *caller,
                                         unsigned long event, void *callData)
{  
  if (MonitorFiducialNodes && caller == this->VerificationPointListNode)
  {
    switch (event)
    {
    case vtkMRMLScene::NodeAddedEvent: // when a fiducial is added to the list      
      UpdateVerificationResultsForCurrentTarget();
      if (this->VerificationPointListNode->GetNumberOfFiducials()>=2)
      {        
        StopVerification();
        UpdateTargetListGUI();
      }      
      break;
    case vtkCommand::ModifiedEvent: // Modified Event    
    case vtkMRMLFiducialListNode::FiducialModifiedEvent:
    case vtkMRMLFiducialListNode::DisplayModifiedEvent:
      UpdateVerificationResultsForCurrentTarget();
      UpdateTargetListGUI();      
      break;
    }
  }

  if ( vtkMRMLScene::SafeDownCast(caller) == this->MRMLScene 
    && event == vtkMRMLScene::NodeAddedEvent )
    {
    vtkMRMLScalarVolumeNode *volumeNode = vtkMRMLScalarVolumeNode::SafeDownCast((vtkMRMLNode*)(callData));
    if (volumeNode!=NULL && this->VolumeSelectorWidget!=NULL && volumeNode!=this->VolumeSelectorWidget->GetSelected() )
      {
      // a new volume is loaded, set as the current verification volume
      this->VolumeSelectorWidget->SetSelected(volumeNode);
      }
    }

  vtkMRMLProstateNavManagerNode *managerNode = vtkMRMLProstateNavManagerNode::SafeDownCast(caller);
  if (managerNode!=NULL && managerNode==GetProstateNavManager())
    {
    switch (event)
      {
      case vtkMRMLProstateNavManagerNode::CurrentTargetChangedEvent:
        break;
      }
    }

}

//----------------------------------------------------------------------------
void vtkProstateNavStepVerification::AddMRMLObservers()
{
  if (this->VerificationPointListNode!=NULL)
  {
    vtkSmartPointer<vtkIntArray> events = vtkSmartPointer<vtkIntArray>::New();
    events->InsertNextValue(vtkCommand::ModifiedEvent);
    events->InsertNextValue(vtkMRMLScene::NodeAddedEvent);
    events->InsertNextValue(vtkMRMLFiducialListNode::DisplayModifiedEvent);
    events->InsertNextValue(vtkMRMLFiducialListNode::FiducialModifiedEvent);
    this->MRMLObserverManager->SetAndObserveObjectEvents(vtkObjectPointer(&VerificationPointListNode),VerificationPointListNode,events);
  }
  vtkMRMLProstateNavManagerNode* manager=this->GetProstateNavManager();
  if (manager!=NULL)
  {
    manager->AddObserver(vtkMRMLProstateNavManagerNode::CurrentTargetChangedEvent, this->MRMLCallbackCommand);    
  }
  if (this->MRMLScene!=NULL)
  {
    this->MRMLScene->AddObserver(vtkMRMLScene::NodeAddedEvent, this->MRMLCallbackCommand);
  }
}

//----------------------------------------------------------------------------
void vtkProstateNavStepVerification::RemoveMRMLObservers()
{
  vtkMRMLProstateNavManagerNode* manager=this->GetProstateNavManager();
  if (manager!=NULL)
  {
    manager->RemoveObservers(vtkMRMLProstateNavManagerNode::CurrentTargetChangedEvent, this->MRMLCallbackCommand);    
  }
  if (this->MRMLScene!=NULL)
  {
    this->MRMLScene->RemoveObservers(vtkMRMLScene::NodeAddedEvent, this->MRMLCallbackCommand);
  }
}

//---------------------------------------------------------------------------
void vtkProstateNavStepVerification::OnMultiColumnListSelection()
{

  vtkMRMLFiducialListNode* fidList = this->GetProstateNavManager()->GetTargetPlanListNode();

  if (fidList == NULL)
    {
    return;
    }

  int numRows = this->TargetList->GetWidget()->GetNumberOfSelectedRows();
  if (numRows == 1)
    {       
    int rowIndex = this->TargetList->GetWidget()->GetIndexOfFirstSelectedRow();    
    int targetIndex=this->TargetList->GetWidget()->GetRowAttributeAsInt(rowIndex, TARGET_INDEX_ATTR);
          
    this->GetProstateNavManager()->SetCurrentTargetIndex(targetIndex);    

    this->GUI->BringTargetToViewIn2DViews(vtkProstateNavGUI::BRING_MARKERS_TO_VIEW_ALIGN_TO_NEEDLE);
    }
}

//----------------------------------------------------------------------------
void vtkProstateNavStepVerification::UpdateTargetListGUI()
{
  if (this->TargetList==NULL || this->TargetList->GetWidget()==NULL)
    {
    // no widget, nothing to update
    return;
    }

  vtkMRMLFiducialListNode* activeFiducialListNode=NULL;
  if (this->GetProstateNavManager()!=NULL)
    {
    activeFiducialListNode=this->GetProstateNavManager()->GetTargetPlanListNode();
    }

  if (activeFiducialListNode == NULL)    //clear out the list box if no target list is available
    {
    if (this->TargetList->GetWidget()->GetNumberOfRows() != 0)
      {
      this->TargetList->GetWidget()->DeleteAllRows();
      }
    return;
    }
  
  // create new target points, if necessary
  this->GetLogic()->UpdateTargetListFromMRML();

  vtkMRMLProstateNavManagerNode *manager = this->GetGUI()->GetProstateNavManagerNode();
  if (!manager)
    {
    return;
    }

  //int numPoints = activeFiducialListNode->GetNumberOfFiducials();
  int numPoints = manager->GetTotalNumberOfTargets();

  bool recreateRows = false;
  if (numPoints != this->TargetList->GetWidget()->GetNumberOfRows())
    {
    // clear out the multi column list box and fill it in with the
    // new list
    this->TargetList->GetWidget()->DeleteAllRows();
    recreateRows = true;
    }
        
  double *xyz;

  for (int row = 0; row < numPoints; row++)
    {      
    int targetIndex=row;
    vtkProstateNavTargetDescriptor* target = manager->GetTargetDescriptorAtIndex(targetIndex);
    NeedleDescriptorStruct* needle = manager->GetNeedle(target);

    vtkKWMultiColumnList* columnList = this->TargetList->GetWidget();

    if (recreateRows)
      {
      // add a row for this point
      columnList->AddRow();
      }

    columnList->SetRowAttributeAsInt(row, TARGET_INDEX_ATTR, targetIndex);

    if (target->GetName().compare(columnList->GetCellText(row,COL_NAME)) != 0)
      {
      columnList->SetCellText(row,COL_NAME,target->GetName().c_str());
      }               

    xyz=target->GetRASLocation();
    if (xyz == NULL)
      {
      vtkErrorMacro ("UpdateTargetListGUI: ERROR: got null xyz for point " << row << endl);
      }
    // selected
    
    if (xyz != NULL)
      {
      for (int i = 0; i < 3; i ++) // for position (x, y, z)
        {
        if (recreateRows || columnList->GetCellTextAsDouble(row,COL_X+i) != xyz[i])
          {
          std::ostrstream os;    
          os << std::setiosflags(ios::fixed | ios::showpoint) << std::setprecision(vtkProstateNavGUI::POSITION_PRECISION_DIGITS);
          os << xyz[i] << std::ends;
          columnList->SetCellText(row,COL_X+i,os.str());
          os.rdbuf()->freeze();
          }
        }
      }

    if (needle->mDescription.compare(columnList->GetCellText(row,COL_NEEDLE)) != 0)
    {
      columnList->SetCellText(row,COL_NEEDLE,needle->mDescription.c_str());
    }

    if (target->GetTargetValidated())
      {
        if (recreateRows || columnList->GetCellTextAsDouble(row,COL_OVERALL_ERROR) != target->GetErrorDistanceFromNeedleLine())
        {
          std::ostrstream os;    
          os << std::setiosflags(ios::fixed | ios::showpoint) << std::setprecision(vtkProstateNavGUI::POSITION_PRECISION_DIGITS*2); // *2 to have double precision as the position that it was computed from
          os << target->GetErrorDistanceFromNeedleLine() << std::ends;
          columnList->SetCellText(row,COL_OVERALL_ERROR,os.str());
          os.rdbuf()->freeze();
        }
        if (recreateRows || columnList->GetCellTextAsDouble(row,COL_LR_ERROR) != target->GetErrorVector()[0])
        {
        std::ostrstream os;    
        os << std::setiosflags(ios::fixed | ios::showpoint) << std::setprecision(vtkProstateNavGUI::POSITION_PRECISION_DIGITS*3);
        os << target->GetErrorVector()[0] << std::ends;
        columnList->SetCellText(row,COL_LR_ERROR,os.str());
        os.rdbuf()->freeze();
        }
      if (recreateRows || columnList->GetCellTextAsDouble(row,COL_AP_ERROR) != target->GetErrorVector()[1])
        {
        std::ostrstream os;    
        os << std::setiosflags(ios::fixed | ios::showpoint) << std::setprecision(vtkProstateNavGUI::POSITION_PRECISION_DIGITS*3);
        os << target->GetErrorVector()[1] << std::ends;
        columnList->SetCellText(row,COL_AP_ERROR,os.str());
        os.rdbuf()->freeze();
        }
      if (recreateRows || columnList->GetCellTextAsDouble(row,COL_IS_ERROR) != target->GetErrorVector()[2])
        {
        std::ostrstream os;    
        os << std::setiosflags(ios::fixed | ios::showpoint) << std::setprecision(vtkProstateNavGUI::POSITION_PRECISION_DIGITS*3);
        os << target->GetErrorVector()[2] << std::ends;
        columnList->SetCellText(row,COL_IS_ERROR,os.str());
        os.rdbuf()->freeze();
        }
      }
    else
      {
        std::string emptyString;
        if (recreateRows || emptyString.compare(columnList->GetCellText(row,COL_OVERALL_ERROR))!=0)
          {
          columnList->SetCellText(row,COL_OVERALL_ERROR,emptyString.c_str());
          }
        if (recreateRows || emptyString.compare(columnList->GetCellText(row,COL_LR_ERROR))!=0)
          {
          columnList->SetCellText(row,COL_LR_ERROR,emptyString.c_str());
          }
        if (recreateRows || emptyString.compare(columnList->GetCellText(row,COL_AP_ERROR))!=0)
          {
          columnList->SetCellText(row,COL_AP_ERROR,emptyString.c_str());
          }
        if (recreateRows || emptyString.compare(columnList->GetCellText(row,COL_IS_ERROR))!=0)
          {
          columnList->SetCellText(row,COL_IS_ERROR,emptyString.c_str());
          }
      }
    }

  int selectedTargetIndex=-1;
  int numRows = this->TargetList->GetWidget()->GetNumberOfSelectedRows();
  if (numRows == 1)
  {       
    int rowIndex = this->TargetList->GetWidget()->GetIndexOfFirstSelectedRow();    
    selectedTargetIndex=this->TargetList->GetWidget()->GetRowAttributeAsInt(rowIndex, TARGET_INDEX_ATTR);
  }
  int currentTargetIndex=this->GetProstateNavManager()->GetCurrentTargetIndex();
  if (currentTargetIndex!=selectedTargetIndex)
  {
    for (int rowIndex=0; rowIndex<this->TargetList->GetWidget()->GetNumberOfRows(); rowIndex++)
    {
      if (this->TargetList->GetWidget()->GetRowAttributeAsInt(rowIndex, TARGET_INDEX_ATTR)==currentTargetIndex)
      {
        // found the row corresponding to the current target
        this->TargetList->GetWidget()->SelectSingleRow(rowIndex);
        break;
      }
    }
  } 

}



//-----------------------------------------------------------------------------
void vtkProstateNavStepVerification::AddGUIObservers()
{
  this->RemoveGUIObservers();

  if (this->LoadVerificationVolumeButton)
    {
    this->LoadVerificationVolumeButton->AddObserver(vtkKWPushButton::InvokedEvent, (vtkCommand *)this->GUICallbackCommand); 
    }
  if (this->ShowWorkspaceButton)
    {
    this->ShowWorkspaceButton->AddObserver(vtkKWCheckButton::SelectedStateChangedEvent, (vtkCommand *)this->GUICallbackCommand);
    }  
  if (this->ShowRobotButton)
    {
    this->ShowRobotButton->AddObserver(vtkKWCheckButton::SelectedStateChangedEvent, (vtkCommand *)this->GUICallbackCommand);
    }  
  if (this->VolumeSelectorWidget)
    {
    this->VolumeSelectorWidget->AddObserver ( vtkSlicerNodeSelectorWidget::NodeSelectedEvent, (vtkCommand *)this->GUICallbackCommand);  
    }
  if (this->VerifyButton)
    {
    this->VerifyButton->AddObserver(vtkKWPushButton::InvokedEvent, (vtkCommand *)this->GUICallbackCommand);
    }
  if (this->ClearButton)
    {
    this->ClearButton->AddObserver(vtkKWPushButton::InvokedEvent, (vtkCommand *)this->GUICallbackCommand);
    }
  if (this->TargetList)
    {
    this->TargetList->GetWidget()->SetCellUpdatedCommand(this, "OnMultiColumnListUpdate");
    this->TargetList->GetWidget()->SetSelectionCommand(this, "OnMultiColumnListSelection");
    }
}
//-----------------------------------------------------------------------------
void vtkProstateNavStepVerification::RemoveGUIObservers()
{
  if (this->LoadVerificationVolumeButton)
    {
    this->LoadVerificationVolumeButton->RemoveObserver((vtkCommand *)this->GUICallbackCommand); 
    }
  if (this->ShowWorkspaceButton)
    {
    this->ShowWorkspaceButton->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }  
  if (this->ShowRobotButton)
    {
    this->ShowRobotButton->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }  
  if (this->VolumeSelectorWidget)
    {
    this->VolumeSelectorWidget->RemoveObserver ((vtkCommand *)this->GUICallbackCommand);  
    }
  if (this->VerifyButton)
    {
    this->VerifyButton->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }
  if (this->ClearButton)
    {
    this->ClearButton->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }
  if (this->TargetList)
    {
    this->TargetList->GetWidget()->SetCellUpdatedCommand(this, "");
    this->TargetList->GetWidget()->SetSelectionCommand(this, "");
    }
}

//--------------------------------------------------------------------------------
void vtkProstateNavStepVerification::UpdateGUI()
{
  vtkMRMLProstateNavManagerNode *mrmlNode = this->GetGUI()->GetProstateNavManagerNode();

  if (!mrmlNode)
  {
    return;
  }

  // Display information about the currently selected target descriptor    
  if (this->Message)
  {    
    vtkMRMLRobotNode* robot=NULL;
    if (this->GetProstateNavManager()!=NULL)
    {
      robot=this->GetProstateNavManager()->GetRobotNode();
    }
    this->Message->SetText("Select a target, click on 'Verify target' button, then click on two points along the visible needle line.");
  }
  const char* volNodeID = mrmlNode->GetVerificationVolumeNodeRef();
  vtkMRMLScalarVolumeNode *volNode=vtkMRMLScalarVolumeNode::SafeDownCast(this->GetLogic()->GetApplicationLogic()->GetMRMLScene()->GetNodeByID(volNodeID));
  if ( volNode && this->VolumeSelectorWidget!=NULL && this->VolumeSelectorWidget->IsCreated())
  {
    this->VolumeSelectorWidget->UpdateMenu();
    this->VolumeSelectorWidget->SetSelected( volNode );
  }

  UpdateTargetListGUI();

  if (this->ShowRobotButton &&this->ShowRobotButton->IsCreated()) 
  {
    this->ShowRobotButton->SetSelectedState(IsRobotModelShown());
  }
  if (this->ShowWorkspaceButton &&this->ShowWorkspaceButton->IsCreated()) 
  {
    this->ShowWorkspaceButton->SetSelectedState(IsWorkspaceModelShown());  
  }  

  DisplayVerificationResultsForCurrentTarget();
}

//----------------------------------------------------------------------------
void vtkProstateNavStepVerification::HideUserInterface()
{
  TearDownGUI(); // HideUserInterface deletes the reference to the scene, so TearDownGUI shall be done before calling HideUserInterface
  Superclass::HideUserInterface();
}

//----------------------------------------------------------------------------
void vtkProstateNavStepVerification::TearDownGUI()
{  
  RemoveGUIObservers();
  RemoveMRMLObservers();

  SetVerificationPointListNode(NULL);
}

//----------------------------------------------------------------------------
void vtkProstateNavStepVerification::StartVerification()
{
  // Activate target fiducials in the Fiducial GUI
  if (this->GetLogic()==NULL)
  {
    vtkErrorMacro("Logic is invalid");
    return;
  }    
  if (this->GetProstateNavManager()==NULL)
  {
    vtkErrorMacro("Manager is invalid");
    return;
  }      
  if (this->VerificationPointListNode==NULL)
  {
    vtkErrorMacro("VerificationPointListNode is invalid");
    return;
  }      
  if (this->GetProstateNavManager()->GetCurrentTargetIndex()<0)
  {
    // no selected target
    return;
  }        

  this->TargetIndexUnderVerification=-1; // updates may be called while modifying MRML nodes, make sure that no verification is attempted

  // Clear previous verif points
  int oldModify=this->VerificationPointListNode->StartModify();
  this->VerificationPointListNode->SetLocked(false);
  this->VerificationPointListNode->RemoveAllFiducials();
  this->VerificationPointListNode->EndModify(oldModify); // triggers stop verification

  // Lock GUI
  this->TargetList->SetEnabled(false);
  this->VerifyButton->SetEnabled(false);

  // Set fiducial placement mode
  GetLogic()->SetCurrentFiducialList(this->VerificationPointListNode);
  GetLogic()->SetMouseInteractionMode(vtkMRMLInteractionNode::Place); 

  this->TargetIndexUnderVerification=this->GetProstateNavManager()->GetCurrentTargetIndex();
}

//----------------------------------------------------------------------------
void vtkProstateNavStepVerification::StopVerification()
{
  // Deactivate target fiducials in the Fiducial GUI
  if (this->GetLogic()==NULL)
  {
    vtkErrorMacro("Logic is invalid");
    return;
  }    
  this->TargetIndexUnderVerification=-1;
  // Exit from fiducial placement mode
  GetLogic()->SetMouseInteractionMode(vtkMRMLInteractionNode::ViewTransform);
  // Unlock GUI
  this->VerificationPointListNode->SetLocked(true);
  this->TargetList->SetEnabled(true);
  this->VerifyButton->SetEnabled(true);
}


void vtkProstateNavStepVerification::UpdateVerificationResultsForCurrentTarget()
{
  if (this->TargetIndexUnderVerification<0)
  {
    // no current target under verification
    this->VerificationPointListNode->RemoveAllFiducials();
    return;
  }

  if (this->VerificationPointListNode->GetNumberOfFiducials()>0)
  {    
    if (strcmp(this->VerificationPointListNode->GetNthFiducialLabelText(0),NEEDLE_TIP_LABEL)!=0)
    {
      this->VerificationPointListNode->SetNthFiducialLabelText(0,NEEDLE_TIP_LABEL);      
    }
  }
  if (this->VerificationPointListNode->GetNumberOfFiducials()>1)
  {  
    if (strcmp(this->VerificationPointListNode->GetNthFiducialLabelText(1),NEEDLE_BASE_LABEL)!=0)
    {
      this->VerificationPointListNode->SetNthFiducialLabelText(1,NEEDLE_BASE_LABEL);      
    }
  }

  if (this->GetProstateNavManager()==NULL)
  {
    vtkErrorMacro("Invalid manager");
    return;
  }
  if (this->VerificationPointListNode->GetNumberOfFiducials()<2)
  {
    // not enough fiducials to compute result
    return;
  }

  vtkProstateNavTargetDescriptor* targetDesc=this->GetProstateNavManager()->GetTargetDescriptorAtIndex(this->TargetIndexUnderVerification);

  float* needleTipPos=this->VerificationPointListNode->GetNthFiducialXYZ(0);
  float* needleBasePos=this->VerificationPointListNode->GetNthFiducialXYZ(1);
  targetDesc->SetNeedleTipValidationPosition(needleTipPos[0],needleTipPos[1],needleTipPos[2]);
  targetDesc->SetNeedleBaseValidationPosition(needleBasePos[0],needleBasePos[1],needleBasePos[2]);

  double overallErr = 0;
  double apErr = 0; double lrErr = 0; double isErr = 0;
  overallErr = ProstateNavMath::ComputeDistanceLinePoint(targetDesc->GetNeedleTipValidationPosition(), targetDesc->GetNeedleBaseValidationPosition(),
    targetDesc->GetRASLocation(), apErr, lrErr, isErr);

  // set the variable values in target descriptor
  targetDesc->SetTargetValidated(true);
  targetDesc->SetErrorDistanceFromNeedleLine(overallErr);
  targetDesc->SetErrorVector(apErr, lrErr, isErr);

  this->GetProstateNavManager()->Modified();
}

void vtkProstateNavStepVerification::DisplayVerificationResultsForCurrentTarget()
{
  if (this->TargetIndexUnderVerification>=0)
  {
    // verification is in progress, don't display verification results
    return;
  }
  if (GetProstateNavManager()==NULL)
  {
    vtkErrorMacro("Invalid manager");
    return;
  }

  int targetIndex=GetProstateNavManager()->GetCurrentTargetIndex(); 
  vtkProstateNavTargetDescriptor* targetDesc=NULL;
  if (targetIndex>=0)
  {
    targetDesc=this->GetProstateNavManager()->GetTargetDescriptorAtIndex(targetIndex);    
  }
  bool targetValidated=false;
  if (targetDesc!=NULL)
  {
    targetValidated=targetDesc->GetTargetValidated();
  }

  if (this->VerificationPointListNode==NULL)
  {
    // no node has been created yet
    return;
  }

  if (!targetValidated)
  {
    // no target is selected
    this->VerificationPointListNode->SetAllFiducialsVisibility(false);
    return;
  }

  // Temporariy disable fiducial monitoring. This allows us to update the fiducials without triggering recomputation/validation of results
  this->MonitorFiducialNodes=false;
  double* needlePos=targetDesc->GetNeedleTipValidationPosition();  
  if (this->VerificationPointListNode->GetNumberOfFiducials()<1)
  {    
    this->VerificationPointListNode->AddFiducialWithLabelXYZSelectedVisibility(NEEDLE_TIP_LABEL,needlePos[0],needlePos[1],needlePos[2],true,true);
  }
  else
  {
    float* fidXYZ=this->VerificationPointListNode->GetNthFiducialXYZ(0);
    if (fidXYZ[0]!=needlePos[0] || fidXYZ[1]!=needlePos[1] || fidXYZ[2]!=needlePos[2])
    {
      this->VerificationPointListNode->SetNthFiducialXYZ(0,needlePos[0],needlePos[1],needlePos[2]);
    }
  }
  needlePos=targetDesc->GetNeedleBaseValidationPosition();  
  if (this->VerificationPointListNode->GetNumberOfFiducials()<2)
  {    
    this->VerificationPointListNode->AddFiducialWithLabelXYZSelectedVisibility(NEEDLE_BASE_LABEL,needlePos[0],needlePos[1],needlePos[2],true,true);
  }
  else
  {
    float* fidXYZ=this->VerificationPointListNode->GetNthFiducialXYZ(1);
    if (fidXYZ[0]!=needlePos[0] || fidXYZ[1]!=needlePos[1] || fidXYZ[2]!=needlePos[2])
    {
      this->VerificationPointListNode->SetNthFiducialXYZ(1,needlePos[0],needlePos[1],needlePos[2]);
    }
  }
  this->MonitorFiducialNodes=true;
}
