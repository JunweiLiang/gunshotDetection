# ========================================================================
#  JANUS-SR   Janus Speech Recognition Toolkit
#             ------------------------------------------------------------
#             Object: ngets -- the networked version of fgets
#             ------------------------------------------------------------
#
#  Author  :  Hua Yu
#  Module  :  ngets.tcl
#  Date    :  $Id: ngets.tcl 3328 2010-11-06 20:37:43Z metze $
#
#  Remarks :  Sample session:
#        ali> janus
#           % ngetsServerStart
#        On other machines, start ngets clients
#          
#        ali: don't crash this janus console, can do nputs
#            or ngetsServerStop. And thanks to Tcl's Eventloop,
#            I can manipulate stuff without worrying about competing
#            so long as it's completed in a single step.
#
#  Todo    :  - rethink about the design, structure it in a way easy
#               to add extensions: like process monitoring, automatic
#               restart a process
#             - replace fgets with normal gets, if server robust enough
#             - nputs can be used to pool warp together, though need a
#               a way to tell this from finished job report
#             - efficiency when lots of clients doing simple task
# ========================================================================
#
#  $Log$
#  Revision 1.3  2003/08/14 11:20:29  fuegen
#  Merged changes on branch jtk-01-01-15-fms (jaguar -> ibis-013)
#
#  Revision 1.2.2.7  2003/06/26 15:09:20  metze
#  Changes for V5.0 P013
#
#  Revision 1.2.2.6  2003/04/09 13:20:13  metze
#  Cleaned up ngets system
#
#  Revision 1.4  2003/04/09 13:15:06  metze
#  Changes to ngets
#
#  Revision 1.3  2003/01/17 15:42:55  fuegen
#  Merged changes on branch jtk-01-01-15-fms
#
#  Revision 1.2.2.4  2003/01/11 11:01:40  metze
#  Introduced glob.org, avoided exec
#
#  Revision 1.2.2.3  2003/01/08 16:41:02  metze
#  ngets now keeps keys in memory during runtim
#
#  Revision 1.2.2.2  2002/11/15 09:17:46  soltau
#  Added touch
#
#  Revision 1.2.2.1  2002/11/05 08:48:11  metze
#  Added multiple keys to ngets
#
#  Revision 1.2  2001/01/11 12:33:17  janus
#  Updated ReleaseLog
#
#  Revision 1.1.2.1  2000/12/10 14:06:12  janus
#  WAV support, ngets, multivar
#
#
# ========================================================================


# ========================================================================
#  ngets              provide same functionality as fgets over socket
#         
#  Note: - be aware the file is accessed by another machine, so local
#          directory names like /usr0/hyu/... won't work
#        - backoff to "fgets" only when server connection can't be 
#          established
# ========================================================================

proc ngets { file v } {
    upvar  $v myvar
    global NGETS

    set MaxFails 10

    # Do we have a locally stored list? (mgets)
    if {[info exists NGETS($file)] && [llength $NGETS($file)]} {
	set myvar        [lindex $NGETS($file) 0]
	set NGETS($file) [lrange $NGETS($file) 1 end]
	return [string length $myvar]
    }

    # Make sure we can read data/ Fallback
    if {![info exists NGETS(FAILED)]} {set NGETS(FAILED) 0}
    while {[catch {set sock [socket $NGETS(HOST) $NGETS(PORT)]} errmsg]
	   && $NGETS(FAILED) < $MaxFails} {
	puts stderr "WARNING: ngets $NGETS(HOST):$NGETS(PORT) returned '$errmsg'."
	after 30000
	incr NGETS(FAILED)
    }
    if {$NGETS(FAILED) == $MaxFails} {
	puts stderr "WARNING: ngets falling back to fgets."
	return [fgets-org $file myvar]
    }

    # Get data
    fconfigure $sock -buffering line
    if {[info exists NGETS(MGETS)] && $NGETS(MGETS)} {
	puts $sock "gets $file $NGETS(MGETS)"
    } else {
	puts $sock "gets $file 1"
    }
    set retval [gets $sock myvar]
    close $sock

    # Are we using mgets?
    if {[info exists NGETS(MGETS)] && $NGETS(MGETS)} {
	set NGETS($file) [lrange $myvar 1 end]
	set myvar        [lindex $myvar 0]
	set retval       [string length $myvar]
    }

    # Standard behaviour: return -1 on end
    if {![string length $myvar]} {set retval -1}
    
    return $retval
}

proc nglob { files } {
    global NGETS

    set MaxFails 10

    # Check availability of server
    if {![info exists NGETS(FAILED)]} {set NGETS(FAILED) 0}
    while {[catch {set sock [socket $NGETS(HOST) $NGETS(PORT)]} errmsg]
	   && $NGETS(FAILED) < $MaxFails} {
	puts stderr "WARNING: nglob $NGETS(HOST):$NGETS(PORT) returned '$errmsg'"
	after 30000
	incr NGETS(FAILED)
    }
    if {$NGETS(FAILED) == $MaxFails} {
	puts stderr "WARNING: trying to use local glob."
	if {[llength [info command glob-org]]} {
	    return [glob-org -nocomplain -- $files]
	} else {
	    return [glob     -nocomplain -- $files]
	}
    }

    # Get data
    fconfigure $sock -buffering line
    puts $sock "glob $files"
    set err [gets $sock line]
    close $sock

    return $line
}


# ========================================================================
#  ngetsServer            serve incoming client requents
#         
#  Note: - this will read the file only once!
#        - you can re-read files
# ========================================================================

proc ngetsAccept { sock addr port } {
    global NGETS NGETS_JOBS

    set multi            1
    set clientHost       [lindex [fconfigure $sock -peername] 1]
    regsub {\..+}        $clientHost {} clientHost
    regexp "pc(\[0-9\])" $clientHost x  multi
    
    set retval [gets $sock cmd]
    if {$NGETS(VERBOSE) >= 2} { puts stderr "recv $clientHost: $cmd" }
    
    if       {[llength $cmd] == 2} {
	set multi 1
    } elseif {[llength $cmd] == 3} {
	set multi [lindex $cmd 2]
    } else {
	puts stderr "Unrecognized command: '$cmd'"
	close $sock
	return
    }
    set op    [lindex $cmd 0]
    set file  [lindex $cmd 1]

    while {[info exists NGETS(BUSY)] && $NGETS(BUSY)} { vwait NGETS(BUSY) }

    switch $op {
	gets {
	    set x   0
	    set str ""
	    for {set i 0} {$i < $multi && $x >= 0} {incr i} {
		if {[set x [ngetsJobNext $file tmp]] < 0} {
		    ngetsJobLog -1          $file ""
		} else {
		    lappend str $tmp
		    ngetsJobLog $clientHost $file $tmp
		}		
	    }
	    if {$x < 0} {
		if {$NGETS(VERBOSE) >= 1} { puts "send $clientHost: exhausted $file" }		    
	    } else {
		if {$NGETS(VERBOSE) >= 2} { puts "send $clientHost: $str" }
	    }
	    puts $sock $str
	}
	puts {
	    set retval [gets $sock line]
	    if {$retval < 0} {
		puts stderr "No arguments for puts"
	    } else {
		if {$NGETS(VERBOSE) >= 2} { puts "nputs: $file $line" }
		nputsJobLog $clientHost "$line"
		append_file $file $line
	    }
	}
	glob {
	    if {[catch {set str [glob-org -nocomplain -- $file]}]} {
		set str         [glob     -nocomplain -- $file]
	    }
	    if {$NGETS(VERBOSE) >= 3} { puts "glob: $file = $str" }
	    puts $sock $str
	}
	touch {
	    catch { file delete $file }
	    set ourfilepointer [open $file w]; close $ourfilepointer
	    if {$NGETS(VERBOSE) >= 2} { puts "touch: $file" }
	    puts $sock $file
	}
 	forget {
 	    catch { unset NGETS_JOBS($file) }
 	    catch { unset NGETS_JOBS($file,TODO) }
 	    catch { unset NGETS_JOBS($file,DONE) }
 	    puts stderr "Forget: $file"
 	}
	default {
	    puts stderr "Unrecognized operation: $op"
	}
    }
    close $sock
}


proc ngetsServerStart { args } {
    global NGETS

    if [info exists NGETS(CurrentSock)] {
	puts stderr "WARNING: server already started"
	return -1
    }

    if {[catch {set sock [socket -server ngetsAccept $NGETS(PORT)]} errmsg]} {
	puts stderr "ERROR: $errmsg"
	return -1
    } else {
	puts "Server accepting connection on $NGETS(PORT) ..."
	set NGETS(CurrentSock) $sock
	if {$NGETS(VERBOSE) >= 2} { puts stderr "CurrentSock: $sock" }
	return 0
    }
}

proc ngetsServerStop { args } {
    global NGETS

    if [info exists NGETS(CurrentSock)] {
	close $NGETS(CurrentSock)
	unset  NGETS(CurrentSock)
	puts "Server stopped."
    } else {
	puts stderr "WARNING: no current socket on duty"
    }
}


# ------------------------------------------------------------------------
#  nputs                  puts
#
#  Note: - this relies on Tcl's event loop to ensure no competing.
#          Of course, this means only single server is allowed
#          i.e. although it might listen to multiple clients simultaneously,
#        - backoff to plain "puts" only when server connection can't be 
#          established
# ------------------------------------------------------------------------

proc nputs { file line } {
    global NGETS

    if [catch {set sock [socket $NGETS(HOST) $NGETS(PORT)]} errmsg] {
	puts stderr "WARNING: backoff to plain puts ($errmsg)"
	set fp [open $file a]
	puts fp $line
	close $fp
	return -1
    }

    puts  $sock "puts $file"
    puts  $sock "$line"
    close $sock
}

proc nputsJobLog { host job } {
    global NGETS_JOBS

    regsub "\n" [date] {} time

    if [info exists NGETS_JOBS($job)] {
	unset NGETS_JOBS($job)
    } else {
	puts stderr "nputsLog: $job hasn't been checked out yet! $host"
    }

    if [llength [info commands ngetsGUIJobUnLog]] { 
	ngetsGUIJobUnLog $job "$host $time"
    }
}


# ------------------------------------------------------------------------
#  ngets
#
#  queue(jobId) = {host time_checkout}
#
#  Todo: a Tk GUI display of outstanding jobs
# ------------------------------------------------------------------------

proc ngetsJobLog { host file str } {
    global NGETS_JOBS

    if {$host == "-1"} {
	# It's finished
	if [llength [info commands ngetsGUIJobLog]] { 
	    ngetsGUIJobLog $host $file $str
	}

	if [info exists NGETS_JOBS($file)] { ngetsJobWrite $file }

	foreach i [array names NGETS_JOBS $file*] { unset NGETS_JOBS($i) }
	return
    }

    if {![info exists NGETS_JOBS($file)]} {
	# A new job, note start time
	set NGETS_JOBS($file) [clock seconds]
    }

    # Store info
    #set NGETS_JOBS($file,$str,HOST) $host
    #set NGETS_JOBS($file,$str,TIME) [clock seconds]

    # GUI Interface
    if [llength [info commands ngetsGUIJobLog]] { 
	ngetsGUIJobLog $host $file $str
    }
}

proc ngetsJobRead { file } {
    global NGETS_JOBS NGETS

    set NGETS_JOBS($file,TODO) [set NGETS_JOBS($file,DONE) ""]
    if {![catch {set fp [open $file r]}]} {
	while {[gets $fp line] != -1} {
	    if {[regexp "^\#" $line]} {
		lappend NGETS_JOBS($file,DONE) [string range $line 1 end]
	    } else {
		lappend NGETS_JOBS($file,TODO) $line
	    }
	}
	close $fp
	set todo [llength $NGETS_JOBS($file,TODO)]
	if {$todo && $NGETS(VERBOSE) >= 1} {
	    puts "ngetsJobRead: read done=[llength $NGETS_JOBS($file,DONE)], todo=$todo keys from $file"
	}
    } else {
	puts stderr "ngetsJobRead: couldn't open $file for reading"
    }
}

proc ngetsJobWrite { file } {
    global NGETS_JOBS

    if {![catch {set fp [open $file w]}]} {
	foreach i $NGETS_JOBS($file,DONE) { puts $fp "#${i}" }
	foreach i $NGETS_JOBS($file,TODO) { puts $fp $i }
	close $fp
	puts "ngetsJobWrite: wrote done=[llength $NGETS_JOBS($file,DONE)], todo=[llength $NGETS_JOBS($file,TODO)] keys into $file"
    } else {
	puts stderr "ngetsJobWrite: couldn't open $file for writing"
    }
}

proc ngetsJobNext { file var } {
    global NGETS_JOBS
    upvar $var key

    if {![info exists NGETS_JOBS($file)]} {
	ngetsJobRead $file
    }

    if {![info exists NGETS_JOBS($file,TODO)] || ![llength $NGETS_JOBS($file,TODO)]} {
	set key ""
	return -1
    }

    lappend NGETS_JOBS($file,DONE) [set key [lindex   $NGETS_JOBS($file,TODO) 0]]
    set     NGETS_JOBS($file,TODO)          [lreplace $NGETS_JOBS($file,TODO) 0 0]

    return [string length $key]
}

# --------------------------------------------------------
#
#  Initializing
#
# --------------------------------------------------------

if {$argc == 1 && [lindex $argv 0] == "-server"} {
    printDo ngetsServerStart
    while 1 { 
	after 10
	update
    }
}
if {$argc == 2 && [lindex $argv 0] == "-forget"} {

    proc ngetsForget { file } {
	global NGETS
	if {[catch {set sock [socket $NGETS(HOST) $NGETS(PORT)]} errmsg]} {
	    writeLog stderr "Error opening socket $NGETS(HOST):$NGETS(PORT) : $errmsg"
	    return -1
	}
	fconfigure $sock -buffering line
	puts $sock "forget [file normalize $file]"
	set retval [gets $sock myvar]
	close $sock
	return $myvar
    }

    printDo ngetsForget [lindex $argv 1]
    exit
}

# --------------------------------------------------------
#
#  sgets - the "simple" fgets/ ngets replacement
#
# --------------------------------------------------------

if {0} {
    #
    # Here's example code how to use this on the client
    # <<< ---
    package require comm
    ::comm::comm configure -local 0 -listen 0
    rename fgets fgets-org

    # Define our new fgets
    proc fgets { file var_ } {
	upvar $var_ var
	return [sgets $file var]
    }
    # --- >>>

    #
    # You need to set up a server as follows (the example assumes it runs on islr0s14)
    #
    # <<< ---
    package require comm
    set file comm-fgets.id
    
    # Start the server and communicate the socket
    ::comm::comm configure -local 0
    puts     [::comm::comm self]
    set fp   [open $file w]; puts $fp [::comm::comm self]; close $fp
    file attributes $file -permissions u+rw,go-rwx
    
    # Define the helper function
    proc F { file } { return [list [fgets $file var] $var] }
    
    # Make sure this can be started as a script
    if {!$tcl_interactive} { vwait dummy_var }
    # --- >>>
}

proc sgets { file var_ } {
    upvar  $var_ line
    global fgetsEXHAUST fgetsCACHE fgetsID env
    set ip islr0s14

    regsub "/.automount/islr0s14/usr3/data/ASR"  [file normalize $file] "/data/ASR2" file
    regsub "/.automount/islr0s14/usr4/data/ASR3"                 $file  "/data/ASR3" file
    if {![info exists fgetsID]} { set fp [open $env(HOME)/comm-fgets.id r]; gets $fp fgetsID; close $fp }

    # Deal with Condor eviction
    if {[info exists fgetsEXHAUST] && $fgetsEXHAUST} {
	writeLog stderr "fgets: honoring fgetsEXHAUST"
	if {[info exists fgetsCACHE] && [llength $fgetsCACHE]} { catch {
	    writeLog stderr "fgets: appending '$fgetsCACHE' to $file"
	    ::comm::comm send "$fgetsID $ip" "set var \"$fgetsCACHE\"; fputs $file var -append 1"
	} }
	return -1 
    }

    # Make sure we have the file
    if {![file exists $file]} { return -1 }

    # Deal with existing multiple fgets
    if {[info exists fgetsCACHE] && [llength $fgetsCACHE]} {
	set line       [lindex $fgetsCACHE 0]
	set fgetsCACHE [lrange $fgetsCACHE 1 end]
	return [string length $line]
    }

    # Get our key remotely
    if {[catch { 
	set ret [::comm::comm send "$fgetsID $ip" "F $file"] 
	set line [lindex $ret 1]
    }]} {
	return -1
    }

    # Did we get multiple keys?
    if {[llength $line] > 1} {
	set fgetsCACHE [lrange $line 1 end]
	set line       [lindex $line 0]
    }

    return   [lindex $ret 0]
}


# --------------------------------------------------------
#
#  cgets - the condor fgets replacement
#
#        * condorInit redefines fgets -> cgets
#        * can be used transparently outside of condor (relies on environment variables)
#        * keeps track of keys using files
#        * condorAbort can push the current key back on the stack
#        
# --------------------------------------------------------

proc condorInit { LSID args } {
    upvar \#0 $LSID SID
    global env doParallelBODYONLY doParallelSERVERONLY

    # Scan command line parameters
    itfParseArgv condorInit $args [list [
	list -nowant       int     {} nowant     {}         {do not use WANT}   ] [
        list -verbose      int     {} verbose    {}         {verbose}           ] ]

    # Make sure we run under Condor
    if {![info exists env(C_NOJOBS)]} {
	writeLog stderr "condorInit: no Condor environment found, ignoring"
	return
    }

    # do not use the WANT mechanism
    if {[info exist nowant] && $nowant} {
	global fgetsCACHE
	set fgetsCACHE(NOWANT) 1
    }
    if {[info exist verbose] && $verbose} {
	global fgetsCACHE
	set fgetsCACHE(VERBOSE) 1
    }

    # A clean way to handle TERM?
    if {1 || [catch {
	package require Tclx
	signal trap TERM { 
	    global MostRecentlyReceivedSignal
	    set MostRecentlyReceivedSignal "TERM"
	    puts stderr "\nTclx: trapped TERM signal ([date])\n" 
	}
    } msg]} {
	proc sigTermScript { } {
	    global MostRecentlyReceivedSignal
	    set MostRecentlyReceivedSignal "TERM"
	    puts stderr "\nsigTermScript: trapped TERM signal ([date])\n" 	    
	}
    }

    # By default, execute body only
    if {![info exists doParallelBODYONLY] && ![info exists doParallelSERVERONLY]} {
	set doParallelBODYONLY 1
    }
    if {[info exists doParallelSERVERONLY] && $doParallelSERVERONLY && $env(C_PROCESS)} {
	writeLog stderr "condorInit: not executing server only for process $env(C_PROCESS)"
	exit
    }
    
    # Wait if there seem to be stale INIT files
    while {[info exists doParallelSERVERONLY] && $doParallelSERVERONLY && [llength [glob -nocomplain *.INIT]]} {
	writeLog stderr "condorInit: attempting to run as server, but suspecting stale *.INIT files - waiting ([date])"
	after 300000
    }

    # Switch on cgets
    rename fgets fgets-org
    proc fgets { file var_ } { upvar $var_ var; return [cgets $file var] }
}

# ---------------------------------------------------------------------------
#   check for TERM signal and make sure the current key doesn't get counted
# ---------------------------------------------------------------------------

proc condorCheckAbort { args } {
    global fgetsCACHE env 
    global MostRecentlyReceivedSignal

    set verbose 0
    set commit  0
    regsub ",done" [array names fgetsCACHE "*,done"] "" file

    if {[info exists fgetsCACHE(VERBOSE)]} { set verbose $fgetsCACHE(VERBOSE) }

    # Scan command line parameters
    itfParseArgv condorCheckAbort $args [list [
	list -file         string  {} file       {}         {file to use}              ] [
	list -commit       int     {} commit     {}         {commit this key}          ] [
        list -verbose      int     {} verbose    {}         {verbose}                  ] ]

    # Make sure we can work
    if {![info exists env(C_PROCESS)]} { return 0 }

    # All is well
    if {![info exists MostRecentlyReceivedSignal] || $MostRecentlyReceivedSignal != "TERM"} { 
	return 0 
    }

    # Remove the current key so it won't be counted
    if {[info exists fgetsCACHE($file)] && [info exists fgetsCACHE($file,done)] && !$commit} {
	set   fgetsCACHE($file) [linsert $fgetsCACHE($file) 0 $fgetsCACHE($file,done)]
	unset fgetsCACHE($file,done) 
	writeLog stderr "condorCheckAbort: pushed [lindex $fgetsCACHE($file) 0]"
    } else {
	writeLog stderr "condorCheckAbort: no action"
    }

    # Abort on return
    return 1
}

# --------------------------
#   fgets replacement
# --------------------------

proc execat { file } {
    set fp [open $file r]
    while {[gets $fp l] != -1} { lappend f $l }
    close $fp
    if {![info exists f]} { return }
    return $f
}

proc cgets { file var_ } {
    upvar  $var_ line
    global fgetsCACHE env
    global MostRecentlyReceivedSignal

    # Make sure we can work
    if {![info exists env(C_PROCESS)] || ![info exists env(C_NOJOBS)]} {
	writeLog stderr "**** cgets: Condor environment not found, trying fall-back ****"
	parray env
	return [fgets-org $file line]
    }

    # Apparently, we successfully completed this key - book-keeping
    if {[info exists fgetsCACHE($file,done)]} { 
	if {[catch {
	    set    fp [open $file.$env(C_PROCESS) a]
	    puts  $fp $fgetsCACHE($file,done)
	    close $fp
	} msg]} { writeLog stderr "cgets: '$msg'" }
	unset fgetsCACHE($file,done)
    }

    # Deal with Condor eviction
    if {[info exists MostRecentlyReceivedSignal] && $MostRecentlyReceivedSignal == "TERM"} {
	if {[info exists fgetsCACHE($file)]} {
	    set    fp [open $file.$env(C_PROCESS).TODO w]
	    foreach w $fgetsCACHE($file) { puts $fp $w }
	    close $fp
	}
	file delete $file.$env(C_PROCESS).WANT
	writeLog stderr "\ncgets: honoring MostRecentlyReceivedSignal==\"TERM\"\n"
	return -1 
    }

    # Load the keys in memory, if we haven't done so
    if {![info exists fgetsCACHE($file)]} {
	if {![file exists $file]} {
	    writeLog stderr "cgets: file '$file' not found"
	    return -1
	}
	if {[file exists $file.$env(C_PROCESS).DONE]} {
	    # We have a DONE file
	    writeLog stderr "cgets: found $file.$env(C_PROCESS).DONE"
	    set line ""
	    return -1
	} elseif {[file exists $file.$env(C_PROCESS).TODO]} {
	    # We have a TODO file
	    set fgetsCACHE($file) [execat $file.$env(C_PROCESS).TODO]
	    writeLog stderr "cgets: reading [llength $fgetsCACHE($file)] keys from '$file.$env(C_PROCESS).TODO'"
	    file delete $file.$env(C_PROCESS).TODO
	} elseif {[file exists $file.$env(C_PROCESS)]} {
	    # Some files have already been processed
	    set keys [execat $file]
	    set k1   [expr int(1.0*[llength $keys]*   $env(C_PROCESS) /$env(C_NOJOBS)  )]
	    set k2   [expr int(1.0*[llength $keys]*(1+$env(C_PROCESS))/$env(C_NOJOBS)-1)]
	    foreach key [lrange $keys $k1 $k2] { set tmpA($key) 1 }
	    foreach key [execat $file.$env(C_PROCESS)] { catch { unset tmpA($key) } }
	    set fgetsCACHE($file) [array names tmpA]
	    writeLog stderr "cgets: reading [llength $fgetsCACHE($file)] keys from '${file}\[$k1:$k2\]\\${file}.$env(C_PROCESS)'"
	} else {
	    # Default case - generate our keys
	    set keys [execat $file]
	    set k1   [expr int(1.0*[llength $keys]*   $env(C_PROCESS) /$env(C_NOJOBS)  )]
	    set k2   [expr int(1.0*[llength $keys]*(1+$env(C_PROCESS))/$env(C_NOJOBS)-1)]
	    set fgetsCACHE($file) [lrange $keys $k1 $k2]
	    writeLog stderr "cgets: reading \[$k1:$k2\] from '$file' ([llength $fgetsCACHE($file)])"
	}
	# This is the minimum number of keys we keep to ourselves to finish correctly
	#set fgetsCACHE($file,min) [expr int(ceil(0.03*([llength $fgetsCACHE($file)])))]
	set fgetsCACHE($file,min) 3
    } else {
	if {[info exists fgetsCACHE(VERBOSE)] && $fgetsCACHE(VERBOSE)} {
	    writeLog stderr "cgets: '$file' cached ([llength $fgetsCACHE($file)])"
	}
    }

    # Let's redistribute work by setting and reading 'WANT'
    switch [llength $fgetsCACHE($file)] {
	1 {
	    if {![info exists fgetsCACHE(NOWANT)]} {
		# We'll soon be out of work
		writeLog stderr "cgets: setting $file.$env(C_PROCESS).WANT"
		touch       $file.$env(C_PROCESS).WANT
	    }
	}
	0 {
	    # Hopefully, we got more work, let's wait for a moment, then read it
	    for {set i 0} {$i < 2 && ![llength [glob -nocomplain $file.$env(C_PROCESS).WANT.*]]} {incr i} {
		after 60000
	    }
	    file delete $file.$env(C_PROCESS).WANT
	    after 10000
	    foreach f [glob -nocomplain $file.$env(C_PROCESS).WANT.*] {
		set fgetsCACHE($file) [concat $fgetsCACHE($file) [execat $f]]
		file delete $f
		writeLog stderr "cgets: pushing from $f"
	    }
	    writeLog stderr "cgets: now have [llength $fgetsCACHE($file)] keys"
	}
	default { catch {
	    # Do others (still) want more work?
	    foreach f [glob -nocomplain $file.*.WANT] {
		if {$f == "$file.$env(C_PROCESS).WANT" || 
		    [file exists $f.$env(C_PROCESS)] || ![file exists $f]} { 
		    continue 
		}
		# This is the number of keys we push
		set l [llength $fgetsCACHE($file)]
		set n [expr $env(C_NOJOBS)-[llength [glob -nocomplain $file.*.DONE]]]
		set k [expr int(ceil(1.0*$l*(2.0+$n)/(4.0*$n)))]
		# Let's keep the last few keys for ourselves
		if {[expr $l-$k] < $fgetsCACHE($file,min)} { break }
		writeLog stderr "cgets: popping $k keys to $f (have $l)"
		incr k -1
		if {[catch {
		    set    fp [open $f.$env(C_PROCESS) w]
		    foreach w [lrange $fgetsCACHE($file) end-$k end] { puts $fp $w }
		    close $fp
		    # If the receiving process crashes,
		    # the keys will not be processed!
		}]} {
		    file delete $f.env(C_PROCESS)
		} else {
		    set fgetsCACHE($file) [lreplace $fgetsCACHE($file) end-$k end]
		}
	    } }
	}
    }

    # Return the next key, if we have one
    if {[llength $fgetsCACHE($file)]} {
	set line                   [lindex   $fgetsCACHE($file) 0]
	set fgetsCACHE($file)      [lreplace $fgetsCACHE($file) 0 0]
	set fgetsCACHE($file,done)  $line
	return [string length $line]
    }

    # We're done
    set line ""
    file rename $file.$env(C_PROCESS) $file.$env(C_PROCESS).DONE
    return -1
}


# This we could use, but it seems to be causing problems from time to time
if {0} {
proc sigTermScript { } {
    # This gets executed when we receive an interrupt request
    set qvar  spk

    global spk utt env $qvar
    global doParallelJOB doParallelBODYONLY doParallelSERVERONLY fgetsEXHAUST

    if {[info exists doParallelJOB]} { set e ${doParallelJOB}$env(HOST).[pid] 
    } else {                           set e                 $env(HOST).[pid] }

    # Welcome
    writeLog stderr "\n  <<<< Received TERM on [date] for $e >>>>"
    if {[info exists spk] && [info exists utt]} {
    writeLog stderr "<<<< Received TERM at $spk $utt >>>>" }
    if {[info exists $qvar]} {
    writeLog stderr "<<<< $qvar contains '[set $qvar]' >>>>" }

    # Do the work
    switch "[file exists $e.INIT]-[file exists $e.END]-[file exists $e.SERVER]" {
	"0-1-0" -
        "1-0-0" { # we are executing the body
            writeLog stderr "set fgetsEXHAUST = doParallelBODYONLY = 1"
            set fgetsEXHAUST       1
            set doParallelBODYONLY 1
        }
        "1-1-0" { # we are waiting for the others to finish the body
            writeLog stderr "<<<< Good bye >>>>"
            file delete $e.INIT $e.END
            exit
        }
        "1-0-1" -
        "0-0-1" -
        "1-1-1" { # we are the server (in various states)
            writeLog stderr "set fgetsEXHAUST = doParallelBODYONLY = 1 (inside server)"
            set fgetsEXHAUST       1
            set doParallelBODYONLY 1        
        }
        default { # we assume we can simply quit
            writeLog stderr "<<<< Bye bye >>>>"
            exit
        }
    }

    # Just finish the current key
    writeLog stderr "<<<< Back to work >>>>"
}
}
