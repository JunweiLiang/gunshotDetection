# ========================================================================
#  SWB        Switchboard Telephone Conversation Corpus
#             ------------------------------------------------------------
#             Object: Topology Routines
#             ------------------------------------------------------------
#
#  Author  :  Michael Finke
#  Module  :  topo.tcl
#  Date    :  06/14/96
#
#  Remarks :  Martin Westphal: added senoneSet/tmSet flags
#                              added phones/phonesSet/tags/topoSet flags
# ========================================================================


proc topo_Info { name } {
    switch $name {
	procs {
	    return "topoSetInit ttreeInit"
	}
	help {
	    return "This file initializes the TopoSet."
	}
	topoSetInit {
	    return "Creates a 'TopoSet'."
	}
 	ttreeInit {
	    return "Creates a 'TopoTree'."
	}
   }
    return "???"
}


# ------------------------------------------------------------------------
#  topoSetInit
# ------------------------------------------------------------------------

proc topoSetInit { LSID args } {
  upvar \#0 $LSID SID

  set log stderr
  set senoneSet    senoneSet$LSID
  set tmSet        tmSet$LSID

  if [info exist SID(tmDesc)] {
      set tmDesc $SID(tmDesc)
  }
  if [info exist SID(topoSetDesc)] { 
      set topoSetDesc $SID(topoSetDesc)
  }

  itfParseArgv topoSetInit $args [list [
    list -tm        string {} tmDesc      {}        {transistion description} ] [
    list -senoneSet object {} senoneSet   SenoneSet {senoneSet set}           ] [
    list -tmSet     object {} tmSet       TmSet     {tmSet set}               ] [
    list -desc      string {} topoSetDesc {}        {topology description}    ] ]

  if { [llength [info command $tmSet]] < 1} {

    TmSet $tmSet
    if { [info exist tmDesc] && [string length $tmDesc] } {
      writeLog $log "$tmSet       read $tmDesc"
      $tmSet read $tmDesc
      set SID(tmDesc) $tmDesc
    }
  }

  if { [llength [info command $tmSet]] &&
       [llength [info command $senoneSet]] &&
       [llength [info command topoSet$LSID]] < 1} {

    TopoSet topoSet$LSID $senoneSet $tmSet

    if { [info exist topoSetDesc] && [string length $topoSetDesc] } {
        writeLog $log "topoSet$LSID     read $topoSetDesc"
        topoSet$LSID read $topoSetDesc
        set SID(topoSetDesc) $topoSetDesc
    }
  }
}

# ------------------------------------------------------------------------
#  ttreeInit
# ------------------------------------------------------------------------

proc ttreeInit { LSID args } {
  upvar \#0 $LSID SID

  set    log  stderr
  set phones        phonesSet$LSID:PHONES
  set phonesSet     phonesSet$LSID
  set tags          tags$LSID
  set topoSet       topoSet$LSID

  if [info exist SID(ttreePTreeDesc)] {
      set ttreePTreeDesc $SID(ttreePTreeDesc)
  }
  if [info exist SID(ttreeDesc)] { set ttreeDesc $SID(ttreeDesc) }
  if [info exist SID(padPhone)]  { set padPhone  $SID(padPhone)  }

  itfParseArgv ttreeInit $args [list [
    list -phones    object  {} phones         Phones    {phones set}       ] [
    list -phonesSet object  {} phonesSet      PhonesSet {phonesSet set}    ] [
    list -tags      object  {} tags           Tags      {tags set}         ] [
    list -topoSet   object  {} topoSet        TopoSet   {topoSet set}      ] [
    list -ptree     string  {} ttreePTreeDesc {}        {polyphonic tree}  ] [
    list -desc      string  {} ttreeDesc      {}        {description file} ] [
    list -padPhone  string  {} padPhone       {}        {padding phone}    ] ]

  if { [llength [info command ttree$LSID]] < 1} {

    Tree ttree$LSID $phones $phonesSet $tags $topoSet

    if { [info exist padPhone] && [string length $padPhone]} {
      ttree$LSID configure -padPhone [$phones index $padPhone]
      set SID(padPhone) $padPhone
    }

    if { [info exist ttreePTreeDesc] && [string length $treePTreeDesc] } { 
       writeLog $log "ttree$LSID.PT    read $ttreePTreeDesc"
       ttree$LSID.ptreeSet read $ttreePTreeDesc
       set SID(ttreePTreeDesc) $ttreePTreeDesc
    }
    if { [info exist ttreeDesc] && [string length $ttreeDesc] } { 
       writeLog $log "ttree$LSID       read $ttreeDesc"
       ttree$LSID read $ttreeDesc
       set SID(ttreeDesc) $ttreeDesc
    }
  }
}
