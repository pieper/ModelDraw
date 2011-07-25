
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
    variable _controlPoints ;# array - initialized in constructor - points for current label
    variable _updatingControlPoints 0
    variable _currentLabel ""

    # methods
    method processEvent {{caller ""} {event ""}} {}
    method apply {} {}
    method buildOptions {} {}
    method updateGUIFromMRML {} {}
    method tearDownOptions {} {}

    method positionActors {} {}
    method offset {} {
      # avoid roundoff issues caused by slicer controllers
      return [format %0.2f [[$sliceGUI GetLogic] GetSliceOffset]]
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
    method updateCurve { {controlPoints ""} } {}
    method applyCurve {} {}
    method seedMovingCallback {seed index} {}
    method seedContextMenuCallback {seed index} {}
    method seedKeyCallback {seed index key} {}
    method copyCurve {{from "nearest"}} {}
    method interpolatedControlPoints {} {}
    method splineToSlice {t} {}
  }
}

# ------------------------------------------------------------------
#                        CONSTRUCTOR/DESTRUCTOR
# ------------------------------------------------------------------
itcl::body ModelDrawEffect::constructor {sliceGUI} {
  if { [$_sliceNode GetLayoutName] != "Red" } {
    # only allow model object drawing in the Red slice plane (at least for now)
    itcl::delete object $this
    return
  }
  # take over event processing from the parent class
  $this configure -delegateEventProcessing 1
  set _scopeOptions "all visible"
  set _currentLabel [EditorGetPaintLabel]
  array set _controlPoints [$_parameterNode GetParameter ModelDraw,$_currentLabel]

  # create the spline interpolators - these are re-used for intra-slice 
  # and inter-slice interpolations
  foreach obj {splineR splineA splineS} {
    set o($obj) [vtkNew vtkKochanekSpline]
  }
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

  if { $_updatingControlPoints } {
    return
  }

  #
  # update curves to match current paint color
  #
  if { $_currentLabel == "" } {
    set _currentLabel [EditorGetPaintLabel]
  }

  if { $caller == $_parameterNode } {
    set paintLabel [EditorGetPaintLabel]
    if { $_currentLabel != $paintLabel } {
      array unset _controlPoints
      array set _controlPoints [$_parameterNode GetParameter ModelDraw,$paintLabel]
      set _currentLabel $paintLabel
      $this updateControlPoints
    }
    return
  }

  #
  # pass event up the chain
  #
  if { [$this preProcessEvent $caller $event] } {
    # superclass processed the event, so we don't
    $this positionActors
    [$sliceGUI GetSliceViewer] RequestRender
    return
  }

  #
  # handle events from widgets
  #

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
      if { ![info exists _controlPoints($offset)] } {
        $this copyCurve "nearest"
      } else {
        [$sliceGUI GetLogic] SetSliceOffset $offset
      }
      return
    }
  }

  chain $caller $event

  #
  # handle mouse/key events
  #
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

  #
  # changes in the slice view
  #
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
  # update:
  # the parameter node
  # the seed widgets 
  # the listbox
  #
 
  set _updatingControlPoints 1
  
  $_parameterNode SetParameter ModelDraw,$_currentLabel [array get _controlPoints]
  
  #
  # update seeds: first - disable old seeds
  # (put them in a list for reuse)
  # then create seeds for each control point
  #
  foreach seed $_seedSWidgets {
    $seed place -10000 -10000 -10000
    lappend _storedSeedSWidgets $seed
    $seed configure -visibility 0
  }
  set _seedSWidgets ""

  set offset [$this offset]
  set index 0
  set controlPoints [$this controlPoints $offset]
  if { [llength $controlPoints] > 0 } {
    # when there are user-defined control points on this slice,
    # display them as editable points
    foreach cp $controlPoints {
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
      $seedSWidget configure -inactive 0
      $seedSWidget configure -color "1.0 0.0 0.0"
      $seedSWidget configure -movingCommand "$this seedMovingCallback $seedSWidget $index"
      $seedSWidget configure -movedCommand "$this applyCurve"
      $seedSWidget configure -contextMenuCommand "$this seedContextMenuCallback $seedSWidget $index"
      $seedSWidget configure -keyCommand "$this seedKeyCallback $seedSWidget $index"
      $seedSWidget processEvent
      incr index
    }
    $this updateCurve $controlPoints
    $this applyCurve
  } else {
    # no user-defined control points on this slice,
    # so just display non-editable outline
    set controlPoints [$this interpolatedControlPoints]
    foreach cp $controlPoints {
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
      $seedSWidget configure -scale 8 
      $seedSWidget configure -glyph Circle
      $seedSWidget configure -visibility 1
      $seedSWidget configure -inactive 1
      $seedSWidget configure -color "0.5 0.5 1.0"
      $seedSWidget processEvent
      incr index
    }
    $this updateCurve $controlPoints
  }

  #
  # update listbox
  #
  if { [info exists o(curves)] } {
    set currentOffset [$this offset]
    set w [$o(curves) GetWidget]
    $w DeleteAllRows
    set visible ""
    foreach offset [lsort -real [array names _controlPoints]] {
      # add it to the listbox
      set row [$w GetNumberOfRows]
      set text "$offset ([llength $_controlPoints($offset)] points)"
      if { $offset == $currentOffset } {
        set text "$text (visible)"
        set visible $offset
      }
      $w InsertCellText $row 0 $text
      # eval $w SetCellBackgroundColor $row $col(Color) [lrange [$lut GetTableValue $c] 0 2]
    }
    if { $visible == "" } {
      set row [$w GetNumberOfRows]
      if { $row == 0 } {
        $w InsertCellText $row 0 "no points, select in Red viewer"
      } else {
        $w InsertCellText $row 0 "$currentOffset - no points, click to copy nearest"
      }
    }
  }
  set _updatingControlPoints 0
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

itcl::body ModelDrawEffect::updateCurve { {controlPoints ""} } {
  
  if { ![info exists o(polyData)] } {
    # not yet initialized
    return
  }
  $this resetPolyData

  if { $controlPoints == "" } {
    set controlPoints [$this controlPoints]
  }

  if { [llength $controlPoints] > 1 } {

    switch $interpolation {
      "linear" {
        foreach cp $controlPoints {
          eval $this addPoint $cp
        }
        eval $this addPoint [lindex $controlPoints 0]
      }
      "spline" {
        foreach obj {splineR splineA splineS} {
          $o($obj) RemoveAllPoints
          $o($obj) SetParametricRange 0 [llength $controlPoints]
          $o($obj) SetClosed 1
        }
        set index 0
        foreach cp $controlPoints {
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
  $w SetHeight 7
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

itcl::body ModelDrawEffect::copyCurve { {from "nearest"} } {
  
  set offset [$this offset]
  if { [info exists _controlPoints($offset)] } {
    if { $_controlPoints($offset) != "" } {
      if { ![EditorConfirmDialog "This slice already has points defined - delete them?"] } {
        return
      }
    }
  }

  set copyFrom ""

  if { $from == "nearest" } {
    set near ""
    set neardist 0
    foreach otherOffset [lsort -real [array names _controlPoints]] {
      set dist [expr abs($offset - $otherOffset)]
      if { $near == "" } {
        set near $otherOffset
        set neardist $dist
      } else {
        if { $dist < $neardist } {
          set near $otherOffset
          set neardist $dist
        }
      }
    }
    set copyFrom $near
  }

  if { $copyFrom == "" } {
    EditorErrorDialog "No control points to copy from"
    return
  }

  # get slice normal vector and offset control points by distance
  set delta [expr $offset - $copyFrom]
  set x [expr $delta * [[$_sliceNode GetSliceToRAS] GetElement 0 2]]
  set y [expr $delta * [[$_sliceNode GetSliceToRAS] GetElement 1 2]]
  set z [expr $delta * [[$_sliceNode GetSliceToRAS] GetElement 2 2]]
  
  set _controlPoints($offset) ""
  foreach cp $_controlPoints($copyFrom) {
    set xx [expr $x + [lindex $cp 0]]
    set yy [expr $y + [lindex $cp 1]]
    set zz [expr $z + [lindex $cp 2]]
    lappend _controlPoints($offset) "$xx $yy $zz"
  }

  $this updateControlPoints
}


itcl::body ModelDrawEffect::splineToSlice {t} {
  # helper - returns signed distance from current
  # slice plane to current spline evaluated at t
  # (may not be in mm if normal not normalized, but
  # will still work when optimizing to zero)
  
  # current slice plane origin and normal
  set or [[$_sliceNode GetSliceToRAS] GetElement 0 3]
  set oa [[$_sliceNode GetSliceToRAS] GetElement 1 3]
  set os [[$_sliceNode GetSliceToRAS] GetElement 2 3]
  set nr [[$_sliceNode GetSliceToRAS] GetElement 0 2]
  set na [[$_sliceNode GetSliceToRAS] GetElement 1 2]
  set ns [[$_sliceNode GetSliceToRAS] GetElement 2 2]

  # spline point
  set r [$o(splineR) Evaluate $t]
  set a [$o(splineA) Evaluate $t]
  set s [$o(splineS) Evaluate $t]

  set dist [expr $nr * ($r - $or) + $na * ($a - $oa) + $ns * ($s - $os)] 
  return $dist
}

itcl::body ModelDrawEffect::interpolatedControlPoints {} {
  # return a new set of control points at current offset by fitting
  # a spline to each of the control point in order
  #
  # - check that there are the same number of control points per slice
  # - for each control point set form a spline 
  # - intersect spline with current slice plane
  # -- use binary search to find t such that spline(t) is on offest plane
  #

  set offsets [lsort -real [array names _controlPoints]]
  if { [llength $offsets] < 2 } {
    # can't interpolate 0 or 1 point
    return ""
  }
  set firstOffset [lindex $offsets 0]
  set pointCount [llength $_controlPoints($firstOffset)]
  foreach offset $offsets {
    if { [llength $_controlPoints($offset)] != $pointCount } {
      # cannot interpolate - point count mismatch
      return ""
    }
  }

  set eps 1e-4
  set interpolatedControlPoints ""
  for {set point 0} {$point < $pointCount} {incr point} {
    # set up the spline between slices for this point
    # first, reset the spline
    foreach obj {splineR splineA splineS} {
      $o($obj) RemoveAllPoints
      $o($obj) SetParametricRange 0 1
      $o($obj) SetClosed 0
    }
    set index 0
    foreach offset $offsets {
      # add the control points
      set cp [lindex $_controlPoints($offset) $point]
      foreach {r a s} $cp {}
      $o(splineR) AddPoint $index $r
      $o(splineA) AddPoint $index $a
      $o(splineS) AddPoint $index $s
      incr index
    }
    # compute the spline
    foreach obj {splineR splineA splineS} {
      $o($obj) Compute
    }

    # perform the binary search
    set guesses 0
    set high 1
    set low 0
    set highDist [$this splineToSlice $high]
    set lowDist [$this splineToSlice $low]
    if { $highDist > $lowDist } {
      set sliceDir 1
    } else {
      set sliceDir -1
    }
    while { $guesses < 100 } {
      set guess [expr 0.5 * ($high - $low) + $low]
      set guessDist [$this splineToSlice $guess]
      if { [expr abs($guessDist)] < $eps } {
        # we have a good value of t (guess), so break
        break
      }
      if { [expr $sliceDir * $guessDist] > 0 } {
        set high $guess
      } else {
        set low $guess
      }
      if { [expr abs($high - $low)] < [expr $eps/10.] } {
        puts "Bad guess local minimum - use current guess anyway"
        break
      }
      incr guesses
    }

    set r [$o(splineR) Evaluate $guess]
    set a [$o(splineA) Evaluate $guess]
    set s [$o(splineS) Evaluate $guess]
    lappend interpolatedControlPoints "$r $a $s"
  }
  return $interpolatedControlPoints
}
