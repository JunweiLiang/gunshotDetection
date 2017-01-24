# ========================================================================
#  JanusRTK   Janus Speech Recognition Toolkit
#             ------------------------------------------------------------
#             Object: MLLR adaptation
#             ------------------------------------------------------------
#
#  Author  :  The Ibis Gang
#  Module  :  mllr.tcl
#  Date    :  $Id: mllr.tcl 2482 2004-08-16 15:44:01Z metze $
#
#  Remarks :  Copied from /project/nespole/sys/mllr
#             Not checked
#
# ========================================================================
# 
#   $Log$
#   Revision 1.2  2004/08/16 15:44:01  metze
#   Seems to be reasonable
#
#
# ========================================================================


# ------------------------------------------------------------------------
#  Settings for MLLR
# ------------------------------------------------------------------------

source ../desc.tcl
randomInit [pid]


# ------------------------------------------------------------------------
#  Settings for accumulate data
# ------------------------------------------------------------------------

set weightName Weights/H323i2
set accuName   Accus/H323i2
set depth      8
set adaptVar   0
set minCount   6000
set semFile    "accu.DONE"

if {! $tcl_interactive} {
  if [catch {itfParseArgv $argv0 $argv [list [
    list -convList   string {} convLst        {} {list of conversations}         ] [
    list -dbase      string {} dbase          {} {database}                      ] [
    list -featDesc   string {} featDesc       {} {feature description file}      ] [
    list -featAccess string {} featAccess     {} {feature access file}           ] [
    list -warpFile   string {} warpFile       {} {file with warp factors}        ] [
    list -minCount   int    {} minCount       {} {min adapt set size}            ] [
    list -beam       int    {} beam           {} {viterbi beam}                  ] [
    list -labelPath  string {} labelPath      {} {path specification for labels} ] [
    list -param      string {} param          {} {parameter description}         ] [
    list -semFile    string {} semFile        {} {semaphore file}             ]]}] {
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
codebookSetInit $SID -param Weights/H323train.8.6000.cbs.gz
distribSetInit  $SID -param Weights/H323train.8.6000.dss.gz
distribTreeInit $SID -ptree ""
senoneSetInit   $SID  distribStream$SID
topoSetInit     $SID 
ttreeInit       $SID 
dictInit        $SID 
trainInit       $SID


# --------------------------------------------------------------------
#  Main Loop
# --------------------------------------------------------------------

codebookSet${SID} createAccus
distribSet${SID}  createAccus

puts "Adaptation : Depth= $depth, minCount: $minCount"
MLAdapt mla${SID} codebookSet$SID
mla${SID} store

foreach cb [codebookSet${SID}:] { 
    mla${SID} add $cb
}
writeLog stderr "Creating MLAdaptation tree [mla${SID} cluster -depth $depth]"

# Load accus
puts "loadAccus : $accuName"
codebookSet${SID} loadAccus $accuName.cba.gz
distribSet${SID}  loadAccus $accuName.dsa.gz    

# Adapt data
if {!$adaptVar} {
    puts "mla$SID update   -minCount $minCount :  [mla$SID update   -minCount $minCount ]"
} else {
    puts "mla$SID variance -minCount $minCount :  [mla$SID variance -minCount $minCount ]"
}
puts "write weights into $weightName.$depth.$minCount"
codebookSet${SID} save ${weightName}.$depth.$minCount.cbs.gz
distribSet${SID}  save ${weightName}.$depth.$minCount.dss.gz

# Clean up (if we wanted to loop)
codebookSet${SID} clearAccus
distribSet${SID}  clearAccus
mla$SID restore

touch mllr.DONE

exit
