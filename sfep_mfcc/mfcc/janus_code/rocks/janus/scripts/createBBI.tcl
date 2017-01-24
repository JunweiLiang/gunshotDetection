# ========================================================================
#  JanusRTK   Janus Speech Recognition Toolkit
#             ------------------------------------------------------------
#             Object: Create BBI tree
#             ------------------------------------------------------------
#
#  Author  :  The Ibis Gang
#  Module  :  createBBI.tcl
#  Date    :  2000-08-07
#
#  Remarks :  This is a BBI creation script
#
# ========================================================================
# 
#  $Log$
#  Revision 1.3  2003/12/17 13:05:15  metze
#  P013
#
#  Revision 1.2  2003/08/14 11:19:42  fuegen
#  Merged changes on branch jtk-01-01-15-fms (jaguar -> ibis-013)
#
#  Revision 1.1.2.2  2002/07/12 16:29:55  metze
#  Approved version
#
#  Revision 1.1.2.1  2002/07/12 10:03:01  metze
#  Initial script collection
#
# ========================================================================


# ------------------------------------------------------------------------
#  Settings for bbi
# ------------------------------------------------------------------------

set iter             4
set depth            8
set gamma            0.5
set codebookSetDesc  ""
set codebookSetParam Weights/4.cbs.gz

set semFile          train.DONE
set descFile         ../desc/desc.tcl


# -----------------------------------------------------------------------
#  Read in command-line options (override settings)
# -----------------------------------------------------------------------

if [catch {itfParseArgv $argv0 $argv [list [
     list -desc        string  {} descFile         {} {description file} ] [
     list -spkLst      string  {} spkLst           {} {list of speakers}] [
     list -cbsDesc     string  {} codebookSetDesc  {} {codebook descriptions} ] [
     list -cbsParam    string  {} codebookSetParam {} {codebook parameters} ] [
     list -iter        int     {} iter             {} {iteration}] [
     list -depth       int     {} depth            {} {depth}] [
     list -gamma       float   {} gamma            {} {gamma value}] [
     list -semFile     string  {} semFile          {} {semaphore file}]]
}] {
    exit 1
}

source   $descFile

foreach i "codebookSetDesc codebookSetParam" {
    if {![info exists $i] || [set $i] == ""} { eval "set $i \$${SID}($i)" }
}

waitFile $semFile


# ------------------------------------------------------------------------
#  Init Modules
# ------------------------------------------------------------------------

phonesSetInit   $SID
tagsInit        $SID
featureSetInit  $SID -desc "" -desc "" -access ""
codebookSetInit $SID -param $codebookSetParam -desc $codebookSetDesc

regexp -all "(\[0-9\]).cbs.gz" $codebookSetDesc dummy iter
if {![file exists "bbiSet"]} {
    set fp [open "bbiSet" w]
    foreach cb [codebookSet$SID] { puts $fp "$cb OneForAll" }
    close $fp
}

bbiSetInit	$SID -desc "bbiSet" -param ""


# ------------------------------------------------------------------------
#  make bbi tree
# ------------------------------------------------------------------------

codebookSet$SID makeBBI -depth $depth -gamma $gamma -verbose 2
codebookSet$SID saveBBI "bbiTree${SID}-${iter}i-${depth}-$gamma.gz"

touch update.DONE

exit
