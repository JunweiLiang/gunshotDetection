# ========================================================================
#  JANUS-SR   Janus Speech Recognition Toolkit
#             ------------------------------------------------------------
#             Object: Labels
#             ------------------------------------------------------------
#
#  Author  :  Michael Finke
#  Module  :  labels.tcl
#  Date    :  06/11/96
#
#  Remarks :  
#  $Log$
#  Revision 1.7  2003/09/09 08:15:21  metze
#  Added 'update' option
#
#  Revision 1.6  2003/08/14 11:20:28  fuegen
#  Merged changes on branch jtk-01-01-15-fms (jaguar -> ibis-013)
#
#  Revision 1.5.2.5  2003/06/06 09:43:59  metze
#  Added latview, fixed upvar, updated tclIndex
#
#  Revision 1.5.2.4  2002/11/04 15:03:57  metze
#  Added documentation code
#
#  Revision 1.5.2.3  2002/10/17 12:11:04  soltau
#  support for labelboxes
#
#  Revision 1.5.2.2  2002/02/13 16:04:37  soltau
#  fixed tryMax
#
#  Revision 1.5.2.1  2001/04/24 08:47:52  metze
#  Initialized msg
#
#  Revision 1.5  2001/01/11 12:33:16  janus
#  Updated ReleaseLog
#
#  Revision 1.4.32.1  2001/01/10 10:43:41  janus
#  Final multivar, Cleanup
#
#  Revision 1.4  2000/09/10 12:18:41  hyu
#  Added Tcl files from Hagen
#
#  Revision 1.3  1999/10/18 15:17:33  soltau
#  made putPath working
#
#  Revision 1.2  1999/10/13 11:44:33  soltau
#  removed doParallel, added options tryMax, putPath
#
#  Revision 1.1  1999/10/13 11:41:30  soltau
#  Initial revision
#
#
# ========================================================================


proc labels_Info { name } {
    switch $name {
	procs {
	    return "labelsWrite labelsMLAdaptWrite"
	}
	help {
	    return "Look here if you need to write labels (time-alignments)."
	}
	labelsWrite {
	    return "Writes labels, i.e. computes and stores a viterbi path
for every utterance of every speaker found in the speaker list. You can
store the labels in separate files or in a 'label-box', which contains
all alignments for one speaker in one singel file."
	}
	labelsMLAdaptWrite {
	    return "Equivalent to 'labelsWrite', except that it performs
speaker-specific MLLR adaptation on the reference before computing the 
labels, which often results in better alignments. Takes more time, though."
	}
    }
    return "???"
}


# ------------------------------------------------------------------------
#  labelsWrite
# ------------------------------------------------------------------------

proc labelsWrite { LSID args} {
    upvar \#0 $LSID SID

    set lbox      lbox$LSID
    set path      path$LSID
    set hmm       hmm$LSID
    set labels    "labels/\$spk.lbox.gz"
    set optWord  \$
    set variants  0
    set beam      300
    set topN      0
    set putPath   0
    set tryMax    5
    set msg       ""
    set update    0

    itfParseArgv labelsWrite $args [list [
    list <spkIDfile>   string  {} file     {}   {file of speaker IDs} ] [
    list -path         string  {} path     {}   {name of path}        ] [
    list -lbox         string  {} lbox     {}   {name of lbox}        ] [
    list -labelPath    string  {} labels   {}   {path of label files} ] [
    list -update       int     {} update   {}   {only try nonexisting paths} ] [
    list -beam         float   {} beam     {}   {viterbi beam}        ] [
    list -topN         int     {} topN     {}   {topN beam}           ] [
    list -optWord      string  {} optWord  {}   {optional word}       ] [
    list -variants     int     {} variants {}   {variants 0/1}        ] [
    list -putPath      int     {} putPath  {}   {write path into log} ] [
    list -tryMax       int     {} tryMax   {}   {increasing beam}     ] ]

    set oldBeam $beam

    # --------------------------------------------------------------------
    #  Main Loop
    # --------------------------------------------------------------------
    
    while {[fgets $file spk] >= 0} {
	if {[llength [info command $lbox]] > 0 } { 
	    $lbox clear 
	}
	foreachSegment utt uttDB $spk {
	    writeLog stderr "%TURN: $spk $utt"
	    
	    set       uttInfo  [uttDB uttInfo $spk $utt]
	    makeArray uttArray $uttInfo
	    
	    eval set label $labels

	    if {$update && [file exists $label]} { continue }
	    
	    set beam $oldBeam
	    set tryN 0; set fehler 1
	    while {$fehler && $tryN <$tryMax} {
		set fehler [catch {set score [viterbiUtterance $LSID $spk $utt -path $path -lbox $lbox\
						  -beam $beam -topN $topN\
						  -optWord $optWord -variants $variants]} msg]
		if $fehler {
		    incr tryN
		    if {$tryN <$tryMax} {
			set beam [expr $beam + 200.0]
			writeLog stderr "%TURN: $spk $utt viterbiUtterance failed."
			writeLog stderr "   increasing beam: $beam"
		    }
		}
	    }
	    if $fehler {
		writeLog stderr "%TURN: $spk $utt $msg"
	    } else {
		if {[llength [info command $lbox]] > 0 } { 
		    if $putPath {writeLog stderr "$spk $utt = $score [$lbox:$utt words $hmm]" }
		} else {
		    catch {makeDir $label}
		    if $putPath {writeLog stderr "$spk $utt = $score [$path words $hmm]" }
		    if [catch {$path bsave $label} msg] {
			puts stderr $msg
		    } else {
			puts stderr "$path bsave $label"
		    }
		}
	    }
	}
	if {[llength [info command $lbox]] > 0 } { 
	    catch {makeDir $label}
	    if [catch {$lbox save $label} msg] {
		puts stderr $msg
	    } else {
		puts stderr "$lbox save $label"
	    }
	}
    }
}

# ------------------------------------------------------------------------
#  labelsMLAdaptWrite   writes maximum likelihood adapted labels. For each
#                       speaker we do a maximum likelihood adaptation
#                       first and then write labels based on a second
#                       viterbi pass using the adapted system.
# ------------------------------------------------------------------------

proc labelsMLAdaptWrite { LSID args} {
    upvar \#0 $LSID SID

    set lbox      lbox$LSID
    set path      path$LSID
    set hmm       hmm$LSID
    set labels    "labels/\$spk.lbox.gz"
    set optWord  \$
    set variants  0
    set beam      300
    set topN      0
    set minCount  1000
    set putPath   0
    set tryMax    5
    set msg       ""
    set update    0
    
    if [info exist SID(codebookSetParam)] { 
	set codebookSetParam $SID(codebookSetParam)
    }

    itfParseArgv labelsMLAdaptWrite $args [list [
    list <spkIDfile>   string  {} file     {}      {file of speaker IDs} ] [
    list <MLAdapt>     object  {} mla      MLAdapt {ML adaptation object}] [
    list -path         string  {} path     {}      {name of path}        ] [
    list -lbox         string  {} lbox     {}      {name of lbox}        ] [
    list -labelPath    string  {} labels   {}      {path of label files} ] [
    list -update       int     {} update   {}      {only try nonexisting paths} ] [
    list -beam         float   {} beam     {}      {viterbi beam}        ] [
    list -topN         int     {} topN     {}      {topN beam}           ] [
    list -optWord      string  {} optWord  {}      {optional word}       ] [
    list -variants     int     {} variants {}      {variants 0/1}        ] [
    list -minCount     float   {} minCount {}      {adaptation minCount} ] [
    list -putPath      int     {} putPath  {}      {write path into log} ] [
    list -tryMax       int     {} tryMax   {}      {increasing beam}     ] ]

    $mla store

    set oldBeam $beam

    # --------------------------------------------------------------------
    #  Main Loop
    # --------------------------------------------------------------------
    
    while {[fgets $file spk] >= 0} {      
	writeLog stderr "$mla restore"
	$mla restore      
	codebookSet$LSID clearAccus
	distribSet$LSID  clearAccus
	
	set spk_bup $spk

	if {[llength [info command $lbox]] > 0 } { 
	    $lbox clear 
	}
	foreachSegment utt uttDB $spk {
	    
	    writeLog stderr "%TURN: $spk $utt"
	
	    set beam $oldBeam
	    set tryN 0; set fehler 1
	    while {$fehler && $tryN <$tryMax} {
		set fehler [catch {viterbiUtterance $LSID $spk $utt -path $path -lbox $lbox \
				       -beam $beam -topN $topN\
				       -optWord $optWord -variants $variants} msg]
		if $fehler {
		    set beam [expr $beam + 200.0]
		    incr tryN
		    writeLog stderr "%TURN: $spk $utt viterbiUtterance failed."
		    writeLog stderr "   increasing beam: $beam"
		}
	    }
	    if $fehler {
		writeLog stderr "%TURN: $spk $utt $msg"
	    } else {
		if {[llength [info command $lbox]] > 0 } { 
		    writeLog stderr "senoneSet$LSID accu $lbox:$utt [senoneSet$LSID accu $lbox:$utt]"
		} else {
		    writeLog stderr "senoneSet$LSID accu $path [senoneSet$LSID accu $path]"
		}
	    }
	}
	writeLog stderr ""
	writeLog stderr "$mla update -minCount $minCount : [$mla update -minCount $minCount]"
	writeLog stderr ""
	
	set spk $spk_bup
	if {[llength [info command $lbox]] > 0 } { 
	    $lbox clear 
	}
	foreachSegment utt uttDB $spk {
	    set       uttInfo  [uttDB uttInfo $spk $utt]
	    makeArray uttArray $uttInfo
	    
	    writeLog stderr "%TURN: $spk $utt"
	    
	    eval set label $labels

	    if {$update && [file exists $label]} { continue }
	    
	    set beam $oldBeam
	    set tryN 0; set fehler 1
	    while {$fehler && $tryN <$tryMax} {
		set fehler [catch {set score [viterbiUtterance $LSID $spk $utt -path $path -lbox $lbox\
						  -beam $beam -topN $topN\
						  -optWord $optWord -variants $variants]} msg]
		if $fehler {
		    incr tryN
		    if {$tryN < $tryMax} {
			set beam [expr $beam + 200.0]
			writeLog stderr "%TURN: $spk $utt viterbiUtterance failed."
			writeLog stderr "   increasing beam: $beam"
		    }
		}
	    }
	    if $fehler {
		writeLog stderr "%TURN: $spk $utt $msg" 
	    } else {
		if {[llength [info command $lbox]] > 0 } { 
		    if $putPath {writeLog stderr "$spk $utt = $score [$lbox:$utt words $hmm]" }
		} else {
		    catch {makeDir $label}
		    if $putPath {writeLog stderr "$spk $utt = $score [$path words $hmm]" }
		    if [catch {$path bsave $label} msg] {
			writeLog stderr $msg
		    } else {
			writeLog stderr "$path bsave $label"
		    }
		}
	    }
	}
	if {[llength [info command $lbox]] > 0 } { 
	    catch {makeDir $label}
	    if [catch {$lbox save $label} msg] {
		writeLog stderr $msg
	    } else {
		writeLog stderr "$lbox save $label"
	    }
	}
    }
}



