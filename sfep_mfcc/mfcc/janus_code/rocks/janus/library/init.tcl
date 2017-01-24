#  ========================================================================
#   JRTk       Janus Recognition Toolkit
#              ------------------------------------------------------------
#              Object: Init Functions
#              ------------------------------------------------------------
#
#   Author  :  Michael Finke & Ivica Rogina
#   Module  :  init.tcl
#   Date    :  $Id: init.tcl 3384 2011-03-01 20:52:11Z metze $
#   Remarks :
#
#   $Log$
#   Revision 1.6  2007/02/23 10:02:00  fuegen
#   modified inclusion of rcFile from Florian Metze/ Friedrich Faubel
#
#   Revision 1.5  2004/07/23 16:57:49  fuegen
#   added proc for adding all tcl-defined methods to objects
#
#   Revision 1.4  2003/08/14 11:19:42  fuegen
#   Merged changes on branch jtk-01-01-15-fms (jaguar -> ibis-013)
#
#   Revision 1.3.52.1  2002/10/11 07:34:18  metze
#   Added itfUsageInfo variable
#
#   Revision 1.3  1999/12/09 09:15:40  soltau
#   source ~/.janusrc
#
#   Revision 1.2  1997/10/21 13:00:32  rogina
#   sabotage?, hm, a line was missing that would produce the name of the
#   violated flag on false input, I've added that line (set name $arg).
#
#   Revision 1.1  1997/07/15 10:14:21  westphal
#   Initial revision
#
#
#  ========================================================================

#  ========================================================================
#   itfParseArg    Argument Parsing 
#  ======================================================================== 

proc itfPrintUsage { method descr } {
  global itfUsageInfo

  if { [llength $descr] == 0 } { return 0 }
  puts "Parameters of '$method' are:"

  catch {unset itfUsageInfo}
  foreach item $descr {

    set name [lindex $item 0]
    set type [lindex $item 1]
    set var  [lindex $item 3]
    set otyp [lindex $item 4]
    set help [lindex $item 5]

    if { $type == "object"  } { set type $otyp }
    if { $type == "lindex"  } { set type "index in $otyp" }
    if { $type == "liarray" } { set type "index array in $otyp" }

    upvar 2 $var defVal
    if { [info exists defVal] == 0 } { set defVal "" }

    puts                 [format " %-12s %s (%s:%s)" $name $help $type $defVal]
    lappend itfUsageInfo [format " %-12s %s (%s:%s)" $name $help $type $defVal]
  }
}

proc itfExpect { what for got } {
  puts "expected $what argument for $for, but got \"$got\""
  error ""
}

proc itfParseArgv { method argv descr } {

  makeArray ar $descr
  set fixIndex 0

  while { [llength $argv] > 0 } {

    set arg  [lindex $argv 0]
    set argv [lrange $argv 1 end]

    if { $arg == "-help" } { itfPrintUsage $method $descr ; error "" }

    if { $fixIndex >= 0 } {

       set name [lindex [lindex $descr $fixIndex] 0]
       if { [string index $name 0] == "-" } { set fixIndex -1 
       } else { set item $ar($name) ; incr fixIndex }
    }

    if { $fixIndex == -1 } {

       if [catch { set item $ar($arg) }] { 
          puts "undefined option: $arg" 
          itfPrintUsage $method $descr
	  error ""
       } 
       set name $arg
       set arg  [lindex $argv 0]
       set argv [lrange $argv 1 end]
    }

    set type [lindex $item 0]
    set cust [lindex $item 1]
    set var  [lindex $item 2]
    set otyp [lindex $item 3]
    set help [lindex $item 4]

    upvar 1 $var resVar

    switch -- $type {
      string  { set resVar $arg }
      float   { if [catch { set x [expr 1.0 * $arg] }] { return [itfExpect float $name $arg] } else { set resVar $x } }
      int     { if [catch { set x [expr 1   * $arg] }] { return [itfExpect int   $name $arg] } else { set resVar $x } }
      char    { if { [string length $arg] != 1 }       { return [itfExpect char  $name $arg] } else { set resVar $arg } }
      object  { if { [catch { set x [$arg  type] }] || $x != $otyp } { return [itfExpect $otyp $name $arg] } else { set resVar $arg } }
      lindex  { if { [catch { set x [$otyp type] }] || $x != "List" } { puts "$otyp is not a list" ; error "" } else { set resVar [$otyp index $arg] } }
      liarray { if { [catch { set x [$otyp type] }] || $x != "List" } { puts "$otyp is not a list" ; error "" } else { set resVar [$otyp index $arg] } }
      iarray  { set ret "" 
                foreach y $arg { if [catch { set x [expr 1 * $y] }] { return [itfExpect iarray $name $arg] } else { lappend ret $x } }
		set resVar $ret 
              }
    }
  }

  if { $fixIndex >= 0  && $fixIndex < [llength $descr]} {
     set name [lindex [lindex $descr $fixIndex] 0]
      if { [string index $name 0] != "-" } { 
            itfPrintUsage $method $descr ; error "" }
  }
}

# Check for the correct version of the initialization
if {![info exists tcl_version] || $tcl_version < 8.5} { 
# This is to be used for Tcl 8.4

# ----------------------------------------------------------------------
# itfInit
# this procedure adds all TCL-DEFINED METHODS to the objects
# ----------------------------------------------------------------------
proc itfInit { } {
    global auto_path

#    auto_reset

    foreach path $auto_path {
	set files [glob -nocomplain "$path/*.tcl"]
	foreach file $files {
	    set fp [open $file r]
	    while { [gets $fp line] >= 0 } {
		if { [regexp {^[^ ]+ +method +[^ ]+ +[^ ]+} $line] } {
		    while { [regexp {\\ *$} $line] && ![eof $fp] } {
			set line [string trimright $line "\\ "]
			set line "$line [gets $fp]"
		    }
		    if { [catch {uplevel $line} msg] } {
			error "$file: $msg\n$line"
		    }
		}
	    }
	    close $fp
	}
    }
}

# End Tcl 8.4
} else {
# This to be used for Tcl 8.5

# Florian Metze: it seems we must
# 1) source the system-wide default init.tcl
# 2) Add some JRTk specific code to the 'unknown' procedure,
#    so we re-define it
# the new code uses some fancy flags for catch, so it doesn't run under 8.4

# system-wide --
foreach path $auto_path {
    if {[file exists [file join $path init.tcl]]} {
	source [file join $path init.tcl]
	break
    }
}

# unknown --
# This procedure is called when a Tcl command is invoked that doesn't
# exist in the interpreter.  It takes the following steps to make the
# command available:
#
#	1. See if the command has the form "namespace inscope ns cmd" and
#	   if so, concatenate its arguments onto the end and evaluate it.
#	2. See if the autoload facility can locate the command in a
#	   Tcl script file.  If so, load it and execute it.
#	3. If the command was invoked interactively at top-level:
#	    (a) see if the command exists as an executable UNIX program.
#		If so, "exec" the command.
#	    (b) see if the command requests csh-like history substitution
#		in one of the common forms !!, !<number>, or ^old^new.  If
#		so, emulate csh's history substitution.
#	    (c) see if the command is a unique abbreviation for another
#		command.  If so, invoke the command.
#
# Arguments:
# args -	A list whose elements are the words of the original
#		command, including the command name.

proc unknown args {
    variable ::tcl::UnknownPending
    global auto_noexec auto_noload env tcl_interactive

    # If the command word has the form "namespace inscope ns cmd"
    # then concatenate its arguments onto the end and evaluate it.

    set cmd [lindex $args 0]

    if {[regexp "\." $cmd]} {
      regsub -all {\.} $cmd " . " cmd
    }
    if {[regexp "\:(?!\:)" $cmd]} {
      if {![regexp "^tix.*:.*" $cmd]} {
        if {![regexp "\:\:" $cmd]} {
          regsub -all {\:} $cmd " : " cmd
        }
      }
    }
    if { $cmd != [lindex $args 0]} {
      set cmd "$cmd [lrange $args 1 end]"
      return [eval $cmd]
    }
    

    if {[regexp "^:*namespace\[ \t\n\]+inscope" $cmd] && [llength $cmd] == 4} {
	#return -code error "You need an {*}"

        set arglist [lrange $args 1 end]
	set ret [catch {uplevel 1 ::$cmd $arglist} result opts]
	dict unset opts -errorinfo
	dict incr opts -level
	return -options $opts $result
    }

    catch {set savedErrorInfo $::errorInfo}
    catch {set savedErrorCode $::errorCode}
    set name $cmd
    if {![info exists auto_noload]} {
	#
	# Make sure we're not trying to load the same proc twice.
	#
	if {[info exists UnknownPending($name)]} {
	    return -code error "self-referential recursion\
		    in \"unknown\" for command \"$name\"";
	}
	set UnknownPending($name) pending;
	set ret [catch {
		auto_load $name [uplevel 1 {::namespace current}]
	} msg opts]
	unset UnknownPending($name);
	if {$ret != 0} {
	    dict append opts -errorinfo "\n    (autoloading \"$name\")"
	    return -options $opts $msg
	}
	if {![array size UnknownPending]} {
	    unset UnknownPending
	}
	if {$msg} {
	    catch {set ::errorCode $savedErrorCode}
	    catch {set ::errorInfo $savedErrorInfo}
	    set code [catch {uplevel 1 $args} msg opts]
	    if {$code ==  1} {
		#
		# Compute stack trace contribution from the [uplevel].
		# Note the dependence on how Tcl_AddErrorInfo, etc. 
		# construct the stack trace.
		#
		set errorInfo [dict get $opts -errorinfo]
		set errorCode [dict get $opts -errorcode]
		set cinfo $args
		if {[string bytelength $cinfo] > 150} {
		    set cinfo [string range $cinfo 0 150]
		    while {[string bytelength $cinfo] > 150} {
			set cinfo [string range $cinfo 0 end-1]
		    }
		    append cinfo ...
		}
		append cinfo "\"\n    (\"uplevel\" body line 1)"
		append cinfo "\n    invoked from within"
		append cinfo "\n\"uplevel 1 \$args\""
		#
		# Try each possible form of the stack trace
		# and trim the extra contribution from the matching case
		#
		set expect "$msg\n    while executing\n\"$cinfo"
		if {$errorInfo eq $expect} {
		    #
		    # The stack has only the eval from the expanded command
		    # Do not generate any stack trace here.
		    #
		    dict unset opts -errorinfo
		    dict incr opts -level
		    return -options $opts $msg
		}
		#
		# Stack trace is nested, trim off just the contribution
		# from the extra "eval" of $args due to the "catch" above.
		#
		set expect "\n    invoked from within\n\"$cinfo"
		set exlen [string length $expect]
		set eilen [string length $errorInfo]
		set i [expr {$eilen - $exlen - 1}]
		set einfo [string range $errorInfo 0 $i]
		#
		# For now verify that $errorInfo consists of what we are about
		# to return plus what we expected to trim off.
		#
		if {$errorInfo ne "$einfo$expect"} {
		    error "Tcl bug: unexpected stack trace in \"unknown\"" {} \
			[list CORE UNKNOWN BADTRACE $einfo $expect $errorInfo]
		}
		return -code error -errorcode $errorCode \
			-errorinfo $einfo $msg
	    } else {
		dict incr opts -level
		return -options $opts $msg
	    }
	}
    }

    if {([info level] == 1) && ([info script] eq "") \
	    && [info exists tcl_interactive] && $tcl_interactive} {
	if {![info exists auto_noexec]} {
	    set new [auto_execok $name]
	    if {$new ne ""} {
		set redir ""
		if {[namespace which -command console] eq ""} {
		    set redir ">&@stdout <@stdin"
		}
		uplevel 1 [list ::catch \
			[concat exec $redir $new [lrange $args 1 end]] \
			::tcl::UnknownResult ::tcl::UnknownOptions]
		dict incr ::tcl::UnknownOptions -level
		return -options $::tcl::UnknownOptions $::tcl::UnknownResult
	    }
	}
	if {$name eq "!!"} {
	    set newcmd [history event]
	} elseif {[regexp {^!(.+)$} $name -> event]} {
	    set newcmd [history event $event]
	} elseif {[regexp {^\^([^^]*)\^([^^]*)\^?$} $name -> old new]} {
	    set newcmd [history event -1]
	    catch {regsub -all -- $old $newcmd $new newcmd}
	}
	if {[info exists newcmd]} {
	    tclLog $newcmd
	    history change $newcmd 0
	    uplevel 1 [list ::catch $newcmd \
		    ::tcl::UnknownResult ::tcl::UnknownOptions]
	    dict incr ::tcl::UnknownOptions -level
	    return -options $::tcl::UnknownOptions $::tcl::UnknownResult
	}

	set ret [catch {set candidates [info commands $name*]} msg]
	if {$name eq "::"} {
	    set name ""
	}
	if {$ret != 0} {
	    dict append opts -errorinfo \
		    "\n    (expanding command prefix \"$name\" in unknown)"
	    return -options $opts $msg
	}
	# Filter out bogus matches when $name contained
	# a glob-special char [Bug 946952]
	if {$name eq ""} {
	    # Handle empty $name separately due to strangeness
	    # in [string first] (See RFE 1243354)
	    set cmds $candidates
	} else {
	    set cmds [list]
	    foreach x $candidates {
		if {[string first $name $x] == 0} {
		    lappend cmds $x
		}
	    }
	}
	if {[llength $cmds] == 1} {
	    uplevel 1 [list ::catch [lreplace $args 0 0 [lindex $cmds 0]] \
		    ::tcl::UnknownResult ::tcl::UnknownOptions]
	    dict incr ::tcl::UnknownOptions -level
	    return -options $::tcl::UnknownOptions $::tcl::UnknownResult
	}
	if {[llength $cmds]} {
	    return -code error "ambiguous command name \"$name\": [lsort $cmds]"
	}
    }
    return -code error "invalid command name \"$name\""
}

# End Tcl 8.5
}

###########################################################
# source rcFileName
###########################################################

if {! $tcl_interactive} {
    set rcFile [file join $env(HOME) .janusrc]
    if {! [file exists ${rcFile}]} {
	error "$file: expected to find janusrc file at $rcFile"
    } else {
	if [catch {source ${rcFile} } msg] {
	    puts stderr "ERROR at source ${rcFile} :\n $msg \n"
	}
    }
}
