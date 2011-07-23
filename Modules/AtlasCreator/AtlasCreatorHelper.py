from Slicer import slicer
from math import floor
from subprocess import call
from time import strftime
import glob
import os
import shutil
import sys

class AtlasCreatorHelper( object ):
    '''
        The helper class for the Atlas Creator
    '''



    '''=========================================================================================='''
    def __init__( self, parentClass ):
        '''
            Initialize this class
            
            parentClass
                a pointer to the parentClass
                
            Returns
                n/a
        '''

        self.__parentClass = parentClass

        # deactivated by default
        self.__debugMode = 0
        self.__guidebugMode = 0

        # create one instance of the AtlasCreatorCxx logic
        self.__cxxlogic = slicer.vtkAtlasCreatorCxxModuleLogic()

        self.__pcaDistanceSourced = 0

        self.__testMode = 0



    '''=========================================================================================='''
    def Destructor( self ):
        '''
            Destruct this class
            
            Returns
                n/a
        '''

        self.__cxxlogic = None
        self.__parentClass = None
        self.__debugMode = None
        self.__guidebugMode = None
        self.__pcaDistanceSourced = None
        self.__testMode = None



    '''=========================================================================================='''
    def EnableTestMode( self ):
        '''
        Enables the test Mode which means a fast but not accurate registration
        
        Returns n/a
        '''
        self.__testMode = 1



    '''=========================================================================================='''
    def EnableDebugMode( self ):
        '''
            Enables the debug Mode
            
            Returns
                n/a
        '''
        self.__debugMode = 1



    '''=========================================================================================='''
    def debug( self, message ):
        ''' 
            Print statement to stdout including a timestamp and a debug label
            
            message
                value to print, will be casted to String
            
            Returns
                n/a
        '''

        if self.__debugMode:

            print "[AtlasCreator " + strftime( "%m/%d/%Y %H:%M:%S" ) + "] DEBUG: " + str( message )

            # flush, to always show the output
            try:
                sys.stdout.flush()
            except:
                pass



    '''=========================================================================================='''
    def guiDebug( self, message ):
        ''' 
            Print statement to stdout including a timestamp and a GUI debug label
            
            message
                value to print, will be casted to String
            
            Returns
                n/a
        '''

        if self.__guidebugMode:

            print "[AtlasCreator " + strftime( "%m/%d/%Y %H:%M:%S" ) + "] GUI DEBUG: " + str( message )

            # flush, to always show the output
            try:
                sys.stdout.flush()
            except:
                pass



    '''=========================================================================================='''
    def info( self, message ):
        ''' 
            Print statement to stdout including a timestamp
            
            message
                value to print, will be casted to String
            
            Returns
                n/a
        '''
        # activated by default
        infoMode = 1

        if infoMode:

            print "[AtlasCreator " + strftime( "%m/%d/%Y %H:%M:%S" ) + "] " + str( message )

            # flush, to always show the output
            try:
                sys.stdout.flush()
            except:
                pass



    '''=========================================================================================='''
    def DeleteFilesAndDirectory( self, filePathList, wipe=False ):
        ''' 
            Delete all files of a filePathList and also the directory
            
            filePathList
                the list of file paths to delete
            wipe
                if TRUE, wipe it without asking questions!
            
            Returns
                TRUE or FALSE depending on success
        '''

        # if we want to wipe something, just do it!
        if wipe:
            if os.path.isdir( os.path.dirname( filePathList[0] ) ):
                shutil.rmtree( os.path.dirname( filePathList[0] ), True, None )
                return True

        # now delete the content in the temporary directory
        for file in filePathList:
            if os.path.isfile( file ):
                os.remove( file )
            else:
                return False

        # now delete the whole temporary directory
        if os.path.isdir( os.path.dirname( filePathList[0] ) ):
            os.rmdir( os.path.dirname( filePathList[0] ) )
        else:
            return False


        return True



    '''=========================================================================================='''
    def GetSlicerLaunchPrefix( self, useCMTK=False, justLauncher=False ):
        '''
            Get the path to the 3D Slicer launcher ready to start a plugin. (OS independent)
            
            useCMTK
                If TRUE, add the CMTK directory instead of the Plugin Directory
            justLauncher
                If TRUE, just return the path to the Slicer launcher
            
            Returns
                the path to the 3D Slicer launcher configured to start a plugin as a String
                f.e. /home/user/Slicer3-build/Slicer3 --launch /home/user/Slicer3-build/lib/Slicer3/Plugins/
                If useCMTK is TRUE:
                f.e. /home/user/Slicer3-build/Slicer3 --launch /home/user/Slicer3user/@SVNREV@/CMTK4Slicer
                pay attention to the trailing /
        '''
        # read the Slicer environment
        slicerDir = str( slicer.Application.GetBinDir() ) + os.sep + ".." + os.sep
        slicerLauncher = os.path.normpath( slicerDir + "Slicer3" )

        if not os.path.isfile( slicerLauncher ):
            self.info( "ERROR! Could not find Slicer3 launcher.. Aborting.." )
            return None

        if justLauncher:
            return slicerLauncher

        self.info( "Found 3D Slicer launcher at " + str( slicerLauncher ) )

        if useCMTK:
            executableDir = os.path.normpath( self.GetCMTKInstallationDirectory() )
        else:
            executableDir = os.path.normpath( slicer.Application.GetPluginsDir() )

        if not os.path.isdir( executableDir ):
            self.info( "ERROR! Could not find Executable directory.. Aborting.." )
            self.info( "ERROR! Path not found: " + str( executableDir ) )
            return None


        launchCommandPrefix = slicerLauncher + " --launch " + executableDir + os.sep
        self.debug( "The launchCommandPrefix is " + str( launchCommandPrefix ) )

        return launchCommandPrefix



    '''=========================================================================================='''
    def GetSlicerTemporaryDirectory( self ):
        '''
            Get the path to the 3D Slicer temporary directory
            
            Returns
                the path to the 3D Slicer temporary directory as a String
                pay attention to the trailing /
        '''
        return os.path.normpath( str( slicer.Application.GetTemporaryDirectory() ) ) + os.sep



    '''=========================================================================================='''
    def BinarizeImageByLabel( self, imageData, labelValue ):
        ''' 
            Binarize an image to 0 and 1, while just keeping the voxels with the labelValue
            
            imageData
                vtkImageData
            labelValue
                the value of the voxels to keep
            
            Returns
                vtkImageData after binarizing
        '''

        threshold = slicer.vtkImageThreshold()
        threshold.SetInput( imageData )
        threshold.ThresholdBetween( labelValue, labelValue )
        threshold.ReplaceInOn()
        threshold.ReplaceOutOn()
        threshold.SetInValue( 1 )
        threshold.SetOutValue( 0 )
        threshold.Update()

        output = slicer.vtkImageData()
        output.DeepCopy( threshold.GetOutput() )

        return output



    '''=========================================================================================='''
    def DivideImage( self, imageData, dividend ):
        ''' 
            Divide an image
            This includes automatic Re-Cast to Float
            
            imageData
                vtkImageData
            dividend
                the number to divide with, will be casted to float
            
            Returns
                vtkImageData after division
        '''

        return self.MultiplyImage( imageData, 1.0 / float( dividend ) )



    '''=========================================================================================='''
    def MultiplyImage( self, imageData, factor ):
        ''' 
            Multiply an image
            This includes automatic Re-Cast to Float
            
            imageData
                vtkImageData
            dividend
                the number to divide with, will be casted to float
            
            Returns
                vtkImageData after multiplication
        '''
        imageData.DeepCopy( self.ReCastImage( imageData, "Float" ) )
        mul = slicer.vtkImageMathematics()
        mul.SetInput( imageData )
        mul.SetOperationToMultiplyByK()
        mul.SetConstantK( factor )
        mul.Update()

        output = slicer.vtkImageData()
        output.DeepCopy( mul.GetOutput() )

        return output



    '''=========================================================================================='''
    def AddImages( self, imageData1, imageData2 ):
        ''' 
            Adds two images
            
            imageData1
                vtkImageData
            imageData2
                vtkImageData
            
            Returns
                vtkImageData after addition
        '''
        add = slicer.vtkImageMathematics()
        add.SetInput1( imageData1 )
        add.SetInput2( imageData2 )
        add.SetOperationToAdd()
        add.Update()

        output = slicer.vtkImageData()
        output.DeepCopy( add.GetOutput() )

        return output



    '''=========================================================================================='''
    def ReCastImage( self, imageData, castString ):
        ''' 
            Re-Cast an image to a different type
            
            imageData
                vtkImageData
            castString
                New type as a String, possible values are
                "Char"
                "Unsigned Char"
                "Double"
                "Float"
                "Int"
                "Unsigned Int"
                "Long"
                "Unsigned Long"
                "Short"
                "Unsigned Short"
                other values will result in "Short"
                
            Returns
                vtkImageData after Re-Cast
        '''

        # get the current type
        currentType = imageData.GetScalarType()

        cast = slicer.vtkImageCast()
        cast.SetInput( imageData )
        cast.ClampOverflowOn()

        needToCast = False

        if ( castString == "Char" and ( currentType != 2 or currentType != 15 ) ):
            cast.SetOutputScalarTypeToChar()
            needToCast = True
        elif ( castString == "Unsigned Char" and currentType != 3 ):
            cast.SetOutputScalarTypeToUnsignedChar()
            needToCast = True
        elif ( castString == "Double" and currentType != 11 ):
            cast.SetOutputScalarTypeToUnsignedChar()
            needToCast = True
        elif ( castString == "Float" and currentType != 10 ):
            cast.SetOutputScalarTypeToFloat()
            needToCast = True
        elif ( castString == "Int" and currentType != 6 ):
            cast.SetOutputScalarTypeToInt()
            needToCast = True
        elif ( castString == "Unsigned Int" and currentType != 7 ):
            cast.SetOutputScalarTypeToUnsignedInt()
            needToCast = True
        elif ( castString == "Long" and currentType != 8 ):
            cast.SetOutputScalarTypeToLong()
            needToCast = True
        elif ( castString == "Unsigned Long" and currentType != 9 ):
            cast.SetOutputScalarTypeToUnsignedLong()
            needToCast = True
        elif ( castString == "Short" and currentType != 4 ):
            cast.SetOutputScalarTypeToShort()
            needToCast = True
        elif ( castString == "Unsigned Short" and currentType != 5 ):
            cast.SetOutputScalarTypeToUnsignedShort()
            needToCast = True
        else:
            cast.SetOutputScalarTypeToShort()
            needToCast = True

        # check if we really need to cast,
        # else exit here and return the input imageData
        if not needToCast:
            return imageData

        cast.Update()

        output = slicer.vtkImageData()
        output.DeepCopy( cast.GetOutput() )

        return output



    '''=========================================================================================='''
    def GetLabels( self, labelMap, includeZero=False ):
        '''
            Get the list of labels in a labelMap
            
            labelMap
                vtkImageData of a labelMap volume
            includeZero
                determines if to include 0 in the list of labels
            
            Returns
                the list of labels
        '''
        accum = slicer.vtkImageAccumulate()
        accum.SetInput( labelMap )
        accum.UpdateWholeExtent()
        accum.Update()
        data = accum.GetOutput()
        numBins = accum.GetComponentExtent()[1]
        nonZeroLabels = []
        if includeZero:
            nonZeroLabels.append( 0 )
        for i in range( 1, numBins + 1 ):
            numVoxels = data.GetScalarComponentAsDouble( i, 0, 0, 0 )
            if ( numVoxels > 0 ):
                nonZeroLabels.append( i )

        self.debug( "Found the following labels: " + str( nonZeroLabels ) )

        accum = None

        return nonZeroLabels



    '''=========================================================================================='''
    def ReadLabelsFromImages( self, pathsList ):
        '''
        Read labels from an image.
        
        pathsList - a list of filePaths to existing Images
        
        Returns a list of labels
        '''
        if not pathsList:
            return None

        labels = []

        for f in pathsList:

            node = self.LoadVolume( os.path.normpath( f ) )

            # by default, do not include 0 in the list of labels
            bg = self.GuessBackgroundValue( os.path.normpath( f ) )
            includeZero = False

            # if the background of the labelmap is 0, include it in the labelList
            if bg == 0:
                includeZero = True

            newLabels = self.GetLabels( node.GetImageData(), includeZero )
            slicer.MRMLScene.RemoveNode( node )

            # now sync newLabels with labels
            labels = list( set( sorted( newLabels + labels ) ) )

        return labels



    '''=========================================================================================='''
    def GetBRAINSFitRegistrationCommand( self, templateFilePath, movingImageFilePath, outputTransformFilePath, outputImageFilePath, numberOfThreads, backgroundValue, affine, affine12, nonrigid ):
        '''
            Get the command to Register an image to a template using BRAINSFit
            
            templateFilePath
                the file path to the template (target) image
            movingImageFilePath
                the file path to the moving image
            outputTransformFilePath
                the file path to the transformation output
            outputImageFilePath
                the file path to the aligned image output
            numberOfThreads
                the number of threads to use or -1 for the maximal number of threads
            backgroundValue
                the backgroundValue of the moving image
            affine
                TRUE if affine registration should be used
            affine12
                TRUE if affine registration with 12 DOF should be used
            nonrigid
                TRUE if nonrigid registration should be used
                
            Returns
                the command to Register an image
        '''

        registrationCommand = "BRAINSFit"
        registrationCommand += " --fixedVolume " + os.path.normpath( templateFilePath )
        registrationCommand += " --movingVolume " + os.path.normpath( movingImageFilePath )
        registrationCommand += " --outputTransform " + os.path.normpath( outputTransformFilePath )
        registrationCommand += " --outputVolume " + os.path.normpath( outputImageFilePath )
        registrationCommand += " --maxBSplineDisplacement 10.0 --outputVolumePixelType short --interpolationMode Linear"
        #registrationCommand += " --maskProcessingMode  ROIAUTO --ROIAutoDilateSize 3.0 --maskInferiorCutOffFromCenter 65.0"
        #registrationCommand += " --useRigid --useScaleVersor3D --useScaleSkewVersor3D"
        registrationCommand += " --initializeTransformMode useCenterOfHeadAlign"
        registrationCommand += " --useRigid"

        # activate different deformationTypes
        if affine:
            registrationCommand += " --useScaleVersor3D"

        if affine12:
            registrationCommand += " --useScaleSkewVersor3D"

        if nonrigid:
            registrationCommand += " --useBSpline"

        # affine with 16 DOF is not supported right now    
        #registrationCommand += " --useAffine"

        registrationCommand += " --debugNumberOfThreads " + str( numberOfThreads )
        registrationCommand += " --backgroundFillValue " + str( backgroundValue )

        # in test mode, use limited DOF
        if self.__testMode:
            registrationCommand += " --numberOfSamples 100 --numberOfIterations 3 --projectedGradientTolerance 1e-1  --splineGridSize 7,5,12"
        else:
            registrationCommand += " --numberOfSamples 100000 --numberOfIterations 1500 --projectedGradientTolerance 1e-5  --splineGridSize 28,20,24"

        registrationCommand += " --translationScale 1000.0 --reproportionScale 1.0 --skewScale 1.0 --fixedVolumeTimeIndex 0"
        registrationCommand += " --movingVolumeTimeIndex 0 --medianFilterSize 0,0,0 --numberOfHistogramBins 50 --numberOfMatchPoints 10 --useCachingOfBSplineWeightsMode ON"
        registrationCommand += " --useExplicitPDFDerivativesMode AUTO --relaxationFactor 0.5 --failureExitCode -1"
        registrationCommand += " --debugLevel 0 --costFunctionConvergenceFactor 1e+9"


        return str( registrationCommand )



    '''=========================================================================================='''
    def GetBRAINSFitResampleCommand( self, segmentationFilePath, templateFilePath, transformFilePath, outputSegmentationFilePath, backgroundValue ):
        '''
            Get the command to Resample a segmentation using BRAINSFit
            
            segmentationFilePath
                the file path to the segmentation
            templateFilePath
                the file path to the template used to define the output space
            transformFilePath
                the file path to the existing transformation          
            outputSegmentationFilePath
                the file path to the resampled segmentation output
            backgroundValue
                the backgroundValue of the segmentation                
                
            Returns
                the command to Resample a segmentation
        '''
        resampleCommand = "BRAINSResample"
        resampleCommand += " --inputVolume " + os.path.normpath( segmentationFilePath )
        resampleCommand += " --referenceVolume " + os.path.normpath( templateFilePath )
        resampleCommand += " --warpTransform " + os.path.normpath( transformFilePath )
        resampleCommand += " --outputVolume " + os.path.normpath( outputSegmentationFilePath )
        resampleCommand += " --defaultValue " + str( backgroundValue )
        resampleCommand += " --pixelType short --interpolationMode NearestNeighbor"

        return str( resampleCommand )



    '''=========================================================================================='''
    def GetCMTKAffineRegistrationCommand( self, templateFilePath, movingImageFilePath, outputTransformDirectory, outputImageFilePath, backgroundValue, backgroundValueTemplate, affine, affine12 ):
        '''
            Get the command to Register an image to a template using CMTK
            
            templateFilePath
                the file path to the template (target) image
            movingImageFilePath
                the file path to the moving image
            outputTransformDirectory
                the file path to the directory for transformation output
            outputImageFilePath
                the file path to the aligned image output
            backgroundValue
                the backgroundValue of the moving image
            backgroundValueTemplae
                the backgroundValue of the template
            affine
                TRUE if affine registration should be used
            affine12
                TRUE if affine registration with 12 DOF should be used
                
            Returns
                the command to Register an image using CMTK in Affine mode
        '''


        registrationCommand = "registration"
        registrationCommand += " --initxlate --exploration 8.0 --accuracy 0.5"
        registrationCommand += " --dofs 6"

        if affine:
            registrationCommand += " --dofs 9"

        if affine12:
            registrationCommand += " --dofs 12"

        registrationCommand += " -o " + os.path.normpath( outputTransformDirectory )
        registrationCommand += " --pad-ref " + str( backgroundValueTemplate )
        registrationCommand += " --pad-flt " + str( backgroundValue )
        registrationCommand += " --write-reformatted " + os.path.normpath( outputImageFilePath )
        registrationCommand += " " + os.path.normpath( templateFilePath )
        registrationCommand += " " + os.path.normpath( movingImageFilePath )

        return str( registrationCommand )



    '''=========================================================================================='''
    def GetCMTKNonRigidRegistrationCommand( self, templateFilePath, movingImageFilePath, outputTransformDirectory, outputImageFilePath, backgroundValue, backgroundValueTemplate ):
        '''
            Get the command to Register an image to a template using CMTK
            
            templateFilePath
                the file path to the template (target) image
            movingImageFilePath
                the file path to the moving image
            outputTransformDirectory
                the file path to the directory for transformation output, here it will be also used as input for the affine transformation
            outputImageFilePath
                the file path to the aligned image output
            backgroundValue
                the backgroundValue of the moving image
            backgroundValueTemplae
                the backgroundValue of the template                
                
            Returns
                the command to Register an image using CMTK in NonRigid mode
        '''


        registrationCommand = "warp"
        registrationCommand += " --fast --grid-spacing 40 --refine 1 --energy-weight 5e-2 --accuracy 1 --coarsest 6"
        registrationCommand += " --initial " + os.path.normpath( outputTransformDirectory )
        registrationCommand += " -o " + os.path.normpath( outputTransformDirectory )
        registrationCommand += " --pad-ref " + str( backgroundValueTemplate )
        registrationCommand += " --pad-flt " + str( backgroundValue )
        registrationCommand += " --write-reformatted " + os.path.normpath( outputImageFilePath )
        registrationCommand += " " + os.path.normpath( templateFilePath )
        registrationCommand += " " + os.path.normpath( movingImageFilePath )

        return str( registrationCommand )



    '''=========================================================================================='''
    def GetCMTKResampleCommand( self, segmentationFilePath, templateFilePath, transformDirectory, outputSegmentationFilePath, backgroundValue ):
        '''
            Get the command to Resample a segmentation using CMTK
            
            segmentationFilePath
                the file path to the segmentation
            templateFilePath
                the file path to the template used to define the output space
            transformDirectory
                the file path to the existing transformation directory         
            outputSegmentationFilePath
                the file path to the resampled segmentation output
            backgroundValue
                the backgroundValue of the segmentation                
            
            Returns
                the command to Resample a segmentation
        '''

        resampleCommand = "reformatx"
        resampleCommand += " -o " + os.path.normpath( outputSegmentationFilePath )
        resampleCommand += " --nn --short --pad-out " + str( backgroundValue )
        resampleCommand += " --floating " + os.path.normpath( segmentationFilePath )
        resampleCommand += " " + os.path.normpath( templateFilePath )
        resampleCommand += " " + os.path.normpath( transformDirectory )

        return str( resampleCommand )



    '''=========================================================================================='''
    def GetDRAMMSRegistrationCommand( self, templateFilePath, movingImageFilePath, outputAlignedImageFilePath, outputDeformationFieldFilePath ):
      '''
          Get the command to Register using DRAMMS
          
          templateFilePath
              the file path to the template (target) image
          movingImageFilePath
              the file path to the moving image
          outputAlignedImageFilePath
              the file path to save the registered image
          outputDeformationFieldFilePath
              the file path to save the generated deformation field
      
          Returns
              the command to Register
      '''

      registrationCommand = "DRAMMS3D.sh"
      registrationCommand += " " + os.path.normpath( movingImageFilePath )
      registrationCommand += " " + os.path.normpath( templateFilePath )
      registrationCommand += " " + os.path.normpath( outputAlignedImageFilePath )
      registrationCommand += " " + os.path.normpath( outputDeformationFieldFilePath )

      return registrationCommand



    '''=========================================================================================='''
    def GetDRAMMSResampleCommand( self, segmentationFilePath, deformationFieldFilePath, outputSegmentationFilePath, sizeX, sizeY, dataType="f'" ):
      '''
          Get the command to Resample using DRAMMS
          
          segmentationFilePath
              the file path to the existing segmentation
          deformationFieldFilePath
              the file path to the existing deformation field
          outputSegmentationFilePath
              the output file path to save the resampled image
          sizeX
              the dimensions in X
          sizeY
              the dimensions in Y
          dataType
              the dataType of the images, DEFAULT: f' for float
      
          Returns
              the command to Resample
      '''

      resamplingCommand = "LinuxApplyDeformationOnIntensityImg"
      resamplingCommand += " " + deformationFieldFilePath
      resamplingCommand += " " + segmentationFilePath
      resamplingCommand += " " + outputSegmentationFilePath
      resamplingCommand += " -v" + str( sizeX ) + "," + str( sizeY )
      resamplingCommand += " -s" + str( sizeX ) + "," + str( sizeY )
      resamplingCommand += " -f'" + str( dataType )

      return resamplingCommand



    '''=========================================================================================='''
    def GetSaveDeformationFieldCommand( self, movingImageFilePath, transformFilePath, outputDeformationFieldFilePath ):
        '''
            Get the command to Save a deformation field
            
            movingImageFilePath
                the file path to the moving image used as the reference image
            transformFilePath
                the file path to the existing BSpline transformation
            outputDeformationFieldFilePath
                the file path to the Deformation Field output
                
            Returns
                the command to save a deformation field
        '''

        saveDefFieldCommand = "BSplineToDeformationField"
        saveDefFieldCommand += " --refImage " + os.path.normpath( movingImageFilePath )
        saveDefFieldCommand += " --tfm " + os.path.normpath( transformFilePath )
        saveDefFieldCommand += " --defImage " + os.path.normpath( outputDeformationFieldFilePath )

        return str( saveDefFieldCommand )



    '''=========================================================================================='''
    def LoadVolume( self, filePath ):
        '''
            Loads an image as a MRMLScalarVolumeNode and adds it to the MRML Scene
            
            filePath
                the file path to an existing image in any 3D Slicer readable format
            
            Returns
                vtkMRMLScalarVolumeNode containing the loaded image and meta information
        '''
        # if we do not have a filePath, exit and return None
        if not filePath:
            return None

        filePath = os.path.normpath( filePath )

        # load the file using Slicer's mechanism
        try:
            volumeNode = slicer.VolumesGUI.GetLogic().AddArchetypeScalarVolume( filePath, "tmpAtlasCreatorNode" )
        except Exception:
            # special case, if the AC is called from the EMSegment commandline tool
            volumeNode = self.__cxxlogic.AddArchetypeScalarVolume( filePath, "tmpAtlasCreatorNode", slicer.ApplicationLogic, slicer.MRMLScene )


        return volumeNode



    '''=========================================================================================='''
    def SaveVolume( self, filePath, volumeNode ):
        '''
            Saves a MRMLScalarVolumeNode
            
            filePath
                the file path to save a MRMLScalarVolumeNode in any 3D Slicer readable format
            
            volumeNode
                vtkMRMLScalarVolumeNode to save
            
            Returns
                TRUE or FALSE depending on success
        '''
        # if we do not have a filePath, exit and return None
        if not filePath:
            return False

        filePath = os.path.normpath( filePath )

        try:
            success = slicer.VolumesGUI.GetLogic().SaveArchetypeVolume( filePath, volumeNode )
        except Exception:
            success = False

        return success



    '''=========================================================================================='''
    def ConvertDirectoryToList( self, directory ):
        '''
            Convert a directory to a list of filePaths. While reading the directory,
            only real files are added to the list, sub-directories as well as links are ignored.
            
            directory
                a String containing the path to the directory
                
            Returns
                list of filePaths
        '''
        listOfFilePaths = []

        if not directory:
            return listOfFilePaths

        if not os.path.isdir( directory ):
            return listOfFilePaths

        # loop through the directory
        for entry in glob.glob( os.path.join( directory, '*.*' ) ):

            if os.path.isfile( entry ) and not os.path.islink( entry ):

                # this is a real file and not a link or subdir
                # add it to the list
                listOfFilePaths.append( os.path.normpath( entry ) )

        return listOfFilePaths



    '''=========================================================================================='''
    def ConvertDirectoryToString( self, directory ):
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
    def ConvertListToString( self, list ):
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
    def GetValidFilePaths( self, list1, list2 ):
        '''
            Returns list1 without any files which baseNames are not in list2.
            
            list1, list2
                list of filePaths
                
            Returns
                list1 without invalid entries
        '''
        for i in list1:
            caseNameInList1 = os.path.basename( i )

            isInBothLists = False

            for j in list2:

                caseNameInList2 = os.path.basename( j )

                if caseNameInList1 == caseNameInList2:
                    isInBothLists = True
                    break

            # remove the item if it is not in both lists
            if not isInBothLists:
                list1.remove( i )

        return list1



    '''=========================================================================================='''
    def DisplayImageInSlicer( self, filePath, nodeName ):
        '''
            Displays an image in 3D Slicer and re-calculates Window/Level on the displayNode
            
            filePath
                the file path to an existing image in any 3D Slicer readable format
                
            nodeName
                the name of the new volume Node
            
            Returns
                The ID of the MRML node as a String or None on failure
        '''
        # if we do not have a filePath, exit and return None
        if not filePath:
            return None

        if not nodeName:
            nodeName = "AtlasCreatorNode"

        filePath = os.path.normpath( filePath )

        if not os.path.exists( filePath ):
            return None

        # load the file using Slicer's mechanism
        volumeNode = slicer.VolumesGUI.GetLogic().AddArchetypeScalarVolume( filePath, nodeName )

        # get the displayNode to re-calculate Window/Level
        displayNode = volumeNode.GetDisplayNode()
        if displayNode:
            displayNode.AutoWindowLevelOff()
            displayNode.AutoWindowLevelOn()

        return str( volumeNode.GetID() )



    '''=========================================================================================='''
    def GuessBackgroundValue( self, filePath ):
        '''
            Guesses the background value for a given image
            
            filePath
                the filepath to an existing image
            
            Returns
                The guess for a background value
        '''


        # load the image
        node = self.LoadVolume( filePath )

        # guess the bg value
        guess = self.__cxxlogic.GuessRegistrationBackgroundLevel( node )

        # remove the node from the scene
        slicer.MRMLScene.RemoveNode( node )

        # and return the bg value
        return guess



    '''=========================================================================================='''
    def SourceTclFile( self, filePath ):
        '''
            Sources a TclFile
            
            filePath
                the filepath to an existing tcl file
            
            Returns
                n/a
        '''
        slicer.Application.LoadScript( filePath )



    '''=========================================================================================='''
    def PCAGenerateDistanceMaps( self, imageFilePath, label, outputFilePath, dryRun=0, maxDist=100, distBound=10 ):
        '''
            Calls the PCADistance function
           
            imageFilePath
                filePath to an existing image
            label
                Label to use for PCA
            outputFilePath
                output filePath to the generated distance map
            
           
            Returns
                n/a
        '''
        pathToAtlasCreator = os.path.normpath( str( slicer.Application.GetPluginsDir() ) + '/../Modules/AtlasCreator' )

        if not self.__pcaDistanceSourced:
            # only source the tcl files once
            self.SourceTclFile( pathToAtlasCreator + os.sep + "HelperFct.tcl" )
            self.SourceTclFile( pathToAtlasCreator + os.sep + "PCA_ModellingFct.tcl" )
            self.__pcaDistanceSourced = 1

        output = "GenerateDistanceMapWrapper"
        output += " " + str( imageFilePath )
        output += " " + str( label )
        output += " " + str( maxDist )
        output += " " + str( distBound )
        output += " " + str( outputFilePath )

        self.debug( "Invoking TCL code: " + output )

        if not dryRun:
            slicer.TkCall( output )
        else:
            self.info( "Skipping execution of GeneratePCAModel.." )



    '''=========================================================================================='''
    def PCAGenerateModel( self, pcaDirectory, labelList, dryRun=0, maxNumberOfEigenVectors=10, saveFileFormat="nrrd", combineStructures=0 ):
        '''
            Calls the PCAGenerate function
           
            pcaDirectory
                filePath to a directory containing appropriate distanceMaps
            labelList
                list of labels to use for PCA
           
            Returns
               n/a
        '''
        pathToAtlasCreator = os.path.normpath( str( slicer.Application.GetPluginsDir() ) + '/../Modules/AtlasCreator' )

        if not self.__pcaDistanceSourced:
            # only source the tcl files once
            self.SourceTclFile( pathToAtlasCreator + os.sep + "HelperFct.tcl" )
            self.SourceTclFile( pathToAtlasCreator + os.sep + "PCA_ModellingFct.tcl" )
            self.__pcaDistanceSourced = 1

        # convert labelList to a huge string with space as delimiter
        labelListAsString = ""

        for label in labelList:
            labelListAsString += str( label )
            labelListAsString += ","

        labelListAsString = labelListAsString.rstrip( "," )

        output1 = "set ::acLabelListAsString " + labelListAsString

        # set the variable for the labelList
        if not dryRun:
            slicer.TkCall( output1 )

        # we use the wrapper to start the generation of PCA models
        # we pass all arguments but the labelList which gets handled using the global variable,
        # since we can not pass space divided lists via the Slicer Python<->Tcl interface!!
        output2 = "GeneratePCAModelWrapper"
        output2 += " " + str( pcaDirectory )
        output2 += " " + str( maxNumberOfEigenVectors )
        output2 += " " + str( saveFileFormat )
        output2 += " " + str( combineStructures )

        self.debug( "Invoking TCL code: " + output1 )
        self.debug( "Invoking TCL code: " + output2 )

        if not dryRun:
            slicer.TkCall( output2 )
        else:
            self.info( "Skipping execution of GeneratePCAModel.." )



    '''=========================================================================================='''
    def GetCMTKInstallationDirectory( self ):
        '''
            Return the installation directory of the CMTK extension.
            
            Returns
                The installation directory as String or None if CMTK was not found
        '''
        currentSVNRevision = slicer.Application.GetSvnRevision()
        extensionsDirForCurrentRev = os.path.join( str( slicer.Application.GetExtensionsInstallPath() ), str( currentSVNRevision ) )
        self.debug( "Looking for CMTK in " + str( extensionsDirForCurrentRev ) )

        if not os.path.isdir( slicer.Application.GetExtensionsInstallPath() ):
            # extension directory does not exist
            return None

        cmtkDir = os.path.join( extensionsDirForCurrentRev, "CMTK4Slicer" )
        if not os.path.isdir( cmtkDir ):
            # CMTK directory for current version of Slicer does not exist
            # let's look for older versions in the last 100 revisions
            topLevelExtensionFolder = str( slicer.Application.GetExtensionsInstallPath() )

            # now check if any of the revisions before had CMTK
            # TODO go through all revisions
            for i in range( 1, currentSVNRevision ):

                rev = currentSVNRevision - i

                extensionsDirForFormerRev = os.path.join( str( slicer.Application.GetExtensionsInstallPath() ), str( rev ) )

                if os.path.isdir( extensionsDirForFormerRev ):
                    # found a folder for a former revision

                    # now check if there is CMTK present
                    potentialCmtkDir = os.path.join( extensionsDirForFormerRev, "CMTK4Slicer" )
                    if os.path.isdir( potentialCmtkDir ):
                        # we found an old CMTK, let's try it
                        cmtkDir = potentialCmtkDir
                        break

        # check if the cmtkDir really has a registration tool
        if not os.path.isfile( os.path.join( cmtkDir, "registration" ) ):
            # CMTK registration command not found
            return None

        # CMTK seems to be available
        return cmtkDir



    '''=========================================================================================='''
    def CreateScript( self, commands, notify, notifyError="echo 1" ):
        '''
            Return a script based on template.sh where commands and notify were inserted properly.
            
            commands - A string containing the execution commands.
            notify - A string containing commands to execute when the last command executed successfully
            notifyError - A string containing commands to execute when the last command failed
            
                
            Returns
                The script where the arguments were inserted properly.
        '''
        pathToAtlasCreator = os.path.normpath( str( slicer.Application.GetPluginsDir() ) + '/../Modules/AtlasCreator' )

        with open( os.path.join( pathToAtlasCreator, 'template.sh' ), 'r' ) as f:

            content = f.read()

        content = content.replace( '############{$COMMANDS}############', commands, 1 )

        notifyWithErrorProof = "RETVAL=$?" + "\n"
        notifyWithErrorProof += "if [ $RETVAL -eq 0 ]" + "\n"
        notifyWithErrorProof += "  then" + "\n"
        notifyWithErrorProof += "    " + notify + "\n"
        notifyWithErrorProof += "  else" + "\n"
        notifyWithErrorProof += "    " + notifyError + "\n"
        notifyWithErrorProof += "fi" + "\n"

        content = content.replace( '#############{$NOTIFY}#############', notifyWithErrorProof, 1 )

        return content



    '''=========================================================================================='''
    def Execute( self, command ):
        '''
            Execute a given command and print runtime information.
            
            command
                A string containing the command to be executed
                
            Returns
                n/a
        '''
        try:
            r = call( command, shell=True )
            if r < 0:
                self.debug( "Command terminated by signal " + str( r ) )
            else:
                self.debug( "Command returned " + str( r ) )
        except OSError, e:
            self.debug( "Execution failed " + str( e ) )
            # this should not happen, we will try to execute the command like this now
            self.debug( "Trying with brute-force....." )
            os.system( command )




