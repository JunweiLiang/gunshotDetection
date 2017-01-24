###############################################################################
#                                                                             #
# This is a collection of error rate measuring tools.                         #
# There are four functions for aligning correct and hypothesised data:        #
#                                                                             #
#     rawAlign      will return the alignment path for one sentence           #
#        align      will return the error summary for one sentence            #
#     rawAlignFile  will return the error summary for an entire file          #
#        alignFile  will not return anything but print to stdout like NIST's  #
#                                                                             #
# The purpose of implementing this in Tcl is to have a tool that will allow   #
# us to build a 1-button-Janus which will be able to tune itself on a given   #
# development (or crossvalidation) test set, using the recognition error rate #
# as the driving objective function.                                          #
#                                                                             #
# There are always two kind's of errors, namely error occurrences (every sub- #
# stitution and every deletion and every insertion is exactly 1 error), and   #
# there are error-penalties which can be defined by the user in three arrays: #
# e.g.:                                                                       #
#           set sub(A,B)     0                                                #
#           set sub(A,C)   500                                                #
#           set sub(B,D)   100                                                #
#           set sub(B,A)  2000                                                #
#                                                                             #
# will define an array for substition penalties. You can regard the values as #
# 'milli-errors', i.e. substituting A by B will cost nothing, substiting A by #
# C will cost half an error, substituting B by D will cost a tenth of a reg-  #
# ular error, and substituting B by A will cost two full errors. You can take #
# care of homophones by setting sub(homophone1,homophone2) 0. The name of the #
# array can be any, as long as it is defined on the top stack level of the    #
# Tcl interpreter.                                                            #
#                                                                             #
#           set del(A)       0                                                #
#           set del(B)     500                                                #
#                                                                             #
# will define an array for deletion penalties, where deleting an A will not   #
# be punished, and deleting a B will cost half an error.                      #
#                                                                             #
#           set ins(A)    2000                                                #
#           set ins(B)     100                                                #
#                                                                             #
# will define an array for insertion penalties, where inserting an A will be  #
# punished with two errors, and inserting a B will cost 0.1 errors.           #
#                                                                             #
# Admittedly, the procedures defined here are kinda slow (well it's Viterbi   #
# in Tcl) but the time for recognizing a sentence should usually be much      #
# greater. A file is being aligned with a speed of 2 seconds per sentence on  #
# an HP 735.                                                                  #
#                                                                           IR#
###############################################################################

proc align_Info { name } {
    switch $name {
	procs {
	    return "align"
	}
	help {
	    return "This is a collection of error rate measuring tools.
There are four functions for aligning correct and hypothesised data:

\\begin{verbatim}
     rawAlign      will return the alignment path for one sentence
        align      will return the error summary for one sentence
     rawAlignFile  will return the error summary for an entire file
        alignFile  will not return anything but print to stdout like NIST's
\\end{verbatim}

The purpose of implementing this in Tcl is to have a tool that will allow
us to build a 1-button-Janus which will be able to tune itself on a given
development (or crossvalidation) test set, using the recognition error rate
as the driving objective function."
	}
	align {
	    return "This function will return the error summary for one sentence.

\\begin{verbatim}
align      co hy \[-sub subP\] \[-ins insP\] \[-del delP\]

   co    = string containing correct sentence (rawAlign, align) or
           file of many correct sentences (alignFiles, rawAlignFiles)
   hy    = string containing the hypothesis (rawAlign, align) or
           file of many hypotheses (alignFiles, rawAlignFiles)
   subP  = two-dimensional array such that subP(w1,w2) is the
           substitution penalty for substituting word w1 by word w2
   insP  = one-dimensional array such that insP(w1) is the
           insertion penalty for inserting the word w1
   delP  = one-dimensional array such that delP(w1) is the
           deletion penalty for deleting the word w1
\\end{verbatim}

There is also an external scoring program, which runs significantly faster."
	}
    }
    return "???"
}


proc rawAlign args {

  # ------------------ #
  # parse command line #
  # ------------------ #
  set argN [llength $args]; 

  if { $argN < 2 || [expr $argN % 2] == 1 || $argN > 8 } { alignUsage; return "" }

  set co [lindex $args 0]
  set hy [lindex $args 1]
  set subp("") 0;  set insp("") 0;  set delp("") 0;
  for { set x 2 } { $x < $argN } { incr x } {
    if [string match [lindex $args $x] "-sub"] { incr x; unset subp; upvar \#0 [lindex $args $x] subp }
    if [string match [lindex $args $x] "-ins"] { incr x; unset insp; upvar \#0 [lindex $args $x] insp }
    if [string match [lindex $args $x] "-del"] { incr x; unset delp; upvar \#0 [lindex $args $x] delp }
  }
  set hyN [llength $hy]
  set coN [llength $co]

  # ---------------------------------------- #
  # compute full substitution penalty matrix #
  # ---------------------------------------- #
  set hyX 0
  foreach hyW $hy {
    set coX 0
    foreach coW $co {
      if [info exists subp($coW,$hyW)] { set sub($hyX,$coX) $subp($coW,$hyW)
      } else { set sub($hyX,$coX) [expr 1000 - 1000 * [string match $coW $hyW]] }
      incr coX
    }
    incr hyX
  }
  # ----------------------------------- #
  # compute full insertion penalty list #
  # ----------------------------------- #
  set hyX 0
  foreach hyW $hy {
    if [info exists insp($hyW)] { set ins($hyX) $insp($hyW) } else { set ins($hyX) 1000 }
    incr hyX
  }
  # ---------------------------------- #
  # compute full deletion penalty list #
  # ---------------------------------- #
  set coX 0
  foreach coW $co {
    if [info exists delp($coW)] { set del($coX) $delp($coW) } else { set del($coX) 1000 }
    incr coX
  }
  # --------------------------------------- #
  # initialize full cumulative socre matrix #
  # --------------------------------------- #
  for { set hyX 0 } { $hyX < $hyN } { incr hyX } {
    for { set coX 0 } { $coX < $coN } { incr coX } {
      set cum($hyX,$coX) 1000000 ;# == infty
    }
  }
  set cum(-1,-1) 0
  for { set hyX 0 } { $hyX < $hyN } { incr hyX } { set cum($hyX,-1) [expr $cum([expr $hyX-1],-1) + $ins($hyX)]; set bp($hyX,-1) 0 }
  for { set coX 0 } { $coX < $coN } { incr coX } { set cum(-1,$coX) [expr $cum(-1,[expr $coX-1]) + $del($coX)]; set bp(-1,$coX) 2 }

  # ---------------- #
  # do the alignment #
  # ---------------- #
  set hyX 0
  foreach hyW $hy {
    set coX 0
    foreach coW $co {

      set newscore [expr $cum([expr $hyX-1],$coX) + $ins($hyX)]                                 ;# try transition 0: insertion
      if { $newscore < $cum($hyX,$coX) } { set bp($hyX,$coX) 0; set cum($hyX,$coX) $newscore; }

      set newscore [expr $cum([expr $hyX-1],[expr $coX-1]) + $sub($hyX,$coX)]                   ;# try transition 1: substitution
      if { $newscore < $cum($hyX,$coX) } { set bp($hyX,$coX) 1; set cum($hyX,$coX) $newscore; }

      set newscore [expr $cum($hyX,[expr $coX-1]) + $del($coX)]                                 ;# try transition 2: deletion
      if { $newscore < $cum($hyX,$coX) } { set bp($hyX,$coX) 2; set cum($hyX,$coX) $newscore; }

      incr coX
    }
    incr hyX 
  }

  # ---------------- #
  # do the backtrace #
  # ---------------- #

  set trace ""
  set cost  ""

  set hyX [expr $hyN-1]
  set coX [expr $coN-1]
 
  while { $hyX >= 0 || $coX >= 0 } {
    set trans $bp($hyX,$coX)
    set trace [linsert $trace 0 $trans]
    set cost  [linsert $cost  0 $cum($hyX,$coX)]
    if { $trans == 0 || $trans == 1 } { set hyX [expr $hyX-1] }
    if { $trans == 2 || $trans == 1 } { set coX [expr $coX-1] }
  }

  return [list "$trace" "$cost"]
}

proc alignUsage { } {

  puts "Usage: rawAlign      co hy \[-sub subP\] \[-ins insP\] \[-del delP\]"
  puts "   or:    align      co hy \[-sub subP\] \[-ins insP\] \[-del delP\]"
  puts "   or: rawAlignFiles co hy \[-sub subP\] \[-ins insP\] \[-del delP\]"
  puts "   or:    alignFiles co hy \[-sub subP\] \[-ins insP\] \[-del delP\]"
  puts ""
  puts "  co    = string containing correct sentence (rawAlign, align) or"
  puts "          file of many correct sentences (alignFiles, rawAlignFiles)"
  puts "  hy    = string containing the hypothesis (rawAlign, align) or"
  puts "          file of many hypotheses (alignFiles, rawAlignFiles)"
  puts "  subP  = two-dimensional array such that subP(w1,w2) is the"
  puts "          substitution penalty for substituting word w1 by word w2"
  puts "  insP  = one-dimensional array such that insP(w1) is the"
  puts "          insertion penalty for inserting the word w1"
  puts "  delP  = one-dimensional array such that delP(w1) is the"
  puts "          deletion penalty for deleting the word w1"
  puts ""
  puts "  Alignment is always case sensitive. If no -sub or -ins or -del"
  puts "  arguments are given, each error will get the same penalty (1000)."
  puts "  So you can define penalties as small a one 'milli-error'." 
  puts "  NOTE: the subP, insP and delP arrays must be visible globally"
  puts "        because align doesn't know what stack level it resides in,"
  puts "        and we don't want to pass the array throu all stack levels."
  puts ""
  puts "  The return value of rawAlign:"
  puts "  -----------------------------"
  puts "  is a list of two lists, the first of which contains every alignment"
  puts "  step (0=insertion, 1=substitution/correct, 2=deletion), and the"
  puts "  second list contains the accumulated cost after each step."
  puts ""
  puts "  The return value of align:"
  puts "  --------------------------"
  puts "  is a list of 8 elments with the following contents:"
  puts "  index 0: the alignment string for the correct sentence"
  puts "  index 1: the alignment string for the hypothesised sentence"
  puts "  index 2: a list of all substition errors"
  puts "  index 3: a list of all insertion errors" 
  puts "  index 4: a list of all deletions errors"
  puts "  index 5: a list of all substition penalties"
  puts "  index 6: a list of all insertion penalties" 
  puts "  index 7: a list of all deletion penalties"
  puts "  doing a puts of the two alignment strings will produce the same kind"
  puts "  of output as the NIST-align program. Every penalty-list entry will"
  puts "  correspond to the same-indext entry of the corresponding error list."
  puts "  Every entry of the insertion/deletion error list is a name of the"
  puts "  inserted/deleted word. Every entry of the substitution error list is"
  puts "  a two-element list containing the correct and the hypothesised word."
  puts ""
  puts "  The return value of rawAlignFile:"
  puts "  ---------------------------------"
  puts "  is a list of 6 elements with the following contents:"
  puts "  index 0: number of substition errors"
  puts "  index 1: number of insertion errors"
  puts "  index 2: number of deletion errors"
  puts "  index 3: cumulated substituion penalties"
  puts "  index 4: cumulated insertion penalties"
  puts "  index 5: cumulated deletion penalties"
  puts ""
  puts "  The return value of alignFile:"
  puts "  ------------------------------"
  puts "  is empty, instead output similar to the one from the NIST-align"
  puts "  program is written to standard output."
  puts ""
}

proc align { args } {

  set spaces "                                                                "
  set stars  "****************************************************************"

  if {[catch {itfParseArgv align $args [ list [ 
      list "<corr>" string {} co   {} "correct sentence (reference)" ] [
      list "<hypo>" string {} hy   {} "recognizer output (hypothesis)" ] [
      list "-sub"   string {} dum1 {} "2D array of substitution penalties" ] [
      list "-ins"   string {} dum2 {} "1D array of insertion penalties" ] [
      list "-del"   string {} dum3 {} "1D array of deletion penalties" ] ]
  } ] } {
    return
  }

  #set co [lindex $args 0]
  #set hy [lindex $args 1]

  set argN [llength $args]; 

  set alignOutput [eval rawAlign $args]

  set path      [lindex $alignOutput 0]
  set penalties [lindex $alignOutput 1]

  set coStr ""  ;# will contain the resulting manipulation of the correct string
  set hyStr ""  ;# will contain the resulting manipulation of the hypothesis string
  set sub   ""  ;# will contain a list of all substitution pairs
  set ins   ""  ;# will contain a list of all inserted words
  set del   ""  ;# will contain a list of all deleted words

  set hyX 0
  set coX 0
  set stepX 0
  set oldPen 0
  set subPen ""
  set insPen ""
  set delPen ""

  foreach step $path { 

    set thisPen [lindex $penalties $stepX]
    set pen [expr $thisPen - $oldPen]
    set oldPen $thisPen

    set hyW [lindex $hy $hyX]; set hyL [string length $hyW]
    set coW [lindex $co $coX]; set coL [string length $coW]

    if { $step == 1 } { 
      if { $pen != 0 } {
        set hyStr "$hyStr [changeCase $hyW]" 
        set coStr "$coStr [changeCase $coW]"
      } else {
        set hyStr "$hyStr $hyW"
        set coStr "$coStr $coW"
      }
      if { $hyW != $coW || $pen != 0 } { lappend sub [list $coW $hyW]; lappend subPen $pen }
      if { $hyL >  $coL } { set coStr "${coStr}[string range $spaces 1 [expr $hyL - $coL]]" }
      if { $hyL <  $coL } { set hyStr "${hyStr}[string range $spaces 1 [expr $coL - $hyL]]" }
      incr hyX; incr coX;
    }
    if { $step == 0 } { 
      set hyStr "$hyStr [changeCase $hyW]"
      set coStr "$coStr [string range $stars 1 $hyL]"
      lappend ins $hyW
      lappend insPen $pen
      incr hyX
    }
    if { $step == 2 } { 
      set coStr "$coStr [changeCase $coW]"
      set hyStr "$hyStr [string range $stars 1 $coL]"
      lappend del $coW
      lappend delPen $pen
      incr coX
    }
    incr stepX
  }
  return [list $coStr $hyStr $sub $ins $del $subPen $insPen $delPen]
}

proc changeCase str {

  set l [string length $str]
  set result ""
  for { set x 0 } { $x < $l } { incr x } {
    set char [string index $str $x]
    set nchar $char
    if [regexp {[a-z]} $char] { set nchar [string toupper $char] }
    if [regexp {[A-Z]} $char] { set nchar [string tolower $char] }
    set result "${result}${nchar}"
  } 
  return $result
}

proc put2s { s1 s2 } {
  puts $s1
  puts $s2
}


proc rawAlignFiles args {

  set coFile [lindex $args 0]
  set hyFile [lindex $args 1]
  set argN   [llength $args]

  if { [catch { set coFp [open $coFile r] } res] } { puts "can't read from $coFile";              return $res }
  if { [catch { set hyFp [open $hyFile r] } res] } { puts "can't read from $hyFile"; close $coFp; return $res }

  # --------------------------------------------- #
  # read the entire correct file into coLine(...) #
  # --------------------------------------------- #

  set lineCnt 0
  while { [gets $coFp line] >= 0 } {
    set parOpen  [string first "(" $line]
    set parClose [string first ")" $line]
    if { $parOpen != -1 && $parClose > $parOpen } { 
      set uttID [string range $line [expr $parOpen+1] [expr $parClose-1]] 
      set line  [string range $line 0                 [expr $parOpen-1]]
    } else { 
      sett uttID $lineCnt 
    }
    set coLine($uttID) $line
    incr lineCnt
  }

  # ------------------------------------------------ #
  # read the entire hypotheses file into coLine(...) #
  # ------------------------------------------------ #

  set lineCnt 0
  while { [gets $hyFp line] >= 0 } {
    set parOpen  [string first "(" $line]
    set parClose [string first ")" $line]
    if { $parOpen != -1 && $parClose > $parOpen } { 
      set uttID [string range $line [expr $parOpen+1] [expr $parClose-1]] 
      set line  [string range $line 0                 [expr $parOpen-1]]
    } else { 
      sett uttID $lineCnt 
    }
    set hyLine($uttID) $line
    incr lineCnt
  }

  # -------------------------------------------------------- #
  # align every hypothesis for which there is a correct line #
  # -------------------------------------------------------- #

  set sumSubN 0; set sumInsN 0; set sumDelN 0;
  set sumSubP 0; set sumInsP 0; set sumDelP 0;
  foreach utt [array names hyLine] {
    if [ info exists coLine($utt) ]  {
       set commandArgs "{ $coLine($utt) } { $hyLine($utt) } [lrange $args 2 [expr $argN-1]]"
       set alignOut [ eval align $commandArgs ]
       set subN [llength [lindex $alignOut 2]] ; set sumSubN [expr $sumSubN + $subN]
       set insN [llength [lindex $alignOut 3]] ; set sumInsN [expr $sumInsN + $insN]
       set delN [llength [lindex $alignOut 4]] ; set sumDelN [expr $sumDelN + $delN]
       set subP [listAdd [lindex $alignOut 5]] ; set sumSubP [expr $sumSubP + $subP]
       set insP [listAdd [lindex $alignOut 6]] ; set sumInsP [expr $sumInsP + $insP]
       set delP [listAdd [lindex $alignOut 7]] ; set sumDelP [expr $sumDelP + $delP]
    }
  }
  return [list $sumSubN $sumInsN $sumDelN $sumSubP $sumInsP $sumDelP]
}

proc listAdd l {
  set sum 0
  foreach i $l { set sum [expr $sum + $i] }
  return $sum
}


proc alignFiles args {

  set coFile [lindex $args 0]
  set hyFile [lindex $args 1]
  set argN   [llength $args]

  if { [catch { set coFp [open $coFile r] } res] } { puts "can't read from $coFile";              return $res }
  if { [catch { set hyFp [open $hyFile r] } res] } { puts "can't read from $hyFile"; close $coFp; return $res }

  # --------------------------------------------- #
  # read the entire correct file into coLine(...) #
  # --------------------------------------------- #

  set lineCnt 0
  while { [gets $coFp line] >= 0 } {
    set parOpen  [string first "(" $line]
    set parClose [string first ")" $line]
    if { $parOpen != -1 && $parClose > $parOpen } { 
      set uttID [string range $line [expr $parOpen+1] [expr $parClose-1]] 
      set line  [string range $line 0                 [expr $parOpen-1]]
    } else { 
      sett uttID $lineCnt 
    }
    set coLine($uttID) $line
    incr lineCnt
  }

  # ------------------------------------------------ #
  # read the entire hypotheses file into coLine(...) #
  # ------------------------------------------------ #

  set lineCnt 0
  while { [gets $hyFp line] >= 0 } {
    set parOpen  [string first "(" $line]
    set parClose [string first ")" $line]
    if { $parOpen != -1 && $parClose > $parOpen } { 
      set uttID [string range $line [expr $parOpen+1] [expr $parClose-1]] 
      set line  [string range $line 0                 [expr $parOpen-1]]
    } else { 
      sett uttID $lineCnt 
    }
    set hyLine($uttID) $line
    incr lineCnt
  }

  # -------------------------------------------------------- #
  # align every hypothesis for which there is a correct line #
  # -------------------------------------------------------- #

  set coWordN 0; set hyWordN 0; 
  set sumSubN 0; set sumInsN 0; set sumDelN 0; set sumErrN 0; 
  set sumSubP 0; set sumInsP 0; set sumDelP 0; set sumErrP 0; 
  foreach utt [array names hyLine] {
    if [ info exists coLine($utt) ]  {
       set wordN [llength $coLine($utt)]
       set coWordN [expr $coWordN + $wordN]
       set hyWordN [expr $hyWordN + [llength $hyLine($utt)]]
       set commandArgs "{ $coLine($utt) } { $hyLine($utt) } [lrange $args 2 [expr $argN-1]]"
       set alignOut [ eval align $commandArgs ]

       set subN [llength [lindex $alignOut 2]] ; set sumSubN [expr $sumSubN + $subN]
       set insN [llength [lindex $alignOut 3]] ; set sumInsN [expr $sumInsN + $insN]
       set delN [llength [lindex $alignOut 4]] ; set sumDelN [expr $sumDelN + $delN]

       set subP [listAdd [lindex $alignOut 5]] ; set sumSubP [expr $sumSubP + $subP]
       set insP [listAdd [lindex $alignOut 6]] ; set sumInsP [expr $sumInsP + $insP]
       set delP [listAdd [lindex $alignOut 7]] ; set sumDelP [expr $sumDelP + $delP]

       set errP [expr $subP + $insP + $delP] ; set sumErrP [expr $sumErrP + $errP]
       set errN [expr $subN + $insN + $delN] ; set sumErrN [expr $sumErrN + $errN]

       set errPR [expr $errP /  10.0 / $wordN] ;# error rate in % using 1000 penalties == 1 error
       set errNR [expr $errN * 100.0 / $wordN] ;# error rate in % using 1 error = 1 error

       puts ""; puts "------------------------- $utt -----------------------"
       put2s [lindex $alignOut 0] [lindex $alignOut 1]
       puts -nonewline "$errN error(s) ($errP), "
       puts -nonewline "$subN substitution(s) ($subP), "
       puts -nonewline "$insN insertion(s) ($insP), "
       puts -nonewline "$delN deletion(s) ($delP), "
       puts -nonewline "${errNR}% error rate (${errPR}%)"
       puts ""

       # ---------------------------------------- #
       # accumulated statistics for substitutions #
       # ---------------------------------------- #

       foreach sub [lindex $alignOut 2] {
         set subCo [lindex $sub 0]
         set subHy [lindex $sub 1]
	 if [info exists subs($subCo,$subHy)] {
           incr subs($subCo,$subHy)
         } else {
           set subs($subCo,$subHy) 1
         }
       }
       # ------------------------------------- #
       # accumulated statistics for insertions #
       # ------------------------------------- #

       foreach ins [lindex $alignOut 3] {
	 if [info exists inss($ins)] { incr inss($ins) } else { set inss($ins) 1 }
       }
       # ------------------------------------ #
       # accumulated statistics for deletions #
       # ------------------------------------ #

       foreach del [lindex $alignOut 4] {
	 if [info exists dels($del)] { incr dels($del) } else { set dels($del) 1 }
       }
    }
  }

  set sumSubNR [expr $sumSubN * 100.0 / $coWordN]
  set sumInsNR [expr $sumInsN * 100.0 / $coWordN]
  set sumDelNR [expr $sumDelN * 100.0 / $coWordN]
  set sumErrNR [expr $sumErrN * 100.0 / $coWordN]

  set sumSubPR [expr $sumSubP /  10.0 / $coWordN]
  set sumInsPR [expr $sumInsP /  10.0 / $coWordN]
  set sumDelPR [expr $sumDelP /  10.0 / $coWordN]
  set sumErrPR [expr $sumErrP /  10.0 / $coWordN]

  puts ""
  puts "==================== SUMMARY ==================="
  puts ""
  puts "words in correct sentences:   $coWordN"
  puts "words in hypotheses:          $hyWordN"
  puts ""
  puts "Substitutions:                $sumSubN ($sumSubP)"
  puts "Insertions:                   $sumInsN ($sumInsP)"
  puts "Deletions:                    $sumDelN ($sumDelP)"
  puts "Errors:                       $sumErrN ($sumErrP)"
  puts "Substitution Rate:            ${sumSubNR}% (${sumSubPR}%)"
  puts "Insertions:                   ${sumInsNR}% (${sumInsPR}%)"
  puts "Deletions:                    ${sumDelNR}% (${sumDelPR}%)"
  puts "Errors:                       ${sumErrNR}% (${sumErrPR}%)"
  puts ""
  puts " ------------- substitutions ------------------"
  parray subs
  puts ""
  puts " ------------- insertions ------------------"
  parray inss
  puts ""
  puts " ------------- deletions ------------------"
  parray dels

  close $coFp
  close $hyFp
}
