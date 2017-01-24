# ========================================================================
#  JANUS-SR   Janus Speech Recognition Toolkit
#             ------------------------------------------------------------
#             Object: Codebook Set
#             ------------------------------------------------------------
#
#  Author  :  Michael Finke
#  Module  :  codebook.tcl
#  Date    :  05/14/96
#
#  Remarks :  
#
# ========================================================================

proc codebook_Info { name } {
    switch $name {
	procs {
	    return "codebookSetInit"
	}
	help {
	    return "A CodebookSet contains a number of Codebooks,
the standard JRTk object for Gaussian functions. The mixture weights
are held in DistribSets."
	}
	codebookSetInit {
	    return "Creates a CodebookSet (reads the description file)
and can also load the parameters."
	}
    }
    return "???"
}


# ------------------------------------------------------------------------
#  CodebookSetInit   creation and initialization of a CodebookSet object
#                    based on a description file and a parameter file.
# ------------------------------------------------------------------------

proc codebookSetInit { LSID args } {
  upvar \#0 $LSID SID

  set codebookSet codebookSet$LSID
  set featureSet  featureSet$LSID
  set log         stderr
  set bmem        0

  if [info exist SID(codebookSetDesc)] {
      set codebookSetDesc $SID(codebookSetDesc)
  }
  if [info exist SID(codebookSetParam)] { 
      set codebookSetParam $SID(codebookSetParam)
  }

  itfParseArgv codebookSetInit $args [list [
    list -codebookSet string  {} codebookSet      {} {codebookSet object}  ] [
    list -featureSet  object  {} featureSet FeatureSet {feature set}       ] [
    list -desc        string  {} codebookSetDesc  {} {description file}    ] [
    list -param       string  {} codebookSetParam {} {parameter file}      ] [
    list -bmem        int     {} bmem             {} {use block memory management} ] [
    list -log         string  {} log              {} {name of log channel} ] ]

  if { [llength [info command $featureSet]] &&
       [llength [info command $codebookSet]] < 1} {

    CodebookSet $codebookSet $featureSet -bmem $bmem
    if { [info exist codebookSetDesc] &&
         [string length $codebookSetDesc]} { 
      writeLog $log "$codebookSet read $codebookSetDesc"
      $codebookSet read   $codebookSetDesc
      set SID(codebookSetDesc)   $codebookSetDesc
    }
    if { [info exist codebookSetParam] &&
         [string length $codebookSetParam]} { 
      writeLog $log "$codebookSet load $codebookSetParam"
      $codebookSet load   $codebookSetParam
      set SID(codebookSetParam)  $codebookSetParam
    }
  }
}



