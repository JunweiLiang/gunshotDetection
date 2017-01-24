# ========================================================================
#  JanusRTK   Janus Speech Recognition Toolkit
#             ------------------------------------------------------------
#             Object: CMS extraction
#             ------------------------------------------------------------
#
#  Author  :  The Ibis Gang
#  Module  :  means.tcl
#  Date    :  $Id: cluster.tcl 2865 2009-02-16 21:22:05Z metze $
#
#  Remarks :  SWB02-style cepstral mean extraction
#
# ========================================================================
# 
#   $Log$
#   Revision 1.2  2003/08/14 11:19:42  fuegen
#   Merged changes on branch jtk-01-01-15-fms (jaguar -> ibis-013)
#
#   Revision 1.1.2.5  2003/08/11 14:28:59  soltau
#   verbose = 0
#
#   Revision 1.1.2.4  2002/10/30 14:23:39  metze
#   Read padPhone from SID array
#
#   Revision 1.1.2.3  2002/07/30 07:42:48  metze
#   Bugfix for variables
#
#   Revision 1.1.2.2  2002/07/12 16:29:54  metze
#   Approved version
#
#   Revision 1.1.2.1  2002/07/12 10:03:00  metze
#   Initial script collection
#
# ========================================================================


# ------------------------------------------------------------------------
#  Settings for clustering
# ------------------------------------------------------------------------

set spkLst        train-spk
set descFile      ../desc/desc.tcl
set semFile       train.DONE

set nodeListFile  nodeList
set minModelCount 100
set minCount      1000
set maxSplit      1000
set maxContext    2

set codebookSetDesc  ../desc/codebookSet.PT.gz
set codebookSetParam Weights/4.cbs.gz
set distribSetDesc   ../desc/distribSet.PT.gz
set distribSetParam  Weights/4.dss.gz
set distribTreeDesc  ../desc/distribTree.PT.gz
set ptreeSetDesc     ../desc/ptreeSet.PT.gz


# -----------------------------------------------------------------------
#  Read in command-line options (override settings)
# -----------------------------------------------------------------------

if [catch {itfParseArgv $argv0 $argv [list [
    list -spkLst      string  {} spkLst           {} {speaker list} ] [
    list -desc        string  {} descFile         {} {description file} ] [
    list -meanPath    string  {} meanPath         {} {path for means} ] [
    list -semFile     string  {} semFile          {} {semaphore file} ] [
    list -maxContext  int     {} maxContext       {} {maximum context} ] [
    list -cbsDesc     string  {} codebookSetDesc  {} {codebook descriptions} ] [
    list -cbsParam    string  {} codebookSetParam {} {codebook parameters} ] [
    list -dssDesc     string  {} distribSetDesc   {} {distribution descriptions} ] [
    list -dssParam    string  {} distribSetParam  {} {distribution parameters} ] [
    list -dstDesc     string  {} distribTreeDesc  {} {distribution tree description} ] [
    list -ptreeDesc   string  {} ptreeSetDesc     {} {ptree description} ] ]
} ] {
    puts stderr "ERROR in $argv0 while scanning options"
    exit -1
}

source $descFile

foreach i "optWord variants dbaseName featureSetDesc meanPath \
           codebookSetDesc codebookSetParam distribSetDesc distribSetParam \
           distribTreeDesc ptreeSetDesc" {
    if {![info exists $i] || [set $i] == ""} { eval "set $i \$${SID}($i)" }
}
eval "set labelPath \[string range \$${SID}(labelPath) 0 end\]"

waitFile "$semFile"


# ------------------------------------------------------------------------
#  Init Modules
# ------------------------------------------------------------------------

phonesSetInit   $SID
tagsInit        $SID
featureSetInit  $SID -desc "" -access "" -lda ""
vtlnInit        $SID 
codebookSetInit $SID -desc $codebookSetDesc -param $codebookSetParam
distribSetInit  $SID -desc $distribSetDesc  -param $distribSetParam
distribTreeInit $SID -desc $distribTreeDesc -ptree $ptreeSetDesc    
senoneSetInit   $SID  distribStream$SID
topoSetInit     $SID
ttreeInit       $SID
dictInit        $SID 
trainInit       $SID
dbaseInit       $SID $dbaseName


# ------------------------------------------------------------------------
#  Question Set
# ------------------------------------------------------------------------

QuestionSet QS phonesSet$SID:PHONES phonesSet$SID tags$SID 

# Add questions for the "WB" tag (this should be "save")
QS add "0=WB"
QS add "0=WB +1=WB"
QS add "0=WB -1=WB"
QS add "0=WB +1=WB -1=WB"
QS add "-1=WB +1=WB +2=WB"
QS add "+1=WB -1=WB -2=WB"

catch {
    # stress marker
    QS add "0=STRESSED-1"
    QS add "0=STRESSED-2"
    QS add "0=STRESSED"
    QS add "0=UNSTRESSED"
    
    QS add "0=STRESSED-1 0=WB"
    QS add "0=STRESSED-2 0=WB"
    QS add "0=STRESSED   0=WB"
    QS add "0=UNSTRESSED 0=WB"
}

# make questions, for left and right context separately:
foreach p [phonesSet$SID:] {
    for {set i 1} {$i <= $maxContext} {incr i} {
	QS add "-$i=$p"
	QS add "+$i=$p"
	QS add "-$i=$p -$i=WB"
	QS add "+$i=$p +$i=WB"
    }
}

foreach p [phonesSet$SID:PHONES] {
    if {$p == [set ${SID}(padPhone)]} { continue }
    for {set i 1} {$i <= $maxContext} {incr i} {	
	QS add "-$i=$p"
	QS add "+$i=$p"
	QS add "-$i=$p -$i=WB"
	QS add "+$i=$p +$i=WB"
    }
}

puts stderr "\nFinished creating question set, here it is:"
puts stderr "[QS:]"


# ------------------------------------------------------------------------
#  Initialize Codebook Clustering
# ------------------------------------------------------------------------

QS configure              -padPhone [phonesSet$SID:PHONES index [set ${SID}(padPhone)]]
distribTree$SID configure -padPhone [phonesSet$SID:PHONES index [set ${SID}(padPhone)]]
distribSet$SID  configure -minCount $minCount

puts stderr "\nTreeCluster distribTree$SID QS "

file mkdir cluster

if {![file exists $nodeListFile]} {
    set fp [open $nodeListFile w]
    foreach i [distribTree$SID.ptreeSet:] { puts $fp $i }
    close $fp
}

doParallel {
    while {[fgets $nodeListFile ptreeNode] >= 0} { 
	puts stderr "  Working on $ptreeNode ..."
	distribTree$SID cluster $ptreeNode -questionSet QS \
	    -maxSplit $maxSplit                            \
	    -minCount $minModelCount                       \
	    -file cluster/${ptreeNode}.cl -v 0
    }
} {
    touch cluster.DONE
} { } { }

exit
