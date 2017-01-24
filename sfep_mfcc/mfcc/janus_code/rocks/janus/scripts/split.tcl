# ========================================================================
#  JanusRTK   Janus Speech Recognition Toolkit
#             ------------------------------------------------------------
#             Object: Split the distributions according to the questions
#             ------------------------------------------------------------
#
#  Author  :  The Ibis Gang
#  Module  :  split.tcl
#  Date    :  $Id: split.tcl 2439 2003-12-17 13:07:16Z metze $
#
#  Remarks :
#
# ========================================================================
#
#   $Log$
#   Revision 1.4  2003/12/17 13:07:14  metze
#   scripts/master.tcl
#
#   Revision 1.3  2003/08/22 09:08:00  metze
#   Added desc.tcl
#
#   Revision 1.2  2003/08/14 11:19:46  fuegen
#   Merged changes on branch jtk-01-01-15-fms (jaguar -> ibis-013)
#
#   Revision 1.1.2.3  2002/07/30 07:44:22  metze
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
#  Settings for spliting
# ------------------------------------------------------------------------

set size            2000
set splitFile       clusterList.cl
set waitFile        cluster.DONE
set descFile        ../desc/desc.tcl

set distribSetDesc  ../desc/distribSet.PT.gz
set distribTreeDesc ../desc/distribTree.PT.gz
set ptreeSetDesc    ../desc/ptreeSet.PT.gz


# -----------------------------------------------------------------------
#  Read in command-line options (override settings)
# -----------------------------------------------------------------------

if [catch {itfParseArgv $argv0 $argv [list [
    list -spkLst      string  {} spkLst           {} {speaker list} ] [
    list -desc        string  {} descFile         {} {description file} ] [
    list -semFile     string  {} semFile          {} {semaphore file} ] [
    list -size        int     {} size             {} {size of the codebook} ] [
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

waitFile $waitFile


# ------------------------------------------------------------------------
#  Init Modules
# ------------------------------------------------------------------------

phonesSetInit   $SID
tagsInit        $SID
featureSetInit  $SID -desc "" -access "" -lda ""
vtlnInit        $SID 
codebookSetInit $SID -param ""             -desc $codebookSetDesc
distribSetInit  $SID -param ""             -desc $distribSetDesc
distribTreeInit $SID -ptree $ptreeSetDesc  -desc $distribTreeDesc
senoneSetInit   $SID  distribStream$SID
topoSetInit     $SID
ttreeInit       $SID


# ------------------------------------------------------------------------
#  Split Tree
# ------------------------------------------------------------------------

if {![file exists $splitFile]} {
    set fileList [glob cluster/*.cl]
    puts stderr "merging split files $fileList into clusterList.cl"
    mergeSplits $fileList clusterList.cl 
}

# read in the split list
set nodes [treeReadSplits $splitFile]

# do the split
treeSplit distribTree$SID $nodes -leafN $size

distribTree$SID          write [set ${SID}(descPath)]/distribTree.${size}.gz
distribTree$SID.ptreeSet write [set ${SID}(descPath)]/ptreeSet.${size}.gz


# ------------------------------------------------------------------------
#  Create Backoffs
# ------------------------------------------------------------------------

puts      "distribTreeShareCBs distribTree$SID"
if [catch {distribTreeShareCBs distribTree$SID}] {
  puts "ERROR distribTreeShareCBs distribTree$SID\n$msg"
}

puts      "distribTreeAddBackoff distribTree$SID"
if [catch {distribTreeAddBackoff distribTree$SID} msg] {
  puts "ERROR distribTreeAddBackoff distribTree$SID\n$msg"
}


# ------------------------------------------------------------------------
#  Write new description files
# ------------------------------------------------------------------------

codebookSet$SID  write [set ${SID}(descPath)]/codebookSet.${size}.gz
distribSet$SID   write [set ${SID}(descPath)]/distribSet.${size}.gz
distribTree$SID  write [set ${SID}(descPath)]/distribTree.${size}.gz


# ------------------------------------------------------------------------
#  Prune tree
# ------------------------------------------------------------------------

distribSetInit  $SID -distribSet dss -codebookSet codebookSet$SID -desc "" -param ""

treePTreePrune distribTree$SID dss

dss             write [set ${SID}(descPath)]/distribSet.${size}p.gz
distribTree$SID write [set ${SID}(descPath)]/distribTree.${size}p.gz

touch split.DONE

exit
