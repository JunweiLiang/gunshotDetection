# ========================================================================
#  JanusRTK   Janus Speech Recognition Toolkit
#             ------------------------------------------------------------
#             Object: Merge and Split training
#             ------------------------------------------------------------
#
#  Author  :  The Ibis Gang
#  Module  :  train-mas.tcl
#  Date    :  $Id: $
#
#  Remarks :  
#
# ========================================================================
# 
#   $Log: samples.tcl,v $
#
# ========================================================================


# ------------------------------------------------------------------------
#  Settings for train
# ------------------------------------------------------------------------

set convLst    "train-spk.mas"
set descFile   "../desc/desc.tcl"
set dataPath   "data"
set semFile    "samples.DONE"

set feature     LDA
set dimN        32
set refMax      32
set mergeThresh 50
set splitStep   0.001

set bigN        8
set smallN      4


# -----------------------------------------------------------------------
#  Read in command-line options (override settings)
# -----------------------------------------------------------------------

if [catch {itfParseArgv $argv0 $argv [list [
    list -client      string  {} doParallelBODYONLY   {} {do body only in doParallel}   ] [
    list -server      string  {} doParallelSERVERONLY {} {do server only in doParallel} ] [
    list -spkLst      string  {} convLst        {} {speaker list} ] [
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

foreach i "optWord variants dbaseName distribSetDesc codebookSetDesc" {
    if {![info exists $i] || [set $i] == ""} { eval "set $i \$${SID}($i)" }
}

waitFile "$semFile"


# ------------------------------------------------------------------------
#  Init Modules
# ------------------------------------------------------------------------

phonesSetInit   $SID
tagsInit        $SID
featureSetInit  $SID -desc ""
codebookSetInit $SID -desc "" -param ""
distribSetInit  $SID -desc "" -param ""

condorInit      $SID


# --------------------------------------------------------------------
#  doAccu
# --------------------------------------------------------------------

proc doTrain { cb } {
    global SID dataPath feature dimN refMax mergeThresh splitStep bigN smallN

    # codebook to distrib mapping
    set ds $cb    
   
    # load data
    set smp  featureSet$SID:${feature}.data
    set mode 0    
    foreach f [glob -nocomplain $dataPath/data*/$cb.smp] {
	puts "load $f" 
	$smp cload $f -append $mode
	set mode 1
    }
    if {!$mode} {
	puts "$cb : couldn't find samples"
	# create codebook and distrib
	codebookSet$SID add $cb $feature 1 $dimN DIAGONAL
	distribSet$SID  add $ds $cb
	return
    }
    $smp resize [$smp configure -m] [expr [$smp configure -n] -1]
    puts "$cb : got [$smp configure -m] samples"

    # create codebook and distrib
    codebookSet$SID add $cb $feature 1 $dimN DIAGONAL
    distribSet$SID  add $ds $cb

    # max. nr. of components
    codebookSet$SID:$cb     configure -refMax $refMax

    # mincount per component
    codebookSet$SID:$cb.cfg configure -mergeThresh $mergeThresh

    # step size for spliting components
    codebookSet$SID:$cb.cfg configure -splitStep $splitStep

    codebookSet$SID:$cb createAccu
    distribSet$SID:$ds  createAccu

    # big iterations with increasing components
    for {set i 0} {$i < $bigN} {incr i} { 
	puts "$cb : iter= $i  refN= [codebookSet$SID:$cb configure -refN]"

	# accumulate data
	codebookSet$SID:$cb.accu clear
	distribSet$SID:$ds.accu  clear
	distribSet${SID} accuFrame $cb 0 -toframe [$smp configure -m]

	# update, split and merge
	distribSet$SID update
	distribSet$SID split
	distribSet$SID merge

	# small iterations without increasing components
	for {set j 0} {$j < $smallN} {incr j} { 
	    codebookSet$SID:$cb.accu clear
	    distribSet$SID:$ds.accu  clear
	    distribSet${SID} accuFrame $ds 0 -toframe [$smp configure -m]
	    distribSet$SID update
	    if {[condorCheckAbort]} { break }
	}
	if {[condorCheckAbort]} { break }
    }
    codebookSet$SID:$cb freeAccu
    distribSet$SID:$ds  freeAccu
    if {[condorCheckAbort]} {
	# This key could not be completed successfully
	printDo distribSet$SID  delete $ds
	printDo codebookSet$SID delete $cb
    }
}


# --------------------------------------------------------------------
#  main loop
# --------------------------------------------------------------------

featureSet$SID FMatrix $feature
featureSet$SID:${feature}.data resize 1 $dimN

if {![file exists $convLst]} {
    set fp [open $convLst w]
    codebookSetInit $SID -codebookSet C -param "" -desc $codebookSetDesc
    foreach cb [C] { puts $fp $cb }; close $fp
    file mkdir Weights
} else { after 2000 }

doParallel { 
    # --- Body ---
    while {[fgets $convLst cb] >= 0} { doTrain $cb } 
    codebookSet$SID write Weights/$env(HOST).[pid].cbsDesc
    codebookSet$SID save  Weights/$env(HOST).[pid].cbs.gz
    distribSet$SID  write Weights/$env(HOST).[pid].dssDesc
    distribSet$SID  save  Weights/$env(HOST).[pid].dss.gz
} {
    # --- Server ---
    codebookSetInit $SID -codebookSet cbs -param "" -desc ""
    distribSetInit  $SID -distribSet  dss -param "" -desc "" \
	                 -codebookSet cbs

    foreach f [glob Weights/*.*.cbsDesc] { printDo cbs read $f }
    foreach f [glob Weights/*.*.dssDesc] { printDo dss read $f }
    foreach f [glob Weights/*.*.cbs.gz]  { printDo cbs load $f }
    foreach f [glob Weights/*.*.dss.gz]  { printDo dss load $f }

    # make sure we have all distributions
    distribSetInit  $SID -distribSet  D   -param "" -desc $distribSetDesc \
	                 -codebookSet cbs
    foreach ds [D] {
	if {[catch {
	    set cb [cbs name [D:$ds configure -cbX]]
	    D:$ds := dss:$cb 
	} msg]} {
	    puts "WARNING $ds: $msg"
	}
    }

    printDo cbs write Weights/final.cbsDesc.gz
    printDo cbs save  Weights/final.cbs.gz
    printDo D   write Weights/final.dssDesc.gz
    printDo D   save  Weights/final.dss.gz

} { 
    # --- Server Finish ---
    touch mas.DONE
} { 
    # --- Client ---
}

exit
