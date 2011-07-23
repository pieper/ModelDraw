from SlicerScriptedModule import ScriptedModuleGUI
from Slicer import slicer
from operator import itemgetter, attrgetter
import glob
import os

from AtlasCreatorHelper import AtlasCreatorHelper
from AtlasCreatorLogic import AtlasCreatorLogic

# Set the event numbers for KWWidgets for easy access
vtkKWPushButton_InvokedEvent = 10000

vtkKWFileBrowserDialog_FileNameChangedEvent = 15000

vtkKWRadioButton_SelectedStateChangedEvent = 10000

vtkKWCheckButton_SelectedStateChangedEvent = 10000

vtkKWEntry_EntryValueChangedEvent = 10000

vtkKWComboBox_EntryValueChangedEvent = 10000

vtkKWSpinBox_SpinBoxValueChangedEvent = 10000

vtkMRMLScene_NodeAddedEvent = 66000

vtkMRMLScene_CloseEvent = 66003

vtkMRMLAtlasCreatorNode_LaunchComputationEvent = 31337

vtkSlicerNodeSelectorWidget_NodeSelectedEvent = 11000


class AtlasCreatorGUI(ScriptedModuleGUI):
    '''
    The AtlasCreator GUI class.
    '''


    '''=========================================================================================='''
    def __init__(self):
        '''
        Constructor of the AtlasCreator GUI class. Sets category, name of the module and creates GUI objects.
        '''

        ScriptedModuleGUI.__init__(self)

        self.SetCategory("Registration")
        self.SetGUIName("AtlasCreator")

        self._moduleFrame = slicer.vtkSlicerModuleCollapsibleFrame()

        self._inputOutputFrame = slicer.vtkSlicerModuleCollapsibleFrame()
        self._parametersFrame = slicer.vtkSlicerModuleCollapsibleFrame()
        
        # I/O Frame
        self._origDirButton = slicer.vtkKWLoadSaveButtonWithLabel()
        self._segDirButton = slicer.vtkKWLoadSaveButtonWithLabel()
        self._outDirButton = slicer.vtkKWLoadSaveButtonWithLabel()                
        self._registrationTypeRadios = slicer.vtkKWRadioButtonSetWithLabel()
        # End I/O Frame
        
        # Parameters Frame
        self._toolkitCombo = slicer.vtkKWComboBoxWithLabel()
        self._deformationTypeRadios = slicer.vtkKWRadioButtonSetWithLabel()
        self._meanIterationsSpinBox = slicer.vtkKWSpinBoxWithLabel()
        self._defaultCaseCombo = slicer.vtkKWComboBoxWithLabel()
        # End Parameters Frame
        
        
        # Advanced Frame
        
        self._advancedFrame = slicer.vtkSlicerModuleCollapsibleFrame()
        self._clusterFrame = slicer.vtkSlicerModuleCollapsibleFrame()
        self._pcaFrame = slicer.vtkSlicerModuleCollapsibleFrame()
        self._transformsFrame = slicer.vtkSlicerModuleCollapsibleFrame()
        self._miscFrame = slicer.vtkSlicerModuleCollapsibleFrame()
        
        # Cluster Frame
        self._clusterCheckBox = slicer.vtkKWCheckButtonWithLabel()
        self._schedulerEntry = slicer.vtkKWEntryWithLabel()        
        # End Cluster Frame
        
        # PCA Frame
        self._pcaCheckBox = slicer.vtkKWCheckButtonWithLabel()
        self._maxEigenVectorsSpinBox = slicer.vtkKWSpinBoxWithLabel()
        self._pcaCombineCheckBox = slicer.vtkKWCheckButtonWithLabel()
        # End PCA Frame
        
        # ExistingTransforms Frame
        self._useExistingTransformsCheckBox = slicer.vtkKWCheckButtonWithLabel()
        self._transformsDirButton = slicer.vtkKWLoadSaveButtonWithLabel()
        self._transformsTemplateButton = slicer.vtkKWLoadSaveButtonWithLabel()
        # End ExistingTransforms Frame
        
        # Misc. Frame
        self._labelsEntry = slicer.vtkKWEntryWithLabel()
        self._normalizeAtlasCheckBox = slicer.vtkKWCheckButtonWithLabel()
        self._normalizeValueEntry = slicer.vtkKWEntryWithLabel()
        self._outputCastCombo = slicer.vtkKWComboBoxWithLabel()
        self._saveTransformsCheckBox = slicer.vtkKWCheckButtonWithLabel()
        self._deleteAlignedImagesCheckBox = slicer.vtkKWCheckButtonWithLabel()
        self._deleteAlignedSegmentationsCheckBox = slicer.vtkKWCheckButtonWithLabel()
        self._ignoreTemplateSegmentationCheckBox = slicer.vtkKWCheckButtonWithLabel()
        self._numberOfThreadsEntry = slicer.vtkKWEntryWithLabel()
        self._debugCheckBox = slicer.vtkKWCheckButtonWithLabel()
        self._dryrunCheckBox = slicer.vtkKWCheckButtonWithLabel() 
        # End Misc. Frame
        
        # End Advanced Frame
        
        
        # The Start Button
        self._launchButton = slicer.vtkKWPushButton()
        

        # Internal Classes and Flags
        self._helper = AtlasCreatorHelper(self)

        self._logic = AtlasCreatorLogic(self)

        self._associatedMRMLNode = None
        self._associatedMRMLNodeTag = None

        self._updating = 0
        
        

    '''=========================================================================================='''
    def Destructor(self):
        '''
        Destruct this class. Gets called automatically.
        '''
        self.GetHelper().guiDebug("Destructor")
        self._helper = None
        self._logic = None
        self._associatedMRMLNode = None
        self._associatedMRMLNodeTag = None
        
        self._updating = None
    
        
    
    '''=========================================================================================='''
    def RemoveLogicObservers(self):
        '''
        Placeholder to remove logic observers. Does not get used here but needs to exist.
        '''
        pass
    
    
        
    '''=========================================================================================='''
    def AddGUIObservers(self):
        '''
        Add the Observers. This method gets called automatically when the module gets created.
        
        For convenience, we also add the MRML observers here.
        '''
        
        # I/O panel
        self._origDirButtonTag = self.AddObserverByNumber(self._origDirButton.GetWidget().GetLoadSaveDialog(),vtkKWFileBrowserDialog_FileNameChangedEvent)
        self._segDirButtonTag = self.AddObserverByNumber(self._segDirButton.GetWidget().GetLoadSaveDialog(),vtkKWFileBrowserDialog_FileNameChangedEvent)
        self._outDirButtonTag = self.AddObserverByNumber(self._outDirButton.GetWidget().GetLoadSaveDialog(),vtkKWFileBrowserDialog_FileNameChangedEvent)
        self._pairFixedRadioTag = self.AddObserverByNumber(self._pairFixedRadio,vtkKWRadioButton_SelectedStateChangedEvent)
        self._pairOnlineRadioTag = self.AddObserverByNumber(self._pairOnlineRadio,vtkKWRadioButton_SelectedStateChangedEvent)
        self._groupOnlineRadioTag = self.AddObserverByNumber(self._groupOnlineRadio,vtkKWRadioButton_SelectedStateChangedEvent)
        
        # parameters panel
        self._toolkitComboTag = self.AddObserverByNumber(self._toolkitCombo.GetWidget(),vtkKWComboBox_EntryValueChangedEvent)
        self._rigidRadioTag = self.AddObserverByNumber(self._rigidRadio,vtkKWRadioButton_SelectedStateChangedEvent)
        self._affineRadioTag = self.AddObserverByNumber(self._affineRadio,vtkKWRadioButton_SelectedStateChangedEvent)
        self._affine12RadioTag = self.AddObserverByNumber(self._affine12Radio,vtkKWRadioButton_SelectedStateChangedEvent)
        self._nonRigidRadioTag = self.AddObserverByNumber(self._nonRigidRadio,vtkKWRadioButton_SelectedStateChangedEvent)
        self._meanIterationsSpinBoxTag = self.AddObserverByNumber(self._meanIterationsSpinBox.GetWidget(),vtkKWSpinBox_SpinBoxValueChangedEvent)
        self._defaultCaseComboTag = self.AddObserverByNumber(self._defaultCaseCombo.GetWidget(),vtkKWComboBox_EntryValueChangedEvent)
        
        # advanced panel
        
        # cluster configuration panel
        self._clusterCheckBoxTag = self.AddObserverByNumber(self._clusterCheckBox.GetWidget(), vtkKWCheckButton_SelectedStateChangedEvent)
        self._schedulerEntryTag = self.AddObserverByNumber(self._schedulerEntry.GetWidget(), vtkKWEntry_EntryValueChangedEvent)
        
        # pca panel
        self._pcaCheckBoxTag = self.AddObserverByNumber(self._pcaCheckBox.GetWidget(), vtkKWCheckButton_SelectedStateChangedEvent)
        self._maxEigenVectorsSpinBoxTag = self.AddObserverByNumber(self._maxEigenVectorsSpinBox.GetWidget(), vtkKWSpinBox_SpinBoxValueChangedEvent)
        self._pcaCombineCheckBoxTag = self.AddObserverByNumber(self._pcaCombineCheckBox.GetWidget(), vtkKWCheckButton_SelectedStateChangedEvent)
        
        # use existing transforms panel
        self._useExistingTransformsCheckBoxTag = self.AddObserverByNumber(self._useExistingTransformsCheckBox.GetWidget(), vtkKWCheckButton_SelectedStateChangedEvent)
        self._transformsDirButtonTag = self.AddObserverByNumber(self._transformsDirButton.GetWidget().GetLoadSaveDialog(),vtkKWFileBrowserDialog_FileNameChangedEvent)
        self._transformsTemplateButtonTag = self.AddObserverByNumber(self._transformsTemplateButton.GetWidget().GetLoadSaveDialog(),vtkKWFileBrowserDialog_FileNameChangedEvent)
        
        # misc. panel
        self._labelsEntryTag = self.AddObserverByNumber(self._labelsEntry.GetWidget(), vtkKWEntry_EntryValueChangedEvent)
        self._saveTransformsCheckBoxTag = self.AddObserverByNumber(self._saveTransformsCheckBox.GetWidget(), vtkKWCheckButton_SelectedStateChangedEvent)
        self._normalizeAtlasCheckBoxTag = self.AddObserverByNumber(self._normalizeAtlasCheckBox.GetWidget(),vtkKWCheckButton_SelectedStateChangedEvent)
        self._normalizeValueEntryTag = self.AddObserverByNumber(self._normalizeValueEntry.GetWidget(), vtkKWEntry_EntryValueChangedEvent)
        self._outputCastComboTag = self.AddObserverByNumber(self._outputCastCombo.GetWidget(),vtkKWComboBox_EntryValueChangedEvent)
        self._deleteAlignedImagesCheckBoxTag = self.AddObserverByNumber(self._deleteAlignedImagesCheckBox.GetWidget(), vtkKWCheckButton_SelectedStateChangedEvent)
        self._deleteAlignedSegmentationsCheckBoxTag = self.AddObserverByNumber(self._deleteAlignedSegmentationsCheckBox.GetWidget(), vtkKWCheckButton_SelectedStateChangedEvent)
        self._ignoreTemplateSegmentationCheckBoxTag = self.AddObserverByNumber(self._ignoreTemplateSegmentationCheckBox.GetWidget(), vtkKWCheckButton_SelectedStateChangedEvent)
        self._numberOfThreadsEntryTag = self.AddObserverByNumber(self._numberOfThreadsEntry.GetWidget(),vtkKWEntry_EntryValueChangedEvent)
        self._debugCheckBoxTag = self.AddObserverByNumber(self._debugCheckBox.GetWidget(),vtkKWCheckButton_SelectedStateChangedEvent)
        self._dryrunCheckBoxTag = self.AddObserverByNumber(self._dryrunCheckBox.GetWidget(),vtkKWCheckButton_SelectedStateChangedEvent)

        # the launch button
        self._launchButtonTag = self.AddObserverByNumber(self._launchButton,vtkKWPushButton_InvokedEvent)
        
        # MRML observer
        self._mrmlNodeAddedTag = self.AddMRMLObserverByNumber(slicer.MRMLScene,vtkMRMLScene_NodeAddedEvent)
        self._mrmlSceneCloseTag = self.AddMRMLObserverByNumber(slicer.MRMLScene,vtkMRMLScene_CloseEvent)



    '''=========================================================================================='''
    def RemoveGUIObservers(self):
        '''
        Removes all created GUI observers. Gets called automatically.
        
        For convenience, we also remove the MRML Scene observers here.
        '''

        # I/O panel
        self.RemoveObserver(self._origDirButtonTag)
        self.RemoveObserver(self._segDirButtonTag)
        self.RemoveObserver(self._outDirButtonTag)
        self.RemoveObserver(self._pairFixedRadioTag)
        self.RemoveObserver(self._pairOnlineRadioTag)
        self.RemoveObserver(self._groupOnlineRadioTag)
        
        # parameters panel
        self.RemoveObserver(self._toolkitComboTag)
        self.RemoveObserver(self._rigidRadioTag)
        self.RemoveObserver(self._affineRadioTag)
        self.RemoveObserver(self._affine12RadioTag)
        self.RemoveObserver(self._nonRigidRadioTag)
        self.RemoveObserver(self._meanIterationsSpinBoxTag)
        self.RemoveObserver(self._defaultCaseComboTag)
        
        # advanced panel
        
        # cluster configuration panel
        self.RemoveObserver(self._clusterCheckBoxTag)
        self.RemoveObserver(self._schedulerEntryTag)
        
        # pca panel
        self.RemoveObserver(self._pcaCheckBoxTag)
        self.RemoveObserver(self._maxEigenVectorsSpinBoxTag)
        self.RemoveObserver(self._pcaCombineCheckBoxTag)
        
        # use existing transforms model
        self.RemoveObserver(self._useExistingTransformsCheckBoxTag)
        self.RemoveObserver(self._transformsDirButtonTag)
        self.RemoveObserver(self._transformsTemplateButtonTag)
        
        # misc. panel
        self.RemoveObserver(self._labelsEntryTag)
        self.RemoveObserver(self._saveTransformsCheckBoxTag)
        self.RemoveObserver(self._normalizeAtlasCheckBoxTag)
        self.RemoveObserver(self._normalizeValueEntryTag)
        self.RemoveObserver(self._outputCastComboTag)
        self.RemoveObserver(self._deleteAlignedImagesCheckBoxTag)
        self.RemoveObserver(self._deleteAlignedSegmentationsCheckBoxTag)
        self.RemoveObserver(self._ignoreTemplateSegmentationCheckBoxTag)
        self.RemoveObserver(self._numberOfThreadsEntryTag)
        self.RemoveObserver(self._debugCheckBoxTag)
        self.RemoveObserver(self._dryrunCheckBoxTag)
        
        # the launch button
        self.RemoveObserver(self._launchButtonTag)
    
    
    
    '''=========================================================================================='''    
    def RemoveMRMLNodeObservers(self):
        '''
        Remove MRML Node and MRML Scene Observers
        '''
        
        self.RemoveMRMLObserverByNumber(slicer.MRMLScene,vtkMRMLScene_NodeAddedEvent)
        self.RemoveMRMLObserverByNumber(slicer.MRMLScene,vtkMRMLScene_CloseEvent)
        
        #if self._associatedMRMLNode and self._associatedMRMLNodeTag:
        #    self.RemoveMRMLObserverByNumber(self._associatedMRMLNode,vtkMRMLAtlasCreatorNode_LaunchComputationEvent)
        


    '''=========================================================================================='''
    def ProcessGUIEvents(self,caller,event):
        '''
        Gets called on GUI Events to handle them.
        '''

        self.GetHelper().guiDebug("ProcessGUIEvents" + str(self._updating))

        # the launch button
        if caller == self._launchButton and event == vtkKWPushButton_InvokedEvent:
            self.UpdateMRML()
            self.Launch()
            
        # I/O panel
        elif caller == self._origDirButton.GetWidget().GetLoadSaveDialog() and event == vtkKWFileBrowserDialog_FileNameChangedEvent:                
            self.UpdateCaseCombobox(self._origDirButton.GetWidget().GetFileName(),self._segDirButton.GetWidget().GetFileName())
            self.UpdateMRML()
            
        elif caller == self._segDirButton.GetWidget().GetLoadSaveDialog() and event == vtkKWFileBrowserDialog_FileNameChangedEvent:
            self.UpdateCaseCombobox(self._origDirButton.GetWidget().GetFileName(),self._segDirButton.GetWidget().GetFileName())
            self.UpdateMRML()
            
        elif caller == self._outDirButton.GetWidget().GetLoadSaveDialog() and event == vtkKWFileBrowserDialog_FileNameChangedEvent:
            self.UpdateMRML()             
               
        elif caller == self._pairFixedRadio and event == vtkKWRadioButton_SelectedStateChangedEvent:
            self.UpdateCaseCombobox(self._origDirButton.GetWidget().GetFileName(),self._segDirButton.GetWidget().GetFileName())
            self.ToggleMeanAndDefaultCase1()
            self.UpdateMRML()
            
        elif caller == self._pairOnlineRadio and event == vtkKWRadioButton_SelectedStateChangedEvent:
            self.ToggleMeanAndDefaultCase2()
            self.UpdateMRML()
            
        elif caller == self._groupOnlineRadio and event == vtkKWRadioButton_SelectedStateChangedEvent:
            self.UpdateMRML()                
            
        # parameter panel
        elif caller == self._toolkitCombo.GetWidget() and event == vtkKWComboBox_EntryValueChangedEvent:
            self.UpdateMRML()
            
        elif caller == self._affineRadio and event == vtkKWRadioButton_SelectedStateChangedEvent:
            self.UpdateMRML()
            
        elif caller == self._nonRigidRadio and event == vtkKWRadioButton_SelectedStateChangedEvent:
            self.UpdateMRML()
            
        elif caller == self._meanIterationsSpinBox.GetWidget() and event == vtkKWSpinBox_SpinBoxValueChangedEvent:
            self.UpdateMRML()
            
        elif caller == self._defaultCaseCombo.GetWidget() and event == vtkKWComboBox_EntryValueChangedEvent:
            self.UpdateLabelList()
            self.UpdateMRML()
            
        # advanced panel
        
        # cluster configuration panel
        elif caller == self._clusterCheckBox.GetWidget() and event == vtkKWCheckButton_SelectedStateChangedEvent:
            self.UpdateMRML()
        
        elif caller == self._schedulerEntry.GetWidget() and event == vtkKWEntry_EntryValueChangedEvent:
            self.UpdateMRML()
            
        # pca panel
        elif caller == self._pcaCheckBox.GetWidget() and event == vtkKWCheckButton_SelectedStateChangedEvent:
            self.UpdateMRML()
            
        elif caller == self._maxEigenVectorsSpinBox.GetWidget() and event == vtkKWSpinBox_SpinBoxValueChangedEvent:
            self.UpdateMRML()
            
        elif caller == self._pcaCombineCheckBox.GetWidget() and event == vtkKWCheckButton_SelectedStateChangedEvent:
            self.UpdateMRML()
            
        # use existing transforms panel
        elif caller == self._useExistingTransformsCheckBox.GetWidget() and event == vtkKWCheckButton_SelectedStateChangedEvent:
            self.UpdateMRML()
            
        elif caller == self._transformsDirButton.GetWidget().GetLoadSaveDialog() and event == vtkKWFileBrowserDialog_FileNameChangedEvent:
            self.UpdateMRML()
        
        elif caller == self._transformsTemplateButton.GetWidget().GetLoadSaveDialog() and event == vtkKWFileBrowserDialog_FileNameChangedEvent:
            self.UpdateLabelList()
            self.UpdateMRML()
                    
        # misc panel
        elif caller == self._labelsEntry.GetWidget() and event == vtkKWEntry_EntryValueChangedEvent:
            self.UpdateMRML()
            
        elif caller == self._saveTransformsCheckBox.GetWidget() and event == vtkKWCheckButton_SelectedStateChangedEvent:
            self.UpdateMRML()
            
        elif caller == self._normalizeAtlasCheckBox.GetWidget() and event == vtkKWCheckButton_SelectedStateChangedEvent:
            self.ToggleNormalize()
            self.UpdateMRML()
            
        elif caller == self._normalizeValueEntry.GetWidget() and event == vtkKWEntry_EntryValueChangedEvent:
            self.UpdateMRML()
            
        elif caller == self._outputCastCombo.GetWidget() and event == vtkKWComboBox_EntryValueChangedEvent:
            self.UpdateMRML()
            
        elif caller == self._deleteAlignedImagesCheckBox.GetWidget() and event == vtkKWCheckButton_SelectedStateChangedEvent:
            self.UpdateMRML()
            
        elif caller == self._deleteAlignedSegmentationsCheckBox.GetWidget() and event == vtkKWCheckButton_SelectedStateChangedEvent:
            self.UpdateMRML()
            
        elif caller == self._ignoreTemplateSegmentationCheckBox.GetWidget() and event == vtkKWCheckButton_SelectedStateChangedEvent:
            self.UpdateMRML()
            
        elif caller == self._numberOfThreadsEntry.GetWidget() and event == vtkKWEntry_EntryValueChangedEvent:
            self.UpdateMRML()
            
        elif caller == self._debugCheckBox.GetWidget() and event == vtkKWCheckButton_SelectedStateChangedEvent:
            self.UpdateMRML()
            
        elif caller == self._dryrunCheckBox.GetWidget() and event == vtkKWCheckButton_SelectedStateChangedEvent:
            self.UpdateMRML()
        
        
        
    '''=========================================================================================='''                
    def CheckForCongeal(self):
        '''
        Checks if Congeal Registration Tool is available in Slicer.
        
        Returns 1 if Congeal is installed or 0 otherwise.
        '''
        if os.path.isfile(str(slicer.Application.GetBinDir())+os.sep+"Congeal") or os.path.isfile(str(slicer.Application.GetBinDir())+os.sep+"Congeal.exe"):
            
            return 1
            
        else:
            
            return 0
    
    
        
    '''=========================================================================================='''                
    def ToggleNormalize(self):
        '''
        Callback for the normalize checkbox. Gets called when the user clicks on it.
        '''
        self.GetHelper().guiDebug("ToggleNormalize " + str(self._updating))
        # check if we already process an update event
        # and jump out if yes
        if not self._updating:        
            
            self._updating = 1
            
            normalize = self._normalizeAtlasCheckBox.GetWidget().GetSelectedState()
            
            if normalize:
                self._normalizeValueEntry.SetEnabled(1)
            else:
                self._normalizeValueEntry.SetEnabled(0)
                
            self._updating = 0
            
                
                
    '''=========================================================================================='''                
    def ToggleMeanAndDefaultCase1(self):
        '''
        Callback for the pairFixed checkbox. Gets called when the user clicks on the checkbox.
        '''
        self.GetHelper().guiDebug("ToggleMeanAndDefaultCase1 " + str(self._updating))
        # check if we already process an update event
        # and jump out if yes
        if not self._updating:
            
            self._updating = 1
            
            useDefCase = self._pairFixedRadio.GetSelectedState()
    
            if useDefCase:
                self._pairOnlineRadio.SetSelectedState(0)
                #self._fixedRadio.SetSelectedState(1)
                self._defaultCaseCombo.SetEnabled(1)
                self._meanIterationsSpinBox.SetEnabled(0)
            else:
                self._pairOnlineRadio.SetSelectedState(1)
                #self._fixedRadio.SetSelectedState(0)
                self._defaultCaseCombo.SetEnabled(0)
                self._meanIterationsSpinBox.SetEnabled(1)
                
            self._updating = 0



    '''=========================================================================================='''
    def ToggleMeanAndDefaultCase2(self):
        '''
        Callback for the pairOnline checkbox. Gets called when the user clicks on the checkbox.
        '''
        self.GetHelper().guiDebug("ToggleMeanAndDefaultCase2 " + str(self._updating))
        # check if we already process an update event
        # and jump out if yes
        if not self._updating:
            
            self._updating = 1
            
            useMean = self._pairOnlineRadio.GetSelectedState()
    
            if useMean:
                #self._dynamicRadio.SetSelectedState(1)
                self._pairFixedRadio.SetSelectedState(0)
                self._defaultCaseCombo.SetEnabled(0)
                self._meanIterationsSpinBox.SetEnabled(1)
            else:
                #self._dynamicRadio.SetSelectedState(0)
                self._pairFixedRadio.SetSelectedState(1)
                self._defaultCaseCombo.SetEnabled(1)
                self._meanIterationsSpinBox.SetEnabled(0)
                
            self._updating = 0



    '''=========================================================================================='''
    def UpdateCaseCombobox(self,imagesDir,segmentationsDir):
        '''
        Updates the defaultCaseCombobox by checking which files are available as originals and segmentations and
        reading the basenames of these files.
        
        imagesDir - the filePath to the directory of the images
        segmentationsDir - the filePath to the directory of the segmentations
        
        '''
        self.GetHelper().guiDebug("UpdateCaseCombobox " + str(self._updating))
        # check if we already process an update event
        # and jump out if yes
        if not self._updating:        
            
            self._updating = 1
            
            if imagesDir and segmentationsDir:
                
                # originals and segmentations dir were configured, now we parse for potential image data
                nrrdFiles = glob.glob(os.path.join(imagesDir, '*.nrrd'))
                nhdrFiles = glob.glob(os.path.join(imagesDir, '*.nhdr'))
                hdrFiles = glob.glob(os.path.join(imagesDir, '*.hdr'))          
                mhdFiles = glob.glob(os.path.join(imagesDir, '*.mhd'))          
                mhaFiles = glob.glob(os.path.join(imagesDir, '*.mha'))
            
                listOfFiles = [
                              ('.nrrd',len(nrrdFiles)),
                              ('.nhdr',len(nhdrFiles)),
                              ('.hdr',len(hdrFiles)),
                              ('.mhd',len(mhdFiles)),
                              ('.mha',len(mhaFiles)),                               
                              ]
    
                self._helper.debug("Found "+str(sorted(listOfFiles, key=itemgetter(1), reverse=True)[0][1])+" files of type "+str(sorted(listOfFiles, key=itemgetter(1), reverse=True)[0][0]))
    
                for nrrdFile in nrrdFiles:
                    caseFile = os.path.basename(nrrdFile)
                    if os.path.isfile(os.path.join(segmentationsDir,caseFile)):
                        # file exists in originals and segmentations directory, so we can add it to our list of cases
                        self._defaultCaseCombo.GetWidget().AddValue("'" + caseFile + "'")
                        self._defaultCaseCombo.GetWidget().SetValue("'" + caseFile + "'")
                
            self._updating = 0
            
            # get the labels from last selected defaultCaseEntry value
            self.UpdateLabelList()
                        
                    
                    
    '''=========================================================================================='''                    
    def UpdateLabelList(self):
        '''
        Updates the list of labels in the GUI according to the first of all segmentations or if a defaultCase is
        specified, then takes the labels from this file.
        '''
        
        self.GetHelper().guiDebug("UpdateLabelList " + str(self._updating))
        
        if not self._updating:
            
            self._updating = 1
        
            defaultCaseSegmentationFilePath = ""
        
            if self._segDirButton.GetWidget().GetFileName():
                # only perform the following if a segmentation directory was selected
                listOfSegmentations = self.GetHelper().ConvertDirectoryToList(self._segDirButton.GetWidget().GetFileName())
                    
                labelList = self.GetHelper().ReadLabelsFromImages(listOfSegmentations)
                
                if labelList:
                    labelListAsString = self.GetHelper().ConvertListToString(labelList)
                        
                    self._labelsEntry.GetWidget().SetValue(labelListAsString)
                
            self._updating = 0
                               
                               
                               
    '''=========================================================================================='''
    def Launch(self):
        ''' 
        Starts an Atlas Creator computation by passing the associated MRML Node to the logic.
        
        Returns FALSE on failure
        '''

        # sanity checks
        if not self._origDirButton.GetWidget().GetFileName() and not self._useExistingTransformsCheckBox.GetWidget().GetSelectedState():
            # we only need the original images if we do not skip the registration
            self.GetHelper().info("ERROR! The Original Images Directory was not set.. Aborting..")
            return False
        
        if not self._segDirButton.GetWidget().GetFileName():
            self.GetHelper().info("ERROR! The Segmentation Directory was not set.. Aborting..")
            return False                
            
        if not self._outDirButton.GetWidget().GetFileName():
            self.GetHelper().info("ERROR! The Output Directory was not set.. Aborting..")
            return False            

        # the associated MRML Node holds the configuration of the AtlasCreator
        # we definitely need it here!
        if not self._associatedMRMLNode:
            self.GetHelper().info("ERROR! No MRML Node was configured.. Aborting..")
            return False

        result = self.GetMyLogic().Start(self._associatedMRMLNode)
        
        if result and not self._pcaCheckBox.GetWidget().GetSelectedState():
            # we can only display atlases if we did not activate PCA


            # now loading the template
            templateID = self.GetHelper().DisplayImageInSlicer(os.path.normpath(self._associatedMRMLNode.GetOutputDirectory()+"/template.nrrd"),'Template')

            #
            # now load all the atlases
            labelsList = self._associatedMRMLNode.GetLabelsList()
            self.GetHelper().guiDebug(labelsList)
            if type(labelsList).__name__ =='int':
                # if yes, convert it from int to a list
                tmpList = []
                tmpList.append(labelsList)
                labelsList = tmpList
        
            self.GetHelper().guiDebug(labelsList)
        
            # we check now if we have a valid list of labels
            if type(labelsList).__name__=='list' and len(labelsList) > 1:            
                
                # only loop through the labels if they are a valid list
                for currentLabel in labelsList:
                    self.GetHelper().guiDebug(currentLabel) 
                    # loading atlas for label i
                    self.GetHelper().DisplayImageInSlicer(os.path.normpath(self._associatedMRMLNode.GetOutputDirectory()+"/atlas"+str(currentLabel)+".nrrd"),'AtlasForLabel'+str(currentLabel))

            
            # now select the template as the active volume in the sliceViewers
            selectionNode = slicer.ApplicationLogic.GetSelectionNode()
            selectionNode.SetReferenceActiveVolumeID(templateID)
            slicer.ApplicationLogic.PropagateVolumeSelection()
            
            
            
    '''=========================================================================================='''            
    def UpdateMRMLFromCallback(self,dummyArgument=None):
        '''
        Just passes through to UpdateMRML.
        
        The dummyArgument is a placeholder when this function is called by a ComboBox callback and the current
        value is attached automatically by KWWidgets.
        '''
        self.UpdateMRML()
        


    '''=========================================================================================='''
    def UpdateMRML(self):
        '''
        Writes the current GUI to the MRML Node. This is called whenever an element on the GUI
        changes.
        '''


        # check if we already process an update event
        # and jump out if yes
        if not self._updating:

            self.GetHelper().guiDebug("UpdateMRML: " + str(self._updating))

            # start blocking of update events
            self._updating = 1
            
            if not self._associatedMRMLNode:
                # if we do not have an associated MRML Node, we will create one right here!!
                self._associatedMRMLNode = slicer.vtkMRMLAtlasCreatorNode()
                self._associatedMRMLNode.InitializeByDefault()
                # add it to the scene
                slicer.MRMLScene.AddNode(self._associatedMRMLNode)
                
            #self.GetHelper().info(self._associatedMRMLNode)                
                

            ###############################################################
            # input panel
            
            imagesDir = self._origDirButton.GetWidget().GetFileName()
            if imagesDir:
                self._associatedMRMLNode.SetOriginalImagesFilePathList(self.GetHelper().ConvertDirectoryToString(imagesDir))

            segmentationsDir = self._segDirButton.GetWidget().GetFileName()
            if segmentationsDir:
                self._associatedMRMLNode.SetSegmentationsFilePathList(self.GetHelper().ConvertDirectoryToString(segmentationsDir))
                
            outputDir = self._outDirButton.GetWidget().GetFileName()
            if outputDir:
                self._associatedMRMLNode.SetOutputDirectory(str(outputDir))
            
            if self._groupOnlineRadio.GetSelectedState():
                self._associatedMRMLNode.SetTemplateType("group")            
            elif self._pairOnlineRadio.GetSelectedState():
                self._associatedMRMLNode.SetTemplateType("dynamic")
            else:
                self._associatedMRMLNode.SetTemplateType("fixed")

            
            ###############################################################    
            # parameters panel
            
            toolkit = self._toolkitCombo.GetWidget().GetValue()
            if toolkit:
                self._associatedMRMLNode.SetToolkit(str(toolkit).strip("'"))
                
            if self._rigidRadio.GetSelectedState():
                self._associatedMRMLNode.SetRegistrationType("Rigid")
            elif self._affineRadio.GetSelectedState():
                self._associatedMRMLNode.SetRegistrationType("Affine")
            elif self._affine12Radio.GetSelectedState():
                self._associatedMRMLNode.SetRegistrationType("Affine12")
            elif self._nonRigidRadio.GetSelectedState():
                self._associatedMRMLNode.SetRegistrationType("Non-Rigid")
            else:
                # affine is default
                self._associatedMRMLNode.SetRegistrationType("Affine")

            meanIterations = self._meanIterationsSpinBox.GetWidget().GetValue()
            if meanIterations:
                self._associatedMRMLNode.SetDynamicTemplateIterations(int(meanIterations))
            
            defaultCase = self._defaultCaseCombo.GetWidget().GetValue()
            if defaultCase and segmentationsDir:
                defaultCaseFullPath = str(self._origDirButton.GetWidget().GetFileName()) + os.sep + str(defaultCase).strip("'")
                self._associatedMRMLNode.SetFixedTemplateDefaultCaseFilePath(defaultCaseFullPath)
            
            
            ###############################################################    
            # advanced panel
            
            # cluster configuration
            #
            if self._clusterCheckBox.GetWidget().GetSelectedState():
                self._associatedMRMLNode.SetUseCluster(1)
            else:
                self._associatedMRMLNode.SetUseCluster(0)
                
            schedCommand = self._schedulerEntry.GetWidget().GetValue()
            if schedCommand:
                self._associatedMRMLNode.SetSchedulerCommand(str(schedCommand))
                
            # principal component analysis
            #
            if self._pcaCheckBox.GetWidget().GetSelectedState():
                self._associatedMRMLNode.SetPCAAnalysis(1)
            else:
                self._associatedMRMLNode.SetPCAAnalysis(0)
            
            pcaMaxEigenVectors = self._maxEigenVectorsSpinBox.GetWidget().GetValue()
            if pcaMaxEigenVectors:
                self._associatedMRMLNode.SetPCAMaxEigenVectors(int(pcaMaxEigenVectors))
                
            if self._pcaCombineCheckBox.GetWidget().GetSelectedState():
                self._associatedMRMLNode.SetPCACombine(1)
            else:
                self._associatedMRMLNode.SetPCACombine(0)
            
            # use existing transforms
            #
            if self._useExistingTransformsCheckBox.GetWidget().GetSelectedState():
                self._associatedMRMLNode.SetSkipRegistration(1)
            else:
                self._associatedMRMLNode.SetSkipRegistration(0)
                
            transformsDir = self._transformsDirButton.GetWidget().GetFileName()
            if transformsDir:
                self._associatedMRMLNode.SetTransformsDirectory(str(transformsDir))
                
            transformsTemplate = self._transformsTemplateButton.GetWidget().GetFileName()
            if transformsTemplate:
                self._associatedMRMLNode.SetExistingTemplate(str(os.path.normpath(transformsTemplate)))
            
            # Misc.
            #
            labels = self._labelsEntry.GetWidget().GetValue()
            if labels:
                self._associatedMRMLNode.SetLabelsList(str(self.GetHelper().ConvertListToString(labels)))
                
            if self._saveTransformsCheckBox.GetWidget().GetSelectedState():
                self._associatedMRMLNode.SetSaveTransforms(1)
            else:
                self._associatedMRMLNode.SetSaveTransforms(0)
                
            if self._normalizeAtlasCheckBox.GetWidget().GetSelectedState():
                self._associatedMRMLNode.SetNormalizeAtlases(1)
            else:
                self._associatedMRMLNode.SetNormalizeAtlases(0)
                
            normalizeTo = self._normalizeValueEntry.GetWidget().GetValue()
            if normalizeTo:
                self._associatedMRMLNode.SetNormalizeTo(str(normalizeTo))
                
            outputCast = self._outputCastCombo.GetWidget().GetValue()
            if outputCast:
                self._associatedMRMLNode.SetOutputCast(str(outputCast).strip("'"))
                
            if self._deleteAlignedImagesCheckBox.GetWidget().GetSelectedState():
                self._associatedMRMLNode.SetDeleteAlignedImages(1)
            else:
                self._associatedMRMLNode.SetDeleteAlignedImages(0)
                
            if self._deleteAlignedSegmentationsCheckBox.GetWidget().GetSelectedState():
                self._associatedMRMLNode.SetDeleteAlignedSegmentations(1)
            else:
                self._associatedMRMLNode.SetDeleteAlignedSegmentations(0)
                
            if self._ignoreTemplateSegmentationCheckBox.GetWidget().GetSelectedState():
                self._associatedMRMLNode.SetIgnoreTemplateSegmentation(1)
            else:
                self._associatedMRMLNode.SetIgnoreTemplateSegmentation(0)
                
            numberOfThreads = self._numberOfThreadsEntry.GetWidget().GetValue()
            if numberOfThreads:
                self._associatedMRMLNode.SetNumberOfThreads(int(numberOfThreads))
                
            if self._debugCheckBox.GetWidget().GetSelectedState():
                self._associatedMRMLNode.SetDebugMode(1)
            else:
                self._associatedMRMLNode.SetDebugMode(0)
                
            if self._dryrunCheckBox.GetWidget().GetSelectedState():
                self._associatedMRMLNode.SetDryrunMode(1)
            else:
                self._associatedMRMLNode.SetDryrunMode(0)

            # stop blocking of update events
            self._updating = 0



    '''=========================================================================================='''
    def UpdateGUI(self):
        '''
        Reads the MRML node and propagates it to the GUI. This is called whenever the MRML node gets
        changed externally.
        '''
        
        self.GetHelper().guiDebug("UpdateGUI" + str(self._updating))
        # check if we already process an update event
        # and jump out if yes        
        if not self._updating:

            if not self._associatedMRMLNode:
                # no MRML node associated, jump out
                return False

            # start blocking of update events
            self._updating = 1

            # reset everything to default
            self.InitializeByDefault()

            n = self._associatedMRMLNode
            
            #self.GetHelper().info(n)

            ###############################################################
            # input panel
            
            imagesDirListString = n.GetOriginalImagesFilePathList()
            # split the string by space
            imagesDirList = imagesDirListString.split(" ")
            if imagesDirList:
                firstItem = imagesDirList[0]
                
                if firstItem:
                    # get the directory of the first item
                    imagesDir = os.path.split(firstItem)[0]
                    if imagesDir:
                        # propagate it to the GUI
                        self._origDirButton.GetWidget().SetInitialFileName(str(imagesDir))
                        # but we only display the last folderName
                        self._origDirButton.GetWidget().SetText(os.path.basename(imagesDir))
                        
            segmentationsDirListString = n.GetSegmentationsFilePathList()
            # split the string by space
            segmentationsDirList = segmentationsDirListString.split(" ")
            if segmentationsDirList:
                firstItem = segmentationsDirList[0]
                
                if firstItem:
                    # get the directory of the first item
                    segmentationsDir = os.path.split(firstItem)[0]
                    if segmentationsDir:
                        # propagate it to the GUI
                        self._segDirButton.GetWidget().SetInitialFileName(str(segmentationsDir))
                        # but we only display the last folderName
                        self._segDirButton.GetWidget().SetText(os.path.basename(segmentationsDir))
                        
            # at this point, perform some updates in the GUI depending on the file selections
            # we need to jump out of the updating block for this
            self._updating = 0
            if imagesDir and segmentationsDir:
                self.UpdateCaseCombobox(imagesDir,segmentationsDir)
            self._updating = 1
                        
            outputDirString = n.GetOutputDirectory()
            if outputDirString:
                self._outDirButton.GetWidget().SetInitialFileName(str(outputDirString))
                self._outDirButton.GetWidget().SetText(os.path.basename(outputDirString))                                    
            
            templateTypeString = n.GetTemplateType()
            if templateTypeString == "fixed":
                # fixed registration
                self._pairFixedRadio.SetSelectedState(1)
                
                self._updating = 0
                # call the fixed pair handler
                self.ToggleMeanAndDefaultCase1()
                self._updating = 1
                
            elif templateTypeString == "dynamic":
                # dynamic registration
                self._pairOnlineRadio.SetSelectedState(1)
                
                self._updating = 0
                # call the dynamic pair handler
                self.ToggleMeanAndDefaultCase2()
                self._updating = 1
                
            elif templateTypeString == "group":
                # group online
                # but only activate it in the GUI if Congeal is installed
                if self.CheckForCongeal():
                    self._groupOnlineRadio.SetEnabled(1)
                    self._groupOnlineRadio.SetSelectedState(1)
                else:
                    self._groupOnlineRadio.SetEnabled(0)
                    # fall back to fixed registration
                    self._pairFixedRadio.SetSelectedState(1)
                    
                    self._updating = 0
                    # call the fixed pair handler
                    self.ToggleMeanAndDefaultCase1()
                    self._updating = 1                    

                
            ###############################################################
            # parameters panel
            
            toolkitString = n.GetToolkit()
            if toolkitString == "CMTK":
                self._toolkitCombo.GetWidget().SetValue("'CMTK'")
                
            registrationTypeString = n.GetRegistrationType()
            if registrationTypeString == "Rigid":
                self._rigidRadio.SetSelectedState(1)
                self._affineRadio.SetSelectedState(0)
                self._affine12Radio.SetSelectedState(0)
                self._nonRigidRadio.SetSelectedState(0)
            elif registrationTypeString == "Affine":
                self._rigidRadio.SetSelectedState(0)
                self._affineRadio.SetSelectedState(1)
                self._affine12Radio.SetSelectedState(0)
                self._nonRigidRadio.SetSelectedState(0)
            elif registrationTypeString == "Affine12":
                self._rigidRadio.SetSelectedState(0)
                self._affineRadio.SetSelectedState(0)
                self._affine12Radio.SetSelectedState(1)
                self._nonRigidRadio.SetSelectedState(0)
            elif registrationTypeString == "Non-Rigid":
                self._rigidRadio.SetSelectedState(0)
                self._affineRadio.SetSelectedState(0)
                self._affine12Radio.SetSelectedState(0)
                self._nonRigidRadio.SetSelectedState(1)
                
            meanIterationsInt = int(n.GetDynamicTemplateIterations())
            if meanIterationsInt > -1:
                self._meanIterationsSpinBox.GetWidget().SetValue(meanIterationsInt)
            
            defaultCaseString = n.GetFixedTemplateDefaultCaseFilePath()
            if defaultCaseString:
                self._defaultCaseCombo.GetWidget().SetValue("'" + os.path.basename(defaultCaseString) + "'")
                
            
            ###############################################################
            # advanced panel
            
            # cluster configuration
            #
            useCluster = n.GetUseCluster()
            if useCluster:
                self._clusterCheckBox.GetWidget().SetSelectedState(1)
            
            schedCommandString = n.GetSchedulerCommand()
            if schedCommandString:
                self._schedulerEntry.GetWidget().SetValue(str(schedCommandString))
    
                
            # principal component analysis
            #
            usePCA = n.GetPCAAnalysis()
            if usePCA: 
                self._pcaCheckBox.GetWidget().SetSelectedState(1)
                
            pcaMaxEigenVectorsInteger = int(n.GetPCAMaxEigenVectors())
            if pcaMaxEigenVectorsInteger > -1:
                self._maxEigenVectorsSpinBox.GetWidget().SetValue(pcaMaxEigenVectorsInteger)
                
            pcaCombine = n.GetPCACombine()
            if pcaCombine:
                self._pcaCombineCheckBox.GetWidget().SetSelectedState(1)
                
                    
            # use existing transforms
            #
            skipRegistration = n.GetSkipRegistration()
            if skipRegistration:
                self._useExistingTransformsCheckBox.GetWidget().SetSelectedState(1)
            
            transformsDir = n.GetTransformsDirectory()
            if transformsDir:
                self._transformsDirButton.GetWidget().SetInitialFileName(str(transformsDir))
                self._transformsDirButton.GetWidget().SetText(os.path.basename(transformsDir))

            transformsTemplate = n.GetExistingTemplate()
            if transformsTemplate:
                self._transformsTemplateButton.GetWidget().SetInitialFileName(str(transformsTemplate))
                self._transformsTemplateButton.GetWidget().SetText(os.path.basename(transformsTemplate))
                    
            # Misc.
            #
            labelsList = n.GetLabelsList()
            if type(labelsList).__name__ =='int':
                # if yes, convert it from int to a list
                tmpList = []
                tmpList.append(labelsList)
                labelsList = tmpList
            
            # we check now if we have a valid list of labels
            if type(labelsList).__name__=='list' and len(labelsList) > 1:
                
                labelsListString = ""
                
                # loop through the list and convert it to a string
                for l in labelsList:
                    labelsListString += str(l)
                    labelsListString += " "
                
                # and set it to the GUI
                self._labelsEntry.GetWidget().SetValue(labelsListString.rstrip())
                    
            saveTransforms = n.GetSaveTransforms()
            if not saveTransforms:
                self._saveTransformsCheckBox.GetWidget().SetSelectedState(0)
                
            normalizeAtlas = n.GetNormalizeAtlases()
            if normalizeAtlas:
                self._normalizeAtlasCheckBox.GetWidget().SetSelectedState(1)
                
            normalizeToInteger = int(n.GetNormalizeTo())
            if normalizeToInteger > 0:
                self._normalizeValueEntry.GetWidget().SetValue(normalizeToInteger)
            
            # trigger normalize handler
            self._updating = 0
            self.ToggleNormalize()
            self._updating = 1
            
            outputCastString = str(n.GetOutputCast())
            if outputCastString:
                self._outputCastCombo.GetWidget().SetValue("'" + outputCastString + "'")
                
            deleteAlignedImages = n.GetDeleteAlignedImages()
            if not deleteAlignedImages:
                self._deleteAlignedImagesCheckBox.GetWidget().SetSelectedState(0)
                
            deleteAlignedSegmentations = n.GetDeleteAlignedSegmentations()
            if not deleteAlignedSegmentations:
                self._deleteAlignedSegmentationsCheckBox.GetWidget().SetSelectedState(0)
            
            ignoreTemplateSegmentation = n.GetIgnoreTemplateSegmentation()
            if ignoreTemplateSegmentation:
                self._ignoreTemplateSegmentationCheckBox.GetWidget().SetSelectedState(1)
            
            numberOfThreads = n.GetNumberOfThreads()
            if numberOfThreads:
                self._numberOfThreadsEntry.GetWidget().SetValue(str(numberOfThreads))
            
            debug = n.GetDebugMode()
            if debug:
                self._debugCheckBox.GetWidget().SetSelectedState(1)
                
            dryrun = n.GetDryrunMode()
            if dryrun:
                self._dryrunCheckBox.GetWidget().SetSelectedState(1)
                
            
            # stop blocking of update events
            self._updating = 0
            #self.GetHelper().info("End of UpdateGUI")



    '''=========================================================================================='''
    def ProcessMRMLEvents(self,callerID,event,callDataID = None):
        '''
        Gets called on MRML Events to handle them.
        '''
    
        # check if we already process an update event
        # and jump out if yes
        self.GetHelper().guiDebug("ProcessMRMLEvents " + str(callerID) + " " + str(event) + " " + str(callDataID) + " " + str(self._updating))
        if not self._updating:        
            
            if self._associatedMRMLNode:
                # check if the callerID is still a valid Node in the scene
                if slicer.MRMLScene.GetNodeByID(callerID):
                    # check if the callerID equals the associated MRML Node but only if the event is the launch event
                    # we do not want to react to Modify events etc.
                    if event == vtkMRMLAtlasCreatorNode_LaunchComputationEvent and callerID == self._associatedMRMLNode.GetID():
                        # quickly store the MRML configuration to the GUI
                        self.UpdateGUI()
                        
                        # the observed node was launched!
                        self.GetMyLogic().Start(self._associatedMRMLNode)
                        
                        # jump out
                        return True
    
    
            # observe MRMLScene NodeAdded events
            if callerID == "MRMLScene" and event == vtkMRMLScene_NodeAddedEvent and callDataID:
                
                callDataAsMRMLNode = slicer.MRMLScene.GetNodeByID(callDataID)
                
                if isinstance(callDataAsMRMLNode, slicer.vtkMRMLAtlasCreatorNode):
                    self.GetHelper().info("A new vtkMRMLAtlasCreatorNode was added: " + str(callDataID))
                    self.GetHelper().debug(callDataAsMRMLNode)
                    self._associatedMRMLNode = callDataAsMRMLNode
                    self._associatedMRMLNodeTag = self.AddMRMLObserverByNumber(self._associatedMRMLNode,vtkMRMLAtlasCreatorNode_LaunchComputationEvent)
                    
                    # now we should propagate the MRML configuration to the GUI
                    self.UpdateGUI()
                    
                    #jump out
                    return True
                
            # observe MRMLScene Close events
            elif callerID == "MRMLScene" and event == vtkMRMLScene_CloseEvent:
                
                # caught MRMLScene close event
                self._updating = 1
                # reset all GUI values to default
                self.InitializeByDefault()
                # remove the associated MRMLNode
                self._associatedMRMLNode = None
                self._associatedMRMLNodeTag = None
                self._updating = 0
                
                return True                    



    '''=========================================================================================='''
    def BuildGUI(self):
        '''
        Creates the Graphical User Interface (GUI) of the AtlasCreator. Gets called once during loading of the module.
        '''

        self.GetUIPanel().AddPage("AtlasCreator","AtlasCreator","")
        self._atlascreatorPage = self.GetUIPanel().GetPageWidget("AtlasCreator")
        helpText = """**Atlas Creator v0.42**
        
More Information available at <a>http://www.slicer.org/slicerWiki/index.php/Modules:AtlasCreator</a>

** HowTo: Simple Atlas Creation **

1. To perform a simple atlas creation, the Original Images and the Segmentations have to be in two different folders but have the same filename.
For example a possible directory structure would be:
./originals/case1.nrrd
./originals/case2.nrrd
./originals/case3.nrrd
./segmentations/case1.nrrd
./segmentations/case2.nrrd
./segmentations/case3.nrrd

2. Select the directories containing the Original Images and the Segmentations in the Input/Output panel.

3. Select an Output Directory in the Input/Output panel. It makes sense to create a new directory to use for the Output.

4. Hit 'Start!'

5. After some wait (minutes or hours!), the generated atlases and the used template will be loaded into 3D Slicer.

Note: This will perform a Pair Fixed Registration against an automatic chosen template. Several options are available using the Parameters and the Advanced panels. See the link above for further description of all features. 

"""
        aboutText = "This module was developed by Daniel Haehn and Kilian Pohl, University of Pennsylvania. The research was funded by an ARRA supplement to NIH NCRR (P41 RR13218)."
        self._helpAboutFrame = self.BuildHelpAndAboutFrame(self._atlascreatorPage,helpText,aboutText)

        # include the UPenn logo
        logoFrame = self.GetLogoFrame()
        pathToLogo = os.path.normpath(slicer.Application.GetBinDir() + '/../share/Slicer3/Modules/AtlasCreator/UPenn_logo.png')
        logo = slicer.vtkKWIcon()
        
        logoReader = slicer.vtkPNGReader()
        logoReader.SetFileName(pathToLogo)
        logoReader.Update()
        
        logo.SetImage(logoReader.GetOutput())
        
        self._logoLabel = slicer.vtkKWLabel()
        self._logoLabel.SetParent(logoFrame)
        self._logoLabel.Create()
        self._logoLabel.SetImageToIcon(logo)
        slicer.TkCall("pack %s -side top -anchor nw -fill x -padx 2 -pady 2" % self._logoLabel.GetWidgetName())

        self._moduleFrame.SetParent(self._atlascreatorPage)
        self._moduleFrame.Create()
        self._moduleFrame.SetLabelText("AtlasCreator")
        self._moduleFrame.ExpandFrame()
        slicer.TkCall("pack %s -side top -anchor nw -fill x -padx 2 -pady 2 -in %s" % (self._moduleFrame.GetWidgetName(),self._atlascreatorPage.GetWidgetName()))

        # starting I/O Frame
        self._inputOutputFrame.SetParent(self._moduleFrame.GetFrame())
        self._inputOutputFrame.Create()
        self._inputOutputFrame.SetLabelText("Input/Output")
        self._inputOutputFrame.ExpandFrame()
        slicer.TkCall("pack %s -side top -anchor nw -fill x -padx 2 -pady 2" % self._inputOutputFrame.GetWidgetName())

        self._origDirButton.SetParent(self._inputOutputFrame.GetFrame())
        self._origDirButton.Create()
        self._origDirButton.GetWidget().SetText("Click to pick a directory")
        self._origDirButton.SetLabelWidth(20)
        self._origDirButton.SetLabelText("Original images:")
        self._origDirButton.GetWidget().GetLoadSaveDialog().ChooseDirectoryOn()
        self._origDirButton.SetBalloonHelpString("The folder of the original Images.")
        slicer.TkCall("pack %s -side top -anchor nw -fill x -padx 2 -pady 2" % self._origDirButton.GetWidgetName())

        self._origDirButton.GetWidget().GetLoadSaveDialog().Create()

        self._segDirButton.SetParent(self._inputOutputFrame.GetFrame())
        self._segDirButton.Create()
        self._segDirButton.GetWidget().SetText("Click to pick a directory")
        self._segDirButton.SetLabelWidth(20)
        self._segDirButton.SetLabelText("Segmentations:")
        self._segDirButton.GetWidget().GetLoadSaveDialog().ChooseDirectoryOn()
        self._segDirButton.SetBalloonHelpString("The folder of the Segmentations.")
        slicer.TkCall("pack %s -side top -anchor nw -fill x -padx 2 -pady 2" % self._segDirButton.GetWidgetName())

        self._segDirButton.GetWidget().GetLoadSaveDialog().Create()

        self._outDirButton.SetParent(self._inputOutputFrame.GetFrame())
        self._outDirButton.Create()
        self._outDirButton.SetLabelWidth(20)
        self._outDirButton.GetWidget().SetText("Click to pick a directory")
        self._outDirButton.SetLabelText("Output directory:")
        self._outDirButton.GetWidget().GetLoadSaveDialog().ChooseDirectoryOn()
        self._outDirButton.SetBalloonHelpString("The Output Folder. Note: a new folder based on the same name will be created.")
        slicer.TkCall("pack %s -side top -anchor nw -fill x -padx 2 -pady 2" % self._outDirButton.GetWidgetName())


        self._registrationTypeRadios.SetParent(self._inputOutputFrame.GetFrame())
        self._registrationTypeRadios.Create()
        self._registrationTypeRadios.SetLabelWidth(20)
        self._registrationTypeRadios.SetLabelText("Registration Type:")

        self._pairFixedRadio = self._registrationTypeRadios.GetWidget().AddWidget(0)
        self._pairFixedRadio.SetText("Pair Fixed")
        self._pairFixedRadio.SetBalloonHelpString("Pair the cases against a fixed template.")

        self._pairOnlineRadio = self._registrationTypeRadios.GetWidget().AddWidget(1)
        self._pairOnlineRadio.SetText("Pair Online")
        self._pairOnlineRadio.SetBalloonHelpString("Pair the cases against a dynamic template calculated as a mean Image.")
        
        self._groupOnlineRadio = self._registrationTypeRadios.GetWidget().AddWidget(2)
        self._groupOnlineRadio.SetText("Group Online")
        self._groupOnlineRadio.SetBalloonHelpString("Use un-biased registration: each case against each case. UNDER CONSTRUCTION!")
        # enable the groupOnline radio only if congeal was installed
        self._groupOnlineRadio.SetEnabled(self.CheckForCongeal())
        
        slicer.TkCall("pack %s -side top -anchor nw -fill x -padx 2 -pady 2" % self._registrationTypeRadios.GetWidgetName())

        self._registrationTypeRadios.GetWidget().GetWidget(0).SetSelectedState(1) # by default, Pair Fixed
        self._defaultCaseCombo.SetEnabled(1)
        self._meanIterationsSpinBox.SetEnabled(0)        
        # ending I/O Frame
        
        # starting Parameter Frame
        self._parametersFrame.SetParent(self._moduleFrame.GetFrame())
        self._parametersFrame.Create()
        self._parametersFrame.SetLabelText("Parameters")
        self._parametersFrame.CollapseFrame()
        slicer.TkCall("pack %s -side top -anchor nw -fill x -padx 2 -pady 2" % self._parametersFrame.GetWidgetName())

        self._toolkitCombo.SetParent(self._parametersFrame.GetFrame())
        self._toolkitCombo.Create()
        self._toolkitCombo.GetWidget().ReadOnlyOn()
        self._toolkitCombo.SetLabelText("Toolkit:")
        self._toolkitCombo.SetLabelWidth(20)
        self._toolkitCombo.SetBalloonHelpString("The toolkit to use for Registration.")
        slicer.TkCall("pack %s -side top -anchor nw -fill x -padx 2 -pady 2" % self._toolkitCombo.GetWidgetName())

        self._toolkitCombo.GetWidget().AddValue("'BRAINSFit'")
        self._toolkitCombo.GetWidget().AddValue("'CMTK'")
        self._toolkitCombo.GetWidget().SetValue("'BRAINSFit'")
        self._toolkitCombo.GetWidget().SetCommand(self.vtkScriptedModuleGUI, "Invoke UpdateMRMLFromCallback")

        self._deformationTypeRadios.SetParent(self._parametersFrame.GetFrame())
        self._deformationTypeRadios.Create()
        self._deformationTypeRadios.SetLabelWidth(20)
        self._deformationTypeRadios.SetLabelText("Deformation:")

        self._rigidRadio = self._deformationTypeRadios.GetWidget().AddWidget(0)
        self._rigidRadio.SetText("Rigid")
        self._rigidRadio.SetBalloonHelpString("Use rigid deformation.")

        self._affineRadio = self._deformationTypeRadios.GetWidget().AddWidget(1)
        self._affineRadio.SetText("Affine (9 DOF)")
        self._affineRadio.SetBalloonHelpString("Use affine deformation with 9 DOF.")

        self._affine12Radio = self._deformationTypeRadios.GetWidget().AddWidget(2)
        self._affine12Radio.SetText("Affine (12 DOF)")
        self._affine12Radio.SetBalloonHelpString("Use affine deformation with 12 DOF.")

        self._nonRigidRadio = self._deformationTypeRadios.GetWidget().AddWidget(3)
        self._nonRigidRadio.SetText("Non-Rigid")
        self._nonRigidRadio.SetBalloonHelpString("Use non-rigid deformation.")
        slicer.TkCall("pack %s -side top -anchor nw -fill x -padx 2 -pady 2" % self._deformationTypeRadios.GetWidgetName())

        self._affineRadio.SetSelectedState(1)


        self._meanIterationsSpinBox.SetParent(self._parametersFrame.GetFrame())
        self._meanIterationsSpinBox.Create()
        self._meanIterationsSpinBox.GetWidget().SetRange(1,10)
        self._meanIterationsSpinBox.GetWidget().SetIncrement(1)
        self._meanIterationsSpinBox.GetWidget().SetValue(5)
        self._meanIterationsSpinBox.SetLabelWidth(20)
        self._meanIterationsSpinBox.SetLabelText("Alignment Iterations:")        
        self._meanIterationsSpinBox.SetBalloonHelpString("The number of iterations for aligning to the mean of the Original Images.")
        slicer.TkCall("pack %s -side top -anchor ne -fill x -padx 2 -pady 2" % self._meanIterationsSpinBox.GetWidgetName())

        self._defaultCaseCombo.SetParent(self._parametersFrame.GetFrame())
        self._defaultCaseCombo.Create()
        self._defaultCaseCombo.GetWidget().ReadOnlyOn()
        self._defaultCaseCombo.SetLabelText("Default case:")
        self._defaultCaseCombo.SetLabelWidth(20)
        self._defaultCaseCombo.SetBalloonHelpString("The filename of the default case used for registration.")
        slicer.TkCall("pack %s -side top -anchor ne -fill x -padx 2 -pady 2" % self._defaultCaseCombo.GetWidgetName())
        self._defaultCaseCombo.GetWidget().SetCommand(self.vtkScriptedModuleGUI, "Invoke UpdateMRMLFromCallback")
        # ending Parameter Frame


        # starting Advanced Frame
        self._advancedFrame.SetParent(self._moduleFrame.GetFrame())
        self._advancedFrame.Create()
        self._advancedFrame.SetLabelText("Advanced")
        self._advancedFrame.CollapseFrame()
        slicer.TkCall("pack %s -side top -anchor nw -fill x -padx 2 -pady 2" % self._advancedFrame.GetWidgetName())

        # starting Cluster Frame
        self._clusterFrame.SetParent(self._advancedFrame.GetFrame())
        self._clusterFrame.Create()
        self._clusterFrame.SetLabelText("Cluster Configuration")
        self._clusterFrame.CollapseFrame()
        slicer.TkCall("pack %s -side top -anchor nw -fill x -padx 2 -pady 2" % self._clusterFrame.GetWidgetName())

        self._clusterCheckBox.SetParent(self._clusterFrame.GetFrame())
        self._clusterCheckBox.Create()
        self._clusterCheckBox.SetLabelText("Use Cluster:")
        self._clusterCheckBox.GetWidget().SetSelectedState(0)
        self._clusterCheckBox.SetLabelWidth(20)
        self._clusterCheckBox.SetBalloonHelpString("Use the cluster configuration to perform all computations.")
        slicer.TkCall("pack %s -side top -anchor nw -fill x -padx 2 -pady 2" % self._clusterCheckBox.GetWidgetName())

        self._schedulerEntry.SetParent(self._clusterFrame.GetFrame())
        self._schedulerEntry.Create()
        self._schedulerEntry.SetLabelText("Scheduler Command:")
        self._schedulerEntry.SetLabelWidth(20)
        self._schedulerEntry.SetBalloonHelpString("The scheduler command is used to execute computations in a cluster configuration.")
        slicer.TkCall("pack %s -side top -anchor nw -fill x -padx 2 -pady 2" % self._schedulerEntry.GetWidgetName())
        # ending Cluster Frame

        # starting PCA Frame
        self._pcaFrame.SetParent(self._advancedFrame.GetFrame())
        self._pcaFrame.Create()
        self._pcaFrame.SetLabelText("Principal Component Analysis")
        self._pcaFrame.CollapseFrame()
        slicer.TkCall("pack %s -side top -anchor nw -fill x -padx 2 -pady 2" % self._pcaFrame.GetWidgetName())   

        self._pcaCheckBox.SetParent(self._pcaFrame.GetFrame())
        self._pcaCheckBox.Create()
        self._pcaCheckBox.SetLabelText("Activate PCA:")
        self._pcaCheckBox.GetWidget().SetSelectedState(0)
        self._pcaCheckBox.SetBalloonHelpString("Activate PCA Analysis on top of Atlas Creation.")
        self._pcaCheckBox.SetLabelWidth(20)
        slicer.TkCall("pack %s -side top -anchor nw -fill x -padx 2 -pady 2" % self._pcaCheckBox.GetWidgetName())

        self._maxEigenVectorsSpinBox.SetParent(self._pcaFrame.GetFrame())
        self._maxEigenVectorsSpinBox.Create()
        self._maxEigenVectorsSpinBox.GetWidget().SetRange(1,100)
        self._maxEigenVectorsSpinBox.GetWidget().SetIncrement(1)
        self._maxEigenVectorsSpinBox.GetWidget().SetValue(10)
        self._maxEigenVectorsSpinBox.SetLabelText("Max. EigenVectors:")
        self._maxEigenVectorsSpinBox.SetLabelWidth(20)
        self._maxEigenVectorsSpinBox.SetBalloonHelpString("Set the maximal number of EigenVectors to use for PCA.")
        slicer.TkCall("pack %s -side top -anchor ne -fill x -padx 2 -pady 2" % self._maxEigenVectorsSpinBox.GetWidgetName())

        self._pcaCombineCheckBox.SetParent(self._pcaFrame.GetFrame())
        self._pcaCombineCheckBox.Create()
        self._pcaCombineCheckBox.SetLabelText("Combine PCAs:")
        self._pcaCombineCheckBox.GetWidget().SetSelectedState(0)
        self._pcaCombineCheckBox.SetLabelWidth(20)
        self._pcaCombineCheckBox.SetBalloonHelpString("Generate only one PCA output for all labels.")        
        slicer.TkCall("pack %s -side top -anchor nw -fill x -padx 2 -pady 2" % self._pcaCombineCheckBox.GetWidgetName())
        # ending PCA Frame

        # starting ExistingTransforms Frame
        self._transformsFrame.SetParent(self._advancedFrame.GetFrame())
        self._transformsFrame.Create()
        self._transformsFrame.SetLabelText("Use Existing Transforms")
        self._transformsFrame.CollapseFrame()
        slicer.TkCall("pack %s -side top -anchor nw -fill x -padx 2 -pady 2" % self._transformsFrame.GetWidgetName())

        self._useExistingTransformsCheckBox.SetParent(self._transformsFrame.GetFrame())
        self._useExistingTransformsCheckBox.Create()
        self._useExistingTransformsCheckBox.SetLabelText("Skip Registration:")
        self._useExistingTransformsCheckBox.SetLabelWidth(20)
        self._useExistingTransformsCheckBox.GetWidget().SetSelectedState(0)
        self._useExistingTransformsCheckBox.SetBalloonHelpString("Use existing Transforms and skip Registration.")
        slicer.TkCall("pack %s -side top -anchor nw -fill x -padx 2 -pady 2" % self._useExistingTransformsCheckBox.GetWidgetName())

        self._transformsDirButton.SetParent(self._transformsFrame.GetFrame())
        self._transformsDirButton.Create()
        self._transformsDirButton.GetWidget().SetText("Click to pick a directory")
        self._transformsDirButton.SetLabelText("Transforms directory:")
        self._transformsDirButton.SetLabelWidth(20)
        self._transformsDirButton.SetBalloonHelpString("Select the folder containing existing transforms.")
        self._transformsDirButton.GetWidget().GetLoadSaveDialog().ChooseDirectoryOn()
        slicer.TkCall("pack %s -side top -anchor nw -fill x -padx 2 -pady 2" % self._transformsDirButton.GetWidgetName())

        self._transformsTemplateButton.SetParent(self._transformsFrame.GetFrame())
        self._transformsTemplateButton.Create()
        self._transformsTemplateButton.GetWidget().SetText("Click to pick a template")
        self._transformsTemplateButton.SetLabelText("Existing Template:")
        self._transformsTemplateButton.SetLabelWidth(20)
        self._transformsTemplateButton.SetBalloonHelpString("Pick an existing template to use as Resampling space.")
        slicer.TkCall("pack %s -side top -anchor nw -fill x -padx 2 -pady 2" % self._transformsTemplateButton.GetWidgetName())
        # ending ExistingTransforms Frame
        
        # starting Misc. Frame
        self._miscFrame.SetParent(self._advancedFrame.GetFrame())
        self._miscFrame.Create()
        self._miscFrame.SetLabelText("Misc.")
        self._miscFrame.CollapseFrame()
        slicer.TkCall("pack %s -side top -anchor nw -fill x -padx 2 -pady 2" % self._miscFrame.GetWidgetName())        

        self._labelsEntry.SetParent(self._miscFrame.GetFrame())
        self._labelsEntry.Create()
        self._labelsEntry.SetLabelText("Labels:")
        self._labelsEntry.SetLabelWidth(20)
        self._labelsEntry.SetBalloonHelpString("Set the labels to generate Atlases for.")
        slicer.TkCall("pack %s -side top -anchor nw -fill x -padx 2 -pady 2" % self._labelsEntry.GetWidgetName())

        self._saveTransformsCheckBox.SetParent(self._miscFrame.GetFrame())
        self._saveTransformsCheckBox.Create()
        self._saveTransformsCheckBox.SetLabelText("Save Transforms:")
        self._saveTransformsCheckBox.GetWidget().SetSelectedState(1)
        self._saveTransformsCheckBox.SetLabelWidth(20)
        self._saveTransformsCheckBox.SetBalloonHelpString("Save all generated transforms to the Output Directory.")
        slicer.TkCall("pack %s -side top -anchor nw -fill x -padx 2 -pady 2" % self._saveTransformsCheckBox.GetWidgetName())
        
        self._normalizeAtlasCheckBox.SetParent(self._miscFrame.GetFrame())
        self._normalizeAtlasCheckBox.Create()
        self._normalizeAtlasCheckBox.SetLabelText("Normalize Atlases:")
        self._normalizeAtlasCheckBox.GetWidget().SetSelectedState(0)
        self._normalizeAtlasCheckBox.SetLabelWidth(20)
        self._normalizeAtlasCheckBox.SetBalloonHelpString("Normalize all Atlases to a given value. If enabled, the outputCast will be Float.")        
        slicer.TkCall("pack %s -side top -anchor nw -fill x -padx 2 -pady 2" % self._normalizeAtlasCheckBox.GetWidgetName())

        self._normalizeValueEntry.SetParent(self._miscFrame.GetFrame())
        self._normalizeValueEntry.Create()
        self._normalizeValueEntry.SetLabelText("Normalize to:")
        self._normalizeValueEntry.GetWidget().SetValue("1")
        self._normalizeValueEntry.SetLabelWidth(20)
        self._normalizeValueEntry.SetEnabled(0)
        self._normalizeValueEntry.SetBalloonHelpString("Set the value to normalize all Atlases to.")
        slicer.TkCall("pack %s -side top -anchor nw -fill x -padx 2 -pady 2" % self._normalizeValueEntry.GetWidgetName())

        self._outputCastCombo.SetParent(self._miscFrame.GetFrame())
        self._outputCastCombo.Create()
        self._outputCastCombo.GetWidget().ReadOnlyOn()
        self._outputCastCombo.SetLabelText("Output cast for Atlases:")
        self._outputCastCombo.SetLabelWidth(20)
        self._outputCastCombo.SetBalloonHelpString("The output cast for the Atlases.")
        slicer.TkCall("pack %s -side top -anchor nw -fill x -padx 2 -pady 2" % self._outputCastCombo.GetWidgetName())

        self._outputCastCombo.GetWidget().AddValue("'char'")              
        self._outputCastCombo.GetWidget().AddValue("'unsigned char'")
        self._outputCastCombo.GetWidget().AddValue("'double'")
        self._outputCastCombo.GetWidget().AddValue("'float'")
        self._outputCastCombo.GetWidget().AddValue("'int'")
        self._outputCastCombo.GetWidget().AddValue("'unsigned int'")                          
        self._outputCastCombo.GetWidget().AddValue("'long'")
        self._outputCastCombo.GetWidget().AddValue("'unsigned long'") 
        self._outputCastCombo.GetWidget().AddValue("'short'")
        self._outputCastCombo.GetWidget().AddValue("'unsigned short'")
        self._outputCastCombo.GetWidget().SetValue("'short'")

        self._outputCastCombo.GetWidget().SetCommand(self.vtkScriptedModuleGUI, "Invoke UpdateMRMLFromCallback")

        self._deleteAlignedImagesCheckBox.SetParent(self._miscFrame.GetFrame())
        self._deleteAlignedImagesCheckBox.Create()
        self._deleteAlignedImagesCheckBox.SetLabelText("Delete Aligned Images:")
        self._deleteAlignedImagesCheckBox.GetWidget().SetSelectedState(1)
        self._deleteAlignedImagesCheckBox.SetLabelWidth(20)
        self._deleteAlignedImagesCheckBox.SetBalloonHelpString("If selected, all aligned images will be deleted after use.")        
        slicer.TkCall("pack %s -side top -anchor nw -fill x -padx 2 -pady 2" % self._deleteAlignedImagesCheckBox.GetWidgetName())

        self._deleteAlignedSegmentationsCheckBox.SetParent(self._miscFrame.GetFrame())
        self._deleteAlignedSegmentationsCheckBox.Create()
        self._deleteAlignedSegmentationsCheckBox.SetLabelText("Delete Aligned Segs.:")
        self._deleteAlignedSegmentationsCheckBox.GetWidget().SetSelectedState(1)
        self._deleteAlignedSegmentationsCheckBox.SetLabelWidth(20)
        self._deleteAlignedSegmentationsCheckBox.SetBalloonHelpString("If selected, all aligned segmentations will be deleted after use.")        
        slicer.TkCall("pack %s -side top -anchor nw -fill x -padx 2 -pady 2" % self._deleteAlignedSegmentationsCheckBox.GetWidgetName())        

        self._ignoreTemplateSegmentationCheckBox.SetParent(self._miscFrame.GetFrame())
        self._ignoreTemplateSegmentationCheckBox.Create()
        self._ignoreTemplateSegmentationCheckBox.SetLabelText("Ignore Template Seg.:")
        self._ignoreTemplateSegmentationCheckBox.GetWidget().SetSelectedState(0)
        self._ignoreTemplateSegmentationCheckBox.SetLabelWidth(20)
        self._ignoreTemplateSegmentationCheckBox.SetBalloonHelpString("If selected, the actual template segmentation will not be included in the atlas generation or other computed images.")        
        slicer.TkCall("pack %s -side top -anchor nw -fill x -padx 2 -pady 2" % self._ignoreTemplateSegmentationCheckBox.GetWidgetName())        

        self._numberOfThreadsEntry.SetParent(self._miscFrame.GetFrame())
        self._numberOfThreadsEntry.Create()
        self._numberOfThreadsEntry.SetLabelText("Number of Threads:")
        self._numberOfThreadsEntry.SetLabelWidth(20)
        self._numberOfThreadsEntry.SetBalloonHelpString("The number of threads used for Registration. If -1, the maximal number of threads will be used.")
        slicer.TkCall("pack %s -side top -anchor nw -fill x -padx 2 -pady 2" % self._numberOfThreadsEntry.GetWidgetName())
        
        self._debugCheckBox.SetParent(self._miscFrame.GetFrame())
        self._debugCheckBox.Create()
        self._debugCheckBox.SetLabelText("Debug Output:")
        self._debugCheckBox.GetWidget().SetSelectedState(0)
        self._debugCheckBox.SetLabelWidth(20)
        self._debugCheckBox.SetBalloonHelpString("If selected, print debug output to the console.")
        slicer.TkCall("pack %s -side top -anchor nw -fill x -padx 2 -pady 2" % self._debugCheckBox.GetWidgetName())
        
        self._dryrunCheckBox.SetParent(self._miscFrame.GetFrame())
        self._dryrunCheckBox.Create()
        self._dryrunCheckBox.SetLabelText("Dry-Run (Simulation):")
        self._dryrunCheckBox.GetWidget().SetSelectedState(0)
        self._dryrunCheckBox.SetLabelWidth(20)
        self._dryrunCheckBox.SetBalloonHelpString("If selected, no computation is really executed - only commands are printed to the console.")        
        slicer.TkCall("pack %s -side top -anchor nw -fill x -padx 2 -pady 2" % self._dryrunCheckBox.GetWidgetName())
        # ending Misc. Frame
        
        # ending Advanced Frame


        self._launchButton.SetParent(self._moduleFrame.GetFrame())
        self._launchButton.Create()
        self._launchButton.SetEnabled(1)
        self._launchButton.SetText("Start!")
        self._launchButton.SetBalloonHelpString("Click to generate the atlases! After computation, they will be loaded into 3D Slicer. If PCA was selected, the output will be written to disk.")
        slicer.TkCall("pack %s -side top -anchor e -padx 2 -pady 2" % self._launchButton.GetWidgetName())



    '''=========================================================================================='''
    def TearDownGUI(self):
        '''
        Deletes the GUI
        '''
        #
        # The shutdown sequence is
        # 1. RemoveGUIObservers
        # 2. TearDownGUI
        # 3. RemoveMRMLNodeObervers 
        #
        
        if self.GetUIPanel().GetUserInterfaceManager():
             self.GetUIPanel().RemovePage("AtlasCreator")
        
        

    '''=========================================================================================='''
    def GetHelper(self):
        '''
        Returns the current Helper class.
        '''
        return self._helper



    '''=========================================================================================='''
    def GetMyLogic(self):
        '''
        Returns the current logic class.
        '''
        return self._logic
    


    '''=========================================================================================='''
    def InitializeByDefault(self):
        '''
        Resets the GUI to default values.
        
        Returns nothing.
        '''

        self.GetHelper().guiDebug("InitializeByDefault" + str(self._updating))
        if not self._updating:
    
            self._updating = 1
            
            # I/O PANEL
            self._origDirButton.GetWidget().SetInitialFileName("")
            self._origDirButton.GetWidget().SetText("Click to pick a directory")
            
            self._segDirButton.GetWidget().SetInitialFileName("")
            self._segDirButton.GetWidget().SetText("Click to pick a directory")
            
            self._outDirButton.GetWidget().SetInitialFileName("")
            self._outDirButton.GetWidget().SetText("Click to pick a directory")       
            
            # use fixed by default
            self._pairFixedRadio.SetSelectedState(1)            
            
            
            # PARAMETERS
        
            # by default, use BRAINSFit
            self._toolkitCombo.GetWidget().SetValue("'BRAINSFit'")
            # by default use affine registration
            self._rigidRadio.SetSelectedState(0)
            self._affineRadio.SetSelectedState(1)
            self._affine12Radio.SetSelectedState(0)
            self._nonRigidRadio.SetSelectedState(0) 
            self._meanIterationsSpinBox.GetWidget().SetValue(5)  
            self._defaultCaseCombo.GetWidget().SetValue("")
            
            
            # ADVANCED
            
            # Cluster config
            self._clusterCheckBox.GetWidget().SetSelectedState(0)
            self._schedulerEntry.GetWidget().SetValue("")
            
            # PCA
            self._pcaCheckBox.GetWidget().SetSelectedState(0)
            self._maxEigenVectorsSpinBox.GetWidget().SetValue(10)
            self._pcaCombineCheckBox.GetWidget().SetSelectedState(0)
            
            # Use Existing
            self._useExistingTransformsCheckBox.GetWidget().SetSelectedState(0)
            self._transformsDirButton.GetWidget().SetInitialFileName("")
            self._transformsDirButton.GetWidget().SetText("Click to pick a directory")                
            self._transformsTemplateButton.GetWidget().SetInitialFileName("")
            self._transformsTemplateButton.GetWidget().SetText("Click to pick a template")
                            
            # Misc. panel
            self._labelsEntry.GetWidget().SetValue("")
            self._saveTransformsCheckBox.GetWidget().SetSelectedState(1)
            self._normalizeAtlasCheckBox.GetWidget().SetSelectedState(0)
            # by default, normalize to 1
            self._normalizeValueEntry.GetWidget().SetValue(1)
            # by default, Short
            self._outputCastCombo.GetWidget().SetValue("'short'")
            self._deleteAlignedImagesCheckBox.GetWidget().SetSelectedState(1)
            self._deleteAlignedSegmentationsCheckBox.GetWidget().SetSelectedState(1)
            self._ignoreTemplateSegmentationCheckBox.GetWidget().SetSelectedState(0)
            self._numberOfThreadsEntry.GetWidget().SetValue(-1)
            self._debugCheckBox.GetWidget().SetSelectedState(0)
            self._dryrunCheckBox.GetWidget().SetSelectedState(0)
            
            self._updating = 0        
            
            # trigger pair online handler
            self.ToggleMeanAndDefaultCase1()            
            # trigger normalize handler
            self.ToggleNormalize()                   
            
     
        
        

        
        
        
    
    
