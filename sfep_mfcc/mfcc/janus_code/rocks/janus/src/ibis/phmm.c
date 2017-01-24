/* ========================================================================
    JANUS      Speech- to Speech Translation
               ------------------------------------------------------------
               Object: Phone Hidden Markov Model
               ------------------------------------------------------------

    Author  :  Hagen Soltau
    Module  :  phmm.c
    Date    :  $Id: phmm.c 3416 2011-03-23 03:02:18Z metze $
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
    Revision 1.7  2005/03/08 08:00:03  metze
    Added MICV support

    Revision 1.6  2003/09/05 15:31:43  soltau
    phmmSetLoad : fixed typecasts

    Revision 1.5  2003/08/14 11:20:05  fuegen
    Merged changes on branch jtk-01-01-15-fms (jaguar -> ibis-013)

    Revision 1.4.4.31  2003/07/14 15:21:58  metze
    Fixed type conversion

    Revision 1.4.4.30  2003/07/11 11:40:02  metze
    Removed warning

    Revision 1.4.4.29  2003/07/09 08:15:15  soltau
    phmmSetLoad : fixed snXA memcpy

    Revision 1.4.4.28  2003/07/08 16:05:28  soltau
    fixed snXA

    Revision 1.4.4.27  2003/05/08 16:41:54  soltau
    Purified!

    Revision 1.4.4.26  2002/10/29 08:59:36  soltau
    phmmSetInit: fixed maximum left context

    Revision 1.4.4.25  2002/10/07 07:35:21  soltau
    vitGeneralSdp : fixed nr. of states

    Revision 1.4.4.24  2002/08/27 08:48:33  metze
    itfParseArgv now uses <name> and NULL initialization for char*

    Revision 1.4.4.23  2002/07/18 12:17:08  soltau
    Changes according to optimized stree structures:
            bestscore -> bestX, worstscore -> worstX

    Revision 1.4.4.22  2002/06/28 16:04:03  soltau
    Replaced UCHAR by PSTATE to encode the hmm states
    Added phmmSetAddGraph to add pronunciation graphs

    Revision 1.4.4.21  2002/06/26 11:57:54  fuegen
    changes for new directory structure support and code cleaning

    Revision 1.4.4.20  2002/03/22 10:49:38  soltau
    Initialize worstscore with -score_max to cover negative acoustic scores

    Revision 1.4.4.19  2001/10/22 09:30:16  soltau
    Fixed wrong transN in phmmSetAdd

    Revision 1.4.4.18  2001/09/20 13:28:55  soltau
    Fixed problems in general viterbi functions

    Revision 1.4.4.17  2001/07/31 19:01:44  soltau
    load phmms into non-empty sets

    Revision 1.4.4.16  2001/07/31 15:46:07  soltau
    added vitGeneralSdp

    Revision 1.4.4.15  2001/07/14 15:28:37  soltau
    Viterbi functions for context dependent single phone words

    Revision 1.4.4.14  2001/06/28 10:48:21  soltau
    Added Load/save routines for binary files to support fast decoder startup

    Revision 1.4.4.13  2001/06/26 15:24:35  soltau
    Changed memory management

    Revision 1.4.4.12  2001/06/25 21:09:04  soltau
    Added general viterbi functions

    Revision 1.4.4.11  2001/06/20 17:25:28  soltau
    *** empty log message ***

    Revision 1.4.4.10  2001/04/03 16:46:53  soltau
    return really *best* score

    Revision 1.4.4.9  2001/03/20 13:31:13  soltau
    Fixed worstscore update for leafs

    Revision 1.4.4.8  2001/03/09 08:54:21  soltau
    Added RCM pruning

    Revision 1.4.4.7  2001/02/27 13:52:15  soltau
    Sun cc beautifying

    Revision 1.4.4.6  2001/02/27 09:04:12  soltau
    Fixed list initialization in phmmSetClearCtx

    Revision 1.4.4.5  2001/02/16 17:47:32  soltau
    BP* bp -> int bpIdx

    Revision 1.4.4.4  2001/02/09 17:45:11  soltau
    scoreA for leafs not needed anymore

    Revision 1.4.4.3  2001/02/08 10:33:05  soltau
    Added a couple of viterbi functions

    Revision 1.4.4.2  2001/02/01 18:41:16  soltau
    NTokA -> NTok*

    Revision 1.4.4.1  2001/01/29 18:00:47  soltau
    CONTEXT -> CTX,
    improved memory blocking

    Revision 1.4  2001/01/24 14:22:55  soltau
    Optimized malloc routines

    Revision 1.3  2001/01/18 13:37:32  soltau
    Fixed a couple of bugs
    Added context models

    Revision 1.2  2001/01/15 19:08:29  soltau
    Fixed return value in phmmSetAdd
    Added access functions

    Revision 1.1  2000/12/27 18:06:45  soltau
    Initial revision

 
   ======================================================================== */

char phmmRcsVersion[]= 
           "@(#)1$Id: phmm.c 3416 2011-03-23 03:02:18Z metze $";

#include "itf.h"
#include "distribStream.h"
#ifndef IBIS
#include "hmeStream.h"
#include "micvStream.h"
#endif
#include "phmm.h"
#include "stree.h"

#include "mach_ind_io.h"

/* ------------------------------------------------------------------------
    forward declarations 
   ------------------------------------------------------------------------ */

extern TypeInfo  phmmInfo;
extern TypeInfo  phmmSetInfo;
static PHMMSet   phmmSetDefault;

/* ========================================================================
    Hidden Markov Model Eval Function
   ======================================================================== */

/* ------------------------------------------------------------------------
    hmm viterbi implementation : general case
   ------------------------------------------------------------------------ */

static float oldscoreA[PSTATE_MAX];
static int   oldBpIdxA[PSTATE_MAX];

float vitGeneralRoot(float* ascoreA, SRoot* rootP, PHMM* hmmOffsetP)
{
  int   morphX      = 0;
  int   morphN      = rootP->morphN;
  float bestscore   =  SCORE_MAX;
  float worstscore  = -SCORE_MAX;

  rootP->bestX  = 0;
  rootP->worstX = 0;

  for (morphX=0;morphX<morphN;morphX++) {
    RTok*  path = rootP->iceA[morphX]->path;
    PHMM* hmm1P = hmmOffsetP + path[1].hmmX;
    float  best = SCORE_MAX;
    int  stateX = 0;

    /* save old tokens (needed for back-loops) */
    for (stateX=0;stateX<=hmm1P->stateN;stateX++) {
      oldscoreA[stateX] = path[stateX].s;
      oldBpIdxA[stateX] = path[stateX].bpIdx;
      path[stateX].s    = SCORE_MAX;
    }

    for (stateX=0;stateX<hmm1P->stateN;stateX++) {
      int       transX = 0;
      int       transN = (int) hmm1P->transNA[stateX];      
      PSTATE*  transXA = hmm1P->transXA[stateX];
      PHMM*      hmm2P = hmmOffsetP + path[stateX+1].hmmX;

      if (oldscoreA[stateX+1] >= SCORE_MAX) continue;

      for (transX=0;transX<transN;transX++) {
	int     stateY = transXA[transX];
	float newscore = SCORE_MAX;

	newscore = oldscoreA[stateX+1] + ascoreA[hmm2P->snXA[stateX]] +           
	  hmm2P->prob.AA[stateX][transX];

	/* exit state */
	if (stateY >= hmm1P->stateN) stateY = -1;

	if (newscore < path[stateY+1].s) {
	  path[stateY+1].s     = newscore;
	  path[stateY+1].bpIdx = oldBpIdxA[stateX+1];
	  path[stateY+1].hmmX  = path[stateX+1].hmmX;
	}
	if (newscore < best) 
	  best= newscore;
      }
    }

    /* update instance best_score */
    rootP->iceA[morphX]->score = best;

    /* update root best_score , worst_score */
    if (best < bestscore) {
      bestscore    = best;
      rootP->bestX = morphX;
    }
    if (best > worstscore) {
      worstscore    = best;
      rootP->worstX = morphX;
    }

  }
  return bestscore;
}

/* -------------------------------------------*/

float vitGeneralNode(PHMM* hmmP, float* ascoreA, SNode* nodeP)
{
  int       morphX = 0;
  int       morphN = nodeP->morphN;
  float bestscore  =  SCORE_MAX;
  float worstscore = -SCORE_MAX;

  nodeP->bestX  = 0;
  nodeP->worstX = 0;

  for (morphX=0;morphX<morphN;morphX++) {
    NTok*  path = nodeP->iceA.n[morphX]->path;
    float  best = SCORE_MAX;
    int  stateX = 0;

    /* save old tokens (needed for back-loops) */
    for (stateX=0;stateX<=hmmP->stateN;stateX++) {
      oldscoreA[stateX] = path[stateX].s;
      oldBpIdxA[stateX] = path[stateX].bpIdx;
      path[stateX].s    = SCORE_MAX;
    }

    for (stateX=0;stateX<hmmP->stateN;stateX++) {
      int      transX = 0;
      int      transN = (int) hmmP->transNA[stateX];      
      PSTATE* transXA = hmmP->transXA[stateX];

      if (oldscoreA[stateX+1] >= SCORE_MAX) continue;

      for (transX=0;transX<transN;transX++) {
	int     stateY = transXA[transX];
	float newscore =  
	  oldscoreA[stateX+1]             +           
	  ascoreA[hmmP->snXA[stateX]]     +           
	  hmmP->prob.AA[stateX][transX];

	/* exit state */
	if (stateY >= hmmP->stateN) stateY = -1;

	if (newscore < path[stateY+1].s) {
	  path[stateY+1].s     = newscore;
	  path[stateY+1].bpIdx = oldBpIdxA[stateX+1];
	}
	if (newscore < best) 
	  best= newscore;
      }
    }

    /* update instance best_score */
    nodeP->iceA.n[morphX]->score = best;

    /* update root best_score , worst_score */
    if (best < bestscore) {
      bestscore    = best;
      nodeP->bestX = morphX;
    }
    if (best > worstscore) {
      worstscore    = best;
      nodeP->worstX = morphX;
    }
  }
  return bestscore;
}

/* -------------------------------------------*/

float vitGeneralLeaf(RCM* rcmP, float* ascoreA, SNode* leafP, PHMM* hmmOffsetP)
{
  int       morphX = 0;
  int       morphN = leafP->morphN;
  int         hmmX = 0;
  int         hmmN = rcmP->hmmN;
  float  bestscore =  SCORE_MAX;
  float worstscore = -SCORE_MAX;

  leafP->bestX  = 0;
  leafP->worstX = 0;

  for (morphX=0;morphX<morphN;morphX++) 
    leafP->iceA.l[morphX]->score = SCORE_MAX;

  /* for each right context */
  for (hmmX=0;hmmX<hmmN;hmmX++) {
    PHMM*    hmmP = hmmOffsetP + rcmP->hmmXA[hmmX];
  
    for (morphX=0;morphX<morphN;morphX++) {
      NTok*  path = leafP->iceA.l[morphX]->path + (hmmP->stateN+1) * hmmX;
      float  best = SCORE_MAX;
      int  stateX = 0;

      /* check if instance of this rcm is pruned away */
      if (path[1].bpIdx == RCM_BPX_NIL) { continue; }

      /* save old tokens (needed for back-loops) */
      for (stateX=0;stateX<=hmmP->stateN;stateX++) {
	oldscoreA[stateX] = path[stateX].s;
	oldBpIdxA[stateX] = path[stateX].bpIdx;
	path[stateX].s  = SCORE_MAX;
      }
      
      /* update scores */
      for (stateX=0;stateX<hmmP->stateN;stateX++) {
	int      transX = 0;
	int      transN = (int) hmmP->transNA[stateX];      
	PSTATE* transXA = hmmP->transXA[stateX];
	
	if (oldscoreA[stateX+1] >= SCORE_MAX) continue;

	for (transX=0;transX<transN;transX++) {
	  int     stateY = transXA[transX];
	  float newscore =  
	    oldscoreA[stateX+1]             +           
	    ascoreA[hmmP->snXA[stateX]]     +           
	    hmmP->prob.AA[stateX][transX];

	  /* exit state */
	  if (stateY >= hmmP->stateN) stateY = -1;

	  if (newscore < path[stateY+1].s) {
	    path[stateY+1].s     = newscore;
	    path[stateY+1].bpIdx = oldBpIdxA[stateX+1];	 
	  }
	  if (newscore < best) 
	    best= newscore;
	}
      }


      /* update instance best_score , take the best one
	 of all possible right_contexts
      */
      if (best < leafP->iceA.l[morphX]->score)
	leafP->iceA.l[morphX]->score = best;
      
    }
  }
  
  /* update global best/worst score */
  for (morphX=0;morphX<morphN;morphX++) {
#if USE_RCM_LM
    leafP->iceA.l[morphX]->score +=  leafP->iceA.l[morphX]->lmp[0];
#endif
    if (bestscore > leafP->iceA.l[morphX]->score) {
      bestscore = leafP->iceA.l[morphX]->score;
      leafP->bestX = morphX;
    }
    if (worstscore < leafP->iceA.l[morphX]->score) {
      worstscore = leafP->iceA.l[morphX]->score;
      leafP->worstX = morphX;
    }

  }

  return bestscore;
}

/* -------------------------------------------*/

float vitGeneralSip(PHMM* hmmP, float* ascoreA, SIPhone* sipP)
{
  int       morphX = 0;
  int       morphN = sipP->morphN;
  float  bestscore = SCORE_MAX;
  float worstscore = -SCORE_MAX;

  sipP->bestX  = 0;
  sipP->worstX = 0;

  for (morphX=0;morphX<morphN;morphX++) {
    NTok*  path = sipP->iceA[morphX]->path;
    float  best = SCORE_MAX;
    int  stateX = 0;

    /* save old tokens (needed for back-loops) */
    for (stateX=0;stateX<=hmmP->stateN;stateX++) {
      oldscoreA[stateX] = path[stateX].s;
      oldBpIdxA[stateX] = path[stateX].bpIdx;
      path[stateX].s  = SCORE_MAX;
    }

    for (stateX=0;stateX<hmmP->stateN;stateX++) {
      int      transX = 0;
      int      transN = (int) hmmP->transNA[stateX];      
      PSTATE* transXA = hmmP->transXA[stateX];

      for (transX=0;transX<transN;transX++) {
	int     stateY = transXA[transX];
	float newscore =  
	  oldscoreA[stateX+1]             +           
	  ascoreA[hmmP->snXA[stateX]]     +           
	  hmmP->prob.AA[stateX][transX];

	/* exit state */
	if (stateY >= hmmP->stateN) stateY = -1;

	if (newscore < path[stateY+1].s) {
	  path[stateY+1].s     = newscore;
	  path[stateY+1].bpIdx = oldBpIdxA[stateX+1];
	}
	if (newscore < best) 
	  best= newscore;
      }
    }

    /* update instance best_score */
    sipP->iceA[morphX]->score = best;

    /* update root best_score , worst_score */
    if (best < bestscore) {
      bestscore   = best;
      sipP->bestX = morphX;
    }
    if (best > worstscore) {
      worstscore = best;
      sipP->worstX = morphX;
    }
  }
  return bestscore;
}

/* -------------------------------------------*/

float vitGeneralSdp(XCM*  xcmP, float* ascoreA, SDPhone* sdpP,
		    PHMM* hmmOffsetP)
{
  int       morphX = 0;
  int       morphN = sdpP->morphN;
  int         hmmX = 0;
  float  bestscore = SCORE_MAX;
  float worstscore = -SCORE_MAX;

  sdpP->bestX  = 0;
  sdpP->worstX = 0;

  for (morphX=0;morphX<morphN;morphX++) 
    sdpP->iceA[morphX]->score = SCORE_MAX;
  
  /* for each instance */
  for (morphX=0;morphX<morphN;morphX++) {
    int   phoneL = sdpP->iceA[morphX]->phoneX;
    int     hmmN = xcmP->hmmNA[phoneL];
    PHMMX* hmmXA = xcmP->hmmXA[phoneL];

    /* for each right context */
    for (hmmX=0;hmmX<hmmN;hmmX++) {
      PHMM*   hmmP = hmmOffsetP + hmmXA[hmmX];
      NTok*   path = sdpP->iceA[morphX]->path + (hmmP->stateN+1) * hmmX;
      float   best = SCORE_MAX;
      int   stateX = 0;

      /* check if instance of this rcm is pruned away */
      if (path[1].bpIdx == RCM_BPX_NIL) { continue; }

      /* save old tokens (needed for back-loops) */
      for (stateX=0;stateX<=hmmP->stateN;stateX++) {
	oldscoreA[stateX] = path[stateX].s;
	oldBpIdxA[stateX] = path[stateX].bpIdx;
	path[stateX].s    = SCORE_MAX;
      }
      
      /* update scores */
      for (stateX=0;stateX<hmmP->stateN;stateX++) {
	int      transX = 0;
	int      transN = (int) hmmP->transNA[stateX];      
	PSTATE* transXA = hmmP->transXA[stateX];
	
	for (transX=0;transX<transN;transX++) {
	  int     stateY = transXA[transX];
	  float newscore =  
	    oldscoreA[stateX+1]             +           
	    ascoreA[hmmP->snXA[stateX]]     +           
	    hmmP->prob.AA[stateX][transX];

	  /* exit state */
	  if (stateY >= hmmP->stateN) stateY = -1;

	  if (newscore < path[stateY+1].s) {
	    path[stateY+1].s     = newscore;
	    path[stateY+1].bpIdx = oldBpIdxA[stateX+1];	 
	  }
	  if (newscore < best) 
	    best= newscore;
	}
      }

      /* update instance best_score , take the best one
	 of all possible right_contexts
      */
      if (best < sdpP->iceA[morphX]->score)
	sdpP->iceA[morphX]->score = best;
      
    } /* for each right context */
  } /* for each left context */
  
  /* update global best/worst score */
  for (morphX=0;morphX<morphN;morphX++) {
    if (bestscore > sdpP->iceA[morphX]->score) {
      bestscore   = sdpP->iceA[morphX]->score;
      sdpP->bestX = morphX;
    }
    if (worstscore < sdpP->iceA[morphX]->score) {
      worstscore   = sdpP->iceA[morphX]->score;
      sdpP->worstX = morphX;
    }
  }
  
  return bestscore;
}

/* ------------------------------------------------------------------------
    hmm viterbi implementation : 3state from left to right, w/o skips
   ------------------------------------------------------------------------ */

/* state numeration:
   0 : exit   state
   1 : begin  state 
   2 : middle state
   3 : end    state
*/

float vit3stateRoot(float* ascoreA, SRoot* rootP, PHMM* hmmOffsetP)
{ 
  int    morphX = 0;
  int    morphN = rootP->morphN;
  register float
    best       = SCORE_MAX,
    bestscore  = SCORE_MAX,
    worstscore = -SCORE_MAX,
    t0,t1;

  rootP->bestX  = 0;
  rootP->worstX = 0;
  
  for (morphX=0;morphX<morphN;morphX++) {
    register RTok* path = rootP->iceA[morphX]->path;
    PHMM* hmm1P = hmmOffsetP + path[1].hmmX;
    PHMM* hmm2P = hmmOffsetP + path[2].hmmX;
    PHMM* hmm3P = hmmOffsetP + path[3].hmmX;

    register float  
      delta3to4 = ascoreA[hmm3P->snXA[2]] + hmm3P->prob.A[5],  /* end -> exit */
      delta3to3 = ascoreA[hmm3P->snXA[2]] + hmm3P->prob.A[4],  /* end -> end  */
      delta2to3 = ascoreA[hmm2P->snXA[1]] + hmm2P->prob.A[3],  /* mid -> end  */
      delta2to2 = ascoreA[hmm2P->snXA[1]] + hmm2P->prob.A[2],  /* mid -> mid  */
      delta1to2 = ascoreA[hmm1P->snXA[0]] + hmm1P->prob.A[1],  /* beg -> mid  */
      delta1to1 = ascoreA[hmm1P->snXA[0]] + hmm1P->prob.A[0];  /* beg -> beg  */

    /* leave hmm, go to exit state */
    path[0] = path[3];
    best = path[0].s  = path[3].s + delta3to4;

    /* update end-state */
    t0 = path[3].s + delta3to3; 
    t1 = path[2].s + delta2to3; 

    if ( t0 < t1) {
      path[3].s = t0;
      if (t0 < best) best = t0;
    } else {
      path[3]   = path[2];
      path[3].s = t1;
      if (t1 < best) best = t1;
    }

    /* update middle-state */
    t0 = path[2].s + delta2to2; 
    t1 = path[1].s + delta1to2; 

    if ( t0 < t1) {
      path[2].s = t0;
      if (t0 < best) best = t0;
    } else {
      path[2]   = path[1];
      path[2].s = t1;
      if (t1 < best) best = t1;
    }

    /* update begin-state */
    path[1].s += delta1to1;
    if (path[1].s < best) best = path[1].s;

    /* update instance best_score */
    rootP->iceA[morphX]->score = best;

    /* update node best_score , worst_score */
    if (best < bestscore) {
      bestscore    = best;
      rootP->bestX = morphX;
    }
    if (best > worstscore) {
      worstscore    = best;
      rootP->worstX = morphX;
    }
  }
  return bestscore;
}

/* -------------------------------------------*/

float vit3stateNode(PHMM* hmmP, float* ascoreA, SNode* nodeP)
{ 
  int    morphX = 0;
  int    morphN = nodeP->morphN;
  PHMMX*   snXA = hmmP->snXA;
  float*  probA = hmmP->prob.A;
  register float  
    delta3to4  = ascoreA[snXA[2]] + probA[5],  /* end -> exit */
    delta3to3  = ascoreA[snXA[2]] + probA[4],  /* end -> end  */
    delta2to3  = ascoreA[snXA[1]] + probA[3],  /* mid -> end  */
    delta2to2  = ascoreA[snXA[1]] + probA[2],  /* mid -> mid  */
    delta1to2  = ascoreA[snXA[0]] + probA[1],  /* beg -> mid  */
    delta1to1  = ascoreA[snXA[0]] + probA[0],  /* beg -> beg  */
    best       = SCORE_MAX,
    bestscore  = SCORE_MAX,
    worstscore = -SCORE_MAX,
    t0,t1;

  nodeP->bestX  = 0;
  nodeP->worstX = 0;

  for (morphX=0;morphX<morphN;morphX++) {
    NTok* path = nodeP->iceA.n[morphX]->path;

    /* leave hmm, go to exit state */
    best = path[0].s  = path[3].s + delta3to4;
    path[0].bpIdx = path[3].bpIdx;

    /* update end-state */
    t0 = path[3].s + delta3to3; 
    t1 = path[2].s + delta2to3; 

    if ( t0 < t1) {
      path[3].s = t0;
      if (t0 < best) best = t0;
    } else {
      path[3].s  = t1;
      path[3].bpIdx = path[2].bpIdx;
      if (t1 < best) best = t1;
    }

    /* update middle-state */
    t0 = path[2].s + delta2to2; 
    t1 = path[1].s + delta1to2; 

    if ( t0 < t1) {
      path[2].s = t0;
      if (t0 < best) best = t0;
    } else {
      path[2].s  = t1;
      path[2].bpIdx = path[1].bpIdx;
      if (t1 < best) best = t1;
    }

    /* update begin-state */
    path[1].s += delta1to1;
    if (path[1].s < best) best = path[1].s;

    /* update instance best_score */
    nodeP->iceA.n[morphX]->score = best;

    /* update node best_score */
    if (best < bestscore) {
      bestscore    = best;
      nodeP->bestX = morphX;
    }
    if (best > worstscore) {
      worstscore    = best;
      nodeP->worstX = morphX;
    }
  }
  return bestscore;
}

/* -------------------------------------------*/

float vit3stateLeaf(RCM* rcmP, float* ascoreA, SNode* leafP,PHMM* hmmOffsetP)
{ 
  int    morphX = 0;
  int    morphN = leafP->morphN;
  int      hmmX = 0;
  int      hmmN = rcmP->hmmN;
  register float 
    best       = SCORE_MAX,
    bestscore  = SCORE_MAX,
    worstscore = -SCORE_MAX;

  leafP->bestX  = 0;
  leafP->worstX = 0;

  for (morphX=0;morphX<morphN;morphX++) 
    leafP->iceA.l[morphX]->score = SCORE_MAX;

  /* for each right context */
  for (hmmX=0;hmmX<hmmN;hmmX++) {
    PHMMX*   snXA = (hmmOffsetP + rcmP->hmmXA[hmmX])->snXA;
    float*  probA = (hmmOffsetP + rcmP->hmmXA[hmmX])->prob.A;
    register float  
      delta3to4 = ascoreA[snXA[2]] + probA[5],  /* end -> exit */
      delta3to3 = ascoreA[snXA[2]] + probA[4],  /* end -> end  */
      delta2to3 = ascoreA[snXA[1]] + probA[3],  /* mid -> end  */
      delta2to2 = ascoreA[snXA[1]] + probA[2],  /* mid -> mid  */
      delta1to2 = ascoreA[snXA[0]] + probA[1],  /* beg -> mid  */
      delta1to1 = ascoreA[snXA[0]] + probA[0],  /* beg -> beg  */
      t0,t1;
   
    for (morphX=0;morphX<morphN;morphX++) {
      /* You question the worthiness of my code?
	 I have here 4 (beg, mid, end, exit) hmm acoustic states 
	 and the leaf instance has to store a path for each possible 
	 right context (hmmN) in a flaten array.
      */
      register NTok* path = 
	leafP->iceA.l[morphX]->path + 4 * hmmX;

      /* check if this right context active (see spassRequest_SLeaf) */
      if (path[1].bpIdx == RCM_BPX_NIL) { continue; }

      /* leave hmm, go to exit state */
      best = path[0].s  = path[3].s + delta3to4;
      path[0].bpIdx = path[3].bpIdx;

      /* update end-state */
      t0 = path[3].s + delta3to3; 
      t1 = path[2].s + delta2to3; 

      if ( t0 < t1) {
	path[3].s = t0;
	if (t0 < best) best = t0;
      } else {
	path[3].s  = t1;
	path[3].bpIdx = path[2].bpIdx;
	if (t1 < best) best = t1;
      }

      /* update middle-state */
      t0 = path[2].s + delta2to2; 
      t1 = path[1].s + delta1to2; 

      if ( t0 < t1) {
	path[2].s = t0;
	if (t0 < best) best = t0;
      } else {
	path[2].s  = t1;
	path[2].bpIdx = path[1].bpIdx;
	if (t1 < best) best = t1;
      }

      /* update begin-state */
      path[1].s += delta1to1;
      if (path[1].s < best) best = path[1].s;

      /* update instance best_score , take the best one
	 of all possible right_contexts
      */
      if (best < leafP->iceA.l[morphX]->score)
	leafP->iceA.l[morphX]->score = best;
      
    }
  }

  /* update global best/worst score */
  for (morphX=0;morphX<morphN;morphX++) {
#if USE_RCM_LM
    leafP->iceA.l[morphX]->score +=  leafP->iceA.l[morphX]->lmp[0];
#endif
    if (bestscore > leafP->iceA.l[morphX]->score) {
      bestscore    = leafP->iceA.l[morphX]->score;
      leafP->bestX = morphX;
    }
    if (worstscore < leafP->iceA.l[morphX]->score) {
     worstscore    = leafP->iceA.l[morphX]->score;
     leafP->worstX = morphX;
    }
  }
  return bestscore;
}

/* -------------------------------------------*/

float vit3stateSip(PHMM* hmmP, float* ascoreA, SIPhone* sipP)
{ 
  int    morphX = 0;
  int    morphN = sipP->morphN;
  PHMMX*   snXA = hmmP->snXA;
  float*  probA = hmmP->prob.A;
  register float  
    delta3to4  = ascoreA[snXA[2]] + probA[5],  /* end -> exit */
    delta3to3  = ascoreA[snXA[2]] + probA[4],  /* end -> end  */
    delta2to3  = ascoreA[snXA[1]] + probA[3],  /* mid -> end  */
    delta2to2  = ascoreA[snXA[1]] + probA[2],  /* mid -> mid  */
    delta1to2  = ascoreA[snXA[0]] + probA[1],  /* beg -> mid  */
    delta1to1  = ascoreA[snXA[0]] + probA[0],  /* beg -> beg  */
    best       = SCORE_MAX,
    bestscore  = SCORE_MAX,
    worstscore = -SCORE_MAX,
    t0,t1;

  sipP->bestX  = 0;
  sipP->worstX = 0;

  for (morphX=0;morphX<morphN;morphX++) {
    NTok* path = sipP->iceA[morphX]->path;

    /* leave hmm, go to exit state */
    best = path[0].s  = path[3].s + delta3to4;
    path[0].bpIdx = path[3].bpIdx;

    /* update end-state */
    t0 = path[3].s + delta3to3; 
    t1 = path[2].s + delta2to3; 

    if ( t0 < t1) {
      path[3].s = t0;
      if (t0 < best) best = t0;
    } else {
      path[3].s  = t1;
      path[3].bpIdx = path[2].bpIdx;
      if (t1 < best) best = t1;
    }

    /* update middle-state */
    t0 = path[2].s + delta2to2; 
    t1 = path[1].s + delta1to2; 

    if ( t0 < t1) {
      path[2].s = t0;
      if (t0 < best) best = t0;
    } else {
      path[2].s  = t1;
      path[2].bpIdx = path[1].bpIdx;
      if (t1 < best) best = t1;
    }

    /* update begin-state */
    path[1].s += delta1to1;
    if (path[1].s < best) best = path[1].s;

    /* update instance best_score */
    sipP->iceA[morphX]->score = best;

    /* update node best_score */
    if (best < bestscore) {
      bestscore   = best;
      sipP->bestX = morphX;
    }
    if (best > worstscore) {
      worstscore   = best;
      sipP->worstX = morphX;
    }
  }

  return bestscore;
}

/* -------------------------------------------*/

float vit3stateSdp(XCM* xcmP, float* ascoreA, SDPhone* sdpP,PHMM* hmmOffsetP)
{ 
  int       morphX = 0;
  int       morphN = sdpP->morphN;
  int         hmmX = 0;
  float       best = SCORE_MAX;
  float  bestscore =  SCORE_MAX;
  float worstscore = -SCORE_MAX;

  sdpP->bestX  = 0;
  sdpP->worstX = 0;

  for (morphX=0;morphX<morphN;morphX++) 
    sdpP->iceA[morphX]->score = SCORE_MAX;

  /* for each instance */
  for (morphX=0;morphX<morphN;morphX++) {
    int   phoneL = sdpP->iceA[morphX]->phoneX;
    int     hmmN = xcmP->hmmNA[phoneL];
    PHMMX* hmmXA = xcmP->hmmXA[phoneL];

    /* for each right context */
    for (hmmX=0;hmmX<hmmN;hmmX++) {
      PHMMX*          snXA = (hmmOffsetP + hmmXA[hmmX])->snXA;
      float*         probA = (hmmOffsetP + hmmXA[hmmX])->prob.A;
      NTok*  path = sdpP->iceA[morphX]->path + 4 * hmmX;
      float  
	delta3to4 = ascoreA[snXA[2]] + probA[5],  /* end -> exit */
	delta3to3 = ascoreA[snXA[2]] + probA[4],  /* end -> end  */
	delta2to3 = ascoreA[snXA[1]] + probA[3],  /* mid -> end  */
	delta2to2 = ascoreA[snXA[1]] + probA[2],  /* mid -> mid  */
	delta1to2 = ascoreA[snXA[0]] + probA[1],  /* beg -> mid  */
	delta1to1 = ascoreA[snXA[0]] + probA[0],  /* beg -> beg  */
	t0,t1;

      /* check if instance of this rcm is pruned away */
      if (path[1].bpIdx == RCM_BPX_NIL) { continue; }

      /* leave hmm, go to exit state */
      best = path[0].s  = path[3].s + delta3to4;
      path[0].bpIdx = path[3].bpIdx;
      
      /* update end-state */
      t0 = path[3].s + delta3to3; 
      t1 = path[2].s + delta2to3; 
      
      if ( t0 < t1) {
	path[3].s = t0;
	if (t0 < best) best = t0;
      } else {
	path[3].s  = t1;
	path[3].bpIdx = path[2].bpIdx;
	if (t1 < best) best = t1;
      }
      
      /* update middle-state */
      t0 = path[2].s + delta2to2; 
      t1 = path[1].s + delta1to2; 
      
      if ( t0 < t1) {
	path[2].s = t0;
	if (t0 < best) best = t0;
      } else {
	path[2].s  = t1;
	path[2].bpIdx = path[1].bpIdx;
	if (t1 < best) best = t1;
      }

      /* update begin-state */
      path[1].s += delta1to1;
      if (path[1].s < best) best = path[1].s;
      
      /* update instance best_score , take the best one
	 of all possible right_contexts
      */
      if (best < sdpP->iceA[morphX]->score)
	sdpP->iceA[morphX]->score = best;
      
    } /* for each right context */
  } /* for each instance */

  /* update global best/worst score */
  for (morphX=0;morphX<morphN;morphX++) {
    if (bestscore > sdpP->iceA[morphX]->score) {
      bestscore   = sdpP->iceA[morphX]->score;
      sdpP->bestX = morphX;
    }
    if (worstscore < sdpP->iceA[morphX]->score) {
      worstscore   = sdpP->iceA[morphX]->score;
      sdpP->worstX = morphX;
    }
  }

  return bestscore;
}

/* ------------------------------------------------------------------------
    array of viterbi functions
   ------------------------------------------------------------------------ */

/* Note: If you change any of these function arrays, you should also
   change phmmSetAdd, phmmSetLoad, and phmmSetSave accordingly.
*/

enum {
  vitGeneralIdx = 0,
  vit3stateIdx  = 1
};
static HmmEvalRootFn* HmmEvalRootA[] = { 
  vitGeneralRoot,    
  vit3stateRoot    
};
static HmmEvalNodeFn* HmmEvalNodeA[] = { 
  vitGeneralNode,    
  vit3stateNode    
};
static HmmEvalLeafFn* HmmEvalLeafA[] = { 
  vitGeneralLeaf,    
  vit3stateLeaf    
};
static HmmEvalSipFn* HmmEvalSipA[] = { 
  vitGeneralSip,    
  vit3stateSip    
};
static HmmEvalSdpFn* HmmEvalSdpA[] = { 
  vitGeneralSdp,    
  vit3stateSdp
};

/* ========================================================================
    Hidden Markov Model Context
   ======================================================================== */

static int phmmcInit(PHMMC* hmmcP, PHMMC* hashkey)
{
  (*hmmcP) = (*hashkey);
  return TCL_OK;
}

static int phmmcDeinit(PHMMC* hmmcP)
{
  return TCL_OK;
}

static int phmmcHashCmp(PHMMC* hmmcP, PHMMC* hashkey)
{
  int phoneX;
  if (hmmcP->phoneA[0] != hashkey->phoneA[0]) return 0;
  if (hmmcP->phoneN    != hashkey->phoneN)    return 0;
  for (phoneX =1;phoneX<=2*hmmcP->phoneN+1;phoneX++)
    if (hmmcP->phoneA[phoneX] != hashkey->phoneA[phoneX]) return 0;
  
  return 1;
}

#define hmmcBit 6
static long phmmcHashKey( const PHMMC *key)
{
  unsigned long ret = 0;  
  int           phoneX;
  int           phoneN  = key->phoneN;
  int           longBit = 8;

  longBit *= sizeof(long);

  /* phones + id*/
  for (phoneX=0; phoneX<=phoneN; phoneX++) 
    ret = ((ret<<hmmcBit) | (ret >> (longBit-hmmcBit))) \
      ^ key->phoneA[phoneX];

  /* tags + dynFlag (needs only 2 bits) */
  for (phoneX=phoneN+1; phoneX<=2*phoneN+1; phoneX++) 
    ret = ((ret<<2) | (ret >> (longBit-2))) \
      ^ key->phoneA[phoneX];
  
  return ret;
}


/* ========================================================================
    Hidden Markov Model 
   ======================================================================== */

/* ------------------------------------------------------------------------
    hmmInit
   ------------------------------------------------------------------------ */

static int phmmInit (PHMM *hmmP, ClientData CD)
{
  /* the second argument CD is not needed here, but necessary to match with 
     listItemInit. we have nothing to do here, because models will be created 
     on demand in hmmSetAdd(Model)
  */
  hmmP->stateN   = (PSTATE) 0;
  hmmP->snXA     = NULL;
  hmmP->transNA  = NULL;
  hmmP->transXA  = NULL;
  hmmP->prob.A   = NULL;
  hmmP->vitFn.e0 = NULL;

  return TCL_OK;
}

/* ------------------------------------------------------------------------
    phmmDeinit
   ------------------------------------------------------------------------ */

static int phmmDeinit (PHMM *hmmP)
{
  hmmP->stateN   = (PSTATE) 0;
  hmmP->snXA     = NULL;
  hmmP->transNA  = NULL;
  hmmP->transXA  = NULL;
  hmmP->prob.A   = NULL;
  hmmP->vitFn.e0 = NULL;
  return TCL_OK;
}

/* ------------------------------------------------------------------------
    phmm hash function (needed for listIndex)
   ------------------------------------------------------------------------ */

/* We assume that a hmm is uniqe given by the sequence of senones
   and the eval functionPtr but actually we have to consider the 
   topology as well
*/

typedef struct {
  PSTATE       stateN;  
  PHMMX*       snXA;  
  void*        vitFn;
} phmmKey;


static int phmmHashCmp( const phmmKey *key, const PHMM *hmmP)
{
  int stateX;

  if (hmmP->snXA[0] != key->snXA[0]) return 0;
  if (hmmP->stateN  != key->stateN ) return 0;

  for (stateX=1;stateX<hmmP->stateN;stateX++)
    if (hmmP->snXA[stateX] != key->snXA[stateX]) return 0;

  /* it's a union, check only e0 */
  if ((void*)hmmP->vitFn.e0 != key->vitFn) return 0;

  return 1;
}

/* For 3state HMM's we can use 64 / 3 = 21 bits per state.
   But 12 bits (4096 models) should be enough to access
   senone indices. In other cases hash conflicts can occur.
 */
#define hmmBit 12

static long phmmHashKey( const phmmKey *key)
{
  unsigned long ret = 0; 
  int           stateX;
  int           longBit = 8;

  longBit *= sizeof(long);

  for (stateX=0; stateX<key->stateN; stateX++) 
    ret = ((ret<<hmmBit) | (ret >> (longBit-hmmBit))) \
      ^ key->snXA[stateX];
  return ret;
}


/* ------------------------------------------------------------------------
    phmmPuts
   ------------------------------------------------------------------------ */

static int phmmPutsItf (ClientData clientData, int argc, char *argv[])
{
  PHMM* hmmP = (PHMM*)clientData;
  int  stateX;

  itfAppendResult("%d {",hmmP->stateN);
  for (stateX=0; stateX< hmmP->stateN; stateX++)
    itfAppendElement("%d",hmmP->snXA[stateX]);
  itfAppendResult(" }");

  return TCL_OK;    
}

/* ========================================================================
    Set of Hidden Markov Models 
   ======================================================================== */

/* ------------------------------------------------------------------------
    phmmSetInit
   ------------------------------------------------------------------------ */

static int phmmSetInit (PHMMSet *hmmsP, char* name, Tree* treeP, int treeX, \
			int useCtx)
{
  int topoX;

  hmmsP->name  = strdup(name);
  hmmsP->useN  = 0;
  hmmsP->treeP = treeP;
  hmmsP->treeX = treeX;
  hmmsP->tpsP  = (TopoSet*) (treeP->mdsP.cd);
  hmmsP->snsP  = hmmsP->tpsP->snsP;
  hmmsP->tmsP  = hmmsP->tpsP->tmSetP;  
  link(hmmsP->treeP,"Tree");

  listInit((List*)&(hmmsP->list),(TypeInfo*) &phmmInfo, sizeof(PHMM), 
                    phmmSetDefault.list.blkSize);

  hmmsP->list.init    = (ListItemInit  *)phmmInit;
  hmmsP->list.deinit  = (ListItemDeinit*)phmmDeinit;
  hmmsP->list.hashKey = (HashKeyFn     *)phmmHashKey;
  hmmsP->list.hashCmp = (HashCmpFn     *)phmmHashCmp;

  /* memory management */
  hmmsP->mem.c = bmemCreate(2000,0);
  hmmsP->mem.s = bmemCreate(2000,0);
  hmmsP->mem.f = bmemCreate(2000,0);

  /* topo map */
  if (NULL == (hmmsP->topoMap = 
	       (int*) malloc(hmmsP->tpsP->list.itemN*sizeof(int)))) {
    ERROR("phmmSetInit: allocation failure\n");
    return TCL_ERROR;
  } 
  for (topoX=0;topoX<hmmsP->tpsP->list.itemN;topoX++) {
    hmmsP->topoMap[topoX] = -1;
  }

  /* init set of phmm context items */
  hmmsP->ctx.memP = NULL;
  if ((hmmsP->ctx.use = useCtx) >0) {
    StreamArray* staP  = &(hmmsP->snsP->strA); 
    int streamX,qsX;
    int maxLeftContext  =  99999;
    int maxRightContext = -99999;

    listInit((List*)&(hmmsP->ctx.list),(TypeInfo*) NULL, sizeof(PHMMC),50000); 
    hmmsP->ctx.list.init    = (ListItemInit   *)phmmcInit;
    hmmsP->ctx.list.deinit  = (ListItemDeinit *)phmmcDeinit;
    hmmsP->ctx.list.hashKey = (HashKeyFn      *)phmmcHashKey;
    hmmsP->ctx.list.hashCmp = (HashCmpFn      *)phmmcHashCmp;

    hmmsP->ctx.memP = bmemCreate(1048576,0);
  
    /* find maximum context size */
    for (streamX=0;streamX<staP->streamN;streamX++) {
      TypeInfo*    tiP   = ((Stream*) staP->strPA[streamX])->tiP;
      Tree*        treeP = NULL;
      QuestionSet* qssP  = NULL;

      if ( streq(tiP->name,"DistribStream") )
	treeP= ((DistribStream*) staP->cdA[streamX])->treeP;
#ifndef IBIS
      if ( streq(tiP->name,"HmeStream") )
	treeP= ((HmeStream*) staP->cdA[streamX])->tree;
      if ( streq(tiP->name,"MicvStream") )
	treeP= ((MicvStream*) staP->cdA[streamX])->pTree;
#endif

      /*cannot handle this stream type */
      if (treeP == NULL) {
	ERROR("phmmSetInit: cannot handle this stream type %s\n",tiP->name);
	return TCL_ERROR;
      }
      qssP= & (treeP->questions );
      for (qsX=0;qsX<qssP->list.itemN;qsX++) {
	Question* qsP = qssP->list.itemA + qsX;
	if (qsP->leftContext < maxLeftContext)
	  maxLeftContext = qsP->leftContext;
	if (qsP->rightContext > maxRightContext)
	  maxRightContext = qsP->rightContext;
      }
    }
    if (maxLeftContext  == 99999) {
      ERROR("phmmSetInit: wrong left context size\n");
      return TCL_ERROR;
    }
    if (maxRightContext  == -99999) {
      ERROR("phmmSetInit: wrong right context size\n");
      return TCL_ERROR;
    }
    hmmsP->ctx.leftMax  =  -maxLeftContext;
    hmmsP->ctx.rightMax =  maxRightContext;
  }

  return TCL_OK;
}

/* ------------------------------------------------------------------------
    phmmSetCreate
   ------------------------------------------------------------------------ */

PHMMSet* phmmSetCreate (char* name, Tree* treeP, int treeX, int use)
{
  PHMMSet *hmmsP = (PHMMSet*) malloc(sizeof(PHMMSet));

  if (! hmmsP) {
    ERROR("phmmSetCreate: allocation failure\n");
    return NULL;
  }
  if (TCL_ERROR == (phmmSetInit(hmmsP,name,treeP,treeX, use))) {    
    free(hmmsP);
    return NULL;
  }
  return hmmsP;
}


static ClientData phmmSetCreateItf( ClientData clientData, 
                                      int argc, char *argv[])
{
  int          ac    =  argc;
  Tree*        treeP =  NULL;
  char*        treeR =  NULL;
  int          treeX = -1;
  int          use   = phmmSetDefault.ctx.use;
  char       *name   = NULL; 

  if ( itfParseArgv( argv[0], &ac, argv, 0,
      "<name>",        ARGV_STRING, NULL, &name, NULL,    "name of the PHMM set",
      "<TTree>",       ARGV_OBJECT, NULL, &treeP, "Tree", "topology tree",
      "<TTreeRoot>",   ARGV_STRING, NULL, &treeR,  NULL,  "root name in TTree",
      "-useCtx",       ARGV_INT,    NULL, &use,    NULL,  "use HMM context table 0/1",
       NULL) != TCL_OK) return NULL;
  
  if ((treeX = treeIndex( treeP, treeR)) < 0) {
    ERROR("Root node %s not found in %s.\n", treeR, treeP->name);
    return NULL;
  }
  return (ClientData) phmmSetCreate(name,treeP,treeX,use);
}


/* ------------------------------------------------------------------------
    phmmSetDeinit
   ------------------------------------------------------------------------ */

static int phmmSetDeinit (PHMMSet *hmmsP)
{
  if (hmmsP->useN >0) {
    ERROR("phmmSet '%s' still in use by other objects.\n", 
            hmmsP->name); 
    return TCL_ERROR;
  }
  delink(hmmsP->treeP,"Tree");
  if (hmmsP->name)    free(hmmsP->name);
  if (hmmsP->topoMap) free(hmmsP->topoMap);

  hmmsP->name    = NULL;
  hmmsP->treeP   = NULL;
  hmmsP->tpsP    = NULL;
  hmmsP->snsP    = NULL;
  hmmsP->tmsP    = NULL;
  hmmsP->topoMap = NULL;
  
  bmemFree(hmmsP->mem.c);
  bmemFree(hmmsP->mem.s);
  bmemFree(hmmsP->mem.f);

  if (hmmsP->ctx.use) {
    listDeinit((List*)&(hmmsP->ctx.list));
    if (hmmsP->ctx.memP) bmemFree(hmmsP->ctx.memP);
    hmmsP->ctx.memP = NULL;
  }

  return listDeinit((List*)&(hmmsP->list));
}

/* ------------------------------------------------------------------------
    phmmSetFree
   ------------------------------------------------------------------------ */

int phmmSetFree(PHMMSet *hmmsP)
{
  if ( phmmSetDeinit(hmmsP) != TCL_OK) return TCL_ERROR;
  free(hmmsP);
  return TCL_OK;
}

static int phmmSetFreeItf (ClientData clientData)
{
  return phmmSetFree((PHMMSet*)clientData);
}

/* ------------------------------------------------------------------------
    phmmSetAdd
   ------------------------------------------------------------------------ */

static PHMMX  _snXA[PSTATE_MAX];
static CTX    _phXA[PSTATE_MAX];

int phmmSetAdd (PHMMSet* hmmsP, Word* wP, int left, int right, int nodeFnType)
{ 
  Topo*     topoP = NULL;
  phmmKey   key   = {0,NULL};
  PHMMC    ckey   = {0,NULL,0};
  PHMM*     hmmP  = NULL;
  PHMMC*    chmmP = NULL;

  int stateX,transN,topoX,vitX,idx,cidx;

  if ((UINT) hmmsP->list.itemN+1 > PHMM_MAX) {
    ERROR("phmmSetAdd: cannot add model (exceeded PHMM_MAX %d)\n",\
          PHMM_MAX);
    return -1;
  } 

  /* check for HMM context hash table */
  if (hmmsP->ctx.use) {
    int c = -left;
    int l = MIN(hmmsP->ctx.leftMax,  c);
    int r = MIN(hmmsP->ctx.rightMax,right);
    int phoneN = r+l+1;
    int phoneX;

    ckey.phoneA= _phXA;

    for (phoneX=0;phoneX<phoneN;phoneX++) {
      ckey.phoneA[phoneX]          = (CTX) wP->phoneA[c-l+phoneX];
      ckey.phoneA[phoneX+1+phoneN] = (CTX) wP->tagA[c-l+phoneX];
    }
    ckey.phoneA[phoneN]     = c;
    ckey.phoneA[2*phoneN+1] = nodeFnType;
    ckey.phoneN = phoneN;
    ckey.hmmX   = -1;
    cidx  = listIndex((List*) &(hmmsP->ctx.list), &ckey, 1);  
    chmmP = hmmsP->ctx.list.itemA+cidx;
    assert (chmmP); // XCode says could be NULL
    if (chmmP->hmmX > -1) {
      return chmmP->hmmX;
    } else {
      chmmP->phoneA= bmemAlloc(hmmsP->ctx.memP,(2*phoneN+2)*sizeof(CTX));
      memcpy(chmmP->phoneA,_phXA,(2*phoneN+2)*sizeof(CTX));
    }
  }

  if ((topoX = treeGetModel( hmmsP->treeP, hmmsP->treeX, wP, left, right)) < 0) {
    ERROR("phmmSetAdd: cannot find topology model)\n");
    return -1;
  }
  topoP  = hmmsP->tpsP->list.itemA + topoX;

  if (topoP->tmN+1 >= PSTATE_MAX) {
    ERROR("phmmSetAdd: topology constrains exceeded stateN= %d)\n",topoP->tmN);
    return -1;
  }

  /* find approriate viterbi function */
  if (hmmsP->topoMap[topoX] > -1) {
    vitX = hmmsP->topoMap[topoX];
  } else {
    vitX = vitGeneralIdx;
    /* check for plain 3state topo */
    if (topoP->tmN == 3) {
      Tm* tm0= hmmsP->tmsP->list.itemA + topoP->tmXA[0];
      Tm* tm1= hmmsP->tmsP->list.itemA + topoP->tmXA[1];
      Tm* tm2= hmmsP->tmsP->list.itemA + topoP->tmXA[2];

      if (tm0->trN == 2 && 
	  tm1->trN == 2 && 
	  tm2->trN == 2 &&
	  tm0->offA[0] == 0 && tm0->offA[1] == 1 &&
	  tm1->offA[0] == 0 && tm1->offA[1] == 1 &&
	  tm2->offA[0] == 0 && tm2->offA[1] == 1) 
	vitX= vit3stateIdx;
    }
    hmmsP->topoMap[topoX] = vitX;
  }

  /* allocate and init senone sequence */
  key.stateN = topoP->tmN;
  key.snXA   = _snXA;

  for (stateX=0;stateX<key.stateN;stateX++) {
    int snX = topoP->stnXA[stateX];
    key.snXA[stateX] = snsGetModel( hmmsP->snsP, snX, wP, left, right);
  }
  if (hmmsP->snsP->list.itemN > PHMM_MAX) {
    ERROR("phmmSetAdd: SenoneSet overflow (check slimits)\n");
    return TCL_ERROR;
  }

  /* set viterbi function Ptr */
  switch (nodeFnType) {
  case hmmEvalFnRoot : 
    key.vitFn = (void*) HmmEvalRootA[vitX]; break;
  case hmmEvalFnNode : 
    key.vitFn = (void*) HmmEvalNodeA[vitX]; break;
  case hmmEvalFnLeaf : 
    key.vitFn = (void*) HmmEvalLeafA[vitX]; break;
  case hmmEvalFnSip : 
    key.vitFn = (void*) HmmEvalSipA[vitX]; break;
  case hmmEvalFnSdp : 
    key.vitFn = (void*) HmmEvalSdpA[vitX]; break;
  default:
    ERROR("phmmSetAdd: unknown nodeFnType %d\n",nodeFnType);
  }

  /* check, if we have this model already */
  idx = listIndex((List*) &(hmmsP->list), &key, 0);  
  if (idx > -1) {
    if (hmmsP->ctx.use) chmmP->hmmX = idx;
    return idx;
  }

  idx = listNewItem((List*) &(hmmsP->list), &key);
  if (hmmsP->ctx.use) chmmP->hmmX = idx;

  hmmP = (PHMM*) (hmmsP->list.itemA+idx);

  hmmP->snXA     = (PHMMX*) bmemAlloc(hmmsP->mem.s,key.stateN*sizeof(PHMMX));
  hmmP->stateN   = key.stateN;
  hmmP->vitFn.e0 = (HmmEvalRootFn*) key.vitFn;

  memcpy(hmmP->snXA,key.snXA,key.stateN*sizeof(PHMMX));

  if (vitX == vit3stateIdx) {
    Tm* tm0= hmmsP->tmsP->list.itemA + topoP->tmXA[0];
    Tm* tm1= hmmsP->tmsP->list.itemA + topoP->tmXA[1];
    Tm* tm2= hmmsP->tmsP->list.itemA + topoP->tmXA[2];

    /* we don't use this, it's hardcoded in the eval function ! */
    hmmP->transNA = NULL;
    hmmP->transXA = NULL;

    hmmP->prob.A= (float*) bmemAlloc(hmmsP->mem.f,sizeof(float)*6);

    hmmP->prob.A[0] = tm0->penA[0];  /* state 0 -> 0*/
    hmmP->prob.A[1] = tm0->penA[1];  /* state 0 -> 1*/
    hmmP->prob.A[2] = tm1->penA[0];  /* state 1 -> 1*/
    hmmP->prob.A[3] = tm1->penA[1];  /* state 1 -> 2*/
    hmmP->prob.A[4] = tm2->penA[0];  /* state 2 -> 2*/
    hmmP->prob.A[5] = tm2->penA[1];  /* state 2 -> 3*/
  }

  if (vitX == vitGeneralIdx) {
    /* count total number of transitions and alloc mem */
    transN = 0;
    for (stateX=0;stateX<hmmP->stateN;stateX++) {
      Tm* tm= hmmsP->tmsP->list.itemA + topoP->tmXA[stateX];
      transN += tm->trN;
    }

    hmmP->transNA = (PSTATE*)    bmemAlloc(hmmsP->mem.c,hmmP->stateN*sizeof(PSTATE));
    hmmP->transXA = (PSTATE**)   bmemAlloc(hmmsP->mem.f,hmmP->stateN*sizeof(PSTATE*));
    hmmP->prob.AA = (float**)    bmemAlloc(hmmsP->mem.f,hmmP->stateN*sizeof(float*));
    hmmP->prob.AA[0] = (float*)  bmemAlloc(hmmsP->mem.f,transN*sizeof(float));
    hmmP->transXA[0] = (PSTATE*) bmemAlloc(hmmsP->mem.c,transN*sizeof(PSTATE));

    for (stateX=1;stateX<hmmP->stateN;stateX++) {
      Tm* tm   = hmmsP->tmsP->list.itemA + topoP->tmXA[stateX-1];
      hmmP->prob.AA[stateX]=hmmP->prob.AA[stateX-1]+tm->trN;
      hmmP->transXA[stateX]=hmmP->transXA[stateX-1]+tm->trN;
    }

    /* init trans and prob arrays */
    for (stateX=0;stateX<hmmP->stateN;stateX++) {
      Tm* tm   = hmmsP->tmsP->list.itemA + topoP->tmXA[stateX];
      int trX  = 0;
      hmmP->transNA[stateX]=tm->trN;
      for (trX=0;trX<tm->trN;trX++,transN++) {
	hmmP->transXA[stateX][trX] = (PSTATE) (stateX + tm->offA[trX]);
	hmmP->prob.AA[stateX][trX] = tm->penA[trX];
      }
    }
  }

  return idx;
}

/* ------------------------------------------------------------------------
    phmmSetAddGraph
   ------------------------------------------------------------------------ */

static int phmmSetAddGraph (PHMMSet* hmmsP, int stateN, PHMMX* snXA, 
			    PSTATE* transNA, PSTATE** transXA, float** probAA)
{ 
  PHMM*   hmmP = NULL;
  phmmKey  key = {0,NULL};

  int stateX,idx,transN;

  if ((UINT) hmmsP->list.itemN+1 > PHMM_MAX-1) {
    ERROR("phmmSetAddGraph: cannot add model (exceeded PHMM_MAX %d)\n",\
          PHMM_MAX);
    return -1;
  } 
  key.stateN = stateN;
  key.snXA   = snXA;
  key.vitFn  = (void*) HmmEvalSipA[vitGeneralIdx];

  /* check, if we have this model already */
  idx = listIndex((List*) &(hmmsP->list), &key, 0);  
  if (idx > -1) 
    return idx;

  /* create new PHMM */
  idx  = listNewItem((List*) &(hmmsP->list), &key);
  hmmP = (PHMM*) (hmmsP->list.itemA+idx);

  /* count total number of transitions */
  transN = 0;
  for (stateX=0; stateX<stateN;stateX++)
    transN += transNA[stateX];

  /* get memory for new PHMM */
  hmmP->snXA    = (PHMMX*)     bmemAlloc(hmmsP->mem.s,stateN*sizeof(PHMMX));
  hmmP->transNA = (PSTATE*)    bmemAlloc(hmmsP->mem.c,stateN*sizeof(PSTATE));
  hmmP->transXA = (PSTATE**)   bmemAlloc(hmmsP->mem.f,stateN*sizeof(PSTATE*));
  hmmP->prob.AA = (float**)    bmemAlloc(hmmsP->mem.f,stateN*sizeof(float*));
  hmmP->prob.AA[0] = (float*)  bmemAlloc(hmmsP->mem.f,transN*sizeof(float));
  hmmP->transXA[0] = (PSTATE*) bmemAlloc(hmmsP->mem.c,transN*sizeof(PSTATE));

  for (stateX=1;stateX<stateN;stateX++) {
    hmmP->prob.AA[stateX]=hmmP->prob.AA[stateX-1]+transNA[stateX-1];
    hmmP->transXA[stateX]=hmmP->transXA[stateX-1]+transNA[stateX-1];
  }

  /* initialize PHMM */
  hmmP->stateN   = stateN;
  hmmP->vitFn.e3 = (HmmEvalSipFn*) key.vitFn;

  for (stateX=0;stateX<stateN;stateX++) {
    int transX = transNA[stateX]-1;
    hmmP->snXA[stateX]    = snXA[stateX];
    hmmP->transNA[stateX] = transNA[stateX];
    while (transX >=0) {
      hmmP->transXA[stateX][transX] = transXA[stateX][transX];
      hmmP->prob.AA[stateX][transX] =  probAA[stateX][transX];
      transX--;
    }
  }
  return idx;
}

static int phmmSetAddGraphItf( ClientData cd, int argc, char *argv[])
{
  PHMMSet*    hmmsP = (PHMMSet*)cd;
  int            ac =  argc - 1;
  char*         snL = NULL;
  char*         trL = NULL;
  char**        snV = NULL;
  char**        trV = NULL;

  int           idx = 0;
  int        stateX = 0;
  int        stateN = 0;
  PHMMX*       snXA = NULL;
  PSTATE*   transNA = NULL;
  PSTATE**  transXA = NULL;
  float**    probAA = NULL;

  if ( itfParseArgv( argv[0], &ac, argv+1, 0,
      "<states>", ARGV_STRING, NULL, &snL, NULL, "list of states",
      "<trans>",  ARGV_STRING, NULL, &trL, NULL, "list of transitions for each state",
       NULL) != TCL_OK) return TCL_ERROR;

  if (Tcl_SplitList( itf,snL, &stateN, &snV) == TCL_ERROR || stateN < 1 || stateN >= PSTATE_MAX) {
    ERROR("phmmSetAddGraphItf: cannot interpret list of states\n");
    return TCL_ERROR;
  }
  if (Tcl_SplitList( itf,trL, &stateX, &trV) == TCL_ERROR || stateX != stateN) {
    ERROR("phmmSetAddGraphItf: cannot interpret list of transitions\n");
    return TCL_ERROR;
  }

    
  if (NULL == (snXA = malloc(stateN*sizeof(PHMMX)))) {
    ERROR("phmmSetAddGraphItf:  Allocation failure\n");
    return TCL_ERROR;
  }
  if (NULL == (transNA = malloc(stateN*sizeof(PSTATE)))) {
    ERROR("phmmSetAddGraphItf:  Allocation failure\n");
    return TCL_ERROR;
  }
  if (NULL == (transXA = malloc(stateN*sizeof(PSTATE*)))) {
    ERROR("phmmSetAddGraphItf:  Allocation failure\n");
    return TCL_ERROR;
  }
  if (NULL == (probAA = malloc(stateN*sizeof(float*)))) {
    ERROR("phmmSetAddGraphItf:  Allocation failure\n");
    return TCL_ERROR;
  }

  for (stateX=0;stateX<stateN;stateX++) {
    char** transV = NULL;
    int       snX = snsIndex(hmmsP->snsP,snV[stateX]);
    int       trN = 0;

    /* get state */
    if (snX < 0)  {
      ERROR("phmmSetAddGraphItf: Wrong state '%s'\n",snV[stateX]);
      return TCL_ERROR;
    }
    snXA[stateX] = snX;

    /* get list of transitions for each state */
    if (Tcl_SplitList( itf,trV[stateX], &trN, &transV) == TCL_ERROR || trN < 1) {
      ERROR("phmmSetAddGraphItf: cannot interpret list of transitions at '%s'\n",trV[stateX]);
      return TCL_ERROR;
    }
    transNA[stateX] = trN;

    if (NULL == (transXA[stateX] = malloc(trN*sizeof(PSTATE)))) {
      ERROR("phmmSetAddGraphItf:  Allocation failure\n");
      return TCL_ERROR;
    }
    if (NULL == (probAA[stateX] = malloc(trN*sizeof(float*)))) {
      ERROR("phmmSetAddGraphItf:  Allocation failure\n");
      return TCL_ERROR;
    }

    /* get transition index and probability for each transition */
    trN--;
    while (trN >= 0) {
      char** tmpV = NULL;
      int   itemN = 0;
      if (Tcl_SplitList( itf,transV[trN], &itemN, &tmpV) == TCL_ERROR || itemN != 2) {
	ERROR("phmmSetAddGraphItf: cannot interpret transition '%s'\n",transV[trN]);
	return TCL_ERROR;
      }
      transXA[stateX][trN] = stateX+atoi(tmpV[0]);
      probAA[stateX][trN]  = atof(tmpV[1]);
      if (tmpV) Tcl_Free((char*) tmpV);
      
      if (transXA[stateX][trN] > stateN+1) {
	ERROR("phmmSetAddGraphItf: Invalid transition index in '%s'\n",transV[trN]);
	return TCL_ERROR;
      }
      trN--;
    } /* for each transition */
    if (transV) Tcl_Free((char*) transV);
  } /* for each state */

  /* free tcl_split strings */
  if (trV) Tcl_Free((char*) trV);
  if (snV) Tcl_Free((char*) snV);

  /* create model */
  idx = phmmSetAddGraph(hmmsP,stateN, snXA, transNA, transXA, probAA);

  /* free array of states and transitions */
  if (transXA && probAA) {
    for (stateX=0;stateX<stateN;stateX++) {
      if (transXA[stateX]) free(transXA[stateX]);
      if (probAA[stateX])  free(probAA[stateX]);
    }
    free(transXA);
    free(probAA);
  }
  if (snXA)    free(snXA);
  if (transNA) free(transNA);

  /* return new PHMM index */
  itfAppendResult("%d",idx);

  return TCL_OK;
}


/* ------------------------------------------------------------------------
    phmmSetLoad
   ------------------------------------------------------------------------ */

int phmmSetLoad (PHMMSet* hmmsP,FILE* fp)
{
  phmmKey   key = {0,NULL};
  size_t sizeSN = sizeof(short);
  char str[1024];
  int hmmX,hmmN;

  if ((PHMMX) hmmsP->snsP->list.itemN > PHMM_MAX-1) {
    ERROR("phmmSetLoad: SenoneSet overflow (check slimits)\n");
    return TCL_ERROR;
  }
  if (! fp) {
    ERROR("phmmSetLoad: Invalid file pointer\n");
    return TCL_ERROR;
  }
  if (hmmsP->list.itemN > 0) {
    WARN("phmmSet: expected empty phmmSet (do it on your risk)\n");
  }

  read_string(fp,str);
  if (0 != strncmp(str,"PHMMSET-BEGIN",13)) {
    ERROR("phmmSetLoad: inconsistent file\n");
    return TCL_ERROR;
  }
  if (0 == strcmp(str,"PHMMSET-BEGIN 1.1")) {
    sizeSN = read_int(fp);
  }

  hmmN= read_int(fp);

  for (hmmX=0;hmmX<hmmN;hmmX++) {
    int typeFctX = -1;
    int typeFctY = -1;
    int      idx = -1;
    int     idx2 = -1;
    int        i =  0;

    PHMM*   hmmP = 0;
    key.stateN = read_int(fp);
    key.snXA   = _snXA;
    key.vitFn  = NULL;

    if (sizeSN == sizeof(short))
      for (i=0;i<key.stateN;i++) key.snXA[i] = (PHMMX) ((USHORT) read_short(fp));
    else
      for (i=0;i<key.stateN;i++) key.snXA[i] = (PHMMX) ((UINT) read_int(fp));

    typeFctX = read_int(fp); 
    typeFctY = read_int(fp); 
    
    if (typeFctX == 0 && typeFctY == 0) key.vitFn = (void*) vitGeneralRoot;
    if (typeFctX == 1 && typeFctY == 0) key.vitFn = (void*) vitGeneralNode;
    if (typeFctX == 2 && typeFctY == 0) key.vitFn = (void*) vitGeneralLeaf;
    if (typeFctX == 3 && typeFctY == 0) key.vitFn = (void*) vitGeneralSip;
    if (typeFctX == 4 && typeFctY == 0) key.vitFn = (void*) vitGeneralSdp;
    if (typeFctX == 0 && typeFctY == 1) key.vitFn = (void*) vit3stateRoot;
    if (typeFctX == 1 && typeFctY == 1) key.vitFn = (void*) vit3stateNode;
    if (typeFctX == 2 && typeFctY == 1) key.vitFn = (void*) vit3stateLeaf;
    if (typeFctX == 3 && typeFctY == 1) key.vitFn = (void*) vit3stateSip;
    if (typeFctX == 4 && typeFctY == 1) key.vitFn = (void*) vit3stateSdp;

    if ( key.vitFn == NULL) {
      ERROR("phmmSetLoad: unknown viterbi function\n");
      return TCL_ERROR;
    }
    
    
    idx2  = listIndex((List*) &(hmmsP->list),&key,0); 
    if (idx2 == -1) {
      idx  = listNewItem((List*) &(hmmsP->list), &key);
    } else {
      idx  = idx2;
    }
    hmmP = hmmsP->list.itemA+idx;

    hmmP->snXA     = (PHMMX*) bmemAlloc(hmmsP->mem.s,key.stateN*sizeof(PHMMX));
    hmmP->stateN   = key.stateN;
    hmmP->vitFn.e0 = (HmmEvalRootFn*) key.vitFn;
    memcpy(hmmP->snXA,key.snXA,key.stateN*sizeof(PHMMX));

    if (typeFctY == 0) {
      int stateX=0, transX=0, transN = 0;

      if (idx2 == -1) {
	hmmP->transNA = (PSTATE*)   bmemAlloc(hmmsP->mem.c,hmmP->stateN*sizeof(PSTATE));
	hmmP->transXA = (PSTATE**)  bmemAlloc(hmmsP->mem.f,hmmP->stateN*sizeof(PSTATE*));
	hmmP->prob.AA = (float**)   bmemAlloc(hmmsP->mem.f,hmmP->stateN*sizeof(float*));
      }

      for (stateX=0;stateX<hmmP->stateN;stateX++) {
	PSTATE tmp = (PSTATE) read_int(fp);
	if (idx2 >= 0 && hmmP->transNA[stateX] != tmp) {
	  WARN("phmmSetLoad: redefine phmm %d\n",idx);
	  idx = -1;
	}
	hmmP->transNA[stateX] = tmp;
	transN += (int)	hmmP->transNA[stateX]; 
      }

      if (idx2 == -1) {
	hmmP->prob.AA[0] = (float*)  bmemAlloc(hmmsP->mem.f,transN*sizeof(float));
	hmmP->transXA[0] = (PSTATE*) bmemAlloc(hmmsP->mem.c,transN*sizeof(PSTATE));
	for (stateX=1;stateX<hmmP->stateN;stateX++) {
	  hmmP->prob.AA[stateX]=hmmP->prob.AA[stateX-1]+ hmmP->transNA[stateX-1];
	  hmmP->transXA[stateX]=hmmP->transXA[stateX-1]+ hmmP->transNA[stateX-1];
	}
      }

      for (stateX=0;stateX<hmmP->stateN;stateX++) {
	for (transX=0;transX<hmmP->transNA[stateX];transX++) {
	  PSTATE tmp = (PSTATE) read_int(fp);
	  float tmp2 = read_float(fp);
	  hmmP->transXA[stateX][transX] = tmp;
	  hmmP->prob.AA[stateX][transX] = tmp2;
	}
      }
    }
    if (typeFctY == 1) {
      hmmP->transNA = NULL;
      hmmP->transXA = NULL;
      if (idx2 == -1) {
	hmmP->prob.A  = (float*) bmemAlloc(hmmsP->mem.f,6*sizeof(float));
      }
      read_floats(fp,hmmP->prob.A,6);
    }
  }
  read_string(fp,str);
  if (0 != strcmp(str,"PHMMSET-END")) {
    ERROR("phmmSetLoad: inconsistent file\n");
    return TCL_ERROR;
  }
  return TCL_OK;    
}

static int phmmSetLoadItf( ClientData cd, int argc, char *argv[])
{
  PHMMSet*   hmmsP = (PHMMSet*)cd;
  int        ac    =  argc - 1;
  char*      name  =  NULL;
  FILE*      fp    = NULL;

  if ( itfParseArgv( argv[0], &ac, argv+1, 0,
      "<filename>", ARGV_STRING, NULL, &name, cd, "file to load from",
       NULL) != TCL_OK) return TCL_ERROR;

  fp = fileOpen(name,"r");
  phmmSetLoad(hmmsP,fp);
  fileClose(name,fp);
  return TCL_OK;
}

/* ------------------------------------------------------------------------
    phmmSetSave
   ------------------------------------------------------------------------ */

int phmmSetSave (PHMMSet* hmmsP,FILE* fp)
{
  int hmmX;
  if (! fp) {
    ERROR("phmmSetSave: Invalid file pointer\n");
    return TCL_ERROR;
  }

  write_string(fp,"PHMMSET-BEGIN 1.1");
  write_int   (fp,sizeof(PHMMX));
  write_int   (fp,hmmsP->list.itemN); 
  
  for (hmmX=0;hmmX<hmmsP->list.itemN;hmmX++) {
    PHMM* hmmP   = hmmsP->list.itemA+hmmX;
    int typeFctX = -1;
    int typeFctY = -1;

    if (hmmP->vitFn.e0 == vitGeneralRoot) {
      typeFctX = 0;
      typeFctY = 0;
    }
    if (hmmP->vitFn.e1 == vitGeneralNode) {
      typeFctX = 1;
      typeFctY = 0;
    }
    if (hmmP->vitFn.e2 == vitGeneralLeaf) {
      typeFctX = 2;
      typeFctY = 0;
    }
    if (hmmP->vitFn.e3 == vitGeneralSip) {
      typeFctX = 3;
      typeFctY = 0;
    }
    if (hmmP->vitFn.e4 == vitGeneralSdp) {
      typeFctX = 4;
      typeFctY = 0; 
    }
    if (hmmP->vitFn.e0 == vit3stateRoot) {
      typeFctX = 0;
      typeFctY = 1;
    }
    if (hmmP->vitFn.e1 == vit3stateNode) {
      typeFctX = 1;
      typeFctY = 1;
    }
    if (hmmP->vitFn.e2 == vit3stateLeaf) {
      typeFctX = 2;
      typeFctY = 1;
    }
    if (hmmP->vitFn.e3 == vit3stateSip) {
      typeFctX = 3;
      typeFctY = 1;
    }
    if (hmmP->vitFn.e4 == vit3stateSdp) {
      typeFctX = 4;
      typeFctY = 1;
    }
    if (typeFctX < -1 || typeFctY < -1) {
      ERROR("phmmSetSave: Unknown viterbi function\n");
      return TCL_ERROR;
    }
    write_int   (fp,hmmP->stateN); 
    if (sizeof(PHMMX) == sizeof(short))
      write_shorts(fp,(short*) hmmP->snXA,hmmP->stateN); 
    else
      write_ints(fp,(int*) hmmP->snXA,hmmP->stateN); 
    write_int   (fp,typeFctX); 
    write_int   (fp,typeFctY); 

    if (typeFctY == 0) {
      int trX,stateX;
      for (stateX=0;stateX<hmmP->stateN;stateX++) {
	write_int(fp,(int) hmmP->transNA[stateX]);
      }
      for (stateX=0;stateX<hmmP->stateN;stateX++) {
	for (trX=0;trX<hmmP->transNA[stateX];trX++) {
	  write_int  (fp,hmmP->transXA[stateX][trX]);
	  write_float(fp,hmmP->prob.AA[stateX][trX]);
	}
      }
    }
    if (typeFctY == 1) {
      write_floats(fp,hmmP->prob.A,6);
    }
  }
  write_string(fp,"PHMMSET-END");
  fflush(fp);
  return TCL_OK;    
}

static int phmmSetSaveItf( ClientData cd, int argc, char *argv[])
{
  PHMMSet*   hmmsP = (PHMMSet*)cd;
  int        ac    =  argc - 1;
  char*      name  =  NULL;
  FILE*      fp    = NULL;

  if ( itfParseArgv( argv[0], &ac, argv+1, 0,
      "<filename>", ARGV_STRING, NULL, &name, cd, "file to save to",
       NULL) != TCL_OK) return TCL_ERROR;

  fp = fileOpen(name,"w");
  phmmSetSave(hmmsP,fp);
  fileClose(name,fp);
  return TCL_OK;
}


/* ------------------------------------------------------------------------
    phmmSetClearCtx
   ------------------------------------------------------------------------ */

int phmmSetClearCtx (PHMMSet* hmmsP)
{ 
  if (hmmsP->ctx.use) {
    bmemClear(hmmsP->ctx.memP);    
    free(hmmsP->ctx.list.sortXA);
    free(hmmsP->ctx.list.hashTable);
    free(hmmsP->ctx.list.hashXA);
    free(hmmsP->ctx.list.itemA);
    hmmsP->ctx.list.sortXA    = NULL;
    hmmsP->ctx.list.hashTable = NULL;
    hmmsP->ctx.list.hashXA    = NULL;
    hmmsP->ctx.list.itemA     = NULL;
    hmmsP->ctx.list.allocN =0;
    hmmsP->ctx.list.itemN  =0;
    listInit((List*)&(hmmsP->ctx.list),(TypeInfo*) NULL, sizeof(PHMMC),50000);
    hmmsP->ctx.list.init    = (ListItemInit   *)phmmcInit;
    hmmsP->ctx.list.deinit  = (ListItemDeinit *)phmmcDeinit;
    hmmsP->ctx.list.hashKey = (HashKeyFn      *)phmmcHashKey;
    hmmsP->ctx.list.hashCmp = (HashCmpFn      *)phmmcHashCmp;

  }
  return TCL_OK;
}

/* ------------------------------------------------------------------------
    phmmSetPuts
   ------------------------------------------------------------------------ */

static int phmmSetPutsItf (ClientData clientData, int argc, char *argv[])
{
  PHMMSet* hmmsP = (PHMMSet*)clientData;

  itfAppendResult("%d",hmmsP->list.itemN);

  return TCL_OK;    
}

/* ------------------------------------------------------------------------
    phmmSetAccessItf
   ------------------------------------------------------------------------ */

static ClientData phmmSetAccessItf (ClientData cd, char *name, TypeInfo **ti)
{ 
  PHMMSet *hmmsP = (PHMMSet*)cd;

  if (*name == '.') { 
    if (name[1]=='\0') {
      itfAppendElement( "tree");
      itfAppendElement( "senoneSet");
      itfAppendElement( "tmSet");
      *ti = NULL; return NULL;
    }
    else { 
      if (! strcmp( name+1, "tree")) {
        *ti = itfGetType("Tree");
        return (ClientData)hmmsP->treeP;
      }
      else if (! strcmp( name+1, "senoneSet")) {
        *ti = itfGetType("SenoneSet");
        return (ClientData)hmmsP->snsP;
      }
      else if (! strcmp( name+1, "tmSet")) {
        *ti = itfGetType("TmSet");
        return (ClientData)hmmsP->tmsP;
      } 
    }
  }
  else if (*name == ':') { 
    if (name[1]=='\0') { 
      itfAppendElement( "0 %d",hmmsP->list.itemN-1);
      *ti = NULL; return NULL;
    }
    else { 
      int i = atoi(name+1);
      *ti = &phmmInfo;
      if (i>=0 && i<hmmsP->list.itemN) 
           return (ClientData) &(hmmsP->list.itemA[i]);
      else return NULL;
    }
  }
  return NULL;
}


/* ========================================================================
    Type Information
   ======================================================================== */

Method phmmMethod[] = 
{
  { "puts",   phmmPutsItf,   "displays the senone sequence of a phone HMM"  },
  { NULL, NULL, NULL }
} ;

Method phmmSetMethod[] = 
{
  { "puts",   phmmSetPutsItf,     "displays the set of Phone models"  },
  { "add",    phmmSetAddGraphItf, "Add a PHMM by specifying a state graph" },
  { "load",   phmmSetLoadItf,     "load a set of Phone models"  },
  { "save",   phmmSetSaveItf,     "save a set of Phone models"  },
  { NULL, NULL, NULL }
} ;

TypeInfo phmmInfo = { 
        "PHMM", 0, 0, phmmMethod, 
	NULL, NULL, NULL, NULL, 
	itfTypeCntlDefaultNoLink,
        "phone hidden markov model" };

TypeInfo phmmSetInfo = 
{ "PHMMSet", 0, -1,  phmmSetMethod, 
   phmmSetCreateItf, phmmSetFreeItf, 
   NULL, phmmSetAccessItf, NULL,
  "set of phone hidden markov models"
};

static int phmmInitialized = 0;

int PHMM_Init (void)
{
  if (! phmmInitialized) {
    if ( Senones_Init() != TCL_OK) return TCL_ERROR;
    if ( Tree_Init()    != TCL_OK) return TCL_ERROR;
    if ( Topo_Init()    != TCL_OK) return TCL_ERROR;
    if ( Tm_Init()      != TCL_OK) return TCL_ERROR;

    phmmSetDefault.name             = NULL;
    phmmSetDefault.useN             = 0;
    phmmSetDefault.list.itemN       = 0;
    phmmSetDefault.list.blkSize     = 500;
    phmmSetDefault.treeP            = NULL;
    phmmSetDefault.snsP             = NULL;
    phmmSetDefault.tmsP             = NULL;
    phmmSetDefault.ctx.use          = 1; 
    itfNewType (&phmmInfo);
    itfNewType (&phmmSetInfo);

    phmmInitialized = 1;
  }
  return TCL_OK;
}

