from __main__ import qt, ctk
import PythonQt

from EMSegmentStep import *
from Helper import *

class EMSegmentEditNodeBasedParametersStep( EMSegmentStep ) :

  def __init__( self, stepid ):
    self.initialize( stepid )
    self.setName( '8. Edit Node-based Parameters' )
    self.setDescription( 'Specify node-based segmentation parameters.' )

    self.__parent = super( EMSegmentEditNodeBasedParametersStep, self )

  def createUserInterface( self ):
    '''
    '''
    self.__layout = self.__parent.createUserInterface()

    # the anatomical tree
    anatomicalTreeGroupBox = qt.QGroupBox()
    anatomicalTreeGroupBox.setTitle( 'Anatomical Tree' )
    self.__layout.addWidget( anatomicalTreeGroupBox )

    anatomicalTreeGroupBoxLayout = qt.QFormLayout( anatomicalTreeGroupBox )

    self.__anatomicalTree = PythonQt.qSlicerEMSegmentModuleWidgets.qSlicerEMSegmentAnatomicalTreeWidget()
    self.__anatomicalTree.structureNameEditable = False
    self.__anatomicalTree.labelColumnVisible = False
    self.__anatomicalTree.probabilityMapColumnVisible = False
    self.__anatomicalTree.classWeightColumnVisible = True
    self.__anatomicalTree.updateClassWeightColumnVisible = True
    self.__anatomicalTree.atlasWeightColumnVisible = True
    self.__anatomicalTree.alphaColumnVisible = False
    self.__anatomicalTree.displayAlphaCheckBoxVisible = False
    anatomicalTreeGroupBoxLayout.addWidget( self.__anatomicalTree )

    tabWidget = qt.QTabWidget()
    self.__layout.addWidget( tabWidget )

    #
    # intensityDistributionPage
    #
    intensityDistributionPage = qt.QWidget()
    intensityDistributionPageLayout = qt.QFormLayout( intensityDistributionPage )

    classLabel = qt.QLabel( "XX" )
    intensityDistributionPageLayout.addRow( "Class:", classLabel )

    specificationComboBox = qt.QComboBox()
    specificationComboBox.addItems( Helper.GetSpecificationTypes() )
    intensityDistributionPageLayout.addRow( "Specification:", specificationComboBox )

    meanMatrixWidget = ctk.ctkMatrixWidget()
    meanMatrixWidget.columnCount = 2
    meanMatrixWidget.rowCount = 1
    intensityDistributionPageLayout.addRow( "Mean:", meanMatrixWidget )

    logCovarianceMatrixWidget = ctk.ctkMatrixWidget()
    logCovarianceMatrixWidget.columnCount = 2
    logCovarianceMatrixWidget.rowCount = 2
    intensityDistributionPageLayout.addRow( "Log Covariance:", logCovarianceMatrixWidget )

    resetDistributionButton = qt.QPushButton()
    resetDistributionButton.text = "Reset Distribution"
    intensityDistributionPageLayout.addRow( resetDistributionButton )

    #
    # manualSamplingPage
    #
    manualSamplingPage = qt.QWidget()
    manualSamplingPageLayout = qt.QFormLayout( manualSamplingPage )

    tabWidget.addTab( intensityDistributionPage, "Intensity Distribution" )
    tabWidget.addTab( manualSamplingPage, "Manual Sampling" )



  def onEntry( self, comingFrom, transitionType ):
    '''
    '''
    self.__parent.onEntry( comingFrom, transitionType )

    self.__anatomicalTree.setMRMLManager( self.mrmlManager() )
    self.__anatomicalTree.updateWidgetFromMRML()


  def validate( self, desiredBranchId ):
    '''
    '''
    self.__parent.validate( desiredBranchId )

    self.__parent.validationSucceeded( desiredBranchId )
