#
# Editor logic procs
#


#
# make a model of the current label map for the given slice logic
#
proc EditorAddQuickModel { sliceLogic } {

  #
  # get the image data for the label layer
  #
  set layerLogic [$sliceLogic GetLabelLayer]
  set volumeNode [$layerLogic GetVolumeNode]
  if { $volumeNode == "" } {
    puts "cannot make quick model - no volume node for $layerLogic in $sliceLogic"
    return
  }
  set imageData [$volumeNode GetImageData]

  #
  # make a poly data in RAS space
  #
  set dsm [vtkDiscreteMarchingCubes New]
  $dsm SetInput $imageData
  set tpdf [vtkTransformPolyDataFilter New]
  set ijkToRAS [vtkTransform New]
  $volumeNode GetRASToIJKMatrix [$ijkToRAS GetMatrix] 
  $ijkToRAS Inverse
  $tpdf SetInput [$dsm GetOutput]
  $tpdf SetTransform $ijkToRAS
  $tpdf Update

  #
  # create a mrml model for the new data
  #
  set modelNode [vtkMRMLModelNode New]
  set modelDisplayNode [vtkMRMLModelDisplayNode New]
  $modelNode SetName "QuickModel"
  $modelNode SetScene $::slicer3::MRMLScene
  $modelDisplayNode SetScene $::slicer3::MRMLScene
  eval $modelDisplayNode SetColor [lrange [EditorGetPaintColor $::Editor(singleton)] 0 2]
  $::slicer3::MRMLScene AddNode $modelDisplayNode
  $modelNode SetAndObserveDisplayNodeID [$modelDisplayNode GetID]
  $::slicer3::MRMLScene AddNode $modelNode
  $modelNode SetAndObservePolyData [$tpdf GetOutput]

  #
  # clean up
  #
  $dsm Delete
  $ijkToRAS Delete
  $tpdf Delete
  $modelNode Delete
  $modelDisplayNode Delete

}

#
#
proc EditorLabelCheckPoint {} {

  #
  # get the image data for the label layer
  #
  set sliceLogic [$::slicer3::ApplicationLogic GetSliceLogic "Red"]
  set layerLogic [$sliceLogic GetLabelLayer]
  set volumeNode [$layerLogic GetVolumeNode]
  if { $volumeNode == "" } {
    puts "cannot make label checkpoint - no volume node"
    return
  }

  # find a unique name for this copy
  set sourceName [$volumeNode GetName]
  set id 0
  while {1} {
    set targetName $sourceName-$id
    set nodes [$::slicer3::MRMLScene GetNodesByName $targetName]
    set numberOfItems [$nodes GetNumberOfItems]
    $nodes Delete
    if { $numberOfItems == 0 } {
      break
    }
    incr id
  }

  set volumesLogic [$::slicer3::VolumesGUI GetLogic]
  set labelNode [$volumesLogic CloneVolume $::slicer3::MRMLScene $volumeNode $targetName]
}

#proc ConversiontomL {this Voxels} {
  
#  set sliceLogic [$::slicer3::ApplicationLogic GetSliceLogic "Red"]
#  set layerLogic [$sliceLogic GetLabelLayer]
#  set volumeNode [$layerLogic GetVolumeNode]
#  set inputVolume [$volumeNode GetImageData]

#  scan [$inputVolume GetSpacing] "%f%f%f" dx dy dz
#  set voxelvolume [expr $dx * $dy * $dz]
#  set conversion 1000

#  set voxelamount [expr $Voxels * $voxelvolume]
#  set mL [expr round($voxelamount) / $conversion]

#  return $mL
#}

#proc ConversiontoVoxels {this mL} {

#  set sliceLogic [$::slicer3::ApplicationLogic GetSliceLogic "Red"]
#  set layerLogic [$sliceLogic GetLabelLayer]
#  set volumeNode [$layerLogic GetVolumeNode]
#  set inputVolume [$volumeNode GetImageData]

#  scan [$inputVolume GetSpacing] "%f %f %f" dx dy dz
#  set voxelvolume [expr $dx * $dy * $dz]
#  set conversion 1000

#  set voxelamount [expr $mL / $voxelvolume]
#  set Voxels [expr round($voxelamount) * $conversion]

#  return $Voxels
#}

#proc EditorCreateGestureParameterNode {id} {

#  set node [vtkMRMLScriptedModuleNode New]
#  $node SetModuleName "Editor"

  #set node defaults
  #puts "setting parameter $id.."
#  $node SetParameter gestureid $id 
  
#  $::slicer3::MRMLScene AddNode $node
#  $node Delete

#proc EditorGetGestureParameterNode {id} {


#  set node ""
 # puts "checking the nodes.. "
#  set nNodes [$::slicer3::MRMLScene GetNumberOfNodesByClass "vtkMRMLScriptedModuleNode"]
#  for {set i 0} {$i < $nNodes} {incr i} {
#    set n [$::slicer3::MRMLScene GetNthNodeByClass $i "vtkMRMLScriptedModuleNode"]
#    if { [$n GetModuleName] == "Editor" } {
   #  puts "got a node matching the module name... "
#      set nodeid [$n GetParameter "gestureid"]
   #   puts "got a node id : $nodeid"
#      if { $nodeid != "" } {
   #      puts "found node $n"
#         $n SetParameter gestureid $id
#         set node $n
#         break
#      }
#    }
#  }

 # if { $node == "" } {
  #  puts "creating a new node..."
 #   EditorCreateGestureParameterNode $id
  #  puts "getting the newly created node..."
 #   set node [EditorGetGestureParameterNode $id]
 # }

 # return $node

#}



#proc EditorGestureCheckPoint {} {

   #
   # Create a new gesture map for the corresponding label layer 
   #
   
 #  puts "Editor Gesture Check Point"
 #  catch {
 #  set sliceLogic [$::slicer3::ApplicationLogic GetSliceLogic "Red"]
 #  set layerLogic [$sliceLogic GetLabelLayer]
 #  set labelNode [$layerLogic GetVolumeNode]
  
 #  set labelID [$labelNode GetID]
   
   #
   # Check if we have a node corresponding to the label node
   #
 #  set name "[$labelNode GetID]-gesture"
  # puts "name : $name"
 #  set nodes [$::slicer3::MRMLScene GetNodesByName $name]
  
  # puts "number of items : [$nodes GetNumberOfItems]"
   
 #  if { [$nodes GetNumberOfItems] == 0 } {

   #   puts "adding a new volume to MRML Scene $name"
 #     set volumesLogic [$::slicer3::VolumesGUI GetLogic]
 #     set gestureNode [$volumesLogic CreateLabelVolume $::slicer3::MRMLScene $labelNode $name]
 #     set ID [$gestureNode GetID]
 #     catch { 
 #       set node [EditorGetGestureParameterNode $ID]
 #     } check1
 #     if { $check1 != ""} { puts " foo check $check1" }

 # } else {
  # puts "node of $name exists... "
 
#   set nScriptedNodes [$::slicer3::MRMLScene GetNumberOfNodesByClass "vtkMRMLScriptedModuleNode"]
    
 #   set nNodes [$nodes GetNumberOfItems]
    
 #   set foundnode -1
    
 #   for {set i 0} {$i < $nNodes} {incr i} {
    
 #    set testvol [$nodes GetItemAsObject $i]
 #    set testId [$testvol GetID]
 #    set testname [$testvol GetName]
     
 #    if {$testname == $name } {
 #       set foundnode $testId
 #       break
 #    }
 #  }
   
 #  if { $foundnode == -1 } {
    
 #    set volumesLogic [$::slicer3::VolumesGUI GetLogic]
 #    set gestureNode [$volumesLogic CreateLabelVolume $::slicer3::MRMLScene $labelNode $name]
 #    set ID [$gestureNode GetID]
#     puts "creating a new node for $ID" 
#     set node [EditorGetGestureParameterNode $ID]
#    } else {
 #     puts "setting the volume ids.. "
#      set node [EditorGetGestureParameterNode $foundnode]
#    }
#  }
   
# } check 
 # if { $check != "" } {puts "error in getting the label node : $labelNodeCheck"}
#}
#}


#
# make it easier to test the model by looking for the first slice logic
#
proc EditorTestQuickModel {} {
  set sliceLogic [lindex [vtkSlicerSliceLogic ListInstances] 0]
  EditorAddQuickModel $sliceLogic
}
