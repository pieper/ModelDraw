from __main__ import qt, ctk
import PythonQt

from EMSegmentStep import *
from Helper import *

class EMSegmentSpecifyIntensityDistributionStep( EMSegmentStep ) :

  def __init__( self, stepid ):
    self.initialize( stepid )
    self.setName( '7. Specify Intensity Distributions' )
    self.setDescription( 'Define intensity distribution for each anatomical structure.' )

    self.__parent = super( EMSegmentSpecifyIntensityDistributionStep, self )
    self.__layout = None
    self.__anatomicalTree = None


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
    self.__anatomicalTree.classWeightColumnVisible = False
    self.__anatomicalTree.updateClassWeightColumnVisible = False
    self.__anatomicalTree.atlasWeightColumnVisible = False
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

    if self.__anatomicalTree:
      self.__anatomicalTree.setMRMLManager( self.mrmlManager() )
      self.__anatomicalTree.updateWidgetFromMRML()


  def validate( self, desiredBranchId ):
    '''
    '''
    self.__parent.validate( desiredBranchId )

    self.__parent.validationSucceeded( desiredBranchId )
