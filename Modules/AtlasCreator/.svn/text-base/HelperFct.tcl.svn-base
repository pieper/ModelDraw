proc VolumeReader { i  filePrefix } {

    set Extension [file extension "$filePrefix"]

    vtkITKArchetypeImageSeriesScalarReader Volume($i,vol)
    set VOL Volume($i,vol)

   $VOL SetArchetype $filePrefix
   if {![$VOL CanReadFile $filePrefix]} {
    puts "ERROR:ITK_Generic_Reader: Cannot read $filePrefix"
    exit 1
   }
   puts "Reading $filePrefix"

   if {[catch "$VOL UpdateInformation" res]} {
        puts "Error:ITK_Generic_Reader: Cannot read information for file $filePrefix\n\n$res"
        exit 1 
   }

   $VOL SetOutputScalarTypeToNative
   # $VOL SetOutputScalarTypeTo$Volume($i,scalarType)
   
   # $VOL SetDesiredCoordinateOrientationToNative
   $VOL SetUseNativeOriginOff

   $VOL Update
}
proc VolumeMathWriter {DATA DIR  FILE  FORMAT MATRIX  } {
    if {[file isdirectory $DIR] == 0} {
    catch {file mkdir $DIR}
       puts "Make Directory $DIR"
    }

    set export_iwriter  [vtkITKImageWriter New] 
    $export_iwriter SetInput $DATA 
    set FileName $DIR/$FILE$FORMAT 
    $export_iwriter SetFileName $FileName
    $export_iwriter SetRasToIJKMatrix $MATRIX
    $export_iwriter SetUseCompression 1                  
    # Write volume data
    puts "Writing $FileName ..." 
    $export_iwriter Write
    $export_iwriter Delete

    # set index 1
    # set FILE "$DIR/PCAModellingStep"
    # while {[file exists ${FILE}${index}.log]} { incr index }
    # WriteASCIIFile ${FILE}${index}.log "PCA_Modelling $::argv_orig"    
}

proc Print { text fid { NoNewLine 0 } } {
   if { $NoNewLine } { set cmd "puts -nonewline"
   } else { set cmd "puts" }
   if {$::MathImage(PrintToLogFile) && $fid != "" } { set cmd  "$cmd $fid" }
   eval $cmd \"$text\"
}

# When you run under windows 'puts stderr' does not spit out the error message
proc PrintError {Text} {
    if { $::env(OS) == "Windows_NT" } {
    puts $Text 
    } else {
    puts stderr $Text 
    }
}

proc assert { command fid} {
  if  {[catch $command errmsg] == 1} {
      if {$::MathImage(PrintToLogFile) && $fid != "" } {
       puts $fid $errmsg
     } else {    
       PrintError $errmsg 
     }
    return 1
  }
  return 0
} 


