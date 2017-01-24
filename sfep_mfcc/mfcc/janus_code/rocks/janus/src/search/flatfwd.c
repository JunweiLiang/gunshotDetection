/* ========================================================================
    JANUS      Speech- to Speech Translation
               ------------------------------------------------------------
               Object: Dynamic Programming Flat Forward Search
               ------------------------------------------------------------

    Author  :  Michael Finke
    Module  :  flatfwd.c
    Date    :  $Id: flatfwd.c 700 2000-11-14 12:35:27Z janus $
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
     Revision 3.14  2000/11/14 12:29:33  janus
     Merging branch jtk-00-10-27-jmcd.

     Revision 3.13.24.3  2000/11/08 17:13:13  janus
     Finished making changes required for compilation under 'gcc -Wall'.

     Revision 3.13.24.2  2000/11/06 12:01:39  janus
     Just checking.

     Revision 3.13.24.1  2000/11/06 10:50:29  janus
     Made changes to enable compilation under -Wall.

     Revision 3.13  2000/09/20 22:55:39  janus
     Merging branch jtk-00-09-15-hyu.

     Revision 3.12.2.1  2000/09/20 21:50:43  hyu
     Fixed flatFwd bug with lookAhead.

     Revision 3.12  2000/09/14 17:47:12  janus
     Merging branch jtk-00-09-14-jmcd.

     Revision 3.11.2.1  2000/09/14 16:20:55  janus
     Merged branches jtk-00-09-10-tschaaf, jtk-00-09-12-hyu, and jtk-00-09-12-metze.

     Revision 3.11  2000/09/14 11:37:02  janus
     merging branch jtk-00-09-11-fuegen-2

 * Revision 3.10  2000/09/12  15:06:14  janus
 * Merged branch jtk-00-09-08-hyu.

 * Revision 3.8.1.2  2000/03/05  15:37:58  soltau
 * don't use lookaheads in flat pass
 * 

     Revision 3.8.1.2.16.1  2000/09/12 01:17:54  hyu
     Added acoustic lattice rescoring from Michael Finke.

     Revision 3.8.1.2  2000/03/05 15:37:58  soltau
     don't use lookaheads in flat pass

     Revision 3.8.1.1  2000/03/05 15:06:25  soltau
     made it working with new lookahead interface

     Revision 3.8.1.2  2000/03/05 15:37:58  soltau
     don't use lookaheads in flat pass

     Revision 3.8.1.1  2000/03/05 15:06:25  soltau
     made it working with new lookahead interface

     Revision 3.8  1998/07/13 16:51:24  tschaaf
     Fix problem that occure if the search is canceled
     during publish-Function. We had problems with using
     the repair-flag in dpsBacktraceHypothesis.

     Revision 3.8.1.1  2000/03/05 15:06:25  soltau
     made it working with new lookahead interface

     Revision 3.8  1998/07/13 16:51:24  tschaaf
     Fix problem that occure if the search is canceled
     during publish-Function. We had problems with using
     the repair-flag in dpsBacktraceHypothesis.

     Revision 3.7  1998/06/11 17:50:32  kries
     adaptation of new LM interface

 * Revision 3.6  1998/06/10  15:33:47  tschaaf
 * the flatFwd can now be interupted during search.
 * This can be done during the publish-Function.
 * All You have to do is return a value of 1 in the publish-function
 * For example: if search is an search-Object
 * ==========================================
 * proc doPublish {} {
 * global break_search
 * if {$break_search} {return 1}   ;# this will stop the search that called the publish-function
 * return 0 ;# to make sure that no funny effects occure :-)
 * }
 * search.treeFwd.hypoList setPublish { doPublish }
 * search.flatFwd.hypoList setPublish { doPublish }
 * search configure -verbose 2     ;# do the publish function during search
 * ==========================================
 *
    Revision 3.5  1997/08/27  17:02:25  monika
    patch for bug in fullflat forward
  
    Revision 3.4  1997/07/18 17:57:51  monika
    gcc-clean

    Revision 3.3  1997/07/18 14:00:01  monika
    dfki-clean code

    Revision 3.2  1997/06/17 17:41:43  westphal
    *** empty log message ***

    Revision 1.14  1996/07/03 13:18:42  monika
    changes to work with 64K vocabulary

    Revision 1.13  96/07/01  14:54:26  14:54:26  monika (Monika Woszczyna)
    use float-matrix (acessible) rather than global array for bestscores
    
    Revision 1.12  96/04/23  19:52:59  19:52:59  maier (Martin Maier)
    monika: partial hypos and RUNON, fullFlatFwd
    
    Revision 1.11  96/02/17  19:25:21  19:25:21  finkem (Michael Finke)
    removed old hypoList stuff and added phone duration models
    
    Revision 1.10  1996/01/14  14:17:51  monika
    always give lm-pointer when calling lm routines
    save bestscoreA in accessible FMatrix element

    Revision 1.9  96/01/01  14:22:05  14:22:05  monika (Monika Woszczyna)
    *** empty log message ***
    
    Revision 1.8  1995/09/12  19:50:38  finkem
    Handling of hypo publishing changed

    Revision 1.7  1995/08/29  02:45:55  finkem
    *** empty log message ***

    Revision 1.6  1995/08/17  17:07:20  rogina
    *** empty log message ***

    Revision 1.5  1995/08/17  13:25:00  finkem
    *** empty log message ***

    Revision 1.4  1995/08/06  21:03:42  finkem
    *** empty log message ***

    Revision 1.3  1995/08/06  19:26:33  finkem
    *** empty log message ***

    Revision 1.2  1995/08/03  14:42:45  finkem
    *** empty log message ***

    Revision 1.1  1995/07/25  13:49:57  finkem
    Initial revision

 
   ======================================================================== */

/* don't use lookahead in flat */
#define  DONT_USE_LA  

#define   SEARCH_MODULE

#include "search.h"
#include "wordlist.h"
#include "flatfwd.h"

char flatfwdRCSVersion[]= "@(#)1$Id: flatfwd.c 700 2000-11-14 12:35:27Z janus $";

/* ========================================================================
    Local language model calling
   ======================================================================== */


static float lm_local_TgScore ( int w1, int w2, int w3 ) {
  int w[3];

  if(w1==NILWORD) {
    w[0] = SD.lmXA[w2]; w[1] = SD.lmXA[w3];
    return SC.lmodel->lmP->scoreFct(SC.lmodel->cd,w,2,1);
  } else {
    w[0] = SD.lmXA[w1]; w[1] = SD.lmXA[w2]; w[2]= SD.lmXA[w3];
    return SC.lmodel->lmP->scoreFct(SC.lmodel->cd,w,3,2);
  }    
}

/* ========================================================================
    Flat Forward Locals
   ======================================================================== */

INT dpsPrintScale( SHORT frameN);

/* ------------------------------------------------------------------------
    flatFwdDefaults
   ------------------------------------------------------------------------ */

static FlatFwd flatFwdDefaults;

/* ------------------------------------------------------------------------
    flatFwdInit
   ------------------------------------------------------------------------ */

int flatFwdInit( FlatFwd* ffP, Vocab* vocabP)
{
  ffP->trellisN           = 0;
  ffP->minEFrameN         = flatFwdDefaults.minEFrameN;
  ffP->maxWindow          = flatFwdDefaults.maxWindow;

  ffP->expWordA           = NULL;
  ffP->expWordXA          = NULL;

  ffP->beamWidth          = flatFwdDefaults.beamWidth;
  ffP->phoneBeamWidth     = flatFwdDefaults.phoneBeamWidth;
  ffP->lastPhoneBeamWidth = flatFwdDefaults.lastPhoneBeamWidth;
  ffP->wordBeamWidth      = flatFwdDefaults.wordBeamWidth;
  ffP->lmZ                = flatFwdDefaults.lmZ;

  ffP->topN               = flatFwdDefaults.topN;

  hypoListInit( &(ffP->hypoList),"hypoList", vocabP, 1);

  return TCL_OK;
}

/* ------------------------------------------------------------------------
    flatFwdDeinit
   ------------------------------------------------------------------------ */

int flatFwdDeinit( FlatFwd* ffP)
{
  return hypoListDeinit( &(ffP->hypoList));
}

/* ------------------------------------------------------------------------
    flatFwdCreate
   ------------------------------------------------------------------------ */

int flatFwdCreate( void )
{
  if (! FF.expWordA)  { FF.expWordA  = (char*)malloc(  SD.vocabN   *
                                                       sizeof(char)); }
  if (! FF.expWordXA) { FF.expWordXA = (WordX*)malloc((SD.vocabN+1)*
                                                       sizeof(WordX));}
  return 0;
}

/* ------------------------------------------------------------------------
    flatFwdFree
   ------------------------------------------------------------------------ */
void flatFwdFree( )
{
  if ( FF.expWordA)  { free( (char*)FF.expWordA);  FF.expWordA  = NULL; }
  if ( FF.expWordXA) { free( (char*)FF.expWordXA); FF.expWordXA = NULL; }
}

/* ------------------------------------------------------------------------
    flatFwdConfigureItf
   ------------------------------------------------------------------------ */

static int flatFwdConfigureItf( ClientData cd, char *var, char *val)
{
  FlatFwd* ffP = (FlatFwd*)cd;
  if (! ffP)  ffP = &flatFwdDefaults;

  if (! var) {

    ITFCFG(flatFwdConfigureItf,cd,"beamWidth");
    ITFCFG(flatFwdConfigureItf,cd,"phoneBeamWidth");
    ITFCFG(flatFwdConfigureItf,cd,"wordBeamWidth");
    ITFCFG(flatFwdConfigureItf,cd,"lastPhoneBeamWidth");
    ITFCFG(flatFwdConfigureItf,cd,"z");
    ITFCFG(flatFwdConfigureItf,cd,"topN");
    ITFCFG(flatFwdConfigureItf,cd,"minEFrameN");
    ITFCFG(flatFwdConfigureItf,cd,"maxWindow");
    return TCL_OK;
  }

  ITFCFG_Float( var,val,   "beamWidth",      
                       ffP->beamWidth, 0);
  ITFCFG_Float( var,val,   "phoneBeamWidth",      
                       ffP->phoneBeamWidth, 0);
  ITFCFG_Float( var,val,   "wordBeamWidth",      
                       ffP->wordBeamWidth, 0)
  ITFCFG_Float( var,val,   "lastPhoneBeamWidth",      
                       ffP->lastPhoneBeamWidth, 0);
  ITFCFG_Float( var,val,   "z",      
                       ffP->lmZ, 0);
  ITFCFG_Int(   var,val,   "topN",      
                       ffP->topN, 0);
  ITFCFG_Int(   var,val,   "minEFrameN",      
                       ffP->minEFrameN, 0);
  ITFCFG_Int(   var,val,   "maxWindow",      
                       ffP->maxWindow, 0);
  return TCL_ERROR;
}

/* ------------------------------------------------------------------------
    flatFwdAccessItf
   ------------------------------------------------------------------------ */

static ClientData flatFwdAccessItf (ClientData cd, char *name, TypeInfo **ti)
{ 
  FlatFwd *ffwdP = (FlatFwd*)cd;

  if (*name == '.') { 
    if (name[1]=='\0') {
      itfAppendElement( "hypoList");
      *ti = NULL; return NULL;
    }
    else { 
      if (! strcmp( name+1, "hypoList")) {
        *ti = itfGetType("HypoList");
        return (ClientData)&(ffwdP->hypoList);
      }
    }
  }
  return NULL;
}

/* ------------------------------------------------------------------------
    flatFwdPutsItf
   ------------------------------------------------------------------------ */

static int flatFwdPutsItf( ClientData cd, int argc, char *argv[])
{
 FlatFwd* tfP   = (FlatFwd*)cd;
 int       ac     =  argc - 1;

 if ( itfParseArgv( argv[0], &ac, argv+1, 0,
       NULL) != TCL_OK) return TCL_OK;

 WARN("FlatFwd RCS version: %s (%X)\n",flatfwdRCSVersion,tfP);

  return TCL_OK;
}

/* ========================================================================
    Flat Forward Trellis Structure
   ======================================================================== */

/* ------------------------------------------------------------------------
    AllocFFwdTrellis  for each of the multiphone words in the ffWordXA
                      list a list of trellis objects is allocated and
                      initialized and assigned to the SD.trellisPA[w]
                      array. Single phone words are already permanently
                      allocated by InitSearchTree().
   ------------------------------------------------------------------------ */

static void dpsAllocFFwdTrellis( void )
{
  WordX w;
  int   i;

  assert( RT.mpx);

  for ( i = 0; (w = sc.wl.wordXA[i]) != NILWORD; i++) {
    VWord*    vocabP = SD.vocabA + w;
    RTrellis* rtrP;
    Trellis*  trP, *ptrP;
    int       p;

    if ( vocabP->segN == 1) continue;

   if (SD.trellisPA[w] != NULL) {
     ERROR("trellis not dealocated for word: %d (%s)\n", w,vocabP->spellS);
   } 

    assert( SD.trellisPA[w] == NULL);

    /* Alloc root and all node (list) trellis objects for a given word
       in one memory bloc */

    rtrP = (RTrellis*)malloc( sizeof(RTrellis) + 
                            ( vocabP->segN-2) * sizeof(Trellis));
    trP  = (Trellis*)(rtrP + 1);

    dpsInitRTrellis( rtrP, vocabP);

    ptrP = NULL;
    for ( p = 1; p < vocabP->segN-1; p++, trP++) {

      dpsInitTrellis( trP, vocabP->segXA[p], vocabP->modelXA[p]);

      /* The idea of having negative word ending diphone ids to create
         a list of trellis object with ascending ids where segments with
         zero or positive id denote word end context models. */

      trP->info.weDiId = p + 1 - vocabP->segN;

      if ( ptrP) ptrP->childP = trP;
      else       rtrP->childP = trP;

      ptrP = trP;
    }

    /* Allocate all possible word endings */

    dpsAllocLastPhone( w);

    if ( ptrP) ptrP->childP = SD.trellisPA[w];
    else       rtrP->childP = SD.trellisPA[w];

    SD.trellisPA[w] = (Trellis*)rtrP;
  }
}

static void dpsFreeFFwdTrellis( void )
{
  WordX     w;
  WordX     i;
  int j;
  RTrellis* rtrP;
  WordX p;

  assert( RT.mpx);

  for ( i = 0; (w = sc.wl.wordXA[i]) != NILWORD; i++) {
    VWord*    vocabP = SD.vocabA + w;
    Trellis*  trP, *ntrP;

    if ( vocabP->segN == 1) continue;

    assert( SD.trellisPA[w] != NULL);

    rtrP = (RTrellis*)SD.trellisPA[w];

    rtrP->active      = -1;
    rtrP->bestScore   =  WORSTSCORE;
    for ( j = 0; j < STATESperTRELLIS-1; j++) rtrP->scoreA[j] = WORSTSCORE;

    trP  =  rtrP->childP;

    for ( ; trP; trP = ntrP) {
      ntrP = trP->childP;
      if (trP->info.weDiId >= 0) free( (char*)trP);
    }

    free( (char*)rtrP);
    SD.trellisPA[w] = NULL;
  }

  for ( p = 0; p < SD.sPhoneN; p++) {
    w    =  SD.sPhoneXA[p];
    rtrP = (RTrellis*)SD.trellisPA[w];

    rtrP->active      = -1;
    rtrP->bestScore   =  WORSTSCORE;
    for ( j = 0; j < STATESperTRELLIS-1; j++) rtrP->scoreA[j] = WORSTSCORE;
  }
}

/* ========================================================================
    Evaluate
   ======================================================================== */
/* ------------------------------------------------------------------------
    ComputeFActiveDist   find all distributions that need to be computed
                         because some active trellis refers to it. We
                         consider only the trellis objects of the words
                         in SD.activePA[] which are assumed to be allocated
                         flat. Thus we are sure that the first trellis 
                         object in each list is a root trellis [which is not 
                         true for the tree scenario]
   ------------------------------------------------------------------------ */

void dpsComputeFActiveDist( short frameX)
{
  WordX*    awXA = SD.activeXA[frameX & 0x1];
  int       i    = SD.activeN[ frameX & 0x1];
  WordX     w    = *(awXA++);
  int*      distXA;
  int       s, stateN;
  float     thresh = bestScore + FF.beamWidth; /* HIER */

  memset( DS.activeDistA, 0, DS.distN * sizeof(char));

  DS.activeDistXN = 0;

  for ( ; i > 0; --i, w = *(awXA++)) {
    RTrellis* rtrP = (RTrellis*)SD.trellisPA[w];
    Trellis*  trP;

    if ( rtrP->active == frameX) {
      stateN = SD.modelA[rtrP->modelXA[0]].stateN;
      for ( s = 0; s < stateN; s++) {
        if (rtrP->scoreA[s] < thresh) { /* HIER */
          distXA = SD.modelA[rtrP->modelXA[s]].senone;
          if (!DS.activeDistA[distXA[s]]) {
            DS.activeDistA[distXA[s]] = 1;
            DS.activeDistXA[DS.activeDistXN++] = distXA[s];
          }
        }
      }
    }
    for ( trP = rtrP->childP; trP; trP = trP->childP) {
      if ( trP->active == frameX) {
        distXA = SD.modelA[trP->modelX].senone;
        stateN = SD.modelA[trP->modelX].stateN;
        for (s = 0; s < stateN; s++) {
	  if (trP->scoreA[s] < thresh)  /* HIER */ {
            if (!DS.activeDistA[distXA[s]]) {
              DS.activeDistA[distXA[s]] = 1;
              DS.activeDistXA[DS.activeDistXN++] = distXA[s];
            }
	  }
        }
      }
    }
  }
}

/* ------------------------------------------------------------------------
    EvaluateFFwdTrellis  evaluates all trellis objects in the list of
                         active words. It is assumed that the first trellis
                         in each list is root trellis, which is in general
                         only true for the flat forward pass. The best
                         score over all trellis objects except the finish
                         word trellis will be returned.
   ------------------------------------------------------------------------ */

static float dpsEvaluateFFwdTrellis( short frameX)
{
  WordX*    awXA = SD.activeXA[frameX & 0x1];
  int       i    = SD.activeN[ frameX & 0x1];
  WordX     w    = *(awXA++);
  float     best = WORSTSCORE;

  for ( ; i > 0; --i, w = *(awXA++)) {
    RTrellis* rtrP = (RTrellis*)SD.trellisPA[w];
    Trellis*  trP;

    if ( rtrP->active == frameX) {

      rtrP->bestScore = SD.modelA[rtrP->modelXA[0]].evalFn->evalMpx( 
                        SD.modelA, rtrP->modelXA, DS.distScoreA,
                        rtrP->scoreA, rtrP->pathA, SD.durP);

      if ( best > rtrP->bestScore && w != SD.finishWordX)
           best = rtrP->bestScore;
    }

    for ( trP = rtrP->childP; trP; trP = trP->childP) {
      if ( trP->active == frameX) {

        trP->bestScore = SD.modelA[trP->modelX].evalFn->eval(
                         SD.modelA + trP->modelX, DS.distScoreA,
                         trP->scoreA, trP->pathA, SD.durP);

        if ( best > trP->bestScore) best = trP->bestScore;
      }
    }
  }
  return DS.bestScoreFMP->matPA[0][frameX] = best;
}

/* ========================================================================
    Expand
   ======================================================================== */
/* ------------------------------------------------------------------------
    ExpandFFwdTrellis    expands/prunes all trellis objects in the list of
                         active words. It is assumed that the first trellis
                         in each list is a root trellis, which is in general
                         only true for the flat forward pass.
   ------------------------------------------------------------------------ */

static int dpsExpandFFwdTrellis( short frameX)
{
  int    cf              = frameX, nf = cf + 1;
  WordX* awXA            = SD.activeXA[frameX & 0x1];
  int    i               = SD.activeN[ frameX & 0x1];
  WordX  w               = *(awXA++);
  float  thresh          = bestScore + FF.beamWidth;
  float  phoneThresh     = bestScore + FF.phoneBeamWidth;
  float  lastPhoneThresh = bestScore + FF.lastPhoneBeamWidth;
  float  wordThresh      = bestScore + FF.wordBeamWidth;

  memset((char*)SD.activeA, 0, SD.vocabN * sizeof(char));

  for ( ; i > 0; --i, w = *(awXA++)) {
    RTrellis* rtrP   = (RTrellis*)SD.trellisPA[w];
    Trellis*  trP, *ntrP;

    if ( rtrP->active == cf && rtrP->bestScore < thresh) {
      float score  = rtrP->scoreA[FINALSTATE];
      rtrP->active = nf;
      SD.activeA[w] = 1;

      /* Check transitions leaving the root trellis object */

      if ( (trP = rtrP->childP)) {
        assert( (SD.vocabA + w)->segN > 1);

        if ((score += SC.phoneInsertionPenalty) < phoneThresh) {

          if ( trP->info.weDiId >= 0) { /* context dependent "leaf" trellis */

#ifdef DONT_USE_LA
            if ( score < lastPhoneThresh) {
#else
            if ( score + dpsLaScore(frameX,trP,0) < lastPhoneThresh) {
#endif
              for ( ; trP; trP = trP->childP) {
                if ( trP->active < cf || score < trP->scoreA[0]) {

                  trP->scoreA[0]    = score;
                  trP->pathA[0]     = rtrP->pathA[FINALSTATE];
                  trP->pathA[0].cnt = 1;
                  trP->active       = nf;
                }
              }
            }
          }
          else { /* "node" trellis */
#ifdef DONT_USE_LA
            if ( score  < phoneThresh &&  \
		 ( trP->active < cf || score < trP->scoreA[0])) {
#else
            if ( score + dpsLaScore(frameX,trP,0) < phoneThresh && 
		 ( trP->active < cf || score < trP->scoreA[0])) {
#endif
              trP->scoreA[0]    = score;
              trP->pathA[0]     = rtrP->pathA[FINALSTATE];
              trP->pathA[0].cnt = 1;
              trP->active       = nf;
            }
          }
        }
      }
      else {
        assert( (SD.vocabA + w)->segN == 1);

        if ( score < wordThresh)
          dpsSaveBackPointer( frameX, w, score, rtrP->pathA[ FINALSTATE], 0);
      }
    }

    /* For all "node" and "leaf" trellis objects we consider pruning or
       expanding to the next object */

    for ( trP = rtrP->childP; trP; trP = trP->childP) {
     if ( trP->active >= cf) {
      if ( trP->bestScore < thresh) {
        float score  = trP->scoreA[FINALSTATE];
        trP->active  = nf;
        SD.activeA[w] = 1;

        if ( trP->info.weDiId < 0) { /* "node" trellis */

          if ((score += SC.phoneInsertionPenalty) < phoneThresh) {
            ntrP = trP->childP;
            if ( ntrP->info.weDiId >= 0) {
#ifdef DONT_USE_LA
              if ( score < lastPhoneThresh) {
#else
              if ( score + dpsLaScore(frameX,trP,0) < lastPhoneThresh) {
#endif
                for ( ; ntrP; ntrP = ntrP->childP) {
                  if ( ntrP->active < cf || score < ntrP->scoreA[0]) {

                    ntrP->scoreA[0]    = score;
                    ntrP->pathA[0]     = trP->pathA[FINALSTATE];
                    ntrP->pathA[0].cnt = 1;
                    ntrP->active       = nf;
                  }
                }
              }
            }
            else {
#ifdef DONT_USE_LA
              if ( score < phoneThresh &&
                 ( ntrP->active < cf || score < ntrP->scoreA[0])) {
#else
             if ( score + dpsLaScore(frameX,trP,0) < phoneThresh &&
                 ( ntrP->active < cf || score < ntrP->scoreA[0])) {
#endif
                ntrP->scoreA[0]    = score;
                ntrP->pathA[0]     = trP->pathA[FINALSTATE];
                ntrP->pathA[0].cnt = 1;
                ntrP->active       = nf;
              }
            }
          }
        }
        else {
          if ( score < wordThresh)
            dpsSaveBackPointer( frameX, w, score, trP->pathA[ FINALSTATE], 
                                 trP->info.weDiId);
        }
      }
      else if ( trP->active != nf) {
        short s;
        trP->bestScore = WORSTSCORE;
        for ( s = 0; s < STATESperTRELLIS-1; s++) trP->scoreA[s] = WORSTSCORE;
      }
     }
    }
  }
  return 0;
}

/* ========================================================================
    Word Transitions
   ======================================================================== */
/* ------------------------------------------------------------------------
    dpsGetExpWordList   creates a list of all words that started within
                         a certain window of frames and were considered
                         as likely when the structure ffWordLstPA[] was
                         filled below [Flat Forward Initialization].
   ------------------------------------------------------------------------ */

static short dpsGetExpWordList( short frameX, short window)
{
  short sFrameX = frameX - window;
  short eFrameX = frameX + window;
  short nwd = 0, i;

  if ( sFrameX <  0)          sFrameX = 0;
  if ( eFrameX >= DS.frameN) eFrameX = DS.frameN;
  memset( FF.expWordA, 0, SD.vocabN * sizeof(char));

  for ( i = sFrameX; i < eFrameX; i++) {
    WordNode* nodeP = sc.wl.wordListPA[i];

    for ( ; nodeP; nodeP = nodeP->nextP) {
      if (! FF.expWordA[nodeP->wordX]) {

        FF.expWordXA[nwd++]        = nodeP->wordX;
        FF.expWordA[ nodeP->wordX] = 1;
      }
    }
  }
  FF.expWordXA[nwd] = NILWORD;
  return nwd;
}

/* ------------------------------------------------------------------------
    dpsFFwdWordTransitions
   ------------------------------------------------------------------------ */

static void dpsFFwdWordTransitions( short frameX)
{
  short     cf = frameX, nf = cf + 1;
  RTrellis* rtrP;
  float     phoneThresh   = bestScore + FF.phoneBeamWidth;
  float     bestSilScore  = WORSTSCORE;
  WordX*    awXA          = SD.activeXA[cf & 0x1];
  BackP*    bp            = BP.tableA;
  WordX     w,b, bestSilBp = 0, bpN;
  int       i, s;
  int*      weDiRelX;
  float*    weScoreStack;
  float     score;

  dpsGetExpWordList( cf, FF.maxWindow);

  bpN = (BP.idx < FF.topN || FF.topN < 1) ? BP.idx : FF.topN;

  for ( b = 0; b < bpN; b++, bp++) {
    VWord* vocabP  = SD.vocabA + bp->wordX;
    VWord* nvocabP;
  
    if ( bp->wordX == SD.finishWordX) continue;

    weDiRelX     = (bp->weDiX >= 0) ? SD.xweModelA[bp->weDiX].relXA : zeroA;
    weScoreStack =  BP.stackA + bp->stackX;

    /* Check transitions to all words in the expand list */

    for ( i = 0; (w = FF.expWordXA[i]) != NILWORD; i++) {
      nvocabP = SD.vocabA + w;
      score   = weScoreStack[weDiRelX[nvocabP->segXA[0]]] +
                lm_local_TgScore(bp->prevWordX,bp->realWordX,nvocabP->realX) +
                SC.phoneInsertionPenalty;

      if ( score < phoneThresh) {

        rtrP = (RTrellis*)SD.trellisPA[w];

#ifdef DONT_USE_LA
        if ( score  < phoneThresh &&
           ( rtrP->active < cf || score < rtrP->scoreA[0])) {
#else
       if ( score + dpsLaScorePhone(frameX,nvocabP->segXA[0]) < phoneThresh &&
           ( rtrP->active < cf || score < rtrP->scoreA[0])) {
#endif
          rtrP->scoreA[0]       = score;
          rtrP->pathA[0].frameX = cf;
          rtrP->pathA[0].bpX    = b;
          rtrP->pathA[0].cnt    = 1;
          rtrP->active          = nf;
          SD.activeA[w]         = 1;

          rtrP->modelXA[0] = SD.xwbModelA[rtrP->dPhoneX].fullXA[vocabP->
                             segXA[vocabP->segN-1]];
        }
      }
    }

    if ( weScoreStack[weDiRelX[SD.wBoundaryX]] < bestSilScore) {

      bestSilScore = weScoreStack[weDiRelX[SD.wBoundaryX]];
      bestSilBp    = b;
    }    
  }
  
  /* Use the information gathered above to decide whether to expand to
     silence word */

  score = bestSilScore + SC.silenceWordPenalty + SC.phoneInsertionPenalty;

  if ( score < phoneThresh && score < WORSTSCORE) {
    w    = SD.silenceWordX;
    rtrP = (RTrellis*)SD.trellisPA[w];

    if ( rtrP->active < cf || score < rtrP->scoreA[0]) {
      rtrP->scoreA[0]       = score;
      rtrP->pathA[0].frameX = cf;
      rtrP->pathA[0].bpX    = bestSilBp;
      rtrP->pathA[0].cnt    = 1;
      rtrP->active          = nf;
      SD.activeA[w]         = 1;
    }
  }

  /* Transition to Filler words */

  score = bestSilScore + SC.fillerWordPenalty + SC.phoneInsertionPenalty;

  if ( SD.firstX > 3 && score < phoneThresh && score < WORSTSCORE) {
    for ( w = 0; w < SD.firstX; w++) {

      if ( w == SD.silenceWordX || ! ISA_FILLER(w)) continue;

      rtrP = (RTrellis*)SD.trellisPA[w];

      if ( rtrP->active < cf || score < rtrP->scoreA[0]) {
        rtrP->scoreA[0]       = score;
        rtrP->pathA[0].frameX = cf;
        rtrP->pathA[0].bpX    = bestSilBp;
        rtrP->pathA[0].cnt    = 1;
        rtrP->active          = nf;
        SD.activeA[w]         = 1;
      }
    }
  }

  /* Reset all root trellis object that have become inactive even after the
     word transitions */

  i = SD.activeN[ cf & 0x1];

  for ( w = *(awXA++); i > 0; --i, w = *(awXA++)) {
    rtrP = (RTrellis*)SD.trellisPA[w];

    if ( rtrP->active == cf) {
      for ( s = 0; s < STATESperTRELLIS-1; s++) rtrP->scoreA[s] = WORSTSCORE;
      rtrP->bestScore = WORSTSCORE;
    }
  }
}

/* ========================================================================
    Flat Forward
   ======================================================================== */
/* ------------------------------------------------------------------------
    InitFFwdOncePerUtterance
    mode: 0 allocate wl from backtrace if pointer is NULL, else copy
    mode: 1 fullflatfwd --> dummy wl
   ------------------------------------------------------------------------ */

static void dpsInitFFwdOncePerUtterance( short frameN, Search * sP, int mode, WordList * wlP)
{
  RTrellis* rtrP;
  WordX     w;
  int       s;

  if (mode == 0) {
    if (!wlP) {
        wordListAlloc(&(SC.wl),sP,FF.minEFrameN); /* extract forward list from BPTable      */
    } else {
        wordListCopy(&(SC.wl),wlP);
    }
  }
  else if (mode ==1)
    wordListAllocDummy (&(SC.wl),sP); /* make all words possible everywhere */

  if ( BP.frameN) bpTableFree();  /* allocate new table of BackPointers */
  bpTableCreate( frameN);
  lm_CacheReset(SC.lmodel);      /* reset language model cache */

  dpsDsReset(frameN);

#ifndef DONT_USE_LA
  dpsLaReset(frameN); 
#endif

  wordTransC = wordTransN = wordTransM = 0;
  compDistN  = compDistPerFrameN = 0;
  bestScore  = WORSTSCORE;

  for ( w = 0; w < SD.vocabN; w++) {
    BP.wordTblXA[w]           =  NILWORD;
    RT.lWordTransA[w].sFrameX = -1;
  }

  rtrP = (RTrellis*)SD.trellisPA[SD.startWordX];

  for ( s = 0; s < STATESperTRELLIS; s++) {
    rtrP->scoreA[s]       = WORSTSCORE;
    rtrP->pathA[s].frameX = NILBPTR;
    rtrP->pathA[s].bpX    = 0;
    rtrP->pathA[s].cnt    = 1;
  }
  rtrP->bestScore =  WORSTSCORE;
  rtrP->scoreA[0] =  0;
  rtrP->active    =  0;

  dpsAllocFFwdTrellis();     /* allocated flat trellis structure       */

  DS.frameX  = -1;           /* reset distribution scoring frame       */

  SD.activeXA[0][0] = SD.startWordX;
  SD.activeN[0]     = 1; SD.activeN[1] = 0;
  RT.activeN[0]     = 0; RT.activeN[1] = 0;
}

/* ------------------------------------------------------------------------
    DeinitFFwdOncePerUtterance
    Note: mode is currently not required. WordLists are freed in
          the alloc routine where necessary (no longer here)
          cauz we may want to keep em.
   ------------------------------------------------------------------------ */

static void dpsDeinitFFwdOncePerUtterance(WordList* wlP)
{
  dpsFreeFFwdTrellis();
  memset( (char*)SD.activeA, 0, SD.vocabN * sizeof(char));
}

/* ------------------------------------------------------------------------
    dpsFFwdOneFrame   evaluates all trellis objects for the given frame,
                       tries to expand or prune certain objects and tests
                       for word transitions.
   ------------------------------------------------------------------------ */

static void dpsFFwdOneFrame( short frameX)
{
  int    nf    = frameX + 1;
  WordX* nawXA = SD.activeXA[nf & 0x1];
  int    i;
  WordX  w,j;

  dpsBpNextFrame( frameX);                    /* set beginning of new frame */
#ifndef DONT_USE_LA
  dpsLaNextFrame( frameX); 
#endif
  if ( ! DS.compAllDist) {/* Calculate distribution scores */

    dpsComputeFActiveDist( frameX);

    compDistN += compDistPerFrameN = 
                 dpsComputeDistScore( frameX, DS.activeDistA);
  }
  else {
    if (! DS.compAllDist) dpsComputeFActiveDist( frameX);

    compDistN += compDistPerFrameN = dpsComputeDistScore( frameX, 
                (DS.compAllDist) ? NULL : DS.activeDistA);
  }

  /* Reuse lookahead scores if computed
     and evaluate all active trellis objects 
   */

#ifndef DONT_USE_LA
  if (LA.matchfN > 0) {
     LA.phoneScoreA = LA.phoneScoreM[frameX]; 
     /* dpsLaNextFrame( frameX); HIER */
     /* bestPhoneScore = dpsEvaluatePhoneScores( frameX); HIER */
  } else {
    LA.phoneScoreA = LA.phoneScoreM[0];
  }
#endif
  bestScore      = dpsEvaluateFFwdTrellis( frameX);

  dpsExpandFFwdTrellis(   frameX);
  dpsBpSort();
  dpsFFwdWordTransitions( frameX);
  dpsBpSaveFrame(         frameX,FF.topN);

  /* Create a new list of active words. Include all active words in the 
     flat forward word list and the begin silence and filler words, but
     not end silence */

  for ( i = 0, j = 0; (w = sc.wl.wordXA[i]) != NILWORD; i++)
    if ( SD.activeA[w]) { *(nawXA++) = w; j++; }

  if ( SD.activeA[SD.startWordX]) { *(nawXA++) = SD.startWordX; j++; }

  for ( w = 0; w < SD.firstX; w++) 
    if ( w != SD.startWordX && w != SD.finishWordX && SD.activeA[w]) { 
      *(nawXA++) = w; j++; }

  SD.activeN[nf & 0x1] = j;

  lm_NextFrame(SC.lmodel);
}

/* ------------------------------------------------------------------------
    FlatForward  flat dynamic programming matching routine based on 
                 the results of a tree forward pass
   ------------------------------------------------------------------------ */

float dpsFlatForward( void* sP, short mode, char* evalS, WordList * wlP)
{
  int   colN = 10;
  short frameX;
  short scFrameNOrig = -1;
  int weHaveAll = 0, runon =0;
  Timer stopit;
  Search * searchP = (Search *) sP;
  if (evalS)
    if ( evalS && (snsFesEval(sc.snsP,evalS) != TCL_OK)) {
       ERROR("Error evaluating feature description file.\n");
       return TCL_ERROR;
    }

  sc.frameN = snsFrameN(sc.snsP, &(sc.from),&(sc.shift),&weHaveAll);
  searchP->searchCfg.frameN = sc.frameN; /* required for wordlist ! */
  if (wlP && wlP->frameN < sc.frameN) {
    INFO( "wordListGrow: frameN %d --> %d\n", wlP->frameN, sc.frameN);
    wordListGrow( wlP, sc.frameN);
    /* if wlP points to search.wordList, update SC.wl too */
    if ( wlP == &(searchP->searchCfg.wl)) {
      sc.wl.wordListPA = wlP->wordListPA;
      sc.wl.frameN     = wlP->frameN;
    }
  }

  if ((!evalS) && (!weHaveAll)) {
     ERROR("Search must always use -eval if in runon mode !.\n");
     return TCL_ERROR;
  }

  runon = !weHaveAll;  /* if we dont have the full sentence now get it later */

  timerReset(&stopit);
  timerStart(&stopit);

  INFO( "Initializing FlatForward\n");

  if (!runon && (sc.frameN < 3)) return 0.0;

  if (!runon) dpsInitFFwdOncePerUtterance( sc.frameN, searchP, mode, wlP);
  else        dpsInitFFwdOncePerUtterance( 2500, searchP, mode, wlP); /* runon mode */

  INFO(" time: (%f secs)\n", timerStop(&stopit));
  timerStart(&stopit);

  if (weHaveAll && SC.verbose < 2) colN = dpsPrintScale(sc.frameN);
  frameX = 0;
  while (1) {
    for (; frameX < sc.frameN; frameX++) {
      dpsFFwdOneFrame( frameX);

      if ( SC.verbose == 2&& frameX % 20 == 0) {
         char objname[1000];
         sprintf(objname,"%s.flatFwd.hypoList",searchP->name);
         dpsBacktraceHypothesis( sc.frameN, 0, 1.0, &FF.hypoList);
         searchCfgUpdate(searchP);
	 /* An TclError in the Publisch Function stopps the Search! */
         if (TCL_ERROR == hypoListPublish (objname)) {
	   INFO("FlatForward stopped at frame %d!\n",frameX);
	   scFrameNOrig  = sc.frameN;
	   sc.frameN     = frameX;
	   weHaveAll     = 1;
	 };
         fflush(stdout); fflush(stderr);
      }
      else if ( SC.verbose < 2 && frameX % colN == 0) { fprintf(STDERR,"|"); }
    }

    if (weHaveAll) break;

    INFO("checking for more data at frame %d\n",sc.frameN);
    if ( evalS && (snsFesEval(sc.snsP,evalS) != TCL_OK)) {
       ERROR("Error evaluating feature description file.\n");
       return TCL_ERROR;
    }
    sc.frameN = snsFrameN(sc.snsP, &(sc.from), &(sc.shift), &weHaveAll);
    if (sc.frameN >= 2500) {
      WARN("too many frames for runon mode, truncating at 2500 (25 secs)\n");
      sc.frameN = 2500;
    }
  }
  if ( SC.verbose < 2) fprintf( STDERR, "\n");

  INFO("WE HAVE ALL, will backtrace\n");

  /* search was not cancled so repair shoud not hurt */
  if (scFrameNOrig != -1) {
    dpsBacktraceHypothesis( scFrameNOrig, 1, FF.lmZ, &FF.hypoList);
  } else {
    dpsBacktraceHypothesis( sc.frameN, 1, FF.lmZ, &FF.hypoList);
  }

  if (!runon) dpsDeinitFFwdOncePerUtterance(wlP);
  else dpsDeinitFFwdOncePerUtterance(wlP);

  INFO( "score %f frames %d -> %.2f wtrans./frame [max. %d].\n", 
         bestTotalScore,
         wordTransC, (float)wordTransN/(float)wordTransC,
         wordTransM);

  INFO("End of Flat Forward (%f secs)\n", timerStop(&stopit));

  fflush( stdout);
  return bestTotalScore;
}

/* ========================================================================
    Flat Initialization
   ======================================================================== */

static Method flatFwdMethod[] = 
{ 
  { "puts", flatFwdPutsItf, NULL },
  {  NULL,  NULL,           NULL } 
};

static TypeInfo flatFwdInfo = { "FlatFwd", 0, -1, flatFwdMethod, 
                                 NULL, NULL,
                                 flatFwdConfigureItf, flatFwdAccessItf, itfTypeCntlDefaultNoLink,
	                        "Search: Flat Forward Module" } ;

static int ffwdInitialized = 0;

int FlatFwd_Init()
{
  if (! ffwdInitialized) {
    itfNewType( &flatFwdInfo);

    flatFwdDefaults.beamWidth          = 100;
    flatFwdDefaults.phoneBeamWidth     = 100;
    flatFwdDefaults.lastPhoneBeamWidth = 100;
    flatFwdDefaults.wordBeamWidth      =  80;
    flatFwdDefaults.lmZ                = 1.0;
    flatFwdDefaults.topN               =   0;
    flatFwdDefaults.minEFrameN         =   4;
    flatFwdDefaults.maxWindow          =  25;
    ffwdInitialized = 1;
  }
  return TCL_OK;
}
