# ========================================================================
#  JANUS-SR   Janus Speech Recognition Toolkit
#             ------------------------------------------------------------
#             Object: Testing Procedures
#             ------------------------------------------------------------
#
#  Author  :  Michael Finke
#  Module  :  test.tcl
#  Date    :  06/24/96
#
#  Remarks :  **** IMPORTANT ****
#
#             The routines in this script are provided for backward
#             compatibility only. They may not work on newer versions
#             of janus, which contain the "ibis" decoder. The 
#             functionality of this script is superceeded by "ibis.tcl"!
#
# ========================================================================
#
#  $Log$
#  Revision 1.6  2003/08/14 11:20:31  fuegen
#  Merged changes on branch jtk-01-01-15-fms (jaguar -> ibis-013)
#
#  Revision 1.5.48.3  2003/08/12 16:12:39  metze
#  Cleanup for P013
#
#  Revision 1.5.48.2  2003/06/06 09:43:59  metze
#  Added latview, fixed upvar, updated tclIndex
#
#  Revision 1.5.48.1  2002/10/04 13:21:47  metze
#  Added warning because this file is superfluous
#
#  Revision 1.5  2000/09/14 17:47:13  janus
#  Merging branch jtk-00-09-14-jmcd.
#
#  Revision 1.4.8.1  2000/09/14 16:20:56  janus
#  Merged branches jtk-00-09-10-tschaaf, jtk-00-09-12-hyu, and jtk-00-09-12-metze.
#
#  Revision 1.4  2000/09/12 09:55:05  janus
#  Added Hua's writeCTM method to score swith SCLITE (...writeSCTM)
#
#  Revision 1.3.2.1  2000/09/14 11:57:19  janus
#  added checking if lattice file exists
#
#  Revision 1.4  2000/09/12 09:55:05  janus
#  Added Hua's writeCTM method to score swith SCLITE (...writeSCTM)
#
#  Revision 1.3  2000/09/10 12:18:48  hyu
#  Added Tcl files from Hagen
#
#  Revision 1.2  2000/01/26 16:09:13  soltau
#  Changed return value in latticeLoad in error case
#
#  Revision 1.1  2000/01/26 16:06:29  soltau
#  Initial revision
#
#
# ========================================================================

catch {
  Search  configure -silenceWordPenalty 5 -use3gFwd 1
  TreeFwd configure -beamWidth 160 -phoneBeamWidth 140
  TreeFwd configure -lastPhoneBeamWidth 100 -lastPhoneAloneBeamWidth 100
  TreeFwd configure -wordBeamWidth 100 -topN 50
  FlatFwd configure -beamWidth 180 -phoneBeamWidth 160
  FlatFwd configure -lastPhoneBeamWidth 140 -wordBeamWidth 120 -topN 50
  Lattice configure -beamWidth 80 -topN 50
}


# ------------------------------------------------------------------------
#  testInit
# ------------------------------------------------------------------------

proc testInit { LSID args } {
  upvar  \#0 $LSID SID
  global janus_lmInterface

  if [info exist SID(vocabDesc)] { set vocabDesc $SID(vocabDesc) }
  if [info exist SID(lmDesc)]    { set lmDesc $SID(lmDesc) }
  if [info exist SID(useXwt)]    { set useXwt $SID(useXwt) }
  if [info exist SID(lmWeight)]  { set lmWeight $SID(lmWeight)
  } else                         { set lmWeight 19 }
  if [info exist SID(lmPenalty)] { set lmPenalty $SID(lmPenalty) 
  } else                         { set lmPenalty 0 }
  
  itfParseArgv testInit $args [list [
    list -vocab  string  {} vocabDesc  {} {vocabulary}              ] [
    list -useXwt string  {} useXwt     {} {use xword models}        ] [
    list -lm     string  {} lmDesc     {} {language model}          ] [
    list -lz     float   {} lmWeight   {} {language model weight}   ] [
    list -lp     float   {} lmPenalty  {} {language model penalty}  ] ]

  if { [llength [info command ttree$LSID]] &&
       [llength [info command amodelSet$LSID]] < 1} {

    if { [llength [info command durTree$LSID]] } {
             AModelSet amodelSet$LSID\
                       ttree$LSID   [ttree$LSID.item(0) configure -name]\
                      -durationTree  durTree$LSID\
                      -durationRoot  ROOT
    } else { AModelSet amodelSet$LSID\
                       ttree$LSID   [ttree$LSID.item(0) configure -name]}
  }

  if { [llength [info command dict$LSID]] &&
       [llength [info command amodelSet$LSID]] &&
       [llength [info command vocab$LSID]] < 1 &&
       [info exist vocabDesc] && [string length $vocabDesc]} {

     writeLog stderr "vocab$LSID       read $vocabDesc"
     Vocab vocab$LSID $vocabDesc -dictionary    dict$LSID  \
                                 -acousticModel amodelSet$LSID \
                                 -useXwt        $useXwt
     set SID(vocabDesc) $vocabDesc
     set SID(useXwt)    $useXwt
  }

  if { [llength [info command vocab$LSID]] < 1 &&
       [info exist vocabDesc] && [string length $vocabDesc]} {

     writeLog stderr "vocab$LSID       read $vocabDesc"
     Vocab vocab$LSID $vocabDesc

     set SID(vocabDesc) $vocabDesc
  }

  if { [llength [info command vocab$LSID]] &&
       [llength [info command lm$LSID]] < 1 &&
       [info exist lmDesc] && [string length $lmDesc] } {



     if { [string match *.tcl $lmDesc] } {
	 writeLog stderr "source $lmDesc lm$LSID  $lmWeight $lmPenalty"
	 source ${lmDesc}
	 lm$LSID configure -lz $lmWeight -lp $lmPenalty
     } else {
	 writeLog stderr "lm$LSID          read $lmDesc -weight $lmWeight -penalty $lmPenalty"
	 if [info exist janus_lmInterface] {
	     Lm lm$LSID vocab$LSID $lmDesc -cachelines 500
	     lm$LSID configure -lz $lmWeight -lp $lmPenalty
	 } else {
	     Lm lm$LSID vocab$LSID $lmDesc -weight $lmWeight -penalty $lmPenalty \
		     -cachelines 500
	 }
     }

     set SID(lmDesc)    $lmDesc
     set SID(lmWeight)  $lmWeight
     set SID(lmPenalty) $lmPenalty
  }

  if { [llength [info command vocab$LSID]] &&
       [llength [info command lm$LSID]] &&
       [llength [info command dict$LSID]] &&
       [llength [info command amodelSet$LSID]] &&  
       [llength [info command search$LSID]] < 1} { 
    Search  search$LSID vocab$LSID lm$LSID
  }
}

# ------------------------------------------------------------------------
#  HypoList::writeCTM
# ------------------------------------------------------------------------

proc hypoListWriteCTM { hl args} {
    
    set file stdout

    itfParseArgv hypoListWriteCTM $args [list [
      list <speaker> string  {} spk  {} {speaker   ID}  ] [
      list <uttID>   string  {} utt  {} {utterance ID}  ] [
      list -file     string  {} file {} {filename}      ] ]

    regexp {(....)-([AB])} $spk dummy conv channel

    set hypo [$hl puts -id $utt -style normal]

    if { $file != "stdout" } { set FP [open $file a] } else { set FP stdout }

    puts $FP $hypo

    if { $file != "stdout" } { close $FP }
    return
}

proc hypoListWriteSCTM { hl args } {
    set start-time 0
    set adcbase    UNK

    itfParseArgv hypoListWriteSCTM $args [list [
        list <ctmfile>  string  {} ctmfile     {} {SCTM filename}  ] [
        list -adcbase   string  {} adcbase     {} {show ID}       ] [
        list -tfrom     string  {} start-time  {} {utt start time}  ]]
    
    if  {1 == [catch {
	set frameInfo [$hl puts -style tcl]
	set frameInfo [lrange [lindex $frameInfo 1] 1 end]
	set fp        [open $ctmfile a]
	foreach piece $frameInfo {
	    set word       [lindex $piece 0]
	    set fromFrame  [lindex $piece 2]
	    set toFrame    [lindex $piece 3]
	    set fromTime   [expr ${start-time} + ${fromFrame}.0 / 100.0]
	    set toTime     [expr ${start-time} + ${toFrame}.0   / 100.0]
	    set durTime    [expr $toTime - $fromTime]
	    if {[string match {+*} $word]} { continue }
	    if {$word == "$" || $word == "(" || $word == ")"} { continue }
	    regsub {\(.*\)} $word {} word	
	    puts $fp "$adcbase 1 $fromTime $durTime $word"
	    flush $fp
	}
	close $fp
    } msg ] } {
	puts stderr "ERROR in hypoListWriteCTM2: $msg\n"
    }       
    return
}

catch {
    HypoList method writeSCTM hypoListWriteSCTM -text "write hypo in SCTM format"
    HypoList method writeCTM  hypoListWriteCTM  -text "write hypo in CTM format"
}


# ------------------------------------------------------------------------
#  testUtterance
# ------------------------------------------------------------------------

proc testUtterance { LSID args} {
  upvar \#0 $LSID SID

  if [info exist SID(lmWeightList)]  { set lmWeightList  $SID(lmWeightList)  }
  if [info exist SID(lmPenaltyList)] { set lmPenaltyList $SID(lmPenaltyList) }
  if [info exist SID(lattice)]       { set lattice       $SID(lattice)       }

  set hypos hypos

  itfParseArgv testUtterance $args [list [
    list <speaker> string  {} spk           {} {speaker   ID}         ] [
    list <uttID>   string  {} utt           {} {utterance ID}         ] [
    list -hypoPath string  {} hypos         {} {path to write hypos}  ] [
    list -llz      string  {} lmWeightList  {} {list of LM weights}   ] [
    list -llp      string  {} lmPenaltyList {} {list of LM penalties} ] [
    list -lattice  string  {} lattice       {} {lattice file}         ] [
    list -mladapt  object  {} mla MLAdapt      {ML Adaptation}        ] ]

  set       uttInfo  [uttDB uttInfo $spk $utt]
  makeArray uttArray $uttInfo

  if {![file exists $hypos]} { exec mkdir $hypos }

  writeLog stderr "testUtterance $LSID $args"

  search${LSID} treeFwd -eval $uttInfo
  search${LSID}.treeFwd.hypoList writeCTM $spk $utt

  if [info exist mla] {
    set text [string trim [search${LSID}.treeFwd.hypoList puts -style simple]]
    set beam     200
    set topN     0
    set minCount 500

    writeLog stderr "MLAdaptation: $spk $utt"

    if [catch {hmm${LSID} make $text} msg] {
        writeLog stderr "ERROR hmm${LSID} make $text\n$msg"
    } else {
      if [catch {path$LSID viterbi hmm$LSID -beam $beam -topN $topN} score] {
        writeLog stderr "ERROR path$LSID viterbi hmm$LSID -beam $beam -topN $topN\n$score"
      } else {
        writeLog stderr "$text (score $score)"
        senoneSet$LSID accu path$LSID
        writeLog stderr "$mla update: [$mla update -minCount $minCount]"
      }
    }
  }

  search${LSID} flatFwd
  search${LSID}.flatFwd.hypoList writeCTM $spk $utt

  search${LSID} lattice

  foreach lz $lmWeightList {
    foreach lp $lmPenaltyList {
      catch {search${LSID}.lattice rescore -lz $lz -lp $lp}
      search${LSID}.lattice.hypoList writeCTM $spk $utt  \
                                    -file $hypos/H_[pid]_z${lz}_p${lp}_LV.ctm
    }
  }
  if { [info exist lattice] && [string length $lattice] } {
    search${LSID}.lattice write $lattice -id $utt
  }
}

# ------------------------------------------------------------------------
#  Lattice::rescoreCTM
# ------------------------------------------------------------------------

proc latticeRescoreCTM { lattice hypoDir spk utt lm llz llp } {
    # remember the lp/lz parameter if the new LM interface is used
    global janus_lmInterface
    if [info exist janus_lmInterface] { set oldP [$lm configure -lp] ; set oldZ [$lm configure -lz] }

    foreach lz $llz {
	foreach lp $llp {
	    if [info exist janus_lmInterface] {
		$lm configure -lp $lp -lz $lz ; catch {$lattice rescore $lm}
	    } else {
		catch {$lattice rescore -lz $lz -lp $lp}
	    }
	    ${lattice}.hypoList writeCTM $spk $utt -file $hypoDir/H_[pid]_z${lz}_p${lp}_LV.ctm
	}
    }

    if [info exist janus_lmInterface] { $lm configure -lp $oldP -lz $oldZ }
}


# ------------------------------------------------------------------------
#  Lattice::load
# ------------------------------------------------------------------------

proc latticeLoad { lattice file } {

    if { ![file exists $file] && ![file exists ${file}.gz] } {
	error "$file not found (even tried .gz)"
    }
    
    if { [string match *.gz $file] } {
	set fi [open "|gunzip -c $file" r]
    } else {
	set fi [open $file r]
    }

    while { [gets $fi line] >= 0} {
	if { [string length $line]} {
	    set key [lindex $line 0]
	    if { $key == "%TURN:" } {
		set key $key
	    } elseif { $key == "END_LATTICE" } {
		close $fi
		return 1
	    } elseif  { $key != "BEGIN_LATTICE" } { 
		eval $lattice add $line
	    }
	}
    }
    error "No end lattice"
}

catch { 
    Lattice method rescoreCTM latticeRescoreCTM -text "rescoring a lattice and writing the results in CTM format"
    Lattice method load latticeLoad -text "load a lattice from an utterance file"
}
