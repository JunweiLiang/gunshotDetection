# ========================================================================
#  JanusRTk   Janus Speech Recognition Toolkit
#             ------------------------------------------------------------
#             Object: Documentation Skeleton File
#             ------------------------------------------------------------
#
#  Author  :  Florian Metze
#  Module  :  lib2tex.tcl
#  Date    :  $Id: lib2tex.tcl 2390 2003-08-14 11:20:32Z fuegen $
#
#  Remarks :  This procedure will generate a skeleton *.tex file from
#             the online help information available in tcl-lib procedures
#
# ------------------------------------------------------------------------
#
# $Log$
# Revision 1.2  2003/08/14 11:18:56  fuegen
# Merged changes on branch jtk-01-01-15-fms (jaguar -> ibis-013)
#
# Revision 1.1.2.7  2003/06/26 15:08:39  metze
# Initial changes for V5.0 P013
#
# Revision 1.1.2.6  2002/11/05 12:36:09  metze
# Cleaner lib2tex, glossary
#
# Revision 1.1.2.5  2002/11/04 14:57:31  metze
# Semi-automatic tcl-lib documentation
#
# Revision 1.1.2.4  2002/10/25 14:54:28  metze
# Not too much output
#
# Revision 1.1.2.3  2002/10/11 08:09:09  metze
# Bugfix for incoherent output format
#
# Revision 1.1.2.2  2002/10/11 07:36:51  metze
# Removed some output
#
# Revision 1.1.2.1  2002/10/11 07:35:59  metze
# Initial revision
#
#
# ========================================================================

proc putsTeX { fp txt } {
    global allTypes

    regsub -all "_"         $txt "\\\\_" Txt
    regsub -all "#"         $Txt "\\\\#" Txt
    regsub -all "&"         $Txt "\\\\&" Txt
    regsub -all "%"         $Txt "\\\\%" Txt
    regsub -all "\\$"       $Txt "\\\\$" Txt
    regsub -all "\\^"       $Txt "\\\\^" Txt
    regsub -all "<"         $Txt "\$<\$" Txt
    regsub -all ">"         $Txt "\$>\$" Txt
    regsub -all "\\\\\""    $Txt "''"    Txt
    regsub -all " xYx "     $Txt " \\& " Txt

    if {[regexp "\\((\[A-Za-z\]*):\[^)\]*\\)" $Txt d o] && [lsearch $allTypes $o] >= 0} {
	regsub "\\((\[A-Za-z\]*):\[^)\]*\\)" $Txt "(\\Jref{module}{$o})" Txt
    }
    regsub -all "\\(\[^)\]*:\[^)\]*\\)" $Txt ""      Txt

    puts $fp $Txt
}

foreach f "[glob ../tcl-lib/*.tcl] [glob ../gui-tcl/*.tcl]" {
    regsub ".tcl" [file tail $f] "_Info" i
    if {![catch {set p [$i procs]}]} {
	lappend procs($f) $p
	lappend files     $f
	set full([file tail $f]) $f
    }
}


set allTypes [types]
set typesL [string tolower $allTypes]

set fp [open lib-auto.tex w]

putsTeX $fp "\\textbf{WARNING:} These entries were generated automatically."
putsTeX $fp "This list is non-exhaustive, but it includes all 'user-level' functions.\n"
# putsTeX $fp "Due to restrictions of the \LaTeX typesetting system,"
#putsTeX $fp "you may find that in some tokens you'll have to replace"
#putsTeX $fp "'-' by '\_' to reach a function, procedure or file name."
#putsTeX $fp "Sorry for any inconvenience.\n"
putsTeX $fp "The argument LSID is the 'local system ID'. It is usually"
putsTeX $fp "given by the variable SID. For example,"
putsTeX $fp "'bbiInit' would usually be called as \\texttt{bbiInit \$SID}"
putsTeX $fp "or \\texttt{bbiInit \$SID -desc bbi.desc -param bbi.param.gz}."

foreach name [lsort [array names full]] {
    set file $full($name)

    puts "\n ++++++++ $file ++++++++\n"

    regsub -all "_" [file tail $file] "-"    key
    regsub -all ".*/tcl-lib" $file "tcl-lib" path
    regsub -all ".*/gui-tcl" $path "gui-tcl" path
    regsub      ".tcl" [file tail $file] "_Info" i

    putsTeX $fp "\n\\section{\\Jlabel{lib}{$key}}\n"
    putsTeX $fp "[$i help]\\\\\n\nProcedures defined in \\texttt{$path}:"

    foreach p [lsort [lindex $procs($file) 0]] {

	catch { unset itfUsageInfo }
	catch { unset argL }
	regsub -all "_" $p "-" q

	switch $p {
	    printDo {
		set argL "{ args         The commands to execute}"
	    }
	    waitFile {
		set argL "{ <dir>        file or file list} \
                          { -sleep       ms to wait between tries} \
                          { -after       ms to wait after file was found}"
	    }
	    foreachSegment {
		set argL "{ <utt>        return value: variable to contain the utterance} \
                          { <dbase>      the database you use} \
                          { <spk>        the speaker} \
                          { <body>       the script to execute}"
	    }
	    default {
		foreach k [info args $p] {
		    puts "$p :: $k"
		    switch $k {
			LSID {
			    lappend argL "LSID          The system id, usually \$SID."
			}
			args {
			    regsub -all "args" [info args $p] "-help" r
			    catch { eval $p $r }
			    if {![info exists itfUsageInfo]} { set itfUsageInfo "{ARGS          ???}" }
			    foreach j $itfUsageInfo { lappend argL $j } 
			}
			default {
			    lappend argL "$k            ..."
			}
		    }
		}
	    }
	}
	
	putsTeX $fp "\n  \\subsection{\\Jlabel{proc}{$q}}\n"
	putsTeX $fp "    [$i $p]\\\\\n\n    \\textbf{Arguments:}\n\n"
	putsTeX $fp "    \\begin{tabular}{ll}"

	foreach arg $argL {
	    set n [string trim [string range $arg 0 13]]
	    set e [string range $arg 14 end]

	    putsTeX $fp "      \\Jlabel{$q}{$n} xYx $e \\\\"
	}
	putsTeX $fp "    \\end{tabular}"
    }
}

close $fp

puts ""
puts "----------------------------------------------------------------"
puts "  You'll find a newly generated lib-auto.tex"
puts "  Run tcl2tex.tcl and re-create the docu."
puts "----------------------------------------------------------------"
puts ""

exit
