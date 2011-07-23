from __main__ import qt, ctk, tcl
import PythonQt

from EMSegmentStep import *
from Helper import *
from EMSegmentDynamicFrame import *

class EMSegmentDefineInputChannelsStep( EMSegmentStep ) :

  def __init__( self, stepid ):
    self.initialize( stepid )
    self.setName( '2. Define Input Channels' )
    self.setDescription( 'Name the input channels and choose the set of scans for segmentation.' )

    self.__parent = super( EMSegmentDefineInputChannelsStep, self )

    self.__stepid = stepid
    self.__dynamicFrame = None
    self.__updating = 0

  def isSimpleMode( self ):
    '''
    '''
    if self.__stepid == ( str( Helper.GetNthStepId( 2 ) ) + str( 'Simple' ) ):
      # we are in simple mode
      return True
    else:
      return False

  def dynamicFrame( self ):
    '''
    '''
    if not self.__dynamicFrame:

      Helper.Debug( "No dynamic frame yet, creating one.." )
      self.__dynamicFrame = EMSegmentDynamicFrame()

    return self.__dynamicFrame

  def createUserInterface( self ):
    '''
    '''
    self.__layout = self.__parent.createUserInterface()

    # the input channels
    inputChannelGroupBox = qt.QGroupBox()
    inputChannelGroupBox.setTitle( 'Input Channels' )
    self.__layout.addWidget( inputChannelGroupBox )

    inputChannelGroupBoxLayout = qt.QFormLayout( inputChannelGroupBox )

    self.__inputChannelList = PythonQt.qSlicerEMSegmentModuleWidgets.qSlicerEMSegmentInputChannelListWidget()
    self.__inputChannelList.setMRMLManager( self.mrmlManager() )
    inputChannelGroupBoxLayout.addWidget( self.__inputChannelList )

    # add empty row
    self.__layout.addRow( "", qt.QWidget() )

    # registration settings
    input2inputChannelRegistration = qt.QGroupBox()
    input2inputChannelRegistration.setTitle( 'Input-to-Input Channel Registration' )
    self.__layout.addWidget( input2inputChannelRegistration )

    input2inputChannelRegistrationLayout = qt.QFormLayout( input2inputChannelRegistration )

    self.__alignInputScansCheckBox = qt.QCheckBox()
    input2inputChannelRegistrationLayout.addRow( 'Align input scans:', self.__alignInputScansCheckBox )
    self.__alignInputScansCheckBox.connect( "stateChanged(int)", self.propagateToMRML )

    # add empty row
    self.__layout.addRow( "", qt.QWidget() )

    if self.isSimpleMode():
      #
      # dynamic frame
      #
      dynamicFrame = qt.QGroupBox()
      dynamicFrame.setTitle( 'Check List' )
      self.__layout.addWidget( dynamicFrame )
      dynamicFrameLayout = qt.QVBoxLayout( dynamicFrame )

      # .. now pass the layout to the dynamicFrame
      self.dynamicFrame().setLayout( dynamicFrameLayout )
      #
      # end of dynamic frame
      #


  def propagateToMRML( self ):
    '''
    '''
    if not self.__updating:

      self.__updating = 1

      self.__inputChannelList.updateMRMLFromWidget()

      self.mrmlManager().SetEnableTargetToTargetRegistration( int( self.__alignInputScansCheckBox.isChecked() ) )

      self.__updating = 0



  def loadFromMRML( self ):
    '''
    '''
    if not self.__updating:

      self.__updating = 1

      self.__inputChannelList.updateWidgetFromMRML()

      if self.__inputChannelList.inputChannelCount() == 0:
        self.__inputChannelList.addInputChannel()

      self.__alignInputScansCheckBox.setChecked( self.mrmlManager().GetEnableTargetToTargetRegistration() )

      self.__updating = 0



  def onEntry( self, comingFrom, transitionType ):
    '''
    '''
    self.__parent.onEntry( comingFrom, transitionType )

    if self.isSimpleMode():
      self.logic().SourceTaskFiles()

      # clear the dynamic panel
      self.dynamicFrame().clearElements()

      logicTclName = self.logic().GetSlicerCommonInterface().GetTclNameFromPointer( self.logic() )

      tcl( '::EMSegmenterSimpleTcl::ShowCheckList ' + str( logicTclName ) )

    self.loadFromMRML()



  def onExit( self, goingTo, transitionType ):
    '''
    '''
    self.__parent.onExit( goingTo, transitionType )

    self.propagateToMRML()


  def validate( self, desiredBranchId ):
    '''
    '''
    self.__parent.validate( desiredBranchId )

    # we need at least one input channel
    if self.__inputChannelList.inputChannelCount() == 0:
      self.__parent.validationFailed( desiredBranchId, 'Input Channel Error', 'Please add at least one input channel!' )

    # we need an assigned volume for each channel
    for c in range( self.__inputChannelList.inputChannelCount() ):
      if not self.__inputChannelList.inputChannelVolume( c ):
        self.__parent.validationFailed( desiredBranchId, 'Input Channel Error', 'Please assign a volume to each input channel!' )

    # TODO number of input channels changed
    # this->GetNumberOfInputChannels() != mrmlManager->GetGlobalParametersNode()->GetNumberOfTargetInputChannels()

    # TODO check if all channels have different volumes assigned

    # TODO check on bad names for the channels



    self.__parent.validationSucceeded( desiredBranchId )


