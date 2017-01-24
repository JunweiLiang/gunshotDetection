#  ========================================================================
#   JANUS      Speech- to Speech Translation
#              ------------------------------------------------------------
#
#   Author  :  Martin Maier
#   Email   :  maier@ira.uka.de
#   Script  :  featview.tcl
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
#   Revision 1.2  2003/08/14 11:19:40  fuegen
#   Merged changes on branch jtk-01-01-15-fms (jaguar -> ibis-013)
#
#   Revision 1.1.52.1  2002/11/04 15:02:08  metze
#   Added documentation code
#
#   Revision 1.1  2000/08/23 11:00:01  jmcd
#   Added janus files from gui-tcl.
#
#   Revision 1.4  1997/10/13 16:52:54  westphal
#   copy ADC to ADC[12]
#
#   Revision 1.3  1997/09/01 16:33:05  westphal
#   byte mode shorten
#
#   Revision 1.2  1997/07/28 16:01:07  westphal
#   *** empty log message ***
#
#   Revision 1.1  1997/07/28 15:39:18  westphal
#   Initial revision
#
# Revision 1.10  1996/03/14  14:28:04  maier
# samplRate in float
#
# Revision 1.9  1996/02/07  12:37:18  maier
# *** empty log message ***
#
# Revision 1.8  1996/01/15  12:43:37  maier
# *** empty log message ***
#
# Revision 1.7  1995/10/17  11:12:51  maier
# used $JANUSLIB, made featview a procedure
#
# Revision 1.6  1995/10/09  14:10:08  maier
# *** empty log message ***
#
# Revision 1.5  1995/10/09  10:54:52  maier
# adapted for new adcIO routines
#
# Revision 1.4  1995/08/31  12:33:35  maier
# ChannelX, ChannelN
#
# Revision 1.3  1995/08/31  08:51:02  maier
# times in ms
#
# Revision 1.2  1995/08/23  17:21:09  maier
# save ADC
#
# Revision 1.1  1995/08/22  08:57:04  maier
# Initial revision
#
# 
#  ========================================================================

#source $JANUSLIB/featshow.tcl
#source $JANUSLIB/FSBox.tcl
#    set fsBox(pattern) {*[{.wav}{.ADC}{.au}{.ulaw}{.alaw}]}


proc featview_Info { name } {
    switch $name {
	procs {
	    return "featview"
	}
	help {
	    return "These functions allow you to display features in a FeatureSet."
	}
	featview {
	    return "Displays different aspects of a FeatureSet, i.e. view its contents, load alternate files ..."
	}
    }
    return "???"
}


#############################################################
#
#  Read ADC
#
#############################################################
proc formatADC {fs {frame ""}} {
  global FEadcFormat FEadcHeader FEadcReadFrom FEadcReadTo FEadcChN FEadcChX

  if {$frame != ""} {
     catch {destroy $frame}
     set f [frame $frame]
     pack $f -side bottom -fill x
  } else {
     catch {destroy .fADC}
     set f [Toplevel .fADC]
     wm title $f "ADC format"
  }
  set hs [Frame $f.hs -relief groove -borderwidth 2]
  set bf [Frame $f.bf -relief groove -borderwidth 2]
  set ch [Frame $f.ch -relief groove -borderwidth 2]
  set sr [Frame $f.sr -relief groove -borderwidth 2]
  set bt [Frame $f.bt -relief groove -borderwidth 2]
  pack  $hs $bf $ch $sr $bt -side top -fill x

  # --------------
  #  header size
  # --------------
  Frame $hs.l
  Label $hs.l.l1 -text "Header Size in byte:"
  Entry $hs.l.e1 -relief sunken -width 6  -textvariable FEadcHeader

  Frame $hs.r
  Radiobutton $hs.r.r1 -relief flat -text    "0" -variable FEadcHeader -value    "0"
  Radiobutton $hs.r.r2 -relief flat -text  "512" -variable FEadcHeader -value  "512"
  Radiobutton $hs.r.r3 -relief flat -text "1024" -variable FEadcHeader -value "1024"
  Radiobutton $hs.r.r4 -relief flat -text "auto" -variable FEadcHeader -value "auto"

  pack $hs.l $hs.r -padx 5 -pady 5 -fill x -fill y -side left
  pack $hs.l.l1 -side top
  pack $hs.l.e1 -pady 10
  pack $hs.r.r1 $hs.r.r2 $hs.r.r3 $hs.r.r4 -anchor w -side top

  # --------------
  #  byte format
  # --------------
  Frame $bf.t
  Label $bf.t.l0 -text "Byte format:"

  Frame $bf.f1
  Label $bf.f1.l1 -text " 8 bit"
  Radiobutton $bf.f1.r1 -relief flat   -text    "linear" -variable FEadcFormat \
                        -value  "lin"
  Radiobutton $bf.f1.r2 -relief flat   -text    "u-law"  -variable FEadcFormat \
                        -value  "ulaw"
  Radiobutton $bf.f1.r3 -relief flat   -text    "a-law"  -variable FEadcFormat \
                        -value  "alaw"

  Frame $bf.f2
  Label $bf.f2.l2 -text "16 bit"
  Radiobutton $bf.f2.r4 -relief flat   -text    "normal" -variable FEadcFormat \
                        -value  "01"
  Radiobutton $bf.f2.r5 -relief flat   -text    "swap"   -variable FEadcFormat \
                        -value  "10"
  Radiobutton $bf.f2.r6 -relief flat   -text    "auto"   -variable FEadcFormat \
                        -value "auto"

  Frame $bf.f3
  Label $bf.f3.l3 -text "other"
  Radiobutton $bf.f3.r7 -relief flat   -text    "shorten"  -variable FEadcFormat \
                        -value  "shorten"

  pack $bf.t       -padx  5 -pady 5 -fill x -side top
  pack $bf.f1 $bf.f2 $bf.f3 -padx 15 -pady 5 -anchor n -side left

  pack $bf.t.l0 -side left
  pack $bf.f1.l1 $bf.f1.r1 $bf.f1.r2 $bf.f1.r3 -anchor w -side top
  pack $bf.f2.l2 $bf.f2.r4 $bf.f2.r5 $bf.f2.r6 -anchor w -side top
  pack $bf.f3.l3 $bf.f3.r7                     -anchor w -side top

  # --------------
  #  channels
  # --------------
  Frame $ch.t2
  Label $ch.t2.l2 -text "Channels:        "
  Entry $ch.t2.e1 -relief flat -width 2  -textvariable FEadcChN
  Button $ch.t2.b1 -text "+" -command         \
     "incr FEadcChN;                          \
      selectCh $ch"
  Button $ch.t2.b2 -text "-" -command         \
     "incr FEadcChN -1;                       \
      if {\$FEadcChN < 1} {                   \
        set FEadcChN 1                        \
      };                                      \
      if {\$FEadcChX >= \$FEadcChN} {         \
        set FEadcChX \[expr \$FEadcChN - 1\]  \
      };\
      selectCh $ch"
  Label $ch.t2.l3 -text " from: "
  Entry $ch.t2.e2 -relief flat -width 4  -textvariable FEadcReadFrom
  Label $ch.t2.l4 -text "sec   to: "
  Entry $ch.t2.e3 -relief flat -width 4  -textvariable FEadcReadTo
  Label $ch.t2.l5 -text "sec"
  


  pack $ch.t2  -padx 5 -pady 5 -fill x -side top 
  pack $ch.t2.l2 $ch.t2.e1 $ch.t2.b1 $ch.t2.b2 \
       $ch.t2.l3 $ch.t2.e2 $ch.t2.l4 $ch.t2.e3  $ch.t2.l5 -side left 
  selectCh $ch

  # ---------------
  #  sampling Rate
  # ---------------
  Frame $sr.t
  Label $sr.t.l0  -text "Sampling rate: "
  Entry $sr.t.e1  -relief flat -state disabled -width 2  \
                  -textvariable FEadcSamplRate
  Button $sr.t.b1 -text "+" -command           \
     "set FEadcSamplRate \[expr \$FEadcSamplRate + 1.0\];   \
      $fs configure -samplingRate \$FEadcSamplRate"
  Button $sr.t.b2 -text "-" -command           \
     "set FEadcSamplRate \[expr \$FEadcSamplRate - 1.0\];   \
      if {\$FEadcSamplRate < 1.0} {                \
        set FEadcSamplRate 1.0                     \
      };                                       \
      $fs configure -samplingRate \$FEadcSamplRate"

  pack $sr.t  -padx 5 -pady 5 -fill x -side top 
  pack $sr.t.l0 $sr.t.e1 $sr.t.b1 $sr.t.b2  -side left 

  # --------------
  #  buttons
  # --------------
  if {$frame != ""} {
    Button $bt.ok -text "close settings \"load ADC\"" -command "destroy $f"
    pack   $bt.ok -pady 5
  } else {
    Button $bt.ok -text "ok" -command "destroy $f"
    pack   $bt.ok -pady 5
    tkwait window $f
  }
}

proc selectCh {ch} {
  global FEadcChX FEadcChN

  catch {destroy $ch.t3}
  if {$FEadcChN > 1} { 
    Frame $ch.t3
    Label $ch.t3.l3 -text "selected:"
    pack  $ch.t3  -padx 15 -pady 5 -fill x
    pack  $ch.t3.l3 -side top -anchor w

    set i 0
    for {set i 1} {$i <= $FEadcChN} {incr i} {
      Radiobutton $ch.t3.r$i -text "$i" -variable FEadcChX -value  "$i"
      pack $ch.t3.r$i -side left
    }
  }
}


proc readADC {fs {reload 0}} {
   global FEadcFile FEadcHeader FEadcFormat FEadcChX FEadcChN 
   global FEadcReadFrom FEadcReadTo FEadcSamplRate

   if {!$reload} {
     set localWavFile [FSBox "Select audio file:"]
     if {$localWavFile == ""} return
     .f.bt.b3.menu entryconfigure "Reload $FEadcFile" \
                   -label "Reload $localWavFile" 
     set FEadcFile $localWavFile
     .f.bt.b3.menu entryconfigure "Reload $FEadcFile" -state normal
     .f.bt.b3.menu entryconfigure "Save ADC"        -state normal
#     .fbt.b3.menu entryconfigure "Save marked"      -state normal

   }
   $fs readADC ADC $FEadcFile -hm $FEadcHeader -bm $FEadcFormat -chX $FEadcChX \
      -chN $FEadcChN -from ${FEadcReadFrom}s -to ${FEadcReadTo}s -sr $FEadcSamplRate
   refreshFeature $fs ADC
   .f.bt.b4 configure -state normal
   .f.bt.b5 configure -state normal
}

proc writeADC {fs {marked 0}} {
   set ADCfile [FSBox "Select audio file to save:"]
   if {$ADCfile == ""} return
   $fs writeADC ADC $ADCfile
}

#############################################################
#
#  Commands
#
#############################################################
proc FEcmd {fs function command source {par ""}} {

   if {$command == "spec-log"} {
     $fs spectrum spec              $source $par
     $fs log      $function  spec    1 1
     $fs delete spec
     refreshFeature $fs $function
     return
   }
   if {$command == "spec-aud-paud"} {
     $fs spectrum spec              $source $par
     $fs auditory    aud  spec
     $fs postaud     $function  aud
     $fs delete spec aud
     refreshFeature $fs $function
     return
   }
   if {$command == "echo"} {
     $fs shift       shift    ADC -delta 2000
     $fs add         $function  1.0 ADC 0.5 shift
     $fs delete shift
     refreshFeature $fs $function
     return
   }
   if {$command == "silence"} {
     $fs adc2pow   pow                   $source $par
     $fs peak      ptp                   $source $par
     $fs silTK     $function  pow ptp
     refreshFeature $fs $function
     return
   }
   eval "$fs $command  $function $source $par"
   refreshFeature $fs $function 
}



#############################################################
#
#  Main
#
#############################################################
#puts "defined procedure featview:"
#puts "USAGE: featview <featureset>"

proc featview { args } {
global FEadcFile FEadcFormat FEadcHeader FEadcChN FEadcChX
global FEadcReadFrom FEadcReadTo FEadcSamplRate 
global JANUSLIB

    if {[catch {itfParseArgv featview $args [ list [
        list "<FeatureSet>" object {} fs    FeatureSet "FeatureSet to use" ] ]
    } ] } {
	return
    }

catch "FeatureSet $fs"
set FEadcFile  ""
set FEadcFormat   "auto"
set FEadcHeader   "0"
set FEadcChN 1
set FEadcChX 1
set FEadcReadFrom 0
set FEadcReadTo   "end"
set FEadcSamplRate [$fs configure -samplingRate]

set title "featview 1.01"
wm title  . "$title"
catch {wm iconbitmap .  @$JANUSLIB/.featshow.xbm}


Frame       .f
set ti      [Frame .f.title ]
set bt      [Frame .f.bt    ]

Label       $ti.l1 -text "$title"

Button      $bt.b1 -text "Exit"           -command "destroy .; exit"
Button      $bt.b2 -text "Featshow"       -command "featshow $fs \"\""
Menubutton  $bt.b3 -text "File"           -menu    $bt.b3.menu
Menubutton  $bt.b4 -text "ADC"            -menu    $bt.b4.menu
Menubutton  $bt.b5 -text "ADC -> feature" -menu    $bt.b5.menu
$bt.b4 configure -state disabled
$bt.b5 configure -state disabled


 Menu $bt.b3.menu
#----------------#
  $bt.b3.menu  add command -label "Load ADC"         -command "readADC $fs"
  $bt.b3.menu  add command -label "Reload $FEadcFile"  -command "readADC $fs 1"
  $bt.b3.menu  add command -label "Settings load"    -command "formatADC $fs .f.ot"
  $bt.b3.menu  add separator
  $bt.b3.menu  add command -label "Save ADC"         -command "writeADC $fs"
#  $bt.b3.menu  add command -label "Save marked"      -command "writeADC $fs 1"
  $bt.b3.menu entryconfigure "Reload $FEadcFile" -state disabled
  $bt.b3.menu entryconfigure "Save ADC"        -state disabled
#  $bt.b3.menu entryconfigure "Save marked"     -state disabled

Menu $bt.b4.menu
#----------------#
  $bt.b4.menu  add command -label   "amplify"    \
                           -command "FEcmd $fs ADC add 2.0 { ADC 0.0 ADC    }"
  $bt.b4.menu  add command -label   "de-amplify" \
                           -command "FEcmd $fs ADC add 0.5 { ADC 0.0 ADC    }"
  $bt.b4.menu  add separator
  $bt.b4.menu  add command -label   "smooth"     \
                           -command "FEcmd $fs ADC filter ADC \
                                     { {-1 {0.33 0.33 0.33}}   }"
  $bt.b4.menu  add command -label   "Smooth"     \
                           -command "FEcmd $fs ADC filter ADC \
                { {-5 {0.04 0.08 0.12 0.16 0.20 0.16 0.12 0.08 0.04}}   }"
  $bt.b4.menu  add command -label   "emphasis"   \
                           -command "FEcmd $fs ADC filter ADC \
                                     { {0 {2 -1.8}}   }"
  $bt.b4.menu  add command -label   "de-emphasis"\
                           -command "FEcmd $fs ADC filter ADC { {0 {0.5} {-0.9}} }"
  $bt.b4.menu  add separator
  $bt.b4.menu  add command -label   "flip"       \
                           -command "FEcmd $fs ADC flip ADC"
  $bt.b4.menu  add command -label   "echo"       \
                           -command "FEcmd $fs ADC echo   ADC"
  $bt.b4.menu  add separator
  $bt.b4.menu  add command -label   "ADC -> ADC2"       \
                           -command "FEcmd $fs ADC2 SVector $fs:ADC.data"
  $bt.b4.menu  add command -label   "ADC -> ADC3"       \
                           -command "FEcmd $fs ADC3 SVector $fs:ADC.data"
  $bt.b4.menu  add command -label   "ADC2 -> ADC"       \
                           -command "FEcmd $fs ADC SVector $fs:ADC2.data"
  $bt.b4.menu  add command -label   "ADC3 -> ADC"       \
                           -command "FEcmd $fs ADC SVector $fs:ADC3.data"


Menu $bt.b5.menu
#----------------#
  $bt.b5.menu  add command -label   "power"       \
                           -command "FEcmd $fs power(ADC)    adc2pow  ADC 16ms"
  $bt.b5.menu  add command -label   "silence"     \
                           -command "FEcmd $fs silence(ADC)  silence  ADC 16ms"
  $bt.b5.menu  add command -label   "zerocrossing"\
                           -command "FEcmd $fs zero(ADC)     zero     ADC 16ms"
  $bt.b5.menu  add separator
  $bt.b5.menu  add command -label   "spectrum"    \
                           -command "FEcmd $fs logspec(ADC)  spec-log ADC 16ms"
  $bt.b5.menu  add command -label   "melscale"    \
                           -command "FEcmd $fs melscale(ADC) adc2mel  ADC 16ms"
  $bt.b5.menu  add command -label   "auditory"    \
                           -command "FEcmd $fs aud(ADC) spec-aud-paud ADC 16ms"
#  $bt.b5.menu  add command -label   "plp"         \
#                           -command "FEcmd $fs plp(ADC) plp   aud(ADC)"

pack .f      -fill both -expand true
pack $ti $bt -fill x    -expand true -side top
pack $ti.l1  -ipadx  5  -ipady 10    -side top
pack $bt.b1 $bt.b2 $bt.b3 $bt.b4 $bt.b5 \
             -padx  5 -pady  5 -ipadx  5 -ipady  5 -side left


}

