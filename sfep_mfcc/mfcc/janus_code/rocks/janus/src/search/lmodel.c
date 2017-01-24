/* ========================================================================
    JanusRTk   Janus Recognition Toolkit
               ------------------------------------------------------------
               Object: Language Model
               ------------------------------------------------------------

    Author  :  Michael Finke
    Module  :  lmodel.c
    Date    :  $Id: lmodel.c 2390 2003-08-14 11:20:32Z fuegen $
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
    Revision 1.7  2003/08/14 11:20:23  fuegen
    Merged changes on branch jtk-01-01-15-fms (jaguar -> ibis-013)

    Revision 1.6.4.6  2002/06/26 11:57:58  fuegen
    changes for new directory structure support and code cleaning

    Revision 1.6.4.5  2002/01/21 09:08:30  metze
    Typo!

    Revision 1.6.4.4  2002/01/21 08:17:57  metze
    Cosmetic changes

    Revision 1.6.4.3  2001/06/01 08:21:38  fuegen
    moved include of rand48.h for windows into common.h

    Revision 1.6.4.2  2001/03/05 08:39:51  metze
    Cleaned up treatment of exact LM in LMLA

    Revision 1.6.4.1  2001/02/27 15:10:27  metze
    LModelNJD and LMLA work with floats

    Revision 1.6  2001/01/15 09:49:59  janus
    Merged Christian's version with WINDOWS support (branch jtk-00-11-14-fuegen)

    Revision 1.5.4.1  2001/01/12 15:16:57  janus
    necessary changes for running janus under WINDOWS

    Revision 1.5  2000/11/14 12:29:35  janus
    Merging branch jtk-00-10-27-jmcd.

    Revision 1.4  2000/11/02 15:41:20  janus
    Merged changes on branch jtk-00-10-24-tschaaf

    Revision 1.3.22.1  2000/11/02 15:08:08  janus
    adding sample function from LModelLong to LModel

    Revision 1.3.26.2  2000/11/08 17:22:07  janus
    Finished making changes required for compilation under 'gcc -Wall'.

    Revision 1.3.26.1  2000/11/06 10:50:34  janus
    Made changes to enable compilation under -Wall.

    Revision 1.3  2000/09/14 17:47:12  janus
    Merging branch jtk-00-09-14-jmcd.

    Revision 1.2.30.1  2000/09/14 16:20:55  janus
    Merged branches jtk-00-09-10-tschaaf, jtk-00-09-12-hyu, and jtk-00-09-12-metze.

    Revision 1.2.24.1  2000/09/13 22:16:29  hyu
    Speeded up LM reading.

    Revision 1.2  1998/07/08 11:22:28  kries
    faster reading + old bugfix applied

 * Revision 1.1  1998/05/26  18:27:23  kries
 * Initial revision
 *

   ======================================================================== */

#include <ctype.h>

#include "common.h"
#include "itf.h"
#include "error.h"
#include "lm.h"
#include "lmodel.h"
#include "lmodelCache.h"
#include "ffmat.h"

char lmodelRCSVersion[]= "@(#)1$Id: lmodel.c 2390 2003-08-14 11:20:32Z fuegen $";


/* WARNING: This is a hack to simulate the use of a discrete (i.e. 8bit lin)
   language model */
/* #define LMCompress(A) ((A) > 0.0) ? 0.0 : (((A) < -8.0) ? -8.0 : (floor((A)*32.0 + 1.0)/32.0)) */
#define LMCompress(A) (A)


/* ------------------------------------------------------------------------
    Forward Declaration
   ------------------------------------------------------------------------ */

extern TypeInfo lmodelInfo;

/* ========================================================================
    FloatHeap
   ======================================================================== */

typedef double HashKey;

/* ------------------------------------------------------------------------
    floatHeapHashKey & floatHeapHashCmp
   ------------------------------------------------------------------------ */

static long floatHeapHashKey (const HashKey *key)
{
  union { float fn; long ln; } f;
  /* union { double fn; long ln; } f;
     f.ln  =  0; */
  f.fn  = *key;

  return f.ln;
}

static int floatHeapHashCmp( const HashKey *key, const double *f)
{
  return (*key == *f);
}

/* ------------------------------------------------------------------------
    floatHeapCreate/Init
   ------------------------------------------------------------------------ */

int floatHeapInit( FloatHeap* fhP)
{
  assert( fhP);

  if ( listInit((List*)fhP, NULL, sizeof(double), 5000) != TCL_OK)
       return TCL_ERROR;

  fhP->init    = (ListItemInit  *)NULL;
  fhP->deinit  = (ListItemDeinit*)NULL;

  fhP->hashKey = (HashKeyFn     *)floatHeapHashKey;
  fhP->hashCmp = (HashCmpFn     *)floatHeapHashCmp;

  return TCL_OK;
}

FloatHeap* floatHeapCreate( )
{
  FloatHeap* fhP = (FloatHeap*)malloc(sizeof(FloatHeap));

  if (! fhP || floatHeapInit( fhP) != TCL_OK) {
     if ( fhP) free( fhP);
     ERROR("Failed to allocate FloatHeap object.\n");
     return NULL; 
  }
  return fhP;
}

/* ------------------------------------------------------------------------
    floatHeapDeinit
   ------------------------------------------------------------------------ */

int floatHeapDeinit( FloatHeap *fhP)
{
  assert( fhP);
  return listDeinit((List*)fhP);
}

int floatHeapFree( FloatHeap* fhP)
{
  if ( floatHeapDeinit(fhP)!=TCL_OK) return TCL_ERROR;
  free(fhP);
  return TCL_OK;
}
/* Not used
static int floatHeapFreeItf(ClientData cd)
{
  return floatHeapFree((FloatHeap*)cd);
}
*/
/* ------------------------------------------------------------------------
    floatHeapAdd
   ------------------------------------------------------------------------ */

int floatHeapAdd( FloatHeap *fhP, double x)
{
  double y   = x;
  int   idx  = listIndex((List*)fhP, (ClientData)&y, 1);
  if (  idx >= 0) fhP->itemA[idx] = y;
  return idx;
}

/* ========================================================================
    LModelItem
   ======================================================================== */
/* ------------------------------------------------------------------------
    lmodelItemInit
   ------------------------------------------------------------------------ */

int lmodelItemInit( LModelItem* lmiP, ClientData CD)
{
  assert( lmiP);

  lmiP->name       =  ( CD) ? strdup((char*)CD) : NULL;
  lmiP->prob[0]    =    0.0;
  lmiP->prob[1]    =    0.0;
  lmiP->backOff    =    0.0;
  lmiP->fbgramX    =   -1;
  lmiP->lbgramX    =   -2;

  return TCL_OK;
}

/* ------------------------------------------------------------------------
    lmodelItemDeinit
   ------------------------------------------------------------------------ */

int lmodelItemDeinit( LModelItem* lmiP)
{
  if (!lmiP) return TCL_OK;
  if ( lmiP->name)   { free(lmiP->name);   lmiP->name   = NULL; }
  return TCL_OK;
}

/* ------------------------------------------------------------------------
    lmodelItemConfigureItf
   ------------------------------------------------------------------------ */

static int lmodelItemConfigureItf( ClientData cd, char *var, char *val)
{
  LModelItem* lmiP = (LModelItem*)cd;
  if (! lmiP) return TCL_ERROR;

  if (! var) {
    ITFCFG(lmodelItemConfigureItf,cd,"name");
    ITFCFG(lmodelItemConfigureItf,cd,"prob0");
    ITFCFG(lmodelItemConfigureItf,cd,"prob1");
    ITFCFG(lmodelItemConfigureItf,cd,"backOff");
    ITFCFG(lmodelItemConfigureItf,cd,"fbgramX");
    ITFCFG(lmodelItemConfigureItf,cd,"lbgramX");
    return TCL_OK;
  }
  ITFCFG_CharPtr( var,val,"name",         lmiP->name,       1);
  ITFCFG_Float(   var,val,"prob0",        lmiP->prob[0],    0);
  ITFCFG_Float(   var,val,"prob1",        lmiP->prob[1],    0);
  ITFCFG_Float(   var,val,"backOff",      lmiP->backOff,    0);
  ITFCFG_Int(     var,val,"fbgramX",      lmiP->fbgramX,    1);
  ITFCFG_Int(     var,val,"lbgramX",      lmiP->lbgramX,    1);
  ERROR("Unknown option '-%s %s'.\n", var, val ? val : "?");
  return TCL_ERROR;
}

/* ------------------------------------------------------------------------
    lmodelItemPutsItf
   ------------------------------------------------------------------------ */

int lmodelItemPuts( LModelItem* lmodelItem)
{
  itfAppendElement("NAME");
  itfAppendElement("%s", lmodelItem->name);
  return TCL_OK;
}

static int lmodelItemPutsItf( ClientData cd, int argc, char *argv[])
{
  int   ac   =  argc-1;
  LModelItem* lmodelItem = (LModelItem*)cd;

  if ( itfParseArgv( argv[0], &ac, argv+1, 0, NULL) != TCL_OK)
       return TCL_ERROR;

  return lmodelItemPuts( lmodelItem);
}

/* ========================================================================
    LModel
   ======================================================================== */

static LModel lmodelDefault;

/* ------------------------------------------------------------------------
    Create LModel Structure
   ------------------------------------------------------------------------ */

int lmodelInit( LModel* lmP, char* name, int cacheN)
{
  assert( name);
  assert( lmP);

  lmP->name   = strdup(name);
  lmP->useN   = 0;

  if ( listInit((List*)&(lmP->list), itfGetType("LModelItem"), 
                                          sizeof(LModelItem),
                                 lmodelDefault.list.blkSize) != TCL_OK) {
    ERROR("Couldn't allocate unigram list.");
    return TCL_ERROR;
  }

  lmP->list.init    = (ListItemInit  *)lmodelItemInit;
  lmP->list.deinit  = (ListItemDeinit*)lmodelItemDeinit;

  if (!(lmP->cacheP  = lmodelCacheCreate( lmP, cacheN, 1))) {
    ERROR("Couldn't allocate cache.");
    listDeinit((List*)&(lmP->list));
    return TCL_ERROR;
  }

  lmP->bgramA          = NULL;
  lmP->bgramN          = 0;

  lmP->bprobA.itemN    = 0;
  lmP->bprobA.itemA    = NULL;
  lmP->bbackOffA.itemN = 0;
  lmP->bbackOffA.itemA = NULL;

  lmP->tgramA          = NULL;
  lmP->tgramN          = 0;
  lmP->tprobA.itemN    = 0;
  lmP->tprobA.itemA    = NULL;

  lmP->tgramSegSize    = 9;
  lmP->tgramXA         = NULL;
  lmP->tgramXN         = 0;

  lmP->lz              = lmodelDefault.lz;
  lmP->lp              = lmodelDefault.lp;

  lmP->begItemX        = -1;
  lmP->endItemX        = -1;

  return TCL_OK;
}

LModel* lmodelCreate( char* name, int cacheN)
{
  LModel *lmP = (LModel*)malloc(sizeof(LModel));

  if (! lmP || lmodelInit( lmP, name, cacheN) != TCL_OK) {
     if ( lmP) free( lmP);
     ERROR("Failed to allocate LModel object '%s'.\n", name);
     return NULL; 
  }
  return lmP;
}

static ClientData lmodelCreateItf (ClientData cd, int argc, char *argv[])
{
  int     ac         = argc-1;
  float   lz         = lmodelDefault.lz;
  float   lp         = lmodelDefault.lp;
  LModel* lmP        = NULL;
  int     cacheLineN = 200;
  

  if ( itfParseArgv(argv[0], &ac, argv+1, 0, 
      "-cacheLineN",  ARGV_INT,    NULL, &cacheLineN, NULL,
      "number of cache lines for Lm",
      "-lz", ARGV_FLOAT, NULL, &lz, NULL, "language model weight",
      "-lp", ARGV_FLOAT, NULL, &lp, NULL, "insertion penalty",
       NULL) != TCL_OK) return NULL;

  if ( (lmP = lmodelCreate(argv[0], cacheLineN)) ) {
    lmP->lz = lz;
    lmP->lp = lp;
  }
  return (ClientData)lmP;
}

/* ------------------------------------------------------------------------
    Free LModel Structure
   ------------------------------------------------------------------------ */

int lmodelLinkN( LModel* lmodel)
{
  int  useN = listLinkN((List*)&(lmodel->list)) - 1;
  if ( useN < lmodel->useN) return lmodel->useN;
  else                             return useN;
}

int lmodelDeinit (LModel *lmP)
{
  assert( lmP);

  if ( lmodelLinkN( lmP)) {
    SERROR("LModel '%s' still in use by other objects.\n", lmP->name);
    return TCL_ERROR;
  } 

  if (lmP->name)    { free(lmP->name);    lmP->name    = NULL; }
  if (lmP->bgramA)  { free(lmP->bgramA);  lmP->bgramA  = NULL; lmP->bgramN  = 0; } 
  if (lmP->tgramA)  { free(lmP->tgramA);  lmP->tgramA  = NULL; lmP->tgramN  = 0; }
  if (lmP->tgramXA) { free(lmP->tgramXA); lmP->tgramXA = NULL; lmP->tgramXN = 0; }

  if (lmP->cacheP)  { lmodelCacheFree( lmP->cacheP); lmP->cacheP = NULL; }

  farrayDeinit( &lmP->bprobA);
  farrayDeinit( &lmP->bbackOffA);
  farrayDeinit( &lmP->tprobA);

  return listDeinit((List*)&(lmP->list));
}

int lmodelFree (LModel *lmP)
{
  if (lmodelDeinit(lmP)!=TCL_OK) return TCL_ERROR;

  free(lmP);
  return TCL_OK;
}

static int lmodelFreeItf(ClientData cd)
{
  return lmodelFree((LModel*)cd);
}

/* ------------------------------------------------------------------------
    lmodelConfigureItf
   ------------------------------------------------------------------------ */

static int lmodelConfigureItf (ClientData cd, char *var, char *val)
{
  LModel* lmodel = (LModel*)cd;
  if (! lmodel) lmodel = &lmodelDefault;

  if (! var) {
    ITFCFG(lmodelConfigureItf,cd,"useN");
    ITFCFG(lmodelConfigureItf,cd,"lz");
    ITFCFG(lmodelConfigureItf,cd,"lp");
    ITFCFG(lmodelConfigureItf,cd,"begItemX");
    ITFCFG(lmodelConfigureItf,cd,"endItemX");
    return listConfigureItf((ClientData)&(lmodel->list), var, val);
  }
  ITFCFG_Int(    var,val,"useN",     lmodel->useN,             1);
  ITFCFG_Float(  var,val,"lz",       lmodel->lz,               0);
  ITFCFG_Float(  var,val,"lp",       lmodel->lp,               0);
  ITFCFG_Int(    var,val,"begItemX", lmodel->begItemX,         0);
  ITFCFG_Int(    var,val,"endItemX", lmodel->endItemX,         0);
  return listConfigureItf((ClientData)&(lmodel->list), var, val);   
}

/* ------------------------------------------------------------------------
    lmodelAccessItf
   ------------------------------------------------------------------------ */

static ClientData lmodelAccessItf (ClientData cd, char *name, TypeInfo **ti)
{ 
  LModel* lmodel = (LModel*)cd;

  if (*name == '.') { 
    if (name[1]=='\0') {
       itfAppendElement( "bprobA");
       itfAppendElement( "bbackOffA");
       *ti = NULL;
    }
    else { 
      if (! strcmp( name+1, "bprobA")) {
        *ti = itfGetType("FArray");
        return (ClientData)&(lmodel->bprobA);
      }
      else if (! strcmp( name+1, "bbackOffA")) {
        *ti = itfGetType("FArray");
        return (ClientData)&(lmodel->bbackOffA);
      }
    }
  }
  return listAccessItf((ClientData)&(lmodel->list),name,ti);
}

/* ------------------------------------------------------------------------
    lmodelPutsItf
   ------------------------------------------------------------------------ */

static int lmodelPutsItf (ClientData cd, int argc, char *argv[])
{
  LModel *lmodel  = (LModel*)cd;
  return listPutsItf((ClientData)&(lmodel->list), argc, argv);
}

/* ------------------------------------------------------------------------
    lmodelIndex
   ------------------------------------------------------------------------ */

int lmodelIndex( LModel* lmodel,  char* name) 
{
  return listIndex((List*)&(lmodel->list), name, 0);
}

int lmodelIndexItf( ClientData clientData, int argc, char *argv[] )
{
  LModel* lmodel = (LModel*)clientData;
  return listName2IndexItf((ClientData)&(lmodel->list), argc, argv);
}

/* ------------------------------------------------------------------------
    lmodelName
   ------------------------------------------------------------------------ */

char* lmodelName( LModel* lmodel, int i) 
{
  return (i < 0) ? "(null)" : lmodel->list.itemA[i].name;
}

static int lmodelNameItf( ClientData clientData, int argc, char *argv[] )
{
  LModel* lmP = (LModel*)clientData;
  return listIndex2NameItf((ClientData)&(lmP->list), argc, argv);
}

/* ------------------------------------------------------------------------
    LModelNameN
   ------------------------------------------------------------------------ */

int lmodelNameN( ClientData clientData ) 
{
  LModel* lmodel = (LModel*)clientData;
  return lmodel->list.itemN;
}

/* ------------------------------------------------------------------------
    lmodelFindBGram  do a binary search to find a bigram in the list of
                     bigrams <w1,*>.
   ------------------------------------------------------------------------ */

int lmodelFindBGram( LModel* lmP, WordIdx w1, WordIdx w2)
{
  int bg      = lmP->list.itemA[w1].fbgramX;
  int lbg     = lmP->list.itemA[w1].lbgramX;
  int lbgramX = lbg;

  while ( bg < lbg - 8) {
    int tmp = ( bg + lbg) / 2;

    if ( w2 < lmP->bgramA[tmp].wordX) lbg = tmp-1;
    else                              bg  = tmp;
  }

  /* ------------------------------------------- */
  /* Do linear search when array is small enough */
  /* ------------------------------------------- */

  for ( ; bg <= lbg; bg++) {
    if (lmP->bgramA[bg].wordX == w2) break;
  }

  return (((bg <= lbgramX) && (lmP->bgramA[bg].wordX == w2)) ? bg : -1);
}

/* ------------------------------------------------------------------------
    lmodelReadLine
   ------------------------------------------------------------------------ */

ArpaboLModelSection lmodelReadLine( LModel* lmP, FILE* fP, char* line, int size)
{
  if (! fgets(  line, size, fP) || feof (fP)) return LM_END;

  if (line[0] != '\\')                 return LM_LINE;

  if (! strcmp( line, "\\data\\\n"))    return LM_DATA;
  if (! strcmp( line, "\\end\\\n"))     return LM_END;
  if (! strcmp( line, "\\1-grams:\n"))  return LM_1GRAMS;
  if (! strcmp( line, "\\2-grams:\n"))  return LM_2GRAMS;
  if (! strcmp( line, "\\3-grams:\n"))  return LM_3GRAMS;
  if (! strcmp( line, "\\unigrams:\n")) return LM_UNIGRAMS;
  return LM_LINE;
}

/* ------------------------------------------------------------------------
    lmodelReadNGramCounts  find the number of bigrams and trigrams in
                           the LM file
   ------------------------------------------------------------------------ */

int lmodelReadNGramCounts( LModel* lmP, FILE* fP, 
                           int* ugramN, int* bgramN, int* tgramN, 
                           ArpaboLModelSection* id)
{
  ArpaboLModelSection lms = LM_END;
  char          line[1024];

  while ((lms = lmodelReadLine( lmP, fP, line, sizeof(line))) == LM_LINE) {

    int  ngram;
    long count;

    /* ---------- */
    /* Parse line */
    /* ---------- */

    if ( sscanf( line, "ngram %d=%ld", &ngram, &count) != 2) {
      if ( line[0] != '\n') INFO("Format error; \\data\\ line ignored: %s", line);
      continue;
    }

    /* ------------ */
    /* Assign Count */
    /* ------------ */

    switch ( ngram) {
    case 1: if ( ugramN) *ugramN = count;
            break;
    case 2: if ( bgramN) *bgramN = count;
            break;
    case 3: if ( tgramN) *tgramN = count;
            break;
    default:

      ERROR("%d-grams are not supported yet.\n", count);
      return TCL_ERROR;
    }
  }

  if ( id) *id = lms; 
  return TCL_OK;
}

/* ------------------------------------------------------------------------
    lmodelReadUnigrams
   ------------------------------------------------------------------------ */

int lmodelReadUnigrams( LModel* lmP, FILE* fP, int ugramN, ArpaboLModelSection* id)
{
  char          line[1024];
  ArpaboLModelSection lms;
  int           n = 0;

  while ((lms = lmodelReadLine( lmP, fP, line, sizeof(line))) == LM_LINE) {

    char  word[1024];
    float prob, backOff;
    int   idx;

    /* ---------- */
    /* Parse line */
    /* ---------- */

    if ( sscanf( line, "%f %s %f", &prob, word, &backOff) != 3) {
      if ( line[0] != '\n') INFO("Format error; unigram ignored: %s", line);
      continue;
    }

    /* -------------------------- */
    /* Try to find word in LModel */
    /* -------------------------- */

    if ( (idx = lmodelIndex( lmP, word)) >= 0) {
      ERROR("Word '%s' already exists in '%s'.\n", word, lmP->name);
      continue;
    }

    /* ---------------------- */
    /* Add new word to LModel */
    /* ---------------------- */

    idx = listNewItem((List*)&(lmP->list), (ClientData)word);

    /* -------------------------------------- */
    /* Modify prob of <s> and backOff of </s> */
    /* -------------------------------------- */

    if (! strcmp( word, "</s>")) { lmP->endItemX = idx; backOff = SMALLPROB; }
    if (! strcmp( word, "<s>"))  { lmP->begItemX = idx; prob    = SMALLPROB; }

    lmP->list.itemA[idx].prob[0]    = LMCompress(prob);
    lmP->list.itemA[idx].prob[1]    = LMCompress(prob);
    lmP->list.itemA[idx].backOff    = LMCompress(backOff);

    n++;
  }

  /* ------------------------ */
  /* Check number of unigrams */
  /* ------------------------ */

  if ( ugramN != n) {
    ERROR( "Number of unigrams does not match.\n");
    ERROR( "Expected %d unigrams but found %d.\n", ugramN, n);
    return TCL_ERROR;
  }

  if ( id) *id = lms; 
  INFO("Read 1-grams  ugramN=%d.\n", ugramN);
  return TCL_OK;
}

/* ------------------------------------------------------------------------
    lmodelReadBigrams
   ------------------------------------------------------------------------ */

int lmodelReadBigrams( LModel* lmP, FILE* fP, 
                       int ugramN, int bgramN, int tgramN, ArpaboLModelSection* id)
{
  FloatHeap*    probHeap    = NULL;
  FloatHeap*    backOffHeap = NULL;
  ArpaboLModelSection lms;
  char          line[1024];
  int           bgramX = 0;
  int           i;

  float prob, backOff;
  int   probX;
  int   backOffX;
  int   idx[2];
  char* segments[1024]; /* the scan of the corresponding line */
  char* scanner;

  /* -------------------------- */
  /* Allocate space for bigrams */
  /* -------------------------- */

  if ( lmP->bgramA) {
    ERROR("Bigram table already exists. bgramN = %d.\n", lmP->bgramN);
    return TCL_ERROR;
  }

  if (!(lmP->bgramA = (BigramItem*)malloc((bgramN+1) * sizeof(BigramItem)))) {
    ERROR("Memory overflow. Bigram table too big, bgramN = %d.\n", bgramN);
    return TCL_ERROR;
  } else {
    lmP->bgramN = bgramN;
  }

  /* -------------------- */
  /* Allocate Float Heaps */
  /* -------------------- */

  if (! (probHeap    = floatHeapCreate()) ||
      ! (backOffHeap = floatHeapCreate())) {

    if ( probHeap)    { floatHeapFree( probHeap);    probHeap    = NULL; }
    if ( backOffHeap) { floatHeapFree( backOffHeap); backOffHeap = NULL; }
    if ( lmP->bgramA) { free( lmP->bgramA); lmP->bgramA = NULL; }
    lmP->bgramN = 0;
    return TCL_ERROR;
  }

  /* --------- */
  /* Read File */
  /* --------- */

  while ((lms = lmodelReadLine( lmP, fP, line, sizeof(line))) == LM_LINE) {

    /* ---------- */
    /* Parse line */
    /* ---------- */

    /* ------------------------------------------ */
    /* Scan the line for space seperated regions  */
    /* ------------------------------------------ */

    for(scanner=line,i=0;*scanner!='\000';i++) {
      for(;*scanner!='\000' && isspace(*scanner);scanner++);
      if(*scanner=='\000')
	break;
      segments[i]=scanner;
      for(;*scanner!='\000' &&  !isspace(*scanner);scanner++);
      if(*scanner!='\000')
	*(scanner++)='\000';
    }

    if( i!=3 && i!=4 ) {
      int skip = (i>1);
      for(scanner=line;i>1;scanner++) 
	if(*scanner=='\000') {
	  *scanner=' '; i--;
	}
      if(skip) INFO("Current line invalid, skipping: %s\n",line);
      continue;
    }
 
    if ( bgramX >= bgramN) {
      ERROR("Too many bigrams in the LModel file.\n");
      return TCL_ERROR;
    }

    /* ----------------------------------- */
    /* Evaluate the arguments of the scan  */
    /* ----------------------------------- */
    prob    = atof(segments[0]);
    backOff = (4 == i) ? atof(segments[3]) : 0;


    /* --------------------------- */
    /* Try to find words in LModel */
    /* --------------------------- */

    if ( (idx[0] = lmodelIndex( lmP, segments[1])) < 0) {
      ERROR("Word '%s' does not exist in '%s'.\n", segments[1], lmP->name);
      continue;
    }
    if ( (idx[1] = lmodelIndex( lmP, segments[2])) < 0) {
      ERROR("Word '%s' does not exist in '%s'.\n", segments[2], lmP->name);
      continue;
    }

    /* ---------------------- */
    /* Modify prob of <s> <s> */
    /* ---------------------- */
    if ( idx[0] == lmP->begItemX && idx[1] == lmP->begItemX) prob = SMALLPROB;
    prob    = LMCompress(prob);
    backOff = LMCompress(backOff);

    /* ---------------------------------- */
    /* Compute Idx of floats in FloatHeap */
    /* ---------------------------------- */

    if ((probX    = floatHeapAdd( probHeap,    prob))    < 0 ||
        (backOffX = floatHeapAdd( backOffHeap, backOff)) < 0) {
      ERROR("FloatHeap Error: %s", line);
      continue;
    }

    if ( probX > MAX_FLOATX || backOffX > MAX_FLOATX ) {
      ERROR("FloatHeap Overflow (prob:%d,backOff:%d): %s", 
             probX, backOffX, line);
      continue;
    }

    /* -------------------- */
    /* Add bigram to LModel */
    /* -------------------- */

    if ( lmP->list.itemA[idx[0]].fbgramX < 0) {
      lmP->list.itemA[idx[0]].fbgramX =  bgramX;
    }
    lmP->list.itemA[idx[0]].lbgramX   =  bgramX;
  
    lmP->bgramA[bgramX].wordX    =  idx[1];
    lmP->bgramA[bgramX].probX    =  probX;
    lmP->bgramA[bgramX].backOffX =  backOffX;
    lmP->bgramA[bgramX].tgramX   =  NO_NGRAM;

    bgramX++;
  }

  /* ------------ */
  /* Set Endpoint */
  /* ------------ */

  lmP->bgramA[bgramX].tgramX     =  0;

  /* --------------- */
  /* Save FloatHeaps */
  /* --------------- */

  farrayInit( &lmP->bprobA,    probHeap->itemN);
  farrayInit( &lmP->bbackOffA, backOffHeap->itemN);

  for ( i = 0; i < probHeap->itemN; i++)
    lmP->bprobA.itemA[i] = probHeap->itemA[i];

  for ( i = 0; i < backOffHeap->itemN; i++)
    lmP->bbackOffA.itemA[i] = backOffHeap->itemA[i];

  if ( probHeap)    { floatHeapFree( probHeap);    probHeap    = NULL; }
  if ( backOffHeap) { floatHeapFree( backOffHeap); backOffHeap = NULL; }

  if ( bgramX != bgramN) {
    ERROR( "Number of bigrams does not match.\n");
    ERROR( "Expected %d bigrams but found %d.\n", bgramN, bgramX);
    return TCL_ERROR;
  }

  if ( id) *id = lms; 

  INFO("Read 2-grams  bgramN=%d bprobN=%d bbackOffN=%d.\n", bgramX, 
        lmP->bprobA.itemN, lmP->bbackOffA.itemN);

  return TCL_OK;
}

/* ------------------------------------------------------------------------
    lmodelReadTrigrams
   ------------------------------------------------------------------------ */

int lmodelReadTrigrams( LModel* lmP, FILE* fP, 
                        int ugramN, int bgramN, int tgramN, ArpaboLModelSection* id)
{
  FloatHeap*    probHeap    = NULL;
  ArpaboLModelSection lms;
  char          line[1024];
  int           w[2]        = { -1, -1};
  int           tgramX      =    0;
  int           i, j;

  float      prob;
  NGramIdx   probX;
  int        idx[3];
  char   *   segments[1024]; /* the scan of the corresponding line */
  char   *   scanner;

  int 	     bgramX;	     /* bigram index */
  int 	     lbgramX;        /* last bigram index */
  int 	     segX;

  /* --------------------------- */
  /* Allocate space for trigrams */
  /* --------------------------- */

  if ( lmP->tgramA) {
    ERROR("Trigram table already exists. tgramN = %d.\n", lmP->tgramN);
    return TCL_ERROR;
  }

  if (!(lmP->tgramA = (TrigramItem*)malloc((tgramN+1) * sizeof(TrigramItem)))) {
    ERROR("Memory overflow. Trigram table too big, tgramN = %d.\n", tgramN);
    return TCL_ERROR;
  } else {
    lmP->tgramN = tgramN;
  }

  /* ------------------------------------------ */
  /* Allocate space for trigram segment offsets */
  /* ------------------------------------------ */

  lmP->tgramXN = (bgramN+1) / (1 << (lmP->tgramSegSize-1)) + 1;
  if (!(lmP->tgramXA = (int*)malloc(lmP->tgramXN * sizeof(int)))) {
    ERROR("Memory overflow. Trigram reference table too big (%d).\n", lmP->tgramXN);
    return TCL_ERROR;
  } else {
    int i;
    for ( i = 0; i < lmP->tgramXN; i++) lmP->tgramXA[i] = -1;
  }

  /* -------------------- */
  /* Allocate Float Heaps */
  /* -------------------- */

  if (! (probHeap    = floatHeapCreate())) {

    if ( probHeap)    { floatHeapFree( probHeap);    probHeap    = NULL; }
    if ( lmP->tgramA) { free( lmP->tgramA); lmP->tgramA = NULL; }
    lmP->tgramN = 0;
    return TCL_ERROR;
  }

  /* --------- */
  /* Read File */
  /* --------- */

  while ((lms = lmodelReadLine( lmP, fP, line, sizeof(line))) == LM_LINE) {

    /* ---------- */
    /* Parse line */
    /* ---------- */

    /* ------------------------------------------ */
    /* Scan the line for space seperated regions  */
    /* ------------------------------------------ */

    for(scanner=line,i=0;*scanner!='\000';i++) {
      for(;*scanner!='\000' && isspace(*scanner);scanner++);
      if(*scanner=='\000')
	break;
      segments[i]=scanner;
      for(;*scanner!='\000' &&  !isspace(*scanner);scanner++);
      if(*scanner!='\000')
	*(scanner++)='\000';
    }

    if(i!=4) {
      for(scanner=line;i>1;scanner++) 
	if(*scanner=='\000') {
	  *scanner=' '; i--;
	}
      if(i!=0) INFO("Current line invalid, skipping: %s\n",line);
      continue;
    }
 
    if ( tgramX >= tgramN) {
      ERROR("Too many trigrams in the LModel file.\n");
      return TCL_ERROR;
    }

    /* ----------------------------------- */
    /* Evaluate the arguments of the scan  */
    /* ----------------------------------- */

    prob=atof(segments[0]);
    
    /* --------------------------- */
    /* Try to find words in LModel */
    /* --------------------------- */

    if ( (idx[0] = lmodelIndex( lmP, segments[1])) < 0) {
      ERROR("Word '%s' does not exist in '%s'.\n", segments[1], lmP->name);
      continue;
    }
    if ( (idx[1] = lmodelIndex( lmP, segments[2])) < 0) {
      ERROR("Word '%s' does not exist in '%s'.\n", segments[2], lmP->name);
      continue;
    }
    if ( (idx[2] = lmodelIndex( lmP, segments[3])) < 0) {
      ERROR("Word '%s' does not exist in '%s'.\n", segments[3], lmP->name);
      continue;
    }

    /* -------------------------- */
    /* Modify prob of <s> <s> <s> */
    /* -------------------------- */
    if ( idx[0] == lmP->begItemX && idx[1] == lmP->begItemX &&
         idx[2] == lmP->begItemX) prob = SMALLPROB;
    prob    = LMCompress(prob);

    /* ---------------------------------- */
    /* Compute Idx of floats in FloatHeap */
    /* ---------------------------------- */

    if ((probX    = (NGramIdx)floatHeapAdd( probHeap, prob)) == NO_NGRAM) {
      ERROR("FloatHeap Error: %s", line);
      continue;
    }

    if ( probX > MAX_FLOATX) {
      ERROR("FloatHeap Overflow (prob:%d): %s", probX, line);
      continue;
    }

    /* --------------------- */
    /* Find bigram to LModel */
    /* --------------------- */

    if ( w[0] != idx[0] || w[1] != idx[1]) {
      lbgramX  =  lmP->list.itemA[idx[0]].lbgramX;
      for ( i  =  lmP->list.itemA[idx[0]].fbgramX; i<= lbgramX; i++)
        if ( lmP->bgramA[i].wordX == idx[1]) break;

      if (  i <=  lbgramX) {
        bgramX =  i;
        segX   =  i >> lmP->tgramSegSize;

        w[0]   =  idx[0];
        w[1]   =  idx[1];

        if ( lmP->tgramXA[segX] < 0) lmP->tgramXA[segX] = tgramX;

        lmP->bgramA[bgramX].tgramX = (NGramIdx)(tgramX - lmP->tgramXA[segX]);

      } else {
        ERROR("Cannot find bigram '%s %s'.\n", segments[1], segments[2]);
        continue;
      }
    }

    /* --------------------- */
    /* Add trigram to LModel */
    /* --------------------- */

    lmP->tgramA[tgramX].wordX    =  idx[2];
    lmP->tgramA[tgramX].probX    =  probX;

    tgramX++;
  }

  if(0) /* Old and bogus */
    j = lmP->bgramA[lmP->bgramN].tgramX = tgramX;    
  else { /* New and hopefully not bugus */
    int bgX  = lmP->bgramN;
    int segX = bgX >> lmP->tgramSegSize;

    if ( lmP->tgramXA[segX] < 0) lmP->tgramXA[segX] = tgramX;

    j = lmP->bgramA[lmP->bgramN].tgramX = (NGramIdx)(tgramX - 
                                                lmP->tgramXA[segX]);   
  }
  for ( i = lmP->bgramN; i >= 0; i--) {
    if ( lmP->bgramA[i].tgramX != NO_NGRAM) j = lmP->bgramA[i].tgramX;
    else                                    lmP->bgramA[i].tgramX = j;
  }

  /* --------------- */
  /* Save FloatHeaps */
  /* --------------- */

  farrayInit( &lmP->tprobA,    probHeap->itemN);

  for ( i = 0; i < probHeap->itemN; i++)
    lmP->tprobA.itemA[i] = probHeap->itemA[i];

  if ( probHeap) { floatHeapFree( probHeap);    probHeap    = NULL; }

  if ( tgramX != tgramN) {
    ERROR( "Number of trigrams does not match.\n");
    ERROR( "Expected %d trigrams but found %d.\n", tgramN, tgramX);
    return TCL_ERROR;
  }

  if ( id) *id = lms; 

  INFO("Read 3-grams  tgramN=%d tprobN=%d.\n", tgramX, lmP->tprobA.itemN);

  return TCL_OK;
}

/* ------------------------------------------------------------------------
    lmodelReadUnigramSection
   ------------------------------------------------------------------------ */

int lmodelReadUnigramSection( LModel* lmP, FILE* fP, int ugramN, ArpaboLModelSection* id)
{
  char          line[1024];
  ArpaboLModelSection lms;
  int           n = 0;

  while ((lms = lmodelReadLine( lmP, fP, line, sizeof(line))) == LM_LINE) {

    char  word[1024];
    float prob;
    int   idx;

    /* ---------- */
    /* Parse line */
    /* ---------- */

    if ( sscanf( line, "%f %s", &prob, word) != 2) {
      if ( line[0] != '\n') INFO("Format error; unigram ignored: %s", line);
      continue;
    }

    /* -------------------------- */
    /* Try to find word in LModel */
    /* -------------------------- */

    if ( (idx = lmodelIndex( lmP, word)) < 0) {
      ERROR("Word '%s' does not exists in '%s'.\n", word, lmP->name);
      continue;
    }

    lmP->list.itemA[idx].prob[1]    = prob;
    n++;
  }

  if ( id) *id = lms; 
  INFO("Read unigrams ugramN=%d.\n", n);
  return TCL_OK;
}

/* ------------------------------------------------------------------------
    lmodelRead
   ------------------------------------------------------------------------ */

int lmodelRead( LModel* lmP, char* fileName)
{
  FILE*         fP = NULL;
  char          line[1024];
  ArpaboLModelSection lms;
  int           ugramN=0, bgramN=0, tgramN=0;
  int           rc = TCL_OK;

  if (! (fP = fileOpen( fileName,"r"))) {
    ERROR("Can't open file '%s' for reading.\n", fileName);
    return TCL_ERROR;
  }

  while ((lms  = lmodelReadLine( lmP, fP, line, sizeof(line))) == LM_LINE);
  while ( lms != LM_END && rc == TCL_OK) {

    switch ( lms) {

    case LM_DATA :

      /* ------------------------------- */
      /* Find the number of NGram counts */
      /* ------------------------------- */

      rc = lmodelReadNGramCounts( lmP, fP, &ugramN, &bgramN, &tgramN, &lms);
      break;

    case LM_1GRAMS :

      rc = lmodelReadUnigrams( lmP, fP, ugramN, &lms);
      break;

    case LM_2GRAMS :

      rc = lmodelReadBigrams(  lmP, fP, ugramN, bgramN, tgramN, &lms);
      break;

    case LM_3GRAMS :

      rc = lmodelReadTrigrams( lmP, fP, ugramN, bgramN, tgramN, &lms);
      break;

    case LM_UNIGRAMS :

      rc = lmodelReadUnigramSection( lmP, fP, ugramN, &lms);
      break;

    default :

      lms = lmodelReadLine( lmP, fP, line, sizeof(line));
    }
  }

  if ( rc != TCL_OK) {

    if ( lmP->bgramA)  { free( lmP->bgramA);  lmP->bgramA  = NULL; lmP->bgramN  = 0; }
    if ( lmP->tgramA)  { free( lmP->tgramA);  lmP->tgramA  = NULL; lmP->tgramN  = 0; }
    if ( lmP->tgramXA) { free( lmP->tgramXA); lmP->tgramXA = NULL; lmP->tgramXN = 0; }

    farrayDeinit( &lmP->bprobA);
    farrayDeinit( &lmP->bbackOffA);
    farrayDeinit( &lmP->tprobA);
  }

  fileClose( fileName, fP);  
  return rc;
}

int lmodelReadItf (ClientData cd, int argc, char *argv[])
{
  LModel* lmP      = (LModel *)cd;
  int     ac       =  argc - 1;
  char*   fileName =  NULL;

  if (itfParseArgv(argv[0], &ac, argv+1, 0, 
     "<filename>", ARGV_STRING, NULL, &fileName, cd, "file to read from", 
       NULL) != TCL_OK) return TCL_ERROR;

  return lmodelRead( lmP, fileName);
}

/* ------------------------------------------------------------------------
    lmodelScore    compute the score of the 3G language model for a
                   sequence of n words.
   ====================================
     WARNING: (see LModelLong)
   ------------------------------------------------------------------------ */

float lmodelScore( LModel* lmP, int* w, int n, int i)
{
  float prob = 0.0;
  int   j;

  if  ( i < 0) i = 0;
  for ( j = i; j < n; j++) {
    if (      j > 1) 
      prob += lmodelCacheTgScore(lmP->cacheP, w[j-2], w[j-1], w[j]);
    else if ( j > 0) {
      if ( w[j-1] == lmP->begItemX)
           prob += lmodelCacheTgScore(lmP->cacheP, lmP->begItemX, w[j-1], w[j]);
      else prob += lmodelCacheBgScore(lmP->cacheP, w[j-1], w[j]);
    }
    else
      prob += lmP->list.itemA[w[j]].prob[1];
  }
  return -lmP->lz * prob + (n-i) * lmP->lp;
}

int lmodelScoreItf( ClientData cd, int argc, char * argv[])
{
  LModel*   lmP    = (LModel *) cd;
  IArray    warray = {NULL, 0};
  int       ac     =  argc-1;
  int       i      =  0;

  if (itfParseArgv( argv[0], &ac,  argv+1,  0,
     "<word sequence>", ARGV_LIARRAY, NULL, &warray, 
     &(lmP->list), "sequence of words", 
     "-idx", ARGV_INT, NULL, &i, NULL, "index of cond. prob.",
     NULL) != TCL_OK) {
    if ( warray.itemA) free( warray.itemA);
    return TCL_ERROR;
  }

  itfAppendElement( "%f", lmodelScore(lmP, warray.itemA, warray.itemN, i));

  if ( warray.itemA) free( warray.itemA);
  return TCL_OK;
} 

/* ------------------------------------------------------------------------
    LModelSample   computes a random predecessor of a word
                   sequence of n words.
   ------------------------------------------------------------------------ */

int lmodelSample( LModel* lmP, int* w, int n)
{
  float prob = chaosRandom(0.0, 1.0);
  int * arr = malloc(sizeof(int)*(n+1));
  int i;

  for(i=0;i<n;i++)
    arr[i]=w[i];
		     
  for ( i = 0; i < lmP->list.itemN; i++) {
    float score;
    arr[n]=i;
    score = pow(10,-lmodelScore( lmP, arr, n+1, n));
    prob -= score;
    if(prob<=0) {
      free(arr);
      return i;
    }
  }
  WARN("Sampling probs do not sum to 1.0\n");
  free(arr);
  return 0;
}

int lmodelSampleItf( ClientData cd, int argc, char * argv[])
{
  LModel*   lmP    = (LModel *) cd;
  IArray    warray = {NULL, 0};
  int       ac     =  argc-1;
  /* int       i      =  0; */

  if(lmP->bgramN == 0) {
    ERROR("Language model has not been read yet.\n");	
    return TCL_ERROR;
  }

  if (itfParseArgv( argv[0], &ac,  argv+1,  0,
     "<word sequence>", ARGV_LIARRAY, NULL, &warray, 
     &(lmP->list), "sequence of words", 
     NULL) != TCL_OK) {
    if ( warray.itemA) free( warray.itemA);
    return TCL_ERROR;
  }

  itfAppendElement( "%d", lmodelSample(lmP, warray.itemA, warray.itemN));

  if ( warray.itemA) free( warray.itemA);
  return TCL_OK;
} 

/* ------------------------------------------------------------------------
    lmodelNextFrame   update cache frame index
   ------------------------------------------------------------------------ */

int lmodelNextFrame( LModel* lmP)
{
  lmP->cacheP->frameX++;
  return TCL_OK;
}

int lmodelNextFrameItf( ClientData cd, int argc, char * argv[])
{
  LModel*   lmP    = (LModel *) cd;
  int       ac     =  argc-1;

  if (itfParseArgv( argv[0], &ac,  argv+1,  0,
       NULL) != TCL_OK) return TCL_ERROR;

  return lmodelNextFrame(lmP);
} 

/* ------------------------------------------------------------------------
    lmodelReset   reset language model cache
   ------------------------------------------------------------------------ */

int lmodelReset( LModel* lmP)
{
  return lmodelCacheReset( lmP->cacheP);
}

int lmodelResetItf( ClientData cd, int argc, char * argv[])
{
  LModel*   lmP    = (LModel *) cd;
  int       ac     =  argc-1;

  if (itfParseArgv( argv[0], &ac,  argv+1,  0,
       NULL) != TCL_OK) return TCL_ERROR;

  return lmodelReset(lmP);
} 

/* ========================================================================
    Type Declaration
   ======================================================================== */

static Method lmodelItemMethod[] = 
{ 
  { "puts", lmodelItemPutsItf,   NULL },
  {  NULL,  NULL, NULL } 
};

TypeInfo lmodelItemInfo = { 
        "LModelItem", 0, -1, lmodelItemMethod, 
         NULL, NULL,
         lmodelItemConfigureItf, NULL,
         itfTypeCntlDefaultNoLink, 
        "3G Language Model Item\n" };

static Method lmodelMethod[] = 
{ 
  { "puts",   lmodelPutsItf,   "display the contents of an LModel" },
  { "read",   lmodelReadItf,   "reads a lmodel file"               },

  { "index",  lmodelIndexItf,  "return the internal index of an LModelItem" },
  { "name",   lmodelNameItf,   "return the name of an LModelItem" },
  { "score",  lmodelScoreItf,  "return the score of a text string"     },
  { "sample",  lmodelSampleItf, "return a randomly selected follower word"},
  { "nextFrame", lmodelNextFrameItf,  "incr. cache frame index"  },
  { "reset",     lmodelResetItf,      "reset language model"     },

  {  NULL,    NULL,           NULL } 
};

TypeInfo lmodelInfo = {
        "LModel", 0, -1, lmodelMethod, 
         lmodelCreateItf,lmodelFreeItf,
         lmodelConfigureItf,lmodelAccessItf, NULL,
	"3G Language Model\n" } ;

static LM lmodelLM = 
       { "LModel", &lmodelInfo, &lmodelInfo,
         (LMIndexFct*)    lmodelIndex,
         (LMNameFct *)    lmodelName,
	 (LMNameNFct*)    lmodelNameN,
         (LMScoreFct*)    lmodelScore,
         (LMNextFrameFct*)lmodelNextFrame,
         (LMResetFct*)    lmodelReset,
       NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL };

int LModel_Init( )
{
  static int lmodelInitialized = 0;

  if (! lmodelInitialized) {

    lmodelDefault.name            = NULL;
    lmodelDefault.useN            = 0;
    lmodelDefault.list.itemN      = 0;
    lmodelDefault.list.blkSize    = 1000;

    lmodelDefault.lz              = 1.0;
    lmodelDefault.lp              = 0.0;

    itfNewType (&lmodelItemInfo);
    itfNewType (&lmodelInfo);
    lmNewType(  &lmodelLM);

    lmodelInitialized = 1;
  }
  return TCL_OK;
}
