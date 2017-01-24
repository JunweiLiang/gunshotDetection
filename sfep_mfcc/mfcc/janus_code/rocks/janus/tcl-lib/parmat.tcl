# ========================================================================
#  JANUS-SR   Janus Speech Recognition Toolkit
#             ------------------------------------------------------------
#             Object: Parameter matrix set (CBNewParMatrix set)
#             ------------------------------------------------------------
#
#  Author  :  Matthias Seeger
#  Module  :  parmat.tcl
#  Date    :  26.1.1998
#
#  Remarks :  
#
# ========================================================================


proc parmat_Info { name } {
    switch $name {
	procs {
	    return "parmatSetInit"
	}
	help {
	    return "Library to initialize semi-tied full covariances."
	}
	parmatSetInit {
	    return "Initializes semi-tied full covariances."
	}
    }
    return "???"
}


# ------------------------------------------------------------------------
#  parmatSetInit          creation and initialization of a
#                         CBNewParMatrixSet object based on a description
#                         file and a parameter file.
# ------------------------------------------------------------------------

proc parmatSetInit { LSID args } {
  upvar \#0 $LSID SID

  set parmatSet   parmatSet$LSID
  set log         stderr
  set dimN        -1

  if [info exists SID(parmatSetDesc)] {
      set parmatSetDesc $SID(parmatSetDesc)
  }
  if [info exists SID(parmatSetParam)] { 
      set parmatSetParam $SID(parmatSetParam)
  }

  itfParseArgv parmatSetInit $args [list [
    list -parmatSet   object  {} parmatSet        {} {parameter matrix set} ] [
    list -desc        string  {} parmatSetDesc    {} {description file}    ] [
    list -dimN        int     {} dimN             {} {number of feature space dim. (if no desc. file is used)} ] [
    list -param       string  {} parmatSetParam   {} {parameter file}      ] [
    list -log         string  {} log              {} {name of log channel} ] ]

  if { [llength [info command $parmatSet]] < 1} {

    if { [info exists parmatSetDesc] &&
         [string length $parmatSetDesc] > 0} {
      writeLog $log "CBNewParMatrixSet $parmatSet @$parmatSetDesc"
      CBNewParMatrixSet $parmatSet @$parmatSetDesc
      set SID(parmatSetDesc) $parmatSetDesc
      if { [info exists parmatSetParam] &&
	   [string length $parmatSetParam] > 0} { 
        writeLog $log "$parmatSet loadWeights $parmatSetParam"
	$parmatSet loadWeights $parmatSetParam
	set SID(parmatSetParam) $parmatSetParam
      }
    } elseif { [info exists dimN] && $dimN > 0 } {
      writeLog $log "CBNewParMatrixSet $parmatSet $dimN"
      CBNewParMatrixSet $parmatSet $dimN
    } else {
      writeLog $log "Failed to create CBNewParMatrixSet $parmatSet:"
      writeLog $log "No desc. file or -dimN argument > 0 detected!"
    }
  }
}
