# ========================================================================
#  JANUS-SR   Janus Speech Recognition Toolkit
#             ------------------------------------------------------------
#             Object: Scoring Script (Main Procedure)
#             ------------------------------------------------------------
#
#  Author  :  Martin Westphal
#  Module  :  scoreProc.tcl
#  Date    :  $Id: scoreProc.tcl 2560 2004-10-27 12:12:43Z metze $
#
#  Remarks :  This script was known as 'scoreAll.tcl' most of the time
#             We moved it to tcl-lib to allow for some kind of
#             version control for it
#
# ========================================================================
#
#  $Log$
#  Revision 1.3  2004/10/27 12:12:43  metze
#  Fixed alin program
#
#  Revision 1.2  2003/08/14 11:20:30  fuegen
#  Merged changes on branch jtk-01-01-15-fms (jaguar -> ibis-013)
#
#  Revision 1.1.2.7  2003/08/12 09:45:20  soltau
#  fixes
#
#  Revision 1.1.2.6  2003/06/12 15:55:02  metze
#  Scoring for Informedia added
#
#  Revision 1.1.2.5  2003/02/02 12:35:51  metze
#  Different string matches
#
#  Revision 1.1.2.4  2002/10/17 12:34:12  metze
#  Give up for empty lists
#
#  Revision 1.1.2.3  2002/10/11 08:12:51  metze
#  Bugfix for itfParseArgv
#
#  Revision 1.1.2.2  2001/10/17 15:42:34  metze
#  Updated configuration options
#
#  Revision 1.1.2.1  2001/10/04 18:12:12  metze
#  Added scoring library funtions
#
#
# ========================================================================

proc puts2 {string} {
  global f_out
  puts $string
  if {"$f_out" != ""} { puts $f_out $string}
}


proc puts2_nonewline {string} {
  global f_out
  puts  -nonewline $string
  if {"$f_out" != ""} { puts  -nonewline $f_out $string }
}


# ---------------------
#  print lz-lp matrix
# ---------------------
proc print_lzlp {compact} {
  global redUttListN uttListN lplist lzlist perf

    if {$compact} { 
	set t "\nWordAccuracy"
    } else {
	set t "\nWA(DEL,INS)" 
    }
  if {$redUttListN < $uttListN} { 
     puts2 "$t !!! for $redUttListN of $uttListN utterances !!!"
  } else {
      puts2 "$t"
  }
  if {$compact} {
    puts2 "======================= COMPACT lz-lp:   WA  ===================="
    puts2 "lz\\lp\t [join $lplist "\t "]"
  } else {
    puts2 "======================= FULL lz-lp:      WA(DEL,INS) ===================="
    puts2 "lz\\lp\t [join $lplist "\t \t "]"
  }
  foreach lz $lzlist {
    puts2_nonewline "$lz\t"
    foreach lp $lplist {
      if {![info exists perf($lz,$lp)]} {
	  if {$compact} { set x "  ~ \t" } else { set x "  ~        \t" }
      } else {
        regexp {(.*)%\((.*)%,(.*)%\)} $perf($lz,$lp) dummy wa del ins
        if {$compact} {
          set x [format "%4.1f\t" $wa]
        } else {
          set x [format "%4.1f(%3.1f|%3.1f)\t" $wa $del $ins]
        }
      }
      puts2_nonewline "$x"
    }
    puts2 ""
  }
}


proc scoreProc { args } {
    global map_mode unk_mode spell_mode tilde_mode case_mode
    global f_out hypoA refA env score_verbose
    global redUttListN uttListN lplist lzlist perf

set uttFile       ""
set hypoFiles     "H_*"
set matchFile     "match.log"
set correctFile   "correct.log"
if {![info exists spell_mode]} { set spell_mode  "1" }
if {![info exists   unk_mode]} { set unk_mode    "0" }
if {![info exists   map_mode]} { set map_mode    "1" }
if {![info exists tilde_mode]} { set tilde_mode  "0" }
if {![info exists  case_mode]} { set case_mode   "0" }
set dbase         ""
set TEXT          TEXT
set filterFile    /project/MT/X1/score-lib/noiseFilter.tcl
set alignProg     align
set cfile         ""    
set logfile	  "tmp.result"
set keepAlign	  ""
set score_verbose 0

if [catch {itfParseArgv scoreProc $args [list [
  list <uttList>       string  {} uttFile        {} {file with utterances}                       ] [
  list -hypo           string  {} hypoFiles      {} {name of hypo file(s)}                       ] [
  list -match          string  {} matchFile      {} {name of match file to write}                ] [
  list -correct        string  {} correctFile    {} {name of correct file to write}              ] [
  list -filter         string  {} filterFile     {} {tcl file that defines the proc noiseFilter} ] [
  list -dbase          string  {} dbase          {} {data base}                                  ] [
  list -fieldname      string  {} TEXT           {} {fieldname of reference text}                ] [
  list -spell          string  {} spell_mode     {} {1=map to <spell>, 0=indiv. letters}         ] [
  list -unk            string  {} unk_mode       {} {0=no action, 1=map Herr x -> Herr <UNK>}    ] [
  list -map            string  {} map_mode       {} {0=no action, 1=map unknown words to <UNK>}  ] [
  list -tilde          string  {} tilde_mode     {} {0=no action}                                ] [
  list -case           string  {} case_mode      {} {0=no action}                                ] [
  list -log            string  {} logfile        {} {write matrix/statistics into this file}     ] [
  list -keepalign      string  {} keepAlign      {} {Keep all align files, use this as prefix}   ] [
  list -alignprog      string  {} alignProg      {} {align program to execute}                   ] [
  list -verbose        string  {} score_verbose  {} {verbosity}                                  ] [
  list -cfile          string  {} cfile          {} {annotated correct file}               ] ] } ] {
      return
  }

source $filterFile

# ------------------------------------------------------------------------

# ---------------------
#  get file list 
# ---------------------
set fileList [getFilenames $hypoFiles]
puts "found hypo files: $fileList"

# ---------------------
#  find lzs and lps
# ---------------------
set lzlist [findlzs $fileList]
set lplist [findlps $fileList]
puts "lz: $lzlist  lp: $lplist"

# ---------------------
#  read utterance list 
# ---------------------
set fp [ open $uttFile r ]
set uttList [read $fp]
close $fp
puts "[llength $uttList] utterances in list"


# ------------------------
#  reduced utterance list 
# ------------------------
regexp {_z([-.0-9]*)_p([-.0-9]*)} [lindex $fileList 0] dummy lz lp
readHypoFiles [matchlzlp $lz $lp $fileList] $uttList;# result in hypoA
set redUttList ""
foreach utt $uttList {
    if [info exists hypoA($utt)] {lappend redUttList $utt}
}

set redUttListN [llength $redUttList]
set uttListN    [llength $uttList]

if {!$redUttListN} {
   puts stderr "Nothing to score. Giving up."
   return 0
}

if {$redUttListN < $uttListN} {
   puts stderr "#----------- WARNING ------------------------------------#"
   puts stderr "Found only $redUttListN of $uttListN utterances."
   puts stderr "Will score the reduced utterance list."
   puts stderr "Reduced correct file in ${correctFile}.reduced"
   puts stderr "Reduced utterance list in uttlist.reduced"
   # ------------------------------------
   # produce a list of missing utterances
   # ------------------------------------
   set mistList {}         ;# list of missed utts (= mist-utts)
   foreach u $uttList {
     if {[lsearch -exact $redUttList $u] == -1} {  
        lappend mistList $u
     }
   }
   set mistUtts [join $mistList]
   puts stderr "MISSING UTTERANCES: $mistUtts"
   puts stderr "#--------------------------------------------------------#"
      
   readCorrectFile $uttList -file $correctFile

   set correctFile "${correctFile}.reduced"
   set uttList     "$redUttList"

   writeCorrectFile $uttList -file $correctFile
   set FP [open uttlist.reduced w]
   foreach utt $uttList {puts $FP $utt}
   close $FP
}


# ---------------------
#  loop over lz lp
# ---------------------

set BEST_WA -1e+99

foreach lz $lzlist {
  foreach lp $lplist {
    if {![llength [set f [matchlzlp $lz $lp $fileList]]]} { continue }
    puts -nonewline "\nslz ${lz}, slp ${lp}, "
    makeArray res [alignUttFiles $uttList $f        \
                                 -prog $alignProg   \
                                 -ref  $correctFile \
                                 -match $matchFile  ]
    set perf($lz,$lp) "$res(WA)($res(DEL),$res(INS))"
    puts "\t$perf($lz,$lp)"
    if {"$keepAlign" != ""} {
      if {$BEST_WA < $res(WA)} {
        set BEST_WA $res(WA)
        set name "${keepAlign}"
        file copy -force tmp.out $name
      }
    }
  }
}


if {"$logfile" != ""} {
  set f_out [open $logfile "w"]
}


print_lzlp 0
print_lzlp 1


# -----------------------------------------
#              statistic 
# -----------------------------------------
set mean    0.0
set smean   0.0
set count   0
set max     0.0
set min   100.0
set minP    ""
set maxP    ""
set list    ""
foreach lz $lzlist {
  foreach lp $lplist {
    if {![info exists perf($lz,$lp)]} { continue }
    regexp {(.*)%\((.*)%,(.*)%\)} $perf($lz,$lp) dummy wa del ins
    lappend list $wa
    incr count
    set mean [expr $mean + $wa]
    set smean [expr $smean + $wa * $wa]
    if {$wa <= $min} {set min $wa; set minP "lz= $lz, lp= $lp"}
    if {$wa >= $max} {set max $wa; set maxP "lz= $lz, lp= $lp"}
  }
}

set mean  [expr $mean/$count]
set smean [expr $smean/$count]
set var   [expr $smean - $mean * $mean]
if { $var < 0.0 } { set var 0.0 }
set dev   [expr sqrt($var)]
set list  [lsort -real $list]
set n1    [expr int(round(1.0 * ($count - 1.0) / 4.0))]
set n2    [expr int(round(2.0 * ($count - 1.0) / 4.0))]
set n3    [expr int(round(3.0 * ($count - 1.0) / 4.0))]
set q1    [lindex $list $n1]
set q2    [lindex $list $n2]
set q3    [lindex $list $n3]
puts2 "\n============ statistic ====================="
puts2 "Parameters:       "
puts2 "    <uttlist> = $uttFile"
foreach l {{hypo hypoFiles} {match matchFile} {correct correctFile} \
	       {filter filterFile} {spell spell_mode} {unk unk_mode} \
	       {map map_mode} {tilde tilde_mode} {case case_mode} \
	       {cfile cfile} {log logfile}} {
    set v [set [lindex $l 1]]
    if {$v != "" && $v != 0} {
	puts2 [format "%13s = %s" "-[lindex $l 0]" $v]
    }
    
}
puts2 ""
puts2 " lz_list:  $lzlist"
puts2 " lp_list:  $lplist"
puts2 ""

# number as given in align log
set fp [open tmp.out] 
while {[gets $fp line] >=0} {
    if [string match "sentences*" $line] {
	set sentN [lindex $line 1]
	break
    }
}
close $fp

if {$redUttListN < $uttListN} { 
    puts2 "  sentN : $sentN $redUttListN   !!! of $uttListN utterances !!!"
} else {
    puts2 "  sentN : $sentN $redUttListN"
}
puts2 "  min   : $min  at $minP"
puts2 "  qlow  : $q1"
puts2 "  median: $q2              mean: $mean"
puts2 "  qup   : $q3"
puts2 "  max   : $max  at $maxP"
puts2 "  var   : $var         dev : $dev" 
puts2 "============================================"

close $f_out

}




