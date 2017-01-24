# ========================================================================
#  JanusRTk   Janus Speech Recognition Toolkit
#             ------------------------------------------------------------
#             Object: Documentation Skeleton File
#             ------------------------------------------------------------
#
#  Author  :  Florian Metze
#  Module  :  tcl2tex.tcl
#  Date    :  $Id: tcl2tex.tcl 3125 2010-02-14 05:24:12Z metze $
#
#  Remarks :  This procedure will generate a skeleton *.tex file from
#             the online help information available in Janus
#
#             Simply run it with the most recent Janus executable in
#             ~/janus/doc to create up-do-date versions of the 
#             automatically generated software
#
# ------------------------------------------------------------------------
#
# $Log$
# Revision 1.2  2003/08/14 11:18:57  fuegen
# Merged changes on branch jtk-01-01-15-fms (jaguar -> ibis-013)
#
# Revision 1.1.2.14  2003/08/07 13:01:24  metze
# *** empty log message ***
#
# Revision 1.1.2.13  2003/07/14 15:30:04  metze
# More complete version for V5.0 P013
#
# Revision 1.1.2.12  2003/06/26 15:08:39  metze
# Initial changes for V5.0 P013
#
# Revision 1.1.2.11  2003/04/30 15:43:59  metze
# Changes before the new repository
#
# Revision 1.1.2.10  2002/11/21 16:17:22  metze
# avoid tex
#
# Revision 1.1.2.9  2002/11/20 16:15:02  metze
# Doku Pre-P012
#
# Revision 1.1.2.8  2002/11/04 14:57:31  metze
# Semi-automatic tcl-lib documentation
#
# Revision 1.1.2.7  2002/10/04 13:24:19  metze
# Now works in janus/doc without system description file
#
# Revision 1.1.2.6  2002/08/30 14:00:05  metze
# Added updates from Christian
#
# Revision 1.1.2.5  2002/08/27 16:19:12  metze
# New version of automatic generator.
#
# Revision 1.1.2.4  2002/08/26 16:52:25  metze
# Cosmetic
#
# Revision 1.1.2.3  2002/08/26 16:40:37  metze
# Re-organization of modules
#
# Revision 1.1.2.2  2002/08/01 13:42:21  metze
# Fixes for clean documentation.
#
# Revision 1.1.2.1  2002/07/31 13:01:39  metze
# *** empty log message ***
#
# ========================================================================


#% d write -help
#Options of 'write' are:
# <filename>  file to write to (string:"NULL")
# -format     file format (janus or lidia) (string:"janus")
#% Dictionary d_ -help
#ERROR   itf.c(0334) <ITF,FCO>  Failed to create 'd_' object.
#Options of 'd_' are:
# <Phones>  phones
# <Tags>    tags
#% LTree l -help
#ERROR   itf.c(0334) <ITF,FCO>  Failed to create 'l' object.
#Options of 'l' are:
# <SearchTree>  Search tree
# -map          Vocabulary mapper to use for LookAhead only
# -depth        Maximum depth of LookAhead tree (int:5)
# -reduced      Set 'reduce' flag for LookAhead nodes (int:0)


set allTypes [types]
set lowTypes [string tolower $allTypes]


proc putsTeX { fp txt } {
    global allTypes

    regsub -all "_"         $txt "\\\\_" Txt
    regsub -all "#"         $Txt "\\\\#" Txt
    regsub -all "&"         $Txt "\\\\&" Txt
    regsub -all "%"         $Txt "\\\\%" Txt
    regsub -all "\\$"       $Txt "\\\\$" Txt
    regsub -all "\\^"       $Txt "\\\\^" Txt
    regsub -all "<"         $Txt "\$<\$" Txt
    regsub -all ">"         $Txt "\$>\$" Txt
    regsub -all "\\\\\""    $Txt "''"    Txt
    regsub -all " xYx "     $Txt " \\& " Txt

    regsub {"\{ "} $Txt "\"\\{ \"" Txt
    regsub {" \}"} $Txt "\" \\}\"" Txt
#    regsub -all ":\[^)\]*\\)" $Txt ")"   Txt
    regsub -all "\\(\[^)\]*:\[^)\]*\\)" $Txt ""      Txt

    puts $fp $Txt
}


proc parseArgs { argL } {
    upvar parseArgsResult lst
    global allTypes lowTypes
    set res ""
    set lst "\n      \\begin{tabular}{ll}\n"

    foreach i $argL {
	#set key  [lindex $i 0]
	#regexp {[A-Za-z]+} $key tmp
	#set name [string tolower $tmp]
	#set key  [lindex $i 0]
	#set expl [lrange $i 1 end]

	set i [string trim $i]
	if {[regexp       -- "^<(\[0-9A-Za-z_ \]+)> (.*)" $i dummy name expl]} {
	    regsub -all "_" [string tolower $name] " " key
	    set res  "$res <$name>"
	} elseif {[regexp -- "^-(\[0-9A-Za-z_\]+) (.*)" $i dummy name expl]} {
	    set key  [string tolower $name]
	    set res  "$res \\Jsb{-$name $key}"
	    set name $key
	} else {
	    set key  [string tolower [set name [lindex $i 0]]]
	    set expl [lrange $i 1 end]
	    if {[regexp "<" $name]} { set res  "$res $name"
	    } else                  { set res  "$res <$name>" }
	}
	regsub -all "<|>" $name "" name
	regsub -all "<>"  $res  "" res

	if {[regexp "\\((\[A-Za-z\]*):\[^)\]*\\)" $i d o] && [lsearch $allTypes $o] >= 0} {
	    regsub "\\(\[A-Za-z\]*:\[^)\]*\\)" $expl "(\\Jref{module}{$o})" expl
#	    regsub {" \}"} $expl {'' \}''} expl
#	    set expl "$expl (\\Jref{module}{$o})"
	}
	set lst "${lst} \\texttt{\\textbf{$name}} xYx $expl \\\\\n"
    }
    set lst "${lst}      \\end{tabular}"

    if {[llength $argL] == 0} { set lst "" }

    return $res
}


proc genInfo { obj } {
    global itfUsageInfo
    global parseArgsResult
    global parType

    puts "\n    $obj"

    catch { unset mthdI }
    catch { unset method }
    catch { unset objI }
    catch { unset subobj }
    catch { unset elements }
    catch { unset crtI }
    catch { unset conf }
    catch { unset itfUsageInfo }

    # Get the type
    if {[catch {set type [$obj type]}]} {
	puts "WARN: Couldn't generate '$obj' info"
	return 
    }
    global ${type}Methods
    puts "      $type\n"

    catch { parray ${type}Methods }

    # Get info
    set objI [split [$obj -help] "\n"]

    # Hack for TCL-Defined methods
    regsub -all "\{TCL-DEFINED METHODS.*" $objI "" objI
    foreach i [array names ${type}Methods] {
	set m [expr \$${type}Methods($i)]
	lappend objI "$i [lindex [lrange $m 1 end] 0] ([lindex $m 0])"
    }

    # Get the glossary information (might be incomplete)
    set gloss [lindex $objI 3]

    # Get the methods
    foreach i [lrange $objI [lsearch $objI METHODS] end] {
	if {[llength $i] < 3 && $i != "puts"} { continue }
	set method([lindex $i 0]) [lrange $i 1 end]
	catch { unset itfUsageInfo }
	catch { $obj [lindex $i 0] -help } msg
	if {[info exists itfUsageInfo]} {
	    # Tcl-defined method!
	    set mthdI([lindex $i 0])   $itfUsageInfo
	} else {
	    set mthdI([lindex $i 0])   [lrange [split $msg "\n"] 1 end]
	}
	if {[lindex $i 0] == "puts" && [string length $mthdI(puts)] > 1000} { set mthdI(puts) "" }
    }

    # Get the configuration options
    catch { foreach i [$obj configure] {
	set conf([lindex $i 0]) [lindex $i 1]
    } }

    # Get the Subobjects
    catch { foreach i [${obj}.] {
	if {[regexp "item\\(.*" $i]} { continue }
	if {[catch { set subobj($i) [${obj}.$i type] } ]} { set subobj($i) "???" }
    } }

    # Get the Elements
    set elements ""
    if {[catch { set elements [${obj}.item(0) type] }]} {
	puts "WARN: Couldn't find '${obj}.item(0)' info."
    }

    # Creation/ Invocation
    if {[catch { $type [set nm ${obj}[clock seconds]] -help } msg]} {
	set crtI [lrange [split $msg "\n"] 1 end]
	if {[lindex [lindex $crtI 0] 0] != "<name>"} { set crtI "{ <name>    name of the object} $crtI" }
    } else {
	set crtI "{ <name>    name of the object}"
    }

    # Output
    puts "Glossary: $gloss"
    puts "Creation: $type <- $crtI"
    puts "Elements: $elements"
    catch {parray method}
    catch {parray mthdI}
    catch {parray conf}
    catch {parray subobj}

    set name [string tolower $type]

    # ----------
    #   Output
    # ----------
    set fp [open modules/${type}.tex w]

    # Header
    puts    $fp "%% <<< Skeleton generated automatically by tcl2tex.tcl\n"
    puts    $fp "\\subsection{\\Jlabel{module}{$type}}\n"
    # puts    $fp "\\JaddGloss{$type}{$gloss}\n"
    puts    $fp "This section describes the '\\Jgloss{$type}': \\textsl{$gloss}\n"
    puts    $fp "\\begin{description}"

    # Creation
    if {[info exist parType($type)] && [string length $parType($type)]} {
	# It's a sub-object only!
	putsTeX $fp "\\vspace{3mm}  \\item\[Creation:\] \\texttt{$type} cannot be created directly.\\\n"
	putsTeX $fp "It is accessible as a sub-object of \\Jref{module}{$parType($type)}!"
    } else {
	putsTeX $fp "\n  \\item\[Creation:\] \\texttt{$type [parseArgs $crtI]}\n"
	putsTeX $fp "$parseArgsResult"
    }

    # Configuration
    if {[llength [array names conf]]} {
	putsTeX $fp "\n\\vspace{3mm}  \\item\[Configuration:\] \\texttt{$name configure}\n"
	putsTeX $fp "\n    \\begin{tabular}{ll}"
	foreach i [lsort [array names conf]] {
	    putsTeX $fp "      \\Jlabel{$type}{$i} xYx = $conf($i) \\\\"
	}
	putsTeX $fp "    \\end{tabular}"
    }

    # Methods
    if {[llength [array names method]]} {
	putsTeX $fp "\n\\vspace{3mm} \\item\[Methods:\] \\texttt{$name}"
	putsTeX $fp "\n    \\begin{description}"
 	foreach i [lsort [array names method]] {
	    putsTeX $fp "      \\Jitem{\\Jlabel{$type}{$i}} \\texttt{[parseArgs $mthdI($i)]} \\\n"
	    putsTeX $fp "        $method($i)"
	    putsTeX $fp "$parseArgsResult"
	}
	putsTeX $fp "    \\end{description}"
    }

    # Subobjects
    if {[info exists subobj]} {
	putsTeX $fp "\n  \\item\[Subobjects:\] \\hfill \\\\"
	putsTeX $fp "\\ \n    \\begin{tabular}{ll}"
	foreach i [lsort [array names subobj]] {
	    putsTeX $fp "      \\texttt{\\textbf{$i}} xYx (\\Jref{module}{$subobj($i)}) \\\\"
	}
	putsTeX $fp "    \\end{tabular}\n\\vspace{3mm}"
    }

    # Elements
    if {$elements != ""} {
	putsTeX $fp "\n  \\item\[Elements:\] are of type \\Jref{module}{$elements}.\n"
    }

    # Finis
    # puts    $fp "\n  \\item\[Files:\] ${name}.h, ${name}.c"

    puts    $fp "\n\\end{description}\n"
    puts    $fp "%% Skeleton generated automatically by tcl2tex.tcl >>>"
    close $fp
}


# ---------------------------------------------
#   Scan the type-information
# ---------------------------------------------
set fp [open modules/gloss.tex w]
foreach type [types] {
    set gloss [lindex [split [$type -help] "\n"] 3]
    puts $fp "\\JaddGloss{$type}{$gloss}\n"
}
close $fp


if {![regexp "janus\/doc$" [file dirname [info script]]]} {
    puts "------------------------------------------------"
    puts "  Usually, it's best to run me in ~/janus/doc."
    puts "  If you disagree - you have been warned!"
    puts "------------------------------------------------"
}


# ---------------------------------------------
#   Create a system from this start-up script
# ---------------------------------------------
set Path [file dirname [info script]]/..
source $Path/tcl-lib/dbase.tcl
source $Path/tcl-lib/ibis.tcl
source $Path/tcl-lib/modalities.tcl
source $Path/tcl-lib/speech.tcl
source $Path/tcl-lib/train.tcl
source $Path/tcl-lib/cfg.tcl

source $Path/scripts/desc.tcl


# --------------------------------------------------------------------
#  Init Modules
# --------------------------------------------------------------------
phonesSetInit     $SID -desc  ""
tagsInit          $SID -desc  ""
featureSetInit    $SID -desc  "" -access "" -lda   ""
codebookSetInit   $SID -desc  "" -param  ""
distribSetInit    $SID -desc  "" -param  ""
bbiSetInit        $SID -desc  "" -param  ""
  phonesSet$SID   add PHONES SIL
  tags$SID        add WB

distribTreeInit   $SID -desc  ""
  distribTreeAddModel $SID {0=SIL} SIL-b SIL LDA 4 4 DIAGONAL -root ROOT-b
  distribTreeAddModel $SID {0=SIL} SIL-m SIL LDA 4 4 DIAGONAL -root ROOT-m
  distribTreeAddModel $SID {0=SIL} SIL-e SIL LDA 4 4 DIAGONAL -root ROOT-e

senoneSetInit     $SID distribStream$SID
topoSetInit       $SID -desc  "" -tm ""
  tmSet$SID       add tm { {1 0.000000} }
  topoSet$SID     add topo {ROOT-b ROOT-m ROOT-e} {tm tm tm}

ttreeInit         $SID -desc  ""
  ttree$SID       add ROOT {} - - - topo


#----------------------------------
# Remaining setup
#----------------------------------
#dbaseInit         $SID $dbase
dictInit          $SID -desc ""
trainInit         $SID

  SVocab          svocab$SID dict$SID
  dict$SID        add {$} {SIL}
  dict$SID        add {(} {SIL}
  dict$SID        add {)} {SIL}
  svocab$SID      add {$}
  svocab$SID      add {(}
  svocab$SID      add {)}

  set fp [open lm w]
  puts $fp "\\data\\\nngram 1=3\n\n\\1-grams:\n0 <UNK>\n0 </s>\n0 <s>\n\\end\\\n"
  close $fp

ibisInit          $SID -xcm 1 -vocab svocab$SID -vocabDesc "" -lmDesc lm
  file delete lm


#----------------------------------
# Create, modify more objects
#----------------------------------
# Create accus (so they appear in the on-line doc output)
codebookSet$SID createAccus
distribSet$SID  createAccus

# Create related extra modules (not created in the start-up script)
[LingKS p PhraseLM].data base  lm$SID
[LingKS m MetaLM].data   LMadd lm$SID
m.data                   LMadd p
LingKS c CFGSet
CFG cfg -lks c
c.data build
GLat              glat$SID svmap$SID

# Create 'fancy' objects
LDA               lda$SID featureSet$SID LDA 4
QuestionSet       QS phonesSet$SID:PHONES phonesSet$SID tags$SID
RewriteSet        rewriteSet$SID
MLAdapt           mlAdapt$SID codebookSet$SID
SampleSet         sampleSet$SID featureSet$SID LDA 4
TextGraph         textGraph$SID
ModalitySet       modalitySet$SID tags$SID
Modality          modality putsInfo WB
StateTable        stateTable$SID modalitySet$SID
CBNewParMatrixSet cbnewparmatrixset 1
CBNewSet          cbnewset cbnewparmatrixset featureSet$SID 1
cbnewTreeInit $SID -distribTree cbsdt -cbnewSet cbnewset -cbnewStream cbnewstream -desc ""

featureSet$SID FMatrix LDA 1 4
hmm$SID make "( \$ )"
path$SID make senoneSet$SID
path$SID.itemList(0) add 1

SignalAdapt       signalAdapt${SID} senoneSet$SID
foreach ds [distribSet$SID] {signalAdapt${SID} add $ds}
signalAdapt${SID} clear 0
signalAdapt${SID} clearAccu 0

# Create 'standard' objects
SVector           svector 1
FVector           fvector 1
DVector           dvector 1
CMatrix           cmatrix 1 1
IMatrix           imatrix 1 1
FMatrix           fmatrix 1 1
DMatrix           dmatrix 1 1
FBMatrix          fbmatrix 1
Labelbox          labelbox
Cbcfg             cbcfg
Dscfg             dscfg
DurationSet       durationset 1
Phones            phones
PTree             ptree    phonesSet$SID:PHONES tags$SID distribSet$SID
PTreeSet          ptreeSet phonesSet$SID:PHONES tags$SID distribSet$SID

  close           [open dummy w]
Lh                lh dummy
  file delete     dummy

bbiSetInit $SID

#codebookSet$SID   makeBBI
codebookSet$SID   addBBI SIL all
codebookSet$SID    makeBBI
codebookSet$SID   createMaps 1

# Filter            filter 1 2
# ContextCache      contextcache
# ModelSet          ms
ModelArray        marray distribSet$SID


# -----------------------------------
#   Loop over all existing objects, 
#   to find their subobjects
# -----------------------------------
catch { unset objArr }
catch { unset parType }
foreach i [set objList [lsort [objects]]] { set parType([$i type]) "" }
while {[llength $objList]} {
    set objNow  [lindex $objList 0]
    set objList [lrange $objList 1 end]
    set objType [$objNow type]

    puts "\n$objNow ($objType) ..."
    if {![info exists objArr($objType)]} { set objArr($objType) $objNow }
    if {[catch { set l [${objNow}.] }]} { set l "" }
    foreach subObj $l {
	regsub -all "\\.\\.\[0-9\]+" $subObj "" tmp
	if {![catch { [set nxtObj ${objNow}.${tmp}] }] &&
	    [lsearch [types] "[set nxtType [$nxtObj type]]"] >= 0    &&         
	    ![info exists objArr([$nxtObj type])]      } {
	    puts "  $nxtObj ($nxtType)"
	    lappend objList $nxtObj
	    if {![info exist parType($nxtType)]} {set parType($nxtType) $objType}
	}
    }
}
puts "Ok."


# ------------------------------------------
#   Generate info for all existing objects
# ------------------------------------------
foreach i [array names objArr] {
    genInfo $objArr($i)
}

# Generate info for sub-objects
genInfo spass$SID.stab
genInfo hmm$SID.stateGraph
genInfo hmm$SID.phoneGraph
genInfo hmm$SID.wordGraph

# Difficulty with TCL-DEFINED METHODS:
#   In tcl-lib: ibis.tcl modalities.tcl speech.tcl train.tcl (dbase.tcl test.tcl)
#   (Test interactively first by Copy&Paste of stuff below)
SPass       -help
SVMap       -help
GLat        -help
LingKS      -help
PhraseLM    -help
MetaLM      -help
CFGSet      -help
ModalitySet -help
StateTable  -help
FeatureSet  -help
HMM         -help
genInfo spass$SID
genInfo svmap$SID
genInfo glat$SID
genInfo lm$SID.data
genInfo p.data
genInfo m.data
genInfo c.data
genInfo modalitySet$SID
genInfo stateTable$SID
genInfo featureSet$SID
genInfo hmm$SID
genInfo marray

puts ""
puts "----------------------------------------------------------------"
puts "  You'll find the newly generated descriptions in doc/modules."
puts "  You can now re-create the documentation."
puts "----------------------------------------------------------------"
puts ""

set fp [open "|grep input{modules modules.tex" r]
while {[gets $fp line] != -1} {
    regsub ".*\/" $line "" f
    regsub "\}"   $f    "" f
    set included($f) 1
}
close $fp
foreach w "CVS CFGExt.tex SRoot.tex CFGFSGraph.tex CFGRuleStack.tex" {
    set included($w) 1
}

foreach i "[glob modules/*]" {
    regsub ".*\/" $i "" f
    if {![info exists included($f)]} {
	puts "WARN: '$f' is not yet input in 'modules.tex'."
    }
}

set fp [open janus-version.tex w]
puts $fp "\\newcommand{\\Jversion} {$janus_version P$janus_patchLevel}"
close $fp

set fp [open janus-version.txt w]
puts $fp "$janus_version P$janus_patchLevel"
close $fp

if {!$tcl_interactive} {
    exit
}
