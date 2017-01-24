/* ========================================================================
    JanusRTk   Janus Recognition Toolkit
               ------------------------------------------------------------
               Object: Language Model Cache
               ------------------------------------------------------------

    Author  :  Michael Finke
    Module  :  lmCache.c
    Date    :  $Id: lmodelCache.c 2390 2003-08-14 11:20:32Z fuegen $
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

   =======================================================================

    $Log$
    Revision 1.4  2003/08/14 11:20:23  fuegen
    Merged changes on branch jtk-01-01-15-fms (jaguar -> ibis-013)

    Revision 1.3.20.1  2002/01/21 08:18:14  metze
    Cosmetic changes

    Revision 1.3  2000/11/14 12:29:36  janus
    Merging branch jtk-00-10-27-jmcd.

    Revision 1.2.30.2  2000/11/08 17:23:16  janus
    Finished making changes required for compilation under 'gcc -Wall'.

    Revision 1.2.30.1  2000/11/06 10:50:34  janus
    Made changes to enable compilation under -Wall.

    Revision 1.2  2000/07/20 12:36:04  soltau
    added forward declaration

    Revision 1.1  1998/05/26 18:27:23  kries
    Initial revision


   ======================================================================== */

#include "common.h"
#include "lmodel.h"
#include "lmodelCache.h"
#include "itf.h"
#include "error.h"

char lmodelCacheRCSVersion[]= 
           "@(#)1$Id: lmodelCache.c 2390 2003-08-14 11:20:32Z fuegen $";


/* ========================================================================
    LModelCacheTgItem
   ======================================================================== */
/* ------------------------------------------------------------------------
    lmodelCacheTgItemInit
   ------------------------------------------------------------------------ */

static int lmodelCacheTgItemInit( LModelCacheTgItem* itemP, WordIdx wordX)
{
  assert( itemP);

  itemP->wordX   =  wordX;
  itemP->lineX   = -1;
  itemP->ftgramX = -1;
  itemP->ltgramX = -2;
  itemP->backOff =  0.0;
  itemP->nextP   =  NULL;
  return TCL_OK;
}

/* ------------------------------------------------------------------------
    lmodelCacheTgItemCreate
   ------------------------------------------------------------------------ */

static LModelCacheTgItem* lmodelCacheTgItemCreate( WordIdx wordX)
{
  LModelCacheTgItem* lmtP = (LModelCacheTgItem*)malloc(
                      sizeof(LModelCacheTgItem));

  if (! lmtP || lmodelCacheTgItemInit( lmtP, wordX) != TCL_OK) {
     if ( lmtP) free( lmtP);
     ERROR("Failed to allocate LModelCacheTgItem object.\n");
     return NULL; 
  }
  return lmtP;
}

/* ------------------------------------------------------------------------
    lmodelCacheTgItemDeinit
   ------------------------------------------------------------------------ */

/* forward declaration */
static int lmodelCacheTgItemFree( LModelCacheTgItem* itemP);

static int lmodelCacheTgItemDeinit( LModelCacheTgItem* itemP)
{
  int  rc = TCL_OK;

  if (!itemP) return TCL_OK;
  if ( itemP->nextP) {
    rc = lmodelCacheTgItemFree( itemP->nextP);
    itemP->nextP = NULL;
  }
  return rc;
}

/* ------------------------------------------------------------------------
    lmodelCacheTgItemFree
   ------------------------------------------------------------------------ */

static int lmodelCacheTgItemFree( LModelCacheTgItem* itemP)
{
  int rc = TCL_OK;

  if (!itemP) return TCL_OK;
  
  rc = lmodelCacheTgItemDeinit( itemP);
  free( itemP);
  return rc;
}


/* ========================================================================
    LModelCacheBgItem
   ======================================================================== */
/* ------------------------------------------------------------------------
    lmodelCacheBgItemInit
   ------------------------------------------------------------------------ */

static int lmodelCacheBgItemInit( LModelCacheBgItem* itemP)
{
  assert( itemP);

  itemP->lineX   = -1;
  itemP->tgItemP =  NULL;
  return TCL_OK;
}

/* ------------------------------------------------------------------------
    lmodelCacheBgItemReset
   ------------------------------------------------------------------------ */

static int lmodelCacheBgItemReset( LModelCacheBgItem* itemP)
{
  if (!itemP) return TCL_OK;
  if ( itemP->tgItemP) lmodelCacheTgItemFree( itemP->tgItemP);

  itemP->lineX   = -1;
  itemP->tgItemP = NULL;
  return TCL_OK;
}

/* ------------------------------------------------------------------------
    lmodelCacheBgItemDeinit
   ------------------------------------------------------------------------ */
/* Not used
static int lmodelCacheBgItemDeinit( LModelCacheBgItem* itemP)
{
  if (!itemP) return TCL_OK;
  return lmodelCacheBgItemReset( itemP);
}
*/
/* ========================================================================
    LModelCacheItem
   ======================================================================== */
/* ------------------------------------------------------------------------
    lmodelCacheItemInit
   ------------------------------------------------------------------------ */

static int lmodelCacheItemInit( LModelCacheItem* itemP, float* probA, int probN)
{
  assert( itemP);

  itemP->wordX[0] =  NO_WORD;
  itemP->wordX[1] =  NO_WORD;
  itemP->frameX   = -1;
  itemP->probA    =  probA;
  itemP->probN    =  probN;
  itemP->tgItemP  =  NULL;
  return TCL_OK;
}

/* ------------------------------------------------------------------------
    lmodelCacheItemReset
   ------------------------------------------------------------------------ */

static int lmodelCacheItemReset( LModelCacheItem* itemP, int probN)
{
  int  i;

  if (!itemP) return TCL_OK;

  itemP->wordX[0] =  NO_WORD;
  itemP->wordX[1] =  NO_WORD;
  itemP->frameX   = -1;

  for ( i = 0; i < probN; i++) itemP->probA[i] = ILLEGAL_PROB + 1;

  itemP->tgItemP  =  NULL;
  return TCL_OK;
}

/* ------------------------------------------------------------------------
    lmodelCacheItemDeinit
   ------------------------------------------------------------------------ */
/* Not used
static int lmodelCacheItemDeinit( LModelCacheItem* itemP, int probN)
{
  if (!itemP) return TCL_OK;
  return lmodelCacheItemReset( itemP, probN);
}
*/
/* ========================================================================
    LModelCache
   ======================================================================== */
/* ------------------------------------------------------------------------
    lmodelCacheInit
   ------------------------------------------------------------------------ */

int lmodelCacheInit( LModelCache* lmcP, LModel* lmP, int itemN, int wordN)
{
  int i;

  assert( lmP);
  assert( lmcP);

  lmcP->lmodelP = lmP;

  if (! (lmcP->itemA = (LModelCacheItem*)malloc( itemN * 
                 sizeof(LModelCacheItem)))) {
    ERROR("Cannot allocate language model cache.\n");
    return TCL_ERROR;
  }

  if (! (lmcP->bgCacheA = (LModelCacheBgItem*)malloc( wordN * 
                    sizeof(LModelCacheBgItem)))) {
    ERROR("Cannot allocate bigram score cache.\n");
    if ( lmcP->itemA) { free( lmcP->itemA); lmcP->itemA = NULL; }
    return TCL_ERROR;
  }

  if (! (lmcP->probA = (float*)malloc( wordN * itemN * sizeof(float)))) {
    ERROR("Cannot allocate probability score cache.\n");
    if ( lmcP->bgCacheA) { free( lmcP->bgCacheA); lmcP->bgCacheA = NULL; }
    if ( lmcP->itemA)    { free( lmcP->itemA);    lmcP->itemA    = NULL; }
    return TCL_ERROR;
  }
  lmcP->itemN = itemN;
  lmcP->wordN = wordN;

  /* ------------------------------------------------ */
  /* Initialize cache infos and the probability array */
  /* ------------------------------------------------ */

  for ( i = 0; i < wordN; i++) lmodelCacheBgItemInit( lmcP->bgCacheA+i);
  for ( i = 0; i < itemN; i++) {
    lmodelCacheItemInit(   lmcP->itemA + i, lmcP->probA + i * wordN, wordN);
  }
  return lmodelCacheReset( lmcP);
}

/* ------------------------------------------------------------------------
    lmodelCacheCreate
   ------------------------------------------------------------------------ */

LModelCache* lmodelCacheCreate( LModel* lmP, int itemN, int wordN)
{
  LModelCache* lmcP = (LModelCache*)malloc(sizeof(LModelCache));

  if (! lmP || lmodelCacheInit( lmcP, lmP, itemN, wordN) != TCL_OK) {
     if ( lmcP) free( lmcP);
     ERROR("Failed to allocate LModelCache object.\n");
     return NULL; 
  }
  return lmcP;
}

/* ------------------------------------------------------------------------
    lmodelCacheResetLine  reset an individual cache line without actually
                          deallocating any info structures.
   ------------------------------------------------------------------------ */

static int lmodelCacheResetLine( LModelCache* lmcP, int line)
{
  LModel* lmP = lmcP->lmodelP;

  if ( lmcP->itemA[line].wordX[0] == NO_WORD) return TCL_OK;
  if ( lmcP->itemA[line].wordX[1] == NO_WORD) {
    int fbgramX = lmP->list.itemA[lmcP->itemA[line].wordX[0]].fbgramX;
    int lbgramX = lmP->list.itemA[lmcP->itemA[line].wordX[0]].lbgramX;
    int i;

    /* ----------------------- */
    /* Reset bigram cache line */
    /* ----------------------- */

    lmcP->bgCacheA[lmcP->itemA[line].wordX[0]].lineX = -1;

    for ( i = fbgramX; i <= lbgramX; i++)
      lmcP->itemA[line].probA[lmP->bgramA[i].wordX] = ILLEGAL_PROB+1;
  }
  else {
    LModelCacheTgItem* tgiP    = lmcP->itemA[line].tgItemP;
    int                ftgramX = tgiP->ftgramX;
    int                ltgramX = tgiP->ltgramX;
    int i;

    /* ------------------------ */
    /* Reset trigram cache line */
    /* ------------------------ */

    tgiP->lineX = -1;

    for ( i = ftgramX; i <= ltgramX; i++)
      lmcP->itemA[line].probA[lmP->tgramA[i].wordX] = ILLEGAL_PROB+1;
  }
  return TCL_OK;
}

/* ------------------------------------------------------------------------
    lmodelCacheReset  resets the language model cache lines to NULL
   ------------------------------------------------------------------------ */

int lmodelCacheReset( LModelCache* lmcP) {
  int i;
  for ( i = 0; i < lmcP->wordN; i++) lmodelCacheBgItemReset( lmcP->bgCacheA+i);
  for ( i = 0; i < lmcP->itemN; i++) lmodelCacheItemReset(   lmcP->itemA + i,
                                                             lmcP->wordN);
  lmcP->frameX = 0;
  return TCL_OK;
}

/* ------------------------------------------------------------------------
    lmodelCacheDeinit
   ------------------------------------------------------------------------ */

int lmodelCacheDeinit( LModelCache* lmcP)
{
  if (!lmcP) return TCL_OK;

  lmodelCacheReset( lmcP);

  if ( lmcP->bgCacheA) { free( lmcP->bgCacheA); lmcP->bgCacheA = NULL; }
  if ( lmcP->itemA)    { free( lmcP->itemA);    lmcP->itemA    = NULL; }
  if ( lmcP->probA)    { free( lmcP->probA);    lmcP->probA    = NULL; }

  lmcP->itemN = 0;
  lmcP->wordN = 0;

  return TCL_OK;
}

/* ------------------------------------------------------------------------
    lmodelCacheFree
   ------------------------------------------------------------------------ */

int lmodelCacheFree( LModelCache* lmcP)
{
  int rc = TCL_OK;

  if (! lmcP) return TCL_OK;
  
  rc = lmodelCacheDeinit( lmcP);
  free( lmcP);
  return rc;
}

/* ------------------------------------------------------------------------
    lmodelCacheGetLine  get a new cache line from the LM cache
   ------------------------------------------------------------------------ */

static int lmodelCacheGetLine( LModelCache* lmcP)
{
  int frX   =  lmcP->frameX;
  int probN =  0x7fffffff;
  int i, j = -1;

  /* ------------------------------------------------------------------ */
  /* Find the next free cache line, if none, oldest cache line, if none */
  /* use the smallest cache line                                        */
  /* ------------------------------------------------------------------ */

  for ( i = 0; i < lmcP->itemN; i++) {
    if ( lmcP->itemA[i].wordX[0] == NO_WORD) return i;
    if ( lmcP->itemA[i].frameX < frX) {
      frX   = lmcP->itemA[i].frameX;
      probN = lmcP->itemA[i].probN;
      j     = i;
    } else if ( lmcP->itemA[i].frameX == frX &&
                lmcP->itemA[i].probN   < probN) {
      probN =   lmcP->itemA[i].probN;
      j     = i;
    }
  }

  lmodelCacheResetLine( lmcP, j);
  return j;
}

/* ------------------------------------------------------------------------
    lmodelCacheUgScore  get a unigram score from lmodel cache
   ------------------------------------------------------------------------ */

float lmodelCacheUgScore( LModelCache* lmcP, WordIdx w1)
{
  return lmcP->lmodelP->list.itemA[w1].prob[0];
}

/* ------------------------------------------------------------------------
    lmodelCacheBgScore  get a bigram score from lmodel cache
   ------------------------------------------------------------------------ */

float lmodelCacheBgScore( LModelCache* lmcP, WordIdx w1, WordIdx w2)
{
  int  lineX;

  if ( lmcP->lmodelP->list.itemN > lmcP->wordN) {

    LModel* lmP   = lmcP->lmodelP;
    int     itemN = lmcP->itemN;

    /* ------------------ */
    /* Recreate the cache */
    /* ------------------ */

    lmodelCacheDeinit( lmcP);
    lmodelCacheInit(   lmcP, lmP, itemN, lmP->list.itemN);
  }

  if ((lineX  = lmcP->bgCacheA[w1].lineX) < 0) {
    LModel* lmP     = lmcP->lmodelP;
    int     fbgramX = lmP->list.itemA[w1].fbgramX;
    int     lbgramX = lmP->list.itemA[w1].lbgramX;
    int     i;

    /* ----------------- */
    /* bigram not cached */
    /* ----------------- */

    lmcP->bgCacheA[w1].lineX = lineX = lmodelCacheGetLine( lmcP);

    lmcP->itemA[lineX].wordX[0] = w1;
    lmcP->itemA[lineX].wordX[1] = NO_WORD;
    lmcP->itemA[lineX].probN    = lbgramX - fbgramX + 1;

    for ( i = fbgramX; i <= lbgramX; i++)
      lmcP->itemA[lineX].probA[lmP->bgramA[i].wordX] =
        lmP->bprobA.itemA[lmP->bgramA[i].probX]; 
  }

  lmcP->itemA[lineX].frameX = lmcP->frameX;

  if ( lmcP->itemA[lineX].probA[w2] >= ILLEGAL_PROB) {

    /* ------------------- */
    /* BackOff to unigrams */
    /* ------------------- */

         return lmcP->lmodelP->list.itemA[w1].backOff +
                lmodelCacheUgScore( lmcP, w2);
  } else return lmcP->itemA[lineX].probA[w2];
}

/* ------------------------------------------------------------------------
    lmodelCacheTgScore  get a trigram score from lmodel cache
   ------------------------------------------------------------------------ */

float lmodelCacheTgScore( LModelCache* lmcP, 
                          WordIdx w1, WordIdx w2, WordIdx w3)
{
  LModel*            lmP      = lmcP->lmodelP;
  LModelCacheTgItem* tgItemP  = NULL;
  LModelCacheTgItem* ptgItemP = NULL;

  if ( lmP->tgramN < 1 || w1 == NO_WORD) 
       return lmodelCacheBgScore( lmcP, w2, w3);

  if ( lmcP->lmodelP->list.itemN > lmcP->wordN) {

    LModel* lmP   = lmcP->lmodelP;
    int     itemN = lmcP->itemN;

    /* ------------------ */
    /* Recreate the cache */
    /* ------------------ */

    lmodelCacheDeinit( lmcP);
    lmodelCacheInit(   lmcP, lmP, itemN, lmP->list.itemN);
  }

  /* ------------------------------ */
  /* Find TgItem structure in Cache */
  /* ------------------------------ */

  for ( tgItemP = lmcP->bgCacheA[w2].tgItemP; tgItemP; 
        tgItemP = tgItemP->nextP) {
    if (tgItemP->wordX == w1) break;
    ptgItemP = tgItemP;
  }

  if (! tgItemP) {

    int bgX = lmodelFindBGram( lmP, w1, w2);
    tgItemP = lmodelCacheTgItemCreate( w1);

    /* ----------------------------------------------------------- */
    /* Create CacheTgItem object and insert into the list of items */
    /* ----------------------------------------------------------- */

    if (! tgItemP) return ILLEGAL_PROB;
    if (  bgX >= 0) {
      tgItemP->ftgramX =  lmP->tgramXA[ bgX    >> lmP->tgramSegSize] + 
                          lmP->bgramA[  bgX].tgramX;
      tgItemP->ltgramX =  lmP->tgramXA[(bgX+1) >> lmP->tgramSegSize] +
                          lmP->bgramA[  bgX+1].tgramX - 1;
      tgItemP->backOff =  lmP->bbackOffA.itemA[lmP->bgramA[bgX].backOffX];
    } else {
      tgItemP->ftgramX = -1;
      tgItemP->ltgramX = -2;
      tgItemP->backOff =  0.0;
    }

    /* ------------------------------------ */
    /* Insert new entry at head of the list */
    /* ------------------------------------ */

    tgItemP->nextP             = lmcP->bgCacheA[w2].tgItemP;
    lmcP->bgCacheA[w2].tgItemP = tgItemP;

  } else if ( ptgItemP) {
    
    /* ----------------------------- */
    /* Move entry to top of the list */
    /* ----------------------------- */

    ptgItemP->nextP            = tgItemP->nextP;
    tgItemP->nextP             = lmcP->bgCacheA[w2].tgItemP;
    lmcP->bgCacheA[w2].tgItemP = tgItemP;
  }

  if ( tgItemP->ltgramX >= tgItemP->ftgramX) {

    float prob  = ILLEGAL_PROB;
    int   lineX;

    /* ----------------- */
    /* Check cache lines */
    /* ----------------- */

    if ((lineX = tgItemP->lineX) < 0) {

      int i;

      /* ------------------------------ */
      /* Find new cache line for tgItem */
      /* ------------------------------ */

      tgItemP->lineX = lineX = lmodelCacheGetLine( lmcP);

      lmcP->itemA[lineX].wordX[0] = w1;
      lmcP->itemA[lineX].wordX[1] = w2;
      lmcP->itemA[lineX].probN    = tgItemP->ltgramX - tgItemP->ftgramX + 1;
      lmcP->itemA[lineX].tgItemP  = tgItemP;

      for ( i = tgItemP->ftgramX; i <= tgItemP->ltgramX; i++) {
        lmcP->itemA[lineX].probA[lmP->tgramA[i].wordX] =
          lmP->tprobA.itemA[lmP->tgramA[i].probX];
      }
    }

    lmcP->itemA[lineX].frameX = lmcP->frameX;

    if ((prob = lmcP->itemA[lineX].probA[w3] ) < ILLEGAL_PROB) return prob;
  }

  /* ------------------ */
  /* BackOff to bigrams */
  /* ------------------ */

  return tgItemP->backOff + lmodelCacheBgScore( lmcP, w2, w3);
}









