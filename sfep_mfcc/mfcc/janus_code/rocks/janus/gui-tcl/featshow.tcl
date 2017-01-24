#  ========================================================================
#   JANUS      Speech- to Speech Translation
#              ------------------------------------------------------------
#
#   Author  :  Martin Westphal
#   Email   :  westphal@ira.uka.de
#   Script  :  featshow.tcl
#   Date    :  17.07.95
#
#  ========================================================================
#  
#   Remarks :  The user relevant  procedures in this file are
#              featshow, showFrame
#
#   See USAGE for details
#  
#  ========================================================================
#
#   $Log$
#   Revision 1.4  2007/02/23 10:25:35  fuegen
#   added support for portaudio devices (from Florian Metze/ Friedrich Faubel)
#
#   Revision 1.3  2003/08/14 11:19:40  fuegen
#   Merged changes on branch jtk-01-01-15-fms (jaguar -> ibis-013)
#
#   Revision 1.2.2.5  2002/11/15 13:00:27  metze
#   Added check to avoid recursion in setScale
#
#   Revision 1.2.2.4  2002/11/04 15:02:07  metze
#   Added documentation code
#
#   Revision 1.2.2.3  2001/10/05 08:00:10  metze
#   More output removed
#
#   Revision 1.2.2.2  2001/10/05 07:51:29  metze
#   Removed annoying wxh messages
#
#   Revision 1.2.2.1  2001/10/05 07:28:30  metze
#   Added button to re-read features as re-reading via <ENTER>
#   inhibits selection of scrolled-out items
#
#   Revision 1.2  2001/01/11 12:33:16  janus
#   Updated ReleaseLog
#
#   Revision 1.1.34.1  2000/12/10 14:06:09  janus
#   WAV support, ngets, multivar
#
#   Revision 1.1  2000/08/23 11:00:01  jmcd
#   Added janus files from gui-tcl.
#
# Revision 1.5  1999/01/22  09:43:39  westphal
# automatic audio device detection
#
#   Revision 1.3  1997/07/28 16:01:13  westphal
#   *** empty log message ***
#
#   Revision 1.2  1997/07/28 15:46:01  westphal
#   don't source lib tcl files
#
#   Revision 1.1  1997/07/28 15:39:18  westphal
#   Initial revision
#
# Revision 1.25  1996/12/03  20:21:44  westphal
# return value in refreshFeature
#
# Revision 1.24  1996/10/11  17:08:20  westphal
# movable grey bar
#
# Revision 1.23  1996/10/11  14:48:08  westphal
# autoresize and labelbar
#
# Revision 1.22  1996/10/04  11:09:09  westphal
# showFrame version 2
#
# Revision 1.21  1996/08/28  15:06:49  westphal
# feature select with single press
#
# Revision 1.20  1996/08/28  12:11:39  westphal
# DEVICE SUN
#
# Revision 1.19  1996/05/29  15:03:18  maier
# cut marked region
#
# Revision 1.18  1996/03/28  12:45:07  maier
# chmod
#
# Revision 1.17  1996/03/14  14:28:22  maier
# samplRate in float
#
# Revision 1.16  1996/02/07  12:36:59  maier
# *** empty log message ***
#
# Revision 1.15  1996/01/15  14:44:20  maier
# device gain
#
# Revision 1.14  1996/01/15  13:30:36  maier
# multiple features also for FMatrix, ps files
#
# Revision 1.13  1996/01/15  10:44:16  maier
# minmax
#
# Revision 1.12  1996/01/03  13:47:30  maier
# *** empty log message ***
#
# Revision 1.11  1995/11/17  13:42:13  maier
# mergeLabel
#
# Revision 1.10  1995/11/15  14:33:09  maier
# more label stuff
#
# Revision 1.9  1995/11/06  13:20:53  maier
# label stuff
#
# Revision 1.8  1995/10/30  17:33:16  maier
# labels
#
# Revision 1.7  1995/10/27  11:34:50  maier
# changed name of parameter variable, added user device, mark window and scroll buttons
#
# Revision 1.6  1995/10/17  11:12:16  maier
# used $JANUSLIB
#
# Revision 1.5  1995/08/24  16:53:25  maier
# change from ADC <-> FEAT => same range displayed, label from and to in sec if ruler in sec
#
# Revision 1.4  1995/08/23  17:31:15  maier
# mark all
#
# Revision 1.3  1995/08/23  09:43:31  maier
# mode instead of grey
#
# Revision 1.2  1995/08/23  07:26:33  maier
# if shift == 0 take FS->shift
#
# Revision 1.1  1995/08/22  08:57:01  maier
# Initial revision
#
# 
#  ========================================================================


proc featshow_Info { name } {
    switch $name {
	procs {
	    return "featshow"
	}
	help {
	    return "These functions allow you to display features."
	}
	featshow {
	    return "Shows a feature, USAGE: featshow <featureset> <feature> \[<width> \[<height>\]."
	}
    }
    return "???"
}


########################################################################### 
# Set JANUSLIB and the audio device play commands
########################################################################### 
set fsN 0

if {![info exists JANUSLIB]} {
  set JANUSLIB [file dirname [info script]]
  puts stderr "JANUSLIB does not exist, set it to $JANUSLIB"
}

########################################################################### 

if ![info exists DEVICE] {
    if {[regexp paInfo [FeatureSet help]]} {
	set DEVICE PortAudio
    } else {
	set DEVICE $tcl_platform(os)
    }
}
if ![info exists DEVICEGAIN] {set DEVICEGAIN 1}
set DEVICEHELP {\
Give a Tcl command using your audio play program.
Use the variables $filename $gain $Rate [in Hz] or $rate [in kHz].}

if {![info exists DEVICEPLAY($tcl_platform(os))]} {
    set DEVICEPLAY(ALPHA) {
	exec cat $filename | aconvert -,r=$rate,t=linear -,r=8 | aplay -g $gaindb -f &
    }
    set DEVICEPLAY(HP) {
	exec cat $filename | /usr/local/bin/splayer -l16 -prate  $Rate &
    }
    set DEVICEPLAY(SunOS) {
	exec audioconvert -i channels=1,rate=$Rate,encoding=pcm $filename | audioplay -v $Gain
    }
    set DEVICEPLAY(Gradient) {
	exec cat $filename | /tools/desklab/bin/gplay  -s $Rate -z &
    }
    set DEVICEPLAY(Linux) {
	#exec splay -s $Rate -b 16 $filename &
	exec sox -q -t raw -r $Rate -s -w $filename -t ossdsp -s -w /dev/dsp
    }
    set DEVICEPLAY(PortAudio) {
      # does nothing here
    }
    if ![info exists DEVICEPLAY(User)] {
	set DEVICEPLAY(User) {puts "no user device set"}
    }
}

###########################################################################
# DrawRuler
#
#   (0,0) __________________________(width,0)
#   |    |    |    |    |    |    |    |    |
#
# start                                    end
#
#   allow a height of 40 pixels, width of $width pixels
###########################################################################
proc DrawRuler {ruler width start end} {
    global tk_library rulerfont

    set dxmin 30.
    set height_scale 10
    set height_number [expr $height_scale + 5]

    set range [expr $end - $start]
    set smax [expr $width / $dxmin]
    set delta 1.


    while {[expr $range / $delta] < $smax} {set delta [expr $delta / 10.]}
    while {[expr $range / $delta] > $smax} {set delta [expr $delta * 10.]}
    $ruler delete all
    $ruler create line 0 0  [expr $width-1] 0 -width 1
    $ruler create line [expr $width-1] 0 [expr $width + 9] 0 \
                       -width 1 \
                       -arrow last

    set i [expr ceil($start/$delta)]
    if {$delta >= 1} {
       set delta [expr int($delta)]
       set i     [expr int($i)]
    }
    while {[expr $i*$delta]  < $end} {
         set x [expr round($width*($i*$delta-$start)/$range)]
         $ruler create line ${x} 0 ${x} $height_scale  -width 1
         $ruler create text ${x} $height_number -text [expr $i*$delta] \
                -anchor n \
                -font "-Adobe-times-medium-r-normal--*-120*"
         set i [expr $i+1]
    }
}
############################################
# showFrame
#
# prints a single frame of a feature
# (this procedure is not needed here)
# Oct'96: version 2 can be much faster! 
############################################
#puts "defined procedure showFrame:"
#puts "USAGE: showFrame <featureset> <feature> <frame>"
#puts ""
#proc showFrame {fs feature frame} {
#   set out [$fs : $feature . data]
#   puts [lindex [lindex $out 0] $frame]
#}
proc showFrame {fs fe from {to {}}} { 
    if {"$to" == ""} {set to $from}
    $fs cut _tmp_ $fe $from $to
    set ret [${fs}:_tmp_.data]
    $fs delete _tmp_
    return $ret
}

############################################
# getFeatParam
#
# 
############################################
proc getFeatParam {fsN} {
    global fsPar

    if {$fsPar($fsN,dx) < 1} {set fsPar($fsN,dx) 1}
    set fe $fsPar($fsN,fe)

    # --- type, rate  ---
    if {$fe == ""} {
       set fsPar($fsN,rate)  [$fsPar($fsN,fes) configure -samplingRate]
       return ""
    }
    set type  [$fsPar($fsN,fes) : $fe . data type]
    set fsPar($fsN,rate)  [$fsPar($fsN,fes) : $fe configure -samplingRate]

    if {$type == "FMatrix"} {
       set fsPar($fsN,frameN) [$fsPar($fsN,fes) : $fe configure -frameN ]
       set fsPar($fsN,coeffN) [$fsPar($fsN,fes) : $fe configure -coeffN]
       set shift  [$fsPar($fsN,fes) : $fe configure -shift]
       if {$shift == 0} {
             set fsPar($fsN,shift)  [$fsPar($fsN,fes) configure -frameShift]
       } else {set fsPar($fsN,shift) $shift}
       set fsPar($fsN,totalSec) \
             [expr 0.001 * $fsPar($fsN,frameN) * $fsPar($fsN,shift)]
    } else {
       if {$type == "SVector"} {
          set fsPar($fsN,frameN) [$fsPar($fsN,fes) : $fe configure -sampleN ]
          set fsPar($fsN,shift)  [$fsPar($fsN,fes) configure -frameShift]
          set fsPar($fsN,totalSec) \
                [expr 0.001 * $fsPar($fsN,frameN) / $fsPar($fsN,rate)]
       }
    }
    return $type
}


####################################################
# convert from
#   timePixel:     pixel -> time
#   pixelTime:     time -> pixel
#   timeFrom:      time -> sample (for ADCs)
#                  time -> frame  (for features)
#   fromTime:      from -> time
####################################################
proc timePixel {fsN pixel} {
   global fsPar

   set time 0
   set type  [$fsPar($fsN,fes) : $fsPar($fsN,fe) . data type]
   if {$type == "FMatrix"} {
     set frame  \
           [expr int($fsPar($fsN,from) + $pixel / $fsPar($fsN,dx))]
     set time  [expr 0.001 * $fsPar($fsN,shift) * $frame ]
   } else {
     if {$type == "SVector"} {
       set sample \
            [expr int($fsPar($fsN,from) + $pixel * $fsPar($fsN,step))]
       set time [expr 0.001 * $sample / $fsPar($fsN,rate)]
     }
   }
   return $time
}

proc pixelTime {fsN time} {
    global fsPar

    set pixel 0
    set type  [$fsPar($fsN,fes) : $fsPar($fsN,fe) . data type]
    if {$type == "FMatrix"} {
	set frame      [expr int(1000.0 * $time / $fsPar($fsN,shift))]
	set pixel      [expr $fsPar($fsN,dx) * ($frame - $fsPar($fsN,from))]
    } else {
	if {$type == "SVector"} {
	    set sample [expr int(1000.0 * $time * $fsPar($fsN,rate))]
	    set pixel  [expr ($sample - $fsPar($fsN,from)) / $fsPar($fsN,step)]
	}
    }
    return $pixel
}

proc timeFrom {fsN from} {
   global fsPar

   set time 0
   set type  [$fsPar($fsN,fes) : $fsPar($fsN,fe) . data type]
   if {$type == "FMatrix"} {
     set time  [expr 0.001 * $fsPar($fsN,shift) * $from ]
   } else {
     if {$type == "SVector"} {
       set time [expr 0.001 * $from / $fsPar($fsN,rate)]
     }
   }
   return $time
}

proc fromTime {fsN time} {
   global fsPar

   set pixel 0
   set type  [$fsPar($fsN,fes) : $fsPar($fsN,fe) . data type]
   if {$type == "FMatrix"} {
     set from  [expr int(1000.0 * $time / $fsPar($fsN,shift))]
   } else {
     if {$type == "SVector"} {
       set from [expr int(1000.0 * $time * $fsPar($fsN,rate))]
      }
   }
   return $from
}

###################################
#   setDeviceGain
###################################
proc setDEVICEGAIN {val} {
   global DEVICEGAIN 
   set DEVICEGAIN [expr $val / 100.0]
} 
proc setDeviceGain {} {
  global DEVICEGAIN DGPERCENT

  set DGPERCENT [expr 100 * $DEVICEGAIN]
  set f [Toplevel .sdg ]
  wm title $f "Set audio device gain"
  set t  [Frame $f.t -relief groove -borderwidth 2]
  set bt [Frame $f.bt -relief groove -borderwidth 2]
  pack $t $bt -side top -fill x

  Scale $t.s -orient horizontal -label "%" \
             -length 200 -from 1 -to 1000 \
             -command "setDEVICEGAIN"
  $t.s set $DGPERCENT
  pack $t.s -fill x 

  Button $bt.ok -text "reset" -command "$t.s set 100"
  Button $bt.ca -text "close" -command "destroy $f"
  pack   $bt.ok $bt.ca -padx 5 -pady 5 -side left
  bind   $f <Return> "destroy $f"
}

###################################
#   setUserDevice
###################################
proc setUserDevice {} {
  global DEVICEPLAY _restore_ DEVICEHELP

  set _restore_ $DEVICEPLAY(User)
  set f [Toplevel .sud ]
  wm title $f "Set user device"
  set t [Frame $f.t -relief groove -borderwidth 2]
  set e [Frame $f.e -relief groove -borderwidth 2]
  set bt [Frame $f.bt -relief groove -borderwidth 2]
  pack $t $e $bt -side top -fill x

  Message  $t.l1 -aspect 700 -pady 10 \
           -font "-Adobe-times-medium-r-normal--*-120*" \
           -text "$DEVICEHELP"
  Label $t.l2 -text "Example:  exec audioplay -f \$filename -sr \$rate"
  Label $e.l1 -anchor w -text "command: " -pady 5
  Entry $e.e1 -relief flat -width 40 -textvariable DEVICEPLAY(User)
  pack $t.l1 $t.l2 -side top -fill x 
  pack $e.l1 $e.e1 -side left

  Button $bt.ok -text "ok"     -command "destroy $f"
  Button $bt.ca -text "cancel" -command "set DEVICEPLAY(User) \$_restore_ ; destroy $f"
  pack   $bt.ok $bt.ca -padx 5 -pady 5 -side left
  bind   $f <Return> "destroy $f"
  tkwait window $f

}

############################################
# Play
# rate in kHz
# from and to in seconds
# 
############################################
proc playADC {fs feature from to} {
  global DEVICE DEVICEPLAY DEVICEGAIN

  set filename /tmp/fsfile.adc
  set Rate  [expr int(1000 * [$fs : $feature  configure -samplingRate])]
  set rate  [expr $Rate / 1000] 
  set gain  $DEVICEGAIN
  set Gain  [expr int(100 * $DEVICEGAIN)]
  if {$gain > 0} {set gaindb [expr 10 * log($gain)]} else {set gaindb 0}

  if {$DEVICE=="PortAudio"} {
    $fs paPlay $feature \
                 -from [expr int($Rate * $from)] \
                 -to   [expr int($Rate * $to)]
  } else {
    $fs writeADC $feature $filename \
                 -from [expr int($Rate * $from)] \
                 -to   [expr int($Rate * $to)]
    exec chmod a+w $filename
    if {[info exists DEVICEPLAY($DEVICE)]} {
        eval "$DEVICEPLAY($DEVICE)" 
    } else   {puts "Don't know device $DEVICE"}
  }
}

proc play {fsN} {
  global fsPar

  playADC $fsPar($fsN,fes) $fsPar($fsN,fe) $fsPar($fsN,mfrom) $fsPar($fsN,mto)
}

############################################
# Marker
#
# 
############################################
proc plotMarker {fsN} {
   global fsPar

   set height  $fsPar($fsN,height)
   catch {.fw$fsN.f3.c delete marker}
   
   set mto    $fsPar($fsN,mto)
   set mfrom  $fsPar($fsN,mfrom)
   set total  $fsPar($fsN,totalSec)

   if {$mto == -1  ||  $mto > $total}  {set mto $total}
   if {$mfrom > $total}  {set mfrom $total}
   if {$mfrom < 0} {set mfrom 0}
   if {$mto   < 0} {set mto   0}
   if {$mfrom > $mto} {
      set help  $mfrom
      set mfrom $mto
      set mto   $help
   }
   set x1 [pixelTime $fsN $mto  ]
   set x2 [pixelTime $fsN $mfrom]
   set fsPar($fsN,mto)   $mto
   set fsPar($fsN,mfrom) $mfrom
   .fw$fsN.f3.c create line $x1 0 $x1 $height  -width 1 -fill blue -tag marker
   .fw$fsN.f3.c create line $x2 0 $x2 $height  -width 1 -fill blue -tag marker
   .fw$fsN.f3.c lower [.fw$fsN.f3.c create rectangle $x1 0 $x2 $height \
                       -fill LightBlue1 -tag marker]
   .fw$fsN.f3.c lower bg
   Scale2 $fsN
}

proc DoB1 {fsN x} {
   global fsPar
   set fsPar($fsN,mfrom) [timePixel $fsN $x]
   plotMarker $fsN
}

proc DoB3 {fsN x} {
   global fsPar
   set fsPar($fsN,mto) [timePixel $fsN $x]
   plotMarker $fsN
}

#####################################################
# procedure for scrollbar 
# doesn't work properly, don't know why yet
# the arrows and troughs don't stop anymore
#####################################################
proc fsScroll {fsN args} {
  global fsPar

 set frameN [expr $fsPar($fsN,frameN) - 1 ] 
 set step   $fsPar($fsN,step)
 set from   $fsPar($fsN,from)
 
  set command [lindex $args 0]
  if {$command == "moveto"} {
     set value [lindex $args 1]
     set from [expr int($value * $frameN)]
  } else {
    if {$command == "scroll"} {
       set value [lindex $args 1]
       set unit  [lindex $args 2]
       if {$unit == "units"} {set step 1}
       set from [expr $from + $value * $step]
    }
  }
  set fsPar($fsN,from) $from 
  plotFeature $fsN
}

#######################################################
# my own scrollbar procedure, even with marker display
#
#
#######################################################
proc plotRect {fsN from to level color} {
   global fsPar
   set c        .fw$fsN.f3.sb2
   if {"$color" == "delete"} {
       set ret [$c delete scale$level]
   } else {
       set width    $fsPar($fsN,width)
       set y0       [expr 10 * $level]
       set y1       [expr 10 * ($level + 1)]
       set ret [$c create rect [expr 1.0 * $from * $width] $y0 \
	                       [expr 1.0 * $to   * $width] $y1 \
                               -fill $color -tag scale$level]
   }
   return "$c $ret"
}

proc Scale2 {fsN} {
   global fsPar

   set c .fw$fsN.f3.sb2
   set frameN   $fsPar($fsN,frameN) 
   set from     $fsPar($fsN,from)
   set to       $fsPar($fsN,to)
   set totalSec $fsPar($fsN,totalSec) 
   set mfrom    $fsPar($fsN,mfrom)
   set mto      $fsPar($fsN,mto)
   set width    $fsPar($fsN,width)

   $c configure -width $width
   if {$frameN == 0} {set frameN 1; set totalSec 1}
   $c delete scale0 scale1
   set gr [lindex [plotRect $fsN [expr 1.0 * $from / $frameN] [expr 1.0 * $to / $frameN] 0 grey] 1]
   set br [lindex [plotRect $fsN [expr 1.0 * $mfrom / $totalSec] [expr 1.0 * $mto / $totalSec] 1 blue] 1]
   set bar [expr 1.0 * ($to - $from) / $frameN]
   $c bind $gr <Button-1>   "set gr(x) %x; set gr(id) $gr"
   $c bind $gr <B1-Motion>  {%W move $gr(id) [expr %x - $gr(x)] 0; set gr(x) %x}
   $c bind $gr <B1-ButtonRelease> "\
	   fsScroll $fsN moveto \[expr 1.0 * \[lindex \[$c coords $gr\] 0\] / $width\]"
   bind $c <Button-2>  "fsScroll $fsN moveto \[expr 1.0 * \%x / $width - 0.5 * $bar\]"
#   $c bind $gr <B3-ButtonRelease> "fsScroll $fsN moveto \[expr 1.0 * \%x / $width - $bar\]"
}

#############################################################
# plotFeature1   replot featshow
# plotFeature    replot featshow and dependent featshows
# 
#############################################################
proc plotFeature1 {fsN {from ""}} {
    global fsPar Scale_Avoid_Recursion


#    if {[info level] > 9} { return }

    set top .fw$fsN
    set f3  $top.f3
    set f4  $top.f4
    bind $f3.c <Configure> ""; #disable autoresize
    $top configure -cursor watch
    $f3  configure -cursor watch

    set oldtype $fsPar($fsN,type)
    set type [getFeatParam $fsN]

#----------------------------------
#   FMatrix
#----------------------------------
    if {$type == "FMatrix"} {
       if {$type != $oldtype } {
           set fsPar($fsN,type)  $type
           menuFM $fsN
       }  
       if {$from == ""} {set from $fsPar($fsN,from)}

       # -- range  ---
       set width  $fsPar($fsN,width)
       set frameN $fsPar($fsN,frameN)
       set range  [expr 1.0 * $width / $fsPar($fsN,dx) ]
       if {$range < 1.0} {set range 1}

       # -- height ---
       set coeffs         $fsPar($fsN,coeffN)
       set fsPar($fsN,dy) [expr $fsPar($fsN,height) / $coeffs]
       set height         [expr $coeffs * \$fsPar($fsN,dy)]
# puts "wxh: $width $height"

       # -- from, to ---
       if {$from < 0} {set from 0}
       set to [expr $from + int(ceil($range)) - 1]
       if {$to >= $frameN} {
	   set from [expr $frameN - int($range)]
	   set to   [expr $frameN - 1]
       }
       if {$from < 0}   {set from 0}
       if {$to < $from} {
         set to $from
       }
       set fsPar($fsN,from)  $from
       set fsPar($fsN,to)    $to
       set fromSec  [expr 0.001 * $from   * $fsPar($fsN,shift) ]
       set toSec    [expr 0.001 * ($to+1) * $fsPar($fsN,shift) ]
       set fsPar($fsN,toSec)      $toSec
       set fsPar($fsN,fromSec)    $fromSec

       # -- step size for buttons ---
       set fsPar($fsN,bstep)  [expr int(ceil($range / 2.0))]

       # -- plot it ---
       $f3.c delete all
       $f3.r delete all
       wm title    .fw$fsN "#$fsN: $fsPar($fsN,fe) $fsPar($fsN,mfe)"
       wm iconname .fw$fsN "#$fsN: $fsPar($fsN,fe)"
       $f3.r configure -width $width
       set time $fsPar($fsN,time)
       if {$time == 0} { 
         DrawRuler $f3.r \
            [expr ($to + 1 - $from) * $fsPar($fsN,dx)] $from [expr $to + 1]
       } else {
         DrawRuler $f3.r \
          [expr ($to + 1 - $from) * $fsPar($fsN,dx)] $fromSec [expr $toSec]
       }
       $f3.c configure -width $width -height $height
       $f3.c create rect 0 0 $width $height -width 0 -fill white -tag bg
       # that's because of the binding of button-1 and -2 to 'all' except of
       # the labels.

       # -- min and max --
       if {$fsPar($fsN,autoscale)} {
          set minmax [$fsPar($fsN,fes):$fsPar($fsN,fe).data minmax]
          set fsPar($fsN,min) [lindex $minmax 0]
          set fsPar($fsN,max) [lindex $minmax 1]
       }
       $fsPar($fsN,fes) display $fsPar($fsN,fe) $f3.c \
             -from $from -to $to \
             -dx   $fsPar($fsN,dx) -dy $fsPar($fsN,dy)\
             -grey $fsPar($fsN,grey) -width 0 -space 0\
             -min  $fsPar($fsN,min) -max $fsPar($fsN,max)

       # -- plot other features ---
       set pcolors "red purple green yellow blue"
       set pindex 0
       foreach feature $fsPar($fsN,mfe) {
	   if {$pindex >= [llength $pcolors]} {set pindex 0}
           catch {
              $fsPar($fsN,fes):$feature.data display $f3.c \
                -from $from -to $to \
                -dx   $fsPar($fsN,dx) -dy $fsPar($fsN,dy)\
                -grey $fsPar($fsN,grey) -width 0 -space 0\
                -min  $fsPar($fsN,min) -max $fsPar($fsN,max)\
                -tag  p$pindex
              $f3.c itemconfigure p$pindex -fill [lindex $pcolors $pindex]
           }
	   incr pindex
       }

       plotMarker $fsN
       # -- plot labels ---
       if {$fsPar($fsN,showlabel)} {
         showLabels $f3.c $fsPar($fsN,labels) $fsPar($fsN,dx) $from $fsN
	 plotLabelbar $fsN $fsPar($fsN,labels) $frameN

       } else {
	 plotLabelbar $fsN "" 0
       }
       update

    } else {
#----------------------------------
#   SVector
#----------------------------------
       if {$type == "SVector"} {
          if {$type != $oldtype } {
              set fsPar($fsN,type)  $type
              menuSV $fsN       
          }

	  if {$from == ""} {eval "set from \$fsPar($fsN,from)"}

          # -- y scale --
          set scale    ".fw$fsN.f4.s"
          set scaleMax $fsPar($fsN,scaleMax)
          set scaleVal $fsPar($fsN,scale)

          if {$scaleVal > $scaleMax} {
              set scaleMax [expr int(ceil($scaleVal))]
	  } else {
              if {$scaleVal > [expr 9*$scaleMax/10]} {
                set scaleMax [expr int(ceil(1.25*$scaleMax))]
	      }
          }
          if {$scaleVal < [expr $scaleMax/10]} {
              set scaleMax [expr int(ceil(0.8*$scaleMax))]
	  }
          set fsPar($fsN,scaleMax) $scaleMax
          $scale configure -from [expr round(100 * $scaleMax)]
          $scale set             [expr round(100 * $scaleVal)]
	  set Scale_Avoid_Recursion 1

          # -- step size --
          set rate  $fsPar($fsN,rate)
          set shift $fsPar($fsN,shift)
          set step  [expr int($shift * $rate / $fsPar($fsN,dx))]
          if {$step < 1} {
             set step 1
             set fsPar($fsN,dx) [expr int($shift * $rate)]
          }
          set fsPar($fsN,step) $step

          # -- range ---
          set width  $fsPar($fsN,width)       
          set height $fsPar($fsN,height)
          set range  [expr $width * $step]

          # -- from, to ---
          set sampleN  $fsPar($fsN,frameN)
          if {$from < 0} {set from 0}
          set to [expr $from + $range - 1]
          if {$to >= $sampleN} {
	      set from [expr $sampleN - $range]
	      set to   [expr $sampleN - 1]
          }
          if {$from < 0} {set from 0}
          set fsPar($fsN,from)  $from
          set fsPar($fsN,to)    $to

          set fromSec  [expr 0.001 * $from / $rate ]
          set toSec    [expr 0.001 * ($to+1)   / $rate ]
	  set fsPar($fsN,toSec)      $toSec
	  set fsPar($fsN,fromSec)    $fromSec

          # -- step size for buttons ---
          set fsPar($fsN,bstep)  [expr $range / 2]

          # -- plot it ---
          $f3.c delete all
          $f3.r delete all
          wm title    .fw$fsN "#$fsN: $fsPar($fsN,fe) $fsPar($fsN,mfe)"
          wm iconname .fw$fsN "#$fsN: $fsPar($fsN,fe)"
          $f3.r configure -width $width
          set time $fsPar($fsN,time)
          if {$time == 0} { 
            DrawRuler $f3.r \
                  [expr ($to + 1 - $from) / $step] $from [expr $to + 1]
          } else {
            DrawRuler $f3.r \
                  [expr ($to + 1 - $from) / $step] $fromSec [expr $toSec]
          }
          $f3.c configure -width $width -height $height
          $f3.c create rect 0 0 $width $height -width 0 -fill white -tag bg
          $f4.s configure -length $height
          $fsPar($fsN,fes) : $fsPar($fsN,fe) . data display $f3.c \
              -from $from -to $to -height $height \
              -step $step -scale $fsPar($fsN,scale)
          # -- plot other features ---
          set pcolors "red  purple green yellow blue"
          set pindex 0
          foreach feature $fsPar($fsN,mfe) {
	      if {$pindex >= [llength $pcolors]} {set pindex 0}
              catch {
                 $fsPar($fsN,fes) : $feature . data display $f3.c \
                    -from $from -to $to -height $height -tag p$pindex\
                    -step $step -scale $fsPar($fsN,scale)
                 $f3.c itemconfigure p$pindex -fill [lindex $pcolors $pindex]
              }
	      incr pindex
          }

          # -- plot marker ---
          plotMarker $fsN
	  update
          # -- plot labels ---
          if {$fsPar($fsN,showlabel)} {
            showLabels $f3.c $fsPar($fsN,labels) $fsPar($fsN,dx) \
                  [expr 1.0 * $from / ($shift * $rate)] $fsN
	    set frameN [expr 1.0 * $sampleN / ($shift * $rate)]
	    plotLabelbar $fsN $fsPar($fsN,labels) $frameN
          } else {
	    plotLabelbar $fsN "" 0
	  }
       }
    }
#
# this would be for the scrollbar (which doesn't work) s.a.
#    set frameN $fsPar($fsN,frameN)
#    if {$frameN > 0} {
#      $f3.sb set \
#            [expr 1.0 * $fsPar($fsN,from) / $fsPar($fsN,frameN)] \
#            [expr 1.0 * $fsPar($fsN,to)   / $fsPar($fsN,frameN)]
#    }
#
    $top configure -cursor hand2
    $f3  configure -cursor top_left_arrow

    # ---- repack the canvas on top of packing list ----
    repackCanvas $fsN
    bind $f3.c <Configure> "autoresizeFS $fsN"; # enable autoresize

}
proc repackCanvas {fsN} {
    set  top .fw$fsN
    pack $top.lf -in $top
    pack $top.f0 -in $top.lf
    pack $top.f3 -in $top.f0
    pack $top.f3.c -in $top.f3   
}

proc plotFeature {fsN {from ""}} {
    global fsPar

    plotFeature1 $fsN $from

    foreach mfs $fsPar($fsN,mfs) {
       gotoTime1 $mfs $fsPar($fsN,fromSec) 0
    }
}

###################################################################
#  replot featshow #fsN and dependent featshows from the given time 
###################################################################
proc gotoTime {fsN time {shift 0.5}} {
   global fsPar

   set fsPar($fsN,from) [expr  int([fromTime $fsN $time] - $shift * $fsPar($fsN,bstep))];
   plotFeature $fsN
}

###################################################################
#  replot featshow #fsN from the given time 
###################################################################
proc gotoTime1 {fsN time {shift 0.5}} {
   global fsPar

   set fsPar($fsN,from) [expr  int([fromTime $fsN $time] - $shift * $fsPar($fsN,bstep))];
   plotFeature1 $fsN
}

proc changeLabel {fsN count text} {
   global fsPar

   set label  [lindex $fsPar($fsN,labels) $count]
   set fromto [lindex $label 1]
   set fsPar($fsN,labels) [lreplace $fsPar($fsN,labels) $count $count "{$text} {$fromto}"] 
   plotFeature $fsN
}

###################################################################
# Label stuff
###################################################################
catch { label_Info procs }
# this is to initialize 'labels.tcl'
# Now redefine selectLabels

##################################################
# selectLabel 
# If $count is to big print warning.
# else:
# Destroy old label text widget and select new 
# label $count. Put marker around.
# If $count is -1 unset labels (set to -1).
# If $count is -2 only destroy text widget and return 
# note: we don't use $text
##################################################

proc selectLabel {fsN count {text ""}} {
   global fsPar

   set list $fsPar($fsN,labels)
   if {$count >= [llength $list]} {
      puts "end of label list"
      return  -1
   }

   #--- destroy old label text widget ---
   set f2a .fw$fsN.f2a
   catch {destroy $f2a.update $f2a.infot}
   if {$count == -2} {return -2}

   if {$count >= 0} {
      set label   [lindex $list $count]

      set fsPar($fsN,labelX) $count
      set fsPar($fsN,ltext)  [lindex $label 0]
#      wm title .fw$fsN "#$fsN: $fsPar($fsN,fe) $fsPar($fsN,mfe) \"$fsPar($fsN,ltext)\""

      set text    [lindex $label 0]
      set fromto  [lindex $label 1]
      set from    [lindex $fromto 0]
      set to      [lindex $fromto 1]
      set fsPar($fsN,mfrom) [expr  0.001 * $from * $fsPar($fsN,shift)]
      set fsPar($fsN,mto)   [expr  0.001 * ($to + 1) * $fsPar($fsN,shift)]

      plotMarker $fsN
#      set fsPar($fsN,labels) "{{no labels} {[expr 100 * $fsPar(0,mfrom)] [expr -1 + 100 * $fsPar(0,mto)]}}"   

      #######################################
      # create text widget for label text 
      #######################################
      if {$fsPar($fsN,showltext)} { 
        Button $f2a.update -text "update" -command \
            "changeLabel $fsN $count \[$f2a.infot get 1.0 1.end\]; selectLabel $fsN $count"
        #--- create text widget  ---
#        set width 60
        set height 1
        set infot  [text $f2a.infot -height $height]
#        bind $infot <Return> "changeLabel $fsN $count \[$f2a.infot get 1.0 1.end\]"
        pack $f2a.update  -side left
        pack $f2a.infot   -side right -fill both -expand true

        #--- set height that the whole text fits into the widget  ---
        set width  [lindex [$infot configure -width] 4] ;# width of text widget
        set charN  [string length $text]                ;# #of characters
        set height [expr int(ceil(1.0 * $charN / $width))] ;# height
        $infot configure -height $height

        #--- insert text ---
        $f2a.infot insert insert $text
      } else {set fsPar($fsN,labelX) -1}

      return $count
   }
   return -1

}

proc deleteLabel {fsN} {
   global fsPar

   set count $fsPar($fsN,labelX)
   if {$count >= 0} {
     set fsPar($fsN,labels) [lreplace $fsPar($fsN,labels) $count $count] 
   }
   plotFeature $fsN
   selectLabel $fsN -2        ;# destroy the label text widget
   incr fsPar($fsN,labelX) -1 ;# select previous label but don't display 
}

proc insertLabel {fsN {text ""}} {
   global fsPar

   #--- calculate 'from' 'to' from marker ---
   set from [expr int(0.5 + 1000 * $fsPar($fsN,mfrom) / $fsPar($fsN,shift))]
   set to   [expr int(0.5 + 1000 * $fsPar($fsN,mto)   / $fsPar($fsN,shift))-1]

   #--- find the right place in label list ---
   set count 0
   foreach label  $fsPar($fsN,labels) {
       set fromto [lindex $label 1]
       set lfrom   [lindex $fromto 0]
       if {$from < $lfrom} break
       incr count
   }
   set fsPar($fsN,labels) [linsert $fsPar($fsN,labels) $count "{$text} {$from $to}"] 

   #--- replot and set as current label ---
   plotFeature $fsN
   selectLabel $fsN $count
   return $count
}

###############################################################
# change the text of the selected label and the time markings
# as set by the featshow marker.
# 'side' can be "left" or "right" then only one time marking
# will be changed.
###############################################################
proc setLabel {fsN {side all}} {
   global fsPar
   
   if {$fsPar($fsN,labelX) >= 0} {
      set i    $fsPar($fsN,labelX)
      set label   [lindex $fsPar($fsN,labels) $i]
      set fromto  [lindex $label  1]
      set from    [lindex $fromto 0]
      set to      [lindex $fromto 1]

      set text $fsPar($fsN,ltext)
      
      if {$side == "all" || $side == "left"} {
         set from [expr int(0.5 + 1000 * $fsPar($fsN,mfrom) / $fsPar($fsN,shift))]
      }
      if {$side == "all" || $side == "right"} {
	  set to   [expr int(0.5 + 1000 * $fsPar($fsN,mto)   / $fsPar($fsN,shift))-1]
      }
      set fsPar($fsN,labels) [
         lreplace  $fsPar($fsN,labels) $i $i "\"$text\" {$from $to}"
      ]
      plotFeature $fsN
      selectLabel $fsN $i
   } else {error "Select label by clicking text with mouse button-1 then try again"}
}

###############################################################
# merge with next label
# Concat text and take left marker of current label as start 
# and right marker of next label as end marking.
# 
###############################################################
proc mergeLabel {fsN} {
   global fsPar
   
   set labelX  $fsPar($fsN,labelX)
   set labelY  [expr $labelX + 1]
   set labelN  [llength $fsPar($fsN,labels)]

   if {$labelX >= 0 && $labelY < $labelN} {
      set Xlabel  [lindex $fsPar($fsN,labels) $labelX]
      set Ylabel  [lindex $fsPar($fsN,labels) $labelY]

      set Xtext   [lindex $Xlabel  0]
      set Ytext   [lindex $Ylabel  0]
      set Xfromto [lindex $Xlabel  1]
      set Yfromto [lindex $Ylabel  1]
      set from    [lindex $Xfromto 0]
      set to      [lindex $Yfromto 1]

      if {$to < $from} {
        puts "WARNING: End of next label is before start of current!"
	puts "         Won't do anything!"
        return -1
      }
      set text "$Xtext $Ytext"

      set fsPar($fsN,labels) [
         lreplace  $fsPar($fsN,labels) $labelX $labelY "\"$text\" {$from $to}"
      ]
      plotFeature $fsN
      selectLabel $fsN $labelX
   } else {error "No label selected or last label"}
}

proc incrLabel {fsN val {begin 1}} {
   global fsPar

   set i [expr $fsPar($fsN,labelX) + int($val)]
   if {[selectLabel $fsN $i ""] >= 0} {
       if {$begin} "gotoTime $fsN $fsPar($fsN,mfrom)" \
       else        "gotoTime $fsN $fsPar($fsN,mto)" 
   }
}

###################################
#   set minmax for FS window
###################################
proc minmaxApply {fsN} {
  global fsPar mfsMin mfsMax
  set fsPar($fsN,min) $mfsMin
  set fsPar($fsN,max) $mfsMax
  plotFeature $fsN
}
proc minmaxFS {fsN} {
  global fsPar mfsMin mfsMax mfsDoit

  set mfsDoit 1  
  set mfsMin $fsPar($fsN,min)
  set mfsMax $fsPar($fsN,max)

  set f [Toplevel .mFS ]
  wm title $f "minmax"
  set a [Frame $f.a -relief groove -borderwidth 2]
  set w [Frame $f.w -relief groove -borderwidth 2]
  set h [Frame $f.h -relief groove -borderwidth 2]
  set bt [Frame $f.bt -relief groove -borderwidth 2]
  pack $a $w $h $bt -side top -fill x

  checkbutton $a.r  -text "autoscale"  -variable fsPar($fsN,autoscale)
  Label $w.l1 -text "min: "
  Label $h.l1 -text "max: "
  Entry $w.e1 -relief flat -textvariable mfsMin
  Entry $h.e1 -relief flat -textvariable mfsMax
  pack $a.r $w.l1 $w.e1 $h.l1 $h.e1 -side left

  Button $bt.ok -text "ok"     -command "destroy $f"
  Button $bt.ap -text "apply"  -command "minmaxApply $fsN"
  Button $bt.ca -text "cancel" -command "set mfsDoit 0; destroy $f"
  pack   $bt.ok $bt.ap $bt.ca -padx 5 -pady 5 -side left
  bind   $f <Return> "destroy $f"
  tkwait window $f

  if {$mfsDoit} {minmaxApply $fsN}
}

###################################
#   resize window
###################################
proc resizeFS {fsN} {
  global fsPar rfsWidth rfsHeight rfsDoit

  set rfsDoit 1  
  set rfsWidth  $fsPar($fsN,width)
  set rfsHeight $fsPar($fsN,height)

  set f [Toplevel .rFS ]
  wm title $f "resize window"
  set w [Frame $f.w -relief groove -borderwidth 2]
  set h [Frame $f.h -relief groove -borderwidth 2]
  set bt [Frame $f.bt -relief groove -borderwidth 2]
  pack $w $h $bt -side top -fill x

  Label $w.l1 -text "width: "
  Label $h.l1 -text "height:"
  Entry $w.e1 -relief flat -width 4 -textvariable rfsWidth
  Entry $h.e1 -relief flat -width 4 -textvariable rfsHeight
  pack $w.l1 $w.e1 $h.l1 $h.e1 -side left

  Button $bt.ok -text "ok"     -command "destroy $f"
  Button $bt.ca -text "cancel" -command "set rfsDoit 0; destroy $f"
  pack   $bt.ok $bt.ca -padx 5 -pady 5 -side left
  bind   $f <Return> "destroy $f"
  tkwait window $f

  if {$rfsDoit} {
  # check values here
    set fsPar($fsN,width) $rfsWidth
    set fsPar($fsN,height) $rfsHeight
    plotFeature $fsN
  }
}

# --------------------------------------------------------
# --------------------------------------------------------
proc autoresizeFS {fsN} {
    global fsPar

    set f3 .fw$fsN.f3
    set c  $f3.c

    bind $c <Configure> ""
#    pack configure $c  -fill both
#    pack configure $f3 -fill both
#    update
    $c configure -scrollregion {0 0 10000 10000}
    set xsize [expr [lindex [$c xview] 1] - [lindex [$c xview] 0]]
    set ysize [expr [lindex [$c yview] 1] - [lindex [$c yview] 0]]
    set fsPar($fsN,width)  [expr int(10000 * $xsize)]
    set fsPar($fsN,height) [expr int(10000 * $ysize)]
#    puts "new canvas size is $fsPar($fsN,width) $fsPar($fsN,height)"
#    pack configure $c  -fill none
#    pack configure $f3 -fill none
    plotFeature $fsN
}

##############################################
# Add extra feature to be plot. Look if the
# feature is already in the list. If so remove
# from list and add as last.
# arguments:
#   fsN         feature display number
#   feature     extra feature
##############################################
proc addFeature {fsN feature} {
   global fsPar

   set type     [$fsPar($fsN,fes) : $fsPar($fsN,fe) . data type]
   set newtype  [$fsPar($fsN,fes) : $feature        . data type]

   if {$newtype == $type} {
      set list "$fsPar($fsN,fe) $fsPar($fsN,mfe)"
      set index [lsearch -exact $list $feature]
      if {$index >= 0} {set list [lreplace $list $index $index]}
      lappend list $feature
      set fsPar($fsN,fe)  [lindex $list 0]
      set fsPar($fsN,mfe) [lrange $list 1 end]

   } else {
      set fsPar($fsN,mfe) ""  
      set fsPar($fsN,fe) "$feature"
   }  
}

##############################################
# feature selection window
# arguments:
#   fsN     feature display number
#   w       toplevel window   
##############################################
proc feList {fsN {w .l1}} {
    global fsPar


    catch {destroy $w}
    if {$w == ".l1"} {  
      Toplevel $w
      wm title $w "Feature Listbox #$fsN"
      wm iconname $w "Features"
      wm minsize $w 1 1
    } else {
      Frame $w
      pack $w -side right
    }

    Message $w.msg -font -Adobe-times-medium-r-normal--*-140* \
	-aspect 300 -text "Select feature with mouse button-(1) or add with button-(3):"
    Frame $w.frame -borderwidth 10
    Frame $w.b     -borderwidth 10
    Button $w.b.ok -text "close selection"  -command "destroy $w"
    Button $w.b.rr -text "re-read features" -command "$w.frame.list delete 0 end; eval \"$w.frame.list insert 0 \[$fsPar($fsN,fes)\]\""
    pack $w.msg   -side top
    pack $w.frame -side top -fill y ;# -expand yes 
    pack $w.b     -side bottom
    pack $w.b.rr $w.b.ok  -side top -fill x

    scrollbar $w.frame.scroll -relief sunken -command "$w.frame.list yview"
    listbox $w.frame.list -yscrollcommand "$w.frame.scroll set" -relief sunken -setgrid 1
    pack $w.frame.scroll -side right -fill y
    pack $w.frame.list -side left -fill both 
# -expand yes 

    eval "$w.frame.list insert 0 [$fsPar($fsN,fes)]"
    if 0 {
	# This inhibits selection of scrolled-out items for me (Florian)
	bind $w <Enter> " $w.frame.list delete 0 end;\
         eval \"$w.frame.list insert 0 \[$fsPar($fsN,fes)\]\" "
    }
    bind $w.frame.list <ButtonRelease-1> \
        "set fsPar($fsN,fe) \[lindex \[selection get\] 0\]
         set fsPar($fsN,mfe) \"\"
         plotFeature $fsN"
    bind $w.frame.list <ButtonPress-3> {
         #%W select clear 0 end
         %W select set [%W nearest %y]}
    bind $w.frame.list <ButtonRelease-3> \
        "addFeature $fsN \[\%W get \[\%W nearest \%y\]\]
         plotFeature $fsN"
}


######################################
# What happens if samplingRate is
# changed
######################################
proc changeRate {fsN} {
   global fsPar
   
   set oldRate [$fsPar($fsN,fes) : $fsPar($fsN,fe) configure -samplingRate]
   $fsPar($fsN,fes) : $fsPar($fsN,fe) configure -samplingRate $fsPar($fsN,rate)
   set fsPar($fsN,mfrom) \
         [expr $fsPar($fsN,mfrom) * $oldRate / $fsPar($fsN,rate)]
   set fsPar($fsN,mto)   \
         [expr $fsPar($fsN,mto) * $oldRate / $fsPar($fsN,rate)]

   plotFeature $fsN

}

######################################
# How does the menu change for
# FMatrix type features and
# Svector type features
######################################
proc menuFM {fsN} {
    global fsPar

    set f1 .fw$fsN.f1
    set f2 .fw$fsN.f2
    set f3   .fw$fsN.f3
    set f4 .fw$fsN.f4
    $f1.display.menu entryconfigure "mode" -state normal
    catch {destroy $f1.v2 $f1.v0 $f1.v1}
# $f1.v1 configure -state disabled

    $f1.p1 configure -state disabled
    Label  $f2.l6    -text " coeffs:"
    Label  $f2.l7    -textvariable fsPar($fsN,coeffN)
    pack   $f2.l6 $f2.l7 -side left

    $f3 configure -cursor top_left_arrow
    bind $f3.c <Button-2> ""
    $f1.e2 configure -state disabled 

    catch {destroy $f4}

    set fsPar($fsN,from) \
        [expr int(1000 * $fsPar($fsN,fromSec) / $fsPar($fsN,shift))]
}

proc menuSV {fsN} {
    global fsPar

    set top  .fw$fsN
    set f1   .fw$fsN.f1
    set f2   .fw$fsN.f2
    set f3   .fw$fsN.f3
    set f0   .fw$fsN.f0
    $f1.display.menu entryconfigure "mode" -state disabled
    $f1.p1 configure -state normal

    Button $f1.v2 -text "100%"  -command \
	"set fsPar($fsN,scale) 1.0;  plotFeature $fsN"
    Button $f1.v0 -text "+"     \
                  -command " set fsPar($fsN,scale) \
                             \[expr $fsPar($fsN,scale) * 1.25\];\
                             plotFeature $fsN"
    Button $f1.v1 -text "-"    \
                  -command " set fsPar($fsN,scale) \
                             \[expr $fsPar($fsN,scale) * 0.80\];\
                             plotFeature $fsN"
    pack $f1.v1 $f1.v0 $f1.v2 -side right

    set  f4  [Frame $top.f4]
    pack $f4 -in $f0 -side right -fill y
    set scaleMax [expr int($fsPar($fsN,scaleMax) * 100)]
    Scale    $f4.s -orient vertical -label "%" \
                   -length $fsPar($fsN,height) -from $scaleMax -to 1 \
                   -command "setScale $fsN"
    pack     $f4.s -pady 5 -side top

    $f3 configure -cursor top_left_arrow
    $f1.e2 configure -state normal
    bind $f3.c <Button-2> "play $fsN"

    catch {destroy $f2.l6}
    catch {destroy $f2.l7}

    set rate  [$fsPar($fsN,fes) : $fsPar($fsN,fe) configure -samplingRate]
    set fsPar($fsN,from) [expr int(1000 * $fsPar($fsN,fromSec) * $rate)]
}

proc setScale {fsN val} {
    global fsPar Scale_Avoid_Recursion
    set fsPar($fsN,scale) [expr $val / 100.0]
    if {![info exists Scale_Avoid_Recursion]} {
	plotFeature $fsN
    }
    catch {unset Scale_Avoid_Recursion}
} 

proc configTime {fsN} {
    global fsPar

   set time $fsPar($fsN,time)
    set f2 .fw$fsN.f2
    if {$time} {
	$f2.l1  configure  -textvariable fsPar($fsN,fromSec)
#	$f2.l2  configure  -text "s.."
	$f2.l3  configure  -textvariable fsPar($fsN,toSec)
	$f2.l4  configure  -text "s total:"
	$f2.l5  configure  -textvariable fsPar($fsN,totalSec)
	$f2.l5a configure  -text "s"
    } else {
	$f2.l1  configure  -textvariable fsPar($fsN,from)
#	$f2.l2  configure  -text ".."
	$f2.l3  configure  -textvariable fsPar($fsN,to)
	$f2.l4  configure  -text "total:"
	$f2.l5  configure  -textvariable fsPar($fsN,frameN)
	$f2.l5a configure  -text ""
    }
}

#####################################################
#
#   refreshFeature 
# Looks for a feature and plot it again. If the
# feature couldn't be found a new 'featshow' will
# be started. 
#
#####################################################
proc refreshFeature {fs fe} {
  global fsN fsPar
  for {set i 0} {$i < $fsN} {incr i} {
     set name $fsPar($i,fe)
     if {$name == $fe} break
  }
  if {$i < $fsN} {
     plotFeature $i
  } else {
     set i [featshow $fs $fe]
  }
  return $i
}

#####################################################
#
#   featshow
# show Feature 'fe' of FeatureSet 'fs' in a window
#
#####################################################
#puts "defined procedure featshow:"
#puts "USAGE: featshow <featureset> <feature> \[<width> \[<height>\]\]"
#puts ""
proc featshow { args } {
    global fsN
    global fsPar
    global JANUSLIB DEVICEPLAY

    set width  600
    set height 300

    if {[catch {itfParseArgv featshow $args [ list [
        list "<FeatureSet>" object {} fes    FeatureSet "FeatureSet to use" ] [
        list "<Feature>"    string {} fe     {}         "name of feature to display" ] [
        list "-width"       int    {} width  {}         "width of window" ] [
        list "-height"      int    {} height {}         "height of window" ] ]
    } ] } {
	return
    }

# --  color for canvas and ruler  --
    set cancol    white
    set rulcol    khaki1
# -- variables of the feature window --
    set fsPar($fsN,fes)      $fes       ;#featureSet
    set fsPar($fsN,fe)       $fe        ;#feature
    set fsPar($fsN,mfe)      ""         ;#more features to display
    set fsPar($fsN,mfs)      ""         ;#more featshows to replot
    set fsPar($fsN,width)    $width     ;#canvas width in pixel
    set fsPar($fsN,height)   $height    ;#   "   height "   "
    set fsPar($fsN,dx)       10         ;#frame   width  in pixel
    set fsPar($fsN,dy)       10         ;#channel height in pixel
    set fsPar($fsN,step)     0          ;#step in samples for SVectorPlot

    set fsPar($fsN,fromSec)  0.0        ;#display from in sec
    set fsPar($fsN,toSec)    0.0        ;#display to   in sec
    set fsPar($fsN,totalSec) 0.0        ;#total time in sec

    set fsPar($fsN,from)     0          ;#display from
    set fsPar($fsN,to)       0          ;#display to
    set fsPar($fsN,frameN)   0          ;#number of frames (samples)
    set fsPar($fsN,coeffN)   1          ;#   "   "  coefficients
    set fsPar($fsN,rate)     [$fes configure -samplingRate]
    set fsPar($fsN,shift)    0          ;#frame shift in ms

    set fsPar($fsN,grey)     1          ;#grey values, boolean
    set fsPar($fsN,ruler)    1          ;#show ruler,  boolean
    set fsPar($fsN,bars)     1          ;#show scale,  boolean 
    set fsPar($fsN,time)     0          ;#time scale,  boolean
    set fsPar($fsN,type)     "UNDEF"    ;#Feature type

    # ---- marker ----
    set fsPar($fsN,mfrom)    0          ;#marker from in sec
    set fsPar($fsN,mto)      -1         ;#  "    to   "   "

    # ---- labels ----
    set fsPar($fsN,showlabel) 0         ;#show labels,  boolean
    set fsPar($fsN,showltext) 1         ;#show label text,  boolean
    set fsPar($fsN,labelX)   -1         ;#current label index
    set fsPar($fsN,ltext)    ""         ;#current label text
    set fsPar($fsN,labels)   {{"no labels" {0 190}}}

    # How many points to go forward, don't mix up SVector plot step: 
    set fsPar($fsN,bstep)     10        ;#it's half the window
    set fsPar($fsN,scale)    1.0
    set fsPar($fsN,scaleMax) 5.0

    set fsPar($fsN,min)      0.0        ;#FMatrix minimum, float
    set fsPar($fsN,max)      0.0        ;#FMatrix maximum, float
    set fsPar($fsN,autoscale)  1        ;#FMatrix autoscale, boolean

# == window ==
    set top [Toplevel .fw$fsN -cursor hand2]
    wm title    $top  "#$fsN: $fe"
    wm iconname $top  "#$fsN: $fe"
    catch {wm iconbitmap $top  @$JANUSLIB/.featshow.xbm}

#   -- components of the left frame: menu, canvas, info --
    set lf   [Frame    $top.lf ]
    set f1   [Frame    $top.f1 ]    ;# Menu
    set f0   [Frame    $top.f0 ]    ;# Plot region
    set f2   [Frame    $top.f2 ]    ;# Info, Scroll buttons
    set f2a  [Frame    $top.f2a ]   ;# Label text
    pack     $lf         -side left -fill both -expand true
    pack     $f1 -in $lf -side top  -fill x
    pack     $f0 -in $lf            -fill both -expand true
    pack     $f2a -in $lf -side bottom -fill x
    pack     $f2 -in $lf -side bottom  -fill x

#   -- left  components in f0 = f3: function plot, ruler and scrollbar  --
#   -- rigth components in f0 = f4: scale                   --
    set f3    [Frame    $top.f3 -bd 5 -relief groove -cursor pirate]
    canvas    $f3.c -width $width -height $height -background $cancol
    canvas    $f3.r -width $width -height [expr 40 * $fsPar($fsN,ruler)] \
                   -background $rulcol
    canvas    $f3.sb2 -width $width -height [expr 30  * $fsPar($fsN,bars)] \
                   -background $cancol
#    scrollbar $f3.sb -orient horizontal -repeatdelay 0 -jump 1 \
#                     -command "fsScroll $fsN"
#    $f3.sb set 0 1
    pack      $f3         -in $f0 -pady 5 -side left   -expand true  -fill both
    pack      $f3.sb2 $f3.r               -side bottom ;# -fill x    ;# -expand true
    pack      $f3.c                       -side top    -expand true  -fill both
#    bind      $f3.c <Configure> "autoresizeFS $fsN"

#   -- menu --
    Button $f1.f0 -text    "Quit" \
                  -command "set fsPar($fsN,fe) \"\"; destroy .fw$fsN"
    Menubutton $f1.display \
                  -text    "Display"  -underline 0 \
                  -menu    $f1.display.menu
      set m1 [Menu $f1.display.menu]
      $m1 add command \
                  -label   "new frame" \
                  -command "featshow \$fsPar($fsN,fes) \$fsPar($fsN,fe) \
                            \$fsPar($fsN,width) \$fsPar($fsN,height)"
      $m1 add command \
                  -label   "save as ps file" \
                  -command "$f3.c postscript \
                   -file \[FSBox \"Select ps file to save:\"\] \
                   -width \$fsPar($fsN,width) -height \$fsPar($fsN,height)"
      $m1 add separator
      $m1 add command \
                  -label   "minmax"\
                  -command "minmaxFS $fsN"
      $m1 add cascade -label "mode" -menu $m1.mode
          Menu $m1.mode
          $m1.mode add radiobutton \
                  -label   "blob" \
                  -variable fsPar($fsN,grey) -value 0 \
                  -command "plotFeature $fsN"
          $m1.mode add radiobutton \
                  -label   "grey" \
                  -variable fsPar($fsN,grey) -value 1 \
                  -command "plotFeature $fsN"
          $m1.mode add radiobutton \
                  -label   "horizontal" \
                  -variable fsPar($fsN,grey) -value 2 \
                  -command "plotFeature $fsN"
          $m1.mode add radiobutton \
                  -label   "vertical" \
                  -variable fsPar($fsN,grey) -value 3 \
                  -command "plotFeature $fsN"
      $m1 add cascade -label "ruler" -menu $m1.ruler
          Menu $m1.ruler
          $m1.ruler add checkbutton  \
                  -label   "show"  -variable fsPar($fsN,ruler) \
                  -command "$f3.r configure -height \
                            \[expr 40 * \$fsPar($fsN,ruler)\]"
          $m1.ruler add checkbutton \
                  -label   "time"   -variable fsPar($fsN,time) \
                  -command "configTime $fsN; plotFeature $fsN"
      $m1 add checkbutton  \
                  -label   "bars"   -variable fsPar($fsN,bars) \
                  -command "$f3.sb2 configure -height \
                            \[expr 30 * \$fsPar($fsN,bars)\]"
      $m1 add command \
                  -label   "size"\
                  -command "resizeFS $fsN"
      $m1 add separator
      $m1 add cascade -label "colors" -menu $m1.col
          Menu $m1.col
          $m1.col add command \
                  -label   "grey" \
                  -command "ButtonConfig grey70; BgConfig grey90"
          $m1.col add command \
                  -label "mixed" \
                  -command "ButtonConfig lightblue3; BgConfig peachpuff"
          $m1.col add command \
                  -label "blue" \
                  -command "ButtonConfig SkyBlue2; BgConfig LightSteelBlue"
          $m1.col add command \
                  -label "green" \
                  -command "ButtonConfig CadetBlue3; BgConfig DarkSeaGreen2"
          $m1.col add command \
                  -label "rosy" \
                  -command "ButtonConfig IndianRed1; BgConfig RosyBrown1"
    Menubutton $f1.marker -text "Marker" -underline 0 -menu $f1.marker.menu
      set m3 [Menu $f1.marker.menu]
      $m3 add command \
                  -label   "mark all" \
                  -command "set fsPar($fsN,mfrom) 0;\
                            set fsPar($fsN,mto)  -1;\
                            plotMarker $fsN"
      $m3 add command \
                  -label   "mark window" \
                  -command "set fsPar($fsN,mfrom) \$fsPar($fsN,fromSec);\
                            set fsPar($fsN,mto)   \$fsPar($fsN,toSec);\
                            plotMarker $fsN"
      $m3 add command \
                  -label   "cut marked region" \
                  -command "\$fsPar($fsN,fes) cut \$fsPar($fsN,fe) \$fsPar($fsN,fe) \
                            \$fsPar($fsN,mfrom)s \$fsPar($fsN,mto)s; plotFeature $fsN"
      $m3 add separator
      $m3 add checkbutton  \
                  -label   "show labels"   -variable fsPar($fsN,showlabel) \
                  -command "plotFeature $fsN"
      $m3 add checkbutton  \
                  -label   "show label text"   -variable fsPar($fsN,showltext) \
                  -command "incrLabel $fsN 0 1"

      $m3 add separator
      $m3 add command  \
                  -label   "next label"  -accelerator "CTRL-F"\
                  -command "incrLabel $fsN +1"
      $m3 add command  \
                  -label   "previous label"  -accelerator "CTRL-B"\
                  -command "incrLabel $fsN -1"
      $m3 add command  \
                  -label   "start of label"  -accelerator "CTRL-A"\
                  -command "incrLabel $fsN 0 1"
      $m3 add command  \
                  -label   "end of label"  -accelerator "CTRL-E"\
                  -command "incrLabel $fsN 0 0"
      $m3 add separator
      $m3 add command  \
                  -label   "insert label" -accelerator "CTRL-I"\
                  -command "insertLabel $fsN"
      $m3 add command  \
                  -label   "delete label" \
                  -command "deleteLabel $fsN"
      $m3 add command  \
                  -label   "merge label with next" -accelerator "CTRL-M"\
                  -command "mergeLabel $fsN"
      $m3 add command  \
                  -label   "set label" -accelerator "CTRL-S"\
                  -command "setLabel $fsN"
      $m3 add command  \
                  -label   "set left label marker" -accelerator "CTRL-L"\
                  -command "setLabel $fsN left"
      $m3 add command  \
                  -label   "set right label marker" -accelerator "CTRL-R"\
                  -command "setLabel $fsN right"
      $m3 add separator
      $m3 add command  \
                  -label   "labels from path" -command "setPathLabels $fsN \$SID"

      bind $top <Control-i> "insertLabel $fsN"
      bind $top <Control-s> "setLabel $fsN"
      bind $top <Control-m> "mergeLabel $fsN"
      bind $top <Control-r> "setLabel $fsN right"
      bind $top <Control-l> "setLabel $fsN left"
      bind $top <Control-f> "incrLabel $fsN +1"
      bind $top <Control-b> "incrLabel $fsN -1"
      bind $top <Control-a> "incrLabel $fsN  0 1"
      bind $top <Control-e> "incrLabel $fsN  0 0"
      bind $top <Enter>     {focus %W}

    Menubutton $f1.device -text "Device" -underline 2 -menu $f1.device.menu
      set m2 [Menu $f1.device.menu]
      $m2 add command -label "gain" -command "setDeviceGain"
      $m2 add separator
      foreach dev [array names DEVICEPLAY] {
        $m2 add radiobutton -label "$dev" -variable DEVICE -value "$dev"
      }
      $m2 add separator
      $m2 add command -label "Set user device" -command "setUserDevice"
    Button $f1.f1 -text "Feature" -command "feList $fsN $top.rf"
    Label  $f1.e0 -text    " "
    Button $f1.b1 -bitmap  \
                  @$JANUSLIB/.icon.closer.17x18.xbm\
                  -text "1/2" \
                  -command "set fsPar($fsN,dx) \[expr \$fsPar($fsN,dx) * 2\];\
                            plotFeature $fsN"
    Button $f1.b2 -bitmap \
                   @$JANUSLIB/.icon.wider.17x18.xbm\
                  -text "2" \
                  -command "set fsPar($fsN,dx) \[expr \$fsPar($fsN,dx) / 2\];\
                            plotFeature $fsN "
    Label  $f1.e1 -text    " "
    Button $f1.p1 -bitmap \
                   @$JANUSLIB/.icon.audio.17x18.xbm\
                  -text "play"\
                  -command "play $fsN"
    $f1.p1 configure -state disabled
    Label  $f1.e2a -text  " "
    Entry  $f1.e2  -relief flat -width 6 -textvariable fsPar($fsN,rate)
    $f1.e2 configure -state disabled 
    Label  $f1.e3 -text "kHz"
    Label  $f1.e4 -text " "

    pack $f1.f0 $f1.f1 $f1.e4 $f1.display $f1.marker $f1.device\
         $f1.e0 $f1.b1 $f1.b2 \
         $f1.e1 $f1.p1 \
         $f1.e2a $f1.e2 $f1.e3 -side left

#   -- info --
    Label  $f2.l0    -text " display: "
    Label  $f2.l1    -textvariable fsPar($fsN,from)
    Label  $f2.l2    -text ".."
    Label  $f2.l3    -textvariable fsPar($fsN,to)
    Label  $f2.l4    -text " total:"
    Label  $f2.l5    -textvariable fsPar($fsN,frameN)
    Label  $f2.l5a    -text " "
    # 6 and 7 are for coeffs
    Label  $f2.l8    -text " marked: "
    Label  $f2.l9    -textvariable fsPar($fsN,mfrom)
    Label  $f2.l10    -text ".."
    Label  $f2.l11    -textvariable fsPar($fsN,mto)
    Label  $f2.l12   -text "s"

    pack   $f2.l0 $f2.l1 $f2.l2 $f2.l3 $f2.l4 $f2.l5 $f2.l5a -side left
#   -- scroll buttons --
    Button $f2.b-2 -text  "<<" -command \
                 "incr fsPar($fsN,from) -\$fsPar($fsN,bstep); plotFeature $fsN"
    Button $f2.b-1 -text  "<"  -command \
                 "incr fsPar($fsN,from) -1; plotFeature $fsN"
    Button $f2.b1  -text   ">" -command \
                 "incr fsPar($fsN,from); plotFeature $fsN"
    Button $f2.b2  -text  ">>" -command \
                 "incr fsPar($fsN,from) \$fsPar($fsN,bstep); plotFeature $fsN"
    pack   $f2.b2 $f2.b1 $f2.b-1 $f2.b-2 -side right
    pack   $f2.l12 $f2.l11 $f2.l10 $f2.l9 $f2.l8 -side right

#   -- canvas and ruler --
    plotFeature $fsN

    $f3.c bind all <Button-1> "DoB1 $fsN \[$f3.c canvasx %x\]"
    $f3.c bind all <Button-3> "DoB3 $fsN \[$f3.c canvasx %x\]"
    bind $f1.e2 <Return> "changeRate $fsN"
    incr fsN
    return [expr $fsN - 1]
}








