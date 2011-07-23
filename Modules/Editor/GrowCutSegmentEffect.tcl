
package require Itcl

#########################################################
#
if {0} { ;# comment

   GrowCutSegmentEffect an editor effect
# TODO : 

}

#
#########################################################

#
#########################################################
# ------------------------------------------------------------------
#                             GrowCutSegmentEffect
# ------------------------------------------------------------------
#
# The class definition - define if needed (not when re-sourcing)
#
if { [itcl::find class GrowCutSegmentEffect] == "" } {

    itcl::class GrowCutSegmentEffect {

  inherit PaintEffect 

  constructor {sliceGUI} {PaintEffect::constructor $sliceGUI} {}
  destructor {}
  
  public variable objectSize 5
  public variable contrastNoiseRatio 0.8
  public variable priorStrength 0.003
  public variable segmented 2
#  public variable radius1 0
#  public variable radius2 0
#  public variable radius3 0
#  public variable radius4 0
#  public variable radius5 0
  

  variable _segmentedImage ""
  variable _gestureImage ""
  variable _foregroundID ""
  variable _labelID ""
  variable _foundPainted 0
  variable _maxObjectSize 0
  
  variable _changedImage 0
  variable _iterations 0

  variable _imageDimension ""
  variable _imageSpacing ""
  variable _activeColoring 0

  method processEvent {{caller ""} {event ""} } {}
  method buildOptions {} {}
  method tearDownOptions {} {}
  method setMRMLDefaults {} {}
  method updateMRMLFromGUI {} {}
  method updateGUIFromMRML {} {}
  method apply {} {} 
  method initialize {} {}
  method removeApply {} {}
  method updateRadius1 {} {}
  method updateRadius2 {} {}
  method updateRadius3 {} {}
  method updateRadius4 {} {}
  method updateRadius5 {} {}
}
}

# ------------------------------------------------------------------
#                        CONSTRUCTOR/DESTRUCTOR
# ------------------------------------------------------------------
itcl::body GrowCutSegmentEffect::constructor {sliceGUI} {
    
   $this configure -sliceGUI $sliceGUI

   puts "GrowCutSegmentEffect"

   set _segmentedImage [vtkNew vtkImageData]
   set _gestureImage [vtkNew vtkImageData]

   set _imageDimension [[$this getInputLabel] GetDimensions]
   
   set _imageSpacing [[$this getInputLabel] GetSpacing]

   set dim [[$this getInputLabel] GetDimensions]
   set x [lindex $dim 0]
   set y [lindex $dim 1]
   set z [lindex $dim 2]
   
   set _maxObjectSize [expr $x * [expr $y * $z]]
   puts "max object size $_maxObjectSize $x $y $z"

   $this initialize
  
}



itcl::body GrowCutSegmentEffect::destructor {} {

  if { [info command $sliceGUI] != "" } {
    foreach tag $_guiObserverTags {
      $sliceGUI RemoveObserver $tag
    }
  }

  if { [info command $_sliceNode] != "" } {
    foreach tag $_nodeObserverTags {
      $_sliceNode RemoveObserver $tag
    }
  }

  if { [info command $_renderer] != "" } {
    foreach a $_actors {
      $_renderer RemoveActor2D $a
    }
  }
  
  $_segmentedImage Delete
  
  $_gestureImage Delete

}


# ------------------------------------------------------------------
#                             METHODS
# ------------------------------------------------------------------

itcl::body GrowCutSegmentEffect::initialize {} {
    
    set grabID [$sliceGUI GetGrabID]
    if { ($grabID != "") && ($grabID != $this) } {
      return
    }

    set numCnodes [$::slicer3::MRMLScene GetNumberOfNodesByClass "vtkMRMLSliceCompositeNode"]
    set j 0
    set cnode [$::slicer3::MRMLScene GetNthNodeByClass $j "vtkMRMLSliceCompositeNode"]
    set _foregroundID [$cnode GetForegroundVolumeID]
    set _labelID [$cnode GetLabelVolumeID]

    

    set dim [[$this getInputLabel] GetDimensions]
    set x [lindex $dim 0]
    set y [lindex $dim 1]
    set z [lindex $dim 2]

    set extent [[$this getInputLabel] GetExtent]
    set origin [[$this getInputLabel] GetOrigin]
    set spacing [[$this getInputLabel] GetSpacing]

    $_segmentedImage SetDimensions $x $y $z
    $_segmentedImage SetExtent [lindex $extent 0] [lindex $extent 1] \
  [lindex $extent 2] [lindex $extent 3] \
  [lindex $extent 4] [lindex $extent 5]

    $_segmentedImage SetOrigin [lindex $origin 0] [lindex $origin 1] [lindex $origin 2]
    $_segmentedImage SetSpacing [lindex $spacing 0] [lindex $spacing 1] [lindex $spacing 2]
    $_segmentedImage SetScalarType [[$this getInputForeground] GetScalarType]
    $_segmentedImage AllocateScalars
    $_segmentedImage DeepCopy [$this getInputForeground]

    $_gestureImage SetDimensions $x $y $z
    $_gestureImage SetExtent [lindex $extent 0] [lindex $extent 1] \
  [lindex $extent 2] [lindex $extent 3] \
  [lindex $extent 4] [lindex $extent 5]

    $_gestureImage SetOrigin [lindex $origin 0] [lindex $origin 1] [lindex $origin 2]
    $_gestureImage SetSpacing [lindex $spacing 0] [lindex $spacing 1] [lindex $spacing 2]
    $_gestureImage SetScalarType [[$this getInputLabel] GetScalarType]
    $_gestureImage AllocateScalars
    $_gestureImage DeepCopy [$this getInputLabel]

  # set range [$_gestureImage GetScalarRange]  
  # if { [llength $range] == 2 } {
  #    set maxRange [lindex $range 1]
  #    puts "range is $range"
  #    if {$maxRange > 0 } {
  #        set _foundPainted 1
  #      }
  #  }
  # if { $_foundPainted == 0 } {
  #   $this setInputImageData $_segmentedImage $_foregroundID
     #$this setInputImageData $_segmentedImage $_labelID

  # } 
   puts "Done Initializing"
}

# ------------------------------------------------------------------
#                             HELPER METHODS
# ------------------------------------------------------------------

itcl::body GrowCutSegmentEffect::processEvent { {caller ""} {event ""} } {

  #puts "In ProcessEvent : GrowCutSegmentEffect "

  # chain to superclass
  chain $caller $event

  if { [info command $sliceGUI] == "" } {
    # the sliceGUI was deleted behind our back, so we need to 
    # self destruct
    itcl::delete object $this
    return 
  }

  set grabID [$sliceGUI GetGrabID]
  if { ($grabID != "") && ($grabID != $this) } {
    # some other widget wants these events
    # -- we can position wrt the current slice node
    $this positionActors
    [$sliceGUI GetSliceViewer] RequestRender
    return 
 }


 set event [$sliceGUI GetCurrentGUIEvent] 
  switch $event {
    "LeftButtonPressEvent" {
        # puts "swapping foreground to gestures..."
        # $this swapInputForegroundLabel $_foregroundID $_labelID
  
        set _changedImage 1

       foreach {x y} [$_interactor GetEventPosition] {}              
       if { $smudge } {
         continue
       } else {
        set activeColor [EditorGetPaintLabel]
         }   
       }
    }
}


itcl::body GrowCutSegmentEffect::apply {} {

    puts "in Apply method"

    set grabID [$sliceGUI GetGrabID]
    if { ($grabID != "") && ($grabID != $this) } {
      return
    }

    
    if { [$this getInputLabel] == "" || [$this getInputBackground] == "" } {
       $this flashCursor 3
       return
    }

    puts "Applying to run the Grow Cut Segmentation Algorithm"

    if { ![info exists o(growCutFilter)] } {
        puts "CREATING GROW CUT FILTER "
       set o(growCutFilter) [vtkNew vtkITKGrowCutSegmentationImageFilter]
    }

   $this setProgressFilter $o(growCutFilter) "GrowCutSegment Filter"
   catch {
   set numCnodes [$::slicer3::MRMLScene GetNumberOfNodesByClass "vtkMRMLScalarVolumeNode"]
   for { set j 0 } { $j < $numCnodes } { incr j } {
    set vnode [$::slicer3::MRMLScene GetNthNodeByClass $j "vtkMRMLScalarVolumeNode"]
    set vID [$vnode GetID]
    if { $vID == $_foregroundID } {
     
      $_segmentedImage DeepCopy [$vnode GetImageData]
    } 
    if { $vID == $_labelID } {
      $_gestureImage DeepCopy [$vnode GetImageData]
    }
   
   }
   } errCopy
  if { $errCopy != "" } { puts "error in getting the gesture and segmented images $errCopy" }

    $o(growCutFilter) SetInput 1 $_gestureImage 
    $o(growCutFilter) SetInput 2 $_segmentedImage 
    
    puts "setting inputs for the grow cut filter ... "
    $o(growCutFilter) SetInput 0 [$this getInputBackground] 
    scan [$_gestureImage GetSpacing] "%f %f %f" dx dy dz
    set voxelvolume [expr $dx * $dy * $dz]
    set conversion 1000
    set voxelamt [expr $objectSize / $voxelvolume]
    set voxelnumber [expr round($voxelamt) * $conversion]
 
    set cubeIndex [expr 1.0/3.0]
    
    set oSize [expr round([expr pow($voxelnumber,$cubeIndex)] )]

    set objectSize 1
    
    $o(growCutFilter) SetObjectSize $oSize
    $o(growCutFilter) SetContrastNoiseRatio $contrastNoiseRatio
    $o(growCutFilter) SetPriorSegmentConfidence  $priorStrength 
    $o(growCutFilter) SetNumberOfThreads 1 
    puts "updating grow cut filter "
    
    after idle [$o(growCutFilter) Update]
    
   $_segmentedImage DeepCopy [$o(growCutFilter) GetOutput]

   $this setInputImageData $_segmentedImage $_foregroundID
   

   #$this swapInputForegroundLabel $_labelID $_foregroundID

   puts "done grow cut..."
  
}



itcl::body GrowCutSegmentEffect::removeApply { } {
    
    #foreach e [after info] {
    #  set script [lindex [after info $e] 0]
    #  puts "$script "  
  if {[info exists o(growCutFilter)]} {
      after 0 ::EffectSWidget::RemoveAl      
  }
    #}
}


itcl::body GrowCutSegmentEffect::updateRadius1 {} {

    set node [EditorGetParameterNode]    
    $node SetParameter "Paint,radius" 1
   
}


itcl::body GrowCutSegmentEffect::updateRadius2 {} {

    set node [EditorGetParameterNode]    
    $node SetParameter "Paint,radius" 2
   
}


itcl::body GrowCutSegmentEffect::updateRadius3 {} {

    set node [EditorGetParameterNode]    
    $node SetParameter "Paint,radius" 3
   
}


itcl::body GrowCutSegmentEffect::updateRadius4 {} {

    set node [EditorGetParameterNode]    
    $node SetParameter "Paint,radius" 4
   
}


itcl::body GrowCutSegmentEffect::updateRadius5 {} {

    set node [EditorGetParameterNode]    
    $node SetParameter "Paint,radius" 5
   
}





itcl::body GrowCutSegmentEffect::buildOptions {} {
   

#  call superclass version of buildOptions
  chain

#  forget the options set by the labeler

  foreach w "radius paintThreshold paintRange cancel" {
      if { [info exists o($w)] } {
    $o($w) SetParent ""
    pack forget [$o($w) GetWidgetName]
      }
  }

  puts "bulding options.... "
  
  
  #
  # a radius control
  #
  set o(radius) [vtkNew vtkKWThumbWheel]
  $o(radius) SetParent [$this getOptionsFrame]
  $o(radius) PopupModeOn
  $o(radius) Create
  $o(radius) DisplayEntryAndLabelOnTopOn
  $o(radius) DisplayEntryOn
  $o(radius) DisplayLabelOn
  $o(radius) SetValue [[EditorGetParameterNode] GetParameter Paint,radius]
  $o(radius) SetMinimumValue 0.01
  $o(radius) ClampMinimumValueOn
  [$o(radius) GetLabel] SetText "Radius: "
  $o(radius) SetBalloonHelpString "Set the radius of the paint brush in millimeters"
  pack [$o(radius) GetWidgetName] \
    -side top -anchor e -fill x -padx 1 -pady 1 
  
  #
  # Preset buttons
  #

  set o(1) [vtkNew vtkKWPushButton]
  $o(1) SetParent [$this getOptionsFrame]
  $o(1) Create
  $o(1) SetText "1"
  $o(1) SetBalloonHelpString "Radius 1"
  pack [$o(1) GetWidgetName] \
      -side left -anchor e -padx 2 -pady 2 


  set o(2) [vtkNew vtkKWPushButton]
  $o(2) SetParent [$this getOptionsFrame]
  $o(2) Create
  $o(2) SetText "2"
  $o(2) SetBalloonHelpString "Radius 2"
  pack [$o(2) GetWidgetName] \
      -side left -anchor e -padx 2 -pady 2 


  set o(3) [vtkNew vtkKWPushButton]
  $o(3) SetParent [$this getOptionsFrame]
  $o(3) Create
  $o(3) SetText "3"
  $o(3) SetBalloonHelpString "Radius 3"
  pack [$o(3) GetWidgetName] \
      -side left -anchor e -padx 2 -pady 2 


  set o(4) [vtkNew vtkKWPushButton]
  $o(4) SetParent [$this getOptionsFrame]
  $o(4) Create
  $o(4) SetText "4"
  $o(4) SetBalloonHelpString "Radius 4"
  pack [$o(4) GetWidgetName] \
      -side left -anchor e -padx 2 -pady 2 


  set o(5) [vtkNew vtkKWPushButton]
  $o(5) SetParent [$this getOptionsFrame]
  $o(5) Create
  $o(5) SetText "5"
  $o(5) SetBalloonHelpString "Radius 5"
  pack [$o(5) GetWidgetName] \
      -side left -anchor e -padx 2 -pady 2 

  #
  # a help button
  #
  set o(help) [vtkNew vtkSlicerPopUpHelpWidget]
  $o(help) SetParent [$this getOptionsFrame]
  $o(help) Create
  $o(help) SetHelpTitle "Grow Cut Segmentation"
  $o(help) SetHelpText "This tool enables to interactively segment a region of interest. For operation, this tool requires the user to specify at least an 'in-region' and an 'out-region' marked using different colors. To specify 'in-region', select a color from the Label and paint a stroke (gesture) inside the region of interest. Repeat the same with a different label color for the 'out-region'. To correct a segmentation, simply paint on some areas that need correction with the appropriate label color. The tool automatically picks a region of interest (ROI) based on the gestures painted by the user. To extend the ROI, paint on the areas that need to be segmented. This tool also supports 'n-class' segmentation. For faster segmentation, optionally use the CropVolume tool to crop the region of interest and use this tool."
  $o(help) SetBalloonHelpString "Bring up help window."
  pack [$o(help) GetWidgetName] \
    -side right -anchor sw -padx 2 -pady 2 

  #
  # an apply button
  #
   set o(apply) [vtkNew vtkKWPushButton]
   $o(apply) SetParent [$this getOptionsFrame]
   $o(apply) Create
   $o(apply) SetText "Apply"
   $o(apply) SetBalloonHelpString "Apply to run segmentation.\n Use the 'a' hotkey to apply segmentation"
   pack [$o(apply) GetWidgetName] \
      -side right -anchor e -padx 2 -pady 2 

  #
  # a cancel button
  #
  set o(cancel) [vtkNew vtkKWPushButton]
  $o(cancel) SetParent [$this getOptionsFrame]
  $o(cancel) Create
  $o(cancel) SetText "Cancel"
  $o(cancel) SetBalloonHelpString "Cancel current segmentation."
  pack [$o(cancel) GetWidgetName] \
      -side right -anchor e -padx 2 -pady 2 


  set tag [$o(1) AddObserver AnyEvent "after idle $this updateRadius1"]
  lappend _observerRecords "$o(1) $tag"

  set tag [$o(2) AddObserver AnyEvent "after idle $this updateRadius2"]
  lappend _observerRecords "$o(2) $tag"

  set tag [$o(3) AddObserver AnyEvent "after idle $this updateRadius3"]
  lappend _observerRecords "$o(3) $tag"

  set tag [$o(4) AddObserver AnyEvent "after idle $this updateRadius4"]
  lappend _observerRecords "$o(4) $tag"

  set tag [$o(5) AddObserver AnyEvent "after idle $this updateRadius5"]
  lappend _observerRecords "$o(5) $tag"
  
  set tag [$o(apply) AddObserver AnyEvent "$this apply"]
  lappend _observerRecords "$o(apply) $tag"
  set tag [$o(cancel) AddObserver AnyEvent "after 0 $this removeApply"]
  lappend _observerRecords "$o(cancel) $tag"

  if { [$this getInputBackground] == "" || [$this getInputLabel] == "" } {
    $this errorDialog "Background and Label map needed for Grow Cut Segmentation"
    after idle ::EffectSWidget::RemoveAll
  }
 
  $this updateGUIFromMRML

}





itcl::body GrowCutSegmentEffect::tearDownOptions { } {

  # call superclass version of tearDownOptions
  chain

  foreach w "help cancel apply 1 2 3 4 5" {
      if { [info exists o($w)] } {
    $o($w) SetParent ""
    pack forget [$o($w) GetWidgetName] 
      }
  }
#  if { $swapped == 1 } {
   $this swapInputForegroundLabel $_foregroundID $_labelID
#  }
}


itcl::body GrowCutSegmentEffect::updateMRMLFromGUI { } {
    
#    puts "updating MRML from GUI"

    chain
    set node [EditorGetParameterNode]
    
#    if { [info exists o(objectSize)] } {
#       $this configure -objectSize [$o(objectSize) GetValue]
#       $node SetParameter "GrowCutSegment,objectSize" $objectSize   
#    }

#    if { [info exists o(contrastNoiseRatio)] } {
#       $this configure -contrastNoiseRatio [$o(contrastNoiseRatio) GetValue]
#       $node SetParameter "GrowCutSegment,contrastNoiseRatio" $contrastNoiseRatio   
#    }

#    if { [info exists o(priorStrength)] } {
#       $this configure -priorStrength [$o(priorStrength) GetValue]
#       $node SetParameter "GrowCutSegment,priorStrength" $priorStrength   
#    }
    
#    if { [info exists o(overlay)] } {
#       $this configure -overlay [[$o(overlay) GetWidget] GetSelectedState]
#       $node SetParameter "GrowCutSegment,overlay" $overlay
#    }

#    puts "done updating MRML from GUI"
    
}


itcl::body GrowCutSegmentEffect::setMRMLDefaults { } {
    
    puts "setting MRML defaults in Grow Cut"

    chain
    
    set node [EditorGetParameterNode]

    foreach {param default} {

      contrastNoiseRatio 0.8
      priorStrength 0.0003
      segmented 2
    } {
        set pvalue [$node GetParameter Paint,$param] 
  if { $pvalue == "" } {
      $node SetParameter GrowCutSegment,$param $default
  }
    }

    $node SetParameter "Paint, radius" 3
}



itcl::body GrowCutSegmentEffect::updateGUIFromMRML { } {

  chain

  set node [EditorGetParameterNode]
    
  $this preview

}


proc GrowCutSegmentEffect::ConfigureAll { args } {
    foreach pw [itcl::find objects -class GrowCutSegmentEffect] {
     eval $pw configure $args
    }
}


