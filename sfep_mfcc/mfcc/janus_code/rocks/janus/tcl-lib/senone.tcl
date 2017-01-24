# ========================================================================
#  JANUS-SR   Janus Speech Recognition Toolkit
#             ------------------------------------------------------------
#             Object: SenoneSet
#             ------------------------------------------------------------
#
#  Author  :  Michael Finke
#  Module  :  distribTree.tcl
#  Date    :  05/14/96
#
#  Remarks :  Martin Westphal: added phones/tags flags
#
# ========================================================================


proc senone_Info { name } {
    switch $name {
	procs {
	    return "senoneSetInit"
	}
	help {
	    return "This file contains various procedures."
	}
	senoneSetInit {
	    return "Initializes the SenoneSet."
	}
    }
    return "???"
}


# ------------------------------------------------------------------------
#  senoneSetInit
# ------------------------------------------------------------------------

proc senoneSetInit { LSID args } {
  upvar \#0 $LSID SID

  set phones        phonesSet$LSID:PHONES
  set tags          tags$LSID

  if [info exist SID(senoneSetDesc)] {
      set senoneSetDesc $SID(senoneSetDesc)
  }

  itfParseArgv senoneSetInit $args [list [
    list <streams>  string  {} streams       {}        {stream array}     ] [
    list -phones    object  {} phones        Phones    {phones set}       ] [
    list -tags      object  {} tags          Tags      {tags set}         ] [
    list -desc      string  {} senoneSetDesc {}        {description file} ] ]

  if { [llength [info command senoneSet$LSID]] < 1} {
    SenoneSet senoneSet$LSID $streams -phones $phones -tags $tags

    if { [info exist senoneSetDesc] &&
         [string length $senoneSetDesc] } {

      puts "senoneSet$LSID   read $senoneSetDesc"
      senoneSet$LSID  read $senoneSetDesc
      set SID(senoneSetDesc) $senoneSetDesc
    }
  }
}
