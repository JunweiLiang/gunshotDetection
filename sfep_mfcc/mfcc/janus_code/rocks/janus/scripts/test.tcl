# ========================================================================
#  JanusRTK   Janus Speech Recognition Toolkit
#             ------------------------------------------------------------
#             Object: Test script
#             ------------------------------------------------------------
#
#  Author  :  The Ibis Gang
#  Module  :  test.tcl
#  Date    :  $Id: test.tcl 3090 2010-01-20 04:17:42Z metze $
#
#  Remarks :  Includes VTLN and FSA
#             Lattice rescoring
#
# ========================================================================
# 
#   $Log$
#   Revision 1.7  2005/03/11 18:04:36  metze
#   Changed lz to 17
#
#   Revision 1.6  2004/11/10 08:37:05  metze
#   Added -time flag
#
#   Revision 1.5  2004/10/22 13:08:46  metze
#   Simplified a few things
#
#   Revision 1.4  2003/12/17 13:07:15  metze
#   scripts/master.tcl
#
#   Revision 1.3  2003/08/22 09:08:43  metze
#   Stupid '-filPen -32068lp' bug fixed
#
#   Revision 1.2  2003/08/14 11:19:47  fuegen
#   Merged changes on branch jtk-01-01-15-fms (jaguar -> ibis-013)
#
#   Revision 1.1.2.8  2003/08/11 12:34:56  soltau
#   changed bbi args
#
#   Revision 1.1.2.7  2003/04/10 13:17:20  metze
#   Added -semFile option
#
#   Revision 1.1.2.6  2003/04/04 11:38:34  metze
#   scoreWarpLst changed
#
#   Revision 1.1.2.5  2003/03/14 16:30:40  metze
#   Removed -silPen
#
#   Revision 1.1.2.4  2002/11/21 16:12:33  metze
#   Ibis changes
#
#   Revision 1.1.2.3  2002/07/30 07:44:29  metze
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
#  Settings for Test
# ------------------------------------------------------------------------

set ModulStatus(Timing) 0
set spkLst              test-spk
set descFile            ../desc/desc.tcl
set semFile             ""

set hypoDir             hypos
#set latticeDir          ""
set latticeDir          lattice
set beam                300
set masterBeam          1.0
set fsadapt             0
set dovtln              1

set minFrames           1000 ;# to deactivate warp re-estimation
set phi                 0.5  ;# history weight factor

set codebookSetParam    ../train/Weights/4.cbs.gz
set distribSetParam     ../train/Weights/4.dss.gz

set netDir      nets

# lz/lp 
set lz  17
set lp   0

set lzR {20 23 26 29 32 35}
set lpR {-3 0 3 6}

# confusion network
set netDens   10
set postScale 2.0
set clpBeam   5.0
set silScale  1.0

# -----------------------------------------------------------------------
#  Read in command-line options (override settings)
# -----------------------------------------------------------------------

if [catch {itfParseArgv $argv0 $argv [list [
    list -lattices     string  {} latticeDir   {} {Lattice directory} ] [
    list -hypos        string  {} hypoDir      {} {Hypos directory} ] [
    list -desc         string  {} descFile     {} {description file} ] [
    list -lz           int     {} lz           {} {Base Lz} ] [
    list -lp           int     {} lp           {} {Base Lp} ] [
    list -beam         float   {} masterBeam   {} {Master beam} ] [
    list -spkLst       string  {} spkLst       {} {Speaker list} ] [
    list -cbsDesc      string  {} codebookSetDesc  {} {codebook descriptions} ] [
    list -cbsParam     string  {} codebookSetParam {} {codebook parameters} ] [
    list -dssDesc      string  {} distribSetDesc   {} {distribution descriptions} ] [
    list -dssParam     string  {} distribSetParam  {} {distribution parameters} ] [
    list -dstDesc      string  {} distribTreeDesc  {} {distribution tree description} ] [
    list -ptreeDesc    string  {} ptreeSetDesc     {} {ptree description} ] [
    list -bbiDesc      string  {} bbiSetDesc       {} {BBI description file} ] [
    list -bbiParam     string  {} bbiSetParam      {} {BBI parameter file} ] [
    list -fsadapt      int     {} fsadapt          {} {Feature space adaptation} ] [
    list -dovtln       int     {} dovtln           {} {VTLN warping} ] [
    list -time         int     {} ModulStatus(Timing) {} {time execution} ] [
    list -semFile      string  {} semFile          {} {semaphore file}   ] ]
} ] {
    puts stderr "ERROR in $argv0 while scanning options"
    exit -1
}

source $descFile

foreach i "optWord variants dbaseName testFeatureSetDesc bbiSetDesc bbiSetParam \
           codebookSetDesc codebookSetParam distribSetDesc distribSetParam \
           distribTreeDesc ptreeSetDesc" {
    if {![info exists $i] || [set $i] == ""} { eval "set $i \$${SID}($i)" }
}

waitFile $semFile


set ${SID}(lmWeight)  $lz
set ${SID}(lmPenalty) $lp


# ------------------------------------------------------------------------
#  Init Modules
# ------------------------------------------------------------------------

phonesSetInit   $SID
tagsInit        $SID
featureSetInit  $SID -desc  $testFeatureSetDesc
codebookSetInit $SID -param $codebookSetParam
distribSetInit  $SID -param $distribSetParam
bbiSetInit      $SID -desc  $bbiSetDesc -param $bbiSetParam
distribTreeInit $SID -ptree ""
senoneSetInit   $SID  distribStream$SID
topoSetInit     $SID
ttreeInit       $SID 
dictInit        $SID
trainInit       $SID
dbaseInit       $SID  $dbaseName


# ------------------------------------------------------------------------
#  Init Ibis Decoder
#    The STree-dump contains the SenoneSet, too!!!
# ------------------------------------------------------------------------

ibisInit        $SID -masterBeam $masterBeam


# ------------------------------------------------------------------------
#  Create SignalAdapt
# ------------------------------------------------------------------------

if {$fsadapt} {
    set sas signalAdapt$SID
    set wX  0                 ;# transformation index
    set aX  1                 ;# accu index
    printDo SignalAdapt $sas senoneSet$SID
    $sas configure -topN 1 -shift 1.0
    foreach ds [distribSet$SID] {$sas add $ds}
    $sas clear $wX

    # Load pre-computed accus:
    #foreach saa [glob "SAS/*"] {
    #    signalAdapt${SID} clearAccu $aX
    #    signalAdapt${SID} readAccu $saa $aX
    #    puts "SignalAdapt${SID} readAccu $aX $saa : [$sas configure -beta($aX)]"
    #    incr aX
    #}
    $sas clearAccu $aX
}


# ------------------------------------------------------------------------
#  Reset dbase filter, get warpPhones, etc ...
# ------------------------------------------------------------------------

proc dbaseUttFilter {dbase args} {return 1}

file mkdir $hypoDir
if {$latticeDir != ""} { file mkdir $latticeDir }

if {[catch { set phoneLst [phonesSet${SID}:[set ${SID}(warpPhones)]] } msg]} {
    puts stderr "WARNING: warping using SID(warpPhones) returned '$msg', trying 'VOICED'"
    catch { set phoneLst [phonesSet${SID}:VOICED] }
}


# ------------------------------------------------------------------------
#  Lattice generation?
# ------------------------------------------------------------------------

if {$latticeDir != ""} {
    spass$SID.glat configure -alphaBeam 200 -topN 150
    puts "glat: [spass$SID.glat configure]"
    file mkdir $latticeDir
}

puts "svmap: [svmap$SID configure]"
puts "spass: [spass$SID configure]"


# ---------------------------------
#  Here we go...
# ---------------------------------

while {[fgets $spkLst spk] != -1} {

    # ------------------------------------
    #  Clear SignalAdaptation accus
    # ------------------------------------

    if {[info exists sas]} {
        $sas  clear      $wX
        $sas  clearAccu  0
    }


    # ------------------------------------
    #  Reset score array for ML VTLN
    # ------------------------------------

    set cwarp       1.000
    set totalFrames 0
    set STORE_CMS   STORE
    set scaleLst    $cwarp
    
    if {$dovtln} {
	set STORE_CMS   UPDATE
	set scaleLst    [makeScaleList $cwarp 10 0.02]
    }

    # --------------------------------------------------
    #  Destroy/create fvector to accumulate
    #  speaker mean for each Warpfactor
    # --------------------------------------------------

    foreach scale $scaleLst {
	set scaleN "[string index $scale 0][string range $scale 2 end]"
	if [llength [info command Mean$scaleN]]  {
	    Mean$scaleN  destroy
	    SMean$scaleN destroy
	}
	[FVector Mean$scaleN   13] configure -count 0
	[FVector SMean$scaleN  13] configure -count 0
    }  

    foreach scale $scaleLst { set frameN($scale) 0; set scoreN($scale) 0.0 }


    # --------------------------------------------------
    #  Loop over all utterances 
    # --------------------------------------------------

    foreachSegment utt uttDB $spk {	
	set uttInfo [uttDB uttInfo $spk $utt]
	TIME START total $utt


	# --------------------------------------------
	#  Apply history Weight factor to CMS & VTLN
	# --------------------------------------------

	foreach scale $scaleLst {
            set scaleN "[string index $scale 0][string range $scale 2 end]"
            set cnt [expr int ($phi * [Mean$scaleN configure -count]) ]
            Mean$scaleN   configure -count $cnt
	    SMean$scaleN  configure -count $cnt
	    set frameN($scale) [expr $phi * [set frameN($scale)]]
	    set scoreN($scale) [expr $phi * [set scoreN($scale)]]
	}

	set WARPSCALE $cwarp
	featureSet$SID eval $uttInfo


	# ------------------------------------
	#  Run decoder and trace back hypo
	# ------------------------------------

        TIME START run $utt
        spass$SID run
        TIME STOP run $utt
 
        if {![catch {set hypo  [spass$SID.stab trace]} msg]} {
            set score [lindex $hypo 1]
            set bpN   [lindex $hypo 0]
            set text  [lrange $hypo 3 [expr [llength $hypo] -2]]
            puts "$utt: $hypo"
        } else {
            set score 999999
            set bpN   0
            set text  ""
            puts "$utt: $msg"
        }


	# ------------------------------------
	#  CMS update
	# ------------------------------------
        set totalFrames  [expr $totalFrames + [featureSet$SID:LDA configure -frameN]]
	set scaleN      "[string index $cwarp 0][string range $cwarp 2 end]"
	puts "$utt: use [Mean$scaleN configure -count]/$totalFrames frames for CMS"
	

	# ------------------------------------
        #  Adapt feature space transform
        # ------------------------------------

        if {[info exists sas]} {

            # Do viterbi alignment
            if {[catch {hmm$SID make "( $text )"} msg]} {
                writeLog stderr $msg; continue
            }
            if {[catch {path$SID viterbi hmm$SID -beam $beam} msg]} {
                writeLog stderr $msg; continue
            }

	    # Re-create the un-adapted feature
	    set   sasT $sas
	    unset sas
	    featureSet$SID eval $uttInfo
	    set   sas  $sasT
            # featureSet$SID matmul LDA FEAT+ featureSet$SID:LDAMatrix.data -cut 32

	    $sas accu path$SID 0
            $sas clear $wX
            if {$totalFrames > 1500} { $sas compute 10 0 $wX }
        }

	
	# ------------------------------------
	#  WARP from last hypo
	# ------------------------------------

        if {[llength $scaleLst] > 1} {
            TIME START warp $utt
 
            # Do viterbi alignment
            if {[catch {hmm$SID make "( $text )"} msg]} {
                writeLog stderr $msg; continue
            }
            if {[catch {path$SID viterbi hmm$SID -beam $beam} msg]} {
                writeLog stderr $msg; continue
            }
 
            set STORE_CMS STORE  ;# store cms
            scoreWarpLst frameN scoreN $SID path$SID hmm$SID $uttInfo $scaleLst $phoneLst

            set WARPSCALE [findBestWarp scoreN $scaleLst]
            set cwarp $WARPSCALE

	    if {$totalFrames > $minFrames} {
		# We can reduce the warp window now
		set max 0.0; set maxS ""

		foreach scale $scaleLst {
		    set scoreN($scale) [expr $phi * [set scoreN($scale)]]
		    if {$scoreN($scale) > $max} { set max $scoreN([set maxS $scale]) }
		}
		set maxS     [lsearch  $scaleLst $maxS]
		set scaleLst [lreplace $scaleLst $maxS $maxS]
	    }
	    
	    if {[llength $scaleLst] > 1} {
		set STORE_CMS UPDATE ;# accumulate temporarily
	    }

            TIME STOP warp $utt 
        }


	# ---------------------
	#  Write hypo
	# ---------------------

         set fp [open "$hypoDir/H_[pid]_z${lz}_p${lp}_LV.ctm" a]
         puts $fp "%TURN: $utt"
         puts $fp "$text (hypo 0 | score = $score)\n"
         close $fp


	# ----------------------
	#  Lattice rescoring
	# ----------------------

	if {$latticeDir != ""} {
	    spass$SID.glat connect
	    set nodeN [expr 15 * [llength [spass$SID.stab trace]]]
	    puts "GLat $spk $utt pruning to $nodeN: [spass$SID.glat configure]"
	    spass$SID.glat prune -nodeN $nodeN
	    spass$SID.glat write $latticeDir/$utt.lat.gz

	    foreach tlz $lzR {
		foreach tlp $lpR {
		    svmap$SID configure -lz $tlz -filPen $tlp -wordPen $tlp

		    set hypo  [lindex [spass$SID.glat rescore] 0]
                    set score [lindex $hypo 1]
                    set bpN   [lindex $hypo 0]
                    set text  [lrange $hypo 3 [expr [llength $hypo] -2]]

		    set fp [open "$hypoDir/H_[pid]_z${tlz}_p${tlp}_R.ctm" a]
		    puts $fp "%TURN: $utt"
		    puts $fp "$text (hypo 0 | score = $score)\n"
		    close $fp
		}
	    }

	    svmap$SID configure -lz $lz -filPen $lp -wordPen $lp
	}

         # consensus
#    	if {$netDir != ""} {
#    	    svmap$SID configure -lz $lz2 -wordPen $lp2
#    	    spass$SID.glat prune -nodeN [expr $netDens * [llength $hypo]]
    
#    	    if {![spass$SID.glat configure -linkN]} {
#    	    	printDo spass$SID.glat read $latOut/$utt.lat.bz2
#    	    }
    
#    	    spass$SID.glat splitMW
    
#    	    spass$SID.glat posteriori -scale [expr $postScale/$lz2]
#    	    set  cons [spass$SID.glat consensus -v 1 -beam $clpBeam -silScale $silScale -cutoff 0.1 -silWord "<breath>"]
    
#    	    set fp [open $netDir/H_${host}.${pid}.net a]
#    	    puts $fp "$utt $uttArray(FROM) $uttArray(TO) $cons"
#    	    close $fp
    
#    	    spass$SID.glat writeCTM  $spk $utt -result [lindex $cons 2] \
#    			-file $hypoDir/H_${host}.${pid}_CONS.ctm
#        }
        
	# Book-keeping
	TIME STOP total $utt

    }
}

if {$ModulStatus(Timing)} {
    saveTIME "time.[pid]"
}

touch test.DONE

exit
