# ========================================================================
#  JANUS-SR   Janus Speech Recognition Toolkit
#             ------------------------------------------------------------
#             Object: Feature Set
#             ------------------------------------------------------------
#
#  Author  :  Michael Finke
#  Module  :  feature.tcl
#  Date    :  Mon Nov 18 13:14:10 EST 1996
#
#  Remarks :  
#
# ========================================================================


proc feature_Info { name } {
    switch $name {
	procs {
	    return "featureSetInit"
	}
	help {
	    return "This file covers the initialization of the
FeatureSet. See 'featshow.tcl' to find out more about the visualization
of these features."
	}
	featureSetInit {
	    return "Initializes a FeatureSet."
	}
    }
    return "???"
}


# ------------------------------------------------------------------------
#  featureSetInit   creation and initialization of a FeatureSet objects.
#                   A feature description file and a feature access 
#                   description can be used to configure the object.
# ------------------------------------------------------------------------

proc featureSetInit { LSID args } {
  upvar \#0 $LSID SID

  set featureSet featureSet$LSID
  set LDAMatrix  LDAMatrix
  set log        stderr

  if [info exist SID(featureSetDesc)] {
      set featureSetDesc $SID(featureSetDesc)
  }
  if [info exist SID(featureSetAccess)] { 
      set featureSetAccess $SID(featureSetAccess)
  }
  if [info exist SID(featureSetLDAMatrix)] { 
      set ldaMatrix $SID(featureSetLDAMatrix)
  }

  itfParseArgv featureSetInit $args [list [
    list -featureSet string  {} featureSet       {} {feature set name}      ] [
    list -desc       string  {} featureSetDesc   {} {description procedure} ] [
    list -access     string  {} featureSetAccess {} {access function}       ] [
    list -lda        string  {} ldaMatrix        {} {ptr to LDA matrix}     ] [
    list -ldaFeat    string  {} LDAMatrix        {} {feat for LDA matrix}   ] [
    list -log        string  {} log              {} {name of log channel}   ] ]

  if { [llength [info command $featureSet]] < 1} {

    FeatureSet $featureSet
    if { [info exist featureSetDesc] && [string length $featureSetDesc] } { 
      writeLog $log "$featureSet  desc   $featureSetDesc"
      $featureSet setDesc   $featureSetDesc
      set SID(featureSetDesc)   $featureSetDesc
    }
    if { [info exist featureSetAccess] && 
         [string length $featureSetAccess] } { 
      writeLog $log "$featureSet  access $featureSetAccess"
      $featureSet setAccess   $featureSetAccess
      set SID(featureSetAccess) $featureSetAccess
    }

  }

  # if featureSet exists and there is an LDA matrix to load
  # bload the matrix

  if { [llength [info command $featureSet]] && \
       [info exist ldaMatrix] && [string length $ldaMatrix]} {
    if { [$featureSet index $LDAMatrix] < 0} {
       $featureSet FMatrix $LDAMatrix 0 0

       if {[file exist $ldaMatrix]} {
         writeLog stderr "$featureSet:$LDAMatrix.data bload $ldaMatrix"
                          $featureSet:$LDAMatrix.data bload $ldaMatrix
       } else {
         writeLog stderr "$featureSet:$LDAMatrix.data bload $ldaMatrix FAILED"
       }       
    }
  }

  return $featureSet
}
