/* ========================================================================
    JANUS      Speech- to Speech Translation
               ------------------------------------------------------------
               Object: Generic Red and Black Search Tree
               ------------------------------------------------------------

    Author  :  Klaus Ries
    Module  :  rbt.h
    Date    :  $Id: rbt.h 2390 2003-08-14 11:20:32Z fuegen $
    Remarks :

   ========================================================================

    This software was developed by

    the  University of Karlsruhe       and  Carnegie Mellon University
         Dept. of Informatics               Dept. of Computer Science
         ILKD, Lehrstuhl Prof.Waibel        Alex Waibel's NN & Speech Group
         Am Fasanengarten 5                 5000 Forbes Ave
         D-76131 Karlsruhe                  Pittsburgh, PA 15213
         - West Germany -                   - USA -

    This software is part of the JANUS Speech- to Speech Translation Project

    USAGE PERMITTED ONLY FOR MEMBERS OF THE JANUS PROJECT
    AT CARNEGIE MELLON UNIVERSITY OR AT UNIVERSITAET KARLSRUHE
    AND FOR THIRD PARTIES ONLY UNDER SEPARATE WRITTEN PERMISSION
    BY THE JANUS PROJECT

    It may be copied  only  to members of the JANUS project
    in accordance with the explicit permission to do so
    and  with the  inclusion  of  the  copyright  notices.

    This software  or  any  other duplicates thereof may
    not be copied or otherwise made available to any other person.

    No title to and ownership of the software is hereby transferred.

   ========================================================================

    $Log$
    Revision 1.4  2003/08/14 11:20:24  fuegen
    Merged changes on branch jtk-01-01-15-fms (jaguar -> ibis-013)

    Revision 1.3.56.1  2002/06/26 11:57:58  fuegen
    changes for new directory structure support and code cleaning

    Revision 1.3  2000/08/27 15:31:28  jmcd
    Merging branch 'jtk-00-08-24-jmcd'.

    Revision 1.2.4.1  2000/08/25 22:19:45  jmcd
    Just checking.

    Revision 1.2  1998/06/29 23:41:51  kries
    corrected FindNode

 * Revision 1.1  1998/05/26  18:27:23  kries
 * Initial revision
 *
 
   ======================================================================== */

#ifdef __cplusplus
extern "C" {
#endif


/* ========================================================================

                        A L G O R I T H M I C S

   Red and Black Trees are efficient binary search trees that are always
   considerably balanced.
   This implementation is especially useful if one wants to implement bounded
   priority queues and features an efficient free list managment.

   All standard operations are O(log #nodes) since
   * the longest path in the tree is not more than twice as long
     as the shortest
   * maintaining the Red Black properties is cheap

   Walking through the sorted tree is O(log #nodes), all
   estimates are worst case and non-amortized.

   The baseline this implementation is built on:

   Thomas H. Cormen, Charles E. Leiserson, and Ronald L. Rivest.
          Introduction to Algorithms.
   New York: McGraw-Hill, 1992. 

   with some of the initial typing work being done by
   http://www.geocities.com/SoHo/2167/book.html

   There one can also find a speed comparision with Skip Lists and
   other standard implementations of dictionaries or sorted datastructures.
   Red and Black Trees compare fairly well with HashTables, however
   they should be used only when one needs a sorted datastructure.

   Other applications are:
   * unbounded priority queues  (here we could also use heaps)
   *   bounded priority queues  (the reason for this implementation,
                                 specialized memory handling)



               S O F T W A R E    E N G I N E E R I N G
        &                 S E M A N T I C S

   DATA SEMANTICS

   The class is generic, it can work with any datatype that is represented
   with ClientData, a pointer-type.

   As soon as a node is entered into the tree the " ownership " of this
   pointer is transferred to the tree.
   It is therefore guaranteed that as soon as the tree " gives up "
   a this ClientData the data_deinit is called, if it is specified (!=NULL).

   The tree can give up ClientData by explicitly deleting it,
   removing the front or back node or by pruning it.
   Unless the data is pruned one can request that the ownership is
   transferred back (namely no data_deinit is called -- no_data_deinit!=0)


   BOUNDED TREES

   If a maximal treesize is specified the tree is pruning off
   entries that are too low and it is using an efficient memory managment
   allocating nodes for the maximal treesize in one malloc.

   If a new node is added one can imagine that it is added virtually
   and the smallest node in the tree is excluded if the size would
   be exceeded otherwise.
   As an optimization the new node is not added if it would be excluded
   immediately again.

   COMPARISIONS

   The ordering is done as defined by the function  >> is_less <<.

   The standard comparision function cmp(A,B) could be implemented as

   cmp(A,B) = is_less(A,B) ? -1 : ( is_less(B,A) ? 1 : 0)

   It is perfectly possible to have a list of elements with elements
   being equal according to this order.
   The RBT_FindNode procedure is finds the largest node <= the search data.
   This element might not be a node equal according to cmp, but
   if there is such a node the smallest one of these is returned.

   ======================================================================== */



#ifndef _rbt
#define _rbt

#include <stdlib.h>
#include <stdio.h>

#include "common.h"
#include "tcl.h"

#include "itf.h"
#include "hash.h"



/* ------------------------------------------------------------------------
   RBT_Node          the nodes inside of the tree
   ------------------------------------------------------------------------ */


typedef enum { RBT_Black, RBT_Red } RBT_Color;

typedef struct RBT_Node_ {
    struct RBT_Node_ *Left;         /* left child */
    struct RBT_Node_ *Right;        /* right child */
    struct RBT_Node_ *Parent;       /* parent */
    RBT_Color Color;            /* node color (black, red) */
    ClientData Data;                     /* data stored in node */
} RBT_Node;

typedef int RBTOrderingFct(ClientData DataHolder, ClientData a, ClientData b); /* < function on Data */
typedef void RBTDeinitFct(ClientData DataHolder, ClientData obj); 


/* ------------------------------------------------------------------------
   RBT               the tree itself
   ------------------------------------------------------------------------ */

typedef struct RBT_Tree_S {
  RBT_Node *Root;                                 /* root of tree */

  int  nodesN;                         /* number of nodes in tree */
  int  nodesMaxN;             /* >0 for bounded trees, 0 otherwise*/  


  RBT_Node * HighNodeP, * LowNodeP;


  RBT_Node * Nodes;           /* fixed storage for bounded trees  */
  RBT_Node * NodesFree;   /* free list for nodes of bounded trees */

  RBTOrderingFct *is_less; /* < function on Data */
  RBTDeinitFct *data_deinit;          /* deinit for Data    */

  ClientData DataHolder;

  /* Dictionary to manage dublicate elimination and direct access
     according to a second independent equality relation.
     Implementation is similar to the list.c module
  */

  Hash* DataEq;
  HashKeyFn2* hashKey;
  HashCmpFn2* hashCmp;
  int  nodesReplaced; /* incremented each time the DataEq replaced a word */
  int  nodesPruned; /* incremented each time the DataEq replaced a word */

  TypeInfo * ti;
} RBT;


/* ------------------------------------------------------------------------
   Sentinel / NIL
   ------------------------------------------------------------------------ */

#define RBT_NIL (&RBT_Sentinel)           /* all leafs are sentinels */
extern RBT_Node RBT_Sentinel;


/* ------------------------------------------------------------------------
   Initialization
   ------------------------------------------------------------------------ */

int RBT_Init(RBT * t,            /* Pointer to the structure initialized */

	     /* The ordering of elements */
	     RBTOrderingFct *is_less,      /*       != NULL */

	     /* Deinitializing elements in the tree */
	     RBTDeinitFct *data_deinit,                   /* maybe == NULL */
	     ClientData DataHolder,                       /* maybe == NULL */

	     /* Bounded trees */
	     int  capacity,   /* >0 for bounded trees, 0 otherwise           */                             

	     /* In the future we may have special incarnations of RBTs       */
	     TypeInfo * ti,                               /* maybe == NULL */

	     /* If hash-key and compare functions are set up dublication
		can be eliminated
		Equality can be defined fully independently of "is_less"
		Unless both are !=0 this is not activated.
		It is always the smaller element that is eliminated
		in case of conflicts.
	     */
	     HashKeyFn2* hashKey,
	     HashCmpFn2* hashCmp);



/* ------------------------------------------------------------------------
   Finding elements and stepping through the tree, e.g.

   RBT_Node* P;
   for(P=RBT_FindLow(t);
       P;
       P=RBT_FindNext(t,P))
       do_something(t,P);
   }

   Note the comparision for P !!!!!

   ------------------------------------------------------------------------ */

extern RBT_Node *RBT_FindLow(RBT* t);
extern RBT_Node *RBT_FindHigh(RBT* t);
extern RBT_Node *RBT_FindNext(RBT* t, RBT_Node* Current);
extern RBT_Node *RBT_FindPrev(RBT* t, RBT_Node* Current);


/* ------------------------------------------------------------------------
   Finding, inserting, deleting, dequeueing at either end
   ------------------------------------------------------------------------ */

extern RBT_Node *RBT_FindNode(RBT* t, ClientData Data);
extern int RBT_InsertNode(RBT * t, ClientData Data);
extern RBT_Node *RBT_InsertNode_(RBT * t, ClientData Data, RBT_Node * X);
extern void RBT_DeleteNode(RBT* t, RBT_Node *Z, int no_deinit);
extern ClientData RBT_PopLowNode(RBT* t, int no_deinit);
extern ClientData RBT_PopHighNode(RBT* t, int no_deinit);
extern int RBT_Deinit(RBT * t);
extern int RBT_isOK(RBT * t);

#endif




#ifdef __cplusplus
}
#endif
