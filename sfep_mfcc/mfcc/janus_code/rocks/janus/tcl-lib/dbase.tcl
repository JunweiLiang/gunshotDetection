# ========================================================================
#  JANUS-SR   Janus Speech Recognition Toolkit
#             ------------------------------------------------------------
#             Object: Database Access
#             ------------------------------------------------------------
#
#  Author  :  
#  Module  :  dbase.tcl
#  
#  $Log$
#  Revision 1.5  2007/02/23 10:23:17  fuegen
#  added support for dbaseConf functions to reconfigure dbase
#  (and mean paths, ...) on-the-fly (from Florian Metze)
#
#  Revision 1.4  2003/08/14 11:20:26  fuegen
#  Merged changes on branch jtk-01-01-15-fms (jaguar -> ibis-013)
#
#  Revision 1.3.50.3  2003/06/06 09:43:58  metze
#  Added latview, fixed upvar, updated tclIndex
#
#  Revision 1.3.50.2  2002/11/04 15:03:56  metze
#  Added documentation code
#
#  Revision 1.3.50.1  2001/09/26 07:16:15  metze
#  Cosmetic change
#
#  Revision 1.3  2000/09/10 12:18:37  hyu
#  Added Tcl files from Hagen
#
#  Revision 1.2  2000/05/10 13:58:07  soltau
#  handle wrong formated uttlists in foreachSegment
#
#  Revision 1.1  2000/05/10 13:14:53  soltau
#  Initial revision
#
#
# ========================================================================

proc dbase_Info { name } {
    switch $name {
	procs {
	    return "dbaseInit dbaseUttInfo foreachSegment dbaseUttFilter"
	}
	help {
	    return "These functions deal with the Janus database. Most scripts
rely on the database to find information related to the current speaker or the
current utterance. In most cases, the DBase is organized as two different databases:
one holding the information for all speakers (including which utterances they spoke) and
one containing the information specific for one utterance (ADC, FROM, TO, speaker, ...)."
	}
	dbaseInit {
	    return "Initializes the DBase."
	}
	dbaseUttInfo {
	    return "Returns all the information available in the DBase for
one given utterance. It combines the information in the speaker and utterance database."
	}
	foreachSegment {
	    return "Can be used to loop over all utterances for a given speaker. Calls
dbaseUttFilter to determine, if some segments should be left out."
	}
	dbaseUttFilter {
	    return "Can be re-defined to leave out utterances during the
training according to certain criteria."
	}
    }
    return "???"
}

# ------------------------------------------------------------------------
#  dbaseInit    creates a dbase object and opens the underlying 
#               data base
# ------------------------------------------------------------------------

proc dbaseInit { LSID args } {
  global dbInfo
  upvar  \#0 $LSID SID

  set dbase       db$LSID
  set log         stderr

  if [info exist SID(dbase)] {
      set dbase $SID(dbase)
  }
  if [info exist SID(dbaseName)] {
      set dbaseName $SID(dbaseName)
  }
  if [info exist SID(dbasePath)] { 
      set dbasePath $SID(dbasePath)
  } else {
      set dbasePath "./"
  }

  itfParseArgv dbaseInit $args [list [
    list <name>  string  {} dbaseName      {} {data base name}      ] [
    list -dbase  string  {} dbase          {} {data base object}    ] [
    list -path   string  {} dbasePath      {} {dbase path}          ] [
    list -log    string  {} log            {} {name of log channel} ] ]

  if { [llength [info command $dbase]] < 1} {

    DBase ${dbase}-spk
    DBase ${dbase}-utt

    ${dbase}-spk open $dbasePath/${dbaseName}-spk.dat \
	              $dbasePath/${dbaseName}-spk.idx -mode r
    ${dbase}-utt open $dbasePath/${dbaseName}-utt.dat \
                      $dbasePath/${dbaseName}-utt.idx -mode r

    writeLog $log "$dbase          open $dbasePath/${dbaseName}"

    # -- field that determines speaker in utt db --
    if [info exist SID(SPK)] {
      set dbInfo($dbase,SPK) $SID(SPK)
    } else {
      set dbInfo($dbase,SPK) SPEAKER
    }
    # -- field that determines utterances in spk db --
    if [info exist SID(UTT)] {
      set dbInfo($dbase,UTT) $SID(UTT)
    } else {
      set dbInfo($dbase,UTT) SEGS
    }
  }
  return $dbase
}

# ------------------------------------------------------------------------
#  dbaseUttInfo      get information of given speaker and utterance
# ------------------------------------------------------------------------
proc dbaseUttInfo { args } {

    if {[catch {itfParseArgv dbaseUttInfo $args [ list [
        list "<dbase>" string {} dbase   {} "database name (not object)" ] [
        list "<spkID>" string {} spk     {} "speaker ID" ] [
        list "<uttID>" string {} utt     {} "utterance ID" ] ]
    } ] } {
	return
    }

    return "{spk $spk} {utt $utt} [${dbase}-spk get $spk] [${dbase}-utt get $utt]"
}

# ------------------------------------------------------------------------
#  hack !!!!
# ------------------------------------------------------------------------
proc dbaseUttInfo1 { dbase args } {
    global SID
    if {"$dbase" == "uttDB"} {set dbase db$SID}
    eval dbaseUttInfo $dbase $args
}
if {![llength [info command uttDB]]} { DBase uttDB }
DBase method uttInfo dbaseUttInfo1 -text "find utterance information"

# ------------------------------------------------------------------------
#  foreachSegment    is a for loop over all segments of a a speaker.
#                    The segment list is obtained from a database.
# ------------------------------------------------------------------------

proc foreachSegment { utt dbase spk body } {
  global dbInfo SID
  upvar  $utt u

  # --- another hack !!! ---
  if {"$dbase" == "uttDB"} {set dbase db$SID}

  # --- not a hack ---
  if {[llength [info proc dbaseConf]]} { dbaseConf $SID $spk }

  set FIELD $dbInfo($dbase,UTT)
  if [catch {${dbase}-spk get $spk} spkInfo] {
    error "Can't find $spk in DBase ${dbase}-spk"
  }
  makeArray spkArray $spkInfo

  # handle wrong uttlist format's
  set uttL $spkArray($FIELD) 
  if {[llength $uttL] == 1} { 
    set uttL [lindex $uttL 0]
    # writeLog stderr "foreachSegment: fixed (?) '$spkArray($FIELD)' -> '$uttL'"
  }

  foreach utt $uttL {
    if {![${dbase}-spk uttFilter $utt]} { continue }
    set u $utt 
    uplevel $body
  }
}

# ------------------------------------------------------------------------
#  uttSpeaker   find speaker of given utterance
# ------------------------------------------------------------------------

proc uttSpeaker { utt dbase } {
  global dbInfo

  set FIELD $dbInfo($dbase,SPK)
  if [catch {${dbase}-utt get $utt} uttInfo] {
    error "Can't find $utt in DBase ${dbase}-utt"
  }
  makeArray uttArray $uttInfo
  return $uttArray($FIELD)
}

# ------------------------------------------------------------------------
#  uttFilter   filter utterances in foreachSegment
# ------------------------------------------------------------------------

proc dbaseUttFilter { args } {
    global SID
    
    if {[catch {itfParseArgv dbaseUttFilter $args [ list [
        list "<dbase>" string {} dbase   {} "database name (not object)" ] [
        list "<uttID>" string {} utt     {} "utterance ID" ] ]
    } ] } {
	return
    }

    # This is the default behaviour - change it in a local copy, if you like
    return 1

    if {"$dbase" == "uttDB"} {set dbase db$SID}
    regsub -- "-spk$|-utt$" $dbase "" dbase
    if [catch {${dbase}-utt get $utt} uttInfo] {
	error "Can't find $utt in DBase ${dbase}-utt"
    }
    makeArray uttArray $uttInfo

    # Beispiel: liefert 1, wenn kein passiver Sprecherueberlapp
    if {![info exists uttArray(XTALK0)]} {return 1}
    return [expr !$uttArray(XTALK0)]
}

DBase method uttFilter dbaseUttFilter -text "filter utterance in foreachSegment"
