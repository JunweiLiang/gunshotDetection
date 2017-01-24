# ========================================================================
#  tcl library for sun audio recordings
#
#  Author  :  westphal
#  Module  :  audio_record.tcl
#  Date    :  16 Oct 1997
#
#  Remarks : 
#
# ========================================================================

# ------------------------------------------------------------------------
#  global variables:
#
#  RecordStatus     0     nothing done yet
#                   1     recording
#                   2     recording done
#  RecordIn         mic   microphone
#                   line  line-in
#
set RecordStatus   0
if {![info exists RecordIn]} {set RecordIn mic}

# ------------------------------------------------------------------------
#  start the recording and return pid
# ------------------------------------------------------------------------
proc startRecording { file } {
    global RecordStatus RecordIn
    set cmd "exec audiorecord -v 50 -b 0 -m 0 -p $RecordIn -s 16000 -e linear -t 60.0 $file &"
    if {[catch {eval $cmd} msg]} {
	puts stderr "ERROR: starting recording to file $file failed"
	puts stderr "$msg"
	return -1
    }
    set RecordStatus 1
    puts stderr "started recording ($msg) from '$RecordIn' to file $file"
    return $msg
}

# ------------------------------------------------------------------------
#  stop the recording
# ------------------------------------------------------------------------
proc stopRecording { pid } {
    global RecordEnd
    if {[catch {exec kill $pid} msg]} {
	puts stderr "ERROR: couldn't stop recording ($pid)"
	puts stderr "$msg"
	return 1
    }
    set RecordStatus 2
    puts stderr "stopped recording ($pid)"
    return 0
}

# ------------------------------------------------------------------------
#  create a record button
# ------------------------------------------------------------------------
proc recordButton { {file "./record.adc"} {text "Record"} {button .rec} {todo ""}} {
    global RecordPID
    
    catch {destroy $button}
    set button [button $button -text $text -width 20 -height 3]
    pack $button

    set start "\
         $button configure -text recording -activebackground red -activeforeground white;\
         set RecordPID \[startRecording $file\]  \
    " 
    set stop  "\
         stopRecording \$RecordPID;              \
         $button configure -text $text -activebackground #ececec -activeforeground black -relief raise;\
         update;                                 \
         eval {$todo}                            \
    "
    # Bind events to record button: 
    #  recording while button is pressed
    #  stop recording when button is released
    bind $button <Button-1>        "$start"
    bind $button <ButtonRelease-1> "$stop"
}

# ------------------------------------------------------------------------
#  play an audio file
# ------------------------------------------------------------------------
proc playRecording { file {bg &}} {
    set cmd "exec audioplay $file $bg"
    if {[catch {eval $cmd} msg]} {
	puts stderr "ERROR: playing file $file failed"
	puts stderr "$msg"
	return -1
    }
    puts stderr "played file $file"
    return 0
}

# ------------------------------------------------------------------------
#  create a play button
# ------------------------------------------------------------------------
proc playButton { {file "./record.adc"} {text "Replay"} {button .play} {todo ""}} {
    
    catch {destroy $button}
    set button [button $button -text "$text" -width 20 -height 3]
    pack $button

    set start "\
         $button configure -text playing;\
         update;                         \
         playRecording $file {};         \
         $button configure -text {$text};  \
    " 
    set stop  "\
         update;                          \
         eval {$todo}                     \
    "
    # Bind events to play button: 
    bind $button <Button-1>        "$start"
    bind $button <ButtonRelease-1> "$stop"
}














