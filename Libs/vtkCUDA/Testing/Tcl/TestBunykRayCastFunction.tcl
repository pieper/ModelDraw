

set objects {
  ren1 renWin iren
  ellip volume rayCastFunction
  opacityTransferFunction colorTransferFunction volumeProperty volumeMapper
}
foreach o $objects {
  if { [info command $o] != "" } {
    $o Delete
  }
}

# Create the standard renderer, render window
# and interactor
vtkRenderer ren1
vtkRenderWindow renWin
    renWin AddRenderer ren1
vtkRenderWindowInteractor iren
    iren SetRenderWindow renWin
    iren SetDesiredUpdateRate 3

# Create some sample data
# This is the data the will be volume rendered
vtkImageEllipsoidSource ellip
ellip SetWholeExtent 0 255 0 255 0 255
[ellip GetOutput] Update

# Create transfer mapping scalar value to opacity
vtkPiecewiseFunction opacityTransferFunction
    opacityTransferFunction AddPoint  80   0.0
    opacityTransferFunction AddPoint  120  0.2
    opacityTransferFunction AddPoint  255  0.2

# Create transfer mapping scalar value to color
vtkColorTransferFunction colorTransferFunction
    colorTransferFunction AddRGBPoint     80.0 0.0 0.0 0.0
    colorTransferFunction AddRGBPoint    120.0 0.0 0.0 1.0
    colorTransferFunction AddRGBPoint    160.0 1.0 0.0 0.0
    colorTransferFunction AddRGBPoint    200.0 0.0 1.0 0.0
    colorTransferFunction AddRGBPoint    255.0 0.0 1.0 1.0

# The property describes how the data will look
vtkVolumeProperty volumeProperty
    volumeProperty SetColor colorTransferFunction
    volumeProperty SetScalarOpacity opacityTransferFunction
    volumeProperty ShadeOff
    volumeProperty SetInterpolationTypeToLinear

vtkVolumeRayCastIsosurfaceFunction rayCastFunction

# The mapper / ray cast function know how to render the data
#vtkGPUVolumeRayCastMapper volumeMapper
#vtkVolumeRayCastMapper volumeMapper
vtkCudaVolumeMapper volumeMapper
    volumeMapper SetVolumeRayCastFunction rayCastFunction
    volumeMapper SetInputConnection [ellip GetOutputPort]

# The volume holds the mapper and the property and
# can be used to position/orient the volume
vtkVolume volume
    volume SetMapper volumeMapper
    volume SetProperty volumeProperty

ren1 AddVolume volume
renWin SetSize 300 300

ren1 ResetCamera
[ren1 GetActiveCamera] Azimuth 20.0
[ren1 GetActiveCamera] Elevation 10.0
[ren1 GetActiveCamera] Zoom 1.5

renWin Render


proc TkCheckAbort {} {
  set foo [renWin GetEventPending]
  if {$foo != 0} {renWin SetAbortRender 1}
}
renWin AddObserver "AbortCheckEvent" {TkCheckAbort}

iren AddObserver UserEvent {wm deiconify .vtkInteract}
iren Initialize

#wm withdraw .



