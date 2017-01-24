# ========================================================================
#    JANUS      Speech- to Speech Translation
#               ------------------------------------------------------------
#               Object: Tk GUI for the JRTk-Net
#               ------------------------------------------------------------
#
#    Author  :  Juergen Fritsch
#    Module  :  JRTk-Net.tcl
#    Date    :  $Id: JRTk-Net.tcl 618 2000-09-14 11:37:03Z janus $
#    Remarks :  
#
#   ========================================================================
#
#    This software was developed by
#
#    the  University of Karlsruhe       and  Carnegie Mellon University
#         Dept. of Informatics               Dept. of Computer Science
#         ILKD, Lehrstuhl Prof.Waibel        Alex Waibel's NN & Speech Group
#         Am Fasanengarten 5                 5000 Forbes Ave
#         D-76131 Karlsruhe                  Pittsburgh, PA 15213
#         - West Germany -                   - USA -
#
#    This software is part of the JANUS Speech- to Speech Translation Project
#
#    USAGE PERMITTED ONLY FOR MEMBERS OF THE JANUS PROJECT
#    AT CARNEGIE MELLON UNIVERSITY OR AT UNIVERSITAET KARLSRUHE
#    AND FOR THIRD PARTIES ONLY UNDER SEPARATE WRITTEN PERMISSION
#    BY THE JANUS PROJECT
#
#    It may be copied  only  to members of the JANUS project
#    in accordance with the explicit permission to do so
#    and  with the  inclusion  of  the  copyright  notices.
#
#    This software  or  any  other duplicates thereof may
#    not be copied or otherwise made available to any other person.
#
#    No title to and ownership of the software is hereby transferred.
#
#   ========================================================================
#
#    $Log$
#    Revision 1.2  2000/09/14 11:36:51  janus
#    merging branch jtk-00-09-11-fuegen-2
#
# Revision 1.1.4.1  2000/09/11  16:04:14  janus
# Merging outdated branch jtk-00-09-08-fuegen.
# 
#    Revision 1.1.2.1  2000/09/11 10:48:01  janus
#    added JRTKNet (by Juergen Fritsch)
#
#    Revision 1.1.1.1  2000/01/13 08:56:22  fuegen
#    imported gui-tcl scripts
#
#    Revision 2.0  1999/02/05 15:23:20  fuegen
#    *** empty log message ***
#
#    Revision 1.1  1997/07/28 15:39:18  westphal
#    Initial revision
#
# * Revision 1.1  1997/01/27  08:59:05  fritsch
# * Initial revision
# *
#
#   ========================================================================
#
#



set tcl_precision 12



# -----------------------------------------------------------------------
#  checking for correct JRTk version
# -----------------------------------------------------------------------

if {![string length [info commands FFNet]]} {
  puts stderr "Wrong JRTk version - no type FFNet available"
  exit
}

catch {FMatrix just_a_dummy_matrix}
just_a_dummy_matrix := {{1 1} {1 1}}
if [catch {just_a_dummy_matrix get 0 1}] {
  puts stderr "Wrong JRTk version - no method get for type FMatrix available"
  exit
}



# ----------------------------------------------------------------------- 
#  quit_menu                                                              
# ----------------------------------------------------------------------- 
proc quit_menu {} {

  catch { destroy .newobj }
  catch { destroy .destobj }
  catch { destroy .fload }
  catch { destroy .fsave }
  catch { destroy .file }
  catch { destroy .layer }
  catch { destroy .link }
  catch { destroy .info }
  catch { destroy .edit }
  catch { destroy .trw }
  catch { destroy .train }
  catch { destroy .fm0 }
  catch { destroy .fm1 }
  catch { destroy .fm2 }
  catch { destroy .fm3 }
  catch { destroy .fm4 }
  catch { destroy .test }
  catch { destroy .analyse }
  catch { destroy .main }

}



# ----------------------------------------------------------------------- 
#  file_files_update                                         
# ----------------------------------------------------------------------- 
proc  file_files_update { w } {

  global    current_type cwd

  while {[$w.list size] > 0} {
    $w.list  delete  0
  }

  if {$cwd != "/"} { 
    set filelist ".." 
  } else { 
    set filelist "" 
  }
  set dirs [glob -nocomplain */]
  foreach dir $dirs {
    lappend filelist $dir
  }

  if {$current_type == "FFNet"} {
    set files [glob -nocomplain "*.net"]
  } else {
    set files [glob -nocomplain "*.{mat,bmat}" ]
  }

  foreach file $files {
    lappend filelist $file
  }

  set slist [lsort $filelist]
  foreach i $slist {
    $w.list insert end $i
  }

}



# ----------------------------------------------------------------------- 
#  file_files                                                              
# ----------------------------------------------------------------------- 
proc  file_files { w } {

  global current_file current_type cwd

  set current_file ""

  frame $w.header
  pack $w.header -side top
  label $w.header.h1 -text "File:" -fg red
  label $w.header.h2 -textvariable current_file -fg blue
  pack $w.header.h1 $w.header.h2 -side left
  listbox $w.list -relief raised -borderwidth 2 \
          -yscrollcommand "$w.scroll set"
  pack $w.list -side left
  scrollbar $w.scroll -command "$w.list yview"
  pack $w.scroll -side right -fill y

  file_files_update $w

  bind $w.list <Double-Button-1> {
    set file [selection get]
    if {($file == "..") && ($cwd != "/")} {
      cd ..
      set cwd [exec pwd]
    } else {
      set idx [expr [string length $file] - 1]
      set last [string range $file $idx $idx]
      if {$last == "/"} {
        cd $file
        set cwd [exec pwd]
      } else {
        set current_file $file
      }
    }
    file_files_update .file.f1.files
  }

}



# ------------------------------------------------------------------------
#  file_objects_update
# ------------------------------------------------------------------------
proc file_objects_update { w } {

  global    current_type  fmatrixlist ffnetlist

  while {[$w.list size] > 0} {
    $w.list  delete  0
  }

  if {$current_type == "FFNet"} {
    set slist [lsort $ffnetlist]
  } else {
    set slist [lsort $fmatrixlist]
  }

  foreach i $slist {
    $w.list insert end $i
  }

}



# ----------------------------------------------------------------------- 
#  file_objects                                                              
# ----------------------------------------------------------------------- 
proc  file_objects { w } {

  global    current_type current_object fmatrixlist ffnetlist

  set current_object ""

  frame $w.header
  pack $w.header -side top
  label $w.header.h1 -text "Object:" -fg red
  label $w.header.h2 -textvariable current_object -fg blue
  pack $w.header.h1 $w.header.h2 -side left
  listbox $w.list -relief raised -borderwidth 2 \
          -yscrollcommand "$w.scroll set"
  pack $w.list -side left
  scrollbar $w.scroll -command "$w.list yview"
  pack $w.scroll -side right -fill y

  file_objects_update $w

  bind $w.list <Double-Button-1> {
    set current_object [selection get]
  }

}


# -----------------------------------------------------------------------
#  newobj
# -----------------------------------------------------------------------
proc  newobj { type name } {

  global     ffnetlist fmatrixlist

  puts "Creating new object $name of type $type"

  if {[string length [info command $name]] > 0} {
    $name  destroy
  }

  if {$type == "FFNet"} {
    $type  $name 2 2 mse
    lappend ffnetlist $name
  }

  if {$type == "FMatrix"} {
    $type  $name
    lappend fmatrixlist $name
  }

  file_objects_update .file.f1.objects
  destroy .newobj
}



# -----------------------------------------------------------------------
#  destobj
# -----------------------------------------------------------------------
proc  destobj { type name } {

  global     ffnetlist fmatrixlist current_net

  if {$type == "FFNet"} {
    set idx [lsearch $ffnetlist $name]
    if {$idx >= 0} {
      puts "Destroying object $name of type $type"
      $name destroy
      set ffnetlist [lreplace $ffnetlist $idx $idx]
      if {$name == $current_net} { set current_net "" }
    }
  }

  if {$type == "FMatrix"} {
    set idx [lsearch $fmatrixlist $name]
    if {$idx >= 0} {
      puts "Destroying object $name of type $type"
      $name destroy
      set fmatrixlist [lreplace $fmatrixlist $idx $idx]
      puts $idx
    }
  }

  if [string length [info commands .file.f1.objects]] {
    file_objects_update .file.f1.objects
  }
  destroy .destobj
}



# -----------------------------------------------------------------------
#  file_new
# -----------------------------------------------------------------------
proc  file_new { } {

  global    current_type type
  set       type  $current_type
  set       name  ""

  if {[string length [info commands .newobj]]} { 
    puts "Already open\a\a"
    return
  }
  toplevel .newobj
  wm title .newobj "Create New Object"

  frame .newobj.f1
  frame .newobj.f2
  frame .newobj.f4
  pack .newobj.f1 .newobj.f2 .newobj.f4 -side top -expand 1 -fill x

  label .newobj.f1.l -text "Type:" -width 8
  entry .newobj.f1.e -width 20 -relief sunken -bd 2\
                     -textvariable type
  pack .newobj.f1.l .newobj.f1.e -side left -expand 1 -fill x

  label .newobj.f2.l -text "Name:" -width 8
  entry .newobj.f2.e -width 20 -relief sunken -bd 2\
                     -textvariable name
  pack .newobj.f2.l .newobj.f2.e -side left -expand 1 -fill x

  button .newobj.f4.b1 -text "      Ok      " -relief raised -bd 2\
               	       -command { newobj $type $name }
  button .newobj.f4.b2 -text "   Dismiss   " -relief raised -bd 2\
                       -command "destroy .newobj"
  pack .newobj.f4.b1 .newobj.f4.b2 -side left -expand 1 -fill x

}



# -----------------------------------------------------------------------
#  file_destroy
# -----------------------------------------------------------------------
proc  file_destroy { } {

  global    current_type current_object type name
  set       type $current_type
  set       name $current_object

  if {[string length $current_object] == 0} return

  if {[string length [info commands .destobj]]} { 
    puts "Already open\a\a"
    return
  }
  toplevel .destobj
  wm title .destobj "Destroy Object"

  frame .destobj.f1 -side left
  frame .destobj.f2 -side left
  frame .destobj.f4 -side left
  pack .destobj.f1 .destobj.f2 .destobj.f4 -side top -fill x -expand 1

  label .destobj.f1.l -text "Type:" -width 8
  entry .destobj.f1.e -width 20 -relief sunken -bd 2\
                     -textvariable type
  pack .destobj.f1.l .destobj.f1.e -side left -expand 1 -fill x

  label .destobj.f2.l -text "Name:" -width 8
  entry .destobj.f2.e -width 20 -relief sunken -bd 2\
                     -textvariable name
  pack .destobj.f2.l .destobj.f2.e -side left -expand 1 -fill x

  button .destobj.f4.b1 -text "      Ok      " -relief raised -bd 2\
               	       -command { destobj $type $name }
  button .destobj.f4.b2 -text "   Dismiss   " -relief raised -bd 2\
                       -command "destroy .destobj"
  pack .destobj.f4.b1 .destobj.f4.b2 -side left -expand 1 -fill x

}



# -----------------------------------------------------------------------
#  load
# -----------------------------------------------------------------------
proc  load { type filename objname } {

  if {(![string length $filename]) || (![string length $objname])} { 
    destroy .fload
  }

  if {$type == "FFNet"} {
    puts "Loading object $objname of type $type from file $filename"
    $objname  load  $filename
  }

  if {$type == "FMatrix"} {
    puts "Loading object $objname of type $type from file $filename"
    if {[string first ".bmat" $filename] >= 0} {
      $objname  bload  $filename
    } else {
      $objname  load  $filename
    }
  }

  destroy .fload
}



# -----------------------------------------------------------------------
#  file_load
# -----------------------------------------------------------------------
proc  file_load { } {

  global    current_type current_object current_file\
            type objname filename
  set       type     $current_type
  set       objname  $current_object
  set       filename $current_file

  if {[string length [info commands .fload]]} { 
    puts "Already open\a\a"
    return
  }
  toplevel .fload
  wm title .fload "Load Object from File"

  frame .fload.f1
  frame .fload.f2
  frame .fload.f3
  frame .fload.f4
  pack .fload.f1 .fload.f2 .fload.f3 .fload.f4 -side top -expand 1 -fill x

  label .fload.f1.l -text "Type:" -width 12
  entry .fload.f1.e -width 20 -relief sunken -bd 2\
                     -textvariable type
  pack .fload.f1.l .fload.f1.e -side left -expand 1 -fill x

  label .fload.f2.l -text "From File:" -width 12
  entry .fload.f2.e -width 20 -relief sunken -bd 2\
                     -textvariable filename
  pack .fload.f2.l .fload.f2.e -side left -expand 1 -fill x

  label .fload.f3.l -text "Into Object:" -width 12
  entry .fload.f3.e -width 20 -relief sunken -bd 2\
                     -textvariable objname
  pack .fload.f3.l .fload.f3.e -side left -expand 1 -fill x

  button .fload.f4.b1 -text "      Ok      " -relief raised -bd 2\
               	       -command { load  $type $filename $objname }
  button .fload.f4.b2 -text "   Dismiss   " -relief raised -bd 2\
                       -command "destroy .fload"
  pack .fload.f4.b1 .fload.f4.b2 -side left -expand 1 -fill x

}



# -----------------------------------------------------------------------
#  save
# -----------------------------------------------------------------------
proc  save { type objname filename } {

  if {(![string length $filename]) || (![string length $objname])} { 
    destroy .fsave
  }

  if {$type == "FFNet"} {
    puts "Saving object $objname of type $type into file $filename"
    $objname  save  $filename
  }

  if {$type == "FMatrix"} {
    puts "Saving object $objname of type $type into file $filename"
    $objname  bsave  $filename
  }

  file_files_update .file.f1.files
  destroy .fsave
}



# -----------------------------------------------------------------------
#  file_save
# -----------------------------------------------------------------------
proc  file_save { } {

  global    current_type current_object current_file\
            type objname filename
  set       type     $current_type
  set       objname  $current_object
  set       filename $current_file

  if {[string length [info commands .fsave]]} { 
    puts "Already open\a\a"
    return
  }
  toplevel .fsave
  wm title .fsave "Save Object into File"

  frame .fsave.f1
  frame .fsave.f2
  frame .fsave.f3
  frame .fsave.f4
  pack .fsave.f1 .fsave.f2 .fsave.f3 .fsave.f4 -side top -fill x -expand 1

  label .fsave.f1.l -text "Type:" -width 12
  entry .fsave.f1.e -width 20 -relief sunken -bd 2\
                     -textvariable type
  pack .fsave.f1.l .fsave.f1.e -side left -expand 1 -fill x

  label .fsave.f2.l -text "From Object:" -width 12
  entry .fsave.f2.e -width 20 -relief sunken -bd 2\
                     -textvariable objname
  pack .fsave.f2.l .fsave.f2.e -side left -expand 1 -fill x

  label .fsave.f3.l -text "Into File:" -width 12
  entry .fsave.f3.e -width 20 -relief sunken -bd 2\
                     -textvariable filename
  pack .fsave.f3.l .fsave.f3.e -side left -expand 1 -fill x

  button .fsave.f4.b1 -text "      Ok      " -relief raised -bd 2\
               	       -command { save  $type $objname $filename }
  button .fsave.f4.b2 -text "   Dismiss   " -relief raised -bd 2\
                       -command "destroy .fsave"
  pack .fsave.f4.b1 .fsave.f4.b2 -side left -expand 1 -fill x

}



# ----------------------------------------------------------------------- 
#  file_buttons                                                              
# ----------------------------------------------------------------------- 
proc  file_buttons { w } {

  global  current_type current_file current_object

  radiobutton $w.rb1 -text "FFNet" -variable current_type\
              -value FFNet -anchor w -relief raised\
              -command {file_objects_update .file.f1.objects;\
                        file_files_update .file.f1.files;\
                        set current_file ""; set current_object ""}
  radiobutton $w.rb2 -text "FMatrix" -variable current_type\
              -value FMatrix -anchor w -relief raised\
              -command {file_objects_update .file.f1.objects;\
                        file_files_update .file.f1.files;\
                        set current_file ""; set current_object ""}
  pack $w.rb1 $w.rb2 -side top -fill x -expand 1

  button $w.load   -text "Load"    -relief raised -borderwidth 2\
	           -command { file_load }
  button $w.save   -text "Save"    -relief raised -borderwidth 2\
	           -command { file_save }
  button $w.new    -text "New"     -relief raised -borderwidth 2\
	           -command { file_new }
  button $w.kill   -text "Destroy" -relief raised -borderwidth 2\
	           -command { file_destroy }
  button $w.dismiss -text "Dismiss"  -relief raised -borderwidth 2\
                   -command "destroy .file"
  pack $w.load $w.save $w.new $w.kill $w.dismiss\
       -side top -fill x -expand 1

}



# ----------------------------------------------------------------------- 
#  file_menu                                                              
# ----------------------------------------------------------------------- 
proc file_menu {} {

  global   ffnetlist fmatrixlist cwd

  if {[string length [info commands .file]]} { 
    puts "Already open\a\a"
    return
  }
  toplevel .file
  wm title .file "File"

  frame .file.f1 -width 340 -height 240
  frame .file.f2 -width 340 -height 50
  pack .file.f1 .file.f2 -side top

  frame .file.f1.files -relief raised -borderwidth 2
  frame .file.f1.objects -relief raised -borderwidth 2
  frame .file.f1.buttons
  pack .file.f1.files .file.f1.objects .file.f1.buttons -side left

  label .file.f2.pwd -textvariable cwd
  pack  .file.f2.pwd -side left

  file_files   .file.f1.files
  file_objects .file.f1.objects
  file_buttons .file.f1.buttons
}



# ------------------------------------------------------------------------
#  edit_update
# ------------------------------------------------------------------------
proc edit_update { w } {

  global    current_net ffnetlist

  while {[$w size] > 0} {
    $w  delete  0
  }

  set slist [lsort $ffnetlist]
  foreach i $slist {
    $w insert end $i
  }

}



# -----------------------------------------------------------------------
#  draw_node                                                            
# ----------------------------------------------------------------------- 
proc  draw_node { cv x y } {

  set xx $x
  set yy $y
  for {set i 0} {$i <= 5} {incr i} {
    set idx [expr 20 + $i*15]
    set dx [expr 5 - $i]
    set dy [expr 5 - $i]
    set x1 [expr $xx - $dx]
    set y1 [expr $yy - $dy]
    set x2 [expr $xx + $dx]
    set y2 [expr $yy + $dy]
    $cv create oval $x1 $y1 $x2 $y2 -fill grey$idx -outline grey$idx
    set xx [expr $xx - 0.7]
    set yy [expr $yy - 0.7]
  }   
}



# -----------------------------------------------------------------------
#  edit_draw                                                            
# ----------------------------------------------------------------------- 
proc  edit_draw { w } {

  global    current_net

  set color(0)  blue
  set color(1)  red
  set color(2)  green
  set color(3)  black
  set color(4)  #FF00FF

  destroy $w
  canvas $w -width 600 -height 600
  pack $w -side left

  if {[string length $current_net] == 0} return 

  set layerN [$current_net configure -layerN]
  if {$layerN > 0} {
    set max [$current_net configure -inputN]
    for {set layerX 0} {$layerX < $layerN} {incr layerX} {
      set neuronN [$current_net.layer($layerX) configure -neuronN]
      if {$neuronN > $max} {
        set max $neuronN
      }
    }
    set layerN [expr $layerN + 1]
    set dx [expr 600.0 / $max]
    set dy [expr 600.0 / $layerN]
    set oy [expr $dy / 2]

    for {set layerX 0} {$layerX < $layerN} {incr layerX} {
      if {$layerX < [expr $layerN - 1]} {
        set neuronN [$current_net.layer($layerX) configure -neuronN]
      } else {
        set neuronN [$current_net configure -inputN]
      }
      set width [expr int($dx * ($neuronN - 1))]
      set offset [expr (600 - $width)/2]
      set y [expr $oy + int($dy*$layerX)]
      for {set k 0} {$k < $neuronN} {incr k} {
        set x [expr $offset + int($dx*$k)]
        draw_node $w $x $y
      }
    }

    for {set layerX 0} {$layerX < $layerN} {incr layerX} {
      if {$layerX < [expr $layerN - 1]} {
        set neuronN [$current_net.layer($layerX) configure -neuronN]
        set width [expr int($dx * ($neuronN - 1))]
        set offset1 [expr (600 - $width)/2]
        set y1 [expr $oy + int($dy*$layerX) + 6]
        set linkN [$current_net.layer($layerX) configure -linkN]
        for {set linkX 0} {$linkX < $linkN} {incr linkX} {
	  set sourceX [$current_net.layer($layerX).link($linkX) configure -sourceX]
          if {$sourceX >= 0} {
            set srcN [$current_net.layer($sourceX) configure -neuronN]
            set y2 [expr $oy + int($dy*$sourceX) - 6]
          } else {
            set srcN [$current_net configure -inputN]
            set y2 [expr $oy + int($dy*($layerN-1)) - 6]
          }
          set width [expr int($dx * ($srcN - 1))]
          set offset2 [expr (600 - $width)/2]

          for {set i 0} {$i < $srcN} {incr i} {
            set x2 [expr $offset2 + int($dx*$i)]
            for {set j 0} {$j < $neuronN} {incr j} { 
              set x1 [expr $offset1 + int($dx*$j)]
              $w create line $x1 $y1 $x2 $y2 -fill $color($linkX) -arrow first
            }
	  }
        }
      }
    }
  }
}



# -----------------------------------------------------------------------
#  edit_new_ffnet      
# ----------------------------------------------------------------------- 
proc edit_new_ffnet { name outputN inputN errFct } {

  global     ffnetlist current_net

  if {[string length [info command $name]] > 0} {
    $name  destroy
  }

  puts "Creating new FFNet $name $outputN $inputN $errFct"

  if [catch {FFNet  $name $outputN $inputN $errFct} msg] {
    puts $msg
  } else {
    switch $errFct {
      mse   { set actfct linear }
      binom { set actfct sigmoid }
      multi { set actfct softmax }
      class { set actfct softmax }
    }
    $name add layer $outputN $actfct
    lappend ffnetlist $name
  }

  set current_net $name
  edit_update .edit.f1.l.bot.lb
  edit_draw .edit.f1.r.c
  destroy .newobj  
}



# -----------------------------------------------------------------------
#  edit_new                                                          
# ----------------------------------------------------------------------- 
proc edit_new {} {

  global    name inputN outputN errFct
  set       name    "net"
  set       inputN  "2"
  set       outputN "2"
  set       errFct  mse

  if {[string length [info commands .newobj]]} { 
    puts "Already open\a\a"
    return
  }
  toplevel .newobj
  wm title .newobj "Create New FFNet"

  frame .newobj.f1 -relief raised -bd 2 -width 200
  frame .newobj.f2 -relief raised -bd 2 -width 200
  frame .newobj.f3 -relief raised -bd 2 -width 200
  frame .newobj.f4 -relief raised -bd 2 -width 200
  frame .newobj.f5 -width 200
  pack .newobj.f1 .newobj.f2 .newobj.f3 .newobj.f4 .newobj.f5 -side top\
           -fill x -expand 1

  label .newobj.f1.l -text "Name:" -width 8
  entry .newobj.f1.e -width 20 -relief sunken -bd 2\
                     -textvariable name
  pack .newobj.f1.l .newobj.f1.e -side left -expand 1 -fill x

  label .newobj.f2.l -text "OutputN:" -width 8
  entry .newobj.f2.e -width 20 -relief sunken -bd 2\
                     -textvariable outputN
  pack .newobj.f2.l .newobj.f2.e -side left -expand 1 -fill x

  label .newobj.f3.l -text "InputN:" -width 8
  entry .newobj.f3.e -width 20 -relief sunken -bd 2\
                     -textvariable inputN
  pack .newobj.f3.l .newobj.f3.e -side left -expand 1 -fill x

  label  .newobj.f4.l1 -text "ErrorFunction:"
  radiobutton .newobj.f4.rb1 -text "Mean Square Error" -variable errFct -value mse\
                             -anchor w
  radiobutton .newobj.f4.rb2 -text "X-entropy (binom)" -variable errFct -value binom\
                             -anchor w
  radiobutton .newobj.f4.rb3 -text "X-entropy (multi)" -variable errFct -value multi\
                             -anchor w
  radiobutton .newobj.f4.rb4 -text "Classification Error" -variable errFct -value class\
                             -anchor w
  pack .newobj.f4.l1 .newobj.f4.rb1 .newobj.f4.rb2 .newobj.f4.rb3\
                 .newobj.f4.rb4 -side top -fill x -expand 1

  button .newobj.f5.b1 -text "      Ok      " -relief raised -bd 2\
               	       -command { edit_new_ffnet $name $outputN $inputN $errFct}
  button .newobj.f5.b2 -text "   Dismiss   " -relief raised -bd 2\
                       -command "destroy .newobj"
  pack .newobj.f5.b1 .newobj.f5.b2 -side left -expand 1 -fill x
  
}



# -----------------------------------------------------------------------
#  edit_destroy_ffnet      
# ----------------------------------------------------------------------- 
proc edit_destroy_ffnet { name } {

  global     ffnetlist current_net inputN outputN errFct

  set idx [lsearch $ffnetlist $name]
  if {$idx >= 0} {
    puts "Destroying FFNet $name"
    $name destroy
    set ffnetlist [lreplace $ffnetlist $idx $idx]
  }

  edit_update .edit.f1.l.bot.lb
  if {$current_net == $name} {
    set current_net ""
    set inputN ""
    set outputN ""
    set errFct ""
    edit_draw .edit.f1.r.c
  } 

  destroy .destobj
}



# -----------------------------------------------------------------------
#  edit_destroy                                                            
# ----------------------------------------------------------------------- 
proc edit_destroy {} {

  global    name current_net
  set       name $current_net

  if {[string length $current_net] == 0} return

  if {[string length [info commands .destobj]]} { 
    puts "Already open\a\a"
    return
  }
  toplevel .destobj
  wm title .destobj "Destroy FFNet"

  frame .destobj.f2
  frame .destobj.f4
  pack .destobj.f2 .destobj.f4 -side top

  label .destobj.f2.l -text "Name:" -width 8
  entry .destobj.f2.e -width 20 -relief sunken -bd 2\
                     -textvariable name
  pack .destobj.f2.l .destobj.f2.e -side left -expand 1 -fill x

  button .destobj.f4.b1 -text "      Ok      " -relief raised -bd 2\
               	       -command { edit_destroy_ffnet $name }
  button .destobj.f4.b2 -text "   Dismiss   " -relief raised -bd 2\
                       -command "destroy .destobj"
  pack .destobj.f4.b1 .destobj.f4.b2 -side left -expand 1 -fill x
  
}



# -----------------------------------------------------------------------
#  add_layer
# ----------------------------------------------------------------------- 
proc add_layer { neuronN actfct } {

  global      current_net

  if [catch {$current_net  add layer $neuronN $actfct} msg] {
    puts $msg
  }

  edit_draw .edit.f1.r.c
  destroy .layer
}



# -----------------------------------------------------------------------
#  edit_layer
# ----------------------------------------------------------------------- 
proc edit_layer {} {

  global    current_net actfct

  if {[string length $current_net] == 0} return

  if {[string length [info commands .layer]]} { 
    puts "Already open\a\a"
    return
  }
  toplevel .layer
  wm title .layer "Adding Layer"

  set neuronN [$current_net configure -outputN]

  frame .layer.f1 -width 300 -relief raised -bd 2
  frame .layer.f2 -width 300 -relief raised -bd 2
  frame .layer.f3 -width 300 
  pack .layer.f1 .layer.f2 .layer.f3 -side top -fill x -expand 1
  
  label .layer.f1.l -text "NeuronN" -anchor w
  pack .layer.f1.l -side left -fill x -expand 1
  entry .layer.f1.e -width 10 -relief sunken -bd 2\
                     -textvariable neuronN
  pack .layer.f1.e -side right -fill x -expand 1

  set actfct linear
  label .layer.f2.l1 -text "Activation Fct"
  radiobutton .layer.f2.rb1 -text "linear" -variable actfct -value linear\
                -anchor w
  radiobutton .layer.f2.rb2 -text "sigmoid" -variable actfct -value sigmoid\
                -anchor w
  radiobutton .layer.f2.rb3 -text "tanh" -variable actfct -value tanh\
                -anchor w
  radiobutton .layer.f2.rb4 -text "exp" -variable actfct -value exp\
                -anchor w
  radiobutton .layer.f2.rb5 -text "softmax" -variable actfct -value softmax\
                -anchor w
  pack .layer.f2.l1 .layer.f2.rb1 .layer.f2.rb2 .layer.f2.rb3\
        .layer.f2.rb4 .layer.f2.rb5 -side top -fill x -expand 1

  button .layer.f3.b1 -text "OK" -command { add_layer $neuronN $actfct }
  button .layer.f3.b2 -text "Dismiss" -command { destroy .layer }
  pack .layer.f3.b1 .layer.f3.b2 -side left -fill x -expand 1

}



# -----------------------------------------------------------------------
#  add_link                                                            
# ----------------------------------------------------------------------- 
proc add_link { targetX sourceX netFct } {

  global        current_net

  if [catch {$current_net add link $targetX $sourceX -net $netFct} msg] {
    puts $msg
  }

  edit_draw .edit.f1.r.c
  destroy .link
}



# -----------------------------------------------------------------------
#  edit_link                                                            
# ----------------------------------------------------------------------- 
proc edit_link {} {

  global    current_net netfct

  set sourceX ""
  set targetX ""
  set netfct  "projective"

  if {[string length $current_net] == 0} return

  if {[string length [info commands .link]]} { 
    puts "Already open\a\a"
    return
  }
  toplevel .link
  wm title .link "Adding Link"

  frame .link.f1 -width 300 -relief raised -bd 2
  frame .link.f2 -width 300 -relief raised -bd 2
  frame .link.f3 -width 300 -relief raised -bd 2
  frame .link.f4 -width 300 
  pack .link.f1 .link.f2 .link.f3 .link.f4 -side top -fill x -expand 1

  label .link.f1.l1 -text "Target Layer" -anchor w
  pack .link.f1.l1 -side left -fill x -expand 1
  entry .link.f1.e1 -width 10 -relief sunken -bd 2\
                     -textvariable targetX
  pack .link.f1.e1 -side right -fill x -expand 1

  label .link.f2.l2 -text "Source Layer" -anchor w
  pack .link.f2.l2 -side left -fill x -expand 1
  entry .link.f2.e2 -width 10 -relief sunken -bd 2\
                     -textvariable sourceX
  pack .link.f2.e2 -side right -fill x -expand 1

  label .link.f3.l1 -text "Net Function"
  radiobutton .link.f3.rb1 -text "projective" -variable netfct -value\
                projective -anchor w
  radiobutton .link.f3.rb2 -text "radial" -variable netfct -value radial\
                -anchor w
  pack .link.f3.l1 .link.f3.rb1 .link.f3.rb2 -side top -fill x -expand 1

  button .link.f4.b1 -text "OK" -command { add_link $targetX $sourceX $netfct }
  button .link.f4.b2 -text "Dismiss" -command { destroy .link }
  pack .link.f4.b1 .link.f4.b2 -side left -fill x -expand 1

}


# -----------------------------------------------------------------------
#  edit_info
# ----------------------------------------------------------------------- 
proc edit_info {} {

  global    current_net

  if {[string length $current_net] == 0} return

  if {[string length [info commands .info]]} { 
    puts "Already open\a\a"
    return
  }
  toplevel .info
  wm title .info "Info on FFNet '$current_net'"

  frame .info.f1 -width 400 -bd 2 -relief raised
  frame .info.f2 -width 400
  frame .info.f3 -width 400
  pack .info.f1 .info.f2 .info.f3 -side top -fill x -expand 1

  set outputN [$current_net configure -outputN]
  set inputN  [$current_net configure -inputN]
  set errFct  [lindex [lindex [$current_net puts] 3] 1]
  set layerN  [$current_net configure -layerN]

  label .info.f1.l1 -text "Name : $current_net" -anchor w
  pack .info.f1.l1 -side top -fill x -expand 1

  label .info.f1.l2 -text "OutputN : $outputN" -anchor w
  pack .info.f1.l2 -side top -fill x -expand 1

  label .info.f1.l3 -text "InputN : $inputN" -anchor w
  pack .info.f1.l3 -side top -fill x -expand 1

  label .info.f1.l4 -text "Error Function : $errFct" -anchor w
  pack .info.f1.l4 -side top -fill x -expand 1

  label .info.f1.l5 -text "LayerN : $layerN" -anchor w
  pack .info.f1.l5 -side top -fill x -expand 1

  set layerN [$current_net configure -layerN]
  for {set layerX 0} {$layerX<$layerN} {incr layerX} {
    frame .info.f2.l$layerX -width 400 -bd 2 -relief raised
    pack .info.f2.l$layerX -side top -fill x -expand 1

    label .info.f2.l$layerX.h -text "Layer $layerX" -anchor w
    pack .info.f2.l$layerX.h -side top -fill x -anchor w

    set actfct [lindex [lindex [$current_net.layer($layerX) puts] 1] 1]
    label .info.f2.l$layerX.l1 -text "      ActFct : $actfct" -anchor w
    pack .info.f2.l$layerX.l1 -side top -fill x -anchor w

    set neuronN [$current_net.layer($layerX) configure -neuronN]
    label .info.f2.l$layerX.l2 -text "      NeuronN : $neuronN" -anchor w
    pack .info.f2.l$layerX.l2 -side top -fill x -anchor w

    set linkN [$current_net.layer($layerX) configure -linkN]
    label .info.f2.l$layerX.l3 -text "      LinkN : $linkN" -anchor w
    pack .info.f2.l$layerX.l3 -side top -fill x -anchor w

    for {set linkX 0} {$linkX < $linkN} {incr linkX} {
      frame .info.f2.l$layerX.li$linkX -width 400
      pack .info.f2.l$layerX.li$linkX -side top -fill x -expand 1

      label .info.f2.l$layerX.li$linkX.h -text "      Link $linkX" -anchor w
      pack .info.f2.l$layerX.li$linkX.h -side top -fill x -anchor w
      
      set srcX [$current_net.layer($layerX).link($linkX) configure -sourceX]
      label .info.f2.l$layerX.li$linkX.l1 -text "            Coming from : Layer $srcX" -anchor w
      pack .info.f2.l$layerX.li$linkX.l1 -side top -fill x -anchor w

      set netfct [lindex [lindex [$current_net.layer($layerX).link($linkX) puts] 1] 1]
      label .info.f2.l$layerX.li$linkX.l2 -text "            NetFct : $netfct" -anchor w
      pack .info.f2.l$layerX.li$linkX.l2 -side top -fill x -anchor w
    }
  }

  frame .info.f2.ll -width 400 -bd 2 -relief raised
  pack .info.f2.ll -side top -fill x -expand 1
  label .info.f2.ll.h -text "Layer -1 (input)" -anchor w
  pack .info.f2.ll.h -side top -fill x -anchor w
  set neuronN [$current_net configure -inputN]
  label .info.f2.ll.l2 -text "      NeuronN : $neuronN" -anchor w
  pack .info.f2.ll.l2 -side top -fill x -anchor w

  button .info.f3.b1 -text "Dismiss" -command { destroy .info }
  pack .info.f3.b1 -side left -fill x -expand 1
}



# -----------------------------------------------------------------------
#  edit_menu                                                            
# ----------------------------------------------------------------------- 
proc edit_menu {} {

  global   current_net current_type current_object\
           inputN outputN errFct

  set current_type FFNet

  if {[string length [info commands .edit]]} { 
    puts "Already open\a\a"
    return
  }
  toplevel .edit
  wm title .edit "NetEdit"

  frame  .edit.f1 -relief raised -bd 2 -width 800 -height 600
  frame  .edit.f2 -relief raised -bd 2 -width 800 -height 40
  pack .edit.f1 .edit.f2 -side top -fill both

  frame .edit.f1.l -width 200 -height 600
  frame .edit.f1.r -width 600 -height 600
  pack .edit.f1.l .edit.f1.r -side left -fill both

  frame .edit.f1.l.top -width 200 -height 50 -relief raised -bd 2
  frame .edit.f1.l.bot -width 200 -height 550
  pack .edit.f1.l.top .edit.f1.l.bot -side top -fill both

  label .edit.f1.l.top.lab1 -text "Net: "
  label .edit.f1.l.top.lab2 -textvariable current_net
  pack .edit.f1.l.top.lab1 .edit.f1.l.top.lab2 -side left -fill x -expand 1

  listbox .edit.f1.l.bot.lb -height 35 -relief raised -borderwidth 2 \
          -yscrollcommand ".edit.f1.l.bot.scroll set"
  pack .edit.f1.l.bot.lb -side left
  scrollbar .edit.f1.l.bot.scroll -command ".edit.f1.l.bot.lb yview"
  pack .edit.f1.l.bot.scroll -side right -fill y

  bind .edit.f1.l.bot.lb <Double-Button-1> {
    set current_net [selection get]
    set inputN [$current_net configure -inputN]
    set outputN [$current_net configure -outputN]
    set errFct [lindex [lindex [$current_net puts] 3] 1]
    edit_draw .edit.f1.r.c
  }
  edit_update .edit.f1.l.bot.lb

  frame .edit.f1.r.s -width 600 -height 50
  canvas .edit.f1.r.c -width 600 -height 600
  pack .edit.f1.r.s .edit.f1.r.c -side top
  edit_draw .edit.f1.r.c

  label .edit.f1.r.s.l1 -text "           OutputN = "
  label .edit.f1.r.s.l2 -textvariable outputN
  label .edit.f1.r.s.l3 -text "           InputN = "
  label .edit.f1.r.s.l4 -textvariable inputN
  label .edit.f1.r.s.l5 -text "           ErrFct = "
  label .edit.f1.r.s.l6 -textvariable errFct
  pack .edit.f1.r.s.l1 .edit.f1.r.s.l2 .edit.f1.r.s.l3 .edit.f1.r.s.l4 \
       .edit.f1.r.s.l5 .edit.f1.r.s.l6 -side left

  button .edit.f2.b1 -text "Dismiss" -command "destroy .edit"
  button .edit.f2.b2 -text "New" -command { edit_new }
  button .edit.f2.b3 -text "Destroy" -command { edit_destroy }
  button .edit.f2.b4 -text "AddLayer" -command { edit_layer }
  button .edit.f2.b5 -text "AddLink" -command { edit_link }
  button .edit.f2.b6 -text "Info" -command { edit_info }
  pack .edit.f2.b1 .edit.f2.b2 .edit.f2.b3 .edit.f2.b4 .edit.f2.b5\
       .edit.f2.b6  -side left -fill x -expand 1

}



# -----------------------------------------------------------------------
#  randomize                                                            
# ----------------------------------------------------------------------- 
proc  randomize { net range } {

  if {![string length $net]} {
    puts stderr "No network selected"
    return
  }

  puts "Randomizing weights of network $net"
  $net randomize -range $range
}



# -----------------------------------------------------------------------
#  update_trw
# ----------------------------------------------------------------------- 
proc  update_trw { modu } {

  global   global_logN global_log global_xlog chuwi itemList

  incr chuwi
  if {$chuwi < $modu } return
  set chuwi 0

  foreach item $itemList {
    .trw.f1.c delete $item
  }
  set itemList ""

  if {$global_logN == 0}  return

  set maxX 0
  set maxY 0.0
  for {set logX 0} {$logX < $global_logN} {incr logX} {  
    set tlist $global_log($logX)
    set N [llength $tlist]
    for {set k 0} {$k < $N} {incr k} {
      set element [lindex $tlist $k]
      set y [lindex $element 1]
      if {$y > $maxY} { set maxY $y }
    }
    if {$N > $maxX} { set maxX $N }
  }

  set maxX [expr 10*(($maxX / 10)+1)]
  set dx [expr 500.0 / $maxX]
  set dy [expr 350.0 / $maxY]

  # draw axis
  set item [.trw.f1.c create line 40 375 560 375 -arrow last]
  lappend itemList $item
  set item [.trw.f1.c create line 50 385 50 15 -arrow last]
  lappend itemList $item
  set item [.trw.f1.c create text 560 385 -text "epoch"]
  lappend itemList $item
  set item [.trw.f1.c create text 20 10 -text "error"]
  lappend itemList $item

  set dd [expr $maxX / 11]
  if {$dd == 0} { set dd 1 }
  for {set k 0} {$k < $maxX} {incr k} {
    if {[expr $k % $dd] == 0} {
      set x [expr int(50 + $k*$dx)]      
      set item [.trw.f1.c create line $x 372 $x 378]
      lappend itemList $item
      set item [.trw.f1.c create text $x 385 -text "$k"]
      lappend itemList $item
    }
  }

  set dd2 [expr $maxY / 11.0]
  for {set k 0} {$k < 11} {incr k} {
    set val [expr $k * $dd2]
    set y [expr int(375 - $val*$dy)]
    set item [.trw.f1.c create line 47 $y 53 $y]
    lappend itemList $item
    set item [.trw.f1.c create text 25 $y -text [format "%5.2f" $val]]
    lappend itemList $item
  }

  # draw curves 
  set terr ""
  set xerr ""

  for {set logX 0} {$logX < $global_logN} {incr logX} {
    set tlist $global_log($logX)
    set N [llength $tlist]
    set oldval [lindex [lindex $tlist 0] 1]
    for {set k 1} {$k < $N} {incr k} {
      set element [lindex $tlist $k]
      set val [lindex $element 1]
      set terr $val
      set x1 [expr int(50 + ($k-1)*$dx)]
      set x2 [expr int(50 + $k*$dx)]
      set y1 [expr int(375 - $oldval*$dy)]
      set y2 [expr int(375 - $val*$dy)]
      set item [.trw.f1.c create line $x1 $y1 $x2 $y2 -fill red]
      lappend itemList $item
      set oldval $val
    }

    set xlog $global_xlog($logX)
    if {[llength $xlog] > 0} {
      set N [llength $xlog]
      set oldval [lindex $xlog 0]
      for {set k 1} {$k < $N} {incr k} {
        set val [lindex $xlog $k]
        set xerr $val
        set x1 [expr int(50 + ($k-1)*$dx)]
        set x2 [expr int(50 + $k*$dx)]
        set y1 [expr int(375 - $oldval*$dy)]
        set y2 [expr int(375 - $val*$dy)]
        set item [.trw.f1.c create line $x1 $y1 $x2 $y2 -fill blue]
        lappend itemList $item
        set oldval $val
      }      
    }
  }

  # draw key
  set item [.trw.f1.c create text 500 30 -text "Training Set ($terr)" -fill red]
  lappend itemList $item
  set item [.trw.f1.c create text 500 50 -text "X-Validation Set ($xerr)" -fill blue]
  lappend itemList $item

  update idletasks
}



# -----------------------------------------------------------------------
#  update_train_window                                                            
# ----------------------------------------------------------------------- 
proc  update_train_window { modu } {

  global     global_logN

  if {![string length [info commands .trw]]} { 
    toplevel .trw
    wm title .trw "Training Network"
    
    frame .trw.f1 -width 600 -height 400
    frame .trw.f2 -width 600 -height 50
    pack .trw.f1 .trw.f2 -side top -fill x -expand 1

    canvas .trw.f1.c -width 600 -height 400 -relief raised -bd 2
    pack .trw.f1.c -side left -fill x -expand 1

    button .trw.f2.b1 -text "Dismiss" -command { destroy .trw }
    button .trw.f2.b2 -text "Clear"   -command { set global_logN 0;\
                                                     update_trw 0 }
    pack .trw.f2.b1 .trw.f2.b2 -side left -fill x -expand 1

    update_trw 0
  } else {
    update_trw $modu
  }
}



# -----------------------------------------------------------------------
#  train                                                            
# ----------------------------------------------------------------------- 
proc  train { net input target testinput testtarget iterN batchN \
              eta etaFac gamma moment adapt shuffle errFct modu } {

  global     global_logN  global_log global_xlog

  if {![string length $net]} {
    puts stderr "No network selected"
    return
  }

  if {![string length $input]} {
    puts stderr "No input matrix selected"
    return
  }

  if {![string length $target]} {
    puts stderr "No target matrix selected"
    return
  }

  if {$iterN <= 0} {
    puts stderr "Invalid number of iterations"
    return
  }

  if {$batchN < 0} {
    puts stderr "Invalid batch size"
    return
  }

  if {$eta <= 0.0} {
    puts stderr "Invalid learning rate"
    return
  }

  if {($etaFac <= 0.0) || ($etaFac > 1.0)} {
    puts stderr "Invalid learning rate annealing factor"
    return
  }

  if {($gamma < 0.0) || ($gamma > 1.0)} {
    puts stderr "Invalid momentum weight"
    return
  }

  if {([string length $testinput] > 0) && 
      ([string length $testtarget] > 0)} {
    set xval 1
  } else {
    set xval 0
  }

  set idx $global_logN
  incr global_logN
  set global_log($idx) ""
  set global_xlog($idx) ""
  set current_eta $eta

  $net confLinks -eta 0.0 -eval 1 -update 0
  $net backprop $input $target -iterN 1 -batchN $batchN -logfile stderr\
                 -errFct $errFct

  if {[catch {set fp [open /tmp/log.ffnet r]} msg]} {
    puts stderr "Wrong version of JRTk - no training log available"
    exit
  } 
  gets $fp line
  lappend global_log($idx) $line
  close $fp   

  if {$xval} { 
    set xErr [$net eval $testinput dummy_output -target $testtarget\
		        -errFct $errFct]
    lappend global_xlog($idx) $xErr
  }

  for {set iterX 0} {$iterX < $iterN} {incr iterX} {
    $net  confLinks -eta $current_eta -alpha $gamma -eval 1 -update 1
    $net backprop $input $target -iterN 1 -batchN $batchN -moment $moment\
                  -adapt $adapt -errFct $errFct -shuffle $shuffle\
                  -logfile stderr

    set current_eta [expr $current_eta * $etaFac]

    if {[catch {set fp [open /tmp/log.ffnet r]} msg]} {
      puts stderr "Wrong version of JRTk - no training log available"
      exit
    } 
    gets $fp line
    lappend global_log($idx) $line
    close $fp   

    if {$xval} { 
      set xErr [$net eval $testinput dummy_output -target $testtarget\
  		        -errFct $errFct]
      lappend global_xlog($idx) $xErr
    }

    update_train_window $modu

  }

  update_train_window 0
}



# -----------------------------------------------------------------------
#  train_menu                                                            
# ----------------------------------------------------------------------- 
proc train_menu {} {

  global     iterN batchN eta etaFac gamma moment adapt random errFct\
             current_net current_input current_target\
             current_testinput current_testtarget\
             ffnetlist fmatrixlist modu

  if {[string length [info commands .train]]} { 
    puts "Already open\a\a"
    return
  }
  toplevel .train
  wm title .train "NetTrain"

  set iterN   100
  set batchN    1
  set eta     0.1
  set etaFac  1.0
  set gamma   0.0
  set moment 0
  set adapt  0
  set random 0.01
  set errFct mse
  set current_net ""
  set current_input ""
  set current_output ""
  set current_target ""
  set current_testinput ""
  set current_testtarget ""
  set modu 1

  frame .train.f1 -width 600
  frame .train.f2 -width 600
  pack .train.f1 .train.f2 -side top -fill x -expand 1

  frame .train.f1.f1 -width 150 -relief raised -bd 2
  frame .train.f1.f2 -width 150 -relief raised -bd 2
  frame .train.f1.f3 -width 150 -relief raised -bd 2
  frame .train.f1.f4 -width 150 -relief raised -bd 2
  pack .train.f1.f1 .train.f1.f2 .train.f1.f3 .train.f1.f4\
           -side left -fill x -expand 1

  # buttons and entries for training
  # --------------------------------

  frame .train.f1.f1.f1 -width 150
  frame .train.f1.f1.f2 -width 150
  frame .train.f1.f1.f3 -width 150
  frame .train.f1.f1.f4 -width 150
  frame .train.f1.f1.f5 -width 150
  frame .train.f1.f1.f6 -width 150
  frame .train.f1.f1.f7 -width 150
  pack .train.f1.f1.f1 .train.f1.f1.f2 .train.f1.f1.f3 .train.f1.f1.f4\
       .train.f1.f1.f5 .train.f1.f1.f7 .train.f1.f1.f6 -side top -fill x -expand 1

  label .train.f1.f1.f1.l -text "Iterations/Epochs" -anchor w
  pack .train.f1.f1.f1.l -side left -expand 1 -fill x
  entry .train.f1.f1.f1.e -width 10 -relief sunken -bd 2\
                          -textvariable iterN
  pack .train.f1.f1.f1.e -side right -expand 1 -fill x

  label .train.f1.f1.f2.l -text "Batch Size" -anchor w
  pack .train.f1.f1.f2.l -side left -expand 1 -fill x
  entry .train.f1.f1.f2.e -width 10 -relief sunken -bd 2\
                          -textvariable batchN
  pack .train.f1.f1.f2.e -side right -expand 1 -fill x

  label .train.f1.f1.f3.l -text "Learning Rate" -anchor w
  pack .train.f1.f1.f3.l -side left -expand 1 -fill x
  entry .train.f1.f1.f3.e -width 10 -relief sunken -bd 2\
                          -textvariable eta
  pack .train.f1.f1.f3.e -side right -expand 1 -fill x

  label .train.f1.f1.f4.l -text "Learning Rate Annealing" -anchor w
  pack .train.f1.f1.f4.l -side left -expand 1 -fill x
  entry .train.f1.f1.f4.e -width 10 -relief sunken -bd 2\
                          -textvariable etaFac
  pack .train.f1.f1.f4.e -side right -expand 1 -fill x

  label .train.f1.f1.f5.l -text "Momentum Weight" -anchor w
  pack .train.f1.f1.f5.l -side left -expand 1 -fill x
  entry .train.f1.f1.f5.e -width 10 -relief sunken -bd 2\
                          -textvariable gamma
  pack .train.f1.f1.f5.e -side right -expand 1 -fill x

  label .train.f1.f1.f6.l -text "Randomization Range" -anchor w
  pack .train.f1.f1.f6.l -side left -expand 1 -fill x
  entry .train.f1.f1.f6.e -width 10 -relief sunken -bd 2\
                          -textvariable random
  pack .train.f1.f1.f6.e -side right -expand 1 -fill x

  label .train.f1.f1.f7.l -text "Plot Update Frequency" -anchor w
  pack .train.f1.f1.f7.l -side left -expand 1 -fill x
  entry .train.f1.f1.f7.e -width 10 -relief sunken -bd 2\
                          -textvariable modu
  pack .train.f1.f1.f7.e -side right -expand 1 -fill x

  label .train.f1.f1.l5 -text "  "
  pack .train.f1.f1.l5 -side top -expand 1 -fill x

  checkbutton .train.f1.f1.cb1 -text "Use Momentum Term" -variable moment -anchor w
  pack .train.f1.f1.cb1 -side top -expand 1 -fill x
  checkbutton .train.f1.f1.cb2 -text "Use Learning Rate Adaptation" -variable adapt -anchor w
  pack .train.f1.f1.cb2 -side top -expand 1 -fill x
  checkbutton .train.f1.f1.cb3 -text "Shuffle Training Set" -variable shuffle -anchor w
  pack .train.f1.f1.cb3 -side top -expand 1 -fill x

  label .train.f1.f1.l6 -text "  "
  pack .train.f1.f1.l6 -side top -expand 1 -fill x

  label .train.f1.f1.l1 -text "Measured Error" -anchor w
  pack .train.f1.f1.l1 -side top -expand 1 -fill x

  radiobutton .train.f1.f1.rb1 -text "Mean Square Error" -variable errFct -value mse\
                             -anchor w
  radiobutton .train.f1.f1.rb2 -text "X-entropy (binom)" -variable errFct -value binom\
                             -anchor w
  radiobutton .train.f1.f1.rb3 -text "X-entropy (multi)" -variable errFct -value multi\
                             -anchor w
  radiobutton .train.f1.f1.rb4 -text "Classification Error" -variable errFct -value class\
                             -anchor w
  pack .train.f1.f1.rb1 .train.f1.f1.rb2 .train.f1.f1.rb3 .train.f1.f1.rb4\
            -side top -fill x -expand 1

  # listboxes
  # ---------
  frame .train.f1.f2.f1 -width 150
  frame .train.f1.f2.f2 -width 150
  pack .train.f1.f2.f1 .train.f1.f2.f2 -side top -fill x -expand 1

  label .train.f1.f2.f1.l1 -text "Net : " -fg red
  pack .train.f1.f2.f1.l1 -side left -fill x -expand 1
  label .train.f1.f2.f1.l2 -textvariable current_net -fg blue
  pack .train.f1.f2.f1.l2 -side right -fill x -expand 1

  listbox .train.f1.f2.f2.lb -height 21 -relief raised -borderwidth 2 \
          -yscrollcommand ".train.f1.f2.f2.scroll set"
  pack .train.f1.f2.f2.lb -side left -fill y -expand 1
  scrollbar .train.f1.f2.f2.scroll -command ".train.f1.f2.f2.lb yview"
  pack .train.f1.f2.f2.scroll -side right -fill y -expand 1

  bind .train.f1.f2.f2.lb <Double-Button-1> {
    set current_net [selection get]
  }
  set slist [lsort $ffnetlist]
  foreach i $slist {
    .train.f1.f2.f2.lb insert end $i
  }
  

  frame .train.f1.f3.f1 -width 150
  frame .train.f1.f3.f2 -width 150
  pack .train.f1.f3.f1 .train.f1.f3.f2 -side top -fill x -expand 1

  label .train.f1.f3.f1.l1 -text "Train-Input : " -fg red
  pack .train.f1.f3.f1.l1 -side left -fill x -expand 1
  label .train.f1.f3.f1.l2 -textvariable current_input -fg blue
  pack .train.f1.f3.f1.l2 -side right -fill x -expand 1

  listbox .train.f1.f3.f2.lb -height 10 -relief raised -borderwidth 2 \
          -yscrollcommand ".train.f1.f3.f2.scroll set"
  pack .train.f1.f3.f2.lb -side left -fill y -expand 1
  scrollbar .train.f1.f3.f2.scroll -command ".train.f1.f3.f2.lb yview"
  pack .train.f1.f3.f2.scroll -side right -fill y -expand 1

  bind .train.f1.f3.f2.lb <Double-Button-1> {
    set current_input [selection get]
  }
  set slist [lsort $fmatrixlist]
  foreach i $slist {
    .train.f1.f3.f2.lb insert end $i
  }

  frame .train.f1.f3.f3 -width 150
  frame .train.f1.f3.f4 -width 150
  pack .train.f1.f3.f3 .train.f1.f3.f4 -side top -fill x -expand 1

  label .train.f1.f3.f3.l1 -text "XVal-Input : " -fg red
  pack .train.f1.f3.f3.l1 -side left -fill x -expand 1
  label .train.f1.f3.f3.l2 -textvariable current_testinput -fg blue
  pack .train.f1.f3.f3.l2 -side right -fill x -expand 1

  listbox .train.f1.f3.f4.lb -height 10 -relief raised -borderwidth 2 \
          -yscrollcommand ".train.f1.f3.f4.scroll set"
  pack .train.f1.f3.f4.lb -side left -fill y -expand 1
  scrollbar .train.f1.f3.f4.scroll -command ".train.f1.f3.f4.lb yview"
  pack .train.f1.f3.f4.scroll -side right -fill y -expand 1

  bind .train.f1.f3.f4.lb <Double-Button-1> {
    set current_testinput [selection get]
  }
  set slist [lsort $fmatrixlist]
  foreach i $slist {
    .train.f1.f3.f4.lb insert end $i
  }


  frame .train.f1.f4.f1 -width 150
  frame .train.f1.f4.f2 -width 150
  pack .train.f1.f4.f1 .train.f1.f4.f2 -side top -fill x -expand 1

  label .train.f1.f4.f1.l1 -text "Train-Target : " -fg red
  pack .train.f1.f4.f1.l1 -side left -fill x -expand 1
  label .train.f1.f4.f1.l2 -textvariable current_target -fg blue
  pack .train.f1.f4.f1.l2 -side right -fill x -expand 1

  listbox .train.f1.f4.f2.lb -height 10 -relief raised -borderwidth 2 \
          -yscrollcommand ".train.f1.f4.f2.scroll set"
  pack .train.f1.f4.f2.lb -side left -fill y -expand 1
  scrollbar .train.f1.f4.f2.scroll -command ".train.f1.f4.f2.lb yview"
  pack .train.f1.f4.f2.scroll -side right -fill y -expand 1

  bind .train.f1.f4.f2.lb <Double-Button-1> {
    set current_target [selection get]
  }
  set slist [lsort $fmatrixlist]
  foreach i $slist {
    .train.f1.f4.f2.lb insert end $i
  }

  frame .train.f1.f4.f3 -width 150
  frame .train.f1.f4.f4 -width 150
  pack .train.f1.f4.f3 .train.f1.f4.f4 -side top -fill x -expand 1

  label .train.f1.f4.f3.l1 -text "XVal-Target : " -fg red
  pack .train.f1.f4.f3.l1 -side left -fill x -expand 1
  label .train.f1.f4.f3.l2 -textvariable current_testtarget -fg blue
  pack .train.f1.f4.f3.l2 -side right -fill x -expand 1

  listbox .train.f1.f4.f4.lb -height 10 -relief raised -borderwidth 2 \
          -yscrollcommand ".train.f1.f4.f4.scroll set"
  pack .train.f1.f4.f4.lb -side left -fill y -expand 1
  scrollbar .train.f1.f4.f4.scroll -command ".train.f1.f4.f4.lb yview"
  pack .train.f1.f4.f4.scroll -side right -fill y -expand 1

  bind .train.f1.f4.f4.lb <Double-Button-1> {
    set current_testtarget [selection get]
  }
  set slist [lsort $fmatrixlist]
  foreach i $slist {
    .train.f1.f4.f4.lb insert end $i
  }

  button .train.f2.b1 -text "Dismiss" -command "destroy .train"
  button .train.f2.b2 -text "Randomize" -command { randomize $current_net $random }

  button .train.f2.b3 -text "Train" -command { 
    train $current_net $current_input $current_target $current_testinput\
          $current_testtarget $iterN $batchN $eta $etaFac $gamma $moment\
          $adapt $shuffle $errFct $modu 
  }
  pack .train.f2.b1 .train.f2.b2 .train.f2.b3 -side left -fill x -expand 1
}



# -----------------------------------------------------------------------
#  show_update
# -----------------------------------------------------------------------
proc  show_update { widget fmat } {

  global   lcm mwc

  set      delta   15

  if {[string length [info commands $widget.f1]]} {
    destroy $widget.f1
    destroy $widget.f2
  }

  set m [$fmat configure -m]
  set n [$fmat configure -n]
  set xN [expr $m * $delta + 35]
  set yN [expr $n * $delta + 35]
  set minmax [$fmat minmax]
  set min [lindex $minmax 0]
  if {$min < 0} { set absmax [expr -1.0*$min] } else { set absmax $min }
  set max [lindex $minmax 1]
  if {$max < 0} { set max [expr -1.0*$max] }
  if {$max > $absmax} { set absmax $max }
  set min [expr -1.0*$absmax]
  set max $absmax

  if {$xN > 600} { set xxN 600 } else { set xxN $xN }
  if {$yN > 600} { set yyN 600 } else { set yyN $yN }

  frame $widget.f1
  frame $widget.f2
  pack $widget.f1 $widget.f2 -side top -fill x -expand 1

  frame $widget.f1.top
  frame $widget.f1.bot
  pack $widget.f1.top $widget.f1.bot -side top -fill x -expand 1

  canvas $widget.f1.top.c -width $xxN -height $yyN -scrollregion "0 0 $xN $yN"\
           -xscrollcommand "$widget.f1.bot.sb set"\
           -yscrollcommand "$widget.f1.top.sb set"
  pack $widget.f1.top.c -side left

  scrollbar $widget.f1.top.sb -orient vertical\
                -command "$widget.f1.top.c yview"
  pack $widget.f1.top.sb -side left -fill y -expand 1

  scrollbar $widget.f1.bot.sb -orient horizontal\
                -command "$widget.f1.top.c xview"
  pack $widget.f1.bot.sb -side left -fill x -expand 1

  set xN [expr $xN - 4]
  set yN [expr $yN - 4]
  $widget.f1.top.c create line 32 32 $xN 32 -fill black
  $widget.f1.top.c create line $xN 32 $xN $yN -fill black
  $widget.f1.top.c create line $xN $yN 32 $yN -fill black
  $widget.f1.top.c create line 32 $yN 32 32 -fill black
 
  for {set x 0} {$x < $m} {incr x} {
    if {[expr $x % 5] == 0} {
      set xm [expr $x*$delta + $delta/2 + 32]
      $widget.f1.top.c create text $xm 15 -text $x
      $widget.f1.top.c create line $xm 32 $xm 28 -fill black
    }
  }

  for {set y 0} {$y < $n} {incr y} {
    if {[expr $y % 5] == 0} {
      set ym [expr $y*$delta + $delta/2 + 32]
      $widget.f1.top.c create text 15 $ym -text $y
      $widget.f1.top.c create line 28 $ym 32 $ym -fill black
    }
  }

  for {set x 0} {$x < $m} {incr x} {
    for {set y 0} {$y < $n} {incr y} {

      set val [$fmat get $x $y]
      if {$val < 0.0} {set absval [expr -1.0*$val] } else { set absval $val }
      set col  $lcm([expr int(50.0*$val / $absmax)])
      set size [expr int(($delta/2 - 2) * $absval / $absmax)]
      set d2 [expr $size + 1]

      set xm [expr $x*$delta + $delta/2 + 32]
      set ym [expr $y*$delta + $delta/2 + 32]
      set x1 [expr $xm-$d2]
      set y1 [expr $ym-$d2]
      set x2 [expr $xm+$d2]
      set y2 [expr $ym+$d2]

      $widget.f1.top.c create rectangle $x1 $y1 $x2 $y2 -outline $col\
                                                     -fill $col
    }
  }
  pack $widget.f1.top.c -side left

  label $widget.f2.l1 -text "$min"
  canvas $widget.f2.c -width 101 -height 20
  for {set k -50} {$k <= 50} {incr k} {
    set x [expr $k + 50]
    if {$k < 0} { set absval [expr -1.0*$k.0] } else { set absval $k.0 }
    set size [expr int(($delta/2 - 2) * $absval / 50.0)]
    set d2 [expr $size + 1]
    $widget.f2.c create rectangle $x [expr 10-$d2] $x\
                              [expr 10+$d2] -outline $lcm($k)
  }

  label $widget.f2.l2 -text "$max"
  pack $widget.f2.l1 $widget.f2.c $widget.f2.l2 -side left

}


# -----------------------------------------------------------------------
#  show_fmatrix
# -----------------------------------------------------------------------
proc  show_fmatrix { fmat } {

  global   lcm mwc

  if {[string length [info commands .fm$mwc]]} { 
    puts "There already is an FMatrix named $fmat\a\a"
    return
  }

  toplevel .fm$mwc
  wm title .fm$mwc "FMatrix: $fmat"

  frame .fm$mwc.f3
  pack .fm$mwc.f3 -side bottom -fill x -expand 1

  show_update .fm$mwc $fmat

  button .fm$mwc.f3.close -text "Close" -command "destroy .fm$mwc"
  button .fm$mwc.f3.update -text "Update" -command "show_update .fm$mwc $fmat"
  pack .fm$mwc.f3.close .fm$mwc.f3.update -side left

  incr mwc
}



# -----------------------------------------------------------------------
#  test_it                                                            
# ----------------------------------------------------------------------- 
proc test_it {} {

  global       cccurrent_net cccurrent_input cccurrent_target\
               cccurrent_output errFct testError

  if {[string length $cccurrent_net] == 0} {
    puts stderr "ERROR: No network specified!"
    return
  }

  if {[string length $cccurrent_input] == 0} {
    puts stderr "ERROR: No input matrix specified!"
    return
  }

  if {[string length $cccurrent_output] > 0} {

    if {[string length $cccurrent_target] > 0} {
      set testError [$cccurrent_net eval $cccurrent_input $cccurrent_output\
       	           -target $cccurrent_target -errFct $errFct]
    } else {
      set testError [$cccurrent_net eval $cccurrent_input $cccurrent_output\
        	           -errFct $errFct]
    }

  } else {

    if {[string length $cccurrent_target] > 0} {
      set testError [$cccurrent_net eval $cccurrent_input dummy_output\
       	           -target $cccurrent_target -errFct $errFct]
    } else {
      set testError [$cccurrent_net eval $cccurrent_input dummy_output\
        	           -errFct $errFct]
    }

  }

  return
}



# -----------------------------------------------------------------------
#  test_menu                                                            
# ----------------------------------------------------------------------- 
proc test_menu {} {

  global    errFct testError cccurrent_net cccurrent_target cccurrent_input\
            cccurrent_output fmatrixlist ffnetlist

  set errFct mse
  set testError 0.0
  set cccurrent_net ""
  set cccurrent_target ""
  set cccurrent_input ""
  set cccurrent_output ""

  if {[string length [info commands .test]]} { 
    puts "Already open\a\a"
    return
  }
  toplevel .test
  wm title .test "Test"

  frame .test.f1
  frame .test.f2
  pack .test.f1 .test.f2 -side top -fill x -expand 1

  frame .test.f1.f1 -relief raised -bd 2
  frame .test.f1.f2
  frame .test.f1.f3
  frame .test.f1.f4
  frame .test.f1.f5
  pack  .test.f1.f1 .test.f1.f2 .test.f1.f3 .test.f1.f4 .test.f1.f5\
        -side left -fill x -expand 1

  label  .test.f1.f1.l1 -text "ErrorFunction:" -fg red
  pack .test.f1.f1.l1 -side top -fill x -expand 1

  radiobutton .test.f1.f1.rb1 -text "Mean Square Error" -variable errFct -value mse\
                             -anchor w
  radiobutton .test.f1.f1.rb2 -text "X-entropy (binom)" -variable errFct -value binom\
                             -anchor w
  radiobutton .test.f1.f1.rb3 -text "X-entropy (multi)" -variable errFct -value multi\
                             -anchor w
  radiobutton .test.f1.f1.rb4 -text "Classification Error" -variable errFct -value class\
                             -anchor w
  label .test.f1.f1.l0 -text "             " 
  label .test.f1.f1.l2 -text " Test Error: " -fg red
  label .test.f1.f1.l3 -textvariable testError -relief sunken -bd 2 -fg blue
  pack .test.f1.f1.rb1 .test.f1.f1.rb2 .test.f1.f1.rb3\
                   .test.f1.f1.rb4 .test.f1.f1.l0 .test.f1.f1.l2\
                   .test.f1.f1.l3 -side top -fill x -expand 1

  frame .test.f1.f2.f1 -width 150
  frame .test.f1.f2.f2 -width 150
  pack .test.f1.f2.f1 .test.f1.f2.f2 -side top -fill x -expand 1

  label .test.f1.f2.f1.l1 -text "Net : " -fg red
  pack .test.f1.f2.f1.l1 -side left -fill x -expand 1
  label .test.f1.f2.f1.l2 -textvariable cccurrent_net -fg blue
  pack .test.f1.f2.f1.l2 -side right -fill x -expand 1

  listbox .test.f1.f2.f2.lb -height 9 -relief raised -borderwidth 2 \
          -yscrollcommand ".test.f1.f2.f2.scroll set"
  pack .test.f1.f2.f2.lb -side left
  scrollbar .test.f1.f2.f2.scroll -command ".test.f1.f2.f2.lb yview"
  pack .test.f1.f2.f2.scroll -side right -fill y

  bind .test.f1.f2.f2.lb <Double-Button-1> {
    set cccurrent_net [selection get]
  }
  set slist [lsort $ffnetlist]
  foreach i $slist {
    .test.f1.f2.f2.lb insert end $i
  }
  

  frame .test.f1.f3.f1 -width 150
  frame .test.f1.f3.f2 -width 150
  pack .test.f1.f3.f1 .test.f1.f3.f2 -side top -fill x -expand 1

  label .test.f1.f3.f1.l1 -text "Test-Input : " -fg red
  pack .test.f1.f3.f1.l1 -side left -fill x -expand 1
  label .test.f1.f3.f1.l2 -textvariable cccurrent_input -fg blue
  pack .test.f1.f3.f1.l2 -side right -fill x -expand 1

  listbox .test.f1.f3.f2.lb -height 9 -relief raised -borderwidth 2 \
          -yscrollcommand ".test.f1.f3.f2.scroll set"
  pack .test.f1.f3.f2.lb -side left
  scrollbar .test.f1.f3.f2.scroll -command ".test.f1.f3.f2.lb yview"
  pack .test.f1.f3.f2.scroll -side right -fill y

  bind .test.f1.f3.f2.lb <Double-Button-1> {
    set cccurrent_input [selection get]
  }
  set slist [lsort $fmatrixlist]
  foreach i $slist {
    .test.f1.f3.f2.lb insert end $i
  }

  frame .test.f1.f4.f1 -width 150
  frame .test.f1.f4.f2 -width 150
  pack .test.f1.f4.f1 .test.f1.f4.f2 -side top -fill x -expand 1

  label .test.f1.f4.f1.l1 -text "Test-Output : " -fg red
  pack .test.f1.f4.f1.l1 -side left -fill x -expand 1
  label .test.f1.f4.f1.l2 -textvariable cccurrent_output -fg blue
  pack .test.f1.f4.f1.l2 -side right -fill x -expand 1

  listbox .test.f1.f4.f2.lb -height 9 -relief raised -borderwidth 2 \
          -yscrollcommand ".test.f1.f4.f2.scroll set"
  pack .test.f1.f4.f2.lb -side left
  scrollbar .test.f1.f4.f2.scroll -command ".test.f1.f4.f2.lb yview"
  pack .test.f1.f4.f2.scroll -side right -fill y

  bind .test.f1.f4.f2.lb <Double-Button-1> {
    set cccurrent_output [selection get]
  }
  set slist [lsort $fmatrixlist]
  foreach i $slist {
    .test.f1.f4.f2.lb insert end $i
  }

  frame .test.f1.f5.f1 -width 150
  frame .test.f1.f5.f2 -width 150
  pack .test.f1.f5.f1 .test.f1.f5.f2 -side top -fill x -expand 1

  label .test.f1.f5.f1.l1 -text "Test-Target : " -fg red
  pack .test.f1.f5.f1.l1 -side left -fill x -expand 1
  label .test.f1.f5.f1.l2 -textvariable cccurrent_target -fg blue
  pack .test.f1.f5.f1.l2 -side right -fill x -expand 1

  listbox .test.f1.f5.f2.lb -height 9 -relief raised -borderwidth 2 \
          -yscrollcommand ".test.f1.f5.f2.scroll set"
  pack .test.f1.f5.f2.lb -side left
  scrollbar .test.f1.f5.f2.scroll -command ".test.f1.f5.f2.lb yview"
  pack .test.f1.f5.f2.scroll -side right -fill y

  bind .test.f1.f5.f2.lb <Double-Button-1> {
    set cccurrent_target [selection get]
  }
  set slist [lsort $fmatrixlist]
  foreach i $slist {
    .test.f1.f5.f2.lb insert end $i
  }  

  button .test.f2.b1 -text "Dismiss" -command "destroy .test"
  button .test.f2.b2 -text "Test" -command "test_it"
  pack .test.f2.b1 .test.f2.b2 -side left -fill none
}



# -----------------------------------------------------------------------
#  analyse_menu                                                            
# ----------------------------------------------------------------------- 
proc analyse_menu {} {

  global         ccurrent_net ccurrent_matrix ffnetlist fmatrixlist\
                 ccurrent_netmatrix netmatrixlist

  set ccurrent_net ""
  set ccurrent_matrix ""
  set ccurrent_netmatrix ""
  set netmatrixlist ""

  if {[string length [info commands .analyse]]} { 
    puts "Already open\a\a"
    return
  }
  toplevel .analyse
  wm title .analyse "Analyse"

  frame .analyse.f1
  frame .analyse.f2
  pack .analyse.f1 .analyse.f2 -side top -fill x -expand 1

  frame .analyse.f1.f1 -width 200
  frame .analyse.f1.f2 -width 200
  frame .analyse.f1.f3 -width 200
  pack  .analyse.f1.f1 .analyse.f1.f2 .analyse.f1.f3\
        -side left -fill x -expand 1

  # Listboxes 
  frame .analyse.f1.f1.f1 -width 150
  frame .analyse.f1.f1.f2 -width 150
  pack .analyse.f1.f1.f1 .analyse.f1.f1.f2 -side top -fill x -expand 1

  label .analyse.f1.f1.f1.l1 -text "Net : " -fg red
  pack .analyse.f1.f1.f1.l1 -side left -fill x -expand 1
  label .analyse.f1.f1.f1.l2 -textvariable ccurrent_net -fg blue
  pack .analyse.f1.f1.f1.l2 -side right -fill x -expand 1

  listbox .analyse.f1.f1.f2.lb -height 21 -relief raised -borderwidth 2 \
          -yscrollcommand ".analyse.f1.f1.f2.scroll set"
  pack .analyse.f1.f1.f2.lb -side left
  scrollbar .analyse.f1.f1.f2.scroll -command ".analyse.f1.f1.f2.lb yview"
  pack .analyse.f1.f1.f2.scroll -side right -fill y

  bind .analyse.f1.f1.f2.lb <Double-Button-1> {

    set ccurrent_net [selection get]
    set netmatrixlist ""
    while {[.analyse.f1.f2.f2.lb size] > 0} {
      .analyse.f1.f2.f2.lb  delete  0
    }

    set layerN [$ccurrent_net configure -layerN]
    for {set layerX 0} {$layerX < $layerN} { incr layerX} {
      set linkN [$ccurrent_net.layer($layerX) configure -linkN]
      lappend netmatrixlist "layer($layerX).act"
      for {set linkX 0} {$linkX < $linkN} {incr linkX} {
        lappend netmatrixlist "layer($layerX).link($linkX).W"
      }
    }

    set slist [lsort $netmatrixlist]
    foreach i $slist {
      .analyse.f1.f2.f2.lb insert end $i
    }

  }
  set slist [lsort $ffnetlist]
  foreach i $slist {
    .analyse.f1.f1.f2.lb insert end $i
  }

  frame .analyse.f1.f2.f1 -width 150
  frame .analyse.f1.f2.f2 -width 150
  pack .analyse.f1.f2.f1 .analyse.f1.f2.f2 -side top -fill x -expand 1

  label .analyse.f1.f2.f1.l1 -textvariable ccurrent_net -fg red
  pack .analyse.f1.f2.f1.l1 -side left -fill x -expand 1
  label .analyse.f1.f2.f1.l2 -text ":" -fg red
  pack .analyse.f1.f2.f1.l2 -side left -fill x -expand 1
  label .analyse.f1.f2.f1.l3 -textvariable ccurrent_netmatrix -fg blue
  pack .analyse.f1.f2.f1.l3 -side right -fill x -expand 1

  listbox .analyse.f1.f2.f2.lb -height 21 -relief raised -borderwidth 2 \
          -yscrollcommand ".analyse.f1.f2.f2.scroll set"
  pack .analyse.f1.f2.f2.lb -side left
  scrollbar .analyse.f1.f2.f2.scroll -command ".analyse.f1.f2.f2.lb yview"
  pack .analyse.f1.f2.f2.scroll -side right -fill y

  bind .analyse.f1.f2.f2.lb <Double-Button-1> {
    set ccurrent_netmatrix [selection get]
    set nmatrix "$ccurrent_net.$ccurrent_netmatrix"
    set m [$nmatrix configure -m]
    set n [$nmatrix configure -n]
    update idletasks
    if {[expr $m * $n] > 0} {
      show_fmatrix $nmatrix
    } else {
      puts stderr "ERROR: FMatrix $nmatrix is empty"
    }
  }
  set slist [lsort $netmatrixlist]
  foreach i $slist {
    .analyse.f1.f2.f2.lb insert end $i
  }

  frame .analyse.f1.f3.f1 -width 150
  frame .analyse.f1.f3.f2 -width 150
  pack .analyse.f1.f3.f1 .analyse.f1.f3.f2 -side top -fill x -expand 1

  label .analyse.f1.f3.f1.l1 -text "FMatrix : " -fg red
  pack .analyse.f1.f3.f1.l1 -side left -fill x -expand 1
  label .analyse.f1.f3.f1.l2 -textvariable ccurrent_matrix -fg blue
  pack .analyse.f1.f3.f1.l2 -side right -fill x -expand 1

  listbox .analyse.f1.f3.f2.lb -height 21 -relief raised -borderwidth 2 \
          -yscrollcommand ".analyse.f1.f3.f2.scroll set"
  pack .analyse.f1.f3.f2.lb -side left
  scrollbar .analyse.f1.f3.f2.scroll -command ".analyse.f1.f3.f2.lb yview"
  pack .analyse.f1.f3.f2.scroll -side right -fill y

  bind .analyse.f1.f3.f2.lb <Double-Button-1> {
    set ccurrent_matrix [selection get]
    set m [$ccurrent_matrix configure -m]
    set n [$ccurrent_matrix configure -n]
    update idletasks
    if {[expr $m * $n] > 0} {
      show_fmatrix $ccurrent_matrix
    } else {
      puts stderr "ERROR: FMatrix $ccurrent_matrix is empty"
    }
  }
  set slist [lsort $fmatrixlist]
  foreach i $slist {
    .analyse.f1.f3.f2.lb insert end $i
  }

  # Buttons
  button .analyse.f2.b1 -text "Dismiss" -command "destroy .analyse"
  pack .analyse.f2.b1 -side left -fill none
}



# -------------------------
#  translate into hex code 
# -------------------------
proc hex { val } {

  if {$val < 0} { return "00" }
  set upper [expr $val / 16]
  set lower [expr $val % 16]
  set hexNum "0 1 2 3 4 5 6 7 8 9 A B C D E F"
  return "[lindex $hexNum $upper][lindex $hexNum $lower]"
}



# ------------------------
#  create local color map
# ------------------------
for {set k 0} {$k <= 50} {incr k} {
  set lcm(-$k) "#0000[hex [expr 50+4*$k]]"
  set lcm($k)  "#[hex [expr 50+4*$k]]0000"
}



# -----------------------------------------------------------------------
#  JRTk-Net
# ----------------------------------------------------------------------- 
proc JRTk-Net {} {

  global   ffnetlist fmatrixlist cwd current_type current_net\
           global_logN chuwi mwc itemList env

  set current_net   ""
  set ffnetlist     ""
  set fmatrixlist   "input output target"
  set cwd           [exec pwd]
  set current_type  FFNet
  set chuwi         0
  set home          "."
  set mwc           0
  set itemList      ""

  
  catch { set home $env(JRTK_NET_HOME) }
  set title_pic "$home/JRTk-NetTitle.gif"
  if {![file exists $title_pic]} {
    puts stderr "Couldn't find $title_pic\nSet your JRTK_NET_HOME environment variable appropriately"
    return
  }

  if { [llength [info command input]] == 0 }  { 
    FMatrix input
    FMatrix output
    FMatrix target
    FMatrix dummy_output
  }

  set global_logN 0

  toplevel .main
  wm title .main "JRTk-Net"
  canvas .main.title -width 416 -height 200
  image create photo title\
        -file $title_pic
  .main.title create image 208 101 -image title
  frame .main.f1 -width 416 -height 30
  pack .main.title .main.f1 -side top -fill x -expand 1
  
  button .main.f1.quit -text "Quit" -command quit_menu
  button .main.f1.file -text "File" -command file_menu
  button .main.f1.edit -text "Edit" -command edit_menu
  button .main.f1.train -text "Train" -command train_menu
  button .main.f1.test  -text "Test" -command test_menu
  button .main.f1.analyse -text "Analyse" -command analyse_menu
  pack .main.f1.quit .main.f1.file .main.f1.edit .main.f1.train\
       .main.f1.test .main.f1.analyse\
       -side left -fill x -expand 1

  wm withdraw .
}


