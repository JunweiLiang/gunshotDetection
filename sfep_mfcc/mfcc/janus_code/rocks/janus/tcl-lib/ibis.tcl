# ========================================================================
#  JANUS-SR   Janus Speech Recognition Toolkit
#             ------------------------------------------------------------
#             Object: Procedures for IBIS
#             ------------------------------------------------------------
#
#  Author  :  Florian Metze
#  Module  :  ibis.tcl
#  Date    :  $Id: ibis.tcl 3419 2011-03-24 00:45:13Z metze $
#
#  Remarks :  This file contains procedures pertaining to the
#             New Janus Decoder called "IBIS"
#
# ========================================================================
#
#  $Log: ibis.tcl,v $
#  Revision 1.9  2007/02/23 10:19:59  fuegen
#  added flag for lookahead svmap language model (from Florian Metze)
#
#  Revision 1.8  2005/04/08 09:09:40  metze
#  Added -lksWeights
#
#  Revision 1.7  2004/10/27 17:21:23  metze
#  Fixed beautification in lksInit
#
#  Revision 1.6  2004/10/27 12:13:19  metze
#  Added 'lksInit'
#
#  Revision 1.5  2004/08/23 11:33:18  metze
#  Added glat writeCTM -conf 1 (requires P014)
#
#  Revision 1.4  2004/08/16 16:20:18  metze
#  P014
#
#  Revision 1.3  2003/11/13 12:37:06  metze
#  Introduced nGramLMMatch
#
#  Revision 1.2  2003/08/14 11:20:27  fuegen
#  Merged changes on branch jtk-01-01-15-fms (jaguar -> ibis-013)
#
#  Revision 1.1.2.46  2003/07/30 11:15:26  metze
#  Fix for <UNK> mappings
#
#  Revision 1.1.2.45  2003/07/14 15:22:16  metze
#  Fixed MetaLM generation
#
#  Revision 1.1.2.44  2003/05/15 12:57:09  soltau
#  Made phraseLMReadMap less verbose
#
#  Revision 1.1.2.43  2003/03/14 16:29:58  metze
#  Can now handle LM that contain '12974'
#
#  Revision 1.1.2.42  2003/03/12 10:57:45  soltau
#  createCNet : added option for consensus beam
#
#  Revision 1.1.2.41  2003/03/07 17:00:26  soltau
#  createCNet : added options for optWord and Scaling
#
#  Revision 1.1.2.40  2003/03/07 14:18:25  soltau
#  createCNet : make glatIn = glatOut possible
#
#  Revision 1.1.2.39  2003/02/12 15:36:08  metze
#  Fix for ESST (mapping to class added)
#
#  Revision 1.1.2.38  2003/02/12 13:59:44  metze
#  Backwards compatibility
#
#  Revision 1.1.2.37  2003/02/05 09:47:55  soltau
#  Added createCNet
#  Added hmmConvertGLat
#  support for callhome hypotheses
#
#  Revision 1.1.2.36  2003/01/08 16:43:29  metze
#  To match P012
#
#  Revision 1.1.2.35  2002/11/27 13:14:23  metze
#  svmap13102SID readSubs -lks
#
#  Revision 1.1.2.34  2002/11/19 18:02:12  metze
#  Merged my changes
#
#  Revision 1.1.2.33  2002/11/19 11:24:44  soltau
#  ibisInit : incredible improvements
#
#  Revision 1.1.2.32  2002/11/18 17:39:06  fuegen
#  minor bugfixes for grammar and one4all support
#
#  Revision 1.1.2.31  2002/11/15 13:50:33  metze
#  Improved 'looks' in ibisInit
#
#  Revision 1.1.2.30  2002/11/13 13:17:25  soltau
#  *** empty log message ***
#
#  Revision 1.1.2.29  2002/11/13 12:47:09  soltau
#  ibisInit : checking for optional words, less verbose output
#
#  Revision 1.1.2.28  2002/11/05 08:47:29  metze
#  Catches and sensible beam values
#
#  Revision 1.1.2.27  2002/11/04 15:03:56  metze
#  Added documentation code
#
#  Revision 1.1.2.26  2002/07/30 07:53:15  metze
#  Added version check
#
#  Revision 1.1.2.25  2002/07/29 13:09:43  metze
#  Moved printDo here
#
#  Revision 1.1.2.24  2002/07/18 14:48:03  metze
#  Version to go with ibis11, supports janus_lmInterface
#
#  Revision 1.1.2.23  2002/07/12 11:14:00  metze
#  Added support for ${SID}(segSize)
#
#  Revision 1.1.2.22  2002/05/27 14:31:10  metze
#  Cleaned up LTree options
#
#  Revision 1.1.2.21  2002/05/03 15:08:44  soltau
#  glatWriteCTM: added option for beam
#
#  Revision 1.1.2.20  2002/05/03 14:06:44  metze
#  New-flavoured LingKS interface implemented
#
#  Revision 1.1.2.19  2002/04/26 12:45:12  soltau
#  changes accorsing 'readSubs'
#
#  Revision 1.1.2.18  2002/04/08 12:36:35  metze
#  Updated readMapFile
#
#  Revision 1.1.2.17  2002/03/24 15:48:18  soltau
#  writeCTM : added warpA option to compensate removed 0' frames
#
#  Revision 1.1.2.16  2002/03/12 09:09:10  metze
#  1 -> base
#
#  Revision 1.1.2.15  2002/02/14 17:38:25  metze
#  Added glatWriteCTM, added -result option
#
#  Revision 1.1.2.14  2002/02/06 14:23:05  soltau
#  phraseLMReadMap: added options verbose and force
#
#  Revision 1.1.2.13  2002/02/02 17:02:12  soltau
#  Added readWeights for MetaLM
#
#  Revision 1.1.2.12  2002/01/26 13:06:50  soltau
#  Added writeCTM for glat
#
#  Revision 1.1.2.11  2002/01/25 17:57:36  soltau
#  Added spassWriteHypo to write in ctm style
#
#  Revision 1.1.2.10  2002/01/25 15:48:54  soltau
#   svmapReadMap: fixed mismatched brackets
#
#  Revision 1.1.2.9  2002/01/17 13:19:15  metze
#  Made svmapReadMap saver to use with LModelNJD
#
#  Revision 1.1.2.8  2001/11/26 12:49:49  metze
#  Bugfix?
#
#  Revision 1.1.2.7  2001/11/07 15:56:24  metze
#  Added XCM
#  Made options for phraseLM (readLMMap) safer to use
#
#  Revision 1.1.2.6  2001/11/06 16:03:57  metze
#  Changes for consistency
#
#  Revision 1.1.2.5  2001/10/22 08:46:45  metze
#  Added switch for Acoustic Fast Match (aka Look-Ahead)
#
#  Revision 1.1.2.4  2001/10/17 15:43:48  metze
#  Implemented extra procedure for LMs
#
#  Revision 1.1.2.3  2001/09/27 13:05:07  metze
#  Added PhraseLM type
#
#  Revision 1.1.2.2  2001/09/25 17:52:18  metze
#  Works for standard GSST decoding
#
#  Revision 1.1.2.1  2001/09/24 17:47:34  metze
#  Initial version
#
# ========================================================================


proc ibis_Info { name } {
    switch $name {
	procs {
	    return "ibisInit lksInit"
	}
	help {
	    return "This file contains procedures for the Ibis decoder. It can \
be initialized by calling 'ibisInit \$SID' as usual. The other rotines are available \
as methods of various objects."
	}
	ibisInit {
	    return "Initializes the Ibis decoder object hierarchy. It is possible to \
integrate existing objects (e.g. language models) into the decoder, although this \
procedure can build objects and load the appropriate descriptions, data- or dumpfiles, too."
	}
        lksInit {
            return "Can be used to create a language model."
        }
    }
    return "???"
}


# ------------------------------------------------------------------------
#  ibisInit
# ----------
#  This procedure can be used to initialize "IBIS", the New Janus Decoder
# ------------------------------------------------------------------------

proc ibisInit { LSID args } {
    upvar \#0 $LSID SID
    global janus_lmInterface janus_patchLevel janus_version janus_lksInit

    set log    stderr

    # Defaults for parameter files
    foreach i { vocabDesc lmDesc lmlaDesc mapDesc phraseDesc baseLMDesc ipolLMDesc streeDump } { set $i "" }
    set svmapMode    base
    set readSubs     0
    set smemFree     1
    
    set verbose      0
    
    set cacheN     100
    set depth        5
    set xcm          0
    set ignoreRCM    1
    set useCtx       1

    set lmlaWeight  26
    set lmWeight    26
    set lmPenalty    0
    set filPenalty  10

    set morphBeam   80
    set stateBeam  130
    set wordBeam    90
    set morphN       8
    set transN      35
    set masterBeam 1.0

    # Defaults for (possibly existing) object names
    set ttree    ttree$LSID
    set phmmSet  phmmSet$LSID
    set lcmSet   lcmSet$LSID
    set rcmSet   rcmSet$LSID
    set xcmSet   xcmSet$LSID
    set dict     dict$LSID
    set vocab    svocab$LSID
    set lm       lm$LSID
    set lmla     ""
    set svmapla  ""
    set baseLM   baselm$LSID
    set ipolLM   ipollm$LSID

    if {$janus_lmInterface == 2} { set lmT LingKS
    } else { set svmapMode 1;      set lmT LModelNJD }
    set svmap    svmap$LSID
    set stree    stree$LSID
    set ltree    ltree$LSID
    set spass    spass$LSID

    set LFID     $LSID

    # Overwrite with application's defaults
    foreach i { svmapMode readSubs cacheN depth vocabDesc mapDesc lmDesc lmlaDesc baseLMDesc ipolLMDesc lmstreeDump laWeight lmWeight lmPenalty filPenalty masterBeam morphBeam stateBeam wordBeam morphN transN } {
	if [info exists SID($i)] { eval "set $i $SID($i)" }
    }

    if {[info exists janus_lksInit]} { set mapDesc [set vocabDesc ""] }

    # Scan command line parameters
    itfParseArgv ibisInit $args [list [
        list -dict         object  {} dict       Dictionary {Search Dictionary}        ] [
        list -ttree        object  {} ttree      Tree       {Topology Tree}            ] [
        list -phmmSet      object  {} phmmSet    PHMMSet    {Phonetic HMM Set}         ] [
        list -lcmSet       object  {} lcmSet     LCMSet     {Left Context Model Set}   ] [
        list -rcmSet       object  {} rcmSet     RCMSet     {Right Context Model Set}  ] [
        list -xcmSet       object  {} xcmSet     XCMSet     {X-Word Context Model Set} ] [
	list -vocab        object  {} vocab      SVocab     {Search Vocabulary}        ] [
        list -svmap        object  {} svmap      SVMap      {Mapper SVX->LVX}          ] [
        list -stree        object  {} stree      STree      {Search Tree (Phonetic)}   ] [
        list -ltree        object  {} ltree      LTree      {Search Tree (Linguistic)} ] [
	list -spass        object  {} spass      Tree       {Search Object}            ] [

        list -streeDump    string  {} streeDump  {}         {search tree dump file}    ] [
        list -vocabDesc    string  {} vocabDesc  {}         {search vocabulary}        ] [
        list -mapDesc      string  {} mapDesc    {}         {traditional LM-map file or pron. variants} ] [
        list -maplaDesc    string  {} maplaDesc  {}         {traditional LM-map file or pron. variants} ] [
        list -readSubs     int     {} readSubs   {}         {read map-table from 'NGramLM' into SVMap} ] [
        list -phraseLMDesc string  {} phraseDesc {}         {multi-word LM file}       ] [
        list -baseLMDesc   string  {} baseLMDesc {}         {base lmodel}              ] [
        list -ipolLMDesc   string  {} ipolLMDesc {}         {interpolation lmodel}     ] [
        list -lmDesc       string  {} lmDesc     {}         {language model}           ] [
        list -lmlaDesc     string  {} lmlaDesc   {}         {language model lookahead} ] [
        list -lalz         float   {} lmlaWeight {}         {LM lookahead weight}      ] [
        list -lz           float   {} lmWeight   {}         {language model weight}    ] [
	list -lp           float   {} lmPenalty  {}         {language model penalty}   ] [
	list -fp           float   {} filPenalty {}         {filler word penalty}      ] [
	list -masterBeam   float   {} masterBeam {}         {master beam setting}      ] [

	list -lmType       string  {} lmT        {}         {Language Model Type}      ] [
	list -lks          object  {} lm         $lmT       {Language Model}           ] [
	list -lm           object  {} lm         $lmT       {Language Model (discouraged)} ] [
	list -lksla        object  {} lmla       $lmT       {LookAhead Language Model} ] [
	list -cacheN       int     {} cacheN     {}         {cache lines in ltree}     ] [
	list -depth        int     {} depth      {}         {depth of ltree}           ] [

	list -xcm          int     {} xcm        {}         {use XCMSet}               ] [
	list -useCtx       int     {} useCtx     {}         {use context}              ] [
	list -smemFree     int     {} smemFree   {}         {free memory}              ] [
	list -ignoreRCM    int     {} ignoreRCM  {}         {ignore RCMs in XCM}       ] [
        list -fastMatch    string  {} LFID       {}         {Fast Match SID}           ] [
	list -verbose      int     {} verbose    {}         {verbose}                  ] ]

    # Check Janus Version
    if {[set pl [string trimleft $janus_patchLevel " P0"]] == ""} { set pl 0 }
    if       {$janus_version < 5.0} {
	writeLog $log "ibisInit           You're running JANUS V${janus_version} P${janus_patchLevel}"
	writeLog $log "ibisInit           This won't do. Please use >= V5.0"
	exit
    } elseif {$janus_version == 5.0 && $pl < 12} {
	writeLog $log "ibisInit           You're running JANUS V${janus_version} P${janus_patchLevel}"
	writeLog $log "ibisInit           Please upgrade to >= V5.0 P012 (trying to continue anyway)"
	set Ibis_Itf 0
    } else {
	set Ibis_Itf 1
    }

    # Check objects we depend upon
    if       {![llength [info command $ttree]]} {
	writeLog $log "\nibisInit: Please create Tree object '$ttree' first."
	break
    } elseif {![llength [info command $dict]]} {
	writeLog $log "\nibisInit: Please create Dictionary object '$dict' first."
	break
    }

    # Phonetic Hidden markov Model
    if {![llength [info command $phmmSet]]} {
	if {!$useCtx} { writeLog $log "$phmmSet     init $ttree [$ttree.item(0) configure -name] -useCtx $useCtx" }
	PHMMSet $phmmSet   $ttree   [$ttree.item(0) configure -name] -useCtx $useCtx
    }

    # Left and Right context models
    if {![llength [info command $lcmSet]]} {
	LCMSet  $lcmSet $phmmSet
    }
    if {![llength [info command $rcmSet]]} {
	RCMSet  $rcmSet $phmmSet
    }
    # CD Single Phone words
    if {![llength [info command $xcmSet]] && $xcm} {
        XCMSet  $xcmSet    $phmmSet -ignoreRCM $ignoreRCM
    }

    # Vocabulary object creation
    if {![llength [info command $vocab]]} {
	SVocab  $vocab     $dict
    }

    # Create a language model
    if {![llength [info command $lm]]} {
	ibisCreateLM $LSID $lmT $lm   $baseLM $ipolLM $lmDesc   $baseLMDesc $ipolLMDesc $mapDesc $phraseDesc
    }

    # Create a language model for the lookahead
    if {![llength [info command $lmla]] && [string length $lmlaDesc]} {
	ibisCreateLM $LSID $lmT $lmla ""      ""      $lmlaDesc ""          ""          ""       "" 
    }

    # Create vocabulary mapper
    if {![llength [info command $svmap]]} {
	SVMap $svmap $vocab $lm
    }

    # Create LA vocabulary mapper
    if {[llength [info command $lmla]] && [string length $maplaDesc] && ![llength [info command $svmapla]]} {
	set svmapla svmapLA
	SVMap $svmapla $vocab $lmla
    }

    # Read SVocab and initialize SVMap if we don't read an stree dump
    if {![string length $streeDump] && ![info exists janus_lksInit]} {

	# Read in SVocab
	if {[string length $vocabDesc]} {
	    writeLog $log "$vocab      read $vocabDesc"
	    $vocab read $vocabDesc
	}

	# SVMap
	writeLog $log "$svmap       map  $svmapMode"
	$svmap map $svmapMode
	if {$readSubs} {
	    writeLog $log "$svmap       subs $lm"
	    $svmap readSubs -lks $lm
	    $svmap map class
	}
	if {[string length $mapDesc]} {
	    writeLog $log "$svmap       read $mapDesc"
	    $svmap readMapFile $mapDesc
	}

	if {[llength [info command $svmapla]]} {
	    $svmapla map base
	    writeLog $log "$svmapla       read $maplaDesc"
	    $svmapla readMapFile $maplaDesc
	}

	if {$Ibis_Itf} {
	    set unkS [$svmap configure -unkString]
	    set unkL [$svmap mappedto $unkS]
	    set unkX [lindex $unkL 0]
	    set unkL [lrange $unkL 1 end]
	    set lvxNIL [$svmap.lingks index ""]
	    if { [llength $unkL] && $unkX != $lvxNIL } {
		set lksName [$svmap.lingks configure -name]
		puts stderr "WARN    ibis.tcl           ibisInit: [llength $unkL] words mapped to ${lksName}:${unkS}"
		if {$verbose > 0} {
		    puts stderr "WARN    ibis.tcl           ibisInit: mapped = $unkL"
		}
		# Make sure the <UNK> words' prob-mass gets properly distributed
		foreach w $unkL { $svmap add $w $unkS -prob [expr -log([llength $unkL])] }
	    }
	}
    }

    # Check for optional silence
    if {$streeDump == "" && $Ibis_Itf} {
	set optWord "$"
	set svxMax [$vocab configure -svxMax]
	set svX    [$vocab index $optWord]
	set dictX  [$dict  index $optWord]
	if {$svxMax == $svX && $dictX >= 0} {
	    $vocab add $optWord -ftag 1
	    $svmap add $optWord <UNK> -prob 0.0
	    puts stderr "WARN    ibis.tcl           ibisInit: added '$optWord' to $vocab"
	    set svX    [$vocab index $optWord]
	}
	if {$svxMax != $svX && $dictX >= 0} {	
	    set ftag  [$vocab:$optWord configure -fTag]
            set dictY [$vocab:$optWord configure -dictX]
            if {$dictX != $dictY} {
                $vocab:$optWord configure -dictX $dictX
                puts stderr "WARN    ibis.tcl           ibisInit: reconfigured '$optWord' (dictionary index)"
            }
	    if {!$ftag} {
		$vocab:$optWord configure -fTag 1
		$svmap add $optWord <UNK> -prob 0.0
		puts stderr "WARN    ibis.tcl           ibisInit: reconfigured '$optWord' (filler tag)"
	    }
	}
    }

    # Search Tree
    if {![llength [info command $stree]]} {
	if {[string length $streeDump]} { writeLog stderr "$stree       read $streeDump" }
	if {[catch {
	    if {$xcm} {
		if {[string length $streeDump]} {
		    STree $stree $svmap $lcmSet $rcmSet -smemFree $smemFree \
			-XCM $xcmSet -dump $streeDump
		} else {
		    STree $stree $svmap $lcmSet $rcmSet -smemFree $smemFree \
			-XCM $xcmSet
		}
	    } else {
		if {[string length $streeDump]} {
		    STree $stree $svmap $lcmSet $rcmSet -smemFree $smemFree \
			-dump $streeDump
		} else {
		    STree $stree $svmap $lcmSet $rcmSet -smemFree $smemFree
		}
	    } }]} {
	    puts stderr "WARN    ibis.tcl           ibisInit: check STree creation!"
	    STree $stree $svmap $lcmSet $rcmSet
	}
    }

    # Linguistic Tree
    if {![llength [info command $ltree]]} {
	if {![string length $svmapla]} {
	    LTree $ltree $stree -depth $depth
	} else {
	    LTree $ltree $stree -depth $depth -map $svmapla
	}
	if {[string length $lmla]} {
	    $ltree configure -cacheN $cacheN -lm $lmla
	} else {
	    $ltree configure -cacheN $cacheN
	}
    }

    # Single Pass Decoder
    if {![llength [info command $spass]]} {
	SPass $spass $stree $ltree
    }

    # Acoustic Fast Match
    if {$LFID != $LSID} {
	global $LFID
	set cbs cbs; set dss dss; set dst dst; set str str; set sns sns
	set ps phonesSet$LSID; set fs featureSet$LSID; set tg tags$LSID

	[CodebookSet $cbs $fs]  read [set ${LFID}(codebookSetDesc)]
	[DistribSet  $dss $cbs] read [set ${LFID}(distribSetDesc)]

	 $cbs load [set ${LFID}(codebookSetParam)]
	 $dss load [set ${LFID}(distribSetParam)]

	Tree $dst $ps:PHONES $ps $tg $dss
	$dst configure -padPhone @
	 $dst read [set ${LFID}(distribTreeDesc)]

	DistribStream $str $dss $dst
	 SenoneSet     $sns $str -phones $ps:PHONES -tags $tg

	$spass fmatch $sns
	 $spass configure -fastMatch 1.0
    }

    # Configure SVMap and SPass
    if {[string length $lmla]} { 
	 $svmap configure -phonePen 0.0 -filPen $filPenalty \
	     -wordPen $lmPenalty -lz $lmWeight -lalz $lmlaWeight 
    } else {
	 $svmap configure -phonePen 0.0 -filPen $filPenalty \
	     -wordPen $lmPenalty -lz $lmWeight
    }

    $spass configure -morphBeam [expr $masterBeam*$morphBeam] \
	-stateBeam [expr $masterBeam*$stateBeam] \
	-wordBeam  [expr $masterBeam*$wordBeam]  \
	-morphN $morphN -transN $transN

    return $spass
}


# ------------------------------------------------------------------------
#  phraseLMReadMap
# -----------------
#  read multi-words from an existing JANUS-Format map file
# ------------------------------------------------------------------------

proc phraseLMReadMap { lm args } {
    global SID

    set base [$lm configure -baseLM]
    set i    0
    set v    0
    set f    multi

    itfParseArgv phraseLMReadMap $args [list [
        list <file>   string  {} file     {} {map-file to read in}                  ] [
        list -mode    string  {} f        {} {add which entries (base, multi, all)} ] [
        list -verbose int     {} v        {} {verbose}                              ] [
        list -base    string  {} base     {} {underlying lm}                        ] ]

    set LVXNIL [$lm.list index IamTheNILWord]

    set fp [open $file r]
    while {[gets $fp line] != -1} {
	set word  [lindex $line 0]
	set basis [lindex $line 1]
	set prob  [expr {([llength $line] == 4) ? [lindex $line 3] : 0.0}]

	if {![string compare $word $basis]} {
	    # We really don't want to deal with 'identical' words here (->SVMap)
	    continue
	}

	if       {![string compare $f "multi"]} {
	    # We only want to deal with single words until in force-mode
	    if {[llength $basis] == 1} { continue }
	} elseif {![string compare $f "base"]} {
	    # Or we only add baseforms
	    if {[regexp "\\(.*\\)" $wort]} { continue }
	}  
	
	set idx [$lm.list index $word]
	if {$idx !=  $LVXNIL && $v == 0} { continue }

	# Do it!
	if {$v} { printDo $lm add $word "$basis" -v $v
	} else {          $lm add $word "$basis" -v $v }
	incr i
    }
    
    close $fp
    return $i
}

PhraseLM method readMapFile phraseLMReadMap -text "read multi-words from an existing JANUS-Format map file"


# ------------------------------------------------------------------------
#  svMapReadMap
# --------------
#  read mappings from an existing JANUS-Format map file
# ------------------------------------------------------------------------

proc svmapReadMap { svmap args } {
    global SID

    set base [$svmap configure -baseLM]
    set i    0
    set v    0

    itfParseArgv svmapReadMap $args [list [
        list <file>   string  {} file     {} {map-file to read in}  ] [
        list -verbose string  {} v        {} {verbosity}            ] [
	list -lm      string  {} base     {} {underlying lm}        ] ]

    set lxn [$base index "TheIDoNotExistInAnyLMWord"]
    set fp  [open $file r]
    while {[gets $fp line] != -1} {
	set prob    0.0
	set wort    [lindex $line 0]
	set current [$svmap get $wort]

	if {![string compare [lindex $line 2] "-prob"]} { 
	    # Get the probability
	    set prob [lindex $line 3]
	}

	if {[llength [set basis [lindex $line 1]]] > 1} {
	    # It must be a multi-word (PhraseLM), so we can only map to the base-form
	    if {[$base index [set basis $wort]] == $lxn} {
		regsub "\\(.*\\)" $wort "" basis
	    }
	}

	if {[set idx [$base index $basis]] != $lxn} {
	    # The base-LM already contains this entry, so subtract the value
	    makeArray tmpA [$base.item($idx) configure]
	    if { ![catch {set prob [expr $prob - $tmpA(-prob)]} msg] } {
		if {[expr abs($prob)] <= 0.001} { continue }
	    }
	}

	if {(![string compare [lindex $current 1] $basis] ||
	     ![string compare [lindex $current 1] $wort]) && 
	    [expr abs($prob)] <= 0.001} {
	    continue
	}

	# Do it!
	if {$v} { printDo $svmap add $wort $basis -prob $prob
	} else {          $svmap add $wort $basis -prob $prob }
	incr i
    }

    close $fp
    return $i
}

SVMap method readMapFile svmapReadMap -text "read mappings from an existing JANUS-Format map file"


# ------------------------------------------------------------------------
#  read interpol weights for MetaLM
# ------------------------------------------------------------------------

proc metaLMloadWeights {lm args} {
    global SID

    itfParseArgv metaLMloadWeights $args [list [
       list <file> string  {} name  {} {weight file}  ] ]
 
    if {! [file exists $name]} {
	writeLog stderr "metaLMloadWeight: cannot find '$name'"
	return
    }
    set fp [open $name] 
    while {[gets $fp line] >= 0} {
	set w [lindex $line 0]
	set s [lindex $line 1]
	$lm add $w -prob $s
    }
    close $fp
}

MetaLM method loadWeights metaLMloadWeights -text "load interpolation weights"


# ------------------------------------------------------------------------
#  ibisCreateLM
# --------------
#  Creates a language model for use with the Ibis decoder
# ------------------------------------------------------------------------

proc ibisCreateLM { LSID lmT lm base ipol lmDesc baseDesc ipolDesc mapDesc phraseDesc} {
    upvar \#0 $LSID SID
    global janus_lmInterface
    
    if {[info exists SID(ngramLMsegSize)]} { set segSize $SID(ngramLMsegSize)
    } else {                                 set segSize 0 }
    
    if {$janus_lmInterface == 2} {
	# ------------------
	#   Ibis Interface 
	# ------------------
	switch $lmT {
	    
	    PhraseLM { # Used for multi words (pronunciation variants are taken care of by SVMap)
		LingKS $base NGramLM
		LingKS $lm   PhraseLM
		if {![string length $baseDesc]} { set baseDesc $lmDesc; set lmDesc "" }
		if {$segSize} { $base.data configure -segSize $segSize }
		
		writeLog stderr "$base      load $baseDesc" 2
		$base    load    $baseDesc
		writeLog stderr "$lm.data     base $base" 2
		$lm.data base    $base

		if        {[string length $lmDesc]} { 
		    writeLog stderr "$lm          load    $lmDesc" 2
		    $lm  load    $lmDesc
		}

		if        {[string length $phraseDesc]} {
		    writeLog stderr "$lm.data     read $phraseDesc" 2
		    $lm.data readMapFile $phraseDesc
		} elseif {[string length $mapDesc]} {
		    writeLog stderr "$lm.data     read $mapDesc" 2
		    $lm.data readMapFile $mapDesc   
		}
	    }
	    
	    MetaLM { # The flexible LM for interpolation, ...
		writeLog stderr "$base      load $baseDesc" 2
		[LingKS $base NGramLM] load $baseDesc
		writeLog stderr "$ipol      load $ipolDesc" 2
		[LingKS $ipol NGramLM] load $ipolDesc
		
		if {$segSize} {
		    $base.data configure -segSize $segSize
		    $ipol.data configure -segSize $segSize
		}
		
		if {[string length $lmDesc]} {
		    writeLog stderr "$lm          read $lmDesc" 2
		    [LingKS $lm $lmT] read $lmDesc
		} else {
		    LingKS $lm MetaLM
		    writeLog stderr "$lm.data     add  $base" 2
		    $lm.data LMadd $base
		    writeLog stderr "$lm.data     add  $ipol" 2
		    $lm.data LMadd $ipol

		    set c 0
		    foreach w [$base puts] {
			if {[$ipol index $w] != -1} { $lm.data add $w; incr c }
		    }
		    #  "Added $c default entries to $lm"
		}
	    }
	    
	    STree { # This lmType can be used to read in the LModelNJD together with the STree dump
		LingKS $lm NGramLM
	    }
	    
	    default { # The default LModelNJD (or NGramLM) type
		LingKS $lm NGramLM
		if {$segSize} { $lm.data configure -segSize $segSize }
		if {[string length $lmDesc]} {
		    writeLog stderr "$lm          load $lmDesc" 2
		    $lm load $lmDesc 
		}
	    }
	}	
    
    } else {
	# ---------------------------
	#   Deferred Ibis Interface 
	# ---------------------------
	writeLog stderr "Using deferred Ibis-LM interface!"
	switch $lmT {
	    
	    PhraseLM { # Used for multi words (pronunciation variants are taken care of by SVMap)
		LModelNJD $base
		PhraseLM  $lm   $base
		if {![string length $baseDesc]} { set baseDesc $lmDesc; set lmDesc "" }
		if {$segSize} { $base configure -segSize $segSize }
		
		 $base   read     $baseDesc
		
		if        {[string length $lmDesc]} { 
		    $lm read     $lmDesc
		}
		if        {[string length $phraseDesc]} {
		    printDo $lm readMapFile $phraseDesc -base $base
		} elseif {[string length $mapDesc]} {
		    printDo $lm readMapFile $mapDesc    -base $base   
		}
	    }
	    
	    MetaLM { # The flexible LM for interpolation, ...
		[LModelNJD $base] read $baseDesc
		[LModelNJD $ipol] read $ipolDesc
		
		if {$segSize} {
		    $base configure -segSize segSize
		    $ipol configure -segSize $segSize 
		}
		
		if {[string length $lmDesc]} {
		     [$lmT $lm] load $lmDesc
		} else {
		    MetaLM $lm
		    ${lm} LMadd $base
		    ${lm} LMadd $ipol
		    set c 0
		    foreach w [$base:] {
			if {[$ipol index $w] != -1} { $lm add $w; incr c }
		    }
		    # puts "Added $c default entries to $lm"
		}
	    }
	    
	    STree { # This lmType can be used to read in the LModelNJD together with the STree dump
		LModelNJD $lm
	    }
	    
	    default { # The default LModelNJD type
		LModelNJD $lm
		if {$segSize} { $lm configure -segSize $segSize }
		if {[string length $lmDesc]} {  $lm read $lmDesc }
	    }
	}
    }
}

proc BeautyOffset { t } {
    global SID
    set o [expr 10 + [string length $SID]]
    return "$t [string repeat { } [expr $o-[string length $t]]]"
}

proc lksInit { LSID args } {
    upvar \#0 $LSID SID
    global janus_lmInterface janus_patchLevel janus_version janus_lksInit

    # Defaults for parameter files
    foreach i { vocabDesc lmDesc lmlaDesc mapDesc phraseDesc baseDesc ipolDesc readSubs streeDump } { set $i "" }
    set svmapMode    base
    set segSize      0
    set verbose      0

    set dict     dict$LSID
    set vocab    svocab$LSID
    set svmap    svmap$LSID

    set lm         lm$LSID
    set base     base$LSID
    set ipol     ipol$LSID

    set lmlaWeight  26
    set lmWeight    26
    set lmPenalty    0
    set filPenalty  10

    # Overwrite with application's defaults
    foreach i { vocabDesc mapDesc lmWeights lmDesc lmlaDesc baseDesc ipolDesc lmstreeDump lmlaWeight lmWeight lmPenalty filPenalty } {
	if {[info exists SID($i)]} { eval "set $i $SID($i)" }
    }

    if {[catch {itfParseArgv lksInit $args [list \
	[list <LKSType>     string  {} lksType     {} {type of LingKS} ] \
	[list -dict         string  {} dict        {} {dictionary}                          ] \
	[list -vocab        string  {} vocab       {} {vocabulary}                          ] \
	[list -svmap        string  {} svmap       {} {mapper SVX->LVX}                     ] \
	[list -lks          string  {} lm          {} {name of LingKS object}               ] \
	[list -lksBase      string  {} base        {} {name of LingKS base object}          ] \
	[list -lksIPol      string  {} ipol        {} {name of LingKS ipol object}          ] \
	[list -segSize      int     {} segSize     {} {segSize parameter for NGramLM}       ] \
	[list -lksWeights   string  {} lmWeights   {} {weights-file to load into main LingKS}       ] \
	[list -lksDesc      string  {} lmDesc      {} {file to load into main LingKS}       ] \
	[list -ipolDesc     string  {} ipolDesc    {} {file to load into ipol LingKS}       ] \
	[list -baseDesc     string  {} baseDesc    {} {file to load into base LingKS}       ] \
	[list -phraseDesc   string  {} phraseDesc  {} {file to load into phrase LingKS}     ] \
	[list -vocabDesc    string  {} vocabDesc   {} {vocabulary file to load}             ] \
	[list -mapDesc      string  {} mapDesc     {} {mapping file to load}                ] \
	[list -readSubs     string  {} readSubs    {} {LM to read substitutions from}       ] \
	[list -verbose      int     {} verbose     {} {verbosity}                           ] ]
    }]} {
	return
    }

    if {$janus_lmInterface != 2} {
	writeLog stderr "lksInit: obsolete Ibis LingKS interface not supported!"
	return
    }


    # -----------------------------------------------
    #   1) Create LVX part
    # -----------------------------------------------
    switch $lksType {
	NGramLM {
	    LingKS $lm NGramLM
	    if {$segSize} { $lm.data configure -segSize $segSize }
	    if {[string length $lmDesc]} {
		writeLog stderr "$lm [string repeat { } [expr 9-[string length $lm]]]load $lmDesc" 1
		$lm load $lmDesc 
	    }
	}

	PhraseLM { # Used for multi words (pronunciation variants are taken care of by SVMap)
	    LingKS $base NGramLM
	    LingKS $lm   PhraseLM
	    if {![string length $baseDesc]} { set baseDesc $lmDesc; set lmDesc "" }
	    if {$segSize} { $base.data configure -segSize $segSize }
	    
	    # Hack for beautiful output
	    set o [expr 11 + [string length $LSID]]

	    writeLog stderr "[BeautyOffset $base] load $baseDesc" 1
	    $base    load    $baseDesc
	    writeLog stderr "[BeautyOffset $lm.data] base $base" 1
	    $lm.data base    $base
	    
	    if       {[string length $phraseDesc]} {
		writeLog stderr "[BeautyOffset $lm.data] read $phraseDesc" 1
		$lm.data readMapFile $phraseDesc
	    } elseif {[string length $lmDesc]} { 
		writeLog stderr "[BeautyOffset $lm] load $lmDesc" 1
		$lm  load    $lmDesc
	    } elseif {[string length $mapDesc]} {
		writeLog stderr "[BeautyOffset $lm.data] read $mapDesc" 1
		$lm.data readMapFile $mapDesc   
	    }
	}
	
	MetaLM { # The flexible LM for interpolation, ...

	    if {![string length [info command $base]]} {
		LingKS $base NGramLM
		if {$segSize} { $base.data configure -segSize $segSize }
		writeLog stderr "$base [string repeat { } [expr 14-[string length $base]]]load $baseDesc" 1
		$base load $baseDesc
	    }
	    if {![string length [info command $base]]} {
		LingKS $ipol NGramLM
		if {$segSize} {$ipol.data configure -segSize $segSize}
		writeLog stderr "$ipol [string repeat { } [expr 14-[string length $ipol]]]load $ipolDesc" 1
		$ipol load $ipolDesc
	    }

	    if {[string length $lmDesc]} {
		writeLog stderr "$lm [string repeat { } [expr 9-[string length $lm]]]read $lmDesc" 1
		[LingKS $lm MetaLM] read $lmDesc
	    } elseif {[string length $lmWeights]} {
		writeLog stderr "$lm [string repeat { } [expr 9-[string length $lm]]]load $lmWeights" 1
		LingKS $lm MetaLM
		$lm.data LMadd $base
		$lm.data LMadd $ipol
		$lm.data loadWeights $lmWeights
	    } else {
		LingKS $lm MetaLM
		writeLog stderr "$lm.data [string repeat { } [expr 9-[string length $lm]]]add  $base" 1
		$lm.data LMadd $base
		writeLog stderr "$lm.data [string repeat { } [expr 9-[string length $lm]]]add  $ipol" 1
		$lm.data LMadd $ipol
		
		set c 0
		foreach w [$base puts] {
		    if {[$ipol index $w] != -1} { $lm.data add $w; incr c }
		}
	    }
	}

	default {
	    writeLog stderr "lksInit: unknown LingKS type '$lksType'"
	    return
	}
    }


    # Return, if we don't deal with SVMap, too
    if {![string length $mapDesc]} { return }
    if {![llength [info command $dict]]} {
        writeLog stderr "lksInit: please create dictionary '$dict' first"
        return
    }


    # -----------------------------------------------
    #   2) Create SVX<-> part
    # -----------------------------------------------

    # Vocabulary object creation
    if {![llength [info command $vocab]]} {
        SVocab  $vocab     $dict
    }

    # Create vocabulary mapper
    if {![llength [info command $svmap]]} {
        SVMap $svmap $vocab $lm
    }

    # Read in SVocab
    if {[string length $vocabDesc]} {
	writeLog stderr "$vocab      read $vocabDesc"
	$vocab read $vocabDesc
    }

    # SVMap
    writeLog stderr "$svmap       map  $svmapMode"
    $svmap map $svmapMode
    if {[string length $readSubs]} {
	writeLog stderr "$svmap       subs $readSubs"
	$svmap readSubs -lks $readSubs
	$svmap map class
    }
    writeLog stderr "$svmap       read $mapDesc"
    $svmap readMapFile $mapDesc

    # Deal with UNK...
    set unkS [$svmap configure -unkString]
    set unkL [$svmap mappedto $unkS]
    set unkX [lindex $unkL 0]
    set unkL [lrange $unkL 1 end]
    set lvxNIL [$svmap.lingks index ""]
    if { [llength $unkL] && $unkX != $lvxNIL } {
	set lksName [$svmap.lingks configure -name]
	puts stderr "INFO    lksInit: [llength $unkL] words mapped to ${lksName}:${unkS}"
	if {$verbose > 0} {
	    puts stderr "INFO    lksInit: mapped = $unkL"
	}
	# Make sure the <UNK> words' prob-mass gets properly distributed
	foreach w $unkL { $svmap add $w $unkS -prob [expr -log([llength $unkL])] }
    }

    # Check for optional silence
    set optWord "$"
    set svxMax [$vocab configure -svxMax]
    set svX    [$vocab index $optWord]
    set dictX  [$dict  index $optWord]
    if {$svxMax == $svX && $dictX >= 0} {
	$vocab add $optWord -ftag 1
	$svmap add $optWord <UNK> -prob 0.0
	puts stderr "INFO    lksInit: added '$optWord' to $vocab"
	set svX    [$vocab index $optWord]
    }
    if {$svxMax != $svX && $dictX >= 0} {
	set ftag  [$vocab:$optWord configure -fTag]
	set dictY [$vocab:$optWord configure -dictX]
	if {$dictX != $dictY} {
	    $vocab:$optWord configure -dictX $dictX
	    puts stderr "INFO    lksInit: reconfigured '$optWord' (dictionary index)"
	}
	if {!$ftag} {
	    $vocab:$optWord configure -fTag 1
	    $svmap add $optWord <UNK> -prob 0.0
	    puts stderr "INFO    lksInit: reconfigured '$optWord' (filler tag)"
	}
    }

    # Configure SVMap
    $svmap configure -phonePen 0.0 -filPen $filPenalty \
	-wordPen $lmPenalty -lz $lmWeight -lalz $lmlaWeight	

    set janus_lksInit 1
}


# ------------------------------------------------------------------------
#  write hypo from SPASS object
# ------------------------------------------------------------------------

proc spassWriteHypo {spass args} {
    global SID
    
    set file     stdout
    set rate     [expr 1000.0 / [featureSet$SID configure -frameShift]]
    set warpA   ""
    set silsmb  ""
    set conv    ""
    set convDB  "CONV"
    set channel  1

    itfParseArgv spassWriteHypo $args [list [
      list <speaker> string  {} spk     {} {speaker   ID}  ] [
      list <uttID>   string  {} utt     {} {utterance ID}  ] [
      list -field    string  {} convDB  {} {conversation field in DB} ] [
      list -conv     string  {} conv    {} {conversation or episode} ] [
      list -channel  string  {} channel {} {channel}       ] [
      list -from     float   {} from    {} {start point}   ] [
      list -file     string  {} file    {} {filename}      ] [
      list -rate     int     {} rate    {} {rate}          ] [
      list -silsmb   string  {} silsmb  {} {symbol for opt sil} ] [
      list -warpA    string  {} warpA   {} {warpA}         ] ]

    # time warp to cover missing/removed frames for time segmentation
    if {$warpA != ""} {
	upvar \#0 $warpA wA
    }

    if {[lsearch [objects] db${SID}-spk] >= 0} {
	set       uttInfo  [uttDB uttInfo $spk $utt]
	makeArray uttArray $uttInfo

	if {[info exists uttArray(CHANNEL)] && $channel == ""} {
	    set channel $uttArray(CHANNEL) }
	if {[info exists uttArray($convDB)]    && $conv == ""}    {
	    set conv    $uttArray($convDB) }
    }
    if {$conv == ""} { set conv $spk }
    if {![info exists from] && [catch { set from $uttArray(FROM) }]} { set from 0 }
    if {![info exists from] && [catch { set from $uttArray(FROM) }]} { set from 0 }

    set result [${spass}.stab trace -v 2]
    set score  [lindex $result 1]
    set hypo   [lrange $result 2 end]
    set hypoN  [llength $hypo]
    set frameN [spass$SID configure -frameX]

    if { $file != "stdout" } { 
	set fp [open $file a] 
    } else { 
	set fp stdout 
    }
 
    puts $fp "# $utt $from $score"

    set oldscore 0.0
    set beg      $from
    for {set i 0} {$i < $hypoN} {incr i} { catch {
	set item   [lindex $hypo $i]
	set frX    [lindex $item 0]
	if [info exists wA($frX)] { set frX $wA($frX) }

	set end    [expr $from + 1.0*$frX/$rate]
	set len    [expr $end - $beg]
	set word   [lindex $item 1]
	set ascore [lindex $item 2]
	set wscore [expr $ascore - $oldscore]
	if {[string length $silsmb]} { regsub -all "\\$" $word $silsmb word }

        if { $word != "$" && $word != "(" && $word != ")" } { 
	    puts $fp  [format "%s %s %7.2f %7.2f %-20s %7.2f" \
			   $conv $channel $beg $len $word $wscore]
	}
	set oldscore $ascore
	set beg      $end
    } }

    if { $file != "stdout" } { 
	if {[catch { close $fp } msg]} {
	    writeLog stderr "spassWriteHypo: closing FP returned '$msg'"
	}
    }
}

SPass method writeCTM spassWriteHypo -text "write hypo in CTM format"


# ------------------------------------------------------------------------
#  write hypo from GLAT object
# ------------------------------------------------------------------------

proc glatWriteHypo {glat args} {
    global SID

    set svmap  svmap$SID
    set file   stdout
    set rate   [expr 1.0 / [$glat configure -frameShift]]
    set topX   0
    set topN   1
    set maxN   20
    set beam   50
    set v      0
    set result ""
    set warpA  ""
    set conf   0
    set lzz    0
    set silsmb ""
    set conv    ""
    set convDB  "CONV"
    set channel  1

    itfParseArgv glatWriteHypo $args [list [
      list <speaker> string  {} spk     {}    {speaker   ID}  ] [
      list <uttID>   string  {} utt     {}    {utterance ID}  ] [
      list -field    string  {} convDB  {}    {conversation field in DB} ] [
      list -conv     string  {} conv    {}    {conversation or episode}  ] [
      list -channel  string  {} channel {}    {channel}       ] [
      list -from     float   {} from    {}    {start point}   ] [
      list -file     string  {} file    {}    {filename}      ] [
      list -result   string  {} result  {}    {result from rescoring} ] [
      list -map      object  {} svmap   SVMap {SVMap}         ] [
      list -topX     int     {} topX    {}    {topX}          ] [
      list -topN     int     {} topN    {}    {topN}          ] [
      list -maxN     int     {} maxN    {}    {maxN}          ] [
      list -beam     float   {} beam    {}    {beam}          ] [
      list -rate     int     {} rate    {}    {rate}          ] [
      list -warpA    string  {} warpA   {}    {warpA}         ] [
      list -conf     int     {} conf    {}    {write confidences instead of scores} ] [
      list -lz       float   {} lzz     {}    {lz for confidences}         ] [
      list -silsmb   string  {} silsmb  {}    {symbol for opt sil}         ] [
      list -v        int     {} v       {}    {verbose}       ] ]

    # time warp to cover missing/removed frames for time segmentation
    if {$warpA != ""} {
	upvar \#0 $warpA wA
    }

    if {[lsearch [objects] db${SID}-spk] >= 0} {
	set       uttInfo  [uttDB uttInfo $spk $utt]
	makeArray uttArray $uttInfo

	if {[info exists uttArray(CHANNEL)] && $channel == ""} {
	    set channel $uttArray(CHANNEL) }
	if {[info exists uttArray($convDB)]    && $conv == ""}    {
	    set conv    $uttArray($convDB) }
    }
    if {$conv == ""} { set conv $spk }
    if {![info exists from] && [catch { set from $uttArray(FROM) }]} { set from 0 }
 
    if {$result == ""} {
	set result [lindex [${glat} rescore -map $svmap -topN $topN -maxN $maxN -beam $beam -v 1] $topX]
    }
    set score  [lindex $result 1]
    if {$conf} {
	if {$lzz <= 0} {set lzz [${svmap} configure -lz]}
	set tt ""
	foreach w [lrange $result 2 end] { set tt "$tt [lindex $w 0]" }
        set hypo   [${glat} confidence $tt -scale [expr 1.0 / $lzz] -v 1]
    } else {
	set hypo   [lrange $result 2 end]
    }
    set hypoN  [llength $hypo]

    if {$v == 1} { puts $result }
    if { $file != "stdout" } { 
	set fp [open $file a] 
    } else { 
	set fp stdout 
    }
 
    puts $fp "# $utt $from $score"

    for {set i 0} {$i < $hypoN} {incr i} { catch {
	set item   [lindex $hypo $i]
	set frX    [lindex $item 1]
	set frY    [lindex $item 2]
	if [info exists wA($frX)] { set frX $wA($frX) }
	if [info exists wA($frY)] { set frY $wA($frY) }
	set beg    [expr $from + 1.0*$frX/$rate]
	set end    [expr $from + 1.0*$frY/$rate]
	set len    [expr $end - $beg]
	set word   [lindex $item 0]
	set wscore [lindex $item 3]
	if {[string length $silsmb]} { regsub -all "\\$" $word $silsmb word }

        if { $word != "$" && $word != "(" && $word != ")" } { 
	    puts $fp  [format "%s %s %7.2f %7.2f %-20s %7.2f" \
			   $conv $channel $beg $len $word $wscore]
	}
    } }

    if { $file != "stdout" } { 
	if {[catch { close $fp } msg]} {
	    writeLog stderr "glatWriteHypo: closing FP returned '$msg'"
	}
    }
}

GLat method writeCTM glatWriteHypo -text "write hypo in CTM format"

proc glatWriteTRN {glat args} {
    global SID

    set svmap  svmap$SID
    set file   stdout
    set rate   100   
    set topX   0
    set topN   1
    set maxN   20
    set beam   50
    set v      0
    set result ""
    set warpA  ""

    itfParseArgv glatWriteTRN $args [list [
      list <speaker> string  {} spk    {}    {speaker   ID}  ] [
      list <uttID>   string  {} utt    {}    {utterance ID}  ] [
      list -from     float   {} from   {}    {start point}   ] [
      list -file     string  {} file   {}    {filename}      ] [
      list -result   string  {} result {}    {result from rescoring} ] [
      list -map      object  {} svmap  SVMap {SVMap}                 ] [
      list -topX     int     {} topX   {}    {topX}          ] [
      list -topN     int     {} topN   {}    {topN}          ] [
      list -maxN     int     {} maxN   {}    {maxN}          ] [
      list -beam     float   {} beam   {}    {beam}          ] [
      list -rate     int     {} rate   {}    {rate}          ] [
      list -warpA    string  {} warpA  {}    {warpA}         ] [
      list -time     int     {} v      {}    {include time information}       ] ]

    # time warp to cover missing/removed frames for time segmentation
    if {$warpA != ""} {
	upvar \#0 $warpA wA
    }

    if {[lsearch [objects] db${SID}-spk] >= 0} {
	set       uttInfo  [uttDB uttInfo $spk $utt]
	makeArray uttArray $uttInfo

	# support for callhome
	if [string match "en*" $spk] {
	    regexp {(.+)[_]([AB])} $spk dummy conv channel
	} else {
	    regexp {(.+)[-]([AB])} $spk dummy conv channel
	}
	if {[info exists uttArray(CONV)]}    { set conv    $uttArray(CONV) }
	if {[info exists uttArray(CHANNEL)]} { set channel $uttArray(CHANNEL) }
    }
    if {![info exists conv]} {set conv $spk; set channel A}
    if {![info exists from] && [catch { set from $uttArray(FROM) }]} { set from 0 }
    
    if {$result == ""} {
	set result [lindex [${glat} rescore -map $svmap -topN $topN -maxN $maxN -beam $beam -v 1] $topX]
    }
    set score  [lindex $result 1]
    set hypo   [lrange $result 2 end]
    set hypoN  [llength $hypo]

    if { $file != "stdout" } { 
	set fp [open $file a] 
    } else { 
	set fp stdout 
    }
 
    for {set i 0} {$i < $hypoN} {incr i} {
	set item   [lindex $hypo $i]
	set frX    [lindex $item 1]
	set frY    [lindex $item 2]
	if [info exists wA($frX)] { set frX $wA($frX) }
	if [info exists wA($frY)] { set frY $wA($frY) }

	set beg    [expr 1.0*$frX/$rate]
	set end    [expr 1.0*$frY/$rate]

	set len    [expr $end - $beg]
	set word   [lindex $item 0]
	set wscore [lindex $item 3]

        if { ![string compare $word "$"] || ![string compare $word "("] || 
	     ![string compare $word ")"] } { continue }

	if {$v} {
	    puts -nonewline $fp "{ $beg $end $word } "
	} else {
	    puts -nonewline $fp "$word "
	}
    }

    puts $fp "\t($utt)"
    if { $file != "stdout" } { 
	if {[catch { close $fp } msg]} {
	    writeLog stderr "glatWriteTRN: closing FP returned '$msg'"
	}
    }
}

GLat method writeTRN glatWriteTRN -text "write hypo in TRN format"

proc glatWriteSRT { glat args } {
    global SID SRT_COUNT

    set svmap  svmap$SID
    set file   stdout
    set rate   100   
    set topX   0
    set topN   1
    set maxN   20
    set beam   50
    set result ""
    set warpA  ""

    itfParseArgv glatWriteSRT $args [list [
      list <speaker> string  {} spk    {}    {speaker   ID}  ] [
      list <uttID>   string  {} utt    {}    {utterance ID}  ] [
      list -from     float   {} from   {}    {start point}   ] [
      list -file     string  {} file   {}    {filename}      ] [
      list -result   string  {} result {}    {result from rescoring} ] [
      list -map      object  {} svmap  SVMap {SVMap}                 ] [
      list -topX     int     {} topX   {}    {topX}          ] [
      list -topN     int     {} topN   {}    {topN}          ] [
      list -maxN     int     {} maxN   {}    {maxN}          ] [
      list -beam     float   {} beam   {}    {beam}          ] [
      list -rate     int     {} rate   {}    {rate}          ] [
      list -warpA    string  {} warpA  {}    {warpA}         ] [
    ] ]

    # time warp to cover missing/removed frames for time segmentation
    if {$warpA != ""} {
	upvar \#0 $warpA wA
    }
    if {![info exists SRT_COUNT]} { set SRT_COUNT 0 }

    if {[lsearch [objects] db${SID}-spk] >= 0} {
	set       uttInfo  [uttDB uttInfo $spk $utt]
	makeArray uttArray $uttInfo
	regexp {(.+)[_]([AB])} $spk dummy conv channel
	if {[info exists uttArray(CONV)]}    { set conv    $uttArray(CONV) }
	if {[info exists uttArray(CHANNEL)]} { set channel $uttArray(CHANNEL) }
    }
    if {![info exists conv]} {set conv $spk; set channel A}
    if {![info exists from] && [catch { set from $uttArray(FROM) }]} { set from 0 }
    
    if {$result == ""} {
	set result [lindex [${glat} rescore -map $svmap -topN $topN -maxN $maxN -beam $beam -v 1] $topX]
    }
    set score  [lindex $result 1]
    set hypo   [lrange $result 2 end]
    set hypoN  [llength $hypo]

    set t1     [clock format [expr int($from)] -format "%H:%M:%S" -gmt 1],[format "%03d" [expr int(fmod($from,1)*1000)]]
    set to     [expr $from + 1.0*[lindex [lindex $hypo end] 2]/$rate]
    set t2     [clock format [expr int($to)] -format "%H:%M:%S" -gmt 1],[format "%03d" [expr int(fmod($to,1)*1000)]]

    if { $file != "stdout" } { 
	set fp [open $file a] 
	fconfigure $fp -translation crlf
    } else { 
	set fp stdout 
    }
    puts $fp [incr SRT_COUNT]
    puts $fp "$t1 --> $t2"
 
    for {set i 0} {$i < $hypoN} {incr i} {
	set item   [lindex $hypo $i]
	set frX    [lindex $item 1]
	set frY    [lindex $item 2]
	if [info exists wA($frX)] { set frX $wA($frX) }
	if [info exists wA($frY)] { set frY $wA($frY) }

	set beg    [expr $from + 1.0*$frX/$rate]
	set end    [expr $from + 1.0*$frY/$rate]

	set len    [expr $end - $beg]
	set word   [lindex $item 0]
	set wscore [lindex $item 3]

        if { ![string compare $word "$"] || ![string compare $word "("] || 
	     ![string compare $word ")"] } { continue }

	puts -nonewline $fp "$word "
    }
    puts $fp ""; puts $fp ""

    if { $file != "stdout" } { 
	if {[catch { close $fp } msg]} {
	    writeLog stderr "glatWriteSRT: closing FP returned '$msg'"
	}
    }
}

GLat method writeSRT glatWriteSRT -text "write hypo in SRT format"

# ------------------------------------------------------------------------
#  Confusion network compression
# ------------------------------------------------------------------------

proc createCNet {glatOut args } {
    global SID
    set    optWord "\$"
    set    factor  0.03
    set    beam    10

    itfParseArgv  createCNet $args [list [
      list <GLat> object  {} glatIn GLat {} {GLat to convert} ] [
      list -optWord  string {} optWord {}   {optional word}   ] [
      list -factor   float  {} factor  {}   {mystic scaling factor} ] [
      list -beam     float  {} beam  {}     {posteriori beam} ] ]

    if {[$glatIn configure -nodeN] < 1} {
	writeLog stderr "createCNet : empty lattice"
	return
    }
  
    $glatIn posteriori -scale $factor
    set txt [lindex [$glatIn consensus -map svmap$SID -beam $beam] 0]
    regsub -all {<UNK>} $txt $optWord txt
    $glatOut clear

    set fromL {-1}
    set frX 0
    set frY 1
    foreach e $txt {
	set nextL ""
	foreach f $e {
	    if {$f == "/"} { continue }
	    if {$f == "@" || $f == $optWord} { 
		set realf $optWord 
	    } else {
		set realf [lindex [lindex [svmap$SID mappedto $f] 1] 0]
	    }
	    set nodeX [$glatOut addNode $realf $frX $frY]
	    foreach l $fromL {
		$glatOut addLink $l $nodeX
	    }
	    lappend nextL $nodeX
	}
	set fromL $nextL
	set nextL ""
	incr frX
	incr frY
    }
    foreach l $fromL { $glatOut addLink $l -2 }    
}

GLat method createCN createCNet -text "convert lattice into confusion network"

# ------------------------------------------------------------------------
#  convert GLat into HMM
# ------------------------------------------------------------------------

proc hmmConvertGLat {hmmOut args } {
    global SID
    set optWord "\$"
    set variants 1

    itfParseArgv hmmConvertGLat  $args [list [
	list <GLat> object  {} glatIn GLat {} {GLat to convert} ] ]

    if {[$glatIn configure -nodeN] < 1} {
	writeLog stderr "hmmConvertGLat : empty lattice"
	return
    }
    $glatIn posteriori -scale 0.1
    set txt [lindex [$glatIn consensus -map svmap$SID -beam 10] 0]
    regsub -all {<s>|</s>|<UNK>} $txt "$" txt
    foreach e $txt {
	lappend txt2 [join $e " / "]
    }

    if {! [string length [info command textGraph]]} {
	TextGraph textGraph
    }

    textGraph make $txt2 -optWord $optWord -svmap svmap$SID
    array set  HMM [textGraph]
    set words $HMM(STATES)
    set trans $HMM(TRANS)
    set init  $HMM(INIT)
    $hmmOut make $words -trans $trans -init $init -variants $variants
}

HMM method convert hmmConvertGLat -text "convert GLat into HMM object"

# ------------------------------------------------------------------------
#  ngramLMMatch
# --------------
#  Identify the best match for this word
# ------------------------------------------------------------------------

proc ngramLMMatch { svmap args } {
    global SID

    set variants 0

    itfParseArgv ngramLMMatch $args [list [
        list <lm>      object  {} lm  {LingKS} {lm to use}      ] [
        list <words>   string  {} words     {} {words to find}  ] [
        list <text>    string  {} file      {} {text to use in file} ] [
	list -variants int     {} variants  {} {include variants in search} ] ]
    
    
    # Get our candidates
    if {$variants} {
	set candidates ""
	foreach w [${lm}.NGramLM:] {
	    if {[regexp "<" $w] } { continue }
	    set c [lrange [svmap$SID mappedto $w] 1 end]
	    if {[string length $c]} { set candidates "$candidates $c" }
	}
    } else {
	foreach w [${lm}.NGramLM:] {
	    if {[regexp "<" $w] } { continue }
	    set c [lindex [svmap$SID mappedto $w] 1]
	    if {[string length $c]} { lappend candidates $c }
	}
    }

    # Read the file
    set fp [open $file r]
    while {[gets $fp line] != -1} { lappend txt $line }
    close $fp

    # Loop over all words in the list
    foreach w $words {
	set test ""

	# Delete pronunciation variants
	regsub -all {\(.*} $w "" w

	# Find all usable lines of text
	for {set i 0} {$i < [llength $txt]} {incr i} {
	    if {[set j [lsearch [set line [lindex $txt $i]] $w]] == -1} { continue }
	    lappend test "( [lrange [lreplace $line $j $j ixXxi] [expr $j-2] [expr $j+2]] )"
	}

	# No examples found
	if {![string length $test]} {
	    lappend matches <UNK>
	    continue
	}

	puts "$w -> $test"

	# Search for the best match
	set m <UNK>
	set s 9999999.9
	foreach W $candidates {
	    if {[catch {
		set S 0.0
		foreach t $test {
		    set S [expr $S + [$lm score [regsub "ixXxi" $t $W] -map $svmap]]
		}
	    } msg]} {
		continue
	    }
	    if {$S < $s} {
		set s $S
		set m $W
	    }
	}

	lappend matches [lindex [svmap$SID get $m] 1]
    }

    return $matches
}

SVMap method match ngramLMMatch -text "find best match for word"
