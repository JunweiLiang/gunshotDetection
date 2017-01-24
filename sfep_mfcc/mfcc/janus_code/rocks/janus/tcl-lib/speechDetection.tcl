# ========================================================================
#  JANUS-SR   Janus Speech Recognition Toolkit
#             ------------------------------------------------------------
#
#  Author  :  Martin Westphal, westphal@ira.uka.de
#  Module  :  speechDetection.tcl
#  Date    :  Oct. 1999
#
#  Remarks :  speech detection and SNR estimation
#  
#  RCS:
#  $Id: speechDetection.tcl 572 2000-09-10 12:18:53Z hyu $
#  $Log$
#  Revision 1.1  2000/09/10 12:18:47  hyu
#  Added Tcl files from Hagen
#
#  Revision 1.1  1999/11/03 12:57:32  westphal
#  Initial revision
#
#
#
# ========================================================================

# ========================================================================
#  SNR
# ========================================================================

# ------------------------------------------------------------------------
# calculateSNR
# arguments are:
#   <featureSet name> <audio feature,SVector> <speech feature,FMatrix>
# return value:
#   Signal to Noise Ratio that is
#                   average speech power
#    SNR = 10 log10 --------------------
#                   average noise power
#   (Here we assume stationary noise)
# ------------------------------------------------------------------------
proc calculateSNR {fes audio speech {pause ""}} {
    $fes adc2pow _POW $audio 16ms

    FMatrix _mean
    $fes mean _mean _POW -weight $speech
    set signalPow [lindex [_mean] 0]

    if {"" == "$pause"} {
	$fes lin     _sil $speech -1 1
    } else {
	$fes lin     _sil $pause   1 0
    }
    $fes mean _mean _POW -weight _sil
    set noisePow [lindex [_mean] 0]

    set speechPow [expr $signalPow - $noisePow]
    puts [format "signal_power= %f, speech_power= %f, noise_power= %f" $signalPow $speechPow $noisePow]

    set speechLogPow [expr log10($speechPow)]
    set noiseLogPow  [expr log10($noisePow)]
    set snr [expr 10 * ($speechLogPow - $noiseLogPow)]
    eval $fes add _means $speechLogPow $speech $noiseLogPow _sil
    _mean destroy
    $fes delete _sil _POW

    puts "log(speech_power)= $speechLogPow, log(noise_power)= $noiseLogPow, snr= $snr"
    return $snr
}
# ------------------------------------------------------------------------
# calculateMeanSNR
# arguments are:
#   <featureSet name> <audio feature,SVector> <speech feature,FMatrix>
# return value:
#   Mean Signal to Noise Ratio 
#             
#    SNR = 10 (average log10 speech power - average log10 noise power)
# ------------------------------------------------------------------------
proc calculateMeanSNR {fes audio speech {pause ""}} {
    $fes adc2pow _POW $audio 16ms
    $fes alog    _lPOW _POW 1 10    ;# a=1 times log10() over b=10 decades

    FMatrix _mean
    $fes mean _mean _lPOW -weight $speech
    set speechMean [lindex [_mean] 0]

    if {"" == "$pause"} {
	$fes lin     _sil $speech -1 1
    } else {
	$fes lin     _sil $pause   1 0
    }
    $fes mean _mean _lPOW -weight _sil
    set noiseMean [lindex [_mean] 0]

    eval $fes add _means $speechMean $speech $noiseMean _sil; # <- here you can see very nicely which means are taken and where is speech/noise
    _mean destroy
    $fes delete _sil _POW ;# _lPOW

    set snr [expr 10 * ($speechMean - $noiseMean)]

    # add ---> speech proportion <--- here
    # ....

    puts "speech_log(power)= $speechMean, noise_log(power)= $noiseMean, mean-snr= $snr"
    return $snr
}
proc calculateModMeanSNR {MeanSNR} {
    return [expr 10.0 * log10(pow(10.0,0.1*$MeanSNR) - 1.0)]
}

# ------------------------------------------------------------------------
# calculateAvgSpec
# arguments are:
#   <featureSet name> <logSpectral feature,FMatrix> <speech feature,FMatrix>
# return value:
#   ASSpeech ASNoise
#             
# ------------------------------------------------------------------------
proc calculateAvgSpec {fes lspec speech {pause ""}} {
    $fes FMatrix _mean
    $fes FMatrix _dev
    set mf $fes:_mean.data
    set df $fes:_dev.data


    $fes lin  _lspecDB $lspec 10 0
    $fes mean $mf _lspecDB -weight $speech -dev $df
    set speechMean [lindex [$mf] 0]
    $fes add _mean 1 _mean 1 _dev
    set speechMeanH [lindex [$mf] 0]
    $fes add _mean 1 _mean -2 _dev
    set speechMeanL [lindex [$mf] 0]
    #puts "dev: [lindex [$df] 0]\nspH: $speechMeanH\nspM: $speechMean\nspL: $speechMeanL\n"

    if {"" == "$pause"} {
	$fes lin     _sil $speech -1 1
    } else {
	$fes lin     _sil $pause   1 0
    }
    $fes mean $mf _lspecDB -weight _sil -dev $df
    set noiseMean [lindex [$mf] 0]
    $fes add _mean 1 _mean 1 _dev
    set noiseMeanH [lindex [$mf] 0]
    $fes add _mean 1 _mean -2 _dev
    set noiseMeanL [lindex [$mf] 0]


    $fes delete _mean _dev
    $fes delete _sil _lspecDB

    set N [llength $speechMean]
    set total 0.0
    for {set i 0} {$i < $N} {incr i} {
	set total [expr $total + ([lindex $speechMean $i] - [lindex $noiseMean $i])]
    }
    set total [expr $total / $N]

    return [list $total $speechMeanL $speechMean $speechMeanH $noiseMeanL $noiseMean $noiseMeanH ]
}


# set ret [calculateAvgSpec featureSet$SID lMELN SPEECH]
# plotWindow .as [list [lindex $ret 0] {lines -fill black} [lindex $ret 1] {lines -fill red}] i logPow 0.0




# ========================================================================
#  Speech Detection
# ========================================================================

# ------------------------------------------------------------------------
# using log power threshold
# % speechDet-1 fs ADC "" lPOW 0       ;# gives you normalized lPOW
# % speechDet-1 fs ADC "" speech 0.25  ;# gives you hard speech detection
# Tip: Use either 'audio' feature or another feature 'fe' of your choice.
# If you don't use 'fe' the lopPower of 'audio' will be taken.
# ------------------------------------------------------------------------
proc speechDet-1 {fes audio {fe ""} {speech SPEECH} {val 0.2}} {
    if {"" == "$fe"} {
	$fes adc2pow _POW $audio 16ms
	$fes alog    _lPOW _POW 1 4    ;# a=1 times log10() over b=4 decades
	$fes filter  _lPOW _lPOW  {-2 {1 2 3 2 1}}
	$fes filter  _lPOW _lPOW  {-2 {1 2 3 2 1}}
	set fe _lPOW
	$fes delete _POW
    }
    $fes   normalize       $speech  $fe    -min 0 -max 1
    if {$val > 0} {
	$fes   thresh          $speech  $speech  1.0 $val upper
	$fes   thresh          $speech  $speech  0.0 $val lower 
    }
    #catch {$fes delete _lPOW}
    puts "--> $speech"
    return $speech
}
# ------------------------------------------------------------------------
# using 2-means clustering
# % speechDet-2 fs ADC "" speech 0.25
# -> _lPOW _means0 _thresh
# ------------------------------------------------------------------------
proc speechDet-2 {fes audio {fe ""} {speech SPEECH} {val 0.25}} {
    if {"" == "$fe"} {
	$fes adc2pow _POW $audio 16ms
	$fes alog    _lPOW _POW 1 4    ;# a=1 times log10() over b=10 decades
	set fe _lPOW
	$fes delete _POW
    }

    # 2-means
    set data $fes:$fe.data
    foreach {min max} [$data minmax] {}
    FMatrix _fm1 "{$min} {$max}"
    FVector _fv1 2
    _fm1 neuralGas $data -maxIter 6 -tempS 0 -counts _fv1 -init 0
    eval set m0 [format "%.4f" [lindex [_fm1] 0]]
    eval set m1 [format "%.4f" [lindex [_fm1] 1]]
    eval set c0 [format "%.0f" [lindex [_fv1] 0]]
    eval set c1 [format "%.0f" [lindex [_fv1] 1]]
    _fm1 destroy
    _fv1 destroy
    set N   [expr $c0 + $c1]
    set c0Prop [format %.1f [expr 100.0 * $c0 / $N]]
    set c1Prop [format %.1f [expr 100.0 * $c0 / $N]]
    set snr [expr 10 * ($m1 - $m0)] ;# only if we used a log10(power) feature
    set thresh [expr $val * ($m1 - $m0) + $m0]
    puts "2-means {N $N} {c0 $c0 ${c0Prop}%} {m0 $m0} {c1 $c1 ${c1Prop}%} {m1 $m1} {snr $snr} {thresh $thresh}"

    # speech feature
    $fes thresh $speech $fe     $m0 $thresh lower
    $fes thresh $speech $speech $m1 $thresh upper
    $fes normalize $speech $speech
    $fes lin     _sil $speech -1 1
    eval $fes add _means0 $m1 $speech $m0 _sil
    eval $fes lin _thresh _means 0.0 $thresh

    #catch {$fes delete _sil _lPOW}
    puts "--> $speech"
    return $speech
}




