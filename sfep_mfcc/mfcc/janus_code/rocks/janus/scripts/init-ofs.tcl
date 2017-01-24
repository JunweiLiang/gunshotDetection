# ========================================================================
# SWB02, create description for semi tied covariances
#
# Hagen Soltau, 02/21/02
# $Log: mkDesc.tcl,v $
# ========================================================================


randomInit [pid]
set tcl_precision 17


# ------------------------------------------------------------------------
#  Settings
# ------------------------------------------------------------------------

set descFile    "../desc/desc.tcl"
set semFile     "mas.DONE"

set feature     LDA         ;# input feature
set dimN        32          ;# input dimension
set block       {32}        ;# structure of block matrices 

source $descFile

waitFile "$semFile"


# ------------------------------------------------------------------------
#  Init Modules
# ------------------------------------------------------------------------

phonesSetInit   $SID
tagsInit        $SID
featureSetInit  $SID 
codebookSetInit $SID -param Weights/final.cbs.gz \
                     -desc  Weights/final.cbsDesc.gz -bmem 1
distribSetInit  $SID -param Weights/final.dss.gz \
                     -desc  Weights/final.dssDesc.gz -bmem 1
parmatSetInit   $SID -desc  "" -dimN $dimN

file mkdir descOFS


# ------------------------------------------------------------------------
#  Init parmatSet :  classes for covariances
# ------------------------------------------------------------------------

set     blockN [llength $block]
SVector blockV 
blockV := $block

# GLOBAL -----------------------------------------------------------------
set name ALL
regsub "@ " [phonesSet$SID:PHONES]] "" G(ALL)

# PHONE ------------------------------------------------------------------
# set name PS
# foreach p [phonesSet$SID:PHONES] {
#     set G($p) $p
# }
# set G(NSE) "&AH &HH &M &OW +BR +HU +LA +MB +NH +SM +TH"

# HAND -------------------------------------------------------------------
#set G0 {SIL +BR +HU +NH +SM +TH +LA}
#set G1 {&AH &HH &M &OW +MB}
#set G2 {IY IH EH AE}
#set G3 {AH AX IX}
#set G4 {AA AO UH UW}
#set G5 {AY OY EY AW OW}
#set G6 {M N EN NG}
#set G7 {F V TH DH S Z ZH SH HH CH JH}
#set G8 {P B T D K G DX}
#set G9 {R L W Y ER AXR}
#set name G10

# SPECIAL ----------------------------------------------------------------
# set fp [open /project/ears2/X7/train10000.newRCM.3/ldaX7.counts]
# while {[gets $fp line] >=0} {
#     set cb [lindex [lindex $line 0] 0]
#     set cn [lindex [lindex $line 0] 1]
#     regexp {(.+)-} $cb dummy p
#     set c $p2a($p)
#     if {! [info exists cnt($c)] } { set cnt($c) 0 }
#     set cnt($c) [expr $cnt($c) + $cn]
# }
# close $fp
# cnt(G0) = 15388567.0
# cnt(G1) = 3835534.0
# cnt(G2) = 11848683.0
# cnt(G3) = 6273852.0
# cnt(G4) = 4502382.0
# cnt(G5) = 9531653.0
# cnt(G6) = 8119134.0
# cnt(G7) = 14348526.0
# cnt(G8) = 13496045.0
# cnt(G9) = 12405100.0

# WORK -------------------------------------------------------------------
foreach p [phonesSet$SID:PHONES] {
    if {$p == "@" || $p == "PAD"} { continue }
    foreach i [array names G] {
	if {[lsearch $G($i) $p] >= 0} { break }
    }
    set c       G$i
    set p2a($p)  $c
    set p2a($c)  $c
    set aA($c)    1
}

set phoneL [array names aA]
foreach g $phoneL {
    set parmat "$g"
    writeLog stderr "parmatSet${SID} add $parmat $blockN [blockV puts]"
    if [ catch {parmatSet${SID} add $parmat $blockN blockV} ] {
	writelog stderr "ERROR: $msg"
	exit
    }
}

writeLog stderr "parmatSet${SID} save descOFS/parmatSet.$name"
if [ catch {parmatSet${SID} save descOFS/parmatSet.$name} msg ] {
    writelog stderr "ERROR: $msg"
    exit
}

writeLog stderr "parmatSet${SID} saveWeights descOFS/0i.$name.pms.gz"
if [ catch {parmatSet${SID} saveWeights descOFS/0i.$name.pms.gz} msg ] {
    writelog stderr "ERROR: $msg"
    exit
}


# ------------------------------------------------------------------------
#  Init extended codebookSet
# ------------------------------------------------------------------------

cbnewSetInit    $SID -desc  ""

foreach ds [distribSet${SID}] {
    set cbX  [distribSet$SID:$ds configure -cbX]
    set refN [codebookSet$SID.item($cbX) configure -refN]
    puts "$ds"
    set phone  [lindex [split $ds "-"] 0]
    regsub {\(\|\)} $phone "" phone
    set parmat $p2a($phone)

    writeLog stderr "cbnewSet${SID} add $ds $feature $refN"
    if [catch {cbnewSet${SID} add $ds $feature $refN} msg] {
	writeLog stderr "ERROR: $msg"
	exit
    }

    set cbIndex [cbnewSet${SID} index $ds]
    if {$cbIndex == -1} {
	writeLog stderr "ERROR: Can't create codebook $dsName!"
	exit
    }

    writeLog stderr "cbnewSet${SID} link $parmat $cbIndex all"
    if [catch {cbnewSet${SID} link $parmat $cbIndex all} msg] {
	writeLog stderr "ERROR: $msg"
	exit
    }
}

writeLog stderr "cbnewSet${SID} save descOFS/cbnewSet.$name"
if [catch {cbnewSet${SID} save descOFS/cbnewSet.$name} msg] {
    writeLog stderr "ERROR: $msg"
    exit
}


# ------------------------------------------------------------------------
#  Convert data structures
# ------------------------------------------------------------------------

FMatrix m1 
FMatrix m2 

foreach ds [distribSet${SID}] {
    puts "working on $ds"
    set cbIndex [distribSet${SID}:$ds configure -cbX]
    set refN    [codebookSet$SID.item($cbIndex) configure -refN]

    # copy means from old codebooks to extended codebooks
    cbnewSet${SID}:$ds set mean codebookSet${SID}.item($cbIndex).mat

    # copy diagonal covariances from old codebooks to extended codebooks
    set matList {}
    for {set i 0} {$i < $refN} {incr i} {
        # covariance:puts return 'det' as last value
        set cvL [list [lrange [lindex [codebookSet${SID}.item($cbIndex).cov($i)] 0] 0 [expr $dimN -1]]]
        set matList [concat $matList $cvL]
    }

    # new codebookstyle based on non-inverse covariances !!!
    m1 copy $matList
    for {set i 0} {$i < $refN} {incr i} {
        for {set j 0} {$j < $dimN} {incr j} {
            set val [expr 1.0 / [m1 get $i $j]]
            if {$val < 1e-19} {set val 1e-19}
            m1 set $i $j $val
        }
    }
    cbnewSet${SID}:$ds set diag m1
    
    # copy distributions from old distribSet to extended codebooks
    m2 := [distribSet${SID}:$ds configure -val]
    cbnewSet${SID}:$ds set distrib m2
}

writeLog stderr "cbnewSet${SID} saveWeights descOFS/0i.${name}.cbns.gz"
if [catch {cbnewSet${SID} saveWeights descOFS/0i.${name}.cbns.gz} msg] {
    writeLog stderr "ERROR: $msg"
    exit
}
catch {
    exec ln -s ../descOFS/0i.${name}.cbns.gz Weights/0i.${name}.cbns.gz
    exec ln -s ../descOFS/0i.${name}.pms.gz  Weights/0i.${name}.pms.gz
}
touch init.DONE

exit
