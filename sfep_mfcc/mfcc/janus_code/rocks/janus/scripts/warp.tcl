# ========================================================================
#  JanusRTK   Janus Speech Recognition Toolkit
#             ------------------------------------------------------------
#             Object: Training script
#             ------------------------------------------------------------
#
#  Author  :  The Ibis Gang
#  Module  :  warp.tcl
#  Date    :  $Id: warp.tcl 3156 2010-02-22 23:16:04Z metze $
#
#  Remarks :  Standard EM training with warp estimation
#
# ========================================================================
#
#   $Log$
#   Revision 1.4  2004/10/22 13:08:46  metze
#   Simplified a few things
#
#   Revision 1.3  2004/10/19 08:40:58  metze
#   Added '-warpFile' option, should do more to clean up initialization
#
#   Revision 1.2  2003/12/17 13:07:16  metze
#   scripts/master.tcl
#
#   Revision 1.1  2003/08/28 12:25:45  metze
#   *** empty log message ***
#
# ========================================================================


# ------------------------------------------------------------------------
#  Settings for train
# ------------------------------------------------------------------------

set spkLst           train-spk
set descFile         ../desc/desc.tcl
set semFile          kmeans.DONE

set beg              1
set end              8

set codebookSetParam Weights/[expr $beg-1].cbs.gz
set distribSetParam  Weights/[expr $beg-1].dss.gz
set warpFile           warps/[expr $beg-1].warp
set meanPath         cms


# -----------------------------------------------------------------------
#  Read in command-line options (override settings)
# -----------------------------------------------------------------------
if [catch {itfParseArgv $argv0 $argv [list [
    list -spkLst       string  {} spkLst             {} {speaker list} ] [
    list -desc         string  {} descFile           {} {description file} ] [
    list -meanPath     string  {} meanPath           {} {path for means} ] [
    list -semFile      string  {} semFile            {} {semaphore file} ] [
    list -warpFile     string  {} warpFile           {} {warp file} ] [
    list -begin        int     {} beg                {} {first iteration} ] [
    list -end          int     {} end                {} {last iteration} ] [
    list -cbsDesc      string  {} codebookSetDesc    {} {codebook descriptions} ] [
    list -cbsParam     string  {} codebookSetParam   {} {codebook parameters} ] [
    list -dssDesc      string  {} distribSetDesc     {} {distribution descriptions} ] [
    list -dssParam     string  {} distribSetParam    {} {distribution parameters} ] [
    list -dstDesc      string  {} distribTreeDesc    {} {distribution tree description} ] [
    list -ptreeDesc    string  {} ptreeSetDesc       {} {ptree description} ] [
    list -featDesc     string  {} featureSetDesc     {} {featDesc to use} ] [
    list -meanFeatDesc string  {} meanFeatureSetDesc {} {meanFeatDesc to use} ] ]
} ] {
    puts stderr "ERROR in $argv0 while scanning options"
    exit -1
}

source $descFile

foreach i "optWord variants dbaseName featureSetDesc meanFeatureSetDesc meanPath \
           codebookSetDesc codebookSetParam distribSetDesc distribSetParam \
           distribTreeDesc ptreeSetDesc warpFile" {
    if {![info exists $i]} { eval "set $i \$${SID}($i)" }
}
eval "set labelPath \[string range \$${SID}(labelPath) 0 end\]"

waitFile "$semFile"


# ------------------------------------------------------------------------
#  Init Modules
# ------------------------------------------------------------------------

phonesSetInit   $SID 
tagsInit        $SID
featureSetInit  $SID
vtlnInit        $SID -param $warpFile
codebookSetInit $SID -desc $codebookSetDesc -param $codebookSetParam
distribSetInit  $SID -desc $distribSetDesc  -param $distribSetParam
distribTreeInit $SID -desc $distribTreeDesc -ptree $ptreeSetDesc
senoneSetInit   $SID  distribStream$SID   
topoSetInit     $SID
ttreeInit       $SID
dictInit        $SID 
trainInit       $SID
# -lbox lbox$SID
dbaseInit       $SID $dbaseName

# hmm$SID configure -rcmSdp 1


# ------------------------------------------------------------------------
#  extract new cms/cvn
# ------------------------------------------------------------------------

proc doCMS {spk warp meanPath} {
    global SID WARPSCALE WAVFILE mean featureSetDesc meanFeatureSetDesc

    puts "\ndoCMS $spk $warp $meanPath"

    set WARPSCALE $warp

    if {[file exists $meanPath/$warp/$spk.mean] &&
	[file exists $meanPath/$warp/$spk.smean]} {

	puts "  $warp already exists ...\n"
    } else {

	FVector featureSet${SID}UpMean   13
	FVector featureSet${SID}UpSMean  13
	featureSet${SID}UpMean  configure -count 0
	featureSet${SID}UpSMean configure -count 0

	printDo featureSet$SID setDesc $meanFeatureSetDesc

	foreachSegment utt uttDB $spk {
	    set       uttInfo  [uttDB uttInfo $spk $utt]
	    featureSet${SID} eval $uttInfo
	}
	
	printDo featureSet$SID setDesc $featureSetDesc
	
	catch { file mkdir $meanPath/$warp }

	featureSet${SID}UpMean  bsave $meanPath/$warp/$spk.mean
	featureSet${SID}UpSMean bsave $meanPath/$warp/$spk.smean
	featureSet${SID}UpMean  destroy
	featureSet${SID}UpSMean destroy
    }

    catch { file delete $meanPath/$spk.mean $meanPath/$spk.smean }
    catch {
	file link $meanPath/$spk.mean  $meanPath/$warp/$spk.mean
	file link $meanPath/$spk.smean $meanPath/$warp/$spk.smean
    }
    featureSet${SID}Mean  bload $meanPath/$spk.mean
    featureSet${SID}SMean bload $meanPath/$spk.smean
}


# --------------------------------------------------------------------
#  doWarp
# --------------------------------------------------------------------

proc doWarp {spk i labelPath warp meanPath} {
    global SID env warpScales WARPSCALE optWord variants
    global $SID

    doCMS $spk $warp $meanPath

    puts "\ndoWarp $spk $warp $meanPath $labelPath"

    foreachSegment utt uttDB $spk {
	set       uttInfo  [uttDB uttInfo $spk $utt] 
	makeArray uttArray $uttInfo		
	eval set label $labelPath
	if {[catch {set score [labelUtterance $SID $spk $utt $label -evalFES 0 \
				   -optWord $optWord -variants $variants]} msg]} {
	    writeLog stderr $msg
	    continue
	}
	# lbox$SID:$utt
	scoreWarpLst frameN scoreA $SID path$SID hmm$SID $uttInfo \
	    [list $warp] [phonesSet$SID:[set ${SID}(warpPhones)]]
    }
    set score [expr $scoreA($warp) / $frameN($warp)]
    puts "doWarp: $spk $warp : $score"

    return $score
}


# --------------------------------------------------------------------
#  doVTLN
# --------------------------------------------------------------------

proc doVTLN {spk i labelPath warpFile} {
    global SID env warpScales WARPSCALE meanPath
    global $SID

    if { $warpFile == "" } { return }

    puts "\ndoVTLN $spk $i $labelPath $warpFile"

    if {![info exists warpScales($spk)]} { set warpScales($spk) 1.0 }

    set delta [expr ($i == 1) ? 0.16 : 0.08]
    set mid   $warpScales($spk)
    set left  [expr $mid - $delta]
    set right [expr $mid + $delta] 

    set scoreA($mid)   [doWarp $spk $i $labelPath $mid   $meanPath]
    set scoreA($left)  [doWarp $spk $i $labelPath $left  $meanPath]
    set scoreA($right) [doWarp $spk $i $labelPath $right $meanPath]
 
    set tryN 0
    while {$tryN < 10} {
	incr tryN
	if {$scoreA($mid) < $scoreA($left) && $scoreA($mid) < $scoreA($right) } {
	    set delta [expr $delta / 2]
	    set left  [expr $mid - $delta]
	    set right [expr $mid + $delta] 
	} else {
	    if {$scoreA($left) < $scoreA($right)} {
		set left  [expr $left  - $delta]
		set mid   [expr $mid   - $delta]
		set right [expr $right - $delta]
	    } else {
		set left  [expr $left  + $delta]
		set mid   [expr $mid   + $delta]
		set right [expr $right + $delta]
	    }
	}
	if {$delta < 0.01} { break }
	if {![info exists scoreA($left)]} {
	    set scoreA($left)  [doWarp $spk $i $labelPath $left  $meanPath]
	}
	if {![info exists scoreA($right)]} {
	    set scoreA($right) [doWarp $spk $i $labelPath $right $meanPath]
	}
	if {![info exists scoreA($mid)]} {
	    set scoreA($mid)   [doWarp $spk $i $labelPath $mid   $meanPath]
	}
    }

    set score 9e9
    foreach warp [array names scoreA] {
	if {$scoreA($warp) < $score} {
	    set score $scoreA($warp)
	    set best  $warp
	}
    }

    set fp [open $warpFile a]; puts $fp "$spk $best"; close $fp
    set warpScales($spk) $best
    doCMS $spk $best $meanPath

    puts "doVTLN $spk : new warp estimation : $best" 
    unset WARPSCALE
}


# --------------------------------------------------------------------
#  doAccu
# --------------------------------------------------------------------

proc doAccu {spkLst i labelPath warpFile} {
    global SID optWord variants env end

    puts "\ndoAccu $spkLst $i $labelPath $warpFile"

    codebookSet${SID} clearAccus
    distribSet${SID}  clearAccus

    while {[fgets $spkLst spk] >= 0} {

	doVTLN $spk $i $labelPath $warpFile

	foreachSegment utt uttDB $spk {
	    set       uttInfo  [uttDB uttInfo $spk $utt] 
	    makeArray uttArray $uttInfo		
	    writeLog stderr "$i: %TURN: $spk $utt"
	    eval set label $labelPath
	    if {$i == $end} {
		set e [catch {set score [viterbiUtterance $SID $spk $utt \
					     -optWord $optWord -variants $variants]} msg]
	    } else {
		set e [catch {set score [labelUtterance $SID $spk $utt $label \
					     -optWord $optWord -variants $variants]} msg]
	    }
	    if {$e} {
		writeLog stderr $msg
	    } else {
		senoneSet${SID} accu path$SID
		# lbox$SID:$utt -from $from -to $to
	    }
	}
    }

    codebookSet${SID} saveAccus Accus/${i}.$env(HOST).[pid].cba
    distribSet${SID}  saveAccus Accus/${i}.$env(HOST).[pid].dsa
} 


# --------------------------------------------------------------------
#  main loop
# --------------------------------------------------------------------

catch { file mkdir warps }

codebookSet${SID} createAccus
distribSet${SID}  createAccus

FVector featureSet${SID}Mean  13
FVector featureSet${SID}SMean 13

for {set i $beg} {$i <= $end} {incr i} {   
    
    set warpFile  "warps/$i.$env(HOST).[pid].warp"
    
    doParallel {
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
	
	foreach cbfile [glob Accus/${i}.*.cba] { file delete $cbfile }
	foreach dsfile [glob Accus/${i}.*.dsa] { file delete $dsfile }

	writeLog stderr "senoneSet${SID} update"
	senoneSet${SID} update
    
	#save Codebook, Distribution
	codebookSet${SID} save Weights/${i}.cbs.gz
	distribSet${SID}  save Weights/${i}.dss.gz 

	#concatenate warping factor
	set flist [glob "warps/$i.*.warp"]
	eval "exec cat $flist > warps/$i.warp"
	foreach f $flist { catch { file delete $f } }

	file copy $spkLst $spkLst.[expr $i +1]

	if {$i == $end} {
	    set flist [glob -nocomplain "$spkLst.*"]
	    foreach f $flist { catch { file delete $f } }
	    set flist [glob -nocomplain "*.DONE"]
	    foreach f $flist { catch { file delete $f } }
	    touch train.DONE
	}

    } { } {
	if {$i < $end} {
	    codebookSet${SID} load  Weights/${i}.cbs.gz
	    distribSet${SID}  load  Weights/${i}.dss.gz
	
	    set fp [open "warps/$i.warp"]
	    while {[gets $fp line] > 0} {
		set warpScales([lindex $line 0]) [lindex $line 1]
	    }
	    close $fp
	}
    }
}

touch train.DONE

exit
