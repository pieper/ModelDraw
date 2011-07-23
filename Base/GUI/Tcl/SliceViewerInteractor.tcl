
package require Itcl


proc SliceViewerFindWidgets { sliceGUI } {
  # find the SliceSWidget for a given gui
  set sswidgets ""
  foreach ssw [itcl::find objects -class SliceSWidget] {
    if { [$ssw cget -sliceGUI] == $sliceGUI } {
      lappend sswidgets $ssw
    }
  }
  return $sswidgets
}

# create a slice widget for the sliceGUI
# - check that there isn't already a widget for this sliceGUI
# - check that logic has been created
# - check that node has been assigned
proc SliceViewerInitialize { sliceGUI } {
  if { [SliceViewerFindWidgets $sliceGUI] != "" } {
    return
  }
  set logic [$sliceGUI GetLogic]
  if { $logic == "" } {
    return
  }
  set node [$logic GetSliceNode]
  if { $node == "" } {
    return
  }
  # new widget is needed, so allocate it
  SliceSWidget #auto $sliceGUI
}


proc SliceViewerShutdown { sliceGUI } {
  # find the SliceSWidget that owns the sliceGUI and delete it
  foreach ssw [SliceViewerFindWidgets $sliceGUI] {
    itcl::delete object $ssw
  }
}

proc __Slicer_bgerror {m} { 
  $::slicer3::Application ErrorMessage "$m"
  $::slicer3::Application ErrorMessage $::errorInfo
  if { [string match *alloc* $m] } {
    set submessage "You may have run out of memory.  Please save your work."
  } else {
    set submessage "This may be a harmless warning, but please use caution."
  }
  if { !$::slicer3::TEST_MODE } {
    set msg "Slicer has encountered an unhandled background error:\n\n  $m\n\n$submessage\nSee the Error Log for more information."
    set dialog [vtkKWMessageDialog New]
    $dialog SetParent [$::slicer3::ApplicationGUI GetMainSlicerWindow]
    $dialog SetMasterWindow [$::slicer3::ApplicationGUI GetMainSlicerWindow]
    $dialog SetStyleToMessage
    $dialog SetText $msg
    $dialog Create
    $dialog Invoke
    $dialog Delete
  }
}


proc SliceViewerHandleEvent {sliceGUI event} {
  # initialize on first call 
  # -- set up our custom bgerror command (it doesn't seem possible to do this
  #    during the Slicer3.cxx startup, so we do it here if needed)
  # -- allows clean shutdown because each vtkSlicerSliceGUI can shutdown in destructor
  # -- this is very light weight, so no problem calling every time

  # hack: peek at the string version of the proc to see if kwwidgets
  # has replaced the system version of bgerror yet - once it has done
  # so, we want to take over with our own version
  if { [info command ::__bgerror_kwwidgets] == "" } {
    if { [info command ::bgerror] != "" && [string match " global Application*" [info body ::bgerror]] } {
      rename ::bgerror ::__bgerror_kwwidgets
      rename ::__Slicer_bgerror ::bgerror
    }
  }

  SliceViewerInitialize $sliceGUI
}
