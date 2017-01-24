# ========================================================================
#  JanusRTK   Janus Speech Recognition Toolkit
#             ------------------------------------------------------------
#             Object: CMS extraction
#             ------------------------------------------------------------
#
#  Author  :  The Ibis Gang
#  Module  :  means.tcl
#  Date    :  $Id: means.tcl 2390 2003-08-14 11:20:32Z fuegen $
#
#  Remarks :  SWB02-style cepstral mean extraction
#
# ========================================================================
# 
#   $Log$
#   Revision 1.2  2003/08/14 11:19:45  fuegen
#   Merged changes on branch jtk-01-01-15-fms (jaguar -> ibis-013)
#
#   Revision 1.1.2.3  2002/07/30 07:44:07  metze
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
#  Settings
# ------------------------------------------------------------------------

set spkLst    train-spk.means
set descFile  ../desc/desc.tcl
set semFile   ""


# -----------------------------------------------------------------------
#  Read in command-line options (override settings)
# -----------------------------------------------------------------------

if [catch {itfParseArgv $argv0 $argv [list [
    list -spkLst      string  {} spkLst        {} {speaker list}     ] [
    list -desc        string  {} descFile      {} {description file} ] [
    list -meanPath    string  {} meanPath      {} {path for means} ] [
    list -semFile     string  {} semFile       {} {semaphore file}   ] ]
} ] {
    puts stderr "ERROR in $argv0 while scanning options"
    exit -1
}

source $descFile

foreach i "optWord variants dbaseName meanFeatureSetDesc meanPath" {
    if {![info exists $i] || [set $i] == ""} { eval "set $i \$${SID}($i)" }
}

waitFile "$semFile"


# ------------------------------------------------------------------------
#  Init Modules
# ------------------------------------------------------------------------

phonesSetInit   $SID
tagsInit        $SID
featureSetInit  $SID -lda "" -desc $meanFeatureSetDesc
vtlnInit        $SID
dbaseInit       $SID $dbaseName

if {![file isdirectory $meanPath]} { exec mkdir $meanPath }


# ------------------------------------------------------------------------
#  Compute Means for all conversations in list
# ------------------------------------------------------------------------

while {[fgets $spkLst spk] >= 0} {

    if {[file exists $meanPath/$spk.mean]} {
	puts stderr "Skipped $spk"
	continue
    }

    FVector featureSet${SID}UpMean   13
    FVector featureSet${SID}UpSMean  13

    featureSet${SID}UpMean  configure -count 0 
    featureSet${SID}UpSMean configure -count 0 

    # Loop over all utterances
    foreachSegment utt uttDB $spk {
	set       uttInfo  [uttDB uttInfo $spk $utt]
	makeArray uttArray $uttInfo
	featureSet${SID} eval $uttInfo
    }
    writeLog stderr "Speaker $spk count [featureSet${SID}UpMean configure -count]"

    printDo featureSet${SID}UpMean   bsave   $meanPath/$spk.mean
    printDo featureSet${SID}UpSMean  bsave   $meanPath/$spk.smean

    featureSet${SID}UpMean  destroy
    featureSet${SID}UpSMean destroy
    
}

touch means.DONE

exit
