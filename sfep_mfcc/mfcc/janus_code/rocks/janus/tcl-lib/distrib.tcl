# ========================================================================
#  JANUS-SR   Janus Speech Recognition Toolkit
#             ------------------------------------------------------------
#             Object: Distribution Set
#             ------------------------------------------------------------
#
#  Author  :  Michael Finke
#  Module  :  distrib.tcl
#  Date    :  06/14/96
#
#  Remarks :  
#
# ========================================================================


proc distrib_Info { name } {
    switch $name {
	procs {
	    return "distribSetInit"
	}
	help {
	    return "This file provides an easy way to set up the
Gaussian mixture weights."
	}
	distribSetInit {
	    return "Initializes a set of distributions. It reads the
descriptions and can then load the parameters. by default, it assumes
that the underlying codebook is called 'codebookSet\$SID', which is
very easy to achieve if you use 'codebookSetInit'."
	}
    }
    return "???"
}


# ------------------------------------------------------------------------
#  distribSetInit  create and initialize a DistribSet object based on a
#                  description and parameter file.
# ------------------------------------------------------------------------

proc distribSetInit { LSID args } {
  upvar \#0 $LSID SID

  set distribSet  distribSet$LSID
  set codebookSet codebookSet$LSID
  set log         stderr
  set bmem        0

  if [info exist SID(distribSetDesc)] {
      set distribSetDesc $SID(distribSetDesc)
  }
  if [info exist SID(distribSetParam)] { 
      set distribSetParam $SID(distribSetParam)
  }

  itfParseArgv distribSetInit $args [list [
    list -distribSet   string  {} distribSet      {} {distribSet object} ] [
    list -codebookSet  object  {} codebookSet CodebookSet {codebook set} ] [
    list -desc         string  {} distribSetDesc  {} {description file}  ] [
    list -param        string  {} distribSetParam {} {parameter file}    ] [
    list -bmem         int     {} bmem            {} {bmem option}       ] ]

  if { [llength [info command $codebookSet]] &&
       [llength [info command $distribSet]] < 1} {

    DistribSet $distribSet $codebookSet -bmem $bmem
    if { [info exist distribSetDesc] && 
         [string length $distribSetDesc]} { 
      writeLog $log "$distribSet  read $distribSetDesc"
      $distribSet read   $distribSetDesc
      set SID(distribSetDesc)   $distribSetDesc
    }

    if { [info exist distribSetParam] &&
         [string length $distribSetParam]} { 
      writeLog $log "$distribSet  load $distribSetParam"
      $distribSet load   $distribSetParam
      set SID(distribSetParam)  $distribSetParam
    }
  }
}

