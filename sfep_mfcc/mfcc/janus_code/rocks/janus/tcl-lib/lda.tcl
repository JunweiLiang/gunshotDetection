# ========================================================================
#  JanusRTK   Janus Speech Recognition Toolkit
#             ------------------------------------------------------------
#             Object: Linear Discriminant Analysis
#             ------------------------------------------------------------
#
#  Author  :  Michael Finke
#  Module  :  lda.tcl
#  Date    :  06/14/96
#  $Log$
#  Revision 1.5  2006/12/06 10:01:45  fuegen
#  substituted $lbox:$utt to get a path with a saver variant
#
#  Revision 1.4  2003/08/14 11:20:28  fuegen
#  Merged changes on branch jtk-01-01-15-fms (jaguar -> ibis-013)
#
#  Revision 1.3.50.4  2003/06/06 09:43:59  metze
#  Added latview, fixed upvar, updated tclIndex
#
#  Revision 1.3.50.3  2002/11/15 13:51:42  metze
#  Code cleanup
#
#  Revision 1.3.50.2  2002/11/04 15:03:57  metze
#  Added documentation code
#
#  Revision 1.3.50.1  2002/10/17 12:11:54  soltau
#  support for labelboxes
#
#  Revision 1.3  2000/09/10 12:18:41  hyu
#  Added Tcl files from Hagen
#
#  Revision 1.2  2000/05/11 12:38:54  soltau
#  support modality stuff
#
#  Revision 1.1  2000/05/11 09:49:51  soltau
#  Initial revision
#
# ========================================================================


proc lda_Info { name } {
    switch $name {
	procs {
	    return "doLDA"
	}
	help {
	    return "LDA (Linear Discriminant Analysis) is part of the
standard preprocessing in the JRTk toolkit."
	}
	doLDA {
	    return "Computes the LDA matrix. Also extracts the counts (i.e. frames)
for every codebook, which is useful information and is used to determine
the module during sample extraction."
	}
    }
    return "???"
}


# ------------------------------------------------------------------------
#  doLDA   accumulates LDA scatter information in parallel and a server
#          process finally computes the simultaneous diagonalization
# ------------------------------------------------------------------------

proc doLDA { LSID args} {
    global env USE_MODALITY
    upvar  \#0 $LSID SID

    set labels    "labels/\$spk.lbox.gz"
    set optWord   \$
    set variants   1
    set stream     0
    set path       path$LSID
    set featureSet featureSet$LSID
    set senoneSet  senoneSet$LSID
    set hmm        hmm$LSID
    set lbox       lbox$LSID
    set log        stderr

    itfParseArgv doLDA $args [list [
    list <LDA>         object  {} lda        LDA  {LDA object}          ] [
    list <spkIDfile>   string  {} file       {}   {file of speaker IDs} ] [
    list -countsFile   string  {} ldaCounts  {}   {file to save counts} ] [
    list -labelPath    string  {} labels     {}   {path of label files} ] [
    list -stream       int     {} stream     {}   {stream index}        ] [
    list -optWord      string  {} optWord    {}   {optional word}       ] [
    list -variants     int     {} variants   {}   {variants 0/1}        ] [
    list -featureSet   string  {} featureSet {}   {feature set}         ] [
    list -hmm          string  {} hmm        {}   {hidden markov model} ] [
    list -senoneSet    string  {} senoneSet  {}   {senone set}          ] [
    list -path         string  {} path       {}   {path object}         ] [
    list -lbox         string  {} lbox       {}   {lbox object}         ] [
    list -semFile      string  {} semFile    {}   {semaphore file}      ] [
    list -log          string  {} log        {}   {name of log channel} ] ]

    writeLog $log ""
    writeLog $log "doLDA $LSID $args"

    # Only start if we need to
    if {[info exists semFile] && [file exists $semFile]} {
	writeLog $log "doLDA: semaphore file '$semFile' exists, returning."
	return
    }

    set ntrain  0
    set nerrors 0
    doParallel {

	# --------------------------------------------------------------------
	#  Main Loop
	# --------------------------------------------------------------------
	$lda clear

	writeLog $log ""
	writeLog $log "$lda: accumulate $file"
	writeLog $log ""

	set lPath $path
	if {[info exists USE_MODALITY] && $USE_MODALITY} { set lCom modLabelUtterance
	} else {                                           set lCom    labelUtterance }
	while {[fgets $file spk] >= 0} {
	    foreachSegment utt uttDB $spk {
		
		set       uttInfo  [uttDB uttInfo $spk $utt]
		makeArray uttArray $uttInfo
		
		eval set label $labels

		writeLog $log "\%TURN: $spk $utt"
		if {[catch {$lCom $LSID $spk $utt $label \
				-optWord $optWord -variants $variants \
				-hmm $hmm -path $path -lbox $lbox \
				-featureSet $featureSet } msg]} {
		    writeLog $log "ERROR $msg"
		    incr nerrors
		} else {	    
		    if {[llength [info command $lbox]]} { set lPath $lbox.item([$lbox.list index $utt]) }
		    if { $stream > -1 } {
			if {[catch {$lPath map $hmm -senoneSet $senoneSet \
					-stream $stream} score]} {
			    writeLog $log "ERROR $lPath map $hmm $score"
			    incr nerrors
			} else { $lda accu $lPath; incr ntrain }
		    } else {     $lda accu $lPath; incr ntrain }			
		}
	    }
	}
	
	writeLog $log ""
	writeLog $log "$lda accu: {ntrain $ntrain} {nerrors $nerrors}"

	$lda saveMeans    $lda.$env(HOST).[pid].mean
	writeLog $log "$lda saveMeans    $lda.$env(HOST).[pid].mean"
	$lda saveScatter  $lda.$env(HOST).[pid].scat
	writeLog $log "$lda saveScatter  $lda.$env(HOST).[pid].scat"
    } {
	
	# --------------------------------------------------------------------
	#  Server    * clear accumulated statistics
	#            * load statistics from all clients
	#            * update matrixT and matrixW
	#            * simulatenous diagonalization & save result
	# --------------------------------------------------------------------

	$lda clear
	
	writeLog $log ""
	writeLog $log "$lda: combining accumulators..."
	
	foreach mfile [glob $lda.*.mean] { $lda loadMean    $mfile }
	foreach sfile [glob $lda.*.scat] { $lda loadScatter $sfile }
	
	$lda saveMeans    $lda.mean
	$lda saveScatter  $lda.scat
	
	foreach mfile [glob $lda.*.mean] { rm $mfile }
	foreach sfile [glob $lda.*.scat] { rm $sfile }

	$lda update
	
	if [info exists ldaCounts] {
	    set countFp [open $ldaCounts w]
	    foreach class [$lda:] { 
		puts $countFp "{ $class [$lda:$class.mean configure -count] }"
	    }
	    close $countFp
	}
	
	catch { doSimDiag $LSID $lda}

    } {
	# --------------------------------------------------------------------
	#  Server Finish
	# --------------------------------------------------------------------
	
        if {[info exists semFile]} { touch $semFile }
    } { }
}

# ------------------------------------------------------------------------
#  doSimDiag    runs simultaneous diagonalization procedure based on the
#               LDA 
# ------------------------------------------------------------------------

proc doSimDiag { LSID args} {
    global env    
    upvar  \#0 $LSID SID
    set    log  stderr
    
    itfParseArgv doSimDiag $args [list [
    list <LDA>  object  {} lda  LDA  {LDA object} ] ]

    writeLog $log ""
    writeLog $log "$lda: Simultaneous Diagonalization..."
    
    DMatrix ${lda}A
    DMatrix ${lda}K
    
    ${lda}A simdiag ${lda}K $lda.matrixT $lda.matrixW
    
    FMatrix ${lda}B
    ${lda}B DMatrix ${lda}A
    ${lda}B bsave   ${lda}.bmat
    
    ${lda}A destroy
    ${lda}K destroy
    ${lda}B destroy
}
