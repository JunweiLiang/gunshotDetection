# ========================================================================
#  JanusRTK   Janus Speech Recognition Toolkit
#             ------------------------------------------------------------
#             Object: Make description files for Polyphone Training
#             ------------------------------------------------------------
#
#  Author  :  The Ibis Gang
#  Module  :  makePT.tcl
#  Date    :  $Id: makePT.tcl 2390 2003-08-14 11:20:32Z fuegen $
#
#  Remarks :  
#
# ========================================================================
# 
#   $Log$
#   Revision 1.2  2003/08/14 11:19:45  fuegen
#   Merged changes on branch jtk-01-01-15-fms (jaguar -> ibis-013)
#
#   Revision 1.1.2.5  2003/08/13 12:54:12  metze
#   Added *hook* to match list
#
#   Revision 1.1.2.4  2003/08/11 12:42:28  soltau
#   fixed convLst
#
#   Revision 1.1.2.3  2002/07/30 07:43:56  metze
#   Bugfix for variables
#
#   Revision 1.1.2.2  2002/07/12 16:29:55  metze
#   Approved version
#
#   Revision 1.1.2.1  2002/07/12 10:03:01  metze
#   Initial script collection
#
# ========================================================================


# ------------------------------------------------------------------------
#  add ptree items in distribTree description and create ptreeSet desc
# ------------------------------------------------------------------------

proc addPtreeDistribDesc {filenameIn filenameOut1 filenameOut2} {
    global SID
    set phoneL [ps:PHONES]
    set fp1 [open $filenameIn "r"]
    set fp2 [open $filenameOut1 "w"]
    set fp3 [open  $filenameOut2 "w"]
    puts $fp3 "; -------------------------------------------------------"
    puts $fp3 ";  Name            : distribTree$SID"
    puts $fp3 ";  Type            : PTreeSet"
    puts $fp3 ";  Date            : [exec date]"
    puts $fp3 "; -------------------------------------------------------"

    while {[gets $fp1 line] >0} {
	if { [string match ";*"     $line] ||
	     [string match "*ROOT*" $line] ||
	     [string match "*hook*" $line]} {
	    puts $fp2 $line; continue 
	}
	set lastE [lindex $line 5]
	regsub -all -- "\\(\\|\\)|-.*" $lastE "" phone
	regsub -all -- "\\(\\|\\)|/."  $lastE "" lastE
	#set strX [expr [string first "-" $lastE] -1]
	#set phone [string range $lastE 0 $strX]
	set line2 "$line -ptree $lastE"
	puts $fp2 $line2
	if {[lsearch $phoneL $phone] >= 0} {
	    puts $fp3 "$lastE \{$phone\} 0 0 -count 1.000000 -model $lastE"
	} else {
	    # check for stress markers
	    foreach x $phoneL {
		if [string match "$phone/?" $x] {
		    puts $fp3 "$lastE \{$x\} 0 0 -count 1.000000 -model $lastE"
		}
	    }
	}
    }
    close $fp1; close $fp2; close $fp3
}


# ------------------------------------------------------------------------
#  Create Model function
# ------------------------------------------------------------------------

proc createModel { ptree args } {

    global SID
    
    itfParseArgv findModel $args [list [
    list <id>    string  {} id       {} {node ID}               ] [
    list <word>  string  {} word     {} {list of tagged phones} ] [
    list <left>  int     {} left     {} {left  context}         ] [
    list <right> int     {} right    {} {right context}         ] [
    list <count> float   {} count    {} {counts}                ] [
    list -model  string  {} model    {} {model name}            ] ]

    # that should never happen:
    if { $count < 1 } { puts stderr "Strange. You ask a non-existing thing. " ; return "-" }
  
    # extract monophone name and tag ('b', 'm', 'e') from node ID:
    regexp {(.*)-([bme])} $ptree dummy phone stag
    regsub -all "\\(\\|\\)" $phone "" phone

    #set distribName as nodeID
    set strX [string length $phone]
    # regsub {/.} $id {} x
    set dsName "$phone-$stag[string range $id $strX end]"

    # well, it has got a name, however, it is not there yet...
    if { [distribSet$SID index $dsName] < 0} {
        # Codebook names are simple in sys1r2 and sys1r3...
        set cbName "${phone}-$stag"
        # this, however, should not happen!!!
        if {[codebookSet$SID index $cbName] < 0} {
            codebookSet$SID add $cbName LDA 16 32 DIAGONAL
            puts stderr "createModel WARNING: this is queer... the codebook $cbName does not exist..."
        }
        # add the distribution, this is the work
        distribSet$SID add $dsName $cbName

    } else {
        puts stderr "createModel WARNING: this is queer... the distribution $dsName exists..."
    }

    return $dsName
}


# ------------------------------------------------------------------------
#  init polyphone tree
# ------------------------------------------------------------------------

proc InitPtree {} {
    global SID

    # tell the tree that some models really don't need polyphone trees
    # +NH  : nonhuman noise should be trated context independent ?
    # +BR  : breath has seen too much of silence
    # SIL  : context independent as well
    foreach leaf [distribTree${SID}:] {
	if { [string range $leaf 0 0] == "+NH" ||
	     [string range $leaf 0 2] == "+BR" ||
	     [string range $leaf 0 2] == "SIL"} {
	    puts stderr "Deleting polyphone tree at the leaf $leaf"
	    distribTree${SID}:$leaf configure -ptree -1
	} 
    }

    # tell the tree to add polyphone models
    distribTree$SID configure -ptreeAdd 1
}


# ------------------------------------------------------------------------
#  create models
# ------------------------------------------------------------------------

proc createContextModels {convLst} {
    global SID variants optWord

    set fp [open $convLst r]
    set sentence_count 0

    while {[gets $fp spk] > 0} {
	if [string match "#*" $spk] {
	    puts "\n%TURN: $spk ignored"
	    continue
	}
	foreachSegment utt uttDB $spk {
	    set       uttInfo [uttDB uttInfo $spk $utt]
	    makeArray uttArray $uttInfo
	    
	    set text $uttArray(TEXT)
	    puts "%TURN: $spk $utt"
	    
	    if [catch {hmm$SID makeUtterance $text -optWord $optWord -variants $variants} msg] {
		puts $msg
	    }
    
	    # for memory saving reasons, destroy the generated
	    # amodels and senones
	    incr sentence_count
	    if { [expr $sentence_count % 5] == 0 } {
		amodelSet$SID reset
		senoneSet$SID reset
	    }
	}
    }
    close $fp
}


# ------------------------------------------------------------------------
#  Settings for Polyphone Tree
# ------------------------------------------------------------------------

set spkLst      spk-train.pt
set descFile    ../desc/desc.tcl
set semFile     ""

set maxContext  2


# -----------------------------------------------------------------------
#  Read in command-line options (override settings)
# -----------------------------------------------------------------------

if [catch {itfParseArgv $argv0 $argv [list [
    list -spkLst     string {} spkLst         {} {list of speakers}] [
    list -desc       string {} descFile       {} {description file} ] [
    list -dbase      string {} dbase          {} {database}] [
    list -featDesc   string {} featureSetDesc {} {feature description file} ] [
    list -featAccess string {} featAccess     {} {feature access file} ] [
    list -dim        int    {} dim            {} {dimension of feature} ] [
    list -warpFile   string {} warpFile       {} {file with warp factors} ] [
    list -qsFile     string {} qsFile         {} {file of questionSet} ] [
    list -maxSplit   int    {} maxSplit       {} {maxSplit} ] [
    list -maxContext int    {} maxContext     {} {context} ] [
    list -beam       int    {} beam           {} {viterbi beam} ] [
    list -modalities int    {} modalities     {} {include modalities} ] [
    list -modQuests  int    {} modalityQuestions {} {include modality Questions} ] [
    list -modalityA  string {} modalityArr    {} {description array of modalities}] [
    list -noise      int    {} addNoise       {} {add noise to some utts} ] [
    list -vtln       int    {} vtln           {} {do vtln} ] [
    list -bottomUp   int    {} bottomUp       {} {bottom up clustering}] [
    list -ml         int    {} ml             {} {do ML clustering} ] [
    list -labelPath  string {} labelPath      {} {path specification for labels} ] [
    list -factor     float  {} factor         {} {factor}      ] [
    list -subPhone   string {} subPhone       {} {subPhone for ptree (b,m,e)} ] [
    list -semFile    string {} semFile        {} {semaphore file}]]
}] {
    exit 1
}

source $descFile

foreach i "optWord variants dbaseName featureSetDesc meanPath" {
    if {![info exists $i] || [set $i] == ""} { eval "set $i \$${SID}($i)" }
}
eval "set labelPath \[string range \$${SID}(labelPath) 0 end\]"


# ------------------------------------------------------------------------
#  Create initial ptree
# ------------------------------------------------------------------------

if {![file exists [set ${SID}(descPath)]/distribTree.CI]} {
    puts stderr "\nCreating *.CI description files ..."
    phonesSetInit   $SID -phonesSet ps
    addPtreeDistribDesc [set ${SID}(descPath)]/distribTree \
	[set ${SID}(descPath)]/distribTree.CI [set ${SID}(descPath)]/ptreeSet.CI
    puts stderr "Ok.\n"
}

waitFile "$semFile"


# ------------------------------------------------------------------------
#  Init Modules
# ------------------------------------------------------------------------

phonesSetInit   $SID
tagsInit        $SID
featureSetInit  $SID -desc "" -access "" -lda ""
codebookSetInit $SID -param ""
distribSetInit  $SID -param ""
distribTreeInit $SID -ptree [set ${SID}(descPath)]/ptreeSet.CI \
                     -desc  [set ${SID}(descPath)]/distribTree.CI
senoneSetInit   $SID  distribStream$SID
topoSetInit     $SID
ttreeInit       $SID
dictInit        $SID 
trainInit       $SID
dbaseInit       $SID $dbaseName


# ------------------------------------------------------------------------
#  main
# ------------------------------------------------------------------------

foreach ptree [distribTree$SID.ptreeSet.list] {
    distribTree$SID.ptreeSet.list:$ptree configure -addProc createModel
    distribTree$SID.ptreeSet.list:$ptree configure -maxContext $maxContext
}

InitPtree
createContextModels $spkLst

codebookSet$SID          write [set ${SID}(descPath)]/codebookSet.PT.gz
distribSet$SID           write [set ${SID}(descPath)]/distribSet.PT.gz
distribTree$SID.ptreeSet write [set ${SID}(descPath)]/ptreeSet.PT.gz
distribTree$SID          write [set ${SID}(descPath)]/distribTree.PT.gz

touch ptree.DONE

exit
