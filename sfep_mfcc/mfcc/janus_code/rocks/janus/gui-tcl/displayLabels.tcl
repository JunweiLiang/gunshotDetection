#
# $Log$
# Revision 1.2  2003/08/14 11:19:39  fuegen
# Merged changes on branch jtk-01-01-15-fms (jaguar -> ibis-013)
#
# Revision 1.1.52.1  2002/11/04 15:02:07  metze
# Added documentation code
#
# Revision 1.1  2000/08/23 11:00:01  jmcd
# Added janus files from gui-tcl.
#
# Revision 1.3  1996/01/15  15:22:43  rogina
# added feature(set) selection menu
#
# Revision 1.2  1996/01/11  15:37:55  rogina
# modified global identifier names to avoid collisions with other scripts
#
# Revision 1.1  1996/01/11  14:49:17  rogina
# Initial revision
#
#


proc displayLabels_Info { name } {
    switch $name {
	procs {
	    return "displayLabels"
	}
	help {
	    return "These functions allow you to view the contents of a 
label object. They are used by featshow."
	}
	displayLabels {
	    return "Usage: \\texttt{displayLabels <path> <hmm>}

        \\begin{description}
	\\item\[THE UTTERANCE WINDOW\]

	The first window will show a rectangle for each of the words that were
	aligned in the utterance. (Optional words that were not aligned are
	not displayed.) Each rectangle's width is proportional to the number
	of frames that are consumed by the word, and its height is proportional
	to the number of states (in terms of AModel-states). Every rectangle is
	labeled with the orthographic spelling of the word, and with the frame
	range (first frame .. last frame). However, you can decide yourself
	what is displayed by choosing the appropriate radio-buttons in the
        'full-view' menu.
	Within the word-rectangles you can see smaller rectangles, representing
	phonemes. These rectangles can be labelled with the phones names, if
        you choose so.
	You can choose the size of the display by clicking on the appropriate
	radio-button in the 'full zoom' window.

	\\item\[THE DETAILED VIEW WINDOW\]

	Clicking on a word's rectangle (not on one of the phones) with the 1st
	mouse button will open a new window with a detailed display of the 
	selected word. You can also select an area by dragging the mouse while 
	holding the 3rd button. Or select an area by clicking on a phone's
        rectangle. After you realease the mouse button, you'll get
	a window with a detailed view of the selected area.
	In this window you'll find a grid displaying the frames and states
	of the utterance. The frames are labelled in a synchonuously scrolling
        canvas below the main display canvas, the states are labelled (with the
	senone names) in a synchronuously scrolling canvas to the left of the
	main display canvas. Above the main display canvas is another
	synchonuously scrolling canvas, whose contents are defined by the
	procedure 'displayLabelsScore'. The default is to display the local
	acoustic score for every frame, however you can redifine this function
	to display whatever you wish from what is available in a CELL (see
	displayLabelsLaprep for details). Every visited state is represented
	by a circle. Below the circle are one, up to three, or up to six) lines
	which show more information about the state. What is displayed below
	the circles is defined in the procedure 'displayLabelsBelowCircle'.
	Have a look at the code of the procedure if you'd like to redefine it.
	Clicking on a circle will display all available information about that
	state in an extra window.
	You can choose how detailed your 'detailed view window' is by clicking
	on the appropriate radio-button in the 'detailed zoom' menu.
	\\end{description}

        Note that this procedure defines several global identifiers."
	}
    }
    return "???"
}


set displayLabelsHelpMessage {
 -----------------------------------------------------------------------------

	USAGE:
	======
   		<path> display <hmm>

	THE UTTERANCE WINDOW
	====================

	The first window will show a rectangle for each of the words that were
	aligned in the utterance. (Optional words that were not aligned are
	not displayed.) Each rectangle's width is proportional to the number
	of frames that are consumed by the word, and its height is proportional
	to the number of states (in terms of AModel-states). Every rectangle is
	labeled with the orthographic spelling of the word, and with the frame
	range (first frame .. last frame). However, you can decide yourself
	what is displayed by choosing the appropriate radio-buttons in the
        "full-view" menu.
	Within the word-rectangles you can see smaller rectangles, representing
	phonemes. These rectangles can be labelled with the phones names, if
        you choose so.
	You can choose the size of the display by clicking on the appropriate
	radio-button in the "full zoom" window.

	THE DETAILED VIEW WINDOW
	========================

	Clicking on a word's rectangle (not on one of the phones) with the 1st
	mouse button will open a new window with a detailed display of the 
	selected word. You can also select an area by dragging the mouse while 
	holding the 3rd button. Or select an area by clicking on a phone's
        rectangle. After you realease the mouse button, you'll get
	a window with a detailed view of the selected area.
	In this window you'll find a grid displaying the frames and states
	of the utterance. The frames are labelled in a synchonuously scrolling
        canvas below the main display canvas, the states are labelled (with the
	senone names) in a synchronuously scrolling canvas to the left of the
	main display canvas. Above the main display canvas is another
	synchonuously scrolling canvas, whose contents are defined by the
	procedure "displayLabelsScore". The default is to display the local
	acoustic score for every frame, however you can redifine this function
	to display whatever you wish from what is available in a CELL (see
	displayLabelsLaprep for details). Every visited state is represented
	by a circle. Below the circle are one, up to three, or up to six) lines
	which show more information about the state. What is displayed below
	the circles is defined in the procedure "displayLabelsBelowCircle".
	Have a look at the code of the procedure if you'd like to redefine it.
	Clicking on a circle will display all available information about that
	state in an extra window.
	You can choose how detailed your "detailed view window" is by clicking
	on the appropriate radio-button in the "detailed zoom" menu.


	GLOBAL IDENTIFIER ALERT:
	========================

	The following procedures are defined in here:

		displayLabelsItemMark
		displayLabelsItemStroke
		displayLabelsSetArea
		displayLabelsHelp
		displayLabelsBothHscroll
		displayLabelsDetailBothHscroll
		displayLabelsDetailBothVscroll
		displayLabelsScore
		displayLabelsBelowCircle
		displayLabelsTrans
		displayLabelsState
		displayLabelsZoomInArea
		displayLabelsUttWindow
		displayLabelsBothCan
		displayLabelsInWindow
		displayLabelsSelect
		displayLabelsFirstFeature
		displayLabelsSelectFeatureSet
		displayLabelsSelectFeature
		displayLabelsUpdateFeature
		displayLabelsConfigurations
		displayLabelsLaprep
		displayLabels

	The following global variables or arrays are defined in here:

		displayLabelsHelpMessage
		displayLabelsParams
		displayLabelsAreaX1 
		displayLabelsAreaY1 
		displayLabelsAreaX2 
		displayLabelsAreaY2

	ABOUT THE PREPARATION OF LABELS
	===============================

	The "displayLabelsLaprep" procedure accepts a Path object and an Hmm 
	object as its arguments and returns a list of five lists, that can be 
	used for displaying. Thre first of the five sublists is an enriched
	word-labels list, the second is an enriched phone-labels list, and the
	third is a list of path cells for each frame, containing all the path 
	information. The fourth list is simply a copy of the HMM's stategraph 
	output's first list, i.e. one senone name for each of the HMM states. 
	The fifth list is a concetenation of all the amodels' senones along 
	the path, i.e. one senone name for each of the 'real' states as opposed
	to the HMM states. The prepraration's most important task is to 
	eliminate alternative paths (optional words, or alternative 
	cross-word-triphones, pronounciation variants etc.). The resulting
	output looks a bit like the output of the "labels" method for paths. 
	However, the state indices that are used here, are different and don't
	have holes where phones or words are missing. That way it is nice to 
	display a path and to see a meaning in its slope. When we are talking 
	about state indices (STATEX) then' we mean those 'cleaned' indices, 
	which resemble the 'real' path (the slope of the path, i.e. the ratio 
	of states over frames, does have a meaning). When we mean the state 
	indices as they are used in the HMM object (actually the  StateGraph 
	subobject off the HMM object) then we call them HMM-STATEX.

	Here comes the output format of displayLabelsLaprep:


	 OUTPUT ::= { { PATH-OBJECT-NAME HMM-OBJECT-NAME }
	              WORD-LIST PHONE-LIST CELL-LIST 
		      HMM-SENONES REAL-SENONES }
	
	 WORD-LIST    ::= { WORD  WORD  ... WORD  }	(wordN  elements)
	 PHONE-LIST   ::= { PHONE PHONE ... PHONE }	(phoneN elements)
	 CELL-LIST    ::= { CELL  CELL  ... CELL  }	(frameN elements)
	 HMM-SENONES  ::= { NAME  NAME  ... NAME  }     (HMM-stateN elements)
	 REAL-SENONES ::= { NAME  NAME  ... NAME  }     (real-stateN elements)

	 WORD ::= { SPELLING { FIRST-FRAME LAST-FRAME } 
                     { FIRST-STATE LAST-STATE PHONE-NUMBER } }

	 PHONE ::= { NAME { FIRST-FRAME LAST-FRAME }
	                  { FIRST-STATE LAST-STATE }
			  { HMM-FIRST-STATE HMM-LAST-STATE HMM-PHONEX } }

	 CELL ::= { BEST-SCORE HMM-STATEX SENONEX HMM-PHONEX HMM-WORDX 
		    LOCAL-SCORE SENONE-NAME { TRANS TRANS ... TRANS } 
		    STATEX STATE-JUMP HMM-STATE-JUMP CUM-SCORE TRANS-PEN }

	 TRANS ::= { OFFSET PENALTY }


	 where the identifiers have the following meanings:

	 SPELLING	   name of word
	 FIRST-FRAME	   index of first frame of the word/phone
	 LAST-FRAME 	   index of last frame of the word/phone
	 FIRST-STATE	   index of first state in the output representation
	 LAST-STATE	   index of last state in the output representation
	 PHONE-NUMBER	   number of phones the build a word
	 HMM-FIRST-STATE   index of first state in the underlying HMM object
	 HMM-LAST-STATE    index of last state in the underlying HMM object
	 BEST-SCORE        best score found in a frame during alignment
	 HMM-PHONEX        phone index as used in phonegraph of HMM object
	 HMM-WORDX	   word index as used in wordgraph of the HMM object
	 HMM-STATEX        state index as used in stategraph of the HMM object
	 SENONEX	   index of the cell's senone
	 SENONE-NAME	   name of the cell's senone
	 STATEX		   index of cell's state in the output representation 
	 STATE-JUMP        offset (number of states) 'really' taken to get here
	 HMM-STATE-JUMP    offset (number of states) as used in the HMM object
	 CUM-SCORE         cumulative score up to (and including) this cell
	 TRANS-PEN         transition penalty from previous frame to this frame

 -----------------------------------------------------------------------------
}

proc displayLabelsItemMark {c x y} {
    global displayLabelsAreaX1 displayLabelsAreaY1 displayLabelsAreaX2 displayLabelsAreaY2
    set displayLabelsAreaX1 [$c canvasx $x]
    set displayLabelsAreaY1 [$c canvasy $y]
    set displayLabelsAreaX2 $displayLabelsAreaX1
    set displayLabelsAreaY2 $displayLabelsAreaY1
    $c delete area
}

proc displayLabelsItemStroke {c x y} {
    global displayLabelsAreaX1 displayLabelsAreaY1 displayLabelsAreaX2 displayLabelsAreaY2
    $c delete area
    if {($displayLabelsAreaX1 != $x) && ($displayLabelsAreaY1 != $y) && ($x>=0)} {
        set x [$c canvasx $x]
        set y [$c canvasy $y]
	$c addtag area withtag [$c create rect $displayLabelsAreaX1 $displayLabelsAreaY1 $x $y -outline blue ]
	set displayLabelsAreaX2 $x
	set displayLabelsAreaY2 $y
    }
}

proc displayLabelsSetArea { x0 y0 x1 y1 } {
  global displayLabelsAreaX1 displayLabelsAreaY1 displayLabelsAreaX2 displayLabelsAreaY2
  set displayLabelsAreaX1 $x0
  set displayLabelsAreaX2 $x1
  set displayLabelsAreaY1 $y0
  set displayLabelsAreaY2 $y1
}

proc displayLabelsHelp { } {
  global displayLabelsHelpMessage
  toplevel .displayLabelsHelp
  wm title .displayLabelsHelp "HELP for displayLabels"
  text .displayLabelsHelp.t -yscrollcommand ".displayLabelsHelp.s set" -setgrid true
  scrollbar .displayLabelsHelp.s -relief flat -command ".displayLabelsHelp.t yview"
  .displayLabelsHelp.t insert 0.0 $displayLabelsHelpMessage
  pack .displayLabelsHelp.s -side right -fill y
  pack .displayLabelsHelp.t -expand yes -fill both
  button .displayLabelsHelp.close -text close -command "destroy .displayLabelsHelp"
  pack .displayLabelsHelp.close
}

proc displayLabelsBothHscroll { win from to } { 
  $win.frame.hscroll set $from $to
  $win.frame.canfra xview moveto $from
  $win.frame.cansig xview moveto $from
}

proc displayLabelsDetailBothHscroll { win from to } { 
  $win.hscroll set $from $to
  $win.botFrame.canfra xview moveto $from
  $win.topFrame.cansco xview moveto $from
}

proc displayLabelsDetailBothVscroll { win from to } { 
  $win.frame.vscroll set $from $to
  $win.frame.cansta yview moveto $from
}


# ------------------------------------------------------------ #
# The following function defines what will be displayed in the #
# scores canvas on top of the main canvas. The default setting #
# is the local acoustic score (actually the first 4 characters #
# of it). You can redefine the procedure for displaying other  #
# data that are available in the given cell argument.          #
# ------------------------------------------------------------ #

proc displayLabelsScore cell {

  set value [lindex $cell 5]
  return [string range [expr $value] 0 3]
}

# ------------------------------------------------------------ #
# The following function defines what will be displayed under  #
# the state-circle in the detailed display canvas. The return  #
# value of this function is a list of six items. If the zoom   #
# for the detailed display is set to 1/1 then only the first   #
# item will be displayed, if the zoom is set to 2/1 then the   #
# fist three items are displayed, if the zoom is set to 4/1    #
# then all six items will be displayed, if the zoom is below   #
# 1/1 then nothing will be displayed. If less than four items  #
# are returned, the missing items are considered to be empty.  #
# You can redefine this procedure to display whatever data is  #
# available in the given cell argument. The argument zoom can  #
# be used to create different list depending on the current    #
# zoom value (which is one of 1/4 1/2 1/1 2/1 4/1).            #
# ------------------------------------------------------------ #

proc displayLabelsBelowCircle { cell zoom } {

  set returnValue ""
  if { [expr $zoom <= 1] } {
    lappend returnValue [string range [expr [lindex $cell 11]] 0 4] ;# cumulative score
  } else {
    lappend returnValue [expr [lindex $cell 11]] ;# cumulative score
  }
  lappend returnValue [expr [lindex $cell  0]] ;# best score in this frame
  lappend returnValue [expr [expr [lindex $cell 11]] - [expr [lindex $cell  0]]] ;# minimal beam
  lappend returnValue "trans pen: [expr [lindex $cell 12]]" ;# transition penalty ('to here', not 'out of here')
  lappend returnValue "HMM Word : [lindex $cell 4]"         ;# HMM word index
  lappend returnValue "HMM Phone: [lindex $cell 3]"         ;# HMM phone index
}


# ------------------------------------------------------------ #
# The following procedure opens a toplevel window which shows  #
# all available information about a state. It's called when a  #
# state circle is clicked in the detailed view window.         #
# ------------------------------------------------------------ #

proc displayLabelsTrans { prep trans stateX frameX } {

  set name [lindex [lindex $prep 0] 0]
  set win [format ".label%s" $name ]
  set tra $win-$stateX-$frameX-trans

  toplevel $tra
  wm title $tra "TRANSITIONS ($stateX,$frameX)"

  frame $tra.top
  frame $tra.top.off
  frame $tra.top.pen
  frame $tra.top.gto

  button $tra.close -text "CLOSE" -width 10 -command "destroy $tra"

  label  $tra.top.off.head -text "OFFSET"   -width 10 -relief flat
  label  $tra.top.pen.head -text "PENALTY"  -width 16 -relief flat
  label  $tra.top.gto.head -text "GOING TO" -width 16 -relief flat

  pack $tra.top.off.head -side top
  pack $tra.top.pen.head -side top
  pack $tra.top.gto.head -side top

  set traX 0
  foreach transition $trans {

    set hmmFromStateX [lindex [lindex [lindex $prep 3] $stateX] 1]
    set hmmToStateX   [expr $hmmFromStateX + [lindex $transition 0]]
    set hmmToSenone   [lindex [lindex $prep 4] $hmmToStateX]

    label $tra.top.off.$traX -text [lindex $transition 0] -width 10 -relief sunken
    label $tra.top.pen.$traX -text [lindex $transition 1] -width 16 -relief sunken
    label $tra.top.gto.$traX -text $hmmToSenone           -width 16 -relief sunken
    pack $tra.top.off.$traX -side top
    pack $tra.top.pen.$traX -side top
    pack $tra.top.gto.$traX -side top

    incr traX
  }

  pack $tra.top.off -side left
  pack $tra.top.pen -side left
  pack $tra.top.gto -side left

  pack $tra.top -side top
  pack $tra.close -side top -fill x

}


# ======================================================================== #
# The following procedure will display all available information about the #
# the given state in an extra window (quite simple, no fancy stuff etc.).  #
# ======================================================================== #

proc displayLabelsState { prep state frameX } {

  set name [lindex [lindex $prep 0] 0]
  set win [format ".label%s" $name ]
  set stateX [lindex $state 8]
  set stw $win-$stateX-$frameX

  toplevel $stw
  wm title $stw "STATE $stateX at FRAME $frameX"

  frame $stw.var
  frame $stw.val

  label $stw.var.bestScore -text "best score in frame ..."  -relief flat
  label $stw.var.hmmStateX -text "state index in HMM ...."  -relief flat
  label $stw.var.hmmPhoneX -text "phone index in HMM ...."  -relief flat
  label $stw.var.hmmWordX  -text "word index in HMM ....."  -relief flat
  label $stw.var.senoneX   -text "senone index .........."  -relief flat
  label $stw.var.senone    -text "senone ................"  -relief flat
  label $stw.var.score     -text "acoustic score ........"  -relief flat
  label $stw.var.stateX    -text "relative state index .."  -relief flat
  label $stw.var.frameX    -text "frame index ..........."  -relief flat
  label $stw.var.cumScore  -text "cumulative score ......"  -relief flat
  label $stw.var.needBeam  -text "minimal beam .........."  -relief flat
  label $stw.var.transPen  -text "trans penalty to here ."  -relief flat

  label $stw.val.bestScore -text [lindex $state 0]      -width 20 -relief sunken
  label $stw.val.hmmStateX -text [lindex $state 1]      -width 20 -relief sunken
  label $stw.val.senoneX   -text [lindex $state 2]      -width 20 -relief sunken
  label $stw.val.hmmPhoneX -text [lindex $state 3]      -width 20 -relief sunken
  label $stw.val.hmmWordX  -text [lindex $state 4]      -width 20 -relief sunken
  label $stw.val.score     -text [lindex $state 5]      -width 20 -relief sunken
  label $stw.val.senone    -text [lindex $state 6]      -width 20 -relief sunken
  label $stw.val.stateX    -text [lindex $state 8]      -width 20 -relief sunken
  label $stw.val.frameX    -text $frameX                -width 20 -relief sunken
  label $stw.val.cumScore  -text [lindex $state 11]     -width 20 -relief sunken
  label $stw.val.needBeam  -text [expr [lindex $state 11] - [lindex $state 0]] -width 20 -relief sunken
  label $stw.val.transPen  -text [lindex $state 12]     -width 20 -relief sunken

  pack $stw.var.frameX -side top
  pack $stw.var.hmmStateX -side top
  pack $stw.var.stateX -side top
  pack $stw.var.hmmPhoneX -side top
  pack $stw.var.hmmWordX -side top
  pack $stw.var.senoneX -side top
  pack $stw.var.senone -side top
  pack $stw.var.score -side top
  pack $stw.var.bestScore -side top
  pack $stw.var.cumScore -side top
  pack $stw.var.needBeam -side top
  pack $stw.var.transPen -side top

  pack $stw.val.frameX -side top
  pack $stw.val.hmmStateX -side top
  pack $stw.val.stateX -side top
  pack $stw.val.hmmPhoneX -side top
  pack $stw.val.hmmWordX -side top
  pack $stw.val.senoneX -side top
  pack $stw.val.senone -side top
  pack $stw.val.score -side top
  pack $stw.val.bestScore -side top
  pack $stw.val.cumScore -side top
  pack $stw.val.needBeam -side top
  pack $stw.val.transPen -side top

  button $stw.var.close -text close -command "destroy $stw"
  pack $stw.var.close -side bottom -fill x

  button $stw.val.trans -text transitions -command "displayLabelsTrans \"$prep\" \"[lindex $state 7]\" $frameX $stateX"
  pack $stw.val.trans -side bottom -fill x

  pack $stw.var -side left
  pack $stw.val -side left
}


# =========================================================================== #
# The following procedure is responsible for the detailed view of the labels. #
# It's called automatically when an area in the utterance window is selected. #
# =========================================================================== #

proc displayLabelsZoomInArea { prep pad } {

  global displayLabelsAreaX1 displayLabelsAreaX2 displayLabelsParams
  set name [lindex [lindex $prep 0] 0]
  set win [format ".label%s" $name ]

  set font $displayLabelsParams($name,font)

  # -------------------------------------------------------------------- #
  # check if the zoomed area is non-empty and possibly switch left/right #
  # -------------------------------------------------------------------- #

  if { $displayLabelsAreaX1 == $displayLabelsAreaX2 } return
  if { $displayLabelsAreaX1 >  $displayLabelsAreaX2 } { 
     set h $displayLabelsAreaX1; set displayLabelsAreaX1 $displayLabelsAreaX2; set displayLabelsAreaX2 $h }

  # --------------------------------------------------------------- #
  # compute the selected frame indices from the given pixel indices #
  # --------------------------------------------------------------- #

  set fromFrame [expr int (($displayLabelsAreaX1 - $pad) / $displayLabelsParams($name,zoom))     ]
  set   toFrame [expr int (($displayLabelsAreaX2 - $pad) / $displayLabelsParams($name,zoom) - 1) ]

  if 0 { puts "zoom frames ($fromFrame ... $toFrame)" }
  displayLabelsItemStroke $win.frame.canvas -1 0

  # -------------------------------------------------------------------- #
  # compute the number of states in that area and the size of the canvas #
  # -------------------------------------------------------------------- #

  set area [lrange [lindex $prep 3] $fromFrame $toFrame]

  set fromState [lindex [lindex $area 0] 8]
  set   toState [lindex [lindex $area [expr [llength $area] -1]] 8]

  set boxSize [expr 32 * $displayLabelsParams($name,zood)]

  set height [expr ($toState-$fromState+1) * $boxSize ]
  set width  [expr ($toFrame-$fromFrame+1) * $boxSize ]

  if 0 { puts "height $height, width $width" }

  # --------------------------- #
  # create new top-level window #
  # --------------------------- #

  set det $win-$fromFrame-$toFrame
  toplevel $det
  wm minsize $det 200 200
  wm title $det "LABELS: [lindex [lindex $prep 0] 0] from $fromFrame to $toFrame"

  # ---------------------------------------- #
  # create frame and scrollbars and canvases #
  # ---------------------------------------- #

  set scoHeight 32;	# height of score canvas
  set staWidth  64;     # width of state/senone canvas
  set scrWidth  10;	# full width of scrollbars
  set fraHeight 10;     # height of frame-number canvas 

  # ----------------------------------------------------- #
  # top frame = close-button, scores-canvas, filler-frame #
  # ----------------------------------------------------- #

  frame  $det.topFrame  ;# frame on top that holds close-button, scores-canvas, and a filler-frame
  frame  $det.topFrame.upLeft  -width [expr $staWidth +  0] -height [expr $scoHeight + 0]
  frame  $det.topFrame.upRight -width [expr $scrWidth + 10] -height [expr $scoHeight + 0]

  canvas  $det.topFrame.upLeft.close -width [expr $staWidth + 0] -height [expr $scoHeight + 0]
          $det.topFrame.upLeft.close create text 5 5 -text "CLOSE" -anchor nw
  pack    $det.topFrame.upLeft.close -side top
          $det.topFrame.upLeft.close bind all <Button-1> "destroy $det"

  pack $det.topFrame.upLeft -side left
  pack $det.topFrame        -side top -fill x

  # -------------------------------------------------------------------------- #
  # bottom frame = filler-frame, frame-numbers-canvas, filler-frame, scrollbar #
  # -------------------------------------------------------------------------- #

  frame  $det.botFrame  ;# frame on bottom that holds frame-number-canvas, and a filler-frame
  frame  $det.botFrame.lowLeft  -width [expr $staWidth +  0] -height [expr $fraHeight + 0]
  frame  $det.botFrame.lowRight -width [expr $scrWidth +  0] -height [expr $fraHeight + 0]

  canvas  $det.botFrame.lowLeft.can  -width [expr $staWidth + 0] -height [expr $fraHeight + 0]
  canvas  $det.botFrame.lowRight.can -width [expr $scrWidth + 0] -height [expr $fraHeight + 0]
  pack    $det.botFrame.lowLeft.can  
  pack    $det.botFrame.lowRight.can 

  # ------------------------------------------------ #
  # middle frame, main canvas and senone-name-canvas #
  # ------------------------------------------------ #

  frame  $det.frame -relief flat -bd 1
  pack   $det.frame -side top -expand yes -fill both
  scrollbar $det.frame.vscroll         -width $scrWidth -relief sunken -command "$det.frame.canvas yview"
  scrollbar $det.hscroll -orient horiz -width $scrWidth -relief sunken -command "$det.frame.canvas xview"
  pack $det.frame.vscroll -side right  -fill y

  set c  $det.frame.canvas
  set cs $det.frame.cansta
  set ca $det.topFrame.cansco
  set cf $det.botFrame.canfra

  set pad 10; # use our own padding here

  set xScrollCommand "displayLabelsDetailBothHscroll $det"
  set yScrollCommand "displayLabelsDetailBothVscroll $det"

  canvas $cs -scrollregion "0 0 0 [expr $height+2*$pad]" -yscrollcommand "$det.frame.vscroll set" -width $staWidth -bd 0
  pack $cs -fill y -side left

  canvas $ca -scrollregion "0 0 [expr $width+2*$pad] 0" -xscrollcommand "$det.hscroll set" -height $scoHeight -bd 0
  pack $ca -fill x -side left -expand yes
  pack $det.topFrame.upRight -side left

  canvas $c -scrollregion "0 0 [expr $width+2*$pad] [expr $height+2*$pad]" \
	    -xscrollcommand $xScrollCommand -yscrollcommand $yScrollCommand  -bd 0

  canvas $cf -scrollregion "0 0 [expr $width+2*$pad] 0" -xscrollcommand "$det.hscroll set" -height 10  -bd 0

  pack $c -expand yes -fill both

  pack $det.botFrame.lowLeft -side left
  pack $cf -fill x -side left -expand yes
  pack $det.botFrame.lowRight -side left
  pack $det.botFrame -side top -fill x
  pack $det.hscroll -side bottom -fill x

  bind $c <2>         "$c scan mark   %x %y"
  bind $c <B2-Motion> "$c scan dragto %x %y"

  $c  yview moveto 1.0
  $cs yview moveto 1.0

  # --------------------- #
  # draw frame/state grid #
  # --------------------- #

  for { set frameX $fromFrame } { $frameX <= [expr $toFrame+1] } { incr frameX } {
    set x0 [expr $pad + (($frameX-$fromFrame) * $boxSize) ]
    $c create line $x0 $pad $x0 [expr $height+$pad] -stipple gray50
  }

  for { set stateX $fromState } { $stateX <= [expr $toState+1] } { incr stateX } {
    set y0 [expr $height + $pad - (($stateX-$fromState) * $boxSize) ]
    $c create line $pad $y0 [expr $width+$pad] $y0 -stipple gray50
  }

  # ----------------------------------- #
  # write frame numbers in frame canvas #
  # ----------------------------------- #

  for { set frameX [expr $fromFrame+5-$fromFrame%5] } { $frameX <= $toFrame } { set frameX [expr $frameX+5] } {
    set x0 [expr $pad + (($frameX-$fromFrame) * $boxSize) + 2]
    $cf create text $x0 10 -text "$frameX" -anchor sw -font $font
  }

  # ---------------------------- #
  # write scores in score canvas #
  # ---------------------------- #

  set frameX $fromFrame
  foreach state $area {
    set lscore [displayLabelsScore $state]
    set x0 [expr $pad + (($frameX-$fromFrame) * $boxSize)]
    $ca create text [expr $x0 + 3] [expr ($scoHeight/3)*($frameX%3)]  -text "$lscore" -anchor nw -font $font
    $ca create line [expr $x0 + 1] [expr ($scoHeight/3)*($frameX%3)] [expr $x0 + 1] $scoHeight -stipple gray50
    incr frameX
  }

  # ------------------------------------- #
  # draw contents for every visited state #
  # ------------------------------------- #
 
  set stateX -1
  set frameX $fromFrame
  foreach state $area {

    set lastStateX $stateX
    set stateX [lindex $state 8]

    # -------------------------- #
    # draw circle for each state #
    # -------------------------- #

    set x0 [expr           $pad + (($frameX-$fromFrame) * $boxSize) + $boxSize/2]
    set y0 [expr $height + $pad - (($stateX-$fromState) * $boxSize) - $boxSize/2]
    set oval [$c create oval [expr $x0+1] [expr $y0-1] [expr $x0+$boxSize/2-1] [expr $y0-$boxSize/2+1] -fill gray50]
    $c bind $oval <Button-1> "displayLabelsState \"$prep\" \"$state\" $frameX"

    # --------------------------------------------------------- #
    # draw light arrows for all transitions that could be taken #
    # --------------------------------------------------------- #

    set maxDisplayStep 2

    if { $stateX != $toState } {

      foreach trans [lindex $state 7] {

        set step [lindex $trans 0]

        set x0 [expr           $pad + (($frameX-$fromFrame) * $boxSize) + $boxSize - 1 ]
        set y0 [expr $height + $pad - (($stateX-$fromState) * $boxSize) - $boxSize*3/4 ]
        set x1 [expr $x0+$boxSize/2+2]
        set y1 [expr $y0-$boxSize*$step]

        if { $step > 0 } { 
          set x0 [expr           $pad + (($frameX-$fromFrame) * $boxSize) + $boxSize*3/4 ]
          set y0 [expr $height + $pad - (($stateX-$fromState) * $boxSize) - $boxSize + 1 ]
        }

	if { $step <= $maxDisplayStep } { $c create line $x0 $y0 $x0 $y1 $x1 $y1 -smooth on -arrow last -stipple gray50 }
      }
    }

    # ------------------------------------------------ #
    # draw the arrow for the transition that was taken #
    # ------------------------------------------------ #

    if { $lastStateX != -1 } {

      set step [expr $stateX-$lastStateX]

      set x0 [expr           $pad + (($frameX    -$fromFrame-1) * $boxSize) + $boxSize - 1 ]
      set y0 [expr $height + $pad - (($lastStateX-$fromState  ) * $boxSize) - $boxSize*3/4 ]
      set x1 [expr $x0+$boxSize/2+2]
      set y1 [expr $y0-$boxSize*$step]

      if { $step > 0 } { 
        set x0 [expr           $pad + (($frameX    -$fromFrame-1) * $boxSize) + $boxSize*3/4 ]
        set y0 [expr $height + $pad - (($lastStateX-$fromState  ) * $boxSize) - $boxSize + 1 ]
      }

      $c create line $x0 $y0 $x0 $y1 $x1 $y1 -smooth on -arrow last
    }

    # ---------------------------- #
    # write stuff under the circle #
    # ---------------------------- #

    set lineStep 10
    set zoom $displayLabelsParams($name,zood) 
    set data [displayLabelsBelowCircle $state $zoom]
    set x0 [expr           $pad + (($frameX-$fromFrame) * $boxSize)              + $zoom ]
    set y0 [expr $height + $pad - (($stateX-$fromState) * $boxSize) - $boxSize/2 + 1]

    if { [expr $zoom >= 1 ] } { $c create text $x0          $y0                    -text [lindex $data 0] -font $font -anchor nw }
    if { [expr $zoom >= 2 ] } { $c create text [expr $x0+1] [expr $y0+$lineStep  ] -text [lindex $data 1] -font $font -anchor nw }
    if { [expr $zoom >= 2 ] } { $c create text [expr $x0+2] [expr $y0+$lineStep*2] -text [lindex $data 2] -font $font -anchor nw }
    if { [expr $zoom >= 4 ] } { $c create text [expr $x0+2] [expr $y0+$lineStep*3] -text [lindex $data 3] -font $font -anchor nw }
    if { [expr $zoom >= 4 ] } { $c create text [expr $x0+2] [expr $y0+$lineStep*4] -text [lindex $data 4] -font $font -anchor nw }
    if { [expr $zoom >= 4 ] } { $c create text [expr $x0+2] [expr $y0+$lineStep*5] -text [lindex $data 5] -font $font -anchor nw }

    incr frameX
  }

  # ---------------------------------------------------- #
  # write state numbers and senone names in state canvas #
  # ---------------------------------------------------- #

  for { set stateX $fromState } { $stateX <=  $toState } { incr stateX } {
    set y0 [expr $height + $pad - (($stateX-$fromState) * $boxSize) ]
    $cs create line 0 $y0 $staWidth $y0 -stipple gray50
    $cs create text 1 $y0 -text [lindex [lindex $prep 5] $stateX] -anchor sw -font $font
  }
}


# =========================================================================== #
# Open a window for selecting a feature set to be displayed.                  #
# =========================================================================== #

proc displayLabelsSelect { name what where title } {

    global displayLabelsParams
    
    set w .${name}_fs

    catch {destroy $w}
    toplevel $w
    wm title $w $title
    wm minsize $w 1 1

    frame   $w.frame 
    frame   $w.b     
    button  $w.b.ok  -text "close selection"  -command "destroy $w"
    frame   $w.cur
    label   $w.cur.text -text "selected: "
    label   $w.cur.val -textvariable displayLabelsParams($name,$where)
    pack $w.cur.text $w.cur.val -side left
    pack $w.b.ok -fill x
    pack $w.frame -side top -fill y 
    pack $w.cur -side top -fill x
    pack $w.b -side bottom -fill x

    scrollbar $w.frame.xscroll -command "$w.frame.list xview" -orient horizontal
    scrollbar $w.frame.yscroll -command "$w.frame.list yview"
    listbox $w.frame.list -width 50 -yscroll "$w.frame.yscroll set" -xscroll "$w.frame.xscroll set" -height 5 -selectmode single
    eval { $w.frame.list insert end } $what
    pack $w.frame.yscroll -side right  -fill y
    pack $w.frame.xscroll -side bottom -fill x
    pack $w.frame.list   -side left -fill both 

    $w.frame.list selection set 0

    bind $w.frame.list <Button-1> "$w.frame.list select anchor \[$w.frame.list nearest %y\]"
    bind $w.frame.list <ButtonRelease-1> "$w.frame.list select set anchor \[$w.frame.list nearest %y\] ; set displayLabelsParams($name,$where) \[lindex \[list $what\] \[$w.frame.list curselection\]\]"

}

proc displayLabelsFirstFeature { name hmm } {

    global displayLabelsParams
    set sns $hmm.stateGraph.senoneSet
    if { [set featSetN [$sns configure -featSetN]] <= 0 } return
    set displayLabelsParams($name,featureSet) [list $sns.featureSet(0) [$sns.featureSet(0) configure -name] ]
    set displayLabelsParams($name,feature)    [lindex [[lindex $displayLabelsParams($name,featureSet) 0]] 0]
}

proc displayLabelsSelectFeatureSet { name hmm } {

    set sns $hmm.stateGraph.senoneSet

    if { [set featSetN [$sns configure -featSetN]] <= 0 } return
    set featSets {}
    for { set featSetX 0 } { $featSetX < $featSetN } { incr featSetX } {
	lappend featSets [list $sns.featureSet($featSetX) [$sns.featureSet($featSetX) configure -name ] ]
    }
    displayLabelsSelect $name $featSets featureSet "Select FeatureSet"
}


proc displayLabelsSelectFeature { name hmm } {

    global displayLabelsParams

    set featureSet [lindex $displayLabelsParams($name,featureSet) 0]
    set features [$featureSet]

    displayLabelsSelect $name $features feature "Select Feature"
}


# =========================================================================== #
# The following procedure is for displaying the utterance window. It accepts  #
# the prepared labels list and uses the global array displayLabelsParams for  #
# deciding what to display. It will be called automatically whenever the zoom #
# value is changed in the zoom-menu. Clicking on a word or phone rectangle or #
# selecting an area will show the selection in detail in an extra window, by  #
# calling the procedure displayLabelsZoomInArea.                              #
# =========================================================================== #

proc displayLabelsUttWindow { prep } {

  set name [lindex [lindex $prep 0] 0]
  global displayLabelsParams
  set zoom $displayLabelsParams($name,zoom).0; # make it floating point

  # ------------------------------------------------------------- #
  # get lists for words, phones, and cells in their own variables #
  # ------------------------------------------------------------- #

  set  wordLabels [ lindex $prep 1 ]
  set  phonLabels [ lindex $prep 2 ]
  set  cellLabels [ lindex $prep 3 ]

  # ------------------------------------ #
  # get frameN and stateN from last word #
  # ------------------------------------ #

  set  frameN [expr [lindex [lindex [lindex $wordLabels [expr [llength $wordLabels]-1]] 1] 1] + 1]
  set  stateN [expr [lindex [lindex [lindex $wordLabels [expr [llength $wordLabels]-1]] 2] 1] + 1]

  # -------------------- #
  # compute canvas sizes #
  # -------------------- #

  set pad $displayLabelsParams($name,pad)

  set height [expr $stateN * ($zoom)]
  set width  [expr $frameN * ($zoom)]

  # ---------------------------------- #
  # create canvas for displying labels #
  # ---------------------------------- #

  set win [format ".label%s" $name ]
  set c   [format "%s.frame.canvas" $win ]
  set cf  [format "%s.frame.canfra" $win ]
  set cs  [format "%s.frame.cansig" $win ]

  catch { destroy $c }
  catch { destroy $cf }
  catch { destroy $cs }

  set xScrollCommand "displayLabelsBothHscroll $win"
  if { $displayLabelsParams($name,sign) } { set featHeight $displayLabelsParams($name,sigh) } else { set featHeight 1 }
  canvas $cs -scrollregion "0 0 [expr $width+2*$pad] 0" -xscrollcommand "$win.frame.hscroll set" -height $featHeight
  pack $cs -fill x -side top

  canvas $c -scrollregion "0 0 [expr $width+2*$pad] [expr $height+2*$pad]" \
	    -xscrollcommand $xScrollCommand -yscrollcommand "$win.frame.vscroll set"
  pack $c -expand yes -fill both

  $c yview moveto 1.0

  canvas $cf -scrollregion "0 0 [expr $width+2*$pad] 0" -xscrollcommand "$win.frame.hscroll set" -height 10
  pack $cf -fill x -side bottom

  # ---------------------------------------------- #
  # button-2 = drag canvas, button-3 = select area #
  # ---------------------------------------------- #

  bind $c <2>         "$c scan mark               %x %y"
  bind $c <B2-Motion> "$c scan dragto             %x %y"
  bind $c <3>         "displayLabelsItemMark   $c %x %y"
  bind $c <B3-Motion> "displayLabelsItemStroke $c %x %y"
  bind $c <B3-ButtonRelease> "displayLabelsZoomInArea \"$prep\" $pad"

  # ----------------------- #
  # display word rectangles #
  # ----------------------- #

  if { $displayLabelsParams($name,wrec) } {

    set wordX 0
    foreach word $wordLabels {

      set wordSpell [lindex $word 0]

      # ------------------------------------------------ #
      # get the word's position in the frame/state space #
      # ------------------------------------------------ #

      set fromFrame [lindex [lindex $word 1] 0]
      set   toFrame [lindex [lindex $word 1] 1] 
      set fromState [lindex [lindex $word 2] 0] 
      set   toState [lindex [lindex $word 2] 1]

      # ---------------------------------- #
      # compute the rectangle's boundaries #
      # ---------------------------------- #
    
      set x0 [ expr $pad +           ($zoom) * ($fromFrame   ) ]
      set x1 [ expr $pad +           ($zoom) * (  $toFrame +1) ]
      set y0 [ expr $pad + $height - ($zoom) * ($fromState   ) ]
      set y1 [ expr $pad + $height - ($zoom) * ( $toState  +1) ]

      # --------------------------------------- #
      # draw the rectangle ad give it a binding #
      # --------------------------------------- #

      set rect [ $c create rectangle $x0 $y0 $x1 $y1 -fill yellow ]
      $c bind $rect <Button-1> "displayLabelsSetArea $x0 $y0 $x1 $y1; displayLabelsZoomInArea \"$prep\" $pad"

      # ------------------------------------------- #
      # write framerange and word's name if desired #
      # ------------------------------------------- #

      if { $displayLabelsParams($name,wnam) } {
        if { $wordSpell != "SIL" } { $c create text $x1 $y0 -text " $wordSpell" -anchor sw -font $displayLabelsParams($name,font) }
      }
      if { $displayLabelsParams($name,wfra) } {
        if { $wordSpell != "SIL" } { $c create text $x0 $y1 -text "$fromFrame..$toFrame " -anchor ne -font $displayLabelsParams($name,font)}
      }

      # --------------------------- #
      # that's it, do the next word #
      # --------------------------- #

      incr wordX
    }
  }

  # ------------------------------------------------------- #
  # done with word rectangles, now display phone rectangles #
  # ------------------------------------------------------- #

  if { $displayLabelsParams($name,prec) } {

    set phonX 0
    foreach phon $phonLabels {

      set phonSpell [lindex $phon 0]

      # ------------------------------------------------- #
      # get the phone's position in the frame/state space #
      # ------------------------------------------------- #

      set fromFrame [lindex [lindex $phon 1] 0]
      set   toFrame [lindex [lindex $phon 1] 1] 
      set fromState [lindex [lindex $phon 2] 0] 
      set   toState [lindex [lindex $phon 2] 1]

      # ---------------------------------- #
      # compute the rectangle's boundaries #
      # ---------------------------------- #
    
      set x0 [ expr $pad +           ($zoom) * ($fromFrame   ) ]
      set x1 [ expr $pad +           ($zoom) * (  $toFrame +1) ]
      set y0 [ expr $pad + $height - ($zoom) * ($fromState   ) ]
      set y1 [ expr $pad + $height - ($zoom) * ( $toState  +1) ]

      # --------------------------------------- #
      # draw the rectangle ad give it a binding #
      # --------------------------------------- #

      set rect [ $c create rectangle $x0 $y0 $x1 $y1 -fill orange ]
      $c bind $rect <Button-1> "displayLabelsSetArea $x0 $y0 $x1 $y1; displayLabelsZoomInArea \"$prep\" $pad"

      # ------------------------------------------------ #
      # write framerange and the phone's name if desired #
      # ------------------------------------------------ #

      if { $displayLabelsParams($name,pnam) } {$c create text $x1 $y0 -text " $phonSpell" -anchor sw -font $displayLabelsParams($name,font)}
      if { $displayLabelsParams($name,pfra) } {$c create text $x0 $y1 -text "$fromFrame..$toFrame " -anchor ne -font $displayLabelsParams($name,font)}

      # ---------------------------- #
      # that's it, do the next phone #
      # ---------------------------- #

      incr phonX
    }
  }

  # --------------------------------------------------- #
  # done with phone rectangles, now display frame lines #
  # --------------------------------------------------- #

  set frameLineBase 80.0

  set frameLineDist [ expr $frameLineBase / ($zoom) ]

  set firstFrame [ lindex [lindex [lindex $wordLabels 0] 1] 0 ]
  for { set frameX [ expr int($firstFrame+$frameLineDist - ($firstFrame+$frameLineDist)) % int($frameLineDist) ] } \
      { $frameX < $frameN } \
      { set frameX [ expr $frameX + $frameLineDist] } {
    if { $displayLabelsParams($name,fral) } {
      $c  create line [ expr $pad + ($zoom) * $frameX ] [ expr $height + 2*$pad ] [ expr $pad + ($zoom) * $frameX ] 0 -stipple gray50
    }
    $cf create text [ expr $pad + ($zoom) * $frameX -10 ] 10 -text "$frameX" -anchor sw -font $displayLabelsParams($name,font)
  }

  # ----------------------------------------------------- #
  # done with displaying frame lines, now display feature #
  # ----------------------------------------------------- #
 
  set featureSet [lindex $displayLabelsParams($name,featureSet) 0]
  set feature    $displayLabelsParams($name,feature)

  if { $displayLabelsParams($name,sign) && [expr ($zoom) >= 1.0] } {
     $featureSet display $feature $cs -dx [expr int($zoom)] \
       -height $displayLabelsParams($name,sigh) -grey 1 -space 0 -x $pad ;# -from 0 -to $frameN
   }
}

proc displayLabelsUpdateFeature name {

  global displayLabelsParams
  set zoom $displayLabelsParams($name,zoom).0; # make it floating point
    
  set cs [format ".label%s.frame.cansig" $name ]

  set featureSet [lindex $displayLabelsParams($name,featureSet) 0]
  set feature    $displayLabelsParams($name,feature)

  if { $displayLabelsParams($name,sign) && [expr ($zoom) >= 1.0] } {
     $featureSet display $feature $cs -dx [expr int($zoom)] \
       -height $displayLabelsParams($name,sigh) -grey 1 -space 0 -x $displayLabelsParams($name,pad) ;# -from 0 -to $frameN
   }
}

proc displayLabelsBothCan args {
  set win    [lindex $args 0]
  set moveto [lindex $args 1]
  set where  [lindex $args 2]
  if 0 { if { $moveto != "moveto" } { puts "hm? $moveto" } }
  $win.frame.canvas xview $moveto $where
  $win.frame.canfra xview $moveto $where
  $win.frame.cansig xview $moveto $where
}

proc displayLabelsInWindow { prep } {

  # ----------------------------------------------------- #
  # create top level window (will initally display words) #
  # ----------------------------------------------------- #

  set name [lindex [lindex $prep 0] 0]
  set hmm  [lindex [lindex $prep 0] 1]
  set win  [format ".label%s" $name ]

  global displayLabelsParams

  toplevel $win
  wm minsize $win 300 300
  wm title $win "LABELS: [lindex [lindex $prep 0] 0]"

  # --------------------------------------------------------- #
  # set up first available feature as default display feature #
  # --------------------------------------------------------- #

  displayLabelsFirstFeature $name $hmm

  # ------------------------------------- #
  # create control buttons and  zoom-menu #
  # ------------------------------------- #

  frame  $win.menu
  menubutton  $win.menu.action  -text action        -menu $win.menu.action.menu  -borderwidth 1 -relief raised
  menubutton  $win.menu.display -text "full-view"   -menu $win.menu.display.menu -borderwidth 1 -relief raised
# menubutton  $win.menu.details -text "detail-view" -menu $win.menu.details.menu -borderwidth 1 -relief raised
  menubutton  $win.menu.zoom    -text "full zoom"   -menu $win.menu.zoom.menu    -borderwidth 1 -relief raised
  menubutton  $win.menu.zoomd   -text "detail zoom" -menu $win.menu.zoomd.menu   -borderwidth 1 -relief raised
 
  menu        $win.menu.zoomd.menu
  foreach zoom { 1/4 1/2 1/1 2/1 4/1 } {
    $win.menu.zoomd.menu add radio -label $zoom -variable displayLabelsParams($name,zood) -value $zoom
  }

  menu        $win.menu.zoom.menu
  foreach zoom { 1/8 1/4 1/2 1/1 2/1 4/1 8/1 } {
    $win.menu.zoom.menu add radio -label $zoom -variable displayLabelsParams($name,zoom) \
				  -value $zoom -command "displayLabelsUttWindow {$prep}"
  }

  menu        $win.menu.action.menu
  $win.menu.action.menu add command -label "help"  -command "displayLabelsHelp"
  $win.menu.action.menu add command -label "close" -command "destroy $win"
  $win.menu.action.menu add command -label "FeatureSet"     -command "displayLabelsSelectFeatureSet $name $hmm"
  $win.menu.action.menu add command -label "Feature"        -command "displayLabelsSelectFeature    $name $hmm"
  $win.menu.action.menu add command -label "Update Feature" -command "displayLabelsUpdateFeature    $name"
  $win.menu.action.menu add command -label "Configurations" -command "displayLabelsConfigurations   $name"

  menu        $win.menu.display.menu
  $win.menu.display.menu add check -label "word rectangles"	-variable displayLabelsParams($name,wrec) -command "displayLabelsUttWindow {$prep}"
  $win.menu.display.menu add check -label "word names"		-variable displayLabelsParams($name,wnam) -command "displayLabelsUttWindow {$prep}"
  $win.menu.display.menu add check -label "word frame ranges"	-variable displayLabelsParams($name,wfra) -command "displayLabelsUttWindow {$prep}"
  $win.menu.display.menu add check -label "phone rectangles"	-variable displayLabelsParams($name,prec) -command "displayLabelsUttWindow {$prep}"
  $win.menu.display.menu add check -label "phone names"		-variable displayLabelsParams($name,pnam) -command "displayLabelsUttWindow {$prep}"
  $win.menu.display.menu add check -label "phone frame ranges"	-variable displayLabelsParams($name,pfra) -command "displayLabelsUttWindow {$prep}"
  $win.menu.display.menu add check -label "frame lines"		-variable displayLabelsParams($name,fral) -command "displayLabelsUttWindow {$prep}"
  $win.menu.display.menu add check -label "speech signal"	-variable displayLabelsParams($name,sign) -command "displayLabelsUttWindow {$prep}"

  pack $win.menu.action $win.menu.display $win.menu.zoom $win.menu.zoomd -side left 

  # -------------------------------------- #
  # create frame and scrollbars for canvas #
  # -------------------------------------- #

  frame  $win.frame -relief flat -bd 1
  pack   $win.menu -side top -fill x
  pack   $win.frame    -side top -expand yes -fill both
  scrollbar $win.frame.vscroll               -width 10 -relief sunken -command "$win.frame.canvas yview"
  scrollbar $win.frame.hscroll -orient horiz -width 10 -relief sunken -command "displayLabelsBothCan $win"
  pack $win.frame.vscroll -side right -fill y
  pack $win.frame.hscroll -side bottom -fill x

  # ------------------------------------------------- #
  # initialize standard display description variables #
  # ------------------------------------------------- #

  set displayLabelsParams($name,wrec) 1;	# rectangles for words
  set displayLabelsParams($name,wnam) 1;	# names (spelling) of words
  set displayLabelsParams($name,wfra) 1;	# frame ranges for words
  set displayLabelsParams($name,prec) 1;	# rectangles for phones
  set displayLabelsParams($name,pnam) 0;	# names of the phones
  set displayLabelsParams($name,pfra) 0;	# frames ranges for phones
  set displayLabelsParams($name,fral) 1;	# vertical lines for frames
  set displayLabelsParams($name,sign) 0;	# vertical lines for frames
  set displayLabelsParams($name,pad) 30;	# space to leave around path in canvas

  set displayLabelsParams($name,sigh) 32;	# height of signal display (in pixels)
  set displayLabelsParams($name,font) "6x9";	# what font to use for displaying stuff

  set displayLabelsParams($name,feature)    "(none)";   # what font to use for displaying stuff
  set displayLabelsParams($name,featureSet) "(none)";   # what font to use for displaying stuff

  displayLabelsFirstFeature $name $hmm

  $win.menu.zoomd.menu invoke 3

  # -------------------------------------------------- #
  # first display will use zomm number 4, which is 1/1 #
  # -------------------------------------------------- #

  $win.menu.zoom.menu invoke 4
}


proc displayLabelsConfigurations name {

  global displayLabelsParams
  set cw .${name}_configurations

  toplevel $cw
  wm title $cw Configurations

  foreach conf { { pad  "padding around graphics " } 
		 { sigh "signal display height   " } 
		 { font "font used for texts     " } } {
	set param [lindex $conf 0]
	frame $cw.$param
	label $cw.$param.l -text [lindex $conf 1]
	entry $cw.$param.e -textvariable displayLabelsParams($name,$param) -width 20
	pack  $cw.$param.l $cw.$param.e -side left
	pack  $cw.$param -side top
  }
  button $cw.close -text "close" -command "destroy $cw"
  pack $cw.close -fill x
}



proc displayLabelsLaprep { pathName hmmName } {

  # ------------------------------------------------------------ #
  # get labels for words, phones, senones in their own variables #
  # ------------------------------------------------------------ #

  set   wordLabels [ lindex [ $pathName labels $hmmName -what words   ] 1 ]
  set  phoneLabels [ lindex [ $pathName labels $hmmName -what phones  ] 1 ]
  set senoneLabels [ lindex [ $pathName labels $hmmName -what senones ] 1 ]

  set path [ $pathName ]

  # ---------------------------------------------------- #
  # get overall number of words, phones, senones, frames #
  # ---------------------------------------------------- #

  set   wordN [ llength $wordLabels   ]
  set  phoneN [ llength $phoneLabels  ]
  set senoneN [ llength $senoneLabels ]
  set frameN  [ lindex [ lindex [ lindex $wordLabels [ expr $wordN-1 ] ] 1 ] 1 ]

  set senonesAll  [ lindex [ $hmmName.stateGraph ] 0 ]
  set transAll    [ lindex [ $hmmName.stateGraph ] 1 ]

  set phoneAllIdx [ lindex [ $hmmName.stateGraph ] 4 ]
  set wordAllIdx  [ lindex [ $hmmName.stateGraph ] 5 ]

  # -------------------------------------------- #
  # replace state fields of every phone and word #
  # -------------------------------------------- #

  set newPhoneLabels ""
  set newWordLabels ""
  set newAmodelLabels ""
  set stateN 0
  set oldWordX 0
  set oldWordFirstState 0
  set oldWordPhoneN -1; # because of increment before use
  set oldWordFirstFrame [lindex [lindex [lindex $phoneLabels 0] 1] 0]

  foreach phone $phoneLabels {

    set firstFrame    [ lindex [ lindex $phone 1 ] 0 ]
    set hmmFirstState [ lindex [ lindex $phone 2 ] 0 ]
    set hmmLastState  [ lindex [ lindex $phone 2 ] 1 ]
    set hmmPhoneX     [ lindex [ lindex [ lindex $path [ expr $firstFrame+5 ] ] 4 ] 2 ]
    set amodel        [ lindex [ $hmmName.phoneGraph.stateGraph($hmmPhoneX) ] 0 ] 
    set phoneStateN   [ llength $amodel ]
    lappend newPhoneLabels [list [ lindex $phone 0 ] [ lindex $phone 1 ] [list $stateN [ expr $stateN+$phoneStateN-1 ] ] \
			         [list $hmmFirstState $hmmLastState $hmmPhoneX]]

    set newAmodelLabels [concat $newAmodelLabels "$amodel"]

    set newState($hmmFirstState)  $stateN

    set wordX [lindex $wordAllIdx [lindex [lindex $phone 2 ] 0]]
    incr oldWordPhoneN

    if { $wordX != $oldWordX } {

        lappend newWordLabels [list [lindex [lindex [$hmmName.wordGraph] 0] $oldWordX]\
			            [list $oldWordFirstFrame [expr [lindex [lindex $phone 1] 0] - 1 ] ]\
				    [list $oldWordFirstState [expr $stateN-1] $oldWordPhoneN ] ]
	set oldWordX $wordX
	set oldWordFirstState $stateN
	set oldWordPhoneN 0
	set oldWordFirstFrame [lindex [lindex $phone 1] 0]
    }
    set stateN [ expr $stateN + $phoneStateN ]
  }
  lappend newWordLabels [list [lindex [lindex [$hmmName.wordGraph] 0] $oldWordX] \
			      [list $oldWordFirstFrame [lindex [lindex $phone 1] 1] ] \
			      [list $oldWordFirstState [expr $stateN-1]] $oldWordPhoneN ]

  # ------------------------------------------------------------------------------ #
  # create verbose viterbi path using the same state, phone, word indices as above #
  # ------------------------------------------------------------------------------ #

  set oldPhoneX -1
  set stateX -1
  set oldHmmStateX [expr [lindex [lindex [lindex $path 5] 4] 0] + 1]; # will make first hmmStateJump to be -1
  set pathList ""
  set lastTransition ""
  set stateJump -1
  set cumScore 0.0

  foreach pathFrame [ lrange $path 5 [llength $path] ] {

    set cellList ""
    lappend cellList [lindex $pathFrame 2]
    if { [lindex $pathFrame 3] != 1 } { return "ERROR: this is not a Viterbi path." }
    set hmmStateX   [lindex [lindex $pathFrame 4] 0]  
    set senoneX     [lindex [lindex $pathFrame 4] 1]
    set hmmPhoneX   [lindex [lindex $pathFrame 4] 2]
    set hmmWordX    [lindex [lindex $pathFrame 4] 3]
    set lscore      [lindex [lindex $pathFrame 4] 7]
    set senoneName  [lindex $senonesAll $hmmStateX ]
    set transition  [lindex $transAll   $hmmStateX ]
    lappend cellList $hmmStateX $senoneX $hmmPhoneX $hmmWordX $lscore $senoneName $transition

    set cumScore [expr $cumScore + $lscore]
    set hmmStateJump [expr $hmmStateX - $oldHmmStateX]

    if { $oldPhoneX != $hmmPhoneX } {
      set stateX $newState($hmmStateX)
      set oldPhoneX $hmmPhoneX
    } else {
      set stateJump [expr $hmmStateX - $oldHmmStateX]
      set stateX [expr $stateX + $stateJump]
    }
    set oldHmmStateX $hmmStateX

    set transPen 0.0
    if { $hmmStateJump != -1 } {
      foreach trans $lastTransition {
        if { [lindex $trans 0] == $hmmStateJump } {
          set transPen [lindex $trans 1]
	  set cumScore [expr $cumScore + $transPen]
          break; # only take first matching transition into account
        }
      }
    }
    lappend cellList $stateX
    lappend cellList $stateJump
    lappend cellList $hmmStateJump
    lappend cellList $cumScore
    lappend cellList $transPen

    lappend pathList $cellList

    set lastTransition $transition
   
  }

  return [list [list $pathName $hmmName] $newWordLabels $newPhoneLabels $pathList $senonesAll $newAmodelLabels]
}

if {0} {
    proc displayLabels { path args } {
	itfParseArgv $path $args [ list [ list "<hmm>" object {} hmm HMM "underlying HMM object" ] ]
	displayLabelsFirstFeature $path $hmm
	displayLabelsInWindow [displayLabelsLaprep $path $hmm]
    }
}

proc displayLabels { args } {
    if {[catch {itfParseArgv displayLabels $args [ list [ 
        list "<path>" object {} path Path "underlying Path object" ] [
        list "<hmm>"  object {} hmm  HMM  "underlying HMM object" ] ]
    } ] } {
	return
    }
    
    displayLabelsFirstFeature $path $hmm
    displayLabelsInWindow     [displayLabelsLaprep $path $hmm]
}
