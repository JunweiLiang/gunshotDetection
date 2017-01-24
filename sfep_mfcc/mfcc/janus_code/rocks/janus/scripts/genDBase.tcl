# ========================================================================
#  Janus-JRTk Janus Speech Recognition Toolkit
#             ------------------------------------------------------------
#             Object: Script to generate DBase
#             ------------------------------------------------------------
#
#  Author  :  Florian Metze
#  Module  :  genDBase.tcl
#  Date    :  $Id: genDBase.tcl 2390 2003-08-14 11:20:32Z fuegen $
#
#  Remarks :  adapt it to your own needs
#
# ========================================================================
#
#  $Log$
#  Revision 1.2  2003/08/14 11:19:43  fuegen
#  Merged changes on branch jtk-01-01-15-fms (jaguar -> ibis-013)
#
#  Revision 1.1.2.1  2002/11/05 10:16:09  metze
#  Initial revision
#
# ========================================================================

# Open the DBases
[DBase db-utt] open db-utt.dat db-utt.idx -mode rwc
[DBase db-spk] open db-spk.dat db-spk.idx -mode rwc

# Open the dictionary
set fp [open diktierISL.vocab.dict.all.islphones.tclstyle r]
while {[gets $fp line] != -1} {
    set w [lindex $line 0]
    set dict([string tolower $w]) $w
}
close $fp

# Open the preliminary file and read it
set uc 0
puts "Utterance DBase ..."
set fp [open pre-dbase r]
while {[gets $fp uttI] != -1} {
    makeArray uttA $uttI
    if {![info exists uttA(SPK)] || ![info exists uttA(UTT)] || ![info exists uttA(TEXT)]} {
	continue
    }

    # Analyze the data
    set spk [string tolower $uttA(SPK)]
    set utt [string range   $uttA(UTT) 1 end]
    set key ${spk}.${utt}
    set adc ${spk}/${utt}.adc
    set lbl ${spk}/${utt}.lbl

    # Check if it's ok
    if {![file exists adcs/$adc] || ![file exists adcs/$lbl]} { continue }

    # Get the speaker's sex
    set fq [open adcs/${spk}/${utt}.lbl r]
    gets $fq line
    close $fq
    set sex [lindex $line 0]
    
    # Check the pronunciation
    set text ""
    foreach w $uttA(TEXT) {
	if {[info exists dict($w)]} { set w $dict($w) } else { puts "OOV: $w" }
	set text "$text $w"
    }

    # Build the entry for the utterance DBase
    set line $key 
    lappend line "SPK $spk"
    lappend line "UTT $key"
    lappend line "SEX $sex"
    lappend line "ADC $adc"
    lappend line "TEXT $text"

    # Add it to the utterance DBase
    db-utt add $key $line

    # Add this key to the speaker's key
    lappend utts($spk) $key
    incr uc
}
close $fp

# Build the speaker DBase
puts "Speaker DBase ..."
foreach spk [array names utts] {
    db-spk add $spk "$spk {UTTS $utts($spk)}"
}

db-utt close
db-spk close

puts "[llength [array names utts]] speakers, $uc utterances."

exit
