#
#
# set sw [SWidget #auto]; $sw configure -sliceGUI [lindex [vtkSlicerSliceGUI ListInstances] 0]
# 
# EffectSWidget #auto [lindex [vtkSlicerSliceGUI ListInstances] 0]
#
# obs; dobs


if { ![array exists ::OBS] } {
  array unset ::OBS
  array set ::OBS {}
  set ::obsCount 0
}

proc obs {} {

  set o $::obsCount
  incr ::obsCount
  set ::OBS($o,insts) ""

  set totalObs 0
  set cmds [info command vtk*]

  foreach cmd $cmds {
    if { [string match vtkObj\[0-9\]* $cmd] || [string match vtkTemp\[0-9\]* $cmd] } { continue } 
    if { ![catch "$cmd ListInstances" insts] } {
      foreach inst $insts {
        if { ![catch "$inst IsA vtkObject" res] } {
          if { $res != 1 } { continue }
          set ::OBS($o,$inst,obs) [llength [lsearch -all [$inst Print] vtkObserver]]
          set totalObs [expr $totalObs + $::OBS($o,$inst,obs)]
          set ::OBS($o,$inst,type) $cmd
          lappend ::OBS($o,insts) $inst
        }
      }
    }
  }
  set ::OBS($o,totalObs) $totalObs
  puts "$totalObs in sample $o"
}

proc dobs { {p ""} {t ""} } {
  if { $t == "" } {
    set t [expr $::obsCount - 1]
  }
  if { $p == "" } {
    set p [expr $::obsCount - 2]
  }

  foreach inst $::OBS($p,insts) {
    if { [lsearch $::OBS($t,insts) $inst] == -1 } {
      puts "$inst ($::OBS($p,$inst,type)) is gone"
    }
  }
  foreach inst $::OBS($t,insts) {
    if { [lsearch $::OBS($p,insts) $inst] == -1 } {
      puts "$inst ($::OBS($t,$inst,type)) is new with $::OBS($t,$inst,obs) observers"
    } else {
      set now $::OBS($t,$inst,obs)
      set then $::OBS($p,$inst,obs)
      if { $now != $then } {
        puts "$inst ($::OBS($t,$inst,type)) has $now but previously had $then"
      }
    }
  }
}
