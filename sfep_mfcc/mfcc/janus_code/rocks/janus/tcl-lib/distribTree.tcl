# ========================================================================
#  JANUS-SR   Janus Speech Recognition Toolkit
#             ------------------------------------------------------------
#             Object: Distribution Tree
#             ------------------------------------------------------------
#
#  Author  :  Michael Finke
#  Module  :  distribTree.tcl
#  Date    :  06/14/96
#
#  Remarks :  
#
# ========================================================================


proc distribTree_Info { name } {
    switch $name {
	procs {
	    return "distribTreeInit"
	}
	help {
	    return "This file provides a wrapper for the tree of
distributions, which is needed to find the distribution for each context."
	}
	distribTreeInit {
	    return "Initializes 'distribTree\$SID'. Needs a 'distribSet',
a description file and creates a 'distribStream', which the 'senoneSet' 
takes to compute scores."
	}
    }
    return "???"
}


# ------------------------------------------------------------------------
#  distribTreeInit   create and initialize distribTree and distribStream
#                    object. Loads tree and ptreeSet.
# ------------------------------------------------------------------------

proc distribTreeInit { LSID args } {
  upvar \#0 $LSID SID

  set distribSet    distribSet$LSID
  set distribTree   distribTree$LSID
  set distribStream distribStream$LSID
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
    list -distribTree   string  {} distribTree   {} {distribTree object}    ] [
    list -distribStream string  {} distribStream {} {distribStream object}  ] [
    list -distribSet    object  {} distribSet DistribSet {distribution set} ] [
    list -phones        object  {} phones     Phones     {phones set}       ] [
    list -phonesSet     object  {} phonesSet  PhonesSet  {phonesSet set}    ] [
    list -tags          object  {} tags       Tags       {tags set}         ] [
    list -ptree         string  {} ptreeSetDesc    {}    {polyphonic tree}  ] [
    list -desc          string  {} distribTreeDesc {}    {description file} ] [
    list -padPhone      string  {} padPhone        {}    {padding phone}    ] [
    list -log           string  {} log             {} {name of log channel} ] ]

  if { [llength [info command $distribSet]] &&
       [llength [info command $distribTree]] < 1} {

    Tree $distribTree $phones $phonesSet $tags $distribSet

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
       writeLog $log "$distribTree read $distribTreeDesc"
       $distribTree read $distribTreeDesc
       set SID(distribTreeDesc) $distribTreeDesc
    }
  }

  if { [llength [info command $distribSet]]    && 
       [llength [info command $distribTree]]   &&
       [llength [info command $distribStream]] <  1} {

     DistribStream $distribStream $distribSet $distribTree
  }
}

# ------------------------------------------------------------------------
#  distribTreeAddModel     adds a new model to a distribution tree.
# ------------------------------------------------------------------------

proc distribTreeAddModel { LSID args } {
  upvar \#0 $LSID SID

  set tree  distribTree$LSID
  set cbs   codebookSet$LSID
  set dss   distribSet$LSID
  set log   stderr
  set root "ROOT"

  itfParseArgv distribTreeAddModel $args [list [
    list <question>   string  {} question {}       {question}             ] [
    list <distrib>    string  {} dsname   {}       {name of distribution} ] [
    list <codebook>   string  {} cbname   {}       {name of codebook}     ] [
    list <feature>    string  {} feature  {}       {name of feature}      ] [
    list <refN>       int     {} refN     {}       {number of ref}        ] [
    list <dimN>       int     {} dimN     {}       {dimension of input}   ] [
    list <type>       string  {} type     {}       {type of covariance}   ] [
    list -codebookSet object  {} cbs      CodebookSet {codebook set}      ] [
    list -distribSet  object  {} dss      DistribSet  {distribution set}  ] [
    list -distribTree object  {} tree     Tree        {tree}              ] [
    list -root        string  {} root     {}       {name of root node}    ] [
    list -ptree       string  {} ptree    {}       {name of ptree}        ] [
    list -log         string  {} log      {}       {name of log channel}  ] ]

  writeLog $log "distribTreeAddModel $LSID $args"

  # Create new codebook $cbname in codebook set $cbs

  if {[$cbs index $cbname] < 0} {
    $cbs add $cbname $feature $refN $dimN $type
  }

  # Create new distribution $dsname in distribution set $dss

  if {[$dss index $dsname] < 0} { $dss add $dsname $cbname }

  # Check, if the requested root in the tree already exists.

  set qnode ROOT-$dsname
  set lnode      $dsname

  if {[$tree index $root] < 0} {
    $tree add $root {} $qnode $qnode $qnode "-"
    $tree add $qnode $question - $lnode - -
    $tree add $lnode {} - - - $dsname
  } else {
    $tree add $qnode $question - $lnode - - 
    $tree add $lnode {} - - - $dsname

    # Find last NO node starting at root

    set lidx [$tree index $root]
    set idx   $lidx

    while { [set idx [$tree.item($lidx) configure -no]] > -1} { set lidx $idx }
    $tree.item($lidx) configure -no [$tree index $qnode]
  }

  # 

  if [info exist ptree] {
    if {[${tree}.ptreeSet index $lnode] < 0} {
         ${tree}.ptreeSet add   $lnode $ptree 0 0 -count 1
    }
    $tree:$lnode configure -ptree [${tree}.ptreeSet index $lnode]
  }
}


# ------------------------------------------------------------------------
#  distribTreeCleanModels for each node that is not a leave, remove any
#                         models that might be attached to this node
# ------------------------------------------------------------------------
proc distribTreeCleanModels { args } {
    itfParseArgv distribTreeCleanModels $args [list [list <tree> object {} tree Tree {tree}] ]

    set itemN [$tree configure -itemN]

    for { set i 0} { $i < $itemN} { incr i} {
	set question [$tree.item($i) configure -question]
	set node     [$tree.item($i) configure -name]
	set model    [$tree.item($i) configure -model]
	
	if { ($question > 0) && ($model > -1)} {
	    $tree.item($i) configure -model -1
	}
    }
}

# ------------------------------------------------------------------------
#  distribTreeShareCBs   for each node in the tree with a polyphonic tree
#                        attached to it all distributions stored in that
#                        ptree are made to shared a single new codebook.
# ------------------------------------------------------------------------

proc distribTreeShareCBs { args } {
  set itemN -1

  itfParseArgv distribTreeShareCBs $args [list [
    list <tree>       object  {} tree   Tree {tree} ] [
    list "-itemN"     int     {} itemN  {}   {number of leafs} ] ]

  if {$itemN < 0} {
    set itemN [$tree configure -itemN]
  }

  for { set i 0} { $i < $itemN} { incr i} {
    set node  [$tree.item($i) configure -name]
    set model [$tree.item($i) configure -model]
    set ptree [$tree.item($i) configure -ptree]

    if { $ptree > -1} {
      ModelArray dtabMA  $tree.modelSet

      $tree.ptreeSet.item($ptree) models dtabMA

      if { [llength [set models [dtabMA puts]]]} {

        # find codebook all models are based on by looking up the first 
        # distribution in the model array
        set cbX    [$tree.modelSet:[lindex [lindex $models 0] 0] configure -cbX]
        set cbName [$tree.modelSet.codebookSet name $cbX]
        set cbFeat [$tree.modelSet.codebookSet.featureSet name\
                   [$tree.modelSet.codebookSet:$cbName configure -featX]]
        set cbRefN [$tree.modelSet.codebookSet:$cbName configure -refN]
        set cbDimN [$tree.modelSet.codebookSet:$cbName configure -dimN]
        set cbType [$tree.modelSet.codebookSet:$cbName configure -type]

	regsub -all "\\(\\|\\)" $node "" node; # This makes for clean CB names (Florian)
        if { [${tree}.modelSet.codebookSet index $node] < 0 } {
          $tree.modelSet.codebookSet add $node $cbFeat $cbRefN $cbDimN $cbType
          $tree.modelSet.codebookSet:$node := $tree.modelSet.codebookSet:$cbName
        }

        foreach model $models {
          $tree.modelSet:[lindex $model 0] configure -cbX \
                         [$tree.modelSet.codebookSet index $node]
        }
      }
      dtabMA destroy
    }
  }
}

# ------------------------------------------------------------------------
#  distribTreeAddBackoff   adds for each node in a distribution tree which
#                          has a polyphonic tree but no model index a new
#                          distribution which serves as backoff distrib
#                          in case a polyphone can't be found in the 
#                          ptree. The distribution will have the same name
#                          as the node and will be defined over the same
#                          codebook as the first model in the ptree (when
#                          read out into a model array).
# ------------------------------------------------------------------------

proc distribTreeAddBackoff { args } {

  set errN 0
  set log  stderr

  itfParseArgv distribTreeAddBackoff $args [list [
    list <tree>       object  {} tree Tree       {tree} ] ]

  set itemN [$tree configure -itemN]

  for { set i 0} { $i < $itemN} { incr i} {
    set node  [$tree.item($i) configure -name]
    set model [$tree.item($i) configure -model]
    set ptree [$tree.item($i) configure -ptree]

    if { $model < 0 && $ptree > -1} {
      ModelArray dtabMA  $tree.modelSet

      $tree.ptreeSet.item($ptree) models dtabMA

      if { [llength [set models [dtabMA puts]]]} {
        set cbX    [$tree.modelSet:[lindex [lindex $models 0] 0] configure -cbX]
        set cbName [$tree.modelSet.codebookSet name $cbX]

        $tree.modelSet add $node $cbName
        $tree.item($i) configure -model [$tree.modelSet index $node]
      } else {
        writeLog $log "E $tree: can't find a backoff distribution for $node."
        incr errN
      }
      dtabMA destroy
    }
  }
  if { $errN > 0} { error "E $tree: no backoffs for $errN distributions" }
}

# ------------------------------------------------------------------------
#  distribTreeAccuBackoff  adds the accumulator of all models within a
#                          polyphonic tree of a node to the accu of the
#                          node model which is usually supposed to be a
#                          backoff model.
# ------------------------------------------------------------------------

proc distribTreeAccuBackoff { args } {

  itfParseArgv distribTreeAddBackoff $args [list [
    list <tree>       object  {} tree Tree       {tree} ]]

  set itemN [$tree configure -itemN]

  for { set i 0} { $i < $itemN} { incr i} {
    set node  [$tree.item($i) configure -name]
    set model [$tree.item($i) configure -model]
    set ptree [$tree.item($i) configure -ptree]

    if { $model > -1 && $ptree > -1} {
      ModelArray MA      $tree.modelSet

      $tree.ptreeSet.item($ptree) models MA
 
      foreach model [MA puts] {
        if { [${tree}.modelSet index ${node}] > -1} {
          catch {${tree}.modelSet:${node}.accu += ${tree}.modelSet:[lindex $model 0].accu}
        } else {
          puts "ERROR ${tree}.modelSet:${node}.accu += ${tree}.modelSet:[lindex $model 0].accu"
        }
      }
      MA destroy
    }
  }
}

# ------------------------------------------------------------------------
#  distribTreeAccuBackoff  adds the accumulator of all models within a
#                          polyphonic tree of a node to the accu of the
#                          node model which is usually supposed to be a
#                          backoff model.
# ------------------------------------------------------------------------

proc distribTreeBackoffInterpolation { args } {

  itfParseArgv distribTreeBackoffInterpolation $args [list [
    list <tree>       object  {} tree Tree       {tree} ]]

  set itemN [$tree configure -itemN]

  for { set i 0} { $i < $itemN} { incr i} {
    set node  [$tree.item($i) configure -name]
    set model [$tree.item($i) configure -model]
    set ptree [$tree.item($i) configure -ptree]

    if { $model > -1 && $ptree > -1} {
      ModelArray MA      $tree.modelSet

      $tree.ptreeSet.item($ptree) models MA

      set boCount [${tree}.modelSet:${node} configure -count]
      set count    0

      foreach model [MA puts] {
        set dsX [lindex $model 0] 
        if { [${tree}.modelSet index ${node}] > -1} {
          set dsCount [${tree}.modelSet:$dsX configure -count]
        }
        set count [expr $count + $dsCount]
      }

      if { $count > 0.0} {
        foreach model [MA puts] {
          set dsX [lindex $model 0] 
          if { [${tree}.modelSet index ${node}] > -1} {
            set dsCount [${tree}.modelSet:$dsX configure -count]
            ${tree}.modelSet:$dsX.accu += ${tree}.modelSet:${node}.accu \
                   -factor [expr $dsCount / $count * $boCount]
          }
        }

        # to balance the counts between base model and ptree models

        ${tree}.modelSet:${node}.accu *= 2
      }
      MA destroy
    }
  }
}
