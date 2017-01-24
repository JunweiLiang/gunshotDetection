# ========================================================================
#  JANUS-SR   Janus Speech Recognition Toolkit
#             ------------------------------------------------------------
#             Object: Sample Extraction
#             ------------------------------------------------------------
#
#  Author  :  Michael Finke
#  Module  :  samples.tcl
#  Date    :  06/14/96
#
#  $Log$
#  Revision 1.10  2006/12/06 10:52:49  fuegen
#  bugfix for lbox handling
#  replaced $lbox:$utt by a saver variant
#
#  Revision 1.9  2003/08/14 11:20:30  fuegen
#  Merged changes on branch jtk-01-01-15-fms (jaguar -> ibis-013)
#
#  Revision 1.8.50.7  2003/06/06 09:43:59  metze
#  Added latview, fixed upvar, updated tclIndex
#
#  Revision 1.8.50.6  2003/04/01 15:13:33  metze
#  Moved writing of codebook list in server part
#
#  Revision 1.8.50.5  2002/11/25 14:32:45  metze
#  Write *.cbs earlier
#
#  Revision 1.8.50.4  2002/11/04 15:03:57  metze
#  Added documentation code
#
#  Revision 1.8.50.3  2002/10/17 11:57:47  soltau
#  Support for labelboxes
#
#  Revision 1.8.50.2  2002/10/14 09:14:17  metze
#  Added catch

#  Revision 1.8.50.1  2002/02/09 14:08:32  metze
#  Added regsub in sample's filename: "&AH/1" => "&AH_1"
#
#  Revision 1.8  2000/09/10 12:18:45  hyu
#  Added Tcl files from Hagen
#
#  Revision 1.7  2000/05/11 12:39:18  soltau
#  support modality stuff
#
#  Revision 1.6  2000/05/11 10:02:01  soltau
#  Changed a couple of things
#
#  Revision 1.5  1999/10/22 12:58:26  soltau
#  Fiex bug
#
#  Revision 1.4  1999/10/21 12:47:26  soltau
#  in any case. we compile a list of codebooks
#
#  Revision 1.3  1999/10/21 09:19:47  soltau
#  do not combine samples flag
#
#  Revision 1.2  1999/10/14 08:27:36  soltau
#  handle single cpu usage
#
#  Revision 1.1  1999/10/13 17:18:39  soltau
#  Initial revision
#
#
# ========================================================================


proc samples_Info { name } {
    switch $name {
	procs {
	    return "doExtract"
	}
	help {
	    return "Allows to extract samples, i.e. store the pre-processed
data for every frame given labels and use it directly at a later stage, for
example for KMeans."
	}
	doExtract {
	    return "Extract the data in separate files for each codebook
according to a given alignment. This is very heavy on file I/O, so plan
your setup accordingly. If you specify a counts file, you can also specify
the 'maxCount'; the system will then automatically compute a modulo, which
prevents more than 'maxCount' samples to be extracted for every codebook."
	}
    }
    return "???"
}


# ------------------------------------------------------------------------
#  doExtract
# ------------------------------------------------------------------------

proc doExtract { LSID args } {
    global env USE_MODALITY
    upvar  \#0 $LSID SID

    set labels      "labels/\$spk.lbox.gz"
    set data         data
    set data_comb   ""
    set path         path$LSID
    set lbox         lbox$LSID
    set hmm          hmm$LSID
    set amodelSet    amodelSet$LSID
    set senoneSet    senoneSet$LSID
    set codebookSet  codebookSet$LSID
    set featureSet   featureSet$LSID
    set log          stderr
    set optWord     \$
    set variants     0
    set stream      -1
    set maxCount     20000
    set modulus      1
    set doCombine    0

    itfParseArgv doExtract $args [list [
    list <SampleSet>   object  {} smp        SampleSet  {SampleSet object} ] [
    list <spkIDfile>   string  {} file       {}   {file of speaker IDs}    ] [
    list -path         string  {} path       {}   {name of path}           ] [
    list -lbox         string  {} lbox       {}   {name of lbox}           ] [
    list -labelPath    string  {} labels     {}   {path of label files}    ] [
    list -dataPath     string  {} data       {}   {path of data files}     ] [
    list -combPath     string  {} data_comb  {}   {path for combining files} ] [
    list -countsFile   string  {} countsFile {}   {file to save counts}    ] [
    list -maxCount     int     {} maxCount   {}   {max count in file}      ] [
    list -modulus      int     {} modulus    {}   {modulus}                ] [
    list -stream       int     {} stream     {}   {stream index}           ] [
    list -optWord      string  {} optWord    {}   {optional word}          ] [
    list -variants     int     {} variants   {}   {variants 0/1}           ] [
    list -doCombine    int     {} doCombine  {}   {doCombine 0/1}          ] [
    list -semFile      string  {} semFile    {}   {semaphore file}         ] [
    list -log          string  {} log        {}   {name of log channel}    ] ]

    writeLog $log ""
    writeLog $log "doExtract $LSID $args"

    # ----------------------------------------------------------------------
    #  Configure SampleSet object
    # ----------------------------------------------------------------------
    
    # Only start if we need to
    if {[info exists semFile] && [file exists $semFile]} {
	writeLog $log "doExtract: semaphore file '$semFile' exists, returning."
	return
    }

    writeLog $log ""
    writeLog $log "configuration of $smp $data/data.$env(HOST).[pid]/*.smp" 

    file mkdir $data/data.$env(HOST).[pid]
    foreach s [${smp}:] {
	regsub -all "/" $s "_" f
	${smp}:$s configure -fileName $data/data.$env(HOST).[pid]/${f}.smp \
	    -maxCount $maxCount -modulus $modulus
    }
    
    # ----------------------------------------------------------------------
    #  Set modulus based on a counts file generated in the LDA pass. The
    #  goal is to set the modulus parameter such that the maxCount frames
    #  extracted from the database are equally well distributed over
    #  the database.
    # ----------------------------------------------------------------------
    
    if {[info exist countsFile] && "$countsFile" != ""} {
	if ![file exists $countsFile] {
	    error "Couldn't find counts file $countsFile"
	}
	set FI [open $countsFile r]
	makeArray counts [read $FI]
	close $FI
	
	writeLog $log ""
	writeLog $log "reconfiguration of $smp"
	writeLog $log "counts: $countsFile maxCount: $maxCount"

	foreach class [$smp:] {
	    if [info exist counts($class)] {
		$smp:$class configure -modulus [expr 1+$counts($class)/$maxCount]
	    } else {
		writeLog $log "WARN $smp:$class no counts -> modulus = $modulus"
	    }
	}
    }

    # new treatment of combining samples
    if {$doCombine} { writeLog $log "\n$smp: ignoring doCombine=1" }
    if {[string length $data_comb]} { set doCombine 1 }
    
    set ntrain  0
    set nerrors 0
    doParallel { 
    
	# --------------------------------------------------------------------
	#  Main Loop
	# --------------------------------------------------------------------
	
	writeLog $log ""
	writeLog $log "$smp: accumulate $file"
	writeLog $log ""
	
	while {[fgets $file spk] >= 0} {
	    foreachSegment utt uttDB $spk {
		set       uttInfo  [uttDB uttInfo $spk $utt]
		makeArray uttArray $uttInfo
		
		eval set label $labels
		
		writeLog $log "\%TURN: $spk $utt"
		
		if { [info exists USE_MODALITY] && ${USE_MODALITY} == 1} {
		    set fehler [catch {modLabelUtterance $LSID $spk $utt $label -path $path -lbox $lbox \
					   -optWord $optWord -variants $variants \
					   -hmm $hmm -path $path \
					   -featureSet $featureSet } msg]
		} else {
		    set fehler [catch {labelUtterance $LSID $spk $utt $label -path $path -lbox $lbox \
					   -optWord $optWord -variants $variants \
					   -hmm $hmm -path $path   \
					   -featureSet $featureSet } msg]
		}
		if {$fehler} {
		    incr nerrors
		    writeLog $log "ERROR $msg"
		    continue
		}
		if { [llength [info command $lbox]] > 0 } { set path $lbox.item([$lbox.list index $utt]) }
		if { $stream > -1 } {
		    if [catch {$path map $hmm -senoneSet $senoneSet -stream $stream} score] {
			incr nerrors
			writeLog $log "ERROR $path map $hmm $score"
		    } else {
			$smp accu $path; incr ntrain 
		    }
		} else {  
		    $smp accu $path; incr ntrain 
		}
	    }
	    $amodelSet reset
	    $senoneSet reset
	}
	$smp flush
	writeLog $log ""
	writeLog $log "$smp accu: {ntrain $ntrain} {nerrors $nerrors}"

    } {
	# --------------------------------------------------------------------
	#  Server
	# --------------------------------------------------------------------

	# generate codebook list for KMeans
	writeLog $log ""
	writeLog $log "compile list of codebooks > $file.cbs..."    
	set fp [open $file.cbs w]
	foreach cb [$codebookSet:] { puts $fp "$cb" }
	close $fp
	
	# generate temporary cbslst for Combining accumulators
	if {$doCombine} {
	    set fp [open ".tmp.cbslst" w]
	    foreach cb [${smp}:] { puts $fp $cb }
	    close $fp
	}

    } {
	# --------------------------------------------------------------------
	#  Server Finish: Combining accumulators
	# --------------------------------------------------------------------
	
	if {!$doCombine} {
	    writeLog $log ""
	    writeLog $log "do not combine accumulators"
	} else { doCombine $data $data_comb }

        if {[info exists semFile] && [string length $semFile]} { touch $semFile }

    } { 
	# --------------------------------------------------------------------
	#  Client Finish
	# --------------------------------------------------------------------

	if ${doCombine} { doCombine $data $data_comb }
    }
}

proc doCombine { data_temp data {list .tmp.cbslst} {log stderr} } {
    FMatrix smpM
    set pathLst [glob $data_temp/data.*]
    set pathT   [lindex $pathLst 0]
    set pathS   [lrange $pathLst 1 end]
    set data    [file join $data [file tail $pathT]]
    file mkdir  $data

    writeLog $log ""
    writeLog $log "combining accumulators $data <- $pathLst"

    while {[fgets $list cb] != -1} {
	if {[file exists [file join $data ${cb}.smp]]} {
	    smpM bload   [file join $data ${cb}.smp]
	    set cnt  [smpM configure -m]
	    set path $pathS
	} else {
	    set cnt  0
	    set path $pathLst
	}
	foreach mfile $path {
	    if {![file exists $mfile/$cb.smp]} { continue }
	    smpM bload $mfile/$cb.smp
	    set cnt [expr $cnt + [smpM configure -m]]
	    smpM bappend $data/$cb.smp
	    file delete $mfile/$cb.smp
	}
	writeLog $log "smpM:$cb counts: $cnt" 
    }

    smpM destroy
}
