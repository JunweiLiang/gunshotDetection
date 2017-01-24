/* ========================================================================
    JANUS      Speech- to Speech Translation
               ------------------------------------------------------------
               Object: Matrix/Vector Type & Operators
               ------------------------------------------------------------

    Author  :  Michael Finke & Martin Westphal
    Module  :  matrix.c
    Date    :  $Id: matrix.c 3428 2011-04-15 18:00:28Z metze $
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
    Revision 3.33  2007/03/02 13:02:01  fuegen
    added SSE support (Florian Metze, Friedrich Faubel)
    added SVector/ FMatrix append functions (Florian Metze, Friedrich Faubel)

    Revision 3.32  2007/02/21 12:32:00  fuegen
    added Early Feature Vector Reduction (EFVR) code from Thilo Koehler

    Revision 3.31  2005/11/08 12:41:27  metze
    Uninitialized variables

    Revision 3.30  2005/04/25 13:23:26  metze
    Improved fmatrixModulo

    Revision 3.29  2005/03/04 09:12:45  metze
    Andreas Eller's DMatrix methods added

    Revision 3.28  2004/09/10 15:39:19  metze
    Cosmetics, after not changing alignment of FMatrices

    Revision 3.27  2004/08/23 11:28:17  metze
    Beautification

    Revision 3.26  2004/05/27 11:24:49  metze
    Cleaned up error messages
    Added code for histograms
    Added code from Yue

    Revision 3.25  2004/01/08 12:26:01  metze
    Added -f parameter to 'modulo'

    Revision 3.24  2003/08/14 11:19:52  fuegen
    Merged changes on branch jtk-01-01-15-fms (jaguar -> ibis-013)

    Revision 3.23.4.17  2003/07/02 17:09:23  fuegen
    fixes for file handling under Windows
    using general functions for file open/close

    Revision 3.23.4.16  2003/06/25 14:18:29  metze
    Initialized variables

    Revision 3.23.4.15  2003/04/16 12:59:36  metze
    MVDR added (Matthias Woelfel)

    Revision 3.23.4.14  2002/11/21 17:09:29  fuegen
    windows code cleaning

    Revision 3.23.4.13  2002/11/04 14:26:06  metze
    Fixed initialization problems

    Revision 3.23.4.11  2002/08/27 16:22:59  metze
    'char* canvas = NULL' added

    Revision 3.23.4.10  2002/07/18 12:19:55  soltau
    Removed malloc.h

    Revision 3.23.4.9  2002/06/26 11:57:53  fuegen
    changes for new directory structure support and code cleaning

    Revision 3.23.4.8  2002/03/26 12:58:46  soltau
    Added dmatrixEigenQR to speed up simdiag from Hua

    Revision 3.23.4.7  2002/02/17 14:47:28  metze
    Simplified 'window' implementation

    Revision 3.23.4.6  2002/02/15 17:55:09  metze
    Added 'window' method for FMatrix

    Revision 3.23.4.5  2002/01/18 13:33:46  metze
    Added cload and csave methods for FMatrix

    Revision 3.23.4.4  2002/01/15 13:53:49  metze
    Quick and dirty bugfix for featshow (fmatrixDisplay)

    Revision 3.23.4.3  2001/11/19 14:46:26  metze
    Go away, boring message!

    Revision 3.23.4.2  2001/10/25 14:59:05  soltau
    removed annoying neuralGas output

    Revision 3.23.4.1  2001/07/03 09:35:31  soltau
    Use memalign instead of malloc in cmatrixCreate, fmatrixCreate

    Revision 3.23  2001/01/15 09:49:59  janus
    Merged Christian's version with WINDOWS support (branch jtk-00-11-14-fuegen)

    Revision 3.22  2001/01/10 16:58:36  janus
    Merged Florian's version, supporting e.g. WAV-Files, Multivar and NGETS

    Revision 3.21.4.1  2001/01/12 15:16:58  janus
    necessary changes for running janus under WINDOWS

    Revision 3.21  2000/11/14 12:32:23  janus
    Merging branch jtk-00-10-27-jmcd.

    Revision 3.20.6.1  2000/11/06 10:50:36  janus
    Made changes to enable compilation under -Wall.

    Revision 3.20  2000/10/24 14:01:19  janus
    Merged changes on branch jtk-00-10-12-tschaaf

    Revision 3.19.4.1  2000/10/24 13:24:13  janus
    moved NDEBUG definition to optimizing definition.
    if NDEBUG is defined assert statements are removed during compilation

    Revision 3.19  2000/10/12 12:57:13  janus
    Merged branch jtk-00-10-09-metze.

    Revision 3.18.20.1  2000/10/12 08:02:15  janus
    This compiles, but does not work on SUN for decoding without SIMD

    Revision 3.18.6.1  2000/10/11 09:24:41  janus
    Added SIMD instructions and preliminary MultiVar version

    Revision 3.18  2000/08/22 16:43:50  soltau
    fixed bug in dmatrixExtDet

    Revision 3.17  2000/08/16 11:27:54  soltau
    Changed NeuralGas
    free -> TclFree

    Revision 3.16  2000/03/14 15:14:45  soltau
    closed memory leak in fmatrixDet, dmatrixDet

    Revision 3.15  2000/02/08 08:45:18  soltau
    Added Semi tied Covariances stuff
    Added Bayesian Information Criterion

    Revision 3.14  1998/10/30 14:02:51  soltau
    reduced neuralGas Info

    Revision 3.13  1998/08/12 13:03:23  westphal
    -format for 'det' methods
 
    Revision 3.12  1998/08/12 09:18:19  kemp
    added LU-decomposition for float and double
    added method 'det' for dmatrix, fmatrix
    added (C-) function dmatrixLogDet

 * Revision 3.11  1998/07/08  13:44:17  kemp
 * Removed bug in computation of distortion in neuralGas
 *
 * Revision 3.10  1998/03/12  17:08:31  kemp
 * Added function fromSample. This allows quick scanning for low gamma values
 * in extracted samples (better than just to 'resize' the extracted sample file :-) )
 *
 * Revision 3.9  1998/03/10  09:53:40  kemp
 * added method addvec to add a vector to a matrix (to each column)
 * Added semantics to the -init flag of neuralGas: if -init is 1, old
 * behaviour (random init), if -init is 2, the first sample vectors are
 * taken which gives fully deterministic behaviour
 *
 * Revision 3.8  1997/11/24  06:57:02  rogina
 * returned distortion info back into code, somebody changed the *source code*
 * to get rid of this (this can be done from tcl, no need to force everybody
 * not to have that info).
 *
    Revision 3.7  1997/10/17 11:08:33  westphal
    if 0 for memcpy

    Revision 3.6  1997/08/11 15:21:28  westphal
    puts for fmatrix

    Revision 3.5  1997/07/23 12:58:58  westphal
    fvector methods add and norm, removed compiler error

    Revision 3.4  1997/07/18 20:24:18  tschaaf
    gcc / DFKI - clean (nearly)

    Revision 3.3  1997/06/17 18:15:45  westphal
    swap method for SVector

 * Revision 3.2  1997/06/17  17:41:43  westphal
 * *** empty log message ***
 *
    Revision 1.50  1997/02/13  16:47:42  fritsch
    Added methods to access single elements in matrices

    Revision 1.49  1997/01/13  08:41:02  kemp
    Added method 'cluster'

    Revision 1.48  1996/08/05  08:58:10  kemp
    Commented.

    Revision 1.47  1996/03/15  18:19:23  maier
    scatterPlot

    Revision 1.46  1996/03/12  15:38:07  maier
    added flag -init for neuralGas

    Revision 1.45  1996/03/12  11:08:25  maier
    puts prec. % 2.6 % 2.8

    Revision 1.44  1996/03/05  14:29:41  rogina
    made gcc -Wall warnings go away

    Revision 1.43  1996/03/02  15:42:25  maier
    changed fmatrixMeanvar

    Revision 1.42  1996/03/02  12:57:41  maier
    bload & bsave for fvector

    Revision 1.41  1996/02/28  00:51:42  fritsch
    changed fmatrixNeuralGasItf to return the last distortion value to Tcl

    Revision 1.40  1996/02/23  17:52:14  maier
    method get and changes in puts for svector

    Revision 1.39  1996/02/13  03:46:01  finkem
    add svd backsubstitution

    Revision 1.38  1996/02/05  14:53:04  maier
    count for neuralGas

    Revision 1.37  1996/01/30  19:01:46  rogina
    fixed a bug from last time

    Revision 1.36  1996/01/30  18:13:31  rogina
    *** empty log message ***

    Revision 1.34  1996/01/29  17:27:25  maier
    allow 0xn or mx0 for FMatrix definition

    Revision 1.33  1996/01/26  00:14:06  torsten
    fixed Dev function to work at all!

    Revision 1.32  1996/01/25  21:21:15  torsten
    init p to null in Resize functions to avoid seg fault

    Revision 1.31  1996/01/22  20:53:47  torsten
    changed random() to rand() for HP compatibility

    Revision 1.30  1996/01/21  03:11:47  finkem
    configure dimensions to find out size of matrix/vectors

    Revision 1.29  1996/01/19  12:52:05  maier
    create only with name: FMatrix f

    Revision 1.28  1996/01/18  21:37:04  finkem
    moved neuralGas to FMatrix

    Revision 1.27  1996/01/17  00:34:31  maier
    svectorMul with div, getFVector, fvectorAdd, getFMatrixP, fmatrixDev,
    fmatrixMulcoef

    Revision 1.26  1996/01/08  17:52:29  maier
    type conversion, dmatrixAdd, ...

    Revision 1.25  1996/01/08  12:06:34  maier
    method FBMatrix mel

    Revision 1.24  1996/01/02  16:22:15  maier
    unrolling for f and dmatrix, changed puts!

    Revision 1.23  1995/12/29  16:43:22  maier
    cosine

    Revision 1.22  1995/12/29  13:49:12  maier
    new methods for fbmatrix, display

    Revision 1.21  1995/12/28  12:57:10  maier
    removed bug in dmatrixGetItf, Resize with 0

    Revision 1.20  1995/12/14  08:49:54  maier
    changes in feSVector, feFMatrix, fesIndexx
    noise also for FMatrix

    Revision 1.18  1995/12/08  04:54:53  maier
    IMatrix
    cleaned up

    Revision 1.17  1995/12/06  22:05:12  maier
    added some stuff to dmatrix, including eigen, svd, inv

    Revision 1.16  1995/11/30  19:44:04  maier
    new object FBMatrix

    Revision 1.15  1995/10/30  09:28:34  maier
    histogram

    Revision 1.14  1995/10/24  10:28:15  maier
    limit SVector scan, changed svectorDisplay

    Revision 1.13  1995/10/20  14:57:34  maier
    svector: power, mean

    Revision 1.12  1995/10/16  22:56:58  torsten
    I found the answer to life, unfortunately, there is not enough room
    in this message to make a note of it...

    Revision 1.11  1995/09/06  07:35:31  kemp
    *** empty log message ***

    Revision 1.10  1995/09/01  16:37:15  maier
    *** empty log message ***

    Revision 1.9  1995/08/21  12:51:52  maier
    added return in fmatrixGetItf

    Revision 1.8  1995/08/17  17:42:18  rogina
    *** empty log message ***

    Revision 1.7  1995/08/15  14:03:40  finkem
    *** empty log message ***

    Revision 1.6  1995/08/14  09:36:16  maier
    floor

    Revision 1.5  1995/08/07  14:49:45  maier
    *** empty log message ***

    Revision 1.4  1995/08/04  09:22:45  maier
    dx and dy instead blobsize

    Revision 1.3  1995/08/01  15:42:46  rogina
    *** empty log message ***

    Revision 1.2  1995/07/28  12:54:24  maier
    *** empty log message ***

    Revision 1.1  1995/07/27  15:31:04  finkem
    Initial revision

  
   ======================================================================== */

#include <math.h>
#include "mach_ind_io.h"
#include "matrix.h"  /* includes common.h(error.h) itf.h */
#include "ffmat.h"
#include "emath.h"

#ifdef BLAS
#ifdef DARWIN
#include <Accelerate/Accelerate.h>
#else
#include "cblas.h"
#endif
#endif

double dataDistortionM(FMatrix *dataP, int M);
double fmatrixDistortion(FMatrix *codebook, FVector *distrib, FMatrix *xvaldata);

char matrixRcsVersion[]= 
           "@(#)1$Id: matrix.c 3428 2011-04-15 18:00:28Z metze $";

#define sq(X)           ((X)*(X))
#define ROUND(a)        (floor((a) + 0.5))
#define ALLOC_ERROR     {SERROR("allocation failure\n"); return TCL_ERROR;}
#define ERROR_SQMAT(_com, _m, _n) \
         ERROR("%s number of rows (%d) and columns (%d) differ.\n",_com,_m,_n)



/* ======================================================================== */
/*               custom chaotic pseudo=random number generator              */
/* ======================================================================== */

static double GlobalRandX;
static double GlobalRandY;
static double GlobalRandZ;



/* ------------------------------------------------------------------------ */
/*               custom chaotic pseudo-random number generator              */
/* ------------------------------------------------------------------------ */
int  chaosRandomInit (double seed) {

  register double     step = 0.01;
  register double     red,green,blue;
  register int        k;

  GlobalRandX = seed;
  GlobalRandY = seed+0.1234567;
  GlobalRandZ = seed-0.1234567;

  for (k=0; k<1000; k++) {
    red   = 10.0*(GlobalRandY-GlobalRandX);
    green = GlobalRandX*(29.0-GlobalRandZ) - GlobalRandY;
    blue  = GlobalRandX*GlobalRandY - 2.66666666666*GlobalRandZ;
    GlobalRandX += step*red;
    GlobalRandY += step*green;
    GlobalRandZ += step*blue;
  }
  
  return TCL_OK;
}



/* ------------------------------------------------------------------------ */
/* generates a pseudo-random number in the range [a,b]                      */
/* using the chaotic Lorenz attractor as RNG                                */
/* ------------------------------------------------------------------------ */
double  chaosRandom (double a, double b) {

  register double     step=0.01;
  register double     red,green,blue;
  register double     rnd,unit;

  red   = 10.0*(GlobalRandY-GlobalRandX);
  green = GlobalRandX*(29.0-GlobalRandZ) - GlobalRandY;
  blue  = GlobalRandX*GlobalRandY - 2.66666666666*GlobalRandZ;
  GlobalRandX += step*red;
  GlobalRandY += step*green;
  GlobalRandZ += step*blue;
  rnd = 997.0*GlobalRandX + 917.0*GlobalRandY + 923.0*GlobalRandZ;
  unit = rnd-floor(rnd);

  return ((1-unit)*a + unit*b);
}



/* ========================================================================
    Short Vector
   ======================================================================== */
#define SLOW    -32768
#define SHIGH    32767
#define SLIMIT(x) \
  ((((x) < SLOW)) ? (SLOW) : (((x) < (SHIGH)) ? (x) : (SHIGH)))
/* ------------------------------------------------------------------------
    Create

    sv = svectorCreate(n)
        return pointer to a short vector, dimension n:

    svectorCreateItf(..):
        one argument n :         create short vector, dimension n
	more arguments {1 2 3} : create short vector with given elements
   ------------------------------------------------------------------------ */

SVector* svectorCreate( int n)
{
  /* --- allocate structure --- */
  SVector* vec = (SVector*)malloc(sizeof(SVector));
  if (! vec) {
    SERROR("allocation failure 1 short vector of size %d.\n", n);
    return NULL;
  }

  /* set dimensions and return if empty */
  vec->n    = n;
  vec->vecA = NULL;
  if (!n) return vec;

  /* --- allocate memory for data, init with 0 --- */
  if ((vec->vecA = (short*)malloc(n * sizeof(short)))) {
     int i;
     
     for ( i=0; i<n; i++) vec->vecA[i] = 0;
     return vec;
  }

  /* --- failure --- */
  free( vec);
  SERROR("allocation failure 2 short vector of size %d.\n", n);
  return NULL;
}

ClientData svectorCreateItf( ClientData cd, int argc, char *argv[])
{
  if (argc == 1)  return (ClientData)svectorCreate(0);
  else if ( argc == 2) {
      SVector *vec = NULL;
      int n,k,l;

      if (sscanf(argv[1],"%d",&n) != 1  ||  sscanf(argv[1],"%d%d",&k,&l) == 2) {
	 argc--;
	 if (itfParseArgv( argv[0], &argc, argv+1, 0,
	    "vector", ARGV_CUSTOM,  getSVector, &vec, cd,
		      "@filename or name or definition",
	    NULL) != TCL_OK) return NULL;
	 return (ClientData)vec;
      }
      return (ClientData)svectorCreate(n);
   }
   
   ERROR("give vector or dimension\n");
   return NULL;
}

/* ------------------------------------------------------------------------
    Free
   ------------------------------------------------------------------------ */

void svectorFree( SVector* vec)
{
  if (vec) {
    if ( vec->vecA) free( vec->vecA);
    free(vec);
  }
}

static int svectorFreeItf( ClientData cd )
{
  svectorFree((SVector*)cd);
  return TCL_OK;
}

/* ------------------------------------------------------------------------
    svectorGetItf
   ------------------------------------------------------------------------ */

SVector* svectorGetItf( char* value)
{
  SVector* vec;

  /* ------------------------------------------------------------------------
     If 'value' is not an existing svector object scan vector elements.
     ----------------------------------------------------------------------- */
  if ((vec = (SVector*)itfGetObject( value, "SVector")) == NULL) {
    int     argc;
    char**  argv;

    MSGCLEAR(NULL);

    if ( Tcl_SplitList( itf, value, &argc, &argv) == TCL_OK) {
       int i; int count = 0;

       if (! (vec = svectorCreate( argc))) {
	  free((char*)argv); return NULL;
       }
       
       for ( i = 0; i < argc; i++) {
	  int d;

	  if ( sscanf( argv[i], "%d", &d) != 1) {
	     ERROR("expected 'short' type elements.\n");
	     free((char*)argv);
	     svectorFree( vec);
	     return NULL;
	  }
	  vec->vecA[i] = SLIMIT(d);
	  if (d > SHIGH || d < SLOW) {
	     if (++count < 4) 
		WARN("value %d exceeding short range, set to %hd\n",d,vec->vecA[i]);
	     if (count == 4)
		WARN("more values exceeding short range\n");
	  }
       }
       Tcl_Free((char*)argv);
    }
    return vec;
 }
  /* ------------------------------------------------------------------------
     'value' is an existing svector object.
     ----------------------------------------------------------------------- */
  else {
     SVector* nvec = svectorCreate( vec->n);
     
     if (! nvec) return NULL;
     return svectorCopy( nvec, vec);
  }
}

/* --- to be used with 'itfParseArgv()' --- */
int getSVector( ClientData cd, char* key, ClientData result,
	        int *argcP, char *argv[])
{
   SVector *vec;
   
   if ( (*argcP<1) || ((vec = svectorGetItf( argv[0]))  ==  NULL)) return -1;
   *((SVector**)result) = vec;
   (*argcP)--;   return 0;
}

/* =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
    Short Vector methods
   =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- */
/* ------------------------------------------------------------------------
    svectorPutsItf
   ------------------------------------------------------------------------ */

static int svectorPutsItf( ClientData cd, int argc, char *argv[])
{
  SVector *vec = (SVector*)cd;
  int     i;
  int     index = -1;

  argc--;
  if (itfParseArgv(argv[0], &argc, argv+1, 0,
	"-index", ARGV_INT,   NULL, &index, cd, "",
	NULL) != TCL_OK)  return TCL_ERROR;

  if (index >= vec->n) {
     /* ERROR("index %d out of range 0..%d\n",index,vec->n - 1);
        return TCL_ERROR; */
  }
  else if (index < 0) {
     for ( i = 0; i < vec->n; i++) itfAppendResult("%d ", vec->vecA[i]);
  }
  else itfAppendResult( "%d", vec->vecA[index]);
  return TCL_OK;
}

/* ------------------------------------------------------------------------
    Copy
   ------------------------------------------------------------------------ */

SVector* svectorCopy( SVector* A, SVector* B)
{
  assert(A && B);

  if ( A->n != B->n) svectorResize(A,B->n); 

  if (A->n) memcpy( A->vecA, B->vecA, sizeof(short) * A->n);
  return A;
}

static int svectorCopyItf( ClientData cd, int argc, char *argv[])
{
  SVector* vec = (SVector*)cd;
  SVector* tmp = NULL;

  argc--;
  if (itfParseArgv(argv[0], &argc, argv+1, 0,
		   "<svector>", ARGV_CUSTOM, getSVector, &tmp, cd, "",
		   NULL) != TCL_OK) {
     if ( tmp) svectorFree( tmp);
     return TCL_ERROR;
  }
  svectorCopy( vec, tmp);
  svectorFree( tmp);
  return TCL_OK;
}

/* ------------------------------------------------------------------------
    Swap
   ------------------------------------------------------------------------ */

#define SWAP_SHORT(x)   (short)((short)(0xFF00 & (short)((short)(x)<<8)) | \
                                (short)(0x00FF & (short)((short)(x)>>8)))

static SVector* svectorSwap( SVector* A, SVector* B)
{ 
  int i;
  assert(A && B);

  if ( A->n != B->n) svectorResize(A,B->n); 
  for( i=0; i<A->n; i++) A->vecA[i] = SWAP_SHORT( B->vecA[i]); 
  return A;
}

static int svectorSwapItf( ClientData cd, int argc, char *argv[])
{
  SVector* vec = (SVector*)cd;

  argc--;
  if (itfParseArgv(argv[0], &argc, argv+1, 0,
		   NULL) != TCL_OK) {
     return TCL_ERROR;
  }
  svectorSwap( vec, vec);
  return TCL_OK;
}

/* ------------------------------------------------------------------------
    Set
   ------------------------------------------------------------------------ */

static int svectorSetItf( ClientData cd, int argc, char *argv[])
{
  SVector* vec = (SVector*)cd;
  int    index = 0;
  float  value = 0;
  argc--;

  if (itfParseArgv(argv[0], &argc, argv+1, 0,
	"<index>", ARGV_INT,   NULL, &index, cd, "",
	"<value>", ARGV_FLOAT, NULL, &value, cd, "",
	NULL) != TCL_OK)  return TCL_ERROR;
  if (index < 0 || index >= vec->n) {
     ERROR("index %d out of range 0..%d\n",index,vec->n - 1);
     return TCL_ERROR;
  }
  vec->vecA[index] = SLIMIT(value);
  return TCL_OK;
}

/* ------------------------------------------------------------------------
    Lin
   ------------------------------------------------------------------------ */

SVector* svectorLin( SVector* A, float a, float b)
{
   int i;
   assert(A);
   for( i=0; i<A->n; i++) A->vecA[i] = SLIMIT(a * A->vecA[i] + b); 
   return A;
}

static int svectorLinItf( ClientData cd, int argc, char *argv[])
{
  SVector* vec = (SVector*)cd;
  float a = 0;
  float b = 0;
  
  argc--;
  if (itfParseArgv(argv[0], &argc, argv+1, 0,
	"<a>", ARGV_FLOAT, NULL, &a, cd, "",
	"<b>", ARGV_FLOAT, NULL, &b, cd, "",
	NULL) != TCL_OK)  return TCL_ERROR;

  svectorLin( vec, a, b);
  return TCL_OK;
}

/* ------------------------------------------------------------------------
    AddScalar
   ------------------------------------------------------------------------ */
int svectorAddScalar( SVector* A, int m)
{
  int i;
  assert(A);
  
  for ( i = 0; i < A->n; i++) A->vecA[i] = SLIMIT(A->vecA[i] + m);
  return 0;
}

/* ------------------------------------------------------------------------
    Resize
    Change number of coefficients and copy elements if possible.
   ------------------------------------------------------------------------ */

int svectorResize( SVector* A, int n)
{
  short *p = NULL;

  assert(A && n >= 0);
  if ( A->n == n) return n;

  if (n) {
     if ((p = (short*)malloc(n * sizeof(short)))) {
	int i;
	for ( i = 0; i < n; i++) p[i] = 0.0;
	for ( i = (( A->n < n) ? A->n : n); i > 0; i--) p[i-1] = A->vecA[i-1];
     }
     else {
	SERROR("allocation failure in 'svectorResize'.\n");
	return -1;
     }
  }

  if ( A->vecA) free( A->vecA);
  A->n    = n;
  A->vecA = p;
  return n;
}

static int svectorResizeItf( ClientData cd, int argc, char *argv[])
{
  int n = 0;
  argc--;
  if (itfParseArgv( argv[0], &argc, argv+1, 0,
		    "<dimension>", ARGV_INT, NULL, &n, cd, "",
		   NULL) != TCL_OK)  return TCL_ERROR;
  svectorResize( (SVector*)cd, n);
  return TCL_OK;
}

/* ------------------------------------------------------------------------
    Add
    mode = 0  vector dimensions have to be identical
           1  result vector has dimension = max(dimension(input))
	  -1  result vector has dimension = min(dimension(input))
   ------------------------------------------------------------------------ */
SVector* svectorAdd( SVector* C, float a, SVector* A, float b, SVector* B, int mode)
{
  /* --- check for mode 0 --- */ 
  if ((mode == 0) && (A->n != B->n)) {
     SERROR("dimensions of svectors differ.\n");
     return NULL;
  }
  
  else {
     int  i, result;
     int  min = MIN( A->n, B->n);
     int  max = MAX( A->n, B->n);
     int  dim = (mode>0)?max:min;

     /* --- all modes --- */
     svectorResize( C, dim);
     for ( i = 0; i < min; i++) {
	result     = ROUND(a * A->vecA[i] + b * B->vecA[i]);
	C->vecA[i] = SLIMIT(result);
     }

     /* --- add elements for mode 1 --- */
     if (mode > 0) {
	SVector* M = (A->n > B->n)?A:B;
	float    m = (A->n > B->n)?a:b;
	for ( i = min; i < max; i++) {
	   result     = ROUND(m * M->vecA[i]);
	   C->vecA[i] = SLIMIT(result);
	}
     }
  }
  
  return C;
}

SVector* svectorAppend(SVector* C, SVector* A, SVector* B)
{
  int     i;
  short*  shortP;

  svectorResize(C, A->n+B->n);
  if (C!=A)
  {
    shortP = &(C->vecA[0]);
    for (i=0; i<A->n; i++)
      shortP[i] = A->vecA[i];
  }
  shortP = &(C->vecA[A->n]);
  for (i=0; i<B->n; i++)
    shortP[i] = B->vecA[i];

  return C;
}

int svectorAppendItf(ClientData cd, int argc, char *argv[])
{
  SVector *C = (SVector*)cd;
  SVector *A = NULL;
  SVector *B = NULL;
  int ret    = TCL_OK;
  
  argc--;
  if (itfParseArgv( argv[0], &argc, argv+1, 0,
    "<SVector>", ARGV_CUSTOM, getSVector, &A, cd, "SVector A",
    "<SVector>", ARGV_CUSTOM, getSVector, &B, cd, "SVector B",
    NULL) != TCL_OK) return TCL_ERROR;

  C = svectorAppend(C, A, B);
  if (!C) ret = TCL_ERROR;

  if (A) svectorFree(A);
  if (B) svectorFree(B);
  return ret;
}

static int svectorAddItf( ClientData cd, int argc, char *argv[])
{
  SVector *C = (SVector*)cd;
  SVector *A = NULL, *B = NULL;
  float    a = 0;
  float    b = 0;
  int   mode = 0;
  
  argc--;
  if (itfParseArgv( argv[0], &argc, argv+1, 0,
		   "<a>",        ARGV_FLOAT,  NULL,       &a, cd, "",
		   "<svectorA>", ARGV_CUSTOM, getSVector, &A, cd, "",
		   "<b>",        ARGV_FLOAT,  NULL,       &b, cd, "",
		   "<svectorB>", ARGV_CUSTOM, getSVector, &B, cd, "",
		   "-mode",      ARGV_INT,    NULL,       &mode, cd,
		   "mode 0, 1 or -1 for dimension(result) =, max or min of input",
		   NULL) != TCL_OK) {
     if (A) svectorFree(A);
     if (B) svectorFree(B);
     SERROR("Usage: <float> <SVector> <float> <SVector>.\n");
     return TCL_ERROR;
  }

  C = svectorAdd( C, a, A, b, B, mode);

  svectorFree(A);
  svectorFree(B);
  if (C) return TCL_OK;
  return TCL_ERROR;
}

/* ------------------------------------------------------------------------
    Mul
    div != 0  division instead multiplication
    mode = 0  vector dimensions have to be identical
           1  result vector has dimension = max(dimension(input))
	  -1  result vector has dimension = min(dimension(input))
   ------------------------------------------------------------------------ */
SVector* svectorMul( SVector* C, float a, SVector* A, SVector* B, int mode, int div)
{
  /* --- check for mode 0 --- */ 
  if ((mode == 0) && (A->n != B->n)) {
     SERROR("dimensions of svectors differ.\n");
     return NULL;
  }
  
  else {
     int  i, result;
     int  min = MIN( A->n, B->n);
     int  max = MAX( A->n, B->n);
     int  dim = (mode>0)?max:min;

     /* --- all modes --- */
     svectorResize( C, dim);
     if (div) for ( i = 0; i < min; i++) {
	if (B->vecA[i]!=0) result = ROUND(a * A->vecA[i] / B->vecA[i]);
	else result = a * A->vecA[i] >= 0 ? SHIGH : SLOW;
	C->vecA[i] = SLIMIT(result);
     }
     else for ( i = 0; i < min; i++) {
	result     = ROUND(a * A->vecA[i] * B->vecA[i]);
	C->vecA[i] = SLIMIT(result);
     }

     /* --- add elements for mode 1 --- */
     if (mode > 0) for ( i = min; i < max; i++) C->vecA[i] = 0;
  }
  
  return C;
}

static int svectorMulItf( ClientData cd, int argc, char *argv[])
{
  SVector *C = (SVector*)cd;
  SVector *A = NULL, *B = NULL;
  float    a = 0;
  int   mode = 0;
  int   div  = 0;
  
  argc--;
  if (itfParseArgv( argv[0], &argc, argv+1, 0,
		   "<svectorA>", ARGV_CUSTOM, getSVector, &A, cd, "",
		   "<svectorB>", ARGV_CUSTOM, getSVector, &B, cd, "",
		   "-a",         ARGV_FLOAT,  NULL,       &a, cd, "",
	           "-div",       ARGV_INT,    NULL,       &div,cd,
		   "division instead multiplication",
		   "-mode",      ARGV_INT,    NULL,       &mode, cd,
		   "mode 0, 1 or -1 for dimension(result) =, max or min of input",
		   NULL) != TCL_OK) {
     if (A) svectorFree(A);
     if (B) svectorFree(B);
     SERROR("Usage: <float> <SVector> <float> <SVector>.\n");
     return TCL_ERROR;
  }

  C = svectorMul( C, a, A, B, mode, div);

  svectorFree(A);
  svectorFree(B);
  if (C) return TCL_OK;
  return TCL_ERROR;
}

/* ------------------------------------------------------------------------
    Minmax from .. to
    to = -1 means to end
    ------------------------------------------------------------------------ */
int svectorMinmaxft( SVector* A, short *min, short *max, int from, int to)
{
   int  i;
   short  coef;

   if ((A==NULL) || (A->n == 0)) {
      SERROR("Svector is empty.\n"); return -1;
   }
   from = (from<0) ? 0 : from;
   to   = (to==-1) ? A->n - 1 : to;
   if ((from>=A->n) || (to<from)) {
      SERROR("empty range.\n");      return 0;
   }
   if (to >= A->n) to = A->n - 1;
   *min = *max = A->vecA[from];
   for ( i=from + 1; i<=to; i++) {
      coef = A->vecA[i] ;
      if (*max<coef)      *max = coef;
      else if (*min>coef) *min = coef;
   }
   return (to + 1 - from );
}

/* ------------------------------------------------------------------------
    Minmax
   ------------------------------------------------------------------------ */
int svectorMinmax( SVector* A, short *min, short *max)
{
   int  i;
   short  coef;

   if ((A==NULL) || (A->n == 0)) {
     SERROR("Svector is empty.\n");
     return 0;
  }
  
   *min = *max = A->vecA[0];
   for ( i=1; i<A->n; i++) {
      coef = A->vecA[i] ;
      if (*max<coef)      *max = coef;
      else if (*min>coef) *min = coef;
   }
   return A->n;
}

int svectorMinmaxItf( ClientData cd, int argc, char *argv[])
{
   short min, max;
   SVector *A = (SVector*)cd;

   if (svectorMinmax( A, &min, &max) > 0) {
      itfAppendResult( "%d  %d", min, max);
      return TCL_OK;
   }
   SERROR("SVector empty.\n");
   return TCL_ERROR;
}

/* ------------------------------------------------------------------------
    Power
   ------------------------------------------------------------------------ */
int svectorPower( SVector* A, double *power)
{
   int  i;

   if ((A==NULL) || (A->n == 0)) {
     SERROR("Svector is empty.\n");
     return 0;
  }
  
   *power = 0.0;
   for ( i=0; i<A->n; i++) *power += A->vecA[i] * A->vecA[i];
   *power /= A->n;
   return A->n;
}

int svectorPowerItf( ClientData cd, int argc, char *argv[])
{
   double power;
   SVector *A = (SVector*)cd;

   if (svectorPower( A, &power) > 0) {
      itfAppendResult( "% 2.4e", power);
      return TCL_OK;
   }
   SERROR("SVector empty.\n");
   return TCL_ERROR;
}

/* ------------------------------------------------------------------------
    Mean
   ------------------------------------------------------------------------ */
int svectorMean( SVector* A, double *mean)
{
   int  i;

   if ((A==NULL) || (A->n == 0)) {
     SERROR("Svector is empty.\n");
     return 0;
  }
  
   *mean = 0.0;
   for ( i=0; i<A->n; i++) *mean += A->vecA[i];
   *mean /= A->n;
   return A->n;
}

int svectorMeanItf( ClientData cd, int argc, char *argv[])
{
   double mean;
   SVector *A = (SVector*)cd;

   if (svectorMean( A, &mean) > 0) {
      itfAppendResult( "% 2.4e", mean);
      return TCL_OK;
   }
   SERROR("SVector empty.\n");
   return TCL_ERROR;
}

/* ------------------------------------------------------------------------
    Histogram
    The range from 'low' to 'high' is divided in 'n' segments and the
    samples within each segment are counted in 'histo' which has to be
    allocated before calling this routine.
    Returns the number of samples used for the histogram. Should be
    'to' - 'from' + 1 if all samples are in the segments.
    If 'exc' is true the samples exceedind 'from' or 'to' are also counted
    either in the 0 or the n-1 segment.
    Example: low = 0, high = 100, n = 10
    exc = 0  => segments: 0-9, 10-19, .. , 80-89, 90-100
    exc = 1  => segments:  -9, 10-19, .. , 80-89, 90-
   ------------------------------------------------------------------------ */
int svectorHistogram( SVector* A, int from, int to, short low, short high, int n,
		     int exc, int *histo)
{
   int  count = 0;
   int  d = (high - low);
   short *val;
    
      
   if ((A==NULL) || (A->n == 0)) {
     SERROR("Svector is empty.\n");
     return -1;
  }

   if (to == -1) to = (A->n - 1);
   if (from < 0 || to >= A->n) {SERROR("range error %d..%d.\n",from,to); return -1;}
   if (n <= 0 || d <= 0) {SERROR("argument error n=%d, d=%d.\n",n,d); return -1;}
   
   for ( val = A->vecA+from; val <= A->vecA+to; val++) {
      if (*val < low ) {
	 if (exc) { histo[0]++; count++;}
      }
      else if (*val >= high) {
	 if (exc || *val == high) {histo[n-1]++; count++;}
      }
      else { histo[n*(*val-low)/d]++; count++;}
   }
   return count;
}

int svectorHistogramItf( ClientData cd, int argc, char *argv[])
{
  SVector *vec = (SVector*)cd;
  int low  = 0, high = 0, n = 0;
  int from = 0, to = -1, all = 0;
  int i, *histo;
  
  argc--;
  if (itfParseArgv( argv[0], &argc, argv+1, 0,
      "<low>",    ARGV_INT,    NULL,  &low,   cd,"lower border",
      "<high>",   ARGV_INT,    NULL,  &high,  cd,"upper border",
      "<n>",      ARGV_INT,    NULL,  &n,     cd,"number of segments",
      "-from",    ARGV_INT,    NULL,  &from,  cd,"from sample",
      "-to",      ARGV_INT,    NULL,  &to,    cd,"to sample",
      "-all",     ARGV_INT,    NULL,  &all,   cd,"also count exceeding values",
		   NULL) != TCL_OK) return TCL_ERROR;

  histo= NULL;
  if (n>0) histo = (int*) malloc(sizeof(int) * n);

  if (!histo) ALLOC_ERROR;
  for (i=0; i<n; i++) histo[i] = 0;

  if (svectorHistogram(vec, from, to, (short)low, (short)high, n, all, histo) < 0) {
     free(histo); return TCL_ERROR;
  }
  else {
     itfAppendResult( "{ ");
     for ( i=0; i<n; i++) {
	itfAppendResult( "%d ", histo[i]);
     }
     itfAppendResult( "}");
     free(histo); return TCL_OK;
  }
}	 



/* ------------------------------------------------------------------------
    vectorDisplay
   ------------------------------------------------------------------------ */
static void canvasConfiguration (Tcl_Interp *interp, char *canvas, 
                                 int *width, int *height)
{
  char tmp [512];

  sprintf (tmp, "lindex [%s configure -scrollregion] 4", canvas);
  if (Tcl_Eval (interp, tmp) != TCL_ERROR) {
    //sscanf (interp->result, "%*d %*d %d %d", width, height);
    sscanf (Tcl_GetStringResult (interp), "%*d %*d %d %d", width, height);
  }
  else {
    WARN ("Can't get canvas configuration\n");
  }
}

int svectorDisplay(Tcl_Interp *interp, char *canvas, SVector *vec, int height, int from, int to, int step, float scale, char *tag)
#define displ_y(Y)  ((int)(yoffset - (Y) * yscale))
{
  int   sampleN;
  int   adc_max  = SHIGH + 1;
  int x,i,j;
  float yoffset = height / 2.;
  float yscale  = scale * yoffset / adc_max;
  short y=0;
  
  DString tclCommand;


  from = (from<0) ? 0 : from;
  to   = (to==-1) ? vec->n - 1 : to;
  if ((from>=vec->n) || (to<from)) {
     SERROR("empty range.\n"); return 0;
  }

  sampleN = to + 1 - from;
  if (step <= 0) step = 1;

  dstringInit(&tclCommand);
  dstringAppend(&tclCommand,"%s create line %d %d %d %d -fill red -tag %s\n", canvas, 0, displ_y(0), (int)(sampleN/step), displ_y(0), tag);
  Tcl_Eval(interp,dstringValue(&tclCommand));
  dstringFree(&tclCommand);

  if (step == 1) {
     for ( i=1,x=from; i<sampleN; i++,x++) {
       dstringInit(&tclCommand);
       dstringAppend(&tclCommand,"%s create line %d %d %d %d -tag %s\n", canvas, i-1, displ_y(vec->vecA[x-1]), i, displ_y(vec->vecA[x]), tag);
       Tcl_Eval(interp,dstringValue(&tclCommand));
       dstringFree(&tclCommand);
     }
  }
  else {
     short local_min, local_max;
     int   ymin, ymax;
     local_min = local_max = vec->vecA[0];
     for ( x=0, i=from + step-1;  i<= to;  i+=step, x++) {
	for (j=step-1;j>=0;j--){
	   y = vec->vecA[i-j];
	   if (y > local_max) local_max = y;
	   else if (y < local_min) local_min = y;
	};
	ymin = displ_y(local_min); ymax = displ_y(local_max);
	dstringInit(&tclCommand);
	dstringAppend(&tclCommand,"%s create line %d %d %d %d -tag %s\n", canvas, x, ymin, x + (ymin==ymax?1:0), ymax, tag);
	Tcl_Eval(interp, dstringValue(&tclCommand));
	dstringFree(&tclCommand);
	local_min = local_max = vec->vecA[i];
     }
  }
  return TCL_OK;
}
     
int svectorDisplayItf( ClientData cd, int argc, char *argv[])
{
   SVector *vec = (SVector*)cd;
   char *canvas = NULL;
   int canvas_width, canvas_height;
   int height = 0, from = 0, to = -1, step = 1;
   float scale = 1.0;
   char *tag = "mytag";
   
   argc--;
   if (itfParseArgv( argv[0], &argc, argv+1, 0,
		    "canvas",      ARGV_STRING, NULL,   &canvas, cd, "",
		    "-height",     ARGV_INT,    NULL,   &height, cd, "",
		    "-from",       ARGV_INT,    NULL,   &from,   cd, "",
		    "-to",         ARGV_INT,    NULL,   &to,     cd, "",
		    "-step",       ARGV_INT,    NULL,   &step,   cd, "",
		    "-scale",      ARGV_FLOAT,  NULL,   &scale,  cd, "",
		    "-tag",        ARGV_STRING, NULL,   &tag,    cd, "",
		    NULL) != TCL_OK) {
      ERROR("Usage:%s <canvas> [-option arg].\n", *argv);
      return TCL_ERROR;
   }
   
   /* get width and height of canvas widget */
   if (height <= 0) {
      canvasConfiguration (itf, canvas, &canvas_width, &canvas_height);
      height = canvas_height;
/*      printf("ch= %d\n",canvas_height); */
   }
   return svectorDisplay (itf, canvas, vec, height, from, to, step, scale, tag);
}

/* ========================================================================
    Integer Matrix
   ======================================================================== */

/* ----------------------------------------------------------------------- */
/*  Macro for checking sanity of IMatrix pointer                           */
/* ----------------------------------------------------------------------- */
#define  ASSERT_IMATRIX( X ) {\
          if ((X) == NULL) {\
	     ERROR("Assertion failure: IMatrix pointer (%p) is NULL\n",X);\
             return TCL_ERROR;\
	   } \
	 }


/* -------------------------------------------------------------------- */
/*  imatrixDeinit                                                       */
/* -------------------------------------------------------------------- */
/*
static int  imatrixDeinit (IMatrix *imat) {

  ASSERT_IMATRIX(imat);

  if (imat->matPA != NULL) {
    free(imat->matPA[0]);
    free(imat->matPA);
  }

  imat->matPA = NULL;
  imat->m = 0;
  imat->n = 0;

  return TCL_OK;
}
*/
/* ------------------------------------------------------------------------
    Create
   ------------------------------------------------------------------------ */

IMatrix* imatrixCreate( int m, int n)
{
  /* --- allocate structure --- */
  IMatrix* mat = (IMatrix*)malloc(sizeof(IMatrix));
  if (! mat) {
    SERROR("allocation failure 1 integer matrix of size %d x %d\n.", m, n);
    return NULL;
  }

  /* set dimensions and return if empty */
  if (!(m*n)) {
     mat->matPA = NULL;
     mat->m     = 0;
     mat->n     = 0;
     return mat;
  }
  mat->m = m;
  mat->n = n;

  /* --- allocate memory for data, init with 0 --- */
  if ((mat->matPA = (int**)malloc(m * sizeof(int*)))) {
     if ((mat->matPA[0] = (int*)malloc(m * n * sizeof(int)))) {
	int i,j;
	for ( i=1; i<m; i++) mat->matPA[i] = mat->matPA[i-1] + n;
	for ( i=0; i<m; i++)
	   for ( j = 0; j < n; j++) mat->matPA[i][j] = 0;
	return mat;
     }
     free( mat->matPA);
  }

  /* --- failure --- */
  free( mat);
  SERROR("allocation failure 2 integer matrix of size %d x %d.\n", m, n);
  return NULL;
}

static ClientData imatrixCreateItf( ClientData cd, int argc, char *argv[])
{
  if (argc == 1)  return (ClientData)imatrixCreate(0,0);
  else if (argc == 2) {
     IMatrix *mat = NULL;
     argc--;
     if (itfParseArgv( argv[0], &argc, argv+1, 0,
	"matrix", ARGV_CUSTOM,  getIMatrix, &mat, cd,
		      "@filename or name or definition",
	NULL) != TCL_OK) return NULL;
     return (ClientData)mat;
  }
  else if (argc == 3) {
    int m=0, n=0;
    argc--;
    if (itfParseArgv( argv[0], &argc, argv+1, 0,
       "<m>", ARGV_INT,  NULL, &m, cd, "rows",
       "<n>", ARGV_INT,  NULL, &n, cd, "columns",
       NULL) != TCL_OK) return NULL;
    return (ClientData)imatrixCreate(m,n);
  }
  ERROR("give matrix or <m> <n>.\n");
  return NULL;
}

/* ------------------------------------------------------------------------
    Free
   ------------------------------------------------------------------------ */

void imatrixFree( IMatrix* mat)
{
  if (mat) {
    if ( mat->matPA) {
      if ( mat->matPA[0]) free(mat->matPA[0]);
      free( mat->matPA);
    }
    free(mat);
  }
}

/* -------------------------------------------------------------------- */
/*  imatrixDestroy                                                      */
/* -------------------------------------------------------------------- */
int  imatrixDestroy (IMatrix *imat)  {

  imatrixFree(imat);
  return TCL_OK;
}

/* ------------------------------------------------------------------------
    imatrixConfigureItf
   ------------------------------------------------------------------------ */

static int imatrixConfigureItf( ClientData cd, char *variable, char *value )
{
  IMatrix *mat = (IMatrix*)cd;

  if (! variable) {
    ITFCFG( imatrixConfigureItf, cd, "m");
    ITFCFG( imatrixConfigureItf, cd, "n");
    return TCL_OK;
  }
  ITFCFG_Int(   variable,value,"m",    mat->m,    1);
  ITFCFG_Int(   variable,value,"n",    mat->n,    1);
  
  ERROR("unknown option '-%s %s'.\n", variable,  value ? value : "?");
  return TCL_ERROR;
}

/* -------------------------------------------------------------------- */
/*  imatrixInitConst                                                    */
/* -------------------------------------------------------------------- */
int  imatrixInitConst (IMatrix *imat, int initVal) {

  register int    i,j;

  for (i=0; i<imat->m; i++)
    for (j=0; j<imat->n; j++)
      imat->matPA[i][j] = initVal;

  return TCL_OK;
}



/* -------------------------------------------------------------------- */
/*  imatrixInitDiag                                                     */
/* -------------------------------------------------------------------- */
int  imatrixInitDiag (IMatrix *imat, int initVal) {

  register int    i,j;

  for (i=0; i<imat->m; i++)
    for (j=0; j<imat->n; j++)
      if (i==j)
        imat->matPA[i][j] = initVal;
      else
	imat->matPA[i][j] = 0.0;

  return TCL_OK;
}



/* -------------------------------------------------------------------- */
/*  imatrixInitRandom                                                   */
/* -------------------------------------------------------------------- */
int  imatrixInitRandom (IMatrix *imat, float maxAbsVal) {

  register int    i,j;

  for (i=0; i<imat->m; i++)
    for (j=0; j<imat->n; j++)

      imat->matPA[i][j] = chaosRandom(-maxAbsVal, +maxAbsVal);

  return TCL_OK;
}



/* -------------------------------------------------------------------- */
/*  imatrixTrans      C = At        (C=A is possible)                   */
/* -------------------------------------------------------------------- */
int  imatrixTrans (IMatrix *C, IMatrix *A) {

  IMatrix           *B;
  register int      i,j;
  
  ASSERT_IMATRIX(C);
  ASSERT_IMATRIX(A);

  if (C==A) {
    B = imatrixCreate(A->n,A->m);
    for (i=0; i<A->m; i++)
      for (j=0; j<A->n; j++)
	B->matPA[j][i] = A->matPA[i][j];
    imatrixResize(A,B->m,B->n);
    (void) imatrixCopy(C,B);
    imatrixDestroy(B);
  } else {
    imatrixResize(C,A->n,A->m);
    for (i=0; i<A->m; i++)
      for (j=0; j<A->n; j++)
	C->matPA[j][i] = A->matPA[i][j];
  }
  
  return TCL_OK;
}



/* ------------------------------------------------------------------------ */
/*  imatrixSave                                                             */
/* ------------------------------------------------------------------------ */
int  imatrixSave (IMatrix *A, FILE *fp) {

  register int        i,j;

  ASSERT_IMATRIX(A);

  write_int(fp,A->m);
  write_int(fp,A->n);
  
  for (i=0; i<A->m; i++)
    for (j=0; j<A->n; j++)
      fputc((int) A->matPA[i][j],fp);

  return TCL_OK;
}



/* ------------------------------------------------------------------------ */
/*  imatrixLoad                                                             */
/* ------------------------------------------------------------------------ */
int  imatrixLoad (IMatrix *A, FILE *fp) {

  register int         i,j,m,n;

  ASSERT_IMATRIX(A);

  m = read_int(fp);
  n = read_int(fp);

  imatrixResize(A,m,n);

  for (i=0; i<A->m; i++)
    for (j=0; j<A->n; j++)
      A->matPA[i][j] = (unsigned char) fgetc(fp);

  return TCL_OK;
}



static int imatrixFreeItf( ClientData cd )
{
  imatrixFree((IMatrix*)cd);
  return TCL_OK;
}


/* =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
    Integer Matrix methods
   =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- */

/* ------------------------------------------------------------------------
    imatrixGetItf
   ------------------------------------------------------------------------ */

IMatrix* imatrixGetItf( char* value)
{
  IMatrix* mat;

  /* ------------------------------------------------------------------------
     If 'value' is not an existing imatrix object scan matrix elements.
     ----------------------------------------------------------------------- */
  if ((mat = (IMatrix*)itfGetObject( value, "IMatrix")) == NULL) {
    int     argc;
    char**  argv;

    MSGCLEAR(NULL);

    if ( Tcl_SplitList( itf, value, &argc, &argv) == TCL_OK) {
      int i, j;

      for ( i = 0; i < argc; i++) {
        int    tokc;
        char** tokv;

        if ( Tcl_SplitList( itf, argv[i], &tokc, &tokv) == TCL_OK) {
          if ( i == 0 &&  ! (mat = imatrixCreate( argc, tokc))) {
	     Tcl_Free((char*)argv);  Tcl_Free((char*)tokv);  return NULL;
	  }

          for ( j = 0; j < tokc && j < mat->n; j++) {
            int d;

            if ( sscanf( tokv[j], "%d", &d) != 1) {
	       ERROR("expected 'int' type elements.\n");
	       Tcl_Free((char*)tokv); Tcl_Free((char*)argv);
	       imatrixFree( mat);
	       return NULL;
            }
            mat->matPA[i][j] = d;
          }
          Tcl_Free((char*)tokv);
        }
      }
      Tcl_Free((char*)argv);
    }
    return mat;
  }
  /* ------------------------------------------------------------------------
     'value' is an existing imatrix object.
     ----------------------------------------------------------------------- */
  else {
    IMatrix* nmat = imatrixCreate( mat->m, mat->n);

    if (! nmat) return NULL;
    return imatrixCopy( nmat, mat);
  }
}


/* --- to be used with 'itfParseArgv()' --- */
int getIMatrix( ClientData cd, char* key, ClientData result,
	        int *argcP, char *argv[])
{
   IMatrix *mat;

   if ( (*argcP<1) || ((mat = imatrixGetItf( argv[0]))  ==  NULL)) return -1;
   *((IMatrix**)result) = mat;
   (*argcP)--;   return 0;
}

/* -------------------------------------------------------------------- */
/*  imatrixPuts                                                         */
/* -------------------------------------------------------------------- */

int  imatrixPuts (IMatrix *imat, FILE *fp) {

  register int    i,j;

  ASSERT_IMATRIX(imat);
  
  for (i=0; i<imat->m; i++) {
    fprintf(fp,"{ ");
    for (j=0; j<imat->n; j++)
      fprintf(fp,"%2d ",imat->matPA[i][j]);
    fprintf(fp,"}\\\n");
  }
    
  return TCL_OK;
}

/* ------------------------------------------------------------------------
    imatrixPutsItf
   ------------------------------------------------------------------------ */

int imatrixPutsItf( ClientData cd, int argc, char *argv[])
{
  IMatrix *mat = (IMatrix*)cd;
  int      i,j;

  for ( i = 0; i < mat->m; i++) {
    itfAppendResult( "{ ");
    for ( j = 0; j < mat->n; j++) {
      itfAppendResult( "% 8d ", mat->matPA[i][j]);
    }
    itfAppendResult( "}\n");    
  }
  return TCL_OK;
}


/* ------------------------------------------------------------------------
    Copy
   ------------------------------------------------------------------------ */

IMatrix* imatrixCopy( IMatrix* A, IMatrix* B)
{
  assert(A && B);

  if ( A->n != B->n || A->m != B->m) imatrixResize(A,B->m,B->n); 

  if (A->n * A->m) memcpy( A->matPA[0], B->matPA[0], sizeof(int) * A->n * A->m);
  return A;
}

int imatrixCopyItf( ClientData cd, int argc, char *argv[])
{
  IMatrix* mat = (IMatrix*)cd;
  IMatrix* tmp;

  if ( argc != 2) {
    ERROR("'copy' expects 1 parameter.\n");
    return TCL_ERROR;
  }
  if ((tmp = imatrixGetItf(argv[1]))) {
    imatrixCopy( mat, tmp);
    imatrixFree( tmp);
    return TCL_OK;
  }
  return TCL_ERROR;
}


/* ------------------------------------------------------------------------
    GetVal
   ------------------------------------------------------------------------ */


int imatrixGetValItf( ClientData cd, int argc, char *argv[])
{
  IMatrix     *mat = (IMatrix*)cd;
  int           ac = argc-1;
  int           x = 0,y = 0;

  if (itfParseArgv( argv[0], &ac, argv+1, 0,
    "<1st index>", ARGV_INT, NULL, &x, NULL, "first index",
    "<2nd index>", ARGV_INT, NULL, &y, NULL, "second index",
    NULL) != TCL_OK) return TCL_ERROR;

  if ((x < 0) || (x >= mat->m)) {
    ERROR("Invalid first index\n");
    return TCL_ERROR;
  }

  if ((y < 0) || (y >= mat->n)) {
    ERROR("Invalid second index\n");
    return TCL_ERROR;
  }

  itfAppendResult("%d",mat->matPA[x][y]);

  return TCL_OK;
}

/* ------------------------------------------------------------------------
    SetVal
   ------------------------------------------------------------------------ */
int imatrixSetValItf( ClientData cd, int argc, char *argv[])
{
  IMatrix     *mat = (IMatrix*)cd;
  int           ac = argc-1;
  int           x = 0,y = 0;
  int           val = 0;

  if (itfParseArgv( argv[0], &ac, argv+1, 0,
    "<1st index>", ARGV_INT,   NULL, &x,   NULL, "first index",
    "<2nd index>", ARGV_INT,   NULL, &y,   NULL, "second index",
    "<value>",     ARGV_INT,   NULL, &val, NULL, "value",
    NULL) != TCL_OK) return TCL_ERROR;

  if ((x < 0) || (x >= mat->m)) {
    ERROR("Invalid first index\n");
    return TCL_ERROR;
  }

  if ((y < 0) || (y >= mat->n)) {
    ERROR("Invalid second index\n");
    return TCL_ERROR;
  }

  mat->matPA[x][y] = val;

  return TCL_OK;
}



/* ------------------------------------------------------------------------
    Resize
   ------------------------------------------------------------------------ */

int imatrixResize( IMatrix* A, int m, int n)
{
  int **p = NULL;

  assert(A && n >= 0 && m >= 0);
  if (A->m == m && A->n == n) return (m*n);
  
  if (m*n) {
     if ( (p = (int**)malloc( m * sizeof(int*))) &&
	  (p[0] = (int*)malloc( m * n * sizeof(int)))) {
	   int i,j;
	   for ( i = 1; i < m; i++) p[i] = p[i-1] + n;
	   for ( i = 0; i < m; i++)
	      for ( j = 0; j < n; j++) p[i][j] = 0;
	   
	   for ( i = (( A->m < m) ? A->m : m); i > 0; i--)
	      for ( j = (( A->n < n) ? A->n : n); j > 0; j--) 
		 p[i-1][j-1] = A->matPA[i-1][j-1];
     }
     else {
	if (p) free(p);
	SERROR("allocation failure in 'imatrixResize'.\n");
	return -1;
     }
  }
  else m = n = 0;
	
  if ( A->matPA) {
     if (  A->matPA[0]) free(A->matPA[0]);
     free( A->matPA);
  }
  A->m     = m;
  A->n     = n;
  A->matPA = p;
  return m * n;
}

int imatrixResizeItf( ClientData cd, int argc, char *argv[])
{
  int m, n;

  if ( argc != 3) {

    ERROR("'resize' expects 2 parameters.\n");
    return TCL_ERROR;
  }

  if ( sscanf( argv[1], "%d", &m) != 1 ||
       sscanf( argv[2], "%d", &n) != 1) {

    ERROR( "matrix dimensions must be 'int' type.\n");
    return TCL_ERROR;
  }
  imatrixResize( (IMatrix*)cd, m, n);
  return TCL_OK;
}


/* ------------------------------------------------------------------------
    Clear
   ------------------------------------------------------------------------ */

IMatrix* imatrixClear( IMatrix* A)
{
  int i,j;
  assert(A);

  for (i=0; i<A->m; i++)
    for (j=0; j<A->n; j++)
      A->matPA[i][j] = 0;
  return A;
}

static int imatrixClearItf( ClientData cd, int argc, char *argv[])
{
  IMatrix* mat = (IMatrix*)cd;
  int   ac     =  argc-1;

  if ( itfParseArgv( argv[0], &ac, argv+1, 0,
       NULL) != TCL_OK) return TCL_ERROR;
  imatrixClear( mat); return TCL_OK;
}


/* ========================================================================
    CHAR Matrix
   ======================================================================== */

/* ----------------------------------------------------------------------- */
/*  Macro for checking sanity of IMatrix pointer                           */
/* ----------------------------------------------------------------------- */
#define  ASSERT_CMATRIX( X ) {\
          if ((X) == NULL) {\
	     ERROR("Assertion failure: CMatrix pointer (%p) is NULL\n",X);\
             return TCL_ERROR;\
	   } \
	 }


/* -------------------------------------------------------------------- */
/*  cmatrixDeinit                                                       */
/* -------------------------------------------------------------------- */
/*
static int  cmatrixDeinit (CMatrix *cmat) {

  ASSERT_CMATRIX(cmat);

  if (cmat->matPA != NULL) {
    free(cmat->matPA[0]);
    free(cmat->matPA);
  }

  cmat->matPA = NULL;
  cmat->m = 0;
  cmat->n = 0;

  return TCL_OK;
}
*/
/* ------------------------------------------------------------------------
    Create
   ------------------------------------------------------------------------ */

CMatrix* cmatrixCreate( int m, int n)
{
  /* --- allocate structure --- */
  CMatrix* mat = (CMatrix*)malloc(sizeof(CMatrix));
  if (! mat) {
    SERROR("allocation failure 1 char matrix of size %d x %d\n.", m, n);
    return NULL;
  }

  /* set dimensions and return if empty */
  if (!(m*n)) {
     mat->matPA = NULL;
     mat->m     = 0;
     mat->n     = 0;
     return mat;
  }
  mat->m = m;
  mat->n = n;

  /* --- allocate memory for data, init with 0 --- */
  if ((mat->matPA = (unsigned char**)memalign(8, m * sizeof(char*)))) {
     if ((mat->matPA[0] = (unsigned char*) memalign (8, m * n * sizeof(char)))) {
	int i,j;

	for ( i=1; i<m; i++) mat->matPA[i] = mat->matPA[i-1] + n;
	for ( i=0; i<m; i++)
	   for ( j = 0; j < n; j++) mat->matPA[i][j] = 0;
	return mat;
     }
     free( mat->matPA);
  }

  /* --- failure --- */
  free( mat);
  SERROR("allocation failure 2 char matrix of size %d x %d.\n", m, n);
  return NULL;
}

static ClientData cmatrixCreateItf( ClientData cd, int argc, char *argv[])
{
  if (argc == 1)  return (ClientData)cmatrixCreate(0,0);
  else if (argc == 3) {
    int m=0, n=0;
    argc--;
    if (itfParseArgv( argv[0], &argc, argv+1, 0,
       "<m>", ARGV_INT,  NULL, &m, cd, "rows",
       "<n>", ARGV_INT,  NULL, &n, cd, "columns",
       NULL) != TCL_OK) return NULL;
    return (ClientData)cmatrixCreate(m,n);
  }
  ERROR("give matrix or <m> <n>.\n");
  return NULL;
}

/* ------------------------------------------------------------------------
    Free
   ------------------------------------------------------------------------ */

void cmatrixFree( CMatrix* mat)
{
  if (mat) {
    if ( mat->matPA) {
      if ( mat->matPA[0]) free(mat->matPA[0]);
      free( mat->matPA);
    }
    free(mat);
  }
}

static int cmatrixFreeItf( ClientData cd )
{
  cmatrixFree((CMatrix*)cd);
  return TCL_OK;
}

/* -------------------------------------------------------------------- */
/*  cmatrixDestroy                                                      */
/* -------------------------------------------------------------------- */
int  cmatrixDestroy (CMatrix *cmat)  {

  cmatrixFree(cmat);
  return TCL_OK;
}

/* ------------------------------------------------------------------------
    cmatrixConfigureItf
   ------------------------------------------------------------------------ */

static int cmatrixConfigureItf( ClientData cd, char *variable, char *value )
{
  CMatrix *mat = (CMatrix*)cd;

  if (! variable) {
    ITFCFG( cmatrixConfigureItf, cd, "m");
    ITFCFG( cmatrixConfigureItf, cd, "n");
    return TCL_OK;
  }
  ITFCFG_Int(   variable,value,"m",    mat->m,    1);
  ITFCFG_Int(   variable,value,"n",    mat->n,    1);
  
  ERROR("unknown option '-%s %s'.\n", variable,  value ? value : "?");
  return TCL_ERROR;
}

/* ------------------------------------------------------------------------
    cmatrixPutsItf
   ------------------------------------------------------------------------ */

int cmatrixPutsItf( ClientData cd, int argc, char *argv[])
{
  CMatrix *mat = (CMatrix*)cd;
  int      i,j;

  for ( i = 0; i < mat->m; i++) {
    itfAppendResult( "{ ");
    for ( j = 0; j < mat->n; j++) {
      itfAppendResult( "% 8d ", mat->matPA[i][j]);
    }
    itfAppendResult( "}\n");    
  }
  return TCL_OK;
}

/* ========================================================================
    Float Vector
   ======================================================================== */
/* ------------------------------------------------------------------------
    Create
   ------------------------------------------------------------------------ */
FVector* fvectorCreate( int n)
{
  FVector* vec = (FVector*)malloc(sizeof(FVector));

  /* --- allocate structure --- */
  if (! vec) {
    SERROR("allocation failure 1 float vector of size %d.", n);
    return NULL;
  }
  vec->n    = n;
  vec->vecA = NULL;
  vec->count= 0;
  if (!n) return vec;

  /* --- allocate data, init with 0 --- */
  if ((vec->vecA = (float*)malloc(n * sizeof(float)))) {
     int i;
     
     for ( i = 0; i < n; i++) vec->vecA[i] = 0;
     return vec;
  }

  /* --- failure --- */
  free( vec);
  SERROR("allocation failure 2 float vector of size %d.\n", n);
  return NULL;
}

ClientData fvectorCreateItf( ClientData cd, int argc, char *argv[])
{
  if (argc == 1)
    return (ClientData) fvectorCreate(0);
  else if (argc == 2) {
      FVector *vec = NULL;
      int n; float k,l;

      if (sscanf(argv[1],"%d",&n) != 1  ||  sscanf(argv[1],"%f%f",&k,&l) == 2) {
	 argc--;
	 if (itfParseArgv( argv[0], &argc, argv+1, 0,
	    "vector", ARGV_CUSTOM,  getFVector, &vec, cd,
		      "@filename or name or definition",
	    NULL) != TCL_OK) return NULL;
	 return (ClientData)vec;
      }
      return (ClientData)fvectorCreate(n);
   }
   
   ERROR("give vector or dimension\n");
   return NULL;
}

/* ------------------------------------------------------------------------
    Free
   ------------------------------------------------------------------------ */

void fvectorFree( FVector* vec)
{
  if (vec) {
    if ( vec->vecA) free( vec->vecA);
    free(vec);
  }
}

static int fvectorFreeItf( ClientData cd )
{
  fvectorFree((FVector*)cd);
  return TCL_OK;
}


/* ------------------------------------------------------------------------
    Configure
   ------------------------------------------------------------------------ */

static int fvectorConfigureItf( ClientData cd, char *variable, char *value )
{
  FVector *vec = (FVector*)cd;

  if (! variable) {
    ITFCFG( fvectorConfigureItf, cd, "n");
    ITFCFG( fvectorConfigureItf, cd, "count");
    return TCL_OK;
  }
  ITFCFG_Int(   variable,value,"n",    vec->n,    1);
  ITFCFG_Float( variable,value,"count",vec->count,0);
  
  ERROR("unknown option '-%s %s'.\n", variable,  value ? value : "?");
  return TCL_ERROR;
}
    

/* ------------------------------------------------------------------------
    fvectorGetItf
   ------------------------------------------------------------------------ */

FVector* fvectorGetItf( char* value)
{
  FVector* vec;

  /* ------------------------------------------------------------------------
     If 'value' is not an existing fvector object scan vector elements.
     ----------------------------------------------------------------------- */
  if ((vec = (FVector*)itfGetObject( value, "FVector")) == NULL) {
    int     argc;
    char**  argv;

    MSGCLEAR(NULL);

    if ( Tcl_SplitList( itf, value, &argc, &argv) == TCL_OK) {
       int i;

       if (! (vec = fvectorCreate( argc))) {
	  free((char*)argv); return NULL;
       }
       
       for ( i = 0; i < argc; i++) {
	  float f;

	  if ( sscanf( argv[i], "%f", &f) != 1) {
	     ERROR("expected 'float' type elements.\n");
	     Tcl_Free((char*)argv);
	     fvectorFree( vec);
	     return NULL;
	  }
	  vec->vecA[i] = f;
       }
       Tcl_Free((char*)argv);
    }
    return vec;
 }
  /* ------------------------------------------------------------------------
     'value' is an existing fvector object.
     ----------------------------------------------------------------------- */
  else {
     
     FVector* nvec = fvectorCreate( vec->n);
     
     if (! nvec) return NULL;
     return fvectorCopy( nvec, vec);
  }
}

/* --- to be used with 'itfParseArgv()' --- */
int getFVector( ClientData cd, char* key, ClientData result, int *argcP, char *argv[])
{
   FVector *vec;
   
   if ( (*argcP<1) || ((vec = fvectorGetItf( argv[0]))  ==  NULL)) return -1;
   *((FVector**)result) = vec;
   (*argcP)--;   return 0;
}

int getFVectorP( ClientData cd, char* key, ClientData result,
		 int *argcP, char *argv[])
/* don't free after use */
{
   FVector *vec;
   
   if ( (*argcP<1) || ((vec = (FVector*)itfGetObject( argv[0], "FVector"))  ==  NULL)) return -1;
   *((FVector**)result) = vec;
   (*argcP)--;   return 0;
}

/* =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
    Float Vector methods
   =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- */
/* ------------------------------------------------------------------------
    fvectorPutsItf
   ------------------------------------------------------------------------ */

static int fvectorPutsItf( ClientData cd, int argc, char *argv[])
{
  FVector* vec    = (FVector*)cd;
  char*    format = "% 2.6e";
  char*    middle  = " ";
  int      i;
  argc--;
  if (itfParseArgv( argv[0], &argc, argv+1, 0,
      "-format", ARGV_STRING, NULL, &format, cd, "format string",
      "-middle", ARGV_STRING, NULL, &middle, cd, "between coefficients",
      NULL) != TCL_OK)  return TCL_ERROR;

  for ( i = 0; i < vec->n; i++) {
      if (i) itfAppendResult(middle);
      itfAppendResult(format, vec->vecA[i]);
  }
  return TCL_OK;
}

/* ------------------------------------------------------------------------
    Copy
   ------------------------------------------------------------------------ */

FVector* fvectorCopy( FVector* A, FVector* B)
{
  assert(A && B);

  if ( A->n != B->n) fvectorResize(A,B->n); 

  if (A->n) memcpy( A->vecA, B->vecA, sizeof(float) * A->n);
  A->count = B->count;
  return A;
}

int fvectorCopyItf( ClientData cd, int argc, char *argv[])
{
  FVector* vec = (FVector*)cd;
  FVector* tmp = NULL;

  argc--;
  if (itfParseArgv(argv[0], &argc, argv+1, 0,
		   "<fvector>", ARGV_CUSTOM, getFVector, &tmp, cd, "",
		   NULL) != TCL_OK) {
     if ( tmp) fvectorFree( tmp);
     return TCL_ERROR;
  }
  fvectorCopy( vec, tmp);
  fvectorFree( tmp);
  return TCL_OK;
}

/* ------------------------------------------------------------------------
    Resize
    Change number of coefficients and copy elements if possible.
   ------------------------------------------------------------------------ */

int fvectorResize( FVector* A, int n)
{
  float *p = NULL;

  assert(A && n >= 0);
  if ( A->n == n) return n;

  if (n) {
     if ((p = (float*)malloc(n * sizeof(float)))) {
	int i;
	for ( i = 0; i < n; i++) p[i] = 0.0;
	for ( i = (( A->n < n) ? A->n : n); i > 0; i--) p[i-1] = A->vecA[i-1];
     }
     else {
	SERROR("allocation failure in 'fvectorResize'.\n");
	return -1;
     }
  }

  if ( A->vecA) free( A->vecA);
  A->n    = n;
  A->vecA = p;
  return n;
}

int fvectorResizeItf( ClientData cd, int argc, char *argv[])
{
  int n;
  argc--;
  if (itfParseArgv( argv[0], &argc, argv+1, 0,
		    "<dimension>", ARGV_INT, NULL, &n, cd, "",
		   NULL) != TCL_OK)  return TCL_ERROR;
  fvectorResize( (FVector*)cd, n);
  return TCL_OK;
}

/* ------------------------------------------------------------------------
    Clear
   ------------------------------------------------------------------------ */

FVector* fvectorClear( FVector* A)
{
  int i;
  assert(A);
  for (i=0; i<A->n; i++) A->vecA[i] = 0.0;
  A->count = 0.0;
  return A;
}
/*
static int fvectorClearItf( ClientData cd, int argc, char *argv[])
{
  FVector* mat = (FVector*)cd;
  int   ac     =  argc-1;

  if ( itfParseArgv( argv[0], &ac, argv+1, 0,
       NULL) != TCL_OK) return TCL_ERROR;
  fvectorClear( mat); return TCL_OK;
}
*/
/* ------------------------------------------------------------------------
    Add
    mode = 0  vector dimensions have to be identical
           1  result vector has dimension = max(dimension(input))
	  -1  result vector has dimension = min(dimension(input))
   ------------------------------------------------------------------------ */
FVector* fvectorAdd( FVector* C, float a, FVector* A, float b, FVector* B, int mode)
{
  /* --- check for mode 0 --- */ 
  if ((mode == 0) && (A->n != B->n)) {
     SERROR("dimensions of fvectors differ.\n");
     return NULL;
  }
  
  else {
     int  i;
     int  min = MIN( A->n, B->n);
     int  max = MAX( A->n, B->n);
     int  dim = (mode>0)?max:min;

     /* --- all modes --- */
     fvectorResize( C, dim);
     for ( i = 0; i < min; i++) 
	C->vecA[i] = a * A->vecA[i] + b * B->vecA[i];


     /* --- add elements for mode 1 --- */
     if (mode > 0) {
	FVector* M = (A->n > B->n)?A:B;
	float    m = (A->n > B->n)?a:b;
	for ( i = min; i < max; i++) 
	   C->vecA[i] = m * M->vecA[i];
     }
  }
  
  return C;
}


static int fvectorAddItf( ClientData cd, int argc, char *argv[])
{
  FVector *C = (FVector*)cd;
  FVector *A = NULL, *B = NULL;
  float    a,  b;
  int mode = 0;
  
  argc--;
  if (itfParseArgv( argv[0], &argc, argv+1, 0,
		   "<a>",        ARGV_FLOAT,  NULL,       &a, cd, "",
		   "<fvectorA>", ARGV_CUSTOM, getFVector, &A, cd, "",
		   "<b>",        ARGV_FLOAT,  NULL,       &b, cd, "",
		   "<fvectorB>", ARGV_CUSTOM, getFVector, &B, cd, "",
		   "-mode",      ARGV_INT,    NULL,       &mode, cd,
		   "mode 0, 1 or -1 for dimension(result) =, max or min of input",
		   NULL) != TCL_OK) {
     if (A) fvectorFree(A);
     if (B) fvectorFree(B);
     SERROR("Usage: <float> <FVector> <float> <FVector>.\n");
     return TCL_ERROR;
  }

  C = fvectorAdd( C, a, A, b, B, mode);

  fvectorFree(A);
  fvectorFree(B);
  if (C) return TCL_OK;
  return TCL_ERROR;
}

/* ------------------------------------------------------------------------
    Norm
   ------------------------------------------------------------------------ */
float fvectorNorm( FVector* A)
{
  float norm = 0.0;
  int i;

  assert(A);
  for ( i = 0; i < A->n; i++) 
    norm += A->vecA[i] * A->vecA[i];
  norm = sqrt(norm);

  return norm;
}


static int fvectorNormItf( ClientData cd, int argc, char *argv[])
{
  FVector *A = (FVector*)cd;
  
  argc--;
  if (itfParseArgv( argv[0], &argc, argv+1, 0,
		    NULL) != TCL_OK) return TCL_ERROR;
  itfAppendResult( "% 2.6e", fvectorNorm(A));
  return TCL_OK;
}

/* ========================================================================
    Float Matrix
   ======================================================================== */
/* ------------------------------------------------------------------------
    Create
   ------------------------------------------------------------------------ */

FMatrix* fmatrixCreate( int m, int n)
{
  FMatrix* mat = NULL;
  int i,j;

#ifdef SSE_OPT
  int bofs  = n&3;
  int lsize = (bofs != 0) ? (n^bofs) + 4 : n;
#endif

  /* --- allocate structure --- */
  if ( !(mat = (FMatrix*)malloc(sizeof(FMatrix))) ) {
    SERROR("allocation failure 1 float matrix of size %d x %d\n.", m, n);
    return NULL;
  }

  if (m >= 0 && n >= 0) {
     mat->m = m;
     mat->n = n;
     mat->count = 0.0;

     /* --- allocate memory for data, init with 0 --- */
     if ( m == 0) {
	mat->matPA = NULL;
	return mat;
     }
     if ( m > 0 && (mat->matPA = (float**)memalign(8, m * sizeof(float*))) != NULL) {
	if ( n == 0) {
	   int i;
	   for ( i=1; i<m; i++) mat->matPA[i] = NULL;
	   return mat;
	}
#ifdef SSE_OPT
	if ( n > 0 && (mat->matPA[0] = (float*)_mm_malloc(m * lsize * sizeof(float),16)) != NULL) {
	  for ( i=1; i<m; i++) mat->matPA[i] = mat->matPA[i-1] + lsize;

#else
	if ( n > 0 && (mat->matPA[0] = (float*)memalign(8, m * n * sizeof(float))) != NULL) {
	  for ( i=1; i<m; i++) mat->matPA[i] = mat->matPA[i-1] + n;
#endif
	  for ( j = 0; j < n; j++) mat->matPA[0][j] = 0.0;
	  for ( i=1; i<m; i++)
	    memcpy (mat->matPA[i], mat->matPA[0], n*sizeof(float));
	  return mat;
	}
	free( mat->matPA);
     }
  }

  /* --- failure --- */
  free( mat);
  SERROR("allocation failure 2 float matrix of size %d x %d.\n", m, n);
  return NULL;
}


ClientData fmatrixCreateItf( ClientData cd, int argc, char *argv[])
{
  if (argc == 1)  return (ClientData)fmatrixCreate(0,0);
  else if (argc == 2) {
     FMatrix *mat = NULL;
     argc--;
     if (itfParseArgv( argv[0], &argc, argv+1, 0,
	"matrix", ARGV_CUSTOM,  getFMatrix, &mat, cd,
		      "@filename or name or definition",
	NULL) != TCL_OK) return NULL;
     return (ClientData)mat;
  }
  else if (argc == 3) {
    int m=0, n=0;
    argc--;
    if (itfParseArgv( argv[0], &argc, argv+1, 0,
       "<m>", ARGV_INT,  NULL, &m, cd, "rows",
       "<n>", ARGV_INT,  NULL, &n, cd, "columns",
       NULL) != TCL_OK) return NULL;
    return (ClientData)fmatrixCreate(m,n);
  }
  ERROR("give matrix or <m> <n>.\n");
  return NULL;
}

/* ------------------------------------------------------------------------
    Free
   ------------------------------------------------------------------------ */

void fmatrixFree( FMatrix* mat)
{
  if (mat) {
    if ( mat->matPA) {
      if ( mat->matPA[0])
#ifdef SSE_OPT
        _mm_free(mat->matPA[0]);
#else
        free(mat->matPA[0]);
#endif
      free( mat->matPA);
    }
    free(mat);
  }
}

/* -------------------------------------------------------------------- */
/*  fmatrixDestroy                                                      */
/* -------------------------------------------------------------------- */
int  fmatrixDestroy (FMatrix *fmat) {

  fmatrixFree(fmat);

  return TCL_OK;
}


static int fmatrixFreeItf( ClientData cd )
{
  fmatrixFree((FMatrix*)cd);
  return TCL_OK;
}

/* ------------------------------------------------------------------------
    Configure
   ------------------------------------------------------------------------ */

static int fmatrixConfigureItf( ClientData cd, char *variable, char *value )
{
  FMatrix *mat = (FMatrix*)cd;

  if (! variable) {
    ITFCFG( fmatrixConfigureItf, cd, "m");
    ITFCFG( fmatrixConfigureItf, cd, "n");
    ITFCFG( fmatrixConfigureItf, cd, "count");
    return TCL_OK;
  }
  ITFCFG_Int(   variable,value,"m",    mat->m,    1);
  ITFCFG_Int(   variable,value,"n",    mat->n,    1);
  ITFCFG_Float( variable,value,"count",mat->count,0);
  
  ERROR("unknown option '-%s %s'.\n", variable,  value ? value : "?");
  return TCL_ERROR;
}
    

/* ------------------------------------------------------------------------
    fmatrixGetItf
   ------------------------------------------------------------------------ */

FMatrix* fmatrixGetItf( char* value)
{
  FMatrix* mat;

  /* ------------------------------------------------------------------------
     If 'value' is not an existing fmatrix object scan matrix elements.
     ----------------------------------------------------------------------- */
  if ((mat = (FMatrix*)itfGetObject( value, "FMatrix")) == NULL) {
    int     argc;
    char**  argv;

    MSGCLEAR(NULL);

    if ( Tcl_SplitList( itf, value, &argc, &argv) == TCL_OK) {
      int i, j;

      for ( i = 0; i < argc; i++) {
        int    tokc;
        char** tokv;

        if ( Tcl_SplitList( itf, argv[i], &tokc, &tokv) == TCL_OK) {
          if ( i == 0 &&  ! (mat = fmatrixCreate( argc, tokc))) {
	     Tcl_Free((char*)argv);  Tcl_Free((char*)tokv);  return NULL;
	  }

          for ( j = 0; j < tokc && j < mat->n; j++) {
            float d;

            if ( sscanf( tokv[j], "%f", &d) != 1) {
	       ERROR("expected 'float' type elements.\n");
	       Tcl_Free((char*)tokv); Tcl_Free((char*)argv);
	       fmatrixFree( mat);
	       return NULL;
            }
            mat->matPA[i][j] = d;
          }
          Tcl_Free((char*)tokv);
        }
      }
      Tcl_Free((char*)argv);
    }
    return mat;
  }
  /* ------------------------------------------------------------------------
     'value' is an existing fmatrix object.
     ----------------------------------------------------------------------- */
  else {
    FMatrix* nmat = fmatrixCreate( mat->m, mat->n);

    if (! nmat) return NULL;
    return fmatrixCopy( nmat, mat);
  }
}

/* --- to be used with 'itfParseArgv()' --- */
int getFMatrix( ClientData cd, char* key, ClientData result,
	        int *argcP, char *argv[])
{
   FMatrix *mat;

   if ( (*argcP<1) || ((mat = fmatrixGetItf( argv[0]))  ==  NULL)) return -1;
   *((FMatrix**)result) = mat;
   (*argcP)--;   return 0;
}

int getFMatrixP( ClientData cd, char* key, ClientData result,
		 int *argcP, char *argv[])
/* don't free after use */
{
   FMatrix *mat;
   
   if ( (*argcP<1) || ((mat = (FMatrix*)itfGetObject( argv[0], "FMatrix"))  ==  NULL)) return -1;
   *((FMatrix**)result) = mat;
   (*argcP)--;   return 0;
}

/* =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
    Float Matrix methods
   =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- */


/* ----------------------------------------------------------------------- */
/*  Macro for checking sanity of FMatrix pointer                           */
/* ----------------------------------------------------------------------- */
#define  ASSERT_FMATRIX(X)   {\
           if ((X) == NULL) {\
             ERROR("Assertion failure: FMatrix pointer (%p) is NULL\n",X);\
             return TCL_ERROR;\
	   }\
	 }



/* -------------------------------------------------------------------- */
/*  fmatrixDeinit                                                       */
/* -------------------------------------------------------------------- */
int  fmatrixDeinit (FMatrix *fmat) {

  ASSERT_FMATRIX(fmat);

  if (fmat->matPA != NULL) {
#ifdef SSE_OPT
    _mm_free(fmat->matPA[0]);
#else
    free(fmat->matPA[0]);
#endif
    free(fmat->matPA);
  }

  fmat->matPA = NULL;
  fmat->m = 0;
  fmat->n = 0;

  return TCL_OK;
}



/* -------------------------------------------------------------------- */
/*  fmatrixPuts                                                         */
/* -------------------------------------------------------------------- */
int  fmatrixPuts (FMatrix *fmat, FILE *fp) {

  register int    i,j;

  ASSERT_FMATRIX(fmat);
  
  for (i=0; i<fmat->m; i++) {
    fprintf(fp,"{ ");
    for (j=0; j<fmat->n; j++)
      fprintf(fp,"%12.7f ",fmat->matPA[i][j]);
    fprintf(fp,"}\\\n");
  }
    
  return TCL_OK;
}



/* -------------------------------------------------------------------- */
/*  fmatrixInitConst                                                    */
/* -------------------------------------------------------------------- */
int  fmatrixInitConst (FMatrix *fmat, float initVal) {

  register int    i,j;

  for (i=0; i<fmat->m; i++)
    for (j=0; j<fmat->n; j++)
      fmat->matPA[i][j] = initVal;

  return TCL_OK;
}



/* -------------------------------------------------------------------- */
/*  fmatrixInitDiag                                                     */
/* -------------------------------------------------------------------- */
int  fmatrixInitDiag (FMatrix *fmat, float initVal) {

  register int    i,j;

  for (i=0; i<fmat->m; i++)
    for (j=0; j<fmat->n; j++)
      if (i==j)
        fmat->matPA[i][j] = initVal;
      else
	fmat->matPA[i][j] = 0.0;

  return TCL_OK;
}



/* -------------------------------------------------------------------- */
/*  fmatrixInitRandom                                                   */
/* -------------------------------------------------------------------- */
int  fmatrixInitRandom (FMatrix *fmat, float maxAbsVal) {

  register int    i,j;

  for (i=0; i<fmat->m; i++)
    for (j=0; j<fmat->n; j++)
      fmat->matPA[i][j] = (float) chaosRandom(-maxAbsVal,+maxAbsVal);

  return TCL_OK;
}



/* -------------------------------------------------------------------- */
/*  fmatrixPrune    --   set all items < eta to zero                    */
/* -------------------------------------------------------------------- */
#define   ETA   1E-5
int  fmatrixPrune (FMatrix *fmat) {

  register int        i,j;

  ASSERT_FMATRIX(fmat);

  for (i=0; i<fmat->m; i++)
    for (j=0; j<fmat->n; j++)
      if (fabs(fmat->matPA[i][j]) < ETA)
        fmat->matPA[i][j] = 0.0;

  return TCL_OK;
}



/* -------------------------------------------------------------------- */
/*  fmatrixTransFF      C = At        (C=A is possible)                 */
/* -------------------------------------------------------------------- */
int  fmatrixTransFF (FMatrix *C, FMatrix *A) {

  FMatrix           *B;
  register int      i,j;
  
  ASSERT_FMATRIX(C);
  ASSERT_FMATRIX(A);

  if (C==A) {
    B = fmatrixCreate(A->n,A->m);
    for (i=0; i<A->m; i++)
      for (j=0; j<A->n; j++)
	B->matPA[j][i] = A->matPA[i][j];
    fmatrixResize(A,B->m,B->n);
    (void) fmatrixCopy(C,B);
    fmatrixDestroy(B);
  } else {
    fmatrixResize(C,A->n,A->m);
    for (i=0; i<A->m; i++)
      for (j=0; j<A->n; j++)
	C->matPA[j][i] = A->matPA[i][j];
  }
  
  return TCL_OK;
}



/* -------------------------------------------------------------------- */
/*  fmatrixTransFF1     C = At + additional row of 1s (C=A is possible) */
/* -------------------------------------------------------------------- */
int  fmatrixTransFF1 (FMatrix *C, FMatrix *A) {

  FMatrix           *B;
  register int      i,j;
  
  ASSERT_FMATRIX(C);
  ASSERT_FMATRIX(A);

  if (C==A) {
    B = fmatrixCreate(A->n+1,A->m);
    for (i=0; i<A->m; i++) {
      for (j=0; j<A->n; j++)
	B->matPA[j][i] = A->matPA[i][j];
      B->matPA[A->n][i] = 1.0;
    }
    fmatrixResize(A,B->m,B->n);
    (void) fmatrixCopy(C,B);
    fmatrixDestroy(B);
  } else {
    fmatrixResize(C,A->n+1,A->m);
    for (i=0; i<A->m; i++) {
      for (j=0; j<A->n; j++)
	C->matPA[j][i] = A->matPA[i][j];
      C->matPA[A->n][i] = 1.0;
    }
  }
  
  return TCL_OK;
}



/* -------------------------------------------------------------------- */
/*  fmatrixMSE                                                          */
/* -------------------------------------------------------------------- */
float  fmatrixMSE (FMatrix *fmat) {

  register int       i,j;
  register double    sum;
  
  ASSERT_FMATRIX(fmat);

  sum = 0.0;
  for (i=0; i<fmat->m; i++)
    for (j=0; j<fmat->n; j++)
      sum += sq(fmat->matPA[i][j]);
  
  return (float) (sum / (fmat->m * fmat->n));
}



/* -------------------------------------------------------------------- */
/*  fmatrixScale     C = A * gamma     (C=A is possible)                */
/* -------------------------------------------------------------------- */
int  fmatrixScale (FMatrix *C, FMatrix *A, float gamma) {

  register int      i,j;
  
  ASSERT_FMATRIX(C);
  ASSERT_FMATRIX(A);

  fmatrixResize(C,A->m,A->n);
  for (i=0; i<A->m; i++)
    for (j=0; j<A->n; j++)
      C->matPA[i][j] = A->matPA[i][j] * gamma;
  
  return TCL_OK;
}



/* -------------------------------------------------------------------- */
/*  fmatrixScaleCol                                                     */
/* -------------------------------------------------------------------- */
int  fmatrixScaleCol (FMatrix *C, FMatrix *A, FMatrix *W) {

  register int      i,j;
  
  ASSERT_FMATRIX(C);
  ASSERT_FMATRIX(A);

  if (W->m != 1) {
    WARN ("fmatrixScaleCol(): W has more than one row\n");
  }

  if (W->n != A->n) {
    ERROR("fmatrixScaleCol(): A,W dimensions do not match\n");
    return TCL_ERROR;
  }
  
  fmatrixResize(C,A->m,A->n);
  for (i=0; i<A->m; i++)
    for (j=0; j<A->n; j++)
      C->matPA[i][j] = A->matPA[i][j] * W->matPA[0][j];
  
  return TCL_OK;
}



/* -------------------------------------------------------------------- */
/*  fmatrixAddFF     C = A + B                                          */
/* -------------------------------------------------------------------- */
int  fmatrixAddFF (FMatrix *C, FMatrix *A, FMatrix *B) {

  register int        i,j;
  
  ASSERT_FMATRIX(C);
  ASSERT_FMATRIX(A);
  ASSERT_FMATRIX(B);

  if ((A->m != B->m) || (A->n != B->n)) {
    ERROR("fmatrixAdd(): dimensions do not match\n");
    return TCL_ERROR;
  }

  fmatrixResize(C,A->m,A->n);

  for (i=0; i<A->m; i++)
    for (j=0; j<A->n; j++)
      C->matPA[i][j] = A->matPA[i][j] + B->matPA[i][j];

  return TCL_OK;
}



/* -------------------------------------------------------------------- */
/*  fmatrixAccu     C = C + alpha*A                                     */
/* -------------------------------------------------------------------- */
int  fmatrixAccu (FMatrix *C, FMatrix *A, float alpha) {

  register int        i,j;
  
  ASSERT_FMATRIX(C);
  ASSERT_FMATRIX(A);

  if ((A->m != C->m) || (A->n != C->n)) {
    ERROR("fmatrixAccu(): dimensions do not match\n");
    return TCL_ERROR;
  }

  for (i=0; i<C->m; i++)
    for (j=0; j<C->n; j++)
      C->matPA[i][j] += alpha * A->matPA[i][j];

  return TCL_OK;
}



/* -------------------------------------------------------------------- */
/*  fmatrixAccuScaled    C = C + g_i*A  (rather specialized operation   */
/*                                       used in HME's evaluation)      */
/* -------------------------------------------------------------------- */
int  fmatrixAccuScaled (FMatrix *C, FMatrix *A, FMatrix *g, int index) {

  register int        i,j;
  
  ASSERT_FMATRIX(C);
  ASSERT_FMATRIX(A);
  ASSERT_FMATRIX(g);

  if ((A->m != C->m) || (A->n != C->n) || (A->m != g->m)) {
    ERROR("fmatrixAccuScaled(): dimensions do not match\n");
    return TCL_ERROR;
  }

  if ((index < 0) || (index >= g->n)) {
    ERROR("fmatrixAccuScaled(): invalid gating index %d\n",index);
    return TCL_ERROR;
  }

  for (i=0; i<C->m; i++)
    for (j=0; j<C->n; j++)
      C->matPA[i][j] += g->matPA[i][index] * A->matPA[i][j];

  return TCL_OK;
}



/* -------------------------------------------------------------------- */
/*  fmatrixSub     C = A - B                                            */
/* -------------------------------------------------------------------- */
int  fmatrixSub (FMatrix *C, FMatrix *A, FMatrix *B) {

  register int        i,j;
  
  ASSERT_FMATRIX(C);
  ASSERT_FMATRIX(A);
  ASSERT_FMATRIX(B);

  if ((A->m != B->m) || (A->n != B->n)) {
    ERROR("fmatrixSub(): dimensions do not match\n");
    return TCL_ERROR;
  }

  fmatrixResize(C,A->m,A->n);

  for (i=0; i<A->m; i++)
    for (j=0; j<A->n; j++)
      C->matPA[i][j] = A->matPA[i][j] - B->matPA[i][j];

  return TCL_OK;
}

int fmatrixFromSample (FMatrix *target, FMatrix *source, float threshold) {
  int i,j,cnt=0;
  ASSERT_FMATRIX(target);
  ASSERT_FMATRIX(source);
  fmatrixResize(target, source->m, source->n -1);
  for (i=0; i < source->m; i++) {
    if (source->matPA[i][source->n -1] > threshold) {
      for (j=0; j < source->n -1; j++) {
	target->matPA[cnt][j] = source->matPA[i][j];
      }
      cnt++;
    }
  }
  fmatrixResize(target, cnt, source->n -1);
  
  return TCL_OK;
}
/* -------------------------------------------------------------------- */
/* fmatrixFromSampleItf                                      
    extract all vectors that have a last column exceeding 'threshold'
    into a target FMatrix without this last column
    This method can be used after sample extraction. For 'threshold'
    zero, it will not take any samples that have been extracted with a
    path gamma value of zero.                                           */
/* -------------------------------------------------------------------- */
int fmatrixFromSampleItf (ClientData cd, int argc, char *argv[])
{
  FMatrix *T = (FMatrix*)cd;
  FMatrix *S = NULL;
  float threshold = 0.5;
  int      ret;

  argc--;
  if (itfParseArgv( argv[0], &argc, argv+1, 0,
		   "<fmatrix>",  ARGV_CUSTOM, getFMatrix,   &S,  cd, "",
		   "<a>",        ARGV_FLOAT,  NULL,         &threshold,  cd, "",
		   NULL) != TCL_OK) {
    if (S) fmatrixFree(S);
    ERROR( "Usage: <FMatrix> <float>.\n");
    return TCL_ERROR;
  }
  ret = fmatrixFromSample(T, S, threshold);
  fmatrixFree(S);
  return(ret);
}
  	

/* -------------------------------------------------------------------- */
/*  fmatrixAddVector    C = a * A + b * V                               */
/*   add to each column of the matrix the same scaled vector            */
/*   FMatrix is built as FM->matPA[m][n], where the first index are the */
/*   rows and the second the columns                                    */
/* -------------------------------------------------------------------- */
int  fmatrixAddVector (FMatrix *C, FMatrix *A, float a, FVector *V, float b) {

  register int        x = 0,y = 0;
  
  ASSERT_FMATRIX(C);
  ASSERT_FMATRIX(A);
  if (V->vecA == NULL) { ERROR("fmatrixAddVector: empty FVector"); return TCL_ERROR; }

  fmatrixResize(C,A->m,A->n);

  if (C->m != A->m || V->n != C->m || C->n != A->n) {
    ERROR("fmatrixAddVector(): dimensions do not match\n");
    return TCL_ERROR;
  }

  for (y=0; y < A->m; y++)
    for (x=0; x < A->n; x++)
      C->matPA[y][x] = a * A->matPA[y][x] + b * V->vecA[y];

  return TCL_OK;
}

/* ------------------------------------------------------------------------
    fmatrixAddVectorItf
   ------------------------------------------------------------------------ */
int fmatrixAddVectorItf( ClientData cd, int argc, char *argv[])
{
  FMatrix *C = (FMatrix*)cd;
  FMatrix *A = NULL;
  FVector *V = NULL;
  float    a = 1.0, b = 1.0;
  int      ret;

  argc--;
  if (itfParseArgv( argv[0], &argc, argv+1, 0,
		   "<fmatrixA>", ARGV_CUSTOM, getFMatrix,   &A,  cd, "",
		   "<a>",        ARGV_FLOAT,  NULL,         &a,  cd, "",
		   "<fvectorV>", ARGV_CUSTOM, getFVector,   &V,  cd, "",
		   "<b>",        ARGV_FLOAT,  NULL,         &b,  cd, "",
		   NULL) != TCL_OK) {
     if (A) fmatrixFree(A);
     if (V) fvectorFree(V);
     ERROR( "Usage: <FMatrix> <float> <FVector> <float>.\n");
     return TCL_ERROR;
  }
  ret = fmatrixAddVector( C, A, a, V, b);
  fmatrixFree(A); fvectorFree(V);
  return(ret);
}


/* -------------------------------------------------------------------- */
/*  fmatrixProd     C = A * B  (component-wise)                         */
/* -------------------------------------------------------------------- */
int  fmatrixProd (FMatrix *C, FMatrix *A, FMatrix *B) {

  register int        i,j;
  
  ASSERT_FMATRIX(C);
  ASSERT_FMATRIX(A);
  ASSERT_FMATRIX(B);

  if ((A->m != B->m) || (A->n != B->n)) {
    ERROR("fmatrixProd(): dimensions do not match\n");
    return TCL_ERROR;
  }

  fmatrixResize(C,A->m,A->n);

  for (i=0; i<A->m; i++)
    for (j=0; j<A->n; j++)
      C->matPA[i][j] = A->matPA[i][j] * B->matPA[i][j];

  return TCL_OK;
}



/* -------------------------------------------------------------------- */
/*  fmatrixCosineFF            Computes the cosine of the angle between */
/*                          two given matrices, interpreted as vectors  */
/* -------------------------------------------------------------------- */
float  fmatrixCosineFF (FMatrix *A, FMatrix *B) {

  float               *a,*b;
  register double     sum,sumA,sumB;
  register int        i,j;

  ASSERT_FMATRIX(A);
  ASSERT_FMATRIX(B);

  if ((A->m != B->m) || (A->n != B->n)) {
    ERROR("fmatrixCosine(): matrices have different size\n");
    return 0.0;
  }

  sum = sumA = sumB = 0.0;

  for (i=0; i<A->m; i++) {
    a = A->matPA[i];
    b = B->matPA[i];
    for (j=0; j<A->n; j++) {
      sum  += a[j]*b[j];
      sumA += sq(a[j]);
      sumB += sq(b[j]);
    }
  }

  return ((float) (sum/(sqrt(sumA)*sqrt(sumB))));
}



/* -------------------------------------------------------------------- */
/*  fmatrixMulFFT  C = A * Bt  B might be a NN weight matrix containing */
/*                            a bias weight column at the far right     */
/* -------------------------------------------------------------------- */
int  fmatrixMulFFT (FMatrix *C, FMatrix *A, FMatrix *B) {

  double  a00,a01,a02,a03,a10,a11,a12,a13,
          a20,a21,a22,a23,a30,a31,a32,a33;
  double  b00,b01,b02,b03,b10,b11,b12,b13,
          b20,b21,b22,b23,b30,b31,b32,b33;
  double  c00,c01,c02,c03,c10,c11,c12,c13,
          c20,c21,c22,c23,c30,c31,c32,c33;
  float   *a0,*a1,*a2,*a3;
  float   *b0,*b1,*b2,*b3;
  float   *c0,*c1,*c2,*c3;
  int     i,j,k,mA,mB,n,bias;

  ASSERT_FMATRIX(C);
  ASSERT_FMATRIX(A);
  ASSERT_FMATRIX(B);

  if ((C==A) || (C==B)) {
    ERROR("fmatrixMulT(): C must be different from A and B\n");
    return TCL_ERROR;
  }

  if ((A->n != B->n) && (A->n+1 != B->n)) {
    ERROR("fmatrixMulT(): dimensions do not match\n");
    return TCL_ERROR;
  }

  if (A->n+1 == B->n) bias=1; else bias=0;
  fmatrixResize(C,A->m,B->m);
  fmatrixInitConst(C,0.0);

  /* ------------------------------------- */
  /*  first pass in chunks of 4x4 elements */
  /* ------------------------------------- */
  mA = 4 * (A->m / 4);
  mB = 4 * (B->m / 4);
  n = 4 * (A->n / 4);
  for (k=0; k<n; k+=4) {
    for (i=0; i<mA; i+=4) {
      
      /* -------------------------------- */
      /*  fetch: get partial matrix in A  */
      /* -------------------------------- */

      a0 = A->matPA[i]+k;   a1 = A->matPA[i+1]+k;
      a2 = A->matPA[i+2]+k; a3 = A->matPA[i+3]+k;
      a00 = a0[0]; a01 = a0[1]; a02 = a0[2]; a03 = a0[3];
      a10 = a1[0]; a11 = a1[1]; a12 = a1[2]; a13 = a1[3];
      a20 = a2[0]; a21 = a2[1]; a22 = a2[2]; a23 = a2[3];
      a30 = a3[0]; a31 = a3[1]; a32 = a3[2]; a33 = a3[3];      

      for (j=0; j<mB; j+=4) {

	/* -------------------------------- */
	/*  fetch: get partial matrix in B  */
	/* -------------------------------- */
	
        b0 = B->matPA[j]+k;   b1 = B->matPA[j+1]+k; 
        b2 = B->matPA[j+2]+k; b3 = B->matPA[j+3]+k;
        b00 = b0[0]; b01 = b0[1]; b02 = b0[2]; b03 = b0[3];
        b10 = b1[0]; b11 = b1[1]; b12 = b1[2]; b13 = b1[3];
        b20 = b2[0]; b21 = b2[1]; b22 = b2[2]; b23 = b2[3];
        b30 = b3[0]; b31 = b3[1]; b32 = b3[2]; b33 = b3[3];      

	/* -------------------------------------- */
	/*  compute partial matrix multipliation  */	
	/* -------------------------------------- */

        c0 = C->matPA[i]+j;
        c00 = a00*b00 + a01*b01 + a02*b02 + a03*b03;
	c01 = a00*b10 + a01*b11 + a02*b12 + a03*b13;
	c02 = a00*b20 + a01*b21 + a02*b22 + a03*b23;
	c03 = a00*b30 + a01*b31 + a02*b32 + a03*b33;
	c0[0] += c00; c0[1] += c01; c0[2] += c02; c0[3] += c03;

        c1 = C->matPA[i+1]+j;
        c10 = a10*b00 + a11*b01 + a12*b02 + a13*b03;
	c11 = a10*b10 + a11*b11 + a12*b12 + a13*b13;
	c12 = a10*b20 + a11*b21 + a12*b22 + a13*b23;
	c13 = a10*b30 + a11*b31 + a12*b32 + a13*b33;
	c1[0] += c10; c1[1] += c11; c1[2] += c12; c1[3] += c13;
        
        c2 = C->matPA[i+2]+j;
        c20 = a20*b00 + a21*b01 + a22*b02 + a23*b03;
	c21 = a20*b10 + a21*b11 + a22*b12 + a23*b13;
	c22 = a20*b20 + a21*b21 + a22*b22 + a23*b23;
	c23 = a20*b30 + a21*b31 + a22*b32 + a23*b33;
	c2[0] += c20; c2[1] += c21; c2[2] += c22; c2[3] += c23;
        
        c3 = C->matPA[i+3]+j;
        c30 = a30*b00 + a31*b01 + a32*b02 + a33*b03;
	c31 = a30*b10 + a31*b11 + a32*b12 + a33*b13;
	c32 = a30*b20 + a31*b21 + a32*b22 + a33*b23;
	c33 = a30*b30 + a31*b31 + a32*b32 + a33*b33;
	c3[0] += c30; c3[1] += c31; c3[2] += c32; c3[3] += c33;

      }

      /* ------------------------------------------ */
      /*  exception: handle the remaining rows of B */
      /* ------------------------------------------ */
      for (j=mB; j<B->m; j++) {
        b0 = B->matPA[j] + k;
	b00 = b0[0]; b01 = b0[1]; b02 = b0[2]; b03 = b0[3];
	C->matPA[i][j]   += a00*b00 + a01*b01 + a02*b02 + a03*b03;
	C->matPA[i+1][j] += a10*b00 + a11*b01 + a12*b02 + a13*b03;
	C->matPA[i+2][j] += a20*b00 + a21*b01 + a22*b02 + a23*b03;
	C->matPA[i+3][j] += a30*b00 + a31*b01 + a32*b02 + a33*b03;
      }

    }
    
    /* ------------------------------------------ */
    /*  exception: handle the remaining rows of A */
    /* ------------------------------------------ */
    for (i=mA; i<A->m; i++) {
      a0 = A->matPA[i] + k;
      a00 = a0[0]; a01 = a0[1]; a02 = a0[2]; a03 = a0[3];

      for (j=0; j<mB; j+=4) {

	/* -------------------------------- */
	/*  fetch: get partial matrix in B  */
	/* -------------------------------- */
	
        b0 = B->matPA[j]+k;   b1 = B->matPA[j+1]+k;
        b2 = B->matPA[j+2]+k; b3 = B->matPA[j+3]+k;
        b00 = b0[0]; b01 = b0[1]; b02 = b0[2]; b03 = b0[3];
        b10 = b1[0]; b11 = b1[1]; b12 = b1[2]; b13 = b1[3];
        b20 = b2[0]; b21 = b2[1]; b22 = b2[2]; b23 = b2[3];
        b30 = b3[0]; b31 = b3[1]; b32 = b3[2]; b33 = b3[3];      

	/* -------------------------------------- */
	/*  compute partial matrix multipliation  */	
	/* -------------------------------------- */

        c0 = C->matPA[i]+j;
        c00 = a00*b00 + a01*b01 + a02*b02 + a03*b03;
	c01 = a00*b10 + a01*b11 + a02*b12 + a03*b13;
	c02 = a00*b20 + a01*b21 + a02*b22 + a03*b23;
	c03 = a00*b30 + a01*b31 + a02*b32 + a03*b33;
	c0[0] += c00; c0[1] += c01; c0[2] += c02; c0[3] += c03;

      }

      /* ------------------------------------------ */
      /*  exception: handle the remaining rows of B */
      /* ------------------------------------------ */
      for (j=mB; j<B->m; j++) {
        b0 = B->matPA[j] + k;
	b00 = b0[0]; b01 = b0[1]; b02 = b0[2]; b03 = b0[3];
	C->matPA[i][j]   += a00*b00 + a01*b01 + a02*b02 + a03*b03;
      }
      
    }    
  }
  
  /* ---------------------------------------- */
  /*  second pass for the remaining elements  */
  /* ---------------------------------------- */
  if (bias) {
    for (j=0; j<A->m; j++) {
      for (k=0; k<B->m; k++) {
        a0 = A->matPA[j];
        b0 = B->matPA[k];
	c0 = C->matPA[j] + k;
        for (i=n; i<A->n; i++)
  	  *c0 += a0[i] * b0[i];
        *c0 += b0[B->n-1];
      }
    }
  } else {
    for (j=0; j<A->m; j++) {
      for (k=0; k<B->m; k++) {
        a0 = A->matPA[j];
        b0 = B->matPA[k];
	c0 = C->matPA[j] + k;
        for (i=n; i<A->n; i++)
  	  *c0 += a0[i] * b0[i];
      }
    }
  }

  return TCL_OK;
}



/* -------------------------------------------------------------------- */
/*  fmatrixMulFF   C = A * B   B might be a NN weight matrix containing */
/*                            a bias weight column at the far right     */
/* -------------------------------------------------------------------- */
int  fmatrixMulFF (FMatrix *C, FMatrix *A, FMatrix *B) {

  FMatrix     *D;
  
  ASSERT_FMATRIX(C);
  ASSERT_FMATRIX(A);
  ASSERT_FMATRIX(B);

  if ((C==A) || (C==B)) {
    ERROR("fmatrixMul(): C must be different from A and B\n");
    return TCL_ERROR;
  }

  if ((A->n != B->m) && (A->n != B->m+1)) {
    ERROR("fmatrixMul(): dimensions do not match\n");
    return TCL_ERROR;
  }

  D = fmatrixCreate(B->n,B->m);
  fmatrixTransFF(D,B);
  fmatrixMulFFT(C,A,D);
  fmatrixDestroy(D);

  return TCL_OK;
}



/* -------------------------------------------------------------------- */
/*  fmatrixMulFF1   C = A * B     B is considered to have an additional */
/*                             implicit column of 1's on the far right  */
/* -------------------------------------------------------------------- */
int  fmatrixMulFF1 (FMatrix *C, FMatrix *A, FMatrix *B) {

  FMatrix     *D;
  
  ASSERT_FMATRIX(C);
  ASSERT_FMATRIX(A);
  ASSERT_FMATRIX(B);

  if ((C==A) || (C==B)) {
    ERROR("fmatrixMul1(): C must be different from A and B\n");
    return TCL_ERROR;
  }

  if (A->n != B->m) {
    ERROR("fmatrixMul1(): dimensions do not match\n");
    return TCL_ERROR;
  }

  D = fmatrixCreate(B->n+1,B->m);
  fmatrixTransFF1(D,B);
  fmatrixMulFFT(C,A,D);
  fmatrixDestroy(D);

  return TCL_OK;
}



/* -------------------------------------------------------------------- */
/*  fmatrixRadT  C = (A - Bt)^2           B might be a NN weight matrix */
/*                           a variance weight column at the far right  */
/* -------------------------------------------------------------------- */
int  fmatrixRadT (FMatrix *C, FMatrix *A, FMatrix *B) {

  register float  a00,a01,a02,a03,a10,a11,a12,a13,
                  a20,a21,a22,a23,a30,a31,a32,a33;
  register float  b00,b01,b02,b03,b10,b11,b12,b13,
                  b20,b21,b22,b23,b30,b31,b32,b33;
  register float  c00,c01,c02,c03,c10,c11,c12,c13,
                  c20,c21,c22,c23,c30,c31,c32,c33;
  float           *a,*b,*c;
  register float  var;
  register int    i,j,k,mA,mB,n,bias;

  ASSERT_FMATRIX(C);
  ASSERT_FMATRIX(A);
  ASSERT_FMATRIX(B);

  if ((C==A) || (C==B)) {
    ERROR("fmatrixRadT(): C must be different from A and B\n");
    return TCL_ERROR;
  }

  if ((A->n != B->n) && (A->n+1 != B->n)) {
    ERROR("fmatrixRadT(): dimensions do not match\n");
    return TCL_ERROR;
  }

  if (A->n+1 == B->n) bias=1; else bias=0;
  fmatrixResize(C,A->m,B->m);
  fmatrixInitConst(C,0.0);

  /* ------------------------------------- */
  /*  first pass in chunks of 4x4 elements */
  /* ------------------------------------- */
  mA = 4 * (A->m / 4);
  mB = 4 * (B->m / 4);
  n = 4 * (B->n / 4);
  for (k=0; k<n; k+=4) {
    for (i=0; i<mA; i+=4) {
      
      /* -------------------------------- */
      /*  fetch: get partial matrix in A  */
      /* -------------------------------- */

      a = A->matPA[i]+k;
      a00 = a[0]; a01 = a[1]; a02 = a[2]; a03 = a[3];
      a = A->matPA[i+1]+k;
      a10 = a[0]; a11 = a[1]; a12 = a[2]; a13 = a[3];
      a = A->matPA[i+2]+k;
      a20 = a[0]; a21 = a[1]; a22 = a[2]; a23 = a[3];
      a = A->matPA[i+3]+k;
      a30 = a[0]; a31 = a[1]; a32 = a[2]; a33 = a[3];      

      for (j=0; j<mB; j+=4) {

	/* -------------------------------- */
	/*  fetch: get partial matrix in B  */
	/* -------------------------------- */
	
        b = B->matPA[j]+k;
        b00 = b[0]; b01 = b[1]; b02 = b[2]; b03 = b[3];
        b = B->matPA[j+1]+k;
        b10 = b[0]; b11 = b[1]; b12 = b[2]; b13 = b[3];
        b = B->matPA[j+2]+k;
        b20 = b[0]; b21 = b[1]; b22 = b[2]; b23 = b[3];
        b = B->matPA[j+3]+k;
        b30 = b[0]; b31 = b[1]; b32 = b[2]; b33 = b[3];      

	/* -------------------------------------- */
	/*  compute partial matrix multipliation  */	
	/* -------------------------------------- */

        c00 = sq(a00-b00) + sq(a01-b01) + sq(a02-b02) + sq(a03-b03);
	c01 = sq(a00-b10) + sq(a01-b11) + sq(a02-b12) + sq(a03-b13);
	c02 = sq(a00-b20) + sq(a01-b21) + sq(a02-b22) + sq(a03-b23);
	c03 = sq(a00-b30) + sq(a01-b31) + sq(a02-b32) + sq(a03-b33);

        c10 = sq(a10-b00) + sq(a11-b01) + sq(a12-b02) + sq(a13-b03);
	c11 = sq(a10-b10) + sq(a11-b11) + sq(a12-b12) + sq(a13-b13);
	c12 = sq(a10-b20) + sq(a11-b21) + sq(a12-b22) + sq(a13-b23);
	c13 = sq(a10-b30) + sq(a11-b31) + sq(a12-b32) + sq(a13-b33);
        
        c20 = sq(a20-b00) + sq(a21-b01) + sq(a22-b02) + sq(a23-b03);
	c21 = sq(a20-b10) + sq(a21-b11) + sq(a22-b12) + sq(a23-b13);
	c22 = sq(a20-b20) + sq(a21-b21) + sq(a22-b22) + sq(a23-b23);
	c23 = sq(a20-b30) + sq(a21-b31) + sq(a22-b32) + sq(a23-b33);
        
        c30 = sq(a30-b00) + sq(a31-b01) + sq(a32-b02) + sq(a33-b03);
	c31 = sq(a30-b10) + sq(a31-b11) + sq(a32-b12) + sq(a33-b13);
	c32 = sq(a30-b20) + sq(a31-b21) + sq(a32-b22) + sq(a33-b23);
	c33 = sq(a30-b30) + sq(a31-b31) + sq(a32-b32) + sq(a33-b33);

	/* ----------------------------------------------- */
	/*  write back: update full matrix multiplication  */
	/* ----------------------------------------------- */
        c = C->matPA[i]+j;
	c[0] += c00; c[1] += c01; c[2] += c02; c[3] += c03;
        c = C->matPA[i+1]+j;
	c[0] += c10; c[1] += c11; c[2] += c12; c[3] += c13;
        c = C->matPA[i+2]+j;
	c[0] += c20; c[1] += c21; c[2] += c22; c[3] += c23;
        c = C->matPA[i+3]+j;
	c[0] += c30; c[1] += c31; c[2] += c32; c[3] += c33;

      }

      /* ------------------------------------------ */
      /*  exception: handle the remaining rows of B */
      /* ------------------------------------------ */
      for (j=mB; j<B->m; j++) {
        b = B->matPA[j] + k;
	b00 = b[0]; b01 = b[1]; b02 = b[2]; b03 = b[3];
	C->matPA[i][j]   += sq(a00-b00) + sq(a01-b01) + sq(a02-b02) + sq(a03-b03);
	C->matPA[i+1][j] += sq(a10-b00) + sq(a11-b01) + sq(a12-b02) + sq(a13-b03);
	C->matPA[i+2][j] += sq(a20-b00) + sq(a21-b01) + sq(a22-b02) + sq(a23-b03);
	C->matPA[i+3][j] += sq(a30-b00) + sq(a31-b01) + sq(a32-b02) + sq(a33-b03);
      }

    }
    
    /* ------------------------------------------ */
    /*  exception: handle the remaining rows of A */
    /* ------------------------------------------ */
    for (i=mA; i<A->m; i++) {
      a = A->matPA[i] + k;
      a00 = a[0]; a01 = a[1]; a02 = a[2]; a03 = a[3];

      for (j=0; j<mB; j+=4) {

	/* -------------------------------- */
	/*  fetch: get partial matrix in B  */
	/* -------------------------------- */
	
        b = B->matPA[j]+k;
        b00 = b[0]; b01 = b[1]; b02 = b[2]; b03 = b[3];
        b = B->matPA[j+1]+k;
        b10 = b[0]; b11 = b[1]; b12 = b[2]; b13 = b[3];
        b = B->matPA[j+2]+k;
        b20 = b[0]; b21 = b[1]; b22 = b[2]; b23 = b[3];
        b = B->matPA[j+3]+k;
        b30 = b[0]; b31 = b[1]; b32 = b[2]; b33 = b[3];      

	/* -------------------------------------- */
	/*  compute partial matrix multipliation  */	
	/* -------------------------------------- */

        c00 = sq(a00-b00) + sq(a01-b01) + sq(a02-b02) + sq(a03-b03);
	c01 = sq(a00-b10) + sq(a01-b11) + sq(a02-b12) + sq(a03-b13);
	c02 = sq(a00-b20) + sq(a01-b21) + sq(a02-b22) + sq(a03-b23);
	c03 = sq(a00-b30) + sq(a01-b31) + sq(a02-b32) + sq(a03-b33);

	/* ----------------------------------------------- */
	/*  write back: update full matrix multiplication  */
	/* ----------------------------------------------- */
        c = C->matPA[i]+j;
	c[0] += c00; c[1] += c01; c[2] += c02; c[3] += c03;

      }

      /* ------------------------------------------ */
      /*  exception: handle the remaining rows of B */
      /* ------------------------------------------ */
      for (j=mB; j<B->m; j++) {
        b = B->matPA[j] + k;
	b00 = b[0]; b01 = b[1]; b02 = b[2]; b03 = b[3];
	C->matPA[i][j]   += sq(a00-b00) + sq(a01-b01) + sq(a02-b02) + sq(a03-b03);
      }
      
    }    
  }
  
  /* ---------------------------------------- */
  /*  second pass for the remaining elements  */
  /* ---------------------------------------- */
  if (bias) {
    for (j=0; j<A->m; j++) {
      for (k=0; k<B->m; k++) {
        a = A->matPA[j];
        b = B->matPA[k];
	c = C->matPA[j] + k;
        for (i=n; i<A->n; i++)
  	  *c += sq(a[i] - b[i]);
        if ((var = b[B->n-1]) < 0.0)
          ERROR("fmatrixRadT(): negative variance\n");
        *c *= var;
      }
    }
  } else {
    for (j=0; j<A->m; j++) {
      for (k=0; k<B->m; k++) {
        a = A->matPA[j];
        b = B->matPA[k];
	c = C->matPA[j] + k;
        for (i=n; i<B->n; i++)
  	  *c += sq(a[i] - b[i]);
      }
    }
  }

  return TCL_OK;
}



/* -------------------------------------------------------------------- */
/*  fmatrixRad    C = A * B    B might be a NN weight matrix containing */
/*                           a variance weight column at the far right  */
/* -------------------------------------------------------------------- */
int  fmatrixRad (FMatrix *C, FMatrix *A, FMatrix *B) {

  FMatrix     *D;
  
  ASSERT_FMATRIX(C);
  ASSERT_FMATRIX(A);
  ASSERT_FMATRIX(B);

  if ((C==A) || (C==B)) {
    ERROR("fmatrixRad(): C must be different from A and B\n");
    return TCL_ERROR;
  }

  if ((A->n != B->m) && (A->n+1 != B->m)) {
    ERROR("fmatrixRad(): dimensions do not match\n");
    return TCL_ERROR;
  }

  D = fmatrixCreate(B->n,B->m);
  fmatrixTransFF(D,B);
  fmatrixRadT(C,A,D);
  fmatrixDestroy(D);

  return TCL_OK;
}



/* ------------------------------------------------------------------------ */
/*  fmatrixSave                                                             */
/* ------------------------------------------------------------------------ */
int  fmatrixSave (FMatrix *A, FILE *fp) {

  register int        i,j;

  ASSERT_FMATRIX(A);

  write_int(fp,A->m);
  write_int(fp,A->n);
  
  for (i=0; i<A->m; i++)
    for (j=0; j<A->n; j++)
      write_float(fp,A->matPA[i][j]);

  return TCL_OK;
}



/* ------------------------------------------------------------------------ */
/*  fmatrixLoadFF                                                           */
/* ------------------------------------------------------------------------ */
int  fmatrixLoadFF (FMatrix *A, FILE *fp) {

  register int         i,j,m,n;

  ASSERT_FMATRIX(A);

  m = read_int(fp);
  n = read_int(fp);

  fmatrixResize(A,m,n);

  for (i=0; i<A->m; i++)
    for (j=0; j<A->n; j++)
      A->matPA[i][j] = read_float(fp);

  return TCL_OK;
}

int float_log_normalize(float* vec, int size)
{
   int j;
   int max_v = 0;
   float norm = 0.0;

   // find the max value
   max_v = vec[0];
   for (j=1; j < size; j++) {
      if (vec[j] > max_v)
         max_v = vec[j];
   }

   // compute log(norm) factor
   norm = 0.0;
   for (j=0; j < size; j++) {
      norm += exp(vec[j] - max_v);
   }
   norm = log(norm) + max_v;

   // normalize
   for (j=0; j < size; j++)
      vec[j] = exp(vec[j] - norm);

   return TCL_OK;
}

// Start Wilson 15 Aug 05
int  fmatrixLogNormalize(FMatrix *A)
{
   int i, j;

   for (i=0; i < A->m; i++) {
      float* vec = A->matPA[i];

      // use log score
      for (j=0; j < A->n; j++)
         vec[j] = -vec[j];

      float_log_normalize(vec, A->n);
   }

   return TCL_OK;
}
// End Wilson 15 Aug 05



/* ------------------------------------------------------------------------
    fmatrixPutsItf
   ------------------------------------------------------------------------ */

int fmatrixPutsItf( ClientData cd, int argc, char *argv[])
{
  FMatrix *mat = (FMatrix*)cd;
  char*    format = "% 2.6e";
  char*    left   = "{ ";
  char*    right  = " }";
  char*    middle  = " ";
  int      i,j;
  int      ib = 0, ie = mat->m - 1;
  int      jb = 0, je = mat->n - 1;
  argc--;
  if (itfParseArgv( argv[0], &argc, argv+1, 0,
      "-ib",     ARGV_INT,    NULL, &ib,     cd, "start row",
      "-ie",     ARGV_INT,    NULL, &ie,     cd, "end row",
      "-jb",     ARGV_INT,    NULL, &jb,     cd, "start column",
      "-je",     ARGV_INT,    NULL, &je,     cd, "end column",
      "-format", ARGV_STRING, NULL, &format, cd, "format string",
      "-left",   ARGV_STRING, NULL, &left,   cd, "left side",
      "-right",  ARGV_STRING, NULL, &right,  cd, "right side",
      "-middle", ARGV_STRING, NULL, &middle, cd, "between coefficients",
      NULL) != TCL_OK)  return TCL_ERROR;

  if (ib < 0) ib = 0;  if (ie >= mat->m) ie = mat->m - 1;
  if (jb < 0) jb = 0;  if (je >= mat->m) je = mat->n - 1;
  for ( i = ib; i <= ie; i++) {
    if (i!=ib) itfAppendResult("\n");
    itfAppendResult(left);
    for ( j = jb; j <= je; j++) {
      if (j!=jb) itfAppendResult(middle);
      itfAppendResult(format, mat->matPA[i][j]);
    }
    itfAppendResult(right);    
  }
  return TCL_OK;
}


/* ------------------------------------------------------------------------
    Window
   --------
    Copies the window at 1st index, 2nd index (size from Matrix2)
    from the original matrix to the new matrix
   ------------------------------------------------------------------------ */

int fmatrixWindowItf (ClientData cd, int argc, char *argv[])
{
  int        ac = argc-1;
  FMatrix* mat1 = (FMatrix*) cd;
  FMatrix* mat2 = NULL;
  int x = 0, y = 0, i, j;

  if (itfParseArgv (argv[0], &ac, argv+1, 0,
    "<FMatrix>",   ARGV_OBJECT, NULL, &mat2, "FMatrix", "float matrix",
    "<1st index>", ARGV_INT,    NULL, &x,    NULL,      "first index",
    "<2nd index>", ARGV_INT,    NULL, &y,    NULL,      "second index",
    NULL) != TCL_OK) return TCL_ERROR;

  for (i = 0; i < mat1->m && i+x < mat2->m; i++)
    for (j = 0; j < mat1->n && j+y < mat2->n; j++)
      mat1->matPA[i][j] = mat2->matPA[i+x][j+y];
 
  return TCL_OK;
}


/* ------------------------------------------------------------------------
     Square
   ----------
     Makes a square matrix from a frame or vice-versa
   ------------------------------------------------------------------------ */

int fmatrixSquareItf (ClientData cd, int argc, char *argv[])
{
  int        ac = argc-1;
  FMatrix* mat1 = (FMatrix*) cd;
  FMatrix* mat2 = NULL;
  int       idx = 0;
  int i, j;

  if (itfParseArgv (argv[0], &ac, argv+1, 0,
    "<FMatrix>",   ARGV_OBJECT, NULL, &mat2, "FMatrix", "float matrix",
     "-index",     ARGV_INT,    NULL, &idx,  NULL,      "index",
    NULL) != TCL_OK) return TCL_ERROR;

  if        (mat1->m == mat1->n && mat2->n == mat1->m * mat1->n) {
    for (i = 0; i < mat1->m; i++)
      for (j = 0; j < mat1->n; j++)
	mat1->matPA[i][j]             = mat2->matPA[idx][i*mat1->n+j];
  } else if (mat2->m == mat2->n && mat1->n == mat2->m * mat2->n) {
    for (i = 0; i < mat2->m; i++)
      for (j = 0; j < mat2->n; j++)
	mat1->matPA[idx][i*mat2->n+j] = mat2->matPA[i][j];
  } else {
    ERROR ("fmatrixSquare: dimensions do not match\n");
    return TCL_ERROR;
  }
 
  return TCL_OK;
}


/* ------------------------------------------------------------------------
    Copy
   ------------------------------------------------------------------------ */

FMatrix* fmatrixCopy( FMatrix* A, FMatrix* B)
{
  assert(A && B);

  if ( A->n != B->n || A->m != B->m) fmatrixResize(A,B->m,B->n); 
#ifdef SSE_OPT
  if (A->n * A->m) {
    int i;
    for (i=0; i<A->m; i++)
      memcpy( A->matPA[i], B->matPA[i], sizeof(float) * A->n);
  }
#else
  if (A->n * A->m) memcpy( A->matPA[0], B->matPA[0], sizeof(float) * A->n * A->m);
#endif
  A->count = B->count;
  return A;
}

int fmatrixCopyItf( ClientData cd, int argc, char *argv[])
{
  FMatrix* mat = (FMatrix*)cd;
  FMatrix* tmp;

  if ( argc != 2) {
    ERROR("'copy' expects 1 parameter.\n");
    return TCL_ERROR;
  }
  if ((tmp = fmatrixGetItf(argv[1]))) {
    fmatrixCopy( mat, tmp);
    fmatrixFree( tmp);
    return TCL_OK;
  }
  return TCL_ERROR;
}

/* ------------------------------------------------------------------------
    Modulo
   ------------------------------------------------------------------------ */

int fmatrixModulo( FMatrix* A, int maxCount, int s)
{
  int i;

  /* Sanity checks */
  assert(A && maxCount > 0);
  if ((A->n == 0) || ((A->m-s) <= maxCount) || !A->matPA)
    return (A->m*A->n);

#ifdef SSE_OPT
  ERROR("fmatrixModulo is not SSE_OPT-save.\n");
  exit (1);
#endif

  /* Compact the data in memory */
  for (i = s; i < maxCount; i++) {
    int j = s + ((float)(i-s)) * ((float)(A->m-s)) / ((float)maxCount);
    int k;
    for (k = 0; k < A->n; k++)
      A->matPA[i][k] = A->matPA[j][k];
  }

  /* Free memory */
  A->matPA[0] = (float*) realloc (A->matPA[0], (s+maxCount) * A->n * sizeof(float));
  for (i = 1; i < (s+maxCount); i++)
    A->matPA[i] = A->matPA[i-1] + A->n;
  A->m     = s+maxCount;

  return A->n * (s+maxCount);
}

int fmatrixModuloItf (ClientData cd, int argc, char *argv[]) {
  int   ac = argc-1;
  float mo = -1.0;
  int   ma = -1.0;
  int    s = 0;

  if (itfParseArgv (argv[0], &ac, argv+1, 0,
    "-mod",   ARGV_FLOAT, NULL, &mo, NULL, "modulo factor",
    "-max",   ARGV_INT,   NULL, &ma, NULL, "maximum count",
    "-start", ARGV_INT,   NULL, &s,  NULL, "don't module first data",
    NULL) != TCL_OK) return TCL_ERROR;

  if ((mo <= 0 && ma <= 0) || (mo > 0 && ma > 0)) {
    ERROR ("fmatrixModuloItf: specify one of -mod or -max\n");
    return TCL_ERROR;
  } else if (s < 0 || s >= ((FMatrix*)cd)->m) {
    ERROR ("fmatrixModuloItf: -start should be 0 ... %d\n", ((FMatrix*)cd)->m);
    return TCL_ERROR;
  } else
    ma = (mo <= 1) ? (((FMatrix*)cd)->m) : (((FMatrix*)cd)->m - s) / mo;
  if (!ma) ma = 1;

  fmatrixModulo ((FMatrix*) cd, ma, s);

  return TCL_OK;
}


/* ------------------------------------------------------------------------
    GetVal
   ------------------------------------------------------------------------ */

int fmatrixGetValItf( ClientData cd, int argc, char *argv[])
{
  FMatrix     *mat = (FMatrix*)cd;
  int           ac = argc-1;
  int           x = 0,y = 0;

  if (itfParseArgv( argv[0], &ac, argv+1, 0,
    "<1st index>", ARGV_INT, NULL, &x, NULL, "first index",
    "<2nd index>", ARGV_INT, NULL, &y, NULL, "second index",
    NULL) != TCL_OK) return TCL_ERROR;

  if ((x < 0) || (x >= mat->m)) {
    ERROR("Invalid first index\n");
    return TCL_ERROR;
  }

  if ((y < 0) || (y >= mat->n)) {
    ERROR("Invalid second index\n");
    return TCL_ERROR;
  }

  itfAppendResult("%f",mat->matPA[x][y]);

  return TCL_OK;
}

/* ------------------------------------------------------------------------
    SetVal
   ------------------------------------------------------------------------ */
int fmatrixSetValItf( ClientData cd, int argc, char *argv[])
{
  FMatrix     *mat = (FMatrix*)cd;
  int           ac = argc-1;
  int           x = 0,y = 0;
  float         val = 0.0;

  if (itfParseArgv( argv[0], &ac, argv+1, 0,
    "<1st index>", ARGV_INT,   NULL, &x,   NULL, "first index",
    "<2nd index>", ARGV_INT,   NULL, &y,   NULL, "second index",
    "<value>",     ARGV_FLOAT, NULL, &val, NULL, "value",
    NULL) != TCL_OK) return TCL_ERROR;

  if ((x < 0) || (x >= mat->m)) {
    ERROR("Invalid first index\n");
    return TCL_ERROR;
  }

  if ((y < 0) || (y >= mat->n)) {
    ERROR("Invalid second index\n");
    return TCL_ERROR;
  }

  mat->matPA[x][y] = val;

  return TCL_OK;
}

/* ------------------------------------------------------------------------
    Resize
   ------------------------------------------------------------------------ */

int fmatrixResize( FMatrix* A, int m, int n)
{
  float **p = NULL;
  int i,j;

#ifdef SSE_OPT
  int bofs  = n&3;
  int lsize = (bofs!=0) ? (n^bofs) + 4 : n;
#endif

  assert(A && n >= 0 && m >= 0);
  if (A->m == m && A->n == n) return (m*n);
  
  if (m*n) {
     if ( (p = (float**)malloc( m * sizeof(float*))) &&
#ifdef SSE_OPT
          (p[0] = (float*)_mm_malloc( m * lsize * sizeof(float), 16))) {
	   for ( i = 1; i < m; i++) p[i] = p[i-1] + lsize;
#else
          (p[0] = (float*)malloc( m * n * sizeof(float)))) {
	   for ( i = 1; i < m; i++) p[i] = p[i-1] + n;
#endif
	   for ( i = 0; i < m; i++)
	      for ( j = 0; j < n; j++) p[i][j] = 0.0;
	   
	   for ( i = (( A->m < m) ? A->m : m); i > 0; i--)
	      for ( j = (( A->n < n) ? A->n : n); j > 0; j--) 
		 p[i-1][j-1] = A->matPA[i-1][j-1];
     }
     else {
	if (p) free(p);
	SERROR("allocation failure in 'fmatrixResize'.\n");
	return TCL_ERROR;
     }
  }
  else m = n = 0;
	
  if ( A->matPA) {
     if (  A->matPA[0])
#ifdef SSE_OPT
       _mm_free(A->matPA[0]);
#else
       free(A->matPA[0]);
#endif
     free( A->matPA);
  }
  A->m     = m;
  A->n     = n;
  A->matPA = p;
  return m * n;
}

int fmatrixResizeItf( ClientData cd, int argc, char *argv[])
{
  int           ac = argc-1;
  int m = 0, n = 0;

  if (itfParseArgv( argv[0], &ac, argv+1, 0,
    "<1st dimension>", ARGV_INT,   NULL, &m,   NULL, "first index",
    "<2nd dimension>", ARGV_INT,   NULL, &n,   NULL, "second index",
    NULL) != TCL_OK) return TCL_ERROR;

  fmatrixResize( (FMatrix*)cd, m, n);

  return TCL_OK;
}

/* ------------------------------------------------------------------------
    Clear
   ------------------------------------------------------------------------ */

FMatrix* fmatrixClear( FMatrix* A)
{
  int i,j;
  assert(A);

  for (i=0; i<A->m; i++)
    for (j=0; j<A->n; j++)
      A->matPA[i][j] = 0.0;
  A->count = 0.0;
  return A;
}

static int fmatrixClearItf( ClientData cd, int argc, char *argv[])
{
  FMatrix* mat = (FMatrix*)cd;
  int   ac     =  argc-1;

  if ( itfParseArgv( argv[0], &ac, argv+1, 0,
       NULL) != TCL_OK) return TCL_ERROR;
  fmatrixClear( mat); return TCL_OK;
}

/* ------------------------------------------------------------------------
    Refill
   ------------------------------------------------------------------------ */

int fmatrixRefill( FMatrix* A, int m, int n, float* data)
{
#ifdef SSE_OPT
  ERROR("fmatrixRefill is not SSE_OPT-save.\n");
  exit (1);
#endif
  if ( A->matPA) {
     if (  A->matPA[0]) free(A->matPA[0]);
     free( A->matPA);
  }
  if ((A->matPA = (float**)malloc(m * sizeof(float*)))) {
     int i;
     for ( i = 0; i < m; i++) A->matPA[i] = data + (i*n);
     A->m = m;
     A->n = n;
     return 0;
  }
  return -1;
}

/* ------------------------------------------------------------------------
    Load
   ------------------------------------------------------------------------ */
static FMatrix* fmatrixLoad( FMatrix *A, char *name)
{
   FILE *fp;
   int i,j;
   FMatrix* tmp;
   assert(A);

   if (( fp=fileOpen(name,"r") )==NULL) {
      SERROR("Can't open fmatrix file %s for reading\n",name);
      return NULL;
   }
   fscanf(fp, "%d %d",&i,&j);

   INFO("Reading (%dx%d)-matrix from %s.\n",i,j,name);
   
   if ((tmp = fmatrixCreate(i,j)) == NULL) {
      fileClose (name, fp); return NULL;
   }
   
   for (i=0; i<tmp->m; i++)
      for (j=0; j<tmp->n; j++)
         if (fscanf(fp, "%f",&(tmp->matPA[i][j])) != 1) {
	    SERROR("Can't read element %d,%d of matrix %s.\n",i,j,name);
	    SERROR("Don't read anything.\n");
	    fmatrixFree(tmp); return NULL;
         }
   /* if (fscanf(fp, "%f",&(tmp->count)) != 1) tmp->count=0.0; */
   fileClose (name, fp);

   fmatrixCopy(A,tmp); fmatrixFree(tmp);
   return A;
}

static int fmatrixLoadItf( ClientData cd, int argc, char *argv[])
{
  FMatrix* mat = (FMatrix*)cd;

  if ( argc != 2) {
    ERROR("Usage: <fmatrix> %s <filename>.\n",argv[0]);
    return TCL_ERROR;
  }
  if ( !fmatrixLoad( mat, argv[1]))  
    { ERROR("couldn't load matrix from file %s\n",argv[1]); return TCL_ERROR; }
  return TCL_OK;
}

/* ------------------------------------------------------------------------
    Meanvar
   ------------------------------------------------------------------------ */

int fmatrixMeanvar( FMatrix* mat, FMatrix* weight, FVector* mean, FVector* smean, FVector* dev)
{
  int i,j,M;

  double m,sm, count = 0.0,sdev = 0.0;
  DVector *meanTemp  = NULL;
  DVector *smeanTemp = NULL;

  smeanTemp = dvectorCreate(mat->n);
  meanTemp  = dvectorCreate(mat->n);

  /* Copy the contents of smean and mean to smeanTemp and meanTemp */
  smeanTemp->n     = smean->n;
  smeanTemp->count = smean->count;
  for (i = 0; i < smean->n; i++) {
	  smeanTemp->vecA[i] = (double) smean->vecA[i];
  }

  meanTemp->n     = mean->n;
  meanTemp->count = mean->count;
  for (i = 0; i < mean->n; i++) {
	  meanTemp->vecA[i] = (double) mean->vecA[i];
  }

  /* Incrementally calculate mean and smean */
  assert(mat && mean && dev && smean);
  M = mat->m;
  if (weight != NULL) {
    if (weight->n == 0) ERROR("weight-feature empty!\n");
    if (weight->m != M) ERROR("weight-feature has %d frames not %d!\n", weight->m, M);
  }
  if ( mat->n != mean->n)   fvectorResize(mean,   mat->n); 
  if ( mat->n != dev->n )   fvectorResize(dev,    mat->n); 
  if ( mat->n != smean->n ) fvectorResize(smean,  mat->n); 
  for (j=0; j<mat->n; j++) mean->vecA[j] = smean->vecA[j] = dev->vecA[j] = 0.0;

  if (weight == NULL) {
    for (i=0; i<M; i++)
      for (j=0; j<mat->n; j++) {
        m               = mat->matPA[i][j];
        meanTemp->vecA[j]  += m;
        smeanTemp->vecA[j] += (m * m);
      }
     count = M;
  } else {
    for (i=0; i<M; i++) {
      double w = weight->matPA[i][0];

      if (w != 0.0) {
        for (j=0; j<mat->n; j++) {
          m               = mat->matPA[i][j];
          meanTemp->vecA[j]  += (w * m);
          smeanTemp->vecA[j] += (w * m * m);
        }
        count += w;
      }
    }
  }
  if (count!=0)  for (j=0; j<mat->n; j++) {
    m              = meanTemp->vecA[j] / count;
    sm             = smeanTemp->vecA[j] / count;
    mean->vecA[j]  = m;
    smean->vecA[j] = sm;
    sdev           = sm - m * m;
    assert(!(sdev < 0));
    dev->vecA[j]   = sqrt(sdev);
  }
  mean->count = count;
  smean->count = count;
  dev->count = count;
  dvectorFree(smeanTemp);
  dvectorFree(meanTemp);
  return mat->m;
}


/* ------------------------------------------------------------------------
    Dev
   ------------------------------------------------------------------------ */

static int fmatrixDev( FMatrix* dev, FMatrix* mean, FMatrix* smean)
{
  int i,j;
  assert(dev && mean && smean);
  assert(mean->n == smean->n && mean->m == smean->m);

  fmatrixResize(dev,mean->m,mean->n); 
  for (i=0; i<mean->m; i++)
     for (j=0; j<mean->n; j++) {
	double d = smean->matPA[i][j] - mean->matPA[i][j]*mean->matPA[i][j];
	if (d>=0) dev->matPA[i][j]   = sqrt(d);
	else {
	   ERROR("for element %d,%d smean < mean*mean!\n",i,j);
	   return TCL_ERROR;
	}
     }
  return TCL_OK;
}

static int fmatrixDevItf( ClientData cd, int argc, char *argv[])
{
  FMatrix *C = (FMatrix*)cd;
  FMatrix *A = NULL, *B = NULL;
  int ret    = TCL_OK;

  argc--;
  if (itfParseArgv( argv[0], &argc, argv+1, 0,
    "<matrix>", ARGV_CUSTOM, getFMatrix, &A, cd, "mean values",
    "<matrix>", ARGV_CUSTOM, getFMatrix, &B, cd, "squared mean values",
    NULL) != TCL_OK) ret = TCL_ERROR;
  else 
    if (fmatrixDev( C, A, B) != TCL_OK) ret = TCL_ERROR;
  if (A) fmatrixFree(A);
  if (B) fmatrixFree(B);
  return ret;
}

/* ------------------------------------------------------------------------
    Transpose
   ------------------------------------------------------------------------ */

FMatrix* fmatrixTrans( FMatrix* A, FMatrix* B)
{
  int i,j;
  assert(A && B);

  if (A==B) return NULL;
  if ( A->n != B->m || A->m != B->n) fmatrixResize(A,B->n,B->m); 

  for (i=0; i<A->m; i++)
     for (j=0; j<A->n; j++)
	A->matPA[i][j] = B->matPA[j][i];
  return A;
}

int fmatrixTransItf( ClientData cd, int argc, char *argv[])
{
  FMatrix* mat = (FMatrix*)cd;
  FMatrix* tmp;

  if ( argc != 2) {
    ERROR( "expecting fmatrix.\n");
    return TCL_ERROR;
  }
  if ((tmp = fmatrixGetItf(argv[1]))) {
    fmatrixTrans( mat, tmp);
    fmatrixFree( tmp);
    return TCL_OK;
  }
  return TCL_ERROR;
}

/* ------------------------------------------------------------------------
    Add
   ------------------------------------------------------------------------ */

FMatrix* fmatrixAdd( FMatrix* C, float a, FMatrix* A, float b, FMatrix* B)
{
  int i,j;
   
  assert(A && B && C);

  if ( A->n != B->n || A->m != B->m) {
     SERROR("Matrix sizes differ.\n");
     return NULL;
  }
  fmatrixResize(C,A->m,A->n);

  for ( i = 0; i < C->m; i++) {
     for ( j = 0; j < C->n; j++) {
	C->matPA[i][j] = a * A->matPA[i][j] + b * B->matPA[i][j];
     }
  }
  return C;
}

int fmatrixAddItf( ClientData cd, int argc, char *argv[])
{
  FMatrix *C = (FMatrix*)cd;
  FMatrix *A = NULL, *B = NULL;
  float    a = 1,  b = 1;

  argc--;
  if (itfParseArgv( argv[0], &argc, argv+1, 0,
		   "<a>",        ARGV_FLOAT,  NULL,         &a,  cd, "",
		   "<fmatrixA>", ARGV_CUSTOM, getFMatrix,   &A,  cd, "",
		   "<b>",        ARGV_FLOAT,  NULL,         &b,  cd, "",
		   "<fmatrixB>", ARGV_CUSTOM, getFMatrix,   &B,  cd, "",
		   NULL) != TCL_OK) {
     if (A) fmatrixFree(A);
     if (B) fmatrixFree(B);
     ERROR( "Usage: <float> <FMatrix> <float> <FMatrix>.\n");
     return TCL_ERROR;
  }
  C = fmatrixAdd( C, a, A, b, B);
  fmatrixFree(A);
  fmatrixFree(B);
  if (C) return TCL_OK;
  return TCL_ERROR;
}

/* ------------------------------------------------------------------------
    VTS
   ------------------------------------------------------------------------ */

FMatrix* fmatrixVTS( FMatrix* C, float a, FMatrix* A, float b, FMatrix* B)
{
  int i,j;
  /* float mod; */

  assert(A && B && C);

  if ( A->n != B->n || A->m != B->m) {
     SERROR("Matrix sizes differ.\n");
     return NULL;
  }
  fmatrixResize(C,A->m,A->n);

  for ( i = 0; i < C->m; i++) {
     for ( j = 0; j < C->n; j++) {
	C->matPA[i][j] = (float) - log10D(1.0 + exp10D((double) -A->matPA[i][j] - (double) B->matPA[i][j]));
     }
  }
  return C;
}

int fmatrixVTSItf( ClientData cd, int argc, char *argv[])
{
  FMatrix *C = (FMatrix*)cd;
  FMatrix *A = NULL, *B = NULL;
  float    a = 1,  b = 1;

  argc--;
  if (itfParseArgv( argv[0], &argc, argv+1, 0,
		   "<a>",        ARGV_FLOAT,  NULL,         &a,  cd, "",
		   "<fmatrixA>", ARGV_CUSTOM, getFMatrix,   &A,  cd, "",
		   "<b>",        ARGV_FLOAT,  NULL,         &b,  cd, "",
		   "<fmatrixB>", ARGV_CUSTOM, getFMatrix,   &B,  cd, "",
		   NULL) != TCL_OK) {
     if (A) fmatrixFree(A);
     if (B) fmatrixFree(B);
     ERROR( "Usage: <float> <FMatrix> <float> <FMatrix>.\n");
     return TCL_ERROR;
  }
  C = fmatrixVTS( C, a, A, b, B);
  fmatrixFree(A);
  fmatrixFree(B);
  if (C) return TCL_OK;
  return TCL_ERROR;
}


/* ----------------------------------------------------------------------
   EFVR (Early Feature Vector Reduction)
   -------------------------------------
   Reduce number of rows of a fmarix representing the feature
   vectors
   ---------------------------------------------------------------------- */
FMatrix* fmatrixEFVR(FMatrix* A, float *weight, float threshold, float *thres, float boost, float decrease, float maxboost, int shrink)
{
  FMatrix* B = NULL;
  int      i,j,k,bi,count,wi,oi,ci;
  float    diff,maxdiff,boostval,tempthres;

  B = fmatrixCreate(A->m,A->n);
  tempthres = threshold;
  if (B) {
    //maxdiff  =  0;
    //avgvalue =  0;
    //maxi     = -1;
    //maxj     = -1;
    bi       =  0;
    //oi       =  0;
    ci       =  0;

    for (k=0; k<A->n; k++) { /* copy the first frame 1:1 */
      B->matPA[0][k] = A->matPA[0][k];
    }
    count=1;

    if (weight) {weight[bi]=1;} /* give the first frame a weight of 1 */

    for (i=1; i<A->m; i++) { /* go through the rest of frames */
      maxdiff = 0;
      for (j=0; j<A->n; j++) {  /* compute distance */
	diff = (B->matPA[bi][j] - A->matPA[i][j]);
	maxdiff += (diff*diff) / ((float)j*decrease+1);
      }

      if (thres) { /* are we using a dynamic theshold vector ? */
	tempthres = threshold * (1-thres[i]);
      }
      if (maxdiff>=tempthres) { /* is difference bigger than threshold ? */

	boostval = 1+((float)(count-1)*boost);
	if (boostval>maxboost) boostval=maxboost;
	if (weight) {weight[bi]=boostval;}
	if (shrink) {count=1;}  /* then write out */
	oi = bi;
	for (wi=0; wi<count-1; wi++) {
	  bi++;
	  for (k=0; k<A->n; k++) {
	    B->matPA[bi][k] = B->matPA[oi][k];
	  }
	  if (weight) {weight[bi]=boostval;
	  }
	}
	bi++;
	for (k=0; k<A->n; k++) {
	  B->matPA[bi][k] = A->matPA[i][k];
	}
	count=1;
      }
      else { /* difference is smaller then threshold, so merge frames */
	count++;
	ci++;
	for (k=0; k<A->n; k++) {
	  B->matPA[bi][k] = (B->matPA[bi][k] * (count-1) + A->matPA[i][k]) / count;
	}
      }
    }

    boostval = 1+((float)(count-1)*boost);
    if (boostval>maxboost) boostval=maxboost;
    if (weight) {weight[bi]=boostval;}
    if (shrink) {count=1;}  /* then write out */
    oi = bi;
    for (wi=0; wi<count-1; wi++) {
      bi++;
      for (k=0; k<A->n; k++) {
	B->matPA[bi][k] = B->matPA[oi][k];
      }
      if (weight) {weight[bi]=boostval;
      }
    }
    bi++;

    fmatrixResize(B,bi,B->n);
    INFO("Cutted %d of %d feature vectors away! (%d real) (boost=%f, decrease=%f, shrink=%d, maxboost=%f)\n",i-bi,i,ci,boost,decrease,shrink,maxboost);
    return B;
  }
  return NULL;
}


/* ------------------------------------------------------------------------
    Mulcoef       multiply each coefficient
    div != 0  division instead multiplication
    mode = 0  number of rows have to be identical
           1  result has max(rows(input)) rows
	  -1  result has min(rows(input)) rows
   ------------------------------------------------------------------------ */
FMatrix* fmatrixMulcoef( FMatrix* C, float a, FMatrix* A, FMatrix* B, int mode, int div)
{
  /* --- check for mode 0 --- */ 
  if (A->n != B->n || ((mode == 0) && (A->m != B->m))) {
     SERROR("dimensions differ %dx%d and %dx%d.\n",A->m,A->n,B->m,B->n);
     return NULL;
  }
  
  else {
     int  i, j;
     int  count = 0;
     int  min = MIN( A->m, B->m);
     int  max = MAX( A->m, B->m);
     int  dim = (mode>0)?max:min;

     /* --- all modes --- */
     fmatrixResize( C, dim, A->n);
     if (div)
	for ( i = 0; i < min; i++)
	   for ( j = 0; j < A->n; j++)
	      if (B->matPA[i][j]!=0)
		 C->matPA[i][j] = a * A->matPA[i][j] / B->matPA[i][j];
	      else 
	         { C->matPA[i][j] = 0; count++;}
     else
	for ( i = 0; i < min; i++)
	   for ( j = 0; j < A->n; j++)
	      C->matPA[i][j] = a * A->matPA[i][j] * B->matPA[i][j];

     /* --- add elements for mode 1 --- */
     if (mode > 0)
	for ( i = min; i < max; i++)
	   for ( j = 0; j < A->n; j++)
	      C->matPA[i][j] = 0.0;
     if (count) WARN("%d divisions by 0, result set to 0\n",count);
  }
  
  return C;
}

static int fmatrixMulcoefItf( ClientData cd, int argc, char *argv[])
{
  FMatrix *C = (FMatrix*)cd;
  FMatrix *A = NULL, *B = NULL;
  float    a = 1.0;
  int mode = 0;
  int div  = 0;
  
  argc--;
  if (itfParseArgv( argv[0], &argc, argv+1, 0,
		   "<fmatrixA>", ARGV_CUSTOM, getFMatrix, &A, cd, "",
		   "<fmatrixB>", ARGV_CUSTOM, getFMatrix, &B, cd, "",
		   "-a",         ARGV_FLOAT,  NULL,       &a, cd, "",
	           "-div",       ARGV_INT,    NULL,       &div,cd,
		   "division instead multiplication",
		   "-mode",      ARGV_INT,    NULL,       &mode, cd,
		   "mode 0, 1 or -1 for dimesion(result) =, max or min of input",
		   NULL) != TCL_OK) {
     if (A) fmatrixFree(A);
     if (B) fmatrixFree(B);
     SERROR("Usage: <float> <FMatrix> <float> <FMatrix>.\n");
     return TCL_ERROR;
  }

  C = fmatrixMulcoef( C, a, A, B, mode, div);

  fmatrixFree(A);
  fmatrixFree(B);
  if (C) return TCL_OK;
  return TCL_ERROR;
}


/* ------------------------------------------------------------------------
    Perceptron

    Perform the operations needed for a layer of an MLP on a matrix
   ------------------------------------------------------------------------ */

FMatrix* fmatrixPerceptron (FMatrix* C, FVector* bias, char* type, int l)
{
  int i, j;
  int N = C->n;

  /* Add the bias */
  if (bias) for (i = 0; i < C->m; i++) {
    float* c = C->matPA[i];
    for (j = 0; j < N; j++)
      c[j] += bias->vecA[j];
  }

  /* softmax case */
  if (!strcmp (type, "softmax")) {
    for (i = 0; i < C->m; i++) {
      float max = -FLT_MAX;
      float sum = 0;
      float*  c = C->matPA[i];

      for (j = 0; j < N; j++)
	if (max < c[j]) max = c[j];
      for (j = 0; j < N; j++)
	sum += c[j] = expf(c[j] - max);
      for (j = 0; j < N; j++)
	c[j] = c[j]/ sum;
      if (l) for (j = 0; j < N; j++)
	c[j] = logf (c[j]);
    }
    
    return C;
  }

  /* This is from QN_fltvec.h from ICSI's QuickNet package
     It was tested on little endian machines (Intel)
     QN_EXPQ = expf --- but fast
     (Florian)
  */
#define QN_EXP_A (1048576/M_LN2)
#define QN_EXP_C 60801
#ifdef QN_WORDS_BIGENDIAN
#define QN_EXPQ(y) (qn_d2i.n.j = (int) (QN_EXP_A*(y)) + (1072693248 - QN_EXP_C), (y > -700.0f && y < 700.0f) ? qn_d2i.d : exp(y) )
#else 
#define QN_EXPQ(y) (qn_d2i.n.i = (int) (QN_EXP_A*(y)) + (1072693248 - QN_EXP_C), (y > -700.0f && y < 700.0f) ? qn_d2i.d : exp(y) )
#endif
#define QN_EXPQ_WORKSPACE union { double d; struct { int j,i;} n; } qn_d2i; qn_d2i.d = 0.0;

  {
    double qd[N];
    int* qi = (int*) qd;
    qi++;

  /* default case: sigmoid */
  for (i = 0; i < C->m; i++) {
    float* c = C->matPA[i];
    /* slow(?) default case */
    /* for (j = 0; j < N; j++)
      c[j]  = 1.0/(1.0 + expf(-c[j])); */
    /* QN_EXPQ_WORKSPACE needed */
    /* for (j = 0; j < N; j++)
      c[j]  = 1.0/(1.0 + QN_EXPQ(-c[j])); */
    /* hardcoded speed-up trick */
    for (j = 0; j < N; j++)
      qi[j+j] = (QN_EXP_A*(-c[j])) + (1072693248 - QN_EXP_C);
    for (j = 0; j < N; j++)
      c[j]  = 1.0/(1.0 + qd[j]);
    if (l) for (j = 0; j < N; j++)
      c[j] = logf (c[j]);
  }
  }

  return C;
}

static int fmatrixPerceptronItf( ClientData cd, int argc, char *argv[])
{
  FMatrix *C    = (FMatrix*) cd;
  FVector *bias = NULL;
  char    *type = "sigmoid";
  int         l = 0;
  
  argc--;
  if (itfParseArgv( argv[0], &argc, argv+1, 0,
		    "-bias",         ARGV_CUSTOM, getFVector, &bias, cd, "the bias FVector",
		    "-log",          ARGV_INT,    NULL,       &l,    cd, "return logarithmic output",
		    "-type",         ARGV_STRING, NULL,       &type, cd, "type: softmax or sigmoid",
		    NULL) != TCL_OK) {
    /*    SERROR("Usage: <float> <FMatrix> <float> <FMatrix>.\n");*/
    return TCL_ERROR;
  }

  if (bias && C->n != bias->n) {
    ERROR ("Weights and bias dimensions don't match: %d != %d.\n", C->n, bias->n);
    return TCL_ERROR;
  }

  if (type && strcmp (type, "sigmoid") && strcmp (type, "softmax")) {
    ERROR ("Type '%s' not known.\n", type);
    return TCL_ERROR;
  }

  if (fmatrixPerceptron (C, bias, type, l))
    return TCL_OK;

  return TCL_ERROR;
}


/* ------------------------------------------------------------------------
    Multiply A * B
   ------------------------------------------------------------------------ */
FMatrix* fmatrixMul( FMatrix* C, FMatrix* A, FMatrix* B)
{
  int i,j,k;
#ifndef BLAS
  double sum00, sum01, sum10, sum11;
  float *aP0, *bP0, *aP1, *bP1;
  int bColumnVectorsize = B->n;

#ifdef SSE_OPT
  /* For SSE_OPT the column vectors in the matrix are not necessarily head to head anymore, since the size of a column vector has to be a multiple 
     of something (see code in fmatrixCreate). bColumnVectorsize contains the actual size (number of float memory cells) of a column vector in memory 
     for Matrix B, instead of the theoretical one as given by B->n. Later on in the code we will then have to jump bColumnVectorsize memory cells in 
     order to go from column vector to column vector instead of B->n cells.
   */
  if (B->n > 1) bColumnVectorsize = B->matPA[1] - B->matPA[0];

#endif /* #ifdef SSE_OPT */
#endif /* #ifndef BLAS */

  assert(A && B && C);
  if ( A->n != B->m) {
    SERROR("Can't multiply matrix %d columns with matrix %d rows\n",A->n,B->m);
    return NULL;
  }
  fmatrixResize(C,A->m,B->n);

#ifdef BLAS
  i = (A->m == 1) ? A->n : A->matPA[1] - A->matPA[0];
  j = (B->m == 1) ? B->n : B->matPA[1] - B->matPA[0];
  k = (C->m == 1) ? C->n : C->matPA[1] - C->matPA[0];
  cblas_sgemm (CblasRowMajor, CblasNoTrans, CblasNoTrans, 
	       A->m, B->n, A->n, 1, 
	       A->matPA[0], i, 
	       B->matPA[0], j, 0, 
	       C->matPA[0], k);  
#else
  /* ---------------------------------
      I.      i..(+2)
     --------------------------------- */
  for ( i = 0; i < C->m - 1; i += 2) {
     /* ---------------------------------
	 Ia.      j..(+2)
	--------------------------------- */
     for ( j = 0; j < C->n - 1; j += 2) {
	sum00 = 0.0; sum01 = 0.0; sum10 = 0.0; sum11  = 0.0;
	aP0 = A->matPA[i];   bP0 = B->matPA[0] + j;
	aP1 = A->matPA[i+1]; bP1 = B->matPA[0] + j + 1;
	for ( k = 0; k < A->n; k++, bP0 += bColumnVectorsize, bP1 += bColumnVectorsize) {
	   sum00 += *aP0   * *bP0;
	   sum01 += *aP0++ * *bP1;
	   sum10 += *aP1   * *bP0;
	   sum11 += *aP1++ * *bP1;
	}
	C->matPA[i  ][j  ] = sum00;
	C->matPA[i  ][j+1] = sum01;
	C->matPA[i+1][j  ] = sum10;
	C->matPA[i+1][j+1] = sum11;
     }
     /* ---------------------------------
	 Ib.      j..(+1)
	--------------------------------- */
     for ( ; j < C->n; j ++) {
	sum00 = 0.0; sum10 = 0.0;
	aP0 = A->matPA[i];   bP0 = B->matPA[0] + j;
	aP1 = A->matPA[i+1];
	for ( k = 0; k < A->n; k++, bP0 += bColumnVectorsize) {
	   sum00 += *aP0++ * *bP0;
	   sum10 += *aP1++ * *bP0;
	}
	C->matPA[i  ][j] = sum00;
	C->matPA[i+1][j] = sum10;
     }
  }
  /* ---------------------------------
      II.      i..(+1)
     --------------------------------- */
  for ( ; i < C->m; i++) {
     /* ---------------------------------
	 IIa.      j..(+2)
	--------------------------------- */
     for ( j = 0; j < C->n - 1; j += 2) {
	sum00 = 0.0; sum01 = 0.0;
	aP0 = A->matPA[i];   bP0 = B->matPA[0] + j;
	bP1 = B->matPA[0] + j + 1;
	for ( k = 0; k < A->n; k++, bP0 += bColumnVectorsize, bP1 += bColumnVectorsize) {
	  sum00 += *aP0   * *bP0;
	  sum01 += *aP0++ * *bP1;
	}
	C->matPA[i][j  ] = sum00;
	C->matPA[i][j+1] = sum01;
     }
     /* ---------------------------------
	 IIb.      j..(+1)
	--------------------------------- */
     for ( ; j < C->n; j++) {
	sum00 = 0.0;
	aP0 = A->matPA[i];   bP0 = B->matPA[0] + j;
	for ( k = 0; k < A->n; k++, bP0 += bColumnVectorsize) {
	   sum00 += *aP0++ * *bP0;
	}
	C->matPA[i][j] = sum00;
     }
  }

#endif
  return C;
}

static int fmatrixMulItf( ClientData cd, int argc, char *argv[])
{
  FMatrix *C = (FMatrix*)cd;
  FMatrix *A = NULL, *B = NULL;
  int ret    = TCL_OK;
  
  argc--;
  if (itfParseArgv( argv[0], &argc, argv+1, 0,
    "<matrix>", ARGV_CUSTOM, getFMatrix, &A, cd, "matrix A",
    "<matrix>", ARGV_CUSTOM, getFMatrix, &B, cd, "matrix B",
    NULL) != TCL_OK) ret = TCL_ERROR;
  else 
    if (fmatrixMul( C, A, B) == NULL) ret = TCL_ERROR;
  if (A) fmatrixFree(A);
  if (B) fmatrixFree(B);
  return ret;
}

/* ------------------------------------------------------------------------
    Append
   ------------------------------------------------------------------------ */
FMatrix* fmatrixAppendBelow( FMatrix* C, FMatrix* A, FMatrix* B)
{
  int i,j;
  int offs;
  
  assert(A && B && C);
  if ((A->n!=B->n) && !(((A->n==0)&&(A->m==0)) || ((B->n==0)&&(B->m==0))) )
  {
    SERROR("Matrices are required to have the same #columns \n",A->n,B->m);
    return NULL;
  }
  if (A->n!=0)
    fmatrixResize(C,A->m+B->m,A->n);
  else
    fmatrixResize(C,A->m+B->m,B->n);

  if (C!=A)
    for (i=0; i<A->m; i++)
      for (j=0; j<A->n; j++)
        C->matPA[i][j] = A->matPA[i][j];  

  offs = A->m;
  for (i=0; i<B->m; i++)
    for (j=0; j<B->n; j++)
      C->matPA[i+offs][j] = B->matPA[i][j];  

  return C;
}

int fmatrixAppendItf( ClientData cd, int argc, char *argv[])
{
  FMatrix *C = (FMatrix*)cd;
  FMatrix *A = NULL;
  FMatrix *B = NULL;
  char *where = NULL;
  int ret    = TCL_OK;
  
  argc--;
  if (itfParseArgv( argv[0], &argc, argv+1, 0,
    "<matrix>", ARGV_CUSTOM, getFMatrix, &A, cd, "matrix A",
    "<matrix>", ARGV_CUSTOM, getFMatrix, &B, cd, "matrix B",
    "<where>",   ARGV_STRING, NULL, &where, cd, "in {above, below, left, right}",
    NULL) != TCL_OK) return TCL_ERROR;

  if (strcmp(where,"below")==0)
  {
    if (fmatrixAppendBelow( C, A, B) == NULL) ret = TCL_ERROR;
  }
  else
  {
      SERROR("fmatrixAppendItf: unknown option for <where>");
      ret = TCL_ERROR;
  }
  if (A) fmatrixFree(A);
  if (B) fmatrixFree(B);
  return ret;
}

/* ------------------------------------------------------------------------
    Multiply A * B'
   ------------------------------------------------------------------------ */
FMatrix* fmatrixMulot( FMatrix* C, FMatrix* A, FMatrix* B)
{
  int i,j,k;
#ifndef BLAS
  double sum00, sum01, sum10, sum11;
  float *aP0, *bP0, *aP1, *bP1;
#endif  
  assert(A && B && C);
  if (C == A || C == B) {
     SERROR("matrix multiplication: result matrix must be different!\n");
     return NULL;
  }
  if ( A->n != B->n) {
     SERROR("Matrix columns differ: %d and %d.\n",A->n,B->n);
     return NULL;
  }
  fmatrixResize(C,A->m,B->m);

#ifdef BLAS
  /*
    printf (" (%d,%d) (%d,%d) (%d,%d)\n", A->m, A->n, B->m, B->n, C->m, C->n);

    To do the calculation, we'll use the BLAS function cblas_dgemm. 
    This function actually does more work than we need, calculating C = x*A*B + y*C, 
    but by passing in the appropriate scalar values for x and y we can accomplish our goal. 
    The function takes a large number of parameters:

cblas_dgemm Parameters
Parameter	Type	Meaning
1	(constant)	Storage order of matrix. Use the ML_BLAS_STORAGE macro to pass the correct value.
2-3	(constant)	Whether or not the A and B (respectively) matrices should be transposed 
                        before multiplication. Pass CblasNoTrans to avoid transpose.
4	integer	The number of rows in the A matrix.
5	integer	The number of columns in the B matrix.
6	integer	The number of columns in the A matrix.
7	double	A scalar to multiply by during computation.
8	double *	The data array for the A matrix.
9	integer	The major stride for the A matrix.
10	double *	The data array for the B matrix.
11	integer	The major stride for the B matrix.
12	double	A scalar to multiply C by during the computation.
13	double *	The data array for the C matrix (output).
14	integer	The major stride for the C matrix.
  */

  i = (A->m == 1) ? A->n : A->matPA[1] - A->matPA[0];
  j = (B->m == 1) ? B->n : B->matPA[1] - B->matPA[0];
  k = (C->m == 1) ? C->n : C->matPA[1] - C->matPA[0];
  cblas_sgemm (CblasRowMajor, CblasNoTrans, CblasTrans, 
	       A->m, B->m, A->n, 1, 
	       A->matPA[0], i, 
	       B->matPA[0], j, 0, 
	       C->matPA[0], k);
#else
  /* ---------------------------------
      I.      i..(+2)
     --------------------------------- */
  for ( i = 0; i < C->m - 1; i += 2) {
     /* ---------------------------------
	 Ia.      j..(+2)
	--------------------------------- */
     for ( j = 0; j < C->n - 1; j += 2) {
	sum00 = 0.0; sum01 = 0.0; sum10 = 0.0; sum11  = 0.0;
	aP0 = A->matPA[i];   bP0 = B->matPA[j];
	aP1 = A->matPA[i+1]; bP1 = B->matPA[j+1];
	for ( k = 0; k < A->n; k++) {
	   sum00 += *aP0   * *bP0;
	   sum01 += *aP0++ * *bP1;
	   sum10 += *aP1   * *bP0++;
	   sum11 += *aP1++ * *bP1++;
	}
	C->matPA[i  ][j  ] = sum00;
	C->matPA[i  ][j+1] = sum01;
	C->matPA[i+1][j  ] = sum10;
	C->matPA[i+1][j+1] = sum11;
     }
     /* ---------------------------------
	 Ib.      j..(+1)
	--------------------------------- */
     for ( ; j < C->n; j ++) {
	sum00 = 0.0; sum10 = 0.0;
	aP0 = A->matPA[i];   bP0 = B->matPA[j];
	aP1 = A->matPA[i+1];
	for ( k = 0; k < A->n; k++) {
	   sum00 += *aP0++ * *bP0;
	   sum10 += *aP1++ * *bP0++;
	}
	C->matPA[i  ][j] = sum00;
	C->matPA[i+1][j] = sum10;
     }
  }
  /* ---------------------------------
      II.      i..(+1)
     --------------------------------- */
  for ( ; i < C->m; i++) {
     /* ---------------------------------
	 IIa.      j..(+2)
	--------------------------------- */
     for ( j = 0; j < C->n - 1; j += 2) {
	sum00 = 0.0; sum01 = 0.0;
	aP0 = A->matPA[i];   bP0 = B->matPA[j];
	bP1 = B->matPA[j+1];
	for ( k = 0; k < A->n; k++) {
	   sum00 += *aP0   * *bP0++;
	   sum01 += *aP0++ * *bP1++;
	}
	C->matPA[i][j  ] = sum00;
	C->matPA[i][j+1] = sum01;
     }
     /* ---------------------------------
	 IIb.      j..(+1)
	--------------------------------- */
     for ( ; j < C->n; j++) {
	sum00 = 0.0;
	aP0 = A->matPA[i];   bP0 = B->matPA[j];
	for ( k = 0; k < A->n; k++) {
	   sum00 += *aP0++ * *bP0++;
	}
	C->matPA[i][j] = sum00;
     }
  }
#endif
  return C;
}

int fmatrixMulotItf( ClientData cd, int argc, char *argv[])
{
  FMatrix *C = (FMatrix*)cd;
  FMatrix *A = NULL, *B = NULL;
  
  if ( (argc == 3) &&
       (A = fmatrixGetItf( argv[1])) && (B = fmatrixGetItf( argv[2]))) {
     C = fmatrixMulot( C, A, B);
     fmatrixFree(A);
     fmatrixFree(B);
     if (C) return TCL_OK;
     else   return TCL_ERROR;
  }
  if (A) fmatrixFree(A);
  if (B) fmatrixFree(B);
  ERROR( "Usage: <FMatrix> <FMatrix'>.\n");
  return TCL_ERROR;
}

/* ------------------------------------------------------------------------
    Multiply FMatrix A with transposed FBMatrix B
   ------------------------------------------------------------------------ */

FMatrix* fmatrixBMulot( FMatrix* C, FMatrix* A, FBMatrix* B)
{
  double sum;
  int i,j;
  float *aP, *bP, *eP;
   
  assert(A && B && C);
  if (C == A) {
     SERROR("matrix multiplication: result matrix must be different!\n");
     return NULL;
  }
  if ( A->n < B->n) { /* B->n can be smaller */
     SERROR("Matrix columns differ: %d and %d.\n",A->n,B->n);
     return NULL;
  }
  fmatrixResize(C,A->m,B->m);
  for ( i = 0; i < C->m; i++) {
    for ( j = 0; j < C->n; j++) {
      sum = 0.0;
      aP = A->matPA[i] + B->offset[j];
      eP = A->matPA[i] + B->offset[j] + B->coefN[j] - 3;
      bP = B->matPA[j]; 
      while (aP < eP) {
	sum += aP[0]*bP[0] +  aP[1]*bP[1] + aP[2]*bP[2] + aP[3]*bP[3];
	aP += 4; bP += 4;
      }
      eP += 3;
      while (aP < eP) sum += *(aP++) * *(bP++);
      C->matPA[i][j] = sum;
    }
  }
  return C;
}

int fmatrixBMulotItf( ClientData cd, int argc, char *argv[])
{
  FMatrix  *C = (FMatrix*)cd;
  FMatrix  *A = NULL;
  FBMatrix *B = NULL;
  
  if ( (argc == 3) &&
       (A = fmatrixGetItf( argv[1])) && (B = fbmatrixGetItf( argv[2]))) {
     C = fmatrixBMulot( C, A, B);
     fmatrixFree(A);
     fbmatrixFree(B);
     if (C) return TCL_OK;
     else   return TCL_ERROR;
  }
  if (A) fmatrixFree(A);
  if (B) fbmatrixFree(B);
  ERROR( "Usage: <FMatrix> <FBMatrix'>.\n");
  return TCL_ERROR;
}


/* ------------------------------------------------------------------------
    Minmax from .. to
    to = -1 means to end
    ------------------------------------------------------------------------ */
int fmatrixMinmaxft( FMatrix* A, float *min, float *max, int from, int to)
{
   int  i,j;
   float  coef;

   if ((A==NULL) || (A->m == 0) || (A->n == 0)) {
      SERROR( "FMatrix is empty.\n"); return -1;
   }
   from = (from<0) ? 0 : from;
   to   = (to==-1) ? A->m - 1 : to;
   if ((from>=A->m) || (to<from)) {
      SERROR("empty range.\n");      return 0;
   }
   if (to >= A->m) to = A->m - 1;
   *min = *max = A->matPA[from][0];
   for ( i=from; i<=to; i++)
      for ( j=0; j<A->n; j++) {
	 coef = A->matPA[i][j];
	 if (*max<coef)      *max = coef;
	 else if (*min>coef) *min = coef;
      }
   return (to + 1 - from);
}

/* ------------------------------------------------------------------------
    Minmax
   ------------------------------------------------------------------------ */
int fmatrixMinmax( FMatrix* A, float *min, float *max)
{
   int  i,j;
   float  coef;

   if ((A==NULL) || (A->m == 0) || (A->n == 0)) {
     ERROR("FMatrix is empty.\n");
     return 0;
  }
  
   *min = *max = A->matPA[0][0];
   for ( i=0; i<A->m; i++)
      for ( j=0; j<A->n; j++) {
	 coef = A->matPA[i][j];
	 if (*max<coef)      *max = coef;
	 else if (*min>coef) *min = coef;
      }
   return A->m;
}

int fmatrixMinmaxItf( ClientData cd, int argc, char *argv[])
{
   float min, max;
   FMatrix *A = (FMatrix*)cd;

   if (fmatrixMinmax( A, &min, &max) > 0) {
      itfAppendResult( "% 2.4e  % 2.4e", min, max);
      return TCL_OK;
   }
   ERROR("matrix empty.\n");
   return TCL_ERROR;
}

/* ------------------------------------------------------------------------
    Cosine

    type 0  is the original cosine transform (IDFT for a symmetric power
            spectrum) taking N/2+1 power coefficients from a N-point DFT.
	    (as in NIST's PLP calculation)
    type 1  is used to transform log mel-frequency coefficients.
            (as in mfcc_lib.c by Aki Ohshima aki@speech1.cs.cmu.edu)
   ------------------------------------------------------------------------ */
#ifndef M_PI
#define M_PI    3.14159265358979323846
#endif
static int fmatrixCosine( FMatrix* A, int m, int n, int type)
{
   int  i,j;

   assert(A && m>=0 && n>=0);
   fmatrixResize(A,m,n);

   if (type == 0) {
      for (i=0; i<A->m; i++) {
	 double fac = i * M_PI / (double)(A->n - 1);
	 for (j=1; j<(A->n-1); j++)
	        A->matPA[i][j]        = 2.0 * cos(fac*j);
	 if (n) A->matPA[i][0]        = 1.0;
	 if (n) A->matPA[i][A->n - 1] = cos(i * M_PI);
      }
   }
   else if (type == 1) {
      for (i=0; i<A->m; i++) {
	 double fac = i * M_PI / (double)A->n;
	 for (j=0; j<A->n; j++) A->matPA[i][j] = cos(fac*(j+0.5));
      }
   }
   else {
      ERROR("cosine: type must be 0 or 1 but is %d\n",type);
      return TCL_ERROR;
   }
   
   return TCL_OK;
}

static int fmatrixCosineItf( ClientData cd, int argc, char *argv[])
{
   FMatrix *A = (FMatrix*)cd;
   int m = A->m, n = A->n, type = 0;

   argc--;
   if (itfParseArgv( argv[0], &argc, argv+1, 0,
		    "m",      ARGV_INT, NULL,   &m,      cd, "",
		    "n",      ARGV_INT, NULL,   &n,      cd, "",
		    "-type",  ARGV_INT, NULL,   &type,   cd, "",
		    NULL) != TCL_OK) return TCL_ERROR;
   return fmatrixCosine(A,m,n,type);
}

/* ------------------------------------------------------------------------
    Vector Ranking for Neural Gas
   ------------------------------------------------------------------------ */

typedef struct {

  int   i;                           /* index of the reference vector       */
  float d;                           /* probability of the reference vector */

} VectorRank;

static VectorRank* vecRankA = NULL;
static int         vecRankN = 0;

static VectorRank* vectorRankAlloc( int refN)
{
  if ( vecRankN < refN) {
    vecRankN =  refN;
    vecRankA = (VectorRank*)realloc( vecRankA, vecRankN * sizeof(VectorRank));
  }
  return vecRankA;
}

static int compareUpRank(const void * a, const void *b)
{
  VectorRank* bpa = (VectorRank *)a;
  VectorRank* bpb = (VectorRank *)b;

  if      ( bpa->d < bpb->d) return -1;
  else if ( bpa->d > bpb->d) return  1;
  else                       return  0;
}

/* ------------------------------------------------------------------------- */
/* fmatrixSizeOpt will compute the optimal codebook size for a codebook that */
/*  represents the input matrix.                                             */
/* ------------------------------------------------------------------------- */
static int fmatrixSizeOpt(FMatrix* dataP, int minM, int maxM, float maxVariance) {
  int M;
  double *distortion;
  
  printf("\nCalled with minM=%d, maxM=%d, maxVariance=%f", minM, maxM,maxVariance); 
  printf("\nHave %d vecs with %d dimensions\n", dataP->m, dataP->n);
  fflush(stdout);
  
  distortion = (double*)malloc(sizeof(double) * (maxM+1-minM));
  distortion[0] = dataDistortionM(dataP, minM);
  for (M=minM+1; M <= maxM; M++){
    distortion[M-minM] = dataDistortionM(dataP,M);
    printf("M=%d --> distortion=%f\n", M, distortion[M-minM]);
    if (distortion[M-minM-1] <= distortion[M-minM]) break;
  }
  free(distortion);
  return (M);
}

/* computes the distortion if a codebook of size M would be chosen */
/* fmatrix->m is #of samples, fmatrix->n is dimensionality of vectos */

#define NUMCHUNKS 3
double dataDistortionM(FMatrix *dataP, int M) {
  int trainN, xvalN, trainX, xvalX, dataX, dataN, dimX, dimN, chunk, idx;
  FMatrix *trainM, *xvalM, *cbM, *fmP;
  double distortion = 0.0;
  FVector *distribP = NULL;

  dataN  = dataP->m;
  dimN   = dataP->n;
  xvalN  = floor( (double)dataN / (double)NUMCHUNKS);
  trainN = dataN - xvalN;
  trainM = fmatrixCreate(trainN, dimN);
  xvalM  = fmatrixCreate(xvalN,  dimN);
  cbM    = fmatrixCreate(M, dimN); /* for neural gas */
  /* ------------------------- */
  /* do it in NUMCHUNKS chunks */
  /* ------------------------- */
  for (chunk=0; chunk < NUMCHUNKS; chunk++) {
    xvalX=0; trainX=0;
    for (dataX=0; dataX < dataN; dataX++){
      if ( dataX >= chunk*xvalN && dataX < (chunk+1)*xvalN) { 
	fmP = xvalM; idx=xvalX; xvalX++;
      } else {
	fmP = trainM; idx=trainX; trainX++;
      }
      for (dimX=0; dimX < dimN; dimX++) fmP->matPA[idx][dimX] = dataP->matPA[dataX][dimX];
    }
    fmatrixNeuralGas(cbM, trainM, 10, 0.0, 0.0, distribP, 1, 1);
    distortion += fmatrixDistortion(cbM, distribP, xvalM);
  }
  distortion /= (float)NUMCHUNKS;
  fmatrixFree(trainM); fmatrixFree(xvalM); fmatrixFree(cbM);
  return (distortion);
}

/* ------------------------------------------------------ */
/* computes the distortion of the codebook given the data */
/* 'How good does the codebook represent the data?'       */
/* High values mean bad representation. Zero is the best  */
/* possible value; negative means error                   */
/* ------------------------------------------------------ */
extern double fmatrixDistortion(FMatrix *codebook, FVector *distrib, FMatrix *xvaldata) {
  double distortion = 0.0;
  int xvalX, refX;
  VectorRank* rank = vectorRankAlloc( codebook->m);    /* array of distances to the ref vecs */

  /* method: standard distortion as in kmeans */
  for (xvalX=0; xvalX < xvaldata->m; xvalX++){
    for (refX=0; refX < codebook->m; refX++) { 
      int dimX; float dist=0.0;
      for (dimX=0; dimX < codebook->n; dimX++) {
	dist += (codebook->matPA[refX][dimX] - xvaldata->matPA[xvalX][dimX]) *
	        (codebook->matPA[refX][dimX] - xvaldata->matPA[xvalX][dimX]);
      }
      rank[refX].d = dist; rank[refX].i = refX;
    }
    qsort( rank, codebook->m, sizeof(VectorRank), compareUpRank);
    distortion += rank[0].d;
  }
  distortion = distortion / (double)xvaldata->m;
  distortion += log(codebook->m);
  return(distortion);
}


/* ------------------------------------------------------------------------
    fmatrixNeuralGas
    cmP - target matrix, fmP - source data vector array, N - #of desired 
    iterations, betaS - start temperature, betaF - temperature decay multi-
    plier, fvP - result count vector, init - flag for random initialization
    (init == 0 causes no initialization at all)
   ------------------------------------------------------------------------ */

double fmatrixNeuralGas( FMatrix* cmP, FMatrix* fmP, int N, 
                         float betaS,  float betaF, FVector* fvP, int init, int step)
{
  /* ------------------------------------------------------------------ */
  /* smP is the accumulator matrix, in its last dimension the count for */
  /* each of the vectors is stored                                      */
  /* ------------------------------------------------------------------ */
  FMatrix*    smP  = fmatrixCreate(   cmP->m, cmP->n+1);
  
  VectorRank* rank = vectorRankAlloc( cmP->m);    /* array of distances to the ref vecs */
  double      beta = betaS==0 ? 0 : cmP->m * betaS;   /* temperature */
  int         dimN = cmP->n;
  int         refN = cmP->m, i, j, n, k;
  double      distortion = 0.0;
  
  if ( dimN != fmP->n) {
    ERROR("Dimension of matrix doesn't match %d != %d.\n", dimN, fmP->n);
    return -1.0;
  }

  /* -------------------------------------------------------------- */
  /* initialize matrix to random sample vectors (init==1, default), */
  /* or initialize matrix to the first refN sample vectors (init>1) */
  /* The latter will produce reproducable results (for kmeans)      */
  /* -------------------------------------------------------------- */
  if (init) {
    int n=0;
    srand(12345);
    for ( k = 0; k < refN; k++) {
      if (init==1) {
	n = rand() % fmP->m;
      } else {
	n++; if (n >= fmP->m) n=0;
      }
      for ( j = 0; j < dimN; j++) cmP->matPA[k][j] = fmP->matPA[n][j];
    }
  }

  /* run N iterations of neural gas */

  for ( n = 0; n < N; n++) {
    double scale      = 0.0;
    distortion        = 0.0;
    /* ---------------------------------------  */
    /* clear the reference vector accumulator   */
    /* set the scaling parameter to the sum of  */
    /* all weighting factors in case of neural  */
    /* gas (and not kmeans)                     */
    /* ---------------------------------------  */
    for ( k = 0; k < refN; k++) {
      for ( j = 0; j <= dimN; j++) smP->matPA[k][j] = 0.0;
      scale += beta > 0 ? exp(-1.0*k/beta) : (k==0);
    }

    /* for all data points do... */
    for ( k = 0; k < fmP->m; k += step) {
      /* -------------------------------------------------- */
      /* compute distance of this data item to all existing */
      /* ref vectors in the cmP vector array and store them */
      /* in rank[] for later sorting                        */
      /* -------------------------------------------------- */
      for ( i = 0; i < refN; i++) {
        rank[i].d = 0;
        rank[i].i = i;
        for ( j = 0; j < dimN; j++)
          rank[i].d += (cmP->matPA[i][j] - fmP->matPA[k][j]) * 
                       (cmP->matPA[i][j] - fmP->matPA[k][j]);
      }

      qsort( rank, refN, sizeof(VectorRank), compareUpRank);
      
      distortion += rank[0].d;
      
      /* if the temperature is zero, do kmeans */
      if (beta<=0) 
      {
	/* ----------------------------------------------- */
	/* add this current data vector to the accumulator */
	/* with weight 1                                   */
	/* ----------------------------------------------- */
        smP->matPA[rank[0].i][dimN] += 1.0;
	for ( j = 0; j < dimN; j++)
          smP->matPA[rank[0].i][j] += fmP->matPA[k][j];
      }
      else for ( i = 0; i < refN; i++) 
      {
	/* ----------------------------------------------- */
	/* neuralGas: add current data vector fuzzily to   */
	/* all accumulators with exponentially decaying    */
	/* weights 'w'                                     */
	/* ----------------------------------------------- */
        float w = exp(-1.0*i/beta) / scale;
        int   x = rank[i].i;

        smP->matPA[x][dimN] += w;
        for ( j = 0; j < dimN; j++)
          smP->matPA[x][j] += w * fmP->matPA[k][j];
      }
    } /* for all data items */

    distortion /= (double)fmP->m;

    /* ----------------- */
    /* Update the Matrix */
    /* ----------------- */
    if (fvP) fvectorResize(fvP,refN); /* if a -counts argument is given,
					 allocate a fvector for the counts */
    
    for ( i = 0; i < refN; i++) {
      if (fvP) fvP->vecA[i] = smP->matPA[i][dimN];
      if ( smP->matPA[i][dimN] > 0.0) {
        for ( j = 0; j < dimN; j++)
          cmP->matPA[i][j] = smP->matPA[i][j] / smP->matPA[i][dimN];
      }
    }

    /* ----------------------------------- */
    /* move unused code vector to hotspots */
    /* ----------------------------------- */
    {
      int minIdx = 0, maxIdx = 0;
      float avgCnt= 0.0;
      float minCnt= fmP->m;
      float maxCnt= 0.0;
      
      for ( i = 0; i < refN; i++) {
	avgCnt += smP->matPA[i][dimN];
	if (smP->matPA[i][dimN] > maxCnt) {
	  maxCnt = smP->matPA[i][dimN];
	  maxIdx = i;
	}
	if (smP->matPA[i][dimN] < minCnt) {
	  minCnt = smP->matPA[i][dimN];
	  minIdx = i;
	}
      }
      if (refN>0) avgCnt /=refN;

      if (minCnt > 0.3 * avgCnt) continue; /* nothing to repair*/
      INFO("NeuralGas Correction : (%d,%f) -> (%d,%f)\n",minIdx,minCnt,maxIdx,maxCnt);
      for ( j = 0; j < dimN; j++) {
	float shift =  0.3 * rand() / (float) RAND_MAX;	
	cmP->matPA[minIdx][j] = smP->matPA[maxIdx][j] * (1.0 + shift);
      }
    }

    beta *= betaF;   /* decay temperature */
  }

  /* get cluster attachments */
  for ( k = 0; k < fmP->m; k++) {
    float bestscore = 1e+6;
    //int bestrefX  = -1;
    for ( i = 0; i < refN; i++) {
      float score = 0;
      for ( j = 0; j < dimN; j++) {
	score += (cmP->matPA[i][j] - fmP->matPA[k][j]) * 
	  (cmP->matPA[i][j] - fmP->matPA[k][j]);
      }
      if (score < bestscore) {
	bestscore= score;
	//bestrefX= i;
      }
    }
  }

  if ( smP) fmatrixFree( smP);
  return  distortion;
}

static int  fmatrixNeuralGasItf (ClientData cd, int argc, char *argv[])
{
  FMatrix*  fmP        = NULL;
  FVector*  fvP        = NULL;
  int       iter       = 1;
  float     betaS      = 1.0;
  float     betaF      = 1.0;
  int       ac         = argc-1;
  int       init       = 1;
  int       step       = 1;
  double    distortion = 0.0;
  
  if ( itfParseArgv( argv[0], &ac, argv+1, 0,
      "<matrix>", ARGV_CUSTOM, getFMatrix,&fmP,cd,"matrix of sample vectors", 
      "-maxIter", ARGV_INT,   NULL, &iter,  NULL, "number of iterations",
      "-tempS",   ARGV_FLOAT, NULL, &betaS, NULL, "start temperature (0=k-means)",
      "-tempF",   ARGV_FLOAT, NULL, &betaF, NULL, "temperature multiplyer",
      "-counts",  ARGV_CUSTOM,getFVectorP,&fvP,cd, "vector with counts", 
      "-step",    ARGV_INT,   NULL, &step,  NULL, "only take every Nth sample",
      "-init",    ARGV_INT,   NULL, &init,  NULL, "initialize with random samples",
      NULL) != TCL_OK) {
    if ( fmP) fmatrixFree( fmP);
    return TCL_ERROR;
  }
  distortion = fmatrixNeuralGas((FMatrix*)cd, fmP, iter, betaS, betaF, fvP, init, step);
  if (distortion < 0.0) {
    ERROR ("fmatrixNeuralGas: distortion < 0\n");
    return TCL_ERROR;
  }
  itfAppendResult("% 2.8e",distortion);
  if ( fmP) fmatrixFree( fmP);
  return TCL_OK;
}

static int  fmatrixSizeOptItf (ClientData cd, int argc, char *argv[]){
  int       ac         = argc-1;
  int       minM       = 1;
  int       maxM       = 64;
  float     maxVariance= 0.0;
  int       optN       = -1;

  if ( itfParseArgv( argv[0], &ac, argv+1, 0,
      "-minM",    ARGV_INT,   NULL, &minM,  NULL, "minimal size of output matrix", 
      "-maxM",    ARGV_INT,   NULL, &maxM,  NULL, "maximal size of output matrix", 
      "-variance",ARGV_FLOAT, NULL, &maxVariance, NULL, "maximal variance when clustering", 
      NULL) != TCL_OK) {
    return TCL_ERROR;
  }
  optN = fmatrixSizeOpt((FMatrix*)cd, minM, maxM, maxVariance);
  if (optN < 0.0) return TCL_ERROR;
  itfAppendResult("%d",optN);
  return TCL_OK;
}

/* ------------------------------------------------------------------------
    matrixDisplay
      canvas		tk name of the canvas 
      matrix		matrix to be displayed
      width, height	
      x, y		
      from, to		rows (frames) to be displayed to = -1 means last
      borderwidth	
      space		
      dx, dy		
      min		
      max		
      tag		
      mode              0 blobs, 1 grey, 2 horizontal, 3 vertical		
      outline		
  ------------------------------------------------------------------------ */
int fmatrixDisplay (Tcl_Interp *interp, char *canvas, FMatrix *matrix, 
		    int width, int height, int x, int y, int from, int to,
		    int borderwidth, int space, int dx, int dy,
		    float min, float max, char *tag, int mode,
                    char *outline) 
{
  int canvas_width, canvas_height;
  int xpos, ypos, color, range, i, j;
  int outlinewidth = 0;
  char result [512];

  DString tclCommand;

  /* -------- Original code from Martin Westphal: --------
  if (outline != "")
    outlinewidth = 1;
  else
    space -= 1;
     -----------------------------------------------------
     Problem:   on SUN, comparison was always true,
                but not on Linux
     Solution:  assume, that comparison is true in general
     Advantage: gui-lib/ tcl-lib will work as usual, 
                although Martin's original idea is true ...      
  */
  outlinewidth = 1;

  /* check range */
  to   = ((to == -1)||(to >= matrix->m)) ? matrix->m - 1 : to;
  from = (from < 0) ? 0 : from;
  if ( (range = to + 1 - from) <= 0) {
     SERROR("Given range is empty! Coefficients 0 .. %d available.\n", matrix->n - 1);
     return TCL_ERROR;
  }
  

  /* get width and height of canvas widget */
  canvasConfiguration (interp, canvas, &canvas_width, &canvas_height);

  if (width && dx) {
    WARN ("Both dx and width specified (dx ignored)\n");
    dx = 0;
  }
  if (height && dy) {
    WARN ("Both dy and height specified (dy ignored)\n");
    dy = 0;
  }

  if(dx) width  = range * dx  +  (range - 1) * space;
  else {
     if(!width)	width = canvas_width;
     width -= 2 * borderwidth;
     dx = (width  -  (range - 1) * space) / range;
  }

  if(dy) height = matrix->n * dy  +  (matrix->n - 1) * space;
  else {
     if(!height) height = canvas_height;
     height -= 2 * borderwidth;
     dy = (height  -  (matrix->n - 1) * space) / matrix->n;
  }
  
  /* consider borderwidth */
  x = x + borderwidth;
  y = y + borderwidth;

  sprintf (result, "%d %d %d %d %d %d %d %d\n", x, y, width, height, dx, dy, borderwidth, space);

  if (min > max) fmatrixMinmax( matrix, &min, &max);

  switch (mode) {
   case 1: { /* display matrix with gray values */
      float scale = 1, f;
     
      if (min < max) scale = 65535.0 / (1024 * (max - min));
      
      /* display matrix elements */
      for ( i = 0; i < range; ++i) {
	 for ( j = 0; j < matrix->n; ++j) {
	    f = matrix->matPA[i+from][j];
	    f = ( f < min) ? min : f;
	    f = ( f > max) ? max : f;
	    xpos = x + i * (dx + space);
	    ypos = height - (y + j * (dy + space));
	    color = 65535 - (int)(scale * (f - min)) * 1024;
	    
	    dstringInit(&tclCommand);
	    dstringAppend(&tclCommand,"%s create rect %d %d %d %d -outline \"%s\" -width %d -fill #%04x%04x%04x", 
		     canvas, xpos, ypos, xpos + dx, ypos - dy,
		     outline, outlinewidth, color, color, color);
	    Tcl_Eval (interp, dstringValue(&tclCommand));
	    dstringFree(&tclCommand);
	 }
      }
      break;
   }
   case 3: { /* display rows as vertical functions */
      float scale = 1, f;
      int xold=0, yold=0, zero=0, axis=0;
      
      if (min < max) {
	 scale = dx / (max - min);
	 if (min < 0 && 0 < max) axis = 1;
	 zero  = - scale * min;

      }
      /* display matrix elements */
      for ( i = 0; i < range; ++i) {
	 if (axis) {
	    xpos = x + i * (dx + space) + dx - zero;
	    ypos = height - (y + 0.5 * dy);
	    dstringInit(&tclCommand);
	    dstringAppend(&tclCommand, "%s create line %d %d %d %d -arrow last -fill red", 
		     canvas, xpos, ypos, xpos, ypos - (matrix->n - 1) * (dy + space));
	    Tcl_Eval (interp, dstringValue(&tclCommand));
	    dstringFree(&tclCommand);
	 }
	 if (outlinewidth) {
	    xpos = x + i * (dx + space);
	    ypos = height - (y + 0.5 * dy);
	    dstringInit(&tclCommand);
	    dstringAppend(&tclCommand, "%s create line %d %d %d %d -fill blue", 
		     canvas, xpos, ypos, xpos, ypos - (matrix->n - 1) * (dy + space));
	    Tcl_Eval (interp, dstringValue(&tclCommand));
	    dstringFree(&tclCommand);
	    xpos += dx;
	    dstringInit(&tclCommand);
	    dstringAppend(&tclCommand, "%s create line %d %d %d %d -fill blue", 
		     canvas, xpos, ypos, xpos, ypos - (matrix->n - 1) * (dy + space));
	    Tcl_Eval (interp, dstringValue(&tclCommand));
	    dstringFree(&tclCommand);
	 }
	 for ( j = 0; j < matrix->n; ++j) {
	    f = matrix->matPA[i+from][j];
	    f = ( f < min) ? min : f;
	    f = ( f > max) ? max : f;
	    xpos = x + i * (dx + space) + dx - scale * (f-min);
	    ypos = height - (y + j * (dy + space) + 0.5 * dy);
	    
	    if (j) {
	      dstringInit(&tclCommand);
	       dstringAppend(&tclCommand, "%s create line %d %d %d %d ", 
			canvas, xold, yold, xpos, ypos);
	       Tcl_Eval (interp, dstringValue(&tclCommand));
	       dstringFree(&tclCommand);
	    }
	    xold = xpos;
	    yold = ypos;
	 }
      }
      break;
   }
   case 2: { /* display columns as horizontal functions */
      float scale = 1, f;
      int xold=0, yold=0, zero=0, axis=0;
      
      if (min < max) {
	 scale = dy / (max - min);
	 if (min < 0 && 0 < max) axis = 1;
	 zero  = - scale * min;
      }
      
      /* display matrix elements */
      for ( j = 0; j < matrix->n; ++j) {
	 if (axis) {
	    xpos = x + 0.5 * dx;
	    ypos = height - (y + j * (dy + space) + zero);
	    dstringInit(&tclCommand);
	    dstringAppend(&tclCommand, "%s create line %d %d %d %d -arrow last -fill red", 
		     canvas, xpos, ypos, xpos + (range-1) * (dx + space), ypos);
	    Tcl_Eval (interp, dstringValue(&tclCommand));
	    dstringFree(&tclCommand);
	 }
	 if (outlinewidth) {
	    xpos = x + 0.5 * dx;
	    ypos = height - (y + j * (dy + space));
	    dstringInit(&tclCommand);
	    dstringAppend(&tclCommand, "%s create line %d %d %d %d -fill blue", 
		     canvas, xpos, ypos, xpos + (range-1) * (dx + space), ypos);
	    Tcl_Eval (interp, dstringValue(&tclCommand));
	    dstringFree(&tclCommand);
	    ypos -= dy;
	    dstringInit(&tclCommand);
	    dstringAppend(&tclCommand, "%s create line %d %d %d %d -fill blue", 
		     canvas, xpos, ypos, xpos + (range-1) * (dx + space), ypos);
	    Tcl_Eval (interp, dstringValue(&tclCommand));
	    dstringFree(&tclCommand);
	 }
	 for ( i = 0; i < range; ++i) {
	    f = matrix->matPA[i+from][j];
	    f = ( f < min) ? min : f;
	    f = ( f > max) ? max : f;
	    xpos = x + i * (dx + space) + 0.5 * dx;
	    ypos = height - (y + j * (dy + space) +  scale * (f-min));
	    
	    if (i) {
	      dstringInit(&tclCommand);
	      dstringAppend(&tclCommand, "%s create line %d %d %d %d ", 
			canvas, xold, yold, xpos, ypos);
	      Tcl_Eval (interp, dstringValue(&tclCommand));
	      dstringFree(&tclCommand);
	    }
	    xold = xpos;
	    yold = ypos;
	 }
      }
      break;
   }
   case 0: { /* display matrix with different blob sizes */
      int xsize, ysize;
      max = MAX (ABS(min),ABS(max));
      for ( i = 0; i < range; ++i) {
	 for ( j = 0; j < matrix->n; ++j) {
	    xsize = (int)( ABS (matrix->matPA[i+from][j]) / max * (float)dx);
	    ysize = (int)( ABS (matrix->matPA[i+from][j]) / max * (float)dy);
	    xpos = x + i * (dx + space) + (dx - xsize) / 2;
	    ypos = height - (y + j * (dy + space) + (dy - ysize) / 2);
	    
	    if (matrix->matPA[i+from][j] > 0) {
	      dstringInit(&tclCommand);
	      dstringAppend(&tclCommand, "%s create rect %d %d %d %d -outline \"%s\" -width %d -fill black -tag %s", 
			canvas, xpos, ypos, xpos + xsize - 1, ypos - ysize + 1,
			outline, outlinewidth, tag);
	      Tcl_Eval (interp, dstringValue(&tclCommand));
	      dstringFree(&tclCommand);
	    }
	    else if (matrix->matPA[i+from][j] < 0) {
	      dstringInit(&tclCommand);
	      dstringAppend(&tclCommand, "%s create rect %d %d %d %d -outline \"%s\" -width %d -fill white -tag %s", 
			canvas, xpos, ypos, xpos + xsize - 1, ypos - ysize + 1,
			outline, outlinewidth, tag);
	      Tcl_Eval (interp, dstringValue(&tclCommand));
	      dstringFree(&tclCommand);
	    }
	 }
      }
      break;
   }
   default:   SERROR("unknown display mode %s.\n",mode);
     return TCL_ERROR; 
  }
  Tcl_SetResult (interp, result, TCL_VOLATILE);
  return TCL_OK;
}


int fmatrixDisplayItf( ClientData cd, int argc, char *argv[])
{
   FMatrix *matrix = (FMatrix*)cd;
   char *canvas = NULL;
   int width = 0, height = 0, borderwidth = 0, dx = 0, dy = 0, x = 0, y = 0,
       from = 0, to = -1, mode = 0;
   int space = 1;
   float min = 0.0, max = -1.0;
   char *tag = "mytag";
   char *outline = "";

   argc--;
   if (itfParseArgv( argv[0], &argc, argv+1, 0,
		    "canvas",      ARGV_STRING, NULL,   &canvas,      cd, "",
		    "-width",      ARGV_INT,    NULL,   &width,       cd, "",
		    "-height",     ARGV_INT,    NULL,   &height,      cd, "",
		    "-borderwidth",ARGV_INT,    NULL,   &borderwidth, cd, "",
		    "-dx",         ARGV_INT,    NULL,   &dx,          cd, "",
		    "-dy",         ARGV_INT,    NULL,   &dy,          cd, "",
		    "-space",      ARGV_INT,    NULL,   &space,       cd, "",
		    "-x",          ARGV_INT,    NULL,   &x,           cd, "",
		    "-y",          ARGV_INT,    NULL,   &y,           cd, "",
		    "-from",       ARGV_INT,    NULL,   &from,        cd, "",
		    "-to",         ARGV_INT,    NULL,   &to,          cd, "",
		    "-mode",       ARGV_INT,    NULL,   &mode,        cd, "",
		    "-grey",       ARGV_INT,    NULL,   &mode,        cd, "",
		    "-min",        ARGV_FLOAT,  NULL,   &min,         cd, "",
		    "-max",        ARGV_FLOAT,  NULL,   &max,         cd, "",
		    "-tag",        ARGV_STRING, NULL,   &tag,         cd, "",
		    "-outline",    ARGV_STRING, NULL,   &outline,     cd, "",
		    NULL) != TCL_OK) {
      ERROR( "Usage:%s <canvas> [-option arg].\n", *argv);
      return TCL_ERROR;
  }
   
  return fmatrixDisplay (itf, canvas, matrix, width, height, x, y, from, to,
			 borderwidth, space, dx, dy, min, max, tag, mode,
			 outline);
}

/* ------------------------------------------------------------------------
    scatterPlot
   ------------------------------------------------------------------------ */
static int fmatrixScatterPlot (Tcl_Interp* interp, char* canvas, FMatrix* matrix,
		 int width, int height, int x, int y,
		 int xindex, int yindex, int from, int to,
		 float xmin, float xmax, float ymin, float ymax,
		 char* tag, int line, int p)
{
  int canvas_width, canvas_height;
  int i;
  int xpos=0, ypos=0, xold=0, yold=0;
  float xfact, yfact;
  DString tclCommand;
  
  /* check ranges */
  if (xindex<0 || yindex<0 || xindex>=matrix->n || yindex>=matrix->n) {
    SERROR("indices %d %d out of range 0..%d\n",xindex, yindex, matrix->n);
    return TCL_ERROR;
  }
  to   = ((to == -1)||(to >= matrix->m)) ? matrix->m - 1 : to;
  from = (from < 0) ? 0 : from;
  if ( to < from) {
     SERROR("Given range is empty! Coefficients 0 .. %d available.\n",
	    matrix->n - 1); return TCL_ERROR;
  }
  
  /* get width and height of canvas widget */
  if (!width || !height) {
    canvasConfiguration (interp, canvas, &canvas_width, &canvas_height);
    if (canvas_width <= 0 || canvas_height <= 0) {
      ERROR("unusual canvas configuration %d x %d\n",canvas_width, canvas_height);
      return TCL_ERROR;
    }
  }
  if (width<=0) width = canvas_width - x;
  if (height<=0) height = canvas_height - y;

  /* min and max */
  if (xmin >= xmax) for (i=0; i<matrix->m; i++) {
    float v = matrix->matPA[i][xindex];
    if (i) {
      if (v>xmax) xmax = v;
      else if (v<xmin) xmin = v;
    }
    else xmax = xmin = v;
  }
  if (ymin >= ymax) for (i=0; i<matrix->m; i++) {
    float v = matrix->matPA[i][yindex];
    if (i) {
      if (v>ymax) ymax = v;
      else if (v<ymin) ymin = v;
    }
    else ymax = ymin = v;
  }
  if (xmin >= xmax || ymin >= ymax) {
    SERROR("min..max range of coefficients not ok\n");
    return TCL_ERROR;
  }

  xfact = width / (xmax-xmin); 
  yfact = height / (ymax-ymin);
  y += height;
  for (i=from; i<=to; i++) {
    xpos = ROUND(xfact * (matrix->matPA[i][xindex]-xmin)) + x;
    ypos = y - ROUND(yfact * (matrix->matPA[i][yindex]-ymin));
    dstringInit(&tclCommand);
    dstringAppend(&tclCommand, "%s create rect %d %d %d %d -tag %s -fill black", 
	     canvas, xpos - p, ypos - p, xpos + p, ypos + p, tag);
    Tcl_Eval (interp, dstringValue(&tclCommand));
    dstringFree(&tclCommand);
    if (line) {
      if (i==from) {xold = xpos; yold = ypos;}
      else {
	dstringInit(&tclCommand);
	dstringAppend(&tclCommand, "%s create line %d %d %d %d -tag %s -fill black", 
		 canvas, xold, yold, xpos, ypos, tag);
	Tcl_Eval (interp, dstringValue(&tclCommand));
	dstringFree(&tclCommand);
	xold = xpos; yold = ypos;
      }
    }
  }
  return TCL_OK;
}

static int fmatrixScatterPlotItf( ClientData cd, int argc, char *argv[])
{
   FMatrix *matrix = (FMatrix*)cd;
   char *canvas = NULL;
   int width = 0, height = 0, x = 0, y = 0, xindex = 0, yindex = 1,
       from = 0, to = -1, line = 0;
   float xmin = 0.0, xmax = 0;
   float ymin = 0.0, ymax = 0;
   int   p = 1;
   char *tag = "mytag";

   argc--;
   if (itfParseArgv( argv[0], &argc, argv+1, 0,
	"canvas",   ARGV_STRING, NULL,   &canvas,      cd, "",
	"-width",   ARGV_INT,    NULL,   &width,       cd, "",
	"-height",  ARGV_INT,    NULL,   &height,      cd, "",
	"-x",       ARGV_INT,    NULL,   &x,           cd, "left side",
	"-y",       ARGV_INT,    NULL,   &y,           cd, "upper side",
	"-xindex",  ARGV_INT,    NULL,   &xindex,      cd, "",
	"-yindex",  ARGV_INT,    NULL,   &yindex,      cd, "",
	"-from",    ARGV_INT,    NULL,   &from,        cd, "",
	"-to",      ARGV_INT,    NULL,   &to,          cd, "",
	"-xmin",    ARGV_FLOAT,  NULL,   &xmin,        cd, "",
	"-xmax",    ARGV_FLOAT,  NULL,   &xmax,        cd, "",
	"-ymin",    ARGV_FLOAT,  NULL,   &ymin,        cd, "",
	"-ymax",    ARGV_FLOAT,  NULL,   &ymax,        cd, "",
	"-tag",     ARGV_STRING, NULL,   &tag,         cd, "",
	"-line",    ARGV_INT,    NULL,   &line,        cd, "draw lines",
	"-p",       ARGV_INT,    NULL,   &p,           cd, "point size",
	NULL) != TCL_OK) {
      ERROR( "Usage:%s <canvas> [-option arg].\n", *argv);
      return TCL_ERROR;
  }
   
  return fmatrixScatterPlot (itf, canvas, matrix, width, height, x, y,
			     xindex, yindex, from, to,
			     xmin, xmax, ymin, ymax, tag, line, p);
}

/* ========================================================================
    Float Band Matrix
   ======================================================================== */
/* ------------------------------------------------------------------------
    Create
   ------------------------------------------------------------------------ */

FBMatrix* fbmatrixCreate( int m, int n)
{
  FBMatrix* mat = (FBMatrix*)malloc(sizeof(FBMatrix));

  if (! mat) {
    SERROR("could not allocate float band matrix");
    return NULL;
  }

  mat->matPA = NULL;
  mat->m = m;
  mat->n = n;
  mat->offset= NULL;
  mat->coefN = NULL;
  if (!m) return mat;
  mat->matPA  = (float**)malloc( m * sizeof(float*));
  mat->offset = (int*)malloc( m * sizeof(int));
  mat->coefN  = (int*)malloc( m * sizeof(int));
  if (mat->matPA && mat->offset && mat->coefN) {
    int i;
    for ( i = 0; i < m; i++) {
      mat->matPA[i]  = NULL;
      mat->offset[i] = 0;
      mat->coefN[i]  = 0;
    }
    return mat;
  }
  else {
    if (mat->matPA)  free(mat->matPA);
    if (mat->offset) free(mat->offset);
    if (mat->coefN)  free(mat->coefN);
    free( mat);
    SERROR( "allocation failure creating float band matrix of size %d x %d.\n",
	    m, n);
    return NULL;
  }
}

static ClientData fbmatrixCreateItf( ClientData cd, int argc, char *argv[])
{
  if (argc == 1)  return (ClientData)fbmatrixCreate(0,0);
  else  if (argc == 2) {
     FBMatrix *mat = NULL;
     argc--;
     if (itfParseArgv( argv[0], &argc, argv+1, 0,
	"matrix", ARGV_CUSTOM,  getFBMatrix, &mat, cd,
		      "@filename or name or definition",
	NULL) != TCL_OK) return NULL;
     return (ClientData)mat;
  }
  else if (argc == 3) {
    int m=0, n=0;
    argc--;
    if (itfParseArgv( argv[0], &argc, argv+1, 0,
       "<m>", ARGV_INT,  NULL, &m, cd, "rows",
       "<n>", ARGV_INT,  NULL, &n, cd, "columns",
       NULL) != TCL_OK) return NULL;
    return (ClientData)fbmatrixCreate(m,n);
  }
  ERROR("give matrix or <m> <n>.\n");
  return NULL;
}

/* ------------------------------------------------------------------------
    Free
   ------------------------------------------------------------------------ */

void fbmatrixFree( FBMatrix* mat)
{
  if (mat) {
    int i;
    if ( mat->matPA) {
      for (i=0;i<mat->m;i++)
	if (mat->matPA[i]) free(mat->matPA[i]); 
      free( mat->matPA);
    }
    if ( mat->offset) free(mat->offset);
    if ( mat->coefN) free(mat->coefN);
    free(mat);
  }
}

static int fbmatrixFreeItf( ClientData cd )
{
  fbmatrixFree((FBMatrix*)cd);
  return TCL_OK;
}

/* ------------------------------------------------------------------------
    fbmatrixGetItf
   ------------------------------------------------------------------------ */

FBMatrix* fbmatrixGetItf( char* value)
{
  FBMatrix* mat;

  if ((mat = (FBMatrix*)itfGetObject( value, "FBMatrix")) == NULL) {
    int     argc;
    char**  argv;

    MSGCLEAR(NULL);

    /* --------------------------------------------------------
       how many list elements
       -------------------------------------------------------- */
    if ( Tcl_SplitList( itf, value, &argc, &argv) == TCL_OK) {
      int i, j;

      for ( i = 0; i < argc; i++) {
        int    tokc, maxN;
        char** tokv;

	/* --------------------------------------------------------
	   scan matrix line: <offset> <coef(0)> .. <coef(tokc-2)>
	   -------------------------------------------------------- */
        if ( Tcl_SplitList( itf, argv[i], &tokc, &tokv) == TCL_OK) {
          if ( i == 0 &&  ! (mat = fbmatrixCreate( argc, tokc))) {
	     Tcl_Free((char*)argv);  Tcl_Free((char*)tokv);  return NULL;
	  }
	  /* --- scan <offset> --- */
	  if ( tokc < 1 ||
	      sscanf( tokv[0], "%d", &mat->offset[i]) != 1) {
	    ERROR("expected integer value for <offset> not: %s\n",tokv[0]);
	    Tcl_Free((char*)tokv); Tcl_Free((char*)argv);
	    fbmatrixFree( mat);	     return NULL;
	  }

	  /* --- How many coefficients? Allocate memory! --- */
	  mat->coefN[i] = tokc - 1;
	  maxN          = mat->offset[i] + mat->coefN[i];
          if (mat->n < maxN) mat->n = maxN; 
	  if (!(mat->matPA[i] = (float*)malloc(mat->coefN[i] * sizeof(float)))){
	    ERROR("allocation failure for fbmatrix.\n");
	    Tcl_Free((char*)tokv); Tcl_Free((char*)argv);
	    fbmatrixFree( mat);	    return NULL;
	  }

	  /* --- scan <coef0> <coef1> .. --- */
	  for ( j = 0; j < mat->coefN[i]; j++) {
            float d;
            if ( sscanf( tokv[j+1], "%f", &d) != 1) {
	       ERROR("expected 'float' type elements.\n");
	       Tcl_Free((char*)tokv); Tcl_Free((char*)argv);
	       fbmatrixFree( mat);       return NULL;
            }
            mat->matPA[i][j] = d;
          }
          Tcl_Free((char*)tokv);
        }
      }
      Tcl_Free((char*)argv);
    }
    return mat;
  }
  else {
    FBMatrix* nmat = fbmatrixCreate( mat->m, mat->n);
    if (! nmat) return NULL;
    return fbmatrixCopy( nmat, mat);
  }
}

/* --- to be used with 'itfParseArgv()' --- */
int getFBMatrix( ClientData cd, char* key, ClientData result,
		 int *argcP, char *argv[])
{
   FBMatrix *mat;

   if ( (*argcP<1) || ((mat = fbmatrixGetItf( argv[0]))  ==  NULL)) return -1;
   *((FBMatrix**)result) = mat;
   (*argcP)--;   return 0;
}

/* =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
    Float Band Matrix methods
   =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- */
/* ------------------------------------------------------------------------
    fbmatrixPutsItf
   ------------------------------------------------------------------------ */

static int fbmatrixPutsItf( ClientData cd, int argc, char *argv[])
{
  FBMatrix *mat = (FBMatrix*)cd;
  int      i,j;

  for ( i = 0; i < mat->m; i++) {
    itfAppendResult( "{ ");
    itfAppendResult( "%d ", mat->offset[i]);
    for ( j = 0; j < mat->coefN[i]; j++)
      itfAppendResult( "% 2.6e ", mat->matPA[i][j]);
    itfAppendResult( "}\n");    
  }
  return TCL_OK;
}

/* ------------------------------------------------------------------------
    Copy
   ------------------------------------------------------------------------ */

FBMatrix* fbmatrixCopy( FBMatrix* A, FBMatrix* B)
{
  int i, j;
  assert(A && B);

  /* free pointers in matrix A */
  if ( A->matPA) {
    for (i=0;i<A->m;i++)
      if (A->matPA[i]) free(A->matPA[i]); 
    free( A->matPA);
  }
  if ( A->offset) free(A->offset);
  if ( A->coefN) free(A->coefN);

  /* init new values */
  A->matPA = NULL;
  A->m = B->m;
  A->n = B->n;
  A->offset= NULL;
  A->coefN = NULL;
  if (!(A->m)) return A;

  /* allocate new pointers */
  A->matPA  = (float**)malloc( A->m * sizeof(float*));
  A->offset = (int*)malloc( A->m * sizeof(int));
  A->coefN  = (int*)malloc( A->m * sizeof(int));
  if (A->matPA && A->offset && A->coefN) {
    for ( i = 0; i < A->m; i++) {
      A->offset[i] = B->offset[i];
      A->coefN[i]  = B->coefN[i];
      A->matPA[i]  = (float*)malloc( A->coefN[i] * sizeof(float));
      if (!A->matPA[i]) break;
      for ( j=0; j<A->coefN[i]; j++) A->matPA[i][j] = B->matPA[i][j]; 
    }
    if (i == A->m) return A;
  }
  if (A->matPA)  free(A->matPA);
  if (A->offset) free(A->offset);
  if (A->coefN)  free(A->coefN);
  free( A);
  SERROR("reallocation failure float band matrix.\n");
  return NULL;
}

int fbmatrixCopyItf( ClientData cd, int argc, char *argv[])
{
  FBMatrix* mat = (FBMatrix*)cd;
  FBMatrix* tmp;

  if ( argc != 2) {
    ERROR("'copy' expects 1 parameter.\n");
    return TCL_ERROR;
  }
  if ((tmp = fbmatrixGetItf(argv[1]))) {
    fbmatrixCopy( mat, tmp);
    fbmatrixFree( tmp);
    return TCL_OK;
  }
  return TCL_ERROR;
}

/* ------------------------------------------------------------------------
    fbmatrixDisplay
      canvas		tk name of the canvas 
      matrix		fbmatrix to be displayed
      width, height	
      x, y		
      min, max		
      tag		
  ------------------------------------------------------------------------ */
static int fbmatrixDisplay (Tcl_Interp *interp, char *canvas, FBMatrix *A, 
                     int width, int height, int x, int y,
                     float min, float max, char *tag)
{
  int xpos=0, ypos=0, xold=0, yold=0, i, j;
  float dx;
  float scale;
  char tmp[512];

  assert(A);
  dx    = width  / (float)A->n;
  scale = height / (max-min);
  y    += height;

  if (min<=0 && max>=0) {
     ypos = y - scale * (-min);
     sprintf (tmp, "%s create line %d %d %d %d -arrow last -fill red -tag %s", 
	      canvas, x, ypos, x+width, ypos, tag);
     Tcl_Eval (interp, tmp);
  }
  /* ------------------
     loop over filter
     ------------------ */
  if (A->coefN) for (i=0; i<A->m; i++) {
     if (A->coefN[i]) for (j=0; j<A->coefN[i]; j++) {
	xpos = x + dx * (0.5 + A->offset[i] + j);
	ypos = y - scale * (A->matPA[i][j]-min);
	if (j==0) {
	   xold = xpos;   yold = ypos;
	}
	sprintf (tmp, "%s create line %d %d %d %d -fill black -tag %s", 
		 canvas, xold, yold, xpos, ypos, tag);
	Tcl_Eval (interp, tmp);
	xold = xpos;   yold = ypos;
     }
  }
  return TCL_OK;
}

static int fbmatrixDisplayItf( ClientData cd, int argc, char *argv[])
{
   FBMatrix *matrix = (FBMatrix*)cd;
   char *canvas = NULL;
   int width = 0, height = 0, x = 0, y = 0;
   float min = 0.0, max = 1.0;
   char *tag = "mytag";

   argc--;
   if (itfParseArgv( argv[0], &argc, argv+1, 0,
		    "canvas",      ARGV_STRING, NULL,   &canvas,      cd, "",
		    "-width",      ARGV_INT,    NULL,   &width,       cd, "",
		    "-height",     ARGV_INT,    NULL,   &height,      cd, "",
		    "-x",          ARGV_INT,    NULL,   &x,           cd, "",
		    "-y",          ARGV_INT,    NULL,   &y,           cd, "",
		    "-min",        ARGV_FLOAT,  NULL,   &min,         cd, "",
		    "-max",        ARGV_FLOAT,  NULL,   &max,         cd, "",
		    "-tag",        ARGV_STRING, NULL,   &tag,         cd, "",
		    NULL) != TCL_OK) {
      ERROR( "Usage:%s <canvas> [-option arg].\n", *argv);
      return TCL_ERROR;
  }
   
  return fbmatrixDisplay (itf, canvas, matrix, width, height, x, y,
			 min, max, tag);
}


/* ------------------------------------------------------------------------
    Bayesian Information Criterion
   ------------------------------------------------------------------------ */

static float fmatrixBIC(FMatrix* sample,int clusterN,float lambda,int iterMax, float eps) 
{
  int       sampleN  = sample->m;
  int       dimN     = sample->n;  
  FMatrix  *mean     = fmatrixCreate(clusterN,dimN);  
  DMatrix **covar    = NULL;  

  int     *clusterIdx = NULL;
  int     *clusterCnt = NULL;

  float bic = 0.0;
  float distAlt = 1E10, distNeu = 0.0;
  int dimX,dimY,sampleX,clusterX,iter;

  clusterIdx=(int*) malloc(sampleN*sizeof(int));
  if (clusterIdx == NULL) {
    ERROR("Allocation of cluster Index Array failed. sampleN= %d\n",sampleN);
    return 1.0;
  }

  clusterCnt=(int*) malloc(clusterN*sizeof(int));
  if (clusterCnt == NULL) {
    ERROR("Allocation of cluster count Array failed. clusterN= %d\n",clusterN);
    return 1.0;
  }

  covar=(DMatrix**) malloc(clusterN*sizeof(DMatrix*));
  if (covar == NULL) {
    ERROR("Allocation of covar matrices failed. clusterN= %d\n",clusterN);
    return 1.0;
  }
  for (clusterX=0;clusterX<clusterN;clusterX++) {
    covar[clusterX]=dmatrixCreate(dimN,dimN);
  }

  /*init cluster means*/
  srand(12345);
  for (clusterX=0;clusterX<clusterN;clusterX++) {
    sampleX=rand() % sampleN;
    for (dimX=0;dimX<dimN;dimX++) {
      mean->matPA[clusterX][dimX]=sample->matPA[sampleX][dimX];
    }
  }

  /*do k-means*/
  for (iter=0;iter<iterMax+1;iter++) {
    distNeu = 0.0;

    for (clusterX=0;clusterX<clusterN;clusterX++) clusterCnt[clusterX]=0;

    /*assign samples*/
    for (sampleX=0;sampleX<sampleN;sampleX++) {
      float bestDistance = 1e20;
      int   bestCluster  = -1;
      for (clusterX=0;clusterX<clusterN;clusterX++) {
	float distance = 0.0;
	for (dimX=0;dimX<dimN;dimX++) {
	  float d = sample->matPA[sampleX][dimX] - mean->matPA[clusterX][dimX];
	  distance += d*d;
	  if (distance > bestDistance) break;
	}
	if (distance < bestDistance) {
	  bestDistance = distance;
	  bestCluster  = clusterX;
	}
      }
      clusterCnt[bestCluster]++;
      clusterIdx[sampleX]=bestCluster;
      distNeu+=bestDistance;

    }
    distNeu/=sampleN;

    if (distAlt - distNeu  <  eps) break;
    distAlt=distNeu;

    if (iter == iterMax) break;

    /*clear cluster*/
    for (clusterX=0;clusterX<clusterN;clusterX++) {
      for (dimX=0;dimX<dimN;dimX++) {
	mean->matPA[clusterX][dimX]=0.0;
      }
    }

    /*accumulate samples*/
    for (sampleX=0;sampleX<sampleN;sampleX++) {
      clusterX=clusterIdx[sampleX];
      for (dimX=0;dimX<dimN;dimX++) {
	mean->matPA[clusterX][dimX]+=sample->matPA[sampleX][dimX];
      }
    }

    /*normalize cluster*/
    for (clusterX=0;clusterX<clusterN;clusterX++) {
      if (clusterCnt[clusterX] == 0) continue;
      for (dimX=0;dimX<dimN;dimX++) {
	mean->matPA[clusterX][dimX]/=clusterCnt[clusterX];
      }
    }

  } /* end kmeans */
  INFO("did %d iterations and ended with %4.4f distortion\n",iter,distNeu);

  /*compute sample covariances*/
  for (sampleX=0;sampleX<sampleN;sampleX++) {
    clusterX=clusterIdx[sampleX];
    for (dimX=0;dimX<dimN;dimX++) {
      double d1 = sample->matPA[sampleX][dimX] - mean->matPA[clusterX][dimX];
      for (dimY=0;dimY<dimN;dimY++) {
	double d2 = sample->matPA[sampleX][dimY] - mean->matPA[clusterX][dimY];
	covar[clusterX]->matPA[dimX][dimY] += d1 * d2;
      }
    }
  }
  for (clusterX=0;clusterX<clusterN;clusterX++) {
    if (clusterCnt[clusterX] == 0) continue;
    for (dimX=0;dimX<dimN;dimX++) {
      for (dimY=0;dimY<dimN;dimY++) {
	covar[clusterX]->matPA[dimX][dimY] /= clusterCnt[clusterX];
      }
    }
  }

  /*compute bic*/
  /* number of free parameter, weighted with lambda*/
  bic = - lambda * clusterN * (dimN + 0.5 * dimN * dimN) * log(sampleN);

  /* likelihood*/
  bic -= 0.5 * sampleN * ( 1 + dimN * log(2* M_PI)); 
  for (clusterX=0;clusterX<clusterN;clusterX++) {
    double det=0.0, sign=0.0;
    if (clusterCnt[clusterX] < 2) continue;
    det = dmatrixLogDet(covar[clusterX],&sign);
    bic -= 0.5 * clusterCnt[clusterX] * det * sign; 
  }
 

  /*clean up*/
  if (clusterIdx) free(clusterIdx);
  if (clusterCnt) free(clusterCnt);
  fmatrixFree(mean);
  if (covar) {
    for (clusterX=0;clusterX<clusterN;clusterX++) dmatrixFree(covar[clusterX]);
    free(covar);
  }

  return bic;
}

static int  fmatrixBICItf (ClientData cd, int argc, char *argv[]){
  int       ac         = argc-1;
  int       clusterN   = 10;
  int       iterMax    = 20;
  float     eps        = 1E-6;
  float     bic        = 1.0;
  float     lambda     = 1.0;

  if ( itfParseArgv( argv[0], &ac, argv+1, 0,
      "clusterN",  ARGV_INT, NULL, &clusterN, NULL, "number of cluster", 
      "-lambda", ARGV_FLOAT, NULL, &lambda,   NULL, "penalty term",
      "-iter",     ARGV_INT, NULL, &iterMax,  NULL, "maximal iteration for kmeans", 
      "-eps",    ARGV_FLOAT, NULL, &eps,      NULL, "minimal distortion", 
      NULL) != TCL_OK) {
    return TCL_ERROR;
  }
  bic = fmatrixBIC((FMatrix*)cd, clusterN, lambda, iterMax, eps);
  itfAppendResult("%4.4f",bic);
  return TCL_OK;
}

/* ========================================================================
    Double Vector
   ======================================================================== */
/* ------------------------------------------------------------------------
    Create
   ------------------------------------------------------------------------ */
DVector* dvectorCreate( int n)
{
  DVector* vec = (DVector*)malloc(sizeof(DVector));

  /* --- allocate structure --- */
  if (! vec) {
    SERROR("allocation failure 1 double vector of size %d.", n);
    return NULL;
  }
  vec->n    = n;
  vec->vecA = NULL;
  vec->count= 0;
  if (!n) return vec;

  /* --- allocate data, init with 0 --- */
  if ((vec->vecA = (double*)malloc(n * sizeof(double)))) {
     int i;
     
     for ( i = 0; i < n; i++) vec->vecA[i] = 0;
     return vec;
  }

  /* --- failure --- */
  free( vec);
  SERROR("allocation failure 2 double vector of size %d.\n", n);
  return NULL;
}

ClientData dvectorCreateItf( ClientData cd, int argc, char *argv[])
{
  if (argc == 1)  return (ClientData)dvectorCreate(0);
  else   if ( argc == 2) {
      DVector *vec = NULL;
      int n; double k,l;

      if (sscanf(argv[1],"%d",&n) != 1  ||  sscanf(argv[1],"%lf%lf",&k,&l) == 2) {
	 argc--;
	 if (itfParseArgv( argv[0], &argc, argv+1, 0,
	    "vector", ARGV_CUSTOM,  getDVector, &vec, cd,
		      "@filename or name or definition",
	    NULL) != TCL_OK) return NULL;
	 return (ClientData)vec;
      }
      return (ClientData)dvectorCreate(n);
   }
   
   ERROR("give vector or dimension\n");
   return NULL;
}

/* ------------------------------------------------------------------------
    Free
   ------------------------------------------------------------------------ */

void dvectorFree( DVector* vec)
{
  if (vec) {
    if ( vec->vecA) free( vec->vecA);
    free(vec);
  }
}

static int dvectorFreeItf( ClientData cd )
{
  dvectorFree((DVector*)cd);
  return TCL_OK;
}


/* ------------------------------------------------------------------------
    Configure
   ------------------------------------------------------------------------ */

static int dvectorConfigureItf( ClientData cd, char *variable, char *value )
{
  DVector *vec = (DVector*)cd;

  if (! variable) {
    ITFCFG( dvectorConfigureItf, cd, "n");
    ITFCFG( dvectorConfigureItf, cd, "count");
    return TCL_OK;
  }
  ITFCFG_Int(   variable,value,"n",    vec->n,    1);
  ITFCFG_Float( variable,value,"count",vec->count,0);
  
  ERROR("unknown option '-%s %s'.\n", variable,  value ? value : "?");
  return TCL_ERROR;
}
    

/* ------------------------------------------------------------------------
    dvectorGetItf
   ------------------------------------------------------------------------ */

DVector* dvectorGetItf( char* value)
{
  DVector* vec;

  /* ------------------------------------------------------------------------
     If 'value' is not an existing dvector object scan vector elements.
     ----------------------------------------------------------------------- */
  if ((vec = (DVector*)itfGetObject( value, "DVector")) == NULL) {
    int     argc;
    char**  argv;

    MSGCLEAR(NULL);

    if ( Tcl_SplitList( itf, value, &argc, &argv) == TCL_OK) {
       int i;

       if (! (vec = dvectorCreate( argc))) {
	  free((char*)argv); return NULL;
       }
       
       for ( i = 0; i < argc; i++) {
	  double f;

	  if ( sscanf( argv[i], "%lf", &f) != 1) {
	     ERROR("expected 'double' type elements.\n");
	     Tcl_Free((char*)argv);
	     dvectorFree( vec);
	     return NULL;
	  }
	  vec->vecA[i] = f;
       }
       Tcl_Free((char*)argv);
    }
    return vec;
 }
  /* ------------------------------------------------------------------------
     'value' is an existing dvector object.
     ----------------------------------------------------------------------- */
  else {
     
     DVector* nvec = dvectorCreate( vec->n);
     
     if (! nvec) return NULL;
     return dvectorCopy( nvec, vec);
  }
}

/* --- to be used with 'itfParseArgv()' --- */
int getDVector( ClientData cd, char* key, ClientData result, int *argcP, char *argv[])
{
   DVector *vec;
   
   if ( (*argcP<1) || ((vec = dvectorGetItf( argv[0]))  ==  NULL)) return -1;
   *((DVector**)result) = vec;
   (*argcP)--;   return 0;
}

/* =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
    Double Vector methods
   =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- */
/* ------------------------------------------------------------------------
    dvectorPutsItf
   ------------------------------------------------------------------------ */

static int dvectorPutsItf( ClientData cd, int argc, char *argv[])
{
  DVector *vec = (DVector*)cd;
  int      i;

  for ( i = 0; i < vec->n; i++) {
      itfAppendResult( "% 2.8e ", vec->vecA[i]);
  }
  return TCL_OK;
}

/* ------------------------------------------------------------------------
    Copy
   ------------------------------------------------------------------------ */

DVector* dvectorCopy( DVector* A, DVector* B)
{
  assert(A && B);

  if ( A->n != B->n) dvectorResize(A,B->n); 

  if (A->n) memcpy( A->vecA, B->vecA, sizeof(double) * A->n);
  A->count = B->count;
  return A;
}

int dvectorCopyItf( ClientData cd, int argc, char *argv[])
{
  DVector* vec = (DVector*)cd;
  DVector* tmp = NULL;

  argc--;
  if (itfParseArgv(argv[0], &argc, argv+1, 0,
		   "<dvector>", ARGV_CUSTOM, getDVector, &tmp, cd, "",
		   NULL) != TCL_OK) {
     if ( tmp) dvectorFree( tmp);
     return TCL_ERROR;
  }
  dvectorCopy( vec, tmp);
  dvectorFree( tmp);
  return TCL_OK;
}

/* ------------------------------------------------------------------------
    Resize
    Change number of coefficients and copy elements if possible.
   ------------------------------------------------------------------------ */

int dvectorResize( DVector* A, int n)
{
  double* p = NULL;

  assert(A && n >= 0);
  if ( A->n == n) return n;

  if (n) {
     if ((p = (double*)malloc(n * sizeof(double)))) {
	int i;
	for ( i = 0; i < n; i++) p[i] = 0.0;
	for ( i = (( A->n < n) ? A->n : n); i > 0; i--) p[i-1] = A->vecA[i-1];
     }
     else {
	SERROR("allocation failure in 'dvectorResize'.\n");
	return -1;
     }
  }

  if ( A->vecA) free( A->vecA);
  A->n    = n;
  A->vecA = p;
  return n;
}

int dvectorResizeItf( ClientData cd, int argc, char *argv[])
{
  int n;
  argc--;
  if (itfParseArgv( argv[0], &argc, argv+1, 0,
		    "<dimension>", ARGV_INT, NULL, &n, cd, "",
		   NULL) != TCL_OK)  return TCL_ERROR;
  dvectorResize( (DVector*)cd, n);
  return TCL_OK;
}

/* ------------------------------------------------------------------------
    Clear
   ------------------------------------------------------------------------ */

DVector* dvectorClear( DVector* A)
{
  int i;
  assert(A);
  for (i=0; i<A->n; i++) A->vecA[i] = 0.0;
  A->count = 0.0;
  return A;
}
/*
static int dvectorClearItf( ClientData cd, int argc, char *argv[])
{
  DVector* mat = (DVector*)cd;
  int   ac     =  argc-1;

  if ( itfParseArgv( argv[0], &ac, argv+1, 0,
       NULL) != TCL_OK) return TCL_ERROR;
  dvectorClear( mat); return TCL_OK;
}
*/
/* ------------------------------------------------------------------------
    Add
   ------------------------------------------------------------------------ */

DVector* dvectorAdd( DVector* C, double a, DVector* A, double b, DVector* B)
{
  int i;
   
  assert(A && B && C);

  if ( A->n != B->n) {
     SERROR("Vector sizes differ.\n");
     return NULL;
  }
  if (C->n != A->n) dvectorResize(C,A->n);
  for ( i=0; i<C->n; i++) C->vecA[i] = a * A->vecA[i] + b * B->vecA[i];
  return C;
}

int dvectorAddItf( ClientData cd, int argc, char *argv[])
{
  DVector *C = (DVector*)cd;
  DVector *A = NULL, *B = NULL;
  float    a = 1,  b = 1;

  argc--;
  if (itfParseArgv( argv[0], &argc, argv+1, 0,
		   "<a>",        ARGV_FLOAT,  NULL,         &a,  cd, "",
		   "<dvectorA>", ARGV_CUSTOM, getDVector,   &A,  cd, "",
		   "<b>",        ARGV_FLOAT,  NULL,         &b,  cd, "",
		   "<dvectorB>", ARGV_CUSTOM, getDVector,   &B,  cd, "",
		   NULL) != TCL_OK) {
     if (A) dvectorFree(A);
     if (B) dvectorFree(B);
     ERROR( "Usage: <float> <DVector> <float> <DVector>.\n");
     return TCL_ERROR;
  }
  C = dvectorAdd( C, a, A, b, B);
  dvectorFree(A);
  dvectorFree(B);
  if (C) return TCL_OK;
  return TCL_ERROR;
}


/* ========================================================================
    Double Matrix
   ======================================================================= */
/* ------------------------------------------------------------------------
    Create
   ------------------------------------------------------------------------ */

DMatrix* dmatrixCreate( int m, int n)
{
  /* --- allocate structure --- */
  DMatrix* mat = (DMatrix*)malloc(sizeof(DMatrix));
  if (! mat) {
    SERROR("allocation failure 1 double matrix of size %d x %d.", m, n);
    return NULL;
  }

  /* set dimensions and return if empty */
  mat->count = 0.0;
  if (!(m*n)) {
    mat->matPA = NULL;
     mat->m     = 0;
     mat->n     = 0;
     return mat;
  }
  mat->m = m;
  mat->n = n;

  /* --- allocate memory for data, init with 0 --- */
  if ((mat->matPA = (double**)malloc(m * sizeof(double*)))) {
    if ((mat->matPA[0] = (double*)malloc(m * n * sizeof(double)))) {
      int i,j;
      for ( i = 1; i < m; i++) mat->matPA[i] = mat->matPA[i-1] + n;
      for ( i = 0; i < m; i++)
         for ( j = 0; j < n; j++) mat->matPA[i][j] = 0.0;

      return mat;
    }
    free( mat->matPA);
  }

  /* --- failure --- */
  free( mat);
  SERROR("allocation failure 2 double matrix of size %d x %d.\n", m, n);
  return NULL;
}

static ClientData dmatrixCreateItf( ClientData cd, int argc, char *argv[])
{
  if (argc == 1)  return (ClientData)dmatrixCreate(0,0);
  else  if (argc == 2) {
     DMatrix *mat = NULL;
     argc--;
     if (itfParseArgv( argv[0], &argc, argv+1, 0,
	"matrix", ARGV_CUSTOM,  getDMatrix, &mat, cd,
		      "@filename or name or definition",
	NULL) != TCL_OK) return NULL;
     return (ClientData)mat;
  }
  else if (argc == 3) {
    int m=0, n=0;
    argc--;
    if (itfParseArgv( argv[0], &argc, argv+1, 0,
       "<m>", ARGV_INT,  NULL, &m, cd, "rows",
       "<n>", ARGV_INT,  NULL, &n, cd, "columns",
       NULL) != TCL_OK) return NULL;
    return (ClientData)dmatrixCreate(m,n);
  }
  ERROR("give matrix or <m> <n>.\n");
  return NULL;
}

/* ------------------------------------------------------------------------
    Free
   ------------------------------------------------------------------------ */

void dmatrixFree( DMatrix* mat)
{
  if (mat) {
    if ( mat->matPA) {
      if ( mat->matPA[0]) free(mat->matPA[0]);
      free( mat->matPA);
    }
    free(mat);
  }
}

static int dmatrixFreeItf( ClientData cd )
{
  dmatrixFree((DMatrix*)cd);
  return TCL_OK;
}

/* ------------------------------------------------------------------------
    Configure
   ------------------------------------------------------------------------ */

static int dmatrixConfigureItf( ClientData cd, char *variable, char *value )
{
  DMatrix *mat = (DMatrix*)cd;

  if (! variable) {
    ITFCFG( dmatrixConfigureItf, cd, "m");
    ITFCFG( dmatrixConfigureItf, cd, "n");
    ITFCFG( dmatrixConfigureItf, cd, "count");
    return TCL_OK;
  }
  ITFCFG_Int(   variable,value,"m",    mat->m,    1);
  ITFCFG_Int(   variable,value,"n",    mat->n,    1);
  ITFCFG_Float( variable,value,"count",mat->count,0);
  
  ERROR("unknown option '-%s %s'.\n", variable,  value ? value : "?");
  return TCL_ERROR;
}
    
/* ------------------------------------------------------------------------
    dmatrixGetItf

    Returns a pointer to a matrix. 'value' can either be a name of an
    existing matrix or the matrix elements: {{1.1 1.2  ...} {2.1 2.2 ...} ...}
    In all cases new memory will be allocated to hold the matrix contents. 
   ------------------------------------------------------------------------ */
DMatrix* dmatrixGetItf( char* value)
{
  DMatrix* mat;

  if ((mat = (DMatrix*)itfGetObject( value, "DMatrix")) == NULL) {
    int     argc;
    char**  argv;

    MSGCLEAR(NULL);

    if ( Tcl_SplitList( itf, value, &argc, &argv) == TCL_OK) {
      int i, j;

      for ( i = 0; i < argc; i++) {
        int    tokc;
        char** tokv;

        if ( Tcl_SplitList( itf, argv[i], &tokc, &tokv) == TCL_OK) {
            if ( i == 0 &&  ! (mat = dmatrixCreate( argc, tokc))) {
                Tcl_Free((char*)tokv); Tcl_Free((char*)argv);
                return NULL; 
            }

          for ( j = 0; j < tokc && j < mat->n; j++) {
            double d;

            if ( sscanf( tokv[j], "%lf", &d) != 1) {

              ERROR("expected 'double' type elements.\n");
              Tcl_Free((char*)tokv); Tcl_Free((char*)argv);
              dmatrixFree( mat);
              return NULL;
            }
            mat->matPA[i][j] = d;
          }
          Tcl_Free((char*)tokv);
        }
      }
    }
    Tcl_Free((char*)argv);
    return mat;
  }
  else {

    DMatrix* nmat = dmatrixCreate( mat->m, mat->n);

    if (! nmat) return NULL;
    return dmatrixCopy( nmat, mat);
  }
}

/* --------------------------------------------------------------------------
   getDMatrix
   getDMatrixP
   
   Both functions when used togeter with 'itfParseArgv()' will provide a
   pointer to a matrix. In case of 'getDMatrix' a name or a matrix definition
   can be used and new memory will be allocated. So the matrix has to be freed
   after use.
   For 'getDmatrixP' only the name of an existing matrix can be used and a
   pointer to this matrix will be returned.
   ------------------------------------------------------------------------- */
int getDMatrix( ClientData cd, char* key, ClientData result,
	        int *argcP, char *argv[])
/* free after use */
{
   DMatrix *mat;
   
   if ( (*argcP<1) || ((mat = dmatrixGetItf( argv[0]))  ==  NULL)) return -1;
   *((DMatrix**)result) = mat;
   (*argcP)--;   return 0;
}

int getDMatrixP( ClientData cd, char* key, ClientData result,
		 int *argcP, char *argv[])
/* don't free after use */
{
   DMatrix *mat;
   
   if ( (*argcP<1) || ((mat = (DMatrix*)itfGetObject( argv[0], "DMatrix"))  ==  NULL)) return -1;
   *((DMatrix**)result) = mat;
   (*argcP)--;   return 0;
}

/* =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
    Double Matrix methods
   =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- */
/* ------------------------------------------------------------------------
    dmatrixPutsItf
   ------------------------------------------------------------------------ */

int dmatrixPutsItf( ClientData cd, int argc, char *argv[])
{
  DMatrix *mat = (DMatrix*)cd;
  int      i,j;

  for ( i = 0; i < mat->m; i++) {
    itfAppendResult( "{ ");
    for ( j = 0; j < mat->n; j++) {
      itfAppendResult( "% 2.8e ", mat->matPA[i][j]);
    }
    itfAppendResult( "}\n");    
  }
  return TCL_OK;
}

/* ------------------------------------------------------------------------
    Copy
   ------------------------------------------------------------------------ */

DMatrix* dmatrixCopy( DMatrix* A, DMatrix* B)
{
  assert(A && B);

  if ( A->n != B->n || A->m != B->m) dmatrixResize(A,B->m,B->n); 

  if (A->m * A->n) memcpy( A->matPA[0], B->matPA[0], sizeof(double) * A->n * A->m);
  A->count = B->count;
  return A;
}

static int dmatrixCopyItf( ClientData cd, int argc, char *argv[])
{
  DMatrix* mat = (DMatrix*)cd;
  DMatrix* tmp;

  if ( argc != 2) {
    ERROR( "'copy' expects 1 parameter.\n");
    return TCL_ERROR;
  }
  if ((tmp = dmatrixGetItf(argv[1]))) {
    dmatrixCopy( mat, tmp);
    dmatrixFree( tmp);
    return TCL_OK;
  }
  return TCL_ERROR;
}

/* ------------------------------------------------------------------------
    GetVal
   ------------------------------------------------------------------------ */

int dmatrixGetValItf( ClientData cd, int argc, char *argv[])
{
  DMatrix     *mat = (DMatrix*)cd;
  int           ac = argc-1;
  int           x = 0,y = 0;

  if (itfParseArgv( argv[0], &ac, argv+1, 0,
    "<1st index>", ARGV_INT, NULL, &x, NULL, "first index",
    "<2nd index>", ARGV_INT, NULL, &y, NULL, "second index",
    NULL) != TCL_OK) return TCL_ERROR;

  if ((x < 0) || (x >= mat->m)) {
    ERROR("Invalid first index\n");
    return TCL_ERROR;
  }

  if ((y < 0) || (y >= mat->n)) {
    ERROR("Invalid second index\n");
    return TCL_ERROR;
  }

  itfAppendResult("%f",mat->matPA[x][y]);

  return TCL_OK;
}

/* ------------------------------------------------------------------------
    SetVal
   ------------------------------------------------------------------------ */
int dmatrixSetValItf( ClientData cd, int argc, char *argv[])
{
  DMatrix     *mat = (DMatrix*)cd;
  int           ac = argc-1;
  int           x = 0,y = 0;
  float         val = 0.0;

  if (itfParseArgv( argv[0], &ac, argv+1, 0,
    "<1st index>", ARGV_INT,   NULL, &x,   NULL, "first index",
    "<2nd index>", ARGV_INT,   NULL, &y,   NULL, "second index",
    "<value>",     ARGV_FLOAT, NULL, &val, NULL, "value",
    NULL) != TCL_OK) return TCL_ERROR;

  if ((x < 0) || (x >= mat->m)) {
    ERROR("Invalid first index\n");
    return TCL_ERROR;
  }

  if ((y < 0) || (y >= mat->n)) {
    ERROR("Invalid second index\n");
    return TCL_ERROR;
  }

  mat->matPA[x][y] = (double) val;

  return TCL_OK;
}

/* ========================================================================
    LU Decomposition
    Given a square Dmatrix a, this routine replaces it by the LU 
    decomposition of a rowwise permutation of itself.
    indx is a DVector that records the row permutation effected by the partial pivoting.
    It should have the size of the DMatrix. If not, it will be resized.
    d is output as +-1 depending on whether the number of row interchanges 
    was even or odd. This routine can be used to solve linear equations, 
    invert a matrix, or compute a determinant.
    This is an adapted version from 'numerical recipes in C'.
   ======================================================================== */
int dludcmp(DMatrix *a, DVector *indx, double *d) {
  int i, imax=0, j, k, n;
  double big, dum, sum, temp;
  DVector *vv;
  
  n = a->n;
  if (a->n != a->m) {
    SERROR("dludcmp assert error: called with non square matrix\n");
    return 2;
  }
  vv = dvectorCreate(n);
  dvectorResize(indx, n);
  *d = 1.0;

  /* loop over rows to get implicit scaling information */
  for (i=0; i<n; i++) {
    big=0.0;
    for (j=0; j<n; j++) {
      if ((temp=fabs(a->matPA[i][j])) > big) big=temp;
    }
    if (big == 0.0) {
      /* WARN ("Singular matrix in dludcmp!\n"); */
      return 1;
    }
    vv->vecA[i] = 1.0 / big;
  }
  /* this is Crout's algorithm */
  for (j=0; j<n; j++) {
    for (i=0; i<j; i++) {
      sum = a->matPA[i][j];
      for (k=0; k<i; k++) sum -= a->matPA[i][k] * a->matPA[k][j];
      a->matPA[i][j] = sum;
    }
    big=0.0;
    for (i=j; i<n; i++) {
      sum=a->matPA[i][j];
      for (k=0; k<j; k++) {
	sum -= a->matPA[i][k] * a->matPA[k][j];
      }
      a->matPA[i][j] = sum;
      if ( (dum=vv->vecA[i] * fabs(sum)) >= big ) {
	/* better pivot than best so far! */
	big = dum;
	imax = i;
      }
    }
    if (j != imax) {
      /* now we have to interchange rows */
      for (k=0; k<n; k++) {
	dum = a->matPA[imax][k];
	a->matPA[imax][k] = a->matPA[j][k];
	a->matPA[j][k] = dum;
      }
      *d = -(*d); /* keep track of parity */
      vv->vecA[imax] = vv->vecA[j];
    }
    indx->vecA[j] = imax;
    if (a->matPA[j][j] == 0.0) a->matPA[j][j] = 1.0e-20; /* well, but the matrix is singular! */
    if (j != n) {
      dum = 1.0 / a->matPA[j][j] ;
      for(i=j+1; i<n; i++) a->matPA[i][j] *= dum;
    }
  } /* next column */
  dvectorFree(vv); 
  return 0;
}
 
   
/* ========================================================================
    dmatrixDet
    Compute the determinant of a matrix
   ======================================================================== */
double dmatrixDet(DMatrix *A) {
  DMatrix *a;
  DVector *dummy;
  double d;
  int i;
  if (A->m != A->n) {
    SERROR("dmatrixDet: non-square matrix\n");
    return(0.0);
  }
	   
  a = dmatrixCreate(A->m, A->n);
  dmatrixCopy(a,A);
  dummy = dvectorCreate(A->n);
  dludcmp(a, dummy, &d);
  dvectorFree(dummy);
  for (i=0; i < A->n; i++) { d *= a->matPA[i][i]; }

  dmatrixFree(a);

  return(d);
}

/* ========================================================================
    dmatrixLogDet
    Compute the natural log of the absolute value of the determinant of a matrix
    In *sign the sign of the determinant is returned (1.0 or -1.0)
    If the determinant is zero, *sign is returned zero
   ======================================================================== */
double dmatrixLogDet(DMatrix *A, double *sign) {
  DMatrix *a;
  DVector *dummy;
  double d;
  int i;
  if (A->m != A->n) {
    SERROR("dmatrixDet: non-square matrix\n");
    return(0.0);
  }
	   
  a = dmatrixCreate(A->m, A->n);
  dmatrixCopy(a,A);
  dummy = dvectorCreate(A->n);
  dludcmp(a, dummy, &d);
  dvectorFree(dummy);
  *sign = d; d=0.0;
  for (i=0; i < A->n; i++) { 
    *sign *= ( a->matPA[i][i] < 0 ? -1.0 : 1.0 );
    if (fabs(a->matPA[i][i]) > 0) {
      d += log(fabs(a->matPA[i][i]));
    } else {
      *sign = 0.0;
      d += 1e20;
    }
  }
  
  return(d);
}

/* -----------------------------------------------------------------------
    dmatrixDeterminantItf
   ----------------------------------------------------------------------- */
static int dmatrixDeterminantItf (ClientData cd, int argc, char *argv[]) {
  DMatrix *A = (DMatrix *)cd;
  int    ac = argc - 1;
  double det;
  char*  format = "%g";

  if ( itfParseArgv( argv[0], &ac, argv+1, 0, 
      "-format", ARGV_STRING, NULL, &format, cd, "format string",
      NULL) != TCL_OK) return TCL_ERROR;
  det = dmatrixDet(A);
  itfAppendResult(format, det);
  return TCL_OK;
}

 /* -------------------------------------------------------------------- */
 /*  LU decomposition                     version to support OFS stuff   */
 /* -------------------------------------------------------------------- */

static void  ludcmp (double **a, int n, double *d) {

  int      i,imax=0,j,k;
  double   big,dum,sum,temp;
  double   *vv;

  vv= (double *) malloc (sizeof(double) * n);
  *d=1.0;
  for (i=0;i<n;i++) {
    big=0.0;
    for (j=0;j<n;j++)
      if ((temp=fabs(a[i][j])) > big) big=temp;
      if (big == 0.0) ERROR("Singular matrix in routine ludcmp");
      vv[i]=1.0/big;
    }
    for (j=0;j<n;j++) {
      for (i=0;i<j;i++) {
        sum=a[i][j];
        for (k=0;k<i;k++) sum -= a[i][k]*a[k][j];
        a[i][j]=sum;
      }
      big=0.0;
      for (i=j;i<n;i++) {
        sum=a[i][j];
        for (k=0;k<j;k++)
          sum -= a[i][k]*a[k][j];
        a[i][j]=sum;
        if ( (dum=vv[i]*fabs(sum)) >= big) {
          big=dum;
          imax=i;
        }
      }
      if (j != imax) {
        for (k=0;k<n;k++) {
          dum=a[imax][k];
          a[imax][k]=a[j][k];
          a[j][k]=dum;
        }
        *d = -(*d);
        vv[imax]=vv[j];
      }
      if (a[j][j] == 0.0) a[j][j]=1E-20;
      if (j != n) {
      dum=1.0/(a[j][j]);
      for (i=j+1;i<n;i++) a[i][j] *= dum;
    }
  }
  free(vv);
}
   
/* -------------------------------------------------------------------- */
/*  dmatrixExtDet                      extended version to support OFS  */
/* -------------------------------------------------------------------- */
int  dmatrixExtDet (DMatrix *A, double *logdet, double *sign) {

  DMatrix        *D;
  int            k;
  double         d,sum,val;

  assert (A);

  if (A->m != A->n) {
    ERROR("A must be squared\n");
    return TCL_ERROR;
  }

  D = dmatrixCreate(A->m,A->n);
  dmatrixCopy(D,A);

  ludcmp(D->matPA,D->m,&d);

  sum = 0.0;
  for (k=0; k<A->m; k++) {
    if ((val = D->matPA[k][k]) < 0.0) {
      val = -1.0*val;
      d *= -1.0;
    }
    sum += log(val);
  }

  *logdet = sum;
  *sign   = d;

  dmatrixFree(D);

  return TCL_OK;
}

/* ------------------------------------------------------------------------
    Resize
   ------------------------------------------------------------------------ */

int dmatrixResize( DMatrix* A, int m, int n)
{
  double **p = NULL;

  assert(A && n >= 0 && m >= 0);
  if (A->m == m && A->n == n) return (m*n);
  
  if (m*n) {
     if ( (p = (double**)malloc( m * sizeof(double*))) &&
	  (p[0] = (double*)malloc( m * n * sizeof(double)))) {
	   int i,j;
	   for ( i = 1; i < m; i++) p[i] = p[i-1] + n;
	   for ( i = 0; i < m; i++)
	      for ( j = 0; j < n; j++) p[i][j] = 0.0;

	   for ( i = (( A->m < m) ? A->m : m); i > 0; i--)
	      for ( j = (( A->n < n) ? A->n : n); j > 0; j--) 
		 p[i-1][j-1] = A->matPA[i-1][j-1];
     }
     else {
	if (p) free(p);
	SERROR("allocation failure in 'dmatrixResize'.\n");
	return -1;
     }
  }
  else m = n = 0;
	
  if ( A->matPA) {
     if (  A->matPA[0]) free(A->matPA[0]);
     free( A->matPA);
  }
  A->m     = m;
  A->n     = n;
  A->matPA = p;
  return m * n;

}

static int dmatrixResizeItf( ClientData cd, int argc, char *argv[])
{
  int m, n;

  if ( argc != 3) {

    ERROR("'resize' expects 2 parameters.\n");
    return TCL_ERROR;
  }

  if ( sscanf( argv[1], "%d", &m) != 1 ||
       sscanf( argv[2], "%d", &n) != 1) {

    ERROR("matrix dimensions must be 'int' type.\n");
    return TCL_ERROR;
  }
  dmatrixResize( (DMatrix*)cd, m, n);
  return TCL_OK;
}

/* ------------------------------------------------------------------------
    Clear
   ------------------------------------------------------------------------ */

DMatrix* dmatrixClear( DMatrix* A)
{
  int i,j;
  assert(A);

  for (i=0; i<A->m; i++)
    for (j=0; j<A->n; j++)
      A->matPA[i][j] = 0.0;
  A->count = 0.0;
  return A;
}

static int dmatrixClearItf( ClientData cd, int argc, char *argv[])
{
  DMatrix* mat = (DMatrix*)cd;
  int   ac     =  argc-1;

  if ( itfParseArgv( argv[0], &ac, argv+1, 0,
       NULL) != TCL_OK) return TCL_ERROR;
  dmatrixClear( mat); return TCL_OK;
}

/* -------------------------------------------------------------------- */
/*  dmatrixInitConst                                                    */
/* -------------------------------------------------------------------- */

int  dmatrixInitConst (DMatrix *fmat, double initVal) 
{
  register int    i,j;

  for (i=0; i<fmat->m; i++)
    for (j=0; j<fmat->n; j++)
      fmat->matPA[i][j] = initVal;

  return TCL_OK;
}

/* -------------------------------------------------------------------- */
/*  dmatrixInitDiag                                                     */
/* -------------------------------------------------------------------- */

int  dmatrixInitDiag (DMatrix *fmat, double initVal) 
{
  register int    i,j;

  for (i=0; i<fmat->m; i++)
    for (j=0; j<fmat->n; j++)
      if (i==j)
        fmat->matPA[i][j] = initVal;
      else
        fmat->matPA[i][j] = 0.00;

  return TCL_OK;
}


/* -------------------------------------------------------------------- */
/*  dmatrixInitRandom                                                   */
/* -------------------------------------------------------------------- */

int  dmatrixInitRandom (DMatrix *fmat, double maxAbsVal) 
{
  register int    i,j;

  for (i=0; i<fmat->m; i++)
    for (j=0; j<fmat->n; j++)
      fmat->matPA[i][j] = chaosRandom(-maxAbsVal,+maxAbsVal);

  return TCL_OK;
}

/* -------------------------------------------------------------------- */
/*  dmatrixScale     C = A * gamma     (C=A is possible)                */
/* -------------------------------------------------------------------- */

int  dmatrixScale (DMatrix *C, DMatrix *A, double gamma) 
{
  register int      i,j;

  assert (C);
  assert (A);

  dmatrixResize(C,A->m,A->n);
  for (i=0; i<A->m; i++)
    for (j=0; j<A->n; j++)
      C->matPA[i][j] = A->matPA[i][j] * gamma;
  
  return TCL_OK;
}

/* -------------------------------------------------------------------- */
/*  dmatrixMinmax                                                       */
/* -------------------------------------------------------------------- */

int dmatrixMinmax (DMatrix* A,double* min,double* max)
{
  register int i,j;
  double mini,maxi,coeff;

  if (A->m == 0 || A->n == 0) {
    ERROR("Matrix mustn't be empty!\n");
    return TCL_ERROR;
  }
  mini = maxi = A->matPA[0][0];
  for (i = 0; i < A->m; i++)
    for (j = 0; j < A->n; j++) {
      coeff = A->matPA[i][j];
      if (coeff > maxi) maxi = coeff;
      else if (coeff < mini) mini = coeff;
    }

  *max = maxi;
  *min = mini;
  return TCL_OK;
}

/* ------------------------------------------------------------------------
    Transpose
   ------------------------------------------------------------------------ */

DMatrix* dmatrixTrans( DMatrix* A, DMatrix* B)
{
  int i,j;
  assert(A && B);

  if (A==B) return NULL;
  if ( A->n != B->m || A->m != B->n) dmatrixResize(A,B->n,B->m); 

  for (i=0; i<A->m; i++)
     for (j=0; j<A->n; j++)
	A->matPA[i][j] = B->matPA[j][i];
  return A;
}

int dmatrixTransItf( ClientData cd, int argc, char *argv[])
{
  DMatrix* mat = (DMatrix*)cd;
  DMatrix* tmp;

  if ( argc != 2) {
    ERROR( "expecting <DMatrix>.\n");
    return TCL_ERROR;
  }
  if ((tmp = dmatrixGetItf(argv[1]))) {
    dmatrixTrans( mat, tmp);
    dmatrixFree( tmp);
    return TCL_OK;
  }
  return TCL_ERROR;
}

/* ------------------------------------------------------------------------
    Add
   ------------------------------------------------------------------------ */

DMatrix* dmatrixAdd( DMatrix* C, double a, DMatrix* A, double b, DMatrix* B)
{
  int i,j;
   
  assert(A && B && C);

  if ( A->n != B->n || A->m != B->m) {
     SERROR("Matrix sizes differ.\n");
     return NULL;
  }
  dmatrixResize(C,A->m,A->n);

  for ( i = 0; i < C->m; i++) {
     for ( j = 0; j < C->n; j++) {
	C->matPA[i][j] = a * A->matPA[i][j] + b * B->matPA[i][j];
     }
  }
  return C;
}

int dmatrixAddItf( ClientData cd, int argc, char *argv[])
{
  DMatrix *C = (DMatrix*)cd;
  DMatrix *A = NULL, *B = NULL;
  float    a = 1,  b = 1;

  argc--;
  if (itfParseArgv( argv[0], &argc, argv+1, 0,
		   "<a>",        ARGV_FLOAT,  NULL,         &a,  cd, "",
		   "<dmatrixA>", ARGV_CUSTOM, getDMatrix,   &A,  cd, "",
		   "<b>",        ARGV_FLOAT,  NULL,         &b,  cd, "",
		   "<dmatrixB>", ARGV_CUSTOM, getDMatrix,   &B,  cd, "",
		   NULL) != TCL_OK) {
     if (A) dmatrixFree(A);
     if (B) dmatrixFree(B);
     ERROR( "Usage: <float> <DMatrix> <float> <DMatrix>.\n");
     return TCL_ERROR;
  }
  C = dmatrixAdd( C, a, A, b, B);
  dmatrixFree(A);
  dmatrixFree(B);
  if (C) return TCL_OK;
  return TCL_ERROR;
}


/* ------------------------------------------------------------------------
   accu:  C = C + A * a
------------------------------------------------------------------------ */

int dmatrixAccu(DMatrix* C, DMatrix* A, double a)
{
        register int        i,j;

        assert(C && A);

        if ((A->m != C->m) || (A->n != C->n)) {
                ERROR("dmatrixAccu(): dimensions do not match\n");
                return TCL_ERROR;
        }

        for (i=0; i<C->m; i++)
                for (j=0; j<C->n; j++)
                        C->matPA[i][j] += a * A->matPA[i][j];

        return TCL_OK;
}


/* ------------------------------------------------------------------------
   sub: C = A - B
------------------------------------------------------------------------ */

int dmatrixSub(DMatrix *C, DMatrix *A, DMatrix *B)
{
        register int        i,j;

        assert(C && A && B);

        if ((A->m != B->m) || (A->n != B->n)) {
                ERROR("dmatrixSub(): dimensions do not match\n");
                return TCL_ERROR;
        }

        dmatrixResize(C,A->m,A->n);

        for (i=0; i<A->m; i++)
                for (j=0; j<A->n; j++)
                        C->matPA[i][j] = A->matPA[i][j] - B->matPA[i][j];

        return TCL_OK;
}


/* ------------------------------------------------------------------------
    Multiply A * B
   ------------------------------------------------------------------------ */

DMatrix* dmatrixMul( DMatrix* C, DMatrix* A, DMatrix* B)
{
  int i,j,k;
  double sum00, sum01, sum10, sum11;
  double *aP0, *bP0, *aP1, *bP1;

  assert(A && B && C);
  if (C == A || C == B) {
     SERROR("matrix multiplication: result matrix must be different!\n");
     return NULL;
  }
  if ( A->n != B->m) {
    SERROR("Can't multiply matrix %d columns with matrix %d rows\n",A->n,B->m);
    return NULL;
  }
  dmatrixResize(C,A->m,B->n);
  /* ---------------------------------
      I.      i..(+2)
     --------------------------------- */
  for ( i = 0; i < C->m - 1; i += 2) {
     /* ---------------------------------
	 Ia.      j..(+2)
	--------------------------------- */
     for ( j = 0; j < C->n - 1; j += 2) {
	sum00 = 0.0; sum01 = 0.0; sum10 = 0.0; sum11  = 0.0;
	aP0 = A->matPA[i];   bP0 = B->matPA[0] + j;
	aP1 = A->matPA[i+1]; bP1 = B->matPA[0] + j + 1;
	for ( k = 0; k < A->n; k++, bP0 += B->n, bP1 += B->n) {
	   sum00 += *aP0   * *bP0;
	   sum01 += *aP0++ * *bP1;
	   sum10 += *aP1   * *bP0;
	   sum11 += *aP1++ * *bP1;
	}
	C->matPA[i  ][j  ] = sum00;
	C->matPA[i  ][j+1] = sum01;
	C->matPA[i+1][j  ] = sum10;
	C->matPA[i+1][j+1] = sum11;
     }
     /* ---------------------------------
	 Ib.      j..(+1)
	--------------------------------- */
     for ( ; j < C->n; j ++) {
	sum00 = 0.0; sum10 = 0.0;
	aP0 = A->matPA[i];   bP0 = B->matPA[0] + j;
	aP1 = A->matPA[i+1];
	for ( k = 0; k < A->n; k++, bP0 += B->n) {
	   sum00 += *aP0++ * *bP0;
	   sum10 += *aP1++ * *bP0;
	}
	C->matPA[i  ][j] = sum00;
	C->matPA[i+1][j] = sum10;
     }
  }
  /* ---------------------------------
      II.      i..(+1)
     --------------------------------- */
  for ( ; i < C->m; i++) {
     /* ---------------------------------
	 IIa.      j..(+2)
	--------------------------------- */
     for ( j = 0; j < C->n - 1; j += 2) {
	sum00 = 0.0; sum01 = 0.0;
	aP0 = A->matPA[i];   bP0 = B->matPA[0] + j;
	bP1 = B->matPA[0] + j + 1;
	for ( k = 0; k < A->n; k++, bP0 += B->n, bP1 += B->n) {
	   sum00 += *aP0   * *bP0;
	   sum01 += *aP0++ * *bP1;
	}
	C->matPA[i][j  ] = sum00;
	C->matPA[i][j+1] = sum01;
     }
     /* ---------------------------------
	 IIb.      j..(+1)
	--------------------------------- */
     for ( ; j < C->n; j++) {
	sum00 = 0.0;
	aP0 = A->matPA[i];   bP0 = B->matPA[0] + j;
	for ( k = 0; k < A->n; k++, bP0 += B->n) {
	   sum00 += *aP0++ * *bP0;
	}
	C->matPA[i][j] = sum00;
     }
  }
  return C;

}

int dmatrixMulItf( ClientData cd, int argc, char *argv[])
{
  DMatrix *C = (DMatrix*)cd;
  DMatrix *A = NULL, *B = NULL;
  int ret    = TCL_OK;
  
  argc--;
  if (itfParseArgv( argv[0], &argc, argv+1, 0,
    "<matrix>", ARGV_CUSTOM, getDMatrix, &A, cd, "matrix A",
    "<matrix>", ARGV_CUSTOM, getDMatrix, &B, cd, "matrix B",
    NULL) != TCL_OK) ret = TCL_ERROR;
  else 
    if (dmatrixMul( C, A, B) == NULL) ret = TCL_ERROR;
  if (A) dmatrixFree(A);
  if (B) dmatrixFree(B);
  return ret;
}

/* ------------------------------------------------------------------------
    Multiply A * B'
   ------------------------------------------------------------------------ */
DMatrix* dmatrixMulot( DMatrix* C, DMatrix* A, DMatrix* B)
{
  int i,j,k;
  double sum00, sum01, sum10, sum11;
  double *aP0, *bP0, *aP1, *bP1;
  
  assert(A && B && C);
  if (C == A || C == B) {
     SERROR("matrix multiplication: result matrix must be different!\n");
     return NULL;
  }
  if ( A->n != B->n) {
     SERROR("Matrix columns differ: %d and %d.\n",A->n,B->n);
     return NULL;
  }
  dmatrixResize(C,A->m,B->m);

  /* ---------------------------------
      I.      i..(+2)
     --------------------------------- */
  for ( i = 0; i < C->m - 1; i += 2) {
     /* ---------------------------------
	 Ia.      j..(+2)
	--------------------------------- */
     for ( j = 0; j < C->n - 1; j += 2) {
	sum00 = 0.0; sum01 = 0.0; sum10 = 0.0; sum11  = 0.0;
	aP0 = A->matPA[i];   bP0 = B->matPA[j];
	aP1 = A->matPA[i+1]; bP1 = B->matPA[j+1];
	for ( k = 0; k < A->n; k++) {
	   sum00 += *aP0   * *bP0;
	   sum01 += *aP0++ * *bP1;
	   sum10 += *aP1   * *bP0++;
	   sum11 += *aP1++ * *bP1++;
	}
	C->matPA[i  ][j  ] = sum00;
	C->matPA[i  ][j+1] = sum01;
	C->matPA[i+1][j  ] = sum10;
	C->matPA[i+1][j+1] = sum11;
     }
     /* ---------------------------------
	 Ib.      j..(+1)
	--------------------------------- */
     for ( ; j < C->n; j ++) {
	sum00 = 0.0; sum10 = 0.0;
	aP0 = A->matPA[i];   bP0 = B->matPA[j];
	aP1 = A->matPA[i+1];
	for ( k = 0; k < A->n; k++) {
	   sum00 += *aP0++ * *bP0;
	   sum10 += *aP1++ * *bP0++;
	}
	C->matPA[i  ][j] = sum00;
	C->matPA[i+1][j] = sum10;
     }
  }
  /* ---------------------------------
      II.      i..(+1)
     --------------------------------- */
  for ( ; i < C->m; i++) {
     /* ---------------------------------
	 IIa.      j..(+2)
	--------------------------------- */
     for ( j = 0; j < C->n - 1; j += 2) {
	sum00 = 0.0; sum01 = 0.0;
	aP0 = A->matPA[i];   bP0 = B->matPA[j];
	bP1 = B->matPA[j+1];
	for ( k = 0; k < A->n; k++) {
	   sum00 += *aP0   * *bP0++;
	   sum01 += *aP0++ * *bP1++;
	}
	C->matPA[i][j  ] = sum00;
	C->matPA[i][j+1] = sum01;
     }
     /* ---------------------------------
	 IIb.      j..(+1)
	--------------------------------- */
     for ( ; j < C->n; j++) {
	sum00 = 0.0;
	aP0 = A->matPA[i];   bP0 = B->matPA[j];
	for ( k = 0; k < A->n; k++) {
	   sum00 += *aP0++ * *bP0++;
	}
	C->matPA[i][j] = sum00;
     }
  }
  return C;
}

int dmatrixMulotItf( ClientData cd, int argc, char *argv[])
{
  DMatrix *C = (DMatrix*)cd;
  DMatrix *A = NULL, *B = NULL;
  int ret    = TCL_OK;
  
  argc--;
  if (itfParseArgv( argv[0], &argc, argv+1, 0,
    "<matrix>", ARGV_CUSTOM, getDMatrix, &A, cd, "matrix A",
    "<matrix>", ARGV_CUSTOM, getDMatrix, &B, cd, "matrix B",
    NULL) != TCL_OK) ret = TCL_ERROR;
  else 
    if (dmatrixMulot( C, A, B) == NULL) ret = TCL_ERROR;
  if (A) dmatrixFree(A);
  if (B) dmatrixFree(B);
  return ret;
}

/* ------------------------------------------------------------------------
    covmul  A * B * A'
   ------------------------------------------------------------------------ */
DMatrix* dmatrixCovmul( DMatrix* C, DMatrix* A, DMatrix* B)
{
  DMatrix *R = NULL;
  DMatrix *H = dmatrixCreate(B->n,B->m); if (!H) return NULL;
  assert(A && B && C);

  if (dmatrixTrans(C,B)    &&           /* C is B'             */
      dmatrixMulot(H,A,C)  &&           /* H is A * B          */
      dmatrixMulot(C,H,A)) R = C;       /* C is A * B * A'     */
  dmatrixFree(H);
  return R;
}

/* ------------------------------------------------------------------------
    Unity
   ------------------------------------------------------------------------ */
DMatrix* dmatrixUnity( DMatrix* A)
{
  int  i, dim;

  assert(A);
  if (A->m != A->n)
    SWARN("unity: rows (%d) and columns (%d) differ!\n",A->m,A->n);
  dim = A->m <= A->n ? A->m : A->n; 
  dmatrixClear(A);
  for ( i=0; i<dim; i++)   A->matPA[i][i] = 1.0;
  return A;
}

int dmatrixUnityItf( ClientData cd, int argc, char *argv[])
{
  DMatrix *R = dmatrixUnity((DMatrix*)cd);
  if (R == NULL) return TCL_ERROR;
  else           return TCL_OK;
}

/* ------------------------------------------------------------------------
    whiten     whitening diagonal matrix A
   ------------------------------------------------------------------------ */
DMatrix* dmatrixWhiten( DMatrix* A)
{
  int i;
  int error = 0;
  assert(A);
  
  
  if (A->m != A->n) {
    ERROR_SQMAT("whiten:",A->m, A->n);
    return NULL;
  }
  for (i=0; i<A->m; i++) {
    double d = A->matPA[i][i];
    if (d > 0.0) A->matPA[i][i]= 1.0/sqrt(d);
    else if (d < 0.0) A->matPA[i][i]= -1.0/sqrt(-d);
    else {
      ERROR("whiten: zero element (%d,%d)\n",i,i);
      error = 1;
    }
  }
  if (error) return NULL;
  return A;
}

/* ------------------------------------------------------------------------
    swapRow     swap matrix rows
   ------------------------------------------------------------------------ */
static DMatrix* dmatrixSwapRow( DMatrix* A, int r1, int r2)
{
  int i;
  assert(A);

  if (r1 < 0 || r2 < 0 || r1 > (A->m - 1) || r2 > (A->m - 1)) {
    ERROR("swapRow: rows %d, %d out of range 0..%d\n",r1,r2,A->m-1);
    return NULL;
  }
  for (i=0; i<A->n; i++) {
    double h = A->matPA[r1][i];
    A->matPA[r1][i] = A->matPA[r2][i];
    A->matPA[r2][i] = h;
  }
  return A;
}


/* ------------------------------------------------------------------------
    Eigen

    dmatrixEigen()
    Finds the eigenvalues and eigenvectors of A using the Jacobi
    transformation. Returns the number of iterations or a negative value
    in case an error occured.
    
    eigenvalues:   diagonal elements of A 
    eigenvectors:  the normalized eigenvectors will be found in EV
    threshold for largest non diagonal element:  th
    maximal number of iterations:                max_iter

    note: The eigenvalues might not be sorted!
    ------------------------------------------------------------------------ */
static int symTest(DMatrix *A,float percent)
/* test if matrix is symmetric */     
{
  int i,j,dim=A->m;
  double err,c1,c2;
  
  if (A->m != A->n) {
    ERROR_SQMAT("symTest:",A->m, A->n);
    return TCL_ERROR;
  }
  /* INFO("symmetry test (ST).\n"); */
  for (i=0; i<(dim-1); i++) 
     for (j=i+1; j<dim; j++) {
        c1 = A->matPA[i][j];
        c2 = A->matPA[j][i];
        if ((c1+c2)!=0) {
           err = fabs( c1 - c2) / (0.005*(c1 + c2));
           if (err!=0)  WARN("ST: deviation %5.3f%% in element (%d,%d).\n",
			      err,i,j);
           if (err>percent) {
	     ERROR("ST: matrix is not symmetric, element (%d,%d).\n",i,j);
	     return TCL_ERROR;
	   }
           if (err!=0) A->matPA[j][i] = A->matPA[i][j] = (c1 + c2)/2; 
        }
        else {
           /* ...? */
        };
     }
  /* INFO("ST: symmetry test ok!\n"); */
  return TCL_OK;
}

static void jacobi(DMatrix* M,int p,int q,double *sinphi,double *cosphi)
/* Jacobi-Transformation, set (p,q)-element from M to 0 */
/* Eigenvalues will remain unchanged                    */
{
   double phi,kp,kq;
   int i,j,dim = M->m;

   phi = 0.5 * atan2(2*M->matPA[p][q] , M->matPA[p][p]-M->matPA[q][q]);
   *cosphi=cos(phi);  *sinphi=sin(phi);
   for (j=0; j<dim; j++) {      
      kp=M->matPA[p][j];   kq=M->matPA[q][j];
      M->matPA[p][j] =  kp**cosphi + kq**sinphi;
      M->matPA[q][j] = -kp**sinphi + kq**cosphi;
   }
   for (i=0; i<dim; i++) {
      kp=M->matPA[i][p];   kq=M->matPA[i][q];  
      M->matPA[i][p] =  kp**cosphi + kq**sinphi;
      M->matPA[i][q] = -kp**sinphi + kq**cosphi;
   }
}

static double maxNotDiag(DMatrix* M,int *p,int *q)
/* Search for the largest non diagonal element */
/* of a symmetric matrix                       */
{
   double max=0.0,kabs;
   int i,j,dim = M->m;
   
   for (i=0; i<(dim-1); i++)       
      for (j=i+1; j<dim; j++) {
         kabs=fabs(M->matPA[i][j]);
         if (kabs>max) {max=kabs; *p=i; *q=j;};
      } 
   /* printf("maxNotDiag=%6.6f is element (%d,%d)\n",max,*p,*q); */
   return max;
}

static void dmatrixNormRows(DMatrix* A)
/* normalizing the rows in matrix A */
{
  int i,j;
  for ( i=0; i<A->m; i++) {
    double l = 0.0;
    for ( j=0; j<A->n; j++) l += (A->matPA[i][j] * A->matPA[i][j]); 
    if (l>0) {
      l = sqrt(l);
      for ( j=0; j<A->n; j++)  A->matPA[i][j]/=l;
    }
  }
}

int dmatrixClean(DMatrix* A,double thresh)
/* cleaning up non diagonal elements close to 0.0 */     
/* return 1 if clean, otherwise 0                 */
{
  int i,j;
  int clean = 1;
  assert(A);
  
  for (i=0; i<A->m; i++) for (j=0; j<A->n; j++)
      if (i!=j){
	if ( A->matPA[i][j] < thresh) A->matPA[i][j] = 0.0;
	else clean = 0;
      }
  return clean;
}

static int sortEigen(DMatrix* EVal, DMatrix* EVec)
/* The input is matrix 'EVal' with eigenvalues as diagonal elements
   and matrix 'EVev' with the eigenvectors as rows. The eigenvectors
   will be sorted according to their eigenvalues.                */
{
  int i,j;
  assert(EVal && EVec && EVal->m == EVal->n && EVec->m == EVec->n &&
	 EVal->m == EVec->m);
  
  for (i=0; i<(EVal->m-1); i++)
    for (j=i+1; j<EVal->m; j++)
      if (EVal->matPA[j][j]*EVal->matPA[j][j] >
	  EVal->matPA[i][i]*EVal->matPA[i][i]) {
	double p = EVal->matPA[j][j];
	EVal->matPA[j][j] = EVal->matPA[i][i];
	EVal->matPA[i][i] = p;
	dmatrixSwapRow(EVec,i,j);
      }
  return 0;
}

int dmatrixEigen(DMatrix* A, DMatrix* EV, double th,int max_iter)
/* the main eigen routine, comments see above */
{
   int p=0,q=0;
   int c=0, dim=A->m;
   DMatrix* H  = dmatrixCreate(dim,dim);
   DMatrix* O  = dmatrixCreate(dim,dim);
   double sinphi,cosphi;

   if (!H || !O) {
     if (H) dmatrixFree(H);
     if (O) dmatrixFree(O);
     return -1;
   }
   
   dmatrixUnity(H);
   dmatrixResize(EV,dim,dim); 
   while ((maxNotDiag(A,&p,&q)>th) && (c<max_iter))
   {  c++;
      jacobi(A,p,q,&sinphi,&cosphi);
      
      dmatrixUnity(O);
      O->matPA[p][p] =  cosphi;   O->matPA[q][q] = cosphi;
      O->matPA[p][q] = -sinphi;   O->matPA[q][p] = sinphi;

      dmatrixCopy(EV,H);
      dmatrixMul(H,EV,O);
   }
   dmatrixTrans(EV,H);
   dmatrixNormRows(EV);       
   dmatrixFree(H);
   dmatrixFree(O);
   return c;
}

int dmatrixEigenItf( ClientData cd, int argc, char *argv[])
{
  DMatrix *A  = (DMatrix*)cd;
  DMatrix *EV = NULL;
  int     iter   = 50000;
  float   thresh = 1.0e-7;
  int     clean  = 1;
  int     sort   = 1;
  int     iterX;

  argc--;
  if (itfParseArgv( argv[0], &argc, argv+1, 0,
    "<matrix>",ARGV_CUSTOM,getDMatrixP,&EV,   cd,"matrix to hold eigenvectors",
    "-iter",  ARGV_INT,    NULL,       &iter, cd,  "max. number of iterations",
    "-thresh",ARGV_FLOAT,  NULL,       &thresh,cd,
		                     "threshold for max. non diagonal element",
    "-clean", ARGV_INT,    NULL,       &clean,cd,"clean up eigenvalue matrix",
    "-sort",  ARGV_INT,    NULL,       &sort, cd,"sort eigenvalues",
    NULL) != TCL_OK) return TCL_ERROR;

  if (symTest(A,0.01) == TCL_ERROR) return TCL_ERROR;
  if ((iterX=dmatrixEigen(A,EV,(double)thresh,iter)) < 0) return TCL_ERROR;
  if (clean && !dmatrixClean(A,(double)thresh))
    WARN("After %d of %d Jacobi-iterations\n"
	 "eigenvalue matrix is not diagonal\n",iterX,iter);
  if (sort) sortEigen(A,EV);
  return TCL_OK;
}

/* ------------------------------------------------------------------------
    dmatrixEigenQR
   ------------------------------------------------------------------------ */

int dmatrixEigenQR(DMatrix* A, DMatrix* EV, double th,int max_iter)
{
  DMatrix *D  = NULL;
  int      rc = TCL_OK;

  D = _dmatrixCreate(1,A->n);
  if (_dmatrixEigen(EV, D, A) != TCL_OK) {
    rc = TCL_ERROR;
  } else {
    double  *d =  D->matPA[0];
    double **a =  A->matPA;
    double **v = EV->matPA;
    double   t;
    int      n = A->n;
    int      i, j;

    for (i=0; i<n; i++) {
      for (j=0; j<n; j++)  a[i][j] = 0.0;
      a[i][i] = d[i];
    }
    for (i=0; i<n; i++) {
      for (j=0; j<i; j++) {
	t=v[j][i]; v[j][i] = v[i][j]; v[i][j] = t;
      }
    }
  }

  _dmatrixDestroy( D);
  return rc;
}

/* ------------------------------------------------------------------------
    Simdiag
   ------------------------------------------------------------------------ */

static DMatrix* dmatrixSimdiag( DMatrix* A, DMatrix* K, DMatrix* T, DMatrix* W,
			 double thresh, int iter)
{
  int dim;
  assert(A && K && T && W);

  /* ------ test dimensions and symmetry ----- */
  if (symTest(T,0.01) == TCL_ERROR  ||  symTest(W,0.01) == TCL_ERROR)
    return NULL;
  dim = T->m;
  dmatrixResize(A,dim,dim);
  dmatrixResize(K,dim,dim);

  /* ----- eigenvalues of W in W, eigenvector in K ----- */
  if (dmatrixEigenQR(W,K,thresh,iter) < 0) return NULL;
  if (!dmatrixClean(A,thresh)) {
    ERROR("Simdiag #1: not enough iterations %d\n",iter);
    return NULL;
  }
  
  /* ------ whitening ----- */
  dmatrixWhiten(W);
  
  dmatrixCovmul(A,K,T);            /* A = K T K' */
  dmatrixCovmul(T,W,A);            /* T = W A W' , transformed total scatter */
  dmatrixMul   (A,W,K);            /* A = W K    , Whiten * EV(Within)       */
                         /* transformed within scatter would be unity matrix */
  dmatrixCopy  (K,T);
  dmatrixCopy  (W,A);

  /* ----- new eigenvalues in K, eigenvector in T ----- */
  if (dmatrixEigenQR(K,T,(double)thresh,iter) < 0) return NULL;
  if (!dmatrixClean(K,thresh)) {
    ERROR("Simdiag #2: not enough iterations %d\n",iter);
    return NULL;
  }
  dmatrixMul   (A,T,W);            /* A = T W  ,EV(Total2)*Whiten*EV(Within) */

  /* ----- sorting ----- */
  sortEigen(K,A);

  return A;
}

static int dmatrixSimdiagItf( ClientData cd, int argc, char *argv[])
{
  DMatrix *A  = (DMatrix*)cd;
  DMatrix *T  = NULL;
  DMatrix *W  = NULL;
  DMatrix *K  = NULL;
  int     iter   = 50000;
  float   thresh = 1.0e-7;
  int     error  = 0;
  
  argc--;
  if (itfParseArgv( argv[0], &argc, argv+1, 0,
    "<matrix>",ARGV_CUSTOM,getDMatrixP,&K,    cd,"matrix with eigenvalues",
    "<matrix>",ARGV_CUSTOM,getDMatrix ,&T,    cd,"total scatter matrix",
    "<matrix>",ARGV_CUSTOM,getDMatrix ,&W,    cd,"within scatter matrix",
    "-iter",  ARGV_INT,    NULL,       &iter, cd,"max. number of iterations",
    "-thresh",ARGV_FLOAT,  NULL,       &thresh,cd,
		                     "threshold for max. non diagonal element",
    NULL) != TCL_OK) error = 1;
  else if (dmatrixSimdiag(A,K,T,W,(double)thresh,iter) == NULL) error = 1;

  if (T) dmatrixFree(T);
  if (W) dmatrixFree(W);
  if (error) return TCL_ERROR;
  return TCL_OK;
}

/* ------------------------------------------------------------------------
    SVD
    Singular Value Decomposition

    dmatrixSVD()
    Given a matrix A(mxn), this routines computes its singular value
    decomposition, A = U*W*V'. The matrix U replaces A on output. The
    diagonal matrix of singular values W(nxn) is output as (1xn). The
    matrix V (not the transpose V') is output as (nxn).
   ------------------------------------------------------------------------ */
static double dsqrarg;
#define DSQR(a) ((dsqrarg=(a)) == 0.0 ? 0.0 : dsqrarg*dsqrarg)

static double dmaxarg1,dmaxarg2;
#define DMAX(a,b) (dmaxarg1=(a),dmaxarg2=(b),(dmaxarg1) > (dmaxarg2) ?\
        (dmaxarg1) : (dmaxarg2))

#if 0
static double dminarg1,dminarg2;
#define DMIN(a,b) (dminarg1=(a),dminarg2=(b),(dminarg1) < (dminarg2) ?\
        (dminarg1) : (dminarg2))

static int imaxarg1,imaxarg2;
#define IMAX(a,b) (imaxarg1=(a),imaxarg2=(b),(imaxarg1) > (imaxarg2) ?\
        (imaxarg1) : (imaxarg2))
#endif

static int iminarg1,iminarg2;
#define IMIN(a,b) (iminarg1=(a),iminarg2=(b),(iminarg1) < (iminarg2) ?\
        (iminarg1) : (iminarg2))

#define SIGN(a,b) ((b) >= 0.0 ? fabs(a) : -fabs(a))

static double dpythag(double a, double b)
/* Computes sqrt(a^2 + b^2) without destructive underflow or overflow */
{
  double absa,absb;
  absa=fabs(a);
  absb=fabs(b);
  if (absa > absb) return absa*sqrt(1.0+DSQR(absb/absa));
  else return (absb == 0.0 ? 0.0 : absb*sqrt(1.0+DSQR(absa/absb)));
}

static void newDVector( int m, double** vecP)
{
  int     i;

  *vecP = (double*)malloc(m * sizeof(double));
  if (*vecP) for ( i = 0; i < m; i++) (*vecP)[i] = 0.0;
}

static void freeDVector( double* vec)
{
  if (vec) free(vec);
}

void dsvdcmp(double **a, int m, int n, double w[], double **v)
{
  /* this is nested extern declaration of `dpythag'
     Why? now it is a comment
     double dpythag(double a, double b);
     */
  int flag,i,its,j,jj,k,l=0,nm=0;
  double anorm,c,f,g,h,s,scale,x,y,z,*rv1;

  newDVector( n, &rv1);
  g=scale=anorm=0.0;
	
  for ( i = 0; i < n; i++) {

    l      = i+1;
    rv1[i] = scale*g;
    g      = s=scale=0.0;

    if (i < m) {
      for ( k=i; k<m; k++) scale += fabs(a[k][i]);
        if (scale) {
          for ( k=i; k<m;k++) {
            a[k][i] /= scale;
            s += a[k][i]*a[k][i];
          }
	  f =  a[i][i];
          g = -SIGN(sqrt(s),f);
	  h = f*g-s;
	  a[i][i]=f-g;
	  for (j=l;j<n;j++) {
	    for (s=0.0,k=i;k<m;k++) s += a[k][i]*a[k][j];
	    f=s/h;
	    for (k=i;k<m;k++) a[k][j] += f*a[k][i];
	  }
	  for (k=i;k<m;k++) a[k][i] *= scale;
	}
      }
      w[i]=scale *g;
      g=s=scale=0.0;
      if (i < m && i != n-1) {
        for (k=l;k<n;k++) scale += fabs(a[i][k]);
	if (scale) {
	  for (k=l;k<n;k++) {
	    a[i][k] /= scale;
	    s += a[i][k]*a[i][k];
	  }
	  f=a[i][l];
	  g = -SIGN(sqrt(s),f);
	  h=f*g-s;
          a[i][l]=f-g;
	  for (k=l;k<n;k++) rv1[k]=a[i][k]/h;
	  for (j=l;j<m;j++) {
	     for (s=0.0,k=l;k<n;k++) s += a[j][k]*a[i][k];
	     for (k=l;k<n;k++) a[j][k] += s*rv1[k];
	  }
	  for (k=l;k<n;k++) a[i][k] *= scale;
	}
      }
      anorm=DMAX(anorm,(fabs(w[i])+fabs(rv1[i])));
   }

   for (i=n-1;i>=0;i--) {
     if (i < n-1) {
       if (g) {
	 for (j=l;j<n;j++) v[j][i]=(a[i][j]/a[i][l])/g;
	 for (j=l;j<n;j++) {
	   for (s=0.0,k=l;k<n;k++) s += a[i][k]*v[k][j];
	   for (k=l;k<n;k++) v[k][j] += s*v[k][i];
	 }
       }
       for (j=l;j<n;j++) v[i][j]=v[j][i]=0.0;
     }
     v[i][i]=1.0;
     g=rv1[i];
     l=i;
   }
	
   for ( i = IMIN(m,n)-1;i >= 0;i--) {
     l=i+1;
     g=w[i];
     for ( j=l; j<n;j++) a[i][j]=0.0;
     if (g) {
       g=1.0/g;
       for (j=l;j<n;j++) {
          for (s=0.0,k=l;k<m;k++) s += a[k][i]*a[k][j];
	  f=(s/a[i][i])*g;
	  for (k=i;k<m;k++) a[k][j] += f*a[k][i];
       }
	for (j=i;j<m;j++) a[j][i] *= g;
     } else for (j=i;j<m;j++) a[j][i]=0.0;
     ++a[i][i];
   }

   for ( k = n-1; k >= 0;k--) {
     for (its=1;its<=30;its++) {
       flag=1;
       for (l=k;l>=0;l--) {
	 nm=l-1;
	 if ((double)(fabs(rv1[l])+anorm) == anorm) {
	    flag=0;
	    break;
	 }
	 if ((double)(fabs(w[nm])+anorm) == anorm) break;
       }
       if (flag) {
         c=0.0;
	 s=1.0;
	 for (i=l;i<=k;i++) {
	   f=s*rv1[i];
	   rv1[i]=c*rv1[i];
	   if ((double)(fabs(f)+anorm) == anorm) break;
	      g=w[i];
	      h=dpythag(f,g);
	      w[i]=h;
	      h=1.0/h;
	      c=g*h;
	      s = -f*h;
      	      for (j=0;j<m;j++) {
		y=a[j][nm];
		z=a[j][i];
		a[j][nm]=y*c+z*s;
		a[j][i]=z*c-y*s;
	      }
	    }
	  }
	  z=w[k];
	  if (l == k) {
	    if (z < 0.0) {
	      w[k] = -z;
	      for (j=0;j<n;j++) v[j][k] = -v[j][k];
	    }
	    break;
          }
	  if (its == 30) return;
                        /* nrerror("no convergence in 30 dsvdcmp iterations"); */
	  x=w[l];
          nm=k-1;
	  y=w[nm];
	  g=rv1[nm];
	  h=rv1[k];
	  f=((y-z)*(y+z)+(g-h)*(g+h))/(2.0*h*y);
	  g=dpythag(f,1.0);
          f=((x-z)*(x+z)+h*((y/(f+SIGN(g,f)))-h))/x;
	  c=s=1.0;
	  for (j=l;j<=nm;j++) {
	     i=j+1;
	     g=rv1[i];
	     y=w[i];
	     h=s*g;
	     g=c*g;
	     z=dpythag(f,h);
	     rv1[j]=z;
	     c=f/z;
	     s=h/z;
	     f=x*c+g*s;
	     g = g*c-x*s;
	     h=y*s;
	     y *= c;
	     for (jj=0;jj<n;jj++) {
	       x=v[jj][j];
	       z=v[jj][i];
	       v[jj][j]=x*c+z*s;
	       v[jj][i]=z*c-x*s;
	     }
             z=dpythag(f,h);
             w[j]=z;
             if (z) {
	       z=1.0/z;
               c=f*z;
	       s=h*z;
	     }
             f=c*g+s*y;
             x=c*y-s*g;
             for (jj=0;jj<m;jj++) {
	        y=a[jj][j];
	        z=a[jj][i];
		a[jj][j]=y*c+z*s;
		a[jj][i]=z*c-y*s;
	     }
	   }
	   rv1[l]=0.0;
	   rv1[k]=f;
	   w[k]=x;
         }
  }
  freeDVector(rv1);
}

int dsvclean( double* w, int n)
{
  int i;
  double wmax = 0, wmin;

  for ( i = 0; i < n; i++) if ( w[i] > wmax) wmax = w[i];
  wmin = wmax * 1.0e-19;
  for ( i = n - 1; i >= 0; i--) if ( w[i] < wmin) { w[i] = 0.0; n--; }
  return n; 
}

void dsvbksb(double **u, double w[], double **v, int m, int n, double b[], double x[])
{
  int jj,j,i;
  double s,*tmp;

  newDVector(n,&tmp);

  for (j=0;j<n;j++) {
    s=0.0;
    if (w[j]) {
       for (i=0;i<m;i++) s += u[i][j]*b[i];
       s /= w[j];
    }
    tmp[j]=s;
  }
  for (j=0;j<n;j++) {
    s=0.0;
    for (jj=0;jj<n;jj++) s += v[j][jj]*tmp[jj];
    x[j]=s;
  }
  freeDVector(tmp);
}

DMatrix*  dmatrixSVD(DMatrix *A, DMatrix *W, DMatrix *V)
{
  int m,n;
  assert(A && W && V);

  m = A->m;
  n = A->n;
  dmatrixResize(W,1,n);
  dmatrixResize(V,n,n);
  dsvdcmp( A->matPA, m, n, W->matPA[0], V->matPA);
  return A;
}

int dmatrixSVDItf( ClientData cd, int argc, char *argv[])
{
  DMatrix *A  = (DMatrix*)cd;
  DMatrix *W  = NULL;
  DMatrix *V  = NULL;
  int  clean  = 1;

  argc--;
  if (itfParseArgv( argv[0], &argc, argv+1, 0,
    "<matrix>", ARGV_CUSTOM, getDMatrixP, &W,   cd,
		                            "matrix W to hold singular values",
    "<matrix>", ARGV_CUSTOM, getDMatrixP, &V,   cd,
		                         "matrix V to hold basis of nullspace",
    "-clean",   ARGV_INT,    NULL,        &clean,cd,
		                                    "clean up singular values",
    NULL) != TCL_OK) return TCL_ERROR;

  dmatrixSVD( A, W, V);
  if (clean) dsvclean(W->matPA[0],W->n);
  return TCL_OK;
}

/* ------------------------------------------------------------------------
    Inv
    matrix inversion

    Using the singular value decomposition this routines returns B the
    inverse of a square matrix A or at least the best choice in a
    least-squares sense. Matrix A won't be changed unless B=A.
    A = U *  W   *  V'     result of the SVD
    B = V * W^-1 *  U'     inverse (note: W diagonal, U V orthonormal)
    B = V * Z              Z' = U * W^-1
    ------------------------------------------------------------------------ */
DMatrix* dmatrixInv(DMatrix *B,DMatrix *A)
{
  int n,i,j;
  DMatrix *U, *V, *W;
  assert(B && A);

  /* --------------------------------
     check dimension
     -------------------------------- */
  n = A->n;
  if (A->m != n) {
    ERROR("Inv: expected square matrix not (%dx%d).\n",A->m,n);
    return NULL;
  }
  if (n == 0) {
    return dmatrixCopy(B,A);
  }

  /* --------------------------------
     prepare matrices
     -------------------------------- */
  U  = dmatrixCreate(n,n);
  W  = dmatrixCreate(1,n);
  V  = dmatrixCreate(n,n);
  dmatrixCopy(U,A);
  dmatrixResize(B,n,n);
  
  /* --------------------------------
     calculate inverse
     -------------------------------- */
  dmatrixSVD(U, W, V);
  dsvclean(W->matPA[0],W->n);
  for (j=0;j<n;j++) {
    double w = W->matPA[0][j];
    if (w) for (i=0;i<n;i++) U->matPA[i][j] /= w;
  }
  dmatrixMulot(B,V,U);
  
  /* --------------------------------
     free and return
     -------------------------------- */
  dmatrixFree(U);
  dmatrixFree(W);
  dmatrixFree(V);
  return B;
}

int dmatrixInvItf( ClientData cd, int argc, char *argv[])
{
  DMatrix *B = (DMatrix*)cd;
  DMatrix *A = NULL;
  int ret    = TCL_OK;

  argc--;
  if (itfParseArgv( argv[0], &argc, argv+1, 0,
    "<matrix>", ARGV_CUSTOM, getDMatrix, &A, cd, "",
    NULL) != TCL_OK) return TCL_ERROR;

  if (dmatrixInv(B,A) == NULL) ret = TCL_ERROR;
  dmatrixFree(A);
  return ret;
}
    
/* ========================================================================
    LU Decomposition
    Given a square Fmatrix a, this routine replaces it by the LU 
    decomposition of a rowwise permutation of itself.
    indx is a FVector that records the row permutation effected by the partial pivoting.
    It should have the size of the FMatrix. If not, it will be resized.
    d is output as +-1 depending on whether the number of row interchanges 
    was even or odd. This routine can be used to solve linear equations, 
    invert a matrix, or compute a determinant.
    This is an adapted version from 'numerical recipes in C'.
   ======================================================================== */
void fludcmp(FMatrix *a, FVector *indx, float *d)
{
  int i, imax=0, j, k, n;
  float big, dum, sum, temp;
  FVector *vv;
  
  n = a->n;
  if (a->n != a->m) {
    SERROR("fludcmp assert error: called with non square matrix\n");
    return;
  }
  vv = fvectorCreate(n);
  fvectorResize(indx, n);
  *d = 1.0;

  /* loop over rows to get implicit scaling information */
  for (i=0; i<n; i++) {
    big=0.0;
    for (j=0; j<n; j++) {
      if ((temp=fabs(a->matPA[i][j])) > big) big=temp;
    }
    if (big == 0.0) { fprintf(stderr,"Singular matrix in ludcmp!\n"); return;}
    vv->vecA[i] = 1.0 / big;
  }
  /* this is Crout's algorithm */
  for (j=0; j<n; j++) {
    for (i=0; i<j; i++) {
      sum = a->matPA[i][j];
      for (k=0; k<i; k++) sum -= a->matPA[i][k] * a->matPA[k][j];
      a->matPA[i][j] = sum;
    }
    big=0.0;
    for (i=j; i<n; i++) {
      sum=a->matPA[i][j];
      for (k=0; k<j; k++) {
	sum -= a->matPA[i][k] * a->matPA[k][j];
      }
      a->matPA[i][j] = sum;
      if ( (dum=vv->vecA[i] * fabs(sum)) >= big ) {
	/* better pivot than best so far! */
	big = dum;
	imax = i;
      }
    }
    if (j != imax) {
      /* now we have to interchange rows */
      for (k=0; k<n; k++) {
	dum = a->matPA[imax][k];
	a->matPA[imax][k] = a->matPA[j][k];
	a->matPA[j][k] = dum;
      }
      *d = -(*d); /* keep track of parity */
      vv->vecA[imax] = vv->vecA[j];
    }
    indx->vecA[j] = imax;
    if (a->matPA[j][j] == 0.0) a->matPA[j][j] = 1.0e-20; /* well, but the matrix is singular! */
    if (j != n) {
      dum = 1.0 / a->matPA[j][j] ;
      for(i=j+1; i<n; i++) a->matPA[i][j] *= dum;
    }
  } /* next column */
  fvectorFree(vv); 
}
 
   
/* ========================================================================
    fmatrixDet
    Compute the determinant of a matrix
   ======================================================================== */
float fmatrixDet(FMatrix *A) {
  FMatrix *a;
  FVector *dummy;
  float d;
  int i;
  if (A->m != A->n) {
    SERROR("fmatrixDet: non-square matrix\n");
    return(0.0);
  }
	   
  a = fmatrixCreate(A->m, A->n);
  fmatrixCopy(a,A);
  dummy = fvectorCreate(A->n);
  fludcmp(a, dummy, &d);
  fvectorFree(dummy);
  for (i=0; i < A->n; i++) { d *= a->matPA[i][i]; }

  fmatrixFree(a);
  return(d);
}

/* -------------------------------------------------------------------- */
/*  fmatrixExtDet  extended version: to support OFS stuff               */                  
/* -------------------------------------------------------------------- */

int  fmatrixExtDet (FMatrix *A, double *logdet, double *sign) {

  DMatrix        *D;
  int            k;
  double         d,sum,val;

  assert (A);

  if (A->m != A->n) {
    ERROR("A must be squared\n");
    return TCL_ERROR;
  }

  D = dmatrixCreate(A->m,A->n);
  dmatrixFMatrix(D,A);

  ludcmp(D->matPA,A->m,&d);

  sum = 0.0;
  for (k=0; k<A->m; k++) {
    if ((val = D->matPA[k][k]) < 0.0) {
      val = -1.0*val;
      d *= -1.0;
    }
    sum += log(val);
  }

  *logdet = sum;
  *sign   = d;

  dmatrixFree(D);
  free(D);

  return TCL_OK;
}

/* -----------------------------------------------------------------------
    fmatrixDeterminantItf
   ----------------------------------------------------------------------- */
static int fmatrixDeterminantItf (ClientData cd, int argc, char *argv[]) {
  FMatrix *A = (FMatrix *)cd;
  int   ac = argc - 1;
  float det;
  char* format = "%g";

  if ( itfParseArgv( argv[0], &ac, argv+1, 0, 
       "-format", ARGV_STRING, NULL, &format, cd, "format string",
       NULL) != TCL_OK) return TCL_ERROR;
  det = fmatrixDet(A);
  itfAppendResult(format, det);
  return TCL_OK;
}
    


/* ========================================================================
    Convert types
   ======================================================================== */

/* ------------------------------------------------------------------------
    FMatrix (to DMatrix)
   ------------------------------------------------------------------------ */

DMatrix* dmatrixFMatrix( DMatrix* A, FMatrix* B)
{
   int i,j;

   assert(A && B);
   if ( A->n != B->n || A->m != B->m) dmatrixResize(A,B->m,B->n); 

   for ( i=0; i<A->m; i++) for ( j=0; j<A->n; j++)
	 A->matPA[i][j] = (float)B->matPA[i][j];
   A->count = B->count;
   return A;
}

static int dmatrixFMatrixItf( ClientData cd, int argc, char *argv[])
{
  DMatrix* mat = (DMatrix*)cd;
  FMatrix* tmp;

  if ( argc != 2) {
    ERROR("'copy' expects 1 parameter.\n");
    return TCL_ERROR;
  }
  if ((tmp = fmatrixGetItf(argv[1]))) {
    dmatrixFMatrix( mat, tmp);
    fmatrixFree( tmp);
    return TCL_OK;
  }
  return TCL_ERROR;
}

/* ------------------------------------------------------------------------
    DMatrix (to FMatrix)
   ------------------------------------------------------------------------ */

FMatrix* fmatrixDMatrix( FMatrix* A, DMatrix* B)
{
   int i,j;

   assert(A && B);
   if ( A->n != B->n || A->m != B->m) fmatrixResize(A,B->m,B->n); 

   for ( i=0; i<A->m; i++) for ( j=0; j<A->n; j++)
	 A->matPA[i][j] = (float)B->matPA[i][j];
   A->count = B->count;
   return A;
}

static int fmatrixDMatrixItf( ClientData cd, int argc, char *argv[])
{
  FMatrix* mat = (FMatrix*)cd;
  DMatrix* tmp;

  if ( argc != 2) {
    ERROR("'copy' expects 1 parameter.\n");
    return TCL_ERROR;
  }
  if ((tmp = dmatrixGetItf(argv[1]))) {
    fmatrixDMatrix( mat, tmp);
    dmatrixFree( tmp);
    return TCL_OK;
  }
  return TCL_ERROR;
}


/* ------------------------------------------------------------------------
    FastIca: from Hyvarinen "Fast and Robust Fixed-Point Algorithms for 
    Independent Component Analysis"
    uses "general purpose" contrast function
    G(u)  = ln cosh(u)
    g(u)  = tanh(u)
    g'(u) = 1 - (tanh(u))^2
    Assumes prewhitened data; can use either deflation or symmetric approach.
    B is the basis matrix; M the whitening matrix. 
    A is the mixing matrix A = M^-1 * B
    W is the unmixing matrix W = M * B'
   ------------------------------------------------------------------------ */

int dmatrixNegentropies(DMatrix* data, DMatrix* B, DMatrix* neg) {
  int      frameN = data->m;
  int      dimN   = data->n;
  int      i, j;
  double   EGy  = 0.0;
  double   EGgy = 0.374576;
  double   sum  = 0.0;
  double   sumS = 0.0;
  double   mean = 0.0;
  double   dev  = 0.0;
  DMatrix* X    = dmatrixCreate(frameN, dimN);

  dmatrixInitConst(neg, 0.0);
  X = dmatrixMulot(X, data, B);

  for (i = 0; i < dimN; i++) {
    sum  = 0.0;
    sumS = 0.0;
    for (j = 0; j < frameN; j++) {
      sum += X->matPA[j][i];
      sumS += X->matPA[j][i] * X->matPA[j][i];
    }
    mean = sum / (double) frameN;
    dev = sqrt(((1.0 / (double) (frameN - 1)) * sumS) - (mean * mean));
    for (j = 0; j < frameN; j++) {
      X->matPA[j][i] = (X->matPA[j][i] - mean) / dev;
    }

    sum = 0.0;
    for (j = 0; j < frameN; j++) {
      sum += log(cosh(X->matPA[j][i]));
    }
    EGy = sum / (double) frameN;
    neg->matPA[0][i] = (EGy - EGgy) * (EGy - EGgy);
  }

  dmatrixFree(X);
  return TCL_OK;
}

#if 0
int dmatrixFastIcaSymm(DMatrix* data, DMatrix* B, DMatrix* A, DMatrix* W, 
		       DMatrix* M, DMatrix* neg, double eps, int maxIterN) {
  int      frameN    = data->m;
  int      dimN      = data->n;
  int      dimX      = 0;
  int      iterX     = 0;
  int      i, j;
  double   minAbsCos = 0.0;
  double   minAbsCos2 = 0.0;
  DMatrix* BOld      = dmatrixCreate(dimN, dimN);
  DMatrix* BOld2     = dmatrixCreate(dimN, dimN);
  DMatrix* BtBOld    = dmatrixCreate(dimN, dimN);
  DMatrix* BtBOld2   = dmatrixCreate(dimN, dimN);
  DMatrix* MI        = dmatrixCreate(dimN, dimN);
  
  dmatrixInitConst(A, 0.0);
  dmatrixInitRand(B, 1.0);
  dmatrixInitConst(BOld, 0.0);
  dmatrixInitConst(BOld2, 0.0);

  MI = dmatrixInv(M);

  for (i = 0; i < dimN; i++) {
    for (j = 0; j < dimN; j++) {
      B->matPA[i][j] = abs(B->matPA[i][j]) - 0.5;
    }
  }

  /* orthogonalize B */

  /* check for termination */
  BtBOld  = dmatrixMulot(BtBOld, BOld, B);
  BtBOld2 = dmatrixMulot(BtBOld2, BOld2, B);
  minAbsCos = 9e99;
  minAbsCos2 = 9e99;
  for (i = 0; i < dimN; i++) {
    if (abs(BtBOld->matPA[i][i]) < minAbsCos) {
      minAbsCos = abs(BtBOld->matPA[i][i]);
    }
    if (abs(BtBOld2->matPA[i][i]) < minAbsCos2) {
      minAbsCos2 = abs(BtBOld->matPA[i][i]);
    }
  }
  
  if (1 - minAbsCos < epsilon) {
    fprintf(stderr, "Converged [0 steps]\n");
    A = dmatrixMul(A, MI, B);
  }

  dmatrixCopy(BOld2, BOld);
  dmatrixCopy(BOld, B);

  for (iterX = 0; iterX < maxIterN; iterX++) {
    W = dmatrixMulot(W, M, B);
    



  }

  dmatrixFree(BOld);
  dmatrixFree(BOld2);
  dmatrixFree(BtBOld);
  dmatrixFree(BtBOld2);
  return TCL_OK;
}
#endif

int dmatrixFastIcaDefl(DMatrix* data, DMatrix* B, DMatrix* A, DMatrix* W, 
		       DMatrix* M, DMatrix* neg, double eps, int maxIterN) {
  int      frameN    = data->m;
  int      dimN      = data->n;
  int      dimX      = 0;
  int      iterX     = 0;
  int      i;
  double   norm      = 0.0;
  double   norm_s    = 0.0;
  double   norm_d    = 0.0;
  double   sum       = 0.0;
  DMatrix* w         = dmatrixCreate(1, dimN);
  DMatrix* a         = dmatrixCreate(1, dimN);
  DMatrix* wr        = dmatrixCreate(1, dimN);
  DMatrix* BTw       = dmatrixCreate(1, dimN);
  DMatrix* BBTw      = dmatrixCreate(1, dimN);
  DMatrix* wOld      = dmatrixCreate(1, dimN);
  DMatrix* wOld2     = dmatrixCreate(1, dimN);
  DMatrix* wSum      = dmatrixCreate(1, dimN);
  DMatrix* wDiff     = dmatrixCreate(1, dimN);
  #if 0
  DMatrix* MI        = dmatrixCreate(dimN, dimN);
  #endif
  DMatrix* hypTan    = dmatrixCreate(1, frameN);
  DMatrix* dHypTan   = dmatrixCreate(1, frameN);
  DMatrix* dHypTanT  = dmatrixCreate(frameN, 1);
  DMatrix* wDHypTanT = dmatrixCreate(1, dimN);
  DMatrix* XHypTan   = dmatrixCreate(1, dimN);

  dmatrixResize(B,   dimN, dimN);
  dmatrixResize(A,   dimN, dimN);
  dmatrixResize(W,   dimN, dimN);
  dmatrixResize(neg, 1,    dimN);

  #if 0
  dmatrixInv(MI, M);
  #endif

  dmatrixInitConst(B, 0.0);

  for (dimX = 0; dimX < dimN; dimX++) {

    fprintf(stderr, "FastIca: Component %d", dimX);

    /* initialize */
    dmatrixInitConst(wOld, 0.0);
    dmatrixInitConst(wOld2, 0.0);
    dmatrixInitRandom(w, 1.0);
    for (i = 0; i < dimN; i++) {
      w->matPA[0][i] = abs(w->matPA[0][i]) - 0.5;
    }

    /* project w onto space orthogonal to other basis vectors */
    BTw  = dmatrixMulot(BTw, w, B);
    BBTw = dmatrixMul(BBTw, BTw, B);
    w    = dmatrixAdd(w, 1.0, w, -1.0, BBTw);

    /* normalize */
    norm = 0.0;
    for (i = 0; i < dimN; i++) {
      norm += w->matPA[0][i] * w->matPA[0][i];
    }
    norm = sqrt(norm);
    for (i = 0; i < dimN; i++) {
      w->matPA[0][i] /= norm;
    }

    for (iterX = 0; iterX <= maxIterN; iterX++) {

      if (iterX == maxIterN) {
	fprintf(stderr, "Failed to converge\n");
	dmatrixFree(w);
	dmatrixFree(a);
	dmatrixFree(wr);
	dmatrixFree(BTw);
	dmatrixFree(BBTw);
	dmatrixFree(wOld);
	dmatrixFree(wOld2);
	dmatrixFree(wSum);
	dmatrixFree(wDiff);
	#if 0
	dmatrixFree(MI);
	#endif
	dmatrixFree(hypTan);
	dmatrixFree(dHypTan);
	dmatrixFree(dHypTanT);
	dmatrixFree(wDHypTanT);
	dmatrixFree(XHypTan);
	return TCL_ERROR;
      }

      fprintf(stderr, ".");

      /* project w onto space orthogonal to other basis vectors */
      BTw  = dmatrixMulot(BTw, w, B);
      BBTw = dmatrixMul(BBTw, BTw, B);
      w    = dmatrixAdd(w, 1.0, w, -1.0, BBTw);
      
      /* normalize */
      norm = 0.0;
      for (i = 0; i < dimN; i++) {
	norm += w->matPA[0][i] * w->matPA[0][i];
      }
      norm = sqrt(norm);
      for (i = 0; i < dimN; i++) {
	w->matPA[0][i] /= norm;
      }
      
      /* test for termination condition */
      norm_s = 0.0;
      norm_d = 0.0;
      for (i = 0; i < dimN; i++) {
	wSum->matPA[0][i] = w->matPA[0][i] + wOld->matPA[0][i];
	wDiff->matPA[0][i] = w->matPA[0][i] - wOld->matPA[0][i];
	norm_s += wSum->matPA[0][i] * wSum->matPA[0][i];
	norm_d += wDiff->matPA[0][i] * wDiff->matPA[0][i];
      }
      norm_s = sqrt(norm_s);
      norm_d = sqrt(norm_d);

      if (norm_s < eps || norm_d < eps) {

	fprintf(stderr, "Converged [%d steps]\n", iterX);

	/* copy the vector into the basis matrix */
	for (i = 0; i < dimN; i++) {
	  B->matPA[dimX][i] = w->matPA[0][i];
	}

	/* calculate the de-whitening vector */
	#if 0
	a = dmatrixMulot(a, MI, w);
	for (i = 0; i < dimN; i++) {
	  A->matPA[dimX][i] = a->matPA[0][i];
	}
	#endif

	/* calculate the ICA filter */
	#if 0
	wr = dmatrixMulot(wr, M, w);
	for (i = 0; i < dimN; i++) {
	  W->matPA[i][dimX] = wr->matPA[0][i];
	}
	#endif

	iterX = maxIterN;
      } else {
	
	wOld2 = dmatrixCopy(wOld2, wOld);
	wOld = dmatrixCopy(wOld, w);

	/* apply learning rule */
	hypTan = dmatrixMulot(hypTan, w, data);
	for (i = 0; i < frameN; i++) {
	  hypTan->matPA[0][i] = tanh(hypTan->matPA[0][i]);
	  dHypTan->matPA[0][i] = (1.0 - (hypTan->matPA[0][i] * 
					 hypTan->matPA[0][i]));
	}
	dHypTanT = dmatrixTrans(dHypTanT, dHypTan);
	sum      = 0.0;
	for (i = 0; i < frameN; i++) {
	  sum += dHypTanT->matPA[i][0];
	}
	
	dmatrixInitConst(wDHypTanT, 0.0);
	for (i = 0; i < dimN; i++) {
	  wDHypTanT->matPA[0][i] = sum * w->matPA[0][i];
	}

	XHypTan = dmatrixMul(XHypTan, hypTan,data);

	norm = 0.0;
	for (i = 0; i < dimN; i++) {
	  w->matPA[0][i] = (XHypTan->matPA[0][i] - wDHypTanT->matPA[0][i]) / 
	    frameN;
	  norm += w->matPA[0][i] * w->matPA[0][i];
	}
	norm = sqrt(norm);
	for (i = 0; i < dimN; i++) {
	  w->matPA[0][i] /= norm;
	}
      }
    }
  }

  /* calculate negentropies and sort */
  dmatrixNegentropies(data, B, neg);

  dmatrixFree(w);
  dmatrixFree(a);
  dmatrixFree(wr);
  dmatrixFree(BTw);
  dmatrixFree(BBTw);
  dmatrixFree(wOld);
  dmatrixFree(wOld2);
  dmatrixFree(wSum);
  dmatrixFree(wDiff);
  #if 0
  dmatrixFree(MI);
  #endif
  dmatrixFree(hypTan);
  dmatrixFree(dHypTan);
  dmatrixFree(dHypTanT);
  dmatrixFree(wDHypTanT);
  dmatrixFree(XHypTan);
  return TCL_OK;
}

int dmatrixFastIcaSymm(DMatrix* data, DMatrix* B, DMatrix* A, DMatrix* W, 
		       DMatrix* M, DMatrix* neg, double eps, int maxIterN) {
  return TCL_OK;
}

int dmatrixFastIcaItf(ClientData cd, int argc, char* argv[]) {
  DMatrix* data     = (DMatrix*) cd;
  DMatrix* B        = NULL;         /* ICA basis matrix */
  DMatrix* A        = NULL;         /* Mixing matrix */
  DMatrix* W        = NULL;         /* Unmixing matrix */
  DMatrix* M        = NULL;         /* Whitening matrix */
  DMatrix* neg      = NULL;         /* per-dim contribs to negentropy */
  double   eps      = 0.1;          /* convergence threshold */
  char*    meth     = "deflation";  /* method: 'symmetric' or 'deflation' */
  int      maxIterN = 25;           /* max # of iterations */
  double   seed     = 0.023;        /* rng seed */

  argc--;
  if (itfParseArgv(argv[0], &argc, argv + 1, 0, 
		   "<B>", ARGV_CUSTOM, getDMatrixP, &B, cd, "ICA Basis matrix",
		   "<M>", ARGV_CUSTOM, getDMatrixP, &M, cd, "Whitening matrix",
		   "<A>", ARGV_CUSTOM, getDMatrixP, &A, cd, "Mixing matrix; = BM^-1", 
		   "<W>", ARGV_CUSTOM, getDMatrixP, &W, cd, "Unmixing matrix = MB'",
		   "<neg>", ARGV_CUSTOM, getDMatrixP, &neg, cd, "Per-feature contributions to total negentropy", 
		   "-eps", ARGV_DOUBLE, NULL, &eps, cd, "Convergence threshold", 
		   "-meth", ARGV_STRING, NULL, &meth, cd, "Method: deflation|symmetric", 
		   "-maxIterN", ARGV_INT, NULL, &maxIterN, cd, "Max # of iterations", 
		   "-seed", ARGV_DOUBLE, NULL, &seed, cd, "rng seed",
		   NULL) != TCL_OK) return TCL_ERROR;
  
  chaosRandomInit(seed);
  
  if (!strcmp(meth, "deflation")) {
    dmatrixFastIcaDefl(data, B, A, W, M, neg, eps, maxIterN);
  } else if (!strcmp(meth, "symmetric")) {
    dmatrixFastIcaSymm(data, B, A, W, M, neg, eps, maxIterN);
  } else {
    fprintf(stderr, "FastIca: Unknown method %s\n", meth);
  }

  return TCL_OK;
}


/* ------------------------------------------------------------------------
    Principal Component Analysis
    input: covariance matrix 
   ------------------------------------------------------------------------ */

int dmatrixCovar(DMatrix* input, DMatrix* mean, DMatrix* covar) {
  int dimN   = input->n;
  int frameN = input->m;
  int i, j, k;

  dmatrixResize(covar, dimN, dimN);
  dmatrixResize(mean, 1, dimN);
  dmatrixInitConst(covar, 0.0);

  for (i = 0; i < frameN; i++) {
    for (j = 0; j < dimN; j++) {
      mean->matPA[0][j] += input->matPA[i][j];
      for (k = 0; k < dimN; k++) {
	covar->matPA[j][k] += input->matPA[i][j] * input->matPA[i][k];
      }
    }
  }

  for (i = 0; i < dimN; i++) {
    mean->matPA[0][i] /= (double) frameN;
  }

  for (i = 0; i < dimN; i++) {
    for (j = 0; j < dimN; j++) {
      covar->matPA[i][j] = (covar->matPA[i][j] / (double) frameN) - 
	(mean->matPA[0][i] * mean->matPA[0][j]);
    }
  }
  return TCL_OK;
}

int dmatrixCovarItf(ClientData cd, int argc, char* argv[]) {
  DMatrix* data  = (DMatrix*) cd;
  DMatrix* mean  = NULL;
  DMatrix* covar = NULL;
  

  argc--;
  if (itfParseArgv(argv[0], &argc, argv + 1, 0, 
		   "<covar>", ARGV_CUSTOM, getDMatrixP, &covar, cd, "matrix to hold covars", 
		   "<mean>", ARGV_CUSTOM, getDMatrixP, &mean, cd, "matrix to hold means", 
		   NULL) != TCL_OK) return TCL_ERROR;

  return dmatrixCovar(data, mean, covar);
}

int dmatrixPcaEigen(DMatrix* input, DMatrix* basis, DMatrix* eigenVal, 
		    DMatrix* whiten) {
  DMatrix* eigenVec = dmatrixCreate(input->n, input->n);
  int      dimN     = input->n;
  int      i;

  dmatrixCopy(eigenVal, input);
  dmatrixResize(basis, dimN, dimN);
  dmatrixResize(whiten, dimN, dimN);

  /* ------------------------------------ */
  /* compute eigenvalues and eigenvectors */
  /* ------------------------------------ */

  dmatrixEigen(eigenVal, eigenVec, 1.0e-7, 50000);
  dmatrixClean(eigenVal, 1.0e-7);
  sortEigen(eigenVal, eigenVec);

  /* ---------------------- */
  /* build whitening matrix */
  /* ---------------------- */

  for (i = 0; i < dimN; i++) {
    whiten->matPA[i][i] = sqrt(1.0 / eigenVal->matPA[i][i]);
  }
  
  /* ---------------------- */
  /* transpose basis matrix */
  /* ---------------------- */

  basis = dmatrixTrans(basis, eigenVec);

  /* ------- */
  /* cleanup */
  /* ------- */

  dmatrixFree(eigenVec);
  return TCL_OK;
}

int dmatrixPcaSvd(DMatrix* input, DMatrix* basis, DMatrix* eigenVal, 
		  DMatrix* whiten) {


  DMatrix* eigen  = dmatrixCreate(1, input->n);
  int      dimN   = input->n;
  int      i;

  dmatrixResize(basis, dimN, dimN);
  dmatrixResize(whiten, dimN, dimN);
  dmatrixResize(eigenVal, dimN, dimN);

  /* ------------------------------------------ */
  /* compute eigenvalues and eigenvectors, sort */
  /* ------------------------------------------ */

  dmatrixSVD(input, eigen, basis);

  for (i = 0; i < dimN; i++) {
    eigenVal->matPA[i][i] = eigen->matPA[0][i];
  }
  dmatrixFree(eigen);
  sortEigen(eigenVal, basis);
  
  /* ---------------------- */
  /* build whitening matrix */
  /* ---------------------- */

  whiten = dmatrixCopy(whiten, eigenVal);
  for (i = 0; i < dimN; i++) {
    whiten->matPA[i][i] = sqrt(1.0 / whiten->matPA[i][i]);
  }

  return TCL_OK;
}

int dmatrixPcaItf(ClientData cd, int argc, char* argv[]) {
  DMatrix* input    = (DMatrix*) cd;
  DMatrix* basis    = NULL;
  DMatrix* eigenVal = NULL;
  DMatrix* whiten   = NULL;
  int      useSvd   = 0;

  argc--;
  if (itfParseArgv(argv[0], &argc, argv + 1, 0,
		   "<basis>", ARGV_CUSTOM, getDMatrixP, &basis, cd, "basis functions", 
		   "<eigenVal>", ARGV_CUSTOM, getDMatrixP, &eigenVal, cd, "eigenvalues", 
		   "<whiten>", ARGV_CUSTOM, getDMatrixP, &whiten, cd, "whitening matrix", 
		   "-useSvd", ARGV_INT, NULL, &useSvd, cd, "use svd? [alt eigen]",
		   NULL) != TCL_OK) return TCL_ERROR;

  if (useSvd) {
    dmatrixPcaSvd(input, basis, eigenVal, whiten);
  } else {
    dmatrixPcaEigen(input, basis, eigenVal, whiten);
  } 
  return TCL_OK;
}




/* ========================================================================
    Type Declaration
   ======================================================================== */

Method svectorMethod[] = 
{ 
  { "append", svectorAppendItf, "appends two svector" }, 
  { "puts",   svectorPutsItf,   "print vector as TCL list" }, 
  { "set",    svectorSetItf,    "set single coefficient" }, 
  { "resize", svectorResizeItf, "resize vector" }, 
  { "copy",   svectorCopyItf,   "copy vector" },
  { ":=",     svectorCopyItf,   "assign vector (equiv. to 'copy')" },
  { "swap",   svectorSwapItf,   "swap byte order of short vector values" },
  { "lin",    svectorLinItf,    "a * vector + b" },
  { "add",    svectorAddItf,    "a * vectorA + b * vectorB" },
  { "minmax", svectorMinmaxItf, "gives minimum and maximum" },
  { "power",  svectorPowerItf,  "gives the power value" },
  { "mean",   svectorMeanItf,   "gives the mean value" },
  { "histogram",svectorHistogramItf, "histogram" },
  { "display",svectorDisplayItf,"display vector" },
  { "mul",    svectorMulItf,    "vector multiplication" },
  { NULL,     NULL, NULL } 
};

TypeInfo svectorInfo = { "SVector", 0, -1, svectorMethod, 
                          svectorCreateItf, svectorFreeItf, NULL, 
			  NULL, itfTypeCntlDefaultNoLink,
		         "Vector of short values\n" } ;

Method imatrixMethod[] = 
{ 
  { "puts",   imatrixPutsItf,   "print matrix contents as TCL list" }, 
  { "resize", imatrixResizeItf, "resize matrix" }, 
  { "clear",  imatrixClearItf,  "set all matrix values to 0" }, 
  { "copy",   imatrixCopyItf,   "copy matrix" },
  { ":=",     imatrixCopyItf,   "assign matrix (equiv. to 'copy')" },
  { "get",    imatrixGetValItf, "get a single entry from a matrix" },
  { "set",    imatrixSetValItf, "set a single entry in a matrix" },
  { "bload",  imatrixBLoadItf,  "load matrix from binary file" },
  { "bsave",  imatrixBSaveItf,  "save matrix to binary file" },
  { NULL,     NULL, NULL } 
};

TypeInfo imatrixInfo = { "IMatrix", 0, -1, imatrixMethod, 
                          imatrixCreateItf, imatrixFreeItf, imatrixConfigureItf,
			  NULL, itfTypeCntlDefaultNoLink,
		         "Matrix of integer values\n" } ;
  
Method cmatrixMethod[] = 
{ 
  { "puts",   cmatrixPutsItf,   "print matrix contents as TCL list" }, 
  { NULL,     NULL, NULL } 
};

TypeInfo cmatrixInfo = { "CMatrix", 0, -1, cmatrixMethod, 
                          cmatrixCreateItf, cmatrixFreeItf, cmatrixConfigureItf,
			  NULL, itfTypeCntlDefaultNoLink,
		         "Matrix of char values\n" } ;
  
Method fvectorMethod[] = 
{ 
  { "puts",   fvectorPutsItf,   "print vector as TCL list" }, 
  { "resize", fvectorResizeItf, "resize vector" }, 
  { "copy",   fvectorCopyItf,   "copy vector" },
  { ":=",     fvectorCopyItf,   "assign vector (equiv. to 'copy')" },
  { "bload",  fvectorBLoadItf,  "load vector from binary file" },
  { "bsave",  fvectorBSaveItf,  "save vector to binary file" },
  { "norm",   fvectorNormItf,   "norm of the vector" },
  { "add",    fvectorAddItf,    "add two vectors" },
  { NULL,     NULL, NULL } 
};

TypeInfo fvectorInfo = { "FVector", 0, -1, fvectorMethod, 
                          fvectorCreateItf,fvectorFreeItf,fvectorConfigureItf,
			  NULL, itfTypeCntlDefaultNoLink,
		         "Vector of float values\n" } ;

Method fmatrixMethod[] = 
{ 
  { "puts",     fmatrixPutsItf,   "print matrix contents as TCL list" }, 
  { "resize",   fmatrixResizeItf, "resize matrix" }, 
  { "modulo",   fmatrixModuloItf, "modulo matrix" }, 
  { "copy",     fmatrixCopyItf,   "copy matrix" },
  { ":=",       fmatrixCopyItf,   "assign matrix (equiv. to 'copy')" },
  { "get",      fmatrixGetValItf, "get a single entry from a matrix" },
  { "set",      fmatrixSetValItf, "set a single entry in a matrix" },
  { "window",   fmatrixWindowItf, "window matrix (into other matrix at offset)" },
  { "square",   fmatrixSquareItf, "convert a row to a square matrix or vice-versa" },
  { "DMatrix",  fmatrixDMatrixItf,"convert from a DMatrix" },
  { "clear",    fmatrixClearItf,  "set all matrix values to 0" }, 
  { "load",     fmatrixLoadItf,   "load matrix from file" },
  { "mload",    fmatrixMLoadItf,  "load matrix from Matlab file" },
  { "bload",    fmatrixBLoadItf,  "load matrix from binary file" },
  { "bsave",    fmatrixBSaveItf,  "save matrix to binary file" },
  { "isave",    fmatrixISaveItf,  "save matrix to IBM file" },
  { "iload",    fmatrixILoadItf,  "load matrix from IBM file" },
  { "cload",    fmatrixCLoadItf,  "load matrix from compressed file" },
  { "csave",    fmatrixCSaveItf,  "save matrix to compressed file" },
  { "bappend",  fmatrixBAppendItf,"append matrix to binary file" },
  { "append",   fmatrixAppendItf, "append matrixB to matrixA" },
  { "trans",    fmatrixTransItf,  "transpose matrix" },
  { "fromSample",fmatrixFromSampleItf, "convert sample to kmeans'able FMatrix" },
  { "add",      fmatrixAddItf,    "a * matrixA + b * matrixB" },
  { "vts",      fmatrixVTSItf,    "a * matrixA + b * matrixB" },
  { "addvec",   fmatrixAddVectorItf, "a * matrixA + b * vectorB" },
  { "dev",      fmatrixDevItf,    "matrixA * matrixB" },
  { "mulcoef",  fmatrixMulcoefItf,"multiply each coefficient" },
  { "perceptron",fmatrixPerceptronItf,"perform perceptron operations for MLP" },
  { "mul",      fmatrixMulItf,    "matrixA * matrixB" },
  { "mulot",    fmatrixMulotItf,  "matrixA * matrixB'" },
  { "bmulot",   fmatrixBMulotItf, "matrixA * bandmatrixB'" },
  { "minmax",   fmatrixMinmaxItf, "gives minimum and maximum" },
  { "det",      fmatrixDeterminantItf, "compute determinant" },
  { "cosine",   fmatrixCosineItf, "create cosine transformation matrix" },
  { "neuralGas",fmatrixNeuralGasItf, "neural gas clustering" },
  { "cluster",  fmatrixSizeOptItf, "create optimal codebook" },
  { "bic",      fmatrixBICItf,     "Bayesian Information Criterion" },
  { "display",  fmatrixDisplayItf, "display matrix" },
  { "scatterPlot", fmatrixScatterPlotItf, "scatter plot" },
  { NULL,       NULL, NULL } 
};

TypeInfo fmatrixInfo = { "FMatrix", 0, -1, fmatrixMethod, 
                          fmatrixCreateItf, fmatrixFreeItf,fmatrixConfigureItf,
			  NULL, itfTypeCntlDefaultNoLink,
		         "Matrix of float values\n" } ;

Method dvectorMethod[] = 
{ 
  { "puts",   dvectorPutsItf,   "print vector as TCL list" }, 
  { "resize", dvectorResizeItf, "resize vector" }, 
  { "copy",   dvectorCopyItf,   "copy vector" },
  { ":=",     dvectorCopyItf,   "assign vector (equiv. to 'copy')" },
  { NULL,     NULL, NULL } 
};

TypeInfo dvectorInfo = { "DVector", 0, -1, dvectorMethod, 
                          dvectorCreateItf,dvectorFreeItf,dvectorConfigureItf,
			  NULL, itfTypeCntlDefaultNoLink,
		         "Vector of double values\n" } ;

Method fbmatrixMethod[] = 
{ 
  { "puts",   fbmatrixPutsItf,   "print matrix contents as TCL list" }, 
  { "mel",    fbmatrixMelItf,    "melscale filterbank" }, 
  { "meltri", fbmatrixMeltriItf, "triangular shaped melscale filterbank" }, 
  { "meltra", fbmatrixMeltraItf, "trapezoid shaped melscale filterbank" }, 
  { "linear", fbmatrixLinearItf, "linear filterbank" },
  { "display",fbmatrixDisplayItf,"display fbmatrix" },
  { NULL,     NULL, NULL } 
};

TypeInfo fbmatrixInfo = { "FBMatrix", 0, -1, fbmatrixMethod, 
                          fbmatrixCreateItf, fbmatrixFreeItf, NULL,
			  NULL, itfTypeCntlDefaultNoLink,
		         "Band matrix of float values\n" } ;

Method dmatrixMethod[] = 
{ 
  { "puts",   dmatrixPutsItf,   "print matrix contents as TCL list" }, 
  { "resize", dmatrixResizeItf, "resize matrix" }, 
  { "clear",  dmatrixClearItf,  "set all matrix values to 0" }, 
  { "copy",   dmatrixCopyItf,   "copy matrix" },
  { ":=",     dmatrixCopyItf,   "assign matrix (equiv. to 'copy')" },
  { "get",    dmatrixGetValItf, "get a single entry from a matrix" },
  { "set",    dmatrixSetValItf, "set a single entry in a matrix" },
  { "FMatrix",dmatrixFMatrixItf,"convert from a FMatrix" },
  { "trans",  dmatrixTransItf,  "transpose matrix" },
  { "unity",  dmatrixUnityItf,  "make matrix a unity matrix" },
  { "mul",    dmatrixMulItf,    "matrixA * matrixB" },
  { "mulot",  dmatrixMulotItf,  "matrixA * matrixB'" },
  { "eigen",  dmatrixEigenItf,  "eigenvalues and vectors of symmetric matrix"},
  { "svd",    dmatrixSVDItf,    "singular value decomposition"},
  { "inv",    dmatrixInvItf,    "inverse of matrix using svd"},
  { "det",    dmatrixDeterminantItf, "compute determinant" },
  { "simdiag",dmatrixSimdiagItf,"simultaneous diagonalisation"},
  { "bsave",  dmatrixBSaveItf,  "save matrix" },
  { "bload",  dmatrixBLoadItf,  "load matrix" },
  { NULL,     NULL, NULL } 
};

TypeInfo dmatrixInfo = { "DMatrix", 0, -1, dmatrixMethod, 
                          dmatrixCreateItf, dmatrixFreeItf,dmatrixConfigureItf,
			  NULL, itfTypeCntlDefaultNoLink,
		         "Matrix of double values\n" } ;

static int matrixInitialized = 0;

int Matrix_Init (void)
{

  if ( !matrixInitialized ) {

    if ( FFMat_Init() != TCL_OK ) return TCL_ERROR;

    itfNewType ( &svectorInfo );
    itfNewType ( &imatrixInfo );
    itfNewType ( &cmatrixInfo );
    itfNewType ( &fvectorInfo );
    itfNewType ( &fmatrixInfo );
    itfNewType ( &fbmatrixInfo );
    itfNewType ( &dvectorInfo );
    itfNewType ( &dmatrixInfo );

    matrixInitialized = 1;
  }

  return TCL_OK;
}
