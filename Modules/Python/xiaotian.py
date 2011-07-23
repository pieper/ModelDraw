XML = """<?xml version="1.0" encoding="utf-8"?>
<executable>

  <category>Diffusion</category>
  <title>DTI Processing Pipeline Test Scripts</title>
  <description>
Performs denoise(Joint Rician LMMSE Image Filter), tensor
estimation(Diffusion Tensor Estimation) and FA measurement(Diffusion
Tensor Scalar Measurement) in pipeline.
</description>
  <version>1.0</version>
  <documentation-url></documentation-url>
  <license></license>
  <contributor>Xiaotian He</contributor>
  <parameters>
    <label>IO</label>
    <description>Input/output parameters</description>
    <image type="diffusion-weighted">
      <name>inputVolumeID</name>
      <label>Input DWI Volume</label>
      <channel>input</channel>
      <index>1</index>
      <description>Input DWI volume</description>
    </image>
    <image type="diffusion-weighted">
      <name>denoisedVolumeID</name>
      <label>Denoised DWI Volume</label>
      <channel>output</channel>
      <index>2</index>
      <description>Denoised DWI volume</description>
    </image>
    <image type="tensor">
      <name>outputTensorID</name>
      <label>Output DTI Volume</label>
      <channel>output</channel>
      <index>3</index>
      <description>Estimated DTI volume</description>
    </image>
    <image type ="scalar">
      <name>outputBaselineID</name>
      <label>Output Baseline Volume</label>
      <channel>output</channel>
      <index>4</index>
      <description>Estimated baseline volume</description>
    </image>
    <image type ="label">
      <name>thresholdMaskID</name>
      <label>Otsu Threshold Mask</label>
      <channel>output</channel>
      <index>5</index>
      <description>Otsu Threshold Mask</description>
    </image>
  </parameters>

</executable>
"""

def Execute (inputVolumeID, denoisedVolumeID, outputTensorID, outputBaselineID, thresholdMaskID):
##def Execute (inputVolumeID, outputTensorID, outputBaselineID, thresholdMaskID):


	Slicer = __import__("Slicer")
	slicer = Slicer.slicer
	scene = slicer.MRMLScene

	inputVolume = scene.GetNodeByID(inputVolumeID)

	denoisedVolume = scene.GetNodeByID(denoisedVolumeID)
##	remedyDenoisedVolume = scene.CreateNodeByClass("vtkMRMLDiffusionWeightedVolumeNode")
##	scene.AddNode(remedyDenoisedVolume)
##	remedyDenoisedID = remedyDenoisedVolume.GetID()
	
	outputTensor = scene.GetNodeByID(outputTensorID)
	baseline = scene.GetNodeByID(outputBaselineID)
	threshold = scene.GetNodeByID(thresholdMaskID)

	denoisePlugin = Slicer.Plugin("Joint Rician LMMSE Image Filter")
##	denoisePlugin.Execute(inputVolume)
	denoisePlugin.Execute(inputVolume, denoisedVolume)
##	denoisePlugin.Execute(inputVolume, remedyDenoisedVolume)

##	denoisedVolume = scene.GetNodeByID("vtkMRMLDiffusionWeightedVolumeNode2")
	
	estimationPlugin = Slicer.Plugin("Diffusion Tensor Estimation")
	estimationPlugin.Execute(denoisedVolume, outputTensor, baseline, threshold, otsuomegathreshold=0.8, shiftNeg='True')
##	estimationPlugin.Execute(inputVolume, outputTensor, baseline, threshold, otsuomegathreshold=0.8, shiftNeg=True)
##	estimationPlugin.Execute(remedyDenoisedVolume, outputTensor, baseline, threshold, otsuomegathreshold=0.8, shiftNeg='True')

	return

