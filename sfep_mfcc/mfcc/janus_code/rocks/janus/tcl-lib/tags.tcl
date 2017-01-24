# ========================================================================
#  JANUS-SR   Janus Speech Recognition Toolkit
#             ------------------------------------------------------------
#             Object: Tags Set
#             ------------------------------------------------------------
#
#  Author  :  Michael Finke
#  Module  :  tags.tcl
#  Date    :  05/14/96
#
#  Remarks :  
#
# ========================================================================


proc tags_Info { name } {
    switch $name {
	procs {
	    return "tagsInit"
	}
	help {
	    return "This file initializes the tags."
	}
	tagsInit {
	    return "Creates a 'Tags' object, usually called tags\$SID."
	}
    }
    return "???"
}


# ------------------------------------------------------------------------
#  tagsInit   creation and initialization of a Tags object
# ------------------------------------------------------------------------

proc tagsInit { LSID args } {
  upvar \#0 $LSID SID

  set tags tags$LSID
  set log  stderr

  if [info exist SID(tagsDesc)] { set tagsDesc $SID(tagsDesc) }

  itfParseArgv tagsInit $args [list [
    list -tags   string  {} tags       {} {tags object name}    ] [
    list -desc   string  {} tagsDesc   {} {description file}    ] [
    list -log    string  {} log        {} {name of log channel} ] ]

  if { [llength [info command $tags]] < 1} {

    Tags $tags
    if { [info exist tagsDesc] &&
         [string length $tagsDesc] } { 
      writeLog $log "$tags        read $tagsDesc"
      $tags read $tagsDesc
      set SID(tagsDesc) $tagsDesc
    }
  }
}
