'''
    Launcher for the Atlas Creator
'''
import glob
from subprocess import call
import getopt
import os
import sys
import socket

'''=========================================================================================='''
def info( message ):
    '''
        Print message to stdout and flush
    '''
    print str( message )
    import sys
    # flush, to always show the output
    sys.stdout.flush()



'''=========================================================================================='''
def is_int( s ):
    try:
        int( s )
        return True
    except ValueError:
        return False



'''=========================================================================================='''
def ConvertDirectoryToString( directory ):
    '''
        Convert a directory to a string of filePaths with space as delimiter. While reading the directory,
        only real files are added to the list, sub-directories as well as links are ignored.
        
        directory
            a String containing the path to the directory
            
        Returns
            string of filePaths with space as delimiter
    '''
    output = ""

    if not directory:
        return listOfFilePaths

    if not os.path.isdir( directory ):
        return listOfFilePaths

    # loop through the directory
    for entry in glob.glob( os.path.join( directory, '*.*' ) ):

        if os.path.isfile( entry ) and not os.path.islink( entry ):

            # this is a real file and not a link or subdir
            # add it to the string
            output += str( os.path.normpath( entry ) )
            # add the delimiter
            output += " "

    return output.rstrip()



'''=========================================================================================='''
def ConvertListToString( list ):
    '''
        Convert a list to a string of items with space as delimiter.
        
        list
            a list object
            
        Returns
            string of list items with space as delimiter
    '''
    output = ""

    for i in list:
        output += str( i )
        output += " "

    return output.rstrip()



'''=========================================================================================='''
def usage():
    '''
        Print the help information.
    '''
    info( "Usage:" )
    info( "" )
    info( "-h, --help" )
    info( "        Show this information." )
    info( "" )
    info( "-i, --images DIR" )
    info( "        Directory containing original images." )
    info( "" )
    info( "-s, --segmentations DIR" )
    info( "        Directory containing segmentations." )
    info( "" )
    info( "-o, --output DIR" )
    info( "        Output directory." )
    info( "" )
    info( "--cmtk" )
    info( "        Use the CMTK toolkit for registration and resampling, instead of BRAINSFit." )
    info( "        The CMTK4Slicer extensions have to be installed in order to use CMTK." )
    info( "" )
    info( "--dramms" )
    info( "        Use the DRAMMS toolkit for registration and resampling, in addition to BRAINSFit or CMTK." )
    info( "" )
    info( "--skipRegistration" )
    info( "        Skip the registration and use existing transforms." )
    info( "" )
    info( "        The following arguments have to be specified if the registration is skipped:" )
    info( "" )
    info( "        --transforms DIR" )
    info( "                Directory containing existing transforms." )
    info( "" )
    info( "        --existingTemplate FILEPATH" )
    info( "                Filepath to an existing template used for resampling only." )
    info( "" )
    info( "--dynamic" )
    info( "        Use a dynamic template for registration based on means of images." )
    info( "" )
    info( "        The following arguments have to be specified if dynamic registration is chosen:" )
    info( "" )
    info( "        -m, --meanIterations INT" )
    info( "                Number of iterations to compute and register against a mean image." )
    info( "" )
    info( "--fixed" )
    info( "        Use a fixed template for registration." )
    info( "" )
    info( "        The following arguments have to be specified if fixed registration is chosen:" )
    info( "" )
    info( "        --template FILEPATH" )
    info( "                Filepath to an image used as a template for fixed registration." )
    info( "" )
    info( "        --ignoreTemplateSegmentation" )
    info( "                If activated, the template's segmentation will not be added to the atlases." )
    info( "" )
    info( "--affine" )
    info( "        Use 9 DOF affine registration additionally." )
    info( "" )
    info( "--affine12" )
    info( "        Use 12 DOF affine registration additionally. This includes --affine automatically." )
    info( "" )
    info( "-n, --non-rigid" )
    info( "        Use Non-Rigid registration additionally. This includes --affine and --affine12 automatically." )
    info( "" )
    info( "-w, --writeTransforms" )
    info( "        Write transforms to output directory." )
    info( "" )
    info( "--keepAligned" )
    info( "        Keep the aligned images and segmentations." )
    info( "" )
    info( "-l, --labels STRING" )
    info( "        List of labels to include for the atlases, f.e. \"3 4 5 6 8 10\"." )
    info( "        DEFAULT: detect labels automatically" )
    info( "" )
    info( "--normalize" )
    info( "        Normalize Atlases to 0..1." )
    info( "        If activated, the output cast will be set to Double." )
    info( "" )
    info( "        --normalizeTo INT" )
    info( "                The upper value to normalize the atlases to." )
    info( "                DEFAULT: 1" )
    info( "" )
    info( "--outputCast INT" )
    info( "        Output cast for the atlases. Possible values:" )
    info( "        0: char" )
    info( "        1: unsigned char" )
    info( "        2: double" )
    info( "        3: float" )
    info( "        4: int" )
    info( "        5: unsigned int" )
    info( "        6: long" )
    info( "        7: unsigned long" )
    info( "        8: short" )
    info( "        9: unsigned short" )
    info( "        DEFAULT: 8" )
    info( "" )
    info( "-c, --cluster" )
    info( "        Use the cluster mode." )
    info( "" )
    info( "        The following arguments have to be specified if cluster mode is chosen:" )
    info( "" )
    info( "        --schedulerCommand EXECUTABLE" )
    info( "                The executable to use as a scheduler in cluster mode, f.e. \"qsub\"." )
    info( "" )
    info( "--numberOfThreads" )
    info( "        Specify the number of threads to use for Registration." )
    info( "        By default, this is set to use the maximal number of threads for your machine." )
    info( "        DEFAULT: -1" )
    info( "" )
    info( "--pca" )
    info( "        Perform PCA Analysis on top of Resampling." )
    info( "" )
    info( "        --pcaMaxEigenVectors INT" )
    info( "                The number of maximal Eigenvectors to use for model generation." )
    info( "                DEFAULT: 10" )
    info( "" )
    info( "        --pcaCombine" )
    info( "                Combine the PCA output." )
    info( "" )
    info( "--slicer FILEPATH" )
    info( "        Filepath to the 3D Slicer launcher including arguments, f.e. \"/usr/bin/Slicer3 --tmp_dir /var/tmp\"." )
    info( "        DEFAULT: Find the 3D Slicer launcher automatically." )
    info( "" )
    info( "-d, --debug" )
    info( "        Enable debug information." )
    info( "" )
    info( "--dryrun" )
    info( "        Output executable commands instead of running the registration or resampling." )
    info( "" )
    info( "--examples" )
    info( "        Show usage examples." )
    info( "" )
    info( "" )
    info( "Developed by Daniel Haehn and Kilian Pohl, University of Pennsylvania. The research was funded by an ARRA supplement to NIH NCRR (P41 RR13218)." )
    info( "" )
    info( "Thanks to everyone!" )
    info( "" )



'''=========================================================================================='''
def examples():

    info( "Examples:" )
    info( "-----------------------------------------------------------------------------------------------" )
    info( "1. Run fixed registration with the testdata and normalize the atlases to 1:" )
    info( "" )
    info( '        python atlascreator.py -i TestData/originals/ -s TestData/segmentations/ -o /tmp/acout --fixed --template TestData/originals/case62.nrrd -w -l "3 4 5 6 7 8 9" --normalize' )
    info( "" )
    info( "-----------------------------------------------------------------------------------------------" )
    info( "2. Run fixed registration with the testdata and use CMTK instead of BRAINSFit and label auto-detection:" )
    info( "" )
    info( '        python atlascreator.py -i TestData/originals/ -s TestData/segmentations/ -o /tmp/acout --fixed --template TestData/originals/case62.nrrd -w --cmtk' )
    info( "" )
    info( "-----------------------------------------------------------------------------------------------" )
    info( "3. Run fixed registration with the testdata and use CMTK, label auto-detection and 12 DOF affine registration:" )
    info( "" )
    info( '        python atlascreator.py -i TestData/originals/ -s TestData/segmentations/ -o /tmp/acout --fixed --template TestData/originals/case62.nrrd -w --cmtk --affine12' )
    info( "" )
    info( "-----------------------------------------------------------------------------------------------" )
    info( "4. Run fixed registration with the testdata and use CMTK, label auto-detection and non-rigid registration:" )
    info( "" )
    info( '        python atlascreator.py -i TestData/originals/ -s TestData/segmentations/ -o /tmp/acout --fixed --template TestData/originals/case62.nrrd -w --cmtk --non-rigid' )
    info( "" )
    info( "-----------------------------------------------------------------------------------------------" )
    info( "5. Run dynamic registration with the testdata and normalize the atlases to 0..100:" )
    info( "" )
    info( '        python atlascreator.py -i TestData/originals/ -s TestData/segmentations/ -o /tmp/acout --dynamic --meanIterations 5 -w -l "3 4 5 6 7 8 9" --normalize --normalizeTo 100' )
    info( "" )
    info( "-----------------------------------------------------------------------------------------------" )
    info( "6. Run dynamic registration with the testdata on a cluster (scheduler command \"qsub -l centos5\"):" )
    info( "" )
    info( '        python atlascreator.py -i TestData/originals/ -s TestData/segmentations/ -o /tmp/acout --dynamic --meanIterations 5 -w -l "3 4 5 6 7 8 9" --normalize --cluster --schedulerCommand \"qsub -l centos5\"' )
    info( "" )
    info( "-----------------------------------------------------------------------------------------------" )
    info( "7. Use existing registrations and just re-sample" )
    info( "" )
    info( '        python atlascreator.py --skipRegistration --transforms /tmp/acout --existingTemplate TestData/segmentations/case62.nrrd -s TestData/segmentations/ -o /tmp/acout -l "3 4 5 6 7 8 9" --normalize --outputCast 3' )
    info( "" )



'''=========================================================================================='''
def main( argv ):
    '''
        Starting point, parse the command line args.
    '''

    info( "AtlasCreator for 3D Slicer" )
    info( "Version v0.42" )
    info( "Hostname: " + str( socket.gethostname() ) )
    info( "" )

    if len( argv ) == 0:
        usage()
        info( "Error: No arguments specified." )
        info( "" )
        sys.exit( 2 )

    try:
        opts, args = getopt.getopt( argv, "hdi:s:o:m:nwl:c", ["help",
                                                        "images=",
                                                        "segmentations=",
                                                        "output=",
                                                        "cmtk",
                                                        "dramms",
                                                        "skipRegistration",
                                                        "transforms=",
                                                        "existingTemplate=",
                                                        "dynamic",
                                                        "meanIterations=",
                                                        "fixed",
                                                        "template=",
                                                        "ignoreTemplateSegmentation",
                                                        "affine",
                                                        "affine12",
                                                        "non-rigid",
                                                        "writeTransforms",
                                                        "keepAligned",
                                                        "labels=",
                                                        "normalize",
                                                        "normalizeTo=",
                                                        "outputCast=",
                                                        "cluster",
                                                        "schedulerCommand=",
                                                        "numberOfThreads=",
                                                        "pca",
                                                        "pcaMaxEigenVectors=",
                                                        "pcaCombine",
                                                        "slicer=",
                                                        "debug",
                                                        "dryrun",
                                                        "testMode",
                                                        "examples"] )

    except getopt.GetoptError, err:
        usage()
        info( "Error: " + str( err ) )
        info( "" )
        sys.exit( 2 )

    # set some default switches
    skipRegistration = False
    slicerLauncherFilePath = None
    imagesDir = None
    segmentationsDir = None
    outputDir = None

    useCMTK = False

    useDRAMMS = False

    transformsDir = None
    existingTemplate = None

    dynamic = False
    meanIterations = 0

    fixed = False
    template = None
    ignoreTemplateSegmentation = False

    affine = False
    affine12 = False
    nonRigid = False

    writeTransforms = False

    keepAligned = False

    labels = None

    normalize = False

    normalizeTo = 1

    outputCast = 8

    cluster = False
    schedulerCommand = None

    numberOfThreads = -1

    pca = False
    pcaMaxEigenVectors = 10
    pcaCombine = False

    debug = False

    dryrun = False

    testMode = False

    for opt, arg in opts:
        if opt in ( "-h", "--help" ):
            usage()
            sys.exit()
        elif opt in ( "-i", "--images" ):
            imagesDir = arg
        elif opt in ( "-s", "--segmentations" ):
            segmentationsDir = arg
        elif opt in ( "-o", "--output" ):
            outputDir = arg
        elif opt in ( "--cmtk" ):
            useCMTK = True
        elif opt in ( "--dramms" ):
            useDRAMMS = True
        elif opt in ( "--skipRegistration" ):
            skipRegistration = True
        elif opt in ( "--transforms" ):
            transformsDir = arg
        elif opt in ( "--existingTemplate" ):
            existingTemplate = arg
        elif opt in ( "--dynamic" ):
            dynamic = True
        elif opt in ( "-m", "--meanIterations" ):
            meanIterations = arg
        elif opt in ( "--fixed" ):
            fixed = True
        elif opt in ( "--template" ):
            template = arg
        elif opt in ( "--ignoreTemplateSegmentation" ):
            ignoreTemplateSegmentation = True
        elif opt in ( "--affine" ):
            affine = True
        elif opt in ( "--affine12" ):
            affine12 = True
        elif opt in ( "-n", "--non-rigid" ):
            nonRigid = True
        elif opt in ( "-w", "--writeTransforms" ):
            writeTransforms = True
        elif opt in ( "--keepAligned" ):
            keepAligned = True
        elif opt in ( "-l", "--labels" ):
            labels = arg.strip().split( " " )
        elif opt in ( "--normalize" ):
            normalize = True
        elif opt in ( "--normalizeTo" ):
            normalize = True
            normalizeTo = arg
        elif opt in ( "--outputCast" ):
            outputCast = arg
        elif opt in ( "-c", "--cluster" ):
            cluster = True
        elif opt in ( "--schedulerCommand" ):
            schedulerCommand = arg
        elif opt in ( "--numberOfThreads" ):
            numberOfThreads = arg
        elif opt in ( "--pca" ):
            pca = True
        elif opt in ( "--pcaMaxEigenVectors" ):
            pca = True
            pcaMaxEigenVectors = arg
        elif opt in ( "--pcaCombine" ):
            pca = True
            pcaCombine = True
        elif opt in ( "--slicer" ):
            slicerLauncherFilePath = arg
        elif opt in ( "-d", "--debug" ):
            debug = True
        elif opt in ( "--dryrun" ):
            debug = True
            dryrun = True
        elif opt in ( "--testMode" ):
            testMode = True
            debug = True
        elif opt in ( "--examples" ):
            examples()
            sys.exit()
        else:
            assert False, "unhandled option"

    # now check if we have all we need
    errorOccured = False


    # we need 3D Slicer, find the launcher
    if not slicerLauncherFilePath:
        # try to find the slicer launcher automatically
        slicerLauncherFilePath = sys.path[0] + os.sep + "../../../../Slicer3"
        slicerLauncherFilePath = os.path.abspath( slicerLauncherFilePath )
        if os.path.isfile( slicerLauncherFilePath ):
            # running in bin directory
            info( "Found 3D Slicer launcher at " + str( slicerLauncherFilePath ) )
        else:
            slicerLauncherFilePath = sys.path[0] + os.sep + "../../../Slicer3-build/Slicer3"
            slicerLauncherFilePath = os.path.abspath( slicerLauncherFilePath )
            if os.path.isfile( slicerLauncherFilePath ):
                # running in src directory
                info( "Found 3D Slicer3 launcher at " + str( slicerLauncherFilePath ) )
            else:
                info( "Error: Could not find the 3D Slicer launcher!" )
                info( "Error: Try to specify the location with --slicer /path/Slicer3" )
                errorOccured = True
    else:
        info( "3D Slicer manually specified: " + str( slicerLauncherFilePath ) )


    # check if we have a valid images dir
    if not ( imagesDir and os.path.isdir( imagesDir ) ) and not skipRegistration:
        # no valid imagesDir and registration is not skipped
        # we have to abort
        info( "Error: Could not find the images!" )
        info( "Error: Location of --images is invalid: " + str( imagesDir ) )
        errorOccured = True
    elif imagesDir:
        imagesDir = os.path.abspath( imagesDir ) + os.sep

    if not ( segmentationsDir and os.path.isdir( segmentationsDir ) ):
        # no valid segmentationsDir
        # we have to abort
        info( "Error: Could not find the segmentations!" )
        info( "Error: Location of --segmentations is invalid: " + str( segmentationsDir ) )
        errorOccured = True
    elif segmentationsDir:
        segmentationsDir = os.path.abspath( segmentationsDir ) + os.sep

    if not outputDir:
        info( "Error: Location of --output is invalid or not a directory: " + str( outputDir ) )
        errorOccured = True

    # check if we have everything if skipRegistration is enabled
    if skipRegistration and transformsDir and existingTemplate:
        # check if transformDir and existingTemplate are not valid
        if not os.path.isdir( transformsDir ):
            # transformDir invalid
            info( "Error: Could not find the directory of existing transforms!" )
            info( "Error: Location of --transforms is invalid: " + str( transformsDir ) )
            errorOccured = True
        else:
            transformsDir = os.path.abspath( transformsDir ) + os.sep
        if not os.path.isfile( existingTemplate ):
            # existingTemplate invalid 
            info( "Error: Could not find the existing template!" )
            info( "Error: Location of --existingTemplate is invalid: " + str( existingTemplate ) )
            errorOccured = True
        else:
            existingTemplate = os.path.abspath( existingTemplate )
    elif skipRegistration:
        # we don't have everything, abort!
        info( "Error: To skip the registration, --transforms and --existingTemplate are required!" )
        info( "Error: Location of --transforms is invalid: " + str( transformsDir ) )
        info( "Error: Location of --existingTemplate is invalid: " + str( existingTemplate ) )
        errorOccured = True

    # check if either dynamic or fixed registration is configured
    if not dynamic and not fixed and not skipRegistration:
        info( "Error: The registration type was not set. Either --fixed or --dynamic are required!" )
        errorOccured = True

    # check if both, dynamic and fixed were configured
    if dynamic and fixed:
        info( "Error: Either --fixed or --dynamic are required - not both!" )
        errorOccured = True

    # check if we have everything if it is dynamic mode
    if dynamic and meanIterations:
        # check if meanIterations is valid
        if not is_int( meanIterations ) or not ( int( meanIterations ) >= 1 ):
            info( "Error: The value of --meanIterations has to be an INT greater than 1." )
            if not is_int( meanIterations ):
                info( "Error: Value of --meanIterations is invalid: NaN" )
            else:
                info( "Error: Value of --meanIterations is invalid: " + str( int( meanIterations ) ) )
            errorOccured = True
    elif dynamic:
        # we don't have everything, abort!
        info( "Error: For dynamic registration, --meanIterations is required!" )
        info( "Error: Value of --meanIterations is invalid: " + str( int( meanIterations ) ) )
        errorOccured = True

    # check if we have everything if it is fixed mode
    if fixed and template:
        # check if template is valid
        if not os.path.isfile( template ):
            # existingTemplate invalid 
            info( "Error: Could not find the template!" )
            info( "Error: Location of --template is invalid: " + str( template ) )
            errorOccured = True
        else:
            template = os.path.abspath( template )
    elif fixed:
        # we don't have everything, abort!
        info( "Error: For fixed registration, --template is required!" )
        info( "Error: Location of --template is invalid: " + str( template ) )
        errorOccured = True

    # check if at least one label was specified
    if labels and type( labels ).__name__ == 'list' and len( labels ) >= 1:
        # convert to integer list
        realLabels = []

        for l in labels:
            if is_int( l ):
                realLabels.append( int( l ) )

        if len( realLabels ) < 1:
            # invalid label list
            info( "Error: At least one label (INT) is required!" )
            info( "Error: Value of --labels: " + str( labels ) )
            errorOccured = True
        else:
            labels = list( set( realLabels ) )
    elif labels:
        info( "Error: At least one label (INT) is required!" )
        info( "Error: Value of --labels: " + str( labels ) )
        errorOccured = True

    # if normalizeTo was set, check if it is an integer
    if normalizeTo != 1 and not is_int( normalizeTo ):
        info( "Error: The normalizeTo value must be an integer." )
        info( "Error: Value of --normalizeTo: " + str( normalizeTo ) )
        errorOccured = True

    # check if pcaMaxEigenVectors is an integer
    if not is_int( pcaMaxEigenVectors ):
        info( "Error: The pcaMaxEigenVectors value must be an integer." )
        info( "Error: Value of --pcaMaxEigenVectors: " + str( pcaMaxEigenVectors ) )
        errorOccured = True


    # check if we have a valid outputCast
    if outputCast and is_int( outputCast ):

        if not ( int( outputCast ) > 0 and int( outputCast ) <= 9 ):
            info( "Error: The specified output cast is invalid - only values from 0-9 accepted." )
            info( "Error: Value of --outputCast: " + str( outputCast ) )
            errorOccured = True

        # valid outputCast integer, convert to string
        if int( outputCast ) == 0:
            outputCast = "char"
        elif int( outputCast ) == 1:
            outputCast = "unsigned char"
        elif int( outputCast ) == 2:
            outputCast = "double"
        elif int( outputCast ) == 3:
            outputCast = "float"
        elif int( outputCast ) == 4:
            outputCast = "int"
        elif int( outputCast ) == 5:
            outputCast = "unsigned int"
        elif int( outputCast ) == 6:
            outputCast = "long"
        elif int( outputCast ) == 7:
            outputCast = "unsigned long"
        elif int( outputCast ) == 8:
            outputCast = "short"
        elif int( outputCast ) == 9:
            outputCast = "unsigned short"

    elif outputCast:
        info( "Error: The specified output cast is invalid - only values from 0-9 accepted." )
        info( "Error: Value of --outputCast: " + str( outputCast ) )
        errorOccured = True

    # check if we have everything if cluster mode is activated
    if cluster and not schedulerCommand:
        info( "Error: In cluster mode, a schedulerCommand is required." )
        info( "Error: Value of --schedulerCommand: " + str( schedulerCommand ) )
        errorOccured = True

    if not is_int( numberOfThreads ):
        info( "Error: The number of threads must be an integer." )
        info( "Error: Value of --numberOfThreads: " + str( numberOfThreads ) )
        errorOccured = True

    if errorOccured:
        info( "" )
        info( "Try --help or --examples to understand the usage of the Atlas Creator." )
        info( "" )
        sys.exit( 2 )


    # lets create the --evalpython command!!
    evalpythonCommand = "from Slicer import slicer;"

    # we create a new vtkMRMLAtlasCreatorNode and configure it..
    evalpythonCommand += "n=slicer.vtkMRMLAtlasCreatorNode();"

    if debug or dryrun or testMode:
        evalpythonCommand += "n.SetDebugMode(1);"

    if dryrun:
        evalpythonCommand += "n.SetDryrunMode(1);"

    if testMode:
        evalpythonCommand += "n.SetTestMode(1);"

    # set special settings if clusterMode or skipRegistrationMode is requested
    if cluster:
        # cluster Mode
        evalpythonCommand += "n.SetUseCluster(1);"
        evalpythonCommand += "n.SetSchedulerCommand('" + schedulerCommand + "');"
    elif skipRegistration:
        # skipRegistration Mode
        evalpythonCommand += "n.SetSkipRegistration(1);"
        evalpythonCommand += "n.SetTransformsDirectory('" + transformsDir + "');"
        evalpythonCommand += "n.SetExistingTemplate('" + existingTemplate + "');"

    # now the configuration options which are valid for all
    if imagesDir:
        evalpythonCommand += "n.SetOriginalImagesFilePathList('" + ConvertDirectoryToString( imagesDir ) + "');"

    if segmentationsDir:
        evalpythonCommand += "n.SetSegmentationsFilePathList('" + ConvertDirectoryToString( segmentationsDir ) + "');"

    if outputDir:
        evalpythonCommand += "n.SetOutputDirectory('" + outputDir + "');"

    if useCMTK:
        evalpythonCommand += "n.SetToolkit('CMTK');"
    else:
        evalpythonCommand += "n.SetToolkit('BRAINSFit');"

    if fixed:
        evalpythonCommand += "n.SetTemplateType('fixed');"
        evalpythonCommand += "n.SetFixedTemplateDefaultCaseFilePath('" + template + "');"
        if ignoreTemplateSegmentation:
            evalpythonCommand += "n.SetIgnoreTemplateSegmentation(1);"
        else:
            evalpythonCommand += "n.SetIgnoreTemplateSegmentation(0);"
    else:
        evalpythonCommand += "n.SetTemplateType('dynamic');"
        evalpythonCommand += "n.SetDynamicTemplateIterations(" + str( meanIterations ) + ");"

    if labels:
        evalpythonCommand += "n.SetLabelsList('" + ConvertListToString( labels ) + "');"
    else:
        # activate label auto-detection
        evalpythonCommand += "n.SetLabelsList(None);"

    if nonRigid:
        evalpythonCommand += "n.SetRegistrationType('Non-Rigid');"
    elif affine12:
        evalpythonCommand += "n.SetRegistrationType('Affine12');"
    elif affine:
        evalpythonCommand += "n.SetRegistrationType('Affine');"
    else:
        evalpythonCommand += "n.SetRegistrationType('Rigid');"

    if writeTransforms:
        evalpythonCommand += "n.SetSaveTransforms(1);"
    else:
        evalpythonCommand += "n.SetSaveTransforms(0);"

    if keepAligned:
        evalpythonCommand += "n.SetDeleteAlignedImages(0);"
        evalpythonCommand += "n.SetDeleteAlignedSegmentations(0);"
    else:
        evalpythonCommand += "n.SetDeleteAlignedImages(1);"
        evalpythonCommand += "n.SetDeleteAlignedSegmentations(1);"

    if normalize:
        evalpythonCommand += "n.SetNormalizeAtlases(1);"
        evalpythonCommand += "n.SetNormalizeTo(" + str( normalizeTo ) + ");"
    else:
        evalpythonCommand += "n.SetNormalizeAtlases(0);"
        evalpythonCommand += "n.SetNormalizeTo(-1);"

    if pca:
        evalpythonCommand += "n.SetPCAAnalysis(1);"
        evalpythonCommand += "n.SetPCAMaxEigenVectors(" + str( pcaMaxEigenVectors ) + ");"
    else:
        evalpythonCommand += "n.SetPCAAnalysis(0);"
        evalpythonCommand += "n.SetPCAMaxEigenVectors(10);"

    if pcaCombine:
        evalpythonCommand += "n.SetPCACombine(1);"
    else:
        evalpythonCommand += "n.SetPCACombine(0);"

    if useDRAMMS:
        evalpythonCommand += "n.SetUseDRAMMS(1);"
    else:
        evalpythonCommand += "n.SetUseDRAMMS(0);"

    evalpythonCommand += "n.SetNumberOfThreads(" + str( numberOfThreads ) + ");"

    evalpythonCommand += "n.SetOutputCast('" + outputCast + "');"

    # add the new node to the MRML scene
    evalpythonCommand += "slicer.MRMLScene.AddNode(n);"
    evalpythonCommand += "n.Launch();"


    command = slicerLauncherFilePath + ' --no_splash --evalpython "' + evalpythonCommand + '"'

    # now run the command
    try:
        r = call( command, shell=True )
    except OSError, e:
        self.info( "Execution failed " + str( e ) )



'''=========================================================================================='''
if __name__ == "__main__":
    main( sys.argv[1:] )

