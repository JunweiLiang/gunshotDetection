/* ========================================================================
    JANUS      Speech- to Speech Translation
               ------------------------------------------------------------
               Object: Single pass decoder
               ------------------------------------------------------------

    Author  :  Hagen Soltau
    Module  :  spass.c
    Date    :  $Id: spass.c 3196 2010-03-20 22:15:51Z metze $
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
    Revision 4.11  2007/03/19 16:09:19  fuegen
    bugfix in spassVitWord_SLeaf
    some other minor fixes

    Revision 4.10  2007/02/22 16:28:00  fuegen
    removed -pedantic compiler warnings with

    Revision 4.9  2006/11/14 16:41:44  fuegen
    support for endless decoding with partial hypothesis
    - bugfix for spass.pathSub method, which subtracts a constant score
      from all active paths
    - new method spass.traceStable which returns the stable hypotheses
      together with a list of backpointers for the next traceStable call

    Revision 4.7  2005/04/05 09:44:43  metze
    Added warning for stupid beams (not really useful, though)

    Revision 4.6  2005/04/01 16:19:11  metze
    Fixed typo

    Revision 4.5  2005/03/08 08:00:17  metze
    Added MICV support

    Revision 4.4  2004/05/27 11:29:49  metze
    Cleaned up treatment of frameShifts in glats

    Revision 4.3  2003/11/06 17:44:48  metze
    Bugfix when adding SIps later

    Revision 4.2  2003/09/01 14:51:30  metze
    Added -frameIdx option to STrace

    Revision 4.1  2003/08/14 11:20:08  fuegen
    Merged changes on branch jtk-01-01-15-fms (jaguar -> ibis-013)

    Revision 1.1.2.106  2003/08/13 08:58:02  fuegen
    added spassGetFinalLCT to get the LCT of the final BP

    Revision 1.1.2.105  2003/07/09 08:18:27  soltau
    spassRequest : fixed snXA access

    Revision 1.1.2.104  2003/07/08 16:05:12  soltau
    fixed snXA

    Revision 1.1.2.103  2003/06/26 14:36:01  metze
    Cosmetic

    Revision 1.1.2.102  2003/03/18 18:10:11  soltau
    Added time based constrains

    Revision 1.1.2.101  2003/01/15 14:33:43  metze
    SPass observes MostRecentlyReceivedSignal

    Revision 1.1.2.100  2003/01/10 10:57:26  fuegen
    spassAddIce_SNode and spassAddIce_SLeaf: asking for fillerTag
    only when word is unique

    Revision 1.1.2.99  2003/01/07 14:59:38  metze
    Consensus changes, added frameShift

    Revision 1.1.2.98  2002/12/11 09:44:36  soltau
    spassStart: fixed initial lct handling

    Revision 1.1.2.97  2002/11/21 17:10:10  fuegen
    windows code cleaning

    Revision 1.1.2.96  2002/11/13 10:08:14  soltau
    - Start and End words rely on LVX now
    - Changes for optional words
    - single phone are allowed to have dynamic LVX now

    Revision 1.1.2.95  2002/10/17 13:22:43  soltau
    code cleaning

    Revision 1.1.2.94  2002/10/07 07:37:23  soltau
    spassExpand_SDPhone: fixed token score update for rcm's
    spassPrune_SDPhone : fixed deactivation of pruned instances

    Revision 1.1.2.93  2002/08/27 08:48:49  metze
    itfParseArgv now uses <name> and NULL initialization for char*

    Revision 1.1.2.92  2002/07/18 12:11:50  soltau
    Changes according to optimized stree structures:
            bestscore -> bestX, worstscore -> worstX

    Revision 1.1.2.91  2002/06/28 16:06:31  soltau
    spassReinit: support for compressed strees

    Revision 1.1.2.90  2002/06/10 09:58:38  soltau
    spassRun: checking maximum score

    Revision 1.1.2.89  2002/04/26 13:25:14  soltau
    Enhanced spassConfigure

    Revision 1.1.2.88  2002/03/23 14:36:00  soltau
    spassClear: deactivate sdp's

    Revision 1.1.2.87  2002/03/22 10:49:51  soltau
    Initialize worstscore with -score_max to cover negative acoustic scores

    Revision 1.1.2.86  2002/03/01 10:00:48  soltau
    cleaned spassDeinit again

    Revision 1.1.2.85  2002/02/28 16:56:17  soltau
    cleaned spassDeinit

    Revision 1.1.2.84  2002/02/21 07:06:58  metze
    Call GetOneLMScore instead of svmapGetLMScore

    Revision 1.1.2.83  2002/02/14 12:02:17  soltau
    support for Single_Mode in LTree

    Revision 1.1.2.82  2002/02/11 10:59:16  soltau
    Added 'case 1b' for instance management

    Revision 1.1.2.81  2002/01/29 16:25:12  soltau
    Removed assertions from spassInitActiveSNodes

    Revision 1.1.2.80  2002/01/28 18:51:17  soltau
    Fixed passVitWord_SDPhone

    Revision 1.1.2.79  2002/01/27 17:49:09  soltau
    support for sdphones in glat

    Revision 1.1.2.78  2002/01/27 15:15:26  soltau
    spassConvertBP : extend lct's for sdphones

    Revision 1.1.2.77  2002/01/14 13:38:04  soltau
    Check for compression flag in spassReinit

    Revision 1.1.2.76  2002/01/11 15:54:49  soltau
    Extended ana_morph to trace tree level information

    Revision 1.1.2.75  2002/01/10 13:37:24  soltau
    Decoding along shared suffixes

    Revision 1.1.2.74  2002/01/09 17:06:53  soltau
    changed child access in stree

    Revision 1.1.2.73  2002/01/08 11:26:50  soltau
    spassRestart: added lattice constraint check

    Revision 1.1.2.72  2001/12/17 12:00:45  soltau
    Removed position mapper

    Revision 1.1.2.71  2001/12/17 09:44:14  soltau
    Deactivation of position mapper

    Revision 1.1.2.70  2001/11/20 16:36:31  soltau
    fixed minor bugs

    Revision 1.1.2.69  2001/10/23 13:18:16  soltau
    Added support for 1state fast match module

    Revision 1.1.2.68  2001/10/15 16:25:32  soltau
    *** empty log message ***

    Revision 1.1.2.67  2001/10/12 17:08:06  soltau
    Improved path recombination

    Revision 1.1.2.66  2001/10/04 17:35:44  soltau
    Check for LMSCORE_MAX

    Revision 1.1.2.65  2001/10/03 14:49:20  soltau
    *** empty log message ***

    Revision 1.1.2.64  2001/10/02 19:10:33  soltau
    Increased size of static child2parent mapper

    Revision 1.1.2.63  2001/10/01 10:47:38  soltau
    Use polymorphism defines from slimits
    Fixed bug in Expand routines

    Revision 1.1.2.62  2001/09/25 15:53:55  soltau
    Check for invalid lct in spassStart

    Revision 1.1.2.61  2001/09/25 14:51:25  soltau
    *** empty log message ***

    Revision 1.1.2.60  2001/09/24 14:21:35  soltau
    Fixed Expand_{Root,SIPhone,SDPhone} routines
      instances wouldn't be added correctly if tryN exceed

    Added analyze statistics for polymorphism
      activate by ANA_MORPH

    Revision 1.1.2.59  2001/08/16 15:35:57  soltau
    avoid sequence of identical filler words

    Revision 1.1.2.58  2001/08/08 19:10:51  soltau
    Removed changes from 1.1.2.57

    Revision 1.1.2.57  2001/08/08 12:13:28  soltau
    Check if have backpointers to convert

    Revision 1.1.2.56  2001/07/31 10:08:53  soltau
    Update *hmmP array pointer

    Revision 1.1.2.55  2001/07/26 17:39:02  soltau
    Fixed lms values in spassExpand_SIPhone

    Revision 1.1.2.54  2001/07/25 11:49:10  soltau
    Added warning for missing sdp support in glat

    Revision 1.1.2.53  2001/07/25 11:35:03  soltau
    Fixed stabFinalize <-> glatMapBP conflict

    Revision 1.1.2.52  2001/07/25 09:45:20  soltau
    Update lms score for early path recombination

    Revision 1.1.2.51  2001/07/24 17:12:34  soltau
    Call stabFinalize

    Revision 1.1.2.50  2001/07/14 15:30:24  soltau
    Added token propagation functions for context dependent single phone words

    Revision 1.1.2.49  2001/07/06 13:12:01  soltau
    Changed compress and scoreFct handling

    Revision 1.1.2.48  2001/07/03 09:46:53  soltau
    Changes according to the new acoustic score function interface

    Revision 1.1.2.47  2001/06/25 21:06:56  soltau
    Changed smem interface

    Revision 1.1.2.46  2001/06/20 17:50:04  soltau
    Removed glat beams
    Fixed spassReinit

    Revision 1.1.2.45  2001/06/12 20:08:23  soltau
    Changed lattice topN to 0 as default (no lattice generation at all)

    Revision 1.1.2.44  2001/06/11 15:00:10  soltau
    Initialize spassP->fmatch.factor

    Revision 1.1.2.43  2001/06/11 14:11:10  soltau
    Removed spassP->lvXA
    Initialize lms for streeP->start in spassStart

    Revision 1.1.2.42  2001/06/05 10:32:44  soltau
    Changes according to the new G'Lat interface

    Revision 1.1.2.41  2001/06/01 14:26:37  soltau
    Added glatClear

    Revision 1.1.2.40  2001/06/01 10:08:11  metze
    Made changes necessary for decoding along lattices

    Revision 1.1.2.39  2001/06/01 10:35:43  soltau
    Added G'Lat support

    Revision 1.1.2.38  2001/05/25 13:22:16  soltau
    Added phonetic fast match strategie

    Revision 1.1.2.37  2001/05/11 16:40:45  metze
    Cleaned up LCT interface

    Revision 1.1.2.36  2001/05/09 15:00:31  metze
    Made the bitmask for hashkey generation an LCT

    Revision 1.1.2.35  2001/04/27 07:39:03  metze
    General access functions to LM via 'LCT' implemented

    Revision 1.1.2.34  2001/04/25 10:52:39  soltau
    changes according to the new lmla interface

    Revision 1.1.2.33  2001/04/23 13:44:01  soltau
    Added restart method to add and delete words during decoding

    Revision 1.1.2.32  2001/04/19 15:36:49  soltau
    - Changed final frame handling
    - Made run-on working
    - Made partial trace back working

    Revision 1.1.2.31  2001/04/12 19:23:36  soltau
    removed same_pron helper

    Revision 1.1.2.30  2001/04/12 18:41:56  soltau
    made same_pron working

    Revision 1.1.2.29  2001/04/04 08:55:43  soltau
    Changel handling of lm scores

    Revision 1.1.2.28  2001/04/03 16:48:28  soltau
    Changed handling of RCM lm pruning, but still not working

    Revision 1.1.2.27  2001/03/27 20:50:54  soltau
    WARN: this version based on 1.1.2.25 and not 1.2.2.26
    Changed handling of lm lookaheads for scores

    Revision 1.1.2.26  2001/03/27 14:26:58  soltau
    global instance pruning

    Revision 1.1.2.25  2001/03/27 10:13:04  soltau
    Use wordbeam if this was the intention

    Revision 1.1.2.24  2001/03/27 09:52:14  soltau
    Apply wordbeam if word become unique

    Revision 1.1.2.23  2001/03/26 17:19:14  soltau
    log search space information

    Revision 1.1.2.22  2001/03/26 06:14:29  soltau
    - Apply morph-beam *and* state-beam for instance transactions
      (enabled with HARD_PRUNE)

    - Do not prune backpointer table in last frame to keep the final state

    Revision 1.1.2.21  2001/03/23 13:35:42  soltau
    Added state+morph threshold for phone transactions
    Deleted phone beam

    Revision 1.1.2.20  2001/03/22 17:20:41  soltau
    - Cleaned handling of position mapper during phone transactions
    - propagate linguistic context for single phone that are not filler models
    - Added sanity checks

    Revision 1.1.2.19  2001/03/20 17:09:37  soltau
    Changed ExpandRoot and ExpandSIPhone loops
      TopN will now be applied in a correct delayed
      way during tihe instance pruning operation

    Revision 1.1.2.18  2001/03/20 13:37:11  soltau
    Reorganized pruning strategie
     - no phone beam anymore
     - criterion for instance transaction is now only the morphbeam
     - delayed application of instance pruning

    Revision 1.1.2.17  2001/03/16 17:18:02  soltau
    renamed lmla object

    Revision 1.1.2.16  2001/03/16 14:54:58  soltau
    endSVX changes

    Revision 1.1.2.15  2001/03/14 12:53:03  soltau
    Added   lmrcmA
    Changed strace interface

    Revision 1.1.2.14  2001/03/13 18:38:40  soltau
    Added lm lookahead pruning for right context models

    Revision 1.1.2.13  2001/03/13 09:55:25  soltau
    use wordbeam for leafs and right context models instead of statebeam

    Revision 1.1.2.12  2001/03/12 19:07:26  soltau
    - Added spassConfigure
    - Added spassCut_SLeaf
      but not useful yet
    - Fixed spassExpand
      deactivate non-surving roots and sip's
    - Changed allocation of position mapper
      all pos2pos arrays will be allocated on demand now
    - Fixed a couple of instance pruning problems again
    - Cleaned debug output
    - Changed lmla handling

    Revision 1.1.2.11  2001/03/09 07:19:21  soltau
    - Changed spassExpand methods
    - Fixed phone transaction criterion (compare now with the begin state
      and not with overall best instance score)
    - Removed some instance pruning problems

    Revision 1.1.2.10  2001/03/06 09:27:26  soltau
    Fixed spassExpand problems

    Revision 1.1.2.9  2001/03/06 06:46:46  soltau
    Fiexed activation problems

    Revision 1.1.2.8  2001/02/27 13:47:53  soltau
    Fixed a couple of bugs

    Revision 1.1.2.7  2001/02/22 16:13:22  soltau
    Added senone interface

    Revision 1.1.2.6  2001/02/21 18:02:17  soltau
    Added    spassConvertBP
    Added    spassExpandRoot
    Added    spassExpandSIPhone
    Added    spassProcess
    Adjusted spassInitTrans

    Revision 1.1.2.5  2001/02/20 12:36:46  soltau
    *** empty log message ***

    Revision 1.1.2.4  2001/02/16 17:51:54  soltau
    Fixed word transition loops
    Added (preliminary) spassExpand routin

    Revision 1.1.2.3  2001/02/15 18:08:24  soltau
    Fixed lmla init call problems

    Revision 1.1.2.2  2001/02/15 18:03:32  soltau
    *** empty log message ***

    Revision 1.1.2.1  2001/02/02 16:11:10  soltau
    initial revision

   ======================================================================== */

char spassRCSVersion[]=
    "$Id: spass.c 3196 2010-03-20 22:15:51Z metze $";

#include <signal.h>
#include "scoreA.h"
#include "spass.h" 
#include "distribStream.h"

#ifndef IBIS
#include "micvStream.h"
#endif

/* ========================================================================
    SPASS init routines
   ======================================================================== */

extern TypeInfo  spassInfo;
static SPass     spassDefault;

/* ------------------------------------------------------------------------
    spassInitSenone
   ------------------------------------------------------------------------ */

static int spassInitSenone (SPass* spassP)
{
  SenoneSet* snsP = spassP->streeP->hmmsP->snsP;

  spassP->senone.snsP    = snsP;
  spassP->senone.totalN  = snsP->list.itemN;
  spassP->senone.activeN = 0;

  if (NULL == (spassP->senone.scoreA = (float*) 
	       malloc(spassP->senone.totalN*sizeof(float)))) {
    ERROR("spassInitSenone: allocation failure\n");
    return TCL_ERROR;
  }
  if (NULL == (spassP->senone.indexA = (int*) 
	       malloc(spassP->senone.totalN*sizeof(int)))) {
    ERROR("spassInitSenone: allocation failure\n");
    return TCL_ERROR;
  }
  if (NULL == (spassP->senone.activeA = (UCHAR*) 
	       calloc(spassP->senone.totalN,sizeof(UCHAR)))) {
    ERROR("spassInitSenone: allocation failure\n");
    return TCL_ERROR;
  }
  return TCL_OK;
}

/* ------------------------------------------------------------------------
    spassDeinitSenone
   ------------------------------------------------------------------------ */

static int spassDeinitSenone (SPass* spassP)
{
  if (spassP->senone.scoreA)  free(spassP->senone.scoreA);
  if (spassP->senone.indexA)  free(spassP->senone.indexA);
  if (spassP->senone.activeA) free(spassP->senone.activeA);

  spassP->senone.totalN  = 0;
  spassP->senone.activeN = 0;
  spassP->senone.scoreA  = NULL;
  spassP->senone.indexA  = NULL;
  spassP->senone.activeA = NULL;

  return TCL_OK;
}

/* ------------------------------------------------------------------------
    spassInitActiceSNodes
   ------------------------------------------------------------------------ */

static int spassInitActiveSNodes (SPass* spassP)
{

  spassP->active.rootN = spassP->streeP->rootTbl.N;
  spassP->active.nodeN = spassP->streeP->nodeTbl.nodeN;
  spassP->active.leafN = spassP->streeP->nodeTbl.leafN;

  if (NULL == (spassP->active.rootPA = (SRoot**) 
	       malloc(spassP->active.rootN*sizeof(SRoot*)))) {
    ERROR("spassInitActiveSNodes: allocation failure\n");
    return TCL_ERROR;
  }
  if (NULL == (spassP->active.nodePA = (SNode**) 
	       malloc(spassP->active.nodeN*sizeof(SNode*)))) {
    ERROR("spassInitActiveSNodes: allocation failure\n");
    return TCL_ERROR;
  }
  if (NULL == (spassP->active.leafPA = (SNode**) 
	       malloc(spassP->active.leafN*sizeof(SNode*)))) {
    ERROR("spassInitActiveSNodes: allocation failure\n");
    return TCL_ERROR;
  }
  if (NULL == (spassP->actNext.rootPA = (SRoot**) 
	       malloc(spassP->active.rootN*sizeof(SRoot*)))) {
    ERROR("spassInitActiveSNodes: allocation failure\n");
    return TCL_ERROR;
  }
  if (NULL == (spassP->actNext.nodePA = (SNode**) 
	       malloc(spassP->active.nodeN*sizeof(SNode*)))) {
    ERROR("spassInitActiveSNodes: allocation failure\n");
    return TCL_ERROR;
  }
  if (NULL == (spassP->actNext.leafPA = (SNode**) 
	       malloc(spassP->active.leafN*sizeof(SNode*)))) {
    ERROR("spassInitActiveSNodes: allocation failure\n");
    return TCL_ERROR;
  }

  spassP->active.rootN  = 0;
  spassP->active.nodeN  = 0;
  spassP->active.leafN  = 0;
  spassP->actNext.rootN = 0;
  spassP->actNext.nodeN = 0;
  spassP->actNext.leafN = 0;

  return TCL_OK;
}

/* ------------------------------------------------------------------------
    spassDeinitActiceSNodes
   ------------------------------------------------------------------------ */

static int spassDeinitActiveSNodes (SPass* spassP)
{
  if (spassP->active.rootPA)  free(spassP->active.rootPA);
  if (spassP->active.nodePA)  free(spassP->active.nodePA);
  if (spassP->active.leafPA)  free(spassP->active.leafPA);
  if (spassP->actNext.rootPA) free(spassP->actNext.rootPA);
  if (spassP->actNext.nodePA) free(spassP->actNext.nodePA);
  if (spassP->actNext.leafPA) free(spassP->actNext.leafPA);

  spassP->active.rootN   = 0;
  spassP->active.nodeN   = 0;
  spassP->active.leafN   = 0;
  spassP->active.rootPA  = NULL; 
  spassP->active.nodePA  = NULL; 
  spassP->actNext.rootN  = 0;
  spassP->actNext.nodeN  = 0;
  spassP->actNext.leafN  = 0;
  spassP->actNext.leafPA = NULL; 
  spassP->actNext.rootPA = NULL; 
  spassP->actNext.nodePA = NULL; 
  spassP->actNext.leafPA = NULL; 

  return TCL_OK;
}

/* ------------------------------------------------------------------------
    spassInitTrans
   ------------------------------------------------------------------------ */

static int spassInitTrans (SPass* spassP)
{
  STree* streeP = spassP->streeP;
  int   phonesN = streeP->dictP->phones->list.itemN;
  float     rsN = streeP->rootTbl.N + streeP->sTbl.iN + streeP->sTbl.dN;
  int      topN = spassP->beam.topN;
  int      topX = 0;
  int    phoneX = 0;
  int    stateN = 0;
  
  /* search for topology with maximum number of states */
  {
    TopoSet* tpsP = streeP->hmmsP->tpsP;
    int     topoN = tpsP->list.itemN;
    int     topoX = 0;

    for (topoX=0;topoX<topoN;topoX++) {
      Topo* tpP = tpsP->list.itemA+topoX;
      if (stateN < tpP->tmN)
	stateN = tpP->tmN;
    }
    assert(stateN >0);
  }

  if (NULL == (spassP->trans.N = (int*) malloc(phonesN*sizeof(int)))) {
    ERROR("spassInitTrans: allocation failure\n");
    return TCL_ERROR;
  }
  if (NULL == (spassP->trans.A = (LIceExt**) malloc(phonesN*sizeof(LIceExt*)))) {
    ERROR("spassInitTrans: allocation failure\n");
    return TCL_ERROR;
  }
  if (NULL == (spassP->trans.A[0] = (LIceExt*) malloc(phonesN*topN*sizeof(LIceExt)))) {
    ERROR("spassInitTrans: allocation failure\n");
    return TCL_ERROR;
  }
  for (phoneX=1;phoneX<phonesN;phoneX++) {
    spassP->trans.A[phoneX] = spassP->trans.A[phoneX-1]+topN;
  }
  if (NULL == (spassP->trans.lmscore = (float**) malloc(topN*sizeof(float*)))) {
    ERROR("spassInitTrans: allocation failure\n");
    return TCL_ERROR;
  }

  for (topX=0;topX<topN;topX++) {
    int rsX;
    if (NULL == (spassP->trans.lmscore[topX] = 
		 (float*) malloc(rsN*sizeof(float)))) {
      ERROR("spassInitTrans: allocation failure\n");
      return TCL_ERROR;
    }
    for (rsX=0;rsX<rsN;rsX++) {
      spassP->trans.lmscore[topX][rsX] = 0.0;
    }
  }

  return TCL_OK;
}


/* ------------------------------------------------------------------------
    spassDeinitTrans
   ------------------------------------------------------------------------ */

static int spassDeinitTrans (SPass* spassP)
{  
  int topN = spassP->beam.topN;
  int topX = 0;

  if (spassP->trans.N) free(spassP->trans.N);
  if (spassP->trans.A) {
    if (spassP->trans.A[0]) {
      free(spassP->trans.A[0]);
    }
    free(spassP->trans.A);
  }  
  if (spassP->trans.lmscore) {
    for (topX=0;topX<topN;topX++) {
      if (spassP->trans.lmscore[topX]) 
	free(spassP->trans.lmscore[topX]);
    }
    free(spassP->trans.lmscore);
  }

  spassP->trans.N       = NULL;
  spassP->trans.A       = NULL;
  spassP->trans.lmscore = NULL;

  return TCL_OK;
}

/* ------------------------------------------------------------------------
    spassInitFMatch
   ------------------------------------------------------------------------ */

static int spassInitFMatch (SPass* spassP)
{
  int phoneX;

  spassP->fmatch.snsP    = NULL;
  spassP->fmatch.activeN = 0;
  spassP->fmatch.factor  = 0.0;
  spassP->fmatch.frameN  = 0;
  spassP->fmatch.stateN  = 0;
  spassP->fmatch.scoreA  = NULL;
  spassP->fmatch.indexA  = NULL;
  spassP->fmatch.snXA    = NULL;
  spassP->fmatch.phoneN  = spassP->streeP->dictP->phones->list.itemN;
 
  if (NULL == (spassP->fmatch.s = (float*)
	       malloc(spassP->fmatch.phoneN*sizeof(float)))) {
    ERROR("spassInitFMatch: allocation failure\n");
    return TCL_ERROR;
  }

  for (phoneX=0;phoneX< spassP->fmatch.phoneN;phoneX++) {
    spassP->fmatch.s[phoneX] = 0.0;
  }
  return TCL_OK;
}

/* ------------------------------------------------------------------------
    spassDeinitFMatch
   ------------------------------------------------------------------------ */

static int spassDeinitFMatch (SPass* spassP)
{
  int frameX;

  if (spassP->fmatch.scoreA) { 
    for (frameX=0;frameX<spassP->fmatch.frameN;frameX++) {
      if (spassP->fmatch.scoreA[frameX])
	free (spassP->fmatch.scoreA[frameX]);
    }
    free (spassP->fmatch.scoreA);
  }
  if (spassP->fmatch.indexA) free(spassP->fmatch.indexA);
  if (spassP->fmatch.snXA)   free(spassP->fmatch.snXA);
  if (spassP->fmatch.s)      free(spassP->fmatch.s);

  spassP->fmatch.snsP    = NULL;
  spassP->fmatch.activeN = 0;
  spassP->fmatch.frameN  = 0;
  spassP->fmatch.stateN  = 0;
  spassP->fmatch.scoreA  = NULL;
  spassP->fmatch.indexA  = NULL;
  spassP->fmatch.phoneN  = 0;

  return TCL_OK;
}

/* ------------------------------------------------------------------------
    spassInit
   ------------------------------------------------------------------------ */

static int spassInit (SPass* spassP, char* name, STree* streeP, LMLA* lmlaP)
{
  if (streeP->smem.spassP) {
    ERROR("spassInit: cannot link to Memory Manager (already in use)\n");
    return TCL_ERROR;
  }
  spassP->name    = strdup(name);
  spassP->useN    = 0;
  spassP->streeP  = streeP;
  spassP->lmlaP   = lmlaP;

  link(spassP->streeP, "STree");
  link(spassP->lmlaP,  "LTree");
  
  spassP->streeP->smem.spassP = spassP;

  spassP->hmmP    = streeP->hmmsP->list.itemA;
  spassP->lcmP    = streeP->lcmsP->list.itemA;
  spassP->rcmP    = streeP->rcmsP->list.itemA;

  if (streeP->xcmsP)
    spassP->xcmP  = streeP->xcmsP->list.itemA;
  else
    spassP->xcmP  = NULL;

  if (NULL == (spassP->stabP = stabCreate(streeP->svmapP))) {
    ERROR("spassInit: allocation failure\n");
    return TCL_ERROR;
  }

  spassP->beam      = spassDefault.beam;
  spassP->beam.best = 0.0;
  spassP->frameX    = 0;
  spassP->restart   = 0;

  /* The memory management needs to know the maximum request for instances */
  streeP->smem.morphMax = spassP->beam.morphN;

  spassP->lmlaP = lmlaP;

  if (TCL_ERROR == (spassInitSenone(spassP)))  
    return TCL_ERROR;
  if (TCL_ERROR == (spassInitActiveSNodes(spassP)))  
    return TCL_ERROR;
  if (TCL_ERROR == (spassInitTrans(spassP)))  
    return TCL_ERROR;
  if (TCL_ERROR == (spassInitFMatch(spassP)))  
    return TCL_ERROR;

  spassP->latP = glatCreate("spassLat",streeP->svmapP,spassP);

  return TCL_OK;
}

/* ------------------------------------------------------------------------
    spassCreate
   ------------------------------------------------------------------------ */

static SPass* spassCreate (char* name, STree* streeP, LMLA* lmlaP)
{
 SPass *spassP = (SPass*) malloc(sizeof(SPass));
  
  if (! spassP) {
    ERROR("spassCreate: allocation failure\n");
    return NULL;
  }
  if (TCL_ERROR == (spassInit(spassP,name,streeP,lmlaP))) {    
    free(streeP);
    return NULL;
  }
  return spassP;
}

static ClientData spassCreateItf( ClientData clientData, 
                                      int argc, char *argv[])
{
  STree* streeP =  NULL;
  LMLA*  lmlaP  =  NULL;
  char* name = NULL;

  if ( itfParseArgv( argv[0], &argc, argv, 0,
      "<name>",  ARGV_STRING, NULL, &name, NULL,         "name of the search pass objects",
      "<STree>",ARGV_OBJECT, NULL, &streeP, "STree", "Search Tree",
      "<LTree>",ARGV_OBJECT, NULL, &lmlaP,  "LTree", "LM Tree",

      NULL) != TCL_OK) return NULL;
  
  return (ClientData) spassCreate(name,streeP,lmlaP);
}

/* ------------------------------------------------------------------------
    spassDeinit
   ------------------------------------------------------------------------ */

/* forward declaration */
static void spassClear (SPass* spassP, int clearStab);

static int spassDeinit (SPass* spassP)
{
  spassClear(spassP,1);

  delink(spassP->streeP,"STree");
  delink(spassP->lmlaP, "LTree");

  spassP->streeP->smem.spassP = NULL;

  if (TCL_ERROR == (glatFree(spassP->latP)))
    return TCL_ERROR;
  if (TCL_ERROR == (spassDeinitSenone(spassP)))  
    return TCL_ERROR;
  if (TCL_ERROR == (spassDeinitActiveSNodes(spassP)))  
    return TCL_ERROR;
  if (TCL_ERROR == (spassDeinitTrans(spassP)))  
    return TCL_ERROR;
  if (TCL_ERROR == (spassDeinitFMatch(spassP)))  
    return TCL_ERROR;
  if (TCL_ERROR == (stabFree(spassP->stabP)))
    return TCL_ERROR;

  return TCL_OK;
}

/* ------------------------------------------------------------------------
    spassFree
   ------------------------------------------------------------------------ */

static int spassFree (SPass* spassP)
{
  if (spassP) {
    if ( spassDeinit(spassP) != TCL_OK) return TCL_ERROR;
    free(spassP);
  }
  return TCL_OK;
}

static int spassFreeItf (ClientData clientData)
{
  return spassFree((SPass*)clientData);
}

/* ========================================================================
    SPASS heap operations : add instances to a heap
   ======================================================================== */

/* ------------------------------------------------------------------------
    add a instance for Nodes
   ------------------------------------------------------------------------ */

static void spassAddIce_SNode(SPass* spassP, SNode* snodeP, LCT lct, SVX svX,
			      int parent_morphX, float new_score, float lms,
			      int bpIdx)
{
  register NIce*  iceP = NULL;
  register NIce** iceA = snodeP->iceA.n;
  register int      tX = LPM_NIL;
  LCT           orgLCT = lct;

  /* --------------------------------------------- */
  /* early recombination step for unique words     */
  /* If the word is unique, we can apply the exact */
  /* lm score and have to keep only instances with */
  /* histories which differ in the last N-2 words  */
  /* for a N-gram model.                           */
  /* --------------------------------------------- */

  if (snodeP->info == WordIsUnique && spassP->streeP->svocabP->list.itemA[svX].fillerTag == 0)
    lct = ExtendLCT (spassP->lmlaP, lct, snodeP->vX.s);

  tX= snodeP->morphN-1;
  while (tX > -1 && iceA[tX]->lct != lct) tX--;
  if (tX == -1) 
    tX = LPM_NIL;

  /* --------------------------------------------- */
  /* case 1: there is already a instance with this */
  /*         linguistic context, check if we have  */
  /*         already the better one                */
  /* --------------------------------------------- */
  if (tX != LPM_NIL && lct == orgLCT && fabs(lms-iceA[tX]->lms) <1e-6) {
    iceP = iceA[tX];
    /* replace token in the begin state if the new one has a better score */
    if (iceP->path[1].s < new_score) return;
    iceP->path[1].s     = new_score;
    iceP->path[1].bpIdx = bpIdx;

    /* replace instance score if the new one is better */
    if (iceP->score <= new_score) return;
    iceP->score = new_score;
    iceP->lms   = lms;
    iceP->svX   = svX;

    /* update best score */
    if (new_score < iceA[snodeP->bestX]->score) {
      snodeP->bestX = tX;
    }
    
    /* update worst score */
    if (tX == snodeP->worstX) {
      int       morphX = snodeP->morphN -1;
      float worstscore = -SCORE_MAX;
      
      for(;morphX>=0;morphX--)
	if (iceA[morphX]->score > worstscore) {
	  snodeP->worstX = morphX;
	  worstscore     = iceA[morphX]->score;
	}
    }

    return;
  }

  /* --------------------------------------------- */
  /* case 1b : same lct but different predecessor  */
  /* --------------------------------------------- */
  if (tX != LPM_NIL && (lct != orgLCT || fabs(lms-iceA[tX]->lms) >=1e-6)) {
    iceP = iceA[tX];

    /* replace instance score if the new one is better */
    if (iceP->score <= new_score) return;
    iceP->score = new_score;
    iceP->lms   = lms;
    iceP->svX   = svX;

    /* replace token score */
    iceP->path[1].s     = new_score;
    iceP->path[1].bpIdx = bpIdx;

    /* update best score */
    if (new_score < iceA[snodeP->bestX]->score) {
      snodeP->bestX = tX;
    }
    
    /* update worst score */
    if (tX == snodeP->worstX) {
      int       morphX = snodeP->morphN -1;
      float worstscore = -SCORE_MAX;
      
      for(;morphX>=0;morphX--)
	if (iceA[morphX]->score > worstscore) {
	  snodeP->worstX = morphX;
	  worstscore     = iceA[morphX]->score;
	}
    }

    return;
  }

  /* ---------------------------------------------- */
  /* case 2a: we have am unseen instance and enough */
  /*          space to store it                     */
  /* ---------------------------------------------- */
  if (snodeP->morphN < snodeP->allocN) {
    tX = (snodeP->morphN)++;
    iceP                  = iceA[tX];
    iceP->lct             = lct;
    iceP->score           = new_score;
    iceP->lms             = lms;
    iceP->svX             = svX;
    iceP->path[1].s       = new_score;
    iceP->path[1].bpIdx   = bpIdx;      
    if (snodeP->morphN == 1 || new_score < iceA[snodeP->bestX]->score) {
      snodeP->bestX = tX;
    }
    if (snodeP->morphN == 1 || new_score > iceA[snodeP->worstX]->score) {
      snodeP->worstX = tX;
    }
    return;
  }
  
  /* --------------------------------------------- */
  /* case 2b: we have an unseen instance and can   */
  /*          allocate new space                   */
  /* --------------------------------------------- */
  if (snodeP->allocN+1 <= spassP->beam.morphN) {
    snodeP->allocN++;
    smemAllocNode(snodeP,spassP->streeP);
    iceA= snodeP->iceA.n;
    tX = (snodeP->morphN)++;
    iceP                  = iceA[tX];
    iceP->lct             = lct;
    iceP->score           = new_score;
    iceP->lms             = lms;
    iceP->svX             = svX;
    iceP->path[1].s       = new_score;
    iceP->path[1].bpIdx   = bpIdx;   
    if (snodeP->morphN == 1 || new_score < iceA[snodeP->bestX]->score) {
      snodeP->bestX = tX;
    }
    if (snodeP->morphN == 1 || new_score > iceA[snodeP->worstX]->score) {
      snodeP->worstX = tX;
    }
    return;
  }

  /* --------------------------------------------- */
  /* case 2c: we have a unseen instance and no     */
  /*          space anymore, delete the worst      */
  /*          instance                             */
  /* --------------------------------------------- */
  if (new_score < iceA[snodeP->worstX]->score) {
    /* This case is handled very slowly, since we prefer
       a unsorted list of instances (with fast insert)
       and hope that we prune usually with a beam and not
       with a topN. The case 2c occur only if we have very
       aggressive morphN settings to constrain the memory.
    */
    PHMM*       hmmP  = spassP->hmmP + snodeP->X.hmm;
    int        stateN = hmmP->stateN+1;
    int        stateX = 0;
    float  worstscore = -SCORE_MAX;

    tX = snodeP->worstX;
    assert(tX < snodeP->morphN);
    iceP = iceA[tX];

    /* deactivate all states of this instance */
    for (stateX=0;stateX<stateN;stateX++) {
      iceP->path[stateX].s = SCORE_MAX;
    }

    /* activate first state for new instance */
    iceP->lct           = lct;
    iceP->score         = new_score;
    iceP->lms           = lms;
    iceP->svX           = svX;
    iceP->path[1].s     = new_score;
    iceP->path[1].bpIdx = bpIdx;   
    if (new_score < iceA[snodeP->bestX]->score) {
      snodeP->bestX = tX;
    }
    /* update worst_score */
    for (tX= snodeP->morphN-1; tX>=0; tX--) {
      if (iceA[tX]->score > worstscore) {
	worstscore     = iceA[tX]->score;
	snodeP->worstX = tX;
      }
    }
    return;
  }
}

/* ------------------------------------------------------------------------
    add a instance for Leafs
   ------------------------------------------------------------------------ */

static void spassAddIce_SLeaf(SPass* spassP, SNode* sleafP, LCT lct, SVX svX,
			      LPM parent_morphX, float new_score, float lms,
			      int bpIdx)
{
  RCM*            rcmP = spassP->rcmP + sleafP->X.xhmm;
  PHMM*           hmmP = spassP->hmmP + rcmP->hmmXA[0];
  int             hmmN = rcmP->hmmN;
  int           stateN = hmmP->stateN+1;
  LCT           orgLCT = lct;

  register LIce*  iceP = NULL;
  register LIce** iceA = sleafP->iceA.l;
  register int      tX = LPM_NIL;
  register int hmmX, stateX;

  /* --------------------------------------------- */
  /* early recombination step for unique words     */
  /* --------------------------------------------- */
  if (sleafP->info == WordIsUnique && spassP->streeP->svocabP->list.itemA[svX].fillerTag == 0)
    lct = ExtendLCT (spassP->lmlaP, lct, sleafP->vX.s);

  tX= sleafP->morphN-1;
  while (tX > -1 && iceA[tX]->lct != lct) tX--;
  if (tX == -1) 
    tX = LPM_NIL;

  /* --------------------------------------------- */
  /* case 1 : same lct from same predecessor       */
  /* --------------------------------------------- */

  /* due to the early path recombination, we have to check if this path
     (bpIdx,lct) correspond to an already existing instance. Therefore,
     we have to compare lct with original lct and consider also the fact
     the lct doesn't change by calling lctExtend but have in fact a different
     lm score
  */
  if (tX != LPM_NIL && lct == orgLCT && fabs(lms-iceA[tX]->lms) <1e-6) {
    iceP = iceA[tX];

    /* update token scores */
    for (hmmX=0;hmmX<hmmN;hmmX++) {
      NTok* path = iceP->path + (hmmX*stateN) +1;
      if (new_score < path->s) {
	path->s     = new_score;
	path->bpIdx = bpIdx;
      }
    }
#if USE_RCM_LM
    new_score += iceP->lmp[0];
#endif

    /* update instance score */
    if (iceP->score <= new_score) return;
    iceP->score = new_score;
    iceP->lms   = lms;
    iceP->svX   = svX;

    /* update best_score */
    if (new_score < iceA[sleafP->bestX]->score) {
      sleafP->bestX = tX;
    }

    /* update worst_score */
    if (sleafP->worstX == tX) {
      int      morphX  = sleafP->morphN -1;
      float worstscore = -SCORE_MAX;
      for (;morphX>=0;morphX--) {
	if (iceA[morphX]->score > worstscore) {
	  worstscore     = iceA[morphX]->score;
	  sleafP->worstX = morphX;
	}
      }
    }

    assert(iceA[sleafP->worstX]->score >= iceA[sleafP->bestX]->score);
    return;
  }

  /* --------------------------------------------- */
  /* case 1b : same lct but different predecessor  */
  /* --------------------------------------------- */
  if (tX != LPM_NIL && (lct != orgLCT || fabs(lms-iceA[tX]->lms) >=1e-6)) {
    iceP = iceA[tX];

    /* update local score */
    if (iceP->score <= new_score) return;
    iceP->score = new_score;
    iceP->lms   = lms;
    iceP->lct   = lct;
    iceP->svX   = svX;
    
    /* update token scores */
    for (hmmX=0;hmmX<hmmN;hmmX++) {
      NTok* path  = iceP->path + (hmmX*stateN) +1;
      path->s     = new_score;
      path->bpIdx = bpIdx;
    }

    /* update best score */
    if (new_score < iceA[sleafP->bestX]->score) {
      sleafP->bestX = tX;
    }      

    /* update worst score */
    if (sleafP->worstX == tX) {
      int       morphX = sleafP->morphN-1;
      float worstscore = -SCORE_MAX;
      for (;morphX>=0;morphX--) {
	if (iceA[morphX]->score > worstscore) {
	  worstscore     = iceA[morphX]->score;
	  sleafP->worstX = morphX;
	}
      }
    }
    assert(iceA[sleafP->worstX]->score >= iceA[sleafP->bestX]->score);
    return;
  }

  /* --------------------------------------------- */
  /* case 2a                                       */
  /* --------------------------------------------- */
  if (sleafP->morphN < sleafP->allocN) {
    tX = (sleafP->morphN)++;
    iceP        = iceA[tX];
    iceP->score = new_score;
    iceP->lms   = lms;
    iceP->lct   = lct;
    iceP->svX   = svX;
    for (hmmX=0;hmmX<hmmN;hmmX++) {
      NTok* path  = iceP->path + (hmmX*stateN) +1;
      path->s     = new_score;
      path->bpIdx = bpIdx;
    }

#if USE_RCM_LM 
    {
      /* The phone look-ahead is computed on the look-ahead
	 language model and therefore we should use the 
	 look-ahead lz */
      float       lz = spassP->streeP->svmapP->lalz;
      lmScore*   lmA = lmlaPhones(spassP->lmlaP,lct);
      float*    lmpA = iceP->lmp;
      int       mapX = 0;
      int       mapN = rcmP->mapN;
      
      /* extended lm lookahead for right context models */
      for (hmmX=0;hmmX<hmmN;hmmX++) {
	lmpA[hmmX]= SCORE_MAX;
      }
      for (mapX=0;mapX<mapN;mapX++) {
	float  s = lz * lmA[mapX];
	/* the best lm lookahead over all right context models
	   will be added for the best instance score to incoporate
	   this information during state pruning as well as
	   rcm pruning. However, the extended lm scores will not
	   be added to the token's score.
	*/
	if (s < lmpA[0]) 
	  lmpA[0]= s;
	hmmX = rcmP->mapXA[mapX];
	if (s < lmpA[hmmX]) 
	  lmpA[hmmX]= s;
      }
      new_score   += lmpA[0];
      iceP->score += lmpA[0];
    }
#endif

    if (sleafP->morphN == 1 || new_score <= iceA[sleafP->bestX]->score) {
      sleafP->bestX = tX;
    }
    if (sleafP->morphN == 1 || new_score >= iceA[sleafP->worstX]->score) {
      sleafP->worstX = tX;
    }
    assert(iceA[sleafP->worstX]->score >= iceA[sleafP->bestX]->score);
    return;
  }

  /* --------------------------------------------- */
  /* case 2b: we have a unseen instance and can    */
  /*          allocate new space                   */
  /* --------------------------------------------- */
  if (sleafP->allocN+1 <= spassP->beam.morphN) {
    sleafP->allocN++;
    smemAllocLeaf(sleafP,spassP->streeP);
    iceA = sleafP->iceA.l;
    tX = (sleafP->morphN)++;
    iceP        = iceA[tX];
    iceP->lct   = lct;
    iceP->score = new_score;
    iceP->lms   = lms;
    iceP->svX   = svX;
    for (hmmX=0;hmmX<hmmN;hmmX++) {
      NTok* path  = iceP->path + (hmmX*stateN) +1;
      path->s     = new_score;
      path->bpIdx = bpIdx;
    }

#if USE_RCM_LM 
    {
      /* The phone look-ahead is computed on the look-ahead
	 language model and therefore we should use the 
	 look-ahead lz */
      float       lz = spassP->streeP->svmapP->lalz;
      lmScore*   lmA = lmlaPhones(spassP->lmlaP,lct);
      float*    lmpA = iceP->lmp;
      int       mapX = 0;
      int       mapN = rcmP->mapN;
      
      /* extended lm lookahead for right context models */
      for (hmmX=0;hmmX<hmmN;hmmX++) {
	lmpA[hmmX]= SCORE_MAX;
      }
      for (mapX=0;mapX<mapN;mapX++) {
	float  s = lz * lmA[mapX];
	if (s < lmpA[0]) 
	  lmpA[0]= s;
	hmmX = rcmP->mapXA[mapX];
	if (s < lmpA[hmmX]) 
	  lmpA[hmmX]= s;
      }
      new_score   += lmpA[0];
      iceP->score += lmpA[0];
    }
#endif

    if (sleafP->morphN == 1 || new_score < iceA[sleafP->bestX]->score) {
      sleafP->bestX = tX;
    }
    if (sleafP->morphN == 1 || new_score > iceA[sleafP->worstX]->score) {
      sleafP->worstX = tX;
    }
    assert(iceA[sleafP->worstX]->score >= iceA[sleafP->bestX]->score);
    return;
  }

  /* --------------------------------------------- */
  /* case 2c: we have a unseen instance and no     */
  /*          space anymore, delete the worst      */
  /*          instance                             */
  /* --------------------------------------------- */
  if (new_score < iceA[sleafP->worstX]->score) {
    float worstscore = -SCORE_MAX;

#if USE_RCM_LM 
    {
      /* This a little bit ugly and should be cleaned sometime.
	 Before we replace a instance we have to compare the 
	 score including the extended lm score.
      */
      /* The phone look-ahead is computed on the look-ahead
	 language model and therefore we should use the 
	 look-ahead lz */
      float       lz = spassP->streeP->svmapP->lalz;
      lmScore*   lmA = lmlaPhones(spassP->lmlaP,lct);
      int       mapX = 0;
      int       mapN = rcmP->mapN;
      float   bestlm = SCORE_MAX;
      for (mapX=0;mapX<mapN;mapX++) {
	float  s = lz * lmA[mapX];
	if (s < bestlm) 
	  bestlm= s;
      }
      if (new_score +bestlm <= iceA[sleafP->worstX]->score) return;
    }
#endif

    tX = sleafP->worstX;
    assert(tX < sleafP->morphN);
    iceP        = sleafP->iceA.l[tX];
    iceP->lct   = lct;
    iceP->score = new_score;
    iceP->lms   = lms;
    iceP->svX   = svX;
    /* deactivate all states of this instance */
    for (stateX=0;stateX<hmmN*stateN;stateX++) {
      iceP->path[stateX].s = SCORE_MAX;
    }
    
    /* activate first state for each right context */
    for (hmmX=0;hmmX<hmmN;hmmX++) {
      NTok* path  = iceP->path + (hmmX*stateN) +1;
      path->s     = new_score;
      path->bpIdx = bpIdx;
    }
    
#if USE_RCM_LM 
    {
      /* The phone look-ahead is computed on the look-ahead
	 language model and therefore we should use the 
	 look-ahead lz */
      float       lz = spassP->streeP->svmapP->lalz;
      lmScore*   lmA = lmlaPhones(spassP->lmlaP,lct);
      float*    lmpA = iceP->lmp;
      int       mapX = 0;
      int       mapN = rcmP->mapN;
      
      for (hmmX=0;hmmX<hmmN;hmmX++) {
	lmpA[hmmX]= SCORE_MAX;
      }
      for (mapX=0;mapX<mapN;mapX++) {
	float  s = lz * lmA[mapX];
	if (s < lmpA[0]) 
	  lmpA[0]= s;
	hmmX = rcmP->mapXA[mapX];
	if (s < lmpA[hmmX]) 
	  lmpA[hmmX]= s;
      }
      new_score   += lmpA[0];
      iceP->score += lmpA[0];
    }
#endif
    
    if (new_score < iceA[sleafP->bestX]->score) {
      sleafP->bestX = tX;
    }
    
    /* update worst_score */
    for (tX = sleafP->morphN-1;tX >=0;tX--) {
      if (iceA[tX]->score > worstscore) {
	worstscore     = iceA[tX]->score;
	sleafP->worstX = tX;
      }
    }
  }
  assert(iceA[sleafP->worstX]->score >= iceA[sleafP->bestX]->score);
  return;
}


/* ========================================================================
    SPASS viterbi routines : propagate tokens into next HMM state
   ======================================================================== */

/* ------------------------------------------------------------------------
    state transactions for Roots
   ------------------------------------------------------------------------ */

static float spassVitState_SRoot (SPass* spassP)
{
  int         rootX = 0;
  int         rootN = spassP->active.rootN;
  SRoot**    rootPA = spassP->active.rootPA;
  float*     scoreA = spassP->senone.scoreA;
  PHMM*  hmmOffsetP = spassP->hmmP;
  float       best  = SCORE_MAX; 
  float       score = 0.0;

  for (rootX=0;rootX<rootN;rootX++,rootPA++) {
    SRoot* srootP = *rootPA;
    LCM*     lcmP = spassP->lcmP + srootP->xhmmX;
    PHMM*    hmmP = hmmOffsetP + lcmP->hmmXA[0];

    if (! lmlaTimeConstraint_LNX(spassP->lmlaP,spassP->frameX,srootP->lnX)) {
      srootDeactivate(srootP,spassP->streeP);
      continue;
    }

    score = hmmP->vitFn.e0(scoreA,srootP,hmmOffsetP);
    if (score < best) 
      best = score;
  }
  return best;
}

/* ------------------------------------------------------------------------
    state transactions for Nodes
   ------------------------------------------------------------------------ */

static float spassVitState_SNode (SPass* spassP)
{
  int          nodeX = 0;
  int          nodeN = spassP->active.nodeN;
  SNode**     nodePA = spassP->active.nodePA;
  float*      scoreA = spassP->senone.scoreA;
  PHMM*   hmmOffsetP = spassP->hmmP;
  float         best = SCORE_MAX; 
  float        score = 0.0;

  for (nodeX=0;nodeX<nodeN;nodeX++) {
    SNode* snodeP = nodePA[nodeX];
    PHMM*    hmmP = hmmOffsetP +snodeP->X.hmm;

    if (snodeP->info == WordIsUnique  &&
        ! lmlaTimeConstraint_SVX(spassP->lmlaP,spassP->frameX,snodeP->vX.s)) {
      snodeDeactivate(snodeP,spassP->streeP);
      continue;
    }
    if (snodeP->info == WordIsNotUnique  &&
        ! lmlaTimeConstraint_LNX(spassP->lmlaP,spassP->frameX,snodeP->vX.l)) {
	snodeDeactivate(snodeP,spassP->streeP);
      continue;
    }

    score = hmmP->vitFn.e1(hmmP,scoreA, snodeP);
    if (score < best) 
      best = score;
  }
  return best;
}

/* ------------------------------------------------------------------------
    state transactions for Leafs
   ------------------------------------------------------------------------ */

static float spassVitState_SLeaf (SPass* spassP)
{
  int         leafX = 0;
  int         leafN = spassP->active.leafN;
  SNode**    leafPA = spassP->active.leafPA;
  float*     scoreA = spassP->senone.scoreA;
  PHMM*  hmmOffsetP = spassP->hmmP;
  float       best  = SCORE_MAX; 
  float       score = 0.0;

  for (leafX=0;leafX<leafN;leafX++) {
    SNode* sleafP = leafPA[leafX];
    RCM*     rcmP = spassP->rcmP + sleafP->X.xhmm;
    PHMM*    hmmP = hmmOffsetP + rcmP->hmmXA[0];

    if (sleafP->info == WordIsUnique  &&
        ! lmlaTimeConstraint_SVX(spassP->lmlaP,spassP->frameX,sleafP->vX.s)) {
	sleafDeactivate(sleafP,spassP->streeP);
      continue;
    }
    if (sleafP->info == WordIsNotUnique  &&
        ! lmlaTimeConstraint_LNX(spassP->lmlaP,spassP->frameX,sleafP->vX.l)) {
      sleafDeactivate(sleafP,spassP->streeP);
      continue;
    }

    score = hmmP->vitFn.e2(rcmP,scoreA, sleafP, hmmOffsetP);
    assert(sleafP->iceA.l[sleafP->worstX]->score >= sleafP->iceA.l[sleafP->bestX]->score);
    if (score < best) 
      best = score;
  }
  return best;
}

/* ------------------------------------------------------------------------
    state transactions for SIPhones
   ------------------------------------------------------------------------ */

static float spassVitState_SIPhone (SPass* spassP)
{
  int          sipX = 0;
  int          sipN = spassP->streeP->sTbl.iN;
  SIPhone*     sipP = spassP->streeP->sTbl.iA;
  float*     scoreA = spassP->senone.scoreA;
  PHMM*  hmmOffsetP = spassP->hmmP;
  float      thresh = spassP->beam.best + spassP->beam.state;
  float       best  = SCORE_MAX; 
  float       score = 0.0;

  for (sipX=0;sipX<sipN;sipX++,sipP++) {
    if (sipP->morphN > 0 && sipP->iceA[sipP->bestX]->score < thresh) {
      PHMM* hmmP  = hmmOffsetP + sipP->hmmX;

      if (! lmlaTimeConstraint_SVX(spassP->lmlaP,spassP->frameX,sipP->svX)) {
	siphoneDeactivate(sipP,spassP->streeP);
	continue;
      }

      score = hmmP->vitFn.e3(hmmP,scoreA,sipP);
      if (score < best) 
	best = score;
    }
  }
  return best;
}

/* ------------------------------------------------------------------------
    state transactions for SDPhones 
   ------------------------------------------------------------------------ */

static float spassVitState_SDPhone (SPass* spassP)
{
  int          sdpX = 0;
  int          sdpN = spassP->streeP->sTbl.dN;
  SDPhone*     sdpP = spassP->streeP->sTbl.dA;
  float*     scoreA = spassP->senone.scoreA;
  PHMM*  hmmOffsetP = spassP->hmmP;
  float      thresh = spassP->beam.best + spassP->beam.state;
  float       best  = SCORE_MAX; 

  for (sdpX=0;sdpX<sdpN;sdpX++,sdpP++) {
    if (sdpP->morphN > 0 && sdpP->iceA[sdpP->bestX]->score < thresh) {
      XCM*   xcmP = spassP->xcmP+sdpP->hmmX;
      PHMM*  hmmP = hmmOffsetP+xcmP->hmmXA[0][0];
      float score = hmmP->vitFn.e4(xcmP,scoreA,sdpP,hmmOffsetP);

      if (! lmlaTimeConstraint_SVX(spassP->lmlaP,spassP->frameX,sdpP->svX)) {
	sdphoneDeactivate(sdpP,spassP->streeP);
	continue;
      }
      
      if (score < best) 
	best = score;
    }
  }
  return best;
}

/* ========================================================================
    SPASS viterbi routines : propagate tokens into next phone
   ======================================================================== */

/* ------------------------------------------------------------------------
    phone transactions for Roots
   ------------------------------------------------------------------------ */

static void spassVitPhone_SRoot (SPass* spassP)
{
  STree*     streeP = spassP->streeP;
  SWord*     swordP = streeP->svocabP->list.itemA;
  SMem*       smemP = & (streeP->smem);
  int         rootX = 0;
  int         rootN = spassP->active.rootN;
  SRoot**    rootPA = spassP->active.rootPA;
  int        morphX = 0;
  int        morphN = 0;
  float statethresh = spassP->beam.best + spassP->beam.state;
  float  wordthresh = spassP->beam.best + spassP->beam.word;
  float        lalz = streeP->svmapP->lalz;
  float          lz = streeP->svmapP->lz;
  int    useSvmapLM = (spassP->lmlaP->mode == Single_Mode);
  LVX      startLVX = streeP->svmapP->idx.start;

  /* --------------------------------------------- */
  /* 1. for each active root                       */
  /* --------------------------------------------- */
  
  for (rootX=0;rootX<rootN;rootX++) {
    SRoot* srootP = rootPA[rootX];
    int    childN = srootP->childN-1;
    int   needLA = 0;
    int   needEA = 0;

    if (srootP->morphN == 0 || srootP->iceA[srootP->bestX]->score >= statethresh) continue;

    while ((!needLA || !needEA) && childN>=0) {
      SNode* childP = srootP->childPA[childN];
      if (childP->info == WordIsUnique) 
	needEA = 1;
      if (childP->info == WordIsNotUnique && childP->vX.l != LTREE_NULL_NODE)
	needLA =1;
      childN--;
    }
        
    /* --------------------------------------------- */
    /* 2. for each linguistic morphed root instance  */
    /* --------------------------------------------- */
      
    morphN = srootP->morphN;
    for (morphX=0;morphX<morphN;morphX++) {
      RIce* iceP   = srootP->iceA[morphX];
      float pscore = iceP->path[0].s + streeP->svmapP->penalty.phone;      
      lmScore* lmLaScore    = NULL;
      lmScore* lmExactScore = NULL;
      
      /* --------------------------------------------- */
      /* pscore : final score from root token plus     */
      /*          phone insertion penalty              */
      /* --------------------------------------------- */

      if (pscore >= statethresh) continue;

      if (needLA) {
	if (needEA && ! useSvmapLM)
	  lmlaFillCtx(spassP->lmlaP,iceP->lct,&lmLaScore,&lmExactScore);
	else
	  lmlaFillCtx(spassP->lmlaP,iceP->lct,&lmLaScore,NULL);
      } else {
	if (needEA && ! useSvmapLM)
	  lmlaFillCtx(spassP->lmlaP,iceP->lct,NULL,&lmExactScore);
      }

      /* --------------------------------------------- */
      /* 3. for all children of this root              */
      /* --------------------------------------------- */

      childN = srootP->childN-1;
      while (childN>=0) {
	SNode* childP = srootP->childPA[childN];
	float     lms = iceP->lms;
	float  wscore = pscore-lms;
	float  fscore = spassP->fmatch.s[childP->phoneX];
	SVX       svX = SVX_NIL;

	if (childP->info == WordIsUnique  &&
	    ! lmlaTimeConstraint_SVX(spassP->lmlaP,spassP->frameX,childP->vX.s)) {
	  if (childP->childN >0)  
	    snodeDeactivate(childP,spassP->streeP);
	  else
	    sleafDeactivate(childP,spassP->streeP);
	  childN--;
	  continue;
	}
	if (childP->info == WordIsNotUnique  &&
	     ! lmlaTimeConstraint_LNX(spassP->lmlaP,spassP->frameX,childP->vX.l)) {
	  if (childP->childN >0)  
	    snodeDeactivate(childP,spassP->streeP);
	  else
	    sleafDeactivate(childP,spassP->streeP);
	  childN--;
	  continue;
	}

	if (childP->info == WordIsUnique) 
	  svX = childP->vX.s;
	if (childP->info == WordWasUnique)
	  svX = srootP->lnX;

	/* ---------------------------------------------- */
	/* wscore : final score from root token plus      */
	/*          phone insertion penalty plus lm score */
	/*                                                */
	/* If the word is not yet unique and the number   */
	/* of children of this root is more than one, we  */
	/* can update the lm lookahead delta score.       */
	/*                                                */
	/* If the word is unique, we add the exact lm     */
	/* score (delta to the lm lookahead).             */
	/* ---------------------------------------------- */

	if (childP->info == WordIsNotUnique && childP->vX.l != LTREE_NULL_NODE) {
	  lms = lmLaScore[childP->vX.l];
	  if (lms == LMSCORE_MAX) 
	    lms = SCORE_MAX; 
	  else 
	    lms *= lalz;
	}
	if (childP->info == WordIsUnique) {
	  if (startLVX == svmapGet(streeP->svmapP,svX)) {
	    if (iceP->path[0].bpIdx == -1) 
	      lms = lz*streeP->svmapP->lvxSA[childP->vX.s];
	    else
	      lms = SCORE_MAX;
	  } else {
	    if (swordP[svX].fillerTag == 1) {
	      lms = lz*streeP->svmapP->lvxSA[childP->vX.s];
	      lms += streeP->svmapP->penalty.fil;
	      lms -= streeP->svmapP->penalty.word;
	    } else {
	      if (useSvmapLM) 
		lms = GetOneLMScore(spassP->lmlaP,iceP->lct,childP->vX.s);
	      else
		lms = lmExactScore[childP->vX.s];
	      if (lms == LMSCORE_MAX) 
		lms = SCORE_MAX; 
	      else 
		lms *= lz;
	    }
	  }
	}
	wscore+=lms;

	if (childP->childN >0) {
	  float  cscore = (childP->morphN >0) ? childP->iceA.n[childP->bestX]->score : SCORE_MAX;
	  if  (wscore+fscore >= statethresh || 
	       wscore        >= cscore+spassP->beam.morph) {
	    childN--;
	    continue;
	  }
	} else {
	  float  cscore = (childP->morphN >0) ? childP->iceA.l[childP->bestX]->score : SCORE_MAX;
	  if (wscore+fscore >= wordthresh || 
	      wscore        >= cscore+spassP->beam.morph) {
	    childN--;
	    continue;
	  }	
	}

	/* --------------------------------------------- */
	/* 4. add this instance to the child heap        */
	/*    Due initial allocation dependencies do:    */
	/*    - alloc triple map for heap position       */	    
	/*    - alloc child instances                    */    
	/* --------------------------------------------- */
	if (childP->childN >0) {
	  if (childP->allocN == 0) {
	    childP->allocN     = smemP->morphBlkN;
	    smemAllocNode(childP,streeP);
	  }
	  if (childP->morphN == 0) {
	    spassP->actNext.nodePA[spassP->actNext.nodeN++] = childP;
	  }
	  spassAddIce_SNode(spassP,childP,iceP->lct,svX,morphX,
			    wscore,lms,iceP->path[0].bpIdx);
	} else {
	  if (childP->allocN == 0) {
	    childP->allocN  = smemP->morphBlkN;
	    smemAllocLeaf(childP,streeP);
	  }
	  if (childP->morphN == 0) {
	    spassP->actNext.leafPA[spassP->actNext.leafN++] = childP;
	  }
	  spassAddIce_SLeaf(spassP,childP,iceP->lct,svX,(LPM)morphX,
			    wscore,lms,iceP->path[0].bpIdx);
	}
	childN--;
      } /* for each child */
    } /* for each instance */
  } /* for each active root */
  return;
}

/* ------------------------------------------------------------------------
    phone transactions for Nodes
   ------------------------------------------------------------------------ */

static void spassVitPhone_SNode (SPass* spassP)
{
  STree*     streeP = spassP->streeP;
  SWord*     swordP = streeP->svocabP->list.itemA;
  SMem*       smemP = & (streeP->smem);
  int         nodeX = 0;
  int         nodeN = spassP->active.nodeN;
  SNode**    nodePA = spassP->active.nodePA;
  int        morphX = 0;
  int        morphN = 0;
  float statethresh = spassP->beam.best + spassP->beam.state;
  float  wordthresh = spassP->beam.best + spassP->beam.word;
  float        lalz = streeP->svmapP->lalz;
  float          lz = streeP->svmapP->lz;
  int    useSvmapLM = (spassP->lmlaP->mode == Single_Mode);
  LVX      startLVX = streeP->svmapP->idx.start;

  /* --------------------------------------------- */
  /* 1. for each active node                       */
  /* --------------------------------------------- */
  
  for (nodeX=0;nodeX<nodeN;nodeX++) {
    SNode* snodeP = nodePA[nodeX];
    int    childN = snodeP->childN-1;
    int    needLA = 0;
    int    needEA = 0;

    if (snodeP->morphN == 0 || snodeP->iceA.n[snodeP->bestX]->score >= statethresh) continue;

    while ((!needLA || !needEA) && childN>=0) {
      SNode* childP = snodeP->childPA[childN];
      if (childP->info == WordIsUnique) needEA = 1;
      if (childP->info == WordIsNotUnique && childP->vX.l != LTREE_NULL_NODE) 
	needLA =1;
      childN--;
    }

    /* --------------------------------------------- */
    /* 2. for each linguistic morphed node instance  */
    /* --------------------------------------------- */
    
    morphN = snodeP->morphN;
    for (morphX=0;morphX<morphN;morphX++) {
      NIce* iceP   = snodeP->iceA.n[morphX];
      float pscore = iceP->path[0].s + streeP->svmapP->penalty.phone;      
      lmScore *lmLaScore, *lmExactScore;
      
      /* --------------------------------------------- */
      /* pscore : final score from node token plus     */
      /*          phone insertion penalty              */
      /* --------------------------------------------- */

      if (pscore >= statethresh) continue;

      if (needLA) {
	if (needEA && ! useSvmapLM)
	  lmlaFillCtx(spassP->lmlaP,iceP->lct,&lmLaScore,&lmExactScore);
	else
	  lmlaFillCtx(spassP->lmlaP,iceP->lct,&lmLaScore,NULL);
      } else {
	if (needEA && ! useSvmapLM)
	  lmlaFillCtx(spassP->lmlaP,iceP->lct,NULL,&lmExactScore);
      }
	
      /* --------------------------------------------- */
      /* 3. for all children of this node              */
      /* --------------------------------------------- */

      childN = snodeP->childN-1;
      while (childN>=0) {
	SNode* childP = snodeP->childPA[childN];
	float     lms = iceP->lms;
	float  wscore = pscore-lms;
	float  fscore = spassP->fmatch.s[childP->phoneX];
	SVX       svX = SVX_NIL;

	if (childP->info == WordIsUnique  &&
	    ! lmlaTimeConstraint_SVX(spassP->lmlaP,spassP->frameX,childP->vX.s)) {
	  if (childP->childN >0)  
	    snodeDeactivate(childP,spassP->streeP);
	  else
	    sleafDeactivate(childP,spassP->streeP);
	  childN--;
	  continue;
	}
	if (childP->info == WordIsNotUnique  &&
	     ! lmlaTimeConstraint_LNX(spassP->lmlaP,spassP->frameX,childP->vX.l)) {
	  if (childP->childN >0)  
	    snodeDeactivate(childP,spassP->streeP);
	  else
	    sleafDeactivate(childP,spassP->streeP);
	  childN--;
	  continue;
	}

	if (childP->info == WordIsUnique) 
	  svX = childP->vX.s;
	if (childP->info == WordWasUnique)
	  svX = iceP->svX;

	/* ---------------------------------------------- */
	/* wscore : final score from node token plus      */
	/*          phone insertion penalty plus lm score */
	/*                                                */
	/* If the word is not yet unique and the number   */
	/* of children of this node is more than one, we  */
	/* can update the lm lookahead delta score.       */
	/*                                                */
	/* If the word is unique, we add the exact lm     */
	/* score (delta to the lm lookahead).             */
	/* ---------------------------------------------- */

	if (childP->info == WordIsNotUnique && childP->vX.l != LTREE_NULL_NODE) {
	  lms = lmLaScore[childP->vX.l];
	  if (lms == LMSCORE_MAX) 
	    lms = SCORE_MAX; 
	  else 
	    lms *= lalz;
	}
	if (childP->info == WordIsUnique) {
	  if (startLVX == svmapGet(streeP->svmapP,svX)) {
	    if (iceP->path[0].bpIdx == -1) 
	      lms = lz*streeP->svmapP->lvxSA[childP->vX.s];
	    else
	      lms = SCORE_MAX;
	  } else {
	    if (swordP[svX].fillerTag == 1) {
	      lms = lz*streeP->svmapP->lvxSA[childP->vX.s];
	      lms += streeP->svmapP->penalty.fil;
	      lms -= streeP->svmapP->penalty.word;
	    } else {
	      if (useSvmapLM)
		lms = GetOneLMScore(spassP->lmlaP,iceP->lct,childP->vX.s);
	      else
		lms = lmExactScore[childP->vX.s];
	      if (lms == LMSCORE_MAX) 
		lms = SCORE_MAX; 
	      else 
		lms *= lz;
	    }
	  }
	}
	wscore += lms;

	if (childP->childN>0) {
	  float  cscore = (childP->morphN >0) ? childP->iceA.n[childP->bestX]->score : SCORE_MAX;
	  if (wscore+fscore >= statethresh || 
	      wscore        >= cscore+spassP->beam.morph) {
	    childN--;
	    continue;
	  }
	} else {
	  float  cscore = (childP->morphN >0) ? childP->iceA.l[childP->bestX]->score : SCORE_MAX;
	  if (wscore+fscore >= wordthresh || 
	      wscore        >= cscore+spassP->beam.morph) {
	    childN--;
	    continue;
	  }
	}

	/* --------------------------------------------- */
	/* 4. add this instance to the child heap        */
	/*    Due initial allocation dependencies do:    */
	/*    - alloc simple map for heap position       */	    
	/*    - alloc child instances                    */    
	/* --------------------------------------------- */

	/* case 1 : child is a SNode */
	if (childP->childN>0) {
	  if (childP->allocN == 0) {
	    childP->allocN     = smemP->morphBlkN;
	    smemAllocNode(childP,streeP);
	  }
	  if (childP->morphN == 0) {
	    spassP->actNext.nodePA[spassP->actNext.nodeN++] = childP;
	  }
	  spassAddIce_SNode(spassP,childP,iceP->lct,svX,morphX,
			    wscore,lms,iceP->path[0].bpIdx);	      
	}
	/* case 2 : child is a SLeaf */
	else {
	  if (childP->allocN == 0) {
	    childP->allocN     = smemP->morphBlkN;
	    smemAllocLeaf(childP,streeP);
	  }
	  if (childP->morphN == 0) {
	    spassP->actNext.leafPA[spassP->actNext.leafN++] = childP;
	  }
	  spassAddIce_SLeaf(spassP,childP,iceP->lct,svX,(LPM)morphX,
			    wscore,lms,iceP->path[0].bpIdx);
	}	 
	childN--;
      } /* for each child */
    } /* for each instance */
  } /* for each active node */
  return;
}

/* ========================================================================
    SPASS viterbi routines : propagate tokens into next word
   ======================================================================== */

/* ------------------------------------------------------------------------
    word transactions from leafs to backpointer table
   ------------------------------------------------------------------------ */

static void spassVitWord_SLeaf (SPass* spassP)
{
  STab*      bptabP = spassP->stabP;

  int         leafX = 0;
  int         leafN = spassP->active.leafN;
  SNode**    leafPA = spassP->active.leafPA;

  RCM*         rcmP = NULL;
  PHMM*        hmmP = NULL;
  int        morphX = 0;
  int        morphN = 0;
  float  wordthresh = spassP->beam.best + spassP->beam.word;

  int hmmX,hmmN,stateN;

  for (leafX=0;leafX<leafN;leafX++) {
    SNode* sleafP = leafPA[leafX];

    morphN = sleafP->morphN;
    rcmP   = spassP->rcmP + sleafP->X.xhmm;
    hmmP   = spassP->hmmP + rcmP->hmmXA[0];
    stateN = hmmP->stateN+1;
    hmmN   = stateN * rcmP->hmmN;

    if (sleafP->morphN == 0 || sleafP->iceA.l[sleafP->bestX]->score >= wordthresh) continue;

    for (morphX=0;morphX<morphN;morphX++) {
      register float score = SCORE_MAX;
      register int   bpIdx = 0;
      NTok* path = sleafP->iceA.l[morphX]->path;
      
      if (sleafP->iceA.l[morphX]->score >= wordthresh) continue;

      /* get best score for this instance */
      for (hmmX=0;hmmX<hmmN;hmmX+=stateN) {
	if (path[hmmX].s < score) {
	  score  = path[hmmX].s;
	  bpIdx  = path[hmmX].bpIdx;
	}
      }
      if (score < wordthresh) {
	BP* bp = stabAlloc(bptabP,spassP->frameX,sleafP->iceA.l[morphX]->lct);
	if (score < bp->score) {
	  bp->svX      = sleafP->iceA.l[morphX]->svX;
	  bp->score    = score;
	  bp->bpIdx    = bpIdx;
	  bp->morphX   = morphX;
	}
      }
    }
  }
  return;
}

/* ------------------------------------------------------------------------
    word transactions from single phone words to backpointer table
   ------------------------------------------------------------------------ */

static void spassVitWord_SIPhone (SPass* spassP)
{
  STree*     streeP = spassP->streeP;
  STab*      bptabP = spassP->stabP;
  int          sipX = 0;
  int          sipN = streeP->sTbl.iN;
  SIPhone*     sipP = streeP->sTbl.iA;
  int        morphX = 0;
  int        morphN = 0;
  int          ftag = 0;
  float  wordthresh = spassP->beam.best + spassP->beam.word;

  for (sipX=0;sipX<sipN;sipX++, sipP++) {
    morphN = sipP->morphN;

    if (morphN == 0 || sipP->iceA[sipP->bestX]->score >= wordthresh) continue;

    ftag = streeP->svmapP->svocabP->list.itemA[sipP->svX].fillerTag;
    for (morphX=0;morphX<morphN;morphX++) {
      NTok* path = sipP->iceA[morphX]->path;
      if (path[0].s < wordthresh) {
	LCT lct = (ftag) ? 
	  sipP->iceA[morphX]->lct :
	  ExtendLCT (spassP->lmlaP, sipP->iceA[morphX]->lct, sipP->svX);

	BP* bp  = stabAlloc(bptabP,spassP->frameX,lct);
	if (path[0].s < bp->score) {
	  bp->svX    = sipP->svX;
	  bp->score  = path[0].s;
	  bp->bpIdx  = path[0].bpIdx;
	  bp->morphX = morphX;
	}
      }
    } 
  }
  return;
}

/* ------------------------------------------------------------------------
    word transactions from context dependent single phone words 
   ------------------------------------------------------------------------ */

static void spassVitWord_SDPhone (SPass* spassP)
{
  STree*     streeP = spassP->streeP;
  STab*      bptabP = spassP->stabP;
  int          sdpN = streeP->sTbl.dN;
  SDPhone*     sdpP = streeP->sTbl.dA;
  float  wordthresh = spassP->beam.best + spassP->beam.word;
  int hmmX,sdpX,morphX;

  for (sdpX=0;sdpX<sdpN;sdpX++, sdpP++) {
    XCM*   xcmP = spassP->xcmP+sdpP->hmmX;
    PHMM*  hmmP = spassP->hmmP + xcmP->hmmXA[0][0];
    int  stateN = hmmP->stateN+1;
    int  morphN = sdpP->morphN;
    int    ftag = streeP->svmapP->svocabP->list.itemA[sdpP->svX].fillerTag;
    if (morphN == 0 || sdpP->iceA[sdpP->bestX]->score >= wordthresh) continue;

    for (morphX=0;morphX<morphN;morphX++) {
      int    hmmN = xcmP->hmmNA[sdpP->iceA[morphX]->phoneX];
      NTok*  path = sdpP->iceA[morphX]->path;
      float score = SCORE_MAX;
      int   bpIdx = -1;

      /* get best score for this instance */
      for (hmmX=0;hmmX<hmmN;hmmX++) {
	if (path[hmmX*stateN].s < score) {
	  score  = path[hmmX*stateN].s;
	  bpIdx  = path[hmmX*stateN].bpIdx;
	}
      }

      if (score < wordthresh) {
	LCT lct = (ftag) ? 
	  sdpP->iceA[morphX]->lct :
	  ExtendLCT (spassP->lmlaP, sdpP->iceA[morphX]->lct, sdpP->svX);
	BP* bp = stabAlloc(bptabP,spassP->frameX,lct);
	if (score < bp->score) {
	  bp->svX    = sdpP->svX;
	  bp->score  = score;
	  bp->bpIdx  = bpIdx;
	  bp->morphX = morphX;
	}
      }
    } 
  }
  return;
}

/* ------------------------------------------------------------------------
    prepare backpointer to start new root instances
   ------------------------------------------------------------------------ */

static void spassConvertBP (SPass* spassP)
{
  STree*     streeP = spassP->streeP;
  SVMap*     svmapP = streeP->svmapP;
  SWord*     swordP = svmapP->svocabP->list.itemA;

  /* starting roots */
  int         rootX = 0;
  int         rootN = streeP->rootTbl.N;
  SRoot**    rootPA = streeP->rootTbl.PA;

  /* starting single phones */
  int          sipX = 0;
  int          sipN = streeP->sTbl.iN;
  SIPhone*     sipA = streeP->sTbl.iA;
  int          sdpX = 0;
  int          sdpN = streeP->sTbl.dN;
  SDPhone*     sdpA = streeP->sTbl.dA;

  /* backpointer */
  BP*           bpA = spassP->stabP->A;
  int      bpOffset = spassP->stabP->useNA[spassP->frameX-1];
  int           bpX = 0;
  int           bpN = spassP->stabP->useNA[spassP->frameX];

  /* backpointer interface */
  LIceExt**  transA = spassP->trans.A;
  int*       transN = spassP->trans.N;
  int        phoneX = 0;
  int        phoneN = streeP->dictP->phones->list.itemN;

  float          lz = svmapP->lz;
  float        lalz = svmapP->lalz;
  float      thresh = spassP->beam.best + spassP->beam.state + spassP->beam.morph;
  LVX        endLVX = streeP->svmapP->idx.end;

  int    useSvmapLM = (spassP->lmlaP->mode == Single_Mode);

  lmScore *lmLaScore, *lmExactScore;


  /* For each starting monophone we need a list of sorted backpointers with 
     the correct (context depend) score. To handle same_pronunciation words, 
     we have to add the final lm score, which is not included in the token
     score of the final state as it is for normal words due the lm lookahead. 
     To avoid another lm call, we store the lm delta score for the 
     same_pronunciation_words in the spassP->trans.delta cache table.
     ATTENTION: phone and word penalties are included in this stage. If we do
                a transition into a filler word, we have to add the difference
		between filler and word penalty.
  */

  for (phoneX=0;phoneX<phoneN;phoneX++) {
    transN[phoneX] = 0;
  }

  for (bpX=bpOffset;bpX<bpN;bpX++) {
    BP*        bp = bpA + bpX;
    SVX       svX = bp->svX;
    float   delta = svmapP->penalty.phone + svmapP->penalty.word;
    int    morphX = bp->morphX;
    float*   lmsA = spassP->trans.lmscore[bpX-bpOffset];
    
    if (endLVX == svmapGet(svmapP,svX)) continue;

    if (streeP->nodeTbl.typeA[svX] == type_SLeaf) {
      SNode* sleafP = streeP->nodeTbl.A[svX];
      RCM*     rcmP = spassP->rcmP + sleafP->X.xhmm;
      PHMM*    hmmP = spassP->hmmP + rcmP->hmmXA[0];
      int    stateN = hmmP->stateN + 1;
      NTok*    path = sleafP->iceA.l[morphX]->path;
      LCT       lct = sleafP->iceA.l[morphX]->lct;

      /* fill lmscore for word transitions for starting roots and sp's*/
      if (useSvmapLM)
	lmlaFillCtx(spassP->lmlaP,lct,&lmLaScore,NULL);
      else
	lmlaFillCtx(spassP->lmlaP,lct,&lmLaScore,&lmExactScore);
      for (rootX=0;rootX<rootN;rootX++) {
	lmsA[rootX] = lmLaScore[rootPA[rootX]->lnX];
	if (lmsA[rootX] == LMSCORE_MAX)
	  lmsA[rootX] = SCORE_MAX;
	else
	  lmsA[rootX] *= lalz; 
      }
      for (sipX=0;sipX<sipN;sipX++) {
	if (swordP[ sipA[sipX].svX ].fillerTag == 0) {
	  if (useSvmapLM)
	    lmsA[rootX+sipX] = GetOneLMScore(spassP->lmlaP,lct,sipA[sipX].svX);
	  else
	    lmsA[rootX+sipX] = lmExactScore[sipA[sipX].svX];
	  if (lmsA[rootX+sipX] == LMSCORE_MAX)
	    lmsA[rootX+sipX] = SCORE_MAX;
	  else
	    lmsA[rootX+sipX] *= lz; 
	}
      }
      for (sdpX=0;sdpX<sdpN;sdpX++) {
	if (swordP[ sdpA[sdpX].svX ].fillerTag == 0) {
	  if (useSvmapLM)
	    lmsA[rootX+sipX+sdpX] = GetOneLMScore(spassP->lmlaP,lct,sdpA[sdpX].svX);
	  else
	    lmsA[rootX+sipX+sdpX] = lmExactScore[sdpA[sdpX].svX];
	  if (lmsA[rootX+sipX+sdpX] == LMSCORE_MAX)
	    lmsA[rootX+sipX+sdpX] = SCORE_MAX;
	  else
	    lmsA[rootX+sipX+sdpX] *= lz; 
	}
      }
      for (phoneX=0;phoneX<phoneN;phoneX++) {
	int    hmmX = rcmP->mapXA[phoneX];
	float score = path[hmmX*stateN].s + delta;
	if (score < thresh) {
	  transA[phoneX][transN[phoneX]].score  = score;
	  transA[phoneX][transN[phoneX]].bpIdx  = bpX;
	  transA[phoneX][transN[phoneX]].lct    = lct;
	  transA[phoneX][transN[phoneX]].phoneX = sleafP->phoneX;
	  transN[phoneX]++;
	}
      }
      continue;
    }
    if (streeP->nodeTbl.typeA[svX] == type_SIPhone) {
      SIPhone* sipP = streeP->nodeTbl.A[svX];
      float   score = sipP->iceA[morphX]->path[0].s + delta;
      LCT       lct = sipP->iceA[morphX]->lct;

      if (swordP[sipP->svX].fillerTag == 0)
	lct = ExtendLCT (spassP->lmlaP, lct, sipP->svX);

      /* fill lm score for word transitions for starting roots and sp's */
      if (useSvmapLM)
	lmlaFillCtx(spassP->lmlaP,lct,&lmLaScore,NULL);
      else
	lmlaFillCtx(spassP->lmlaP,lct,&lmLaScore,&lmExactScore);

      for (rootX=0;rootX<rootN;rootX++) {
	lmsA[rootX] = lmLaScore[rootPA[rootX]->lnX];
	if (lmsA[rootX] == LMSCORE_MAX)
	  lmsA[rootX] = SCORE_MAX;
	else
	  lmsA[rootX] *= lalz; 
      }
      for (sipX=0;sipX<sipN;sipX++) {
	if (swordP[ sipA[sipX].svX ].fillerTag == 0) {
	  if (useSvmapLM)
	    lmsA[rootX+sipX] = GetOneLMScore(spassP->lmlaP,lct,sipA[sipX].svX);
	  else
	    lmsA[rootX+sipX] = lmExactScore[sipA[sipX].svX];
	  if (lmsA[rootX+sipX] == LMSCORE_MAX)
	    lmsA[rootX+sipX] = SCORE_MAX;
	  else
	    lmsA[rootX+sipX] *= lz; 
	}
      }
      for (sdpX=0;sdpX<sdpN;sdpX++) {
	if (swordP[ sdpA[sdpX].svX ].fillerTag == 0) {
	  if (useSvmapLM)
	    lmsA[rootX+sipX+sdpX] =  GetOneLMScore(spassP->lmlaP,lct,sdpA[sdpX].svX);
	  else
	    lmsA[rootX+sipX+sdpX] = lmExactScore[sdpA[sdpX].svX];
	  if (lmsA[rootX+sipX+sdpX] == LMSCORE_MAX)
	    lmsA[rootX+sipX+sdpX] = SCORE_MAX;
	  else
	    lmsA[rootX+sipX+sdpX] *= lz; 
	}
      }

      if (score < thresh) {
	for (phoneX=0;phoneX<phoneN;phoneX++) {
	  transA[phoneX][transN[phoneX]].score = score;
	  transA[phoneX][transN[phoneX]].bpIdx = bpX;
	  transA[phoneX][transN[phoneX]].lct   = lct;
	  transA[phoneX][transN[phoneX]].phoneX = sipP->phoneX;
	  transN[phoneX]++;
	}
      }
      continue;
    }
    if (streeP->nodeTbl.typeA[svX] == type_SDPhone) {
      SDPhone* sdpP = streeP->nodeTbl.A[svX];
      XCM*     xcmP = spassP->xcmP + sdpP->hmmX;
      PHMM*    hmmP = spassP->hmmP + xcmP->hmmXA[0][0];
      int    stateN = hmmP->stateN + 1;
      NTok*    path = sdpP->iceA[morphX]->path;
      LCT       lct = sdpP->iceA[morphX]->lct;
      int    phoneL = sdpP->iceA[morphX]->phoneX;

      if (swordP[sdpP->svX].fillerTag == 0)
	lct = ExtendLCT (spassP->lmlaP, lct, sdpP->svX);

      /* fill lmscore for word transitions for starting roots and sp's*/
      if (useSvmapLM)
	lmlaFillCtx(spassP->lmlaP,lct,&lmLaScore,NULL);
      else
	lmlaFillCtx(spassP->lmlaP,lct,&lmLaScore,&lmExactScore);

      for (rootX=0;rootX<rootN;rootX++) {
	lmsA[rootX] = lmLaScore[rootPA[rootX]->lnX];
	if (lmsA[rootX] == LMSCORE_MAX)
	  lmsA[rootX] = SCORE_MAX;
	else
	  lmsA[rootX] *= lalz; 
      }
      for (sipX=0;sipX<sipN;sipX++) {
	if (swordP[ sipA[sipX].svX ].fillerTag == 0) {
	  if (useSvmapLM)
	    lmsA[rootX+sipX] = GetOneLMScore(spassP->lmlaP,lct,sipA[sipX].svX);
	  else
	    lmsA[rootX+sipX] = lmExactScore[sipA[sipX].svX];
	  if (lmsA[rootX+sipX] == LMSCORE_MAX)
	    lmsA[rootX+sipX] = SCORE_MAX;
	  else
	    lmsA[rootX+sipX] *= lz; 
	}
      }
      for (sdpX=0;sdpX<sdpN;sdpX++) {
	if (swordP[ sdpA[sdpX].svX ].fillerTag == 0) {
	  if (useSvmapLM)
	    lmsA[rootX+sipX+sdpX] = GetOneLMScore(spassP->lmlaP,lct,sdpA[sdpX].svX);
	  else
	    lmsA[rootX+sipX+sdpX] = lmExactScore[sdpA[sdpX].svX];
	  if (lmsA[rootX+sipX+sdpX] == LMSCORE_MAX)
	    lmsA[rootX+sipX+sdpX] = SCORE_MAX;
	  else
	    lmsA[rootX+sipX+sdpX] *= lz; 
	}
      }

      for (phoneX=0;phoneX<phoneN;phoneX++) {
	int    hmmX = xcmP->mapXA[phoneL][phoneX];
	float score = path[hmmX*stateN].s + delta;
	if (score < thresh) {
	  transA[phoneX][transN[phoneX]].score  = score;
	  transA[phoneX][transN[phoneX]].bpIdx  = bpX;
	  transA[phoneX][transN[phoneX]].lct    = lct;
	  transA[phoneX][transN[phoneX]].phoneX = sdpP->phoneX;
	  transN[phoneX]++;
	}
      }
      continue;
    }
    ERROR("spassConvertBP: cannot handle node type for word %d\n",svX);
  }

  return;
}

/* ------------------------------------------------------------------------
    prepare backpointer to start new root instances without context info
   ------------------------------------------------------------------------ */

static void spassConvertBP_CI (SPass* spassP)
{
  STree*     streeP = spassP->streeP;
  SWord*     swordP = streeP->svocabP->list.itemA;
  SVMap*     svmapP = streeP->svmapP;

  /* starting roots */
  int         rootX = 0;
  int         rootN = streeP->rootTbl.N;
  SRoot**    rootPA = streeP->rootTbl.PA;

  /* starting single phones */
  int          sipX = 0;
  int          sipN = streeP->sTbl.iN;
  SIPhone*     sipA = streeP->sTbl.iA;
  int          sdpX = 0;
  int          sdpN = streeP->sTbl.dN;
  SDPhone*     sdpA = streeP->sTbl.dA;

  /* backpointer */
  BP*           bpA = spassP->stabP->A;
  int      bpOffset = spassP->stabP->useNA[spassP->frameX-1];
  int           bpX = 0;
  int           bpN = spassP->stabP->useNA[spassP->frameX];

  /* backpointer interface */
  LIceExt**  transA = spassP->trans.A;
  int*       transN = spassP->trans.N;
  int        phoneX = 0;
  int        phoneN = streeP->dictP->phones->list.itemN;

  float          lz = svmapP->lz;
  float        lalz = svmapP->lalz;
  float      thresh = spassP->beam.best + spassP->beam.state + spassP->beam.morph;
  LVX        endLVX = streeP->svmapP->idx.end;

  int    useSvmapLM = (spassP->lmlaP->mode == Single_Mode);

  lmScore *lmLaScore, *lmExactScore;

  for (phoneX=0;phoneX<phoneN;phoneX++) {
    transN[phoneX] = 0;
  }

  for (bpX=bpOffset;bpX<bpN;bpX++) {
    BP*        bp = bpA + bpX;
    SVX       svX = bp->svX;
    LCT       lct = LCT_NIL;
    SVX      svx0 = (SVX) spassP->streeP->svmapP->idx.start;
    SVX      svx1 = svx0;
    SVX      svx2 = svx0;
    int    phoneY = -1;
    float   score = bp->score + svmapP->penalty.phone + svmapP->penalty.word;
    float*   lmsA = spassP->trans.lmscore[bpX-bpOffset];
    
    if (endLVX == svmapGet(svmapP,svX)) continue;

    if (streeP->nodeTbl.typeA[svX] == type_SLeaf) {
      phoneY = ((SNode*) streeP->nodeTbl.A[svX])->phoneX;
    }
    if (streeP->nodeTbl.typeA[svX] == type_SIPhone) {
      phoneY = ((SIPhone*) streeP->nodeTbl.A[svX])->phoneX;
    }
    if (streeP->nodeTbl.typeA[svX] == type_SDPhone) {
      phoneY = ((SDPhone*) streeP->nodeTbl.A[svX])->phoneX;
    }
    if (phoneY == -1) {
      ERROR("spassConvertBP_CI: cannot handle node type for word %d\n",svX);
      continue;
    }

    /* find lct by backtracking */
    while (bp->bpIdx > -1 && swordP[svX].fillerTag) {
      bp  = bpA + bp->bpIdx;
      svX = bp->svX;
    }
    if (bp->bpIdx > -1) { 
      svx2 = svX;
      bp   = bpA + bp->bpIdx;
      svX  = bp->svX;
    }
    while (bp->bpIdx > -1 && swordP[svX].fillerTag) {
      bp  = bpA + bp->bpIdx;
      svX = bp->svX;
    }
    if (bp->bpIdx > -1)
      svx1 = svX;
    lct = CreateLCT (spassP->lmlaP,      svx0);
    lct = ExtendLCT (spassP->lmlaP, lct, svx1);
    lct = ExtendLCT (spassP->lmlaP, lct, svx2);

    /* fill lmscore for word transitions for starting roots and sp's*/
    if (useSvmapLM)
      lmlaFillCtx(spassP->lmlaP,lct,&lmLaScore,NULL);
    else
      lmlaFillCtx(spassP->lmlaP,lct,&lmLaScore,&lmExactScore);

    for (rootX=0;rootX<rootN;rootX++) {
      lmsA[rootX] = lmLaScore[rootPA[rootX]->lnX];
      if (lmsA[rootX] == LMSCORE_MAX)
	lmsA[rootX] = SCORE_MAX;
      else
	lmsA[rootX] *= lalz; 
    }
    for (sipX=0;sipX<sipN;sipX++) {
      if (swordP[ sipA[sipX].svX ].fillerTag == 0) {
	if (useSvmapLM)
	  lmsA[rootX+sipX] = GetOneLMScore(spassP->lmlaP,lct,sipA[sipX].svX);
	else
	  lmsA[rootX+sipX] = lmExactScore[sipA[sipX].svX];
	if (lmsA[rootX+sipX] == LMSCORE_MAX)
	  lmsA[rootX+sipX] = SCORE_MAX;
	else
	  lmsA[rootX+sipX] *= lz; 
      }
    }
    for (sdpX=0;sdpX<sdpN;sdpX++) {
      if (swordP[ sdpA[sdpX].svX ].fillerTag == 0) {
	if (useSvmapLM)
	  lmsA[rootX+sipX+sdpX] = GetOneLMScore(spassP->lmlaP,lct,sdpA[sdpX].svX);
	else
	  lmsA[rootX+sipX+sdpX] = lmExactScore[sdpA[sdpX].svX];
	if (lmsA[rootX+sipX+sdpX] == LMSCORE_MAX)
	  lmsA[rootX+sipX+sdpX] = SCORE_MAX;
	else
	  lmsA[rootX+sipX+sdpX] *= lz; 
      }
    }

    if (score < thresh) {
      for (phoneX=0;phoneX<phoneN;phoneX++) {
	transA[phoneX][transN[phoneX]].score = score;
	transA[phoneX][transN[phoneX]].bpIdx = bpX;
	transA[phoneX][transN[phoneX]].lct   = lct;
	transA[phoneX][transN[phoneX]].phoneX = phoneY;
	transN[phoneX]++;
      }
    }
  }

  return;
}

/* ------------------------------------------------------------------------
    expand root instances from backpointer candidates
   ------------------------------------------------------------------------ */

static void spassExpand_SRoot (SPass* spassP)
{
  /* starting roots */
  STree*     streeP = spassP->streeP;
  int         rootX = 0;
  int         rootN = streeP->rootTbl.N;
  SRoot**    rootPA = streeP->rootTbl.PA;

  /* backpointer interface */
  LIceExt**  transA = spassP->trans.A;
  int*       transN = spassP->trans.N;
  int      bpOffset = spassP->stabP->useNA[spassP->frameX-1];
  float**      lmsA = spassP->trans.lmscore;

  /* lct2pos access information */
  register int hashSize = streeP->lct2pos.hashSize;
  register int bitmask  = streeP->lct2pos.bitmask;
  register int offset   = streeP->lct2pos.offset;
  register int tryMax   = streeP->lct2pos.tryMax;

  register float statethresh = spassP->beam.best + spassP->beam.state;

  int      morphMax = streeP->smem.morphMax;

  int newX;

  /* go into all roots */
  for (rootX=0; rootX<rootN;rootX++) {
    SRoot* srootP = rootPA[rootX];
    RIce**   iceA = srootP->iceA;
    RIce*    iceP = NULL;
    float  fscore = spassP->fmatch.s[srootP->phoneX];

    /* get number/size of path tokens */
    LCM*     lcmP = spassP->lcmP + srootP->xhmmX;

    /* new candidates from the backpointer table */
    LIceExt* newP = transA[srootP->phoneX];
    int      newN = transN[srootP->phoneX]; 
    
    /* get info from old instances */
    int    morphN = srootP->morphN;
    int      oldN = morphN;
    LPM*      l2p = srootP->lct2pos;

    if (newN == 0) continue;

    if (! lmlaTimeConstraint_LNX(spassP->lmlaP,spassP->frameX,srootP->lnX)) {
      continue;
    }

    for (newX=0;newX<newN;newX++,newP++) {
      int       tryN = 0;
      int         s1 = (newP->lct & bitmask);
      float      lms = lmsA[newP->bpIdx-bpOffset][rootX];
      float newscore = newP->score + lms;

      if (newscore +fscore >= statethresh) continue;
      if (morphN > 0 && newscore >= srootP->iceA[srootP->bestX]->score + spassP->beam.morph) continue;

      iceP = NULL;

      /* --------------------------------------------------- */
      /* get position candiate by hash function              */
      /* --------------------------------------------------- */
      while (tryN < tryMax) {
	int s2 = l2p[s1];

	/* check hash entry */
	if (s2 != LPM_NIL && newP->lct != iceA[s2]->lct) {
	  tryN++;
	  s1 += offset;	    	  
	  if (s1 > hashSize) {
	    s1 -= hashSize;
	  }
	  continue;
	}

	/* same lct : update instance */
	if (s2 != LPM_NIL) {	 

	  /* check token score */
	  if (newscore >= iceA[s2]->path[1].s) {iceP = NULL; break; }
	  iceP=iceA[s2];
	  iceP->path[1].s     = newscore;
	  iceP->path[1].bpIdx = newP->bpIdx;
	  iceP->path[1].hmmX  = lcmP->hmmXA[newP->phoneX];	   
	  
	  /* check instance score */
	  if (newscore >= iceP->score) {iceP = NULL; break; }
	  iceP->score = newscore;

	  /* update best score */
	  if (newscore <= iceA[srootP->bestX]->score) {
	    srootP->bestX = s2;
	  }

	  /* update worst score */
	  if (s2 == srootP->worstX) {
	    int       morphX = morphN-1;
	    float worstscore = -SCORE_MAX;
	    for (;morphX>=0;morphX--) {
	      if (iceA[morphX]->score > worstscore) {		   
		srootP->worstX = morphX;
		worstscore     = iceA[morphX]->score;
	      }
	    }
	  }	  	  
	  iceP = NULL;
	  break;
	}

	/* new lct -> insert instance */
	if (s2 == LPM_NIL) {
	  if (morphN < srootP->allocN) {
	    iceP = iceA[(int) (l2p[s1] = morphN++)];
	  } else if ((morphN < morphMax || newscore < iceA[srootP->worstX]->score) && morphN < LPM_MAX) {
	    srootP->allocN++;
	    srootP->morphN = morphN; /* update morphN for re-allocation */ 
	    smemAllocRoot(srootP,streeP);
	    iceA = srootP->iceA;
	    iceP = iceA[(int) (l2p[s1] = morphN++)] ;
	  }
	  break;
	}
      }
      
      /* --------------------------------------------------- */
      /* get position candiate by search                     */
      /* --------------------------------------------------- */
      
      if (tryN == tryMax) {
	int morphY = morphN-1;
	while (morphY >=0 && iceA[morphY]->lct != newP->lct) morphY--;
	if (morphY < 0) {
	  if (morphN < srootP->allocN) {
	    iceP = iceA[morphN++];
	  } else if ((morphN < morphMax || newscore < iceA[srootP->worstX]->score) && morphN < LPM_MAX) {
	    srootP->allocN++;
	    srootP->morphN = morphN;
	    smemAllocRoot(srootP,streeP);
	    iceA = srootP->iceA;
	    iceP = iceA[morphN++];
	  }
	} else {
	  if (newscore >= iceA[morphY]->path[1].s) { iceP = NULL; continue; }
	  iceP=iceA[morphY];
	  iceP->path[1].s     = newscore;
	  iceP->path[1].bpIdx = newP->bpIdx;
	  iceP->path[1].hmmX  = lcmP->hmmXA[newP->phoneX];
	  
	  /* update worst score */
	  if (morphY == srootP->worstX) {
	    int       morphX = morphN-1;
	    float worstscore = -SCORE_MAX;
	    for (;morphX>=0;morphX--) {
	      if (iceA[morphX]->score > worstscore) {		   
		srootP->worstX = morphX;
		worstscore     = iceA[morphX]->score;
	      }
	    }
	  }
	  
	  /* update best score */
	  if (newscore < iceA[srootP->bestX]->score) {
	    srootP->bestX = morphY;	   
	  }
	  iceP = NULL;
	}
      }
      
      /* --------------------------------------------------- */
      /* fill new instance                                   */
      /* --------------------------------------------------- */
      
      if (iceP) {
	if (morphN == 1 || newscore < iceA[srootP->bestX]->score) {
	  srootP->bestX = morphN-1;
	}
	if (morphN == 1 || newscore > iceA[srootP->worstX]->score) {
	  srootP->worstX = morphN-1;
	}
	iceP->score = newscore;
	iceP->lms   = lms;
	iceP->lct   = newP->lct;
	iceP->path[1].s     = newscore;
	iceP->path[1].bpIdx = newP->bpIdx;
	iceP->path[1].hmmX  = lcmP->hmmXA[newP->phoneX];	      
      }
    } /* for all candidates */

    /* activate root pointer */
    srootP->morphN = morphN;
    if (oldN == 0)
      spassP->actNext.rootPA[spassP->actNext.rootN++] = srootP;

  } /* for all roots */
  return;
}

/* ------------------------------------------------------------------------
    expand single phone word instances from backpointer candidates
   ------------------------------------------------------------------------ */

static void spassExpand_SIPhone (SPass* spassP)
{
  STree*     streeP = spassP->streeP;  
  SVMap*     svmapP = streeP->svmapP;
  SWord*     swordP = streeP->svocabP->list.itemA;

  /* starting SIPhones */
  int          sipX = 0;
  int          sipN = streeP->sTbl.iN;
  SIPhone*     sipP = streeP->sTbl.iA;
  int         rootN = streeP->rootTbl.N;

  /* backpointer interface */
  LIceExt**  transA = spassP->trans.A;
  int*       transN = spassP->trans.N;
  BP*           bpA = spassP->stabP->A;
  int      bpOffset = spassP->stabP->useNA[spassP->frameX-1];
  float**      lmsA = spassP->trans.lmscore;

  /* lct2pos access information */
  register int hashSize = streeP->lct2pos.hashSize;
  register int bitmask  = streeP->lct2pos.bitmask;
  register int offset   = streeP->lct2pos.offset;
  register int tryMax   = streeP->lct2pos.tryMax;

  LVX      startLVX = streeP->svmapP->idx.start;
  int      morphMax = streeP->smem.morphMax;

  register float statethresh = spassP->beam.best + spassP->beam.state;

  /* go into all single phone words */
  for (sipX=0; sipX<sipN;sipX++,sipP++) {
    NIce**   iceA = sipP->iceA;
    NIce*    iceP = NULL;
    int    filTag = (swordP + sipP->svX)->fillerTag;
    LVX       lvX = svmapGet(svmapP,sipP->svX);
    float   delta = 0.0;

    /* new candidates from the backpointer table */
    LIceExt* newP = transA[sipP->phoneX];
    int      newN = transN[sipP->phoneX]; 
    
    /* sip instances */
    int    morphN = sipP->morphN;
    LPM*      l2p = sipP->lct2pos;

    int newX; 

    if (lvX == startLVX || newN == 0)  continue; 
    if (filTag == 0 && lvX == LVX_NIL) continue;

    if (! lmlaTimeConstraint_SVX(spassP->lmlaP,spassP->frameX,sipP->svX)) {
      continue;
    }

    /* adjust word penalty */
    if (filTag == 1) 
      delta = streeP->svmapP->penalty.fil - streeP->svmapP->penalty.word;

    for (newX=0;newX<newN;newX++,newP++) {
      int       tryN = 0;
      int         s1 = (newP->lct & bitmask);
      float      lms = lmsA[newP->bpIdx-bpOffset][rootN+sipX];
      float newscore = newP->score + lms + delta;

      /* avoid sequence of identical filler words */
      if (filTag && sipP->svX == bpA[newP->bpIdx].svX) continue;

      if (newscore >= statethresh) continue;
      if (morphN > 0 && newscore >= sipP->iceA[sipP->bestX]->score + spassP->beam.morph) continue;

      iceP = NULL;

      /* --------------------------------------------------- */
      /* get position candiate by hash function              */
      /* --------------------------------------------------- */
      while (tryN < tryMax) {
	int s2 = l2p[s1];

	/* check hash entry */
        if (s2 != LPM_NIL && newP->lct != iceA[s2]->lct) {
          tryN++;
          s1 += offset;           
          if (s1 > hashSize) {
            s1 -= hashSize;
          }
          continue;
        }

        /* same lct : update instance */
        if (s2 != LPM_NIL) {     

	  /* check token score */
          if (newscore >= iceA[s2]->path[1].s) {iceP = NULL; break; }
	  iceP=iceA[s2];
	  iceP->path[1].s     = newscore;
	  iceP->path[1].bpIdx = newP->bpIdx;
	  
	  /* check instance score */
          if (newscore >= iceP->score) {iceP = NULL; break; }
          iceP->score = newscore;

          /* update best score */
          if (newscore <= iceA[sipP->bestX]->score) {
            sipP->bestX = s2;
          }

	  /* update worst score */
          if (s2 == sipP->worstX) {
            int       morphX = morphN-1;
            float worstscore = -SCORE_MAX;
            for (;morphX>=0;morphX--) {
              if (iceA[morphX]->score > worstscore) {              
                sipP->worstX = morphX;
                worstscore     = iceA[morphX]->score;
              }
            }
          }               
	  iceP = NULL;
	  break;
	}

	/* new lct -> insert instance */
	if (s2 == LPM_NIL) {
	  if (morphN < sipP->allocN) {
	    iceP = iceA[(int) (l2p[s1] = morphN++)] ;
	  } else if ((morphN < morphMax || newscore < iceA[sipP->worstX]->score) && morphN < LPM_MAX) {
	    sipP->allocN++;
	    sipP->morphN = morphN;
	    smemAllocSIPhone(sipP,streeP);
	    iceA = sipP->iceA;
	    iceP = iceA[(int) (l2p[s1] = morphN++)] ;
	  }
	  break;
	}
      }
      
      /* --------------------------------------------------- */
      /* get position candiate by search                     */
      /* --------------------------------------------------- */
      
      if (tryN == tryMax) {
	int morphY = morphN-1;
        while (morphY >=0 && iceA[morphY]->lct != newP->lct) morphY--;
        if (morphY < 0) {
	  if (morphN < sipP->allocN) {
	    iceP = iceA[morphN++];
	  } else if ((morphN < morphMax || newscore < iceA[sipP->worstX]->score) && morphN < LPM_MAX) {
	    sipP->allocN++;
	    sipP->morphN = morphN;
	    smemAllocSIPhone(sipP,streeP);
	    iceA = sipP->iceA;
	    iceP = iceA[morphN++];
	  }
	} else {
	  if (newscore >= iceA[morphY]->path[1].s) { iceP= NULL; break; }
	  iceP=iceA[morphY];
	  iceP->path[1].s     = newscore;
	  iceP->path[1].bpIdx = newP->bpIdx;
	  
	  /* update best score */
	  if (newscore < iceA[sipP->bestX]->score) {
	    sipP->bestX = morphY;      
	  }
	  
	    /* update worst score */
	  if (morphY == sipP->worstX) {
	    int       morphX = morphN-1;
	    float worstscore = -SCORE_MAX;
	    for (;morphX>=0;morphX--) {
	      if (iceA[morphX]->score > worstscore) {            
		sipP->worstX = morphX;
		worstscore     = iceA[morphX]->score;
	      }
	    }
	  }	    
	  iceP = NULL;
	}
      }
      
      /* --------------------------------------------------- */
      /* fill new instance                                   */
      /* --------------------------------------------------- */
      
      if (iceP) {
	if (morphN == 1 || newscore < iceA[sipP->bestX]->score) {
          sipP->bestX = morphN-1;
        }
        if (morphN == 1 || newscore > iceA[sipP->worstX]->score) {
          sipP->worstX = morphN-1;
        }

	iceP->score = newscore;
	iceP->lms   = lms;
	iceP->lct   = newP->lct;
	iceP->path[1].s     = newscore;
	iceP->path[1].bpIdx = newP->bpIdx;
      }
    } /* for all candidates */

    sipP->morphN = morphN;

  } /* for all single phone words */
  return;
}

/* ------------------------------------------------------------------------
    expand context dependent single phone word instances 
   ------------------------------------------------------------------------ */

static void spassExpand_SDPhone (SPass* spassP)
{
  STree*     streeP = spassP->streeP;  
  SVMap*     svmapP = streeP->svmapP;
  SWord*     swordP = streeP->svocabP->list.itemA;

  /* starting SDPhones */
  int          sdpX = 0;
  int          sdpN = streeP->sTbl.dN;
  SDPhone*     sdpP = streeP->sTbl.dA;
  int         rootN = streeP->rootTbl.N;
  int          sipN = streeP->sTbl.iN;

  /* backpointer interface */
  LIceExt**  transA = spassP->trans.A;
  int*       transN = spassP->trans.N;
  int      bpOffset = spassP->stabP->useNA[spassP->frameX-1];
  float**      lmsA = spassP->trans.lmscore;

  /* lct2pos access information */
  register int hashSize = streeP->lct2pos.hashSize;
  register int bitmask  = streeP->lct2pos.bitmask;
  register int offset   = streeP->lct2pos.offset;
  register int tryMax   = streeP->lct2pos.tryMax;

  LVX      startLVX = streeP->svmapP->idx.start;

  register float statethresh = spassP->beam.best + spassP->beam.state;
  int               morphMax = streeP->smem.morphMax;

  /* go into all single phone words */
  for (sdpX=0; sdpX<sdpN;sdpX++,sdpP++) {
    XIce**   iceA = sdpP->iceA;
    XIce*    iceP = NULL;
    int    filTag = (swordP + sdpP->svX)->fillerTag;
    LVX       lvX = svmapGet(svmapP,sdpP->svX);
    float   delta = 0.0;

    /* get number/size of path tokens */
    XCM*     xcmP = spassP->xcmP + sdpP->hmmX;
    PHMM*    hmmP = spassP->hmmP + xcmP->hmmXA[0][0];
    int    stateN = hmmP->stateN + 1;

    /* new candidates from the backpointer table */
    LIceExt* newP = transA[sdpP->phoneX];
    int      newN = transN[sdpP->phoneX]; 
    
    /* sdp instances */
    int    morphN = sdpP->morphN;
    LPM*      l2p = sdpP->lct2pos;

    int newX, hmmX; 

    if (lvX == startLVX || newN == 0) continue;
    if (filTag == 0 && lvX == LVX_NIL) continue;

    if (! lmlaTimeConstraint_SVX(spassP->lmlaP,spassP->frameX,sdpP->svX)) {
      continue;
    }

    /* adjust word penalty */
    if (filTag == 1) 
      delta = streeP->svmapP->penalty.fil - streeP->svmapP->penalty.word;

    for (newX=0;newX<newN;newX++,newP++) {
      int       tryN = 0;
      int         s1 = (newP->lct & bitmask);
      float      lms = lmsA[newP->bpIdx-bpOffset][rootN+sipN+sdpX];
      int       hmmN = xcmP->hmmNA[newP->phoneX];
      float newscore = newP->score + lms +delta;

      if (newscore >= statethresh) continue;
      if (morphN > 0 && newscore >= iceA[sdpP->bestX]->score + spassP->beam.morph) continue;

      iceP = NULL;

      /* --------------------------------------------------- */
      /* get position candiate by hash function              */
      /* --------------------------------------------------- */
      while (tryN < tryMax) {
	int s2 = l2p[s1];

	/* check hash entry */
        if (s2 != LPM_NIL && (newP->lct != iceA[s2]->lct || newP->phoneX != iceA[s2]->phoneX)) {
          tryN++;
          s1 += offset;           
          if (s1 > hashSize) {
            s1 -= hashSize;
          }
          continue;
        }

        /* same lct : update instance */ 
	if (s2 != LPM_NIL) {

	  /* check token score for first state */          
	  iceP=iceA[s2];
	  for (hmmX=0;hmmX<hmmN;hmmX++) {
	    if (newscore < iceP->path[hmmX*stateN+1].s) {
	      iceP->path[hmmX*stateN+1].s     = newscore;
	      iceP->path[hmmX*stateN+1].bpIdx = newP->bpIdx;
	    }
	  }

	  /* check instance score */
          if (newscore >= iceP->score) {iceP = NULL; break; }
          iceP->score = newscore;

	  /* update best score */
          if (newscore <= iceA[sdpP->bestX]->score) {
            sdpP->bestX = s2;
          }

          /* update worst score */
          if (s2 == sdpP->worstX) {
            int       morphX = morphN-1;
            float worstscore = -SCORE_MAX;
            for (;morphX>=0;morphX--) {
              if (iceA[morphX]->score > worstscore) {              
                sdpP->worstX = morphX;
                worstscore   = iceA[morphX]->score;
              }
            }
          }               
	  iceP = NULL;
          break;
	}

	/* unseen lct -> insert instance */
	if (s2 == LPM_NIL) {
	  if (morphN < sdpP->allocN) {
	    iceP = iceA[(int) (l2p[s1] = morphN++)] ;
	  } else if ((morphN < morphMax || newscore < iceA[sdpP->worstX]->score) && morphN < LPM_MAX) {
	    sdpP->allocN++;
	    sdpP->morphN = morphN;
	    smemAllocSDPhone(sdpP,streeP);
	    iceA = sdpP->iceA;
	    iceP = iceA[(int) (l2p[s1] = morphN++)] ;
	  }
	  break;
	}
	tryN++;
	s1 += offset;	    	  
	if (s1 > hashSize) {
	  s1 -= hashSize;
	}
      }
      
      /* --------------------------------------------------- */
      /* get position candiate by search                     */
      /* --------------------------------------------------- */
      
      if (tryN == tryMax) {
	int morphY = morphN-1;
	while (morphY >= 0 && 
	       (iceA[morphY]->lct    != newP->lct  ||
		iceA[morphY]->phoneX != newP->phoneX)) morphY--;

	if (morphY < 0) {
	  if (morphN < sdpP->allocN) {
	    iceP = iceA[morphN++];
	  } else if ((morphN < morphMax || newscore < iceA[sdpP->worstX]->score) && morphN < LPM_MAX) {     
	    sdpP->allocN++;
	    sdpP->morphN = morphN;
	    smemAllocSDPhone(sdpP,streeP);
	    iceA = sdpP->iceA;
	    iceP = iceA[morphN++];
	  }	
	} else {
	  iceP=iceA[morphY];
	  for (hmmX=0;hmmX<hmmN;hmmX++) {
	    if (newscore <  iceP->path[hmmX*stateN+1].s) {
	      iceP->path[hmmX*stateN+1].s     = newscore;
	      iceP->path[hmmX*stateN+1].bpIdx = newP->bpIdx;
	    }
	  }

	  /* update best score */
	  if (newscore < iceA[sdpP->bestX]->score) {
	    sdpP->bestX = morphY;      
	  }
	  
	  /* update worst score */
	  if (morphY == sdpP->worstX) {
	    int       morphX = morphN-1;
	    float worstscore = -SCORE_MAX;
	    for (;morphX>=0;morphX--) {
	      if (iceA[morphX]->score > worstscore) {            
		sdpP->worstX = morphX;
		worstscore     = iceA[morphX]->score;
	      }
	    }
	  } 
	  iceP = NULL;
	}
      }

      /* --------------------------------------------------- */
      /* fill new instance                                   */
      /* --------------------------------------------------- */
      
      if (iceP) {
	if (morphN == 1 || newscore < iceA[sdpP->bestX]->score) {
          sdpP->bestX = morphN-1;
        }
        if (morphN == 1 || newscore > iceA[sdpP->worstX]->score) {
          sdpP->worstX = morphN-1;
        }
	iceP->score  = newscore;
	iceP->lms    = lms;
	iceP->lct    = newP->lct;
	iceP->phoneX = newP->phoneX;
	for (hmmX=0;hmmX<hmmN;hmmX++) {
	  iceP->path[hmmX*stateN+1].s     = newscore;
	  iceP->path[hmmX*stateN+1].bpIdx = newP->bpIdx;
	}
      }
    }
    sdpP->morphN = morphN;

  } /* for all single phone words */
  return;
}

/* ========================================================================
    SPASS prune operations
   ======================================================================== */

/* ------------------------------------------------------------------------
    prune root instances
   ------------------------------------------------------------------------ */

static void spassPrune_SRoot(SPass* spassP)
{
  int             rootX = 0;
  int             rootN = spassP->actNext.rootN;
  SRoot**        rootPA = spassP->actNext.rootPA;

  STree*         streeP = spassP->streeP;
  float     statethresh = spassP->beam.best + spassP->beam.state;
  int              topN = spassP->beam.morphN;

  register int hashSize = streeP->lct2pos.hashSize;
  register int  bitmask = streeP->lct2pos.bitmask;
  register int   offset = streeP->lct2pos.offset;
  register int   tryMax = streeP->lct2pos.tryMax;

  for (rootX=0;rootX<rootN;rootX++) {
    SRoot*         srootP = rootPA[rootX];
    LCM*             lcmP = spassP->lcmP + srootP->xhmmX;
    PHMM*            hmmP = spassP->hmmP + lcmP->hmmXA[0];
    int            stateN = hmmP->stateN + 1;
 
    register RIce**  iceA = srootP->iceA;
    register RIce*   iceP = NULL;
    register LPM*     l2p = srootP->lct2pos;
    int            morphN = srootP->morphN;
    register int   morphX = 0;
    register int   morphY = morphN;
    register float thresh = spassP->beam.morph;

    int stateX,s1,s2,tryN,morphZ;

    if (morphN == 0) continue;
    if (! lmlaTimeConstraint_LNX(spassP->lmlaP,spassP->frameX,srootP->lnX)) {
      srootDeactivate(srootP,spassP->streeP);
      continue;
    }
    thresh += iceA[srootP->bestX]->score;

    if (iceA[srootP->bestX]->score >= statethresh) {
      srootDeactivate(srootP,streeP);
      continue;
    }
    spassP->active.rootPA[spassP->active.rootN++] = srootP;
   
    if (morphN < 2 || (iceA[srootP->worstX]->score < thresh  && morphN <= topN)) 
      continue;
    
    if (morphN > topN) {
      srootSort(srootP,streeP);
      morphX=0;
      while (morphX < topN && iceA[morphX]->score < thresh) morphX++;
      
      srootP->bestX  = 0;
      srootP->worstX = morphX-1;
      
    } else {
      register float  bestscore =  SCORE_MAX;
      register float worstscore = -SCORE_MAX;

      while (morphX < morphY) { 
	/* get next pruned instance */
	while (morphX < morphY && iceA[morphX]->score < thresh)  morphX++;
	/* get next active instance */
	morphY--;
	while (morphX < morphY && thresh < iceA[morphY]->score)  morphY--;  
	if (morphX >= morphY) break;
	
	/* move it */
	iceP = iceA[morphX];
	iceA[morphX] = iceA[morphY];  
	iceA[morphY] = iceP;
	morphX++;
      }
      
      /* update best_score, worst_score */
      for (morphY=morphX-1;morphY>=0;morphY--) {
	if (bestscore > iceA[morphY]->score) {
	  bestscore= iceA[morphY]->score;
	  srootP->bestX = morphY;
	}
	if (worstscore < iceA[morphY]->score) {
	  worstscore= iceA[morphY]->score;
	  srootP->worstX = morphY;
	}
      }      
    }

    /* compute new lct2pos map */
    for (morphZ=0;morphZ<hashSize;morphZ++)
      l2p[morphZ]=LPM_NIL;
    
    for (morphY=0;morphY<morphX;morphY++) {
      tryN = 0;
      s1 = (iceA[morphY]->lct & bitmask);
      while (tryN < tryMax) {
	s2 = l2p[s1];
	if (s2 == morphY) {
	  break;
	}
	if (s2 == LPM_NIL) {
	  l2p[s1]=morphY;
	  break;
	} else {
	  tryN++;
	  s1 += offset;	    	  
	  if (s1 > hashSize) {
	    s1 -= hashSize;
	  }
	}
      }
    }
    
    /* deactivate pruned instances */
    srootP->morphN = morphX;
    for (;morphX<morphN;morphX++) {
      RTok* path = iceA[morphX]->path;
      iceA[morphX]->lct = LCT_NIL;
      for (stateX=0;stateX<stateN;stateX++) {
	path[stateX].s = SCORE_MAX;
      }
    }    
  } /* for all active roots */

  return;
}

/* ------------------------------------------------------------------------
    prune node instances
   ------------------------------------------------------------------------ */

static void spassPrune_SNode(SPass* spassP)
{
  int             nodeX = 0;
  int             nodeN = spassP->actNext.nodeN;
  SNode**        nodePA = spassP->actNext.nodePA;
  STree*         streeP = spassP->streeP;
  float     statethresh = spassP->beam.best + spassP->beam.state;

  for (nodeX=0;nodeX<nodeN;nodeX++) {
    SNode*       snodeP   = nodePA[nodeX];
    PHMM*          hmmP   = spassP->hmmP + snodeP->X.hmm;
    int          stateN   = hmmP->stateN + 1;

    register NIce** iceA  = snodeP->iceA.n;
    register NIce*  iceP  = NULL;
    int           morphN  = snodeP->morphN;
    register int  morphX  = 0;
    register int  morphY  = morphN;
    float         thresh  = 0.0;
    register float bestscore, worstscore;
    int stateX;

    if (snodeP->info == WordIsUnique  &&
	! lmlaTimeConstraint_SVX(spassP->lmlaP,spassP->frameX,snodeP->vX.s)) {
      snodeDeactivate(snodeP,spassP->streeP);
      continue;
    }
    if (snodeP->info == WordIsNotUnique  &&
	! lmlaTimeConstraint_LNX(spassP->lmlaP,spassP->frameX,snodeP->vX.l)) {
      snodeDeactivate(snodeP,spassP->streeP);
      continue;
    }
    thresh = iceA[snodeP->bestX]->score + spassP->beam.morph;

    if (iceA[snodeP->bestX]->score >= statethresh) {
      snodeDeactivate(snodeP,streeP);
      continue;
    }
    spassP->active.nodePA[spassP->active.nodeN++] = snodeP;
    
    if (morphN < 2 || iceA[snodeP->worstX]->score < thresh) continue;
    
    morphX=0;
    while (morphX < morphY) { 
      /* get next pruned instance */
      while (morphX < morphY && iceA[morphX]->score < thresh) morphX++;
      
      /* get next active instance */
      morphY--;
      while (morphX < morphY && thresh < iceA[morphY]->score) morphY--;  
      if (morphX >= morphY) break;
      
      iceP =  iceA[morphX];
      iceA[morphX] = iceA[morphY];  
      iceA[morphY] = iceP;
      
      morphX++;
    }

    /* update best_score, worst_score */
    bestscore  =  SCORE_MAX;
    worstscore = -SCORE_MAX;
    for (morphY=morphX-1;morphY>=0;morphY--) {
      if (bestscore > iceA[morphY]->score) {
	bestscore= iceA[morphY]->score;
	snodeP->bestX = morphY;
      }
      if (worstscore < iceA[morphY]->score) {
	worstscore= iceA[morphY]->score;
	snodeP->worstX = morphY;
      }
    }
    
    assert (morphX > 0);
    
    /* deactivate pruned instances */
    snodeP->morphN = morphX;
    
    for (;morphX<morphN;morphX++) {
      NTok* path = iceA[morphX]->path;
      iceA[morphX]->lct = LCT_NIL;
      for (stateX=0;stateX<stateN;stateX++) {
	path[stateX].s = SCORE_MAX;
      }
    }
  } /* for all nodes */
  return;
}

/* ------------------------------------------------------------------------
    prune leaf instances
   ------------------------------------------------------------------------ */

static void spassPrune_SLeaf(SPass* spassP)
{
  int             leafX = 0;
  int             leafN = spassP->actNext.leafN;
  SNode**        leafPA = spassP->actNext.leafPA;
  STree*         streeP = spassP->streeP;
  float      wordthresh = spassP->beam.best + spassP->beam.word;

  for (leafX=0;leafX<leafN;leafX++) {
    SNode*         sleafP = leafPA[leafX];
    RCM*             rcmP = spassP->rcmP + sleafP->X.xhmm;
    PHMM*            hmmP = spassP->hmmP + rcmP->hmmXA[0];
    int            stateN = (hmmP->stateN+1)*rcmP->hmmN;

    register LIce** iceA  = sleafP->iceA.l;
    register LIce*  iceP  = NULL;
    int           morphN  = sleafP->morphN;
    register int  morphX  = 0;
    register int  morphY  = morphN;
    float         thresh  = 0.0;
    register float bestscore, worstscore;
    int stateX;

    if (sleafP->info == WordIsUnique  &&
	! lmlaTimeConstraint_SVX(spassP->lmlaP,spassP->frameX,sleafP->vX.s)) {
      sleafDeactivate(sleafP,spassP->streeP);
      continue;
    }
    if ( sleafP->info == WordIsNotUnique  &&
	! lmlaTimeConstraint_LNX(spassP->lmlaP,spassP->frameX,sleafP->vX.l)) {
      sleafDeactivate(sleafP,spassP->streeP);
      continue;
    }
    thresh = iceA[sleafP->bestX]->score + spassP->beam.morph;
    if (iceA[sleafP->bestX]->score >= wordthresh) {
      sleafDeactivate(sleafP,streeP);
      continue;
    } 
    spassP->active.leafPA[spassP->active.leafN++] = sleafP;

    if (morphN < 2 || iceA[sleafP->worstX]->score  < thresh) continue;

    morphX=0;
    while (morphX < morphY) {   
      /* get next pruned instance */
      while (morphX < morphY && iceA[morphX]->score < thresh) morphX++;
      /* get next active instance */
      morphY--;
      while (morphX < morphY && thresh < iceA[morphY]->score) morphY--;  
      if (morphX >= morphY) break;
      iceP = iceA[morphX]; 
      iceA[morphX] = iceA[morphY];
      iceA[morphY] = iceP;
      morphX++;
    }
    
    /* update best_score, worst_score */
    bestscore  =  SCORE_MAX;
    worstscore = -SCORE_MAX;
    for (morphY=morphX-1;morphY>=0;morphY--) {
      if (bestscore > iceA[morphY]->score) {
	bestscore= iceA[morphY]->score;
	sleafP->bestX = morphY;
      }
      if (worstscore < iceA[morphY]->score) {
	worstscore= iceA[morphY]->score;
	sleafP->worstX = morphY;
      }
    }

    /* deactivate pruned instances */
    sleafP->morphN = morphX;
    for (;morphX<morphN;morphX++) {
      NTok* path = iceA[morphX]->path;
      iceA[morphX]->lct = LCT_NIL;
      for (stateX=0;stateX<stateN;stateX++) {
	path[stateX].s = SCORE_MAX;
      }
    }
    assert(sleafP->iceA.l[sleafP->worstX]->score >= sleafP->iceA.l[sleafP->bestX]->score);
  } /* for all leafs */
  return;
}

/* ------------------------------------------------------------------------
    prune SIPhone instances
   ------------------------------------------------------------------------ */

static void spassPrune_SIPhone(SPass* spassP)
{

  STree*         streeP = spassP->streeP;
  SIPhone*         sipP = streeP->sTbl.iA;
  int              sipN = streeP->sTbl.iN;
  int              sipX = 0;
  int              topN = spassP->beam.morphN;

  register int hashSize = streeP->lct2pos.hashSize;
  register int  bitmask = streeP->lct2pos.bitmask;
  register int   offset = streeP->lct2pos.offset;
  register int   tryMax = streeP->lct2pos.tryMax;

  float     statethresh = spassP->beam.best + spassP->beam.state;

  for (sipX=0;sipX<sipN;sipX++,sipP++) {
    PHMM*            hmmP = spassP->hmmP + sipP->hmmX;
    int            stateN = hmmP->stateN + 1;
    register NIce**  iceA = sipP->iceA;
    register NIce*   iceP = NULL;
    register LPM*     l2p = sipP->lct2pos;

    int            morphN = sipP->morphN;
    register int   morphX = 0;
    register int   morphY = morphN;
    register float thresh = spassP->beam.morph;

    int stateX,s1,s2,tryN,morphZ;

    if (morphN == 0) continue;
    thresh += iceA[sipP->bestX]->score;

    if (! lmlaTimeConstraint_SVX(spassP->lmlaP,spassP->frameX,sipP->svX)) {
      siphoneDeactivate(sipP,spassP->streeP);
      continue;
    }

    if (iceA[sipP->bestX]->score >= statethresh) {
      siphoneDeactivate(sipP,streeP);
      continue;
    } 

    if (morphN < 2 || (iceA[sipP->worstX]->score < thresh && morphN <= topN))
      continue; 
    
    if (morphN > topN) {
      siphoneSort(sipP,streeP);
      morphX=0;
      while (morphX < topN && iceA[morphX]->score < thresh) morphX++;
      
      sipP->bestX  = 0;
      sipP->worstX = morphX-1;
      
    } else {
      register float  bestscore =  SCORE_MAX;
      register float worstscore = -SCORE_MAX;
      
      while (morphX < morphY) { 
	/* get next pruned instance */
	while (morphX < morphY && iceA[morphX]->score < thresh) morphX++;
	/* get next active instance */
	morphY--;
	while (morphX < morphY && thresh < iceA[morphY]->score) morphY--;  
	if (morphX >= morphY) break;
	  
	/* move it */
	iceP = iceA[morphX];
	iceA[morphX] = iceA[morphY];  
	iceA[morphY] = iceP;
	
	morphX++;
      }
      
      /* update best_score, worst_score */
      bestscore  =  SCORE_MAX;
      worstscore = -SCORE_MAX;
      for (morphY=morphX-1;morphY>=0;morphY--) {
	if (bestscore > iceA[morphY]->score) {
	  bestscore= iceA[morphY]->score;
	  sipP->bestX = morphY;
	}
	if (worstscore < iceA[morphY]->score) {
	  worstscore= iceA[morphY]->score;
	  sipP->worstX = morphY;
	}
      }      
    }

    /* compute new lct2pos map */
    for (morphZ=0;morphZ<hashSize;morphZ++)
      l2p[morphZ]=LPM_NIL;
    
    for (morphY=0;morphY<morphX;morphY++) {
      tryN = 0;
      s1 = (iceA[morphY]->lct & bitmask);
      while (tryN < tryMax) {
	s2 = l2p[s1];
	if (s2 == morphY) {
	  break;
	}
	if (s2 == LPM_NIL) {
	  l2p[s1]=morphY;
	  break;
	} else {
	  tryN++;
	  s1 += offset;	    	  
	  if (s1 > hashSize) {
	    s1 -= hashSize;
	  }
	}
      }
    }

    /* deactivate pruned instances */
    sipP->morphN = morphX;
    for (;morphX<morphN;morphX++) {
      NTok* path = iceA[morphX]->path;
      iceA[morphX]->lct = LCT_NIL;
      for (stateX=0;stateX<stateN;stateX++) {
	path[stateX].s = SCORE_MAX;
      }
    }
  }
  return;
}

/* ------------------------------------------------------------------------
    prune SDPhone instances
   ------------------------------------------------------------------------ */

static void spassPrune_SDPhone(SPass* spassP)
{

  STree*         streeP = spassP->streeP;
  SDPhone*         sdpP = streeP->sTbl.dA;
  int              sdpN = streeP->sTbl.dN;
  int              sdpX = 0;
  int              topN = spassP->beam.morphN;

  register int hashSize = streeP->lct2pos.hashSize;
  register int  bitmask = streeP->lct2pos.bitmask;
  register int   offset = streeP->lct2pos.offset;
  register int   tryMax = streeP->lct2pos.tryMax;

  float     statethresh = spassP->beam.best + spassP->beam.state;

  for (sdpX=0; sdpX<sdpN;sdpX++,sdpP++) {
    XCM*             xcmP = spassP->xcmP + sdpP->hmmX;
    PHMM*            hmmP = spassP->hmmP + xcmP->hmmXA[0][0];
    int            stateN = hmmP->stateN + 1;
    register XIce**  iceA = sdpP->iceA;
    register XIce*   iceP = NULL;
    register LPM*     l2p = sdpP->lct2pos;

    int            morphN = sdpP->morphN;
    register int   morphX = 0;
    register int   morphY = morphN;
    register float thresh = spassP->beam.morph;

    int stateX,s1,s2,tryN,morphZ;

    if (morphN == 0) continue;
    thresh += iceA[sdpP->bestX]->score;

    if (! lmlaTimeConstraint_SVX(spassP->lmlaP,spassP->frameX,sdpP->svX)) {
      sdphoneDeactivate(sdpP,spassP->streeP);
      continue;
    }

    if (iceA[sdpP->bestX]->score >= statethresh) {
      sdphoneDeactivate(sdpP,streeP);
      continue;
    } 

    if (morphN < 2 || (iceA[sdpP->worstX]->score < thresh && morphN <= topN))
      continue; 
    
    if (morphN > topN) {
      sdphoneSort(sdpP,streeP);
      morphX=0;
      while (morphX < topN && iceA[morphX]->score < thresh) morphX++;
      
      sdpP->bestX  = 0;
      sdpP->worstX = morphX-1;
      
    } else {
      register float  bestscore =  SCORE_MAX;
      register float worstscore = -SCORE_MAX;
	
      while (morphX < morphY) { 
	/* get next pruned instance */
	while (morphX < morphY && iceA[morphX]->score < thresh) morphX++;
	/* get next active instance */
	morphY--;
	while (morphX < morphY && thresh < iceA[morphY]->score) morphY--;  
	if (morphX >= morphY) break;
	
	/* move it */
	iceP = iceA[morphX];
	iceA[morphX] = iceA[morphY];  
	iceA[morphY] = iceP;
	
	morphX++;
      }

      /* update best_score, worst_score */
      for (morphY=morphX-1;morphY>=0;morphY--) {
	if (bestscore > iceA[morphY]->score) {
	  bestscore= iceA[morphY]->score;
	  sdpP->bestX = morphY;
	}
	if (worstscore < iceA[morphY]->score) {
	  worstscore= iceA[morphY]->score;
	  sdpP->worstX = morphY;
	}
      }     
    }

    /* compute new lct2pos map */
    for (morphZ=0;morphZ<hashSize;morphZ++)
      l2p[morphZ]=LPM_NIL;
    
    for (morphY=0;morphY<morphX;morphY++) {
      tryN = 0;
      s1 = (iceA[morphY]->lct & bitmask);
      while (tryN < tryMax) {
	s2 = l2p[s1];
	if (s2 == morphY) {
	  break;
	}
	if (s2 == LPM_NIL) {
	  l2p[s1]=morphY;
	  break;
	} else {
	  tryN++;
	  s1 += offset;	    	  
	  if (s1 > hashSize) {
	    s1 -= hashSize;
	  }
	}
      }
    }
    
    /* deactivate pruned instances */
    sdpP->morphN = morphX;
    for (;morphX<morphN;morphX++) {
      NTok* path = iceA[morphX]->path;
      int   hmmN = xcmP->hmmMax;
      iceA[morphX]->lct = LCT_NIL;
      for (stateX=0;stateX<stateN*hmmN;stateX++) {
	path[stateX].s = SCORE_MAX;
      }
    }     
  }

  return;
}

/* ========================================================================
    SPASS acoustic evaluation routines
   ======================================================================== */

/* ------------------------------------------------------------------------
    extract needed senone evaluations for roots
   ------------------------------------------------------------------------ */

static void spassRequest_SRoot (SPass* spassP)
{
  int       activeN = spassP->senone.activeN;
  UCHAR*    activeA = spassP->senone.activeA;
  int*       indexA = spassP->senone.indexA;
  int         rootX = 0;
  int         rootN = spassP->active.rootN;
  SRoot**    rootPA = spassP->active.rootPA;
  float statethresh = spassP->beam.best + spassP->beam.state;
  int stateX,morphX;

  for (rootX=0;rootX<rootN;rootX++) {
    SRoot*     srootP = rootPA[rootX];
    LCM*         lcmP = spassP->lcmP + srootP->xhmmX;
    PHMM*        hmmP = spassP->hmmP + lcmP->hmmXA[0];
    int        morphN = rootPA[rootX]->morphN;
    int        stateN = hmmP->stateN+1;    
    float morphthresh = srootP->iceA[srootP->bestX]->score + spassP->beam.morph;
    float      thresh = (morphthresh < statethresh) ? morphthresh : statethresh; 
    int           snX = 0;

    for (morphX=0;morphX<morphN;morphX++) {
      RTok* path = srootP->iceA[morphX]->path;
  
      snX = -1;
      for (stateX=1;stateX<stateN;stateX++) {
	if (path[stateX].s < thresh) {
	  snX = (spassP->hmmP + path[stateX].hmmX)->snXA[stateX-1];
	  if (activeA[snX] == 0) { 
	    activeA[snX] = 1; 
	    indexA[activeN++]=snX;
	  }
	}
      }
    }
  }
  spassP->senone.activeN = activeN;
  return;
}

/* ------------------------------------------------------------------------
    extract needed senone evaluations for nodes
   ------------------------------------------------------------------------ */

static void spassRequest_SNode (SPass* spassP)
{
  int       activeN = spassP->senone.activeN;
  UCHAR*    activeA = spassP->senone.activeA;
  int*       indexA = spassP->senone.indexA;
  int         nodeX = 0;
  int         nodeN = spassP->active.nodeN;
  SNode**    nodePA = spassP->active.nodePA;
  float statethresh = spassP->beam.best + spassP->beam.state;
  int stateX, morphX, snX;

  for (nodeX=0;nodeX<nodeN;nodeX++) {
    SNode*     snodeP = nodePA[nodeX];
    PHMM*        hmmP = spassP->hmmP + snodeP->X.hmm;
    int        morphN = snodeP->morphN;
    int        stateN = hmmP->stateN+1;
    PHMMX*       snXA = hmmP->snXA-1;
    float morphthresh = snodeP->iceA.n[snodeP->bestX]->score + spassP->beam.morph;
    float      thresh = (morphthresh < statethresh) ? morphthresh : statethresh; 

    for (morphX=0;morphX<morphN;morphX++) {
     NTok* path = snodeP->iceA.n[morphX]->path;
      snX = -1;

      for (stateX=1;stateX<stateN;stateX++) {
	if (path[stateX].s < thresh) {
	  snX = snXA[stateX];
	  if (activeA[snX] == 0) { 
	    activeA[snX] = 1; 
	    indexA[activeN++]=snX;
	  }
	}
      }
    }
  }  
  spassP->senone.activeN = activeN;
  return;
}

/* ------------------------------------------------------------------------
    extract needed senone evaluations for leafs
   ------------------------------------------------------------------------ */

static void spassRequest_SLeaf (SPass* spassP)
{
  UCHAR*    activeA = spassP->senone.activeA;
  int       activeN = spassP->senone.activeN;
  int*       indexA = spassP->senone.indexA;
  int         leafX = 0;
  int         leafN = spassP->active.leafN;
  SNode**    leafPA = spassP->active.leafPA;
  PHMM*        hmmP = spassP->hmmP;
  PHMMX*       snXA = NULL;
  float  wordthresh = spassP->beam.best + spassP->beam.word; 
  int stateX,hmmX,morphX,snX;

  for (leafX=0;leafX<leafN;leafX++) {
    SNode*     sleafP = leafPA[leafX];
    RCM*         rcmP = spassP->rcmP + sleafP->X.xhmm;
    int          hmmN = rcmP->hmmN;
    int        morphN = sleafP->morphN;
    int        stateN = (hmmP + rcmP->hmmXA[0])->stateN+1;
    float morphthresh = sleafP->iceA.l[sleafP->bestX]->score + spassP->beam.morph;
    float      thresh = (morphthresh < wordthresh) ? morphthresh : wordthresh; 

    for (morphX=0;morphX<morphN;morphX++) {
      NTok*     path    = sleafP->iceA.l[morphX]->path;

#if USE_RCM_LM 
      float*       lmpA = sleafP->iceA.l[morphX]->lmp+1;
#endif

      for (hmmX=0;hmmX<hmmN;hmmX++, path+=stateN) {
	if (path[1].bpIdx == RCM_BPX_NIL) continue;
	snXA = (hmmP + rcmP->hmmXA[hmmX])->snXA-1;

	snX = -1;
	for (stateX=1;stateX<stateN;stateX++) {
#if USE_RCM_LM
	  if (path[stateX].s +lmpA[hmmX] < thresh) {
#else
	  if (path[stateX].s             < thresh) {
#endif
	    snX = snXA[stateX];
	    if (activeA[snX] == 0) { 
	      activeA[snX] = 1; 
	      indexA[activeN++]=snX;
	    }
	  }
	}
	/* tricky state pruning: snX == -1 => no state of this right 
	   context instance has a better score than statethresh
	   path[0] is the final state, 
	           set score to SCORE_MAX  to block transitions
	   path[1] is the begin state, 
                   set bpX to RCM_BPX_NIL to block viterbi calls
	*/
	if (snX == -1) { 
	  for (stateX=0;stateX<stateN;stateX++) 
	    path[stateX].s = SCORE_MAX; 
	  path[1].bpIdx = RCM_BPX_NIL; 
	}
      }
    } /* for each instance */
  } /* end leaf */

  spassP->senone.activeN = activeN;

  return;
}
#ifdef KEEP_EMACS_HAPPY
}
#endif

/* ------------------------------------------------------------------------
    extract needed senone evaluations for single phone words
   ------------------------------------------------------------------------ */

static void spassRequest_SIPhone (SPass* spassP)
{
  int       activeN = spassP->senone.activeN;
  UCHAR*    activeA = spassP->senone.activeA;
  int*       indexA = spassP->senone.indexA;
  int          sipX = 0;
  int          sipN = spassP->streeP->sTbl.iN;
  SIPhone*     sipP = spassP->streeP->sTbl.iA;
  float statethresh = spassP->beam.best + spassP->beam.state;
  int stateX, morphX, snX;

  for (sipX=0;sipX<sipN;sipX++, sipP++) {
    PHMM*        hmmP = spassP->hmmP + sipP->hmmX;
    int        morphN = sipP->morphN;
    int        stateN = hmmP->stateN+1;
    PHMMX*       snXA = hmmP->snXA-1;
    float   bestscore = (morphN >0) ? sipP->iceA[sipP->bestX]->score : SCORE_MAX;
    float morphthresh = bestscore + spassP->beam.morph;
    float      thresh = (morphthresh < statethresh) ? morphthresh : statethresh; 

    if (statethresh < bestscore) continue;
   
    for (morphX=0;morphX<morphN;morphX++) {
      NTok* path = sipP->iceA[morphX]->path;
      for (stateX=1;stateX<stateN;stateX++) {
	if (path[stateX].s < thresh) {
	  snX = snXA[stateX];
	  if (activeA[snX] == 0) { 
	    activeA[snX] = 1; 
	    indexA[activeN++]=snX;
	  }
	}
      }
    }
  }
  spassP->senone.activeN = activeN;
  return;
}

/* ------------------------------------------------------------------------
    extract senone evaluations for context dependent single phone words
   ------------------------------------------------------------------------ */

static void spassRequest_SDPhone (SPass* spassP)
{
  int       activeN = spassP->senone.activeN;
  UCHAR*    activeA = spassP->senone.activeA;
  int*       indexA = spassP->senone.indexA;
  int          sdpX = 0;
  int          sdpN = spassP->streeP->sTbl.dN;
  SDPhone*     sdpP = spassP->streeP->sTbl.dA;
  float statethresh = spassP->beam.best + spassP->beam.state;
  int hmmX, stateX, morphX, snX;

  for (sdpX=0;sdpX<sdpN;sdpX++, sdpP++) {
    XCM*         xcmP = spassP->xcmP + sdpP->hmmX;
    int        morphN = sdpP->morphN;
    float   bestscore = (morphN >0) ? sdpP->iceA[sdpP->bestX]->score : SCORE_MAX;
    float morphthresh = bestscore + spassP->beam.morph;
    float      thresh = (morphthresh < statethresh) ? morphthresh : statethresh; 

    if (statethresh < bestscore) continue;
  
    for (morphX=0;morphX<morphN;morphX++) {
      int       hmmN = xcmP->hmmNA[sdpP->iceA[morphX]->phoneX];
      NTok*     path = sdpP->iceA[morphX]->path;

      for (hmmX=0;hmmX<hmmN;hmmX++) {
	PHMM*     hmmP = spassP->hmmP + xcmP->hmmXA[sdpP->iceA[morphX]->phoneX][hmmX];
	int     stateN = hmmP->stateN+1;
	PHMMX*    snXA = hmmP->snXA-1;

	if (path[hmmX*stateN+1].bpIdx == RCM_BPX_NIL) continue;

	snX= -1;
	for (stateX=1;stateX<stateN;stateX++) {
	  if (path[hmmX*stateN+stateX].s < thresh) {
	    snX = snXA[stateX];
	    if (activeA[snX] == 0) { 
	      activeA[snX] = 1; 
	      indexA[activeN++]=snX;
	    }
	  }
	}
	if (snX ==-1) {
	  path[hmmX*stateN+1].bpIdx = RCM_BPX_NIL;
	  for (stateX=0;stateX<stateN;stateX++) {
	    path[hmmX*stateN+stateX].s = SCORE_MAX;
	  }
	}
      }
    }
  }
  spassP->senone.activeN = activeN;
  return;
}


/* ========================================================================
   SPASS fast match operations
   ======================================================================== */

/* ------------------------------------------------------------------------
    initialize fast match module
   ------------------------------------------------------------------------ */

static int spassBuildFMatch (SPass* spassP, SenoneSet* snsP, int* snTagA, 
			     int frameN, int stateN, float factor) 
{
  int   snsMax  = spassP->fmatch.phoneN*stateN;
  int  padPhone = spassP->streeP->hmmsP->treeP->questions.padPhoneX;
  int* activeA = NULL;
  int     tagX = 0;
  int phoneX,frameX,stateX;
  Word word;

  if (! (stateN ==1 || (stateN ==2 && frameN ==5))) {
    ERROR("spassBuildFMatch: Invalid configuration\n");
    return TCL_ERROR;
  }

  if (spassP->fmatch.snsP) delink(spassP->fmatch.snsP,"SenoneSet");

  if (spassP->fmatch.scoreA) {
    for (frameX=0;frameX<spassP->fmatch.frameN;frameX++) {
      if (spassP->fmatch.scoreA[frameX]) 
	free(spassP->fmatch.scoreA[frameX]);
    }
    free(spassP->fmatch.snsP);
  }

  link(snsP,"SenoneSet");

  spassP->fmatch.snsP   = snsP;
  spassP->fmatch.frameN = frameN;
  spassP->fmatch.stateN = stateN;
  spassP->fmatch.factor = factor;

  if (NULL == (spassP->fmatch.scoreA = (float**) 
	       malloc(frameN*sizeof(float*)))) {
    ERROR("spassBuildFMatch: allocation failure\n");
    return TCL_ERROR;
  }
  for (frameX=0;frameX<frameN;frameX++) {
    if (NULL == (spassP->fmatch.scoreA[frameX] = (float*) 
		 malloc(snsMax*sizeof(float)))) {
      ERROR("spassBuildFMatch: allocation failure\n");
      return TCL_ERROR;
    }
  }

  if (NULL == (spassP->fmatch.indexA= (int*)
	       realloc(spassP->fmatch.indexA,snsMax*sizeof(int)))) {
    ERROR("spassBuildFMatch: allocation* failure\n");
    return TCL_ERROR;
  }
  
  if (NULL == (spassP->fmatch.snXA= (int*)
	       realloc(spassP->fmatch.snXA,snsMax*sizeof(int)))) {
    ERROR("spassBuildFMatch: allocation failure\n");
    return TCL_ERROR;
  }
 
  /* initialize monophone word */
  word.phoneA  = &phoneX;
  word.tagA    = &tagX;
  word.itemN   = 1;
  word.phonesP = spassP->streeP->dictP->phones;
  word.tagsP   = spassP->streeP->dictP->tags;

  /* get sequence of senone indices for all fast match models */
  for (phoneX=0;phoneX<spassP->fmatch.phoneN;phoneX++) {
    if (phoneX == padPhone) continue;
    for (stateX=0;stateX<stateN;stateX++) {
      spassP->fmatch.snXA[phoneX*stateN+stateX] = 
	snsGetModel(snsP,snTagA[stateX],&word,0,0);
      assert(spassP->fmatch.snXA[phoneX*stateN+stateN] >=0);
    }
  }      

  if (NULL == (activeA = (int*) calloc(snsP->list.itemN,sizeof(int)))) {
    ERROR("spassBuildFMatch: allocation failure\n");
    return TCL_ERROR;
  }
   
  /* get array of unique (permanently) active senone indices */
  spassP->fmatch.activeN=0;
  for (phoneX=0;phoneX<spassP->fmatch.phoneN;phoneX++) {
    if (phoneX == padPhone) continue;
    for (stateX=0;stateX<stateN;stateX++) {
      int snX = spassP->fmatch.snXA[phoneX*stateN+stateX];
      if (! activeA[snX]) {
	spassP->fmatch.indexA[spassP->fmatch.activeN++]= snX;
	activeA[snX] = 1;
      }
    }
  }

  if (activeA) free(activeA);
  return TCL_OK;
}

static int spassBuildFMatchItf( ClientData clientData, 
			int argc, char *argv[])
{
  int             ac = argc - 1;
  SPass*      spassP = (SPass*)clientData;
  SenoneSet*    snsP = NULL;
  SenoneTagArray sta = {NULL,0,NULL};
  char*         name = "ROOT-b ROOT-m";

  int         frameN = (spassP->fmatch.frameN > 0) ? 
    spassP->fmatch.frameN : spassDefault.fmatch.frameN;
  float       factor = (spassP->fmatch.frameN > 0) ? 
    spassP->fmatch.factor : spassDefault.fmatch.factor;

  if ( itfParseArgv( argv[0], &ac, argv+1, 0,
       "<senoneSet>", ARGV_OBJECT, NULL, &snsP,   "SenoneSet", "set of senones",
       "-frameN",     ARGV_INT,    NULL, &frameN, NULL, "nr. of fast match frames",
       "-factor",     ARGV_FLOAT,  NULL, &factor, NULL, "weighting factor for fast match models",
       "-snTag",      ARGV_STRING, NULL, &name,   NULL, "sequence of senone tags",
        NULL) != TCL_OK) return TCL_ERROR;  

  /* get senone tag array */
  sta.snsP = snsP;
  senoneTagArrayGetItf(&sta,name);

  return spassBuildFMatch(spassP,snsP,sta.tagA,frameN,sta.tagN,factor);

}			      

/* ------------------------------------------------------------------------
    compute fast match score
   ------------------------------------------------------------------------ */

static void spassFastMatch (SPass* spassP)
{
  int padPhone = spassP->streeP->hmmsP->treeP->questions.padPhoneX;
  int frameN = spassP->fmatch.frameN;
  int phoneN = spassP->fmatch.phoneN;
  int stateN = spassP->fmatch.stateN;
  float best = SCORE_MAX;

  int frameX,phoneX;

  if (spassP->fmatch.frameN <= 0) return;
  if (spassP->fmatch.factor <= 0) return;

  if (spassP->frameX + frameN >= spassP->frameN) {
    for (phoneX=0;phoneX<phoneN;phoneX++) 
      spassP->fmatch.s[phoneX] = 0;
    return;
  }

  if (spassP->frameX == 0) {
    /* compute acoustic scores for all fast match frames */
    for (frameX=0;frameX<frameN;frameX++) {
      spassP->fmatch.snsP->scoreAFctP(spassP->fmatch.snsP,
	       spassP->fmatch.indexA,
	       spassP->fmatch.scoreA[frameX],
	       spassP->fmatch.activeN,
	       spassP->frameX+frameX);
    }
  } else {
    /* update ring buffer */
    float* tmp =  spassP->fmatch.scoreA[0];
    for (frameX=0;frameX<frameN-1;frameX++) {
      spassP->fmatch.scoreA[frameX]= spassP->fmatch.scoreA[frameX+1];
    }
    spassP->fmatch.scoreA[frameN-1]= tmp;
    spassP->fmatch.snsP->scoreAFctP(spassP->fmatch.snsP,
	     spassP->fmatch.indexA,
	     spassP->fmatch.scoreA[frameN-1],
	     spassP->fmatch.activeN,
	     spassP->frameX+frameN-1);
  }

  /* compute fast match scores for 2state topologies*/
  if (stateN == 2 && frameN == 5) {
    for (phoneX=0;phoneX<phoneN;phoneX++) {
      if (phoneX == padPhone) {
	spassP->fmatch.s[phoneX] = SCORE_MAX;
      } else {
	int snX = spassP->fmatch.snXA[phoneX*stateN+0];
	int snY = spassP->fmatch.snXA[phoneX*stateN+1];
	float slow = 
	  spassP->fmatch.scoreA[1][snX] + 
	  spassP->fmatch.scoreA[2][snX] +
	  spassP->fmatch.scoreA[3][snY] + 
	  spassP->fmatch.scoreA[4][snY];
	float fast = 
	  spassP->fmatch.scoreA[1][snX] + 
	  spassP->fmatch.scoreA[1][snX] +
	  spassP->fmatch.scoreA[2][snY] + 
	  spassP->fmatch.scoreA[2][snY];
	if (slow < fast)
	  spassP->fmatch.s[phoneX] = spassP->fmatch.factor * slow;
	else
	  spassP->fmatch.s[phoneX] = spassP->fmatch.factor * fast;
	if (spassP->fmatch.s[phoneX] < best)
	  best = spassP->fmatch.s[phoneX];
      }
    }
  }

  /* compute fast match scores for 1state topologies*/
  if (stateN == 1) {
    for (phoneX=0;phoneX<phoneN;phoneX++) {
      if (phoneX == padPhone) {
	spassP->fmatch.s[phoneX] = SCORE_MAX;
      } else {
	int     snX = spassP->fmatch.snXA[phoneX*stateN+0];
	float score = 0.0;
	int  frameX = 1;
	for (frameX=1;frameX<frameN;frameX++) {
	  score +=  spassP->fmatch.scoreA[frameX][snX];
	}
	spassP->fmatch.s[phoneX] = spassP->fmatch.factor * score;
	if (spassP->fmatch.s[phoneX] < best)
	  best = spassP->fmatch.s[phoneX];
      }
    }
  }

  /* Actually the best overall score should be computed as the maximum
     over all current theories with their best phonetic match score.
     However, we approximate this by the best theory score plus best
     phonetic match score and can simply subtract the best phone score
     here.
  */
  for (phoneX=0;phoneX<phoneN;phoneX++) 
    spassP->fmatch.s[phoneX] -= best;

  return;

}

/* ========================================================================
   spassPathSub : subtracts from all (active) path a given value 
   ======================================================================== */

static void spassPathSub_SRoot (SPass* spassP, float value)
{
  int         rootX = 0;
  int         rootN = spassP->active.rootN;
  SRoot**    rootPA = spassP->active.rootPA;
  int stateX,morphX;

#ifdef STABLE_DBG
  fprintf (stderr, "SRoot: ");
#endif
  for (rootX=0;rootX<rootN;rootX++) {
    SRoot*     srootP = rootPA[rootX];
    LCM*         lcmP = spassP->lcmP + srootP->xhmmX;
    PHMM*        hmmP = spassP->hmmP + lcmP->hmmXA[0];
    int        morphN = rootPA[rootX]->morphN;
    int        stateN = hmmP->stateN+1;    

    for (morphX=0;morphX<morphN;morphX++) {
      RTok* path = srootP->iceA[morphX]->path;
  
      for (stateX=1;stateX<stateN;stateX++) {
	if ( path[stateX].s < SCORE_MAX ) {
	  path[stateX].s -= value;
#ifdef STABLE_DBG
	  fprintf (stderr, "%d-%d-%d-%d ", rootX, morphX, stateX, path[stateX].bpIdx);
#endif
	}
      }
    }
  }
#ifdef STABLE_DBG
  fprintf (stderr, "\n");
#endif
  return;
}

static void spassPathSub_SNode (SPass* spassP, float value)
{
  int         nodeX = 0;
  int         nodeN = spassP->active.nodeN;
  SNode**    nodePA = spassP->active.nodePA;
  int stateX, morphX;

#ifdef STABLE_DBG
  fprintf (stderr, "SNode: ");
#endif
  for (nodeX=0;nodeX<nodeN;nodeX++) {
    SNode*     snodeP = nodePA[nodeX];
    PHMM*        hmmP = spassP->hmmP + snodeP->X.hmm;
    int        morphN = snodeP->morphN;
    int        stateN = hmmP->stateN+1;

    for (morphX=0;morphX<morphN;morphX++) {
      NTok* path = snodeP->iceA.n[morphX]->path;
      
      for (stateX=1;stateX<stateN;stateX++) {
	if ( path[stateX].s < SCORE_MAX ) {
	  path[stateX].s -= value;
#ifdef STABLE_DBG
	  fprintf (stderr, "%d-%d-%d-%d ", nodeX, morphX, stateX, path[stateX].bpIdx);
#endif
	}
      }
    }
  }
#ifdef STABLE_DBG
  fprintf (stderr, "\n");
#endif
  return;
}

static void spassPathSub_SLeaf (SPass* spassP, float value)
{
  int         leafX = 0;
  int         leafN = spassP->active.leafN;
  SNode**    leafPA = spassP->active.leafPA;
  PHMM*        hmmP = spassP->hmmP;
  int stateX,hmmX,morphX;

#ifdef STABLE_DBG
  fprintf (stderr, "SLeaf: ");
#endif
  for (leafX=0;leafX<leafN;leafX++) {
    SNode*     sleafP = leafPA[leafX];
    RCM*         rcmP = spassP->rcmP + sleafP->X.xhmm;
    int          hmmN = rcmP->hmmN;
    int        morphN = sleafP->morphN;
    int        stateN = (hmmP + rcmP->hmmXA[0])->stateN+1;

    for (morphX=0;morphX<morphN;morphX++) {
      NTok*     path    = sleafP->iceA.l[morphX]->path;

      for (hmmX=0;hmmX<hmmN;hmmX++, path+=stateN) {
	/* TODO: check if continue is okay */
	if (path[1].bpIdx == RCM_BPX_NIL) continue;

	for (stateX=1;stateX<stateN;stateX++) {
	  if ( path[stateX].s < SCORE_MAX ) {
	    path[stateX].s -= value;
#ifdef STABLE_DBG
	    fprintf (stderr, "%d-%d-%d-%d-%d ", leafX, morphX, hmmX, stateX, path[stateX].bpIdx);
#endif
	  }
	}
      } 
    } /* for each instance */
  } /* end leaf */
#ifdef STABLE_DBG
  fprintf (stderr, "\n");
#endif

  return;
}

static void spassPathSub_SIPhone (SPass* spassP, float value)
{
  int          sipX = 0;
  int          sipN = spassP->streeP->sTbl.iN;
  SIPhone*     sipP = spassP->streeP->sTbl.iA;
  int stateX, morphX;

#ifdef STABLE_DBG
  fprintf (stderr, "SIPhone: ");
#endif
  for (sipX=0;sipX<sipN;sipX++, sipP++) {
    PHMM*        hmmP = spassP->hmmP + sipP->hmmX;
    int        morphN = sipP->morphN;
    int        stateN = hmmP->stateN+1;

    for (morphX=0;morphX<morphN;morphX++) {
      NTok* path = sipP->iceA[morphX]->path;
      for (stateX=1;stateX<stateN;stateX++) {
	if ( path[stateX].s < SCORE_MAX ) {
	  path[stateX].s -= value;
#ifdef STABLE_DBG
	  fprintf (stderr, "%d-%d-%d-%d ", sipX, morphX, stateX, path[stateX].bpIdx);
#endif
	}
      }
    }
  }
#ifdef STABLE_DBG
  fprintf (stderr, "\n");
#endif

  return;
}

static void spassPathSub_SDPhone (SPass* spassP, float value)
{
  int          sdpX = 0;
  int          sdpN = spassP->streeP->sTbl.dN;
  SDPhone*     sdpP = spassP->streeP->sTbl.dA;
  int hmmX, stateX, morphX;

#ifdef STABLE_DBG
  fprintf (stderr, "SDPhone: ");
#endif
  for (sdpX=0;sdpX<sdpN;sdpX++, sdpP++) {
    XCM*         xcmP = spassP->xcmP + sdpP->hmmX;
    int        morphN = sdpP->morphN;

    for (morphX=0;morphX<morphN;morphX++) {
      int       hmmN = xcmP->hmmNA[sdpP->iceA[morphX]->phoneX];
      NTok*     path = sdpP->iceA[morphX]->path;

      for (hmmX=0;hmmX<hmmN;hmmX++) {
	PHMM*     hmmP = spassP->hmmP + xcmP->hmmXA[sdpP->iceA[morphX]->phoneX][hmmX];
	int     stateN = hmmP->stateN+1;

	if (path[hmmX*stateN+1].bpIdx == RCM_BPX_NIL) continue;

	for (stateX=1;stateX<stateN;stateX++) {
	  if ( path[hmmX*stateN+stateX].s < SCORE_MAX ) {
	    path[hmmX*stateN+stateX].s -= value;
#ifdef STABLE_DBG
	    fprintf (stderr, "%d-%d-%d-%d-%d ", sdpX, morphX, hmmX, stateX, path[stateX].bpIdx);
#endif
	  }
	}
      }
    }
  }
#ifdef STABLE_DBG
  fprintf (stderr, "\n");
#endif

  return;
}

static void spassPathSub (SPass* spassP, float value) 
{

  /* update array pointer */
  spassP->hmmP    = spassP->streeP->hmmsP->list.itemA;
  spassP->lcmP    = spassP->streeP->lcmsP->list.itemA;
  spassP->rcmP    = spassP->streeP->rcmsP->list.itemA;
  if (spassP->streeP->xcmsP)
    spassP->xcmP  = spassP->streeP->xcmsP->list.itemA;
  else
    spassP->xcmP  = NULL;

  spassPathSub_SRoot  (spassP, value);
  spassPathSub_SNode  (spassP, value);
  spassPathSub_SLeaf  (spassP, value);
  spassPathSub_SIPhone(spassP, value);
  spassPathSub_SDPhone(spassP, value);

  /* adjust the score of the best path to match the new score */
  spassP->beam.best -= value;
  
  return;
}

static int spassPathSubItf( ClientData clientData, int argc, char *argv[]) {
  int          ac   = argc - 1;
  SPass*     spassP = (SPass*)clientData;
  float       value = 0.0;

  if ( itfParseArgv( argv[0], &ac, argv+1, 0,
       "<val>",  ARGV_FLOAT, NULL, &value,  NULL, "value subtracted from all active paths",
        NULL) != TCL_OK) return TCL_ERROR;

  spassPathSub    (spassP,value);

  return TCL_OK;
}

/* ========================================================================
   spassActiveBPs: return all active BPs in last frame
   ======================================================================== */
static int spassTraceStable (SPass *spassP, int fromX, int toX, int *endA, int endN, int verbose) {

  int         rootX = 0;
  int         rootN = spassP->active.rootN;
  SRoot**    rootPA = spassP->active.rootPA;
  int         nodeX = 0;
  int         nodeN = spassP->active.nodeN;
  SNode**    nodePA = spassP->active.nodePA;
  int         leafX = 0;
  int         leafN = spassP->active.leafN;
  SNode**    leafPA = spassP->active.leafPA;
  PHMM*        hmmP = spassP->hmmP;
  int          sipX = 0;
  int          sipN = spassP->streeP->sTbl.iN;
  SIPhone*     sipP = spassP->streeP->sTbl.iA;
  int          sdpX = 0;
  int          sdpN = spassP->streeP->sTbl.dN;
  SDPhone*     sdpP = spassP->streeP->sTbl.dA;
  int stateX,hmmX,morphX;
  int*          bpA = NULL;
  int           bpN = 0;
  int             b;
  int           res;

#ifdef STABLE_DBG
  int *tpA = NULL;
  int  tpN = 0;
  int  t;
#endif

  /* update array pointer */
  spassP->hmmP    = spassP->streeP->hmmsP->list.itemA;
  spassP->lcmP    = spassP->streeP->lcmsP->list.itemA;
  spassP->rcmP    = spassP->streeP->rcmsP->list.itemA;
  if (spassP->streeP->xcmsP)
    spassP->xcmP  = spassP->streeP->xcmsP->list.itemA;
  else
    spassP->xcmP  = NULL;

  /* SRoots */
  for (rootX=0;rootX<rootN;rootX++) {
    SRoot*     srootP = rootPA[rootX];
    LCM*         lcmP = spassP->lcmP + srootP->xhmmX;
    PHMM*        hmmP = spassP->hmmP + lcmP->hmmXA[0];
    int        morphN = rootPA[rootX]->morphN;
    int        stateN = hmmP->stateN+1;    

    for (morphX=0;morphX<morphN;morphX++) {
      RTok* path = srootP->iceA[morphX]->path;
  
      for (stateX=1;stateX<stateN;stateX++) {
	if ( path[stateX].s >= SCORE_MAX ) {
#ifdef STABLE_DBG
	  if ( tpN%40 == 0 ) tpA = (int*)realloc (tpA, (tpN+40)*sizeof(int));
	  t   = 0;
	  while (t<tpN && tpA[t] != path[stateX].bpIdx) t++;
	  if (t==tpN) tpA[tpN++] = path[stateX].bpIdx;
#endif
	  continue;
	}
	if ( bpN % 40 == 0 ) {
	  if ( NULL == (bpA = (int*)realloc (bpA, (bpN+40) * sizeof(int))) ) {
	    ERROR ("spassTraceStable: allocation failure\n");
	    return TCL_ERROR;
	  }
	}
	b = 0;
	while ( b < bpN && bpA[b] != path[stateX].bpIdx ) b++;
	if ( b == bpN ) bpA[bpN++] = path[stateX].bpIdx;
      }
    }
  }

  /* SNodes */
  for (nodeX=0;nodeX<nodeN;nodeX++) {
    SNode*     snodeP = nodePA[nodeX];
    PHMM*        hmmP = spassP->hmmP + snodeP->X.hmm;
    int        morphN = snodeP->morphN;
    int        stateN = hmmP->stateN+1;

    for (morphX=0;morphX<morphN;morphX++) {
      NTok* path = snodeP->iceA.n[morphX]->path;
      
      for (stateX=1;stateX<stateN;stateX++) {
	if ( path[stateX].s >= SCORE_MAX ) {
#ifdef STABLE_DBG
	  if ( tpN%40 == 0 ) tpA = (int*)realloc (tpA, (tpN+40)*sizeof(int));
	  t   = 0;
	  while (t<tpN && tpA[t] != path[stateX].bpIdx) t++;
	  if (t==tpN) tpA[tpN++] = path[stateX].bpIdx;
#endif
	  continue;
	}
	if ( bpN % 40 == 0 ) {
	  if ( NULL == (bpA = (int*)realloc (bpA, (bpN+40) * sizeof(int))) ) {
	    ERROR ("spassTraceStable: allocation failure\n");
	    return TCL_ERROR;
	  }
	}
	b = 0;
	while ( b < bpN && bpA[b] != path[stateX].bpIdx ) b++;
	if ( b == bpN ) bpA[bpN++] = path[stateX].bpIdx;
      }
    }
  }

  /* SLeafs */
  for (leafX=0;leafX<leafN;leafX++) {
    SNode*     sleafP = leafPA[leafX];
    RCM*         rcmP = spassP->rcmP + sleafP->X.xhmm;
    int          hmmN = rcmP->hmmN;
    int        morphN = sleafP->morphN;
    int        stateN = (hmmP + rcmP->hmmXA[0])->stateN+1;

    for (morphX=0;morphX<morphN;morphX++) {
      NTok* path = sleafP->iceA.l[morphX]->path;

      for (hmmX=0;hmmX<hmmN;hmmX++, path+=stateN) {
	if (path[1].bpIdx == RCM_BPX_NIL) continue;

	for (stateX=1;stateX<stateN;stateX++) {
	  if ( path[stateX].s >= SCORE_MAX ) {
#ifdef STABLE_DBG
	    if ( tpN%40 == 0 ) tpA = (int*)realloc (tpA, (tpN+40)*sizeof(int));
	    t   = 0;
	    while (t<tpN && tpA[t] != path[stateX].bpIdx) t++;
	    if (t==tpN) tpA[tpN++] = path[stateX].bpIdx;
#endif
	    continue;
	  }
	  if ( bpN % 40 == 0 ) {
	    if ( NULL == (bpA = (int*)realloc (bpA, (bpN+40) * sizeof(int))) ) {
	      ERROR ("spassTraceStable: allocation failure\n");
	      return TCL_ERROR;
	    }
	  }
	  b = 0;
	  while ( b < bpN && bpA[b] != path[stateX].bpIdx ) b++;
	  if ( b == bpN ) bpA[bpN++] = path[stateX].bpIdx;
	}
      } 
    } /* for each instance */
  } /* end leaf */

  /* SIPhones */
  for (sipX=0;sipX<sipN;sipX++, sipP++) {
    PHMM*        hmmP = spassP->hmmP + sipP->hmmX;
    int        morphN = sipP->morphN;
    int        stateN = hmmP->stateN+1;

    for (morphX=0;morphX<morphN;morphX++) {
      NTok* path = sipP->iceA[morphX]->path;
      for (stateX=1;stateX<stateN;stateX++) {
	if ( path[stateX].s >= SCORE_MAX ) {
#ifdef STABLE_DBG
	  if ( tpN%40 == 0 ) tpA = (int*)realloc (tpA, (tpN+40)*sizeof(int));
	  t   = 0;
	  while (t<tpN && tpA[t] != path[stateX].bpIdx) t++;
	  if (t==tpN) tpA[tpN++] = path[stateX].bpIdx;
#endif
	  continue;
	}
	if ( bpN % 40 == 0 ) {
	  if ( NULL == (bpA = (int*)realloc (bpA, (bpN+40) * sizeof(int))) ) {
	    ERROR ("spassTraceStable: allocation failure\n");
	    return TCL_ERROR;
	  }
	}
	b = 0;
	while ( b < bpN && bpA[b] != path[stateX].bpIdx ) b++;
	if ( b == bpN ) bpA[bpN++] = path[stateX].bpIdx;
      }
    }
  }

  /* SDPhones */
  for (sdpX=0;sdpX<sdpN;sdpX++, sdpP++) {
    XCM*         xcmP = spassP->xcmP + sdpP->hmmX;
    int        morphN = sdpP->morphN;

    for (morphX=0;morphX<morphN;morphX++) {
      int       hmmN = xcmP->hmmNA[sdpP->iceA[morphX]->phoneX];
      NTok*     path = sdpP->iceA[morphX]->path;

      for (hmmX=0;hmmX<hmmN;hmmX++) {
	PHMM*     hmmP = spassP->hmmP + xcmP->hmmXA[sdpP->iceA[morphX]->phoneX][hmmX];
	int     stateN = hmmP->stateN+1;

	if (path[hmmX*stateN+1].bpIdx == RCM_BPX_NIL) continue;

	for (stateX=1;stateX<stateN;stateX++) {
	  if ( path[hmmX*stateN+stateX].s >= SCORE_MAX ) {
#ifdef STABLE_DBG
	    if ( tpN%40 == 0 ) tpA = (int*)realloc (tpA, (tpN+40)*sizeof(int));
	    t   = 0;
	    while (t<tpN && tpA[t] != path[hmmX*stateN+stateX].bpIdx) t++;
	    if (t==tpN) tpA[tpN++] = path[hmmX*stateN+stateX].bpIdx;
#endif
	    continue;
	  }
	  if ( bpN % 40 == 0 ) {
	    if ( NULL == (bpA = (int*)realloc (bpA, (bpN+40) * sizeof(int))) ) {
	      ERROR ("spassTraceStable: allocation failure\n");
	      return TCL_ERROR;
	    }
	  }
	  b = 0;
	  while ( b < bpN && bpA[b] != path[hmmX*stateN+stateX].bpIdx ) b++;
	  if ( b == bpN ) bpA[bpN++] = path[hmmX*stateN+stateX].bpIdx;
	}
      }
    }
  }

#ifdef STABLE_DBG
  if ( tpN ) {
    int i;
    itfAppendResult ("{xBP ");
    for (i=0;i<tpN;i++)
      itfAppendResult ("%d ",tpA[i]);
    itfAppendResult ("} ");
  }
  if ( bpN ) {
    int i;
    itfAppendResult ("{aBP ");
    for (i=0;i<bpN;i++)
      itfAppendResult ("%d ",bpA[i]);
    itfAppendResult ("} ");
  }
#endif

  res = stabTraceStable (spassP->stabP, fromX, toX, endA, endN, bpA, bpN, verbose);

#ifdef STABLE_DBG
  if ( tpN ) free (tpA);
  if ( bpN ) free (bpA);
#endif

  return res;
}

static int spassTraceStableItf (ClientData cd, int argc, char *argv[]) {

  SPass* spassP = (SPass*)cd;
  int     fromX = 0;
  int       toX = -1;
  int   verbose = 0;
  char     *bpL = NULL;
  int     *endA = NULL;
  int      endN = 0;
  char     **av = NULL;
  int        ac = 0;
  int       res;

  argc--;
  if ( itfParseArgv( argv[0], &argc, argv+1, 0,
      "-fromX",  ARGV_INT,    NULL, &fromX,   NULL, "start frame for trace back",
      "-toX",    ARGV_INT,    NULL, &toX,     NULL, "final frame for trace back",
      "-bpL",    ARGV_STRING, NULL, &bpL,     NULL, "list of backpointers to end trace back",
      "-v",      ARGV_INT,    NULL, &verbose, NULL, "verbose output",
        NULL) != TCL_OK) return TCL_ERROR;

  if ( bpL ) {
    if ( TCL_ERROR == (Tcl_SplitList (itf, bpL, &ac, &av)) ) {
      ERROR ("unable to split list\n");
      if (av) Tcl_Free ((char*) av);
      return TCL_ERROR;
    }

    endA = (int*)malloc (ac * sizeof(int));
    for (endN=0; endN<ac; endN++) {
      endA[endN] = atoi(av[endN]);
    }
  }

  res = spassTraceStable (spassP, fromX, toX, endA, endN, verbose);

  if (av)   Tcl_Free ((char*) av);
  if (endA) free (endA);

  return res;
}


/* ========================================================================
    main program loop
   ======================================================================== */

/* ------------------------------------------------------------------------
    start procedure
   ------------------------------------------------------------------------ */

/* forward declaration */
static void spassClear (SPass* spassP, int clearStab);

static void spassStart (SPass* spassP)
{
  SVMap*  svmapP = spassP->streeP->svmapP;
  float       lz = svmapP->lz;
  int    bitmask = spassP->streeP->lct2pos.bitmask;
  int      bpIdx = -1;
  float    score = 0.0;
  LCT        lct = CreateLCT (spassP->lmlaP,spassP->streeP->startTbl.svX);
  int         s1 = (lct & bitmask);
  int        svX = 0;
  int hmmX;

  if (lct == LCT_NIL) {
    ERROR("spassStart: Couldn't initialize linguistic state\n");   
    spassClear (spassP,1);
    if (spassP->latP->beam.nodeN > 0) {
      glatClear(spassP->latP);

    }
    spassP->frameX = INT_MAX;
    return;
  }

  /* Set the frame shift for the lattice (in sec) */
  if (spassP->senone.snsP) {
    SenoneSet*      snsP = spassP->senone.snsP;
    StreamArray*    staP = &(snsP->strA); 
    Stream*      pStream = (Stream*)staP->strPA[0];
    FeatureSet*     fesP = NULL;

    if      (streq (pStream->name, "DistribSet"))
      fesP = ((DistribStream*)staP->cdA[0])->dssP->cbsP->feat;
#ifndef IBIS
    else if (streq (pStream->name, "MicvSet"))
      fesP =    ((MicvStream*)staP->cdA[0])->pMicvSet->pFeatSet;
#endif
    else {
      ERROR ("spassStart: Unknown stream type.\n");
      return;
    }

    spassP->latP->frameShift = 0.001*fesP->sfshift;
  }

  spassP->frameX       = 0;
  spassP->restart      = 0;
  spassP->beam.best    = 0.0;

  spassP->active.rootN = 0;
  spassP->active.nodeN = 0;
  spassP->active.leafN = 0;

  spassP->actNext.rootN = 0;
  spassP->actNext.nodeN = 0;
  spassP->actNext.leafN = 0;

  for (svX=0;svX<spassP->streeP->startTbl.N;svX++) {
    if (spassP->streeP->startTbl.typeA[svX] == type_SIPhone) {
      SIPhone* sipP = spassP->streeP->startTbl.A[svX];

      sipP->bestX                  = 0;
      sipP->worstX                 = 0;
      sipP->morphN                 = 1;
      sipP->lct2pos[s1]            = 0;
      sipP->iceA[0]->lms           = lz*svmapP->lvxSA[sipP->svX];
      sipP->iceA[0]->lct           = lct;
      sipP->iceA[0]->score         = score + sipP->iceA[0]->lms;
      sipP->iceA[0]->path[1].s     = score + sipP->iceA[0]->lms;
      sipP->iceA[0]->path[1].bpIdx = bpIdx;
    }
    if (spassP->streeP->startTbl.typeA[svX] == type_SDPhone) {
      SDPhone* sdpP = spassP->streeP->startTbl.A[svX];

      /* get number/size of path tokens */
      XCM*     xcmP = spassP->xcmP + sdpP->hmmX;
      PHMM*    hmmP = spassP->hmmP + xcmP->hmmXA[0][0];
      int    stateN = hmmP->stateN + 1;
      int      hmmN = xcmP->hmmNA[sdpP->phoneX];

      sdpP->bestX              = 0;
      sdpP->worstX             = 0;
      sdpP->morphN             = 1;
      sdpP->lct2pos[s1]        = 0;
      sdpP->iceA[0]->phoneX    = sdpP->phoneX;
      sdpP->iceA[0]->lms       = lz*svmapP->lvxSA[sdpP->svX];
      sdpP->iceA[0]->lct       = lct;
      sdpP->iceA[0]->score     = score + sdpP->iceA[0]->lms;
      for (hmmX=0;hmmX<hmmN;hmmX++) {
	sdpP->iceA[0]->path[hmmX*stateN+1].s     = score + sdpP->iceA[0]->lms;
	sdpP->iceA[0]->path[hmmX*stateN+1].bpIdx = bpIdx;
      }
    }
    if (spassP->streeP->startTbl.typeA[svX] == type_SRoot) {
      SRoot* srootP = spassP->streeP->startTbl.A[svX];
      LCM*     lcmP = spassP->lcmP + srootP->xhmmX;
      int      hmmX = lcmP->hmmXA[srootP->phoneX];

      spassP->active.rootPA[spassP->active.rootN++]   = srootP;
      spassP->actNext.rootPA[spassP->actNext.rootN++] = srootP;
      srootP->allocN++;
      smemAllocRoot(srootP,spassP->streeP);

      srootP->bestX                  = 0;
      srootP->worstX                 = 0;
      srootP->morphN                 = 1;
      srootP->lct2pos[s1]            = 0;
      srootP->iceA[0]->lms           = 0.0;
      srootP->iceA[0]->lct           = lct;
      srootP->iceA[0]->score         = score;
      srootP->iceA[0]->path[1].s     = score;
      srootP->iceA[0]->path[1].bpIdx = bpIdx;
      srootP->iceA[0]->path[1].hmmX  = hmmX;
    }
  }

  if (spassP->latP->beam.nodeN > 0) {
    glatClear(spassP->latP);
  }

  return;
}

/* ------------------------------------------------------------------------
    finish procedure
   ------------------------------------------------------------------------ */

static void spassClear (SPass* spassP, int clearStab)
{
  int x;
  
  smemClear(&(spassP->streeP->smem));

  if (clearStab) {
    stabClear(spassP->stabP);
  }

  /* nodes and leafs are deactivated by smemClear, only
     roots, sip's and sdp's have to be considered
  */
  for (x=0;x<spassP->active.rootN;x++) {
    SRoot* srootP = spassP->active.rootPA[x];
    srootDeactivate(srootP,spassP->streeP);
  }
  for (x=0;x<spassP->streeP->sTbl.iN;x++) {
    SIPhone* sipP = spassP->streeP->sTbl.iA + x;
    siphoneDeactivate(sipP,spassP->streeP);
  }
  for (x=0;x<spassP->streeP->sTbl.dN;x++) {
    SDPhone* sdpP = spassP->streeP->sTbl.dA + x;
    sdphoneDeactivate(sdpP,spassP->streeP);
  }
 
  spassP->frameX = 0;

  return;
}

/* ------------------------------------------------------------------------
    run procedure
   ------------------------------------------------------------------------ */

static void spassRun (SPass* spassP, int toX) 
{
  int   frameX = spassP->frameX;
  Timer stopit;

  /* check first frame */
  if (frameX >= toX) return;

  /* update array pointer */
  spassP->hmmP    = spassP->streeP->hmmsP->list.itemA;
  spassP->lcmP    = spassP->streeP->lcmsP->list.itemA;
  spassP->rcmP    = spassP->streeP->rcmsP->list.itemA;
  if (spassP->streeP->xcmsP)
    spassP->xcmP  = spassP->streeP->xcmsP->list.itemA;
  else
    spassP->xcmP  = NULL;

  /* reset timer */
  timerReset(&stopit);
  timerStart(&stopit);
  
  /* to allow interrupt */
  MostRecentlyReceivedSignal = 0;

  /* tell sliding window cache how far to go, and how much to cache */
  spassP->senone.snsP->toX = toX;

  /* main loop */
  for(;frameX<toX;frameX++, spassP->frameX++) {
    float score, best;
    int   snX;

    /* This allows the user to type CTRL-C at the console
       and interrupt the decoding */
    if (MostRecentlyReceivedSignal == SIGINT) {
      MostRecentlyReceivedSignal = 0;
      INFO ("spassRun: received interrupt; processed %d of %d frames.\n", frameX, toX);
      break;
    }

    /* Since we want to keep all bp entries for the final frame and we don't
       know it yet, we prune the backpointer table from the last frame if we
       go into the next frame.
    */

    if (frameX > 0 && ! spassP->restart) {
      frameX--;
      spassP->frameX--;   

      /* --------------------------------------------- */
      /* 1. prune backpointer table                    */
      /* --------------------------------------------- */

      stabPrune(spassP->stabP,frameX,spassP->beam.topN);

      /* --------------------------------------------- */
      /* 2. update lattice mapper                      */
      /* --------------------------------------------- */

      if (spassP->latP->beam.nodeN > 0) {
	glatMapBP(spassP->latP);
      }

      /* --------------------------------------------- */
      /* 3. expand new theories from previous bp's     */
      /* --------------------------------------------- */

      spassConvertBP     (spassP);
      spassExpand_SRoot  (spassP);
      spassExpand_SIPhone(spassP);
      spassExpand_SDPhone(spassP);

      /* --------------------------------------------- */
      /* 4. prune roots and single phone words         */
      /* --------------------------------------------- */

      spassP->active.rootN = 0;

      spassPrune_SRoot  (spassP);
      spassPrune_SIPhone(spassP);
      spassPrune_SDPhone(spassP);

      spassP->actNext.rootN = spassP->active.rootN;
      
      memcpy(spassP->actNext.rootPA,
	     spassP->active.rootPA,
	     sizeof(SRoot*)*spassP->active.rootN);

      frameX++;
      spassP->frameX++;

    }
    spassP->restart = 0;

    /* --------------------------------------------- */
    /* 5. evaluate senones                           */
    /* --------------------------------------------- */

    memset(spassP->senone.activeA,0,spassP->senone.totalN);
    
    for (snX=0;snX<spassP->senone.totalN;snX++) {
      spassP->senone.scoreA[snX] = SCORE_MAX;
    }

    spassP->senone.activeN = 0;
 
    spassRequest_SRoot  (spassP);
    spassRequest_SNode  (spassP);
    spassRequest_SLeaf  (spassP);
    spassRequest_SIPhone(spassP);
    spassRequest_SDPhone(spassP);

    spassP->senone.snsP->scoreAFctP(spassP->senone.snsP,
				   spassP->senone.indexA,
				   spassP->senone.scoreA,
				   spassP->senone.activeN,
				   frameX);

    /* --------------------------------------------- */
    /* 6. state transitions                          */
    /* --------------------------------------------- */
    
    best = spassVitState_SRoot (spassP);
    if (best > (score= spassVitState_SNode(spassP))) {
      best = score;
    }
    if (best > (score= spassVitState_SLeaf(spassP))) {
      best = score;
    }
    if (best > (score= spassVitState_SIPhone(spassP))) {
      best = score;
    }
    if (best > (score= spassVitState_SDPhone(spassP))) {
      best = score;
    }
    spassP->beam.best = best;

    if (spassP->beam.best + spassP->beam.state >= SCORE_MAX) {
      ERROR("spassRun at frame %d: maximum score exceeded (%6.4f)\n", frameX, spassP->beam.best);
      return;
    }

    /* --------------------------------------------- */
    /* 7. phone transitions                          */
    /* --------------------------------------------- */

    spassFastMatch(spassP);
    spassVitPhone_SRoot(spassP);
    spassVitPhone_SNode(spassP);

    /* --------------------------------------------- */
    /* 8. prune nodes and leafs                      */
    /* --------------------------------------------- */

    spassP->active.nodeN = 0;
    spassP->active.leafN = 0;

    spassPrune_SNode(spassP);
    spassPrune_SLeaf(spassP);

    spassP->actNext.nodeN = spassP->active.nodeN;
    spassP->actNext.leafN = spassP->active.leafN;

    memcpy(spassP->actNext.nodePA,
	   spassP->active.nodePA,
	   sizeof(SNode*)*spassP->active.nodeN);

    memcpy(spassP->actNext.leafPA,
	   spassP->active.leafPA,
	   sizeof(SNode*)*spassP->active.leafN);


    /* --------------------------------------------- */
    /* 9. word transitions                           */
    /* --------------------------------------------- */

    spassVitWord_SLeaf  (spassP);
    spassVitWord_SIPhone(spassP);
    spassVitWord_SDPhone(spassP);

    /* --------------------------------------------- */
    /* 10. lattice generation                         */
    /* --------------------------------------------- */

    if (spassP->latP->beam.nodeN > 0) {
      glatAdd_SLeaf  (spassP->latP);
      glatAdd_SIPhone(spassP->latP);
      glatAdd_SDPhone(spassP->latP);
      glatCut        (spassP->latP);
    }
  } 

  /* sort the backpointer entries for the final frame, but keep all of them */
  if (frameX>0) {
    stabPrune(spassP->stabP,frameX-1,spassP->stabP->N);    
    stabFinalize(spassP->stabP);
    if (spassP->latP->beam.nodeN > 0) {
      spassP->frameX--;
      glatMapBP(spassP->latP);
      spassP->frameX++;
    }
  }

  /* switch off cacheing */
  spassP->senone.snsP->toX = -1;

  return;
}

static int spassRunItf( ClientData clientData, 
			int argc, char *argv[])
{
  int          ac   = argc - 1;
  SPass*     spassP = (SPass*)clientData;
  SenoneSet*   snsP = spassP->senone.snsP;
  int         dummy = 0;
  int          endX = snsFrameN(snsP,&dummy,&dummy,&dummy);
  int           toX = endX;
  int          init = 1;
  
  spassP->frameN = endX;

  if ( itfParseArgv( argv[0], &ac, argv+1, 0,
       "-to",    ARGV_INT, NULL, &toX,  NULL, "frameN",
       "-init",  ARGV_INT, NULL, &init, NULL, "initialize search tree",
        NULL) != TCL_OK) return TCL_ERROR;  

  if (!init && ! spassP->frameX) {
    WARN("spassRun: forced search network initialization\n");
    init = 1;
  }

  if (toX > endX) toX = endX;

  if (init) {
    spassClear  (spassP,1);
    spassStart  (spassP);
  }
  spassRun    (spassP,toX);

  return TCL_OK;
}

/* ------------------------------------------------------------------------
    spassReinit
   ------------------------------------------------------------------------ */

static int spassReinit (SPass* spassP, int frameX)
{
  STree* streeP = spassP->streeP;

  if (frameX > spassP->frameX) {
    WARN("spassReinit: cannot restart at %d (current frame is %d)\n",
	 frameX,spassP->frameX);
  }
  if (frameX > -1 && spassP->latP->beam.nodeN > 0) {
    ERROR("spassReinit: cannot restart at %d by activated lattice generation\n",
	 frameX);
    return TCL_ERROR;
  }
  if (frameX > -1 && spassP->lmlaP->latlm.latP) {
    ERROR("spassReinit: cannot restart at %d by activated lattice constraints\n",
	  frameX);
    return TCL_ERROR;
  }

  streeReinit(streeP);
  lmlaReinit(spassP->lmlaP);

  /* update array pointer */
  spassP->hmmP    = spassP->streeP->hmmsP->list.itemA;
  spassP->lcmP    = spassP->streeP->lcmsP->list.itemA;
  spassP->rcmP    = spassP->streeP->rcmsP->list.itemA;
  if (spassP->streeP->xcmsP)
    spassP->xcmP  = spassP->streeP->xcmsP->list.itemA;
  else
    spassP->xcmP  = NULL;

  if (NULL == (spassP->active.rootPA = (SRoot**) 
	       realloc(spassP->active.rootPA,
		       spassP->streeP->rootTbl.N*sizeof(SRoot*)))) {
    ERROR("spassReinit: allocation failure\n");
    return TCL_ERROR;
  }
  if (NULL == (spassP->active.nodePA = (SNode**) 
	       realloc(spassP->active.nodePA,
		       spassP->streeP->nodeTbl.nodeN*sizeof(SNode*)))) {
    ERROR("spassReinit: allocation failure\n");
    return TCL_ERROR;
  }
  if (NULL == (spassP->active.leafPA = (SNode**) 
	       realloc(spassP->active.leafPA,
		       spassP->streeP->nodeTbl.leafN*sizeof(SNode*)))) {
    ERROR("spassReinit: allocation failure\n");
    return TCL_ERROR;
  }
  if (NULL == (spassP->actNext.rootPA = (SRoot**) 
	       realloc(spassP->actNext.rootPA,
		       spassP->streeP->rootTbl.N*sizeof(SRoot*)))) {
    ERROR("spassReinit: allocation failure\n");
    return TCL_ERROR;
  }
  if (NULL == (spassP->actNext.nodePA = (SNode**) 
	       realloc(spassP->actNext.nodePA,
		       spassP->streeP->nodeTbl.nodeN*sizeof(SNode*)))) {
    ERROR("spassReinit: allocation failure\n");
    return TCL_ERROR;
  }
  if (NULL == (spassP->actNext.leafPA = (SNode**) 
	       realloc(spassP->actNext.leafPA,
		       spassP->streeP->nodeTbl.leafN*sizeof(SNode*)))) {
    ERROR("spassReinit: allocation failure\n");
    return TCL_ERROR;
  }

  spassDeinitTrans(spassP);
  spassInitTrans(spassP);
  spassDeinitSenone(spassP);
  spassInitSenone(spassP);

  if (frameX == 0) {
    spassClear(spassP,1);
    spassStart(spassP);
    return TCL_OK;
  }
  if (frameX > 0) {
    int frameY;
    spassClear(spassP,0);
  
    for (frameY=frameX;frameY<=spassP->stabP->frameN;frameY++) {
      spassP->stabP->useNA[frameY]=0;
    }
    spassP->stabP->frameN = frameX;
    spassP->stabP->N      = spassP->stabP->useNA[frameX-1];

    spassP->frameX        = frameX;
    spassP->beam.best     = SCORE_MAX;

    spassP->active.rootN  = 0;
    spassP->active.nodeN  = 0;
    spassP->active.leafN  = 0;
    
    spassP->actNext.rootN = 0;
    spassP->actNext.nodeN = 0;
    spassP->actNext.leafN = 0;

    /* expand bp's from frameX-1 into new roots at frameX */
    spassP->frameX--;
    spassConvertBP_CI  (spassP);
    spassExpand_SRoot  (spassP);
    spassExpand_SIPhone(spassP);
    spassExpand_SDPhone(spassP);

    spassP->active.rootN = 0;
    
    spassPrune_SRoot  (spassP);
    spassPrune_SIPhone(spassP);
    spassPrune_SDPhone(spassP);
    
    spassP->actNext.rootN = spassP->active.rootN;
    
    memcpy(spassP->actNext.rootPA,
	   spassP->active.rootPA,
	   sizeof(SRoot*)*spassP->active.rootN);

    spassP->frameX++;
    spassP->restart = 1;
  }

  return TCL_OK;
}

static int spassReinitItf( ClientData clientData, 
			   int argc, char *argv[])
{
  int          ac   = argc - 1;
  SPass*     spassP = (SPass*)clientData;
  int        startX = -1;

  if ( itfParseArgv( argv[0], &ac, argv+1, 0,
       "-start", ARGV_INT, NULL, &startX,  NULL, "frameX for restart",
        NULL) != TCL_OK) return TCL_ERROR;  

  return spassReinit(spassP,startX);
}

/* ----------------------------------------------------------------------
   spassGetFinalLCT
   ---------------------------------------------------------------------- */
LCT spassGetFinalLCT (SPass *spassP) {

  int      bpX    = -1;
  int      fstate = 1;
  BP      *bpA    = spassP->stabP->A;
  BP      *bp     = NULL;

  if ( stabGetFinalBPX (spassP->stabP, &bpX, &fstate, -1, 0) != TCL_OK ) {
    return LCT_NIL;
  }

  bp = bpA + bpX;

  if ( spassP->streeP->nodeTbl.typeA[bp->svX] == type_SLeaf ) {
    SNode *sleafP = spassP->streeP->nodeTbl.A[bp->svX];
    
    return sleafP->iceA.l[bp->morphX]->lct;
  }

  if ( spassP->streeP->nodeTbl.typeA[bp->svX] == type_SIPhone ) {
    SIPhone *sipP = spassP->streeP->nodeTbl.A[bp->svX];
    LCT      lct  = ExtendLCT (spassP->lmlaP, sipP->iceA[bp->morphX]->lct,
			       bp->svX);

    return lct;
  }

  if ( spassP->streeP->nodeTbl.typeA[bp->svX] == type_SDPhone ) {
    SDPhone *sdpP = spassP->streeP->nodeTbl.A[bp->svX];
    LCT      lct  = ExtendLCT (spassP->lmlaP, sdpP->iceA[bp->morphX]->lct,
			       bp->svX);

    return lct;
  }

  return LCT_NIL;
}

/* ========================================================================
    SPASS interface
   ======================================================================== */

/* ------------------------------------------------------------------------
    spassPuts
   ------------------------------------------------------------------------ */

static int spassPutsItf (ClientData clientData, int argc, char *argv[])
{
  return TCL_OK;    
}

/* ------------------------------------------------------------------------
    spassConfigure
   ------------------------------------------------------------------------ */

static int spassConfigureItf(ClientData cd, char *var, char *val) {
  SPass* spassP   = (SPass*)cd; 
  if (! spassP) return TCL_ERROR;

  if (! var) {
    ITFCFG(spassConfigureItf,cd,"name");
    ITFCFG(spassConfigureItf,cd,"useN");
    ITFCFG(spassConfigureItf,cd,"frameX");
    ITFCFG(spassConfigureItf,cd,"morphBeam");
    ITFCFG(spassConfigureItf,cd,"stateBeam");
    ITFCFG(spassConfigureItf,cd,"wordBeam");
    ITFCFG(spassConfigureItf,cd,"transN");
    ITFCFG(spassConfigureItf,cd,"morphN");
    ITFCFG(spassConfigureItf,cd,"fastMatch");
    return TCL_OK;
  }

  ITFCFG_CharPtr(var,val,"name",    spassP->name,      1);
  ITFCFG_Int    (var,val,"useN",    spassP->useN,      1);
  ITFCFG_Int  (var,val,"frameX",    spassP->frameX,    1);
  ITFCFG_Float(var,val,"morphBeam", spassP->beam.morph,0);
  ITFCFG_Float(var,val,"stateBeam", spassP->beam.state,0);
  ITFCFG_Float(var,val,"wordBeam",  spassP->beam.word, 0);  
  ITFCFG_Float(var,val,"fastMatch", spassP->fmatch.factor, 0);  

  /* This is not always useful! */
  if (spassP->beam.morph >= SCORE_MAX || spassP->beam.morph < 0 ||
      spassP->beam.state >= SCORE_MAX || spassP->beam.state < 0 ||
      spassP->beam.word  >= SCORE_MAX || spassP->beam.word  < 0) {
    WARN ("insensible beams (0..%f)\n", SCORE_MAX);
  }

  /* we have to update the memory manager if we change the
     number of linguistic morphed instances  */
  if (! strcmp(var,"morphN")) {
    int morphN = (val) ?  atoi(val) : spassP->beam.morphN;
    if (morphN >LPM_MAX) {
      ERROR("Polymorphism should be below %d. (check slimits.h)\n",LPM_MAX);
      morphN = spassP->beam.morphN;
      itfAppendResult("%d ",morphN); 
      return TCL_OK; 
    }
    if (morphN != spassP->beam.morphN)
      spassP->streeP->smem.morphMax = spassP->beam.morphN = morphN;
    itfAppendResult("%d ",morphN); 
    return TCL_OK; 
  }

  /* we have to reconstruct the datastructures for converting backpointers
     if we change the number of word transitions
  */
  if (! strcmp(var,"transN")) {
    int topN = (val) ?  atoi(val) : spassP->beam.topN;
    if (topN != spassP->beam.topN) {
      spassDeinitTrans(spassP);
      spassP->beam.topN = topN;
      spassInitTrans(spassP);
    }
    itfAppendResult("%d ",topN); 
    return TCL_OK; 
  }

  ERROR("unknown option '-%s %s'.\n", var, val ? val : "?");
  return TCL_ERROR;   
}

/* ------------------------------------------------------------------------
    spassAccess
   ------------------------------------------------------------------------ */

static ClientData spassAccessItf (ClientData clientData, char *name, 
				  TypeInfo **ti)
{
  SPass* spassP = (SPass*) clientData;
  if (*name == '.') { 
    if (name[1]=='\0') {
      itfAppendElement( "stree");
      itfAppendElement( "stab");
      itfAppendElement( "glat");
      *ti = NULL; return NULL;
    }
    else { 
      if (! strcmp( name+1, "stree")) {
        *ti = itfGetType("STree");
        return (ClientData)spassP->streeP;
      }
      if (! strcmp( name+1, "stab")) {
        *ti = itfGetType("STab");
        return (ClientData)spassP->stabP;
      }
      if (! strcmp( name+1, "glat")) {
        *ti = itfGetType("GLat");
        return (ClientData)spassP->latP;
      }
    }
  }
  return NULL;
}

/* ========================================================================
    Type Information
   ======================================================================== */

Method spassMethod[] = 
{
  { "puts",   spassPutsItf ,   "puts information"},
  { "run",    spassRunItf ,    "run decoder using the underlying search network"},
  { "reinit", spassReinitItf , "reinit decoder after changes in search network"},
  { "fmatch", spassBuildFMatchItf, "initialize fast match module"},
  { "pathSub",spassPathSubItf, "subtract a constant score value from all active paths"},
  { "traceStable",spassTraceStableItf, "trace back stable hypothesis"},
  { NULL, NULL, NULL }
} ;

TypeInfo spassInfo = 
{ "SPass", 0, -1,  spassMethod, 
  spassCreateItf, spassFreeItf, 
  spassConfigureItf, spassAccessItf, NULL,
  "Single Pass Decoder"
};

static int spassInitialized = 0;

int SPass_Init (void)
{
  if (! spassInitialized) {
    if (STree_Init() != TCL_OK) return TCL_ERROR;
    if (STab_Init()  != TCL_OK) return TCL_ERROR;
    if (GLat_Init()  != TCL_OK) return TCL_ERROR;

    spassDefault.beam.morph    =  80.0;
    spassDefault.beam.state    = 130.0;
    spassDefault.beam.word     =  90.0;
    spassDefault.beam.lat      = 150.0;

    spassDefault.beam.topN     =   35;
    spassDefault.beam.morphN   =    8;

    spassDefault.fmatch.frameN =  5;
    spassDefault.fmatch.factor =  2.0;

    itfNewType (&spassInfo);
    spassInitialized = 1;
  }
  return TCL_OK;
}

