# ========================================================================
#  JanusRTK   Janus Speech Recognition Toolkit
#             ------------------------------------------------------------
#             Object: Writing of labels
#             ------------------------------------------------------------
#
#  Author  :  The Ibis Gang
#  Module  :  labels.tcl
#  Date    :  $Id: labels.tcl 3320 2010-10-13 21:15:11Z metze $
#
#  Remarks :
#
# ========================================================================
#
#   $Log$
#   Revision 1.4  2004/09/23 15:33:15  metze
#   By default generates compressed labels
#
#   Revision 1.3  2003/12/17 13:07:46  metze
#   P013
#
#   Revision 1.2  2003/08/14 11:19:44  fuegen
#   Merged changes on branch jtk-01-01-15-fms (jaguar -> ibis-013)
#
#   Revision 1.1.2.3  2002/07/30 07:43:34  metze
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
#  Settings for train
# ------------------------------------------------------------------------
set spkLst       train-spk
set descFile     ../desc/desc.tcl
set semFile      ""
set labelPath    "\$spk/\$utt.lbl.gz"

set minCount     2000
set depth        3
set tryMax       3

#set codebookSetParam ../train/Weights/4.cbs.gz
#set distribSetParam  ../train/Weights/4.dss.gz


# -----------------------------------------------------------------------
#  Read in command-line options (override settings)
# -----------------------------------------------------------------------

if [catch {itfParseArgv $argv0 $argv [list [
    list -spkLst      string  {} spkLst           {} {speaker list} ] [
    list -desc        string  {} descFile         {} {description file} ] [
    list -meanPath    string  {} meanPath         {} {path for means} ] [
    list -labelPath   string  {} labelPath        {} {path for labels} ] [
    list -rewrite     string  {} rewriteRules     {} {rewriteSet description} ] [
    list -cbsParam    string  {} codebookSetParam {} {codebook parameters} ] [
    list -dssParam    string  {} distribSetParam  {} {distribution parameters} ] [
    list -lda         string  {} featureSetLDAMatrix {} {LDA Matrix to use} ] [
    list -semFile     string  {} semFile          {} {semaphore file} ] ]
} ] {
    puts stderr "ERROR in $argv0 while scanning options"
    exit -1
}

source $descFile

foreach i "optWord variants dbaseName featureSetDesc meanPath \
           codebookSetParam distribSetParam rewriteRules featureSetLDAMatrix" {
    if {![info exists $i] || [set $i] == ""} { 
        if { [catch { eval "set $i \$${SID}($i)" } msg] } {
            puts "WARNING: '$msg' caught ..."
        }
    }
}

waitFile "$semFile"


# ------------------------------------------------------------------------
#  Init Modules
# ------------------------------------------------------------------------

phonesSetInit   $SID
tagsInit        $SID
featureSetInit  $SID -lda $featureSetLDAMatrix
vtlnInit        $SID

if {![info exists rewriteRules] || $rewriteRules == ""} {
    codebookSetInit $SID -param $codebookSetParam
    distribSetInit  $SID -param $distribSetParam
} else {
    # Load the RewriteSet -- if necessary
    printDo [RewriteSet rewriteSet$SID] read $rewriteRules

    # We hope it's ok to load these (old) codebooks/ distribs
    printDo [CodebookSet cbs featureSet$SID] read [file join [file dirname $codebookSetParam] codebookSet]
    printDo [DistribSet  dss cbs]            read [file join [file dirname $distribSetParam]  distribSet]
    printDo cbs load $codebookSetParam
    printDo dss load $distribSetParam

    # Create the new codebooks/ distribs
    codebookSetInit $SID
    distribSetInit  $SID

    # Read the set, copy the codebooks/ distribs
    set fp [open $rewriteRules r]
    while {[gets $fp line] != -1} {
	if {[regexp "^;" $line]} { continue }
	set from [lindex $line 0]; set to [lindex $line 1]
	puts stderr "    ReWriting $from -> $to"
	catch { codebookSet$SID:$to := cbs:$from }
	catch { distribSet$SID:$to  := dss:$from }
    }
    close $fp
}

distribTreeInit $SID
senoneSetInit   $SID  distribStream$SID   
topoSetInit     $SID
ttreeInit       $SID
dictInit        $SID 
trainInit       $SID
dbaseInit       $SID $dbaseName


# ------------------------------------------------------------------------
#  Initialize Adaptation
# ------------------------------------------------------------------------
 
writeLog stderr "Adaption : Depth= $depth, minCount: $minCount"
 
codebookSet$SID createAccus
distribSet$SID  createAccus
 
writeLog stderr "Creating MLAdaptation object"
MLAdapt mla${SID} codebookSet${SID}
 
writeLog stderr "Defining codebooks as classes in MLAdapt object"
foreach cb [codebookSet${SID}:] { mla${SID} add $cb }
 
writeLog stderr "mla${SID} cluster : [mla${SID} cluster -depth $depth]"


# ------------------------------------------------------------------------
#  Write Labels
# ------------------------------------------------------------------------

labelsMLAdaptWrite $SID $spkLst mla${SID} -labelPath $labelPath \
                   -optWord $optWord -variants $variants        \
                   -minCount $minCount -tryMax $tryMax -putPath 1

touch labels.DONE

exit
