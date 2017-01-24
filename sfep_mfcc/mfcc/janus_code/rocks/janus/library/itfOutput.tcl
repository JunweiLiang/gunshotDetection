#  ========================================================================
#   JRTk       Janus Recognition Toolkit
#              ------------------------------------------------------------
#              Object: Error Output Functions
#              ------------------------------------------------------------
#
#   Author  :  Michael Finke & Ivica Rogina
#   Module  :  itfOutput.tcl
#   Date    :  $Id: itfOutput.tcl 28 1997-07-15 10:19:12Z westphal $
#   Remarks :
#
#   $Log$
#   Revision 1.2  1997/07/15 10:19:12  westphal
#   corrected bug with format
#
#   Revision 1.1  1997/07/15 10:14:44  westphal
#   Initial revision
#
#
#  ========================================================================

set itfErrorMajor ""
set itfErrorMinor ""

proc itfOutput { cmd mode text file line major minor } {

  global itfOutputBuffer itfErrorMajor itfErrorMinor

  if {"$cmd" != "DUMP"} {
      set itfErrorMajor $major
      set itfErrorMinor $minor
  }

  switch $cmd {
    CLEAR   { if [info exist itfOutputBuffer] { unset itfOutputBuffer }}
    DUMP    { if [info exist itfOutputBuffer] {
                 foreach e $itfOutputBuffer { puts -nonewline stderr $e }
                 unset  itfOutputBuffer
              }
            }
  }

  if { $major != "" } { 
    set where [format "%s(%04d) <%-3s,%-3s> " $file $line $major $minor]
  } else              { 
    set where [format "%s(%04d)"              $file $line]
  }
  set msgString [format "%-18s %s" $where $text]

  if { $mode == 1} {
     lappend itfOutputBuffer [format "%-8s%s" $cmd $msgString]
     return $cmd
  }

  if [info exist itfOutputBuffer] {
      foreach e $itfOutputBuffer { puts -nonewline stderr $e }
      unset  itfOutputBuffer
  }

  switch $cmd {
    PRINT   { puts -nonewline "$text"                     }
    INFO    { puts -nonewline stderr "INFO    $msgString" }
    ERROR   { puts -nonewline stderr "ERROR   $msgString" }
    WARNING { puts -nonewline stderr "WARNING $msgString" }
    FATAL   { puts -nonewline stderr "FATAL   $msgString"; exit }
  }
  return
}

