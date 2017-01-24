/* ========================================================================
    JanusRTk   Janus Recognition Toolkit
               ------------------------------------------------------------
               Object: Language Model --  n>3 possible
               ------------------------------------------------------------

    Author  :  Klaus Ries
    Module  :  lmodelLong.c
    Date    :  $Id: lmodelLong.c 3418 2011-03-23 15:07:34Z metze $
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
    Revision 1.5  2003/08/14 11:20:23  fuegen
    Merged changes on branch jtk-01-01-15-fms (jaguar -> ibis-013)

    Revision 1.4.4.9  2002/06/26 11:57:58  fuegen
    changes for new directory structure support and code cleaning

    Revision 1.4.4.8  2002/01/23 14:06:36  metze
    Warnings, go away

    Revision 1.4.4.7  2002/01/21 08:18:11  metze
    Cosmetic changes

    Revision 1.4.4.6  2001/11/19 14:46:27  metze
    Go away, boring message!

    Revision 1.4.4.5  2001/06/01 08:20:59  fuegen
    moved include of rand48.h for windows into common.h

    Revision 1.4.4.4  2001/05/23 08:10:23  metze
    PartFill included, cleaned up code

    Revision 1.4.4.3  2001/03/13 14:18:49  metze
    Beautyfying

    Revision 1.4.4.2  2001/03/05 08:40:28  metze
    Cleaned up treatment of exact LM in LMLA

    Revision 1.4.4.1  2001/02/27 15:10:28  metze
    LModelNJD and LMLA work with floats

    Revision 1.4  2001/01/15 09:49:59  janus
    Merged Christian's version with WINDOWS support (branch jtk-00-11-14-fuegen)

    Revision 1.3.4.1  2001/01/12 15:16:57  janus
    necessary changes for running janus under WINDOWS

    Revision 1.3  2000/11/14 12:32:23  janus
    Merging branch jtk-00-10-27-jmcd.

    Revision 1.2.30.2  2000/11/08 17:24:40  janus
    Finished making changes required for compilation under 'gcc -Wall'.

    Revision 1.2.30.1  2000/11/06 10:50:34  janus
    Made changes to enable compilation under -Wall.

    Revision 1.2  1998/06/23 11:37:30  kries
    *** empty log message ***

 * Revision 1.1  1998/05/26  18:27:23  kries
 * Initial revision
 *

   ======================================================================== */

#include <ctype.h>

#include "common.h"
#include "itf.h"
#include "error.h"
#include "lm.h"
#include "lmodelLong.h"
#include "ffmat.h"

char lmodelLongRCSVersion[]= 
           "@(#)1$Id: lmodelLong.c 3418 2011-03-23 15:07:34Z metze $";

/* ------------------------------------------------------------------------
    Forward Declaration
   ------------------------------------------------------------------------ */

extern TypeInfo LModelLongInfo;

typedef double HashKey;

/* ------------------------------------------------------------------------
    floatHeapHashKey & floatHeapHashCmp
   ------------------------------------------------------------------------ */

static long floatHeapHashKey (const HashKey *key)
{
  union { float fn; int ln; } f;
  /* union { double fn; long  ln; } f;
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

static int floatHeapInit( FloatHeap* fhP)
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

static FloatHeap* floatHeapCreate( )
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

static int floatHeapDeinit( FloatHeap *fhP)
{
  assert( fhP);
  return listDeinit((List*)fhP);
}

static int floatHeapFree( FloatHeap* fhP)
{
  if ( floatHeapDeinit(fhP)!=TCL_OK) return TCL_ERROR;
  free(fhP);  
  return TCL_OK;
}
/*
static int floatHeapFreeItf(ClientData cd)
{
  return floatHeapFree((FloatHeap*)cd);
}
*/
/* ------------------------------------------------------------------------
    floatHeapAdd
   ------------------------------------------------------------------------ */

static int floatHeapAdd( FloatHeap *fhP, double x)
{
  double y   = x;
  int   idx  = listIndex((List*)fhP, (ClientData)&y, 1);
  if (  idx >= 0) fhP->itemA[idx] = y;
  return idx;
}

/* ========================================================================
    LModelLongItem
   ======================================================================== */
/* ------------------------------------------------------------------------
    LModelLongItemInit
   ------------------------------------------------------------------------ */

int LModelLongItemInit( LModelLongItem* lmiP, ClientData CD)
{
  assert( lmiP);

  lmiP->name       =  ( CD) ? strdup((char*)CD) : NULL;
  lmiP->prob[0]    =    0.0;
  lmiP->prob[1]    =    0.0;
  lmiP->backOff    =    0.0;
  lmiP->nngramX    =   -1;

  return TCL_OK;
}

/* ------------------------------------------------------------------------
    LModelLongItemDeinit
   ------------------------------------------------------------------------ */

int LModelLongItemDeinit( LModelLongItem* lmiP)
{
  if (!lmiP) return TCL_OK;
  if ( lmiP->name)   { free(lmiP->name);   lmiP->name   = NULL; }
  return TCL_OK;
}

/* ------------------------------------------------------------------------
    LModelLongItemConfigureItf
   ------------------------------------------------------------------------ */

static int LModelLongItemConfigureItf( ClientData cd, char *var, char *val)
{
  LModelLongItem* lmiP = (LModelLongItem*)cd;
  if (! lmiP) return TCL_ERROR;

  if (! var) {
    ITFCFG(LModelLongItemConfigureItf,cd,"name");
    ITFCFG(LModelLongItemConfigureItf,cd,"prob0");
    ITFCFG(LModelLongItemConfigureItf,cd,"prob1");
    ITFCFG(LModelLongItemConfigureItf,cd,"backOff");
    ITFCFG(LModelLongItemConfigureItf,cd,"nngramX");
    ITFCFG(LModelLongItemConfigureItf,cd,"frameX");
    return TCL_OK;
  }
  ITFCFG_CharPtr( var,val,"name",         lmiP->name,       1);
  ITFCFG_Float(   var,val,"prob0",        lmiP->prob[0],    0);
  ITFCFG_Float(   var,val,"prob1",        lmiP->prob[1],    0);
  ITFCFG_Float(   var,val,"backOff",      lmiP->backOff,    0);
  ITFCFG_Int(     var,val,"nngramX",      lmiP->nngramX,    1);
  ITFCFG_Int(     var,val,"frameX",      lmiP->frameX,    1);
  ERROR("Unknown option '-%s %s'.\n", var, val ? val : "?");
  return TCL_ERROR;
}

/* ------------------------------------------------------------------------
    LModelLongItemPutsItf
   ------------------------------------------------------------------------ */

int LModelLongItemPuts( LModelLongItem* LModelLongItem)
{
  itfAppendElement("NAME");
  itfAppendElement("%s", LModelLongItem->name);
  return TCL_OK;
}

static int LModelLongItemPutsItf( ClientData cd, int argc, char *argv[])
{
  int   ac   =  argc-1;
  LModelLongItem* lmodelItem = (LModelLongItem*)cd;

  if ( itfParseArgv( argv[0], &ac, argv+1, 0, NULL) != TCL_OK)
       return TCL_ERROR;

  return LModelLongItemPuts( lmodelItem);
}

/* ========================================================================
    LModelLong
   ======================================================================== */

static LModelLong LModelLongDefault;

/* ------------------------------------------------------------------------
    Create LModelLong Structure
   ------------------------------------------------------------------------ */

int LModelLongInit( LModelLong* lmP, char* name, int cacheN)
{
  assert( name);
  assert( lmP);

  lmP->name   = strdup(name);
  lmP->useN   = 0;

  if ( listInit((List*)&(lmP->list), itfGetType("LModelLongItem"), 
                                          sizeof(LModelLongItem),
                                 LModelLongDefault.list.blkSize) != TCL_OK) {
    ERROR("Couldn't allocate unigram list.");
    return TCL_ERROR;
  }

  lmP->list.init    = (ListItemInit  *)LModelLongItemInit;
  lmP->list.deinit  = (ListItemDeinit*)LModelLongItemDeinit;


  /* Disable all caching for now */

  /*
  if (!(lmP->cacheP  = LModelLongCacheCreate( lmP, cacheN, 1))) {
    ERROR("Couldn't allocate cache.");
    listDeinit((List*)&(lmP->list));
    return TCL_ERROR;
  }

  */

  lmP->n               = 0;
  lmP->ngrams          = NULL;

  lmP->tgramA          = NULL;
  lmP->tgramN          = 0;
  lmP->tprobA.itemN    = 0;
  lmP->tprobA.itemA    = NULL;

  lmP->ngramSegSize    = LModelLongDefault.ngramSegSize;

  lmP->lz              = LModelLongDefault.lz;
  lmP->lp              = LModelLongDefault.lp;

  lmP->begItemX        = -1;
  lmP->endItemX        = -1;

  return TCL_OK;
}

LModelLong* LModelLongCreate( char* name, int cacheN)
{
  LModelLong *lmP = (LModelLong*)malloc(sizeof(LModelLong));

  if (! lmP || LModelLongInit( lmP, name, cacheN) != TCL_OK) {
     if ( lmP) free( lmP);
     ERROR("Failed to allocate LModelLong object '%s'.\n", name);
     return NULL; 
  }
  return lmP;
}

static ClientData LModelLongCreateItf (ClientData cd, int argc, char *argv[])
{
  int         ac         = argc-1;
  float       lz         = LModelLongDefault.lz;
  float       lp         = LModelLongDefault.lp;
  LModelLong* lmP        = NULL;
  int         cacheLineN = 200;
  
  if ( itfParseArgv(argv[0], &ac, argv+1, 0, 
      "-cacheLineN",  ARGV_INT,    NULL, &cacheLineN, NULL,
      "number of cache lines for Lm",
      "-lz", ARGV_FLOAT, NULL, &lz, NULL, "language model weight",
      "-lp", ARGV_FLOAT, NULL, &lp, NULL, "insertion penalty",
       NULL) != TCL_OK) return NULL;

  if ( (lmP = LModelLongCreate(argv[0], cacheLineN)) ) {
    lmP->lz = lz;
    lmP->lp = lp;
  }
  return (ClientData)lmP;
}

/* ------------------------------------------------------------------------
    Free LModelLong Structure
   ------------------------------------------------------------------------ */

int LModelLongLinkN( LModelLong* LModelLong)
{
  int  useN = listLinkN((List*)&(LModelLong->list)) - 1;
  if ( useN < LModelLong->useN) return LModelLong->useN;
  else                             return useN;
}

int LModelLongDeinit (LModelLong *lmP)
{
  int i;

  assert( lmP);

  if ( LModelLongLinkN( lmP)) {
    SERROR("LModelLong '%s' still in use by other objects.\n", lmP->name);
    return TCL_ERROR;
  } 

  if (lmP->name)    { free(lmP->name);    lmP->name    = NULL; };

  if(lmP->ngrams) {
    for(i=0;i<lmP->n-2;i++) {
      free(lmP->ngrams[i].ngramA);
      free(lmP->ngrams[i].ngramXA);
      farrayDeinit(&(lmP->ngrams[i].nprobA));
      farrayDeinit(&(lmP->ngrams[i].nbackOffA));
    }    
    free(lmP->ngrams);
    lmP->ngrams=NULL;
  }

  /* 
  if (lmP->cacheP)  { LModelLongCacheFree( lmP->cacheP); lmP->cacheP = NULL; }
  */

  farrayDeinit( &lmP->tprobA);

  return listDeinit((List*)&(lmP->list));
}

int LModelLongFree (LModelLong *lmP)
{
  if (LModelLongDeinit(lmP)!=TCL_OK) return TCL_ERROR;

  free(lmP);
  return TCL_OK;
}

static int LModelLongFreeItf(ClientData cd)
{
  return LModelLongFree((LModelLong*)cd);
}

/* ------------------------------------------------------------------------
    LModelLongConfigureItf
   ------------------------------------------------------------------------ */

static int LModelLongConfigureItf (ClientData cd, char *var, char *val)
{
  LModelLong* lmodel = (LModelLong*)cd;
  if (! lmodel) lmodel = &LModelLongDefault;

  if (! var) {
    ITFCFG(LModelLongConfigureItf,cd,"useN");
    ITFCFG(LModelLongConfigureItf,cd,"lz");
    ITFCFG(LModelLongConfigureItf,cd,"lp");
    ITFCFG(LModelLongConfigureItf,cd,"n");
    ITFCFG(LModelLongConfigureItf,cd,"begItemX");
    ITFCFG(LModelLongConfigureItf,cd,"endItemX");
    ITFCFG(LModelLongConfigureItf,cd,"ngramSegSize");
    return listConfigureItf((ClientData)&(lmodel->list), var, val);
  }
  ITFCFG_Int(    var,val,"useN",     lmodel->useN,             1);
  ITFCFG_Float(  var,val,"lz",       lmodel->lz,               0);
  ITFCFG_Float(  var,val,"lp",       lmodel->lp,               0);
  ITFCFG_Int(    var,val,"n",        lmodel->n,                0);
  ITFCFG_Int(    var,val,"begItemX", lmodel->begItemX,         0);
  ITFCFG_Int(    var,val,"endItemX", lmodel->endItemX,         0);
  ITFCFG_Int(    var,val,"ngramSegSize", lmodel->ngramSegSize,0);
  return listConfigureItf((ClientData)&(lmodel->list), var, val);   
}

/* ------------------------------------------------------------------------
    LModelLongAccessItf
   ------------------------------------------------------------------------ */

static ClientData LModelLongAccessItf (ClientData cd, char *name, TypeInfo **ti)
{ 
  LModelLong* lmodel = (LModelLong*)cd;


  /* KR: No ITF access right now ... so what */

  if (*name == '.') { 
    if (name[1]=='\0') {
      /*
       itfAppendElement( "nprobA");
       itfAppendElement( "nbackOffA");
       */
       *ti = NULL;
    }
    else { 
      /*
      if (! strcmp( name+1, "nprobA")) {
        *ti = itfGetType("FArray");
        return (ClientData)&(LModelLong->nprobA);
      }
      else if (! strcmp( name+1, "nbackOffA")) {
        *ti = itfGetType("FArray");
        return (ClientData)&(LModelLong->nbackOffA);
      }
      */
    }
  }
  return listAccessItf((ClientData)&(lmodel->list),name,ti);
}

/* ------------------------------------------------------------------------
    LModelLongPutsItf
   ------------------------------------------------------------------------ */

static int LModelLongPutsItf (ClientData cd, int argc, char *argv[])
{
  LModelLong *lmodel  = (LModelLong*)cd;
  return listPutsItf((ClientData)&(lmodel->list), argc, argv);
}

/* ------------------------------------------------------------------------
    LModelLongIndex
   ------------------------------------------------------------------------ */

int LModelLongIndex( LModelLong* LModelLong,  char* name) 
{
  return listIndex((List*)&(LModelLong->list), name, 0);
}

int LModelLongIndexItf( ClientData clientData, int argc, char *argv[] )
{
  LModelLong* lmodel = (LModelLong*)clientData;
  
  return listName2IndexItf((ClientData)&(lmodel->list), argc, argv);
}

/* ------------------------------------------------------------------------
    LModelLongName
   ------------------------------------------------------------------------ */

char* LModelLongName( LModelLong* LModelLong, int i) 
{
  return (i < 0) ? "(null)" : LModelLong->list.itemA[i].name;
}

static int LModelLongNameItf( ClientData clientData, int argc, char *argv[] )
{
  LModelLong* lmP = (LModelLong*)clientData;
  return listIndex2NameItf((ClientData)&(lmP->list), argc, argv);
}

/* ------------------------------------------------------------------------
    lmodelNameN
   ------------------------------------------------------------------------ */

int LModelLongNameN( LModelLong* lmodel) 
{
  return lmodel->list.itemN;
}


/* ------------------------------------------------------------------------
    LModelLongFindBGram  do a binary search to find a bigram in the list of
                     bigrams <w1,*>.
   ------------------------------------------------------------------------ */


int LModelLongFindNGram( LModelLong* lmP, int n, int * arr, int * searchX)
{
  int bg, lbg, nX;
  int           sentinel_store;
  const int     cmp  = lmP->list.itemA[arr[n-1]].class;

  if(n==1) {     /* Direct mapping */
    *searchX=cmp;
    return 1;
  }
  else if(n==2) {  /* boundaries from unigram */
    *searchX=lmP->list.itemA[arr[0]].class;
    bg      = lmP->list.itemA[*searchX].nngramX;
    if(*searchX<lmP->listN-1)
      lbg     = lmP->list.itemA[*searchX+1].nngramX-1;
    else if(n==lmP->n)
      lbg     = lmP->tgramN-1;
    else
      lbg     = lmP->ngrams[0].ngramN-1;
  }
  else {         /* Search for boundaries and resvolve two level addressing */
    int segmentOffset;
    nX = LModelLongFindNGram( lmP, n-1, arr, searchX);
    if(nX!=n-1)
      return nX;
    
    segmentOffset = lmP->ngrams[n-3].ngramXA[*searchX>>lmP->ngramSegSize];

    bg      = lmP->ngrams[n-3].ngramA[*searchX].nngramX;
    bg     += lmP->ngrams[n-3].ngramXA[*searchX>>lmP->ngramSegSize];

    lbg     = lmP->ngrams[n-3].ngramA[1+*searchX].nngramX-1;
    lbg    += lmP->ngrams[n-3].ngramXA[(1+*searchX)>>lmP->ngramSegSize];
  }  

  /* Find the index in the array using binary search */
  /* Use linear search if array is small enough */

  if(n<lmP->n) { /* Not the final array */
    NgramItem * ngramA = lmP->ngrams[n-2].ngramA;
    
    while ( bg < lbg - 8) {                               /* Do binary search */ 
      int tmp = ( bg + lbg) / 2;
      if ( cmp < ngramA[tmp].wordX) lbg = tmp-1;
      else                          bg  = tmp;
    }

    if(bg>lbg)
      return n-1;
    sentinel_store=ngramA[lbg].wordX; ngramA[lbg].wordX=cmp; /* Insert sentinel  */    
    while(ngramA[bg++].wordX < cmp);                         /* Do linear search */    
    ngramA[lbg].wordX=sentinel_store;                        /* Remove sentinel  */
    
    if(ngramA[--bg].wordX == cmp) {
      *searchX=bg; return n;
    }
  }
  else {          /* final array */
    TrigramItemLong * ngramA = lmP->tgramA;
    
    while ( bg < lbg - 8) {
      int tmp = ( bg + lbg) / 2;
      if ( cmp < ngramA[tmp].wordX) lbg = tmp-1;
      else                          bg  = tmp;
    }

    if(bg>lbg)
      return n-1;
    sentinel_store=ngramA[lbg].wordX; ngramA[lbg].wordX=cmp; /* Insert sentinel  */    
    while(ngramA[bg++].wordX < cmp);                         /* Do linear search */    
    ngramA[lbg].wordX=sentinel_store;                        /* Remove sentinel  */
    
    if(ngramA[--bg].wordX == cmp) {
      *searchX=bg; return n;
    }
  }
  return n-1;
}

/* WARNING: =======================================================
   (LModel and LModelLong)

   The Trigram <w1,w2,w3>, if you haven't seen any tri- or
   bigrams, should be modelled as [w1]+[w2]+<w3> ([] = backoff,
   <> = score), however, these procedures model it as [w2]+<w3>.
   Use LModelNJD if you're concerned about this
   ================================================================ */

float LModelLongProbNGramClass( LModelLong* lmP, int n, int * arr)
{
  int searchX, nX;
  float alpha = 0;

  nX = LModelLongFindNGram (lmP, n, arr, &searchX);

  if (n == 1) {
    return lmP->list.itemA[searchX].prob[0];
  }

  if(nX == n-1) { /* backoff && alpha != 0 */    
    alpha = (nX == 1) ?
      lmP->list.itemA[searchX].backOff: 
      lmP->ngrams[nX-2].nbackOffA.itemA[lmP->ngrams[nX-2].ngramA[searchX].backOffX];
  }

  if(nX<=n-1)   /* backoff */
    return alpha + LModelLongProbNGramClass( lmP, n-1, arr+1);

  if(n==lmP->n) /* Highest distribution */
    return lmP->tprobA.itemA[lmP->tgramA[searchX].probX];
  else
    return lmP->ngrams[n-2].nprobA.itemA[lmP->ngrams[n-2].ngramA[searchX].probX];
}

float LModelLongProbNGram( LModelLong* lmP, int n, int * arr)
{
  return LModelLongProbNGramClass( lmP, n, arr) + lmP->list.itemA[arr[n-1]].penalty;
}

/* ------------------------------------------------------------------------
    LModelLongReadLine
   ------------------------------------------------------------------------ */

static int LModelLongReadLine_N=0;

ArpaboLModelSection LModelLongReadLine( LModelLong* lmP, FILE* fP, char* line, int size)
{
  LModelLongReadLine_N=0;

  if (! fgets(  line, size, fP) || feof (fP)) return LM_END;

  if (! strcmp( line, "\\data\\\n"))    return LM_DATA;
  if (! strcmp( line, "\\end\\\n"))     return LM_END;
  if ( 0 < sscanf( line, "\\%d-grams:\n", &LModelLongReadLine_N ))  return LM_NGRAMS;
  if (! strcmp( line, "\\unigrams:\n")) return LM_UNIGRAMS;
  if (! strcmp( line, "\\subsmodel:\n")) return LM_SUBS;
  if(!strncmp( line, "\\",1)) INFO("Unknown segment %s",line);
  return LM_LINE;
}

/* ------------------------------------------------------------------------
    LModelLongReadNGramCounts  find the number of 1..n-grams in the LM file
                           and builds the ngrams table
   ------------------------------------------------------------------------ */

int LModelLongReadNGramCounts( LModelLong* lmP, FILE* fP, ArpaboLModelSection* id)
{
  ArpaboLModelSection lms = LM_END;
  char          line[1024];
  int		ngramN[1000];
  int           n=0;
  int i;


  while ((lms = LModelLongReadLine( lmP, fP, line, sizeof(line))) == LM_LINE) {

    int  ngram;
    long count;

    /* ---------- */
    /* Parse line */
    /* ---------- */

    if ( sscanf( line, "ngram %d=%ld", &ngram, &count) != 2) {
      if ( line[0] != '\n')
	INFO("Format error; \\data\\ line ignored: %s", line);
      continue;
    }

    n++;
    if(n!=ngram) {
      ERROR("\\data section not ordered n=%d ngram=%d\n",n,ngram); return TCL_ERROR;
    }
    ngramN[n-1]=count;
  }
  
  lmP->n=n;
  lmP->listN = ngramN[0];
  lmP->tgramN = ngramN[n-1];

  if(n>2) {
    if(0 == (lmP->ngrams = malloc((n-2)*sizeof(Ngram)))) {
      ERROR("Could not allocate memory for ngram basetable\n");
      return TCL_ERROR;
    }
    for(i=0;i<n-2;i++) {
      lmP->ngrams[i].ngramN=ngramN[i+1];
      lmP->ngrams[i].ngramA=NULL;
      lmP->ngrams[i].ngramXN=0;
      lmP->ngrams[i].ngramXA=NULL;
      lmP->ngrams[i].nprobA.itemA=NULL;
      lmP->ngrams[i].nprobA.itemN=0;
      lmP->ngrams[i].nbackOffA.itemA=NULL;
      lmP->ngrams[i].nbackOffA.itemN=0;
    }
  }

  INFO("Reading a %d-gram model\n",lmP->n);
  for(i=1;i<=lmP->n;i++) {
    if(i==1)
      n=lmP->listN;
    else if(i==lmP->n)
      n=lmP->tgramN;
    else
      n=lmP->ngrams[i-2].ngramN;
	
    INFO("%d-gram: %d\n",i,n);
  }

  if ( id) *id = lms; 
  return TCL_OK;
}

/* ------------------------------------------------------------------------
    LModelLongReadNgrams
   ------------------------------------------------------------------------ */

int LModelLongReadNgrams( LModelLong* lmP, FILE* fP, int N, ArpaboLModelSection* id)
{
  char          line[1024];
  ArpaboLModelSection lms;
  FloatHeap*    probHeap    = NULL;
  FloatHeap*    backOffHeap = NULL;
  int           lowerX = 0, nextX;
  int           i,j;
  int           ngram[1024];

  int           idx=0;          /* current array index to enter */
  int           expected_idx;   /* the number indices to expect */

  char      *   segments[1024]; /* the scan of the corresponding line */
  char      *   scanner;

  float         prob, backOff;
  int           probX, backOffX;
  

  if(N==1)	
    expected_idx=lmP->listN;
  else if(N==lmP->n)
    expected_idx=lmP->tgramN;
  else 
    expected_idx=lmP->ngrams[N-2].ngramN;

  if(N>1) {
  
    /* -------------------- */
    /* Allocate Float Heaps */
    /* -------------------- */

    if (! (probHeap    = floatHeapCreate()))
      return TCL_ERROR;

    /* -------------------- */
    /* Allocate Tables      */
    /* -------------------- */

    if(N==lmP->n) {

      /* -------------------- */
      /*  Allocate Tables     */
      /* -------------------- */

      if (lmP->tgramA) {
	ERROR("%d-gram table already exists. ngramN = %d.\n", N, lmP->tgramN);
	return TCL_ERROR;
      }

      if (!(lmP->tgramA = (TrigramItemLong*)malloc((lmP->tgramN+1) * sizeof(TrigramItemLong)))) {
	ERROR("Memory overflow. Trigram table too big, tgramN = %d.\n", lmP->tgramN);
	return TCL_ERROR;
      }
    }
    else {

      /* -------------------- */
      /*  Allocate Backoff    */
      /* -------------------- */      
 
      if (! (backOffHeap    = floatHeapCreate())) {
	floatHeapFree(   probHeap);    probHeap    = NULL;
	return TCL_ERROR;
      }
      

      /* -------------------------- */
      /* Allocate space for Ngrams  */
      /* -------------------------- */

      if ( lmP->ngrams[N-2].ngramA) {
	ERROR("%d-gram table already exists. ngramN = %d.\n", N, lmP->ngrams[N-2].ngramN);
	return TCL_ERROR;
      }

      if (!(lmP->ngrams[N-2].ngramA = (NgramItem*)malloc((lmP->ngrams[N-2].ngramN+1) * sizeof(NgramItem)))) {
	ERROR("Memory overflow. %ld-gram table too big (%d elements).\n", N, lmP->ngrams[N-2].ngramN);
	return TCL_ERROR;
      }

      for( i = 0; i <  lmP->ngrams[N-2].ngramN+1; i++ ) lmP->ngrams[N-2].ngramA[i].nngramX=(NGramIdx)-1;      

      /* ------------------------------------- */
      /* Allocate space for 2-level addressing */
      /* ------------------------------------- */

      lmP->ngrams[N-2].ngramXN = (lmP->ngrams[N-2].ngramN+1) / (1 << (lmP->ngramSegSize-1)) + 1;
      if (!(lmP->ngrams[N-2].ngramXA = (int*)malloc((lmP->ngrams[N-2].ngramXN+1) * sizeof(int)))) {
	ERROR("Memory overflow. %d-gram reference table too big (%d).\n", N, lmP->ngrams[N-2].ngramXN);
	return TCL_ERROR;
      } else {
	int i;
	for ( i = 0; i < lmP->ngrams[N-2].ngramXN; i++) lmP->ngrams[N-2].ngramXA[i] = -1;
      }
    }
  }

  while (((lms = LModelLongReadLine( lmP, fP, line, sizeof(line))) == LM_LINE) && (idx<expected_idx) ) {

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

    if( !((N + 1==i) || (N!=lmP->n && N+2==i))) {
      for(scanner=line;i>1;scanner++) 
	if(*scanner=='\000') {
	  *scanner=' '; i--;
	}
      ERROR("Current line invaliad, skipping: %s\n",line);
      continue;
    }
 
    /* ----------------------------------- */
    /* Evaluate the arguments of the scan  */
    /* ----------------------------------- */

    prob=atof(segments[0]);
    backOff = N+2==i ? atof(segments[N+1]) : 0;

    for(i=0;i<N;i++) {
      /* -------------- */
      /* Map word to id */
      /* -------------- */

      ngram[i] = LModelLongIndex( lmP, segments[i+1]);
      if(N==1) {
	if (ngram[i] >= 0) {
	  ERROR("Word '%s' already exists in '%s'.\n", segments[i+1], lmP->name);
	  goto LModelLongReadNgrams_readloop;
	}	
	ngram[i] = listNewItem((List*)&(lmP->list), (ClientData)segments[i+1]);
      }
      else {
	if (ngram[i] < 0) {
	  ERROR("Word '%s' does not exist in '%s'.\n", segments[i+1], lmP->name);
	  goto LModelLongReadNgrams_readloop;
	}	
      }   
    }   
    
    for(scanner=line;i>0;scanner++) 
      if(*scanner=='\000') {
	*scanner=' '; i--;
      }

    if(N==1) {
      /* -------------------------------------- */
      /* Modify prob of <s> and backOff of </s> */
      /* -------------------------------------- */

      if (! strcmp( segments[i+1], "</s>")) { lmP->endItemX = ngram[0]; backOff = SMALLPROB; }
      if (! strcmp( segments[i+1], "<s>"))  { lmP->begItemX = ngram[0]; prob    = SMALLPROB; }
      lmP->list.itemA[idx].prob[0]    = prob;
      lmP->list.itemA[idx].prob[1]    = prob;
      lmP->list.itemA[idx].backOff    = backOff;
      lmP->list.itemA[idx].nngramX    = -1;
      lmP->list.itemA[idx].class      = idx;
      lmP->list.itemA[idx].penalty    = 0;
    }
    else {
      /* -------------------- */
      /* Add Ngram to LModelLong  */
      /* -------------------- */

      /* -------------------- */
      /* Add Ngram to Tree    */
      /* -------------------- */

      if(N==2) {	
	if ( lmP->list.itemA[ngram[0]].nngramX < 0)
	  lmP->list.itemA[ngram[0]].nngramX =  idx;
      }
      else {
	if(LModelLongFindNGram( lmP, N-1, ngram, &lowerX)!=N-1) {
	  ERROR("ARPABO file is not a tree, ignoring line %s\n",line);
	  goto LModelLongReadNgrams_readloop;
	}

	if ( lmP->ngrams[N-3].ngramA[lowerX].nngramX == (NGramIdx)-1 ) {
	  if ( lmP->ngrams[N-3].ngramXA[lowerX >> lmP->ngramSegSize] < 0)
	    lmP->ngrams[N-3].ngramXA[lowerX >> lmP->ngramSegSize] = idx;
	  lmP->ngrams[N-3].ngramA[lowerX].nngramX =
	    (NGramIdx)(idx - lmP->ngrams[N-3].ngramXA[lowerX >> lmP->ngramSegSize]);
	  if(lmP->ngrams[N-3].ngramA[lowerX].nngramX+lmP->ngrams[N-3].ngramXA[lowerX >> lmP->ngramSegSize] != idx) {
	    FATAL("Segmentsize %ld is too small. Segment: %ld 2-level: %ld Index: %ld, line: %s\n",lmP->ngramSegSize,
		  lmP->ngrams[N-3].ngramXA[lowerX >> lmP->ngramSegSize],lmP->ngrams[N-3].ngramA[lowerX].nngramX,idx,line);
	  }
	}
      }

      /* ------------------------------------ */
      /*  Change <s> <s> ... <s> to SMALLPROB */
      /* ------------------------------------ */

      for(i=0;i<N && lmP->begItemX==ngram[i];i++);
      if(i==N)
	prob=SMALLPROB;

      /* -------------------- */
      /*  Add Ngram values    */
      /* -------------------- */

      if ((probX    = floatHeapAdd( probHeap,    prob)) > MAX_FLOATX) {
	ERROR("FloatHeap Overeflow: %s", line);
	goto LModelLongReadNgrams_readloop;
      }

      if(N==lmP->n) {
	lmP->tgramA[idx].wordX=ngram[N-1];
	lmP->tgramA[idx].probX=probX;
      }
      else {
	if ((backOffX = floatHeapAdd( backOffHeap, backOff)) > MAX_FLOATX) {
	  ERROR("FloatHeap Overflow: %s", line);
	  goto LModelLongReadNgrams_readloop;
	}
	lmP->ngrams[N-2].ngramA[idx].wordX=ngram[N-1];
	lmP->ngrams[N-2].ngramA[idx].probX=probX;
	lmP->ngrams[N-2].ngramA[idx].backOffX=backOffX;
      }
    }
    idx++;
    LModelLongReadNgrams_readloop:
    idx = idx;
  }

  /* ------------ */
  /* Set Endpoint */
  /* ------------ */

  if(N>1) {
    if(N==lmP->n)
      lmP->tgramA[idx].wordX=0;
    else 
      lmP->ngrams[N-2].ngramA[idx].wordX=0;
  }

  /* --------------- */
  /* Save FloatHeaps */
  /* --------------- */

  if(N>1) {
    if(N<lmP->n) {
      farrayInit( &lmP->ngrams[N-2].nprobA,    probHeap->itemN);
      for ( i = 0; i < probHeap->itemN; i++)
	lmP->ngrams[N-2].nprobA.itemA[i] = probHeap->itemA[i];

      farrayInit( &lmP->ngrams[N-2].nbackOffA, backOffHeap->itemN);
      for ( i=0; i < backOffHeap->itemN; i++)
	lmP->ngrams[N-2].nbackOffA.itemA[i] = backOffHeap->itemA[i];
    }
    else if(N==lmP->n) {
      farrayInit( &lmP->tprobA,    probHeap->itemN);
      for ( i=0; i < probHeap->itemN; i++)
	lmP->tprobA.itemA[i] = probHeap->itemA[i];
    }
  }    

  /* --------------- */
  /* Fillup Tree     */
  /* --------------- */

  if(N>2) { /* In the unigram table we make a special assumption */
    for(i=0;i<lmP->ngrams[N-3].ngramN;i++) {     
      if(lmP->ngrams[N-3].ngramA[i].nngramX == (NGramIdx)-1) {
	for(j=i+1;j<lmP->ngrams[N-3].ngramN && lmP->ngrams[N-3].ngramA[j].nngramX==(NGramIdx)-1; j++);
	nextX = ( j == lmP->ngrams[N-3].ngramN ) ? idx :
	  (lmP->ngrams[N-3].ngramA[j].nngramX + lmP->ngrams[N-3].ngramXA[j >> lmP->ngramSegSize]);
	for(;i<j;i++) {
	  if ( lmP->ngrams[N-3].ngramXA[i >> lmP->ngramSegSize] < 0)
	    lmP->ngrams[N-3].ngramXA[i >> lmP->ngramSegSize] = nextX;
	  lmP->ngrams[N-3].ngramA[i].nngramX = (NGramIdx)(nextX-lmP->ngrams[N-3].ngramXA[i >> lmP->ngramSegSize]);
	  if(lmP->ngrams[N-3].ngramA[i].nngramX+lmP->ngrams[N-3].ngramXA[i >> lmP->ngramSegSize] != nextX) {
	    FATAL("Segmentsize %ld is too small\n",lmP->ngramSegSize);
	  }
	}
	i--; /* Compensate for i++ in the main loop */
      }
    }	

    assert(i==lmP->ngrams[N-3].ngramN);
    if ( lmP->ngrams[N-3].ngramXA[i >> lmP->ngramSegSize] < 0)
      lmP->ngrams[N-3].ngramXA[i >> lmP->ngramSegSize] = idx;
    lmP->ngrams[N-3].ngramA[i].nngramX = (NGramIdx)(idx-lmP->ngrams[N-3].ngramXA[i >> lmP->ngramSegSize]);
    if(lmP->ngrams[N-3].ngramA[i].nngramX+lmP->ngrams[N-3].ngramXA[i >> lmP->ngramSegSize] != idx) {
      FATAL("Segmentsize %ld is too small\n",lmP->ngramSegSize);
    }
  } else if(N==2) {
    for(i=0;i<lmP->listN;i++) {
      if(lmP->list.itemA[i].nngramX<0) {
	for(j=i+1; j<lmP->listN && lmP->list.itemA[j].nngramX < 0;j++);
	nextX = ( j == lmP->listN ) ? idx : lmP->list.itemA[j].nngramX;
	for(;i<j;i++) {
	  lmP->list.itemA[i].nngramX = nextX;
	}	
	i--;	/* Compensate for i++ in the main loop */
      }
    }	  
  }      
    
  if ( probHeap)    { floatHeapFree( probHeap);    probHeap    = NULL; }
  if ( backOffHeap) { floatHeapFree( backOffHeap); backOffHeap = NULL; }

  if ( expected_idx != idx) {
    if(idx<expected_idx) {
      INFO( "Expected %d %d-grams but found %d.\n", expected_idx, N,idx);
      
    }
    else {
      ERROR( "Number of %d-grams does not match.\n",N);
      ERROR( "Expected %d-grams but found %d.\n", expected_idx, idx);      
      return TCL_ERROR;
    }
  }

  if ( id) *id = lms; 

  return TCL_OK;

}

/* ------------------------------------------------------------------------
    LModelLongReadUnigramSection
   ------------------------------------------------------------------------ */

int LModelLongReadUnigramSection( LModelLong* lmP, FILE* fP, ArpaboLModelSection* id)
{
  char          line[1024];
  ArpaboLModelSection lms;
  int           n = 0;

  while ((lms = LModelLongReadLine( lmP, fP, line, sizeof(line))) == LM_LINE) {

    char  word[1024];
    float prob;
    int   idx;

    /* ---------- */
    /* Parse line */
    /* ---------- */

    if ( sscanf( line, "%f %s", &prob, word) != 2) {
      if ( line[0] == '\n')
	break;
      INFO("Format error; unigram ignored: %s", line);
    }

    /* -------------------------- */
    /* Try to find word in LModelLong */
    /* -------------------------- */

    if ( (idx = LModelLongIndex( lmP, word)) < 0) {
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
    LModelLongReadSubsSection
   ------------------------------------------------------------------------ */

int LModelLongReadSubsSection( LModelLong* lmP, FILE* fP, ArpaboLModelSection* id)
{
  char          line[1024];
  ArpaboLModelSection lms;
  int           n = 0;

  while ((lms = LModelLongReadLine( lmP, fP, line, sizeof(line))) == LM_LINE) {

    char  word1[1024], word2[1024];
    float prob;
    int   idx1,idx2;

    /* ---------- */
    /* Parse line */
    /* ---------- */

    if ( sscanf( line, "%f %s %s", &prob, word1,word2) != 3) {
      if ( line[0] == '\n')
	continue;
      INFO("Format error; line ignored: %s", line);
    }

    /* -------------------------- */
    /* Try to find word in LModelLong */
    /* -------------------------- */
    
    if ( (idx1 = LModelLongIndex( lmP, word1)) < 0) {
      idx1 = listNewItem((List*)&(lmP->list), (ClientData)word1);
    }
    if ( (idx2 = LModelLongIndex( lmP, word2)) < 0 || idx2>=lmP->listN) {
      ERROR("Class '%s' does not exists in '%s' unigramtable, ignoring word %s.\n", word2, lmP->name, word1);
      continue;
    }

    lmP->list.itemA[idx1].class    = idx2;
    lmP->list.itemA[idx1].penalty  = prob;

    n++;
  }

  if ( id) *id = lms; 
  INFO("Read #substutions %d.\n", n);
  return TCL_OK;
}

/* ------------------------------------------------------------------------
    LModelLongRead
   ------------------------------------------------------------------------ */

int LModelLongRead( LModelLong* lmP, char* fileName)
{
  FILE*         fP = NULL;
  char          line[1024];
  ArpaboLModelSection lms;
  int           section=0;
  int           i;

  int           rc = TCL_OK;

  if (! (fP = fileOpen( fileName,"r"))) {
    ERROR("Can't open file '%s' for reading.\n", fileName);
    return TCL_ERROR;
  }

  while ((lms  = LModelLongReadLine( lmP, fP, line, sizeof(line))) == LM_LINE);
  while ( lms != LM_END && rc == TCL_OK) {

    switch ( lms) {

    case LM_DATA :

      /* ------------------------------- */
      /* Find the number of NGram counts */
      /* ------------------------------- */

      rc = LModelLongReadNGramCounts( lmP, fP, &lms);
      section=1;
      break;

    case LM_NGRAMS :
      if(section==LModelLongReadLine_N) {
	section++;
	INFO("Reading %d-grams\n",LModelLongReadLine_N);
	rc = LModelLongReadNgrams( lmP, fP, LModelLongReadLine_N, &lms);
      }
      else {
	ERROR("%d-gram section expected, found %d\n",section,LModelLongReadLine_N);
	rc = TCL_ERROR;
      }
      break;

    case LM_UNIGRAMS :

      rc = LModelLongReadUnigramSection( lmP, fP, &lms);
      break;

    case LM_SUBS :

      rc = LModelLongReadSubsSection( lmP, fP, &lms);
      break;

    default :

	lms = LModelLongReadLine( lmP, fP, line, sizeof(line));
    }
  }

  if( rc == TCL_OK ) {
    lms = LModelLongReadLine( lmP, fP, line, sizeof(line));
    while ( lms != LM_END && rc == TCL_OK) {
      switch ( lms) {
      case LM_SUBS :
	rc = LModelLongReadSubsSection( lmP, fP, &lms);
	break;
      default :
	lms = LModelLongReadLine( lmP, fP, line, sizeof(line));
      }
    }
  }


  if ( rc != TCL_OK) {

    if( lmP->ngrams ) {
      for(i=0;i<lmP->n-2;i++) {
	if ( lmP->ngrams[i].ngramA)  { free( lmP->ngrams[i].ngramA);  lmP->ngrams[i].ngramA = NULL; lmP->ngrams[i].ngramN  = 0; }
	if ( lmP->ngrams[i].ngramXA)  { free( lmP->ngrams[i].ngramXA);  lmP->ngrams[i].ngramXA = NULL; lmP->ngrams[i].ngramXN  = 0; }
	farrayDeinit( &lmP->ngrams[i].nprobA);
	farrayDeinit( &lmP->ngrams[i].nbackOffA);
      }
      free(lmP->ngrams); lmP->ngrams=0;
    }
    farrayDeinit( &lmP->tprobA);
    lmP->n=0;
  }
  else {
    INFO("Completed reading %s\n",fileName);
  }

  fileClose( fileName, fP);  
  return rc;
}

int LModelLongReadItf (ClientData cd, int argc, char *argv[])
{
  LModelLong* lmP      = (LModelLong *)cd;
  int     ac       =  argc - 1;
  char*   fileName =  NULL;

  if (itfParseArgv(argv[0], &ac, argv+1, 0, 
     "<filename>", ARGV_STRING, NULL, &fileName, cd, "file to read from", 
       NULL) != TCL_OK) return TCL_ERROR;

  return LModelLongRead( lmP, fileName);
}

/* ------------------------------------------------------------------------
    LModelLongScore    compute the score of the 3G language model for a
                   sequence of n words.
   ------------------------------------------------------------------------ */

float LModelLongScore( LModelLong* lmP, int* w, int n, int i)
{
  float prob = 0.0;
  int   j;

  if  ( i < 0) i = 0;
  for ( j = i; j < n; j++) {
    if(j+1<lmP->n) {
      prob += LModelLongProbNGram( lmP, j+1, w);
    } else {
      prob += LModelLongProbNGram( lmP, lmP->n, w+j+1-lmP->n);
    }
  }
  return -lmP->lz * prob + (n-i) * lmP->lp;
}

int LModelLongScoreItf( ClientData cd, int argc, char * argv[])
{
  LModelLong*   lmP    = (LModelLong *) cd;
  IArray    warray = {NULL, 0};
  int       ac     =  argc-1;
  int       i      =  0;

  if(lmP->n==0) {
    ERROR("Language model has not been read yet.\n");	
    return TCL_ERROR;
  }

  if (itfParseArgv( argv[0], &ac,  argv+1,  0,
     "<word sequence>", ARGV_LIARRAY, NULL, &warray, 
     &(lmP->list), "sequence of words", 
     "-idx", ARGV_INT, NULL, &i, NULL, "index of cond. prob.",
     NULL) != TCL_OK) {
    if ( warray.itemA) free( warray.itemA);
    return TCL_ERROR;
  }

  itfAppendElement( "%f", LModelLongScore(lmP, warray.itemA, warray.itemN, i));

  if ( warray.itemA) free( warray.itemA);
  return TCL_OK;
} 

/* ------------------------------------------------------------------------
    LModelLongSample   computes a random predecessor of a word
                   sequence of n words.
   ------------------------------------------------------------------------ */

int LModelLongSample( LModelLong* lmP, int* w, int n)
{
  float prob = chaosRandom (0.0, 1.0);
  int * arr = malloc(sizeof(int)*(n+1));
  int i;

  for(i=0;i<n;i++)
    arr[i]=w[i];
		     
  for ( i = 0; i < lmP->list.itemN; i++) {
    float score;
    arr[n]=i;
    score = pow(10,-LModelLongScore( lmP, arr, n+1, n));
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

int LModelLongSampleItf( ClientData cd, int argc, char * argv[])
{
  LModelLong*   lmP    = (LModelLong *) cd;
  IArray    warray = {NULL, 0};
  int       ac     =  argc-1;

  if(lmP->n==0) {
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

  itfAppendElement( "%d", LModelLongSample(lmP, warray.itemA, warray.itemN));

  if ( warray.itemA) free( warray.itemA);
  return TCL_OK;
} 

/* ------------------------------------------------------------------------
    LModelLongNextFrame   update cache frame index
   ------------------------------------------------------------------------ */

int LModelLongNextFrame( LModelLong* lmP)
{
  /*
  lmP->cacheP->frameX++;
  */
  return TCL_OK;
}

int LModelLongNextFrameItf( ClientData cd, int argc, char * argv[])
{
  LModelLong*   lmP    = (LModelLong *) cd;
  int       ac     =  argc-1;

  if (itfParseArgv( argv[0], &ac,  argv+1,  0,
       NULL) != TCL_OK) return TCL_ERROR;

  return LModelLongNextFrame(lmP);
} 

/* ------------------------------------------------------------------------
    LModelLongReset   reset language model cache
   ------------------------------------------------------------------------ */

int LModelLongReset( LModelLong* lmP)
{
  
  return TCL_OK;
  /*
  return LModelLongCacheReset( lmP->cacheP);
  */
}

int LModelLongResetItf( ClientData cd, int argc, char * argv[])
{
  LModelLong*   lmP    = (LModelLong *) cd;
  int       ac     =  argc-1;

  if (itfParseArgv( argv[0], &ac,  argv+1,  0,
       NULL) != TCL_OK) return TCL_ERROR;

  return LModelLongReset(lmP);
} 

/* ========================================================================
    Type Declaration
   ======================================================================== */

static Method LModelLongItemMethod[] = 
{ 
  { "puts", LModelLongItemPutsItf,   NULL },
  {  NULL,  NULL, NULL } 
};

TypeInfo LModelLongItemInfo = { 
        "LModelLongItem", 0, -1, LModelLongItemMethod, 
         NULL, NULL,
         LModelLongItemConfigureItf, NULL,
         itfTypeCntlDefaultNoLink, 
        "Mgram Language Model Item\n" };

static Method LModelLongMethod[] = 
{ 
  { "puts",   LModelLongPutsItf,   "display the contents of an LModelLong" },
  { "read",   LModelLongReadItf,   "reads a LModelLong file"               },

  { "index",  LModelLongIndexItf,  "return the internal index of an LModelLongItem" },
  { "name",   LModelLongNameItf,   "return the name of an LModelLongItem" },
  { "score",  LModelLongScoreItf,  "return the score of a text string"     },
  { "sample",  LModelLongSampleItf, "return a randomly selected follower word"},

  { "nextFrame", LModelLongNextFrameItf,  "incr. cache frame index"  },
  { "reset",     LModelLongResetItf,      "reset language model"     },

  {  NULL,    NULL,           NULL } 
};

TypeInfo LModelLongInfo = {
        "LModelLong", 0, -1, LModelLongMethod, 
         LModelLongCreateItf,LModelLongFreeItf,
         LModelLongConfigureItf,LModelLongAccessItf, NULL,
	"Mgram Language Model\n" } ;

static LM LModelLongLM = 
       { "LModelLong", &LModelLongInfo, &LModelLongInfo,
         (LMIndexFct*)    LModelLongIndex,
         (LMNameFct *)    LModelLongName,
         (LMNameNFct *)   LModelLongNameN,
         (LMScoreFct*)    LModelLongScore,
         (LMNextFrameFct*)LModelLongNextFrame,
         (LMResetFct*)    LModelLongReset,
	 NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL};

int LModelLong_Init( )
{
  static int LModelLongInitialized = 0;

  if (! LModelLongInitialized) {

    LModelLongDefault.name            = NULL;
    LModelLongDefault.useN            = 0;
    LModelLongDefault.list.itemN      = 0;
    LModelLongDefault.list.blkSize    = 1000;

    LModelLongDefault.lz              = 1.0;
    LModelLongDefault.lp              = 0.0;
    LModelLongDefault.ngramSegSize    = 9;

    itfNewType (&LModelLongItemInfo);
    itfNewType (&LModelLongInfo);
    lmNewType(  &LModelLongLM);

    LModelLongInitialized = 1;
  }
  return TCL_OK;
}
