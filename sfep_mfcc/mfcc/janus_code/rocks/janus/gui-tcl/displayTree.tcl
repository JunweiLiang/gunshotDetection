# ========================================================================
#  JanusRTK   Janus Speech Recognition Toolkit
#             ------------------------------------------------------------
#             Object: Displaying of Tree objects
#             ------------------------------------------------------------
#             Time-stamp: <07 Jul 00 08:17:38 metze>
#
#  Author  :  Ivica Rogina
#  Module  :  treeDisplay.tcl
#  Date    :  2001-09-27
#
#  Remarks :  Also see showSTree.tcl
#
# ========================================================================
#  RCS info:  $Id: displayTree.tcl 2390 2003-08-14 11:20:32Z fuegen $
# 
#  $Log$
#  Revision 1.2  2003/08/14 11:19:39  fuegen
#  Merged changes on branch jtk-01-01-15-fms (jaguar -> ibis-013)
#
#  Revision 1.1.2.2  2002/11/04 15:02:07  metze
#  Added documentation code
#
#  Revision 1.1.2.1  2001/09/27 08:27:32  metze
#  Checked in old script
#
# ========================================================================


proc displayTree_Info { name } {
    switch $name {
	procs {
	    return "treeDisplay"
	}
	help {
	    return "The functions defined in this file allow you to view 
a Tree object in a Tk widget."
	}
	treeDisplay {
	    return "Displays a tree if you give it the name of the tree object
and the name of the root node to start from."
	}
    }
    return "???"
}


# =========================================================================== #
# treeDisplay.tcl							      #
# --------------------------------------------------------------------------- #
# Display a tree (distributions, topologies, durations) on a Tk canvas. This  #
# also allows some basic interactions with the tree, like finding good split- #
# questions and splitting a node. Polyphone lists can be displayed as well as #
# the underlying models. The display of the tree can be designed interactive- #
# ly by expanding or removing mouse-selected nodes.                           #
# The only important function to be used from 'outside' is treeDisplay which  #
# accepts two parameters, the name of the tree object and the name of a root  #
# node, which should be the first node to be displayed.                       #
# --------------------------------------------------------------------------- #
# All global variables and all procedure names start with "treeDisplay".      #
# The display is organized in cells (descrete positions on the canvas) in     #
# rows and columns.                                                           #
# =========================================================================== #
#
# display cell datastructure:
#
#	...$tree($r,$c,what)	what is in the cell (empty/node/ptree/...)
#	...$tree($r,$c,info)	additional information about the node
#	...$tree($r,$c,indx)	index of the node/ptree
#	...$tree($r,$c,prex)	indices of predecessor-nodes
#	...$tree($r,$c,prec)	columns of predecessors' display (one row up)
#	...$tree($r,$c,sucw)	what types do the succesor nodes have
#	...$tree($r,$c,sucx)	indices of successor-nodes
#	...$tree($r,$c,succ)	columns of successors' display (one row down)
#	...$tree($r,$c,text)	verbose information text about the node/ptree
#	...$tree($r,$c,wino)	names of the window objects displaying the cell
#	...$tree($r,$c,wini)	canvas item indices of the cell
#	...$tree($r,$c,suce)	canvas item indices of edges to sucessors
#
# other global data:
#
#	...$tree(rowN)		number of cell-rows to be displayed
#	...$tree(colN)		number of cell-columns to be displayed
#	...$tree(rowY,$r)	vertical pixel index for row $r
#	...$tree(colX,$c)	horizontal pixel index for column $c
#	...$tree(canvas)	Tk-name of the displaying canvas
#	...$tree(cWidth)	width of the canvas in pixels
#	...$tree(cHeight)	height of the canvas in pixels
#	...$tree(top)		Tk-name of the top-level window
#	...$tree(quest)		name of the question set for splitting
#	...$tree(minCount)	argument given to splitting function
#

# =========================================================================== #
# initialize the display datastructure                                        #
# =========================================================================== #

proc treeDisplayInitCell { tree row col } {

  upvar #0 treeDisplay$tree tr
  set tr($row,$col,what) "empty"	;# string
  set tr($row,$col,info) {}		;# list of anything
  set tr($row,$col,text) ""		;# string
  set tr($row,$col,indx) -1		;# integer
  set tr($row,$col,prex) {}		;# list of integers
  set tr($row,$col,prec) {}		;# list of integers
  set tr($row,$col,sucw) {}		;# list of strings
  set tr($row,$col,sucx) {}		;# list of integers
  set tr($row,$col,succ) {}		;# list of integers
  set tr($row,$col,suce) {}		;# list of integers
  set tr($row,$col,wino) {}		;# list of strings
  set tr($row,$col,wini) {}		;# list of integers
}


# =========================================================================== #
# initialize all global data with their default values                        #
# =========================================================================== #

proc treeDisplayInit { tree } {

  upvar #0 treeDisplay$tree tr

  set tr(rowN) 20
  set tr(colN) 9

  set tr(cHeight) 1200
  set tr(cWidth)  2400

  set tr(dX) [expr $tr(cWidth)  / $tr(colN)]
  set tr(dY) [expr $tr(cHeight) / $tr(rowN)]

  set tr(top)    ""
  set tr(canvas) ""

  set tr(quest) $tree.questionSet
  set tr(minCount) 100

  for {set row 0} {$row < $tr(rowN)} {incr row} {
    set tr(rowY,$row) [expr $row * $tr(dY)]
  }
  for {set col 0} {$col < $tr(colN)} {incr col} {
    set tr(colX,$col) [expr $col * $tr(dX)]
  }

  for {set row 0} {$row < $tr(rowN)} {incr row} {
    for {set col 0} {$col < $tr(colN)} {incr col} {
      treeDisplayInitCell $tree $row $col
    }
  }
}


# =========================================================================== #
# copy a cell's contents into another cell (in the same row)                  #
# =========================================================================== #

proc treeDisplayCopy { tree fromR fromC toR toC } {

  upvar #0 treeDisplay$tree tr
  set tr($toR,$toC,what) $tr($fromR,$fromC,what)
  set tr($toR,$toC,info) $tr($fromR,$fromC,info)
  set tr($toR,$toC,text) $tr($fromR,$fromC,text)
  set tr($toR,$toC,indx) $tr($fromR,$fromC,indx)
  set tr($toR,$toC,prex) $tr($fromR,$fromC,prex)
  set tr($toR,$toC,prec) $tr($fromR,$fromC,prec)
  set tr($toR,$toC,sucw) $tr($fromR,$fromC,sucw)
  set tr($toR,$toC,sucx) $tr($fromR,$fromC,sucx)
  set tr($toR,$toC,succ) $tr($fromR,$fromC,succ)
  set tr($toR,$toC,suce) $tr($fromR,$fromC,suce)
  set tr($toR,$toC,wino) $tr($fromR,$fromC,wino)
  set tr($toR,$toC,wini) $tr($fromR,$fromC,wini)
}

# =========================================================================== #
# move a cell to a new position in the canvas (stays in same row)             #
# =========================================================================== #

proc treeDisplayMove { tree row from to } {

  upvar #0 treeDisplay$tree tr

  # ------------------------------------------------------------------------
  # check if the target position is empty; if not, don't do the move
  # ------------------------------------------------------------------------
  if { $tr($row,$to,what) != "empty" } {
    puts "unexpint error: target ($row,$to) not empty but $tr($row,$to,what)"
    return
  }

  # ------------------------------------------------------------------------
  # check if the source position is empty; if so, don't do the move
  # ------------------------------------------------------------------------
  if { $tr($row,$from,what) == "empty" } return

  # ------------------------------------------------------------------------
  # notify predecessors, and redraw their edges
  # ------------------------------------------------------------------------
  set preRow [expr $row-1]
  foreach prec $tr($row,$from,prec) {
    set succX [lsearch $tr($preRow,$prec,succ) $from]
    set tr($preRow,$prec,succ) [
      lreplace $tr($preRow,$prec,succ) $succX $succX $to
    ]
    $tr(canvas) delete [lindex $tr($preRow,$prec,suce) $succX]
    set edgeItem [treeDisplayEdge $tree $preRow $prec $to]
    set tr($preRow,$prec,suce) [
      lreplace $tr($preRow,$prec,suce) $succX $succX $edgeItem
    ]
  }

  # ------------------------------------------------------------------------
  # notify successors, and redraw the edges to them
  # ------------------------------------------------------------------------
  eval $tr(canvas) delete $tr($row,$from,suce)
  set tr($row,$from,suce) {}
  set sucRow [expr $row+1]
  foreach succ $tr($row,$from,succ) {
    set precX [lsearch $tr($sucRow,$succ,prec) $from]
    set tr($sucRow,$succ,prec) [
      lreplace $tr($sucRow,$succ,prec) $precX $precX $to
    ]
    lappend tr($row,$from,suce) [treeDisplayEdge $tree $row $to $succ]
  }

  # -------------------------------------------------------------------------
  # remove the window of the cell and create a new one at the target position
  # -------------------------------------------------------------------------
  treeDisplayCopy $tree $row $from $row $to
  eval $tr(canvas) delete $tr($row,$from,wini)
  eval destroy $tr($row,$from,wino)
  treeDisplayInitCell $tree $row $from
  set stuff [treeDisplayPut $tree $row $to]
  set tr($row,$to,wino) [lindex $stuff 0]
  set tr($row,$to,wini) [lrange $stuff 1 end]
}


# =========================================================================== #
# scroll up the entire display by one row (scrolling down is not possible)    #
# =========================================================================== #

proc treeDisplayScrollUp { tree lines } {

  upvar #0 treeDisplay$tree tr
  set c $tr(top).f.c

  # --------------------------------------------------------------------------
  # delete all items from the canvas, because everything will appear elsewhere
  # --------------------------------------------------------------------------

  for { set rowX 0 } { $rowX < $tr(rowN) } { incr rowX } {
    for { set colX 0 } { $colX < $tr(colN) } { incr colX } {
      if { $tr($rowX,$colX,what) != "empty" } {
        if { [llength $tr($rowX,$colX,wino)] > 0 } { 
          eval destroy $tr($rowX,$colX,wino)
          set tr($rowX,$colX,wino) {}
        }
        if { [llength $tr($rowX,$colX,wini)] > 0 } { 
          eval $c delete $tr($rowX,$colX,wini)
          set tr($rowX,$colX,wini) {}
        }
        if { [llength $tr($rowX,$colX,suce)] > 0 } { 
          eval $c delete $tr($rowX,$colX,suce)
          set tr($rowX,$colX,suce) {}
        }
      }
    }
  }

  # ----------------------------------------------
  # move the contents of every cell $lines rows up
  # ----------------------------------------------

  for { set rowX $lines } { $rowX < $tr(rowN) } { incr rowX } {
    for { set colX 0 } { $colX < $tr(colN) } { incr colX } {
      treeDisplayCopy $tree $rowX $colX [expr $rowX-$lines] $colX
    }
  }

  # -------------------------------------------------
  # first row does not have any predecessors any more
  # -------------------------------------------------

  for { set colX 0 } { $colX < $tr(colN) } { incr colX } {
    set tr(0,$colX,prex) {}
    set tr(0,$colX,prec) {}
  }

  # -------------------------------
  # clear all cells in the last row
  # -------------------------------

  set lastRow [expr $tr(rowN)-1]
  for { set colX 0 } { $colX < $tr(colN) } { incr colX } {
    treeDisplayInitCell $tree $lastRow $colX
  }

  # -------------------------------------
  # display all cells on the canvas again
  # -------------------------------------

  for { set rowX 0 } { $rowX < $tr(rowN) } { incr rowX } {
    for { set colX 0 } { $colX < $tr(colN) } { incr colX } {
      set stuff [treeDisplayPut $tree $rowX $colX]
      set tr($rowX,$colX,wino) [lindex $stuff 0]
      set tr($rowX,$colX,wini) [lrange $stuff 1 end]
    }
  }

  # -------------------------------------
  # display all edges on the canvas again
  # -------------------------------------

  for { set rowX 0 } { $rowX < $tr(rowN) } { incr rowX } {
    for { set colX 0 } { $colX < $tr(colN) } { incr colX } {
      foreach succ $tr($rowX,$colX,succ) {
        lappend tr($rowX,$colX,suce) [treeDisplayEdge $tree $rowX $colX $succ]
      }
    }
  }

  # ------------------------------------------------------------
  # remove orphan polyphone lists and verbose infos in first row
  # ------------------------------------------------------------

  for { set colX 0 } { $colX < $tr(colN) } { incr colX } {
    if { $tr(0,$colX,what) == "verbp" || $tr(0,$colX,what) == "ptree" || \
         $tr(0,$colX,what) == "verbn" } {
      treeDisplayRemoveNode $tree 0 $colX
    }
  }
}


# =========================================================================== #
# insert a new node (type =  $type, index = $nodeX, parent node's colum =     #
# $parentC) into row $row of tree $tree.                                      #
# =========================================================================== #

proc treeDisplayInsertNode { tree row type nodeX parentC } {

  upvar #0 treeDisplay$tree tr

  # -------------------------------------------- #
  # check if this node is already on the display #
  # -------------------------------------------- #

  for { set rowX 0 } { $rowX < $tr(rowN) } { incr rowX } {
    for { set colX 0 } { $colX < $tr(colN) } { incr colX } {
      if { $tr($rowX,$colX,what) == $type && \
           $tr($rowX,$colX,indx) == $nodeX } {
        puts "internal error: $type $nodeX already on $rowX $colX)" ; return 
      }
    }
  }
   
  # ------------------------------------------------------------------ #
  # find out whether we want to make space to the right or to the left #
  # ------------------------------------------------------------------ #

  set left 0 ; set right 0 ; set pos "left"
  for { set colX 0 } { $colX < $tr(colN) } { incr colX } {
    if { $pos == "left"  && $tr($row,$colX,what) == "empty" } { incr left }
    if { $pos == "right" && $tr($row,$colX,what) == "empty" } { incr right }
    if { $tr($row,$colX,what) != "empty" } { set pos "right" }
  }
  if { $left == 0 && $right == 0 } { puts "error: no space in row" ; return }

  # ------------------------------------------------------------------ #
  # find out between which nodes we want the new node to be positioned #
  # ------------------------------------------------------------------ #

  for {set insColX 0} {$insColX < [expr $tr(colN)-$right]} {incr insColX} {
    if { [lindex $tr($row,$insColX,prec) 0] > $parentC } break
  }

  if { $insColX == $tr(colN) } {
    if { $left > $right } { set insColX [expr $left-1]
    } else {                set insColX [expr $tr(colN)-$right] }
  }
  
  # ---------------------------------------------------- #
  # make room for the new node by moving the others away #
  # ---------------------------------------------------- #

  if { [expr $left + $right] == $tr(colN) } {set insColX [expr $tr(colN)/2]
  } else {
    if { $left > $right } {
      incr insColX -1
      for { set colX 0 } { $colX < $insColX } { incr colX } {
        treeDisplayMove $tree $row [expr $colX+1] $colX
      }
    } else {
      for {set colX [expr $tr(colN)-1]} {$colX > $insColX} {incr colX -1} {
        treeDisplayMove $tree $row [expr $colX-1] $colX
      }
    }
  }

  # -------------------------------- #
  # set the contents of the new cell #
  # -------------------------------- #

  set tr($row,$insColX,what) $type
  set tr($row,$insColX,info) {}
  set tr($row,$insColX,text) ""
  set tr($row,$insColX,indx) $nodeX
  set tr($row,$insColX,sucw) {}
  set tr($row,$insColX,sucx) {}
  set tr($row,$insColX,succ) {}
  set tr($row,$insColX,suce) {}
  set tr($row,$insColX,wino) {}
  set tr($row,$insColX,wini) {}
  if {$row > 0} { set tr($row,$insColX,prex) $tr([expr $row-1],$parentC,indx) }
  if {$parentC > -1} { set tr($row,$insColX,prec) $parentC 
  } else { set tr($row,$insColX,prec) {} }

  # ------------------------------- #
  # create a new item in the canvas #
  # ------------------------------- #

  set stuff [treeDisplayPut $tree $row $insColX]
  set tr($row,$insColX,wino) [lindex $stuff 0]
  set tr($row,$insColX,wini) [lrange $stuff 1 end]

  if { $parentC >= 0 } {

    # ------------------------------------- #
    # draw the edge to the predecessor node #
    # ------------------------------------- #

    set preRow [expr $row-1]
    set edgeItem [treeDisplayEdge $tree $preRow $parentC $insColX]

    # --------------------------------------------------- #
    # notify the predecessor node about its new successor #
    # --------------------------------------------------- #

    lappend tr($preRow,$parentC,sucw) $type
    lappend tr($preRow,$parentC,sucx) $nodeX
    lappend tr($preRow,$parentC,succ) $insColX
    lappend tr($preRow,$parentC,suce) $edgeItem
  }
  return $insColX
}


# =========================================================================== #
# remove a node from the display, move one side of the row into the hole      #
# =========================================================================== #

proc treeDisplayRemoveNode { tree row col } {

  upvar #0 treeDisplay$tree tr

  # ------------------------------------------------------------------------
  # first recursively remove the entire subtree below the given node
  # ------------------------------------------------------------------------

  treeDisplayRemoveRec $tree $row $col

  # -------------------------------------------------------------------------
  # shrink all lines moving one side of the row into the newly created holes,
  # -------------------------------------------------------------------------

  for { set rowX $row } { $rowX < $tr(rowN) } { incr rowX } {

    # -------------------------------------------------------------------------
    # compute a move-mapping for the row's cells
    # -------------------------------------------------------------------------

    for { set colX 0 ; set pos 0 } { $colX < $tr(colN) } { incr colX } {
      if { $tr($rowX,$colX,what) != "empty" } { incr pos }
    }
    set first [expr ($tr(colN)-$pos)/2]
    for { set colX 0 ; set pos 0 } { $colX < $tr(colN) } { incr colX } {
      if { $tr($rowX,$colX,what) != "empty" } {
        set map($colX) [expr $first+$pos]
        incr pos
      } else { set map($colX) -1 }
    }

    set notFinished 1
    while { $notFinished == 1 } {
      set notFinished 0
      for { set colX 0 } { $colX < $tr(colN) } { incr colX } {
        if { $map($colX) != -1 && $tr($rowX,$map($colX),what) == "empty" } {
          treeDisplayMove $tree $rowX $colX $map($colX)
	  set map($colX) -1
          set notFinished 1
        }
      }
    }
  }
}


# =========================================================================== #
# recursive function for removing an entire subtree from the display
# =========================================================================== #

proc treeDisplayRemoveRec { tree row col } {

  upvar #0 treeDisplay$tree tr

  # ------------------------------------------------------------------------
  # check if the source position is empty; if so, don't do the remove
  # ------------------------------------------------------------------------
  if { $tr($row,$col,what) == "empty" } return

  # ------------------------------------------------------------------------
  # notify predecessors, and delete their edges
  # ------------------------------------------------------------------------
  set preRow [expr $row-1]
  foreach prec $tr($row,$col,prec) {
    set succX [lsearch $tr($preRow,$prec,succ) $col]
    set tr($preRow,$prec,succ) [
      lreplace $tr($preRow,$prec,succ) $succX $succX
    ]
    $tr(canvas) delete [lindex $tr($preRow,$prec,suce) $succX]
    set tr($preRow,$prec,suce) [
      lreplace $tr($preRow,$prec,suce) $succX $succX
    ]
  }

  # ------------------------------------------------------------------------
  # remove all successors recursively
  # ------------------------------------------------------------------------
  set sucRow [expr $row+1]
  foreach succ $tr($row,$col,succ) {treeDisplayRemoveRec $tree $sucRow $succ}

  # -------------------------------------------------------------------------
  # remove the window of the cell and clear the cell's contents
  # -------------------------------------------------------------------------
#puts "eval $tr(canvas) delete $tr($row,$col,wini)"
  eval $tr(canvas) delete $tr($row,$col,wini)
  eval destroy $tr($row,$col,wino)
  treeDisplayInitCell $tree $row $col

}


# =========================================================================== #
# create and display the menu bar                                             #
# =========================================================================== #

proc treeDisplayMakeMenu tree {

  upvar #0 treeDisplay$tree tr

  set w $tr(top)
  frame  $w.menu

  # -------------------------------------------------------------- #
  # there'll be four menus: "File", "Action", "Option", and "Help" #
  # -------------------------------------------------------------- #

  menubutton $w.menu.file   -text File     -menu $w.menu.file.menu \
			    -borderwidth 1 -relief raised
  menubutton $w.menu.action -text Action   -menu $w.menu.action.menu \
                            -borderwidth 1 -relief raised
  menubutton $w.menu.option -text Option   -menu $w.menu.option.menu \
			    -borderwidth 1 -relief raised
  menubutton $w.menu.help   -text Help     -menu $w.menu.help.menu \
			    -borderwidth 1 -relief raised
  # --------------- #
  # the "File" menu #
  # --------------- #

  menu $w.menu.file.menu
  $w.menu.file.menu add command -label "About treeDisplay" \
                                -command "treeDisplayAbout"
  $w.menu.file.menu add command -label "Dump postscript" \
			        -command "treeDisplayDumpPostscript"
  $w.menu.file.menu add command -label Quit  -command "destroy $w"

  # ----------------- #
  # the "Action" menu #
  # ----------------- #

  menu $w.menu.action.menu
  $w.menu.action.menu add command -label "Trace model" \
				  -command "treeDisplayTrace tree"
  # ----------------- #
  # the "Option" menu #
  # ----------------- #

  menu $w.menu.option.menu
  $w.menu.option.menu add command -label "Resize Display" \
				  -command "treeDisplayResizeDisplay $tree"
  $w.menu.option.menu add command -label "Define QuestionSet" \
				  -command "treeDisplayQuestionSet $tree"
  $w.menu.option.menu add command -label "Define minCount" \
				  -command "treeDisplayMinCount $tree"

  # --------------- #
  # the "Help" menu #
  # --------------- #

  menu $w.menu.help.menu
  $w.menu.help.menu add command -label "How to use" \
			        -command "treeDisplayHowTo"
  $w.menu.help.menu add command -label "Quick reference" \
				-command "treeDisplayQuickRef"

  # ---------------------------- #
  # put the menus on the display #
  # ---------------------------- #

  pack $w.menu.file $w.menu.action $w.menu.option $w.menu.help -side left 
  pack $w.menu -side top -fill x
}


# =========================================================================== #
# stuff for resizing the display
# =========================================================================== #

proc treeDisplayDoScale { tree w dummy } {

 upvar #0 treeDisplay$tree tr
 set x $tr(sizeX)
 set y $tr(sizeY)
 $w.f1.c delete all
 $w.f1.c create rect 10 10 [expr 20+($x-5)*3.8] [expr 20+($y-5)*1.6] -fill blue
}

proc treeDisplaySetSize { tree win } {

  upvar #0 treeDisplay$tree tr

  for { set rowX $tr(rowN) } { $rowX < $tr(sizeY) } { incr rowX } {
    for { set colX 0 } { $colX < $tr(sizeX) } { incr colX } {
      treeDisplayInitCell $tree $rowX $colX
    }
  }
  for { set colX $tr(colN) } { $colX < $tr(sizeX) } { incr colX } {
    for { set rowX 0 } { $rowX < $tr(rowN) } { incr rowX } {
      treeDisplayInitCell $tree $rowX $colX
    }
  }
  destroy $win
  set tr(rowN) $tr(sizeY)
  set tr(colN) $tr(sizeX)
  set scrollregion [list 0 0 [expr $tr(colN)*$tr(dX)] [expr $tr(rowN)*$tr(dY)]]
  $tr(canvas) configure -scrollregion $scrollregion
}

proc treeDisplayResizeDisplay tree {

  upvar #0 treeDisplay$tree tr

  set tr(sizeY) $tr(rowN)
  set tr(sizeX) $tr(colN)

  set win .s
  catch { destroy $win }
  toplevel $win
  frame $win.f1
  frame $win.f2
  canvas $win.f1.c -width 100 -height 100
  scale $win.f1.s -command "treeDisplayDoScale $tree $win " -from 5 -to 50 \
		  -variable treeDisplay${tree}(sizeY) -orient vertical 
  scale $win.f2.s -command "treeDisplayDoScale $tree $win " -from 5 -to 25 \
		  -variable treeDisplay${tree}(sizeX) -orient horizontal
  button $win.f2.b -text OK -command "treeDisplaySetSize $tree $win"
  pack $win.f1.c -side right
  pack $win.f1.s -side left
  pack $win.f2.s -side right
  pack $win.f2.b -side left
  pack $win.f1 -side top
  pack $win.f2 -side bottom
}


# =========================================================================== #
# show version message etc.                                                   #
# =========================================================================== #

proc treeDisplayAbout { } {
  catch  { destroy .about }
  toplevel .about
  wm title .about "About treeDisplay"
  button .about.b -text { treeDisplay Version 0.0 (96/06/06,09:25) } \
		  -command "destroy .about"
  pack .about.b
}


# =========================================================================== #
# show text window with the Quick Reference message                           #
# =========================================================================== #

proc treeDisplayQuickRef { } {

  catch  { destroy .quickRef }
  toplevel .quickRef
  wm title .quickRef "Quick Reference"
  frame .quickRef.t
  button .quickRef.b -text close -command "destroy .quickRef"
  pack .quickRef.t -side top
  pack .quickRef.b -side bottom
  set scrltxt [treeDisplayMakeScrollText .quickRef.t 80 25 {}]
  set text {
  	button 1	expand / collapse
	button 2	show/remove polyphone list
	button 3	show/remove model display
  }
  $scrltxt insert end $text
}


# =========================================================================== #
# show text window with the help message                                      #
# =========================================================================== #

proc treeDisplayHowTo { } {

  catch  { destroy .howto }
  toplevel .howto
  wm title .howto "How to Use the treeDisplay Tool"
  frame .howto.t
  button .howto.b -text close -command "destroy .howto"
  pack .howto.t -side top
  pack .howto.b -side bottom
  set scrltxt [treeDisplayMakeScrollText .howto.t 80 25 {}]
  set text {

    This is the "treeDisplay" HOWTO
    ===============================

    Overview
    --------

    When you are reading this, you seem to know how to fire up the tree
    display tool. The display shows a menu-bar with four menus, which are
    described in detail, later. The main display is a canvas which, at the
    beginning shows a root node (as specified when the fire-up command was
    entered). Every node on the canvas has a menu button at it's left end.
    Clicking on the menu button will show you various things you can do with
    the node. The detailed description of these menus will follow later.
    There are two different kinds of nodes, the ones with a yellow background
    are nodes that have a meaningful question and thus can have successor
    nodes. The ones with a pink background are nodes that have no successors
    (leaf nodes). The yellow (non-leaf) nodes are labeled with the name of the
    question, the pink nodes are labeled with the name of the used model
    (which usually is a distribution). This convention assumes that the tree
    conforms with the notion that there actually are such things as leaf-nodes
    and non-leaf nodes. As you know, the Janus trees can be arbitrarily complex
    and every node can have multiple parents, nodes can have both, successors
    and polyphone lists attached, and then can even have a model, too. This
    display tool will not display nodes with multiple parents correctly, only
    one parent can be displayed at one time.
    All nodes are centered in the canvas, which means that adding or removing
    nodes does influence all the other nodes in the same level. There will not
    be any 'holes' in the display. Whenever a node is removed, the neighboring
    nodes will be shifted to the inside and fill the gap.
    You can manually and selectively grow the tree display by clicking on
    nodes. If a node's successors are not displayed when you click on it, you
    will get the successors on the canvas. If the successors are displayed and
    you click on the node then the entire subtrees of the successors including
    the successors will disappear.

    Main Menu
    ---------

    The main menu offers the following items:

	File
        ----
        About treeDisplay:	Show version number etc.
	Quit:			Quit the display tool, no questions asked.
	Dump postscript:	Write a postscript file of the current display,
				the name of the file will be <treeName>.ps.
	Action
	------
	Trace model:		Will prompt you with an entry. After typing in
				a phone context you will be shown a trace
				thru the tree down to the used model for the 
				entered context.
        Option
        ------
	Resize display:		Will prompt you with two sliders for entering
				the numbers of rows and columns. If you change
				the number of columns, the displayed nodes will
				be centered anew.
	Define QuestionSet:	Enter the name of the question set from which
				questions for splitting can be chosen.	
	Define minCount:	Define the minCount parameter that is given to
				the 'question' method when looking for the
				optimal splitting question.
	Help
        ----
	How to use:		Will show this text.
	Quick reference:	Will show a summary of the things that can
				be done with only very brief descriptions.

    Node Menu
    ---------

    Every displayed node has a small button at its left end. This button is
    an menu with things that can be done to a node. Some of these things can
    also be done by simple mouseclicks on the node boackground, too. Here's
    the meaning of the menu items:

	verbose info:		Open a new window, displaying all information
				about the node, configurable parameters can
				be modified, but will not effect the display
				immediately. (same as middle button)
	find question:		Display the question (from the tree's question
				set) which would make the optimal split.
	optimal split:		Perform the optimal split on the node and
				display the newly created successor nodes.
	custom split:		Perform a split according to a user-entered
				question.
	expand/collapse:	If the node's successors are displayed, then
				remove them (including all their successors),
				if no successors are displayed, show the direct
				successors. (same as left button)
	show polyphones:	Display a node with a scrollable list of the
				nodes polyphones, behaves like a sucessor node.
				If the polyphones are already on the display,
				remove them. (same as right button)
	display model:		Make a node appear which displays the model
				(usually a distribution) of the node. This
				too appears like a regular successor.
  }
  $scrltxt insert end $text
}


# =========================================================================== #
# create the main canvas including scrollbars for displaying the tree         #
# =========================================================================== #

proc treeDisplayTrace tree {

 upvar #0 treeDisplay$tree tr

 puts "Can't do the trace, yet."

}


# =========================================================================== #
# prompt the user to enter the name of a question set to be used for splits   #
# =========================================================================== #

proc treeDisplayQuestionSet tree {

 catch { destroy .quest }
 toplevel .quest
 wm title .quest "Enter Name of QuestionSet"
 entry .quest.e -textvariable treeDisplay${tree}(quest) -width 30
 button .quest.ok -text OK -command "destroy .quest"
 pack .quest.e .quest.ok -side left
}

# =========================================================================== #
# prompt the user to enter the minCount parameter for splitting               #
# =========================================================================== #

proc treeDisplayMinCount tree {

 catch { destroy .minc }
 toplevel .minc
 wm title .minc "Enter the value of minCount"
 entry .minc.e -textvariable treeDisplay${tree}(minCount) -width 9
 button .minc.ok -text OK -command "destroy .minc"
 pack .minc.e .minc.ok -side left
}

# =========================================================================== #
# create the main canvas including scrollbars for displaying the tree         #
# =========================================================================== #

proc treeDisplayMakeCanvas tree {

 upvar #0 treeDisplay$tree tr

 set w $tr(top)
 set f $tr(top).f
 set c $tr(top).f.c

 frame $f -relief flat -bd 1
 pack  $f -side top -expand yes -fill both

 scrollbar $f.vscroll -width 8 -relief sunken -command "$c yview"
 scrollbar $f.hscroll -width 8 -relief sunken -command "$c xview" -orient horiz

 pack  $f.vscroll -side right  -fill y
 pack  $f.hscroll -side bottom -fill x
 canvas $c -scrollregion "0 0 $tr(cWidth) $tr(cHeight)" \
           -xscrollcommand "$f.hscroll set" -yscrollcommand "$f.vscroll set" \
	   -width 500
 pack $c -expand yes -fill both
 $c yview moveto 0.0
 $c xview moveto 0.4

 set tr(canvas) $c
}

# =========================================================================== #
# display a window with a scrollable editable text in the given Tk-frame      #
# =========================================================================== #

proc treeDisplayMakeScrollText { f w h tit { font 8x16 }} {

  if { [llength $tit] > 0 } {
    label $f.l -text $tit -font $font -relief raised -borderwidth 1
    pack  $f.l -side top -expand yes -fill x
  }
  text  $f.t -width $w -height $h -setgrid true -wrap none -font $font \
             -borderwidth 1 \
             -xscrollcommand [list $f.xscroll set] \
             -yscrollcommand [list $f.yscroll set]
  scrollbar $f.xscroll -orient horizontal -command [list $f.t xview] \
                       -width 6 -borderwidth 1
  scrollbar $f.yscroll -orient vertical -command [list $f.t yview] \
                       -width 6 -borderwidth 1
  pack $f.yscroll -side right  -fill y
  pack $f.xscroll -side bottom -fill x 
  pack $f.t -side left -expand true -fill x

  return $f.t
}

# =========================================================================== #
# display the successors of the node at the given position                    #
# =========================================================================== #

proc treeDisplayPlus { tree row col } {

  upvar #0 treeDisplay$tree tr

  set nodeX $tr($row,$col,indx)

  # ------------------------------------------------------------
  # find successors, don't display same successor more than once
  # ------------------------------------------------------------

  set n [$tree.item($nodeX) configure -no]
  set y [$tree.item($nodeX) configure -yes]
  set u [$tree.item($nodeX) configure -undef]

  if { $y == $n } { set y -1 }
  if { $u == $n || $u == $y } { set u -1 }

  # -------------------------------------------
  # if necessary scroll the display up one line
  # -------------------------------------------

  if { [expr $row+1]  == $tr(rowN) } {
    treeDisplayScrollUp $tree 1
    incr row -1
  }

  # -------------------------------------
  # now put all successors on the display
  # -------------------------------------

  if { $n != -1 } { treeDisplayInsertNode $tree [expr $row+1] node $n $col }
  if { $y != -1 } { treeDisplayInsertNode $tree [expr $row+1] node $y $col }
  if { $u != -1 } { treeDisplayInsertNode $tree [expr $row+1] node $u $col }

  #puts "$n $y $u"

  if { $n == -1 && $y == -1 && $u == -1 } { 
    puts "e1" ; treeDisplayBeep ; return 
  }
}


# =========================================================================== #
# remove the successors of the node at the given position                     #
# =========================================================================== #

proc treeDisplayMinus { tree row col } {

  upvar #0 treeDisplay$tree tr

  while { [llength $tr($row,$col,succ)] > 0 } {
    treeDisplayRemoveNode $tree [expr $row+1] [lindex $tr($row,$col,succ) 0]
  }
}

# =========================================================================== #
# do either a plus (when no succs present) or a minus (when succs present)
# =========================================================================== #

proc treeDisplayPlusMinus { tree row col } {

  upvar #0 treeDisplay$tree tr

  if { [llength $tr($row,$col,succ)] > 0 } {
    treeDisplayMinus $tree $row $col
  } else {
    treeDisplayPlus $tree $row $col
  }
}

# =========================================================================== #
# display a menu of things tha can be done to a node and execute it           #
# =========================================================================== #

proc treeDisplayNodeMenu { tree row col } {

  upvar #0 treeDisplay$tree tr

  set c $tr(top).f.c
  set nodeX $tr($row,$col,indx)
  set id "$tr($row,$col,what)$nodeX"

  menubutton $c.menu$id -menu $c.menu$id.menu -borderwidth 1 -relief raised
  menu $c.menu$id.menu
  $c.menu$id.menu add command -label "verbose info" \
                              -command "treeDisplayVerboseInfo $tree $row $col"
  $c.menu$id.menu add command -label "find question" \
                              -command "treeDisplayFindQuest $tree $row $col"
  $c.menu$id.menu add command -label "optimal split" \
                              -command "treeDisplayOptSplit $tree $row $col"
  $c.menu$id.menu add command -label "custom split" \
                              -command "treeDisplayCustomSplit $tree $row $col"
  $c.menu$id.menu add command -label "expand/collapse" \
                              -command "treeDisplayPlusMinus $tree $row $col"
  $c.menu$id.menu add command -label "toggle polyphones" \
                              -command "treeDisplayPolyOnOff $tree $row $col"
  $c.menu$id.menu add command -label "toggle model" \
                              -command "treeDisplayModelOnOff $tree $row $col"
  $c.menu$id.menu add command -label "scroll to top" \
                              -command "treeDisplayScrollUp $tree $row"
  return $c.menu$id
}

# =========================================================================== #
# display the model of the node at the given position (or remove it)          #
# =========================================================================== #

proc treeDisplayModelOnOff { tree row col } {
  
  upvar #0 treeDisplay$tree tr

  # --------------------------------------------------- #
  # if we are already showing the model, then remove it #
  # --------------------------------------------------- #

  set row1 [expr $row+1]
  foreach succ $tr($row,$col,succ) {
    if {$tr($row1,$succ,what) == "model"} {
      treeDisplayRemoveNode $tree $row1 $succ
      return
    }
  }

  # -------------------------------------------------------
  # if it is not yet on the display, get the polyphone list
  # -------------------------------------------------------

  set nodeX $tr($row,$col,indx)
  if { [set modelX [$tree.item($tr($row,$col,indx)) configure -model]] == -1} {
    puts "e2" ; treeDisplayBeep ; return 
  }

  # -------------------------------------------
  # if necessary scroll the display up one line
  # -------------------------------------------

  if { [expr $row+1]  == $tr(rowN) } {
    treeDisplayScrollUp $tree 1
    incr row -1
  }

  # ------------------------------
  # finally, put it on the display
  # ------------------------------

  treeDisplayInsertNode $tree [expr $row+1] model $modelX $col
}

# =========================================================================== #
# display the polyphone set of the node at the given position (or remove it)  #
# =========================================================================== #

proc treeDisplayPolyOnOff { tree row col } {
  
  upvar #0 treeDisplay$tree tr

  # ------------------------------------------------------------ #
  # if we are already showing the polyphone list, then remove it #
  # ------------------------------------------------------------ #

  set row1 [expr $row+1]
  foreach succ $tr($row,$col,succ) {
    if {$tr($row1,$succ,what) == "verbp" || $tr($row1,$succ,what) == "ptree"} {
      treeDisplayRemoveNode $tree $row1 $succ
      return
    }
  }

  # -------------------------------------------------------
  # if it is not yet on the display, get the polyphone list
  # -------------------------------------------------------

  set nodeX $tr($row,$col,indx)
  if { [set ptreeX [$tree.item($tr($row,$col,indx)) configure -ptree]] == -1} {
    puts "e2" ; treeDisplayBeep ; return 
  }

  # -------------------------------------------
  # if necessary scroll the display up one line
  # -------------------------------------------

  if { [expr $row+1]  == $tr(rowN) } {
    treeDisplayScrollUp $tree 1
    incr row -1
  }

  # ------------------------------
  # finally, put it on the display
  # ------------------------------

  treeDisplayInsertNode $tree [expr $row+1] verbp $ptreeX $col
}


# =========================================================================== #
# display verbose information about a node in an extra successor              #
# =========================================================================== #

proc treeDisplayVerboseInfo { tree row col } {

  upvar #0 treeDisplay$tree tr

  # ---------------------------------------------------------- #
  # if we are already showing the verbose info, then remove it #
  # ---------------------------------------------------------- #

  set row1 [expr $row+1]
  foreach succ $tr($row,$col,succ) {
    if {$tr($row1,$succ,what) == "verbn"} {
      treeDisplayRemoveNode $tree $row1 $succ
      return
    }
  }

  # -------------------------------------------------- #
  # if it is not yet on the display, then put it there #
  # -------------------------------------------------- #

  treeDisplayInsertNode $tree [expr $row+1] verbn $tr($row,$col,indx) $col
}

 
# =========================================================================== #
# find the optimal question to split the given node and perform the splitting #
# =========================================================================== #

proc treeDisplayOptSplit { tree row col } {
  treeDisplayFindQuest $tree $row $col
  treeDisplaySplit $tree $row $col
}

# =========================================================================== #
# prompt the user for a question and split the given node according to that   #
# =========================================================================== #

proc treeDisplayCustomSplit { tree row col } {

  puts "must fix event handling first ...."; return
  catch { destroy .cquest }
  toplevel .cquest
  wm title .cquest "Enter Name of Question"
  entry .cquest.e -textvariable treeDisplay${tree}($row,$col,ques) -width 30
  button .cquest.ok -text OK -command "destroy .cquest"
  pack .cquest.e .cquest.ok -side left
  treeDisplaySplit $tree $row $col
}


# =========================================================================== #
# display the question which optimally splits the node at the given position  #
# =========================================================================== #

proc treeDisplayFindQuest { tree row col } {
  
  upvar #0 treeDisplay$tree tr

  # --------------------------------------------------
  # check if this is a regular node without successors
  # --------------------------------------------------

  if { $tr($row,$col,what) != "node" } { treeDisplayBeep ; return }
  set nodeX $tr($row,$col,indx)
  if {[$tree.item($nodeX) configure -no   ] != -1 || \
      [$tree.item($nodeX) configure -yes  ] != -1 || \
      [$tree.item($nodeX) configure -undef] != -1} { treeDisplayBeep ; return }

  # -----------------------------------
  # let Janus find the optimal question
  # -----------------------------------

  set nodeName [$tree name $nodeX]
  set question [lindex [$tree question $nodeName -questionSet $tr(quest) \
					         -minCount    $tr(minCount)] 0]
  
  # -----------------------------------
  # display that question and its score
  # -----------------------------------

  set tr($row,$col,ques) $question
  puts $question

  return
}


# =========================================================================== #
# split the node at the given position according to the given question        #
# =========================================================================== #

proc treeDisplaySplit { tree row col } {

  upvar #0 treeDisplay$tree tr

  # ---------------------------------
  # check if the node is 'splittable'
  # ---------------------------------

  set nodeX $tr($row,$col,indx)

  # ---------------------------------------------------------------
  # find successors, don't split if the node has already successors
  # ---------------------------------------------------------------

  set n [$tree.item($nodeX) configure -no]
  set y [$tree.item($nodeX) configure -yes]
  set u [$tree.item($nodeX) configure -undef]

  if { $n != -1  || $y != -1 || $u != -1 } return

  # -----------------------------------------
  # remove the original node from the display
  # -----------------------------------------

  set parentC [lindex $tr($row,$col,prec) 0]
  treeDisplayRemoveNode $tree $row $col

  # -----------------
  # perform the split
  # -----------------

  set node [$tree name $nodeX]
  set n ${node}..n
  set y ${node}..y
  set u ${node}..u

#puts "$tree split $node $tr($row,$col,ques) $n $y $u -minCount $tr(minCount)"

  $tree split $node $tr($row,$col,ques) $n $y $u -minCount $tr(minCount)

  # --------------------------------------------------------------
  # add the node that was split plus its successors to the display 
  # --------------------------------------------------------------

  set insColX [treeDisplayInsertNode $tree $row node $nodeX $parentC]
  treeDisplayPlus $tree $row $insColX
}

# =========================================================================== #
# make a "BEEP" sound to indicate an error or to provoke attention            #
# =========================================================================== #

proc treeDisplayBeep { } { puts [format "%c" 7] }


# =========================================================================== #
# display something at the given position                                     #
# =========================================================================== #

proc treeDisplayPut { tree row col } {

  upvar #0 treeDisplay$tree tr

#puts "Put $tree $row $col"

  set nodeX $tr($row,$col,indx)

  switch -exact -- $tr($row,$col,what) {
    empty  { return "" }
    node   { set tr($row,$col,text) "blah" }
    ptree  { set tr($row,$col,text) "blah" }
    verbn  { set tr($row,$col,text) [treeDisplayNodeText $tree $nodeX] }
    verbp  { set tr($row,$col,text) [treeDisplayPtreeText $tree $nodeX] }
    model  { set tr($row,$col,text) "blah" }
    quest  { return "" }
  }

  switch -exact -- $tr($row,$col,what) {
    empty  { return "" }
    node   { return [treeDisplayPutNode  $tree $row $col] }
    ptree  { return [treeDisplayPutPtree $tree $row $col] }
    verbn  { return [treeDisplayPutVerbN $tree $row $col] }
    verbp  { return [treeDisplayPutVerbP $tree $row $col] }
    model  { return [treeDisplayPutModel $tree $row $col] }
    quest  { return [treeDisplayPutQuest $tree $row $col] }
  }
}


# =========================================================================== #
# display model (i.e. distribution, or topology, or duration)                 #
# =========================================================================== #

proc treeDisplayPutModel { tree row col } {

  switch -- [$tree.modelSet type] {
   DistribSet  {set items [treeDisplayPutModelDistrib  $tree $row $col]}
   TopoSet     {set items [treeDisplayPutModelTopo     $tree $row $col]}
   DurationSet {set items [treeDisplayPutModelDuration $tree $row $col]}
  }
  return [eval list \{\} $items] ;# unpack $items
}


# =========================================================================== #
# display distribution                                                        #
# =========================================================================== #

proc treeDisplayPutModelDistrib { tree row col } {

  upvar #0 treeDisplay$tree tr

  set modelX $tr($row,$col,indx)
  set tit [$tree.modelSet.item($modelX) configure -name]
  set c $tr(canvas)

  set height [expr $tr(rowY,[expr $row+1]) - $tr(rowY,$row) - 5]
  set width  [expr $tr(colX,[expr $col+1]) - $tr(colX,$col) - 5]

  set items {}

  lappend items [$c create rect $tr(colX,$col) $tr(rowY,$row) \
			        [expr $tr(colX,$col)+$width]  \
			        [expr $tr(rowY,$row)+$height] \
				-fill green]

  set valN [$tree.modelSet.item($modelX) configure -valN]
  set step [expr ($width-4) / $valN]
  set val  [lindex [$tree.modelSet.item($modelX) configure -val] 0]

  for { set valX 0 ; set max 0.0 } { $valX < $valN } { incr valX } {
    if { [lindex $val $valX] > $max } { set max [lindex $val $valX] }
  }

  for { set valX 0 } { $valX < $valN } { incr valX } {
    set v     [lindex $val $valX]
    set left  [expr $tr(colX,$col)+2+$step* $valX   ]
    set right [expr $tr(colX,$col)+2+$step*($valX+1)]
    set low   [expr $tr(rowY,$row)+$height-2]
    set high  [expr $tr(rowY,$row)+$height-2 - ($height-4)*$v/$max]
    lappend items [$c create rect $left $high $right $low -fill black]
  }
#puts $items
  return $items
}


# =========================================================================== #
# display topology                                                            #
# =========================================================================== #

proc treeDisplayPutModelTopo { tree row col } {

  upvar #0 treeDisplay$tree tr

  set nodeX $tr($row,$col,indx)
  set modelX [$tree.item($nodeX) configure -model]
  set tit [$tree.modelSet.item($modelX) configure -name]
  set c $tr(canvas)

  set items {}

  puts "Can't display topologies yet."

  return $items
}


# =========================================================================== #
# display duration                                                            #
# =========================================================================== #

proc treeDisplayPutModelDuration { tree row col } {

  upvar #0 treeDisplay$tree tr

  set nodeX $tr($row,$col,indx)
  set modelX [$tree.item($nodeX) configure -model]
  set tit [$tree.modelSet.item($modelX) configure -name]
  set c $tr(canvas)

  puts "Can't display durations yet."

  set items {}

  return $items
}


# =========================================================================== #
# display verbose tree node                                                   #
# =========================================================================== #

proc treeDisplayPutVerbN { tree row col } {

  upvar #0 treeDisplay$tree tr

  set nodeX $tr($row,$col,indx)
  set id "$tr($row,$col,what)$nodeX"
  set tit [$tree.item($nodeX) configure -name]
  set txt $tr($row,$col,text)
  set c $tr(canvas)

  if [llength [info commands $c.t$id]] { 
    puts "e3" ; treeDisplayBeep ; return { } 
  }

  frame $c.v$id
  pack  $c.v$id
  treeDisplayMakeScrollText $c.v$id 24 3 {} 6x9
  $c.v$id.t insert end $txt
  set item [$c create window $tr(colX,$col) $tr(rowY,$row) \
	    -window $c.v$id -anchor nw]

#puts ":: $tree, row $row, col $col, id $id, nodeX $nodeX, tit $tit, item $item"

  return [list $c.v$id $item]
}


# =========================================================================== #
# display compact tree node                                                   #
# =========================================================================== #

proc treeDisplayPutNode { tree row col } {

  upvar #0 treeDisplay$tree tr

  set colOff 80
  set rowOff 5

  set nodeX $tr($row,$col,indx)
  set id "$tr($row,$col,what)$nodeX"
  set tit [$tree.questionSet name [$tree.item($nodeX) configure -question]]
  if { $tit == "\{\}" } { 
    set tit [$tree.modelSet name [$tree.item($nodeX) configure -model]]
  }
  set txt $tr($row,$col,text)
  set c $tr(canvas)

  set n [$tree.item($nodeX) configure -no]
  set y [$tree.item($nodeX) configure -yes]
  set u [$tree.item($nodeX) configure -undef]

  if { $n == -1 && $y == -1 && $u == -1 } { set fillColor pink 
  } else { set fillColor yellow }

  if [llength [info commands $c.t$id]] { 
    puts "e3" ; treeDisplayBeep ; return { } 
  }

  set r1 [$c create rect [expr   5+$tr(colX,$col)] [expr  5+$tr(rowY,$row)] \
			 [expr 155+$tr(colX,$col)] [expr 25+$tr(rowY,$row)] \
			 -fill black]
  set r2 [$c create rect           $tr(colX,$col)           $tr(rowY,$row) \
			 [expr 150+$tr(colX,$col)] [expr 20+$tr(rowY,$row)] \
			 -fill $fillColor]
  set t1 [$c create text [expr $colOff+$tr(colX,$col)] \
			 [expr $rowOff+$tr(rowY,$row)] \
			 -text $tit -anchor n ]

  set menuWin [treeDisplayNodeMenu $tree $row $col]
  $c create window $tr(colX,$col) $tr(rowY,$row) -window $menuWin -anchor nw

  $c bind $r2 <Button-1> "treeDisplayPlusMinus  $tree $row $col"
  $c bind $r2 <Button-2> "treeDisplayPolyOnOff  $tree $row $col"
  $c bind $r2 <Button-3> "treeDisplayModelOnOff $tree $row $col"

  return [list $menuWin $r1 $r2 $t1]
}


# =========================================================================== #
# display verbose polyphone list					      #
# =========================================================================== #

proc treeDisplayPutVerbP { tree row col } {

  upvar #0 treeDisplay$tree tr

  set nodeX $tr($row,$col,indx)
  set id "$tr($row,$col,what)$nodeX"
  set tit [$tree.ptreeSet.item($nodeX) configure -name]
  set c $tr(canvas)
  if [llength [info commands $c.p$id]] { 
    puts "e4" ; treeDisplayBeep ; return { } 
  }
  frame $c.p$id
  pack $c.p$id
  treeDisplayMakeScrollText $c.p$id 24 2 $tit 6x9
  $c.p$id.t insert end $tr($row,$col,text)
  set item [$c create window $tr(colX,$col) $tr(rowY,$row) \
            -window $c.p$id -anchor nw]

#puts ";; $tree, row $row, col $col, id $id, nodeX $nodeX, tit $tit, item $item"

  return [list $c.p$id $item]
}


# =========================================================================== #
# put a tree edge on the canvas           
# =========================================================================== #

proc treeDisplayEdge { tree row from to {type undef} } {

  upvar #0 treeDisplay$tree tr
  set c $tr(canvas)

  set colOff 80
  set rowOff 20

  if { $type == "yes" }   { set stipple gray50 }
  if { $type == "undef" } { set stipple gray25 }
  if { $type == "undef" } { set stipple "" }

  set item [$c create line \
	       [expr $tr(colX,$from) + $colOff] \
	       [expr $tr(rowY,$row)  + $rowOff] \
	       [expr $tr(colX,$to)   + $colOff] \
	       [expr $tr(rowY,[expr $row+1])] \
	       -width 3 -stipple $stipple ]
  $c lower $item

#  puts ",, $tree row $row, from $from, to $to, item $item"
  return $item
}


# =========================================================================== #
# return the text to be displayed in a verbose polyphone list                 #
# =========================================================================== #

proc treeDisplayPtreeText { tree nodeX } {
  set poly [$tree.ptreeSet.item($nodeX)]
  set len  [llength [split $poly "\n"]]
  return "$len POLYPHONES:\n$poly"
}


# =========================================================================== #
# return the text to be displayed in a verbose tree node                      #
# =========================================================================== #

proc treeDisplayNodeText { tree nodeX } {

  set name     [$tree.item($nodeX) configure -name]
  set quX      [$tree.item($nodeX) configure -question]
  set quN      [$tree.questionSet name $quX]
  set noX      [$tree.item($nodeX) configure -no]
  set noN      [$tree name $noX]
  set yesX     [$tree.item($nodeX) configure -yes]
  set yesN     [$tree name $yesX]
  set undefX   [$tree.item($nodeX) configure -undef]
  set undefN   [$tree name $undefX]
  set modelX   [$tree.item($nodeX) configure -model]
  set modelN   [$tree.modelSet name $modelX]
  set ptreeX   [$tree.item($nodeX) configure -ptree]
  set ptreeN   [$tree.ptreeSet name $ptreeX]

  set descText          [format "QUEST: %5d %s\n" $quX $quN]
  set descText $descText[format "NO   : %5d %s\n" $noX $noN]
  set descText $descText[format "YES  : %5d %s\n" $yesX $yesN]
  set descText $descText[format "UNDEF: %5d %s\n" $undefX $undefN]
  set descText $descText[format "MODEL: %5d %s\n" $modelX $modelN]
  set descText $descText[format "PTREE: %5d %s\n" $ptreeX $ptreeN]
  set descText $descText[format "INDEX: %5d\n" $nodeX]
  set descText $descText[format "NAME : %s\n" $name]

  return $descText
}


# =========================================================================== #
# main function for displaying a tree, needs name of tree and a starting node #
# =========================================================================== #

proc treeDisplay { args } {

 if {[catch {itfParseArgv treeDisplay $args [ list [
      list "<tree>" object {} tree Tree "tree object" ] [
      list "<root>" string {} root {}   "name of root node" ] ]
  } ] } {
    return
  }

  upvar #0 treeDisplay$tree tr

  treeDisplayInit $tree
  set rootX [$tree index $root]
  set tr(top) .trd$tree
  toplevel $tr(top)
  wm title $tr(top) "Tree Display: $tree"
  treeDisplayMakeMenu $tree
  treeDisplayMakeCanvas $tree
  treeDisplayInsertNode $tree 0 node $rootX -1
}

return

source startupnd.tcl
set tree distribTreep1
treeDisplay $tree ROOT-m

distribSetp1:ZH(|)-m configure -val { 1 2 3 4 2 3 4 5 3 4 5 6 4 5 6 7 }
distribSetp1:Y(T<WB>|UW,B<WB>)-m configure -val { 1 2 3 4 2 3 4 5 3 4 5 6 4 5 6 7 }
distribSetp1:Y(T<WB>|UW,B<WB>)-m configure -count 10000
distribSetp1:Y(V<WB>|UW,Z<WB>)-m configure -count 10000

return
