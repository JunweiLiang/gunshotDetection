/* ========================================================================
    JANUS      Speech- to Speech Translation
               ------------------------------------------------------------
               Object: Optimized Matrix Operations for Neural Nets
               ------------------------------------------------------------

    Author  :  Juergen Fritsch
    Module  :  ffmat.h
    Date    :  $Id: ffmat.h 2390 2003-08-14 11:20:32Z fuegen $
    Remarks :  To avoid naming conflicts with matrix.[ch], all routines
               identifiers in this module start with a single underscore

   ========================================================================

    This software was developed by

    the  University of Karlsruhe       and  Carnegie Mellon University
         Dept. of Informatics               Dept. of Computer Science
         ILKD, Lehrstuhl Prof.Waibel        Alex Waibel's NN & Speech Group
         Am Fasanengarten 5                 5000 Forbes Ave
         D-76131 Karlsruhe                  Pittsburgh, PA 15213
         - West Germany -                   - USA -

    Copyright (C) 1990-1994.   All rights reserved.

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
    Revision 4.1  2003/08/14 11:19:51  fuegen
    Merged changes on branch jtk-01-01-15-fms (jaguar -> ibis-013)

    Revision 1.1.2.2  2002/06/26 11:57:53  fuegen
    changes for new directory structure support and code cleaning

    Revision 1.1.2.1  2002/03/26 10:05:38  soltau
    added to NJD

    Revision 4.1  2001/02/20 17:34:53  hyu
    *** empty log message ***

 * Revision 1.1  98/06/15  19:21:52  hyu
 * Initial revision
 * 
 * Revision 1.1  1996/09/27  08:59:05  fritsch
 * Initial revision
 *

   ======================================================================== */



#ifndef __ffmat__
#define __ffmat__


/* ==================================================================== */
/*                              Data Types                              */
/* ==================================================================== */


#include "matrix.h"


typedef struct {

  FMatrix          *A;
  FMatrix          *B;
  FMatrix          *C;
  int              offset;
  int              threadX;

} FFMatArg;



/* ==================================================================== */
/*                              Proto Types                             */
/* ==================================================================== */


int FFMat_Init (void);


int     chaosRandomInit    (double seed);
double  chaosRandom        (double a, double b);



/* -------------------------------------------------------------------- */
/*                             IMatrix routines                         */
/* -------------------------------------------------------------------- */

int        _imatrixDeinit      (IMatrix *imat);
int        _imatrixDestroy     (IMatrix *imat);
int        _imatrixInitConst   (IMatrix *imat, int initVal);
int        _imatrixInitDiag    (IMatrix *imat, int initVal);
int        _imatrixInitRandom  (IMatrix *imat, float maxAbsVal);
int        _imatrixResize      (IMatrix *imat, int m, int n);
IMatrix    *_imatrixCreate     (int m, int n);

int        _imatrixPrint       (IMatrix *imat);
int        _imatrixPuts        (IMatrix *imat, FILE *fp);
int        _imatrixCopy        (IMatrix *C, IMatrix *A);
int        _imatrixTrans       (IMatrix *C, IMatrix *A);

int        _imatrixSave        (IMatrix *A, FILE *fp);
int        _imatrixLoad        (IMatrix *A, FILE *fp);



/* -------------------------------------------------------------------- */
/*                             FMatrix routines                         */
/* -------------------------------------------------------------------- */

int        _fmatrixDeinit      (FMatrix *fmat);
int        _fmatrixDestroy     (FMatrix *fmat);
int        _fmatrixInitConst   (FMatrix *fmat, float initVal);
int        _fmatrixInitDiag    (FMatrix *fmat, float initVal);
int        _fmatrixInitRandom  (FMatrix *fmat, float maxAbsVal);
int        _fmatrixPrune       (FMatrix *fmat);
int        _fmatrixResize      (FMatrix *fmat, int m, int n);
FMatrix    *_fmatrixCreate     (int m, int n);

int        _fmatrixPrint       (FMatrix *fmat);
int        _fmatrixPuts        (FMatrix *fmat, FILE *fp);
int        _fmatrixCopy        (FMatrix *C, FMatrix *A);
int        _fmatrixTrans       (FMatrix *C, FMatrix *A);
int        _fmatrixTrans1      (FMatrix *C, FMatrix *A);
float      _fmatrixMSE         (FMatrix *fmat);

int        _fmatrixScale       (FMatrix *C, FMatrix *A, float gamma);
int        _fmatrixScaleCol    (FMatrix *C, FMatrix *A, FMatrix *W);
int        _fmatrixAdd         (FMatrix *C, FMatrix *A, FMatrix *B);
int        _fmatrixAccu        (FMatrix *C, FMatrix *A, float alpha);
int        _fmatrixAccuScaled  (FMatrix *C, FMatrix *A, FMatrix *g, int index);
int        _fmatrixSub         (FMatrix *C, FMatrix *A, FMatrix *B);
int        _fmatrixProd        (FMatrix *C, FMatrix *A, FMatrix *B);
float      _fmatrixCosine      (FMatrix *A, FMatrix *B);
int        _fmatrixMulT        (FMatrix *C, FMatrix *A, FMatrix *B);
int        _fmatrixMul         (FMatrix *C, FMatrix *A, FMatrix *B);
int        _fmatrixMul1        (FMatrix *C, FMatrix *A, FMatrix *B);
int        _fmatrixInv         (FMatrix *C, FMatrix *A);
int        _fmatrixLS          (FMatrix *D, FMatrix *W, FMatrix *Y, FMatrix *X);
int        _fmatrixEigen       (FMatrix *V, FMatrix *D, FMatrix *A);
int        _fmatrixDet         (FMatrix *A, double *logdet, double *sign);

int        _fmatrixSave        (FMatrix *A, FILE *fp);
int        _fmatrixLoad        (FMatrix *A, FILE *fp);



/* -------------------------------------------------------------------- */
/*                             DMatrix routines                         */
/* -------------------------------------------------------------------- */


int        _dmatrixDeinit     (DMatrix *dmat);
int        _dmatrixDestroy    (DMatrix *dmat);
int        _dmatrixInitConst  (DMatrix *dmat, double initVal);
int        _dmatrixInitDiag   (DMatrix *dmat, double initVal);
int        _dmatrixInitRandom (DMatrix *dmat, double maxAbsVal);
int        _dmatrixResize     (DMatrix *dmat, int m, int n);
DMatrix    *_dmatrixCreate    (int m, int n);

int        _dmatrixPrint      (DMatrix *dmat);
int        _dmatrixPuts       (DMatrix *dmat, FILE *fp);
int        _dmatrixCopy       (DMatrix *C, DMatrix *A);
int        _dmatrixTrans      (DMatrix *C, DMatrix *A);
int        _dmatrixTrans1     (DMatrix *C, DMatrix *A);
double     _dmatrixMSE        (DMatrix *dmat);

int        _dmatrixScale      (DMatrix *C, DMatrix *A, double gamma);
int        _dmatrixScaleCol   (DMatrix *C, DMatrix *A, DMatrix *W);
int        _dmatrixAdd        (DMatrix *C, DMatrix *A, DMatrix *B);
int        _dmatrixAccu       (DMatrix *C, DMatrix *A, double alpha);
int        _dmatrixSub        (DMatrix *C, DMatrix *A, DMatrix *B);
int        _dmatrixProd       (DMatrix *C, DMatrix *A, DMatrix *B);
double     _dmatrixCosine     (DMatrix *A, DMatrix *B);
int        _dmatrixMulT       (DMatrix *C, DMatrix *A, DMatrix *B);
int        _dmatrixMul        (DMatrix *C, DMatrix *A, DMatrix *B);
int        _dmatrixMul1       (DMatrix *C, DMatrix *A, DMatrix *B);
int        _dmatrixInv        (DMatrix *C, DMatrix *A);
int        _dmatrixLS         (DMatrix *D, DMatrix *W, DMatrix *Y, DMatrix *X);
int        _dmatrixEigen      (DMatrix *V, DMatrix *D, DMatrix *A);
int        _dmatrixDet        (DMatrix *A, double *logdet, double *sign);

int        _dmatrixSave       (DMatrix *A, FILE *fp);
int        _dmatrixLoad       (DMatrix *A, FILE *fp);


/* -------------------------------------------------------------------- */
/*                           conversion routines                        */
/* -------------------------------------------------------------------- */

int        _fmatrix2dmatrix   (DMatrix *C, FMatrix *A);
int        _dmatrix2fmatrix   (FMatrix *C, DMatrix *A);



#endif
