

set objects {
  ren1 renWin iren
  ellip reader volume rayCastFunction
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

# Create transfer mapping scalar value to opacity
vtkPiecewiseFunction opacityTransferFunction
    opacityTransferFunction AddPoint  80   0.0
    opacityTransferFunction AddPoint  120  0.2
    opacityTransferFunction AddPoint  255  0.2

proc thresh {t} {
  opacityTransferFunction RemoveAllPoints
  opacityTransferFunction AddPoint 0 0
  opacityTransferFunction AddPoint [expr $t - 1] 0
  opacityTransferFunction AddPoint $t 1.0
}

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


# Create some sample data
# This is the data the will be volume rendered
vtkImageEllipsoidSource ellip
ellip SetWholeExtent 0 255 0 255 0 255
[ellip GetOutput] Update
set data [ellip GetOutput]

set dataFile /home/pieper/data/CTC-256.nhdr
if { [file exists $dataFile] } {
  vtkNRRDReader reader
  reader SetFileName $dataFile
  reader Update
  set data [reader GetOutput]
  thresh 3500
}


vtkVolumeRayCastIsosurfaceFunction rayCastFunction

# The mapper / ray cast function know how to render the data
vtkCudaVolumeMapper volumeMapper
    volumeMapper SetInput $data
    volumeMapper ShadingOn

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

proc cc {} {
  colorTransferFunction RemoveAllPoints
  colorTransferFunction AddRGBPoint 0 0 0 0
  colorTransferFunction AddRGBPoint 1 50 50 50
  colorTransferFunction AddRGBPoint 1000 50 50 50
}


proc tanim {} {
  for {set t 0} {$t < 600} {incr t} {
    thresh $t
    renWin Render
    update
  }
}

proc rot {} {
  for {set t 0} {$t < 36} {incr t} {
    [ren1 GetActiveCamera] Azimuth 10.0
    renWin Render
    update
  }
}
