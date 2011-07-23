#include "vtkObject.h"
#include "vtkObjectFactory.h"
#include "vtkCommand.h"
#include "vtkCamera.h"
#include "vtkRenderer.h"
#include "vtkFollower.h"
#include "vtkProperty.h"
#include "vtkActorCollection.h"
#include "vtkSlicerInteractorStyle.h"
#include "vtkMath.h"

#include "vtkSlicerApplication.h"
#include "vtkSlicerApplicationGUI.h"
#include "vtkSlicerModuleGUI.h"
#include "vtkSlicerViewControlGUI.h"
#include "vtkSlicerWindow.h"
#include "vtkSlicerTheme.h"

#include "vtkKWTopLevel.h"
#include "vtkKWFrame.h"
#include "vtkKWWidget.h"
#include "vtkKWScale.h"
#include "vtkKWScaleWithEntry.h"
#include "vtkKWPushButton.h"
#include "vtkKWCheckButton.h"
#include "vtkKWRadioButton.h"
#include "vtkKWLabel.h"
#include "vtkKWMenuButton.h"
#include "vtkKWMenu.h"
#include "vtkKWEntry.h"
#include "vtkKWEntryWithLabel.h"
#include "vtkKWSimpleEntryDialog.h"
#include "vtkKWTkUtilities.h"
#include "vtkKWRenderWidget.h"
#include "vtkSlicerViewControlIcons.h"
#include "vtkSlicerFoundationIcons.h"
#include "vtkMRMLSceneSnapshotNode.h"
#include "vtkKWLoadSaveButton.h"

#include "vtkMRMLSceneSnapshotNode.h"

// uncomment in order to stub out the NavZoom widget.
//#define NAVZOOMWIDGET_DEBUG


#define DEGREES2RADIANS 0.0174532925
#define RADIANS2DEGREES 57.295779513
#define NUM_ZOOM_INCREMENTS 20.0


//---------------------------------------------------------------------------
vtkStandardNewMacro (vtkSlicerViewControlGUI );
vtkCxxRevisionMacro ( vtkSlicerViewControlGUI, "$Revision: 15119 $");


//---------------------------------------------------------------------------
vtkSlicerViewControlGUI::vtkSlicerViewControlGUI ( )
{
  
  this->NavigationRenderPending = 0;
  this->ZoomRenderPending = 0;
  this->EntryUpdatePending = 0;
  this->SceneClosing = false;
  this->ProcessingMRMLEvent = 0;
  this->RockCount = 0;
  this->NavigationZoomWidgetWid = 150;
  this->NavigationZoomWidgetHit = 80;
  this->MySnapshotName = "view";


  this->SliceMagnification = 10.0;
  this->SliceInteracting = 0;
  this->MySnapshotName = "view";

  this->NameDialog = NULL;
  this->SlicerViewControlIcons = NULL;
  this->SpinButton = NULL;
  this->RockButton = NULL;
  this->OrthoButton = NULL;

  this->NameDialog = NULL;
  this->CenterButton = NULL;
  this->StereoButton = NULL;
  this->ScreenGrabButton = NULL;
  this->VisibilityButton = NULL;
  this->SelectSceneSnapshotMenuButton = NULL;
  this->SceneSnapshotButton = NULL;

  //--- ui for the ViewControlFrame
  this->ViewAxisAIconButton = NULL;
  this->ViewAxisPIconButton = NULL;
  this->ViewAxisRIconButton = NULL;
  this->ViewAxisLIconButton = NULL;
  this->ViewAxisSIconButton = NULL;
  this->ViewAxisIIconButton = NULL;
  this->ViewAxisCenterIconButton = NULL;
  this->ViewAxisTopCornerIconButton = NULL;
  this->ViewAxisBottomCornerIconButton = NULL;

  // temporary thing until navzoom window is built.
  this->NavigationWidget = NULL;
  this->ZoomWidget = NULL;
  this->NavigationZoomFrame = NULL;

  this->SliceMagnifier = NULL;
  this->SliceMagnifierCursor = NULL;
  this->SliceMagnifierMapper = NULL;
  this->SliceMagnifierActor = NULL;

  this->FOVBox = NULL;
  this->FOVBoxMapper = NULL;
  this->FOVBoxActor = NULL;

  //Enable/Disable navigation window
  this->EnableDisableNavButton = NULL;
  
  this->ViewNode = NULL;
  this->RedSliceNode = NULL;
  this->YellowSliceNode = NULL;
  this->GreenSliceNode = NULL;
  this->RedSliceEvents = NULL;
  this->YellowSliceEvents = NULL;
  this->GreenSliceEvents = NULL;
  this->MainViewerEvents = NULL;

  this->PitchButton = NULL;
  this->RollButton = NULL;
  this->YawButton = NULL;
  this->ZoomInButton = NULL;
  this->ZoomOutButton = NULL;
  
//--- Screen snapshot configure window
  this->ScreenGrabFormatMenuButton = NULL;
  this->ScreenGrabOptionsWindow = NULL;
  this->ScreenGrabNameEntry = NULL;
  this->ScreenGrabNumberEntry = NULL;
  this->ScreenGrabOverwriteButton = NULL;
  this->ScreenGrabCaptureButton = NULL;
  this->ScreenGrabCloseButton = NULL;
  this->ScreenGrabDialogButton = NULL;
  this->ScreenGrabMagnificationEntry  = NULL;
  this->ScreenGrabOverwrite = 0;
  
  this->ScreenGrabDirectory = NULL;
  this->ScreenGrabName = NULL;
  this->ScreenGrabNumber = 0;
  this->ScreenGrabMagnification = 1;
  this->ScreenGrabFormat = ".png";


  this->SetAndObserveMRMLScene ( NULL );
  this->SetApplicationGUI ( NULL);
}


//---------------------------------------------------------------------------
void vtkSlicerViewControlGUI::TearDownGUI ( )
{
  this->SelectSceneSnapshotMenuButton->GetMenu()->DeleteAllItems();
  this->RemoveSliceEventObservers();
  this->RemoveMainViewerEventObservers();

  this->SetAndObserveMRMLScene ( NULL );
  this->SetApplicationGUI ( NULL);
  this->SetApplication ( NULL );
}


//---------------------------------------------------------------------------
vtkSlicerViewControlGUI::~vtkSlicerViewControlGUI ( )
{

  this->NavigationRenderPending = 0;
  this->ZoomRenderPending = 0;
  this->EntryUpdatePending = 0;
  this->SceneClosing = false;
  this->RockCount = 0;  
  this->SliceInteracting = 0;

  if ( this->NameDialog )
    {
    this->NameDialog->SetParent ( NULL );
    this->NameDialog->Delete();
    this->NameDialog = NULL;
    }
  if ( this->SliceMagnifier )
    {
    this->SliceMagnifier->Delete();
    this->SliceMagnifier = NULL;
    }
  if ( this->SliceMagnifierCursor )
    {
    this->SliceMagnifierCursor->Delete();
    this->SliceMagnifierCursor = NULL;
    }
  if ( this->SliceMagnifierMapper)
    {
    this->SliceMagnifierMapper->Delete();
    this->SliceMagnifierMapper = NULL;
    }
  if ( this->SliceMagnifierActor)
    {
    this->SliceMagnifierActor->Delete();
    this->SliceMagnifierActor = NULL;
    }
  if ( this->SlicerViewControlIcons )
    {
    this->SlicerViewControlIcons->Delete ( );
    this->SlicerViewControlIcons = NULL;
    }
   //--- widgets from ViewControlFrame
  if ( this->SpinButton ) 
    {
    this->SpinButton->SetParent ( NULL );
    this->SpinButton->Delete();
    this->SpinButton = NULL;
    }
  if ( this->RockButton) 
    {
    this->RockButton->SetParent ( NULL );
    this->RockButton->Delete();
    this->RockButton = NULL;
    }
  if ( this->OrthoButton ) 
    {
    this->OrthoButton->SetParent ( NULL );
    this->OrthoButton->Delete();
    this->OrthoButton = NULL;
    }
  if ( this->CenterButton ) 
    {
    this->CenterButton->SetParent ( NULL );      
    this->CenterButton->Delete();
    this->CenterButton = NULL;
    }
  if ( this->ScreenGrabButton ) 
    {
    this->ScreenGrabButton->SetParent ( NULL );
    this->ScreenGrabButton->Delete();
    this->ScreenGrabButton = NULL;
    }

  if ( this->StereoButton ) 
    {
    this->StereoButton->SetParent ( NULL );
    this->StereoButton->Delete();
    this->StereoButton = NULL;
    }
  if ( this->VisibilityButton ) 
    {
    this->VisibilityButton->SetParent ( NULL );
    this->VisibilityButton->Delete();
    this->VisibilityButton = NULL;
    }
  if ( this->SelectSceneSnapshotMenuButton )
    {
    this->SelectSceneSnapshotMenuButton->SetParent ( NULL );
    this->SelectSceneSnapshotMenuButton->Delete();
    this->SelectSceneSnapshotMenuButton = NULL;
    }
  if ( this->SceneSnapshotButton )
    {
    this->SceneSnapshotButton->SetParent ( NULL );
    this->SceneSnapshotButton->Delete();
    this->SceneSnapshotButton = NULL;    
    }
  if ( this->ViewAxisAIconButton ) 
    {
    this->ViewAxisAIconButton->SetParent ( NULL );      
    this->ViewAxisAIconButton->Delete ( );
    this->ViewAxisAIconButton = NULL;
    }
  if ( this->ViewAxisPIconButton ) 
    {
    this->ViewAxisPIconButton->SetParent ( NULL );
    this->ViewAxisPIconButton->Delete ( );
    this->ViewAxisPIconButton = NULL;
   }
  if ( this->ViewAxisRIconButton ) 
    {
    this->ViewAxisRIconButton->SetParent ( NULL );
    this->ViewAxisRIconButton->Delete ( );
    this->ViewAxisRIconButton = NULL;
    }
  if ( this->ViewAxisLIconButton ) 
    {
    this->ViewAxisLIconButton->SetParent ( NULL );
    this->ViewAxisLIconButton->Delete ( );
    this->ViewAxisLIconButton = NULL;
    }
  if ( this->ViewAxisSIconButton ) 
    {
    this->ViewAxisSIconButton->SetParent ( NULL );
    this->ViewAxisSIconButton->Delete ( );
    this->ViewAxisSIconButton = NULL;
    }
  if ( this->ViewAxisIIconButton ) 
    {
    this->ViewAxisIIconButton->SetParent ( NULL );
    this->ViewAxisIIconButton->Delete ( );
    this->ViewAxisIIconButton = NULL;
    }
  if ( this->ViewAxisCenterIconButton ) 
    {
    this->ViewAxisCenterIconButton->SetParent ( NULL );
    this->ViewAxisCenterIconButton->Delete ( );
    this->ViewAxisCenterIconButton = NULL;
    }
  if ( this->ViewAxisTopCornerIconButton ) 
    {
    this->ViewAxisTopCornerIconButton->SetParent ( NULL );
    this->ViewAxisTopCornerIconButton->Delete ( );
    this->ViewAxisTopCornerIconButton = NULL;
    }
  if ( this->ViewAxisBottomCornerIconButton ) 
    {
    this->ViewAxisBottomCornerIconButton->SetParent ( NULL );
    this->ViewAxisBottomCornerIconButton->Delete ( );
    this->ViewAxisBottomCornerIconButton = NULL;
    }
  if ( this->NavigationWidget )
    {
    this->NavigationWidget->SetParent ( NULL );
    this->NavigationWidget->Delete ();
    this->NavigationWidget = NULL;
    }
  if( this->EnableDisableNavButton)
  {
      this->EnableDisableNavButton->SetParent ( NULL );
      this->EnableDisableNavButton->Delete ();
      this->EnableDisableNavButton = NULL;
  }
  if ( this->ZoomWidget )
    {
    this->ZoomWidget->SetParent ( NULL );
    this->ZoomWidget->Delete ();
    this->ZoomWidget = NULL;
    }
  if ( this->NavigationZoomFrame )
    {
    this->NavigationZoomFrame->SetParent ( NULL );
    this->NavigationZoomFrame->Delete ();
    this->NavigationZoomFrame = NULL;
    }
  if ( this->FOVBox )
    {
    this->FOVBox->Delete();
    this->FOVBox = NULL;
    }
  if ( this->FOVBoxMapper )
    {
    this->FOVBoxMapper->Delete();
    this->FOVBoxMapper = NULL;
    }
  if (this->FOVBoxActor)
    {
    this->FOVBoxActor->Delete();
    this->FOVBoxActor = NULL;
    }

  if ( this->PitchButton )
    {
    this->PitchButton->SetParent ( NULL );
    this->PitchButton->Delete();
    this->PitchButton = NULL;
    }
  if ( this->RollButton )
    {
    this->RollButton->SetParent ( NULL );
    this->RollButton->Delete();    
    this->RollButton = NULL;    
    }
  if ( this->YawButton )
    {
    this->YawButton->SetParent ( NULL );
    this->YawButton->Delete();
    this->YawButton = NULL;    
    }
  if ( this->ZoomInButton )
    {
    this->ZoomInButton->SetParent ( NULL );
    this->ZoomInButton->Delete();
    this->ZoomInButton = NULL;    
    }
  if ( this->ZoomOutButton )
    {
    this->ZoomOutButton->SetParent ( NULL );
    this->ZoomOutButton->Delete();
    this->ZoomOutButton = NULL;    
    }

  //--- Screen snapshot configure window
  this->DestroyScreenGrabOptionsWindow();
  this->SetScreenGrabDirectory ( NULL );
  this->SetScreenGrabName ( NULL );

  vtkSetAndObserveMRMLNodeMacro ( this->ViewNode, NULL );
  vtkSetAndObserveMRMLNodeMacro ( this->RedSliceNode, NULL );
  vtkSetAndObserveMRMLNodeMacro ( this->GreenSliceNode, NULL );
  vtkSetAndObserveMRMLNodeMacro ( this->YellowSliceNode, NULL );
  this->RemoveSliceEventObservers();
  this->RemoveMainViewerEventObservers();
  this->SetRedSliceEvents(NULL);
  this->SetYellowSliceEvents(NULL);
  this->SetGreenSliceEvents(NULL);
  this->SetMainViewerEvents ( NULL );
  this->SetAndObserveMRMLScene ( NULL );
  this->SetApplicationGUI ( NULL );
}




//---------------------------------------------------------------------------
void vtkSlicerViewControlGUI::MakeViewControlRolloverBehavior ( )
{

  //--- configure and bind for rollover interaction
  //--- as there are no events on labels, we'll do this the old fashioned way.
  
  this->ViewAxisAIconButton->SetBorderWidth (0);
  this->ViewAxisAIconButton->SetBinding ( "<Enter>",  this, "EnterViewAxisACallback");
  this->ViewAxisAIconButton->SetBinding ( "<Leave>",  this, "LeaveViewAxisACallback");
  this->ViewAxisAIconButton->SetBinding ( "<Button-1>", this, "ViewControlACallback");
  this->Script ( "%s ListMethods", this->GetTclName() );

  this->ViewAxisPIconButton->SetBorderWidth (0);
  this->ViewAxisPIconButton->SetBinding ( "<Enter>", this, "EnterViewAxisPCallback");
  this->ViewAxisPIconButton->SetBinding ( "<Leave>", this, "LeaveViewAxisPCallback");
  this->ViewAxisPIconButton->SetBinding ( "<Button-1>", this, "ViewControlPCallback");

  this->ViewAxisRIconButton->SetBorderWidth (0);
  this->ViewAxisRIconButton->SetBinding ( "<Enter>", this, "EnterViewAxisRCallback");
  this->ViewAxisRIconButton->SetBinding ( "<Leave>", this, "LeaveViewAxisRCallback");
  this->ViewAxisRIconButton->SetBinding ( "<Button-1>", this, "ViewControlRCallback");
  
  this->ViewAxisLIconButton->SetBorderWidth (0);
  this->ViewAxisLIconButton->SetBinding ( "<Enter>", this, "EnterViewAxisLCallback");
  this->ViewAxisLIconButton->SetBinding ( "<Leave>", this, "LeaveViewAxisLCallback");
  this->ViewAxisLIconButton->SetBinding ( "<Button-1>", this, "ViewControlLCallback");

  this->ViewAxisSIconButton->SetBorderWidth (0);
  this->ViewAxisSIconButton->SetBinding ( "<Enter>", this, "EnterViewAxisSCallback");
  this->ViewAxisSIconButton->SetBinding ( "<Leave>", this, "LeaveViewAxisSCallback");
  this->ViewAxisSIconButton->SetBinding ( "<Button-1>", this, "ViewControlSCallback");
  
  this->ViewAxisIIconButton->SetBorderWidth (0);
  this->ViewAxisIIconButton->SetBinding ( "<Enter>", this, "EnterViewAxisICallback");
  this->ViewAxisIIconButton->SetBinding ( "<Leave>", this, "LeaveViewAxisICallback");
  this->ViewAxisIIconButton->SetBinding ( "<Button-1>", this, "ViewControlICallback");
  
  this->ViewAxisCenterIconButton->SetBorderWidth (0);
  this->ViewAxisTopCornerIconButton->SetBorderWidth (0);
  this->ViewAxisBottomCornerIconButton->SetBorderWidth (0);
}



//---------------------------------------------------------------------------
void vtkSlicerViewControlGUI::PrintSelf ( ostream& os, vtkIndent indent )
{
  //TODO: need to print everything
  this->vtkObject::PrintSelf ( os, indent );

  // eventuall these get moved into the view node...
  os << indent << "SlicerViewControlGUI: " << this->GetClassName ( ) << "\n";

  // class widgets
  os << indent << "ViewAxisAIconButton: " << this->GetViewAxisAIconButton (  ) << "\n";
  os << indent << "ViewAxisPIconButton: " << this->GetViewAxisPIconButton (  ) << "\n";
  os << indent << "ViewAxisRIconButton: " << this->GetViewAxisRIconButton (  ) << "\n";
  os << indent << "ViewAxisLIconButton: " << this->GetViewAxisLIconButton (  ) << "\n";
  os << indent << "ViewAxisSIconButton: " << this->GetViewAxisSIconButton (  ) << "\n";
  os << indent << "ViewAxisIIconButton: " << this->GetViewAxisIIconButton (  ) << "\n";
  os << indent << "ViewAxisCenterIconButton: " << this->GetViewAxisCenterIconButton (  ) << "\n";
  os << indent << "ViewAxisTopCornerIconButton: " << this->GetViewAxisTopCornerIconButton (  ) << "\n";
  os << indent << "ViewAxisBottomCornerIconButton: " << this->GetViewAxisBottomCornerIconButton (  ) << "\n";
  os << indent << "SpinButton: " << this->GetSpinButton(  ) << "\n";
  os << indent << "RockButton: " << this->GetRockButton(  ) << "\n";
  os << indent << "OrthoButton: " << this->GetOrthoButton(  ) << "\n";
  os << indent << "CenterButton: " << this->GetCenterButton(  ) << "\n";
  os << indent << "StereoButton: " << this->GetStereoButton(  ) << "\n";
  os << indent << "ScreenGrabButton: " << this->GetScreenGrabButton(  ) << "\n";
  os << indent << "VisibilityButton: " << this->GetVisibilityButton(  ) << "\n";
  os << indent << "SelectSceneSnapshotMenuButton: " << this->GetSelectSceneSnapshotMenuButton ( ) << "\n";
  os << indent << "SceneSnapshotButton: " << this->GetSceneSnapshotButton() << "\n";

  os << indent << "PitchButton: " << this->GetPitchButton() << "\n";
  os << indent << "RollButton: " << this->GetRollButton() << "\n";
  os << indent << "YawButton: " << this->GetYawButton() << "\n";
  os << indent << "ZoomInButton: " << this->GetZoomInButton() << "\n";    
  os << indent << "ZoomOutButton: " << this->GetZoomOutButton() << "\n";    

  os << indent << "SlicerViewControlIcons: " << this->GetSlicerViewControlIcons(  ) << "\n";
  os << indent << "ApplicationGUI: " << this->GetApplicationGUI(  ) << "\n";

  os << indent << "NavigationWidget: " << this->GetNavigationWidget(  ) << "\n";    
  os << indent << "ZoomWidget: " << this->GetZoomWidget(  ) << "\n";    
  os << indent << "NavigationZoomFrame: " << this->GetNavigationZoomFrame(  ) << "\n";

  os << indent << "SliceMagnifier: " << this->GetSliceMagnifier(  ) << "\n";
  os << indent << "SliceMagnifierCursor: " << this->GetSliceMagnifierCursor(  ) << "\n";    
  os << indent << "SliceMagnifierMapper: " << this->GetSliceMagnifierMapper(  ) << "\n";    
  os << indent << "SliceMagnifierActor: " << this->GetSliceMagnifierActor(  ) << "\n";    
  
  os << indent << "FOVBox: " << this->GetFOVBox(  ) << "\n";
  os << indent << "FOVBoxMapper: " << this->GetFOVBoxMapper(  ) << "\n";
  os << indent << "FOVBoxActor: " << this->GetFOVBoxActor(  ) << "\n";

  os << indent << "ViewNode: " << this->GetViewNode(  ) << "\n";    
  os << indent << "RedSliceNode: " << this->GetRedSliceNode(  ) << "\n";
  os << indent << "GreenSliceNode: " << this->GetGreenSliceNode(  ) << "\n";    
  os << indent << "YellowSliceNode: " << this->GetYellowSliceNode(  ) << "\n";    
  os << indent << "RedSliceEvents: " << this->GetRedSliceEvents(  ) << "\n";    
  os << indent << "GreenSliceEvents: " << this->GetGreenSliceEvents(  ) << "\n";    
  os << indent << "YellowSliceEvents: " << this->GetYellowSliceEvents(  ) << "\n";    
  os << indent << "MainViewerEvents: " << this->GetMainViewerEvents() << "\n";

  os << indent << "NavigationRenderPending: " << this->GetNavigationRenderPending(  ) << "\n";
  os << indent << "ZoomRenderPending: " << this->GetZoomRenderPending(  ) << "\n";
  os << indent << "EntryUpdatePending: " << this->GetEntryUpdatePending(  ) << "\n";  
  os << indent << "ProcessingMRMLEvent: " << this->GetProcessingMRMLEvent(  ) << "\n";  
  os << indent << "RockCount: " << this->GetRockCount(  ) << "\n";      
  os << indent << "NavigationZoomWidgetWid: " << this->GetNavigationZoomWidgetWid(  ) << "\n";    
  os << indent << "NavigationZoomWidgetHit: " << this->GetNavigationZoomWidgetHit(  ) << "\n";
  os << indent << "SliceMagnification: " << this->GetSliceMagnification(  ) << "\n";
  os << indent << "SliceInteracting: " << this->GetSliceInteracting(  ) << "\n";      
}




//---------------------------------------------------------------------------
void vtkSlicerViewControlGUI::RemoveGUIObservers ( )
{
  // FILL IN
    this->SpinButton->RemoveObservers (vtkKWCheckButton::SelectedStateChangedEvent, (vtkCommand *)this->GUICallbackCommand );
    this->RockButton->RemoveObservers (vtkKWCheckButton::SelectedStateChangedEvent, (vtkCommand *)this->GUICallbackCommand );

    this->OrthoButton->RemoveObservers (vtkKWPushButton::InvokedEvent, (vtkCommand *)this->GUICallbackCommand );
    this->StereoButton->GetMenu()->RemoveObservers (vtkKWMenu::MenuItemInvokedEvent, (vtkCommand *)this->GUICallbackCommand );
    this->CenterButton->RemoveObservers (vtkKWPushButton::InvokedEvent, (vtkCommand *)this->GUICallbackCommand );
    this->ScreenGrabButton->RemoveObservers (vtkKWPushButton::InvokedEvent, (vtkCommand *)this->GUICallbackCommand );
    this->VisibilityButton->GetMenu()->RemoveObservers (vtkKWMenu::MenuItemInvokedEvent, (vtkCommand *)this->GUICallbackCommand );
    this->SelectSceneSnapshotMenuButton->GetMenu()->RemoveObservers ( vtkKWMenu::MenuItemInvokedEvent, (vtkCommand *)this->GUICallbackCommand );
    this->SelectSceneSnapshotMenuButton->GetMenu()->RemoveObservers ( vtkKWMenu::MenuItemInvokedEvent, (vtkCommand *)this->GUICallbackCommand );
    this->SceneSnapshotButton->RemoveObservers (vtkKWPushButton::InvokedEvent, (vtkCommand *)this->GUICallbackCommand );

    this->PitchButton->RemoveObservers (vtkKWPushButton::InvokedEvent, (vtkCommand *)this->GUICallbackCommand );
    this->RollButton->RemoveObservers (vtkKWPushButton::InvokedEvent, (vtkCommand *)this->GUICallbackCommand );
    this->YawButton->RemoveObservers (vtkKWPushButton::InvokedEvent, (vtkCommand *)this->GUICallbackCommand );
    this->ZoomInButton->RemoveObservers (vtkKWPushButton::InvokedEvent, (vtkCommand *)this->GUICallbackCommand );
    this->ZoomOutButton->RemoveObservers (vtkKWPushButton::InvokedEvent, (vtkCommand *)this->GUICallbackCommand );
}



//---------------------------------------------------------------------------
void vtkSlicerViewControlGUI::AddGUIObservers ( )
{
  // FILL IN
    this->SpinButton->AddObserver (vtkKWCheckButton::SelectedStateChangedEvent, (vtkCommand *)this->GUICallbackCommand );
    this->RockButton->AddObserver (vtkKWCheckButton::SelectedStateChangedEvent, (vtkCommand *)this->GUICallbackCommand );

    this->OrthoButton->AddObserver (vtkKWPushButton::InvokedEvent, (vtkCommand *)this->GUICallbackCommand );
    this->StereoButton->GetMenu()->AddObserver (vtkKWMenu::MenuItemInvokedEvent, (vtkCommand *)this->GUICallbackCommand );
    this->CenterButton->AddObserver (vtkKWPushButton::InvokedEvent, (vtkCommand *)this->GUICallbackCommand );
    this->ScreenGrabButton->AddObserver (vtkKWPushButton::InvokedEvent, (vtkCommand *)this->GUICallbackCommand );
    this->VisibilityButton->GetMenu()->AddObserver (vtkKWMenu::MenuItemInvokedEvent, (vtkCommand *)this->GUICallbackCommand );
    this->SelectSceneSnapshotMenuButton->GetMenu()->AddObserver ( vtkKWMenu::MenuItemInvokedEvent, (vtkCommand *)this->GUICallbackCommand );
    this->SceneSnapshotButton->AddObserver (vtkKWPushButton::InvokedEvent, (vtkCommand *)this->GUICallbackCommand );
    this->SelectSceneSnapshotMenuButton->GetMenu()->AddObserver ( vtkKWMenu::MenuItemInvokedEvent, (vtkCommand *)this->GUICallbackCommand );

    this->PitchButton->AddObserver (vtkKWPushButton::InvokedEvent, (vtkCommand *)this->GUICallbackCommand );
    this->RollButton->AddObserver (vtkKWPushButton::InvokedEvent, (vtkCommand *)this->GUICallbackCommand );
    this->YawButton->AddObserver (vtkKWPushButton::InvokedEvent, (vtkCommand *)this->GUICallbackCommand );
    this->ZoomInButton->AddObserver (vtkKWPushButton::InvokedEvent, (vtkCommand *)this->GUICallbackCommand );
    this->ZoomOutButton->AddObserver (vtkKWPushButton::InvokedEvent, (vtkCommand *)this->GUICallbackCommand );

}




//---------------------------------------------------------------------------
void vtkSlicerViewControlGUI::AddMainViewerEventObservers()
{
  
  if ( this->GetApplicationGUI() != NULL )
    {
    if ( this->MainViewerEvents != NULL )
      {
      this->MainViewerEvents->AddObserver ( vtkCommand::EndInteractionEvent, this->GUICallbackCommand );
      }
    }
}



//---------------------------------------------------------------------------
void vtkSlicerViewControlGUI::RemoveMainViewerEventObservers()
{
  if ( this->GetApplicationGUI() != NULL )
    {
    if ( this->MainViewerEvents != NULL )
      {
      this->MainViewerEvents->RemoveObservers ( vtkCommand::EndInteractionEvent, this->GUICallbackCommand );
      }
    }
}



//---------------------------------------------------------------------------
void vtkSlicerViewControlGUI::UpdateSliceGUIInteractorStyles ( )
{
  // get all views from the scene
  // and observe active view.
  if (this->SceneClosing)
    {
    return;
    }

  // Find current SliceGUIs; if there are none, do nothing.
  if ( ( this->GetApplicationGUI()->GetMainSliceGUI("Red") == NULL ) ||
       ( this->GetApplicationGUI()->GetMainSliceGUI("Yellow") == NULL ) ||
       ( this->GetApplicationGUI()->GetMainSliceGUI("Green") == NULL ))
    {
    return;
    }

  // If the interactor and these references are out of sync...
  if ( ( this->GetApplicationGUI()->GetMainSliceGUI("Red")->GetSliceViewer()->
         GetRenderWidget()->GetRenderWindowInteractor()->GetInteractorStyle() != this->RedSliceEvents ) ||
       ( this->GetApplicationGUI()->GetMainSliceGUI("Yellow")->GetSliceViewer()->
         GetRenderWidget()->GetRenderWindowInteractor()->GetInteractorStyle() != this->YellowSliceEvents ) ||
       ( this->GetApplicationGUI()->GetMainSliceGUI("Green")->GetSliceViewer()->
         GetRenderWidget()->GetRenderWindowInteractor()->GetInteractorStyle() != this->GreenSliceEvents ) )
    {
    this->RemoveSliceEventObservers();
    this->SetRedSliceEvents(NULL );
    this->SetYellowSliceEvents(NULL );
    this->SetGreenSliceEvents(NULL );

    if ( this->GetApplicationGUI()->GetMainSliceGUI ("Red") != NULL &&
         this->GetApplicationGUI()->GetMainSliceGUI ("Red")->GetSliceViewer() != NULL &&
         this->GetApplicationGUI()->GetMainSliceGUI ("Red")->GetSliceViewer()->GetRenderWidget() != NULL &&
         this->GetApplicationGUI()->GetMainSliceGUI ("Red")->GetSliceViewer()->GetRenderWidget()->GetRenderWindowInteractor() != NULL)
      {
      this->SetRedSliceEvents( vtkSlicerInteractorStyle::SafeDownCast(
                                                                      this->GetApplicationGUI()->
                                                                      GetMainSliceGUI("Red")->
                                                                      GetSliceViewer()->
                                                                      GetRenderWidget()->
                                                                      GetRenderWindowInteractor()->
                                                                      GetInteractorStyle() ));
      }
    if ( this->GetApplicationGUI()->GetMainSliceGUI ("Yellow") != NULL &&
         this->GetApplicationGUI()->GetMainSliceGUI ("Yellow")->GetSliceViewer() != NULL &&
         this->GetApplicationGUI()->GetMainSliceGUI ("Yellow")->GetSliceViewer()->GetRenderWidget() != NULL &&
         this->GetApplicationGUI()->GetMainSliceGUI ("Yellow")->GetSliceViewer()->GetRenderWidget()->GetRenderWindowInteractor() != NULL)
      {
      this->SetYellowSliceEvents( vtkSlicerInteractorStyle::SafeDownCast(
                                                                         this->GetApplicationGUI()->
                                                                         GetMainSliceGUI("Yellow")->
                                                                         GetSliceViewer()->
                                                                         GetRenderWidget()->
                                                                         GetRenderWindowInteractor()->
                                                                         GetInteractorStyle() ));
      }
    if ( this->GetApplicationGUI()->GetMainSliceGUI ("Green") != NULL &&
         this->GetApplicationGUI()->GetMainSliceGUI ("Green")->GetSliceViewer() != NULL &&
         this->GetApplicationGUI()->GetMainSliceGUI ("Green")->GetSliceViewer()->GetRenderWidget() != NULL &&
         this->GetApplicationGUI()->GetMainSliceGUI ("Green")->GetSliceViewer()->GetRenderWidget()->GetRenderWindowInteractor() != NULL)
      {
      this->SetGreenSliceEvents( vtkSlicerInteractorStyle::SafeDownCast(
                                                                        this->GetApplicationGUI()->
                                                                        GetMainSliceGUI("Green")->
                                                                        GetSliceViewer()->
                                                                        GetRenderWidget()->
                                                                        GetRenderWindowInteractor()->
                                                                        GetInteractorStyle() ));
      }
    this->AddSliceEventObservers();
    }
}

 
//---------------------------------------------------------------------------
void vtkSlicerViewControlGUI::UpdateBehaviorFromMRML()
{
  vtkMRMLViewNode *vn = this->ViewNode;
  if ( vn == NULL )
    {
    vtkDebugMacro ( "Got NULL ViewNode. Can't update navigation render's appearance." );
    return;
    }

  if ( this->SpinButton == NULL || !this->SpinButton->IsCreated() )
    {
    vtkErrorMacro ( "No Spin Button Found. Not updating spin/rock behavior." );
    return;
    }
  if ( this->RockButton == NULL || !this->RockButton->IsCreated() )
    {
    vtkErrorMacro ( "No Rock Button Found. Not updating spin/rock behavior." );
    return;
    }
  
  // make sure GUI setting tracks MRML.
  if (( vn->GetAnimationMode() == vtkMRMLViewNode::Off ) &&
      ( this->SpinButton->GetSelectedState() == 1) )
    {
    this->SpinButton->Deselect();
    }
  else if (( vn->GetAnimationMode() == vtkMRMLViewNode::Off ) &&
           ( this->RockButton->GetSelectedState() == 1 ) )
    {
    this->RockButton->Deselect();
    }
  else if (( vn->GetAnimationMode() == vtkMRMLViewNode::Spin ) &&
           ( this->RockButton->GetSelectedState() == 1) )
    {
    this->SpinButton->Select();
    this->RockButton->Deselect();
    //   this->MainViewSpin();
    }
  else if (( vn->GetAnimationMode() == vtkMRMLViewNode::Rock ) &&
           ( this->SpinButton->GetSelectedState() == 1) )
    {
    this->SpinButton->Deselect();
    this->RockButton->Select();
    //   this->SetRockCount ( vn->GetRockCount ( ) );
    //   this->MainViewRock();
    }
}

//---------------------------------------------------------------------------
void vtkSlicerViewControlGUI::UpdateAppearanceFromMRML()
{
  vtkMRMLViewNode *vn = this->ViewNode;
  if ( vn == NULL )
    {
    vtkDebugMacro ( "Got NULL ViewNode. Can't update navigation render's appearance." );
    return;
    }

  if ( this->VisibilityButton == NULL )
    {
    // no need to update its menu...
    return;
    }
  vtkKWMenu *m = this->VisibilityButton->GetMenu();
  
 // thing visibility
 if ( vn->GetFiducialsVisible() != m->GetItemSelectedState("Fiducial points"))
   {
   m->SetItemSelectedState ( "Fiducial points", vn->GetFiducialsVisible() );
   }
 if ( vn->GetFiducialLabelsVisible() !=m->GetItemSelectedState("Fiducial labels"))
   {
   m->SetItemSelectedState ( "Fiducial labels", vn->GetFiducialLabelsVisible() );
   }
 if ( vn->GetBoxVisible() !=m->GetItemSelectedState ("3D cube"))
   {
   m->SetItemSelectedState ( "3D cube", vn->GetBoxVisible() );
   }
 if ( vn->GetAxisLabelsVisible() != m->GetItemSelectedState ("3D axis labels"))
   {
   m->SetItemSelectedState ("3D axis labels", vn->GetAxisLabelsVisible() );
   }
 
 // background color
  vtkSlicerApplication *app = vtkSlicerApplication::SafeDownCast( this->GetApplication() );
  if ( !app )
    {
    vtkErrorMacro ("Got NULL Application" );
    return;
    }

  vtkSlicerTheme *theme = app->GetSlicerTheme();
  if ( !theme )
    {
    return;
    }
  vtkSlicerColor *colors = theme->GetSlicerColors();
  if ( !colors )
    {
    return;
    }

  this->MainViewBackgroundColor ( vn->GetBackgroundColor() );
  //
  // Annoyingly, a direct comparison of values is failing
  // out in the wee decimal places. Comparing to the
  // first is sufficient... Is there a 
  //
  double *vnColor = NULL;
  double *testColor = NULL;
  vnColor = vn->GetBackgroundColor();
  
  testColor = colors->ViewerBlue;
  if ( testColor && vnColor )
    {
    if ( (floor ((vnColor[0]) * 10.0f)) == (floor (testColor[0]*10.0f))   &&
         (floor ((vnColor[1]) * 10.0f)) == (floor (testColor[1]*10.0f))   &&
         (floor ((vnColor[2]) * 10.0f)) == (floor (testColor[2]*10.0f)) )
      {
      if (m->GetItemSelectedState ("Light blue background") != 1 )
        {
        m->SetItemSelectedState ( "Light blue background", 1 );
        }
      }
    }

  testColor = colors->Black;
  if ( testColor && vnColor )
    {
    if ( (floor ((vnColor[0]) * 10.0f)) == (floor (testColor[0]*10.0f))   &&
         (floor ((vnColor[1]) * 10.0f)) == (floor (testColor[1]*10.0f))   &&
         (floor ((vnColor[2]) * 10.0f)) == (floor (testColor[2]*10.0f)) )
      {
      if (m->GetItemSelectedState ("Black background") != 1 )
        {
        m->SetItemSelectedState ( "Black background", 1 );
        }
      }
    }
  
  testColor = colors->White;
  if ( testColor && vnColor )
    {
    if ( (floor ((vnColor[0]) * 10.0f)) == (floor (testColor[0]*10.0f))   &&
         (floor ((vnColor[1]) * 10.0f)) == (floor (testColor[1]*10.0f))   &&
         (floor ((vnColor[2]) * 10.0f)) == (floor (testColor[2]*10.0f)) )
      {
      if (m->GetItemSelectedState ("White background") != 1 )
        {
        m->SetItemSelectedState ( "White background", 1 );
        }
      }
    }
}


//---------------------------------------------------------------------------
void vtkSlicerViewControlGUI::UpdateNavigationViewAppearanceFromMRML()
{
  if ( this->ApplicationGUI == NULL )
    {
    vtkDebugMacro ( "Got NULL application GUI." );
    return;
    }

  vtkSlicerViewerWidget* viewer_widget = this->GetApplicationGUI()->GetActiveViewerWidget();
  if (!viewer_widget)
    {
    vtkDebugMacro ( "Got NULL viewer widget" );
    return;
    }
  
  vtkMRMLViewNode *vnode = this->GetActiveView();
  if (vnode == NULL)
    {
    vtkDebugMacro  ( "Got NULL ViewNode." );
    return;
    }

  // set background color
  this->GetNavigationWidget()->SetRendererBackgroundColor (    vnode->GetBackgroundColor() );
}


//---------------------------------------------------------------------------
void vtkSlicerViewControlGUI::UpdateFromMRML()
{
  this->UpdateSlicesFromMRML();
  this->UpdateSceneSnapshotsFromMRML();
  this->RequestNavigationRender ( );
}

//---------------------------------------------------------------------------
void vtkSlicerViewControlGUI::UpdateSceneSnapshotsFromMRML()
{
  
  if (this->MRMLScene == NULL)
    {
    vtkErrorMacro ( "ViewControlGUI: Got NULL MRMLScene.");
    return;
    }

  vtkSlicerApplicationGUI *appGUI = vtkSlicerApplicationGUI::SafeDownCast( this->GetApplicationGUI ( ));
  
  //---- update the SelectSceneSnapshotMenu...
  if (this->GetMRMLScene()!= NULL  && appGUI != NULL)
    {

    const char *name;
    int item;
    const char *imageName;

    //--- count number of vtkMRMLSceneSnapshotNodes in the scene.
    //--- if there are none, then make the GUI current
    int num = this->GetMRMLScene()->GetNumberOfNodesByClass ( "vtkMRMLSceneSnapshotNode");
    //--- refresh the menu.
    this->GetSelectSceneSnapshotMenuButton()->GetMenu()->DeleteAllItems();

    for (int i = 0; i < num; i++ )
      {
      name = this->GetMRMLScene()->GetNthNodeByClass ( i, "vtkMRMLSceneSnapshotNode")->GetName();
      vtkKWMenu *m1 = vtkKWMenu::New();
      m1->SetParent ( this->GetSelectSceneSnapshotMenuButton()->GetMenu());
      m1->Create();
      this->GetSelectSceneSnapshotMenuButton()->GetMenu()->AddCascade (name, m1);

      std::string cmd;
      cmd = "RestoreSceneSnapshot";
      cmd += " \"";
      cmd += name;
      cmd += "\"";
      item = m1->AddCommand ( "restore", this, cmd.c_str() );
      imageName = "RestoreImage";
      vtkKWTkUtilities::UpdatePhotoFromIcon ( this->GetApplication(), imageName,  appGUI->GetSlicerFoundationIcons()->GetSlicerGoIcon ( ));
      m1->SetItemImage ( item, imageName);
      m1->SetItemCompoundModeToLeft ( item );
          
      cmd = "DeleteSceneSnapshot";
      cmd += " \"";
      cmd += name;
      cmd += "\"";
      item = m1->AddCommand ( "delete", this, cmd.c_str() );
      imageName = "DeleteImage";
      vtkKWTkUtilities::UpdatePhotoFromIcon ( this->GetApplication(), imageName,  appGUI->GetSlicerFoundationIcons()->GetSlicerDeleteIcon ( ));
      m1->SetItemImage ( item, imageName);
      m1->SetItemCompoundModeToLeft ( item );

      m1->AddSeparator ();
      m1->AddCommand ( "close");
      m1->Delete();
      }
    }
}


//---------------------------------------------------------------------------
void vtkSlicerViewControlGUI::RestoreSceneSnapshot( const char *nom )
{

  if (this->MRMLScene == NULL)
    {
    vtkErrorMacro ( "ViewControlGUI: Got NULL MRMLScene.");
    return;
    }
  
  vtkCollection *col = this->MRMLScene->GetNodesByName ( nom );
  if ( col != NULL )
    {
    col->InitTraversal();

    //--- get the first one. Name checking should make it unique...
    vtkMRMLSceneSnapshotNode *node =  vtkMRMLSceneSnapshotNode::SafeDownCast (col->GetNextItemAsObject() );
    if (node)
      {
      this->MRMLScene->SaveStateForUndo();
      node->RestoreScene();
      }
    col->Delete();
    }
}


//---------------------------------------------------------------------------
void vtkSlicerViewControlGUI::DeleteSceneSnapshot(const char *nom )
{
  if (this->MRMLScene == NULL)
    {
    vtkErrorMacro ( "ViewControlGUI: Got NULL MRMLScene.");
    return;
    }

  vtkCollection *col = this->MRMLScene->GetNodesByName ( nom );
  if ( col != NULL )
    {
    col->InitTraversal();

    //--- get the first one. Name checking should make it unique...
    vtkMRMLSceneSnapshotNode *node =  vtkMRMLSceneSnapshotNode::SafeDownCast (col->GetNextItemAsObject() );
    if ( node)
      {
      this->MRMLScene->SaveStateForUndo();
      this->MRMLScene->RemoveNode(node);
      this->UpdateSceneSnapshotsFromMRML();
      }
    col->Delete();
    }
}



//---------------------------------------------------------------------------
void vtkSlicerViewControlGUI::SetViewNode(vtkMRMLViewNode *node)
{
  if (this->SceneClosing || this->ViewNode == node)
    {
    return;
    }

  vtkSetAndObserveMRMLNodeMacro(this->ViewNode, NULL);

  if (node)
    {
    vtkIntArray  *events = vtkIntArray::New();
    events->InsertNextValue( vtkMRMLViewNode::AnimationModeEvent);
    events->InsertNextValue( vtkMRMLViewNode::RenderModeEvent); 
    events->InsertNextValue( vtkMRMLViewNode::StereoModeEvent);
    events->InsertNextValue( vtkMRMLViewNode::VisibilityEvent);
    events->InsertNextValue( vtkMRMLViewNode::BackgroundColorEvent);    
    events->InsertNextValue ( vtkMRMLViewNode::ActiveModifiedEvent );
    vtkSetAndObserveMRMLNodeEventsMacro(this->ViewNode, node, events);
    events->Delete();
    events = NULL;
    }
}

//---------------------------------------------------------------------------
void vtkSlicerViewControlGUI::UpdateViewFromMRML()
{
  // For backward compat, keep UpdateViewFromMRML; 
  // GetActiveView essentially triggers the same code, i.e. update ViewNode
  this->GetActiveView(); 
}

//---------------------------------------------------------------------------
void vtkSlicerViewControlGUI::UpdateSlicesFromMRML()
{
  if (this->MRMLScene == NULL)
    {
    vtkErrorMacro ( "ViewControlGUI: Got NULL MRMLScene.");
    return;
    }

  if (this->SceneClosing)
    {
    return;
    }

  // update Slice nodes
  vtkMRMLSliceNode *node= NULL;
  vtkMRMLSliceNode *nodeRed= NULL;
  vtkMRMLSliceNode *nodeGreen= NULL;
  vtkMRMLSliceNode *nodeYellow= NULL;
  int nnodes = this->MRMLScene->GetNumberOfNodesByClass("vtkMRMLSliceNode");
  for (int n=0; n<nnodes; n++)
    {
    node = vtkMRMLSliceNode::SafeDownCast (
          this->MRMLScene->GetNthNodeByClass(n, "vtkMRMLSliceNode"));
    if (!strcmp(node->GetLayoutName(), "Red"))
      {
      nodeRed = node;
      }
    else if (!strcmp(node->GetLayoutName(), "Green"))
      {
      nodeGreen = node;
      }
    else if (!strcmp(node->GetLayoutName(), "Yellow"))
      {
      nodeYellow = node;
      }
    node = NULL;
    }

  // set and observe
  if (nodeRed != this->RedSliceNode)
    {
    vtkSetAndObserveMRMLNodeMacro(this->RedSliceNode, nodeRed);
    }
  if (nodeGreen != this->GreenSliceNode)
   {
   vtkSetAndObserveMRMLNodeMacro(this->GreenSliceNode, nodeGreen);
   }
  if (nodeYellow != this->YellowSliceNode)
   {
   vtkSetAndObserveMRMLNodeMacro(this->YellowSliceNode, nodeYellow);
   }

  // tidy up.
  nodeRed = NULL;
  nodeGreen = NULL;
  nodeYellow = NULL;
}



//---------------------------------------------------------------------------
void vtkSlicerViewControlGUI::RequestNavigationRender()
{

#ifndef NAVZOOMWIDGET_DEBUG
  if (this->GetNavigationRenderPending())
    {
    return;
    }

  if (!this->EnableDisableNavButton->GetSelectedState())
  {
          this->NavigationWidget->RemoveAllViewProps();
          this->NavigationWidget->Render();
          return;
  }

  if ( this->GetTclName() )
    {
    this->SetNavigationRenderPending(1);
    this->Script("after idle \"%s NavigationRender\"", this->GetTclName());
    }

#endif
}

//---------------------------------------------------------------------------
void vtkSlicerViewControlGUI::RequestZoomRender()
{
#ifndef NAVZOOMWIDGET_DEBUG
  if (this->GetZoomRenderPending())
    {
    return;
    }

  if ( this->GetTclName() )
    {
    this->SetZoomRenderPending(1);
    this->Script("after idle \"%s ZoomRender\"", this->GetTclName());
    }
#endif
}




//---------------------------------------------------------------------------
void vtkSlicerViewControlGUI::ZoomRender()
{
#ifndef NAVZOOMWIDGET_DEBUG
  if ( this->SliceMagnifier->GetInput() != NULL )
    {
    this->ZoomWidget->Render();
    }
  this->SetZoomRenderPending(0);
#endif
}


//---------------------------------------------------------------------------
void vtkSlicerViewControlGUI::NavigationRender()
{

#ifndef  NAVZOOMWIDGET_DEBUG
  this->UpdateNavigationWidgetViewActors ( );
  this->ConfigureNavigationWidgetRender ( );
  this->NavigationWidget->Render();
  this->SetNavigationRenderPending(0);
#endif
}



//---------------------------------------------------------------------------
void vtkSlicerViewControlGUI::ResetNavigationCamera()
{
  double center[3];
  double distance;
  double vn[3], *vup;
  double bounds[6];
  double viewAngle = 20.0;
  
  vtkRenderer *ren = this->NavigationWidget->GetRenderer();
  vtkCamera *cam = ren->IsActiveCameraCreated() ? ren->GetActiveCamera() : NULL;
  
  ren->ComputeVisiblePropBounds( bounds );
  if (!vtkMath::AreBoundsInitialized(bounds))
    {
    vtkDebugMacro( << "Cannot reset camera!" );
    }
  else
    {
    ren->InvokeEvent(vtkCommand::ResetCameraEvent, ren);

    if ( cam != NULL )
      {
      cam->GetViewPlaneNormal(vn);
      }
    else
      {
      vtkErrorMacro(<< "Trying to reset non-existant camera");
      return;
      }

    center[0] = (bounds[0] + bounds[1])/2.0;
    center[1] = (bounds[2] + bounds[3])/2.0;
    center[2] = (bounds[4] + bounds[5])/2.0;

    double w1 = bounds[1] - bounds[0];
    double w2 = bounds[3] - bounds[2];
    double w3 = bounds[5] - bounds[4];
    w1 *= w1;
    w2 *= w2;
    w3 *= w3;
    double radius = w1 + w2 + w3;

    // If we have just a single point, pick a radius of 1.0
    radius = (radius==0)?(1.0):(radius);

    // compute the radius of the enclosing sphere
    radius = sqrt(radius)*0.5;

    // default so that the bounding sphere fits within the view fustrum
    // compute the distance from the intersection of the view frustum with the
    // bounding sphere. Basically in 2D draw a circle representing the bounding
    // sphere in 2D then draw a horizontal line going out from the center of
    // the circle. That is the camera view. Then draw a line from the camera
    // position to the point where it intersects the circle. (it will be tangent
    // to the circle at this point, this is important, only go to the tangent
    // point, do not draw all the way to the view plane). Then draw the radius
    // from the tangent point to the center of the circle. You will note that
    // this forms a right triangle with one side being the radius, another being
    // the target distance for the camera, then just find the target dist using
    // a sin.
    distance = radius/sin(viewAngle*vtkMath::Pi()/360.0);

    // check view-up vector against view plane normal
    vup = cam->GetViewUp();
    if ( fabs(vtkMath::Dot(vup,vn)) > 0.999 )
      {
      vtkWarningMacro(<<"Resetting view-up since view plane normal is parallel");
      cam->SetViewUp(-vup[2], vup[0], vup[1]);
      }

    // update the camera
    cam->SetFocalPoint(center[0],center[1],center[2]);
    cam->SetPosition(center[0]+distance*vn[0],
                     center[1]+distance*vn[1],
                     center[2]+distance*vn[2]);

    ren->ResetCameraClippingRange( bounds );

    // setup default parallel scale
    cam->SetParallelScale(radius);
    }
}


//---------------------------------------------------------------------------
const char* vtkSlicerViewControlGUI::CreateSceneSnapshotNode( const char *nodeName)
{
  vtkMRMLSceneSnapshotNode *node = NULL;
  const char *id;

  if (this->MRMLScene == NULL)
    {
    vtkErrorMacro ( "ViewControlGUI: Got NULL MRMLScene.");
    return (NULL);
    }

  node = vtkMRMLSceneSnapshotNode::SafeDownCast (this->MRMLScene->CreateNodeByClass( "vtkMRMLSceneSnapshotNode" ));
  if (node == NULL)
    {
    return (NULL);
    }

  // Invoke a new node event giving an observer an opportunity to
  // configure the node
//  this->InvokeEvent(vtkSlicerNodeSelectorWidget::NewNodeEvent, node);

  node->SetScene(this->MRMLScene);

  std::stringstream ss;
  const char *name;
  if (nodeName == NULL || !strcmp(nodeName,"") )
    {
    name = this->MRMLScene->GetTagByClassName("vtkMRMLSceneSnapshotNode");
    }
  else
    {
    name = nodeName;
    }

//  ss << this->MRMLScene->GetUniqueNameByString(name);
  ss << nodeName;
  node->SetName(ss.str().c_str());
  vtkDebugMacro("\tset the name to " << node->GetName() << endl);

  // the ID is set in the call to AddNode
  this->MRMLScene->AddNode(node);
  id = node->GetID();
  node->Delete();
  return ( id );

}


//---------------------------------------------------------------------------
int vtkSlicerViewControlGUI::InvokeNameDialog( const char *msg, const char *name)
{
  //--- now name the node...
  if ( this->NameDialog )
    {
    vtkKWEntryWithLabel *entry = this->NameDialog->GetEntry();
    this->NameDialog->SetText ( msg );
    if (entry->GetWidget()->GetValue() == NULL || !strcmp(entry->GetWidget()->GetValue(),"")) 
      {
      entry->GetWidget()->SetValue(name);
      }
    int result = this->NameDialog->Invoke();
    if ( !result )
      {
      return 0;
      }
    return 1;
    }
  else
    {
    return 0;
    }
}



//---------------------------------------------------------------------------
void vtkSlicerViewControlGUI::ProcessGUIEvents ( vtkObject *caller,
                                                 unsigned long event, void * vtkNotUsed(callData) )
{

  // Right now this class contains state variables that will be moved
  // to a vtkMRMLViewNode in the next iteration.

  vtkSlicerApplication *app = vtkSlicerApplication::SafeDownCast( this->GetApplication() );
  if ( !app )
    {
    vtkErrorMacro ("ProcessGUIEvents: Got NULL Application" );
    return;
    }
  vtkSlicerApplicationGUI *appGUI = vtkSlicerApplicationGUI::SafeDownCast( this->GetApplicationGUI ( ));
  if (!appGUI )
    {
    vtkErrorMacro ("ProcessGUIEvents: Got NULL Application GUI" );
    return;
    }
  vtkSlicerWindow *win = appGUI->GetMainSlicerWindow();
  if ( !win )
    {
    vtkErrorMacro ("ProcessGUIEvents: Got NULL Slicer Window" );
    return;
    }

  //Check for abort during rendering of navigation widget
  if(caller==this->NavigationWidget->GetRenderWindow()&&event==vtkCommand::AbortCheckEvent)
    {
    this->CheckAbort();
    return;
    }

  //--- SafeDownCast all possibilities.
  vtkKWCheckButton *b = vtkKWCheckButton::SafeDownCast ( caller );
  vtkKWPushButton *p = vtkKWPushButton::SafeDownCast ( caller );
  vtkKWMenu *m = vtkKWMenu::SafeDownCast ( caller );
  vtkKWLoadSaveDialog *d = vtkKWLoadSaveDialog::SafeDownCast ( caller );
  vtkSlicerInteractorStyle *istyle = vtkSlicerInteractorStyle::SafeDownCast ( caller );
  vtkSlicerViewerInteractorStyle *vstyle = vtkSlicerViewerInteractorStyle::SafeDownCast ( caller );

  // if user has requested a larger or smaller font,
  // execute this.
  if (0)
    {
    this->ReconfigureGUIFonts();
    }

  // has interaction occured in the slice viewers?
  if ( istyle == this->RedSliceEvents || istyle == this->YellowSliceEvents || istyle == this->GreenSliceEvents)
    {
#ifndef NAVZOOMWIDGET_DEBUG
    this->SliceViewMagnify( event, istyle );
#endif
    }
#ifndef NAVZOOMWIDGET_DEBUG
  // has interaction occured in the main viewer?
  if (vstyle == this->MainViewerEvents )
    {
    this->RequestNavigationRender();
    }
#endif
  
  //--- check buttons w/o view node
  if ( b != NULL )
    {
    if ( b == this->ScreenGrabOverwriteButton && event == vtkKWCheckButton::SelectedStateChangedEvent )
      {
      this->ScreenGrabOverwrite = this->ScreenGrabOverwriteButton->GetSelectedState();
      return;
      }
    }
  
  //--- screen grabs w/o view node
  if ( p != NULL )
    {
    if ( p == this->ScreenGrabButton && event == vtkKWPushButton::InvokedEvent )  
      {
      this->RaiseScreenGrabOptionsWindow();
      }
    else if (p == this->ScreenGrabCaptureButton && event == vtkKWPushButton::InvokedEvent)
      {
      //--- check for bugs
      //--- if bad values for scale or version number were entered, just return.
      //--- callbacks for each entry will take care of the error message.

      if ( this->ScreenGrabNameEntry )
        {
        // check
        if ( (this->ScreenGrabNameEntry->GetValue() != NULL)  &&
             (strcmp(this->ScreenGrabNameEntry->GetValue(), this->ScreenGrabName)) )
          {
          this->SetScreenGrabName ( this->ScreenGrabNameEntry->GetValue() );
          }
        else if ( this->ScreenGrabNameEntry->GetValue() == NULL )
          {
          // dialog
          vtkKWMessageDialog *dialog = vtkKWMessageDialog::New();
          dialog->SetParent ( win );
          dialog->SetStyleToMessage();
          std::string msg = "Please enter a valid name for the screen capture.";
          dialog->SetText(msg.c_str());
          dialog->Create ( );
          dialog->Invoke();
          dialog->Delete();
          return;
          }
        }
      if ( this->ScreenGrabNumberEntry )
        {
        int version = this->ScreenGrabNumberEntry->GetValueAsInt();
        if ( version >= 0  )
          {
          this->ScreenGrabNumber = version;
          //--- if a user entered B, the  version will be '0'. so fix the entry.
          this->ScreenGrabNumberEntry->SetValueAsInt ( this->ScreenGrabNumber );
          }
        else 
          {
          vtkKWMessageDialog *dialog = vtkKWMessageDialog::New();
          dialog->SetParent (  win );
          dialog->SetStyleToMessage();
          std::string msg = "Please enter a non-negative (integer) image version number.";
          dialog->SetText(msg.c_str());
          dialog->Create ( );
          dialog->Invoke();
          dialog->Delete();
          return;
          }
        }

      if ( this->ScreenGrabMagnificationEntry  )
        {
        int mag  = this->ScreenGrabMagnificationEntry->GetValueAsInt();

        if ( ( mag <= 5 ) && ( mag > 0 ))
          {
          this->ScreenGrabMagnification = this->ScreenGrabMagnificationEntry->GetValueAsInt ( );
          //--- make sure user sees what we see after converting to int.
          this->ScreenGrabMagnificationEntry->SetValueAsInt ( this->ScreenGrabMagnification );
          }
        else 
          {
          vtkKWMessageDialog *dialog = vtkKWMessageDialog::New();
          dialog->SetParent (  win );
          dialog->SetStyleToMessage();
          std::string msg = "Please enter an integer image scale value between 1 and 5.";
          dialog->SetText(msg.c_str());
          dialog->Create ( );
          dialog->Invoke();
          dialog->Delete();
          return;
          }
        }
      //--- try first setting user's selection. if nothing there, choose the last path. if nothing there, mark for error.
      std::string dirname;
      if ( this->GetScreenGrabDirectory() != NULL )
        {
        dirname = this->GetScreenGrabDirectory();
        }
      else
        {
        if ( this->ScreenGrabDialogButton->GetLoadSaveDialog()->GetLastPath() != NULL )
          {
          dirname = this->ScreenGrabDialogButton->GetLoadSaveDialog()->GetLastPath();
          }
        else
          {
          dirname = "none";
          }
        }
      if ( !(strcmp(dirname.c_str(), "none") ))
        {
        // provide a warning 
        vtkKWMessageDialog *dialog = vtkKWMessageDialog::New();
        dialog->SetParent (  win );
        dialog->SetStyleToMessage();
        std::string msg = "Please specify a valid directory.";
        dialog->SetText(msg.c_str());
        dialog->Create ( );
        dialog->Invoke();
        dialog->Delete();
        return;
        }
      else if (this->ScreenGrabName == NULL )
        {
        // provide a warning 
        vtkKWMessageDialog *dialog = vtkKWMessageDialog::New();
        dialog->SetParent (  win );
        dialog->SetStyleToMessage();
        std::string msg = "Please specify a valid image name.";
        dialog->SetText(msg.c_str());
        dialog->Create ( );
        dialog->Invoke();
        dialog->Delete();
        return;
        }
      else if  (this->ScreenGrabFormat.c_str() == NULL )
        {
        // provide a warning 
        vtkKWMessageDialog *dialog = vtkKWMessageDialog::New();
        dialog->SetParent (  win );
        dialog->SetStyleToMessage();
        std::string msg = "Please specify a valid format.";
        dialog->SetText(msg.c_str());
        dialog->Create ( );
        dialog->Invoke();
        dialog->Delete();
        return;
        }
      else
        {
        //--- assemble filename with path.
        int capture = 1;
        std::vector<std::string> pathComponents;
        vtksys::SystemTools::SplitPath ( dirname.c_str(), pathComponents );
        std::stringstream ss;
        ss << this->ScreenGrabName << "_" << this->ScreenGrabNumber << this->ScreenGrabFormat;
        std::string s = ss.str();
        pathComponents.push_back ( s );
        std::string filename = vtksys::SystemTools::JoinPath ( pathComponents );
        //--- make sure it's a unix-style path.
        std::string upath = vtksys::SystemTools::ConvertToUnixOutputPath ( filename.c_str() );


        //--- see if file already exists
        if ( vtksys::SystemTools::FileExists ( upath.c_str()) ||
             vtksys::SystemTools::FileExists ( filename.c_str()) )
          {
          //--- if overwrite not selected and file exists, give a warning & option to overwrite.
          if (!(this->ScreenGrabOverwrite) )
            {
            vtkKWMessageDialog *dialog = vtkKWMessageDialog::New();
            dialog->SetParent (  win );
            dialog->SetStyleToYesNo();
            std::string msg = "File already exists. Overwrite?";
            dialog->SetText(msg.c_str());
            dialog->Create ( );
            capture = dialog->Invoke();
            dialog->Delete();
            }
          }
        //--- if good for capture, set status text and grab the screenshot.
        if ( capture )
          {
          win->SetStatusText ( "Capturing Screenshot...." );
          app->Script ( "update idletasks" );
          this->Script ( "SlicerSaveLargeImage %s %d", upath.c_str(), this->GetScreenGrabMagnification() );
          this->ScreenGrabNumberEntry->SetValueAsInt ( this->ScreenGrabNumber+1 );
          win->SetStatusText ( "" );
          app->Script ( "update idletasks" );
          }
        }
      return;
      }
    }
  

  if ( m != NULL && event == vtkKWMenu::MenuItemInvokedEvent )
    {
    if ( this->ScreenGrabFormatMenuButton != NULL && m == this->ScreenGrabFormatMenuButton->GetMenu() )
      {
      this->SetScreenGrabFormat ( this->ScreenGrabFormatMenuButton->GetValue() );
      return;
      }
    }

  
  if ( d != NULL && event == vtkKWTopLevel::WithdrawEvent )
    {
    if ( this->ScreenGrabDialogButton && d == this->ScreenGrabDialogButton->GetLoadSaveDialog() )
      {
      std::string dirname;
      if ( this->ScreenGrabDialogButton->GetLoadSaveDialog()->GetFileName() != NULL )
        {
        dirname = this->ScreenGrabDialogButton->GetLoadSaveDialog()->GetFileName();
        if ( vtksys::SystemTools::FileIsDirectory ( dirname.c_str() ) )
          {
          this->SetScreenGrabDirectory ( dirname.c_str() );
          }
        }
      else
        {
        if ( this->ScreenGrabDialogButton->GetLoadSaveDialog()->GetLastPath() != NULL )
          {
          dirname = this->ScreenGrabDialogButton->GetLoadSaveDialog()->GetLastPath();
          if ( vtksys::SystemTools::FileIsDirectory ( dirname.c_str() ) )
            {
            this->SetScreenGrabDirectory ( dirname.c_str() );
            }
          }
        else
          {
          this->SetScreenGrabDirectory ( NULL );
          }
        }
      return;
      }
    }
  
  // Make requested changes to the ViewNode      
  // save state for undo
  if ( (m == this->StereoButton->GetMenu() && event == vtkKWMenu::MenuItemInvokedEvent) ||
       (m == this->VisibilityButton->GetMenu() && event == vtkKWMenu::MenuItemInvokedEvent) ||
       (m == this->StereoButton->GetMenu() && event == vtkKWMenu::MenuItemInvokedEvent) ||
       (m == this->SelectSceneSnapshotMenuButton->GetMenu() && event == vtkKWMenu::MenuItemInvokedEvent) ||
       (m == this->SelectSceneSnapshotMenuButton->GetMenu() && event == vtkKWMenu::MenuItemInvokedEvent) ||
       (p == this->CenterButton && event == vtkKWPushButton::InvokedEvent) ||                      
       (p == this->OrthoButton && event == vtkKWPushButton::InvokedEvent) ||                      
       (p == this->PitchButton && event == vtkKWPushButton::InvokedEvent) ||
       (p == this->RollButton && event == vtkKWPushButton::InvokedEvent) ||
       (p == this->YawButton && event == vtkKWPushButton::InvokedEvent) ||
       (p == this->SceneSnapshotButton && event == vtkKWPushButton::InvokedEvent) ||
       (p == this->ZoomInButton && event == vtkKWPushButton::InvokedEvent) ||
       (p == this->ZoomOutButton && event == vtkKWPushButton::InvokedEvent) ||
       (b == this->SpinButton && event == vtkKWCheckButton::SelectedStateChangedEvent) ||                      
       (b == this->RockButton && event == vtkKWCheckButton::SelectedStateChangedEvent ) )
    {
    if ( m == this->SelectSceneSnapshotMenuButton->GetMenu() && event == vtkKWMenu::MenuItemInvokedEvent )
      {
      }
    vtkMRMLViewNode *vn = this->GetActiveView();
    if ( vn != NULL )
      {
      appGUI->GetMRMLScene()->SaveStateForUndo( vn );
      if ( m == this->StereoButton->GetMenu() && event == vtkKWMenu::MenuItemInvokedEvent )
        {
        if ( !strcmp (this->StereoButton->GetValue(), "No stereo"))
          {
          vn->SetStereoType( vtkMRMLViewNode::NoStereo );
          }
        else if (!strcmp (this->StereoButton->GetValue(), "Red/Blue"))
          {
          vn->SetStereoType( vtkMRMLViewNode::RedBlue );
          }
        else if (!strcmp (this->StereoButton->GetValue(), "Anaglyph"))
          {
          vn->SetStereoType( vtkMRMLViewNode::Anaglyph );
          }
        else if (!strcmp (this->StereoButton->GetValue(), "CrystalEyes"))
          {
          vn->SetStereoType( vtkMRMLViewNode::CrystalEyes );
          }
        else if (!strcmp (this->StereoButton->GetValue(), "Interlaced"))
          {
          vn->SetStereoType( vtkMRMLViewNode::Interlaced );            
          }
        }
      else if ( m == this->VisibilityButton->GetMenu() && event == vtkKWMenu::MenuItemInvokedEvent )
        {
        // Get all menu items
        if ( vn->GetFiducialsVisible() != m->GetItemSelectedState("Fiducial points"))
          {
//                this->SetMRMLFiducialPointVisibility (m->GetItemSelectedState("Fiducial points"));
          }
        if ( vn->GetFiducialLabelsVisible() !=m->GetItemSelectedState("Fiducial labels"))
          {
//                this->SetMRMLFiducialLabelVisibility (m->GetItemSelectedState("Fiducial labels"));
          }
        if ( vn->GetBoxVisible() !=m->GetItemSelectedState ("3D cube"))
          {
          vn->SetBoxVisible ( m->GetItemSelectedState ("3D cube"));
          this->RequestNavigationRender();
          }
        if ( vn->GetAxisLabelsVisible() != m->GetItemSelectedState ("3D axis labels"))
          {
          vn->SetAxisLabelsVisible (m->GetItemSelectedState ("3D axis labels"));
          this->RequestNavigationRender();
          }
        if ( m->GetItemSelectedState ("Light blue background" ) == 1 )
          {
          vn->SetBackgroundColor ( app->GetSlicerTheme()->GetSlicerColors()->ViewerBlue );
          this->RequestNavigationRender();
          }
        else if ( m->GetItemSelectedState ("Black background" ) == 1 )
          {
          vn->SetBackgroundColor ( app->GetSlicerTheme()->GetSlicerColors()->Black );
          this->RequestNavigationRender();
          }
        else if ( m->GetItemSelectedState ("White background" ) == 1 )
          {
          vn->SetBackgroundColor (app->GetSlicerTheme()->GetSlicerColors()->White );
          this->RequestNavigationRender();
          }            
        }
    
      if ( (p == this->CenterButton) && (event == vtkKWPushButton::InvokedEvent ) )
        {
        this->MainViewResetFocalPoint ( );
        this->RequestNavigationRender();
        }
      else if ( (p == this->OrthoButton) && (event == vtkKWPushButton::InvokedEvent ) && vn )
        {
        if ( vn->GetRenderMode() == vtkMRMLViewNode::Orthographic )
          {
          vn->SetRenderMode (vtkMRMLViewNode::Perspective);
          }
        else if ( vn->GetRenderMode() == vtkMRMLViewNode::Perspective )
          {
          vn->SetRenderMode(vtkMRMLViewNode::Orthographic );
          }
        }
      else if ( p == this->PitchButton && event == vtkKWPushButton::InvokedEvent )
        {
        this->MainViewPitch();
        }
      else if ( p == this->RollButton && event == vtkKWPushButton::InvokedEvent )
        {
        this->MainViewRoll();
        }
      else if ( p == this->YawButton && event == vtkKWPushButton::InvokedEvent )
        {
        this->MainViewYaw();
        }
      else if ( p == this->ZoomInButton && event == vtkKWPushButton::InvokedEvent )
        {
        this->MainViewZoomIn();
        }
      else if ( p == this->ZoomOutButton && event == vtkKWPushButton::InvokedEvent )
        {
        this->MainViewZoomOut();
        }
      else if ( p == this->SceneSnapshotButton  && event == vtkKWPushButton::InvokedEvent )
        {
        //--- create a new node...
        const char *id =  this->CreateSceneSnapshotNode ( this->MySnapshotName );
        vtkMRMLSceneSnapshotNode *snapshotNode = vtkMRMLSceneSnapshotNode::SafeDownCast (this->MRMLScene->GetNodeByID( id ));

        if ( snapshotNode == NULL )
          {
          return;
          }
          
        std::stringstream ss;
        int result =this->InvokeNameDialog ("Type a name your snapshot.", snapshotNode->GetName() );
        if (!result) 
          {
          this->MRMLScene->RemoveNode(snapshotNode);
          snapshotNode = NULL;
          }
        else 
          {
          vtkKWEntryWithLabel *entry = this->NameDialog->GetEntry();
          if ( strcmp (entry->GetWidget()->GetValue(), this->MySnapshotName ))
            {
//                this->MySnapshotName = entry->GetWidget()->GetValue();
            }
          ss <<  this->MRMLScene->GetUniqueNameByString(entry->GetWidget()->GetValue());
          snapshotNode->SetName(ss.str().c_str());
          snapshotNode->StoreScene();
          }
        }

      
      //--- turn View Spin and Rocking on and off
      if ( (b == this->SpinButton) && (event == vtkKWCheckButton::SelectedStateChangedEvent) && vn )
        {
        // toggle the Spin 
        if ( vn->GetAnimationMode() != vtkMRMLViewNode::Spin  && this->SpinButton->GetSelectedState() == 1 )
          {
          vn->SetAnimationMode ( vtkMRMLViewNode::Spin );
          }
        else if ( vn->GetAnimationMode() == vtkMRMLViewNode::Spin && this->SpinButton->GetSelectedState() == 0 )
          {
          vn->SetAnimationMode( vtkMRMLViewNode::Off );
          // update the navigation view when spinning stops
          this->RequestNavigationRender();
          }
        }
    
      if ( (b == this->RockButton) && (event == vtkKWCheckButton::SelectedStateChangedEvent) && vn )
        {
        // toggle the Rock 
        if ( vn->GetAnimationMode() != vtkMRMLViewNode::Rock && this->RockButton->GetSelectedState() == 1 )
          {
          vn->SetAnimationMode( vtkMRMLViewNode::Rock );
          }
        else if (vn->GetAnimationMode() == vtkMRMLViewNode::Rock && this->RockButton->GetSelectedState() == 0 )
          {
          vn->SetAnimationMode ( vtkMRMLViewNode::Off );
          // update the navigation view when rocking stops
          this->RequestNavigationRender();
          }
        }
      }
    }
}

//---------------------------------------------------------------------------
void vtkSlicerViewControlGUI::MainViewZoom(double factor)
{

#ifndef NAVZOOMWIDGET_DEBUG
  vtkSlicerApplicationGUI *appGUI = vtkSlicerApplicationGUI::SafeDownCast( this->GetApplicationGUI ( ));
  vtkRenderer *ren = NULL;
  vtkCamera *cam = NULL;

  if ( !appGUI )
    {
    return;
    }
  vtkSlicerViewerWidget* viewer_widget = appGUI->GetActiveViewerWidget();
  ren = viewer_widget ? viewer_widget->GetMainViewer()->GetRenderer() : NULL;    
  if ( !ren )
    {
    return;
    }
  cam = ren->IsActiveCameraCreated() ? ren->GetActiveCamera() : NULL;

  if ( !cam )
    {
    return;
    }

  if (cam->GetParallelProjection())
    {
    cam->SetParallelScale(cam->GetParallelScale() / factor);
    }
  else
    {
    cam->Dolly(factor);
    ren->ResetCameraClippingRange();
    ren->UpdateLightsGeometryToFollowCamera();
    }
  viewer_widget->RequestRender();
  this->RequestNavigationRender();

  cam = NULL;
  ren = NULL;
  appGUI = NULL;
#endif
  
}


//---------------------------------------------------------------------------
void vtkSlicerViewControlGUI::SetMRMLFiducialPointVisibility ( int vtkNotUsed(state) )
{

  // Right now, fiducial visibility is stored in three places.
  // TODO: agree on where to keep it, and whether to override it.
/*
  if ( this->ApplicationGUI  )
    {
    vtkSlicerApplicationGUI *p = vtkSlicerApplicationGUI::SafeDownCast( this->GetApplicationGUI ( ));
    vtkMRMLViewNode *vn = this->GetActiveView();
    if ( vn != NULL )
      {
      p->GetMRMLScene()->SaveStateForUndo( vn );
      // update ViewNode
      vn->SetFiducialsVisible (state );
      }
    }
*/
}


//---------------------------------------------------------------------------
void vtkSlicerViewControlGUI::SetMRMLFiducialLabelVisibility ( int vtkNotUsed(state) )
{

  // Right now, fiducial visibility is stored in three places.
  // TODO: agree on where to keep it, and whether to override it.
/*
  if ( this->ApplicationGUI  )
    {
    vtkSlicerApplicationGUI *p = vtkSlicerApplicationGUI::SafeDownCast( this->GetApplicationGUI ( ));
    vtkMRMLViewNode *vn = this->GetActiveView();
    if ( vn != NULL )
      {
      // update ViewNode
      vn->SetFiducialLabelsVisible (state);
      }
    }
*/
}





//---------------------------------------------------------------------------
void vtkSlicerViewControlGUI::CreateFieldOfViewBoxActor ( )
{
#ifndef NAVZOOMWIDGET_DEBUG
  this->FOVBox = vtkOutlineSource::New();
  this->FOVBoxMapper = vtkPolyDataMapper::New();
  this->FOVBoxMapper->SetInput( this->FOVBox->GetOutput() );
  this->FOVBoxActor = vtkFollower::New();

  this->FOVBoxMapper->Update();
   
  this->FOVBoxActor->SetMapper( this->FOVBoxMapper );
  this->FOVBoxActor->SetPickable(0);
  this->FOVBoxActor->SetDragable(0);
  this->FOVBoxActor->SetVisibility(1);
  this->FOVBoxActor->SetScale(1.0, 1.0, 1.0);
  this->FOVBoxActor->GetProperty()->SetColor( 0.1, 0.45, 0.1 );
  this->FOVBoxActor->GetProperty()->SetLineWidth (2.0);
#endif
}


//---------------------------------------------------------------------------
void vtkSlicerViewControlGUI::InitializeNavigationWidgetCamera ( )
{

#ifndef NAVZOOMWIDGET_DEBUG
  double camPos[3];
  double focalPoint[3];

  if ( this->GetApplicationGUI() != NULL )
    {
    vtkCamera *cam;
    vtkCamera *navcam;
    vtkRenderer *ren;
    vtkSlicerApplicationGUI *p = vtkSlicerApplicationGUI::SafeDownCast( this->GetApplicationGUI ( ));
    vtkSlicerViewerWidget* viewer_widget = p->GetActiveViewerWidget();
    ren = viewer_widget ? viewer_widget->GetMainViewer()->GetRenderer() : NULL;
    if (ren)
      {
      cam = ren->IsActiveCameraCreated() ? ren->GetActiveCamera() : NULL;
      if (cam)
        {
        cam->GetPosition (camPos );
        cam->GetFocalPoint ( focalPoint );
        navcam = this->NavigationWidget->GetRenderer()->IsActiveCameraCreated() ? this->NavigationWidget->GetRenderer()->GetActiveCamera() : NULL;
        if (navcam)
          {
          navcam->SetPosition ( camPos );
          navcam->SetFocalPoint ( focalPoint );
          navcam->ComputeViewPlaneNormal();
          navcam->SetViewUp( cam->GetViewUp() );
          this->FOVBoxActor->SetCamera (navcam);
          this->FOVBox->SetBoxTypeToOriented ( );
          }
        }
      }
    }
#endif
}



//---------------------------------------------------------------------------
void vtkSlicerViewControlGUI::ConfigureNavigationWidgetRender ( )
{
  
#ifndef NAVZOOMWIDGET_DEBUG
  // Scale the FOVBox actor to show the
  // MainViewer's window on the scene.
  if ( this->GetApplicationGUI() != NULL )
    {
        //Add an observer for check abort events
        if(!this->NavigationWidget->GetRenderWindow()->HasObserver(vtkCommand::AbortCheckEvent,(vtkCommand*)this->GUICallbackCommand))
        {
            this->NavigationWidget->GetRenderWindow()->AddObserver(vtkCommand::AbortCheckEvent,(vtkCommand*)this->GUICallbackCommand);
        }

    vtkSlicerApplicationGUI *p = vtkSlicerApplicationGUI::SafeDownCast( this->GetApplicationGUI ( ));    
    // 3DViewer's renderer and its camera
    vtkSlicerViewerWidget* viewer_widget = p->GetActiveViewerWidget();
    vtkRenderer *ren = viewer_widget ? viewer_widget->GetMainViewer()->GetRenderer() : NULL;
    vtkCamera *cam = ren ? (ren->IsActiveCameraCreated() ? ren->GetActiveCamera() : NULL) : NULL;
    if (!cam)
      {
      return;
      }
    
    // 3DViewer's camera configuration
    double *focalPoint = cam->GetFocalPoint ( );
    double *camPos= cam->GetPosition ( );
    double *vpn = cam->GetViewPlaneNormal ();
    double thetaV = (cam->GetViewAngle()) / 2.0;

    // camera distance, and distance of FOVBox from focalPoint
    double camDist = cam->GetDistance ();
    double boxDist = camDist * 0.89;

    // configure navcam based on main renderer's camera
    vtkRenderer *navren = this->NavigationWidget->GetRenderer();
    vtkCamera *navcam = this->NavigationWidget->GetRenderer()->IsActiveCameraCreated() ? this->NavigationWidget->GetRenderer()->GetActiveCamera() : NULL;

    if ( navcam != NULL )
      {
      // give navcam the same parameters as MainViewer's ActiveCamera
      navcam->SetPosition ( camPos );
      navcam->SetFocalPoint ( focalPoint );
      navcam->SetViewUp( cam->GetViewUp() );
      navcam->ComputeViewPlaneNormal ( );

      // compute FOVBox height & width to correspond 
      // to the main viewer's size and aspect ratio, in world-coordinates,
      // positioned just behind the near clipping plane, to make sure
      // nothing in the scene occludes it.
      double boxHalfHit;
      if ( cam->GetParallelProjection() )
        {
        boxHalfHit = cam->GetParallelScale();
        }
      else
        {
        boxHalfHit = (camDist) * tan ( thetaV * DEGREES2RADIANS);
        }

      // 3D MainViewer height and width for computing aspect
      int mainViewerWid = ren->GetRenderWindow()->GetSize()[0];
      int mainViewerHit = ren->GetRenderWindow()->GetSize()[1];
      // width of the FOVBox that represents MainViewer window.
      double boxHalfWid = boxHalfHit * (double)mainViewerWid / (double)mainViewerHit;

      // configure and position the FOVBox
      double data [24];
      data[0] = -1.0;
      data[1] = -1.0;
      data[2] = 0.0;
      data[3] = 1.0;
      data[4] = -1.0;
      data[5] = 0.0;
      data[6] = -1.0;
      data[7] = 1.0;
      data[8] = 0.0;
      data[9] = 1.0;
      data[10] = 1.0;
      data[11] = 0.0;

      data[12] = -1.0;
      data[13] = -1.0;
      data[14] = 0.0;
      data[15] = 1.0;
      data[16] = -1.0;
      data[17] = 0.0;
      data[18] = -1.0;
      data[19] = 1.0;
      data[20] = 0.0;
      data[21] = 1.0;
      data[22] = 1.0;
      data[23] = 0.0;
      this->FOVBox->SetCorners ( data );
      // Position and scale FOVBox very close to camera,
      // to prevent things in the scene from occluding it.
      this->FOVBoxActor->SetScale ( boxHalfWid, boxHalfHit, 1.0);
      this->FOVBoxActor->SetPosition (focalPoint[0]+ (vpn[0]*boxDist),
                                      focalPoint[1] + (vpn[1]*boxDist),
                                      focalPoint[2] + (vpn[2]*boxDist));
      this->ResetNavigationCamera();
      // put the focal point back into the center of
      // the scene without the FOVBox included,
      // since ResetNavigationCamera moved it.
      navcam->SetFocalPoint ( focalPoint );
      navren->ResetCameraClippingRange();
      navren->UpdateLightsGeometryToFollowCamera();
      }
      navren = NULL;
      navcam = NULL;
      p = NULL;
    }
#endif
}



//---------------------------------------------------------------------------
void vtkSlicerViewControlGUI::UpdateNavigationWidgetViewActors ( )
{

#ifndef NAVZOOMWIDGET_DEBUG
  if ( this->GetApplicationGUI() != NULL )
    {
    vtkRenderer *ren;
    vtkActorCollection *mainActors;
    vtkActor *mainActor;
    vtkActorCollection *navActors;
    vtkActor *newActor;
    vtkPolyDataMapper *newMapper;
    double bounds[6];
    double dimension;
    double x,y,z;
    double cutoff = 0.1;
    double cutoffDimension;

    // iterate thru NavigationWidget's actor collection,
    // remove item, delete actor, delete mapper.
    navActors = this->NavigationWidget->GetRenderer()->GetActors();
    
    if (navActors != NULL )
      {
      this->NavigationWidget->RemoveAllViewProps();
      navActors->RemoveAllItems();
      }

    // get estimate of Main Viewer's visible scene max dimension;
    vtkSlicerApplicationGUI *p = vtkSlicerApplicationGUI::SafeDownCast( this->GetApplicationGUI ( ));    
    vtkSlicerViewerWidget* viewer_widget = p->GetActiveViewerWidget();
    ren = viewer_widget ? viewer_widget->GetMainViewer()->GetRenderer() : NULL;
    if (!ren)
      {
      return;
      }
    ren->ComputeVisiblePropBounds( bounds );
    x = bounds[1] - bounds[0];
    y = bounds[3] - bounds[2];
    z = bounds[5] - bounds[4];
    dimension = x*x + y*y + z*z;
    cutoffDimension = cutoff * dimension;

    // Get actor collection from the main viewer's renderer
    mainActors = ren->GetActors();    
    if (mainActors != NULL )
      {
      // add the little FOV box to NavigationWidget's actors
      this->NavigationWidget->GetRenderer()->AddViewProp( this->FOVBoxActor);
      mainActors->InitTraversal();
      mainActor = mainActors->GetNextActor();
      while (mainActor != NULL )
        {
        // get the bbox of this actor
        int vis = mainActor->GetVisibility();
        //if (vis)
        //  {
          mainActor->GetBounds ( bounds );
          // check to see if it's big enough to include in the scene...
          x = bounds[1] - bounds[0];
          y = bounds[3] - bounds[2];
          z = bounds[5] - bounds[4];
          dimension = x*x + y*y + z*z;
         // }
        // add a copy of the actor to NavigationWidgets's renderer
        // only if it's big enough to count (don't bother with tiny
        // and don't bother with invisible stuff)
        if ( dimension > cutoffDimension && vis )
          {
          // ---new: create new actor, mapper, deep copy, add it.
          newMapper = vtkPolyDataMapper::New();
          newMapper->ShallowCopy (mainActor->GetMapper() );
          newMapper->SetInput ( vtkPolyData::SafeDownCast(mainActor->GetMapper()->GetInput()) );

          newActor = vtkActor::New();
          newActor->ShallowCopy (mainActor );
          newActor->SetMapper ( newMapper );
          newMapper->Delete();
          
          this->NavigationWidget->GetRenderer()->AddActor( newActor );
          newActor->Delete();
          }
        mainActor = mainActors->GetNextActor();
        }
      }
    }
#endif
}





//---------------------------------------------------------------------------
void vtkSlicerViewControlGUI::MainViewSetStereo ( )
{
  if ( this->GetApplicationGUI() != NULL )
    {
    vtkSlicerApplicationGUI *p = vtkSlicerApplicationGUI::SafeDownCast( this->GetApplicationGUI ( )); 
    vtkMRMLViewNode *vn = this->GetActiveView();
    vtkSlicerViewerWidget* viewer_widget = p->GetActiveViewerWidget();
    if (!viewer_widget)
      {
      return;
      }

    if ( vn != NULL )
      {
      int s = vn->GetStereoType();
      switch ( s )
        {
        case vtkMRMLViewNode::NoStereo:
          viewer_widget->GetMainViewer()->GetRenderWindow()->StereoRenderOff ( );
          break;
        case vtkMRMLViewNode::RedBlue:
          viewer_widget->GetMainViewer()->GetRenderWindow()->SetStereoTypeToRedBlue();
          viewer_widget->GetMainViewer()->GetRenderWindow()->StereoRenderOn ( );
          break;
        case vtkMRMLViewNode::Anaglyph:
          viewer_widget->GetMainViewer()->GetRenderWindow()->SetStereoTypeToAnaglyph();
          //viewer_widget->GetMainViewer()->GetRenderWindow()->SetAnaglyphColorSaturation(0.1);
          viewer_widget->GetMainViewer()->GetRenderWindow()->StereoRenderOn ( );
          break;
        case vtkMRMLViewNode::CrystalEyes:
          viewer_widget->GetMainViewer()->GetRenderWindow()->SetStereoTypeToCrystalEyes();
          viewer_widget->GetMainViewer()->GetRenderWindow()->StereoRenderOn ( );
          break;
        case vtkMRMLViewNode::Interlaced:
          viewer_widget->GetMainViewer()->GetRenderWindow()->SetStereoTypeToInterlaced();
          viewer_widget->GetMainViewer()->GetRenderWindow()->StereoRenderOn ( );
          break;
        default:
          viewer_widget->GetMainViewer()->GetRenderWindow()->StereoRenderOff ( );
          break;
        }
      viewer_widget->RequestRender();
      }
    } 
}


//---------------------------------------------------------------------------
void vtkSlicerViewControlGUI::DeviceCoordinatesToXYZ(vtkSlicerSliceGUI *sgui, int x, int y, int xyz[3] )
{

  if ( sgui == NULL )
    {
    vtkErrorMacro ( "ViewControlGUI: Got NULL SlicerSLiceGUI" );
    return;
    }
  vtkMRMLSliceNode *snode = sgui->GetSliceNode();
  if ( snode == NULL )
    {
    vtkErrorMacro ( "ViewControlGUI: got NULL Slice Node." );
    return;
    }
  
  vtkRenderWindowInteractor *iren
    = sgui->GetSliceViewer()->GetRenderWidget()->GetRenderWindowInteractor();
  int *windowSize = iren->GetRenderWindow()->GetSize();

  double tx = x / (double) windowSize[0];
  double ty = (windowSize[1] - y) / (double) windowSize[1];
  
  vtkRenderer *ren = iren->FindPokedRenderer(x, y);
  int *origin = ren->GetOrigin();
  
  xyz[0] = x - origin[0];
  xyz[1] = y - origin[1];
  xyz[2] = static_cast<int> ( floor(ty*snode->GetLayoutGridRows())
                              *snode->GetLayoutGridColumns()
                              + floor(tx*snode->GetLayoutGridColumns()) );
}


//---------------------------------------------------------------------------
void vtkSlicerViewControlGUI::SliceViewMagnify(int event, vtkSlicerInteractorStyle *istyle )
{
  int x, y;
  if ( this->GetApplicationGUI() != NULL )
    {
    vtkSlicerApplicationGUI *appGUI = vtkSlicerApplicationGUI::SafeDownCast( this->GetApplicationGUI ( ));
    
    if ( istyle == this->RedSliceEvents )
      {
      if ( appGUI->GetMainSliceGUI("Red")->GetLogic() != NULL )
        {
        if (appGUI->GetMainSliceGUI("Red")->GetLogic()->GetImageData() != NULL )
          {
          if (event == vtkCommand::EnterEvent )
            {
            // configure zoom
            x = this->RedSliceEvents->GetLastPos ()[0];
            y = this->RedSliceEvents->GetLastPos ()[1];

            // check that the event position is in the window
            int *windowSize =
              appGUI->GetMainSliceGUI("Red")->GetSliceViewer()->GetRenderWidget()->GetRenderWindowInteractor()->GetRenderWindow()->GetSize();
            if ( x >= 0 && y >= 0 && x < windowSize[0] && y < windowSize[1] )
              {
              int xyz[3];
              this->DeviceCoordinatesToXYZ(appGUI->GetMainSliceGUI("Red"),
                                           x, y, xyz);

              this->SliceMagnifier->SetX ( xyz[0] );
              this->SliceMagnifier->SetY ( xyz[1] );
              this->SliceMagnifier->SetZ ( xyz[2] );
              }
            else
              {
              // event position is not in the window, punt
              this->SliceMagnifier->SetX( 0 );
              this->SliceMagnifier->SetY( 0 );
              this->SliceMagnifier->SetZ( 0 );
              }
            this->SliceMagnifier->SetInput ( appGUI->GetMainSliceGUI("Red")->GetLogic()->GetImageData());
            this->SliceMagnifierCursor->SetInput ( this->SliceMagnifier->GetOutput());
            this->SliceMagnifierMapper->SetInput ( this->SliceMagnifierCursor->GetOutput() );
            this->SliceMagnifierActor->SetMapper ( this->SliceMagnifierMapper );
            this->ZoomWidget->GetRenderer()->AddActor2D ( this->SliceMagnifierActor );
            this->RequestZoomRender();
            this->PackZoomWidget();
            }
          else if (event == vtkCommand::LeaveEvent )
            {
            this->PackNavigationWidget();          
            this->ZoomWidget->GetRenderer()->RemoveActor2D ( this->SliceMagnifierActor );
            this->SliceMagnifierMapper->SetInput ( NULL );
            this->SliceMagnifierCursor->SetInput ( NULL );
            this->SliceMagnifier->SetInput ( NULL );
            }
          else if ( event == vtkCommand::MouseMoveEvent )
            {
            // configure zoom
            x = this->RedSliceEvents->GetLastPos ()[0];
            y = this->RedSliceEvents->GetLastPos ()[1];

            int xyz[3];
            this->DeviceCoordinatesToXYZ(appGUI->GetMainSliceGUI("Red"),
                                           x, y, xyz);
            this->SliceMagnifier->SetX ( xyz[0] );
            this->SliceMagnifier->SetY ( xyz[1] );
            this->SliceMagnifier->SetZ ( xyz[2] );

            this->SliceMagnifier->SetInput ( appGUI->GetMainSliceGUI("Red")->GetLogic()->GetImageData());
            this->RequestZoomRender();
            }
          }
        }
      }
    
    else if ( istyle == this->YellowSliceEvents)
      {
      if ( appGUI->GetMainSliceGUI("Yellow")->GetLogic() != NULL )
        {
        if (appGUI->GetMainSliceGUI("Yellow")->GetLogic()->GetImageData() != NULL )
          {
          if (event == vtkCommand::EnterEvent )
            {
            // configure zoom
            x = this->YellowSliceEvents->GetLastPos ()[0];
            y = this->YellowSliceEvents->GetLastPos ()[1];

            // check that the event position is in the window
            int *windowSize =
              appGUI->GetMainSliceGUI("Yellow")->GetSliceViewer()->GetRenderWidget()->GetRenderWindowInteractor()->GetRenderWindow()->GetSize();
            if ( x >= 0 && y >= 0 && x < windowSize[0] && y < windowSize[1] )
              {
              int xyz[3];
              this->DeviceCoordinatesToXYZ(appGUI->GetMainSliceGUI("Yellow"),
                                           x, y, xyz);
              this->SliceMagnifier->SetX ( xyz[0] );
              this->SliceMagnifier->SetY ( xyz[1] );
              this->SliceMagnifier->SetZ ( xyz[2] );
              }
            else
              {
              // event position is not in the window, punt
              this->SliceMagnifier->SetX( 0 );
              this->SliceMagnifier->SetY( 0 );
              this->SliceMagnifier->SetZ( 0 );
              }
            this->SliceMagnifier->SetInput ( appGUI->GetMainSliceGUI("Yellow")->GetLogic()->GetImageData());
            this->SliceMagnifierCursor->SetInput ( this->SliceMagnifier->GetOutput());
            this->SliceMagnifierMapper->SetInput ( this->SliceMagnifierCursor->GetOutput() );
            this->SliceMagnifierActor->SetMapper ( this->SliceMagnifierMapper );
            this->ZoomWidget->GetRenderer()->AddActor2D ( this->SliceMagnifierActor );
            this->RequestZoomRender();
            this->PackZoomWidget();
            }
          else if (event == vtkCommand::LeaveEvent )
            {
            this->PackNavigationWidget();
            this->ZoomWidget->GetRenderer()->RemoveActor2D ( this->SliceMagnifierActor );
            this->SliceMagnifierMapper->SetInput ( NULL );
            this->SliceMagnifierCursor->SetInput ( NULL );
            this->SliceMagnifier->SetInput ( NULL );
            }
          else if ( event == vtkCommand::MouseMoveEvent )
            {
            // configure zoom
            x = this->YellowSliceEvents->GetLastPos ()[0];
            y = this->YellowSliceEvents->GetLastPos ()[1];

            int xyz[3];
            this->DeviceCoordinatesToXYZ(appGUI->GetMainSliceGUI("Yellow"),
                                           x, y, xyz);
            this->SliceMagnifier->SetX ( xyz[0] );
            this->SliceMagnifier->SetY ( xyz[1] );
            this->SliceMagnifier->SetZ ( xyz[2] );

            this->SliceMagnifier->SetInput (appGUI->GetMainSliceGUI("Yellow")->GetLogic()->GetImageData());
            this->RequestZoomRender();
            }
          }
        }
      }
    else if ( istyle == this->GreenSliceEvents )
      {
      if ( appGUI->GetMainSliceGUI("Green")->GetLogic() != NULL )
        {
        if (appGUI->GetMainSliceGUI("Green")->GetLogic()->GetImageData() != NULL )
          {
          if (event == vtkCommand::EnterEvent )
            {
            // configure zoom
            x = this->GreenSliceEvents->GetLastPos ()[0];
            y = this->GreenSliceEvents->GetLastPos ()[1];

            // check that the event position is in the window
            int *windowSize =
              appGUI->GetMainSliceGUI("Green")->GetSliceViewer()->GetRenderWidget()->GetRenderWindowInteractor()->GetRenderWindow()->GetSize();
            if ( x >= 0 && y >= 0 && x < windowSize[0] && y < windowSize[1] )
              {
              int xyz[3];
              this->DeviceCoordinatesToXYZ(appGUI->GetMainSliceGUI("Green"),
                                           x, y, xyz);
              this->SliceMagnifier->SetX ( xyz[0] );
              this->SliceMagnifier->SetY ( xyz[1] );
              this->SliceMagnifier->SetZ ( xyz[2] );
              }
            else
              {
              // event position is not in the window, punt
              this->SliceMagnifier->SetX( 0 );
              this->SliceMagnifier->SetY( 0 );
              this->SliceMagnifier->SetZ( 0 );
              }
            this->SliceMagnifier->SetInput ( appGUI->GetMainSliceGUI("Green")->GetLogic()->GetImageData());
            this->SliceMagnifierCursor->SetInput ( this->SliceMagnifier->GetOutput());
            this->SliceMagnifierMapper->SetInput ( this->SliceMagnifierCursor->GetOutput() );
            this->SliceMagnifierActor->SetMapper ( this->SliceMagnifierMapper );
            this->ZoomWidget->GetRenderer()->AddActor2D ( this->SliceMagnifierActor );
            this->RequestZoomRender();
            this->PackZoomWidget();
            }
          else if (event == vtkCommand::LeaveEvent )
            {
            this->PackNavigationWidget();
            this->ZoomWidget->GetRenderer()->RemoveActor2D ( this->SliceMagnifierActor );
            this->SliceMagnifierMapper->SetInput ( NULL );
            this->SliceMagnifierCursor->SetInput ( NULL );
            this->SliceMagnifier->SetInput ( NULL );
            }
          else if ( event == vtkCommand::MouseMoveEvent )
            {
            // configure zoom
            x = this->GreenSliceEvents->GetLastPos ()[0];
            y = this->GreenSliceEvents->GetLastPos ()[1];

            int xyz[3];
            this->DeviceCoordinatesToXYZ(appGUI->GetMainSliceGUI("Green"),
                                           x, y, xyz);
            this->SliceMagnifier->SetX ( xyz[0] );
            this->SliceMagnifier->SetY ( xyz[1] );
            this->SliceMagnifier->SetZ ( xyz[2] );

            this->SliceMagnifier->SetInput (appGUI->GetMainSliceGUI("Green")->GetLogic()->GetImageData());
            this->RequestZoomRender();
            }
          }
        }
      }
    }
}



//---------------------------------------------------------------------------
void vtkSlicerViewControlGUI::MainViewResetFocalPoint ( )
{
  double x_cen, y_cen, z_cen;
  vtkRenderer *ren;
  double bounds[6];

  int boxVisible;
  int axisLabelsVisible;
  
  // This method computes the visible scene bbox and
  // recenters the camera around the bbox centroid.
  
  vtkMRMLViewNode *vn = this->GetActiveView();
  if ( vn != NULL )
    {
    boxVisible = vn->GetBoxVisible();
    axisLabelsVisible = vn->GetAxisLabelsVisible();

    // if box is visible, turn its visibility temporarily off
    if ( boxVisible )
      {
      vn->SetBoxVisible ( 0 );
      }
    // if axis actors are visible, turn their visibility temporarily off.
    if ( axisLabelsVisible )
      {
      vn->SetAxisLabelsVisible ( 0 );
      }

    vtkSlicerApplicationGUI *p = 
      vtkSlicerApplicationGUI::SafeDownCast( this->GetApplicationGUI ( ));    
    vtkSlicerViewerWidget* viewer_widget = p->GetActiveViewerWidget();
    if (!viewer_widget)
      {
      return;
      }
    ren = viewer_widget->GetMainViewer()->GetRenderer();
    ren->ComputeVisiblePropBounds( bounds );
    x_cen = (bounds[1] + bounds[0]) / 2.0;
    y_cen = (bounds[3] + bounds[2]) / 2.0;
    z_cen = (bounds[5] + bounds[4]) / 2.0;
    this->MainViewSetFocalPoint ( x_cen, y_cen, z_cen);

  // if box was visible, turn its visibility back on.
    if ( boxVisible )
      {
      vn->SetBoxVisible ( 1 );
      }
  // if axis actors were visible, turn their visibility back on.
    if ( axisLabelsVisible )
      {
      vn->SetAxisLabelsVisible ( 1 );
      }
    
  // Code used to recenter the view around the origin.
  //  this->MainViewSetFocalPoint ( 0.0, 0.0, 0.0);
    }
}


//---------------------------------------------------------------------------
void vtkSlicerViewControlGUI::MainViewSetFocalPoint ( double x, double y, double z)
{
   if ( this->ApplicationGUI )
    {
    vtkSlicerApplicationGUI *p = vtkSlicerApplicationGUI::SafeDownCast( this->GetApplicationGUI ( ));    
    vtkMRMLViewNode *vn = this->GetActiveView();
    if ( vn != NULL )
      {
      vtkMRMLCameraNode *cn = this->GetActiveCamera();
      if ( cn != NULL )
        {
        vtkCamera *cam = cn->GetCamera();
        cam->SetFocalPoint( x, y, z );
        cam->ComputeViewPlaneNormal ( );
        cam->OrthogonalizeViewUp();
        vtkSlicerViewerWidget* viewer_widget = p->GetActiveViewerWidget();
        if (viewer_widget)
          {
          viewer_widget->GetMainViewer()->GetRenderer()->UpdateLightsGeometryToFollowCamera();
          viewer_widget->RequestRender();
          this->RequestNavigationRender();
          }
        }
      }
    }
}


//---------------------------------------------------------------------------
void vtkSlicerViewControlGUI::MainViewRock ( )
{
  if ( this->ApplicationGUI )
    {
    vtkMRMLViewNode *vn = this->GetActiveView();
    if ( vn != NULL )
      {
      if ( vn->GetAnimationMode() == vtkMRMLViewNode::Rock )
        {
        this->RockView ( );
        this->Script ( "update idletasks" );
        this->Script ( "after 5 \"%s MainViewRock \"",  this->GetTclName() );
        }
      }
    }
}




//---------------------------------------------------------------------------
void vtkSlicerViewControlGUI::RockView ( )
{

  if ( this->ApplicationGUI )
    {
    vtkSlicerApplicationGUI *p = vtkSlicerApplicationGUI::SafeDownCast( this->GetApplicationGUI ( ));    
    vtkMRMLViewNode *vn = this->GetActiveView();
    if ( vn != NULL )
      {
      vtkMRMLCameraNode *cn = this->GetActiveCamera();
      if ( cn != NULL )
        {
        vtkCamera *cam = cn->GetCamera();

        double frac = (double) this->RockCount / (double) vn->GetRockLength();
        double az = 1.5 * cos ( 2.0 * 3.1415926 * (frac- floor(frac)));
        this->SetRockCount ( this->GetRockCount() + 1 );

        // Move the camera
        cam->Azimuth ( az );
        cam->OrthogonalizeViewUp ( );
        
        //Make the lighting follow the camera to avoid illumination changes
        vtkSlicerViewerWidget* viewer_widget = p->GetActiveViewerWidget();
        if (viewer_widget)
          {
          viewer_widget->GetMainViewer()->GetRenderer()->UpdateLightsGeometryToFollowCamera();
          viewer_widget->RequestRender();
          // don't rock the view in navigation render for performance reasons.
//          this->RequestNavigationRender();
          }
        }
      }
    }
}




//---------------------------------------------------------------------------
void vtkSlicerViewControlGUI::MainViewSpin ( )
{
  if ( this->ApplicationGUI )
    {
    vtkMRMLViewNode *vn = this->GetActiveView();
    if ( vn != NULL )  
      {
      if ( vn->GetAnimationMode() == vtkMRMLViewNode::Spin )
        {
        this->SpinView (vn->GetSpinDirection(), vn->GetSpinDegrees() );
        this->Script ( "update idletasks" );
        this->Script ( "after 5 \"%s MainViewSpin \"",  this->GetTclName() );
        }
      }
    }
}


//---------------------------------------------------------------------------
void vtkSlicerViewControlGUI::SpinView ( int dir, double degrees )
{
  
  if ( this->ApplicationGUI )
    {
    vtkSlicerApplicationGUI *p = vtkSlicerApplicationGUI::SafeDownCast( this->GetApplicationGUI ( ));    
    vtkMRMLViewNode *vn = this->GetActiveView();
    if ( vn != NULL )  
      {
      vtkMRMLCameraNode *cn = this->GetActiveCamera();
      if ( cn != NULL )
        {
        vtkCamera *cam = cn->GetCamera();
        double ndegrees = -degrees;

        switch ( dir ) {
        case vtkMRMLViewNode::PitchUp:
          cam->Elevation ( degrees );
          break;
        case vtkMRMLViewNode::PitchDown:
          cam->Elevation ( ndegrees );
          break;
        case vtkMRMLViewNode::YawLeft:
          cam->Azimuth ( degrees );
          break;
        case vtkMRMLViewNode::YawRight:
          cam->Azimuth ( ndegrees );
          break;
        default:
          break;
        }
        cam->OrthogonalizeViewUp ( );

        //Make the lighting follow the camera to avoid illumination changes
        vtkSlicerViewerWidget* viewer_widget = p->GetActiveViewerWidget();
        if (viewer_widget)
          {
          viewer_widget->GetMainViewer()->GetRenderer()->UpdateLightsGeometryToFollowCamera();
          viewer_widget->RequestRender();
          // don't spin the view in navigation render for performance reasons.
          //this->RequestNavigationRender();
          }
        }
      }  
    }
}




//---------------------------------------------------------------------------
void vtkSlicerViewControlGUI::MainViewBackgroundColor ( double *color )
{

  if ( this->ApplicationGUI )
    {
    vtkSlicerApplicationGUI *p = vtkSlicerApplicationGUI::SafeDownCast( this->GetApplicationGUI ( ));    
    // set background color
    vtkSlicerViewerWidget* viewer_widget = p->GetActiveViewerWidget();
    if (!viewer_widget)
      {
      return;
      }
    if ( !viewer_widget->GetMainViewer() )
      {
      return;
      }
    viewer_widget->GetMainViewer()->SetRendererBackgroundColor ( color );

    // set axis label colors (prevent white on white)
    if ( color[0] == 1.0 && color[1] == 1.0 && color[2] == 1.0 )
      {
      viewer_widget->ColorAxisLabelActors (0.0, 0.0, 0.0 );
      }
    else
      {
      viewer_widget->ColorAxisLabelActors (1.0, 1.0, 1.0 );
      }
    viewer_widget->UpdateFromMRML();
    }

  if ( this->GetNavigationWidget())
    {
    this->GetNavigationWidget()->SetRendererBackgroundColor ( color );
    }
}

//---------------------------------------------------------------------------
void vtkSlicerViewControlGUI::MainViewVisibility ( )
{
  if ( this->ApplicationGUI)
    {
    vtkSlicerApplicationGUI *p = vtkSlicerApplicationGUI::SafeDownCast( this->GetApplicationGUI ( ));    
    vtkMRMLViewNode *vn = this->GetActiveView();
    if ( vn != NULL )  
      {
      vtkSlicerViewerWidget* viewer_widget = p->GetActiveViewerWidget();
      if (viewer_widget)
        {
        viewer_widget->UpdateFromMRML();
        }
      }
    }
}
  
//---------------------------------------------------------------------------
void vtkSlicerViewControlGUI::MainViewSetProjection ( )
{

  if ( this->ApplicationGUI)
    {
    vtkSlicerApplicationGUI *p = vtkSlicerApplicationGUI::SafeDownCast( this->GetApplicationGUI ( ));    
    vtkMRMLViewNode *vn = this->GetActiveView();
    if ( vn != NULL )  
      {
      vtkMRMLCameraNode *cn = this->GetActiveCamera();
      if ( cn != NULL )
        {
        vtkCamera *cam = cn->GetCamera();
        // update the Rendering mode, then toggle
        // the button's icon appropriately 
        if ( vn->GetRenderMode() == vtkMRMLViewNode::Perspective )
          {
          cam->ParallelProjectionOff();
          this->OrthoButton->SetImageToIcon ( this->SlicerViewControlIcons->GetOrthoButtonIcon() );
          }
        else if ( vn->GetRenderMode() == vtkMRMLViewNode::Orthographic )
          {
          cam->ParallelProjectionOn();
          cam->SetParallelScale ( vn->GetFieldOfView() );
          this->OrthoButton->SetImageToIcon ( this->SlicerViewControlIcons->GetPerspectiveButtonIcon() );
          }
        vtkSlicerViewerWidget* viewer_widget = p->GetActiveViewerWidget();
        if (viewer_widget)
          {
          viewer_widget->RequestRender();
          this->RequestNavigationRender();
          }
        }
      }
    }
}





//---------------------------------------------------------------------------
void vtkSlicerViewControlGUI::MainViewPitch ( )
{
  double deg, negdeg;
  vtkMRMLViewNode *vn = this->GetActiveView();
  if ( this->ApplicationGUI && vn )
    {
    vtkSlicerApplicationGUI *p = vtkSlicerApplicationGUI::SafeDownCast( this->GetApplicationGUI ( ));    
    deg = vn->GetRotateDegrees ( );
    negdeg = -deg;
  
    vtkMRMLCameraNode *cn = this->GetActiveCamera();
    if ( cn != NULL )
      {
      vtkCamera *cam = cn->GetCamera();
      cam->Elevation ( deg );
      cam->OrthogonalizeViewUp();
      vtkSlicerViewerWidget* viewer_widget = p->GetActiveViewerWidget();
      if (viewer_widget)
        {
        viewer_widget->GetMainViewer()->GetRenderer()->UpdateLightsGeometryToFollowCamera();
        viewer_widget->RequestRender();
        this->RequestNavigationRender();
        }
      }
    }
}
//---------------------------------------------------------------------------
void vtkSlicerViewControlGUI::MainViewRoll ( )
{
double deg, negdeg;
vtkMRMLViewNode *vn = this->GetActiveView();
if ( this->ApplicationGUI && vn )
{
    vtkSlicerApplicationGUI *p = vtkSlicerApplicationGUI::SafeDownCast( this->GetApplicationGUI ( ));    
    deg = vn->GetRotateDegrees ( );
    negdeg = -deg;
  
    vtkMRMLCameraNode *cn = this->GetActiveCamera();
    if ( cn != NULL )
      {
      vtkCamera *cam = cn->GetCamera();
      cam->Roll ( deg );
      cam->OrthogonalizeViewUp();
      vtkSlicerViewerWidget* viewer_widget = p->GetActiveViewerWidget();
      if (viewer_widget)
        {
        viewer_widget->GetMainViewer()->GetRenderer()->UpdateLightsGeometryToFollowCamera();
        viewer_widget->RequestRender();
        this->RequestNavigationRender();
      }
    }
}
}
//---------------------------------------------------------------------------
void vtkSlicerViewControlGUI::MainViewYaw ( )
{
  double deg, negdeg;
  vtkMRMLViewNode *vn = this->GetActiveView();
  if ( this->ApplicationGUI && vn )
    {
    vtkSlicerApplicationGUI *p = vtkSlicerApplicationGUI::SafeDownCast( this->GetApplicationGUI ( ));    
    deg = vn->GetRotateDegrees ( );
    negdeg = -deg;
  
    vtkMRMLCameraNode *cn = this->GetActiveCamera();
    if ( cn != NULL )
      {
      vtkCamera *cam = cn->GetCamera();
      cam->Azimuth ( deg );
      cam->OrthogonalizeViewUp();
      vtkSlicerViewerWidget* viewer_widget = p->GetActiveViewerWidget();
      if (viewer_widget)
        {
        viewer_widget->GetMainViewer()->GetRenderer()->UpdateLightsGeometryToFollowCamera();
        viewer_widget->RequestRender();
        this->RequestNavigationRender();
      }
    }
    }

}

//---------------------------------------------------------------------------
void vtkSlicerViewControlGUI::MainViewZoomIn ( )
{
  // percent
  double zoomfactor = 105.0;
  zoomfactor /= 100.0;
  this->MainViewZoom ( zoomfactor );
}

//---------------------------------------------------------------------------
void vtkSlicerViewControlGUI::MainViewZoomOut ( )
{
  // percent
  double zoomfactor = 95.0;
  zoomfactor /= 100.0;
  this->MainViewZoom ( zoomfactor );
}



//---------------------------------------------------------------------------
void vtkSlicerViewControlGUI::MainViewRotateAround ( const char *axis )
{
  double deg;
  // axis vector
  double a[3];
  // look-at point
  double fp[3];
  // camera position
  double p[3];
  // camera up
  double up[3];
  double magup;
  // view vector and its magnitude
  double v[3];
  double nv[3];
  double magv;
  // center of circle around which to rotate
  double c[3];
  // vector along which to translate camera
  double q[3];
  // angular offset from axis to rotate around.
  double theta = 0.0;
  double dot;

  if ( this->ApplicationGUI)
    {
    vtkSlicerApplicationGUI *appGUI = vtkSlicerApplicationGUI::SafeDownCast( this->GetApplicationGUI ( ));
    vtkMRMLViewNode *vn = this->GetActiveView();
    if ( vn != NULL )  
      {
      deg = vn->GetRotateDegrees ( );

      vtkMRMLCameraNode *cn = this->GetActiveCamera();
      if ( cn != NULL )
        {
        vtkCamera *cam = cn->GetCamera();
        // get camera position
        cam->GetPosition(p);
        cam->GetFocalPoint(fp);
        
        // magnitude of view vector should be | p-fp | 
        v[0] = p[0] - fp[0];
        v[1] = p[1] - fp[1];
        v[2] = p[2] - fp[2];
        magv = vtkMath::Norm( v );
        nv[0] = v[0]/magv;
        nv[1] = v[1]/magv;
        nv[2] = v[2]/magv;        
        
        if ( (!strcmp (axis, "R")) || !(strcmp(axis, "L")) )
          {
          // unit normal vector for plane of rotation 
          a[0] = 1.0;
          a[1] = 0.0;
          a[2] = 0.0;
          // find angle to the unit R-L axis.
          dot = vtkMath::Dot(nv,a);
          theta = acos ( dot );
          // center of rotation
          c[0] = fp[0] + ( magv*cos(theta) );
          c[1] = fp[1];
          c[2] = fp[2];
          }
        if ( (!strcmp (axis, "A")) || !(strcmp(axis, "P")) )
          {
          // unit normal vector for plane of rotation 
          a[0] = 0.0;
          a[1] = 1.0;
          a[2] = 0.0;
          // find angle to the unit A-P axis.
          dot = vtkMath::Dot(nv,a);
          theta = acos ( dot );
          // center of rotation
          c[0] = fp[0];
          c[1] = fp[1] + ( magv*cos(theta) );
          c[2] = fp[2];
          }
        if ( (!strcmp (axis, "S")) || !(strcmp(axis, "I")) )
          {
          // unit normal vector for plane of rotation 
          a[0] = 0.0;
          a[1] = 0.0;
          a[2] = 1.0;
          // find angle to the unit S-I axis.
          dot = vtkMath::Dot(nv,a);
          theta = acos ( dot );
          // center of rotation
          c[0] = fp[0];
          c[1] = fp[1];
          c[2] = fp[2] + (magv*cos(theta) );
          }

        // if the current viewvector is along the axis,
        // then just roll.
        // right now, this doesn't seem to capture
        // all cases.
        if ( theta == 0.0 )
          {
          cam->Roll (deg);
          cam->OrthogonalizeViewUp();
          }
        else
          {
#if ( (VTK_MAJOR_VERSION >= 6) || ( VTK_MAJOR_VERSION == 5 && VTK_MINOR_VERSION >= 4 ) )
          deg = vtkMath::RadiansFromDegrees(deg);
#else
          deg *= vtkMath::DoubleDegreesToRadians();
#endif
          this->ArbitraryRotate( p, deg, fp, c, q);
          cam->SetPosition(q);
          cam->SetFocalPoint(fp);
          // now rotate up vector.
          cam->GetViewUp(up);
          this->ArbitraryRotate ( up, deg, fp, c, q);
          magup = vtkMath::Norm(q);
          q[0]/=magup;
          q[1]/=magup;
          q[2]/=magup;
          cam->SetViewUp(q);
          cam->GetViewUp(up);
          vtkDebugMacro( "ViewUp is: " << up[0] << up[1] << up[2] );          
          }
        vtkSlicerViewerWidget* viewer_widget = appGUI->GetActiveViewerWidget();
        if (viewer_widget)
          {
          viewer_widget->GetMainViewer()->GetRenderer()->UpdateLightsGeometryToFollowCamera();
          viewer_widget->RequestRender();
          this->RequestNavigationRender();
          }
        }
      }
    }
}


//---------------------------------------------------------------------------
void vtkSlicerViewControlGUI::ArbitraryRotate(double *p, double theta, double *p1, double *p2, double *q)
{
/*
   Rotate a point p by angle theta around an arbitrary line segment p1-p2
   Return the rotated point.
   Positive angles are anticlockwise looking down the axis
   towards the origin.
   Assume right hand coordinate system.  
*/
   double costheta,sintheta;
   double r[3];
   double mag;

   q[0] = q[1] = q[2] = 0.0;
   r[0] = p2[0] - p1[0];
   r[1] = p2[1] - p1[1];
   r[2] = p2[2] - p1[2];

   // shift back to the origin
   p[0] -= p1[0];
   p[1] -= p1[1];
   p[2] -= p1[2];

   mag = vtkMath::Norm(r);
   if (mag != 0.0)
     {
     r[0]/=mag;
     r[1]/=mag;
     r[2]/=mag;
     }

   costheta = cos(theta);
   sintheta = sin(theta);

   q[0] += (costheta + (1 - costheta) * r[0] * r[0]) * p[0];
   q[0] += ((1 - costheta) * r[0] * r[1] - r[2] * sintheta) * p[1];
   q[0] += ((1 - costheta) * r[0] * r[2] + r[1] * sintheta) * p[2];

   q[1] += ((1 - costheta) * r[0] * r[1] + r[2] * sintheta) * p[0];
   q[1] += (costheta + (1 - costheta) * r[1] * r[1]) * p[1];
   q[1] += ((1 - costheta) * r[1] * r[2] - r[0] * sintheta) * p[2];

   q[2] += ((1 - costheta) * r[0] * r[2] - r[1] * sintheta) * p[0];
   q[2] += ((1 - costheta) * r[1] * r[2] + r[0] * sintheta) * p[1];
   q[2] += (costheta + (1 - costheta) * r[2] * r[2]) * p[2];

   // translate back to original position
   q[0] += p1[0];
   q[1] += p1[1];
   q[2] += p1[2];
}






//---------------------------------------------------------------------------
void vtkSlicerViewControlGUI::MainViewRotateAround ( int axis )
{
  double deg, negdeg;
  double fp[3];

  if ( this->ApplicationGUI)
    {
   vtkSlicerApplicationGUI *p = vtkSlicerApplicationGUI::SafeDownCast( this->GetApplicationGUI ( ));    
   vtkMRMLViewNode *vn = this->GetActiveView();
    if ( vn != NULL )  
      {
      deg = vn->GetRotateDegrees ( );
      negdeg = -deg;

      vtkMRMLCameraNode *cn = this->GetActiveCamera();
      if ( cn != NULL )
        {
        vtkCamera *cam = cn->GetCamera();
        cam->GetFocalPoint(fp);
        
        switch ( axis )
          {
          case vtkMRMLViewNode::PitchDown:
            cam->Elevation ( deg );
            break;
          case vtkMRMLViewNode::PitchUp:
            cam->Elevation ( negdeg );
            break;
          case vtkMRMLViewNode::RollLeft:
            cam->Roll ( deg );
            break;
          case vtkMRMLViewNode::RollRight:
            cam->Roll ( negdeg );
            break;
          case vtkMRMLViewNode::YawLeft:
            cam->Azimuth ( deg );
            break;
          case vtkMRMLViewNode::YawRight:
            cam->Azimuth ( negdeg );
            break;
          default:
            break;
          }
        cam->OrthogonalizeViewUp();
        cam->GetFocalPoint(fp);
        vtkSlicerViewerWidget* viewer_widget = p->GetActiveViewerWidget();
        if (viewer_widget)
          {
          viewer_widget->GetMainViewer()->GetRenderer()->UpdateLightsGeometryToFollowCamera();
          viewer_widget->RequestRender();
          this->RequestNavigationRender();
          }
        }
      }
    }
}




  
//---------------------------------------------------------------------------
void vtkSlicerViewControlGUI::MainViewLookFrom ( const char *dir )
{

 double fov, *fp, widefov;

   if ( this->ApplicationGUI)
     {
     vtkSlicerApplicationGUI *p = vtkSlicerApplicationGUI::SafeDownCast( this->GetApplicationGUI ( ));    
     // TODO: get the active view, not the 0th view.
     vtkMRMLViewNode *vn = this->GetActiveView();
     if ( vn != NULL )  
       {
       fov = vn->GetFieldOfView ( );
       widefov = fov*3;
    
       vtkMRMLCameraNode *cn = this->GetActiveCamera();
      if ( cn != NULL )
        {
        vtkCamera *cam = cn->GetCamera();
        fp = cn->GetFocalPoint();

        if ( !strcmp (dir, "R"))
          {
          cam->SetPosition ( fp[0]+widefov, fp[1], fp[2] );
          cam->SetViewUp ( 0, 0, 1);
          }
        else if ( !strcmp (dir, "L"))
          {
          cam->SetPosition ( fp[0]-widefov, fp[1], fp[2]);
          cam->SetViewUp ( 0, 0, 1);        
          }
        else if ( !strcmp (dir, "S"))
          {
          cam->SetPosition ( fp[0], fp[1], fp[2]+widefov );
          cam->SetViewUp ( 0, 1, 0);
          }
        else if ( !strcmp (dir, "I"))
          {
          cam->SetPosition ( fp[0], fp[1], fp[2]-widefov );
          cam->SetViewUp ( 0, 1, 0);
          }
        else if ( !strcmp (dir, "A"))
          {
          cam->SetPosition (fp[0], fp[1]+widefov, fp[2] );
          cam->SetViewUp ( 0, 0, 1 );
          }
        else if ( !strcmp (dir, "P"))
          {
          cam->SetPosition (fp[0], fp[1]-widefov, fp[2] );
          cam->SetViewUp ( 0, 0, 1 );
          }
        vtkSlicerViewerWidget* viewer_widget = p->GetActiveViewerWidget();
        if (viewer_widget)
          {
          viewer_widget->GetMainViewer()->GetRenderer()->ResetCameraClippingRange ( );
          cam->ComputeViewPlaneNormal();
          cam->OrthogonalizeViewUp();
          viewer_widget->GetMainViewer()->GetRenderer()->UpdateLightsGeometryToFollowCamera();
          viewer_widget->RequestRender();      
          this->RequestNavigationRender();
          }
        }
       }
     }
}



  




//---------------------------------------------------------------------------
void vtkSlicerViewControlGUI::BuildVisibilityMenu ( )
{
  this->VisibilityButton->GetMenu()->DeleteAllItems ( );
  this->VisibilityButton->GetMenu()->AddCheckButton ("Fiducial points" );
  this->VisibilityButton->GetMenu()->AddCheckButton ("Fiducial labels" );
  this->VisibilityButton->GetMenu()->AddCheckButton ("3D cube" );
  this->VisibilityButton->GetMenu()->AddCheckButton ("3D axis labels" );
  this->VisibilityButton->GetMenu()->AddRadioButton ("Light blue background" );  
  this->VisibilityButton->GetMenu()->AddRadioButton ("Black background" );  
  this->VisibilityButton->GetMenu()->AddRadioButton ("White background" );  
  this->VisibilityButton->GetMenu()->AddSeparator();
  this->VisibilityButton->GetMenu()->AddCommand ( "close");
  this->VisibilityButton->GetMenu()->SetItemStateToDisabled ( "Fiducial points" );
  this->VisibilityButton->GetMenu()->SetItemStateToDisabled ( "Fiducial labels" );
//  this->VisibilityButton->GetMenu()->SelectItem ("Fiducial points" );
//  this->VisibilityButton->GetMenu()->SelectItem ("Fiducial labels" );
  this->VisibilityButton->GetMenu()->SelectItem ("3D cube" );
  this->VisibilityButton->GetMenu()->SelectItem ("3D axis labels" );
  this->VisibilityButton->GetMenu()->SelectItem ("Light blue background" );
}


//---------------------------------------------------------------------------
void vtkSlicerViewControlGUI::BuildStereoSelectMenu ( )
{
  this->StereoButton->GetMenu()->DeleteAllItems ( );
  this->StereoButton->GetMenu()->AddRadioButton ( "No stereo" );
  this->StereoButton->GetMenu()->AddRadioButton ( "Red/Blue" );
  this->StereoButton->GetMenu()->AddRadioButton ( "Anaglyph" );
  this->StereoButton->GetMenu()->AddRadioButton ( "Interlaced" );  
  this->StereoButton->GetMenu()->AddRadioButton ( "CrystalEyes" );
  this->StereoButton->GetMenu()->AddSeparator();
  this->StereoButton->GetMenu()->AddCommand ( "close");
  this->StereoButton->GetMenu()->SelectItem ( "No stereo");
  
  int enableStereoCapableWindow = 0;
  // check whether stereo is enabled.
  if ( this->GetApplicationGUI() != NULL )
    {
    vtkSlicerApplication *slicerApp = vtkSlicerApplication::SafeDownCast( this->GetApplicationGUI()->GetApplication() ); 
    if ( slicerApp->GetStereoEnabled() )
      {
      enableStereoCapableWindow = 1;
      }
    }
 if ( !enableStereoCapableWindow )
   {
     // let always enabled Red/Blue, Anaglyph, and Interlaced since those modes don't need the
     // special enabled stereo window
     this->StereoButton->GetMenu()->SetItemStateToDisabled ( "CrystalEyes" );
   }
}






//---------------------------------------------------------------------------
void vtkSlicerViewControlGUI::ProcessLogicEvents ( vtkObject * vtkNotUsed(caller),
                                            unsigned long vtkNotUsed(event), void * vtkNotUsed(callData) )
{
    // Fill in
}



//---------------------------------------------------------------------------
void vtkSlicerViewControlGUI::ProcessMRMLEvents ( vtkObject *caller,
                                           unsigned long event, void * vtkNotUsed(callData) )
{
  if (this->ProcessingMRMLEvent != 0 )
    {
    return;
    }

  this->ProcessingMRMLEvent = event;

  vtkDebugMacro("processing event " << event);
   
  if (event == vtkMRMLScene::SceneCloseEvent )
    {
    this->SceneClosing = true;
    }
  else 
    {
    this->SceneClosing = false;
    }


  vtkMRMLViewNode *vnode = vtkMRMLViewNode::SafeDownCast ( caller );
  vtkMRMLSelectionNode *snode = vtkMRMLSelectionNode::SafeDownCast ( caller );


  if ( this->GetMRMLScene() == NULL )
    {
    vtkErrorWithObjectMacro ( this, << "*********MRMLCallback called recursively?" << endl);
    return;
    }

  // has a node been added or deleted? Or has the scene closed or just opened??
  if ( vtkMRMLScene::SafeDownCast(caller) == this->MRMLScene)
    {
    if (( event == vtkMRMLScene::NodeAddedEvent || event == vtkMRMLScene::NodeRemovedEvent))
      {
      this->UpdateFromMRML();
      this->UpdateAppearanceFromMRML();
      this->UpdateBehaviorFromMRML();
      this->UpdateNavigationViewAppearanceFromMRML();
      this->UpdateNavigationWidgetViewActors ( );
      }
    else if (( event == vtkMRMLScene::SceneCloseEvent) || (event == vtkMRMLScene::NewSceneEvent ))
      {
      this->UpdateFromMRML();
      this->UpdateAppearanceFromMRML();
      this->UpdateBehaviorFromMRML();
      this->UpdateNavigationViewAppearanceFromMRML();
      this->UpdateNavigationWidgetViewActors ( );
      }
    }

  // has a new camera or view has been selected?
  if ( snode != NULL )
    {
     this->UpdateFromMRML();
     this->UpdateAppearanceFromMRML();
     this->UpdateBehaviorFromMRML();
     this->UpdateNavigationViewAppearanceFromMRML();
     this->UpdateNavigationWidgetViewActors ( );
    }    

  // has view been manipulated?
  if ( vnode != NULL )
    {
    if ( event == vtkMRMLViewNode::ActiveModifiedEvent )
      {
      this->MainViewBackgroundColor ( vnode->GetBackgroundColor() );
      this->UpdateAppearanceFromMRML();
      this->UpdateBehaviorFromMRML();
      this->UpdateNavigationViewAppearanceFromMRML();
      this->UpdateNavigationWidgetViewActors ( );
      this->RequestNavigationRender();
      }
    if (event == vtkMRMLViewNode::AnimationModeEvent )
      {
      // handle the mode change 
      if ( vnode->GetAnimationMode() == vtkMRMLViewNode::Spin )
        {
        if ( this->RockButton->GetSelectedState() == 1 )
          {
          this->RockButton->Deselect();
          }
        if ( this->SpinButton->GetSelectedState() == 0 )
          {
          this->SpinButton->Select();
          }
        this->MainViewSpin (  );
        }
      // handle the mode change
      else if ( vnode->GetAnimationMode() == vtkMRMLViewNode::Rock )
        {
        if ( this->SpinButton->GetSelectedState() == 1 )
          {
          this->SpinButton->Deselect();
          }
        if ( this->RockButton->GetSelectedState() == 0 )
          {
          this->RockButton->Select();
          }
        this->SetRockCount ( vnode->GetRockCount ( ) );
        this->MainViewRock ( );
        }
      else if ( vnode->GetAnimationMode() == vtkMRMLViewNode::Off )
        {
        if ( this->RockButton->GetSelectedState() == 1 )
          {
          this->RockButton->Deselect();
          }
        if ( this->SpinButton->GetSelectedState() == 1 )
          {
          this->SpinButton->Deselect();
          }
        if ( this->SpinButton->GetSelectedState() == 0 &&
             this->RockButton->GetSelectedState() == 0)
          {
          this->RequestNavigationRender();
          }
        }
      }

    // a stereo event?
    if ( event == vtkMRMLViewNode::StereoModeEvent )
      {
      // does the menu match the node? if not, update the menu
      this->MainViewSetStereo ( );
      }
    // background color change?
    else if ( event == vtkMRMLViewNode::BackgroundColorEvent )
      {
      // does the background color match the node? if not, update the menu
      this->MainViewBackgroundColor ( vnode->GetBackgroundColor() );
      // set the navigation render background color
      this->UpdateNavigationViewAppearanceFromMRML();      
      }
    // visibility of something changed
    else if ( event == vtkMRMLViewNode::VisibilityEvent )
      {
      // axis labels, fiducial points, fiducial labels or 3Dcube?
      // does the menu match the node? if not update the menu
      this->MainViewVisibility ( );
      // if an actor's visibility has changed, update the nav render.
      this->RequestNavigationRender();
      }
    // render mode changed
    else if ( event == vtkMRMLViewNode::RenderModeEvent )
      {
      // does the button match the state? if not, update the button.
      this->MainViewSetProjection();
      }
    // whatever else...
    }
  // handle whatever other change is made to the view.
  else
    {
    }
  this->ProcessingMRMLEvent = 0;
}




//---------------------------------------------------------------------------
void vtkSlicerViewControlGUI::Enter ( )
{
    // Fill in
}

//---------------------------------------------------------------------------
void vtkSlicerViewControlGUI::Exit ( )
{
    // Fill in
}


//---------------------------------------------------------------------------
void vtkSlicerViewControlGUI::SetApplicationGUI ( vtkSlicerApplicationGUI *appGUI )
{
  this->ApplicationGUI = appGUI;
}


//---------------------------------------------------------------------------
void vtkSlicerViewControlGUI::ViewControlACallback ( )
{
  // want to rotate around AP or look from A. Need to figure out
  // where A-vector is wrt camera view vector.
   if ( this->ApplicationGUI)
     {
     vtkMRMLViewNode *vn = this->GetActiveView();
     if ( vn != NULL )  
       {
       if ( vn->GetViewAxisMode() == vtkMRMLViewNode::RotateAround )
         {
//         this->MainViewRotateAround ( vtkMRMLViewNode::RollLeft );
         this->MainViewRotateAround ( "A" );
         }
       else
         {
         this->MainViewLookFrom ( "A" );
         }
       }
     }
}


//---------------------------------------------------------------------------
void vtkSlicerViewControlGUI::ViewControlPCallback ( )
{
   if ( this->ApplicationGUI)
     {
     vtkMRMLViewNode *vn = this->GetActiveView();
     if ( vn != NULL )  
       {
       if ( vn->GetViewAxisMode() == vtkMRMLViewNode::RotateAround )
         {
//         this->MainViewRotateAround ( vtkMRMLViewNode::RollRight );
         this->MainViewRotateAround ( "A" );
         }
       else
         {
         this->MainViewLookFrom ("P");
         }
       }
     }
}


//---------------------------------------------------------------------------
void vtkSlicerViewControlGUI::ViewControlSCallback ( )
{
   if ( this->ApplicationGUI)
     {
     vtkMRMLViewNode *vn = this->GetActiveView();
     if ( vn != NULL )  
       {
       if ( vn->GetViewAxisMode() == vtkMRMLViewNode::RotateAround )
         {
//         this->MainViewRotateAround ( vtkMRMLViewNode::YawLeft );
         this->MainViewRotateAround ( "S" );
         }
       else
         {
         this->MainViewLookFrom ("S");
         }
       }
     }
}


//---------------------------------------------------------------------------
void vtkSlicerViewControlGUI::ViewControlICallback ( )
{
  if ( this->ApplicationGUI)
    {
    vtkMRMLViewNode *vn = this->GetActiveView();
    if ( vn != NULL )  
      {

      if ( vn->GetViewAxisMode() == vtkMRMLViewNode::RotateAround )
        {
//        this->MainViewRotateAround ( vtkMRMLViewNode::YawRight );
         this->MainViewRotateAround ( "S" );
        }
      else
        {
        this->MainViewLookFrom ("I");
        }
      }
    }
}


//---------------------------------------------------------------------------
void vtkSlicerViewControlGUI::ViewControlRCallback ( )
{
  if ( this->ApplicationGUI)
    {
    vtkMRMLViewNode *vn = this->GetActiveView();
    if ( vn != NULL )  
      {

      if ( vn->GetViewAxisMode() == vtkMRMLViewNode::RotateAround )
        {
//        this->MainViewRotateAround ( vtkMRMLViewNode::PitchUp );
         this->MainViewRotateAround ( "R" );
        }
      else
        {
        this->MainViewLookFrom ("R");
        }
      }
    }
}


//---------------------------------------------------------------------------
void vtkSlicerViewControlGUI::ViewControlLCallback ( )
{
  if ( this->ApplicationGUI)
    {
    vtkMRMLViewNode *vn = this->GetActiveView();
    if ( vn != NULL )  
      {
      if ( vn->GetViewAxisMode() == vtkMRMLViewNode::RotateAround )
        {
//        this->MainViewRotateAround (vtkMRMLViewNode::PitchDown);
         this->MainViewRotateAround ( "R" );
        }
      else
        {
        this->MainViewLookFrom ("L");
        }
      }
    }
}


//---------------------------------------------------------------------------
void vtkSlicerViewControlGUI::EnterViewAxisACallback ( ) {
  if ( this->ApplicationGUI)
    {
    vtkMRMLViewNode *vn = this->GetActiveView();
    if ( vn != NULL )  
      {

      if (vn->GetViewAxisMode() == vtkMRMLViewNode::RotateAround )
        {
        this->ViewAxisPIconButton->SetImageToIcon (this->SlicerViewControlIcons->GetViewAxisPIconHI() );
        this->ViewAxisAIconButton->SetImageToIcon (this->SlicerViewControlIcons->GetViewAxisAIconHI() );
        }
      else
        {
        this->ViewAxisAIconButton->SetImageToIcon (this->SlicerViewControlIcons->GetViewAxisAIconHI() );
        }
      }
    }

}

//---------------------------------------------------------------------------
void vtkSlicerViewControlGUI::LeaveViewAxisACallback ( ) {
  this->ViewAxisPIconButton->SetImageToIcon (this->SlicerViewControlIcons->GetViewAxisPIconLO() );
  this->ViewAxisAIconButton->SetImageToIcon (this->SlicerViewControlIcons->GetViewAxisAIconLO() );
}

//---------------------------------------------------------------------------
void vtkSlicerViewControlGUI::EnterViewAxisPCallback ( ) {
  if ( this->ApplicationGUI)
    {
    vtkMRMLViewNode *vn = this->GetActiveView();
    if ( vn != NULL )  
      {

      if ( vn->GetViewAxisMode() == vtkMRMLViewNode::RotateAround )
        {
        this->ViewAxisPIconButton->SetImageToIcon (this->SlicerViewControlIcons->GetViewAxisPIconHI() );
        this->ViewAxisAIconButton->SetImageToIcon (this->SlicerViewControlIcons->GetViewAxisAIconHI() );
        }
      else
        {
        this->ViewAxisPIconButton->SetImageToIcon (this->SlicerViewControlIcons->GetViewAxisPIconHI() );
        }
      }
    }
}

//---------------------------------------------------------------------------
void vtkSlicerViewControlGUI::LeaveViewAxisPCallback ( ) {
  this->ViewAxisPIconButton->SetImageToIcon (this->SlicerViewControlIcons->GetViewAxisPIconLO() );
  this->ViewAxisAIconButton->SetImageToIcon (this->SlicerViewControlIcons->GetViewAxisAIconLO() );
}

//---------------------------------------------------------------------------
void vtkSlicerViewControlGUI::EnterViewAxisRCallback ( ) {
  if ( this->ApplicationGUI)
    {
    vtkMRMLViewNode *vn = this->GetActiveView();
    if ( vn != NULL )  
      {

      if ( vn->GetViewAxisMode() == vtkMRMLViewNode::RotateAround )
        {
        this->ViewAxisRIconButton->SetImageToIcon (this->SlicerViewControlIcons->GetViewAxisRIconHI() );
        this->ViewAxisLIconButton->SetImageToIcon (this->SlicerViewControlIcons->GetViewAxisLIconHI() );
        }
      else
        {
        this->ViewAxisRIconButton->SetImageToIcon (this->SlicerViewControlIcons->GetViewAxisRIconHI() );
        }
      }
    }

}

//---------------------------------------------------------------------------
void vtkSlicerViewControlGUI::LeaveViewAxisRCallback ( ) {
  this->ViewAxisRIconButton->SetImageToIcon (this->SlicerViewControlIcons->GetViewAxisRIconLO() );
  this->ViewAxisLIconButton->SetImageToIcon (this->SlicerViewControlIcons->GetViewAxisLIconLO() );
}

//---------------------------------------------------------------------------
void vtkSlicerViewControlGUI::EnterViewAxisLCallback ( ) {
  if ( this->ApplicationGUI)
    {
    vtkMRMLViewNode *vn = this->GetActiveView();
    if ( vn != NULL )  
      {

      if ( vn->GetViewAxisMode() == vtkMRMLViewNode::RotateAround )
        {
        this->ViewAxisRIconButton->SetImageToIcon (this->SlicerViewControlIcons->GetViewAxisRIconHI() );
        this->ViewAxisLIconButton->SetImageToIcon (this->SlicerViewControlIcons->GetViewAxisLIconHI() );
        }
      else
        {
        this->ViewAxisLIconButton->SetImageToIcon (this->SlicerViewControlIcons->GetViewAxisLIconHI() );
        }
      }
    }

}

//---------------------------------------------------------------------------
void vtkSlicerViewControlGUI::LeaveViewAxisLCallback ( ) {
  this->ViewAxisRIconButton->SetImageToIcon (this->SlicerViewControlIcons->GetViewAxisRIconLO() );
  this->ViewAxisLIconButton->SetImageToIcon (this->SlicerViewControlIcons->GetViewAxisLIconLO() );
}

//---------------------------------------------------------------------------
void vtkSlicerViewControlGUI::EnterViewAxisSCallback ( ) {
  if ( this->ApplicationGUI)
    {
    vtkMRMLViewNode *vn = this->GetActiveView();
    if ( vn != NULL )  
      {

      if ( vn->GetViewAxisMode() == vtkMRMLViewNode::RotateAround )
        {
        this->ViewAxisSIconButton->SetImageToIcon (this->SlicerViewControlIcons->GetViewAxisSIconHI() );
        this->ViewAxisIIconButton->SetImageToIcon (this->SlicerViewControlIcons->GetViewAxisIIconHI() );
        }
      else
        {
        this->ViewAxisSIconButton->SetImageToIcon (this->SlicerViewControlIcons->GetViewAxisSIconHI() );
        }
      }
    }
}

//---------------------------------------------------------------------------
void vtkSlicerViewControlGUI::LeaveViewAxisSCallback ( ) {
  this->ViewAxisSIconButton->SetImageToIcon (this->SlicerViewControlIcons->GetViewAxisSIconLO() );
  this->ViewAxisIIconButton->SetImageToIcon (this->SlicerViewControlIcons->GetViewAxisIIconLO() );
}

//---------------------------------------------------------------------------
void vtkSlicerViewControlGUI::EnterViewAxisICallback ( ) {
  if ( this->ApplicationGUI)
    {
    vtkMRMLViewNode *vn = this->GetActiveView();
    if ( vn != NULL )  
      {

      if ( vn->GetViewAxisMode() == vtkMRMLViewNode::RotateAround )
        {
        this->ViewAxisIIconButton->SetImageToIcon (this->SlicerViewControlIcons->GetViewAxisIIconHI() );
        this->ViewAxisSIconButton->SetImageToIcon (this->SlicerViewControlIcons->GetViewAxisSIconHI() );
        }
      else
        {
        this->ViewAxisIIconButton->SetImageToIcon (this->SlicerViewControlIcons->GetViewAxisIIconHI() );
        }
      }
    }
}

//---------------------------------------------------------------------------
void vtkSlicerViewControlGUI::LeaveViewAxisICallback ( ) {
  this->ViewAxisIIconButton->SetImageToIcon (this->SlicerViewControlIcons->GetViewAxisIIconLO() );
  this->ViewAxisSIconButton->SetImageToIcon (this->SlicerViewControlIcons->GetViewAxisSIconLO() );
}


//---------------------------------------------------------------------------
void vtkSlicerViewControlGUI::PackNavigationWidget ( )
{
  if ( this->ZoomWidget != NULL )
    {
    this->Script ("pack forget %s ", this->ZoomWidget->GetWidgetName() );
    }
  if ( this->NavigationWidget != NULL )
    {
    this->Script ( "pack %s -side top -anchor c -padx 0 -pady 0 -fill x -fill y -expand n", this->NavigationWidget->GetWidgetName ( ) );
    }
}



//---------------------------------------------------------------------------
void vtkSlicerViewControlGUI::PackZoomWidget ( )
{
  if ( this->NavigationWidget != NULL )
    {
    this->Script ("pack forget %s ", this->NavigationWidget->GetWidgetName() );
    }
  if ( this->ZoomWidget != NULL )
    {
    this->Script ( "pack %s -side top -anchor c -padx 0 -pady 0 -fill x -fill y -expand n", this->ZoomWidget->GetWidgetName ( ) );
    }
}



//---------------------------------------------------------------------------
void vtkSlicerViewControlGUI::ReconfigureGUIFonts ( )
{
}


//---------------------------------------------------------------------------
void vtkSlicerViewControlGUI::BuildGUI ( vtkKWFrame *appF )
{

  vtkSlicerApplicationGUI *p = this->GetApplicationGUI ( );  
  // populate the application's 3DView control GUI panel
  if ( p != NULL )
    {
    if ( p->GetApplication() != NULL )
      {
      vtkSlicerApplication *app = vtkSlicerApplication::SafeDownCast( p->GetApplication() );

      this->SlicerViewControlIcons = vtkSlicerViewControlIcons::New ( );

      this->NameDialog  = vtkKWSimpleEntryDialog::New();
      this->NameDialog->SetParent ( appF );
      this->NameDialog->SetTitle("Scene Snapshot Name");
      this->NameDialog->SetSize(400, 200);
      this->NameDialog->SetStyleToOkCancel();
      vtkKWEntryWithLabel *entry = this->NameDialog->GetEntry();
      entry->SetLabelText("Snapshot Name");
      entry->GetWidget()->SetValue("");
      this->NameDialog->Create ( );
      
      this->SpinButton = vtkKWCheckButton::New ( );
      this->RockButton = vtkKWCheckButton::New ( );
      this->OrthoButton = vtkKWPushButton::New ( );

      this->CenterButton = vtkKWPushButton::New ( );
      this->StereoButton = vtkKWMenuButton::New ( );
      this->ScreenGrabButton = vtkKWPushButton::New ( );
      this->VisibilityButton = vtkKWMenuButton::New ( );
      this->SceneSnapshotButton = vtkKWPushButton::New();
      this->SelectSceneSnapshotMenuButton = vtkKWMenuButton::New();

      this->PitchButton = vtkKWPushButton::New();
      this->RollButton = vtkKWPushButton::New();
      this->YawButton = vtkKWPushButton::New();
      this->ZoomInButton = vtkKWPushButton::New();
      this->ZoomOutButton = vtkKWPushButton::New();

      //--- ui for the ViewControlFrame
      this->ViewAxisAIconButton = vtkKWLabel::New ( );
      this->ViewAxisPIconButton = vtkKWLabel::New ( );
      this->ViewAxisRIconButton = vtkKWLabel::New ( );
      this->ViewAxisLIconButton = vtkKWLabel::New ( );
      this->ViewAxisSIconButton = vtkKWLabel::New ( );
      this->ViewAxisIIconButton = vtkKWLabel::New ( );
      this->ViewAxisCenterIconButton = vtkKWLabel::New ( );
      this->ViewAxisTopCornerIconButton = vtkKWLabel::New ( );
      this->ViewAxisBottomCornerIconButton = vtkKWLabel::New ( );

      // temporary thing until navzoom window is built.
      this->NavigationWidget = vtkKWRenderWidget::New( );
      this->ZoomWidget = vtkKWRenderWidget::New ( );
      this->NavigationZoomFrame = vtkKWFrame::New ( );

      //--- NEW
      vtkKWFrame *frameL = vtkKWFrame::New();
      vtkKWFrame *frameM = vtkKWFrame::New();
      vtkKWFrame *frameR = vtkKWFrame::New();

      frameL->SetParent (appF);
      frameL->Create();
      frameM->SetParent (appF);
      frameM->Create();
      frameR->SetParent (appF);
      frameR->Create();
      this->Script ( "pack %s %s %s -side left -expand n -padx 3 -pady 3",
                     frameL->GetWidgetName(),
                     frameM->GetWidgetName(),
                     frameR->GetWidgetName() );

      
      // create and pack the look from and rotate around checkbuttons

      // create and pack rollover labels for rotate around and look from camera control
      this->ViewAxisAIconButton->SetParent ( frameL);
      this->ViewAxisAIconButton->Create ( );
      this->ViewAxisAIconButton->SetBorderWidth ( 0 );
      this->ViewAxisAIconButton->SetImageToIcon ( this->SlicerViewControlIcons->GetViewAxisAIconLO() );
      this->ViewAxisAIconButton->SetBalloonHelpString ("Look from A toward origin.");

      this->ViewAxisPIconButton->SetParent ( frameL );
      this->ViewAxisPIconButton->Create ( );
      this->ViewAxisPIconButton->SetBorderWidth ( 0 );
      this->ViewAxisPIconButton->SetImageToIcon ( this->SlicerViewControlIcons->GetViewAxisPIconLO() );
      this->ViewAxisPIconButton->SetBalloonHelpString ("Look from P toward origin.");

      this->ViewAxisRIconButton->SetParent ( frameL );
      this->ViewAxisRIconButton->Create ( );
      this->ViewAxisRIconButton->SetBorderWidth ( 0 );
      this->ViewAxisRIconButton->SetImageToIcon ( this->SlicerViewControlIcons->GetViewAxisRIconLO() );
      this->ViewAxisRIconButton->SetBalloonHelpString ("Look from R toward origin.");

      this->ViewAxisLIconButton->SetParent ( frameL );
      this->ViewAxisLIconButton->Create ( );
      this->ViewAxisLIconButton->SetBorderWidth ( 0 );
      this->ViewAxisLIconButton->SetImageToIcon ( this->SlicerViewControlIcons->GetViewAxisLIconLO() );
      this->ViewAxisLIconButton->SetBalloonHelpString ("Look from L toward origin.");

      this->ViewAxisSIconButton->SetParent ( frameL );      
      this->ViewAxisSIconButton->Create ( );
      this->ViewAxisSIconButton->SetBorderWidth ( 0 );
      this->ViewAxisSIconButton->SetImageToIcon ( this->SlicerViewControlIcons->GetViewAxisSIconLO() );
      this->ViewAxisSIconButton->SetBalloonHelpString ("Look from S toward origin.");

      this->ViewAxisIIconButton->SetParent ( frameL );
      this->ViewAxisIIconButton->Create ( );
      this->ViewAxisIIconButton->SetBorderWidth ( 0 );
      this->ViewAxisIIconButton->SetImageToIcon ( this->SlicerViewControlIcons->GetViewAxisIIconLO() );
      this->ViewAxisIIconButton->SetBalloonHelpString ("Look from I toward origin.");

      this->ViewAxisCenterIconButton->SetParent ( frameL );
      this->ViewAxisCenterIconButton->Create ( );
      this->ViewAxisCenterIconButton->SetBorderWidth ( 0 );
      this->ViewAxisCenterIconButton->SetImageToIcon ( this->SlicerViewControlIcons->GetViewAxisCenterIcon() );

      this->ViewAxisTopCornerIconButton->SetParent ( frameL );
      this->ViewAxisTopCornerIconButton->Create ( );
      this->ViewAxisTopCornerIconButton->SetBorderWidth ( 0 );
      this->ViewAxisTopCornerIconButton->SetImageToIcon ( this->SlicerViewControlIcons->GetViewAxisTopCornerIcon() );

      this->ViewAxisBottomCornerIconButton->SetParent ( frameL );
      this->ViewAxisBottomCornerIconButton->Create ( );
      this->ViewAxisBottomCornerIconButton->SetBorderWidth ( 0 );
      this->ViewAxisBottomCornerIconButton->SetImageToIcon ( this->SlicerViewControlIcons->GetViewAxisBottomCornerIcon() );
      this->Script ("grid %s -row 0 -column 0 -sticky sw -padx 0 -pady 0 -ipadx 0 -ipady 0", this->ViewAxisPIconButton->GetWidgetName ( ));
      this->Script ("grid %s -row 0 -column 1 -sticky sw -padx 0 -pady 0 -ipadx 0 -ipady 0", this->ViewAxisSIconButton->GetWidgetName ( ));
      this->Script ("grid %s -row 0 -column 2 -sticky sw -padx 0 -pady 0 -ipadx 0 -ipady 0",this->ViewAxisTopCornerIconButton->GetWidgetName ( ) );
      this->Script ("grid %s -row 1 -column 0 -sticky w -padx 0 -pady 0 -ipadx 0 -ipady 0", this->ViewAxisRIconButton->GetWidgetName ( ) );
      this->Script ("grid %s -row 1 -column 1 -sticky w -padx 0 -pady 0 -ipadx 0 -ipady 0", this->ViewAxisCenterIconButton->GetWidgetName ( ));
      this->Script ("grid %s -row 1 -column 2 -sticky w -padx 0 -pady 0 -ipadx 0 -ipady 0", this->ViewAxisLIconButton->GetWidgetName ( ) );
      this->Script ("grid %s -row 2 -column 0 -sticky nw -padx 0 -pady 0 -ipadx 0 -ipady 0", this->ViewAxisBottomCornerIconButton->GetWidgetName ( ));
      this->Script ("grid %s -row 2 -column 1 -sticky nw -padx 0 -pady 0 -ipadx 0 -ipady 0", this->ViewAxisIIconButton->GetWidgetName ( ) );
      this->Script ("grid %s -row 2 -column 2 -sticky nw -padx 0 -pady 0 -ipadx 0 -ipady 0", this->ViewAxisAIconButton->GetWidgetName ( ) );
      this->MakeViewControlRolloverBehavior ( );

      this->PitchButton->SetParent ( frameM );
      this->PitchButton->Create();
      this->PitchButton->SetReliefToFlat();
      this->PitchButton->SetBorderWidth ( 0 );
      this->PitchButton->SetOverReliefToNone ( );
      this->PitchButton->SetImageToIcon ( this->SlicerViewControlIcons->GetPitchIcon() );
      this->PitchButton->SetBalloonHelpString ( "Pitch the view of the scene." );
      
      this->RollButton->SetParent ( frameM );
      this->RollButton->Create();
      this->RollButton->SetReliefToFlat();
      this->RollButton->SetBorderWidth ( 0 );
      this->RollButton->SetOverReliefToNone ( );
      this->RollButton->SetImageToIcon ( this->SlicerViewControlIcons->GetRollIcon() );
      this->RollButton->SetBalloonHelpString ( "Roll the view of the scene." );

      this->YawButton->SetParent ( frameM );
      this->YawButton->Create();
      this->YawButton->SetReliefToFlat();
      this->YawButton->SetBorderWidth ( 0 );
      this->YawButton->SetOverReliefToNone ( );
      this->YawButton->SetImageToIcon ( this->SlicerViewControlIcons->GetYawIcon() );
      this->YawButton->SetBalloonHelpString ( "Yaw the view of the scene." );

      this->ZoomInButton->SetParent ( frameM );
      this->ZoomInButton->Create();
      this->ZoomInButton->SetReliefToFlat();
      this->ZoomInButton->SetBorderWidth ( 0 );
      this->ZoomInButton->SetOverReliefToNone ( );
      this->ZoomInButton->SetImageToIcon ( this->SlicerViewControlIcons->GetZoomInIcon() );
      this->ZoomInButton->SetBalloonHelpString ( "Zoom in on the scene by a small amount." );

      this->ZoomOutButton->SetParent ( frameM );
      this->ZoomOutButton->Create();
      this->ZoomOutButton->SetReliefToFlat();
      this->ZoomOutButton->SetBorderWidth ( 0 );
      this->ZoomOutButton->SetOverReliefToNone ( );
      this->ZoomOutButton->SetImageToIcon ( this->SlicerViewControlIcons->GetZoomOutIcon() );
      this->ZoomOutButton->SetBalloonHelpString ( "Zoom out of the scene by a small amount." );

      //--- Push button to toggle between perspective and ortho rendering
      this->OrthoButton->SetParent ( frameM);
      this->OrthoButton->Create ( );
      this->OrthoButton->SetReliefToFlat ( );
      this->OrthoButton->SetBorderWidth ( 0 );
      this->OrthoButton->SetOverReliefToNone ( );
      this->OrthoButton->SetImageToIcon ( this->SlicerViewControlIcons->GetOrthoButtonIcon() );      
      this->OrthoButton->SetBalloonHelpString ( "Toggle between orthographic and perspective rendering in the 3D view.");
      //--- Pusbbutton to center the rendered vie on the scene center
      this->CenterButton->SetParent ( frameM);
      this->CenterButton->Create ( );
      this->CenterButton->SetReliefToFlat ( );
      this->CenterButton->SetBorderWidth ( 0 );
      this->CenterButton->SetOverReliefToNone ( );
      this->CenterButton->SetImageToIcon ( this->SlicerViewControlIcons->GetCenterButtonIcon() );      
      this->CenterButton->SetBalloonHelpString ( "Center the 3D view on the scene.");
      //--- Menubutton to show stereo options
      this->StereoButton->SetParent ( frameM);
      this->StereoButton->Create ( );
      this->StereoButton->SetReliefToFlat ( );
      this->StereoButton->SetBorderWidth ( 0 );
      this->StereoButton->SetImageToIcon ( this->SlicerViewControlIcons->GetStereoButtonIcon() );      
      this->StereoButton->IndicatorVisibilityOff ( );
      this->StereoButton->SetBalloonHelpString ( "Select among stereo viewing options (3DSlicer must be started with stereo enabled to use these features).");
      //--- Menubutton to capture or select among saved 3D views.
      this->ScreenGrabButton->SetParent ( frameM);
      this->ScreenGrabButton->Create ( );
      this->ScreenGrabButton->SetReliefToFlat ( );
      this->ScreenGrabButton->SetBorderWidth ( 0 );
      this->ScreenGrabButton->SetOverReliefToNone();
      this->ScreenGrabButton->SetImageToIcon ( this->SlicerViewControlIcons->GetScreenCaptureButtonIcon() );
      this->ScreenGrabButton->SetBalloonHelpString ( "Capture a screenshot of the 3D view.");
      //--- MenuButton to select among saved scene snapshots
      this->SelectSceneSnapshotMenuButton->SetParent ( frameM);
      this->SelectSceneSnapshotMenuButton->Create ( );
      this->SelectSceneSnapshotMenuButton->SetReliefToFlat ( );
      this->SelectSceneSnapshotMenuButton->SetBorderWidth ( 0 );
      this->SelectSceneSnapshotMenuButton->SetImageToIcon ( this->SlicerViewControlIcons->GetSelectSceneSnapshotIcon() );
      this->SelectSceneSnapshotMenuButton->IndicatorVisibilityOff ( );
      this->SelectSceneSnapshotMenuButton->SetBalloonHelpString ( "Restore or delete saved scene snapshots.");
      this->SelectSceneSnapshotMenuButton->SetBinding ( "<Button-1>", this, "UpdateSceneSnapshotsFromMRML");
      
      //--- Pushbutton to take a scene snapshot.
      this->SceneSnapshotButton->SetParent ( frameM);
      this->SceneSnapshotButton->Create();
      this->SceneSnapshotButton->SetReliefToFlat();
      this->SceneSnapshotButton->SetBorderWidth ( 0 );
      this->SceneSnapshotButton->SetImageToIcon ( p->GetSlicerFoundationIcons()->GetSlicerCameraIcon () );
      this->SceneSnapshotButton->SetBalloonHelpString ( "Capture and name a scene snapshot." );

      //--- Checkbutton to spin the view
      this->SpinButton->SetParent ( frameM );
      this->SpinButton->Create ( );
      this->SpinButton->SetImageToIcon ( this->SlicerViewControlIcons->GetSpinOffButtonIcon() );      
      this->SpinButton->SetSelectImageToIcon ( this->SlicerViewControlIcons->GetSpinOnButtonIcon() );      
      this->SpinButton->Deselect();
      this->SpinButton->SetBalloonHelpString ( "Spin the 3D view.");
      //--- CheckButton to rotate the view
      this->RockButton->SetParent ( frameM );
      this->RockButton->Create ( );
      this->RockButton->SetImageToIcon ( this->SlicerViewControlIcons->GetRockOffButtonIcon() );      
      this->RockButton->SetSelectImageToIcon ( this->SlicerViewControlIcons->GetRockOnButtonIcon() );      
      this->RockButton->SetBalloonHelpString ( "Rock the 3D view.");
      this->RockButton->Deselect();
      //--- Menubutton to turn on/off axes, cube, outlines, annotations in 3D view.
      this->VisibilityButton->SetParent (frameM);
      this->VisibilityButton->Create ( );
      this->VisibilityButton->IndicatorVisibilityOff ( );
      this->VisibilityButton->SetBorderWidth ( 0 );
      this->VisibilityButton->SetImageToIcon ( this->SlicerViewControlIcons->GetVisibilityButtonIcon ( ) );
      this->VisibilityButton->SetBalloonHelpString ("Toggle visibility of elements in the 3D view." );

      this->Script ("grid rowconfigure %s 0 -weight 0", frameM->GetWidgetName() );
      this->Script ("grid rowconfigure %s 1 -weight 0", frameM->GetWidgetName() );
      this->Script ("grid rowconfigure %s 2 -weight 0", frameM->GetWidgetName() );
      this->Script ("grid columnconfigure %s 0 -weight 0", frameM->GetWidgetName() );
      this->Script ("grid columnconfigure %s 1 -weight 0", frameM->GetWidgetName() );
      this->Script ("grid columnconfigure %s 2 -weight 0", frameM->GetWidgetName() );
      this->Script ("grid columnconfigure %s 3 -weight 0", frameM->GetWidgetName() );
      this->Script ("grid columnconfigure %s 4 -weight 0", frameM->GetWidgetName() );

      this->Script ("grid %s -row 0 -column 0 -sticky w -padx 2 -pady 0 -ipadx 0 -ipady 0", this->PitchButton->GetWidgetName ( ));
      this->Script ("grid %s -row 1 -column 0 -sticky w -padx 2 -pady 0 -ipadx 0 -ipady 0", this->RollButton->GetWidgetName ( ));
      this->Script ("grid %s -row 2 -column 0 -sticky w -padx 2 -pady 0 -ipadx 0 -ipady 0", this->YawButton->GetWidgetName ( ));

      this->Script ("grid %s -row 0 -column 1 -sticky w -padx 2 -pady 0 -ipadx 0 -ipady 0", this->CenterButton->GetWidgetName ( ));
      this->Script ("grid %s -row 1 -column 1 -sticky w -padx 2 -pady 0 -ipadx 0 -ipady 0", this->OrthoButton->GetWidgetName ( ));
      this->Script ("grid %s -row 2 -column 1 -sticky w -padx 2 -pady 0 -ipadx 0 -ipady 0", this->VisibilityButton->GetWidgetName ( ));      
      
      this->Script ("grid %s -row 0 -column 2 -sticky w -padx 2 -pady 0 -ipadx 0 -ipady 0", this->ScreenGrabButton->GetWidgetName ( ));
      this->Script ("grid %s -row 1 -column 2 -sticky w -padx 2 -pady 0 -ipadx 0 -ipady 0", this->SceneSnapshotButton->GetWidgetName ( ));
      this->Script ("grid %s -row 2 -column 2 -sticky w -padx 2 -pady 0 -ipadx 0 -ipady 0", this->SelectSceneSnapshotMenuButton->GetWidgetName ( ));      

      this->Script ("grid %s -row 0 -column 3 -sticky w -padx 2 -pady 0 -ipadx 0 -ipady 0", this->ZoomInButton->GetWidgetName ( ));
      this->Script ("grid %s -row 1 -column 3 -sticky w -padx 2 -pady 0 -ipadx 0 -ipady 0", this->ZoomOutButton->GetWidgetName ( ));
      this->Script ("grid %s -row 2 -column 3 -columnspan 2 -sticky w -padx 2 -pady 0 -ipadx 0 -ipady 0", this->StereoButton->GetWidgetName ( ));
      
      this->Script ("grid %s -row 0 -column 4 -sticky e -padx 4 -pady 0 -ipadx 0 -ipady 0", this->SpinButton->GetWidgetName ( ));
      this->Script ("grid %s -row 1 -column 4 -sticky e -padx 4 -pady 0 -ipadx 0 -ipady 0", this->RockButton->GetWidgetName ( ));

      //--- create the nav/zoom widgets
      this->ZoomWidget->SetParent ( frameR );
      this->ZoomWidget->Create ( );
      this->ZoomWidget->SetWidth ( this->NavigationZoomWidgetWid );
      this->ZoomWidget->SetHeight ( this->NavigationZoomWidgetHit );
      this->ZoomWidget->SetRendererBackgroundColor ( app->GetSlicerTheme()->GetSlicerColors()->Black );
      this->ZoomWidget->GetRenderWindow()->AddRenderer(this->ZoomWidget->GetRenderer() );
      
      this->SliceMagnifier = vtkSlicerImageCloseUp2D::New();
      this->SliceMagnifier->SetHalfWidth (7 );
      this->SliceMagnifier->SetHalfHeight ( 4 );
      this->SliceMagnifier->SetMagnification ( 10 );
      this->SliceMagnifier->SetInput ( NULL );
      
      this->SliceMagnifierCursor = vtkSlicerImageCrossHair2D::New();
      this->SliceMagnifierCursor->SetCursor (75, 40);
      this->SliceMagnifierCursor->BullsEyeOn();
      this->SliceMagnifierCursor->ShowCursorOn();
      this->SliceMagnifierCursor->IntersectCrossOff();
      this->SliceMagnifierCursor->SetCursorColor ( 1.0, 0.75, 0.0);
      this->SliceMagnifierCursor->SetNumHashes ( 0 );
      this->SliceMagnifierCursor->SetHashLength ( 6 );
      this->SliceMagnifierCursor->SetHashGap ( 10 );
      this->SliceMagnifierCursor->SetMagnification ( 1.0 );      
      this->SliceMagnifierCursor->SetInput ( NULL );

      this->SliceMagnifierMapper = vtkImageMapper::New();
      this->SliceMagnifierMapper->SetColorWindow(255);
      this->SliceMagnifierMapper->SetColorLevel (127.5);
      this->SliceMagnifierMapper->SetInput ( NULL);
      this->SliceMagnifierActor = vtkActor2D::New();
      this->SliceMagnifierActor->SetMapper ( NULL );

      this->CreateFieldOfViewBoxActor ( );

      this->NavigationWidget->SetParent (frameR);
      this->NavigationWidget->Create();
      this->NavigationWidget->SetWidth ( this->NavigationZoomWidgetWid );
      this->NavigationWidget->SetHeight ( this->NavigationZoomWidgetHit );
      this->NavigationWidget->SetRendererBackgroundColor ( app->GetSlicerTheme()->GetSlicerColors()->ViewerBlue );
      this->NavigationWidget->GetRenderWindow()->AddRenderer(this->NavigationWidget->GetRenderer() );
      this->NavigationWidget->GetRenderWindow()->DoubleBufferOn();
      this->NavigationWidget->GetRenderer()->GetRenderWindow()->GetInteractor()->Disable();

      // TODO: Why does the display flash the first time we 'pack forget' and then 'pack'?
      // pack zoom widget temporarily, then swap in navigation widget.
      this->Script ( "pack %s -side top -anchor c -padx 0 -pady 0 -fill x -fill y -expand n", this->ZoomWidget->GetWidgetName ( ) );      
      this->PackNavigationWidget ( );
      
      // populate menus
      this->BuildStereoSelectMenu ( );
      this->BuildVisibilityMenu ( );

      //Create the Enable/Disable BUtton for the navigation widget but don't pack it
      this->EnableDisableNavButton=vtkKWCheckButton::New();
      this->EnableDisableNavButton->SetParent(frameR);
      this->EnableDisableNavButton->Create();
      this->EnableDisableNavButton->SelectedStateOn();
    
      frameL->Delete();
      frameM->Delete();
      frameR->Delete();

      // Observe an event that updates the status text during loading of data.
      vtkIntArray *events = vtkIntArray::New();
      events->InsertNextValue( vtkMRMLScene::NodeAddedEvent );
      events->InsertNextValue( vtkMRMLScene::NodeRemovedEvent );
      events->InsertNextValue( vtkMRMLScene::SceneCloseEvent );
      events->InsertNextValue( vtkMRMLScene::NewSceneEvent );
      this->SetAndObserveMRMLSceneEvents (this->MRMLScene, events );
      events->Delete();
      }
    }
}



//---------------------------------------------------------------------------
void vtkSlicerViewControlGUI::AddSliceEventObservers()
{
  
  if ( this->GetApplicationGUI() != NULL )
    {
    if ( this->RedSliceEvents != NULL )
      {
      this->RedSliceEvents->AddObserver ( vtkCommand::EnterEvent, this->GUICallbackCommand );
      this->RedSliceEvents->AddObserver ( vtkCommand::LeaveEvent, this->GUICallbackCommand );
      this->RedSliceEvents->AddObserver ( vtkCommand::MouseMoveEvent, this->GUICallbackCommand );
      this->RedSliceEvents->AddObserver ( vtkCommand::RightButtonPressEvent, this->GUICallbackCommand );
      this->RedSliceEvents->AddObserver ( vtkCommand::RightButtonReleaseEvent, this->GUICallbackCommand );
      this->RedSliceEvents->AddObserver ( vtkCommand::MiddleButtonPressEvent, this->GUICallbackCommand );
      this->RedSliceEvents->AddObserver ( vtkCommand::MiddleButtonReleaseEvent, this->GUICallbackCommand );
      }
    if ( this->YellowSliceEvents != NULL )
      {
      this->YellowSliceEvents->AddObserver ( vtkCommand::EnterEvent, this->GUICallbackCommand );
      this->YellowSliceEvents->AddObserver ( vtkCommand::LeaveEvent, this->GUICallbackCommand );
      this->YellowSliceEvents->AddObserver ( vtkCommand::MouseMoveEvent, this->GUICallbackCommand );
      this->YellowSliceEvents->AddObserver ( vtkCommand::RightButtonPressEvent, this->GUICallbackCommand );
      this->YellowSliceEvents->AddObserver ( vtkCommand::RightButtonReleaseEvent, this->GUICallbackCommand );
      this->YellowSliceEvents->AddObserver ( vtkCommand::MiddleButtonPressEvent, this->GUICallbackCommand );
      this->YellowSliceEvents->AddObserver ( vtkCommand::MiddleButtonReleaseEvent, this->GUICallbackCommand );
      }
    if ( this->GreenSliceEvents != NULL )
      {
      this->GreenSliceEvents->AddObserver ( vtkCommand::EnterEvent, this->GUICallbackCommand );
      this->GreenSliceEvents->AddObserver ( vtkCommand::LeaveEvent, this->GUICallbackCommand );
      this->GreenSliceEvents->AddObserver ( vtkCommand::MouseMoveEvent, this->GUICallbackCommand );
      this->GreenSliceEvents->AddObserver ( vtkCommand::RightButtonPressEvent, this->GUICallbackCommand );
      this->GreenSliceEvents->AddObserver ( vtkCommand::RightButtonReleaseEvent, this->GUICallbackCommand );
      this->GreenSliceEvents->AddObserver ( vtkCommand::MiddleButtonPressEvent, this->GUICallbackCommand );
      this->GreenSliceEvents->AddObserver ( vtkCommand::MiddleButtonReleaseEvent, this->GUICallbackCommand );
      }
    }
}

//---------------------------------------------------------------------------
void vtkSlicerViewControlGUI::RemoveSliceEventObservers()
{
  if ( this->GetApplicationGUI() != NULL )
    {
    if ( this->RedSliceEvents != NULL )
      {
      this->RedSliceEvents->RemoveObservers ( vtkCommand::EnterEvent, this->GUICallbackCommand );
      this->RedSliceEvents->RemoveObservers ( vtkCommand::LeaveEvent, this->GUICallbackCommand );
      this->RedSliceEvents->RemoveObservers ( vtkCommand::MouseMoveEvent, this->GUICallbackCommand );
      this->RedSliceEvents->RemoveObservers ( vtkCommand::RightButtonPressEvent, this->GUICallbackCommand );
      this->RedSliceEvents->RemoveObservers ( vtkCommand::RightButtonReleaseEvent, this->GUICallbackCommand );
      this->RedSliceEvents->RemoveObservers ( vtkCommand::MiddleButtonPressEvent, this->GUICallbackCommand );
      this->RedSliceEvents->RemoveObservers ( vtkCommand::MiddleButtonReleaseEvent, this->GUICallbackCommand );
      }
    if ( this->YellowSliceEvents != NULL )
      {
      this->YellowSliceEvents->RemoveObservers ( vtkCommand::EnterEvent, this->GUICallbackCommand );
      this->YellowSliceEvents->RemoveObservers ( vtkCommand::LeaveEvent, this->GUICallbackCommand );
      this->YellowSliceEvents->RemoveObservers ( vtkCommand::MouseMoveEvent, this->GUICallbackCommand );
      this->YellowSliceEvents->RemoveObservers ( vtkCommand::RightButtonPressEvent, this->GUICallbackCommand );
      this->YellowSliceEvents->RemoveObservers ( vtkCommand::RightButtonReleaseEvent, this->GUICallbackCommand );
      this->YellowSliceEvents->RemoveObservers ( vtkCommand::MiddleButtonPressEvent, this->GUICallbackCommand );
      this->YellowSliceEvents->RemoveObservers ( vtkCommand::MiddleButtonReleaseEvent, this->GUICallbackCommand );
      }
    if ( this->GreenSliceEvents != NULL )
      {
      this->GreenSliceEvents->RemoveObservers ( vtkCommand::EnterEvent, this->GUICallbackCommand );
      this->GreenSliceEvents->RemoveObservers ( vtkCommand::LeaveEvent, this->GUICallbackCommand );
      this->GreenSliceEvents->RemoveObservers ( vtkCommand::MouseMoveEvent, this->GUICallbackCommand );
      this->GreenSliceEvents->RemoveObservers ( vtkCommand::RightButtonPressEvent, this->GUICallbackCommand );
      this->GreenSliceEvents->RemoveObservers ( vtkCommand::RightButtonReleaseEvent, this->GUICallbackCommand );
      this->GreenSliceEvents->RemoveObservers ( vtkCommand::MiddleButtonPressEvent, this->GUICallbackCommand );
      this->GreenSliceEvents->RemoveObservers ( vtkCommand::MiddleButtonReleaseEvent, this->GUICallbackCommand );
      }
    }
}

//---------------------------------------------------------------------------
vtkMRMLViewNode *vtkSlicerViewControlGUI::GetActiveView ( )
{
  if (this->ApplicationGUI)
    {
    vtkMRMLViewNode *node = NULL;
    if (this->ApplicationGUI->GetActiveViewerWidget())
      {
      node = this->ApplicationGUI->GetActiveViewerWidget()->GetViewNode();
      }
    if (node && node->GetActive() && this->ViewNode != node)
      {
      this->SetViewNode(node);
      this->UpdateMainViewerInteractorStyles();
      }
    }
  return this->ViewNode;
}

//---------------------------------------------------------------------------
vtkMRMLCameraNode *vtkSlicerViewControlGUI::GetActiveCamera()
{
  if (this->ApplicationGUI)
    {
    vtkMRMLViewNode *vn = this->GetActiveView();
    if (vn != NULL)
      {
      std::vector<vtkMRMLNode*> cnodes;
      int nnodes = this->ApplicationGUI->GetMRMLScene()->GetNodesByClass(
        "vtkMRMLCameraNode", cnodes);
      for (int n = 0; n < nnodes; n++)
        {
        vtkMRMLCameraNode *node = vtkMRMLCameraNode::SafeDownCast(cnodes[n]);
        if (node && 
            node->GetActiveTag() && 
            !strcmp(node->GetActiveTag(), vn->GetID()))
          {
          return node;
          }
        }
      }
    }
  return NULL;
}

//---------------------------------------------------------------------------
void vtkSlicerViewControlGUI::UpdateMainViewerInteractorStyles ( )
{
  // get all views from the scene
  // and observe active view.
  if (this->SceneClosing)
    {
    return;
    }

  // Find current MainViewer; if there is none, do nothing.
  vtkSlicerViewerWidget* viewer_widget = 
    this->GetApplicationGUI()->GetActiveViewerWidget();
  if (  viewer_widget == NULL )
    {
    return;
    }
  if (  viewer_widget->GetMainViewer() == NULL )
    {
    return;
    }

  // If the interactor and these references are out of sync...
  if ( ( viewer_widget->GetMainViewer()->
         GetRenderWindowInteractor()->GetInteractorStyle() != this->MainViewerEvents ) )
    {
    // tear down pointer assignment
    this->RemoveMainViewerEventObservers();
    this->SetMainViewerEvents(NULL );
    // reassign pointer
    this->SetMainViewerEvents( vtkSlicerViewerInteractorStyle::SafeDownCast(
                                 viewer_widget->GetMainViewer()->GetRenderWindowInteractor()->GetInteractorStyle() ));
    // add observers on events
    this->AddMainViewerEventObservers();
    }
}

//---------------------------------------------------------------------------
void vtkSlicerViewControlGUI::CheckAbort(void)
{
  vtkSlicerViewerWidget* viewer_widget = 
    this->GetApplicationGUI()->GetActiveViewerWidget();
  int pending=viewer_widget  && viewer_widget->GetMainViewer()->GetRenderWindow()->GetEventPending();
  //int pendingGUI=viewer_widget && vtkKWTkUtilities::CheckForPendingInteractionEvents(viewer_widget->GetMainViewer()->GetRenderWindow());
    if(pending!=0)//||pendingGUI!=0)
    {
        this->NavigationWidget->GetRenderWindow()->SetAbortRender(1);
        return;
    }
}




//---------------------------------------------------------------------------
void vtkSlicerViewControlGUI::DestroyScreenGrabOptionsWindow ( )
{
  if ( !this->GetScreenGrabOptionsWindow() )
    {
    return;
    }
  if ( ! (this->GetScreenGrabOptionsWindow()->IsCreated()) )
    {
    vtkErrorMacro ( "DestroyScreenGrabOptionsWindow: ScreenGrabOptionsWindow is not created." );
    return;
    }
  vtkSlicerApplication *app = vtkSlicerApplication::SafeDownCast ( this->GetApplication() );
  if ( app )
    {
    app->Script ( "grab release %s", this->ScreenGrabOptionsWindow->GetWidgetName() );
    }
  this->ScreenGrabOptionsWindow->Withdraw();
  
  if ( this->ScreenGrabDialogButton )
    {
    this->ScreenGrabDialogButton->GetLoadSaveDialog()->RemoveObservers ( vtkKWTopLevel::WithdrawEvent, (vtkCommand *)this->GUICallbackCommand );
    this->ScreenGrabDialogButton->SetParent ( NULL );
    this->ScreenGrabDialogButton->Delete();
    this->ScreenGrabDialogButton = NULL;
    }
  if ( this->ScreenGrabNameEntry )
    {
    this->ScreenGrabNameEntry->SetParent ( NULL );
    this->ScreenGrabNameEntry->Delete();
    this->ScreenGrabNameEntry = NULL;
    }
  if ( this->ScreenGrabNumberEntry )
    {
    this->ScreenGrabNumberEntry->SetParent ( NULL );
    this->ScreenGrabNumberEntry->Delete();
    this->ScreenGrabNumberEntry = NULL;    
    }
  if ( this->ScreenGrabMagnificationEntry )
    {
    this->ScreenGrabMagnificationEntry->SetParent (NULL );
    this->ScreenGrabMagnificationEntry->Delete();
    this->ScreenGrabMagnificationEntry = NULL;    
    }
  if ( this->ScreenGrabFormatMenuButton )
    {
    this->ScreenGrabFormatMenuButton->GetMenu()->RemoveObservers ( vtkKWMenu::MenuItemInvokedEvent, (vtkCommand *) this->GUICallbackCommand );
    this->ScreenGrabFormatMenuButton->GetMenu()->DeleteAllItems();
    this->ScreenGrabFormatMenuButton->SetParent ( NULL );
    this->ScreenGrabFormatMenuButton->Delete();
    this->ScreenGrabFormatMenuButton = NULL;
    }
  if ( this->ScreenGrabOverwriteButton)
    {
    this->ScreenGrabOverwriteButton->RemoveObservers (vtkKWCheckButton::SelectedStateChangedEvent, (vtkCommand *)this->GUICallbackCommand );
    this->ScreenGrabOverwriteButton->SetParent ( NULL );
    this->ScreenGrabOverwriteButton->Delete();
    this->ScreenGrabOverwriteButton = NULL;    
    }
  if ( this->ScreenGrabCaptureButton )
    {
    this->ScreenGrabCaptureButton->RemoveObservers (vtkKWPushButton::InvokedEvent, (vtkCommand *)this->GUICallbackCommand );
    this->ScreenGrabCaptureButton->SetParent ( NULL);
    this->ScreenGrabCaptureButton->Delete();
    this->ScreenGrabCaptureButton = NULL;    
    }
  if ( this->ScreenGrabCloseButton )
    {
    this->ScreenGrabCloseButton->RemoveObservers (vtkKWPushButton::InvokedEvent, (vtkCommand *)this->GUICallbackCommand );
    this->ScreenGrabCloseButton->SetParent ( NULL );
    this->ScreenGrabCloseButton->Delete();
    this->ScreenGrabCloseButton = NULL;    
    }
  this->ScreenGrabOptionsWindow->Delete();
  this->ScreenGrabOptionsWindow = NULL;
}




//---------------------------------------------------------------------------
void vtkSlicerViewControlGUI::WithdrawScreenGrabOptionsWindow ( )
{
  vtkSlicerApplication *app = vtkSlicerApplication::SafeDownCast ( this->GetApplication() );
  if ( app && this->ScreenGrabOptionsWindow )
    {
    app->Script ( "grab release %s", this->ScreenGrabOptionsWindow->GetWidgetName() );
    }

  if ( this->ScreenGrabDialogButton )
    {
    this->ScreenGrabDialogButton->GetLoadSaveDialog()->RemoveObservers ( vtkKWTopLevel::WithdrawEvent, (vtkCommand *)this->GUICallbackCommand );
    }
  if ( this->ScreenGrabFormatMenuButton )
    {
    this->ScreenGrabFormatMenuButton->GetMenu()->RemoveObservers ( vtkKWMenu::MenuItemInvokedEvent, (vtkCommand *)this->GUICallbackCommand);
    }
  if ( this->ScreenGrabOverwriteButton )
    {
    this->ScreenGrabOverwriteButton->RemoveObservers (vtkKWCheckButton::SelectedStateChangedEvent, (vtkCommand *)this->GUICallbackCommand );
    }
  if ( this->ScreenGrabCaptureButton )
    {
    this->ScreenGrabCaptureButton->RemoveObservers (vtkKWPushButton::InvokedEvent, (vtkCommand *)this->GUICallbackCommand );
    }
  if ( this->ScreenGrabCloseButton )
    {
    this->ScreenGrabCloseButton->RemoveObservers (vtkKWPushButton::InvokedEvent, (vtkCommand *)this->GUICallbackCommand );
    }
  if ( this->ScreenGrabOptionsWindow )
    {
    this->ScreenGrabOptionsWindow->Withdraw();
    }
}



//---------------------------------------------------------------------------
void vtkSlicerViewControlGUI::RaiseScreenGrabOptionsWindow ( )
{
  //--- create window if not already created.
  vtkSlicerApplication *app = vtkSlicerApplication::SafeDownCast ( this->GetApplication() );

  if ( this->ScreenGrabDirectory == NULL )
    {
    if ( app )
      {
      if ( app->GetTemporaryDirectory() )
        {
        this->SetScreenGrabDirectory(app->GetTemporaryDirectory() );
        }
      }
    }

  if ( this->ScreenGrabOptionsWindow == NULL )
    {
    int px, py;
    //-- top level container.
    this->ScreenGrabOptionsWindow = vtkKWTopLevel::New();
    this->ScreenGrabOptionsWindow->SetMasterWindow ( this->GetScreenGrabButton() );
    this->ScreenGrabOptionsWindow->SetApplication ( this->GetApplication() );
    this->ScreenGrabOptionsWindow->Create();
    vtkKWTkUtilities::GetWidgetCoordinates( this->GetScreenGrabButton(), &px, &py );
    this->ScreenGrabOptionsWindow->SetPosition ( px + 10, py - 250 );
    this->ScreenGrabOptionsWindow->SetBorderWidth ( 1 );
    this->ScreenGrabOptionsWindow->SetReliefToFlat();
    this->ScreenGrabOptionsWindow->SetTitle ( "Screen Capture Options");
    this->ScreenGrabOptionsWindow->SetSize ( 550, 250 );
    this->ScreenGrabOptionsWindow->Withdraw();
    this->ScreenGrabOptionsWindow->SetDeleteWindowProtocolCommand ( this, "DestroyScreenGrabOptionsWindow");

    vtkKWFrame *f1 = vtkKWFrame::New();
    f1->SetParent ( this->ScreenGrabOptionsWindow );
    f1->Create();
    f1->SetBorderWidth ( 1 );
    this->Script ( "pack %s -side top -anchor nw -fill x -expand y -padx 0 -pady 1", f1->GetWidgetName() );


    vtkKWLabel *l0 = vtkKWLabel::New();
    l0->SetParent ( f1 );
    l0->Create();
    l0->SetText ( "Directory / folder:" );
    this->ScreenGrabDialogButton = vtkKWLoadSaveButton::New();
    this->ScreenGrabDialogButton->SetParent ( f1 );
    this->ScreenGrabDialogButton->Create();
    if ( this->GetScreenGrabDirectory() == NULL )
      {
      this->ScreenGrabDialogButton->GetLoadSaveDialog()->RetrieveLastPathFromRegistry ("OpenPath");
      const char *lastpath = this->ScreenGrabDialogButton->GetLoadSaveDialog()->GetLastPath();
      if ( lastpath != NULL && !(strcmp(lastpath, "" )) )
        {
        this->ScreenGrabDialogButton->SetInitialFileName (lastpath);
        }
      }
    else
      {
      this->ScreenGrabDialogButton->GetLoadSaveDialog()->SetLastPath ( this->GetScreenGrabDirectory() );
      this->ScreenGrabDialogButton->SetInitialFileName ( this->GetScreenGrabDirectory() );
      }
    this->ScreenGrabDialogButton->TrimPathFromFileNameOff();
    this->ScreenGrabDialogButton->SetMaximumFileNameLength (128 );
    this->ScreenGrabDialogButton->GetLoadSaveDialog()->ChooseDirectoryOn();
    
    this->ScreenGrabDialogButton->SetBalloonHelpString ( "Select a directory in which screen captures will be saved." );

    vtkKWLabel *l1 = vtkKWLabel::New();
    l1->SetParent ( f1 );
    l1->Create();
    l1->SetText ( "Image name:" );
    this->SetScreenGrabName  ("SlicerImage");
    this->ScreenGrabNameEntry = vtkKWEntry::New();
    this->ScreenGrabNameEntry->SetParent ( f1 );
    this->ScreenGrabNameEntry->Create();
    this->ScreenGrabNameEntry->SetCommandTriggerToReturnKeyAndFocusOut ();
    this->ScreenGrabNameEntry->SetValue ( this->ScreenGrabName );    
    this->ScreenGrabNameEntry->SetBalloonHelpString ( "Select a base-name for the image file, or use the default provided." );

    vtkKWLabel *l2 = vtkKWLabel::New();
    l2->SetParent ( f1 );
    l2->Create();
    l2->SetText ( "Image version number:" );
    this->ScreenGrabNumberEntry = vtkKWEntry::New();
    this->ScreenGrabNumberEntry->SetParent ( f1 );
    this->ScreenGrabNumberEntry->Create();
    this->ScreenGrabNumberEntry->SetCommandTriggerToReturnKeyAndFocusOut();
    this->ScreenGrabNumberEntry->SetValueAsInt ( this->ScreenGrabNumber );
    this->ScreenGrabNumberEntry->SetBalloonHelpString ( "Select a number to append to the image file base-name to create a unique filename." );

    vtkKWLabel *l3 = vtkKWLabel::New();
    l3->SetParent ( f1 );
    l3->Create();
    l3->SetText ( "Image scale:" );
    this->ScreenGrabMagnificationEntry = vtkKWEntry::New();
    this->ScreenGrabMagnificationEntry->SetParent ( f1 );
    this->ScreenGrabMagnificationEntry->Create();
this->ScreenGrabMagnificationEntry->SetCommandTriggerToReturnKeyAndFocusOut();
    this->ScreenGrabMagnificationEntry->SetValueAsInt ( this->ScreenGrabMagnification );
    this->ScreenGrabMagnificationEntry->SetBalloonHelpString ( "Select an integer scale factor (between 1 and 5) for the image file, e.g. a value of \"2\" will save an image twice the size of the current 3D Viewer." );

    vtkKWLabel *l4 = vtkKWLabel::New();
    l4->SetParent (f1);
    l4->Create();
    l4->SetText ( "Choose Format:" );
    this->ScreenGrabFormatMenuButton = vtkKWMenuButton::New();
    this->ScreenGrabFormatMenuButton->SetParent ( f1 );
    this->ScreenGrabFormatMenuButton->Create();
    this->ScreenGrabFormatMenuButton->GetMenu()->AddRadioButton ( ".png" );
    this->ScreenGrabFormatMenuButton->GetMenu()->AddRadioButton ( ".jpg" );
    this->ScreenGrabFormatMenuButton->GetMenu()->AddRadioButton ( ".tiff" );
    this->ScreenGrabFormatMenuButton->GetMenu()->AddRadioButton ( ".eps" );
    this->ScreenGrabFormatMenuButton->GetMenu()->AddRadioButton ( ".ps" );
    this->ScreenGrabFormatMenuButton->GetMenu()->AddRadioButton ( ".prn" );
    this->ScreenGrabFormatMenuButton->GetMenu()->AddRadioButton ( ".pnm" );
    this->ScreenGrabFormatMenuButton->GetMenu()->AddRadioButton ( ".ppm" );
    this->ScreenGrabFormatMenuButton->GetMenu()->AddSeparator();
    this->ScreenGrabFormatMenuButton->GetMenu()->AddCommand ( "close" );
    this->ScreenGrabFormat.clear();
    this->ScreenGrabFormat = ".png";
    this->ScreenGrabFormatMenuButton->SetWidth ( 15 );
    this->ScreenGrabFormatMenuButton->SetValue ( this->ScreenGrabFormat.c_str() );
    this->ScreenGrabFormatMenuButton->SetBalloonHelpString ( "Specify an image file format." );
    
    vtkKWLabel *l5 = vtkKWLabel::New();
    l5->SetParent (f1);
    l5->Create();
    l5->SetText  ( "Overwrite existing any files" );
    this->ScreenGrabOverwriteButton = vtkKWCheckButton::New();
    this->ScreenGrabOverwriteButton->SetParent ( f1 );
    this->ScreenGrabOverwriteButton->Create();
    this->ScreenGrabOverwriteButton->SetSelectedState ( this->ScreenGrabOverwrite );
    this->ScreenGrabOverwriteButton->SetBalloonHelpString ( "Select this option if you wish to overwrite any existing image files with new screen captures." );

    this->Script ( "grid %s -row 0 -column 0 -padx 2 -pady 2 -sticky e", l0->GetWidgetName() );
    this->Script ( "grid %s -row 0 -column 1  -columnspan 2 -padx 2 -pady 2 -sticky ew", this->ScreenGrabDialogButton->GetWidgetName() );
    this->Script ( "grid %s -row 1 -column 0 -padx 2 -pady 2 -sticky e", l1->GetWidgetName() );
    this->Script ( "grid %s -row 1 -column 1  -columnspan 2 -padx 2 -pady 2 -sticky ew", this->ScreenGrabNameEntry->GetWidgetName() );
    this->Script ( "grid %s -row 2 -column 0 -padx 2 -pady 2 -sticky e", l2->GetWidgetName() );
    this->Script ( "grid %s -row 2 -column 1  -padx 2 -pady 2 -sticky ew", this->ScreenGrabNumberEntry->GetWidgetName() );
    this->Script ( "grid %s -row 3 -column 0 -padx 2 -pady 2 -sticky e", l3->GetWidgetName() );
    this->Script ( "grid %s -row 3 -column 1  -padx 2 -pady 2 -sticky ew", this->ScreenGrabMagnificationEntry->GetWidgetName() );
    this->Script ( "grid %s -row 4 -column 0 -padx 2 -pady 2 -sticky e", l4->GetWidgetName() );
    this->Script ( "grid %s -row 4 -column 1  -padx 2 -pady 2 -sticky w", this->ScreenGrabFormatMenuButton->GetWidgetName() );
    this->Script ( "grid %s -row 5 -column 0 -padx 2 -pady 2 -sticky e", this->ScreenGrabOverwriteButton->GetWidgetName() );
    this->Script ( "grid %s -row 5 -column 1  -columnspan 2 -padx 2 -pady 2 -sticky w", l5->GetWidgetName() );
    this->Script ( "grid columnconfigure %s 0 -weight 0", f1->GetWidgetName() );
    this->Script ( "grid columnconfigure %s 1 -weight 0", f1->GetWidgetName() );
    this->Script ( "grid columnconfigure %s 2 -weight 1", f1->GetWidgetName() );

    
    vtkKWFrame *f2 = vtkKWFrame::New();
    f2->SetParent ( this->ScreenGrabOptionsWindow );
    f2->Create();
    f2->SetBorderWidth ( 1 );
    this->Script ( "pack %s -side top -anchor nw -fill x -expand y -padx 0 -pady 3", f2->GetWidgetName() );

    this->ScreenGrabCaptureButton = vtkKWPushButton::New();
    this->ScreenGrabCaptureButton->SetParent ( f2 );
    this->ScreenGrabCaptureButton->Create();
    this->ScreenGrabCaptureButton->SetText ( "Capture" );
    this->ScreenGrabCaptureButton->SetWidth ( 9 );
    
    this->ScreenGrabCloseButton = vtkKWPushButton::New();
    this->ScreenGrabCloseButton->SetParent ( f2 );
    this->ScreenGrabCloseButton->Create();
    this->ScreenGrabCloseButton->SetText ( "Close" );
    this->ScreenGrabCloseButton->SetCommand ( this, "WithdrawScreenGrabOptionsWindow" );
    this->ScreenGrabCloseButton->SetWidth ( 9 );

    this->Script ( "pack %s %s -side left -anchor c -fill x -expand n -padx 0 -pady 1",
                   this->ScreenGrabCloseButton->GetWidgetName(),
                   this->ScreenGrabCaptureButton->GetWidgetName() );
    

    l0->Delete();
    l1->Delete();
    l2->Delete();
    l3->Delete();
    l4->Delete();
    l5->Delete();
    f1->Delete();
    f2->Delete();

    }


  this->ScreenGrabDialogButton->GetLoadSaveDialog()->AddObserver ( vtkKWTopLevel::WithdrawEvent, (vtkCommand *)this->GUICallbackCommand );
  this->ScreenGrabFormatMenuButton->GetMenu()->AddObserver ( vtkKWMenu::MenuItemInvokedEvent, (vtkCommand *)this->GUICallbackCommand);
  this->ScreenGrabOverwriteButton->AddObserver (vtkKWCheckButton::SelectedStateChangedEvent, (vtkCommand *)this->GUICallbackCommand );
  this->ScreenGrabCaptureButton->AddObserver (vtkKWPushButton::InvokedEvent, (vtkCommand *)this->GUICallbackCommand );
  this->ScreenGrabCloseButton->AddObserver (vtkKWPushButton::InvokedEvent, (vtkCommand *)this->GUICallbackCommand );

  // display
  this->ScreenGrabOptionsWindow->DeIconify();
  this->ScreenGrabOptionsWindow->Raise();
  if ( app )
    {
    app->Script ( "grab %s", this->ScreenGrabOptionsWindow->GetWidgetName() );
    app->ProcessIdleTasks();
    }
  this->Script ( "update idletasks");

}



//---------------------------------------------------------------------------
const char *vtkSlicerViewControlGUI::GetScreenGrabFormat ( )
{
  if ( ScreenGrabFormat.c_str() != NULL && (strcmp(ScreenGrabFormat.c_str(), "" ) ) )
    {
    return ( ScreenGrabFormat.c_str() );
    }
  else
    {
    return NULL;
    }
}


//---------------------------------------------------------------------------
void vtkSlicerViewControlGUI::SetScreenGrabFormat ( const char *format )
{
  this->ScreenGrabFormat.clear();
  this->ScreenGrabFormat = format;
}
