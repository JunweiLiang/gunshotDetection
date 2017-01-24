# ========================================================================
#  JANUS-SR   Janus Speech Recognition Toolkit
#             ------------------------------------------------------------
#             Object: Multivar training library functions
#             ------------------------------------------------------------
#
#  Author  :  Florian Metze
#  Module  :  multivar.tcl
#  Date    :  $Id: multivar.tcl 732 2001-01-11 12:33:17Z janus $
#
#  Remarks :  These functions are needed to do a ML-Training with
#             Univar/ Multivar
#
# ========================================================================
#
#  $Log$
#  Revision 1.2  2001/01/11 12:33:16  janus
#  Updated ReleaseLog
#
#  Revision 1.1.2.2  2001/01/10 10:43:41  janus
#  Final multivar, Cleanup
#
#  Revision 1.1.2.1  2000/12/10 14:06:12  janus
#  WAV support, ngets, multivar
#
#
# ========================================================================


# ----------------------------
# Some configuration variables
# ----------------------------
set MUERVBEG 0.7
set MUECVEND 0.7


proc MV_LoadSamples { data paths file {verbose 1} } {
    #
    # This function will load the samples in a feature set, so that
    # one can run a Multivar-Training on them
    #
    # Files are read from '$path/data.*/$file' forall paths from the list
    # 'data' should be of the form "featureSet:feature"
    #
    regsub ":.*" $data "" data1
    regsub ".*:" $data "" data2

    if {[lsearch [$data1:] $data2] == -1} {
	$data1 FMatrix $data2
    } else {
	$data1:$data2.data resize 0 0
    }

    set append -1
    set m ""
    foreach path $paths {
	if {[llength [set fl [glob -nocomplain $path/data.*/$file]]] == 0} {set fl [glob -nocomplain $path/$file]}
	set om [${data}.data configure -m]
	foreach f $fl {
	    set tm [${data}.data configure -m]
	    ${data}.data bload $f -append [incr append]
	    if {$verbose} {puts "$data1:$data2 loaded $f, [expr [${data}.data configure -m] - $tm]"}
	}
	lappend m [expr [${data}.data configure -m] - $om]
    }
    set n [${data}.data configure -n]
    ${data}.data resize [${data}.data configure -m] [expr int($n-1)]
    
    return $m
}


proc MV_SetRho { dss ds rho } {
    #
    # This will set rhoGlob and update the Determinant
    #
    set cbs            [$dss         configure -codebookSet]
    set cb  [$cbs name [$dss:$ds     configure -cbX]]
    $cbs:$cb.cfg configure -rhoGlob $rho
    $dss multiVar $ds -mode 0 -verbosity 0 -samples 0
}


proc MV_Count2Prob { dss ds } {
    #
    # Converts counts from ds.accu to probabilities
    #
    set tmpCount 0.0; set val ""
    set il [lindex [$dss:$ds.accu puts] 0]
    foreach i $il {set tmpCount [expr $i+$tmpCount]} 
    foreach i $il {lappend val  [expr $i/$tmpCount]}
    return $val
}


proc MV_Test { dss ds {first 0} {last -1} } {
    #
    # This procedure computes the likelihood of a codebook over a test-set
    #
    set cbs            [$dss           configure -codebookSet]
    set cb  [$cbs name [$dss:$ds       configure -cbX]]
    set fs             [$cbs           configure -featureSet]
    set feat [$fs name [$cbs:$cb       configure -featX]]
    set cbu            [$cbs:$cb.cfg   configure -update]
    set dsu            [$dss:$ds.cfg   configure -update]
    if {$last == -1} {set last [$fs:$feat.data configure -m]}
    $cbs:$cb.cfg configure -update n
    $dss:$ds.cfg configure -update n

    $dss multiVar $ds -verbosity 0 -first $first -last $last -samples -1

    $cbs:$cb.cfg configure -update $cbu
    $dss:$ds.cfg configure -update $dsu
    
    return [$cbs:$cb.accu configure -score]
}


proc MV_TestAll { path dss1 {dss2 ""} } {
    #
    # Computes the average scores for one or two codebooks
    #
    if {$dss2 == ""} {set dss2 $dss1}
    set cbs1            [$dss1     configure -codebookSet]
    set cbs2            [$dss2     configure -codebookSet]
    set fs1             [$cbs1     configure -featureSet]
    set fs2             [$cbs2     configure -featureSet]

    set total1 [set total2 [set tc1 [set tc2 0.0]]]
    foreach ds [$dss1:] {
	set cb1   [$cbs1 name [$dss1:$ds  configure -cbX]]
	set feat1 [$fs1  name [$cbs1:$cb1 configure -featX]]
	set cb2   [$cbs2 name [$dss2:$ds  configure -cbX]]
	set feat2 [$fs2  name [$cbs2:$cb2 configure -featX]]
	set donot [MV_LoadSamples $fs1:$feat1 $path $ds.smp 0]
	if {"$fs1:$feat1" != "$fs2:$feat2"} {set donot [expr $donot * [MV_LoadSamples $fs2:$feat2 $path $file 0]]}
	if {$donot} {
	    set score1 [MV_Test $dss1 $ds]
	    set count1 [$fs1:$feat1.data configure -m]
	    set total1 [expr $total1 + $score1 * $count1]
	    set tc1    [expr $tc1    + $count1]
	    if {$dss1 != $dss2} {
		set score2 [MV_Test $dss2 $ds]
		set count2 [$fs2:$feat2.data configure -m]
		set total2 [expr $total2 + $score2 * $count2]
		set tc2    [expr $tc2    + $count2]
		puts "$ds : $dss1 = $score1 , $count1 ; $dss2 = $score2 , $count2"
	    } else {
		puts "$ds : score = $score1 , $count1 = count"
	    }
	}
    }
    if {$dss1 != $dss2} {
	puts "$dss1 : score = [expr $total1/$tc1] ( $tc1 ), $dss2: score = [expr $total2/$tc2] ( $tc2 )"
    } else {
	puts "$dss1 : score = [expr $total1/$tc1] ( $tc1 )"
    } 
    return $total1
}


proc MV_Copy { dss ds1 ds2 } {
    #
    # Copies a distribution along with the underlying codebook,
    # generates the entries, if necessary
    #

    # Names, does ds2 already exist?
    set cbs            [$dss      configure -codebookSet]
    set cb1 [$cbs name [$dss:$ds1 configure -cbX]]
    if {[lsearch [$dss:] $ds2] == -1} {
	set cb2 $ds2
    } else {
	set cb2 [$cbs name [$dss:$ds2 configure -cbX]]
    }

    $cbs add $cb2 [[$cbs configure -featureSet] name [$cbs:$cb1 configure -featX]] [$cbs:$cb1 configure -refN] [$cbs:$cb1 configure -dimN] [$cbs:$cb1 configure -type]
    $dss add $ds2 $cb2

    $cbs:$cb2     := $cbs:$cb1
    $cbs:$cb2.mat := $cbs:$cb1.mat
    for {set i 0} {$i < [$cbs:$cb1 configure -refN]} {incr i} {
	$cbs:$cb2.cov($i) := $cbs:$cb1.cov($i)
    }

    if {[llength [$cbs:$cb1.accu configure]]} {
	$dss:$ds2 createAccu
	$cbs:$cb2 createAccu
	catch {$cbs:$cb2.accu := $cbs:$cb1.accu}
	catch {$dss:$ds2.accu := $dss:$ds1.accu}
    }

    regsub " *{ *" [$dss:$ds1 configure -val] "" tmp
    regsub " *} *" $tmp                       "" tmp
    $dss:$ds2 configure -val $tmp
}


proc kmeansOne { dss ds steps {temp 0.01} {m -1} } {
    #
    # This does KMeans
    #
    set cbs            [$dss           configure -codebookSet]
    set cb  [$cbs name [$dss:$ds       configure -cbX]]
    set fs             [$cbs           configure -featureSet]
    set feat [$fs name [$cbs:$cb       configure -featX]]
    if {$m == -1} {set m [$fs:$feat.data configure -m]}

    puts "KMeans with $m samples in $steps steps on codebook $cb"
    puts "  Settings: -tempF $temp"

    if {$m < [$fs:$feat.data configure -m]} {
	set t kmeansOneTemporary
	[FMatrix $t] := $fs:$feat.data
	$t resize $m [$fs:$feat.data configure -n]
	$cbs:$cb.mat neuralGas $t -maxIter $steps -tempF $temp
	catch {$t destroy}
    } else {
	$cbs:$cb.mat neuralGas $fs:$feat.data -maxIter $steps -tempF $temp
    }
    return $steps
}


proc univarOne { dss ds erg steps } {
    #
    # This function will do Univar-training
    #
    global MUERVBEG
    set cbs            [$dss           configure -codebookSet]
    set cb  [$cbs name [$dss:$ds       configure -cbX]]
    set fs             [$cbs           configure -featureSet]
    set feat [$fs name [$cbs:$cb       configure -featX]]
    set m              [$fs:$feat.data configure -m]
    set mpp      [expr [$cbs:$cb       configure -refN]*[$cbs:$cb configure -dimN]]
    if {$mpp > $m} {set mpp $m}

    puts "Univar using $mpp of $m samples in $steps steps on codebook $cb"
    puts "  Settings: cbUpdate = [$cbs:$cb.cfg configure -update], dsUpdate = [$dss:$ds.cfg configure -update]"
    puts "  Learning: $MUERVBEG = mueRv"

    $cbs:$cb.cfg configure -momentum   $MUERVBEG
    $cbs:$cb.cfg configure -momentumCv 1.00
    $dss multiVarInit $ds -seed 1.0 -rhoRel 1.05
    set maxRho [$cbs:$cb.cfg configure -rhoGlob]

    for {set i $steps} {$i > 0 && ![info exists ende]} {incr i -1} {
	$cbs:$cb.cfg configure -rhoGlob [set rho [expr $maxRho*$i/$steps]]
	$dss multiVar $ds -verbosity 0 -mode 0 -samples $mpp
	set score [$cbs:$cb.accu configure -score]

	puts "  UNI $i: $rho $score [$cbs:$cb.accu configure -minDistance] [$cbs:$cb.accu configure -maxDistance] [$cbs:$cb.accu configure -distortion]"

	if {![info exists min] || $score < $min} {
	    set min   $score
	    set minat $rho
	    MV_Copy $dss $ds $erg
	}
	if {$score > [expr 1.05*$min]} {set ende 1}
    }
    puts "Univar $ds: Minimum score = $min at rho = $minat"

    return $minat
}


proc multivarOne { dss ds steps {mueRvbeg ""} {mueCvend ""} } {
    #
    # This procedure will do Multivar-training
    #
    global MUERVBEG MUECVEND
    if {$mueRvbeg == ""} {set mueRvbeg $MUERVBEG}
    if {$mueCvend == ""} {set mueCvend $MUECVEND}
    set cbs            [$dss         configure -codebookSet]
    set cb  [$cbs name [$dss:$ds     configure -cbX]]
    set fs             [$cbs         configure -featureSet]
    set feat [$fs name [$cbs:$cb     configure -featX]]

    set mpp [set m [$fs:$feat.data configure -m]]
    if {$mpp > 500000} {set mpp 500000}

    puts "Multivar using $mpp of $m samples in $steps steps on codebook $cb"
    puts "  Settings: cbUpdate = [$cbs:$cb.cfg configure -update], dsUpdate = [$dss:$ds.cfg configure -update]"
    puts "  Learning: $mueRvbeg ... mueRv ... 1.0, 1.0 ... mueCv ... $mueCvend"

    for {set i $steps} {$i > 0 && ![info exists ende]} {incr i -1} {
	set mueRv [expr 1.0       - (1.0*$i/$steps)*(1.0-$mueRvbeg)]
	set mueCv [expr $mueCvend + (1.0*$i/$steps)*(1.0-$mueCvend)]
	$cbs:$cb.cfg configure -momentum   $mueRv
	$cbs:$cb.cfg configure -momentumCv $mueCv

	# Do it!
	$dss multiVar $ds -verbosity 0 -mode 1 -samples $mpp
	set score [$cbs:$cb.accu configure -score]

	puts  "  MULTI $i: $mueRv $mueCv $score [$cbs:$cb.accu configure -minDistance] [$cbs:$cb.accu configure -maxDistance] [$cbs:$cb.accu configure -distortion]"

	# Time to quit?
	if {![info exists min]} {set min [expr 1.1*$score]; set minat 0}
	if {$score < $min && [expr $steps-$i] > 10} {
	    if {$score > [expr (1.0 - 5E-5)*$min]} {set ende 1}
	    set min $score
	    set minat $i
	}

    }
    puts "Multivar $ds: Minimum score = $min at $minat"

    return $minat
}


proc EMOne { dss ds steps {m -1} {mode 5} } {
    #
    # This procedure will do EM-training
    #
    set cbs            [$dss           configure -codebookSet]
    set cb  [$cbs name [$dss:$ds       configure -cbX]]
    set fs             [$cbs           configure -featureSet]
    set feat [$fs name [$cbs:$cb       configure -featX]]
    if {$m == -1} {set  m [$fs:$feat.data configure -m]}
    $cbs:$cb.cfg configure -momentum    0.0
    $cbs:$cb.cfg configure -momentumCv -1.0

    puts "EM with $m samples in $steps steps on codebook $cb"
    puts "  Settings: cbUpdate = [$cbs:$cb.cfg configure -update], dsUpdate = [$dss:$ds.cfg configure -update]"
    if {$m < [$fs:$feat.data configure -m]} {puts "  XVal set is [expr [$fs:$feat.data configure -m] - $m] data points."}

    set mini   6
    set scoreX "n/a"
    catch {unset ende}
    catch {unset min}
    for {set i 1} {$i <= $steps} {incr i} {

	$dss multiVar $ds -verbosity 0 -last $m -mode $mode
	set ms [set score [$cbs:$cb.accu configure -score]]
	if {$m < [$fs:$feat.data configure -m]} {set ms [set scoreX [MV_Test $dss $ds $m]]}
	if {![info exists min]} {set min [expr 1.1*$ms]}
	set slimit [expr [string compare "n/a" $scoreX] ?  $min  :  [expr (1.0 - 5E-4)*$min] ]
 	puts  "  EM $i: - - : $score $scoreX : [$cbs:$cb.accu configure -minDistance] [$cbs:$cb.accu configure -maxDistance] , [$cbs:$cb.accu configure -distortion]"

	# If no XVal-set in use, exit if improvements get too slow;
	#   otherwise, exit if increase in LH (and copy best codebook back into place)
	if {$ms > $slimit && $i >= $mini} {
	    if {$scoreX != "n/a"} {MV_Copy $dss emtmp $ds}
	    break
	}
	if {$ms < $min} {set min $ms; set minat $i; MV_Copy $dss $ds emtmp}

    }
    puts "EM $ds: Minimum score = $min at i = $minat (exit criterion: $score , $scoreX ; $ms > $slimit ?)"

    if {[lsearch "2 3 4 5" $mode] != -1} {
	# Enforce covariances structure
	if {![info exists ende]} {MV_Copy $dss emtmp $ds}
	$dss multiVar $ds -verbosity 0 -last 0 -mode $mode
    }

    return $minat
}


proc doMultivar { LSID args } {
    #
    # This procedure works like "doKMeans"
    #
    set cbs           codebookSet$LSID
    set dss           distribSet$LSID
    set data          data
    set xval          ""
    set maxIter1      10
    set maxIter2      100
    set cbu           y
    set dsu           n
    set modus         ke
    set emode         5
    set param         Weights/mv

    itfParseArgv doMultivar $args [list [
        list <cbListFile>   string  {} file     {}   {file of codebook names}                ] [
        list -codebookSet   object  {} cbs      CodebookSet {codebook set}                   ] [
        list -distribSet    object  {} dss      DistribSet  {distribution set}               ] [
        list -paramFile     string  {} param    {}   {base name of parameters}               ] [
        list -dataPath      string  {} data     {}   {path of sample files}                  ] [
        list -xvalPath      string  {} xval     {}   {path of xval files}                    ] [
        list -cbUpdate      string  {} cbu      {}   {update codebook (y/n)}                 ] [
        list -dsUpdate      string  {} dsu      {}   {update distributions (y/n)}            ] [
	list -modus         string  {} modus    {}   {u, um, km, eu, ke}                     ] [
	list -emode         int     {} emode    {}   {1-5 for EM-Training}                   ] [
        list -maxIter1      int     {} maxIter1 {}   {number of iterations, Univar or KMeans}] [
	list -maxIter2      int     {} maxIter2 {}   {number of iterations, Multivar or EM}  ] ]

    set feat [featureSet$LSID name [$cbs:[$cbs name 0] configure -featX]]
    set uname "[exec uname -n].[pid]"

    $cbs createAccus
    $dss createAccus

    set cbsL [$cbs:]
    set dssL [$dss:]

    # ------------------------------------------------------------------------
    # At present: do only use for fully continuous systems
    # ------------------------------------------------------------------------
    doParallel {
	# --------------------------------------------------------------------
	#  Main Loop
	# --------------------------------------------------------------------

	set fp [open ${param}.tmp.$uname.info w]
	while {[fgets $file ds] >= 0} {
	    
	    puts "\n        -------- Multivar $dss:$ds (modus = $modus) --------" 
	    if {[catch {set cb [$cbs name [$dss:$ds configure -cbX]]} msg]} {
		puts "WARNING: '$msg'"
		puts $fp "$ds $cb FAILED: Codebook doesn't exist."
		continue
	    }

	    set m [MV_LoadSamples featureSet${LSID}:$feat "$data $xval" $cb.smp]
	    if {[llength $m] == 2} {set mx [lindex $m 1]; set m [lindex $m 0]} else {set mx 0}
	    if {$m == 0} {
		# We couldn't load the data!
		set m [MV_LoadSamples featureSet${LSID}:$feat "$data $xval" $ds.smp]
		if {[llength $m] == 2} {set mx [lindex $m 1]; set m [lindex $m 0]} else {set mx 0}
		if {$m == 0} {
		    # We couldn't load any data!
		    puts "WARNING: couldn't load data for '$cb.smp'/'$ds.smp'"
		    puts $fp "$ds $cb FAILED: Data not found."
		    continue
		}
	    }

	    # Kind of update?
	    set cbub [$cbs:$cb.cfg configure -update]
	    set dsub [$dss:$ds.cfg configure -update]
	    $cbs:$cb.cfg configure -update $cbu
	    $dss:$ds.cfg configure -update $dsu
	    
	    set zeit [clock seconds]
	    set t ""
	    set scoreX 0.0
	    if {[regexp u $modus]} {
		# Univar part --------------------------------------------
		set type [$cbs:$cb configure -type]
		for {set i 0} {$i < [$cbs:$cb configure -refN]} {incr i} {$cbs:$cb covarType $i RADIAL}
		MV_Copy    $dss $ds uni_$ds
		set unirho [univarOne $dss uni_$ds $ds $maxIter1]
		for {set i 0} {$i < [$cbs:$cb configure -refN]} {incr i} {$cbs:$cb covarType $i $type}
		MV_Copy   $dss     $ds uni_$ds
		MV_SetRho $dss     $ds $unirho
		MV_SetRho $dss uni_$ds $unirho
		set score [MV_Test $dss $ds 0 $m]
		if {$mx != 0} {set t " ; scoreX = [set scoreX [MV_Test $dss $ds $m]]"}
		puts "Univar-phase $ds: score = $score ; unirho = $unirho , time = [expr [clock seconds]-$zeit]$t"
	    } else {
		set unirho 0.0
	    }
	    
	    if {[regexp k $modus]} {
		# KMeans part --------------------------------------------
		set iter  [kmeansOne $dss $ds $maxIter1 0.01 $m]
		set score [MV_Test $dss $ds 0 $m]
		if {$mx != 0} {set t " ; scoreX = [set scoreX [MV_Test $dss $ds $m]]"}
		puts "KMeans-phase $ds: score = $score ; iter = $iter , time = [expr [clock seconds]-$zeit]$t"
	    } else {
		set iter 0
	    }
	    
	    if {[regexp m $modus]} {
		# Multivar part ------------------------------------------
		set iter [multivarOne $dss $ds $maxIter2]
		set score [MV_Test $dss $ds 0 $m]
		if {$mx != 0} {set scoreX [MV_Test $dss $ds $m]}
		puts "Multivar-phase $ds: score = $score ; iter = $iter , time = [expr [clock seconds]-$zeit]$t"
	    }
	    
	    if {[regexp e $modus]} {
		# EM part ------------------------------------------------
		set iter  [EMOne $dss $ds $maxIter2 $m $emode]
		set score [MV_Test $dss $ds 0 $m]
		if {$mx != 0} {set t " ; scoreX = [set scoreX [MV_Test $dss $ds $m]]"}
		puts "EM-phase $ds: score = $score ; iter = $iter , time = [expr [clock seconds]-$zeit]$t"
	    }
	    
	    if {$dsu == "n"} {
		# Initialization of distributions with counts (from MV_Test)
		MV_Test $dss $ds 0 $m
		$dss:$ds configure -val [MV_Count2Prob $dss $ds]
		set score  [MV_Test $dss $ds 0 $m]
		if {$mx != 0} {set t " ; scoreX = [set scoreX [MV_Test $dss $ds $m]]"}
		puts "Distribution $ds: score = ${score}$t ; Codebook $cb"
	    }

	    set t "score = $score , scoreX = $scoreX ; rho = $unirho , iter = $iter ; time = [expr [clock seconds]-$zeit] ; data = $m , xval = $mx ; mode = $modus,$cbu$dsu"
	    puts     "Summary $ds: $t"
	    puts $fp "$ds $cb OK: $t"

	    # Reset update settings
	    $cbs:$cb.cfg configure -update $cbub
	    $dss:$ds.cfg configure -update $dsub
	    
	}
	close $fp

	$cbs save ${param}.tmp.$uname.cbs.gz
	$dss save ${param}.tmp.$uname.dss.gz

    } {
	# --------------------------------------------------------------------
	#  Server: Combining codebooks/ distributions
	# --------------------------------------------------------------------

	puts "doMultivar: combining results ..."

	CodebookSet cbsTmp featureSet$LSID
	DistribSet  dssTmp cbsTmp
	foreach cb $cbsL {cbsTmp add $cb $feat [$cbs:$cb configure -refN] [$cbs:$cb configure -dimN] [$cbs:$cb configure -type]}
	foreach ds $dssL {dssTmp add $ds [$cbs name [$dss:$ds configure -cbX]]}

	set dssA ""
	foreach infoF [glob "${param}.tmp.*.info"] {
	    puts "doMultivar: loading $infoF ..."
	    regsub "info$" $infoF "cbs.gz" cbsF
	    regsub "info$" $infoF "dss.gz" dssF
	    $dss load $dssF
	    $cbs load $cbsF

	    set fp [open $infoF r]
	    while {[gets $fp line] != -1} {
		set ds [lindex $line 0]
		set cb [lindex $line 1]
		if {[lsearch $dssL $ds] == -1 || [lindex $line 2] != "OK:"} {
		    puts "  IGNORING line '$line'"
		} else {
		    puts "  line: '$line'"
		    cbsTmp:$cb := $cbs:$cb
		    dssTmp:$ds := $dss:$ds
		    lappend dssA $ds
		}
	    }
	    close $fp
	}

	puts "doMultivar: loaded [llength $dssA] distributions for [llength $dssL] in set"

	cbsTmp save ${param}.cbs.gz
	dssTmp save ${param}.dss.gz

    } {
	# --------------------------------------------------------------------
	#  Server finish
	# --------------------------------------------------------------------
	#foreach f [glob -nocomplain "${param}.tmp.*"] {rm $f}
    } {
	# --------------------------------------------------------------------
	#  Client update
	# --------------------------------------------------------------------
    }
}
