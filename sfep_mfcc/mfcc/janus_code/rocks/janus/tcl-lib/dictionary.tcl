# ========================================================================
#  JANUS-SR   Janus Speech Recognition Toolkit
#             ------------------------------------------------------------
#             Object: Dictionary
#             ------------------------------------------------------------
#
#  Author  :  Michael Finke
#  Module  :  dictionary.tcl
#  Date    :  05/14/96
#
#  Remarks :  
#
# ========================================================================


proc dictionary_Info { name } {
    switch $name {
	procs {
	    return "dictInit"
	}
	help {
	    return "These functions deals with the dictionary."
	}
	dictInit {
	    return "Creates a dictionary object and possibly loads a
dictionary file into it."
	}
    }
    return "???"
}

Dictionary configure -blkSize 5000


# ------------------------------------------------------------------------
#  dictInit
# ------------------------------------------------------------------------

proc dictInit { LSID args } {
  upvar \#0 $LSID      SID

  set    log       stderr
  set    dict      dict$LSID
  set    phones    phonesSet$LSID:PHONES
  set    tags      tags$LSID

  if {[info exist SID(dictDesc)]} {
      set dictDesc $SID(dictDesc)
  }

  itfParseArgv dictInit $args [list [
    list -phones    string  {} phones          {} {phones}              ] [
    list -tags      string  {} tags            {} {tags}                ] [
    list -dict      string  {} dict            {} {dictionary}          ] [
    list -desc      string  {} dictDesc        {} {description file}    ] ]

  if {![llength [info command $dict]]} {
    Dictionary $dict $phones $tags
    if {[info exist dictDesc] &&
        [string length $dictDesc]} {
      writeLog $log "$dict        read $dictDesc"
      $dict read          $dictDesc
      set   SID(dictDesc) $dictDesc
    }
  }
}
