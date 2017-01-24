/* ========================================================================
    JANUS      Speech- to Speech Translation
               ------------------------------------------------------------
               Object: Language model that can connect or mix LMs
               ------------------------------------------------------------

    Author  :  Florian Metze
    Module  :  metaLM.c
    Date    :  $Id: metaLM.c 3416 2011-03-23 03:02:18Z metze $

    Remarks :  This module is a meta language model, it needs
               base language models underneath and can interpolate ...
               them

	       If you wan to add new scoring functions, all you'd have o
	       do is add their parameters, write them, make the data
	       read- and saveable and take care of the lct handling
	       functions

   ========================================================================

    This software was developed by

    the  University of Karlsruhe       and  Carnegie Mellon University
         Dept. of Informatics               Dept. of Computer Science
         ILKD, Lehrstuhl Prof.Waibel        Alex Waibel's NN & Speech Group
         Am Fasanengarten 5                 5000 Forbes Ave
         D-76131 Karlsruhe                  Pittsburgh, PA 15213
         - Germany -                        - USA -

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
    Revision 4.3  2004/11/12 16:28:45  fuegen
    Deactivated use of MetaLMElem.weight. Use MetaLMCoverItf instead.

    Revision 4.2  2004/08/12 12:20:31  fuegen
    set dirty flag after clearing lists in createLCT

    Revision 4.1  2003/08/14 11:20:04  fuegen
    Merged changes on branch jtk-01-01-15-fms (jaguar -> ibis-013)

    Revision 1.1.2.39  2003/08/06 17:23:54  soltau
    interpolCreateLCT : set dirty flag

    Revision 1.1.2.38  2003/07/15 16:48:29  fuegen
    added forward declaration of putsItf function

    Revision 1.1.2.37  2003/07/15 16:37:51  fuegen
    changed LingKS load/save interface
    added lingKSPutsFct

    Revision 1.1.2.36  2003/03/11 16:37:23  metze
    Cacheing for MetaLM, Map for GLat computeLCT

    Revision 1.1.2.35  2002/11/27 13:14:48  metze
    -lmA/-lmB -> -lksA/-lksB

    Revision 1.1.2.34  2002/08/01 13:42:31  metze
    Fixes for clean documentation.

    Revision 1.1.2.33  2002/07/25 07:27:36  soltau
    metaLMInit_hash : increased ringbuffer to 200'000

    Revision 1.1.2.32  2002/07/18 12:10:09  soltau
    Added ringbuffer to optimize hashtables for lctList

    Revision 1.1.2.31  2002/06/18 16:39:47  metze
    Sped up interpolated score computation

    Revision 1.1.2.30  2002/06/13 08:07:18  metze
    Added dirty as a function in LMs

    Revision 1.1.2.29  2002/06/12 10:09:48  metze
    Moved declarations

    Revision 1.1.2.28  2002/06/12 09:44:10  metze
    Checks for valid LCT

    Revision 1.1.2.27  2002/06/11 11:40:06  metze
    Code reorganization

    Revision 1.1.2.26  2002/06/10 13:27:09  metze
    Added 'dirty' tag

    Revision 1.1.2.25  2002/05/27 14:32:20  metze
    Added check for invalid LCTs in interpolExtendLCT_hash

    Revision 1.1.2.24  2002/05/03 14:02:27  metze
    Freeing of LingKS now works properly

    Revision 1.1.2.23  2002/05/03 13:46:35  soltau
    mlctHashKey: optimized hash function

    Revision 1.1.2.22  2002/05/02 12:53:08  metze
    Bugfix in interface

    Revision 1.1.2.21  2002/04/29 13:29:37  metze
    Redesign of LM interface (LingKS)

    Revision 1.1.2.20  2002/02/28 16:53:20  soltau
    MetaLMDeinit: removed warning

    Revision 1.1.2.19  2002/02/27 15:11:34  soltau
    default score functions are now hash based

    Revision 1.1.2.18  2002/02/26 11:02:12  metze
    Replaced fclose by fileClose

    Revision 1.1.2.17  2002/02/15 17:13:19  soltau
    mlctList info

    Revision 1.1.2.16  2002/02/14 18:31:25  soltau
    found the correct number

    Revision 1.1.2.15  2002/02/07 20:52:46  metze
    Removed history for metaLM

    Revision 1.1.2.14  2002/02/07 13:36:25  soltau
    Added lct handling by hashing

    Revision 1.1.2.13  2002/02/07 12:21:23  metze
    Removed "simple" score function

    Revision 1.1.2.12  2002/02/07 12:15:31  metze
    Changes in interface to LM score functions

    Revision 1.1.2.11  2002/02/06 14:18:27  soltau
    Access correct interpolation weights

    Revision 1.1.2.10  2002/02/05 19:27:32  soltau
    Added tmp score arrays to make interpolation working

    Revision 1.1.2.9  2002/02/05 11:31:38  soltau
    Fixed problems with the global 'c' structure in case of hierarchical mteaLM's

    Revision 1.1.2.8  2002/01/19 11:39:07  metze
    Added whole bunch of convenience interface functions

    Revision 1.1.2.7  2002/01/14 13:59:13  soltau
    *** empty log message ***

    Revision 1.1.2.6  2002/01/14 10:30:47  metze
    Cleaned up reading/ writing of dumps

    Revision 1.1.2.5  2002/01/11 19:20:28  soltau
    Addded 'simple' score function

    Revision 1.1.2.4  2001/10/22 08:40:50  metze
    Changed interface to score functions

    Revision 1.1.2.3  2001/10/11 17:23:10  metze
    Basic implementation seems to work. Unifying of LCTs necessary here too

    Revision 1.1.2.2  2001/10/03 15:53:37  metze
    Basic implementation

    Revision 1.1.2.1  2001/09/26 08:58:08  metze
    Renamed interpolLM to metaLM

   ======================================================================== */


#include "metaLM.h"
#include "mach_ind_io.h"


char metaLMRCSVersion[]="$Id: metaLM.c 3416 2011-03-23 03:02:18Z metze $";


/* ------------------------------------------------------------------------
    forward declarations 
   ------------------------------------------------------------------------ */

static int MetaLMInitialized = 0;


/* ========================================================================
     Interpol Score Function
   ======================================================================== */

static int     metaLMInit_interpol (LingKS* lksP);
static LCT     interpolCreateLCT   (LingKS* lksP, LVX lvX);
static LCT     interpolExtendLCT   (LingKS* lksP, LCT lct, LVX lvX);
static LCT     interpolReduceLCT   (LingKS* lksP, LCT lct, int n);
static int     interpolDecodeLCT   (LingKS* lksP, LCT lct, LVX* lvxA, int n);
static int     interpolFreeLCT     (LingKS* lksP, LCT lct);
static int     interpolScoreArray  (LingKS *lksP, LCT lct, lmScore *arr, int usehistory);
static lmScore interpolScore       (LingKS* lksP, LCT lct, LVX w, int usehistory);
static int     setInterpolScoreFct (MetaLM* lmP);


/* ========================================================================
     Hash Score Function
   ======================================================================== */

static int     metaLMInit_hash          (LingKS* lksP);
static LCT     interpolCreateLCT_hash   (LingKS* lksP, LVX lvX);
static LCT     interpolExtendLCT_hash   (LingKS* lksP, LCT lct, LVX lvX);
static LCT     interpolReduceLCT_hash   (LingKS* lksP, LCT lct, int n);
static int     interpolDecodeLCT_hash   (LingKS* lksP, LCT lct, LVX* lvxA, int n);
static int     interpolFreeLCT_hash     (LingKS* lksP, LCT lct);
static int     interpolScoreArray_hash  (LingKS *lksP, LCT lct, lmScore *arr, int usehistory);
static lmScore interpolScore_hash       (LingKS* lksP, LCT lct, LVX w, int usehistory);
static int     setInterpolScoreFct_hash (MetaLM* lmP);

/* LSA functions */ /* Wilson May 05 */
static lmScore interpolScore_hash_LSA       (LingKS* lksP, LCT lct, LVX w, int usehistory); 

/* perform log-linear combination to avoid expensive online log(.) and exp(.) operation */
static lmScore log_interpolScore_hash (LingKS* lksP, LCT lct, LVX w, int usehistory);
static int     log_setInterpolScoreFct_hash (MetaLM* lmP);
static int     log_interpolScoreArray_hash (LingKS *lksP, LCT lct, lmScore *arr, int n);

/* use ratio combination */
static lmScore log_uniform_interpolScore_hash (LingKS* lksP, LCT lct, LVX w, int usehistory);
static int     log_uniform_setInterpolScoreFct_hash (MetaLM* lmP);
static int     setInterpolScoreFct_hash_LSA (MetaLM* lmP); 


/* ========================================================================
    Model-specific functions
   ======================================================================== */
static int MetaLMPutsItf (ClientData cd, int argc, char *argv[]);
static int MetaLMItemAIsStopWord (MetaLM* lmP, int i);


/* ========================================================================
   ==================================
     Interpolating scoring function
   ==================================
   ======================================================================== */

static union {
  LCT             l;
  MetaLMPathItem *m;
} c;

int metaLMInit_interpol (LingKS* lksP)
{
  return TCL_OK;
}


/* ------------------------------------------------------------------------
     reduceLCT
   -------------
     Needed to find the reduced history in the case of short-range 
     lookahead models, i.e. if you want a bigram lookahead for a 
     trigram LM
   ------------------------------------------------------------------------ */

LCT interpolReduceLCT (LingKS* lksP, LCT lct, int n)
{
  MetaLMPathItem* pP;

  c.l = lct;
  pP  = c.m;

  while (n-- > 0 && pP->fromP)
    pP = pP->fromP;

  c.l = 0, c.m = pP;
  return c.l;
}


/* ------------------------------------------------------------------------
     extendLCT
   -------------
     Extend an LCT with an LVX, i.e. add another word to the history
   ------------------------------------------------------------------------ */

LCT interpolExtendLCT (LingKS* lksP, LCT lct, LVX lvX)
{
  MetaLM*         lmP = lksP->data.metaLM;
  MetaLMItem*    lmiP = lmP->list.itemA+lvX;
  LCT            lctA, lctB;
  MetaLMPathItem  *cP, *nP, *fromP;
  LingKS*        subP;

  c.l = lct;
  cP  = fromP = c.m;

  /* Extend the LCTs */
  subP = lmP->subl.itemA[lmiP->lmA].lksP;
  lctA = subP->extendLCT (subP, cP->lctA[0], lmiP->idxA);
  subP = lmP->subl.itemA[lmiP->lmB].lksP;
  lctB = subP->extendLCT (subP, cP->lctA[1], lmiP->idxB);


  /* Check if this LCT already exists */
  nP = lmiP->pathP;
  while (nP) {
    if (nP->lctA[0] == lctA && nP->lctA[1] == lctB) {
      c.l = 0, c.m = nP;
      return c.l;
    }
    nP = nP->nextP;
  }

  /* Create a new node */
  nP = (MetaLMPathItem*) bmemItem (lmP->mem.node);
  nP->lctA    = (LCT*)  bmemAlloc (lmP->mem.lct, lmP->mem.subN*sizeof (LCT));
  nP->lctA[0] = lctA;
  nP->lctA[1] = lctB;
  nP->toP     = NULL;
  nP->fromP   = fromP;
  nP->nextP   = lmiP->pathP;
  nP->prob    = lmiP->prob;
  lmiP->pathP = nP;

  c.l = 0, c.m = nP;
  return c.l;
}


/* ------------------------------------------------------------------------
     decodeLCT
   -------------
     Decode an LCT into a sequence of LVX (words)
   ------------------------------------------------------------------------ */

int interpolDecodeLCT (LingKS* lksP, LCT lct, LVX* lvxA, int n)
{
  MetaLM*         lmP = lksP->data.metaLM;
  MetaLMItem*    lmiP = lmP->list.itemA;
  LingKS*        subP = lmP->subl.itemA[lmiP->lmA].lksP;
  return subP->decodeLCT (subP, c.m->lctA[0], lvxA, n);
}


/* ------------------------------------------------------------------------
     createLCT
   -------------
     Create an LCT corresponding to '(' (begin-of-sentence), where this
     symbol is given by LVX lvX
   ------------------------------------------------------------------------ */

LCT interpolCreateLCT (LingKS* lksP, LVX lvX)
{
  MetaLM*         lmP = lksP->data.metaLM;
  MetaLMItem*    lmiP = lmP->list.itemA+lvX;
  MetaLMPathItem*  nP;
  LingKS*        subP;
  int               i;

  /* Free the memory pool */
  bmemClear (lmP->mem.node);
  bmemClear (lmP->mem.lct);
  lksP->dirty = 1;

  /* Empty all the linked lists in our lvX */
  for (i = 0; i < lmP->list.itemN; i++)
    lmP->list.itemA[i].pathP = NULL;     

  /* Create a node */
  nP = (MetaLMPathItem*) bmemItem (lmP->mem.node);
  nP->fromP = NULL;
  nP->toP   = NULL;
  nP->nextP = NULL;
  nP->prob  = lmiP->prob;
  nP->lctA  = (LCT*)  bmemAlloc (lmP->mem.lct, lmP->mem.subN*sizeof (LCT));

  /* Fill the node with meaning */
  subP = lmP->subl.itemA[lmiP->lmA].lksP;
  nP->lctA[0] = subP->createLCT (subP, lmiP->idxA);
  subP = lmP->subl.itemA[lmiP->lmB].lksP;
  nP->lctA[1] = subP->createLCT (subP, lmiP->idxB);

  lmiP->pathP = nP;

  c.l = 0, c.m = nP;
  return c.l;
}


/* ------------------------------------------------------------------------
     freeLCT
   -----------
     This LCT is no longer needed, so a grammar could release some memory
   ------------------------------------------------------------------------ */

int interpolFreeLCT (LingKS* lksP, LCT lct)
{
  return TCL_OK;
}


/* ------------------------------------------------------------------------
     interpolScoreArray
   ----------------------
     Produces an array of scores using interpolation
     'usehistory' could be used to switch the LM to a 'fast' or
     short-range mode, however this function does not support this mode
   ------------------------------------------------------------------------ */

int interpolScoreArray (LingKS *lksP, LCT lct, lmScore *arr, int usehistory)
{
  MetaLM*          lmP = lksP->data.metaLM;
  MetaLMItem*     lmiP = lmP->list.itemA;
  MetaLMPathItem*   pP = NULL;
  LingKS*         lksA = lmP->subl.itemA[lmiP->lmA].lksP;
  LingKS*         lksB = lmP->subl.itemA[lmiP->lmB].lksP;
  lmScore*        tmpA = NULL;
  lmScore*        tmpB = NULL;
  double             p = 0.0;
  int                i;

  /* Initialization */
  c.l   = lct;
  pP    = c.m;
  tmpA  = lmP->tmpA  = (lmScore*) realloc (lmP->tmpA, lksA->itemN(lksA) * sizeof (lmScore));
  tmpB  = lmP->tmpB  = (lmScore*) realloc (lmP->tmpB, lksB->itemN(lksB) * sizeof (lmScore));

  /* Get scores from the base LMs */
  lksA->scoreArrayFct (lksA, pP->lctA[0], tmpA, usehistory);
  lksB->scoreArrayFct (lksB, pP->lctA[1], tmpB, usehistory);

  /* Loop over all entries */
  p = pP->prob;
  for (i = 0; i < lmP->list.itemN; i++) {
    double a = tmpA[lmiP[i].idxA];
    double b = tmpB[lmiP[i].idxB];
    if (a>b) {
      double s =  exp(M_LN10*(b-a)/LMSCORE_SCALE_FACTOR);
      arr[i]= a + LMSCORE_SCALE_FACTOR*(log(p + (1-p) * s)/M_LN10);
    } else {
      double s =  exp(M_LN10*(a-b)/LMSCORE_SCALE_FACTOR);
      arr[i]= b + LMSCORE_SCALE_FACTOR*(log((1-p) + p * s)/M_LN10);
    }
  }

  return TCL_OK;
}


/* ------------------------------------------------------------------------
     interpolScore
   -----------------
     Scores an LCT with an LVX using interpolation
     For the original implementation, see lmodelInt.c:lmodelIntScore
   ------------------------------------------------------------------------ */

lmScore interpolScore (LingKS* lksP, LCT lct, LVX w, int usehistory)
{
  MetaLM*          lmP = lksP->data.metaLM;
  MetaLMItem*     lmiP = lmP->list.itemA+w;
  MetaLMPathItem*   pP = NULL;
  /* lmScore         prob = 0; */
  LingKS*         lksA = lmP->subl.itemA[lmiP->lmA].lksP;
  LingKS*         lksB = lmP->subl.itemA[lmiP->lmB].lksP;
  double sA, sB, alpha;

  c.l = lct;
  pP  = c.m;

  sA    = UnCompressScore (lksA->scoreFct (lksA, pP->lctA[0], lmiP->idxA, usehistory));
  sB    = UnCompressScore (lksB->scoreFct (lksB, pP->lctA[1], lmiP->idxB, usehistory));
  alpha = lmP->list.itemA[w].prob;

  /* Ensure sA >= sB */
  if (sA < sB) return CompressScore (sB + log ((1.0-alpha) + alpha * exp(M_LN10*(sA - sB))) / M_LN10);
  else         return CompressScore (sA + log (alpha + (1.0-alpha) * exp(M_LN10*(sB - sA))) / M_LN10);
}


/* ------------------------------------------------------------------------
     setInterpolScoreFct
   -----------------------
     Sets all pointers to the 'interpol' score function
   ------------------------------------------------------------------------ */

int setInterpolScoreFct (MetaLM* lmP)
{
  LingKS         *lksP = lmP->lksP;

  lksP->scoreArrayFct  = interpolScoreArray;
  lksP->scoreFct       = interpolScore;
  lksP->createLCT      = interpolCreateLCT;
  lksP->reduceLCT      = interpolReduceLCT;
  lksP->extendLCT      = interpolExtendLCT;
  lksP->decodeLCT      = interpolDecodeLCT;
  lksP->freeLCT        = interpolFreeLCT;

  lmP->mem.subN        = 2;
  
  return TCL_OK;
}


/* ========================================================================
     Hash-table for LCTs
   ======================================================================== */

static int hashlvxInit     (HashLVX* lvxP, HashLVX* hashkey)
{
  (*lvxP) = (*hashkey);
  return TCL_OK;
}
  
static int hashlvxHashCmp  (HashLVX* lvxP, HashLVX* hashkey)
{
  return (lvxP->lvx == hashkey->lvx && lvxP->lct == hashkey->lct);
}

#define lvxMask 0xFFFF
static long hashlvxHashKey (const HashLVX *key)
{
  unsigned long ret = key->lct & lvxMask;
  return ret;
}


/* ========================================================================
     Hash-table scoring function
   ======================================================================== */

static int mlctInit(MetaLCT* mlctP,MetaLCT* hashkey)
{
  (*mlctP) = (*hashkey);
  return TCL_OK;
}
  
static int mlctHashCmp(MetaLCT* mlctP, MetaLCT* hashkey)
{
  if (mlctP->lctA != hashkey->lctA) return 0;
  if (mlctP->lctB != hashkey->lctB) return 0;
  return 1;
}

# define lctBit 16
static long mlctHashKey( const MetaLCT *key)
{
  unsigned long ret = 0;
  int       longBit = 8;

  longBit *= sizeof(long);

  ret = ((ret<<lctBit) | (ret >> (longBit-lctBit)))  ^ key->lctA;
  ret = ((ret<<lctBit) | (ret >> (longBit-lctBit)))  ^ key->lctB;

  return ret;
}

int metaLMInit_hash (LingKS* lksP)
{
  MetaLM* lmP = lksP->data.metaLM;

  listInit((List*)&(lmP->mlctList),NULL, sizeof(MetaLCT),20000);
  lmP->mlctList.init    = (ListItemInit  *)mlctInit;
  lmP->mlctList.deinit  = NULL;
  lmP->mlctList.hashKey = (HashKeyFn     *)mlctHashKey;
  lmP->mlctList.hashCmp = (HashCmpFn     *)mlctHashCmp;
  lmP->mlctList.itemMax = 200000;

  return TCL_OK;
}


/* ------------------------------------------------------------------------
     createLCT
   -------------
     Create an LCT corresponding to '(' (begin-of-sentence), where this
     symbol is given by LVX lvX
   ------------------------------------------------------------------------ */

LCT interpolCreateLCT_hash (LingKS* lksP, LVX lvX)
{
  MetaLM*       lmP = lksP->data.metaLM;
  MetaLMItem*  lmiP = lmP->list.itemA+lvX;
  MetaLCT hkey;
  LingKS* subP;
  int idx;

  listClear((List*) &lmP->mlctList);

  if (lmP->lvxList.itemMax)
    listClear ((List*) &lmP->lvxList);

  lksP->dirty = 1;

  /* Create the LCTs : generate hashkey */
  subP      = lmP->subl.itemA[lmiP->lmA].lksP;
  hkey.lctA = subP->createLCT (subP, lmiP->idxA);
  subP      = lmP->subl.itemA[lmiP->lmB].lksP;
  hkey.lctB = subP->createLCT (subP, lmiP->idxB);
  hkey.prob = lmiP->prob;
  
  idx = listIndex((List*) &lmP->mlctList, (void*) &hkey,1);
  return (LCT) idx;
}


/* ------------------------------------------------------------------------
     extendLCT
   -------------
     Extend an LCT with an LVX, i.e. add another word to the history
   ------------------------------------------------------------------------ */

LCT interpolExtendLCT_hash (LingKS* lksP, LCT lct, LVX lvX)
{
  MetaLM*       lmP = lksP->data.metaLM;
  MetaLMItem*  lmiP = lmP->list.itemA+lvX;
  LCT          mlct = lct % lmP->mlctList.itemMax;
  MetaLCT*    mlctP = lmP->mlctList.itemA+ (int) mlct;
  LCT          lctA = mlctP->lctA;
  LCT          lctB = mlctP->lctB;
  MetaLCT      hkey;
  LingKS*      subP;
  int idx;

  /* Check for valid LCT */
  assert ((int) lct >= 0 && (int) lct < lmP->mlctList.itemN);

  /* check for hashed lct, lvx -> new map */
  if (lmP->lvxList.itemMax) {
    int        idx;
    HashLVX   hkey;
    hkey.lct = lct;
    hkey.lvx = lvX;

    if ((idx = listIndex ((List*) & lmP->lvxList, (void*) &hkey, 0)) >= 0) {
      idx = idx % lmP->lvxList.itemMax;
      return lmP->lvxList.itemA[idx]._new;
    }
  }

  /* Extend the LCTs : generate hashkey */
  subP      = lmP->subl.itemA[lmiP->lmA].lksP;
  hkey.lctA = subP->extendLCT (subP, lctA, lmiP->idxA);
  subP      = lmP->subl.itemA[lmiP->lmB].lksP;
  hkey.lctB = subP->extendLCT (subP, lctB, lmiP->idxB);
  hkey.prob = lmiP->prob;
  
  idx = listIndex((List*) &lmP->mlctList, (void*) &hkey,1);

  if (lmP->lvxList.itemMax) {
    HashLVX hkey;
    hkey.lct  =       lct;
    hkey._new  = (LCT) idx;
    hkey.lvx  =       lvX;
    listIndex ((List*) & lmP->lvxList, (void*) &hkey, 1);
  }

  return (LCT) idx;
}


/* ------------------------------------------------------------------------
     reduceLCT
   -------------
     Needed to find the reduced history in the case of short-range 
     lookahead models, i.e. if you want a bigram lookahead for a 
     trigram LM
   ------------------------------------------------------------------------ */

LCT interpolReduceLCT_hash (LingKS* lksP, LCT lct, int n)
{
  MetaLM*       lmP = lksP->data.metaLM;
  LCT          mlct = lct % lmP->mlctList.itemMax;
  MetaLCT*    mlctP = lmP->mlctList.itemA+ (int) mlct;
  LCT          lctA = mlctP->lctA;
  LCT          lctB = mlctP->lctB;
  MetaLCT hkey;
  LingKS* subP;
  int idx;

  WARN("interpolReduceLCT_hash: assume lmA=0, lmB=1\n");

  /* Reduce the LCTs : generate hashkey */
  subP = lmP->subl.itemA[0].lksP;
  hkey.lctA = subP->reduceLCT (subP,lctA,n);
  subP = lmP->subl.itemA[1].lksP;
  hkey.lctB = subP->reduceLCT (subP,lctB,n);
  hkey.prob = mlctP->prob;
  idx = listIndex((List*) &lmP->mlctList, (void*) &hkey,1);
  return (LCT) idx;

}


/* ------------------------------------------------------------------------
     decodeLCT
   -------------
     Decode an LCT into a sequence of LVX (words)
   ------------------------------------------------------------------------ */

int interpolDecodeLCT_hash (LingKS* lksP, LCT lct, LVX* lvxA, int n)
{
  MetaLM*         lmP = lksP->data.metaLM;
  MetaLMItem*    lmiP = lmP->list.itemA;
  LingKS*        subP = lmP->subl.itemA[lmiP->lmA].lksP;
  LCT            mlct = lct % lmP->mlctList.itemMax;
  MetaLCT*      mlctP = lmP->mlctList.itemA+ (int) mlct;
  return subP->decodeLCT (subP, mlctP->lctA, lvxA, n);
}


/* ------------------------------------------------------------------------
     freeLCT
   -----------
     This LCT is no longer needed, so a grammar could release some memory
   ------------------------------------------------------------------------ */

int interpolFreeLCT_hash (LingKS* lksP, LCT lct)
{
  return TCL_OK;
}


/* ------------------------------------------------------------------------
   Added By Wilson: use *log* linear interpol to combine multiple lm scores
     log_interpolScoreArray
   ----------------------
     Produces an array of scores using interpolation
     'usehistory' could be used to switch the LM to a 'fast' or
     short-range mode, however this function does not support this mode
   ------------------------------------------------------------------------ */

int log_interpolScoreArray_hash (LingKS *lksP, LCT lct, lmScore *arr, int n)
{
  MetaLM*          lmP = lksP->data.metaLM;
  MetaLMItem*     lmiP = lmP->list.itemA;
  LCT             mlct = lct % lmP->mlctList.itemMax;
  MetaLCT*       mlctP = lmP->mlctList.itemA+ (int) mlct;
  LingKS*         lksA = lmP->subl.itemA[lmiP->lmA].lksP;
  LingKS*         lksB = lmP->subl.itemA[lmiP->lmB].lksP;
  lmScore*        tmpA = NULL;
  lmScore*        tmpB = NULL;
  double             p = 0.0;
  double             q = 0.0;
  int                i;

  /* Initialization */
  tmpA  = lmP->tmpA  = (lmScore*) realloc (lmP->tmpA, lksA->itemN(lksA) * sizeof (lmScore));
  tmpB  = lmP->tmpB  = (lmScore*) realloc (lmP->tmpB, lksB->itemN(lksB) * sizeof (lmScore));

  /* Get scores from the base LMs */
  lksA->scoreArrayFct (lksA, mlctP->lctA, tmpA, n);
  lksB->scoreArrayFct (lksB, mlctP->lctB, tmpB, n);

  /* Loop over all entries */
  if (lksP->global_interpol) {
     p = lksA->interpol_wgt;
     q = lksB->interpol_wgt;
  } else {
     p = mlctP->prob;
     q = 1.0 - p;
  }

  for (i = 0; i < lmP->list.itemN; i++) {
    double a = tmpA[lmiP[i].idxA];
    double b = tmpB[lmiP[i].idxB];

    // direct log-linear interpol
    arr[i] = p * a + q * b;

/*
    if (a>b) {
      double s =  exp(M_LN10*(b-a)/LMSCORE_SCALE_FACTOR);
      arr[i]= a + LMSCORE_SCALE_FACTOR*(log(p + (1-p) * s)/M_LN10);
    } else {
      double s =  exp(M_LN10*(a-b)/LMSCORE_SCALE_FACTOR);
      arr[i]= b + LMSCORE_SCALE_FACTOR*(log((1-p) + p * s)/M_LN10);
    }
*/
  }

  return TCL_OK;
}


/* ------------------------------------------------------------------------
     interpolScoreArray
   ----------------------
     Produces an array of scores using interpolation
     'usehistory' could be used to switch the LM to a 'fast' or
     short-range mode, however this function does not support this mode
   ------------------------------------------------------------------------ */

int interpolScoreArray_hash (LingKS *lksP, LCT lct, lmScore *arr, int n)
{
  MetaLM*          lmP = lksP->data.metaLM;
  MetaLMItem*     lmiP = lmP->list.itemA;
  LCT             mlct = lct % lmP->mlctList.itemMax;
  MetaLCT*       mlctP = lmP->mlctList.itemA+ (int) mlct;
  LingKS*         lksA = lmP->subl.itemA[lmiP->lmA].lksP;
  LingKS*         lksB = lmP->subl.itemA[lmiP->lmB].lksP;
  lmScore*        tmpA = NULL;
  lmScore*        tmpB = NULL;
  double             p = 0.0;
  int                i;

  /* Initialization */
  tmpA  = lmP->tmpA  = (lmScore*) realloc (lmP->tmpA, lksA->itemN(lksA) * sizeof (lmScore));
  tmpB  = lmP->tmpB  = (lmScore*) realloc (lmP->tmpB, lksB->itemN(lksB) * sizeof (lmScore));

  /* Get scores from the base LMs */
  lksA->scoreArrayFct (lksA, mlctP->lctA, tmpA, n);
  lksB->scoreArrayFct (lksB, mlctP->lctB, tmpB, n);

  /* Loop over all entries */
  p = mlctP->prob;
  for (i = 0; i < lmP->list.itemN; i++) {
    double a = tmpA[lmiP[i].idxA];
    double b = tmpB[lmiP[i].idxB];
    if (a>b) {
      double s =  exp(M_LN10*(b-a)/LMSCORE_SCALE_FACTOR);
      arr[i]= a + LMSCORE_SCALE_FACTOR*(log(p + (1-p) * s)/M_LN10);
    } else {
      double s =  exp(M_LN10*(a-b)/LMSCORE_SCALE_FACTOR);
      arr[i]= b + LMSCORE_SCALE_FACTOR*(log((1-p) + p * s)/M_LN10);
    }
  }

  return TCL_OK;
}


/* ------------------------------------------------------------------------
     interpolScore
   -----------------
     Scores an LCT with an LVX using interpolation
     For the original implementation, see lmodelInt.c:lmodelIntScore
   ------------------------------------------------------------------------ */

lmScore interpolScore_hash (LingKS* lksP, LCT lct, LVX w, int n)
{
  MetaLM*          lmP = lksP->data.metaLM;
  MetaLMItem*     lmiP = lmP->list.itemA+w;
  LCT             mlct = lct % lmP->mlctList.itemMax;
  MetaLCT*       mlctP = lmP->mlctList.itemA+ (int) mlct;
  LingKS*         lksA = lmP->subl.itemA[lmiP->lmA].lksP;
  LingKS*         lksB = lmP->subl.itemA[lmiP->lmB].lksP;
  double sA, sB, alpha;

  sA    = UnCompressScore (lksA->scoreFct (lksA, mlctP->lctA, lmiP->idxA, n));
  sB    = UnCompressScore (lksB->scoreFct (lksB, mlctP->lctB, lmiP->idxB, n));
  alpha = mlctP->prob;

  /* Ensure sA >= sB */
  if (sA < sB) return CompressScore (sB + log ((1.0-alpha) + alpha * exp(M_LN10*(sA - sB))) / M_LN10);
  else         return CompressScore (sA + log (alpha + (1.0-alpha) * exp(M_LN10*(sB - sA))) / M_LN10);
}

lmScore log_uniform_interpolScore_hash (LingKS* lksP, LCT lct, LVX w, int n)
{
  MetaLM*          lmP = lksP->data.metaLM;
  MetaLMItem*     lmiP = lmP->list.itemA+w;
  LCT             mlct = lct % lmP->mlctList.itemMax;
  MetaLCT*       mlctP = lmP->mlctList.itemA+ (int) mlct;
  LingKS*         lksA = lmP->subl.itemA[lmiP->lmA].lksP;
  LingKS*         lksB = lmP->subl.itemA[lmiP->lmB].lksP;
  double sA, sB;

  sA    = UnCompressScore (lksA->scoreFct (lksA, mlctP->lctA, lmiP->idxA, n));
  sB    = UnCompressScore (lksB->scoreFct (lksB, mlctP->lctB, lmiP->idxB, n));

  /* log linear interpolation is done in log10 */
  return CompressScore (sA + sB);
}

lmScore log_interpolScore_hash (LingKS* lksP, LCT lct, LVX w, int n)
{
  MetaLM*          lmP = lksP->data.metaLM;
  MetaLMItem*     lmiP = lmP->list.itemA+w;
  LCT             mlct = lct % lmP->mlctList.itemMax;
  MetaLCT*       mlctP = lmP->mlctList.itemA+ (int) mlct;
  LingKS*         lksA = lmP->subl.itemA[lmiP->lmA].lksP;
  LingKS*         lksB = lmP->subl.itemA[lmiP->lmB].lksP;
  double sA, sB, alpha;

  sA    = UnCompressScore (lksA->scoreFct (lksA, mlctP->lctA, lmiP->idxA, n));
  sB    = UnCompressScore (lksB->scoreFct (lksB, mlctP->lctB, lmiP->idxB, n));

  if (lksP->global_interpol) {
     float p = lksA->interpol_wgt;
     float q = lksB->interpol_wgt;

     return CompressScore (p * sA + q * sB);
  } else {
     // use weight prescribed in weight field
     alpha = mlctP->prob;
     /* log linear interpolation is done in log10 */
     return CompressScore (alpha * sA + (1.0-alpha) * sB);
  }

  /* Ensure sa>=sB */
/*
  if (sA < sB) return CompressScore (sB + log ((1.0-alpha) + alpha * exp(M_LN10*(sA - sB))) / M_LN10);
  else         return CompressScore (sA + log (alpha + (1.0-alpha) * exp(M_LN10*(sB - sA))) / M_LN10);
*/
}


/* ------------------------------------------------------------------------
     setInterpolScoreFct
   -----------------------
     Sets all pointers to the 'hash' score function
   ------------------------------------------------------------------------ */

int log_uniform_setInterpolScoreFct_hash (MetaLM* lmP)
{
  LingKS         *lksP = lmP->lksP;

  setInterpolScoreFct_hash (lmP);

  /* use log-linear combination (log ratio based) */
  lksP->scoreFct       = log_uniform_interpolScore_hash;
  
  return TCL_OK;
}


int log_setInterpolScoreFct_hash (MetaLM* lmP)
{
  LingKS         *lksP = lmP->lksP;

  setInterpolScoreFct_hash (lmP);

  /* use log-linear combination */
  lksP->scoreFct       = log_interpolScore_hash;
  lksP->scoreArrayFct  = log_interpolScoreArray_hash;
  
  return TCL_OK;
}

/* END Wilson 15 Nov 05 */

/* Start Wilson 16 May 05 */

lmScore interpolScore_hash_LSA (LingKS* lksP, LCT lct, LVX w, int n)
{
  MetaLM*          lmP = lksP->data.metaLM;
  MetaLMItem*     lmiP = lmP->list.itemA+w;
  LCT             mlct = lct % lmP->mlctList.itemMax;
  MetaLCT*       mlctP = lmP->mlctList.itemA+ (int) mlct;
  LingKS*         lksA = lmP->subl.itemA[lmiP->lmA].lksP;
  LingKS*         lksB = lmP->subl.itemA[lmiP->lmB].lksP;
  double sA, sB, alpha;

  /* if w is a stopword, use the first LM for score computation */
  if (MetaLMItemAIsStopWord (lmP, w))
     return lksA->scoreFct (lksA, mlctP->lctA, lmiP->idxA, n);

  sA    = UnCompressScore (lksA->scoreFct (lksA, mlctP->lctA, lmiP->idxA, n));
  sB    = UnCompressScore (lksB->scoreFct (lksB, mlctP->lctB, lmiP->idxB, n));
  alpha = mlctP->prob;

  /* Ensure sa>=sB */
  if (sA < sB) return CompressScore (sB + log ((1.0-alpha) + alpha * exp(M_LN10*(sA - sB))) / M_LN10);
  else         return CompressScore (sA + log (alpha + (1.0-alpha) * exp(M_LN10*(sB - sA))) / M_LN10);

}

/* ------------------------------------------------------------------------
     setInterpolScoreFct
   -----------------------
     Sets all pointers to the 'hash' score function
   ------------------------------------------------------------------------ */

int setInterpolScoreFct_hash_LSA (MetaLM* lmP)
{
  LingKS         *lksP = lmP->lksP;

  setInterpolScoreFct_hash (lmP);

  /* skip stopword computation in LSA model */
  lksP->scoreFct       = interpolScore_hash_LSA;
  
  return TCL_OK;
}

/* ------------------------------------------------------------------------
     setInterpolScoreFct
   -----------------------
     Sets all pointers to the 'hash' score function
   ------------------------------------------------------------------------ */


int setInterpolScoreFct_hash (MetaLM* lmP)
{
  LingKS         *lksP = lmP->lksP;

  lksP->scoreArrayFct  = interpolScoreArray_hash;
  lksP->scoreFct       = interpolScore_hash;
  lksP->createLCT      = interpolCreateLCT_hash;
  lksP->reduceLCT      = interpolReduceLCT_hash;
  lksP->extendLCT      = interpolExtendLCT_hash;
  lksP->decodeLCT      = interpolDecodeLCT_hash;
  lksP->freeLCT        = interpolFreeLCT_hash;

  lmP->mem.subN        = 2;
  
  return TCL_OK;
}


/* ========================================================================
   ==========================
     LM Interface functions
   ==========================
   ======================================================================== */

static LVX itemX (LingKS *lksP, char *name)
{
  MetaLM *lmP = lksP->data.metaLM;
  int l = listIndex ((List*)&(lmP->list), name, 0);
  if (l == -1) return LVX_NIL;
  return (LVX) l;
}

static char *itemA (LingKS* lksP, int i)
{
  return lksP->data.metaLM->list.itemA[i].name;
}

static int itemN (LingKS* lksP)
{
  return lksP->data.metaLM->list.itemN;
}

static int MetaLMIsDirty (LingKS* lksP)
{
  MetaLM* lmP = lksP->data.metaLM;
  int   dirty = lksP->dirty;
  int i;
  for (i = 0; i < lmP->subl.itemN; i++) 
    dirty = dirty || (lmP->subl.itemA+i)->lksP->isDirty ((lmP->subl.itemA+i)->lksP);
  lksP->dirty = FALSE;
  return dirty;
}

/* ------------------------------------------------------------------------
    MetaLMItemConfigureItf
   ------------------------
    Configuration interface for an LMItem (a word)
   ------------------------------------------------------------------------ */

static int MetaLMItemConfigureItf (ClientData cd, char *var, char *val)
{
  MetaLMItem* lmiP = (MetaLMItem*)cd;
  if (!lmiP) return TCL_ERROR;

  if (!var) {
    ITFCFG (MetaLMItemConfigureItf, cd, "name");
    ITFCFG (MetaLMItemConfigureItf, cd, "idxA");
    ITFCFG (MetaLMItemConfigureItf, cd, "idxB");
    ITFCFG (MetaLMItemConfigureItf, cd, "lmA");
    ITFCFG (MetaLMItemConfigureItf, cd, "lmB");
    ITFCFG (MetaLMItemConfigureItf, cd, "prob");
    return TCL_OK;
  }

  ITFCFG_CharPtr (var, val, "name", lmiP->name, 1);
  ITFCFG_Int     (var, val, "idxA", lmiP->idxA, 0);
  ITFCFG_Int     (var, val, "idxB", lmiP->idxB, 0);
  ITFCFG_Int     (var, val, "lmA",  lmiP->lmA,  0);
  ITFCFG_Int     (var, val, "lmB",  lmiP->lmB,  0);
  ITFCFG_Float   (var, val, "prob", lmiP->prob, 0);

  ERROR ("Unknown option '-%s %s'.\n", var, val ? val : "?");

  return TCL_ERROR;
}


/* ------------------------------------------------------------------------
    MetaLMElemConfigureItf
   ------------------------
    Configuration interface for an LMElem (an underlying LM)
   ------------------------------------------------------------------------ */

static int MetaLMElemConfigureItf (ClientData cd, char *var, char *val)
{
  MetaLMElem* lmeP = (MetaLMElem*)cd;
  if (!lmeP) return TCL_ERROR;

  if (!var) {
    ITFCFG (MetaLMElemConfigureItf, cd, "name");
    ITFCFG (MetaLMElemConfigureItf, cd, "weight");
    return TCL_OK;
  }

  lmeP->lksP->dirty = TRUE;

  ITFCFG_CharPtr (var, val, "name",   lmeP->name,   1);
  ITFCFG_Float   (var, val, "weight", lmeP->weight, 0);


  ERROR ("Unknown option '-%s %s'.\n", var, val ? val : "?");

  return TCL_ERROR;
}


/* ------------------------------------------------------------------------
    MetaLMItemPuts
   ----------------
    Print out the content of a word
   ------------------------------------------------------------------------ */

static int MetaLMItemPutsItf (ClientData cd, int argc, char *argv[])
{
  int           ac = argc-1;
  MetaLMItem* lmiP = (MetaLMItem*) cd;

  if (itfParseArgv( argv[0], &ac, argv+1, 0, NULL) != TCL_OK)
      return TCL_ERROR;

  itfAppendElement ("%s %d %d %d %d %f",
		    lmiP->name, lmiP->idxA, lmiP->idxB, lmiP->lmA, lmiP->lmB, lmiP->prob);

  return TCL_OK;
}

static int MetaLMGetItf (ClientData cd, int argc, char *argv[])
{
  int           ac = argc-1;
  MetaLM*      lmP = (MetaLM*) cd;
  MetaLMItem* lmiP = NULL;
  char          *w = NULL;
  LVX          lvX = 0;

  if (itfParseArgv (argv[0], &ac, argv+1, 0,
    "<word>", ARGV_STRING, NULL, &w,  NULL, "item", NULL) != TCL_OK)
     return TCL_ERROR;

  if ((lvX = itemX (lmP->lksP, w)) != LVX_NIL) {
    lmiP = lmP->list.itemA+lvX;
    itfAppendElement ("%s %d %d %d %d %f",
		      lmiP->name, lmiP->idxA, lmiP->idxB, lmiP->lmA, lmiP->lmB, lmiP->prob);
  }

  return TCL_OK;
}


/* ------------------------------------------------------------------------
    MetaLMElemPuts
   ----------------
    Print out the content of an underlying LM
   ------------------------------------------------------------------------ */

static int MetaLMElemPutsItf (ClientData cd, int argc, char *argv[])
{
  int           ac = argc-1;
  MetaLMElem* lmeP = (MetaLMElem*) cd;

  if (itfParseArgv( argv[0], &ac, argv+1, 0, NULL) != TCL_OK)
      return TCL_ERROR;

  itfAppendElement ("%s", lmeP->name);

  return TCL_OK;
}


/* ========================================================================
    Interesting functions
   ======================================================================== */

/* ------------------------------------------------------------------------
    MetaLMSave
   ------------
    Dump the contents of the LM to a file
   ------------------------------------------------------------------------ */

extern int MetaLMSave (LingKS* lksP, FILE* fp)
{
  int       i;
  MetaLM* lmP = lksP->data.metaLM;
 
  /* Hello world & type/ version information*/
  write_string   (fp, "MetaLM-BEGIN");
  write_string   (fp, "Interpol_v1");

  /* Basic parameters */
  write_int      (fp, lmP->order);
  write_int      (fp, lmP->subl.itemN);
  write_int      (fp, lmP->list.itemN);

  /* Elements */
  for (i = 0; i < lmP->subl.itemN; i++) {
    write_string (fp, lmP->subl.itemA[i].name);
    write_float  (fp, lmP->subl.itemA[i].weight);
  }

  /* Items (list) */
  for (i = 0; i < lmP->list.itemN; i++) {
    MetaLMItem* lmiP = lmP->list.itemA+i;
    write_string (fp, lmiP->name);
    write_int    (fp, lmiP->lmA);
    write_int    (fp, lmiP->lmB);
    write_int    (fp, lmiP->idxA);
    write_int    (fp, lmiP->idxB);
    write_float  (fp, lmiP->prob);
  }

  /* Bye, bye */
  write_string   (fp, "MetaLM-END");

  return TCL_OK;
}

int MetaLMSaveItf (ClientData cd, int argc, char *argv[]) {

  MetaLM *lmP  = (MetaLM*)cd;
  int      ac   = argc - 1;
  char    *name = NULL;
  FILE    *fp   = NULL;
  int      res  = TCL_OK;

  if (itfParseArgv (argv[0], &ac, argv+1, 0,
      "<filename>", ARGV_STRING, NULL, &name, NULL, "file to save (dump) LM to",
      NULL) != TCL_OK) return TCL_ERROR;

  if ( !(fp = fileOpen (name, "w")) ) {
    ERROR ("Could not open '%s' for writing.\n", name);
    return TCL_ERROR;
  }

  res = MetaLMSave (lmP->lksP, fp);

  fileClose (name, fp);

  return res;
}


/* ------------------------------------------------------------------------
    MetaLMLoad
   ------------
    Read the contents of a dumped LM
   ------------------------------------------------------------------------ */

extern int MetaLMLoad (LingKS* lksP, FILE* fp, char *txt) 
{
  MetaLM* lmP = lksP->data.metaLM;
  int i, itemN, elemN;
  char str[MAXLMLINE];

  /* Consistency checks */
  if (lmP->list.itemN != 1 || lmP->subl.itemN) {
    ERROR ("Can only load into empty LMs (not %d, %d).\n", lmP->list.itemN, lmP->subl.itemN); 
    return TCL_ERROR;
  } else if (!fp) {
    ERROR ("Invalid file pointer.\n");
    return TCL_ERROR;
  } else if (txt) {
    if (strcmp (str, "MetaLM-BEGIN")) {
      ERROR ("Inconsistent file.\n");
      return TCL_ERROR;
    }
  } else {
    read_string  (fp, str);
    if (strcmp (str, "MetaLM-BEGIN")) {
      ERROR ("Inconsistent file.\n");
      return TCL_ERROR;
    }
  }
  read_string    (fp, str);
  if (strcmp (str, "Interpol_v1")) {
    ERROR ("Cannot read MetaLM file of type and version '%s.\n", str);
    return TCL_ERROR;
  }

  /* Basic parameters */
  lmP->order   = read_int (fp);
  elemN        = read_int (fp);
  itemN        = read_int (fp);

  /* Elements */
  for (i = 0; i < elemN; i++) {
    read_string  (fp, str);

    /* (Sanity) checks */
    if (i != listIndex ((List*)&(lmP->subl), str, 1)) {
      WARN ("Indexing doesn't look good for lm '%s' != %d.\n", str, i);
      return TCL_ERROR;
    }

    lmP->subl.itemA[i].lksP   = lksP;
    lmP->subl.itemA[i].weight = read_float (fp);

    link (lksP, "LingKS");
  }

  /* Items (list) */
  for (i = 0; i < itemN; i++) {
    read_string (fp, str);
    if (i != listIndex ((List*)&(lmP->list), str, 1)) {
      WARN ("Indexing doesn't look good for word '%s' != %d.\n", str, i);
      return TCL_ERROR;
    }
    lmP->list.itemA[i].lmA  = read_int   (fp);
    lmP->list.itemA[i].lmB  = read_int   (fp);
    lmP->list.itemA[i].idxA = read_int   (fp);
    lmP->list.itemA[i].idxB = read_int   (fp);
    lmP->list.itemA[i].prob = read_float (fp);
  }

  /* Bye, bye */
  read_string    (fp, str);
  if (strcmp (str, "MetaLM-END")) {
    WARN ("Didn't find end token.\n");
    return TCL_ERROR;
  }

  return TCL_OK;
}

static int MetaLMLoadItf (ClientData cd, int argc, char *argv[]) {

  MetaLM *lmP      = (MetaLM*)cd;
  char    *name     = NULL;
  int      ac       = argc - 1;
  int      r        = 0;
  FILE    *fp       = NULL;

  if (itfParseArgv(argv[0], &ac, argv+1, 0, 
      "<filename>", ARGV_STRING, NULL, &name,     cd,   "file to read from", 
      NULL) != TCL_OK) return TCL_ERROR;

  /* Try to open the file */
  if ( !(fp = fileOpen (name, "r")) ) {
    ERROR ("Cannot open file '%s' for reading.\n", name);
    return TCL_ERROR;
  }

  r = MetaLMLoad (lmP->lksP, fp, NULL);

  fileClose (name, fp);

  return r;
}


/* ------------------------------------------------------------------------
    MetaLMScoreFctItf
   -------------------
    This LM could be used to implement a number of different score
    functions, i.e. combining LMs in numerous ways. The function to be
    used is configureable via this procedure. However, as of 2001-10-03,
    only a function 'interpol' is implemented
   ------------------------------------------------------------------------ */

int MetaLMScoreFctItf (ClientData cd, int argc, char * argv[])
{
  MetaLM*   lmP = (MetaLM*) cd;
  int        ac = argc-1;
  char*     fct = NULL;

  if (itfParseArgv (argv[0], &ac, argv+1, 0,
     "<function>", ARGV_STRING, NULL, &fct, NULL, "score function (one of: current, interpol, hash, hash+LSA, hash+log)", 
     NULL) != TCL_OK) {
    return TCL_ERROR;
  }

  if        (streq (fct, "interpol")) {
    setInterpolScoreFct (lmP);
    itfAppendResult ("interpol");

  } else if (streq (fct, "hash")) {
    setInterpolScoreFct_hash (lmP);
    itfAppendResult ("hash");
  } else if (streq (fct, "hash+log_unif")) {

     /* log uniform interpolation */
    log_uniform_setInterpolScoreFct_hash(lmP);
    itfAppendResult ("hash+log_unif");

  } else if (streq (fct, "hash+log")) {
     /* log linear interpolation */
    log_setInterpolScoreFct_hash(lmP);
    itfAppendResult ("hash+log");

  } else if (streq (fct, "hash+LSA")) {
     /* add a new interpol function in which scores of stopwords are computed using only the BG ngram LM (must be the first added LM) */
    setInterpolScoreFct_hash_LSA (lmP);
    itfAppendResult ("hash+LSA");

  } else if (streq (fct, "current") || streq (fct, "?")) {
    if      (lmP->lksP->scoreFct == interpolScore)
      itfAppendResult ("interpol");
    else if (lmP->lksP->scoreFct == interpolScore_hash)
      itfAppendResult ("hash");
    else if (lmP->lksP->scoreFct == interpolScore_hash_LSA)
      itfAppendResult ("hash+LSA");
    else if (lmP->lksP->scoreFct == log_interpolScore_hash)
      itfAppendResult ("hash+log");
    else
      itfAppendResult ("N/A");

  } else {
    WARN ("Unknown score function '%s':\n", fct);
    WARN ("%s\n", metaLMRCSVersion);
    return TCL_ERROR;
  }
    
  lmP->lksP->dirty = TRUE;

  return TCL_OK;
}


/* ------------------------------------------------------------------------
    MetaLMAdd
   -----------
    Add an item to the meta language model
   ------------------------------------------------------------------------ */

static LVX metaLMAdd (MetaLM* lmP, char *name, int lmA, int lmB, char *nameA, char *nameB, float prob)
{
  int idx;

  if (lmA < 0 || lmA >= lmP->subl.itemN) {
    WARN ("Index '%d' for -lksA out of range.\n", lmA);
    return LVX_NIL;
  } else if (lmB < 0 || lmB >= lmP->subl.itemN) {
    WARN ("Index '%d' for -lksB out of range.\n", lmB);
    return LVX_NIL;
  } else if (lmP->subl.itemA[lmA].lksP->itemX (lmP->subl.itemA[lmA].lksP, nameA) == LVX_NIL) {
    WARN ("Word '%s' not found in LM '%s' (index '%d').\n", nameA, lmP->subl.itemA[lmA].name, lmA);
    return LVX_NIL;
  } else if (lmP->subl.itemA[lmB].lksP->itemX (lmP->subl.itemA[lmB].lksP, nameB) == LVX_NIL) {
    WARN ("Word '%s' not found in LM '%s' (index '%d').\n", nameB, lmP->subl.itemA[lmB].name, lmB);
    return LVX_NIL;
  } else if (itemX (lmP->lksP, name) != LVX_NIL) {
    idx = (int) itemX (lmP->lksP, name);
  } else if ((idx = listNewItem ((List*)&(lmP->list), (ClientData) name)) == -1) {
    WARN ("Couldn't allocate list for word '%s'.\n", name);
    return LVX_NIL;
  }

  lmP->list.itemA[idx].lmA  = (unsigned char) lmA;
  lmP->list.itemA[idx].lmB  = (unsigned char) lmB;
  lmP->list.itemA[idx].idxA = lmP->subl.itemA[lmA].lksP->itemX (lmP->subl.itemA[lmA].lksP, nameA);
  lmP->list.itemA[idx].idxB = lmP->subl.itemA[lmB].lksP->itemX (lmP->subl.itemA[lmB].lksP, nameB);
  lmP->list.itemA[idx].prob = prob;

  lmP->lksP->dirty = TRUE;

  return (LVX) idx;
}

static int MetaLMAddItf (ClientData cd, int argc, char * argv[])
{
  MetaLM *lmP = (MetaLM*) cd;
  int      ac = argc-1;
  char     *w = NULL, *nA = NULL, *nB = NULL;
  int     lmA = 0, lmB = 1;
  LVX     idx = LVX_NIL;
  float     p = 0;  

  if (itfParseArgv (argv[0], &ac, argv+1, 0,
      "<LM word>",    ARGV_STRING, NULL, &w,   NULL,             "LM word in this model",
      "-lksA",        ARGV_INT,    NULL, &lmA, NULL,             "Language Model A",
      "-lksB",        ARGV_INT,    NULL, &lmB, NULL,             "Language Model B",
      "-nameA",       ARGV_STRING, NULL, &nA,  NULL,             "corresponding word in LM A",
      "-nameB",       ARGV_STRING, NULL, &nB,  NULL,             "corresponding word in LM B",
      "-prob",        ARGV_FLOAT,  NULL, &p,   NULL,             "probability",
        NULL) != TCL_OK) return TCL_ERROR;

  if (!nA) nA = w;
  if (!nB) nB = w;

  if ((idx = metaLMAdd (lmP, w, lmA, lmB, nA, nB, p)) == LVX_NIL) {
    itfAppendResult ("-1");
    return TCL_ERROR;
  }

  itfAppendResult ("%d", idx);
  return TCL_OK;
}


/* ------------------------------------------------------------------------
    MetaLMCoverItf
   ---------------
    Read all words from an atomic LM and try to map them
   ------------------------------------------------------------------------ */

static int MetaLMCoverItf (ClientData cd, int argc, char *argv[])
{
  MetaLM *lmP = (MetaLM*) cd;
  int      ac = argc-1;
  int     lmA = 0, lmB = 1;
  float     p = 0.0;
  int     max = lmP->subl.itemA[lmA].lksP->itemN (lmP->subl.itemA[lmA].lksP);
  LVX     lvX;

  if (itfParseArgv (argv[0], &ac, argv+1, 0,
      "-lksA",         ARGV_INT,    NULL, &lmA, NULL, "index of atomic LM to read words from",
      "-lksB",         ARGV_INT,    NULL, &lmB, NULL, "index of atomic LM to connect with",
      "-prob",        ARGV_FLOAT,  NULL, &p,   NULL, "probability",
        NULL) != TCL_OK) return TCL_ERROR;

  if (lmA < 0 || lmA >= lmP->subl.itemN || lmB < 0 || lmB >= lmP->subl.itemN) {
    ERROR ("My penguin can type better than that!\n");
    return TCL_ERROR;
  }

  for (lvX = 0; lvX < max; lvX++) {
    char* name = lmP->subl.itemA[lmA].lksP->itemA (lmP->subl.itemA[lmA].lksP, lvX);
    if (metaLMAdd (lmP, name, lmA, lmB, name, name, p) == LVX_NIL)
      itfAppendResult ("%s", name);
  }

  return TCL_OK;
}


/* ========================================================================
    Object functions
   ======================================================================== */

/* ------------------------------------------------------------------------
    MetaLMItemInit/ DeInit
   ------------------------------------------------------------------------ */

int MetaLMItemInit (MetaLMItem* lmiP, ClientData cd)
{
  assert (lmiP);
  lmiP->name = (cd) ? strdup ((char*) cd) : NULL;
  return TCL_OK;
}

int MetaLMItemDeinit (MetaLMItem* lmiP)
{
  assert (lmiP);
  if (lmiP->name) Nulle (lmiP->name);
  return TCL_OK;
}


/* ------------------------------------------------------------------------
    MetaLMElemInit/ DeInit
   ------------------------------------------------------------------------ */

int MetaLMElemInit (MetaLMElem* lmeP, ClientData cd)
{
  assert (lmeP);
  lmeP->name = (cd) ? strdup ((char*) cd) : NULL;
  return TCL_OK;
}

int MetaLMElemDeinit (MetaLMElem* lmeP)
{
  assert (lmeP);
  if (lmeP->name) Nulle (lmeP->name);
  return TCL_OK;
}


/* ------------------------------------------------------------------------
    MetaLMInit/ DeInit
   ------------------------------------------------------------------------ */

int MetaLMLinkN (MetaLM* lmP)
{
  int useN = listLinkN((List*)&(lmP->list)) - 1;
  if (useN < lmP->lksP->useN) return lmP->lksP->useN;
  else                        return            useN;
}

int MetaLMInit (LingKS* lksP, MetaLM* lmP, char* name)
{

  /*
  ** Initialize the data structure itself
  */
  lmP->order           = -1;
  lmP->tmpA            = NULL;
  lmP->tmpB            = NULL;
  lmP->lksP            = lksP;

  setInterpolScoreFct_hash(lmP);
  lksP->lingKSSaveFct    = MetaLMSave;
  lksP->lingKSLoadFct    = MetaLMLoad;
  lksP->lingKSSaveItfFct = MetaLMSaveItf;
  lksP->lingKSLoadItfFct = MetaLMLoadItf;
  lksP->lingKSPutsFct    = MetaLMPutsItf;
  lksP->itemN            = itemN;
  lksP->itemA            = itemA;
  lksP->itemX            = itemX;
  lksP->isDirty          = MetaLMIsDirty;

  /* Init the list of items (words) */
  if (listInit((List*)&(lmP->list), itfGetType ("MetaLMItem"), 
	       sizeof (MetaLMItem), 1000) != TCL_OK) {
    ERROR("Couldn't allocate item list.");
    return TCL_ERROR;
  }
  lmP->list.init    = (ListItemInit  *)MetaLMItemInit;
  lmP->list.deinit  = (ListItemDeinit*)MetaLMItemDeinit;

  listIndex ((List*)&(lmP->list), "<UNK>", 1);

  /* Init the list of sub-LMs */
  if (listInit((List*)&(lmP->subl), itfGetType ("MetaLMElem"), 
	       sizeof (MetaLMElem), 10) != TCL_OK) {
    ERROR("Couldn't allocate element list.");
    return TCL_ERROR;
  }
  lmP->subl.init    = (ListItemInit  *)MetaLMElemInit;
  lmP->subl.deinit  = (ListItemDeinit*)MetaLMElemDeinit;

  /* Init the MetaLMPathItems to keep track of LCTs */
  lmP->mem.node = bmemCreate (16384, sizeof (GNode));
  lmP->mem.lct  = bmemCreate (16384, 0);

  /* Init the lvX hash table */
  listInit ((List*) &(lmP->lvxList), NULL, sizeof (HashLVX), 20000);
  lmP->lvxList.init    = (ListItemInit *)hashlvxInit;
  lmP->lvxList.deinit  = NULL;
  lmP->lvxList.hashKey = (HashKeyFn    *)hashlvxHashKey;
  lmP->lvxList.hashCmp = (HashCmpFn    *)hashlvxHashCmp;
  lmP->lvxList.itemMax = 0;

  metaLMInit_interpol (lksP);
  metaLMInit_hash     (lksP);

  return TCL_OK;
}


int MetaLMDeinit (MetaLM* lmP)
{
  int i;

  assert (lmP);

  if (MetaLMLinkN (lmP)) {
    SERROR ("MetaLM '%s' still in use by other objects.\n", lmP->lksP->name);
    return TCL_ERROR;
  }

  bmemFree (lmP->mem.node);
  bmemFree (lmP->mem.lct);
  if (lmP->tmpA) free(lmP->tmpA);
  if (lmP->tmpB) free(lmP->tmpB);
  lmP->tmpA = NULL;
  lmP->tmpB = NULL;

  for (i = 0; i < lmP->subl.itemN; i++)
    delink (lmP->subl.itemA[i].lksP, "LingKS");

  if (listDeinit ((List*)&(lmP->subl)) == TCL_ERROR) 
    WARN ("Something went wrong during deinitialization of the elements list.\n");

  listDeinit((List*) &(lmP->mlctList));

  return listDeinit ((List*) &(lmP->list));
}


/* ------------------------------------------------------------------------
    MetaLMConfigureItf
   ------------------------------------------------------------------------ */

extern int MetaLMConfigureItf (ClientData cd, char *var, char *val)
{
  MetaLM* lmP = (MetaLM*) cd;

  if (!var) {
    ITFCFG (MetaLMConfigureItf, cd, "elemN");
    ITFCFG (MetaLMConfigureItf, cd, "order");
    ITFCFG (MetaLMConfigureItf, cd, "mlctN");
    ITFCFG (MetaLMConfigureItf, cd, "mlctMax");
    ITFCFG (MetaLMConfigureItf, cd, "lvxCache");
    ITFCFG (MetaLMConfigureItf, cd, "lvxCacheN");
    return listConfigureItf ((ClientData)&(lmP->list), var, val);
  }

  ITFCFG_Int     (var, val, "elemN",       lmP->subl.itemN,       1);
  ITFCFG_Int     (var, val, "order",       lmP->order,            1);
  ITFCFG_Int     (var, val, "mlctN",       lmP->mlctList.itemN,   1);
  ITFCFG_Int     (var, val, "mlctMax",     lmP->mlctList.itemMax, 0);
  ITFCFG_Int     (var, val, "lvxCacheN",   lmP->lvxList.itemN,    1);
  ITFCFG_Int     (var, val, "lvxCache",    lmP->lvxList.itemMax,  0);

  return listConfigureItf ((ClientData)&(lmP->list), var, val);   
}


/* ------------------------------------------------------------------------
    MetaLMAccessItf
   ------------------------------------------------------------------------ */

extern ClientData MetaLMAccessItf (ClientData cd, char *name, TypeInfo **ti)
{ 
  MetaLM* lmP = (MetaLM*) cd;
  int     lmX = 0;

  if (*name == '.') { 
    if (name[1]=='\0') {
      if (lmP->subl.itemN)
	itfAppendElement("lm(0..%d)", lmP->subl.itemN-1);
    } else { 
      if (sscanf (name+1, "lm(%d)", &lmX) == 1) {
	*ti = itfGetType ("MetaLMElem");
	if (lmX >= 0 && lmX < lmP->subl.itemN) 
	  return (ClientData) &(lmP->subl.itemA[lmX]);
      }
    }
  }

  return (ClientData) listAccessItf ((ClientData)&(lmP->list), name, ti);
}


/* ------------------------------------------------------------------------
    MetaLMPutsItf
   ---------------
    Prints out the items
   ------------------------------------------------------------------------ */

static int MetaLMPutsItf (ClientData cd, int argc, char *argv[])
{
  MetaLM *lmP  = (MetaLM*) cd;
  return listPutsItf((ClientData)&(lmP->list), argc, argv);
}

/* return 1 if the word is a stopword */
static int MetaLMItemAIsStopWord (MetaLM* lmP, int i)
{
  return (i < 0) ? 0 : (int) lmP->list.itemA[i].isStopWord;
}

/* Load stopword list into the */
static int MetaLMLoadStopListItf (ClientData cd, int argc, char *argv[])
{
  MetaLM *lmP = (MetaLM*) cd;
  int       ac =  argc - 1;
  char* filename = 0;
  char  word[512];
  int i;
  int widx = -1;
  FILE *fp = 0;

  if (itfParseArgv (argv[0], &ac, argv+1, 0, 
      "<filename>", ARGV_STRING, NULL, &filename, cd, "path to the stopword list", 
      NULL) != TCL_OK) return TCL_ERROR;

  if ( !(fp = fileOpen (filename, "r")) ) {
    ERROR ("Could not open stopword list '%s' for reading.\n", filename);
    return TCL_ERROR;
  }

  /* clear previous flags */
  for (i=0; i < lmP->list.itemN; i++) {
     lmP->list.itemA[i].isStopWord = 0;
  }

  while ( !feof(fp) && fscanf(fp, "%s", word) ) {
     /* find integer word idx */
     if ((widx = itemX (lmP->lksP, word)) != LVX_NIL) {

        /* mark stopword */
        lmP->list.itemA[widx].isStopWord = 1;
     }
  }

  fileClose(filename, fp);

  return TCL_OK;
}

static int MetaLMIsStopWordItf(ClientData cd, int argc, char *argv[])
{
  MetaLM  *lmP = (MetaLM*) cd;
  int       ac =  argc - 1;
  char*   word = NULL;
  int     widx = -1;

  if (itfParseArgv (argv[0], &ac, argv+1, 0, 
      "<word>", ARGV_STRING, NULL, &word, NULL, "a word", 
      NULL) != TCL_OK) return TCL_ERROR;

  if ((widx = itemX (lmP->lksP, word)) != LVX_NIL)
     itfAppendResult ("%d", MetaLMItemAIsStopWord(lmP, widx));
  else
     itfAppendResult ("0");

  return TCL_OK;
}


/* ========================================================================
    Sub-Object functions
   ----------------------
    The corresponding functions for items are somewhat distributed
    across the code
   ======================================================================== */

static int MetaLMElemAddItf (ClientData cd, int argc, char * argv[])
{
  MetaLM    *lmP = (MetaLM*) cd;
  int         ac = argc-1;
  char       *lm = NULL;
  int        idx = -1;
  float   weight = 1.0;
  LingKS*   lksP = NULL;
  
  if (itfParseArgv (argv[0], &ac, argv+1, 0,
      "<LingKS>",     ARGV_OBJECT, NULL, &lksP,   "LingKS", "Linguistic Knowledge Source",
      "-weight",      ARGV_FLOAT,  NULL, &weight, NULL,     "weight",
        NULL) != TCL_OK) return TCL_ERROR;

  /* Sanity checks */
  lm = lksP->name;
  if (listIndex ((List*)&(lmP->subl), lm, 0) != -1) {
    WARN ("Language model '%s' already exists.\n", lm);
    return TCL_ERROR;
  } else if ((idx = listNewItem ((List*)&(lmP->subl), (ClientData) lm)) == -1) {
    WARN ("Couldn't allocate new list element for '%s'.\n", lm);
    return TCL_ERROR;
  }

  lmP->subl.itemA[idx].lksP   = lksP;
  lmP->subl.itemA[idx].weight = weight;

// set interpol weight to sublm
  lksP->interpol_wgt = weight;

  /* Link the NULL word to the first LM */
  if (!idx) {
    lmP->list.itemA[0].lmA  = 0;
    lmP->list.itemA[0].lmB  = 0;
    lmP->list.itemA[0].idxA = 0;
    lmP->list.itemA[0].idxB = 0;
    lmP->list.itemA[0].prob = 0.0;
  }

  link (lksP, "LingKS");

  itfAppendResult ("%d", idx);
  return TCL_OK;
}

static int MetaLMElemIndexItf (ClientData clientData, int argc, char *argv[])
{
  MetaLM* lmP = (MetaLM*) clientData;
  return listName2IndexItf ((ClientData)&(lmP->subl), argc, argv);
}

static int MetaLMElemNameItf (ClientData clientData, int argc, char *argv[] )
{
  MetaLM* lmP = (MetaLM*) clientData;
  return listIndex2NameItf ((ClientData)&(lmP->subl), argc, argv);
}

static int MetaLMElemListItf (ClientData cd, int argc, char * argv[])
{
  MetaLM *lmP  = (MetaLM*) cd;
  return listPutsItf ((ClientData)&(lmP->subl), argc, argv);
}


/* ========================================================================
    Type Information
   ======================================================================== */

static Method MetaLMItemMethod[] = { 
  { "puts", MetaLMItemPutsItf, NULL },
  {  NULL, NULL, NULL } 
};

TypeInfo MetaLMItemInfo = { 
  "MetaLMItem", 0, -1, MetaLMItemMethod, 
  NULL, NULL,
  MetaLMItemConfigureItf, NULL,
  itfTypeCntlDefaultNoLink,
  "Meta language model item.\n" 
};

static Method MetaLMElemMethod[] = { 
  { "puts",  MetaLMElemPutsItf, NULL },
  {  NULL, NULL, NULL } 
};

TypeInfo MetaLMElemInfo = { 
  "MetaLMElem", 0, -1, MetaLMElemMethod, 
  NULL, NULL,
  MetaLMElemConfigureItf, NULL,
  itfTypeCntlDefaultNoLink,
  "Meta language model element (sub-LM).\n" 
};

static Method MetaLMMethod[] = { 
  { "puts",      MetaLMPutsItf,      "display the contents of a MetaLM"              },
  { "add",       MetaLMAddItf,       "add an item (using atomic LMs)"                },
  { "get",       MetaLMGetItf,       "get the parameters for one item"               },
  { "list",      MetaLMElemListItf,  "list the currently available LMs"              },
  { "LMadd",     MetaLMElemAddItf,   "add a language model for usage with metaLM"    },
  { "LMindex",   MetaLMElemIndexItf, "return the internal index of an atomic LM"     },
  { "LMname",    MetaLMElemNameItf,  "return the name of an element (atomic LM)"     },
  { "cover",     MetaLMCoverItf,     "cover an element (read all words from it)"     },
  { "scoreFct",  MetaLMScoreFctItf,  "change the score function"                     },
  { "loadStopList", MetaLMLoadStopListItf,      "loads a list of stopwords (must be called after LM is loaded)"  },
  { "isStopWord", MetaLMIsStopWordItf,      "check if a word is a stopword"  },
  {  NULL,       NULL,                NULL } 
};

TypeInfo MetaLMInfo = {
  "MetaLM", 0, -1,    MetaLMMethod, 
  NULL,               NULL,
  MetaLMConfigureItf, MetaLMAccessItf, NULL,
  "Meta language model: flexible LM using sub-LMs.\n"
};


extern int MetaLM_Init()
{
  if (!MetaLMInitialized) {

    itfNewType (&MetaLMInfo);
    itfNewType (&MetaLMItemInfo);
    itfNewType (&MetaLMElemInfo);

    MetaLMInitialized = TRUE;
  }
  return TCL_OK;
}
