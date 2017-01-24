# ========================================================================
#  JANUS-SR   Janus Speech Recognition Toolkit
#             ------------------------------------------------------------
#             Object: Tree Routines
#             ------------------------------------------------------------
#
#  Author  :  Michael Finke
#  Module  :  tree.tcl
#  Date    :  06/24/96
#
#  Remarks :  
#  $Log$
#  Revision 1.3  2003/08/14 11:20:32  fuegen
#  Merged changes on branch jtk-01-01-15-fms (jaguar -> ibis-013)
#
#  Revision 1.2.50.1  2002/11/04 15:03:58  metze
#  Added documentation code
#
#  Revision 1.2  2000/09/10 12:18:51  hyu
#  Added Tcl files from Hagen
#
#  Revision 1.1  2000/02/07 09:26:28  soltau
#  Initial revision
#
# ========================================================================


proc tree_Info { name } {
    switch $name {
	procs {
	    return "treeQuestion treeCluster treeReadSplits"
	}
	help {
	    return "Various handy procedures for Tree objects."
	}
	treeQuestion {
	    return "Find a question for a given node in the tree (if there
                 is a polyphonic tree attached to the node)."
	}
	treeCluster {
	    return "Clusters tree given a set of questions, the minimum
                number of counts expected to be in the ModelArray for
                each answer node, the minimum count and the maximum
                number of splits for each node."
	}
	treeReadSplits {
	    return "Reads cluster log file into an array indexed by the
                  gain of each split. This array is used to split
                  a decision tree."
	} 
    }
    return "???"
}


# ------------------------------------------------------------------------
#  treePTreeList     returns a list of all nodes in a tree that have a
#                    polyphonic tree attached
# ------------------------------------------------------------------------

proc treePTreeList { args } {

  itfParseArgv treePTreeList $args [list [
    list <tree> object  {} tree     Tree {tree} ] ]

  set lst {}

  foreach name [$tree:] { 
    if { [set p [$tree:$name configure -ptree]] >= 0} { lappend lst $name }
  }
  return $lst
}

# ------------------------------------------------------------------------
#  treePTreePrune
# ------------------------------------------------------------------------

proc treePTreePrune { tree args } {

  itfParseArgv treePTreePrune $args [list [
    list <distribSet> object  {} dss DistribSet {distribution set} ] ]

  foreach node [$tree:] {
    set model [$tree:$node configure -model]
    set ptree [$tree:$node configure -ptree]

    if { $ptree > -1 } { $tree:$node configure -ptree -1 }
    if { $model > -1 } {
      set dsname [$tree.modelSet name $model]
      $dss add $dsname [$tree.modelSet.codebookSet name \
                       [$tree.modelSet:$dsname configure -cbX]]
    }
  }
}

# ------------------------------------------------------------------------
#  treeQuestion   find a question for a given node in the tree (if there
#                 is a polyphonic tree attached to the node).
# ------------------------------------------------------------------------

proc treeQuestion { args } {

  itfParseArgv treeQuestion $args [list [
    list <tree>        object  {} tree     Tree {tree}                ] [
    list <node>        string  {} node     {}   {node name}           ] [
    list <questionSet> object  {} qus      QuestionSet {question set} ] [
    list <parent>      string  {} parent   {}   {parent name}         ] [
    list <nodes>       string  {} nodesA   {}   {nodes array}         ] [
    list <count>       string  {} countA   {}   {count array}         ] ]

  upvar $countA count
  upvar $nodesA nodes

  if { [set p [$tree:$node configure -ptree]] >= 0} {
    set c     [$tree.ptreeSet.item($p) configure -count]

    set question [$tree question $node -questionSet $qus]
    set score    [lindex $question 1]
    set question [lindex $question 0]

    writeLog stderr "$tree question $node -questionSet $qus: $question ($score)"
    if { [string length $question] } {
      lappend nodes($score) [list $node $parent $question $c]

      if {! [array exist count] || ! [info exist count($parent)]} { 
        set count($parent) 0
      }
    }
  }
}

# ------------------------------------------------------------------------
#  treeCluster   clusters tree given a set of questions, the minimum
#                number of counts expected to be in the ModelArray for
#                each answer node, the minimum count and the maximum
#                number of splits for each node.
# ------------------------------------------------------------------------

proc treeCluster { args } {

  set maxSplit   30
  set minCount   1000
  set saveN      1000
  set saveX      0
  set saveIdx    0
  set newNodes   ""
  set file       ""
  set nodeList   ""

  itfParseArgv treeCluster $args [list [
    list "<tree>"        object  {} tree     Tree        {tree}       ] [
    list "<questionSet>" object  {} qus      QuestionSet {question set}       ] [
    list "-file"         string  {} file     {}          {cluster log file}   ] [
    list "-nodeList"     string  {} nodeList {}          {list of nodes}      ] [
    list "-minCount"     float   {} minCount {}          {minimum count}      ] [
    list "-maxSplit"     float   {} maxSplit {}          {max.number of split}] ]

  if {$nodeList == ""} { set nodeList   [treePTreeList $tree] }

  foreach name $nodeList { 
    treeQuestion $tree $name $qus $name nodes count
  }

  # open file to save the list of splits

  if [string length $file] { set FO [open $file w] } else { set FO stdout }

  set scores [lsort -real -decreasing [array names nodes]]

  while { [llength $scores] } {
    set   score [lindex $scores 0]
    set   nlist $nodes($score)
    unset nodes([lindex $scores 0])

    set lst ""

    foreach node $nlist {
      set name   [lindex $node 0]
      set parent [lindex $node 1]
      set quest  [lindex $node 2]
      set cnt    [lindex $node 3]

      if { [string length $quest] && $cnt > $minCount &&\
           [set c $count($parent)] < $maxSplit } {

        writeLog stderr "$tree split $name $quest ${parent}($c)\
                                       ${parent}([expr $c+1])\
                                       ${parent}([expr $c+2])"

        puts $FO "$score $name \{$quest\} ${parent}($c)\
                                          ${parent}([expr $c+1])\
                                          ${parent}([expr $c+2])"

        $tree split $name $quest ${parent}($c) ${parent}([expr $c+1])\
                                               ${parent}([expr $c+2])
        incr count($parent) 3   

        for {} { $c < $count($parent)} { incr c} {
          if { [set idx [$tree index ${parent}($c)]] > -1} {
            treeQuestion $tree ${parent}($c) $qus $parent nodes count
          }
        }
      } else { lappend newNodes "$name" }
    }
    if [array exists nodes] { set scores [lsort -real -decreasing [array names nodes]]
    } else                  { set scores {}}
  }

  if [string length $file] { close $FO }

  return $newNodes
}

# ------------------------------------------------------------------------
#  treeReadSplits  reads cluster log file into an array indexed by the
#                  gain of each split. This array is used to split
#                  a decision tree.
# ------------------------------------------------------------------------

proc treeReadSplits { args } {

  set lst   ""

  itfParseArgv treeReadSplits $args [list [
    list "<files>"  string {} file  {} {cluster log files}  ] [
    list "-list"    string {} lst   {} {initial split list} ] ]

  set flist [glob $file]

  while {[llength $flist]} {
    set f     [lindex $flist 0]
    set flist [lrange $flist 1 end]
    set FI    [open $f r]
    set l     ""
    set line  ""
    set read  1

    writeLog stderr "treeReadSplits $f"

    while { 1 } {
      if { $read} {
        if { [gets $FI line] < 0} { foreach x $lst { lappend l $x }; break }
      }
      if {! [llength $lst]} {
        lappend l $line
        while { [gets $FI line] >= 0} { lappend l $line }
        break
      }
      if { [lindex $line 0] > [lindex [lindex $lst 0] 0]} {
        lappend l $line
        set read  1
      } else { 
        lappend l [lindex $lst 0]
        set lst   [lrange $lst 1 end] 
        set read  0
      }
    }
    close $FI
    set lst $l
  }
  return $lst
}


# ------------------------------------------------------------------------
#  treeSplit
# ------------------------------------------------------------------------

proc treeSplit { tree args } {

  set nodeList   [treePTreeList $tree]
  set count      [llength $nodeList]
  set i          0

  itfParseArgv treeSplit $args [list [
    list "<list>"  string {} lst    {} {split list}      ] [
    list "-leafN"  int    {} leafN  {} {number of leafs} ] ]

  foreach node $lst {

    set name   [lindex $node 1]
    set quest  [lindex $node 2]
    set nodeN  [lindex $node 3]
    set nodeY  [lindex $node 4]
    set nodeU  [lindex $node 5]
   
    if {[$tree index $name] > -1 } {
      writeLog stderr "$tree split $name $quest $nodeN $nodeY $nodeU"

      $tree split $name $quest $nodeN $nodeY $nodeU
      incr  count -1

      foreach id "$nodeN $nodeY $nodeU" {
        if { [set idx [$tree index $id]] > -1} { incr count }
      }
      if { $count >=$leafN } return
    }
    incr i
  }
  return [lrange $lst $i end]
}

# ------------------------------------------------------------------------
#  treeSplitStat
# ------------------------------------------------------------------------

proc treeSplitStat { args } {

  set s         0
  set out       ""

  itfParseArgv treeSplitStat $args [list [
    list "<list>"  string {} lst    {} {split list}      ] [
    list "-leafN"  int    {} leafN  {} {number of leafs} ] ]

  for { set i 1 } { $i < 4 } { incr i} {
    set R($i) 0
    set L($i) 0
  }

  foreach node $lst {

    set name   [lindex $node 1]
    set quest  [lindex $node 2]
    set nodeN  [lindex $node 3]
    set nodeY  [lindex $node 4]
    set nodeU  [lindex $node 5]

    for { set i 1 } { $i < 4 } { incr i} {
      set txtR "\\+$i"
      set txtL "\\-$i"
      if { [regexp $txtR $quest]} { incr R($i) }
      if { [regexp $txtL $quest]} { incr L($i) }
    }

    set line $s
    for { set i 1 } { $i < 4 } { incr i} {
       lappend line $L($i) 
       lappend line $R($i)
    }
    lappend out $line
    incr    s
  }
  return $out
}



