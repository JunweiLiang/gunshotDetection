/* ========================================================================
    JanusRTk   Janus Recognition Toolkit
               ------------------------------------------------------------
               Object: Language Model Word Mapper
               ------------------------------------------------------------

    Author  :  Michael Finke
    Module  :  lmodelMap.c
    Date    :  $Id: lmodelMap.c 700 2000-11-14 12:35:27Z janus $
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
    Revision 1.5  2000/11/14 12:32:23  janus
    Merging branch jtk-00-10-27-jmcd.

    Revision 1.4.26.2  2000/11/08 17:24:59  janus
    Finished making changes required for compilation under 'gcc -Wall'.

    Revision 1.4.26.1  2000/11/06 10:50:35  janus
    Made changes to enable compilation under -Wall.

    Revision 1.4  2000/09/14 17:47:12  janus
    Merging branch jtk-00-09-14-jmcd.

    Revision 1.3.30.1  2000/09/14 16:20:56  janus
    Merged branches jtk-00-09-10-tschaaf, jtk-00-09-12-hyu, and jtk-00-09-12-metze.

    Revision 1.3.28.1  2000/09/14 16:00:43  janus
    Incorporated bug fixes from Klaus Ries.

    Revision 1.3  2000/08/16 11:29:52  soltau
    Free -> TclFree

    Revision 1.2  1998/07/02 11:01:09  kries
    introduced lmzMap

 * Revision 1.1  1998/05/26  18:27:23  kries
 * Initial revision
 *

   ======================================================================== */

#include "common.h"
#include "itf.h"
#include "error.h"
#include "lm.h"
#include "lmodelMap.h"

char lmodelMapRCSVersion[]= 
           "@(#)1$Id: lmodelMap.c 700 2000-11-14 12:35:27Z janus $";


/* ------------------------------------------------------------------------
    Forward Declaration
   ------------------------------------------------------------------------ */

extern TypeInfo lmodelMapInfo;

/* ========================================================================
    LModelMapItem
   ======================================================================== */
/* ------------------------------------------------------------------------
    lmodelMapItemInit
   ------------------------------------------------------------------------ */

int lmodelMapItemInit( LModelMapItem* lmiP, ClientData CD)
{
  assert( lmiP);

  lmiP->name    =  ( CD) ? strdup((char*)CD) : NULL;
  lmiP->prob    =    0.0;
  return TCL_OK;
}

/* ------------------------------------------------------------------------
    lmodelMapItemDeinit
   ------------------------------------------------------------------------ */

int lmodelMapItemDeinit( LModelMapItem* lmiP)
{
  if (!lmiP) return TCL_OK;
  if ( lmiP->name)   { free(lmiP->name);   lmiP->name   = NULL; }
  iarrayDeinit(&lmiP->idxA);
  return TCL_OK;
}

/* ------------------------------------------------------------------------
    lmodelMapItemConfigureItf
   ------------------------------------------------------------------------ */

static int lmodelMapItemConfigureItf( ClientData cd, char *var, char *val)
{
  LModelMapItem* lmiP = (LModelMapItem*)cd;
  if (! lmiP) return TCL_ERROR;

  if (! var) {
    ITFCFG(lmodelMapItemConfigureItf,cd,"name");
    ITFCFG(lmodelMapItemConfigureItf,cd,"prob");
    return TCL_OK;
  }
  ITFCFG_CharPtr( var,val,"name",         lmiP->name,       1);
  ITFCFG_Float(   var,val,"prob",         lmiP->prob,       0);
  ERROR("Unknown option '-%s %s'.\n", var, val ? val : "?");
  return TCL_ERROR;
}

/* ------------------------------------------------------------------------
    lmodelMapItemPutsItf
   ------------------------------------------------------------------------ */

int lmodelMapItemPuts( LModelMapItem* itemP)
{
  int i;
  itfAppendElement("WORD");
  itfAppendElement("%s", itemP->name);
  itfAppendElement("IDX");
  itfAppendResult(" {");
  for ( i = 0; i < itemP->idxA.itemN; i++) {
    itfAppendElement("%d", itemP->idxA.itemA[i]);
  }
  itfAppendResult("}");
  itfAppendElement("PROB");
  itfAppendElement("%e", itemP->prob);
  return TCL_OK;
}

static int lmodelMapItemPutsItf( ClientData cd, int argc, char *argv[])
{
  int            ac    =  argc-1;
  LModelMapItem* itemP = (LModelMapItem*)cd;

  if ( itfParseArgv( argv[0], &ac, argv+1, 0, NULL) != TCL_OK)
       return TCL_ERROR;

  return lmodelMapItemPuts( itemP);
}

/* ========================================================================
    LModelMap
   ======================================================================== */

static LModelMap lmodelMapDefault;

/* ------------------------------------------------------------------------
    Create LModelMap Structure
   ------------------------------------------------------------------------ */

int lmodelMapInit( LModelMap* lmP, char* name, LMPtr lmPtr)
{
  assert( name);
  assert( lmP);

  lmP->name   = strdup(name);
  lmP->useN   = 0;

  if ( listInit((List*)&(lmP->list), itfGetType("LModelMapItem"), 
                                          sizeof(LModelMapItem),
                                 lmodelMapDefault.list.blkSize) != TCL_OK) {
    ERROR("Couldn't allocate mapping list.");
    return TCL_ERROR;
  }

  lmP->list.init    = (ListItemInit  *)lmodelMapItemInit;
  lmP->list.deinit  = (ListItemDeinit*)lmodelMapItemDeinit;

  lmP->lmPtr        =  lmPtr;
  lmP->lz           =  lmodelMapDefault.lz;
  lmP->lzMap        =  lmodelMapDefault.lzMap;
  lmP->lp           =  lmodelMapDefault.lp;
  lmP->commentChar  =  lmodelMapDefault.commentChar;

  return TCL_OK;
}

LModelMap* lmodelMapCreate( char* name, LMPtr lmPtr)
{
  LModelMap *lmP = (LModelMap*)malloc(sizeof(LModelMap));

  if (! lmP || lmodelMapInit( lmP, name, lmPtr) != TCL_OK) {
     if ( lmP) free( lmP);
     ERROR("Failed to allocate LModelMap object '%s'.\n", name);
     return NULL; 
  }
  return lmP;
}

static ClientData lmodelMapCreateItf (ClientData cd, int argc, char *argv[])
{
  int        ac  = argc-1;
  LMPtr      lmP;

  lmPtrInit( &lmP);

  if ( itfParseArgv(argv[0], &ac, argv+1, 0, 
      "<lm>",  ARGV_CUSTOM, getLMPtr, &lmP, NULL, "language model",
       NULL) != TCL_OK) return NULL;

  return (ClientData)lmodelMapCreate(argv[0], lmP);
}

/* ------------------------------------------------------------------------
    Free LModelMap Structure
   ------------------------------------------------------------------------ */

int lmodelMapLinkN( LModelMap* lmodel)
{
  int  useN = listLinkN((List*)&(lmodel->list)) - 1;
  if ( useN < lmodel->useN) return lmodel->useN;
  else                             return useN;
}

int lmodelMapDeinit (LModelMap *lmP)
{
  assert( lmP);

  if ( lmodelMapLinkN( lmP)) {
    SERROR("LModelMap '%s' still in use by other objects.\n", lmP->name);
    return TCL_ERROR;
  } 

  if (lmP->name)    { free(lmP->name);    lmP->name    = NULL; }

  return listDeinit((List*)&(lmP->list));
}

int lmodelMapFree (LModelMap *lmP)
{
  if (lmodelMapDeinit(lmP)!=TCL_OK) return TCL_ERROR;

  free(lmP);
  return TCL_OK;
}

static int lmodelMapFreeItf(ClientData cd)
{
  return lmodelMapFree((LModelMap*)cd);
}

/* ------------------------------------------------------------------------
    lmodelMapConfigureItf
   ------------------------------------------------------------------------ */

static int lmodelMapConfigureItf (ClientData cd, char *var, char *val)
{
  LModelMap* lmodel = (LModelMap*)cd;
  if (! lmodel) lmodel = &lmodelMapDefault;

  if (! var) {
    ITFCFG(lmodelMapConfigureItf,cd,"useN");
    ITFCFG(lmodelMapConfigureItf,cd,"lz");
    ITFCFG(lmodelMapConfigureItf,cd,"lzMap");
    ITFCFG(lmodelMapConfigureItf,cd,"lp");
    return listConfigureItf((ClientData)&(lmodel->list), var, val);
  }
  ITFCFG_Int(    var,val,"useN",   lmodel->useN,             1);
  ITFCFG_Float(  var,val,"lz",     lmodel->lz,               0);
  ITFCFG_Float(  var,val,"lzMap",  lmodel->lzMap,            0);
  ITFCFG_Float(  var,val,"lp",     lmodel->lp,               0);
  return listConfigureItf((ClientData)&(lmodel->list), var, val);   
}

/* ------------------------------------------------------------------------
    lmodelMapAccessItf
   ------------------------------------------------------------------------ */

static ClientData lmodelMapAccessItf (ClientData cd, char *name, TypeInfo **ti)
{ 
  LModelMap* lmodel = (LModelMap*)cd;
  return listAccessItf((ClientData)&(lmodel->list),name,ti);
}

/* ------------------------------------------------------------------------
    lmodelMapPutsItf
   ------------------------------------------------------------------------ */

static int lmodelMapPutsItf (ClientData cd, int argc, char *argv[])
{
  LModelMap *lmodel  = (LModelMap*)cd;
  return listPutsItf((ClientData)&(lmodel->list), argc, argv);
}

/* ------------------------------------------------------------------------
    lmodelMapIndex
   ------------------------------------------------------------------------ */

int lmodelMapIndex( LModelMap* lmodel,  char* name) 
{
  return listIndex((List*)&(lmodel->list), name, 0);
}

int lmodelMapIndexItf( ClientData clientData, int argc, char *argv[] )
{
  LModelMap* lmodel = (LModelMap*)clientData;
  return listName2IndexItf((ClientData)&(lmodel->list), argc, argv);
}

/* ------------------------------------------------------------------------
    lmodelMapNameN
   ------------------------------------------------------------------------ */

int lmodelMapNameN( LModelMap* lmodel) 
{
  return lmodel->list.itemN;
}


/* ------------------------------------------------------------------------
    lmodelMapName
   ------------------------------------------------------------------------ */

char* lmodelMapName( LModelMap* lmodel, int i) 
{
  return (i < 0) ? "(null)" : lmodel->list.itemA[i].name;
}

static int lmodelMapNameItf( ClientData clientData, int argc, char *argv[] )
{
  LModelMap* lmP = (LModelMap*)clientData;
  return listIndex2NameItf((ClientData)&(lmP->list), argc, argv);
}

/* ------------------------------------------------------------------------
    lmodelMapAdd
   ------------------------------------------------------------------------ */

int lmodelMapAdd( LModelMap* lmP, char* word, char* wlst, float prob)
{
  IArray  idxA;
  int     argc;
  char**  argv;

  if ( Tcl_SplitList( itf, wlst, &argc, &argv) == TCL_OK) {
    int i;
    int wordX;

    iarrayInit( &idxA, argc);

    for ( i = 0; i < argc; i++) {
      int idx = lmP->lmPtr.lmP->indexFct( lmP->lmPtr.cd, argv[i]);

      if ( idx < 0) {
        ERROR("Word \"%s\" not in LM.\n", argv[i]);
        iarrayDeinit( &idxA);
        Tcl_Free((char*)argv);
        return TCL_ERROR;
      }
      idxA.itemA[i] = idx;
    }
    Tcl_Free((char*)argv);

    /* ------------------------ */
    /* List of words is correct */
    /* ------------------------ */

    if ((wordX = lmodelMapIndex( lmP, word)) >= 0) {
      ERROR("Word '%s' already exists in '%s'.\n", word, lmP->name);
      iarrayDeinit( &idxA);
      return TCL_ERROR;
    }
    wordX = listNewItem((List*)&(lmP->list), (ClientData)word);

    lmP->list.itemA[wordX].idxA.itemA = idxA.itemA;
    lmP->list.itemA[wordX].idxA.itemN = idxA.itemN;
    lmP->list.itemA[wordX].prob       = prob;
    return TCL_OK;
  }
  return TCL_ERROR;
}

int lmodelMapAddItf (ClientData cd, int argc, char *argv[])
{
  LModelMap* lmP      = (LModelMap *)cd;
  int        ac       =  argc - 1;
  char*      word     =  NULL;
  char*      wlst     =  NULL;
  float      prob     =  0.0;

  if (itfParseArgv(argv[0], &ac, argv+1, 0, 
     "<word>",      ARGV_STRING, NULL, &word, cd, "word", 
     "<word list>", ARGV_STRING, NULL, &wlst, cd, "word list", 
     "-prob",       ARGV_FLOAT,  NULL, &prob, cd, "probability", 
       NULL) != TCL_OK) return TCL_ERROR;

  return lmodelMapAdd( lmP, word, wlst, prob);
}

/* ------------------------------------------------------------------------
    lmodelMapScore    compute the score of the 3G language model for a
                      sequence of n words.
   ------------------------------------------------------------------------ */

float lmodelMapScore( LModelMap* lmP, int* w, int n, int k)
{
  float p    = 0.0;
  float lp   = 0.0;
  int   koff = 0;
  int   len  = 0;
  int   v[100000];  
  int   i, j;

  for ( i =  0; i < n; i++) {
    if (i == k)   koff  = len;
    if (i >= k) { p    -= lmP->lz * lmP->lzMap * lmP->list.itemA[w[i]].prob;
                  lp   += lmP->lp *              lmP->list.itemA[w[i]].idxA.itemN;
                }
    for ( j = 0; j < lmP->list.itemA[w[i]].idxA.itemN; j++) {
       v[len++] = lmP->list.itemA[w[i]].idxA.itemA[j];
    }
    assert( len*sizeof(int) < sizeof(v));
  }
  return p + lmP->lz * lmP->lmPtr.lmP->scoreFct(lmP->lmPtr.cd, v, len, koff) + lp;
}

int lmodelMapScoreItf( ClientData cd, int argc, char * argv[])
{
  LModelMap*   lmP    = (LModelMap*) cd;
  IArray       warray = {NULL, 0};
  int          ac     =  argc-1;
  int          i      =  0;

  if (itfParseArgv( argv[0], &ac,  argv+1,  0,
     "<word sequence>", ARGV_LIARRAY, NULL, &warray, 
     &(lmP->list), "sequence of words", 
     "-idx", ARGV_INT, NULL, &i, NULL, "index of cond. prob.",
     NULL) != TCL_OK) {
    if ( warray.itemA) free( warray.itemA);
    return TCL_ERROR;
  }

  itfAppendElement( "%f", lmodelMapScore(lmP, warray.itemA, warray.itemN, i));

  if ( warray.itemA) free( warray.itemA);
  return TCL_OK;
} 

/* ------------------------------------------------------------------------
    lmodelMapNextFrame   update cache frame index
   ------------------------------------------------------------------------ */

int lmodelMapNextFrame( LModelMap* lmP)
{
  if (lmP->lmPtr.cd && lmP->lmPtr.lmP->nextFrameFct)
	lmP->lmPtr.lmP->nextFrameFct( lmP->lmPtr.cd);
  return TCL_OK;
}

int lmodelMapNextFrameItf( ClientData cd, int argc, char * argv[])
{
  LModelMap*   lmP    = (LModelMap *) cd;
  int          ac     =  argc-1;

  if (itfParseArgv( argv[0], &ac,  argv+1,  0,
       NULL) != TCL_OK) return TCL_ERROR;

  return lmodelMapNextFrame(lmP);
} 

/* ------------------------------------------------------------------------
    lmodelMapReset   reset language model cache
   ------------------------------------------------------------------------ */

int lmodelMapReset( LModelMap* lmP)
{
  if (lmP->lmPtr.cd && lmP->lmPtr.lmP->resetFct)
	  lmP->lmPtr.lmP->resetFct( lmP->lmPtr.cd);
  return TCL_OK;
}

int lmodelMapResetItf( ClientData cd, int argc, char * argv[])
{
  LModelMap*   lmP    = (LModelMap *) cd;
  int          ac     =  argc-1;

  if (itfParseArgv( argv[0], &ac,  argv+1,  0,
       NULL) != TCL_OK) return TCL_ERROR;

  return lmodelMapReset(lmP);
}
 
/* ------------------------------------------------------------------------
    lmodelMapRead
   ------------------------------------------------------------------------ */

int lmodelMapReadItf (ClientData cd, int argc, char *argv[])
{
  LModelMap*  lmP      = (LModelMap*)cd;
  int         ac       =  argc - 1;
  char*       fileName =  NULL;
  int         n;

  if (itfParseArgv(argv[0], &ac, argv+1, 0, 
     "<filename>", ARGV_STRING, NULL, &fileName, cd, "file to read from", 
       NULL) != TCL_OK) return TCL_ERROR;

  itfAppendResult( "%d", n = freadAdd( cd, fileName, lmP->commentChar, 
                                                     lmodelMapAddItf));
  return (n >= 0) ? TCL_OK : TCL_ERROR;
}

/* ========================================================================
    Type Declaration
   ======================================================================== */

static Method lmodelMapItemMethod[] = 
{ 
  { "puts", lmodelMapItemPutsItf,   NULL },
  {  NULL,  NULL, NULL } 
};

TypeInfo lmodelMapItemInfo = { 
        "LModelMapItem", 0, -1, lmodelMapItemMethod, 
         NULL, NULL,
         lmodelMapItemConfigureItf, NULL,
         itfTypeCntlDefaultNoLink, 
        "Language Model mapping\n" };

static Method lmodelMapMethod[] = 
{ 
  { "puts",   lmodelMapPutsItf,   "display the contents of an LModel" },
  { "add",    lmodelMapAddItf,    "add a mapping to LModelMap" },
  { "index",  lmodelMapIndexItf,  "return the internal index of an LModelItem" },
  { "name",   lmodelMapNameItf,   "return the name of an LModelItem" },
  { "score",  lmodelMapScoreItf,  "return the score of a text string"     },

  { "nextFrame", lmodelMapNextFrameItf,  "incr. cache frame index"  },
  { "reset",     lmodelMapResetItf,      "reset language model"     },

  { "read",      lmodelMapReadItf,       "read mapping from file"     },

  {  NULL,    NULL,           NULL } 
};

TypeInfo lmodelMapInfo = {
        "LModelMap", 0, -1, lmodelMapMethod, 
         lmodelMapCreateItf,lmodelMapFreeItf,
         lmodelMapConfigureItf,lmodelMapAccessItf, NULL,
	"Language Model Mapper\n" } ;

/* The model adds no state information --> just map down to compoment model */

LMState lmodelMapstateCreateFct(LModelMap * lmP) {
  if(lmP->lmPtr.cd && lmP->lmPtr.lmP->stateCreateFct)
    return lmP->lmPtr.lmP->stateCreateFct(lmP->lmPtr.cd);
  else
    return 0;
}

void lmodelMapstateFreeFct(LModelMap * lmP, LMState State) {
  if(lmP->lmPtr.cd && lmP->lmPtr.lmP->stateFreeFct)
    lmP->lmPtr.lmP->stateFreeFct(lmP->lmPtr.cd, State);
}

LMStateMem lmodelMapstateMemCreateFct(LModelMap * lmP, LMState State) {
  if(lmP->lmPtr.cd && lmP->lmPtr.lmP->stateMemCreateFct)
    return lmP->lmPtr.lmP->stateMemCreateFct(lmP->lmPtr.cd, State);
  else
    return 0;
}

void lmodelMapstateMemFreeFct(LModelMap * lmP, LMStateMem State) {
  if(lmP->lmPtr.cd && lmP->lmPtr.lmP->stateMemFreeFct)
    lmP->lmPtr.lmP->stateMemFreeFct(lmP->lmPtr.cd, State);
}

LMState lmodelMapsetStateFct(LModelMap * lmP, LMState State) {
  if (lmP->lmPtr.cd && lmP->lmPtr.lmP->setStateFct)
  	return lmP->lmPtr.lmP->setStateFct(lmP->lmPtr.cd, State);
  else
    	return 0;
}

void lmodelMapcopyStateFct( LModelMap * lmP, LMState toState,LMState fromState) {
  if (lmP->lmPtr.cd && lmP->lmPtr.lmP->copyStateFct)
	lmP->lmPtr.lmP->copyStateFct(lmP->lmPtr.cd, toState,fromState);
}

int lmodelMapfinalStateFct(LModelMap * lmP) {
  if(lmP->lmPtr.lmP->finalStateFct)
    return lmP->lmPtr.lmP->finalStateFct(lmP->lmPtr.cd);
  else
    return 1; /* Default is to go over to the next word (otherwise this loops forever) */
}

LMStateTransitionArray * lmodelMapgetNextStateFct(LModelMap * lmP, IArray * warray, int timeStamp) {
  IArray warrayMap;
  int   v[100000];  
  int   i, j;
  
  if (!lmP->lmPtr.cd || !lmP->lmPtr.lmP->getNextStateFct) return NULL;

  warrayMap.itemA = v;
  warrayMap.itemN=0;

  for(i=0;i<warray->itemN;i++) {
    for ( j = 0; j < lmP->list.itemA[warray->itemA[i]].idxA.itemN; j++) {
       warrayMap.itemA[warrayMap.itemN++] = lmP->list.itemA[warray->itemA[i]].idxA.itemA[j];
    }
    assert( warrayMap.itemN*sizeof(int) < sizeof(v));
  }
  return lmP->lmPtr.lmP->getNextStateFct(lmP->lmPtr.cd, &warrayMap, timeStamp);
}

int lmodelMapstatePrintFct(LModelMap * lmP, LMState State, char * printString) {
  if (lmP->lmPtr.cd && lmP->lmPtr.lmP->statePrintFct)
	return lmP->lmPtr.lmP->statePrintFct(lmP->lmPtr.cd, State, printString);
  else
    	return 0;
}
int lmodelMapstateMemPrintFct(LModelMap * lmP, LMStateMem State, char * printString) {
  if (lmP->lmPtr.cd && lmP->lmPtr.lmP->stateMemPrintFct)
	return lmP->lmPtr.lmP->stateMemPrintFct(lmP->lmPtr.cd, State, printString);
  else
	return 0;
}



int lmodelMapstateEqFct(LModelMap * lmP, LMState State1, LMState State2) {
  if (lmP->lmPtr.cd && lmP->lmPtr.lmP->stateEqFct)
	return lmP->lmPtr.lmP->stateEqFct(lmP->lmPtr.cd, State1, State2);
  else
	return 0;
}

long lmodelMapstateHashFct(LModelMap * lmP, LMState State) {
  if (lmP->lmPtr.cd && lmP->lmPtr.lmP->stateHashFct)
	return lmP->lmPtr.lmP->stateHashFct(lmP->lmPtr.cd, State);
  else
    	return 0;
}

void lmodelMapPathCreateFct(LModelMap * lmP, AStar * searchObjectP, AStarNode* nodeP) {
  if (lmP->lmPtr.cd && lmP->lmPtr.lmP->pathCreateFct)
	lmP->lmPtr.lmP->pathCreateFct(lmP->lmPtr.cd, searchObjectP, nodeP);
}

static LM lmodelMapLM = 
       { "LModelMap", &lmodelMapInfo, &lmodelMapInfo,
         (LMIndexFct*)    lmodelMapIndex,
         (LMNameFct *)    lmodelMapName,
         (LMNameNFct *)   lmodelMapNameN,
         (LMScoreFct*)    lmodelMapScore,
         (LMNextFrameFct*)lmodelMapNextFrame,
         (LMResetFct*)    lmodelMapReset,
	 (LMStateCreateFct*)  lmodelMapstateCreateFct,
	 (LMStateFreeFct*)  lmodelMapstateFreeFct,
	 (LMStatePrintFct*)  lmodelMapstatePrintFct,
	 (LMSetStateFct*)    lmodelMapsetStateFct,
	 (LMCopyStateFct*)   lmodelMapcopyStateFct,
	 (LMFinalStateFct*)  lmodelMapfinalStateFct,
	 (LMGetNextStateFct*)lmodelMapgetNextStateFct,
	 (LMStateEqFct*)     lmodelMapstateEqFct,
	 (LMStateHashFct*)   lmodelMapstateHashFct,
	 (LMStateMemCreateFct*)  lmodelMapstateMemCreateFct,
	 (LMStateMemFreeFct*)  lmodelMapstateMemFreeFct,
	 (LMStateMemPrintFct*)  lmodelMapstateMemPrintFct,
	 (LMpathCreateFct*)  lmodelMapPathCreateFct
       };


int LModelMap_Init( )
{
  static int lmodelMapInitialized = 0;

  if (! lmodelMapInitialized) {

    lmodelMapDefault.name            = NULL;
    lmodelMapDefault.useN            = 0;
    lmodelMapDefault.list.itemN      = 0;
    lmodelMapDefault.list.blkSize    = 1000;
    lmodelMapDefault.lz              = 1.0;
    lmodelMapDefault.lzMap           = 1.0;
    lmodelMapDefault.lp              = 0.0;
    lmodelMapDefault.commentChar     = ';';

    itfNewType (&lmodelMapItemInfo);
    itfNewType (&lmodelMapInfo);
    lmNewType(  &lmodelMapLM);

    lmodelMapInitialized = 1;
  }
  return TCL_OK;
}
