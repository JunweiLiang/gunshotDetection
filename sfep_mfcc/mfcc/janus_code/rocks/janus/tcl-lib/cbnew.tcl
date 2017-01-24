# ========================================================================
#  JANUS-SR   Janus Speech Recognition Toolkit
#             ------------------------------------------------------------
#             Object: Extended codebook set (CBNew set)
#             ------------------------------------------------------------
#
#  Author  :  Matthias Seeger
#  Module  :  cbnew.tcl
#  Date    :  26.1.1998
#
#  Remarks :  Must be included into the Tcl library!
#
# ========================================================================

proc cbnew_Info { name } {
    switch $name {
	procs {
	    return "cbnewSetInit"
	}
	help {
	    return "This is the Extended Codebook Set. Use it
in conjunction with STCs (semi-tied co-variances), to find the
OFS (optimal feature space)."
	}
	cbnewSetInit {
	    return "Initializes the CBNew set. Load the 'CBNewParMatrixSet',
requires 'ParmatSet'."
	}
    }
    return "???"
}


# ------------------------------------------------------------------------
#  CBNewSetInit      creation and initialization of a CBNewSet object
#                    based on a description file and a parameter file.
# ------------------------------------------------------------------------

proc cbnewSetInit { LSID args } {
  upvar \#0 $LSID SID

  set cbnewSet    cbnewSet$LSID
  set featureSet  featureSet$LSID
  set parmatSet   parmatSet$LSID
  set log         stderr

  if [info exist SID(cbnewSetDesc)] {
      set cbnewSetDesc $SID(cbnewSetDesc)
  }
  if [info exist SID(cbnewSetParam)] { 
      set cbnewSetParam $SID(cbnewSetParam)
  }

  itfParseArgv cbnewSetInit $args [list [
    list -cbnewSet    string  {} cbnewSet         {} {codebook set}     ] [
    list -featureSet  object  {} featureSet FeatureSet {feature set}       ] [
    list -parmatSet   object  {} parmatSet CBNewParMatrixSet {parameter matrix set}       ] [
    list -desc        string  {} cbnewSetDesc     {} {description file}    ] [
    list -param       string  {} cbnewSetParam    {} {parameter file}      ] [
    list -log         string  {} log              {} {name of log channel} ] ]

  if { [llength [info command $featureSet]] &&
       [llength [info command $parmatSet]] &&
       [llength [info command $cbnewSet]] < 1} {

    if { [info exists cbnewSetDesc] &&
         [string length $cbnewSetDesc] > 0} {
      writeLog $log "CBNewSet $cbnewSet $parmatSet $featureSet @$cbnewSetDesc"
      CBNewSet $cbnewSet $parmatSet $featureSet @$cbnewSetDesc
      set SID(cbnewSetDesc) $cbnewSetDesc
      if { [info exists cbnewSetParam] &&
           [string length $cbnewSetParam] > 0} {
        writeLog $log "$cbnewSet loadWeights $cbnewSetParam"
        $cbnewSet loadWeights $cbnewSetParam
        set SID(cbnewSetParam) $cbnewSetParam
      }
    } else {
      set dimN [$parmatSet configure -dimN]
      writeLog $log "CBNewSet $cbnewSet $parmatSet $featureSet $dimN"
      CBNewSet $cbnewSet $parmatSet $featureSet $dimN
    }
  }
}

# ------------------------------------------------------------------------
#  Protocol management functions
# ------------------------------------------------------------------------
#  Protocol lists are maintained by Janus via
#  the global array cbnewProt (Index: <SID>,<cbX>,<protNum>).
#  An active protocol list is associated with an active protocol
#  slot of a CBNew codebook who will feed the list with values.
#  There's another global array cbnewProtChanged (indexed the same
#  way) which contains a flag for every active protocol:
#    0 => The list has not been changed since last saving
#    1 => Since last saving, some insertions took place
#  This array is maintained, but not used by the Tcl functions. It
#  should however be used by GUI's or the user himself (e.g. to
#  display warnings before a changed list is destroyed without
#  saving). 'Saving' means performing cbnewSaveProt or
#  cbnewAppendProt, the target is not necessarily a file but may
#  also be a Tcl list variable.
# ------------------------------------------------------------------------

#  --- Internal functions ---

#  -------------------------------------------------------------------
#  _cbnewSetWriteProt <fileID> <listvar>
#  Writes protocol list into file. Format: GNUplot (one entry per row,
#  entry = <timepoint> <critValue>).
#  -------------------------------------------------------------------

proc _cbnewSetWriteProt {fileID listvar} {
    foreach elem $listvar {
	puts $fileID "[lindex $elem 0] [lindex $elem 1]"
    }
}

#  -------------------------------------------------------------------
#  _cbnewSetAddToProtLists <SID> <cbX> <listvar>
#  Adds the entries of <listvar> to the corr. protocol lists of the
#  codebook with index <cbX>. An entry has the form
#    {<protNum> <timepoint> <critValue>}.
#  -------------------------------------------------------------------

proc _cbnewSetAddToProtLists {LSID cbX listvar} {
    global cbnewProt cbnewProtChanged

    foreach elem $listvar {
	;# we have to use 'expr' because 'protnum' may contain whitespace!
	set protnum [expr [lindex $elem 0]]
	set cbX [expr $cbX]
	lappend cbnewProt($LSID,$cbX,$protnum) [lrange $elem 1 2]
	set cbnewProtChanged($LSID,$cbX,$protnum) 1
    }
}


#  --- User interface ---

#  -------------------------------------------------------------------
#  cbnewSetOpenProt <SID> <cbName> <datamat> {-critFunc <cname>}
#    {-begin <num>} {-end <num>}
#  Opens new protocol using a free slot. The protocol list is
#  initially empty. The slot number is returned.
#  -------------------------------------------------------------------

proc cbnewSetOpenProt {LSID args} {
    global cbnewProt cbnewProtChanged
    upvar  \#0 $LSID SID

    set cbnewSet    cbnewSet$LSID
    set log         stderr
    set cbName      ""
    set datamat     ""
    set crit        ""
    set begin       ""
    set end         ""

    itfParseArgv cbnewSetOpenProt $args [list [
    list <cbName>  string {} cbName   {}      {codebook name}] [
    list <dataMat> object {} datamat  FMatrix {evaluation data matrix}] [
    list -critFunc string {} crit     {}      {criterion function}] [
    list -begin    int    {} begin    {}      {first row of eval. epoch in data matrix}] [
    list -end      int    {} end      {}      {last row of eval. epoch in data matrix}] [
    list -log      string {} log      {}      {name of log channel}]]

    set cbIndex [$cbnewSet index $cbName]
    if {$cbIndex < 0} {
	writelog $log "ERROR: Codebook $cbName doesn't exist!"
	return
    }
    set command [list $cbnewSet openProt $cbIndex $datamat]
    if {[string length $crit]} {
	lappend command -critFunc $crit
    }
    if {[string length $begin]} {
	lappend command -begin $begin
    }
    if {[string length $end]} {
	lappend command -end $end
    }
    if [catch {eval $command} protnum] {
	writeLog $loge "ERROR $command\n$protnum"
	return
    }

    set protnum [expr $protnum]
    set cbIndex [expr $cbIndex]
    set cbnewProt($LSID,$cbIndex,$protnum) {}
    set cbnewProtChanged($LSID,$cbIndex,$protnum) 0
    return $protnum
}

#  -------------------------------------------------------------------
#  cbnewSetSaveProt <SID> <cbName> <protNum> {-file <fname>}
#    {-list <listvar>}
#  Saves protocol list into file <fname>. <fname> is overwritten. The
#  list is also copied to the Tcl list <listvar>. If neither -file nor
#  -list is present, an error is raised.
#  <listvar> must be the name of a global variable.
#  -------------------------------------------------------------------

proc cbnewSetSaveProt {LSID args} {
    global cbnewProt cbnewProtChanged
    upvar  \#0 $LSID SID

    set cbnewSet    cbnewSet$LSID
    set log         stderr
    set cbName      ""
    set protnum     ""
    set fname       ""
    set listvar     ""

    itfParseArgv cbnewSetSaveProt $args [list [
    list <cbName>  string {} cbName   {}      {codebook name}] [
    list <protNum> int    {} protnum  {}      {protocol number}] [
    list -file     string {} fname    {}      {protocol file name}] [
    list -list     string {} listvar  {}      {Tcl list variable}] [
    list -log      string {} log      {}      {name of log channel}]]

    set cbIndex [$cbnewSet index $cbName]
    if {$cbIndex < 0} {
	writelog $log "ERROR cbnewSetSaveProt\nCodebook $cbName doesn't exist!"
	return
    }
    # One of -file,-list must be present
    if {![string length $fname] && ![string length $listvar]} {
	writeLog $log "ERROR cbnewSetSaveProt\nNo target specified!"
	return
    }
    # Check if protocol slot is active
    set cbIndex [expr $cbIndex]
    if {![info exists cbnewProt($LSID,$cbIndex,$protnum)]} {
	writeLog $log "ERROR cbnewSetSaveProt\nProtocol does not exist!\n$result"
	return
    }
    # Save list and delete changed flag
    if {[string length $fname]} {
	if [catch {open $fname w} fileID] {
	    writeLog $log "ERROR cbnewSetSaveProt\nCan't open protocol file '$fname'\n$fileID"
	    return
	}
	_cbnewSetWriteProt $fileID $cbnewProt($LSID,$cbIndex,$protnum)
	catch {close $fileID}
    }
    if {[string length $listvar]} {
	upvar \#0 $listvar lv  ;# variable must be global!
	set lv $cbnewProt($LSID,$cbIndex,$protnum)
    }
    set cbnewProtChanged($LSID,$cbIndex,$protnum) 0
}

#  -------------------------------------------------------------------
#  cbnewSetAppendProt <SID> <cbName> <protNum> {-file <fname>}
#    {-list <listvar>}
#  Same as cbnewSetSaveProt, but the protocol list is appended.
#  -------------------------------------------------------------------

proc cbnewSetAppendProt {LSID args} {
    global cbnewProt cbnewProtChanged
    upvar  \#0 $LSID SID

    set cbnewSet    cbnewSet$LSID
    set log         stderr
    set cbName      ""
    set protnum     ""
    set fname       ""
    set listvar     ""

    itfParseArgv cbnewSetAppendProt $args [list [
    list <cbName>  string {} cbName   {}      {codebook name}] [
    list <protNum> int    {} protnum  {}      {protocol number}] [
    list -file     string {} fname    {}      {protocol file name}] [
    list -list     string {} listvar  {}      {Tcl list variable}] [
    list -log      string {} log      {}      {name of log channel}]]

    set cbIndex [$cbnewSet index $cbName]
    if {$cbIndex < 0} {
	writelog $log "ERROR cbnewSetAppendProt\nCodebook $cbName doesn't exist!"
	return
    }
    set cbIndex [expr $cbIndex]
    # One of -file,-list must be present
    if {![string length $fname] && ![string length $listvar]} {
	writeLog $log "ERROR cbnewSetAppendProt\nNo target specified!"
	return
    }
    # Check if protocol slot is active
    if {![info exists cbnewProt($LSID,$cbIndex,$protnum)]} {
	writeLog $log "ERROR cbnewSetAppendProt\nProtocol does not exist!\n$result"
	return
    }
    # Append list and delete changed flag
    if {[string length $fname]} {
	if {[file exists $fname]} {
	    if [catch {open $fname a} fileID] {
		error "ERROR cbnewSetAppendProt\nCan't open protocol file '$fname'\n$fileID"
	    }
	} else {
	    if [catch {open $fname w} fileID] {
		error "ERROR cbnewSetAppendProt\nCan't open protocol file '$fname'\n$fileID"
	    }
	}
	_cbnewSetWriteProt $fileID $cbnewProt($LSID,$cbIndex,$protnum)
	catch {close $fileID}
    }
    if {[string length $listvar]} {
	upvar \#0 $listvar lv
	set lv [concat $lv $cbnewProt($LSID,$cbIndex,$protnum]
    }
    set cbnewProtChanged($LSID,$cbIndex,$protnum) 0
}

#  -------------------------------------------------------------------
#  cbnewSetDestroyProt <SID> <obj> <protNum>
#  Destroys the protocol at slot <protNum> without saving!
#  -------------------------------------------------------------------

proc cbnewSetDestroyProt {LSID args} {
    global cbnewProt cbnewProtChanged
    upvar  \#0 $LSID SID

    set cbnewSet    cbnewSet$LSID
    set log         stderr
    set cbName      ""
    set protnum     ""

    itfParseArgv cbnewSetDestroyProt $args [list [
    list <cbName>  string {} cbName   {}      {codebook name}] [
    list <protNum> int    {} protnum  {}      {protocol number}] [
    list -log      string {} log      {}      {name of log channel}]]

    set cbIndex [$cbnewSet index $cbName]
    if {$cbIndex < 0} {
	writelog $log "ERROR cbnewSetDestroyProt\nCodebook $cbName doesn't exist!"
	return
    }
    set cbIndex [expr $cbIndex]
    set command [list $cbnewSet closeProt $cbIndex $protnum]
    if [catch {eval $command} result] {
	writeLog $log "ERROR $command\n$result"
	return
    }
    unset cbnewProt($LSID,$cbIndex,$protnum)
    unset cbnewProtChanged($LSID,$cbIndex,$protnum)
}

#  -------------------------------------------------------------------
#  cbnewSetEvalProt <SID> <cbName> {-prot <protNum>}
#  Evaluates protocol slot <protNum> or all active slots if -prot is
#  not present.
#  -------------------------------------------------------------------

proc cbnewSetEvalProt {LSID args} {
    upvar \#0 $LSID SID

    set cbnewSet    cbnewSet$LSID
    set log         stderr
    set cbName      ""
    set protnum     ""

    itfParseArgv cbnewSetEvalProt $args [list [
    list <cbName>  string {} cbName   {}      {codebook name}] [
    list -protNum  int    {} protnum  {}      {protocol number (def: all)}] [
    list -log      string {} log      {}      {name of log channel}]]

    set cbIndex [$cbnewSet index $cbName]
    if {$cbIndex < 0} {
	writelog $log "ERROR cbnewSetEvalProt\nCodebook $cbName doesn't exist!"
	return
    }
    set cbIndex [expr $cbIndex]
    set command [list $cbnewSet evalProt $cbIndex]
    if {[string length $protnum]} {
	lappend command -prot $protnum
    }
    if [catch {eval $command} result] {
	writelog $log "ERROR $command\n$result"
	return
    }
    _cbnewSetAddToProtLists $LSID $cbIndex $result
}
