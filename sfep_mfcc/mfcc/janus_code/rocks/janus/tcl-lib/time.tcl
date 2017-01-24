#------------------------------------------------------------------------------
# misc Routines
#------------------------------------------------------------------------------

if {![info exists ModulStatus(Timing)]} {
    set ModulStatus(Timing) 0
}

proc TIME { mode what {id -1}} {
    global ModulStatus
    global TimeA
    
    if {$ModulStatus(Timing) == 0} {
	return
    }

    set uTime [usertime -total 1]
    set sTime [systemtime -total 1]
    set TimeA($what,$mode,uTime,$id) $uTime
    set TimeA($what,$mode,sTime,$id) $sTime
    puts "$mode \t$what $id ($uTime, $sTime)"


    switch $mode {
	
	START {
	}

	STOP {
	    if {![info exist TimeA($what,uTime)]} {
		set TimeA($what,uTime) 0.0
		set TimeA($what,sTime) 0.0
	    }
	    set TimeA($what,uTime) [expr   $TimeA($what,uTime)\
					+ (  $TimeA($what,STOP,uTime,$id)\
					   - $TimeA($what,START,uTime,$id))
				   ]
	    set TimeA($what,sTime) [expr   $TimeA($what,sTime)\
					+ (  $TimeA($what,STOP,sTime,$id)\
					   - $TimeA($what,START,sTime,$id))
				   ]
	}

	default {
	    ERROR 0 "TIME: unknown mode ($mode)"
	    exit
	}
    }
}

proc saveTIME {filename} {
    global ModulStatus
    global TimeA
    
    if {$ModulStatus(Timing) == 0} {
	return
    } 
    
    set    fd [open $filename w]
    puts  $fd [array get TimeA]
    close $fd
}


proc printTime {} {

    global ModulStatus
    global TimeA
    
    if {$ModulStatus(Timing) == 0} {
	return
    } 
    
    set whatL [array names TimeA *,uTime]
    puts "whatL $whatL"
    regsub -all {,uTime} $whatL "" whatL
    puts "whatL $whatL"

    foreach w $whatL {
	printWhatTIME $w
    }
}


proc printWhatTIME {what} {
    global TimeA

    set uTime     $TimeA($what,uTime)
    set sTime     $TimeA($what,sTime)
    set TotalTime [expr $uTime + $sTime]
    
    puts "$what : $TotalTime : $uTime : $sTime"
}

