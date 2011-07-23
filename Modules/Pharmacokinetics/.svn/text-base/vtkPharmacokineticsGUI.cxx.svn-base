#include "vtkObject.h"
#include "vtkObjectFactory.h"

#include "vtkPharmacokineticsGUI.h"
#include "vtkSlicerApplication.h"
#include "vtkSlicerModuleCollapsibleFrame.h"
#include "vtkSlicerSliceControllerWidget.h"

#include "vtkMath.h"
#include "vtkCornerAnnotation.h"
#include "vtkDoubleArray.h"

// MRML
#include "vtkMRMLColorNode.h"
#include "vtkMRMLTimeSeriesBundleNode.h"
#include "vtkMRMLPlotNode.h"
#include "vtkMRMLArrayPlotNode.h"
#include "vtkMRMLOrthogonalLinePlotNode.h"
#include "vtkMRMLXYPlotManagerNode.h"
#include "vtkMRMLPharmacokineticsCurveAnalysisNode.h"
#include "vtkMRMLDoubleArrayNode.h"

// KWWidgets
#include "vtkKWTkUtilities.h"
#include "vtkKWWidget.h"
#include "vtkKWFrameWithLabel.h"
#include "vtkKWFrame.h"
#include "vtkKWLabel.h"
#include "vtkKWEvent.h"
#include "vtkKWMultiColumnList.h"
#include "vtkKWScaleWithEntry.h"
#include "vtkKWScale.h"
#include "vtkKWPushButton.h"
#include "vtkKWRadioButton.h"
#include "vtkKWMenuButton.h"
#include "vtkKWSpinBox.h"
#include "vtkKWRange.h"
#include "vtkKWCheckButtonWithLabel.h"
#include "vtkKWEntryWithLabel.h"
#include "vtkKWFileBrowserDialog.h"
#include "vtkKWProgressDialog.h"
#include "vtkKWMessageDialog.h"
#include "vtkKWLoadSaveButtonWithLabel.h"
#include "vtkKWProgressGauge.h"
#include "vtkKWPopupFrame.h"
#include "vtkKWRadioButtonSetWithLabel.h"

#include <sstream>


//---------------------------------------------------------------------------
vtkStandardNewMacro (vtkPharmacokineticsGUI );
vtkCxxRevisionMacro ( vtkPharmacokineticsGUI, "$Revision: 3674 $");
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
vtkPharmacokineticsGUI::vtkPharmacokineticsGUI ( )
{

  //----------------------------------------------------------------
  // Logic values
  this->Logic = NULL;
  this->DataCallbackCommand = vtkCallbackCommand::New();
  this->DataCallbackCommand->SetClientData( reinterpret_cast<void *> (this) );
  this->DataCallbackCommand->SetCallback(vtkPharmacokineticsGUI::DataCallback);

  this->IntensityCurves = vtkPharmacokineticsIntensityCurves::New();
  this->PlotManagerNode = NULL;
  this->InitialParameterListInputType.clear();
  this->InitialParameterListNodeNames.clear();
  this->FittingTargetMenuNodeList.clear();

  //----------------------------------------------------------------
  // GUI widgets
  this->ProgressDialog                = NULL;

  // -----------------------------------------
  // Bundle Selector
  this->Active4DBundleSelectorWidget  = NULL;

  // -----------------------------------------
  // Frame Control
  this->ForegroundVolumeSelectorScale = NULL;
  this->BackgroundVolumeSelectorScale = NULL;
  this->WindowLevelRange              = NULL;
  this->ThresholdRange                = NULL;

  // -----------------------------------------
  // Intensity Plot
  this->MaskNodeSelector              = NULL;
  this->GenerateCurveButton           = NULL;
  this->ValueTypeButtonSet            = NULL;
  this->IntensityPlot                 = NULL;
  this->ErrorBarCheckButton           = NULL;
  this->PlotList                      = NULL;
  this->ImportPlotButton              = NULL;
  this->SelectAllPlotButton           = NULL;
  this->DeselectAllPlotButton         = NULL;
  this->PlotDeleteButton              = NULL;
  this->SavePlotButton                = NULL;

  // -----------------------------------------
  // Model / Parameters
  this->FittingTargetMenu             = NULL;
  this->ModelSelectButton       = NULL;
  this->CurveScriptSelectButton       = NULL;
  this->CurveScriptMethodName         = NULL;
  this->CurveFittingStartIndexSpinBox = NULL;
  this->CurveFittingEndIndexSpinBox   = NULL;
  this->InitialParameterList          = NULL;
  this->PlotSelectPopUpMenu           = NULL;
  this->CurveAnalysisNode = NULL;

  // -----------------------------------------
  // Curve Fitting
  this->RunFittingButton              = NULL;
  this->ResultParameterList           = NULL;

  // -----------------------------------------
  // Parameter Map
  this->ParameterMapRegionButtonSet   = NULL;
  this->MapRegionMaskSelectorWidget   = NULL;
  this->MapRegionMaskLabelEntry       = NULL;
  this->MapOutputVolumePrefixEntry    = NULL;
  this->GenerateMapButton             = NULL; // name should be changed
  this->MapIMinSpinBox                = NULL;
  this->MapIMaxSpinBox                = NULL;
  this->MapJMinSpinBox                = NULL;
  this->MapJMaxSpinBox                = NULL;
  this->MapKMinSpinBox                = NULL;
  this->MapKMaxSpinBox                = NULL;

  //----------------------------------------------------------------
  // Time
  this->TimerFlag = 0;

}

//---------------------------------------------------------------------------
vtkPharmacokineticsGUI::~vtkPharmacokineticsGUI ( )
{

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

  if (this->ProgressDialog)
    {
    this->ProgressDialog->SetParent(NULL);
    this->ProgressDialog->Delete();
    }

  // -----------------------------------------
  // Bundle Selector
  if (this->Active4DBundleSelectorWidget)
    {
    this->Active4DBundleSelectorWidget->SetParent(NULL);
    this->Active4DBundleSelectorWidget->Delete();
    }

  // -----------------------------------------
  // Frame Control
  if (this->ForegroundVolumeSelectorScale)
    {
    this->ForegroundVolumeSelectorScale->SetParent(NULL);
    this->ForegroundVolumeSelectorScale->Delete();
    }
  if (this->BackgroundVolumeSelectorScale)
    {
    this->BackgroundVolumeSelectorScale->SetParent(NULL);
    this->BackgroundVolumeSelectorScale->Delete();
    }
  if (this->WindowLevelRange)
    {
    this->WindowLevelRange->SetParent(NULL);
    this->WindowLevelRange->Delete();
    }
  if (this->ThresholdRange)
    {
    this->ThresholdRange->SetParent(NULL);
    this->ThresholdRange->Delete();
    }

  // -----------------------------------------
  // Intensity Plot
  if (this->MaskNodeSelector)
    {
    this->MaskNodeSelector->SetParent(NULL);
    this->MaskNodeSelector->Delete();
    }
  if (this->GenerateCurveButton)
    {
    this->GenerateCurveButton->SetParent(NULL);
    this->GenerateCurveButton->Delete();
    }
  if (this->ValueTypeButtonSet)
    {
    this->ValueTypeButtonSet->SetParent(NULL);
    this->ValueTypeButtonSet->Delete();
    }
  if (this->IntensityPlot)
    {
    this->IntensityPlot->SetParent(NULL);
    this->IntensityPlot->Delete();
    }
  if (this->ErrorBarCheckButton)
    {
    this->ErrorBarCheckButton->SetParent(NULL);
    this->ErrorBarCheckButton->Delete();
    }
  if (this->PlotList)
    {
    this->PlotList->SetParent(NULL);
    this->PlotList->Delete();
    }
  if (this->ImportPlotButton)
    {
    this->ImportPlotButton->SetParent(NULL);
    this->ImportPlotButton->Delete();
    }
  if (this->SelectAllPlotButton)
    {
    this->SelectAllPlotButton->SetParent(NULL);
    this->SelectAllPlotButton->Delete();
    }
  if (this->DeselectAllPlotButton)
    {
    this->DeselectAllPlotButton->SetParent(NULL);
    this->DeselectAllPlotButton->Delete();
    }
  if (this->PlotDeleteButton)
    {
    this->PlotDeleteButton->SetParent(NULL);
    this->PlotDeleteButton->Delete();
    }
  if (this->SavePlotButton)
    {
    this->SavePlotButton->SetParent(NULL);
    this->SavePlotButton->Delete();
    }


  // -----------------------------------------
  // Model / Parameters
  if (this->FittingTargetMenu)
    {
    this->FittingTargetMenu->SetParent(NULL);
    this->FittingTargetMenu->Delete();
    }
  if (this->CurveScriptSelectButton)
    {
    this->CurveScriptSelectButton->SetParent(NULL);
    this->CurveScriptSelectButton->Delete();
    }
  if (this->ModelSelectButton)
    {
    this->ModelSelectButton->SetParent(NULL);
    this->ModelSelectButton->Delete();
    }
  if (this->CurveScriptMethodName)
    {
    this->CurveScriptMethodName->SetParent(NULL);
    this->CurveScriptMethodName->Delete();
    }
  if (this->CurveFittingStartIndexSpinBox)
    {
    this->CurveFittingStartIndexSpinBox->SetParent(NULL);
    this->CurveFittingStartIndexSpinBox->Delete();
    }
  if (this->CurveFittingEndIndexSpinBox)
    {
    this->CurveFittingEndIndexSpinBox->SetParent(NULL);
    this->CurveFittingEndIndexSpinBox->Delete();
    }
  if (this->InitialParameterList)
    {
    this->InitialParameterList->SetParent(NULL);
    this->InitialParameterList->Delete();
    }


  // -----------------------------------------
  // Curve Fitting

  if (this->RunFittingButton)
    {
    this->RunFittingButton->SetParent(NULL);
    this->RunFittingButton->Delete();
    }
  if (this->ResultParameterList)
    {
    this->ResultParameterList->SetParent(NULL);
    this->ResultParameterList->Delete();
    }


  // -----------------------------------------
  // Parameter Map

  if (this->ParameterMapRegionButtonSet)
    {
    this->ParameterMapRegionButtonSet->SetParent(NULL);
    this->ParameterMapRegionButtonSet->Delete();
    }
  if (this->MapRegionMaskSelectorWidget)
    {
    this->MapRegionMaskSelectorWidget->SetParent(NULL);
    this->MapRegionMaskSelectorWidget->Delete();
    }
  if (this->MapRegionMaskLabelEntry)
    {
    this->MapRegionMaskLabelEntry->SetParent(NULL);
    this->MapRegionMaskLabelEntry->Delete();
    }
  if (this->MapOutputVolumePrefixEntry)
    {
    this->MapOutputVolumePrefixEntry->SetParent(NULL);
    this->MapOutputVolumePrefixEntry->Delete();
    }
  if (this->GenerateMapButton)
    {
    this->GenerateMapButton->SetParent(NULL);
    this->GenerateMapButton->Delete();
    }
  if (this->MapIMinSpinBox)
    {
    this->MapIMinSpinBox->SetParent(NULL);
    this->MapIMinSpinBox->Delete();
    }
  if (this->MapIMaxSpinBox)
    {
    this->MapIMaxSpinBox->SetParent(NULL);
    this->MapIMaxSpinBox->Delete();
    }
  if (this->MapJMinSpinBox)
    {
    this->MapJMinSpinBox->SetParent(NULL);
    this->MapJMinSpinBox->Delete();
    }
  if (this->MapJMaxSpinBox)
    {
    this->MapJMaxSpinBox->SetParent(NULL);
    this->MapJMaxSpinBox->Delete();
    }
  if (this->MapKMinSpinBox)
    {
    this->MapKMinSpinBox->SetParent(NULL);
    this->MapKMinSpinBox->Delete();
    }
  if (this->MapKMaxSpinBox)
    {
    this->MapKMaxSpinBox->SetParent(NULL);
    this->MapKMaxSpinBox->Delete();
    }


  //----------------------------------------------------------------
  // Intensity curve
  if (this->IntensityCurves)
    {
    this->IntensityCurves->Delete();
    }

  //----------------------------------------------------------------
  // Plot Manager Node
  if (this->PlotManagerNode)
    {
    //if (this->GetMRMLScene())
    //  {
    //  this->GetMRMLScene()->RemoveNode(this->PlotManagerNode);
    //  }
    this->PlotManagerNode->Delete();
    }

  //--- Let go of MRML
  if ( this->CurveAnalysisNode )
    {
    vtkSetAndObserveMRMLNodeMacro ( this->CurveAnalysisNode, NULL );
    }

  //----------------------------------------------------------------
  // Unregister Logic class

  if (this->Logic)
    {
    this->Logic->Delete();
    }

}


//---------------------------------------------------------------------------
void vtkPharmacokineticsGUI::Init()
{

  if (this->GetMRMLScene())
    {
    
    // Register node classes. SmartPointer is used.
      {
      this->GetMRMLScene()->RegisterNodeClass( vtkSmartPointer< vtkMRMLPharmacokineticsCurveAnalysisNode >::New() );
      }

    if (this->IntensityCurves)
      {
      this->IntensityCurves->SetMRMLScene(this->GetMRMLScene());
      }

    // Setup plot manager node
    if (!this->PlotManagerNode)
      {
      vtkMRMLXYPlotManagerNode* node = vtkMRMLXYPlotManagerNode::New();
      this->GetMRMLScene()->AddNode(node);
      
      vtkIntArray  *events = vtkIntArray::New();
      events->InsertNextValue ( vtkCommand::ModifiedEvent );
      vtkSetAndObserveMRMLNodeEventsMacro ( this->PlotManagerNode, node, events );
      
      node->Delete();
      events->Delete();
      node = NULL;
      events = NULL;
      this->PlotManagerNode->Refresh();
      }

    if ( this->IntensityPlot )
      {
      this->IntensityPlot->SetAndObservePlotManagerNode(this->PlotManagerNode);
      }
    if ( this->PlotManagerNode )
      {
      this->PlotManagerNode->Refresh();
      }

    }

    
}

//---------------------------------------------------------------------------
void vtkPharmacokineticsGUI::Enter()
{
  // Fill in
  //vtkSlicerApplicationGUI *appGUI = this->GetApplicationGUI();
  
  if (this->TimerFlag == 0)
    {
    this->TimerFlag = 1;
    this->TimerInterval = 100;  // 100 ms
    ProcessTimerEvents();
    }

}


//---------------------------------------------------------------------------
void vtkPharmacokineticsGUI::Exit ( )
{
  // Fill in
}


//---------------------------------------------------------------------------
void vtkPharmacokineticsGUI::PrintSelf ( ostream& os, vtkIndent indent )
{
  this->vtkObject::PrintSelf ( os, indent );

  os << indent << "PharmacokineticsGUI: " << this->GetClassName ( ) << "\n";
  os << indent << "Logic: " << this->GetLogic ( ) << "\n";
}


//---------------------------------------------------------------------------
void vtkPharmacokineticsGUI::RemoveGUIObservers ( )
{
  //vtkSlicerApplicationGUI *appGUI = this->GetApplicationGUI();

  // -----------------------------------------
  // Bundle Selector
  if (this->Active4DBundleSelectorWidget)
    {
    this->Active4DBundleSelectorWidget
      ->RemoveObservers(vtkSlicerNodeSelectorWidget::NodeSelectedEvent,
                        (vtkCommand *)this->GUICallbackCommand );
    }

  // -----------------------------------------
  // Frame Control
  if (this->ForegroundVolumeSelectorScale)
    {
    this->ForegroundVolumeSelectorScale
      ->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }
  if (this->BackgroundVolumeSelectorScale)
    {
    this->BackgroundVolumeSelectorScale
      ->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }
  if (this->WindowLevelRange)
    {
    this->WindowLevelRange
      ->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }
  if (this->ThresholdRange)
    {
    this->ThresholdRange
      ->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }

  // -----------------------------------------
  // Intensity Plot
  if (this->MaskNodeSelector)
    {
    this->MaskNodeSelector
      ->RemoveObservers(vtkSlicerNodeSelectorWidget::NodeSelectedEvent,
                        (vtkCommand *)this->GUICallbackCommand );
    }
  if (this->GenerateCurveButton)
    {
    this->GenerateCurveButton
      ->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }
  if (this->ErrorBarCheckButton)
    {
    this->ErrorBarCheckButton->GetWidget()
      ->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }
  if (this->PlotList)
    {
    this->PlotList->GetWidget()
      ->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }
  if (this->ImportPlotButton)
    {
    this->ImportPlotButton
      ->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }
  if (this->SelectAllPlotButton)
    {
    this->SelectAllPlotButton
      ->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }
  if (this->DeselectAllPlotButton)
    {
    this->DeselectAllPlotButton
      ->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }
  if (this->PlotDeleteButton)
    {
    this->PlotDeleteButton
      ->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }
  if (this->SavePlotButton)
    {
    this->SavePlotButton
      ->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }

  // -----------------------------------------
  // Model / Parameters

  if (this->FittingTargetMenu)
    {
    this->FittingTargetMenu
      ->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }
  if (this->CurveScriptSelectButton)
    {
    this->CurveScriptSelectButton
      ->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }
  if (this->ModelSelectButton)
    {
    this->ModelSelectButton
      ->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }
  if (this->CurveFittingStartIndexSpinBox)
    {
    this->CurveFittingStartIndexSpinBox
      ->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }
  if (this->CurveFittingEndIndexSpinBox)
    {
    this->CurveFittingEndIndexSpinBox
      ->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }


  // -----------------------------------------
  // Curve Fitting

  if (this->RunFittingButton)
    {
    this->RunFittingButton
      ->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }

  // -----------------------------------------
  // Parameter Map

  if (this->GenerateMapButton)
    {
    this->GenerateMapButton
      ->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }
  if (this->ParameterMapRegionButtonSet)
    {
    this->ParameterMapRegionButtonSet->GetWidget()->GetWidget(0)
      ->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    this->ParameterMapRegionButtonSet->GetWidget()->GetWidget(1)
      ->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }
  if (this->MapOutputVolumePrefixEntry)
    {
    this->MapOutputVolumePrefixEntry->GetWidget()
      ->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }
  if (this->MapIMinSpinBox)
    {
    this->MapIMinSpinBox
      ->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }
  if (this->MapIMaxSpinBox)
    {
    this->MapIMaxSpinBox
      ->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }
  if (this->MapJMinSpinBox)
    {
    this->MapJMinSpinBox
      ->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }
  if (this->MapJMaxSpinBox)
    {
    this->MapJMaxSpinBox
      ->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }
  if (this->MapKMinSpinBox)
    {
    this->MapKMinSpinBox
      ->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }
  if (this->MapKMaxSpinBox)
    {
    this->MapKMaxSpinBox
      ->RemoveObserver((vtkCommand *)this->GUICallbackCommand);
    }


  //----------------------------------------------------------------
  // Remove logic observers
  this->RemoveLogicObservers();

}


//---------------------------------------------------------------------------
void vtkPharmacokineticsGUI::AddGUIObservers ( )
{
  this->RemoveGUIObservers();

  //vtkSlicerApplicationGUI *appGUI = this->GetApplicationGUI();

  //----------------------------------------------------------------
  // MRML

  vtkIntArray* events = vtkIntArray::New();
  events->InsertNextValue(vtkMRMLScene::NodeAddedEvent);
  events->InsertNextValue(vtkMRMLScene::NodeRemovedEvent);
  events->InsertNextValue(vtkMRMLScene::SceneCloseEvent);
  
  if (this->GetMRMLScene() != NULL)
    {
    this->SetAndObserveMRMLSceneEvents(this->GetMRMLScene(), events);
    }
  events->Delete();

  //----------------------------------------------------------------
  // GUI Observers

  // -----------------------------------------
  // Bundle Selector
  if (this->Active4DBundleSelectorWidget)
    {
    this->Active4DBundleSelectorWidget
      ->AddObserver(vtkSlicerNodeSelectorWidget::NodeSelectedEvent,
                    (vtkCommand *)this->GUICallbackCommand );
    }

  // -----------------------------------------
  // Frame Control
  if (this->ForegroundVolumeSelectorScale)
    {
    this->ForegroundVolumeSelectorScale
      ->AddObserver(vtkKWScale::ScaleValueChangingEvent, (vtkCommand *)this->GUICallbackCommand);
    }
  if (this->BackgroundVolumeSelectorScale)
    {
    this->BackgroundVolumeSelectorScale
      ->AddObserver(vtkKWScale::ScaleValueChangingEvent, (vtkCommand *)this->GUICallbackCommand);
    }
  if (this->WindowLevelRange)
    {
    this->WindowLevelRange
      ->AddObserver(vtkKWRange::RangeValueChangingEvent, (vtkCommand *)this->GUICallbackCommand);
    }
  if (this->ThresholdRange)
    {
    this->ThresholdRange
      ->AddObserver(vtkKWRange::RangeValueChangingEvent, (vtkCommand *)this->GUICallbackCommand);
    }

  // -----------------------------------------
  // Intensity Plot
  if (this->MaskNodeSelector)
    {
    this->MaskNodeSelector
      ->AddObserver(vtkSlicerNodeSelectorWidget::NodeSelectedEvent,
                    (vtkCommand *)this->GUICallbackCommand );
    }
  if (this->GenerateCurveButton)
    {
    this->GenerateCurveButton
      ->AddObserver(vtkKWPushButton::InvokedEvent, (vtkCommand *)this->GUICallbackCommand);
    }
  if (this->ErrorBarCheckButton)
    {
    this->ErrorBarCheckButton->GetWidget()
      ->AddObserver(vtkKWCheckButton::SelectedStateChangedEvent, (vtkCommand *)this->GUICallbackCommand);
    }

  if (this->ImportPlotButton)
    {
    this->ImportPlotButton
      ->AddObserver(vtkKWPushButton::InvokedEvent, (vtkCommand *)this->GUICallbackCommand);
    }
  if (this->SelectAllPlotButton)
    {
    this->SelectAllPlotButton
      ->AddObserver(vtkKWPushButton::InvokedEvent, (vtkCommand *)this->GUICallbackCommand);
    }
  if (this->DeselectAllPlotButton)
    {
    this->DeselectAllPlotButton
      ->AddObserver(vtkKWPushButton::InvokedEvent, (vtkCommand *)this->GUICallbackCommand);
    }
  if (this->PlotDeleteButton)
    {
    this->PlotDeleteButton
      ->AddObserver(vtkKWPushButton::InvokedEvent, (vtkCommand *)this->GUICallbackCommand);
    }
  if (this->SavePlotButton)
    {
    this->SavePlotButton
      ->AddObserver(vtkKWPushButton::InvokedEvent, (vtkCommand *)this->GUICallbackCommand);
    }

  // -----------------------------------------
  // Model / Parameters

  if (this->FittingTargetMenu)
    {
    this->FittingTargetMenu
      ->AddObserver(vtkKWPushButton::InvokedEvent, (vtkCommand *)this->GUICallbackCommand);
    }
  if (this->CurveScriptSelectButton)
    {
    this->CurveScriptSelectButton->GetWidget()->GetLoadSaveDialog()
      ->AddObserver(vtkKWLoadSaveDialog::FileNameChangedEvent, (vtkCommand *)this->GUICallbackCommand);
    }
  if (this->ModelSelectButton)
    {
    this->ModelSelectButton->GetMenu()
      ->AddObserver(vtkKWMenu::MenuItemInvokedEvent, (vtkCommand *)this->GUICallbackCommand);
    }
  if (this->CurveFittingStartIndexSpinBox)
    {
    this->CurveFittingStartIndexSpinBox
      ->AddObserver(vtkKWSpinBox::SpinBoxValueChangedEvent, (vtkCommand *)this->GUICallbackCommand);
    }
  if (this->CurveFittingEndIndexSpinBox)
    {
    this->CurveFittingEndIndexSpinBox
      ->AddObserver(vtkKWSpinBox::SpinBoxValueChangedEvent, (vtkCommand *)this->GUICallbackCommand);
    }


  // -----------------------------------------
  // Curve Fitting

  if (this->RunFittingButton)
    {
    this->RunFittingButton
      ->AddObserver(vtkKWPushButton::InvokedEvent, (vtkCommand *)this->GUICallbackCommand);
    }


  // -----------------------------------------
  // Parameter Map

  if (this->ParameterMapRegionButtonSet)
    {
    this->ParameterMapRegionButtonSet->GetWidget()->GetWidget(0)
      ->AddObserver(vtkKWRadioButton::SelectedStateChangedEvent, (vtkCommand *)this->GUICallbackCommand);
    this->ParameterMapRegionButtonSet->GetWidget()->GetWidget(1)
      ->AddObserver(vtkKWRadioButton::SelectedStateChangedEvent, (vtkCommand *)this->GUICallbackCommand);
    }
  if (this->MapOutputVolumePrefixEntry)
    {
    this->MapOutputVolumePrefixEntry->GetWidget()
      ->AddObserver(vtkKWEntry::EntryValueChangedEvent, (vtkCommand *)this->GUICallbackCommand);
    }
  if (this->GenerateMapButton)
    {
    this->GenerateMapButton
      ->AddObserver(vtkKWPushButton::InvokedEvent, (vtkCommand *)this->GUICallbackCommand);
    }

  if (this->MapIMinSpinBox)
    {
    this->MapIMinSpinBox
      ->AddObserver(vtkKWSpinBox::SpinBoxValueChangedEvent, (vtkCommand *)this->GUICallbackCommand);
    }
  if (this->MapIMaxSpinBox)
    {
    this->MapIMaxSpinBox
      ->AddObserver(vtkKWSpinBox::SpinBoxValueChangedEvent, (vtkCommand *)this->GUICallbackCommand);
    }
  if (this->MapJMinSpinBox)
    {
    this->MapJMinSpinBox
      ->AddObserver(vtkKWSpinBox::SpinBoxValueChangedEvent, (vtkCommand *)this->GUICallbackCommand);
    }
  if (this->MapJMaxSpinBox)
    {
    this->MapJMaxSpinBox
      ->AddObserver(vtkKWSpinBox::SpinBoxValueChangedEvent, (vtkCommand *)this->GUICallbackCommand);
    }
  if (this->MapKMinSpinBox)
    {
    this->MapKMinSpinBox
      ->AddObserver(vtkKWSpinBox::SpinBoxValueChangedEvent, (vtkCommand *)this->GUICallbackCommand);
    }
  if (this->MapKMaxSpinBox)
    {
    this->MapKMaxSpinBox
      ->AddObserver(vtkKWSpinBox::SpinBoxValueChangedEvent, (vtkCommand *)this->GUICallbackCommand);
    }

  //----------------------------------------------------------------
  // Logic Observers

  this->AddLogicObservers();

}


//---------------------------------------------------------------------------
void vtkPharmacokineticsGUI::RemoveLogicObservers ( )
{
  if (this->GetLogic())
    {
    this->GetLogic()->RemoveObservers(vtkCommand::ModifiedEvent,
                                  (vtkCommand *)this->LogicCallbackCommand);
    this->GetLogic()->RemoveObservers(vtkPharmacokineticsLogic::StatusUpdateEvent,
                                      (vtkCommand *)this->LogicCallbackCommand);
    }
}


//---------------------------------------------------------------------------
void vtkPharmacokineticsGUI::AddLogicObservers ( )
{
  this->RemoveLogicObservers();  

  if (this->GetLogic())
    {
    this->GetLogic()->AddObserver(vtkCommand::ModifiedEvent,
                                  (vtkCommand *)this->LogicCallbackCommand);
    this->GetLogic()->AddObserver(vtkPharmacokineticsLogic::StatusUpdateEvent,
                                  (vtkCommand *)this->LogicCallbackCommand);
    }
}


//---------------------------------------------------------------------------
void vtkPharmacokineticsGUI::HandleMouseEvent(vtkSlicerInteractorStyle *style)
{
}


//---------------------------------------------------------------------------
void vtkPharmacokineticsGUI::ProcessGUIEvents(vtkObject *caller,
                                         unsigned long event, void *callData)
{

  const char *eventName = vtkCommand::GetStringFromEventId(event);

  if (strcmp(eventName, "LeftButtonPressEvent") == 0)
    {
    vtkSlicerInteractorStyle *style = vtkSlicerInteractorStyle::SafeDownCast(caller);
    HandleMouseEvent(style);
    return;
    }

  else if (this->Active4DBundleSelectorWidget == vtkSlicerNodeSelectorWidget::SafeDownCast(caller)
           && event == vtkSlicerNodeSelectorWidget::NodeSelectedEvent ) 
    {
    vtkMRMLTimeSeriesBundleNode *bundleNode = 
      vtkMRMLTimeSeriesBundleNode::SafeDownCast(this->Active4DBundleSelectorWidget->GetSelected());
    SelectActive4DBundle(bundleNode);
    }
  else if (this->ForegroundVolumeSelectorScale == vtkKWScaleWithEntry::SafeDownCast(caller)
      && event == vtkKWScale::ScaleValueChangingEvent /*vtkKWScale::ScaleValueChangedEvent*/)
    {
    vtkMRMLTimeSeriesBundleNode *bundleNode = 
      vtkMRMLTimeSeriesBundleNode::SafeDownCast(this->Active4DBundleSelectorWidget->GetSelected());
    int volume = (int)this->ForegroundVolumeSelectorScale->GetValue();
    if (bundleNode)
      {
      SetForeground(bundleNode->GetID(), volume);
      }
    }
  else if (this->BackgroundVolumeSelectorScale == vtkKWScaleWithEntry::SafeDownCast(caller)
      && event == vtkKWScale::ScaleValueChangingEvent /*vtkKWScale::ScaleValueChangedEvent*/ )
    {
    int volume = (int)this->BackgroundVolumeSelectorScale->GetValue();
    vtkMRMLTimeSeriesBundleNode *bundleNode = 
      vtkMRMLTimeSeriesBundleNode::SafeDownCast(this->Active4DBundleSelectorWidget->GetSelected());
    if (bundleNode)
      {
      SetBackground(bundleNode->GetID(), volume);
      }
    }
  else if (this->WindowLevelRange == vtkKWRange::SafeDownCast(caller)
      && event == vtkKWRange::RangeValueChangingEvent)
    {
    double wllow, wlhigh;
    this->WindowLevelRange->GetRange(wllow, wlhigh);
    this->Window = wlhigh - wllow;
    this->Level  = (wlhigh + wllow) / 2.0;
    SetWindowLevelForCurrentFrame();
    }
  else if (this->ThresholdRange == vtkKWRange::SafeDownCast(caller)
      && event == vtkKWRange::RangeValueChangingEvent)
    {
    double thlow, thhigh;
    this->ThresholdRange->GetRange(thlow, thhigh);
    this->ThresholdUpper  = thhigh; 
    this->ThresholdLower  = thlow; 
    SetWindowLevelForCurrentFrame();
    }
  else if (this->MaskNodeSelector == vtkSlicerNodeSelectorWidget::SafeDownCast(caller)
           && event == vtkSlicerNodeSelectorWidget::NodeSelectedEvent ) 
    {
    vtkMRMLScalarVolumeNode* maskNode =
      vtkMRMLScalarVolumeNode::SafeDownCast(this->MaskNodeSelector->GetSelected());
    this->IntensityCurves->SetMaskNode(maskNode);
    }

  else if (this->GenerateCurveButton == vtkKWPushButton::SafeDownCast(caller)
           && event == vtkKWPushButton::InvokedEvent)
    {
    vtkMRMLTimeSeriesBundleNode *bundleNode = 
      vtkMRMLTimeSeriesBundleNode::SafeDownCast(this->Active4DBundleSelectorWidget->GetSelected());
    vtkMRMLScalarVolumeNode* maskNode =
      vtkMRMLScalarVolumeNode::SafeDownCast(this->MaskNodeSelector->GetSelected());
    if (bundleNode && maskNode)
      {
      //--- make sure everybody is current.
      if ( this->IntensityCurves->GetBundleNode() != bundleNode )
        {
        this->IntensityCurves->SetBundleNode(bundleNode);
        }
      if ( this->IntensityCurves->GetMaskNode() != maskNode )
        {
        this->IntensityCurves->SetMaskNode(maskNode);
        }
      }
    else
      {
      this->PostMessage ( "Please make sure an image Time-Series and a VOI mask are selected first." );
      }

    // check the value type for curve
    if (this->ValueTypeButtonSet->GetWidget()->GetWidget(0)->GetSelectedState())
      {
      this->IntensityCurves->SetValueType(vtkPharmacokineticsIntensityCurves::TYPE_MEAN);
      }
    else if (this->ValueTypeButtonSet->GetWidget()->GetWidget(1)->GetSelectedState())
      {
      this->IntensityCurves->SetValueType(vtkPharmacokineticsIntensityCurves::TYPE_MAX);
      }
    else if (this->ValueTypeButtonSet->GetWidget()->GetWidget(2)->GetSelectedState())
      {
      this->IntensityCurves->SetValueType(vtkPharmacokineticsIntensityCurves::TYPE_MIN);
      }
    if ( this->GeneratePlotNodes() )
      {
      this->UpdatePlotList();
      }

    }

  else if (this->ErrorBarCheckButton->GetWidget() == vtkKWCheckButton::SafeDownCast(caller)
           && event == vtkKWCheckButton::SelectedStateChangedEvent)
    {
    if (this->ErrorBarCheckButton->GetWidget()->GetSelectedState() == 1)
      {
      this->PlotManagerNode->SetErrorBarAll(1);
      this->PlotManagerNode->Refresh();
      }
    else
      {
      this->PlotManagerNode->SetErrorBarAll(0);
      this->PlotManagerNode->Refresh();
      }
    }
  else if (this->ImportPlotButton == vtkKWPushButton::SafeDownCast(caller)
           && event == vtkKWPushButton::InvokedEvent)
    {
    vtkKWFileBrowserDialog* fbrowse = vtkKWFileBrowserDialog::New();
    fbrowse->SetParent(this->GetApplicationGUI()->GetActiveViewerWidget());
    fbrowse->Create();
    fbrowse->SetFileTypes("{{Array data} {.txt .csv}}");
    fbrowse->MultipleSelectionOff();
    if (fbrowse->Invoke())
      {
      const char* path = fbrowse->GetFileName();
      ImportPlotNode(path);
      }
    fbrowse->Delete();
    }
  else if (this->SelectAllPlotButton == vtkKWPushButton::SafeDownCast(caller)
           && event == vtkKWPushButton::InvokedEvent)
    {
    SelectAllPlots();
    }
  else if (this->DeselectAllPlotButton == vtkKWPushButton::SafeDownCast(caller)
           && event == vtkKWPushButton::InvokedEvent)
    {
    DeselectAllPlots();
    }
  else if (this->PlotDeleteButton == vtkKWPushButton::SafeDownCast(caller)
           && event == vtkKWPushButton::InvokedEvent)
    {
    DeleteSelectedPlots();
    // TODO: If the MRML event handler works properly, The following call is not necessary.
    this->PlotManagerNode->Refresh();
    }
  else if (this->SavePlotButton == vtkKWPushButton::SafeDownCast(caller)
    //&& event == vtkKWLoadSaveDialog::FileNameChangedEvent)
           && event == vtkKWPushButton::InvokedEvent)
    {
    vtkKWFileBrowserDialog* fbrowse = vtkKWFileBrowserDialog::New();
    fbrowse->SetParent(this->GetApplicationGUI()->GetActiveViewerWidget());
    fbrowse->SaveDialogOn();
    fbrowse->Create();
    fbrowse->SetTitle("Save Curves");
    fbrowse->MultipleSelectionOff();
    if (fbrowse->Invoke())
      {
      const char* filename = fbrowse->GetFileName();
      SaveMarkedPlotNode(filename);
      }
    fbrowse->Delete();
    }
  else if (this->ModelSelectButton->GetMenu() == vtkKWMenu::SafeDownCast ( caller ) &&
           event == vtkKWMenu::MenuItemInvokedEvent )
    {
    //--- for the selected model, populate the interface with appropriate initial parameters.
    this->UpdateInitialParameterList ( );
    }


#ifdef Pharmacokinetics_USE_SCIPY  
  else if (this->CurveScriptSelectButton->GetWidget()->GetLoadSaveDialog()
           == vtkKWLoadSaveDialog::SafeDownCast(caller)
           && event == vtkKWLoadSaveDialog::FileNameChangedEvent)
    {
    if ( this->CurveAnalysisNode == NULL )
      {
      this->CurveAnalysisNode = vtkMRMLPharmacokineticsCurveAnalysisNode::New();
      //--- TODO: SetAndObserve
      }

    this->GetMRMLScene()->AddNode(this->CurveAnalysisNode);

    //const char* script = this->CurveScriptSelectButton->GetWidget()->GetFileName();
    if (this->CurveAnalysisScript)
      {
      this->CurveAnalysisScript->Delete();
      }
    this->CurveAnalysisScript = vtkPharmacokineticsCurveAnalysisPythonInterface::New();
    this->CurveAnalysisScript->SetScript(this->CurveScriptSelectButton->GetWidget()->GetFileName());
    //this->GetLogic()->GetCurveAnalysisInfo(script, this->CurveAnalysisNode);
    this->CurveAnalysisScript->SetPharmacokineticsCurveAnalysisNode(this->CurveAnalysisNode);
    this->CurveAnalysisScript->GetInfo();

    // Update method's name field
    if (this->CurveScriptMethodName)
      {
      this->CurveScriptMethodName->GetWidget()->SetValue(this->CurveAnalysisNode->GetMethodName());
      }

    this->UpdateInitialParameterList( );

    this->GetMRMLScene()->RemoveNode(this->CurveAnalysisNode);
    this->CurveAnalysisNode->Delete();
    }

  else if (this->RunFittingButton == vtkKWPushButton::SafeDownCast(caller)
           && event == vtkKWPushButton::InvokedEvent)
    {
    int n = this->FittingTargetMenu->GetMenu()->GetIndexOfSelectedItem();
    const char* nodeID = this->FittingTargetMenuNodeList[n].c_str();
 
    //vtkIntArray* labels = this->IntensityCurves->GetLabelList();
    //int label = labels->GetValue(n);

    // Get the path to the script
    if (!this->CurveAnalysisScript)
      {
      vtkErrorMacro("Script is not selected.");
      return;
      }

    //vtkDoubleArray* curve = this->IntensityCurves->GetCurve(label);
    vtkMRMLArrayPlotNode* pnode = vtkMRMLArrayPlotNode::SafeDownCast(this->GetMRMLScene()->GetNodeByID(nodeID));
    if (pnode && pnode->GetArray())
      {
      // This section need to be reviewd: New()s and Delete()s are not corresponding.

      vtkMRMLDoubleArrayNode* anode = pnode->GetArray();
      vtkDoubleArray* curve = anode->GetArray();
      if (curve)
        {
        // Add a new vtkMRMLPharmacokineticsCurveAnalysisNode to the MRML scene
        if ( this->CurveAnalysisNode == NULL )
          {
          this->CurveAnalysisNode= vtkMRMLPharmacokineticsCurveAnalysisNode::New();
          //--- TODO SetAndObserve...
          }

        this->GetMRMLScene()->AddNode(this->CurveAnalysisNode);
        this->CurveAnalysisScript->SetPharmacokineticsCurveAnalysisNode(this->CurveAnalysisNode);
        this->CurveAnalysisScript->GetInfo();
        
        // Prepare vtkDoubleArray to pass the source cueve data
        vtkDoubleArray* srcCurve = vtkDoubleArray::New();
        srcCurve->SetNumberOfComponents( curve->GetNumberOfComponents() );
        
        // Check the range of the curve to fit
        int max   = curve->GetNumberOfTuples();
        int start = (int)this->CurveFittingStartIndexSpinBox->GetValue();
        int end   = (int)this->CurveFittingEndIndexSpinBox->GetValue();
        if (start < 0)   start = 0;
        if (end >= max)  end   = max-1;
        if (start > end) start = end;
        for (int i = start; i <= end; i ++)
          {
          double* xy = curve->GetTuple(i);
          srcCurve->InsertNextTuple(xy);
          }
        
        // Get initial parameters for the curve fitting
        GetInitialParametersAndInputCurves(this->CurveAnalysisNode, start, end);
        
        // Prepare vtkDoubleArray to receive a fitted curve from the script
        // (The size of the fitted curve should be the same as original
        // intensity curve)
        vtkDoubleArray* fittedCurve = vtkDoubleArray::New();
        fittedCurve->SetNumberOfComponents(2);
        for (int i = start; i < end; i ++)
          {
          double* xy = curve->GetTuple(i);
          fittedCurve->InsertNextTuple(xy);
          }
        
        // Set source and fitted curve arrays to the curve analysis node.
        this->CurveAnalysisNode->SetTargetCurve(srcCurve);
        this->CurveAnalysisNode->SetFittedCurve(fittedCurve);
        
        // Call Logic to excecute the curve fitting script
        //this->GetLogic()->RunCurveFitting(script, this->CurveAnalysisNode);
        this->CurveAnalysisScript->Run();
        
        // Display result parameters
        UpdateOutputParameterList(this->CurveAnalysisNode);
        
        vtkDoubleArray* resultCurve = this->CurveAnalysisNode->GetFittedCurve();
        vtkMRMLDoubleArrayNode* resultCurveNode = vtkMRMLDoubleArrayNode::New();
        this->GetMRMLScene()->AddNode(resultCurveNode);
        resultCurveNode->SetArray(resultCurve);
        
        vtkMRMLArrayPlotNode* plotNode = vtkMRMLArrayPlotNode::New();
        this->GetMRMLScene()->AddNode(plotNode);
        plotNode->SetAndObserveArray(resultCurveNode);
        
        plotNode->SetColor(1.0, 0.0, 0.0);
        this->PlotManagerNode->AddPlotNode(plotNode);
        this->PlotManagerNode->SetAutoXRange(1);
        this->PlotManagerNode->SetAutoYRange(1);
        this->PlotManagerNode->Refresh();
        
        resultCurveNode->Delete();
        plotNode->Delete();
        }
      }
    }
  else if (this->ParameterMapRegionButtonSet->GetWidget()->GetWidget(0) == vtkKWRadioButton::SafeDownCast(caller) &&
           event == vtkKWRadioButton::SelectedStateChangedEvent &&
           this->ParameterMapRegionButtonSet->GetWidget()->GetWidget(0)->GetSelectedState() == 1)
    {
    // "Region by label" is selected.
    
   // Turn on the widgets in "Region by label"
    this->MapRegionMaskSelectorWidget->SetState(1);
    this->MapRegionMaskLabelEntry->SetState(1);

    // Turn off the widgets in "Region by indices"
    this->MapIMinSpinBox->SetState(0);
    this->MapIMaxSpinBox->SetState(0);
    this->MapJMinSpinBox->SetState(0);
    this->MapJMaxSpinBox->SetState(0);
    this->MapKMinSpinBox->SetState(0);
    this->MapKMaxSpinBox->SetState(0);
    }
  else if (this->ParameterMapRegionButtonSet->GetWidget()->GetWidget(1) == vtkKWRadioButton::SafeDownCast(caller) &&
           event == vtkKWRadioButton::SelectedStateChangedEvent &&
           this->ParameterMapRegionButtonSet->GetWidget()->GetWidget(1)->GetSelectedState() == 1)
    {
    // "Region by indecies" is selected.
    
    // Turn off the widgets in "Region by label"
    this->MapRegionMaskSelectorWidget->SetState(0);
    this->MapRegionMaskLabelEntry->SetState(0);

    // Turn on the widgets in "Region by indices"
    this->MapIMinSpinBox->SetState(1);
    this->MapIMaxSpinBox->SetState(1);
    this->MapJMinSpinBox->SetState(1);
    this->MapJMaxSpinBox->SetState(1);
    this->MapKMinSpinBox->SetState(1);
    this->MapKMaxSpinBox->SetState(1);
    }
  else if (this->GenerateMapButton == vtkKWPushButton::SafeDownCast(caller)
           && event == vtkKWPushButton::InvokedEvent)
    {
    if (this->ParameterMapRegionButtonSet->GetWidget()->GetWidget(0)->GetSelectedState() == 1)
      {
      // The region is specified by mask
      vtkMRMLScalarVolumeNode *volumeNode = 
        vtkMRMLScalarVolumeNode::SafeDownCast(this->MapRegionMaskSelectorWidget->GetSelected());
      int label = this->MapRegionMaskLabelEntry->GetWidget()->GetValueAsInt();
      if (!volumeNode->GetLabelMap())
        {
        // TODO: display warning that the mask is not label map
        }
      else
        {
        if ( this->CurveAnalysisNode == NULL )
          {
          this->CurveAnalysisNode =vtkMRMLPharmacokineticsCurveAnalysisNode::New();
          //TODO SetAndObserve
          }
        this->GetMRMLScene()->AddNode(this->CurveAnalysisNode);
        this->CurveAnalysisScript->SetPharmacokineticsCurveAnalysisNode(this->CurveAnalysisNode);
        this->CurveAnalysisScript->GetInfo();

        const char* prefix   = this->MapOutputVolumePrefixEntry->GetWidget()->GetValue();
        int start = (int)this->CurveFittingStartIndexSpinBox->GetValue();
        int end   = (int)this->CurveFittingEndIndexSpinBox->GetValue();

        GetInitialParametersAndInputCurves(this->CurveAnalysisNode, start, end);
        vtkMRMLTimeSeriesBundleNode *bundleNode = 
          vtkMRMLTimeSeriesBundleNode::SafeDownCast(this->Active4DBundleSelectorWidget->GetSelected());

        if (prefix && bundleNode && this->CurveAnalysisScript)
          {
          this->GetLogic()->AddObserver(vtkPharmacokineticsLogic::ProgressDialogEvent,  this->LogicCallbackCommand);
          this->GetLogic()->GenerateParameterMapInMask(this->CurveAnalysisScript,
                                                       this->CurveAnalysisNode, bundleNode, prefix,
                                                       start, end, volumeNode, label);
          this->GetLogic()->RemoveObservers(vtkPharmacokineticsLogic::ProgressDialogEvent,  this->LogicCallbackCommand);
          }
        else
          {
          // TODO print out error message
          }
        //this->CurveAnalysisNode->Delete();
        }
      }
    else
      {
      // The region is specified by indices
      // Add a new vtkMRMLPharmacokineticsCurveAnalysisNode to the MRML scene
      if ( this->CurveAnalysisNode == NULL )
        {
        this->CurveAnalysisNode = vtkMRMLPharmacokineticsCurveAnalysisNode::New();
        // TODO SetAndObserve
        }
      this->GetMRMLScene()->AddNode(this->CurveAnalysisNode);
      this->CurveAnalysisScript->SetPharmacokineticsCurveAnalysisNode(this->CurveAnalysisNode);
      this->CurveAnalysisScript->GetInfo();
      
      int start = (int)this->CurveFittingStartIndexSpinBox->GetValue();
      int end   = (int)this->CurveFittingEndIndexSpinBox->GetValue();
      
      GetInitialParametersAndInputCurves(this->CurveAnalysisNode, start, end);
      
      const char* prefix   = this->MapOutputVolumePrefixEntry->GetWidget()->GetValue();
      
      int imin = (int)this->MapIMinSpinBox->GetValue();
      int imax = (int)this->MapIMaxSpinBox->GetValue();
      int jmin = (int)this->MapJMinSpinBox->GetValue();
      int jmax = (int)this->MapJMaxSpinBox->GetValue();
      int kmin = (int)this->MapKMinSpinBox->GetValue();
      int kmax = (int)this->MapKMaxSpinBox->GetValue();
      
      vtkMRMLTimeSeriesBundleNode *bundleNode = 
        vtkMRMLTimeSeriesBundleNode::SafeDownCast(this->Active4DBundleSelectorWidget->GetSelected());
      if (prefix && bundleNode && this->CurveAnalysisScript)
        {
        this->GetLogic()->AddObserver(vtkPharmacokineticsLogic::ProgressDialogEvent,  this->LogicCallbackCommand);
        
        this->GetLogic()->GenerateParameterMap(this->CurveAnalysisScript,
                                               this->CurveAnalysisNode,
                                               bundleNode,
                                               prefix,
                                               start, end,
                                               imin, imax, jmin, jmax, kmin, kmax);
        this->GetLogic()->RemoveObservers(vtkPharmacokineticsLogic::ProgressDialogEvent,  this->LogicCallbackCommand);
        }
      else
        {
        // TODO: show error message here ..
        }
      //this->CurveAnalysisNode->Delete();
      }
    }
#endif // Pharmacokinetics_USE_SCIPY
} 


//---------------------------------------------------------------------------
void vtkPharmacokineticsGUI::DataCallback(vtkObject *caller, 
                                     unsigned long eid, void *clientData, void *callData)
{
  vtkPharmacokineticsGUI *self = reinterpret_cast<vtkPharmacokineticsGUI *>(clientData);
  vtkDebugWithObjectMacro(self, "In vtkPharmacokineticsGUI DataCallback");
  self->UpdateAll();
}


//---------------------------------------------------------------------------
void vtkPharmacokineticsGUI::ProcessLogicEvents ( vtkObject *caller,
                                             unsigned long event, void *callData )
{
  if (event == vtkPharmacokineticsLogic::StatusUpdateEvent)
    {
    //this->UpdateDeviceStatus();
    }
  else if (event ==  vtkCommand::ProgressEvent) 
    {
    double progress = *((double *)callData);
    this->GetApplicationGUI()->GetMainSlicerWindow()->GetProgressGauge()->SetValue(100*progress);
    }
  else if (event == vtkPharmacokineticsLogic::ProgressDialogEvent)
    {
    vtkPharmacokineticsLogic::StatusMessageType* smsg
      = (vtkPharmacokineticsLogic::StatusMessageType*)callData;
    if (smsg->show)
      {
      if (!this->ProgressDialog)
        {
        this->ProgressDialog = vtkKWProgressDialog::New();
        this->ProgressDialog->SetParent(this->GetApplicationGUI()->GetMainSlicerWindow());
        this->ProgressDialog->SetMasterWindow(this->GetApplicationGUI()->GetMainSlicerWindow());
        this->ProgressDialog->Create();
        }
      this->ProgressDialog->SetMessageText(smsg->message.c_str());
      this->ProgressDialog->UpdateProgress(smsg->progress);
      this->ProgressDialog->Display();
      }
    else
      {
      if (this->ProgressDialog)
        {
        this->ProgressDialog->SetParent(NULL);
        this->ProgressDialog->Delete();
        this->ProgressDialog = NULL;
        }
      }
    }

}


//---------------------------------------------------------------------------
void vtkPharmacokineticsGUI::ProcessMRMLEvents ( vtkObject *caller,
                                            unsigned long event, void *callData )
{

  if (event == vtkMRMLScene::NodeAddedEvent)
    {
    //vtkObject* obj = (vtkObject*)callData;
    //vtkMRMLNode* node = vtkMRMLNode::SafeDownCast(obj);
    //if (node && strcmp(node->GetClassName(), "vtkMRMLArrayPlotNode") == 0)
    //  {
    //  UpdateFittingTargetMenu();
    //  }
    }
  else if (event == vtkMRMLScene::NodeRemovedEvent)
    {
    }
  else if (event == vtkMRMLScene::SceneCloseEvent)
    {
    }
  else if (event == vtkCommand::ModifiedEvent)
    {
    if (this->PlotManagerNode == vtkMRMLXYPlotManagerNode::SafeDownCast(caller))
      {
      UpdatePlotList();
      UpdateFittingTargetMenu();
      }
    //else if (vtkMRMLArrayPlotNode::SafeDownCast(caller))
    //  {
    //  UpdateFittingTargetMenu();
    //  }
    //
    }    
}


//---------------------------------------------------------------------------
void vtkPharmacokineticsGUI::ProcessTimerEvents()
{
  if (this->TimerFlag)
    {
    // update timer
    vtkKWTkUtilities::CreateTimerHandler(vtkKWApplication::GetMainInterp(), 
                                         this->TimerInterval,
                                         this, "ProcessTimerEvents");        
    }
}


//---------------------------------------------------------------------------
void vtkPharmacokineticsGUI::BuildGUI ( )
{

  // ---
  // MODULE GUI FRAME 
  // create a page
  this->UIPanel->AddPage ( "Pharmacokinetics", "Pharmacokinetics", NULL );

  BuildGUIForHelpFrame();
  BuildGUIForActiveBundleSelectorFrame();
  BuildGUIForFrameControlFrame(0);
  BuildGUIForFunctionViewer(0);
  BuildGUIForMethodSetting(0);
  BuildGUIForCurveFitting(0);
  BuildGUIForMapGenerator(0);

  //---
  //--- Initialize according to state in node
  //---
//  this->ModelSelectButton->SetParent ( modelframe );
}


//---------------------------------------------------------------------------
void vtkPharmacokineticsGUI::BuildGUIForActiveBundleSelectorFrame ()
{
  //vtkSlicerApplication *app = (vtkSlicerApplication *)this->GetApplication();
  vtkKWWidget *page = this->UIPanel->GetPageWidget ("Pharmacokinetics");

  //  Volume to select
  this->Active4DBundleSelectorWidget = vtkSlicerNodeSelectorWidget::New() ;
  this->Active4DBundleSelectorWidget->SetParent(page);
  this->Active4DBundleSelectorWidget->Create();
  this->Active4DBundleSelectorWidget->SetNodeClass("vtkMRMLTimeSeriesBundleNode", NULL, NULL, NULL);
  this->Active4DBundleSelectorWidget->SetMRMLScene(this->GetMRMLScene());
  this->Active4DBundleSelectorWidget->SetBorderWidth(2);
  this->Active4DBundleSelectorWidget->GetWidget()->GetWidget()->IndicatorVisibilityOff();
  this->Active4DBundleSelectorWidget->GetWidget()->GetWidget()->SetWidth(24);
  this->Active4DBundleSelectorWidget->SetLabelText( "Select Time-Series: ");
  this->Active4DBundleSelectorWidget->SetBalloonHelpString("Select a volume from the current scene.");
  this->Script("pack %s -side top -anchor nw -fill x -padx 2 -pady 2",
               this->Active4DBundleSelectorWidget->GetWidgetName());

}


//---------------------------------------------------------------------------
void vtkPharmacokineticsGUI::BuildGUIForHelpFrame ()
{
  // ----------------------------------------------------------------
  // HELP FRAME         
  // ----------------------------------------------------------------

  // Define your help text here.
  const char *help = 
    "**The Pharmacokinetics Module** helps you to load, view and analyze a series of 3D images such as perfusion MRI, and DCE CT/MRI. \n\n Please see <a>http://www.slicer.org/slicerWiki/index.php/Modules:Pharmacokinetics</a> for details.\n\n";
  const char *about =
    "This project is a combined effort  by the groups directed by Hiroto Hatabu, MD, PhD (BWH). Fiona Fennessy, MD, PhD (BWH). The module is designed and implemented by Junichi Tokuda, PhD (BWH) with support from Center for Pulmonary Functional Imaging at BWH, NCIGT and NA-MIC, Wendy Plesniak, PhD (BWH) with support from the Harvard CTSC, and contributors from GE Global Research.\n\n";
  vtkKWWidget *page = this->UIPanel->GetPageWidget ( "Pharmacokinetics" );
  this->BuildHelpAndAboutFrame (page, help, about);

}


//---------------------------------------------------------------------------
void vtkPharmacokineticsGUI::BuildGUIForFrameControlFrame(int show)
{

  vtkSlicerApplication *app = (vtkSlicerApplication *)this->GetApplication();
  vtkKWWidget *page = this->UIPanel->GetPageWidget ("Pharmacokinetics");
  
  vtkSlicerModuleCollapsibleFrame *conBrowsFrame = vtkSlicerModuleCollapsibleFrame::New();

  conBrowsFrame->SetParent(page);
  conBrowsFrame->Create();
  conBrowsFrame->SetLabelText("Time-Point Selection & Display");
  if (!show)
    {
    conBrowsFrame->CollapseFrame();
    }
  app->Script ("pack %s -side top -anchor nw -fill x -padx 2 -pady 2 -in %s",
               conBrowsFrame->GetWidgetName(), page->GetWidgetName());


  // -----------------------------------------
  // Frame Control

  vtkKWFrameWithLabel *fframe = vtkKWFrameWithLabel::New();
  fframe->SetParent(conBrowsFrame->GetFrame());
  fframe->Create();
  fframe->SetLabelText ("Frame");
  this->Script ( "pack %s -side top -fill x -expand y -anchor w -padx 2 -pady 2",
                 fframe->GetWidgetName() );
  
  //vtkKWFrame *sframe = vtkKWFrame::New();
  //sframe->SetParent(fframe->GetFrame());
  //sframe->Create();
  //this->Script ( "pack %s -side top -fill x -expand y -anchor w -padx 2 -pady 2",
  //               sframe->GetWidgetName() );

  vtkKWFrame *fgframe = vtkKWFrame::New();
  fgframe->SetParent(fframe->GetFrame());
  fgframe->Create();
  this->Script ( "pack %s -side top -fill x -expand y -anchor w -padx 2 -pady 2",
                 fgframe->GetWidgetName() );

  this->ForegroundVolumeSelectorScale = vtkKWScaleWithEntry::New();
  this->ForegroundVolumeSelectorScale->SetParent(fgframe);
  this->ForegroundVolumeSelectorScale->Create();
  this->ForegroundVolumeSelectorScale->SetEntryPosition(vtkKWScaleWithEntry::EntryPositionRight);
  this->ForegroundVolumeSelectorScale->SetOrientationToHorizontal();
  this->ForegroundVolumeSelectorScale->SetLabelText("FG");
  this->ForegroundVolumeSelectorScale->SetRange(0.0, 100.0);
  this->ForegroundVolumeSelectorScale->SetResolution(1.0);
  this->ForegroundVolumeSelectorScale->ExpandEntryOff();
  //this->ForegroundVolumeSelectorScale->SetWidth(30);

  this->Script("pack %s -side right -fill x -expand y -padx 2 -pady 2", 
               this->ForegroundVolumeSelectorScale->GetWidgetName());

  vtkKWFrame *bgframe = vtkKWFrame::New();
  bgframe->SetParent(fframe->GetFrame());
  bgframe->Create();
  this->Script ( "pack %s -side top -fill x -expand y -anchor w -padx 2 -pady 2",
                 bgframe->GetWidgetName() );

  this->BackgroundVolumeSelectorScale = vtkKWScaleWithEntry::New();
  this->BackgroundVolumeSelectorScale->SetParent(bgframe);
  this->BackgroundVolumeSelectorScale->Create();
  this->BackgroundVolumeSelectorScale->SetEntryPosition(vtkKWScaleWithEntry::EntryPositionRight);
  this->BackgroundVolumeSelectorScale->SetOrientationToHorizontal();
  this->BackgroundVolumeSelectorScale->SetLabelText("BG");
  this->BackgroundVolumeSelectorScale->SetRange(0.0, 100.0);
  this->BackgroundVolumeSelectorScale->SetResolution(1.0);
  this->BackgroundVolumeSelectorScale->ExpandEntryOff();
  //this->BackgroundVolumeSelectorScale->SetWidth(30);
  
  this->Script("pack %s -side right -fill x -expand y -padx 2 -pady 2", 
               this->BackgroundVolumeSelectorScale->GetWidgetName());

  vtkKWFrame *apframe = vtkKWFrame::New();
  apframe->SetParent(fframe->GetFrame());
  apframe->Create();
  this->Script ( "pack %s -side top -fill x -expand y -anchor w -padx 2 -pady 2",
                 apframe->GetWidgetName() );
  
  // -----------------------------------------
  // Contrast control

  vtkKWFrameWithLabel *cframe = vtkKWFrameWithLabel::New();
  cframe->SetParent(conBrowsFrame->GetFrame());
  cframe->Create();
  cframe->SetLabelText ("Appearance");
  this->Script ( "pack %s -side top -fill x -expand y -anchor w -padx 2 -pady 2",
                 cframe->GetWidgetName() );

  vtkKWFrame *lwframe = vtkKWFrame::New();
  lwframe->SetParent(cframe->GetFrame());
  lwframe->Create();
  this->Script ( "pack %s -side top -fill x -expand y -anchor w -padx 2 -pady 2",
                 lwframe->GetWidgetName() );

  vtkKWLabel *lwLabel = vtkKWLabel::New();
  lwLabel->SetParent(lwframe);
  lwLabel->Create();
  lwLabel->SetWidth (14);
  lwLabel->SetText("Window/Level: ");

  this->WindowLevelRange = vtkKWRange::New();
  this->WindowLevelRange->SetParent(lwframe);
  this->WindowLevelRange->Create();
  this->WindowLevelRange->SymmetricalInteractionOn();
  this->WindowLevelRange->EntriesVisibilityOff ();  
  this->WindowLevelRange->SetWholeRange(0.0, 1.0);
  /*
  this->WindowLevelRange->SetCommand(this, "ProcessWindowLevelCommand");
  this->WindowLevelRange->SetStartCommand(this, "ProcessWindowLevelStartCommand");
  */
  //--- grid
  app->Script ( "grid columnconfigure %s 0 -weight 0", lwframe->GetWidgetName() );
  app->Script ( "grid columnconfigure %s 1 -weight 1", lwframe->GetWidgetName() );
  app->Script("grid %s -row 0 -column 0 -sticky se  -padx 2 -pady 2", lwLabel->GetWidgetName() );
  app->Script("grid %s -row 0 -column 1 -sticky swe -padx 4 -pady 2", this->WindowLevelRange->GetWidgetName() );

  vtkKWFrame *thframe = vtkKWFrame::New();
  thframe->SetParent(cframe->GetFrame());
  thframe->Create();
  this->Script ( "pack %s -side top -fill x -expand y -anchor w -padx 2 -pady 2",
                 thframe->GetWidgetName() );

  vtkKWLabel *thLabel = vtkKWLabel::New();
  thLabel->SetParent(thframe);
  thLabel->Create();
  thLabel->SetWidth (14);
  thLabel->SetText("Threshold: ");

  this->ThresholdRange = vtkKWRange::New();
  this->ThresholdRange->SetParent(thframe);
  this->ThresholdRange->EntriesVisibilityOff ();
  this->ThresholdRange->Create();
  this->ThresholdRange->SymmetricalInteractionOff();
  this->ThresholdRange->SetWholeRange(0.0, 1.0);
  /*
  this->ThresholdRange->SetCommand(this, "ProcessThresholdCommand");
  this->ThresholdRange->SetStartCommand(this, "ProcessThresholdStartCommand");
  */
  //--- grid
  app->Script ( "grid columnconfigure %s 0 -weight 0", thframe->GetWidgetName() );
  app->Script ( "grid columnconfigure %s 1 -weight 1", thframe->GetWidgetName() );
  app->Script("grid %s -row 0 -column 0 -sticky se  -padx 2 -pady 2", thLabel->GetWidgetName() );
  app->Script("grid %s -row 0 -column 1 -sticky swe -padx 4 -pady 2", this->ThresholdRange->GetWidgetName() );

  // -----------------------------------------
  // Delete pointers

  conBrowsFrame->Delete();
  fframe->Delete();
  fgframe->Delete();
  bgframe->Delete();
  apframe->Delete();
  cframe->Delete();
  lwframe->Delete();
  lwLabel->Delete();
  thframe->Delete();
  thLabel->Delete();
  //sframe->Delete();

}


//---------------------------------------------------------------------------
void vtkPharmacokineticsGUI::BuildGUIForFunctionViewer(int show)
{


  vtkSlicerApplication *app = (vtkSlicerApplication *)this->GetApplication();
  vtkKWWidget *page = this->UIPanel->GetPageWidget ("Pharmacokinetics");
  
  vtkSlicerModuleCollapsibleFrame *conBrowsFrame = vtkSlicerModuleCollapsibleFrame::New();

  conBrowsFrame->SetParent(page);
  conBrowsFrame->Create();
  conBrowsFrame->SetLabelText("Intensity Plotting");
  if (!show)
    {
    conBrowsFrame->CollapseFrame();
    }
  app->Script ("pack %s -side top -anchor nw -fill x -padx 2 -pady 2 -in %s",
               conBrowsFrame->GetWidgetName(), page->GetWidgetName());

  // -----------------------------------------
  // Mask selector frame
  
  vtkKWFrameWithLabel *msframe = vtkKWFrameWithLabel::New();
  msframe->SetParent(conBrowsFrame->GetFrame());
  msframe->Create();
  msframe->SetLabelText ("Generate Curves");
  this->Script ( "pack %s -side top -fill x -expand y -anchor w -padx 2 -pady 2",
                 msframe->GetWidgetName() );

  vtkKWFrame* mframe = vtkKWFrame::New();
  mframe->SetParent(msframe->GetFrame());
  mframe->Create();
  this->Script ( "pack %s -side top -fill x -expand y -anchor w -padx 2 -pady 2",
                 mframe->GetWidgetName() );

  this->MaskNodeSelector = vtkSlicerNodeSelectorWidget::New();
  this->MaskNodeSelector->SetParent(mframe);
  this->MaskNodeSelector->Create();
  this->MaskNodeSelector->SetNodeClass("vtkMRMLScalarVolumeNode", NULL, NULL, NULL);
  this->MaskNodeSelector->SetNewNodeEnabled(0);
  this->MaskNodeSelector->SetMRMLScene(this->GetMRMLScene());
  this->MaskNodeSelector->SetBorderWidth(2);
  this->MaskNodeSelector->GetWidget()->GetWidget()->IndicatorVisibilityOff();
  this->MaskNodeSelector->GetWidget()->GetWidget()->SetWidth(24);
  this->MaskNodeSelector->SetLabelText( "VOI Mask: ");
  this->MaskNodeSelector->SetBalloonHelpString("Select a mask to specify regions of interest.");

  this->Script("pack %s -side left -fill x -expand y -anchor w -padx 2 -pady 2", 
               this->MaskNodeSelector->GetWidgetName() );

  vtkKWFrame* vframe = vtkKWFrame::New();
  vframe->SetParent(msframe->GetFrame());
  vframe->Create();
  this->Script ( "pack %s -side top -fill x -expand y -anchor w -padx 2 -pady 2",
                 vframe->GetWidgetName() );

  this->ValueTypeButtonSet = vtkKWRadioButtonSetWithLabel::New();
  this->ValueTypeButtonSet->SetParent(vframe);
  this->ValueTypeButtonSet->Create();
  this->ValueTypeButtonSet->SetLabelText("Value to plot: ");
  this->ValueTypeButtonSet->GetWidget()->PackHorizontallyOn();
  this->ValueTypeButtonSet->GetWidget()->UniformColumnsOn();
  this->ValueTypeButtonSet->GetWidget()->UniformRowsOn();
  this->ValueTypeButtonSet->GetWidget()->AddWidget(0);
  this->ValueTypeButtonSet->GetWidget()->GetWidget(0)->SetText("Mean");
  this->ValueTypeButtonSet->GetWidget()->AddWidget(1);
  this->ValueTypeButtonSet->GetWidget()->GetWidget(1)->SetText("Max");
  this->ValueTypeButtonSet->GetWidget()->AddWidget(2);
  this->ValueTypeButtonSet->GetWidget()->GetWidget(2)->SetText("Min");
  this->ValueTypeButtonSet->GetWidget()->GetWidget(0)->SelectedStateOn();
  
  app->Script("pack %s -side top -anchor w -fill x -padx 2 -pady 2", 
              this->ValueTypeButtonSet->GetWidgetName());

  vtkKWFrame* cframe = vtkKWFrame::New();
  cframe->SetParent(msframe->GetFrame());
  cframe->Create();
  this->Script ( "pack %s -side top -fill x -expand y -anchor w -padx 2 -pady 2",
                 cframe->GetWidgetName() );

  this->GenerateCurveButton = vtkKWPushButton::New();
  this->GenerateCurveButton->SetParent(cframe);
  this->GenerateCurveButton->Create();
  this->GenerateCurveButton->SetText ("Generate");
  this->GenerateCurveButton->SetWidth (8);

  this->Script("pack %s -side top -anchor center -expand y -padx 2 -pady 2", 
               this->GenerateCurveButton->GetWidgetName());

  // -----------------------------------------
  // Plot frame
  vtkKWFrameWithLabel *frame = vtkKWFrameWithLabel::New();
  frame->SetParent(conBrowsFrame->GetFrame());
  frame->Create();
  frame->SetLabelText ("Intensity Plot");
  this->Script ( "pack %s -side top -fill x -expand y -anchor w -padx 2 -pady 2",
                 frame->GetWidgetName() );
  
  this->IntensityPlot = vtkSlicerXYPlotWidget::New();
  this->IntensityPlot->SetParent(frame->GetFrame());
  this->IntensityPlot->Create();
  this->IntensityPlot->SetHeight(250);

  this->IntensityPlot->SetAxisLineColor(1.0, 1.0, 1.0);
  this->IntensityPlot->SetMRMLScene(this->GetMRMLScene());
  if (this->PlotManagerNode)
    {
    this->IntensityPlot->SetAndObservePlotManagerNode(this->PlotManagerNode);
    }

  vtkKWFrame* eframe = vtkKWFrame::New();
  eframe->SetParent(frame->GetFrame() );
  eframe->Create();
  this->Script ( "pack %s -side top -fill x -expand y -anchor w -padx 2 -pady 2",
                 eframe->GetWidgetName() );

  this->ErrorBarCheckButton = vtkKWCheckButtonWithLabel::New();
  this->ErrorBarCheckButton->SetParent(eframe );
  this->ErrorBarCheckButton->Create();
  this->ErrorBarCheckButton->GetWidget()->SelectedStateOn();
  this->ErrorBarCheckButton->SetLabelWidth ( 24 );
  this->ErrorBarCheckButton->SetLabelPositionToRight();
  this->ErrorBarCheckButton->SetLabelText("Show Standard Deviation");
  this->Script ( "pack %s -side top -anchor center -padx 2 -pady 2",
                 this->ErrorBarCheckButton->GetWidgetName() );

  this->PlotList = vtkKWMultiColumnListWithScrollbars::New();
  this->PlotList->SetParent(frame->GetFrame());
  this->PlotList->Create();
  this->PlotList->GetWidget()->SetHeight(6);
  this->PlotList->GetWidget()->SetReliefToRidge();
  this->PlotList->GetWidget()->SetBorderWidth(1);
  //this->PlotList->GetWidget()->SetSelectionTypeToRow();
  this->PlotList->GetWidget()->SetSelectionTypeToCell();
  this->PlotList->GetWidget()->SetSelectionModeToSingle();
  this->PlotList->GetWidget()->MovableRowsOff();
  this->PlotList->GetWidget()->MovableColumnsOff();
  this->PlotList->GetWidget()->AddColumn("");
  this->PlotList->GetWidget()->AddColumn("V");
  this->PlotList->GetWidget()->AddColumn("Color");
  this->PlotList->GetWidget()->AddColumn("Curve name");
  this->PlotList->GetWidget()->AddColumn("MRML ID");

  // On/off column
  this->PlotList->GetWidget()->SetColumnEditWindowToCheckButton(COLUMN_SELECT);
  this->PlotList->GetWidget()->SetColumnStretchable(COLUMN_SELECT, 0);
  this->PlotList->GetWidget()->SetColumnResizable(COLUMN_SELECT, 0);
  this->PlotList->GetWidget()->SetColumnWidth(COLUMN_SELECT, 0);
  this->PlotList->GetWidget()->ColumnEditableOn(COLUMN_SELECT);
  this->PlotList->GetWidget()->SetColumnFormatCommandToEmptyOutput (COLUMN_SELECT);

  // On/off column
  this->PlotList->GetWidget()->SetColumnEditWindowToCheckButton(COLUMN_VISIBLE);
  this->PlotList->GetWidget()->SetColumnStretchable(COLUMN_VISIBLE, 0);
  this->PlotList->GetWidget()->SetColumnResizable(COLUMN_VISIBLE, 0);
  this->PlotList->GetWidget()->SetColumnWidth(COLUMN_VISIBLE, 0);
  this->PlotList->GetWidget()->ColumnEditableOn(COLUMN_VISIBLE);
  this->PlotList->GetWidget()->SetColumnFormatCommandToEmptyOutput (COLUMN_VISIBLE);

  // Color column
  this->PlotList->GetWidget()->ColumnEditableOff(COLUMN_COLOR);
  this->PlotList->GetWidget()->SetColumnWidth(COLUMN_COLOR, 5);
  this->PlotList->GetWidget()->SetColumnResizable(COLUMN_COLOR, 0);

  // Curve name column
  this->PlotList->GetWidget()->ColumnEditableOn(COLUMN_NODE_NAME);
  this->PlotList->GetWidget()->SetColumnWidth(COLUMN_NODE_NAME, 15);
  this->PlotList->GetWidget()->SetColumnEditWindowToEntry(COLUMN_NODE_NAME);
  this->PlotList->GetWidget()->SetColumnAlignmentToLeft(COLUMN_NODE_NAME);
  this->PlotList->GetWidget()->SetColumnStretchable(COLUMN_NODE_NAME, 1);

  // MRML ID column
  this->PlotList->GetWidget()->ColumnEditableOff(COLUMN_MRML_ID);
  this->PlotList->GetWidget()->SetColumnWidth(COLUMN_MRML_ID, 15);
  this->PlotList->GetWidget()->SetColumnAlignmentToLeft(COLUMN_MRML_ID);
  this->PlotList->GetWidget()->SetColumnStretchable(COLUMN_MRML_ID, 1);
  
  this->Script("pack %s %s -side top -fill x -expand y -anchor w -padx 2 -pady 2", 
               this->IntensityPlot->GetWidgetName(),
               this->PlotList->GetWidgetName());

  // Button frame  
  vtkKWFrame *bframe = vtkKWFrame::New();
  bframe->SetParent(frame->GetFrame());
  bframe->Create();

  this->Script ("pack %s -side top -fill x -expand y -anchor w -padx 2 -pady 2",
                bframe->GetWidgetName() );

  this->ImportPlotButton = vtkKWPushButton::New();
  this->ImportPlotButton->SetParent(bframe);
  this->ImportPlotButton->Create();
  this->ImportPlotButton->SetText ("Import");
  this->ImportPlotButton->SetWidth (4);

  this->SelectAllPlotButton = vtkKWPushButton::New();
  this->SelectAllPlotButton->SetParent(bframe);
  this->SelectAllPlotButton->Create();
  this->SelectAllPlotButton->SetText ("Select All");
  this->SelectAllPlotButton->SetWidth (4);

  this->DeselectAllPlotButton = vtkKWPushButton::New();
  this->DeselectAllPlotButton->SetParent(bframe);
  this->DeselectAllPlotButton->Create();
  this->DeselectAllPlotButton->SetText ("Deselect All");
  this->DeselectAllPlotButton->SetWidth (4);

  this->PlotDeleteButton = vtkKWPushButton::New();
  this->PlotDeleteButton->SetParent(bframe);
  this->PlotDeleteButton->Create();
  this->PlotDeleteButton->SetText ("Delete");
  this->PlotDeleteButton->SetWidth (4);

  this->SavePlotButton = vtkKWPushButton::New();
  this->SavePlotButton->SetParent(bframe);
  this->SavePlotButton->Create();
  this->SavePlotButton->SetText ("Save");
  this->SavePlotButton->SetWidth (4);
  this->Script ("pack %s %s %s %s %s -side left -fill x -expand y -anchor w -padx 2 -pady 2",
                this->ImportPlotButton->GetWidgetName(),
                this->SelectAllPlotButton->GetWidgetName(),
                this->DeselectAllPlotButton->GetWidgetName(),
                this->PlotDeleteButton->GetWidgetName(),
                this->SavePlotButton->GetWidgetName());

  // Event handlers:
  this->PlotList->GetWidget()->SetCellUpdatedCommand(this, "UpdatePlotListElement");
  //this->PlotList->GetWidget()->SetRightClickCommand(this, "OnClickPlotList");
  //this->PlotList->GetWidget()->SetSelectionCommand( this, "CurveVisibilityToggle");

  msframe->Delete();
  mframe->Delete();
  eframe->Delete();
  vframe->Delete();
  cframe->Delete();
  frame->Delete();
  bframe->Delete();
  conBrowsFrame->Delete();


}



//----------------------------------------------------------------------------
void vtkPharmacokineticsGUI::BuildGUIForMethodSetting(int show)
{
  vtkSlicerApplication *app = (vtkSlicerApplication *)this->GetApplication();
  vtkKWWidget *page = this->UIPanel->GetPageWidget ("Pharmacokinetics");
  
  vtkSlicerModuleCollapsibleFrame *conBrowsFrame = vtkSlicerModuleCollapsibleFrame::New();

  conBrowsFrame->SetParent(page);
  conBrowsFrame->Create();
  conBrowsFrame->SetLabelText("Pharmacokinetic Models & Parameters");
  if (!show)
    {
    conBrowsFrame->CollapseFrame();
    }
  app->Script ("pack %s -side top -anchor nw -fill x -padx 2 -pady 2 -in %s",
               conBrowsFrame->GetWidgetName(), page->GetWidgetName());

  // -----------------------------------------
  // Curve fitting Setting frame
  
  vtkKWFrameWithLabel *cframe = vtkKWFrameWithLabel::New();
  cframe->SetParent(conBrowsFrame->GetFrame());
  cframe->Create();
  cframe->SetLabelText ("Models & Initial Parameters");
  this->Script ( "pack %s -side top -fill x -expand y -anchor w -padx 2 -pady 2",
                 cframe->GetWidgetName() );

  vtkKWFrame *modelframe = vtkKWFrame::New();
  modelframe->SetParent(cframe->GetFrame());
  modelframe->Create();
  this->Script ("pack %s -side top -fill x -expand y -anchor w -padx 2 -pady 2",
                modelframe->GetWidgetName() );

  if ( this->Logic )
    {
    //--- Make sure all known models are available.
    this->Logic->AddKnownPharmacokineticModels();
  
    vtkKWLabel *modelLabel = vtkKWLabel::New();
    modelLabel->SetParent(modelframe);
    modelLabel->Create();
    modelLabel->SetWidth (21);
    modelLabel->SetText("Select Model: ");
    this->Script("pack %s -side left -anchor e -padx 2 -pady 2", 
                 modelLabel->GetWidgetName());

    this->ModelSelectButton = vtkKWMenuButton::New();
    this->ModelSelectButton->SetParent ( modelframe );
    this->ModelSelectButton->Create();
    vtkKWMenu *msmenu = this->ModelSelectButton->GetMenu();
    //--- populate menu
    int numModels = this->Logic->GetNumberOfPharmacokineticModels();
    std::string modName;
    for ( int zz = 0; zz < numModels; zz++ )
      {
      modName.clear();
      modName = this->Logic->GetNthPharmacokineticModelName ( zz );
      if ( !modName.empty() )
        {
        msmenu->AddRadioButton (modName.c_str() );
        //--- for now, since some of these python models are not integrated... disable in menu
        if ( zz!=0)
          {
          msmenu->SetItemStateToDisabled ( msmenu->GetIndexOfItem ( modName.c_str()) );
          }
        }
      }
    //--- select default.
    msmenu->SelectItem ( 0 );
    msmenu->AddSeparator();
    msmenu->AddCommand ("close");
    this->Script("pack %s -side left -fill x -expand y -padx 2 -pady 2", 
                 this->ModelSelectButton->GetWidgetName());  

#ifdef Pharmacokinetics_USE_SCIPY  
    this->CurveScriptSelectButton = vtkKWLoadSaveButtonWithLabel::New();
    this->CurveScriptSelectButton->SetParent(modelframe);
    this->CurveScriptSelectButton->Create();
    this->CurveScriptSelectButton->GetWidget()->SetText ("Script Path");
    this->CurveScriptSelectButton->GetWidget()->GetLoadSaveDialog()->SaveDialogOff();
    this->Script("pack %s -side right -fill x -expand y -padx 2 -pady 2", 
                 this->CurveScriptSelectButton->GetWidgetName());
    this->CurveScriptMethodName = vtkKWEntryWithLabel::New();
    this->CurveScriptMethodName->SetParent(cframe->GetFrame());
    this->CurveScriptMethodName->Create();
    //this->CurveScriptMethodName->SetWidth(20);
    this->CurveScriptMethodName->SetLabelText("Method: ");
    this->CurveScriptMethodName->GetWidget()->ReadOnlyOn();
    this->CurveScriptMethodName->GetWidget()->SetValue("----");
    this->Script ( "pack %s -side top -fill x -expand y -anchor w -padx 2 -pady 2",
                   this->CurveScriptMethodName->GetWidgetName() );
#endif


    vtkKWFrame *rangeframe = vtkKWFrame::New();
    rangeframe->SetParent(cframe->GetFrame());
    rangeframe->Create();
    this->Script ("pack %s -side top -fill x -expand y -anchor w -padx 2 -pady 2",
                  rangeframe->GetWidgetName() );

    vtkKWLabel *startLabel = vtkKWLabel::New();
    startLabel->SetParent(rangeframe);
    startLabel->Create();
    startLabel->SetWidth (21);
    startLabel->SetText("Analyze frames from: ");
  
    this->CurveFittingStartIndexSpinBox = vtkKWSpinBox::New();
    this->CurveFittingStartIndexSpinBox->SetParent(rangeframe);
    this->CurveFittingStartIndexSpinBox->Create();
    this->CurveFittingStartIndexSpinBox->SetWidth(3);

    vtkKWLabel *endLabel = vtkKWLabel::New();
    endLabel->SetParent(rangeframe);
    endLabel->Create();
    endLabel->SetText("to: ");

    this->CurveFittingEndIndexSpinBox = vtkKWSpinBox::New();
    this->CurveFittingEndIndexSpinBox->SetParent(rangeframe);
    this->CurveFittingEndIndexSpinBox->Create();
    this->CurveFittingEndIndexSpinBox->SetWidth(3);

    this->Script("pack %s %s %s %s -side left -anchor w -anchor w -padx 2 -pady 2",
                 startLabel->GetWidgetName(),
                 this->CurveFittingStartIndexSpinBox->GetWidgetName(),
                 endLabel->GetWidgetName(),
                 this->CurveFittingEndIndexSpinBox->GetWidgetName());

    this->InitialParameterList = vtkKWMultiColumnListWithScrollbars::New();
    this->InitialParameterList->SetParent(cframe->GetFrame());
    this->InitialParameterList->Create();
    this->InitialParameterList->GetWidget()->SetHeight(10);
    this->InitialParameterList->GetWidget()->SetSelectionTypeToRow();
    this->InitialParameterList->GetWidget()->SetSelectionModeToSingle();
    this->InitialParameterList->GetWidget()->MovableRowsOff();
    this->InitialParameterList->GetWidget()->MovableColumnsOff();
    this->InitialParameterList->GetWidget()->AddColumn("Parameter name");
    this->InitialParameterList->GetWidget()->AddColumn("Initial value / curves");
    this->InitialParameterList->GetWidget()->SetColumnWidth(0, 16);
    this->InitialParameterList->GetWidget()->SetColumnWidth(1, 20);
    this->InitialParameterList->GetWidget()->SetColumnAlignmentToLeft(1);
    this->InitialParameterList->GetWidget()->ColumnEditableOff(0);
    this->InitialParameterList->GetWidget()->ColumnEditableOn(1);
    this->InitialParameterList->GetWidget()->SetSelectionTypeToCell();
    this->InitialParameterListInputType.clear();
    this->InitialParameterListNodeNames.clear();

    this->Script ( "pack %s -side top -fill x -expand y -anchor w -padx 2 -pady 2",
                   this->InitialParameterList->GetWidgetName() );

    this->InitialParameterList->GetWidget()->SetSelectionCommand(this, "OnInitialParameterListSelected");

    this->PlotSelectPopUpMenu = vtkKWMenu::New();
    this->PlotSelectPopUpMenu->SetParent(this->GetApplicationGUI()->GetMainSlicerWindow());
    this->PlotSelectPopUpMenu->Create();

    rangeframe->Delete();
    startLabel->Delete();
    endLabel->Delete();
    modelLabel->Delete();
    }

  modelframe->Delete();
  cframe->Delete();
  conBrowsFrame->Delete();
}




//----------------------------------------------------------------------------
void vtkPharmacokineticsGUI::BuildGUIForCurveFitting(int show)
{
  vtkSlicerApplication *app = (vtkSlicerApplication *)this->GetApplication();
  vtkKWWidget *page = this->UIPanel->GetPageWidget ("Pharmacokinetics");
  
  vtkSlicerModuleCollapsibleFrame *conBrowsFrame = vtkSlicerModuleCollapsibleFrame::New();

  conBrowsFrame->SetParent(page);
  conBrowsFrame->Create();
  conBrowsFrame->SetLabelText("Curve Fitting");
  if (!show)
    {
    conBrowsFrame->CollapseFrame();
    }
  app->Script ("pack %s -side top -anchor nw -fill x -padx 2 -pady 2 -in %s",
               conBrowsFrame->GetWidgetName(), page->GetWidgetName());

  // -----------------------------------------
  // Curve fitting
  
  vtkKWFrameWithLabel *oframe = vtkKWFrameWithLabel::New();
  oframe->SetParent(conBrowsFrame->GetFrame());
  oframe->Create();
  oframe->SetLabelText ("Results");
  this->Script ( "pack %s -side top -fill x -expand y -anchor w -padx 2 -pady 2",
                 oframe->GetWidgetName() );

  vtkKWFrame *runframe = vtkKWFrame::New();
  runframe->SetParent(oframe->GetFrame());
  runframe->Create();
  this->Script ("pack %s -side top -fill x -expand y -anchor w -padx 2 -pady 2",
                runframe->GetWidgetName() );


  vtkKWLabel *fittingLabelLabel = vtkKWLabel::New();
  fittingLabelLabel->SetParent(runframe);
  fittingLabelLabel->Create();
  fittingLabelLabel->SetText("Target:");

  this->FittingTargetMenu = vtkKWMenuButton::New();
  this->FittingTargetMenu->SetParent(runframe);
  this->FittingTargetMenu->Create();
  this->FittingTargetMenu->SetWidth(5);

  this->RunFittingButton = vtkKWPushButton::New();
  this->RunFittingButton->SetParent(runframe);
  this->RunFittingButton->Create();
  this->RunFittingButton->SetText ("Run");
  this->RunFittingButton->SetWidth (4);

  this->Script("pack %s %s %s -side left -fill x -expand y -anchor w -padx 2 -pady 2", 
               fittingLabelLabel->GetWidgetName(),
               this->FittingTargetMenu->GetWidgetName(),
               this->RunFittingButton->GetWidgetName());

  this->FittingTargetMenuNodeList.clear();

  this->ResultParameterList = vtkKWMultiColumnListWithScrollbars::New();
  this->ResultParameterList->SetParent(oframe->GetFrame());
  this->ResultParameterList->Create();
  this->ResultParameterList->GetWidget()->SetHeight(10);
  this->ResultParameterList->GetWidget()->SetSelectionTypeToRow();
  this->ResultParameterList->GetWidget()->SetSelectionModeToSingle();
  this->ResultParameterList->GetWidget()->MovableRowsOff();
  this->ResultParameterList->GetWidget()->MovableColumnsOff();
  this->ResultParameterList->GetWidget()->AddColumn("Parameter name");
  this->ResultParameterList->GetWidget()->AddColumn("Output value");
  this->ResultParameterList->GetWidget()->SetColumnWidth(0, 16);
  this->ResultParameterList->GetWidget()->SetColumnWidth(1, 20);
  this->ResultParameterList->GetWidget()->SetColumnAlignmentToLeft(1);
  this->ResultParameterList->GetWidget()->ColumnEditableOff(0);
  this->ResultParameterList->GetWidget()->ColumnEditableOff(1);
  this->ResultParameterList->GetWidget()->SetSelectionTypeToCell();

  this->Script ( "pack %s -side top -fill x -expand y -anchor w -padx 2 -pady 2",
                 this->ResultParameterList->GetWidgetName() );
  
  oframe->Delete();
  runframe->Delete();
  fittingLabelLabel->Delete();
  conBrowsFrame->Delete();
}



//----------------------------------------------------------------------------
void vtkPharmacokineticsGUI::BuildGUIForMapGenerator(int show)
{
  vtkSlicerApplication *app = (vtkSlicerApplication *)this->GetApplication();
  vtkKWWidget *page = this->UIPanel->GetPageWidget ("Pharmacokinetics");
  
  vtkSlicerModuleCollapsibleFrame *conBrowsFrame = vtkSlicerModuleCollapsibleFrame::New();

  conBrowsFrame->SetParent(page);
  conBrowsFrame->Create();
  conBrowsFrame->SetLabelText("Parameter Maps");
  if (!show)
    {
    conBrowsFrame->CollapseFrame();
    }
  app->Script ("pack %s -side top -anchor nw -fill x -padx 2 -pady 2 -in %s",
               conBrowsFrame->GetWidgetName(), page->GetWidgetName());

  this->ParameterMapRegionButtonSet = vtkKWRadioButtonSetWithLabel::New();
  this->ParameterMapRegionButtonSet->SetParent(conBrowsFrame->GetFrame());
  this->ParameterMapRegionButtonSet->Create();
  this->ParameterMapRegionButtonSet->SetLabelText("Region: ");
  this->ParameterMapRegionButtonSet->GetWidget()->PackHorizontallyOn();
  this->ParameterMapRegionButtonSet->GetWidget()->SetMaximumNumberOfWidgetsInPackingDirection(2);
  this->ParameterMapRegionButtonSet->GetWidget()->UniformColumnsOn();
  this->ParameterMapRegionButtonSet->GetWidget()->UniformRowsOn();
  this->ParameterMapRegionButtonSet->GetWidget()->AddWidget(0);
  this->ParameterMapRegionButtonSet->GetWidget()->GetWidget(0)->SetText("by mask");
  this->ParameterMapRegionButtonSet->GetWidget()->AddWidget(1);
  this->ParameterMapRegionButtonSet->GetWidget()->GetWidget(1)->SetText("by index");
  this->ParameterMapRegionButtonSet->GetWidget()->GetWidget(0)->SelectedStateOn();
  
  app->Script("pack %s -side top -anchor w -fill x -padx 2 -pady 2", 
              this->ParameterMapRegionButtonSet->GetWidgetName());


  // -----------------------------------------
  // Region by mask
  vtkKWFrameWithLabel *rmframe = vtkKWFrameWithLabel::New();
  rmframe->SetParent(conBrowsFrame->GetFrame());
  rmframe->Create();
  rmframe->SetLabelText ("Region by mask");
  this->Script ("pack %s -side top -fill x -expand y -anchor w -padx 2 -pady 2",
                rmframe->GetWidgetName() );
  
  this->MapRegionMaskSelectorWidget = vtkSlicerNodeSelectorWidget::New();
  this->MapRegionMaskSelectorWidget->SetParent(rmframe->GetFrame());
  this->MapRegionMaskSelectorWidget->Create();
  this->MapRegionMaskSelectorWidget->SetNodeClass("vtkMRMLScalarVolumeNode", NULL, NULL, NULL);
  this->MapRegionMaskSelectorWidget->SetNewNodeEnabled(0);
  this->MapRegionMaskSelectorWidget->SetMRMLScene(this->GetMRMLScene());
  this->MapRegionMaskSelectorWidget->SetBorderWidth(2);
  this->MapRegionMaskSelectorWidget->GetWidget()->GetWidget()->IndicatorVisibilityOff();
  this->MapRegionMaskSelectorWidget->GetWidget()->GetWidget()->SetWidth(24);
  this->MapRegionMaskSelectorWidget->SetLabelText( "Mask: ");
  this->MapRegionMaskSelectorWidget->SetBalloonHelpString("Select a lmask to specify regions of interest.");

  this->MapRegionMaskLabelEntry = vtkKWEntryWithLabel::New();
  this->MapRegionMaskLabelEntry->SetParent(rmframe->GetFrame());
  this->MapRegionMaskLabelEntry->Create();
  this->MapRegionMaskLabelEntry->SetLabelText("Label: ");
  this->MapRegionMaskLabelEntry->GetWidget()->SetRestrictValueToInteger();
  this->MapRegionMaskLabelEntry->GetWidget()->SetValueAsInt(1);

  this->Script("pack %s %s -side top -fill x -expand y -anchor w -padx 2 -pady 2", 
               this->MapRegionMaskSelectorWidget->GetWidgetName(),
               this->MapRegionMaskLabelEntry->GetWidgetName());
  
  // -----------------------------------------
  // Region by Index

  vtkKWFrameWithLabel *dframe = vtkKWFrameWithLabel::New();
  dframe->SetParent(conBrowsFrame->GetFrame());
  dframe->Create();
  dframe->SetLabelText ("Region by indices");
  this->Script ("pack %s -side top -fill x -expand y -anchor w -padx 2 -pady 2",
                dframe->GetWidgetName() );

  // i range
  vtkKWFrame *iframe = vtkKWFrame::New();
  iframe->SetParent(dframe->GetFrame());
  iframe->Create();
  this->Script ("pack %s -side top -fill x -expand y -anchor w -padx 2 -pady 2",
                iframe->GetWidgetName() );
  
  vtkKWLabel *ilabel1 = vtkKWLabel::New();
  ilabel1->SetParent(iframe);
  ilabel1->Create();
  ilabel1->SetText("i range: ");

  vtkKWLabel *ilabel2 = vtkKWLabel::New();
  ilabel2->SetParent(iframe);
  ilabel2->Create();
  ilabel2->SetText("min:");

  this->MapIMinSpinBox = vtkKWSpinBox::New();
  this->MapIMinSpinBox->SetParent(iframe);
  this->MapIMinSpinBox->Create();
  this->MapIMinSpinBox->SetWidth(3);
  this->MapIMinSpinBox->SetRange(0, 0);

  vtkKWLabel *ilabel3 = vtkKWLabel::New();
  ilabel3->SetParent(iframe);
  ilabel3->Create();
  ilabel3->SetText("max:");

  this->MapIMaxSpinBox = vtkKWSpinBox::New();
  this->MapIMaxSpinBox->SetParent(iframe);
  this->MapIMaxSpinBox->Create();
  this->MapIMaxSpinBox->SetWidth(3);
  this->MapIMaxSpinBox->SetRange(0, 0);

  this->Script("pack %s %s %s %s %s -side left -fill x -expand y -anchor w -padx 2 -pady 2",
               ilabel1->GetWidgetName(),
               ilabel2->GetWidgetName(),
               this->MapIMinSpinBox->GetWidgetName(),
               ilabel3->GetWidgetName(),
               this->MapIMaxSpinBox->GetWidgetName());


  // j range

  vtkKWFrame *jframe = vtkKWFrame::New();
  jframe->SetParent(dframe->GetFrame());
  jframe->Create();
  this->Script ("pack %s -side top -fill x -expand y -anchor w -padx 2 -pady 2",
                jframe->GetWidgetName() );
  
  vtkKWLabel *jlabel1 = vtkKWLabel::New();
  jlabel1->SetParent(jframe);
  jlabel1->Create();
  jlabel1->SetText("j range: ");

  vtkKWLabel *jlabel2 = vtkKWLabel::New();
  jlabel2->SetParent(jframe);
  jlabel2->Create();
  jlabel2->SetText("min:");

  this->MapJMinSpinBox = vtkKWSpinBox::New();
  this->MapJMinSpinBox->SetParent(jframe);
  this->MapJMinSpinBox->Create();
  this->MapJMinSpinBox->SetWidth(3);
  this->MapJMinSpinBox->SetRange(0, 0);

  vtkKWLabel *jlabel3 = vtkKWLabel::New();
  jlabel3->SetParent(jframe);
  jlabel3->Create();
  jlabel3->SetText("max:");

  this->MapJMaxSpinBox = vtkKWSpinBox::New();
  this->MapJMaxSpinBox->SetParent(jframe);
  this->MapJMaxSpinBox->Create();
  this->MapJMaxSpinBox->SetWidth(3);
  this->MapJMaxSpinBox->SetRange(0, 0);
  
  this->Script("pack %s %s %s %s %s -side left -fill x -expand y -anchor w -padx 2 -pady 2",
               jlabel1->GetWidgetName(),
               jlabel2->GetWidgetName(),
               this->MapJMinSpinBox->GetWidgetName(),
               jlabel3->GetWidgetName(),
               this->MapJMaxSpinBox->GetWidgetName());

  // k range

  vtkKWFrame *kframe = vtkKWFrame::New();
  kframe->SetParent(dframe->GetFrame());
  kframe->Create();
  this->Script ("pack %s -side top -fill x -expand y -anchor w -padx 2 -pady 2",
                kframe->GetWidgetName() );
  
  vtkKWLabel *klabel1 = vtkKWLabel::New();
  klabel1->SetParent(kframe);
  klabel1->Create();
  klabel1->SetText("k range: ");

  vtkKWLabel *klabel2 = vtkKWLabel::New();
  klabel2->SetParent(kframe);
  klabel2->Create();
  klabel2->SetText("min:");

  this->MapKMinSpinBox = vtkKWSpinBox::New();
  this->MapKMinSpinBox->SetParent(kframe);
  this->MapKMinSpinBox->Create();
  this->MapKMinSpinBox->SetWidth(3);
  this->MapKMinSpinBox->SetRange(0, 0);

  vtkKWLabel *klabel3 = vtkKWLabel::New();
  klabel3->SetParent(kframe);
  klabel3->Create();
  klabel3->SetText("max:");

  this->MapKMaxSpinBox = vtkKWSpinBox::New();
  this->MapKMaxSpinBox->SetParent(kframe);
  this->MapKMaxSpinBox->Create();
  this->MapKMaxSpinBox->SetWidth(3);
  this->MapKMaxSpinBox->SetRange(0, 0);

  this->MapIMinSpinBox->SetState(0);
  this->MapIMaxSpinBox->SetState(0);
  this->MapJMinSpinBox->SetState(0);
  this->MapJMaxSpinBox->SetState(0);
  this->MapKMinSpinBox->SetState(0);
  this->MapKMaxSpinBox->SetState(0);
  
  this->Script("pack %s %s %s %s %s -side left -fill x -expand y -anchor w -padx 2 -pady 2",
               klabel1->GetWidgetName(),
               klabel2->GetWidgetName(),
               this->MapKMinSpinBox->GetWidgetName(),
               klabel3->GetWidgetName(),
               this->MapKMaxSpinBox->GetWidgetName());

  vtkKWFrame *outputFrame = vtkKWFrame::New();
  outputFrame->SetParent(conBrowsFrame->GetFrame());
  outputFrame->Create();
  this->Script ("pack %s -side top -fill x -expand y -anchor w -padx 2 -pady 2",
                outputFrame->GetWidgetName());

  this->MapOutputVolumePrefixEntry = vtkKWEntryWithLabel::New();
  this->MapOutputVolumePrefixEntry->SetParent(outputFrame);
  this->MapOutputVolumePrefixEntry->Create();
  this->MapOutputVolumePrefixEntry->SetLabelText("Output prefix:");
  this->MapOutputVolumePrefixEntry->GetWidget()->SetValue("ParameterMap");

  this->GenerateMapButton = vtkKWPushButton::New();
  this->GenerateMapButton->SetParent(outputFrame);
  this->GenerateMapButton->Create();
  this->GenerateMapButton->SetText ("Run");
  this->GenerateMapButton->SetWidth (10);
  
  this->Script("pack %s %s -side left -fill x -expand y -anchor w -padx 2 -pady 2", 
               this->MapOutputVolumePrefixEntry->GetWidgetName(),
               this->GenerateMapButton->GetWidgetName());

  rmframe->Delete();
  dframe->Delete();
  iframe->Delete();
  ilabel1->Delete();
  ilabel2->Delete();
  ilabel3->Delete();
  jframe->Delete();
  jlabel1->Delete();
  jlabel2->Delete();
  jlabel3->Delete();
  kframe->Delete();
  klabel1->Delete();
  klabel2->Delete();
  klabel3->Delete();
  outputFrame->Delete();
  conBrowsFrame->Delete();

}


//----------------------------------------------------------------------------
void vtkPharmacokineticsGUI::UpdateAll()
{
  
}


//----------------------------------------------------------------------------
void vtkPharmacokineticsGUI::SelectActive4DBundle(vtkMRMLTimeSeriesBundleNode* bundleNode)
{
  if (bundleNode == NULL)
    {
    return;
    }

  int volume = (int)this->ForegroundVolumeSelectorScale->GetValue();

  // frame control
  int n = bundleNode->GetNumberOfFrames();
  this->ForegroundVolumeSelectorScale->SetRange(0.0, (double) n-1);
  this->BackgroundVolumeSelectorScale->SetRange(0.0, (double) n-1);
  bundleNode->SwitchDisplayBuffer(0, volume);
  bundleNode->SwitchDisplayBuffer(1, volume);
  
  // plot
  this->CurveFittingStartIndexSpinBox->SetRange(0, n-1);
  this->CurveFittingEndIndexSpinBox->SetRange(0, n-1);
  this->CurveFittingStartIndexSpinBox->SetValue(0);
  this->CurveFittingStartIndexSpinBox->Modified();
  this->CurveFittingEndIndexSpinBox->SetValue(n-1);
  this->CurveFittingEndIndexSpinBox->Modified();
  
  // set spin box ranges for 3D map generation and cropping
  vtkMRMLScalarVolumeNode* volumeNode = vtkMRMLScalarVolumeNode::SafeDownCast(bundleNode->GetFrameNode(0));
  if (volumeNode)
    {
    vtkImageData* imageData = volumeNode->GetImageData();
    int* dimensions = imageData->GetDimensions();
    
    // Map
    this->MapIMinSpinBox->SetRange(0, dimensions[0]-1);
    this->MapIMinSpinBox->SetValue(0);
    this->MapIMaxSpinBox->SetRange(0, dimensions[0]-1);
    this->MapIMaxSpinBox->SetValue(dimensions[0]-1);
    this->MapJMinSpinBox->SetRange(0, dimensions[1]-1);
    this->MapJMinSpinBox->SetValue(0);
    this->MapJMaxSpinBox->SetRange(0, dimensions[1]-1);
    this->MapJMaxSpinBox->SetValue(dimensions[1]-1);
    this->MapKMinSpinBox->SetRange(0, dimensions[2]-1);
    this->MapKMinSpinBox->SetValue(0);
    this->MapKMaxSpinBox->SetRange(0, dimensions[2]-1);
    this->MapKMaxSpinBox->SetValue(dimensions[2]-1);
    
    }

  this->ForegroundVolumeSelectorScale->SetRange(0.0, (double) n-1);
  this->BackgroundVolumeSelectorScale->SetRange(0.0, (double) n-1);
  SetForeground(bundleNode->GetID(), 0);
  SetBackground(bundleNode->GetID(), 0);
}


//----------------------------------------------------------------------------
void vtkPharmacokineticsGUI::SetForeground(const char* bundleID, int index)
{

  int i, nnodes;
  vtkMRMLSliceCompositeNode *cnode;
  //vtkSlicerApplicationGUI *appGUI = this->GetApplicationGUI();
  
  vtkMRMLTimeSeriesBundleNode* bundleNode 
    = vtkMRMLTimeSeriesBundleNode::SafeDownCast(this->GetMRMLScene()->GetNodeByID(bundleID));

  if (!bundleNode)
    {
    return;
    }

  vtkMRMLVolumeNode* volNode;
  //volNode = vtkMRMLVolumeNode::SafeDownCast(bundleNode->GetFrameNode(index));
  bundleNode->SwitchDisplayBuffer(0, index);
  volNode = vtkMRMLVolumeNode::SafeDownCast(bundleNode->GetDisplayBufferNode(0));

  if (volNode)
    {
    //std::cerr << "volume node name  = " <<  volNode->GetName() << std::endl;
    nnodes = this->GetMRMLScene()->GetNumberOfNodesByClass ( "vtkMRMLSliceCompositeNode");
    for ( i=0; i<nnodes; i++)
      {
      cnode = vtkMRMLSliceCompositeNode::SafeDownCast (
        this->GetMRMLScene()->GetNthNodeByClass (i, "vtkMRMLSliceCompositeNode"));
      if ( cnode != NULL)
        {
        cnode->SetForegroundVolumeID(volNode->GetID());
        }
      }
    //SetWindowLevelForCurrentFrame();
    }
}


//----------------------------------------------------------------------------
void vtkPharmacokineticsGUI::SetBackground(const char* bundleID, int index)
{
  int i, nnodes;
  vtkMRMLSliceCompositeNode *cnode;
  //vtkSlicerApplicationGUI *appGUI = this->GetApplicationGUI();
  
  vtkMRMLTimeSeriesBundleNode* bundleNode 
    = vtkMRMLTimeSeriesBundleNode::SafeDownCast(this->GetMRMLScene()->GetNodeByID(bundleID));

  if (!bundleNode)
    {
    return;
    }

  vtkMRMLVolumeNode* volNode;
  //volNode = vtkMRMLVolumeNode::SafeDownCast(bundleNode->GetFrameNode(index));
  bundleNode->SwitchDisplayBuffer(1, index);
  volNode = vtkMRMLVolumeNode::SafeDownCast(bundleNode->GetDisplayBufferNode(1));

  if (volNode)
    {
    //std::cerr << "node id = " << nodeID << std::endl;
    nnodes = this->GetMRMLScene()->GetNumberOfNodesByClass ( "vtkMRMLSliceCompositeNode");          
    for ( i=0; i<nnodes; i++)
      {
      cnode = vtkMRMLSliceCompositeNode::SafeDownCast (
        this->GetMRMLScene()->GetNthNodeByClass (i, "vtkMRMLSliceCompositeNode"));
      if ( cnode != NULL)
        {
        cnode->SetBackgroundVolumeID(volNode->GetID());
        }
      }
    //SetWindowLevelForCurrentFrame();
    }
}


//----------------------------------------------------------------------------
void vtkPharmacokineticsGUI::SetWindowLevelForCurrentFrame()
{

  vtkMRMLSliceCompositeNode *cnode = 
    vtkMRMLSliceCompositeNode::SafeDownCast (this->GetMRMLScene()->GetNthNodeByClass (0, "vtkMRMLSliceCompositeNode"));
  if (cnode != NULL)
    {
    const char* fgNodeID = cnode->GetForegroundVolumeID();
    const char* bgNodeID = cnode->GetBackgroundVolumeID();
    if (fgNodeID)
      {
      vtkMRMLVolumeNode* fgNode =
        vtkMRMLVolumeNode::SafeDownCast(this->GetMRMLScene()->GetNodeByID(fgNodeID));
      vtkMRMLScalarVolumeDisplayNode* displayNode = vtkMRMLScalarVolumeDisplayNode::SafeDownCast(fgNode->GetDisplayNode());
      if (displayNode)
        {
        //double r[2];
        //fgNode->GetImageData()->GetScalarRange(r);
        double lower = this->RangeLower;
        double upper = this->RangeUpper;
        double range = upper - lower;
        double thLower = lower + range * this->ThresholdLower;
        double thUpper = lower + range * this->ThresholdUpper;
        double window  = range * this->Window;
        double level   = lower + range * this->Level;
        displayNode->SetAutoWindowLevel(0);
        displayNode->SetAutoThreshold(0);
        double cThLower = displayNode->GetLowerThreshold();
        double cThUpper = displayNode->GetUpperThreshold();
        double cWindow  = displayNode->GetWindow();
        double cLevel   = displayNode->GetLevel();

        int m = 0;
        if (cThLower!=thLower) { displayNode->SetLowerThreshold(thLower); m = 1;}
        if (cThUpper!=thUpper) { displayNode->SetUpperThreshold(thUpper); m = 1;}
        if (cWindow!=window)   { displayNode->SetWindow(window); m = 1;}
        if (cLevel!=level)     { displayNode->SetLevel(level); m = 1;}
        if (m) { displayNode->UpdateImageDataPipeline(); }
        }
      }
    if (bgNodeID && strcmp(fgNodeID, bgNodeID) != 0)
      {
      vtkMRMLVolumeNode* bgNode =
        vtkMRMLVolumeNode::SafeDownCast(this->GetMRMLScene()->GetNodeByID(bgNodeID));
      vtkMRMLScalarVolumeDisplayNode* displayNode = vtkMRMLScalarVolumeDisplayNode::SafeDownCast(bgNode->GetDisplayNode());
      if (displayNode)
        {
        //double r[2];
        //bgNode->GetImageData()->GetScalarRange(r);
        double lower = this->RangeLower;
        double upper = this->RangeUpper;
        double range = upper - lower;
        double thLower = lower + range * this->ThresholdLower;
        double thUpper = lower + range * this->ThresholdUpper;
        double window  = range * this->Window;
        double level   = lower + range * this->Level;
        displayNode->SetAutoWindowLevel(0);
        displayNode->SetAutoThreshold(0);
        double cThLower = displayNode->GetLowerThreshold();
        double cThUpper = displayNode->GetUpperThreshold();
        double cWindow  = displayNode->GetWindow();
        double cLevel   = displayNode->GetLevel();

        int m = 0;
        if (cThLower!=thLower) { displayNode->SetLowerThreshold(thLower); m = 1;}
        if (cThUpper!=thUpper) { displayNode->SetUpperThreshold(thUpper); m = 1;}
        if (cWindow!=window)   { displayNode->SetWindow(window); m = 1;}
        if (cLevel!=level)     { displayNode->SetLevel(level); m = 1;}
        if (m) { displayNode->UpdateImageDataPipeline(); }
        }
      }
    }
}


//----------------------------------------------------------------------------
void vtkPharmacokineticsGUI::UpdatePlotList()
{
  std::cerr << "void vtkPharmacokineticsGUI::UpdatePlotList() begin" << std::endl;

  if (this->PlotList && this->PlotManagerNode && this->GetMRMLScene())
    {
    std::cerr << "void vtkPharmacokineticsGUI::UpdatePlotList()" << std::endl;

    //---
    //--- Obtain a list of plot nodes
    //---
    std::vector<vtkMRMLNode*> nodes;
    this->GetMRMLScene()->GetNodesByClass("vtkMRMLArrayPlotNode", nodes);

    //---
    //--- Save selected nodes before delete all rows
    //---
    if ( this->PlotList && this->PlotList->IsCreated() && this->PlotList->GetWidget() )
      {
      int nRow = this->PlotList->GetWidget()->GetNumberOfRows();
      std::map<std::string, int> selected;
      for (int i = 0; i < nRow; i ++)
        {
        int v = this->PlotList->GetWidget()->GetCellTextAsInt(i, COLUMN_SELECT);
        const char* nodeID = this->PlotList->GetWidget()->GetCellText(i, COLUMN_MRML_ID);
        if (v)
          {
          selected[nodeID] = 1;
          }
        else
          {
          selected[nodeID] = 0;
          }
        }

      this->PlotList->GetWidget()->DeleteAllRows();
      this->PlotList->GetWidget()->AddRows(nodes.size());
    
      for (unsigned int i = 0; i < nodes.size(); i ++)
        {
        vtkMRMLPlotNode* node = vtkMRMLPlotNode::SafeDownCast(nodes[i]);
        if (node)
          {
          double r, g, b;
          node->GetColor(r, g, b);

          // Selection
          std::map<std::string, int>::iterator iter;
          iter = selected.find(node->GetID());
          if (iter != selected.end() && iter->second == 1)
            {
            this->PlotList->GetWidget()->SetCellTextAsInt(i, COLUMN_SELECT, 1);
            this->PlotList->GetWidget()->SetCellWindowCommandToCheckButton(i, COLUMN_SELECT);
            }
          else
            {
            this->PlotList->GetWidget()->SetCellTextAsInt(i, COLUMN_SELECT, 0);
            this->PlotList->GetWidget()->SetCellWindowCommandToCheckButton(i, COLUMN_SELECT);
            }

          // Visibility
          int v = node->GetVisible();
          if (v > 0)
            {
            this->PlotList->GetWidget()->SetCellTextAsInt(i, COLUMN_VISIBLE, 1);
            this->PlotList->GetWidget()->SetCellWindowCommandToCheckButton(i, COLUMN_VISIBLE);
            }
          else
            {
            this->PlotList->GetWidget()->SetCellTextAsInt(i, COLUMN_VISIBLE, 0);
            this->PlotList->GetWidget()->SetCellWindowCommandToCheckButton(i, COLUMN_VISIBLE);
            }

          // Color panel
          this->PlotList->GetWidget()->SetCellSelectionBackgroundColor(i, COLUMN_COLOR, r, g, b);
          this->PlotList->GetWidget()->SetCellBackgroundColor(i, COLUMN_COLOR, r, g, b);

          // Curve name entry
          this->PlotList->GetWidget()->SetCellText(i, COLUMN_NODE_NAME, node->GetName());

          // MRML node ID
          this->PlotList->GetWidget()->SetCellText(i, COLUMN_MRML_ID, node->GetID());
          }
        }
      }
    }
}


//----------------------------------------------------------------------------
void vtkPharmacokineticsGUI::UpdatePlotListElement(int row, int col, char * str)
{
  if (this->PlotManagerNode &&
      this->GetMRMLScene() &&
      (row >= 0) && (row < this->PlotList->GetWidget()->GetNumberOfRows()) &&
      (col >= 0) && (col < this->PlotList->GetWidget()->GetNumberOfColumns()))
    {
    const char* nodeID = this->PlotList->GetWidget()->GetCellText(row, COLUMN_MRML_ID);
    vtkMRMLPlotNode* pnode = vtkMRMLPlotNode::SafeDownCast(this->GetMRMLScene()->GetNodeByID(nodeID));
    if (pnode)
      {
      if (col == COLUMN_VISIBLE)
        {
        int v = this->PlotList->GetWidget()->GetCellTextAsInt(row, col);
        if (v == 0)
          {
          pnode->SetVisible(0);
          }
        else
          {
          pnode->SetVisible(1);
          }
        this->PlotManagerNode->Refresh();
        }
      else if (col == COLUMN_NODE_NAME)
        {
        const char* name = this->PlotList->GetWidget()->GetCellText(row, col);
        pnode->SetName(name);
        UpdateFittingTargetMenu();
        }
      }
    }
}

//----------------------------------------------------------------------------
void vtkPharmacokineticsGUI::DeleteSelectedPlots()
{
  if (this->PlotManagerNode && this->GetMRMLScene())
    {
    int numColumn = this->PlotList->GetWidget()->GetNumberOfRows();
    std::vector< std::string > list;
    list.clear();
    // Check the plot list
    for (int row = 0; row < numColumn; row ++)
      {
      int v = this->PlotList->GetWidget()->GetCellTextAsInt(row, COLUMN_SELECT);
      if (v)
        {
        list.push_back(this->PlotList->GetWidget()->GetCellText(row, COLUMN_MRML_ID));
        }
      }

    //---
    //--- Is at least one plot is selected for deletion?
    //---
    if (list.size() > 0) 
      {
      int answer = this->PostQuestion ( "Are you sure you want to delete the selected plots?");
        if (answer)
        {
        // go ahead and delete nodes
        std::vector< std::string >::iterator iter;
        for (iter = list.begin(); iter != list.end(); iter ++)
          {
          vtkMRMLArrayPlotNode* node = vtkMRMLArrayPlotNode::SafeDownCast(this->GetMRMLScene()->GetNodeByID(iter->c_str()));
          if (node)
            {
            this->PlotManagerNode->RemovePlotNodeByNodeID(iter->c_str());
            vtkMRMLDoubleArrayNode* anode = node->GetArray();
            this->GetMRMLScene()->RemoveNode(anode);
            this->GetMRMLScene()->RemoveNode(node);
            
            this->PlotManagerNode->SetAutoXRange(1);
            this->PlotManagerNode->SetAutoYRange(1);
            this->PlotManagerNode->Refresh();
            }
          }
        UpdatePlotList();
        }
      }
    }
}


//----------------------------------------------------------------------------
void vtkPharmacokineticsGUI::SelectAllPlots()
{
  if (this->PlotManagerNode)
    {
    int numColumn = this->PlotList->GetWidget()->GetNumberOfRows();
    for (int row = 0; row < numColumn; row ++)
      {
      this->PlotList->GetWidget()->SetCellTextAsInt(row, COLUMN_SELECT, 1);
      this->PlotList->GetWidget()->SetCellWindowCommandToCheckButton(row, COLUMN_SELECT);
      }
    }
}

//----------------------------------------------------------------------------
void vtkPharmacokineticsGUI::DeselectAllPlots()
{
  if (this->PlotManagerNode)
    {
    int numColumn = this->PlotList->GetWidget()->GetNumberOfRows();
    for (int row = 0; row < numColumn; row ++)
      {
      this->PlotList->GetWidget()->SetCellTextAsInt(row, COLUMN_SELECT, 0);
      this->PlotList->GetWidget()->SetCellWindowCommandToCheckButton(row, COLUMN_SELECT);
      }
    }
}




//----------------------------------------------------------------------------
void vtkPharmacokineticsGUI::UpdateInitialParameterList( )
{
  if (this->InitialParameterList == NULL)
    {
    return;
    }

  //---
  //--- check to see if node is null . if so, create and set up.
  //---
  vtkMRMLPharmacokineticsCurveAnalysisNode *curveNode = this->GetCurveAnalysisNode();
  if ( curveNode == NULL )
    {
    curveNode = vtkMRMLPharmacokineticsCurveAnalysisNode::New();
    vtkIntArray *events = vtkIntArray::New();
    events->InsertNextValue ( vtkMRMLPharmacokineticsCurveAnalysisNode::ErrorEvent );
    events->InsertNextValue ( vtkMRMLPharmacokineticsCurveAnalysisNode::PlotReadyEvent );
    events->InsertNextValue ( vtkMRMLPharmacokineticsCurveAnalysisNode::ModelChangedEvent );
    vtkSetAndObserveMRMLNodeEventsMacro ( this->CurveAnalysisNode, curveNode, events );
    curveNode->Delete();
    if ( this->Logic->GetCurveAnalysisNode() != NULL)
      {
      this->Logic->SetCurveAnalysisNode ( NULL );
      }
    this->Logic->SetCurveAnalysisNode ( this->GetCurveAnalysisNode() );
    events->Delete();
    }
  
  //---
  //--- propagate the node's model to the GUI if changes are new.
  //---
  if ( this->GetCurveAnalysisNode() == NULL)
    {
    vtkErrorMacro ( "Got NULL Curve Analysis Node." );
    return;
    }

  //---
  //--- Update curve fitting model and initial parameters!
  //---
  const char *model = this->GetCurveAnalysisNode()->GetMethodName();
  if ( model && *model )
    {

    if (this->ModelSelectButton->GetMenu()->GetIndexOfItem ( model ) >= 0)
      {
      if ( strcmp(this->ModelSelectButton->GetValue(), model ))
        {
        this->ModelSelectButton->GetMenu()->SelectItem ( model );
        }
      }
    }
  else
    {
    //--- set the default if we need to.
    if (this->ModelSelectButton->GetMenu()->GetIndexOfItem ( "Tofts Kinetic Model" ) >= 0)
      {
      if ( strcmp(this->ModelSelectButton->GetValue(), "Tofts Kinetic Model" ))
        {
        this->ModelSelectButton->GetMenu()->SelectItem ( "Tofts Kinetic Model" );
        }
      }
    else
      {
      vtkErrorMacro ( "Requested model not available." );
      }
    }

  //--- WJP You are here! Make all update methods go thru node!!!

  // Adjust number of rows
  //int numRows = this->InitialParameterList->GetWidget()->GetNumberOfRows();

  vtkStringArray* paramNames      = this->CurveAnalysisNode->GetInitialParameterNameArray();
  vtkStringArray* inputParamNames = this->CurveAnalysisNode->GetConstantNameArray();
  vtkStringArray* inputDataNames  = this->CurveAnalysisNode->GetInputArrayNameArray();
  int numParameters      = paramNames->GetNumberOfTuples();
  int numInputParameters = inputParamNames->GetNumberOfTuples();
  int numInputCurves     = inputDataNames->GetNumberOfTuples();

  this->InitialParameterList->GetWidget()->DeleteAllRows();
  this->InitialParameterList->GetWidget()->AddRows(numParameters + numInputParameters + numInputCurves);

  this->InitialParameterListInputType.clear();
  this->InitialParameterListNodeNames.clear();
  this->InitialParameterListInputType.resize(numParameters + numInputParameters + numInputCurves);
  this->InitialParameterListNodeNames.resize(numParameters + numInputParameters + numInputCurves);

  char label[256];

  for (int i = 0; i < numParameters; i ++)
    {
    const char* name = paramNames->GetValue(i);
    double value     = this->CurveAnalysisNode->GetInitialParameter(name);
    sprintf(label, "[P] %s", name);
    this->InitialParameterList->GetWidget()->SetCellText(i, 0, label);
    this->InitialParameterList->GetWidget()->SetCellEditWindowToEntry(i, 1);
    this->InitialParameterList->GetWidget()->SetCellTextAsDouble(i, 1, value);
    this->InitialParameterListInputType[i] = INPUT_VALUE_INITIALPARAM;
    this->InitialParameterListNodeNames[i] = "";
    }

  for (int i = 0; i < numInputParameters; i ++)
    {
    int row = i + numParameters;
    const char* name = inputParamNames->GetValue(i);
    double value     = this->CurveAnalysisNode->GetConstant(name);
    sprintf(label, "[I] %s", name);
    this->InitialParameterList->GetWidget()->SetCellText(row, 0, label);
    this->InitialParameterList->GetWidget()->SetCellEditWindowToEntry(row, 1);
    this->InitialParameterList->GetWidget()->SetCellTextAsDouble(row, 1, value);
    this->InitialParameterListInputType[row] = INPUT_VALUE_CONSTANT;
    this->InitialParameterListNodeNames[row] = "";
    }

  for (int i = 0; i < numInputCurves; i ++)
    {
    int row = i + numParameters + numInputParameters;
    const char* name = inputDataNames->GetValue(i);
    sprintf(label, "[C] %s", name);
    this->InitialParameterList->GetWidget()->SetCellText(row, 0, label);
    this->InitialParameterList->GetWidget()->SetCellEditWindowToEntry(row, 1);
    //this->InitialParameterList->GetWidget()->SetCellEditWindowToSpinBox(row, 1);
    this->InitialParameterList->GetWidget()->SetCellText(row, 1, "(Click to select)");
    this->InitialParameterListInputType[row] = INPUT_PLOTNODE;
    this->InitialParameterListNodeNames[row] = "";
    }

}

//----------------------------------------------------------------------------
void vtkPharmacokineticsGUI::GetInitialParametersAndInputCurves( int start, int end)
{
  //--- check the multicolumn list
  if (this->InitialParameterList == NULL)
    {
    return;
    }

  vtkStringArray* paramNames      = this->CurveAnalysisNode->GetInitialParameterNameArray();
  //---
  //--- get list of constant names and input array names.
  //---
  vtkStringArray* inputParamNames = this->CurveAnalysisNode->GetConstantNameArray();
  vtkStringArray* inputDataNames  = this->CurveAnalysisNode->GetInputArrayNameArray();
  //---
  //--- get numbers of parameters and curves.
  //---
  int numParameters      = paramNames->GetNumberOfTuples();
  int numInputParameters = inputParamNames->GetNumberOfTuples();
  int numInputCurves     = inputDataNames->GetNumberOfTuples();

  //---
  //--- Set Initial Parameters on the Node from GUI
  //---
  for (int i = 0; i < numParameters; i ++)
    {
    const char* name = paramNames->GetValue(i);
    double value = this->InitialParameterList->GetWidget()->GetCellTextAsDouble(i, 1);
    this->CurveAnalysisNode->SetInitialParameter(name, value);
    std::cerr << name << " = " << value << std::endl;
    }

  for (int i = 0; i < numInputParameters; i ++)
    {
    int row = i + numParameters;
    const char* name = inputParamNames->GetValue(i);
    double value = this->InitialParameterList->GetWidget()->GetCellTextAsDouble(row, 1);
    this->CurveAnalysisNode->SetConstant(name, value);
    std::cerr << name << " = " << value << std::endl;
    }

  for (int i = 0; i < numInputCurves; i ++)
    {
    int row = i + numParameters + numInputParameters;
    const char* name = inputDataNames->GetValue(i);
    //int label = this->InitialParameterList->GetWidget()->GetCellTextAsInt(row, 1);
    const char* nodeID = this->InitialParameterListNodeNames[row].c_str();
    
    std::cerr << "node id = " << nodeID << std::endl;;

    //vtkDoubleArray* curve = this->IntensityCurves->GetCurve(label);
    //vtkMRMLDoubleArrayNode* anode = this->IntensityCurves->GetCurve(label);
    vtkMRMLArrayPlotNode* pnode = vtkMRMLArrayPlotNode::SafeDownCast(this->GetMRMLScene()->GetNodeByID(nodeID));
    if (pnode && pnode->GetArray())
      {
      vtkDoubleArray* curve = pnode->GetArray()->GetArray();
      if (curve)
        {
        vtkDoubleArray* inputCurve = vtkDoubleArray::New();
        inputCurve->SetNumberOfComponents( curve->GetNumberOfComponents() );
        int max   = curve->GetNumberOfTuples();
        
        if (start < 0)   start = 0;
        if (end >= max)  end   = max-1;
        if (start > end) start = end;
        for (int i = start; i <= end; i ++)
          {
          double* xy = curve->GetTuple(i);
          inputCurve->InsertNextTuple(xy);
          }
        this->CurveAnalysisNode->SetInputArray(name, inputCurve);
        }
      }
    }

}



//----------------------------------------------------------------------------
void vtkPharmacokineticsGUI::OnInitialParameterListSelected()
{
  int row[1];
  int col[1];

  if (this->InitialParameterList->GetWidget()->GetNumberOfSelectedCells() > 0)
    {
    this->InitialParameterList->GetWidget()->GetSelectedCells(row, col);

    // Check the row index if it's a input curve selection cell.
    if (col[0] == 1 &&
        row[0] < this->InitialParameterListInputType.size() &&
        this->InitialParameterListInputType[row[0]] == INPUT_PLOTNODE)
      {
      int x, y;
      // Get current position of the mouse pointer
      std::stringstream ss;
      ss << "ProcPlotSelectPopUpMenu " << row[0] << " " << col[0];
      vtkKWTkUtilities::GetMousePointerCoordinates(this->GetApplicationGUI()->GetMainSlicerWindow(), &x, &y);
      UpdatePlotSelectPopUpMenu(ss.str().c_str());
      this->PlotSelectPopUpMenu->PopUp(x, y);
      }
    }
}

//----------------------------------------------------------------------------
void vtkPharmacokineticsGUI::ProcPlotSelectPopUpMenu(int row, int col, const char* nodeID)
{
  if (col == 1 && this->InitialParameterListInputType[row] == INPUT_PLOTNODE && this->GetMRMLScene())
    {
    vtkMRMLNode* node = this->GetMRMLScene()->GetNodeByID(nodeID);
    if (node)
      {
      this->InitialParameterList->GetWidget()->SetCellText(row, col, node->GetName());
      this->InitialParameterListNodeNames[row] = node->GetID();
      }
    }
}


//----------------------------------------------------------------------------
void vtkPharmacokineticsGUI::UpdatePlotSelectPopUpMenu(const char* command)
{
  if (this->MRMLScene && this->PlotSelectPopUpMenu)
    {
    this->PlotSelectPopUpMenu->DeleteAllItems();

    vtkCollection* collection = this->PlotManagerNode->GetPlotNodes("ArrayPlot");
    int ncol = collection->GetNumberOfItems(); 
    for (int i = 0; i < ncol; i ++)
      {
      vtkMRMLArrayPlotNode* node = vtkMRMLArrayPlotNode::SafeDownCast(collection->GetItemAsObject(i));
      if (node != NULL)
        {
        std::stringstream ss;
        ss << command << " " << node->GetID();
        this->PlotSelectPopUpMenu->AddRadioButton(node->GetName(), this, ss.str().c_str());
        }
      }

    //int n = this->PlotManagerNode->GetNumberOfPlotNodes();
    //for (int i = 0; i < n; i ++)
    //  {
    //  vtkMRMLArrayPlotNode* node = 
    //    vtkMRMLArrayPlotNode::SafeDownCast(this->PlotManagerNode->GetPlotNode(i));
    //  if (node != NULL)
    //    {
    //    std::stringstream ss;
    //    ss << command << " " << node->GetID();
    //    this->PlotSelectPopUpMenu->AddRadioButton(node->GetName(), this, ss.str().c_str());
    //    }
    //  }
    
    //// The following code gets the list from MRML scene (not from PlotManagerNode)    
    //std::vector<vtkMRMLNode *> nodes;
    //this->MRMLScene->GetNodesByClass("vtkMRMLArrayPlotNode", nodes);
    //
    //std::vector<vtkMRMLNode *>::iterator iter;
    //for (iter = nodes.begin(); iter != nodes.end(); iter ++)
    //  {
    //  vtkMRMLNode* node = *iter;
    //  if (node != NULL)
    //    {
    //    std::stringstream ss;
    //    ss << command << " " << node->GetID();
    //    this->PlotSelectPopUpMenu->AddRadioButton(node->GetName(), this, ss.str().c_str());
    //    }
    //  }
    }
  
}

//----------------------------------------------------------------------------
void vtkPharmacokineticsGUI::UpdateOutputParameterList( )
{
  if (this->ResultParameterList == NULL)
    {
    return;
    }

  // Adjust number of rows
  //int numRows = this->ResultParameterList->GetWidget()->GetNumberOfRows();

  vtkStringArray* paramNames = this->CurveAnalysisNode->GetOutputValueNameArray();
  int numParameters  = paramNames->GetNumberOfTuples();

  this->ResultParameterList->GetWidget()->DeleteAllRows();
  this->ResultParameterList->GetWidget()->AddRows(numParameters);

  for (int i = 0; i < numParameters; i ++)
    {
    const char* name = paramNames->GetValue(i);
    double value     = this->CurveAnalysisNode->GetOutputValue(name);
    this->ResultParameterList->GetWidget()->SetCellText(i, 0, name);
    this->ResultParameterList->GetWidget()->SetCellTextAsDouble(i, 1, value);
    }

}


//----------------------------------------------------------------------------
int vtkPharmacokineticsGUI::GeneratePlotNodes()
{
  
  //---
  //--- return 0 for failure, 1 for success.
  //---
  if ( this->IntensityCurves->Update() == 0 )
    {
    this->PostMessage ( "Please select both an image Time-Series and a VOI mask." );
    return 0;
    }

  vtkMRMLScalarVolumeNode* node = this->IntensityCurves->GetMaskNode();
  if (node == NULL || !node->GetLabelMap())
    {
    vtkErrorMacro ( "GeneratePlotNodes: Could not find valid MaskNode or label map" );
    return 0;
    }
  vtkMRMLVolumeDisplayNode* dnode = node->GetVolumeDisplayNode();
  if (dnode == NULL)
    {
    vtkErrorMacro ( "GeneratePlotNodes: Could not find valid DisplayNode for MaskNode." );
    return 0;
    }
  vtkMRMLColorNode* cnode = dnode->GetColorNode();
  if (cnode == NULL)
    {
    vtkErrorMacro ( "GeneratePlotNodes: Could not find valid ColorNode for MaskNode's DisplayNode." );
    return 0;
    }

  //---
  //--- should have intensity curves for VOIs now.
  //---
  vtkLookupTable* lt = cnode->GetLookupTable();

  vtkIntArray* labels = this->IntensityCurves->GetLabelList();
  int n = labels->GetNumberOfTuples();

  //this->IntensityPlot->ClearPlot();

  //---
  //--- Create and populate a PlotNode with a curve for each label in VOI mask
  //---
  for (int i = 0; i < n; i ++)
    {
    int label = labels->GetValue(i);
    vtkMRMLDoubleArrayNode* anode = this->IntensityCurves->GetCurve(label);
    vtkMRMLArrayPlotNode* cnode = vtkMRMLArrayPlotNode::New();

    this->GetMRMLScene()->AddNode(cnode);
    cnode->SetAndObserveArray(anode);
    cnode->SetErrorBar(1);
    //---
    //--- Make sure we don't exceed the maximum number of timepoints.
    //---
    if ( this->PlotManagerNode->AddPlotNode(cnode) <0 )
      {
      //--- oops abort this and return.
      this->PostMessage ( "This module supports a maximum of 10000 timepoints. The selected dataset appears to have exceeded this limit." );
      cnode->SetAndObserveArray ( NULL );
      this->GetMRMLScene()->RemoveNode ( cnode );
      cnode->Delete();
      return 0;
      }

    double color[3];
    lt->GetColor(label, color);
    if (color[0] > 0.99 && color[1] > 0.99  && color[2] > 0.99)
      {
      // if the line color is white, change the color to black
      this->PostMessage ( "Note: The plot line for the white label has been drawn in black, so it displays on a white background..." );
      color[0] = 0.0;
      color[1] = 0.0;
      color[2] = 0.0;
      }
    cnode->SetColor(color[0], color[1], color[2]);
    cnode->Delete();
    }

  this->PlotManagerNode->SetAutoXRange(1);
  this->PlotManagerNode->SetAutoYRange(1);

  this->PlotManagerNode->SetXLabel("Time (s)");
  this->PlotManagerNode->Refresh();

  //// Update FittingTargetMenu
  //if (this->FittingTargetMenu)
  //  {
  //  this->FittingTargetMenuNodeList.clear();
  //  this->FittingTargetMenu->GetMenu()->DeleteAllItems();
  //  for (int i = 0; i < n; i ++)
  //    {
  //    char str[256];
  //    sprintf(str, "%d", labels->GetValue(i));
  //    this->FittingTargetMenu->GetMenu()->AddRadioButton(str);
  //    this->FittingTargetMenuNodeList.push_back()
  //    }
  //  }

  labels->Delete();
  return 1;
}


//----------------------------------------------------------------------------
void  vtkPharmacokineticsGUI::ImportPlotNode(const char* path)
{
  vtkMRMLDoubleArrayNode* anode = this->GetLogic()->LoadDoubleArrayNodeFromFile(path);
  if (anode)
    {
    this->GetMRMLScene()->AddNode(anode);
    vtkMRMLArrayPlotNode* cnode = vtkMRMLArrayPlotNode::New();
    this->GetMRMLScene()->AddNode(cnode);
    cnode->SetAndObserveArray(anode);
    cnode->SetErrorBar(1);

    double color[3];
    
    color[0] = 0.0;
    color[1] = 0.0;
    color[2] = 0.0;
    cnode->SetColor(color[0], color[1], color[2]);
    
    this->PlotManagerNode->AddPlotNode(cnode);
    this->PlotManagerNode->SetAutoXRange(1);
    this->PlotManagerNode->SetAutoYRange(1);
    this->PlotManagerNode->Refresh();

    anode->Delete();
    cnode->Delete();
    }

}


//----------------------------------------------------------------------------
void  vtkPharmacokineticsGUI::SaveMarkedPlotNode(const char* path)
{
  if (this->PlotManagerNode && this->GetMRMLScene())
    {
    int numColumn = this->PlotList->GetWidget()->GetNumberOfRows();
    std::vector< std::string > list;
    list.clear();
    // Check the plot list
    for (int row = 0; row < numColumn; row ++)
      {
      int v = this->PlotList->GetWidget()->GetCellTextAsInt(row, COLUMN_SELECT);
      if (v)
        {
        list.push_back(this->PlotList->GetWidget()->GetCellText(row, COLUMN_MRML_ID));
        }
      }

    if (list.size() > 0)
      {
      std::vector< std::string >::iterator iter;
      for (iter = list.begin(); iter != list.end(); iter ++)
        {
        vtkMRMLArrayPlotNode* node = vtkMRMLArrayPlotNode::SafeDownCast(this->GetMRMLScene()->GetNodeByID(iter->c_str()));
        if (node)
          {
          if (node->GetArray() && node->GetArray()->GetArray())
            {
            vtkDoubleArray* array = node->GetArray()->GetArray();
            std::stringstream ss;

            ss << path << node->GetName();
            this->GetLogic()->SaveCurve(array, ss.str().c_str());
            }
          }
        }
      }
    }
}



//----------------------------------------------------------------------------
void  vtkPharmacokineticsGUI::UpdateFittingTargetMenu()
{
  if (this->MRMLScene && this->PlotManagerNode)
    {
    this->FittingTargetMenuNodeList.clear();
    this->FittingTargetMenu->GetMenu()->DeleteAllItems();

    vtkCollection* collection = this->PlotManagerNode->GetPlotNodes("ArrayPlot");
    int ncol = collection->GetNumberOfItems(); 
    for (int i = 0; i < ncol; i ++)
      {
      vtkMRMLPlotNode* node = vtkMRMLPlotNode::SafeDownCast(collection->GetItemAsObject(i));
      if (node != NULL)
        {
        this->FittingTargetMenu->GetMenu()->AddRadioButton(node->GetName());
        this->FittingTargetMenuNodeList.push_back(node->GetID());

        }
      }

    //int n = this->PlotManagerNode->GetNumberOfPlotNodes();
    //vtkIntArray* nodeList = GetPlotNodeIDList();
    //for (int i = 0; i < n; i ++)
    //  {
    //  vtkMRMLArrayPlotNode* node = 
    //    vtkMRMLArrayPlotNode::SafeDownCast(this->PlotManagerNode->GetPlotNode(i));
    //  if (node != NULL)
    //    {
    //    this->FittingTargetMenu->GetMenu()->AddRadioButton(node->GetName());
    //    this->FittingTargetMenuNodeList.push_back(node->GetID());
    //    }
    //  }
    }

  //// The following code gets the list from MRML scene (not from PlotManagerNode)
  //if (this->MRMLScene && this->PlotSelectPopUpMenu)
  //  {
  //  this->FittingTargetMenuNodeList.clear();
  //  this->FittingTargetMenu->GetMenu()->DeleteAllItems();
  //  
  //  std::vector<vtkMRMLNode *> nodes;
  //  this->MRMLScene->GetNodesByClass("vtkMRMLArrayPlotNode", nodes);
  //  
  //  std::vector<vtkMRMLNode *>::iterator iter;
  //  for (iter = nodes.begin(); iter != nodes.end(); iter ++)
  //    {
  //    vtkMRMLNode* node = *iter;
  //    if (node != NULL)
  //      {
  //      this->FittingTargetMenu->GetMenu()->AddRadioButton(node->GetName());
  //      this->FittingTargetMenuNodeList.push_back(node->GetID());
  //      }
  //    }
  //  }

}


//----------------------------------------------------------------------------
int vtkPharmacokineticsGUI::PostQuestion ( const char *question )
{
  if ( question == NULL)
    {
    vtkErrorMacro (" Got NULL dialog message. Cannot post a dialog." );
    return 0;
    }

  // Ask the user to confirm
  vtkKWMessageDialog* dialog = vtkKWMessageDialog::New();
  dialog->SetMasterWindow(this->GetApplicationGUI()->GetMainSlicerWindow());
  dialog->SetParent(this->GetApplicationGUI()->GetMainSlicerWindow());
  dialog->SetTitle("Pharmacokinetics Module");
  dialog->SetStyleToOkCancel();
  dialog->Create();
  dialog->SetText(question);
  int response = dialog->Invoke();
  dialog->Delete();
  return (response);

}


//----------------------------------------------------------------------------
void vtkPharmacokineticsGUI::PostMessage ( const char *message )
{
  if ( message == NULL)
    {
    vtkErrorMacro (" Got NULL dialog message. Cannot post a dialog." );
    return;
    }

  vtkKWMessageDialog* dialog = vtkKWMessageDialog::New();
  dialog->SetMasterWindow(this->GetApplicationGUI()->GetMainSlicerWindow());
  dialog->SetParent(this->GetApplicationGUI()->GetMainSlicerWindow());
  dialog->SetTitle("Pharmacokinetics Module");
  dialog->SetStyleToOkCancel();
  dialog->Create();
  dialog->SetText(message);
  dialog->Invoke();
  dialog->Delete();
}
