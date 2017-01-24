/* ========================================================================
    JANUS      Speech- to Speech Translation
               ------------------------------------------------------------
               Object: A^* Search
               ------------------------------------------------------------

    Author  :  Klaus Ries
    Module  :  astar.h
    Date    :  $Id: astar.h 512 2000-08-27 15:31:32Z jmcd $
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
    Revision 1.3  2000/08/27 15:31:07  jmcd
    Merging branch 'jtk-00-08-24-jmcd'.

    Revision 1.2.4.1  2000/08/25 22:19:42  jmcd
    Just checking.

    Revision 1.2  1998/06/11 15:45:40  kries
    changes for lat - class

 * Revision 1.1  1998/05/26  18:38:33  kries
 * Initial revision
 *

 
   ======================================================================== */

#ifdef __cplusplus
extern "C" {
#endif

#include "lm.h"

#ifndef _astar
#define _astar

#include "rbt.h"

#define INITASTARMEMORYBLOCKING 1024
#define MAXASTARMEMORYBLOCKING 1024*1024


/* ------------------------------------------------------------------------
   AStarPath
   ------------------------------------------------------------------------ */

#define TYPEDEF(A)  struct A ## _TypeDummy { int dummy; }; typedef struct A ## _TypeDummy* A;

TYPEDEF(AStarArcP)
TYPEDEF(AStarNodeP)
TYPEDEF(AStarSearchStateP)

#undef TYPEDEF


typedef struct AStarPath_S AStarPath;

struct AStarPath_S {
  int useN;
  AStarPath* parentP;

  AStarNodeP        AStarElement;
  LMStateMem        StateMem;
};


typedef struct AStarNode_S  AStarNode;

struct AStarNode_S {
  AStarNodeP        AStarElement;
  AStarPath *       pathP;

  long              hashValue;          /* the actual hashvalue */
  union {
    long            hashValueDynamic;   /* a value useful for dynamic programming calculation */
    AStarNode *     nextFree;           /* the index of the next free AStarNode               */
  } un;

  float             score;              /* pathScore + lookahead */
  float             pathScore;          /* score on this path so far */

  AStarSearchStateP searchState;        /* state of the search itself */
  LMState           State;              /* state of LM */

};



typedef struct AStarPathMemory_S  AStarPathMemory;
struct AStarPathMemory_S {
  int size,pathSize;
  AStarPath * paths;
  AStarPathMemory * next;
};




/* ------------------------------------------------------------------------
   AStar function types
   ------------------------------------------------------------------------ */


typedef struct AStar_S AStar;

typedef void       AStarPathBacktraceFct( AStar * searchObjectP, int hypoN, AStarPath** hypoP, AStarNode * nodeP);

typedef int        AStarNodeElementFct(AStar * searchObjectP, AStarNodeP);
typedef int        AStarPathEndFct(AStar *, AStarNode *);

typedef AStarArcP  AStarNodeOutputArcsFct(AStar *, AStarNode *); /* Begin of search == NULL */
/* returns NULL at last arc, arcs must be deinited by AStarNextOutputArcsFct */
typedef AStarArcP  AStarNextOutputArcsFct(AStar *, AStarArcP);  

typedef AStarNodeP AStarArcNodeFct(AStar *, AStarArcP);

typedef int        AStarSkipOutputArcsFct(AStar *, AStarNode *);
typedef float      AStarArcTransitionFct(AStar *, AStarArcP);
typedef float      AStarNodeLookaheadFct(AStar *, AStarNode* pP);
typedef int        AStarNodeTimeFct(AStar * searchObjectP, AStarNode* nodeP);


typedef AStarSearchStateP  AStarSearchStateCreateFct(AStar *);
typedef void               AStarSearchStateCopyFct(AStar *, AStarSearchStateP, AStarSearchStateP); /* to, from */
typedef void               AStarSearchStateFreeFct(AStar *,AStarSearchStateP);
typedef long               AStarSearchStateHashFct(AStar *,AStarSearchStateP);
typedef int                AStarSearchStateEqFct(AStar *,AStarSearchStateP,AStarSearchStateP);
typedef AStarSearchStateP AStarArcSearchStateFct(AStar *, AStarArcP);


/* ------------------------------------------------------------------------
   AStar 
   ------------------------------------------------------------------------ */

struct AStar_S {
  ClientData searchStructureP;
  LMPtr     lm;
  int       n; /* Number of hypos to extract */
  int       context; /* Size of the context for equality calculation */
  int       maxPathN; /* Size of the A^* stack */

  RBT *     boundedQueueP;
  RBT *     outputQueueP;

  int       hypTriedN,maxHypTriedN;
  int       hypRejectN,hypoReplacedN;

  /* Memory management for paths */
  AStarPath*  free_list;
  AStarPathMemory* mem_list;

  
  /* Memory management for paths */
  AStarNode*  free_node_list;
  AStarNode*  mem_node_array;


  /**************
             THIS PART NEEDS TO BE CONFIGURED
   **************/
  /* Walk through the search graph */

  AStarNodeElementFct    * nodeElement;

  AStarNodeLookaheadFct  * nodeLookahead;
  AStarArcTransitionFct  * arcTransition;
  AStarNodeTimeFct       * nodeTime;

  AStarPathEndFct        * pathEnd;

  AStarNodeOutputArcsFct * nodeOutputArcs; /* Begin of search == NULL */
  AStarNextOutputArcsFct * nextOutputArcs;
  AStarSkipOutputArcsFct * skipOutputArcs; /* reasonable default */

  AStarArcNodeFct        * arcNode;

  /* To define a certain extraction of the hypo for other programs */
  AStarPathBacktraceFct * pathBacktrace;

  /* If the search has its own states, if not should be NULL */

  AStarSearchStateCreateFct * searchStateCreate;
  AStarSearchStateFreeFct * searchStateFree;
  AStarSearchStateHashFct * searchStateHash;
  AStarSearchStateEqFct   * searchStateEq;
  AStarSearchStateCopyFct * searchStateCopy; /* to, from */
  AStarArcSearchStateFct  * arcSearchState;


};


/* ------------------------------------------------------------------------
    Function Declarations
   ------------------------------------------------------------------------ */

extern int AStarSearch(AStar* searchObjectPtr );

#endif

#ifdef __cplusplus
}
#endif
