#
#  $Id: showDist.tcl 460 2000-08-23 11:00:01Z jmcd $
#
#  $Log$
#  Revision 1.1  2000/08/23 11:00:01  jmcd
#  Added janus files from gui-tcl.
#
# Revision 1.4  1996/02/05  13:28:23  maier
# show also Codebooks
#
# Revision 1.3  1996/01/31  17:05:10  maier
# showDSS
#
# Revision 1.2  1996/01/31  15:58:15  maier
# *** empty log message ***
#
# Revision 1.1  1995/09/21  21:40:37  torsten
# Initial revision
#


# TOR - procedure for displaying a graphical representation of
# distributions.  Takes distribution object and list of distribution
# names as input.
proc showDistributions args {

  if {[llength $args] < 2} {
    puts "usage: showDistributions <dsSet> dist1 dist2..."
    return
  }

  set max 1.0
  set heightDS 150
  set heightCB 150
  set wx       10

  set dss [lindex $args 0]
  set names [lrange $args 1 end]
  set cbs [$dss.codebookSet configure -name]

  foreach name $names {
    set vector [lindex [$dss:$name] 0]
    if {[lindex $vector 0] == "ERROR"} {
     puts "$name not found"
     next
     }

    set vN [llength $vector]
    set width [expr $wx * $vN + 1]
    catch {destroy .sd$dss$name}
    set t [toplevel .sd$dss$name]
    canvas $t.dsc -height $heightDS -width $width
    canvas $t.cbc -height $heightCB -width $width
    button $t.quit -text "close" -command "destroy $t"
    pack $t.dsc $t.cbc $t.quit -fill x -side top -pady 3 -padx 3

    # --- display distribution ---
    set x0 1
    set y0 $heightDS
    for {set i 0} {$i < $vN} {incr i} {
      set num [lindex $vector $i]
      set x1 [expr $x0 + $wx]
      set y1 [expr $heightDS - ($num * $heightDS)/$max]
      if {($i % 5) == 0} {
        $t.dsc create rectangle $x0 $y0 $x1 $y1 -fill green
      } else {
        $t.dsc create rectangle $x0 $y0 $x1 $y1 -fill red
      }
      set x0 $x1
    }
    
    # --- display codebook ---
    set cbI [$dss:$name configure -cbX]
    $cbs.item($cbI).mat display $t.cbc -dx $wx -height $heightCB -grey 1 -space 0
  }
}

proc showDSS {dss {w ""}} {
    if {$w == ""} {  
      set w ".sdss$dss"
      catch {destroy $w}
      Toplevel $w
      wm title $w "Show distribSet $dss"
      wm iconname $w "distribSet"
      wm minsize $w 1 1
    } else {
      catch {destroy $w}
      Frame $w
      pack $w
    }
    frame $w.frame
    frame $w.b
    pack $w.frame $w.b  -side top

    button $w.b.ok -text "close"  -command "destroy $w"
    scrollbar $w.frame.scroll -relief sunken -command "$w.frame.list yview"
    listbox $w.frame.list -yscroll "$w.frame.scroll set" -relief sunken \
	    -setgrid 1
    pack $w.b.ok $w.frame.list $w.frame.scroll -side left -fill y
    set list [$dss puts]
    eval "$w.frame.list insert 0 $list"
    bind $w.frame.list <Double-1> \
         "showDistributions $dss \[selection get\]"
}






