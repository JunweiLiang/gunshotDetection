# ========================================================================
#  JANUS-SR   Janus Speech Recognition Toolkit
#             ------------------------------------------------------------
#
#  Author  :  Martin Westphal, westphal@ira.uka.de
#  Module  :  scatter.tcl
#  Date    :  Sep 1999
#
#  Remarks :  Scatter Plots
#  
#  $Id: scatter.tcl 2390 2003-08-14 11:20:32Z fuegen $
#
#  $Log$
#  Revision 1.2  2003/08/14 11:19:41  fuegen
#  Merged changes on branch jtk-01-01-15-fms (jaguar -> ibis-013)
#
#  Revision 1.1.2.1  2002/02/27 09:57:10  metze
#  Found it in Martin's stuff
#
#
# ========================================================================
#source ruler.tcl

# ========================================================================
# init plot view
#
#   ---------------------
#   |                   |   'xmin','xmax' are extreme values of the plot.
#   | _|                |   'x0' is the left border, 'dx' the right border.
#   | _|                |   Together with the canvas width 'cwidth' we can
#   | _|                |   calculate 'X0' and 'Xmax' which are the canvas
#   | _|                |   border points.
#   | _|______________  |   Similar for y dimension, note that canvas 0 is
#   |  | | | | | | | |  |   in the upper left corner, but Y0 is on the 
#   | xmin          xmax|   bottom.
#   ---------------------
#   -->|             |<--
#    x0               dx
#   <---- cwidth ------->
# ========================================================================
# ----------------------------------------
#  Given a plot name or "" call this 
#  procedure to get the name.
#  If the plot does not exist it will be 
#  created.
# ----------------------------------------
proc whichScatterPlot {{plot ""}} {
    global spPar
    
    if {![info exists spPar(plotN)]} {
	# this procedure is called the first time
	set spPar(plotN) 0
    }

    #puts "looking for plot '$plot'"
    #for {set i 1} {$i <= $spPar(plotN)} {incr i} {
    #	puts "$i '$spPar(name,$i)'"
    #}

    if {"" == "$plot"} {
	# no name given, take the last active one
	for {set i $spPar(plotN)} {$i > 0} {incr i -1} {
	    if {"" != "$spPar(name,$i)"} {return $spPar(name,$i)}
	}
	# no active plot found, make new
    } else {
	#search for the name
	for {set i 1} {$i <= $spPar(plotN)} {incr i} {
	    if {"$plot" == "$spPar(name,$i)"} {
		return $plot
	    }
	}
	# not found, make new
    }
    return [initScatterPlot $plot]
}
proc initScatterPlot {plot} {
    global spPar

    if {"" != "$plot"} {
	for {set i 1} {$i <= $spPar(plotN)} {incr i} {
	    if {"$plot" == "$spPar(name,$i)"} {error "scatterPlot $plot already exists"}
	}
    }
    for {set i 1} {$i <= $spPar(plotN)} {incr i} {
	if {"" == "$spPar(name,$i)"} break
    }
    if {"" == "$plot"} {set plot .plot$i}
    if {$i > $spPar(plotN)} {set spPar(plotN) $i}
    
    set spPar(name,$i)       $plot         ;# toplevel widget and id of the plot
    set spPar($plot,plotN)   $i            ;# number of the current plot
    set spPar($plot,c)       $plot.c       ;# canvas widget
    set spPar($plot,ctrl)    ${plot}ctrl.f ;# control widget
    set spPar($plot,cwidth)  400           ;# initial canvas size
    set spPar($plot,cheight) 400
    set spPar($plot,bgcolor) white        ;# background color

    set spPar($plot,x0)      60            ;# border points
    set spPar($plot,dx)      40
    set spPar($plot,y0)      40
    set spPar($plot,dy)      60

    set spPar($plot,autoscale) 1           ;# use auto scale
    set spPar($plot,xmin)   -1000.0        ;# axis
    set spPar($plot,xmax)    1000.0
    set spPar($plot,ymin)   -1000.0
    set spPar($plot,ymax)    1000.0

    set spPar($plot,xcur)    0             ;# pointer position
    set spPar($plot,ycur)    0

    set spPar($plot,fes)     ""            ;# featureSet
    set spPar($plot,feL)     ""            ;# feature list
    set spPar($plot,xindex)  0             ;# 
    set spPar($plot,yindex)  1             ;# 
    set spPar($plot,line)    0             ;# 
    set spPar($plot,pcolors) [list black red blue green pink]   ;# colors for scatter plots
    set spPar($plot,useWH)   0             ;# use Winkelhalbierende
    set spPar($plot,useLegend) 0           ;# use legend or not (0 or 1)

    # --- toplevel, control and canvas ---
    toplevel ${plot}ctrl
    initScatterControl $plot [frame $spPar($plot,ctrl)]
    toplevel $plot
    pack $spPar($plot,ctrl) -fill both
    pack [canvas $spPar($plot,c) -width $spPar($plot,cwidth) -height $spPar($plot,cheight) -bg $spPar($plot,bgcolor)] -expand true -fill both -side left
    update

    #set minx [expr $spPar($plot,x0) + $spPar($plot,dx) + 20 + [winfo width $spPar($plot,ctrl)]]
    set minx [expr $spPar($plot,x0) + $spPar($plot,dx) + 20]
    set miny [expr $spPar($plot,y0) + $spPar($plot,dy) + 20]
    wm minsize $plot $minx $miny

    #scatterSetup $plot
    bind $spPar($plot,c) <Configure> "scatter $plot"
    return $plot
}
proc destroyScatterPlot {plot} {
    global spPar
    destroy $plot
    destroy ${plot}ctrl
    set i $spPar($plot,plotN)
    puts "destroying plot $i '$spPar(name,$i)'"
    set spPar(name,$i) ""
}
proc initScatterControl {plot frame} {
    global spPar

    pack [button $frame.close -text close -command "destroyScatterPlot $plot"] -side bottom -fill both

    set i 0
    set bindL ""
    foreach row {
	{pack [label $f.l1 -text "x = "] [label $f.l2 -textvariable spPar($plot,xcur)] -side left -fill x}
	{pack [label $f.l1 -text "y = "] [label $f.l2 -textvariable spPar($plot,ycur)] -side left -fill x}
	{pack [button $f.b1 -text "replot" -command "scatter $plot"] -side left -expand true -fill x}
	{pack [button $f.b1 -text "one time autoscale" -command "scatterAutoScale $plot; scatter $plot"] \
	      [button $f.b2 -text "min = 0.0" -command "set spPar($plot,xmin) 0.0; set spPar($plot,ymin) 0.0; scatter $plot"] \
	      -side left -fill x}
	{pack [button $f.b1 -text "zoom in " -command "scatterZoom $plot 0.5"] \
	      [button $f.b2 -text "zoom out" -command "scatterZoom $plot 2.0"] -side left -fill x}
	{pack [checkbutton $f.b1 -text "autoscale" -variable spPar($plot,autoscale) -command "scatter $plot"] -side left -fill x}
	{pack [label $f.l1 -text "xmin = " -width 8] [entry $f.l2 -textvariable spPar($plot,xmin)] -side left -fill x; lappend bindL $f.l2}
	{pack [label $f.l1 -text "xmax = " -width 8] [entry $f.l2 -textvariable spPar($plot,xmax)] -side left -fill x; lappend bindL $f.l2}
	{pack [label $f.l1 -text "ymin = " -width 8] [entry $f.l2 -textvariable spPar($plot,ymin)] -side left -fill x; lappend bindL $f.l2}
	{pack [label $f.l1 -text "ymax = " -width 8] [entry $f.l2 -textvariable spPar($plot,ymax)] -side left -fill x; lappend bindL $f.l2}
	{pack [checkbutton $f.b1 -text "lines" -variable spPar($plot,line) -command "scatter $plot"] -side left -fill x}
	{pack [checkbutton $f.b1 -text "wh" -variable spPar($plot,useWH) -command "scatterWH $plot"] -side left -fill x}
	{pack [checkbutton $f.b1 -text "legend" -variable spPar($plot,useLegend) -command "scatterLegend $plot"] -side left -fill x}
	{pack [button $f.b1 -text "flash1" -command "scatterFlash $plot 0"] \
	      [button $f.b2 -text "flash2" -command "scatterFlash $plot 1"] \
	      [button $f.b3 -text "flash3" -command "scatterFlash $plot 2"] -side left -fill x
	}
	{pack [button $f.b1 -text "delete1" -command "scatterDelete $plot 0"] \
	      [button $f.b2 -text "delete2" -command "scatterDelete $plot 1"] \
	      [button $f.b3 -text "delete3" -command "scatterDelete $plot 2"] -side left -fill x
	}
	{pack [label $f.l1 -text "x = " -width 8] [entry $f.l2 -textvariable spPar($plot,xindex)] -side left -fill x; lappend bindL $f.l2}
	{pack [label $f.l1 -text "y = " -width 8] [entry $f.l2 -textvariable spPar($plot,yindex)] -side left -fill x; lappend bindL $f.l2}
	{pack [label $f.l1 -text "feL: " -width 8] [entry $f.l2 -textvariable spPar($plot,feL)] -side left -fill x; lappend bindL $f.l2}
	{pack [button $f.b1 -text "postscript" -command "scatterPostscript $plot"] -side left  -expand true -fill x}
	{pack [button $f.b1 -text "Xoff" -command "noXFrame $plot 1"] \
	      [button $f.b2 -text "Xon" -command "noXFrame $plot 0"] \
	      [button $f.b3 -text "xv" -command "exec xv &"] \
	     -side left  -expand true -fill x}
    } {
	pack [set f [frame $frame.f$i]] -side top -fill x
	eval $row
	incr i
    }
    foreach w $bindL {
	bind $w <Return> "scatter $plot"
    }
    return $frame
}
proc scatterSetup {plot} {
    global spPar
    set cwidth  [winfo width $spPar($plot,c)]
    set cheight [winfo height $spPar($plot,c)]

    # given the canvas size and border values calculate most important canvas points
    set spPar($plot,X0)      $spPar($plot,x0)
    set spPar($plot,Xmax)    [expr $cwidth  - $spPar($plot,dx)]
    set spPar($plot,Width)   [expr $spPar($plot,Xmax)  - $spPar($plot,X0)]
    set spPar($plot,Y0)      [expr $cheight - $spPar($plot,y0)]
    set spPar($plot,Ymax)    $spPar($plot,dy)
    set spPar($plot,Height)  [expr $spPar($plot,Y0)  - $spPar($plot,Ymax)]
    set spPar($plot,xfac)    [expr (1.0 * $spPar($plot,xmax) - $spPar($plot,xmin)) / $spPar($plot,Width)]
    set spPar($plot,yfac)    [expr (1.0 * $spPar($plot,ymax) - $spPar($plot,ymin)) / $spPar($plot,Height)]
}
# ========================================================================
# plot procedures
# ========================================================================
# ----------------------------------------
# given the canvas coordinates calculate
# xy-Values of the plot.
# ----------------------------------------
proc scatterXY {plot wx wy} {
    global spPar
    set x [expr $spPar($plot,xfac) * ($wx - $spPar($plot,x0)) + $spPar($plot,xmin)]
    set y [expr $spPar($plot,yfac) * ($spPar($plot,cheight) - $spPar($plot,y0) - $wy) + $spPar($plot,ymin)]
    set spPar($plot,xcur) $x
    set spPar($plot,ycur) $y
    return [list $x $y]
}
# ----------------------------------------
# given the xy-Values of the plot calculate
# canvas coordinates.
# ----------------------------------------
proc scatterWXY {plot x y} {
    global spPar
    set wx [expr int(($x - $spPar($plot,xmin))/$spPar($plot,xfac) + $spPar($plot,X0))]
    set wy [expr int(-($y - $spPar($plot,ymin))/$spPar($plot,yfac) + $spPar($plot,Y0))]
    return [list $wx $wy]
}
proc scatterZoom {plot fac} {
    global spPar
    set spPar($plot,autoscale) 0
    set xmean [expr ($spPar($plot,xmax) + $spPar($plot,xmin)) / 2.0]
    set xd    [expr ($spPar($plot,xmax) - $spPar($plot,xmin)) / 2.0]
    set spPar($plot,xmax) [expr $xmean + $fac * $xd]
    set spPar($plot,xmin) [expr $xmean - $fac * $xd]

    set ymean [expr ($spPar($plot,ymax) + $spPar($plot,ymin)) / 2.0]
    set yd    [expr ($spPar($plot,ymax) - $spPar($plot,ymin)) / 2.0]
    set spPar($plot,ymax) [expr $ymean + $fac * $yd]
    set spPar($plot,ymin) [expr $ymean - $fac * $yd]
    scatter $plot
    return
}
proc scatterLegend {plot} {
    global spPar
    set c $spPar($plot,c)
    set f $c.f
    catch {destroy $f}
    if {$spPar($plot,useLegend) == 0} return

    set x [expr 10 + $spPar($plot,x0)]
    set y 10
    set leg [$c create window $x $y -window [frame $f -relief ridge -borderwidth 5] -anchor nw -tags legend]
    scatterSetFS $plot fes feL
    set i 0
    foreach fe $feL {
	set color [lindex $spPar($plot,pcolors) $i]
	pack [frame $f.f$i] -side top -fill x -padx 5 -pady 5
	pack [label $f.f$i.l1 -text " " -bg $color] [label $f.f$i.l2 -text $fe] -side left -anchor w 
	incr i
    }
    bind $f <Button-1> {set legX %x; set legY %y}
    bind $f <B1-Motion> "$c move legend \[expr %x - \$legX\] \[expr %y - \$legY\]"
}

# ----------------------------------------
# delete scatters given in the number list
# ----------------------------------------
proc scatterClear {plot pL} {
    global spPar
    foreach p $pL {
	$spPar($plot,c) delete p$p
    }
}
# ----------------------------------------
# color scatters given in the number list
# if no color given pick from color list
# ----------------------------------------
proc scatterColor {plot i {color ""}} {
    global spPar
    if {"" == "$color"} {
	set ic    [expr $i % [llength $spPar($plot,pcolors)]]
	set color [lindex $spPar($plot,pcolors) $ic]
    }
    $spPar($plot,c) itemconfigure p$i -fill $color ;# -outline $color
    catch {$spPar($plot,c) itemconfigure p$i -outline $color};# if plotted as lines no outline
    return $color
}
# ----------------------------------------
# make one of the scatters flashing
# ----------------------------------------
proc scatterFlash {plot i} {
    global spPar
    set color [lindex $spPar($plot,pcolors) $i]
    set bg    $spPar($plot,bgcolor)
    for {set j 0} {$j < 10} {incr j} {
	after 100
	$spPar($plot,c) itemconfigure p$i -fill $bg ;# -outline $bg
	catch {$spPar($plot,c) itemconfigure p$i -outline $bg}
	update
	after 100
	$spPar($plot,c) itemconfigure p$i -fill $color ;# -outline $color
	catch {$spPar($plot,c) itemconfigure p$i -outline $color}
	update
    }
    return $color
}
proc scatterDelete {plot i} {
    global spPar
    catch {$spPar($plot,c) delete p$i}
}
# ----------------------------------------
# Winkelhalbierende
# ----------------------------------------
proc scatterWH {plot} {
    global spPar
    catch {$spPar($plot,c) delete wh}
    if {$spPar($plot,useWH)} {
	set maxmin [expr $spPar($plot,xmin) > $spPar($plot,ymin) ? $spPar($plot,xmin) : $spPar($plot,ymin)]
	set minmax [expr $spPar($plot,xmax) < $spPar($plot,ymax) ? $spPar($plot,xmax) : $spPar($plot,ymax)]
	if {$maxmin >= $minmax} return
	set p1 [scatterWXY $plot $maxmin $maxmin]
	set p2 [scatterWXY $plot $minmax $minmax]
	eval $spPar($plot,c) create line $p1 $p2 -tag wh
    }
}
# ----------------------------------------
# set featureSet and feature list
# ----------------------------------------
proc scatterSetFS {plot fesP feLP} {
    global spPar
    upvar $fesP fes
    upvar $feLP feL
    if {[info exists fes] && "" != "$fes"} {set spPar($plot,fes) $fes}
    set fes $spPar($plot,fes)
    if {[info exists feL] && "" != "$feL"} {set spPar($plot,feL) $feL}
    set feL $spPar($plot,feL)
}

# ----------------------------------------
# find min and max of all plotted features
# and set same values for x and y axis
# ----------------------------------------
proc scatterAutoScale {plot {fes ""} {feL ""}} {
    global spPar
    
    scatterSetFS $plot fes feL
    foreach fe $feL {
	set minmax [$fes : $fe . data minmax]
	if {![info exists Min]} {
	    foreach {Min Max} $minmax {}
	} else {
	    foreach {min max} $minmax {}
	    set Min [expr $min < $Min ? $min : $Min]
	    set Max [expr $max > $Max ? $max : $Max]
	}
    }
    if {![info exists Min]} {
	foreach {Min Max} {0.0 1.0} {}
    }
    if {$Min == $Max} {
	set d   [expr $Min == 0.0 ? 1.0 : 0.1 * abs($Min)]
	set Max [expr $Max + $d]
	set Min [expr $Max - $d]
    }
    foreach "spPar($plot,xmin) spPar($plot,xmax)" "$Min $Max" {}
    foreach "spPar($plot,ymin) spPar($plot,ymax)" "$Min $Max" {}
}
# ----------------------------------------
# plot all the given features 
# or get list from older call
# ----------------------------------------
proc scatter {{plot ""} {fes ""} {feL ""} {auto ""}} {
    global spPar

    set plot [whichScatterPlot $plot]
    if {"" != "$auto"} {set spPar($plot,autoscale) $auto}
    if {$spPar($plot,autoscale)} {scatterAutoScale $plot $fes $feL}

    set c    $spPar($plot,c)
    scatterSetFS $plot fes feL
    scatterSetup $plot

    $c delete all

    # --- plot axis, wh ---
    global X0 Y0
    foreach {X0 Y0} "$spPar($plot,X0) $spPar($plot,Y0)" {}
    DrawHRuler $c $spPar($plot,Width)  $spPar($plot,xmin) $spPar($plot,xmax) "x$spPar($plot,xindex)"
    DrawVRuler $c $spPar($plot,Height) $spPar($plot,ymin) $spPar($plot,ymax) "x$spPar($plot,yindex)"
    scatterWH $plot

    # --- plot points ---
    set i 0
    foreach fs $feL {
	$fes : $fs . data scatterPlot $c \
	    -width $spPar($plot,Width) -height $spPar($plot,Height) \
	    -x $spPar($plot,X0) -y $spPar($plot,Ymax) \
	    -xindex $spPar($plot,xindex) -yindex $spPar($plot,yindex) \
	    -from   0 -to    -1 \
	    -xmin $spPar($plot,xmin) -xmax $spPar($plot,xmax) \
	    -ymin $spPar($plot,ymin) -ymax $spPar($plot,ymax) \
	    -tag p$i -line $spPar($plot,line) -p 1
	
	set color [scatterColor $plot $i]
	incr i
	puts "  $i: plotted $fs ($spPar($plot,xindex),$spPar($plot,yindex)) $color $spPar($plot,line)"

	# --- histogram ---
	if 1 {
	    set values [$fes : $fs . data puts -jb $spPar($plot,xindex) -je $spPar($plot,xindex)]
	    set histo1 [histogram $values $spPar($plot,xmin) $spPar($plot,xmax) 40]
	    set values [$fes : $fs . data puts -jb $spPar($plot,yindex) -je $spPar($plot,yindex)]
	    set histo2 [histogram $values $spPar($plot,xmin) $spPar($plot,xmax) 40]

	    plotWindow .scatterHistogram$i [list $histo1 "histo -outline black -fill $color" $histo2 "histo -outline blue -width 3"]\
		-ylabel count -xmin  $spPar($plot,xmin) -xmax  $spPar($plot,xmax) -ymin 0.0 -width [winfo width $spPar($plot,c)]
	    #plotFunction $c $histo1 $spPar($plot,Width) 100 0.0 100.0 "histo -outline black -fill $color" 1 p$i
	    #plotFunction $c $histo2 $spPar($plot,Width) 100 0.0 100.0 "histo -outline blue -width 3" 1 pp$i
	}
    }



    # --- legend ---
    scatterLegend $plot

    # --- bindings ---
    #$c bind all <Motion> "scatterXY $plot %x %y"
    bind $c <Motion> "scatterXY $plot %x %y"
}
# ========================================================================
#  Here is an example of a scatter plot
# ========================================================================
proc scatterTest {} {
    #wm withdraw .
    FeatureSet fs
    fs noise N1 1000  -dim 2 -type uniform
    fs normalize N1 N1 -min -100 -max 2000
    
    fs noise N2 1000  -dim 2
    fs normalize N2 N2 -min -1000 -max 1000
    
    scatter "" fs {N1 N2}
}



# ========================================================================
#  scatterBA (before after)
#  Create a scatter plot of a given coefficient. 'fe1' is the reference
#  feature (for example without noise) and 'fe2L' holds a list of modified
#  features (noisy, enhanced).
#  After the first call it can be refreshed with: scatterBA
# ========================================================================
proc scatterBA {{plot ""} {fes ""} {fe1 ""} {fe2L ""} {coefX 0} {auto ""}} {
    global BAPar
    if {"" == "$fes"} {
	if {![info exists BAPar(fes)]} return
	foreach {fes fe1 fe2L coefX auto} [list $BAPar(fes) $BAPar(fe1) $BAPar(fe2L) $BAPar(coefX) $BAPar(auto)] {}
    }

    $fes split _T1 $fe1 $coefX $coefX 
    set feL ""
    foreach fe2 $fe2L {
	set mse [featureMSE $fes $fe1 $fe2]
	#puts stderr "  MSE($fe2): $mse"
	puts stderr "  MSE($fe2)\[$coefX\] ->  [lindex $mse $coefX]"

	$fes split _T2 $fe2 $coefX $coefX
	$fes merge scatter$fe2 _T1 _T2
	#puts stderr "  MSE($fe2): [featureMSE $fes _T1 _T2]"
	lappend feL scatter$fe2
    }
    $fes delete _T1 _T2

    global spPar
    set plot [whichScatterPlot $plot]
    set spPar($plot,xindex) 0
    set spPar($plot,yindex) 1
    scatter $plot $fes $feL $auto
    foreach {BAPar(fes) BAPar(fe1) BAPar(fe2L) BAPar(coefX) BAPar(auto)} [list $fes $fe1 $fe2L $coefX $auto] {}
}
proc featureMSE {fes fe1 fe2} {
    $fes add _MSE 1 $fe1 -1 $fe2
    $fes pow _MSE _MSE 1 2
    FMatrix mean
    $fes mean mean _MSE
    set m [lindex [mean puts -format {% 6f}] 0]
    mean destroy
    $fes delete _MSE
    return $m
} 

proc scatterPostscript {plot {file ""}} {
    global spPar
    set c $spPar($plot,c)
    if {"" == "$file"} {
	set file [FSBox "Select ps file to save:"]
    }
    if {"" != "$file"} {
	$c postscript -file $file
    }
}



# --------------------------------------------------------
#  Just in case we want to grab a canvas with xv without
#  the window frame:
# --------------------------------------------------------
proc noXFrame {toplevel {noX 1}} {
    wm overrideredirect $toplevel $noX
    wm withdraw $toplevel
    update
    raise $toplevel
    wm deiconify $toplevel
}

# ---------
# for codebooks: select all codebooks of type $CB
#set cbL [codebookSet$SID]
#set CB U; foreach cb $cbL {if {[regexp "^${CB}\\(" "$cb"] == 0} continue; ...}