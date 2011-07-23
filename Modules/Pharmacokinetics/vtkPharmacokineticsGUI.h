#ifndef __vtkPharmacokineticsGUI_h
#define __vtkPharmacokineticsGUI_h

#ifdef WIN32
#include "vtkPharmacokineticsWin32Header.h"
#endif

#include "vtkSlicerModuleGUI.h"
#include "vtkCallbackCommand.h"
#include "vtkSlicerInteractorStyle.h"
#include "vtkDoubleArray.h"

#include "vtkPharmacokineticsLogic.h"
#include "vtkSlicerNodeSelectorWidget.h"
#include "vtkSlicerXYPlotWidget.h"

#include "vtkKWMultiColumnListWithScrollbars.h"
#include "vtkKWLoadSaveButtonWithLabel.h"
#include "vtkMRMLPharmacokineticsCurveAnalysisNode.h"

#include <map>

class vtkKWScaleWithEntry;
class vtkKWPushButton;
class vtkKWPiecewiseFunctionEditor;
class vtkKWMenuButton;
class vtkKWSpinBox;
class vtkKWCanvas;
class vtkKWProgressDialog;
class vtkKWRadioButtonSet;
class vtkKWRadioButton;
class vtkKWRange;
class vtkKWEntryWithLabel;
class vtkPharmacokineticsIntensityCurves;
class vtkKWCheckButtonWithLabel;
class vtkSlicerNodeSelectorWidget;
class vtkKWMenu;
class vtkKWRadioButtonSetWithLabel;
class vtkPharmacokineticsCurveAnalysisITKInterface;

//TODO Adapt python SCIPY solvers

#ifdef Pharmacokinetics_USE_SCIPY
class vtkPharmacokineticsCurveAnalysisPythonInterface;
#endif

class VTK_Pharmacokinetics_EXPORT vtkPharmacokineticsGUI : public vtkSlicerModuleGUI
{
 public:

  vtkTypeRevisionMacro ( vtkPharmacokineticsGUI, vtkSlicerModuleGUI );

  //----------------------------------------------------------------
  // Set/Get Methods
  //----------------------------------------------------------------

  vtkGetObjectMacro ( Logic, vtkPharmacokineticsLogic );
  virtual void SetModuleLogic ( vtkPharmacokineticsLogic *logic )
  { 
    this->SetLogic ( vtkObjectPointer (&this->Logic), logic );
  }
  virtual void SetAndObserveModuleLogic ( vtkPharmacokineticsLogic *logic )
  {
    this->SetAndObserveLogic ( vtkObjectPointer (&this->Logic), logic );
  }
  virtual void SetModuleLogic( vtkSlicerLogic *logic )
  {
    this->SetModuleLogic (reinterpret_cast<vtkPharmacokineticsLogic*> (logic));
  }

  vtkGetObjectMacro ( CurveAnalysisNode, vtkMRMLPharmacokineticsCurveAnalysisNode );
  vtkSetObjectMacro ( CurveAnalysisNode, vtkMRMLPharmacokineticsCurveAnalysisNode );
  

 protected:
  //----------------------------------------------------------------
  // Constructor / Destructor (proctected/private) 
  //----------------------------------------------------------------

  vtkPharmacokineticsGUI ( );
  virtual ~vtkPharmacokineticsGUI ( );


 private:
  vtkPharmacokineticsGUI ( const vtkPharmacokineticsGUI& ); // Not implemented.
  void operator = ( const vtkPharmacokineticsGUI& ); //Not implemented.

 public:
  //----------------------------------------------------------------
  // New method, Initialization etc.
  //----------------------------------------------------------------

  static vtkPharmacokineticsGUI* New ();
  void Init();
  virtual void Enter ( );
  virtual void Exit ( );
  void PrintSelf (ostream& os, vtkIndent indent );

  // Get the categorization of the module.
  const char *GetCategory() const { return "Quantification"; }
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
  //BTX
  void BuildGUIForHelpFrame();
  void BuildGUIForActiveBundleSelectorFrame();
  void BuildGUIForFrameControlFrame(int show);
  void BuildGUIForFunctionViewer(int show);
  void BuildGUIForCurveFitting (int show);
  void BuildGUIForMethodSetting (int show );
  void BuildGUIForMapGenerator (int show );
  //ETX

  //----------------------------------------------------------------
  // Update routines
  //----------------------------------------------------------------

  void UpdateAll();
  void SelectActive4DBundle(vtkMRMLTimeSeriesBundleNode* bundleNode);

  //----------------------------------------------------------------
  // Viewer control
  //----------------------------------------------------------------

  // -----------------------------------------
  // Frame control
  void SetForeground(const char* bundleID, int index);
  void SetBackground(const char* bundleID, int index);
  void SetWindowLevelForCurrentFrame();

  // -----------------------------------------
  // Plot list
  void UpdatePlotList();
  void UpdatePlotListElement(int row, int col, char * str);
  void DeleteSelectedPlots();
  void SelectAllPlots();
  void DeselectAllPlots();

  // Description:
  // GeneratePlotNodes() calculates time-intensity curves in the regions specified by the label data.
  int GeneratePlotNodes();
  void ImportPlotNode(const char* path);
  void SaveMarkedPlotNode(const char* path);

  // -----------------------------------------
  // Initial parameter list
  void UpdateInitialParameterList( );
  void GetInitialParametersAndInputCurves(int start, int end);
  void OnInitialParameterListSelected();
  void ProcPlotSelectPopUpMenu(int row, int col, const char* nodeID);
  void UpdatePlotSelectPopUpMenu(const char* command);

  // -----------------------------------------
  // Output parameter list
  void UpdateOutputParameterList( );
  void UpdateFittingTargetMenu();

  //----------------------------------------------------------------
  // Send user a message.
  void PostMessage( const char *message);
  int PostQuestion ( const char *question );

 protected:
  //----------------------------------------------------------------
  // Timer
  //----------------------------------------------------------------
  
  int TimerFlag;
  int TimerInterval;  // ms

  //----------------------------------------------------------------
  // GUI widgets
  //----------------------------------------------------------------

  vtkKWProgressDialog*                ProgressDialog;

  // -----------------------------------------
  // Active 4D Bundle selector
  vtkSlicerNodeSelectorWidget*        Active4DBundleSelectorWidget;

  // -----------------------------------------
  // Frame Control
  vtkKWScaleWithEntry*                ForegroundVolumeSelectorScale;
  vtkKWScaleWithEntry*                BackgroundVolumeSelectorScale;
  vtkKWRange*                         WindowLevelRange;
  vtkKWRange*                         ThresholdRange;

  // -----------------------------------------
  // Intensity Plot
  vtkSlicerNodeSelectorWidget*        MaskNodeSelector;
  vtkKWPushButton*                    GenerateCurveButton;
  vtkKWRadioButtonSetWithLabel*       ValueTypeButtonSet;
  vtkSlicerXYPlotWidget*              IntensityPlot;
  vtkKWCheckButtonWithLabel*          ErrorBarCheckButton;
  vtkKWMultiColumnListWithScrollbars* PlotList;

  vtkKWPushButton*                    ImportPlotButton;
  vtkKWPushButton*                    SelectAllPlotButton;
  vtkKWPushButton*                    DeselectAllPlotButton;
  vtkKWPushButton*                    PlotDeleteButton;
  vtkKWPushButton*                    SavePlotButton;

  // -----------------------------------------
  // Model / Parameters
  vtkKWMenuButton*                    FittingTargetMenu;
  vtkKWMenuButton*                    ModelSelectButton;
  vtkKWLoadSaveButtonWithLabel*       CurveScriptSelectButton;
  vtkKWEntryWithLabel*                CurveScriptMethodName;
  vtkKWSpinBox*                       CurveFittingStartIndexSpinBox;
  vtkKWSpinBox*                       CurveFittingEndIndexSpinBox;
  vtkKWMultiColumnListWithScrollbars* InitialParameterList;
  vtkKWMenu*                          PlotSelectPopUpMenu;

  // -----------------------------------------
  // Curve Fitting
  vtkKWPushButton*                    RunFittingButton;
  vtkKWMultiColumnListWithScrollbars* ResultParameterList;

  // -----------------------------------------
  // Parameter Map
  vtkKWRadioButtonSetWithLabel*       ParameterMapRegionButtonSet;
  vtkSlicerNodeSelectorWidget*        MapRegionMaskSelectorWidget;
  vtkKWEntryWithLabel*                MapRegionMaskLabelEntry;
  vtkKWSpinBox*                       MapIMinSpinBox;
  vtkKWSpinBox*                       MapIMaxSpinBox;
  vtkKWSpinBox*                       MapJMinSpinBox;
  vtkKWSpinBox*                       MapJMaxSpinBox;
  vtkKWSpinBox*                       MapKMinSpinBox;
  vtkKWSpinBox*                       MapKMaxSpinBox;
  vtkKWPushButton*                    GenerateMapButton;
  vtkKWEntryWithLabel*                MapOutputVolumePrefixEntry;


  //----------------------------------------------------------------
  // Logic Values
  //----------------------------------------------------------------

  vtkPharmacokineticsLogic*      Logic;
  vtkCallbackCommand*         DataCallbackCommand;
  int                         CloseScene;

  // -----------------------------------------
  // Window/level adjustment  -- will be integrated in 4D Image
  double  RangeLower;
  double  RangeUpper;

  double  Window;
  double  Level;
  double  ThresholdUpper;
  double  ThresholdLower;

  // -----------------------------------------
  // Curve / plot data
  vtkPharmacokineticsIntensityCurves*              IntensityCurves;
  vtkMRMLXYPlotManagerNode*        PlotManagerNode;
  vtkMRMLPharmacokineticsCurveAnalysisNode *CurveAnalysisNode;
  
  // -----------------------------------------
  // List management
  //BTX
  // Row index for   vtkKWMultiColumnListWithScrollbars* PlotList;
  enum {
    COLUMN_SELECT    = 0,
    COLUMN_VISIBLE   = 1,
    COLUMN_COLOR     = 2,
    COLUMN_NODE_NAME = 3,
    COLUMN_MRML_ID   = 4
  };
  //ETX

  //BTX
  std::vector<int> InitialParameterListInputType;
  std::vector<std::string> InitialParameterListNodeNames;
  enum {
    INPUT_VALUE_INITIALPARAM = 0,
    INPUT_VALUE_CONSTANT,
    INPUT_PLOTNODE,
  };
  std::vector< std::string > FittingTargetMenuNodeList;
  //ETX

};

#endif
