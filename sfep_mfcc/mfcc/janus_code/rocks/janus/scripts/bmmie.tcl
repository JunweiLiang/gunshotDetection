# ========================================================================
# SWB/IBIS : adapted from eval6/test.tcl
#
# Author: 
#
# $Log$
# ========================================================================

#source /nfs/sunflower0/hyu/BN/tcl/ibis.tcl
source /afs/cs.cmu.edu/user/wrhsiao/janus_scripts/transtac/decode_scripts/ibis_hyu.tcl
source /afs/cs.cmu.edu/user/wrhsiao/janus_scripts/gale_decode/decode_scripts/dbase.tcl
source /afs/cs.cmu.edu/user/wrhsiao/janus_scripts/standard/decode_scripts/train.tcl
source /afs/cs.cmu.edu/user/wrhsiao/janus_scripts/standard/decode_scripts/mymisc.tcl
#source /nfs/islpc1_0/yct/Software/janus1/decode_scripts/dbase.tcl
#source /nfs/islpc1_0/yct/Software/janus1/decode_scripts/train.tcl
#source /nfs/islpc1_0/yct/Software/janus1/decode_scripts/mymisc.tcl

#source desc.tcl
#source lmdesc.tcl
set desc "desc.tcl"
set lmdesc "lmdesc.tcl"

source /afs/cs.cmu.edu/user/wrhsiao/janus_scripts/transtac/decode_scripts/common.tcl
#source /nfs/islpc1_0/yct/Software/janus1/decode_scripts/common.tcl

set accuPath    Accus
set semFile     ""
set convLst     spkLst.gdfsa
set latDir      lattices.new
set meanPath    means
set infoPath    Info
set beg 1
set end 4

set dbase        ../dbase/rt034
set ADCPATH      ../../ADCs
set hypoDir     hypo
set meanPath    means
set warpFile    warpFactors
set semFile     ""
set featDesc    ""
set frameShift  10
set nIter       2
set gIter       4
set minCount    1500

set mode 0
set thresh 0.00

set E     2.0
set H     1.0
set II   100.0
set lz   40
set accu_max_count 150

set confDir     $hypoDir
set sasDir      FSA
set signalAdapt 1
set doMLLR      0

# Wilson 7 Sept 04
#set streeDump   stree$SID.gz
set streeDump   ""
# END

set mb      2.0
set transN 40
set morphN 10

# base lz/lp
set lz 40
set lp  4
set sp 40

# final lz/lp
set lz2 40
set lp2  4

# rescoring matrix
set llz {30 34 38 42 46 50}
set llp {0 4 8 10 12 14 16}

set useExpT  0
set useTopN1 0 
set nbest    1
set scoreFct base

set scale 0.1
set num_classes 256

if {! $tcl_interactive} {
  if [catch {itfParseArgv $argv0 $argv [list \
    [list <convLst>    string {} convLst        {} {list of clusters}] \
    [list -dbase        string {} dbase          {} {database} ] \
    [list -semFile      string {} semFile        {} {semaphore file to wait for}
 ] \
    [list -hypoDir      string {} hypoDir        {} {hypo dir} ] \
    [list -latIn        string {} latIn          {} {rescore/decode} ] \
    [list -wghtPath     string {} wghtPath       {} {wght dir} ] \
    [list -mllr         string {} doMLLR         {} {mllr? 0/1} ] \
    [list -signalAdapt  string {} signalAdapt    {} {signalAdapt? 0/1} ] \
    [list -ADCPATH      string {} ADCPATH        {} {adc path} ] \
    [list -featDesc     string {} featDesc       {} {featDesc} ] \
    [list -desc         string {} desc           {} {desc.tcl} ] \
    [list -frameShift   string {} frameShift     {} {frameshift default=10ms} ] \
    [list -useExpT      int    {} useExpT        {} {0/1=use speedup Gaussian evaluation} ] \
    [list -useTopN1     int    {} useTopN1       {} {0/1=use speedup Gaussian evaluation by using the best Gaussian} ] \
    [list -transN       int    {} transN         {} {param (speedup)} ] \
    [list -morphN       int    {} morphN         {} {param (speedup)} ] \
    [list -nbest        int    {} nbest          {} {# of nbest hypos} ] \
    [list -lz           int    {} lz             {} {lz} ] \
    [list -scale        float  {} scale             {} {lz} ] \
    [list -num_classes  int    {} num_classes    {} {num_classes} ] \
    [list -scoreFct     string {} scoreFct       {} {scoreFct type (speedup)} ] \
    [list -mbeam        string {} mb             {} {masterbeam. default=2.0} ] \
    [list -lmdesc       string {} lmdesc         {} {lmdesc.tcl} ] \
    ]}] {
    exit 1
   }
}

source $desc
source $lmdesc

if {[string first "/" $dbase] >= 0} {
    set GSID(dbasePath) [file dirname $dbase]
    set dbase           [file tail    $dbase]
}

if {$signalAdapt && $sasDir != "" && ![file isdirectory $sasDir]} { 
    printDo file mkdir $sasDir  
}
waitFile $semFile

# ------------------------------------------------------------------------
#  init modules
# ------------------------------------------------------------------------

phonesSetInit   $SID
tagsInit        $SID
featureSetInit  $SID 
codebookSetInit $SID
distribSetInit  $SID
distribTreeInit $SID 
senoneSetInit   $SID  distribStream$SID
topoSetInit     $SID
ttreeInit       $SID
if {[info exists streeDump] && [file exists $streeDump]} {
    dictInit        $SID -desc ""
} else {
    dictInit        $SID
}
trainInit       $SID
dbaseInit       $SID $dbase -path $GSID(dbasePath)

TextGraph textGraphTmp
mkdir $accuPath

# speedup Gaussian evaluation
if {$useExpT} {
   puts "INFO: codebookSet$SID set -expT -10 (faster setup)"
   codebookSet$SID set -expT -10
}

# speedup Gaussian evaluation
if {$useTopN1} {
   puts "INFO: codebookSet$SID set -topN 1 (faster setup)"
   codebookSet$SID set -topN 1
}


printDo featureSet$SID configure -frameShift $frameShift
set lz  [expr int($lz  * 10.0 / $frameShift)]
set lz2 [expr int($lz2 * 10.0 / $frameShift)]
puts stderr "lz2 = $lz2, lp2 = $lp2"
set rate [expr 1000.0 / [featureSet$SID configure -frameShift]]
#senoneSet$SID setScoreFct base
senoneSet$SID setScoreFct $scoreFct

if {$signalAdapt && $sasDir != ""} {
  SignalAdapt SignalAdapt${SID} senoneSet$SID -maxTran 2
  SignalAdapt${SID} configure -topN 1 -shift 1.0
  foreach ds [distribSet$SID] { SignalAdapt${SID} add $ds }
  set sas SignalAdapt${SID}
}

[LingKS lks$SID NGramLM] load $GSID(lmDesc)
SVocab svocab$SID dict$SID
SVMap  svmap$SID  svocab$SID lks$SID

foreach w [dict$SID] {
  svocab$SID add $w
}

svocab$SID add {$}                -ftag 1
svocab$SID add {SIL}         -ftag 1

svmap$SID map base

Aegis aegis codebookSet${SID} distribSet${SID} phonesSet${SID} distribTree${SID} dict${SID} -threads 1
printDo aegis initMPE svmap$SID

svmap$SID   configure -wordPen $lp -filPen $sp -lz $lz

codebookSet${SID} createAccus -subN 3
distribSet${SID}  createAccus -subN 3

codebookSet${SID}:SIL-m.cfg configure -E $E -H $H -I $II -method c

Path pathML$SID
Path pathMPEDen$SID
Path pathNumC$SID
Path pathDenC$SID
Path pathTmpP$SID

proc hmmMakeUtterance { hmm args } {
  set optWord   \$
  set optPen     0.0
  set variants   0

  itfParseArgv hmmMakeUtterance $args [list [
  list <text>    string  {} text     {} {transcription}         ] [
  list -optWord  string  {} optWord  {} {optional word}         ] [
  list -optPen   float   {} optPen  {}  {optional word penalty} ] [
  list -variants int     {} variants {} {variants 0/1}          ] ]

  textGraphTmp make "( $text )" -optWord $optWord

  array set  HMM [textGraphTmp]
  set words $HMM(STATES)
  set trans $HMM(TRANS)
  set init  $HMM(INIT)

#  $hmm configure -rcmSdp 1
  if [catch {$hmm make $words -trans $trans \
    -init $init -variants $variants} msg] {
    puts "ERROR $hmm make $args\n$msg"
  }
  return $words
}

proc viterbiUtterance { LSID args } {
  upvar \#0 $LSID SID
  global USE_MODALITY

  if { [info exists USE_MODALITY] && ${USE_MODALITY} == 1} {
    set score [eval "modViterbiUtterance $LSID $args"]
    return $score
  }

  set hmm         hmm$LSID
  set path        path$LSID
  set lbox        lbox$LSID

  set optWord  \$
  set variants  0
  set beam      300
  set topN      0

  if [info exist SID(TRL)] { set trl $SID(TRL) } else { set trl TEXT }

# default: dont use cleanup of backpointer during viterbi
  set bpMod       -1
  set bpMul        3

  itfParseArgv viterbiUtterance $args [list [
  list <speaker> string  {} spk      {} {speaker   ID}  ] [
  list <uttID>   string  {} utt      {} {utterance ID}  ] [
  list -text     string  {} text     {} {text to align} ] [
  list -hmm      string  {} hmm      {} {name of hmm}   ] [
  list -path     string  {} path     {} {name of path}     ] [
  list -lbox     string  {} lbox     {} {name of Labelbox} ] [
  list -beam     string  {} beam     {} {viterbi beam}  ] [
  list -topN     string  {} topN     {} {topN beam}     ] [
  list -bpMod    int     {} bpMod    {} {after every X frames clean up bpTable (<0 never)}      ] [
  list -bpMul    int     {} bpMul    {} {go Y * X frames back during cleanup (<1 start at first frame)} ] [
  list -optWord  string  {} optWord  {} {optional word} ] [
  list -variants int     {} variants {} {variants 0/1}  ] ]

  set       uttInfo  [uttDB uttInfo $spk $utt]
  makeArray uttArray $uttInfo
  #set uttArray(TEXT) [string toupper $uttArray(TEXT)]

  if {! [info exist text]} { set text $uttArray($trl) }

  if [catch {$hmm makeUtterance $text  -optWord $optWord\
    -variants $variants} msg] {
    puts "ERROR $hmm makeUtterance $text\n$msg"
  }
  if [catch {$path viterbi $hmm -eval $uttInfo -beam $beam\
    -topN $topN -bpMod $bpMod -bpMul $bpMul} score] {
      puts "ERROR $utt: $path viterbi $hmm -beam $beam -topN $topN\n$score"
    }
  set ref [$path words $hmm]
  return $score
}

proc computeNumScore {glat spk utt {gamma 1.0} } {
  global SID optWord variants oldUtt oldRes lz

  glat$SID clear
  set refNodeLst [addRefToGLat glat$SID $spk $utt]

  if { ![llength $refNodeLst] } {
    puts "ERROR at accuMPEStats $spk $utt"
    return 0.0
  }
  set flag 0
  foreach item $refNodeLst {
    if { $flag == 1 } {
      set refNodeLstStr "${refNodeLstStr}_${item}"
    } else {
      set refNodeLstStr $item
      set flag 1
    }
  }

  regsub -all " " $refNodeLstStr "_" refNodeLstStr
 
  if [catch {glat$SID initAddon hmm$SID pathTmpP$SID -refNodeList $refNodeLstStr -mpe 1 -mode 0 -verbose 0} msg] {
    puts "ERROR at initAddon $spk $utt\n$msg"
    glat$SID clearAddon
    return 0.0
  }

  set num_score [glat$SID posteriori -scale [expr 1.0 / $lz]]

  printDo glat$SID clearAddon

  return $num_score
}

proc addRefToGLat {glat spk utt {gamma 1.0} } {
  global SID optWord variants oldUtt oldRes lz

  set score ""
  if [catch {set score [viterbiUtterance $SID $spk $utt -beam 500 -topN 0 -optWord $optWord -variants $variants]} msg] {
    puts "ERROR at addRefToGLat $spk $utt : $msg"
    return ""
  }

  if {![llength $score]} {
    puts "ERROR at addRefToGLat $spk $utt : $score"
    return ""
  }

  set ref [path$SID words hmm$SID]

  regsub  ".*\{\\( \[0-9\]+" $ref  "\{( 0" ref2
  regexp  "\{\\).*" $ref tmp
  regsub  " \{\\).*"         $ref2 ""     ref3
  set ref "$ref3 \{) [lindex [lindex $tmp 0] 1] [lindex [lindex $tmp end] 2]\}"

  set oldUtt $utt
  set oldRes $ref
  set lastNode -1
  regsub {\{\$ ([0-9]+) ([0-9]+)\} \{\) ([0-9]+) ([0-9]+)\}$} $ref {{) \1 \4}} tmp
  set ref $tmp

  foreach word $ref {
    if { [svocab$SID index [lindex $word 0]] < 0} {
      puts "ERROR at addRefToGLat $spk $utt : $word not found"
        return ""
    }
  }
  set refNodeList ""
  foreach word $ref {
    set score [path$SID lscore hmm$SID -from [lindex $word 1] -to [lindex $word 2]]
    set newNode [$glat addNode [lindex $word 0] [lindex $word 1] [lindex $word 2] -score $score -gamma $gamma]
    $glat addLink $lastNode $newNode
    lappend refNodeList $newNode
    set lastNode $newNode
  }
  $glat addLink $lastNode -2

  set refWords ""
  foreach item $ref {
    lappend refWords [lindex $item 0]
  }

  return $refNodeList
}

GLat glat$SID svmap$SID


readArray $warpFile warpScales
#if {$doMLLR || ($signalAdapt && $sasDir != "")} {
#    readArray hypos.txt HYPO
#    readArray probs.txt PROB
#}

if {$tcl_interactive} { set spk fsh_60262-A; set utt fsh_60262-A-0000; return }
puts "--- test started at [date]"

# ------------------------------------------------------------------------
#  main loop
# ------------------------------------------------------------------------

Path pathTmp

proc doAccu {convLst i} {
  global SID optWord variants env infoPath sasDir nIter gIter latDir mode thresh
  global signalAdapt sas vit_train labelPath warpScales lz accu_max_count

  set accu_count $accu_max_count
  codebookSet${SID} clearAccus -subX -1
  distribSet${SID}  clearAccus -subX -1

  while {[fgets $convLst spk] >= 0} {
    puts stderr "Processing $spk..."
    
    ########      FSA  accu & adaptation       ########
    if {$signalAdapt && $sasDir != ""} {
      $sas clear 0
  
      for {set iter 0} {$iter < $nIter} {incr iter} {
        $sas clear     1
        $sas clearAccu 1
        foreachSegment utt uttDB $spk {
            accuOneFSA_dbase $spk $utt
        }
        $sas compute 10 1 1
    
        $sas combine 0 1
      }
#      $sas save $sasDir/$spk.sas.gz 0
    }
  
    foreachSegment utt uttDB $spk {
      set uttInfo  [uttDB uttInfo $spk $utt]
      makeArray uttArray $uttInfo
      featureSet$SID eval $uttInfo

      set g $latDir/$utt.lat.gz
      if { [file exists $g] == 1} {
        if [catch {glat$SID read $g} msg] {
          puts "NFS error detected on reading $g, waiting..."
          after [expr {int(200)}]
          continue
        }

        set refNodeLst [addRefToGLat glat$SID $spk $utt]

        if { ![llength $refNodeLst] } {
          puts "ERROR at accuMPEStats $spk $utt"
          continue
        }
        set flag 0
        foreach item $refNodeLst {
          if { $flag == 1 } {
            set refNodeLstStr "${refNodeLstStr}_${item}"
          } else {
            set refNodeLstStr $item
            set flag 1
          }
        }

        regsub -all " " $refNodeLstStr "_" refNodeLstStr
        
        if [catch {glat$SID initAddon hmm$SID pathTmpP$SID -refNodeList $refNodeLstStr -mpe 1 -mode 0 -verbose 0} msg] {
          puts stderr "ERROR at initAddon $spk $utt\n$msg"
          glat$SID clearAddon
          continue
        }
        if [catch {glat$SID approxLocalPhnAcc hmm$SID -verbose 0 -refNodeList $refNodeLstStr} msg] {
          puts stderr "ERROR at approxLocalPhnAcc $spk $utt\n$msg"
          glat$SID clearAddon
          continue
        }

        #adjust the likelihood scores for bMMI
        aegis adjustScoresBMMI glat$SID -scale [expr 1.0 / $lz] -b 0.5

        set den_score [glat$SID posteriori -scale [expr 1.0 / $lz]]
        printDo path$SID alignGLatbMMIE pathNumC$SID pathDenC$SID hmm$SID glat$SID -refNodeList $refNodeLstStr -mode $mode -thresh $thresh -verbose 0

        printDo glat$SID clearAddon

        set num_score [computeNumScore glat$SID $spk $utt]
        puts stderr "num score: $num_score"
        puts stderr "den score: $den_score"
        puts stderr "obj: [expr  - $num_score + $den_score]"

        codebookSet${SID} configure -subX 0
        distribSet${SID}  configure -subX 0
        senoneSet${SID} accu path$SID

        codebookSet${SID} configure -subX 1
        distribSet${SID}  configure -subX 1
        senoneSet${SID} accu pathNumC$SID

        codebookSet${SID} configure -subX 2
        distribSet${SID}  configure -subX 2
        senoneSet${SID} accu pathDenC$SID

        path$SID reset
        pathMPEDen$SID reset
        pathNumC$SID reset
        pathDenC$SID reset
        pathML$SID reset
        pathTmpP$SID reset
      }
    }
    set accu_count [expr $accu_count - 1]
    if { $accu_count <= 0 } {
      puts stderr "Saving intermediate accus"
      if {[file exist Accus/${i}.$env(HOST).[pid].cba]} {
        codebookSet${SID} loadAccus Accus/${i}.$env(HOST).[pid].cba
        distribSet${SID}  loadAccus Accus/${i}.$env(HOST).[pid].dsa
      }
      codebookSet${SID} saveAccus Accus/${i}.$env(HOST).[pid].cba
      distribSet${SID}  saveAccus Accus/${i}.$env(HOST).[pid].dsa
      codebookSet${SID} clearAccus -subX -1
      distribSet${SID}  clearAccus -subX -1
      set accu_count $accu_max_count
    }
  }
  if {[file exist Accus/${i}.$env(HOST).[pid].cba]} {
    codebookSet${SID} loadAccus Accus/${i}.$env(HOST).[pid].cba
    distribSet${SID}  loadAccus Accus/${i}.$env(HOST).[pid].dsa
  }
  codebookSet${SID} saveAccus Accus/${i}.$env(HOST).[pid].cba
  distribSet${SID}  saveAccus Accus/${i}.$env(HOST).[pid].dsa
}

if ![file exist ${convLst}.${beg}] {catch {cp $convLst ${convLst}.${beg}}}

for {set i $beg} {$i <= $end} {incr i} {
  doParallel {
    doAccu $convLst.${i} $i
  } {
    codebookSet${SID} clearAccus -subX -1
    distribSet${SID}  clearAccus -subX -1

    writeLog stderr "$i: codebookSet${SID} loadAccus [glob Accus/${i}.*.cba]"
    writeLog stderr "$i: distribSet${SID}  loadAccus [glob Accus/${i}.*.dsa]"

    foreach cbfile [glob Accus/${i}.*.cba] { codebookSet${SID} loadAccus $cbfile }
    foreach dsfile [glob Accus/${i}.*.dsa] { distribSet${SID}  loadAccus $dsfile }

    codebookSet${SID} saveAccus Accus/${i}.cba.gz
    distribSet${SID}  saveAccus Accus/${i}.dsa.gz

    foreach cbfile [glob Accus/${i}.*.cba] { rm $cbfile }
    foreach dsfile [glob Accus/${i}.*.dsa] { rm $dsfile }

    writeLog stderr "senoneSet${SID} update"
    senoneSet${SID} update

    codebookSet${SID} save Weights/${i}.cbs.gz
    distribSet${SID}  load $GSID(distribSetParam)

    cp $convLst $convLst.[expr $i +1]

    if {$i == $end} {
      set flist [glob -nocomplain "$convLst.*"]
        foreach f $flist { catch { rm $f } }
      set flist [glob -nocomplain "*.DONE"]
        foreach f $flist { catch { rm $f } }
      touch train.DONE
    }
  } { } {
    if {$i < $end} {
      codebookSet${SID} load Weights/${i}.cbs.gz
    }
  }
}


exit
