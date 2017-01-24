# ========================================================================
#  JANUS-SR   Janus Speech Recognition Toolkit
#             ------------------------------------------------------------
#             Object: General Routines
#             ------------------------------------------------------------
#
#  Author  :  Michael Finke
#  Module  :  misc.tcl
#  Date    :  $Id: misc.tcl 3086 2010-01-20 04:07:47Z metze $
#
#  Remarks :  
#
# ========================================================================
#
# $Log$
# Revision 1.5  2007/01/02 09:27:00  fuegen
# added global doParallelJOB variable, which allows to run multiple
# doParallel processes working on different lists in the same directory,
# by setting this variable to process specific values
#
# Revision 1.4  2004/08/16 16:20:17  metze
# P014
#
# Revision 1.3  2003/11/13 12:36:50  metze
# Introduced fileAfter
#
# Revision 1.2  2003/08/14 11:20:29  fuegen
# Merged changes on branch jtk-01-01-15-fms (jaguar -> ibis-013)
#
# Revision 1.1.50.10  2003/07/16 11:56:52  fuegen
# changed channel for putsInfo to stderr
#
# Revision 1.1.50.9  2002/11/15 13:53:04  metze
# Merged in my changes
#
# Revision 1.1.50.8  2002/11/15 09:30:18  fuegen
# added some puts functions, helpful for logging
#
# Revision 1.1.50.7  2002/11/08 14:27:50  metze
# Flexible doParallel once and for all: use 'doParallelServer'
#
# Revision 1.1.50.6  2002/11/04 15:03:57  metze
# Added documentation code
#
# Revision 1.1.50.5  2002/10/17 12:14:30  soltau
# doParallel: list of jobs base now on [lsort [glob *INIT]] instead of [glob *END]
#
# Revision 1.1.50.4  2002/03/17 12:16:16  metze
# Fixed major cock-up: "glob *.DONE" would find kmeans.DONE and
# the first job to finish would become the server and start right
# away (not waiting for the others ...)
#
# Revision 1.1.50.3  2002/03/16 17:36:01  metze
# Separated glob and -nocomplain
#
# Revision 1.1.50.2  2002/02/13 11:16:14  soltau
# *** empty log message ***
#
# Revision 1.1.50.1  2002/02/11 10:24:01  soltau
# doParallel: added a couple of nocomplains, and catch rm commands
#
# ========================================================================


proc misc_Info { name } {
    switch $name {
	procs {
	    return "printDo"
	}
	help {
	    return "This file contains various procedures."
	}
	printDo {
	    return "Performs an action (its argument) and prints the command
line to stderr. Don't try to set variables within printDo, though."
	}
	putsInfo {
	    return "Same as puts, but starting with 'INFO <called function>: '. Default output channel is stdout."
	}
	putsWarn {
	    return "Same as puts, but starting with 'WARN <called function>: '. Default output channel is stderr."
	}
	putsError {
	    return "Same as puts, but starting with 'ERROR <called function>: '. Default output channel is stderr."
	}
	putsDebug {
	    return "Same as puts, but starting with 'DEBUG <called function>: '. Default output channel is stderr. Printing such debug messages can be turned on, by setting PrintDebug to 1 (default is 0)."
	}
    }
    return "???"
}


# ------------------------------------------------------------------------
#  printDo
# ----------
#  Just a little helper function
# ------------------------------------------------------------------------

proc printDo args {
    writeLog stderr "$args" 2
    uplevel  eval    $args
}


# --------------------------------------------------------------------
#  random package
# --------------------------------------------------------------------

proc randomInit { seed } { 
    global rand
    set rand(ia) 9301    ; # Multiplier
    set rand(ic) 49297   ; # Constant
    set rand(im) 233280  ; # Divisor
    set rand(seed) $seed ; # last result
}

proc random {} {
    global rand
    set rand(seed) [expr ($rand(seed) * $rand(ia) + $rand(ic)) % $rand(im) ]
    return [expr $rand(seed) / double($rand(im))]
}

proc randomRange { range } {
    expr int([random] * $range)
}


# ------------------------------------------------------------------------
#  makeDir            creates a subdirectory structure (i.e. multi-level
#                     subdirectories) if it does not exist already
# ------------------------------------------------------------------------

proc makeDir { args } {

  set dir  [pwd]
  set path ""

  itfParseArgv makeDir $args [list [
    list <dir> string  {} dir {} {directory} ] ]

  set sdl [split $dir "/"]
  set sdl [lrange $sdl 0 [expr [llength $sdl]-2]]

  foreach sd $sdl {
    if { $sd == ""} {
      if { $path == "" } {
	set path "/"
      }
    } elseif [file isdirectory $path$sd] {
      set path "$path$sd/"
    } else {
      if {! [file exist $path$sd]} {
        mkdir $path$sd
        set path "$path$sd/"
      }
    }
  }
  return $path
}


# ------------------------------------------------------------------------
#  writeLog
# ------------------------------------------------------------------------

proc writeLog { file line {sub 1}} {
  if {[string length $file]} {
    set level [expr [info level] - $sub]
    for { set i 0 } { $i < $level } { incr i } { puts -nonewline $file "  " }
    puts $file $line
  }
}


# ------------------------------------------------------------------------
#  doParallel
# ------------------------------------------------------------------------

proc fileAfter { waitSeconds } {
    global env

    after       $waitSeconds
    touch       .$env(HOST).[pid].tmp
    file delete .$env(HOST).[pid].tmp
}

proc doParallel { body server serverFinish client } {
  global          env
  global          doParallelJOB doParallelBODYONLY doParallelSERVERONLY

  if {![info exists doParallelJOB]} {
    set doParallelJOB ""
  } else {
    writeLog stderr "using '$doParallelJOB' as head for glob"
  }

  set host        $env(HOST)
  set semFile     ${doParallelJOB}$host.[pid]
  set waitSeconds [expr 5 * 1000]

  # Added for better support of Condor
  if {[llength [glob -nocomplain ${doParallelJOB}*.*.SERVER]] > 0} {
    writeLog stderr "\ndoParallel: found active SERVER [glob ${doParallelJOB}*.*.SERVER], aborting"
    return
  }
  if {[info exists doParallelSERVERONLY] && $doParallelSERVERONLY} {
    writeLog stderr "\ndoParallel: executing SERVER only"
    uplevel $server
    uplevel $serverFinish
    return
  }

  touch $semFile.INIT

  uplevel $body

  # This can be used to terminate relatively cleanly under Condor
  if {[info exists doParallelBODYONLY] && $doParallelBODYONLY} {
    writeLog stderr "\ndoParallel: executing BODY only"
    # file rename -force $semFile.INIT $semFile.BODY
    file delete $semFile.INIT
    return -1
  }

  touch $semFile.END

  writeLog stderr "\ndoParallel: waiting for [glob -nocomplain ${doParallelJOB}*.INIT] ..."
  while { [llength [glob -nocomplain ${doParallelJOB}*.INIT]] != [llength [glob -nocomplain ${doParallelJOB}*.END]] } {
    fileAfter $waitSeconds
  }

  if {[catch {set SERVER [doParallelServer ${doParallelJOB}]}]} {
    # puts stderr "SERVER-LIST [lsort [glob ${doParallelJOB}*.INIT]]"
    set SERVER [lindex [lsort [glob ${doParallelJOB}*.INIT]] end]
    set SERVER [string range $SERVER 0 [expr [string length $SERVER]-6]]
  } else {
    writeLog stderr "\ndoParallel: using 'doParallelServer' function: $SERVER"
  }

  if { $SERVER == $semFile } {
    writeLog stderr "\ndoParallel: server $SERVER executing ..."

    touch $SERVER.SERVER

    uplevel $server

    writeLog stderr "\ndoParallel: waiting [expr 0.002*$waitSeconds]s to synchronize clients"
    after $waitSeconds
    after $waitSeconds

    file delete -force $SERVER.END

    writeLog stderr "\ndoParallel: server $SERVER waiting for clients ..."
    while { [llength [glob -nocomplain ${doParallelJOB}*.END]] } { fileAfter $waitSeconds }
    writeLog stderr "\ndoParallel: server $SERVER finishing up ..."

    file delete -force $SERVER.INIT

    uplevel $serverFinish

    file delete -force $SERVER.SERVER

  } else {

    writeLog stderr "\ndoParallel: waiting for server $SERVER.END (to disappear) ..."
    while { [file exists $SERVER.END] } { fileAfter $waitSeconds }

    file delete -force $semFile.INIT

    uplevel $client

    file delete -force $semFile.END

    writeLog stderr "\ndoParallel: waiting for server $SERVER.SERVER (to disappear) ..."
    while { [file exists $SERVER.SERVER] } { fileAfter $waitSeconds }
  }
}


# ------------------------------------------------------------------------
#  waitFile
# ------------------------------------------------------------------------

proc waitFile args {
    set sleep 10000
    set after  1000

    if {[ itfParseArgv waitFile $args [list [
      list <dir>  string  {} files {} {file or file list} ] [
      list -sleep string  {} sleep {} {ms to wait between tries} ] [
      list -after string  {} after {} {ms to wait after file was found} ] ]
    ] != ""} return


    writeLog stderr "waiting for $files ..."
    foreach file $files {
      while {! [file exists $file]} { after $sleep }
      writeLog stderr "... found $file @ [clock format [clock seconds]]"
    }
    after $after
}


# ----------------------------------------------------------------------
# putsError, PutsWarn, putsInfo, putsDebug
# several useful functions for logging
# ----------------------------------------------------------------------

proc putsError { args } {
    global PutsArgs

    set PutsArgs $args
    set res      [putsType putsError]
    unset PutsArgs

    return $res
}

proc putsWarn { args } {
    global PutsArgs

    set PutsArgs $args
    set res      [putsType putsWarn]
    unset PutsArgs

    return $res
}

proc putsInfo { args } {
    global PutsArgs

    set PutsArgs $args
    set res      [putsType putsInfo]
    unset PutsArgs

    return $res
}

proc putsDebug { args } {
    global PutsArgs

    set PutsArgs $args
    set res      [putsType putsDebug]
    unset PutsArgs

    return $res
}

proc putsType { type } {
    global PutsArgs
    global ExitOnError PrintDebug

    set args $PutsArgs

    # type settings
    switch $type {
        putsError {
            set tag     ERROR
            set channel stderr
            if { ![info exists ExitOnError] } { set ExitOnError 0 }
        }
        putsWarn {
            set tag     WARN
            set channel stderr
        }
        putsInfo {
            set tag     INFO
            set channel stderr
        }
        putsDebug {
            set tag     DEBUG
            set channel stderr
            if { ![info exists PrintDebug] } { set PrintDebug 0 }
            if { !$PrintDebug } { return 0 }
        }
        default {
            error "putsType: unknow puts type '$type'"
        }
    }

    # parse argv
    set nonewline 0
    set argc      [llength $args]

    if { $argc && ![string compare [lindex $args 0] "-nonewline"] } {
        set nonewline 1
        set args      [lrange $args 1 end]
        incr argc -1
    }

    if { $argc == 0 } {
        error "wrong \# args: should be '$type ?-nonewline? ?channelId? string'"
    }

    if { $argc == 2 } {
        set channel [lindex $args 0]
        set string  [lindex $args 1]
    } elseif { $argc == 1 } {
        set string  [lindex $args 0]
    } else {
        error "wrong \# args: should be '$type ?-nonewline? ?channelId? string'"
    }

    # get called procedure and level
    set level [expr [info level] - 2]
    set call  ""

    if { $level > 0 } { set call [lindex [info level $level] 0] }

    # print tabs
    for {set i 0} {$i < $level} {incr i} { puts -nonewline $channel "  " }

    # print message
    if { $nonewline } {
        if { [catch {puts -nonewline $channel "$tag $call: $string"} msg] } {
            error "$type: $msg"
        }
    } else {
        if { [catch {puts $channel "$tag $call: $string"} msg] } {
            error "$type: $msg"
        }
    }

    flush $channel

    switch $type {
        putsError {
            if { $ExitOnError } { exit 0 }
        }
    }

    return 0
}

proc putsMem { {txt ""} } {
    set a [exec cat /proc/[pid]/status]
    regexp {VmSize:\t *([0-9]*) kB} $a tmp v
    regexp {VmRSS:\t *([0-9]*) kB} $a tmp r

    puts "MEMORY $txt is [expr int($r./1024)] of [expr 1+int($v./1024)] Mb"
}
