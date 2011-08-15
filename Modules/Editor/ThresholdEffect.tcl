
package require Itcl

#########################################################
#
if {0} { ;# comment

  ThresholdEffect an editor effect


# TODO : 

}
#
#########################################################

#
#########################################################
# ------------------------------------------------------------------
#                             ThresholdEffect
# ------------------------------------------------------------------
#
# The class definition - define if needed (not when re-sourcing)
#
if { [itcl::find class ThresholdEffect] == "" } {

  itcl::class ThresholdEffect {

    inherit EffectSWidget

    constructor {sliceGUI} {EffectSWidget::constructor $sliceGUI} {}
    destructor {}

    public variable range ""
    public variable label "1"

    # methods
    method processEvent {{caller ""} {event ""}} {}
    method preview {} {}
    method apply {} {}
    method positionCursor {} {}
    method buildOptions {} {}
    method tearDownOptions {} {}
    method previewOptions {} {}
    method setAnimationState { p } {}
    method applyOptions {} {}
    method applyInSegmented {} {}
    method setPaintThreshold {} {}
    method labelSelectDialog {} {}
  }
}

# ------------------------------------------------------------------
#                        CONSTRUCTOR/DESTRUCTOR
# ------------------------------------------------------------------
itcl::body ThresholdEffect::constructor {sliceGUI} {
  set _scopeOptions "all visible"
}

itcl::body ThresholdEffect::destructor {} {
  # rely on superclass destructor
}

# ------------------------------------------------------------------
#                             METHODS
# ------------------------------------------------------------------

itcl::body ThresholdEffect::processEvent { {caller ""} {event ""} } {

  if { [$this preProcessEvent $caller $event] } {
    # superclass processed the event, so we don't
    return
  }

}

itcl::body ThresholdEffect::apply {} {

  if { [$this getInputBackground] == "" || [$this getInputLabel] == "" } {
    $this errorDialog "Background and Label map needed for Threshold"
    return
  }

  foreach b {apply useForPainting cancel} {
    $o($b) SetStateToDisabled
  }

  set thresh [vtkImageThreshold New]
  $thresh SetInput [$this getInputBackground]
  eval $thresh ThresholdBetween $range
  $thresh SetInValue [EditorGetPaintLabel]
  $thresh SetOutValue 0
  $thresh SetOutput [$this getOutputLabel]
  $thresh SetOutputScalarType [[$this getInputLabel] GetScalarType]
  $this setProgressFilter $thresh "Threshold"
  $thresh Update
  $thresh Delete

  $this postApply
}


itcl::body ThresholdEffect::preview {} {

  if { [$this getInputBackground] == "" || [$this getInputLabel] == "" } {
    $this errorDialog "Background and Label map needed for Threshold"
    return
  }

  #
  # make a lookup table where inside the threshold is opaque and colored
  # by the label color, while the background is transparent (black)
  # - apply the threshold operation to the currently visible background
  #   (output of the layer logic's vtkImageReslice instance)
  #

  set color [::EditorGetPaintColor $::Editor(singleton)]
  if { ![info exists o(lut)] } {
    set o(lut) [vtkNew vtkLookupTable]
  }
  $o(lut) SetRampToLinear
  $o(lut) SetNumberOfTableValues 2
  $o(lut) SetTableRange 0 1
  $o(lut) SetTableValue 0  0 0 0  0
  eval $o(lut) SetTableValue 1  $color
  set map [vtkImageMapToRGBA New]
  $map SetOutputFormatToRGBA
  $map SetLookupTable $o(lut)

  set thresh [vtkImageThreshold New]
  set logic [[$sliceGUI GetLogic] GetBackgroundLayer]
  $thresh SetInput [[$logic GetReslice] GetOutput]
  eval $thresh ThresholdBetween $range
  $thresh SetInValue 1
  $thresh SetOutValue 0
  $thresh SetOutputScalarTypeToUnsignedChar
  $map SetInput [$thresh GetOutput]

  $map Update

  $o(cursorMapper) SetInput [$map GetOutput]
  $o(cursorActor) VisibilityOn

  $map Delete
  $thresh Delete

  [$sliceGUI GetSliceViewer] RequestRender
}

#
# p will be a floating point number 
#
itcl::body ThresholdEffect::setAnimationState { p } {

  if { [info exists o(lut)] } {
    set amt [expr 0.5 + 0.25 * (1 + cos(6.2831852 * ($p - floor($p)))) ]
    set color [::EditorGetPaintColor $::Editor(singleton)]
    eval $o(lut) SetTableValue 1 [lreplace $color 3 3 $amt]
  }
}

itcl::body ThresholdEffect::positionCursor {} {
  foreach actor $_cursorActors {
    $actor SetPosition 0 0
  }
}

itcl::body ThresholdEffect::buildOptions { } {

  chain
  
  #
  # a range setting for threshold values
  #
  set o(range) [vtkNew vtkKWRange]
  $o(range) SetParent [$this getOptionsFrame]
  $o(range) Create
  $o(range) SetLabelText "Range"
  $o(range) SetReliefToGroove
  $o(range) SetBalloonHelpString "Set the range of the background values that should be labeled."

  if { [$this getInputBackground] != "" } {
    set range [[$this getInputBackground] GetScalarRange]
    foreach {lo hi} $range {}
    set lo [expr $lo - 1]
    set hi [expr $hi + 1]
    set range "$lo $hi"
    eval $o(range) SetWholeRange $range
    if { [EditorGetPaintThresholdState] } {
      foreach {lo hi} [EditorGetPaintThreshold] {}
      $o(range) SetRange $lo $hi
    } else {
      foreach {lo hi} $range {}
      set lo [expr $lo + (0.25 * ($hi - $lo))]
      $o(range) SetRange $lo $hi
    }
  }

  pack [$o(range) GetWidgetName] -side top -anchor e -fill x -padx 2 -pady 2 

  #
  # an apply in segmented
  #
  set o(applyInSegmented) [vtkNew vtkKWPushButton]
  $o(applyInSegmented) SetParent [$this getOptionsFrame]
  $o(applyInSegmented) Create
  $o(applyInSegmented) SetText "Apply In Segmented..."
  $o(applyInSegmented) SetBalloonHelpString "Create a new label map where this threshold is applied to each label value of the current label map."
  pack [$o(applyInSegmented) GetWidgetName] \
    -side bottom -anchor e -padx 2 -pady 2 

  #
  # buttons frame
  #
  set o(buttonsFrame) [vtkNew vtkKWFrame]
  $o(buttonsFrame) SetParent [$this getOptionsFrame]
  $o(buttonsFrame) Create
  $o(buttonsFrame) SetBorderWidth 0
  $o(buttonsFrame) SetReliefToFlat
  pack [$o(buttonsFrame) GetWidgetName] \
    -side bottom -anchor e -padx 2 -pady 2 


  #
  # a cancel button
  #
  set o(cancel) [vtkNew vtkKWPushButton]
  $o(cancel) SetParent $o(buttonsFrame)
  $o(cancel) Create
  $o(cancel) SetText "Cancel"
  $o(cancel) SetBalloonHelpString "Cancel threshold without applying to label map."
  pack [$o(cancel) GetWidgetName] \
    -side right -anchor e -padx 2 -pady 2 

  #
  # use for painting button
  #
  set o(useForPainting) [vtkNew vtkKWPushButton]
  $o(useForPainting) SetParent $o(buttonsFrame)
  $o(useForPainting) Create
  $o(useForPainting) SetText "Use For Paint"
  $o(useForPainting) SetBalloonHelpString "Transfer the current threshold settings to be used for Paint and Draw operations."
  pack [$o(useForPainting) GetWidgetName] \
    -side right -anchor e -padx 2 -pady 2 

  #
  # an apply button
  #
  set o(apply) [vtkNew vtkKWPushButton]
  $o(apply) SetParent $o(buttonsFrame)
  $o(apply) Create
  $o(apply) SetText "Apply"
  $o(apply) SetBalloonHelpString "Apply current threshold settings to the label map."
  pack [$o(apply) GetWidgetName] \
    -side right -anchor e -padx 2 -pady 2 


  if { [$this getInputBackground] == "" || [$this getInputLabel] == "" } {
    $this errorDialog "Need to have background and label layers to use threshold"
    after idle ::EffectSWidget::RemoveAll
    return
  }

  #
  # call update before adding observers to avoid triggering preview
  # updates during packing and configuring
  #
  $::slicer3::Application ProcessPendingEvents

  #
  # event observers - TODO: if there were a way to make these more specific, I would...
  #
  set tag [$o(range) AddObserver AnyEvent "after idle $this previewOptions"]
  lappend _observerRecords "$o(range) $tag"
  set tag [$o(apply) AddObserver AnyEvent "$this applyOptions; after idle ::EffectSWidget::RemoveAll"]
  lappend _observerRecords "$o(apply) $tag"
  set tag [$o(useForPainting) AddObserver AnyEvent "$this setPaintThreshold"]
  lappend _observerRecords "$o(useForPainting) $tag"
  set tag [$o(cancel) AddObserver AnyEvent "after idle ::EffectSWidget::RemoveAll"]
  lappend _observerRecords "$o(cancel) $tag"
  set tag [$o(applyInSegmented) AddObserver AnyEvent "$this labelSelectDialog"]
  lappend _observerRecords "$o(applyInSegmented) $tag"

}

itcl::body ThresholdEffect::tearDownOptions { } {
  chain
  if { [info exists o(range)] } {
    foreach w "range buttonsFrame apply useForPainting cancel applyInSegmented" {
      if { [info exists o($w)] } {
        $o($w) SetParent ""
        pack forget [$o($w) GetWidgetName] 
      }
    }
  }
}

itcl::body ThresholdEffect::previewOptions { } {
  foreach te [itcl::find objects -class ThresholdEffect] {
    $te configure -range [$o(range) GetRange]
    $te animateCursor off
    $te preview
    $te animateCursor on
  }
}

itcl::body ThresholdEffect::applyOptions { } {
  $this previewOptions
  set te [lindex [itcl::find objects -class ThresholdEffect] 0]
  $te apply
}

itcl::body ThresholdEffect::setPaintThreshold {} {

  eval EditorSetPaintThreshold [$o(range) GetRange]
  EditorSetPaintThresholdState 1
}

itcl::body ThresholdEffect::applyInSegmented { } {
  # create a new label map that has one thresholded region for each
  # of the unique label values in the source label map
  $this labelSelectDialog
  set sourceLabels [$o(labelMapSelector) GetSelected]

  #
  # first, make the binary thresholded image (over the whole volume)
  #
  set thresh [vtkImageThreshold New]
  $thresh SetInput [$this getInputBackground]
  eval $thresh ThresholdBetween $range
  $thresh SetInValue 1
  $thresh SetOutValue 0
  $thresh SetOutputScalarType [[$sourceLabels GetImageData] GetScalarType]
  $thresh Update

  #
  # now, make a combinedImage that has the threhold masked
  # for each of the distinct labels
  #
  set accum [vtkImageAccumulate New]
  $accum SetInput [$sourceLabels GetImageData]
  $accum Update
  set lo [expr int([lindex [$accum GetMin] 0])]
  set hi [expr int([lindex [$accum GetMax] 0])]
  $accum Delete

  set labelExtract [vtkImageThreshold New]
  set logic [vtkImageLogic New]
  $logic SetOperationToAnd
  $logic SetOutputTrueValue 1
  set changer [vtkImageLabelChange New]
  $changer SetInput [$logic GetOutput]
  set combiner [vtkImageLabelCombine New]
  set combinedImage ""
  for {set i $lo} {$i <= $hi} {incr i} {
    if { $i == 0 } { 
      # skip the background
      continue 
    }
    $labelExtract SetInput [$sourceLabels GetImageData]
    $labelExtract SetInValue 1
    $labelExtract SetOutValue 0
    $labelExtract ReplaceInOn
    $labelExtract ReplaceOutOn
    $labelExtract ThresholdBetween $i $i
    $labelExtract SetOutputScalarType [[$sourceLabels GetImageData] GetScalarType]
    $labelExtract Update
    if { [[$labelExtract GetOutput] GetScalarRange] != " 0.0 0.0" } {
      # AND of binary threshold and binary extracted label region
      $logic SetInput1 [$thresh GetOutput]
      $logic SetInput2 [$labelExtract GetOutput]
      # convert result of AND back to label value
      $changer SetInputLabel 1
      $changer SetOutputLabel $i
      $changer Update

      # now combine it
      if { $combinedImage == "" } {
        set combinedImage [vtkImageData New]
        $combinedImage DeepCopy [$changer GetOutput]
      } else {
        $combiner SetInput1 $combinedImage
        $combiner SetInput2 [$changer GetOutput]
        $combiner Update
        $combinedImage DeepCopy [$combiner GetOutput]
      }
    }
  }

  # put the result in this label map
  [$this getOutputLabel] DeepCopy $combinedImage
  $this postApply

  $labelExtract Delete
  $thresh Delete
  $changer Delete
  $combiner Delete
  $combinedImage Delete

  after idle ::EffectSWidget::RemoveAll
}

# Label Select Dialog - prompt to pick a label map for applyInSegmented
itcl::body ThresholdEffect::labelSelectDialog { } {

  if { ![info exists o(labelSelectTopLevel)] } {
    set o(labelSelectTopLevel) [vtkNew vtkKWTopLevel]
    $o(labelSelectTopLevel) SetApplication $::slicer3::Application
    $o(labelSelectTopLevel) ModalOn
    $o(labelSelectTopLevel) Create
    $o(labelSelectTopLevel) SetMasterWindow [$::slicer3::ApplicationGUI GetMainSlicerWindow]
    $o(labelSelectTopLevel) SetDisplayPositionToPointer
    $o(labelSelectTopLevel) HideDecorationOff
    $o(labelSelectTopLevel) Withdraw
    $o(labelSelectTopLevel) SetBorderWidth 2
    $o(labelSelectTopLevel) SetReliefToGroove

    set topFrame [vtkNew vtkKWFrame]
    $topFrame SetParent $o(labelSelectTopLevel)
    $topFrame Create
    pack [$topFrame GetWidgetName] -side top -anchor w -padx 2 -pady 2 -fill both -expand true

    set o(labelPromptLabel) [vtkNew vtkKWLabel]
    $o(labelPromptLabel) SetParent $topFrame
    $o(labelPromptLabel) Create
    pack [$o(labelPromptLabel) GetWidgetName] -side top -anchor w -padx 2 -pady 2 -fill both -expand true

    set o(labelMapSelector) [vtkNew vtkSlicerNodeSelectorWidget]
    $o(labelMapSelector) SetParent $topFrame
    $o(labelMapSelector) Create
    $o(labelMapSelector) SetNodeClass "vtkMRMLScalarVolumeNode" "LabelMap" "1" ""
    $o(labelMapSelector) NewNodeEnabledOff
    $o(labelMapSelector) NoneEnabledOff
    $o(labelMapSelector) DefaultEnabledOn
    $o(labelMapSelector) ShowHiddenOn
    $o(labelMapSelector) ChildClassesEnabledOff
    $o(labelMapSelector) SetMRMLScene $::slicer3::MRMLScene
    $o(labelMapSelector) UpdateMenu
    $o(labelMapSelector) SetLabelText "Label Map:"
    $o(labelMapSelector) SetBalloonHelpString "Pick the label map to use as the source for thresholding.\nNote that all non-zero label values will be used."
    pack [$o(labelMapSelector) GetWidgetName] -side top -fill x -expand true

    set buttonFrame [vtkNew vtkKWFrame]
    $buttonFrame SetParent $topFrame
    $buttonFrame Create
    pack [$buttonFrame GetWidgetName] -side left -anchor w -padx 2 -pady 2 -fill both -expand true

    set o(labelDialogApply) [vtkNew vtkKWPushButton]
    $o(labelDialogApply) SetParent $buttonFrame
    $o(labelDialogApply) Create
    $o(labelDialogApply) SetText Apply
    set o(labelDialogCancel) [vtkNew vtkKWPushButton]
    $o(labelDialogCancel) SetParent $buttonFrame
    $o(labelDialogCancel) Create
    $o(labelDialogCancel) SetText Cancel
    pack [$o(labelDialogCancel) GetWidgetName] [$o(labelDialogApply) GetWidgetName] -side left -padx 4 -anchor c 

    # invoked event
    set broker $::slicer3::Broker
    $broker AddObservation $o(labelDialogApply) 10000 "$this applyInSegmented; $o(labelSelectTopLevel) Withdraw"
    $broker AddObservation $o(labelDialogCancel) 10000 "$o(labelSelectTopLevel) Withdraw"
  }

  $o(labelPromptLabel) SetText "Select existing label map to define threshold regions."

  $o(labelSelectTopLevel) DeIconify
  $o(labelSelectTopLevel) Raise
}
