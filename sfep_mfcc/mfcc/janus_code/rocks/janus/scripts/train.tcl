# ========================================================================
#  JanusRTK   Janus Speech Recognition Toolkit
#             ------------------------------------------------------------
#             Object: EM training
#             ------------------------------------------------------------
#
#  Author  :  The Ibis Gang
#  Module  :  train.tcl
#  Date    :  $Id: train.tcl 2894 2009-07-27 15:55:07Z metze $
#
#  Remarks :  
#
# ========================================================================
# 
#   $Log$
#   Revision 1.4  2003/12/17 13:07:16  metze
#   scripts/master.tcl
#
#   Revision 1.3  2003/08/22 09:09:24  metze
#   Initial weights don't have to exist when script is started
#
#   Revision 1.2  2003/08/14 11:19:47  fuegen
#   Merged changes on branch jtk-01-01-15-fms (jaguar -> ibis-013)
#
#   Revision 1.1.2.4  2003/08/11 14:31:10  soltau
#   fixed itfParseArgv handling
#
#   Revision 1.1.2.3  2002/07/30 07:44:36  metze
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
#  Settings for train
# ------------------------------------------------------------------------

set spkLst           train-spk
set descFile         ../desc/desc.tcl
set semFile          kmeans.DONE

set beg              1
set end              4

set codebookSetParam Weights/[expr $beg-1].cbs.gz
set distribSetParam  Weights/[expr $beg-1].dss.gz


# -----------------------------------------------------------------------
#  Read in command-line options (override settings)
# -----------------------------------------------------------------------

if [catch {itfParseArgv $argv0 $argv [list [
    list -spkLst      string  {} spkLst           {} {speaker list} ] [
    list -desc        string  {} descFile         {} {description file} ] [
    list -meanPath    string  {} meanPath         {} {path for means} ] [
    list -semFile     string  {} semFile          {} {semaphore file} ] [
    list -begin       int     {} beg              {} {first iteration} ] [
    list -end         int     {} end              {} {last iteration} ] [
    list -cbsDesc     string  {} codebookSetDesc  {} {codebook descriptions} ] [
    list -cbsParam    string  {} codebookSetParam {} {codebook parameters} ] [
    list -dssDesc     string  {} distribSetDesc   {} {distribution descriptions} ] [
    list -dssParam    string  {} distribSetParam  {} {distribution parameters} ] [
    list -dstDesc     string  {} distribTreeDesc  {} {distribution tree description} ] [
    list -ptreeDesc   string  {} ptreeSetDesc     {} {ptree description} ] ]
} msg] {
    puts stderr "ERROR in $argv0 while scanning options: $msg"
    exit -1
}

source $descFile

foreach i "optWord variants dbaseName featureSetDesc meanPath \
           codebookSetDesc codebookSetParam distribSetDesc distribSetParam \
           distribTreeDesc ptreeSetDesc" {
    if {![info exists $i]} { eval "set $i \$${SID}($i)" }
}
eval "set labelPath \[string range \$${SID}(labelPath) 0 end\]"

waitFile "$semFile"

file mkdir Accus Weights


# ------------------------------------------------------------------------
#  Init Modules
# ------------------------------------------------------------------------

phonesSetInit   $SID
tagsInit        $SID
featureSetInit  $SID
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


# --------------------------------------------------------------------
#  doAccu
# --------------------------------------------------------------------

proc doAccu {spkLst i labelPath warpFile} {
    global SID optWord variants env warpScales WARPSCALE
    global $SID ;# HACK to make findLabelWarp happy

    puts "doAccu $spkLst $i $labelPath $warpFile"

    codebookSet${SID} clearAccus
    distribSet${SID}  clearAccus

    while {[fgets $spkLst spk] >= 0} {

	if [info exists warpScales($spk)] {
	    set cwarp $warpScales($spk) 
	} else {
	    set cwarp 1.0
	    puts "WARNING: use default 1.0 warping factor for $spk in Iteration $i"
	}

	foreachSegment utt uttDB $spk {
	    # set WARPSCALE $warpScales($spk)
	    set       uttInfo  [uttDB uttInfo $spk $utt]
	    makeArray uttArray $uttInfo		
	    writeLog stderr "$i: %TURN: $spk $utt"
	    eval set label $labelPath		
	    if [catch {set score [labelUtterance $SID $spk $utt $label -optWord $optWord -variants $variants]} msg] {
		writeLog stderr $msg
	    } else {
		writeLog stderr "$i: accu path; score: $score"
		senoneSet${SID} accu path$SID
	    }
	}
    }	
    codebookSet${SID} saveAccus Accus/${i}.$env(HOST).[pid].cba
    distribSet${SID}  saveAccus Accus/${i}.$env(HOST).[pid].dsa
} 


# --------------------------------------------------------------------
#  main loop
# --------------------------------------------------------------------

codebookSet${SID} createAccus
distribSet${SID}  createAccus

if {![file exists $spkLst.$beg]} { file copy $spkLst $spkLst.$beg }

for {set i $beg} {$i <= $end} {incr i} {   

    set warpFile  "Warps/$i.$env(HOST).[pid].warp"

    doParallel {
	set warpFile  "Warps/$i.$env(HOST).[pid].warp"
	doAccu $spkLst.${i} ${i} $labelPath $warpFile
    } {

	codebookSet${SID} clearAccus
	distribSet${SID}  clearAccus

	writeLog stderr "$i: codebookSet${SID} loadAccus [glob Accus/${i}.*.cba]"
	writeLog stderr "$i: distribSet${SID}  loadAccus [glob Accus/${i}.*.dsa]"
    
	foreach cbfile [glob Accus/${i}.*.cba] { codebookSet${SID} loadAccus $cbfile }
	foreach dsfile [glob Accus/${i}.*.dsa] { distribSet${SID}  loadAccus $dsfile }
    
	codebookSet${SID} saveAccus Accus/${i}.cba.gz
	distribSet${SID}  saveAccus Accus/${i}.dsa.gz
	
	foreach cbfile [glob Accus/${i}.*.cba] { rm $cbfile }
	foreach dsfile [glob Accus/${i}.*.dsa] { rm $dsfile }

	writeLog stderr "senoneSet${SID} update"
	senoneSet${SID} update
    
	# save Codebook, Distribution
	codebookSet${SID} save Weights/${i}.cbs.gz
	distribSet${SID}  save Weights/${i}.dss.gz 

	file copy $spkLst $spkLst.[expr $i +1]

	if {$i == $end} {
	    set flist [glob -nocomplain "$spkLst.*"]
	    foreach f $flist { catch { rm $f } }
	    set flist [glob -nocomplain "*.DONE"]
	    foreach f $flist { catch { rm $f } }
	    touch train.DONE
	}

    } { } {
	if {$i < $end} {
	    codebookSet${SID} load  Weights/${i}.cbs.gz
	    distribSet${SID}  load  Weights/${i}.dss.gz
	}
    }
    
}

exit
