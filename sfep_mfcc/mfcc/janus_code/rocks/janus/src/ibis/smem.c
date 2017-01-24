/* ========================================================================
    JANUS      Speech- to Speech Translation
               ------------------------------------------------------------
               Object: Search Tree memory management
               ------------------------------------------------------------

    Author  :  Hagen Soltau
    Module  :  smem.c
    Date    :  $Id: smem.c 3416 2011-03-23 03:02:18Z metze $
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
    Revision 4.1  2003/08/14 11:20:07  fuegen
    Merged changes on branch jtk-01-01-15-fms (jaguar -> ibis-013)

    Revision 1.1.2.30  2003/03/18 18:08:52  soltau
    Added checks in Deactivation routines

    Revision 1.1.2.29  2002/07/18 12:16:14  soltau
    Changes according to optimized stree structures:
            bestscore -> bestX, worstscore -> worstX

    Revision 1.1.2.28  2002/06/26 11:57:54  fuegen
    changes for new directory structure support and code cleaning

    Revision 1.1.2.27  2002/03/01 10:01:08  soltau
    cleaned smemClear

    Revision 1.1.2.26  2002/02/28 16:55:23  soltau
    cleaned smemFreeRoot

    Revision 1.1.2.25  2002/01/29 16:14:14  soltau
    Fixed ABR's in QSort

    Revision 1.1.2.24  2002/01/09 17:06:44  soltau
    changed child access in stree

    Revision 1.1.2.23  2001/12/17 12:00:14  soltau
    Removed position mapper

    Revision 1.1.2.22  2001/12/17 09:45:50  soltau
    Deactivation of position mapper

    Revision 1.1.2.21  2001/12/06 16:09:40  soltau
    smemAllocSIPhone : LPM max checking

    Revision 1.1.2.20  2001/11/20 16:34:02  soltau
    optimized memory management

    Revision 1.1.2.19  2001/10/01 10:44:13  soltau
    Use polymorphism defines from slimits

    Revision 1.1.2.18  2001/07/14 15:29:12  soltau
    fixed typo

    Revision 1.1.2.17  2001/07/12 18:44:04  soltau
    Added SDPhones

    Revision 1.1.2.16  2001/06/26 15:28:23  soltau
    Added Access to bmem objects

    Revision 1.1.2.15  2001/06/25 21:06:26  soltau
    another redesign to support arbitrary hmm topologies

    Revision 1.1.2.14  2001/06/20 17:27:16  soltau
    *** empty log message ***

    Revision 1.1.2.13  2001/04/23 13:42:52  soltau
    Added semFree*

    Revision 1.1.2.12  2001/04/12 19:19:37  soltau
    don't alloc floats for leafs if USE_RCM_LM == 0

    Revision 1.1.2.11  2001/04/03 16:46:05  soltau
    fixed lmp allocation

    Revision 1.1.2.10  2001/03/27 20:49:25  soltau
    Added float blocks

    Revision 1.1.2.9  2001/03/12 18:43:30  soltau
    Fixed smemClear

    Revision 1.1.2.8  2001/03/09 07:15:18  soltau
    removed wrong reallocation for position mapper

    Revision 1.1.2.7  2001/03/06 06:44:56  soltau
    purified

    Revision 1.1.2.6  2001/02/27 13:45:52  soltau
    *** empty log message ***

    Revision 1.1.2.5  2001/02/15 09:22:23  soltau
    removed block of floats

    Revision 1.1.2.4  2001/02/09 17:46:54  soltau
    Fixed hopefully all dual and triple maps for heap indices

    Revision 1.1.2.3  2001/02/08 09:41:03  soltau
    Added char blocks
    Removed segfault candidates

    Revision 1.1.2.2  2001/01/31 14:08:40  soltau
    redesign

    Revision 1.1.2.1  2001/01/29 17:57:44  soltau
    Initial version


   ======================================================================== */

char  smemRcsVersion[]=
    "$Id: smem.c 3416 2011-03-23 03:02:18Z metze $";

#include "stree.h" 
#include "spass.h" 
#include "smem.h"


/* ------------------------------------------------------------------------
    smemInit
   ------------------------------------------------------------------------ */

int smemInit(SMem* smemP)
{
  if (smemP == NULL) {
    ERROR("smemInit: no SMem object\n");
    return TCL_ERROR;
  }
  if (smemP->streeP == NULL) {
    ERROR("smemInit: no associated Search Tree\n");
    return TCL_ERROR;
  }
  smemP->spassP = NULL;

  /* initialize the maximum request, but the right value is
     known only in the spass object
  */

  smemP->morphMax   = UCHAR_MAX-1;
  smemP->c.blkSize  =  4000;
  smemP->f.blkSize  =  4000 * sizeof(float);
  smemP->p.blkSize  =  4000 * sizeof(void*);
  smemP->r.blkSize  =  1000 * sizeof(RTok);
  smemP->n.blkSize  = 12000 * sizeof(NTok);
  smemP->ri.blkSize =   400 * sizeof(RIce);
  smemP->ni.blkSize =  8000 * sizeof(NIce);
  smemP->li.blkSize =   800 * sizeof(LIce);

  smemP->c.blk  = bmemCreate(smemP->c.blkSize,0);
  smemP->f.blk  = bmemCreate(smemP->f.blkSize,0);
  smemP->p.blk  = bmemCreate(smemP->p.blkSize,0);
  smemP->r.blk  = bmemCreate(smemP->r.blkSize,0);
  smemP->n.blk  = bmemCreate(smemP->n.blkSize,0);
  smemP->ri.blk = bmemCreate(smemP->ri.blkSize,0);
  smemP->ni.blk = bmemCreate(smemP->ni.blkSize,0);
  smemP->li.blk = bmemCreate(smemP->li.blkSize,0); 

  /* user addresses */
  smemP->user.N      = 0;
  smemP->user.allocN = 1000;
  if (NULL == (smemP->user.PA = (SNode**) 
	       malloc(smemP->user.allocN*sizeof(SNode*)))) {
    ERROR("smemInit: allocation failure\n");
    return TCL_ERROR;
  }

  return TCL_OK;
}

/* ------------------------------------------------------------------------
    smemFree
   ------------------------------------------------------------------------ */

int smemDeinit(SMem* smemP)
{
  if (smemP == NULL) {
    WARN("smemDeinit: attempted to free a non-existing SMem object");
    return TCL_ERROR;
  }

  bmemFree(smemP->c.blk);
  bmemFree(smemP->f.blk);
  bmemFree(smemP->p.blk);
  bmemFree(smemP->r.blk);
  bmemFree(smemP->n.blk);
  bmemFree(smemP->ri.blk);
  bmemFree(smemP->ni.blk);
  bmemFree(smemP->li.blk);

  /* free user addesses */
  if (smemP->user.PA) free(smemP->user.PA);

  smemP->streeP = NULL;
  smemP->spassP = NULL;

  return TCL_OK;
}

/* ------------------------------------------------------------------------
    smemClear
   ------------------------------------------------------------------------ */

int smemClear(SMem* smemP)
{
  int userX=0, rootX=0, userN=0;

  if (smemP == NULL) {
    WARN("smemClear: attempted to clear a non-existing SMem object");
    return TCL_ERROR;
  }
  bmemClear(smemP->c.blk);
  bmemClear(smemP->f.blk);
  bmemClear(smemP->p.blk);
  bmemClear(smemP->r.blk);
  bmemClear(smemP->n.blk);
  bmemClear(smemP->ri.blk);
  bmemClear(smemP->ni.blk);
  bmemClear(smemP->li.blk);

  /* clear userN addresses */ 
  userN = smemP->user.N;
  smemP->user.N = 0;

  for (userX=0;userX<userN;userX++) {
    SNode* snodeP = smemP->user.PA[userX];
    snodeP->morphN     = 0;
    snodeP->allocN     = 0;
    snodeP->bestX      = LPM_NIL;
    snodeP->worstX     = LPM_NIL;
    snodeP->iceA.n     = 0;
  }
  for (userX=0;userX<userN;userX++) {
    SNode* snodeP = smemP->user.PA[userX];
    if (snodeP->childN > 0 && snodeP->level < smemP->level) {
      snodeP->allocN  = smemP->morphBlkN;
      smemAllocNode(snodeP,smemP->streeP);
    }
  }

  /* clear roots */
  for (rootX=0;rootX<smemP->streeP->rootTbl.N;rootX++) {
    SRoot* srootP = smemP->streeP->rootTbl.PA[rootX];
    srootP->morphN     = 0;
    srootP->allocN     = 0;
    srootP->bestX      = LPM_NIL;
    srootP->worstX     = LPM_NIL;
    srootP->iceA       = NULL;
    if (smemP->level >=0) {
      srootP->allocN  = smemP->morphBlkN;
      smemAllocRoot(srootP,smemP->streeP);
    }
  }

  return TCL_OK;
}

/* ------------------------------------------------------------------------
    smemGetLPM
   ------------------------------------------------------------------------ */

LPM* smemGetLPM(SMem* smemP, int lpmN)
{
  int x;
  LPM *res = bmemAlloc(smemP->c.blk,sizeof(LPM)*lpmN);
  for (x=0;x<lpmN;x++)
    res[x] = LPM_NIL;

  return res;
}

/* ------------------------------------------------------------------------
    smemGetFloat
   ------------------------------------------------------------------------ */

#if USE_RCM_LM
static float* smemGetFloat(SMem* smemP, int floatN)
{
  int floatX;
  float *res = bmemAlloc(smemP->f.blk,sizeof(float)*floatN);
  for (floatX=0;floatX<floatN;floatX++)
    res[floatX]= SCORE_MAX;
  return res;
}
#endif

/* ------------------------------------------------------------------------
    smemGetPtr
   ------------------------------------------------------------------------ */

static void* smemGetPtr(SMem* smemP, int ptrN)
{
  void **res = bmemAlloc(smemP->p.blk,sizeof(void*)*ptrN);
  return res;
}

/* ------------------------------------------------------------------------
    smemGetNTok
   ------------------------------------------------------------------------ */

static NTok* smemGetNTok(SMem* smemP, int tokN)
{
  int tokX;
  NTok *res = bmemAlloc(smemP->n.blk,sizeof(NTok)*tokN);
  for (tokX=0;tokX<tokN;tokX++)
    res[tokX].s = SCORE_MAX;

  return res;
}

/* ------------------------------------------------------------------------
    smemGetRTok
   ------------------------------------------------------------------------ */

static RTok* smemGetRTok(SMem* smemP, int tokN)
{
  int tokX;
  RTok *res = bmemAlloc(smemP->r.blk,sizeof(RTok)*tokN);
  for (tokX=0;tokX<tokN;tokX++) {
    res[tokX].s   = SCORE_MAX;
    res[tokX].hmmX = 0;
  }
  return res;
}

/* ------------------------------------------------------------------------
    smemGetRIce
   ------------------------------------------------------------------------ */

static RIce* smemGetRIce(SMem* smemP,int iceN)
{
  RIce *res = bmemAlloc(smemP->ri.blk,iceN*sizeof(RIce));
  return res;
}

/* ------------------------------------------------------------------------
    smemGetNIce
   ------------------------------------------------------------------------ */

static NIce* smemGetNIce(SMem* smemP,int iceN)
{
  NIce *res = bmemAlloc(smemP->ni.blk,iceN*sizeof(NIce));
  return res;
}

/* ------------------------------------------------------------------------
    smemGetLIce
   ------------------------------------------------------------------------ */

static LIce* smemGetLIce(SMem* smemP,int iceN)
{
  LIce *res = bmemAlloc(smemP->li.blk,iceN*sizeof(LIce));
  return res;
}


/* ------------------------------------------------------------------------
    smemAllocRoot
   ------------------------------------------------------------------------ */

int smemAllocRoot(SRoot* srootP, STree* streeP)
{
  LCM*      lcmP = streeP->lcmsP->list.itemA + srootP->xhmmX;
  PHMM*     hmmP = streeP->hmmsP->list.itemA + lcmP->hmmXA[0];
  SMem*    smemP = &(streeP->smem);
  int     stateN = hmmP->stateN+1;
  int     morphN = srootP->morphN;
  int     allocN = srootP->allocN;
  RIce**     ptr = NULL;
  int morphX;
  
  /* --------------------------------- */
  /* get position mapper            */
  /* --------------------------------- */
  if (!srootP->lct2pos) {
    if (NULL == (srootP->lct2pos = (LPM*) 
		 malloc(streeP->lct2pos.hashSize*sizeof(LPM)))) {
      ERROR("smemAllocRoot: allocation failure\n");
      return TCL_ERROR;
    }
    for (morphX=0;morphX<streeP->lct2pos.hashSize;morphX++) {
      srootP->lct2pos[morphX]= LPM_NIL;
    }
  }
  if (allocN == 0) return TCL_OK;

  /* make sure that the malloc request match to the 2*blocksize */
  allocN = (2*smemP->morphBlkN) * ceil(1.0*srootP->allocN / (2*smemP->morphBlkN));
  if (allocN > LPM_MAX) allocN = LPM_MAX;
  srootP->allocN = allocN;

  /* --------------------------------- */
  /* 1. update sroot pointer array     */
  /* --------------------------------- */

  ptr = (RIce**) smemGetPtr(smemP,allocN);
  if (morphN > 0) {
    memcpy(ptr,srootP->iceA,morphN*sizeof(void*));
    if (smemP->smemFree)
      bmemDealloc(smemP->p.blk,srootP->iceA,morphN*sizeof(void*));
  }
  srootP->iceA = ptr;

  /* --------------------------------- */
  /* 2. get root instances             */
  /* --------------------------------- */
  srootP->iceA[morphN] = smemGetRIce(smemP,allocN-morphN);

  for (morphX=morphN+1;morphX<allocN;morphX++) {
    srootP->iceA[morphX] = srootP->iceA[morphX-1] + 1;
  }
  for (morphX=morphN; morphX<allocN;morphX++) {
    srootP->iceA[morphX]->path = smemGetRTok(smemP,stateN);
  }

  return TCL_OK;
}

/* ------------------------------------------------------------------------
    smemAllocNode
   ------------------------------------------------------------------------ */

/* We use a memory allocation on demand. To avoid real malloc/realloc 
   calls during decoding, we use preallocated memblocks for Node 
   instances organized in the SMem object. To speed up the heap 
   operations during decoding, we need pointer of instances.
*/

int smemAllocNode(SNode* snodeP, STree* streeP)
{
  SMem*    smemP = &(streeP->smem);
  PHMM*     hmmP = streeP->hmmsP->list.itemA + snodeP->X.hmm;
  int     stateN = hmmP->stateN+1;
  int     morphN = snodeP->morphN;
  int     allocN = snodeP->allocN;
  NIce**     ptr = NULL;

  int morphX;
  
  /* should never happen */
  assert(allocN  > morphN);
  assert(allocN  > 0);

  /* make sure that the malloc request match to the blocksize */
  allocN = smemP->morphBlkN * ceil(1.0*snodeP->allocN / smemP->morphBlkN);
  if (allocN > smemP->morphMax) {
    allocN =  smemP->morphMax;
  }
  snodeP->allocN = allocN;

  /* --------------------------------- */
  /* 1. add the snode address          */
  /* --------------------------------- */

  /* add the snode address */
  if (morphN == 0) {
    if (smemP->user.N ==  smemP->user.allocN) {
      smemP->user.allocN += 1000;
      if (NULL == (smemP->user.PA = (SNode**) realloc(
	smemP->user.PA,smemP->user.allocN*sizeof(SNode*)))) {
	ERROR("smemAllocNode: allocation failure\n");
	return TCL_ERROR;
      }
    }
    smemP->user.PA[smemP->user.N] = snodeP;
    smemP->user.N++;
  } 

  /* --------------------------------- */
  /* 2. update snode pointer array     */
  /* --------------------------------- */

  ptr = (NIce**) smemGetPtr(smemP,allocN);
  if (morphN > 0) {
    memcpy(ptr,snodeP->iceA.n,morphN*sizeof(void*));
    if (smemP->smemFree)
      bmemDealloc(smemP->p.blk,snodeP->iceA.n,morphN*sizeof(void*));
  }
  snodeP->iceA.n = ptr;


  /* --------------------------------- */
  /* 3. get node instances             */
  /* --------------------------------- */
  snodeP->iceA.n[morphN] = smemGetNIce(smemP,allocN-morphN);

  for (morphX=morphN+1;morphX<allocN;morphX++) {
    snodeP->iceA.n[morphX] = snodeP->iceA.n[morphX-1] + 1;
  }
  for (morphX=morphN; morphX<allocN;morphX++) {
    snodeP->iceA.n[morphX]->path = smemGetNTok(smemP,stateN);
  }

  return TCL_OK;
}

/* ------------------------------------------------------------------------
    smemAllocLeaf
   ------------------------------------------------------------------------ */

int smemAllocLeaf(SNode* sleafP, STree* streeP)
{
  SMem*    smemP = &(streeP->smem);
  RCM*      rcmP = streeP->rcmsP->list.itemA + sleafP->X.xhmm;
  PHMM*     hmmP = streeP->hmmsP->list.itemA + rcmP->hmmXA[0];
  int       hmmN = rcmP->hmmN;
  int     stateN = hmmP->stateN+1;
  int     morphN = sleafP->morphN;
  int     allocN = sleafP->allocN;
  LIce**     ptr = NULL;

  int morphX;

  /* should never happen */
  assert(allocN  > morphN);
  assert(allocN  > 0);
  assert(hmmN    > 0);

  /* make sure that the malloc request match to the blocksize */
  allocN = smemP->morphBlkN * ceil(1.0*sleafP->allocN / smemP->morphBlkN);
  if (allocN > smemP->morphMax) {
    allocN =  smemP->morphMax;
  }
  sleafP->allocN = allocN;

  /* --------------------------------- */
  /* 1. add the sleaf address          */
  /* --------------------------------- */

  if (morphN == 0) {
    if (smemP->user.N ==  smemP->user.allocN) {
      smemP->user.allocN += 1000;
      if (NULL == (smemP->user.PA = (SNode**) realloc(
	smemP->user.PA,smemP->user.allocN*sizeof(SNode*)))) {
	ERROR("smemAllocLeaf: allocation failure\n");
	return TCL_ERROR;
      }
    }
    smemP->user.PA[smemP->user.N] = sleafP;
    smemP->user.N++;
  } 

  /* --------------------------------- */
  /* 2. update sleaf pointer array     */
  /* --------------------------------- */

  ptr = (LIce**) smemGetPtr(smemP,allocN);
  if (morphN > 0) {
    memcpy(ptr,sleafP->iceA.l,morphN*sizeof(void*));
    if (smemP->smemFree)
      bmemDealloc(smemP->p.blk,sleafP->iceA.l,morphN*sizeof(void*));
  }
  sleafP->iceA.l = ptr;

  /* --------------------------------- */
  /* 3. store leaf instances           */
  /* --------------------------------- */
  sleafP->iceA.l[morphN] = smemGetLIce(smemP,allocN-morphN);

  for (morphX=morphN+1; morphX<allocN;morphX++) {
    sleafP->iceA.l[morphX] = sleafP->iceA.l[morphX-1]+1;
  }

  for (morphX=morphN; morphX<allocN;morphX++) {
    sleafP->iceA.l[morphX]->path = smemGetNTok(smemP,hmmN*stateN);
#if USE_RCM_LM
    sleafP->iceA.l[morphX]->lmp  = smemGetFloat(smemP,hmmN+1);
#endif
  }

  return TCL_OK;
}

/* ------------------------------------------------------------------------
    smemAllocSIPhone
   ------------------------------------------------------------------------ */

int smemAllocSIPhone(SIPhone* sipP, STree* streeP)
{
  PHMM*     hmmP = streeP->hmmsP->list.itemA + sipP->hmmX;
  int     stateN = hmmP->stateN+1;
  int     morphN = sipP->morphN;
  int     allocN = sipP->allocN;  
  int stateX, morphX;
  
  /* should never happen */
  assert(allocN  > morphN);
  assert(allocN  > 0);
  if (allocN > LPM_MAX) {
    allocN = LPM_MAX;
    sipP->allocN = allocN;
  }
  if (NULL == (sipP->iceA = (NIce**) 
	       realloc(sipP->iceA,allocN*sizeof(NIce*)))) {
    ERROR("smemAllocSIPhone: allocation failure\n");
    return TCL_ERROR;
  }
  for (morphX=morphN;morphX<allocN;morphX++) {
    if (NULL == (sipP->iceA[morphX] = (NIce*) malloc(sizeof(NIce)))) {
      ERROR("smemAllocSIPhone: allocation failure\n");
      return TCL_ERROR;
    }
    if (NULL == (sipP->iceA[morphX]->path = 
		 malloc(stateN*sizeof(NTok))))  {
      ERROR("smemAllocSIPhone: allocation failure\n");
      return TCL_ERROR;
    }
    for (stateX=0;stateX<stateN;stateX++) {
      sipP->iceA[morphX]->path[stateX].s = SCORE_MAX;
    }
  }
  if (morphN == 0) {
    if (NULL == (sipP->lct2pos = (LPM*) 
		 malloc(streeP->lct2pos.hashSize*sizeof(LPM)))) {
      ERROR("smemAllocSIPhone: allocation failure\n");
      return TCL_ERROR;
    }
    for (morphX=0;morphX<streeP->lct2pos.hashSize;morphX++)
      sipP->lct2pos[morphX]= LPM_NIL;
  }

  return TCL_OK;
}

/* ------------------------------------------------------------------------
    smemAllocSDPhone
   ------------------------------------------------------------------------ */

int smemAllocSDPhone(SDPhone* sdpP, STree* streeP)
{
  XCM*      xcmP = streeP->xcmsP->list.itemA + sdpP->hmmX;
  PHMM*     hmmP = streeP->hmmsP->list.itemA + xcmP->hmmXA[0][0];
  int     stateN = (hmmP->stateN+1)*xcmP->hmmMax;
  int     morphN = sdpP->morphN;
  int     allocN = sdpP->allocN;  
  int stateX, morphX;
  
  /* should never happen */
  assert(allocN  > morphN);
  assert(allocN  > 0);

  if (NULL == (sdpP->iceA = (XIce**) 
	       realloc(sdpP->iceA,allocN*sizeof(XIce*)))) {
    ERROR("smemAllocSDPhone: allocation failure\n");
    return TCL_ERROR;
  }
  for (morphX=morphN;morphX<allocN;morphX++) {
    if (NULL == (sdpP->iceA[morphX] = (XIce*) malloc(sizeof(XIce)))) {
      ERROR("smemAllocSDPhone: allocation failure\n");
      return TCL_ERROR;
    }
    if (NULL == (sdpP->iceA[morphX]->path = 
		 malloc(stateN*sizeof(NTok))))  {
      ERROR("smemAllocSDPhone: allocation failure\n");
      return TCL_ERROR;
    }
    for (stateX=0;stateX<stateN;stateX++) {
      sdpP->iceA[morphX]->path[stateX].s = SCORE_MAX;
    }
  }
  if (morphN == 0) {
    if (NULL == (sdpP->lct2pos = (LPM*) 
		 malloc(streeP->lct2pos.hashSize*sizeof(LPM)))) {
      ERROR("smemAllocSDPhone: allocation failure\n");
      return TCL_ERROR;
    }
    for (morphX=0;morphX<streeP->lct2pos.hashSize;morphX++)
      sdpP->lct2pos[morphX]= LPM_NIL;
  }

  return TCL_OK;
}

/* ------------------------------------------------------------------------
    smemFreeRoot
   ------------------------------------------------------------------------ */

int smemFreeRoot(SRoot* srootP, STree* streeP)
{
  SPass* spassP = streeP->smem.spassP;
  
  if (srootP->lct2pos)
    free(srootP->lct2pos);

  srootP->iceA    = NULL;
  srootP->lct2pos = NULL;
  srootP->morphN  = 0;
  srootP->allocN  = 0;

  if (srootP->allocN == 0) return TCL_OK;

  /* delete root from the active root list */
  if (spassP) {
    int rootX=0;
    while (rootX < spassP->active.rootN && 
	   spassP->active.rootPA[rootX] != srootP)
      rootX++;
    if (rootX < spassP->active.rootN) {
      spassP->active.rootN--;
      spassP->active.rootPA[rootX] = 
	spassP->active.rootPA[spassP->active.rootN];
    }
    rootX=0;
    while (rootX < spassP->actNext.rootN && 
	   spassP->actNext.rootPA[rootX] != srootP)
      rootX++;
    if (rootX < spassP->actNext.rootN) {
      spassP->actNext.rootN--;
      spassP->actNext.rootPA[rootX] = 
	spassP->actNext.rootPA[spassP->actNext.rootN];
    }
  }
  return TCL_OK;
}

/* ------------------------------------------------------------------------
    smemFreeNode
   ------------------------------------------------------------------------ */

int smemFreeNode(SNode* snodeP, STree* streeP)
{
  SMem*    smemP = &(streeP->smem);
  SPass*  spassP = smemP->spassP;
  int userX;

  /* delete snode from the user list */
  userX=0;
  while (userX < smemP->user.N && smemP->user.PA[userX] != snodeP) 
    userX++;
  if (userX < smemP->user.N) {
    smemP->user.N--;
    smemP->user.PA[userX] = smemP->user.PA[smemP->user.N];
  }

  /* delete node from the active node list */
  if (spassP) {
    int nodeX=0;
    while (nodeX < spassP->active.nodeN && 
	   spassP->active.nodePA[nodeX] != snodeP)
      nodeX++;
    if (nodeX < spassP->active.nodeN) {
      spassP->active.nodeN--;
      spassP->active.nodePA[nodeX] = 
	spassP->active.nodePA[spassP->active.nodeN];
    }
    nodeX=0;
    while (nodeX < spassP->actNext.nodeN && 
	   spassP->actNext.nodePA[nodeX] != snodeP)
      nodeX++;
    if (nodeX < spassP->actNext.nodeN) {
      spassP->actNext.nodeN--;
      spassP->actNext.nodePA[nodeX] = 
	spassP->actNext.nodePA[spassP->actNext.nodeN];
    }
  }

  snodeP->iceA.n  = NULL;
  snodeP->morphN  = 0;
  snodeP->allocN  = 0;

  return TCL_OK;
}

/* ------------------------------------------------------------------------
    smemFreeLeaf
   ------------------------------------------------------------------------ */

int smemFreeLeaf(SNode* sleafP, STree* streeP)
{
  SMem*    smemP = &(streeP->smem);
  SPass*  spassP = smemP->spassP;
  int userX;

  /* delete snode from the user list */
  userX=0;
  while (userX < smemP->user.N && smemP->user.PA[userX] != sleafP) 
    userX++;
  if (userX < smemP->user.N) {
    smemP->user.N--;
    smemP->user.PA[userX] = smemP->user.PA[smemP->user.N];
  }

  /* delete leaf from the active leaf list */
  if (spassP) {
    int leafX=0;
    while (leafX < spassP->active.leafN && 
	   spassP->active.leafPA[leafX] != sleafP)
      leafX++;
    if (leafX < spassP->active.leafN) {
      spassP->active.leafN--;
      spassP->active.leafPA[leafX] = 
	spassP->active.leafPA[spassP->active.leafN];
    }
    leafX=0;
    while (leafX < spassP->actNext.leafN && 
	   spassP->actNext.leafPA[leafX] != sleafP)
      leafX++;
    if (leafX < spassP->actNext.leafN) {
      spassP->actNext.leafN--;
      spassP->actNext.leafPA[leafX] = 
	spassP->actNext.leafPA[spassP->actNext.leafN];
    }
  }

  sleafP->iceA.l  = NULL;
  sleafP->morphN  = 0;
  sleafP->allocN  = 0;

  return TCL_OK;
}

/* ------------------------------------------------------------------------
    smemFreeSIPhone
   ------------------------------------------------------------------------ */

int smemFreeSIPhone(SIPhone* sipP, STree* streeP)
{
  int allocN = sipP->allocN;
  int morphX;
  
  if (sipP->allocN == 0) return TCL_OK;

  if (sipP->iceA) {
    for (morphX=0;morphX<allocN;morphX++) {
      if (sipP->iceA[morphX]->path) 
	free(sipP->iceA[morphX]->path);
      if (sipP->iceA[morphX])
	free(sipP->iceA[morphX]);
    }
    free(sipP->iceA);
  }

  if (sipP->lct2pos)
    free(sipP->lct2pos);

  sipP->iceA    = NULL;
  sipP->lct2pos = NULL;
  sipP->morphN  = 0;
  sipP->allocN  = 0;

  return TCL_OK;
}

/* ------------------------------------------------------------------------
    smemFreeSDPhone
   ------------------------------------------------------------------------ */

int smemFreeSDPhone(SDPhone* sdpP, STree* streeP)
{
  int allocN = sdpP->allocN;
  int morphX;
  
  if (sdpP->allocN == 0) return TCL_OK;

  if (sdpP->iceA) {
    for (morphX=0;morphX<allocN;morphX++) {
      if (sdpP->iceA[morphX]->path) 
	free(sdpP->iceA[morphX]->path);
      if (sdpP->iceA[morphX])
	free(sdpP->iceA[morphX]);
    }
    free(sdpP->iceA);
  }

  if (sdpP->lct2pos)
    free(sdpP->lct2pos);

  sdpP->iceA    = NULL;
  sdpP->lct2pos = NULL;
  sdpP->morphN  = 0;
  sdpP->allocN  = 0;

  return TCL_OK;
}

/* ------------------------------------------------------------------------
    smemDeallocRoot
   ------------------------------------------------------------------------ */

static void riceQSort(RIce** iceA, int left, int right)
{
  RIce*  iceP = NULL;
  int     mid = (left+right)/2;
  int i,last;

  if (left >= right) return;
  iceP = iceA[left];

  iceA[left] = iceA[mid];
  iceA[mid]  = iceP;

  last = left;
  for (i=left+1;i<=right;i++) {
    if (iceA[i] < iceA[left]) {
      last++;
      iceP       = iceA[last];
      iceA[last] = iceA[i];
      iceA[i]    = iceP;
    }
  }
  iceP       = iceA[left];
  iceA[left] = iceA[last];
  iceA[last] = iceP;

  riceQSort(iceA,left,last-1);
  riceQSort(iceA,last+1,right);
  return;
}


int smemDeallocRoot(SRoot* srootP, STree* streeP)
{
  SMem*    smemP = &(streeP->smem);
  LCM*      lcmP = streeP->lcmsP->list.itemA + srootP->xhmmX;
  PHMM*     hmmP = streeP->hmmsP->list.itemA + lcmP->hmmXA[0];
  int     stateN = hmmP->stateN+1;
  int     allocN = srootP->allocN;
  int      size1 = smemP->morphBlkN;
  int      size2 = allocN - size1 *(allocN/size1);

  int morphX;

  /* nothing to do */
  if (allocN == 0) return TCL_OK;

  /* free node token */
  for (morphX=0;morphX<allocN;morphX++) {
    bmemDealloc(smemP->r.blk,srootP->iceA[morphX]->path,stateN*sizeof(RTok)); 
  }

  /* we have to sort the list of instances according to their pointer */
  riceQSort(srootP->iceA,0,allocN-1);

  /* free root instances */
  morphX=0;
  while (morphX < allocN) {
    if (morphX+size1 <= allocN &&
	srootP->iceA[morphX]+size1-1 == srootP->iceA[morphX+size1-1]) {
      bmemDealloc(smemP->ri.blk,srootP->iceA[morphX],size1*sizeof(RIce));
      morphX += size1;
    } else if (size2> 0 && morphX+size2 <= allocN &&
	srootP->iceA[morphX]+size2-1 == srootP->iceA[morphX+size2-1]) {
      bmemDealloc(smemP->ri.blk,srootP->iceA[morphX],size2*sizeof(RIce));
      morphX += size2;
    } else {
      assert (morphX+size2 > allocN);
      break;
    }
  }

  /* free pointer of instances array */
  bmemDealloc(smemP->p.blk,srootP->iceA,allocN*sizeof(void*));
  srootP->iceA   = NULL;
  srootP->morphN = 0;
  srootP->allocN = 0;

  return TCL_OK;
}

/* ------------------------------------------------------------------------
    smemDeallocNode
   ------------------------------------------------------------------------ */

static void niceQSort(NIce** iceA, int left, int right)
{
  NIce*  iceP = NULL;
  int     mid = (left+right)/2;
  int i,last;

  if (left >= right) return;
  iceP = iceA[left];

  iceA[left] = iceA[mid];
  iceA[mid]  = iceP;

  last = left;
  for (i=left+1;i<=right;i++) {
    if (iceA[i] < iceA[left]) {
      last++;
      iceP       = iceA[last];
      iceA[last] = iceA[i];
      iceA[i]    = iceP;
    }
  }
  iceP       = iceA[left];
  iceA[left] = iceA[last];
  iceA[last] = iceP;

  niceQSort(iceA,left,last-1);
  niceQSort(iceA,last+1,right);
  return;
}


int smemDeallocNode(SNode* snodeP, STree* streeP)
{
  SMem*    smemP = &(streeP->smem);
  PHMM*     hmmP = streeP->hmmsP->list.itemA + snodeP->X.hmm;
  int     stateN = hmmP->stateN+1;
  int     allocN = snodeP->allocN;
  int      size1 = smemP->morphBlkN;
  int      size2 = allocN - size1 *(allocN/size1);

  int morphX;

  /* nothing to do */
  if (allocN == 0) return TCL_OK;

  /* free node token */
  for (morphX=0;morphX<allocN;morphX++) {
    bmemDealloc(smemP->n.blk,snodeP->iceA.n[morphX]->path,stateN*sizeof(NTok)); 
  }

  /* we have to sort the list of instances according to their pointer */
  niceQSort(snodeP->iceA.n,0,allocN-1);

  /* free node instances */
  morphX=0;
  while (morphX < allocN) {
    if (morphX+size1 <= allocN &&
	snodeP->iceA.n[morphX]+size1-1 == snodeP->iceA.n[morphX+size1-1]) {
      bmemDealloc(smemP->ni.blk,snodeP->iceA.n[morphX],size1*sizeof(NIce));
      morphX += size1;
    } else if (size2> 0 && morphX+size2 <= allocN &&
	snodeP->iceA.n[morphX]+size2-1 == snodeP->iceA.n[morphX+size2-1]) {
      bmemDealloc(smemP->ni.blk,snodeP->iceA.n[morphX],size2*sizeof(NIce));
      morphX += size2;
    } else {
      assert (morphX+size2 > allocN);
      break;
    }
  }

  /* free pointer of instances array */
  bmemDealloc(smemP->p.blk,snodeP->iceA.n,allocN*sizeof(void*));
  snodeP->iceA.n = NULL;

  snodeP->morphN = 0;
  snodeP->allocN = 0;

  return TCL_OK;
}

/* ------------------------------------------------------------------------
    smemDeallocLeaf
   ------------------------------------------------------------------------ */

static void liceQSort(LIce** iceA, int left, int right)
{
  LIce*  iceP = NULL;
  int     mid = (left+right)/2;
  int i,last;

  if (left >= right) return;
  iceP = iceA[left];

  iceA[left] = iceA[mid];
  iceA[mid]  = iceP;

  last = left;
  for (i=left+1;i<=right;i++) {
    if (iceA[i] < iceA[left]) {
      last++;
      iceP       = iceA[last];
      iceA[last] = iceA[i];
      iceA[i]    = iceP;
    }
  }
  iceP       = iceA[left];
  iceA[left] = iceA[last];
  iceA[last] = iceP;

  liceQSort(iceA,left,last-1);
  liceQSort(iceA,last+1,right);
  return;
}


int smemDeallocLeaf(SNode* sleafP, STree* streeP)
{
  SMem*    smemP = &(streeP->smem);
  RCM*      rcmP = streeP->rcmsP->list.itemA + sleafP->X.xhmm;
  PHMM*     hmmP = streeP->hmmsP->list.itemA + rcmP->hmmXA[0];
  int       hmmN = rcmP->hmmN;
  int     stateN = hmmP->stateN+1;
  int     allocN = sleafP->allocN;
  int      size1 = smemP->morphBlkN;
  int      size2 = allocN - size1 *(allocN/size1);

  int morphX;

  /* nothing to do */
  if (allocN == 0) return TCL_OK;

  /* free node token */
  for (morphX=0;morphX<allocN;morphX++) {
    bmemDealloc(smemP->n.blk,sleafP->iceA.l[morphX]->path,stateN*hmmN*sizeof(NTok)); 
  }

  /* we have to sort the list of instances according to their pointer */
  liceQSort(sleafP->iceA.l,0,allocN-1);

  /* free node instances */
  morphX=0;
  while (morphX < allocN) {
    if (morphX+size1 <= allocN &&
	sleafP->iceA.l[morphX]+size1-1 == sleafP->iceA.l[morphX+size1-1]) {
      bmemDealloc(smemP->li.blk,sleafP->iceA.l[morphX],size1*sizeof(LIce));
      morphX += size1;
    } else if (size2> 0 && morphX+size2 <= allocN &&
	sleafP->iceA.l[morphX]+size2-1 == sleafP->iceA.l[morphX+size2-1]) {
      bmemDealloc(smemP->li.blk,sleafP->iceA.l[morphX],size2*sizeof(LIce));
      morphX += size2;
    } else {
      assert (morphX+size2 > allocN);
      break;
    }
  }

  /* free pointer of instances array */
  bmemDealloc(smemP->p.blk,sleafP->iceA.l,allocN*sizeof(void*));
  sleafP->iceA.l = NULL;

  sleafP->morphN = 0;
  sleafP->allocN = 0;

  return TCL_OK;
}

/* ------------------------------------------------------------------------
    smemPuts
   ------------------------------------------------------------------------ */

static int smemPutsItf (ClientData cd, int argc, char *argv[]) { 
  return TCL_OK;
}

/* ------------------------------------------------------------------------
    smemConfigure
   ------------------------------------------------------------------------ */

static int smemConfigureItf(ClientData cd, char *var, char *val) {
  SMem* smemP   = (SMem*)cd; 
  if (! smemP) return TCL_ERROR;
  if (! var) {
    ITFCFG(smemConfigureItf,cd,"level");
    ITFCFG(smemConfigureItf,cd,"smemFree");
    ITFCFG(smemConfigureItf,cd,"morphBlk");
    return TCL_OK;
  }
  ITFCFG_Int(var,val,"level",   smemP->level,    1);
  ITFCFG_Int(var,val,"smemFree",smemP->smemFree, 0);

  if (! strcmp(var,"morphBlk")) {
    int morphBlk = (val) ?  atoi(val) : smemP->morphBlkN;
    if (morphBlk != smemP->morphBlkN && smemP->smemFree && smemP->user.N > 0) {
      ERROR("SMem: cannot change block size during decoding\n");
      morphBlk = smemP->morphBlkN;
      itfAppendResult("%d ",morphBlk); 
      return TCL_OK; 
    }
    smemP->morphBlkN = morphBlk;
    itfAppendResult("%d ",morphBlk); 
    return TCL_OK; 
  }

  ERROR("unknown option '-%s %s'.\n", var, val ? val : "?");
  return TCL_ERROR;   
}

/* ------------------------------------------------------------------------
    smemAccess
   ------------------------------------------------------------------------ */

static ClientData smemAccessItf(ClientData cd, char *name, TypeInfo **ti)
{
  SMem* smemP   = (SMem*)cd; 
  if (! smemP) return NULL;

 if (*name == '.') { 
    if (name[1]=='\0') {
      itfAppendElement( "c");
      itfAppendElement( "f");
      itfAppendElement( "p");
      itfAppendElement( "r");
      itfAppendElement( "ri");
      itfAppendElement( "n");
      itfAppendElement( "ni");
      itfAppendElement( "li");
      *ti = NULL; return NULL;
    }
    else { 
      if (! strcmp( name+1, "c")) {
        *ti = itfGetType("BMem");
        return (ClientData)smemP->c.blk;
      }
      if (! strcmp( name+1, "f")) {
        *ti = itfGetType("BMem");
        return (ClientData)smemP->f.blk;
      }
      if (! strcmp( name+1, "p")) {
        *ti = itfGetType("BMem");
        return (ClientData)smemP->p.blk;
      }
      if (! strcmp( name+1, "r")) {
        *ti = itfGetType("BMem");
        return (ClientData)smemP->r.blk;
      }
      if (! strcmp( name+1, "ri")) {
        *ti = itfGetType("BMem");
        return (ClientData)smemP->ri.blk;
      }
      if (! strcmp( name+1, "n")) {
        *ti = itfGetType("BMem");
        return (ClientData)smemP->n.blk;
      }
      if (! strcmp( name+1, "ni")) {
        *ti = itfGetType("BMem");
        return (ClientData)smemP->ni.blk;
      }
      if (! strcmp( name+1, "li")) {
        *ti = itfGetType("BMem");
        return (ClientData)smemP->li.blk;
      }
    }
  }
 return NULL;
}

/* ========================================================================
    Type Information
   ======================================================================== */

Method smemMethod[] = 
{
  { "puts", smemPutsItf,   "displays the contents of a memory manager"  },
  { NULL, NULL, NULL }
} ;

TypeInfo smemInfo = 
{ "SMem", 0, -1,  smemMethod, 
   NULL, NULL, 
   smemConfigureItf, smemAccessItf, NULL,
  "Search Memory Manager"
};

static int smemInitialized = 0;

int SMem_Init (void)
{
  if (! smemInitialized) {
     if (BMem_Init() != TCL_OK) return TCL_ERROR;

    itfNewType (&smemInfo);
    smemInitialized = 1;
  }
  return TCL_OK;
}
