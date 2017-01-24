/* ========================================================================
    JANUS      Speech- to Speech Translation
               ------------------------------------------------------------
               Object: Viterbi Alignment
               ------------------------------------------------------------

    Author  :  Michael Finke & Ivica Rogina
    Module  :  viterbi.c
    Date    :  $Id: viterbi.c 3402 2011-03-20 20:56:52Z metze $
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
    Revision 3.9  2007/02/22 16:27:59  fuegen
    removed -pedantic compiler warnings with

    Revision 3.8  2006/11/10 10:15:39  fuegen
    merged changes from branch jtk-05-02-02-shajith
    - all modification related to MMIE training
    - first unverified functions for MPE training
    - modifications made by Shajith, Roger, Wilson, and Sebastian

    Revision 3.7.12.1  2006/11/03 12:28:22  stueker
    Initial check-in of the MMIE from Shajith, Wilson, and Roger. Contains viterbiTmp

    Revision 3.7  2003/08/14 11:19:58  fuegen
    Merged changes on branch jtk-01-01-15-fms (jaguar -> ibis-013)

    Revision 3.6.20.7  2003/02/05 09:23:18  soltau
    Fixed forwardbackward

    Revision 3.6.20.6  2003/01/13 14:30:39  soltau
    Fixed index overflow in guided viterbi alignment

    Revision 3.6.20.5  2002/07/18 12:28:19  soltau
    Removed malloc.h

    Revision 3.6.20.4  2001/07/06 13:12:03  soltau
    Changed compress and scoreFct handling

    Revision 3.6.20.3  2001/07/03 09:39:37  soltau
    Changes according to the new acoustic score function interface

    Revision 3.6.20.2  2001/06/22 14:45:21  soltau
    *** empty log message ***

    Revision 3.6.20.1  2001/06/22 12:11:23  soltau
    fixed initialization problem

    Revision 3.3.24.1  2000/09/12 21:43:57  hyu
    Fixed bug causing segmentation fault in guided viterbi alignment.

    Revision 3.3.18.1  2000/09/12 17:18:27  janus
    added cleanup of viterbi backpointer table

    Revision 3.3  2000/08/16 11:50:09  soltau
    Added duration probabilities in path computation
    use computeScore macro instead of snsScore to access score cache

 * Revision 3.3  2000/08/16  11:50:09  soltau
 * Added duration probabilities in path computation
 * use computeScore macro instead of snsScore to access score cache
 * 

    Revision 3.2  1997/06/17 17:41:43  westphal
    *** empty log message ***

 * Revision 1.18  96/11/29  17:17:32  mmeyer
 * check success of malloc (to avoid Segmentation faults)
 * 
 * Revision 1.17  1996/04/29  13:59:33  monika
 * removed bug affecting the return value of
 * pitemList configure -score for the last frame
 * in a viterbi (now returns score on path,
 * before best score in frame).
 *
 * Revision 1.16  1996/03/13  21:57:12  finkem
 * added scor to path.itemList as best path score in viterbi
 *
 * Revision 1.15  1996/03/05  18:36:26  monika
 * removed bug in vitebi (but not forward backward) tahat makde
 * code crash for -from > 0
 *
 * Revision 1.14  96/01/25  18:28:29  18:28:29  rogina (Ivica Rogina)
 * added error codes
 * 
 * Revision 1.13  1996/01/17  02:42:33  torsten
 * Removed 1e-19 from forward Backward, since it hurt performance alot.
 *
 * Revision 1.12  1995/12/17  16:52:51  finkem
 * added +1e-19 to score in FwdBwd to prevent sum=0.
 *
 * Revision 1.11  1995/11/30  15:06:11  finkem
 * filling out path structure.best corrected
 *
 * Revision 1.10  1995/11/17  22:25:42  finkem
 * revised viterbi and brand new, damned fast forward backward
 *
 * Revision 1.9  1995/11/14  06:05:14  finkem
 * count clustering of polyphones
 *
 * Revision 1.8  1995/10/19  09:59:56  rogina
 * bug [0] --> [stX]
 *
 * Revision 1.7  1995/10/18  08:51:25  rogina
 * alignment along labels
 *
 * Revision 1.6  1995/10/16  15:53:37  finkem
 * made it work with new hmm.c
 *
 
   ======================================================================== */


char viterbiRCSVersion[]= 
           "@(#)1$Id: viterbi.c 3402 2011-03-20 20:56:52Z metze $";

#ifndef DARWIN
#include <omp.h>
#endif
#include "common.h"
#include "error.h"
#include "hmm.h"
#include "scoreA.h"

/* ========================================================================
    Forward Declarations & Globals
   ======================================================================== */

#define BIGNUM 9e19		        /* higher than any accumulated score */

/* ========================================================================
    Forced Alignment Score Computation
   ======================================================================== */

static float* faScoreA  = NULL;
static int    faScoreN  = 0;
static char*  faActiveA = NULL;

#ifndef DARWIN
#pragma omp threadprivate (faScoreN, faScoreA, faActiveA)
#endif
int faScoreACreate( SenoneSet* snsP)
{
  int n = snsP->list.itemN;

  if (! faScoreA || faScoreN < n) {
    faScoreN  =  n;
    faScoreA  = (float*)realloc( faScoreA,  faScoreN * sizeof(float));
    faActiveA = (char *)realloc( faActiveA, faScoreN * sizeof(char));
  }
  memset( faActiveA, 0, n * sizeof(char));
  return n;
}

/* ========================================================================
    Viterbi
   ======================================================================== */
/* ------------------------------------------------------------------------
    FACell
   ------------------------------------------------------------------------ */

typedef struct {

  int   stateX;
  int   fromX;
  float score;

} FACell;

/* ------------------------------------------------------------------------
    compareFACell
   ------------------------------------------------------------------------ */

static int compareFACell(const void * a, const void *b)
{
  FACell* bpa = (FACell *)a;
  FACell* bpb = (FACell *)b;

  if      ( bpa->score < bpb->score) return -1;
  else if ( bpa->score > bpb->score) return  1;
  else                               return  0;
}

/* ------------------------------------------------------------------------
    state durations
   ------------------------------------------------------------------------ */

static short *durFrom =NULL;
static short *durTo   =NULL;
static int durSize=0;


/* ------------------------------------------------------------------------
    clean up backpointer table
   ------------------------------------------------------------------------ */

void compressBpTabel(int * bpTableN,  FACell ** bpTableA, int fromFrame, int toFrame) {
  /* clean BackPointerTable */
  int i     = 0;
  int j     = 0;
  int bpSum = 0;
  int frX   = 0;
  char * activeBpA = NULL;
  int  * mapFromBpA = NULL;
  FACell * destPtr = NULL;
  FACell * srcPtr  = NULL;

  if (toFrame <= fromFrame) {
    return;
  }

  for (frX = toFrame; frX > fromFrame; frX--) {
    int nfrX = frX + 1;
    j = 0;
    activeBpA = (char *) malloc(bpTableN[frX] * sizeof(char));
    memset( activeBpA, 0,bpTableN[frX]  * sizeof(char));
    mapFromBpA = (int *) malloc(bpTableN[frX] * sizeof(int));
    memset( mapFromBpA, -1,bpTableN[frX]  * sizeof(int));
    /* mark all used backpointer */
    for (i = 0;  i < bpTableN[nfrX]; i++) {
      activeBpA[bpTableA[nfrX][i].fromX] = 1;
    }

    bpSum   = 0;
    for (i = 0;  i < bpTableN[frX]; i++) {
      if (activeBpA[i]) {
        bpSum++;
        mapFromBpA[i] = j;
        j++;
      }
    }
    
    /* now do compression */
    destPtr = bpTableA[frX];
    srcPtr  = bpTableA[frX];
    for (i = 0;  i < bpTableN[frX]; i++) {
      if (activeBpA[i]) {
        if (destPtr != srcPtr) {
          destPtr->fromX  = srcPtr->fromX;
          destPtr->stateX = srcPtr->stateX;
          destPtr->score  = srcPtr->score;
        }
        destPtr++;
      }
      srcPtr++;
    }
    for (i = 0;  i < bpTableN[nfrX]; i++) {
      bpTableA[nfrX][i].fromX = mapFromBpA[bpTableA[nfrX][i].fromX];
    }
    /* INFO("frX %d old %d new %d\n", frX, bpTableN[frX], bpSum); */
    if (bpTableN[frX] != bpSum) {
      FACell * tmpPtr;
      tmpPtr = bpTableA[frX];
      bpTableA[frX] = (FACell *) malloc(bpSum * sizeof(FACell));

      for (i = 0; i < bpSum; i++) {
        bpTableA[frX][i].fromX  = tmpPtr[i].fromX;
        bpTableA[frX][i].stateX = tmpPtr[i].stateX;
        bpTableA[frX][i].score  = tmpPtr[i].score;
      }
      free(tmpPtr);

      bpTableN[frX] = bpSum;

      /* bpTableA[frX] = (FACell *) realloc(bpTableA[frX], bpSum * sizeof(FACell)); */
    }
    free(activeBpA);
    free(mapFromBpA);
  }

  return;
}

/* ------------------------------------------------------------------------
    viterbi
   ------------------------------------------------------------------------ */

int viterbi( Path* path, HMM* hmm, int topN, int bpCleanUpMod, int bpCleanUpMul)
{
  SenoneSet*  snsP      = amodelSetSenoneSet(hmm->amodels);
  StateGraph* graph     = hmm->stateGraph;
  PhoneGraph* pgraphP   = hmm->phoneGraph;
  AModelSet*  amsP      = hmm->phoneGraph->amsP;
  DurationSet* durP     = NULL;
  FACell**    bpTableA  = NULL;
  int*        bpTableN  = NULL;
  int*        bpTableX  = NULL;
  int         from      = path->firstFrame;
  int         to        = path->lastFrame, frX;
  int         scoreN, stN, i, bpX, bpIdx;
  int         bestExitX;
  float       bestExitScore;
  int         ret = TCL_OK;

  if (hmm->phoneGraph->amsP->durP) 
    durP     = (DurationSet*) hmm->phoneGraph->amsP->durP->mdsP.cd;

  if (! graph) {
    ERROR("<VIT,CMS>Can't make state graph from a word graph.\n");
    return TCL_ERROR;
  }
  if ( from < 0 || from > to) { 
    ERROR("<VIT,UPD>Unreasonable path-delimiters: [%d...%d].\n", from, to);
    return TCL_ERROR;
  }

  stN      =  graph->stateN;
  scoreN   =  faScoreACreate( snsP);
  bpTableA = (FACell**)malloc((to-from+1) * sizeof(FACell*)) - from;
  bpTableN = (int    *)malloc((to-from+1) * sizeof(int))     - from;
  bpTableX = (int    *)malloc((stN)       * sizeof(int));

  memset( bpTableA+from, 0, (to-from+1) * sizeof(FACell*));
  memset( bpTableN+from, 0, (to-from+1) * sizeof(int));
  memset( bpTableX,      0, (stN)       * sizeof(int));

  /* malloc and init duration arrays */
  if (durP) {
    if (stN > durSize) {
      durFrom = (short*) realloc((void*) durFrom,stN*sizeof(short));
      durTo   = (short*) realloc((void*) durTo  ,stN*sizeof(short));
      durSize = stN;
    }
    assert (durFrom && durTo);
    for (i=0;i<stN;i++) {
      durFrom[i]=durTo[i]=0;
    }
  }

  /* Fill out the initial states */
  
  bpTableA[from] = (FACell*)malloc( stN * sizeof(FACell));
    /* check memory allocation */
  if (bpTableA[from] == NULL) {
    SERROR( "<VIT,CB>Failed to alloc memory in viterbi(%s,%s).\n",path->name, hmm->name); 
     ret = TCL_ERROR;
     goto skipBacktrace;   
   }

  bpIdx          =  0;

  for ( i = 0; i < graph->initN; i++) {
    int   j;

    if (durP) durFrom[graph->init[i]] = 1;

    bpTableA[from][bpIdx].stateX =  graph->init[i];
    bpTableA[from][bpIdx].fromX  = -1;
    bpTableA[from][bpIdx].score  =  snsP->scoreFctP( snsP, graph->senoneX[i], from);

    if ( path->pitemListA[0].itemN) {
      float senoneMissPen = path->senoneMissPen;
      float phoneMissPen  = path->phoneMissPen;
      float wordMissPen   = path->wordMissPen;

      for ( j = 0; j < path->pitemListA[0].itemN; j++) {
        PathItem* pi  = path->pitemListA[0].itemA + j;
        int       stX = bpTableA[from][bpIdx].stateX;

        if ( pi->senoneX   != -1 && pi->senoneX == graph->senoneX[stX])
             senoneMissPen  =  pi->alpha;
        if ( pi->phoneX    != -1 && pi->phoneX  == graph->phoneX[stX])
             phoneMissPen   =  pi->beta;
        if ( pi->wordX     != -1 && pi->wordX   == graph->wordX[stX])
             wordMissPen    =  pi->gamma;
      }
      bpTableA[from][bpIdx].score += senoneMissPen + phoneMissPen + wordMissPen;
    }
    bpIdx++;
  }
  bpTableN[from] =  bpIdx;
  bpTableA[from] = (FACell*)realloc( bpTableA[from], bpIdx * sizeof(FACell));

  qsort( bpTableA[from], bpIdx, sizeof(FACell), compareFACell);

  /* For each frame do a forward path */

  for ( frX = from; frX < to; frX++) {
    FACell* bp   =  bpTableA[frX];
    int     bpN  = (topN && topN < bpTableN[frX]) ? topN : bpTableN[frX];
    int     nfrX =  frX + 1;

    bpTableA[nfrX] = (FACell*)malloc( stN * sizeof(FACell));
    /* check memory allocation */
    if (bpTableA[nfrX] == NULL ){
      SERROR( "<VIT,CB>Failed to alloc memory in viterbi(%s,%s).\n",path->name, hmm->name); 
      ret = TCL_ERROR;
      goto skipBacktrace;   
    }

    bpIdx          =  0;

    memset( bpTableX,  0, stN    * sizeof(int));
    memset( faActiveA, 0, scoreN * sizeof(char));

    for ( bpX = 0; bpX < bpN; bpX++, bp++) {

      if ( bp->score < path->pitemListA[frX-from].beam + 
                       bpTableA[frX][0].score) {

        int stX         = bp->stateX;
	int phoneX      = graph->phoneX[stX];
	int amodelX     = pgraphP->amodel[phoneX];
	AModel *amodelP = amsP->list.itemA+amodelX;
	int durX        = amodelP->durX;
        int trX;

        for ( trX = 0; trX < graph->fwdTrN[stX]; trX++) {
          int     toX   = stX + graph->fwdIdx[stX][trX];
	  float   score = bp->score;
	  int    phoneY = 0;
	  int     toBpX = 0;

	  if (toX < 0 || toX >= stN) continue;
	  phoneY = graph->phoneX[toX];
        
	  if (durP) {
	    if (phoneY == phoneX) 
	      durTo[toX]=durFrom[stX]+1;
	    else
	      score += durSetScore(durP,durX,durFrom[stX]);
	  }

          toBpX  = bpTableX[toX];
          score += graph->fwdPen[stX][trX];

          if (! toBpX) {
            FACell* toBp  = bpTableA[nfrX] + bpIdx++;
            toBp->stateX  = toX;
            toBp->score   = score;
            toBp->fromX   = bpX;
            bpTableX[toX] = bpIdx;
          }
          else {
            FACell* toBp = bpTableA[nfrX]+toBpX-1;
            if ( score < toBp->score) {
              toBp->score = score;
              toBp->fromX = bpX;
            }
          }
        }
      }
    }

    bpTableN[nfrX] =  bpIdx;
    bpTableA[nfrX] = (FACell*)realloc( bpTableA[nfrX], bpIdx*sizeof(FACell));
    bp             =  bpTableA[nfrX];
    bpN            =  bpTableN[nfrX];

    /* add the log of the observation probabilities to the new frame
       of FACells allocated and filled above */

    for ( bpX = 0; bpX < bpN; bpX++, bp++) {
      int   snX = graph->senoneX[bp->stateX];
      int   j;

      if ( faActiveA[snX]) bp->score += faScoreA[snX];
      else {
           bp->score += faScoreA[snX] = snsP->scoreFctP( snsP, snX, nfrX);
           faActiveA[snX] = 1;
      }

      if ( path->pitemListA[nfrX-from].itemN) {
        float senoneMissPen = path->senoneMissPen;
        float phoneMissPen  = path->phoneMissPen;
        float wordMissPen   = path->wordMissPen;

        for ( j = 0; j < path->pitemListA[nfrX-from].itemN; j++) {
          PathItem* pi  = path->pitemListA[nfrX-from].itemA + j;
          int       stX = bpTableA[nfrX][bpX].stateX;

          if ( pi->senoneX   != -1 && pi->senoneX == graph->senoneX[stX])
               senoneMissPen  =  pi->alpha;
          if ( pi->phoneX    != -1 && pi->phoneX  == graph->phoneX[stX])
               phoneMissPen   =  pi->beta;
          if ( pi->wordX     != -1 && pi->wordX   == graph->wordX[stX])
               wordMissPen    =  pi->gamma;
        }
        bp->score += senoneMissPen + phoneMissPen + wordMissPen;
      }
    }
    qsort( bpTableA[nfrX], bpIdx, sizeof(FACell), compareFACell);

    /*  the duration counter*/
    if (durP) {
      for (i=0;i<stN;i++) 
	durFrom[i]=durTo[i];
    }

    if (bpCleanUpMod >= 0 && frX % bpCleanUpMod == 0) {
      /* clean up th bpTable every bpCleanUpMod and go back in history bpCleanUpMod * bpCleanUpMul Frames */
      if (bpCleanUpMul > 0) {
	compressBpTabel(bpTableN, bpTableA, MAX(from,frX - (bpCleanUpMod * bpCleanUpMul) ), frX);
      } else {
	compressBpTabel(bpTableN, bpTableA, from , frX);
      }
    }


  }  /* end foreach frame*/

  /* Find the best exit transition */

  bestExitScore = BIGNUM;
  bestExitX     = -1;

  for ( i   = 0; i < bpTableN[to]; i++) {
    int   stX   = bpTableA[to][i].stateX;
    float score = bpTableA[to][i].score;
    int   trX;

    for ( trX = 0; trX < graph->fwdTrN[stX]; trX++) {
      if ( stX + graph->fwdIdx[stX][trX] >= stN) {
        if ( score < bestExitScore) {
          bestExitScore = score;
          bestExitX     = i;
        }
      }
    }     
  }

  /* Go through the backtrace and fill out the path... */

  if ( bestExitX < 0) {
    SERROR( "<VIT,CB>Failed to backtrace in viterbi(%s,%s).\n",path->name, hmm->name); 
    path->gscore = (-1.0);

    for ( frX = to-from; frX >= 0; frX--) 
      pathItemListClear( path->pitemListA + frX);

    ret = TCL_ERROR;
    goto skipBacktrace;
  }

  pathItemListClear( path->pitemListA + to-from);
  pathItemListAdd(   path->pitemListA + to-from, 1, 
                     bpTableA[to][bestExitX].stateX,
                     graph->senoneX[bpTableA[to][bestExitX].stateX],
                     graph->phoneX[ bpTableA[to][bestExitX].stateX],
                     graph->wordX[  bpTableA[to][bestExitX].stateX]);

  path->pitemListA[to-from].itemA[0].gamma = 1.0;
  path->pitemListA[to-from].logScale       = 0.0;
  path->pitemListA[to-from].score          = bpTableA[to][bestExitX].score;
  path->pitemListA[to-from].best           = bpTableA[to][0].score;

  bestExitX = bpTableA[to][bestExitX].fromX;

  for (frX = to-from-1; frX >= 0; frX--) {
    int stX = bpTableA[frX+from][bestExitX].stateX;

    pathItemListClear( path->pitemListA + frX);
    pathItemListAdd(   path->pitemListA + frX, 1, stX,
                       graph->senoneX[stX], graph->phoneX[stX],
                       graph->wordX[stX]);

    path->pitemListA[frX].itemA[0].gamma = 1.0;
    path->pitemListA[frX].logScale       = 0.0;
    path->pitemListA[frX].score          = bpTableA[frX+from][bestExitX].score;
    path->pitemListA[frX].best           = bpTableA[frX+from][0].score;

    bestExitX = bpTableA[frX+from][bestExitX].fromX;
  }

  /* ----------------------------------------------- */
  /* store the final DTW score in the path structure */
  /* ----------------------------------------------- */

  path->gscore = bestExitScore;

  /* ---------------------------- */
  /* release the allocated memory */
  /* ---------------------------- */

  skipBacktrace:

  /* Free allocated temporary data structures */

  if ( bpTableA) {
    for ( frX = from; frX <= to; frX++)
      if ( bpTableA[frX]) free( bpTableA[frX]);
    free( bpTableA+from);
  }
  if ( bpTableN) free( bpTableN+from);
  if ( bpTableX) free( bpTableX);

  return ret;
}


int viterbiMMIE( Path* path, HMM* hmm, PhoneGraph *pgraphP, int topN, int bpCleanUpMod, int bpCleanUpMul)
{
  SenoneSet*  snsP      = amodelSetSenoneSet(hmm->amodels);
  StateGraph* graph     = hmm->stateGraph;
  AModelSet*  amsP      = hmm->phoneGraph->amsP;
  DurationSet* durP     = NULL;
  FACell**    bpTableA  = NULL;
  int*        bpTableN  = NULL;
  int*        bpTableX  = NULL;
  int         from      = path->firstFrame;
  int         to        = path->lastFrame, frX;
  int         scoreN, stN, i, bpX, bpIdx;
  int         bestExitX;
  float       bestExitScore;
  int         ret = TCL_OK;

  if (hmm->phoneGraph->amsP->durP) 
    durP     = (DurationSet*) hmm->phoneGraph->amsP->durP->mdsP.cd;

  if (! graph) {
    ERROR("<VIT,CMS>Can't make state graph from a word graph.\n");
    return TCL_ERROR;
  }
  if ( from < 0 || from > to) { 
    ERROR("<VIT,UPD>Unreasonable path-delimiters: [%d...%d].\n", from, to);
    return TCL_ERROR;
  }

  stN      =  graph->stateN;
  scoreN   =  faScoreACreate( snsP);
  bpTableA = (FACell**)malloc((to-from+1) * sizeof(FACell*)) - from;
  bpTableN = (int    *)malloc((to-from+1) * sizeof(int))     - from;
  bpTableX = (int    *)malloc((stN)       * sizeof(int));

  memset( bpTableA+from, 0, (to-from+1) * sizeof(FACell*));
  memset( bpTableN+from, 0, (to-from+1) * sizeof(int));
  memset( bpTableX,      0, (stN)       * sizeof(int));

  /* malloc and init duration arrays */
  if (durP) {
    if (stN > durSize) {
      durFrom = (short*) realloc((void*) durFrom,stN*sizeof(short));
      durTo   = (short*) realloc((void*) durTo  ,stN*sizeof(short));
      durSize = stN;
    }
    assert (durFrom && durTo);
    for (i=0;i<stN;i++) {
      durFrom[i]=durTo[i]=0;
    }
  }

  /* Fill out the initial states */
  
  bpTableA[from] = (FACell*)malloc( stN * sizeof(FACell));
    /* check memory allocation */
  if (bpTableA[from] == NULL) {
    SERROR( "<VIT,CB>Failed to alloc memory in viterbi(%s,%s).\n",path->name, hmm->name); 
     ret = TCL_ERROR;
     goto skipBacktrace;   
   }

  bpIdx          =  0;

  for ( i = 0; i < graph->initN; i++) {
    int   j;

    if (durP) durFrom[graph->init[i]] = 1;

    bpTableA[from][bpIdx].stateX =  graph->init[i];
    bpTableA[from][bpIdx].fromX  = -1;
    bpTableA[from][bpIdx].score  =  snsP->scoreFctP( snsP, graph->senoneX[i], from);

    if ( path->pitemListA[0].itemN) {
      float senoneMissPen = path->senoneMissPen;
      float phoneMissPen  = path->phoneMissPen;
      float wordMissPen   = path->wordMissPen;

      for ( j = 0; j < path->pitemListA[0].itemN; j++) {
        PathItem* pi  = path->pitemListA[0].itemA + j;
        int       stX = bpTableA[from][bpIdx].stateX;

        if ( pi->senoneX   != -1 && pi->senoneX == graph->senoneX[stX])
             senoneMissPen  =  pi->alpha;
        if ( pi->phoneX    != -1 && pi->phoneX  == graph->phoneX[stX])
             phoneMissPen   =  pi->beta;
        if ( pi->wordX     != -1 && pi->wordX   == graph->wordX[stX])
             wordMissPen    =  pi->gamma;
      }
      bpTableA[from][bpIdx].score += senoneMissPen + phoneMissPen + wordMissPen;
    }
    bpIdx++;
  }
  bpTableN[from] =  bpIdx;
  bpTableA[from] = (FACell*)realloc( bpTableA[from], bpIdx * sizeof(FACell));

  qsort( bpTableA[from], bpIdx, sizeof(FACell), compareFACell);

  /* For each frame do a forward path */

  for ( frX = from; frX < to; frX++) {
    FACell* bp   =  bpTableA[frX];
    int     bpN  = (topN && topN < bpTableN[frX]) ? topN : bpTableN[frX];
    int     nfrX =  frX + 1;

    bpTableA[nfrX] = (FACell*)malloc( stN * sizeof(FACell));
    /* check memory allocation */
    if (bpTableA[nfrX] == NULL ){
      SERROR( "<VIT,CB>Failed to alloc memory in viterbi(%s,%s).\n",path->name, hmm->name); 
      ret = TCL_ERROR;
      goto skipBacktrace;   
    }

    bpIdx          =  0;

    memset( bpTableX,  0, stN    * sizeof(int));
    memset( faActiveA, 0, scoreN * sizeof(char));

    for ( bpX = 0; bpX < bpN; bpX++, bp++) {

      if ( bp->score < path->pitemListA[frX-from].beam + 
                       bpTableA[frX][0].score) {

        int stX         = bp->stateX;
	int phoneX      = graph->phoneX[stX];
	int amodelX     = pgraphP->amodel[phoneX];
	AModel *amodelP = amsP->list.itemA+amodelX;
	int durX        = amodelP->durX;
        int trX;

        for ( trX = 0; trX < graph->fwdTrN[stX]; trX++) {
          int     toX   = stX + graph->fwdIdx[stX][trX];
	  float   score = bp->score;
	  int    phoneY = 0;
	  int     toBpX = 0;

	  if (toX < 0 || toX >= stN) continue;
	  phoneY = graph->phoneX[toX];
        
	  if (durP) {
	    if (phoneY == phoneX) 
	      durTo[toX]=durFrom[stX]+1;
	    else
	      score += durSetScore(durP,durX,durFrom[stX]);
	  }

          toBpX  = bpTableX[toX];
          score += graph->fwdPen[stX][trX];

          if (! toBpX) {
            FACell* toBp  = bpTableA[nfrX] + bpIdx++;
            toBp->stateX  = toX;
            toBp->score   = score;
            toBp->fromX   = bpX;
            bpTableX[toX] = bpIdx;
          }
          else {
            FACell* toBp = bpTableA[nfrX]+toBpX-1;
            if ( score < toBp->score) {
              toBp->score = score;
              toBp->fromX = bpX;
            }
          }
        }
      }
    }

    bpTableN[nfrX] =  bpIdx;
    bpTableA[nfrX] = (FACell*)realloc( bpTableA[nfrX], bpIdx*sizeof(FACell));
    bp             =  bpTableA[nfrX];
    bpN            =  bpTableN[nfrX];

    /* add the log of the observation probabilities to the new frame
       of FACells allocated and filled above */

    for ( bpX = 0; bpX < bpN; bpX++, bp++) {
      int   snX = graph->senoneX[bp->stateX];
      int   j;

      if ( faActiveA[snX]) bp->score += faScoreA[snX];
      else {
           bp->score += faScoreA[snX] = snsP->scoreFctP( snsP, snX, nfrX);
           faActiveA[snX] = 1;
      }

      if ( path->pitemListA[nfrX-from].itemN) {
        float senoneMissPen = path->senoneMissPen;
        float phoneMissPen  = path->phoneMissPen;
        float wordMissPen   = path->wordMissPen;

        for ( j = 0; j < path->pitemListA[nfrX-from].itemN; j++) {
          PathItem* pi  = path->pitemListA[nfrX-from].itemA + j;
          int       stX = bpTableA[nfrX][bpX].stateX;

          if ( pi->senoneX   != -1 && pi->senoneX == graph->senoneX[stX])
               senoneMissPen  =  pi->alpha;
          if ( pi->phoneX    != -1 && pi->phoneX  == graph->phoneX[stX])
               phoneMissPen   =  pi->beta;
          if ( pi->wordX     != -1 && pi->wordX   == graph->wordX[stX])
               wordMissPen    =  pi->gamma;
        }
        bp->score += senoneMissPen + phoneMissPen + wordMissPen;
      }
    }
    qsort( bpTableA[nfrX], bpIdx, sizeof(FACell), compareFACell);

    /*  the duration counter*/
    if (durP) {
      for (i=0;i<stN;i++) 
	durFrom[i]=durTo[i];
    }

    if (bpCleanUpMod >= 0 && frX % bpCleanUpMod == 0) {
      /* clean up th bpTable every bpCleanUpMod and go back in history bpCleanUpMod * bpCleanUpMul Frames */
      if (bpCleanUpMul > 0) {
	compressBpTabel(bpTableN, bpTableA, MAX(from,frX - (bpCleanUpMod * bpCleanUpMul) ), frX);
      } else {
	compressBpTabel(bpTableN, bpTableA, from , frX);
      }
    }


  }  /* end foreach frame*/

  /* Find the best exit transition */

  bestExitScore = BIGNUM;
  bestExitX     = -1;

  for ( i   = 0; i < bpTableN[to]; i++) {
    int   stX   = bpTableA[to][i].stateX;
    float score = bpTableA[to][i].score;
    int   trX;

    for ( trX = 0; trX < graph->fwdTrN[stX]; trX++) {
      if ( stX + graph->fwdIdx[stX][trX] >= stN) {
        if ( score < bestExitScore) {
          bestExitScore = score;
          bestExitX     = i;
        }
      }
    }     
  }

  /* Go through the backtrace and fill out the path... */

  if ( bestExitX < 0) {
    SERROR( "<VIT,CB>Failed to backtrace in viterbi(%s,%s).\n",path->name, hmm->name); 
    path->gscore = (-1.0);

    for ( frX = to-from; frX >= 0; frX--) 
      pathItemListClear( path->pitemListA + frX);

    ret = TCL_ERROR;
    goto skipBacktrace;
  }

  pathItemListClear( path->pitemListA + to-from);
  pathItemListAdd(   path->pitemListA + to-from, 1, 
                     bpTableA[to][bestExitX].stateX,
                     graph->senoneX[bpTableA[to][bestExitX].stateX],
                     graph->phoneX[ bpTableA[to][bestExitX].stateX],
                     graph->wordX[  bpTableA[to][bestExitX].stateX]);

  path->pitemListA[to-from].itemA[0].gamma = 1.0;
  path->pitemListA[to-from].logScale       = 0.0;
  path->pitemListA[to-from].score          = bpTableA[to][bestExitX].score;
  path->pitemListA[to-from].best           = bpTableA[to][0].score;

  bestExitX = bpTableA[to][bestExitX].fromX;

  for (frX = to-from-1; frX >= 0; frX--) {
    int stX = bpTableA[frX+from][bestExitX].stateX;

    pathItemListClear( path->pitemListA + frX);
    pathItemListAdd(   path->pitemListA + frX, 1, stX,
                       graph->senoneX[stX], graph->phoneX[stX],
                       graph->wordX[stX]);

    path->pitemListA[frX].itemA[0].gamma = 1.0;
    path->pitemListA[frX].logScale       = 0.0;
    path->pitemListA[frX].score          = bpTableA[frX+from][bestExitX].score;
    path->pitemListA[frX].best           = bpTableA[frX+from][0].score;

    bestExitX = bpTableA[frX+from][bestExitX].fromX;
  }

  /* ----------------------------------------------- */
  /* store the final DTW score in the path structure */
  /* ----------------------------------------------- */

  path->gscore = bestExitScore;

  /* ---------------------------- */
  /* release the allocated memory */
  /* ---------------------------- */

  skipBacktrace:

  /* Free allocated temporary data structures */

  if ( bpTableA) {
    for ( frX = from; frX <= to; frX++)
      if ( bpTableA[frX]) free( bpTableA[frX]);
    free( bpTableA+from);
  }
  if ( bpTableN) free( bpTableN+from);
  if ( bpTableX) free( bpTableX);

  return ret;
}

int viterbiMMIE_OMP( Path* path, HMM* hmm, StateGraph* graph, PhoneGraph *pgraphP, int topN, int bpCleanUpMod, int bpCleanUpMul)
{
  SenoneSet*  snsP      = amodelSetSenoneSet(hmm->amodels);
  AModelSet*  amsP      = hmm->phoneGraph->amsP;
  DurationSet* durP     = NULL;
  FACell**    bpTableA  = NULL;
  int*        bpTableN  = NULL;
  int*        bpTableX  = NULL;
  int         from      = path->firstFrame;
  int         to        = path->lastFrame, frX;
  int         scoreN, stN, i, bpX, bpIdx;
  int         bestExitX;
  float       bestExitScore;
  int         ret = TCL_OK;

  if (hmm->phoneGraph->amsP->durP) 
    durP     = (DurationSet*) hmm->phoneGraph->amsP->durP->mdsP.cd;

  if (! graph) {
    ERROR("<VIT,CMS>Can't make state graph from a word graph.\n");
    return TCL_ERROR;
  }
  if ( from < 0 || from > to) { 
    ERROR("<VIT,UPD>Unreasonable path-delimiters: [%d...%d].\n", from, to);
    return TCL_ERROR;
  }

  stN      =  graph->stateN;
  scoreN   =  faScoreACreate( snsP);
  bpTableA = (FACell**)malloc((to-from+1) * sizeof(FACell*)) - from;
  bpTableN = (int    *)malloc((to-from+1) * sizeof(int))     - from;
  bpTableX = (int    *)malloc((stN)       * sizeof(int));

  memset( bpTableA+from, 0, (to-from+1) * sizeof(FACell*));
  memset( bpTableN+from, 0, (to-from+1) * sizeof(int));
  memset( bpTableX,      0, (stN)       * sizeof(int));

  /* malloc and init duration arrays */
  if (durP) {
    if (stN > durSize) {
      durFrom = (short*) realloc((void*) durFrom,stN*sizeof(short));
      durTo   = (short*) realloc((void*) durTo  ,stN*sizeof(short));
      durSize = stN;
    }
    assert (durFrom && durTo);
    for (i=0;i<stN;i++) {
      durFrom[i]=durTo[i]=0;
    }
  }

  /* Fill out the initial states */
  
  bpTableA[from] = (FACell*)malloc( stN * sizeof(FACell));
    /* check memory allocation */
  if (bpTableA[from] == NULL) {
    SERROR( "<VIT,CB>Failed to alloc memory in viterbi(%s,%s).\n",path->name, hmm->name); 
     ret = TCL_ERROR;
     goto skipBacktrace;   
   }

  bpIdx          =  0;

  for ( i = 0; i < graph->initN; i++) {
    int   j;

    if (durP) durFrom[graph->init[i]] = 1;

    bpTableA[from][bpIdx].stateX =  graph->init[i];
    bpTableA[from][bpIdx].fromX  = -1;
    bpTableA[from][bpIdx].score  =  snsP->scoreFctP( snsP, graph->senoneX[i], from);

    if ( path->pitemListA[0].itemN) {
      float senoneMissPen = path->senoneMissPen;
      float phoneMissPen  = path->phoneMissPen;
      float wordMissPen   = path->wordMissPen;

      for ( j = 0; j < path->pitemListA[0].itemN; j++) {
        PathItem* pi  = path->pitemListA[0].itemA + j;
        int       stX = bpTableA[from][bpIdx].stateX;

        if ( pi->senoneX   != -1 && pi->senoneX == graph->senoneX[stX])
             senoneMissPen  =  pi->alpha;
        if ( pi->phoneX    != -1 && pi->phoneX  == graph->phoneX[stX])
             phoneMissPen   =  pi->beta;
        if ( pi->wordX     != -1 && pi->wordX   == graph->wordX[stX])
             wordMissPen    =  pi->gamma;
      }
      bpTableA[from][bpIdx].score += senoneMissPen + phoneMissPen + wordMissPen;
    }
    bpIdx++;
  }
  bpTableN[from] =  bpIdx;
  bpTableA[from] = (FACell*)realloc( bpTableA[from], bpIdx * sizeof(FACell));

  qsort( bpTableA[from], bpIdx, sizeof(FACell), compareFACell);

  /* For each frame do a forward path */

  for ( frX = from; frX < to; frX++) {
    FACell* bp   =  bpTableA[frX];
    int     bpN  = (topN && topN < bpTableN[frX]) ? topN : bpTableN[frX];
    int     nfrX =  frX + 1;

    bpTableA[nfrX] = (FACell*)malloc( stN * sizeof(FACell));
    /* check memory allocation */
    if (bpTableA[nfrX] == NULL ){
      SERROR( "<VIT,CB>Failed to alloc memory in viterbi(%s,%s).\n",path->name, hmm->name); 
      ret = TCL_ERROR;
      goto skipBacktrace;   
    }

    bpIdx          =  0;

    memset( bpTableX,  0, stN    * sizeof(int));
    memset( faActiveA, 0, scoreN * sizeof(char));

    for ( bpX = 0; bpX < bpN; bpX++, bp++) {

      if ( bp->score < path->pitemListA[frX-from].beam + 
                       bpTableA[frX][0].score) {

        int stX         = bp->stateX;
	int phoneX      = graph->phoneX[stX];
	int amodelX     = pgraphP->amodel[phoneX];
	AModel *amodelP = amsP->list.itemA+amodelX;
	int durX        = amodelP->durX;
        int trX;

        for ( trX = 0; trX < graph->fwdTrN[stX]; trX++) {
          int     toX   = stX + graph->fwdIdx[stX][trX];
	  float   score = bp->score;
	  int    phoneY = 0;
	  int     toBpX = 0;

	  if (toX < 0 || toX >= stN) continue;
	  phoneY = graph->phoneX[toX];
        
	  if (durP) {
	    if (phoneY == phoneX) 
	      durTo[toX]=durFrom[stX]+1;
	    else
	      score += durSetScore(durP,durX,durFrom[stX]);
	  }

          toBpX  = bpTableX[toX];
          score += graph->fwdPen[stX][trX];

          if (! toBpX) {
            FACell* toBp  = bpTableA[nfrX] + bpIdx++;
            toBp->stateX  = toX;
            toBp->score   = score;
            toBp->fromX   = bpX;
            bpTableX[toX] = bpIdx;
          }
          else {
            FACell* toBp = bpTableA[nfrX]+toBpX-1;
            if ( score < toBp->score) {
              toBp->score = score;
              toBp->fromX = bpX;
            }
          }
        }
      }
    }

    bpTableN[nfrX] =  bpIdx;
    bpTableA[nfrX] = (FACell*)realloc( bpTableA[nfrX], bpIdx*sizeof(FACell));
    bp             =  bpTableA[nfrX];
    bpN            =  bpTableN[nfrX];

    /* add the log of the observation probabilities to the new frame
       of FACells allocated and filled above */

    for ( bpX = 0; bpX < bpN; bpX++, bp++) {
      int   snX = graph->senoneX[bp->stateX];
      int   j;

      if ( faActiveA[snX]) bp->score += faScoreA[snX];
      else {
           bp->score += faScoreA[snX] = snsP->scoreFctP( snsP, snX, nfrX);
           faActiveA[snX] = 1;
      }

      if ( path->pitemListA[nfrX-from].itemN) {
        float senoneMissPen = path->senoneMissPen;
        float phoneMissPen  = path->phoneMissPen;
        float wordMissPen   = path->wordMissPen;

        for ( j = 0; j < path->pitemListA[nfrX-from].itemN; j++) {
          PathItem* pi  = path->pitemListA[nfrX-from].itemA + j;
          int       stX = bpTableA[nfrX][bpX].stateX;

          if ( pi->senoneX   != -1 && pi->senoneX == graph->senoneX[stX])
               senoneMissPen  =  pi->alpha;
          if ( pi->phoneX    != -1 && pi->phoneX  == graph->phoneX[stX])
               phoneMissPen   =  pi->beta;
          if ( pi->wordX     != -1 && pi->wordX   == graph->wordX[stX])
               wordMissPen    =  pi->gamma;
        }
        bp->score += senoneMissPen + phoneMissPen + wordMissPen;
      }
    }
    qsort( bpTableA[nfrX], bpIdx, sizeof(FACell), compareFACell);

    /*  the duration counter*/
    if (durP) {
      for (i=0;i<stN;i++) 
	durFrom[i]=durTo[i];
    }

    if (bpCleanUpMod >= 0 && frX % bpCleanUpMod == 0) {
      /* clean up th bpTable every bpCleanUpMod and go back in history bpCleanUpMod * bpCleanUpMul Frames */
      if (bpCleanUpMul > 0) {
	compressBpTabel(bpTableN, bpTableA, MAX(from,frX - (bpCleanUpMod * bpCleanUpMul) ), frX);
      } else {
	compressBpTabel(bpTableN, bpTableA, from , frX);
      }
    }


  }  /* end foreach frame*/

  /* Find the best exit transition */

  bestExitScore = BIGNUM;
  bestExitX     = -1;

  for ( i   = 0; i < bpTableN[to]; i++) {
    int   stX   = bpTableA[to][i].stateX;
    float score = bpTableA[to][i].score;
    int   trX;

    for ( trX = 0; trX < graph->fwdTrN[stX]; trX++) {
      if ( stX + graph->fwdIdx[stX][trX] >= stN) {
        if ( score < bestExitScore) {
          bestExitScore = score;
          bestExitX     = i;
        }
      }
    }     
  }

  /* Go through the backtrace and fill out the path... */

  if ( bestExitX < 0) {
    SERROR( "<VIT,CB>Failed to backtrace in viterbi(%s,%s).\n",path->name, hmm->name); 
    path->gscore = (-1.0);

    for ( frX = to-from; frX >= 0; frX--) 
      pathItemListClear( path->pitemListA + frX);

    ret = TCL_ERROR;
    goto skipBacktrace;
  }

  pathItemListClear( path->pitemListA + to-from);
  pathItemListAdd(   path->pitemListA + to-from, 1, 
                     bpTableA[to][bestExitX].stateX,
                     graph->senoneX[bpTableA[to][bestExitX].stateX],
                     graph->phoneX[ bpTableA[to][bestExitX].stateX],
                     graph->wordX[  bpTableA[to][bestExitX].stateX]);

  path->pitemListA[to-from].itemA[0].gamma = 1.0;
  path->pitemListA[to-from].logScale       = 0.0;
  path->pitemListA[to-from].score          = bpTableA[to][bestExitX].score;
  path->pitemListA[to-from].best           = bpTableA[to][0].score;

  bestExitX = bpTableA[to][bestExitX].fromX;

  for (frX = to-from-1; frX >= 0; frX--) {
    int stX = bpTableA[frX+from][bestExitX].stateX;

    pathItemListClear( path->pitemListA + frX);
    pathItemListAdd(   path->pitemListA + frX, 1, stX,
                       graph->senoneX[stX], graph->phoneX[stX],
                       graph->wordX[stX]);

    path->pitemListA[frX].itemA[0].gamma = 1.0;
    path->pitemListA[frX].logScale       = 0.0;
    path->pitemListA[frX].score          = bpTableA[frX+from][bestExitX].score;
    path->pitemListA[frX].best           = bpTableA[frX+from][0].score;

    bestExitX = bpTableA[frX+from][bestExitX].fromX;
  }

  /* ----------------------------------------------- */
  /* store the final DTW score in the path structure */
  /* ----------------------------------------------- */

  path->gscore = bestExitScore;

  /* ---------------------------- */
  /* release the allocated memory */
  /* ---------------------------- */

  skipBacktrace:

  /* Free allocated temporary data structures */

  if ( bpTableA) {
    for ( frX = from; frX <= to; frX++)
      if ( bpTableA[frX]) free( bpTableA[frX]);
    free( bpTableA+from);
  }
  if ( bpTableN) free( bpTableN+from);
  if ( bpTableX) free( bpTableX);

  return ret;
}
/* ========================================================================
    Forward Backward
   ======================================================================== */
/* ------------------------------------------------------------------------
    FBCell
   ------------------------------------------------------------------------ */

typedef struct {

  int    stateX;
  double alpha;
  double beta;
  double gamma;

} FBCell;

/* ------------------------------------------------------------------------
    compareFBCell
   ------------------------------------------------------------------------ */

static int compareAlphaFBCell(const void * a, const void *b)
{
  FBCell* bpa = (FBCell *)a;
  FBCell* bpb = (FBCell *)b;

  if      ( bpa->alpha > bpb->alpha) return -1;
  else if ( bpa->alpha < bpb->alpha) return  1;
  else                               return  0;
}

static int compareFBCell(const void * a, const void *b)
{
  FBCell* bpa = (FBCell *)a;
  FBCell* bpb = (FBCell *)b;

  if      ( bpa->gamma > bpb->gamma) return -1;
  else if ( bpa->gamma < bpb->gamma) return  1;
  else                               return  0;
}

/* ------------------------------------------------------------------------
    forwardBackward
   ------------------------------------------------------------------------ */

int forwardBackward( Path* path, HMM* hmm, int topN, int width)
{
  SenoneSet*  snsP      = amodelSetSenoneSet(hmm->amodels);
  StateGraph* graph     = hmm->stateGraph;
  float**     trnProb   = NULL;
  FBCell**    bpTableA  = NULL;
  int*        bpTableN  = NULL;
  int*        bpTableX  = NULL;
  int         from      = path->firstFrame;
  int         to        = path->lastFrame, frX;
  double      sum;
  int         stN, scoreN, i, bpX, bpIdx;
  int         bestExitX;
  int         ret = TCL_OK;

  double      senoneMissProb = exp(-path->senoneMissPen);
  double      phoneMissProb  = exp(-path->phoneMissPen);
  double      wordMissProb   = exp(-path->wordMissPen);

  if (! graph) {
    ERROR("<FB,CMS>Can't make state graph from a word graph.\n");
    return TCL_ERROR;
  }
  if ( from < 0 || from > to) { 
    ERROR("<FB,UPD>Unreasonable path-delimiters: [%d...%d].\n", from, to);
    return TCL_ERROR;
  }

  path->gscore =  0.0;
  stN          =  graph->stateN;
  scoreN       =  faScoreACreate( snsP);
  trnProb      =  sgraphLogPen2Prob( graph);
  bpTableA     = (FBCell**)malloc((to-from+1) * sizeof(FBCell*)) - from;
  bpTableN     = (int    *)malloc((to-from+1) * sizeof(int))     - from;
  bpTableX     = (int    *)malloc((stN)       * sizeof(int));

  memset( bpTableA+from, 0, (to-from+1) * sizeof(FACell*));
  memset( bpTableN+from, 0, (to-from+1) * sizeof(int));
  memset( bpTableX,      0, (stN)       * sizeof(int));

  /* Fill out the initial states */

  bpTableA[from] = (FBCell*)malloc( stN * sizeof(FBCell));
  bpIdx          =  0;
  sum            =  0.0;

  for ( i = 0; i < graph->initN; i++) {
    int    j;

    bpTableA[from][bpIdx].stateX =  graph->init[i];
    bpTableA[from][bpIdx].beta   =  0.0;
    bpTableA[from][bpIdx].alpha  =  exp(-snsP->scoreFctP( snsP, graph->senoneX[i], from));

    if ( path->pitemListA[0].itemN) {
      double senoneMissPen = senoneMissProb;
      double phoneMissPen  = phoneMissProb;
      double wordMissPen   = wordMissProb;

      for ( j = 0; j < path->pitemListA[0].itemN; j++) {
        PathItem* pi  = path->pitemListA[0].itemA + j;
        int       stX = bpTableA[from][bpIdx].stateX;

        if ( pi->senoneX   != -1 && pi->senoneX == graph->senoneX[stX])
             senoneMissPen  =  exp(-pi->alpha);
        if ( pi->phoneX    != -1 && pi->phoneX  == graph->phoneX[stX])
             phoneMissPen   =  exp(-pi->beta);
        if ( pi->wordX     != -1 && pi->wordX   == graph->wordX[stX])
             wordMissPen    =  exp(-pi->gamma);
      }
      bpTableA[from][bpIdx].alpha *= senoneMissPen * phoneMissPen * wordMissPen;
    }
    sum += bpTableA[from][bpIdx].alpha;

    bpIdx++;
  }
  bpTableN[from] =  bpIdx;
  bpTableA[from] = (FBCell*)realloc( bpTableA[from], bpIdx * sizeof(FBCell));

  qsort( bpTableA[from], bpIdx, sizeof(FBCell), compareAlphaFBCell);

  /* For each frame do a forward path */

  for ( frX = from; frX < to; frX++) {
    FBCell* bp   =  bpTableA[frX];
    int     bpN  = (topN && topN < bpTableN[frX]) ? topN : bpTableN[frX];
    int     nfrX =  frX + 1;

    bpTableA[nfrX] = (FBCell*)malloc( stN * sizeof(FBCell));
    bpIdx          =  0;

    path->pitemListA[frX-from].logScale = sum;

    memset( bpTableX,  0, stN    * sizeof(int));
    memset( faActiveA, 0, scoreN * sizeof(char));

    if  ( sum == 0.0) {
      ERROR("<FB,CFFP>Cannot find a forward path through HMM %s in %d.\n", hmm->name, frX);
      ret    = TCL_ERROR;
      goto skipFwdBwd;
    }

    for ( bpX = 0; bpX < bpN; bpX++, bp++) {
      int stX = bp->stateX;
      int trX;

      bp->alpha /= sum;

      for ( trX = 0; trX < graph->fwdTrN[stX]; trX++) {
        int    toX   = stX + graph->fwdIdx[stX][trX], toBpX;
        double alpha = bp->alpha;

        if (toX < 0 || toX >= stN) continue;

        toBpX  = bpTableX[toX];
        alpha *= trnProb[stX][trX];

        if (! toBpX) {
          FBCell* toBp  = bpTableA[nfrX] + bpIdx++;
          toBp->stateX  = toX;
          toBp->alpha   = alpha;
          toBp->beta    = 0.0;
          toBp->gamma   = 0.0;
          bpTableX[toX] = bpIdx;
        }
        else bpTableA[nfrX][toBpX-1].alpha += alpha;
      }
    }
    bpTableN[nfrX] =  bpIdx;
    bpTableA[nfrX] = (FBCell*)realloc( bpTableA[nfrX], bpIdx*sizeof(FBCell));
    bp             =  bpTableA[nfrX];
    bpN            =  bpTableN[nfrX];
    sum            =  0.0;

    for ( bpX = 0; bpX < bpN; bpX++, bp++) {
      int snX = graph->senoneX[bp->stateX];
      int j;

      if ( faActiveA[snX]) bp->alpha *= faScoreA[snX];
      else {
           bp->alpha *= faScoreA[snX] = exp(-snsP->scoreFctP( snsP, snX, nfrX));
           faActiveA[snX] = 1;
      }

      if ( path->pitemListA[nfrX-from].itemN) {
        double senoneMissPen = senoneMissProb;
        double phoneMissPen  = phoneMissProb;
        double wordMissPen   = wordMissProb;

        for ( j = 0; j < path->pitemListA[nfrX-from].itemN; j++) {
          PathItem* pi  = path->pitemListA[nfrX-from].itemA + j;
          int       stX = bpTableA[from][bpX].stateX;

          if ( pi->senoneX   != -1 && pi->senoneX == graph->senoneX[stX])
               senoneMissPen  =  exp(-pi->alpha);
          if ( pi->phoneX    != -1 && pi->phoneX  == graph->phoneX[stX])
               phoneMissPen   =  exp(-pi->beta);
          if ( pi->wordX     != -1 && pi->wordX   == graph->wordX[stX])
               wordMissPen    =  exp(-pi->gamma);
        }
        bp->alpha *= senoneMissPen * phoneMissPen * wordMissPen;
      }
      sum += bp->alpha;
    }
    qsort( bpTableA[nfrX], bpIdx, sizeof(FBCell), compareAlphaFBCell);
  }

  if  ( sum == 0.0) {
    ERROR("<FB,CCFP>Cannot complete forward path through HMM %s.\n", hmm->name);
    ret    = TCL_ERROR;
    goto skipFwdBwd;
  }

  /* Find the best exit transition */

  path->pitemListA[to-from].logScale =  sum;
  bestExitX                     =  0;

  for ( i   = 0; i < bpTableN[to]; i++) {
    int   stX   =  bpTableA[to][i].stateX;
    int   trX;

    for ( trX = 0; trX < graph->fwdTrN[stX]; trX++) {
      if ( stX + graph->fwdIdx[stX][trX] >= stN) {
        bpTableA[to][i].beta  = 1.0;
        bpTableA[to][i].gamma = bpTableA[to][i].alpha;
        bestExitX            += 1;
      }
    }
  }

  if  (! bestExitX) {
    ERROR("<FB,CFBP>Cannot find a backward path through HMM %s.\n", hmm->name);
    ret    = TCL_ERROR;
    goto skipFwdBwd;
  }

  /* Backward pass */

  for ( frX = to-1; frX >= from; frX--) {
    FBCell* bp   =  bpTableA[frX];
    int     bpN  =  bpTableN[frX];
    int     nfrX =  frX + 1;
    sum          =  0.0;

    memset( faActiveA, 0, scoreN * sizeof(char));

    for ( bpX = 0; bpX < bpN; bpX++, bp++) {
      int stX = bp->stateX;
      int trX;

      for (  trX = 0; trX < graph->fwdTrN[stX]; trX++) {
        int  toX = stX + graph->fwdIdx[stX][trX], toBpX;

        if ( toX < 0 || toX >= stN) continue;

        if ( (toBpX = bpTableX[toX]) ) {
          FBCell* toBp  = bpTableA[nfrX]+toBpX-1;
          int     snX   = graph->senoneX[toBp->stateX];
          double  score;

          if ( faActiveA[snX]) score = faScoreA[snX];
          else {
              score = faScoreA[snX] = exp(-snsP->scoreFctP( snsP, snX, nfrX));
              faActiveA[snX] = 1;
          }
          bp->beta += toBp->beta * trnProb[stX][trX] * score;
        }
      }
      sum += bp->beta;
    }

    if  ( sum == 0.0) {
      ERROR("<FB,CFBP>Cannot find a backward path through HMM %s.\n", hmm->name);
      ret    = TCL_ERROR;
      goto skipFwdBwd;
    }

    memset( bpTableX, 0, (stN) * sizeof(int));

    bp   =  bpTableA[frX];
    for ( bpX = 0; bpX < bpTableN[frX]; bpX++, bp++) {
      bp->beta             /= sum;
      bp->gamma             = bp->alpha * bp->beta;
      bpTableX[bp->stateX]  = bpX + 1;
    }
  }

  /* For each frame store the path */

  for ( frX = from; frX <= to; frX++) {
    FBCell* bp   =  bpTableA[frX];
    int     bpN  = (width && width < bpTableN[frX]) ? width : bpTableN[frX];

    qsort( bp, bpTableN[frX], sizeof(FBCell), compareFBCell);

    for ( sum  = 0.0, bpX = 0; bpX < bpN; bpX++) 
          sum += bpTableA[frX][bpX].gamma;

    if ( sum == 0.0) {
      ERROR("<FB,CCG>Cannot compute gammas for HMM %s in %d.\n", hmm->name,frX);
      ret    = TCL_ERROR;
      goto skipFwdBwd;
    }

    for ( bpX = 0; bpX < bpN; bpX++)
      if ((float)(bpTableA[frX][bpX].gamma /= sum) <= 0.0) break;

    bpN = bpX;

    pathItemListClear( path->pitemListA + frX - from);
    pathItemListAdd(   path->pitemListA + frX - from, bpN, -1, -1, -1, -1);

    for ( bpX = 0; bpX < bpN; bpX++, bp++) {
      path->pitemListA[frX-from].itemA[bpX].stateX  =  bp->stateX;
      path->pitemListA[frX-from].itemA[bpX].senoneX =  graph->senoneX[bp->stateX];
      path->pitemListA[frX-from].itemA[bpX].phoneX  =  graph->phoneX[bp->stateX];
      path->pitemListA[frX-from].itemA[bpX].wordX   =  graph->wordX[bp->stateX];
      path->pitemListA[frX-from].itemA[bpX].alpha   =  bp->alpha;
      path->pitemListA[frX-from].itemA[bpX].beta    =  bp->beta;
      path->pitemListA[frX-from].itemA[bpX].gamma   =  bp->gamma;
    }
  }

  /* ----------------------------------------------- */
  /* store the final DTW score in the path structure */
  /* ----------------------------------------------- */

  for ( frX = 0; frX < path->pitemListN; frX++) {
    path->gscore                -= log( path->pitemListA[frX].logScale);
    path->pitemListA[frX].score  = path->gscore;
    path->pitemListA[frX].best   = path->gscore;
  }

  /* ---------------------------- */
  /* release the allocated memory */
  /* ---------------------------- */

  skipFwdBwd:

  if ( trnProb) {
    for ( i = 0; i < stN; i++)
      if ( trnProb[i]) free( trnProb[i]);
    free( trnProb);
  }
  if ( bpTableA) {
    for ( frX = from; frX <= to; frX++)
      if ( bpTableA[frX]) free( bpTableA[frX]);
    free( bpTableA+from);
  }
  if ( bpTableN) free( bpTableN+from);
  if ( bpTableX) free( bpTableX);

  return ret;
}
