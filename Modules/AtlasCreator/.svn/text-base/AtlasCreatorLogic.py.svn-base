import Slicer
from Slicer import slicer
import os
import glob
import time
import tempfile
import shutil

from AtlasCreatorHelper import AtlasCreatorHelper

class AtlasCreatorLogic( object ):
    '''
        The logic class for the Atlas Creator
        
        In this class, all files are passed as lists of filepaths to enable possible parallelized
        computation in a grid environment 
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

        self._parentClass = parentClass

        self.__dryRun = 0

        # this indicates if a registration or resampling job really failed
        self.__failure = ""



    '''=========================================================================================='''
    def Destructor( self ):
        '''
            Destruct this class
            
            Returns
                n/a
        '''
        self.__parentClass = None
        self.__dryRun = None
        self.__failure = None



    '''=========================================================================================='''
    def EnableDryrunMode( self ):
        '''
            Enables the dryrun Mode
            
            Returns
                n/a
        '''
        self.__dryRun = 1



    '''=========================================================================================='''
    def Helper( self ):
        '''
            Return the helper
            
            Returns
                the helper
        '''
        if not self._parentClass:
            # no parentClass, we have to create our own helper
            # this is not normal behavior
            return AtlasCreatorHelper( self )
        else:
            # return the helper of the parentClass
            return self._parentClass.GetHelper()



    '''=========================================================================================='''
    def Start( self, node ):
        '''
            Entry point for the Atlas Creation
            
            node
                an AtlasCreator MRML Node (vtkMRMLAtlasCreatorNode) transporting a configuration
                
            Returns
                TRUE or FALSE
        '''

        # reset the failure container
        self.__failure = ""

        # check if we have a valid vtkMRMLAtlasCreatorNode
        if not isinstance( node, slicer.vtkMRMLAtlasCreatorNode ):
            self.Helper().info( "ERROR: No valid vtkMRMLAtlasCreatorNode!" )
            self.Helper().info( "Aborting now!" )
            return False

        if node.GetDebugMode():
            # activate debug mode
            self.Helper().EnableDebugMode()
            self.Helper().info( "Debug Mode is enabled!" )

        if node.GetDryrunMode():
            # activate dryrun mode
            self.EnableDryrunMode()
            self.Helper().info( "Dry-Run is activated: Output executable commands instead of running the registration or resampling." )

        if node.GetTestMode():
            # activate test mode
            self.Helper().EnableTestMode()
            self.Helper().info( "WARNING: TEST MODE ACTIVATED! Registration will not be very accurate..." )

        clusterMode = 0
        skipRegistrationMode = 0

        if ( node.GetUseCluster() ):

            # Cluster Mode
            self.Helper().info( "--------------------------------------------------------------------------------" )
            self.Helper().info( "                   Starting Atlas Creator Cluster Mode                          " )
            clusterMode = 1

        elif ( node.GetSkipRegistration() ):

            # Skip Registration mode
            self.Helper().info( "--------------------------------------------------------------------------------" )
            self.Helper().info( "               Starting Atlas Creator Skip Registration Mode                    " )
            skipRegistrationMode = 1

        else:

            # Standard Mode
            self.Helper().info( "--------------------------------------------------------------------------------" )
            self.Helper().info( "                      Starting Atlas Creator Normal Mode                        " )


        # check if we want to use CMTK
        useCMTK = False
        if node.GetToolkit() == "CMTK":
            # check if CMTK is installed
            cmtkDir = self.Helper().GetCMTKInstallationDirectory()
            if cmtkDir:
                self.Helper().info( "Found CMTK at: " + cmtkDir )
                useCMTK = True
            else:
                self.Helper().info( "WARNING: CMTK extensions not found!" )
                self.Helper().info( "WARNING: Falling back to BRAINSFit..." )
                self.Helper().info( "WARNING: Please install CMTK4Slicer in order to use CMTK!" )
                useCMTK = False

        #
        # convert the string based filePath lists to real lists
        #
        originalImagesFilePathList = str( node.GetOriginalImagesFilePathList() )
        if originalImagesFilePathList != "":
            originalImagesFilePathList = originalImagesFilePathList.split()
        else:
            originalImagesFilePathList = []

        segmentationsFilePathList = str( node.GetSegmentationsFilePathList() )
        if segmentationsFilePathList != "":
            segmentationsFilePathList = segmentationsFilePathList.split()
        else:
            segmentationsFilePathList = []

        # if we need to register, check if all files in the configuration.GetOriginalImagesFilePathList() are
        # also present in  configuration.GetSegmentationsFilePathList() and vice-versa
        #
        if not skipRegistrationMode:

            originalImagesFilePathList = self.Helper().GetValidFilePaths( originalImagesFilePathList,
                                                                         segmentationsFilePathList )
            segmentationsFilePathList = self.Helper().GetValidFilePaths( segmentationsFilePathList,
                                                                        originalImagesFilePathList )


        # convert the string based labels list to a real list is not necessary because python does it
        # automatically
        labelsList = node.GetLabelsList()
        # check if there is only one label in the list, this is a special case
        if type( labelsList ).__name__ == 'int':
            # if yes, convert it from int to a list
            tmpList = []
            tmpList.append( labelsList )
            labelsList = tmpList

        # we check now if we have a valid list of labels
        if not type( labelsList ).__name__ == 'list' or len( labelsList ) < 1:
            # this is not a valid list of labels,
            # let's read them from the first segmentation
            if len( segmentationsFilePathList ) > 1:

                self.Helper().info( "Labels were not specified correctly.." )
                self.Helper().info( "Reading labels from the segmentations.." )
                labelsList = self.Helper().ReadLabelsFromImages( segmentationsFilePathList )

                # now we save the labelList back to the node
                node.SetLabelsList( self.Helper().ConvertListToString( labelsList ) )

            else:
                # we could not get the labels.. abort.
                self.Helper().info( "Could not read labels.. ABORTING.." )
                return False




        # at this point, we have a valid configuration, let's print it
        self.Helper().info( "Configuration for Atlas Creator:\n" + str( node ) )

        # create the output directories     
        outputDir = node.GetOutputDirectory()

        # the following code checks if the outputDir already exists
        # if yes, it creates a new one with a number appended
        if outputDir and os.path.isdir( outputDir ):
            # outputDir already exists

            # let's check if it is empty
            # if it is empty, we can just keep the users setting
            if os.listdir( outputDir ):

                # it is not empty, so let's take some action:

                # we create a new unique one
                outputDir = os.path.abspath( outputDir )
                self.Helper().info( "Warning: The output directory (" + str( outputDir ) + ") is not empty.." )

                # the directory already exists,
                # we want to add an index to the new one
                count = 2
                newOutputDir = os.path.abspath( str( outputDir ) + str( count ) )

                while ( os.path.isdir( newOutputDir ) ):
                    count = count + 1
                    newOutputDir = os.path.abspath( str( outputDir ) + str( count ) )

                self.Helper().info( "Warning: Using new output directory instead: " + str( newOutputDir ) )
                os.makedirs( newOutputDir )

                outputDir = newOutputDir + os.sep

            else:

                self.Helper().info( "The output directory already exists but is empty.. Let's use it." )
                outputDir = os.path.normpath( outputDir ) + os.sep

        elif outputDir and not os.path.isfile( outputDir ):
            # outputDir did not exist and is not a file
            # create it
            outputDir = os.path.abspath( outputDir ) + os.sep
            os.makedirs( outputDir )
            self.Helper().info( "Created output directory: " + str( outputDir ) )

        else:
            self.Helper().info( "ERROR: No valid output directory configured!" )
            self.Helper().info( "Aborting now!" )
            return False

        # update the MRMLNode and propagate the possibly changed output directory
        node.SetOutputDirectory( outputDir )


        transformDirectory = outputDir + "transforms" + os.sep
        os.makedirs( transformDirectory )
        registeredDirectory = outputDir + "registered" + os.sep
        os.makedirs( registeredDirectory )
        resampledDirectory = outputDir + "resampled" + os.sep
        os.makedirs( resampledDirectory )
        scriptsRegistrationDirectory = outputDir + "scriptsRegistration" + os.sep
        os.makedirs( scriptsRegistrationDirectory )
        notifyRegistrationDirectory = outputDir + "notifyRegistration" + os.sep
        os.makedirs( notifyRegistrationDirectory )
        scriptsResamplingDirectory = outputDir + "scriptsResampling" + os.sep
        os.makedirs( scriptsResamplingDirectory )
        notifyResamplingDirectory = outputDir + "notifyResampling" + os.sep
        os.makedirs( notifyResamplingDirectory )
        scriptsCombineToAtlasDirectory = outputDir + "scriptsCombineToAtlas" + os.sep
        os.makedirs( scriptsCombineToAtlasDirectory )
        notifyCombineToAtlasDirectory = outputDir + "notifyCombineToAtlas" + os.sep
        os.makedirs( notifyCombineToAtlasDirectory )
        scriptsDrammsDirectory = outputDir + "scriptsDramms" + os.sep
        os.makedirs( scriptsDrammsDirectory )
        notifyDrammsDirectory = outputDir + "notifyDramms" + os.sep
        os.makedirs( notifyDrammsDirectory )


        # executable configuration
        numberOfThreads = node.GetNumberOfThreads()
        if numberOfThreads == -1:
            self.Helper().info( "Using maximal number of threads.." )
        else:
            self.Helper().info( "Using " + str( numberOfThreads ) + " threads.." )

        sleepValue = 1 # seconds to wait between each check on completed jobs
        schedulerCommand = "" # assume no scheduler by default

        if clusterMode:
            # if this is a cluster mode, add the schedulerCommand
            # also, raise the seconds to wait between each check on completed jobs to 60
            self.Helper().info( "Found cluster configuration.." )
            schedulerCommand = node.GetSchedulerCommand()
            self.Helper().debug( "Scheduler Command: " + str( schedulerCommand ) )
            sleepValue = 60


        # check if we register or if we use existing transforms
        if not skipRegistrationMode:
            # we register to get fresh transforms!!

            #
            #
            # REGISTRATION STAGE
            #
            #
            self.Helper().info( "Entering Registration Stage.." )
            self.Helper().info( "Using " + str( node.GetRegistrationType() ) + " registration." )

            #
            # FIXED REGISTRATION
            #
            if node.GetTemplateType() == "fixed":
                # fixed registration.. only register once against the defaultCase

                defaultCase = node.GetFixedTemplateDefaultCaseFilePath()

                self.Helper().info( "Fixed registration against " + str( defaultCase ) )

                alignedImages = self.Register( schedulerCommand,
                                              originalImagesFilePathList,
                                              defaultCase,
                                              transformDirectory,
                                              registeredDirectory,
                                              scriptsRegistrationDirectory,
                                              notifyRegistrationDirectory,
                                              node.GetRegistrationType(),
                                              numberOfThreads,
                                              useCMTK,
                                              sleepValue )

            #
            # DYNAMIC REGISTRATION
            #
            elif node.GetTemplateType() == "dynamic":
                # dynamic registration.. register against mean until number of iterations reached

                alignedImages = originalImagesFilePathList

                self.Helper().info( "Dynamic registration with " + str( node.GetDynamicTemplateIterations() ) + " iterations" )

                for i in range( 0, node.GetDynamicTemplateIterations() ):

                    self.Helper().info( "Starting iteration " + str( i + 1 ) + "..." )

                    # create directories for the current registration iteration as subdirs of the registrationDir
                    iterationString = "iteration" + str( i + 1 )
                    uniqueRegisteredDirectory = registeredDirectory + iterationString + os.sep
                    os.makedirs( uniqueRegisteredDirectory )

                    # create a temporary meanImage which might get used
                    meanImageFilePath = tempfile.mkstemp( ".nrrd", "acTmpMeanImage", uniqueRegisteredDirectory )[1]

                    uniqueScriptsDirectory = scriptsRegistrationDirectory + iterationString + os.sep
                    os.makedirs( uniqueScriptsDirectory )
                    uniqueNotifyDirectory = notifyRegistrationDirectory + iterationString + os.sep
                    os.makedirs( uniqueNotifyDirectory )

                    # create directories for meanImageComputation as subdirs of the registrationDir
                    meanScriptsDirectory = uniqueScriptsDirectory + "meanTemplate" + os.sep
                    os.makedirs( meanScriptsDirectory )
                    meanNotifyDirectory = uniqueNotifyDirectory + "meanTemplate" + os.sep
                    os.makedirs( meanNotifyDirectory )

                    # we generate the current meanImage
                    if not self.__dryRun:
                        self.ComputeMeanImage( schedulerCommand,
                                              alignedImages,
                                              meanImageFilePath,
                                              meanScriptsDirectory,
                                              meanNotifyDirectory,
                                              sleepValue )

                        if node.GetDeleteAlignedImages():
                            if alignedImages != originalImagesFilePathList:
                                # do not delete the original images
                                self.Helper().DeleteFilesAndDirectory( alignedImages, True )

                    # we register the original images against the meanImage
                    # we then set the alignedImages and start over..
                    alignedImages = self.Register( schedulerCommand,
                                                  originalImagesFilePathList,
                                                  meanImageFilePath,
                                                  transformDirectory,
                                                  uniqueRegisteredDirectory,
                                                  uniqueScriptsDirectory,
                                                  uniqueNotifyDirectory,
                                                  node.GetRegistrationType(),
                                                  numberOfThreads,
                                                  useCMTK,
                                                  sleepValue )

                # now we point the defaultCase to the meanImageFilePath
                defaultCase = meanImageFilePath

                self.Helper().info( "End of Dynamic registration.." )

            # for both registration modes:

            if not alignedImages:
                self.Helper().info( "ERROR: The Registration failed.. ABORTING.." )
                return False

            # now use the alignedImages to return as output as well the normalized intensity maps of aligned cases
            meanScriptsDirectory = registeredDirectory + "meanTemplateScripts" + os.sep
            os.makedirs( meanScriptsDirectory )
            meanNotifyDirectory = registeredDirectory + "meanTemplateNotify" + os.sep
            os.makedirs( meanNotifyDirectory )

            normalizedIntensityMapFilePath = outputDir + "normalizedIntensityMapOfAligned.nrrd"

            # if we use fixed registration, we will now add the defaultCase as well
            # to prevent information loss during the combination stage
            if node.GetTemplateType() == "fixed":
                # find the segmentation to the default case by looping through the segmentationsFilePathList
                for s in segmentationsFilePathList:
                    if os.path.basename( s ) == os.path.basename( defaultCase ):
                        # this defaultCase has a segmentation so we do want to consider it in the atlas creation                
                        alignedImages.append( defaultCase )

            self.ComputeMeanImage( schedulerCommand,
                                  alignedImages,
                                  normalizedIntensityMapFilePath,
                                  meanScriptsDirectory,
                                  meanNotifyDirectory,
                                  sleepValue )

            # we will save the template
            # this will ensure that we can later 
            # use the transforms (if they exist) and the template to resample
            # at this point, the defaultCase is either the meanImage or the fixed defaultCase
            pathToTemplate = outputDir + "template" + os.path.splitext( defaultCase )[1]
            self.Helper().info( "Saving template to " + str( pathToTemplate ) )
            shutil.copyfile( defaultCase, pathToTemplate )

            # update reference to defaultCase to new location, only if templateType is dynamic
            if node.GetTemplateType() == "dynamic":
                defaultCase = pathToTemplate

            # now wipe the temporary registered content, if selected
            if node.GetDeleteAlignedImages() and not node.GetUseDRAMMS():
                shutil.rmtree( registeredDirectory, True, None )

        else:
            # we are skipping the registration
            self.Helper().info( "Skipping the registration and using the existing transforms.." )

            transformDirectory = node.GetTransformsDirectory()
            # we set the defaultCase to an existing one
            defaultCase = node.GetExistingTemplate()



        #
        #
        # RESAMPLING STAGE
        #
        #
        self.Helper().info( "Entering Resampling Stage.." )

        # if there was a failure during registration, we want to update the segmentations to resample
        if self.__failure:
            # only use segmentations which were actually registered
            segmentationsFilePathList = self.Helper().GetValidFilePaths( segmentationsFilePathList,
                                                                        alignedImages )

        self.Resample( schedulerCommand,
                      segmentationsFilePathList,
                      defaultCase,
                      transformDirectory,
                      resampledDirectory,
                      scriptsResamplingDirectory,
                      notifyResamplingDirectory,
                      useCMTK,
                      sleepValue )

        if not self.__dryRun:
            resampledSegmentationsFilePathList = self.Helper().ConvertDirectoryToList( resampledDirectory )

            # if we use fixed registration, we will now add the segmentation of the defaultCase as well
            # to prevent information loss during the combination stage
            if node.GetTemplateType() == "fixed" and not node.GetIgnoreTemplateSegmentation():

                # find the segmentation to the default case by looping through the segmentationsFilePathList
                for s in segmentationsFilePathList:
                    if os.path.basename( s ) == os.path.basename( defaultCase ):
                        self.Helper().info( "Adding the template's segmentation to the atlases.." )
                        resampledSegmentationsFilePathList.append( s )
                        break

        else:
            resampledSegmentationsFilePathList = segmentationsFilePathList


        if node.GetPCAAnalysis():
            #
            #
            # PCA ANALYSIS STAGE
            #
            #
            self.Helper().info( "Performing PCA Analysis.." )

            self.PerformPCAAnalysis( labelsList,
                                    resampledSegmentationsFilePathList,
                                    outputDir,
                                    node.GetPCAMaxEigenVectors(),
                                    node.GetPCACombine() )

        else:

            #
            #
            # USE DRAMMS?
            #
            #         
            if node.GetUseDRAMMS():
                self.Helper().info( "Performing DRAMMS Registration and Resampling.." )
                drammsSegmentationDir = self.PerformDRAMMS( schedulerCommand,
                                                     alignedImages,
                                                     defaultCase,
                                                     resampledSegmentationsFilePathList,
                                                     outputDir,
                                                     scriptsDrammsDirectory,
                                                     notifyDrammsDirectory,
                                                     sleepValue )

                resampledSegmentationsFilePathList = self.Helper().ConvertDirectoryToList( drammsSegmentationDir )

            #
            #
            # COMBINE TO ATLAS STAGE
            #
            #
            self.Helper().info( "Entering Combine-To-Atlas Stage.." )

            # let's check if normalize is enabled
            # if no, we set a new NormalizeTo value
            if not node.GetNormalizeAtlases():
                node.SetNormalizeTo( -1 )

            self.CombineToAtlas( schedulerCommand,
                                resampledSegmentationsFilePathList,
                                labelsList,
                                node.GetOutputCast(),
                                node.GetNormalizeTo(),
                                outputDir,
                                scriptsCombineToAtlasDirectory,
                                notifyCombineToAtlasDirectory,
                                sleepValue )


        # cleanup!!
        # delete the scripts and notify directories
        if not self.__dryRun:
            shutil.rmtree( scriptsRegistrationDirectory, True, None )
            shutil.rmtree( notifyRegistrationDirectory, True, None )
            shutil.rmtree( scriptsResamplingDirectory, True, None )
            shutil.rmtree( notifyResamplingDirectory, True, None )
            shutil.rmtree( scriptsCombineToAtlasDirectory, True, None )
            shutil.rmtree( notifyCombineToAtlasDirectory, True, None )
            shutil.rmtree( scriptsDrammsDirectory, True, None )
            shutil.rmtree( notifyDrammsDirectory, True, None )

        # now delete the resampled segmentations
        if node.GetDeleteAlignedSegmentations():
            shutil.rmtree( resampledDirectory, True, None )

        # delete the transforms, if we did not want to save them
        if not node.GetSaveTransforms() and not skipRegistrationMode:
            shutil.rmtree( transformDirectory, True, None )


        self.Helper().info( "--------------------------------------------------------------------------------" )
        self.Helper().info( "                             All Done, folks!                                   " )
        if self.__failure:
            self.Helper().info( "WARNING: There were errors!! (Grep this log for FAILURE to get more information!)" )
            self.Helper().info( self.__failure )
        self.Helper().info( "--------------------------------------------------------------------------------" )

        return True



    '''=========================================================================================='''
    def ComputeMeanImage( self, schedulerCommand, filePathsList, outputFilePath, outputScriptsDirectory, outputNotifyDirectory, sleepValue=5 ):
        '''
            Get the mean image of a set of images and save it

            schedulerCommand
                the schedulerCommand if used else ""            
            filePathsList
                list of file paths to the images
            outputFilePath
                the filePath where the meanImage gets stored
            outputScriptsDirectory
                directory to use for generated scripts
            outputNotifyDirectory
                directory to use for notification files
            sleepValue
                seconds to wait between each check on completed jobs                           
                
            Returns
                TRUE or FALSE depending on Success
        '''
        # sanity checks
        if len( filePathsList ) == 0:
            self.Helper().info( "Empty filePathsList for ComputeMeanImage() command. Aborting.." )
            return False

        if not outputFilePath:
            self.Helper().info( "Empty outputFilePath for ComputeMeanImage() command. Aborting.." )
            return False

        if not outputScriptsDirectory:
            self.Helper().info( "Empty outputScriptsDirectory for ComputeMeanImage() command. Aborting.." )
            return None

        if not outputNotifyDirectory:
            self.Helper().info( "Empty outputNotifyDirectory for ComputeMeanImage() command. Aborting.." )
            return None


        # get the launch command for Slicer3
        launchCommandPrefix = self.Helper().GetSlicerLaunchPrefix( False, True )

        uniqueID = 1

        command = ""

        # add some args to the Slicer launcher call
        command += str( launchCommandPrefix ) + " --launch " + str( slicer.Application.GetBinDir() ) + os.sep + "ACMeanImage";

        # now the args
        command += " \""

        for file in filePathsList:
            command += str( file ) + " "

        command += "\""
        command += " " + str( outputFilePath )

        self.Helper().debug( "ComputeMeanImage Command: " + str( command ) )

        # create notification ID
        notify = "echo \"done\" > " + str( outputNotifyDirectory ) + str( uniqueID ) + ".ac"
        notifyError = "echo \"err\" > " + str( outputNotifyDirectory ) + str( uniqueID ) + ".f"

        # now generate a script containing the commands and the notify line
        script = self.Helper().CreateScript( command, notify, notifyError )

        scriptFilePath = outputScriptsDirectory + "script" + str( uniqueID ) + ".sh"

        with open( scriptFilePath, 'w' ) as f:
            f.write( script )

        # set executable permissions
        os.chmod( scriptFilePath, 0777 )

        self.Helper().debug( "Executing generated ComputeMeanImage Script: " + scriptFilePath )

        if self.__dryRun:
            self.Helper().info( "DRYRUN - skipping execution.." )
        else:
            self.Helper().Execute( schedulerCommand + " " + scriptFilePath )

        if self.__dryRun:
            return True

        # at this point:
        # either the computation was completed if the os.system call did not send it to the background
        # or it still runs in the background
        # latter is possible, if a cluster scheduler was specified
        # now, we will wait until all output exists

        allOutputsExist = False

        failures = {}

        while not allOutputsExist:
            # not all outputs exist yet
            # get number of existing files
            self.Helper().info( "Waiting for ComputeMeanImage to complete.." )

            # wait some secs and then check again
            time.sleep( int( sleepValue ) )

            # we assume everything exists
            allOutputsExist = True

            # but now we really check if it is so            

            file = outputNotifyDirectory + str( uniqueID ) + ".ac"
            errorFile = outputNotifyDirectory + str( uniqueID ) + ".f"

            if os.path.isfile( errorFile ):
                # uh-oh this job failed
                if uniqueID in failures:
                    # failed before, increase the failcount
                    failures[uniqueID] = failures[uniqueID] + 1
                else:
                    # first failure
                    failures[uniqueID] = 1

                # check if the job failed already 4 times
                if failures[uniqueID] > 4:
                    # that's it.. we will cancel this job and notify the user
                    self.Helper().info( "-------------------------------------------------------" )
                    self.Helper().info( "-------------------------------------------------------" )
                    self.Helper().info( "FAILURE: ComputeMeanImage Job with id " + str( uniqueID ) + " failed 4 times!" )
                    self.Helper().info( "-------------------------------------------------------" )
                    self.Helper().info( "-------------------------------------------------------" )
                    # now delete the failure flag
                    os.remove( errorFile )

                    # save which file failed
                    self.__failure += "ComputeMeanImage failed!\n"

                    # .. and touch the file
                    open( file, 'w' ).close()

                else:
                    # let's try this job again

                    self.Helper().info( "ComputeMeanImage Job with id " + str( uniqueID ) + " failed in attempt " + str( failures[uniqueID] ) + ". Relaunching.." )
                    # now delete the failure flag
                    os.remove( errorFile )

                    # .. and relaunch the job
                    scriptFilePath = outputScriptsDirectory + "script" + str( uniqueID ) + ".sh"
                    self.Helper().Execute( schedulerCommand + " " + scriptFilePath )

                    allOutputsExist = False


            elif not os.path.isfile( file ):
                self.Helper().debug( "ComputeMeanImage Job not completed!" )
                # if only one file does not exist,
                # we know we have to wait longer
                allOutputsExist = False

        self.Helper().debug( "All ComputeMeanImage output exists!" )

        return True



    '''=========================================================================================='''
    def Register( self, schedulerCommand, filePathsList, templateFilePath, outputTransformsDirectory, outputAlignedDirectory, outputScriptsDirectory, outputNotifyDirectory, registrationType, numberOfThreads, useCMTK=False, sleepValue=5 ):
        '''
            Register a set of images, get a transformation and save it
            
            schedulerCommand
                the schedulerCommand if used else ""
            filePathsList
                list of existing filepaths to images
            templateFilePath
                file path to the template
            outputTransformsDirectory
                directory to save the generated transformation
            outputAlignedDirectory
                directory to save the aligned images
            outputScriptsDirectory
                directory to use for generated scripts
            outputNotifyDirectory
                directory to use for notification files
            registrationType
                type of registration as String, could be "rigid", "affine", "affine12" and "non-rigid"
                if the value is invalid, affine registration is assumed
            numberOfThreads
                if -1, use the maximal number of threads else use the specified number
            useCMTK
                if TRUE, use CMTK instead of BRAINSFit
            sleepValue
                seconds to wait between each check on completed jobs
                
            Returns
                A list of filepaths to the aligned Images or None depending on success
        '''

        # sanity checks
        if len( filePathsList ) == 0:
            self.Helper().info( "Empty filePathsList for Register() command. Aborting.." )
            return None

        if not templateFilePath:
            self.Helper().info( "Empty templateFilePath for Register() command. Aborting.." )
            return None

        if not outputTransformsDirectory:
            self.Helper().info( "Empty outputTransformsDirectory for Register() command. Aborting.." )
            return None

        if not outputAlignedDirectory:
            self.Helper().info( "Empty outputAlignedDirectory for Register() command. Aborting.." )
            return None

        if not outputScriptsDirectory:
            self.Helper().info( "Empty outputScriptsDirectory for Register() command. Aborting.." )
            return None

        if not outputNotifyDirectory:
            self.Helper().info( "Empty outputNotifyDirectory for Register() command. Aborting.." )
            return None

        rigid = False
        affine = False
        affine12 = False
        nonrigid = False

        if registrationType == "Rigid":
            rigid = True
        elif registrationType == "Affine":
            rigid = True
            affine = True
        elif registrationType == "Affine12":
            rigid = True
            affine = True
            affine12 = True
        elif registrationType == "Non-Rigid":
            rigid = True
            affine = True
            affine12 = True
            nonrigid = True
        else:
            # assume affine registration with 9 DOF if wrong value
            rigid = True
            affine = True

        outputAlignedImages = []

        # get the launch command for Slicer3
        launchCommandPrefix = self.Helper().GetSlicerLaunchPrefix( useCMTK )

        uniqueID = 0
        mapFilesToIDs = {}

        # loop through filePathsList and start registration command
        for movingImageFilePath in filePathsList:

            # do not register the same file
            if movingImageFilePath == templateFilePath:
                continue

            # increase the uniqueID
            uniqueID = uniqueID + 1

            # guess the background level of the current image
            backgroundGuess = self.Helper().GuessBackgroundValue( movingImageFilePath )
            self.Helper().debug( "Guessing background value: " + str( backgroundGuess ) )

            if useCMTK:
                # guess the background level of the current image, only if CMTK is used
                backgroundGuessTemplate = self.Helper().GuessBackgroundValue( templateFilePath )
                self.Helper().debug( "Guessing background value for template: " + str( backgroundGuessTemplate ) )

            movingImageName = os.path.splitext( os.path.basename( movingImageFilePath ) )[0]

            # generate file path to save output transformation
            # by getting the filename of the case and appending it to the outputTransformsDirectory
            outputTransformFilePath = outputTransformsDirectory + str( movingImageName ) + ".mat"

            # generate file path to save aligned output image
            # by getting the filename of the case and appending it to the outputTransformsDirectory
            outputAlignedImageFilePath = outputAlignedDirectory + str( movingImageName ) + ".nrrd"

            # save the uniqueID and the filePath in our map
            mapFilesToIDs[uniqueID] = outputAlignedImageFilePath

            # we will create a string containing the command(s) which then get written to a script
            command = ""

            if useCMTK:

                if numberOfThreads != -1:
                    # for CMTK, we have to set the variable to configure the number of threads
                    command += "CMTK_NUM_THREADS=" + str( numberOfThreads ) + "\n"

                command += str( launchCommandPrefix ) + self.Helper().GetCMTKAffineRegistrationCommand( templateFilePath,
                                                                                                    movingImageFilePath,
                                                                                                    outputTransformFilePath,
                                                                                                    outputAlignedImageFilePath,
                                                                                                    backgroundGuess,
                                                                                                    backgroundGuessTemplate,
                                                                                                    affine,
                                                                                                    affine12 )

                if nonrigid:
                    # for non-rigid, we include the second CMTK command
                    command += "\n"
                    command += str( launchCommandPrefix ) + self.Helper().GetCMTKNonRigidRegistrationCommand( templateFilePath,
                                                                                                           movingImageFilePath,
                                                                                                           outputTransformFilePath,
                                                                                                           outputAlignedImageFilePath,
                                                                                                           backgroundGuess,
                                                                                                           backgroundGuessTemplate )


            else:
                command += str( launchCommandPrefix ) + self.Helper().GetBRAINSFitRegistrationCommand( templateFilePath,
                                                                                                   movingImageFilePath,
                                                                                                   outputTransformFilePath,
                                                                                                   outputAlignedImageFilePath,
                                                                                                   numberOfThreads,
                                                                                                   backgroundGuess,
                                                                                                   affine,
                                                                                                   affine12,
                                                                                                   nonrigid )

            self.Helper().debug( "Register command(s): " + str( command ) )

            # create notification ID
            notify = "echo \"done\" > " + str( outputNotifyDirectory ) + str( uniqueID ) + ".ac"
            notifyError = "echo \"err\" > " + str( outputNotifyDirectory ) + str( uniqueID ) + ".f"

            # now generate a script containing the commands and the notify line
            script = self.Helper().CreateScript( command, notify, notifyError )

            scriptFilePath = outputScriptsDirectory + "script" + str( uniqueID ) + ".sh"

            with open( scriptFilePath, 'w' ) as f:
                f.write( script )

            # set executable permissions
            os.chmod( scriptFilePath, 0777 )

            self.Helper().debug( "Executing generated Registration Script: " + scriptFilePath )

            if self.__dryRun:
                self.Helper().info( "DRYRUN - skipping execution.." )
            else:
                self.Helper().Execute( schedulerCommand + " " + scriptFilePath )

            outputAlignedImages.append( str( outputAlignedImageFilePath ) )

        if self.__dryRun:
            return outputAlignedImages

        # at this point:
        # either the registration was completed if the os.system call did not send it to the background
        # or the registration still runs in the background
        # latter is possible, if a cluster scheduler was specified
        # now, we will wait until all output images exist

        allOutputsExist = False

        # the dictionary for failures
        failures = {}

        while not allOutputsExist:
            # not all outputs exist yet
            # get number of existing files
            numberOfExistingFiles = len( glob.glob( outputNotifyDirectory + "*.ac" ) )
            self.Helper().info( "Waiting for Registration to complete.. (" + str( numberOfExistingFiles ) + "/" + str( uniqueID ) + " done)" )

            # wait some secs and then check again
            time.sleep( int( sleepValue ) )

            # we assume everything exists
            allOutputsExist = True

            # but now we really check if it is so            
            #for file in outputAlignedImages:
            for index in range( 1, uniqueID + 1 ):

                file = outputNotifyDirectory + str( index ) + ".ac"
                errorFile = outputNotifyDirectory + str( index ) + ".f"

                if os.path.isfile( errorFile ):
                    # uh-oh this job failed
                    if index in failures:
                        # failed before, increase the failcount
                        failures[index] = failures[index] + 1
                    else:
                        # first failure
                        failures[index] = 1

                    # check if the job failed already 4 times
                    if failures[index] > 4:
                        # that's it.. we will cancel this job and notify the user
                        self.Helper().info( "-------------------------------------------------------" )
                        self.Helper().info( "-------------------------------------------------------" )
                        self.Helper().info( "FAILURE: Registration Job with id " + str( index ) + " failed 4 times! Let's continue without it.." )
                        self.Helper().info( "FAILURE: File " + str( mapFilesToIDs[index] ) + " will NOT be considered during the next steps.." )
                        self.Helper().info( "-------------------------------------------------------" )
                        self.Helper().info( "-------------------------------------------------------" )
                        # now delete the failure flag
                        os.remove( errorFile )
                        # remove the output file of this job from our alignedImages list
                        outputAlignedImages.remove( mapFilesToIDs[index] )

                        # save which file failed
                        self.__failure += "Registration of " + str( mapFilesToIDs[index] ) + " failed!\n"

                        # .. and touch the file
                        open( file, 'w' ).close()

                    else:
                        # let's try this job again

                        self.Helper().info( "Registration Job with id " + str( index ) + " failed in attempt " + str( failures[index] ) + ". Relaunching.." )
                        # now delete the failure flag
                        os.remove( errorFile )

                        # .. and relaunch the job
                        scriptFilePath = outputScriptsDirectory + "script" + str( index ) + ".sh"
                        self.Helper().Execute( schedulerCommand + " " + scriptFilePath )

                        allOutputsExist = False

                elif not os.path.isfile( file ):
                    self.Helper().debug( "Registration Job with id " + str( index ) + " not completed!" )
                    # if only one file does not exist,
                    # we know we have to wait longer
                    allOutputsExist = False

        self.Helper().debug( "All Registration outputs exist!" )

        return outputAlignedImages



    '''=========================================================================================='''
    def Resample( self, schedulerCommand, filePathsList, templateFilePath, transformDirectory, outputSegmentationDirectory, outputScriptsDirectory, outputNotifyDirectory, useCMTK=False, sleepValue=5 ):
        '''
            Resample a set of segmentations using exising transforms
            
            schedulerCommand
                the schedulerCommand if used else ""
            filePathsList
                list of existing filepaths to segmentations
            templateFilePath
                file path to the template
            transformDirectory
                directory to existing transformations
                the transformation has to be named after the basename of the filepaths with a .mat extension      
                (can be a directory as well)
            outputSegmentationDirectory
                directory to save the resampled images
            outputScriptsDirectory
                directory to use for generated scripts
            outputNotifyDirectory
                directory to use for notification files                
            useCMTK
                if TRUE, use CMTK instead of BRAINSFit
            sleepValue
                seconds to wait between each check on completed jobs
                                
            Returns
                TRUE or FALSE depending on success
        '''

        # sanity checks
        if len( filePathsList ) == 0:
            self.Helper().info( "Empty filePathsList for Resample() command. Aborting.." )
            return False

        if not templateFilePath:
            self.Helper().info( "Empty templateFilePath for Resample() command. Aborting.." )
            return False

        if not transformDirectory:
            self.Helper().info( "Empty transformDirectory for Resample() command. Aborting.." )
            return False

        if not outputSegmentationDirectory:
            self.Helper().info( "Empty outputSegmentationDirectory for Resample() command. Aborting.." )
            return False

        if not outputScriptsDirectory:
            self.Helper().info( "Empty outputScriptsDirectory for Resample() command. Aborting.." )
            return False

        if not outputNotifyDirectory:
            self.Helper().info( "Empty outputNotifyDirectory for Resample() command. Aborting.." )
            return False

        # get the launch command for Slicer3
        launchCommandPrefix = self.Helper().GetSlicerLaunchPrefix( useCMTK )

        uniqueID = 0

        mapFilesToIDs = {}

        # loop through filePathsList and start resample command
        for segmentationFilePath in filePathsList:

            # do not resample the same file
            if os.path.basename( segmentationFilePath ) == os.path.basename( templateFilePath ):
                continue

            # increase the uniqueID
            uniqueID = uniqueID + 1

            # guess the background level of the current image
            backgroundGuess = self.Helper().GuessBackgroundValue( segmentationFilePath )
            self.Helper().debug( "Guessing background value: " + str( backgroundGuess ) )

            segmentationName = os.path.splitext( os.path.basename( segmentationFilePath ) )[0]

            transformFilePath = transformDirectory + str( segmentationName ) + ".mat"

            # generate file path to output segmentation
            # by getting the filename of the case and appending it to the outputSegmentationDirectory
            outputSegmentationFilePath = outputSegmentationDirectory + str( segmentationName ) + ".nrrd"

            # save the uniqueID and the filePath in our map
            mapFilesToIDs[uniqueID] = outputSegmentationFilePath

            # we will create a string containing the command(s) which then get written to a script
            command = ""

            if useCMTK:
                command += str( launchCommandPrefix ) + self.Helper().GetCMTKResampleCommand( segmentationFilePath,
                                                                                           templateFilePath,
                                                                                           transformFilePath,
                                                                                           outputSegmentationFilePath,
                                                                                           backgroundGuess )
            else:
                command += str( launchCommandPrefix ) + self.Helper().GetBRAINSFitResampleCommand( segmentationFilePath,
                                                                                                templateFilePath,
                                                                                                transformFilePath,
                                                                                                outputSegmentationFilePath,
                                                                                                backgroundGuess )

            self.Helper().debug( "Resample command(s): " + str( command ) )

            # create notification ID
            notify = "echo \"done\" > " + str( outputNotifyDirectory ) + str( uniqueID ) + ".ac"
            notifyError = "echo \"err\" > " + str( outputNotifyDirectory ) + str( uniqueID ) + ".f"

            # now generate a script containing the commands and the notify line
            script = self.Helper().CreateScript( command, notify, notifyError )

            scriptFilePath = outputScriptsDirectory + "script" + str( uniqueID ) + ".sh"

            with open( scriptFilePath, 'w' ) as f:
                f.write( script )

            # set executable permissions
            os.chmod( scriptFilePath, 0777 )

            self.Helper().debug( "Executing generated Resample Script: " + scriptFilePath )

            if self.__dryRun:
                self.Helper().info( "DRYRUN - skipping execution.." )
            else:
                self.Helper().Execute( schedulerCommand + " " + scriptFilePath )

        if self.__dryRun:
            return True

        # at this point:
        # either the resampling was completed if the os.system call did not send it to the background
        # or it still runs in the background
        # latter is possible, if a cluster scheduler was specified
        # now, we will wait until all output exists

        allOutputsExist = False

        # the dictionary for failures
        failures = {}

        while not allOutputsExist:
            # not all outputs exist yet
            # get number of existing files
            numberOfExistingFiles = len( glob.glob( outputNotifyDirectory + "*.ac" ) )
            self.Helper().info( "Waiting for Resampling to complete.. (" + str( numberOfExistingFiles ) + "/" + str( uniqueID ) + " done)" )

            # wait some secs and then check again
            time.sleep( int( sleepValue ) )

            # we assume everything exists
            allOutputsExist = True

            # but now we really check if it is so            
            #for file in outputAlignedImages:
            for index in range( 1, uniqueID + 1 ):

                file = outputNotifyDirectory + str( index ) + ".ac"
                errorFile = outputNotifyDirectory + str( index ) + ".f"

                if os.path.isfile( errorFile ):
                    # uh-oh this job failed
                    if index in failures:
                        # failed before, increase the failcount
                        failures[index] = failures[index] + 1
                    else:
                        # first failure
                        failures[index] = 1

                    # check if the job failed already 4 times
                    if failures[index] > 4:
                        # that's it.. we will cancel this job and notify the user
                        self.Helper().info( "-------------------------------------------------------" )
                        self.Helper().info( "-------------------------------------------------------" )
                        self.Helper().info( "FAILURE: Resampling Job with id " + str( index ) + " failed 4 times! Let's continue without it.." )
                        self.Helper().info( "FAILURE: File " + str( mapFilesToIDs[index] ) + " will NOT be considered during the next steps.." )
                        self.Helper().info( "-------------------------------------------------------" )
                        self.Helper().info( "-------------------------------------------------------" )
                        # now delete the failure flag
                        os.remove( errorFile )

                        # save which file failed
                        self.__failure += "Resampling of " + str( mapFilesToIDs[index] ) + " failed!\n"

                        # .. and touch the file
                        open( file, 'w' ).close()

                    else:
                        # let's try this job again

                        self.Helper().info( "Resampling Job with id " + str( index ) + " failed in attempt " + str( failures[index] ) + ". Relaunching.." )
                        # now delete the failure flag
                        os.remove( errorFile )

                        # .. and relaunch the job
                        scriptFilePath = outputScriptsDirectory + "script" + str( index ) + ".sh"
                        self.Helper().Execute( schedulerCommand + " " + scriptFilePath )

                        allOutputsExist = False


                elif not os.path.isfile( file ):
                    self.Helper().debug( "Resampling Job with id " + str( index ) + " not completed!" )
                    # if only one file does not exist,
                    # we know we have to wait longer
                    allOutputsExist = False


        self.Helper().debug( "All Resampling outputs exist!" )

        return True

    '''=========================================================================================='''
    def PerformDRAMMS( self, schedulerCommand, alignedImages, pathToTemplate, segmentations, outputDir, outputScriptsDirectory, outputNotifyDirectory, sleepValue=5 ):
        '''
            Perform DRAMMS Registration and Resampling
            
            schedulerCommand
                the schedulerCommand if used else ""
            alignedImages
                list of aligned image filepaths
            pathToTemplate
                filePath to template
            segmentations
                list of aligned segmentation filepaths
            outputDir
                output directory, a subdirectory 'dramms' will be appended
            outputScriptsDirectory
                directory to use for generated scripts
            outputNotifyDirectory
                directory to use for notification files    
            sleepValue
                seconds to wait between each check on completed jobs                
                
            Returns
                the directory path to the resampled segmentations
        '''

        alignedSegmentationsOutputDir = ""

        # sanity checks
        if len( alignedImages ) == 0:
            self.Helper().info( "Empty alignedImages for PerformDRAMMS() command. Aborting.." )
            return alignedSegmentationsOutputDir

        if len( segmentations ) == 0:
            self.Helper().info( "Empty segmentations for PerformDRAMMS() command. Aborting.." )
            return alignedSegmentationsOutputDir

        if len( outputDir ) == 0:
            self.Helper().info( "Empty outputDir for PerformDRAMMS() command. Aborting.." )
            return alignedSegmentationsOutputDir

        # try to find dramms
        cmtkDir = self.Helper().GetCMTKInstallationDirectory()
        drammsDir = str( cmtkDir ) + os.sep + 'DRAMMS' + os.sep

        if not os.path.isdir( drammsDir ):
            self.Helper().info( "ERROR: Could not find DRAMMS at " + drammsDir )
            return alignedSegmentationsOutputDir

        uniqueID = 0

        mapFilesToIDs = {}

        # create all output directories
        outputDir += os.sep + 'dramms' + os.sep
        os.makedirs( outputDir )
        tmpImagesOutputDir = outputDir + 'tmpImages' + os.sep
        os.makedirs( tmpImagesOutputDir )
        tmpSegmentationsOutputDir = outputDir + 'tmpSegmentations' + os.sep
        os.makedirs( tmpSegmentationsOutputDir )
        alignedOutputDir = outputDir + os.sep + 'alignedImages' + os.sep
        os.makedirs( alignedOutputDir )
        alignedSegmentationsOutputDir = outputDir + os.sep + 'alignedSegmentations' + os.sep
        os.makedirs( alignedSegmentationsOutputDir )
        deformationFieldsOutputDir = outputDir + os.sep + 'deformationFields' + os.sep
        os.makedirs( deformationFieldsOutputDir )

        # convert pathToTemplate to .hdr/.img
        pluginsDir = slicer.Application.GetPluginsDir()
        orientImagesCommand = pluginsDir + os.sep + "OrientImage -o RIP"
        orientImagesCommand += " " + str( pathToTemplate ) + " " + str( tmpImagesOutputDir ) + 'template.hdr'
        self.Helper().Execute( orientImagesCommand )

        originalTemplate = pathToTemplate
        pathToTemplate = tmpImagesOutputDir + 'template.img'

        # loop through alignedImages and start dramms command
        for imageFilePath in alignedImages:

            # do not process the template
            if os.path.basename( imageFilePath ) == os.path.basename( originalTemplate ):
                continue

            # increase the uniqueID
            uniqueID = uniqueID + 1

            fileNameWithoutExtension = os.path.splitext( os.path.basename( imageFilePath ) )[0]

            # get the existing image filepath (.IMG format)
            imageFilePathHdr = tmpImagesOutputDir + os.sep + fileNameWithoutExtension + '.hdr'
            imageFilePathImg = tmpImagesOutputDir + os.sep + fileNameWithoutExtension + '.img'
            volNode = self.Helper().LoadVolume( imageFilePath )
            # .. get the dimensions of the current image
            currentImageData = volNode.GetImageData()
            dimensions = currentImageData.GetDimensions()
            sizeX = dimensions[0]
            sizeY = dimensions[1]
            slicer.MRMLScene.RemoveNode( volNode )
            # .. convert orientation to RIP
            self.Helper().info( "Converting image: " + str( fileNameWithoutExtension ) + " to .hdr/.img .." )
            pluginsDir = slicer.Application.GetPluginsDir()
            orientImagesCommand = pluginsDir + os.sep + "OrientImage -o RIP"
            orientImagesCommand += " " + str( imageFilePath ) + " " + str( imageFilePathHdr )
            self.Helper().Execute( orientImagesCommand )


            # get the existing segmentation filepath (.IMG format)
            segmentationDir = os.path.dirname( segmentations[0] )
            segmentationFilePath = segmentationDir + os.sep + fileNameWithoutExtension + '.nrrd'
            segmentationFilePathHdr = tmpSegmentationsOutputDir + os.sep + fileNameWithoutExtension + '.hdr'
            segmentationFilePathImg = tmpSegmentationsOutputDir + os.sep + fileNameWithoutExtension + '.img'
            # .. convert to .IMG
            self.Helper().info( "Converting segmentation: " + str( fileNameWithoutExtension ) + " to .hdr/.img .." )
            pluginsDir = slicer.Application.GetPluginsDir()
            orientImagesCommand = pluginsDir + os.sep + "OrientImage -o RIP"
            orientImagesCommand += " " + str( segmentationFilePath ) + " " + str( segmentationFilePathHdr )
            self.Helper().Execute( orientImagesCommand )

            # set the paths
            outputAlignedImageFilePath = alignedOutputDir + fileNameWithoutExtension + ".img"
            outputDeformationFieldFilePath = deformationFieldsOutputDir + fileNameWithoutExtension + ".def"
            outputSegmentationFilePath = alignedSegmentationsOutputDir + fileNameWithoutExtension + ".img"

            # since the DRAMMS resampling does not re-create a .hdr file, we will just copy them over
            shutil.copyfile( segmentationFilePathHdr, alignedSegmentationsOutputDir + fileNameWithoutExtension + ".hdr" )

            # save the uniqueID and the filePath in our map
            mapFilesToIDs[uniqueID] = outputSegmentationFilePath

            # we will create a string containing the command(s) which then get written to a script
            command = ""

            command += drammsDir + "script" + os.sep + self.Helper().GetDRAMMSRegistrationCommand( pathToTemplate,
                                                                  imageFilePathImg,
                                                                  outputAlignedImageFilePath,
                                                                  outputDeformationFieldFilePath )

            command += "\n"

            command += drammsDir + "exe" + os.sep + self.Helper().GetDRAMMSResampleCommand( segmentationFilePathImg,
                                                              outputDeformationFieldFilePath,
                                                              outputSegmentationFilePath,
                                                              sizeX,
                                                              sizeY )

            self.Helper().debug( "DRAMMS command(s): " + str( command ) )

            # create notification ID
            notify = "echo \"done\" > " + str( outputNotifyDirectory ) + str( uniqueID ) + ".ac"
            notifyError = "echo \"err\" > " + str( outputNotifyDirectory ) + str( uniqueID ) + ".f"

            # now generate a script containing the commands and the notify line
            script = self.Helper().CreateScript( command, notify, notifyError )

            scriptFilePath = outputScriptsDirectory + "script" + str( uniqueID ) + ".sh"

            with open( scriptFilePath, 'w' ) as f:
                f.write( script )

            # set executable permissions
            os.chmod( scriptFilePath, 0777 )

            self.Helper().debug( "Executing generated DRAMMS Script: " + scriptFilePath )

            if self.__dryRun:
                self.Helper().info( "DRYRUN - skipping execution.." )
            else:
                self.Helper().Execute( schedulerCommand + " " + scriptFilePath )

        if self.__dryRun:
            return alignedSegmentationsOutputDir


        # at this point:
        # either the dramms run was completed if the os.system call did not send it to the background
        # or it still runs in the background
        # latter is possible, if a cluster scheduler was specified
        # now, we will wait until all output exists

        allOutputsExist = False

        # the dictionary for failures
        failures = {}

        while not allOutputsExist:
            # not all outputs exist yet
            # get number of existing files
            numberOfExistingFiles = len( glob.glob( outputNotifyDirectory + "*.ac" ) )
            self.Helper().info( "Waiting for DRAMMS to complete.. (" + str( numberOfExistingFiles ) + "/" + str( uniqueID ) + " done)" )

            # wait some secs and then check again
            time.sleep( int( sleepValue ) )

            # we assume everything exists
            allOutputsExist = True

            # but now we really check if it is so            
            #for file in outputAlignedImages:
            for index in range( 1, uniqueID + 1 ):

                file = outputNotifyDirectory + str( index ) + ".ac"
                errorFile = outputNotifyDirectory + str( index ) + ".f"

                if os.path.isfile( errorFile ):
                    # uh-oh this job failed
                    if index in failures:
                        # failed before, increase the failcount
                        failures[index] = failures[index] + 1
                    else:
                        # first failure
                        failures[index] = 1

                    # check if the job failed already 4 times
                    if failures[index] > 4:
                        # that's it.. we will cancel this job and notify the user
                        self.Helper().info( "-------------------------------------------------------" )
                        self.Helper().info( "-------------------------------------------------------" )
                        self.Helper().info( "FAILURE: DRAMMS Job with id " + str( index ) + " failed 4 times! Let's continue without it.." )
                        self.Helper().info( "FAILURE: File " + str( mapFilesToIDs[index] ) + " will NOT be considered during the next steps.." )
                        self.Helper().info( "-------------------------------------------------------" )
                        self.Helper().info( "-------------------------------------------------------" )
                        # now delete the failure flag
                        os.remove( errorFile )

                        # save which file failed
                        self.__failure += "DRAMMS of " + str( mapFilesToIDs[index] ) + " failed!\n"

                        # .. and touch the file
                        open( file, 'w' ).close()

                    else:
                        # let's try this job again

                        self.Helper().info( "DRAMMS Job with id " + str( index ) + " failed in attempt " + str( failures[index] ) + ". Relaunching.." )
                        # now delete the failure flag
                        os.remove( errorFile )

                        # .. and relaunch the job
                        scriptFilePath = outputScriptsDirectory + "script" + str( index ) + ".sh"
                        self.Helper().Execute( schedulerCommand + " " + scriptFilePath )

                        allOutputsExist = False


                elif not os.path.isfile( file ):
                    self.Helper().debug( "DRAMMS Job with id " + str( index ) + " not completed!" )
                    # if only one file does not exist,
                    # we know we have to wait longer
                    allOutputsExist = False


        self.Helper().debug( "All DRAMMS outputs exist!" )

        return alignedSegmentationsOutputDir


    '''=========================================================================================='''
    def CombineToAtlas( self, schedulerCommand, filePathsList, labelsList, reCastString, normalizeTo, outputAtlasDirectory, outputScriptsDirectory, outputNotifyDirectory, sleepValue=5 ):
        '''
            Combine segmentations to an Atlas based on labels. For each label an Atlas gets created. Additionally,
            a combined Atlas is generated. All output is saved.
            
            schedulerCommand
                the schedulerCommand if used else ""            
            filePathsList
                list of existing filepaths to segmentations
            labelsList
                list of labels to analyze and build Atlases for
            reCastString
                re-Cast the Atlases to a certain type defined as a String
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
            normalizeTo
                flag to enable the normalization of Atlas values
                -1: disable
                any other: upper value for normalization
            outputAtlasDirectory
                directory to save the Atlases
            outputScriptsDirectory
                directory to use for generated scripts
            outputNotifyDirectory
                directory to use for notification files                    
            sleepValue
                seconds to wait between each check on completed jobs
                                
            Returns
                TRUE or FALSE depending on success
        '''

        # sanity checks
        if len( filePathsList ) == 0:
            self.Helper().info( "Empty filePathsList for CombineToAtlas() command. Aborting.." )
            return False

        if len( labelsList ) == 0:
            self.Helper().info( "Empty labelsList for CombineToAtlas() command. Aborting.." )
            return False

        if not outputAtlasDirectory:
            self.Helper().info( "Empty outputAtlasDirectory for CombineToAtlas() command. Aborting.." )
            return False

        if not outputScriptsDirectory:
            self.Helper().info( "Empty outputScriptsDirectory for CombineToAtlas() command. Aborting.." )
            return False

        if not outputNotifyDirectory:
            self.Helper().info( "Empty outputNotifyDirectory for CombineToAtlas() command. Aborting.." )
            return False

        # get the launch command for Slicer3
        launchCommandPrefix = self.Helper().GetSlicerLaunchPrefix( False, True )

        uniqueID = 0

        mapFilesToIDs = {}

        # loop through all labels
        for label in labelsList:

            # increase the uniqueID
            uniqueID = uniqueID + 1
            command = ""

            # save the uniqueID and the filePath in our map
            mapFilesToIDs[uniqueID] = "atlas" + str( label )

            # add some args to the Slicer launcher call
            command += str( launchCommandPrefix ) + " --launch " + str( slicer.Application.GetBinDir() ) + os.sep + "ACCombiner";

            # now the args
            command += " " + str( label )

            command += " \""

            for file in filePathsList:
                command += str( file ) + " "

            command += "\""
            command += " " + str( outputAtlasDirectory )
            command += " \"" + str( reCastString ) + "\""
            command += " " + str( normalizeTo )

            self.Helper().debug( "CombineToAtlas Command(s): " + str( command ) )

            # create notification ID
            notify = "echo \"done\" > " + str( outputNotifyDirectory ) + str( uniqueID ) + ".ac"
            notifyError = "echo \"err\" > " + str( outputNotifyDirectory ) + str( uniqueID ) + ".f"

            # now generate a script containing the commands and the notify line
            script = self.Helper().CreateScript( command, notify, notifyError )

            scriptFilePath = outputScriptsDirectory + "script" + str( uniqueID ) + ".sh"

            with open( scriptFilePath, 'w' ) as f:
                f.write( script )

            # set executable permissions
            os.chmod( scriptFilePath, 0777 )

            self.Helper().debug( "Executing generated CombineToAtlas Script: " + scriptFilePath )

            if self.__dryRun:
                self.Helper().info( "DRYRUN - skipping execution.." )
            else:
                self.Helper().Execute( schedulerCommand + " " + scriptFilePath )

        if self.__dryRun:
            return True

        # at this point:
        # either the combination was completed if the os.system call did not send it to the background
        # or it still runs in the background
        # latter is possible, if a cluster scheduler was specified
        # now, we will wait until all output exists

        allOutputsExist = False

        failures = {}

        while not allOutputsExist:
            # not all outputs exist yet
            # get number of existing files
            numberOfExistingFiles = len( glob.glob( outputNotifyDirectory + "*.ac" ) )
            self.Helper().info( "Waiting for CombineToAtlas to complete.. (" + str( numberOfExistingFiles ) + "/" + str( uniqueID ) + " done)" )

            # wait some secs and then check again
            time.sleep( int( sleepValue ) )

            # we assume everything exists
            allOutputsExist = True

            # but now we really check if it is so            
            #for file in outputAlignedImages:
            for index in range( 1, uniqueID + 1 ):

                file = outputNotifyDirectory + str( index ) + ".ac"
                errorFile = outputNotifyDirectory + str( index ) + ".f"

                if os.path.isfile( errorFile ):
                    # uh-oh this job failed
                    if index in failures:
                        # failed before, increase the failcount
                        failures[index] = failures[index] + 1
                    else:
                        # first failure
                        failures[index] = 1

                    # check if the job failed already 4 times
                    if failures[index] > 4:
                        # that's it.. we will cancel this job and notify the user
                        self.Helper().info( "-------------------------------------------------------" )
                        self.Helper().info( "-------------------------------------------------------" )
                        self.Helper().info( "FAILURE: CombineToAtlas Job with id " + str( index ) + " failed 4 times! Let's continue without it.." )
                        self.Helper().info( "-------------------------------------------------------" )
                        self.Helper().info( "-------------------------------------------------------" )
                        # now delete the failure flag
                        os.remove( errorFile )

                        # save which file failed
                        self.__failure += "CombineToAtlas of " + str( mapFilesToIDs[index] ) + " failed!\n"

                        # .. and touch the file
                        open( file, 'w' ).close()

                    else:
                        # let's try this job again

                        self.Helper().info( "CombineToAtlas Job with id " + str( index ) + " failed in attempt " + str( failures[index] ) + ". Relaunching.." )
                        # now delete the failure flag
                        os.remove( errorFile )

                        # .. and relaunch the job
                        scriptFilePath = outputScriptsDirectory + "script" + str( index ) + ".sh"
                        self.Helper().Execute( schedulerCommand + " " + scriptFilePath )

                        allOutputsExist = False



                elif not os.path.isfile( file ):
                    self.Helper().debug( "CombineToAtlas Job with id " + str( index ) + " not completed!" )
                    # if only one file does not exist,
                    # we know we have to wait longer
                    allOutputsExist = False
                    break

        self.Helper().debug( "All CombineToAtlas outputs exist!" )

        return True



    '''=========================================================================================='''
    def PerformPCAAnalysis( self, labelsList, filePathsList, outputDirectory, maxEigenVectors, combine ):
        '''
            Performs PCA Analysis on top of the Atlas Generation.
            
            labelsList
                list of labels to perform PCA analysis for
            filePathsList
                list of existing filepaths to segmentations
            outputDirectory
                directory to write the PCA analysis, trailing slash is required
            maxEigenVectors
                number of max EigenVectors to use for PCA as integer
            combine
                TRUE if all structures should be combined or FALSE
                
            Returns
                TRUE or FALSE depending on success
        '''
        #if len(labelsList) == 0:
        #    self.Helper().info("Empty labelsList for PerformPCAAnalysis() command. Aborting..")
        #    return False

        if len( filePathsList ) == 0:
            self.Helper().info( "Empty filePathsList for PerformPCAAnalysis() command. Aborting.." )
            return False

        if not outputDirectory:
            self.Helper().info( "Empty outputDirectory for PerformPCAAnalysis() command. Aborting.." )
            return False

        # create output dir for PCA distance maps
        # f.e. /tmp/acout/pcaDistanceMaps/
        pcaOutputDirectory = outputDirectory + "PCA" + os.sep
        os.makedirs( pcaOutputDirectory )

        count = 0

        for filePath in filePathsList:

            count = count + 1

            # generate the caseName (without extension)
            # f.e. case1, case2...
            caseName = "case" + str( count )

            # create directory structure for this case
            # f.e. /tmp/acout/pcaDistanceMaps/case01/
            casePcaDistanceMapDirectory = os.path.join( pcaOutputDirectory, caseName ) + os.sep
            os.makedirs( casePcaDistanceMapDirectory )

            for label in labelsList:

                # create input path, which is just the current aligned segmentation
                inputAtlasFilePath = filePath

                # create output path
                # f.e. /tmp/acout/pcaDistanceMaps/case01/X.nrrd, where X is the current label
                outputDistanceMapFilePath = os.path.join( casePcaDistanceMapDirectory, str( label ) + ".nrrd" )

                # generate distance maps
                self.Helper().PCAGenerateDistanceMaps( inputAtlasFilePath, label, outputDistanceMapFilePath, self.__dryRun )

        # now after creating all the distance maps,
        # we can generate the PCA shape representations
        self.Helper().PCAGenerateModel( pcaOutputDirectory, labelsList, self.__dryRun, int( maxEigenVectors ), "nrrd" , int( combine ) )

        return True


