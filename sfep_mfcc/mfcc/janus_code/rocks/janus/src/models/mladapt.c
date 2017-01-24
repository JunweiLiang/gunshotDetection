/* ========================================================================
    JanusRTk   Janus Recognition Toolkit
               ------------------------------------------------------------
               Object: Maximum Likelihood Adaptation
               ------------------------------------------------------------

    Author  :  Michael Finke
    Module  :  mladapt.c
    Date    :  $Id: mladapt.c 3416 2011-03-23 03:02:18Z metze $
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
    Revision 3.8  2005/02/24 16:06:55  metze
    Cosmetics for x86

    Revision 3.7  2003/08/14 11:20:15  fuegen
    Merged changes on branch jtk-01-01-15-fms (jaguar -> ibis-013)

    Revision 3.6.4.12  2003/02/05 09:01:32  soltau
    changed interface for cbAccuClear

    Revision 3.6.4.11  2003/01/29 13:44:57  metze
    Bugfix from Yue

    Revision 3.6.4.10  2002/08/30 12:30:31  soltau
    Added storeAccus, restoreAccus

    Revision 3.6.4.9  2002/08/01 13:42:34  metze
    Fixes for clean documentation.

    Revision 3.6.4.8  2001/04/05 08:56:35  fuegen
    added some ifdef MTHREAD, to make the compilation without definition of MTHREAD possible

    Revision 3.6.4.7  2002/04/03 12:20:22  soltau
    Create threads on demand

    Revision 3.6.4.6  2002/04/02 16:48:23  soltau
    Added multi-threading support for mladaptAdaptCBFull and
    mladaptSATAccuFull

    Revision 3.6.4.5  2002/03/20 11:37:58  soltau
    Support for covariances in Store/Restore

    Revision 3.6.4.4  2002/03/18 17:46:21  soltau
    use bmem matrix allocation from bmem module

    Revision 3.6.4.3  2002/03/14 10:01:38  soltau
    fvectorCreate_BMEM  clear memory

    Revision 3.6.4.2  2002/03/11 10:42:27  soltau
    Added Block memory management for SAT

    Revision 3.6.4.1  2002/02/21 11:13:58  soltau
    Enabled adaptation over different feature streams

    Revision 3.6  2001/01/15 09:49:59  janus
    Merged Christian's version with WINDOWS support (branch jtk-00-11-14-fuegen)

    Revision 3.5  2001/01/10 16:58:37  janus
    Merged Florian's version, supporting e.g. WAV-Files, Multivar and NGETS

    Revision 3.4.4.1  2001/01/12 15:16:59  janus
    necessary changes for running janus under WINDOWS

    Revision 3.4  2000/11/14 12:35:24  janus
    Merging branch jtk-00-10-27-jmcd.

    Revision 3.3.34.1  2000/11/06 10:50:36  janus
    Made changes to enable compilation under -Wall.

    Revision 3.3  1998/03/10 09:48:01  kemp
    Added method 'transform' which allows printing (TCL-style) of the
    computed transformation matrices

 * Revision 3.2  1997/06/17  17:41:43  westphal
 * *** empty log message ***
 *

   ======================================================================== */

#include "common.h"
#include "itf.h"
#include "mladapt.h"
#include "mach_ind_io.h"

#ifdef MTHREAD

#include <pthread.h>
#include <semaphore.h>
#define  NUM_THREADS 2
#ifndef _REENTRANT
#define  _REENTRANT
#endif

#endif

char mladaptRcsVersion[]= 
           "@(#)1$Id: mladapt.c 3416 2011-03-23 03:02:18Z metze $";


/* ------------------------------------------------------------------------
    Forward Declarations
   ------------------------------------------------------------------------ */

extern TypeInfo mladaptItemInfo;
extern TypeInfo mladaptInfo;

/* ------------------------------------------------------------------------
    Bitarray Macros
   ------------------------------------------------------------------------ */

#define SETBIT(c,i)   ((c)[i/8] |= (       (1 << (i%8))))
#define TESTBIT(c,i)  ((c)[i/8] &  (       (1 << (i%8))))
#define UNSETBIT(c,i) ((c)[i/8] &= (0xff ^ (1 << (i%8))))

/* ========================================================================
    MLAdaptItem
   ======================================================================== */
/* ------------------------------------------------------------------------
    mladaptInit  initialize a MLAdaptItem object
   ------------------------------------------------------------------------ */

int mladaptItemInit( MLAdaptItem* mlaiP)
{
  assert(mlaiP);

  mlaiP->cbX     = -1;
  mlaiP->refX    = -1;
  mlaiP->wX      = -1;
  mlaiP->wbestX  = -1;
  mlaiP->count   =  0.0;

  mlaiP->muAccuV =  NULL;
  mlaiP->muAccuM =  NULL;
  mlaiP->muCovAV =  NULL;
  mlaiP->muGamma =  0.0;

  mlaiP->mu      =  NULL;
  mlaiP->ac      =  NULL;

  return TCL_OK;
}

/* ------------------------------------------------------------------------
    mladaptItemDeinit  deinitialize a MLAdaptItem object
   ------------------------------------------------------------------------ */

int mladaptItemDeinit( MLAdaptItem* mlaiP)
{
  assert( mlaiP);
  if ( mlaiP->muAccuV) { fvectorFree( mlaiP->muAccuV); mlaiP->muAccuV = NULL; }
  if ( mlaiP->muAccuM) { fmatrixFree( mlaiP->muAccuM); mlaiP->muAccuM = NULL; }
  if ( mlaiP->muCovAV) { fvectorFree( mlaiP->muCovAV); mlaiP->muCovAV = NULL; }
  if ( mlaiP->mu)      { free(        mlaiP->mu);      mlaiP->mu      = NULL; }
  if ( mlaiP->ac)      { free(        mlaiP->ac);      mlaiP->ac      = NULL; }

  return TCL_OK;
}

/* ------------------------------------------------------------------------
    mladaptItemConfigureItf   configure a MLAdaptItem
   ------------------------------------------------------------------------ */

static int mladaptItemConfigureItf(ClientData cd, char *var, char *val)
{
  MLAdaptItem *cbmiP = (MLAdaptItem*)cd;
  if (! cbmiP) return TCL_ERROR;

  if (! var) {
    ITFCFG(mladaptItemConfigureItf,cd,"cbX");
    ITFCFG(mladaptItemConfigureItf,cd,"refX");
    ITFCFG(mladaptItemConfigureItf,cd,"wX");
    ITFCFG(mladaptItemConfigureItf,cd,"wbestX");
    ITFCFG(mladaptItemConfigureItf,cd,"count");
    return TCL_OK;
  }
  ITFCFG_Int(    var,val,"cbX",    cbmiP->cbX,    0);
  ITFCFG_Int(    var,val,"refX",   cbmiP->refX,   0);
  ITFCFG_Int(    var,val,"wX",     cbmiP->wX,     1);
  ITFCFG_Int(    var,val,"wbestX", cbmiP->wbestX, 1);
  ITFCFG_Float(  var,val,"count",  cbmiP->count,  0);
  ERROR("unknown option '-%s %s'.\n", var, val ? val : "?");
  return TCL_ERROR;   
}

/* ------------------------------------------------------------------------
    mladaptItemPutsItf
   ------------------------------------------------------------------------ */

static int mladaptItemPutsItf (ClientData cd, int argc, char *argv[]) 
{
  MLAdaptItem *cbmiP = (MLAdaptItem*)cd;
  itfAppendResult("%d %d", cbmiP->cbX, cbmiP->refX);
  return TCL_OK;
}

/* ========================================================================
    MLAdaptNode      Regression Tree Object
   ======================================================================== */
/* ------------------------------------------------------------------------
    mladaptNodeInit  initialize a MLAdaptNode object
   ------------------------------------------------------------------------ */

int mladaptNodeInit( MLAdaptNode* mlanP, int first, int last, float count)
{
  assert(mlanP);

  mlanP->refF   = first;
  mlanP->refL   = last;
  mlanP->count  = count;

  mlanP->w      = -1;
  mlanP->idxN   = 0;
  mlanP->idxA   = NULL;
  mlanP->accuA  = NULL;

  mlanP->leftP  = NULL;
  mlanP->rightP = NULL;
  return TCL_OK;
}

MLAdaptNode* mladaptNodeCreate( int first, int last, float count)
{
  MLAdaptNode *mlanP = (MLAdaptNode*)malloc(sizeof(MLAdaptNode));

  if ((!mlanP) || (mladaptNodeInit(mlanP, first, last, count) != TCL_OK)) {
    if (mlanP) free (mlanP);
    ERROR("Cannot create MLAdaptNode object.\n");
    return NULL;
  }
  return mlanP;
}

/* ------------------------------------------------------------------------
    mladaptNodeDeinit  deinitialize a MLAdaptNode object
   ------------------------------------------------------------------------ */

int mladaptNodeFree( MLAdaptNode* mlanP);

int mladaptNodeDeinit( MLAdaptNode* mlanP)
{
  assert( mlanP);

  if ( mlanP->idxA)   { free(mlanP->idxA);              mlanP->idxA   = NULL; }
  if ( mlanP->accuA)  { free(mlanP->accuA);             mlanP->accuA  = NULL; }
  if ( mlanP->leftP)  { mladaptNodeFree(mlanP->leftP);  mlanP->leftP  = NULL; }
  if ( mlanP->rightP) { mladaptNodeFree(mlanP->rightP); mlanP->rightP = NULL; }

  mlanP->idxN = 0;
  return TCL_OK;
}

int mladaptNodeFree( MLAdaptNode* mlanP)
{
  if (mladaptNodeDeinit(mlanP) != TCL_OK) return TCL_ERROR;
  free(mlanP);
  return TCL_OK;
}

/* ------------------------------------------------------------------------
    mladaptNodeSplit
   ------------------------------------------------------------------------ */

int mladaptNodeSplit( MLAdaptNode* mlanP, MLAdapt* cbmP, int level, 
                                                         int maxDepth,
                      int kmeansN, float kmeansBetaS, float kmeansBetaF)
{
  int      dimN    = cbmP->dimN;
  FMatrix* dataM   = NULL;
  FMatrix* kmeansM = NULL;
  int      n       = mlanP->refL-mlanP->refF+1;
  int      nodesN  = 1;
  int      r, rl;
  int      c[2]    = {0,0};
  double   kmeansError = 0.0;  

  if ( mlanP->leftP || mlanP->rightP || n < 1 || level >= maxDepth) {

    /* -------------------------------- */
    /* leaf node of the regression tree */
    /* -------------------------------- */

    mlanP->idxN  =  cbmP->itemN;
    mlanP->idxA  = (char*)malloc( sizeof(char) * (mlanP->idxN / 8 + 1));
    mlanP->accuA =  NULL;

    assert( mlanP->idxA);

    for ( r = 0; r < mlanP->idxN; r++) {
      if ( mlanP->refF <= r && r <= mlanP->refL)
           SETBIT(   mlanP->idxA, r);
      else UNSETBIT( mlanP->idxA, r);
    }
    return nodesN;
  }

  /* --------------------------------------------------------------- */
  /* Allocate matrices to save all mean vectors of the codebooks and */
  /* two reference vectors to run k-means                            */
  /* --------------------------------------------------------------- */

  if (! ( dataM   = fmatrixCreate(n,dimN)) ||
      ! ( kmeansM = fmatrixCreate(2,dimN))) {

    if ( dataM)   fmatrixFree( dataM);
    if ( kmeansM) fmatrixFree( kmeansM);
    ERROR("Cannot allocate cluster matrices %d x %d.", n, dimN);
    return nodesN;
  }

  for ( r = mlanP->refF, rl = mlanP->refL; r <= rl; r++) {
    Codebook*  cP   = cbmP->cbsP->list.itemA + cbmP->itemA[r].cbX;
    int        refX = cbmP->itemA[r].refX;
    int        j;

    for ( j = 0; j < dimN; j++)
      dataM->matPA[r-mlanP->refF][j] = cP->rv->matPA[refX][j];

    cbmP->itemA[r].wX = r-mlanP->refF;
  }

  if (10.0 < (kmeansError = fmatrixNeuralGas( kmeansM, dataM, kmeansN, 
                                            kmeansBetaS, kmeansBetaF, NULL, 1, 1))) {
      INFO ("fmatrixNeuralGas returns %f\n", kmeansError);
  }

  for ( r = mlanP->refF, rl = mlanP->refL; r <= rl; ) {
    int        i, j, k;
    float      d[2];

    for ( i = 0; i < 2; i++) {
      d[i] = 0.0;
      for ( j = 0; j < dimN; j++) {
        d[i] += ( kmeansM->matPA[i][j] - dataM->matPA[cbmP->itemA[r].wX][j]) *
                ( kmeansM->matPA[i][j] - dataM->matPA[cbmP->itemA[r].wX][j]);
      }
    }
    k = ( d[0] < d[1]) ? 0 : 1;
    c[k]++;

    if ( k) {
      MLAdaptItem l   = cbmP->itemA[r ];
      cbmP->itemA[r ] = cbmP->itemA[rl];
      cbmP->itemA[rl] = l;
      rl--;
    }
    else r++;
  }

  if ( c[0] && c[1]) {
    mlanP->leftP   = mladaptNodeCreate( mlanP->refF, mlanP->refF+c[0]-1, 0);
    nodesN        += mladaptNodeSplit(  mlanP->leftP, cbmP, level+1, maxDepth,
                                        kmeansN, kmeansBetaS, kmeansBetaF);

    mlanP->rightP  = mladaptNodeCreate( mlanP->refF+c[0], mlanP->refL, 0);
    nodesN        += mladaptNodeSplit(  mlanP->rightP, cbmP,level+1, maxDepth,
                                        kmeansN, kmeansBetaS, kmeansBetaF);
  }

  if ( dataM)   fmatrixFree( dataM);
  if ( kmeansM) fmatrixFree( kmeansM);

  return nodesN;
}

/* ------------------------------------------------------------------------
    mladaptTreeSave  save tree and tree accus to a file
   ------------------------------------------------------------------------ */

int mladaptNodeSave( MLAdaptNode* mlanP, MLAdapt* cbmP, FILE* fp)
{
  if (! mlanP->leftP || ! mlanP->rightP) {
    int i;

    /* ----------------------------------- */
    /* leaf node of the tree, thus save it */
    /* ----------------------------------- */

    assert( mlanP->idxA);
    fwrite( mlanP->idxA, sizeof(char), (mlanP->idxN/8+1), fp);

    /* -------------------------------- */
    /* write accu if exist and non-zero */
    /* -------------------------------- */

    if ( mlanP->accuA) {
      for ( i = 0; i < mlanP->idxN; i++) if ( mlanP->accuA[i] > 0.0) break;
      if  ( i < mlanP->idxN) {
        write_int(    fp, mlanP->idxN);
        write_floats( fp, mlanP->accuA, mlanP->idxN);
      }
      else write_int( fp, 0);
    } else write_int( fp, 0);
  }
  else {
    int rc;

    if ((rc = mladaptNodeSave( mlanP->leftP,  cbmP, fp)) != TCL_OK ||
        (rc = mladaptNodeSave( mlanP->rightP, cbmP, fp)) != TCL_OK) return rc;
  }
  return TCL_OK;
}

int mladaptTreeSave( MLAdapt* cbmP, char* fileName)
{
  FILE*    fp;
  int      rc;

  if (! cbmP->nodeP) {
    ERROR("There is no clustering tree for MLAdapt.\n");
    return TCL_ERROR;
  }
  if ((fp=fileOpen(fileName,"w"))==NULL) { 
    ERROR("Can't open file %s for writing.\n",fileName); 
    return TCL_ERROR;
  }
  rc = mladaptNodeSave( cbmP->nodeP, cbmP, fp);

  fileClose(fileName, fp);
  return rc;  
}

static int mladaptTreeSaveItf (ClientData clientData, int   argc, 
                                                     char* argv[]) 
{
  char *filename = NULL; int ac=argc-1;

  if ( itfParseArgv( argv[0], &ac, argv+1, 0,
      "<filename>", ARGV_STRING, NULL, &filename, clientData, 
      "file to save accumulators",
       NULL) != TCL_OK) return TCL_ERROR;

  return mladaptTreeSave((MLAdapt*)clientData, filename); 
}

/* ------------------------------------------------------------------------
    mladaptTreeLoad  load tree and tree accus from a file
   ------------------------------------------------------------------------ */

int mladaptNodeLoad( MLAdaptNode* mlanP, MLAdapt* cbmP, FILE* fp)
{
  if (! mlanP->leftP || ! mlanP->rightP) {
    int    i;
    int    idxN;

    /* ----------------------------------- */
    /* leaf node of the tree, thus load it */
    /* ----------------------------------- */

    assert( mlanP->idxA);
    fread(  mlanP->idxA, sizeof(char), (mlanP->idxN/8+1), fp);

    /* ------------------ */
    /* read accu if exist */
    /* ------------------ */

    idxN = read_int( fp);

    if ( idxN == mlanP->idxN) {
      if (! mlanP->accuA) {
        mlanP->accuA = (float*)malloc( sizeof(float) * idxN);

        assert( mlanP->accuA);

        for ( i = 0; i < mlanP->idxN; i++) mlanP->accuA[i]  = read_float( fp);
      } else {
        for ( i = 0; i < mlanP->idxN; i++) mlanP->accuA[i] += read_float( fp);
      }
    }
  }
  else {
    int rc;

    if ((rc = mladaptNodeLoad( mlanP->leftP,  cbmP, fp)) != TCL_OK ||
        (rc = mladaptNodeLoad( mlanP->rightP, cbmP, fp)) != TCL_OK) return rc;
  }
  return TCL_OK;
}

int mladaptTreeLoad( MLAdapt* cbmP, char* fileName)
{
  FILE*    fp;
  int      rc;

  if (! cbmP->nodeP) {
    ERROR("There is no clustering tree for MLAdapt.\n");
    return TCL_ERROR;
  }
  if ((fp=fileOpen(fileName,"r"))==NULL) { 
    ERROR("Can't open file %s for reading.\n",fileName); 
    return TCL_ERROR;
  }
  rc = mladaptNodeLoad( cbmP->nodeP, cbmP, fp);

  fileClose(fileName, fp);
  return rc;  
}

static int mladaptTreeLoadItf (ClientData clientData, int   argc, 
                                                     char* argv[]) 
{
  char *filename = NULL; int ac=argc-1;

  if ( itfParseArgv( argv[0], &ac, argv+1, 0,
      "<filename>", ARGV_STRING, NULL, &filename, clientData, 
      "file to save accumulators",
       NULL) != TCL_OK) return TCL_ERROR;

  return mladaptTreeLoad((MLAdapt*)clientData, filename); 
}

/* ------------------------------------------------------------------------
    mladaptNodeAccu
   ------------------------------------------------------------------------ */

int mladaptNodeAccu( MLAdaptNode* mlanP, MLAdapt* cbmP, int w)
{
  if (  mlanP->w != -1) w = mlanP->w;
  if (! mlanP->leftP || ! mlanP->rightP) {
    int    r;

    /* --------------------- */
    /* leaf node of the tree */
    /* --------------------- */

    assert( mlanP->idxA);

    for ( r = 0; r < mlanP->idxN; r++) {
      if ( cbmP->itemA[r].wbestX == w) {
        if (! mlanP->accuA) {
          int i;
          mlanP->accuA = (float*)malloc( sizeof(float) * mlanP->idxN);

          assert( mlanP->accuA);

          for ( i = 0; i < mlanP->idxN; i++) mlanP->accuA[i] = 0.0;
	}
        mlanP->accuA[r] += cbmP->itemA[r].count;
      }
    }
  }
  else {
    int rc;

    if ((rc = mladaptNodeAccu( mlanP->leftP,  cbmP, w)) != TCL_OK ||
        (rc = mladaptNodeAccu( mlanP->rightP, cbmP, w)) != TCL_OK) return rc;
  }
  return TCL_OK;
}

/* ------------------------------------------------------------------------
    mladaptTreeAccu     accumulate information about the best
                        transformation to be used for each codevector.
   ------------------------------------------------------------------------ */

int mladaptTreeAccu( MLAdapt* cbmP)
{
  int      rc;
  int      i;
  FVector* mu   = NULL;
  FVector* f    = NULL;
  int      dimN = cbmP->dimN;

  if (! cbmP->nodeP) {
    ERROR("There is no clustering tree for MLAdapt.\n");
    return TCL_ERROR;
  }
  if (  cbmP->wX < 0) {
    ERROR("There a no transformations defined.\n");
    return TCL_ERROR;
  }

  /* ------------------------------------------------------------ */
  /* based on the likelihood compute what the best transformation */
  /* for each codevector would be.                                */
  /* ------------------------------------------------------------ */

  mu = fvectorCreate( dimN);
  f  = fvectorCreate( dimN+1);

  for ( i = 0; i < cbmP->itemN; i++) {
    MLAdaptItem*  itemP = cbmP->itemA + i;
    Codebook*     cP    = cbmP->cbsP->list.itemA + itemP->cbX;
    int           refX  = itemP->refX;
    CodebookAccu* caP   = cP->accu;

    itemP->wbestX = itemP->wX;

    if ( caP && caP->count && itemP->mu) {
      double bestL = 0.0;
      double count = 0.0;
      int    subX;
      int    w;

      for ( subX = 0; subX < caP->subN; subX++) count += caP->count[subX][refX];

      if ( count > 0.0) {
        int dimX;

        f->vecA[0] = 1;
        for ( dimX = 0; dimX < dimN; dimX++) f->vecA[dimX+1] = itemP->mu[dimX];

        for ( w = 0; w < cbmP->wX; w++) {
          double l = 0;
          int    j;

          /* ---------------------------------------------- */
          /* compute W.mu based on the original mu (stored) */
          /* ---------------------------------------------- */

          for ( dimX = 0; dimX < dimN; dimX++) {
            double d = 0;
            for ( j = 0; j <= dimN; j++) d += f->vecA[j] * 
                                              cbmP->wA[w]->matPA[dimX][j];
            mu->vecA[dimX] = d;
          }

          /* --------------------------------- */
          /* compute the likelihood given W.mu */
          /* --------------------------------- */

          for ( subX = 0; subX < caP->subN; subX++) {
            double x = caP->count[subX][refX];
            if ( x > 0.0) {
              for ( dimX = 0; dimX < dimN; dimX++) {
                double s = cP->cv[refX]->m.d[dimX];
                double y = caP->rv[subX]->matPA[refX][dimX];
  
                l += 2 * s * mu->vecA[dimX] * y -
                     x * s * mu->vecA[dimX] * mu->vecA[dimX];
              }
	    }
          }
          if (! w || l > bestL) {
            itemP->wbestX = w;
            bestL         = l;
          }
        }
      }
    }
  }

  if ( mu) { fvectorFree( mu); mu = NULL; }
  if (  f) { fvectorFree(  f); f  = NULL; }

  /* ------------------------------- */
  /* accumulate the assignment found */
  /* ------------------------------- */

  rc = mladaptNodeAccu( cbmP->nodeP, cbmP, -1);

  return rc;  
}

static int mladaptTreeAccuItf (ClientData clientData, int   argc, 
                                                      char* argv[]) 
{
  int ac = argc-1;

  if ( itfParseArgv( argv[0], &ac, argv+1, 0,
       NULL) != TCL_OK) return TCL_ERROR;

  return mladaptTreeAccu((MLAdapt*)clientData); 
}

/* ------------------------------------------------------------------------
    mladaptNodeClearAccu  traverse the tree recursively to reset all
                          accumulators to 0
   ------------------------------------------------------------------------ */

int mladaptNodeClearAccu( MLAdaptNode* mlanP, MLAdapt* cbmP)
{
  if (! mlanP->leftP || ! mlanP->rightP) {

    /* --------------------- */
    /* leaf node of the tree */
    /* --------------------- */

    assert ( mlanP->idxA);

    if ( mlanP->accuA) { free( mlanP->accuA); mlanP->accuA = NULL; }
  }
  else {
    int rc;
    if ((rc = mladaptNodeClearAccu( mlanP->leftP,  cbmP)) != TCL_OK ||
        (rc = mladaptNodeClearAccu( mlanP->rightP, cbmP)) != TCL_OK) 
         return rc;
  }
  return TCL_OK;
}

int mladaptTreeClear( MLAdapt* cbmP)
{
  if (! cbmP->nodeP) {
    ERROR("There is no clustering tree for MLAdapt.\n");
    return TCL_ERROR;
  }
  return mladaptNodeClearAccu( cbmP->nodeP, cbmP);
}

static int mladaptTreeClearItf (ClientData clientData, int   argc, 
                                                       char* argv[]) 
{
  int ac = argc-1;

  if ( itfParseArgv( argv[0], &ac, argv+1, 0,
       NULL) != TCL_OK) return TCL_ERROR;

  return mladaptTreeClear((MLAdapt*)clientData); 
}

/* ------------------------------------------------------------------------
    mladaptNodeMaxAccu    traverse the tree recursively to find the max
                          count for each accu resp. codevector.
   ------------------------------------------------------------------------ */

int mladaptNodeMaxAccu( MLAdaptNode* mlanP, MLAdapt* cbmP, float* maxA)
{
  assert( maxA);

  if (! mlanP->leftP || ! mlanP->rightP) {
    int    r;

    /* --------------------- */
    /* leaf node of the tree */
    /* --------------------- */

    assert ( mlanP->idxA);
    
    if ( mlanP->accuA) {
      for ( r = 0; r < mlanP->idxN; r++)
        if ( maxA[r] < mlanP->accuA[r]) maxA[r] = mlanP->accuA[r];
    }
  }
  else {
    int rc;
    if ((rc = mladaptNodeMaxAccu( mlanP->leftP,  cbmP, maxA)) != TCL_OK ||
        (rc = mladaptNodeMaxAccu( mlanP->rightP, cbmP, maxA)) != TCL_OK) 
         return rc;
  }
  return TCL_OK;
}


/* ------------------------------------------------------------------------
    mladaptNodeOpt        traverse the tree recursively to reset all
                          accumulators to 0
   ------------------------------------------------------------------------ */

int mladaptNodeOpt( MLAdaptNode* mlanP, MLAdapt* cbmP, float* accuA)
{
  int rc = 0;

  assert( mlanP);
  assert( cbmP);
  assert( accuA);

  if (! mlanP->leftP || ! mlanP->rightP) {
    int    r;

    /* --------------------- */
    /* leaf node of the tree */
    /* --------------------- */

    assert ( mlanP->idxA);

    for ( r = 0; r < mlanP->idxN; r++) {
      if (   accuA[r] > 0.0) {
        if ( mlanP->accuA && accuA[r] == mlanP->accuA[r]) {
          if (! TESTBIT( mlanP->idxA, r)) rc++;
          SETBIT(   mlanP->idxA, r);
                                     /* ------------------------------ */
          accuA[r] += 1.0;           /* hack to prevent being assigned */
                                     /* to more than one node          */
                                     /* ------------------------------ */
        } else {
          UNSETBIT( mlanP->idxA, r);
        }
      }
    }
  }
  else {

    /* ----------------------- */
    /* pseudo-balance the tree */
    /* ----------------------- */

    if ( rand() % 2) {
      rc = mladaptNodeOpt( mlanP->leftP,  cbmP, accuA) +
           mladaptNodeOpt( mlanP->rightP, cbmP, accuA);
    } else {
      rc = mladaptNodeOpt( mlanP->rightP, cbmP, accuA) +
           mladaptNodeOpt( mlanP->leftP,  cbmP, accuA);
    }
  }
  return rc;
}

int mladaptTreeOpt( MLAdapt* cbmP)
{
  float* accuA = NULL;
  int    rc    = TCL_OK;
  int    r;

  if (! cbmP->nodeP) {
    ERROR("There is no clustering tree for MLAdapt.\n");
    return -1;
  }

  /* --------------------------------------------- */
  /* compute the MAX count per codevector and node */
  /* --------------------------------------------- */

  accuA = (float*)malloc( sizeof(float) * cbmP->itemN);

  assert( accuA);

  for ( r = 0; r < cbmP->itemN; r++) accuA[r] = 0.0;

  if ( mladaptNodeMaxAccu( cbmP->nodeP, cbmP, accuA) != TCL_OK) {
    if ( accuA) { free( accuA); accuA = NULL; }
    return -1;
  }

  /* ----------------------------------------------------- */
  /* move codevectors to those nodes which have MAX counts */
  /* ----------------------------------------------------- */

  rc = mladaptNodeOpt( cbmP->nodeP, cbmP, accuA);

  if ( accuA) { free( accuA); accuA = NULL; }
  return rc;  
}

static int mladaptTreeOptItf (ClientData clientData, int   argc, 
                                                     char* argv[]) 
{
  int ac = argc-1;

  if ( itfParseArgv( argv[0], &ac, argv+1, 0,
       NULL) != TCL_OK) return TCL_ERROR;

  itfAppendResult( "%d",  mladaptTreeOpt((MLAdapt*)clientData));
  return TCL_OK; 
}


/* ========================================================================
    MLAdapt
   ======================================================================== */

#ifdef MTHREAD
/* forward declarations */
static void mlaThreadCollectInit();
static void satThreadCollectInit();
#endif

/* ------------------------------------------------------------------------
    mladaptInit  initialize a MLAdapt object
   ------------------------------------------------------------------------ */

int mladaptInit( MLAdapt* cbmP, char* name, CodebookSet* cbsP, int mode, 
		 int bmem, int thread)
{
  assert(cbmP);
  cbmP->name   =  strdup( name);
  cbmP->useN   =  0;
  cbmP->itemN  =  0;
  cbmP->itemA  =  NULL;
  cbmP->cbsP   =  cbsP; link( cbsP, "CodebookSet");
  cbmP->dimN   =  0;
  cbmP->featX  = -1;
  cbmP->type   =  COV_DIAGONAL;
  cbmP->nodeP  =  NULL;
  cbmP->mode   =  mode;

  cbmP->wX     =  0;
  cbmP->wN     =  0;
  cbmP->wA     =  NULL;
  cbmP->memP   = NULL;
  cbmP->mem2P  = NULL;

  if (bmem) {
    cbmP->memP = bmemCreate(65536,0);

#ifdef MTHREAD
#if NUM_THREADS != 2
    ERROR("mladaptInit: Incompatible block memory management for multi-threaded system");
    return TCL_ERROR;
#else
    if (thread) {
      mlaThreadCollectInit();
      satThreadCollectInit();
      cbmP->mem2P = bmemCreate(65536,0);
    }
#endif
#endif
  }

  return TCL_OK;
}

MLAdapt* mladaptCreate( char* name, CodebookSet* cbsP, int mode, int bmem, int thread)
{
  MLAdapt *cbmP = (MLAdapt*)malloc(sizeof(MLAdapt));

  if ((!cbmP) || (mladaptInit(cbmP, name, cbsP, mode, bmem, thread) != TCL_OK)) {
    if (cbmP) free (cbmP);
    ERROR("Cannot create MLAdapt object %s.\n", name);
    return NULL;
  }
  return cbmP;
}

static ClientData mladaptCreateItf (ClientData cd, int argc, char *argv[])
{
  CodebookSet* cbsP  = NULL; 
  char*        name  = NULL;
  int          ac    = argc;
  int          mode  = 2;
  int          bmem  = 0;
  int         thread = 0;

  if ( itfParseArgv( argv[0], &ac, argv, 0,
      "<name>",       ARGV_STRING, NULL, &name, NULL,           "name of MLAdapt object",
      "<CodebookSet>",ARGV_OBJECT, NULL, &cbsP, "CodebookSet",  "name of the codebook set", 
      "-mode",        ARGV_INT,    NULL, &mode, NULL,           "0=mean 1=diagonal 2=full",
      "-bmem",        ARGV_INT,    NULL, &bmem, NULL,           "use block memory management for SAT",
      "-thread",      ARGV_INT,    NULL, &thread, NULL,         "use multiple threads for MLLR/SAT",
       NULL) != TCL_OK) return NULL;

  return (ClientData)mladaptCreate( name, cbsP, mode,bmem,thread);
}

/* ------------------------------------------------------------------------
    mladaptDeinit  deinitialize a MLAdapt object
   ------------------------------------------------------------------------ */

int mladaptDeinit( MLAdapt* cbmP)
{
  assert( cbmP);

  if ( cbmP->itemA) {
    int i;
    for ( i = 0; i < cbmP->itemN; i++) mladaptItemDeinit( cbmP->itemA + i);
    free( cbmP->itemA);
    cbmP->itemA = NULL; cbmP->itemN = 0;
  }
  if ( cbmP->wA) {
    int i;
    for ( i = 0; i < cbmP->wN; i++) {
      if ( cbmP->wA[i]) { dmatrixFree( cbmP->wA[i]); cbmP->wA[i] = NULL; }
    }
    free( cbmP->wA); cbmP->wA = NULL;
  }
  if ( cbmP->nodeP) { mladaptNodeFree( cbmP->nodeP); cbmP->nodeP = NULL; }

  if (cbmP->memP) 
    bmemFree(cbmP->memP);
  if (cbmP->mem2P) 
    bmemFree(cbmP->mem2P);

  cbmP->memP  = NULL;
  cbmP->mem2P = NULL;

  return TCL_OK;
}

int mladaptFree( MLAdapt* cbmP)
{
  if (mladaptDeinit(cbmP) != TCL_OK) return TCL_ERROR;
  free(cbmP);
  return TCL_OK;
}

static int mladaptFreeItf (ClientData cd)
{
  return mladaptFree((MLAdapt*)cd);
}

/* ------------------------------------------------------------------------
    mladaptConfigureItf     configure items in the MLAdapt object
   ------------------------------------------------------------------------ */

static int mladaptConfigureItf(ClientData cd, char *var, char *val)
{
  MLAdapt *cbmP = (MLAdapt*)cd;
  if (! cbmP) return TCL_ERROR;
  if (! var) {
    ITFCFG(mladaptConfigureItf,cd,"name");
    ITFCFG(mladaptConfigureItf,cd,"useN");
    ITFCFG(mladaptConfigureItf,cd,"itemN");
    ITFCFG(mladaptConfigureItf,cd,"dimN");
    ITFCFG(mladaptConfigureItf,cd,"featX");
    return TCL_OK;
  }
  ITFCFG_CharPtr(var,val,"name",  cbmP->name,   1);
  ITFCFG_Int(    var,val,"useN",  cbmP->useN,   1);
  ITFCFG_Int(    var,val,"itemN", cbmP->itemN,  1);
  ITFCFG_Int(    var,val,"dimN",  cbmP->dimN,   1);
  ITFCFG_Int(    var,val,"featX", cbmP->featX,  1);
  ERROR("unknown option '-%s %s'.\n", var, val ? val : "?");
  return TCL_ERROR;   
}

/* ------------------------------------------------------------------------
    mladaptAccessItf     access subitems in the MLAdapt object
   ------------------------------------------------------------------------ */

static ClientData mladaptAccessItf (ClientData cd, char *name, TypeInfo **ti)
{
  MLAdapt* cbmP = (MLAdapt*)cd;
  int          idx;

  if ( *name == '.') {
    if ( name[1] =='\0') {
      if ( cbmP->itemN > 0) {
        itfAppendElement("item(0..%d)", cbmP->itemN-1);
      }
      *ti=NULL; return NULL; 
    }
    else {
      if (sscanf(name+1,"item(%d)", &idx) == 1) {
        *ti = itfGetType("MLAdaptItem"); 
        if ( idx >= 0 && idx < cbmP->itemN)
             return (ClientData)(cbmP->itemA+idx);
        else return  NULL;
      }
      *ti=NULL; return NULL;
    }
  }
  return NULL;
}

/* ------------------------------------------------------------------------
    mladaptPuts
   ------------------------------------------------------------------------ */

static int mladaptPutsItf (ClientData clientData, int argc, char *argv[]) 
{
  MLAdapt* cbmP = (MLAdapt*)clientData;
  int          i;

  for ( i = 0; i < cbmP->itemN; i++) {
    itfAppendResult("{ ");
    mladaptItemPutsItf((ClientData)(cbmP->itemA+i), argc, argv);
    itfAppendResult("}\n");
  }
  return TCL_OK;
}

/* ------------------------------------------------------------------------
    mladaptAdd   adds a single or a number of entries to the list in the
                 MLAdapt (itemA) and fills it with idxX -> idxY values
                 after having them initialized
   ------------------------------------------------------------------------ */

int mladaptAdd( MLAdapt* cbmP, int cbX)
{
  int n = cbmP->cbsP->list.itemA[cbX].refN;
  int j = 0, i;

  if ( cbmP->itemN) {
    if ( cbmP->dimN  != cbmP->cbsP->list.itemA[cbX].dimN || 
         cbmP->type  != cbmP->cbsP->list.itemA[cbX].type) {
         ERROR("Codebooks in MLAdapt object do not match.\n");
         return TCL_ERROR;
    }
  }
  else {
    cbmP->dimN  = cbmP->cbsP->list.itemA[cbX].dimN; 
    cbmP->featX = cbmP->cbsP->list.itemA[cbX].featX;
    cbmP->type  = cbmP->cbsP->list.itemA[cbX].type;
  }

  cbmP->itemA = (MLAdaptItem*)realloc( cbmP->itemA,(cbmP->itemN+n)*
                                       sizeof(MLAdaptItem));

  for ( i = cbmP->itemN; i < cbmP->itemN+n; i++) {
    mladaptItemInit( cbmP->itemA + i);

    cbmP->itemA[i].cbX  = cbX;
    cbmP->itemA[i].refX = j++;
  }
  i            = cbmP->itemN;
  cbmP->itemN += n;
  return i;
}

static int mladaptAddItf( ClientData clientData, int argc, char *argv[]) 
{
  MLAdapt *cbmP  = (MLAdapt*)clientData;
  int      ac    =  argc-1;
  int      cbX   = -1;

  if ( itfParseArgv (argv[0], &ac, argv+1, 0,
      "<Codebook>", ARGV_LINDEX, NULL, &cbX,&(cbmP->cbsP->list), "codebook",
        NULL) != TCL_OK) return TCL_ERROR;

  itfAppendResult( "%d", mladaptAdd( cbmP, cbX));
  return TCL_OK;
}

/* ------------------------------------------------------------------------
    mladaptClear  all entries are deleted from the itemA list
   ------------------------------------------------------------------------ */

int mladaptClear( MLAdapt* cbmP)
{
  int i;
  for ( i = 0; i < cbmP->itemN; i++) mladaptItemDeinit( cbmP->itemA + i);

  if ( cbmP->itemA) { free( cbmP->itemA); cbmP->itemA = NULL; }
  cbmP->itemN = 0;
  return TCL_OK;
}

static int mladaptClearItf (ClientData clientData, int argc, char *argv[]) 
{
  MLAdapt *cbmP = (MLAdapt*)clientData;
  int      ac   =  argc-1;

  if ( itfParseArgv (argv[0], &ac, argv+1, 0, NULL) != TCL_OK) return TCL_ERROR;
  return mladaptClear( cbmP);
}

/* ------------------------------------------------------------------------
    mladaptStore      save the current gaussians of the codebooks
                      in the MLAdaptItems
   ------------------------------------------------------------------------ */

static int mladaptStore( MLAdapt* cbmP, int covar)
{
  int i;
  int dimN = cbmP->dimN;

  for ( i = 0; i < cbmP->itemN; i++) {
    MLAdaptItem* itemP = cbmP->itemA + i;
    Codebook*    cP    = cbmP->cbsP->list.itemA + itemP->cbX;
    int          refX  = itemP->refX;

    if (covar && cP->cv[0]->type != COV_DIAGONAL) {
      WARN("mladaptStore: cannot store non-diagonal covariances\n");
      covar = 0;
    }

    if (!  itemP->mu) {
      int allocN = dimN * (covar+1) * sizeof(float);
      if (NULL == (itemP->mu = malloc(allocN))) {
	ERROR("Couldn't allocate MLAdapt buffer");
	return TCL_ERROR;
      }
    }

    memcpy(itemP->mu,cP->rv->matPA[refX],dimN*sizeof(float));
    if (covar)
      memcpy(itemP->mu+dimN,cP->cv[refX]->m.d,dimN*sizeof(float));
  }
  return TCL_OK;
}

static int mladaptStoreItf (ClientData clientData, int argc, char *argv[]) 
{
  MLAdapt *cbmP = (MLAdapt*)clientData;
  int      ac   =  argc-1;
  int     covar = 0;

  if ( itfParseArgv( argv[0], &ac, argv+1, 0,
      "-covar", ARGV_INT, NULL, &covar, NULL, "store (diagonal) covariances",
		    NULL) != TCL_OK) return TCL_ERROR;

  return mladaptStore(cbmP,covar);
}

/* ------------------------------------------------------------------------
    mladaptRestore    restore mean vectors of the codebooks
   ------------------------------------------------------------------------ */

static int mladaptRestore( MLAdapt* cbmP, int covar)
{
  int i;
  int dimN = cbmP->dimN;

  for ( i = 0; i < cbmP->itemN; i++) {
    MLAdaptItem* itemP = cbmP->itemA + i;

    if ( itemP->mu) {
      Codebook*    cP    = cbmP->cbsP->list.itemA + itemP->cbX;
      int          refX  = itemP->refX;

      if (covar && cP->cv[0]->type != COV_DIAGONAL) {
	WARN("mladaptStore: cannot store non-diagonal covariances\n");
	covar = 0;
      }
      memcpy(cP->rv->matPA[refX],itemP->mu,dimN*sizeof(float));
      if (covar)
	memcpy(cP->cv[refX]->m.d,itemP->mu+dimN,dimN*sizeof(float));
    }
  }
  return TCL_OK;
}

static int mladaptRestoreItf (ClientData clientData, int argc, char *argv[]) 
{
  MLAdapt *cbmP = (MLAdapt*)clientData;
  int      ac   =  argc-1;
  int     covar = 0;

  if ( itfParseArgv( argv[0], &ac, argv+1, 0,
	"-covar", ARGV_INT, NULL, &covar, NULL, "restore (diagonal) covariances",
		     NULL) != TCL_OK) return TCL_ERROR;

  return mladaptRestore(cbmP,covar);
}

/* ------------------------------------------------------------------------
    mladaptStoreAccu  save the current accumulators of the codebooks
                      in the MLAdaptItems
   ------------------------------------------------------------------------ */

static int mladaptStoreAccu( MLAdapt* cbmP, int covar)
{
  int i;
  int dimN = cbmP->dimN;

  for ( i = 0; i < cbmP->itemN; i++) {
    MLAdaptItem*  itemP = cbmP->itemA + i;
    Codebook*        cP = cbmP->cbsP->list.itemA + itemP->cbX;
    CodebookAccu* accuP = cP->accu;
    int           refX  = itemP->refX;

    /* no accumulator */
    if (! accuP) {
      WARN("mladaptStoreAccu: no accumulators to store in %s\n",cP->name);
      continue;
    }

    /* accumulator isn't allocated yet : no warning */
    if (! accuP->count) {
      continue;
    }
 
    if (accuP->subN > 1)  {
      WARN("mladaptStoreAccu: cannot store %d sub-accumulators in %s\n",
	   accuP->subN,cP->name);
      continue;
    }

    if (covar && cP->cv[0]->type != COV_DIAGONAL) {
      WARN("mladaptStoreAccu: cannot store non-diagonal covariances in %s\n",cP->name);
      covar = 0;
    }

    if (!  itemP->ac) {
      int allocN = (1 + dimN * (covar+1)) * sizeof(double);
      if (NULL == (itemP->ac = malloc(allocN))) {
	ERROR("mladaptStoreAccu: Couldn't allocate MLAdapt buffer");
	return TCL_ERROR;
      }
    }
    /* count */
    itemP->ac[0] = accuP->count[0][refX];

    /* mean */
    memcpy(itemP->ac+1,accuP->rv[0]->matPA[refX],dimN*sizeof(double));

    /* covar */
    if (covar)
      memcpy(itemP->ac+1+dimN,accuP->sumOsq[0][refX].m.d,dimN*sizeof(double));

  }
  return TCL_OK;
}

static int mladaptStoreAccuItf (ClientData clientData, int argc, char *argv[]) 
{
  MLAdapt *cbmP = (MLAdapt*)clientData;
  int      ac   =  argc-1;
  int     covar = 0;

  if ( itfParseArgv( argv[0], &ac, argv+1, 0,
      "-covar", ARGV_INT, NULL, &covar, NULL, "store (diagonal) covariances",
		    NULL) != TCL_OK) return TCL_ERROR;

  return mladaptStoreAccu(cbmP,covar);
}

/* ------------------------------------------------------------------------
    mladaptRestoreAccu    restore accumulators of the codebooks
   ------------------------------------------------------------------------ */

static int mladaptRestoreAccu( MLAdapt* cbmP, int covar)
{
  int i;
  int dimN = cbmP->dimN;

  for ( i = 0; i < cbmP->itemN; i++) {
    MLAdaptItem* itemP = cbmP->itemA + i;

    if ( itemP->ac) {
      Codebook*        cP = cbmP->cbsP->list.itemA + itemP->cbX;
      CodebookAccu* accuP = cP->accu;
      int           refX  = itemP->refX;

      if (covar && cP->cv[0]->type != COV_DIAGONAL) {
	WARN("mladaptRestore: cannot restore non-diagonal covariances\n");
	covar = 0;
      }

      /* count */
      accuP->count[0][refX] = itemP->ac[0];

      /* mean */
      memcpy(accuP->rv[0]->matPA[refX],itemP->ac+1,dimN*sizeof(double));

      /* covariance */
      if (covar)
	memcpy(accuP->sumOsq[0][refX].m.d,itemP->ac+1+dimN,dimN*sizeof(double));
    } else {
      Codebook*        cP = cbmP->cbsP->list.itemA + itemP->cbX;
      CodebookAccu* accuP = cP->accu;

      /* clear accumulator if there wasn't an allocated accu at 'store' */
      if (accuP) cbAccuClear(accuP,-1);
    }
  }
  return TCL_OK;
}

static int mladaptRestoreAccuItf (ClientData clientData, int argc, char *argv[]) 
{
  MLAdapt *cbmP = (MLAdapt*)clientData;
  int      ac   =  argc-1;
  int     covar = 0;

  if ( itfParseArgv( argv[0], &ac, argv+1, 0,
	"-covar", ARGV_INT, NULL, &covar, NULL, "restore (diagonal) covariances",
		     NULL) != TCL_OK) return TCL_ERROR;

  return mladaptRestoreAccu(cbmP,covar);
}

/* ------------------------------------------------------------------------
    mladaptCluster    grows a clustering tree for the adaptation procedure
                      by using the accus in associated codebooks to
                      determine single gaussian codebooks and successively
                      splitting these gaussian along the direction of 
                      maximum variance.
   ------------------------------------------------------------------------ */

int mladaptCluster( MLAdapt* cbmP, int maxDepth, 
                    int kmeansN, float kmeansBetaS, float kmeansBetaF)
{
  if (! cbmP->itemN)   return 0;
  if (  cbmP->nodeP) { mladaptNodeFree( cbmP->nodeP); cbmP->nodeP = NULL; }

  cbmP->nodeP  = mladaptNodeCreate( 0, cbmP->itemN-1, 0);
  return mladaptNodeSplit( cbmP->nodeP, cbmP, 0, maxDepth, kmeansN,
                                                           kmeansBetaS,
                                                           kmeansBetaF);
}

static int mladaptClusterItf (ClientData clientData, int argc, char *argv[]) 
{
  MLAdapt *cbmP        = (MLAdapt*)clientData;
  int      kmeansN     = 10;
  float    kmeansBetaS =  1.0;
  float    kmeansBetaF =  0.01;
  int      maxDepth    =  5;
  int      ac          =  argc-1;

  if ( itfParseArgv (argv[0], &ac, argv+1, 0,
      "-depth",   ARGV_INT,   NULL, &maxDepth,    NULL, "maximum depth of tree",
      "-maxIter", ARGV_INT,   NULL, &kmeansN,     NULL, "number of iterations",
      "-tempS",   ARGV_FLOAT, NULL, &kmeansBetaS, NULL, "start temperature (0=k-means)",
      "-tempF",   ARGV_FLOAT, NULL, &kmeansBetaF, NULL, "temperature decay",
      NULL) != TCL_OK) return TCL_ERROR;

  itfAppendResult( " %d",  mladaptCluster( cbmP, maxDepth, kmeansN,
                                                           kmeansBetaS,
                                                           kmeansBetaF));
  return TCL_OK;
}

/* ------------------------------------------------------------------------
    mladaptAdaptCBMean  computes the optimal linear shift of the
                        codevectors which are in the item array from adaptF
                        to adaptL. The transformation will be based on the
                        accus from all codevectors within the range refF to
                        refL. 
   ------------------------------------------------------------------------ */

DMatrix* mladaptAdaptCBMean( MLAdapt* cbmP, char* refA, int refN)
{
  int       dimN  = cbmP->dimN;
  double    gamma = 0;
  DVector*  f     = NULL;
  DMatrix*  w     = NULL;
  int       r, dimX;

  f =  dvectorCreate( dimN);
  w =  dmatrixCreate( dimN,  dimN+1);

  /* ------------------------------------------------------- */
  /* collect the information with the different accumulators */
  /* ------------------------------------------------------- */

  for ( r = 0; r < refN; r++) {
    Codebook*     cP    = cbmP->cbsP->list.itemA + cbmP->itemA[r].cbX;
    int           refX  = cbmP->itemA[r].refX;
    CodebookAccu* caP   = cP->accu;
    double        count = 0.0;

    if ( TESTBIT( refA, r) && caP && caP->count) {
      int   subX;
      for ( subX = 0; subX < caP->subN; subX++) {
        double x = caP->count[subX][refX];
        if ( x > 0.0) {
          for ( dimX = 0; dimX < cbmP->dimN; dimX++) {
            double y = caP->rv[subX]->matPA[refX][dimX];

            f->vecA[dimX] += y;
          }
          count += x;
        }
      }
      if ( count > 0) {
        for ( dimX = 0; dimX < cbmP->dimN; dimX++) {
          f->vecA[dimX] -= count * cP->rv->matPA[refX][dimX];
        }
        gamma += count;
      }
    }
  }

  /* -------------------------- */
  /* compute the transformation */
  /* -------------------------- */

  for ( dimX = 0; dimX < dimN; dimX++) {
    int j;
    w->matPA[dimX][0] = f->vecA[dimX] / gamma;
  
    for ( j = 1; j <= dimN; j++)
      w->matPA[dimX][j] = ( j == dimX+1) ? 1.0 : 0.0;
  }

  if ( f) dvectorFree( f);
  return w;
}

/* ------------------------------------------------------------------------
    mladaptAdaptCBDiag computes the optimal linear shift/scaling of the
                        codevectors which are in the item array from adaptF
                        to adaptL. The transformation will be based on the
                        accus from all codevectors within the range refF to
                        refL. 
   ------------------------------------------------------------------------ */

DMatrix* mladaptAdaptCBDiag( MLAdapt* cbmP, char* refA, int refN)
{
  int       dimN  = cbmP->dimN;
  double    gamma = 0;
  DVector*  f     = NULL;
  DMatrix*  g     = NULL;
  DMatrix*  w     = NULL;
  DVector*  wv    = NULL;
  int       i, r, dimX;

  f  =  dvectorCreate( 2 * dimN);
  wv =  dvectorCreate( 2 * dimN);
  g  =  dmatrixCreate( 2 * dimN,  2 * dimN);

  w  =  dmatrixCreate( dimN,  dimN+1);

  /* ------------------------------------------------------- */
  /* collect the information with the different accumulators */
  /* ------------------------------------------------------- */

  for ( r = 0; r < refN; r++) {
    Codebook*     cP    = cbmP->cbsP->list.itemA + cbmP->itemA[r].cbX;
    int           refX  = cbmP->itemA[r].refX;
    CodebookAccu* caP   = cP->accu;
    double        count = 0.0;

    if ( TESTBIT( refA, r) && caP && caP->count) {
      int   subX;
      for ( subX = 0; subX < caP->subN; subX++) {
        double x = caP->count[subX][refX];
        if ( x > 0.0) {
          for ( dimX = 0; dimX < dimN; dimX++) {
            double s = cP->cv[refX]->m.d[dimX];
            double y = caP->rv[subX]->matPA[refX][dimX];

            f->vecA[dimX]      += s * y;
            f->vecA[dimX+dimN] += s * cP->rv->matPA[refX][dimX] * y;
          }
          count += x;
        }
      }
      if ( count > 0) {
        for ( dimX = 0; dimX < dimN; dimX++) {
          g->matPA[dimX][dimX]           += count * cP->cv[refX]->m.d[dimX];
          g->matPA[dimX][dimX+dimN]      += count * cP->cv[refX]->m.d[dimX]  * 
                                                    cP->rv->matPA[refX][dimX];
          g->matPA[dimX+dimN][dimX]       = g->matPA[dimX][dimX+dimN];
          g->matPA[dimX+dimN][dimX+dimN] += count * cP->cv[refX]->m.d[dimX]  * 
                                                    cP->rv->matPA[refX][dimX]*
                                                    cP->rv->matPA[refX][dimX];
        }
        gamma += count;
      }
    }
  }

  /* -------------------------- */
  /* compute the transformation */
  /* -------------------------- */

  if ( g) {
    DMatrix* u =  dmatrixCreate( 2*dimN,2*dimN);
    DMatrix* v =  dmatrixCreate( 2*dimN,2*dimN);
    DVector* l =  dvectorCreate( 2*dimN);
    int      j;

    for ( j = 0; j < 2*dimN; j++)
       for ( i = 0; i < 2*dimN; i++)
         u->matPA[i][j] = u->matPA[j][i] = g->matPA[i][j];

    dsvdcmp(  u->matPA, 2*dimN, 2*dimN, l->vecA, v->matPA);
    dsvclean( l->vecA,  2*dimN);
    dsvbksb(  u->matPA, l->vecA, v->matPA, 2*dimN, 2*dimN,
              f->vecA, wv->vecA);

    if ( u) dmatrixFree( u);
    if ( v) dmatrixFree( v);
    if ( l) dvectorFree( l);
  }

  if ( g) dmatrixFree( g);

  for ( dimX = 0; dimX < dimN; dimX++) {
    int j;
    w->matPA[dimX][0] = wv->vecA[dimX];
  
    for ( j = 1; j <= dimN; j++)
      w->matPA[dimX][j] = ( j == dimX+1) ? wv->vecA[dimX+dimN] : 0.0;
  }

  if ( f)  dvectorFree( f);
  if ( wv) dvectorFree( wv);
  return w;
}

/* ------------------------------------------------------------------------
    mladaptAdaptCBFull computes the optimal linear transformation of the
                        codevectors which are in the item array from adaptF
                        to adaptL. The transformation will be based on the
                        accus from all codevectors within the range refF to
                        refL. 
   ------------------------------------------------------------------------ */

/* ------------------------------------------------------------------------
    Threads
   ------------------------------------------------------------------------ */

#ifdef MTHREAD

static pthread_attr_t pthreadAttribute;
static pthread_t mlaCollectTID      [NUM_THREADS];
static sem_t     mlaCollectBeginSem [NUM_THREADS];
static sem_t     mlaCollectReadySem [NUM_THREADS];
static int       mlaCollectIDS      [NUM_THREADS];

typedef struct {
  MLAdapt* cbmP;
  DMatrix*  z;
  DMatrix*  w;
  DMatrix** g;
  char*   refA;
  int     refN;

} mlaThreadCollectInfo;

static mlaThreadCollectInfo mlaCollectInfo;

void* collectThread( void* arg) 
{
  int* tidP = (int*) arg;
  int   tid = *tidP;

  MLAdapt* cbmP = NULL;
  char*    refA = NULL;
  int      refN = 0;
  DMatrix*  z   = NULL;
  DMatrix*  w   = NULL;
  DMatrix** g   = NULL;
  int r,dimStart,dimEnd;

  while(1) {
    sem_wait( &mlaCollectBeginSem[tid]);
    cbmP = mlaCollectInfo.cbmP;
    z    = mlaCollectInfo.z;
    w    = mlaCollectInfo.w;
    g    = mlaCollectInfo.g;
    refA = mlaCollectInfo.refA;
    refN = mlaCollectInfo.refN;

    dimStart = cbmP->dimN * tid       / NUM_THREADS;
    dimEnd   = cbmP->dimN * (tid +1)  / NUM_THREADS;

    for ( r = 0; r < refN; r++) {
      Codebook*     cP   = cbmP->cbsP->list.itemA + cbmP->itemA[r].cbX;
      CodebookAccu* caP  = cP->accu;
      int           refX = cbmP->itemA[r].refX;
      float        *rv   = cP->rv->matPA[refX];
      float        *cv   = cP->cv[refX]->m.d;
      int i,dimX,subX;

      if (! TESTBIT( refA, r) || ! caP || ! caP->count) { continue; }

      for ( subX = 0; subX < caP->subN; subX++) {
	double  x  = caP->count[subX][refX];
	double *yV = caP->rv[subX]->matPA[refX];
	if ( x > 0.0) {
	  for ( dimX = dimStart; dimX < dimEnd; dimX++) {
	    double  *zV = z->matPA[dimX];
	    double **gM = g[dimX]->matPA;
	    
	    double   s = cv[dimX];
	    double   y = yV[dimX];
	    double  sy = s*y;
	    int      j;
	    zV[0]    += sy;
	    gM[0][0] += s * x;
	    
	    for ( j = 0; j < cbmP->dimN; j++) {
	      register double tmp =  s * x * rv[j];
	      zV[j+1]    += sy * rv[j];
	      gM[j+1][0] += tmp;
	      
	      for ( i = 0; i <= j; i++)
		gM[j+1][i+1] += tmp * rv[i];
	    }
	  }
	}
      }
    }
    sem_post(&mlaCollectReadySem[tid]);
  }
  return NULL;
}

static void mlaThreadCollectInit() 
{
  int i;
  pthread_attr_init(     &pthreadAttribute);
  pthread_attr_setscope( &pthreadAttribute, PTHREAD_SCOPE_SYSTEM);

  for ( i = 0; i < NUM_THREADS; i++) {   
     mlaCollectIDS[i] = i;
     sem_init( &mlaCollectBeginSem[i], 0, 0);
     sem_init( &mlaCollectReadySem[i], 0, 0);

     /* START FF HACK - This might also crash under Windows (see scoreA) */
     /* ORIGINAL: pthread_create( &mlaCollectTID[i], &pthreadAttribute, 
		     collectThread, &mlaCollectIDS+i);*/
     pthread_create( &mlaCollectTID[i], &pthreadAttribute, 
		     collectThread, &(mlaCollectIDS[i]));
     /* END FF HACK - This might also crash under Windows (see scoreA) */
  }
}


#endif



DMatrix* mladaptAdaptCBFull( MLAdapt* cbmP, char* refA, int refN)
{
  int       dimN = cbmP->dimN;
  DMatrix*  z    = NULL;
  DMatrix*  w    = NULL;
  DMatrix** g    = NULL;
  int  threadUse = 0;
  int  i,r,dimX;

#ifdef MTHREAD
  threadUse = 1;
#else 
  threadUse = 0;
#endif
  if (! cbmP->mem2P) threadUse = 0;

  z =  dmatrixCreate( dimN,  dimN+1);
  w =  dmatrixCreate( dimN,  dimN+1);
  g = (DMatrix**)malloc( dimN * sizeof(DMatrix*));

  assert( g);

  for ( i = 0; i < dimN; i++) 
    g[i] = dmatrixCreate( dimN+1, dimN+1);

  /* ------------------------------------------------------- */
  /* collect the information with the different accumulators */
  /* ------------------------------------------------------- */

  if (threadUse) {
#ifdef MTHREAD
    mlaCollectInfo.cbmP = cbmP;
    mlaCollectInfo.z    = z;
    mlaCollectInfo.w    = w;
    mlaCollectInfo.g    = g;
    mlaCollectInfo.refA = refA;
    mlaCollectInfo.refN = refN;
    for ( i = 0; i < NUM_THREADS; i++) sem_post( &mlaCollectBeginSem[i]);
    for ( i = 0; i < NUM_THREADS; i++) sem_wait( &mlaCollectReadySem[i]);
#endif
  } else {
    for ( r = 0; r < refN; r++) {
      Codebook*     cP   = cbmP->cbsP->list.itemA + cbmP->itemA[r].cbX;
      int           refX = cbmP->itemA[r].refX;
      float        *rv   = cP->rv->matPA[refX];
      float        *cv   = cP->cv[refX]->m.d;
      
      CodebookAccu* caP  = cP->accu;
    
      if ( TESTBIT( refA, r) && caP && caP->count) {
	int   subX;
	for ( subX = 0; subX < caP->subN; subX++) {
	  double  x  = caP->count[subX][refX];
	  double *yV = caP->rv[subX]->matPA[refX];
	  int dimY;
	  
	  if ( x > 0.0) {
	    for ( dimY = 0; dimY < dimN; dimY++) {
	      double  *zV = z->matPA[dimY];
	      double **gM = g[dimY]->matPA;
	      
	      double   s = cv[dimY];
	      double   y = yV[dimY];
	      double  sy = s*y;
	      int      j;
	      zV[0]    += sy;
	      gM[0][0] += s * x;
	      
	      for ( j = 0; j < dimN; j++) {
		register double tmp =  s * x * rv[j];
		zV[j+1]    += sy * rv[j];
		gM[j+1][0] += tmp;
		
		for ( i = 0; i <= j; i++)
		  gM[j+1][i+1] += tmp * rv[i];
	      }
	    }
	  }
	}
      }
    }
  }

  /* -------------------------- */
  /* compute the transformation */
  /* -------------------------- */

  for ( dimX = 0; dimX < cbmP->dimN; dimX++) {
    DMatrix* u =  dmatrixCreate( dimN+1,dimN+1);
    DMatrix* v =  dmatrixCreate( dimN+1,dimN+1);
    DVector* l =  dvectorCreate( dimN+1);
    int      j;

    for ( j = 0; j <= dimN; j++)
       for ( i = 0; i <= j; i++)
         u->matPA[i][j] = u->matPA[j][i] = g[dimX]->matPA[i][j] =
                                           g[dimX]->matPA[j][i];

    dsvdcmp(  u->matPA, dimN+1, dimN+1, l->vecA, v->matPA);
    dsvclean( l->vecA,  dimN+1);
    dsvbksb(  u->matPA, l->vecA, v->matPA, dimN+1, dimN+1,
              z->matPA[dimX], w->matPA[dimX]);

    if ( u) dmatrixFree( u);
    if ( v) dmatrixFree( v);
    if ( l) dvectorFree( l);
  }

  if ( z) dmatrixFree( z);

  if ( g) {
    for ( i = 0; i < dimN; i++) dmatrixFree( g[i]);
    free( g);
  }

  return w;
}

/* ------------------------------------------------------------------------
    mladaptAdaptCB computes the optimal linear transformation of the
                    codevectors which are in the item array from adaptF
                    to adaptL. The transformation will be based on the
                    accus from all codevectors within the range refF to
                    refL. 
   ------------------------------------------------------------------------ */

int mladaptAdaptCB( MLAdapt* cbmP, char* refA, char* idxA, int idxN)
{
  int       dimN  = cbmP->dimN;
  DMatrix*  w     = NULL;
  DVector*  f     = NULL;
  int       i, r, dimX;

  /* ---------------------- */
  /* compute transformation */
  /* ---------------------- */

  switch ( cbmP->mode) {
  case 0:   w = mladaptAdaptCBMean( cbmP, refA, idxN);
            break;
  case 1:   w = mladaptAdaptCBDiag( cbmP, refA, idxN);
            break;
  default:  w = mladaptAdaptCBFull( cbmP, refA, idxN);
  }

  /* ------------------- */
  /* save transformation */
  /* ------------------- */

  if (  cbmP->wX >= cbmP->wN) {
    cbmP->wA = (DMatrix**)realloc( cbmP->wA,(cbmP->wN + 10)*sizeof(DMatrix*));
    for ( i = cbmP->wN; i < cbmP->wN + 10; i++) cbmP->wA[i] = NULL;
    cbmP->wN += 10;
  }
  cbmP->wA[cbmP->wX] = w;

  /* ----------------------- */
  /* transform the codebooks */
  /* ----------------------- */

  f =  dvectorCreate( dimN+1);

  for ( r = 0; r < idxN; r++) {
    Codebook*     cP   = cbmP->cbsP->list.itemA + cbmP->itemA[r].cbX;
    int           refX = cbmP->itemA[r].refX;

    if ( TESTBIT( idxA, r)) {
      cbmP->itemA[r].wX  = cbmP->wX;
      f->vecA[0]         = 1;

      for ( dimX = 0; dimX < dimN; dimX++) 
        f->vecA[dimX+1] = cP->rv->matPA[refX][dimX];

      for ( dimX = 0; dimX < dimN; dimX++) {
        double d = 0;
        for ( i = 0; i <= dimN; i++) d += f->vecA[i] * w->matPA[dimX][i];
        cP->rv->matPA[refX][dimX] = d;
      }
    }
  }

  if ( f) dvectorFree( f);

  return cbmP->wX++;
}

/* ------------------------------------------------------------------------
    mladaptNodeAdapt   traverse the tree in a recursive way and find
                       the adaptation transformations, assign them to
                       the nodes. 
   ------------------------------------------------------------------------ */

int mladaptNodeAdapt( MLAdaptNode* mlanP, MLAdapt* cbmP, 
                      char* idxA,  int idxN,
                      float minCount)
{
  assert( mlanP);
  assert( idxA);

  if (! mlanP->leftP || ! mlanP->rightP) {
    double count = 0;
    int    i;

    /* -------------------------------- */
    /* count gamma and copy index array */
    /* -------------------------------- */

    for ( i = 0; i < mlanP->idxN; i++) {
      if ( TESTBIT( mlanP->idxA, i)) {
        Codebook*     cP   = cbmP->cbsP->list.itemA + cbmP->itemA[i].cbX;
        int           refX = cbmP->itemA[i].refX;
        CodebookAccu* caP  = cP->accu;

        if ( caP && caP->count) {
          int   subX;
          for ( subX = 0; subX < caP->subN; subX++) 
             count += caP->count[subX][refX];
        }
        SETBIT( idxA, i);
        cbmP->itemA[i].count  = count;
      }
      mlanP->count  = count;
    }

    /* ---------------------------------------- */
    /* adapt if sufficient counts are available */
    /* ---------------------------------------- */

    if ( mlanP->count > minCount) 
         mlanP->w =  mladaptAdaptCB( cbmP, idxA, idxA, idxN);
    else mlanP->w = -1;
  }
  else {
    int   w[2] = { -1, -1};
    char* idxB =    NULL;
    int   i;

    idxB = (char*)malloc( sizeof(char) * (idxN / 8 + 1));

    assert( idxB);

    memset( idxB, 0,      sizeof(char) * (idxN / 8 + 1));

    w[0] = mladaptNodeAdapt( mlanP->leftP,  cbmP, idxA, idxN, minCount);
    w[1] = mladaptNodeAdapt( mlanP->rightP, cbmP, idxB, idxN, minCount);

    mlanP->count = mlanP->leftP->count + mlanP->rightP->count;

    if (    w[0] < 0 && w[1] < 0) {

      /* --------------------------------------- */
      /* none of the child nodes had enough data */
      /* --------------------------------------- */

      for ( i = 0; i < (idxN/8+1); i++) idxA[i] |= idxB[i];

      if ( mlanP->count > minCount) 
           mlanP->w =  mladaptAdaptCB( cbmP, idxA, idxA, idxN);
      else mlanP->w = -1;
    }
    else if ( w[0] < 0) {

      /* ------------------------------------------------- */
      /* right child adapted, adapt left with data of both */
      /* ------------------------------------------------- */

      for ( i = 0; i < (idxN/8+1); i++) idxB[i] |= idxA[i];

      mlanP->w =  mladaptAdaptCB( cbmP, idxB, idxA, idxN);

      for ( i = 0; i < (idxN/8+1); i++) idxA[i]  = idxB[i];
    }
    else if ( w[1] < 0) {

      /* ------------------------------------------------- */
      /* left child adapted, adapt right with data of both */
      /* ------------------------------------------------- */

      for ( i = 0; i < (idxN/8+1); i++) idxA[i] |= idxB[i];

      mlanP->w =  mladaptAdaptCB( cbmP, idxA, idxB, idxN);
    }
    else {

      /* ----------------------------------- */
      /* adaptation on that path is complete */
      /* ----------------------------------- */

      for ( i = 0; i < (idxN/8+1); i++) idxA[i] |= idxB[i];

      mlanP->w = (mlanP->leftP->count < mlanP->rightP->count) ? w[1] : w[0];
    } 
    if ( idxB) free(idxB);
  }
  return mlanP->w;
}

/* ------------------------------------------------------------------------
    mladaptUpdateCV  update covariance
   ------------------------------------------------------------------------ */

int mladaptUpdateCV( MLAdapt* cbmP, int refF,   int refL, 
                                    int adaptF, int adaptL)

{
  int       dimN = cbmP->dimN;
  DVector*  f    = NULL;
  int       r, rl, dimX;
  double    count= 0.0;

  if ( refL-refF < 0 || adaptL-adaptF < 0) return -1;

  f =  dvectorCreate( dimN+1);

  /* collect the information with the different accumulators */

  for ( r = refF, rl = refL; r <= rl; r++) {
    Codebook*     cP   = cbmP->cbsP->list.itemA + cbmP->itemA[r].cbX;
    int           refX = cbmP->itemA[r].refX;
    CodebookAccu* caP  = cP->accu;

    if ( caP && caP->count) {
      double x = 0; 
      int    subX;

      for ( subX = 0; subX < caP->subN;  subX++) x += caP->count[subX][refX];
      if ( x > 0.0) {
        for ( dimX = 0; dimX < cbmP->dimN; dimX++) {
          double s = cP->cv[refX]->m.d[dimX];
          double m = cP->rv->matPA[refX][dimX];
          double c = 0;
          double y = 0;

          for (  subX = 0; subX < caP->subN; subX++) {
            y += caP->rv[subX]->matPA[refX][dimX];
            c += caP->sumOsq[subX][refX].m.d[dimX];
          }
          f->vecA[dimX] += (c - 2*m*y + m*m*x) * s;
        }
        count += x;
      }
    }
  }

  /* transform the codebooks */

  for ( r = adaptF, rl = adaptL; r <= rl; r++) {
    Codebook*     cP   = cbmP->cbsP->list.itemA + cbmP->itemA[r].cbX;
    int           refX = cbmP->itemA[r].refX;

    cP->cv[refX]->det = 0.0;

    for ( dimX = 0; dimX < cbmP->dimN; dimX++) {
      double val = (f->vecA[dimX]/count) / cP->cv[refX]->m.d[dimX];
      if ( val <= COV_FLOOR) val = COV_FLOOR;

      cP->cv[refX]->m.d[dimX]  =  1.0 / val;
      cP->cv[refX]->det       +=  log(val);
    }
  }

  if ( f) dvectorFree( f);
  return TCL_OK;
}

/* ------------------------------------------------------------------------
    mladaptNodeVariance
   ------------------------------------------------------------------------ */

int mladaptNodeVariance( MLAdaptNode* mlanP, MLAdapt* cbmP, float minCount)
{
  assert( mlanP);

  if (! mlanP->leftP || ! mlanP->rightP) {
    mladaptUpdateCV( cbmP, mlanP->refF, mlanP->refL,
                           mlanP->refF, mlanP->refL);
    return 1;
  }
  else {
    double lcount = mlanP->leftP->count;
    double rcount = mlanP->rightP->count;

    if ( lcount < minCount && rcount < minCount) {
      mladaptUpdateCV( cbmP, mlanP->refF, mlanP->refL, 
                             mlanP->refF, mlanP->refL);
      return 1;
    }
    else if ( lcount < minCount) {
      mladaptUpdateCV( cbmP, mlanP->refF,        mlanP->refL, 
                             mlanP->leftP->refF, mlanP->leftP->refL);
      return mladaptNodeVariance( mlanP->rightP, cbmP, minCount) + 1;
    }
    else if ( rcount < minCount) {
      mladaptUpdateCV( cbmP, mlanP->refF,         mlanP->refL, 
                             mlanP->rightP->refF, mlanP->rightP->refL);
      return mladaptNodeVariance( mlanP->leftP, cbmP, minCount) + 1;
    }
    else {
      return mladaptNodeVariance( mlanP->leftP,  cbmP, minCount) + 
             mladaptNodeVariance( mlanP->rightP, cbmP, minCount);
    }
  }
}


/* ------------------------------------------------------------------------
    mladaptUpdate  moves the codevectors such that the likelihood of the
                   data as observed in the accumulation process will be
                   increased.
   ------------------------------------------------------------------------ */

int mladaptUpdate( MLAdapt* cbmP, float minCount)
{
  /* --------------------------------- */
  /* Reset old transformation matrices */
  /* --------------------------------- */

  if (  cbmP->wA) {
    int i;
    for ( i = 0; i < cbmP->wN; i++) {
      if ( cbmP->wA[i]) { dmatrixFree( cbmP->wA[i]); cbmP->wA[i] = NULL; }
    }

    for ( i = 0; i < cbmP->itemN; i++) {
      MLAdaptItem* itemP = cbmP->itemA + i;
      itemP->wX = -1;
    }
    cbmP->wX = 0;
  }

  if (! cbmP->itemN) return 0;
  if (! cbmP->nodeP) {
    ERROR("There is no clustering tree for MLAdapt.\n");
    return 0;
  }
  else {
    char* idxA =    NULL;
    int   idxN =    cbmP->itemN;

    idxA = (char*)malloc( sizeof(char) * (idxN/8+1));

    assert( idxA);

    memset( idxA, 0,      sizeof(char) * (idxN/8+1));

    mladaptNodeAdapt( cbmP->nodeP, cbmP, idxA, idxN, minCount);

    if ( idxA) { free( idxA); }
  }
  return cbmP->wX;
}

static int mladaptUpdateItf (ClientData clientData, int argc, char *argv[]) 
{
  MLAdapt *cbmP     = (MLAdapt*)clientData;
  float    minCount =  1000.0;
  int      ac       =  argc-1;

  if ( itfParseArgv (argv[0], &ac, argv+1, 0,
      "-minCount", ARGV_FLOAT, NULL, &minCount, NULL,"minimal splitting count",
       NULL) != TCL_OK) return TCL_ERROR;

  itfAppendResult( " %d",  mladaptUpdate( cbmP, minCount));
  return TCL_OK;
}

/* ------------------------------------------------------------------------
    mladaptVariance    adapt variance of the codebooks
   ------------------------------------------------------------------------ */

int mladaptVariance( MLAdapt* cbmP, float minCount, float minAdapt)
{
  float         gamma = 0;
  int           updN  = 0;

  if (! cbmP->itemN) return 0;
  if (! cbmP->nodeP) {
    ERROR("There is no clustering tree for MLAdapt.\n");
    return 0;
  }

  if (gamma < minAdapt) return 0;
  updN  =  mladaptNodeVariance( cbmP->nodeP, cbmP, minCount);

  return updN;
}

static int mladaptVarianceItf (ClientData clientData, int argc, char *argv[]) 
{
  MLAdapt *cbmP     = (MLAdapt*)clientData;
  float    minCount =  1000.0;
  float    minAdapt =     0.0;
  int      ac       =  argc-1;

  if ( itfParseArgv (argv[0], &ac, argv+1, 0,
      "-minCount", ARGV_FLOAT, NULL, &minCount, NULL,"minimal splitting count",
      "-minAdapt", ARGV_FLOAT, NULL, &minAdapt, NULL,"minimal total count for adaptation",
       NULL) != TCL_OK) return TCL_ERROR;

  itfAppendResult( " %d",  mladaptVariance( cbmP, minCount, minAdapt));
  return TCL_OK;
}


/* ========================================================================
    Speaker Adaptive Training
   ======================================================================== */

/* ------------------------------------------------------------------------
    mladaptSATClear   clear all SAT accus by deallocating them
   ------------------------------------------------------------------------ */

int mladaptSATClear( MLAdapt* cbmP)
{
  int      i;

  for ( i = 0; i < cbmP->itemN; i++) {
    MLAdaptItem*  itemP = cbmP->itemA + i;

    if (! cbmP->memP) {
      if ( itemP->muAccuV) fvectorFree( itemP->muAccuV); 
      if ( itemP->muCovAV) fvectorFree( itemP->muCovAV); 
      if ( itemP->muAccuM) fmatrixFree( itemP->muAccuM); 
    } 
    itemP->muAccuV = NULL;
    itemP->muCovAV = NULL;
    itemP->muAccuM = NULL;
    itemP->muGamma = 0.0;
  }
  if (cbmP->memP) 
    bmemClear(cbmP->memP);

  return TCL_OK;  
}

static int mladaptSATClearItf (ClientData clientData, int   argc, 
                                                      char* argv[]) 
{
  int ac=argc-1;

  if ( itfParseArgv( argv[0], &ac, argv+1, 0,
       NULL) != TCL_OK) return TCL_ERROR;

  return mladaptSATClear((MLAdapt*)clientData); 
}

/* ------------------------------------------------------------------------
    mladaptSATSave  save accus to an accumulator file
   ------------------------------------------------------------------------ */

int mladaptSATSave( MLAdapt* cbmP, char* fileName)
{
  FVector* muAccuV = NULL;
  FMatrix* muAccuM = NULL;
  FVector* muCovAV = NULL;
  FILE*    fp;
  int      i,k;
  int      dimN =  cbmP->dimN;
  int      l    = (cbmP->mode < 2) ? 1 : dimN;

  if ((fp=fileOpen(fileName,"w"))==NULL) { 
    ERROR("Can't open file %s for writing.\n",fileName); 
    return TCL_ERROR;
  }

  write_int( fp, cbmP->itemN);

  if (! muAccuV) muAccuV = fvectorCreate( dimN);
  if (! muCovAV) muCovAV = fvectorCreate( dimN);
  if (! muAccuM) muAccuM = fmatrixCreate( dimN, dimN);

  for ( i = 0; i < cbmP->itemN; i++) {
    MLAdaptItem*  itemP = cbmP->itemA + i;

    write_int(    fp, 0);
    write_float(  fp, itemP->muGamma);

    if ( itemP->muAccuV)
         write_floats( fp, itemP->muAccuV->vecA, dimN);
    else write_floats( fp,        muAccuV->vecA, dimN);

    if ( itemP->muCovAV)
         write_floats( fp, itemP->muCovAV->vecA, dimN);
    else write_floats( fp,        muCovAV->vecA, dimN);

    for ( k = 0; k < dimN; k++) {

      if ( itemP->muAccuM)
           write_floats( fp, itemP->muAccuM->matPA[k], l);
      else write_floats( fp,        muAccuM->matPA[k], l);
    }
  }

  if ( muAccuV) { fvectorFree( muAccuV); muAccuV = NULL; }
  if ( muCovAV) { fvectorFree( muCovAV); muCovAV = NULL; }
  if ( muAccuM) { fmatrixFree( muAccuM); muAccuM = NULL; }

  fileClose(fileName, fp);
  return TCL_OK;  
}

static int mladaptSATSaveItf (ClientData clientData, int   argc, 
                                                     char* argv[]) 
{
  char *filename = NULL; int ac=argc-1;

  if ( itfParseArgv( argv[0], &ac, argv+1, 0,
      "<filename>", ARGV_STRING, NULL, &filename, clientData, 
      "file to save accumulators",
       NULL) != TCL_OK) return TCL_ERROR;

  return mladaptSATSave((MLAdapt*)clientData, filename); 
}

/* ------------------------------------------------------------------------
    mladaptSATLoad   load accu file to memory
   ------------------------------------------------------------------------ */

int mladaptSATLoad( MLAdapt* cbmP, char* fileName)
{
  FVector* muAccuV = NULL;
  FMatrix* muAccuM = NULL;
  FVector* muCovAV = NULL;
  float    muGamma;
  FILE*    fp;
  int      i,j,k;
  int      dimN =  cbmP->dimN;
  int      l    = (cbmP->mode < 2) ? 1 : dimN;

  if ((fp=fileOpen(fileName,"r"))==NULL) { 
    ERROR("Can't open file %s for reading.\n",fileName); 
    return TCL_ERROR;
  }
  if (  cbmP->itemN != read_int( fp)) {
    ERROR("Accu %s doesn't match to MLAdapt %s.", fileName, cbmP->name);
    fileClose(fileName, fp);
    return TCL_ERROR;
  }

  if (! muAccuV) muAccuV = fvectorCreate( dimN);
  if (! muCovAV) muCovAV = fvectorCreate( dimN);
  if (! muAccuM) muAccuM = fmatrixCreate( dimN, l);

  for ( i = 0; i < cbmP->itemN; i++) {
    MLAdaptItem*  itemP = cbmP->itemA + i;

    read_int(    fp);
    muGamma = read_float(  fp);

    read_floats( fp, muAccuV->vecA, dimN);
    read_floats( fp, muCovAV->vecA, dimN);
    for ( k = 0; k < dimN; k++)
       read_floats( fp, muAccuM->matPA[k], l);

    if ( muGamma > 0) {
      if (cbmP->memP) {
	if (! itemP->muAccuV) itemP->muAccuV = fvectorCreate_BMEM( dimN,    cbmP->memP);
	if (! itemP->muCovAV) itemP->muCovAV = fvectorCreate_BMEM( dimN,    cbmP->memP);
	if (! itemP->muAccuM) itemP->muAccuM = fmatrixCreate_BMEM( dimN, l, cbmP->memP);
      } else {
	if (! itemP->muAccuV) itemP->muAccuV = fvectorCreate( dimN);
	if (! itemP->muCovAV) itemP->muCovAV = fvectorCreate( dimN);
	if (! itemP->muAccuM) itemP->muAccuM = fmatrixCreate( dimN, l);
      }
      for ( j = 0; j < dimN; j++) {
        itemP->muAccuV->vecA[j] += muAccuV->vecA[j];
        itemP->muCovAV->vecA[j] += muCovAV->vecA[j];
        for ( k = 0; k < l; k++) {
          itemP->muAccuM->matPA[j][k] += muAccuM->matPA[j][k];
        }
      }
      itemP->muGamma += muGamma;
    }
  }

  if ( muAccuV) { fvectorFree( muAccuV); muAccuV = NULL; }
  if ( muCovAV) { fvectorFree( muCovAV); muCovAV = NULL; }
  if ( muAccuM) { fmatrixFree( muAccuM); muAccuM = NULL; }

  fileClose(fileName, fp);
  return TCL_OK;  
}

static int mladaptSATLoadItf (ClientData clientData, int   argc, 
                                                     char* argv[]) 
{
  char *filename = NULL; int ac=argc-1;

  if ( itfParseArgv( argv[0], &ac, argv+1, 0,
      "<filename>", ARGV_STRING, NULL, &filename, clientData, 
      "file accumulators",
       NULL) != TCL_OK) return TCL_ERROR;

  return mladaptSATLoad((MLAdapt*)clientData, filename); 
}

/* ------------------------------------------------------------------------
    mladaptSATAccuDiag accumulate SAT mean vector for diag. transformation
                       matrix adaptation.
   ------------------------------------------------------------------------ */

int mladaptSATAccuDiag( MLAdapt* cbmP, char* fileName)
{
  FVector* muAccuV  = NULL;
  FVector* muCovAV  = NULL;
  FMatrix* muAccuM  = NULL;
  FILE*    fp       = NULL; 
  float    muGamma;
  int      i, j, k;
  int      dimN = cbmP->dimN;

  assert (cbmP->mode < 2);

  if ( fileName) {

    if ((fp=fileOpen(fileName,"r+"))==NULL) { 
      ERROR("Can't open file %s for writing.\n",fileName); 
      return TCL_ERROR;
    }
    if (  cbmP->itemN != read_int( fp)) {
      ERROR("Accu %s doesn't match to MLAdapt %s.", fileName, cbmP->name);
      fileClose(fileName, fp);
      return TCL_ERROR;
    }
  
    if (! muAccuV) muAccuV = fvectorCreate( dimN);
    if (! muCovAV) muCovAV = fvectorCreate( dimN);
    if (! muAccuM) muAccuM = fmatrixCreate( dimN, dimN);
  } 

  for ( i = 0; i < cbmP->itemN; i++) {
    MLAdaptItem*  itemP = cbmP->itemA + i;
    Codebook*     cP    = cbmP->cbsP->list.itemA + itemP->cbX;
    int           refX  = itemP->refX;
    CodebookAccu* caP   = cP->accu;

    if ( caP && caP->count && caP->count[0][refX] > 0.0 && itemP->wX >= 0) {
      DMatrix* w     = cbmP->wA[itemP->wX];
      double   gamma = caP->count[0][refX];
      long     off   = i * (dimN * (3) * sizeof(float) +
                            sizeof(int) + sizeof(float)) + sizeof(int);
      int      dimX;
      //int      useN  = 0;

      if ( fileName) {

        /* ------------------------- */
        /* wait for write permission */
        /* ------------------------- */

        fseek( fp, off, SEEK_SET);
        while ( read_int( fp) ) { sleep(1); fseek( fp, off, SEEK_SET); }

        fseek( fp, off, SEEK_SET);
        write_int( fp, 1);

        /* --------- */
        /* read accu */
        /* --------- */

        muGamma = read_float( fp);
        read_floats( fp, muAccuV->vecA, dimN);
        read_floats( fp, muCovAV->vecA, dimN);
        for ( k = 0; k < dimN; k++)
          read_floats( fp, muAccuM->matPA[k], 1);

      } else {

        /* ----------------------------- */
        /* use locally accumulated stuff */
        /* ----------------------------- */

	if (cbmP->memP) {
	  if (! itemP->muAccuV) itemP->muAccuV = fvectorCreate_BMEM( dimN,    cbmP->memP);
	  if (! itemP->muCovAV) itemP->muCovAV = fvectorCreate_BMEM( dimN,    cbmP->memP);
	  if (! itemP->muAccuM) itemP->muAccuM = fmatrixCreate_BMEM( dimN, 1, cbmP->memP);
	} else {
	  if (! itemP->muAccuV) itemP->muAccuV = fvectorCreate( dimN);
	  if (! itemP->muCovAV) itemP->muCovAV = fvectorCreate( dimN);
	  if (! itemP->muAccuM) itemP->muAccuM = fmatrixCreate( dimN, 1);
	}

	muAccuV = itemP->muAccuV;
        muAccuM = itemP->muAccuM;
        muCovAV = itemP->muCovAV;
        muGamma = itemP->muGamma;
      }

      /* ---------- */
      /* accumulate */
      /* ---------- */

      muGamma += gamma;

      for ( dimX = 0; dimX < dimN; dimX++) {
        double x = 0;

        /* W^t C^-1 gamma ( o - b) = W^t C^-1 ( accu - gamma * b) */

        for ( j = 0; j < dimN; j++) {

          x += w->matPA[j][dimX+1] * cP->cv[refX]->m.d[dimX] *
              (caP->rv[0]->matPA[refX][j] - gamma * w->matPA[j][0]);
        }
        muAccuM->matPA[dimX][0] += gamma * cP->cv[refX]->m.d[dimX] *
                                           w->matPA[dimX][dimX+1]  *
                                           w->matPA[dimX][dimX+1];
        muAccuV->vecA[dimX]     +=     x;
        muCovAV->vecA[dimX]     +=     caP->sumOsq[0][refX].m.d[dimX] -
                                   2 * caP->rv[0]->matPA[refX][dimX] *
                                       cP->rv->matPA[refX][dimX] + gamma *
                                       cP->rv->matPA[refX][dimX] *
                                       cP->rv->matPA[refX][dimX];
      }

      if ( fileName) {

        /* ---------- */
        /* write accu */
        /* ---------- */

        fseek( fp, off+sizeof(int), SEEK_SET);

        write_float(  fp, muGamma);
        write_floats( fp, muAccuV->vecA, dimN);
        write_floats( fp, muCovAV->vecA, dimN);
        for ( k = 0; k < dimN; k++)
          write_floats( fp, muAccuM->matPA[k], 1);

        /* ----------------------- */
        /* return write permission */
        /* ----------------------- */

        fseek( fp, off, SEEK_SET);
        write_int( fp, 0);

      } else {

        itemP->muGamma = muGamma;

      }
    }
  }
 
  if ( fileName) {

    if ( muAccuV) { fvectorFree( muAccuV); muAccuV = NULL; }
    if ( muCovAV) { fvectorFree( muCovAV); muCovAV = NULL; }
    if ( muAccuM) { fmatrixFree( muAccuM); muAccuM = NULL; }

    fileClose(fileName, fp);
  }
  return TCL_OK;
}

/* ------------------------------------------------------------------------
    mladaptSATAccuFull accumulate SAT mean vector for full transformation
                       matrix adaptation.
   ------------------------------------------------------------------------ */

/* ------------------------------------------------------------------------
    Threads
   ------------------------------------------------------------------------ */

#ifdef MTHREAD

static pthread_t satCollectTID      [NUM_THREADS];
static sem_t     satCollectBeginSem [NUM_THREADS];
static sem_t     satCollectReadySem [NUM_THREADS];
static int       satCollectIDS      [NUM_THREADS];

typedef struct {
  MLAdapt* cbmP;
} satThreadCollectInfo;

static satThreadCollectInfo satCollectInfo;

void* collectSatThread( void* arg) 
{
  int*         tidP = (int*) arg;
  int           tid = *tidP;
  MLAdapt*     cbmP = NULL;
  BMem*        memP = NULL;
  FVector* muAccuV  = NULL;
  FVector* muCovAV  = NULL;
  FMatrix* muAccuM  = NULL;
  float    muGamma  = 0.0;
  int i,j,dimX,dimN;

  while(1) {
    sem_wait( &satCollectBeginSem[tid]);
    cbmP   = satCollectInfo.cbmP;   
    dimN   = cbmP->dimN;

    if (tid == 0) memP = cbmP->memP;
    if (tid == 1) memP = cbmP->mem2P;

    for ( i = tid; i < cbmP->itemN; i += NUM_THREADS) {
      MLAdaptItem*  itemP = cbmP->itemA + i;
      Codebook*     cP    = cbmP->cbsP->list.itemA + itemP->cbX;
      int           refX  = itemP->refX;
      CodebookAccu* caP   = cP->accu;

      if ( caP && caP->count && caP->count[0][refX] > 0.0 && itemP->wX >= 0) {
	DMatrix* w     = cbmP->wA[itemP->wX];
	float*   rv    = cP->rv->matPA[refX];
	float*   cv    = cP->cv[refX]->m.d;
	double*  rvA   = caP->rv[0]->matPA[refX];
	double*  cvA   = caP->sumOsq[0][refX].m.d;      
	double   gamma = caP->count[0][refX];

	if (memP) {
	  if (! itemP->muAccuV) itemP->muAccuV = fvectorCreate_BMEM( dimN,       memP);
	  if (! itemP->muCovAV) itemP->muCovAV = fvectorCreate_BMEM( dimN,       memP);
	  if (! itemP->muAccuM) itemP->muAccuM = fmatrixCreate_BMEM( dimN, dimN, memP);
	} else {
	  if (! itemP->muAccuV) itemP->muAccuV = fvectorCreate( dimN);
	  if (! itemP->muCovAV) itemP->muCovAV = fvectorCreate( dimN);
	  if (! itemP->muAccuM) itemP->muAccuM = fmatrixCreate( dimN, dimN);
	}

	muAccuV = itemP->muAccuV;
        muAccuM = itemP->muAccuM;
        muCovAV = itemP->muCovAV;
        muGamma = itemP->muGamma;

	muGamma += gamma;
	for ( dimX = 0; dimX < dimN; dimX++) {
	  int    k;
	  double x = 0;

	  for ( j = 0; j < dimN; j++) {
	    register double  y  = 0;
	    double* wV = w->matPA[j];

	    x += wV[dimX+1] * cv[dimX] * (rvA[j] - gamma * wV[0]);
	    for ( k = 0; k < dimN; k++) {
	      y += w->matPA[k][dimX+1] * w->matPA[k][j+1];
	    }
	    muAccuM->matPA[dimX][j] += gamma * cv[dimX] * y;
	  }
	  muAccuV->vecA[dimX] += x;
	  muCovAV->vecA[dimX] += cvA[dimX] + gamma * rv[dimX] * rv[dimX];
	  muCovAV->vecA[dimX] -= 2 * rvA[dimX] * rv[dimX];
	}
	itemP->muGamma = muGamma;
      }
    }
    sem_post(&satCollectReadySem[tid]);
  }
  return NULL;
}

static void satThreadCollectInit() 
{
  int i;
  for ( i = 0; i < NUM_THREADS; i++) {
     satCollectIDS[i] = i;
     sem_init( &satCollectBeginSem[i], 0, 0);
     sem_init( &satCollectReadySem[i], 0, 0);

     /* START FF HACK - This might also cause a crash under Windows */
     /* ORIGINAL: pthread_create( &satCollectTID[i], &pthreadAttribute, 
                     collectSatThread, &satCollectIDS+i);*/
     pthread_create( &satCollectTID[i], &pthreadAttribute, 
                     collectSatThread, &(satCollectIDS[i]));
     /* END FF HACK - This might also cause a crash under Windows */
  }
}

#endif

int mladaptSATAccuFull( MLAdapt* cbmP, char* fileName)
{
  FVector* muAccuV  = NULL;
  FVector* muCovAV  = NULL;
  FMatrix* muAccuM  = NULL;
  FILE*    fp       = NULL;
  int      dimN     = cbmP->dimN;
  float    muGamma  = 0.0;
  int     threadUse = 0;
  int      i,j,k;

#ifdef MTHREAD
  threadUse = 1;
#else
  threadUse = 0;
#endif
  if (! cbmP->mem2P) threadUse = 0;

  assert (cbmP->mode > 1);

  if ( fileName) {

    if ((fp=fileOpen(fileName,"r+"))==NULL) { 
      ERROR("Can't open file %s for writing.\n",fileName); 
      return TCL_ERROR;
    }
    if (  cbmP->itemN != read_int( fp)) {
      ERROR("Accu %s doesn't match to MLAdapt %s.", fileName, cbmP->name);
      fileClose(fileName, fp);
      return TCL_ERROR;
    }
  
    if (! muAccuV) muAccuV = fvectorCreate( dimN);
    if (! muCovAV) muCovAV = fvectorCreate( dimN);
    if (! muAccuM) muAccuM = fmatrixCreate( dimN, dimN);
  } 

  if (threadUse) {
#ifdef MTHREAD
    assert (fileName == NULL);
    satCollectInfo.cbmP = cbmP;
    for ( i = 0; i < NUM_THREADS; i++) sem_post( &satCollectBeginSem[i]);
    for ( i = 0; i < NUM_THREADS; i++) sem_wait( &satCollectReadySem[i]);
#endif
  } else {
    for ( i = 0; i < cbmP->itemN; i++) {
      MLAdaptItem*  itemP = cbmP->itemA + i;
      Codebook*     cP    = cbmP->cbsP->list.itemA + itemP->cbX;
      int           refX  = itemP->refX;
      CodebookAccu* caP   = cP->accu;

      if ( caP && caP->count && caP->count[0][refX] > 0.0 && itemP->wX >= 0) {
	DMatrix* w     = cbmP->wA[itemP->wX];
	float*   rv    = cP->rv->matPA[refX];
	float*   cv    = cP->cv[refX]->m.d;
	double*  rvA   = caP->rv[0]->matPA[refX];
	double*  cvA   = caP->sumOsq[0][refX].m.d;      
	double   gamma = caP->count[0][refX];
	long     off   = i * (dimN * (dimN+2) * sizeof(float) +
			      sizeof(int) + sizeof(float)) + sizeof(int);
	int      dimX;
	//int      useN  = 0;
	
	if ( fileName) {
	  
	  /* ------------------------- */
	  /* wait for write permission */
	  /* ------------------------- */
	  
	  fseek( fp, off, SEEK_SET);
	  while ( read_int( fp) ) { sleep(1); fseek( fp, off, SEEK_SET); }
	  
	  fseek( fp, off, SEEK_SET);
	  write_int( fp, 1);
	  
	  /* --------- */
	  /* read accu */
	  /* --------- */
	  
	  muGamma = read_float( fp);
	  read_floats( fp, muAccuV->vecA, dimN);
	  read_floats( fp, muCovAV->vecA, dimN);
	  for ( k = 0; k < dimN; k++)
	    read_floats( fp, muAccuM->matPA[k], dimN);
	  
	} else {
	  
	  /* ----------------------------- */
	  /* use locally accumulated stuff */
	  /* ----------------------------- */
	  
	  if (cbmP->memP) {
	    if (! itemP->muAccuV) itemP->muAccuV = fvectorCreate_BMEM( dimN,       cbmP->memP);
	    if (! itemP->muCovAV) itemP->muCovAV = fvectorCreate_BMEM( dimN,       cbmP->memP);
	    if (! itemP->muAccuM) itemP->muAccuM = fmatrixCreate_BMEM( dimN, dimN, cbmP->memP);
	  } else {
	    if (! itemP->muAccuV) itemP->muAccuV = fvectorCreate( dimN);
	    if (! itemP->muCovAV) itemP->muCovAV = fvectorCreate( dimN);
	    if (! itemP->muAccuM) itemP->muAccuM = fmatrixCreate( dimN, dimN);
	  }
	  
	  muAccuV = itemP->muAccuV;
	  muAccuM = itemP->muAccuM;
	  muCovAV = itemP->muCovAV;
	  muGamma = itemP->muGamma;
	}
	
	/* ---------- */
	/* accumulate */
	/* ---------- */
	
	muGamma += gamma;
	
	for ( dimX = 0; dimX < dimN; dimX++) {
	  int    k;
	  double x = 0;

	  /* W^t C^-1 gamma ( o - b) = W^t C^-1 ( accu - gamma * b) */
	  
	  for ( j = 0; j < dimN; j++) {
	    register double  y  = 0;
	    double* wV = w->matPA[j];
	    
	    x += wV[dimX+1] * cv[dimX] * (rvA[j] - gamma * wV[0]);
	    
	    /* W^t C^-1 W */
	    
	    for ( k = 0; k < dimN; k++) {
	      y += w->matPA[k][dimX+1] * w->matPA[k][j+1];
	    }
	    muAccuM->matPA[dimX][j] += gamma * cv[dimX] * y;
	  }
	  muAccuV->vecA[dimX] += x;
	  muCovAV->vecA[dimX] += cvA[dimX] + gamma * rv[dimX] * rv[dimX];
	  muCovAV->vecA[dimX] -= 2 * rvA[dimX] * rv[dimX];
	  
	}

	if ( fileName) {
	  
	  /* ---------- */
	  /* write accu */
	  /* ---------- */
	  
	  fseek( fp, off+sizeof(int), SEEK_SET);
	  
	  write_float(  fp, muGamma);
	  write_floats( fp, muAccuV->vecA, dimN);
	  write_floats( fp, muCovAV->vecA, dimN);
	  for ( k = 0; k < dimN; k++)
	    write_floats( fp, muAccuM->matPA[k], dimN);
	  
	  /* ----------------------- */
	  /* return write permission */
	  /* ----------------------- */
	  
	  fseek( fp, off, SEEK_SET);
	  write_int( fp, 0);
	  
	} else {
	  
	  itemP->muGamma = muGamma;
	  
	}
      }
    }
  }
 
  if ( fileName) {

    if ( muAccuV) { fvectorFree( muAccuV); muAccuV = NULL; }
    if ( muCovAV) { fvectorFree( muCovAV); muCovAV = NULL; }
    if ( muAccuM) { fmatrixFree( muAccuM); muAccuM = NULL; }

    fileClose(fileName, fp);
  }
  return TCL_OK;
}

static int mladaptSATAccuItf (ClientData clientData, int argc, char *argv[]) 
{
  MLAdapt *cbmP     = (MLAdapt*)clientData;
  int      ac       =  argc-1;
  char*    fileName = NULL;

  if ( itfParseArgv( argv[0], &ac, argv+1, 0,
      "-file", ARGV_STRING, NULL, &fileName, clientData, "SAT accu file", 
       NULL) != TCL_OK) return TCL_ERROR;

  if (cbmP->mode < 2) return mladaptSATAccuDiag( cbmP, fileName);
  else                return mladaptSATAccuFull( cbmP, fileName);
}

/* ------------------------------------------------------------------------
    mladaptSATUpdate    Accumulate SAT mean vector
   ------------------------------------------------------------------------ */

int mladaptSATUpdate( MLAdapt* cbmP, char* fileName, int updateMu, 
                                                     int updateCov)
{
  FVector* muAccuV = NULL;
  FVector* muCovAV = NULL;
  FMatrix* muAccuM = NULL;
  FILE*    fp      = NULL;
  float    muGamma;
  int      i, j, k;
  int      dimN = cbmP->dimN;

  if ( fileName) {

    /* --------------------- */
    /* open file for reading */
    /* --------------------- */

    if ((fp=fileOpen(fileName,"r"))==NULL) { 
      ERROR("Can't open file %s for reading.\n",fileName); 
      return TCL_ERROR;
    }

    if (  cbmP->itemN != read_int( fp)) {
      ERROR("Accu %s doesn't match to MLAdapt %s.", fileName, cbmP->name);
      fileClose(fileName, fp);
      return TCL_ERROR;
    }

    if (! muAccuV) muAccuV = fvectorCreate( dimN);
    if (! muCovAV) muCovAV = fvectorCreate( dimN);
    if (! muAccuM) muAccuM = fmatrixCreate( dimN, dimN);
  }

  for ( i = 0; i < cbmP->itemN; i++) {
    MLAdaptItem*  itemP = cbmP->itemA + i;
    Codebook*     cP    = cbmP->cbsP->list.itemA + itemP->cbX;
    int           refX  = itemP->refX;
    long          off   = i * (dimN * (((cbmP->mode<2) ? 1 : dimN)+2) * 
                               sizeof(float) +
                               sizeof(int) + sizeof(float)) + sizeof(int);
    //int           useN;

    if ( fileName) {

      /* ---------------------- */
      /* wait for unlocked accu */
      /* ---------------------- */

      fseek( fp, off, SEEK_SET);
      while ( read_int( fp) ) { sleep(1); fseek( fp, off, SEEK_SET); }

      /* --------- */
      /* read accu */
      /* --------- */

      if ((muGamma = read_float(fp)) > 0.0) {

        read_floats( fp, muAccuV->vecA, dimN);
        read_floats( fp, muCovAV->vecA, dimN);
        for ( k = 0; k < dimN; k++)
          read_floats( fp, muAccuM->matPA[k], (cbmP->mode<2) ? 1 : dimN);
      }
    } else {
        muAccuV = itemP->muAccuV;
        muAccuM = itemP->muAccuM;
        muCovAV = itemP->muCovAV;
        muGamma = itemP->muGamma;
    }

    if ( muGamma > 0.0 && muAccuV && muAccuM && muCovAV) {
      if ( updateMu) {

        DMatrix* u =  dmatrixCreate( dimN,dimN);
        DMatrix* v =  dmatrixCreate( dimN,dimN);
        DVector* l =  dvectorCreate( dimN);
        DVector* z =  dvectorCreate( dimN);
        DVector* b =  dvectorCreate( dimN);
        int      j, k;

        for ( j = 0; j < dimN; j++) {
          b->vecA[j] = muAccuV->vecA[j];

          if ( cbmP->mode < 2) {
            u->matPA[j][j] =  muAccuM->matPA[j][0];
          }
          else {
            for ( k = 0; k < dimN; k++)
               u->matPA[k][j] = muAccuM->matPA[k][j];
          }
        }
        dsvdcmp(  u->matPA, dimN, dimN, l->vecA, v->matPA);
        dsvclean( l->vecA,  dimN);
        dsvbksb(  u->matPA, l->vecA, v->matPA, dimN, dimN,
                  b->vecA,  z->vecA);

        for ( k = 0; k < dimN; k++) cP->rv->matPA[refX][k] = z->vecA[k];

        if ( u) dmatrixFree( u);
        if ( v) dmatrixFree( v);
        if ( l) dvectorFree( l);
        if ( z) dvectorFree( z);
        if ( b) dvectorFree( b);
      }

      /* ------------------------ */
      /* update Covariance matrix */
      /* ------------------------ */

      if ( updateCov && muGamma >= cP->cfg->minCvCount) {

        cP->cv[refX]->det = 0.0;

        for ( j = 0; j < dimN; j++) {
          double val = muCovAV->vecA[j] / muGamma;
          if ( val <= COV_FLOOR) val = COV_FLOOR;

          cP->cv[refX]->m.d[j]  =  1.0 / val;
          cP->cv[refX]->det    +=  log(val);
        }        
      }
    }
  }

  if ( fileName) {
    if ( muAccuV) { fvectorFree( muAccuV); muAccuV = NULL; }
    if ( muCovAV) { fvectorFree( muCovAV); muCovAV = NULL; }
    if ( muAccuM) { fmatrixFree( muAccuM); muAccuM = NULL; }

    fileClose(fileName, fp);
  }
  return TCL_OK;
}

static int mladaptTransform(MLAdapt *cbmP, int matrixX) {
  DMatrix *trn;
  int x,y;

  if (matrixX >= cbmP->wX) {
    ERROR("Illegal index %d (currently %d transformations available)\n", matrixX, cbmP->wX);
    return(TCL_ERROR);
  }
  trn = cbmP->wA[matrixX];
  for (x=0; x < trn->m; x++) {
    itfAppendResult("{ ");
    for (y=0; y < trn->n; y++) {
      itfAppendResult("%f ", trn->matPA[x][y]);
    }
    itfAppendResult("} ");
  }
  return TCL_OK;
}

static int mladaptTransformItf (ClientData clientData, int argc, char *argv[]) {
  MLAdapt *cbmP        = (MLAdapt*)clientData;
  int      ac          =  argc-1;
  int      matrixX     = 0;
  if ( itfParseArgv( argv[0], &ac, argv+1, 0,
      "<matrixX>",   ARGV_INT, NULL, &matrixX, clientData, "print which transformation matrix", 
        NULL) != TCL_OK) return TCL_ERROR;
  return mladaptTransform(cbmP, matrixX);
}
  
static int mladaptSATUpdateItf (ClientData clientData, int argc, char *argv[]) 
{
  MLAdapt *cbmP        = (MLAdapt*)clientData;
  int      ac          =  argc-1;
  char*    fileName    =  NULL;
  int      updateMeans =  1;
  int      updateCovs  =  1;

  if ( itfParseArgv( argv[0], &ac, argv+1, 0,
      "-file",       ARGV_STRING, NULL, &fileName, clientData, "SAT accu file", 
      "-updateMean", ARGV_INT, NULL, &updateMeans, NULL,"update means",
      "-updateCV",   ARGV_INT, NULL, &updateCovs,  NULL,"update covariances",
        NULL) != TCL_OK) return TCL_ERROR;

  return mladaptSATUpdate( cbmP, fileName, updateMeans, updateCovs);
}

/* ========================================================================
    Type Declaration
   ======================================================================== */

static Method mladaptItemMethod[] = 
{
  { "puts", mladaptItemPutsItf,   NULL },
  {  NULL,  NULL, NULL } 
};

TypeInfo mladaptItemInfo = {
        "MLAdaptItem", 0, -1, mladaptItemMethod, 
         NULL, NULL, 
         mladaptItemConfigureItf, NULL, 
         itfTypeCntlDefaultNoLink,
        "MLAdaptItem\n" };

static Method mladaptMethod[] = 
{
  { "puts",       mladaptPutsItf,       NULL },
  { "add",        mladaptAddItf,        "add items to the adaptation"},
  { "clear",      mladaptClearItf,      "remove all items from the adaptation list"},
  { "store",      mladaptStoreItf,      "save current means to MLAdapt"},
  { "restore",    mladaptRestoreItf,    "restore means from MLAdapt"},
  { "storeAccu",  mladaptStoreAccuItf,  "save accumulators of the gaussians to MLAdapt"},
  { "restoreAccu",mladaptRestoreAccuItf,"restore accumulators from MLAdapt"},
  { "cluster",    mladaptClusterItf,    "cluster items in the list"},
  { "accuTree",   mladaptTreeAccuItf,   "accu MLAdapt information for optimizing tree"},
  { "clearTree",  mladaptTreeClearItf,  "clear MLAdapt tree accus"},
  { "optTree",    mladaptTreeOptItf,    "optimize tree based on accus"},
  { "save",       mladaptTreeSaveItf,   "save MLAdapt tree/accus to file"},
  { "load",       mladaptTreeLoadItf,   "load MLAdapt tree/accus from file"},
  { "update",     mladaptUpdateItf,     "update codebook means"},
  { "transform",  mladaptTransformItf,  "print transformation matrix"},
  { "variance",   mladaptVarianceItf,   "update codebook variances"},
  { "clearSAT",   mladaptSATClearItf,   "clear SAT accus"},
  { "saveSAT",    mladaptSATSaveItf,    "save SAT accus to file"},
  { "loadSAT",    mladaptSATLoadItf,    "load SAT accus from file"},
  { "accuSAT",    mladaptSATAccuItf,    "SAT accu means"},
  { "updateSAT",  mladaptSATUpdateItf,  "SAT update codebook means"},
  {  NULL,  NULL, NULL } 
};

TypeInfo mladaptInfo = { 
        "MLAdapt", 0, -1, mladaptMethod, 
         mladaptCreateItf, mladaptFreeItf, 
         mladaptConfigureItf, mladaptAccessItf, 
         itfTypeCntlDefaultNoLink,
        "Maximum Likelihood Adaptation\n" };

static int mladaptInitialized = 0;

extern int MLNorm_Init();

int MLAdapt_Init()
{
  if ( mladaptInitialized) return TCL_OK;

  itfNewType( &mladaptItemInfo);
  itfNewType( &mladaptInfo);
  mladaptInitialized = 1;
  return TCL_OK;
}
