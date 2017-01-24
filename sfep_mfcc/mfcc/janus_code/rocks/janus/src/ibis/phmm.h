/* ========================================================================
    JANUS      Speech- to Speech Translation
               ------------------------------------------------------------
               Object: Phone Hidden Markov Model
               ------------------------------------------------------------

    Author  :  Hagen Soltau
    Module  :  phmm.h
    Date    :  $Id: phmm.h 2765 2007-02-22 16:42:38Z fuegen $
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
    Revision 1.6  2007/02/22 16:42:38  fuegen
    added some comments

    Revision 1.5  2003/08/14 11:20:05  fuegen
    Merged changes on branch jtk-01-01-15-fms (jaguar -> ibis-013)

    Revision 1.4.4.9  2003/07/08 16:05:43  soltau
    fixed snXA

    Revision 1.4.4.8  2002/06/28 16:04:33  soltau
    Replaced UCHAR by PSTATE to encode the hmm states

    Revision 1.4.4.7  2002/02/01 14:17:20  fuegen
    moved stypes.h into slimits.h

    Revision 1.4.4.6  2001/07/14 15:34:58  soltau
    Changed type for hmmEvalSdpFn

    Revision 1.4.4.5  2001/06/28 10:48:22  soltau
    Added Load/save routines for binary files to support fast decoder startup

    Revision 1.4.4.4  2001/06/26 15:23:44  soltau
    Changed memory management

    Revision 1.4.4.3  2001/02/09 17:44:32  soltau
    Added vit's for sip

    Revision 1.4.4.2  2001/02/01 18:40:54  soltau
    replaced typedefs to sglobal

    Revision 1.4.4.1  2001/01/29 17:59:49  soltau
    CONTEXT -> CTX

    Revision 1.4  2001/01/24 14:08:07  soltau
    Optimized malloc routines

    Revision 1.3  2001/01/19 16:02:00  soltau
    backtrace.h -> strace.h

    Revision 1.2  2001/01/18 13:36:53  soltau
    Changed arguments in viterbi functions
    Changed struct for Phone hidden markov model
    Added context hash table for phmm's

    Revision 1.1  2000/12/27 18:06:55  soltau
    Initial revision

 
   ======================================================================== */

#ifndef _phmm
#define _phmm

#include <limits.h>
#include "slimits.h"
#include "sglobal.h"
#include "bmem.h"
#include "list.h"
#include "senones.h"
#include "tree.h"
#include "trans.h"
#include "topo.h"
#include "word.h"

/* ========================================================================
    Hidden Markov Model Eval Function
   ======================================================================== */

enum {
  hmmEvalFnRoot = 0,
  hmmEvalFnNode = 1,
  hmmEvalFnLeaf = 2,
  hmmEvalFnSip  = 3,
  hmmEvalFnSdp  = 4
};

typedef float (HmmEvalRootFn) (float* ascoreA, SRoot* rootP, 
			       PHMM* hmmOffsetP);
typedef float (HmmEvalNodeFn) (PHMM* hmmP, float* ascoreA, SNode* nodeP);
typedef float (HmmEvalLeafFn) (RCM*  rcmP, float* ascoreA, SNode* leafP,
			       PHMM* hmmOffsetP);
typedef float (HmmEvalSipFn)  (PHMM* hmmP, float* ascoreA, SIPhone* sipP);
typedef float (HmmEvalSdpFn)  (XCM*  xcmP, float* ascoreA, SDPhone* sdpP,
			       PHMM* hmmOffsetP);


/* ========================================================================
    Hidden Markov Model Context
   ======================================================================== */

typedef struct {
  UCHAR   phoneN;        /* number of phones                   */
  CTX*    phoneA;        /* array of phones and tags with the format [phoneN phones, size of left context, phoneN tags, HMM eval function type] */
  int     hmmX;          /* corresponding PHMM index (need -1) */
} PHMMC;

typedef struct LIST(PHMMC) PHMMCList;

/* ========================================================================
    Hidden Markov Model
   ======================================================================== */

struct PHMM_S {
  PSTATE       stateN;   /* number of HMM states                               */
  PHMMX*       snXA;     /* sequence of senone indices                         */
  PSTATE*      transNA;  /* number of transitions per state                    */
                         /* transNA == NULL indicate optimized version         */
  PSTATE**     transXA;  /* sequence of transition indices per state           */
  union {                /* array of transition probabilities per state        */
    float*     A;        /* flaten array for optimized evalFunction            */
    float**    AA;       /* array per state and trans for general evalFunction */
  } prob;
  union {                /* HMM Viterbi Function                               */
    HmmEvalRootFn*  e0;  /* roots with multiple left contexts                  */
    HmmEvalNodeFn*  e1;  /* nodes with unique left and right context           */
    HmmEvalLeafFn*  e2;  /* leafs with multiple right contexts                 */
    HmmEvalSipFn*   e3;  /* context independet single phone words              */
    HmmEvalSdpFn*   e4;  /* context dependet single phone words                */
  } vitFn;
};

/* ========================================================================
    Set of Hidden Markov Models
   ======================================================================== */
 
typedef struct LIST(PHMM) PHMMList;

struct PHMMSet_S {
  char*         name;
  int           useN;

  PHMMList      list;

  Tree*         treeP;    /* topology tree                */
  int           treeX;    /* root of topo. tree           */
  TopoSet*      tpsP;     /* underlying topology   models */
  TmSet*        tmsP;     /* underlying transition models */
  SenoneSet*    snsP;     /* underlying senone set        */
  int*          topoMap;  /* cash map topoX -> vitX       */

  struct {                /* memory management            */
    BMem*       c;        /* block of PSTATES             */
    BMem*       s;        /* block of shorts              */
    BMem*       f;        /* block of floats              */
  } mem;

  struct {                /* map context -> phmm index    */
    PHMMCList  list;
    int        leftMax;   
    int        rightMax;
    int        use;
    BMem*      memP;   
  } ctx;
};

/* ========================================================================
    Module Interface
   ======================================================================== */

extern int      PHMM_Init();
extern PHMMSet* phmmSetCreate   (char* name,Tree* treeP, int treeX, int useCtx);
extern int      phmmSetFree     (PHMMSet *hmmsP);
extern int      phmmSetAdd      (PHMMSet* hmmsP, Word* wP, int left, int right, \
				 int nodeFnType);
extern int      phmmSetClearCtx (PHMMSet* hmmsP);
extern int      phmmSetLoad     (PHMMSet* hmmsP, FILE* fp);
extern int      phmmSetSave     (PHMMSet* hmmsP, FILE* fp);

#endif
