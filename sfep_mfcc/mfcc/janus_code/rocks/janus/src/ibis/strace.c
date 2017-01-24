/* ========================================================================
    JANUS      Speech- to Speech Translation
               ------------------------------------------------------------
               Object: SPASS Backpointer Table
               ------------------------------------------------------------

    Author  :  Hagen Soltau
    Module  :  strace.c
    Date    :  $Id: strace.c 2894 2009-07-27 15:55:07Z metze $
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
    Revision 4.8  2007/03/22 09:49:00  fuegen
    removed bug in stabFinalize

    Revision 4.7  2007/03/19 15:59:17  fuegen
    bugfixes for traceStable

    Revision 4.6  2006/11/15 11:59:25  fuegen
    removed debugging output in stabTraceStable

    Revision 4.5  2006/11/14 16:41:44  fuegen
    support for endless decoding with partial hypothesis
    - bugfix for spass.pathSub method, which subtracts a constant score
      from all active paths
    - new method spass.traceStable which returns the stable hypotheses
      together with a list of backpointers for the next traceStable call

    Revision 4.4  2003/11/06 17:43:22  metze
    bpN initialized

    Revision 4.3  2003/09/05 15:34:02  soltau
    stabTrace : write best bpX again

    Revision 4.2  2003/09/01 14:51:29  metze
    Added -frameIdx option to STrace

    Revision 4.1  2003/08/14 11:20:09  fuegen
    Merged changes on branch jtk-01-01-15-fms (jaguar -> ibis-013)

    Revision 1.1.2.33  2003/08/13 08:59:46  fuegen
    added stabGetFinalBPX to get the final BP

    Revision 1.1.2.32  2002/11/13 10:07:01  soltau
    - STab rely now on SVMap directly
    - Start and End words rely on LVX now

    Revision 1.1.2.31  2002/06/26 11:57:54  fuegen
    changes for new directory structure support and code cleaning

    Revision 1.1.2.30  2002/01/29 16:46:09  soltau
    *** empty log message ***

    Revision 1.1.2.29  2002/01/29 16:40:48  soltau
    stabTrace: checking empty stab

    Revision 1.1.2.28  2001/12/07 10:29:44  soltau
    stabTrace: 'improved' reallocation :-)

    Revision 1.1.2.27  2001/12/06 15:39:24  soltau
    beautified

    Revision 1.1.2.26  2001/11/23 09:32:21  soltau
    removed (debug?) output in stabFinalize

    Revision 1.1.2.25  2001/10/25 15:01:11  soltau
    beautified

    Revision 1.1.2.24  2001/10/17 16:45:19  soltau
    Deactivated LCT_CHECK

    Revision 1.1.2.23  2001/10/12 17:06:32  soltau
    Improved path recombination

    Revision 1.1.2.22  2001/08/09 17:45:25  soltau
    *** empty log message ***

    Revision 1.1.2.21  2001/08/09 17:24:46  soltau
    Fixed again a couple of stabFinalize problems

    Revision 1.1.2.20  2001/08/08 19:12:32  soltau
    Fixed stabFinalize
    Update frameN in stabPrune

    Revision 1.1.2.19  2001/08/08 14:19:08  metze
    Added '-v 2' option to stab trace

    Revision 1.1.2.18  2001/07/25 11:34:17  soltau
    *** empty log message ***

    Revision 1.1.2.17  2001/07/24 17:11:47  soltau
    Added stabFinalize

    Revision 1.1.2.16  2001/06/20 18:35:38  soltau
    *** empty log message ***

    Revision 1.1.2.15  2001/06/01 10:39:11  soltau
    Added stabGetFrame

    Revision 1.1.2.14  2001/04/27 07:39:15  metze
    General access functions to LM via 'LCT' implemented

    Revision 1.1.2.13  2001/04/19 15:37:23  soltau
    - Changed final frame handling
    - Made run-on working
    - Made partial trace back working

    Revision 1.1.2.12  2001/03/26 06:11:32  soltau
    Added verbose option in stabTrace

    Revision 1.1.2.11  2001/03/22 17:21:38  soltau
    Fixed stabPrune problems if we didn't got any backpointers

    Revision 1.1.2.10  2001/03/20 13:25:39  soltau
    fixed stabAlloc memory overflow

    Revision 1.1.2.9  2001/03/17 17:37:16  soltau
    Removed double allocation in stabInit

    Revision 1.1.2.8  2001/03/16 14:55:37  soltau
    endSVX changes

    Revision 1.1.2.7  2001/03/14 14:41:56  soltau
    Fixed allocation in stabCreate

    Revision 1.1.2.6  2001/03/14 12:21:40  soltau
    Changed interface

    Revision 1.1.2.5  2001/03/13 17:59:09  soltau
    Removed debug output

    Revision 1.1.2.4  2001/03/12 18:49:41  soltau
    Added bpTabClear
    Fixed bpTabPrune (set offset according to pruned bp's)

    Revision 1.1.2.3  2001/02/27 13:50:24  soltau
    Added preliminary traceback function

    Revision 1.1.2.2  2001/02/16 17:48:47  soltau
    Added pruning of backpointer entries

    Revision 1.1.2.1  2001/02/02 16:10:58  soltau
    initial revision


   ======================================================================== */

#include "svocab.h"
#include "svmap.h"
#include "strace.h"


char straceRcsVersion[]= 
    "$Id: strace.c 2894 2009-07-27 15:55:07Z metze $";

extern TypeInfo  stabInfo;
static STab      stabDefault;

/* ========================================================================
    Linguistic state : Hash functions
   ======================================================================== */

static int lkeyInit( LKey* lkeyP, LKey* hashkey)
{
  *lkeyP = *hashkey;
  return TCL_OK;
}

static int lkeyDeinit( LKey* lkeyP)
{
  return TCL_OK;
}

static int lkeyHashCmp( const LKey* hashkey, const LKey* lkeyP)
{
  return (hashkey->lct == lkeyP->lct);
}

#define lkeyBit 8
static long lkeyHashKey( const LKey* key)
{
  unsigned long ret = key->lct; 

  ret = (ret<< (8*sizeof(long)-lkeyBit));
  return ret;
}

/* ------------------------------------------------------------------------
    stabInit
   ------------------------------------------------------------------------ */

static int stabInit(STab* tabP, SVMap* svmapP)
{
  int frameX;

  tabP->blkSize      = stabDefault.blkSize;
  tabP->frameBlkSize = stabDefault.frameBlkSize;

  /* should never happen */
  assert(tabP->blkSize       > 0);
  assert(tabP->frameBlkSize  > 0);
  assert(svmapP->svocabP->list.itemN > 0);

  tabP->svmapP      = svmapP;
  tabP->N           = 0;
  tabP->allocN      = tabP->blkSize;
  tabP->frameN      = 0;
  tabP->frameAllocN = tabP->frameBlkSize;

  /* lct hash function */
  listInit((List*) &(tabP->l2x),(TypeInfo*) NULL,sizeof(LKey),100);
  tabP->l2x.init    = (ListItemInit  *)lkeyInit;
  tabP->l2x.deinit  = (ListItemDeinit*)lkeyDeinit;
  tabP->l2x.hashKey = (HashKeyFn     *)lkeyHashKey;
  tabP->l2x.hashCmp = (HashCmpFn     *)lkeyHashCmp;

  if (NULL == (tabP->A = (BP*) malloc(tabP->blkSize*sizeof(BP)))) {
    ERROR("stabInit: allocation failure\n");
    return TCL_ERROR;
  }
  if (NULL == (tabP->useNA = (int*) 
	       malloc((tabP->frameBlkSize+1)*sizeof(int)))) {
    ERROR("stabInit: allocation failure\n");
    return TCL_ERROR;
  }
  for (frameX=0;frameX<=tabP->frameBlkSize;frameX++) {
    tabP->useNA[frameX] = 0;
  }
  /* to optimize useNA[frameX-1] access for frameX = 0 */
  tabP->useNA++;

  if (NULL == (tabP->wordXA = (BP**) 
	       malloc(svmapP->svocabP->list.itemN*sizeof(BP*)))) {
    ERROR("stabInit: allocation failure\n");
    return TCL_ERROR;
  }
  return TCL_OK;
}

/* ------------------------------------------------------------------------
    stabCreate
   ------------------------------------------------------------------------ */

STab* stabCreate(SVMap* svmapP)
{
  STab* tabP = malloc(sizeof(STab));
  
  if( ! tabP) {
    ERROR("stabCreate: allocation failure\n");
    return NULL;
  }
  if (TCL_ERROR == (stabInit(tabP,svmapP))) {
      return NULL;
  }
  return tabP;
}
      
/* ------------------------------------------------------------------------
    stabDeinit
   ------------------------------------------------------------------------ */

static int stabDeinit(STab* tabP)
{
  /* to compensate for increase after allocation */
  tabP->useNA--;
  if (tabP->A)      free(tabP->A);
  if (tabP->useNA)  free(tabP->useNA);
  if (tabP->wordXA) free(tabP->wordXA);

  listDeinit((List*) &(tabP->l2x));

  tabP->A           = NULL;
  tabP->useNA       = NULL;
  tabP->wordXA      = NULL;
  tabP->N           = 0;
  tabP->allocN      = 0;
  tabP->frameN      = 0;
  tabP->frameAllocN = 0;

  return TCL_OK;
}

/* ------------------------------------------------------------------------
    stabFree
   ------------------------------------------------------------------------ */

int stabFree(STab* tabP)
{
  if (tabP == NULL) return TCL_OK;
  if (TCL_ERROR == stabDeinit(tabP))
    return TCL_ERROR;
  free(tabP);
  return TCL_OK;
}

/* ------------------------------------------------------------------------
    stabClear
   ------------------------------------------------------------------------ */

int stabClear(STab* tabP)
{
  int frameX;
  for (frameX=0;frameX<tabP->frameAllocN;frameX++)
    tabP->useNA[frameX]=0;

  listClear((List*) &(tabP->l2x));

  tabP->N      = 0;
  tabP->frameN = 0;

  return TCL_OK;
}
/* ------------------------------------------------------------------------
    stabAlloc
   ------------------------------------------------------------------------ */

#define LCTCHECK 0

BP* stabAlloc(STab* tabP,int frameX, LCT lct)
{ 
  BP* ret = NULL;
  int idx = -1;

#if LCTCHECK
  LKey lkey;
  lkey.lct = lct;
  lkey.bpX = tabP->N;
  idx = listIndex((List*) &(tabP->l2x),&lkey,0); 
#endif

  if (idx >= 0) {
    LKey* lkeyP = tabP->l2x.itemA+idx;
    ret = tabP->A + lkeyP->bpX;
    return ret;
  } else {
#if LCTCHECK
    listIndex((List*) &(tabP->l2x),&lkey,1);
#endif

    /* get next free backpointer */
    if (tabP->N >= tabP->allocN) {
      tabP->allocN += tabP->blkSize;
      if (NULL== (tabP->A = (BP*) realloc(tabP->A,tabP->allocN*sizeof(BP)))) {
	ERROR("stabAlloc: allocation failure\n");
	return NULL;
      }
    }    
    ret = tabP->A + (tabP->N)++;
    ret->score =SCORE_MAX;
  }

  /* store frame information */
  if (frameX > tabP->frameN) {
    tabP->frameN = frameX;
  }
  if (frameX >= tabP->frameAllocN) {
    int frameY = tabP->frameAllocN;
    tabP->frameAllocN += tabP->frameBlkSize;
    tabP->useNA--;
    if (NULL== (tabP->useNA = (int*) realloc(tabP->useNA,(tabP->frameAllocN+1)*sizeof(int)))) {
      ERROR("stabAlloc: allocation failure\n");
      return NULL;
    }
    tabP->useNA++;
    for (;frameY<tabP->frameAllocN;frameY++) {
      tabP->useNA[frameY]=0;
    }
  }
  if (tabP->useNA[frameX] == 0) {
    tabP->useNA[frameX] =  tabP->useNA[frameX-1];
  }
  tabP->useNA[frameX]++;
  return ret;
}

/* ------------------------------------------------------------------------
    prune backpointer table
   ------------------------------------------------------------------------ */

static int cmpBp(const void* a,const void* b)
{
  float s1 = ((BP*) a)->score;
  float s2 = ((BP*) b)->score;

  if (s1<s2) return -1;
  if (s1>s2) return  1;
  return 0;
}

int stabPrune (STab* tabP, int frameX, int topN)
{
  int  bpN = 0;
  int  bpM = 0;

  if (frameX >= tabP->frameAllocN) {
    int frameY = tabP->frameAllocN;
    tabP->frameAllocN += tabP->frameBlkSize;
    tabP->useNA--;
    if (NULL== (tabP->useNA = (int*) 
	realloc(tabP->useNA,(tabP->frameAllocN+1)*sizeof(int)))) {
      ERROR("stabPrune: allocation failure\n");
      return 0;
    }
    tabP->useNA++;
    for (;frameY<tabP->frameAllocN;frameY++) {
      tabP->useNA[frameY]=0;
    }
  }

  if (frameX > tabP->frameN) {
    assert(frameX == tabP->frameN +1);
    tabP->frameN = frameX;
  }

  bpN = tabP->useNA[frameX-1];
  bpM = tabP->useNA[frameX];

  /* if we got no backpointer in this frame */
  if (bpM ==0) {
    tabP->useNA[frameX] = bpM = bpN;
  }

  /* we need a sorted list of backpointers for expanding SRoot's */
  if (bpM-bpN > 1) {   
    qsort((void*) (tabP->A+bpN),bpM-bpN,sizeof(BP),cmpBp);
  }
  if (bpM-bpN > topN) {
    tabP->useNA[frameX] = bpN + topN;
    tabP->N -= ((bpM-bpN) -topN);
  }
  listClear((List*) &(tabP->l2x));
  return (tabP->useNA[frameX] - bpN);
}

/* ------------------------------------------------------------------------
    stabGetFrame : return frame index for a given backpointer
   ------------------------------------------------------------------------ */

int stabGetFrame (STab* tabP, int bpIdx)
{
  int frameX= tabP->frameN;
  if (bpIdx == -1) return -1;
  while (bpIdx < tabP->useNA[frameX] && frameX > -1) frameX--;
  return frameX+1;
}

/* ------------------------------------------------------------------------
    stabPuts
   ------------------------------------------------------------------------ */
int stabPuts (STab* tabP, int fromX, int toX) {
  SWord*  swordP = tabP->svmapP->svocabP->list.itemA;
  BP*        bpA = tabP->A;
  int bpX, frameX;

  for (frameX=fromX;frameX<=toX;frameX++) {
    int bpEnd   = tabP->useNA[frameX];
    int bpStart = (frameX > 0) ? tabP->useNA[frameX-1] : 0;
    itfAppendResult("frameX= %d bpN= %d\n",frameX,bpEnd-bpStart);
    for (bpX=bpStart;bpX<bpEnd;bpX++) {
      BP*     bp = bpA + bpX;
      char* name = (swordP + bp->svX)->name;
      itfAppendResult("  bpX = %d : name = %s  score = %f  from = %d\n",
		      bpX,name,bp->score,bp->bpIdx);
    }
  }

  return TCL_OK;
}

static int stabPutsItf (ClientData cd, int argc, char *argv[]) { 
  STab* tabP   = (STab*) cd;
  int   fromX  = 0;
  int   toX    = tabP->frameN;
  int   frameN = tabP->frameN;

  argc--;
  if ( itfParseArgv( argv[0], &argc, argv+1, 0,
       "-fromX",    ARGV_INT, NULL, &fromX,  NULL, "from frame",
       "-toX",      ARGV_INT, NULL, &toX,    NULL, "to frame",
        NULL) != TCL_OK) return TCL_ERROR;  

  if (fromX > frameN || fromX < 0 || toX > frameN || toX < 0) {
    ERROR ("indices out of bound\n");
    return TCL_ERROR;
  }

  return stabPuts (tabP, fromX, toX);
}

/* ------------------------------------------------------------------------
    stabFinalize
   ------------------------------------------------------------------------ */

int stabFinalize (STab* tabP)
{
  SVMap*     svmapP = tabP->svmapP; 
  int        frameN = tabP->frameN;
  LVX        endLVX = tabP->svmapP->idx.end;
  SVX        endSVX = (SVX) endLVX;
  int      bpOffset = tabP->useNA[frameN-1];
  int           bpN = tabP->useNA[frameN];
  BP*            bp = NULL;
  int           bpX;

  /* looking for last frame with a bp */
  while (bpOffset == bpN && frameN > 2) {
    frameN--;
    bpOffset = tabP->useNA[frameN-1];
    bpN = tabP->useNA[frameN];
  }
  if (bpOffset == bpN) {
    ERROR("stabFinalize: couldn't add final node\n");
    return TCL_ERROR;
  }

  /* look for final word */
  if (frameN == tabP->frameN) {
    BP* bpA = tabP->A;
    bpX = bpOffset;
    while (bpX < bpN && endLVX != svmapGet(svmapP,bpA[bpX].svX)) bpX++;

    /* everything ok, we have a final node at final frame */
    if (bpX < bpN) return TCL_OK;
  }

  /* pick best backpointer from last frame */
  bpX= bpOffset;
  bp = stabAlloc(tabP,tabP->frameN, LCT_NIL);
  bp->svX      = endSVX;
  bp->score    = tabP->A[bpX].score;
  /*--------------------*/
  /* START FF & FM HACK */
  /*--------------------*/
  /* Originally:  bp->bpIdx    = bpX; */
  frameN = tabP->frameN;
  while (tabP->useNA[frameN-2] == tabP->useNA[frameN-1] && frameN > 2)
    frameN--;
  bpX = tabP->useNA[frameN-2];
  while (tabP->A[bpX].svX==endSVX && tabP->A[bpX].bpIdx!=-1)
    bpX = tabP->A[bpX].bpIdx;
  bp->bpIdx    = bpX;
  /*------------------*/
  /* END FF & FM HACK */
  /*------------------*/
  bp->morphX   = 0;
  
  return TCL_OK;
}

/* ------------------------------------------------------------------------
   stabGetFinalBPX
   ------------------------------------------------------------------------ */

int stabGetFinalBPX (STab *tabP, int *rbpX, int *rfstate, int frameX, int verbose) 
{
  int frameN   = tabP->frameN;
  int bpOffset = tabP->useNA[frameN-1];
  int bpN      = tabP->useNA[frameN];
  int bpX      = *rbpX;
  int fstate   = *rfstate;
  LVX endLVX   = tabP->svmapP->idx.end;
  BP* bpA      = tabP->A;

  if (frameN == 0 || tabP->N == 0) {
    ERROR("stabGetFinalBPX: nothing to trace\n");
    return TCL_ERROR;
  }

  /* Partial traceback */
  if (frameX >= 0 && frameX < frameN) {
    bpOffset = tabP->useNA[frameX-1];
    bpN      = tabP->useNA[frameX];
  }

  if (bpX >=0) {
    if (bpX >= bpN) {
      ERROR("stabGetFinalBPX: invalid final state given\n");
      return TCL_ERROR;
    }
  } else {
    /* look for final word */
    bpX = bpOffset;
    while (bpX < bpN && svmapGet(tabP->svmapP,bpA[bpX].svX) != endLVX) bpX++;

    if (verbose ==1 && bpX < bpN) 
      INFO("stabGetFinalBPX: found final word at position %d\n",bpX-bpOffset);

    if (bpX == bpN) {
      if ( verbose >= 0 ) ERROR("stabGetFinalBPX: no final state\n");
      /* take best backpointer from final frame */
      fstate = 0;
      bpX    = bpOffset;
    } 
  }

  *rbpX    = bpX;
  *rfstate = fstate;

  return TCL_OK;
}

int into (int y, int *xA, int xN) {

  int i   = 0;
  int min = xA[0];
  while ( i < xN ) {
    if ( xA[i] == y ) return 1;
    if ( xA[i] < min ) min = xA[i];
    i++;
  }

  /* out of range */
  assert ( y >= min );

  return 0;
}

static float stabTrace (STab* tabP, int bpX, int frameX, int fromX, int *endA, int endN, int verbose)
{
  SVMap*     svmapP  = tabP->svmapP;
  SWord*     swordP  = svmapP->svocabP->list.itemA;
  int        frameN  = tabP->frameN;
  BP*        bpA     = tabP->A;
  int        bpY     = 0;
  int        newBpX  = 0;
  LVX        endLVX  = svmapP->idx.end;
  SVX        endSVX  = (SVX) endLVX;
  BP**       traceA  = NULL;
  int        hypoN   = 0;
  int        fstate  = 1;
  float      score   = SCORE_MAX;
  int        bpStart = (fromX > 0) ? tabP->useNA[fromX-1] : 0;

  if (frameN == 0 || tabP->N == 0) {
    ERROR("stabTrace: nothing to trace\n");
    return 0.0;
  }

  if ( stabGetFinalBPX (tabP, &bpX, &fstate, frameX, verbose) != TCL_OK )
    return SCORE_MAX;

  bpY   = bpX;
  score = bpA[bpX].score;

  while ( (!endA && bpX != -1 && bpX >= bpStart) || (endA && !into (bpX, endA, endN)) ) {
    if (hypoN % 40 == 0) {
      if (NULL == (traceA=realloc(traceA,(hypoN+40)*sizeof(BP*)))) {
	ERROR("stabTrace: allocation failure\n");
	return SCORE_MAX;
      }
    }
    /* START FF HACK --- avoid infinite loop */
    newBpX = bpA[bpX].bpIdx;
    if (newBpX==bpX) {
	      ERROR("stabTrace: loop in backpointer table!\n");
	      return SCORE_MAX;
    }
    /* END FF HACK --- avoid infinite loop */
    traceA[hypoN++] = &(bpA[bpX]);
    bpX = bpA[bpX].bpIdx;
  }

  if ( endA ) {
    int fX = stabGetFrame (tabP, bpX) + 1;
    if ( fX > fromX ) fromX = fX;
#ifdef STABLE_DBG
    fprintf (STDERR, "bpY=%d bpX=%d fromX=%d\n", bpY, bpX, fromX);
#endif
  }
  hypoN--;

  if (verbose == 2) {
    itfAppendResult( "%d %f ", bpY, score);
    while (hypoN > -1) {
      itfAppendResult("{%d %s %f} ", stabGetFrame (tabP, traceA[hypoN]-bpA),
		      (swordP+traceA[hypoN]->svX)->name, traceA[hypoN]->score);
      hypoN--;
    }
    if (!fstate) 
      itfAppendResult("{%s %f}",(swordP+endSVX)->name,score);
  } else if (verbose == 3) {
    itfAppendResult ("{hypo ");
    while (hypoN > -1) {
      itfAppendResult("{%d %s %f} ", stabGetFrame (tabP, traceA[hypoN]-bpA),
		      (swordP+traceA[hypoN]->svX)->name, traceA[hypoN]->score);
      hypoN--;
    }
    if (!fstate) 
      itfAppendResult("{%s %f}",(swordP+endSVX)->name,score);
    itfAppendResult ("} ");

    itfAppendResult ("{fromX %d} ", fromX);
    itfAppendResult ("{toX %d} ",   stabGetFrame (tabP, traceA[hypoN+1]-bpA));
    itfAppendResult ("{endX %d}",   stabGetFrame (tabP, traceA[hypoN+1]-bpA));
  } else {
    itfAppendResult( "%d %f ", bpY, score);
    while (hypoN > -1)
      itfAppendResult("%s ",(swordP+traceA[hypoN--]->svX)->name);
    if (!fstate) 
      itfAppendResult("%s",(swordP+endSVX)->name);
  }

  if (traceA) free(traceA);
  return score;
}

static int stabTraceItf (ClientData cd, int argc, char *argv[]) { 
  STab*    tabP = (STab*) cd;
  int       bpX = -1;
  int    frameX = -1;
  int     fromX = 0;
  int   verbose = 0;
  char     *bpL = NULL;
  int     *endA = NULL;
  int      endN = 0;
  char     **av = NULL;
  int        ac = 0;
  float     res = 0.0;

  argc--;
  if ( itfParseArgv( argv[0], &argc, argv+1, 0,
      "-bpIdx",    ARGV_INT, NULL, &bpX,     NULL, "final state for trace back",
      "-frameIdx", ARGV_INT, NULL, &frameX,  NULL, "final frame for trace back",
      "-fromX",    ARGV_INT, NULL, &fromX,   NULL, "trace back until frame fromX",
      "-bpL",    ARGV_STRING, NULL, &bpL,     NULL, "list of backpointers to end trace back",
      "-v",        ARGV_INT, NULL, &verbose, NULL, "verbose output",
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

  res = stabTrace(tabP, bpX, frameX, fromX, endA, endN, verbose);

  if (av)   Tcl_Free ((char*) av);
  if (endA) free (endA);

  return TCL_OK;
}

/* ----------------------------------------------------------------------
   stabTraceStable
   returns the longest stable hypothesis
   ---------------------------------------------------------------------- */
int stabTraceStable (STab *tabP, int fromX, int toX, int *endA, int endN, int *abpA, int abpN, int verbose) {

  SVMap  *svmapP   = tabP->svmapP;
  SWord  *swordP   = svmapP->svocabP->list.itemA;
  int     frameN   = tabP->frameN;
  BP   ***traceA   = NULL;
  int    *hypoN    = NULL;
  BP     *bpA      = tabP->A;
  int     i        = 0;
  int     found    = 0;
  int    *resA     = NULL;
  int     resN     = 0;
  int     bpStart, bpEnd, bpEND, bpN, bpX, loopN;

  if (frameN == 0 || tabP->N == 0) {
    ERROR ("stabTraceStable: nothing to trace\n");
    return TCL_ERROR;
  }
  if (fromX < 0)                 fromX = 0;
  if (toX   < 0 || toX > frameN) toX   = frameN;

  /* init */
  bpStart  = tabP->useNA[toX-1];
  bpEnd    = tabP->useNA[toX];
  while ( bpEnd <= bpStart && toX >= 0 ) {
    /* if we do not have BPs in this frame go one frame back */
    toX--;
    bpStart  = tabP->useNA[toX-1];
    bpEnd    = tabP->useNA[toX];
  }
  if ( fromX > toX ) {
    ERROR ("stabTraceStable: nothing to trace\n");
    return TCL_ERROR;
  }
  bpEND    = (fromX > 0) ? tabP->useNA[fromX-1] : 0;
  bpN      = bpEnd - bpStart + abpN;
  bpX      = bpStart;

  traceA   = (BP***)calloc (bpN, sizeof(BP**));
  hypoN    = (int*) calloc (bpN, sizeof(hypoN));
  if ( !traceA || !hypoN ) {
    ERROR ("stabTraceStable: allocation failure\n");
    return TCL_ERROR;
  }

  /* collect necessary information */
#ifdef STABLE_DBG
  itfAppendResult ("{sBP ");
#endif
  while ( bpX < bpEnd ) {
    int bpY = bpX;

#ifdef STABLE_DBG
    itfAppendResult ("%d ",bpY);
#endif

    while ( (!endA && bpY >= bpEND) || (endA && !into (bpY, endA, endN)) ) {
      if ( hypoN[i] % 40 == 0 ) {
	if ( NULL == (traceA[i] = (BP**)realloc (traceA[i], (hypoN[i]+40) * sizeof(BP*)))) {
	  ERROR ("stabTraceStable: allocation failure\n");
	  return TCL_ERROR;
	}
      }
      traceA[i][hypoN[i]++] = bpA+bpY;
      bpY = (bpA+bpY)->bpIdx;
    }
    if ( endA ) {
      int fX = stabGetFrame (tabP, bpY) + 1;
      if ( fX > fromX ) fromX = fX;

      /* append BPs of endA to the traceA, so that for the case if no
	 identical path was found endA is given back as a result.
	 Therefore, we do not increase hypoN[i], hypoN[i]+2 points
	 then to the BPs of endA */
      if ( hypoN[i] % 40 == 0 ) {
	if ( NULL == (traceA[i] = (BP**)realloc (traceA[i], (hypoN[i]+1) * sizeof(BP*)))) {
	  ERROR ("stabTraceStable: allocation failure\n");
	  return TCL_ERROR;
	}
      }
      traceA[i][hypoN[i]] = bpA+bpY;
    }
    i++; bpX++;
  }
#ifdef STABLE_DBG
  itfAppendResult ("} ");
#endif

  for (; i<bpN; i++) {
    int bpY = abpA[i-bpN+abpN];

    while ( (!endA && bpY >= bpEND) || (endA && !into (bpY, endA, endN)) ) {
      if ( hypoN[i] % 40 == 0 ) {
	if ( NULL == (traceA[i] = (BP**)realloc (traceA[i], (hypoN[i]+40) * sizeof(BP*)))) {
	  ERROR ("stabTraceStable: allocation failure\n");
	  return TCL_ERROR;
	}
      }
      traceA[i][hypoN[i]++] = bpA+bpY;
      bpY = (bpA+bpY)->bpIdx;
    }
    if ( endA ) {
      int fX = stabGetFrame (tabP, bpY) + 1;
      if ( fX > fromX ) fromX = fX;

      /* append BPs of endA to the traceA, so that for the case if no
	 identical path was found endA is given back as a result.
	 Therefore, we do not increase hypoN[i], hypoN[i]+2 points
	 then to the BPs of endA */
      if ( hypoN[i] % 40 == 0 ) {
	if ( NULL == (traceA[i] = (BP**)realloc (traceA[i], (hypoN[i]+1) * sizeof(BP*)))) {
	  ERROR ("stabTraceStable: allocation failure\n");
	  return TCL_ERROR;
	}
      }
      traceA[i][hypoN[i]] = bpA+bpY;
    }
  }

  /* output */
  for (i=0; i<bpN; i++) hypoN[i]--;

  loopN = 0;
  itfAppendResult ("{hypo ");
  while ( !found ) {
    int j = hypoN[0]--;

    loopN++;

    if ( j >= 0 ) {
      SVX svX = traceA[0][j]->svX;

      /* we do not exit here, as soon as found == 1, because then
	 we do not know the state of hypoN */
      for ( i = 1; i < bpN; i++ ) {
	j = hypoN[i]--;
	if ( j < 0 || svX != traceA[i][j]->svX ) found = 1;
      }

      if ( !found ) {
	if ( verbose == 2 ) {
	  j = hypoN[0]+1;
	  itfAppendResult ("{%d %s %f} ",
			   stabGetFrame (tabP, traceA[0][j]-bpA),
			   (swordP+svX)->name, traceA[0][j]->score);
	} else if ( verbose == 3 ) {
	  j = hypoN[0]+1;
	  itfAppendResult ("{%d %s %f {",
			   stabGetFrame (tabP, traceA[0][j]-bpA),
			   (swordP+svX)->name, traceA[0][j]->score);
	  for (i=0; i<bpN; i++) {
	    int k = hypoN[i]+1;
	    int b = traceA[i][k]-bpA;
	    itfAppendResult ("%d ", b);
	  }
	  itfAppendResult ("}} ");
	} else {
	  itfAppendResult ("%s ", (swordP+svX)->name);
	}
      }
    } else {
      hypoN[0]--; /* this is correct, otherwise endX would be wrong */
      found = 1;
    }
  }
  itfAppendResult ("} ");

  itfAppendResult ("{fromX %d} ", fromX);
  itfAppendResult ("{toX %d} ",   toX);
  itfAppendResult ("{endX %d} ",
		   stabGetFrame (tabP, traceA[0][hypoN[0]+2]-bpA));

  /* if no matching words were found even for the first word
     (this could occur, if we are at the beginning of the utterance)
     then the loop above will only be executed once, which means
     that no bpL and scoreL can be set! */
  if ( endA || loopN > 1 ) {
    if ( NULL == (resA = (int*)malloc (bpN*sizeof(int))) ) {
      ERROR ("stabTraceStable: allocation failure\n");
      return TCL_ERROR;
    }
    itfAppendResult ("{bpL");
    for (i=0; i<bpN; i++) {
      int j = hypoN[i]+2;
      int k = 0;
      bpX   = traceA[i][j]-bpA;
      while ( k < resN && resA[k] != bpX ) k++;
      if ( k == resN ) {
	resA[resN++] = bpX;
	itfAppendResult (" %d", bpX);
      }
    }
    itfAppendResult ("} ");

    itfAppendResult ("{scoreL");
    for (i=0; i<resN; i++) {
      itfAppendResult (" %f",(bpA+resA[i])->score);
    }
    itfAppendResult ("}");
  }

  if ( resA   ) free (resA);
  if ( hypoN  ) free (hypoN);
  if ( traceA ) {
    for (i=0; i<bpN; i++) {
      if ( traceA[i] ) free (traceA[i]);
    }
    free (traceA);
  }

  return TCL_OK;
}

/* ========================================================================
    Type Information
   ======================================================================== */

Method stabMethod[] = 
{
  { "puts",   stabPutsItf,  "displays the contents of a backpointer table"  },
  { "trace",  stabTraceItf, "trace back from final state"},
  { NULL, NULL, NULL }
} ;

TypeInfo stabInfo = 
{ "STab", 0, -1,  stabMethod, 
   NULL, NULL, 
   NULL, NULL, NULL,
  "Backpointer table"
};

static int stabInitialized = 0;

int STab_Init (void)
{
  if (! stabInitialized) {
    itfNewType (&stabInfo);
    stabInitialized = 1;

    stabDefault.blkSize      = 1000;
    stabDefault.frameBlkSize = 1000;

  }
  return TCL_OK;
}
