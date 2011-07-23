comment = """
import os
execfile(os.environ['Slicer3_HOME'] + "/../Slicer3/Base/Testing/Performance.py")
"""

global reslicing
def reslicing(iters=100):
  """ go into a loop that stresses the reslice performance
  """
  import time
  import Slicer
  slicesGUI = Slicer.slicer.ApplicationGUI.GetSlicesGUI()
  sliceGUI = slicesGUI.GetNthSliceGUI(1)
  sliceLogic = sliceGUI.GetLogic()
  nodes = Slicer.slicer.ListNodes()
  sliceNode = nodes['Red']
  dims = sliceNode.GetDimensions()
  elapsedTime = 0
  for i in xrange(iters):
    startTime = time.time() 
    sliceLogic.SetSliceOffset(20)
    Slicer.slicer.Application.ProcessPendingEvents()
    endTime1 = time.time()
    sliceLogic.SetSliceOffset(80)
    Slicer.slicer.Application.ProcessPendingEvents()
    endTime2 = time.time()
    delta = ((endTime1-startTime) + (endTime2 - endTime1)) / 2.
    elapsedTime += delta
  fps = iters / elapsedTime
  print ("%d x %d, fps = %g (%g ms per frame)" % (dims[0], dims[1], fps, 1000./fps))

global timeProbe
def timeProbe(iters=10, steps=30):
  for step in xrange(steps):
    reslicing(iters)
