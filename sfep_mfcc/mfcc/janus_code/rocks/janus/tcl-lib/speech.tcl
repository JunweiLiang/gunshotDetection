# ========================================================================
#  JANUS-SR   Janus Speech Recognition Toolkit
#             ------------------------------------------------------------
#             Object: Sil/Speech Detector
#             ------------------------------------------------------------
#
#  Author  :  Martin Westphal
#  Module  :  speech.tcl
#  Date    :  Jan.97
#
#  Remarks :  
#
# ========================================================================


proc speech_Info { name } {
    switch $name {
	procs {
	    return "speechInit"
	}
	help {
	    return "Sil/Speech Detector based on Gaussian mixture."
	}
	speechInit {
	    return "Creation and initialization of a speech detector
using a codebookSet and a distribSet
based on a description file and a parameter file."
	}
    }
    return "???"
}


# ------------------------------------------------------------------------
#  SpeechInit   creation and initialization of a speech detector
#               using a codebookSet and a distribSet
#               based on a description file and a parameter file.
# ------------------------------------------------------------------------
#  featureSetInit  $SID 
#  speechInit $SID
#  
#    $fes speechDetect SPEECH FEAT
#
proc speechInit { LSID args } {
  upvar \#0 $LSID SID

  set codebookSet codebookSetSpeech
  set distribSet  distribSetSpeech
  set featureSet  featureSet$LSID

  set codebookSetDesc  ../speech_desc/codebookSet
  set codebookSetParam speech.cbs.gz
  set distribSetDesc   ../speech_desc/distribSet
  set distribSetParam  speech.dss.gz
  set aspe 0.8
  set log         stderr

  itfParseArgv speechInit $args [list [
    list -featureSet  object  {} featureSet FeatureSet {feature set}       ] [
    list -cbsdesc     string  {} codebookSetDesc  {} {description file}    ] [
    list -cbsparam    string  {} codebookSetParam {} {parameter file}      ] [
    list -dssdesc     string  {} distribSetDesc   {} {description file}    ] [
    list -dssparam    string  {} distribSetParam  {} {parameter file}      ] [
    list -apriori     float   {} aspe             {} {speech a priori prob}] [
    list -log         string  {} log              {} {name of log channel} ] ]

  # Init speech codebookSet
  if { [llength [info command $codebookSet]] < 1} {
    CodebookSet $codebookSet $featureSet
    if { [info exist codebookSetDesc] &&
         [string length $codebookSetDesc]} { 
      writeLog $log "$codebookSet read $codebookSetDesc"
      $codebookSet read   $codebookSetDesc
    }
    if { [info exist codebookSetParam] &&
         [string length $codebookSetParam]} { 
      writeLog $log "$codebookSet load $codebookSetParam"
      $codebookSet load   $codebookSetParam
    }
  }
  # Init speech distribSet
  if { [llength [info command $distribSet]] < 1} {
    DistribSet $distribSet $codebookSet
    if { [info exist distribSetDesc] && 
         [string length $distribSetDesc]} { 
      writeLog $log "$distribSet  read $distribSetDesc"
      $distribSet read   $distribSetDesc
    }

    if { [info exist distribSetParam] &&
         [string length $distribSetParam]} { 
      writeLog $log "$distribSet  load $distribSetParam"
      $distribSet load   $distribSetParam
    }
  }

  writeLog $log "a priori value of speech class: $aspe"
  # Define speechDetect procedure
  proc speechDetect "fes destFe sourceFe {dss $distribSet} {aspe $aspe}" {
    # --- a priori values ---
    set asil [expr 1.0 - $aspe]
    set frameN [$fes : $sourceFe configure -frameN]
    set buffer ""
    for {set frameX 0} {$frameX < $frameN} {incr frameX} {
        set sil [expr exp(-1.0 * [$dss score SIL $frameX])]
        set spe [expr exp(-1.0 * [$dss score SPEECH $frameX])]
        set total [expr $asil * $sil + $aspe * $spe]
        set pspe [expr $aspe * $spe / $total]
        lappend buffer "$pspe"
    }
    $fes FMatrix $destFe $buffer
    return $frameN
  }
}

FeatureSet method speechDetect speechDetect -text "speech detector based on gaussian mixture"
