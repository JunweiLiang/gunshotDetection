# ========================================================================
#  JANUS-SR   Janus Speech Recognition Toolkit
#             ------------------------------------------------------------
#             Object: Scoring Script (Library)
#             ------------------------------------------------------------
#
#  Author  :  Martin Westphal
#  Module  :  scoreLib.tcl
#  Date    :  $Id: scoreLib.tcl 2390 2003-08-14 11:20:32Z fuegen $
#
#  Remarks :  The scoreLib was added to the tcl-lib in order to
#             allow for some common version controling
#
# ========================================================================
#
#  $Log$
#  Revision 1.2  2003/08/14 11:20:30  fuegen
#  Merged changes on branch jtk-01-01-15-fms (jaguar -> ibis-013)
#
#  Revision 1.1.2.4  2003/08/12 09:45:06  soltau
#  fixes
#
#  Revision 1.1.2.3  2003/06/12 15:55:14  metze
#  Scoring for Informedia
#
#  Revision 1.1.2.2  2003/02/02 12:35:51  metze
#  Different string matches
#
#  Revision 1.1.2.1  2001/10/04 18:12:12  metze
#  Added scoring library funtions
#
#  Revision 1.1  1999/12/17 08:21:56  metze
#  Initial revision
#
# Revision 1.8  1997/08/29  13:44:39  westphal
# corrected to work with missing utterances
#
# Revision 1.7  1997/04/30  11:37:07  westphal
# VMeval97 path
#
# Revision 1.6  1996/09/22  14:40:29  sloboda
# the script now automatically checks the machine type the program is
# running on and uses an align program which was compiled for this
# architecture, using the path /home/sst/VMeval97/scoring/bin/$machType
#
#  Revision 1.5  1996/09/12 10:52:29  westphal
#  writeMatchFile writes also hypofiles
#
#
# ========================================================================

# ------------------------------------------------------------------------
# getFilenames
#
# Get a list of filenames matching the expressions in 'fileMatchList' 
# ------------------------------------------------------------------------

proc getFilenames {fileMatchList} {
   set fileList ""
   foreach fileMatch "$fileMatchList" {
      if [catch {set fileList "$fileList [glob $fileMatch]"}] {
         puts stderr "no files found matching $fileMatch"
      }
   }
   return $fileList
}


# ------------------------------------------------------------------------
# findlzs
# findlps
# ------------------------------------------------------------------------

proc findlzs {fileList} {
    set lzList {}
    foreach elem $fileList {
	if [regexp {z([-0123456789\.]+)_p} $elem dummy lz] {
	    if {[lsearch $lzList $lz] == -1} {
		lappend lzList $lz
	    }
	}
    }
    if { [catch {set rc [lsort -real $lzList] } ] == 1} {
	set rc [lsort -ascii $lzList]
    }
    return $rc
}

proc findlps {fileList} {
    set lpList {}
    foreach elem $fileList {
	if [regexp {_p([-0123456789\.]+)} $elem dummy lp] {
	    if {[lsearch $lpList $lp] == -1} {
		lappend lpList $lp
	    }
	}
    }
    if { [catch {set rc [lsort -real $lpList] } ] == 1} {
	set rc [lsort -ascii $lpList]
    }
    return $rc
}


# ------------------------------------------------------------------------
# matchlzlp
# Returns the sublist of files that match the given lz and lp
# ------------------------------------------------------------------------

proc matchlzlp {lz lp fileList} {
   set zpFiles ""
   foreach file $fileList {
      if [string match "*z${lz}_p${lp}_*" "$file"] {
        lappend zpFiles $file
      }
   }
   return $zpFiles
}


# ------------------------------------------------------------------------
# readHypoFiles
#
# Parse the hypofiles given in 'fileList' and put the hypos for the 
# utterances listed in 'uttList' into the global array 'hypoA'.
# ------------------------------------------------------------------------

proc readHypoFiles {fileList uttList} {
   global hypoA

   set count     0
   set notInList 0
   foreach file "$fileList" {
#      puts "reading $file"
       set fp [ open $file r ]
       set didbreak 0

       while { [ gets $fp turn ] != -1 } {
          while {$turn == ""} {if {[gets $fp turn] == -1} {set didbreak 1;break}}
          if $didbreak break
          incr count
          if {[llength $turn] != 2} {
             puts stderr "ERROR more than 2 elements in TURN line, $count: $turn"
             return -1
          }
          set turn1 [lindex $turn 0]
          if { "$turn1" != "%TURN:"} {
             puts stderr "ERROR TURN line is wrong: $turn"; return -1
          }
          set utt   [lindex $turn 1]
          if {[gets $fp hypo] == -1} {
             puts stderr "ERROR reading hypo $count"; return -1
          }
          if {[gets $fp empty] == -1} {
             puts stderr "ERROR reading empty $count"; return -1
          }
          if {$empty != ""} {
             puts stderr "ERROR empty $count not empty but $empty"; return -1
          }
          if {[lsearch $uttList $utt] < 0} {
	      # puts stderr "Couldn't find hypo $utt in utterance list!"
	      incr notInList
          } else {
	      set hypoA($utt) $hypo 
	  }
       }
       close $fp
   }
   puts "$count hypos found"
   set countOK [expr $count - $notInList]
   if {$notInList > 0} {
      puts "$notInList not in utterance list"
      puts "$countOK utterances ok"
   }
   set missing [expr [llength $uttList] - $countOK ]
   puts "$missing utterances missing"
   return "$missing"
}


# ------------------------------------------------------------------------
# writeMatchFile
# ------------------------------------------------------------------------

proc writeMatchFile {args} {
   global hypoA

   set filename "match.log"
   set hformat 0
   set filter  1
   if [catch {itfParseArgv writeMatchFile $args [list [
      list <uttList>   string  {} uttList  {} {tcl list of utterances}   ] [
      list -file       string  {} filename {} {name of match file}       ] [
      list -hformat    int     {} hformat  {} {write as hypo file}       ] [
      list -filter     int     {} filter   {} {use noise filter before writing }       ] ]
   }] {
      return -1 }

   set fp [ open $filename w ]
   foreach utt "$uttList" {
      if [info exists hypoA($utt)] {
	 if {$hformat} {puts $fp "%TURN: $utt"}
	 if {$filter} {
	     puts $fp [noiseFilter $hypoA($utt)]
	 } else {
	     puts $fp "$hypoA($utt)"
	 }
	 if {$hformat} {puts $fp ""}
      } else {
         puts stderr "ERROR no hypo for $utt"
      }
   }
   close $fp
}


# ------------------------------------------------------------------------
# readCorrectFromDBase
# Define the procedure 'noiseFilter' first.
# ------------------------------------------------------------------------

proc readCorrectFromDBase {args} {
   global refA env

   set uttList     ""
   set dbase       ""
   set TEXT        TEXT
   if [catch {itfParseArgv readCorrectFromDBase $args [list [
      list <uttList>  string  {} uttList {} {tcl list of utterances}     ] [
      list <dbase>    string  {} dbase   {} {data base file name}        ] [
      list -fieldname string  {} TEXT    {} {fieldname of reference text}]]}] {
      return -1 }

   DBase db
   db open $dbase.dat $dbase.idx -mode r
   foreach utt "$uttList" {
      makeArray loc [db get $utt]
      if [info exists loc($TEXT)] {
	  set refA($utt) [noiseFilter $loc($TEXT)]
      } else {
         puts stderr "ERROR no field $TEXT for $utt in data base"
      }
   }
   db destroy
}


# ------------------------------------------------------------------------
# readCorrectFile
# ------------------------------------------------------------------------

proc readCorrectFile {args} {
   global refA

   set filename "correct.log"
   if [catch {itfParseArgv readCorrectFile $args [list [
      list <uttList>   string  {} uttList  {} {tcl list of utterances}   ] [
      list -file       string  {} filename {} {name of correct file}     ] ]}] {
      return -1 }

   set fp [ open $filename r ]
   foreach utt "$uttList" {
      if {[gets $fp refA($utt)] == -1} {
         puts stderr "ERROR no more references for $utt"
      }
   }
   close $fp
}


# ------------------------------------------------------------------------
# writeCorrectFile
# ------------------------------------------------------------------------

proc writeCorrectFile {args} {
   global refA

   set tokenmode 0
   set filename  "correct.log"
   if [catch {itfParseArgv writeCorrectFile $args [list [
      list <uttList>   string  {} uttList   {} {tcl list of utterances}   ] [
      list -file       string  {} filename  {} {name of correct file}     ] [
      list -token      string  {} tokenmode {} {print out token as well}] ]}] {
      return -1 }

   set fp [ open $filename w ]
   foreach utt "$uttList" {
      if [info exists refA($utt)] {
	 if $tokenmode {
	    # mit Tokenname, menschliche Auswertung:
            puts $fp "$utt: $refA($utt)"
         } else {
	    # ohne Tokenname, maschinelle Auswertung
            puts $fp $refA($utt)
	 }
      } else {
         puts stderr "ERROR no reference for $utt"
      }
   }
   close $fp
}


# ------------------------------------------------------------------------
# getMachineType
# ------------------------------------------------------------------------

proc getMachineType {} {
    global tcl_platform
    return $tcl_platform(os)
}

# ------------------------------------------------------------------------
# alignUtts
#
# Make sure that there is a 'match.log' and a 'correct.log' file. You can
# use: 
#   writeMatchFile   $uttList
#   writeCorrectFile $uttList
# ------------------------------------------------------------------------

proc alignUtts {args} {
    global score_verbose

    set machType [getMachineType]
    set prog     "/home/sst/VMeval97/scoring/bin/$machType/align"
    set hypFile  "match.log"
    set refFile  "correct.log"
    set filename "tmp.out"
    if [catch {itfParseArgv alignUtts $args [list [
        list <uttList>   string  {} uttList  {} {tcl list of utterances}   ] [
        list -prog       string  {} prog     {} {align program}            ] [
        list -hyp        string  {} hypFile  {} {hypothesis file}          ] [
        list -ref        string  {} refFile  {} {reference file}           ] [
        list -file       string  {} filename {} {name of tmp file}       ] ]}] {
        return -1 }

    if {$score_verbose} {
	puts "*** exec $prog -hyp $hypFile -def $refFile > $filename ***"
    }
    if {[catch {exec $prog -hyp $hypFile -def $refFile > $filename} msg] && $score_verbose} {
	puts "Scoring result:\n$msg"
    }

    set wa  "0.00%"
    set ins "0.00%"
    set del "0.00%"

    set fp [open $filename] 
    while {[gets $fp line] >= 0} {
	if [string match "WORD ACCURACY*" $line] { set wa  [lindex $line end] }
	if [string match "Insertions*"    $line] { set ins [lindex $line 2]   }
	if [string match "Deletions*"     $line] { set del [lindex $line 2]   }
    }
    close $fp
    return "{WA $wa} {INS $ins} {DEL $del}"
}


# ------------------------------------------------------------------------
# alignUttFiles
#
# Make sure that there is a 'correct.log' file. You can use: 
#   writeCorrectFile $uttList
# ------------------------------------------------------------------------

proc alignUttFiles {args} {
   set machType [getMachineType]
   set prog      "/home/sst/VMeval97/scoring/bin/$machType/align"
   set matchFile "match.log"
   set refFile   "correct.log"
   set filename  "tmp.out"
   if [catch {itfParseArgv alignUttFiles $args [list [
      list <uttList>   string  {} uttList   {} {tcl list of utterances}   ] [
      list <hypoFiles> string  {} fileList  {} {list of hypo files}       ] [
      list -prog       string  {} prog      {} {align program}            ] [
      list -match      string  {} matchFile {} {name of match file to write} ] [
      list -ref        string  {} refFile   {} {reference file to use}    ] [
      list -file       string  {} filename  {} {name of tmp file}       ] ]}] {
      return -1 }

   readHypoFiles $fileList $uttList
   writeMatchFile $uttList -file $matchFile
   return [alignUtts $uttList -prog $prog -hyp $matchFile -ref $refFile]
}


# ------------------------------------------------------------------------
# return a list of
# {speakerID {utt1 utt2 ... }}
# ------------------------------------------------------------------------

proc getSpkUttList {args} {
   set uttList ""
   set dbname  ""
   set spk SPEAKER

   if [catch {itfParseArgv alignUtts $args [list [
      list <uttList> string {} uttList {} {tcl list of utterances}          ] [
      list -dbase    string {} dbname  {} {utterance dbase with speaker IDs}] [
      list -spk      string {} field   {} {speaker fieldname }          ] ]}] {
      return -1 }

   set spkList ""
   DBase db
   db open $dbname.dat $dbname.idx -mode r
   foreach utt "$uttList" {
      if [catch {makeArray loc [db get $utt]} msg] {
         puts stderr "ERROR no entries for $utt in data base"
      } else {
         if ![info exists loc($field)] {
             puts stderr "ERROR no field $field for $utt in data base"
         } else {
             set spk $loc($field)
	     if {[lsearch $spkList $spk] == -1} {
                lappend spkList $spk
                set spkUttList($spk) "$utt"
             } else {
                lappend spkUttList($spk) "$utt"
             }
         }
     }
   }
   db destroy

   set ret ""
   foreach spk $spkList {
      lappend ret "$spk {$spkUttList($spk)}"
   }
   return $ret
}


# ------------------------------------------------------------------------
# readCorrectFromCFile
# reads the references from a tagged reference file (format: key <ref>)
# ------------------------------------------------------------------------

proc readCorrectFromCFile {args} {
    global refA
    
    set uttList     ""
    if [catch {itfParseArgv readCorrectFromCFile $args [list [
    list <uttList>   string  {} uttList {} {tcl list of utterances}     ] [
    list <cFileName> string  {} cfile   {} {tagged correct file name}   ] ] } ] {
	
	return -1 
    }
    
    if { [catch {set fp [open $cfile r]} errvar ] != 0 } {
	puts stderr "ERROR could not open file $cfile"
	return -1
    }
    while { [ gets $fp line ] >= 0 } {
	# will be a line like m255d_0000a     Lieber Herr Kruse <NIB>
	set key [lindex $line 0]
	set trans [lreplace $line 0 0]
	# do we have that key?
	foreach utt $uttList {
	    if { $utt == $key } {
		set refA($utt) $trans
	    }
	}
    }
    close $fp
    # check whether we have them all
    set gotN [llength [array names refA] ]
    set shouldN [llength $uttList]
    if { $gotN != $shouldN } {
	puts stderr "WARNING: did find only $gotN from $shouldN refs in $cfile"
    }
}



