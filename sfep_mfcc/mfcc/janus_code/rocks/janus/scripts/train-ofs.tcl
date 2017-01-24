# ========================================================================
# SWB02, train semi-tied covariance models
#
# Author : Hagen Soltau
# Date   : Wed Mar 20 17:10:35 CET 2002
# $Log: train.tcl,v $
# Revision 1.1  2002/03/21 12:28:41  soltau
# Initial revision
#
# ========================================================================


# ------------------------------------------------------------------------
#  Settings
# ------------------------------------------------------------------------

set descFile   "../desc/desc.tcl"
set samplePath "data"
set accuPath   "Accus"
set paramPath  "Weights"
set qCounter    0
set lhCounter   0
set protPath   "descOFS"
set semFile    "mas.DONE"
set doConvert   1

set stepN       100
set smallSteps  20
set firstSmall  40
set deltaThres  0.05

set beg        ""
set end         4
set convLst    "ofsList"
set id         "ALL"

set cbsDesc    "$paramPath/final.cbsDesc.gz"
set dssDesc    "$paramPath/final.dssDesc.gz"


# -----------------------------------------------------------------------
#  Read in command-line options (override settings)
# -----------------------------------------------------------------------

if [catch {itfParseArgv $argv0 $argv [list [
    list -client      string  {} doParallelBODYONLY   {} {do body only in doParallel}   ] [
    list -server      string  {} doParallelSERVERONLY {} {do server only in doParallel} ] [
    list -spkList     string  {} convLst        {} {list of keys} ] [
    list -start       int     {} beg            {} {start with epoch} ] [
    list -end         int     {} end            {} {last epoch to train} ] [
    list -convert     int     {} doConvert      {} {convert CBs at last epoch} ] [
    list -desc        string  {} descFile       {} {description file} ] ]
} ] {
    puts stderr "ERROR in $argv0 while scanning options"
    exit -1
}

source $descFile

if {$beg == "" && [llength [set tmp [lindex [glob -nocomplain $convLst.\[0-9\]*] 0]]]} {
    regexp "$convLst.(\[0-9\]*)" $tmp tmp2 beg
    writeLog stderr "set beg to $beg"    
} elseif {$beg == ""} {
    set beg 1
}

waitFile $semFile


# ------------------------------------------------------------------------
#  Init Modules
# ------------------------------------------------------------------------

phonesSetInit   $SID
tagsInit        $SID
featureSetInit  $SID
parmatSetInit   $SID -desc  $protPath/parmatSet.${id} \
                     -param $paramPath/[expr $beg-1]i.${id}.pms.gz
cbnewSetInit    $SID -desc  $protPath/cbnewSet.${id}   \
                     -param $paramPath/[expr $beg-1]i.${id}.cbns.gz
cbnewTreeInit   $SID -ptree ""
senoneSetInit   $SID  cbnewStream$SID
topoSetInit     $SID
ttreeInit       $SID
dictInit        $SID
trainInit       $SID -lbox lbox$SID

condorInit      $SID


# ------------------------------------------------------------------------
#  accumulate data (sample based)
# ------------------------------------------------------------------------

proc doAccu {param samplePath convLst i} {
    global SID env accuPath
    global protPath qCounter lhCounter ;#hack to make calcProts, storLHProt happy

    cbnewSet${SID}  clearAccus   
    FMatrix smp

    while {[fgets $convLst cb] >= 0} {
	writeLog stderr "working on $cb ..."
	regsub "\\(\\|\\)" $cb "" ds
	set q [set c 0]
	foreach f [glob -nocomplain $samplePath/data.*/${cb}.smp $samplePath/data.*/${ds}.smp] {
	    smp bload $f
	    
	    set m [smp configure -m]
	    set n [smp configure -n]
	    smp resize $m [expr $n - 1]
	    
	    incr q; incr c $m
	    set cbIndex [cbnewSet$SID index $cb]
	    cbnewSet$SID accuMatrix $cbIndex smp
	}
	writeLog stderr "... $q files $c counts for $cb"
    }
    smp destroy

    cbnewSet${SID} saveAccusDep $accuPath/${param}.$env(HOST).[pid].cbna
}


# --------------------------------------------------------------------
#  main loop
# --------------------------------------------------------------------

# Make sure we have a list to work on
if {![file exists $convLst.$beg]} {
    set fp [open $convLst.$beg w]
    foreach cb [cbnewSet$SID:] { puts $fp $cb }
    close $fp
    file mkdir $protPath $accuPath
} else { after 1000 }

cbnewSet${SID} phase work

for {set i $beg} {$i <= $end} {incr i} {
    set param "${i}i.${id}"

    doParallel {	
	# --- Client ---
	puts stderr "doAccu $param $convLst.${i} $samplePath"
	doAccu "$param" $samplePath $convLst.$i $i
    } {
	# --- Server ---
	# load Accus
	cbnewSet${SID}  clearAccus
	foreach cbfile [glob $accuPath/${param}.*.cbna] {
	    printDo cbnewSet${SID} loadAccusDep $cbfile
	}
	printDo cbnewSet${SID} saveAccus $accuPath/${param}.cbna.gz

	# senone update
	writeLog stderr "senoneSet${SID} update"
	senoneSet${SID} update

	# parmatSet update
	writeLog stderr "parmatSet${SID} update -stepN $stepN -smallSteps $smallSteps -firstSmall $firstSmall -deltaThres $deltaThres"
	parmatSet${SID} update -stepN $stepN -smallSteps $smallSteps -firstSmall $firstSmall -deltaThres $deltaThres

	# evaluate Kullback-Leibler Criterion for parmatSet
	calcProts

	# save weights
	writeLog stderr "Storing weights..."
	parmatSet${SID} saveWeights $paramPath/${param}.pms.gz
	cbnewSet${SID}  saveWeights $paramPath/${param}.cbns.gz

	file delete $convLst.$i

	if {$i == $end && $doConvert} {
	    # convert codebooks
	    CodebookSet cbs featureSet$SID
	    DistribSet  dss cbs

	    cbs read $cbsDesc
	    dss read $dssDesc

	    foreach cb [cbs] { cbs:$cb alloc }

	    cbnewSet$SID convert cbs dss

	    cbs write $protPath/cbsDesc.$id
	    dss write $protPath/dssDesc.$id

	    cbs save  $paramPath/${i}i.${id}.cbs.gz
	    dss save  $paramPath/${i}i.${id}.dss.gz

	    # merge LDA and STC matrices
	    FMatrix new
	    featureSet$SID:LDAMatrix.data resize \
		[parmatSet$SID configure -dimN] [featureSet$SID:LDAMatrix.data configure -n]

	    foreach p [parmatSet$SID] {
		new mul parmatSet$SID:$p.item(0) featureSet$SID:LDAMatrix.data 
		new bsave $paramPath/${i}i.$p.bmat
	    }

	    catch {
		exec ln -s ${i}i.${id}.cbs.gz $paramPath/0i.cbs.gz
		exec ln -s ${i}i.${id}.dss.gz $paramPath/0i.dss.gz
	    }

	    touch ofs.DONE
	} else {
	    set fp [open $convLst.[expr $i+1] w]
	    foreach cb [cbnewSet$SID:] { puts $fp $cb }
	    close $fp
	}
    } {
	# --- Server Finish ---
	cbnewSet${SID}  clearAccus  ;# this is necessary to load weights properly
	parmatSet${SID} loadWeights $paramPath/${param}.pms.gz
	cbnewSet${SID}  loadWeights $paramPath/${param}.cbns.gz

	foreach cbfile [glob $accuPath/${param}.*.cbna] { file delete $cbfile }

    } {
	# --- Client ---
	cbnewSet${SID}  clearAccus  ;# this is necessary to load weights properly
	parmatSet${SID} loadWeights $paramPath/${param}.pms.gz
	cbnewSet${SID}  loadWeights $paramPath/${param}.cbns.gz
    }

    # For playing nice with Condor
    if {[info exists MostRecentlyReceivedSignal] && $MostRecentlyReceivedSignal == "TERM" &&
	[info exists doParallelBODYONLY]         && $doParallelBODYONLY} { break }
}

exit
