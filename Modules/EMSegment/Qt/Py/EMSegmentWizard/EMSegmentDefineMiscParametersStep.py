from __main__ import qt, ctk
import PythonQt

from EMSegmentStep import *
from Helper import *

class EMSegmentDefineMiscParametersStep( EMSegmentStep ) :

  def __init__( self, stepid ):
    self.initialize( stepid )
    self.setName( '9. Define Miscellaneous Parameters' )
    self.setDescription( 'Define miscellaneous parameters for performing segmentation' )

    self.__parent = super( EMSegmentDefineMiscParametersStep, self )
    self.__updating = 0;

  def createUserInterface( self ):
    '''
    '''
    self.__layout = self.__parent.createUserInterface()

    # deactivate next button since it is the last step
    self.buttonBoxHints = self.NextButtonHidden

    # the ROI parameters
    voiGroupBox = qt.QGroupBox()
    voiGroupBox.setTitle( 'Define VOI' )
    self.__layout.addWidget( voiGroupBox )

    voiGroupBoxLayout = qt.QFormLayout( voiGroupBox )

    roiWidget = PythonQt.qSlicerAnnotationsModuleWidgets.qMRMLAnnotationROIWidget()
    voiGroupBoxLayout.addWidget( roiWidget )

    #
    # save groupbox
    #
    saveGroupBox = qt.QGroupBox()
    saveGroupBox.setTitle( 'Save' )
    self.__layout.addWidget( saveGroupBox )

    saveGroupBoxLayout = qt.QFormLayout ( saveGroupBox )

    saveButton = qt.QPushButton( 'Create' )
    saveGroupBoxLayout.addRow( "Create Template File:", saveButton )

    saveIntermediateResultsCheckBox = qt.QCheckBox()
    saveGroupBoxLayout.addRow( "Save Intermediate Results:", saveIntermediateResultsCheckBox )

    selectDirectoryButton = qt.QPushButton( 'Select' )
    saveGroupBoxLayout.addRow( "Select Intermediate Directory:", selectDirectoryButton )

    #
    # post processing groupbox
    #
    postProcessingGroupBox = qt.QGroupBox()
    postProcessingGroupBox.setTitle( 'Postprocessing' )
    self.__layout.addWidget( postProcessingGroupBox )

    postProcessingGroupBoxLayout = qt.QFormLayout( postProcessingGroupBox )

    subparcellationCheckBox = qt.QCheckBox()
    postProcessingGroupBoxLayout.addRow( "Subparcellation enabled:", subparcellationCheckBox )

    minimumIslandSizeSpinBox = qt.QSpinBox()
    minimumIslandSizeSpinBox.minimum = 0
    minimumIslandSizeSpinBox.maximum = 200
    minimumIslandSizeSpinBox.singleStep = 1
    postProcessingGroupBoxLayout.addRow( "Minimum island size:", minimumIslandSizeSpinBox )

    twoDIslandNeighborhoodCheckBox = qt.QCheckBox()
    postProcessingGroupBoxLayout.addRow( "2D Island Neighborhood:", twoDIslandNeighborhoodCheckBox )

    #
    # Misc. groupbox
    #
    miscGroupBox = qt.QGroupBox()
    miscGroupBox.setTitle( 'Misc.' )
    self.__layout.addWidget( miscGroupBox )

    miscGroupBoxLayout = qt.QFormLayout( miscGroupBox )

    multithreadingCheckBox = qt.QCheckBox()
    miscGroupBoxLayout.addRow( "Multi-threading enabled:", multithreadingCheckBox )


  def onEntry( self, comingFrom, transitionType ):
    '''
    '''
    self.__parent.onEntry( comingFrom, transitionType )




  def validate( self, desiredBranchId ):
    '''
    '''
    self.__parent.validate( desiredBranchId )

    self.__parent.validationSucceeded( desiredBranchId )



  def propagateToMRML( self ):
    '''
    '''
    if not self.__updating:

      self.__updating = 1

      self.__updating = 0


  def loadFromMRML( self ):
    '''
    '''
    if not self.__updating:

      self.__updating = 1

      self.__updating = 0
