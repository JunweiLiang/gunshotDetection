#==================================================================================
# MAP Adaptation GMM trainer (allow Multiple segment label files for each class)
# the GMM for one class is created by adapt on the UBM model using training data
# Qin Jin
# 05/05/05 ISL (CMU)
#==================================================================================
puts "Start $argv0 $argv"
clock format [clock clicks]

#-----------------------------------------------------------------------------------
# reads a list file 
#-----------------------------------------------------------------------------------
proc readWorkFile {workFile} {
    set fp [open $workFile]
    set workL ""
    while {[gets $fp LINE] >= 0} {
        set LINE [string trim $LINE]
        if {$LINE == ""} {continue}
        lappend workL $LINE
    }
    close $fp
    return $workL
}

proc readClassDescFile {classDescFile} {
    set fp [open $classDescFile]
    set theClasses ""
    while {[gets $fp LINE] >= 0} {
	set LINE [string trim $LINE]
	if {$LINE == ""} {continue}
	lappend theClasses $LINE
    }
    close $fp
    return $theClasses
}

#-----------------------------------------------------------------------------------
# Default Settings
#-----------------------------------------------------------------------------------
set featName        FEAT 
set semFile        ""

# -----------------------------------------------------------------------
#  Read in command-line options (override settings)
# -----------------------------------------------------------------------
if [catch {itfParseArgv $argv0 $argv [list [
  list <listFile>    string  {} listFile            {} {file listing all the label files to process } ] [
  list -adcPath      string  {} adcPath             {} {adc path} ] [
  list -featDesc     string  {} featureDescFile     {} {feature description file} ] [
  list -featExt      string  {} featExt             {} {feature file extension name} ] [
  list -featName     string  {} featName            {} {feature name} ] [
  list -outPath      string  {} outPath             {} {output path} ] [
  list -touchFile    string  {} touchFile           {} {the file name that next Janus job need to wait} ] [
  list -semFile      string  {} semFile             {} {semaphore file}   ] ]
} ] {
    puts stderr "ERROR in $argv0 while scanning options"
    exit -1
}

waitFile ${semFile}

# -----------------------------------------------------------------------
# create FeatureSet Objects
# -----------------------------------------------------------------------
FeatureSet fes        ;# object to store computed features

# -----------------------------------------------------------------------
# read Feature Processing Description
# -----------------------------------------------------------------------
fes setDesc @$featureDescFile

condorInit dummy

while {[fgets $listFile workElem] != -1} {

    #   read audio segments and preprocess audio to features
    set ADCFILE     [lindex $workElem 0]
    #set ADCNAME     [lindex $workElem 1]
    set ADCNAME     [file rootname [file tail $ADCFILE]]
    set startTime   0
    set endTime     -1

    if {[catch {
	puts "$ADCFILE ..."
	    fes eval "{ADCFILE $ADCFILE} {FROM $startTime} {TO $endTime}"
	    set frameN [fes:$featName configure -frameN]
	    set outFile "${ADCNAME}.${featExt}"
	puts "===== $outFile $frameN [fes:ADC44 configure -samplingRate] ====="
	if {[file exists ${outPath}/${outFile}.bz2]} { puts "WARN $ADCFILE potential duplicate" }
	#fes:$featName.data csave "${outPath}/${outFile}.gz"
	if {[lindex [fes:ADC44.data minmax] 0] == [lindex [fes:ADC44.data minmax] 1]} { continue }
	set fp_out [open ${outPath}/${outFile} w]
	foreach row [fes:$featName.data] {
	    puts $fp_out [string trim $row]
	}
	close $fp_out
        catch { exec bzip2 ${outPath}/${outFile} }
    } msg]} {
        puts "ERROR $ADCFILE - $msg"
    }
}

exit
