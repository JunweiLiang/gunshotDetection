proc setVariable {name text {min ""} {max ""}} {
    global $name
    
    set t [toplevel .sV$name]
    wm title $t "set $name"
    label $t.l1 -text "$text"
    entry $t.e1 -relief flat -width 4 -textvariable $name
    button $t.b1 -text "+" -command  "\
	    set $name \[expr \$$name + 1\] ;\
	    if {[string length $max] && \$$name > $max} {set $name $max};\
	    if {[string length $min] && \$$name < $min} {set $name $min}"
    button $t.b2 -text "-" -command  "\
	    set $name \[expr \$$name - 1\];\
	    if {[string length $max] && \$$name > $max} {set $name $max};\
	    if {[string length $min] && \$$name < $min} {set $name $min}"
    button $t.b3 -text "close" -command "destroy $t"
    pack $t.l1 $t.e1 $t.b1 $t.b2 $t.b3 -side left
    return $t
}

proc selectObject {obj} {
    set type [$obj type]
    if [catch {set subObjects [$obj .]}] {
    }
    if [catch {set configureL [$obj configure]}] {
    }
    if [catch {set elements   [$obj :]}] {
    }

    puts "============================="
    puts "Object: $obj"
    puts "Type:   $type"
    if [info exists subObjects] {
	puts "Sub objects: $obj"
    }
    if [info exists configureL] {
	puts "Configure:   $configureL"
    }
#    if [info exists elements] {
#	puts "Elements:    $elements"
#    }
    puts "============================="

    regsub -all {\.} $obj {_} wn
    catch {destroy .so$wn}
    set top [toplevel .so$wn]
    wm title    $top  "janus object: $obj"
    wm iconname $top  "$obj"
    
    pack [label $top.name -text "Object: $obj" -anchor w] -fill x
    pack [label $top.type -text "Type: $type" -anchor w] -fill x
    set i 0
    if [info exists subObjects] {
	foreach sub $subObjects {
	    if {"$sub" != "list" && [string range $sub 0 4] != {item(} } {
		if [regexp {(.*)\(([0-9]*)\.\.([0-9]*)\)} "$sub" all name b1 b2] {
		    pack [button $top.$i -text "$sub" -command "selectObjectL $obj.$name $b1 $b2"] -fill x
		    incr i
		} else {
		    pack [button $top.$i -text "$sub" -command "selectObject $obj.$sub"] -fill x
		    incr i
		}
	    }
	}
    }
}
proc selectObjectL {obj b1 b2} {
    global var

    set var $b1
    if {"$b1" != "$b2"} {
	set text "Select one of $b1 .. $b2"
	tkwait window [setVariable var $text $b1 $b2]
    }
    selectObject ${obj}($var)
}

#return
set frame .f
pack [frame $frame]

scrollbar $frame.scroll -relief sunken -command "$frame.list yview"
listbox $frame.list -yscroll "$frame.scroll set" -relief sunken -setgrid 1
pack $frame.scroll -side right -fill y
pack $frame.list -side left -fill both ;# -expand yes 

eval "$frame.list insert 0 [objects]"
bind $frame <Enter> " $frame.list delete 0 end;\
	eval \"$frame.list insert 0 \[objects\]\" "
bind $frame.list <ButtonRelease-1> {
	selectObject [lindex [selection get] 0] }
