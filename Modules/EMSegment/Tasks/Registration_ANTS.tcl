proc Get_ANTS_Installation_Path { } {
    set REGISTRATION_PACKAGE_FOLDER [Get_Installation_Path "ANTS-build" "ANTS"]
    return $REGISTRATION_PACKAGE_FOLDER
}


# ----------------------------------------------------------------------------
proc ANTSRegistration { fixedVolumeNode movingVolumeNode outVolumeNode backgroundLevel deformableType affineType } {
    variable SCENE
    variable LOGIC
    variable REGISTRATION_PACKAGE_FOLDER
    variable mrmlManager

    # Do not get rid of debug mode variable - it is sometimes very helpful !
    set ANTS_DEBUG_MODE 0

    if { $ANTS_DEBUG_MODE } {
        $LOGIC PrintText ""
        $LOGIC PrintText "DEBUG: ==========ANTSRegistration DEBUG MODE ============="
        $LOGIC PrintText ""
    }

    $LOGIC PrintText "TCL: =========================================="
    $LOGIC PrintText "TCL: == Image Alignment CommandLine: $deformableType "
    $LOGIC PrintText "TCL: =========================================="

    # check arguments

    if { $fixedVolumeNode == "" || [$fixedVolumeNode GetImageData] == "" } {
        PrintError "ANTSRegistration: fixed volume node not correctly defined"
        return ""
    }

    if { $movingVolumeNode == "" || [$movingVolumeNode GetImageData] == "" } {
        PrintError "ANTSRegistration: moving volume node not correctly defined"
        return ""
    }

    if { $outVolumeNode == "" } {
        PrintError "ANTSRegistration: output volume node not correctly defined"
        return ""
    }

    set fixedVolumeFileName [WriteDataToTemporaryDir $fixedVolumeNode Volume]
    if { $fixedVolumeFileName == "" } {
        # remove files
        return ""
    }
    set RemoveFiles "$fixedVolumeFileName"


    set movingVolumeFileName [WriteDataToTemporaryDir $movingVolumeNode Volume]
    if { $movingVolumeFileName == "" } {
        #remove files
        return ""
    }
    set RemoveFiles "$RemoveFiles $movingVolumeFileName"


    set outVolumeFileName [CreateTemporaryFileNameForNode $outVolumeNode]
    if { $outVolumeFileName == "" } {
        #remove files
        return ""
    }
    set RemoveFiles "$RemoveFiles $outVolumeFileName"

    ## ANTS specific arguments

    set CMD "$REGISTRATION_PACKAGE_FOLDER/ANTS"

    set CMD "$CMD 3"
    set CMD "$CMD -m MI\\\[$fixedVolumeFileName,$movingVolumeFileName,1,32\\\]"
    set CMD "$CMD -o \"$outVolumeFileName\""

    if { $affineType == [$mrmlManager GetRegistrationTypeFromString RegistrationTest] } {
        set CMD "$CMD -i 1x0x0"
    } elseif { $affineType == [$mrmlManager GetRegistrationTypeFromString RegistrationFast] } {
        set CMD "$CMD -i 30x20x10"
    } elseif { $affineType == [$mrmlManager GetRegistrationTypeFromString RegistrationSlow] } {
        set CMD "$CMD -i 50x30x10"
    } else {
        PrintError "ANTSRegistration: Unknown deformableType: $deformableType"
        return ""
    }

    set CMD "$CMD -r Gauss\\\[3,1\\\] -t Elast\\\[3\\\]"

    append outLinearTransformFileName $outVolumeFileName Affine.txt
    append outNonLinearTransformFileName $outVolumeFileName Warp.nii.gz
    set outTransformFileName $outNonLinearTransformFileName

    $LOGIC PrintText "TCL: Executing $CMD"
    catch { eval exec $CMD } errmsg
    $LOGIC PrintText "TCL: $errmsg"


    #TODO: remove workaround
    append outVolumeFileName_nii $outVolumeFileName ".nii"
    set CMD "$REGISTRATION_PACKAGE_FOLDER/WarpImageMultiTransform"
    set CMD "$CMD 3"
    set CMD "$CMD $movingVolumeFileName $outVolumeFileName_nii"
    set CMD "$CMD $outNonLinearTransformFileName $outLinearTransformFileName"
    set CMD "$CMD -R $fixedVolumeFileName"
    $LOGIC PrintText "TCL: Executing $CMD"
    catch { eval exec $CMD } errmsg
    $LOGIC PrintText "TCL: $errmsg"


    if { "[ANTSGetPixelTypeFromVolumeNode $fixedVolumeNode]" != "float" } {
        #TODO: remove workaround
        # convert pixeltype because of missing functionality in ANTS
        set CMD "$REGISTRATION_PACKAGE_FOLDER/ConvertImagePixelType"
        append $outVolumeFileName_nii ".nrrd"
        set CMD "$CMD $outVolumeFileName_nii $outVolumeFileName"
        set CMD "$CMD [ANTSGetPixelTypeFromVolumeNode $fixedVolumeNode]"
        $LOGIC PrintText "TCL: Executing $CMD"
        catch { eval exec $CMD } errmsg
        $LOGIC PrintText "TCL: $errmsg"
    }


    ## Read results back to scene
#    append outVolumeFileName
    if { [ReadDataFromDisk $outVolumeNode $outVolumeFileName Volume] == 0 } {
        if { [file exists $outVolumeFileName] == 0 } {
            set outTransformDirName ""
            PrintError "ANTSRegistration: out volume file doesn't exists"
        }
    }

    if { [file exists $outTransformFileName] == 0 } {
        set outTransformFileName ""
        PrintError "ANTSRegistration: out transform file doesn't exists"
    }

    foreach NAME $RemoveFiles {
        #file delete -force $NAME
    }

    # Remove Transformation from image
    $movingVolumeNode SetAndObserveTransformNodeID ""
    $SCENE Edited

    # return transformation directory name or ""
    puts "outTransformFileName: $outTransformFileName"
    return $outVolumeFileName
}


proc ANTSGetPixelTypeFromVolumeNode { volumeNode } {

    set referenceVolume [$volumeNode GetImageData]
    set scalarType [$referenceVolume GetScalarTypeAsString]
    switch -exact "$scalarType" {
        "char"           { set PIXELTYPE "0" }
        "unsigned char"  { set PIXELTYPE "1" }
        "short"          { set PIXELTYPE "2" }
        "unsigned short" { set PIXELTYPE "3" }
        "int"            { set PIXELTYPE "4" }
        "unsigned int"   { set PIXELTYPE "5" }
        "float"          { set PIXELTYPE "float" }
        "double"         { set PIXELTYPE "double" }
        default {
            PrintError "CMTKGetPixelTypeFromVolumeNode: Cannot handle a volume of type $scalarType"
            set PIXELTYPE "INVALID"
        }
    }
    return $PIXELTYPE
}


proc ANTSResampleCLI { inputVolumeNode referenceVolumeNode outVolumeNode transformDirName interpolationType backgroundLevel } {
    variable SCENE
    variable LOGIC
    variable REGISTRATION_PACKAGE_FOLDER

    $LOGIC PrintText "TCL: =========================================="
    $LOGIC PrintText "TCL: == Resample Image CLI : ANTSResampleCLI "
    $LOGIC PrintText "TCL: =========================================="

    set CMD "$REGISTRATION_PACKAGE_FOLDER/WarpImageMultiTransform"

    set outVolumeFileName [CreateTemporaryFileNameForNode $outVolumeNode]
    if { $outVolumeFileName == "" } { return 1 }

    set inputVolumeFileName [WriteDataToTemporaryDir $inputVolumeNode Volume]
    if { $inputVolumeFileName == "" } { return 1 }

    set referenceVolumeFileName [WriteDataToTemporaryDir $referenceVolumeNode Volume]
    if { $referenceVolumeFileName == "" } { return 1 }


    append outLinearTransformFileName $transformDirName Affine.txt
    append outNonLinearTransformFileName $transformDirName Warp.nii.gz

    append outVolumeFileName_nii $outVolumeFileName ".nii"

    set CMD "$CMD 3"
    set CMD "$CMD $inputVolumeFileName $outVolumeFileName_nii"
    set CMD "$CMD $outNonLinearTransformFileName $outLinearTransformFileName"
    set CMD "$CMD -R $referenceVolumeFileName"

    $LOGIC PrintText "TCL: Executing $CMD"
    catch { eval exec $CMD } errmsg
    $LOGIC PrintText "TCL: $errmsg"


#    set PLUGINS_DIR "[$LOGIC GetPluginsDirectory]" 
#    set CMD "${PLUGINS_DIR}/Cast"

    if { "[ANTSGetPixelTypeFromVolumeNode $referenceVolumeNode]" != "float" } {

        #TODO: remove workaround
        # convert pixeltype because of missing functionality in ANTS
        set CMD "$REGISTRATION_PACKAGE_FOLDER/ConvertImagePixelType"
        append $outVolumeFileName_nii ".nrrd"
        set CMD "$CMD $outVolumeFileName_nii $outVolumeFileName"
        set CMD "$CMD [ANTSGetPixelTypeFromVolumeNode $referenceVolumeNode]"
        $LOGIC PrintText "TCL: Executing $CMD"
        catch { eval exec $CMD } errmsg
        $LOGIC PrintText "TCL: $errmsg"

    }

    # Write results back to scene
    # This does not work $::slicer3::ApplicationLogic RequestReadData [$outVolumeNode GetID] $outVolumeFileName 0 1
    ReadDataFromDisk $outVolumeNode $outVolumeFileName Volume

    # clean up
#    file delete -force $outVolumeFileName
    file delete -force $outVolumeFileName_nii
    file delete -force $inputVolumeFileName
    file delete -force $referenceVolumeFileName

    return 0
}
