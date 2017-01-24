/* ========================================================================
    JANUS      Speech- to Speech Translation
               ------------------------------------------------------------
               Object: Score
               ------------------------------------------------------------

    Author  :  Thomas Kemp & Juergen Fritsch
    Module  :  score.c
    Date    :  $Id: score.c 3043 2009-12-05 00:51:51Z metze $
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
    Revision 3.14  2005/04/25 13:25:23  metze
    cbcClear works (max be slow, needed for MMIE)

    Revision 3.13  2005/03/04 09:14:41  metze
    scScoreTopAllBbi returns 1E30 instead of faulting without codebooks

    Revision 3.12  2004/09/10 15:38:10  metze
    Added some code for better parallelization

    Revision 3.11  2004/09/02 17:15:05  metze
    Beautification

    Revision 3.10  2003/08/14 11:20:17  fuegen
    Merged changes on branch jtk-01-01-15-fms (jaguar -> ibis-013)

    Revision 3.9.4.9  2003/02/12 13:58:50  metze
    Added warnings for missing codebooks

    Revision 3.9.4.8  2003/02/05 08:57:21  soltau
    Added cbcClear

    Revision 3.9.4.7  2003/01/28 14:11:20  soltau
    cb-cfg->weight scales now the complete acoustic score

    Revision 3.9.4.6  2002/06/26 11:57:56  fuegen
    changes for new directory structure support and code cleaning

    Revision 3.9.4.5  2002/06/04 17:17:45  metze
    Faster scores with INTEL_COMPILER

    Revision 3.9.4.4  2002/03/18 17:41:38  soltau
    Deactivated cbcAlloc information

    Revision 3.9.4.3  2001/10/16 11:53:49  soltau
    Introduced CBX for bbi

    Revision 3.9.4.2  2001/07/09 14:37:47  soltau
    scaleRV,scaleCV support

    Revision 3.9.4.1  2001/06/26 19:29:34  soltau
    beautified/fixed a couple of ugly hacker routines

    Revision 3.9  2001/01/15 09:49:59  janus
    Merged Christian's version with WINDOWS support (branch jtk-00-11-14-fuegen)

    Revision 3.8  2001/01/10 16:58:37  janus
    Merged Florian's version, supporting e.g. WAV-Files, Multivar and NGETS

    Revision 3.7.4.1  2001/01/12 15:17:00  janus
    necessary changes for running janus under WINDOWS

    Revision 3.5  2000/08/16 11:38:48  soltau
    check cbcCache dimensions

    Revision 3.4  1997/10/01 16:56:05  koll
    fixed bug in topN1 - radial

    Revision 3.3  1997/07/23 07:13:26  fritsch
    minor changes in BBI flag handling + extensions for BBI on radial covariance codebooks

    Revision 3.2  1997/06/17 17:41:43  westphal
    *** empty log message ***

 * Revision 1.26  96/11/15  10:02:46  fritsch
 * Fixed scScoreTopOneBbi
 * 
 * Revision 1.25  1996/11/14  09:32:05  fritsch
 * Removed bug in topN with BBI scoring, added pruning of exp() calls
 *
 * Revision 1.24  1996/10/08  09:15:44  fritsch
 * added scScoreTopNBbi() for top-N (N>1) scoring
 *
 * Revision 1.23  1996/09/27  08:33:27  fritsch
 * made scScoreTopAllBbi() scScoreTopOneBbi() work with codebook flag bbiOn.
 *
 * Revision 1.22  1996/09/26  17:20:45  fritsch
 * replaced scScoreTopBbi() with two new scoring routines,
 * one for top-all and a separate one for optimized top-1
 *
 * Revision 1.21  1996/09/24  15:22:09  fritsch
 * Added scoring in a dimensionality reduced feature space (in scScoreTopBbi)
 *
 * Revision 1.20  1996/07/08  14:53:02  kemp
 * Added a weighting between codebooks and distributions
 * (cbfact)
 *
 * Revision 1.19  1996/03/21  17:11:37  monika
 * removed little bug I inserted while speeding up radial covariances in
 * v 1.18
 *
 * Revision 1.18  96/03/21  11:08:49  11:08:49  monika (Monika Woszczyna)
 * faster for radial covariances
 * 
 * Revision 1.17  96/02/27  00:21:43  00:21:43  rogina (Ivica Rogina)
 * made it work with tied / typed covars
 * 
 * Revision 1.16  1996/01/16  21:15:05  rogina
 * made reference vectors use matrices
 *
 * Revision 1.15  95/11/25  20:26:40  20:26:40  finkem (Michael Finke)
 * removed -60 threshold to compare with old score function
 * 
 * Revision 1.14  1995/11/20  13:09:15  rogina
 * return TCL_ERROR --> return 0.0   in float  scScoreTopBbi()
 *
 * Revision 1.13  1995/11/15  04:08:37  finkem
 * Added BBI score computation
 *
 * Revision 1.12  1995/10/19  14:35:03  finkem
 * added top1 training stuff
 *
 * Revision 1.10  1995/10/06  18:16:01  rogina
 * damn severe bug fixed
 *
 * Revision 1.9  1995/09/28  19:02:15  finkem
 * mixVal update for training fixed
 *
 * Revision 1.8  1995/09/25  14:53:09  finkem
 * fixed scoring bug in exact topN
 *
 * Revision 1.7  1995/09/23  18:17:23  finkem
 * removed some allocation problems
 *
 * Revision 1.6  1995/09/08  19:16:50  rogina
 * *** empty log message ***
 *
 * Revision 1.5  1995/09/08  18:18:23  rogina
 * *** empty log message ***
 *
 * Revision 1.4  1995/09/06  07:39:10  kemp
 * *** empty log message ***
 *
 * Revision 1.3  1995/08/31  08:29:06  kemp
 * *** empty log message ***
 *
 * Revision 1.1  1995/08/29  13:15:05  kemp
 * Initial revision
 *
 
   ======================================================================== */

char scoreRCSVersion[]= 
           "@(#)1$Id: score.c 3043 2009-12-05 00:51:51Z metze $";

#include <math.h>
#include <string.h>
#include <limits.h>
#include "codebook.h"
#include "score.h"

#include "bbi.h"

float scGauss (Codebook *cb, int refX, float *sample);

/* ----------------------------------------------------------------------- */

static short cacheEmptyIndicator = (-1);

/* If this value is found in a bestIndex[][][] cache element, this means   */
/* that this element is not a valid cache entry. The purpose of not taking */
/* a fixed value like (-1) is, that we want to be able to fill the cache   */
/* as fast as possible with the memset system call which can fill only with*/
/* bytes, so we need a short value here that might be system dependent.    */
/* This value will be set by cbcFillEmptyIndicator.                        */

static short cacheToMemset = 255;

/* This is the byte value that is to be used by the memset system call     */
/* If it is zero, this indicates that memset cannot be used at all!        */
/* ----------------------------------------------------------------------- */


/* ========================================================================= */
/* scInitLookup() will initialize the lookup array for the exp() function.   */
/* The 'input parameter' is the global variable epsilon. It gives the max.   */
/* tolerable error of the computation of exp(x). Results are 'ScaleFact', a  */
/* negative number that performs the scaling between x and the index (index= */
/* ScaleFact * x), 'Lower_bound', the smallest x-value that must be evalu-   */
/* ated (for all smaller: exp(x) < epsilon), LookupMax (the highest possible */
/* index value, it is higher than the index value for LowerBound!), and the  */
/* filled float array Lookup[] itself.                                       */
/* Lower_boundIndex is set to the highest index value that must be evaluated */
/* (it's just the index value for LowerBound).                               */
/* ------------------------------------------------------------------------- */

#define MAX_CODEBOOKSIZE 4097


static int    expLookupMax =  0;
static float *expLookup    = (float *)NULL;
static float  expLowerBound;
static int    expLowerBoundIndex;
static float  expScaleFact;

int scInitLookup( float epsilon)
{
  int maxIndex;
  int i;

  expLowerBound      = log(epsilon);
  maxIndex           = 20.0 / epsilon + 1;
  expLookupMax       = maxIndex - 1;
  expScaleFact       = -(1.0 / epsilon);
  expLowerBoundIndex = expLowerBound * expScaleFact;

  if (! expLookup) expLookup = (float *)malloc((maxIndex+1)*sizeof(float));
 
  for (i=0; i < maxIndex; i++) expLookup[i] = exp(i / expScaleFact);
  return(0);
}
/* ========================================================================= */

/* ========================================================================
    Codebook Cache
   ======================================================================== */
/* ------------------------------------------------------------------------
    Create Codebook Cache Structure
   ------------------------------------------------------------------------ */

int cbcAlloc( CbCache* cbcP)
{
  if ( cbcP->cbN != cbcP->cbsP->list.itemN) {

    int cbN          = cbcP->cbsP->list.itemN;
    int distCacheN   = cbcP->distCacheN;
    int i;

    cbcDealloc( cbcP);

    cbcP->cbN        =  cbN;
    cbcP->distFrameX = (int     *)malloc(sizeof(int)      * distCacheN);
    cbcP->logdist    = (float ***)malloc(sizeof(float**)  * distCacheN);
    cbcP->bestIndex  = (short ***)malloc(sizeof(short **) * distCacheN);
    cbcP->usedRefN   = (short  **)malloc(sizeof(short *)  * distCacheN);
    cbcP->refN       = (int     *)malloc(sizeof(int)      * cbN);

    /* ------------------------------------ */
    /* fill the frame index array with (-1) */
    /* ------------------------------------ */

    for (i=0; i < distCacheN; i++) cbcP->distFrameX[i] = (-1);

    /* ------------------------- */
    /* fill the refN array first */
    /* ------------------------- */

    for (i=0, cbcP->totalRefN = 0; i < cbcP->cbN; i++) {
      cbcP->refN[i]    = cbcP->cbsP->list.itemA[i].refN;
      cbcP->totalRefN += cbcP->refN[i];
    }
    /* INFO("cbcAlloc: allocated score cache for %d gaussians\n",cbcP->totalRefN); */

    /* ------------------------------------------- */
    /* now allocate the usedRefN and logdist array */
    /* ------------------------------------------- */

    cbcP->usedRefN[0]  = (short  *)malloc(sizeof(short)  * cbN * distCacheN);
    cbcP->logdist[0]   = (float **)malloc(sizeof(float*) * cbN * distCacheN);
    cbcP->bestIndex[0] = (short **)malloc(sizeof(short*) * cbN * distCacheN);

    for ( i = 1; i < distCacheN; i++) {

      cbcP->usedRefN[i]   = cbcP->usedRefN[ i-1] + cbN;
      cbcP->logdist[i]    = cbcP->logdist[  i-1] + cbN;
      cbcP->bestIndex[i]  = cbcP->bestIndex[i-1] + cbN;
    }

    /* ----------------------------------------------------- */
    /* now allocate for each frame the distance cache arrays */
    /* ----------------------------------------------------- */

    for ( i = 0; i < distCacheN; i++){
      int t;

      /* ---------------------------------------------------------------------------- */
      /* The logdist array is allocated for refX+1 elements each to give space to the */
      /* 'scale' argument which has to be used in score computation. 'scale' will be  */
      /* the last element of the logdist array.                                       */
      /* ---------------------------------------------------------------------------- */

      if (! (cbcP->logdist[i][0]   = (float *)malloc( sizeof(float) *
                                     (cbcP->totalRefN + cbN))) ||
          ! (cbcP->bestIndex[i][0] = (short *)malloc( sizeof(short) *
                                     (cbcP->totalRefN)))) {
	FATAL("Out of memory allocating %d floats.\n", cbcP->totalRefN);
      }

      for ( t = 1; t < cbN; t++){

	cbcP->logdist[i][t]   = cbcP->logdist[  i][t-1] + cbcP->refN[t-1] + 1;
	cbcP->bestIndex[i][t] = cbcP->bestIndex[i][t-1] + cbcP->refN[t-1];
      }
    }

    /* ------------------------------ */
    /* all right, mark them as unused */
    /* ------------------------------ */

    for (i=0; i < distCacheN; i++) cbcMarkUnused( cbcP, i);
  }
  return TCL_OK;
}

int cbcInit( CbCache* cbcP, char* name, CodebookSet* cbsP, int distCacheN)
{
  static int warningIssued = 0;

  if (cbcFillEmptyIndicator() < 0 && ! warningIssued){
    WARN("quick cache clearing not possible on this machine.\n");
    warningIssued++;
  }
  assert( cbsP && distCacheN);

  cbcP->name        =  strdup(name);
  cbcP->useN        =  0;
  cbcP->cbsP        =  cbsP;
  cbcP->cbN         =  0;
  cbcP->distCacheN  =  distCacheN;

  cbcP->distFrameX  =  NULL;
  cbcP->logdist     =  NULL;
  cbcP->bestIndex   =  NULL;
  cbcP->usedRefN    =  NULL;
  cbcP->refN        =  NULL;
  cbcP->totalRefN   =  0;

  return cbcAlloc( cbcP);
}

CbCache* cbcCreate( char* name, CodebookSet* cbsP, int distCacheN)
{
  CbCache* cbcP = (CbCache*)malloc(sizeof(CbCache));

  if (! cbcP || cbcInit( cbcP, name, cbsP, distCacheN) != TCL_OK) {
    if ( cbcP) free( cbcP);
    ERROR("Failed to allocate CbCache '%s'.\n", name); 
    return NULL;
  }
  return cbcP;
}

/* ------------------------------------------------------------------------
    Codebook Cache Deinit/Free
   ------------------------------------------------------------------------ */

int cbcDealloc( CbCache* cbcP)
{
  assert( cbcP);

  if (cbcP->cbN == 0  || cbcP->distCacheN == 0) return TCL_OK;
  if (cbcP->totalRefN != 0) {
    int cbcPX;
    for ( cbcPX = 0; cbcPX < cbcP->distCacheN; cbcPX++){
      free(cbcP->logdist  [cbcPX][0]);
      free(cbcP->bestIndex[cbcPX][0]);
    }
    cbcP->totalRefN   =  0;
  }
  if (   cbcP->logdist) {
    if ( cbcP->logdist[0])   free( cbcP->logdist[0]);
    free(cbcP->logdist);
  }
  if (   cbcP->bestIndex) {
    if ( cbcP->bestIndex[0]) free( cbcP->bestIndex[0]);
    free(cbcP->bestIndex);
  }
  if (   cbcP->distFrameX)   free(cbcP->distFrameX);
  if (   cbcP->refN)         free(cbcP->refN);
  if (   cbcP->usedRefN) {
    if ( cbcP->usedRefN[0])  free( cbcP->usedRefN[0]);
    free(cbcP->usedRefN);
  }

  cbcP->cbN         =  0;
  cbcP->distFrameX  =  NULL;
  cbcP->logdist     =  NULL;
  cbcP->bestIndex   =  NULL;
  cbcP->usedRefN    =  NULL;
  cbcP->refN        =  NULL;
  cbcP->totalRefN   =  0;
  return TCL_OK;
}

int cbcDeinit(  CbCache* cbcP)
{
  assert( cbcP);
  return cbcDealloc( cbcP);
}

int cbcFree( CbCache* cbcP)
{
  if ( cbcDeinit(  cbcP) != TCL_OK) return TCL_ERROR;
  if ( cbcP) free( cbcP);
  return TCL_OK;
}

/* ------------------------------------------------------------------------
     Codebook Cache Clear
   ------------------------------------------------------------------------ */

int cbcClear (CbCache* cbcP, int cacheN)
{
  cbcDealloc (cbcP);
  cbcP->distCacheN = cacheN;
  cbcAlloc   (cbcP);

  /* This doesn't work: (Florian)
  int cacheN = cbcP->distCacheN;
  int i;

  for (i=0; i < cacheN; i++)
    cbcP->distFrameX[i] = (-1);

  if (cacheToMemset == 0) {
    int cacheX = 0;
    for (cacheX = 0; cacheX < cacheN; cacheX++) {
      for (i = 0; i < cbcP->cbN; i++)
	cbcP->usedRefN[cacheX][i] = cacheEmptyIndicator;
    }
  } else {
    memset (cbcP->usedRefN[0], cacheToMemset, sizeof(short) * cacheN * cbcP->cbN);
  }
  */

  return TCL_OK;
}

/* ------------------------------------------------------------------------
    cbcMarkUnused   marks one frame of the distance cache as unused
   ------------------------------------------------------------------------ */

int cbcMarkUnused( CbCache* cbcP, int cacheX)
{
  int i;

  assert( cbcP);

  if ( cbcP->cbN == 0 || cbcP->bestIndex == NULL ||
       cacheX    >= cbcP->distCacheN) return TCL_ERROR;

  if (cacheToMemset == 0){

    /* ---------------- */
    /* slow clearing... */
    /* ---------------- */
    for ( i = 0; i < cbcP->cbN; i++)
      cbcP->usedRefN[cacheX][i] = cacheEmptyIndicator;
  }
  else{
    memset( cbcP->usedRefN[cacheX],
	    cacheToMemset, sizeof(short) * cbcP->cbN);
  }
  return TCL_OK;
}

/* ------------------------------------------------------------------------
    cbcFillEmptyIndicator  will set the static variable 'cacheEmptyIndicator'
                           to a value that can be made by calling 'memset'. 
                           The static variable 'cacheToMemset' is set to the 
                           (char) argument needed by 'memset'.
   ------------------------------------------------------------------------ */

int cbcFillEmptyIndicator()
{
  int   i;
  short x;

  for (i=255; i>0; i--){
    memset(&x, (char)i, sizeof(short));
    if (x<0 || x > 32000) { 
      cacheEmptyIndicator = x; 
      cacheToMemset       = i;
      return 1;
    }
  }
  /* ------------------------------------------------- */
  /* no value found, indicate there is something amiss */
  /* ------------------------------------------------- */
  cacheToMemset = 0;
  return(-1);
}

/* ------------------------------------------------------------------------ */
/*    score computation function(s)                                         */
/* ------------------------------------------------------------------------ */

/* ---------------------------------------------------------------- */
/* scScoreTopAllBbi()  -  top-all scoring with optional BBI approx  */
/* ---------------------------------------------------------------- */
/* This function replaces the standard score computing function     */
/* scScoreTopAll(). If the additional pointer parameter 'bbi' is    */
/* pointing to NULL, scScoreTopBbi() is computing the same score    */
/* than scScoreTopAll(), using all Gaussians in the codebook.       */
/* However, if there is a BBI tree available, it will be used to    */
/* determine a restricted set of Gaussians which will then be       */
/* evaluated. Depending on the BBI trees used, this scoring         */
/* function may be several times faster than scScoreTopAll().       */
/* ---------------------------------------------------------------- */

float scScoreTopAllBbi (CodebookSet *cbsP, int cbX, float *dsVal,
		        int frameX, float *mixVal) {

  register double  score,minlogdist,dist;
  register int     dimX,refX,rbufX;
  register int     usedRefNCache,i;

  int              dimN,refN,cacheX;
  Codebook        *cb;
  float           *logdistCache;
  short           *minIndexCache;
  float           *pattern;
  BBITree         *bbi;

  float  scaleRV = cbsP->scaleRV;
  float  scaleCV = cbsP->scaleCV;
  float  offset  = cbsP->offset;

  int    cntExp  = 0;

#ifdef __INTEL_COMPILER
  __declspec(align(16)) float sa[256]; /* Array needed for SPEED! */
#endif

  /* ----------------------------------------- */
  /* alloc new cache, if codebook size changed */
  /* ----------------------------------------- */
  if (cbsP->cache->cbN != cbsP->list.itemN) cbcAlloc(cbsP->cache);

  /* ------------------------ */
  /* set some basic variables */
  /* ------------------------ */
  score          = 0.0;
  cb             = &(cbsP->list.itemA[cbX]);
  dimN           = cb->dimN;
  refN           = cb->refN;
  rbufX          = frameX % cbsP->cache->distCacheN;

  if ((cb->cfg->rdimN > 0) && (cb->cfg->rdimN < cb->dimN)) 
       dimN = cb->cfg->rdimN;

  if (cb->bbiX >= 0) bbi = cbsP->bbi.itemA + cb->bbiX;
               else  bbi = NULL;

  if (!cb->rv || !cb->cv) {
    ERROR ("scScoreTopAllBbi: codebook '%s' not loaded => bogus results.\n", cb->name);
    return 1E30;
  }

  /* -------------------------- */
  /* check, if cache is useable */
  /* -------------------------- */
  if (cbsP->cache->distFrameX[rbufX] != frameX) {
    if (cbcMarkUnused(cbsP->cache,rbufX) < 0)
      WARN("scMarkDistcacheUnused returning error!\n");
    cbsP->cache->distFrameX[rbufX] = frameX;
  }
  logdistCache  = cbsP->cache->logdist[rbufX][cbX];
  minIndexCache = cbsP->cache->bestIndex[rbufX][cbX];
  usedRefNCache = cbsP->cache->usedRefN[rbufX][cbX];
  cacheX= usedRefNCache;

  /*check if cache works */
  if (refN != cbsP->cache->refN[cbX]) {
    ERROR("wrong score cache size for %s. expected %d but got %d\n",\
	  cb->name,refN,cbsP->cache->refN[cbX]);
    return 0.0;
  }

  /* ----------------------------------------------- */
  /* set a pointer to the corresponding speech frame */
  /* ----------------------------------------------- */
  if ((pattern = fesFrame(cbsP->feat,cb->featX,frameX)) == NULL) {
    ERROR("Can't get frame %d of feature %d.\n",frameX,cb->featX);
    return 0.0;
  }

  /* ------------------------------------------------------- */
  /* If the Gaussians of the actual codebook are not in the  */
  /* cache, we will have to compute them now                 */
  /* ------------------------------------------------------- */
  if (usedRefNCache <= 0) {
    if ((bbi != NULL) && (cb->cfg->bbiOn)) {

      /* ----------------------------------------------- */
      /* There is a BBI tree available to find a subset  */
      /* ----------------------------------------------- */
      int         pos,depth,depthX,adr,bbiN;
      BBINode     *actNode;
      BBILeaf     *bucket;
      CBX         *bbilist;

      /* walk down the tree */
      pos = 1; 
      depth = bbi->depthN;
      for (depthX=0; depthX<depth; depthX++) {
        actNode = (bbi->node)+pos;
        pos = (pos<<1) + ((pattern[(int) actNode->k]-offset) > actNode->h*scaleRV);            
      }
      adr      = pos-(1<<depth);            /* adress of the leaf/bucket  */
      bucket   = &bbi->leaf[adr][cb->bbiY]; /* pointer to the leaf/bucket */  
      bbiN     = bucket->cbvN;              /* number of Gaussians in the leaf */
      bbilist  = bucket->cbX;               /* pointer to the leaf's BBI list */
      for (refX=0; refX<bbiN; refX++) 
        minIndexCache[refX] = bbilist[refX];

      cbsP->cache->usedRefN[rbufX][cbX] = bbiN;   /* number of Gaussians */

    } else {
  
      /* --------------------------------------------------- */
      /* There is NO BBI tree available => use all Gaussians */
      /* --------------------------------------------------- */

      for (refX=0; refX<refN; refX++) minIndexCache[refX] = refX;

      cbsP->cache->usedRefN[rbufX][cbX] = refN;   /* select all Gaussians */
    }
    
    /* ----------------------------------------------------------------- */
    /* Compute all the Mahalanobis distances for the subset of Gaussians */
    /* ----------------------------------------------------------------- */

    usedRefNCache = cbsP->cache->usedRefN[rbufX][cbX];
    minlogdist = 1E20;

    switch (cb->type) {

      case COV_NO:

        for (i=0; i<usedRefNCache; i++) {
          int      refX     = minIndexCache[i];
          float    *pt      = pattern;
	  float    *rv      = cb->rv->matPA[refX];
          double   distSum  = cb->pi*scaleRV*scaleRV;
          for (dimX=0; dimX<dimN; dimX++) {
            register double diff0 = *rv++ - *pt++;
            distSum += diff0*diff0;
          }
          logdistCache[i] = 0.5*distSum/(scaleRV*scaleRV);
          if (logdistCache[i] < minlogdist)
            minlogdist = logdistCache[i];
        }
        break;

      case COV_RADIAL:

        for (i=0; i<usedRefNCache; i++) {
          int     refX     =  minIndexCache[i];
          float   *pt      =  pattern;
	  float   *rv      =  cb->rv->matPA[refX];
          float   cv       =  cb->cv[refX]->m.r;
          float  distSum  =  0.0;
          for (dimX=0; dimX<dimN; dimX++) {
            float diff0 = *rv++ - *pt++;
            distSum += diff0*diff0;
          }
          distSum = (cb->pi + cb->cv[refX]->det) + cv * distSum/(scaleRV*scaleRV*scaleCV);            
          logdistCache[i] = 0.5*distSum;
          if (logdistCache[i] < minlogdist)
            minlogdist = logdistCache[i];
        }
        break;

      case COV_DIAGONAL:

#ifdef __INTEL_COMPILER
        for (i = 0; i < usedRefNCache; i++) {
          int             refX =  minIndexCache[i];
	  float        distSum = (cb->pi + cb->cv[refX]->det)*scaleRV*scaleRV*scaleCV; 
	  float   *restrict pt = pattern;
	  float   *restrict rv = cb->rv->matPA[refX];
	  float   *restrict cv = cb->cv[refX]->m.d;

	  /*
	  __assume_aligned(rv,16);
	  __assume_aligned(cv,16);
          __assume_aligned(pt,16);
	  #pragma vector aligned
	  */

	  for (dimX = 0; dimX < dimN; dimX++)
	    sa[dimX] = rv[dimX]-pt[dimX], distSum += sa[dimX]*sa[dimX]*cv[dimX];

          logdistCache[i] = 0.5*distSum/(scaleRV*scaleRV*scaleCV);
        }
	for (i = 0; i < usedRefNCache; i++)
          if (logdistCache[i] < minlogdist)
            minlogdist = logdistCache[i];
#else
#ifdef SSE_OPT
        for (i = 0; i < usedRefNCache; i++) {
          int     refX     =  minIndexCache[i];
	  register float distSum = (cb->pi + cb->cv[refX]->det)*scaleRV*scaleRV*scaleCV;
	  __m128 *m128pt = (__m128*) pattern;
	  __m128 *m128rv = (__m128*) cb->rv->matPA[refX];
	  __m128 *m128cv = (__m128*) cb->cv[refX]->m.d;
	  __m128  mo     = _mm_set_ps1(0.0);
	  __m128  m1, m2;
	  int     dimN_8 = dimN >> 3;
	  float  *pt, *rv, *cv;
	  float   m_result[4] __attribute__ ((aligned (16)));

	  /* PROCESS 8 ELEMENTS LOOP */
	  for (dimX=0; dimX<dimN_8; dimX++) {
	    m1 = _mm_sub_ps(*m128pt++, *m128rv++);
	    m1 = _mm_mul_ps(m1, m1);
	    m1 = _mm_mul_ps(m1, *m128cv++);
	    mo = _mm_add_ps(mo, m1);
	    m2 = _mm_sub_ps(*m128pt++, *m128rv++);
	    m2 = _mm_mul_ps(m2, m2);
	    m2 = _mm_mul_ps(m2, *m128cv++);
	    mo = _mm_add_ps(mo, m2);
	  }
	  _mm_store_ps (m_result, mo);
	  distSum += m_result[0] + m_result[1] + m_result[2] + m_result[3];

	  /* PROCESS SINGLE 4 ELEMENTS */
	  if ((dimN&4)!=0) {
	    m1 = _mm_sub_ps(*m128pt++, *m128rv++);
	    m1 = _mm_mul_ps(m1, m1);
	    m1 = _mm_mul_ps(m1, *m128cv++);
	    
	    _mm_store_ps (m_result, m1);
	    distSum += m_result[0] + m_result[1] + m_result[2] + m_result[3];
	  }
	  
	  /* PROCESS REMAINING ELEMENTS */
	  pt = (float*)m128pt;
	  rv = (float*)m128rv;
	  cv = (float*)m128cv;
	  if ((dimN&2)!=0) {
	    register float diff0 = *rv++ - *pt++;
	    register float diff1 = *rv++ - *pt++;
	    distSum += diff0*diff0*(*cv++);
	    distSum += diff1*diff1*(*cv++);
	  }
	  if ((dimN&1)!=0) {
	    register float diff0 = *rv++ - *pt++;
	    distSum += diff0*diff0*(*cv++);
	  }

	  logdistCache[i] = 0.5*distSum/(scaleRV*scaleRV*scaleCV);
          if (logdistCache[i] < minlogdist)
            minlogdist = logdistCache[i];
        }
#else
        for (i = 0; i < usedRefNCache; i++) {
          int     refX     =  minIndexCache[i];
	  double  distSum  = (cb->pi + cb->cv[refX]->det)*scaleRV*scaleRV*scaleCV;
	  float   *pt      =  pattern;
	  float   *rv      =  cb->rv->matPA[refX];
          float   *cv      =  cb->cv[refX]->m.d;
          for (dimX = 0; dimX < dimN; dimX++) {
	    register double diff0 = *rv++ - *pt++;
	    distSum += diff0*diff0*(*cv++);
          }

          logdistCache[i] = 0.5*distSum/(scaleRV*scaleRV*scaleCV);
          if (logdistCache[i] < minlogdist)
            minlogdist = logdistCache[i];
        }
#endif
#endif
        break;

      default :  /* full covariances - slow default case */

        for (i=0; i<usedRefNCache; i++) {
          int refX        =  minIndexCache[i];
          logdistCache[i] = scGauss(cb, refX, pattern);
          if (logdistCache[i] < minlogdist)
            minlogdist = logdistCache[i];
        }
        break;
    }

    logdistCache[refN] = minlogdist;

  } else 

    /* ----------------------------------------- */
    /* Gaussians are cached, so we only need the */
    /* value of the minimum Mahalanobis distance */
    /* which we have already computed previously */
    /* ----------------------------------------- */
    minlogdist = logdistCache[refN]; 

  /* ---------------------------------------------------- */
  /* if the codebook weight is far from 1.0, minlogdist   */
  /* might be unable to help us in order to get numerical */
  /* stability if not adapted also:                       */
  /* ---------------------------------------------------- */
  /* minlogdist *= cb->cfg->weight; */
 
  /* ---------------------------------------------- */
  /* Add up the exponentiated Mahalanobis distances */
  /* in the cache, and logarithmize the sum to get  */
  /* the score. In case of one single vector, we    */
  /* do not need to exponentiate                    */
  /* ---------------------------------------------- */
  if (usedRefNCache == 1) {

    if (mixVal) {
      for (i=0; i<refN; i++) mixVal[i] = 0.0;
      mixVal[minIndexCache[0]] = 1.0;
    }
    return (cb->cfg->weight * (logdistCache[0] + dsVal[minIndexCache[0]]));

  } else {

    if (!mixVal) {

      if (bbi && (cb->cfg->bbiOn)) {
	/* BBI code */
	for (i=0; i<usedRefNCache; i++) {
	  refX   = minIndexCache[i];
	  dist   = minlogdist - logdistCache[i];
	  if (dist > cb->cfg->expT) 
	    score += exp(dist - dsVal[refX]);
	}

      } else {
	/* Code can be executed in parallel because i <-> refX */
	for (i = 0; i < usedRefNCache; i++)
	  if (- logdistCache[i] > cb->cfg->expT - minlogdist)
	    score += exp (minlogdist - logdistCache[i] - dsVal[i]);
      }

    } else {

      for (i=0; i<refN; i++) mixVal[i] = 0.0;
      for (i=0; i<usedRefNCache; i++) {
	refX   =  minIndexCache[i];
	dist   =  minlogdist - (logdistCache[i]);
	if (dist > cb->cfg->expT) {
	  cntExp++;
	  score += (mixVal[refX] = exp(dist - dsVal[refX]));
	}
      }
    }

    return (float) (cb->cfg->weight * (minlogdist-log(score)));
  }
}



/* ---------------------------------------------------------------- */
/* scScoreTopOneBbi()  -  top-one scoring with optional BBI approx  */
/* ---------------------------------------------------------------- */
/* Alternative to scScoreTopAllBbi; only the top-1 Gaussian is used */
/* to compute the actual score. If BBI trees are used, the top-1    */
/* Gaussians will be taken from reduced BBI codevector lists        */
/* ---------------------------------------------------------------- */
float scScoreTopOneBbi (CodebookSet *cbsP, int cbX, float *dsVal,
		        int frameX, float *mixVal) {

  register float         score,minDistSum;
  Codebook                *cb;
  register int            dimN,dimX,refN,refX,rbufX;
  register int            usedRefNCache,i,minDistIdx;
  float                   *logdistCache;
  short                   *minIndexCache;
  float                   *pattern;

  BBITree                 *bbi;
  
  /* ----------------------------------------- */
  /* alloc new cache, if codebook size changed */
  /* ----------------------------------------- */
  if (cbsP->cache->cbN != cbsP->list.itemN) cbcAlloc(cbsP->cache);

  /* ------------------------ */
  /* set some basic variables */
  /* ------------------------ */
  score          = 0.0;
  cb             = &(cbsP->list.itemA[cbX]);
  dimN           = cb->dimN;
  refN           = cb->refN;
  rbufX          = frameX % cbsP->cache->distCacheN;

  if ((cb->cfg->rdimN > 0) && (cb->cfg->rdimN < cb->dimN)) 
       dimN = cb->cfg->rdimN;

  if (cb->bbiX >= 0) bbi = cbsP->bbi.itemA + cb->bbiX;
               else  bbi = NULL;

  if (!cb->rv || !cb->cv) {
    ERROR ("scScoreTopOneBbi: codebook '%s' not loaded => bogus results.\n", cb->name);
    abort ();
  }

  /* -------------------------- */
  /* check, if cache is useable */
  /* -------------------------- */
  if (cbsP->cache->distFrameX[rbufX] != frameX) {
    if (cbcMarkUnused(cbsP->cache,rbufX) < 0)
      WARN("scMarkDistcacheUnused returning error!\n");
    cbsP->cache->distFrameX[rbufX] = frameX;
  }
  logdistCache  = cbsP->cache->logdist[rbufX][cbX];
  minIndexCache = cbsP->cache->bestIndex[rbufX][cbX];
  usedRefNCache = cbsP->cache->usedRefN[rbufX][cbX];

  /* ----------------------------------------------- */
  /* set a pointer to the corresponding speech frame */
  /* ----------------------------------------------- */
  if ((pattern = fesFrame(cbsP->feat,cb->featX,frameX)) == NULL) {
    ERROR("Can't get frame %d of feature %d.\n",frameX,cb->featX);
    return 0.0;
  }

  /* ------------------------------------------------------- */
  /* If the Gaussians of the actual codebook are not in the  */
  /* cache, we will have to compute them now                 */
  /* ------------------------------------------------------- */
  if (usedRefNCache <= 0) {
    if ((bbi != NULL) && (cb->cfg->bbiOn)) {

      /* ----------------------------------------------- */
      /* There is a BBI tree available to find a subset  */
      /* ----------------------------------------------- */
      int         pos,depth,depthX,adr,bbiN;
      BBINode     *actNode;
      BBILeaf     *bucket;
      CBX         *bbilist;

      /* walk down the tree */
      pos = 1; 
      depth = bbi->depthN;
      for (depthX=0; depthX<depth; depthX++) {
        actNode = (bbi->node)+pos;
        pos = (pos<<1) + (pattern[(int) actNode->k] > actNode->h);            
      }
      adr      = pos-(1<<depth);            /* adress of the leaf/bucket  */
      bucket   = &bbi->leaf[adr][cb->bbiY]; /* pointer to the leaf/bucket */  
      bbiN     = bucket->cbvN;              /* number of Gaussians in the leaf */
      bbilist  = bucket->cbX;               /* pointer to the leaf's BBI list */
      for (refX=0; refX<bbiN; refX++) 
        minIndexCache[refX] = bbilist[refX];

      cbsP->cache->usedRefN[rbufX][cbX] = bbiN;   /* number of Gaussians */

    } else {
  
      /* --------------------------------------------------- */
      /* There is NO BBI tree available => use all Gaussians */
      /* --------------------------------------------------- */

      for (refX=0; refX<refN; refX++) minIndexCache[refX] = refX;

      cbsP->cache->usedRefN[rbufX][cbX] = refN;   /* select all Gaussians */
    }
    
    /* ----------------------------------------------------------------- */
    /* Compute all the Mahalanobis distances for the subset of Gaussians */
    /* ----------------------------------------------------------------- */

    usedRefNCache = cbsP->cache->usedRefN[rbufX][cbX];
    minDistSum = 1E20;
    minDistIdx = 0;
    
    switch (cb->type) {

      case COV_NO:

        for (i=0; i<usedRefNCache; i++) {
          int      refX     = minIndexCache[i];
          float    *pt      = pattern;
	  float    *rv      = cb->rv->matPA[refX];
          float   distSum  = cb->pi;
          for (dimX=0; dimX<dimN; dimX++) {
            register float diff0 = *rv++ - *pt++;
            distSum += diff0*diff0;
	    if (distSum > minDistSum) break;
          }
          if (distSum < minDistSum) {
	    minDistSum = distSum;
            minDistIdx = refX;
   	    logdistCache[refX] = 0.5*distSum;
	  }
        }
        break;

      case COV_RADIAL:

        for (i=0; i<usedRefNCache; i++) {
          int     refX     =  minIndexCache[i];
          float   *pt      =  pattern;
	  float   *rv      =  cb->rv->matPA[refX];
          float   cv       =  cb->cv[refX]->m.r;

          register float   distSum  =  (cb->pi + cb->cv[refX]->det)/cv;
	  register float  breakSum  =  minDistSum/cv;

	  for (dimX=0; dimX<dimN; dimX++) {
	    register float diff0;

	    if (distSum > breakSum) break;
	    diff0 = *rv++ - *pt++;
	    distSum += diff0*diff0; 
	  }


	  if (distSum < breakSum) {
	    minDistIdx = refX;
            distSum    = cv * distSum;
	    minDistSum = distSum;
            logdistCache[refX] = 0.5*distSum;
          }
        }
        break;

      case COV_DIAGONAL:

        for ( i = 0; i < usedRefNCache; i++) {
          int     refX     =  minIndexCache[i];
	  float   *pt      =  pattern;
	  float   *rv      =  cb->rv->matPA[refX];
          float   *cv      =  cb->cv[refX]->m.d;
	  float   distSum  = (cb->pi + cb->cv[refX]->det);
          int     dimN_4 = dimN/4;
          for ( dimX = 0; dimX < dimN_4; dimX++) {
	    register float diff0;
	    if (distSum > minDistSum) break;
            diff0 = *rv++ - *pt++;
	    distSum += diff0*diff0*(*cv++);
            diff0 = *rv++ - *pt++;
	    distSum += diff0*diff0*(*cv++);
            diff0 = *rv++ - *pt++;
	    distSum += diff0*diff0*(*cv++);
            diff0 = *rv++ - *pt++;
	    distSum += diff0*diff0*(*cv++);
          }

          if (dimX == dimN_4) {
            for (dimX = 4*dimN_4; dimX < dimN; dimX++) {
	      register float diff0 = *rv++ - *pt++;
	      distSum += diff0*diff0*(*cv++);
            }
	  }

          /* don't forget to do rest */
	  if (distSum < minDistSum) {
	    minDistSum = distSum;
	    minDistIdx = refX;
            logdistCache[refX] = 0.5*distSum;
	  }
        }
        break;

      default :  /* full covariances - slow default case */

        for (i=0; i<usedRefNCache; i++) {
          int refX        =  minIndexCache[i];
          logdistCache[refX] = scGauss(cb, refX, pattern);
	  if (logdistCache[refX] < minDistSum) {
	    minDistSum = logdistCache[refX];
	    minDistIdx = refX;
	  }
        }
        break;
    }

    minIndexCache[0] = minDistIdx;
    logdistCache[0]  = logdistCache[minDistIdx];
    
  }

  if (mixVal) {
    for (i=0; i<refN; i++) mixVal[i] = 0.0;
    mixVal[minIndexCache[0]] = 1.0;
  }

  return (cb->cfg->weight * (logdistCache[0] + dsVal[minIndexCache[0]]));
}


/* ------------------------------------------------------------------ */
/* scScoreTopNBbi()  -  top-N scoring with optional BBI approx        */
/* ------------------------------------------------------------------ */
/* Alternative to scScoreTopAllBbi; only the top-N Gaussians are used */
/* to compute the actual score. If BBI trees are used, the top-N      */
/* Gaussians will be taken from reduced BBI codevector lists          */
/* ------------------------------------------------------------------ */
float scScoreTopNBbi (CodebookSet *cbsP, int cbX, float *dsVal,
	              int frameX, float *mixVal) {

  static short            *topli = NULL;  /* top-N list of cbv indices */
  static double           *topls = NULL;  /* top-N list of cbv scores */
  static int              toplN  = 0;     /* length of top-N list */
  register double         score,dist,minDistSum,minlogdist;
  Codebook                *cb;
  register int            dimN,dimX,refN,refX,rbufX;
  register int            i,minDistIdx,topN,k;
  short                   *usedRefNCache;
  float                   *logdistCache;
  short                   *minIndexCache;
  float                   *pattern;

  BBITree                 *bbi;

  /* ----------------------------------------- */
  /* alloc new cache, if codebook size changed */
  /* ----------------------------------------- */
  if (cbsP->cache->cbN != cbsP->list.itemN) cbcAlloc(cbsP->cache);

  /* ------------------------ */
  /* set some basic variables */
  /* ------------------------ */
  score          = 0.0;
  cb             = &(cbsP->list.itemA[cbX]);
  dimN           = cb->dimN;
  refN           = cb->refN;
  rbufX          = frameX % cbsP->cache->distCacheN;

  if ((cb->cfg->rdimN > 0) && (cb->cfg->rdimN < cb->dimN)) 
       dimN = cb->cfg->rdimN;

  if (cb->bbiX >= 0) bbi = cbsP->bbi.itemA + cb->bbiX;
               else  bbi = NULL;

  if (!cb->rv || !cb->cv) {
    ERROR ("scScoreTopNBbi: codebook '%s' not loaded => bogus results.\n", cb->name);
    abort ();
  }

  /* -------------------------- */
  /* check, if cache is useable */
  /* -------------------------- */
  if (cbsP->cache->distFrameX[rbufX] != frameX) {
    if (cbcMarkUnused(cbsP->cache,rbufX) < 0)
      WARN("scMarkDistcacheUnused returning error!\n");
    cbsP->cache->distFrameX[rbufX] = frameX;
  }
  logdistCache  = cbsP->cache->logdist[rbufX][cbX];
  minIndexCache = cbsP->cache->bestIndex[rbufX][cbX];
  usedRefNCache = &cbsP->cache->usedRefN[rbufX][cbX];

  /* --------------------------------------------- */
  /*  alloc new topli,topls lists if topN changed  */
  /* --------------------------------------------- */
  if (cb->cfg->topN < 2) return 0.0;
  if (cb->cfg->topN != toplN) {
    if (toplN != 0) {
      free(topli);
      free(topls);
    }
    topli  = (short *) malloc (sizeof(double) * cb->cfg->topN);
    topls  = (double *) malloc (sizeof(double) * cb->cfg->topN);
    toplN = cb->cfg->topN;
  }
  
  /* ----------------------------------------------- */
  /* set a pointer to the corresponding speech frame */
  /* ----------------------------------------------- */
  if ((pattern = fesFrame(cbsP->feat,cb->featX,frameX)) == NULL) {
    ERROR("Can't get frame %d of feature %d.\n",frameX,cb->featX);
    return 0.0;
  }

  /* ------------------------------------------------------- */
  /* If the Gaussians of the actual codebook are not in the  */
  /* cache, we will have to compute them now                 */
  /* ------------------------------------------------------- */
  if (*usedRefNCache <= 0) {
    if ((bbi != NULL) && (cb->cfg->bbiOn)) {

      /* ----------------------------------------------- */
      /* There is a BBI tree available to find a subset  */
      /* ----------------------------------------------- */
      int         pos,depth,depthX,adr,bbiN;
      BBINode     *actNode;
      BBILeaf     *bucket;
      CBX         *bbilist;

      /* walk down the tree */
      pos = 1; 
      depth = bbi->depthN;
      for (depthX=0; depthX<depth; depthX++) {
        actNode = (bbi->node)+pos;
        pos = (pos<<1) + (pattern[(int) actNode->k] > actNode->h);            
      }
      adr      = pos-(1<<depth);            /* adress of the leaf/bucket  */
      bucket   = &bbi->leaf[adr][cb->bbiY]; /* pointer to the leaf/bucket */  
      bbiN     = bucket->cbvN;              /* number of Gaussians in the leaf */
      bbilist  = bucket->cbX;               /* pointer to the leaf's BBI list */
      for (refX=0; refX<bbiN; refX++) 
        minIndexCache[refX] = bbilist[refX];

      cbsP->cache->usedRefN[rbufX][cbX] = bbiN;   /* number of Gaussians */

    } else {

      /* --------------------------------------------------- */
      /* There is NO BBI tree available => use all Gaussians */
      /* --------------------------------------------------- */

      for (refX=0; refX<refN; refX++) minIndexCache[refX] = refX;
      cbsP->cache->usedRefN[rbufX][cbX] = refN;   /* select all Gaussians */

    }

    /* ------------------------------------------------------------------ */
    /*  Compute Mahalanobis distances for the top-N vectors in the subset */
    /* ------------------------------------------------------------------ */    
    topN = 0;
    minDistSum = 1E20;
    
    switch (cb->type) {

      case COV_NO:

        for (i=0; i<*usedRefNCache; i++) {
          int      refX     = minIndexCache[i];
          float    *pt      = pattern;
  	  float    *rv      = cb->rv->matPA[refX];
          double   distSum  = cb->pi;
          for (dimX=0; dimX<dimN; dimX++) {
            register double diff0 = *rv++ - *pt++;
            distSum += diff0*diff0;
            if (distSum > minDistSum) break;
          }
          if ((topN < toplN) || (distSum < minDistSum)) {
	    minDistIdx = refX;
            minDistSum = distSum;
            /* insert into list */
            if (topN < toplN) {
	      topli[topN] = refX;
	      topls[topN] = minDistSum;
	      topN++;
	    } else {
              topli[topN-1] = refX;
	      topls[topN-1] = minDistSum;
	    }
	    /* sort list */
            for (k=topN-2; k>=0; k--) {
              if (topls[k] < minDistSum)
		break;
	      else {
	        topli[k+1] = topli[k];
	        topls[k+1] = topls[k];
              }
	    }
            topli[k+1] = minDistIdx;
            topls[k+1] = minDistSum;
            minDistSum = topls[topN-1];
	  }
	}
	for (i=0; i<topN; i++) {
	  logdistCache[i]  = 0.5*topls[i];
	  minIndexCache[i] = topli[i];
	}
	*usedRefNCache = topN;
        break;

      case COV_RADIAL:

        for (i=0; i<*usedRefNCache; i++) {
          int     refX     =  minIndexCache[i];
          float   *pt      =  pattern;
	  float   *rv      =  cb->rv->matPA[refX];
          float   cv       = cb->cv[refX]->m.r;
          double  distSum  =  cb->pi + cb->cv[refX]->det;
          for (dimX=0; dimX<dimN; dimX++) {
            register double diff0 = *rv++ - *pt++;
            distSum += cv*diff0*diff0;
            if (distSum > minDistSum) break;
          }
          if ((topN < toplN) || (distSum < minDistSum)) {
	    minDistIdx = refX;
            minDistSum = distSum;
            /* insert into list */
            if (topN < toplN) {
	      topli[topN] = refX;
	      topls[topN] = minDistSum;
	      topN++;
	    } else {
              topli[topN-1] = refX;
	      topls[topN-1] = minDistSum;
	    }
	    /* sort list */
            for (k=topN-2; k>=0; k--) {
              if (topls[k] < minDistSum)
		break;
	      else {
	        topli[k+1] = topli[k];
	        topls[k+1] = topls[k];
              }
	    }
            topli[k+1] = minDistIdx;
            topls[k+1] = minDistSum;
            minDistSum = topls[topN-1];
	  }
        }
	for (i=0; i<topN; i++) {
          int     refX     = topli[i];

	  logdistCache[i]  = 0.5*topls[i];
	  minIndexCache[i] = refX;
	}
	*usedRefNCache = topN;
        break;

      case COV_DIAGONAL:

        for ( i = 0; i < *usedRefNCache; i++) {
          int     refX     =  minIndexCache[i];
          float   *pt      =  pattern;
          float   *rv      =  cb->rv->matPA[refX];
          float   *cv      =  cb->cv[refX]->m.d;
          double  distSum  =  cb->pi + cb->cv[refX]->det;
	      
          for ( dimX = 0; dimX < dimN; dimX++) {
            register double diff0 = *rv++ - *pt++;
            distSum += diff0*diff0*(*cv++);
            if (distSum > minDistSum) break;
          }
          if ((topN < toplN) || (distSum < minDistSum)) {
	    minDistIdx = refX;
            minDistSum = distSum;
            /* insert into list */
            if (topN < toplN) {
	      topli[topN] = refX;
	      topls[topN] = minDistSum;
	      topN++;
	    } else {
              topli[topN-1] = refX;
	      topls[topN-1] = minDistSum;
	    }
	    /* sort list */
            for (k=topN-2; k>=0; k--) {
              if (topls[k] < minDistSum)
		break;
	      else {
	        topli[k+1] = topli[k];
	        topls[k+1] = topls[k];
              }
	    }
            topli[k+1] = minDistIdx;
            topls[k+1] = minDistSum;
            minDistSum = topls[topN-1];
	  }
        }
	for (i=0; i<topN; i++) {
	  logdistCache[i]  = 0.5*topls[i];
	  minIndexCache[i] = topli[i];
	}
	*usedRefNCache = topN;
	exit(-1);
        break;

      default :  /* full covariances - slow default case */
  
        for (i=0; i<*usedRefNCache; i++) {
          int    refX    = minIndexCache[i];
          double distSum = scGauss(cb, refX, pattern);
          if ((topN < toplN) || (distSum < minDistSum)) {
	    minDistIdx = refX;
            minDistSum = distSum;
            /* insert into list */
            if (topN < toplN) {
	      topli[topN] = refX;
	      topls[topN] = minDistSum;
	      topN++;
	    } else {
              topli[topN-1] = refX;
	      topls[topN-1] = minDistSum;
	    }
	    /* sort list */
            for (k=topN-2; k>=0; k--) {
              if (topls[k] < minDistSum)
		break;
	      else {
	        topli[k+1] = topli[k];
	        topls[k+1] = topls[k];
              }
	    }
            topli[k+1] = minDistIdx;
            topls[k+1] = minDistSum;
            minDistSum = topls[topN-1];
	  }
        }
	for (i=0; i<topN; i++) {
	  logdistCache[i] = topls[i];
	  minIndexCache[i]  = topli[i];
	}
	*usedRefNCache = topN;
        break;
      }

  }

  minlogdist = logdistCache[0];

  /* ---------------------------------------------- */
  /* Add up the exponentiated Mahalanobis distances */
  /* in the cache, and logarithmize the sum to get  */
  /* the score. In case of one single vector, we    */
  /* do not need to exponentiate                    */
  /* ---------------------------------------------- */
  if (*usedRefNCache == 1) {

    if (mixVal) {
      for (i=0; i<refN; i++) mixVal[i] = 0.0;
      mixVal[minIndexCache[0]] = 1.0;
    }
    return (cb->cfg->weight * (logdistCache[0] + dsVal[minIndexCache[0]]));

  } else {

    if (!mixVal) {
      for (i=0; i<*usedRefNCache; i++) {
        refX   = minIndexCache[i];
        dist   = minlogdist - logdistCache[i];
        if (dist > cb->cfg->expT) score += exp(dist - dsVal[refX]);
      }
    } else {
      for (i=0; i<refN; i++) mixVal[i] = 0.0;
      for (i=0; i<*usedRefNCache; i++) {
        refX   =  minIndexCache[i];
        dist   =  minlogdist - logdistCache[i]; 
        if (dist > cb->cfg->expT) 
            score += (mixVal[refX] = exp(dist -dsVal[refX]));
      }
    }
    return (float) (cb->cfg->weight *(minlogdist-log(score)));
  }  
}



/* -------------------------------------------------------------- */
/* scGauss computes the Mahalanobis distance between the codebook */
/* and the sample. It takes into account all vectors of the code- */
/* book and handles all types of covariance modelling.            */
/* -------------------------------------------------------------- */
#define MAX_DIM             100
#define square(X) ((X)*(X))

float scGauss (Codebook *cb, int refX, float *sample)
{
  register int dimX, dimY;
  register double distSum = 0.0;

  float    d[MAX_DIM];

  switch (cb->type)
  {
	 /* ---------------------------------------------------------------- */
         case COV_NO:		/* unity covariance matrix Gaussian value:   */
				/* ----------------------------------------- */

	      for (dimX=0; dimX<cb->dimN; dimX++)
		  distSum += square(cb->rv->matPA[refX][dimX]-sample[dimX]);

	      distSum += cb->pi;
	      distSum *= 0.5;

	      break;

	 /* ---------------------------------------------------------------- */
	 case COV_RADIAL:	/* radial covariance matrix Gaussian value:  */
				/* ----------------------------------------- */

	      for (dimX=0; dimX<cb->dimN; dimX++)
		  distSum += square(cb->rv->matPA[refX][dimX]-sample[dimX]);
                  
              distSum *= cb->cv[refX]->m.r;

	      distSum += (cb->pi + cb->cv[refX]->det);
	      distSum *= 0.5;

	      break;

	 /* ---------------------------------------------------------------- */
	 case COV_DIAGONAL:	/* diagonal covariance matrix Gaussian value */
				/* ----------------------------------------- */

	      for (dimX=0; dimX<cb->dimN; dimX++)
       		  distSum += square(cb->rv->matPA[refX][dimX]-sample[dimX])
			   * cb->cv[refX]->m.d[dimX];

	      distSum += (cb->pi + cb->cv[refX]->det);
	      distSum *= 0.5;

	      break;

	 /* ---------------------------------------------------------------- */
	 case COV_FULL:		/* full covariance matrix Gaussian value:    */
				/* ----------------------------------------- */

	      for (dimX=0; dimX<cb->dimN; dimX++) 
		  d[dimX] = (cb->rv->matPA[refX][dimX]-sample[dimX]);

	      for (dimX=0; dimX<cb->dimN; dimX++) 
		  for (dimY=0; dimY<cb->dimN; dimY++) 
		      distSum += d[dimX]*d[dimY]*cb->cv[refX]->m.f[dimX][dimY];

	      distSum += (cb->pi + cb->cv[refX]->det);
	      distSum *= 0.5;

	      break;

         case COV_ERR:
	      break;

         default:
              break;
  }
  return (float)distSum;
}

/* ========================================================================
    Type Declaration
   ======================================================================== */

static int scoreInitialized = 0;

int Score_Init()
{
  if ( scoreInitialized) return TCL_OK;

  /* scInitLookup( 1e-4); */

  scoreInitialized = 1;
  return TCL_OK;
}
