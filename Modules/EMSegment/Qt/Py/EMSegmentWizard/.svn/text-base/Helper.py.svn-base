# slicer imports
from __main__ import vtk, slicer

# python includes
import sys
import time

class Helper( object ):
  '''
  classdocs
  '''

  @staticmethod
  def Info( message ):
    '''
    
    '''

    print "[EMSegmentPy " + time.strftime( "%m/%d/%Y %H:%M:%S" ) + "]: " + str( message )
    sys.stdout.flush()

  @staticmethod
  def Warning( message ):
    '''
    
    '''

    print "[EMSegmentPy " + time.strftime( "%m/%d/%Y %H:%M:%S" ) + "]: WARNING: " + str( message )
    sys.stdout.flush()

  @staticmethod
  def Error( message ):
    '''
    
    '''

    print "[EMSegmentPy " + time.strftime( "%m/%d/%Y %H:%M:%S" ) + "]: ERROR: " + str( message )
    sys.stdout.flush()


  @staticmethod
  def Debug( message ):
    '''
    
    '''

    showDebugOutput = 1
    from time import strftime
    if showDebugOutput:
        print "[EMSegmentPy " + time.strftime( "%m/%d/%Y %H:%M:%S" ) + "] DEBUG: " + str( message )
        sys.stdout.flush()

  @staticmethod
  def CreateSpace( n ):
    '''
    '''
    spacer = ""
    for s in range( n ):
      spacer += " "

    return spacer


  @staticmethod
  def GetNthStepId( n ):
    '''
    '''
    steps = [None, # 0
             'SelectTask', # 1
             'DefineInputChannels', # 2
             'DefineAnatomicalTree', # 3
             'DefineAtlas', # 4
             'EditRegistrationParameters', # 5
             'DefinePreprocessing', # 6
             'SpecifyIntensityDistribution', # 7
             'EditNodeBasedParameters', # 8
             'DefineMiscParameters', # 9
             'Segment']                         # 10

    if n < 0 or n > len( steps ):
      n = 0

    return steps[n]

  @staticmethod
  def GetRegistrationTypes():
    '''
    '''
    return ['None', 'Fast', 'Accurate']

  @staticmethod
  def GetInterpolationTypes():
    '''
    '''
    return ['Linear', 'Nearest Neighbor', 'Cubic' ]

  @staticmethod
  def GetPackages():
    '''
    '''
    return ['CMTK', 'BRAINS', 'PLASTIMATCH']

  @staticmethod
  def GetSpecificationTypes():
    '''
    '''
    return ['Manual', 'Manual Sampling', 'Auto Sampling']






