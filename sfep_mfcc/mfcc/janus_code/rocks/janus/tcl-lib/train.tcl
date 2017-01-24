# ========================================================================
#  JANUS-SR   Janus Speech Recognition Toolkit
#             ------------------------------------------------------------
#             Object: Standard Training Procedure
#             ------------------------------------------------------------
#
#  Author  :  Michael Finke
#  Module  :  train.tcl
#  Date    :  06/10/96
#
#  Remarks :  
#  $Log$
#  Revision 1.9  2003/08/22 09:10:23  metze
#  utt names used in lbox now can contain '.'
#
#  Revision 1.8  2003/08/14 11:20:31  fuegen
#  Merged changes on branch jtk-01-01-15-fms (jaguar -> ibis-013)
#
#  Revision 1.7.48.8  2003/08/12 16:12:38  metze
#  Cleanup for P013
#
#  Revision 1.7.48.7  2003/06/12 15:54:08  metze
#  INFOs and several TEXT fields in DBase added
#
#  Revision 1.7.48.6  2003/06/06 09:43:59  metze
#  Added latview, fixed upvar, updated tclIndex
#
#  Revision 1.7.48.5  2002/11/27 12:44:23  soltau
#  labelUtterance : fixed makeUtt access
#
#  Revision 1.7.48.4  2002/11/15 13:54:08  metze
#  Text doesn't get overwritten from dbase in labelUtt all the time
#
#  Revision 1.7.48.3  2002/11/04 15:03:58  metze
#  Added documentation code
#
#  Revision 1.7.48.2  2002/10/17 12:08:39  soltau
#  support for labelboxes
#
#  Revision 1.7.48.1  2002/09/02 13:28:53  metze
#  Added -text option to fwdBwdUtterance
#
#  Revision 1.7  2000/09/14 17:47:13  janus
#  Merging branch jtk-00-09-14-jmcd.
#
#  Revision 1.6.14.1  2000/09/14 16:20:56  janus
#  Merged branches jtk-00-09-10-tschaaf, jtk-00-09-12-hyu, and jtk-00-09-12-metze.
#
#  Revision 1.6.2.1  2000/09/14 12:13:08  janus
#  added interfaces to cleanup of backpointer table using viterbi
#
# Revision 1.6  2000/09/10  12:18:50  hyu
# Added Tcl files from Hagen
# 
#  Revision 1.5  2000/04/18 09:59:23  soltau
#  Added modality training procedures
#
#  Revision 1.4  1999/12/10 09:16:30  soltau
#  Added trl flag
#  Made SID array really global
#
#  Revision 1.3  1999/10/13 11:51:59  soltau
#  labelUtterance return lscore
#
#
# ========================================================================


proc train_Info { name } {
    switch $name {
	procs {
	    return "trainInit viterbiUtterance fwdBwdUtterance labelUtterance pathWriteCTM"
	}
	help {
	    return "This file contains various procedures helpful 
during recognizer development. Once initialized with 'trainInit \$SID',
the training environment provides path, hmm and other objects along with
a number of Tcl-defined methods."
	}
	trainInit {
	    return "Initializes the standard JRTk training environment."
	}
	viterbiUtterance {
	    return "Performs viterbi alignment of an utterance. The
necessary information can be read from the database."
	}
	fwdBwdUtterance {
	    return "Performs forward-backward alignment of an utterance. The
necessary information can be read from the database."
	}
	labelUtterance {
	    return "Reads a binary dumped path into the path\$SID
                    structure and translates the senone indices by
                    referring to the utterance HMM and using the path
                    state indices to find the new senone indices therein."
	}
	pathWriteCTM {
	    return "Writes a CTM-format hypothesis file from a path object."
	}
    }
    return "???"
}


# ------------------------------------------------------------------------
#  trainInit    initialize training setup including acoustic models,
#               a hidden markov model and a path object.
# ------------------------------------------------------------------------
global LBOX_SPEAKER

proc trainInit { LSID args } {
  upvar \#0 $LSID SID

  set hmm         hmm$LSID
  set path        path$LSID
  set lbox        ""
  set ttree       ttree$LSID
  set ttreeRoot   ""
  set amodelSet   amodelSet$LSID
  set durTree     durTree$LSID
  set durTreeRoot ROOT
  set dict        dict$LSID

  if {[info exists SID(rcmSdp)]} {set rcmSdp $SID(rcmSdp)} else {set rcmSdp 0}

  itfParseArgv trainInit $args [list [
    list -amodelSet    string  {} amodelSet   {}   {acoustic models}       ] [
    list -hmm          string  {} hmm         {}   {hidden markov model}   ] [
    list -path         string  {} path        {}   {path object}           ] [
    list -lbox         string  {} lbox        {}   {Labelbox object}       ] [
    list -topoTree     object  {} ttree       Tree {topology tree}         ] [
    list -topoTreeRoot string  {} ttreeRoot   {}   {root of topoTree}      ] [
    list -durTree      object  {} durTree     Tree {duration tree}         ] [
    list -durTreeRoot  string  {} durTreeRoot {}   {root of duration tree} ] [
    list -rcmSdp       int     {} rcmSdp      {}   {use right context for context-dependent single phone words} ] [
    list -dict         object  {} dict        Dictionary {dictionary}      ] ]

  if { [llength [info command $ttree]] &&
       [llength [info command $amodelSet]] < 1} {

    if {![string length $ttreeRoot]} { 
      set ttreeRoot [$ttree.item(0) configure -name]
    }
    if { [llength [info command $durTree]] } {
             AModelSet $amodelSet $ttree $ttreeRoot\
                      -durationTree $durTree\
                      -durationRoot $durTreeRoot
    } else { AModelSet $amodelSet $ttree $ttreeRoot}
  }

  if { [string length $hmm] &&
       [llength [info command $dict]] &&
       [llength [info command $amodelSet]] &&
       [llength [info command $hmm]] < 1} {

     HMM $hmm $dict $amodelSet
     catch { $hmm configure -rcmSdp $rcmSdp }
  }

  if { [string length $path] &&
       [llength [info command $path]] < 1} {
     Path $path
  }
  if { [string length $lbox] &&
       [llength [info command $lbox]] < 1} {
     Labelbox $lbox
  }

}


# ------------------------------------------------------------------------
#  hmmModMakeUtterance (modality based)
# ------------------------------------------------------------------------

proc hmmModMakeUtterance { hmm LSID args } {
    upvar \#0 $LSID SID

    set optWord     \$
    set variants    0
    set modalitySet modalitySet$LSID
    set distribTree distribTree$LSID
    set amodelSet   amodelSet$LSID
    set senoneSet   senoneSet$LSID

    if [info exist SID(TRL)] { set trl $SID(TRL) } else { set trl TEXT }

    itfParseArgv hmmModMakeUtterance $args [list [
      list <speaker>     string  {} spk         {} {speaker   ID}          ] [
      list <uttID>       string  {} utt         {} {utterance ID}          ] [
      list -text         string  {} text        {} {text to align}         ] [
      list -modalitySet  string  {} modalitySet {} {name of ModalitySet}   ] [
      list -distribTree  string  {} distribTree {} {name of DistribTree}   ] [
      list -amodelSet    string  {} amodelSet   {} {name of AmodelSet}     ] [
      list -senoneSet    string  {} senoneSet   {} {name of SenoneSet}     ] [
      list -textTag      string  {} TEXT        {} {text tag in uttInfo}   ] [
      list -frameN       int     {} frameN      {} {number of frames}      ] [
      list -optWord      string  {} optWord     {} {optional word}         ] [
      list -variants     int     {} variants    {} {variants 0/1}          ] ]

    set uttInfo [uttDB uttInfo $spk $utt]
    makeArray uttArray $uttInfo

    if { ![info exists text] } { set text $uttArray($trl) }

    if [catch {$modalitySet updateUtterance $LSID $spk $utt} msg] {
        error "ERROR $modalitySet updateUtterance $LSID $spk $utt\n$msg"
    }

    # quick hack (tags are over the whole sentence equal majorityAnswer)
    PhoneGraph configure -modTags [$modalitySet answer2codedTags [$modalitySet majorityAnswer]]

    if [catch {$hmm make $text -optWord $optWord -variants $variants} msg] {
        error "ERROR 1. $hmm make $text -optWord $optWord -variants $variants\n$msg"
    }

    unset uttInfo
    unset uttArray

    return $text
}


HMM method modMakeUtterance hmmModMakeUtterance \
    -text "create utterance HMM with modalities"


# ------------------------------------------------------------------------
#  modViterbiUtterance  (modality based)
# ------------------------------------------------------------------------

proc modViterbiUtterance { LSID args } {
    upvar \#0 $LSID SID

    set optWord     \$
    set variants    0
    set hmm         hmm$LSID
    set modalitySet modalitySet$LSID
    set distribTree distribTree$LSID
    set amodelSet   amodelSet$LSID
    set senoneSet   senoneSet$LSID
    set path        path$LSID
    set lbox        lbox$LSID

    if [info exist SID(TRL)] { set trl $SID(TRL) } else { set trl TEXT }

    # default: dont use cleanup of backpointer during viterbi 
    set bpMod       -1
    set bpMul        3

    set beam        300
    set topN        0

    itfParseArgv modViterbiUtterance $args [list [
      list <speaker>     string  {} spk         {} {speaker   ID}          ] [
      list <uttID>       string  {} utt         {} {utterance ID}          ] [
      list -text         string  {} text        {} {text to align}         ] [
      list -hmm          string  {} hmm         {} {name of hmm}           ] [
      list -modalitySet  string  {} modalitySet {} {name of modalitySet}   ] [
      list -distribTree  string  {} distribTree {} {name of distribTree}   ] [
      list -amodelSet    string  {} amodelSet   {} {name of amodelSet}     ] [
      list -senoneSet    string  {} senoneSet   {} {name of senoneSet}     ] [
      list -path         string  {} path        {} {name of path}          ] [
      list -lbox         string  {} lbox        {} {Labelbox object}       ] [
      list -textTag      string  {} TEXT        {} {text tag in uttInfo}   ] [
      list -beam         string  {} beam        {} {viterbi beam}          ] [
      list -topN         string  {} topN        {} {topN beam}             ] [
      list -optWord      string  {} optWord     {} {optional word}         ] [
      list -variants     int     {} variants    {} {variants 0/1}          ] [
      list -bpMod        int     {} bpMod       {} {after every X frames clean up bpTable (<0 never)}      ] [
      list -bpMul        int     {} bpMul       {} {go Y * X frames back during cleanup (<1 start at first frame)} ] [
      list -log          string  {} log         {} {name of log channel} ] ]

    set uttInfo [uttDB uttInfo $spk $utt]
    makeArray uttArray $uttInfo

    if { ![info exists text] } { set text $uttArray($trl) }
    
    if [catch {$hmm modMakeUtterance $LSID $spk $utt -text $text \
                   -optWord $optWord -variants $variants } msg] {
        error "ERROR $hmm modMakeUtterance $LSID $spk $utt -text $text -optWord $optWord\
              -variants $variants\n$msg"
    }

    if { [llength [info command $lbox]] > 0} {
       if { [$lbox.list index $utt] < 0} {
          $lbox add $utt
       }
       set path $lbox.item([$lbox.list index $utt]) 
    }
    if [catch {$path viterbi hmm$LSID -eval $uttInfo -beam $beam \
                   -topN $topN -bpMod $bpMod -bpMul $bpMul} score] {
        error "ERROR $utt: $path viterbi hmm$LSID -beam $beam -topN $topN\n$score"
    }

    unset uttInfo
    unset uttArray

    # This function can be used to filter unreliable frames
    if {[llength [info command filterPath]]} {
        filterPath $spk $utt $path
        #writeLog stderr "$spk $utt filterPath <= [filterPath $spk $utt $path] =>"
    }

    return $score
}


# ------------------------------------------------------------------------
#  modLabelUtterance  (modality based)
# ------------------------------------------------------------------------

proc modLabelUtterance { LSID args } {
    global LBOX_SPEAKER
    upvar \#0 $LSID SID

    set optWord     \$
    set variants    0
    set hmm         hmm$LSID
    set modalitySet modalitySet$LSID
    set distribTree distribTree$LSID
    set amodelSet   amodelSet$LSID
    set senoneSet   senoneSet$LSID
    set lbox        lbox$LSID
    set path        path$LSID

    if [info exist SID(TRL)] { set trl $SID(TRL) } else { set trl TEXT }

    set eval       ""
    set evalFES    1
    set featureSet featureSet$LSID

    itfParseArgv modLabelUtterance $args [list [
      list <speaker>     string  {} spk         {} {speaker   ID}          ] [
      list <uttID>       string  {} utt         {} {utterance ID}          ] [
      list <file>        string  {} file        {} {filename}              ] [
      list -text         string  {} text        {} {text to align}         ] [
      list -eval         string  {} eval        {} {eval string extension} ] [
      list -evalFES      int     {} evalFES     {} {eval feature set 0/1}  ] [
      list -hmm          string  {} hmm         {} {name of hmm}           ] [
      list -modalitySet  string  {} modalitySet {} {name of modalitySet}   ] [
      list -distribTree  string  {} distribTree {} {name of distribTree}   ] [
      list -amodelSet    string  {} amodelSet   {} {name of amodelSet}     ] [
      list -senoneSet    string  {} senoneSet   {} {name of senoneSet}     ] [
      list -path         string  {} path        {} {name of path}          ] [
      list -lbox         string  {} lbox        {} {name of lbox}          ] [
      list -featureSet   string  {} featureSet  {} {feature set}           ] [
      list -textTag      string  {} TEXT        {} {text tag in uttInfo}   ] [
      list -optWord      string  {} optWord     {} {optional word}         ] [
      list -variants     int     {} variants    {} {variants 0/1}          ] [
      list -log          string  {} log         {} {name of log channel} ] ]

    set uttInfo [uttDB uttInfo $spk $utt]
    makeArray uttArray $uttInfo

    if { ![info exists text] } { set text $uttArray($trl) }

    if [catch {$hmm modMakeUtterance $LSID $spk $utt -text $text \
                   -optWord $optWord -variants $variants } msg] {
        error "ERROR $hmm modMakeUtterance $LSID $spk $utt -text $text -optWord $optWord \
              -variants $variants\n$msg"
    }
    if { $evalFES } {
        if [catch {$featureSet eval "$uttInfo $eval"} msg] {
            error "$featureSet eval $uttInfo\m$msg"
        }
    }
    eval set file $file
    if {![file exists "$file"]} {
       error "labelbox file '$file' doesn't exist.\n"
    }
    if { [llength [info command $lbox]] < 1} {
      if [catch {$path bload $file -hmm $hmm} score] {
         error "$utt: bload $file -hmm $hmm\n$score"
      } 
    } else {
      if {! [info exists LBOX_SPEAKER] || $spk !=  ${LBOX_SPEAKER}} {
        $lbox clear
        if [catch {printDo $lbox load $file}] {
           error "$spk: $lbox load $file\n"
        }
        set LBOX_SPEAKER $spk
      }
      set path $lbox.item([$lbox.list index $utt]) 
      if [catch {$path map $hmm} score] {
         error "$utt: $lbox:$utt map $hmm\n$score"
      }
    }
    unset uttInfo
    unset uttArray

    # This function can be used to filter unreliable frames
    if {[llength [info command filterPath]]} {
        filterPath $spk $utt $path
        #writeLog stderr "$spk $utt filterPath <= [filterPath $spk $utt $path] =>"
    }

    return $score
}


# ------------------------------------------------------------------------
#  Create HMM from Utterance
# ------------------------------------------------------------------------

proc hmmMakeUtterance { hmm args } {
 
  set optWord   \$
  set variants   0

  itfParseArgv hmmMakeUtterance $args [list [
    list <text>    string  {} text     {} {transcription}         ] [
    list -optWord  string  {} optWord  {} {optional word}         ] [
    list -variants int     {} variants {} {variants 0/1}          ] ]

  if [catch {$hmm make $text -optWord $optWord -variants $variants} msg] {
      error "ERROR $hmm make $args\n$msg"
  }
  return $text
}

HMM method makeUtterance hmmMakeUtterance -text "create utterance HMM"


# ------------------------------------------------------------------------
#  viterbiUtterance
# ------------------------------------------------------------------------

proc viterbiUtterance { LSID args } {
    upvar \#0 $LSID SID
    global USE_MODALITY
    
    if { [info exists USE_MODALITY] && ${USE_MODALITY} == 1} { 
	set score [eval "modViterbiUtterance $LSID $args"]
	return $score
    }
    
    set hmm         hmm$LSID
    set path        path$LSID
    set lbox        lbox$LSID

    set optWord  \$
    set variants  0
    set beam      300
    set topN      0

    if [info exist SID(TRL)] { set trl $SID(TRL) } else { set trl TEXT }

    # default: dont use cleanup of backpointer during viterbi 
    set bpMod       -1
    set bpMul        3

    itfParseArgv viterbiUtterance $args [list [
      list <speaker> string  {} spk      {} {speaker   ID}  ] [
      list <uttID>   string  {} utt      {} {utterance ID}  ] [
      list -text     string  {} text     {} {text to align} ] [
      list -hmm      string  {} hmm      {} {name of hmm}   ] [
      list -path     string  {} path     {} {name of path}     ] [
      list -lbox     string  {} lbox     {} {name of Labelbox} ] [
      list -beam     string  {} beam     {} {viterbi beam}  ] [
      list -topN     string  {} topN     {} {topN beam}     ] [
      list -bpMod    int     {} bpMod    {} {after every X frames clean up bpTable (<0 never)}      ] [
      list -bpMul    int     {} bpMul    {} {go Y * X frames back during cleanup (<1 start at first frame)} ] [
      list -optWord  string  {} optWord  {} {optional word} ] [
      list -variants int     {} variants {} {variants 0/1}  ] ]

    set       uttInfo  [uttDB uttInfo $spk $utt]
    makeArray uttArray $uttInfo

    if {! [info exist text]} { set text $uttArray($trl) }
    
    if [catch {$hmm makeUtterance $text -optWord  $optWord\
		   -variants $variants} msg] {
	error "ERROR $hmm makeUtterance $text\n$msg"
    }
    if { [llength [info command $lbox]] > 0} {
	if { [$lbox.list index $utt] < 0} {
	    $lbox add $utt
	}
	set path $lbox.item([$lbox.list index $utt]) 
    }
    if [catch {$path viterbi $hmm -eval $uttInfo -beam $beam\
		   -topN $topN -bpMod $bpMod -bpMul $bpMul} score] {
	error "ERROR $utt: $path viterbi $hmm -beam $beam -topN $topN\n$score"
    }

    # This function can be used to filter unreliable frames
    if {[llength [info command filterPath]]} {
        filterPath $spk $utt $path
        #writeLog stderr "$spk $utt filterPath <= [filterPath $spk $utt $path] =>"
    }

    return $score
}


# ------------------------------------------------------------------------
#  fwdBwdUtterance   forward backward alignment of an utterance
# ------------------------------------------------------------------------

proc fwdBwdUtterance { LSID args } {

    upvar \#0 $LSID SID
    set hmm      hmm$LSID
    set lbox     lbox$LSID
    set path     path$LSID   
    set optWord  \$
    set variants 0
    set width    40
    set topN     100

    if [info exist SID(TRL)] { set trl $SID(TRL) } else { set trl TEXT }

    itfParseArgv fwdBwdUtterance $args [list [
    list <speaker> string  {} spk      {} {speaker   ID}  ] [
    list <uttID>   string  {} utt      {} {utterance ID}  ] [
    list -text     string  {} text     {} {text to align} ] [
    list -hmm      string  {} hmm      {} {hmm}           ] [
    list -path     string  {} path     {} {path}          ] [
    list -lbox     string  {} lbox     {} {name of lbox}  ] [
    list -topN     string  {} topN     {} {topN beam}     ] [
    list -width    string  {} width    {} {width of path} ] [
    list -optWord  string  {} optWord  {} {optional word} ] [
    list -variants int     {} variants {} {variants 0/1}  ] ]

    set       uttInfo  [uttDB uttInfo $spk $utt]
    makeArray uttArray $uttInfo
    
    if {! [info exist text]} { set text $uttArray($trl) }
    puts "\n%TURN: $spk $utt"

    if [catch {$hmm makeUtterance $text -optWord  $optWord -variants $variants} msg] {
	error "ERROR $hmm makeUtterance $text\n$msg"
    }
    if { [llength [info command $lbox]] > 0} {
	if { [$lbox.list index $utt] < 0} {
	    $lbox add $utt
	}
	set path $lbox:$utt 
    }
    if [catch {$path fwdBwd hmm$LSID -eval $uttInfo -width $width \
		   -topN $topN} score] {
	error "ERROR $utt: $path fwdBwd hmm$LSID -width $width -topN $topN\n$score"
    }
    puts "$text (score $score)"

    # This function can be used to filter unreliable frames
    if {[llength [info command filterPath]]} {
        filterPath $spk $utt $path
        #writeLog stderr "$spk $utt filterPath <= [filterPath $spk $utt $path] =>"
    }

    return $score
}


# ------------------------------------------------------------------------
#  labelUtterance    reads a binary dumped path into the path$SID
#                    structure and translates the senone indices by
#                    referring to the utterance HMM and using the path
#                    state indices to find the new senone indices therein.
# ------------------------------------------------------------------------

proc labelUtterance { LSID args } {
    global LBOX_SPEAKER USE_MODALITY

    if { [info exists USE_MODALITY] && ${USE_MODALITY} == 1} { 
	set score [eval "modLabelUtterance $LSID $args"]
	return $score
    }

    upvar \#0 $LSID SID

    set optWord   \$
    set variants   0
    set log        stderr
    set evalS      ""
    set evalFES    1
    set featureSet featureSet$LSID
    set hmm        hmm$LSID
    set lbox       lbox$LSID
    set path       path$LSID
    set evalScore  0
    if {[info exist SID(TRL)]}  { set trl $SID(TRL) } else { set trl TEXT }
    if {[info exist SID(LBOX)]} { set lbx $SID(LBOX)} else { set lbx {$spk} }

    itfParseArgv labelUtterance $args [list [
    list <speaker>   string  {} spk        {} {speaker   ID}          ] [
    list <uttID>     string  {} utt        {} {utterance ID}          ] [
    list <file>      string  {} file       {} {filename}              ] [
    list -text       string  {} text       {} {text to align}         ] [
    list -optWord    string  {} optWord    {} {optional word}         ] [
    list -variants   int     {} variants   {} {variants 0/1}          ] [
    list -eval       string  {} evalS      {} {eval string extension} ] [
    list -evalFES    int     {} evalFES    {} {eval feature set 0/1}  ] [
    list -featureSet string  {} featureSet {} {feature set}           ] [
    list -hmm        string  {} hmm        {} {hmm}                   ] [
    list -path       string  {} path       {} {path}                  ] [
    list -lbox       string  {} lbox       {} {name of lbox}          ] [
    list -evalScore  int     {} evalScore  {} {compute path score}    ] [
    list -log        string  {} log        {} {name of log channel}   ] ]

    set       uttInfo  [uttDB uttInfo $spk $utt]
    makeArray uttArray $uttInfo

    # Get text
    if {![info exist text]} {
	set text $uttArray($trl)
	if {0} {
	    # Some systems may rely on this code
	    foreach tmptrl $trl {if {[info exists uttArray($tmptrl)]} { break } }
	    if {[llength [lindex $uttArray($tmptrl) 0]] == 1} {
		set text $uttArray($tmptrl)
	    } else {
		set text ""
		foreach w $uttArray($tmptrl) {
		    regsub {\([^\)]*\)$} [lindex $w 0] "" w
		    set text "$text $w"
		}
		puts "Using special TRL '$tmptrl' -> '$text'."
	    }
	}
    }

    # Make HMM
    if {[catch {$hmm makeUtterance $text -optWord $optWord \
		    -variants $variants} msg]} {
	error "$hmm makeUtterance $text\n$msg"
    }

    # Eval feature set
    if { $evalFES } {
	if [catch {$featureSet eval "$uttInfo $evalS"} msg] {
	    error "$featureSet eval $uttInfo\n$msg"
	}
    }

    # Load label (box)
    eval set file $file
    if {[llength [info command $lbox]] < 1} {
	if {[regexp "\.lbox" $file]} { puts "INFO: Really load *.lbox file into Label object?"}
	if [catch {$path bload $file -hmm $hmm} score] {
	    error "$utt: bload $file -hmm $hmm\n$score"
	} 
    } else {
	if {[regexp "\.lbl" $file]} { puts "INFO: Really load *.lbl file into LBox object?" }
	if {![info exists LBOX_SPEAKER] || [eval set LBS $lbx] != ${LBOX_SPEAKER}} {
	    $lbox clear
	    if {[catch {printDo $lbox load $file}]} {
		error "$spk: $lbox load $file\n"
	    }
	    eval set LBOX_SPEAKER $lbx
	}
	set path $lbox.item([$lbox.list index $utt]) 
	if {[catch {$path map $hmm} score]} {
	    error "$utt: $lbox:$utt map $hmm\n$score"
	}
    }
    if {$evalScore} { set score [$path lscore $hmm] }

    # This function can be used to filter unreliable frames
    if {[llength [info command filterPath]]} {
        filterPath $spk $utt $path
        #writeLog stderr "$spk $utt filterPath <= [filterPath $spk $utt $path] =>"
    }

    return $score
}


# ------------------------------------------------------------------------
#  pathWriteCTM
# ------------------------------------------------------------------------

proc pathWriteCTM { LSID args } {

  upvar \#0 $LSID SID
  set    file stdout

  itfParseArgv pathWriteCTM $args [list [
    list <speaker> string  {} spk  {} {speaker   ID}  ] [
    list <uttID>   string  {} utt  {} {utterance ID}  ] [
    list <from>    int     {} from {} {from frame}    ] [
    list -file     string  {} file {} {filename}      ] ]

  regexp {(....)-([AB])} $spk dummy conv channel

  if { $file != "stdout" } { set FP [open $file a] } else { set FP stdout }

  puts $FP "# $utt $from"

  set frN [expr [path$LSID configure -lastFrame] + 1 - \
                [path$LSID configure -firstFrame]]

  set wordX    0
  set word    "("
  set beg      0

  for {set fr 1} {$fr < $frN} {incr fr} {
    set wX [path$LSID.itemList($fr).item(0) configure -wordX]
    if { $wX != $wordX } {
      set end   [expr $fr-1]
      set score 0
      set len   [expr  ($end + 1 - $beg) / 100.0]
      set beg   [expr  ($beg / 100.0) + $from]
      puts $FP  [format "%4d %s %7.2f %7.2f %-20s %7.2f" $conv $channel \
                                               $beg $len $word $score]

      set beg   $fr
      set wordX $wX
      set word  [lindex [lindex [hmm$LSID.wordGraph] 0] $wX]
    }
  }
  set end   [expr $frN-1]
  set score 0
  set len   [expr  ($end + 1 - $beg) / 100.0]
  set beg   [expr  ($beg / 100.0) + $from]
  puts $FP  [format "%4d %s %7.2f %7.2f %-20s %7.2f" $conv $channel \
                                            $beg $len $word $score]

  if { $file != "stdout" } { close $FP }
  return
}


# ------------------------------------------------------------------------
#  viterbiLattice
# ------------------------------------------------------------------------

proc viterbiLattice { LSID args } {

  upvar \#0 $LSID SID

  set variants  0
  set beam      300
  set topN      0
  set lz        16
  set lp        0
  set optPen    5

  # default: dont use cleanup of backpointer during viterbi 
  set bpMod       -1
  set bpMul        3

  if [info exist SID(lmWeightList)]  { set lz  $SID(lmWeightList)  }
  if [info exist SID(lmPenaltyList)] { set lp $SID(lmPenaltyList) }

  itfParseArgv viterbiUtterance $args [list [
    list <speaker> string  {} spk      {}        {speaker   ID}     ] [
    list <uttID>   string  {} utt      {}        {utterance ID}     ] [
    list <lattice> object  {} lattice  {Lattice} {lattice to align} ] [
    list -z        float   {} lz       {}        {LM weight}        ] [
    list -p        float   {} lp       {}        {LM penalty}       ] [
    list -beam     string  {} beam     {}        {viterbi beam}     ] [
    list -topN     string  {} topN     {}        {topN beam}        ] [
    list -bpMod    int     {} bpMod    {} {after every X frames clean up bpTable (<0 never)}      ] [
    list -bpMul    int     {} bpMul    {} {go Y * X frames back during cleanup (<1 start at first frame)}      ] [
    list -optPen   float   {} optPen   {}        {optWord penalty}  ] [
    list -variants int     {} variants {}        {variants 0/1}     ] ]

  set       uttInfo  [uttDB uttInfo $spk $utt]
  makeArray uttArray $uttInfo

  puts "\n%TURN: $spk $utt ($uttArray(FROM) $uttArray(TO))"
  if [catch {set hmmLst [$lattice hmm -lz $lz -lp $lp \
                                      -silenceWordPenalty $optPen]} msg] {
      error "ERROR $lattice hmm\n$msg"
  }
  if [catch {hmm${LSID} make  [lindex $hmmLst 0] -trans [lindex $hmmLst 1]\
                        -init [lindex $hmmLst 2] -variants $variants} msg] {
      error "ERROR hmm${LSID} makeUtterance $text\n$msg"
  }
  if [catch {path$LSID viterbi hmm$LSID -eval $uttInfo -beam $beam\
                                                       -topN $topN -bpMod $bpMod -bpMul $bpMul} score] {
    if [catch {path$LSID viterbi hmm$LSID -eval $uttInfo -beam [expr 2*$beam] \
                                          -topN [expr 2*$topN] -bpMod $bpMod -bpMul $bpMul} score] {
      if [catch {path$LSID viterbi hmm$LSID -eval $uttInfo -beam [expr 4*$beam]\
                                            -topN [expr 4*$topN] -bpMod $bpMod -bpMul $bpMul} score] {
        error "ERROR $utt: path$LSID viterbi hmm$LSID -beam [expr 4*$beam]\
                                                      -topN [expr 4*$topN]\n$score"
      }
    }
  }
  return $score
}


# ------------------------------------------------------------------------
#  fwdBwdLattice
# ------------------------------------------------------------------------

proc fwdBwdLattice { LSID args } {

  upvar \#0 $LSID SID

  set variants  0
  set width     20
  set topN      0
  set lz        16
  set lp        0
  set optPen    5

  if [info exist SID(lmWeightList)]  { set lz $SID(lmWeightList)  }
  if [info exist SID(lmPenaltyList)] { set lp $SID(lmPenaltyList) }

  itfParseArgv fwdBwdUtterance $args [list [
    list <speaker> string  {} spk      {}        {speaker   ID}     ] [
    list <uttID>   string  {} utt      {}        {utterance ID}     ] [
    list <lattice> object  {} lattice  {Lattice} {lattice to align} ] [
    list -z        float   {} lz       {}        {LM weight}        ] [
    list -p        float   {} lp       {}        {LM penalty}       ] [
    list -width    int     {} width    {}        {fwdBwd path width}] [
    list -topN     int     {} topN     {}        {topN}             ] [
    list -optPen   float   {} optPen   {}        {optWord penalty}  ] [
    list -variants int     {} variants {}        {variants 0/1}     ] ]

  set       uttInfo  [uttDB uttInfo $spk $utt]
  makeArray uttArray $uttInfo

  puts "\n%TURN: $spk $utt ($uttArray(FROM) $uttArray(TO))"
  if [catch {set hmmLst [$lattice hmm -lz $lz -lp $lp \
                                      -silenceWordPenalty $optPen]} msg] {
      error "ERROR $lattice hmm\n$msg"
  }
  if [catch {hmm${LSID} make  [lindex $hmmLst 0] -trans [lindex $hmmLst 1]\
                        -init [lindex $hmmLst 2] -variants $variants} msg] {
      error "ERROR hmm${LSID} makeUtterance $text\n$msg"
  }
  if [catch {path$LSID fwdBwd hmm$LSID -eval $uttInfo -width $width\
                                                      -topN $topN} score] {
    if [catch {path$LSID fwdBwd hmm$LSID -eval $uttInfo -width [expr 2*$width] \
                                         -topN [expr 2*$topN]} score] {
      if [catch {path$LSID fwdBwd hmm$LSID -eval $uttInfo -width [expr 4*$width]\
                                           -topN [expr 4*$topN]} score] {
        error "ERROR $utt: path$LSID fwdBwd hmm$LSID -width [expr 4*$width]\
                                                     -topN [expr 4*$topN]\n$score"
      }
    }
  }
  return $score
}


# ------------------------------------------------------------------------
#  check LDA counts vs codebook counts
# ------------------------------------------------------------------------

proc checkLDACounts { LSID args } {
    upvar \#0 $LSID SID

    set dss distribSet$LSID
    if {[info exists SID(featureSetLDAMatrix)]} {
	regsub ".bmat" $SID(featureSetLDAMatrix) ".counts" file
    } else {
	set file lda${LSID}.counts
    }

    itfParseArgv fwdBwdUtterance $args [list [
        list -distribSet string  {} dss      {}        {distribSet}       ] [
        list -file       string  {} file     {}        {LDA counts file}  ] ]

    if {[catch {set fp [open $file r]} msg]} {
	writeLog stderr "checkLDAcounts: couldn't open '$file', counts not checked."
	return 0
    }
    while {[gets $fp line] != -1} {
	set l [lindex $line 0]
	set c([lindex $l 0]) [lindex $l 1]
    }
    close $fp

    set ok [set mismatch [set bad 0]]
    foreach ds [lsort -unique "[${dss}:] [array names c]"] {
	if {[$dss index $ds] == -1 || ![info exists c($ds)]} {
	    incr mismatch
	} else {
	    if {[${dss}:${ds} configure -count] != $c($ds)} {
		incr bad
	    } else {
		incr ok
	    }
	}
    }
    if {$mismatch} {
	writeLog stderr "checkLDAcounts: $mismatch mismatches detected."
    }
    if {$bad} {
	writeLog stderr "checkLDAcounts: $bad bad counts detected ($dss vs $file)."
    }
    writeLog stderr "checkLDAcounts: $ok distribs had good counts."
    return $ok
}
