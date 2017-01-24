/* ========================================================================
    JANUS      Speech- to Speech Translation
               ------------------------------------------------------------
               Object: Search Tree
               ------------------------------------------------------------

    Author  :  Hagen Soltau
    Module  :  stree.h
    Date    :  $Id: stree.h 2789 2007-03-13 18:05:45Z fuegen $
    Remarks : 

   ========================================================================

    This software was developed by

    the  University of Karlsruhe       and  Carnegie Mellon University
         Dept. of Informatics               Dept. of Computer Science
         ILKD, Lehrstuhl Prof.Waibel        Alex Waibel's NN & Speech Group
         Am Fasanengarten 5                 5000 Forbes Ave
         D-76131 Karlsruhe                  Pittsburgh, PA 15213
         - Germany -                        - USA -

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
    Revision 1.8  2007/03/13 18:05:45  fuegen
    changed childN variables in SRoot and SNode from UCHAR to USHORT
    to allow for decoding with larger vocabularies

    Revision 1.7  2003/08/14 11:20:09  fuegen
    Merged changes on branch jtk-01-01-15-fms (jaguar -> ibis-013)

    Revision 1.6.4.31  2002/12/11 09:45:10  soltau
    Added canonical start index

    Revision 1.6.4.30  2002/11/13 09:59:04  soltau
    - Start and End words rely on LVX now
    - Changes for optional words
    - single phone are allowed to have dynamic LVX now

    Revision 1.6.4.29  2002/07/18 12:13:18  soltau
    optimized stree structures: bestscore -> bestX, worstscore -> worstX

    Revision 1.6.4.28  2002/06/28 16:05:40  soltau
    Deleted streeCheck_*

    Revision 1.6.4.27  2002/02/01 14:17:20  fuegen
    moved stypes.h into slimits.h

    Revision 1.6.4.26  2002/01/14 13:37:26  soltau
    Added reverse search tree

    Revision 1.6.4.25  2002/01/09 17:06:36  soltau
    changed child access in stree

    Revision 1.6.4.24  2001/12/17 12:00:24  soltau
    Removed position mapper

    Revision 1.6.4.23  2001/12/17 09:43:49  soltau
    Deactivation of position mapper

    Revision 1.6.4.22  2001/10/01 10:44:22  soltau
    Use polymorphism defines from slimits

    Revision 1.6.4.21  2001/09/24 14:11:14  soltau
    Added ltree argument for check routines

    Revision 1.6.4.20  2001/07/12 18:48:55  soltau
    Added SDPhones

    Revision 1.6.4.19  2001/06/22 15:33:25  soltau
    Added Tree memory management

    Revision 1.6.4.18  2001/05/09 15:00:20  metze
    Made the bitmask for hashkey generation an LCT

    Revision 1.6.4.17  2001/04/27 07:39:24  metze
    General access functions to LM via 'LCT' implemented

    Revision 1.6.4.16  2001/04/23 13:45:16  soltau
    added streeReinit

    Revision 1.6.4.15  2001/04/12 19:21:09  soltau
    removed same_pron table

    Revision 1.6.4.14  2001/04/12 18:43:04  soltau
    same_pron support

    Revision 1.6.4.13  2001/03/22 17:09:54  soltau
    Added siphoneCheck

    Revision 1.6.4.12  2001/03/20 17:07:39  soltau
    Added sort routines

    Revision 1.6.4.11  2001/03/20 13:31:58  soltau
    Added sanity check routines

    Revision 1.6.4.10  2001/03/13 18:00:38  soltau
    Removed p2l

    Revision 1.6.4.9  2001/03/12 18:44:16  soltau
    added tryMax for lct hashtable

    Revision 1.6.4.8  2001/02/27 13:42:36  soltau
    SIPhone start -> SIPhone* start

    Revision 1.6.4.7  2001/02/21 18:05:45  soltau
    Changed pos2pos handling

    Revision 1.6.4.6  2001/02/16 17:45:11  soltau
    Added typeinfo

    Revision 1.6.4.5  2001/02/15 18:00:58  soltau
    Added routines to deactivate nodes
    Added position mapper

    Revision 1.6.4.4  2001/02/02 16:07:13  soltau
    Added SWord -> SLeaf map

    Revision 1.6.4.3  2001/02/01 18:39:43  soltau
    Added nr. of nodes and leafs

    Revision 1.6.4.2  2001/01/31 14:08:55  soltau
    redesign

    Revision 1.6.4.1  2001/01/29 18:03:21  soltau
    Added SinglePhone words
    Added Memory Management

    Revision 1.6  2001/01/25 11:00:35  soltau
    temporary checkin to make Christian happy

    Revision 1.5  2001/01/25 08:52:20  soltau
    *** empty log message ***

    Revision 1.4  2001/01/19 16:01:34  soltau
    backtrace.h -> strace.h

    Revision 1.3  2001/01/18 13:40:24  soltau
    Removed definitions for token structs (now in backtrace)
    Added table of root pointers
    Added basephone inded for roots and nodes

    Revision 1.2  2001/01/16 07:19:39  soltau
    Saved wrong idea (diphone and LCM mapper not needed)

    Revision 1.1  2001/01/12 18:20:18  soltau
    Initial revision


   ======================================================================== */

#ifndef _stree
#define _stree

#include "slimits.h"
#include "sglobal.h"
#include "phmm.h"
#include "xhmm.h"
#include "svocab.h"
#include "svmap.h"
#include "strace.h"
#include "smem.h"
#include "bmem.h"

/* ========================================================================
    Search Tree Root
   ======================================================================== */

struct SRoot_S {
  CTX      phoneX;     /* basephone index                                   */
  PHMMX    xhmmX;      /* left context model index                          */ 
  LPM      morphN;     /* nr. of linguistic morphed root instances          */
  LPM      allocN;     /* nr. of allocated instances                        */
  LPM      bestX;      /* index of best instance                            */
  LPM      worstX;     /* index of worst instance                           */
  LPM*     lct2pos;    /* map : LCT -> heap position                        */
  LVX      lnX;        /* LM Lookahead tree node index                      */
  RIce**   iceA;       /* array of root instances                           */
  SNode**  childPA;    /* array  of children                                */
  USHORT   childN;     /* number of children                                */
};

/* ========================================================================
    Search Tree Node/Leaf
   ======================================================================== */

/* Note:
   The SNode struct covers nodes and leafs of the search tree to avoid
   pointer overhead in the tree structure. Several parameters are used
   in a different way for node and leaf instances.
*/

struct SNode_S {
  CTX      phoneX;    /* basephone index                                    */
  LPM      morphN;    /* nr. of linguistic morphed node instances           */
  LPM      allocN;    /* nr. of allocated instances                         */
  LPM      bestX;      /* index of best instance                            */
  LPM      worstX;     /* index of worst instance                           */
  LEVEL    level;     /* search tree level                                  */
  UCHAR    info;      /* unqiue info                                        */  
  union {
    PHMMX   hmm;      /* unique phmm model index                            */
    PHMMX  xhmm;      /* right context model index                          */
  } X;
  union {
    LVX       l;      /* LM Lookahead tree node index (for nodes)           */
    SVX       s;      /* search vocabulary word index (for leafs)           */
  } vX;
  union {
    NIce**    n;      /* array of node instances                            */
    LIce**    l;      /* array of leaf instances                            */
  } iceA;
  SNode**  childPA;    /* array  of children                                */
  USHORT   childN;     /* number of children                                */
};

/* Flags to mark if a node is unqiue, e.g the fan-out of the remaining
   nodes of the path is 1. To avoid multiple exaxt lm score calls, we
   need an additional WordWasUnique flag.
*/

enum { 
  WordIsNotUnique = 0, 
  WordIsUnique    = 1,  
  WordWasUnique   = 2  
};

/* ========================================================================
    Single Phone Word (context independent)
   ======================================================================== */

struct SIPhone_S {
  CTX      phoneX;     /* basephone index                                   */
  PHMMX    hmmX;       /* unique phmm model index                           */ 
  LPM      morphN;     /* nr. of linguistic morphed word instances          */
  LPM      allocN;     /* nr. of allocated instances                        */
  LPM      bestX;      /* index of best instance                            */
  LPM      worstX;     /* index of worst instance                           */
  LPM*     lct2pos;    /* map : LCT -> heap position                        */
  SVX      svX;        /* SVocab word index                                 */
  NIce**   iceA;       /* array of instances                                */
};

/* ========================================================================
    Single Phone Word (context dependent)
   ======================================================================== */

struct SDPhone_S {
  CTX      phoneX;     /* basephone index                                   */
  PHMMX    hmmX;       /* unique context model index                        */ 
  LPM      morphN;     /* nr. of linguistic morphed word instances          */
  LPM      allocN;     /* nr. of allocated instances                        */
  LPM      bestX;      /* index of best instance                            */
  LPM      worstX;     /* index of worst instance                           */
  LPM*     lct2pos;    /* map : LCT -> heap position                        */
  SVX      svX;        /* SVocab word index                                 */
  XIce**   iceA;       /* array of instances                                */
};

/* ========================================================================
    Search Tree 
   ======================================================================== */

enum {
  type_NIL     = 0,
  type_SRoot   = 1,
  type_SNode   = 2,
  type_SLeaf   = 3,
  type_SIPhone = 4,
  type_SDPhone = 5
};

struct STree_S {
  char*        name;
  int          useN;

  SVMap*       svmapP;          /* search vocabulary mapper                 */
  SVocab*      svocabP;         /* search vocabulary                        */
  Dictionary*  dictP;           /* pronunciation dictionary                 */
  PHMMSet*     hmmsP;           /* set of phone hidden markov models        */
  LCMSet*      lcmsP;           /* set of left  context models              */
  RCMSet*      rcmsP;           /* set of right context models              */
  XCMSet*      xcmsP;           /* set of full context models               */

  int          compress;        /* compression flag                         */
  
  struct {                      /* SRoot pointer table                      */
    int        N;               /* number of roots                          */
    int        allocN;          /* number of allocated root pointers        */
    int        blkSize;         /* block size                               */
    SRoot**    PA;              /* array of roots pointers                  */
  } rootTbl;

  struct {
    int        N;               /* number of vocabulary words               */
    int        nodeN;           /* number of nodes                          */
    int        leafN;           /* number of leafs                          */
    UCHAR*     typeA;           /* type info for each sword                 */
    void**     A;               /* map sword -> SLeaf/SIPhone/SDPhone       */
  } nodeTbl;

  struct {                      /* SPhone word table                        */
    int        iN;              /* number of context independent models     */
    int        dN;              /* number of context   dependent models     */
    SIPhone*   iA;              /* array  of context independent models     */
    SDPhone*   dA;              /* array  of context   dependent models     */
  } sTbl;

  struct {
    int        N;               /* number of start words                    */
    UCHAR*     typeA;           /* type info for each sword                 */
    void**     A;               /* map sword -> SRoot/SIPhone/SDPhone       */
    SVX        svX;             /* canonical start index                    */
  } startTbl;

  struct {                      /* info to create lct2pos map               */
    int        hashSize;        /* size of hash table to store heap pos.    */
    LCT        bitmask;         /* mask to generate hashkey from lct        */
    int        offset;          /* offset for hash conflicts                */
    int        tryMax;          /* nr. for offset shifts                    */
  } lct2pos;

  SMem         smem;            /* dynamic instance memory management       */

  struct {                      /* Tree memory management                   */
    BMem*     root;             /* blocks of roots                          */
    BMem*     node;             /* blocks of nodes                          */
    BMem*     ptr;              /* blocks of pointer                        */
  } mem;

};

/* ========================================================================
    Reverse Search Tree 
   ======================================================================== */

struct RNode_S {
  void*  nodeP;    
  RNode* parentP;    
  RNode* nextP;
};

struct RArray_S {
  RNode* A;
  int    X;
};

/* ========================================================================
    Module Interface
   ======================================================================== */

extern int STree_Init();

extern int  streeReinit       (STree* streeP);

extern void srootDeactivate   (SRoot* srootP, STree* streeP);
extern void snodeDeactivate   (SNode* snodeP, STree* streeP);
extern void sleafDeactivate   (SNode* sleafP, STree* streeP);
extern void siphoneDeactivate (SIPhone* sipP, STree* streeP);
extern void sdphoneDeactivate (SDPhone* sdpP, STree* streeP);

extern void srootSort         (SRoot* srootP, STree* streeP);
extern void siphoneSort       (SIPhone* sipP, STree* streeP);
extern void sdphoneSort       (SDPhone* sdpP, STree* streeP);

#endif
