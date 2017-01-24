# ========================================================================
#  JanusRTK   Janus Speech Recognition Toolkit
#             ------------------------------------------------------------
#             Object: Displaying of Tree objects
#             ------------------------------------------------------------
#             Time-stamp: <07 Jul 00 08:17:38 metze>
#
#  Author  :  Torsten Zeppenfeld
#  Module  :  showSTree.tcl
#  Date    :  1995-09-21
#
#  Remarks :  Also see displayTree.tcl
#
# ========================================================================
#  RCS info:  $Id: showSTree.tcl 2390 2003-08-14 11:20:32Z fuegen $
# 
#  $Log$
#  Revision 1.2  2003/08/14 11:19:41  fuegen
#  Merged changes on branch jtk-01-01-15-fms (jaguar -> ibis-013)
#
#  Revision 1.1.52.2  2002/11/04 15:02:08  metze
#  Added documentation code
#
#  Revision 1.1.52.1  2001/09/27 12:29:07  metze
#  Added scrolleable canvas
#
# ========================================================================


proc showSTree_Info { name } {
    switch $name {
	procs {
	    return "showSTree"
	}
	help {
	    return "These functions allow you to view trees, too."
	}
	showSTree {
	    return "Displays a tree object! This procedure does not display an STree object, though!"
	}
    }
    return "???"
}


# Opens a toplevel window that Displays a graph of a tree 'stree' starting 
# at 'StartNode' up to depth = 'depth'.
# Middle Button click in a node starts up a new window starting at the node
# clicked upon.
proc showSTree { args } {

  if {[catch {itfParseArgv showSTree $args [ list [ 
      list "<tree>"      object {} stree     Tree "tree object to display" ] [
      list "<startNode>" string {} StartNode {}   "name of start node (of tree to display)" ] [
      list "<depth>"     string {} depth     {}   "depth of displayed tree" ] ]
  } ] } {
    return
  }

  toplevel .st$StartNode

  set xb 0
  set yb 0
  set xe 1600
  set ye 1200

  frame     .st$StartNode.f
  canvas    .st$StartNode.f.can -relief sunken \
                              -xscrollcommand ".st$StartNode.f.hscroll set" \
                              -yscrollcommand ".st$StartNode.f.vscroll set" \
                              -scrollregion   "0 0 $xe $ye"
  scrollbar .st$StartNode.f.vscroll -command ".st$StartNode.f.can yview"
  scrollbar .st$StartNode.f.hscroll -command ".st$StartNode.f.can xview" -orient horiz
  button    .st$StartNode.quit -text "close" -command "destroy .st$StartNode"
  label     .st$StartNode.ln -text "  node: " -anchor w
  label     .st$StartNode.lq -text " quest: " -anchor w
  label     .st$StartNode.ls -text "senone: " -anchor w

  pack      .st$StartNode.f.vscroll -side   right  -fill y
  pack      .st$StartNode.f.hscroll -side   bottom -fill x  
  pack      .st$StartNode.f.can     -fill both -expand 1

  pack .st$StartNode.f -fill both -expand yes
  pack .st$StartNode.ln .st$StartNode.lq .st$StartNode.ls .st$StartNode.quit -fill x -side top -pady 3 -padx 3
  

  set nodeindex [$stree index $StartNode]
  if {$nodeindex != -1} {
    sstShowNode $stree st$StartNode $nodeindex $xb $yb $xe $ye 1 $depth
  }
}


proc sstDisplayNodeData {stree win node} {
  global SID

  switch florian {
    torsten {
      .$win.ln configure -text \
               "  node: [$stree name $node]"
      .$win.lq configure -text \
               " quest: [$stree.questions name [lindex [lindex [$stree.item($node) configure] 0] 1]]"
      .$win.ls configure -text \
               "senone: [$stree.senoneSet name [lindex [lindex [$stree.item($node) configure] 4] 1]]"
    }
    florian {
      set c 0.0
      foreach i [lrange [lindex [lindex [lindex [lindex [$stree.questionSet.item([$stree.item($node) configure -question])] 1] 0] 0] 0] 1 end] {
        set c [expr $c + [distribSet$SID:${i}(|)-m configure -count]]
      }
      .$win.ln configure -text \
               "  node: [$stree name $node], count=$c"
      .$win.lq configure -text \
               " quest: [lindex [$stree.questionSet.item([$stree.item($node) configure -question])] 0]"
      .$win.ls configure -text \
	  " info: [lindex [lindex [lindex [$stree.questionSet.item([$stree.item($node) configure -question])] 1] 0] 0]"
    }
  }
}

proc sstShowNode {stree win startnode xb yb xe ye curdepth depthN} {
    global SID

    set c 0.0
    foreach i [lrange [lindex [lindex [lindex [lindex [$stree.questionSet.item([$stree.item($startnode) configure -question])] 1] 0] 0] 0] 1 end] {
        set c [expr $c + [distribSet$SID:${i}(|)-m configure -count]]
    }
    set c [expr int($c/1000+0.5)]

  set ovalSize 10
  if {$curdepth > $depthN} {
    return
  }
  set x1 [expr $xb + ($xe - $xb) / 2]
  set y1 [expr $yb + ($ye - $yb) / 8]
  set oval [.$win.f.can create oval $x1 $y1 [expr $x1 + $ovalSize] [expr $y1 + $ovalSize] -fill blue]
  .$win.f.can bind $oval <Button-2> "showSTree $stree [$stree name $startnode] $depthN"
  .$win.f.can bind $oval <Enter> "sstDisplayNodeData $stree $win $startnode"
  .$win.f.can create text [expr $x1 + $ovalSize + 5] $y1 -text \
      "[$stree name $startnode]" -anchor w
  if 0 {
    .$win.f.can create text [expr $x1 + $ovalSize + 5] [expr $y1+50] -text "[lindex [lindex [lindex [$stree.questionSet.item([$stree.item($startnode) configure -question])] 1] 0] 0] = ${c}k" -anchor w
    .$win.f.can create text [expr $x1 - $ovalSize - 5] [expr $y1+20] -text "[lindex [$stree.questionSet.item([$stree.item($startnode) configure -question])] 0]" -anchor e
  }


  set nodeNo [set nodeYes [set nodeUndef -1]]
  foreach i [$stree.item($startnode) configure] {
      switch -- [lindex $i 0] {
	  -no    { set nodeNo    [lindex $i 1] }
	  -yes   { set nodeYes   [lindex $i 1] }
	  -undef { set nodeUndef [lindex $i 1] }
      }
  }

  # UNRELIABLE, not to say faulty:
  # set answer [$stree.item($startnode) configure]
  # set nodeNo [lindex [lindex $answer 1] 1]
  # set nodeYes [lindex [lindex $answer 2] 1]
  # set nodeUndef [lindex [lindex $answer 3] 1]

  set nyb [expr $yb + ($ye - $yb) / 4]
  set nye $ye

  set g [expr ($nodeUndef == -1) ? 2 : 3]
  if {$nodeNo != -1} {
    set nxb $xb
    set nxe [expr $xb + ($xe - $xb) / $g]
    .$win.f.can create line [expr $x1 + $ovalSize / 2] [expr $y1 + 3 * $ovalSize/2] [expr $nxb + $ovalSize/2 + ($nxe - $nxb) / 2] [expr $nyb - $ovalSize/2 + ($nye - $nyb) / 8] -fill red
    sstShowNode $stree $win $nodeNo $nxb $nyb $nxe $nye [expr $curdepth + 1] $depthN
  }
  if {$nodeUndef != -1} {
    set nxb [expr $xb + ($xe - $xb) / 3]
    set nxe [expr $xb + 2 * ($xe - $xb) / 3]
    .$win.f.can create line [expr $x1 + $ovalSize/2] [expr $y1 + 3 * $ovalSize/2] [expr $nxb + $ovalSize/2 + ($nxe - $nxb) / 2] [expr $nyb - $ovalSize/2 + ($nye - $nyb) / 8] -fill black
    sstShowNode $stree $win $nodeUndef $nxb $nyb $nxe $nye [expr $curdepth + 1] $depthN
  }
  if {$nodeYes != -1} {
    set nxb [expr $xb + ($g - 1) * ($xe - $xb) / $g]
    set nxe $xe
    .$win.f.can create line [expr $x1 + $ovalSize/2] [expr $y1 + 3 * $ovalSize/2] [expr $nxb + $ovalSize/2 + ($nxe - $nxb) / 2] [expr $nyb - $ovalSize/2 + ($nye - $nyb) / 8] -fill green
    sstShowNode $stree $win $nodeYes $nxb $nyb $nxe $nye [expr $curdepth + 1] $depthN
  }
}
