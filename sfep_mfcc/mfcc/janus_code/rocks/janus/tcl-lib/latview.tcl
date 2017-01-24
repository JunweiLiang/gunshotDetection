# ========================================================================
#  JANUS-SR   Janus Speech Recognition Toolkit
#             ------------------------------------------------------------
#             Object: Routines to display lattices
#             ------------------------------------------------------------
#
#  Author  :  Florian Metze, Wenzel Svojanowsky
#  Module  :  latview.tcl
#  Date    :  $Id: latview.tcl 2390 2003-08-14 11:20:32Z fuegen $
#
#  Remarks :  
#
# ========================================================================
#
# $Log$
# Revision 1.2  2003/08/14 11:20:28  fuegen
# Merged changes on branch jtk-01-01-15-fms (jaguar -> ibis-013)
#
# Revision 1.1.2.2  2003/06/26 15:09:20  metze
# Changes for V5.0 P013
#
# Revision 1.1.2.1  2003/06/06 09:37:50  metze
# As written by Wenzel
#
# ========================================================================


proc latview_Info { name } {
    switch $name {
        procs {
            return "showlat"
        }
        help {
            return "A viewer for GLat objects."
        }
        showlat {
            return "Display the contents of a GLat lattice in a Tk window.
                    Be careful with large objects."
        }
    }
    return "???"
}


#Definiere Canvas-Variablen
set lv_linealoffset 5
set lv_canvasheight 100
set lv_canvaswidth 100
set lv_nodeoffset 0.2

set lv_version 1.0

#set windowcount 0

set lv_canvasyScale 1.0
set lv_canvasxScale 3.0

set lv_aktMarkedNode -5

set lv_latObject 0

set lv_marknumbers []

set lv_windowcount 0

proc lv_scroll {a1 a2 args} {
  eval $a1 $args
  eval $a2 $args
}

proc lv_drawLineal {} {
  global lv_canvasyScale lv_canvasxScale w lv_linealoffset lv_latticeWidth
  $w.lineal delete all
  set i 0
  while {$i <[expr $lv_latticeWidth +3]}\
  {
    set x [expr 1.0*$lv_linealoffset+$i/$lv_canvasxScale]
    $w.lineal create line ${x}c 0c ${x}c 0.2c
    if {$i % 5 ==0} {
      set y 0.6
      if {$i % 2 ==0} {set y 0.8}
      $w.lineal create line ${x}c 0c ${x}c ${y}c
      set y [expr $y + 0.2]
      $w.lineal create text ${x}c ${y}c -text "$i" -anchor center -tags text
    }
    set i [expr $i+1]
  }
}

proc lv_zoomin {} {
global lv_canvasyScale lv_canvasxScale lv_canvaswidth lv_canvasheight w lv_latticeWidth
  if {$lv_canvasyScale < 3.0} {
    set lv_canvasxScale [expr $lv_canvasxScale * 0.7]
    set lv_canvasyScale [expr $lv_canvasyScale / 0.7]
    set lv_canvaswidth [expr $lv_latticeWidth/$lv_canvasxScale +12]
    set lv_canvasheight [expr $lv_canvasheight /0.7]
    eval $w.lineal configure [format "-scrollregion {0c 0c %4.1fc 20c}" $lv_canvaswidth]
    eval $w.c configure [format "-scrollregion {0c 0c %4.1fc %4.1fc}" $lv_canvaswidth $lv_canvasheight]
    lv_drawnodesfunction
    lv_drawLineal
  }
}

proc lv_zoomout {} {
global lv_canvasyScale lv_canvasxScale lv_canvaswidth lv_canvasheight w lv_latticeWidth
  if {$lv_canvasyScale > 0.2} {
    set lv_canvasxScale [expr $lv_canvasxScale / 0.7]
    set lv_canvasyScale [expr $lv_canvasyScale * 0.7]
    set lv_canvaswidth [expr $lv_latticeWidth/$lv_canvasxScale +12]
    set lv_canvasheight [expr $lv_canvasheight *0.7]
    eval $w.lineal configure [format "-scrollregion {0c 0c %4.1fc 20c}" $lv_canvaswidth]
    eval $w.c configure [format "-scrollregion {0c 0c %4.1fc %4.1fc}" $lv_canvaswidth $lv_canvasheight]
    lv_drawnodesfunction
    lv_drawLineal
  }
}

proc lv_drawNode {n} \
{
   global lv_latticeWidth lv_latticeHeight lv_canvasxScale lv_canvasyScale lv_linealoffset lv_nodeA w lv_linkA lv_nodeoffset

    if {$n < 0} {
     #Startnode
     if {$n == -1} {
       set x1 [expr (-1.0) / $lv_canvasxScale +$lv_linealoffset]
       set x2 [expr (-5.0) / $lv_canvasxScale +$lv_linealoffset]
     } else {
     #Endnode
       set x1 [expr 1.0* ($lv_latticeWidth +1) / $lv_canvasxScale +$lv_linealoffset]
       set x2 [expr 1.0* ($lv_latticeWidth +5) / $lv_canvasxScale +$lv_linealoffset]
     }
    } else {
     #Jede andere Node!
     set x1 [expr 1.0*$lv_nodeA($n,from)/$lv_canvasxScale +$lv_linealoffset]
     set x2 [expr 1.0*$lv_nodeA($n,to)/$lv_canvasxScale +$lv_linealoffset]
   }
   set y1 [expr (1.0*$lv_nodeA($n,yPos)+0.1)*$lv_canvasyScale]
   set y2 [expr (1.0*$lv_nodeA($n,yPos)+0.9)*$lv_canvasyScale]

     ##setze Node und bind Event
    set lv_nodeA($n,rectNumber) [$w.c create rectangle ${x1}c ${y1}c ${x2}c ${y2}c -width 1 -outline blue -fill white]
    eval "$w.c bind  $lv_nodeA($n,rectNumber) <Button-1> {lv_clickon $n}"


    set tempNum [$w.c create text [expr ($x1+$x2)/2]c [expr $y1+0.3]c -text [string trim $lv_nodeA($n,text)] -anchor center -tags text]
    eval "$w.c bind  $tempNum <Button-1> {lv_clickon $n}"


    foreach i $lv_nodeA($n,outLinks) {

	if {$lv_linkA($i,nodeX) == -2} { continue }

	set j  $lv_linkA($i,nodeY)
	set x1 [expr 1.0*$lv_nodeA($n,to) /$lv_canvasxScale +$lv_linealoffset -$lv_nodeoffset]
	set y1 [expr (1.0*$lv_nodeA($n,yPos) +0.5)*$lv_canvasyScale]
	set x2 [expr 1.0*$lv_nodeA($j,from) /$lv_canvasxScale +$lv_linealoffset +$lv_nodeoffset]
	set y2 [expr (1.0*$lv_nodeA($j,yPos) +0.5)*$lv_canvasyScale]
	#Ist's die Endenode
	if {$j == -2} {set x2 [expr 1.0* ($lv_latticeWidth +1) / $lv_canvasxScale +$lv_linealoffset+$lv_nodeoffset]}
	#Ist's Startnode
	if {$n == -1} {set x1 [expr (-1.0) / $lv_canvasxScale +$lv_linealoffset -$lv_nodeoffset] }
	$w.c create line ${x1}c ${y1}c ${x2}c ${y2}c
    }
}

## wird ausgeführt, wenn auf die Node geklickt wird
proc lv_clickon {node} {
  global lv_aktMarkedNode
  if {$lv_aktMarkedNode==$node}  {
    lv_unmarknode $lv_aktMarkedNode
    set lv_aktMarkedNode -5
  } else {
    lv_unmarknode $lv_aktMarkedNode
    set lv_aktMarkedNode $node
    lv_marknode $node
  }
}

proc lv_marknode {node} {
  global w lv_nodeA lv_linkA lv_linealoffset lv_canvasxScale lv_canvasyScale lv_nodeoffset stringborder lv_marknumbers lv_latticeWidth

  $w.infoFrame.nodenrvalue configure -text "$node"
  $w.infoFrame.scorevalue configure -text "$lv_nodeA($node,score)"
  $w.infoFrame.alphavalue configure -text "$lv_nodeA($node,alpha)"
  $w.infoFrame.betavalue configure -text "$lv_nodeA($node,beta)"
  $w.infoFrame.gammavalue configure -text "$lv_nodeA($node,gamma)"
  $w.infoFrame.textvalue configure -text "$lv_nodeA($node,text)"
  $w.infoFrame.tovalue configure -text "$lv_nodeA($node,to)"
  $w.infoFrame.fromvalue configure -text "$lv_nodeA($node,from)"

  set stringborder $node

  # färbe die Node
  $w.c itemconfigure $lv_nodeA($node,rectNumber) -fill red

  #male Linien als outLinks
  if {$node>=0 || $node==-1} {
    foreach i $lv_nodeA($node,outLinks) {
      set j  $lv_linkA($i,nodeY)
      set x1 [expr 1.0*$lv_nodeA($node,to) /$lv_canvasxScale +$lv_linealoffset -$lv_nodeoffset]
      set y1 [expr (1.0*$lv_nodeA($node,yPos) +0.5)*$lv_canvasyScale]
      set x2 [expr 1.0*$lv_nodeA($j,from) /$lv_canvasxScale +$lv_linealoffset +$lv_nodeoffset]
      set y2 [expr (1.0*$lv_nodeA($j,yPos) +0.5)*$lv_canvasyScale]
      #Ist's Startnode
      if {$node == -1} {set x1 [expr (-1.0) / $lv_canvasxScale +$lv_linealoffset -$lv_nodeoffset] }
      #Ist's die Endenode
      if {$j == -2} {set x2 [expr 1.0* ($lv_latticeWidth +1) / $lv_canvasxScale +$lv_linealoffset+$lv_nodeoffset]}
      set number [$w.c create line ${x1}c ${y1}c ${x2}c ${y2}c -fill red -width 3]
      lappend lv_marknumbers $number

      #Male das Rechteck
      set x1 [expr 1.0*$lv_nodeA($j,from)/$lv_canvasxScale +$lv_linealoffset]
      set x2 [expr 1.0*$lv_nodeA($j,to)/$lv_canvasxScale +$lv_linealoffset]
      if {$j == -2} {
       set x1 [expr 1.0* ($lv_latticeWidth +1) / $lv_canvasxScale +$lv_linealoffset]
       set x2 [expr 1.0* ($lv_latticeWidth +5) / $lv_canvasxScale +$lv_linealoffset]
      }
      if {$j == -1} {continue}
      set y1 [expr (1.0*$lv_nodeA($j,yPos)+0.1)*$lv_canvasyScale]
      set y2 [expr (1.0*$lv_nodeA($j,yPos)+0.9)*$lv_canvasyScale]
      set number [$w.c create rectangle ${x1}c ${y1}c ${x2}c ${y2}c -width 3 -outline red]
      lappend lv_marknumbers $number
    }
  }

  #male Linien als inLinks
  if {$node>=0 || $node==-2} {
    foreach i $lv_nodeA($node,inLinks) {
      set j  $lv_linkA($i,nodeX)
      set x1 [expr 1.0*$lv_nodeA($node,from) /$lv_canvasxScale +$lv_linealoffset +$lv_nodeoffset]
      set y1 [expr (1.0*$lv_nodeA($node,yPos) +0.5)*$lv_canvasyScale]
      set x2 [expr 1.0*$lv_nodeA($j,to) /$lv_canvasxScale +$lv_linealoffset -$lv_nodeoffset]
      set y2 [expr (1.0*$lv_nodeA($j,yPos) +0.5)*$lv_canvasyScale]
      #Ist's die Endenode
      if {$node == -2} {set x1 [expr 1.0* ($lv_latticeWidth +1) / $lv_canvasxScale +$lv_linealoffset+$lv_nodeoffset]}
      #Ist's Startnode
      if {$j == -1} {set x2 [expr (-1.0) / $lv_canvasxScale +$lv_linealoffset -$lv_nodeoffset] }
      if {$j == -2} {continue}
      set number [$w.c create line ${x1}c ${y1}c ${x2}c ${y2}c -fill red -width 3]
      lappend lv_marknumbers $number

      #Male das Rechteck
      set x1 [expr 1.0*$lv_nodeA($j,from)/$lv_canvasxScale +$lv_linealoffset]
      set x2 [expr 1.0*$lv_nodeA($j,to)/$lv_canvasxScale +$lv_linealoffset]
      if {$j == -1} {
        set x1 [expr (-1.0) / $lv_canvasxScale +$lv_linealoffset]
        set x2 [expr (-5.0) / $lv_canvasxScale +$lv_linealoffset]
      }
      if {$j == -2} {continue}
      set y1 [expr (1.0*$lv_nodeA($j,yPos)+0.1)*$lv_canvasyScale]
      set y2 [expr (1.0*$lv_nodeA($j,yPos)+0.9)*$lv_canvasyScale]
      set number [$w.c create rectangle ${x1}c ${y1}c ${x2}c ${y2}c -width 3 -outline red]
      lappend lv_marknumbers $number
    }
  }
}

proc lv_unmarknode {node} {
  global w lv_nodeA lv_marknumbers
  $w.infoFrame.nodenrvalue configure -text ""
  $w.infoFrame.scorevalue configure -text ""
  $w.infoFrame.alphavalue configure -text ""
  $w.infoFrame.betavalue configure -text ""
  $w.infoFrame.gammavalue configure -text ""
  $w.infoFrame.textvalue configure -text ""
  $w.infoFrame.tovalue configure -text ""
  $w.infoFrame.fromvalue configure -text ""
  # färbe die Node
  if {$node != -5} {
    $w.c itemconfigure $lv_nodeA($node,rectNumber) -fill white
    foreach i $lv_marknumbers { $w.c delete $i }
  }
}

#Diese Prozedur löscht einen Knoten aus einer Liste
proc lv_NodeListDelete {li n} {
    upvar $li l
    set i [lsearch $l $n]
    set l [lreplace $l $i $i]
}

proc lv_minKostcompare {a b} \
{
  global lv_nodeA
  if {$lv_nodeA($a,minKost) < $lv_nodeA($b,minKost)} \
  {
    return -1
  }
  return 1
}

#IterationsFunktionen

set lv_collpower 0.30
set lv_collborder 15.00
set lv_stringpower 0.30


proc lv_singleoptfunction {} {
  global lv_NodeList lv_nodeA
  foreach i $lv_NodeList {
    if {$i>=0} {
      if {[llength $lv_nodeA($i,outLinks)]==1} {
        set lv_nodeA([lv_folgeNode $i 0],yPos) $lv_nodeA($i,yPos)
      }
    }
  }
}

proc lv_autoiterationfunction {} {
  global lv_collpower lv_stringpower lv_NodeList lv_nodeA

  set lv_collpower 0.1
  set lv_stringpower 0.4

  for {set i 0} {$i < 10} {incr i 1} {
    lv_iterationstring
    lv_iterationcollision
    set lv_collpower [expr $lv_collpower * 1.02]
    set lv_stringpower [expr $lv_stringpower * 0.95]
  }

  set countcollisions 0
  foreach j $lv_NodeList {
    set countcollisions [expr $countcollisions + [llength [lv_collCheck $j]]]
  }

  lv_singleoptfunction

  set autoabbruch 1
  while {$autoabbruch} {

    set lv_collpower 1.0
    for {set i 0} {$i < [expr $countcollisions/8 +1]} {incr i 1} {lv_iterationcollision}

    set countcollisions 0
    foreach j $lv_NodeList {
      set countcollisions [expr $countcollisions + [llength [lv_collCheck $j]]]
    }
    if {$countcollisions == 0} { set autoabbruch 0}
  }

  lv_singleoptfunction

  set countcollisions 0
  foreach j $lv_NodeList {
    set countcollisions [expr $countcollisions + [llength [lv_collCheck $j]]]
  }

  set lv_collpower 0.1
  set lv_stringpower 0.4
  for {set i 0} {$i < 6} {incr i 1} {
    lv_iterationstring
    lv_iterationcollision
    set lv_collpower [expr $lv_collpower * 1.02]
    set lv_stringpower [expr $lv_stringpower * 0.95]
  }

  set autoabbruch 1
  while {$autoabbruch} {

    set lv_collpower 1.0
    for {set i 0} {$i < [expr $countcollisions/4 +1]} {incr i 1} {lv_iterationcollision}

    set countcollisions 0
    foreach j $lv_NodeList {
      set countcollisions [expr $countcollisions + [llength [lv_collCheck $j]]]
    }

    if {$countcollisions == 0} { set autoabbruch 0}
  }

  # Anpassen vom Endknoten
  set lv_endnodecount 0
  set lv_endnodevalue 0
  foreach i [lv_listVorNode -2] {
    incr lv_endnodecount 1
    set lv_endnodevalue [expr $lv_endnodevalue + $lv_nodeA($i,yPos)]
  }
  set lv_nodeA(-2,yPos) [expr $lv_endnodevalue / $lv_endnodecount]


 lv_drawnodesfunction
}


proc lv_drawnodesfunction {} {
  global lv_NodeList w iterations lv_aktMarkedNode
  lv_nodestranspose
  $w.c delete all
  foreach i $lv_NodeList {lv_drawNode $i}
  if {$lv_aktMarkedNode!=-5}  {
    lv_marknode $lv_aktMarkedNode
  }
}


set lv_stringiterations 0
set lv_colliterations   0

proc lv_iterationstring { } \
{
  global lv_stringiterations lv_NodeList lv_nodeA lv_linkA w lv_stringpower
  incr lv_stringiterations 1

  foreach i $lv_NodeList \
  {
    #initialisiere die Kraft
    set lv_nodeA($i,yForce) 0.0
    set outForce 0.0
    set inForce 0.0

    #Berechne outForce
    #die Kraft, die von den outLinks auf die Node wirkt
    set outLength [llength $lv_nodeA($i,outLinks)]
    if {$outLength > 0} \
    {
      for {set j 0} {$j< $outLength} {incr j 1} \
      {
        set outForce [expr $outForce + $lv_nodeA([lv_folgeNode $i $j],yPos) - $lv_nodeA($i,yPos)]
      }
      set outForce [expr $outForce/$outLength]
    }

    #berechne inForce
    #die Kraft, die von den inLinks auf die Node wirkt
    set inLength [llength $lv_nodeA($i,inLinks)]
    if {$inLength > 0} \
    {
      for {set j 0} {$j< $inLength} {incr j 1} \
      {
        set inForce [expr $inForce + $lv_nodeA([lv_vorNode $i $j],yPos) - $lv_nodeA($i,yPos)]
      }
      set inForce [expr $inForce/$inLength]
    }

    #bestimme die Bewegung
    set lv_nodeA($i,yForce) [expr $lv_stringpower*$outForce + $lv_stringpower*$inForce]

  }

  foreach i $lv_NodeList \
  {
    set lv_nodeA($i,yPos) [expr $lv_nodeA($i,yPos) + $lv_nodeA($i,yForce)]
  }
}

proc lv_iterationcollision { } \
{
  global lv_colliterations lv_NodeList lv_nodeA lv_linkA w
  global lv_collpower lv_collborder
  incr lv_colliterations 1

  foreach i $lv_NodeList \
  {
    #initialisiere die Kraft
    set lv_nodeA($i,yForce) 0.0
    set collForce 0.0

    #berechne collForce
    #die Kraft, die durch Kollisionen auf die Node wirkt
    set lll [lv_collCheck $i]
    if {[llength $lll] > 0} {
      foreach j $lll {
	  set a [expr 1.0*$lv_nodeA($i,yPos)]
	  set b [expr 1.0*$lv_nodeA($j,yPos)]
	  if {$a != $b} {
	      set wert [expr 1.0-abs($a-$b)]
	      if {$a<$b} {set wert [expr -$wert]}
	  } else {
                        set xxx [expr 0.1 * [llength $lll]]
	     set wert [expr 0.0001*$xxx * ($lv_nodeA($i,minKost)-$lv_nodeA($j,minKost))]
	  }

        set collForce [expr $collForce + $wert]
      }
    }

    #bestimme die Bewegung
    set lv_nodeA($i,yForce) [expr 1.0*$lv_collpower*$collForce]
  }

  foreach i $lv_NodeList \
  {
    if {[expr abs($lv_nodeA($i,yForce))] <= $lv_collborder} \
    {
      set lv_nodeA($i,yPos) [expr $lv_nodeA($i,yPos) + $lv_nodeA($i,yForce)]
    } else \
    {
      if {$lv_nodeA($i,yForce)>= 0.0} \
      {
        set lv_nodeA($i,yPos) [expr $lv_nodeA($i,yPos) + $lv_collborder]
      } else \
      {
        set lv_nodeA($i,yPos) [expr $lv_nodeA($i,yPos) - $lv_collborder]
      }
    }
  }
}
proc lv_collCheck {node} {
  global lv_nodeA
  set colllist { }
  set pos1 [expr $lv_nodeA($node,yPos) - 0.9995]
  set pos2 [expr $lv_nodeA($node,yPos) + 0.9995]
  #erstelle Kollisionsliste
  foreach j $lv_nodeA($node,collisions) {
    if {($lv_nodeA($j,yPos) > $pos1) && ($lv_nodeA($j,yPos) < $pos2)} \
    {
      lappend colllist $j
    }
  }
  #Kollisionsliste erstellt in colllist
  return $colllist
}

proc lv_betrag {zahl} {
  if {$zahl >= 0.00} {
    return $zahl
  } else {
    return [expr 0.00-$zahl]
  }
}

proc lv_nodestranspose { } {
  global lv_NodeList lv_nodeA lv_linkA w

  set yMin 100000

  foreach i $lv_NodeList \
  {
    if {$lv_nodeA($i,yPos) < $yMin} {set yMin $lv_nodeA($i,yPos)}
  }

  set yMin [expr $yMin -1]

  foreach i $lv_NodeList \
  {
    set lv_nodeA($i,yPos) [expr $lv_nodeA($i,yPos) - $yMin]
  }
}


proc lv_listVorNode {node} \
{
  global lv_nodeA lv_linkA
  set ergebnis { }
  for {set index 0} {$index<[llength $lv_nodeA($node,inLinks)]} {incr index 1} \
    {lappend ergebnis $lv_linkA([lindex $lv_nodeA($node,inLinks) $index],nodeX)}

  return $ergebnis

}
proc lv_listFolgeNode {node} \
{
  global lv_nodeA lv_linkA
  set ergebnis {}
  for {set index 0} {$index<[llength $lv_nodeA($node,outLinks)]} {incr index 1} \
    {lappend ergebnis $lv_linkA([lindex $lv_nodeA($node,outLinks) $index],nodeY);}

  return $ergebnis
}

proc lv_vorNode {node index} \
{
  global lv_nodeA lv_linkA
  return $lv_linkA([lindex $lv_nodeA($node,inLinks) $index],nodeX)
}

proc lv_folgeNode {node index} \
{
  global lv_nodeA lv_linkA
  return $lv_linkA([lindex $lv_nodeA($node,outLinks) $index],nodeY)
}


#######################################################################
#                                                                     #
#  MAIN PROCEDURE                                                     #
#                                                                     #
#######################################################################

proc showlat { obj args } {

    global lv_nodeA lv_linkA w lv_version lv_linealoffset lv_canvasheight lv_windowcount
    global lv_canvaswidth lv_nodeoffset lv_canvasyScale lv_canvasxScale lv_NodeList
    global lv_aktMarkedNode lv_latObject lv_marknumbers lv_latticeWidth lv_latticeHeight

set lv_latObject $obj
############################
##source nodeinit.tcl
#Lese Lattice-Objekt ein!
set lv_NodeList {}
set lv_linkcount 0
set lv_latticeWidth 0
set nodeN [$lv_latObject configure -nodeN]
for {set i -2} {$i < $nodeN} {incr i} \
{
  set lv_nodeA($i,inLinks) []
  set lv_nodeA($i,outLinks) []
  set lv_nodeA($i,minKost) 0
  set lv_nodeA($i,minLink) 0
  set lv_nodeA($i,yPos) 0
  set lv_nodeA($i,collisions) []

  set lv_inf [$lv_latObject puts -nodeX $i]
  set lv_NodeList [linsert $lv_NodeList end $i]

  # Position des Textes
  set x [string first " : " $lv_inf]
  incr x 3
  set y [string first " frame= " $lv_inf]
  set lv_nodeA($i,text) [string range $lv_inf $x [expr $y -1]]
  set x [expr $y + 8]
  set lv_inf [string range $lv_inf $x end]

  # from to
  set x [string first " " $lv_inf]
  set y [string first " score= " $lv_inf]
  set lv_nodeA($i,from) [string range $lv_inf 0 [expr $x -1]]
  incr x 1
  set lv_nodeA($i,to) [string range $lv_inf $x [expr $y -1]]
  incr y 8
  set lv_inf [string range $lv_inf $y end]

  # score
  set x [string first " " $lv_inf]
  set lv_nodeA($i,score) [string range $lv_inf 0 [expr $x -1]]

  # alpha beta gamma
  incr x 9
  set y [string first "  beta= " $lv_inf]
  set lv_nodeA($i,alpha) [string range $lv_inf $x [expr $y -1]]
  set x [expr $y + 8]
  set y [string first " gamma= " $lv_inf]
  set lv_nodeA($i,beta) [string range $lv_inf $x [expr $y -1]]
  set x [expr $y + 8]
  set y [string first "   link" $lv_inf]
  incr y -2
  if {$y < 0} {set lv_nodeA($i,gamma) [string range $lv_inf $x end]
  } else {

    set lv_nodeA($i,gamma) [string range $lv_inf $x $y]
    incr y 2
    set lv_inf [string range $lv_inf $y end]
    while {[string first "   link " $lv_inf] != -1} {

      set lv_linkA($lv_linkcount,nodeX) $i

      set x [string first " : " $lv_inf]
      incr x 8
      set lv_inf [string range $lv_inf $x end]
      set x [string first "frame=" $lv_inf]
      incr x 6
      set lv_inf [string range $lv_inf $x end]
      set x [string first " score= " $lv_inf]
      incr x 8
      set y [string first " node= " $lv_inf]
      set lv_linkA($lv_linkcount,score) [string range $lv_inf $x [expr $y -1]]

      set x [expr $y + 7]
      set y [string first "   link" $lv_inf]
      incr y -2
      if {$y < 0} {set lv_linkA($lv_linkcount,nodeY) [string range $lv_inf $x [expr [string length $lv_inf] -2]]
      } else {set lv_linkA($lv_linkcount,nodeY) [string range $lv_inf $x $y]}
      incr y 2
      set lv_inf [string range $lv_inf $y end]

      set lv_nodeA($i,outLinks) [linsert $lv_nodeA($i,outLinks) end $lv_linkcount]

      incr lv_linkcount 1
    }


  }

  if {$lv_nodeA($i,to) > $lv_latticeWidth} {set lv_latticeWidth $lv_nodeA($i,to)}
}
## Berechnug der inLinks
for {set i 0} {$i < $lv_linkcount} {incr i} {
  set lv_nodeA($lv_linkA($i,nodeY),inLinks) [linsert $lv_nodeA($lv_linkA($i,nodeY),inLinks) end $i]
}

#Berechne Kollisionslisten
set restListe $lv_NodeList
foreach i $lv_NodeList {
  #Lösche Node aus Restliste
  set elempos [lsearch $restListe $i]
  set restListe [lreplace $restListe $elempos $elempos]

  foreach j $restListe \
  {
    if {($lv_nodeA($i,from)<=$lv_nodeA($j,to)) && ($lv_nodeA($i,from)>=$lv_nodeA($j,from)) &&  ($j>=0)} \
    {
      lappend lv_nodeA($i,collisions) $j
      lappend lv_nodeA($j,collisions) $i
    } elseif {($lv_nodeA($i,to)<=$lv_nodeA($j,to)) && ($lv_nodeA($i,to)>=$lv_nodeA($j,from)) &&  ($j>=0)} \
    {
      lappend lv_nodeA($i,collisions) $j
      lappend lv_nodeA($j,collisions) $i
    } elseif {($lv_nodeA($i,to)>=$lv_nodeA($j,to)) && ($lv_nodeA($i,from)<=$lv_nodeA($j,from)) &&  ($j>=0)} \
    {
      lappend lv_nodeA($i,collisions) $j
      lappend lv_nodeA($j,collisions) $i
    }
  }
}

#Berechne Höhe der Lattice
set lv_latticeHeight 0
foreach i $lv_NodeList \
{
  if {[llength $lv_nodeA($i,collisions)] > $lv_latticeHeight }  { set lv_latticeHeight [llength $lv_nodeA($i,collisions)]}
}

incr lv_latticeHeight 1

#############################################
###end nodeinit.tcl

#############################################
###source path.tcl

#Initialisiere Pfadsuche
set pathlist {-1}
set i 0
#Pfadsuche
while {[llength $pathlist]>0} \
{
  set n [lindex $pathlist 0]
  foreach i $lv_nodeA($n,outLinks) \
  {
    #Berechne den Folgeknoten und die Minkosten
    set tmpminkost [expr $lv_nodeA($n,minKost)+$lv_linkA($i,score)]
    set tmpnode $lv_linkA($i,nodeY)
    set tmpminkost [expr $tmpminkost + $lv_nodeA($tmpnode,score)]

    if {$lv_nodeA($tmpnode,minKost)==0 || $tmpminkost<$lv_nodeA($tmpnode,minKost)} \
    {
      set lv_nodeA($tmpnode,minKost) $tmpminkost
      set lv_nodeA($tmpnode,minLink) $i
#Hier muss noch verhindert werden, dass Node 2 mal in der Liste, sollte dann nach hinten
      set pathlist [linsert $pathlist 100000 $tmpnode]
    }
  lv_NodeListDelete pathlist $n
  }
}

#Erstelle Liste mit besten Pfad
set bestpath {-2}
#Lese besten Pfad
set tmpnode -2
while {$tmpnode!=-1} {
  set tmplink $lv_nodeA($tmpnode,minLink)
  set tmpnode $lv_linkA($tmplink,nodeX)
  set bestpath [linsert $bestpath 0 $tmpnode]
}

#die jeweiligen Minkosten-Berechnungen sind abgeschlossen

#Sortierung von NodeList nach minKost
set lv_NodeList [lsort -command lv_minKostcompare $lv_NodeList]

#############################################
###end path.tcl

#############################################
###source newWindow.tcl

set w .win${lv_windowcount}
toplevel $w
wm title $w [format "lattice-view %1.1f" $lv_version]
incr lv_windowcount 1

set c $w.c

frame $w.lv_grid
scrollbar $w.hscroll -orient horiz -command "lv_scroll $w.c $w.lineal xview"
scrollbar $w.vscroll -command "$w.c yview"

set lv_canvaswidth [expr $lv_latticeWidth/$lv_canvasxScale +12]
set lv_canvasheight $lv_latticeHeight

canvas $w.lineal -xscrollcommand "$w.hscroll set" -relief sunken -borderwidth 2 -height 1.1c
eval $w.lineal configure [format "-scrollregion {0c 0c %4.1fc 20c}" $lv_canvaswidth]

canvas $w.c -xscrollcommand "$w.hscroll set" -yscrollcommand "$w.vscroll set" -relief sunken -borderwidth 2
eval $w.c configure [format "-scrollregion {0c 0c %4.1fc %4.1fc}" $lv_canvaswidth $lv_canvasheight]

frame $w.infoFrame -relief groove -borderwidth 4 -height 1.5c
frame $w.infoButtonFrame -relief groove -borderwidth 4 -height 1.5c

button $w.zoominbutton  -text "+" -command lv_zoomin
button $w.zoomoutbutton  -text "-" -command lv_zoomout
button $w.optibutton  -text "o" -command lv_autoiterationfunction

pack $w.lv_grid -expand yes -fill both -padx 1 -pady 1

grid rowconfig    $w.lv_grid 4 -weight 1 -minsize 0
grid columnconfig $w.lv_grid 0 -weight 1 -minsize 0

grid rowconfig $w.lv_grid 0 -weight 0 -minsize 0
grid columnconfig $w.lv_grid 1 -weight 1 -minsize 0

#Frame oben
grid $w.infoFrame -padx 1 -in $w.lv_grid -pady 1 \
  -row 0 -column 0 -rowspan 3 -columnspan 1 -sticky news
#Zoombuttons
grid $w.zoominbutton -padx 1 -in $w.lv_grid -pady 1 \
  -row 0 -column 1 -rowspan 1 -columnspan 2 -sticky news
grid $w.zoomoutbutton -padx 1 -in $w.lv_grid -pady 1 \
  -row 2 -column 1 -rowspan 1 -columnspan 2 -sticky news
#Lineal
grid $w.lineal -padx 1 -in $w.lv_grid -pady 1 \
  -row 3 -column 0 -rowspan 1 -columnspan 3 -sticky news
#Fläche
grid $w.c -padx 1 -in $w.lv_grid -pady 1 \
  -row 4 -column 0 -rowspan 1 -columnspan 2 -sticky news
#scrollbars
grid $w.vscroll -in $w.lv_grid -padx 1 -pady 1 \
  -row 4 -column 2 -rowspan 1 -columnspan 1 -sticky news
grid $w.hscroll -in $w.lv_grid -padx 1 -pady 1 \
  -row 5 -column 0 -rowspan 1 -columnspan 2 -sticky news
#Optimierungsbutton
grid $w.optibutton -in $w.lv_grid -padx 1 -pady 1 \
  -row 1 -column 1 -rowspan 1 -columnspan 2 -sticky news

lv_drawLineal
#############################################
###end newWindow.tcl

#############################################
###source graphics.tcl

label $w.infoFrame.nodenr -text "Number" -width 10 -anchor e
label $w.infoFrame.nodenrvalue -text "" -width 20 -relief sunken
label $w.infoFrame.score -text "Score" -width 10 -anchor e
label $w.infoFrame.scorevalue -text "" -width 20 -relief sunken
label $w.infoFrame.alpha -text "Alpha" -width 10 -anchor e
label $w.infoFrame.alphavalue -text "" -width 20 -relief sunken
label $w.infoFrame.beta -text "Beta" -width 10 -anchor e
label $w.infoFrame.betavalue -text "" -width 20 -relief sunken
label $w.infoFrame.gamma -text "Gamma" -width 10 -anchor e
label $w.infoFrame.gammavalue -text "" -width 20 -relief sunken
label $w.infoFrame.text -text "Text" -width 10 -anchor e
label $w.infoFrame.textvalue -text "" -width 20 -relief sunken
label $w.infoFrame.to -text "To" -width 10 -anchor e
label $w.infoFrame.tovalue -text "" -width 20 -relief sunken
label $w.infoFrame.from -text "From" -width 10 -anchor e
label $w.infoFrame.fromvalue -text "" -width 20 -relief sunken
label $w.infoFrame.latname -text "Name" -width 10 -anchor e
label $w.infoFrame.latnamevalue -text $lv_latObject -width 20 -relief sunken

label $w.infoFrame.message -text ""


grid $w.infoFrame.nodenr -padx 1 -in $w.infoFrame -pady 1 \
  -row 0 -column 0 -rowspan 1 -columnspan 1 -sticky news
grid $w.infoFrame.from -padx 1 -in $w.infoFrame -pady 1 \
  -row 1 -column 0 -rowspan 1 -columnspan 1 -sticky news
grid $w.infoFrame.to -padx 1 -in $w.infoFrame -pady 1 \
  -row 2 -column 0 -rowspan 1 -columnspan 1 -sticky news
grid $w.infoFrame.nodenrvalue -padx 1 -in $w.infoFrame -pady 1 \
  -row 0 -column 1 -rowspan 1 -columnspan 1 -sticky news
grid $w.infoFrame.fromvalue -padx 1 -in $w.infoFrame -pady 1 \
  -row 1 -column 1 -rowspan 1 -columnspan 1 -sticky news
grid $w.infoFrame.tovalue -padx 1 -in $w.infoFrame -pady 1 \
  -row 2 -column 1 -rowspan 1 -columnspan 1 -sticky news


grid $w.infoFrame.alpha -padx 1 -in $w.infoFrame -pady 1 \
  -row 0 -column 2 -rowspan 1 -columnspan 1 -sticky news
grid $w.infoFrame.beta -padx 1 -in $w.infoFrame -pady 1 \
  -row 1 -column 2 -rowspan 1 -columnspan 1 -sticky news
grid $w.infoFrame.gamma -padx 1 -in $w.infoFrame -pady 1 \
  -row 2 -column 2 -rowspan 1 -columnspan 1 -sticky news
grid $w.infoFrame.alphavalue -padx 1 -in $w.infoFrame -pady 1 \
  -row 0 -column 3 -rowspan 1 -columnspan 1 -sticky news
grid $w.infoFrame.betavalue -padx 1 -in $w.infoFrame -pady 1 \
  -row 1 -column 3 -rowspan 1 -columnspan 1 -sticky news
grid $w.infoFrame.gammavalue -padx 1 -in $w.infoFrame -pady 1 \
  -row 2 -column 3 -rowspan 1 -columnspan 1 -sticky news


grid $w.infoFrame.score -padx 1 -in $w.infoFrame -pady 1 \
  -row 1 -column 4 -rowspan 1 -columnspan 1 -sticky news
grid $w.infoFrame.text -padx 1 -in $w.infoFrame -pady 1 \
  -row 0 -column 4 -rowspan 1 -columnspan 1 -sticky news
grid $w.infoFrame.latname -padx 1 -in $w.infoFrame -pady 1 \
  -row 2 -column 4 -rowspan 1 -columnspan 1 -sticky news
grid $w.infoFrame.scorevalue -padx 1 -in $w.infoFrame -pady 1 \
  -row 1 -column 5 -rowspan 1 -columnspan 1 -sticky news
grid $w.infoFrame.textvalue -padx 1 -in $w.infoFrame -pady 1 \
  -row 0 -column 5 -rowspan 1 -columnspan 1 -sticky news
grid $w.infoFrame.latnamevalue -padx 1 -in $w.infoFrame -pady 1 \
  -row 2 -column 5 -rowspan 1 -columnspan 1 -sticky news

#grid $w.infoFrame.message -padx 1 -in $w.infoFrame -pady 1 \
#  -row 2 -column 4 -rowspan 1 -columnspan 2 -sticky news


#############################################
###end graphics.tcl


#############################################
###source startpos.tcl

foreach i $bestpath {set lv_nodeA($i,yPos) 1 }

foreach i $lv_NodeList \
{
  if {$lv_nodeA($i,yPos) == 1} {continue}

  set searchpos true
  set yPosTry 0
  while {$searchpos} \
  {
    incr yPosTry 1
    set searchpos false
    foreach j $lv_nodeA($i,collisions) \
    {
      if {$lv_nodeA($j,yPos)==$yPosTry} {set searchpos true}
    }
  }

  set lv_nodeA($i,yPos) $yPosTry
}

#############################################
###end startpos.tcl

foreach i $lv_NodeList {lv_drawNode $i}

###############
}

GLat method show showlat -text "Display a GLat in a Tk window"
