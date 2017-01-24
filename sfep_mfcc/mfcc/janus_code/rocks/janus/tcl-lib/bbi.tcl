# ========================================================================
#  JANUS-SR   Janus Speech Recognition Toolkit
#             ------------------------------------------------------------
#             Object: BBI stuff
#             ------------------------------------------------------------
#
#  Author  :  Juergen Fritsch
#  Module  :  bbi.tcl
#  Date    :  05/14/96
#
#  Remarks :  
#
# ========================================================================


proc bbi_Info { name } {
    switch $name {
	procs {
	    return "bbiSetInit"
	}
	help {
	    return "BBI (Bucket-Box-Intersection) is a Gaussian selection
algorithm, used for speed-up during decoding. Usually, the use of BBI during
decodings results in a speed-up of factor 2, with marginal loss in word accuracy.
The routines here set up the BBI infrastructure."
	}
	bbiSetInit {
	    return "Initializes a BBI tree (loads the description file) and loads
the parameters into the corresponding codebook. The codebook's scoring function then
uses the BBI tree for future score computations. You can also use this function
during creation of a BBI."
	}
    }
    return "???"
}


# ------------------------------------------------------------------------
#  bbiSetInit        initialize set of BBI trees for codebooks
#                    based on a description file and a parameter file.
# ------------------------------------------------------------------------

proc bbiSetInit { LSID args } {
  upvar \#0 $LSID SID

  set codebookSet codebookSet$LSID
  set log         stderr

  if [info exists SID(codebookSetDesc)] {
      set codebookSetDesc $SID(codebookSetDesc)
  }
  if [info exists SID(bbiSetDesc)] {
      set bbiSetDesc $SID(bbiSetDesc)
  }
  if [info exists SID(bbiSetParam)] {
      set bbiSetParam $SID(bbiSetParam)
  }

  itfParseArgv codebookSetInit $args [list [
    list -codebookSet string  {} codebookSet      {} {codebookSet object}  ] [
    list -desc        string  {} bbiSetDesc       {} {description file}    ] [
    list -param       string  {} bbiSetParam      {} {parameter file}      ] [
    list -log         string  {} log              {} {name of log channel} ] ]

  if { [llength [info command $codebookSet]] &&
       [info exist bbiSetDesc] &&
       [string length $bbiSetDesc] } {

    writeLog $log "$codebookSet readBBI $bbiSetDesc"
    $codebookSet  readBBI  $bbiSetDesc
    set SID(bbiSetDesc)   $bbiSetDesc
  }
  if { [llength [info command $codebookSet]] &&
       [info exist bbiSetParam] &&
       [string length $bbiSetParam] } { 

    writeLog $log "$codebookSet loadBBI $bbiSetParam"
    $codebookSet loadBBI  $bbiSetParam
    set SID(bbiSetParam)  $bbiSetParam
  }
}
