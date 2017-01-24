/* ========================================================================
    JANUS      Speech- to Speech Translation
               ------------------------------------------------------------
               Object: Search Tree Memory management
               ------------------------------------------------------------

    Author  :  Hagen Soltau
    Module  :  smem.h
    Date    :  $Id: smem.h 2390 2003-08-14 11:20:32Z fuegen $
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
    Revision 4.1  2003/08/14 11:20:08  fuegen
    Merged changes on branch jtk-01-01-15-fms (jaguar -> ibis-013)

    Revision 1.1.2.15  2002/01/10 13:36:30  soltau
    Added svX in ice structures to enable decoding along shared suffixes

    Revision 1.1.2.14  2001/11/20 16:33:48  soltau
    optimized memory management

    Revision 1.1.2.13  2001/10/01 10:44:04  soltau
    Use polymorphism defines from slimits

    Revision 1.1.2.12  2001/07/12 18:44:18  soltau
    Added SDPhones

    Revision 1.1.2.11  2001/06/25 21:07:36  soltau
    another redesign to support arbitrary hmm topologies

    Revision 1.1.2.10  2001/04/23 13:42:38  soltau
    Added smemFree*

    Revision 1.1.2.9  2001/04/12 19:19:56  soltau
    don't alloc floats for leafs if USE_RCM_LM == 0

    Revision 1.1.2.8  2001/04/04 08:55:06  soltau
    Added lm only score to propagate lookahead score without deltas

    Revision 1.1.2.7  2001/03/27 20:48:58  soltau
    stored lm lookahead scores for phones in leaf instances
    Added float blocks

    Revision 1.1.2.6  2001/02/15 09:21:20  soltau
    removed block of floats

    Revision 1.1.2.5  2001/02/09 17:46:00  soltau
    scoreA for leafs not needed anymore

    Revision 1.1.2.4  2001/02/08 09:38:55  soltau
    Added char blocks

    Revision 1.1.2.3  2001/02/02 16:04:59  soltau
    moved typedefs to sglobal

    Revision 1.1.2.2  2001/01/31 14:08:24  soltau
    redesign

    Revision 1.1.2.1  2001/01/29 17:57:28  soltau
    Initial version


   ======================================================================== */

#ifndef _smem
#define _smem

#include "slimits.h"
#include "strace.h"
#include "sglobal.h"
#include "bmem.h"

/* ========================================================================
    linguistic morphed instances for roots, nodes, and leafs
   ======================================================================== */

struct RIce_S {
  LCT      lct;              /* linguistic context id                       */
  SVX      svX;              /* token for sword index                       */
  float    score;            /* score of this root instance                 */
  float    lms;              /* lm only score                               */
  RTok*    path;             /* token to trace back the best hypotheses     */
};


struct NIce_S {
  LCT      lct;              /* linguistic context id                       */
  SVX      svX;              /* token for sword index                       */
  float    score;            /* score of this node instance                 */
  float    lms;              /* lm only score                               */
  NTok*    path;             /* token to trace back the best hypotheses     */
};

struct LIce_S {
  LCT      lct;              /* linguistic context id                       */
  SVX      svX;              /* token for sword index                       */
  float    score;            /* score of this leaf instance                 */
  NTok*    path;             /* flaten array for each context and state     */
  float    lms;              /* lm only score                               */
#if USE_RCM_LM
  float*   lmp;              /* lm lookahead scores for phones              */
#endif
};

struct XIce_S {
  LCT      lct;              /* linguistic context id                       */
  CTX      phoneX;           /* left phonetic context                       */
  float    score;            /* score of this leaf instance                 */
  NTok*    path;             /* flaten array for each context and state     */
  float    lms;              /* lm only score                               */
};


/* ========================================================================
    Search Tree Memory Manager
   ======================================================================== */

struct SMem_S {
  STree*           streeP;      /* associated stree                         */
  SPass*           spassP;      /* associated single pass decoder           */
  int              level;       /* tree level for early mem allocation      */

  int              morphBlkN;   /* block size of instances                  */
  int              morphMax;    /* maximum number of instances              */
  int              smemFree;    /* dealloc. mem. from deactivated instances */

  struct {                      /* blocks of LPM's                          */
    int     blkSize;            /* block size                               */
    BMem*   blk;                /* list of blocks                           */
  } c;
  struct {                      /* blocks of float                          */
    int     blkSize;            /* block size                               */
    BMem*   blk;                /* list of blocks                           */
  } f;
  struct {                      /* blocks of pointer                        */
    int     blkSize;            /* block size                               */
    BMem*   blk;                /* list of blocks                           */
  } p;
  struct {                      /* blocks of root tokens                    */
    int     blkSize;            /* block size                               */
    BMem*   blk;                /* list of blocks                           */
  } r;
  struct {                      /* blocks of root instances                 */
    int     blkSize;            /* block size                               */
    BMem*   blk;                /* list of blocks                           */
  } ri;
  struct {                      /* blocks of node tokens                    */
    int     blkSize;            /* block size                               */
    BMem*   blk;                /* list of blocks                           */
  } n;
  struct {                      /* blocks of node instances                 */
    int     blkSize;            /* block size                               */
    BMem*   blk;                /* list of blocks                           */
  } ni;
  struct {                      /* blocks of leaf instances                 */
    int     blkSize;            /* block size                               */
    BMem*   blk;                /* list of blocks                           */
  } li;
  struct {                      /* container to store the SNode addresses   */
    int      N;                 /* who called the smemAlloc routines to     */
    int      allocN;            /* allow a fast reset of the used memory    */
    SNode**  PA;
  } user;

};

/* ========================================================================
    Module Interface
   ======================================================================== */

extern int   smemInit      (SMem* smemP);
extern int   smemDeinit    (SMem* smemP);
extern int   smemClear     (SMem* smemP);

extern int   smemAllocRoot    (SRoot*   srootP, STree* streeP);
extern int   smemAllocNode    (SNode*   snodeP, STree* streeP);
extern int   smemAllocLeaf    (SNode*   sleafP, STree* streeP);
extern int   smemAllocSIPhone (SIPhone* sipP,   STree* streeP);
extern int   smemAllocSDPhone (SDPhone* sdpP,   STree* streeP);
extern LPM*  smemGetLPM       (SMem* smemP, int lpmN);

extern int   smemFreeRoot    (SRoot*   srootP, STree* streeP);
extern int   smemFreeNode    (SNode*   snodeP, STree* streeP);
extern int   smemFreeLeaf    (SNode*   sleafP, STree* streeP);
extern int   smemFreeSIPhone (SIPhone* sipP,   STree* streeP);
extern int   smemFreeSDPhone (SDPhone* sdpP,   STree* streeP);

extern int   smemDeallocRoot (SRoot*   srootP, STree* streeP);
extern int   smemDeallocNode (SNode*   snodeP, STree* streeP);
extern int   smemDeallocLeaf (SNode*   sleafP, STree* streeP);

extern int   SMem_Init     (void);

#endif
