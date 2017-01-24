# ========================================================================
#  JANUS-SR   Janus Speech Recognition Toolkit
#             ------------------------------------------------------------
#             Object: VTLN
#             ------------------------------------------------------------
#
#  Author  :  Martin Westphal
#  Module  :  warp.tcl
#  Date    :  Fri Feb 14 14:46:33 MET 1997
#
#  Remarks :  Feb 25: Added some procedures from Torsten to find the
#                     best warp factor during test!
#
# $Log: vtln.tcl,v $
# Revision 1.12  2005/07/07 14:05:13  stueker
# frameN and vscore, arrays for accumulated score and frames, are being reset for every new speaker
#
# Revision 1.11  2005/04/18 15:26:18  metze
# Changed output of findLabelWarp (Seastian)
#
# Revision 1.10  2004/10/27 12:13:04  metze
# Beautification
#
# Revision 1.9  2003/08/14 11:20:32  fuegen
# Merged changes on branch jtk-01-01-15-fms (jaguar -> ibis-013)
#
# Revision 1.8.50.5  2003/03/14 16:25:23  metze
# Cosmetic
#
# Revision 1.8.50.4  2002/11/27 12:46:41  soltau
# support for labelboxes
#
# Revision 1.8.50.3  2002/11/15 13:54:26  metze
# Nicer output, while we're at it
#
# Revision 1.8.50.2  2002/11/04 15:03:58  metze
# Added documentation code
#
# Revision 1.8.50.1  2002/10/17 11:45:34  soltau
# findLabelWarp   : re-set cwarp for each utterance
# findViterbiWarp : re-set cwarp for each utterance
#
# Revision 1.8  2000/09/10 12:18:53  hyu
# Added Tcl files from Hagen
#
# Revision 1.7  2000/03/06 18:17:29  soltau
# fixed bug
#
# Revision 1.6  2000/01/12 17:28:07  soltau
# Fixed a couple of problems (made vtln working !)
#
# Revision 1.5  1999/12/10 09:30:57  soltau
# Added trl flag
#
# Revision 1.4  1999/10/13 12:00:11  soltau
# warp on voiced phones
# made cwarp argument safe
#
# Revision 1.3  1999/10/07 12:57:33  soltau
# Changed cwarp argument to string format
#
# Revision 1.2  1999/03/30 09:00:25  soltau
# fixed makeScaleList to get the maximum too
# (makeScaleList 1.0 12 0.02 didn't return 1.240)
#
#
# ========================================================================


proc vtln_Info { name } {
    switch $name {
	procs {
	    return "vtlnInit findLabelWarp findViterbiWarp"
	}
	help {
	    return "Procedure to handle VTLN (Vocal Tract Length Normalization)
estimation and use."
	}
	vtlnInit {
	    return "Reads in a file containing warp factors 
(there is no procedure to write them, though; the file format is '<spk> <scale>'
in every line)."
	}
	findLabelWarp {
	    return "Given a path (in a label file) rescore all utterances
                  of the given speaker within a window of different
                  warp scales. Utterances of the speaker are taken until 
                  a maximum number of frames is reached.
                  Return warp factor with best score and frames used."
	}
	findViterbiWarp {
	    return "Find the best warp factor within a given window around 
                  an initial value. Use a first hypothesis given in 
                  \$HYPO(\$utt) and do viterbi. Rescore for different warp
                  scales and all utterances of the speaker.
                  Return warp factor with best score and frames used."
	}
    }
    return "???"
}


set tcl_precision 12

# ------------------------------------------------------------------------
# read warp scales from a file with format:
# <spk> <scale>
# ------------------------------------------------------------------------
proc readWarpScales { file { SID2 "" } } {
  global warpScales SID

    if {![info exist SID2] || ![string length $SID2] } { set SID2 $SID }

  if [file exists $file] {
    writeLog stderr "warpScales [string repeat " " [string length $SID2] ] load $file" 2
    set FI [open $file r]
    while {[gets $FI line] >= 0} {
      if {[llength $line] != 2} {
	error "old warpScale file format"
      }
      set warpScales([lindex $line 0]) [lindex $line 1]
    }
    close $FI
  } else {
    writeLog stderr "no warpfile found"
  }
}

# ------------------------------------------------------------------------
#  vtlnInit   reading the warp scales if given
# ------------------------------------------------------------------------
proc vtlnInit { LSID args } {
  upvar \#0 $LSID SID

  set warpFile ""
  if [info exist SID(warpFile)] { set warpFile $SID(warpFile) }

  itfParseArgv vtlnInit $args [list [
    list -param      string  {} warpFile     {} {file with warp scales} ] ]

  readWarpScales "$warpFile" $LSID
}

# ------------------------------------------------------------------------
#  pathPhoneScores   finds the local acoustic score of all segments
#                    labelled to be one of the phones in <lst>. This
#                    procedure is used for example to find the score
#                    of voiced regions.
#                    If the list is empty return total score.
# ------------------------------------------------------------------------

proc pathPhoneScores {path hmm lst {usegamma 0}} {
    set frameV     0
    set vscore     0 

    if {[llength "$lst"]} {
	foreach seg [$path phones $hmm] {
	    set phone  [lindex $seg 0]
	    # find score for specific phones only
	    if {[lsearch $lst $phone] == -1} { continue }
	    
	    set frameS [lindex $seg 1]
	    set frameE [lindex $seg 2]
	    set lscore [$path lscore $hmm -from $frameS -to $frameE -gamma $usegamma]
	    if {![catch { expr $lscore }]} {
		set frameV [expr $frameV + ($frameE - $frameS) + 1]
		set vscore [expr $vscore + $lscore]
	    }
	}
    } else {
	set frameV [expr 1 + [$path configure -lastFrame] + [$path configure -firstFrame]]
	set vscore [$path lscore $hmm -gamma $usegamma]
    }

    return "$frameV $vscore"
}

# ------------------------------------------------------------------------
#  makeScaleList   Create list around center value
# ------------------------------------------------------------------------

proc makeScaleList {center window delta {format "%.3f"}} {
    set lst ""
    set v [expr $center - $window * $delta]
    set n [expr 2*$window +1]
    for {set i 0} {$i < $n} {incr i} {
	lappend lst [format $format $v]
	set v [expr $v + $delta]
    }
    return "$lst"
}

#------------------------------------------------------------------------
# scoreWarpLst    Given a path score a list of warp scales
#                 and write the scores and frames into an array
#------------------------------------------------------------------------

proc scoreWarpLst {frameO scoreO LSID path hmm uttInfo scaleLst phoneLst {usegamma 0}} {
  global WARPSCALE
  upvar $frameO frameA
  upvar $scoreO scoreA

  foreach scale $scaleLst {
    set WARPSCALE $scale
    if [catch { 
      featureSet$LSID eval $uttInfo
      set frameScore [pathPhoneScores $path $hmm $phoneLst $usegamma]
    } msg] {
      writeLog stderr "ERROR rescoring $scale : $msg"
      continue
    } 
    if [info exist scoreA($scale)] {
      set  scoreA($scale) [expr $scoreA($scale)+[lindex $frameScore 1]]
      set  frameA($scale) [expr $frameA($scale)+[lindex $frameScore 0]]
    } else {
      set  scoreA($scale) [lindex $frameScore 1]
      set  frameA($scale) [lindex $frameScore 0]
    }
  } 
}

#------------------------------------------------------------------------
# findBestWarp    Search for the best (minimum) warp factor.
#                 Do smoothing if smooth == 1.
#------------------------------------------------------------------------
proc findBestWarp {arrayname scaleLst {smooth 1}} {
  upvar $arrayname scoreN

  # smooth the normalized phone scores
  if {$smooth} {
      for { set i [expr [llength $scaleLst] - 2]} { $i > 0 } { incr i -1 } {
	  set scale  [lindex $scaleLst $i]
	  set scaleL [lindex $scaleLst [expr $i-1]]
	  set scaleR [lindex $scaleLst [expr $i+1]]
	  set tmpN($scale) [expr ($scoreN($scale)+$scoreN($scaleL)+$scoreN($scaleR))/3]
      }
      for { set i 1 } { $i < [expr [llength $scaleLst] - 1] } { incr i }  {
	  set scale  [lindex $scaleLst $i]
	  set scoreN($scale) $tmpN($scale)
      }
  }

  # find minimum
  set lscale [lindex $scaleLst 0]
  set lmin   $scoreN($lscale)

  foreach scale $scaleLst {
    if { $scoreN($scale) < $lmin} {
      set lmin   $scoreN($scale)
      set lscale $scale
    }
  }
  return $lscale
}

# ------------------------------------------------------------------------
#  findLabelWarp   Given a path (in a label file) rescore all utterances
#                  of the given speaker within in a window of different
#                  warp scales. Utterances of the speaker are taken until 
#                  a maximum number of frames is reached.
#                  Return warp factor with best score and frames used.
# ------------------------------------------------------------------------

proc findLabelWarp { LSID args } {

  upvar \#0 $LSID SID
  global WARPSCALE warpScales

  set    cwarp        1.00
  set    window       2
  set    delta        0.02
  set    maxFrame     2000
  set    v            0

  if [info exists SID(labelPath)]  {set  labelPath  $SID(labelPath)  } else { set labelPath ""}
  if [info exists SID(warpPhones)] {set  phoneLst   $SID(warpPhones) } else { set phoneLst ""}
  if [info exists SID(optWord)]    {set  optWord    $SID(optWord)    } else { set optWord \$}
  if [info exists SID(variants)]   {set  variants   $SID(variants)   } else { set variants 0 }
  if [info exist  SID(TRL)]        {set  trl        $SID(TRL)        } else { set trl TEXT }

  itfParseArgv findLabelWarp $args [list [
    list <speaker>     string  {} spk         {} {speaker ID}          ] [
    list -labelPath    string  {} labelPath   {} {path of label files} ] [
    list -warp         float   {} cwarp       {} {center warp}         ] [
    list -window       string  {} window      {} {window width/2}      ] [
    list -delta        string  {} delta       {} {delta steps}         ] [
    list -maxFrame     int     {} maxFrame    {} {maximal number of frames to use} ] [
    list -v            int     {} v           {} {verbosity}           ] [
    list -optWord      int     {} optWord     {} {optional HMM word} ] [
    list -variants     int     {} variants    {} {use pronunciation variants} ] [
    list -trl          int     {} trl         {} {Text field in database entry} ] [
    list -phoneLst     string  {} phoneLst    {} {list of phones}      ] ]

  if {$v} {
      puts "findLabelWarp using:"
      puts "speaker  : $spk"
      puts "labelPath: $labelPath"
      puts "warp     : $cwarp"
      puts "window   : $window"
      puts "delta    : $delta"
      puts "maxFrame : $maxFrame"
      puts "optWord  : $optWord"
      puts "variants : $variants"
      puts "trl      : $trl"
      puts "phoneLst : $phoneLst"
  }
  
  #convert cwarp from float to string
  set cwarp [lindex [makeScaleList $cwarp 1 0] 0]

  if { $window < 1} { return $cwarp }
  set scaleLst  [makeScaleList $cwarp $window $delta]

  #reset the vscore frameN array from previous speaker
  foreach scale $scaleLst {
        set vscore($scale) 0.0
        set frameN($scale) 0
  }

  foreachSegment utt uttDB $spk {
    writeLog stderr "%TURN: $spk $utt : scaleList = $cwarp $window $delta"
    set uttInfo [uttDB uttInfo $spk $utt]
    makeArray uttArray $uttInfo       ;# needed for labelpath
    eval set labelFile "$labelPath"
    set WARPSCALE  $cwarp

    if [catch { labelUtterance $LSID $spk $utt $labelFile \
           -variants $variants -optWord $optWord} msg] { 
	writeLog stderr $msg
    } else {
	set path path$LSID
	if {[llength [info command lbox$LSID]] >0 } { set path lbox$LSID:$utt }
	scoreWarpLst frameN vscore $LSID $path hmm$LSID $uttInfo $scaleLst $phoneLst 
    }
    if {  [info exist frameN($cwarp)] && $frameN($cwarp) > $maxFrame } { break }
  }
  if {$v} {writeLog stderr "result for speaker $spk $scaleLst"}
  foreach scale $scaleLst {
      if { (! [info exists frameN($scale)]) || $frameN($scale) == 0 } {
          # not a single vtln-frame
          puts stderr "no frames for VTLN!!!"
          return $cwarp
      }
      set scoreN($scale)  [expr $vscore($scale) / $frameN($scale)]
      if {$v} {writeLog stderr "  scoreV $spk $scale $frameN($scale) $vscore($scale)"}
  }
  return [findBestWarp scoreN $scaleLst]
}


# ------------------------------------------------------------------------
#  findViterbiWarp Find the best warp factor within a given window around 
#                  an initial value. Use a first hypothesis given in 
#                  $HYPO($utt) and do viterbi. Rescore for different warp
#                  scales and all utterances of the speaker.
#                  Return warp factor with best score and frames used.
# ------------------------------------------------------------------------

proc findViterbiWarp { LSID args } {
    upvar \#0 $LSID SID
    global WARPSCALE HYPO
    
    set    cwarp        1.0
    set    window       2
    set    delta        0.02
    set    maxFrame     2000
    set    beam         400

    catch { 
	if {[catch {set  phoneLst     [phonesSet$LSID:$SID(warpPhones)]}]} {
	    set  phoneLst     [phonesSet${LSID}:PHONES]
	    puts "findViterbiWarp: will use phonesSet$LSID:PHONES"
	}
	set    optWord      $SID(optWord)
	set    variants     $SID(variants)
	if [info exist SID(TRL)] { set trl $SID(TRL) } else { set trl TEXT }
    }

    if {[catch {itfParseArgv findViterbiWarp $args [list \
    [list <speaker>     string  {} spk         {} {speaker ID}          ] \
    [list -warp         float   {} cwarp       {} {center warp}         ] \
    [list -window       int     {} window      {} {window width/2}      ] \
    [list -delta        float   {} delta       {} {delta steps}         ] \
    [list -maxFrame     int     {} maxFrame    {} {maximal number of frames to use} ] \
    [list -phoneLst     string  {} phoneLst    {} {list of phones}      ] ]
    }]} {
	return
    }

  #convert cwarp from float to string
  set cwarp [lindex [makeScaleList $cwarp 1 0] 0]

  if { $window < 1} { return $cwarp }
  set scaleLst  [makeScaleList $cwarp $window $delta]

  writeLog stderr "%TURN: $spk : scaleList = $cwarp $window $delta"
  foreachSegment utt uttDB $spk {
    set uttInfo [uttDB uttInfo $spk $utt]
    makeArray uttArray $uttInfo       ;# needed for labelpath

    if {! [info exist HYPO($utt)] } { 	
      #writeLog stderr "WARNING: using data base text as warp hypo for $utt!\n"
      set text $uttArray($trl)
    } else {
      set text $HYPO($utt)
    }
    set WARPSCALE    $cwarp
    if [catch {viterbiUtterance $LSID $spk $utt \
               -text $text -beam $beam -variants $variants -optWord $optWord} msg] {
         writeLog stderr $msg
    } else {
	set path path$LSID
	if {[llength [info command lbox$LSID]] >0 } { set path lbox$LSID:$utt }
	scoreWarpLst frameN vscore $LSID $path hmm$LSID $uttInfo $scaleLst $phoneLst 
    }
      
    if {  [info exist frameN($cwarp)] && $frameN($cwarp) > $maxFrame } { 
	break 
    } else {
        if { ! [info exist frameN($cwarp)] } {
	    writeLog stderr "frameN($cwarp) doesn't exist!" 
	} else {
	    #writeLog stderr "Frames: $frameN($cwarp) (of $maxFrame)"  
	}
    }
  }
  foreach scale $scaleLst {
      if { (! [info exists frameN($scale)]) || $frameN($scale) == 0 } {
          # not a single vtln-frame
          puts stderr "no frames for VTLN!!!"
          return $cwarp
      }
      set scoreN($scale)  [expr $vscore($scale) / $frameN($scale)]
  }
  return [findBestWarp scoreN $scaleLst]
}










