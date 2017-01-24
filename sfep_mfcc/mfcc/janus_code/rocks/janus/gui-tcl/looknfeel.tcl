############################################
# look and feel stuff
############################################
set mmcolors(button) ""
set mmcolors(back)   ""
#set mmcolors(button) "-background SkyBlue2"
#set mmcolors(back)   "-background LightSteelBlue"
  
proc Button {args} {
   global mmcolors
   lappend mmcolors(butlist) [lindex $args 0]
   eval "button $args -padx 3 -pady 3 $mmcolors(button)"
}
proc Menubutton {args} {
   global mmcolors
   lappend mmcolors(butlist) [lindex $args 0]
   eval "menubutton $args -padx 3 -pady 3  -relief raised $mmcolors(button)"
}
proc Menu {args} {
   global mmcolors
   lappend mmcolors(butlist) [lindex $args 0]
   eval "menu $args $mmcolors(button)"
}


proc Toplevel {args} {
   global mmcolors
   lappend mmcolors(bglist) [lindex $args 0]
   eval "toplevel $args -borderwidth 5 $mmcolors(back)"
}
proc Scale {args} {
   global mmcolors
   lappend mmcolors(bglist) [lindex $args 0]
   eval "scale $args $mmcolors(back)"
}
proc Label {args} {
   global mmcolors
   lappend mmcolors(bglist) [lindex $args 0]
   eval "label $args $mmcolors(back)"
}
proc Radiobutton {args} {
   global mmcolors
   lappend mmcolors(bglist) [lindex $args 0]
   eval "radiobutton $args $mmcolors(back)"
}
proc Entry {args} {
   global mmcolors
   lappend mmcolors(bglist) [lindex $args 0]
   eval "entry $args $mmcolors(back)"
}
proc Message {args} {
   global mmcolors
   lappend mmcolors(bglist) [lindex $args 0]
   eval "message $args $mmcolors(back)"
}
proc Frame {args} {
   global mmcolors
   lappend mmcolors(bglist) [lindex $args 0]
   eval "frame $args $mmcolors(back)"
}

################################################################
# use this procedure to reconfigure all "button" widgets
################################################################
proc ButtonConfig {color} {
   global mmcolors

   set mmcolors(button) "-background $color"
   foreach button $mmcolors(butlist) {
      eval "catch \{$button configure -bg $color\}"
   }  
}
################################################################
# use this procedure to reconfigure all "bg" widgets
################################################################
proc BgConfig {color} {
   global mmcolors

   set mmcolors(back) "-background $color"
   foreach bg $mmcolors(bglist) {
      eval "catch \{$bg configure -bg $color\}"
   }  
}

