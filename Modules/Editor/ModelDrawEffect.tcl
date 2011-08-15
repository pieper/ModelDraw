
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
    variable _moveStartRAS ""
    variable _moveStartControlPoints ""
    variable _controlPoints ;# array - points for current label, indexed by offset
    variable _curves ;# array - interpolated for current points, indexed by offset
    variable _updatingControlPoints 0
    variable _currentLabel ""
    variable _sliceSplineR "" ;# local storage for dynamicly created temp splines
    variable _sliceSplineA ""
    variable _sliceSplineS ""
    variable _modelDrawNode "" ;# custom parameter node for this tool

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
    method addControlPoint {r a s} {}
    method deleteControlPoint {index} {}
    method splitControlPoint {index} {}
    method updateControlPoints {} {}
    method updateCurve { {controlPoints ""} } {}
    method applyCurve {} {}
    method applyCurves {} {}
    method seedStartMovingCallback {seed index} {}
    method seedMovingCallback {seed index} {}
    method seedContextMenuCallback {seed index} {}
    method seedKeyCallback {seed index key} {}
    method copyCurve {} {}
    method interpolatedControlPoints {} {}
    method splineToSlice {t} {}
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
  
  set rSum 0
  set aSum 0
  set sSum 0
  foreach cp $cps {
    foreach {r a s} $cp {}
    set rSum [expr $rSum + $r]
    set aSum [expr $aSum + $a]
    set sSum [expr $sSum + $s]
  }
  set rCentroid [expr $rSum / [llength $cps]]
  set aCentroid [expr $aSum / [llength $cps]]
  set sCentroid [expr $sSum / [llength $cps]]
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


  # enable the apply curves option if more than one slice is defined
  if { [info exists o(applyCurves)] } {
    if { [llength [array names _controlPoints]] > 1 } {
      $o(applyCurves) SetStateToNormal
    } else {
      $o(applyCurves) SetStateToDisabled
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

itcl::body ModelDrawEffect::applyCurves {} {
  # rasterize the current set of curves to all slices in the
  # volume between high and low offsets
 
  #
  # find the sliceSWidget to use for increment
  # find min/max offset for iteration
  # update to current real or interpolated curve
  # apply draw operation to label map
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
    $this applyCurve
    update
  }

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
        set steps [expr $index * 10]
        set t 0
        for {set step 0} {$step <= $steps} {incr step} {
          set r [$o(splineR) Evaluate $t]
          set a [$o(splineA) Evaluate $t]
          set s [$o(splineS) Evaluate $t]
          $this addPoint $r $a $s
          lappend _curves($offset) "$r $a $s"
          set t [expr $t + 0.1]
        }
      }
    }
  }

  $this positionActors
  $this positionCursor
  [$sliceGUI GetSliceViewer] RequestRender
}

itcl::body ModelDrawEffect::buildOptions {} {

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

  foreach w "spline curves deleteCurve applyCurves" {
    if { [info exists o($w)] } {
      $o($w) SetParent ""
      pack forget [$o($w) GetWidgetName] 
    }
  }
}

itcl::body ModelDrawEffect::copyCurve {} {

  set offset [$this offset]
  if { [info exists _controlPoints($offset)] } {
    if { $_controlPoints($offset) != "" } {
      if { ![EditorConfirmDialog "This slice already has points defined - delete them?"] } {
        return
      }
    }
  }

  # interpolate by default, but fall back to nearest
  # if needed or bail out if nothing can be done
  set from interpolated
  if { [array names _controlPoints] == 0 } {
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
