# ========================================================================
#  JANUS-SR   Janus Speech Recognition Toolkit
#             ------------------------------------------------------------
#
#  Author  :  Christian Fuegen
#  Module  :  set of procedures for the modalities
#  Date    :  07.01.99
#  Time-stamp: <10 May 00 15:35:34 soltau>
#  $Log$
#  Revision 1.8  2003/08/14 11:20:29  fuegen
#  Merged changes on branch jtk-01-01-15-fms (jaguar -> ibis-013)
#
#  Revision 1.7.50.1  2003/06/06 09:43:59  metze
#  Added latview, fixed upvar, updated tclIndex
#
#  Revision 1.7  2000/09/10 12:18:42  hyu
#  Added Tcl files from Hagen
#
#  Revision 1.6  2000/05/10 13:36:06  soltau
#  putsError -> puts stderr
#
#  Revision 1.5  2000/04/18 10:00:28  soltau
#  set global parameter USE_MODALITY
#
#  Revision 1.4  2000/04/18 08:12:28  soltau
#  removed frameN arguments
#
#  Revision 1.3  2000/02/07 08:10:12  soltau
#  don't load labelfile in hmmMakeUtterance
#
#  Revision 1.2  2000/02/01 09:56:25  soltau
#  Changed hardcoded trl keys
#
#  Revision 1.1  2000/02/01 09:35:39  soltau
#  Initial revision
#
# ========================================================================

# ===========================================================================
# ========              D E S C R I P T I O N                        ========
# ===========================================================================
# - procedures for initializing the modalities:
#   for initializing the modalities you need
#   1. dummy distributions for the modTree
#   2. the modtree (sub trees only with questions over modalities)
#   3. a modality set with all modalities
#   4. a state table, for looking up the ds indices to calculate the scores
# - procedures for the calculation of the modalities:
#   these procedures were used by the update procedures for automatic
#   calculation of the modalities. You can also use these procedures
#   without the update procedures
# - procedures for the training of the recognizer
#   firstable there are some procedures listed, which can be used
#   to get back the tags for one utterance, f.e. to use
#   with the construction of a ptree.
#   There are also other procedures listet.
# - form of the modality array
#   set modalityA(modality) [list updateProc tagName updateMode]
# - you will find my training scripts in ~/diplomarbeit/modalityVM/scriptPool
#
# -----------------------------------
# initializing sequence (overview)
# -----------------------------------
#
# be sure that 'QuestionSet configure -tagOperation' == 1
#
# phonesSetInit    $SID
# tagsInit         $SID
# featureSetInit   $SID
# vtlnInit         $SID
# codebookSetInit  $SID
# distribSetInit   $SID
# 
# if { $modalities } {
#
# for testing 
#     modFeatureSetInit     $SID
#     modalitySetInit       $SID -addTags 1 -modalityA $modalityArr
#     modTreeInit           $SID -desc $tree.mod.gz
#     stateTableInit        $SID
#     set ${SID}(distribTreeDesc) $tree.main.gz
#
# for transforming tree
#     # increase blkSize, blkSize must be greater than nodes in tree.
#     # if you don't increase it, you will get ERRORs in treeTransform
#     # (node XYZ without father, successors ...) for a node with index
#     # equal blkSize
#     Tree configure -blkSize 50000
#     modFeatureSetInit     $SID
#     modTreeInit           $SID -tree modTree$SID  -desc ""
#     modTreeInit           $SID -tree mainTree$SID -desc ""
#     modalitySetInit       $SID -addTags 1 -modalityA $modalityArr
# 
# for training
#     modFeatureSetInit     $SID
#     modalitySetInit       $SID -addTags 1 -modalityA $modalityArr
#
# }
# 
# distribTreeInit  $SID
#
# only the first time (after that you can write new distrib and the first modTree)
# if { $modalities } {
# for transforming tree 
#     distribTreeTransform  $SID -mainDesc $mainDesc -modDesc $modDesc
# }
# 
# senoneSetInit   $SID
# topoSetInit     $SID
# ttreeInit       $SID
# dictInit        $SID
#
# if { $modalities } {
# for clustering
#     questionSetAddTags    $SID
# }
# ....
#


# ===========================================================================
# CONFIGURATION
# ===========================================================================

set MODCFG(dummyName) dummyDs
set MODCFG(cbFeature) LDA


# ===========================================================================
# procedures for initializing the modalities
# ===========================================================================

# initialize the featureSet to get the frameN for an utterance for modalities
proc modFeatureSetInit { LSID args } {
    global updateA
    upvar  \#0 $LSID SID

    set featureSet       modFeatureSet$LSID
    set log              stderr

    if { [info exists SID(modFeatureSetDesc)] } {
	set featureSetDesc   $SID(modFeatureSetDesc)
    }

    if { [info exists SID(modFeatureSetAccess)] } {
	set featureSetAccess $SID(modFeatureSetAccess)
    }

    itfParseArgv featureSetInit $args [list [
      list -featureSet string  {} featureSet       {} {feature set name}      ] [
      list -desc       string  {} featureSetDesc   {} {description procedure} ] [
      list -access     string  {} featureSetAccess {} {access function}       ] [
      list -log        string  {} log              {} {name of log channel}   ] ]

    if { [llength [info command $featureSet]] < 1} {

	FeatureSet $featureSet

	if { [info exists featureSetDesc] && [string length $featureSetDesc] } { 
	    writeLog $log "$featureSet  desc   $featureSetDesc"
	    $featureSet setDesc   @$featureSetDesc
	    set SID(featureSetDesc)   $featureSetDesc
	}

	if { [info exists featureSetAccess] && [string length $featureSetAccess] } { 
	    writeLog $log "$featureSet  access $featureSetAccess"
	    $featureSet setAccess   @$featureSetAccess
	    set SID(featureSetAccess) $featureSetAccess
	}
    }

    set updateA(featureSet) $featureSet

    return $featureSet
}

# initialize dummy distribution tree
proc modTreeInit { LSID args } {
    global MODCFG
    upvar  \#0 $LSID SID

    set modTree     modTree$LSID
    set modalitySet modalitySet$LSID
    set phonesSet   phonesSet$LSID
    set phones      phonesSet$LSID:PHONES
    set distribSet  distribSet$LSID
    set tags        tags$LSID
    set codebookSet codebookSet$LSID
    set featureSet  featureSet$LSID
    set dummyName   $MODCFG(dummyName)
    set log         stderr

    if { [info exists SID(modTreeDesc)] } { 
	set modTreeDesc $SID(modTreeDesc)
    }

    itfParseArgv modTreeInit $args [list [
      list -tree        string {} modTree       {}      {modTree object}          ] [
      list -phones      object {} phones     Phones     {phones set}              ] [
      list -phonesSet   object {} phonesSet  PhonesSet  {phonesSet set}           ] [
      list -distribSet  object {} distribSet DistribSet {distribution set}        ] [
      list -tags        object {} tags       Tags       {tags set}                ] [
      list -codebookSet object {} codebookSet CodebookSet {codebook set}          ] [
      list -featureSet  object {} featureSet  FeatureSet  {feature set}           ] [
      list -dummyName   string {} dummyName     {}        {name of dummy distribs}] [
      list -desc        string {} modTreeDesc   {}      {description file}        ] [
      list -log         string {} log           {}      {name of log channel}     ] ]


    if { [llength [info command $distribSet]] &&
	 [llength [info command $modTree]] < 1} {
	
	Tree $modTree $phones $phonesSet $tags $distribSet
    }
	
    if { [info exists modTreeDesc] &&
	 [string length $modTreeDesc]} {

	addDummyDs $LSID $modTreeDesc -distribSet $distribSet \
	    -codebookSet $codebookSet -featureSet $featureSet \
	    -dummyName $dummyName -log $log
	writeLog $log "$modTree    read $modTreeDesc"
	$modTree read $modTreeDesc
	set SID(modTreeDesc) $modTreeDesc
    }

    if { [llength [info command $modalitySet]] } {
	$modalitySet configure -tree $modTree
    }

    return $modTree
}

# initialize modality set and modalities
# modalityA(modality) = {updateProc tagName updateMode}
proc modalitySetInit { LSID args } {
    global updateA  USE_MODALITY
    upvar  \#0 $LSID SID

    set modalitySet modalitySet$LSID
    set modTree     modTree$LSID
    set tags        tags$LSID
    set addTags     0
    set timeInfo    0
    set log         stderr

    if { [info exists SID(modDbasePath)] && [info exists SID(modDbaseName)] } {
	set modDB $SID(modDbasePath)/$SID(modDbaseName)
    }

    itfParseArgv modalitySetInit $args [list [
      list -modalitySet string {} modalitySet   {}      {modality set}            ] [
      list -modTree     object {} modTree     Tree      {modTree object}          ] [
      list -tags        object {} tags        Tags      {tags set}                ] [
      list -addTags     int    {} addTags       {}      {add tags to tags object} ] [
      list -modalityA   string {} modalityA     {}      {array of modalities}     ] [
      list -modDB       string {} modDB         {}      {dbase of modalities}     ] [
      list -timeInfo    int    {} timeInfo      {}   {calculates time for update} ] [
      list -log         string {} log           {}      {name of log channel}     ] ]

    if { [llength [info command $modalitySet]] < 1 } {
#	ModalitySet $modalitySet $modTree $tags -addTags $addTags
	ModalitySet $modalitySet $tags -addTags $addTags
    }

    if { [info exists modalityA] } {
	upvar $modalityA modA

	set names [array names modA]
	if { ![llength $names] } {
	    puts stderr "WARN modalitySetInit: no modality in $modalityA"
	}

	foreach modality $names {
	    $modalitySet add $modality [lindex $modA($modality) 0] \
		[lindex $modA($modality) 1]
	    ${modalitySet}:$modality configure \
		-updateMode [lindex $modA($modality) 2] \
		-timeInfo $timeInfo
	}

	writeLog $log "$modalitySet    read from $modalityA ([llength $names])"
	set updateA(modalityN) [llength $names]
    } else {
	error "ERROR modalitySetInit: no modalityA defined"
    }

    if { [info exists modDB] } {
	DBase modDB$LSID
	if { [file exists $modDB.dat] && [file exists $modDB.idx] } {
	    modDB$LSID open $modDB.dat $modDB.idx -mode r
	} else {
	    puts stderr "Can't open $modDB"
	}
	set updateA(modDB) modDB$LSID
    }
    set USE_MODALITY 1

    return $modalitySet
}

# initialize state table
proc stateTableInit { LSID args } {
    upvar \#0 $LSID SID

    set stateTable  stateTable$LSID
    set modalitySet modalitySet$LSID
    set distribSet  distribSet$LSID
    set log         stderr

    itfParseArgv stateTableInit $args [list [
      list -stateTable  string {} stateTable  {}          {state table}          ] [
      list -modalitySet object {} modalitySet ModalitySet {modality set}         ] [
      list -distribSet  object {} distribSet  DistribSet  {distribution set}     ] [
      list -log         string {} log         {}          {name of log channel}  ] ]

    writeLog $log "$stateTable    initialization"

    if { [llength [info command $stateTable]] < 1 } {
	StateTable $stateTable $modalitySet
    }

    $stateTable create

    if { [llength [info command $distribSet]] } {
	$distribSet configure -stateTable $stateTable
    }

    return $stateTable
}


# add questions after modality-tags to questionSet
proc questionSetAddTags { LSID args } {
    upvar \#0 $LSID SID

    set questionSet questionSet$LSID
    set modalitySet modalitySet$LSID
    set phones      phonesSet${LSID}:PHONES
    set phonesSet   phonesSet$LSID
    set tags        tags$LSID
    set log         stderr
    set splitPoints {}
    set sortedLst   {}
    set partition   0

    itfParseArgv questionSetAddTags $args [list [
      list -modalitySet object {} modalitySet ModalitySet {modality set}         ] [
      list -questionSet string {} questionSet {}          {question set}         ] [
      list -phones      object {} phones      Phones      {phones set}           ] [
      list -phonesSet   object {} phonesSet   PhonesSet   {phonesSet set}        ] [
      list -tags        object {} tags        Tags        {tags set}             ] [
      list -splitPoints string {} splitPoints {}          {splitting points}     ] [
      list -partition   int    {} partition   {}          {partition list}     ] [
      list -sortedLst   string {} sortedLst   {}          {sorted list of quests} ] [
      list -log         string {} log         {}          {name of log channel}  ] ]

    if { [llength [info command $questionSet]] < 1 } {
	QuestionSet $questionSet $phones $phonesSet $tags
    }

    if { [llength $splitPoints] || $partition } {

	# look for a pre sorted questionLst
	if { [llength $sortedLst] } {
	    set questionLst $sortedLst
	    set sorted      1
	} else {
	    foreach modality [$modalitySet:] {
		lappend questionLst "0=[$modalitySet:$modality configure -tagName]"
	    }
	    set sorted      0
	}

	if { $partition } {
	    # produce question combinations (OR-questions) by moving a
	    # window of variable size over the questionLst
	    set questionLst [lstPartition $questionLst $sorted]
	} else {
	    # produce question combinations (OR-questions) by means
	    # of given splitPoints
	    set questionLst [lstCombinate $questionLst $splitPoints $sorted]
	}

	# add questions
	foreach question $questionLst {
	    regsub -all { } $question { | } question
	    $questionSet add $question
	}

    } else {

	# add questions
	foreach modality [$modalitySet:] {
	    $questionSet add "0=[$modalitySet:$modality configure -tagName]"
	}
    }
}

# divide tree into one with and one without modality questions
proc distribTreeTransform { LSID args } {
    upvar \#0 $LSID SID

    set distribTree    distribTree$LSID
    set mainTree       mainTree$LSID
    set modTree        modTree$LSID
    set modQuestionSet modQuestionSet$LSID
    set rootIdentifier "ROOT"
    set divide         1
    set desc           $SID(distribTreeDesc)
    set log            stderr

    regsub {\.gz$} $desc {} desc

    if { [info exists SID(mainTreeDesc)] } {
	set mainTreeDesc $SID(mainTreeDesc)
    } else {
	set mainTreeDesc $desc.main.gz
	set SID(mainTreeDesc) $mainTreeDesc
    }

    if { [info exists SID(modTreeDesc)] } {
	set modTreeDesc $SID(modTreeDesc)
    } else {
	set modTreeDesc $desc.mod.gz
	set SID(modTreeDesc) $modTreeDesc
    }

    itfParseArgv treeTransform $args [list [
      list -distribTree  object {} distribTree  Tree {distribTree object}         ] [
      list -mainTree     object {} mainTree     Tree {mainTree object}            ] [
      list -modTree      object {} modTree      Tree {modTree object}             ] [
      list -questionSet  object {} modQuestionSet {} {question set for modalities}] [
      list -mainDesc     string {} mainTreeDesc   {} {description for mainTree}   ] [
      list -modDesc      string {} modTreeDesc    {} {description for modTree}    ] [
      list -rootIdentifier string {} rootIdentifier {} {Identifier for root nodes}] [
      list -divide       int    {} divide         {} {divide tree into subtrees}  ] [
      list -log          string {} log            {} {name of log channel}        ] ]

    $distribTree transform $distribTree $mainTree $modTree $modQuestionSet \
	-rootIdentifier $rootIdentifier -divide $divide

    if { $divide } {
	writeLog $log "$mainTree\twrite $mainTreeDesc"
	$mainTree write $mainTreeDesc

	writeLog $log "$modTree\twrite $modTreeDesc"
	$modTree write $modTreeDesc
    }

    return 1
}


proc treeDFS { root treeArray varArray } {
    upvar $treeArray treeArr
    upvar $varArray  varArr

    if { ![info exists treeArr($root)] || \
	 ([lindex $treeArr($root) 0] == "-" && [lindex $treeArr($root) 1] == "-") } {
	# root is a leave and must have a model
	foreach modality $varArr(modalityLst) {
	    if { [info exists varArr($modality)] && $varArr($modality) } {
		if { $varArr($modality,yes) } {
		    if { ![info exist varArr($modality,yesCount)] } {
			set varArr($modality,yesCount) 0
		    }
		    incr varArr($modality,yesCount)
		} elseif { $varArr($modality,no) } {
		    if { ![info exist varArr($modality,noCount)] } {
			set varArr($modality,noCount) 0
		    }
		    incr varArr($modality,noCount)
		}
	    } else {
		if { ![info exists varArr($modality,count)] } {
		    set varArr($modality,count) 0
		}
		incr varArr($modality,count)
	    }
	}
	return
    }

    if { [info exists varArr($root)] } { return }
    set varArr($root) 1

    set no    [lindex $treeArr($root) 0]
    set yes   [lindex $treeArr($root) 1]
    set undef [lindex $treeArr($root) 2]
    set quest [lindex $treeArr($root) 3]
    
    set foundModLst {}

    foreach modality $varArr(modalityLst) {
	if { [regexp "^0=$modality\$" $quest] } {
	    lappend foundModLst $modality
	}
    }

    foreach modality $foundModLst {
	set varArr($modality)     1
	set varArr($modality,no)  1
	set varArr($modality,yes) 0
    }
    treeDFS $no    treeArr varArr

    foreach modality $foundModLst {
	set varArr($modality)     1
	set varArr($modality,no)  0
	set varArr($modality,yes) 1
    }
    treeDFS $yes   treeArr varArr

    foreach modality $foundModLst {
	set varArr($modality)     0
	set varArr($modality,no)  0
	set varArr($modality,yes) 0
    }
    # treeDFS $undef treeArr varArr

    return
}


# goes bottom up
proc modalityTreeSplit { tree args } {

    set nodeList   [treePTreeList $tree]
    set count      [llength $nodeList]
    set i          0
    set leafN      1
    
    itfParseArgv treeSplit $args [list [
       list "<list>"         string {} lst    {} {split list}      ] [
       list "<modalityLst>"  string {} modLst {} {list of modality tags}   ] [
       list "-leafN"         int {} leafN  {} {number of leafs for each modality} ] ]

    # get root nodes (for each phone)
    foreach node $lst {
	set score [lindex $line 0]
	set root  [lindex $line 1]
	set quest [lindex $line 2]
	set no    [lindex $line 3]
	set yes   [lindex $line 4]
	set undef [lindex $line 5]
	
	set nodeA($root) [list $no $yes $undef $quest]
	
	if { ![regexp {\(} $root] }  { lappend rootLst $root }
    }

    # count models for each modality (tag nodes)
    foreach root $rootLst { treeDFS $root nodeA varA }

    # delete nodes
}


# ===========================================================================
# Helpers for init
# ===========================================================================


# add dummyN distributions to distribSet (imaginary distributions indices)
proc addDummyDs { LSID args } {
    global MODCFG updateA
    upvar  \#0 $LSID SID

    set distribSet  distribSet$LSID
    set codebookSet codebookSet$LSID
    set featureSet  featureSet$LSID
    set dummyName   $MODCFG(dummyName)
    set log         stderr

    itfParseArgv addDummyDs $args [list [
      list <desc>       string {} modTreeDesc   {}        {description file}      ] [
      list -distribSet  object {} distribSet  DistribSet  {distribution set}      ] [
      list -codebookSet object {} codebookSet CodebookSet {codebook set}          ] [
      list -featureSet  object {} featureSet  FeatureSet  {feature set}           ] [
      list -dummyName   string {} dummyName     {}        {name of dummy distribs}] [
      list -log         string {} log           {}        {name of log channel}   ] ]

    $distribSet configure -dummyName $dummyName

    set dimN [$codebookSet.item(0) configure -dimN]
    set refN [$codebookSet.item(0) configure -refN]
    
    $codebookSet add $dummyName $MODCFG(cbFeature) $refN $dimN NO
    set dummyN [getDummyN $LSID $modTreeDesc -log $log]
    
    for {set i 1} {$i <= $dummyN} {incr i} {
	$distribSet add $dummyName-$i $dummyName
    }

    writeLog $log "$distribSet    [expr $i-1] dummyDs added"

    set updateA(treeN) [expr $i-1]
}


# get the number of dummy distributions
proc getDummyN { LSID args } {
    upvar \#0 $LSID SID

    set log        stderr

    itfParseArgv getDummyN $args [list [
      list <desc>      string {} modTreeDesc   {}    {description file}        ] [
      list -log        string {} log           {}    {name of log channel}     ] ]

    if { [regexp {\.gz$} $modTreeDesc] } {
	if [catch {exec gunzip -c $modTreeDesc | grep ROOT | wc -l} wc] {
	    set dummyN 0
	} else {
	    set dummyN [lindex $wc 0]
	}
    } else {
	if [catch {exec cat $modTreeDesc | grep ROOT | wc -l} wc] {
	    set dummyN 0
	} else {
	    set dummyN [lindex $wc 0]
	}
    }

    return $dummyN
}


# ===========================================================================
# procedures for the calculation of the modalities
# ===========================================================================

proc modalityUpdateUtterance { modalitySet LSID args } {
    global updateA
    upvar  \#0 $LSID SID

    set featureSet $updateA(featureSet)

    itfParseArgv modalityUpdateUtterance $args [list [
      list <spk>        string {} spk              {}     {spkeaker id}           ] [
      list <utt>        string {} utt              {}     {utterance id}          ] [
      list -featureSet  string {} featureSet       {}     {feature set}           ] [
      list -modInfo     string {} modInfo          {}     {list of modalities}    ] ]

    set uttInfo          [uttDB uttInfo $spk $utt]
    set updateA(uttInfo) $uttInfo
    set updateA(spk)     $spk
    set updateA(utt)     $utt
    if { ![info exists modInfo] } {
	set db               $updateA(modDB)
	set modInfo          [$db get $utt]
    }
    set updateA(modInfo) $modInfo

    if [catch {getFrameN $LSID $uttInfo -featureSet $featureSet} frameN] {
	    error "ERROR getFrameN $LSID $uttInfo ...\n$frameN"
    }    

    if [catch {$modalitySet reset} msg] {
	error "ERROR $modalitySet reset\n$msg"
    }
    if [catch {$modalitySet update 0 [expr $frameN-1]} msg] {
	error "$modalitySet update 0 [expr $frameN-1]\n$msg"
    }

    unset uttInfo
    unset modInfo
}

ModalitySet method updateUtterance modalityUpdateUtterance \
    -text "update modality for the whole utterance"


proc stateTableUpdateUtterance { stateTable LSID args } {
    global updateA
    upvar  \#0 $LSID SID

    set featureSet $updateA(featureSet)

    itfParseArgv stateTableUpdateUtterance $args [list [
      list <spk>        string {} spk              {}     {spkeaker id}           ] [
      list <utt>        string {} utt              {}     {utterance id}          ] [
      list -featureSet  string {} featureSet       {}     {feature set}           ] ]

    set uttInfo          [uttDB uttInfo $spk $utt]
    set updateA(uttInfo) $uttInfo
    set updateA(spk)     $spk
    set updateA(utt)     $utt
    set db               $updateA(modDB)
    set modInfo          [$db get $utt]
    set updateA(modInfo) $modInfo

    if [catch {getFrameN $LSID $uttInfo -featureSet $featureSet } frameN] {
	error "ERROR getFrameN $LSID $uttInfo ...\n$frameN"
    }

    if [catch {$stateTable reset} msg] {
	error "ERROR $stateTable reset\n$msg"
    }
    if [catch {$stateTable update 0 [expr $frameN-1]} msg] {
	error "$stateTable update 0 [expr $frameN-1]\n$msg"
    }

    unset uttInfo
    unset modInfo
}

StateTable method updateUtterance stateTableUpdateUtterance \
    -text "update stateTable and modalities for the whole utterance"


proc stateTableUpdateUnsupervised { stateTable LSID args } {
    global updateA
    upvar  \#0 $LSID SID

    set featureSet $updateA(featureSet)

    itfParseArgv stateTableUpdateUtterance $args [list [
      list <uttInfo>    string {} uttInfo          {}     {uttInfo}               ] [
      list <fieldLst>   string {} fieldLst         {}     {list of fields}        ] [
      list <modLst>     string {} modLst           {}     {list of modalities}    ] [
      list -featureSet  string {} featureSet       {}     {feature set}           ] ]

    makeArray uttInfoA $uttInfo

    set updateA(uttInfo) $uttInfo
    set updateA(spk)     $uttInfoA(spk)
    set updateA(utt)     $uttInfoA(utt)
    set db               $updateA(modDB)
    set modInfo          [$db get $uttInfoA(utt)]
    set fields           [llength $fieldLst]


    makeArray modArr $modInfo

    for {set i 0} {$i < $fields} {incr i} {
	set modArr([lindex $fieldLst $i]) [lindex $modLst $i]
    }

    set updateA(modInfo) [infoArr2lst modArr 0 1]

    if [catch {getFrameN $LSID $uttInfo -featureSet $featureSet } frameN] {
	error "ERROR getFrameN $LSID $uttInfo ...\n$frameN"
    }

    if [catch {$stateTable reset} msg] {
	error "ERROR $stateTable reset\n$msg"
    }
    if [catch {$stateTable update 0 [expr $frameN-1]} msg] {
	error "$stateTable update 0 [expr $frameN-1]\n$msg"
    }

    unset uttInfo
    unset modInfo

    puts stderr "$uttInfoA(spk) $uttInfoA(utt) updated $fieldLst to $modLst"
}

StateTable method updateUnsupervised stateTableUpdateUnsupervised \
    -text "update stateTable and modalities for the whole utterance (unsupervised)"


# ---------------------------------------------------------------------------
# get the length of a feature in frames
# ---------------------------------------------------------------------------

proc getFrameN { LSID uttInfo args } {
    global updateA
    upvar  \#0 $LSID SID

    set featureSet       modFeatureSet$LSID
    set feature          FFT

    itfParseArgv getFrameN $args [list [
      list -featureSet  string {} featureSet       {}     {feature set}           ] [
      list -feature     string {} feature          {}     {single feature}        ] ]

    if { ![info exists updateA(readADC)] } {
	if [catch {$featureSet eval $uttInfo} msg] {
	    puts stderr "$featureSet eval $uttInfo\n$msg"
	    return 0
	}
	set updateA(readADC) 1
    }

    unset updateA(readADC)

    return [$featureSet:$feature configure -frameN]
}


# ---------------------------------------------------------------------------
# score modalities for identification
# ---------------------------------------------------------------------------

proc scoreModalityLst {frameO scoreO LSID uttInfo modalityLst field \
			   {modPhoneLst {}} {verbose 0} {logFile ""}} {
    global WARPSCALE updateA
    upvar $frameO frameA
    upvar $scoreO scoreA

    makeArray uttInfoA $uttInfo
    
    set updateA(uttInfo) $uttInfo
    set updateA(spk)     $uttInfoA(spk)
    set updateA(utt)     $uttInfoA(utt)
    set db               $updateA(modDB)
    set modInfo          [$db get $uttInfoA(utt)]
    set cwarp            $WARPSCALE

    makeArray modArr $modInfo

    if { [catch {featureSet$LSID eval "$uttInfo"} msg] } {
	puts stderr "featureSet$LSID eval $uttInfo\n$msg"
	continue
    }

    if [catch {getFrameN $LSID $uttInfo -featureSet featureSet$LSID } frameN] {
	error "ERROR getFrameN $LSID $uttInfo ...\n$frameN"
    }

    if { [llength $logFile] } { set outLst [list [list spk $updateA(spk)] \
						 [list utt $updateA(utt)]] }

    foreach modality $modalityLst {

	set modArr($field)   $modality
	set updateA(modInfo) [infoArr2lst modArr 0 1]

	stateTable$LSID reset
	stateTable$LSID update 0 [expr $frameN-1]

	if { [catch {pathPhoneScores $LSID $modPhoneLst} frameScore] } {
	    puts stderr "($modality) pathPhoneScores $LSID $modPhoneLst\n$frameScore"
	    continue
	}

	if { $verbose } { puts stderr "$modality $frameScore" }
	if { [llength $logFile] } { lappend outLst [list $modality $frameScore] }

	if [info exists scoreA($modality)] {
	    set  scoreA($modality) [expr $scoreA($modality)+[lindex $frameScore 1]]
	    set  frameA($modality) [expr $frameA($modality)+[lindex $frameScore 0]]
	} else {
	    set  scoreA($modality) [lindex $frameScore 1]
	    set  frameA($modality) [lindex $frameScore 0]
	}
    }

    if { [llength $logFile] } {
	set fp [open $logFile a]
	puts $fp $outLst
	close $fp
    }

    set WARPSCALE $cwarp
}

proc scoreModalities { LSID args } {
    global updateA
    upvar  \#0 $LSID SID

    set modalitySet      modalitySet$LSID
    set stateTable       stateTable$LSID
    set hmm              hmm$LSID
    set path             path$LSID
    set featureSet       $updateA(featureSet)
    set feature          FFT
    set beam             200
    set topN             0
    set verbose          0

    itfParseArgv scoreModalities $args [list [
      list <spk>        string {} spk              {} {spkeaker id}           ] [
      list <utt>        string {} utt              {} {utterance id}          ] [
      list <scoreA>     string {} scoreA           {} {scoring array}         ] [
      list -text        string {} text             {} {text to align}         ] [
      list -modalityLst string {} modalityLst      {} {modality set}          ] [
      list -field       string {} field            {} {field in modalityArr}  ] [
      list -modalitySet string {} modalitySet      {} {modality set}          ] [
      list -featureSet  string {} featureSet       {} {feature set}           ] [
      list -feature     string {} feature          {} {single feature}        ] [
      list -hmm         string {} hmm              {} {name of hmm}           ] [
      list -path        string {} path             {} {name of path}          ] [
      list -beam        string {} beam             {} {viterbi beam}          ] [
      list -topN        string {} topN             {} {topN beam}             ] [
      list -optWord     string {} optWord          {} {optional word}         ] [
      list -variants    int    {} variants         {} {variants 0/1}          ] ]

    upvar $scoreA sA

    set uttInfo          [uttDB uttInfo $spk $utt]
    set updateA(uttInfo) $uttInfo
    set updateA(spk)     $spk
    set updateA(utt)     $utt
    set db               $updateA(modDB)
    set modInfo          [$db get $utt]
    set minScore         10000000
    set minModality      ""

    makeArray modArr $modInfo

    puts stderr "$modInfo"

    if [catch {getFrameN $LSID $uttInfo -featureSet $featureSet} frameN] {
	error "ERROR getFrameN $LSID $uttInfo ...\n$frameN"
    }

    if { [llength [info command stateTable$LSID]] } {
	set object $stateTable
    } else {
	set object $modalitySet
    }

    set should $modArr($field)

    foreach modality $modalityLst {

	set modArr($field)   $modality
	set updateA(modInfo) [infoArr2lst modArr 0 1]

	if [catch {$modalitySet reset} msg] {
	    error "ERROR $modalitySet reset\n$msg"
	}
	if [catch {$object update 0 [expr $frameN-1]} msg] {
	    error "$object update 0 [expr $frameN-1]\n$msg"
	}

#	putsInfo "set to [$modalitySet answer2tags [$modalitySet majorityAnswer]]"

	PhoneGraph configure -modTags [$modalitySet answer2codedTags [$modalitySet majorityAnswer]]

	if [catch {$hmm make $text -optWord $optWord -variants $variants} msg] {
	    error "ERROR $hmm make $text -optWord $optWord -variants $variants\n$msg"
	}

	if [catch {$path viterbi hmm$LSID -eval $uttInfo -beam $beam -topN $topN} score] {
	    error "ERROR $path viterbi hmm$LSID\n$score"
	}

	if { $verbose } { puts stderr "[format "%-20s" $modality] $score" }
	lappend sA($score) $modality
    }

    return $should
}

