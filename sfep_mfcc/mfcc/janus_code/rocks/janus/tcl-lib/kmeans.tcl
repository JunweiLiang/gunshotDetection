# ========================================================================
#  JANUS-SR   Janus Speech Recognition Toolkit
#             ------------------------------------------------------------
#             Object: Codevector Clustering (KMeans/Neural Gas)
#             ------------------------------------------------------------
#
#  Author  :  Michael Finke
#  Module  :  kmeans.tcl
#  Date    :  Thu Nov 14 18:05:11 EST 1996
#
#  Remarks :
#
# ========================================================================
#
#  $Log$
#  Revision 1.4  2003/08/14 11:20:28  fuegen
#  Merged changes on branch jtk-01-01-15-fms (jaguar -> ibis-013)
#
#  Revision 1.3.50.10  2003/06/06 09:43:59  metze
#  Added latview, fixed upvar, updated tclIndex
#
#  Revision 1.3.50.9  2002/11/20 15:52:18  metze
#  Removed extra output
#
#  Revision 1.3.50.8  2002/11/08 10:24:26  metze
#  Code cleaning
#
#  Revision 1.3.50.7  2002/11/04 15:03:56  metze
#  Added documentation code
#
#  Revision 1.3.50.6  2001/12/21 10:36:41  metze
#  Added -maxCount option to cope with large data sets
#
#  Revision 1.3.50.5  2001/11/26 12:49:00  metze
#  Bugfix
#
#  Revision 1.3.50.4  2001/11/22 12:57:18  metze
#  Made training with features easier (AF/d.arber)
#
#  Revision 1.3.50.3  2001/10/03 19:05:23  metze
#  Added 'alloc' also for reading of matrices (before bload)
#  Parallel kmeans needs this
#
#  Revision 1.3.50.2  2001/09/25 07:39:12  metze
#  Bugfix for FMatrix allocation in codebooks
#  cbs:cb alloc introduced (may not run on old janus -> catch)
#
#  Revision 1.3.50.1  2001/09/24 17:13:02  metze
#  doKMeans now respects the ${SID}(REWRITE) variable, set to an array
#  containing file to codebook mappings
#
#  Revision 1.3  2000/09/10 12:18:40  hyu
#  Added Tcl files from Hagen
#
#  Revision 1.2  1999/10/21 09:38:32  soltau
#  doCombine flag
#
#  Revision 1.1  1999/10/21 09:20:22  soltau
#  Initial revision
# 
#
# ========================================================================


proc kmeans_Info { name } {
    switch $name {
	procs {
	    return "doKMeans"
	}
	help {
	    return "This file makes it easier to start EM training by 
initializing the codebooks with the K-Means algorithm. Before you can do
that, you need to extract samples."
	}
	doKMeans {
	    return "Performs K-Means in parallel, creating a CodebookSet (
a DistribSet is produced, too, but the weights are equally distributed). 
This procedure can combine and cluster data from different sample extractions."
	}
    }
    return "???"
}


# ------------------------------------------------------------------------
#  doKMeans
# ------------------------------------------------------------------------

proc doKMeans { LSID args } {

    # This is needed for initializing one codebook with multiple files
    upvar \#0 $LSID SID
    if {[info exists SID(REWRITE)]} { upvar \#0 $SID(REWRITE) VAR }
    
    set cbs           codebookSet$LSID
    set dss           distribSet$LSID
    set kmeans        kmeans
    set data          data
    set maxIter       5
    set maxCount      -1
    set tempF         .05
    set param         0i
    set distribUpdate 0
    set doCombine     0

    itfParseArgv doKMeans $args [list [
	list <cbListFile>   string  {} file      {}     {file of codebook names} ] [
        list -codebookSet   object  {} cbs       CodebookSet {codebook set}      ] [
        list -distribSet    object  {} dss       DistribSet  {distribution set}  ] [
        list -paramFile     string  {} param     {}     {base name of parameters}] [
        list -dataPath      string  {} data      {}     {path of sample files}   ] [
        list -kmeansPath    string  {} kmeans    {}     {path of kmeans files}   ] [
        list -distribUpdate int     {} distribUpdate {} {update distributions}   ] [
        list -tempF         float   {} tempF     {}     {final temperature}      ] [
        list -maxIter       int     {} maxIter   {}     {number of iterations}   ] [
        list -maxCount      float   {} maxCount  {}     {max no of samples}      ] [
	list -semFile       string  {} semFile   {}     {semaphore file}         ] [
        list -doCombine     int     {} doCombine {}     {combine samples on demand} ] ]

    # Only start if we need to
    if {[info exists semFile] && [file exists $semFile]} {
        writeLog $log "doKMeans: semaphore file '$semFile' exists, returning."
        return
    }
    if {![file exist            $kmeans]} { file mkdir           $kmeans }
    if {![file exists [file dir $param]]} { file mkdir [file dir $param] }

    doParallel {

	# --------------------------------------------------------------------
	#  Main Loop
	# --------------------------------------------------------------------

	FeatureSet [set fes fssmp]
	$fes FMatrix smpM$LSID
	$fes FMatrix smpT$LSID
	FVector smpC$LSID
	set smpM "${fes}:smpM${LSID}.data"
	set smpT "${fes}:smpT${LSID}.data"

	while {[fgets $file cb] >= 0} {
	    
	    $smpM resize 0 0
	    $smpT resize 0 0
	    if {[info exists VAR($cb)]} {
		# Use rewrite set (for e.g. Articulatory Features)
		set t        $VAR($cb)
		set doappend 0
		set modul    1
		set flist    ""
		
		foreach c $t { set flist [concat $flist [glob -nocomplain $data/data.*/${c}.smp $data/data.*/${c}.smp.SCP]] }
		set flist [lsort -unique $flist]

		if {![llength $flist]} {
		    puts stderr "ERROR: no samples exist for $cb from '$t'"
		    continue
		} else {
		    puts stderr "  REWRITING $cb: [llength $flist] sample files."
			
		    if {$maxCount == -1} { # We can simply read in the data
			foreach f $flist { 
			    $smpM bload $f -append $doappend 
			    set doappend 1
			}
			
		    } else { # We're expecting so much data that we'd like to modulo it
			
			foreach f $flist {
			    $smpT bload $f -append $doappend
			    set doappend 1
			    
			    if {[$smpT configure -m] > $maxCount} {
				if {$modul != 1} {
				    $smpT modulo [expr [$smpT configure -m]/$modul]
				    # puts stderr "    Reduced smpT to [$smpT configure -m] samples (modul $modul)."
				}
				if {![$smpM configure -m]} { $smpM copy $smpT } else { $fes concat smpM$LSID smpM$LSID smpT$LSID }
				set doappend 0
				while {[$smpM configure -m] > $maxCount} {
				    set modul [expr $modul*2]
				    puts stderr "    Have [$smpM configure -m] samples, changing modul to $modul."
				    $smpM modulo [expr [$smpM configure -m]/2]
				}
			    }
			}
			if {$modul != 1} {
			    $smpT modulo [expr [$smpT configure -m]/$modul]
			    # puts stderr "    Reduced $smpT to [$smpT configure -m] samples (modul $modul)."
			}
			if {![$smpM configure -m]} { $smpM copy $smpT } else { $fes concat smpM$LSID smpM$LSID smpT$LSID }
		    }
		}
		
	    } elseif {$doCombine} {
		# Combine several sample files
		set flist [glob -nocomplain $data/data.*/${cb}.smp $data/data.*/${cb}.smp.SCP]
		if {![llength $flist]} {
		    puts stderr "ERROR: $data/data.*/${cb}.smp does not exist."
		    continue
		} else {
		    $smpM cload [lindex $flist 0] -append 0
		    foreach f [lrange $flist 1 end] {$smpM cload $f -append 1}
		}
		
	    } else {
		# Do not combine several sample files
		if [file exists $data/${cb}.smp] {
		    $smpM bload  $data/${cb}.smp
		} else {
		    puts stderr "ERROR: $data/${cb}.smp does not exist."
		    continue
		}
	    }
	    
	    puts stderr "  Final smpM: [$smpM configure]"
	    if {[$smpM configure -n] && [$smpM configure -m]} {
		
		$smpM resize [$smpM configure -m] \
		    [expr [$smpM configure -n]-1]

		puts stderr "    $cbs:$cb do kmeans using [$smpM configure -m] samples"
		puts stderr "    $cbs:$cb.mat neuralGas -maxIter $maxIter -tempF $tempF"

		catch {$cbs:$cb alloc}
		$cbs:$cb.mat neuralGas $smpM -maxIter $maxIter \
		    -tempF $tempF -counts smpC$LSID

		$cbs:$cb.mat bsave $kmeans/$cb.mat
		smpC$LSID    bsave $kmeans/$cb.vec
	    }
	}
	
	$fes      delete smpM$LSID
	$fes      delete smpT$LSID
	smpC$LSID destroy
	
    } {
	
	# --------------------------------------------------------------------
	#  Server: Defining initial distributions/codebooks...
	# --------------------------------------------------------------------
	
	puts stderr "doKMeans: combining results ($kmeans) ..."
	
	foreach cb [$cbs:] {
	    if {[file exists $kmeans/${cb}.mat]} {
		puts stderr "doKMeans: loading $kmeans/$cb.mat ..."
		catch {$cbs:$cb alloc}
		$cbs:$cb.mat bload $kmeans/$cb.mat
	    }
	}
	
	if {$distribUpdate} {
	    
	    # find codebook to distribution mapping
	    
	    foreach ds [$dss:] {
		set     cb [$cbs name [$dss:$ds configure -cbX]]
		lappend DS($cb) $ds
	    }
	    
	    FVector cvec
	    
	    foreach cb [array name DS] {
		if [file exist $kmeans/$cb.vec] {
		    cvec bload $kmeans/$cb.vec
		    set  sum   0
		    set  vec   ""
		    
		    foreach x [cvec puts] { set sum     [expr $sum + $x]   }
		    foreach x [cvec puts] { lappend vec [expr $x   / $sum] }
		    puts stderr "init $cb distributions $DS($cb): $sum"
		    foreach x $DS($cb) {
			$dss:$x configure -count $sum -val $vec
		    }
		}
	    }
	    
	    cvec destroy
	}
	
	$cbs save ${param}.cbs.gz
	$dss save ${param}.dss.gz
	
    } {
	# --------------------------------------------------------------------
	#  Server Finish
	# --------------------------------------------------------------------

        if {[info exists semFile]} { touch $semFile }
    } {
	# --------------------------------------------------------------------
	#  Client
	# --------------------------------------------------------------------
	
    }
}


#########################################################################
# do it for 2 data sources
#########################################################################

# ------------------------------------------------------------------------
#  doKMeans2
# ------------------------------------------------------------------------

proc doKMeans2 { LSID args} {

  set cbs           codebookSet$LSID
  set dss           distribSet$LSID
  set kmeans        kmeans
  set data1         data
  set data2         data
  set maxIter       5
  set tempF         .05
  set param         0i
  set distribUpdate 0

  itfParseArgv doKMeans2 $args [list [
    list <cbListFile>   string  {} file     {}   {file of codebook names} ] [
    list -codebookSet   object  {} cbs      CodebookSet {codebook set}    ] [
    list -distribSet    object  {} dss      DistribSet  {distribution set}] [
    list -paramFile     string  {} param    {}   {base name of parameters}] [
    list -dataPath1     string  {} data1    {}   {path of sample files}   ] [
    list -dataPath2     string  {} data2    {}   {path of sample files}   ] [
    list -kmeansPath    string  {} kmeans   {}   {path of kmeans files}   ] [
    list -distribUpdate int     {} distribUpdate {} {update distributions}] [
    list -maxIter       int     {} maxIter  {}   {number of iterations}   ] [
    list -tempF         float   {} tempF    {}   {final temperature}      ] ]

  if {![file exist            $kmeans]} { file mkdir           $kmeans }
  if {![file exists [file dir $param]]} { file mkdir [file dir $param] }

  doParallel {

    # --------------------------------------------------------------------
    #  Main Loop
    # --------------------------------------------------------------------

    FMatrix smpM$LSID
    FVector smpC$LSID

    while {[fgets $file cb] >= 0} {
	if [file exists $data1/${cb}.smp] {
	    smpM$LSID bload  $data1/${cb}.smp

	    #append second data source
	    if [file exists $data2/${cb}.smp] {
		smpM$LSID bappend $data2/${cb}.smp
	    } else {
		puts stderr "ERROR: $data2/${cb}.smp does not exist. use only first data source !"
	    }
	    
	    smpM$LSID resize [smpM$LSID configure -m] \
		[expr [smpM$LSID configure -n]-1]

	    puts stderr "$cbs:$cb.mat neuralGas -maxIter $maxIter -tempF $tempF"
	    
	    $cbs:$cb.mat neuralGas smpM$LSID -maxIter $maxIter\
		-tempF $tempF -counts smpC$LSID
	    $cbs:$cb.mat bsave $kmeans/$cb.mat
	    smpC$LSID    bsave $kmeans/$cb.vec
	} else {
	    puts stderr "ERROR: $data1/${cb}.smp does not exist."
	}
    }

    smpM$LSID destroy
    smpC$LSID destroy
    
  } {

    # --------------------------------------------------------------------
    #  Server: Defining initial distributions/codebooks...
    # --------------------------------------------------------------------

    puts stderr "doKMeans: combining results..."

    foreach cb [$cbs:] {
      if [file exists $kmeans/${cb}.mat] {
        puts stderr "doKMeans: loading $kmeans/$cb.mat..."
        $cbs:$cb.mat bload $kmeans/$cb.mat
      }
    }

    if { $distribUpdate} {

    # find codebook to distribution mapping

    foreach ds [$dss:] {
      set     cb [$cbs name [$dss:$ds configure -cbX]]
      lappend DS($cb) $ds
    }

    FVector cvec

    foreach cb [array name DS] {
      if [file exist $kmeans/$cb.vec] {
        cvec bload $kmeans/$cb.vec
        set  sum   0
        set  vec   ""

        foreach x [cvec puts] { set sum     [expr $sum + $x]   }
        foreach x [cvec puts] { lappend vec [expr $x   / $sum] }
        puts stderr "init $cb distributions $DS($cb): $sum"
        foreach x $DS($cb) {
          $dss:$x configure -count $sum -val $vec
        }
      }
    }

    cvec destroy
    }

    codebookSet$LSID save ${param}.cbs.gz
    distribSet$LSID  save ${param}.dss.gz

  } {

  } {

    # --------------------------------------------------------------------
    #  Client
    # --------------------------------------------------------------------

  }
}
