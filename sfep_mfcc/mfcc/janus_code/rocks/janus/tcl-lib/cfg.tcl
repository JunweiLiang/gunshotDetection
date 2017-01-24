# ========================================================================
#  JANUS-SR   Janus Speech Recognition Toolkit
#             ------------------------------------------------------------
#             Object: Procedures for Context Free Grammars (CFG)
#             ------------------------------------------------------------
#
#  Author  :  Christian Fuegen
#  Module  :  cfgLib.tcl
#  Date    :  $Id: cfg.tcl 2470 2004-07-23 18:02:28Z fuegen $
#
#  Remarks :  This file contains procedures for using
#             Context Free Grammars together with the
#             New Janus Decoder called "IBIS"
#
# ========================================================================
#
#  $Log$
#  Revision 1.3  2004/07/23 18:02:28  fuegen
#  - added procs for weighting rules to support a tight coupling of
#    speech recognition and dialogue management
#  - added proc for printing rules in the AT&T FSM format
#  - added procs for generating terminal sequences for CFGs and CFGSets
#
#  Revision 1.2  2003/08/14 11:20:26  fuegen
#  Merged changes on branch jtk-01-01-15-fms (jaguar -> ibis-013)
#
#  Revision 1.1.2.9  2003/08/13 14:15:45  fuegen
#  added some procedure description
#
#  Revision 1.1.2.8  2003/08/13 08:55:27  fuegen
#  startover requires now float factor
#
#  Revision 1.1.2.7  2003/08/04 16:00:03  fuegen
#  adding of probabilties to svmap if specified
#  string tolower
#
#  Revision 1.1.2.6  2003/07/25 09:45:16  fuegen
#  update to match executable
#
#  Revision 1.1.2.5  2003/06/23 13:12:45  fuegen
#  made it compatible to new CFG implementation
#
#  Revision 1.1.2.4  2002/11/18 17:39:05  fuegen
#  minor bugfixes for grammar and one4all support
#
#  Revision 1.1.2.3  2002/11/18 15:14:06  fuegen
#  makes also a map file
#
#  Revision 1.1.2.2  2002/11/18 08:42:14  fuegen
#  added activation and deactivation functions
#  removed some bugs from cfgGetParseTree
#
#  Revision 1.1.2.1  2002/11/15 08:57:23  fuegen
#  Initial version
#
#
# ========================================================================

proc cfg_Info { name } {
    switch $name {
	procs {
	    return "cfgInit cfgGetParseTree cfgActivate cfgDeactivate cfgSetWeightRules cfgSetWriteFSM cfgSetGenerate cfgGenerate"
	}
	help {
	    return "This file contains procedures for using Context Free Grammars together with the Ibis decoder. The grammars can be initialized by calling 'cfgInit \$SID' as usual. Other procedures are provided as methods of various objects."
	}
	cfgInit {
	    return "Initializes the CFGs. By using the option '-makeDict' and defininig a base dictionary, it is also possible to build a new dictionary limited to the words given by the CFGs."
	}
	cfgGetParseTree {
	    return "Returns the parse tree of a given text string. This method is case sensitive!"
	}
	cfgActivate {
	    return "Sets the status of all grammars to active, which match the given tag. The tags 'SHARED' and 'all' are reserved"
	}
	cfgDeactivate {
	    return "Sets the status of all grammars to inactive, which match the given tag. The tags 'SHARED' and 'all' are reserved"
	}
	cfgSetWeightRule {
	    return "Weights all given rules by a given value"
	}
	cfgSetWriteFSM {
	    return "Writes a rule in a CFG or CFGSet in AT&T FSM format"
	}
	cfgSetGenerate {
	    return "Generates terminal sequences for a CFGSet"
	}
	cfgGenerate {
	    return "Generates terminal sequences for a CFG"
	}
    }

    return "???"
}

# ----------------------------------------------------------------------
# cfgInit
# initialization of CFG objects
# ----------------------------------------------------------------------
proc cfgInit { LSID args } {
    upvar \#0 $LSID SID

    # default settings
    set classes    ""
    set fillers    ""
    set startover -1.0
    set makeDict   0

    foreach i [list grammars baseDict dict vocab classes fillers] {
	if { [info exists SID(cfg,$i)] } { eval "set $i {$SID(cfg,$i)}" }
    }

    itfParseArgv cfgInit $args [list [
      list -grammars     string {} grammars  {} {list of grammars and tags} ] [
      list -baseDict     string {} baseDict  {} {base dict for lookup}      ] [
      list -dict         string {} dict      {} {resulting new dict}        ] [
      list -classes      string {} classes   {} {mapping of classes}        ] [
      list -fillers      string {} fillers   {} {list of filler words}      ] [
      list -startover    float  {} startover {} {allow starting over}       ] [
      list -makeDict     int    {} makeDict  {} {make dict out of cfg}      ] ]

    # checking
    if { ![info exists grammars] } {
	error "Couldn't build CFGs, because of missing grammars files."
    }

    if { $makeDict && (![info exists baseDict] || ![info exists dict]) } {
	error "Couldn't make dictionary, because of missing base/result dict."
    }

    # create linguistic knowledge source
    LingKS cfgSet$LSID CFGSet

    # create and load grammars
    makeArray cfgA $grammars

    foreach tag [array names cfgA] {
	CFG cfg$tag -lks cfgSet$LSID -tag $tag

	foreach file $cfgA($tag) {
	    cfg$tag load $file
	}

	if { $startover >= 0 } {
	    cfg$tag configure -startover $startover
	}
    }

    # build grammar structures
    cfgSet$LSID.data build

    # create dictionary out of grammar
    if { $makeDict } {
	cfgSet$LSID.data makeDict $baseDict $dict \
	    -classes $classes -fillers $fillers
    }

    return cfgSet$LSID
}

# ----------------------------------------------------------------------
# cfgMakeDict
# produces a dictionary out of a defined vocabulary
#
# the vocabulary is build out of the base forms of
#	- cfg vocabulary
#	- noises
#	- class elements refered by cfg vocabulary
# ----------------------------------------------------------------------
proc cfgMakeDict { cfgSet args } {

    set classes ""
    set fillers ""

    itfParseArgv cfgMakeDict $args [list [
      list <baseDict>     string {} baseDict {} {base dict for lookup}      ] [
      list <dict>         string {} dict     {} {resulting new dict}        ] [
      list -vocab         string {} vocab    {} {resulting search vocab}    ] [
      list -map           string {} map      {} {resulting mapping file}    ] [
      list -classes       string {} classes  {} {mapping of classes}        ] [
      list -fillers       string {} fillers  {} {list of filler words}      ] ]

    if { $fillers != "" } {
	if { ![info exists vocab] } { set vocab $dict.v }
	if { ![info exists map]   } { set map   $dict.m }
    }

    putsInfo "creating new dictionary out of grammar"

    # get cfg lexicon
    set tags [cfgGetLexicon $cfgSet vocabA]

    # get classes
    cfgGetClasses $classes $tags vocabA tagA

    if { $fillers != "" } {
	# get filler words
	cfgGetFillers $fillers fillerA
    }

    # build dictionary and store it into file
    return [cfgWriteDict $baseDict $dict $vocab $map vocabA tagA fillerA]
}

CFGSet method makeDict cfgMakeDict -text "makes a dictionary out of a base dictionary limited to the word entries of the CFG"

proc cfgGetLexicon { cfgSet vocabA } {
    upvar     $vocabA vA

    set vL   [$cfgSet.lex]
    set tags {}

    foreach v $vL {

	# skip begin and end of sentence
	if { [regexp {<?s>} $v] } { continue }

	# extract class tags out of lexicon
	if { [regexp {^@} $v] } {
	    lappend tags $v
	} else {
	    set vA($v) 1
	}
    }

    return $tags
}

proc cfgGetClasses { classes tags vocabA tagA } {
    upvar     $vocabA vA
    upvar     $tagA   tA

    set n 0
    
    # build tags array for easy checking
    foreach t $tags {
	set tmpA($t) 1
    }

    foreach file $classes {

	if { [catch {set fp [open $file r]} msg] } {
	    putsError "$msg"
	    return $n
	}

	while { [gets $fp line] >= 0 } {
	    set word [lindex $line 0]
	    set tag  [lindex $line 1]
	    set prob [lindex $line 2]

	    if { [info exists tmpA($tag)] } {
		set vA($word) 1
		set tA($word) $tag
		if { $prob != "" } { set tA($word,prob) $prob }
		incr n
	    }
	}
	close $fp
    }

    return $n
}

proc cfgGetFillers { fillers fillerA } {
    upvar $fillerA fA

    set n  0

    foreach file $fillers {
	if { [catch {set fp [open $file r]} msg] } {
	    putsError "$msg"
	    return $n
	}

	while { [gets $fp line] >= 0 } {
	    set word [lindex $line 0]
	    set fA($word) 1
	    incr n
	}
	close $fp
    }

    return $n
}

proc cfgWriteDict { baseDict dict vocab map vocabA tagA fillerA } {
    upvar     $vocabA  vA
    upvar     $tagA    tA
    upvar     $fillerA fA

    set n   0
    set dN  0
    set fpI [open $baseDict r]
    set fpO [open $dict     w]

    if { $vocab != "" } {
	set fpV [open $vocab w]
    }

    if { $vocab != "" } {
	set fpM [open $map w]
    }

    while { [gets $fpI line] >= 0 } {
	set word [lindex $line 0]
	set pron [lrange $line 1 end]

	if { [info exists dA($word)] } { incr dN; continue }
	set dA($word) 1

	if { [info exists fA($word)] } {
	    # word is in filler array
	    puts $fpO $line

	    if { [info exists fpV] } { puts $fpV "$word 1" }

	    set fA($word) 0
	    incr n
	} elseif { [regexp {^\$$} $word] } {
	    # word is a silence
	    puts $fpO $line

	    if { [info exists fpV] } { puts $fpV "$word 1" }

	    incr n
	} elseif { [regexp {^[()]$} $word] } {
	    # word is begin or end of sentence
	    puts $fpO $line

	    if { [info exists fpV] } { puts $fpV "$word" }

	    incr n
	} else {
	    # word is a regular word, so extract base form
	    regsub -all {\(.*\)} $word {} base

	    # take only words with matching base form
	    if { [info exists vA($base)] } {
		puts $fpO $line

		if { [info exists fpV] } { puts $fpV "$word" }
		if { [info exists fpM] && [info exists tA($base)] } {
		    if { [info exists tA($base,prob)] } {
			puts $fpM "$word $tA($base) -prob $tA($base,prob)"
		    } else {
			puts $fpM "$word $tA($base)"
		    }
		}

		set vA($base) 0
		incr n
	    }
	}
    }
    close $fpO
    close $fpI

    if { [info exists fpV] } { close $fpV }
    if { [info exists fpM] } { close $fpM }

    # check if we have all words
    set vL [lsort [array names fA]]
    foreach v $vL {
	if { $fA($v) } { putsWarn "no dictionary entry found for '$v'" }
    }

    set vL [lsort [array names vA]]
    foreach v $vL {
	if { $vA($v) } { putsWarn "no dictionary entry found for '$v'" }
    }
    
    if { $dN } { putsWarn "$dN doubles with different prons in $baseDict" }

    return $n
}

# ----------------------------------------------------------------------
# cfgGetParseTree
# returns the parse tree of a text string
# ----------------------------------------------------------------------
proc cfgGetParseTree { cfgSet args } {

    set svmap  ""
    set auxNT  0
    set format soup

    itfParseArgv cfgGetParseTree $args [list [
      list <text>   string {} text   {}     {text string to parse}        ] [
      list -svmap   object {} svmap  SVMap  {use SVMap to map SVX<->LVX}  ] [
      list -format  string {} format {}     {output format (soup|jsgf)}   ] [
      list -auxNT   int    {} auxNT  {}     {print also auxilliary NTs}   ] ]

    if { [llength [info command $svmap]] } {
	# use svmap to extract lmwords (get also class tags)
	set hypo $text
	set text ""
	foreach word $hypo {
	    if { [catch {$svmap get $word} map] } {
		set w [string tolower $word]
		if { $word == $w || [catch {$svmap get $w} map] } {
		    putsError "no mapping found for '$hypo'."
		    return {}
		}
	    }

	    set base [lindex $map 1]

	    if { ![string compare $base "<UNK>"] } { continue }

	    if { [regexp {^@} $base] } {
		lappend mapA($base) [lindex $map 0]
	    }
	    set text "$text $base"
	}

	# normalizations
	regsub -all {<s>}  $text {}  text
	regsub -all {</s>} $text {}  text
	regsub -all { +}   $text { } text
	regsub      {^ }   $text {}  text
	regsub      { $}   $text {}  text

	putsDebug "HYPO '$hypo'"
    }
    putsDebug "TEXT '$text'"

    # parse text and get last node index (just a temporary workaround)
    if { [catch {$cfgSet parse $text} idx] } {
	putsError "no parse found for '$text'."
	return {}
    }

    # trace back to get parse tree
    set tree [$cfgSet.pt.node($idx) trace -format $format -auxNT $auxNT]

    if { [llength [info command $svmap]] } {
	# remap words to class tags
	foreach tag [array names mapA] {
	    foreach w $mapA($tag) {
		regsub $tag $tree $w tree
	    }
	}
    }

    return $tree
}

CFGSet method parseTree cfgGetParseTree -text "returns the parse tree of a given text string"
CFG    method parseTree cfgGetParseTree -text "returns the parse tree of a given text string"

# ----------------------------------------------------------------------
# cfgActivate, cfgDeactivate
# activate or deactivate grammars, given by tags
# ----------------------------------------------------------------------
proc cfgActivate { cfgSet args } {

    set tag ""
    set n   0

    itfParseArgv cfgActivate $args [list [
      list <tag>    string {} tag    {}     {tag of the grammar}        ] ]

    set cfgN [$cfgSet configure -cfgN]

    if { ![string compare $tag "all"] } {
	for {set i 0} {$i < $cfgN} {incr i} {
	    if { [string compare [$cfgSet.cfg($i) configure -tag] "SHARED"] } {
		$cfgSet.cfg($i) configure -status ACTIVE
		incr n
	    }
	}
    } else {
	if { ![string compare $tag "SHARED"] } {
	    error "Can't change status of a shared grammar"
	}

	for {set i 0} {$i < $cfgN} {incr i} {
	    if { ![string compare [$cfgSet.cfg($i) configure -tag] $tag] } {
		$cfgSet.cfg($i) configure -status ACTIVE
		incr n
	    }
	}
    }

    return $n
}

proc cfgDeactivate { cfgSet args } {

    set tag ""
    set n   0

    itfParseArgv cfgDeactivate $args [list [
      list <tag>    string {} tag    {}     {tag of the grammar}        ] ]

    set cfgN [$cfgSet configure -cfgN]

    if { ![string compare $tag "all"] } {
	for {set i 0} {$i < $cfgN} {incr i} {
	    if { [string compare [$cfgSet.cfg($i) configure -tag] "SHARED"] } {
		$cfgSet.cfg($i) configure -status INACTIVE
		incr n
	    }
	}
    } else {
	if { ![string compare $tag "SHARED"] } {
	    error "Can't change status of a shared grammar"
	}

	for {set i 0} {$i < $cfgN} {incr i} {
	    if { ![string compare [$cfgSet.cfg($i) configure -tag] $tag] } {
		$cfgSet.cfg($i) configure -status INACTIVE
		incr n
	    }
	}
    }

    return $n
}

CFGSet method activate   cfgActivate   -text "activates a grammar given by tag"
CFGSet method deactivate cfgDeactivate -text "deactivates a grammar given by tag"

# ----------------------------------------------------------------------
# cfgSetWeightRules
# ----------------------------------------------------------------------
proc cfgSetWeightRules { cfgSet args } {

    set rules  ""
    set weight 0.00

    itfParseArgv cfgSetWeightRules $args [list [
      list <rules>    string {} rules    {}     {list of rules}               ] [
      list -weight    float  {} weight   {}     {weight apllied to all rules} ] ]

    set cfgN [$cfgSet configure -cfgN]
    set n    0
    foreach rule $rules { set ruleA($rule) 1 }

    for {set i 0} {$i < $cfgN} {incr i} {
	set cfg $cfgSet.cfg($i)

	for {set j 0} {$j < [$cfg.bos configure -arcN]} {incr j} {
	    set rule [lindex [$cfg.bos.arc($j) puts] 0]

	    if { [info exists ruleA($rule)] || [info exists ruleA(_ALL_)] } {
		$cfg.bos.arc($j) configure -score $weight
		incr n
	    }
	}
    }

    return $n
}

CFGSet method weightRules cfgSetWeightRules -text "applies a weight to the given list of entry rules"

# ----------------------------------------------------------------------
# cfgSetWriteFSM
# ----------------------------------------------------------------------
proc cfgSetWriteFSM { cfgSet args } {

    itfParseArgv cfgWriteFSM $args [list [
      list <rule>    string {} rule    {}     {rule to print as FSM} ] [
      list <fsm>     string {} fsm     {}     {fsm file}             ] ]

    # write fsm
    if { [catch {
	set fp [open $fsm.stxt w]
	puts -nonewline $fp [$cfgSet:$rule puts -format fsm]
	close $fp
    } msg] } {
	error "can't get/write initial fsm $fsm: $msg"
    }


    # write lexicon (syms)
    set lexT  [$cfgSet.lex puts -type T_Arc]
    set lexNT [$cfgSet.lex puts -type NT_Arc]
    set lex   [concat "(null)" $lexT $lexNT]

    set i     0
    set fp    [open $fsm.syms w]
    foreach l $lex { puts $fp "$l $i"; incr i }
    close $fp

    # produce FSM
    if { [catch {exec cat $fsm.stxt | fsmcompile -i$fsm.syms            > $fsm.fsa} msg ] } {
	error "can't compile fsm $fsm: $msg"
    }

    if { [catch {exec cat $fsm.fsa  | fsmdraw    -i$fsm.syms | dot -Tps > $fsm.ps} msg] } {
	error "can't draw fsm $fsm: $msg"
    }
}

CFGSet method fsm cfgSetWriteFSM -text "write a FSM for a given rule"
CFG    method fsm cfgSetWriteFSM -text "write a FSM for a given rule"

# ----------------------------------------------------------------------
# cfgGenerate / cfgSetGenerate
# ----------------------------------------------------------------------
proc cfgGenerate { cfg args } {

    set seqN    0
    set mode    "random"
    set recurse 0
    set file    ""

    itfParseArgv cfgGenerate $args [list [
      list <seqN>    int    {} seqN     {} {number of terminal sequences}   ] [
      list -mode     string {} mode     {} {generation mode (random|fixed)} ] [
      list -recurse  int    {} recurse  {} {follow recursions}              ] [
      list -file     string {} fileName {} {file name to write output}      ] ]

    # generate
    if { $file != "" } {
	$cfg.root generate $seqN -mode $mode -recurse $recurse -fileName $file
    } else {
	$cfg.root generate $seqN -mode $mode -recurse $recurse
    }
}

proc cfgSetGenerate { cfgSet args } {

    set seqN    0
    set mode    "random"
    set recurse 0
    set file    ""

    itfParseArgv cfgSetGenerate $args [list [
      list <seqN>    int    {} seqN     {} {number of terminal sequences}   ] [
      list -mode     string {} mode     {} {generation mode (random|fixed)} ] [
      list -recurse  int    {} recurse  {} {follow recursions}              ] [
      list -file     string {} fileName {} {file name to write output}      ] ]

    set cfgN [$cfgSet configure -cfgN]
    set sN   [expr $seqN / $cfgN]

    for {set i 0} {$i < $cfgN} {incr i} {
	set cfg $cfgSet.cfg($i)

	cfgGenerate $cfg $sN -mode $mode -recurse $recurse -file $fileName
    }
}

CFG    method generate cfgGenerate    -text "generate terminal sequences"
CFGSet method generate cfgSetGenerate -text "generate terminal sequences"

