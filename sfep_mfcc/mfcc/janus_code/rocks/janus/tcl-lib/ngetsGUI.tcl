# ========================================================================
#  JANUS-SR   Janus Speech Recognition Toolkit
#             ------------------------------------------------------------
#             Object: ngetsGUI -- interface for ngets
#             ------------------------------------------------------------
#
#  Author  :  Hua Yu
#  Module  :  ngetsGUI.tcl
#  Date    :  $Id: ngetsGUI.tcl 2390 2003-08-14 11:20:32Z fuegen $
#
#  Remarks :  Interaction with ngets.tcl: NGETS(BUSY) NGETS(CurrentSock)
#             and 2 callback functions ngetsGUIJobLog(), ngetsGUIJobUnLog()
#
#             Sample session:
#               source ngetsGUI.tcl
#               (press start)
#
# ========================================================================
#
#  $Log$
#  Revision 1.3  2003/08/14 11:20:29  fuegen
#  Merged changes on branch jtk-01-01-15-fms (jaguar -> ibis-013)
#
#  Revision 1.2.2.5  2003/08/12 16:12:39  metze
#  Cleanup for P013
#
#  Revision 1.2.2.4  2003/04/09 13:20:13  metze
#  Cleaned up ngets system
#
#  Revision 1.4  2003/04/09 13:15:25  metze
#  Changes to ngets
#
#  Revision 1.3  2003/01/17 15:42:55  fuegen
#  Merged changes on branch jtk-01-01-15-fms
#
#  Revision 1.2.2.3  2003/01/08 16:41:03  metze
#  ngets now keeps keys in memory during runtim
#
#  Revision 1.2.2.2  2002/01/21 14:55:13  metze
#  Better alignment
#
#  Revision 1.2.2.1  2001/11/13 16:12:19  metze
#  Removed Hard-Link to my home directory
#
#  Revision 1.2  2001/01/11 12:33:17  janus
#  Updated ReleaseLog
#
#  Revision 1.1.2.1  2000/12/10 14:06:12  janus
#  WAV support, ngets, multivar
#
#
# ========================================================================


proc ngetsGUITimer {} {
    #
    # Updates the Frontend
    #
    global NGETS

    after $NGETS(TIMER) ngetsGUITimer

    .butt.time configure -text [clock format [clock seconds] -format "%a %H:%M"]
}

proc ngetsGUIJobLog { host file str } {
    #
    # The procedure which gets called for updates
    #
    global NGETS_JOBS NGETSGUIJobSelected
    
    regsub -all {\.} $file {%} fileJob
    set butName .list.job$fileJob
    
    if {$host == "-1"} {
	destroy $butName
	catch {unset NGETSGUIJobSelected($file)}
	return
    } elseif {![winfo exists $butName]} {
	set NGETSGUIJobSelected($file) 0
	checkbutton $butName -variable NGETSGUIJobSelected($file) -justify left
	pack $butName
    }

    set done                [llength $NGETS_JOBS($file,DONE)]
    set items [expr $done + [llength $NGETS_JOBS($file,TODO)]]
    set perc  ?
    catch {set perc [expr int(100.0*$done/$items+0.5)]}

    if {$done == 0} {
	set eta [clock seconds]
    } else {
	set eta [expr int(0.5 + $NGETS_JOBS($file) + ([clock seconds]-$NGETS_JOBS($file))*$items/$done)]
    }

    $butName configure -text "$file\n[clock format $NGETS_JOBS($file) -format {%a %H:%M}]  CUR    ('$str' on $host)\n[clock format $eta -format {%a %H:%M}]  ETA    (${perc}% of $items)"

    if {[llength [pack slaves .list]]} {
	pack $butName -before [lindex [pack slaves .list] 0] -side top -expand 1 -anchor w
    }
}

proc ngetsGUIJobUnLog {job info} {
    global NGETSGUIJobSelected

    regsub -all {\.} $job {%} pathJob
    set butName .list.job$pathJob
    
    if {![winfo exists $butName]} {
	return
    }
    destroy $butName; unset   NGETSGUIJobSelected($job)

    #ngetsGUIProgress [lindex $job 0]
}

proc ngetsGUIJobClear {} {
    global NGETSGUIJobSelected NGETS_JOBS

    foreach job [array names NGETSGUIJobSelected] {
	if {!$NGETSGUIJobSelected($job)} { continue }
	regsub -all {\.} $job {%} pathJob
	set butName .list.job$pathJob
	destroy $butName
	foreach i [array names NGETS_JOBS $job*] { unset NGETS_JOBS($i) }
	catch {unset NGETSGUIJobSelected($job)}
	puts stderr "ngetsGUIJobClear: cleared $job"
    }
}

proc ngetsGUIJobWrite {} {
    global NGETSGUIJobSelected

    foreach job [array names NGETSGUIJobSelected] {
	if {!$NGETSGUIJobSelected($job)} { continue }
	regsub -all {\.} $job {%} pathJob
	ngetsJobWrite $job
	catch {unset NGETSGUIJobSelected($job)}
    }
}

proc ngetsGUIJobRead {} {
    global NGETSGUIJobSelected

    foreach job [array names NGETSGUIJobSelected] {
	if {!$NGETSGUIJobSelected($job)} { continue }
	regsub -all {\.} $job {%} pathJob
	ngetsJobRead $job
	catch {unset NGETSGUIJobSelected($job)}
    }
}

# toggle start/stop
proc ngetsGUIStart {} {
    global NGETS

    if {![info exists NGETS(INITIALIZED)]} {
	set NGETS(INITIALIZED) 1

	wm deiconify .

	frame       .butt
	button      .butt.quit   -text "Quit"   -command ngetsGUIQuit
	button      .butt.start  -text "Start"  -command ngetsGUIStart
	button      .butt.read   -text "Read"   -command ngetsGUIJobRead
	button      .butt.write  -text "Write"  -command ngetsGUIJobWrite
	button      .butt.clear  -text "Clear"  -command ngetsGUIJobClear
	checkbutton .butt.busy   -text "Busy"   -variable NGETS(BUSY)
	label       .butt.time   -text ""
	frame       .list
	
	canvas .sep -relief sunken -borderwidth 1 -height 0
	
	pack .butt.start .butt.read .butt.write .butt.clear -side left
	pack .butt.time -side right
	pack .butt .sep .list -side top -fill x
	
	set     NGETS(TIMER) 6000
	after  $NGETS(TIMER) ngetsGUITimer
	update idletasks
    }

    if {![info exists NGETS(CurrentSock)]} {
	ngetsServerStart
	.butt.start configure -text "Stop"
	.butt.start configure -fg   red
    } else {
	ngetsServerStop
	.butt.start configure -text "Start"
	.butt.start configure -fg   black
    }
}

proc ngetsGUIQuit {} {
    global NGETS

    if {[info exists NGETS(CurrentSock)]} {
	ngetsServerStop
    }
    exit
}


#--------- antique
#text .text -relief raised -bd 2 \
#    -yscrollcommand ".scroll set"
#scrollbar .scroll -command ".text yview"
#pack .scroll -side right -fill y
#pack .text   -side left
#.text insert end "Starting...\n"
#after $TIMER_INTERVAL ngetsGUIShowJobStatus

#proc ngetsGUIShowJobStatus {} {
#    global NGETS_JOBS TIMER_INTERVAL
#
#    after $TIMER_INTERVAL ngetsGUIShowJobStatus
#    
#    if {![info exists NGETS_JOBS]} { return }
#
#    .text delete 0.0 end
#    .text insert end "Current time: [date]"
#    foreach job [lsort [array names NGETS_JOBS]] {
#	.text insert end "$job   $NGETS_JOBS($job)\n"
#    }
#}


# --------------------------------------------------------
#
#  Initializing
#
# --------------------------------------------------------
if {$argc == 1 && [lindex $argv 0] == "-server"} {
    ngetsGUIStart
}
