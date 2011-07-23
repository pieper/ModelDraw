/*=auto=========================================================================

  Portions (c) Copyright 2005 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $RCSfile: vtkSlicerNodeSelectorWidget.h,v $
  Date:      $Date: 2010-02-15 16:34:06 -0500 (Mon, 15 Feb 2010) $
  Version:   $Revision: 12141 $

=========================================================================auto=*/

///  vtkSlicerNodeSelectorWidget - menu to select volumes from current mrml scene
/// 
/// Inherits most behavior from kw widget, but is specialized to observe
/// the current mrml scene and update the entries of the pop up menu to correspond
/// to the currently available volumes.  This widget also has a notion of the current selection
/// that can be observed or set externally
//


#ifndef __vtkSlicerMRMLTreeWidget_h
#define __vtkSlicerMRMLTreeWidget_h

#include "vtkSlicerWidget.h"

#include "vtkMRMLScene.h"
#include "vtkMRMLNode.h"
#include "vtkMRMLTransformNode.h"
#include "vtkMRMLTransformableNode.h"

#include <vtksys/stl/string>

class vtkKWMenu;
class vtkKWTreeWithScrollbars;
class vtkKWEntryWithLabel;
class vtkKWLabelWithLabel;
class vtkKWCheckButtonWithLabel;

class VTK_SLICER_BASE_GUI_EXPORT vtkSlicerMRMLTreeWidget : public vtkSlicerWidget
{
  
public:
  static vtkSlicerMRMLTreeWidget* New();
  vtkTypeRevisionMacro(vtkSlicerMRMLTreeWidget,vtkSlicerWidget);
  void PrintSelf(ostream& os, vtkIndent indent);
  
  /// 
  /// alternative method to propagate events generated in GUI to logic / mrml
  virtual void ProcessWidgetEvents ( vtkObject *caller, unsigned long event, void *callData );
  
  /// 
  /// alternative method to propagate events generated in GUI to logic / mrml
  virtual void ProcessMRMLEvents ( vtkObject *caller, unsigned long event, void *callData );
  
  /// 
  /// removes observers on widgets in the class
  virtual void RemoveWidgetObservers ( );
  
  /// 
  /// removes observers on node deleted events 
  virtual void AddMRMLObservers();
  virtual void RemoveMRMLObservers();

  
  /// :
  /// Callbacks
  virtual void DeleteNodeCallback(const char *id);

  virtual void CutNodeCallback(const char *id);

  virtual void PasteNodeCallback(const char *id);

  virtual void InsertTransformNodeCallback(const char *id);

  virtual void SelectNodeCallback(const char *id);
  
  virtual void NodeParentChangedCallback (const char *node, const char *new_parent, const char*);
  
  virtual void ToggleVisibilityCallback(const char *id);
  virtual void VisibilityCallback(const char *id, int vis);

  virtual void PlotCallback(const char *id);

  virtual void HardenTransformCallback(const char *id);
 
//BTX
  enum
    {
      SelectedEvent = 73400,
   };
//ETX
protected:
  vtkSlicerMRMLTreeWidget();
  virtual ~vtkSlicerMRMLTreeWidget();

  /// 
  /// Create the widget.
  virtual void CreateWidget();
  
  void UpdateTreeFromMRML();
  void AddNodeToTree(vtkMRMLNode *node);
  void UpdateNodeInTree(vtkMRMLNode *node);
  void UpdateNodeInspector(vtkMRMLNode *node);
  vtkMRMLNode* GetSelectedNodeInTree();

  int IsLeafSelected(const char* leaf);
  void SetSelectesLeaves();

  void ClearCutNodes();

  //BTX
  std::vector< vtkMRMLTransformableNode* > CutNodes;
  std::vector<vtksys_stl::string> SelectedLeaves;
  //ETX

private:
  
  vtkKWTreeWithScrollbars *TreeWidget;
  vtkKWMenu *ContextMenu;

  vtkKWCheckButtonWithLabel  *ShowIDButton;

  vtkKWLabelWithLabel *NodeID;
  vtkKWEntryWithLabel *NodeName;

  vtkSlicerMRMLTreeWidget(const vtkSlicerMRMLTreeWidget&); /// Not implemented
  void operator=(const vtkSlicerMRMLTreeWidget&); /// Not Implemented
};

#endif

