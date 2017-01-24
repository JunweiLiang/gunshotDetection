# ========================================================================
#  JANUS-SR   Janus Speech Recognition Toolkit
#             ------------------------------------------------------------
#             Object: CBNew Model Tree
#             ------------------------------------------------------------
#
#  Author  :  Matthias Seeger
#  Module  :  cbnewTree.tcl
#  Date    :  Feb 5, 1998
#
#  Remarks :  We use the name 'distribTree$SID' for the model tree to
#             not get into trouble with other library functions. However
#             the models in the leaf nodes corr. to CBNew codebooks.
#
# ========================================================================

# ------------------------------------------------------------------------
#  cbnewTreeInit     create and initialize distribTree and cbnewStream
#                    object. Loads tree and ptreeSet.
# ------------------------------------------------------------------------

proc cbnewTreeInit { LSID args } {
  upvar \#0 $LSID SID

  set cbnewSet      cbnewSet$LSID
  set distribTree   distribTree$LSID
  set cbnewStream   cbnewStream$LSID
  set phones        phonesSet$LSID:PHONES
  set phonesSet     phonesSet$LSID
  set tags          tags$LSID
  set log           stderr

  if [info exist SID(ptreeSetDesc)] {
      set ptreeSetDesc $SID(ptreeSetDesc)
  }
  if [info exist SID(distribTreeDesc)] { 
    set distribTreeDesc $SID(distribTreeDesc)
  }
  if [info exist SID(padPhone)] { set padPhone $SID(padPhone) }

  itfParseArgv distribTreeInit $args [list [
    list -distribTree   string  {} distribTree   {} {model tree object}     ] [
    list -cbnewStream   string  {} cbnewStream   {} {cbnewStream object}    ] [
    list -cbnewSet      object  {} cbnewSet   CBNewSet {codebook set}       ] [
    list -phones        object  {} phones     Phones     {phones set}       ] [
    list -phonesSet     object  {} phonesSet  PhonesSet  {phonesSet set}    ] [
    list -tags          object  {} tags       Tags       {tags set}         ] [
    list -ptree         string  {} ptreeSetDesc    {}    {polyphonic tree}  ] [
    list -desc          string  {} distribTreeDesc {}    {description file} ] [
    list -padPhone      string  {} padPhone        {}    {padding phone}    ] [
    list -log           string  {} log             {} {name of log channel} ] ]

  if { [llength [info command $cbnewSet]] &&
       [llength [info command $distribTree]] < 1} {

    Tree $distribTree $phones $phonesSet $tags $cbnewSet

    if { [info exist padPhone] && [string length $padPhone]} {
      $distribTree configure -padPhone [$phones index $padPhone]
      set SID(padPhone) $padPhone
    }

    if { [info exist ptreeSetDesc] && [string length $ptreeSetDesc]} { 
       writeLog $log "$distribTree.ptreeSet read $ptreeSetDesc"
       $distribTree.ptreeSet read $ptreeSetDesc
       set SID(ptreeSetDesc) $ptreeSetDesc
    }
    if { [info exist distribTreeDesc] && [string length $distribTreeDesc]} {
       writeLog $log "$distribTree    read $distribTreeDesc"
       $distribTree read $distribTreeDesc
       set SID(distribTreeDesc) $distribTreeDesc
    }
  }

  if { [llength [info command $cbnewSet]]    && 
       [llength [info command $distribTree]]   &&
       [llength [info command $cbnewStream]] <  1} {

     CBNewStream $cbnewStream $cbnewSet $distribTree
  }
}
