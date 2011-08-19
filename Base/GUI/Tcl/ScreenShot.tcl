
#------------------------------------------------------------------------------
# utility to grab screenshots
#------------------------------------------------------------------------------
proc SlicerSaveLargeImage { fileName resolutionFactor } {
    
  set appGUI $::slicer3::ApplicationGUI
  set viewerWidget [$appGUI GetActiveViewerWidget]
  set mainViewer [$viewerWidget GetMainViewer]
  set window [ $mainViewer GetRenderWindow ]
  set renderer [$mainViewer GetRenderer]

  #
  # commented out because vtkRenderLargeImage was
  # not handling vtkActors properly (was copying them
  # into each tile rather than magnifiying them once.
  #
  #set renderLargeImage [vtkRenderLargeImage New]
  #$renderLargeImage SetInput $renderer
  #$renderLargeImage SetMagnification $resolutionFactor
  set renderLargeImage [ vtkWindowToImageFilter New ]
  $renderLargeImage SetInput $window
  $renderLargeImage SetMagnification $resolutionFactor

  switch { [string tolower [file ext $fileName]] } {
    "png" {
      set writer [vtkPNGWriter New]
    }
    "jpg" - "jpeg" {
      set writer [vtkJPEGWriter New]
    }
    "tif" - "tiff" {
      set writer [vtkTIFFWriter New]
    }
    "eps" - "ps" - "prn" {
      set writer [vtkPostscriptWriter New]
    }
    "pnm" - "ppm" {
      set writer [vtkPNMWriter New]
    }
    default {
      set writer [vtkPNGWriter New]
    }
  }

  $writer SetInput [$renderLargeImage GetOutput]
  $writer SetFileName $fileName
  $writer Write

  $renderLargeImage Delete
  $writer Delete
  # put the renderer back to normal
  $mainViewer Render
}


proc SlicerSaveEachRenderCancel { {renderer ""} } {
  if { $renderer == "" } {
    set appGUI $::slicer3::ApplicationGUI
    set viewerWidget [$appGUI GetActiveViewerWidget]
    set mainViewer [$viewerWidget GetMainViewer]
    set window [ $mainViewer GetRenderWindow ]
    set renderer [$mainViewer GetRenderer]
  }
  $renderer RemoveObserver $::SLICERSAVE($renderer,observerTag)
}

proc SlicerSaveEachRender { {fileNamePattern /tmp/slicer-%04d.png} {resolutionFactor 1} } {

  set appGUI $::slicer3::ApplicationGUI
  set viewerWidget [$appGUI GetActiveViewerWidget]
  set mainViewer [$viewerWidget GetMainViewer]
  set window [ $mainViewer GetRenderWindow ]
  set renderer [$mainViewer GetRenderer]

  if { [info exists ::SLICERSAVE($renderer,observerTag)] } {
    SlicerSaveEachRenderCancel $renderer
  }
  set ::SLICERSAVE($renderer,observerTag) [$renderer AddObserver EndEvent "SlicerSaveEachRenderCallback $renderer"]

  set ::SLICERSAVE($renderer,fileNamePattern) $fileNamePattern
  set ::SLICERSAVE($renderer,resolutionFactor) $resolutionFactor
  set ::SLICERSAVE($renderer,frameNumber) 0
  set ::SLICERSAVE($renderer,saving) 0
}

proc SlicerSaveEachRenderCallback { renderer } {
  if { $::SLICERSAVE($renderer,saving) } {
    # don't do a screen grab triggered by our own render
    puts "skipping"
    return
  }
  set ::SLICERSAVE($renderer,saving) 1

  set fileName [format $::SLICERSAVE($renderer,fileNamePattern) $::SLICERSAVE($renderer,frameNumber)]
  incr ::SLICERSAVE($renderer,frameNumber)

  SlicerSaveLargeImage $fileName $::SLICERSAVE($renderer,resolutionFactor)
  set ::SLICERSAVE($renderer,saving) 0
}

proc SlicerSpinMovie { {degrees 5} {fps 10} } {

  set camera [[$::slicer3::MRMLScene GetNthNodeByClass 0 "vtkMRMLCameraNode"] GetCamera]

  SlicerSaveEachRender
  [$::slicer3::ApplicationGUI GetMainSlicerWindow] SetStatusText "Starting render..."

  for {set arc 0} {$arc < 360} {set arc [expr $arc + $degrees] } {
    $camera Azimuth $degrees
    $camera OrthogonalizeViewUp

    # Make the lighting follow the camera to avoid illumination changes
    set viewerWidget [$::slicer3::ApplicationGUI GetActiveViewerWidget]
    [[$viewerWidget GetMainViewer] GetRenderer] UpdateLightsGeometryToFollowCamera
    $viewerWidget RequestRender
    update
    [$::slicer3::ApplicationGUI GetMainSlicerWindow] SetStatusText "Rendered $arc"
  }

  SlicerSaveEachRenderCancel
  [$::slicer3::ApplicationGUI GetMainSlicerWindow] SetStatusText "Encoding..."

  # TODO: need an encoder command and way to view:
  # ffmpeg -v]eval exec ffmpeg -y -i $files \/tmp/movie.ogv
  # try webm
  # see, for example: http://diveintohtml5.org/video.html
  # and https://groups.google.com/a/webmproject.org/group/webm-discuss/browse_thread/thread/438f52c6683bedde?pli=1
  # or try: ffmpeg  -i slicer-0%03d.png video.mpg
  # ffmpeg -i slicer-0%03d.png video.webm
  # http://www.catswhocode.com/blog/19-ffmpeg-commands-for-all-needs
  # for now:
  set files [lsort [glob /tmp/slicer-*.png]]
  eval exec animate -delay 10 $files 

  [$::slicer3::ApplicationGUI GetMainSlicerWindow] SetStatusText "Render finished."
}



