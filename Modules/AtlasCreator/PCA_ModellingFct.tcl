#=auto==========================================================================
# (c) Copyright 2002 Massachusetts Institute of Technology
#
# Permission is hereby granted, without payment, to copy, modify, display 
# and distribute this software and its documentation, if any, for any purpose, 
# provided that the above copyright notice and the following three paragraphs 
# appear on all copies of this software.  Use of this software constitutes 
# acceptance of these terms and conditions.
#
# IN NO EVENT SHALL MIT BE LIABLE TO ANY PARTY FOR DIRECT, INDIRECT, SPECIAL, 
# INCIDENTAL, OR CONSEQUENTIAL DAMAGES ARISING OUT OF THE USE OF THIS SOFTWARE 
# AND ITS DOCUMENTATION, EVEN IF MIT HAS BEEN ADVISED OF THE POSSIBILITY OF 
# SUCH DAMAGE.
#
# MIT SPECIFICALLY DISCLAIMS ANY EXPRESS OR IMPLIED WARRANTIES INCLUDING, 
# BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR 
# A PARTICULAR PURPOSE, AND NON-INFRINGEMENT.
#
# THE SOFTWARE IS PROVIDED "AS IS."  MIT HAS NO OBLIGATION TO PROVIDE 
# MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS. 
#
#===============================================================================

# -----------------------------------------------------------------------------
# This script has important functions for PCA modelling 
#source [file join $env(SCRIPT_HOME) tcl/HelperFct.tcl]

proc GenerateDistanceMapWrapper { INPUT_FILE LABEL MAXDIST DISTBOUND OUTPUT_FILE } {

      VolumeReader 1 $INPUT_FILE 

      set OUTPUT [ GenerateDistanceMap  [Volume(1,vol) GetOutput]  $LABEL  $MAXDIST $DISTBOUND ]
      VolumeMathWriter $OUTPUT [file dirname $OUTPUT_FILE]  [file rootname [file tail   $OUTPUT_FILE ] ] [file extension $OUTPUT_FILE]  [Volume(1,vol) GetRasToIjkMatrix ]

      $OUTPUT Delete
      Volume(1,vol)  Delete  
}
proc GenerateDistanceMap { INPUT LABEL  MAXDIST DISTBOUND } {
     set OUTPUT [vtkImageData New] 
     eval $OUTPUT SetWholeExtent [$INPUT GetExtent] 
     eval $OUTPUT SetExtent [$INPUT GetExtent] 
      $OUTPUT SetNumberOfScalarComponents 1
      $OUTPUT SetScalarTypeToFloat 
      $OUTPUT AllocateScalars

     vtkImagePCAFilter Transfere
           Transfere TransfereLabelmapIntoPCADistanceMap $INPUT  $LABEL  $MAXDIST  $DISTBOUND  $OUTPUT
      Transfere Delete
      return $OUTPUT 
} 

# Determines the minum boundaing box of a volume 
proc FindCommonBoundingBoxOfVolume {vtkImage} {
    package require vtkEMPrivateSegment
    vtkImageClipAutomatic blub
    set Box [blub DetermineOutputWholeExtent $vtkImage]
    blub Delet
    return "$Box"
}

proc FindMinBoundingBoxOfVolumeList {VolumePathList} {
    set BoundMinX -1 
    set BoundMinY -1 
    set BoundMinZ -1 

    set BoundMaxX 0 
    set BoundMaxY 0 
    set BoundMaxZ 0 

    vtkIntArray MinBound 
    MinBound SetNumberOfValues 3
    vtkIntArray MaxBound 
    MaxBound SetNumberOfValues 3

    foreach VolumePath $VolumePathList {
    VolumeReader VolumeBlub $VolumePath

    vtkImageClipAutomatic blub
    blub DetermineOutputWholeExtent [Volume(VolumeBlub,vol) GetOutput] MinBound MaxBound
    blub Delete

    Volume(VolumeBlub,vol) Delete

    set MinX [MinBound GetValue 0]
    set MinY [MinBound GetValue 1]
    set MinZ [MinBound GetValue 2]

    set MaxX [MaxBound GetValue 0]
    set MaxY [MaxBound GetValue 1]
    set MaxZ [MaxBound GetValue 2]

    if {$BoundMinX > $MinX ||  $BoundMinX < 0 } {set BoundMinX $MinX }
    if {$BoundMaxX < $MaxX } {set BoundMaxX $MaxX }
    if {$BoundMinY > $MinY ||  $BoundMinY < 0 } {set BoundMinY $MinY }
    if {$BoundMaxY < $MaxY } {set BoundMaxY $MaxY }
    if {$BoundMinZ > $MinZ ||  $BoundMinZ < 0 } {set BoundMinZ $MinZ }
    if {$BoundMaxZ < $MaxZ } {set BoundMaxZ $MaxZ }
    # puts "Res: $MinX $MaxX $MinY $MaxY $MinZ $MaxZ"
    # puts "Box: $BoundMinX $BoundMaxX $BoundMinY $BoundMaxY $BoundMinZ $BoundMaxZ"
    }
    MinBound Delete
    MaxBound Delete
    return "$BoundMinX $BoundMaxX $BoundMinY $BoundMaxY $BoundMinZ $BoundMaxZ"
}


proc PrintPCAEigenvalues {NumberOfValues StructureName vtkPCA PCAPath } {
    global fid
    set EigenValuesFiles $PCAPath/PCA$StructureName/EigenValues_${StructureName}.txt 
    if  {[assert "set ::fidEigen [open $EigenValuesFiles w]" $fid] == 0} { 
    Print "Saved Eigenvalues in $EigenValuesFiles" $fid
    for {set i 0} { $i < $NumberOfValues } {incr i } {                 
        assert "puts $::fidEigen [format "%f" [$vtkPCA GetEigenValue $i]]" $fid
    }  
    assert "close $::fidEigen" $fid      
    }
}


proc AddEmptySlide {PATH FullMin FullMax ImageMin ImageMax} {
    global fid MathImage
    # ------------------------------
    # Add Empty slices if necessary 
    for {set j $FullMin} {$j < $ImageMin} {incr j} {
    set Number $j 
    if { [expr $j < 10] } { set Number "00$j"
    } else {if { [expr $j < 100]} { set Number "0$j" } }
    assert "exec cp $MathImage(PCAPath)/EmptySlices1/I.$Number  $PATH/." $fid
    }
    for {set j [expr $ImageMax +1] } {$j <= $FullMax} {incr j} {
    set Number $j 
    if { [expr $j < 10] } { set Number "00$j"
    } else { if { [expr $j < 100] } { set Number "0$j" }  }
    assert "exec cp $MathImage(PCAPath)/EmptySlices2/I.$Number  $PATH/." $fid
    }        
}
         
proc SeperateAndSaveCombinedPCAVolumes {vtkImage Extent BoundingBox FileDir FileName FileExtension MATRIX origEXTENT fid } {
    vtkImageClip Clip
      Clip SetInput $vtkImage 
      eval Clip SetOutputWholeExtent $Extent 
      Clip ClipDataOn
      Clip Update 
     
    # Now you have to extent id to normal size 
    # that is not correct yet - first push cliped extent to the right location 
    vtkImageTranslateExtent translate
    translate SetInput [Clip GetOutput]
    # KILIAN : originally translate SetTranslation [lindex $BoundingBox 0] [lindex $BoundingBox 2] [expr [lindex $MathImage(volRange) 0] - [lindex $Extent 4]] but I do not think this is correct 
    translate SetTranslation [lindex $BoundingBox 0] [lindex $BoundingBox 2]   [expr  [lindex $BoundingBox 4]   - [lindex $Extent 4]] 
    translate Update

    vtkImageConstantPad pad
      pad SetInput [translate GetOutput]
      # Change it if it is not zero !
      pad SetConstant 0.0
      eval pad SetOutputWholeExtent "$origEXTENT" 
      pad Update

    puts "ImageExtent [$vtkImage GetExtent] "
    puts "ClipExtent  [[Clip GetOutput] GetExtent]"
    puts "TransExtent [[translate GetOutput] GetExtent]"  
    puts "PaddiExtent [[pad GetOutput] GetExtent]" 
     
    Print "[VolumeMathWriter [pad GetOutput]  $FileDir $FileName $FileExtension $MATRIX ]" $fid
    pad Delete 
    translate Delete
    Clip Delete    
}


# -----------------------------------------
# Generate PCA model from shape data

# The wrapper is used in Slicer to pass a labelList correctly.
# Background: space divided lists can not be passed from Slicer - so we use a global variable as a workaround!
proc GeneratePCAModelWrapper {PCAPath  MaxNumberOfEigenVectors FileExtension PCACombineStructures} {

  # we fetch the global labelList
  set labelList [split $::acLabelListAsString ,]
  
  GeneratePCAModel $PCAPath $MaxNumberOfEigenVectors $labelList $FileExtension $PCACombineStructures
  
  return 1

}


proc GeneratePCAModel  { PCAPath  MaxNumberOfEigenVectors StructureList  FileExtension PCACombineStructures } {
  # puts " GeneratePCAModel  $PCAPath  $MaxNumberOfEigenVectors \"$StructureList\"  $FileExtension $PCACombineStructures"
 
  global tcl_platform  tcl_precision fid 
    if {[catch {set CaseList [exec ls -1 $PCAPath | grep case]}]  } {
    PrintError "Error: No cases as input for GeneratePCAModel in directory $PCAPath"
    return
  }

  # set CaseList [lrange $CaseList 0 19]
  # puts "Just Debugging "
  # puts "Warning: Currently no more than 20 cases" 
  set LengthCaseList [llength $CaseList]
  set NumberOfEigenVectors [expr $MaxNumberOfEigenVectors < $LengthCaseList  ? $MaxNumberOfEigenVectors : $LengthCaseList ]  
 
  # MathImageFct.tcl specific parameters

  set ::MathImage(PrintToLogFile) 0

 # Open Log File - still has to be completed 
  if { $::MathImage(PrintToLogFile) } { 
      set fid [OpenLogFile $LogFile GeneratePCAModel "PCATrainingFolder $PCAPath - StructureDirList $StructureList"]
      if {$fid == ""} {exit 1}
  } else {
     set fid ""
  }
 
  Print "------------------------------------------------------------------------" $fid 
  Print "-- Generate PCA Parameters" $fid 
  Print "------------------------------------------------------------------------" $fid 
  
  set MATRIX ""
  # Do we want to have them concaninated together or defined for each structure individidually 
  if {$PCACombineStructures }  {
      Print "Define one PCA Model for the following structures: $StructureList" $fid 
   
      set CombinationName ""
      foreach STRUCTURE $StructureList {
      if { $CombinationName == "" } {
          set CombinationName "[string range $STRUCTURE 0 3]"
      } else {
          set CombinationName "${CombinationName}_[string range $STRUCTURE 0 3]"
      }
      }

      vtkImagePCATraining PCA
      PCA SetNumberOfEigenVectors $NumberOfEigenVectors
   
      # Now find out the maximum of the bounding box 
      set VolumeList ""
      foreach CASE $CaseList  {
      set CASE_NO [file tail $CASE] 
      foreach STRUCTURE $StructureList {
          set VolumeList "$VolumeList  $PCAPath/$CASE_NO/${STRUCTURE}.${FileExtension}"
      }
      }
  
      # Clip so you have more space 
      # Kilian Define a bounding box later for each structure individually
      # As we just stakk them together it does not matter if each voxel in 
      # the stack across structure points to the same location 
      # Just take the max dimension of each of those boxes  
      # but keep orginal origin - thus a lot smaller 
      set BoundingBox [FindMinBoundingBoxOfVolumeList "$VolumeList"]
      puts "Minimum Common Bounding Box: $BoundingBox"

      set InputIndex 0

      foreach CASE $CaseList  {
          set CASE_NO [file tail $CASE] 
      vtkImageAppend imageAllStructures($CASE_NO)
      # Add images along the z Axis 
      imageAllStructures($CASE_NO) SetAppendAxis 2
  
      foreach STRUCTURE $StructureList {
          # Now designed for warfiled files 
             assert "VolumeReader VolumeBlub $PCAPath//$CASE_NO/$STRUCTURE.${FileExtension}" $fid   
              if { $MATRIX == "" } {
          set MATRIX [vtkMatrix4x4 New] 
                  $MATRIX Copy [Volume(VolumeBlub,vol) GetRasToIjkMatrix ]
                  set EXTENT [Volume(VolumeBlub,vol)  GetExtent]
          }
          
          
          vtkImageClip Clip(${CASE_NO},$STRUCTURE)
            Clip(${CASE_NO},$STRUCTURE) SetInput [Volume(VolumeBlub,vol) GetOutput]
             eval Clip(${CASE_NO},$STRUCTURE) SetOutputWholeExtent $BoundingBox
             Clip(${CASE_NO},$STRUCTURE) ClipDataOn
             Clip(${CASE_NO},$STRUCTURE) Update

         Volume(VolumeBlub,vol) Delete 
         imageAllStructures($CASE_NO) AddInput [Clip(${CASE_NO},$STRUCTURE) GetOutput]
         imageAllStructures($CASE_NO) Update
     }
     imageAllStructures($CASE_NO) Update
     PCA SetInput $InputIndex  [imageAllStructures($CASE_NO)  GetOutput] 
     incr InputIndex 
     }

     # puts [[imageAllStructures($CASE_NO) GetOutput] Print]
     # ------------------------------
     # Calculate EigenVectors and save results
     PCA Update

        assert "exec rm -r $PCAPath/PCA$CombinationName" $fid
    
     assert "file mkdir $PCAPath/PCA${CombinationName}" $fid

     PrintPCAEigenvalues  $NumberOfEigenVectors $CombinationName PCA $PCAPath

     # -----------------------------------
     # Write it out by returning to the case list 

     set NumberSlices  [expr [lindex $BoundingBox 5] - [lindex $BoundingBox 4]  + 1] 
     #  puts "[[PCA GetMean] Print]"
     set StructureIndex 0
     set BoundExtentMaxX [expr [lindex $BoundingBox 1] -[lindex $BoundingBox 0]]
     set BoundExtentMaxY [expr [lindex $BoundingBox 3] -[lindex $BoundingBox 2]]
 
     foreach STRUCTURE $StructureList {
     assert "file mkdir $PCAPath/PCA${CombinationName}/$STRUCTURE" $fid
     # This might be wrong -I 
     set Extent "0 $BoundExtentMaxX  0 $BoundExtentMaxY [expr $NumberSlices * $StructureIndex]  [expr $NumberSlices * ($StructureIndex +1) - 1]"
     SeperateAndSaveCombinedPCAVolumes [PCA GetMean] "$Extent" "$BoundingBox" "$PCAPath/PCA${CombinationName}/$STRUCTURE" MeanImage $FileExtension $MATRIX $EXTENT $fid
     incr StructureIndex 
     }

     for {set i 0} {$i < $NumberOfEigenVectors} {incr i} {
         set StructureIndex 0
     foreach STRUCTURE $StructureList {
         set Extent "0 $BoundExtentMaxX  0 $BoundExtentMaxY [expr $NumberSlices * $StructureIndex]  [expr $NumberSlices * ($StructureIndex +1) - 1]"
             SeperateAndSaveCombinedPCAVolumes [PCA GetEigenVectorIndex $i]  "$Extent" "$BoundingBox" "$PCAPath/PCA${CombinationName}/$STRUCTURE" EigenVector$i $FileExtension $MATRIX $EXTENT $fid   
         incr StructureIndex 
     }
     }
     PCA Delete
     $MATRIX Delete
     # Do not need them anymore 
     foreach CASE $CaseList  {
      set CASE_NO [file tail $CASE] 
      foreach STRUCTURE $StructureList {
          Clip(${CASE_NO},$STRUCTURE) Delete 
      }
     }

  } else {
    foreach STRUCTURE $StructureList { 
      Print "---- $STRUCTURE ----" $fid 
          set pcaStructDir   $PCAPath/PCA$STRUCTURE 
           if { [ file exists $pcaStructDir ] } { 
          assert "exec rm -r $pcaStructDir " $fid
       }

           assert "file mkdir $pcaStructDir" $fid

          vtkImagePCATraining PCA
              # PCA DebugOn

          PCA SetNumberOfEigenVectors $NumberOfEigenVectors
          set InputIndex 0
               
              set VolumeIDList "" 

          # ------------------------------
          # Read in Cases
          foreach CASE $CaseList  {
          # Load In image 
          set CASE_NO [file tail $CASE] 
                  # puts "Reading $CASE_NO"
          # Now designed for warfiled files 
          assert "VolumeReader $CASE_NO $PCAPath/$CASE_NO/$STRUCTURE.$FileExtension " $fid
                  set VolumeIDList "${VolumeIDList} $CASE_NO" 
          PCA SetInput $InputIndex [Volume($CASE_NO,vol)  GetOutput] 
          incr InputIndex
                 if { $MATRIX == "" } {
             set MATRIX [vtkMatrix4x4 New] 
                     $MATRIX DeepCopy [Volume($CASE_NO,vol) GetRasToIjkMatrix ]
                }
          }

          # ------------------------------
          # Calculate EigenVectors and save results
          puts "finished reading"
              PCA Update
              puts "Finished training "
          PrintPCAEigenvalues   $NumberOfEigenVectors  $STRUCTURE PCA  $PCAPath

          assert "[VolumeMathWriter [PCA GetMean]  $pcaStructDir  MeanImage  .$FileExtension $MATRIX ]" $fid
          for {set i 0} {$i < $NumberOfEigenVectors} {incr i} {
          assert "VolumeMathWriter [PCA GetEigenVectorIndex $i] $pcaStructDir  EigenVector$i  .$FileExtension $MATRIX " $fid
          }
             $MATRIX  Delete
             set MATRIX ""
          foreach ID $VolumeIDList   {
          Volume($ID,vol) Delete 
          }
          PCA Delete
      }
  }
  # Closing Log File
  if {$::MathImage(PrintToLogFile)} {assert "close $fid" ""}
}

# -----------------------------------------
# Load in PCA Model 
# Input: vtkImagePCAApply PCA
proc LoadPCAModel {PCAApply } {
    global MathImage
    if {$MathImage(PCACombineStructures)} { 
    set CombineName [string range [file tail $MathImage(PCAPath)] 3 end]
    set EigenValueList [ReadASCIIFile [file join $MathImage(PCAPath)/EigenValues_$CombineName.txt]]
    } else {
    set EigenValueList [ReadASCIIFile [file join $MathImage(PCAPath)/PCA$MathImage(StructureList)/EigenValues_$MathImage(StructureList).txt]]
    }
    set NumEigenValues [llength $EigenValueList]

    set index 0 
    while {($index < $NumEigenValues) && ([lindex $EigenValueList $index] > 0.0)} {incr index }
    
    if {$index <  $NumEigenValues} {
    puts "Ignored all Eigenvalues after the $index position, bc they were smaller or equal to zero"
    set EigenValueList [lrange $EigenValueList 0 [expr $index -1]]
    set NumEigenValues $index
    }

    # Take those out of the list for whom we do not have a fit 
    set index 0
    set SortedEigenVectorList ""
    if {$MathImage(PCACombineStructures)} { 
    if {[catch {set EigenVectorList [exec ls -1 $MathImage(PCAPath)/$MathImage(StructureList) | grep EigenVector]}]  } {
        PrintError "Error: No EigenVectors as input for ViewPCAModel in $MathImage(PCAPath)/PCA$MathImage(StructureList)"
        exit 1
    }
    } else {
    if {[catch {set EigenVectorList [exec ls -1 $MathImage(PCAPath)/PCA$MathImage(StructureList) | grep EigenVector]}]  } {
        PrintError "Error: No EigenVectors as input for ViewPCAModel in $MathImage(PCAPath)/PCA$MathImage(StructureList)"
        exit 1
    }
    }


    for {set i 0} {$i < $NumEigenValues} {incr i} {
    set position [lsearch $EigenVectorList  EigenVector$i]  
    if {$position < 0} {
        set EigenValueList [lreplace $EigenValueList $index $index ]
    } else {
        incr index
        lappend SortedEigenVectorList EigenVector$i
    }
    }
    set NumEigenModes  $index

    if {$NumEigenValues > $MathImage(MaxNumberOfEigenVectors) }  {
    set EigenValueList  [lrange  $EigenValueList 0 [expr $MathImage(MaxNumberOfEigenVectors) -1]]
    set SortedEigenVectorList  [lrange  $SortedEigenVectorList 0 [expr $MathImage(MaxNumberOfEigenVectors) -1]]
    set NumEigenModes  $MathImage(MaxNumberOfEigenVectors)
    }


    puts "Currently defined to only load in Floats as PCA values! "
    set volDataTyp $MathImage(volDataType)
    set MathImage(volDataType) Float

    if {$MathImage(PCACombineStructures)} { 
    set Volume(Mean,filePrefix)  $MathImage(PCAPath)/$MathImage(StructureList)/MeanImage/I
    } else {
    set Volume(Mean,filePrefix)  $MathImage(PCAPath)/PCA$MathImage(StructureList)/MeanImage/I
    }

    set MathImage(Mean,littleEndian) $MathImage(littleEndian)
    VolumeReader Mean

    $PCAApply SetMean [Volume(Mean,vol)  GetOutput] 

    set InputIndex 0
    foreach EV $SortedEigenVectorList  {
    if {$MathImage(PCACombineStructures)} { 
        set Volume(Eigen$InputIndex,filePrefixLoad)  $MathImage(PCAPath)/$MathImage(StructureList)/$EV/I
    } else {
        set Volume(Eigen$InputIndex,filePrefixLoad)  $MathImage(PCAPath)/PCA$MathImage(StructureList)/$EV/I
    }
    set Volume(Eigen$InputIndex,littleEndian) $MathImage(littleEndian)
    VolumeReader Eigen$InputIndex
    $PCAApply SetEigenVectorIndex $InputIndex [Volume(Eigen$InputIndex,vol)  GetOutput] 
    incr InputIndex
    }

    set index 0
    foreach EV $EigenValueList {
    $PCAApply SetEigenValue $index $EV
    incr index
    }
    $PCAApply Update

    # I can delete EigenVectors now bc they are copied to filter - saves space 
    set index 0
    foreach EV $SortedEigenVectorList  {
    Volume(Eigen$index,vol) Delete
    incr index
    }

    set MathImage(volDataType) $volDataTyp 

    return $NumEigenModes 
}

proc  Generate_3DModel_of_PCA { } {
    global MathImage Volume

    proc ReturnToDistanceManifold {vtkInput vtkThres vtkDistance  } {
    global MathImage BorderValue

    $vtkThres SetOutputScalarTypeTo$MathImage(volDataType)
    $vtkThres SetInput $vtkInput
    $vtkThres ThresholdByUpper 10
    $vtkThres SetInValue 1
    $vtkThres SetOutValue 0
    $vtkThres Update
    # [$vtkThres GetOutput] ReleaseDataFlagOn

    $vtkDistance SetAlgorithmToSaito
    $vtkDistance SignedDistanceMapOn
    $vtkDistance SetObjectValue  1
        $vtkDistance SetZeroBoundaryInside
        $vtkDistance SquareRootDistance
        $vtkDistance SetMaximumDistance 100
        $vtkDistance ConsiderAnisotropyOff
    $vtkDistance SetInput [$vtkThres GetOutput]
    $vtkDistance  Update
    # [$vtkDistance  GetOutput] ReleaseDataFlagOn
    }

    vtkImagePCAApply PCA
    set NumEigenModes [LoadPCAModel PCA]
    set Setting ""
    for {set i 0} { $i < $NumEigenModes } {incr i} {
    set Setting "${Setting}0.0 "
    set bValue($i) 0.0
    }

    # Return to distance manifold

    vtkImageData MeanImage 
    vtkFloatArray b
    Transfere_PCAParameters_Into_Representation PCA MeanImage b "$Setting"
    vtkImageKilianDistanceTransform vtkDistanceMean 
    vtkImageThreshold vtkThreshMean 
    ReturnToDistanceManifold MeanImage vtkThreshMean vtkDistanceMean 

    vtkImageData EigenImage
    set Setting "-1.0 $Setting"
  
    Transfere_PCAParameters_Into_Representation PCA EigenImage b "$Setting"

    vtkImageKilianDistanceTransform vtkDistanceShape 
    vtkImageThreshold vtkThreshShape
    ReturnToDistanceManifold EigenImage vtkThreshShape vtkDistanceShape

    # Just how distancemanifolds works
    set BorderValue 0
    incr BorderValue -1

    # Extract Area of interest 

    vtkImageThreshold thres
      thres SetOutputScalarTypeTo$MathImage(volDataType)
      thres SetInput [vtkDistanceShape GetOutput]
      # Kilian
      # If you just want to compute the border 
      # thres ThresholdBetween [expr $BorderValue + 2]  [expr $BorderValue + 4]
      thres ThresholdByUpper $BorderValue
      thres SetInValue 1
      thres SetOutValue 0
      thres Update
      # [thres GetOutput] ReleaseDataFlagOn
      # VolumeWriter [thres GetOutput]  $MathImage(PCAPath)/blubber I [lindex $MathImage(volRange) 0] [lindex $MathImage(volRange) 1]

      # Extract the mean to find out what was responsible for just the changes 
      vtkImageMathematics ExpantionImage
        ExpantionImage SetInput1 [vtkDistanceShape GetOutput]
        ExpantionImage SetInput2 [vtkDistanceMean  GetOutput] 
        ExpantionImage SetOperationToSubtract
        ExpantionImage Update
        # [ExpantionImage GetOutput] ReleaseDataFlagOn
     # VolumeWriter [ExpantionImage GetOutput]  $MathImage(PCAPath)/blubber I [lindex $MathImage(volRange) 0] [lindex $MathImage(volRange) 1]

      # Merge the data
      set Extrems  [ImageDataMergeData thres [ExpantionImage GetOutput]]
      set minValue [lindex $Extrems 0] 
      set maxValue [lindex $Extrems 1]
      puts "Volume Extrema: $minValue $maxValue"
     # GenerateModel
     vtkPolyDataMapper ModelMapper 
     # This set sets the range of the lookup table
     ModelMapper SetScalarRange $minValue $maxValue

     puts "-----------------------------------------------------"
     puts "Computing Eigenvector"

     set decimateIterations  0 
     set smoothIterations  5
     set UseSinc 0
     set NumberOfContours  13
     if {$MathImage(PCACombineStructures)} { 
        set FileName  $MathImage(PCAPath)/[lindex $MathImage(StructureList) 0]/PCAModel.vtk
    } else {
        set FileName  $MathImage(PCAPath)/PCA[lindex $MathImage(StructureList) 0]/PCAModel.vtk
    }

    # VolumeWriter [Math2 GetOutput]  $MathImage(PCAPath)/blubber I [lindex $MathImage(volRange) 0] [lindex $MathImage(volRange) 1]

     ModelMakerMarch 1 $decimateIterations  $smoothIterations   $UseSinc 4  [Math2 GetOutput] $FileName ModelMapper $minValue $maxValue $NumberOfContours

     # Delete Variables 
     Math Delete
     thres2  Delete
     Math2 Delete
     ExpantionImage Delete
     thres Delete
     vtkThreshMean Delete 
     vtkThreshShape Delete 
     Volume(Mean,vol) Delete

    vtkDistanceMean Delete
    vtkDistanceShape Delete
}

proc View_PCA_Model { } {
    global MathImage NumEigenModes
    # ----------------------------------
    # PCA specific window functions 
    
    proc SetWindowEigenValue {index value} {
    global b  EigenImage window level ColorBlub
    b SetValue $index $value 
    PCA GetParameterisedShape b EigenImage
    viewer SetColorLevel $level
    SetWindow $window
    }


    proc SaveModel { } {
      global MathImage bValue NumEigenModes
      set volDataTyp $MathImage(volDataType)
      set MathImage(volDataType) Float
      VolumeWriter EigenImage PCAModel  PCAModel  [lindex $MathImage(volRange) 0 ] [lindex $MathImage(volRange) 1] 
      set Settings ""
      for {set i 0} { $i < $NumEigenModes } {incr i} {
    set Settings "${Settings}$bValue($i) "
      }
      WriteASCIIFile PCAModel/PCAModel.txt "Parameters: $Settings"
      puts "Saved in data format Float"
      set MathImage(volDataType) $volDataTyp 
    }


    proc SaveModelBin { } {
      global MathImage level
      
      set volDataType  $MathImage(volDataType)
      set MathImage(volDataType) Short
      vtkImageThreshold thres
      thres SetOutputScalarTypeTo$MathImage(volDataType)
      thres SetInput EigenImage
      thres ThresholdByUpper $level
      thres SetInValue 1
      thres SetOutValue 0
      thres Update
      VolumeWriter [thres GetOutput] PCAModelBin  PCAModel  [lindex $MathImage(volRange) 0 ] [lindex $MathImage(volRange) 1]  
      thres Delete
      puts "Saved in data format $MathImage(volDataType)"

      set MathImage(volDataType) $volDataType 

    }

    # ----------------------------------
    # Initialize window with mean setting
    vtkImagePCAApply PCA
    set NumEigenModes [LoadPCAModel PCA]
    set Setting ""
    for {set i 0} { $i < $NumEigenModes } {incr i} {
    set Setting "${Setting}0.0 "
    set bValue($i) 0.0
    }

    vtkImageData EigenImage 
    vtkFloatArray b
    Transfere_PCAParameters_Into_Representation PCA EigenImage b "$Setting"

   # -------------------------------------------
   # Viewer
   vtkImageViewer viewer
   MathViewer EigenImage viewer
   source [file join $MathImage(Slicer_Home) Base/tests/WindowLevelInterface.tcl] 
   # Define Slides for EigenVectors 
   set index 3 
   for {set i 0} {$i < $NumEigenModes} {incr i} {
      frame .wl.f$index
      label .wl.f$index.lEigen -text "[expr $i +1]. EigenVector"
      scale .wl.f$index.sEigen -from -5 -to 5 -orient horizontal -command "SetWindowEigenValue $i" -variable bValue($i)  -resolution 1.0
     
      pack .wl.f$index  -side top
      pack .wl.f$index.lEigen .wl.f$index.sEigen  -side left
      incr index
   }

   button .wl.bSave -text "Save" -command "SaveModel"
   pack .wl.bSave -side top

   button .wl.bSaveBin -text "Save As Binary" -command "SaveModelBin"
   pack .wl.bSaveBin -side top

   # Start Window
   set window 1
   SetWindow 1 
   set level 0
   SetLevel 0
}

proc Transfere_Representation_Into_PCAParameters { } {
    global Volume MathImage

    vtkImagePCAApply PCA

    set NumEigenModes [LoadPCAModel PCA]

    vtkFloatArray ResultArray   
    ResultArray SetNumberOfValues $NumEigenModes
    
    # Make sure that they are from type Float
    vtkImageCast cast
      cast SetInput [Volume(1,vol) GetOutput]
      cast SetOutputScalarTypeToFloat 
      cast Update

    PCA GetShapeParameters [cast GetOutput] ResultArray 
    
    set Result ""

    # The Value in () is the B value needed to generate the shape with this filter - it is a little bit wired 
    set EigenValueList [ReadASCIIFile [file join $MathImage(PCAPath)/PCA$MathImage(StructureList)/EigenValues_$MathImage(StructureList).txt]]

    for {set i 0 } {$i < $NumEigenModes} {incr i} {
    lappend Result "[ResultArray GetValue $i] ([expr [ResultArray GetValue $i]*sqrt([lindex $EigenValueList $i])])"
    }

    cast Delete
    ResultArray Delete 
    PCA Delete

    return "$Result"
} 

# vtkPCA has to be from type vtkImagePCAApply
# vtkResult has to be from type vtkImageData
# vtkFloat has to be from type vtkFloatArray
proc Transfere_PCAParameters_Into_Representation {vtkPCA vtkResult vtkFloat Setting} {
    global MathImage
    $vtkResult SetScalarTypeToFloat
    eval $vtkResult SetSpacing $MathImage(volSpacing)
    eval $vtkResult SetExtent 0 [expr [lindex $MathImage(volDim) 0]-1] 0 [expr [lindex $MathImage(volDim) 1]-1] $MathImage(volRange)
    $vtkResult SetNumberOfScalarComponents $MathImage(volScalarNumber)
    $vtkResult Update
    $vtkResult AllocateScalars
    
    $vtkFloat SetNumberOfValues [llength $Setting]
    
    set index 0 
    foreach val $Setting {
        $vtkFloat SetValue $index $val
    incr index
    }
    $vtkPCA GetParameterisedShape $vtkFloat $vtkResult
}

# Initialize parameters
set tcl_precision 17
