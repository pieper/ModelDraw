
package require Itcl

#########################################################
#
if {0} { ;# comment

  MRMLWatcher is a simple class to observe the mrml
  scene and maintain a global MRML array to allow 
  access to the elements

  this file also contains some simple test procs that can
  be used from the console for interactive performance
  assessment

}
#
#########################################################


proc MRMLWatch {} {
  if { ![info exists ::MRML] } {
    MRMLWatcher #auto
  }
}

proc TimeJumpSliceIteration {sliceNode} {
  $sliceNode JumpSlice 0 0 0
  update
  $sliceNode JumpSlice 0 0 40
  update
}

proc TimeJumpSlice { {sliceName "Red"} {iters 10} } {
  MRMLWatch
  set sliceNode $::MRML($sliceName)
  set usecs [lindex [time "TimeJumpSliceIteration $sliceNode" $iters] 0]
  set fps [expr 1. / ($usecs / 1e6)]
  puts "$fps frames per second"
}


proc TimeSlideSliceIteration {offsetScale} {
  $offsetScale ButtonPressCallback
  $offsetScale SetValue 10
  $offsetScale ButtonReleaseCallback
  update
  $offsetScale ButtonPressCallback
  $offsetScale SetValue 50
  $offsetScale ButtonReleaseCallback
  update
}

proc TimeSlideSlice { {sliceName "Red"} {iters 10} } {
  set offsetScale ""
  foreach sc [vtkSlicerSliceControllerWidget ListInstances] {
    if { [[$sc GetSliceNode] GetName] == $sliceName } {
      set offsetScale [$sc GetOffsetScale]
    }
  }
  set usecs [lindex [time "TimeSlideSliceIteration $offsetScale" $iters] 0]
  set fps [expr 1. / ($usecs / 1e6)]
  puts "$fps frames per second"
}


#
## Memory tracking - optimized for linux
#

proc MemoryTrack { {interval 1000} } {
  
  if { $interval == "cancel" } {
    if { [info exists ::MRML(memoryTrackAfterID)] } {
      after cancel $::MRML(memoryTrackAfterID)
    }
    return
  }

  if { ![info exists ::MRML(sysinfo)] } {
    set ::MRML(sysinfo) [vtkSystemInformation New]
  }
  $::MRML(sysinfo) RunMemoryCheck
  set availableMemory [$::MRML(sysinfo) GetAvailablePhysicalMemory]

  if { [file exists "/proc/[pid]/statm"] } {
    set fp [open "/proc/[pid]/statm" "r"]
    set statm [gets $fp]
    close $fp
    scan $statm "%d" vsize
    # TODO: assume 4096 pagesize for now
    set vsize [expr $vsize/1024 * 4]

    puts "using $vsize - $availableMemory left"
  } else {
    puts "$availableMemory left"
  }

  set ::MRML(memoryTrackAfterID) [after $interval ::MemoryTrack $interval]
}


#
# The partent class definition - define if needed (not when re-sourcing)
#
if { [itcl::find class MRMLWatcher] == "" } {

  itcl::class MRMLWatcher {

    constructor  {} {
    }

    destructor {
    }

    # configure options
    public variable scene ""  ;# the scene
    public variable arrayName "MRML"  ;# the array to manage
    public variable verbose 0

    # instance variables
    variable _observerRecords ""

    # methods
    method processEvent {{caller ""} {event ""}} {}
    method removeObservers {} {}
    method refresh {} {}
  }
}

# ------------------------------------------------------------------
#                        CONSTRUCTOR/DESTRUCTOR
# ------------------------------------------------------------------
itcl::body MRMLWatcher::constructor {} {

  $this configure -scene $::slicer3::MRMLScene
}


itcl::body MRMLWatcher::destructor {} {

  $this removeObservers
  array unset $arrayName
}


itcl::configbody MRMLWatcher::scene {
  $this removeObservers
  if { $scene != "" } {
    lappend _observerRecords [$scene AddObserver DeleteEvent "::SWidget::ProtectedDelete $this"]
    lappend _observerRecords [$scene AddObserver AnyEvent "::SWidget::ProtectedCallback $this processEvent $scene"]
  }

  $this processEvent $scene
}

# remove entries from the list box
itcl::body MRMLWatcher::removeObservers { } {
  foreach record $_observerRecords {
    foreach {obj tag} $record {
      if { [info command $obj] != "" } {
        $obj RemoveObserver $tag
      }
    }
  }
  set _observerRecords ""
}

#
# handle gui events
# -basically just map button events onto methods
#
itcl::body MRMLWatcher::processEvent { {caller ""} {event ""} } {

  if { $verbose } {
    puts "$this: [clock seconds] processing event from $caller"
  }

  if { $caller != $scene } {
    error "unknown caller for $this"
    return
  }
  
  $this refresh
}

itcl::body MRMLWatcher::refresh {} {

  array unset ::$arrayName

  set nNodes [$scene GetNumberOfNodes]
  for {set i 0} {$i < $nNodes} {incr i} {
    set node [$scene GetNthNode $i]
    if { ![catch "$node GetID" id] } {
      # if the node has an id, set it in array
      # (here we are catching the error where the node 
      #  is not properly registered with the tcl interp)
      set name [$node GetName]
      set ::${arrayName}(id,$id) $node
      set ::${arrayName}($name) $node
    }
  }
}
