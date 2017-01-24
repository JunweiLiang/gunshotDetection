# ------------------------------------------------------------------------
#  Evaluates the Q criterion for all parmats, appends the results to
#  the protocol files. The system-average value is also calc. and
#  stored.
# ------------------------------------------------------------------------

proc calcProts {} {
    global SID qCounter protPath

    writeLog stderr "Calc. and storing Q crit. for all parmats..."
    foreach pm [parmatSet${SID}] {
	set qVal [parmatSet${SID} evalKL -parMat $pm]
	set entry [list $qCounter $qVal]
	set fName $protPath/$pm.prot
	if {[file exists $fName]} {set mode a} else {set mode w}
	set fp [open $fName $mode]
	puts $fp $entry
	close $fp
    }
    writeLog stderr "Calc. and storing system average of Q crit..."
    set qVal [parmatSet${SID} evalKL]
    set entry [list $qCounter $qVal]
    set fName $protPath/sys-avg.prot
    if {[file exists $fName]} {set mode a} else {set mode w}
    set fp [open $fName $mode]
    puts $fp $entry
    close $fp
    incr qCounter
    writeLog stderr "System average of Q crit.: $qVal"
}

proc storeLHProt {lhVal} {
    global lhCounter protPath

    writeLog stderr "Storing LH crit."
    set entry [list $lhCounter $lhVal]
    set fName $protPath/likeli.prot
    if {[file exists $fName]} {set mode a} else {set mode w}
    set fp [open $fName $mode]
    puts $fp $entry
    close $fp
    incr lhCounter
}

proc storeLH {fName} {
    global SID

    set fp [open $fName w]
    puts $fp [senoneSet${SID} configure -likelihood]
    close $fp
}

proc loadLH {fName} {
    global SID

    set fp [open $fName r]
    set lh [expr [gets $fp]]
    close $fp
    return $lh
}
