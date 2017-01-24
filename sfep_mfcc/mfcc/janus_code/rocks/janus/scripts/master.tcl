# ========================================================================
#  JanusRTk   Janus Speech Recognition Toolkit
#             ------------------------------------------------------------
#             Object: Master Script
#             ------------------------------------------------------------
#
#  Author  :  Florian Metze
#  Module  :  master.tcl
#  Date    :  $Id: master.tcl 2894 2009-07-27 15:55:07Z metze $
#
#  Remarks :  
#
# ------------------------------------------------------------------------
#
# $Log$
# Revision 1.3  2003/12/17 13:07:27  metze
# P013
#
# Revision 1.2  2003/08/14 11:19:45  fuegen
# Merged changes on branch jtk-01-01-15-fms (jaguar -> ibis-013)
#
# Revision 1.1.2.10  2003/08/11 14:53:25  soltau
# startJanus : added globals
#
# Revision 1.1.2.9  2003/08/11 14:31:32  soltau
# fixed call for trainPT
#
# Revision 1.1.2.8  2003/08/11 13:09:25  soltau
# startJanus : added file copy option for log-file
#
# Revision 1.1.2.7  2003/08/11 12:24:43  soltau
# fixed startJanus
#
# Revision 1.1.2.6  2003/04/01 12:32:45  metze
# Creation of directories
#
# Revision 1.1.2.5  2002/11/21 16:12:33  metze
# Ibis changes
#
# Revision 1.1.2.4  2002/11/06 10:36:41  metze
# Removed clusterLH (from superficial view)
#
# Revision 1.1.2.3  2002/07/30 07:42:09  metze
# Beautification and looks for janusNoX
#
# Revision 1.1.2.2  2002/07/12 16:29:55  metze
# Approved version
#
# Revision 1.1.2.1  2002/07/12 10:03:01  metze
# Initial script collection
#
#
# ========================================================================


set doL              ""
set testSpk          [file join desc test-spk]
set trainSpk         [file join desc train-spk]
set descFile         [file join desc desc.tcl]
set scriptFile       [info script]
set semFile          ""

# Find a good Janus executable
if { [ catch { set janus [which ibis] } ] } {
    if { [ catch { set janus [which janusNoX] } ] } {
	if { [ catch { set janus [which janusX] } ] } { set janus janus }
    }
}

set stepsL           "makeCI means lda samples kmeans train makePT trainPT cluster split bbi test score labels"
set steps(makeCI)    "- 0- Generate the context independent system's description files"
set steps(means)     "- 1- Compute the means"
set steps(lda)       "- 2- Compute the system's LDA matrix"
set steps(samples)   "- 3- Extract codebook samples"
set steps(kmeans)    "- 4- K-Means on samples"
set steps(train)     "-5a- Train system with EM (CI, CD or PT)"
set steps(warp)      "-5b- Train system with EM (CI or CD) and VTLN"
set steps(makePT)    "- 6- Generate polyphone trees for the distribution tree(s)"
set steps(trainPT)   "- 7- Train polyphonic distributions"
# set steps(samplesLH) "-8a- Extract LH-samples"
# set steps(clusterLH) "-8b- Generate question list(s) via LH"
set steps(cluster)   "- 8- Generate question list(s)"
set steps(split)     "- 9- Split the polyphonic tree(s)"
set steps(bbi)       "-10- Compute BBI tree(s)"
set steps(test)      "-11- Test the system"
set steps(score)     "-12- Score the system"
set steps(labels)    "-13- Write ML-adapted labels"


proc startJanus { wDir script sFile sLst opts } {
    global scriptPath descFile spkLst janus 
    global tcl_platform LOGFILE
    set oldpwd [pwd]
    regsub -all .tcl [file join Log $script] .log lFile

    if {![file isdirectory $wDir]} { file mkdir $wDir }
    if {$sFile != ""} {
	file copy -force $sFile [file join $wDir $sLst]
    } else {
	set sLst "''"
    }
    if {![file isdirectory [file join $wDir Log]]} { file mkdir [file join $wDir Log] }
    printDo cd $wDir

    # windows don't like log-file sometimes
    if {[regexp {indows} $tcl_platform(os)] && [info exists LOGFILE] } {
        eval printDo exec $janus [file join $scriptPath $script] -spkLst $sLst -desc $descFile $opts
	if [file exists $LOGFILE] {
	    if {1 == [catch {file rename $LOGFILE $lFile} msg]} {
		puts "cannot rename $LOGFILE : $msg"
		if {1 == [catch {file copy $LOGFILE $lFile} msg]} {
		    puts "cannot copy $LOGFILE : $msg"
		}
	    }
	}
    } else {
        eval printDo exec $janus [file join $scriptPath $script] -spkLst $sLst -desc $descFile $opts >& $lFile
    }
    if { 1== [catch {exec gzip -f $lFile} msg]} {
        puts "cannot gzip $lFile : $msg"
    }

    cd   $oldpwd
}


# ---------------------------------
#   Handle command line arguments
# ---------------------------------
if {[llength $argv] > 0 } {
    if {[lindex $argv 0] == "-h" || [lindex $argv 0] == "-help" || [lindex $argv 0] == "--help"} {
	puts stderr "\nUsage: $argv0 \[-h|-help|--help\]       \n \
                     \[-janus    <janus executable>\]        \n \
                     \[-testSpk  <test speaker file>\]       \n \
                     \[-trainSpk <train speaker file>\]      \n \
                     \[-desc     <system description file>\] \n \
                     \[-semFile  <semaphore file>\]          \n \
                     \[-do       <step> <step> ...\]\n"
	puts stderr "The following steps can be performed:"
	foreach i $stepsL {
	    puts stderr "  $i:  \t$steps($i)"
	}
	puts stderr ""
	exit
    }

    # Parse command line
    for {set i 0} {$i < [llength $argv]} {incr i} {
	set argument [lindex $argv $i]
	switch -- $argument {
	    "-janus" {
		incr i
		set janus    [lindex $argv $i]
	    }
	    "-testSpk" {
		incr i
		set testSpk  [lindex $argv $i]
	    }
	    "-trainSpk" {
		incr i
		set trainSpk [lindex $argv $i]
	    }
	    "-desc" {
		incr i
		set descFile [lindex $argv $i]
	    }
            "-semFile" {
                incr i
                set semFile  [lindex $argv $i]
            }
	    "-do" {
		set doL ""
		incr i
		while {$i < [llength $argv] && [info exists steps([lindex $argv $i])] } {
		    lappend doL [lindex $argv $i]
		    incr i
		}
		if {$i < [llength $argv]} {incr i -1}
	    }
	    default {
		puts stderr "Unrecognized argument '$argument'!"
	    }
	}
    }
}

if {$doL == ""} { set doL $stepsL }


# ------------------------------------------------------------------
# we need sysFile and logChannel as absolute pathnames. Otherwise,
# after doing a 'cd workdir', they would either be no longer there,
# the wrong file would be there, or they would be created in the
# wrong location. Care for that.
# ------------------------------------------------------------------
if {[file pathtype $descFile]   == "relative"} {
    set descFile   [file join [pwd] $descFile]
}
if {[file pathtype $scriptFile] == "relative"} {
    set scriptFile [file join [pwd] $scriptFile]
}
set scriptPath [file dirname $scriptFile]
set runPath    [pwd]

puts stderr "\$Id: master.tcl 2894 2009-07-27 15:55:07Z metze $"
puts stderr "Started on $env(HOST).[pid], [exec date]"
puts stderr "Description file: $descFile"
puts stderr "Agenda:"
foreach i $doL { puts stderr "  $i:\t $steps($i)" }
puts stderr [string repeat "-" 72] 
puts stderr "  Performing sanity checks ...\n"

if {[file readable $descFile]} {
    source $descFile 
} else {
    puts stderr "ERROR: $descFile not found or not readable."
    exit
}
puts stderr ""

if {0 && ![file exists [expr \$${SID}(codebookSetDesc)]] ||
    ![file exists [expr \$${SID}(distribSetDesc)]]} {
    if {[lsearch $doL "makeCI"] == -1} {
	puts stderr "Adding 'makeCI' to agenda."
	set doL "makeCI $doL"
    }
}

if {![file exists $testSpk]} {
    puts stderr "WARN: $testSpk not found (test-speakers)"
}
if {![file exists $trainSpk]} {
    puts stderr "WARN: $trainSpk not found (training-speakers)"
}
waitFile $semFile


# ------------------------------------------------------------------------
#   makeCI
# ----------
#   make CI description files
# ------------------------------------------------------------------------
if {[lsearch $doL makeCI] >= 0} {
    puts stderr [string repeat "-" 72]
    puts stderr "  makeCI (no parallelization):\t $steps(makeCI) ... "

    startJanus . makeCI.tcl "" "" ""
}


# ------------------------------------------------------------------------
#   means
# ---------
#   compute cepstral means
# ------------------------------------------------------------------------
if {[lsearch $doL means] >= 0} {
    puts stderr [string repeat "-" 72]
    puts stderr "  means:\t $steps(means) ..."

    startJanus train means.tcl $trainSpk train-spk.means ""
}


# ------------------------------------------------------------------------
#   lda
# -------
#   compute lda
# ------------------------------------------------------------------------
if {[lsearch $doL lda] >= 0} {
    puts stderr [string repeat "-" 72]
    puts stderr "  lda:\t $steps(lda) ..."

    startJanus train lda.tcl $trainSpk train-spk.lda ""
}


# ------------------------------------------------------------------------
#   samples
# -----------
#   extract samples
# ------------------------------------------------------------------------
if {[lsearch $doL samples] >= 0} {
    puts stderr [string repeat "-" 72]
    puts stderr "  samples:\t $steps(samples) ..."

    startJanus train samples.tcl $trainSpk train-spk.samples ""
}


# ------------------------------------------------------------------------
#   kmeans
# ----------
#   create initial codebooks from samples
# ------------------------------------------------------------------------
if {[lsearch $doL kmeans] >= 0} {
    puts stderr [string repeat "-" 72]
    puts stderr "  kmeans:\t $steps(kmeans) ..."

    startJanus train kmeans.tcl "" "" ""
}


# ------------------------------------------------------------------------
#   train
# ---------
#   train codebooks with EM
# ------------------------------------------------------------------------
if {[lsearch $doL train] >= 0} {
    puts stderr [string repeat "-" 72]
    puts stderr "  train:\t $steps(train) ..."

    startJanus train train.tcl $trainSpk train-spk ""
}


# ------------------------------------------------------------------------
#   warp
# --------
#   train codebooks with EM and estimate warp factors
# ------------------------------------------------------------------------
if {[lsearch $doL warp] >= 0} {
    puts stderr [string repeat "-" 72]
    puts stderr "  warp:\t $steps(warp) ..."

    startJanus train warp.tcl $trainSpk train-spk ""
}


# ------------------------------------------------------------------------
#   makePT
# ----------
#   init polyphone training
# ------------------------------------------------------------------------
if {[lsearch $doL makePT] >= 0} {
    puts stderr [string repeat "-" 72]
    puts stderr "  makePT (no parallelization):\t $steps(makePT) ..."

    startJanus pt makePT.tcl $trainSpk train-spk ""
}


# ------------------------------------------------------------------------
#   trainPT
# -----------
#   train polyphonic distributions
# ------------------------------------------------------------------------
if {[lsearch $doL trainPT] >= 0} {
    puts stderr [string repeat "-" 72]
    puts stderr "  trainPT:\t $steps(trainPT) ..."

    # Set variables for PT training
    set d  [set ${SID}(descPath)]
    if {[catch {set cw [pwd]/[lindex [lsort -decreasing [glob train/Weights/*.cbs.gz]] 0]}]} {
	set cw [set ${SID}(codebookSetParam)]
    }

    startJanus pt train.tcl $trainSpk train-spk "-cbsDesc $d/codebookSet.PT.gz \
        -cbsParam $cw -dssDesc $d/distribSet.PT.gz -dssParam {\"\"}            \
        -dstDesc $d/distribTree.PT.gz -ptreeDesc $d/ptreeSet.PT.gz             \
        -semFile ptree.DONE"
}


# ------------------------------------------------------------------------
#   samplesLH
# -------------
#   extract likelihood samples
# ------------------------------------------------------------------------
if {[lsearch $doL samplesLH] >= 0} {
    puts stderr [string repeat "-" 72]
    puts stderr "  samplesLH:\t $steps(samplesLH) EXPERIMENTAL! ..."

    startJanus pt samplesLH.tcl $trainSpk train-spk ""
}


# ------------------------------------------------------------------------
#   clusterLH
# -------------
#   cluster via likelihood
# ------------------------------------------------------------------------
if {0 && [lsearch $doL clusterLH] >= 0} {
    puts stderr [string repeat "-" 72]
    puts stderr "  clusterLH:\t $steps(clusterLH) EXPERIMENTAL! ..."

    startJanus pt clusterLH.tcl $trainSpk train-spk ""
}


# ------------------------------------------------------------------------
#   cluster
# -----------
#   generate questions using entropy
# ------------------------------------------------------------------------
if {[lsearch $doL cluster] >= 0} {
    puts stderr [string repeat "-" 72]
    puts stderr "  cluster:\t $steps(cluster) ..."

    startJanus pt cluster.tcl $trainSpk train-spk ""
}


# ------------------------------------------------------------------------
#   split
# ---------
#   split polyphones (init context-dependent system)
# ------------------------------------------------------------------------
if {[lsearch $doL split] >= 0} {
    puts stderr [string repeat "-" 72]
    puts stderr "  split (no parallelization):\t $steps(split) ..."

    startJanus pt split.tcl $trainSpk train-spk ""
}


# ------------------------------------------------------------------------
#   bbi
# -------
#   compute bbi
# ------------------------------------------------------------------------
if {[lsearch $doL bbi] >= 0} {
    puts stderr [string repeat "-" 72]
    puts stderr "  bbi (no parallelization):\t $steps(bbi) ..."

    startJanus train createBBI.tcl "" "" ""
}


# ------------------------------------------------------------------------
#   labels
# ----------
#   write labels
# ------------------------------------------------------------------------
if {[lsearch $doL labels] >= 0} {
    puts stderr [string repeat "-" 72]
    puts stderr "  labels:\t $steps(labels) ..."

    startJanus labels labels.tcl $trainSpk train-spk ""
}


# ------------------------------------------------------------------------
#   test
# --------
#   test system
# ------------------------------------------------------------------------
if {[lsearch $doL test] >= 0} {
    puts stderr [string repeat "-" 72]
    puts stderr "  test:\t $steps(test) ..."

     startJanus test test.tcl $testSpk test-spk ""
}


# ------------------------------------------------------------------------
#   score
# ---------
#   score the test system
# ------------------------------------------------------------------------
if {[lsearch $doL score] >= 0} {
    puts stderr [string repeat "-" 72]
    puts stderr "  score (no parallelization):\t $steps(score) ..."

     startJanus test score.tcl "" "" ""
}


puts stderr [string repeat "-" 72]
puts stderr "Ok."

exit
