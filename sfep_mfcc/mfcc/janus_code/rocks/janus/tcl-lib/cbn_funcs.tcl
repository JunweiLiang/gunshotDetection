# ========================================================================
#  JANUS-SR   Janus Speech Recognition Toolkit
#             ------------------------------------------------------------
#             Object: Tcl functions for CBNew
#             ------------------------------------------------------------
#
#  Author  :  Matthias Seeger
#  Module  :  cbn_funcs.tcl
#  Date    :  26 Jan 1998
#  Remarks :  Must be integrated in cbnew.tcl!
#
# ========================================================================

#  -------------------------------------------------------------------
#  Tcl user functions
#  - Protocol management: Protocol lists are maintained by Janus via
#    the global array CBNewProt (Index: <cbsName>,<cbX>,<protNum>).
#    An active protocol list is associated with an active protocol
#    slot of a CBNew object who will feed the list with values.
#    There's another global array CBNewProtChanged (indexed the same
#    way) which contains a flag for every active protocol:
#      0 => The list has not been changed since last saving
#      1 => Since last saving, some insertions took place
#    This array is maintained, but not used by the Tcl functions. It
#    should however be used by GUI's or the user himself (e.g. to
#    display warnings before a changed list is destroyed without
#    saving). 'Saving' means performing CBNewSaveProt or
#    CBNewAppendProt, the target is not necessarily a file but may
#    also be a Tcl list variable.
#  -------------------------------------------------------------------

#  --- Internal functions ---

#  -------------------------------------------------------------------
#  _CBNewWriteProt <fileID> <listvar>
#  Writes protocol list into file. Format: GNUplot (one entry per row,
#  entry = <timepoint> <critValue>).
#  -------------------------------------------------------------------

proc _CBNewWriteProt {fileID listvar} {
    foreach elem $listvar {
	puts $fileID "[lindex $elem 0] [lindex $elem 1]"
    }
}

#  -------------------------------------------------------------------
#  _CBNewAddToProtLists <cbsName> <cbX> <listvar>
#  Adds the entries of <listvar> to the corr. protocol lists of the
#  CBNew object <cbsName>,<cbX>. An entry has the form
#    {<protNum> <timepoint> <critValue>}.
#  -------------------------------------------------------------------

proc _CBNewAddToProtLists {cbsName cbX listvar} {
    global CBNewProt CBNewProtChanged

    foreach elem $listvar {
	;# we have to use 'expr' because 'protnum' may contain whitespace!
	set protnum [expr [lindex $elem 0]]
	set cbX [expr $cbX]
	lappend CBNewProt($cbsName,$cbX,$protnum) [lrange $elem 1 2]
	set CBNewProtChanged($cbsName,$cbX,$protnum) 1
    }
}


#  --- User interface ---

#  -------------------------------------------------------------------
#  CBNewOpenProt <cbsName> <cbName> <datamat> {-critFunc <cname>}
#    {-begin <num>} {-end <num>}
#  Opens new protocol using a free slot. The protocol list is
#  initially empty. The slot number is returned.
#  -------------------------------------------------------------------

proc CBNewOpenProt {args} {
    global CBNewProt CBNewProtChanged

    set cbsName ""
    set cbName ""
    set datamat ""
    set crit ""
    set begin ""
    set end ""

    itfParseArgv CBNewOpenProt $args [list [
    list <cbsName> object {} cbsName  CBNewSet  {CBNewSet object}] [
    list <cbName>  object {} cbName   CBNew     {codebook in <cbsName>}] [
    list <dataMat> object {} datamat  FMatrix {evaluation data matrix}] [
    list -critFunc string {} crit     {}      {criterion function}] [
    list -begin    int    {} begin    {}      {first row of eval. epoch in data matrix}] [
    list -end      int    {} end      {}      {last row of eval. epoch in data matrix}]]

    if {[catch {$cbsName index $cbName} cbIndex] || $cbIndex < 0} {
	error "ERROR: Codebook $cbName has no index!"
    }
    set command [list $cbsName openProt $cbIndex $datamat]
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
	error "ERROR $command\n$protnum"
    }

    set protnum [expr $protnum]
    set cbIndex [expr $cbIndex]
    set CBNewProt($cbsName,$cbIndex,$protnum) {}
    set CBNewProtChanged($cbsName,$cbIndex,$protnum) 0
    return $protnum
}

#  -------------------------------------------------------------------
#  CBNewSaveProt <cbsName> <cbName> <protNum> {-file <fname>}
#    {-list <listvar>}
#  Saves protocol list into file <fname>. <fname> is overwritten. The
#  list is also copied to the Tcl list <listvar>. If neither -file nor
#  -list is present, an error is raised.
#  <listvar> must be the name of a global variable.
#  -------------------------------------------------------------------

proc CBNewSaveProt {args} {
    global CBNewProt CBNewProtChanged

    set cbsName ""
    set cbName ""
    set protnum ""
    set fname ""
    set listvar ""

    itfParseArgv CBNewSaveProt $args [list [
    list <obj>     object {} obj      CBNew  {CBNew object}] [
    list <protNum> int    {} protnum  {}      {protocol number}] [
    list -file     string {} fname    {}      {protocol file name}] [
    list -list     string {} listvar  {}      {Tcl list variable}]]

    ;# One of -file,-list must be present
    if {![string length $fname] && ![string length $listvar]} {
	error "ERROR CBNewSaveProt\nNo target specified!"
    }
    ;# Check if protocol slot is active
    if {![info exists CBNewProt($obj,$protnum)]} {
	error "ERROR CBNewSaveProt\nProtocol does not exist!\n$result"
    }
    ;# Save list and delete changed flag
    if {[string length $fname]} {
	if [catch {open $fname w} fileID] {
	    error "ERROR CBNewSaveProt\nCan't open protocol file '$fname'\n$fileID"
	}
	_CBNewWriteProt $fileID $CBNewProt($obj,$protnum)
	catch {close $fileID}
    }
    if {[string length $listvar]} {
	upvar \#0 $listvar lv
	set lv $CBNewProt($obj,$protnum)
    }
    set CBNewProtChanged($obj,$protnum) 0
}

#  -------------------------------------------------------------------
#  CBNewAppendProt <obj> <protNum> {-file <fname>} {-list <listvar>}
#  Same as CBNewSaveProt, but the protocol list is appended.
#  -------------------------------------------------------------------

proc CBNewAppendProt {args} {
    global CBNewProt CBNewProtChanged

    set obj ""
    set protnum ""
    set fname ""
    set listvar ""

    itfParseArgv CBNewAppendProt $args [list [
    list <obj>     object {} obj      CBNew  {CBNew object}] [
    list <protNum> int    {} protnum  {}      {protocol number}] [
    list -file     string {} fname    {}      {protocol file name}] [
    list -list     string {} listvar  {}      {Tcl list variable}]]

    ;# One of -file,-list must be present
    if {![string length $fname] && ![string length $listvar]} {
	error "ERROR CBNewAppendProt\nNo target specified!"
    }
    ;# Check if protocol slot is active
    if {![info exists CBNewProt($obj,$protnum)]} {
	error "ERROR CBNewAppendProt\nProtocol does not exist!\n$result"
    }
    ;# Append list and delete changed flag
    if {[string length $fname]} {
	if {[file exists $fname]} {
	    if [catch {open $fname a} fileID] {
		error "ERROR CBNewOpenProt\nCan't open protocol file '$fname'\n$fileID"
	    }
	} else {
	    if [catch {open $fname w} fileID] {
		error "ERROR CBNewOpenProt\nCan't open protocol file '$fname'\n$fileID"
	    }
	}
	_CBNewWriteProt $fileID $CBNewProt($obj,$protnum)
	catch {close $fileID}
    }
    if {[string length $listvar]} {
	upvar \#0 $listvar lv
	set lv [concat $lv $CBNewProt($obj,$protnum]
    }
    set CBNewProtChanged($obj,$protnum) 0
}

#  -------------------------------------------------------------------
#  CBNewDestroyProt <obj> <protNum>
#  Destroys the protocol at slot <protNum> without saving!
#  -------------------------------------------------------------------

proc CBNewDestroyProt {args} {
    global CBNewProt CBNewProtChanged

    set obj ""
    set protnum ""

    itfParseArgv CBNewDestroyProt $args [list [
    list <obj>     object {} obj      CBNew   {CBNew object}] [
    list <protNum> int    {} protnum  {}      {protocol number}]]

    set command [list $obj closeProt $protnum]
    if [catch {eval $command} result] {
	error "ERROR $command\n$result"
    }
    unset CBNewProt($obj,$protnum)
    unset CBNewProtChanged($obj,$protnum)
}

#  -------------------------------------------------------------------
#  CBNewEvalProt <obj> {-prot <protNum>}
#  Evaluates protocol slot <protNum> or all active slots if -prot is
#  not present.
#  -------------------------------------------------------------------

proc CBNewEvalProt {args} {
    set obj ""
    set protnum ""

    itfParseArgv CBNewEvalProt $args [list [
    list <obj>     object {} obj      CBNew   {CBNew object}] [
    list -prot     int    {} protnum  {}      {protocol number (none => eval. all prot.)}]]

    set command [list $obj evalProt]
    if {[string length $protnum]} {
	lappend command -prot $protnum
    }
    if [catch {eval $command} result] {
	error "ERROR $command\n$result"
    }
    _CBNewAddToProtLists $obj $result
}
