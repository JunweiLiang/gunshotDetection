# ========================================================================
#  JanusRTK   Janus Speech Recognition Toolkit
#             ------------------------------------------------------------
#             Object: Scoring script
#             ------------------------------------------------------------
#
#  Author  :  The Ibis Gang
#  Module  :  score.tcl
#  Date    :  $Id: score.tcl 2390 2003-08-14 11:20:32Z fuegen $
#
#  Remarks :  Example scoring script
#
# ========================================================================
# 
#   $Log$
#   Revision 1.2  2003/08/14 11:19:46  fuegen
#   Merged changes on branch jtk-01-01-15-fms (jaguar -> ibis-013)
#
#   Revision 1.1.2.6  2003/08/12 09:43:19  soltau
#   fixed alignproc; changed tilde mode
#
#   Revision 1.1.2.5  2003/08/11 12:38:29  soltau
#   support for windows
#
#   Revision 1.1.2.4  2003/08/07 12:41:04  metze
#   Added options
#
#   Revision 1.1.2.3  2003/07/30 11:16:20  metze
#   Added -alignProg option
#
#   Revision 1.1.2.2  2002/07/12 16:29:55  metze
#   Approved version
#
#   Revision 1.1.2.1  2002/07/12 10:03:02  metze
#   Initial script collection
#
# ========================================================================


# ------------------------------------------------------------------------
#  Settings
# ------------------------------------------------------------------------

set spkLst    train-spk.means
set descFile  ../desc/desc.tcl
set semFile   ""
set hypoDir   hypos
set alignprog ""
set nfilter   ""

# -----------------------------------------------------------------------
#  Read in command-line options (override settings)
# -----------------------------------------------------------------------

if [catch {itfParseArgv $argv0 $argv [list [
    list -spkLst      string  {} spkLst        {} {speaker list}        ] [
    list -desc        string  {} descFile      {} {description file}    ] [
    list -hypos       string  {} hypoDir       {} {hypo directory}      ] [
    list -alignProg   string  {} alignprog     {} {align program}       ] [
    list -filter      string  {} nfilter       {} {noise filter script} ] [
    list -semFile     string  {} semFile       {} {semaphore file}      ] ]
} ] {
    puts stderr "ERROR in $argv0 while scanning options"
    exit -1
}

source $descFile

foreach i "optWord variants dbaseName meanFeatureSetDesc meanPath" {
    if {![info exists $i] || $i == ""} { eval "set $i \$${SID}($i)" }
}

waitFile "$semFile"


# ------------------------------------------------------------------------
#  Do it
# ------------------------------------------------------------------------

set path [file join $projectHome scoring]
cd $hypoDir

if { [regexp {indows} $tcl_platform(os)] } {
    set alignprog $path/align
} else {
    set alignprog $path/align.[exec uname -s]
}
if {![string length $nfilter]} {
    set nfilter   $path/noiseFilter.tcl 
}

scoreProc $path/utts  -correct $path/refs  \
    -filter    $nfilter                    \
    -alignprog $alignprog                  \
    -spell 0   -unk 5 -tilde 0             \
    -log RESULTS.test -keepalign alignTest

exit
