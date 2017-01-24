/* ========================================================================
    JANUS      Speech- to Speech Translation
               ------------------------------------------------------------
               Object: Dynamic Programming Tree Forward
               ------------------------------------------------------------

    Author  :  Michael Finke
    Module  :  treefwd.c
    Date    :  $Id: treefwd.c 3418 2011-03-23 15:07:34Z metze $
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
    Revision 3.10  2003/08/14 11:20:24  fuegen
    Merged changes on branch jtk-01-01-15-fms (jaguar -> ibis-013)

    Revision 3.9.24.6  2002/11/20 13:20:44  soltau
    Made it compile-able

    Revision 3.9.24.5  2002/06/26 11:57:58  fuegen
    changes for new directory structure support and code cleaning

    Revision 3.9.24.4  2001/09/19 19:42:58  soltau
    Change score function dynamically

    Revision 3.9.24.3  2001/07/06 13:12:02  soltau
    Changed compress and scoreFct handling

    Revision 3.9.24.2  2001/07/03 09:40:08  soltau
    Changes according to the new acoustic score function interface

    Revision 3.9.24.1  2001/06/26 19:29:35  soltau
    beautified/fixed a couple of ugly hacker routines

    Revision 3.9  2000/10/27 16:11:27  janus
    Merged branch jtk-00-10-24-jmd.

    Revision 3.7.1.3.28.1  2000/10/27 15:05:04  janus
    Made SIMD a run-time option.

    Revision 3.7.1.3  2000/04/28 16:33:47  soltau
    runon changes

    Revision 3.7.1.2  2000/03/05 15:06:05  soltau
    made it working with new lookahead interface

    Revision 3.7.1.1  1999/11/17 10:52:45  soltau
    Applied old (!) bugfix to interrupt treefwd properly

    Revision 3.7  1998/07/13 16:52:57  tschaaf
    Fix problem that occure if the search is canceled
    during publish-Function. We had problems with using
    the repair-flag in dpsBacktraceHypothesis.

    Revision 3.6  1998/06/11 17:50:32  kries
    adaptation of new LM interface

 * Revision 3.5  1998/06/10  15:33:09  tschaaf
 * the treeFwd can now be interupted during search.
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
    Revision 3.4  1997/08/27  10:16:54  monika
    added a few changes required for the combination of runon
    and lookaheads.
    bugfix to avoid crashes in second sentence in runon mode.
  
    Revision 3.3  1997/07/18 17:57:51  monika
    gcc-clean

    Revision 3.2  1997/06/17 17:41:43  westphal
    *** empty log message ***

    Revision 1.3  1997/02/19 14:28:10  monika
    save a lot of calls for scores at the beginning odf tree leafs and single
    phone words

 * Revision 1.1  1997/01/22  08:18:52  monika
 * Initial revision
 *
    Revision 1.19  1996/11/27 15:03:50  monika
    added pruning method for treefwd object (trellisTopN)

    Revision 1.18  1996/07/11 10:57:37  monika
    removed bug that crashed janus in runonmode if it could not
    terminate in final state . iths bug does not appear in early
    code versions that still use static arrays for bestScoreA.

    Revision 1.17  96/07/03  13:18:42  13:18:42  monika (Monika Woszczyna)
    changes to work with 64K vocabulary
    
    Revision 1.16  96/07/01  14:52:42  14:52:42  monika (Monika Woszczyna)
    changes for better bookkeeping (in acessible matrix)
    changes to avoid expansion of inactive last phonemes
    changes to always evaluate first state in active root (clean pruning)
    
    Revision 1.15  96/04/23  19:51:52  19:51:52  maier (Martin Maier)
    monika: partial hypos and RUNON
    
    
    Revision 1.14  96/03/29  13:47:19  13:47:19  monika (Monika Woszczyna)
    removed bug with:
     - lastPhoneAloneBeamWidth no longer stuck at 80
     - trellises deactivated AFTER all trellis-to-trellis expansions
     - trellises reset whenever deactivated.
   
    Revision 1.13  96/02/27  00:03:44  00:03:44  finkem (Michael Finke)
    Duration modelling modified
    
    Revision 1.12  1996/02/17  19:25:21  finkem
    removed old hypoList stuff and added phone duration models

    Revision 1.11  1996/01/18  16:51:18  monika
    removed bug in initializing search data structures (hopefully)

    Revision 1.9  1996/01/01  14:22:19  monika
    *** empty log message ***

    Revision 1.8  1995/09/12  19:52:10  finkem
    Handling of hypo publishing changed

    Revision 1.7  1995/08/29  02:45:42  finkem
    *** empty log message ***

    Revision 1.6  1995/08/17  17:05:10  rogina
    *** empty log message ***

    Revision 1.5  1995/08/17  13:25:05  finkem
    *** empty log message ***

    Revision 1.4  1995/08/06  21:03:42  finkem
    *** empty log message ***

    Revision 1.3  1995/08/06  19:26:27  finkem
    *** empty log message ***

    Revision 1.2  1995/08/03  14:41:17  finkem
    *** empty log message ***

    Revision 1.1  1995/07/25  13:49:57  finkem
    Initial revision

 
   ======================================================================== */

#define   SEARCH_MODULE

#include <assert.h>

#include "search.h"
#include "treefwd.h"
#include "lookahead.h"
#include "scoreA.h"

static char* RCS = 
          "@(#)1$Id: treefwd.c 3418 2011-03-23 15:07:34Z metze $";


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
    Global Search Config
   ======================================================================== */

SConfig sc;


/* ========================================================================
    Search Dictionary
   ======================================================================== */
/* ------------------------------------------------------------------------
    SDict:   Search dictionary local variables
   ------------------------------------------------------------------------ */

int sdictInit( SDict* sdP, Vocab* vocabP)
{
  assert(vocabP);

  sdP->vocabN       = vocabP->vwordList.itemN;
  sdP->vocabA       = vocabP->vwordList.itemA;

  assert(vocabP->amodelP);
  assert(vocabP->xwbModelP);
  assert(vocabP->xweModelP);

  sdP->modelN       = vocabP->amodelP->list.itemN;
  sdP->modelA       = vocabP->amodelP->list.itemA;

  sdP->durP         =(vocabP->amodelP->durP) ? (DurationSet*)
                      vocabP->amodelP->durP->mdsP.cd : NULL;

  sdP->xwbModelA    = vocabP->xwbModelP->list.itemA;
  sdP->xweModelA    = vocabP->xweModelP->list.itemA;

  sdP->phoneN       = vocabP->phonesN;

  sdP->xwTags       = vocabP->dictP->xwTags;

  sdP->firstX       = vocabP->firstWordX;
  sdP->silenceWordX = vocabP->silenceWordX;
  sdP->startWordX   = vocabP->startWordX;
  sdP->finishWordX  = vocabP->finishWordX;

  sdP->wBoundaryX   = sdP->vocabA[vocabP->silenceWordX].segXA[0];

  sdP->trellisPA    = NULL;       /* list of word trellis objects      */

  sdP->activeA      = NULL;       /* active words                      */
  sdP->activeXA[0]  = NULL;       /* active word trellis objects       */
  sdP->activeXA[1]  = NULL;
  sdP->activeN[ 0]  = 0;          /* number of act.word trellis obj.   */
  sdP->activeN[ 1]  = 0;
  sdP->sPhoneN      = 0;          /* number of single phone words      */
  sdP->sPhoneLmN    = 0;          /* number of LM single phone words   */
  sdP->sPhoneXA     = NULL;       /* single phone words                */

  return TCL_OK;
}

int sdictReInit( SDict* sdP, Vocab* vocabP)
{
  assert(vocabP);

  sdP->vocabN       = vocabP->vwordList.itemN;
  sdP->vocabA       = vocabP->vwordList.itemA;

  assert(vocabP->amodelP);
  assert(vocabP->xwbModelP);
  assert(vocabP->xweModelP);

  sdP->modelN       = vocabP->amodelP->list.itemN;
  sdP->modelA       = vocabP->amodelP->list.itemA;

  sdP->xwbModelA    = vocabP->xwbModelP->list.itemA;
  sdP->xweModelA    = vocabP->xweModelP->list.itemA;

  sdP->phoneN       = vocabP->phonesN;

  sdP->firstX       = vocabP->firstWordX;
  sdP->silenceWordX = vocabP->silenceWordX;
  sdP->startWordX   = vocabP->startWordX;
  sdP->finishWordX  = vocabP->finishWordX;

  sdP->wBoundaryX   = sdP->vocabA[vocabP->silenceWordX].segXA[0];

  return TCL_OK;
}

int sdictDeinit( SDict * sdP)
{
  assert (sdP == sdP); /* keep compiler quiet */
  return TCL_OK;
}

/* ------------------------------------------------------------------------
    sdictCreate
   ------------------------------------------------------------------------ */

int sdictCreate()
{
  int i;

  SD.trellisPA   = (Trellis**)malloc(SD.vocabN * sizeof(Trellis*));
  SD.lmXA        = (int  *)malloc(   SD.vocabN * sizeof(int));

  for ( i = 0; i < SD.vocabN; i++) {
    int idx = -1;

    SD.lmXA[i] = -1;
    if ( ISA_FILLER( i)) continue;

    if (    i == SD.startWordX) {
      idx = SC.lmodel->lmP->indexFct( SC.lmodel->cd, "<s>");
    }
    else if ( i == SD.finishWordX) {
      idx = SC.lmodel->lmP->indexFct( SC.lmodel->cd, "</s>");
    }
    else  {
      idx = SC.lmodel->lmP->indexFct( SC.lmodel->cd, 
            SD.vocabA[SD.vocabA[i].realX].spellS);
    }
    if ( idx < 0) {
      if ((idx = SC.lmodel->lmP->indexFct( SC.lmodel->cd, "<UNK>")) < 0) {
        ERROR("Couldn't map '%s' to LM word.", 
               SD.vocabA[SD.vocabA[i].realX].spellS);
      } else {
        INFO("Mapped '%s' to LM word <UNK>.\n", 
              SD.vocabA[SD.vocabA[i].realX].spellS);
      }
    }
    SD.lmXA[i] = idx;
  }


  SD.activeA     = (char *)malloc(   SD.vocabN * sizeof(char));
  SD.activeXA[0] = (WordX*)malloc(2* SD.vocabN * sizeof(WordX));
  SD.activeXA[1] =  SD.activeXA[0]  + SD.vocabN;
  SD.activeN[ 0] =  SD.activeN[ 1]  = 0;

  memset((char*)SD.trellisPA, 0, SD.vocabN * sizeof(Trellis*)); 
  memset((char*)SD.activeA,   0, SD.vocabN * sizeof(char)); 

  return 0;
}

/* ------------------------------------------------------------------------
    sdictFree
   ------------------------------------------------------------------------ */

void sdictFree()
{
  if ( SD.trellisPA)   { free((char*)SD.trellisPA); SD.trellisPA = NULL; }
  if ( SD.activeA)     { free((char*)SD.activeA);   SD.activeA   = NULL; }
  if ( SD.activeXA[0]) { free((char*)SD.activeXA[0]);
                         SD.activeXA[0] = NULL; SD.activeXA[1] = NULL; }
}

/* ------------------------------------------------------------------------
    Local Variables
   ------------------------------------------------------------------------ */

float bestScore          = WORSTSCORE;
float bestPhoneScore     = WORSTSCORE;
float bestTotalScore     = WORSTSCORE;
float lastPhoneBestScore = WORSTSCORE;

int   compDistN          = 0;
int   compDistPerFrameN  = 0;

int   rtrellisEvalN      = 0;
int   ntrellisEvalN      = 0;
int   ltrellisEvalN      = 0;
int   wtrellisEvalN      = 0;

/* ========================================================================
    Tree Forward Locals
   ======================================================================== */

int   zeroA[MAX_MONOPHONES];    /* vector of MAX_MONOPHONES zeros
                                   that is used as replacement, when
                                   no diphone context is defined but
                                   we need an offset array to access
                                   BP score stack */

#define CANDFRAME_ALLOCSIZE 64

typedef struct {

  short eFrameX;    /* end frame of the word before this candidate */
  int   candX;      /* index of first candidate in a list of candidates
                       all starting at the same frame */

} CandFrame;

static int        cframeAllocN = 0;
static CandFrame* cframeA      = NULL;

/* ------------------------------------------------------------------------
    treeFwdDefaults
   ------------------------------------------------------------------------ */

static TreeFwd   treeFwdDefaults;

/* ------------------------------------------------------------------------
    treeFwdInit
   ------------------------------------------------------------------------ */

int treeFwdInit( TreeFwd* tfP, Vocab* vocabP)
{
  tfP->beamWidth               =  treeFwdDefaults.beamWidth;
  tfP->phoneBeamWidth          =  treeFwdDefaults.phoneBeamWidth;
  tfP->wordBeamWidth           =  treeFwdDefaults.wordBeamWidth;
  tfP->lastPhoneBeamWidth      =  treeFwdDefaults.lastPhoneBeamWidth;
  tfP->lastPhoneAloneBeamWidth =  treeFwdDefaults.lastPhoneAloneBeamWidth;
  tfP->pruneMethod             =  treeFwdDefaults.pruneMethod;

  tfP->topN                    =  treeFwdDefaults.topN;
  tfP->trellisTopN             =  treeFwdDefaults.trellisTopN;
  tfP->skip                    =  treeFwdDefaults.skip;
  tfP->maxFramesRunon          =  treeFwdDefaults.maxFramesRunon;
  tfP->publishFrameN           =  treeFwdDefaults.publishFrameN;

  hypoListInit( &(tfP->hypoList), "hypoList", vocabP, 1);

  return TCL_OK;
}

/* ------------------------------------------------------------------------
    treeFwdDeinit
   ------------------------------------------------------------------------ */

int treeFwdDeinit( TreeFwd* tfP)
{
  hypoListDeinit( &(tfP->hypoList));
  return TCL_OK;
}

/* ------------------------------------------------------------------------
    treeFwdConfigureItf
   ------------------------------------------------------------------------ */

static int treeFwdConfigureItf( ClientData cd, char *var, char *val)
{
  TreeFwd* tfP = (TreeFwd*)cd;
  if (! tfP) tfP = &treeFwdDefaults;

  if (! var) {

    ITFCFG(treeFwdConfigureItf,cd,"beamWidth");
    ITFCFG(treeFwdConfigureItf,cd,"phoneBeamWidth");
    ITFCFG(treeFwdConfigureItf,cd,"wordBeamWidth");
    ITFCFG(treeFwdConfigureItf,cd,"lastPhoneBeamWidth");
    ITFCFG(treeFwdConfigureItf,cd,"lastPhoneAloneBeamWidth");
    ITFCFG(treeFwdConfigureItf,cd,"pruneMethod");
    ITFCFG(treeFwdConfigureItf,cd,"topN");
    ITFCFG(treeFwdConfigureItf,cd,"trellisTopN");
    ITFCFG(treeFwdConfigureItf,cd,"skip");
    ITFCFG(treeFwdConfigureItf,cd,"maxFramesRunon");
    ITFCFG(treeFwdConfigureItf,cd,"publishFrameN");
    return TCL_OK;
  }

  ITFCFG_Float(    var,val,   "beamWidth",      
                          tfP->beamWidth, 0);
  ITFCFG_Float(    var,val,   "phoneBeamWidth",      
                          tfP->phoneBeamWidth, 0);
  ITFCFG_Float(    var,val,   "wordBeamWidth",      
                          tfP->wordBeamWidth, 0);
  ITFCFG_Float(    var,val,   "lastPhoneBeamWidth",      
                          tfP->lastPhoneBeamWidth, 0);
  ITFCFG_Float(    var,val,   "lastPhoneAloneBeamWidth",      
                          tfP->lastPhoneAloneBeamWidth, 0);
  ITFCFG_Int(      var,val,   "pruneMethod",      
                          tfP->pruneMethod, 0);
  ITFCFG_Int(      var,val,   "topN",      
                          tfP->topN, 0);
  ITFCFG_Int(      var,val,   "trellisTopN",      
                          tfP->trellisTopN, 0);
  ITFCFG_Int(      var,val,   "skip",      
                          tfP->skip, 0);
  ITFCFG_Int(      var,val,   "maxFramesRunon",      
                          tfP->maxFramesRunon, 0);
  ITFCFG_Int(      var,val,   "publishFrameN",      
                          tfP->publishFrameN, 0);
  return TCL_ERROR;
}

/* ------------------------------------------------------------------------
    treeFwdAccessItf
   ------------------------------------------------------------------------ */

static ClientData treeFwdAccessItf (ClientData cd, char *name, TypeInfo **ti)
{ 
  TreeFwd *tfwdP = (TreeFwd*)cd;

  if (*name == '.') { 
    if (name[1]=='\0') {
      itfAppendElement( "hypoList");
      *ti = NULL; return NULL;
    }
    else { 
      if (! strcmp( name+1, "hypoList")) {
        *ti = itfGetType("HypoList");
        return (ClientData)&(tfwdP->hypoList);
      }
    }
  }
  return NULL;
}

/* ------------------------------------------------------------------------
    treeFwdPutsItf
   ------------------------------------------------------------------------ */

static int treeFwdPutsItf( ClientData cd, int argc, char *argv[])
{
  TreeFwd* tfP   = (TreeFwd*)cd;
 int       ac     =  argc - 1;

 if ( itfParseArgv( argv[0], &ac, argv+1, 0,
       NULL) != TCL_OK) return TCL_OK;

 WARN("TreeFwd RCS version: %s (%X)\n",RCS,tfP);

  return TCL_OK;
}

/* ========================================================================
    Trellis & Tree Structure
   ======================================================================== */

static int      dpsInitSearchTree( void );
static void     dpsCreateSearchTree( char useLM);
static void     dpsDeleteSearchTree( void );
static void     dpsDeleteSearchSubtree( Trellis* trP);

int rootInit( Root* rtP)
{
  rtP->allocN      = 0;                /* number of allocated roots         */
  rtP->trellisN    = 0;                /* number of used RTRELLIS objects   */
  rtP->trellisA    = NULL;             /* array of root trellis objects     */
  rtP->nodeN       = 0;                /* number of used trellis nodes      */
  rtP->maxNodeN    = 0;                /* max.number of nodes               */
  rtP->activePA[0] = NULL;             /* arrays of active TRELLIS objects  */
  rtP->activePA[1] = NULL;
  rtP->activeN[0]  = 0;                /* number of active objects          */
  rtP->activeN[1]  = 0;
  rtP->mpx         = 1;                /* multiplex root nodes              */
  rtP->hPhoneXA    = NULL;             /* list of homophone words           */
  rtP->dPhoneXA    = NULL;             /* diphone to root trellis map       */

  rtP->lPhoneCandN = 0;                /* last phone candidates in list     */
  rtP->lPhoneCandA = NULL;             /* list of last phone candidates     */
  rtP->lWordTransA = NULL;             /* cache for storing last word trans */

  return TCL_OK;
}

int rootDeinit( Root* rtP)
{
  assert (rtP == rtP); /* keep compiler quiet */
  return TCL_OK;
}

/* ------------------------------------------------------------------------
    rootCreate
   ------------------------------------------------------------------------ */

int rootCreate()
{
  WordX w;

  RT.hPhoneXA    = (WordX*)malloc(      SD.vocabN * sizeof(WordX));
  RT.lPhoneCandA = (LPhoneCand*)malloc( SD.vocabN * sizeof(LPhoneCand));
  RT.lWordTransA = (LWordTrans*)malloc( SD.vocabN * sizeof(LWordTrans));

  for ( w = 0; w < SD.vocabN; w++) {
    RT.hPhoneXA[w]            = NILWORD;
    RT.lWordTransA[w].sFrameX = -1;
  }
  dpsInitSearchTree();
  return 0;
}

/* ------------------------------------------------------------------------
    rootFree
   ------------------------------------------------------------------------ */

void rootFree()
{
  dpsDeleteSearchTree();

  if ( RT.lPhoneCandA) { free((char*)RT.lPhoneCandA);
                         RT.lPhoneCandA = NULL; }
  if ( RT.lWordTransA) { free((char*)RT.lWordTransA);
                         RT.lWordTransA = NULL; }
  if ( RT.hPhoneXA)    { free((char*)RT.hPhoneXA);
                         RT.hPhoneXA = NULL; }
  if ( RT.dPhoneXA)    { free((char*)RT.dPhoneXA);
                         RT.dPhoneXA = NULL; }

  /* TODO: FREEING SOME OF THE TREE MALLOCS */
}

/* ------------------------------------------------------------------------
    InitSearchTree   prepares the generation of a search and actually
                     creates one, to find the maximum number of nodes
                     in that tree, when no LM constraints are specified.
   ------------------------------------------------------------------------ */

static int dpsInitSearchTree( void )
{
  WordX     w;
  int       i;
  int       maxModelX = -1;
  RTrellis* rtrellisA;
 
  /* Count the number of words consisting of one phone only. ALL words
     are include in that process and it is assumed that the filler words
     at the beginning of the dictionary are single phone words by themself */

  SD.sPhoneN   = SD.firstX;          /* reset number of single phone words */
  SD.sPhoneLmN = 0;

  for ( w = SD.firstX; w < SD.vocabN; w++) {
    if ( SD.vocabA[w].segN == 1) SD.sPhoneN++;
    else if ( maxModelX < SD.vocabA[w].xwbModelX)
              maxModelX = SD.vocabA[w].xwbModelX;
  }

  /* Create and initialize root trellis objects */

  RT.allocN   =  maxModelX+1;
  RT.nodeN    =  0;
  RT.trellisN =  0;
  RT.trellisA = (RTrellis*)malloc( RT.allocN * sizeof(RTrellis));

  for ( i = 0; i < RT.allocN; i++) dpsInitRTrellis( RT.trellisA+i, NULL);

  /* Create single phone root trellis objects including the filler words */

  rtrellisA   = (RTrellis*)malloc( SD.sPhoneN * sizeof(RTrellis));
  SD.sPhoneXA = (WordX*)malloc(    SD.sPhoneN * sizeof(WordX));

  /* Add/Init all single phone words from the dictionary. As above
     this also includes all the filler words */

  for ( i = 0, w = 0; w < SD.vocabN; w++) {
    if ( SD.vocabA[w].segN == 1) {

      dpsInitRTrellis( rtrellisA+i, SD.vocabA+w);
      SD.trellisPA[w] = (Trellis*)(rtrellisA + i);
      i++;
    }
  }

  /* Create the full tree once to find the maximum number of nodes in
     case all words without any LM restriction are allowed. This
     number is used to allocated array like the active trellis list. */

  dpsCreateSearchTree(0);
  dpsDeleteSearchTree();
  dpsCreateSearchTree(1);

#if 0
  /*attach word id's as eraly as possible for lm lookahead*/
  dpsInitSearchTreeLA(); 
#endif

  return 0;
}

/* ------------------------------------------------------------------------
    CreateSearchTree   allocates and initializes the search tree. This
                       can be done with respect to a specific language
                       model (i.e. the tree will include only vocabulary
                       words which are also in language model) or for all
                       words. Calling the function the first time, no LM
                       should be included in order to find maximum alloc
                       size [this is, what InitSearchTree does].
   ------------------------------------------------------------------------ */

static float dpsLPMinSubtree( Trellis* trP);
static void  dpsLPMinTree( void );
static void  dpsLmPenaltySubtree( Trellis* trP, float lp);
static void  dpsLmPenaltyTree( void );

static void dpsCreateSearchTree( char useLM)
{
  RTrellis* rtrP;
  WordX     w;
  int       i;
  int       discardedWordN = 0;

#if 0
  PhonesSet*  psP  = NULL;
  Phones*     phonesP = NULL;
  psP     = (PhonesSet*) itfGetObject("phonesSetDINO","PhonesSet"); /* MEGA HACK */
  phonesP = psP->list.itemA;
#endif

  if (useLM) INFO( "Creating search tree\n");
  else       INFO( "Estimating maximal search tree\n");

  if (! RT.dPhoneXA) RT.dPhoneXA = (int*)malloc( RT.allocN * sizeof(int));

  for ( w = 0; w < SD.vocabN; w++) RT.hPhoneXA[w] =  NILWORD;
  for ( i = 0; i < RT.allocN; i++) RT.dPhoneXA[i] = -1;

  RT.nodeN     = 0;  /* number of initialized node trellis objects */
  RT.trellisN  = 0;  /* number of initialized root trellis objects */

  SD.sPhoneLmN = 0;  /* number of one phone words in LM */

  for ( w = SD.firstX; w < SD.vocabN; w++) {
    VWord* vocabP = SD.vocabA + w;
    short  len    = vocabP->segN;
    short  p;

    /* Check, if the word is either one of the filler words or it
       is represented in the language model. */

    if ( useLM && SD.lmXA[vocabP->realX] < 0) {
         discardedWordN++;
         continue;
    }
    if ( len == 1) { SD.sPhoneXA[SD.sPhoneLmN++] = w; continue; }

    /* Insert word w into the trellis tree */

    if ( RT.dPhoneXA[vocabP->xwbModelX] < 0) {

      RT.dPhoneXA[vocabP->xwbModelX] = RT.trellisN;
      rtrP = RT.trellisA + RT.trellisN;
      dpsInitRTrellis( rtrP, vocabP);
      RT.trellisN++; 
    }
    else rtrP = RT.trellisA + RT.dPhoneXA[vocabP->xwbModelX];

    if ( len == 2) {        /* add word to penultimate word list */
      WordX j;

      if ((j = rtrP->wordX)   == NILWORD) rtrP->wordX = w;
      else {
        for ( ; RT.hPhoneXA[j] != NILWORD; j = RT.hPhoneXA[j]) ;
        RT.hPhoneXA[j] = w;
      }
    }
    else { /* insert node */

      short    phoneX = vocabP->segXA[1];
      int      modelX = vocabP->modelXA[1];
      Trellis* trP    = rtrP->childP;
      WordX    j;

      if (! trP) {
        trP = rtrP->childP = (Trellis*)malloc(sizeof(Trellis));
        dpsInitTrellis( trP, phoneX, modelX);
        RT.nodeN++;
      }
      else {
        Trellis* ptrP = NULL;

        for (; trP && (trP->modelX != modelX); ptrP = trP, trP = trP->nextP);
        if (! trP) {
          trP = ptrP->nextP = (Trellis*)malloc(sizeof(Trellis));
          dpsInitTrellis( trP, phoneX, modelX);
          RT.nodeN++;
        }
      }

      for ( p = 2; p < len-1; p++) {
        phoneX = vocabP->segXA[p];
        modelX = vocabP->modelXA[p];

        if (! trP->childP) {
          trP = trP->childP = (Trellis*)malloc(sizeof(Trellis));
          dpsInitTrellis( trP, phoneX, modelX);
          RT.nodeN++;
        }
        else {
          Trellis* ptrP = trP = trP->childP;

          for (; trP && (trP->modelX != modelX); ptrP = trP, trP = trP->nextP);
          if (! trP) {
            trP = ptrP->nextP = (Trellis*)malloc(sizeof(Trellis));
            dpsInitTrellis( trP, phoneX, modelX);
            RT.nodeN++;
          }
        }
      }
 
      /* insert word in the penultimate phone word list */

      if ((j = trP->info.wordX) == NILWORD) 
	trP->info.wordX = w;
      else {
        for ( ; RT.hPhoneXA[j] != NILWORD; j = RT.hPhoneXA[j]);
        RT.hPhoneXA[j] = w;
      }          
#if 0
      dpsInitSearchTreeLA();
#endif
    }
  }

  /* Let SD.sPhoneLmN be the number of real single phone words including
     the finish word silence while SD.sPhoneN designates the number of
     all single phone words in LM plus silence etc. */

  SD.sPhoneXA[SD.sPhoneLmN++] = SD.finishWordX;
  SD.sPhoneN = SD.sPhoneLmN;

  SD.sPhoneXA[SD.sPhoneN++]   = SD.startWordX;
  SD.sPhoneXA[SD.sPhoneN++]   = SD.silenceWordX;

  /* Now insert all other filler words as single phone words in the channels */

  for ( w = 0; w < SD.firstX; w++) {
    if ( w == SD.silenceWordX || w == SD.finishWordX || w == SD.startWordX) 
         continue;
    if ( useLM && (! ISA_FILLER(w)) && 
                  (SD.lmXA[SD.vocabA[w].realX] < 0))
         continue;

    SD.sPhoneXA[SD.sPhoneN++] = w;
  }

  if ( RT.maxNodeN < RT.nodeN + 1) {
    if ( RT.activePA[0]) free((char*)RT.activePA[0]);
    RT.maxNodeN = RT.nodeN + 128;

    RT.activePA[0] = (Trellis**)malloc(2 * RT.maxNodeN * sizeof(Trellis*)); 
    RT.activePA[1] =  RT.activePA[0] + RT.maxNodeN;
    RT.activeN[ 0] =  RT.activeN[ 1] = 0; 
  }  

  if (useLM) {
    dpsLPMinTree();
    dpsLmPenaltyTree();
  }

  INFO( "RT.allocN %d  RT.trellisN %d  RT.nodeN %d  SD.sPhoneN %d discardedWordN %d\n",
         RT.allocN, RT.trellisN, RT.nodeN, SD.sPhoneN, discardedWordN);
}

/* ------------------------------------------------------------------------
    DeleteSearchTree/Subtree  Depth-First deletion of the search tree
   ------------------------------------------------------------------------ */

static void dpsDeleteSearchTree( void )
{
  int      i;

  for (i = 0; i < RT.trellisN; i++) {
    Trellis* trP = RT.trellisA[i].childP;

    while (trP) {
      Trellis* nextP = trP->nextP;

      dpsDeleteSearchSubtree(trP);
      trP = nextP;
    }
    RT.trellisA[i].wordX  = NILWORD;
    RT.trellisA[i].childP = NULL;
  }
}

static void dpsDeleteSearchSubtree( Trellis* trP) 
{
  Trellis* childP;

  for ( childP = trP->childP; childP; ) {
    Trellis* nextP = childP->nextP;

    dpsDeleteSearchSubtree( childP);
    childP = nextP;
  }
  free( (char*)trP);
}

/* ------------------------------------------------------------------------
    dpsLPMinTree
   ------------------------------------------------------------------------ */

static float dpsLmLookaheadScore( WordX w)
{
  return lm_UgScore(SC.lmodel,SD.lmXA[SD.vocabA[w].realX]);
}


static void dpsLPMinTree( void )
{
  int      i;

  lm_CacheReset(SC.lmodel);

  for (i = 0; i < RT.trellisN; i++) {
    RTrellis* rtrP  = RT.trellisA + i;
    Trellis*  trP   = RT.trellisA[i].childP;
    float     lp    = WORSTSCORE;
    float     lprob;
    WordX     w;

    lm_NextFrame(SC.lmodel);

    while (trP) {
      Trellis* nextP = trP->nextP;

      if ((lprob = dpsLPMinSubtree(trP)) < lp) lp = lprob;
      trP = nextP;
    }
    for ( w = rtrP->wordX; w != NILWORD; w = RT.hPhoneXA[w]) {
      if ((lprob = dpsLmLookaheadScore(w)) < lp) lp = lprob;
    }
    RT.trellisA[i].lpMin  =   lp;
  }
}

static float dpsLPMinSubtree( Trellis* trP) 
{
  Trellis*  childP;
  float     lp = WORSTSCORE;
  float     lprob;
  WordX     w;

  for ( childP = trP->childP; childP; ) {
    Trellis*  nextP = childP->nextP;

    if ((lprob = dpsLPMinSubtree( childP)) < lp) lp = lprob;
    childP     = nextP;
  }
  for ( w = trP->info.wordX; w !=NILWORD; w = RT.hPhoneXA[w]) {
    if ((lprob = dpsLmLookaheadScore(w)) < lp) lp = lprob;
  }
  return trP->lpMin = lp;
}

/* ------------------------------------------------------------------------
    dps_LmPenaltyTree
   ------------------------------------------------------------------------ */

static void dpsLmPenaltySubtree( Trellis* trP, float lp);

static void dpsLmPenaltyTree( void )
{
  int      i;

  for (i = 0; i < RT.trellisN; i++) {
    /*    RTrellis* rtrP = RT.trellisA + i; */
    Trellis*  trP  = RT.trellisA[i].childP;
    float     lp   =  RT.trellisA[i].lpMin;

    while (trP) {
      Trellis* nextP = trP->nextP;

      dpsLmPenaltySubtree(trP, lp);
      trP = nextP;
    }
    RT.trellisA[i].lp =  RT.trellisA[i].lpMin;
  }
}

static VOID dpsLmPenaltySubtree( Trellis* trP, float lpMin) 
{
  Trellis*  childP;

  for ( childP = trP->childP; childP; ) {
    Trellis*  nextP = childP->nextP;
   
    dpsLmPenaltySubtree( childP, trP->lpMin);
    childP = nextP;
  }
  trP->lp =  trP->lpMin -  lpMin;
}

/* ------------------------------------------------------------------------
    AllocLastPhone     allocate all possibe context dependent word end 
                       models. The trellis objects are stored as a linked 
                       list sorted by their model index. This list is 
                       anchored in the SD.trellisPA[] array. This function
                       assumes that the indices in the compressed dipo
   ------------------------------------------------------------------------ */

Trellis* dpsAllocLastPhone( WordX w)
{
  Trellis* trP     =  SD.trellisPA[w];
  VWord*   vocabP  =  SD.vocabA + w;
  short    len     =  vocabP->segN;
  short    phoneX  =  vocabP->segXA[len-1];  
  int*     modelXA;
  int      i, j;

  modelXA = SD.xweModelA[vocabP->xweModelX].uniqXA;

  assert( RT.mpx);

  if ( trP == NULL || trP->modelX != *modelXA) {
    trP              = (Trellis*)malloc(sizeof(Trellis));
    trP->childP      =  SD.trellisPA[w];
    SD.trellisPA[w]  =  trP;

    trP->info.weDiId =  0;
    trP->bestScore   =  WORSTSCORE;
    for ( j = 0; j < STATESperTRELLIS-1; j++) trP->scoreA[j] = WORSTSCORE;
    trP->active      = -1;
    trP->modelX      = *modelXA;
    trP->phoneX      =  phoneX;
  }

  for ( i = 1, modelXA++; *modelXA >= 0; modelXA++, i++) {
    if ( trP->childP == NULL || trP->childP->modelX != *modelXA) {
      Trellis* ntrP  = (Trellis*)malloc(sizeof(Trellis));

      ntrP->childP     =  trP->childP;
      trP->childP      =  ntrP;
      trP              =  ntrP;

      trP->info.weDiId =  i;
      trP->bestScore   =  WORSTSCORE;
      for ( j = 0; j < STATESperTRELLIS-1; j++) trP->scoreA[j] = WORSTSCORE;
      trP->active      = -1;
      trP->modelX      = *modelXA;
      trP->phoneX      =  phoneX;
    }
    else trP = trP->childP;
  }
  return SD.trellisPA[w];
}

static void dpsFreeLastPhone( WordX w)
{
  Trellis* trP  = SD.trellisPA[w];
  Trellis* ntrP;

  for ( ; trP; trP = ntrP) {
    ntrP = trP->childP;
    free( (char*)trP);
  }
  SD.trellisPA[w] = NULL;
}

/* ------------------------------------------------------------------------
    InitRTrellis   initialize root trellis to WORSTSCORE and insert
                   model indices if <vocab> is defined
   ------------------------------------------------------------------------ */

void dpsInitRTrellis( RTrellis* trellis, VWord* vocab)
{
  int i;
  for ( i = 0; i < STATESperTRELLIS-1; i++) {
    trellis->scoreA[i]       = WORSTSCORE;
    trellis->pathA[i].frameX = NILBPTR;
    trellis->pathA[i].cnt    = 0;
  }
  trellis->bestScore   = WORSTSCORE;

  if ( vocab) {
    int modelX;

    trellis->dPhoneX = vocab->xwbModelX;
    trellis->phoneX  = vocab->segXA[0];

    modelX = SD.xwbModelA[trellis->dPhoneX].fullXA[SD.wBoundaryX];

    /* The first model index in vocabulary is a diphone index and NOT
       a model index, we have to translate it first */

    for ( i = 0; i < STATESperMODEL; i++) trellis->modelXA[i] = modelX;
  } 
  trellis->wordX      =  NILWORD;
  trellis->active     = -1;
  trellis->childP     =  NULL;
}

/* ------------------------------------------------------------------------
    ResetRTrellis   resets all root trellis object (single and multiphone)
                    to WORSTSCORE if they where active in frameX.
   ------------------------------------------------------------------------ */

static void dpsResetRTrellis( int frameX)
{
  int       i, j;
  RTrellis* rtrP = RT.trellisA;

  for ( i = RT.trellisN; i > 0; --i, rtrP++) {
    if ( rtrP->active == frameX) {
      for ( j = 0; j < STATESperTRELLIS-1; j++) rtrP->scoreA[j] = WORSTSCORE;
      rtrP->bestScore =  WORSTSCORE;
    }
  }
  for ( i = 0; i < SD.sPhoneN; i++) {
    WordX w =  SD.sPhoneXA[i];
    rtrP    = (RTrellis*)SD.trellisPA[w];

    if ( rtrP->active == frameX) {
      for ( j = 0; j < STATESperTRELLIS-1; j++) rtrP->scoreA[j] = WORSTSCORE;
      rtrP->bestScore =  WORSTSCORE;
    }
  }
}

/* ------------------------------------------------------------------------
    InitTrellis   initialize trellis to WORSTSCORE and insert model indices
   ------------------------------------------------------------------------ */

void dpsInitTrellis( Trellis* trellis, int phoneX, int modelX)
{
  int i;
  for ( i = 0; i < STATESperTRELLIS-1; i++) {
    trellis->scoreA[i]       = WORSTSCORE;
    trellis->pathA[i].frameX = NILBPTR;
    trellis->pathA[i].cnt    = 0;
  }
  trellis->bestScore  =  WORSTSCORE;
  trellis->phoneX     =  phoneX;
  trellis->modelX     =  modelX;
  trellis->info.wordX =  NILWORD;
  trellis->wordY      =  NILWORD;
  trellis->active     = -1;
  trellis->nextP      =  NULL;
  trellis->childP     =  NULL;
}

/* ------------------------------------------------------------------------
    DeactivateTrellis
   ------------------------------------------------------------------------ */

void dpsDeactivateTrellis( int frameX)
{
  int       i,j;
  RTrellis* rtrP = RT.trellisA;
  Trellis** atrP = RT.activePA[frameX & 0x1];
  Trellis*  trP;
  WordX*    awXP = SD.activeXA[frameX & 0x1];
  WordX     w;

  for ( i = RT.trellisN; i > 0; --i, rtrP++) {
    rtrP->active      = -1;
    rtrP->bestScore   =  WORSTSCORE;
    for ( j = 0; j < STATESperTRELLIS-1; j++) rtrP->scoreA[j] = WORSTSCORE;
  }

  i = RT.activeN[frameX & 0x1];
  for ( trP = *(atrP++); i > 0; --i, trP=*(atrP++)) {
    trP->active       = -1;
    trP->bestScore    =  WORSTSCORE;
    for ( j = 0; j < STATESperTRELLIS-1; j++) trP->scoreA[j] = WORSTSCORE;
  }

  i = SD.activeN[frameX & 0x1];
  for ( w = *(awXP++); i > 0; --i, w = *(awXP++)) {
    SD.activeA[w] = 0;
    dpsFreeLastPhone(w);
  }

  for ( i = 0; i < SD.sPhoneN; i++) {
    w    =  SD.sPhoneXA[i];
    rtrP = (RTrellis*)SD.trellisPA[w];

    rtrP->active      = -1;
    rtrP->bestScore   =  WORSTSCORE;
    for ( j = 0; j < STATESperTRELLIS-1; j++) rtrP->scoreA[j] = WORSTSCORE;
  }
}

/* ========================================================================
    Distribution Scoring:
   ======================================================================== */

int distScoreInit( DistScore* dsP)
{
  dsP->distN       =  0;                 /* overall number of distributions */
  dsP->frameX      = -1;                 /* current frame (first frame in
                                            the score vector array)         */
  dsP->frameN      =  0;                 /* number of frames                */
  dsP->compAllDist =  0;                 /* compute all distances each frame*/
  dsP->distScoreA  =  NULL;              /* "current" score vector          */
  dsP->activeDistA =  NULL;              /* vector of active distributions  */

  dsP->activeScoreA =  NULL;
  dsP->activeDistXA =  NULL;             /* vector of active distributions  */
  dsP->activeDistXN =  0;     

  dsP->bestScoreFMP     = NULL;
  dsP->lPBestScoreFMP   = NULL;
  dsP->rTrellisFMP = NULL;
  dsP->nTrellisFMP = NULL;
  dsP->wTrellisFMP = NULL;
  dsP->lTrellisFMP = NULL;
  dsP->scoreCallFMP = NULL;
  dsP->senoneMapSVP  = NULL;

  return TCL_OK;
}

int distScoreDeinit( DistScore* dsP)
{
  assert (dsP == dsP); /* keep compiler quiet */
  return TCL_OK;
}

/* ------------------------------------------------------------------------
    distScoreCreate  initialize data structures to evaluate distribution 
                     scores and enable fast phoneme matching (lookahead)
   ------------------------------------------------------------------------ */

int distScoreCreate()
{
  int i;

  /* Find maximum number of different distributions if not already defined */

  if (! DS.distN) {
    int j, n  = -1;

    for ( i = 0; i < SD.modelN; i++)
      for ( j = 0; j < SD.modelA[i].stateN; j++)
        if ( SD.modelA[i].senone[j] > n) n = SD.modelA[i].senone[j];

    DS.distN= n+1;
  }

  DS.bestScoreFMP     = fmatrixCreate(1,1); 
  DS.bestSenoneFMP     = fmatrixCreate(1,1); 
  DS.lPBestScoreFMP   = fmatrixCreate(1,1); 
  DS.rTrellisFMP      = fmatrixCreate(1,1);
  DS.nTrellisFMP      = fmatrixCreate(1,1);
  DS.wTrellisFMP      = fmatrixCreate(1,1);
  DS.lTrellisFMP      = fmatrixCreate(1,1);
  DS.scoreCallFMP     = fmatrixCreate(1,1);
  DS.senoneMapSVP     = svectorCreate(1);
  
  /* the following data arrays are allocated such that we can
     also handle distribution with index -1 (= unknown distribution). */

  DS.activeDistA = (char*)   malloc((DS.distN+1) * sizeof(char)) + 1;
  DS.distScoreA  = (float *) malloc((DS.distN+1)* sizeof(float)) + 1;
  DS.activeDistXA = (int*)   malloc((DS.distN+1) * sizeof(int))    + 1;
  DS.activeScoreA = (float*) malloc((DS.distN+1) * sizeof(float))  + 1;
  DS.activeDistXN =  0;

  return TCL_OK;

}

/* ------------------------------------------------------------------------
    distScoreFree
   ------------------------------------------------------------------------ */

void distScoreFree()
{

  if ( DS.activeDistA)    free((char*)(--DS.activeDistA));
  if ( DS.activeDistXA)  free((int *)(--DS.activeDistXA));
  if ( DS.activeScoreA)  free((int *)(--DS.activeScoreA));

  if ( DS.bestScoreFMP)   fmatrixFree (DS.bestScoreFMP);
  if ( DS.bestSenoneFMP)   fmatrixFree (DS.bestScoreFMP);
  if ( DS.lPBestScoreFMP) fmatrixFree (DS.lPBestScoreFMP);
  if ( DS.rTrellisFMP)    fmatrixFree (DS.rTrellisFMP);
  if ( DS.nTrellisFMP)    fmatrixFree (DS.nTrellisFMP);
  if ( DS.wTrellisFMP)    fmatrixFree (DS.wTrellisFMP);
  if ( DS.lTrellisFMP)    fmatrixFree (DS.lTrellisFMP);
  if ( DS.scoreCallFMP)   fmatrixFree (DS.scoreCallFMP);
  if ( DS.senoneMapSVP)   svectorFree (DS.senoneMapSVP);
}

/* ------------------------------------------------------------------------
    dsReset      reset the scoring arrays
   ------------------------------------------------------------------------ */

void dpsDsReset( short frameN)
{


  DS.frameN         =  frameN;
  DS.frameX         = -1;

  if (TF.pruneMethod == 1) { 
   if (DS.bestScoreFMP->m != frameN)
     ERROR("cannot prune from bestScoreFMP of length %d for %d frames\n",
           DS.bestScoreFMP->m,frameN );
           
   if (DS.lPBestScoreFMP->m != frameN)
     ERROR("cannot prune from lPBestScoreFMP of length %d for %d frames\n",
           DS.lPBestScoreFMP->m,frameN );
  }
  fmatrixResize(DS.bestScoreFMP,frameN,1);
  fmatrixResize(DS.bestSenoneFMP,frameN,1);
  fmatrixResize(DS.lPBestScoreFMP,frameN,1);
  fmatrixResize(DS.rTrellisFMP,frameN,1);
  fmatrixResize(DS.nTrellisFMP,frameN,1);
  fmatrixResize(DS.wTrellisFMP,frameN,1);
  fmatrixResize(DS.lTrellisFMP,frameN,1);
  fmatrixResize(DS.scoreCallFMP,frameN,1);
}

/* ------------------------------------------------------------------------
    Sort active node trellis objects
   ------------------------------------------------------------------------ */

static int compareTrellisP( const void * a, const void * b)
{
  Trellis* tra = *(Trellis **)a;
  Trellis* trb = *(Trellis **)b;

  if      ( tra->bestScore < trb->bestScore) return -1;
  else if ( tra->bestScore > trb->bestScore) return  1;
  else                                       return  0;
}

/* ------------------------------------------------------------------------
    ComputeTActiveDist   find all distribution that need to be computed
                         because some active trellis refers to it. This
                         function considers all active trellis object
                         within a tree.
   ------------------------------------------------------------------------ */

void dpsComputeTActiveDist( short frameX)
{
  RTrellis* rtrP  = RT.trellisA;
  Trellis** atrPA = RT.activePA[frameX & 0x1];
  WordX*    awXA  = SD.activeXA[frameX & 0x1];
  Trellis*  trP;
  int*      distXA;
  int       i, s, stateN;
  WordX     w;
  float     thresh =  bestScore + TF.beamWidth; /* HIER */
  float     lastPhoneThresh = lastPhoneBestScore +TF.lastPhoneBeamWidth;
  
  memset( DS.activeDistA, 0, DS.distN * sizeof(char));

  DS.activeDistXN = 0;

  /* Distributions in root trellis objects */

  for (i = RT.trellisN,  rtrP = RT.trellisA; i > 0; --i, rtrP++) {
    if ( rtrP->active == frameX) {
      stateN = SD.modelA[rtrP->modelXA[0]].stateN;
      distXA = SD.modelA[rtrP->modelXA[0]].senone;
      if (! DS.activeDistA[distXA[0]]) {
         DS.activeDistA[distXA[0]] = 1;
         DS.activeDistXA[DS.activeDistXN++] = distXA[0];
      }
      for ( s = 1; s < stateN; s++) {
        if (rtrP->scoreA[s] < thresh) {
          distXA = SD.modelA[rtrP->modelXA[s]].senone;
          if (! DS.activeDistA[distXA[s]]) {
            DS.activeDistA[distXA[s]] = 1;
            DS.activeDistXA[DS.activeDistXN++] = distXA[s];
          }
        } 
      }
    }
  }

  /* Top N Pruning of active node trellis objects */ 

  if ( TF.trellisTopN > 0 && TF.trellisTopN < RT.activeN[ frameX & 0x1]) {
    int (*cmp)(const void * a, const void *b) = compareTrellisP;

    qsort( RT.activePA[frameX & 0x1], 
           RT.activeN[ frameX & 0x1], sizeof(Trellis*), cmp);

    /* deactivate all pruned trellis objects */

    for ( i = TF.trellisTopN; i < RT.activeN[ frameX & 0x1]; i++) {

      RT.activePA[frameX & 0x1][i]->active    = -1;
      RT.activePA[frameX & 0x1][i]->bestScore =  WORSTSCORE;

      for ( s = 0; s < STATESperTRELLIS-1; s++) 
        RT.activePA[frameX & 0x1][i]->scoreA[s] = WORSTSCORE;
    }    

    RT.activeN[ frameX & 0x1] = TF.trellisTopN;
  }

  /* Distributions for node TRELLIS objects in tree */

  for ( i   = RT.activeN[ frameX & 0x1], trP = *(atrPA++); i > 0; --i, 
        trP = *(atrPA++)) {

    distXA = SD.modelA[trP->modelX].senone;
    stateN = SD.modelA[trP->modelX].stateN;

    if (! DS.activeDistA[distXA[0]]) {
      DS.activeDistA[ distXA[0]]         = 1;
      DS.activeDistXA[DS.activeDistXN++] = distXA[0];
    }
    for (s = 1; s < stateN; s++) {
       if (trP->scoreA[s] < thresh && ! DS.activeDistA[distXA[s]]) {
         DS.activeDistA[distXA[s]]          = 1; 
         DS.activeDistXA[DS.activeDistXN++] = distXA[s];
       }
    }
  }


  /* Distributions for word TRELLIS objects in tree */

  for ( i = SD.activeN[ frameX & 0x1], w = *(awXA++); i > 0;  --i, 
        w = *(awXA++)) {

    for (trP = SD.trellisPA[w]; trP; trP = trP->childP) {

      distXA = SD.modelA[trP->modelX].senone;
      stateN = SD.modelA[trP->modelX].stateN;
      if (!DS.activeDistA[distXA[0]]) {
        DS.activeDistA[distXA[0]] = 1;
        DS.activeDistXA[DS.activeDistXN++] = distXA[0];
      }

      for (s = 1; s < stateN; s++) {
	if (trP->scoreA[s] < lastPhoneThresh) {
          if (! DS.activeDistA[distXA[s]]) {
            DS.activeDistA[distXA[s]]          = 1;
            DS.activeDistXA[DS.activeDistXN++] = distXA[s];
	  }
	}
      }
    }
  }

  /* Distributions for single phone RTrellis objects */

  for ( i = 0; i < SD.sPhoneN; i++) {
    w    = SD.sPhoneXA[i];
    rtrP = (RTrellis*)SD.trellisPA[w];

    if ( rtrP->active == frameX) {
      stateN = SD.modelA[rtrP->modelXA[0]].stateN;
      distXA = SD.modelA[rtrP->modelXA[0]].senone;
      if (! DS.activeDistA[distXA[0]]) {
        DS.activeDistA[distXA[0]] = 1;
        DS.activeDistXA[DS.activeDistXN++] = distXA[0];
      }
      for ( s = 1; s < stateN; s++) {
	if (rtrP->scoreA[s] <lastPhoneThresh) {
          distXA = SD.modelA[rtrP->modelXA[s]].senone;
          if (! DS.activeDistA[distXA[s]]) {
            DS.activeDistA[distXA[s]] = 1;
            DS.activeDistXA[DS.activeDistXN++] = distXA[s];
          }
        }
      }
    }    
  }
}

/* ------------------------------------------------------------------------
    ComputeDistScore   compute the distribution scores for a given frame
                       either for all distributions or only an active
                       subset computed with dpsComputeTActiveDist. The
                       computed scores will be returned.
   ------------------------------------------------------------------------ */
int dpsComputeDistScore( short frameX, char* activeDist)
{

  /* HACK ALERT: verify that DS.distScoreA still contains the
     info from last frame or else we have a probem ! */

  int    d, n = 0;

  if ( activeDist) {
    SC.snsP->scoreAFctP( SC.snsP, DS.activeDistXA, DS.activeScoreA, DS.activeDistXN, frameX); 
    for ( d = 0; d < DS.distN; d++) 
                            DS.distScoreA[d] = WORSTSCORE/3;
    for ( d = 0; d < DS.activeDistXN; d++)
                  DS.distScoreA[DS.activeDistXA[d]] = DS.activeScoreA[d];
    n = DS.activeDistXN;

  } else {
    n = DS.distN;
    for ( d = 0; d < DS.distN; d++)
          DS.distScoreA[d] = SC.snsP->scoreFctP(SC.snsP,d,frameX);
  }

  return n;
}

/* ========================================================================
    BackPointer Table
   ======================================================================== */

int bpTableInit( BPTable* bptP)
{
  bptP->frameN    = 0;             /* number of frames for which the BPTable 
                                      is allocated                          */
  bptP->tableA    = NULL;          /* table of backpointers                 */
  bptP->tableN    = NULL;          /* number of table entries per row       */
  bptP->tablePA   = NULL;          /* table rows                            */

  bptP->idx       = 0;             /* index of first free position in table */
  bptP->wordTblXA = NULL;          /* word to table index for current frame */

  bptP->stackSize = 0;             /* maximal stack size per frame          */
  bptP->stackA    = NULL;          /* BP score stack for the current frame  */
  bptP->stackN    = NULL;          /* size of frame score stack             */
  bptP->stackPA   = NULL;          /* BP score stack                        */

  bptP->topX      = 0;             /* top of score stack                    */
  bptP->overflow  = 0;             /* backpointer table overflow            */
  return TCL_OK;
}

int bpTableDeinit( BPTable* bptP)
{
  assert (bptP == bptP); /* keep compiler quiet */
  return TCL_OK;
}

int  wordTransC = 0;           /* number of word transitions: # of frames   */
int  wordTransN = 0;           /* number of word transitions: # endpoints   */
int  wordTransM = 0;           /* number of word transitions: max # points  */

/* ------------------------------------------------------------------------
    bpTableCreate
   ------------------------------------------------------------------------ */

int bpTableCreate( short frameN)
{
  BP.frameN    =  frameN + 2;

  BP.tableA    = (BackP*) malloc( SD.vocabN    * sizeof(BackP));
  BP.tablePA   = (BackP**)malloc((BP.frameN+2) * sizeof(BackP*));
  BP.tableN    = (WordX*) malloc((BP.frameN+2) * sizeof(WordX));

  memset( BP.tablePA, 0, (BP.frameN+2) * sizeof(BackP*));
  memset( BP.tableN,  0, (BP.frameN+2) * sizeof(WordX));

  BP.tablePA++; BP.tableN++; /* make BP.tablePA[-1] valid */

  BP.stackSize = ((RT.mpx) ? SD.phoneN : 1) * SD.vocabN;
  BP.stackA    = (float*) malloc( BP.stackSize * sizeof(float));
  BP.stackPA   = (float**)malloc( BP.frameN    * sizeof(float*));
  BP.stackN    = (int*)   malloc( BP.frameN    * sizeof(int));

  memset( BP.stackPA, 0, BP.frameN * sizeof(float*));
  memset( BP.stackN,  0, BP.frameN * sizeof(int));

  BP.wordTblXA = (WordX*) malloc( SD.vocabN * sizeof(WordX));
  BP.overflow  = 0;
  BP.idx       = 0;
  BP.topX      = 0;
  return 0;
}

/* ------------------------------------------------------------------------
    bpTableFree
   ------------------------------------------------------------------------ */

void bpTableFree()
{
  if ( BP.tablePA) {  /* Free BackPointer table */
    int i;

    BP.tablePA--; BP.tableN--;
    for ( i = 0; i < BP.frameN+2; i++)
      if ( BP.tablePA[i]) free((char*)BP.tablePA[i]);

    free((char*)BP.tablePA);
    free((char*)BP.tableN);
  } 

  if ( BP.stackPA) {
    int i;

    for ( i = 0; i < BP.frameN; i++)
      if ( BP.stackPA[i]) free((char*)BP.stackPA[i]);

    free((char*)BP.stackPA);
    free((char*)BP.stackN);
  } 

  if ( BP.tableA)    { free((char*)BP.tableA);    BP.tableA    = NULL; }
  if ( BP.stackA)    { free((char*)BP.stackA);    BP.stackA    = NULL; }
  if ( BP.wordTblXA) { free((char*)BP.wordTblXA); BP.wordTblXA = NULL; }
  BP.frameN = 0;
}

/* ------------------------------------------------------------------------
    bpNextFrame
   ------------------------------------------------------------------------ */

void dpsBpNextFrame( short frameX)
{
  BP.idx  = 0;
  BP.topX = 0;

  assert(! BP.tablePA[frameX]);

  BP.tablePA[frameX] = BP.tableA;    /* set beginning of new frame */
  BP.tableN[ frameX] = 0;
  BP.stackPA[frameX] = BP.stackA;
  BP.stackN[ frameX] = 0;
}

/* ------------------------------------------------------------------------
    bpReduce
   ------------------------------------------------------------------------ */

static int compareBackP(const void * a, const void *b)
{
  BackP* bpa = (BackP *)a;
  BackP* bpb = (BackP *)b;

  if      ( bpa->wordX == SD.finishWordX) return -1;
  else if ( bpb->wordX == SD.finishWordX) return 1;
  else if ( bpa->score < bpb->score) return -1;
  else if ( bpa->score > bpb->score) return  1;
  else                               return  0;
}

void dpsBpSort()
{
  int (*cmp)(const void * a, const void * b) = compareBackP;

  qsort( BP.tableA, BP.idx, sizeof(BackP), cmp);
}

/* ------------------------------------------------------------------------
    bpSaveFrame   save the contents of the current (temporary) BackPointer
                  frame to the BackPointer table. This is used by tree and
                  flat forward.
   ------------------------------------------------------------------------ */

void dpsBpSaveFrame( short frameX, int topN)
{
  int bpN = (BP.idx < topN || topN < 1) ? BP.idx : topN;
  
  if ( BP.idx) {
    int b;
    BackP* bp = BP.tableA;
    for ( b = 0 ; b < BP.idx; b++, bp++) BP.wordTblXA[bp->wordX] = NILWORD;

    BP.tablePA[frameX] = (BackP*)malloc(bpN  * sizeof(BackP));
    BP.tableN[ frameX] = bpN;
    memcpy( BP.tablePA[frameX], BP.tableA, bpN  * sizeof(BackP));
  }
  else {
    BP.tablePA[frameX] =  NULL;
    BP.tableN[ frameX] =  0;
  }
  if ( BP.topX) {

    if ( bpN == BP.idx) {

      BP.stackPA[frameX] = (float*)malloc(BP.topX * sizeof(float));
      BP.stackN[ frameX] =  BP.topX;
      memcpy( BP.stackPA[frameX], BP.stackA, BP.topX * sizeof(float));
    }
    else {

      int    b;
      int    topX = 0;
      BackP* bp   = BP.tablePA[frameX];

      for ( b = 0 ; b < bpN; b++, bp++) {
        VWord* vocabP = SD.vocabA + bp->wordX;

        if ( vocabP->segN != 1)
             topX += SD.xweModelA[vocabP->xweModelX].uniqN;
        else topX++;
      }

      BP.stackPA[frameX] = (float*)malloc(topX * sizeof(float));
      BP.stackN[ frameX] =  topX;

      bp   = BP.tablePA[frameX];
      topX = 0;

      for ( b = 0 ; b < bpN; b++, bp++) {
        VWord* vocabP = SD.vocabA + bp->wordX;
        int    ssize;

        if ( vocabP->segN != 1)
             ssize = SD.xweModelA[vocabP->xweModelX].uniqN;
        else ssize = 1;
  
        if ( ssize > 1) memcpy( BP.stackPA[frameX]+topX, BP.stackA+bp->stackX, ssize * sizeof(float));
        else            BP.stackPA[frameX][topX] = BP.stackA[bp->stackX];

        bp->stackX  = topX;
        topX       += ssize;
      }
    }
  }
  else {
    BP.stackPA[frameX] =  NULL;
    BP.stackN[ frameX] =  0;
  }

  wordTransC++;
  wordTransN +=  bpN;
  if ( wordTransM < bpN) wordTransM = bpN;
}

/* ------------------------------------------------------------------------
    SaveBackPointer    writes a new cell to the BackPointer table for
                       a given frame, word and word-end-diphone triple.
   ------------------------------------------------------------------------ */

WordX dpsSaveBackPointer( short frameX, WordX w, float score, BPtr path, 
                           int   weRelDiX)
{
  WordX bpX = BP.wordTblXA[w];            /* find the BackPointer for w 
                                             in the current frame */
  if ( bpX != NILWORD) {

    /* If there is already a BP object for this word (in this frame)
       then we have still to update best score (over every possible context) 
       and path information */

    if ( score < BP.tableA[bpX].score) {
      if ( BP.tableA[bpX].path.frameX != path.frameX || 
           BP.tableA[bpX].path.bpX    != path.bpX) {
           BPtr newPath;
           newPath.frameX     = frameX;
           newPath.bpX        = bpX;
           BP.tableA[bpX].path = path;
           dpsSetBPRealWord( newPath);
      }
      BP.tableA[bpX].score = score;
    }
    BP.stackA[BP.tableA[bpX].stackX + weRelDiX] = score;
  }
  else {

    /* Insert a new BackP object into the table. Set score stack size
       and initialize stack to WORSTSCORE. */

    VWord* vocabP     = SD.vocabA + w;         /* vocabulary entry for w    */
    BackP* bpe        = BP.tableA + BP.idx;    /* next free pos in BP table */
    int    weDiModelN = 0;
    BPtr   newPath;

    if ( BP.topX >= BP.stackSize - SD.phoneN) {

      if (! BP.overflow) {
        ERROR( "BPTable OVERFLOW: IGNORING REST OF UTTERANCE!\n");
        BP.overflow = 1;
      }
      return -1;
    }

    BP.wordTblXA[w] = BP.idx;      /* map w-> BP index   */
    bpe->wordX      = w;           /* word index         */
    bpe->path       = path;        /* previous bp cell   */
    bpe->score      = score;       /* best score         */
    bpe->stackX     = BP.topX;     /* top of score stack */

    if ( vocabP->segN != 1 && RT.mpx) {
      float *stackA = BP.stackA + BP.topX;

      int    i;

      /* For multiplexed words longer than 1 we have to reserve a stack
         to save the scores for different word end models. */

      bpe->weDiX  =  vocabP->xweModelX;                  /* word end context */
      weDiModelN  =  SD.xweModelA[bpe->weDiX].uniqN;     /* max stack size   */

      for ( i = weDiModelN; i > 0; --i, stackA++) *stackA = WORSTSCORE;
      BP.stackA[BP.topX + weRelDiX] = score;       /* score to stack         */
    }
    else {
      bpe->weDiX         = -1;
      weDiModelN         =  1;
      BP.stackA[BP.topX] =  score;
    }
    newPath.frameX = frameX;                       /* for temporary use only */
    newPath.bpX    = BP.idx;
    dpsSetBPRealWord( newPath);                   /* correct prev words     */
    BP.idx++;                                      /* new free table pointer */
    BP.topX += weDiModelN;                         /* new free stack pointer */
  }
  return BP.wordTblXA[w];
}

/* ------------------------------------------------------------------------
    SetBPRealWord   find the indices of the real words (e.g. not silence).
                    To find the real word given a BackP object we follow
                    the back pointer as long as we find filler words only.
                    We also fill in the 3gram score if trigram scoring is
                    used in the forward pass.
   ------------------------------------------------------------------------ */

void dpsSetBPRealWord( BPtr path)
{
  BackP* bpe = BP.tablePA[path.frameX] + path.bpX;
  BPtr   pbp = path;
  WordX  w   = bpe->wordX;

  while (ISA_FILLER(w)) {       /* go back along the bp trace as long as
                                   there are only filler words like silence.
                                   Terminates, since we have a dedicated
                                   start word index which is no filler      */
    pbp = BP.tablePA[pbp.frameX][pbp.bpX].path;
    w   = BP.tablePA[pbp.frameX][pbp.bpX].wordX;
  }
  bpe->realWordX = SD.vocabA[w].realX;
    
  if ( SC.use3gFwd && pbp.frameX != NILBPTR) { 
                                /* if there is no previous word in backtrace
                                   we define it by NILWORD which should 
                                   correspond to NO_WORD in the LM module   */

       pbp            =  BP.tablePA[pbp.frameX][pbp.bpX].path;
       bpe->prevWordX = (pbp.frameX != NILBPTR) ? 
                         BP.tablePA[pbp.frameX][pbp.bpX].realWordX : NILWORD;
  } 
  else bpe->prevWordX = NILWORD;
}

/* ------------------------------------------------------------------------
    BackPointerScores  computes the acoustic and language model scores
                       for a BackPointer cell in BP.tablePA. z constitutes 
                       the language model weight correction factor.
   ------------------------------------------------------------------------ */

float dpsBackPointerScores( BackP* bp, float z, float* plmScore)
{
  short  pframeX = bp->path.frameX;
  WordX  pbpX    = bp->path.bpX;
  float  lmScore, aScore;

  if ( pframeX == NILBPTR) { lmScore = 0; aScore = bp->score; }
  else {
    BackP* pbp      =  BP.tablePA[pframeX] + pbpX;
    VWord* vocabP   =  SD.vocabA + bp->wordX;
    int*   weDiRelX = (pbp->weDiX >= 0) ? SD.xweModelA[pbp->weDiX].relXA : zeroA;
    float  score    =  BP.stackPA[pframeX][pbp->stackX+
                                           weDiRelX[vocabP->segXA[0]]];
                                           
                                           
    if (      bp->wordX == SD.silenceWordX) lmScore = SC.silenceWordPenalty;
    else if ( ISA_FILLER(bp->wordX))        lmScore = SC.fillerWordPenalty;
    else lmScore = lm_local_TgScore( pbp->prevWordX, pbp->realWordX, vocabP->realX);


    if (score < WORSTSCORE) {
      aScore = bp->score - score - lmScore;
    } else {
      WARN("fixed illegal score in bpTable (probably due to repair final)\n");
      WARN("%f %f %f\n", bp->score, score, lmScore);
      aScore = -lmScore -3000;
    }

  }
  if ( plmScore) *plmScore = lmScore;
  return aScore;
}

/* ------------------------------------------------------------------------
    CreateBPScoreTable
   ------------------------------------------------------------------------ */

float** dpsCreateBPScoreTable( short frameN, float z)
{
  float** scorePA = (float**)malloc( frameN*sizeof(float*));
  short   fX;

  for ( fX = 0; fX < frameN; fX++) {
    BackP*  bp  = BP.tablePA[fX];
    WordX   bpN = BP.tableN[ fX], bpX;

    scorePA[fX] = bpN ? (float*)malloc( bpN * sizeof(float)) : NULL;

    for ( bpX = 0; bpX < bpN; bpX++, bp++) {
      scorePA[fX][bpX] = dpsBackPointerScores(bp,z,NULL);
    }
  }
  return scorePA;
}

/* ------------------------------------------------------------------------
    FreeBPScoreTable
   ------------------------------------------------------------------------ */

void dpsFreeBPScoreTable( float** scorePA, short frameN)
{
  short   fX;

  for ( fX = 0; fX < frameN; fX++) {


    if ( scorePA[fX]) free((char*)scorePA[fX]);
  }
  free((char*)scorePA);
}

/* ========================================================================
    Evaluate Trellis Objects
   ======================================================================== */

/* ------------------------------------------------------------------------
    EvaluateRTrellis   evaluate all root trellis objects that are active
                       at frameX. Distribution score must already be comp.
   ------------------------------------------------------------------------ */
static float dpsEvaluateRTrellis( int frameX)
{
  RTrellis* rtrP = RT.trellisA;
  int       i    = RT.trellisN;
  float     best = WORSTSCORE;

  for ( ; i > 0; --i, rtrP++) {
    if ( rtrP->active == frameX) {

      rtrP->bestScore = SD.modelA[rtrP->modelXA[0]].evalFn->evalMpx( 
                        SD.modelA, rtrP->modelXA, DS.distScoreA,
                        rtrP->scoreA, rtrP->pathA, SD.durP);

      if ( best > rtrP->bestScore) best = rtrP->bestScore;

      /* INFO("ROOT %d %d %f\n",frameX,i,rtrP->bestScore); */
      rtrellisEvalN++;
    }
  }
  return best;
}

/* ------------------------------------------------------------------------
    EvaluateNTrellis  evaluate all node trellis objects that are active
                      at frameX. Distribution score must already be comp.
   ------------------------------------------------------------------------ */

static float dpsEvaluateNTrellis( int frameX)
{
  Trellis** trPA = RT.activePA[ frameX & 0x1];
  int       i    = RT.activeN[  frameX & 0x1];
  float     best = WORSTSCORE;
  Trellis*  trP  = *(trPA++);

  ntrellisEvalN += i;

  for ( ; i > 0; --i, trP = *(trPA++)){
    /* assert( trP->active == frameX); Why should we commment this assertion? ask ThomasS*/

    trP->bestScore = SD.modelA[trP->modelX].evalFn->eval(
                     SD.modelA + trP->modelX, DS.distScoreA,
                     trP->scoreA, trP->pathA, SD.durP);

    if ( best > trP->bestScore) best = trP->bestScore;
    /* INFO("NODE %d %d %f\n",frameX,i,trP->bestScore); */
  }

  return best;
}

/* ------------------------------------------------------------------------
    EvaluateWTrellis  evaluate all word trellis objects this includes 
                      the single phone words that are active at frameX. 
                      Distribution score must already be comp.
   ------------------------------------------------------------------------ */

static float dpsEvaluateWTrellis( int frameX)
{
  RTrellis* rtrP;
  Trellis*  trP;
  WordX*    awXA = SD.activeXA[ frameX & 0x1];
  int       i    = SD.activeN[  frameX & 0x1];
  float     best = WORSTSCORE;
  WordX     w;
  int       k = 0, j = 0;

  for ( w = *(awXA++); i > 0; --i, w = *(awXA++)) {
    /* assert( SD.activeA[w] != 0); Why should we commment this assertion? ask ThomasS*/

    SD.activeA[w] = 0;

    assert( SD.trellisPA[w] != NULL);

    for ( trP = SD.trellisPA[w]; trP; trP = trP->childP) {
      assert( trP->active == frameX);

      trP->bestScore = SD.modelA[trP->modelX].evalFn->eval(
                       SD.modelA + trP->modelX, DS.distScoreA,
                       trP->scoreA, trP->pathA, SD.durP);

      if ( best > trP->bestScore) best = trP->bestScore;

      k++;
    }
  }

  for ( i = 0; i < SD.sPhoneN; i++) {
    w    =  SD.sPhoneXA[i];
    rtrP = (RTrellis*)SD.trellisPA[w];

    if ( rtrP->active < frameX) continue;

    rtrP->bestScore = SD.modelA[rtrP->modelXA[0]].evalFn->evalMpx( 
                      SD.modelA, rtrP->modelXA, DS.distScoreA,
                      rtrP->scoreA, rtrP->pathA, SD.durP);

    if ( best > rtrP->bestScore ) best = rtrP->bestScore;

    j++;
  }

  k += j;
  ntrellisEvalN += k;
  ltrellisEvalN += k;
  wtrellisEvalN += SD.activeN[frameX & 0x1] + j;

  
  if (TF.pruneMethod == 1) {
    lastPhoneBestScore = DS.lPBestScoreFMP->matPA[frameX][0];
  }
  else {
    DS.lPBestScoreFMP->matPA[frameX][0] = lastPhoneBestScore = best;
  }

  /* INFO("LAST %d %f\n",frameX,lastPhoneBestScore); */
  return lastPhoneBestScore;
}

/* ------------------------------------------------------------------------
    EvaluateTrellis
   ------------------------------------------------------------------------ */

static float dpsEvaluateTrellis( int frameX)
{
  float best = WORSTSCORE, score;

  if ((score = dpsEvaluateRTrellis( frameX)) < best) best = score;
  if ((score = dpsEvaluateNTrellis( frameX)) < best) best = score;
  if ((score = dpsEvaluateWTrellis( frameX)) < best) best = score;
  
  if (TF.pruneMethod == 1) {
     bestScore = DS.bestScoreFMP->matPA[frameX][0];
  } 
  else {
     DS.bestScoreFMP->matPA[frameX][0] = bestScore = best;
  } 
  return bestScore;
}

/* ========================================================================
    Expand Trellis Objects
   ======================================================================== */
/* ------------------------------------------------------------------------
    ExpandRTrellis   prune currently active root trellis objects for the
                     next frame. Also perform exit transitions out of them
                     to active successors. Pruned out trellis are not yet
                     initialized to WORSTSCORE because they might be
                     reactivated by a transition from some other trellis in
                     the current frame. If not, they are initalized later on.
   ------------------------------------------------------------------------ */

static int dpsExpandRTrellis( short frameX)
{
  short       cf     = frameX, nf = cf + 1;
  RTrellis*   rtrP   = RT.trellisA;
  Trellis**   natrPA = RT.activePA[nf & 0x1]; /* next active trellis ptr.array */
  Trellis*    trP;
  LPhoneCand* candP  = RT.lPhoneCandA + RT.lPhoneCandN;
  int         i;
  WordX       w;
  float       thresh          = bestScore + TF.beamWidth;
  float       phoneThresh     = bestScore + TF.phoneBeamWidth;
  float       lastPhoneThresh = bestScore + TF.lastPhoneBeamWidth;
  float       score;

#if 0
  PhonesSet*  psP  = NULL;
  Phones*     phonesP = NULL;
  psP     = (PhonesSet*) itfGetObject("phonesSetDINO","PhonesSet"); /* MEGA HACK */
  phonesP = psP->list.itemA;
#endif

  for ( i = 0; i < RT.trellisN; i++, rtrP++) {
    if ( rtrP->active < cf) continue;       /* trellis is already inactive */

    if ( rtrP->bestScore < thresh) {
      rtrP->active = nf;              /* set trellis active for next frame */

      /* Consider transitions out of this root trellis */

      score = rtrP->scoreA[FINALSTATE] + SC.phoneInsertionPenalty;

      if ( score < phoneThresh) {

        /* Transition to all child trellis objects of the next tree level.
           Activate either if not already active in the current frame or
           if the new score is better compared to the first state score
           of the node trellis */

        for ( trP = rtrP->childP; trP; trP = trP->nextP) {
          float nscore = score + trP->lp;
          if ( score + dpsLaScore(frameX,trP,1) < phoneThresh && 
             ( trP->active < cf || nscore < trP->scoreA[0])) {


#if 0
	    Trellis* ntrP;
	    char nextPhones[256];
	    nextPhones[0]= 0;
	    for ( ntrP = trP->childP; ntrP; ntrP = ntrP->nextP) {
	      strcat(nextPhones," ");
	      strcat(nextPhones,phonesName(phonesP,ntrP->phoneX));
	    }

	    if (frameX < 50) {
	      INFO("frameX: %d root: %d : %s -> %s -> %s : %f\n",frameX, i, \
		   phonesName(phonesP,rtrP->phoneX),\
		   phonesName(phonesP,trP->phoneX),\
		   nextPhones,\
		   nscore);
	    }
#endif

            assert ( nscore < trP->scoreA[0]);
            trP->scoreA[0]    = nscore;
            trP->pathA[0]     = rtrP->pathA[FINALSTATE];
            trP->pathA[0].cnt = 1;
            trP->active       = nf;
            *(natrPA++)       = trP;

            if ( nscore < trP->bestScore) trP->bestScore = nscore;
          }
        }

        /* Transition to last phone of all words for which this is
           is the penultimate phone (the last phones may need multiple 
           right contexts) */

        if ( score < lastPhoneThresh) {
          for ( w = rtrP->wordX; w != NILWORD; w = RT.hPhoneXA[w]) {
            VWord* vocabP = SD.vocabA + w;

            if ( score + dpsLaScorePhone(frameX,vocabP->segXA[vocabP->segN - 1]) <
                 lastPhoneThresh) {

              candP->wordX = w;
              candP->score = score - rtrP->lpMin - SC.wordPenalty;
              candP->path  = rtrP->pathA[FINALSTATE];
              RT.lPhoneCandN++; candP++;
            }
          }
        }
      }
    }
  }
  return (RT.activeN[nf & 0x1] = natrPA - RT.activePA[nf & 0x1]);
}

/* ------------------------------------------------------------------------
    ExpandNTrellis   prune currently active node trellis objects for the
                     next frame. Also perform exit transitions out of them
                     to active successors. Pruned out trellis are not yet
                     initialized to WORSTSCORE because they might be
                     reactivated by a transition from some other trellis in
                     the current frame. If not, they are initalized later on.
   ------------------------------------------------------------------------ */

#if 0
static int activeTrellisTot = 0;
static int activeTrellisWID = 0;
#endif

static int dpsExpandNTrellis( short frameX)
{
  short       cf     = frameX, nf = cf + 1;
  Trellis**   atrPA  = RT.activePA[cf & 0x1];
  Trellis**   natrPA = RT.activePA[nf & 0x1] + RT.activeN[nf & 0x1];
  Trellis*    trP    = *(atrPA++);
  LPhoneCand* candP  = RT.lPhoneCandA + RT.lPhoneCandN;
  int         i;
  WordX       w;
  float       thresh          = bestScore + TF.beamWidth;
  float       phoneThresh     = bestScore + TF.phoneBeamWidth;
  float       lastPhoneThresh = bestScore + TF.lastPhoneBeamWidth;
  float       score;

#if 0
  PhonesSet*  psP  = NULL;
  Phones*     phonesP = NULL;
  psP     = (PhonesSet*) itfGetObject("phonesSetDINO","PhonesSet"); /* MEGA HACK */
  phonesP = psP->list.itemA;
#endif

#if 0
  VWord* vocabP  = NULL;
  BPtr       bp;
  BackP*    bpe  = NULL;
  float lmscore  = 0.0;
#endif

  for ( i = RT.activeN[cf & 0x1]; i > 0; --i, trP = *(atrPA++)) {
    assert( trP->active >= cf);

    if ( trP->bestScore < thresh) {
      if ( trP->active != nf) { trP->active = nf; *(natrPA++) = trP; }

#if 0
      /* get backpointer */
      bp  = trP->pathA[FINALSTATE];
      bpe = BP.tablePA[bp.frameX] + bp.bpX;
#endif

      /* Consider transitions out of this node trellis */

      score = trP->scoreA[FINALSTATE] + SC.phoneInsertionPenalty;

      if ( score < phoneThresh) {
        Trellis* ntrP = trP->childP;

        /* For all children of the current node, check whether they
           should be activated [either because they are inactive or
           because their first state score is worse than score] */

        for ( ; ntrP; ntrP = ntrP->nextP) {
          float nscore = score + ntrP->lp;

#if 0
	  /* word id is already known */
	  if (ntrP->wordY != NILVWORD) {
	    vocabP  = SD.vocabA + ntrP->wordY;
	    lmscore = lm_local_TgScore(bpe->prevWordX, bpe->realWordX, vocabP->realX);
	  } else {
	    lmscore = 0.0;
	  }
#endif

#if 0
	  activeTrellisTot++;
	  if (ntrP->wordY != NILVWORD) activeTrellisWID++;
#endif

          if ( score + dpsLaScore(frameX,ntrP,1) < phoneThresh &&
             ( ntrP->active < cf || nscore < ntrP->scoreA[0])) {
            assert (nscore < ntrP->scoreA[0]);
            ntrP->scoreA[0]    = nscore;
            ntrP->pathA[0]     = trP->pathA[FINALSTATE];
            ntrP->pathA[0].cnt = 1;
            if ( ntrP->active != nf) { ntrP->active = nf; *(natrPA++) = ntrP; }

            if ( nscore < ntrP->bestScore) ntrP->bestScore = nscore;
          }

	}

        /* Transition to last phone of all words for which this is
           is the penultimate phone (the last phones may need multiple 
           right contexts) */

        if ( score < lastPhoneThresh) {
          for ( w = trP->info.wordX; w != NILWORD; w = RT.hPhoneXA[w]) {
            VWord* vocabP = SD.vocabA + w;

            if ( score + dpsLaScorePhone(frameX,vocabP->segXA[vocabP->segN - 1]) <
                 lastPhoneThresh) {

              candP->wordX = w;
              candP->score = score - trP->lpMin - SC.wordPenalty;
              candP->path  = trP->pathA[FINALSTATE];
              RT.lPhoneCandN++; candP++;
            }
          }
        }
      }
    }
  }
  
  /* go over the active trellises a second time to clean up
    the deactivated trellises. It's a seperate run to make sure
    all trellises have been expanded into and are treated equally */

  atrPA  = RT.activePA[cf & 0x1];
  trP    = *(atrPA++);
  for ( i = RT.activeN[cf & 0x1]; i > 0; --i, trP = *(atrPA++)) {
    if ( trP->active != nf) { /* if they are not active now, nothing
                               can activate them */
      int j;
      trP->active = -1;
      trP->bestScore = WORSTSCORE;
      for ( j = 0; j < STATESperTRELLIS-1; j++) trP->scoreA[j] = WORSTSCORE;
    }    
  }
  return (RT.activeN[nf & 0x1] = natrPA - RT.activePA[nf & 0x1]);
}

/* ------------------------------------------------------------------------
    ExpandToLastPhone  expand into the last phone for all candidates words 
                       emerging from the TRELLIS tree.  Attach LM scores to 
                       such transitions. (Executed after pruning root and 
                       non-root, but before ExpandWords)
   ------------------------------------------------------------------------ */

static int dpsExpandToLastPhone( short frameX)
{
  int         cf      = frameX, nf = cf + 1;
  WordX*      nawXA   = SD.activeXA[nf & 0x1]; /* list of words supposed to
                                                 be active in the next frame */
  LPhoneCand* candP   = RT.lPhoneCandA;
  int         cframeN = 0;                    /* set list of start frame
                                                 candidates to 0 */
  float       best    = lastPhoneBestScore;
  float       thresh;
  float       dscore;
  int         i;

  for ( i = 0; i < RT.lPhoneCandN; i++, candP++) {  /* forall candidates */
    short  bframeX  =  candP->path.frameX;
    BackP* bpe      =  BP.tablePA[bframeX] + candP->path.bpX;
    int*   weDiRelX = (bpe->weDiX >= 0) ? SD.xweModelA[bpe->weDiX].relXA : zeroA;
    VWord* vocabP   =  SD.vocabA + candP->wordX;


    /* We need a this-word-only score for the candidate. Therefore we
       have to lookup the total score of the preceeding word considering
       the right word end context of that word. This score is substracted 
       from the candidates score */

    candP->score -= BP.stackPA[bframeX][bpe->stackX + weDiRelX[vocabP->segXA[0]]];

    /* Check whether this candidate was already considered in an earlier
       frame. If not, we have to prepare for finding the best transition
       score into the last phone [where delayed LM scores have to be
       considered to] */

    if ( RT.lWordTransA[candP->wordX].sFrameX != bframeX + 1) {
      int j;

      /* Check if there is already any entry into the candidate start
         frame list for the start frame in question */

      for ( j = 0; j < cframeN; j++) 
         if ( cframeA[j].eFrameX == bframeX) break;

      if ( j < cframeN) candP->nextX = cframeA[j].candX;
      else {
        if (cframeN >= cframeAllocN) {
          cframeAllocN +=  CANDFRAME_ALLOCSIZE;
          cframeA       = (CandFrame*)realloc( cframeA, cframeAllocN * 
                                               sizeof(CandFrame));
          INFO( "cframeA[] increased to %d entries\n", cframeAllocN);
        }
        j                  =  cframeN++;
        candP->nextX       = -1;
        cframeA[j].eFrameX =  bframeX;
      }
      cframeA[j].candX                     = i;
      RT.lWordTransA[candP->wordX].dscore  = WORSTSCORE;
      RT.lWordTransA[candP->wordX].sFrameX = bframeX + 1;
    }
  }

  /* For all different end frame candidate lists created above the language
     model score has to be calculated */

  for ( i = 0; i < cframeN; i++) {
    short  bframeX = cframeA[i].eFrameX;
    WordX  bpN     = BP.tableN[ bframeX];
    BackP* bpe     = BP.tablePA[bframeX];
    float* stackA  = BP.stackPA[bframeX];
    int    candX   = cframeA[i].candX;
    WordX  bpX     = 0;
    VWord* vocabP;

    for ( bpX = 0; bpX < bpN; bpX++, bpe++) {
      int* weDiRelX = (bpe->weDiX >= 0) ? SD.xweModelA[bpe->weDiX].relXA : zeroA;
      int  j;

      for ( j = candX; j >= 0; j = candP->nextX) {

        candP  = RT.lPhoneCandA + j;
        vocabP = SD.vocabA + candP->wordX;
        dscore = stackA[bpe->stackX + weDiRelX[vocabP->segXA[0]]] +
                 lm_local_TgScore(bpe->prevWordX, bpe->realWordX, vocabP->realX);

        if ( dscore < RT.lWordTransA[candP->wordX].dscore) {

          RT.lWordTransA[candP->wordX].dscore      = dscore;
          RT.lWordTransA[candP->wordX].path.frameX = bframeX;
          RT.lWordTransA[candP->wordX].path.bpX    = bpX;
        }
      }
    }
  }

  /* Update best transition for all last phone candidates in the list
     by the best word transition as found above */

  for ( i = 0, candP = RT.lPhoneCandA; i < RT.lPhoneCandN; i++, candP++) {

    candP->score += RT.lWordTransA[candP->wordX].dscore;
    candP->path   = RT.lWordTransA[candP->wordX].path;

    if ( best > candP->score) best = candP->score;
  }
 
  if (TF.pruneMethod == 1) {
    lastPhoneBestScore = DS.lPBestScoreFMP->matPA[frameX][0];
  }
  else {
    lastPhoneBestScore = best;
  }
  
  thresh = lastPhoneBestScore + TF.lastPhoneAloneBeamWidth; /* HIER */

  /* For each last phone candidate we have the best entry score including
     the language model score. So we consider each of the candidates
     and if their score is below a threshold, we allocate the leaf
     nodes for all possible word end context models and append these leafs
     to the list of active word trellis objects */

  for ( i = RT.lPhoneCandN, candP = RT.lPhoneCandA; i > 0; --i, candP++) {
    WordX w = candP->wordX;
    VWord*   vocabP = SD.vocabA + candP->wordX;

    if ( candP->score + dpsLaScorePhone(frameX,vocabP->segXA[vocabP->segN - 1]) < thresh) {
      int      k   = 0;
      Trellis* trP = dpsAllocLastPhone( w);

      for ( ; trP; trP = trP->childP) {
        if ( trP->active < cf || candP->score < trP->scoreA[0]) {
          assert (candP->score < trP->scoreA[0]);
          trP->scoreA[0]    = candP->score;
          trP->pathA[0]     = candP->path;
          trP->pathA[0].cnt = 1;

          assert( trP->active != nf);

          trP->active = nf;
          k++;
        }
      }
      if ( k > 0) {
        assert(! SD.activeA[w]);
        *(nawXA++)    = w;
        SD.activeA[w] = 1;
      }
    }
  }
  return (SD.activeN[nf & 0x1] = nawXA - SD.activeXA[nf & 0x1]);
}

/* ------------------------------------------------------------------------
    ExpandLeafs   expand and prune already active leaf trellis objects as 
                  well as single phone root trellis objects.
   ------------------------------------------------------------------------ */

static int dpsExpandLeafs( short frameX)
{
  short     cf     = frameX, nf = cf + 1;
  WordX*    awXA   = SD.activeXA[cf & 0x1];
  WordX*    nawXA  = SD.activeXA[nf & 0x1] + SD.activeN[nf & 0x1];
  float     wordThresh      = lastPhoneBestScore + TF.wordBeamWidth;
  float     lastPhoneThresh = lastPhoneBestScore + TF.lastPhoneAloneBeamWidth;

  Trellis*  trP;
  RTrellis* rtrP;
  int       i  =   SD.activeN[cf & 0x1];
  WordX     w  = *(awXA++);

  for ( ; i > 0; --i, w = *(awXA++)) {
    Trellis** ptrP = SD.trellisPA + w;  /* list of context trellis objects */
    Trellis*  ntrP;
    int       k    = 0;

    /* Consider all possible word ends, prune or activate words */

    for ( trP = SD.trellisPA[w]; trP; trP = ntrP) {
      assert( trP->active >= cf);

      ntrP = trP->childP;
      if ( trP->bestScore < lastPhoneThresh) {
        trP->active =   nf;
        ptrP        = &(trP->childP);
        k++;

        if ( trP->scoreA[FINALSTATE] < wordThresh) {
          dpsSaveBackPointer( frameX, w, trP->scoreA[FINALSTATE],
                                          trP->pathA[FINALSTATE],
                                          trP->info.weDiId);
        }
      }
      else if ( trP->active == nf) ptrP = &(trP->childP);
      else {
        free( (char*)trP);
        *ptrP = ntrP;
      }
    }
    if ( k > 0 && (! SD.activeA[w])) {
      *(nawXA++)   = w;
      SD.activeA[w] = 1;
    }
  }
  SD.activeN[nf & 0x1] = nawXA - SD.activeXA[nf & 0x1];

  /* Expand/Prune permanently allocated single phone words */

  for ( i = 0; i < SD.sPhoneN; i++) {
    w    =  SD.sPhoneXA[i];
    rtrP = (RTrellis*)SD.trellisPA[w];

    if ( rtrP->active >= cf && rtrP->bestScore < lastPhoneThresh) {
      rtrP->active = nf;

      if ( rtrP->scoreA[FINALSTATE] < wordThresh) {
           dpsSaveBackPointer( frameX, w, rtrP->scoreA[FINALSTATE],
                                           rtrP->pathA[ FINALSTATE], 0);
      }
    }
  }
  return SD.activeN[nf & 0x1];
}

/* ------------------------------------------------------------------------
    ExpandTrellis    expand/prune all active trellis objects in tree
   ------------------------------------------------------------------------ */

static int dpsExpandTrellis( short frameX)
{
  RT.lPhoneCandN = 0;       /* reset the list of last phone candidate words */

  dpsExpandRTrellis(    frameX);
  dpsExpandNTrellis(    frameX);
  dpsExpandToLastPhone( frameX);
  dpsExpandLeafs(       frameX);

  return TCL_OK;
}

/* ========================================================================
    Word to Word Transitions
   ======================================================================== */

typedef struct BestWEBP_S {

  float score;
  WordX bpX;
  int   phoneX;      /* terminating monophone index of the word that has
                        the best score when considering a transition to
                        the phone respresented here */
} BestWEBP;

BestWEBP bestWeBP[MAX_MONOPHONES];

/* ------------------------------------------------------------------------
    dpsWordTransitions
   ------------------------------------------------------------------------ */

static void dpsWordTransitions( short frameX)
{
  short      cf = frameX, nf = cf + 1;
  RTrellis*  rtrP;
  BestWEBP*  bweBP;
  VWord*     vocabP, *pvocabP;
  int*       weDiRelX;
  float*     weScoreStack;
  BackP*     bpe         = BP.tableA;
  float      phoneThresh = bestScore + TF.phoneBeamWidth;
  float      score;
  int        phoneX;
  int        i, j, k = 0;
  WordX      bp,w,bpN;

  if (phoneThresh > WORSTSCORE) phoneThresh = WORSTSCORE;

  bpN = (BP.idx < TF.topN || TF.topN < 1) ? BP.idx : TF.topN;

  for ( i = SD.phoneN-1; i >= 0; --i) bestWeBP[i].score = WORSTSCORE;

  /* For all words ending in the current frame (= have an entry in the
     back pointer table) we test the transition to all monophones.
     For each monophone the best context dependent word-end score
     is stored in bestWeBP[] including the path and the last monophone
     within the word. */

  for ( bp = 0; bp < bpN; bp++, bpe++) {

    if ( bpe->wordX == SD.finishWordX) continue;

    k++;
    vocabP       =  SD.vocabA + bpe->wordX;
    weDiRelX     = (bpe->weDiX >= 0) ? SD.xweModelA[bpe->weDiX].relXA : zeroA;
    phoneX       =  vocabP->segXA[vocabP->segN - 1];
    weScoreStack =  BP.stackA + bpe->stackX;

    for ( j = SD.phoneN-1; j >= 0; --j) {
      if ( weScoreStack[weDiRelX[j]] < bestWeBP[j].score) {

        bestWeBP[j].score  = weScoreStack[weDiRelX[j]];
        bestWeBP[j].bpX    = bp;
        bestWeBP[j].phoneX = phoneX;
      }
    } 
  }
  
  if ( k == 0) return;

  /* Hypothesize successors to words finished in this frame. To that end
     we have to consider all root trellis objects th*/

  for ( i = RT.trellisN, rtrP = RT.trellisA; i > 0; --i, rtrP++) {
    bweBP = bestWeBP + rtrP->phoneX;
    score = bweBP->score   + rtrP->lpMin + 
            SC.wordPenalty + SC.phoneInsertionPenalty;

    if ( score + dpsLaScoreR(frameX,rtrP,1) - rtrP->lpMin < phoneThresh &&
       ( rtrP->active < cf || score < rtrP->scoreA[0])) {
      if (score >= rtrP->scoreA[0]) {
         INFO ("score: %f; rtrP->scoreA[0]: %f; phonethresh: %f ",
                score,     rtrP->scoreA[0],     phoneThresh);
      }

      assert (score < rtrP->scoreA[0]);
      rtrP->scoreA[0]       = score;
      rtrP->pathA[0].frameX = cf;
      rtrP->pathA[0].bpX    = bweBP->bpX;
      rtrP->pathA[0].cnt    = 1;
      rtrP->active          = nf;
      rtrP->modelXA[0]      = SD.xwbModelA[rtrP->dPhoneX].fullXA[bweBP->phoneX];
    }
  }
  
  /* Single phone words: Since we have not yet considered the LM probability
     for these words it is necessary to find the best transition first */

  for ( i = 0; i < SD.sPhoneLmN; i++) {
    RT.lWordTransA[SD.sPhoneXA[i]].dscore      = WORSTSCORE;
    RT.lWordTransA[SD.sPhoneXA[i]].path.frameX = cf;
  }

  for ( bpe = BP.tableA, bp = 0; bp < bpN; bp++, bpe++) {
    weDiRelX     = (bpe->weDiX >= 0) ? SD.xweModelA[bpe->weDiX].relXA : zeroA;
    weScoreStack =  BP.stackA + bpe->stackX;

    for ( i = 0; i < SD.sPhoneLmN; i++) {
      w      = SD.sPhoneXA[i];
      vocabP = SD.vocabA + w;
      score  = weScoreStack[weDiRelX[vocabP->segXA[0]]] +
               lm_local_TgScore(bpe->prevWordX, bpe->realWordX, vocabP->realX);

      if ( score < RT.lWordTransA[w].dscore) {
        RT.lWordTransA[w].dscore   = score;
        RT.lWordTransA[w].path.bpX = bp;
      }
    }
  }

/* ah, ton pis. ci on sais que c'est faux, on peu le faire. mais il faudrait
   faire le meme chose dans le 'beamfinder'
  phoneThresh = WORSTSCORE/2;  */

 /* I had set it to WORSTSCORE/2 (not WORSTSCORE) to avoid inactives slipping through
   it's not wise to prune against the normal phoneThresh because score contains lm  
   and word-transition penalty, bestscore does not; but then our lookaheads
   won't help to avoid those nasty things to become alive */
  
  for ( i = 0; i < SD.sPhoneLmN; i++) {
    w      =  SD.sPhoneXA[i];
    rtrP   = (RTrellis*)SD.trellisPA[w];
    score  =  RT.lWordTransA[w].dscore + SC.phoneInsertionPenalty;

    if ( w != SD.finishWordX && 
       ( score + dpsLaScoreR(frameX,rtrP,1) >= phoneThresh /*+100*/)) continue;

    if ( score < phoneThresh /* +100 */) {
      BackP* bpe = BP.tableA + RT.lWordTransA[w].path.bpX;
      pvocabP    = SD.vocabA + bpe->wordX;

      if ( rtrP->active < cf || score < rtrP->scoreA[0]) {
        assert ( score < rtrP->scoreA[0]);
        rtrP->scoreA[0]    = score;
        rtrP->pathA[0]     = RT.lWordTransA[w].path;
        rtrP->pathA[0].cnt = 1;
        rtrP->active       = nf;
        rtrP->modelXA[0]   = 
            SD.xwbModelA[rtrP->dPhoneX].fullXA[pvocabP->segXA[pvocabP->segN-1]];
      }
    }
  }

  /* The silence word is not part of LM and non multiplexed. */

  bweBP = bestWeBP     + SD.wBoundaryX;
  score = bweBP->score + SC.silenceWordPenalty + SC.phoneInsertionPenalty;

  if ( score -SC.silenceWordPenalty < phoneThresh) {
    rtrP = (RTrellis*)SD.trellisPA[SD.silenceWordX];
    if ( rtrP->active < cf || score < rtrP->scoreA[0]) {
        assert (score < rtrP->scoreA[0]);
        rtrP->scoreA[0]       = score;
        rtrP->pathA[0].frameX = cf;
        rtrP->pathA[0].bpX    = bweBP->bpX;
        rtrP->pathA[0].cnt    = 1;
        rtrP->active          = nf;
    }
  }

  /* Filler words not in the LM */

  score = bweBP->score + SC.fillerWordPenalty + SC.phoneInsertionPenalty;

  if ( SD.firstX > 3 && (score -SC.fillerWordPenalty) < phoneThresh) {
    for ( w = 0; w < SD.firstX; w++) {

      if ( w == SD.silenceWordX || ! ISA_FILLER(w)) continue;

      rtrP = (RTrellis*)SD.trellisPA[w];
      if ( rtrP->active < cf || score < rtrP->scoreA[0]) {
        assert ( score < rtrP->scoreA[0]);
        rtrP->scoreA[0]       = score;
        rtrP->pathA[0].frameX = cf;
        rtrP->pathA[0].bpX    = bweBP->bpX;
        rtrP->pathA[0].cnt    = 1;
        rtrP->active          = nf;
      }
    }
  }
}

/* ========================================================================
    TreeForward
   ======================================================================== */
/* ------------------------------------------------------------------------
    PrintScale
   ------------------------------------------------------------------------ */

INT dpsPrintScale( SHORT frameN)
{
  SHORT frameX;
  SHORT colN = ( SC.printWidth > 1) ? ((frameN / SC.printWidth)+1) : 10;

  for ( frameX = 0; frameX < frameN; frameX++) {
    if      ( frameX % (10*colN) == 0) { fprintf(STDERR,"|"); }
    else if ( frameX % ( 5*colN) == 0) { fprintf(STDERR,"+"); }
    else if ( frameX %     colN  == 0) { fprintf(STDERR,"-"); }
  }
  fprintf(STDERR,"[%d]\n", frameN);
  return colN;
}

/* ------------------------------------------------------------------------
    InitTFwdOncePerUtterance  allocates the BackPointer table, checks the
                              language model and initializes the begin
                              silence trellis.
   ------------------------------------------------------------------------ */

static void dpsInitTFwdOncePerUtterance( short frameN)
{

  RTrellis* rtrP;
  int       s;
  WordX     w;

  if ( BP.frameN) bpTableFree();  /* allocate new table of BackPointers */
  bpTableCreate( frameN);

#if 0

  if ((lm = lmGetCurrent()) != SC.lmodel) { /* check if LM has changed */

    if ( SC.lmodel) dpsDeleteSearchTree();
     dpsCreateSearchTree(1);
    SC.lmodel = lm;
  }
#endif

  /* lm_CacheReset(SC.lmodel); HIER */
  dpsDsReset(frameN);
  dpsLaReset(frameN);

  wordTransC = wordTransN = wordTransM = 0;
  compDistN  = compDistPerFrameN = 0;
  bestScore  = WORSTSCORE;

  for ( w = 0; w < SD.vocabN; w++) {
    BP.wordTblXA[w]           =  NILWORD;
    RT.lWordTransA[w].sFrameX = -1;
  }

  SD.activeN[0] = 0; SD.activeN[1] = 0;
  RT.activeN[0] = 0; RT.activeN[1] = 0;

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
}

static void dpsDeinitTFwdOncePerUtterance( short frameX)
{
  dpsDeactivateTrellis( frameX);  
}

/* ------------------------------------------------------------------------
    TFwdOneFrame    evaluate frameX, prune trellis objects and try word
                    transitions.
   ------------------------------------------------------------------------ */

static void dpsTFwdOneFrame( short frameX)
{

  static int lastcomputed = 0;


  dpsBpNextFrame( frameX);

  /* lookahead stuff */
  dpsLaNextFrame( frameX);
  bestPhoneScore = dpsEvaluatePhoneScores( frameX);

  if ( ! DS.compAllDist) {  /* Calculate distribution scores */

    /* can I choose these more wisely, maybe using lookahead scores ? */

    /* differs = nextFrameDiffers(); */

    if ((TF.skip == 0) || (frameX % TF.skip != 0)) { /* POINT 2 */
     lastcomputed = frameX;
     dpsComputeTActiveDist( frameX);

     compDistN += 
         compDistPerFrameN = dpsComputeDistScore( frameX, DS.activeDistA);
    } else {
      compDistPerFrameN = 0;
    }
  }
  else {
    compDistN += compDistPerFrameN = dpsComputeDistScore( frameX, NULL);
  }

  /* Evaluate, Expand and Prune Tree Trellis objects */

  bestScore      = dpsEvaluateTrellis(     frameX);

  /* INFO("BEST %d %f\n",frameX,bestScore); */
  dpsExpandTrellis( frameX);

  /* If there are new BP Table entries try word to word transitions */

  if ( BP.idx) {   /* test for word transitions and save the new
                      entries in the BackPointer table */

    dpsBpSort();
    dpsWordTransitions( frameX);
  }

  dpsBpSaveFrame(   frameX,TF.topN);
  dpsResetRTrellis( frameX);  /* reset all root trellis object that were
                                  active in frameX but have not been 
                                  activated for the next frame */
  if (frameX % 2 == 0) lm_NextFrame(SC.lmodel); /* don't need to be every frame ! */
}

/* ------------------------------------------------------------------------
    TreeForward  dynamic programming matching routine based on a tree
                 of allophonic models.
   ------------------------------------------------------------------------ */

float dpsTreeForward( void* sP, char* evalS)
{
  int   colN = 10;
  short frameX, frameN;
  short scFrameNOrig = -1;
  int weHaveAll = 0, runon =0;
  Timer stopit;
  Search* searchP = (Search *) sP;

  if (sc.snsP->scoreAFctP != &snsScoreArray && sc.snsP->scoreAFctP != &snsInlineScoreArray) {
    WARN("Changed score function pointer according to the old decoder (-> SenoneSet setScoreFct)\n");

    /*  doesn't exists anymore ...  if (sc.snsP->refMax == 0) { ask nobody@dev.zero */
    if (0) {
      sc.snsP->scoreFctP  = &snsScore;
      sc.snsP->scoreAFctP = &snsScoreArray;
      if (sc.la.snsP) {
	sc.la.snsP->scoreFctP  = &snsScore;
	sc.la.snsP->scoreAFctP = &snsScoreArray;
      }
    } else {
      sc.snsP->scoreFctP  = &snsScore;
      sc.snsP->scoreAFctP = &snsInlineScoreArray;
      if (sc.la.snsP) {
	sc.la.snsP->scoreFctP  = &snsScore;
	sc.la.snsP->scoreAFctP = &snsInlineScoreArray;
      }
    }
  }

#if 0
  activeTrellisTot = 0;
  activeTrellisWID = 0;
#endif

  if (evalS)
    if ( evalS && (snsFesEval(sc.snsP,evalS) != TCL_OK)) {
       ERROR("Error evaluating feature description file.\n");
       return TCL_ERROR;
    }

  sc.frameN = snsFrameN(sc.snsP, &(sc.from),&(sc.shift),&weHaveAll);

  if ((!evalS) && (!weHaveAll)) {
     ERROR("Search must always use -eval if in runon mode !.\n");
     return TCL_ERROR;
  }

  runon = !weHaveAll;
  timerReset(&stopit);
  timerStart(&stopit);

  INFO("Initializing TreeForward\n");

  if (!runon && (sc.frameN < 3)) return 0.0;

  if (!runon) dpsInitTFwdOncePerUtterance( sc.frameN);
  else        dpsInitTFwdOncePerUtterance( sc.tf.maxFramesRunon); /* runon mode */

  INFO("TreeForward Init OK\n");

  if (weHaveAll && sc.verbose < 2) colN = dpsPrintScale(sc.frameN);

  frameX = 0;
  while (1) {
    if (!weHaveAll){
            frameN = sc.frameN - LA.matchfN;
    } else {
      frameN = sc.frameN;
      LA.frameN = sc.frameN;
    }
     
    for ( ; frameX < frameN; frameX++) {

      rtrellisEvalN =0; ntrellisEvalN =0; ltrellisEvalN =0; wtrellisEvalN =0;

      dpsTFwdOneFrame( frameX);

      DS.rTrellisFMP->matPA[frameX][0]  = rtrellisEvalN;
      DS.nTrellisFMP->matPA[frameX][0]  = ntrellisEvalN;
      DS.lTrellisFMP->matPA[frameX][0]  = ltrellisEvalN;
      DS.wTrellisFMP->matPA[frameX][0]  = wtrellisEvalN;
      DS.scoreCallFMP->matPA[frameX][0] = compDistPerFrameN;
   
      if ( sc.verbose == 2 && frameX % sc.tf.publishFrameN == 0) {
        char objname[1000];
        sprintf(objname,"%s.treeFwd.hypoList",searchP->name);
        dpsBacktraceHypothesis( sc.frameN, 0, 1.0, &TF.hypoList);
        searchCfgUpdate(searchP);
	/* An TclError in the Publish Function stops decoding! */
	if (TCL_ERROR == hypoListPublish (objname) || strcmp(itf->result,"1") == 0) {
	  INFO("TreeForward stopped at frame %d!\n",frameX);
	  dpsBacktraceHypothesis(sc.frameN, 1, 1.0, &TF.hypoList);
          
          /* Hack */
          dpsDeinitTFwdOncePerUtterance(0);
          dpsDeinitTFwdOncePerUtterance(1);
          /* dpsDeinitTFwdOncePerUtterance(frameX); */
          
          INFO( "score %f frames %d -> %.2f wtrans./frame [max. %d].\n", 
                bestTotalScore,
                wordTransC, (float)wordTransN/(float)wordTransC,
                wordTransM);
          
          INFO("End of Tree Forward (%f secs)\n", timerStop(&stopit));
          fflush( stdout);        fflush(stdout); fflush(stderr);
          
          return ( bestTotalScore );
        }
        fflush(stdout); fflush(stderr);
      }      
      else if ( sc.verbose < 2 && frameX % colN == 0) { fprintf(STDERR,"|"); }
    }

    if (weHaveAll) break;

    INFO("checking for more data at frame %d\n",sc.frameN);
    if ( evalS && (snsFesEval(sc.snsP,evalS) != TCL_OK)) {
       ERROR("Error evaluating feature description file.\n");
       return TCL_ERROR;
    }
    sc.frameN = snsFrameN(sc.snsP, &(sc.from), &(sc.shift), &weHaveAll);
    if (sc.frameN >= sc.tf.maxFramesRunon) {
      WARN("too many frames for runon mode, truncating at %d\n",sc.tf.maxFramesRunon);
      sc.frameN = sc.tf.maxFramesRunon;
    }
    if (weHaveAll) LA.frameN = sc.frameN;
  }
  if ( sc.verbose < 2) fprintf( STDERR, "\n");

  INFO("WE HAVE ALL, will backtrace\n");

  /* search was not cancled so repair should not hurt */
  if (scFrameNOrig != -1) {
    dpsBacktraceHypothesis( scFrameNOrig, 1, 1.0, &TF.hypoList);
  } else {
    dpsBacktraceHypothesis( sc.frameN, 1, 1.0, &TF.hypoList);
  }

  if (!runon) dpsDeinitTFwdOncePerUtterance(sc.frameN);
  else dpsDeinitTFwdOncePerUtterance(sc.frameN);

  if ( wordTransC ) {
    INFO( "score %f frames %d -> %.2f wtrans./frame [max. %d].\n", 
          bestTotalScore,
          wordTransC, (float)wordTransN/(float)wordTransC,
          wordTransM);
  } else {
    INFO( "score %f frames %d -> %.2f wtrans./frame [max. %d].\n", 
        bestTotalScore,
          wordTransC, (float)wordTransN/(float)1,
          wordTransM);    
  }
  INFO("End of Tree Forward (%f secs)\n", timerStop(&stopit));
  fflush( stdout);

#if 0
  INFO("active Trellis:              %d\n", activeTrellisTot);
  INFO("active Trellis with word ID: %d\n", activeTrellisWID);
#endif



  return ( bestTotalScore );
}

/* ========================================================================
    Tree Initialization
   ======================================================================== */

static Method treeFwdMethod[] = 
{ 
  { "puts", treeFwdPutsItf, NULL },
  {  NULL,  NULL,           NULL } 
};

static TypeInfo treeFwdInfo = { "TreeFwd", 0, -1, treeFwdMethod, 
                                 NULL, NULL,
                                 treeFwdConfigureItf, treeFwdAccessItf, itfTypeCntlDefaultNoLink,
	                        "Search: Tree Forward Module" } ;

static int tfwdInitialized = 0;

int TreeFwd_Init( )
{
  if (! tfwdInitialized) {
    int i;

    treeFwdDefaults.beamWidth                =  150;
    treeFwdDefaults.phoneBeamWidth           =  100;
    treeFwdDefaults.wordBeamWidth            =  100;
    treeFwdDefaults.lastPhoneBeamWidth       =   80;
    treeFwdDefaults.lastPhoneAloneBeamWidth  =    0;

    treeFwdDefaults.topN                     =    0;
    treeFwdDefaults.trellisTopN              =    0;
    treeFwdDefaults.pruneMethod              =    0;

    treeFwdDefaults.skip                     =    0;
    treeFwdDefaults.maxFramesRunon           = 2500;
    treeFwdDefaults.publishFrameN            =   20;
 
    for ( i = 0; i < MAX_MONOPHONES; i++) zeroA[i] = 0;

    if (! cframeA) {
      cframeAllocN =  CANDFRAME_ALLOCSIZE;
      cframeA      = (CandFrame*)malloc(cframeAllocN * sizeof(CandFrame));
    }

    itfNewType( &treeFwdInfo);
    tfwdInitialized = 1;
  }
  return TCL_OK;
}
