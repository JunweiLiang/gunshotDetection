/* ========================================================================
    JANUS      Speech- to Speech Translation
               ------------------------------------------------------------
               Object: Matrix/Vector Type & Operators
               ------------------------------------------------------------

    Author  :  Martin Maier & Michael Finke 
    Module  :  matrix.h
    Date    :  $Id: matrix.h 3377 2011-02-28 00:42:24Z metze $
    Remarks :

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
    Revision 3.15  2007/02/21 12:32:00  fuegen
    added Early Feature Vector Reduction (EFVR) code from Thilo Koehler

    Revision 3.14  2005/03/04 09:12:45  metze
    Andreas Eller's DMatrix methods added

    Revision 3.13  2004/05/27 11:25:41  metze
    Cleaned up error messages (chenged type of dludcmp)

    Revision 3.12  2003/08/14 11:19:52  fuegen
    Merged changes on branch jtk-01-01-15-fms (jaguar -> ibis-013)

    Revision 3.11.4.3  2003/04/16 12:59:37  metze
    MVDR added (Matthias Wölfel)

    Revision 3.11.4.2  2002/06/26 11:57:53  fuegen
    changes for new directory structure support and code cleaning

    Revision 3.11.4.1  2002/01/18 13:35:05  metze
    Added fmatrixCSave and fmatrixCLoad

    Revision 3.11  2001/01/10 16:58:36  janus
    Merged Florian's version, supporting e.g. WAV-Files, Multivar and NGETS

    Revision 3.10.16.2  2001/01/10 10:43:41  janus
    Final multivar, Cleanup

    Revision 3.10.16.1  2000/10/31 21:26:38  janus
    Integrated Univar, Multivar still has problems

    Revision 3.10  2000/10/12 12:57:13  janus
    Merged branch jtk-00-10-09-metze.

    Revision 3.9.24.1  2000/10/12 08:02:16  janus
    This compiles, but does not work on SUN for decoding without SIMD

    Revision 3.9.14.1  2000/10/11 09:24:42  janus
    Added SIMD instructions and preliminary MultiVar version

    Revision 3.9  2000/08/27 15:31:24  jmcd
    Merging branch 'jtk-00-08-24-jmcd'.

    Revision 3.8.2.1  2000/08/25 22:19:45  jmcd
    Just checking.

    Revision 3.8  2000/08/24 13:32:26  jmcd
    Merging changes from branch jtk-00-08-23-jmcd.

    Revision 3.7.2.1  2000/08/24 09:32:48  jmcd
    More revision fixes.

    Revision 3.7  2000/02/07 16:55:27  soltau
    Added imatrixPutsItf (to make modality stuff happy)

    Revision 3.6  2000/02/07 16:52:27  soltau
    Added some semi-tied covariances stuff

    Revision 3.5  1998/08/12 09:19:50  kemp
    added dmatrixDet and dmatrixLogDet extern declarations

 * Revision 3.4  1997/07/23  12:18:59  tschaaf
 * gcc Clean DFKI (nearly)
 *
    Revision 3.3  1997/07/04 11:52:33  fritsch
    Added function header to make other modules -Wall clean.

    Revision 1.22  1997/02/13  16:48:00  fritsch
    *** empty log message ***

    Revision 1.21  1996/03/12  15:37:52  maier
    changed neuralGas

    Revision 1.20  1996/03/02  15:43:34  maier
    changed fmatrixMeanvar

    Revision 1.19  1996/03/02  12:57:15  maier
    bload & bsave for fvector

    Revision 1.18  1996/02/28  00:51:17  fritsch
    changed return value of fmatrixNeuralGas to double

    Revision 1.17  1996/02/13  03:46:01  finkem
    add svd backsubstitution

    Revision 1.16  1996/02/05  17:18:20  maier
    changed neuralGas arguments

    Revision 1.15  1996/01/18  21:37:04  finkem
    moved neuralGas to FMatrix

    Revision 1.14  1996/01/17  00:33:15  maier
    *** empty log message ***

    Revision 1.13  1996/01/08  17:52:19  maier
    new methods

    Revision 1.12  1995/12/29  13:49:04  maier
    *** empty log message ***

    Revision 1.11  1995/12/14  08:49:11  maier
    new field 'dtype'

    Revision 1.9  1995/12/08  04:53:46  maier
    functions in matrix-io.c
    cleaned up
    IMatrix

    Revision 1.8  1995/12/06  21:56:33  maier
    added some stuff to dmatrix, including eigen, svd, inv

    Revision 1.7  1995/11/30  19:43:41  maier
    new functions (FBMatrix)

    Revision 1.6  1995/10/30  09:29:38  maier
    *** empty log message ***

    Revision 1.5  1995/10/20  14:58:09  maier
    svector: power, mean

    Revision 1.4  1995/09/01  16:37:06  maier
    count

    Revision 1.3  1995/08/04  09:23:08  maier
    dx and dy instead blobsize

    Revision 1.2  1995/07/28  12:54:55  maier
    *** empty log message ***

    Revision 1.1  1995/07/27  15:31:04  finkem
    Initial revision

  
   ======================================================================== */

#ifdef __cplusplus
extern "C" {
#endif

#ifndef _matrix
#define _matrix

#include "common.h"   /* includes error.h */
#include "itf.h"

int Matrix_Init (void);

int     chaosRandomInit  (double seed);
double  chaosRandom      (double a, double b);


/* ------------------------------------------------------------------------
    Short Vector
   ------------------------------------------------------------------------ */

typedef struct {

  short*    vecA;
  int       n;

} SVector;

extern SVector*   svectorCreate(     int n);
extern void       svectorFree(       SVector* A);
extern ClientData svectorCreateItf(  ClientData cd, int argc, char *argv[]);
extern SVector*   svectorGetItf(     char *value);
extern int        getSVector(        ClientData cd, char* key,
			             ClientData result,
			             int *argcP, char *argv[]);

/* --- Short Vector Methods --- */
extern SVector* svectorCopy(         SVector* A, SVector* B);
extern int      svectorAddScalar(    SVector* A, int m);
extern SVector* svectorLin(          SVector* A, float a, float b);
extern SVector* svectorAdd(          SVector* C, float a, SVector* A, float b,
			             SVector* B, int mode);
extern SVector* svectorMul(          SVector* C, float a, SVector* A,
			             SVector* B, int mode, int div);
extern int      svectorMinmax(       SVector* A, short *min, short *max);
extern int      svectorMinmaxft(     SVector* A, short *min, short *max,
				     int from, int to);
extern int      svectorPower(        SVector* A, double *power);
extern int      svectorMean(         SVector* A, double *mean);

extern int      svectorResize(       SVector* A, int n);
extern int      svectorMinmaxItf(    ClientData cd, int argc, char *argv[]);
extern int      svectorPowerItf(     ClientData cd, int argc, char *argv[]);
extern int      svectorMeanItf(      ClientData cd, int argc, char *argv[]);
extern int      svectorHistogram(    SVector* A, int from, int to, short low,
				     short high, int n, int exc, int *histo);
extern int      svectorHistogramItf( ClientData cd, int argc, char *argv[]);
extern int      svectorDisplay(      Tcl_Interp *interp, char *canvas,
			             SVector *vec, int height, int from,
			             int to, int step, float scale, char *tag);
extern int      svectorDisplayItf(   ClientData cd, int argc, char *argv[]);

/* ------------------------------------------------------------------------
    Int Matrix
   ------------------------------------------------------------------------ */

typedef struct {

  int**     matPA;
  int       n;
  int       m;

} IMatrix;

extern int      imatrixDestroy     (IMatrix *imat);
extern IMatrix* imatrixCreate(     int m, int n);
extern void     imatrixFree(       IMatrix* A);
extern int      imatrixInitConst   (IMatrix *imat, int initVal);
extern int      imatrixInitDiag    (IMatrix *imat, int initVal);
extern int      imatrixInitRandom  (IMatrix *imat, float maxAbsVal);
extern int      imatrixTrans       (IMatrix *C, IMatrix *A);
extern int      imatrixSave        (IMatrix *A, FILE *fp);
extern int      imatrixLoad        (IMatrix *A, FILE *fp);
extern IMatrix* imatrixGetItf(     char* value);
extern int      getIMatrix(        ClientData cd, char* key,
			           ClientData result,
			           int *argcP, char *argv[]);

/* --- Integer Matrix Methods --- */
extern IMatrix* imatrixCopy(       IMatrix* A, IMatrix* B);
extern int      imatrixResize(     IMatrix* A, int m, int n);
extern IMatrix* imatrixClear(      IMatrix* A);

extern int      imatrixGetValItf ( ClientData cd, int argc, char *argv[]);
extern int      imatrixSetValItf ( ClientData cd, int argc, char *argv[]);
extern int      imatrixCopyItf   ( ClientData cd, int argc, char *argv[]);
extern int      imatrixResizeItf ( ClientData cd, int argc, char *argv[]);
extern int      imatrixPutsItf   ( ClientData cd, int argc, char *argv[]);

/* ------------------------------------------------------------------------
    Char Matrix
   ------------------------------------------------------------------------ */

typedef struct {

  unsigned char** matPA;
  int       n;
  int       m;

} CMatrix;

extern int      cmatrixDestroy(CMatrix *imat);
extern CMatrix* cmatrixCreate(int m, int n);
extern void     cmatrixFree(CMatrix* A);

/* ------------------------------------------------------------------------
    Float Vector
   ------------------------------------------------------------------------ */

typedef struct {

  float*    vecA;
  int       n;
  double    count;
} FVector;

extern FVector*   fvectorCreate(    int n);
extern void       fvectorFree(      FVector* A);
extern ClientData fvectorCreateItf( ClientData cd, int argc, char *argv[]);
extern FVector*   fvectorGetItf(    char *value);
extern FVector*   fvectorAdd(       FVector* C, float a, FVector* A, float b,
			            FVector* B, int mode);
extern int        getFVector(       ClientData cd, char* key,
			            ClientData result,
			            int *argcP, char *argv[]);
extern int        getFVectorP(      ClientData cd, char* key,
			            ClientData result,
			            int *argcP, char *argv[]);

/* --- Float Vector Methods --- */
extern FVector*   fvectorCopy(      FVector* A, FVector* B);
extern int        fvectorResize(    FVector* A, int n);
extern FVector*   fvectorClear(     FVector* A);

extern int        fvectorCopyItf(   ClientData cd, int argc, char *argv[]);
extern int        fvectorResizeItf( ClientData cd, int argc, char *argv[]);

/* ------------------------------------------------------------------------
    Float Matrix
   ------------------------------------------------------------------------ */

typedef struct {

  float**   matPA;
  int       n;
  int       m;
  double    count;

} FMatrix;

extern FMatrix* fmatrixCreate(     int m, int n);
extern int      fmatrixDestroy     (FMatrix *fmat);
extern int      fmatrixDeinit      (FMatrix *fmat);
extern void     fmatrixFree(       FMatrix* A);
extern ClientData fmatrixCreateItf(ClientData cd, int argc, char *argv[]);
extern FMatrix* fmatrixGetItf(     char* value);
extern int      getFMatrix(        ClientData cd, char* key,
			           ClientData result,
			           int *argcP, char *argv[]);
extern int      getFMatrixP(       ClientData cd, char* key,
			           ClientData result,
			           int *argcP, char *argv[]);

/* --- Float Matrix Methods --- */
int  fmatrixPuts (FMatrix *fmat, FILE *fp);
int  fmatrixInitConst (FMatrix *fmat, float initVal);
int  fmatrixInitDiag (FMatrix *fmat, float initVal);
int  fmatrixInitRandom (FMatrix *fmat, float maxAbsVal);
int  fmatrixPrune (FMatrix *fmat);
int  fmatrixTransFF (FMatrix *C, FMatrix *A);
int  fmatrixTransFF1 (FMatrix *C, FMatrix *A);
float  fmatrixMSE (FMatrix *fmat);
int  fmatrixScale (FMatrix *C, FMatrix *A, float gamma);
int  fmatrixScaleCol (FMatrix *C, FMatrix *A, FMatrix *W);
int  fmatrixAddFF (FMatrix *C, FMatrix *A, FMatrix *B);
int  fmatrixAccu (FMatrix *C, FMatrix *A, float alpha);
int  fmatrixAccuScaled (FMatrix *C, FMatrix *A, FMatrix *g, int index);
int  fmatrixSub (FMatrix *C, FMatrix *A, FMatrix *B);
int  fmatrixProd (FMatrix *C, FMatrix *A, FMatrix *B);
float  fmatrixCosineFF (FMatrix *A, FMatrix *B);
int  fmatrixMulFFT (FMatrix *C, FMatrix *A, FMatrix *B);
int  fmatrixMulFF (FMatrix *C, FMatrix *A, FMatrix *B);
int  fmatrixMulFF1 (FMatrix *C, FMatrix *A, FMatrix *B);
int  fmatrixRadT (FMatrix *C, FMatrix *A, FMatrix *B);
int  fmatrixRad (FMatrix *C, FMatrix *A, FMatrix *B);
int  fmatrixSave (FMatrix *A, FILE *fp);
int  fmatrixLoadFF (FMatrix *A, FILE *fp);
int  fmatrixLogNormalize(FMatrix *A);
int float_log_normalize(float* vec, int size);
extern float    fmatrixDet   (     FMatrix *A);
extern int      fmatrixExtDet(     FMatrix *A,double *logdet, double *sign);
extern FMatrix* fmatrixCopy(       FMatrix* A, FMatrix* B);
extern int      fmatrixResize(     FMatrix* A, int m, int n);
extern FMatrix* fmatrixClear(      FMatrix* A);
extern FMatrix* fmatrixTrans(      FMatrix* A, FMatrix* B);
extern int      fmatrixMeanvar(    FMatrix* mat, FMatrix* weight, FVector* mean,
			           FVector* smean, FVector *dev);
extern int      fmatrixRefill(     FMatrix* A, int m, int n, float* data);
extern FMatrix* fmatrixAdd(        FMatrix* C, float a, FMatrix* A, float b,
			           FMatrix* B);
extern FMatrix* fmatrixMulcoef(    FMatrix* C, float a, FMatrix* A,
			           FMatrix* B, int mode, int div);
extern FMatrix* fmatrixMul(        FMatrix* C, FMatrix* A, FMatrix* B);
extern FMatrix* fmatrixMulot(      FMatrix* C, FMatrix* A, FMatrix* B);
extern int      fmatrixMinmaxft(   FMatrix* A, float *min, float *max,
				   int from, int to);
extern int      fmatrixMinmax(     FMatrix* A, float *min, float *max);
extern double   fmatrixNeuralGas(  FMatrix* cmP, FMatrix* fmP, int N, 
                                   float betaS,  float betaF, FVector* fvP,
				   int init, int step);
extern int      fmatrixDisplay (   Tcl_Interp *interp, char *canvas,
				   FMatrix *matrix, int width, int height,
				   int x, int y, int from, int to ,
				   int borderwidth, int space, int dx, int dy,
				   float min, float max,
				   char *tag, int grey, char *outline);
extern FMatrix* fmatrixEFVR(   FMatrix* A, float *weight, float threshold, float *thres, float boost, float decrease, float maxboost, int shrink); 

extern int      fmatrixCopyItf   ( ClientData cd, int argc, char *argv[]);
extern int      fmatrixGetValItf ( ClientData cd, int argc, char *argv[]);
extern int      fmatrixSetValItf ( ClientData cd, int argc, char *argv[]);
extern int      fmatrixResizeItf ( ClientData cd, int argc, char *argv[]);
extern int      fmatrixTransItf  ( ClientData cd, int argc, char *argv[]);
extern int      fmatrixAddItf    ( ClientData cd, int argc, char *argv[]);
extern int      fmatrixMulotItf  ( ClientData cd, int argc, char *argv[]);
extern int      fmatrixMinmaxItf ( ClientData cd, int argc, char *argv[]);
extern int      fmatrixDisplayItf( ClientData cd, int argc, char *argv[]);
extern int      fmatrixPutsItf   ( ClientData cd, int argc, char *argv[]);


extern double   fmatrixDistortion( FMatrix *codebook, FVector *distrib, FMatrix *xvaldata);

/* ------------------------------------------------------------------------
    Float Band Matrix
   ------------------------------------------------------------------------ */

typedef struct {

  float**   matPA;
  int       m;
  int       n;
  int*      offset;                 /* offset */
  int*      coefN;                  /* number of coefficients */
  float     rate;                   /* sampling rate in Hz */
} FBMatrix;

extern FBMatrix*  fbmatrixCreate    ( int m, int n);
extern void       fbmatrixFree      ( FBMatrix* A );
extern FBMatrix*  fbmatrixGetItf    ( char* value);
extern int        getFBMatrix       ( ClientData cd, char* key, 
				      ClientData result,
				      int *argcP, char *argv[]);
extern int        getFBMatrix       ( ClientData cd, char* key,
				      ClientData result,
				      int *argcP, char *argv[]);

/* --- Float Band Matrix Methods --- */
extern FBMatrix*  fbmatrixCopy      ( FBMatrix* A, FBMatrix* B);
extern FMatrix*   fmatrixBMulot     ( FMatrix* C, FMatrix* A, FBMatrix* B);

extern int        fbmatrixCopyItf   ( ClientData cd, int argc, char *argv[]);
extern int        fmatrixBMulotItf  ( ClientData cd, int argc, char *argv[]);

extern int        fbmatrixMelItf    ( ClientData cd, int argc, char *argv[]);
extern int        fbmatrixMeltriItf ( ClientData cd, int argc, char *argv[]);
extern int        fbmatrixMeltraItf ( ClientData cd, int argc, char *argv[]);
extern int        fbmatrixLinearItf ( ClientData cd, int argc, char *argv[]);



/* ------------------------------------------------------------------------
    Double Vector
   ------------------------------------------------------------------------ */

typedef struct {

  double*   vecA;
  int       n;
  double    count;
} DVector;

extern DVector*   dvectorCreate(    int n);
extern void       dvectorFree(      DVector* A);
extern ClientData dvectorCreateItf( ClientData cd, int argc, char *argv[]);
extern DVector*   dvectorGetItf(    char *value);
extern DVector*   dvectorAdd(       DVector* C, double a, DVector* A, double b,
			            DVector* B);
extern int        getDVector(       ClientData cd, char* key,
			            ClientData result,
			            int *argcP, char *argv[]);

/* --- Double Vector Methods --- */
extern DVector*   dvectorCopy(      DVector* A, DVector* B);
extern int        dvectorResize(    DVector* A, int n);
extern DVector*   dvectorClear(     DVector* A);

extern int        dvectorCopyItf(   ClientData cd, int argc, char *argv[]);
extern int        dvectorResizeItf( ClientData cd, int argc, char *argv[]);
extern int        dvectorAddItf(    ClientData cd, int argc, char *argv[]);


/* ------------------------------------------------------------------------
    Double Matrix
   ------------------------------------------------------------------------ */

typedef struct {

  double**  matPA;
  int       n;
  int       m;
  double    count;

} DMatrix;

extern DMatrix*  dmatrixCreate(   int m, int n);
extern void      dmatrixFree(     DMatrix* A);
extern DMatrix*  dmatrixGetItf(   char* value);
extern int       getDMatrix(      ClientData cd, char* key, ClientData result,
			          int *argcP, char *argv[]);
extern int       getDMatrixP(     ClientData cd, char* key, ClientData result,
			          int *argcP, char *argv[]);

/* --- Double Matrix Methods --- */
extern DMatrix*  dmatrixCopy(     DMatrix* A, DMatrix* B);
extern int       dmatrixResize(   DMatrix* A, int m, int n);
extern DMatrix*  dmatrixClear(    DMatrix* A);
extern DMatrix*  dmatrixTrans(    DMatrix* A, DMatrix* B);
extern DMatrix*  dmatrixAdd(      DMatrix* C, double a, DMatrix* A, double b,
			          DMatrix* B);
extern DMatrix*  dmatrixMul(      DMatrix* C, DMatrix* A, DMatrix* B);
extern DMatrix*  dmatrixMulot(    DMatrix* C, DMatrix* A, DMatrix* B);
extern DMatrix*  dmatrixCovmul(   DMatrix* C, DMatrix* A, DMatrix* B);
extern DMatrix*  dmatrixUnity(    DMatrix* A);
extern int       dmatrixEigen(    DMatrix* A, DMatrix* EV, double th,
			          int max_iter);
extern int       dmatrixAccu(     DMatrix* C, DMatrix* A, double a);
extern int       dmatrixSub(      DMatrix *C, DMatrix *A, DMatrix *B);
extern DMatrix*  dmatrixSVD(      DMatrix *A, DMatrix *W, DMatrix *V);
extern DMatrix*  dmatrixInv(      DMatrix *B,DMatrix *A);
extern double    dmatrixDet(      DMatrix *A);
extern double    dmatrixLogDet(   DMatrix *A, double *sign);

extern int     dmatrixInitConst(  DMatrix* A, double initVal);
extern int     dmatrixInitDiag(   DMatrix* A, double initVal);
extern int     dmatrixInitRandom( DMatrix* A, double maxAbsVal);
extern int     dmatrixScale(      DMatrix* C, DMatrix* A,     double gamma);
extern int     dmatrixExtDet(     DMatrix* A, double* logdet, double* sign);
extern int     dmatrixMinmax(     DMatrix* A, double* min,    double* max);

extern int      dmatrixGetValItf ( ClientData cd, int argc, char *argv[]);
extern int      dmatrixSetValItf ( ClientData cd, int argc, char *argv[]);
extern int       dmatrixTransItf(  ClientData cd, int argc, char *argv[]);
extern int       dmatrixAddItf(    ClientData cd, int argc, char *argv[]);
extern int       dmatrixMulItf(    ClientData cd, int argc, char *argv[]);
extern int       dmatrixMulotItf(  ClientData cd, int argc, char *argv[]);
extern int       dmatrixUnityItf(  ClientData cd, int argc, char *argv[]);
extern int       dmatrixEigenItf(  ClientData cd, int argc, char *argv[]);
extern int       dmatrixSVDItf(    ClientData cd, int argc, char *argv[]);
extern int       dmatrixInvItf(    ClientData cd, int argc, char *argv[]);
extern DMatrix*  dmatrixWhiten(   DMatrix *A);
extern int       dmatrixPutsItf(   ClientData cd, int argc, char *argv[]);
extern int dmatrixEigenQR(DMatrix* A, DMatrix* EV, double th,int max_iter);
extern int dmatrixClean(DMatrix* A,double thresh);

extern FMatrix* fmatrixDMatrix(  FMatrix* A, DMatrix* B);
extern DMatrix* dmatrixFMatrix(  DMatrix* A, FMatrix* B);

extern int  dludcmp(  DMatrix *a, DVector *indx, double *d);
extern void dsvdcmp(  double** a, int m, int n, double w[], double **v);
extern int  dsvclean( double*  w, int n);
extern void dsvbksb(  double** u, double w[], double **v, int m, int n, 
                      double b[], double x[]);

/* ------------------------------------------------------------------------
    Functions in matrix-io.c
   ------------------------------------------------------------------------ */
extern int imatrixPuts (IMatrix *A, FILE *fp);
extern IMatrix* imatrixBSave(    IMatrix *A, char *filename);
extern IMatrix* imatrixBLoad(    IMatrix *A, char *filename, int im);

extern int imatrixBSaveItf(      ClientData cd, int argc, char *argv[]);
extern int imatrixBLoadItf(      ClientData cd, int argc, char *argv[]);

extern FVector* fvectorBSave(    FVector *A, char *filename);
extern FVector* fvectorBLoad(    FVector *A, char *filename);

extern int fvectorBSaveItf(      ClientData cd, int argc, char *argv[]);
extern int fvectorBLoadItf(      ClientData cd, int argc, char *argv[]);

extern FMatrix* fmatrixBAppend(  FMatrix *A, char *filename);
extern FMatrix* fmatrixBSave(    FMatrix *A, char *filename);
extern FMatrix* fmatrixBLoad(    FMatrix *A, char *filename, int im);
extern FMatrix* fmatrixCSave(    FMatrix *A, char *filename, char *modus);
extern FMatrix* fmatrixCLoad(    FMatrix *A, char *filename, int append);

extern int fmatrixBAppendItf(    ClientData cd, int argc, char *argv[]);
extern int fmatrixBSaveItf(      ClientData cd, int argc, char *argv[]);
extern int fmatrixBLoadItf(      ClientData cd, int argc, char *argv[]);
extern int fmatrixCSaveItf(      ClientData cd, int argc, char *argv[]);
extern int fmatrixCLoadItf(      ClientData cd, int argc, char *argv[]);
extern int fmatrixMLoadItf(      ClientData cd, int argc, char *argv[]);
extern int fmatrixILoadItf(      ClientData cd, int argc, char *argv[]);
extern int fmatrixISaveItf(      ClientData cd, int argc, char *argv[]);

extern int     dmatrixBLoad(      DMatrix* A, const char* filename);
extern int     dmatrixBLoadItf(   ClientData cd, int argc, char *argv[]);
extern int     dmatrixBSave(      DMatrix* A, const char* filename);
extern int     dmatrixBSaveItf(   ClientData cd, int argc, char *argv[]);

#endif




#ifdef __cplusplus
}
#endif
