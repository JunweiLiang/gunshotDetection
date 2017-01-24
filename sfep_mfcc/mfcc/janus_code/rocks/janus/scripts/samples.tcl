# ========================================================================
#  JanusRTK   Janus Speech Recognition Toolkit
#             ------------------------------------------------------------
#             Object: Sample extraction
#             ------------------------------------------------------------
#
#  Author  :  The Ibis Gang
#  Module  :  samples.tcl
#  Date    :  $Id: samples.tcl 2390 2003-08-14 11:20:32Z fuegen $
#
#  Remarks :  
#
# ========================================================================
# 
#   $Log$
#   Revision 1.2  2003/08/14 11:19:46  fuegen
#   Merged changes on branch jtk-01-01-15-fms (jaguar -> ibis-013)
#
#   Revision 1.1.2.3  2002/07/30 07:44:15  metze
#   Bugfix for variables
#
#   Revision 1.1.2.2  2002/07/12 16:29:55  metze
#   Approved version
#
#   Revision 1.1.2.1  2002/07/12 10:03:02  metze
#   Initial script collection
#
# ========================================================================


# ------------------------------------------------------------------------
#  Settings for samples
# ------------------------------------------------------------------------

set spkLst      train-spk.samples 
set descFile    ../desc/desc.tcl
set dataPath    data
set semFile     lda.DONE

set feature     LDA
set dim         32
set maxCount    2500


# -----------------------------------------------------------------------
#  Read in command-line options (override settings)
# -----------------------------------------------------------------------

if [catch {itfParseArgv $argv0 $argv [list [
    list -spkLst      string  {} spkLst         {} {speaker list} ] [
    list -desc        string  {} descFile       {} {description file} ] [
    list -meanPath    string  {} meanPath       {} {path for means} ] [
    list -dataPath    string  {} dataPath       {} {path for samples} ] [
    list -semFile     string  {} semFile        {} {semaphore file} ] [
    list -feature     string  {} feature        {} {feature to use} ] [
    list -dimN        int     {} dim            {} {dimensionality} ] [
    list -maxCount    int     {} maxCount       {} {max count} ] [
    list -featDesc    string  {} featureSetDesc {} {featDesc to use} ] ]
} ] {
    puts stderr "ERROR in $argv0 while scanning options"
    exit -1
}

source $descFile

foreach i "optWord variants dbaseName featureSetDesc meanPath" {
    if {![info exists $i] || [set $i] == ""} { eval "set $i \$${SID}($i)" }
}
eval "set labelPath \[string range \$${SID}(labelPath) 0 end\]"

waitFile "$semFile"


# ------------------------------------------------------------------------
#  Init Modules
# ------------------------------------------------------------------------

phonesSetInit   $SID
tagsInit        $SID
featureSetInit  $SID
vtlnInit        $SID 
codebookSetInit $SID -param ""
distribSetInit  $SID -param ""
distribTreeInit $SID 
senoneSetInit   $SID  distribStream$SID
topoSetInit     $SID
ttreeInit       $SID
dictInit        $SID 
trainInit       $SID
dbaseInit       $SID $dbaseName

file mkdir $dataPath


# ------------------------------------------------------------------------
#  Samples Definition
# ------------------------------------------------------------------------

writeLog stderr "\nsmp${SID}: creating classes based on codebookSet${SID}"

SampleSet smp${SID} featureSet${SID} $feature $dim
foreach cb [codebookSet${SID}:] { smp${SID} add $cb }


# ------------------------------------------------------------------------
#  Defining a distribution to codebook map
# ------------------------------------------------------------------------
writeLog stderr "\nsmp${SID}: mapping distribSet${SID} to codebookSet${SID}"

foreach ds [distribSet${SID}:] {
  set cb [codebookSet${SID} name [distribSet${SID}:$ds configure -cbX]]
  smp${SID} map [distribSet${SID} index $ds] -class $cb
}


# ------------------------------------------------------------------------
#  Extract samples
# ------------------------------------------------------------------------

doExtract $SID smp${SID} $spkLst -labelPath $labelPath \
     -optWord $optWord -variants $variants             \
     -stream 0 -countsFile lda${SID}.counts            \
     -maxCount $maxCount -dataPath $dataPath -doCombine 0

touch samples.DONE

exit
