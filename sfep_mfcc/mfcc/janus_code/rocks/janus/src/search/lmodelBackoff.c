/* ========================================================================
    JanusRTk   Janus Recognition Toolkit
               ------------------------------------------------------------
               Object: Backoff with intermediate classes
               ------------------------------------------------------------

    Author  :  Klaus Ries
    Module  :  lmodelBackoff.c
    Date    :  $Id: lmodelBackoff.c 700 2000-11-14 12:35:27Z janus $
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
    Revision 1.4  2000/11/14 12:29:35  janus
    Merging branch jtk-00-10-27-jmcd.

    Revision 1.3.30.2  2000/11/08 17:22:53  janus
    Finished making changes required for compilation under 'gcc -Wall'.

    Revision 1.3.30.1  2000/11/06 10:50:34  janus
    Made changes to enable compilation under -Wall.

    Revision 1.3  1998/08/14 12:38:58  kries
    *** empty log message ***

    Revision 1.2  1998/08/02 10:58:15  kries
    changed modeling

 * Revision 1.1  1998/06/30  00:34:09  kries
 * Initial revision
 *
 * Revision 1.1  1998/06/11  14:36:09  kries
 * Initial revision
 *

   ======================================================================== */

#include "common.h"
#include "itf.h"
#include "error.h"
#include "lm.h"
#include "lmodelBackoff.h"
#include <math.h>

char lmodelBackoffRCSVersion[]= 
           "@(#)1$Id: lmodelBackoff.c 700 2000-11-14 12:35:27Z janus $";


/* ------------------------------------------------------------------------
    Forward Declaration
   ------------------------------------------------------------------------ */

extern TypeInfo lmodelBackoffInfo;

/* ========================================================================
    LModelBackoffItem
   ======================================================================== */
/* ------------------------------------------------------------------------
    LModelBackoffItemInit
   ------------------------------------------------------------------------ */

int LModelBackoffItemInit( LModelBackoffItem* lmiP, ClientData CD)
{
  assert( lmiP);

  lmiP->name    =  ( CD) ? strdup((char*)CD) : NULL;
  lmiP->wordIdxToWord    =   -1;
  lmiP->classIdxToWord   =   -1;
  lmiP->wordIdxToClass   =   -1;
  lmiP->classIdxToClass  =   -1;
  return TCL_OK;
}

/* ------------------------------------------------------------------------
    LModelBackoffItemDeinit
   ------------------------------------------------------------------------ */

int LModelBackoffItemDeinit( LModelBackoffItem* lmiP)
{
  if (!lmiP) return TCL_OK;
  if ( lmiP->name)   { free(lmiP->name);   lmiP->name   = NULL; }
  return TCL_OK;
}

/* ------------------------------------------------------------------------
    LModelBackoffItemConfigureItf
   ------------------------------------------------------------------------ */

static int LModelBackoffItemConfigureItf( ClientData cd, char *var, char *val)
{
  LModelBackoffItem* lmiP = (LModelBackoffItem*)cd;
  if (! lmiP) return TCL_ERROR;

  if (! var) {
    ITFCFG(LModelBackoffItemConfigureItf,cd,"name");
    ITFCFG(LModelBackoffItemConfigureItf,cd,"wordIdxToClass");
    ITFCFG(LModelBackoffItemConfigureItf,cd,"classIdxToClass");
    ITFCFG(LModelBackoffItemConfigureItf,cd,"wordIdxToWord");
    ITFCFG(LModelBackoffItemConfigureItf,cd,"classIdxToWord");
    return TCL_OK;
  }
  ITFCFG_CharPtr( var,val,"name",         lmiP->name,       1);
  ITFCFG_Int(     var,val,"wordIdxToClass",  lmiP->wordIdxToClass, 1);
  ITFCFG_Int(     var,val,"classIdxToClass", lmiP->wordIdxToClass, 1);
  ITFCFG_Int(     var,val,"wordIdxToWord",   lmiP->wordIdxToWord,  1);
  ITFCFG_Int(     var,val,"classIdxToWord",  lmiP->wordIdxToWord,  1);
  ERROR("Unknown option '-%s %s'.\n", var, val ? val : "?");
  return TCL_ERROR;
}

/* ------------------------------------------------------------------------
    LModelBackoffItemPutsItf
   ------------------------------------------------------------------------ */

int LModelBackoffItemPuts( LModelBackoffItem* itemP)
{
  itfAppendElement("WORD");
  itfAppendElement("%s", itemP->name);
  itfAppendElement("WORDIDXTOWORD");
  itfAppendElement("%d", itemP->wordIdxToWord);
  itfAppendElement("CLASSIDXTOWORD");
  itfAppendElement("%d", itemP->classIdxToWord);
  itfAppendElement("WORDIDXTOCLASS");
  itfAppendElement("%d", itemP->wordIdxToClass);
  itfAppendElement("CLASSIDXTOCLASS");
  itfAppendElement("%d", itemP->classIdxToClass);
  return TCL_OK;
}

static int LModelBackoffItemPutsItf( ClientData cd, int argc, char *argv[])
{
  int            ac    =  argc-1;
  LModelBackoffItem* itemP = (LModelBackoffItem*)cd;

  if ( itfParseArgv( argv[0], &ac, argv+1, 0, NULL) != TCL_OK)
       return TCL_ERROR;

  return LModelBackoffItemPuts( itemP);
}


/* ========================================================================
    LModelBackoff
   ======================================================================== */

static LModelBackoff LModelBackoffDefault;

/* ------------------------------------------------------------------------
    Create LModelBackoff Structure
   ------------------------------------------------------------------------ */

int LModelBackoffInit( LModelBackoff* lmP, char* name, LMPtr * toWordLmP, LMPtr * toClassLmP)
{
  assert( name);
  assert( lmP );

  lmP->name        = strdup(name);
  lmP->useN        = 0;

  lmP->toWordLmP.lmP = toWordLmP->lmP;
  lmP->toWordLmP.cd  = toWordLmP->cd;

  lmP->toClassLmP.lmP = toClassLmP->lmP;
  lmP->toClassLmP.cd  = toClassLmP->cd;

  if ( listInit((List*)&(lmP->w), itfGetType("LModelBackoffItem"), 
       sizeof(LModelBackoffItem),LModelBackoffDefault.w.blkSize) != TCL_OK) {
    ERROR("Couldn't allocate word mapping list.\n");
    return TCL_ERROR;
  }
  lmP->w.init      = (ListItemInit  *)LModelBackoffItemInit;
  lmP->w.deinit    = (ListItemDeinit*)LModelBackoffItemDeinit;

  lmP->lz          = LModelBackoffDefault.lz;
  lmP->lp          = LModelBackoffDefault.lp;
  lmP->commentChar = LModelBackoffDefault.commentChar;

  return TCL_OK;
}

LModelBackoff* LModelBackoffCreate( char* name, LMPtr * toWordLmP, LMPtr * toClassLmP)
{
  LModelBackoff *lmP = (LModelBackoff*)malloc(sizeof(LModelBackoff));

  if (! lmP || LModelBackoffInit( lmP, name, toWordLmP, toClassLmP) != TCL_OK) {
     if ( lmP) free( lmP);
     ERROR("Failed to allocate LModelBackoff object '%s'.\n", name);
     return NULL; 
  }
  return lmP;
}

static ClientData LModelBackoffCreateItf (ClientData cd, int argc, char *argv[])
{
  int        ac  = argc-1;
  LMPtr      toWordLmP;
  LMPtr      toClassLmP;
  LModelBackoff* lmmP;

  lmPtrInit( &toWordLmP);
  lmPtrInit( &toClassLmP);

  if ( itfParseArgv(argv[0], &ac, argv+1, 0, 
      "<lmToWord>",  ARGV_CUSTOM, getLMPtr, &toWordLmP,  NULL, "sub language model onto word",
      "<lmToClass>", ARGV_CUSTOM, getLMPtr, &toClassLmP, NULL, "sub language model onto class",
       NULL) != TCL_OK) return NULL;

  if (!( lmmP     = LModelBackoffCreate(argv[0], &toWordLmP, &toClassLmP))) {
    SERROR("Cannot create backoff model\n"); return NULL;
  };
  return (ClientData)lmmP;
}


/* ------------------------------------------------------------------------
    Free LModelBackoff Structure
   ------------------------------------------------------------------------ */

int LModelBackoffLinkN( LModelBackoff* lmodel)
{
  int  useN = listLinkN((List*)&(lmodel->w)) - 1;
  if ( useN < lmodel->useN) return lmodel->useN;
  else                      return useN;
}

int LModelBackoffDeinit (LModelBackoff *lmP)
{
  assert( lmP);

  if ( LModelBackoffLinkN(lmP)) {
    SERROR("LModelBackoff '%s' still in use by other objects.\n", lmP->name);
    return TCL_ERROR;
  } 

  if (lmP->name)    { free(lmP->name);    lmP->name    = NULL; }
  if (listDeinit((List*)&(lmP->w))  == TCL_ERROR) return TCL_ERROR;
  lmPtrDeinit(&(lmP->toWordLmP));
  lmPtrDeinit(&(lmP->toClassLmP));

  return TCL_ERROR;
}

int LModelBackoffFree (LModelBackoff *lmP)
{
  if (LModelBackoffDeinit(lmP)!=TCL_OK) return TCL_ERROR;

  free(lmP);
  return TCL_OK;
}

static int LModelBackoffFreeItf(ClientData cd)
{
  return LModelBackoffFree((LModelBackoff*)cd);
}

/* ------------------------------------------------------------------------
    LModelBackoffConfigureItf
   ------------------------------------------------------------------------ */

static int LModelBackoffConfigureItf (ClientData cd, char *var, char *val)
{
  LModelBackoff* lmodel = (LModelBackoff*)cd;
  if (! lmodel) lmodel = &LModelBackoffDefault;

  if (! var) {
    ITFCFG(LModelBackoffConfigureItf,cd,"useN");
    ITFCFG(LModelBackoffConfigureItf,cd,"lz");
    ITFCFG(LModelBackoffConfigureItf,cd,"lp");
    return listConfigureItf((ClientData)&(lmodel->w), var, val);
  }
  ITFCFG_Int(    var,val,"useN",    lmodel->useN,     1);
  ITFCFG_Float(  var,val,"lz",      lmodel->lz,       0);
  ITFCFG_Float(  var,val,"lp",      lmodel->lp,       0);
  return listConfigureItf((ClientData)&(lmodel->w), var, val);   
}

/* ------------------------------------------------------------------------
    LModelBackoffAccessItf
   ------------------------------------------------------------------------ */

static ClientData LModelBackoffAccessItf (ClientData cd, char *name, TypeInfo **ti)
{ 
  LModelBackoff* lmodel = (LModelBackoff*)cd;
  return listAccessItf((ClientData)&(lmodel->w),name,ti);
}

/* ------------------------------------------------------------------------
    LModelBackoffPutsItf
   ------------------------------------------------------------------------ */

static int LModelBackoffPutsItf (ClientData cd, int argc, char *argv[])
{
  LModelBackoff *lmodel  = (LModelBackoff*)cd;
  return listPutsItf((ClientData)&(lmodel->w), argc, argv);
}

/* ------------------------------------------------------------------------
    LModelBackoffIndex
   ------------------------------------------------------------------------ */

int LModelBackoffIndex( LModelBackoff* lmodel,  char* name) 
{
  return listIndex((List*)&(lmodel->w), name, 0);
}

int LModelBackoffIndexItf( ClientData clientData, int argc, char *argv[] )
{
  LModelBackoff* lmodel = (LModelBackoff*)clientData;
  return listName2IndexItf((ClientData)&(lmodel->w), argc, argv);
}

/* ------------------------------------------------------------------------
    LModelBackoffNameN
   ------------------------------------------------------------------------ */

int LModelBackoffNameN( LModelBackoff* lmodel) 
{
  return lmodel->w.itemN;
}


/* ------------------------------------------------------------------------
    LModelBackoffName
   ------------------------------------------------------------------------ */

char* LModelBackoffName( LModelBackoff* lmodel, int i) 
{
  return (i < 0) ? "(null)" : lmodel->w.itemA[i].name;
}

static int LModelBackoffNameItf( ClientData clientData, int argc, char *argv[] )
{
  LModelBackoff* lmP = (LModelBackoff*)clientData;
  return listIndex2NameItf((ClientData)&(lmP->w), argc, argv);
}

float LModelBackoffScore( LModelBackoff* lmP, int* w, int n, int k)
{
  static   IArray w2  = { NULL, 0 }; /* Keep this for all calls to come !! */
  int    i;
  float  s;

  /* initialize translation table */

  if(k>=n) return 0;

  if(w2.itemN<n+1) {
    free(w2.itemA); w2.itemN = n+1;
    w2.itemA = (int*) calloc( w2.itemN, sizeof(int));
    if(!w2.itemA) FATAL("Cannot allocate temporary scoring table");
  }

  /* Calculate -->class [ words last_word last_class --> class ]*/

  for(i=0;i<k;i++)   w2.itemA[i]   = lmP->w.itemA[w[i]].wordIdxToClass;
  for(;i<n;i++)      w2.itemA[i+1] = lmP->w.itemA[w[i]].classIdxToClass;

  if(k==0) { /* No context backoff, empty context ... */
    w2.itemA[0] = w2.itemA[1];
    s = lmP->toClassLmP.lmP->scoreFct(lmP->toClassLmP.cd,w2.itemA,1,0);
  }
  else {
    w2.itemA[k]   = lmP->w.itemA[w[k-1]].classIdxToClass;
    s = lmP->toClassLmP.lmP->scoreFct(lmP->toClassLmP.cd,w2.itemA,k+2,k+1);
  }    

  for(i=k+1;i<n;i++) {
    w2.itemA[i-1] = lmP->w.itemA[w[i-1]].wordIdxToClass;
    s+=lmP->toClassLmP.lmP->scoreFct(lmP->toClassLmP.cd,w2.itemA,i+2,i+1);
  }

  /* Calculate -->word [words class -> word ]*/

  for(i=0;i<k;i++)   w2.itemA[i]   = lmP->w.itemA[w[i]].wordIdxToWord;
  for(;i<n;i++)      w2.itemA[i+1] = lmP->w.itemA[w[i]].wordIdxToWord;

  for(i=k;i<n;i++) {
    w2.itemA[i]  = lmP->w.itemA[w[i]].classIdxToWord;
    s           += lmP->toWordLmP.lmP->scoreFct(lmP->toWordLmP.cd,w2.itemA,i+2,i+1);
    w2.itemA[i]  = w2.itemA[i+1];
  }

  return lmP->lz * s + (n-k) * lmP->lp;
}

int LModelBackoffScoreItf( ClientData cd, int argc, char * argv[])
{
  LModelBackoff*   lmP    = (LModelBackoff*) cd;
  IArray       warray = {NULL, 0};
  int          ac     =  argc-1;
  int          i      =  0;

  if (itfParseArgv( argv[0], &ac,  argv+1,  0,
     "<word sequence>", ARGV_LIARRAY, NULL, &warray, 
     &(lmP->w), "sequence of words", 
     "-idx", ARGV_INT, NULL, &i, NULL, "index of cond. prob.",
     NULL) != TCL_OK) {
    if ( warray.itemA) free( warray.itemA);
    return TCL_ERROR;
  }

  itfAppendElement( "%f", LModelBackoffScore(lmP, warray.itemA, warray.itemN, i));

  if ( warray.itemA) free( warray.itemA);
  return TCL_OK;
} 

/* ------------------------------------------------------------------------
    LModelBackoffNextFrame   update cache frame index
   ------------------------------------------------------------------------ */

char * LModelBackoffNextFrame( LModelBackoff* lmP) { lmP->toWordLmP.lmP->resetFct(lmP->toWordLmP.cd); return lmP->toClassLmP.lmP->resetFct(lmP->toClassLmP.cd); }

int LModelBackoffNextFrameItf( ClientData cd, int argc, char * argv[])
{
  LModelBackoff*   lmP    = (LModelBackoff *) cd;
  int          ac     =  argc-1;

  if (itfParseArgv( argv[0], &ac,  argv+1,  0, NULL) != TCL_OK) return TCL_ERROR;
  return LModelBackoffNextFrame(lmP) ? TCL_OK : TCL_ERROR;
} 

/* ------------------------------------------------------------------------
    LModelBackoffReset   reset language model cache
   ------------------------------------------------------------------------ */

char* LModelBackoffReset( LModelBackoff* lmP) { lmP->toWordLmP.lmP->resetFct(lmP->toWordLmP.cd); return lmP->toClassLmP.lmP->resetFct(lmP->toClassLmP.cd); }

int LModelBackoffResetItf( ClientData cd, int argc, char * argv[])
{
  LModelBackoff*   lmP    = (LModelBackoff *) cd;
  int          ac     =  argc-1;

  if (itfParseArgv( argv[0], &ac,  argv+1,  0, NULL) != TCL_OK) return TCL_ERROR;
  return LModelBackoffReset(lmP)  ? TCL_OK : TCL_ERROR;
}
 
/* ------------------------------------------------------------------------
    LModelNackoffAdd
   ------------------------------------------------------------------------ */

int LModelBackoffAdd( LModelBackoff* lmP, char* word, char *class )
{
  int     idx;

  if((idx = LModelBackoffIndex( lmP, word)) >= 0) {
    ERROR("Word '%s' already exists in '%s'.\n", word, lmP->name);
    return TCL_ERROR;
  }

  idx = listNewItem((List*)&(lmP->w), (ClientData)word);
  lmP->w.itemA[idx].wordIdxToWord     = lmP->toWordLmP.lmP->indexFct(lmP->toWordLmP.cd,word);
  lmP->w.itemA[idx].classIdxToWord    = class ? lmP->toWordLmP.lmP->indexFct(lmP->toWordLmP.cd,class) : lmP->w.itemA[idx].wordIdxToWord;

  if ( lmP->w.itemA[idx].wordIdxToWord < 0) WARN("Word '%s' is not in language model.\n", word);
  if ( class && (lmP->w.itemA[idx].classIdxToWord < 0)) WARN("Word-class '%s' is not in language model '%s'.\n", class, lmP->toWordLmP.lmP->name);
    
  lmP->w.itemA[idx].wordIdxToClass     = lmP->toClassLmP.lmP->indexFct(lmP->toClassLmP.cd,word);
  lmP->w.itemA[idx].classIdxToClass    = class ? lmP->toClassLmP.lmP->indexFct(lmP->toClassLmP.cd,class) : lmP->w.itemA[idx].wordIdxToClass;

  if ( lmP->w.itemA[idx].wordIdxToClass < 0) WARN("Word '%s' is not in language model.\n", word);
  if ( class && (lmP->w.itemA[idx].classIdxToClass < 0)) WARN("Word-class '%s' is not in language model '%s'.\n", class, lmP->toClassLmP.lmP->name);
    
  return TCL_OK;
}

int LModelBackoffAddItf (ClientData cd, int argc, char *argv[])
{
  LModelBackoff* lmP      = (LModelBackoff *)cd;
  int        ac       =  argc - 1;
  char*      word     =  NULL;
  char*      class =  NULL;
  int        ret;

  if (itfParseArgv(argv[0], &ac, argv+1, 0, 
     "<word>" ,   ARGV_STRING, NULL, &word ,    cd, "word", 
     "-class" ,   ARGV_STRING, NULL, &class ,   cd, "class", 
      NULL) != TCL_OK) return TCL_ERROR;

  ret = LModelBackoffAdd( lmP, word, class );
  return ret;
}


/* ------------------------------------------------------------------------
    LModelBackoffRead
   ------------------------------------------------------------------------ */

int LModelBackoffReadItf (ClientData cd, int argc, char *argv[])
{
  LModelBackoff*  lmP      = (LModelBackoff*)cd;
  int         ac       =  argc - 1;
  char*       fileName =  NULL;
  int         n;

  if (itfParseArgv(argv[0], &ac, argv+1, 0, "<filename>", ARGV_STRING, NULL, &fileName, cd, "file to read from", 
       NULL) != TCL_OK) return TCL_ERROR;

  itfAppendResult( "%d", n = freadAdd( cd, fileName, lmP->commentChar, LModelBackoffAddItf));
  return (n >= 0) ? TCL_OK : TCL_ERROR;
}

/* ========================================================================
    Type Declaration
   ======================================================================== */

static Method LModelBackoffItemMethod[] = 
{ 
  { "puts", LModelBackoffItemPutsItf,   NULL },
  {  NULL,  NULL, NULL } 
};

TypeInfo LModelBackoffItemInfo = { 
        "LModelBackoffItem", 0, -1, LModelBackoffItemMethod, 
         NULL, NULL,
         LModelBackoffItemConfigureItf, NULL,
         itfTypeCntlDefaultNoLink, 
        "Language Model Selection\n" };

static Method LModelBackoffMethod[] = 
{ 
  { "puts",   LModelBackoffPutsItf,   "display the contents of an LModel" },
  { "add",    LModelBackoffAddItf,    "add a mapping to LModelBackoff" },
  { "index",  LModelBackoffIndexItf,  "return the internal index of an LModelItem" },
  { "name",   LModelBackoffNameItf,   "return the name of an LModelItem" },
  { "score",  LModelBackoffScoreItf,  "return the score of a text string"     },

  { "nextFrame", LModelBackoffNextFrameItf,  "incr. cache frame index"  },
  { "reset",     LModelBackoffResetItf,      "reset language model"     },

  { "read",      LModelBackoffReadItf,       "read mapping from file"     },

  {  NULL,    NULL,           NULL } 
};

TypeInfo LModelBackoffInfo = {
        "LModelBackoff", 0, -1, LModelBackoffMethod, 
         LModelBackoffCreateItf,LModelBackoffFreeItf,
         LModelBackoffConfigureItf,LModelBackoffAccessItf, NULL,
	"Language Model Selection\n" } ;

static LM LModelBackoffLM = 
       { "LModelBackoff", &LModelBackoffInfo, &LModelBackoffInfo,
         (LMIndexFct*)    LModelBackoffIndex,
         (LMNameFct *)    LModelBackoffName,
         (LMNameNFct *)   LModelBackoffNameN,
         (LMScoreFct*)    LModelBackoffScore,
         (LMNextFrameFct*)LModelBackoffNextFrame,
         (LMResetFct*)    LModelBackoffReset,
       NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL };

int LModelBackoff_Init()
{
  static int LModelBackoffInitialized = 0;

  if (! LModelBackoffInitialized) {

    LModelBackoffDefault.name            = NULL;
    LModelBackoffDefault.useN            = 0;
    LModelBackoffDefault.w.itemN         = 0;
    LModelBackoffDefault.w.blkSize       = 1000;
    LModelBackoffDefault.lz              = 1.0;
    LModelBackoffDefault.lp              = 0.0;
    LModelBackoffDefault.commentChar     = ';';

    itfNewType (&LModelBackoffItemInfo);
    itfNewType (&LModelBackoffInfo);
    lmNewType(  &LModelBackoffLM);

    LModelBackoffInitialized = 1;
  }
  return TCL_OK;
}
