# ========================================================================
#  JANUS-SR   Janus Speech Recognition Toolkit
#             ------------------------------------------------------------
#
#  Author  :  Christian Fuegen
#  Module  :  update procedures for modalities
#  Date    :  07.01.99
#  $Log$
#  Revision 1.2  2000/09/10 12:18:52  hyu
#  Added Tcl files from Hagen
#
#  Revision 1.1  2000/04/18 07:04:12  soltau
#  Initial revision
#
#
# ========================================================================

# ===========================================================================
# ========              D E S C R I P T I O N                        ========
# ===========================================================================
# format : name  := modalityName
#          start := startFrameX
#          end   := endFrameX
# output : list of answers for each frameX
#          answers are: 0 := no
#                       1 := yes
# all things wich will be need by one of these procedures are stored in
# an array called updateA
#
# example: A call of 'testMod update 0 15' calls the linked update procedure
#          with 'update testMod 0 15'.
#          The update procedure must return a list of answers for the frames
#          0 to 15 inclusive, for example '{0 1 1 0 1 0 0 1 1 1 1 0 0 1 1 1}'



# ---------------------------------------------------------------------------
# modality    : speaking rate
# description : find out the speaking rate of a given record on base of
#               a phonem recognizer.
#               speaking rate can change over the sentence
# tagName     : FAST SLOW
# ---------------------------------------------------------------------------

proc updateSpkRate {name start end} {
    global updateA

    if { ![info exists updateA($name)] } {
	error "ERROR updateSpkRate"
    }

    set type      SPKRATE
    set threshold $updateA($name)

    makeArray modArr $updateA(modInfo)

    if { ![info exists modArr($type)] } {
	putsWarn "modality for $type not found, so extract"
#	set modArr($type) [extractModality $spk $utt $type]
	set modArr($type) -1.0
    }

    set answer 0
    set val    $modArr($type)

    if { $val < $threshold } { set answer 1 }

    unset modArr

    return $answer
}


# ---------------------------------------------------------------------------
# modality    : speaking rate (MRATE)
# description : find out the speaking rate of a given record on base of
#               a phonem recognizer.
#               speaking rate can change over the sentence
# tagName     : FAST SLOW
# ---------------------------------------------------------------------------

proc updateMRate {name start end} {
    global updateA

    if { ![info exists updateA($name)] } {
	error "ERROR updateSpkRate"
    }

    set type      MRATE
    set threshold $updateA($name)

    makeArray modArr $updateA(modInfo)

    if { ![info exists modArr($type)] } {
	putsWarn "modality for $type not found, so extract"
#	set modArr($type) [extractModality $spk $utt $type]
	set modArr($type) -1.0
    }

    set answer 0
    set val    $modArr($type)

    if { $val >= $threshold } { set answer 1 }

    unset modArr

    return $answer
}


# ---------------------------------------------------------------------------
# modality    : signal to noise ratio
# description : a low SNR will be defined as noisy with an answer of 1
#               assumes that the SNR is over the whole sentence the same
# tagName     : NOISY CLEAR
# updateMode  : UPD_NOT
# ---------------------------------------------------------------------------

proc updateSNR {name start end} {
    global updateA

    if { ![info exists updateA($name)] } {
	error "ERROR updateSNR"
    }

    set type      SNR
    set threshold $updateA($name)

    makeArray modArr $updateA(modInfo)

    if { ![info exists modArr($type)] } {
	putsWarn "modality for $type not found, so extract"
#	set modArr($type) [extractModality $spk $utt $type]
	set modArr($type) 0.0
    }

    # assumption
    set val    $modArr($type)
    set answer 0

    if { $val <= $threshold } { set answer 1 }

    unset modArr

    return $answer
}

# ---------------------------------------------------------------------------
# modality    : dialect
# description : this procedure determine the spicific dialect of a given speaker
#               assumes that the dialect is over the whole sentence the same
# tagName     : D_* (too much to list here)
# updateMode  : UPD_NOT
# ---------------------------------------------------------------------------

proc updateDialect {name start end} {
    global updateA

    set type DIALECT

    makeArray modArr $updateA(modInfo)

    if { ![info exists modArr($type)] } {
	putsWarn "modality for $type not found, so extract"
#	set modArr($type) [extractModality $spk $utt $type]
	set modArr($type) "."
    }

    set answer  0
    set dialect $modArr($type)
    regsub {d_} $name {} modName

    if { [info exists updateA($modName)] && ![info exists updateA(unsupervised)] } {
	if { [lsearch -exact $updateA($modName) $dialect] >= 0 } {
	    set answer 1
	}
    } else {
	if { ![string compare $modName $dialect] } {
	    set answer 1
	}
    }

    unset modArr

#    puts stderr "$dialect <> $modName => $answer"

    return $answer
}


# ---------------------------------------------------------------------------
# modality    : sex / gender
# description : male will be set to 1, female to 0
#               sex is over the whole sentence the same
# tagName     : MALE
# updateMode  : UPD_NOT
# ---------------------------------------------------------------------------

proc updateGender {name start end} {
    global updateA

    set type SEX

    makeArray modArr $updateA(modInfo)

    if { ![info exists modArr($type)] } {
	putsWarn "modality for $type not found, so extract"
#	set modArr($type) [extractModality $spk $utt $type]
	set modArr($type) "."
    }

    # assumption
    set val    $modArr($type)
    set answer 0
    set modSex [string tolower [string range $name 0 0]]

    if { ![string compare [string tolower $val] $modSex] } {
	set answer 1
    }

    unset modArr

    return $answer
}

# ---------------------------------------------------------------------------
# modality    : spontaneous speaking
# description : if the given sentence is spontaneous, answer 1
#               the spontaneity is over the whole sentence equal
# tagName     : SPONTANEOUS
# ---------------------------------------------------------------------------

proc updateSpontan {name start end} {
    global updateA

    if { ![info exists updateA($name)] } {
	error "ERROR updateSpkRate"
    }

    set threshold $updateA($name)
    set modLst    [getModalityForUpdate spontan $start $end]
    set n         [expr $end-$start+1]

    # assumption
    set answer 0

    if { [lindex $modLst 0] >= $threshold } {
	set answer 1
    }

    for {set i 0} {$i < $n} {incr i} {
	lappend res $answer
    }

    return $res
}

# ---------------------------------------------------------------------------
# modality    : crosstalk
# description : two speaker in one sentence?
#               crosstalk is over the whole sentence same
#               kann spezialisiert werden, indem die stelle des anderen sprechers
#               getagt wird.
# tagName     : 
# ---------------------------------------------------------------------------

proc updateXTalk {name start end} {
    global updateA

    set modLst    [getModalityForUpdate xtalk $start $end]
    set n         [expr $end-$start+1]

    # assumption
    set answer 0

    if { [lindex $modLst 0] } {
	set answer 1
    }

    for {set i 0} {$i < $n} {incr i} {
	lappend res $answer
    }

    return $res
}

# ---------------------------------------------------------------------------
# modality    : environment
# description : environment of recording (room style)
#               assumes that the environment is over the wohle sentence equal
# tagName     : ENVWRK
# updateMode  : UPD_NOT
# ---------------------------------------------------------------------------

proc updateEnviron {name start end} {
    global updateA

    set type ENVIRON

    makeArray modArr $updateA(modInfo)

    if { ![info exists modArr($type)] } {
	putsWarn "modality for $type not found, so extract"
#	set modArr($type) [extractModality $spk $utt $type]
	set modArr($type) "."
    }

    # assumption
    set val    $modArr($type)
    set answer 0

    if { ![string compare "gemeinsam_in_Labor_mit_Workstations" $val] } {
	set answer 1
    }

    unset modArr

    return $answer
}


# ---------------------------------------------------------------------------
# modality    : pitch frequency
# description : a low pitch frequency will be set to 1 (male)
# tagName     : LOW
# ---------------------------------------------------------------------------

proc updatePitch {name start end} {
    global updateA

}


# ---------------------------------------------------------------------------
# modality    : prosody
# description :
# tagName     : 
# ---------------------------------------------------------------------------

proc updateProsody {name start end} {
    global updateA

}

# ---------------------------------------------------------------------------
# modality    : stiles
# description :
# tagName     : 
# ---------------------------------------------------------------------------

proc updateStile {name start end} {
    global updateA

}

# ---------------------------------------------------------------------------
# modality    : city-<city>
# description : For each City another modality
# tagName     : CITY-<city>
# ---------------------------------------------------------------------------

proc updateCity {name start end} {
    global updateA

    makeArray uttArr $updateA(uttInfo)

    set city "\\-$uttArr(CITY)\$"
    set match [regexp $city $name]

    for {set i $start} {$i <= $end} {incr i} {
	lappend res $match
    }

    return $res
}



# ---------------------------------------------------------------------------
# modality    : dummy
# description : dummy updateProc for testing
# tagName     : DUMMY
# ---------------------------------------------------------------------------

proc updateDummy {name start end} {
    global updateA

    set lst [list 0 0 0 0 0 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 0 0 0 0 0]

    set j 0
    for {set i $start} {$i <= $end} {incr i} {

	if { $j == 30 } { set j 0 }

	lappend res [lindex $lst $j]

	incr j
    }

    return $res
}





# ===========================================================================
# getModalityForUpdate
# ===========================================================================

proc getModalityForUpdate { type start end {optionalLst {}} } {
    global updateA

    set uttInfo $updateA(uttInfo)
    makeArray infoArr $uttInfo

    set res     {}
    set spk     $infoArr(spk)
    set utt     $infoArr(utt)
    set db      $updateA(modDB)
    set modInfo [$db get $utt]

    makeArray modArr $modInfo

    if { ![info exists modArr($type)] } {
	putsWarn "modality for $type not found, so extract"
	set modArr($type) [extractModality $spk $utt $type]
    }

    if { ![string compare $type "dialect"] } {
	set dialect $modArr($type)
	set modName [lindex $optionalLst 0]

	regsub {d_} $modName {} modName

	set match 0
	if { [info exists updateA($modName)] } {
	    if { [lsearch -exact $updateA($modName) $dialect] >= 0 } {
		set match 1
	    }
	} else {
	    if { ![string compare $modName $dialect] } {
		set match 1
	    }
	}
	lappend res $match

    } elseif { ![string compare $type "spkrate"] } {
	set res {}
	for {set i $start} {$i <= $end} {incr i} {
	    lappend res $modArr($type)
	}

    } elseif { ![string compare $type "xtalk"] } {
	lappend res $modArr($type)

    } else {
	lappend res $modArr($type)

    }

    return $res
}



# ===========================================================================

proc roundTo { val round } {

    if { $val < 0 } { set neg -1 } else { set neg 1 }
    set val  [expr abs($val)]
    set n    [expr round($val/$round)]

    return [expr $neg * $n * $round]
}

# quasi kontinuierlich modelierte Modalitaet (Quantisierung)
proc updateSNROnly {name start end} {
    global updateA

    set round    5.0
    set type     SNR
    set answer   0

    makeArray modArr $updateA(modInfo)

    if { ![info exists modArr($type)] } {
	putsError "modality for $type not found"
	set modArr($type) 0.0
    }
    
    set val [roundTo $modArr($type) $round]
    set vgl [string range $name 3 end]

    if { [regexp {_} $vgl] } {
	regsub {_} $vgl { } vgl
	set vgl1 [lindex $vgl 0]
	set vgl2 [lindex $vgl 1]
	
	if { $val >= $vgl1 && $vgl <= $vgl2 } { set answer 1 }
	if { $vgl1 == -10 && $val < $vgl1 } { set answer 1 }
	if { $vgl2 ==  60 && $val > $vgl2 } { set answer 1 }

	unset modArr
	return $answer
    }

    if { $vgl == $val } { set answer 1 }
    if { $vgl == -10 && $val < $vgl } { set answer 1 }
    if { $vgl ==  60 && $val > $vgl } { set answer 1 }

    unset modArr
    
    return $answer
}

# for manually added noise to an utterance
proc updateNOISE {name start end} {
    global updateA
    global addNoise

    return $addNoise
}

# quasi kontinuierlich modelierte Modalitaet (Quantisierung)
proc updateSpkRateOnly {name start end} {
    global updateA

    set round  0.2
    set type   SPKRATE
    set answer 0

    makeArray modArr $updateA(modInfo)

    set val [roundTo $modArr($type) $round]
    if { [string first "-" $name] >= 0 } {
	set vgl "[string range $name 3 4].[string range $name 5 end]"
    } else {
	set vgl "[string range $name 3 3].[string range $name 4 end]"
    }

    if { $vgl == $val } { set answer 1 }
    if { $vgl == -0.2 && $val < $vgl } { set answer 1 }
    if { $vgl ==  3.0 && $val > $vgl } { set answer 1 }

    unset modArr

    return $answer
}

# quasi kontinuierlich modelierte Modalitaet (Quantisierung)
proc updateMrateOnly {name start end} {
    global updateA

    set round  0.5
    set type   MRATE
    set answer 0

    makeArray modArr $updateA(modInfo)

    set val [roundTo $modArr($type) $round]
    if { [string first "-" $name] >= 0 } {
	set vgl "[string range $name 1 2].[string range $name 3 end]"
    } else {
	set vgl "[string range $name 1 1].[string range $name 2 end]"
    }

    if { $vgl == $val } { set answer 1 }
    if { $vgl == -0.5 && $val < $vgl } { set answer 1 }
    if { $vgl ==  7.0 && $val > $vgl } { set answer 1 }

    unset modArr

    return $answer
}


proc updateMrateMA {name start end} {
    global updateA

    set round  0.5
    set type   MRATE
    set answer 0

    makeArray modArr $updateA(modInfo)

    set val [expr 10 * [roundTo $modArr($type) $round]]
    set vgl [string range $name 1 end]

    if { [regexp {_} $vgl] } {
	regsub {_} $vgl { } vgl
	set vgl1 [lindex $vgl 0]
	set vgl2 [lindex $vgl 1]
	
	if { $val >= $vgl1 && $vgl <= $vgl2 } { set answer 1 }
	if { $vgl1 == 15 && $val < $vgl1 } { set answer 1 }
	if { $vgl2 == 70 && $val > $vgl2 } { set answer 1 }

	unset modArr
	return $answer
    }

    if { $vgl == $val } { set answer 1 }
    if { $vgl == 15 && $val < $vgl } { set answer 1 }
    if { $vgl == 70 && $val > $vgl } { set answer 1 }

    unset modArr
    
    return $answer
}
    