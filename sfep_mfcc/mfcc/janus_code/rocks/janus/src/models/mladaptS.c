/* ========================================================================
    JanusRTk   Janus Recognition Toolkit
               ------------------------------------------------------------
               Object: Maximum Likelihood Signal Adaptation
               ------------------------------------------------------------

    Author  :  Hagen Soltau
    Module  :  mladaptS.c
    Date    :  $Id: mladaptS.c 3416 2011-03-23 03:02:18Z metze $
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
    Revision 1.19  2007/03/02 13:09:49  fuegen
    bugfix in accu method, when giving frame boundaries

    Revision 1.18  2005/10/06 18:14:39  tschaaf
    merged branch janus-20051004-tschaaf

    Revision 1.17.14.1  2005/10/06 17:50:56  tschaaf
    integrated hua yu's combine function. This function allows to combine two succesive MLLT transforms into one

    Revision 1.17  2004/05/27 11:32:32  metze
    Introduced error counter in dludcmp

    Revision 1.16  2003/08/14 11:20:15  fuegen
    Merged changes on branch jtk-01-01-15-fms (jaguar -> ibis-013)

    Revision 1.15.18.1.2.11  2002/11/20 13:25:24  soltau
    Removed "slimits.h"

    Revision 1.15.18.1.2.10  2002/04/29 09:40:22  soltau
    beautified

    Revision 1.15.18.1.2.9  2002/04/02 16:31:09  metze
    Changes for streams

    Revision 1.15.18.1.2.8  2002/03/11 11:37:53  soltau
    mladaptSCompute : finite check for determinant

    Revision 1.15.18.1.2.7  2002/02/01 14:17:20  fuegen
    moved stypes.h into slimits.h

    Revision 1.15.18.1.2.6  2001/11/20 11:03:11  metze
    Made number of aX and wX an option at creation time

    Revision 1.15.18.1.2.5  2001/11/06 15:45:14  metze
    Changes for streams

    Revision 1.15.18.1.2.4  2001/10/22 09:40:11  soltau
    Fixed wrong refN in SignalAdaptInit
    Added dimN checking in SignalAdaptAdd

    Revision 1.15.18.1.2.3  2001/07/04 18:08:55  soltau
    Changes according to new compression interface

    Revision 1.15.18.1.2.2  2001/07/03 09:45:01  soltau
    Support for compressed features and codebooks

    Revision 1.15.18.1.2.1  2001/06/26 19:29:33  soltau
    beautified/fixed a couple of ugly hacker routines

    Revision 1.15.18.1  2001/01/19 12:35:46  soltau
    Relaced by my version 1.15

    Revision 1.13  2000/05/01 21:24:47  soltau
    Added SignalAdaptCompareTransform

    Revision 1.12  2000/03/22 12:48:06  soltau
    Changed interface

    Revision 1.11  2000/03/20 10:40:24  soltau
    Fixed bug in lubksb

    Revision 1.10  2000/03/17 08:17:49  soltau
    perform matrix inversion by lu decomposition

    Revision 1.9  2000/03/16 17:45:26  soltau
    made it damned fast

    Revision 1.8  2000/03/16 17:02:47  soltau
    optimized cofactor computation

    Revision 1.7  2000/03/15 15:21:28  soltau
    Added load/save store/restore routines

    Revision 1.6  2000/03/14 15:14:14  soltau
    Fixed bugs
    Add warning

    Revision 1.5  2000/03/13 17:48:55  soltau
    made it working

    Revision 1.4  2000/03/09 18:37:49  soltau
    *** empty log message ***

    Revision 1.3  1998/09/30 16:58:21  soltau
    *** empty log message ***

    Revision 1.2  1998/09/28 11:26:26  soltau
    made it working

    Revision 1.1  1998/09/25 10:25:59  soltau
    Initial revision

   ======================================================================== */

#include "common.h"
#include "itf.h"
#include "mladaptS.h"
#include "path.h"
#include "mach_ind_io.h"
#include "distribStream.h"
#include "scoreA.h"

#ifdef BLAS
#ifdef DARWIN
#include <Accelerate/Accelerate.h>
#else
#include "cblas.h"
#endif
#endif

extern TypeInfo SignalAdaptInfo;

/* ------------------------------------------------------------------------
    SignalAdaptInit
   ------------------------------------------------------------------------ */

static int SignalAdaptInit(SignalAdapt* saP,char* name, DistribSet* dssP, 
			   SenoneSet* snsP, int strX, int max_accu, int max_tran) { 

  int i,j,accuX,tranX,dssN,dimN,refN;

  saP->dssP = dssP;
  saP->cbsP = dssP->cbsP;
  saP->snsP = snsP;
  saP->fsP  = saP->cbsP->feat;
  
  dssN= saP->dssP->list.itemN;         
  dimN= saP->cbsP->list.itemA[0].dimN; 
  refN= saP->cbsP->list.itemA[0].refN; 

  saP->max_accu = max_accu;
  saP->max_tran = max_tran;

  saP->beta  = (float*)     malloc (max_accu*sizeof(float));
  saP->count = (int*)       malloc (max_accu*sizeof(int));
  saP->z     = (DMatrix**)  malloc (max_accu*sizeof(DMatrix*));
  saP->g     = (DMatrix***) malloc (max_accu*sizeof(DMatrix**));
  saP->w     = (DMatrix**)  malloc (max_tran*sizeof(DMatrix*));

  if (!saP->beta || !saP->count || !saP->z || !saP->g || !saP->w) {
    ERROR ("SignalAdaptInit: allocation failure");
    return TCL_ERROR;
  } 

  for (i=1;i<saP->cbsP->list.itemN;i++) {
    Codebook* cbP = saP->cbsP->list.itemA+i;
    if (cbP->refN > refN) refN = cbP->refN;
  }

  saP->strX = strX;
  saP->dimN = dimN;
  saP->name=   strdup( name);
  saP->useN=   0;
  saP->topN=   1;
  saP->shift=  1.0;
  saP->dsXA=  (int*) calloc(dssN,sizeof(int));
  saP->addCount= (float*) calloc(refN,sizeof(float));
  saP->gammaX= (SortElement*) malloc(refN*sizeof(SortElement));

  for (accuX=0;accuX<saP->max_accu;accuX++) {
    saP->count[accuX]=  0;
    saP->beta[accuX] =  0.0;
  }

  for (tranX=0;tranX<saP->max_tran;tranX++)
    saP->w[tranX] = dmatrixCreate(dimN,dimN+1);

  for (accuX=0;accuX<saP->max_accu;accuX++) {
    saP->z[accuX] = dmatrixCreate(dimN,dimN+1);

    if (NULL == (saP->g[accuX]= (DMatrix**) malloc(dimN*sizeof(DMatrix*)))) {
      ERROR("SignalAdaptInit: allocation failure");
      return TCL_ERROR;
    }
    for (i=0;i<dimN;i++) saP->g[accuX][i]=dmatrixCreate(dimN+1,dimN+1);
  }

  if (NULL == (saP->ginv= (DMatrix**) malloc(dimN*sizeof(DMatrix*)))) {
    ERROR("SignalAdaptInit: allocation failure");
    return TCL_ERROR;
  }
  for (i=0;i<dimN;i++) saP->ginv[i]=dmatrixCreate(dimN+1,dimN+1);

  /*Init saP->w = Identity */
  for (tranX=0;tranX<saP->max_tran;tranX++) {
    for (i=0;i<dimN;i++) {
      saP->w[tranX]->matPA[i][0] = 0.0;
      for (j=0;j<dimN;j++) {
	saP->w[tranX]->matPA[i][j+1] = (i==j);
      }
    }
  }

  return TCL_OK;
}


/* ------------------------------------------------------------------------
    SignalAdaptDeinit
   ------------------------------------------------------------------------ */

static int SignalAdaptDeinit(SignalAdapt* saP) { 
  int i,accuX,tranX;
  int dimN= saP->dimN; 

  for (tranX=0;tranX<saP->max_tran;tranX++) {
    if (saP->w[tranX]) dmatrixFree(saP->w[tranX]);
  }

  for (accuX=0;accuX<saP->max_accu;accuX++) {
    if (saP->z[accuX])  dmatrixFree(saP->z[accuX]);
    if (saP->g[accuX]) {
      for (i=0;i<dimN;i++) dmatrixFree(saP->g[accuX][i]);
      free(saP->g[accuX]);
    }
  }

  if (saP->ginv) {
    for (i=0;i<dimN;i++) dmatrixFree(saP->ginv[i]);
    free(saP->ginv);
  }

  if (saP->name) free(saP->name);
  if (saP->dsXA) free(saP->dsXA);
  if (saP->addCount) free(saP->addCount);
  if (saP->gammaX) free(saP->gammaX);

  if (saP->beta)  free (saP->beta);
  if (saP->count) free (saP->count);
  if (saP->z)     free (saP->z);
  if (saP->g)     free (saP->g);
  if (saP->w)     free (saP->w);

  return TCL_OK;
}

/* ------------------------------------------------------------------------
    SignalAdaptCreate
   ------------------------------------------------------------------------ */

static SignalAdapt* SignalAdaptCreate(char* name, DistribSet* dssP, SenoneSet* snsP, 
				      int strX, int ma, int mt) {

  SignalAdapt* saP= (SignalAdapt*) malloc(sizeof(SignalAdapt));

  if ((!saP) || (SignalAdaptInit(saP, name, dssP, snsP, strX, ma, mt) != TCL_OK)) {
    if (saP) free (saP);
    ERROR("Cannot create SignalAdapt object %s.\n", name);
    return NULL;
  }
  return saP;
}

static ClientData SignalAdaptCreateItf (ClientData cd, int argc, char *argv[]) {
  StreamArray* staP  = NULL;
  DistribSet*  dssP  = NULL; 
  SenoneSet*   snsP  = NULL;
  TypeInfo*    tiP   = NULL;
  char*        name  = NULL;
  int          ac    = argc;
  int          strX  = -1;
  int       max_accu = 5;
  int       max_tran = 1;

  if ( itfParseArgv( argv[0], &ac, argv, 0,
      "<name>",       ARGV_STRING, NULL, &name,     NULL,         "name of SignalAdapt object",
      "<SenoneSet>",  ARGV_OBJECT, NULL, &snsP,     "SenoneSet",  "name of the senone set", 
      "-stream",      ARGV_INT,    NULL, &strX,     NULL,         "stream to use",
      "-maxAccu",     ARGV_INT,    NULL, &max_accu, NULL,         "max number of accus",
      "-maxTran",     ARGV_INT,    NULL, &max_tran, NULL,         "max number of transformations",
      NULL) != TCL_OK) return NULL;

 
  staP  = &(snsP->strA); 

  if (strX == -1 && staP->streamN != 1)
    WARN ("Have %d streams, maybe use the '-stream X' option?\n", staP->streamN);
  if (strX < 0) strX = 0;

  tiP  = ((Stream*) staP->strPA[strX])->tiP;
  if (!streq (tiP->name, "DistribStream") )
    ERROR("Expected Distribstream, but got %s\n", tiP->name);

  dssP  = ((DistribStream*) staP->cdA[strX])->dssP;

  return (ClientData) SignalAdaptCreate (name, dssP, snsP, strX, max_accu, max_tran);
}

/* ------------------------------------------------------------------------
    SignalAdaptFree
   ------------------------------------------------------------------------ */
  
static int SignalAdaptFree(SignalAdapt* saP) {
  if (SignalAdaptDeinit(saP) != TCL_OK) return TCL_ERROR;
  free(saP);
  return TCL_OK;
}
  
static int SignalAdaptFreeItf (ClientData cd) {
  return SignalAdaptFree((SignalAdapt*)cd);
}

/* ------------------------------------------------------------------------
    SignalAdaptAdd
   ------------------------------------------------------------------------ */

static int SignalAdaptAdd(SignalAdapt* saP, int dsX) {
  int      dimN = saP->w[0]->m;
  Distrib*  dsP = saP->dssP->list.itemA+dsX;
  Codebook* cbP = saP->cbsP->list.itemA+dsP->cbX;
  
  if (dimN != cbP->dimN) {
    ERROR("SignalAdaptAdd: %s has bad dimension. \n",dsP->name);
    return TCL_ERROR;
  } else {
    saP->dsXA[dsX]= TRUE;
    return TCL_OK;
  }
}

static int SignalAdaptAddItf( ClientData clientData, int argc, char *argv[]) {
  SignalAdapt *saP  = (SignalAdapt*)clientData;
  int      ac    =  argc-1;
  int      dsX   = -1;

  if ( itfParseArgv (argv[0], &ac, argv+1, 0,
      "<Distribution>", ARGV_LINDEX, NULL, &dsX,&(saP->dssP->list), "distribution",
        NULL) != TCL_OK) return TCL_ERROR;

  itfAppendResult( "%d", SignalAdaptAdd( saP, dsX));
  return TCL_OK;
}


/* ------------------------------------------------------------------------
    SignalAdaptAccu
   ------------------------------------------------------------------------ */

static int compareSortElement(const void * x, const void *y) {
  SortElement* xs = (SortElement*) x;
  SortElement* ys = (SortElement*) y;
  if      ( xs->gamma > ys->gamma) return -1;
  else if ( xs->gamma < ys->gamma) return  1;
  else                             return  0;
}

static int SignalAdaptAccuOne(SignalAdapt* saP, int dsX, int frameX, float gamma, 
			      int accuX) {
  int i,j,k,topX,cbX,refX,dimN;
  float countSum;
  float*   pattern = NULL; 
  double* patternD = NULL;
  UCHAR*  patternC = NULL; 

  Codebook*   cbP;
  Distrib*    dsP;

  if (! saP->dsXA[dsX] ) return TCL_OK;

  saP->count[accuX]++;
  dsP  = saP->dssP->list.itemA+dsX;
  cbX  = dsP->cbX;
  cbP  = saP->cbsP->list.itemA+cbX;
  dimN = cbP->dimN;

  if (! (pattern = fesFrame(saP->fsP, cbP->featX, frameX))) { 
    ERROR("Can't get frame %d of feature %d\n", frameX, saP->featX); 
    return TCL_ERROR;
  }
  if (cbP->featY >= 0) {
    patternC  = (saP->cbsP->feat->featA+cbP->featY)->data.cmat->matPA[frameX];
  }
  patternD = (double*) malloc (dimN * sizeof (double));
  for (i = 0; i < dimN; i++)
    patternD[i] = pattern[i];

  if (cbP->rv)
    saP->cbsP->score( saP->cbsP, cbX, dsP->val, frameX, saP->addCount);
  else
    /* compressed codebooks */
    ssa_comp_all(saP->dssP,dsX,frameX,saP->addCount);

  for ( refX = 0; refX < cbP->refN; refX++) {
      saP->gammaX[refX].gamma = saP->addCount[refX];
      saP->gammaX[refX].refX = refX;
  }
  qsort(saP->gammaX, cbP->refN, sizeof(SortElement), compareSortElement);

  /* normalize sum of all probs of all (topN) ref vectors (should be one) */
  countSum=0.0;
  for (topX=0;topX<saP->topN;topX++) {
    refX=saP->gammaX[topX].refX;
    countSum += saP->addCount[refX]; 
  }
  for (topX=0;topX<saP->topN;topX++) {
    refX=saP->gammaX[topX].refX;
    saP->addCount[refX] /= countSum;
  }

  /* update total prob : beta =sum_s_t gamma(s,t) */
  saP->beta[accuX] += gamma;

  for (topX=0;topX<saP->topN;topX++) {
    refX=saP->gammaX[topX].refX;

    /* only diagonal covariances are supported */
    assert (cbP->cv[refX]->type == COV_DIAGONAL);

    /* FMe: This takes a long time and should be written as a BLAS matrix op */
    for (i=0;i<dimN;i++) {     
      double *z = saP->z[accuX]->matPA[i];
      double  c = gamma * saP->addCount[refX];

      if (cbP->rv) {
	c *= cbP->rv->matPA[refX][i];
	c *= cbP->cv[refX]->m.d[i];
	z[0] += c;
	for (j=0;j<dimN;j++) z[j+1] += c * patternD[j];      
      } else {
	c *= (cbP->rvC[refX][i]-saP->cbsP->offset)/saP->cbsP->scaleRV;
	if (saP->cbsP->comMode == 1) 
	  c *= cbP->cvC[refX][i]/saP->cbsP->scaleCV;
	else
	  c *= cbP->cvC[refX][i/4]/saP->cbsP->scaleCV;
	z[0] += c;
    assert (patternC); // XCode says could be NULL
	for (j=0;j<dimN;j++) z[j+1] += c * (patternC[j]-saP->cbsP->offset)/saP->cbsP->scaleRV;
      }
    }

    /* FMe: This takes a long time and should be written as a BLAS matrix op */
    for (i = 0; i < dimN; i++) {
      double **g = saP->g[accuX][i]->matPA;
      double   c = gamma * saP->addCount[refX];

      if (cbP->rv) {
	c *= cbP->cv[refX]->m.d[i];
	g[0][0] += c;      
	for (j = 0; j < dimN; j++) {
	  double d = c*patternD[j];
	  g[j+1][0]+= d;
	  for (k = 0; k <= j; k++) 
	    g[j+1][k+1] += d * patternD[k];	
	}
      } else {
	c *= cbP->cvC[refX][i]/saP->cbsP->scaleCV;
	g[0][0] += c;      
	for (j = 0; j < dimN; j++) {
	  double d = c*(patternC[j]-saP->cbsP->offset)/saP->cbsP->scaleRV;
	  g[j+1][0] += d;
	  for (k = 0; k <= j; k++) 
	    g[j+1][k+1] += d * (patternC[k]-saP->cbsP->offset)/saP->cbsP->scaleRV;
	}
      }
    }
  }

  Nulle (patternD);

  return TCL_OK;
}

static int SignalAdaptAccu(SignalAdapt* saP, Path* path, int accuX, char* match,
			   int frameA, int frameE, int strX, float gamma, FVector* conf)
{
  int frameX,i,counter=0;

  if (frameE < 0 || frameE >= path->pitemListN ) 
    frameE = path->pitemListN - 1;

  if (strX < 0)
    strX = 0;

  if (conf != NULL) {
    if (conf->n != (frameE - frameA)) {
      ERROR("Expect %d confidence values, but got %d.\n",frameE - frameA,conf->n);
      return TCL_ERROR;
    }
  }

  for (frameX = frameA; frameX <= frameE; frameX++) {
    for (i = 0; i < path->pitemListA[frameX].itemN; i++) {
      float     c = (conf == NULL) ? 1.0 : conf->vecA[frameX-frameA];
      Senone* snP = saP->snsP->list.itemA + path->pitemListA[frameX].itemA[i].senoneX;
      int     dsX = snP->classXA[((strX > snP->streamN) ? 0 : strX)];

      /*  if (match && !strstr (snP->name, match)) */
      if (match && !Tcl_RegExpMatch (itf, snP->name, match))
	continue;
      counter++;

      SignalAdaptAccuOne (saP, dsX,
			  frameX+path->firstFrame,
			  path->pitemListA[frameX].itemA[i].gamma * gamma * c,
			  accuX);
    }
  }

  if (match && strX == 1)
    INFO ("%d of %d frames accumulated (in stream %d).\n", counter, frameX-frameA, strX);

  return TCL_OK;
}

static int SignalAdaptAccuItf( ClientData cd, int argc, char *argv[]) { 
  SignalAdapt* saP   = (SignalAdapt*)cd; 
  Path*       path   =  NULL;
  FVector*    conf   =  NULL;
  int         strX   =  saP->strX;
  int         ac     =  argc-1;
  int         accuX  =  0;
  int        frameA  =  0;
  int        frameE  = -1;
  float      gamma   =  1.0;
  char*        match = NULL;

  if ( itfParseArgv( argv[0], &ac, argv+1, 0,
      "<path>",  ARGV_OBJECT, NULL, &path,  "Path",    "name of the path object",
      "<accuX>", ARGV_INT,    NULL, &accuX,  NULL,     "accu to be used",
      "-match",  ARGV_STRING, NULL, &match,  NULL,     "only accu senones that match this string",
      "-from",   ARGV_INT,    NULL, &frameA, NULL,     "start frame",
      "-to",     ARGV_INT,    NULL, &frameE, NULL,     "end frame (-1 = last frame)",
      "-stream", ARGV_INT,    NULL, &strX,   NULL,     "stream to accumulate",
      "-gamma",  ARGV_FLOAT,  NULL, &gamma,  NULL,     "scaling factor",
      "-conf",   ARGV_OBJECT, NULL, &conf,  "FVector", "Confidence values (FVector)",
  
     NULL) != TCL_OK) return TCL_ERROR;

  return SignalAdaptAccu(saP,path,accuX,match,frameA,frameE,strX,gamma,conf);
}


/* ------------------------------------------------------------------------
    SignalAdaptCompute
   ------------------------------------------------------------------------ */

static void lubksb(DMatrix* A, DVector *indx, double* b)
{
  double **a = A->matPA;
  int      n = A->m;
  double sum = 0.0;
  int     ii = -1;
  int i,ip,j;

  for (i=0;i<n;i++) {
    ip   = (int) indx->vecA[i];
    sum  = b[ip];
    b[ip]= b[i];
    if (ii > -1)
      for (j=ii;j<=i-1;j++) sum -= a[i][j]*b[j];
    else
      if (sum) ii=i;
    b[i]=sum;
  }
  for (i=n-1;i>=0;i--) {
    sum=b[i];
    for (j=i+1;j<n;j++) sum -= a[i][j]*b[j];
    b[i]=sum/a[i][i];
  }
}

/* -------------------------------------------*/

static DMatrix* tmp1P = NULL;

static int dludcmp_error_counter = 0;

static double cofactor (DMatrix* w, int i0, double* cofactorA)
{
  DVector *indx, *col;
  double   det;
  int dimN = w->m;
  int i,j;

  if (tmp1P == NULL) {
    tmp1P = dmatrixCreate(dimN , dimN);
  } else {
    dmatrixResize(tmp1P,dimN ,dimN);
  }
  indx = dvectorCreate(dimN);
  col  = dvectorCreate(dimN);

  for (i=0;i<dimN;i++) cofactorA[i]=0.0;

  /* get w without shift */
  for (i=0;i<dimN;i++) {
    for (j=0;j<dimN;j++) {
      tmp1P->matPA[i][j]= w->matPA[i][j+1];
    }
  }

  /* get det(W) */  
  dludcmp_error_counter += dludcmp(tmp1P, indx, &det);
  for (i=0; i < dimN; i++) { det *= tmp1P->matPA[i][i]; }

  /* get i0.th column of INV(W) */
  col->vecA[i0]= 1.0;
  lubksb(tmp1P,indx,col->vecA);

  /* Adjunct(A) = det(A) * inv(A) */
  for (j=0;j<dimN;j++) 
    cofactorA[j] = det * col->vecA[j];

  dvectorFree(indx);
  dvectorFree(col);

  return det;
}

/* -------------------------------------------*/

static DMatrix* tmp2P = NULL;

static void dmatrixInvByLU(DMatrix* GINV, DMatrix* G)
{  
  DVector *indx;
  double   det;
  int dimN = G->m;
  int i,j;

  if (tmp2P == NULL) {
    tmp2P = dmatrixCreate(dimN, dimN);
  } else {
    dmatrixResize(tmp2P,dimN, dimN);
  }
  indx = dvectorCreate(dimN);
  memcpy(tmp2P->matPA[0],G->matPA[0],sizeof(double)*dimN*dimN);

  dludcmp(tmp2P, indx, &det);
  
  /* !!! Hopefully not necessary to check !!!
    for (i=0; i < dimN; i++) { det *= tmp2P->matPA[i][i]; 
    if (fabs(det) < 1e-18 || fabs(det) > 1e18)
    WARNING("dmatrixInvByLU: couldn't inverse G. det(G) under/over flow: %f\N",det);
  */

  /* this goes column by column, but it doesn't matter because g[i][j]=g[j][i] */
  for (i=0; i < dimN; i++) {
    double* col= GINV->matPA[i];
    for (j=0; j < dimN; j++) col[j]= (i==j);
    lubksb(tmp2P,indx,col);
  }

  dvectorFree(indx);
}
/* -------------------------------------------*/

#define DEBUG_SADAPT 0

static int SignalAdaptCompute(SignalAdapt* saP, int iterN,int accuX,int tranX) {
  int i,j,k,iterX;
  DVector   *tmpD = NULL;
  DVector   *tmpP = NULL; 
   
  double beta2 = saP->beta[accuX];
  double   det = 1.0;
  double delta = 0.0;
  int     dimN = saP->dimN; 

  tmpD= dvectorCreate(dimN+1);
  tmpP= dvectorCreate(dimN+1);

  /* compute inverse GINV[i] = inv G[i] */
  for (i=0;i<dimN;i++) {
    double **g = saP->g[accuX][i]->matPA;
    /* make g symmetric */
    for (j=0;j<=dimN;j++) {
      for (k=0;k<=j;k++) {
	g[k][j]=g[j][k];
      }
    }
    dmatrixInvByLU(saP->ginv[i],saP->g[accuX][i]);   
  }

  dludcmp_error_counter = 0;
  for (iterX=0;iterX<iterN;iterX++) {
    for (i=0;i<dimN;i++) {
      double alpha,alpha1, alpha2;
      double kullback1,kullback2;
      double  term1 = 0.0;
      double  term2 = 0.0;
      double  term3 = 0.0;
      double  term4 = 0.0;

      register double  *zTmp = saP->z[accuX]->matPA[i];    

      register double  *dTmp = tmpD->vecA;
      register double  *pTmp = tmpP->vecA;

      pTmp[0]= 0.0;
      det= cofactor(saP->w[tranX],i,pTmp+1);

      /* here is the mystic part */
      delta =fabs(det - 1.0);
      if (det < 0.98) 
	beta2 += beta2 * delta /500;
      if (det > 1.02) 
	beta2 -= beta2 * delta /500;

      /* dTmp = p[i] * GINV[i] */
      for (j=0;j<=dimN;j++) { 
	dTmp[j] = 0.0;
	for (k=0;k<=dimN;k++) {
	  dTmp[j] += pTmp[k] * saP->ginv[i]->matPA[k][j];
	}
      }

      /* term1 = p[i] * GINV[i] * p[i] = dTmp * p[i] */
      for (j=0;j<=dimN;j++) { 
	term1 += dTmp[j] * pTmp[j];
      }
      
      /* term2 = p[i] * GINV[i] * z[i] = dTmp * z[i] */
      for (j=0;j<=dimN;j++) { 
	term2 += dTmp[j] * zTmp[j];
      }
      
      /* solve term1 * a^2 + term2 * a - beta = 0 */
      term3 = term2 / (2 * term1);
      term4 = term3 * term3 + beta2 / term1;
      term4 = sqrt(term4);

      alpha1 = -1.0 * term3 + term4;
      alpha2 = -1.0 * term3 - term4;

      /* select alpha which maximize likelihood */
      kullback1= beta2 * log(fabs(alpha1*term1+term2)) - 0.5 * alpha1*alpha1*term1;
      kullback2= beta2 * log(fabs(alpha2*term1+term2)) - 0.5 * alpha2*alpha2*term1;
      if (kullback2 > kullback1) {
	alpha = alpha2;
      } else {
	alpha = alpha1;
      }
      
      /*reestimate w : w[i] = (alpha*p[i] + z[i]) * ginv[i] */
      for (j=0;j<=dimN;j++) { 
	dTmp[j] = 0.0;
	for (k=0;k<=dimN;k++) {
	  dTmp[j] += (alpha * pTmp[k] + zTmp[k]) * saP->ginv[i]->matPA[k][j];
	}
      }
      for (j=0;j<=dimN;j++) {
	saP->w[tranX]->matPA[i][j] = dTmp[j];
      }      
    }
  }

  if (dludcmp_error_counter) {
    WARN ("Singular matrix in dludcmp! (%d times)\n", dludcmp_error_counter);
  }

  /*check det(W) */
  if (! finite(delta) || ! finite(beta2) || delta > 0.3) {
    WARN("SignalAdaptCompute: det(W) != 1.0.  will reset W (det= %4.4f  beta2= %4.4f)\n",det,beta2);
    for (i=0;i<dimN;i++) {
      /* w[i][0] holds the shift */
      saP->w[tranX]->matPA[i][0] = 0.0;
      for (j=0;j<dimN;j++)
	saP->w[tranX]->matPA[i][j+1] = (i==j);
    }
  } else {
    INFO("SignalAdaptCompute: det= %4.4f  beta2= %4.4f\n",det,beta2);
  }

  dvectorFree(tmpD);
  dvectorFree(tmpP);
  
  return TCL_OK;
}

/* -------------------------------------------*/

static int SignalAdaptComputeItf( ClientData cd, int argc, char *argv[]) { 
  SignalAdapt* saP   = (SignalAdapt*)cd; 
  int         ac     =  argc-1;
  int         iterN  =  0;
  int         accuX  =  0;
  int         tranX  =  0;

  if ( itfParseArgv( argv[0], &ac, argv+1, 0,
      "<iter>",  ARGV_INT, NULL, &iterN, NULL, "Number of iterations",
      "<accuX>", ARGV_INT, NULL, &accuX, NULL, "accu index",
      "<tranX>", ARGV_INT, NULL, &tranX, NULL, "transformation index",
       NULL) != TCL_OK) return TCL_ERROR;

  if (accuX>=saP->max_accu) {
    ERROR("Bad accu index: %d should be less then %d.\n",accuX,saP->max_accu);
    return TCL_ERROR;
  }
  if (tranX>=saP->max_tran) {
    ERROR("Bad transformation index: %d should be less than %d.\n",tranX,saP->max_tran);
    return TCL_ERROR;
  }
  INFO("SignalAdaptCompute: using %d frames\n",saP->count[accuX]);

  SignalAdaptCompute(saP,iterN,accuX,tranX);
  return TCL_OK;
}

/* ------------------------------------------------------------------------
    SignalAdaptAdapt
   ------------------------------------------------------------------------ */

static int SignalAdaptAdapt(SignalAdapt* saP,FMatrix* src, FMatrix* dst,int tranX) 
{
  FMatrix* sav;
  int dimX,frameX,i;

  int frameN=src->m;
  int dimN= saP->dimN; 

  if (src == dst) {
    sav=fmatrixCreate(frameN,dimN);
    fmatrixCopy(sav,src);
  } else {
    sav = src;
  }

  fmatrixResize(dst,frameN,dimN);

  /* translation only, no rotation */
  if (saP->shift == -1) {
    for (frameX=0;frameX<frameN;frameX++) {
      for (dimX=0;dimX<dimN;dimX++) {
	dst->matPA[frameX][dimX]= saP->w[tranX]->matPA[dimX][0] + sav->matPA[frameX][dimX];
      }
    }
  } else {
    /* do all */
#ifdef BLAS
    FMatrix* tmp  = fmatrixCreate (saP->w[tranX]->m, saP->w[tranX]->n-1);
    /* copy this into a transposed float matrix */
    for (dimX = 0; dimX < dimN; dimX++) 
      for (i = 0; i < dimN; i++)
	tmp->matPA[dimX][i] = saP->w[tranX]->matPA[dimX][i+1];

    /* initialize the target matrix */
    for (frameX = 0; frameX < frameN; frameX++)
      for (dimX = 0; dimX < dimN; dimX++)
	dst->matPA[frameX][dimX] = saP->w[tranX]->matPA[dimX][0];

    /* fmatrixMulot (dst, sav, tmp) (kind of) */
    cblas_sgemm (CblasRowMajor, CblasNoTrans, CblasTrans, 
		 sav->m, tmp->m, tmp->n, 1, 
		 sav->matPA[0], (sav->m > 1) ? sav->matPA[1] - sav->matPA[0] : sav->n, 
		 tmp->matPA[0], (tmp->m > 1) ? tmp->matPA[1] - tmp->matPA[0] : tmp->n, saP->shift, 
		 dst->matPA[0], (dst->m > 1) ? dst->matPA[1] - dst->matPA[0] : dst->n);

    fmatrixFree (tmp);
#else
    for (frameX=0;frameX<frameN;frameX++) {
      for (dimX=0;dimX<dimN;dimX++) {
        dst->matPA[frameX][dimX]= saP->shift * saP->w[tranX]->matPA[dimX][0];
        for (i=0;i<dimN;i++) {
          dst->matPA[frameX][dimX]+=saP->w[tranX]->matPA[dimX][i+1] * sav->matPA[frameX][i];
        }
      }
    }
#endif
  }

  if (src == dst)
    fmatrixFree(sav);

  return TCL_OK;
}

static int SignalAdaptAdaptItf( ClientData cd, int argc, char *argv[]) 
{ 
  SignalAdapt* saP   = (SignalAdapt*)cd; 
  int         ac     =  argc-1;
  FMatrix*    src    = NULL;
  FMatrix*    dst    = NULL;
  int         tranX  = 0;

 if ( itfParseArgv( argv[0], &ac, argv+1, 0,
      "<src>",   ARGV_OBJECT, NULL, &src,   "FMatrix", "source feature, FMatrix",
      "<dst>",   ARGV_OBJECT, NULL, &dst,   "FMatrix", "dst feature, FMatrix", 
      "<tranX>", ARGV_INT,    NULL, &tranX, NULL,      "transformation index", 
		    NULL) != TCL_OK) return TCL_ERROR;

 if (tranX>=saP->max_tran) {
   ERROR("Bad transformation index: %d should be less then %d.\n",tranX,saP->max_tran);
   return TCL_ERROR;
 }
  SignalAdaptAdapt(saP,src,dst,tranX);
  return TCL_OK;
}


/* ------------------------------------------------------------------------
    Combine  hyu@cs   Combine two linear transformations into one
                      T1(.) <== T2( T1(.))
   ------------------------------------------------------------------------ */

int SignalAdaptCombine(SignalAdapt* saP,int tranX1, int tranX2) {
  DMatrix *m1 = saP->w[tranX1];
  DMatrix *m2 = saP->w[tranX2];
  int    dimN = saP->dimN; 
  int    dimX,i,j,k;

  /* translation only, no rotation */
  if (saP->shift == -1) {
    for (dimX=0; dimX<dimN; dimX++) {
      m1->matPA[dimX][0] += m2->matPA[dimX][0];
    }
  } else {
    DMatrix *m0 = dmatrixCreate(m1->m, m1->n);

    /* do all */
    for (i=0; i<dimN; i++) {
      m0->matPA[i][0] = m2->matPA[i][0];
      for (k=0; k<dimN; k++)
	m0->matPA[i][0] += m2->matPA[i][k+1] * m1->matPA[k][0];
    }
    for (i=0; i<dimN; i++)
      for (j=0; j<dimN; j++)
	for (k=0; k<dimN; k++)
	  m0->matPA[i][j+1] += m2->matPA[i][k+1] * m1->matPA[k][j+1];
    dmatrixCopy( m1, m0);
    dmatrixFree( m0);
  }

  return TCL_OK;
}

static int SignalAdaptCombineItf( ClientData cd, int argc, char *argv[]) { 
  SignalAdapt* saP   = (SignalAdapt*)cd; 
  int         ac     =  argc-1;
  int         tranX1 = 0;
  int         tranX2 = 1;

  if ( itfParseArgv( argv[0], &ac, argv+1, 0,
      "<tranX1>", ARGV_INT,    NULL, &tranX1, NULL,      "transformation 1", 
      "<tranX2>", ARGV_INT,    NULL, &tranX2, NULL,      "transformation 2", 
		    NULL) != TCL_OK) return TCL_ERROR;

  if (tranX1>=saP->max_tran || tranX2>=saP->max_tran) {
    ERROR("wrong transfomration index: %d and %d should be below %d\n",tranX1, tranX2,saP->max_tran);
    return TCL_ERROR;
  }
  if (tranX1==tranX2) {
    ERROR("Can't combine the same transformations. Copy to another tranX first.\n");
    return TCL_ERROR;
  }
  SignalAdaptCombine(saP,tranX1,tranX2);
  return TCL_OK;
}

/* -------------------------------------------*/


/* ------------------------------------------------------------------------
    SignalAdaptClear
   ------------------------------------------------------------------------ */

int SignalAdaptClearAccu(SignalAdapt* saP, int accuX)
{
  int i;
  int dimN= saP->dimN; 
  if (accuX>=saP->max_accu) {
    ERROR("SignalAdaptClearAccu: Bad accu index: %d should be less then %d.\n",
	  accuX,saP->max_accu);
    return TCL_ERROR;
  }

  saP->count[accuX]=0;
  saP->beta[accuX] =0;
  dmatrixClear(saP->z[accuX]);
  for (i=0;i<dimN;i++) {
    dmatrixClear(saP->g[accuX][i]);
  }

  return TCL_OK;
}

static int SignalAdaptClearAccuItf( ClientData cd, int argc, char *argv[])
{ 
  SignalAdapt* saP   = (SignalAdapt*)cd; 
  int         ac     = argc-1;
  int         accuX  = 0;
  if ( itfParseArgv( argv[0], &ac, argv+1, 0,
      "<accuX>", ARGV_INT, NULL, &accuX, NULL, "accu index",
		     NULL) != TCL_OK) return TCL_ERROR;
 
  return SignalAdaptClearAccu(saP,accuX);
}

/* ------------------------------------------------------------------------
    SignalAdaptScaleAccu
   ------------------------------------------------------------------------ */

static int SignalAdaptScaleAccu( SignalAdapt* saP, int accuX, float scale)
{
  int i,j,k;
  int dimN= saP->dimN;
  
  if (accuX>=saP->max_accu) {
    ERROR("SignalAdaptScaleAccu: Bad accu index: %d should be less then %d.\n",
	  accuX,saP->max_accu);
    return TCL_ERROR;
  }

  saP->count[accuX] *= scale;
  saP->beta[accuX]  *= scale;
  
  for (i=0;i<dimN;i++)
    for (j=0;j<=dimN;j++)
      saP->z[accuX]->matPA[i][j] *= scale;

  for (i=0;i<dimN;i++)
    for (j=0;j<=dimN;j++)
      for (k=0;k<=dimN;k++)
	saP->g[accuX][i]->matPA[j][k] *= scale;
 
  return TCL_OK;
}

static int SignalAdaptScaleAccuItf( ClientData cd, int argc, char *argv[]) { 
  SignalAdapt* saP   = (SignalAdapt*)cd; 
  int         ac     =  argc-1;
  float       scale  = 1.0;
  int         accuX  = 0;
  

  if ( itfParseArgv( argv[0], &ac, argv+1, 0,
      "<factor>", ARGV_FLOAT, NULL, &scale, NULL, "scaling factor for accu's",
      "<accuX>", ARGV_INT, NULL, &accuX, NULL, "accu index",
		   NULL) != TCL_OK) return TCL_ERROR;

  return SignalAdaptScaleAccu(saP,accuX,scale);
}


/* ------------------------------------------------------------------------
    SignalAdaptClear
   ------------------------------------------------------------------------ */

static int SignalAdaptClear( SignalAdapt* saP, int tranX)
{
  int i,j;
  int dimN= saP->dimN; 

  if (tranX>=saP->max_tran) {
    ERROR("SignalAdaptClear: Bad transformation index: %d should be less then %d.\n",
	  tranX,saP->max_tran);
    return TCL_ERROR;
  }

  /*Init saP->w = Identity */
  for (i=0;i<dimN;i++) {
    /* w[i][0] holds the shift */
    saP->w[tranX]->matPA[i][0] = 0.0;
    for (j=0;j<dimN;j++) {
      saP->w[tranX]->matPA[i][j+1] = (i==j);
    }
  }

  return TCL_OK;
}

static int SignalAdaptClearItf( ClientData cd, int argc, char *argv[]) { 
  SignalAdapt* saP   = (SignalAdapt*)cd; 
  int         ac     =  argc-1;
  int         tranX  = 0;

  if ( itfParseArgv( argv[0], &ac, argv+1, 0,
     "<tranX>", ARGV_INT, NULL, &tranX, NULL, "transformation index",
		     NULL) != TCL_OK) return TCL_ERROR;

  return SignalAdaptClear(saP,tranX);
}

/* ------------------------------------------------------------------------
    SignalAdaptCompareTransform
   ------------------------------------------------------------------------ */

static float SignalAdaptCompareTransform (SignalAdapt* saP, int tranX, int tranY)
{
  double sum = 0.0;
  int   dimN = saP->dimN; 
  int i,j;
 
  if (tranX>=saP->max_tran) {
    ERROR("SignalAdaptCompareTransform : Bad transformation index: %d should be less then %d.\n",
	  tranX,saP->max_tran);
    return sum;
  }
  if (tranY>=saP->max_tran) {
    ERROR("SignalAdaptCompareTransform : Bad transformation index: %d should be less then %d.\n",
	  tranY,saP->max_tran);
    return sum;
  }

  /* sum of squares */
  for (i=0;i<dimN;i++) {
    for (j=0;j<dimN;j++) {
      double tmp = saP->w[tranX]->matPA[i][j] - saP->w[tranY]->matPA[i][j];
      sum += tmp*tmp;
    }
  }
  return sum;
}

static int SignalAdaptCompareTransformItf( ClientData cd, int argc, char *argv[]) { 
  SignalAdapt* saP   = (SignalAdapt*)cd; 
  int         ac     =  argc-1;
  int         tranX  = 0;
  int         tranY  = 1;
 
  if ( itfParseArgv( argv[0], &ac, argv+1, 0,
     "<tranX>", ARGV_INT, NULL, &tranX, NULL, "transformation index",
     "<tranY>", ARGV_INT, NULL, &tranY, NULL, "transformation index",
		     NULL) != TCL_OK) return TCL_ERROR;

  
  itfAppendResult("%4.4f",SignalAdaptCompareTransform(saP,tranX,tranY));
  return TCL_OK;
}


/* ------------------------------------------------------------------------
    SignalAdaptWriteAccu
   ------------------------------------------------------------------------ */

static int SignalAdaptWriteAccu (SignalAdapt* saP, char* name, int accuX)
{
  int  dimN = saP->dimN; 
  FILE  *fp = NULL;
  float dummy;
  int i,j,k;

  if (accuX>=saP->max_accu) {
    ERROR("SignalAdaptWriteAccu: Bad accu index: %d should be less then %d.\n",
	  accuX,saP->max_accu);
    return TCL_ERROR;
  }

  if ((fp=fileOpen(name,"w")) == NULL) {
    ERROR("SignalAdaptWriteAccu: couldn't open '%s'\n",name);
    return TCL_ERROR;
  }
  fwrite("SAS",sizeof(char),3,fp);

  write_int(fp,saP->dimN);
  write_int(fp,saP->count[accuX]);
  dummy= (float) saP->beta[accuX];
  write_float(fp,dummy);

  /*write z */
  for (i=0;i<dimN;i++)  
    for (j=0;j<=dimN;j++) {
      dummy= (float) saP->z[accuX]->matPA[i][j];
      write_float(fp, dummy);
    }

  /*write g */
  for (i=0;i<dimN;i++)
    for (j=0;j<=dimN;j++)
      for (k=0;k<=dimN;k++) {
	dummy= (float) saP->g[accuX][i]->matPA[j][k];
	write_float(fp,dummy); 
      }

  fileClose(name,fp);
   return TCL_OK;
}

static int SignalAdaptWriteAccuItf( ClientData cd, int argc, char *argv[]) 
{ 
  int           ac =  argc-1;
  SignalAdapt *saP = (SignalAdapt*)cd; 
  char*       name = NULL;
  int        accuX = 0;

  if ( itfParseArgv( argv[0], &ac, argv+1, 0,
      "<filename>", ARGV_STRING, NULL, &name, NULL, "filename",
      "<accuX>", ARGV_INT, NULL, &accuX, NULL, "accu index",
		     NULL) != TCL_OK) return TCL_ERROR;

  return SignalAdaptWriteAccu(saP,name,accuX);
}

/* ------------------------------------------------------------------------
    SignalAdaptReadAccu
   ------------------------------------------------------------------------ */

static int SignalAdaptReadAccu( SignalAdapt* saP, char* name, int accuX, 
				float factor) 
{ 
  FILE*   fp = NULL;
  char  fmagic[3];
  float dummy;
  int i,j,k,dimN;

  if (accuX>=saP->max_accu) {
    ERROR("SignalAdaptReadAccu: Bad accu index: %d should be less then %d.\n",
	  accuX,saP->max_accu);
    return TCL_ERROR;
  }

  if ((fp=fileOpen(name,"r")) == NULL) {
    ERROR("SignalAdaptReadAccu: Couldn't open '%s'\n",name);
    return TCL_ERROR;
  }

  if ( fread (fmagic, sizeof(char), 3, fp) != 3 || strncmp("SAS",fmagic,3)) {
    ERROR("SignalAdaptReadAccu: Couldn't find magic SAS in file %s.\n",name);
    fileClose(name,fp);
    return TCL_ERROR;
  }


  dimN=read_int(fp);

  if (dimN != saP->dimN) {
    ERROR("SignalAdaptReadAccu: Dimension mismatch. Exptected dimN= %d, but got %d.\n",
	  saP->dimN,dimN);
    fileClose(name,fp);
    return TCL_ERROR;
  }
	
  /* read count, beta */
  saP->count[accuX] += factor * read_int(fp);
  dummy = read_float(fp);
  saP->beta[accuX]  += (double) factor * (double) dummy;

  /*read z*/  
  for (i=0;i<dimN;i++) {
    for (j=0;j<=dimN;j++) {
      dummy= read_float(fp);
      saP->z[accuX]->matPA[i][j] += (double) factor * (double) dummy;
    }
  }

  /*read g[i]*/
  for (i=0;i<dimN;i++) {
    for (j=0;j<=dimN;j++) {
      for (k=0;k<=dimN;k++) {
	dummy= read_float(fp);
	saP->g[accuX][i]->matPA[j][k] += (double) factor * (double) dummy;
      }
    }
  }

  fileClose(name,fp);
  return TCL_OK;
}

static int SignalAdaptReadAccuItf( ClientData cd, int argc, char *argv[]) 
{ 
  int           ac =  argc-1;
  SignalAdapt *saP = (SignalAdapt*)cd; 
  char       *name = NULL; 
  float     factor = 1.0;
  int        accuX = 0;

  if ( itfParseArgv( argv[0], &ac, argv+1, 0,
      "<filename>", ARGV_STRING, NULL, &name,   NULL, "filename",
      "<accuX>",    ARGV_INT,    NULL, &accuX,  NULL, "accu index",
      "-factor",    ARGV_FLOAT,  NULL, &factor, NULL, "weighting factor",
		     NULL) != TCL_OK) return TCL_ERROR;

  return SignalAdaptReadAccu(saP,name,accuX,factor);
}

/* ------------------------------------------------------------------------
    SignalAdaptSave
   ------------------------------------------------------------------------ */

static int SignalAdaptSave (SignalAdapt* saP, char* name, int tranX)
{
  int   dimN = saP->dimN; 
  FILE*   fp = NULL;
  int i,j;

  if (tranX>=saP->max_tran) {
    ERROR("SignalAdaptSave: Bad transformation index: %d should be less then %d.\n",
	  tranX,saP->max_tran);
    return TCL_ERROR;
  }
  if ((fp=fileOpen(name,"w")) == NULL) {
    ERROR("SignalAdaptSave: Couldn't open '%s'\n",name);
    return TCL_ERROR;
  }
  fwrite("SAW",sizeof(char),3,fp);

  write_int(fp,saP->dimN);

  /*write w*/  
  for (i=0;i<dimN;i++) 
    for (j=0;j<=dimN;j++) 
      write_float(fp, (float) saP->w[tranX]->matPA[i][j]);

  fileClose(name,fp);
  return TCL_OK;
}

static int SignalAdaptSaveItf( ClientData cd, int argc, char *argv[]) { 
  int           ac =  argc-1;
  SignalAdapt *saP = (SignalAdapt*)cd; 
  char       *name = NULL; 
  int        tranX = 0;

  if ( itfParseArgv( argv[0], &ac, argv+1, 0,
      "<filename>", ARGV_STRING, NULL, &name, NULL, "filename",
      "<tranX>",    ARGV_INT,    NULL, &tranX, NULL, "transformation index",
		     NULL) != TCL_OK) return TCL_ERROR;

  return SignalAdaptSave(saP,name,tranX);
}

/* ------------------------------------------------------------------------
    SignalAdaptLoad
   ------------------------------------------------------------------------ */

static int SignalAdaptLoad (SignalAdapt* saP, char* name, int tranX)
{
  FILE*   fp = NULL;
  char fmagic[3];
  int i,j,dimN;

  if (tranX>=saP->max_tran) {
    ERROR("SignalAdaptLoad: Bad transformation index: %d should be less then %d.\n",
	  tranX,saP->max_tran);
    return TCL_ERROR;
  }

  if ((fp=fileOpen(name,"r")) == NULL) {
    ERROR("SignalAdaptLoad: Couldn't open '%s'\n",name);
    return TCL_ERROR;
  }
  if ( fread (fmagic, sizeof(char), 3, fp) != 3 || strncmp("SAW",fmagic,3)) {
    ERROR("SignalAdaptLoad: Couldn't find magic SAW in file %s\n",name);
    fileClose(name,fp);
    return TCL_ERROR;
  }

  dimN=read_int(fp);

  if (dimN != saP->dimN) {
    ERROR("SignalAdaptLoad: Dimension mismatch. Expected dimN= %d but got %d.\n",
	  saP->dimN,dimN);
    fileClose(name,fp);
    return TCL_ERROR;
  }

  /*read w*/  
  for (i=0;i<dimN;i++) 
    for (j=0;j<=dimN;j++) 
      saP->w[tranX]->matPA[i][j]= (double) read_float(fp);

  fileClose(name,fp);
  return TCL_OK;
}

static int SignalAdaptLoadItf( ClientData cd, int argc, char *argv[]) { 
  int           ac =  argc-1;
  SignalAdapt *saP = (SignalAdapt*)cd; 
  char       *name = NULL; 
  int        tranX = 0;

  if ( itfParseArgv( argv[0], &ac, argv+1, 0,
      "<filename>", ARGV_STRING, NULL, &name, NULL, "filename",
      "<tranX>",    ARGV_INT,    NULL, &tranX, NULL, "transformation index",
		     NULL) != TCL_OK) return TCL_ERROR;

  return SignalAdaptLoad(saP,name,tranX);
}

/* ------------------------------------------------------------------------
    SignalAdaptAddAccu
   ------------------------------------------------------------------------ */

static int SignalAdaptAddAccu( SignalAdapt* saP, int accuX, int accuY, 
			       float factor)
{ 
  int dimN = saP->dimN; 
  int i,j,k;

  saP->count[accuX] += factor * saP->count[accuY];
  saP->beta[accuX]  += factor * saP->beta[accuY];

  for (i=0;i<dimN;i++) 
    for (j=0;j<=dimN;j++) 
      saP->z[accuX]->matPA[i][j] += factor * saP->z[accuY]->matPA[i][j];

  for (i=0;i<dimN;i++) 
    for (j=0;j<=dimN;j++) 
      for (k=0;k<=dimN;k++) 
	saP->g[accuX][i]->matPA[j][k] += factor * saP->g[accuY][i]->matPA[j][k];
  
  return TCL_OK;
}

static int SignalAdaptAddAccuItf( ClientData cd, int argc, char *argv[]) { 
  int           ac =  argc-1;
  SignalAdapt *saP = (SignalAdapt*)cd; 
  int        accuX = 0;
  int        accuY = 0;
  float     factor = 0.0;

  if ( itfParseArgv( argv[0], &ac, argv+1, 0,
      "<accuX>", ARGV_INT,   NULL, &accuX,  NULL, "accuX",
      "<accuY>", ARGV_INT,   NULL, &accuY,  NULL, "accuY",
      "-factor", ARGV_FLOAT, NULL, &factor, NULL, "weighting factor",
		     NULL) != TCL_OK) return TCL_ERROR;

  return SignalAdaptAddAccu(saP,accuX,accuY,factor);
}

/* -------------------------------------------*/

static int SignalAdaptConfigureItf(ClientData cd, char *var, char *val) {
  int accuX;
  char betaStr[200];
  SignalAdapt* saP   = (SignalAdapt*)cd; 
  if (! saP) return TCL_ERROR;
  if (! var) {
    sprintf(betaStr,"beta(0..%d)",saP->max_accu);
    ITFCFG(SignalAdaptConfigureItf,cd,"name");
    ITFCFG(SignalAdaptConfigureItf,cd,"useN");
    ITFCFG(SignalAdaptConfigureItf,cd,"stream");
    ITFCFG(SignalAdaptConfigureItf,cd,"topN");
    ITFCFG(SignalAdaptConfigureItf,cd,"shift");
    ITFCFG(SignalAdaptConfigureItf,cd,betaStr);
    return TCL_OK;
  }
  ITFCFG_CharPtr(var,val,"name",   saP->name,   1);
  ITFCFG_Int(    var,val,"useN",   saP->useN,   1);
  ITFCFG_Int(    var,val,"stream", saP->strX,   1);
  ITFCFG_Int(    var,val,"topN",   saP->topN,   0);
  ITFCFG_Float(  var,val,"shift",  saP->shift,  0);
  for (accuX=0;accuX<saP->max_accu;accuX++) {
     sprintf(betaStr,"beta(%d)",accuX);
     ITFCFG_Float(  var,val,betaStr,saP->beta[accuX],  0);
     betaStr[0]=0;
  }
  /* ERROR("unknown option '-%s %s'.\n", var, val ? val : "?"); */
  return TCL_ERROR;   
}

/* -------------------------------------------*/

static ClientData SignalAdaptAccessItf (ClientData cd, char *name, TypeInfo **ti) {
  SignalAdapt* saP = (SignalAdapt*)cd;
  int dimX,accuX,tranX;
  int dimN= saP->cbsP->list.itemA[0].dimN -1; 

  if ( *name == '.') {
    if ( name[1] =='\0') {
        itfAppendElement("z(0..%d)",saP->max_accu-1);
	itfAppendElement("g(0..%d,0..%d)",saP->max_accu-1,dimN); 
	itfAppendElement("w(0..%d)",saP->max_tran-1);
	*ti= NULL;
	return NULL;
    }
    else {
      if (sscanf(name+1,"z(%d)",&accuX) == 1) {
	if (accuX <= saP->max_accu) {
	  *ti = itfGetType("DMatrix"); 
	  return (ClientData)(saP->z[accuX]);
	}
      }
      if (sscanf(name+1,"g(%d,%d)",&accuX,&dimX) == 2) {
	if (dimX <= dimN && accuX < saP->max_accu) {
	  *ti = itfGetType("DMatrix"); 
	  return (ClientData)(saP->g[accuX][dimX]);
	}
      }
      if (sscanf(name+1,"w(%d)",&tranX) == 1) {
	if (tranX <= saP->max_tran) {
	  *ti = itfGetType("DMatrix"); 
	  return (ClientData)(saP->w[tranX]);
	}    
      }
    }
  }  
  *ti= NULL;
  return NULL;
}

/* -------------------------------------------*/

static int SignalAdaptPutsItf (ClientData cd, int argc, char *argv[]) { 
  SignalAdapt* saP = (SignalAdapt*)cd;
  int dssX, dssN= saP->dssP->list.itemN;         
  
  for (dssX=0;dssX<dssN;dssX++) {
    if (saP->dsXA[dssX]) {
      itfAppendResult("%s ", dssName(saP->dssP,dssX));
    }
  }
  return TCL_OK;
}


/* -------------------------------------------*/

static Method SignalAdaptMethod[] = { 
  { "puts",        SignalAdaptPutsItf,      "puts distributons"},
  { "add",         SignalAdaptAddItf,       "add distribution for signal adaption"},
  { "accu",        SignalAdaptAccuItf,      "accu path for signal adaption"},
  { "compute",     SignalAdaptComputeItf,   "compute adaption matrix"},
  { "adapt",       SignalAdaptAdaptItf,     "adapt feature"},
  { "clear",       SignalAdaptClearItf,     "clear parameter matrix (will not be done automatically!)"},
  { "compare",     SignalAdaptCompareTransformItf,  "compare two transforms (sum of squares)"},
  { "combine",     SignalAdaptCombineItf,   "combine two transforms"},
  { "save",        SignalAdaptSaveItf,      "save  parameter matrix"},
  { "load",        SignalAdaptLoadItf,      "load  parameter matrix"},
  { "scaleAccu",   SignalAdaptScaleAccuItf, "scale accu's"},
  { "clearAccu",   SignalAdaptClearAccuItf, "clear accu's"},
  { "writeAccu",   SignalAdaptWriteAccuItf, "write accu's"},
  { "readAccu",    SignalAdaptReadAccuItf,  "read accu's"},
  { "addAccu",     SignalAdaptAddAccuItf,   "accuX += factor *accuY"},
  {  NULL,  NULL, NULL } 
};

TypeInfo SignalAdaptInfo = {
        "SignalAdapt", 0, -1, SignalAdaptMethod, 
         SignalAdaptCreateItf, SignalAdaptFreeItf,
         SignalAdaptConfigureItf, SignalAdaptAccessItf,
         itfTypeCntlDefaultNoLink,
        "Signal Adaption\n" 
};



static int SignalAdaptInitialized = 0;

int SignalAdapt_Init() {
  if ( SignalAdaptInitialized) return TCL_OK;
  itfNewType( &SignalAdaptInfo);
  SignalAdaptInitialized = 1;
  return TCL_OK;
}
