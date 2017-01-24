/* ============================================================================

  WARNING: THIS MODULE IS ONLY EXPERIMENTAL AND MIGHT BE REMOVED FROM
           JANUS WITHOUT PRIOR WARNING. DON'T USE IT. MONIKA WOSZCZYNA.
           
  Time-Stamp: <01 Jul 96 13:50:46 monika>
  
  This is to be an additional pass to the janus JANUS search, that basically
  does the same as the tree pass but runs with very few words and a somewhat
  reduced 'language model'; The points for word transitions can also be
  restricted.
  
  The idea is to get information on the search with a much finer granularity
  than provided by the search itself.

  Currently this only works for the treefwd, non-tree mode has not been
  tested. Also, sanity-checks suggest that
    a) there still might be bugs in the code below
    b) the potential applications might not work out at all
     
  WARNING: THIS MODULE IS ONLY EXPERIMENTAL AND MIGHT BE REMOVED FROM
           JANUS WITHOUT PRIOR WARNING. DON'T USE IT. MONIKA WOSZCZYNA.

 =========================================================================== */

#define SEARCH_MODULE

#include <assert.h>

#include "search.h"
#include "beamopti.h"

char beamoptiRCSVersion[]= "@(#)1$Id: beamopti.c 2390 2003-08-14 11:20:32Z fuegen $";

/* ========================================================================
    Local language model calling
   ======================================================================== */


static float local_lm_BgScore ( int w1, int w2 ) {
  int w[2];

  w[0] = SD.lmXA[w1]; w[1] = SD.lmXA[w2];
  return SC.lmodel->lmP->scoreFct(SC.lmodel->cd,w,2,1);
}

static float local_lm_TgScore ( int w1, int w2, int w3 ) {
  int w[3];

  if(w1==NILWORD) {
    w[0] = SD.lmXA[w2]; w[1] = SD.lmXA[w3];
    return SC.lmodel->lmP->scoreFct(SC.lmodel->cd,w,2,1);
  } else {
    w[0] = SD.lmXA[w1]; w[1] = SD.lmXA[w2]; w[2]= SD.lmXA[w3];
    return SC.lmodel->lmP->scoreFct(SC.lmodel->cd,w,3,2);
  }    
}

static Forced forcedDefaults;

/* ----------------------------------------------------------------------------
 from the tree structure, find out the lmPen and lmMin values asigned to
 phoneme transitions for the words in the hypothesis.
 hm, there is a bug in this one because i go past the last phoneme..
 --------------------------------------------------------------------------- */
static void forcedLmMinInfo(VWord* vWordP, RTrellis* rtrP)
{
 int len         = vWordP->segN;
 int xwbModelX   = vWordP->xwbModelX; /* identifies word begin Model */
 int dPhoneX     = RT.dPhoneXA[xwbModelX]; /* index of root node */
 int segX;
 Trellis* childP = RT.trellisA[dPhoneX].childP;
 Trellis* trP    = rtrP->childP;
    
 rtrP->lp    = RT.trellisA[dPhoneX].lp;
 rtrP->lpMin = RT.trellisA[dPhoneX].lpMin;
 
 for (segX = 1; segX < len-1; segX ++,trP++) {
   /* len-1 because the last phoneme in the tree is not allocated ! */
   assert(childP);
   while (childP->modelX != vWordP->modelXA[segX]) {
     assert(childP);
     childP = childP->nextP;
   }
   trP->lp    = childP->lp;
   trP->lpMin = childP->lpMin;
   childP     = childP->childP;
 }
}
  
/* ------------------------------------------------------------------------- 
  Note on language modelling: if we are running useTgForward, we should
  really use the backtrace to determine the correct pre-predecessor.
 --------------------------------------------------------------------------*/

static void dpsInitForcedOncePerUtterance(short frameN)
{
  Hypo * hypoP = FR.hypoList.hypoPA[0];
  WordX hWordX;
  WordX hWordN = hypoP->hwordN; /* number of words in hypo */
  WordX * realXA  = (WordX *) malloc (hWordN *sizeof (WordX));
  WordX * predXA  = (WordX *) malloc (hWordN *sizeof (WordX));
  WordX * ppredXA = (WordX *) malloc (hWordN *sizeof (WordX));

  FR.rTrellisN  = hWordN;
  FR.rTrellisA  = (RTrellis *)  malloc (hWordN *sizeof (RTrellis));
  FR.backTraceM = (FRTrace ***) malloc (hWordN *sizeof (FRTrace **));

  FR.transiPenA   = (float *) malloc (hWordN *sizeof (float));
  FR.diffScoreM = (float **) malloc (hWordN * sizeof (float*));
  
  fmatrixResize(FR.scoreFMP,frameN,1);
  fmatrixResize(FR.inLastPhoneFMP,frameN,1);
  fmatrixResize(FR.phoneXFMP,frameN,1);
  fmatrixResize(FR.stateXFMP,frameN,1);
  fmatrixResize(FR.hWordXFMP,frameN,1);
  
  for ( hWordX = 0; hWordX < hWordN; hWordX ++ ) {
    int    vWordX = hypoP->hwordA[hWordX].vocabX;
    VWord* vWordP = &SD.vocabA[vWordX];


    int tmpWordX = hWordX;
    realXA[hWordX] = vWordP->realX;
    while (realXA[hWordX] < SD.firstX && 
           realXA[hWordX] != SD.startWordX && 
           realXA[hWordX] != SD.finishWordX) {
      tmpWordX --;
      if (tmpWordX < 0) {
        realXA[hWordX] = -1;
        break;
      }
      else {
         realXA[hWordX] = realXA[tmpWordX];
      }
    }
    predXA[hWordX] = (tmpWordX <1)?-1:realXA[tmpWordX-1];
    ppredXA[hWordX] = (tmpWordX <1)?-1:predXA[tmpWordX-1];
    if (hWordX > 0) {
      if (vWordX == SD.silenceWordX) FR.transiPenA[hWordX-1] =
                                              SC.silenceWordPenalty;
      else if (vWordX < SD.firstX && vWordX != SD.finishWordX 
               && vWordX != SD.startWordX)   FR.transiPenA[hWordX-1] =
                                              SC.fillerWordPenalty;
      else if (SC.use3gFwd) FR.transiPenA[hWordX-1] = 
	      local_lm_TgScore(ppredXA[hWordX],predXA[hWordX],realXA[hWordX]);
      else  FR.transiPenA[hWordX-1] =
	      local_lm_BgScore(predXA[hWordX],realXA[hWordX]);
    }
  }    
  
  for ( hWordX = 0; hWordX < hWordN; hWordX ++ ) {
    int     vWordX = hypoP->hwordA[hWordX].vocabX;
    VWord* vWordP = &SD.vocabA[vWordX];
    Trellis * trP;
    int phX;
    
    dpsInitRTrellis(&FR.rTrellisA[hWordX], vWordP); /* init root node */
    
    FR.backTraceM[hWordX] = 
                 (FRTrace **) malloc(vWordP->segN * sizeof(FRTrace *));
    FR.diffScoreM[hWordX] = (float *) malloc(frameN* sizeof(float));
    FR.backTraceM[hWordX][0] = (FRTrace *) malloc(frameN*(sizeof(FRTrace)));
         
    if (vWordP->segN > 1) {
      FR.rTrellisA[hWordX].childP = trP =
                 (Trellis*) malloc ((vWordP->segN-1) *sizeof(Trellis));

      for (phX = 1; phX < vWordP->segN; phX++, trP++) {
        dpsInitTrellis (trP, vWordP->segXA[phX], vWordP->modelXA[phX]);
        if (phX < vWordP->segN-1) trP->childP = trP+1;
        else trP->childP = NULL;
        FR.backTraceM[hWordX][phX] =(FRTrace*) malloc(frameN* sizeof(FRTrace));
        memset( FR.backTraceM[hWordX][phX], 100, frameN * sizeof(FRTrace));
      }
      
      {
        VWord* nextWordP = &SD.vocabA[hypoP->hwordA[hWordX+1].vocabX]; 
        /* note that the above line is only save because the last
           word of each sentence only has one phoneme */
        int nextPhX = nextWordP->segXA[0];
           
        trP --; /* set focus back on last phoneme for xwt-adjustments */
        
        /* now find the modelXA -- note that the vocab will set them
           all to be the same if we don't have xwt's: */
        trP->modelX = SD.xweModelA[vWordP->xweModelX].fullXA[nextPhX];
      }
      forcedLmMinInfo(vWordP, &FR.rTrellisA[hWordX]);
    }
    
    FR.rTrellisA[0].scoreA[0] = 0;
    FR.rTrellisA[0].bestScore = 0;
  }
  
  free (realXA);   free (predXA);   free (ppredXA);
}

/* ------------------------------------------------------------------------- */

static void dpsDeinitForcedOncePerUtterance( void )
{
  WordX hWordX, hWordN = FR.hypoList.hypoPA[0]->hwordN;

  for ( hWordX = 0; hWordX < hWordN; hWordX ++ ) {
    int     vWordX = FR.hypoList.hypoPA[0]->hwordA[hWordX].vocabX;
    VWord * vWordP = &SD.vocabA[vWordX];  
    int phX;
    if (FR.rTrellisA[hWordX].childP) free (FR.rTrellisA[hWordX].childP);
    for (phX = 0; phX < vWordP->segN; phX ++) {
      free (FR.backTraceM[hWordX][phX]);
    }
    free (FR.backTraceM[hWordX]);
    free (FR.diffScoreM[hWordX]);
  }
  free (FR.rTrellisA);
  free (FR.backTraceM);
  free (FR.transiPenA);
  free (FR.diffScoreM);
  hypoListReinit(&FR.hypoList);
  FR.rTrellisA = NULL;
  FR.backTraceM = NULL;
}

/* ----------------------------------------------------------------------------
  find out which senones need to be computed.
  caveat:
  - preliminary version always returns all senones used for all words
    and recomputes this list every time :-(
  - the frameX will be only needed when we start looking at active trellises
  - also prepares trellis path entries for _next_ frame
 --------------------------------------------------------------------------- */

static void dpsComputeForcedActiveDist( void )
{
  WordX hWordX, hWordN = FR.hypoList.hypoPA[0]->hwordN;
  memset( DS.activeDistA, 0, DS.distN * sizeof(char));
  
  for ( hWordX = 0; hWordX < hWordN; hWordX ++ ) {
    int     vWordX = FR.hypoList.hypoPA[0]->hwordA[hWordX].vocabX;
    VWord * vWordP = &SD.vocabA[vWordX];
    RTrellis * rtrP = &FR.rTrellisA[hWordX];
    Trellis  *  trP = rtrP->childP;
    int phX;
    int * distXA;
    int sX, stateN = SD.modelA[rtrP->modelXA[0]].stateN; 
    
    for (sX = 0; sX< stateN; sX++) {
      distXA = SD.modelA[rtrP->modelXA[sX]].senone;
      DS.activeDistA[distXA[sX]] = 1;
      rtrP->pathA[sX].bpX = sX;
    }
    rtrP->pathA[FINALSTATE].bpX = -88;
   
    for (phX = 1; phX < vWordP->segN; phX++, trP++) {
      distXA = SD.modelA[trP->modelX].senone;
      stateN = SD.modelA[trP->modelX].stateN;
      for (sX=0; sX< stateN; sX++) {
        DS.activeDistA[distXA[sX]] = 1;
        trP->pathA[sX].bpX = sX;
      }
      trP->pathA[FINALSTATE].bpX = -88; /* so we see it better */

    }
  }
}

/* ------------------------------------------------------------------------- */

static void dpsForcedSaveBacktrace(short frameX)
{
  WordX hWordX, hWordN = FR.hypoList.hypoPA[0]->hwordN;
  
  for ( hWordX = 0; hWordX < hWordN; hWordX ++ ) {
    int      vWordX = FR.hypoList.hypoPA[0]->hwordA[hWordX].vocabX;
    VWord  * vWordP = &SD.vocabA[vWordX];
    RTrellis * rtrP = &FR.rTrellisA[hWordX];
    Trellis  *  trP = rtrP->childP;
    int sX, phX = 0;
    int stateN = SD.modelA[rtrP->modelXA[0]].stateN; 
    
    for (sX = 0; sX< stateN; sX++) {
      FR.backTraceM[hWordX][phX][frameX].scoreA[sX] = rtrP->scoreA[sX];
      FR.backTraceM[hWordX][phX][frameX].fromA[sX]  = rtrP->pathA[sX].bpX;
    }
    FR.backTraceM[hWordX][phX][frameX].scoreA[FINALSTATE]=
                                               rtrP->scoreA[FINALSTATE];
    FR.backTraceM[hWordX][phX][frameX].fromA[FINALSTATE]=
                                               rtrP->pathA[FINALSTATE].bpX;
    
    FR.backTraceM[hWordX][phX][frameX].bestScore = rtrP->bestScore;
    
    for (phX = 1; phX < vWordP->segN; phX++, trP++) {
      stateN = SD.modelA[trP->modelX].stateN;
      for (sX=0; sX< stateN; sX++) {
        FR.backTraceM[hWordX][phX][frameX].scoreA[sX]= trP->scoreA[sX];
        FR.backTraceM[hWordX][phX][frameX].fromA[sX]= trP->pathA[sX].bpX;
      }
      FR.backTraceM[hWordX][phX][frameX].scoreA[FINALSTATE]=
                                               trP->scoreA[FINALSTATE];
      FR.backTraceM[hWordX][phX][frameX].fromA[FINALSTATE]=
                                               trP->pathA[FINALSTATE].bpX;
      FR.backTraceM[hWordX][phX][frameX].bestScore = trP->bestScore;
    }
  }
}

/* ------------------------------------------------------------------------- */


static void dpsForcedWithinTrellis( void )
{
  WordX hWordX, hWordN = FR.hypoList.hypoPA[0]->hwordN;
  
  for ( hWordX = 0; hWordX < hWordN; hWordX ++ ) {
    int      vWordX = FR.hypoList.hypoPA[0]->hwordA[hWordX].vocabX;
    VWord  * vWordP = &SD.vocabA[vWordX];
    RTrellis * rtrP = &FR.rTrellisA[hWordX];
    Trellis  *  trP = rtrP->childP;
    int phX;
     
    rtrP->bestScore =  SD.modelA[rtrP->modelXA[0]].evalFn->evalMpx( 
                       SD.modelA, rtrP->modelXA, DS.distScoreA,
                       rtrP->scoreA, rtrP->pathA, SD.durP);
       
    for (phX = 1; phX < vWordP->segN; phX++, trP++) {
       trP->bestScore = SD.modelA[trP->modelX].evalFn->eval(
                        SD.modelA + trP->modelX, DS.distScoreA,
                        trP->scoreA, trP->pathA, SD.durP);
    }
  }
}

/* ------------------------------------------------------------------------- 
   dpsForcedTrellisTransitions (short frameX)
   phoneme transitons are a bit tedious as we have to do the lm lookahead,
   but only if in tree mode.
   -------------------------------------------------------------------------*/

static void dpsForcedTrellisTransitions ( void )
{
  WordX hWordX, hWordN = FR.hypoList.hypoPA[0]->hwordN;
  
  for ( hWordX = 0; hWordX < hWordN; hWordX ++ ) {
    int     vWordX  = FR.hypoList.hypoPA[0]->hwordA[hWordX].vocabX;
    VWord * vWordP  = &SD.vocabA[vWordX];
    RTrellis * rtrP = &FR.rTrellisA[hWordX];
    Trellis  *  trP = rtrP->childP;
    int phX;
    int phN = vWordP->segN;
    float score;
    
    /*--------- root trellis into second trellis ---------------*/

    if (phN > 2) {
      assert (trP);
      score = rtrP->scoreA[FINALSTATE];
      if (FR.treeMode) score += trP->lp;
      if (score < trP->scoreA[0]) {
        trP->scoreA[0] = score;
        trP->pathA[0].bpX = NILWORD;
        trP->pathA[0].frameX = rtrP->pathA[FINALSTATE].frameX;
      }
    }
    else if (phN == 2) {
      assert (trP);
      score = rtrP->scoreA[FINALSTATE];
      if (FR.treeMode && FR.useBacktrace) {
         int fromFrameX = rtrP->pathA[FINALSTATE].frameX;
         score += FR.diffScoreM[hWordX][fromFrameX];
      }
      if (FR.treeMode) score -= rtrP->lpMin; /* correct lp lookaheads */
      if (FR.treeMode) score += FR.transiPenA[hWordX-1];
      if (score < trP->scoreA[0]) {
        trP->scoreA[0] = score;
        trP->pathA[0].bpX = -1;
        trP->pathA[0].frameX = rtrP->pathA[FINALSTATE].frameX;
      }
    }       
    
    /*--------- from node into node------------------------------------*/
    
    if (phN > 2) {
      for (phX = 1; phX < phN-2; phX++, trP++) {
        assert(trP->childP);
        score = trP->scoreA[FINALSTATE];
        if (FR.treeMode) score += trP->childP->lp;
        if (score < trP->childP->scoreA[0]) {
          trP->childP->scoreA[0] = score;
          trP->childP->pathA[0].bpX = -1; 
          trP->childP->pathA[0].frameX = trP->pathA[FINALSTATE].frameX;
        }
      }
    
    /*--------- node into last phoneme --------------------------------*/
      score = trP->scoreA[FINALSTATE];
      assert(trP->childP);
      if (FR.treeMode) score -= trP->lpMin;
      if (FR.treeMode) score += FR.transiPenA[hWordX-1];
      if (FR.treeMode && FR.useBacktrace) {
         int fromFrameX = trP->pathA[FINALSTATE].frameX;
         score += FR.diffScoreM[hWordX][fromFrameX];
      }
      if (score < trP->childP->scoreA[0]) {
        trP->childP->scoreA[0] = score;
        trP->childP->pathA[0].bpX = -1; 
        trP->childP->pathA[0].frameX = trP->pathA[FINALSTATE].frameX;
      }
    }
  }
}

/* -------------------------------------------------------------------------
  for the treePass in the mode that does use the backtrace, we need to
  find the best word-end at that frame (in the backtrace) -- if we
  expand into a word with more than one phoneme only.
  hrmpf -- we should also memorize what it was or we can't correct that
  later (upon going into the last phoneme). If we fix the segmentation,
  we only need to know the best accoustic score for each frame, else for
  each frame and each word (preferably). Make sure our backtrace remembers
  the frame where we entered the word !
 ---------------------------------------------------------------------------*/

static void dpsForcedWordTransitions (short frameX)
{
  WordX hWordX, hWordN = FR.hypoList.hypoPA[0]->hwordN;

  for ( hWordX = 0; hWordX < hWordN -1; hWordX ++ ) {
    int        vWordX = FR.hypoList.hypoPA[0]->hwordA[hWordX].vocabX;
    VWord    * vWordP = &SD.vocabA[vWordX]; 
    RTrellis * nwrtrP = &FR.rTrellisA[hWordX+1]; /* root trellis next word   */
    RTrellis * rtrP   = &FR.rTrellisA[hWordX];   /* root trellis this word   */
    Trellis  * trP    = rtrP->childP;            /* second trellis this word */
    float score;
    if (FR.useSegments 
       && (frameX != FR.hypoList.hypoPA[0]->hwordA[hWordX].weFrameXF)) continue;
    if (trP) {  /* from word with more than one phoneme */
        trP = trP + vWordP->segN-2; /* last trellis this word */
        score = trP->scoreA[FINALSTATE];
    } else {
        score = rtrP->scoreA[FINALSTATE];
    }
    
    if (FR.treeMode && FR.useBacktrace && nwrtrP->childP) {
      /* find the best predecessor word given the first phoneme in nwrtP */
      BackP *     bpe = BP.tablePA[frameX];
      WordX   bpX,bpN = BP.tableN[frameX];
      float bestScore = WORSTSCORE;
      int    toPhoneX = nwrtrP->phoneX;
      int fromPhoneX; /* last phoneme in best predecessor word */
      
      for (bpX =0; bpX < bpN; bpX++, bpe++) {
        VWord * fromWordP    = &SD.vocabA[bpe->wordX];
        int   * weDiRelX     = SD.xweModelA[bpe->weDiX].relXA;
        int     phoneX       = fromWordP->segXA[fromWordP->segN-1];
        float * weScoreStack = BP.stackPA[frameX] + bpe->stackX;
        if (weScoreStack[weDiRelX[toPhoneX]]<bestScore) {
          fromPhoneX = phoneX;
          bestScore  = weScoreStack[weDiRelX[toPhoneX]];
        }
      }
      FR.diffScoreM[hWordX+1][frameX] = score-bestScore;
      /* diffScore gives us the correction needed when entering last phone */
      score = bestScore;
    }
    
    if (!FR.treeMode) score += FR.transiPenA[hWordX];
    else if (!nwrtrP->childP) score += FR.transiPenA[hWordX];
    else score += nwrtrP->lp;
    
    if (score < nwrtrP->scoreA[0]) {
      nwrtrP->scoreA[0] = score;
      nwrtrP->pathA[0].bpX = -1;
      nwrtrP->pathA[0].frameX = frameX;
      nwrtrP->modelXA[0] = 
         SD.xwbModelA[nwrtrP->dPhoneX].fullXA[vWordP->segXA[vWordP->segN-1]];
    }
    
  }
}

/* ------------------------------------------------------------------------- */

static void dpsForcedOneFrame(short frameX)
{
  dpsComputeForcedActiveDist(); /* acutally does some more... */
  dpsComputeDistScore( frameX, DS.activeDistA);
  dpsForcedWithinTrellis();
  dpsForcedTrellisTransitions();
  dpsForcedWordTransitions(frameX);
  dpsForcedSaveBacktrace(frameX);
}

/* -------------------------------------------------------------------------
   dpsForcedBacktrace
   we go backwards from the last state in the last phoneme of the last word ;
   while it's not clear what information we want to gather from the backtrace
   this is far from complete...
   We print out some info per frame to show something is happening.
 --------------------------------------------------------------------------- */

static float dpsForcedBacktrace(short frameN)
{
  int frameX = frameN-1;
  WordX hWordX = FR.hypoList.hypoPA[0]->hwordN -1;
  int     vWordX = FR.hypoList.hypoPA[0]->hwordA[hWordX].vocabX;
  VWord * vWordP = &SD.vocabA[vWordX];
  int phX = vWordP->segN-1;
  int stateX = FINALSTATE;
  float score = FR.backTraceM[hWordX][phX][frameN-1].scoreA[stateX];
  float retval = score;
/*  INFO("frameX: %3d word: %d phone: %d state: %d score %f\n", frameX, hWordX, phX, stateX,score); */

  for (frameX= frameN-1;frameX >=0; frameX--) {
    int oldstateX = stateX;
    stateX = FR.backTraceM[hWordX][phX][frameX].fromA[stateX];
    if (stateX == NILWORD) {
      stateX = FINALSTATE;
      phX --; 
      if (phX < 0) {
        hWordX --;

        vWordX = FR.hypoList.hypoPA[0]->hwordA[hWordX].vocabX;
        vWordP = &SD.vocabA[vWordX];
        phX = vWordP->segN-1;
      }
      score  = FR.backTraceM[hWordX][phX][frameX].scoreA[FINALSTATE];
      stateX = FR.backTraceM[hWordX][phX][frameX].fromA[FINALSTATE];
    } else {
      assert(stateX < FINALSTATE); 
      score  = FR.backTraceM[hWordX][phX][frameX].scoreA[oldstateX];
    }
    
    FR.phoneXFMP->matPA[frameX][0] = (float) phX;            
    FR.hWordXFMP->matPA[frameX][0] = (float) hWordX;
    FR.stateXFMP->matPA[frameX][0] = (float) stateX;
    FR.scoreFMP->matPA[frameX][0] = score;
    if (phX != vWordP->segN-1) {
      FR.inLastPhoneFMP->matPA[frameX][0] = 0; /* not in last phoneme */
    } else {
      FR.inLastPhoneFMP->matPA[frameX][0] = 1; /* in last phoneme */
    }

  }
  return retval;
}

/* ------------------------------------------------------------------------- */

float dpsForced( void* sP, char* evalS)
{
 int weHaveAll = 0;
 int frameX;
 
  if (evalS) {
    if ( evalS && (snsFesEval(sc.snsP,evalS) != TCL_OK)) {
      ERROR("Error evaluating feature description file.\n");
      return TCL_ERROR;
    }
  }  
  sc.frameN = snsFrameN(sc.snsP, &(sc.from),&(sc.shift),&weHaveAll);

  if (!weHaveAll) {
    ERROR("Forced Recognition cannot run in runon mode !");
    return TCL_ERROR;
  }

  if (FR.hypoList.hypoN <= 0) {
      ERROR("Need some infos in hypoList for forced recognition.\n");
      return TCL_ERROR;
  } 
  
  INFO ("Initializing Forced Recognition\n");
  
  dpsInitForcedOncePerUtterance(sc.frameN);
  
  INFO ("Done Initializing\n");
  
  for (frameX = 0; frameX< sc.frameN; frameX ++) {
    dpsForcedOneFrame(frameX);
  }

  dpsForcedBacktrace(sc.frameN);
  dpsDeinitForcedOncePerUtterance();
  
  return 0.0;
}

/* ========================================================================
    Interface Stuff for TCL Interface:
   ======================================================================== */

/* ------------------------------------------------------------------------
  forcedCreate
  allocate all data structures that are allocated only once at the creation
  of the module.
  -------------------------------------------------------------------------*/
int forcedCreate ()
{
 FR.scoreFMP       = fmatrixCreate(1,1);
 FR.inLastPhoneFMP = fmatrixCreate(1,1);
 FR.hWordXFMP       = fmatrixCreate(1,1);
 FR.phoneXFMP      = fmatrixCreate(1,1);
 FR.stateXFMP      = fmatrixCreate(1,1);
 return TCL_OK;
}

/* ------------------------------------------------------------------------
  forcedCreate
  inititalize all data structures to initial values;
  -------------------------------------------------------------------------*/
int forcedInit ( Forced * frP, Vocab* vocabP)
{
 frP->treeMode       = forcedDefaults.treeMode;
 frP->useBacktrace   = forcedDefaults.useBacktrace;
 frP->useSegments    = forcedDefaults.useSegments;
 frP->scoreFMP       = NULL;
 frP->inLastPhoneFMP = NULL;
 frP->phoneXFMP      = NULL;
 frP->hWordXFMP      = NULL;
 frP->stateXFMP      = NULL;
 frP->backTraceM     = NULL;
 frP->transiPenA     = NULL;
 
 hypoListInit (&(frP->hypoList),"hypoList",vocabP,1);
 
 return TCL_OK;
}
   
/* ------------------------------------------------------------------------
   forcedConfigureItf
 ------------------------------------------------------------------------ */

static int forcedConfigureItf( ClientData cd, char *var, char *val)
{
  Forced* frP = (Forced*)cd;
  if (! frP)  frP = &forcedDefaults;

  if (! var) {
    ITFCFG(forcedConfigureItf,cd,"useSegments");
    ITFCFG(forcedConfigureItf,cd,"useBacktrace");
    ITFCFG(forcedConfigureItf,cd,"treeMode");
    return TCL_OK;
  }

  ITFCFG_Int( var,val, "useSegments",  frP->useSegments,  0);
  ITFCFG_Int( var,val, "useBacktrace", frP->useBacktrace, 0);
  ITFCFG_Int( var,val, "treeMode",     frP->treeMode,     0);

  return TCL_ERROR;
}
   
/* ------------------------------------------------------------------------
    forcedAccessItf
   ------------------------------------------------------------------------ */

static ClientData forcedAccessItf (ClientData cd, char *name, TypeInfo **ti)
{ 
  Forced *forcedP = (Forced*) cd;
  if (*name == '.') { 
    if (name[1]=='\0') {
      itfAppendElement( "hypoList");
      itfAppendElement( "scoreA");
      itfAppendElement( "inLastPhoneA");
      itfAppendElement( "hWordXA");
      itfAppendElement( "phoneXA");
      itfAppendElement( "stateXA");
      *ti = NULL; return NULL;
    }
    else { 
      if (! strcmp( name+1, "hypoList")) {
        *ti = itfGetType("HypoList");
        return (ClientData)&(forcedP->hypoList);
      }
      if (! strcmp( name+1, "scoreA")) {
        *ti = itfGetType("FMatrix");
        return (ClientData)(forcedP->scoreFMP);
      }
      if (! strcmp( name+1, "inLastPhoneA")) {
        *ti = itfGetType("FMatrix");
        return (ClientData)(forcedP->inLastPhoneFMP);
      }
      if (! strcmp( name+1, "stateXA")) {
        *ti = itfGetType("FMatrix");
        return (ClientData)(forcedP->stateXFMP);
      }
      if (! strcmp( name+1, "phoneXA")) {
        *ti = itfGetType("FMatrix");
        return (ClientData)(forcedP->phoneXFMP);
      }  
      if (! strcmp( name+1, "hWordXA")) {
        *ti = itfGetType("FMatrix");
        return (ClientData)(forcedP->hWordXFMP);
      }  
    
    }
  }

  return NULL;
}

/* ------------------------------------------------------------------------
    forcedPutsItf
   ------------------------------------------------------------------------ */

static int forcedPutsItf( ClientData cd, int argc, char *argv[])
{
  Forced* boP   = (Forced*)cd;
  assert (boP == boP); /* unused; shut up compiler */
  return TCL_OK;
}

/* ------------------------------------------------------------------------
   ForcedInit
   ------------------------------------------------------------------------ */

static Method forcedMethod[] = 
{ 
  { "puts", forcedPutsItf, NULL },
  {  NULL,  NULL,           NULL } 
};

static TypeInfo forcedInfo = { "Forced", 0, -1, forcedMethod, 
                                NULL, NULL,
                                forcedConfigureItf, forcedAccessItf, 
                                itfTypeCntlDefaultNoLink,
                               "Search: EXPERIMENTAL Beam Optimization Pass" };

static int forcedInitialized = 0;

int Forced_Init() /* called once when this module is loaded */
{
  if (! forcedInitialized) {
    itfNewType(&forcedInfo);

    forcedDefaults.treeMode     = 0;
    forcedDefaults.useBacktrace = 0;
    forcedDefaults.useSegments  = 0;
    
    forcedInitialized = 1;
  }
  return TCL_OK;
}
