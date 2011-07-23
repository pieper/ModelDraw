from __main__ import qt, ctk
from Helper import *

class EMSegmentDynamicFrame:

  def __init__( self ):
    '''
    '''
    self.__layout = None
    self.__checkButtonElementIDs = dict()
    self.__textLabelElementIDs = dict()

  def setLayout( self, layout ):
    '''
    '''
    self.__layout = layout

  def layout( self ):
    '''
    '''
    if not self.__layout:
      Helper.Error( "No layout for dynamic frame!" )

    return self.__layout


  def getElementByID( self, id ):
    '''
    '''
    if not self.__elementIDs.has_key( id ):
      Helper.Error( "Could not find element!" )
    else:
      return self.__elementIDs[id]

  def saveCheckButtonElement( self, id, element ):
    '''
    '''
    if ( not id >= 0 )  or not element:
      Helper.Error( "Wrong call: id and element have to be set" )
      return False

    if self.__checkButtonElementIDs.has_key( id ):
      Helper.Warning( "We are overwriting an element.." )

    self.__checkButtonElementIDs[id] = element

  def saveTextLabelElement( self, id, element ):
    '''
    '''
    if ( not id >= 0 )  or not element:
      Helper.Error( "Wrong call: id and element have to be set" )
      return False

    if self.__textLabelElementIDs.has_key( id ):
      Helper.Warning( "We are overwriting an element.." )

    self.__textLabelElementIDs[id] = element

  def clearElements( self ):
    '''
    '''
    # clear off the layout from checkbuttons
    for n in self.__checkButtonElementIDs.keys():
      print "removing.."
      w = self.__checkButtonElementIDs[n]
      if self.layout():
        self.layout().removeWidget( w )
      w.deleteLater()
      w.setParent( None )
      w = None
      self.__checkButtonElementIDs[n] = None

    # clear off the layout from textlabels
    for n in self.__textLabelElementIDs.keys():
      print "removing.."
      w = self.__textLabelElementIDs[n]
      if self.layout():
        self.layout().removeWidget( w )
      w.deleteLater()
      w.setParent( None )
      w = None
      self.__textLabelElementIDs[n] = None

    # reset the dicts
    self.__checkButtonElementIDs = dict()
    self.__textLabelElementIDs = dict()


  def DefineCheckButton( self, label, initState, id ):
    '''
    '''
    newCheckButton = qt.QCheckBox( label )
    newCheckButton.setChecked( initState )

    if self.layout():
      self.layout().addWidget( newCheckButton )
      self.saveCheckButtonElement( id, newCheckButton )
      return True
    else:
      return False

  def DefineTextLabel( self, label, id ):
    '''
    '''
    labelReplaced = label.replace( "\n", "<br>" )

    newLabel = qt.QLabel( labelReplaced )

    if self.layout():
      self.layout().addWidget( newLabel )
      self.saveTextLabelElement( id, newLabel )
      return True
    else:
      return False



