# ========================================================================
#  JANUS-SR   Janus Speech Recognition Toolkit
#             ------------------------------------------------------------
#
#  Author  :  Martin Westphal, westphal@ira.uka.de
#  Module  :  histogram.tcl
#  Date    :  Oct. 1999
#
#  Remarks :  
#  
#  RCS:
#  $Id: histogram.tcl 2894 2009-07-27 15:55:07Z metze $
#  $Log$
#  Revision 1.3  2004/08/16 16:21:06  metze
#  P014, histogram
#
#  Revision 1.2  2003/08/14 11:19:40  fuegen
#  Merged changes on branch jtk-01-01-15-fms (jaguar -> ibis-013)
#
#  Revision 1.1.2.1  2002/02/27 09:54:08  metze
#  Found it in Martin's stuff
#
#
#
# ========================================================================
# ------------------------------------------------------------------------
# histogram
#  given a list of numbers return a histogram
# ------------------------------------------------------------------------
proc histogram {list min max bins} {
    set delta [expr 1.0 *($max - $min)/$bins]
    for {set i 0} {$i < $bins} {incr i} {
	set count($i) 0
    }
    if {$delta == 0.0} {
	set count(0) [llength $list]
    } else {
	foreach value $list {
	    set index [expr ($value - $min)/$delta]
	    set index [expr $index < 0 ? 0 : $index >= $bins ? $bins - 1 : int(floor($index))]
	    incr count($index)
	}
    }
    set retL ""
    for {set i 0} {$i < $bins} {incr i} {
	lappend retL $count($i)
    }
    return $retL
}

proc plotHistogram {canvas values min max bins {maxcount 0} {scaling 1.0}} {
    global X0 Y0
    set X 300
    set dX 50
    set Y 100
    set dY 50
    set f   1

    foreach v [histogram $values $min $max $bins] {
	lappend histo [expr $scaling*round($v/$f)]
    }
    
    if {$maxcount == 0} {
	foreach val $histo { if {$val > $maxcount} {set maxcount $val } }
    }
    set Y $maxcount

    catch {destroy $canvas}
    pack [canvas $canvas -width [expr $X + 2*$dX] -height [expr $Y + 3*$dY]]
    set X0 [expr $dX]
    set Y0 [expr $dY + $Y]

    printDo DrawHRuler $canvas $X $min $max x
    printDo DrawVRuler $canvas $Y 0  $maxcount "count/$f"

    plotFunction $canvas $histo $X $Y 0 $maxcount histo 1 f1
    $canvas itemconfigure f1 -fill blue
}

proc ArrayHistogram { a } {
    upvar $a arr

    set canvas .c
    set coef 10

    set min 0
    set max 41
    set bins 40

    set min 0.8
    set max 1.2

    foreach v [array names arr] { lappend vals $arr($v) }

    plotHistogram $canvas $vals $min $max $bins
}

if 0 {
    set canvas .t1.c
    set coef 0
    set min  0
    set max  10
    set bins 40
    set values [$fes:lMEL0.data puts -jb $coef -je $coef]
    plotHistogram $canvas $values $min $max $bins    
}
