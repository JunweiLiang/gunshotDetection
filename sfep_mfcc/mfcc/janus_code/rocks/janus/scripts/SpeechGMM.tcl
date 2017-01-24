# ========================================================================
# routines required by a particle filter
# ------------------------------------------------------------------------
#
#  Authors :  Matthias Woelfel
#  Module  :  SpeechGMM.tcl (to be called by featDesc.mvdr.PF)
#  Date    :  12/09/2008
# ========================================================================

global SID AM AMN amPar pathPFAM
set AM  AM0     ;# the name of the clean acoustic model
set AMN AM1     ;# the name of the nose  acoustic model
set amRoot  "."
set amDesc  $amRoot
set amParam $amRoot
global $AM $AMN

set ${AM}(codebookSetDesc)  ${pathPFAM}/final.cbsDesc.gz
set ${AM}(codebookSetParam) ${pathPFAM}/final.cbs.gz
set ${AM}(distribSetDesc)   ${pathPFAM}/final.dssDesc.gz
set ${AM}(distribSetParam)  ${pathPFAM}/final.dss.gz

# -----------------------------------------
#  other settings
# -----------------------------------------
set amPar(class)         ALL       ;# class name of models
set amPar(Input,fe)      SPEC      ;# the noisy feature
set amPar(Noise,fe)      NSPEC     ;# noise samples
set amPar(Noise,dim)     20        ;# noise sample dimension
set amPar(Noise,type)    DIAGONAL

# -----------------------------------------
#  procedures
# -----------------------------------------
proc initAM {SID fes AM AMN warp} {
    global $AM $AMN amPar

    # --- clean AM ---
    codebookSetInit $AM  -featureSet featureSet$SID
    distribSetInit  $AM

    set cbs codebookSet$AM
    set dss distribSet$AM

    # --- unimodal noise model ---
    codebookSet$AM add noise $amPar(Noise,fe) 1 $amPar(Noise,dim) $amPar(Noise,type) 
    codebookSet$AM:noise createAccu
    distribSet$AM add noise noise

    codebookSet$AM add fnoise diffSPEC 1 $amPar(Noise,dim) $amPar(Noise,type)
    codebookSet$AM:fnoise createAccu
    distribSet$AM add fnoise fnoise

    if { [llength [objects FMatrix LPnoise]] != 1} {
	FMatrix LPnoise 40 40
    }

    set var "10.0"
    set varR "0.001"
    if { [llength [objects FMatrix $fes:PREDICTVAR]] != 1} {
        $fes FMatrix PREDICTVAR
        $fes:PREDICTVAR.data := "
	$var $var $var $var $var $var $var $var $var $var $var $var $var $var $var $var $var $var $var $var
	$varR $varR $varR $varR $varR $varR $varR $varR $varR $varR $varR $varR $varR $varR $varR $varR $varR $varR $varR $varR
	"
        
    }
}

proc copyAM {AMto AMfrom classto classfrom} {
    codebookSet$AMto:$classto := codebookSet$AMfrom:$classfrom
    distribSet$AMto:$classto := distribSet$AMfrom:$classfrom
}

# procedure to train a noise codebook
proc trainCB {dss cbs class} {
    set fe [$cbs.featureSet name [$cbs:$class configure -featX]]
    set frameN [$cbs.featureSet : $fe configure -frameN]
    $dss clearAccus
    $cbs clearAccus
    for {set i 0} {$i < $frameN} {incr i} {
        $dss accuFrame $class $i
    }
    $dss update
    puts "trained new $class model"

    $cbs:$class.cov(0) 

    set m [lindex [$cbs:$class.mat] 0]
    set v [lindex [$cbs:$class.cov(0)] 0]
    
    puts "INFO new cb $class\n   mean=$m\n   var=$v"
}

# subtract noise codebooks
proc subtractCB {dss cbs class1 class2} {
    set mean_values "{"
    
    set m1 [lindex [$cbs:$class1.mat] 0]
    set m2 [lindex [$cbs:$class2.mat] 0]
    
    set count 0
    foreach m $m1 {
        set temp [expr pow(10.0,0.1*[lindex $m1 $count])-pow(10.0,0.1*[lindex $m2 $count])]
        if { $temp < 10.0 } { set temp 10.0 }
        set temp [expr 10.0*log10($temp)]        
        incr count
        set mean_values "$mean_values $temp"
    }
    set mean_values "$mean_values }"

    $cbs:$class1 set $mean_values

    set m [lindex [$cbs:$class1.mat] 0]
    set v [lindex [$cbs:$class1.cov(0)] 0]
        
    puts "INFO new cb $class1\n   mean=$m\n   var=$v"
}
