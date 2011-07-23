from Slicer import slicer
import sys
import os
import shutil
import tempfile

#
# AtlasCreator Test 1
# - fixed registration
#

# setup the pathes..
pathToData = os.path.normpath(slicer.Application.GetBinDir() + '/../share/Slicer3/Modules/AtlasCreator/TestData/')
segPath = pathToData + os.sep + 'segmentations' + os.sep 
origPath = pathToData + os.sep + 'originals' + os.sep
outDir = tempfile.mkdtemp('ACTest') + os.sep

# create the node
n = slicer.vtkMRMLAtlasCreatorNode() 
n.InitializeByDefault()
n.SetOriginalImagesFilePathList(origPath + 'case60.nrrd ' + origPath + 'case61.nrrd ' + origPath + 'case62.nrrd')
n.SetSegmentationsFilePathList(segPath + 'case60.nrrd ' + segPath + 'case61.nrrd ' + segPath + 'case62.nrrd')

# use CMTK, if available, this will likely fall back to BRAINSFit
n.SetToolkit('CMTK')

n.SetOutputDirectory(outDir)
n.SetFixedTemplateDefaultCaseFilePath(origPath + 'case62.nrrd')
n.SetDebugMode(1)
slicer.MRMLScene.AddNode(n)
n.SetLabelsList('3 4 5')

# set test mode
n.SetTestMode(1)

# and fire it up!!
n.Launch();

# to check if everything worked, simply check if the atlases are there
if os.path.isfile(outDir + 'atlas3.nrrd') and os.path.isfile(outDir + 'atlas4.nrrd') and os.path.isfile(outDir + 'atlas5.nrrd'):
    # all there, cleanup
    shutil.rmtree(outDir,True)
    ret = True
else:
    # error
    ret = False

# send back the correct exit code depending on failure or success..
if ret:
    slicer.Application.Evaluate("exit 0")
else:
    slicer.Application.Evaluate("exit 1")


