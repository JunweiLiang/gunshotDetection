#######################################################################
# Setting
#######################################################################
set num_class 4
set vocab_file train.id
set model out.net
#######################################################################

source /nfs/paprika9/yct/ibis-new/tcl-lib/ch_lib.tcl
source /nfs/paprika9/yct/ibis-new/tcl-lib/elda.tcl

proc get_prob {log10_p} {
   set lp [expr log(10) * -$log10_p]

   set p [expr exp($lp)]

   return $p
}

# init lda
printDo LSA lda $model $vocab_file $num_class

for {set k 0} {$k < $num_class} {incr k} {
   set w [lda nbest_w $k -cumu 0.3]
   set ww [lrange $w 0 end-1]
   set total [lindex $w end]

   puts "$k: [int2char $ww] $total"
}

#foreach item [lda nbest_kl -kl 1] {
#   set w   [lindex $item 0]
#   set val [lindex $item 1]
#   puts "[int2char $w] $val"
#}
