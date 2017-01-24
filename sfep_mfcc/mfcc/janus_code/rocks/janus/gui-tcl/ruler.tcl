# ========================================================================
#  JANUS-SR   Janus Speech Recognition Toolkit
#             ------------------------------------------------------------
#
#  Author  :  Martin Westphal, westphal@ira.uka.de
#  Module  :  ruler.tcl
#  Date    :  Oct. 1999
#
#  Remarks :  
#  
#  $Id: ruler.tcl 2390 2003-08-14 11:20:32Z fuegen $
#
#  $Log$
#  Revision 1.2  2003/08/14 11:19:41  fuegen
#  Merged changes on branch jtk-01-01-15-fms (jaguar -> ibis-013)
#
#  Revision 1.1.2.1  2002/02/27 09:56:22  metze
#  Found it in Martin's stuff
#
#
# ========================================================================

### MM 95 ##############################################################
#
#              vertical and horizontal rulers 
#
#  with range 'start' to 'end' and label 'label'.
#  'ruler' is the canvas name and 
#  'width' the width in pixels of the horizontal ruler
#  'height' the height in pixel of the vertical ruler.
#  80 and 40 are good values.
#  'X0' and 'Y0' are global and hold the (0,0) point in pixels.
#  It'll be the left upper most point of the vruler and the right
#  lower most point of the hruler.
# 
#  If the procedures are called more than once the old rulers will
#  be destroyed automatically .
#########################################################################

set rulerfont "-Adobe-times-medium-r-normal--*-120*"

proc DrawHRuler {ruler width start end label {tag hrul}} {
    global tk_library rulerfont
    global X0 Y0

    set dxmin         30.   ;# space for numbers
    set dxmin5        20.   ;# demi
    set dxmin10       10.   ;# deci 
    set height_scale  10
    set height_scale5  8
    set height_scale10 5
    set height_number [expr $height_scale + 5]

    if {$width <= 0} { error "DrawHRuler: width of ruler must be > 0 (but is $width)" }
    set range [expr $end - $start]
    if {$range == 0.0} {set range 1.0}
    set fac   [expr $width / $range]
    set delta 1.

    while {[expr ($fac * $delta) > $dxmin ]} {set delta [expr $delta / 10.]}
    while {[expr ($fac * $delta) < $dxmin ]} {set delta [expr $delta * 10.]}
    $ruler delete $tag
    $ruler create line $X0 $Y0  [expr $X0 + $width-1] $Y0 -width 1 -tag $tag
    $ruler create line [expr $X0 + $width-1] $Y0 [expr $X0 + $width + 19] $Y0 -width 1 -arrow last -tag $tag
    $ruler create text [expr $X0 + $width + 24] $Y0 -text $label -anchor w -tag $tag

    set i [expr ceil($start/$delta)]
    while {[expr $i*$delta]  <= $end} {
         set x [expr round($width*($i*$delta-$start)/$range)]
         $ruler create line [expr $X0 + $x] $Y0 [expr $X0 + $x] [expr $Y0 + $height_scale]  -width 1 -tag $tag
         $ruler create text [expr $X0 + $x] [expr $Y0 + $height_number] -text [expr $i*$delta] -anchor n \
	     -tag $tag ;# -font $rulerfont
         set i [expr $i+1]
    }
    set delta [expr $delta / 2.0]
    if {[expr ($fac * $delta) > $dxmin5 ]} {
	set i [expr ceil($start/$delta)]
	while {[expr $i*$delta]  <= $end} {
	    set x [expr round($width*($i*$delta-$start)/$range)]
	    $ruler create line [expr $X0 + $x] $Y0 [expr $X0 + $x] [expr $Y0 + $height_scale5]  -width 1 -tag $tag
	    if {[expr ($fac * $delta) > $dxmin ]} {
		$ruler create text [expr $X0 + $x] [expr $Y0 + $height_number] -text [expr $i*$delta] -anchor n \
		    -tag $tag ;# -font $rulerfont
	    }
	    set i [expr $i+1]
	}
    }
    set delta [expr $delta / 5.0]
    if {[expr ($fac * $delta) > $dxmin10 ]} {
	set i [expr ceil($start/$delta)]
	while {[expr $i*$delta]  <= $end} {
	    set x [expr round($width*($i*$delta-$start)/$range)]
	    $ruler create line [expr $X0 + $x] $Y0 [expr $X0 + $x] [expr $Y0 + $height_scale10]  -width 1 -tag $tag
	    set i [expr $i+1]
	}
    }
}

proc DrawVRuler {ruler height start end label {tag vrul}} {
    global tk_library rulerfont
    global X0 Y0

    set dxmin         30.   ;# space for numbers
    set dxmin5        20.   ;# demi
    set dxmin10       10.   ;# deci 
    set height_scale  10
    set height_scale5  7
    set height_scale10 4
    set height_number [expr $height_scale + 5]

    if {$height <= 0} { error "DrawVRuler: height of ruler must be > 0 (but is $height)" }
    set range [expr $end - $start]
    if {$range == 0.0} {set range 1.0}
    set fac   [expr $height / $range]
    set delta 1.

    while {[expr ($fac * $delta) > $dxmin ]} {set delta [expr $delta / 10.]}
    while {[expr ($fac * $delta) < $dxmin ]} {set delta [expr $delta * 10.]}
    $ruler delete $tag
    $ruler create line $X0 $Y0  $X0 [expr $Y0 - $height+1] -width 1 -tag $tag
    $ruler create line $X0 [expr $Y0 - $height+1] $X0 [expr $Y0 - $height - 19] -width 1 -arrow last  -tag $tag
    $ruler create text $X0 [expr $Y0 - $height - 24] -text $label -anchor se -tag $tag

    set i [expr ceil($start/$delta)]
    while {[expr $i*$delta]  <= $end} {
         set y [expr round($height*($i*$delta-$start)/$range)]
         $ruler create line $X0 [expr $Y0 - $y] [expr $X0 - $height_scale] [expr $Y0 - $y] -width 1 -tag $tag
         $ruler create text  [expr $X0 - $height_number] [expr $Y0 - $y]  -text [expr $i*$delta] -anchor e \
	     -tag $tag ;# -font $rulerfont
         set i [expr $i+1]
    }
    set delta [expr $delta / 2.0]
    if {[expr ($fac * $delta) > $dxmin5 ]} {
	set i [expr ceil($start/$delta)]
	while {[expr $i*$delta]  <= $end} {
	    set y [expr round($height*($i*$delta-$start)/$range)]
	    $ruler create line $X0 [expr $Y0 - $y] [expr $X0 - $height_scale5] [expr $Y0 - $y] -width 1 -tag $tag
	    if {[expr ($fac * $delta) > $dxmin ]} {
		$ruler create text  [expr $X0 - $height_number] [expr $Y0 - $y]  -text [expr $i*$delta] -anchor e \
		    -tag $tag ;# -font $rulerfont
	    }
	    set i [expr $i+1]
	}
    }
    set delta [expr $delta / 5.0]
    if {[expr ($fac * $delta) > $dxmin10 ]} {
	set i [expr ceil($start/$delta)]
	while {[expr $i*$delta]  <= $end} {
	    set y [expr round($height*($i*$delta-$start)/$range)]
	    $ruler create line $X0 [expr $Y0 - $y] [expr $X0 - $height_scale10] [expr $Y0 - $y] -width 1 -tag $tag
	    set i [expr $i+1]
	}
    }
}
# --------------------------------------------------------
#  plotFunction
#     canvas      name of canvas widget
#     values      Tcl list with y-values to plot
#     X           width of plot in pixels
#     Y           height of plot in pixels
#     ymin        minimum y value
#     ymax        maximum y value
#     type        there are different styles for the plot:
#                   lines, histo and points
#                 you can also give flags here
#     h           plot horizontal or vertical
#     tag         canvas tag for the plotted function
# --------------------------------------------------------
proc plotFunction {canvas values X Y ymin ymax {type lines} {h 1} {tag f}} {
    global X0 Y0
    
    if {$ymax == $ymin} {error "plotFunction: ymax ($ymin) must be > ymin ($ymin)"}
    set N [llength $values]
    set cmd  [lindex $type 0]
    if {"$cmd" == "histo"} {
	lappend values 0
    } else {
	incr N -1
    }
    set my [expr 1.0 * $Y / ($ymax - $ymin)]
    set mx [expr 1.0 * $X / $N]
    set i 0
    set new ""
    foreach value $values {
	set x [expr int(floor($mx * $i))]
	set y [expr int(floor($my * ($value - $ymin)))]
	lappend new [list $x $y]
	incr i
    }
    set y01 [expr int(floor(-1.0 * $my * $ymin))]

    set args [lrange $type 1 end]
    switch -- $cmd {
	lines {
	    foreach xy $new {
		foreach {x1 y1} $xy {
		    set x  [expr $X0 + ($h ?  $x1 : -1 * $y1)]
		    set y  [expr $Y0 - ($h ?  $y1 : $x1)]
		    if {[info exists xold]} {
			eval "$canvas create line $xold $yold $x $y $args -tag $tag"
		    }
		    set xold $x
		    set yold $y
		}
	    }
	}
	histo {
	    foreach xy $new {
		foreach {x1 y1} $xy {
		    set x  [expr $X0 + ($h ?  $x1 : -1 * $y1)]
		    set y  [expr $Y0 - ($h ?  $y1 : $x1)]
		    if {[info exists xold]} {
			#eval "$canvas create line $xold $yold $x $yold $x $y $args -tag $tag"
			eval "$canvas create rect $xold $yold $x $y0 $args -tag $tag"
		    } else {
			set x0  [expr $X0 + ($h ?  0 : -1 * $y01)]
			set y0  [expr $Y0 - ($h ?  $y01 : 0)]
			#eval "$canvas create line $x0 $y0 $x0 $y $args -tag $tag"
		    }
		    set xold $x
		    set yold $y
		}
	    }
	}
	points {
	    foreach xy $new {
		foreach {x1 y1} $xy {
		    set x  [expr $X0 + ($h ?  $x1 : -1 * $y1)]
		    set y  [expr $Y0 - ($h ?  $y1 : $x1)]
		    #$canvas create oval [expr $x-1] [expr $y-1] [expr $x+1] [expr $y+1] -tag $tag
		    eval "$canvas create oval $x $y $x $y $args -tag $tag"
		}
	    }
	}
     }
    #set y0  [expr Y0 - ($h ?  $y1 : $x1)]

}


proc testPlot {{canvas .c}} {
    global X0 Y0
    set X 300
    set dX 50
    set Y 200
    set dY 50

    catch {destroy $canvas}
    pack [canvas $canvas -width [expr $X + 2*$dX] -height [expr $Y + 2*$dY]]
    set X0 [expr $dX]
    set Y0 [expr $dY + $Y]
    DrawHRuler $canvas $X 0.0    1.0 x
    DrawVRuler $canvas $Y 0.0  100.0 count

    set values {50 25 10 5 0 33 66 100 90 80}
    plotFunction $canvas $values $X $Y 0.0 100.0 {histo -width 4 -outline blue -fill yellow} 1 f1

    set dx [expr 0.5 * $X / [llength $values]]
    set X  [expr $X - int(2 * $dx)]
    set X0 [expr $X0 + int($dx)]
    plotFunction $canvas $values $X $Y 0.0 100.0 {lines -fill red -width 3} 1 f2
    #$canvas itemconfigure f2 -fill red -width 3

    plotFunction $canvas $values $X $Y 0.0 100.0 {points -width 5} 1 f3
}

# funcL is {valL1 args1 valL2 args2 ...}
proc plotWindow {args} { 
    global pwPar
    set xlabel x
    set ylabel y
    set xmin   0.0
    set xmax   auto
    set min    auto
    set max    auto
    set width  300
    set height 200
    if [catch {itfParseArgv plotWindow $args [list \
        [ list <toplevel> string {} t      {} "tk toplevel widget"  ] \
        [ list <funcL>    string {} funcL  {} "{valL1 args1 valL2 args2 ...}"  ] \
        [ list -xlabel    string {} xlabel {} ""  ] \
        [ list -ylabel    string {} ylabel {} ""  ] \
        [ list -xmin      string {} xmin   {} ""  ] \
        [ list -xmax      string {} xmax   {} ""  ] \
        [ list -ymin      string {} min    {} ""  ] \
        [ list -ymax      string {} max    {} ""  ] \
        [ list -width     int    {} width  {} ""  ] \
        [ list -height    int    {} height {} ""  ] \
    ] } msg] { error "$msg" }
    set callagain "plotWindow $args"
    if {![info exists pwPar($t,max)]} {
	set pwPar($t,max) "" 
	set pwPar($t,min) "" 
    }
    if {"auto" == "$max"} {
	if {"" != "$pwPar($t,max)"} {
	    set max $pwPar($t,max)
	} else {
	    set max -9e99
	    foreach {valL args} $funcL {
		foreach x $valL {if {$x > $max} {set max $x}}
	    }
	}
    }
    if {"auto" == "$min"} {
	if {"" != "$pwPar($t,min)"} {
	    set min $pwPar($t,min)
	} else {
	    set min 9e99
	    foreach {valL args} $funcL {
		foreach x $valL {if {$x < $min} {set min $x}}
	    }
	}
    }
    if {$min == $max} {set max [expr $max + 1.0]}

    if {![winfo exists $t]} {
	toplevel $t
	pack [canvas $t.c -width $width -height $height -bg white] -fill both -expand true

	set f ${t}ctrl
	toplevel $f
	pack [button $f.b0 -text "close" -command "destroy $t; destroy $f"] -fill x -side bottom
	pack [button $f.b1 -text "Xoff" -command "noXFrame $t 1"] \
	    [button $f.b2 -text "Xon" -command "noXFrame $t 0"] \
	    [button $f.b3 -text "xv" -command "exec xv &"] \
	    -side left -fill x
	update
    } else {
	$t.c configure -width $width -height $height
    }
    bind $t.c <Configure> "$callagain"
    set width  [winfo width $t.c]
    set height [winfo height $t.c]
    global X0 Y0
    set X0 60; set X1 40
    set Y0 40; set Y1 60
    set X  [expr $width - $X0 - $X1]
    set Y  [expr $height - $Y0 - $Y1]

    set Y0 [expr $height - $Y0]
    $t.c delete all

    if {"auto" == "$xmax"} { 
	set N 0
	foreach {valL args} $funcL {
	    set N [llength $valL]
	}
	set xmax [expr $N - 1]
    }
    DrawHRuler $t.c $X $xmin $xmax  $xlabel
    DrawVRuler $t.c $Y $min  $max   $ylabel    vrul
    foreach {valL args} $funcL {
	plotFunction $t.c $valL $X $Y $min $max $args 1 "f"
    }
    set pwPar($t,c)      $t.c
    set pwPar($t,curmin) $min
    set pwPar($t,curmax) $max
    $t.c bind vrul <Button-1> "plotWindowSetY $t"

}
proc plotWindowSetY {t} {
    global pwPar
    if {"" == "$pwPar($t,max)"} {
	$pwPar($t,c) itemconfigure vrul -fill red
	set pwPar($t,max) $pwPar($t,curmax)
	set pwPar($t,min) $pwPar($t,curmin)
	puts "MSE plot with fixed min max $pwPar($t,min) $pwPar($t,max)"
    } else {
	$pwPar($t,c) itemconfigure vrul -fill green
	set pwPar($t,max) ""
	set pwPar($t,min) ""
	puts "MSE plot with auto scale"
    }
    after 200 $pwPar($t,c) itemconfigure vrul -fill black
}

