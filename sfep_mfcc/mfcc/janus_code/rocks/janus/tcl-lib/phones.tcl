# ========================================================================
#  JANUS-SR   Janus Speech Recognition Toolkit
#             ------------------------------------------------------------
#             Object: Phones Set
#             ------------------------------------------------------------
#
#  Author  :  Michael Finke
#  Module  :  phones.tcl
#  Date    :  06/07/96
#
#  Remarks :  
#
# ========================================================================


proc phones_Info { name } {
    switch $name {
	procs {
	    return "phonesSetInit"
	}
	help {
	    return "Deals with the PhonesSet."
	}
	phonesSetInit {
	    return "Initializes a PhonesSet."
	}
    }
    return "???"
}


# ------------------------------------------------------------------------
#  phonesSetInit   creation and initialization of a phonesSet object
# ------------------------------------------------------------------------

proc phonesSetInit { LSID args } {
  upvar \#0 $LSID SID

  set phonesSet phonesSet$LSID
  set log       stderr

  if [info exist SID(phonesSetDesc)] { set phonesSetDesc $SID(phonesSetDesc) }

  itfParseArgv phonesSetInit $args [list [
    list -phonesSet string  {} phonesSet       {} {phones set}          ] [
    list -desc      string  {} phonesSetDesc   {} {description file}    ] [
    list -log       string  {} log             {} {name of log channel} ] ]

  if { [llength [info command $phonesSet]] < 1} {

    PhonesSet $phonesSet
    if { [info exist phonesSetDesc] && [string length $phonesSetDesc] } { 
      writeLog $log "$phonesSet   read $phonesSetDesc"
      $phonesSet read $phonesSetDesc
      set SID(phonesSetDesc) $phonesSetDesc
    }
  }
}

