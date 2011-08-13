from __main__ import qt, ctk
import PythonQt

from EMSegmentStep import *
from Helper import *

class EMSegmentStartSegmentationStep( EMSegmentStep ) :

  def __init__( self, stepid ):
    self.initialize( stepid )
    self.setName( '10. Segment' )
    self.setDescription( '' )
    self.icon = qt.QIcon( ":/Icons/EMSegment.png" )

    self.__parent = super( EMSegmentStartSegmentationStep, self )

  def createUserInterface( self ):
    '''
    '''
    self.__layout = self.__parent.createUserInterface()


  def onEntry( self, comingFrom, transitionType ):
    '''
    '''
    self.__parent.onEntry( comingFrom, transitionType )

    targetNode = self.mrmlManager().GetWorkingDataNode().GetInputTargetNode()
    alignedAtlasNode = self.mrmlManager().GetWorkingDataNode().GetAlignedAtlasNode()

    if not self.mrmlManager().DoTargetAndAtlasDataTypesMatch( targetNode, alignedAtlasNode ):
      # error
      messageBox = qt.QMessageBox.warning( self, "Error", "Scalar type mismatch for input images; all image scalar types must be the same (including input channels and aligned/resampled/casted atlas images)." )
      return

    workingDirectory = self.mrmlManager().GetSaveWorkingDirectory()
    if self.mrmlManager().GetSaveIntermediateResults():
      # if we want to save the results directly, we have to check if the workingDirectory is setup correctly
      if not workingDirectory:
        # workingDirectory was not configured
        messageBox = qt.QMessageBox.warning( self, "Error", "Saving intermediate results is turned on but no intermediate directory is selected." )
        return
      elif not os.path.exists( workingDirectory ):
        # the workingDirectory path does not exist
        messageBox = qt.QMessageBox.warning( self, "Error", "Saving intermediate results is turned on but the intermediate directory " + workingDirectory + " does not exist!" )
        return

    if self.mrmlManager().GetAtlasNumberOfTrainingSamples() <= 0:
      # compute the number of training samples
      self.mrmlManager().ComputeAtlasNumberOfTrainingSamples()

    # create output volumes
    self.logic().CreateOutputVolumeNode()

    # start the segmentation
    returnValue = self.logic().StartSegmentationWithoutPreprocessing( slicer.app.appLogic() )

    if returnValue != 0:
      # something went wrong!
      # error message!
      messageBox = qt.QMessageBox.warning( self, "Error", "Segmentation did not execute correctly!" )
      return

    outputNode = self.mrmlManager().GetOutputVolumeNode()

    if not outputNode:
      messageBox = qt.QMessageBox.warning( self, "Error", "Output node was not found!" )
      return

    selectionNode = slicer.app.mrmlApplicationLogic().GetSelectionNode()
    #selectionNode.SetReferenceActiveVolumeID(bgVolumeID)
    selectionNode.SetReferenceSecondaryVolumeID( outputNode.GetID() )
    slicer.app.mrmlApplicationLogic().PropagateVolumeSelection()

    # show foreground volume
    numberOfCompositeNodes = slicer.mrmlScene.GetNumberOfNodesByClass( 'vtkMRMLSliceCompositeNode' )
    for n in xrange( numberOfCompositeNodes ):
      compositeNode = slicer.mrmlScene.GetNthNodeByClass( n, 'vtkMRMLSliceCompositeNode' )
      if compositeNode:
        # display a fade of foreground/background
        compositeNode.SetForegroundOpacity( 0.5 )

    Helper.Debug( "All Done!" )

  def validate( self, desiredBranchId ):
    '''
    '''
    self.__parent.validate( desiredBranchId )

    self.__parent.validationSucceeded( desiredBranchId )
