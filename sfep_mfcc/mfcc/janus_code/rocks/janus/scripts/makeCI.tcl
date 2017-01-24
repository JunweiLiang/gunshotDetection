# ========================================================================
#  JanusRTk   Janus Speech Recognition Toolkit
#             ------------------------------------------------------------
#             Object: Initialization/ Checks for new system
#             ------------------------------------------------------------
#
#  Author  :  The Ibis Gang
#  Module  :  makeCI.tcl
#  Date    :  $Id: makeCI.tcl 2390 2003-08-14 11:20:32Z fuegen $
#
#  Remarks :  create context independent description files for codebooks
#             and distributions and grow a tree with polyphonic nodes
#             for clustering. In comparison to the original, here the 
#             polyphones all have their corresponding distribution when
#             they are created - this avoids hand-editing the ptreeSet
#             description file.
#
# ========================================================================
#
#  $Log$
#  Revision 1.2  2003/08/14 11:19:45  fuegen
#  Merged changes on branch jtk-01-01-15-fms (jaguar -> ibis-013)
#
#  Revision 1.1.2.3  2002/07/30 07:43:49  metze
#  Bugfix for variables
#
#  Revision 1.1.2.2  2002/07/12 16:29:55  metze
#  Approved version
#
#  Revision 1.1.2.1  2002/07/12 10:03:01  metze
#  Initial script collection
#
# ========================================================================


# ------------------------------------------------------------------------
#  Initialize context independent model tree
# ------------------------------------------------------------------------

proc initCISystem { LSID args } {
    global SYS
    upvar $LSID SID
    set stags  "-b -m -e"

    itfParseArgv treeInitCISystem $args [list [
    list <tree>      object  {} tree     Tree     {tree}                ] [
    list <feature>   string  {} feature  {}       {name of feature}     ] [
    list <refN>      int     {} refN     {}       {number of ref}       ] [
    list <dimN>      int     {} dimN     {}       {dimension of input}  ] [
    list <type>      string  {} type     {}       {type of covariance}  ] [
    list -stateTags  string  {} stags    {}       {state tags}          ] ]

    set phones  [$tree.questionSet.phones puts]
    set nnode   ""
    
    # build now the distribution tree in inverse order
    # (the first phone will be the last one in the tree)
    foreach p $phones {
	if {$p == "@"}   { continue }

	# Semi-continuous cases?
	regsub "\/.*" $p "" q

	# Tune the architecture we want
	#   (we don't want 3-state SIL)
	switch $q {
	    SIL     { set stagsL "-m" }
	    default { set stagsL $stags }
	}

	if {[distribSet$LSID index ${q}-m] != -1} { continue }

	foreach s $stagsL {
	    distribTreeAddModel $LSID \
		"0=$q" "${q}$s" "${q}$s" $feature $refN $dimN $type -root ROOT$s
	}
    }

    # Allocate the codebooks (needed if we want to write them out)
    foreach cb [codebookSet$LSID:] { codebookSet$LSID:$cb alloc }
}


# ------------------------------------------------------------------------
#  Main
# ------------------------------------------------------------------------

set featureName  LDA
set codebookRefN 48
set codebookDimN 32
set covarType    DIAGONAL

set descFile ../desc/desc.tcl

if {!$tcl_interactive} {
    if [catch {itfParseArgv $argv0 $argv [list [
        list -spkLst         string  {} spkLst        {} {speaker list}     ] [
        list -desc           string  {} descFile      {} {description file} ] [
        list -featureName    string  {} featureName   {} {feature of codebook (e.g. LDA)} ] [
        list -refN           string  {} codebookRefN  {} {number of vecs in codebook} ] [
        list -dimN           string  {} codebookDimN  {} {dimension of codebook}      ] [
	list -covarType      string  {} covarType     {} {covariance type of codebook}] ]
    }] {
        exit -1
    }
}

source $descFile

foreach i "codebookSetDesc distribSetDesc distribTreeDesc phonesSetDesc tagsDesc" {
    if {![info exists $i] || [set $i] == ""} { eval "set $i \$${SID}($i)" }
}


# Init the system
PhonesSet     phonesSet$SID
phonesSet$SID read $phonesSetDesc

Tags          tags$SID
tags$SID      read $tagsDesc

FeatureSet    featureSet$SID

CodebookSet   codebookSet$SID featureSet$SID 
DistribSet    distribSet$SID  codebookSet$SID 
Tree          distribTree$SID phonesSet${SID}:PHONES phonesSet$SID tags$SID distribSet$SID


# Create cbs, dss and dst
initCISystem  $SID distribTree$SID $featureName $codebookRefN $codebookDimN $covarType


# Write out the data
if {![file exists $codebookSetDesc] && $codebookSetDesc != ""} {
    printDo codebookSet$SID           write $codebookSetDesc
}
if {![file exists $distribSetDesc]  && $distribSetDesc != ""} {
    printDo distribSet$SID            write $distribSetDesc
}
if {![file exists $distribTreeDesc] && $distribTreeDesc != ""} {
    printDo distribTree$SID           write ${distribTreeDesc}
}

exit
