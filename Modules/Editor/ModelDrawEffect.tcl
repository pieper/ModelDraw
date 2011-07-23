
package require Itcl

#########################################################
#
if {0} { ;# comment

  ModelDrawEffect an editor effect


# TODO : 

}
#
#########################################################

#
#########################################################
# ------------------------------------------------------------------
#                             ModelDrawEffect
# ------------------------------------------------------------------
#
# The class definition - define if needed (not when re-sourcing)
#
if { [itcl::find class ModelDrawEffect] == "" } {

  itcl::class ModelDrawEffect {

    inherit DrawEffect

    constructor {sliceGUI} {DrawEffect::constructor $sliceGUI} {}
    destructor {}

    # could be "linear" or "spline"
    public variable interpolation "spline"

    # a list of seeds - the callback info includes the mapping to list and index
    variable _seedSWidgets ""
    variable _storedSeedSWidgets ""
    variable _modelNode ""
    variable _controlPoints ;# array - initialized in constructor
    variable _populatingCurves 0

    # methods
    method processEvent {{caller ""} {event ""}} {}
    method apply {} {}
    method buildOptions {} {}
    method updateGUIFromMRML {} {}
    method tearDownOptions {} {}

    method positionActors {} {}
    method offset {} {
      # avoid roundoff issues caused by slicer controllers
      return [format %0.3f [[$sliceGUI GetLogic] GetSliceOffset]]
    }
    method controlPoints { {offset ""} } {
      if { $offset == "array" } {
        return [array get _controlPoints]
      }
      if { $offset == "" } {
        set offset [$this offset]
      }
      if { [info exists _controlPoints($offset)] } {
        return $_controlPoints($offset)
      }
      return ""
    }
    method addControlPoint {r a s} {}
    method deleteControlPoint {index} {}
    method splitControlPoint {index} {}
    method updateControlPoints {} {}
    method updateModel {} {}
    method updateCurve {} {}
    method applyCurve {} {}
    method seedMovingCallback {seed index} {}
    method seedContextMenuCallback {seed index} {}
    method seedKeyCallback {seed index key} {}
  }
}

# ------------------------------------------------------------------
#                        CONSTRUCTOR/DESTRUCTOR
# ------------------------------------------------------------------
itcl::body ModelDrawEffect::constructor {sliceGUI} {
  # take over event processing from the parent class
  $this configure -delegateEventProcessing 1
  array set _controlPoints {}
  set _scopeOptions "all visible"
}

itcl::body ModelDrawEffect::destructor {} {
  foreach seed [concat $_seedSWidgets $_storedSeedSWidgets] {
    ::SWidget::ProtectedDelete ::ModelDrawEffect::$seed
  }
  set _seedSWidgets ""
  set _storedSeedSWidgets ""
}

itcl::configbody ModelDrawEffect::interpolation {
  if { [lsearch "spline linear" $interpolation] == -1 } {
    error "invalid interpolation.  Must be linear or spline"
  }
  $this updateControlPoints
}

# ------------------------------------------------------------------
#                             METHODS
# ------------------------------------------------------------------

itcl::body ModelDrawEffect::positionActors { } {
  chain
}

itcl::body ModelDrawEffect::processEvent { {caller ""} {event ""} } {

  if { $_populatingCurves } {
    return
  }

  if { [$this preProcessEvent $caller $event] } {
    # superclass processed the event, so we don't
    $this positionActors
    [$sliceGUI GetSliceViewer] RequestRender
    return
  }

  if { [info exists o(spline)] } {
    if { $caller == $o(spline) } {
      if { [[$o(spline) GetWidget] GetSelectedState] } {
        EffectSWidget::ConfigureAll ModelDrawEffect -interpolation spline
      } else {
        EffectSWidget::ConfigureAll ModelDrawEffect -interpolation linear
      }
      return
    }
  }

  if { [info exists o(curves)] } {
    if { $caller == $o(curves) } {
      set row [[$o(curves) GetWidget] GetIndexOfFirstSelectedRow]
      if { $row == -1 } {
        # no valid row is selected, so ignore event
        return
      }
      set offset [lindex [[$o(curves) GetWidget] GetCellText $row 0] 0]
      [$sliceGUI GetLogic] SetSliceOffset $offset
      return
    }
  }

  chain $caller $event

  set grabID [$sliceGUI GetGrabID]
  if { ($grabID != "") && ($grabID != $this) } {
    $o(cursorActor) VisibilityOff
  } else {
    $o(cursorActor) VisibilityOn
  }

  set event [$sliceGUI GetCurrentGUIEvent] 
  set _currentPosition [$this xyToRAS [$_interactor GetEventPosition]]

  if { $caller == $sliceGUI } {
    switch $event {
      "LeftButtonPressEvent" {
        $o(cursorActor) VisibilityOff
      }
      "LeftButtonReleaseEvent" {
        eval $this addControlPoint $_currentPosition
        $o(cursorActor) VisibilityOn
        $sliceGUI SetGUICommandAbortFlag 1
      }
      "KeyPressEvent" { 
        set key [$_interactor GetKeySym]
        # TODO: fill in key bindings
        if { [lsearch "a x Return" $key] != -1 } {
          $sliceGUI SetCurrentGUIEvent "" ;# reset event so we don't respond again
          $sliceGUI SetGUICommandAbortFlag 1
          switch [$_interactor GetKeySym] {
            "Return" -
            "a" {
              $this apply
              set _actionState ""
            }
            "x" {
              $this deleteLastPoint
            }
          }
        } 
      }
      "EnterEvent" {
        $o(cursorActor) VisibilityOn
        if { [info exists o(tracingActor)] } {
         $o(tracingActor) VisibilityOn
        }
      }
      "LeaveEvent" {
        $o(cursorActor) VisibilityOff
        if { [info exists o(tracingActor)] } {
         $o(tracingActor) VisibilityOff
        }
      }
    }
  }

  if { $caller != "" && [$caller IsA "vtkMRMLSliceNode"] } {
    $this updateControlPoints
  }

  $this positionActors
  $this positionCursor
  [$sliceGUI GetSliceViewer] RequestRender
}

itcl::body ModelDrawEffect::apply {} {
  chain
}

itcl::body ModelDrawEffect::addControlPoint {r a s} {

  set offset [$this offset]
  lappend _controlPoints($offset) "$r $a $s"
  $this updateControlPoints
}

itcl::body ModelDrawEffect::deleteControlPoint {index} {

  set offset [$this offset]
  set _controlPoints($offset) [lreplace $_controlPoints($offset) $index $index]
  after idle $this updateControlPoints
}

itcl::body ModelDrawEffect::splitControlPoint {index} {

  # TODO: could make the tolerance and offset configurable
  set offset [$this offset]
  set cp [lindex $_controlPoints($offset) $index]
  foreach ele $cp {
    if { [expr abs($ele - $offset) > 0.001] } {
      lappend ras [expr $ele + 3]
    } else {
      lappend ras $offset
    }
  }
  set _controlPoints($offset) [linsert $_controlPoints($offset) $index $ras]
  after idle $this updateControlPoints
}

itcl::body ModelDrawEffect::updateControlPoints {} {
  #
  # update the seed widgets and the listbox
  #
  
  #
  # first - disable old seeds
  # - put them in a list for reuse
  #
  foreach seed $_seedSWidgets {
    $seed place -10000 -10000 -10000
    lappend _storedSeedSWidgets $seed
    $seed configure -visibility 0
  }
  set _seedSWidgets ""

  set offset [$this offset]
  set index 0
  foreach cp [$this controlPoints $offset] {
    # get a stored seed widget or create a new one
    if { [llength $_storedSeedSWidgets] > 0 } {
      set seedSWidget [lindex $_storedSeedSWidgets 0]
      set _storedSeedSWidgets [lrange $_storedSeedSWidgets 1 end]
    } else {
      set seedSWidget [SeedSWidget #auto $sliceGUI]
    }
    lappend _seedSWidgets $seedSWidget

    # configure the seed to act as a control point
    eval $seedSWidget place $cp
    $seedSWidget configure -scale 10
    $seedSWidget configure -glyph Circle
    $seedSWidget configure -visibility 1
    $seedSWidget configure -movingCommand "$this seedMovingCallback $seedSWidget $index"
    $seedSWidget configure -movedCommand "$this applyCurve"
    $seedSWidget configure -contextMenuCommand "$this seedContextMenuCallback $seedSWidget $index"
    $seedSWidget configure -keyCommand "$this seedKeyCallback $seedSWidget $index"
    $seedSWidget processEvent
    incr index
  }
  $this updateCurve
  $this applyCurve

  #
  # update listbox
  #
  if { [info exists o(curves)] } {
    set _populatingCurves 1
    set currentOffset [$this offset]
    set w [$o(curves) GetWidget]
    $w DeleteAllRows
    foreach offset [lsort -real [array names _controlPoints]] {
      # add it to the listbox
      set row [$w GetNumberOfRows]
      set text "$offset ([llength $_controlPoints($offset)] points)"
      if { $offset == $currentOffset } {
        set text "$text (visible)"
      }
      $w InsertCellText $row 0 $text
      # eval $w SetCellBackgroundColor $row $col(Color) [lrange [$lut GetTableValue $c] 0 2]
    }
    set _populatingCurves 0
  }
}

itcl::body ModelDrawEffect::seedKeyCallback {seed index key} {
  set offset [$this offset]
  set handled 0
  switch $key {
    "s" {
      # split the current control point and offset the new one in-plane by 3
      $this splitControlPoint $index
      set handled 1
    }
    "x" {
      # delete the current control point
      $this deleteControlPoint $index
      set handled 1
    }
  }
  return $handled
}

itcl::body ModelDrawEffect::applyCurve {} {

  if { [llength [$this controlPoints]] > 2 } {
    # delete the old drawing
    if { ![info exists o(change)] } {
      set o(change) [vtkNew vtkImageLabelChange]
    }
    $this configure -scope visible
    $o(change) SetInput [$this getInputLabel]
    $o(change) SetOutput [$this getOutputLabel]
    $o(change) SetInputLabel [EditorGetPaintLabel]
    $o(change) SetOutputLabel 0
    [$this getOutputLabel] Update
    $o(change) SetOutput ""
    $this postApply
    $this configure -scope all

    $this apply
    $this updateCurve
  }
}

itcl::body ModelDrawEffect::seedMovingCallback {seed index} {
  set offset [$this offset]
  set _controlPoints($offset) [lreplace $_controlPoints($offset) $index $index [$seed getRASPosition]]
  $this updateCurve
}

itcl::body ModelDrawEffect::seedContextMenuCallback {seed index} {
  set parent [[$sliceGUI GetSliceViewer] GetWidgetName]
  set contextMenu $parent.contextMenu
  catch "destroy $contextMenu"
  menu $contextMenu -bg white
  $contextMenu insert end command -label "Split" -command "$this splitControlPoint $index"
  $contextMenu insert end command -label "Delete" -command "$this deleteControlPoint $index"
  $contextMenu insert end separator
  $contextMenu insert end command -label "Close" -command ""
  eval $contextMenu post [winfo pointerxy $parent]
}

itcl::body ModelDrawEffect::updateCurve {} {
  
  if { ![info exists o(polyData)] } {
    # not yet initialized
    return
  }
  $this resetPolyData

  if { [llength [$this controlPoints]] > 1 } {

    switch $interpolation {
      "linear" {
        foreach cp [$this controlPoints] {
          eval $this addPoint $cp
        }
        eval $this addPoint [lindex [$this controlPoints] 0]
      }
      "spline" {
        if { ![info exists o(splineR)] } {
          foreach obj {splineR splineA splineS} {
            set o($obj) [vtkNew vtkKochanekSpline]
            $o($obj) SetClosed 1
          }
        }
        foreach obj {splineR splineA splineS} {
          $o($obj) RemoveAllPoints
        }
        set index 0
        foreach cp [$this controlPoints] {
          foreach {r a s} $cp {}
          $o(splineR) AddPoint $index $r
          $o(splineA) AddPoint $index $a
          $o(splineS) AddPoint $index $s
          incr index
        }
        foreach obj {splineR splineA splineS} {
          $o($obj) Compute
        }
        for {set t 0} {$t < $index} {set t [expr $t + 0.1]} {
          set r [$o(splineR) Evaluate $t]
          set a [$o(splineA) Evaluate $t]
          set s [$o(splineS) Evaluate $t]
          $this addPoint $r $a $s
        }
      }
    }
  }

  $this positionActors
  $this positionCursor
  [$sliceGUI GetSliceViewer] RequestRender
}

itcl::body ModelDrawEffect::updateModel {} {

  set modelNode [$o(modelSelect) GetSelected]
  if { $_modelNode == $modelNode } {
    # already have the right model - nothing to do
    return
  }

  if { $modelNode == "" } {
    # no model selected - can't do anything
    return
    set _modelNode ""
    $this updateControlPoints
    return
  }
  set _modelNode $modelNode

  set polyData [$_modelNode GetPolyData]
  if { $polyData == "" } {
    # no polydata yet - we need to create it
    set points [vtkPoints New]
    set polyData [vtkPolyData New]
    $polyData SetPoints $points

    set lines [vtkCellArray New]
    $polyData SetLines $lines
    set linesIDArray [$lines GetData]
    $linesIDArray Reset
    $linesIDArray InsertNextTuple1 0

    set polygons [vtkCellArray New]
    $polyData SetPolys $polygons
    set idArray [$polygons GetData]
    $idArray Reset
    $idArray InsertNextTuple1 0

    $_modelNode SetAndObservePolyData $polyData
  }
  
  set displayNode [$modelNode GetDisplayNode]
  if { $displayNode == "" } {
    # no displayNode yet - we need to create it
    set modelDisplay [vtkMRMLModelDisplayNode New]
    $modelDisplay SetColor 1 1 0  ;# yellow
    $modelDisplay SetScene $::slicer3::MRMLScene
    $::slicer3::MRMLScene AddNodeNoNotify $modelDisplay
    $_modelNode SetAndObserveDisplayNodeID [$modelDisplay GetID]
    $modelDisplay SetPolyData [$_modelNode GetPolyData]
  }
}

itcl::body ModelDrawEffect::buildOptions {} {

  # TODO: remove this if not needed
  if { 0 } {
    set o(modelSelect) [vtkSlicerNodeSelectorWidget New]
    $o(modelSelect) SetParent [$this getOptionsFrame]
    $o(modelSelect) Create
    $o(modelSelect) NewNodeEnabledOn
    $o(modelSelect) SetNodeClass "vtkMRMLModelNode" "" "" ""
    $o(modelSelect) SetMRMLScene [[$sliceGUI GetLogic] GetMRMLScene]
    $o(modelSelect) UpdateMenu
    $o(modelSelect) SetLabelText "Target Model:"
    $o(modelSelect) SetBalloonHelpString "The model to edit"
    pack [$o(modelSelect) GetWidgetName] -side top -anchor e -padx 2 -pady 2 

    $::slicer3::Broker AddObservation $o(modelSelect) AnyEvent "after idle $this updateModel"
  }

  set o(spline) [vtkNew vtkKWCheckButtonWithLabel]
  $o(spline) SetParent [$this getOptionsFrame]
  $o(spline) Create
  $o(spline) SetLabelText "Spline: "
  $o(spline) SetBalloonHelpString "Use spline interpolation."
  [$o(spline) GetLabel] SetWidth 22
  [$o(spline) GetLabel] SetAnchorToEast
  pack [$o(spline) GetWidgetName] \
    -side top -anchor w -padx 2 -pady 2 
  [$o(spline) GetWidget] SetSelectedState 1

  set SelectedStateChangedEvent 10000
  $::slicer3::Broker AddObservation [$o(spline) GetWidget] $SelectedStateChangedEvent "$this processEvent $o(spline) $SelectedStateChangedEvent"

  # call superclass version of buildOptions
  chain
  
  # we use scope internally, but users shouldn't see it
  pack forget [$o(scopeOption) GetWidgetName]

  #
  # the list of defined curves for this label
  #
  set o(curves) [vtkNew vtkKWMultiColumnListWithScrollbars]
  $o(curves) SetParent [$this getOptionsFrame]
  $o(curves) Create
  set w [$o(curves) GetWidget]
  $w SetSelectionTypeToRow
  $w SetSelectionModeToSingle
  $w MovableRowsOff
  $w MovableColumnsOn
  $o(curves) HorizontalScrollbarVisibilityOff
  $w SetHeight 5
  $w SetPotentialCellColorsChangedCommand $w "ScheduleRefreshColorsOfAllCellsWithWindowCommand"
  $w SetColumnSortedCommand $w "ScheduleRefreshColorsOfAllCellsWithWindowCommand"

  set col [$w AddColumn Curves]
  $w ColumnEditableOff $col
  $w SetColumnWidth $col 30
  $w SetColumnSortModeToReal $col

  set SelectionChangedEvent 10000
  $::slicer3::Broker AddObservation [$o(curves) GetWidget] $SelectionChangedEvent "$this processEvent $o(curves)"

  pack [$o(curves) GetWidgetName] \
    -side bottom -anchor s -fill both -expand true -padx 2 -pady 2 

  $this updateGUIFromMRML
}

itcl::body ModelDrawEffect::updateGUIFromMRML { } {
  set _updatingGUI 1
  chain
  set _updatingGUI 0
}

itcl::body ModelDrawEffect::tearDownOptions { } {

  # call superclass version of tearDownOptions
  chain

  foreach w "modelSelect spline curves" {
    if { [info exists o($w)] } {
      $o($w) SetParent ""
      pack forget [$o($w) GetWidgetName] 
    }
  }
}
