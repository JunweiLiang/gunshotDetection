# ========================================================================
#  JanusRTK   Janus Speech Recognition Toolkit
#             ------------------------------------------------------------
#             Object: MAP adaptation
#             ------------------------------------------------------------
#
#  Author  :  The Ibis Gang
#  Module  :  map.tcl
#  Date    :  $Id: map.tcl 2482 2004-08-16 15:44:01Z metze $
#
#  Remarks :  Copied from /project/nespole/sys/mllr
#             Not checked
#
# ========================================================================
# 
#   $Log$
#   Revision 1.2  2004/08/16 15:44:00  metze
#   Seems to be reasonable
#
#
# ========================================================================


# ------------------------------------------------------------------------
#  Settings for MAP
# ------------------------------------------------------------------------

source ../desc.tcl
randomInit [pid]


# ------------------------------------------------------------------------
#  Settings for accumulate data
# ------------------------------------------------------------------------

set map        0.8
set weightName Weights/MAPi2
set accuName   Accus/MAPi2

set param1     /project/sst/VMeval98/Q33b/train4/Accus/5
set param2     Accus/MAPi2

set count1     [expr 23330331 - 1560838] ; # from /project/sst/VMeval98/Q33b/train/ldaQ33.counts
set count2     [expr   447598 -   81611] ; # from /project/nespole/sys/mllr/train.h323/ldamam.counts 

set semFile    ""

if {! $tcl_interactive} {
  if [catch {itfParseArgv $argv0 $argv [list [
    list -convList  string  {} convLst        {} {list of conversations}] [
    list -dbase     string  {} dbase          {} {database}] [
    list -featDesc   string {} featDesc       {} {feature description file} ] [
    list -featAccess string {} featAccess     {} {feature access file} ] [
    list -warpFile   string {} warpFile       {} {file with warp factors} ] [
    list -minCount   int    {} minCount       {} {min adapt set size} ] [
    list -beam       int    {} beam           {} {viterbi beam} ] [
    list -labelPath  string {} labelPath      {} {path specification for labels}      ] [
    list -param      string {} param          {} {parameter description}      ] [
    list -semFile    string {} semFile        {} {semaphore file}]]}] {
    exit 1
  }
}

waitFile "$semFile"


# ------------------------------------------------------------------------
#  Init Modules
# ------------------------------------------------------------------------

phonesSetInit   $SID 
tagsInit        $SID
featureSetInit  $SID
codebookSetInit $SID
distribSetInit  $SID
distribTreeInit $SID -ptree ""
senoneSetInit   $SID  distribStream$SID
topoSetInit     $SID 
ttreeInit       $SID 
dictInit        $SID 
trainInit       $SID


# --------------------------------------------------------------------
#  Main Part
# --------------------------------------------------------------------

codebookSet${SID} createAccus
distribSet${SID}  createAccus

# Accus 1
set factor [expr $map          * ($count1 + $count2) / $count1]
puts "loadAccus : $param1  with factor = $factor"
codebookSet${SID} loadAccus $param1.cba.gz -factor $factor
distribSet${SID}  loadAccus $param1.dsa.gz -factor $factor

# Accus 2
set factor [expr (1.0 - $map ) * ($count1 + $count2) / $count2]
puts "loadAccus : $param2  with factor = $factor"
codebookSet${SID} loadAccus $param2.cba.gz -factor $factor
distribSet${SID}  loadAccus $param2.dsa.gz -factor $factor

# Mixing
puts "senoneSet$SID update"
senoneSet$SID update

# Writing out
puts "write weights into $weightName.$map"
codebookSet${SID} save      ${weightName}.$map.cbs.gz
distribSet${SID}  save      ${weightName}.$map.dss.gz

codebookSet${SID} saveAccus ${accuName}.$map.cba.gz
distribSet${SID}  saveAccus ${accuName}.$map.dsa.gz

touch map.DONE

exit
