/* ========================================================================
    JANUS      Speech- to Speech Translation
               ------------------------------------------------------------
               Object: semi tied covariances
               ------------------------------------------------------------

    Author  :  Matthias Seeger
    Module  :  matrix_ext.c
    Date    :  $Id: matrix_ext.c 2693 2005-11-08 13:09:16Z metze $
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
    Revision 1.2  2005/11/08 13:09:16  metze
    Fixed uninitialized access

    Revision 1.1  2000/08/16 11:32:47  soltau
    Initial revision

   ======================================================================== */


/* Janus standards */
#include "common.h"
#include "error.h"
#include "mach_ind_io.h"

#include "matrix_ext.h"
#include "cbn_global.h"

#include <stdlib.h>

/* --- Module local functions --- */

/* -------------------------------------------------------------------
   fmat2matx
   No conversion, just masking!
   ------------------------------------------------------------------- */
static void fmat2matx (FMatrixX* xmat,FMatrix* fmat)
{
  xmat->m = xmat->buff_m = fmat->m;
  xmat->n = xmat->buff_n = fmat->n;
  xmat->matPA = fmat->matPA;
}

/* -------------------------------------------------------------------
   fmatx2mat
   No conversion, just masking!
   ------------------------------------------------------------------- */
static void fmatx2mat (FMatrix* fmat,FMatrixX* xmat)
{
  fmat->m = xmat->m;
  fmat->n = xmat->n;
  fmat->matPA = xmat->matPA;
}

/* -------------------------------------------------------------------
   dmat2matx
   No conversion, just masking!
   ------------------------------------------------------------------- */
static void dmat2matx (DMatrixX* xmat,DMatrix* fmat)
{
  xmat->m = xmat->buff_m = fmat->m;
  xmat->n = xmat->buff_n = fmat->n;
  xmat->matPA = fmat->matPA;
}

/* -------------------------------------------------------------------
   dmatx2mat
   No conversion, just masking!
   ------------------------------------------------------------------- */
static void dmatx2mat (DMatrix* fmat,DMatrixX* xmat)
{
  fmat->m = xmat->m;
  fmat->n = xmat->n;
  fmat->matPA = xmat->matPA;
}


/* -------------------------------------------------------------------
   Interface FMatrixGen
   ------------------------------------------------------------------- */

/* -------------------------------------------------------------------
   fmatrixgenDestroy
   ------------------------------------------------------------------- */
int fmatrixgenDestroy    (FMatrixGen *gmat)
{
#ifdef DEBUG
  assert (gmat);
#endif

  free (gmat);

  return TCL_OK;
}

/* -------------------------------------------------------------------
   fmatrixgenCreate
   ------------------------------------------------------------------- */
FMatrixGen* fmatrixgenCreate (FMatrix* fmat)
{
  FMatrixGen* gmat;

#ifdef DEBUG
  assert (fmat);
#endif

  if (!(gmat = (FMatrixGen*) malloc (sizeof(FMatrixGen)))) {
    ERROR("Out of memory!\n");
    return 0;
  }
  gmat->type = 0;
  gmat->entry.fm = fmat;

  return gmat;
}

/* -------------------------------------------------------------------
   fmatrixgenCreateX
   ------------------------------------------------------------------- */
FMatrixGen* fmatrixgenCreateX (FMatrixX* fmat)
{
  FMatrixGen* gmat;

#ifdef DEBUG
  assert (fmat);
#endif

  if (!(gmat = (FMatrixGen*) malloc (sizeof(FMatrixGen)))) {
    ERROR("Out of memory!\n");
    return 0;
  }
  gmat->type = 1;
  gmat->entry.fmx = fmat;

  return gmat;
}

/* -------------------------------------------------------------------
   fmatrixgenMask
   ------------------------------------------------------------------- */
int fmatrixgenMask (FMatrixGen* gmat,FMatrix* fmat)
{
#ifdef DEBUG
  assert (gmat);
  assert (fmat);
#endif

  gmat->type = 0;
  gmat->entry.fm = fmat;

  return TCL_OK;
}

/* -------------------------------------------------------------------
   fmatrixgenMaskX
   ------------------------------------------------------------------- */
int fmatrixgenMaskX (FMatrixGen* gmat,FMatrixX* fmat)
{
#ifdef DEBUG
  assert (gmat);
  assert (fmat);
#endif

  gmat->type = 1;
  gmat->entry.fmx = fmat;

  return TCL_OK;
}

/* -------------------------------------------------------------------
   fmatrixgenMulT
   ------------------------------------------------------------------- */
int fmatrixgenMulT (FMatrixGen* C,FMatrixGen* A,FMatrixGen* B)
{
  FMatrix helpa,helpb,helpc;
  FMatrix* pa,*pb,*pc;

#ifdef DEBUG
  assert (C);
  assert (A);
  assert (B);
#endif

  if ((C->type == 0 && A->type == 0 && C->entry.fm == A->entry.fm) ||
      (C->type == 1 && B->type == 1 && C->entry.fmx == B->entry.fmx)) {
    ERROR("Target must be different from sources!\n");
    return TCL_ERROR;
  }
  if (A->type == 0)
    pa = A->entry.fm;
  else {
    fmatx2mat (&helpa,A->entry.fmx);
    pa = &helpa;
  }
  if (B->type == 0)
    pb = B->entry.fm;
  else {
    fmatx2mat (&helpb,B->entry.fmx);
    pb = &helpb;
  }
  if (C->type == 0) {
    pc = C->entry.fm;
    if (fmatrixResize (pc,pa->m,pb->m) == TCL_ERROR) {
#ifdef DEBUG
      ERROR("<= called by\n");
#endif
      return TCL_ERROR;
    }
  } else {
    if (fmatrixxResize (C->entry.fmx,pa->m,pb->m) == TCL_ERROR) {
#ifdef DEBUG
      ERROR("<= called by\n");
#endif
      return TCL_ERROR;
    }
    fmatx2mat (&helpc,C->entry.fmx);
    pc = &helpc;
  }
  if (!fmatrixMulot (pc,pa,pb)) {
#ifdef DEBUG
    ERROR("<= called by\n");
#endif
    return TCL_ERROR;
  }

  return TCL_OK;
}

/* -------------------------------------------------------------------
   fmatrixgenMul
   ------------------------------------------------------------------- */
int fmatrixgenMul (FMatrixGen* C,FMatrixGen* A,FMatrixGen* B)
{
  FMatrix helpa,helpb,helpc;
  FMatrix* pa,*pb,*pc;

#ifdef DEBUG
  assert (C);
  assert (A);
  assert (B);
#endif

  if ((C->type == 0 && A->type == 0 && C->entry.fm == A->entry.fm) ||
      (C->type == 1 && B->type == 1 && C->entry.fmx == B->entry.fmx)) {
    ERROR("Target must be different from sources!\n");
    return TCL_ERROR;
  }
  if (A->type == 0)
    pa = A->entry.fm;
  else {
    fmatx2mat (&helpa,A->entry.fmx);
    pa = &helpa;
  }
  if (B->type == 0)
    pb = B->entry.fm;
  else {
    fmatx2mat (&helpb,B->entry.fmx);
    pb = &helpb;
  }
  if ((pa->n != pa->m) && (pa->n+1 != pa->m)) {
    ERROR("Dimensions don't fit!\n");
    return TCL_ERROR;
  }
  if (C->type == 0) {
    pc = C->entry.fm;
    if (fmatrixResize (pc,pa->m,pb->n) == TCL_ERROR) {
#ifdef DEBUG
      ERROR("<= called by\n");
#endif
      return TCL_ERROR;
    }
  } else {
    if (fmatrixxResize (C->entry.fmx,pa->m,pb->n) == TCL_ERROR) {
#ifdef DEBUG
      ERROR("<= called by\n");
#endif
      return TCL_ERROR;
    }
    fmatx2mat (&helpc,C->entry.fmx);
    pc = &helpc;
  }
  if (!fmatrixMul (pc,pa,pb)) {
#ifdef DEBUG
    ERROR("<= called by\n");
#endif
    return TCL_ERROR;
  }

  return TCL_OK;
}


/* -------------------------------------------------------------------
   Interface FMatrixX
   ------------------------------------------------------------------- */

/* -------------------------------------------------------------------
   fmatrixxDeinit
   ------------------------------------------------------------------- */
int fmatrixxDeinit (FMatrixX *fmat)
{
#ifdef DEBUG
  assert (fmat);
#endif

  if (fmat->buff_n) free (fmat->matPA[0]);
  if (fmat->matPA) free (fmat->matPA);
  fmat->matPA = 0;
  fmat->m = fmat->n = fmat->buff_m = fmat->buff_n = 0;

  return TCL_OK;
}

/* -------------------------------------------------------------------
   fmatrixxDestroy
   ------------------------------------------------------------------- */
int fmatrixxDestroy (FMatrixX *fmat)
{
#ifdef DEBUG
  assert (fmat);
#endif

  fmatrixxDeinit (fmat);
  free (fmat);

  return TCL_OK;
}

/* -------------------------------------------------------------------
   fmatrixxResize
   ------------------------------------------------------------------- */
int fmatrixxResize (FMatrixX *fmat, int m, int n)
{
  int j;
  FMatrixX copy;

#ifdef DEBUG
  assert (fmat);
#endif

  if ((fmat->m == m) && (fmat->n == n)) return TCL_OK;
  if (m <= 0 || n <= 0) {
    ERROR("Matrix must have positive dimensions!\n");
    return TCL_ERROR;
  }

  if ((n > fmat->buff_n) || (m > fmat->buff_m)) {
    /* Allocate larger buffers */
    if (!(copy.matPA = (float**) malloc (m * sizeof(float*)))) {
      ERROR("Out of memory!\n");
      return TCL_ERROR;
    }
    if (!(copy.matPA[0] = (float*) malloc (m*n * sizeof(float)))) {
      ERROR("Out of memory!\n");
      free (copy.matPA);
      return TCL_ERROR;
    }
    fmatrixxDeinit (fmat);
    copy.count = copy.m = copy.n = copy.buff_m = copy.buff_n = 0; 
    *fmat = copy;
    for (j = 1; j < m; j++)
      fmat->matPA[j] = fmat->matPA[j-1] + n;
    fmat->buff_m = m; fmat->buff_n = n;
  }
  fmat->m = m; fmat->n = n;

  return TCL_OK;
}

/* -------------------------------------------------------------------
   fmatrixxAdjustBuffer
   ------------------------------------------------------------------- */
int fmatrixxAdjustBuffer (FMatrixX* fmat)
{
  int m = fmat->m,n = fmat->n;

#ifdef DEBUG
  assert (fmat);
#endif

  fmatrixxDeinit (fmat);
  if (fmatrixxResize (fmat,m,n) == TCL_ERROR) {
#ifdef DEBUG
    ERROR("<= called by\n");
#endif
    return TCL_ERROR;
  }

  return TCL_OK;
}

/* -------------------------------------------------------------------
   fmatrixxCreate
   ------------------------------------------------------------------- */
FMatrixX* fmatrixxCreate (int m,int n)
{
  FMatrixX  *fmat;

  if (!(fmat = (FMatrixX*) malloc (sizeof(FMatrixX)))) {
    ERROR("Out of memory!\n");
    return 0;
  }
  fmat->matPA = 0;
  fmat->m = fmat->n = fmat->buff_m = fmat->buff_n = 0;
  if (fmatrixxResize (fmat,m,n) == TCL_ERROR) {
#ifdef DEBUG
    ERROR("<= called by\n");
#endif
    return 0;
  }

  return fmat;
}

/* -------------------------------------------------------------------
   fmatrixxAlias
   ------------------------------------------------------------------- */
int fmatrixxAlias (FMatrixX* refmat,FMatrixX* amat,int r1,int c1,
		      int r2,int c2)
{
  int m = r2-r1+1,n = c2-c1+1,j;
  float** copy;

#ifdef DEBUG
  assert (refmat);
  assert (amat);
#endif

  if ((m <= 0) || (n <= 0) || (r1 < 0) || (c1 < 0) || (r2 >= refmat->m) ||
      (c2 >= refmat->n)) {
    ERROR("Wrong coordinates!\n");
    return TCL_ERROR;
  }
  if (amat->buff_n) {
    ERROR("Argument must be alias matrix!\n");
    return TCL_ERROR;
  }
  if (amat->buff_m < m) {
    /* Allocate larger pointer array */
    if (!(copy = (float**) malloc (m * sizeof(float*)))) {
      ERROR("Out of memory!\n");
      return TCL_ERROR;
    }
    if (amat->matPA) free (amat->matPA);
    amat->matPA = copy;
    amat->buff_m = m;
  }
  amat->n = n; amat->m = m;
  for (j = 0; j < m; j++)
    amat->matPA[j] = refmat->matPA[j+r1] + c1;

  return TCL_OK;
}

/* -------------------------------------------------------------------
   fmatrixxAlias2
   ------------------------------------------------------------------- */
int fmatrixxAlias2 (FMatrix* refmat,FMatrixX* amat,int r1,int c1,
		    int r2,int c2)
{
  FMatrixX hmat;

#ifdef DEBUG
  assert (refmat);
  assert (amat);
#endif

  fmat2matx (&hmat,refmat);
  if (fmatrixxAlias (&hmat,amat,r1,c1,r2,c2) == TCL_ERROR) {
#ifdef DEBUG
    ERROR("<= called by\n");
#endif
    return TCL_ERROR;
  }

  return TCL_OK;
}

/* -------------------------------------------------------------------
   fmatrixxInitConst
   ------------------------------------------------------------------- */
int fmatrixxInitConst (FMatrixX* fmat,float initVal)
{
  FMatrix hmat;

#ifdef DEBUG
  assert (fmat);
#endif

  fmatx2mat (&hmat,fmat);
  return fmatrixInitConst (&hmat,initVal);
}

/* -------------------------------------------------------------------
   fmatrixxInitDiag
   ------------------------------------------------------------------- */
int fmatrixxInitDiag (FMatrixX* fmat,float initVal)
{
  FMatrix hmat;

#ifdef DEBUG
  assert (fmat);
#endif

  fmatx2mat (&hmat,fmat);
  return fmatrixInitDiag (&hmat,initVal);
}

/* -------------------------------------------------------------------
   fmatrixxInitRandom
   ------------------------------------------------------------------- */
int fmatrixxInitRandom (FMatrixX* fmat,float maxAbsVal)
{
  FMatrix hmat;

#ifdef DEBUG
  assert (fmat);
#endif

  fmatx2mat (&hmat,fmat);
  return fmatrixInitRandom (&hmat,maxAbsVal);
}

/* -------------------------------------------------------------------
   fmatrixxCopy
   ------------------------------------------------------------------- */
int fmatrixxCopy (FMatrixX* C,FMatrixX* A)
{
  register int i,j;

#ifdef DEBUG
  assert (C);
  assert (A);
#endif

  if (C==A) return TCL_OK;
  if (fmatrixxResize (C,A->m,A->n) == TCL_ERROR) {
#ifdef DEBUG
    ERROR("<= called by\n");
#endif
    return TCL_ERROR;
  }
  for (i=0; i<A->m; i++)
    for (j=0; j<A->n; j++)
      C->matPA[i][j] = A->matPA[i][j];

  return TCL_OK;
}

/* -------------------------------------------------------------------
   fmatrixxTrans
   ------------------------------------------------------------------- */
int fmatrixxTrans (FMatrixX* C,FMatrixX* A)
{
  register int i,j;

#ifdef DEBUG
  assert (C);
  assert (A);
#endif

  if (C==A) {
    ERROR("Arguments must be different objects!\n");
    return TCL_ERROR;
  }
  if (fmatrixxResize (C,A->n,A->m) == TCL_ERROR) {
#ifdef DEBUG
    ERROR("<= called by\n");
#endif
    return TCL_ERROR;
  }
  for (i=0; i<A->m; i++)
    for (j=0; j<A->n; j++)
      C->matPA[j][i] = A->matPA[i][j];

  return TCL_OK;
}

/* -------------------------------------------------------------------
   fmatrixxScale
   ------------------------------------------------------------------- */
int fmatrixxScale (FMatrixX* C,FMatrixX* A,float gamma)
{
  register int i,j;

#ifdef DEBUG
  assert (C);
  assert (A);
#endif

  if (fmatrixxResize (C,A->m,A->n) == TCL_ERROR) {
#ifdef DEBUG
    ERROR("<= called by\n");
#endif
    return TCL_ERROR;
  }
  for (i=0; i<A->m; i++)
    for (j=0; j<A->n; j++)
      C->matPA[i][j] = A->matPA[i][j] * gamma;

  return TCL_OK;
}

/* -------------------------------------------------------------------
   fmatrixxAdd
   ------------------------------------------------------------------- */
int fmatrixxAdd (FMatrixX* C,FMatrixX* A,FMatrixX* B)
{
  register int i,j;

#ifdef DEBUG
  assert (C);
  assert (A);
  assert (B);
#endif

  if ((A->m != B->m) || (A->n != B->n)) {
    ERROR("Dimensions don't fit!\n");
    return TCL_ERROR;
  }
  if (fmatrixxResize (C,A->m,A->n) == TCL_ERROR) {
#ifdef DEBUG
    ERROR("<= called by\n");
#endif
    return TCL_ERROR;
  }
  for (i=0; i<A->m; i++)
    for (j=0; j<A->n; j++)
      C->matPA[i][j] = A->matPA[i][j] + B->matPA[i][j];

  return TCL_OK;
}

/* -------------------------------------------------------------------
   fmatrixxSub
   ------------------------------------------------------------------- */
int fmatrixxSub (FMatrixX* C,FMatrixX* A,FMatrixX* B)
{
  register int i,j;

#ifdef DEBUG
  assert (C);
  assert (A);
  assert (B);
#endif

  if ((A->m != B->m) || (A->n != B->n)) {
    ERROR("Dimensions don't fit!\n");
    return TCL_ERROR;
  }
  if (fmatrixxResize (C,A->m,A->n) == TCL_ERROR) {
#ifdef DEBUG
    ERROR("<= called by\n");
#endif
    return TCL_ERROR;
  }
  for (i=0; i<A->m; i++)
    for (j=0; j<A->n; j++)
      C->matPA[i][j] = A->matPA[i][j] - B->matPA[i][j];

  return TCL_OK;
}

/* -------------------------------------------------------------------
   fmatrixxProd
   ------------------------------------------------------------------- */
int fmatrixxProd (FMatrixX* C,FMatrixX* A,FMatrixX* B)
{
  register int i,j;

#ifdef DEBUG
  assert (C);
  assert (A);
  assert (B);
#endif

  if ((A->m != B->m) || (A->n != B->n)) {
    ERROR("Dimensions don't fit!\n");
    return TCL_ERROR;
  }
  if (fmatrixxResize (C,A->m,A->n) == TCL_ERROR) {
#ifdef DEBUG
    ERROR("<= called by\n");
#endif
    return TCL_ERROR;
  }
  for (i=0; i<A->m; i++)
    for (j=0; j<A->n; j++)
      C->matPA[i][j] = A->matPA[i][j] * B->matPA[i][j];

  return TCL_OK;
}

/* -------------------------------------------------------------------
   fmatrixxDet
   ------------------------------------------------------------------- */
int fmatrixxDet (FMatrixX* A,double* logdet,double* sign)
{
  FMatrix hmat;

#ifdef DEBUG
  assert (A);
  assert (logdet);
  assert (sign);
#endif

  fmatx2mat (&hmat,A);
  return fmatrixExtDet (&hmat,logdet,sign);
}

/* -------------------------------------------------------------------
   fmatrixxPuts
   ------------------------------------------------------------------- */
int fmatrixxPuts (FMatrixX* fmat,FILE* fp)
{
  register int i,j;

#ifdef DEBUG
  assert (fmat);
#endif

  if (!fp) {
    for (i=0; i<fmat->m; i++) {
      itfAppendResult ("{ ");
      for (j=0; j<fmat->n; j++)
	itfAppendResult ("%12.7f ",fmat->matPA[i][j]);
      itfAppendResult ("}\n");
    }
  } else {
    for (i = 0; i < fmat->m; i++) {
      for (j = 0; j < fmat->n; j++)
	fprintf (fp,"%12.7f ",fmat->matPA[i][j]);
      fprintf (fp,"\n");
    }
  }

  return TCL_OK;
}

/* -------------------------------------------------------------------
   fmatrixx2fmatrix
   ------------------------------------------------------------------- */
int fmatrixx2fmatrix (FMatrix* C,FMatrixX* A)
{
  register int i,j;

#ifdef DEBUG
  assert (C);
  assert (A);
#endif

  if (fmatrixResize (C,A->m,A->n) == TCL_ERROR) {
#ifdef DEBUG
    ERROR("<= called by\n");
#endif
    return TCL_ERROR;
  }
  for (i = 0; i < A->m; i++)
    for (j = 0; j < A->n; j++)
      C->matPA[i][j] = A->matPA[i][j];

  return TCL_OK;
}

/* -------------------------------------------------------------------
   fmatrix2fmatrixx
   ------------------------------------------------------------------- */
int fmatrix2fmatrixx (FMatrixX* C,FMatrix* A)
{
  register int i,j;

#ifdef DEBUG
  assert (C);
  assert (A);
#endif

  if (fmatrixxResize (C,A->m,A->n) == TCL_ERROR) {
#ifdef DEBUG
    ERROR("<= called by\n");
#endif
    return TCL_ERROR;
  }
  for (i = 0; i < A->m; i++)
    for (j = 0; j < A->n; j++)
      C->matPA[i][j] = A->matPA[i][j];

  return TCL_OK;
}


/* -------------------------------------------------------------------
   Interface DMatrixGen
   ------------------------------------------------------------------- */

/* -------------------------------------------------------------------
   dmatrixgenDestroy
   ------------------------------------------------------------------- */
int dmatrixgenDestroy    (DMatrixGen *gmat)
{
#ifdef DEBUG
  assert (gmat);
#endif

  free (gmat);

  return TCL_OK;
}

/* -------------------------------------------------------------------
   dmatrixgenCreate
   ------------------------------------------------------------------- */
DMatrixGen* dmatrixgenCreate (DMatrix* fmat)
{
  DMatrixGen* gmat;

#ifdef DEBUG
  assert (fmat);
#endif

  if (!(gmat = (DMatrixGen*) malloc (sizeof(DMatrixGen)))) {
    ERROR("Out of memory!\n");
    return 0;
  }
  gmat->type = 0;
  gmat->entry.fm = fmat;

  return gmat;
}

/* -------------------------------------------------------------------
   dmatrixgenCreateX
   ------------------------------------------------------------------- */
DMatrixGen* dmatrixgenCreateX (DMatrixX* fmat)
{
  DMatrixGen* gmat;

#ifdef DEBUG
  assert (fmat);
#endif

  if (!(gmat = (DMatrixGen*) malloc (sizeof(DMatrixGen)))) {
    ERROR("Out of memory!\n");
    return 0;
  }
  gmat->type = 1;
  gmat->entry.fmx = fmat;

  return gmat;
}

/* -------------------------------------------------------------------
   dmatrixgenMask
   ------------------------------------------------------------------- */
int dmatrixgenMask (DMatrixGen* gmat,DMatrix* fmat)
{
#ifdef DEBUG
  assert (gmat);
  assert (fmat);
#endif

  gmat->type = 0;
  gmat->entry.fm = fmat;

  return TCL_OK;
}

/* -------------------------------------------------------------------
   dmatrixgenMaskX
   ------------------------------------------------------------------- */
int dmatrixgenMaskX (DMatrixGen* gmat,DMatrixX* fmat)
{
#ifdef DEBUG
  assert (gmat);
  assert (fmat);
#endif

  gmat->type = 1;
  gmat->entry.fmx = fmat;

  return TCL_OK;
}

/* -------------------------------------------------------------------
   dmatrixgenMulT
   ------------------------------------------------------------------- */
int dmatrixgenMulT (DMatrixGen* C,DMatrixGen* A,DMatrixGen* B)
{
  DMatrix helpa,helpb,helpc;
  DMatrix* pa,*pb,*pc;

#ifdef DEBUG
  assert (C);
  assert (A);
  assert (B);
#endif

  if ((C->type == 0 && A->type == 0 && C->entry.fm == A->entry.fm) ||
      (C->type == 1 && B->type == 1 && C->entry.fmx == B->entry.fmx)) {
    ERROR("Target must be different from sources!\n");
    return TCL_ERROR;
  }
  if (A->type == 0)
    pa = A->entry.fm;
  else {
    dmatx2mat (&helpa,A->entry.fmx);
    pa = &helpa;
  }
  if (B->type == 0)
    pb = B->entry.fm;
  else {
    dmatx2mat (&helpb,B->entry.fmx);
    pb = &helpb;
  }
  if (C->type == 0) {
    pc = C->entry.fm;
    if (dmatrixResize (pc,pa->m,pb->m) == TCL_ERROR) {
#ifdef DEBUG
      ERROR("<= called by\n");
#endif
      return TCL_ERROR;
    }
  } else {
    if (dmatrixxResize (C->entry.fmx,pa->m,pb->m) == TCL_ERROR) {
#ifdef DEBUG
      ERROR("<= called by\n");
#endif
      return TCL_ERROR;
    }
    dmatx2mat (&helpc,C->entry.fmx);
    pc = &helpc;
  }
  if (!dmatrixMulot (pc,pa,pb)) {
#ifdef DEBUG
    ERROR("<= called by\n");
#endif
    return TCL_ERROR;
  }

  return TCL_OK;
}

/* -------------------------------------------------------------------
   dmatrixgenMul
   ------------------------------------------------------------------- */
int dmatrixgenMul (DMatrixGen* C,DMatrixGen* A,DMatrixGen* B)
{
  DMatrix helpa,helpb,helpc;
  DMatrix* pa,*pb,*pc;

#ifdef DEBUG
  assert (C);
  assert (A);
  assert (B);
#endif

  if ((C->type == 0 && A->type == 0 && C->entry.fm == A->entry.fm) ||
      (C->type == 1 && B->type == 1 && C->entry.fmx == B->entry.fmx)) {
    ERROR("Target must be different from sources!\n");
    return TCL_ERROR;
  }
  if (A->type == 0)
    pa = A->entry.fm;
  else {
    dmatx2mat (&helpa,A->entry.fmx);
    pa = &helpa;
  }
  if (B->type == 0)
    pb = B->entry.fm;
  else {
    dmatx2mat (&helpb,B->entry.fmx);
    pb = &helpb;
  }
  if ((pa->n != pa->m) && (pa->n+1 != pa->m)) {
    ERROR("Dimensions don't fit!\n");
    return TCL_ERROR;
  }
  if (C->type == 0) {
    pc = C->entry.fm;
    if (dmatrixResize (pc,pa->m,pb->n) == TCL_ERROR) {
#ifdef DEBUG
      ERROR("<= called by\n");
#endif
      return TCL_ERROR;
    }
  } else {
    if (dmatrixxResize (C->entry.fmx,pa->m,pb->n) == TCL_ERROR) {
#ifdef DEBUG
      ERROR("<= called by\n");
#endif
      return TCL_ERROR;
    }
    dmatx2mat (&helpc,C->entry.fmx);
    pc = &helpc;
  }
  if (!dmatrixMul (pc,pa,pb)) {
#ifdef DEBUG
    ERROR("<= called by\n");
#endif
    return TCL_ERROR;
  }

  return TCL_OK;
}

/* -------------------------------------------------------------------
   dmatrixgenInv
   ------------------------------------------------------------------- */
int dmatrixgenInv (DMatrixGen* C,DMatrixGen* A)
{
  DMatrix helpa,helpc;
  DMatrix* pa,*pc;

#ifdef DEBUG
  assert (C);
  assert (A);
#endif

  if (A->type == 0)
    pa = A->entry.fm;
  else {
    dmatx2mat (&helpa,A->entry.fmx);
    pa = &helpa;
  }
  if (C->type == 0) {
    pc = C->entry.fm;
    if (dmatrixResize (pc,pa->m,pa->n) == TCL_ERROR) {
#ifdef DEBUG
      ERROR("<= called by\n");
#endif
      return TCL_ERROR;
    }
  } else {
    if (dmatrixxResize (C->entry.fmx,pa->m,pa->n) == TCL_ERROR) {
#ifdef DEBUG
      ERROR("<= called by\n");
#endif
      return TCL_ERROR;
    }
    dmatx2mat (&helpc,C->entry.fmx);
    pc = &helpc;
  }
  if (!dmatrixInv (pc,pa)) {
#ifdef DEBUG
    ERROR("<= called by\n");
#endif
    return TCL_ERROR;
  }

  return TCL_OK;
}


/* -------------------------------------------------------------------
   Interface DMatrixX
   ------------------------------------------------------------------- */

/* -------------------------------------------------------------------
   dmatrixxDeinit
   ------------------------------------------------------------------- */
int dmatrixxDeinit (DMatrixX *fmat)
{
#ifdef DEBUG
  assert (fmat);
#endif

  if (fmat->buff_n) free (fmat->matPA[0]);
  if (fmat->matPA) free (fmat->matPA);
  fmat->matPA = 0;
  fmat->m = fmat->n = fmat->buff_m = fmat->buff_n = 0;

  return TCL_OK;
}

/* -------------------------------------------------------------------
   dmatrixxDestroy
   ------------------------------------------------------------------- */
int dmatrixxDestroy (DMatrixX *fmat)
{
#ifdef DEBUG
  assert (fmat);
#endif

  dmatrixxDeinit (fmat);
  free (fmat);

  return TCL_OK;
}

/* -------------------------------------------------------------------
   dmatrixxResize
   ------------------------------------------------------------------- */
int dmatrixxResize (DMatrixX *fmat, int m, int n)
{
  int j;
  DMatrixX copy;

#ifdef DEBUG
  assert (fmat);
#endif

  if ((fmat->m == m) && (fmat->n == n)) return TCL_OK;
  if (m <= 0 || n <= 0) {
    ERROR("Matrix must have positive dimensions!\n");
    return TCL_ERROR;
  }

  if ((n > fmat->buff_n) || (m > fmat->buff_m)) {
    /* Allocate larger buffers */
    if (!(copy.matPA = (double**) malloc (m * sizeof(double*)))) {
      ERROR("Out of memory!\n");
      return TCL_ERROR;
    }
    if (!(copy.matPA[0] = (double*) malloc (m*n * sizeof(double)))) {
      ERROR("Out of memory!\n");
      free (copy.matPA);
      return TCL_ERROR;
    }
    dmatrixxDeinit (fmat);
    copy.count = copy.m = copy.n = copy.buff_m = copy.buff_n = 0;
    *fmat = copy;
    for (j = 1; j < m; j++)
      fmat->matPA[j] = fmat->matPA[j-1] + n;
    fmat->buff_m = m; fmat->buff_n = n;
  }
  fmat->m = m; fmat->n = n;

  return TCL_OK;
}

/* -------------------------------------------------------------------
   dmatrixxAdjustBuffer
   ------------------------------------------------------------------- */
int dmatrixxAdjustBuffer (DMatrixX* fmat)
{
  int m = fmat->m,n = fmat->n;

#ifdef DEBUG
  assert (fmat);
#endif

  dmatrixxDeinit (fmat);
  if (dmatrixxResize (fmat,m,n) == TCL_ERROR) {
#ifdef DEBUG
    ERROR("<= called by\n");
#endif
    return TCL_ERROR;
  }

  return TCL_OK;
}

/* -------------------------------------------------------------------
   dmatrixxCreate
   ------------------------------------------------------------------- */
DMatrixX* dmatrixxCreate (int m,int n)
{
  DMatrixX  *fmat;

  if (!(fmat = (DMatrixX*) malloc (sizeof(DMatrixX)))) {
    ERROR("Out of memory!\n");
    return 0;
  }
  fmat->matPA = 0;
  fmat->m = fmat->n = fmat->buff_m = fmat->buff_n = 0;
  if (dmatrixxResize (fmat,m,n) == TCL_ERROR) {
#ifdef DEBUG
    ERROR("<= called by\n");
#endif
    return 0;
  }

  return fmat;
}

/* -------------------------------------------------------------------
   dmatrixxAlias
   ------------------------------------------------------------------- */
int dmatrixxAlias (DMatrixX* refmat,DMatrixX* amat,int r1,int c1,
		      int r2,int c2)
{
  int m = r2-r1+1,n = c2-c1+1,j;
  double** copy;

#ifdef DEBUG
  assert (refmat);
  assert (amat);
#endif

  if ((m <= 0) || (n <= 0) || (r1 < 0) || (c1 < 0) || (r2 >= refmat->m) ||
      (c2 >= refmat->n)) {
    ERROR("Wrong coordinates!\n");
    return TCL_ERROR;
  }
  if (amat->buff_n) {
    ERROR("Argument must be alias matrix!\n");
    return TCL_ERROR;
  }
  if (amat->buff_m < m) {
    /* Allocate larger pointer array */
    if (!(copy = (double**) malloc (m * sizeof(double*)))) {
      ERROR("Out of memory!\n");
      return TCL_ERROR;
    }
    if (amat->matPA) free (amat->matPA);
    amat->matPA = copy;
    amat->buff_m = m;
  }
  amat->n = n; amat->m = m;
  for (j = 0; j < m; j++)
    amat->matPA[j] = refmat->matPA[j+r1] + c1;

  return TCL_OK;
}

/* -------------------------------------------------------------------
   dmatrixxAlias2
   ------------------------------------------------------------------- */
int dmatrixxAlias2 (DMatrix* refmat,DMatrixX* amat,int r1,int c1,
		    int r2,int c2)
{
  DMatrixX hmat;

#ifdef DEBUG
  assert (refmat);
  assert (amat);
#endif

  dmat2matx (&hmat,refmat);
  if (dmatrixxAlias (&hmat,amat,r1,c1,r2,c2) == TCL_ERROR) {
#ifdef DEBUG
    ERROR("<= called by\n");
#endif
    return TCL_ERROR;
  }

  return TCL_OK;
}

/* -------------------------------------------------------------------
   dmatrixxInitConst
   ------------------------------------------------------------------- */
int dmatrixxInitConst (DMatrixX* fmat,double initVal)
{
  DMatrix hmat;

#ifdef DEBUG
  assert (fmat);
#endif

  dmatx2mat (&hmat,fmat);
  return dmatrixInitConst (&hmat,initVal);
}

/* -------------------------------------------------------------------
   dmatrixxInitDiag
   ------------------------------------------------------------------- */
int dmatrixxInitDiag (DMatrixX* fmat,double initVal)
{
  DMatrix hmat;

#ifdef DEBUG
  assert (fmat);
#endif

  dmatx2mat (&hmat,fmat);
  return dmatrixInitDiag (&hmat,initVal);
}

/* -------------------------------------------------------------------
   dmatrixxInitRandom
   ------------------------------------------------------------------- */
int dmatrixxInitRandom (DMatrixX* fmat,double maxAbsVal)
{
  DMatrix hmat;

#ifdef DEBUG
  assert (fmat);
#endif

  dmatx2mat (&hmat,fmat);
  return dmatrixInitRandom (&hmat,maxAbsVal);
}

/* -------------------------------------------------------------------
   dmatrixxCopy
   ------------------------------------------------------------------- */
int dmatrixxCopy (DMatrixX* C,DMatrixX* A)
{
  register int i,j;

#ifdef DEBUG
  assert (C);
  assert (A);
#endif

  if (C==A) return TCL_OK;
  if (dmatrixxResize (C,A->m,A->n) == TCL_ERROR) {
#ifdef DEBUG
    ERROR("<= called by\n");
#endif
    return TCL_ERROR;
  }
  for (i=0; i<A->m; i++)
    for (j=0; j<A->n; j++)
      C->matPA[i][j] = A->matPA[i][j];

  return TCL_OK;
}

/* -------------------------------------------------------------------
   dmatrixxTrans
   ------------------------------------------------------------------- */
int dmatrixxTrans (DMatrixX* C,DMatrixX* A)
{
  register int i,j;

#ifdef DEBUG
  assert (C);
  assert (A);
#endif

  if (C==A) {
    ERROR("Arguments must be different objects!\n");
    return TCL_ERROR;
  }
  if (dmatrixxResize (C,A->n,A->m) == TCL_ERROR) {
#ifdef DEBUG
    ERROR("<= called by\n");
#endif
    return TCL_ERROR;
  }
  for (i=0; i<A->m; i++)
    for (j=0; j<A->n; j++)
      C->matPA[j][i] = A->matPA[i][j];

  return TCL_OK;
}

/* -------------------------------------------------------------------
   dmatrixxScale
   ------------------------------------------------------------------- */
int dmatrixxScale (DMatrixX* C,DMatrixX* A,double gamma)
{
  register int i,j;

#ifdef DEBUG
  assert (C);
  assert (A);
#endif

  if (dmatrixxResize (C,A->m,A->n) == TCL_ERROR) {
#ifdef DEBUG
    ERROR("<= called by\n");
#endif
    return TCL_ERROR;
  }
  for (i=0; i<A->m; i++)
    for (j=0; j<A->n; j++)
      C->matPA[i][j] = A->matPA[i][j] * gamma;

  return TCL_OK;
}

/* -------------------------------------------------------------------
   dmatrixxAdd
   ------------------------------------------------------------------- */
int dmatrixxAdd (DMatrixX* C,DMatrixX* A,DMatrixX* B)
{
  register int i,j;

#ifdef DEBUG
  assert (C);
  assert (A);
  assert (B);
#endif

  if ((A->m != B->m) || (A->n != B->n)) {
    ERROR("Dimensions don't fit!\n");
    return TCL_ERROR;
  }
  if (dmatrixxResize (C,A->m,A->n) == TCL_ERROR) {
#ifdef DEBUG
    ERROR("<= called by\n");
#endif
    return TCL_ERROR;
  }
  for (i=0; i<A->m; i++)
    for (j=0; j<A->n; j++)
      C->matPA[i][j] = A->matPA[i][j] + B->matPA[i][j];

  return TCL_OK;
}

/* -------------------------------------------------------------------
   dmatrixxSub
   ------------------------------------------------------------------- */
int dmatrixxSub (DMatrixX* C,DMatrixX* A,DMatrixX* B)
{
  register int i,j;

#ifdef DEBUG
  assert (C);
  assert (A);
  assert (B);
#endif

  if ((A->m != B->m) || (A->n != B->n)) {
    ERROR("Dimensions don't fit!\n");
    return TCL_ERROR;
  }
  if (dmatrixxResize (C,A->m,A->n) == TCL_ERROR) {
#ifdef DEBUG
    ERROR("<= called by\n");
#endif
    return TCL_ERROR;
  }
  for (i=0; i<A->m; i++)
    for (j=0; j<A->n; j++)
      C->matPA[i][j] = A->matPA[i][j] - B->matPA[i][j];

  return TCL_OK;
}

/* -------------------------------------------------------------------
   dmatrixxProd
   ------------------------------------------------------------------- */
int dmatrixxProd (DMatrixX* C,DMatrixX* A,DMatrixX* B)
{
  register int i,j;

#ifdef DEBUG
  assert (C);
  assert (A);
  assert (B);
#endif

  if ((A->m != B->m) || (A->n != B->n)) {
    ERROR("Dimensions don't fit!\n");
    return TCL_ERROR;
  }
  if (dmatrixxResize (C,A->m,A->n) == TCL_ERROR) {
#ifdef DEBUG
    ERROR("<= called by\n");
#endif
    return TCL_ERROR;
  }
  for (i=0; i<A->m; i++)
    for (j=0; j<A->n; j++)
      C->matPA[i][j] = A->matPA[i][j] * B->matPA[i][j];

  return TCL_OK;
}

/* -------------------------------------------------------------------
   dmatrixxDet
   ------------------------------------------------------------------- */
int dmatrixxDet (DMatrixX* A,double* logdet,double* sign)
{
  DMatrix hmat;

#ifdef DEBUG
  assert (A);
  assert (logdet);
  assert (sign);
#endif

  dmatx2mat (&hmat,A);
  return dmatrixExtDet (&hmat,logdet,sign);
}

/* -------------------------------------------------------------------
   dmatrixxMinmax
   ------------------------------------------------------------------- */
int dmatrixxMinmax (DMatrixX* A,double* min,double* max)
{
  DMatrix hmat;

#ifdef DEBUG
  assert (A);
  assert (min);
  assert (max);
#endif

  dmatx2mat (&hmat,A);
  return dmatrixMinmax (&hmat,min,max);
}

/* -------------------------------------------------------------------
   dmatrixxInv
   ------------------------------------------------------------------- */
int dmatrixxInv (DMatrixX* C,DMatrixX* A)
{
  DMatrix helpa,helpc;

#ifdef DEBUG
  assert (A);
  assert (C);
#endif

  if (dmatrixxResize (C,A->m,A->n) == TCL_ERROR) {
#ifdef DEBUG
    ERROR("<= called by\n");
#endif
    return TCL_ERROR;
  }

  dmatx2mat (&helpa,A);
  dmatx2mat (&helpc,C);
  if (dmatrixInv (&helpc,&helpa) == 0) return TCL_ERROR;
  else return TCL_OK;
}

/* -------------------------------------------------------------------
   dmatrixxPuts
   ------------------------------------------------------------------- */
int dmatrixxPuts (DMatrixX* fmat,FILE* fp)
{
  register int i,j;

#ifdef DEBUG
  assert (fmat);
#endif

  if (!fp) {
    for (i=0; i<fmat->m; i++) {
      itfAppendResult ("{ ");
      for (j=0; j<fmat->n; j++)
	itfAppendResult ("%12.7f ",fmat->matPA[i][j]);
      itfAppendResult ("}\n");
    }
  } else {
    for (i = 0; i < fmat->m; i++) {
      for (j = 0; j < fmat->n; j++)
	fprintf (fp,"%12.7f ",fmat->matPA[i][j]);
      fprintf (fp,"\n");
    }
  }

  return TCL_OK;
}

/* -------------------------------------------------------------------
   dmatrixx2dmatrix
   ------------------------------------------------------------------- */
int dmatrixx2dmatrix (DMatrix* C,DMatrixX* A)
{
  register int i,j;

#ifdef DEBUG
  assert (C);
  assert (A);
#endif

  if (dmatrixResize (C,A->m,A->n) == TCL_ERROR) {
#ifdef DEBUG
    ERROR("<= called by\n");
#endif
    return TCL_ERROR;
  }
  for (i = 0; i < A->m; i++)
    for (j = 0; j < A->n; j++)
      C->matPA[i][j] = A->matPA[i][j];

  return TCL_OK;
}

/* -------------------------------------------------------------------
   dmatrix2dmatrix
   ------------------------------------------------------------------- */
int dmatrix2dmatrixx (DMatrixX* C,DMatrix* A)
{
  register int i,j;

#ifdef DEBUG
  assert (C);
  assert (A);
#endif

  if (dmatrixxResize (C,A->m,A->n) == TCL_ERROR) {
#ifdef DEBUG
    ERROR("<= called by\n");
#endif
    return TCL_ERROR;
  }
  for (i = 0; i < A->m; i++)
    for (j = 0; j < A->n; j++)
      C->matPA[i][j] = A->matPA[i][j];

  return TCL_OK;
}
