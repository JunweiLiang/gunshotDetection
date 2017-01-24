# ========================================================================
#  JanusRTK   Janus Speech Recognition Toolkit
#             ------------------------------------------------------------
#             Object: LDA computation
#             ------------------------------------------------------------
#
#  Author  :  The Ibis Gang
#  Module  :  lda.tcl
#  Date    :  $Id: lda.tcl 2404 2003-08-22 09:07:39Z metze $
#
#  Remarks :  Computes an LDA matrix
#
# ========================================================================
# 
#   $Log$
#   Revision 1.3  2003/08/22 09:07:39  metze
#   Now waits for means.DONE
#
#   Revision 1.2  2003/08/14 11:19:44  fuegen
#   Merged changes on branch jtk-01-01-15-fms (jaguar -> ibis-013)
#
#   Revision 1.1.2.3  2002/07/30 07:43:43  metze
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
#  Settings for LDA
# ------------------------------------------------------------------------

set spkLst      train-spk.lda
set descFile    ../desc/desc.tcl
set semFile    "means.DONE"

set feature     FEAT+        
set dim         143


# -----------------------------------------------------------------------
#  Read in command-line options (override settings)
# -----------------------------------------------------------------------
if [catch {itfParseArgv $argv0 $argv [list [
    list -spkLst   string  {} spkLst         {} {speaker list} ] [
    list -desc     string  {} descFile       {} {description file} ] [
    list -meanPath string  {} meanPath       {} {path for means} ] [
    list -semFile  string  {} semFile        {} {semaphore file} ] [
    list -featDesc string  {} featureSetDesc {} {featDesc to use} ] ]
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
featureSetInit  $SID -lda "" -desc $featureSetDesc
vtlnInit        $SID
codebookSetInit $SID -param ""
distribSetInit  $SID -param ""
distribTreeInit $SID -ptree ""
senoneSetInit   $SID  distribStream$SID
topoSetInit     $SID
ttreeInit       $SID
dictInit        $SID 
trainInit       $SID
dbaseInit       $SID $dbaseName


# ------------------------------------------------------------------------
#  LDA Definition and mapping
# ------------------------------------------------------------------------

writeLog stderr "\nInit LDA: $feature $dim"

LDA lda${SID} featureSet${SID} $feature $dim

foreach cb [codebookSet${SID}:] { lda${SID} add $cb }

foreach ds [distribSet${SID}:] {
  set cb [codebookSet${SID} name [distribSet${SID}:$ds configure -cbX]]
  lda${SID} map [distribSet${SID} index $ds] -class $cb
}


# ------------------------------------------------------------------------
#  LDA:
# ------------------------------------------------------------------------

doLDA $SID lda${SID} $spkLst -labelPath $labelPath \
     -optWord $optWord -variants $variants         \
     -stream 0 -countsFile lda${SID}.counts

touch lda.DONE

exit
