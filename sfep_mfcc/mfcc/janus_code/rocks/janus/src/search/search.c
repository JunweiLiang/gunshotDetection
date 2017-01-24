/* ========================================================================
    JANUS      Speech- to Speech Translation
               ------------------------------------------------------------
               Object: Search Module
               ------------------------------------------------------------

    Author  :  Michael Finke
    Module  :  search.c
    Date    :  $Id: search.c 2864 2009-02-16 21:18:17Z metze $
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
    Revision 3.13  2003/08/14 11:20:24  fuegen
    Merged changes on branch jtk-01-01-15-fms (jaguar -> ibis-013)

    Revision 3.12.20.2  2002/06/26 11:57:58  fuegen
    changes for new directory structure support and code cleaning

    Revision 3.12.20.1  2002/04/25 15:28:27  metze
    *** empty log message ***

    Revision 3.12  2000/11/14 12:35:25  janus
    Merging branch jtk-00-10-27-jmcd.

    Revision 3.11.28.2  2000/11/08 17:31:02  janus
    Finished making changes required for compilation under 'gcc -Wall'.

    Revision 3.11.28.1  2000/11/06 10:50:40  janus
    Made changes to enable compilation under -Wall.

    Revision 3.11  2000/09/12 15:06:28  janus
    Merged branch jtk-00-09-08-hyu.

    Revision 3.10.16.1  2000/09/12 01:18:03  hyu
    Added acoustic lattice rescoring from Michael Finke.

    Revision 3.10  2000/04/25 12:31:25  soltau
    wordListAll replaced wordlist

    Revision 3.9  1999/08/11 15:54:43  soltau
    fast LModel access

    Revision 3.8  1998/08/02 10:56:04  kries
    added count and lmodelBackoff object initialization

 * Revision 3.7  1998/06/15  11:02:43  kries
 * *** empty log message ***
 *
 * Revision 3.6  1998/06/11  17:50:32  kries
 * adaptation of new LM interface
 *
 * Revision 3.4  1998/05/26  18:27:23  kries
 * adaptation to new language model interface
 * -- only the initialization routine changed
 *
 * Revision 3.3  1997/07/18  17:57:51  monika
 * gcc-clean
 *
    Revision 3.2  1997/06/17 17:41:43  westphal
    *** empty log message ***

    Revision 1.19  1996/07/03 13:18:42  monika
    changes to work with 64K vocabulary

    Revision 1.18  96/07/01  14:56:14  14:56:14  monika (Monika Woszczyna)
    - better bookkeeping facilities
    - changes for forced recognition
    
    Revision 1.17  96/04/23  19:50:30  19:50:30  maier (Martin Maier)
    monika: partial hypos and RUNON
    
    Revision 1.16  96/01/18  16:52:07  16:52:07  monika (Monika Woszczyna)
    removed bug in initializing search data structures *hopefully*
    
    Revision 1.15  1996/01/14  14:19:39  monika
    always give lm-pointer when calling lm routines
    removed bugs with multiple search objects

    Revision 1.14  96/01/01  14:22:31  14:22:31  monika (Monika Woszczyna)
    *** empty log message ***
    
    Revision 1.13  1995/09/22  01:56:10  finkem
    made use3gFwd = 1 the default

    Revision 1.12  1995/09/21  21:39:43  finkem
    new -eval flag in the treeFwd pass that combines feature set eval
    and the first search pass.

    Revision 1.11  1995/09/12  19:51:16  finkem
    Rudimentary handling of a complete search transaction

    Revision 1.10  1995/08/29  02:45:29  finkem
    *** empty log message ***

    Revision 1.9  1995/08/22  13:42:08  monika
    modified searchCreate to make the itfTypeCntl work

    Revision 1.8  1995/08/17  17:08:51  rogina
    *** empty log message ***

    Revision 1.1  1995/07/25  13:49:57  finkem
    Initial revision

 
   ======================================================================== */

#include "search.h"
#include "lat.h"
#include "lm_3g.h"
#include "lm.h"
#include "lmodel.h"
#include "lattice.h"
#include "lmodelInt.h"
#include "lmodelMap.h"
#include "lmodelLong.h"
#include "lmodelBackoff.h"
#include "hmm3gram.h"

char searchRCSVersion[]= "@(#)1$Id: search.c 2864 2009-02-16 21:18:17Z metze $";

/* ========================================================================
    Local language model calling
   ======================================================================== */


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

/* ========================================================================
    Search
   ======================================================================== */

static int        searchFreeItf(      ClientData cd);
static int        searchConfigureItf( ClientData cd, char *var, char *val);
static ClientData searchAccessItf(    ClientData cd, char *name, TypeInfo **ti);
static int        searchPutsItf(      ClientData cd, int argc, char *argv[]);

static Search     searchDefault;

/* ------------------------------------------------------------------------
    searchInit
   ------------------------------------------------------------------------ */

int searchInit( Search* SP, char* name, Vocab* vocabP, LMPtr* lmP)
{
  SP->name    =  strdup( name);
  SP->useN    =  0;
  SP->lmP.cd  =  lmP->cd;
  SP->lmP.lmP =  lmP->lmP;
  SP->vocabP  =  vocabP;

  if ( sdictInit(     &(SP->searchCfg.sd), vocabP)            != TCL_OK ||
       distScoreInit( &(SP->searchCfg.ds))                    != TCL_OK ||
       lookAheadInit( &(SP->searchCfg.la), vocabP)            != TCL_OK ||
       rootInit(      &(SP->searchCfg.rt))                    != TCL_OK ||
       bpTableInit(   &(SP->searchCfg.bp))                    != TCL_OK ||
       treeFwdInit(   &(SP->searchCfg.tf), vocabP)            != TCL_OK ||
       flatFwdInit(   &(SP->searchCfg.ff), vocabP)            != TCL_OK ||
       forcedInit(    &(SP->searchCfg.fr), vocabP)            != TCL_OK ||
       latticeInit(   &(SP->searchCfg.lattice), name, vocabP) != TCL_OK ||
       wordListInit(  &(SP->searchCfg.wl), name, vocabP)      != TCL_OK )
       return TCL_ERROR;

  SP->searchCfg.lmodel                = &SP->lmP;
  SP->searchCfg.snsP                  = amodelSetSenoneSet( vocabP->amodelP);
  SP->searchCfg.phoneInsertionPenalty = searchDefault.searchCfg.phoneInsertionPenalty;
  SP->searchCfg.wordPenalty           = searchDefault.searchCfg.wordPenalty;
  SP->searchCfg.silenceWordPenalty    = searchDefault.searchCfg.silenceWordPenalty;
  SP->searchCfg.fillerWordPenalty     = searchDefault.searchCfg.fillerWordPenalty;
  SP->searchCfg.use3gFwd              = searchDefault.searchCfg.use3gFwd;
  SP->searchCfg.verbose               = searchDefault.searchCfg.verbose;
  SP->searchCfg.printWidth            = searchDefault.searchCfg.printWidth;


  /* linkLMPtr(&(SP->lmP)); */
  link( vocabP,             "Vocab");
  link( SP->searchCfg.snsP, "SenoneSet");

  return TCL_OK;
}
/* ------------------------------------------------------------------------
    searchCfgInit / Deinit
   ------------------------------------------------------------------------ */

static void searchCfgInit( Search* sP)
{
  memcpy( &sc, &(sP->searchCfg), sizeof(SConfig));
}

static void searchCfgDeinit( Search* sP)
{
  memcpy( &(sP->searchCfg), &sc, sizeof(SConfig));
  memset( &sc, 0, sizeof(SConfig)); /* so we notice if some jerk usees it ;-)*/
}

/* ------------------------------------------------------------------------
    Create Search Structure
   ------------------------------------------------------------------------ */
Search* searchCreate( char* name, Vocab* vocabP, LMPtr * lmP)
{
  Search* searchP = (Search*)malloc( sizeof(Search));

  if (! searchP || searchInit( searchP, name, vocabP, lmP) != TCL_OK) {
    if ( searchP) free( searchP);
    ERROR( "Failed to allocate Search '%s'.\n", name); 
    return NULL;
  }

  searchCfgInit(  searchP);
  sdictCreate();
  distScoreCreate();
  lookAheadCreate();
  rootCreate();
  flatFwdCreate();
  forcedCreate();
  searchCfgDeinit(searchP);
  return searchP;
}

static ClientData searchCreateItf( ClientData cd, int argc, char *argv[])
{
  int       ac     =  argc - 1;
  Vocab*    vocabP =  NULL;
  LMPtr     lmP;
  ClientData ret;

  lmPtrInit( &lmP);

  assert (cd == cd); /* not used here, keep compiler quiet */

  if ( itfParseArgv( argv[0], &ac, argv+1, 0,
      "<Vocab>", ARGV_OBJECT, NULL, &vocabP, "Vocab","vocabulary",
      "<LM>",    ARGV_CUSTOM, getLMPtr, &lmP, NULL,"language model",
       NULL) != TCL_OK) return NULL;

  ret = (ClientData)searchCreate(argv[0], vocabP, &lmP);

  lmPtrDeinit( &lmP);
  
  return ret;
}


/* ------------------------------------------------------------------------
    Free Search Structure
   ------------------------------------------------------------------------ */
int searchCfgUpdate( Search* sP)
{
  memcpy( &(sP->searchCfg), &sc, sizeof(SConfig));
  return TCL_OK;
}

int searchLinkN( Search* SP)
{
  return SP->useN;
}

int searchDeinit( Search* SP)
{
  if (SP==NULL) { ERROR("null argument"); return TCL_ERROR; }

  if ( searchLinkN( SP)) {
    SERROR("Search '%s' still in use by other objects.\n", SP->name);
    return TCL_ERROR;
  }

  sdictDeinit(     &(SP->searchCfg.sd));
  distScoreDeinit( &(SP->searchCfg.ds));
  rootDeinit(      &(SP->searchCfg.rt));
  bpTableDeinit(   &(SP->searchCfg.bp));
  treeFwdDeinit(   &(SP->searchCfg.tf));
  flatFwdDeinit(   &(SP->searchCfg.ff));

  /* unlinkLMPtr(&(SP->lmP)); */
  unlink( SP->vocabP,         "Vocab");
  unlink( SP->searchCfg.snsP, "SenoneSet");

  return TCL_OK;
}

int searchFree( Search* searchP)
{
  searchCfgInit(  searchP);
  sdictFree();
  rootFree();
  bpTableFree();
  flatFwdFree();
  searchCfgDeinit(searchP);

  searchDeinit( searchP);    /* what about the return value? */

  if ( searchP) free( searchP);

  return TCL_OK;
}

static int searchFreeItf( ClientData cd)
{
  searchFree((Search*)cd);
  return TCL_OK;
}

/* ------------------------------------------------------------------------
    searchConfigureItf
   ------------------------------------------------------------------------ */

static int searchConfigureItf( ClientData cd, char *var, char *val)
{
  Search*   SP = (Search*)cd;
  if (! SP) SP = &searchDefault;

  if (! var) {

    ITFCFG(searchConfigureItf,cd,"phoneInsertionPenalty");
    ITFCFG(searchConfigureItf,cd,"wordPenalty");
    ITFCFG(searchConfigureItf,cd,"silenceWordPenalty");
    ITFCFG(searchConfigureItf,cd,"fillerWordPenalty");
    ITFCFG(searchConfigureItf,cd,"use3gFwd");
    ITFCFG(searchConfigureItf,cd,"verbose");
    ITFCFG(searchConfigureItf,cd,"printWidth");
    return TCL_OK;
  }

  ITFCFG_Float(    var,val,"phoneInsertionPenalty",      
              SP->searchCfg.phoneInsertionPenalty, 0);
  ITFCFG_Float(    var,val,"wordPenalty",      
              SP->searchCfg.wordPenalty, 0);
  ITFCFG_Float(    var,val,"silenceWordPenalty",
              SP->searchCfg.silenceWordPenalty, 0);
  ITFCFG_Float(    var,val,"fillerWordPenalty",
              SP->searchCfg.fillerWordPenalty, 0);
  ITFCFG_Int(      var,val,"use3gFwd",
              SP->searchCfg.use3gFwd, 0);
  ITFCFG_Int(      var,val,"verbose",
              SP->searchCfg.verbose, 0);
  ITFCFG_Int(      var,val,"printWidth",
              SP->searchCfg.printWidth, 0);

  return TCL_ERROR;
}

/* ------------------------------------------------------------------------
    searchAccessItf
   ------------------------------------------------------------------------ */

static ClientData searchAccessItf( ClientData cd, char *name, TypeInfo **ti)
{ 
  Search* searchP  = (Search*)cd;
  if (*name == '.') { 
    if (name[1]=='\0') {
      itfAppendElement( "treeFwd");
      itfAppendElement( "flatFwd");
      itfAppendElement( "lookAhead");
      itfAppendElement( "forced");
      itfAppendElement( "vocab");
      itfAppendElement( "lm");
      itfAppendElement( "lattice");
      itfAppendElement( "wordList");
      itfAppendElement( "senoneSet");
      itfAppendElement( "bestScores");
      itfAppendElement( "lPBestScores");
      itfAppendElement( "rootTellis");
      itfAppendElement( "nodeTellis");
      itfAppendElement( "wordTellis");
      itfAppendElement( "leafTellis");
      itfAppendElement( "scoreCalls");
      itfAppendElement( "senoneMap");
      *ti = NULL; return NULL;
    }
    else {
      if (! strcmp( name+1, "vocab")) {
        *ti = itfGetType("Vocab");
        return (ClientData)(searchP->vocabP);
      }
      if (! strcmp( name+1, "lm")) {
        *ti = searchP->lmP.lmP->tiP;
        return (ClientData)(searchP->lmP.cd);
      }
      if (! strcmp( name+1, "treeFwd")) {
        *ti = itfGetType("TreeFwd");
        return (ClientData)&(searchP->searchCfg.tf);
      }
      else if (! strcmp( name+1, "flatFwd")) {
        *ti = itfGetType("FlatFwd");
        return (ClientData)&(searchP->searchCfg.ff);
      }
      else if (! strcmp( name+1, "lookAhead")) {
        *ti = itfGetType("LookAhead");
        return (ClientData)&(searchP->searchCfg.la);
      }
      else if (! strcmp( name+1, "forced")) {
        *ti = itfGetType("Forced");
        return (ClientData)&(searchP->searchCfg.fr);
      }
      else if (! strcmp( name+1, "lattice")) {
        *ti = itfGetType("Lattice");
        return (ClientData)&(searchP->searchCfg.lattice);
      }
      else if (! strcmp( name+1, "wordList")) {
        *ti = itfGetType("WordList");
        return (ClientData)&(searchP->searchCfg.wl);
      }
      else if (! strcmp( name+1, "senoneSet")) {
        *ti = itfGetType("SenoneSet");
        return (ClientData)(searchP->searchCfg.snsP);
      }
      else if (! strcmp( name+1, "bestScores")) {
        *ti = itfGetType("FMatrix");
        return (ClientData)(searchP->searchCfg.ds.bestScoreFMP);
      }
      else if (! strcmp( name+1, "bestSenone")) {
        *ti = itfGetType("FMatrix");
        return (ClientData)(searchP->searchCfg.ds.bestSenoneFMP);
      }
      else if (! strcmp( name+1, "lPBestScores")) {
        *ti = itfGetType("FMatrix");
        return (ClientData)(searchP->searchCfg.ds.lPBestScoreFMP);
      }
      else if (! strcmp( name+1, "rootTrellis")) {
        *ti = itfGetType("FMatrix");
        return (ClientData)(searchP->searchCfg.ds.rTrellisFMP);
      }
      else if (! strcmp( name+1, "nodeTrellis")) {
        *ti = itfGetType("FMatrix");
        return (ClientData)(searchP->searchCfg.ds.nTrellisFMP);
      }
      else if (! strcmp( name+1, "wordTrellis")) {
        *ti = itfGetType("FMatrix");
        return (ClientData)(searchP->searchCfg.ds.wTrellisFMP);
      }
      else if (! strcmp( name+1, "leafTrellis")) {
        *ti = itfGetType("FMatrix");
        return (ClientData)(searchP->searchCfg.ds.lTrellisFMP);
      }
      else if (! strcmp( name+1, "scoreCalls")) {
        *ti = itfGetType("FMatrix");
        return (ClientData)(searchP->searchCfg.ds.scoreCallFMP);
      }
      else if (! strcmp( name+1, "senoneMap")) {
        *ti = itfGetType("SVector");
        return (ClientData)(searchP->searchCfg.ds.senoneMapSVP);
      }
    }
  }
  return NULL;
}

/* ------------------------------------------------------------------------
    searchPutsItf
   ------------------------------------------------------------------------ */

static int searchPutsItf( ClientData cd, int argc, char *argv[])
{
  Search* SR   = (Search*)cd;
  int     ac   =  argc-1;

  assert (SR == SR); /* not used here, keep compiler quiet */

  if ( itfParseArgv( argv[0], &ac, argv+1, 0,
       NULL) != TCL_OK) return TCL_ERROR;

  return TCL_OK;
}


/* ------------------------------------------------------------------------
    searchTreeFwdItf
   ------------------------------------------------------------------------ */

static int searchTreeFwdItf( ClientData cd, int argc, char *argv[])
{
  Search*   SR     = (Search*)cd;
  int       ac     =  argc-1;
  char*     evalS  =  NULL;
  float     score;

  if ( itfParseArgv( argv[0], &ac, argv+1, 0,
      "-eval",   ARGV_STRING, NULL, &evalS, NULL,"feature set eval string",
       NULL) != TCL_OK) return TCL_ERROR;


  sdictReInit(&SR->searchCfg.sd,SR->vocabP);
  searchCfgInit(SR);
  score = dpsTreeForward((void*) SR, evalS);
  itfAppendResult("%6.2f", score);
  searchCfgDeinit(SR);
  return TCL_OK;
}

/* ------------------------------------------------------------------------
    searchForcedItf
   ------------------------------------------------------------------------ */

static int searchForcedItf( ClientData cd, int argc, char *argv[])
{
  Search*   SR     = (Search*)cd;
  int       ac     =  argc-1;
  char*     evalS  =  NULL;
  float     score;

  if ( itfParseArgv( argv[0], &ac, argv+1, 0,
      "-eval",   ARGV_STRING, NULL, &evalS, NULL,"feature set eval string",
       NULL) != TCL_OK) return TCL_ERROR;

  sdictReInit(&SR->searchCfg.sd,SR->vocabP);
  searchCfgInit(SR);
  score = dpsForced((void*) SR, evalS);
  itfAppendResult("%6.2f", score);
  searchCfgDeinit(SR);
  return TCL_OK;
}

/* ------------------------------------------------------------------------
    searchFullFlatFwdItf
   ------------------------------------------------------------------------ */

static int searchFullFlatFwdItf( ClientData cd, int argc, char *argv[])
{
  Search* SR    =(Search*)cd;
  int     ac    = argc-1;
  char*   evalS = NULL;
  float   score;

  if ( itfParseArgv( argv[0], &ac, argv+1, 0,
      "-eval",   ARGV_STRING, NULL, &evalS, NULL,"feature set eval string",
       NULL) != TCL_OK) return TCL_ERROR;


  sdictReInit(&SR->searchCfg.sd,SR->vocabP);
  searchCfgInit(SR);
  score = dpsFlatForward((void*) SR,1,evalS,NULL);
  itfAppendResult ("%6.2f",score);
  searchCfgDeinit(SR);
  return TCL_OK;
}

/* ------------------------------------------------------------------------
    searchFlatFwdItf
   ------------------------------------------------------------------------ */

static int searchFlatFwdItf( ClientData cd, int argc, char *argv[])
{
  Search* SR    = (Search*)cd;
  int     ac    = argc-1;
  char*   evalS = 0;
  WordList * wlP = NULL;
  float   score;

  if ( itfParseArgv( argv[0], &ac, argv+1, 0,
      "-eval",     ARGV_STRING, NULL, &evalS, NULL,"feature set eval string",
      "-wordList", ARGV_OBJECT, NULL, &wlP, "WordList","wordlist to use",
       NULL) != TCL_OK) return TCL_ERROR;


  sdictReInit(&SR->searchCfg.sd,SR->vocabP);
  searchCfgInit(  SR);
  score = dpsFlatForward((void *) SR,0,evalS,wlP);
  itfAppendResult ("%6.2f",score);
  searchCfgDeinit(SR);
  return TCL_OK;
}


/* ------------------------------------------------------------------------
    searchLatticeItf
   ------------------------------------------------------------------------ */

static int searchLatticeItf( ClientData cd, int argc, char *argv[])
{
  Search* SR     = (Search*)cd;
  float   beam   =  SR->searchCfg.lattice.beamWidth;
  int     frameN =  SR->searchCfg.frameN;
  int     ac     =  argc-1;

  if ( itfParseArgv( argv[0], &ac, argv+1, 0,
      "-beamWidth",   ARGV_FLOAT, NULL, &beam, NULL,"lattice allocation beam",
       NULL) != TCL_OK) return TCL_ERROR;

  if ( frameN < 1) {
    ERROR("No input features available.\n");
    return TCL_ERROR;
  }

  searchCfgInit(  SR);
  latticeAlloc( &(SC.lattice), frameN, 1.0, beam);
  searchCfgDeinit(SR);
  return TCL_OK;
}

/* ------------------------------------------------------------------------
    searchWordlistItf: build wordlist from backtrace structure
                       note that the wl-routines do not use SC, so we don't
                       have to call searchCfgInit.
   ------------------------------------------------------------------------ */

static int searchWordListLATItf( ClientData cd, int argc, char *argv[])
{
  Search* SR     = (Search*) cd;
  int minEFrameN = 0;
  int ac         = argc-1;

  if ( itfParseArgv( argv[0], &ac, argv+1, 0,
   "-minEFrameN",   ARGV_INT, NULL, &minEFrameN, NULL,"number of final frames",
    NULL) != TCL_OK) return TCL_ERROR;

  wordListAllocLAT(&(SR->searchCfg.wl),SR, minEFrameN);
  return TCL_OK;
}

static int searchWordListItf( ClientData cd, int argc, char *argv[])
{
  Search* SR     = (Search*) cd;
  int minEFrameN = SR->searchCfg.wl.minEFrameN;
  int frameN     = SR->searchCfg.frameN;
  int ac         = argc-1;

  if ( itfParseArgv( argv[0], &ac, argv+1, 0,
   "-minEFrameN",   ARGV_INT, NULL, &minEFrameN, NULL,"number of final frames",
    NULL) != TCL_OK) return TCL_ERROR;

  if ( frameN < 1) {
    ERROR("No input features available.\n");
    return TCL_ERROR;
  }

  wordListAlloc(&(SR->searchCfg.wl),SR, minEFrameN);
  return TCL_OK;
}

static int searchDummyWordListItf( ClientData cd, int argc, char *argv[])
{
  Search* SR     = (Search*) cd;
  int minEFrameN = SR->searchCfg.wl.minEFrameN;
  int frameN     = SR->searchCfg.frameN;
  int ac         = argc-1;

  if ( itfParseArgv( argv[0], &ac, argv+1, 0,
   "-minEFrameN",   ARGV_INT, NULL, &minEFrameN, NULL,"number of final frames",
    NULL) != TCL_OK) return TCL_ERROR;

  if ( frameN < 1) {
    ERROR("No input features available.\n");
    return TCL_ERROR;
  }

  wordListAllocDummy(&(SR->searchCfg.wl),SR);
  return TCL_OK;
}

/* ------------------------------------------------------------------------
    Backtrace       recursive procedure to read the backtrace represented
                    in BPTable.
   ------------------------------------------------------------------------ */

static Hypo* dpsBacktrace( BPtr path, WordX wordN)
{
  if ( path.frameX != NILBPTR) {

    short frameX = path.frameX;
    WordX bpX    = path.bpX;
    Hypo* hP     = dpsBacktrace( BP.tablePA[frameX][bpX].path, wordN+1);
    short hwordX  = hP->hwordN++;

    hP->hwordA[hwordX].vocabX    =  BP.tablePA[frameX][bpX].wordX;
    hP->hwordA[hwordX].wordScore = BP.tablePA[frameX][bpX].score - hP->score;
    hP->hwordA[hwordX].wsFrameXF = (hwordX < 1) ? 0 :
                                  hP->hwordA[hwordX-1].weFrameXF+1;
    hP->hwordA[hwordX].weFrameXF = frameX;
    hP->score                   = BP.tablePA[frameX][bpX].score;

    hP->moreInfoA[hwordX].rank              = bpX;
    
    if (frameX < SC.frameN) {
      hP->moreInfoA[hwordX].bestScore         = DS.bestScoreFMP->matPA[frameX][0];
      hP->moreInfoA[hwordX].lastPhoneBestScore= DS.lPBestScoreFMP->matPA[frameX][0];
    } else {
      hP->moreInfoA[hwordX].bestScore         = 0;
      hP->moreInfoA[hwordX].lastPhoneBestScore= 0;
    }
    
    
    /* assuming backpointer table score is sorted */

    hP->moreInfoA[hwordX].bestWEndScore      = BP.tablePA[frameX][0].score;

    if (BP.tableN[frameX] > 1) { /* END_SIL is sorted to be always better */
      if (BP.tablePA[frameX][1].score < BP.tablePA[frameX][0].score)
          hP->moreInfoA[hwordX].bestWEndScore = BP.tablePA[frameX][1].score;
    }
    hP->moreInfoA[hwordX].totalScore = BP.tablePA[frameX][bpX].score;
    hP->moreInfoA[hwordX].maxRank    = BP.tableN[frameX];

    return hP;
  }
  else {
    Hypo* hP   = hypoCreate( wordN);
    hP->hwordN = 0;
    hP->score  = 0.0;
    return hP;
  }
}

/* ------------------------------------------------------------------------
    BacktraceHypothesis                          
   ------------------------------------------------------------------------ */

int dpsBacktraceHypothesis( short frameN, char repairFinal, float z,
                            HypoList* hlP)
{
  short  frameX = frameN - 1;
  BackP* bp     = BP.tablePA[frameX];
  WordX  bpN    = BP.tableN[ frameX];
  WordX  bpX;
  BPtr   path;
  Hypo*  hP     = NULL;

  /* Check, if final word has been reached so far */

  for ( bpX = 0; bpX < bpN; bpX++, bp++) 
    if ( bp->wordX == SD.finishWordX) break;


  hypoListReinit( hlP);  /* reset list of hypothesis   */

  if ( bpX >= bpN) {     /* fall back to best word end */

    float best   = WORSTSCORE;
    BPtr  bestBp;
    short f;
    bestBp.bpX = 0;

    if ( repairFinal) ERROR( "Failed to terminate in final state\n");   

    /* Find the most recent frame with BackP objects */

    for ( f = frameX; f >= 0 && ! BP.tableN[f]; --f);
    if ( f < 0) {
      return 0;
    }

    /* Find the best BackP entry by testing all LM transitions to </s> */

    bp            = BP.tablePA[f];
    bpN           = BP.tableN[ f];
    bestBp.frameX = f;

    for ( bpX = 0; bpX < bpN; bpX++, bp++) {

      float lmScore = local_lm_TgScore(bp->prevWordX, bp->realWordX, SD.finishWordX);

      if ( bp->score + lmScore < best) {
        best       = bp->score + lmScore;
        bestBp.bpX = bpX;
      }
    }
    if ( repairFinal) {

      int from,to,x;

      frameX++;

      INFO("f: %d; frameX: %d; bestBp.bpX: %d\n",f, frameX,bestBp.bpX);
      INFO("uniqN: %d\n",SD.xweModelA[BP.tablePA[f][bestBp.bpX].weDiX].uniqN);
      INFO("segM: %d\n",SD.vocabA[BP.tablePA[f][bestBp.bpX].wordX].segN);

      from = BP.tablePA[f][bestBp.bpX].stackX;
          if (SD.vocabA[BP.tablePA[f][bestBp.bpX].wordX].segN != 1)
      to = from+SD.xweModelA[BP.tablePA[f][bestBp.bpX].weDiX].uniqN;
          else to = from +1;

      INFO ("from: %d; to: %d; stackN %d\n", from, to, BP.stackN[f]);


      for (x = from; x <to; x++) 
       BP.stackPA[f][x] = BP.tablePA[f][bestBp.bpX].score;

      BP.wordTblXA[SD.finishWordX] = NILBPTR;

      dpsBpNextFrame(     frameX);
      dpsSaveBackPointer( frameX, SD.finishWordX, best, bestBp, 0);
      dpsBpSaveFrame(     frameX,0);

      path.frameX = frameX;
      path.bpX    = 0;
    }
    else {
      path.frameX = f;
      path.bpX    = bestBp.bpX;
    }
  }
  else {
    path.frameX = frameX; 
    path.bpX    = bpX;
  }
  bestTotalScore = BP.tablePA[path.frameX][path.bpX].score;

  hypoListAppend(hlP, hP = dpsBacktrace( path, 0));
  return hP->hwordN;
}

/* ------------------------------------------------------------------------
  wordListAllocLAT   create word list based on the lattice
   ------------------------------------------------------------------------ */

int wordListAllocLAT(WordList* wlP, Search* searchP, int minEFrameN)
{
  WordNode *nodeP, *pnodeP, *nnodeP;
  Lattice*  latP   = &(searchP->searchCfg.lattice);
  LatNode*  lnodeP =   NULL;
  LatLink*  linkP  =   NULL;
  int       i, nwd = 0;
  int       frameN;
  int       vocabN = searchP->vocabP->vwordList.itemN;
  char    * tmpUseA;  /* marks used/unused vocab words */

  if ( latticeCheck( latP) != TCL_OK) {
    ERROR("Can't test along lattice since lattice is inconsistent.\n");
    return TCL_ERROR; 
  }

  INFO("allocating wordlist\n");

  /* Initialize word node list array and the list of active vocabulary
     word to zero -- we allocate an extra frame as the previous pass
     might have added a frame if no backtrace was possible */

  wordListFree(wlP); /* free old wordlist contents (if any) */
  wlP->frameN = frameN = latP->frameN;
  
  wlP->wordListPA = (WordNode **) malloc( sizeof(WordNode *) * (frameN+1));
  wlP->wordXA     = (WordX *)     malloc( sizeof(WordX)      * (vocabN+1));
  wlP->isDummy = 0; /* this is a real wordlist */
 
  tmpUseA = (char*) malloc (sizeof(char) * (vocabN+1));
  
  memset( (char*)wlP->wordListPA, 0, (frameN+1) * sizeof(WordNode*));
  memset( (char*)tmpUseA,         0, (vocabN+1) * sizeof(char));


  /* For each start frame we create a list of words, that are entered at
     that frame in BPTable. Each of these words will be annotated by their
     first and last exit frame */

  for ( lnodeP = latP->nodeP; lnodeP; lnodeP = lnodeP->nextP) {
    if (! lnodeP->linkP) {
      short  sFrameX =   lnodeP->frameX;
      short  eFrameX =   frameN-1;

      WordX  w       =   lnodeP->wordX;

      if ( (w < searchP->vocabP->firstWordX-2) /* filler words or silence */
	   || (w == searchP->vocabP->startWordX)) continue;

      for ( nodeP = wlP->wordListPA[sFrameX]; nodeP && nodeP->wordX != w;
            nodeP = nodeP->nextP) ;

      if ( nodeP) {
        if ( nodeP->leFrameX < eFrameX) nodeP->leFrameX = eFrameX;
        if ( nodeP->feFrameX > eFrameX) nodeP->feFrameX = eFrameX;
      }
      else {
        nodeP = (WordNode*)malloc( sizeof(WordNode));

        nodeP->wordX         = w;
        nodeP->feFrameX      = nodeP->leFrameX = eFrameX;
        nodeP->nextP         = wlP->wordListPA[sFrameX];
        wlP->wordListPA[sFrameX] = nodeP;
      }
    }
    for ( linkP = lnodeP->linkP; linkP; linkP = linkP->nextP) {
      short  sFrameX =   lnodeP->frameX;
      short  eFrameX =   linkP->frameX;

      WordX  w       =   lnodeP->wordX;

      if ( (w < searchP->vocabP->firstWordX-2) /* filler words or silence */
	   || (w == searchP->vocabP->startWordX)) continue;

      for ( nodeP = wlP->wordListPA[sFrameX]; nodeP && nodeP->wordX != w;
            nodeP = nodeP->nextP) ;

      if ( nodeP) {
        if ( nodeP->leFrameX < eFrameX) nodeP->leFrameX = eFrameX;
        if ( nodeP->feFrameX > eFrameX) nodeP->feFrameX = eFrameX;
      }
      else {
        nodeP = (WordNode*)malloc( sizeof(WordNode));

        nodeP->wordX         = w;
        nodeP->feFrameX      = nodeP->leFrameX = eFrameX;
        nodeP->nextP         = wlP->wordListPA[sFrameX];
        wlP->wordListPA[sFrameX] = nodeP;
      }
    }
  }

  /* Eliminate words that have only very few endpoints in the backtrace.
     Also remove finish words that do not end the last frame. Finish
     words ending in the last frame are not tested for the minimum number
     of end frames as all other words are. */

  for ( i = 0; i < frameN; i++) {
    pnodeP = NULL;

    for ( nodeP = wlP->wordListPA[i]; nodeP; nodeP = nnodeP) {
      nnodeP = nodeP->nextP;

      if ((nodeP->wordX != searchP->vocabP->finishWordX &&
           nodeP->leFrameX - nodeP->feFrameX < minEFrameN) ||
         ( nodeP->wordX == searchP->vocabP->finishWordX && nodeP->leFrameX < frameN-1)) {

        if ( ! pnodeP) wlP->wordListPA[i] = nnodeP;
        else           pnodeP->nextP  = nnodeP;

        free( (char*)nodeP);
      }
      else pnodeP = nodeP;
    }
  }

  /* Generate a list of all remaining words ... */

  for ( i = 0; i < frameN; i++) {
    for ( nodeP = wlP->wordListPA[i]; nodeP; nodeP = nodeP->nextP) {
      if (! tmpUseA[nodeP->wordX]) {

        tmpUseA[nodeP->wordX] = 1;
        wlP->wordXA[ nwd++]        = nodeP->wordX;
      }
    }
  }
  wlP->wordXA[nwd] = NILWORD;

  free( tmpUseA);

  INFO("Lattice WordList %d\n", nwd);
  return TCL_OK;
}

/* ------------------------------------------------------------------------
  wordListAlloc build a list of words that are somewhat "likely" to
                have started for each frame of the utterance, using the
                backtrace structure of a search. Neither the start word
                nor silence are included.
   ------------------------------------------------------------------------ */

int wordListAlloc(WordList* wlP, Search* searchP, int minEFrameN)
{
  WordNode *nodeP, *pnodeP, *nnodeP;
  short     f;
  int       i, nwd = 0;
  int       frameN = searchP->searchCfg.frameN;
  int       vocabN = searchP->vocabP->vwordList.itemN;
  char    * tmpUseA;  /* marks used/unused vocab words */
 
  /* wordList* and the search must used the same vocabulary object */

  INFO("allocating wordlist\n");

  /* Initialize word node list array and the list of active vocabulary
     word to zero -- we allocate an extra frame as the previous pass
     might have added a frame if no backtrace was possible */

  wordListFree(wlP); /* free old wordlist contents (if any) */
  wlP->frameN = frameN;

  wlP->wordListPA = (WordNode **) malloc( sizeof(WordNode *) * (frameN +1));
  wlP->wordXA     = (WordX *) malloc (sizeof (WordX) * vocabN+1);
  wlP->isDummy = 0; /* this is a real wordlist */

  tmpUseA = (char*) malloc (sizeof(char) * vocabN+1);

  memset( (char*)wlP->wordListPA, 0, (frameN+1) * sizeof(WordNode*));
  memset( (char*)tmpUseA,  0, (vocabN+1)  * sizeof(char));

  /* For each start frame we create a list of words, that are entered at
     that frame in BPTable. Each of these words will be annotated by their
     first and last exit frame */

  for ( f = 0; f < frameN; f++) {
    BackP*    bp  = searchP->searchCfg.bp.tablePA[f];
    WordX     bpN = searchP->searchCfg.bp.tableN[ f];

    for ( i = 0; i < bpN; i++, bp++) {
      short  sFrameX = ( bp->path.frameX < 0) ? 0 : bp->path.frameX + 1;
      short  eFrameX =   f;
      WordX  w       =   bp->wordX;

      if ( (w < searchP->vocabP->firstWordX-2) /* filler words or silence */
          || (w == searchP->vocabP->startWordX)) continue;

      for ( nodeP = wlP->wordListPA[sFrameX]; nodeP && nodeP->wordX != w;
            nodeP = nodeP->nextP) ;

      if ( nodeP) nodeP->leFrameX = eFrameX;
      else {
        nodeP = (WordNode*)malloc( sizeof(WordNode));

        nodeP->wordX         = w;
        nodeP->feFrameX      = nodeP->leFrameX = eFrameX;
        nodeP->nextP         = wlP->wordListPA[sFrameX];
        wlP->wordListPA[sFrameX] = nodeP;
      }
    }
  }

  /* Eliminate words that have only very few endpoints in the backtrace.
     Also remove finish words that do not end the last frame. Finish
     words ending in the last frame are not tested for the minimum number
     of end frames as all other words are. */

  for ( i = 0; i < frameN; i++) {
    pnodeP = NULL;

    for ( nodeP = wlP->wordListPA[i]; nodeP; nodeP = nnodeP) {
      nnodeP = nodeP->nextP;

      if ((nodeP->wordX != searchP->vocabP->finishWordX &&
           nodeP->leFrameX - nodeP->feFrameX < minEFrameN) ||
         ( nodeP->wordX == searchP->vocabP->finishWordX && 
                                   nodeP->leFrameX < frameN-1)) {

        if ( ! pnodeP) wlP->wordListPA[i] = nnodeP;
        else           pnodeP->nextP  = nnodeP;

        free( (char*)nodeP);
      }
      else pnodeP = nodeP;
    }
  }

  /* Generate a list of all words in all frames that were not removed ... */

  for ( i = 0; i < frameN; i++) {
    for ( nodeP = wlP->wordListPA[i]; nodeP; nodeP = nodeP->nextP) {
      if (! tmpUseA[nodeP->wordX]) {
        tmpUseA[nodeP->wordX] = 1;
        wlP->wordXA[ nwd++]        = nodeP->wordX;
      }
    }
  }
  wlP->wordXA[nwd] = NILWORD;

  free(tmpUseA);

  INFO("WordList %d\n", nwd);
  return TCL_OK;
}

/* --------------------------------------------------------------------------
  wordListAllocDummy: build a wordlist that allows all words to start
                      at all points, except for words not known to the lm,
                      silence, fillers and the start word;
                      as the list of words is the same for all frames,
                      we only build one list and have all pointers point to it.
  --------------------------------------------------------------------------*/
int wordListAllocDummy (WordList* wlP, Search* searchP) {
  int f =0;
  int nwd = 0;

  int   frameN = searchP->searchCfg.frameN;
  int   vocabN = searchP->vocabP->vwordList.itemN;
  char * tmpUseA;

  wordListFree(wlP); /* free old wordlist contents (if any) */

  wlP->frameN = frameN;

  wlP->wordListPA = (WordNode **) malloc( sizeof(WordNode *) * (frameN +1));
  wlP->wordXA     = (WordX *)     malloc (sizeof(WordX) * vocabN+1);

  wlP->isDummy = 1; /* all pointers point to the same wordlist */

  tmpUseA = (char*) malloc (sizeof(char) * vocabN+1);

  memset( (char*)wlP->wordListPA, 0, (frameN+1) * sizeof(WordNode*));
  memset( (char*)tmpUseA,  0, searchP->vocabP->vwordList.itemN  * sizeof(char));
  
  {
    WordX    w = searchP->vocabP->firstWordX;
    VWord *  vWordA = searchP->vocabP->vwordList.itemA;
    WordNode* nodeP, *lastP = NULL;
    for (w = 0; w < searchP->vocabP->vwordList.itemN; w ++) {
      if (w != searchP->vocabP->finishWordX) {
                                                  /* omit: */
        if ( (w < searchP->vocabP->firstWordX-2)  /* filler words or silence */
           || (w == searchP->vocabP->startWordX)) /* startWord */
                              continue;
                                                  /* words unknown to lm */
        if(! lm_VocabwdInCurrent(vWordA[vWordA[w].realX].spellS)) 
                                                               continue;
      }

      nodeP = (WordNode*) malloc( sizeof(WordNode));
      if (!lastP) {wlP->wordListPA[0] = nodeP;} /* first in list */ 
      else  {lastP->nextP = nodeP;} 
      nodeP->wordX = w;
      nodeP->feFrameX = 0;
      nodeP->leFrameX = frameN;
      nodeP->nextP = NULL;   
      lastP = nodeP;
      if (! tmpUseA[nodeP->wordX]) {
        tmpUseA[nodeP->wordX] = 1;
        wlP->wordXA[ nwd++]  = w;
      }
    }
  } 

  /* all pointers point to the same wordlist : */

  for (f=1;f<frameN+1;f++) wlP->wordListPA[f] = wlP->wordListPA[0];

  wlP->wordXA[nwd] = NILWORD;

  free(tmpUseA);

  INFO("WordList %d\n", nwd);
  return (TCL_OK);
}

/* ------------------------------------------------------------------------
    treeFwdTreeInfoItf
    as indicated by the name, I would prefer this to be a method
    of the subobject treeFwd. Currently, the TreeFwd structure does not
    contain the necessary information to do this.
   ------------------------------------------------------------------------ */

static int treeFwdTreeInfoItf( ClientData cd, int argc, char *argv[])
{
  Search* SP   = (Search*)cd;
  char * name;
  int   vidx;

  int   ac   =  argc-1;
  Vocab* vocabP = SP->vocabP;
  
  if ( itfParseArgv( argv[0], &ac, argv+1, 1,
      "<name>", ARGV_STRING,NULL,&name, NULL, "spelling",
       NULL) != TCL_OK) return TCL_ERROR;
  
  vidx = listIndex((List*)&(vocabP->vwordList), name, 0);

  if (vidx >= 0) {
    SConfig* sCfgP  = &(SP->searchCfg);
    SDict*   sDictP = &(sCfgP->sd);
    VWord*   vWordP = &(sDictP->vocabA[vidx]);
    int len        = vWordP->segN;
    
    if (len == 1) {
     return TCL_OK;
    } else {
      int xwbModelX   = vWordP->xwbModelX; /* identifies word begin Model */
      int dPhoneX     = sCfgP->rt.dPhoneXA[xwbModelX]; /* index of root node */
      int segX;
      Trellis* childP = sCfgP->rt.trellisA[dPhoneX].childP;
      
      itfAppendResult("%f",sCfgP->rt.trellisA[dPhoneX].lp);
    
      for (segX = 1; segX < len-1; segX ++) {
        assert(childP);
        while (childP->modelX != vWordP->modelXA[segX]) {
          assert(childP);
          childP = childP->nextP;
        }
        itfAppendResult(" %f",childP->lp );
        childP = childP->childP;
      }
      return TCL_OK;
    }
    
  } else {
    itfAppendResult("testoutput: %s is not in vocab",name);
    return TCL_ERROR;
  }
}
  

/* ========================================================================
    Type Declaration
   ======================================================================== */

static Method searchMethod[] = 
{ 
  { "puts",       searchPutsItf,    NULL },
  { "treeInfo",   treeFwdTreeInfoItf,  "get lm-lookahead info for a word" },
  { "treeFwd",    searchTreeFwdItf,    "run tree forward pass of search" },
  { "flatFwd",    searchFlatFwdItf,    "run flat forward on likely words" },
  { "fullFlatFwd",searchFullFlatFwdItf,"run flat forward with all words" },
  { "forced",     searchForcedItf,     "run forced recognition" },
  { "lattice",    searchLatticeItf,    "build lattice from last search pass" },
  { "wordList",   searchWordListItf, "build wordList from last search pass" },
  { "wordListLAT", searchWordListLATItf, "build wordList from lattice" },
  { "wordListAll", searchDummyWordListItf, "build wordList with all entries" },
  {  NULL,        NULL,             NULL } 
};

static TypeInfo searchInfo = { "Search", 0, -1, searchMethod, 
                                searchCreateItf, searchFreeItf,
                                searchConfigureItf, searchAccessItf, NULL,
	                       "Search Module" } ;

static int searchInitialized = 0;

int Search_Init( )
{
  if (! searchInitialized) {

    if ( TreeFwd_Init()   != TCL_OK) return TCL_ERROR;
    if ( LookAhead_Init() != TCL_OK) return TCL_ERROR;
    if ( FlatFwd_Init()   != TCL_OK) return TCL_ERROR;
    if ( Lattice_Init()   != TCL_OK) return TCL_ERROR;
    if ( Lat_Init()       != TCL_OK) return TCL_ERROR;
    if ( WordList_Init()  != TCL_OK) return TCL_ERROR;
    if ( Lm_Init()        != TCL_OK) return TCL_ERROR;
    if ( LM_Init()        != TCL_OK) return TCL_ERROR;
    if ( LModel_Init()    != TCL_OK) return TCL_ERROR;
    if ( LModelInt_Init() != TCL_OK) return TCL_ERROR;
    if ( LModelMap_Init() != TCL_OK) return TCL_ERROR;
    if ( LModelLong_Init()!= TCL_OK) return TCL_ERROR;
    if ( LModelBackoff_Init() != TCL_OK) return TCL_ERROR;
    if ( hmm3gram_Init()  != TCL_OK) return TCL_ERROR;
    if ( Hypos_Init()     != TCL_OK) return TCL_ERROR;
    if ( Vocab_Init()     != TCL_OK) return TCL_ERROR;
    if ( Forced_Init()    != TCL_OK) return TCL_ERROR;

    searchDefault.searchCfg.lmodel                = NULL;
    searchDefault.searchCfg.snsP                  = NULL;
    searchDefault.searchCfg.phoneInsertionPenalty = 0.0;
    searchDefault.searchCfg.wordPenalty           = 0.0;
    searchDefault.searchCfg.silenceWordPenalty    = 0.0;
    searchDefault.searchCfg.fillerWordPenalty     = 0.0;
    searchDefault.searchCfg.use3gFwd              =   1;

    searchDefault.searchCfg.verbose               =   1;
    searchDefault.searchCfg.printWidth            =  70;

    itfNewType( &searchInfo);
    searchInitialized = 1;
  }
  return TCL_OK;
}
