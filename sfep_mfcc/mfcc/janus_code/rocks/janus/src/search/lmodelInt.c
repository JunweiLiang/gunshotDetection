/* ========================================================================
    JanusRTk   Janus Recognition Toolkit
               ------------------------------------------------------------
               Object: Language Model Interpolation
               ------------------------------------------------------------

    Author  :  Michael Finke
    Module  :  lmodelInt.c
    Date    :  $Id: lmodelInt.c 700 2000-11-14 12:35:27Z janus $
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
    Revision 1.3  2000/11/14 12:32:23  janus
    Merging branch jtk-00-10-27-jmcd.

    Revision 1.2.30.2  2000/11/08 17:24:26  janus
    Finished making changes required for compilation under 'gcc -Wall'.

    Revision 1.2.30.1  2000/11/06 10:50:34  janus
    Made changes to enable compilation under -Wall.

    Revision 1.2  1998/08/14 12:35:25  kries
    corrected interpolation (bug in swapping)

    Revision 1.1  1998/05/26 18:27:23  kries
    Initial revision


   ======================================================================== */

#include "common.h"
#include "itf.h"
#include "error.h"
#include "lm.h"
#include "lmodelInt.h"

char lmodelIntRCSVersion[]= 
           "@(#)1$Id: lmodelInt.c 700 2000-11-14 12:35:27Z janus $";


/* ------------------------------------------------------------------------
    Forward Declaration
   ------------------------------------------------------------------------ */

extern TypeInfo lmodelIntInfo;

/* ========================================================================
    LModelIntItem
   ======================================================================== */
/* ------------------------------------------------------------------------
    lmodelIntItemInit
   ------------------------------------------------------------------------ */

int lmodelIntItemInit( LModelIntItem* lmiP, ClientData CD)
{
  assert( lmiP);

  lmiP->name    =  ( CD) ? strdup((char*)CD) : NULL;
  lmiP->prob    =    0.0;
  lmiP->idxA    =   -1;
  lmiP->idxB    =   -1;
  return TCL_OK;
}

/* ------------------------------------------------------------------------
    lmodelIntItemDeinit
   ------------------------------------------------------------------------ */

int lmodelIntItemDeinit( LModelIntItem* lmiP)
{
  if (!lmiP) return TCL_OK;
  if ( lmiP->name)   { free(lmiP->name);   lmiP->name   = NULL; }
  return TCL_OK;
}

/* ------------------------------------------------------------------------
    lmodelIntItemConfigureItf
   ------------------------------------------------------------------------ */

static int lmodelIntItemConfigureItf( ClientData cd, char *var, char *val)
{
  LModelIntItem* lmiP = (LModelIntItem*)cd;
  if (! lmiP) return TCL_ERROR;

  if (! var) {
    ITFCFG(lmodelIntItemConfigureItf,cd,"name");
    ITFCFG(lmodelIntItemConfigureItf,cd,"idxA");
    ITFCFG(lmodelIntItemConfigureItf,cd,"idxB");
    ITFCFG(lmodelIntItemConfigureItf,cd,"prob");
    return TCL_OK;
  }
  ITFCFG_CharPtr( var,val,"name",         lmiP->name,       1);
  ITFCFG_Float(   var,val,"prob",         lmiP->prob,       0);
  ITFCFG_Int(     var,val,"idxA",         lmiP->idxA,       0);
  ITFCFG_Int(     var,val,"idxB",         lmiP->idxB,       0);
  ERROR("Unknown option '-%s %s'.\n", var, val ? val : "?");
  return TCL_ERROR;
}

/* ------------------------------------------------------------------------
    lmodelIntItemPutsItf
   ------------------------------------------------------------------------ */

int lmodelIntItemPuts( LModelIntItem* itemP)
{
  itfAppendElement("WORD");
  itfAppendElement("%s", itemP->name);
  itfAppendElement("IDX");
  itfAppendElement("%d %d", itemP->idxA, itemP->idxB);
  itfAppendElement("PROB");
  itfAppendElement("%e", itemP->prob);
  return TCL_OK;
}

static int lmodelIntItemPutsItf( ClientData cd, int argc, char *argv[])
{
  int            ac    =  argc-1;
  LModelIntItem* itemP = (LModelIntItem*)cd;

  if ( itfParseArgv( argv[0], &ac, argv+1, 0, NULL) != TCL_OK)
       return TCL_ERROR;

  return lmodelIntItemPuts( itemP);
}

/* ========================================================================
    LModelInt
   ======================================================================== */

static LModelInt lmodelIntDefault;

/* ------------------------------------------------------------------------
    Create LModelInt Structure
   ------------------------------------------------------------------------ */

int lmodelIntInit( LModelInt* lmP, char* name, LMPtr lmPtrA, LMPtr lmPtrB)
{
  assert( name);
  assert( lmP);

  lmP->name   = strdup(name);
  lmP->useN   = 0;

  if ( listInit((List*)&(lmP->list), itfGetType("LModelIntItem"), 
                                          sizeof(LModelIntItem),
                                 lmodelIntDefault.list.blkSize) != TCL_OK) {
    ERROR("Couldn't allocate mapping list.");
    return TCL_ERROR;
  }

  lmP->list.init    = (ListItemInit  *)lmodelIntItemInit;
  lmP->list.deinit  = (ListItemDeinit*)lmodelIntItemDeinit;

  lmP->lmPtrA       =  lmPtrA;
  lmP->lmPtrB       =  lmPtrB;
  lmP->lz           =  lmodelIntDefault.lz;
  lmP->lp           =  lmodelIntDefault.lp;
  lmP->unkProb      =  lmodelIntDefault.unkProb;
  lmP->commentChar  =  lmodelIntDefault.commentChar;

  return TCL_OK;
}

LModelInt* lmodelIntCreate( char* name, LMPtr lmPtrA, LMPtr lmPtrB)
{
  LModelInt *lmP = (LModelInt*)malloc(sizeof(LModelInt));

  if (! lmP || lmodelIntInit( lmP, name, lmPtrA, lmPtrB) != TCL_OK) {
     if ( lmP) free( lmP);
     ERROR("Failed to allocate LModelInt object '%s'.\n", name);
     return NULL; 
  }
  return lmP;
}

static ClientData lmodelIntCreateItf (ClientData cd, int argc, char *argv[])
{
  int        ac  = argc-1;
  LMPtr      lmPA;
  LMPtr      lmPB;
  LModelInt* lmmP;

  lmPtrInit( &lmPA);
  lmPtrInit( &lmPB);

  if ( itfParseArgv(argv[0], &ac, argv+1, 0, 
      "<lmA>",  ARGV_CUSTOM, getLMPtr, &lmPA, NULL, "language model A",
      "<lmB>",  ARGV_CUSTOM, getLMPtr, &lmPB, NULL, "language model B",
       NULL) != TCL_OK) return NULL;

  if ( (lmmP     = lmodelIntCreate(argv[0], lmPA, lmPB)) ) {
  }
  return (ClientData)lmmP;
}

/* ------------------------------------------------------------------------
    Free LModelInt Structure
   ------------------------------------------------------------------------ */

int lmodelIntLinkN( LModelInt* lmodel)
{
  int  useN = listLinkN((List*)&(lmodel->list)) - 1;
  if ( useN < lmodel->useN) return lmodel->useN;
  else                             return useN;
}

int lmodelIntDeinit (LModelInt *lmP)
{
  assert( lmP);

  if ( lmodelIntLinkN( lmP)) {
    SERROR("LModelInt '%s' still in use by other objects.\n", lmP->name);
    return TCL_ERROR;
  } 

  if (lmP->name)    { free(lmP->name);    lmP->name    = NULL; }

  return listDeinit((List*)&(lmP->list));
}

int lmodelIntFree (LModelInt *lmP)
{
  if (lmodelIntDeinit(lmP)!=TCL_OK) return TCL_ERROR;

  free(lmP);
  return TCL_OK;
}

static int lmodelIntFreeItf(ClientData cd)
{
  return lmodelIntFree((LModelInt*)cd);
}

/* ------------------------------------------------------------------------
    lmodelIntConfigureItf
   ------------------------------------------------------------------------ */

static int lmodelIntConfigureItf (ClientData cd, char *var, char *val)
{
  LModelInt* lmodel = (LModelInt*)cd;
  if (! lmodel) lmodel = &lmodelIntDefault;

  if (! var) {
    ITFCFG(lmodelIntConfigureItf,cd,"useN");
    ITFCFG(lmodelIntConfigureItf,cd,"lz");
    ITFCFG(lmodelIntConfigureItf,cd,"lp");
    ITFCFG(lmodelIntConfigureItf,cd,"unkProb");
    return listConfigureItf((ClientData)&(lmodel->list), var, val);
  }
  ITFCFG_Int(    var,val,"useN",    lmodel->useN,     1);
  ITFCFG_Float(  var,val,"lz",      lmodel->lz,       0);
  ITFCFG_Float(  var,val,"lp",      lmodel->lp,       0);
  ITFCFG_Float(  var,val,"unkProb", lmodel->unkProb,  0);
  return listConfigureItf((ClientData)&(lmodel->list), var, val);   
}

/* ------------------------------------------------------------------------
    lmodelIntAccessItf
   ------------------------------------------------------------------------ */

static ClientData lmodelIntAccessItf (ClientData cd, char *name, TypeInfo **ti)
{ 
  LModelInt* lmodel = (LModelInt*)cd;
  return listAccessItf((ClientData)&(lmodel->list),name,ti);
}

/* ------------------------------------------------------------------------
    lmodelIntPutsItf
   ------------------------------------------------------------------------ */

static int lmodelIntPutsItf (ClientData cd, int argc, char *argv[])
{
  LModelInt *lmodel  = (LModelInt*)cd;
  return listPutsItf((ClientData)&(lmodel->list), argc, argv);
}

/* ------------------------------------------------------------------------
    lmodelIntIndex
   ------------------------------------------------------------------------ */

int lmodelIntIndex( LModelInt* lmodel,  char* name) 
{
  return listIndex((List*)&(lmodel->list), name, 0);
}

int lmodelIntIndexItf( ClientData clientData, int argc, char *argv[] )
{
  LModelInt* lmodel = (LModelInt*)clientData;
  return listName2IndexItf((ClientData)&(lmodel->list), argc, argv);
}

/* ------------------------------------------------------------------------
    lmodelIntNameN
   ------------------------------------------------------------------------ */

int lmodelIntNameN( LModelInt* lmodel) 
{
  return lmodel->list.itemN;
}


/* ------------------------------------------------------------------------
    lmodelIntName
   ------------------------------------------------------------------------ */

char* lmodelIntName( LModelInt* lmodel, int i) 
{
  return (i < 0) ? "(null)" : lmodel->list.itemA[i].name;
}

static int lmodelIntNameItf( ClientData clientData, int argc, char *argv[] )
{
  LModelInt* lmP = (LModelInt*)clientData;
  return listIndex2NameItf((ClientData)&(lmP->list), argc, argv);
}

/* ------------------------------------------------------------------------
    lmodelIntAdd
   ------------------------------------------------------------------------ */

int lmodelIntAdd( LModelInt* lmP, char* word, float prob)
{
  int     idx;
  int     idxA = lmP->lmPtrA.lmP->indexFct( lmP->lmPtrA.cd, word);
  int     idxB = lmP->lmPtrB.lmP->indexFct( lmP->lmPtrB.cd, word);

  if ( idxA < 0) {
    ERROR("Word '%s' is not in language model A.\n", word);
    return TCL_ERROR;
  }
  if ( idxB < 0) {
    ERROR("Word '%s' is not in language model B.\n", word);
    return TCL_ERROR;
  }

  if ((idx = lmodelIntIndex( lmP, word)) >= 0) {
    ERROR("Word '%s' already exists in '%s'.\n", word, lmP->name);
    return TCL_ERROR;
  }
  idx = listNewItem((List*)&(lmP->list), (ClientData)word);

  lmP->list.itemA[idx].idxA       = idxA;
  lmP->list.itemA[idx].idxB       = idxB;
  lmP->list.itemA[idx].prob       = prob;

  if(! strcmp( word, "<UNK>")) { lmP->unkProb = prob; };

  return TCL_OK;
}

int lmodelIntAddItf (ClientData cd, int argc, char *argv[])
{
  LModelInt* lmP      = (LModelInt *)cd;
  int        ac       =  argc - 1;
  char*      word     =  NULL;
  float      prob     =  0.0;

  if (itfParseArgv(argv[0], &ac, argv+1, 0, 
     "<word>",      ARGV_STRING, NULL, &word,  cd, "word", 
     "<prob>",      ARGV_FLOAT,  NULL, &prob,  cd, "probability",
       NULL) != TCL_OK) return TCL_ERROR;

  return lmodelIntAdd( lmP, word, prob);
}

/* ------------------------------------------------------------------------
    lmodelIntScore    compute the score of the 3G language model for a
                      sequence of n words.
   ------------------------------------------------------------------------ */

float lmodelIntScore( LModelInt* lmP, int* w, int n, int k)
{
  double p     = 0.0;
  int    u[1000];
  int    v[1000];
  int    j;

  if(n*sizeof(int) >= sizeof(u)) {
    ERROR("n=%ld\n");
    assert( n*sizeof(int) < sizeof(u));
    assert( n*sizeof(int) < sizeof(v));
  }
  /* ---------------------- */
  /* Translate word indices */
  /* ---------------------- */

  for ( j =  0; j < n; j++) {
    u[j] = lmP->list.itemA[w[j]].idxA;
    v[j] = lmP->list.itemA[w[j]].idxB;
  }

  /* ----------------------------- */
  /* Compute interpolated LM probs */
  /* ----------------------------- */

  for ( j = k; j < n; j++) {
    double sA    = -lmP->lmPtrA.lmP->scoreFct( lmP->lmPtrA.cd, u, j+1, j);
    double sB    = -lmP->lmPtrB.lmP->scoreFct( lmP->lmPtrB.cd, v, j+1, j);
    double alpha = (j  > 0)  ? lmP->list.itemA[w[j-1]].prob : lmP->unkProb;

    /* Ensure sa>=sB */
    if(sA<sB) { float t = sA; sA = sB; sB=t; alpha = 1-alpha; }

    p +=  log(    alpha  + (1.0-alpha) * exp(log(10.0)*(sB - sA))) / log(10.0) + sA;
  }

  return -lmP->lz * p + (n-k) * lmP->lp;;
}

int lmodelIntScoreItf( ClientData cd, int argc, char * argv[])
{
  LModelInt*   lmP    = (LModelInt*) cd;
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

  itfAppendElement( "%f", lmodelIntScore(lmP, warray.itemA, warray.itemN, i));

  if ( warray.itemA) free( warray.itemA);
  return TCL_OK;
} 

/* ------------------------------------------------------------------------
    lmodelIntNextFrame   update cache frame index
   ------------------------------------------------------------------------ */

int lmodelIntNextFrame( LModelInt* lmP)
{
  lmP->lmPtrA.lmP->nextFrameFct( lmP->lmPtrA.cd);
  lmP->lmPtrB.lmP->nextFrameFct( lmP->lmPtrB.cd);
  return TCL_OK;
}

int lmodelIntNextFrameItf( ClientData cd, int argc, char * argv[])
{
  LModelInt*   lmP    = (LModelInt *) cd;
  int          ac     =  argc-1;

  if (itfParseArgv( argv[0], &ac,  argv+1,  0,
       NULL) != TCL_OK) return TCL_ERROR;

  return lmodelIntNextFrame(lmP);
} 

/* ------------------------------------------------------------------------
    lmodelIntReset   reset language model cache
   ------------------------------------------------------------------------ */

int lmodelIntReset( LModelInt* lmP)
{
  lmP->lmPtrA.lmP->resetFct( lmP->lmPtrA.cd);
  lmP->lmPtrB.lmP->resetFct( lmP->lmPtrB.cd);
  return TCL_OK;
}

int lmodelIntResetItf( ClientData cd, int argc, char * argv[])
{
  LModelInt*   lmP    = (LModelInt *) cd;
  int          ac     =  argc-1;

  if (itfParseArgv( argv[0], &ac,  argv+1,  0,
       NULL) != TCL_OK) return TCL_ERROR;

  return lmodelIntReset(lmP);
}
 
/* ------------------------------------------------------------------------
    lmodelIntRead
   ------------------------------------------------------------------------ */

int lmodelIntReadItf (ClientData cd, int argc, char *argv[])
{
  LModelInt*  lmP      = (LModelInt*)cd;
  int         ac       =  argc - 1;
  char*       fileName =  NULL;
  int         n;

  if (itfParseArgv(argv[0], &ac, argv+1, 0, 
     "<filename>", ARGV_STRING, NULL, &fileName, cd, "file to read from", 
       NULL) != TCL_OK) return TCL_ERROR;

  itfAppendResult( "%d", n = freadAdd( cd, fileName, lmP->commentChar, 
                                                     lmodelIntAddItf));
  return (n >= 0) ? TCL_OK : TCL_ERROR;
}

/* ========================================================================
    Type Declaration
   ======================================================================== */

static Method lmodelIntItemMethod[] = 
{ 
  { "puts", lmodelIntItemPutsItf,   NULL },
  {  NULL,  NULL, NULL } 
};

TypeInfo lmodelIntItemInfo = { 
        "LModelIntItem", 0, -1, lmodelIntItemMethod, 
         NULL, NULL,
         lmodelIntItemConfigureItf, NULL,
         itfTypeCntlDefaultNoLink, 
        "Language Model interpolation\n" };

static Method lmodelIntMethod[] = 
{ 
  { "puts",   lmodelIntPutsItf,   "display the contents of an LModel" },
  { "add",    lmodelIntAddItf,    "add a mapping to LModelInt" },
  { "index",  lmodelIntIndexItf,  "return the internal index of an LModelItem" },
  { "name",   lmodelIntNameItf,   "return the name of an LModelItem" },
  { "score",  lmodelIntScoreItf,  "return the score of a text string"     },

  { "nextFrame", lmodelIntNextFrameItf,  "incr. cache frame index"  },
  { "reset",     lmodelIntResetItf,      "reset language model"     },

  { "read",      lmodelIntReadItf,       "read mapping from file"     },

  {  NULL,    NULL,           NULL } 
};

TypeInfo lmodelIntInfo = {
        "LModelInt", 0, -1, lmodelIntMethod, 
         lmodelIntCreateItf,lmodelIntFreeItf,
         lmodelIntConfigureItf,lmodelIntAccessItf, NULL,
	"Language Model Intper\n" } ;

static LM lmodelIntLM = 
       { "LModelInt", &lmodelIntInfo, &lmodelIntInfo,
         (LMIndexFct*)    lmodelIntIndex,
         (LMNameFct *)    lmodelIntName,
         (LMNameNFct *)   lmodelIntNameN,
         (LMScoreFct*)    lmodelIntScore,
         (LMNextFrameFct*)lmodelIntNextFrame,
         (LMResetFct*)    lmodelIntReset,
       NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL };

int LModelInt_Init( )
{
  static int lmodelIntInitialized = 0;

  if (! lmodelIntInitialized) {

    lmodelIntDefault.name            = NULL;
    lmodelIntDefault.useN            = 0;
    lmodelIntDefault.list.itemN      = 0;
    lmodelIntDefault.list.blkSize    = 1000;
    lmodelIntDefault.lz              = 1.0;
    lmodelIntDefault.lp              = 0.0;
    lmodelIntDefault.unkProb         = 0.5;
    lmodelIntDefault.commentChar     = ';';

    itfNewType (&lmodelIntItemInfo);
    itfNewType (&lmodelIntInfo);
    lmNewType(  &lmodelIntLM);

    lmodelIntInitialized = 1;
  }
  return TCL_OK;
}
