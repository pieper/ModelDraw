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
    self.__classLabel = None
    self.__specificationComboBox = None
    self.__meanMatrixWidget = None
    self.__logCovarianceMatrixWidget = None
    self.__resetDistributionButton = None
    self.__tabWidget = None

    self.__updating = 0


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
    self.__anatomicalTree.connect( 'currentTreeNodeChanged(vtkMRMLNode*)', self.onTreeSelectionChanged )
    anatomicalTreeGroupBoxLayout.addWidget( self.__anatomicalTree )

    self.__tabWidget = qt.QTabWidget()
    self.__layout.addWidget( self.__tabWidget )

    #
    # intensityDistributionPage
    #
    intensityDistributionPage = qt.QWidget()
    intensityDistributionPageLayout = qt.QFormLayout( intensityDistributionPage )

    self.__classLabel = qt.QLabel( "XX" )
    intensityDistributionPageLayout.addRow( "Class:", self.__classLabel )

    self.__specificationComboBox = qt.QComboBox()
    self.__specificationComboBox.addItems( Helper.GetSpecificationTypes() )
    self.__specificationComboBox.model().item( 2 ).setSelectable( False )
    self.__specificationComboBox.model().item( 2 ).setEnabled( False )
    intensityDistributionPageLayout.addRow( "Specification:", self.__specificationComboBox )
    self.__specificationComboBox.connect( 'currentIndexChanged(int)', self.propagateToMRML )

    self.__meanMatrixWidget = ctk.ctkMatrixWidget()
    self.__meanMatrixWidget.columnCount = 1
    self.__meanMatrixWidget.rowCount = 1
    intensityDistributionPageLayout.addRow( "Mean:", self.__meanMatrixWidget )

    self.__logCovarianceMatrixWidget = ctk.ctkMatrixWidget()
    self.__logCovarianceMatrixWidget.columnCount = 2
    self.__logCovarianceMatrixWidget.rowCount = 2
    intensityDistributionPageLayout.addRow( "Log Covariance:", self.__logCovarianceMatrixWidget )

    self.__resetDistributionButton = qt.QPushButton()
    self.__resetDistributionButton.text = "Reset Distribution"
    intensityDistributionPageLayout.addRow( self.__resetDistributionButton )

    #
    # manualSamplingPage
    #
    manualSamplingPage = qt.QWidget()
    manualSamplingPageLayout = qt.QFormLayout( manualSamplingPage )

    self.__tabWidget.addTab( intensityDistributionPage, "Intensity Distribution" )
    self.__tabWidget.addTab( manualSamplingPage, "Manual Sampling" )



  def onEntry( self, comingFrom, transitionType ):
    '''
    '''
    self.__parent.onEntry( comingFrom, transitionType )

    if self.__anatomicalTree:
      self.__anatomicalTree.setMRMLManager( self.mrmlManager() )
      self.__anatomicalTree.updateWidgetFromMRML()

    # reset the panel
    self.loadFromMRML()

    manualSampling = 1
    if manualSampling:

      slicer.sliceWidgetRed_interactorStyle.AddObserver( vtk.vtkCommand.LeftButtonReleaseEvent, self.onClickInRedSliceView )
      slicer.sliceWidgetYellow_interactorStyle.AddObserver( vtk.vtkCommand.LeftButtonReleaseEvent, self.onClickInYellowSliceView )
      slicer.sliceWidgetGreen_interactorStyle.AddObserver( vtk.vtkCommand.LeftButtonReleaseEvent, self.onClickInGreenSliceView )



  def validate( self, desiredBranchId ):
    '''
    '''
    self.__parent.validate( desiredBranchId )

    self.__parent.validationSucceeded( desiredBranchId )

  def onTreeSelectionChanged( self ):
    '''
    '''

    mrmlNode = self.__anatomicalTree.currentNode()

    if mrmlNode:
      vtkId = self.mrmlManager().MapMRMLNodeIDToVTKNodeID( mrmlNode.GetID() )
      # check if we have a leaf
      isLeaf = self.mrmlManager().GetTreeNodeIsLeaf( vtkId )

      if isLeaf:
        self.loadFromMRML( mrmlNode )
      else:
        self.loadFromMRML( None )


  def propagateToMRML( self ):
    '''
    '''
    if not self.__updating:

      self.__updating = 1

      # save the values to MRML
      mrmlNode = self.__anatomicalTree.currentNode()

      if mrmlNode:
        vtkId = self.mrmlManager().MapMRMLNodeIDToVTKNodeID( mrmlNode.GetID() )

        self.mrmlManager().SetTreeNodeDistributionSpecificationMethod( vtkId, self.__specificationComboBox.currentIndex )

        # manual sampling, en-/disable the manual sampling tab
        self.__tabWidget.setTabEnabled( 1, ( self.__specificationComboBox.currentIndex == 1 ) )


      self.__updating = 0


  def loadFromMRML( self, mrmlNode=None ):
    '''
    '''
    if not self.__updating:

      self.__updating = 1

      self.resetPanel()

      if mrmlNode:
        # adjust the panel to the given mrmlNode
        vtkId = self.mrmlManager().MapMRMLNodeIDToVTKNodeID( mrmlNode.GetID() )

        # number of volumes
        numberOfVolumes = self.mrmlManager().GetTargetNumberOfSelectedVolumes()

        # re-enable the widgets
        self.resetPanel( True )

        self.__classLabel.setText( mrmlNode.GetName() )
        self.__specificationComboBox.setCurrentIndex( self.mrmlManager().GetTreeNodeDistributionSpecificationMethod( vtkId ) )

        if self.__specificationComboBox.currentIndex == 1:
          # manual sampling, enable the manual sampling tab
          self.__tabWidget.setTabEnabled( 1, True )

        self.__meanMatrixWidget.columnCount = numberOfVolumes
        self.__meanMatrixWidget.rowCount = 1
        # only editable, if specification is Manual
        self.__meanMatrixWidget.editable = ( self.__specificationComboBox.currentIndex == 0 )
        # fill values
        for c in range( numberOfVolumes ):
          #self.__meanMatrixWidget.setValue( 0, c, self.mrmlManager().GetTreeNodeDistributionMeanWithCorrection( vtkId, c ) )
          pass

        self.__logCovarianceMatrixWidget.columnCount = numberOfVolumes
        self.__logCovarianceMatrixWidget.rowCount = numberOfVolumes
        # only editable, if specification is Manual
        self.__logCovarianceMatrixWidget.editable = ( self.__specificationComboBox.currentIndex == 0 )
        # fill values
        for r in range( numberOfVolumes ):
          for c in range( numberOfVolumes ):
            #self.__logCovarianceMatrixWidget.setValue( r, c, self.mrmlManager().GetTreeNodeDistributionLogCovarianceWithCorrection( vtkId, r, c ) )
            pass

      self.__updating = 0


  def resetPanel( self, enabled=False ):
    '''
    '''
    if self.__classLabel:
      self.__classLabel.setText( '' )

    if self.__specificationComboBox:
      self.__specificationComboBox.setEnabled( enabled )
      self.__specificationComboBox.setCurrentIndex( 0 )

    if self.__meanMatrixWidget:
      self.__meanMatrixWidget.setEnabled( enabled )
      self.__meanMatrixWidget.columnCount = 1
      self.__meanMatrixWidget.rowCount = 1
      #self.__meanMatrixWidget.setValue( 0, 0, 0 )

    if self.__logCovarianceMatrixWidget:
      self.__logCovarianceMatrixWidget.setEnabled( enabled )
      self.__logCovarianceMatrixWidget.columnCount = 1
      self.__logCovarianceMatrixWidget.rowCount = 1
      #self.__logCovarianceMatrixWidget.setValue( 0, 0, 0 )

    if self.__resetDistributionButton:
      self.__resetDistributionButton.setEnabled( enabled )

    if self.__tabWidget:
      self.__tabWidget.setEnabled( enabled )
      self.__tabWidget.setTabEnabled( 1, False )


  def onClickInRedSliceView( self, interactorStyleTrackballCamera, event ):
    '''
    '''
    rasPos = Helper.onClickInSliceView( slicer.sliceWidgetRed_interactorStyle, slicer.sliceWidgetRed_sliceLogic )

    Helper.Debug( ' RED RAS: ' + str( rasPos ) )

  def onClickInYellowSliceView( self, interactorStyleTrackballCamera, event ):
    '''
    '''
    rasPos = Helper.onClickInSliceView( slicer.sliceWidgetYellow_interactorStyle, slicer.sliceWidgetYellow_sliceLogic )

    Helper.Debug( ' YELLOW RAS: ' + str( rasPos ) )

  def onClickInGreenSliceView( self, interactorStyleTrackballCamera, event ):
    '''
    '''
    rasPos = Helper.onClickInSliceView( slicer.sliceWidgetGreen_interactorStyle, slicer.sliceWidgetGreen_sliceLogic )

    Helper.Debug( ' GREEN RAS: ' + str( rasPos ) )
