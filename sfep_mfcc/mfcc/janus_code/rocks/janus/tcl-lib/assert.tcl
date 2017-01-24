# -----------------------------------------------------------------------------
#  assert
#
#  Like its C counterpart this procedure checks a condition the programer
#  assumes to be true and exits the program if not.
#  The condition should be in {} when assert is called so also variables
#  can be checked and displayed. Example:
#
#     assert {$i == 0} myfile #001
#  
#  Jul.1997 by Martin Westphal, westphal@ira.uka.de
# -----------------------------------------------------------------------------
proc assert {condition {name ""} {nr ""}} {
    global argv0
    if {"$name" == ""} {set name "$argv0"}
    if {[catch { 
	if {![uplevel "expr $condition"]} {
	    puts stderr "Tcl assertion failed in $name ($nr):"
	    puts stderr " condition was: $condition"
	    uplevel "puts stderr \" values       : $condition\""
	    exit -1
	} 
    } msg]} {
	puts stderr "Error while evaluating assertion in $name ($nr):"
	puts stderr " condition was: $condition"
	puts stderr " $msg"
	exit -1
    }
}
