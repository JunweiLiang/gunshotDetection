# convert mp4 to flac

set srcpath [lindex $argv 0]
set outpath [lindex $argv 1]
FeatureSet fs
foreach filename [exec find $srcpath -name "*.mp4"] {
    #puts $filename
    fs readADC ADC $filename
    if {![fs:ADC configure -sampleN]} {
        puts "  no samples"
        continue
    }
    fs resample ADC ADC 16
    set basename [ file tail $filename ]
    set outname $outpath/$basename
    regsub ".mp4" $outname ".wav"  out
    #regsub ".mp4" $file ".flac" flac
    fs writeADC ADC $out -hm WAV
    catch { printDo exec flac $out}
    #file delete $out
}
puts ok
exit
