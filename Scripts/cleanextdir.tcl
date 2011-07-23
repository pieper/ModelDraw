#!/bin/sh
# the next line restarts using tclsh \
    exec tclsh "$0" "$@"



# little utility script for deleting old copies of slicer extension zip files
# NOTE: this runs on unix/posix only (requires find command)
#
proc Usage {} {
  puts "cleanextdir \[options\] <dir>"
  puts "-h, --help (this message)"
  puts "-v, --verbose print updates while processing"
  puts "<dir> is extension dir created by Slicer's extend.tcl script"
  puts ""
  puts "This script deletes all be the latest version of an extension build."
  puts "The <dir> is searched recursively for extenisons."
}

set ::DIR ""
set ::VERBOSE 0

set strippedargs ""
set argc [llength $argv]
for {set i 0} {$i < $argc} {incr i} {
    set a [lindex $argv $i]
    switch -glob -- $a {
        "-v" -
        "--verbose" {
            set ::VERBOSE 1
        }
        "--help" -
        "-h" {
            Usage
            exit 1
        }
        "-*" {
            Usage "unknown option $a\n"
            exit 1
        }
        default {
            lappend strippedargs $a
        }
    }
}

set argv $strippedargs
set argc [llength $argv]

if {$argc != 1 } {
  Usage
  exit 1
}

set ::DIR $strippedargs

set dirs [exec find $::DIR -type d]

foreach dir $dirs {
  if { $::VERBOSE } {
    puts "checking $dir"
  }
  set files [glob -nocomplain $dir/*.zip]
  foreach f $files {
    if { ![file exists $f] } {
      # file was deleted in earlier iteration
      continue
    }
    set fTail [file tail $f]]

    # strip off everything before the svn tag, since
    # module names may have their own hyphens embedded
    # (like plastimatch-slicer, for example)
    set svnIndex [string first "-svn" $fTail]
    if { $svnIndex == -1 } {
      # no svn tag, skip it
      continue
    }

    set module [string range $fTail 0 [expr $svnIndex -1]]
    set id [string range $fTail [expr $svnIndex + 1] end]


    # extract the date, finds all matching files in dir
    set namelist [split $id "-."]
    if { [llength $namelist] < 5 } {
      # file was not a slicer extension file, skip it
      continue
    }

    # get all builds for this module
    set matchfiles [lsort [glob $dir/$module-svn*]]

    # delete all but the most recent build of the most recent svn number
    if { [llength $matchfiles] > 1 } {
      foreach ff [lrange $matchfiles 0 end-1] {
        if { $::VERBOSE } {
          puts "deleting $ff"
        }
        file delete $ff
      }
    }
    if { $::VERBOSE } {
      puts "keeping [lindex $matchfiles end]"
    }
  }
}
