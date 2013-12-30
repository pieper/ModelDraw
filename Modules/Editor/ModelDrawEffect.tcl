
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
    public variable snap 0
    public variable splineSteps 10
    public variable patchSize "3 15"
    public variable debugExtractPatch 0
    public variable edgeTangents 0
    public variable edgeTangentSampleDistance 3
    public variable edgeTangentSampleSteps 90

    # a list of seeds - the callback info includes the mapping to list and index
    variable _seedSWidgets ""
    variable _storedSeedSWidgets ""
    variable _moveStartRAS ""
    variable _moveStartControlPoints ""
    variable _controlPoints ;# array - points for current label, indexed by offset
    variable _curves ;# array - interpolated for current points, indexed by offset
    variable _edgeTangents ;# array - image based estimate, indexed by offset
    variable _updatingControlPoints 0
    variable _currentLabel ""
    variable _sliceSplineR "" ;# local storage for dynamicly created temp splines
    variable _sliceSplineA ""
    variable _sliceSplineS ""
    variable _modelDrawNode "" ;# custom parameter node for this tool
    variable _hermiteWeights ;# array of precalculated weights per splineSteps

    # methods
    method processEvent {{caller ""} {event ""}} {}
    method apply {} {}
    method buildOptions {} {}
    method updateGUIFromMRML {} {}
    method tearDownOptions {} {}

    method modelDrawNode {} {}
    method positionActors {} {}
    method offset {} {
      # avoid roundoff issues caused by slicer controllers
      return [format %0.2f [[$sliceGUI GetLogic] GetSliceOffset]]
    }
    method controlPoints { {offset ""} } {}
    method controlCentroid { {offset ""} } {}
    method centroid { controlPoints } {}
    method addControlPoint {r a s} {}
    method deleteControlPoint {index} {}
    method splitControlPoint {index} {}
    method jumpToControlPoints {arg} {}
    method updateControlPoints {} {}
    method updateEdgeTangents {} {}
    method estimateEdgeTangent { cp } {}
    method updateCurve { {controlPoints ""} } {}
    method applyCurve {} {}
    method applyOperation {operation} {}
    method copyCurves {} {}
    method applyCurves {} {}
    method seedStartMovingCallback {seed index} {}
    method seedMovingCallback {seed index} {}
    method seedContextMenuCallback {seed index} {}
    method seedKeyCallback {seed index key} {}
    method copyCurve { {promptForOverwrite "yes"} } {}
    method interpolatedControlPoints {} {}
    method splineToSlice {t} {}
    method snapCurve { {controlPoints ""} } {}
    method extractPatch { imagePatch point normal tangent sliceNormal } {}
    method comparePatches {i1 i2} {}
    method hermiteWeights {} {}
    method importDialog {} {}
    method importCallback {} {}
  }
}

# ------------------------------------------------------------------
#                        CONSTRUCTOR/DESTRUCTOR
# ------------------------------------------------------------------
itcl::body ModelDrawEffect::constructor {sliceGUI} {

  set _modelDrawNode [$this modelDrawNode]
  array set _controlPoints [$_modelDrawNode GetParameter $_currentLabel]

  if { [$_sliceNode GetLayoutName] != "Red" } {
    # only allow model object drawing in the Red slice plane (at least for now)
    itcl::delete object $this
    return
  }
  # take over event processing from the parent class
  $this configure -delegateEventProcessing 1
  set _scopeOptions "all visible"
  set _currentLabel [EditorGetPaintLabel]

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

  if { $_sliceSplineR != "" } {
    $_sliceSplineR Delete
    $_sliceSplineA Delete
    $_sliceSplineS Delete
  }
}

# ------------------------------------------------------------------
#                             PUBLIC VARIABLES
# ------------------------------------------------------------------
#
itcl::configbody ModelDrawEffect::interpolation {
  if { [lsearch "spline linear" $interpolation] == -1 } {
    error "invalid interpolation.  Must be linear or spline"
  }
  $this updateControlPoints
}

itcl::configbody ModelDrawEffect::snap {
  $this updateCurve
}

itcl::configbody ModelDrawEffect::edgeTangents {
  $this updateCurve
}

# ------------------------------------------------------------------
#                             METHODS
# ------------------------------------------------------------------

itcl::body ModelDrawEffect::modelDrawNode { } {
  if { $_modelDrawNode != "" } {
    return $_modelDrawNode
  }
  set number [$::slicer3::MRMLScene GetNumberOfNodesByClass vtkMRMLScriptedModuleNode]
  for {set n 0} {$n < $number} {incr n} {
    set node [$::slicer3::MRMLScene GetNthNodeByClass $n vtkMRMLScriptedModuleNode]
    if { [$node GetModuleName] == "ModelDraw" } {
      return $node
    }
  }
  
  # get here if there is no model draw parameter node yet in scene
  set node [vtkMRMLScriptedModuleNode New]
  $node SetModuleName "ModelDraw"
  $::slicer3::MRMLScene AddNode $node
  $node Delete

  return [$this modelDrawNode]
}

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


  # parameter node changed, update control points
  if { $caller == $_parameterNode } {
    set paintLabel [EditorGetPaintLabel]
    if { $_currentLabel != $paintLabel } {
      array unset _controlPoints
      if { $_modelDrawNode != "" } {
        array set _controlPoints [$_modelDrawNode GetParameter $paintLabel]
      } else {
        array set _controlPoints ""
      }
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

  if { [info exists o(snap)] } {
    if { $caller == $o(snap) } {
      if { [[$o(snap) GetWidget] GetSelectedState] } {
        EffectSWidget::ConfigureAll ModelDrawEffect -snap 1
      } else {
        EffectSWidget::ConfigureAll ModelDrawEffect -snap 0
      }
      return
    }
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

  if { [info exists o(edgeTangents)] } {
    if { $caller == $o(edgeTangents) } {
      if { [[$o(edgeTangents) GetWidget] GetSelectedState] } {
        EffectSWidget::ConfigureAll ModelDrawEffect -edgeTangents 1
      } else {
        EffectSWidget::ConfigureAll ModelDrawEffect -edgeTangents 0
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
        $this copyCurve
      } else {
        [$sliceGUI GetLogic] SetSliceOffset $offset
      }
      return
    }
  }

  if { [info exists o(deleteCurve)] } {
    if { $caller == $o(deleteCurve) } {
      set offset [$this offset]
      if { [info exists _controlPoints($offset)] } {
        unset _controlPoints($offset)
        $this updateControlPoints
      }
      return
    }
  }

  if { [info exists o(applyCurves)] } {
    if { $caller == $o(applyCurves) } {
      $this applyCurves
      return
    }
  }

  if { [info exists o(copyCurves)] } {
    if { $caller == $o(copyCurves) } {
      $this copyCurves
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
        if { ![$_interactor GetShiftKey] } {
          # ignore the release event if shift is down - could have been a pan event for
          # non-three button mice
          eval $this addControlPoint $_currentPosition
          $o(cursorActor) VisibilityOn
          $sliceGUI SetGUICommandAbortFlag 1
        }
      }
      "KeyPressEvent" { 
        set key [$_interactor GetKeySym]
        puts [$_interactor GetKeySym]
        # TODO: fill in key bindings
        if { [lsearch "a x period j J Return" $key] != -1 } {
          $sliceGUI SetCurrentGUIEvent "" ;# reset event so we don't respond again
          $sliceGUI SetGUICommandAbortFlag 1
          switch [$_interactor GetKeySym] {
            "Return" -
            "a" {
              $this apply
              set _actionState ""
            }
            "period" {
              eval $this addControlPoint $_currentPosition
              $sliceGUI SetGUICommandAbortFlag 1
            }
            "x" {
              $this deleteLastPoint
            }
            "j" {
              $this jumpToControlPoints "next"
            }
            "J" {
              $this jumpToControlPoints "previous"
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

itcl::body ModelDrawEffect::controlPoints { {offset ""} } {
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

itcl::body ModelDrawEffect::controlCentroid { {offset ""} } {
  if { $offset == "" } {
    set offset [$this offset]
  }
  if { [info exists _controlPoints($offset)] } {
    set cps $_controlPoints($offset)
  } else {
    set cps [$this interpolatedControlPoints]
  }
  if { $cps == "" } {
    return ""
  }

  return [$this centroid $cps]
}

itcl::body ModelDrawEffect::centroid { controlPoints } {
  
  set rSum 0
  set aSum 0
  set sSum 0
  foreach cp $controlPoints {
    foreach {r a s} $cp {}
    set rSum [expr $rSum + $r]
    set aSum [expr $aSum + $a]
    set sSum [expr $sSum + $s]
  }
  set rCentroid [expr $rSum / [llength $controlPoints]]
  set aCentroid [expr $aSum / [llength $controlPoints]]
  set sCentroid [expr $sSum / [llength $controlPoints]]
  return "$rCentroid $aCentroid $sCentroid"
}

itcl::body ModelDrawEffect::addControlPoint {r a s} {

  set offset [$this offset]
  if { [array names _controlPoints] != "" } {
    # there are control points on other slices
    if { ![info exists _controlPoints($offset)] } {
      # no control points yet on this slice, offer to copy
      if { [EditorConfirmDialog "Control points exist on other slices - copy them?"] } {
        $this copyCurve
        return
      }
    } else {
      if { [llength [array names _controlPoints]] > 1 } {
        # already control points on this slice (copied), offer to ignore click
        if { ![EditorConfirmDialog "Really add new control point?"] } {
          return
        }
      }
    }
  }

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


itcl::body ModelDrawEffect::jumpToControlPoints {arg} {
  # go to prev/next slice offset where points are defined

  set offset [$this offset]
  set sortedOffsets [lsort -real -increasing [array names _controlPoints]]
  set offsetIndex [lsearch $sortedOffsets $offset]
  if { $offsetIndex == -1 } {
    set sortedOffsets [concat $sortedOffsets $offset]
    set sortedOffsets [lsort -real -increasing $sortedOffsets]
  }
  set offsetIndex [lsearch $sortedOffsets $offset]
  set jumpTo $offset
  if { $arg == "previous" } {
    set jumpTo [lindex $sortedOffsets $offsetIndex-1]
    if { $jumpTo == "" } {
      set jumpTo [lindex $sortedOffsets end]
    }
  }
  if { $arg == "next" } {
    set jumpTo [lindex $sortedOffsets $offsetIndex+1]
    if { $jumpTo == "" } {
      set jumpTo [lindex $sortedOffsets 0]
    }
  }
  [$sliceGUI GetLogic] SetSliceOffset $jumpTo
}


itcl::body ModelDrawEffect::updateControlPoints {} {
  #
  # update:
  # the parameter node
  # the seed widgets 
  # the listbox
  #
  
  if { $_modelDrawNode == "" } {
    # not initialized yet
    return
  }

  set _updatingControlPoints 1

  # copy control points into the model draw node
  $_modelDrawNode SetParameter $_currentLabel [array get _controlPoints]
  
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
      $seedSWidget configure -startMovingCommand "$this seedStartMovingCallback $seedSWidget $index"
      $seedSWidget configure -movingCommand "$this seedMovingCallback $seedSWidget $index"
      $seedSWidget configure -movedCommand "$this updateControlPoints"
      $seedSWidget configure -contextMenuCommand "$this seedContextMenuCallback $seedSWidget $index"
      $seedSWidget configure -keyCommand "$this seedKeyCallback $seedSWidget $index"
      $seedSWidget processEvent
      incr index
    }
    $this updateCurve $controlPoints
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
    set visibleRow 0
    foreach offset [lsort -real [array names _controlPoints]] {
      # add it to the listbox
      set row [$w GetNumberOfRows]
      set text "$offset ([llength $_controlPoints($offset)] points)"
      if { $offset == $currentOffset } {
        set text "$text (visible)"
        set visible $offset
        set visibleRow $row
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
    } else {
      $w SeeRow $visibleRow
    }
  }

  #
  # update defined points label
  #
  [$o(definedPointsLabel) GetWidget] SetText "None"
  if { $_modelDrawNode != "" } {
    $_modelDrawNode RequestParameterList
    array set p [$_modelDrawNode GetParameterList]
    set definedPoints ""
    foreach l [array names p] {
      if { $p($l) != "" } {
        lappend definedPoints $l
      }
    }
    [$o(definedPointsLabel) GetWidget] SetText [lsort -integer $definedPoints]
  }

  # enable the apply curves option if more than one slice is defined
  if { [info exists o(applyCurves)] } {
    if { [llength [array names _controlPoints]] > 1 } {
      $o(applyCurves) SetStateToNormal
      $o(copyCurves) SetStateToNormal
    } else {
      $o(copyCurves) SetStateToDisabled
    }
  }

  # enable the delete points option if anything exists on this slice
  if { [info exists o(applyCurves)] } {
    if { [info exists _controlPoints($offset)] } {
      $o(deleteCurve) SetStateToNormal
    } else {
      $o(deleteCurve) SetStateToDisabled
    }
  }

  set _updatingControlPoints 0
}


itcl::body ModelDrawEffect::estimateEdgeTangent { cp } {
  # - search neighborhood for best match image patch at various rotations
 
  set sliceToRAS [$_sliceNode GetSliceToRAS]
  foreach row {0 1 2} {
    lappend tangent [$sliceToRAS GetElement $row 0]
    lappend normal [$sliceToRAS GetElement $row 1]
    lappend sliceNormal [$sliceToRAS GetElement $row 2]
  }
  set tangent [$this normalize $tangent]
  set normal [$this normalize $normal]
  set sliceNormal [$this normalize $sliceNormal]

  set cpPatch [vtkImageData New]
  set samplePatch [vtkImageData New]
  $this extractPatch $cpPatch $cp $normal $tangent $sliceNormal

  # rotate tangent and normal, and offset cp by edgeTangentSampleDistance along tangent
  # - keep track of min weight
  set minWeight 1e6
  set minAngle ""
  set minTangent ""
  set twoPi [expr 3.14159 * 2]
  set angleStep [expr $twoPi / $edgeTangentSampleSteps]
  for {set angle 0} {$angle < $twoPi} {set angle [expr $angle + $angleStep]} {
    set c [expr cos($angle)]
    set s [expr sin($angle)]
    set newT ""; set newN ""; set newP ""
    foreach tele $tangent nele $normal {
      lappend newT [expr $c * $tele - $s * $nele]
      lappend newN [expr $c * $nele + $s * $tele]
    }
    foreach pele $cp tele $newT {
      lappend newP [expr $pele + $edgeTangentSampleDistance * $tele]
    }

    $this extractPatch $samplePatch  $newP  $normal  $tangent  $sliceNormal

    set weight [$this comparePatches $cpPatch $samplePatch]

    if { $weight < $minWeight } {
      set minWeight $weight
      set minAngle $angle
      set minTangent $newT
    }
  }

  # TODO: optimize tangent estimate around minTangent
  # TODO: calculate in and out tangents along line perp to minTangent

  $samplePatch Delete
  $cpPatch Delete

  return $minTangent
}


itcl::body ModelDrawEffect::updateEdgeTangents {} {

  #
  # for each control point
  # - calculate a new tangent if needed
  # - place locked seeds along tangent
  #

  set offset [$this offset]

  if { ![info exists _controlPoints($offset)] } {
    return
  }

  if { ![info exists _edgeTangents($offset,controlPoints)] } {
    # if no existing control points, set up dummies
    foreach cp $_controlPoints($offset) {
      lappend _edgeTangents($offset,controlPoints) "none"
      lappend _edgeTangents($offset,tangents) "none"
    }
  }

  set newTangents ""
  foreach cp $_controlPoints($offset) \
          oldCP $_edgeTangents($offset,controlPoints) \
          oldTangent $_edgeTangents($offset,tangents) {
    if { $cp == $oldCP } {
      # if the control point hasn't changed, use old tangent
      lappend newTangents $oldTangent
    } else {
      lappend newTangents [$this estimateEdgeTangent $cp]
    }
  }
  set _edgeTangents($offset,controlPoints) $_controlPoints($offset)
  set _edgeTangents($offset,tangents) $newTangents

  foreach cp $_controlPoints($offset) \
          tangent $_edgeTangents($offset,tangents) {
    # get a stored seed widget or create a new one
    if { [llength $_storedSeedSWidgets] > 0 } {
      set seedSWidget [lindex $_storedSeedSWidgets 0]
      set _storedSeedSWidgets [lrange $_storedSeedSWidgets 1 end]
    } else {
      set seedSWidget [SeedSWidget #auto $sliceGUI]
    }
    lappend _seedSWidgets $seedSWidget

    set seedPoint ""
    foreach cpele $cp tele $tangent {
      lappend seedPoint [expr $cpele + $edgeTangentSampleDistance * $tele]
    }
    eval $seedSWidget place $seedPoint
    $seedSWidget configure -scale 5 
    $seedSWidget configure -glyph Circle
    $seedSWidget configure -visibility 1
    $seedSWidget configure -inactive 1
    $seedSWidget configure -color "0.5 0.8 0.5"
    $seedSWidget processEvent
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
  # rasterize the current curve to the current slice

  if { [$o(xyPoints) GetNumberOfPoints] > 2 } {
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

itcl::body ModelDrawEffect::applyOperation {operation} {
  # go through all current set of curves and apply operation to all slices in the
  # volume between high and low offsets
 
  #
  # find the sliceSWidget to use for increment
  # find min/max offset for iteration
  # update to current real or interpolated curve
  # apply operation
  #

  set sliceSWidget ""
  foreach sw [itcl::find objects -class SliceSWidget] {
    if { [$sw cget -sliceGUI] == $sliceGUI } {
      set sliceSWidget $sw
    }
  }

  set offsets [lsort -real [array names _controlPoints]]
  if { [llength $offsets] < 2 } {
    # can't interpolate 0 or 1 point
    return ""
  }
  set firstOffset [lindex $offsets 0]
  set lastOffset [lindex $offsets end]

  [$sliceGUI GetLogic] SetSliceOffset $firstOffset
  while { [[$sliceGUI GetLogic] GetSliceOffset] < $lastOffset } {
    $sliceSWidget incrementSlice
    eval $operation
    update
  }
}

itcl::body ModelDrawEffect::copyCurves {} {
  # Copy interpolated curves to all slices where they don't yet exist

  $this applyOperation "$this copyCurve no"
}

itcl::body ModelDrawEffect::applyCurves {} {
  # rasterize the current set of curves to all slices in the
  # volume between high and low offsets
  #

  $this applyOperation "$this applyCurve"
}

itcl::body ModelDrawEffect::seedStartMovingCallback {seed index} {
  # keep track of start point to support multi-seed moves
  set _moveStartRAS [$seed getRASPosition]
  set _moveStartControlPoints [$this controlPoints]
}

itcl::body ModelDrawEffect::seedMovingCallback {seed index} {
  set offset [$this offset]
  if { [$_interactor GetShiftKey] } {
    if { [$_interactor GetControlKey] } {
      # rotate all seeds
      foreach {startR startA startS} $_moveStartRAS {r a s} [$seed getRASPosition] {}
      foreach {centR centA centS} [$this controlCentroid] {}
      set centToStartR [expr $startR - $centR]
      set centToStartA [expr $startA - $centA]
      set centToStartS [expr $startS - $centS]
      set centToStartDist [expr sqrt($centToStartR * $centToStartR + $centToStartA * $centToStartA + $centToStartS * $centToStartS)]
      set centToStartR [expr $centToStartR / $centToStartDist]
      set centToStartA [expr $centToStartA / $centToStartDist]
      set centToStartS [expr $centToStartS / $centToStartDist]
      set centToPointR [expr $r - $centR]
      set centToPointA [expr $a - $centA]
      set centToPointS [expr $s - $centS]
      set centToPointDist [expr sqrt($centToPointR * $centToPointR + $centToPointA * $centToPointA + $centToPointS * $centToPointS)]
      set centToPointR [expr $centToPointR / $centToPointDist]
      set centToPointA [expr $centToPointA / $centToPointDist]
      set centToPointS [expr $centToPointS / $centToPointDist]
      set dotR [expr $centToStartR * $centToPointR]
      set dotA [expr $centToStartA * $centToPointA]
      set dotS [expr $centToStartS * $centToPointS]
      set dot [expr $dotR + $dotA + $dotS]
      if { $dot < 0 } {
        return
      }
      set angle [expr acos($dot)]
      set crossR [expr $centToStartA * $centToPointS - $centToStartS * $centToPointA]
      set crossA [expr $centToStartS * $centToPointR - $centToStartR * $centToPointS]
      set crossS [expr $centToStartR * $centToPointA - $centToStartA * $centToPointR];
      set nR [[$_sliceNode GetSliceToRAS] GetElement 0 2]
      set nA [[$_sliceNode GetSliceToRAS] GetElement 1 2]
      set nS [[$_sliceNode GetSliceToRAS] GetElement 2 2]
      set dotR [expr $crossR * $nR]
      set dotA [expr $crossA * $nA]
      set dotS [expr $crossS * $nS]
      set dot [expr $dotR + $dotA + $dotS]
      if { $dot < 0 } {
        set angle [expr -1. * $angle]
      }

      set centXYZ [$this rasToXYZ [$this controlCentroid]]
      foreach {centX centY centZ} $centXYZ {}
      set trans [vtkMatrix4x4 New]
      $trans SetElement 0 3 [expr -1 * $centX]
      $trans SetElement 1 3 [expr -1 * $centY]
      $trans SetElement 2 3 [expr -1 * $centZ]
      set rotate [vtkMatrix4x4 New]
      $rotate SetElement 0 0 [expr cos($angle)]
      $rotate SetElement 0 1 [expr sin($angle)]
      $rotate SetElement 1 0 [expr -sin($angle)]
      $rotate SetElement 1 1 [expr cos($angle)]
      $rotate Multiply4x4 $rotate $trans $rotate
      $trans Identity
      $trans SetElement 0 3 $centX
      $trans SetElement 1 3 $centY
      $trans SetElement 2 3 $centZ
      $rotate Multiply4x4 $trans $rotate $rotate

      # apply rotation around centroid to all points
      for {set i 0} {$i < [llength $_seedSWidgets]} {incr i} {
        set cp [lindex $_moveStartControlPoints $i]
        set cpXYZ [$this rasToXYZ $cp]
        set xyzw [eval $rotate MultiplyPoint $cpXYZ 1]
        foreach {x y z w} $xyzw {}
        set sw [lindex $_seedSWidgets $i]
        $sw setXYZPosition $x $y $z
        set _controlPoints($offset) [lreplace $_controlPoints($offset) $i $i [$sw getRASPosition]]
      }
      $rotate Delete
      $trans Delete
    } else {
      # move all seeds
      foreach {startR startA startS} $_moveStartRAS {r a s} [$seed getRASPosition] {}
      set deltaR [expr $r - $startR]
      set deltaA [expr $a - $startA]
      set deltaS [expr $s - $startS]
      for {set i 0} {$i < [llength $_seedSWidgets]} {incr i} {
        set cp [lindex $_moveStartControlPoints $i]
        foreach {cpr cpa cps} $cp {}
        set cpr [expr $cpr + $deltaR]
        set cpa [expr $cpa + $deltaA]
        set cps [expr $cps + $deltaS]
        set _controlPoints($offset) [lreplace $_controlPoints($offset) $i $i "$cpr $cpa $cps"]
        if { $i != $index } {
          # skip the widget that generated the event - it will update itself
          set sw [lindex $_seedSWidgets $i]
          $sw place $cpr $cpa $cps
        }
      }
    }
  } else {
    if { [$_interactor GetControlKey] } {
      # scale all seeds
      foreach {startR startA startS} $_moveStartRAS {r a s} [$seed getRASPosition] {}
      set deltaR [expr $r - $startR]
      set deltaA [expr $a - $startA]
      set deltaS [expr $s - $startS]
      foreach {centR centA centS} [$this controlCentroid] {}
      set centToPointR [expr $startR - $centR]
      set centToPointA [expr $startA - $centA]
      set centToPointS [expr $startS - $centS]
      set centToPointDist [expr sqrt($centToPointR * $centToPointR + $centToPointA * $centToPointA + $centToPointS * $centToPointS)]
      set centToPointR [expr $centToPointR / $centToPointDist]
      set centToPointA [expr $centToPointA / $centToPointDist]
      set centToPointS [expr $centToPointS / $centToPointDist]
      set dotR [expr $deltaR * $centToPointR]
      set dotA [expr $deltaA * $centToPointA]
      set dotS [expr $deltaS * $centToPointS]
      set dot [expr $dotR + $dotA + $dotS]
      set dist [expr sqrt($dotR * $dotR + $dotA * $dotA + $dotS * $dotS)]
      set ratio [expr $dist / $centToPointDist]
      if { $dot < 0 } {
        set ratio [expr -1. * $ratio]
      }
      for {set i 0} {$i < [llength $_seedSWidgets]} {incr i} {
        set cp [lindex $_moveStartControlPoints $i]
        foreach {cpr cpa cps} $cp {}
        set centToCPR [expr $cpr - $centR]
        set centToCPA [expr $cpa - $centA]
        set centToCPS [expr $cps - $centS]
        set cpr [expr $cpr + $ratio * $centToCPR]
        set cpa [expr $cpa + $ratio * $centToCPA]
        set cps [expr $cps + $ratio * $centToCPS]
        set _controlPoints($offset) [lreplace $_controlPoints($offset) $i $i "$cpr $cpa $cps"]
        if { 1 || $i != $index } {
          # skip the widget that generated the event - it will update itself
          set sw [lindex $_seedSWidgets $i]
          $sw place $cpr $cpa $cps
        }
      }
    } else {
      # move just selected control point
      set _controlPoints($offset) [lreplace $_controlPoints($offset) $index $index [$seed getRASPosition]]
    }
  }
  set oldTangentMode $edgeTangents
  $this configure -edgeTangents 0
  $this updateCurve
  $this configure -edgeTangents $oldTangentMode
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

  if { $edgeTangents } {
    # TODO: when showing an interpolated set of control points
    #  probably better to interpolate tangents rather than recompute them
    #$this updateEdgeTangents $controlPoints
    $this updateEdgeTangents
  }

  set offset [$this offset]
  set _curves($offset) ""

  if { [llength $controlPoints] > 1 } {

    switch $interpolation {
      "linear" {
        foreach cp $controlPoints {
          eval $this addPoint $cp
          lappend _curves($offset) $cp
        }
        eval $this addPoint [lindex $controlPoints 0]
        lappend _curves($offset) [lindex $controlPoints 0]
      }
      "spline" {
        if { $edgeTangents } {
          # use hermite interpolation
          if { ![info exists _edgeTangents($offset,tangents)] } {
            return;
          }
          set tangents $_edgeTangents($offset,tangents)
          set h [$this hermiteWeights]

          set pointCount [llength $controlPoints]
          for {set index 0} {$index < $pointCount} {incr index} {
            set index1 [expr ($index + 1) % $pointCount]
            set index2 [expr ($index + 2) % $pointCount]
            set indexm1 [expr $index -1]
            if { $indexm1 < 0 } {
              set indexm1 [expr $pointCount - 1]
            }
            set cpm1 [lindex $controlPoints $indexm1]
            set cp0 [lindex $controlPoints $index]
            set cp1 [lindex $controlPoints $index1]
            set cp2 [lindex $controlPoints $index2]
            set t0 [lindex $tangents $index]
            set t1 [lindex $tangents $index1]

            set inVector [$this minus $cp0 $cpm1]
            set outVector [$this minus $cp1 $cp0]
            set v0 [$this multiply 0.5 [$this plus $inVector $outVector]]

            set inVector [$this minus $cp1 $cp0]
            set outVector [$this minus $cp2 $cp1]
            set v1 [$this multiply 0.5 [$this plus $inVector $outVector]]

            if { [$this dot $v0 $t0] < 0 } {
              set t0 [$this negative $t0]
            }
            if { [$this dot $v1 $t1] < 0 } {
              set t1 [$this negative $t1]
            }
            set t0 [$this multiply [expr 1.5 * [$this length $v0]] $t0]
            set t1 [$this multiply [expr 1.5 * [$this length $v1]] $t1]

            for {set step 0} {$step < $splineSteps} {incr step} {
              foreach {h00 h10 h01 h11} [lindex $h $step] {}
              set p ""
              foreach cp0ele $cp0 cp1ele $cp1 t0ele $t0 t1ele $t1 {
                lappend p [expr $h00 * $cp0ele + $h10 * $t0ele + $h01 * $cp1ele + $h11 * $t1ele]
              }
              eval $this addPoint $p
              lappend _curves($offset) $p
            }
          }

        } else {
          # no edge tangents, use TBC Splines
          foreach obj {splineR splineA splineS} {
            $o($obj) RemoveAllPoints
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
          set steps [expr $index * $splineSteps]
          set deltaT [expr 1. / $splineSteps]
          set t 0
          for {set step 0} {$step <= $steps} {incr step} {
            set r [$o(splineR) Evaluate $t]
            set a [$o(splineA) Evaluate $t]
            set s [$o(splineS) Evaluate $t]
            $this addPoint $r $a $s
            lappend _curves($offset) "$r $a $s"
            set t [expr $t + $deltaT]
          }
        }
      }
    }
  }

  if { $snap } {
    $this snapCurve
  }

  $this positionActors
  $this positionCursor
  [$sliceGUI GetSliceViewer] RequestRender
}

itcl::body ModelDrawEffect::snapCurve { {controlPoints ""} } {

  #
  # For each control point, extract a patch of
  # image data perpendicular to the curve
  #
  # For each point on the curve
  # - interoplate image patches
  # - get local normal to curve
  # - move along normal
  # -- find best fit to interpolated patch
  #
 
  if { $controlPoints == "" } {
    set controlPoints [$this controlPoints]
  }

  set offset [$this offset]
  set pointCount [llength $_curves($offset)]
  if { $pointCount < 3 } {
    return
  }

  set sliceToRAS [$_sliceNode GetSliceToRAS]
  set sliceNormal [list \
    [$sliceToRAS GetElement 0 2] \
    [$sliceToRAS GetElement 1 2] \
    [$sliceToRAS GetElement 2 2] ]
  set sliceNormal [$this normalize $sliceNormal]

  set lastPoint [expr $pointCount - 1]
  set normals ""
  set tangents ""
  for {set i 0} {$i < $pointCount} {incr i} {
    set iIn [expr $i - 1]
    set iOut [expr $i + 1]
    if { $iIn < 0 } {
      set iIn $lastPoint
    }
    if { $iOut > $lastPoint  } {
      set iOut 0
    }
    foreach pp {"" In Out} {
      set p$pp [lindex $_curves($offset) [set i$pp]]
    }
    set tangent ""
    foreach a $p b $pIn c $pOut {
      lappend tangent [expr 0.5 * ($a - $b) + ($c - $a)]
    }
    set tangent [$this normalize $tangent]
    lappend tangents $tangent
    lappend normals [$this normalize [$this cross $tangent $sliceNormal]]
  }

  # get the image patch at each control point
  set i 0
  set cpPatches ""
  foreach cp $controlPoints {
    set imagePatch [vtkImageData New]
    set point [lindex $_curves($offset) $i]
    set normal [lindex $normals $i]
    set tangent [lindex $tangents $i]
    $this extractPatch $imagePatch $point $normal $tangent $sliceNormal
    lappend cpPatches $imagePatch
    incr i $splineSteps
  }
  
  # set up the interpolation pipeline
  if { ![info exists o(cmpBlend)] } {
    set o(cmpBlend) [vtkNew vtkImageBlend]
  }

  $this resetPolyData
  set i 0
  set imagePatch [vtkImageData New]
  foreach normal $normals tangent $tangents point $_curves($offset) {

    # find the two control points on either side of the curve
    # and the interpolation value (t)
    set cp0 [expr $i / $splineSteps]
    set cp1 [expr $cp0 + 1]
    if { $cp1 >= [llength $controlPoints] } {
      set cp1 0
    }
    set t [expr ($i % $splineSteps) / (1. * $splineSteps) ]

    $o(cmpBlend) RemoveAllInputs
    $o(cmpBlend) SetInput 0 [lindex $cpPatches $cp0]
    $o(cmpBlend) SetInput 1 [lindex $cpPatches $cp1]
    $o(cmpBlend) SetOpacity 1 $t

    # optimize the offset for each point on the curve as a distance
    # along the normal 
    # - first, sample a range of offsets
    # - pick the one with the min weight
    # - TODO: check standard deviation of guesses - ignore non-clear winners
    # - use neighbors to initialize guesses - don't stray far from previous winner
    # - have weighted falloff for diff image (priorize near sample point)
    # - check gradient of interpolated image - if not much edge, discount snap guess
    #
    # Considerations:
    # - how big a sample patch
    # - how big a search area
    # - how much to weight the center of the sample
    # - when to decide there is no good matching feature
    # -- then using the previous winner as new control point
    # -- or go back to original curve?
    # - whether to consider the near by winner samples compared to the interpolated control points
    # - use last winner as guess for optimizer
    # - other ways to enforce smoothness

    set minWeight 1e6
    set minSample ""
    set minS ""
    set range 10
    set steps 10
    for {set step 0} {$step < $steps} {incr step} {
      set s [expr -1 * $range/2. + ($step/(1.*$steps)) * $range]
      set samplePoint ""
      foreach p $point n $normal {
        lappend samplePoint [expr $p + $s * $n]
      }

      # extract the patch for the current guess
      $this extractPatch $imagePatch $samplePoint $normal $tangent $sliceNormal

      if { $debugExtractPatch } {
        if { ![info exists o(blendviewer)] } {
          set o(blendviewer) [vtkNew vtkImageViewer]
          set o(blendmag) [vtkNew vtkImageMagnify]
        }
        set logic [[$sliceGUI GetLogic]  GetBackgroundLayer]
        set node [$logic GetVolumeNode]
        set displayNode [$node GetDisplayNode]
        $o(blendviewer) SetColorWindow [$displayNode GetWindow]
        $o(blendviewer) SetColorLevel [$displayNode GetLevel]
        $o(blendmag) SetMagnificationFactors 15 15 1
        $o(blendmag) SetInput [$o(cmpBlend) GetOutput]
        $o(blendviewer) SetInput [$o(blendmag) GetOutput]
        $o(blendviewer) Render
      }

      set weight [$this comparePatches $imagePatch [$o(cmpBlend) GetOutput]]
      if { $weight < $minWeight } {
        set minWeight $weight
        set minSample $samplePoint
        set minS $s
      }
    }


    eval $this addPoint $minSample
    incr i



  }
  $imagePatch Delete
}

itcl::body ModelDrawEffect::extractPatch { imagePatch point normal tangent sliceNormal } {
  if { ![info exists o(resliceCast)] } {
    set o(resliceCast) [vtkNew vtkImageCast]
    $o(resliceCast) SetOutputScalarTypeToFloat
    set o(reslice) [vtkNew vtkImageReslice]
    $o(reslice) SetInputConnection [$o(resliceCast) GetOutputPort]
    set o(resliceTransform) [vtkNew vtkTransform]
    set o(resliceMatrix) [vtkNew vtkMatrix4x4]
    set o(rasToIJKMatrix) [vtkNew vtkMatrix4x4]
    $o(reslice) SetResliceTransform $o(resliceTransform)
    $o(reslice) SetInterpolationModeToLinear
    $o(reslice) InterpolateOn
    $o(reslice) AutoCropOutputOff
  }

  set logic [[$sliceGUI GetLogic]  GetBackgroundLayer]
  set node [$logic GetVolumeNode]
  $o(resliceCast) SetInput [$node GetImageData]
  eval $o(reslice) SetOutputSpacing 1 1 1
  foreach {w h} $patchSize {}
  $o(reslice) SetOutputOrigin 0 0 0
  $o(reslice) SetOutputDimensionality 3
  $o(reslice) SetOutputExtent 0 [expr $w-1] 0 [expr $h-1] 0 0

  # goes from World space to input volume IJK space
  $node GetIJKToRASMatrix $o(rasToIJKMatrix)
  $o(rasToIJKMatrix) Invert

  # goes from output IJK space to World space
  $o(resliceMatrix) Identity
  foreach p $point n $normal t $tangent sn $sliceNormal row {0 1 2} {
    # offset point to the upper left corner of output
    set p [expr $p - 0.5 * $n * $h - 0.5 * $t * $w]
    $o(resliceMatrix) SetElement $row 0 $t
    $o(resliceMatrix) SetElement $row 1 $n
    $o(resliceMatrix) SetElement $row 2 $sn
    $o(resliceMatrix) SetElement $row 3 $p
  }
  # goes from output IJK to input IJK
  $o(resliceMatrix) Multiply4x4 $o(rasToIJKMatrix) $o(resliceMatrix) $o(resliceMatrix)

  $o(resliceTransform) SetMatrix $o(resliceMatrix)

  $o(reslice) SetOutput $imagePatch
  $o(reslice) UpdateWholeExtent

  if { $debugExtractPatch } {
    if { ![info exists o(viewermag)] } {
      set o(viewermag) [vtkNew vtkImageMagnify]
      set o(viewer) [vtkNew vtkImageViewer]
    }
    set displayNode [$node GetDisplayNode]
    $o(viewer) SetColorWindow [$displayNode GetWindow]
    $o(viewer) SetColorLevel [$displayNode GetLevel]
    $o(viewermag) SetInput $imagePatch
    $o(viewermag) SetMagnificationFactors 15 15 1
    $o(viewer) SetInput [$o(viewermag) GetOutput]
    $o(viewer) Render
  }
}

itcl::body ModelDrawEffect::comparePatches {p1 p2} {

  # set up the comparision pipeline
  if { ![info exists o(cmpDiff)] } {
    set o(cmpDiff) [vtkNew vtkImageMathematics]
    set o(cmpAbs) [vtkNew vtkImageMathematics]
    set o(cmpAccumulate) [vtkNew vtkImageAccumulate]
  }

  # calculate mean abs value of difference and return as weight
  $o(cmpDiff) SetInput1 $p1
  $o(cmpDiff) SetInput2 $p2
  $o(cmpDiff) SetOperationToSubtract
  $o(cmpAbs) SetInput1 [$o(cmpDiff) GetOutput]
  $o(cmpAbs) SetOperationToAbsoluteValue
  $o(cmpAccumulate) SetInput [$o(cmpAbs) GetOutput]
  $o(cmpAccumulate) Update
  set weight [lindex [$o(cmpAccumulate) GetMean] 0]
  return $weight

}

itcl::body ModelDrawEffect::buildOptions {} {


  #
  # Defined points frame
  #
  set o(definedPointsFrame) [vtkNew vtkKWFrame]
  $o(definedPointsFrame) SetParent [$this getOptionsFrame]
  $o(definedPointsFrame) Create
  pack [$o(definedPointsFrame) GetWidgetName] \
    -side top -anchor w -padx 2 -pady 0 -expand true -fill x

  set o(definedPointsLabel) [vtkNew vtkKWLabelWithLabel]
  $o(definedPointsLabel) SetParent $o(definedPointsFrame)
  $o(definedPointsLabel) Create
  $o(definedPointsLabel) SetLabelText "Defined: "
  $o(definedPointsLabel) SetBalloonHelpString "List of label numbers that have control points defined.\nUse the label selector above to select new labels."
  pack [$o(definedPointsLabel) GetWidgetName] \
    -side left -anchor w -padx 2 -pady 0

  set o(importPoints) [vtkNew vtkKWPushButton]
  $o(importPoints) SetParent $o(definedPointsFrame)
  $o(importPoints) Create
  $o(importPoints) SetText "Import..."
  $o(importPoints) SetBalloonHelpString "Import control points from another study."
  pack [$o(importPoints) GetWidgetName] \
    -side right -anchor e -padx 2 -pady 0
  set invokedEvent 10000
  $::slicer3::Broker AddObservation $o(importPoints) $invokedEvent "$this importDialog"

  #
  # option check boxes
  #

  set o(edgeTangents) [vtkNew vtkKWCheckButtonWithLabel]
  $o(edgeTangents) SetParent [$this getOptionsFrame]
  $o(edgeTangents) Create
  $o(edgeTangents) SetLabelText "Edge Tangents: "
  $o(edgeTangents) SetBalloonHelpString "Use calculate edge tangets when interpolating."
  [$o(edgeTangents) GetLabel] SetWidth 22
  [$o(edgeTangents) GetLabel] SetAnchorToEast
  pack [$o(edgeTangents) GetWidgetName] \
    -side top -anchor w -padx 2 -pady 2 
  [$o(edgeTangents) GetWidget] SetSelectedState $edgeTangents

  set SelectedStateChangedEvent 10000
  $::slicer3::Broker AddObservation [$o(edgeTangents) GetWidget] $SelectedStateChangedEvent "$this processEvent $o(edgeTangents) $SelectedStateChangedEvent"

  set o(spline) [vtkNew vtkKWCheckButtonWithLabel]
  $o(spline) SetParent [$this getOptionsFrame]
  $o(spline) Create
  $o(spline) SetLabelText "Spline: "
  $o(spline) SetBalloonHelpString "Use spline interpolation."
  [$o(spline) GetLabel] SetWidth 22
  [$o(spline) GetLabel] SetAnchorToEast
  pack [$o(spline) GetWidgetName] \
    -side top -anchor w -padx 2 -pady 2 
  if { $interpolation == "spline" } {
    [$o(spline) GetWidget] SetSelectedState 1
  } else {
    [$o(spline) GetWidget] SetSelectedState 0
  }

  set SelectedStateChangedEvent 10000
  $::slicer3::Broker AddObservation [$o(spline) GetWidget] $SelectedStateChangedEvent "$this processEvent $o(spline) $SelectedStateChangedEvent"

  set o(snap) [vtkNew vtkKWCheckButtonWithLabel]
  $o(snap) SetParent [$this getOptionsFrame]
  $o(snap) Create
  $o(snap) SetLabelText "Snap: "
  $o(snap) SetBalloonHelpString "Snap interpolated curve to follow gradient profile of control points."
  [$o(snap) GetLabel] SetWidth 22
  [$o(snap) GetLabel] SetAnchorToEast
  pack [$o(snap) GetWidgetName] \
    -side top -anchor w -padx 2 -pady 2 
  [$o(snap) GetWidget] SetSelectedState $snap

  set SelectedStateChangedEvent 10000
  $::slicer3::Broker AddObservation [$o(snap) GetWidget] $SelectedStateChangedEvent "$this processEvent $o(snap) $SelectedStateChangedEvent"

  # call superclass version of buildOptions
  chain
  
  # we use scope internally, but users shouldn't see it
  pack forget [$o(scopeOption) GetWidgetName]

  # set the help text (widget defined in DrawEffect superclass)
  $o(help) SetHelpText "Use this tool to draw interpolated outlines.  Red control points can be modified, Blue control points are automatically interpolated but can be converted to Red points by clicking on the slice.  Slices with outline but no control points are outside interpolation region.  Use Apply Curves to generate a label map.\n\nLeft Click: add control point.\nLeft Drag: move control point.\nShift-Left Drag: translate control points\nControl-Left Drag: scale control points\nControl-Shift-Left Drag: rotate control points."

  #
  # Apply label maps for all curves (interolate)
  #
  set o(applyCurves) [vtkNew vtkKWPushButton]
  $o(applyCurves) SetParent [$this getOptionsFrame]
  $o(applyCurves) Create
  $o(applyCurves) SetText "Apply Curves"
  $o(applyCurves) SetBalloonHelpString "Fill in all curves between start and end slices"
  $o(applyCurves) SetStateToDisabled
  pack [$o(applyCurves) GetWidgetName] \
    -side bottom -anchor w -padx 2 -pady 2 
  set invokedEvent 10000
  $::slicer3::Broker AddObservation $o(applyCurves) $invokedEvent "$this processEvent $o(applyCurves)"

  #
  # copy curves to all slices (interolate)
  #
  set o(copyCurves) [vtkNew vtkKWPushButton]
  $o(copyCurves) SetParent [$this getOptionsFrame]
  $o(copyCurves) Create
  $o(copyCurves) SetText "Copy Curves"
  $o(copyCurves) SetBalloonHelpString "Make curves for all interpolated slices"
  $o(copyCurves) SetStateToDisabled
  pack [$o(copyCurves) GetWidgetName] \
    -side bottom -anchor w -padx 2 -pady 2 
  set invokedEvent 10000
  $::slicer3::Broker AddObservation $o(copyCurves) $invokedEvent "$this processEvent $o(copyCurves)"

  #
  # an delete slice points button (active when on slice with control points)
  #
  set o(deleteCurve) [vtkNew vtkKWPushButton]
  $o(deleteCurve) SetParent [$this getOptionsFrame]
  $o(deleteCurve) Create
  $o(deleteCurve) SetText "Delete Points"
  $o(deleteCurve) SetBalloonHelpString "Delete the points in the current slice"
  $o(deleteCurve) SetStateToDisabled
  pack [$o(deleteCurve) GetWidgetName] \
    -side bottom -anchor w -padx 2 -pady 2 
  set invokedEvent 10000
  $::slicer3::Broker AddObservation $o(deleteCurve) $invokedEvent "$this processEvent $o(deleteCurve)"


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

  $this updateControlPoints
}

itcl::body ModelDrawEffect::updateGUIFromMRML { } {
  set _updatingGUI 1
  chain
  set _updatingGUI 0
}

itcl::body ModelDrawEffect::tearDownOptions { } {

  # call superclass version of tearDownOptions
  chain

  set widgets {spline snap edgeTangents curves deleteCurve applyCurves copyCurves definedPointsLabel importPoints definedPointsFrame }
  foreach w $widgets {
    if { [info exists o($w)] } {
      $o($w) SetParent ""
      pack forget [$o($w) GetWidgetName] 
    }
  }
}

itcl::body ModelDrawEffect::copyCurve { {promptForOverwrite "yes"} } {

  set offset [$this offset]
  if { [info exists _controlPoints($offset)] } {
    if { $promptForOverwrite == "yes" } {
      if { $_controlPoints($offset) != "" } {
        if { ![EditorConfirmDialog "This slice already has points defined - delete them?"] } {
          return
        }
      } else {
        return
      }
    }
  }

  # interpolate by default, but fall back to nearest
  # if needed or bail out if nothing can be done
  set from interpolated
  if { [llength [array names _controlPoints]] == 0 } {
    # no points to copy from - can't do anything
    return
  }
  if { [llength [array names _controlPoints]] == 1 } {
    # only one curve defined, copy it
    set from nearest
  }
  set sortedOffsets [lsort -real [array names _controlPoints]]
  if { $offset < [lindex $sortedOffsets 0] || $offset > [lindex $sortedOffsets end] } {
    # outside of interpolation zone, use nearest
    set from nearest
  }

  switch $from {
    "nearest" {
      set copyFrom ""
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
    }
    "interpolated" {
      set _controlPoints($offset) [$this interpolatedControlPoints]
    }
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
  set r [$_sliceSplineR Evaluate $t]
  set a [$_sliceSplineA Evaluate $t]
  set s [$_sliceSplineS Evaluate $t]

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

  # first, reset the splines
  if { $_sliceSplineR != "" } {
    $_sliceSplineR Delete
    $_sliceSplineA Delete
    $_sliceSplineS Delete
  }
  set _sliceSplineR [vtkKochanekSpline New]
  set _sliceSplineA [vtkKochanekSpline New]
  set _sliceSplineS [vtkKochanekSpline New]

  set eps 1e-4
  set interpolatedControlPoints ""
  for {set point 0} {$point < $pointCount} {incr point} {
    # set up the spline between slices for this point
    set index 0
    foreach offset $offsets {
      # add the control points
      set cp [lindex $_controlPoints($offset) $point]
      foreach {r a s} $cp {}
      $_sliceSplineR AddPoint $index $r
      $_sliceSplineA AddPoint $index $a
      $_sliceSplineS AddPoint $index $s
      incr index
    }
    # compute the spline
    foreach obj {splineR splineA splineS} {
      $o($obj) Compute
    }

    # perform the binary search
    set guesses 0
    set high [llength $offsets]
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
      if { [expr abs($high - $low)] < [expr $eps/1000.] } {
        # TODO: could extrapolate beyond current range if needed
        #puts "Bad guess local minimum dist: $guessDist, low,high: $low,$high"
        break
      }
      if { [expr $sliceDir * $guessDist] > 0 } {
        set high $guess
      } else {
        set low $guess
      }
      incr guesses
    }

    set r [$_sliceSplineR Evaluate $guess]
    set a [$_sliceSplineA Evaluate $guess]
    set s [$_sliceSplineS Evaluate $guess]
    lappend interpolatedControlPoints "$r $a $s"
  }
  return $interpolatedControlPoints
}

itcl::body ModelDrawEffect::hermiteWeights {} {
  if { ![info exists _hermiteWeights($splineSteps)] } {
    set deltaT [expr 1. / $splineSteps]
    set t 0.
    for {set step 0} {$step <= $splineSteps} {incr step} {
      # hermite interpolation functions
      set h00 [expr  2.*$t**3 - 3.*$t**2       + 1.]
      set h10 [expr     $t**3 - 2.*$t**2 + $t      ]
      set h01 [expr -2.*$t**3 + 3.*$t**2           ]
      set h11 [expr     $t**3 -    $t**2           ]
      lappend _hermiteWeights($splineSteps) "$h00 $h10 $h01 $h11"
      set t [expr $t + $deltaT]
    }
  }
  return $_hermiteWeights($splineSteps)
}


# import Dialog - pick scene with existing control points and grayscale
itcl::body ModelDrawEffect::importDialog {} {

  if { [array names _controlPoints] != "" } {
    # there are control points in this scene
    if { ![EditorConfirmDialog "Control points exist in this scene.\nReplace them with import?"] } {
      return
    }
  }

  if { ![info exists o(importTopLevel)] } {
    set o(importTopLevel) [vtkNew vtkKWTopLevel]
    $o(importTopLevel) SetApplication $::slicer3::Application
    $o(importTopLevel) ModalOn
    $o(importTopLevel) Create
    $o(importTopLevel) SetMasterWindow [$::slicer3::ApplicationGUI GetMainSlicerWindow]
    $o(importTopLevel) SetDisplayPositionToPointer
    $o(importTopLevel) HideDecorationOff
    $o(importTopLevel) Withdraw
    $o(importTopLevel) SetBorderWidth 2
    $o(importTopLevel) SetReliefToGroove

    set topFrame [vtkNew vtkKWFrame]
    $topFrame SetParent $o(importTopLevel)
    $topFrame Create
    pack [$topFrame GetWidgetName] -side top -anchor w -padx 2 -pady 2 -fill both -expand true

    set o(labelPromptLabel) [vtkNew vtkKWLabel]
    $o(labelPromptLabel) SetParent $topFrame
    $o(labelPromptLabel) Create
    $o(labelPromptLabel) SetText "Select a mrml scene containing control points to import.\nIf the scene contains a volume, optionally register it to define a transform for the control points."
    pack [$o(labelPromptLabel) GetWidgetName] -side top -anchor w -padx 2 -pady 2 -fill both -expand true

    set o(sceneSelect) [vtkNew vtkKWLoadSaveButtonWithLabel]
    $o(sceneSelect) SetParent $topFrame
    $o(sceneSelect) Create
    $o(sceneSelect) SetLabelText "Scene file to import:"
    $o(sceneSelect) SetBalloonHelpString "Select a .mrml scene containing model draw control points and a grayscale volume for registration."
    [$o(sceneSelect) GetWidget] TrimPathFromFileNameOff
    set loadSaveDialog [[$o(sceneSelect) GetWidget] GetLoadSaveDialog]
    $loadSaveDialog ChooseDirectoryOff
    $loadSaveDialog SaveDialogOff
    $loadSaveDialog SetTitle "Select Scene to Import"
    $loadSaveDialog SetFileTypes "{ {MRML Scene} {.mrml} }"
    $loadSaveDialog RetrieveLastPathFromRegistry "ModelDrawImportScene"
    pack [$o(sceneSelect) GetWidgetName] -side top -fill x -expand true

    set o(importRegister) [vtkNew vtkKWCheckButtonWithLabel]
    $o(importRegister) SetParent $topFrame
    $o(importRegister) Create
    $o(importRegister) SetLabelText "Register Volume: "
    $o(importRegister) SetBalloonHelpString "Register volume from selected scene to calculate the transform to use with the imported control points."
    [$o(importRegister) GetLabel] SetWidth 22
    [$o(importRegister) GetLabel] SetAnchorToEast
    pack [$o(importRegister) GetWidgetName] \
      -side top -anchor w -padx 2 -pady 2 
    [$o(importRegister) GetWidget] SetSelectedState 1

    set buttonFrame [vtkNew vtkKWFrame]
    $buttonFrame SetParent $topFrame
    $buttonFrame Create
    pack [$buttonFrame GetWidgetName] -side left -anchor w -padx 2 -pady 2 -fill both -expand true

    set o(importOK) [vtkNew vtkKWPushButton]
    $o(importOK) SetParent $buttonFrame
    $o(importOK) Create
    $o(importOK) SetText OK
    set o(importCancel) [vtkNew vtkKWPushButton]
    $o(importCancel) SetParent $buttonFrame
    $o(importCancel) Create
    $o(importCancel) SetText Cancel
    pack [$o(importCancel) GetWidgetName] [$o(importOK) GetWidgetName] -side left -padx 4 -anchor c 

    # invoked event
    set broker $::slicer3::Broker
    $broker AddObservation $o(importOK) 10000 "$this importCallback; $o(importTopLevel) Withdraw"
    $broker AddObservation $o(importCancel) 10000 "$o(importTopLevel) Withdraw"
  }

  $o(importTopLevel) DeIconify
  $o(importTopLevel) Raise
}

itcl::body ModelDrawEffect::importCallback {} {
  set loadSaveDialog [[$o(sceneSelect) GetWidget] GetLoadSaveDialog]
  $loadSaveDialog SaveLastPathToRegistry "ModelDrawImportScene"

  #
  # load the import scene
  # - sanity check import scene (for control points and volume)
  # - if needed find the scalar volume and register it
  # - transform the control points from the import scene
  # - adjust the control points to lie on offsets
  #

  # create the new scene
  set scene [vtkMRMLScene New]
  set tag [$scene GetTagByClassName "vtkMRMLScriptedModuleNode"]
  if { $tag == "" } {
    set node [vtkMRMLScriptedModuleNode New]
    $scene RegisterNodeClass $node
    $node Delete
  }

  set sceneFile [[$o(sceneSelect) GetWidget] GetFileName]
  $scene SetURL $sceneFile
  $scene Connect

  set modelDrawNode ""
  set number [$scene GetNumberOfNodesByClass vtkMRMLScriptedModuleNode]
  for {set n 0} {$n < $number} {incr n} {
    set node [$scene GetNthNodeByClass $n vtkMRMLScriptedModuleNode]
    if { [$node GetModuleName] == "ModelDraw" } {
      set modelDrawNode $node
    }
  }

  if { $modelDrawNode == "" } {
    EditorErrorDialog "No model draw information in $sceneFile"
    return
  }

  if { [[$o(importRegister) GetWidget] GetSelectedState] } {
    # perform the registration
    # - get the scalar volume
    # - run the registration
    set importVolume ""
    set number [$scene GetNumberOfNodesByClass vtkMRMLScalarVolumeNode]
    for {set n 0} {$n < $number} {incr n} {
      set node [$scene GetNthNodeByClass $n vtkMRMLScalarVolumeNode]
      if { ![$node GetLabelMap] } {
        set importVolume $node
      }
    }
    set movingNode ""
    if { $importVolume == "" } {
      EditorErrorDialog "No scalar volume in $sceneFile\nCannot register"
    } else {
      set storage [$importVolume GetStorageNode]
      set path [$storage GetFileName]
    
      set volumeLogic [$::slicer3::VolumesGUI GetLogic]
      set ret [catch [list $volumeLogic AddArchetypeVolume "$path" "ModelDrawReference"] movingNode]
      if { $ret } {
        EditorErrorDialog "Could not import $path\nCannot register"
      } 
    }
    if { $movingNode != "" } {
      #
      # perform the registration
      # - create node
      # - apply the task
      # - wait for result
      #
      set registrationModule ""
      foreach gui [vtkCommandLineModuleGUI ListInstances] {
        if { [$gui GetGUIName] == "Fast Affine registration" } {
          set registrationModule $gui
        }
      }
      if { $registrationModule == "" } {
        EditorErrorDialog "Could find registration module\nCannot register"
      } else {
        # the module parameter node
        set moduleNode [vtkMRMLCommandLineModuleNode New]
        $::slicer3::MRMLScene AddNode $moduleNode
        $moduleNode SetName "Model Draw Import Registration"
        $moduleNode SetModuleDescription "Fast Affine registration"
        $registrationModule Enter
        # fixed (our edit volume) and moving (the imported volume)
        set logic [[$sliceGUI GetLogic]  GetBackgroundLayer]
        set node [$logic GetVolumeNode]
        $moduleNode SetParameterAsString "FixedImageFileName" [$node GetID]
        $moduleNode SetParameterAsString "MovingImageFileName" [$movingNode GetID]
        # the transform node
        set transformNode [vtkMRMLLinearTransformNode New]
        $transformNode SetName "ReferenceToSubject"
        $::slicer3::MRMLScene AddNode $transformNode
        $moduleNode SetParameterAsString "OutputTransform" [$transformNode GetID]

        $registrationModule SetCommandLineModuleNode $moduleNode
        [$registrationModule GetLogic] SetCommandLineModuleNode $moduleNode
        [$registrationModule GetLogic] LazyEvaluateModuleTarget $moduleNode
        [$registrationModule GetLogic] Apply $moduleNode

        set waitWindow [vtkNew vtkKWTopLevel]
        $waitWindow SetApplication $::slicer3::Application
        $waitWindow ModalOn
        $waitWindow Create
        $waitWindow SetMasterWindow [$::slicer3::ApplicationGUI GetMainSlicerWindow]
        $waitWindow HideDecorationOff
        $waitWindow SetBorderWidth 2
        $waitWindow SetReliefToGroove

        set waitLabel [vtkNew vtkKWLabel]
        $waitLabel SetParent $waitWindow
        $waitLabel Create
        $waitLabel SetText "Registration in process.  Please wait..."
        pack [$waitLabel GetWidgetName] -side top -anchor w -padx 2 -pady 2 -fill both -expand true
        $waitWindow DeIconify
        $waitWindow Raise
        set seconds 0
        update
        while { [$moduleNode GetStatusString] != "Completed" } {
          $waitLabel SetText "Registration in process ($seconds sec).  Please wait..."
          incr seconds
          update
          after 1000
        }

        while { [$::slicer3::ApplicationLogic GetReadDataQueueSize] } {
            $waitLabel SetText "Waiting for data to be read...queue size = [$::slicer3::ApplicationLogic GetReadDataQueueSize]"
            update
            after 1000
        }


        # Now transform the control points by the registration matrix
        # - and figure out a valid offset for them
        # - then snap each new control point on to the slice plane at offset
        $waitLabel SetText "Transforming Control Points..."
        set sliceToRAS [$_sliceNode GetSliceToRAS]
        foreach row {0 1 2} {
          lappend sliceNormal [$sliceToRAS GetElement $row 2]
        }
        set sliceNormal [$this normalize $sliceNormal]
        set matrix [$transformNode GetMatrixTransformToParent]
        $modelDrawNode RequestParameterList
        array set p [$modelDrawNode GetParameterList]
        foreach index [array names p] {
          # make a new set of control points per label value
          array unset newOffsetCPArray
          foreach {offset, cps} $p($index) {
            # transform points by registration matrix
            set transformedCPs ""
            foreach cp $cps {
              lappend transformedCPs [lrange [eval $matrix MultiplyPoint $cp 1] 0 2]
            }
            # determine best offset value for this group of points
            set cent [$this centroid $transformedCPs]
            eval $_sliceNode JumpSlice $cent
            if {[catch {[$sliceGUI GetLogic] SnapSliceOffsetToIJK}]} {
              puts "No SnapSliceOffsetToIJK in this version of slicer"
            }
            set newOffset [$this offset]
            set newCPs ""
            foreach newCP $transformedCPs {
              # snap the points to the offset plane
              set cpToSlice ""
              foreach cpEle $newCP row {0 1 2} {
                # calculate vector from cp to slice point
                lappend cpToSlice [expr [$sliceToRAS GetElement $row 3] - $cpEle]
              }
              set toPlane [$this dot $sliceNormal $cpToSlice]
              set cpOnSlice ""
              foreach cpEle $newCP snEle $sliceNormal {
                # add the portion of cpToSlice that is along the slice normal
                lappend cpOnSlice [expr $cpEle + $toPlane * $snEle]
              }
              lappend newCPs $cpOnSlice
            }
            # update array of control points for this slice offset
            set newOffsetCPArray($newOffset) $newCPs
          }
          # set parameter for this label value
          $modelDrawNode SetParameter $index [array get newOffsetCPArray]
        }
        $transformNode Delete
        $moduleNode Delete
        $waitLabel Delete
        $waitWindow Delete
      }
    }
  }

  # put new points, imported and possibly registered, into scene
  $_modelDrawNode Copy $modelDrawNode

  array set _controlPoints [$_modelDrawNode GetParameter $_currentLabel]
  $this updateControlPoints

  $scene Delete
}

