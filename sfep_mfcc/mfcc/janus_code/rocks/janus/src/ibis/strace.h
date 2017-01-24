/* ========================================================================
    JANUS      Speech- to Speech Translation
               ------------------------------------------------------------
               Object: SPASS Backpointer Table
               ------------------------------------------------------------

    Author  :  Hagen Soltau
    Module  :  strace.h
    Date    :  $Id: strace.h 2731 2006-11-14 16:41:44Z fuegen $
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
    Revision 1.4  2006/11/14 16:41:44  fuegen
    support for endless decoding with partial hypothesis
    - bugfix for spass.pathSub method, which subtracts a constant score
      from all active paths
    - new method spass.traceStable which returns the stable hypotheses
      together with a list of backpointers for the next traceStable call

    Revision 1.3  2003/09/01 14:51:30  metze
    Added -frameIdx option to STrace

    Revision 1.2  2003/08/14 11:20:09  fuegen
    Merged changes on branch jtk-01-01-15-fms (jaguar -> ibis-013)

    Revision 1.1.4.13  2003/08/13 08:59:47  fuegen
    added stabGetFinalBPX to get the final BP

    Revision 1.1.4.12  2002/11/13 10:06:15  soltau
    STab rely now on SVMap directly

    Revision 1.1.4.11  2002/02/01 14:17:20  fuegen
    moved stypes.h into slimits.h

    Revision 1.1.4.10  2001/10/12 17:06:54  soltau
    Improved Path recombination

    Revision 1.1.4.9  2001/10/01 10:46:42  soltau
    Use polymorphism defines from slimits

    Revision 1.1.4.8  2001/07/24 17:11:32  soltau
    Added stabFinalize

    Revision 1.1.4.7  2001/06/01 10:38:53  soltau
    Added stabGetFrame

    Revision 1.1.4.6  2001/04/23 13:44:37  soltau
    removed delta from bp struct

    Revision 1.1.4.5  2001/03/14 12:13:02  soltau
    Changed Interface

    Revision 1.1.4.4  2001/03/12 18:47:51  soltau
    Added bpTabClear

    Revision 1.1.4.3  2001/02/27 13:50:13  soltau
    Added preliminary traceback function

    Revision 1.1.4.2  2001/02/16 17:48:03  soltau
    B* bp -> int bpIdx

    Revision 1.1.4.1  2001/02/02 16:06:29  soltau
    Added Instances for Roots, Nodes, and Leafs
    Added table of backpointers

    Revision 1.1  2001/01/19 16:03:47  soltau
    Inital version

 
   ======================================================================== */

#ifndef _strace
#define _strace

#include "slimits.h"
#include "sglobal.h"

/* ========================================================================
    Backpointer
   ======================================================================== */

struct BP_S {
  SVX    svX;         /* index of last word in this theory                  */
  float  score;       /* score for this theory                              */
  LPM    morphX;      /* SLeaf instance (needed only temp. for spassExpand) */
  int    bpIdx;       /* backpointer index to the last but one word         */
};

/* ========================================================================
    Token for Search Roots and Nodes
   ======================================================================== */

struct RTok_S {
  int   bpIdx;        /* index to a item in the Backpointer table           */
  float s;            /* score for this theory                              */
  PHMMX hmmX;         /* Phone HMM index (needed for acoustic polymorphism) */
};

struct NTok_S {
  int   bpIdx;        /* index to a item in the Backpointer table           */
  float s;            /* score for this theory                              */
};

/* ========================================================================
    Hashkey for linguistic state
   ======================================================================== */

typedef struct LKey_S {
  LCT     lct;
  int     bpX;
} LKey;

typedef struct LIST(LKey) LCTList;

/* ========================================================================
    Table of Backpointers
   ======================================================================== */

struct STab_S {
  SVMap* svmapP;      /* underlying svmap object to interpret backpointer   */

  BP*   A;            /* array of backpointers                              */
  int   N;            /* number of used      backpointers                   */
  int   allocN;       /* number of allocated backpointers                   */
  int   blkSize;      /* block size for bp allocation                       */

  LCTList  l2x;       /* haskfunction to map from lct to bpIdx              */
  int*  useNA;        /* number of used backpointer per frame               */
  int   frameN;       /* number of used      useN's                         */
  int   frameAllocN;  /* number of allocated useN's                         */
  int   frameBlkSize; /* block size for allocating useNA array              */

  BP**  wordXA;       /* wordXA[svX] = backpointer for sword in this frame  */
};

/* ========================================================================
    Module Interface
   ======================================================================== */

extern STab* stabCreate      (SVMap* svmapP);
extern int   stabFree        (STab* tabP);
extern BP*   stabAlloc       (STab* tabP, int frameX, LCT lct);
extern int   stabPrune       (STab* tabP, int frameX, int topN);
extern int   stabClear       (STab* tabP);
extern int   stabFinalize    (STab* tabP);
extern int   stabGetFrame    (STab* tabP, int bpIdx);
extern int   stabGetFinalBPX (STab *tabP, int *bpX, int *fstate, int frameX, int verbose);
extern int   stabTraceStable (STab *tabP, int fromX, int toX, int *endA, int endN, int *abpA, int abpN, int verbose);


int STab_Init (void);

#endif
