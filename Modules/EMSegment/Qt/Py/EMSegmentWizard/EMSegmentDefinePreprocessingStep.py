from __main__ import qt, ctk, tcl
import PythonQt

from EMSegmentStep import *
from Helper import *
from EMSegmentDynamicFrame import *

class EMSegmentDefinePreprocessingStep( EMSegmentStep ) :

  def __init__( self, stepid ):
    self.initialize( stepid )
    self.setName( '6. Define Input Channels' )
    self.setDescription( 'Answer questions for preprocessing of input images' )

    self.__parent = super( EMSegmentDefinePreprocessingStep, self )

    self.__stepid = stepid
    self.__dynamicFrame = None

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


  def onEntry( self, comingFrom, transitionType ):
    '''
    '''
    self.__parent.onEntry( comingFrom, transitionType )

    # we are always in advanced mode, so let's fill the dynamic frame
    self.logic().SourceTaskFiles()

    # clear the dynamic panel
    self.dynamicFrame().clearElements()

    logicTclName = self.logic().GetSlicerCommonInterface().GetTclNameFromPointer( self.logic() )

    tcl( '::EMSegmenterPreProcessingTcl::ShowUserInterface ' + str( logicTclName ) )


  def validate( self, desiredBranchId ):
    '''
    '''
    self.__parent.validate( desiredBranchId )

    # TODO



    self.__parent.validationSucceeded( desiredBranchId )


