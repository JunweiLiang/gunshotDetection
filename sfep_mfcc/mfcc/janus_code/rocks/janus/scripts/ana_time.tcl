# ========================================================================
#  JANUS-SR   Janus Speech Recognition Toolkit
#             ------------------------------------------------------------
#             Object: Timing Analysis
#             ------------------------------------------------------------
#
#  Author  :  Hagen Soltau
#  Module  :  ana_time.tcl
#  Date    :  $Id: ana_time.tcl 2567 2004-11-09 16:55:01Z metze $
#
#  Remarks :  GSST system, VM II, timing analysis
#
# ========================================================================
#
#  $Log$
#  Revision 1.1  2004/11/09 16:55:01  metze
#  Timing analysis for IslSystem
#
#
# ========================================================================

set uttFile /project/njd/IslData/scoring/utts
set glob    {time.[0-9]*}
set seconds 5319.16

if [catch {itfParseArgv $argv0 $argv [list [
  list <uttFile>  string  {} uttFile  {} {file with utterances}          ] [
  list <seconds>  string  {} seconds  {} {duration of set (seconds)}     ] [
  list -glob      string  {} glob     {} {annotated correct file}        ] ] } ] {
    exit 1
}

# ----------------------------------------------------------------------

proc ReadTime { {glob {time.[0-9]*}} } {
    global time
    set flist [glob $glob]

    puts stderr "ReadTime: $flist"

    foreach f $flist {
	set fp [open $f]
	array set time [read $fp]
	close $fp
    }
    foreach entry [array names time] {
	set mode [lindex [split $entry ','] 0]
	set modeA($mode) 1
    }
    return [lsort -ascii [array names modeA]]
}

proc AnaTime {mode uttlist seconds} {
    global time

    set cnt 0.0
    foreach utt $uttlist {
	if [info exists time($mode,STOP,uTime,$utt)] {
	    set cnt [expr $cnt + $time($mode,STOP,uTime,$utt) - $time($mode,START,uTime,$utt)]
	}
    }

    set rtf [expr $cnt / $seconds]
    puts "RTF $mode \t: $rtf"
}

set modeL   [ReadTime $glob]
set uttlist [split [exec cat $uttFile] '\n']

puts stderr "$uttFile -> [llength $uttlist] utterances"
puts stderr "duration \t= ${seconds}s"


foreach mode $modeL {
    AnaTime $mode $uttlist $seconds
}

exit
