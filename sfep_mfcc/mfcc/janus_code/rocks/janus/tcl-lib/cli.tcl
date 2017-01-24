# ========================================================================
#  JANUS-SR   Janus Speech Recognition Toolkit
#             ------------------------------------------------------------
#             Object: cli.tcl - shortcuts for command line interface
#                     replaces former C-implemented calls
#             ------------------------------------------------------------
#
#  Author  :  Florian Metze and Hagen Soltau
#  Module  :  cli.tcl
#  Date    :  $Id: cli.tcl 2425 2003-09-09 08:14:48Z metze $
#
#  Remarks :  It is unneccessary to implement "rm" in C just to
#             call a second Tcl interpreter to do the work
#
#  Tcl_CreateCommand ( interp, "rm",         rmItf,          0, 0 );
#  Tcl_CreateCommand ( interp, "mv",         mvItf,          0, 0 );
#  Tcl_CreateCommand ( interp, "cp",         cpItf,          0, 0 );
#  Tcl_CreateCommand ( interp, "cd",         cdItf,          0, 0 );
#  Tcl_CreateCommand ( interp, "sleep",      sleepItf,       0, 0 );
#  Tcl_CreateCommand ( interp, "wait",       waitItf,        0, 0 );
#  Tcl_CreateCommand ( interp, "mkdir",      mkdirItf,       0, 0 );
#  Tcl_CreateCommand ( interp, "rmdir",      rmdirItf,       0, 0 );
#  Tcl_CreateCommand ( interp, "touch",      touchItf,       0, 0 );
#  Tcl_CreateCommand ( interp, "date",       dateItf,        0, 0 );
#
# ========================================================================
#
#  $Log$
#  Revision 1.4  2003/09/09 08:14:48  metze
#  Code moved here from itf.c
#
#  Revision 1.3  2003/08/18 11:50:51  metze
#  Cleanup of 'rm...' problem
#
#  Revision 1.2  2003/08/18 10:11:32  metze
#  Moved 'rm mv cp mkdir rmdir touch date sleep wait' into tcl-lib
#
#  Revision 1.1  2003/08/15 15:32:02  metze
#  *** empty log message ***
#
# ========================================================================

proc cli_Info { name } {
    switch $name {
        procs {
            return "rm mv cp mkdir rmdir touch sleep wait"
        }
        help {
            return "Procedures to provide backward compatibility for commands included to reduce the need for forks. Usage is not exactly the same as the standard Unix commands."
        }
	rm { return "Removes files" }
	mv { return "Moves files" }
	cp { return "Copies files" }
	rmdir { return "Removes directories" }
	mkdir { return "Creates directories" }
	touch { return "Touches files" }
	sleep { return "Sleeps." }
	wait  { return "Waits a while." }
    }
}

proc rm { args } {
    set force 0

    itfParseArgv rm $args [list [
        list <file> string  {} gexpr    {} {file name(s) (glob expression)} ] [
        list -f     int     {} force    {} {0 return on error, 1 continue}  ] ]

    if {$force} {
	return [file delete -force -- [glob -nocomplain $gexpr]]
    } else {
	return [file delete        -- [glob             $gexpr]]
    }
}

proc mv { args } {
    set force 0

    itfParseArgv mv $args [list [
        list <from> string  {} gexpr    {} {file name(s) (glob expression)} ] [
        list <to>   string  {} target   {} {target (directory)}             ] [
        list -f     int     {} force    {} {0 return on error, 1 continue}  ] ]

    if {$force} {
	catch  {eval file rename -force -- [glob -nocomplain $gexpr] $target}
    } else {
	return [eval file rename        -- [glob             $gexpr] $target]
    }
}

proc cp { args } {
    set force 1

    itfParseArgv cp $args [list [
        list <from> string  {} gexpr    {} {file name(s) (glob expression)} ] [
        list <to>   string  {} target   {} {target (directory)}             ] [
        list -f     int     {} force    {} {0 return on error, 1 continue}  ] ]

    if {$force} {
	catch  {eval file copy -force -- [glob -nocomplain $gexpr] $target}
    } else {
	return [eval file copy        -- [glob             $gexpr] $target]
    }
}

proc rmdir { args } {
    set force 0

    itfParseArgv rmdir $args [list [
        list <dir>  string  {} gexpr    {} {directory(ies) (glob expression)} ] [
        list -f     int     {} force    {} {0 return on error, 1 continue}  ] ]

    if {$force} {
	return [file delete -force -- [glob -nocomplain $gexpr]]
    } else {
	return [file delete        -- [glob             $gexpr]]
    }
}

proc mkdir { args } {
    set force 0

    itfParseArgv mkdir $args [list [
        list <dir>  string  {} file     {} {directory(ies)}                   ] [
        list -f     int     {} force    {} {0 return on error, 1 continue}    ] ]

    if {$force} {
	catch  {eval file mkdir $file}
    } else {
	return [eval file mkdir $file]
    }
}

proc touch { args } {
    itfParseArgv touch $args [list [
        list <files> string  {} files    {} {file name(s) (no glob)} ] ]

    foreach f $files {
	set fp [open $f a]
	close $fp
    }
}

proc date { } {
    return [clock format [clock seconds]]
}

proc sleep { args } {
    itfParseArgv sleep $args [list [
        list <sec> string  {} sec    {} {sleep <sec> seconds} ] ]

    after [expr $sec*1000]
}

proc wait { args } {
    set iv 1
    set mt -1

    itfParseArgv wait $args [list [
        list <file>     string  {} file {} {name of file to wait for}  ] [
	list -intervall int     {} iv   {} {poll every n seconds}      ] [
        list -maxtime   int     {} mt   {} {wait no longer than (sec)} ] ]
 
    if {$mt >= 0 && $iv >= 0} {set mt [expr $mt/$iv]}
    set iv [expr $iv*1000]

    for {set ct 0} {$ct != $mt && ![file exists $file]} {incr ct} {
	after $iv
    }
}
