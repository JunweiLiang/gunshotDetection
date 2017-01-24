# ========================================================================
#  JanusRTK   Janus Speech Recognition Toolkit
#             ------------------------------------------------------------
#             Object: K-Means training
#             ------------------------------------------------------------
#
#  Author  :  The Ibis Gang
#  Module  :  kmeans.tcl
#  Date    :  $Id: kmeans.tcl 2390 2003-08-14 11:20:32Z fuegen $
#
#  Remarks :  
#
# ========================================================================
# 
#   $Log$
#   Revision 1.2  2003/08/14 11:19:44  fuegen
#   Merged changes on branch jtk-01-01-15-fms (jaguar -> ibis-013)
#
#   Revision 1.1.2.3  2002/07/30 07:43:26  metze
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
#  Settings for kmeans
# ------------------------------------------------------------------------

set descFile     ../desc/desc.tcl
set semFile      samples.DONE

set maxIter      10
set tempF        0.01

set cbsLst       train-spk.samples.cbs
set paramFile    Weights/0
set dataPath     data
set kmeansPath   ${dataPath}/kmeans


# -----------------------------------------------------------------------
#  Read in command-line options (override settings)
# -----------------------------------------------------------------------
if [catch {itfParseArgv $argv0 $argv [list [
    list -spkLst      string  {} spkLst         {} {list of speakers} ] [
    list -cbsLst      string  {} cbsLst         {} {list of codebooks} ] [
    list -desc        string  {} descFile       {} {description file} ] [
    list -maxIter     int     {} maxIter        {} {number of iterations} ] [
    list -tempF       float   {} tempF          {} {temperature} ] [
    list -dataPath    string  {} dataPath       {} {path for samples} ] [
    list -kmeansPath  string  {} kmeansPath     {} {path for kmeans files} ] [
    list -semFile     string  {} semFile        {} {semaphore file} ] ]
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
featureSetInit  $SID -desc "" -access "" -lda ""
codebookSetInit $SID -param ""
distribSetInit  $SID -param ""

file mkdir $dataPath


# ------------------------------------------------------------------------
#  KMeans with one data source
# ------------------------------------------------------------------------

doKMeans $SID $cbsLst -maxIter $maxIter -tempF $tempF -paramFile $paramFile \
         -dataPath $dataPath -kmeansPath $kmeansPath -doCombine 1

touch kmeans.DONE

exit
