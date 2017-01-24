#!/home/yct/bin/janus.lda1
# ========================================================================
#  JanusRTK   Janus Speech Recognition Toolkit
#             ------------------------------------------------------------
#             Object: LSA training
#             ------------------------------------------------------------
#
#  Author  :  Wilson Tam
#  Module  :  train_lsa.tcl
#  Date    :  $Id: train_lsa.tcl,v 0.1 2005/07/03
#
#  Remarks :  
#
# ========================================================================


# ------------------------------------------------------------------------
#  Settings for train
# ------------------------------------------------------------------------
set SID              LSA
set spkLst           train.lst
set numClass         200
set eldaModel        model.N$numClass
set eldaVocab        train.lex
set semFile          ""

set beg              1
set end              10
set max_buffer       6000
set use_kl           0
set dirtree          0
set learn            0.5

# -----------------------------------------------------------------------
#  Read in command-line options (override settings)
# -----------------------------------------------------------------------
if [catch {itfParseArgv $argv0 $argv [list [
    list spkLst       string  {} spkLst               {} {train file list} ] [
    list -i           string  {} eldaModel            {} {input model file} ] [
    list -v           string  {} eldaVocab            {} {vocab file} ] [
    list -semFile     string  {} semFile              {} {semaphore file} ] [
    list -N           int     {} numClass             {} {Number of topics} ] [
    list -kl          int     {} use_kl               {} {Use KL wgt? 0/1} ] [
    list -begin       int     {} beg                  {} {first iteration} ] [
    list -dirtree     int     {} dirtree              {} {use dirtree? 0/1} ] [
    list -learn       string  {} learn                {} {learning rate 4 alpha?} ] [
    list -end         int     {} end                  {} {last iteration} ] ]
} ] {
    puts stderr "ERROR in $argv0 while scanning options"
    exit -1
}

waitFile "$semFile"

#if {$codebookSetParam == ""} {set codebookSetParam [glob -nocomplain Weights/[expr $beg-1].cbs.gz]}
#if {$distribSetParam  == ""} {set distribSetParam  [glob -nocomplain Weights/[expr $beg-1].dss.gz]}

foreach i "numClass eldaModel eldaVocab" {
    if {![info exists $i] || [set $i] == ""} { eval "set $i \$${SID}($i)" }
}
#eval "set labelPath \[string range \$${SID}(labelPath) 0 end\]"

# create output directory
file mkdir Accus
file mkdir Model

# ------------------------------------------------------------------------
#  Init Modules
# ------------------------------------------------------------------------

#LSAInit $SID
if {$dirtree} {
   printDo LSA lsa${SID} $eldaModel $eldaVocab $numClass -s $max_buffer -kl $use_kl -t 1
} else {
   printDo LSA lsa${SID} $eldaModel $eldaVocab $numClass -s $max_buffer -kl $use_kl
}

# --------------------------------------------------------------------
#  doAccu
# --------------------------------------------------------------------
proc doAccu {spkLst i } {
    global SID env numClass beg
    global $SID ;# HACK to make findLabelWarp happy

    puts "doAccu $spkLst $i"

    # clear
    lsa${SID}  clearAccus

    while {[fgets $spkLst train_file] >= 0} {
        # accumulate statistics
        printDo lsa${SID} accu $train_file
    }

    # dump accumulators
    lsa${SID} writeAcc Accus/${i}.N$numClass.$env(HOST).[pid]

    # remove previous acc
    if {$i > $beg} {
       set prv_i [expr $i - 1]
       file delete Accus/${prv_i}.N$numClass.$env(HOST).[pid].betas
       file delete Accus/${prv_i}.N$numClass.$env(HOST).[pid].gammas
    }
}


# --------------------------------------------------------------------
#  main loop
# --------------------------------------------------------------------
printDo lsa${SID} createAccus

if {![file exists $spkLst.$beg]} { file copy $spkLst $spkLst.$beg }

set prv_kl_sum 0.0
set kl_sum 0.0

for {set i $beg} {$i <= $end} {incr i} {   

    doParallel {
	printDo doAccu $spkLst.${i} ${i}
    } {
        # code executed by server

	printDo lsa${SID} clearAccus

        # read all acc
	foreach accfile [glob Accus/${i}.*.betas] { 
           regsub -all {\.betas$} $accfile {} accfile
           printDo lsa${SID} addAccus $accfile 
        }
    
        # update model
	set kl_sum [lsa${SID} update -learn $learn]
        puts stderr "kl = $kl_sum"
    
	# save model
	printDo lsa${SID} save Model/${i}.N$numClass

        # debug: dump accumulators again
        #printDo lsa${SID} writeAcc Accus/${i}.N$numClass.$env(HOST).[pid].combine

        # remove previous acc + model to save space
        if {$i > $beg} {
           set prv_i [expr $i - 1]

           # only save models every 10 iterations
           if {[expr $prv_i % 10] != 0} {
	      file delete Model/${prv_i}.N${numClass}.alpha
	      file delete Model/${prv_i}.N${numClass}.beta
           }
           # zip previous model to save space
	   #eval "exec gzip Model/${prv_i}.N${numClass}.alpha"
	   #eval "exec gzip Model/${prv_i}.N${numClass}.beta"

           if {$prv_kl_sum > $kl_sum} {
              puts stderr "KL sum decreases: $kl_sum < $prv_kl_sum"
           }
        }

        set prv_kl_sum $kl_sum

	# concatenate warping factor
#	if $warpON {
#	    set flist [glob "Warps/$i.*.warp"]
#	    eval "exec cat $flist > Warps/$i.warp"
#	    foreach f $flist {catch { rm $f} }		
#	}

	file copy $spkLst $spkLst.[expr $i + 1]

        # clear temp files and set we are DONE
	if {$i == $end} {
	    set flist [glob -nocomplain "$spkLst.*"]
	    foreach f $flist { catch { rm $f } }
	    set flist [glob -nocomplain "*.DONE"]
	    foreach f $flist { catch { rm $f } }
	    touch train.DONE
	}

    } { } {
        # code executed by clients

        # load previous model
	if {$i < $end} {
	    printDo lsa${SID} load Model/${i}.N$numClass
	}
    }
}

exit
