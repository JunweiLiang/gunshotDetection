/* -------------------------------------------------------------------
   Extended matrix classes
   Matthias Seeger
   -------------------------------------------------------------------
   Implements FMatrixX, DMatrixX (internal classes). Differences to
   FMatrix/DMatrix:
   - resizing of matrix will allocate new memory only if the old
     buffer is too small
   - alias matrices which have no buffer for their rows, but point
     into buffers of other matrices (very nice for windowing another
     matrix)
   -------------------------------------------------------------------
   C header file.
   Last modified: Jan. 20, 1998.
   ------------------------------------------------------------------- */

#ifdef __cplusplus
extern "C" {
#endif

#ifndef __matrix_ext__
#define __matrix_ext__

/* Janus interface */
#include "itf.h"
#include "matrix.h"

#include <stdio.h>

/* --- Exported datatypes --- */

/* -------------------------------------------------------------------
   FMatrixX
   Alias matrices have 'buff_n' = 0. 'buff_m', 'buff_n' hold the
   buffer dimensions.
   ------------------------------------------------------------------- */
typedef struct {
  float**   matPA;
  int       n;
  int       m;
  float     count;
  int       buff_n;
  int       buff_m;
} FMatrixX;

/* -------------------------------------------------------------------
   FMatrixGen
   Polymorphical type. Points either to an 'FMatrix' or an 'FMatrixX'
   object.
   ------------------------------------------------------------------- */
typedef struct {
  char type;  /* 0: FMatrix, 1: FMatrixX */
  union {
    FMatrix* fm;
    FMatrixX* fmx;
  } entry;
} FMatrixGen;

/* -------------------------------------------------------------------
   DMatrixX
   Alias matrices have 'buff_n' = 0. 'buff_m', 'buff_n' hold the
   buffer dimensions.
   ------------------------------------------------------------------- */
typedef struct {
  double** matPA;
  int       n;
  int       m;
  float     count;  /* ??? */
  int       buff_n;
  int       buff_m;
} DMatrixX;

/* -------------------------------------------------------------------
   DMatrixGen
   Polymorphical type. Points either to an 'DMatrix' or an 'DMatrixX'
   object.
   ------------------------------------------------------------------- */
typedef struct {
  char type;  /* 0: DMatrix, 1: DMatrixX */
  union {
    DMatrix* fm;
    DMatrixX* fmx;
  } entry;
} DMatrixGen;


/* -------------------------------------------------------------------
   Interface FMatrixGen
   ------------------------------------------------------------------- */

int        fmatrixgenDestroy  (FMatrixGen *gmat);
FMatrixGen* fmatrixgenCreate  (FMatrix* fmat);
FMatrixGen* fmatrixgenCreateX (FMatrixX* fmat);
int        fmatrixgenMask     (FMatrixGen* gmat,FMatrix* fmat);
int        fmatrixgenMaskX    (FMatrixGen* gmat,FMatrixX* fmat);
int        fmatrixgenMulT     (FMatrixGen* C,FMatrixGen* A,FMatrixGen* B);
int        fmatrixgenMul      (FMatrixGen* C,FMatrixGen* A,FMatrixGen* B);


/* -------------------------------------------------------------------
   Interface FMatrixX
   ------------------------------------------------------------------- */

int        fmatrixxDeinit     (FMatrixX *fmat);
int        fmatrixxDestroy    (FMatrixX *fmat);

/* Attention: The matrix entries will be left undefined! If 'fmat' is
   an alias matrix, it will become ordinary, the row pointers will be lost!
   We never decrease the buffer size here. You can use 'fmatrixxAdjustBuffer'
   to do that. */
int        fmatrixxResize     (FMatrixX *fmat,int m,int n);

/* Adjust buffer size to exactly fit the current dimensions m,n.
   Attention: This will leave the entries undefined! */
int        fmatrixxAdjustBuffer (FMatrixX*);

/* Attention: The matrix entries will be left undefined!
   Note that alias matrices may be created by:
      amat = fmatrixxCreate (0,0);
      fmatrixxAlias (refmat,amat,uprow,leftcol,lowrow,rightcol);
   (see fmatrixxAlias). */
FMatrixX*  fmatrixxCreate     (int m,int n);

/* Sets alias matrix Arg2 to submatrix (Arg3,Arg4) - (Arg5,Arg6) of
   matrix Arg1. */
int        fmatrixxAlias      (FMatrixX*,FMatrixX*,int,int,int,int);

/* Same as 'fmatrixxAlias', but Arg1 is a FMatrix. */
int        fmatrixxAlias2     (FMatrix*,FMatrixX*,int,int,int,int);

int        fmatrixxInitConst  (FMatrixX *imat, float initVal);
int        fmatrixxInitDiag   (FMatrixX *imat, float initVal);
int        fmatrixxInitRandom (FMatrixX *imat, float maxAbsVal);
int        fmatrixxCopy       (FMatrixX *C, FMatrixX *A);

/* Arg1 and Arg2 must NOT be the same objects (unlike 'fmatrixTrans')! */
int fmatrixxTrans (FMatrixX* C,FMatrixX* A);

int        fmatrixxScale      (FMatrixX *C, FMatrixX *A, float gamma);
int        fmatrixxAdd        (FMatrixX *C, FMatrixX *A,
				  FMatrixX *B);
int        fmatrixxSub        (FMatrixX *C, FMatrixX *A,
				  FMatrixX *B);
int        fmatrixxProd       (FMatrixX *C, FMatrixX *A,
				  FMatrixX *B);
int        fmatrixxDet        (FMatrixX *A, double *logdet,
				  double *sign);

/* If 'fp' = 0, the target is the itf result string. */
int        fmatrixxPuts       (FMatrixX* fmat,FILE* fp);

int        fmatrixx2fmatrix   (FMatrix* C,FMatrixX* A);
int        fmatrix2fmatrixx   (FMatrixX* C,FMatrix* A);


/* -------------------------------------------------------------------
   Interface DMatrixGen
   ------------------------------------------------------------------- */

int        dmatrixgenDestroy  (DMatrixGen *gmat);
DMatrixGen* dmatrixgenCreate  (DMatrix* dmat);
DMatrixGen* dmatrixgenCreateX (DMatrixX* dmat);
int        dmatrixgenMask     (DMatrixGen* gmat,DMatrix* dmat);
int        dmatrixgenMaskX    (DMatrixGen* gmat,DMatrixX* dmat);
int        dmatrixgenMulT     (DMatrixGen* C,DMatrixGen* A,DMatrixGen* B);
int        dmatrixgenMul      (DMatrixGen* C,DMatrixGen* A,DMatrixGen* B);


/* -------------------------------------------------------------------
   Interface DMatrixX
   ------------------------------------------------------------------- */

int        dmatrixxDeinit     (DMatrixX *fmat);
int        dmatrixxDestroy    (DMatrixX *fmat);

/* Attention: The matrix entries will be left undefined! If 'fmat' is
   an alias matrix, it will become ordinary, the row pointers will be lost!
   We never decrease the buffer size here. You can use 'dmatrixxAdjustBuffer'
   to do that. */
int        dmatrixxResize     (DMatrixX *fmat,int m,int n);

/* Adjust buffer size to exactly fit the current dimensions m,n.
   Attention: This will leave the entries undefined! */
int        dmatrixxAdjustBuffer (DMatrixX*);

/* Attention: The matrix entries will be left undefined!
   Note that alias matrices may be created by:
      amat = dmatrixxCreate (0,0);
      dmatrixxAlias (refmat,amat,uprow,leftcol,lowrow,rightcol);
   (see dmatrixxAlias). */
DMatrixX*  dmatrixxCreate     (int m,int n);

/* Sets alias matrix Arg2 to submatrix (Arg3,Arg4) - (Arg5,Arg6) of
   matrix Arg1. */
int        dmatrixxAlias      (DMatrixX*,DMatrixX*,int,int,int,int);

/* Same as 'dmatrixxAlias', but Arg1 is a DMatrix. */
int        dmatrixxAlias2     (DMatrix*,DMatrixX*,int,int,int,int);

int        dmatrixxInitConst  (DMatrixX *imat, double initVal);
int        dmatrixxInitDiag   (DMatrixX *imat, double initVal);
int        dmatrixxInitRandom (DMatrixX *imat, double maxAbsVal);
int        dmatrixxCopy       (DMatrixX *C, DMatrixX *A);

/* Arg1 and Arg2 must NOT be the same objects (unlike 'DMatrixTrans')! */
int dmatrixxTrans (DMatrixX* C,DMatrixX* A);

int        dmatrixxScale      (DMatrixX *C, DMatrixX *A, double gamma);
int        dmatrixxAdd        (DMatrixX *C, DMatrixX *A,
				  DMatrixX *B);
int        dmatrixxSub        (DMatrixX *C, DMatrixX *A,
				  DMatrixX *B);
int        dmatrixxProd       (DMatrixX *C, DMatrixX *A,
				  DMatrixX *B);
int        dmatrixxDet        (DMatrixX *A, double *logdet,
				  double *sign);
int        dmatrixxMinmax     (DMatrixX* A,double* min,double* max);
int        dmatrixxInv        (DMatrixX* C,DMatrixX* A);

/* If 'fp' = 0, the target is the itf result string. */
int        dmatrixxPuts       (DMatrixX* fmat,FILE* fp);

int        dmatrixx2DMatrix   (DMatrix* C,DMatrixX* A);
int        DMatrix2dmatrixx   (DMatrixX* C,DMatrix* A);
int        dmatrixgenInv (DMatrixGen* C,DMatrixGen* A);

#endif

#ifdef __cplusplus
}
#endif
