#  ========================================================================
#   JANUS      Speech- to Speech Translation
#              ------------------------------------------------------------
#
#   Author  :  Martin Westphal
#   Email   :  westphal@ira.uka.de
#   Script  :  label.tcl
#   Date    :  21.07.95
#
#  ========================================================================
#
#   $Log$
#   Revision 1.2  2004/08/16 16:21:06  metze
#   P014, histogram
#
#   Revision 1.1  2000/08/23 11:00:01  jmcd
#   Added janus files from gui-tcl.
#
# Revision 1.2  1999/01/22  09:39:47  westphal
# labels from path (multiple rows)
#
# Revision 1.1  1997/07/28  15:39:18  westphal
# Initial revision
#
# Revision 1.4  1996/10/11  17:08:48  westphal
# bindings for label bars
#
# Revision 1.3  1995/11/06  13:20:26  maier
# selectLabel
#
# Revision 1.2  1995/10/30  17:24:06  maier
# *** empty log message ***
#
# Revision 1.1  1995/08/21  06:01:34  maier
# Initial revision
#
# 
#  ========================================================================

proc label_Info { name } {
    switch $name {
	procs {
	    return ""
	}
    }
    return "???"
}

#------------------------------------------------------------------------------
# This procedure is called when a label text was selected with mouse button-1
# and can be replaced by another procedure doing something else.
#------------------------------------------------------------------------------
proc selectLabel {tag count text} {
   puts "$tag $count: $text"
}

#------------------------------------------------------------------------------
# This procedure shows a single label and is used by 'showLabels' to display
# a whole set of labels.
#------------------------------------------------------------------------------
proc showLabel {c tag count text from to {rcolor ""} {tcolor ""} {y 0}} {

   set space 5
   set height [lindex [$c configure -height] 4]

   # --- box ---
   if {$rcolor != ""} {set rcolor "-fill $rcolor"}
   eval "$c create rect $from $y $to $height -tag $tag $rcolor"

   # --- text ---
   incr from  $space
   incr to   -$space
   set width [expr $to - $from]

   if {$width > 0} {
     if {$tcolor != ""} {set tcolor "-fill $tcolor"}
     $c bind [
         eval "$c create text $from [expr $y + $space] -tag $tag -width $width \
	       -anchor nw $tcolor -text {$text}"
     ] <Button-1> "catch {selectLabel $tag $count {$text}}" 
   }
}

#------------------------------------------------------------------------------
# This procedure plots the label bar, an overview of all labels
#------------------------------------------------------------------------------
proc plotLabelbar {fsN labels frameN} {
    plotRect $fsN 0 0 2 delete
    set count 0
    foreach label $labels {
	set time [lindex $label 1]
	set from [lindex $time 0]
	set to   [lindex $time 1]; incr to
	set ret [plotRect $fsN [expr 1.0 * $from / $frameN] [expr 1.0 * $to / $frameN] 2 red]
	set canvas [lindex $ret 0]
	set ind    [lindex $ret 1]
	$canvas bind $ind <Button-1> "\
		if {\[selectLabel $fsN $count {}\] >= 0} {\
	           gotoTime $fsN \$fsPar($fsN,mfrom)\
	        }"
	incr count
    }
}
#==============================================================================
#
# showLabels
#
# c          canvas
# labels     string with labels "{text {from to}} {text {from to}} ..."
# dx         framewidth in pixels (integer or float)
# fromL      first frame displayed in canvas (interger or float)
#==============================================================================
proc showLabels {c labels dx fromL {tag label}} {

   if {$labels == ""} {return 0}
   set SILcolor    {{} black}
   set NOISEcolor  {{} blue}
   set SPEECHcolor {{} red}
   set width [lindex [$c configure -width] 4]
   set toL   [expr int($width/$dx + $fromL + 1)]
   $c delete label
   set count 0
   foreach label $labels {

      # --- get info ---
      if {[llength $label] < 2} {
        error "expected label: \"text {from to} ..\" not \"$label\""
      }
      set text [lindex $label 0]
      set time [lindex $label 1]
      if {[llength $label] > 2} {
	  set y [lindex $label 2]
      } else {
	  set y 0
      }
      if {[llength $time] != 2} {
        error "expected label: \"text {from to}\" not \"$label\""
      }
      set from [lindex $time 0]
      set to   [lindex $time 1]
      # --- plot label ---
      if {$to > $fromL && $from < $toL} {
        set color $SPEECHcolor
        set from [expr int ($dx * ($from - $fromL)     + 0.5)]
        set to   [expr int ($dx * ($to   - $fromL + 1) + 0.5)]

        if {$text == "(" || $text == ")" || $text == "SIL" || 
            $text == "optional-silence"} { 
          set color $SILcolor 
        }

        set fl [string index $text 0]
        if {$fl == "+" || $fl == "\$" || $fl == "#"} {set color $NOISEcolor}

        showLabel $c $tag $count $text $from $to [lindex $color 0] [lindex $color 1] $y
      }
      incr count
   }
}

#==============================================================================
# setPathLabels
# set the labels according to a given a 'path' and 'hmm' from a recognition 
# system
#==============================================================================
proc setPathLabels {fsX SID {levels {words phones}} {dx 30}} {
    if {[objects Path path$SID] == "" || [objects HMM hmm$SID] == ""} {
	error "'path$SID' or 'hmm$SID' does not exist, create first!"
    }
    global fsPar
    set L ""
    set y 0
    foreach what $levels {
	foreach item [lindex [path$SID labels hmm$SID -what $what] 1] {
	    lappend L "[lrange $item 0 1] $y"
	}
	incr y $dx
    }
    set fsPar($fsX,labels) $L
}







