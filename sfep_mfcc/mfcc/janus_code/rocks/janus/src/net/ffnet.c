/* ========================================================================
    JANUS      Speech- to Speech Translation
               ------------------------------------------------------------
               Object: General feed forward multi layer neural nets
               ------------------------------------------------------------

    Author  :  Juergen Fritsch
    Module  :  ffnet.c
    Date    :  $Id: ffnet.c 2390 2003-08-14 11:20:32Z fuegen $
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
    Revision 3.6  2003/08/14 11:20:20  fuegen
    Merged changes on branch jtk-01-01-15-fms (jaguar -> ibis-013)

    Revision 3.5.4.2  2003/07/02 17:52:08  fuegen
    fixes for file handling under Windows
    using general functions for file open/close

    Revision 3.5.4.1  2002/06/26 11:57:57  fuegen
    changes for new directory structure support and code cleaning

    Revision 3.5  2001/01/15 09:49:57  janus
    Merged Christian's version with WINDOWS support (branch jtk-00-11-14-fuegen)

    Revision 3.4.4.1  2001/01/12 15:16:54  janus
    necessary changes for running janus under WINDOWS

    Revision 3.4  2000/11/14 12:29:33  janus
    Merging branch jtk-00-10-27-jmcd.

    Revision 3.3.34.2  2000/11/08 17:12:30  janus
    Finished making changes required for compilation under 'gcc -Wall'.

    Revision 3.3.34.1  2000/11/06 10:50:28  janus
    Made changes to enable compilation under -Wall.

    Revision 3.3  1997/07/04 11:53:03  fritsch
    made it gcc -Wall clean

 * Revision 1.1  1997/01/27  08:59:05  fritsch
 * Initial revision
 *

   ======================================================================== */

char ffnetRCSVersion[]= 
           "@(#)1$Id: ffnet.c 2390 2003-08-14 11:20:32Z fuegen $";


#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include <string.h>
#include "common.h"
#include "itf.h"

#include "error.h"
#include "mach_ind_io.h"
#include "matrix.h"
#include "ffnet.h"
#include "ffmat.h"

#if defined(WINDOWS) & defined(DISABLE_WARNINGS)
#pragma warning( disable : 4113 )
#endif


#define   COMMENT_CHAR       ';'
#define   MAJOR_VERSION      1
#define   MINOR_VERSION      0
#define   MINVAL             1E-10
#define   ETA_INCREASE       0.05
#define   ETA_DECREASE       0.5
#define   sq(X)              ((X)*(X))




/* ==================================================================== */
/*                                                                      */
/*   PART I : network, activation and error functions (+ derivatives)   */
/*                                                                      */
/* ==================================================================== */


/*   

    The following table gives an overview of error functions and
    corresponding link (activation) functions which are available
    right now. Using the appropriate link function is essential for
    statistically interpretable network outputs *and* an efficient 
    update (backprop) algorithm.             fritsch@ira.uka.de

            mse   = mean squared error 
            binom = cross entropy
            kl    = relative entropy (KL-distance)
            multi = cross entropy for n-way classification

 +-------------+-----------------+--------------------+------------------+
 |  ErrorFct   |  canonical link |  probability model |  application     |
 + ------------------------------+--------------------+------------------+
 |  mse        |  identity       |  Gaussian          |  Regression      |
 |  binom      |  sigmoid        |  Binomial          |  2-way class.    |
 |  kl         |  sigmoid        |  Binomial          |  2-way class.    |
 |  multi      |  softmax        |  Multinomial       |  n-way class.    |
 +-------------+-----------------+--------------------+------------------+

*/



/* ============== activation functions + derivatives ================== */


static char   netFctName[][12] = { "UNDEFINED",
                                   "projective",
                                   "radial" };

static char   actFctName[][12] = { "UNDEFINED",
                                   "linear",
                                   "tanh",
                                   "sigmoid",
                                   "exp",
                                   "softmax" };

static char   errFctName[][12] = { "UNDEFINED",
                                   "mse",
                                   "binom",
                                   "kl",
                                   "multi",
                                   "class" };



/* ---------------------------------------------------------------------- */
/*  char2netFct                                                           */
/* ---------------------------------------------------------------------- */
int  char2netFct (char *name, NetFctPtr *netFct) {

  *netFct = NULL;

  if (strcmp(name,netFctName[1]) == 0) {
    *netFct = fmatrixMulFFT;
    return TCL_OK;
  }

  if (strcmp(name,netFctName[2]) == 0) {
    *netFct = fmatrixRadT;
    return TCL_OK;
  }

  ERROR("Can't resolve network function name\n");
  return TCL_ERROR;
}



/* ---------------------------------------------------------------------- */
/*  netFct2char                                                           */
/* ---------------------------------------------------------------------- */
int  netFct2char (NetFctPtr netFct, char **name) {

  *name = netFctName[0];

  if (netFct == fmatrixMulFFT) {
    *name = netFctName[1];
    return TCL_OK;
  }

  if (netFct == fmatrixRadT) {
    *name = netFctName[2];
    return TCL_OK;
  }

  return TCL_ERROR;
}



/* ---------------------------------------------------------------------- */
/*  char2actFct                                                           */
/* ---------------------------------------------------------------------- */
int  char2actFct (char *name, ActFctPtr *actFct, 
                  ActDerFctPtr *actDerFct) {

  *actFct    = NULL;
  *actDerFct = NULL;

  if (strcmp(name,actFctName[1]) == 0) { 
    *actFct = idActFct; 
    *actDerFct = idActDerFct;
    return TCL_OK;
  }

  if (strcmp(name,actFctName[2]) == 0) {
    *actFct = tanhActFct; 
    *actDerFct = tanhActDerFct; 
    return TCL_OK;
  }

  if (strcmp(name,actFctName[3]) == 0) {
    *actFct = sigActFct; 
    *actDerFct = sigActDerFct; 
    return TCL_OK;
  }

  if (strcmp(name,actFctName[4]) == 0) {
    *actFct = expActFct; 
    *actDerFct = expActDerFct; 
    return TCL_OK;
  }

  if (strcmp(name,actFctName[5]) == 0) {
    *actFct = softActFct; 
    *actDerFct = softActDerFct; 
    return TCL_OK;
  }

  ERROR("Can't resolve activation function name\n");
  return TCL_ERROR;
}



/* ---------------------------------------------------------------------- */
/*  actFct2char                                                           */
/* ---------------------------------------------------------------------- */
int  actFct2char (ActFctPtr actFct, ActDerFctPtr actDerFct,
                  char **name) {

  *name = actFctName[0];

  if ((actFct == idActFct) && (actDerFct == idActDerFct)) {
    *name = actFctName[1];
    return TCL_OK;
  }

  if ((actFct == tanhActFct) && (actDerFct == tanhActDerFct)) {
    *name = actFctName[2];
    return TCL_OK;
  }

  if ((actFct == sigActFct) && (actDerFct == sigActDerFct)) {
    *name = actFctName[3];
    return TCL_OK;
  }

  if ((actFct == expActFct) && (actDerFct == expActDerFct)) {
    *name = actFctName[4];
    return TCL_OK;
  }

  if ((actFct == softActFct) && (actDerFct == softActDerFct)) {
    *name = actFctName[5];
    return TCL_OK;
  }

  return TCL_ERROR;
}



/* ---------------------------------------------------------------------- */
/*  char2errFct                                                           */
/* ---------------------------------------------------------------------- */
int  char2errFct (char *name, ErrFctPtr *errFct, 
                  ErrDerFctPtr *errDerFct) {

  *errFct    = NULL;
  *errDerFct = NULL;

  if (strcmp(name,errFctName[1]) == 0) { 
    *errFct = mseErrFct; 
    *errDerFct = mseErrDerFct; 
    return TCL_OK;
  }

  if (strcmp(name,errFctName[2]) == 0) { 
    *errFct = binomErrFct; 
    *errDerFct = binomErrDerFct; 
    return TCL_OK;
  }

  if (strcmp(name,errFctName[3]) == 0) { 
    *errFct = klErrFct; 
    *errDerFct = klErrDerFct; 
    return TCL_OK;
  }

  if (strcmp(name,errFctName[4]) == 0) { 
    *errFct = multiErrFct; 
    *errDerFct = multiErrDerFct; 
    return TCL_OK;
  }

  if (strcmp(name,errFctName[5]) == 0) { 
    *errFct = classErrFct; 
    *errDerFct = multiErrDerFct; 
    return TCL_OK;
  }

  ERROR("Can't resolve network error function name\n");
  return TCL_ERROR;
}



/* ---------------------------------------------------------------------- */
/*  errFct2char                                                           */
/* ---------------------------------------------------------------------- */
int  errFct2char (ErrFctPtr errFct, ErrDerFctPtr errDerFct,
                  char **name) {

  *name = errFctName[0];

  if ((errFct == mseErrFct) && (errDerFct == mseErrDerFct)) {
    *name = errFctName[1];
    return TCL_OK;
  }

  if ((errFct == binomErrFct) && (errDerFct == binomErrDerFct)) {
    *name = errFctName[2];
    return TCL_OK;
  }

  if ((errFct == klErrFct) && (errDerFct == klErrDerFct)) {
    *name = errFctName[3];
    return TCL_OK;
  }

  if ((errFct == multiErrFct) && (errDerFct == multiErrDerFct)) {
    *name = errFctName[4];
    return TCL_OK;
  }

  if ((errFct == classErrFct) && (errDerFct == multiErrDerFct)) {
    *name = errFctName[5];
    return TCL_OK;
  }

  return TCL_ERROR;
}




/* -------------------------------------------------------------------- */
/*  idActFct                 implementing identity activation function  */
/* -------------------------------------------------------------------- */
int  idActFct (FMatrix *Y, FMatrix *X) {

  (void) fmatrixCopy(Y,X);

  return TCL_OK;
}



/* -------------------------------------------------------------------- */
/*  idActDerFct              implementing identity activation function  */
/* -------------------------------------------------------------------- */
int  idActDerFct (FMatrix *Y, FMatrix *X) {

  return fmatrixInitConst(Y,1.0);
}



/* -------------------------------------------------------------------- */
/*  tanhActFct      implementing asymetric sigmoid activation function  */
/* -------------------------------------------------------------------- */
int  tanhActFct (FMatrix *Y, FMatrix *X) {

  register int     i,j;

  assert(Y && X);
  fmatrixResize(Y,X->m,X->n);

  for (i=0; i<Y->m; i++)
    for (j=0; j<Y->n; j++)
      Y->matPA[i][j] = tanh(X->matPA[i][j]);

  return TCL_OK;
}



/* -------------------------------------------------------------------- */
/*  tanhActDerFct   implementing asymetric sigmoid activation function  */
/* -------------------------------------------------------------------- */
int  tanhActDerFct (FMatrix *Y, FMatrix *X) {

  register int     i,j;

  assert(Y && X);
  fmatrixResize(Y,X->m,X->n);

  for (i=0; i<Y->m; i++)
    for (j=0; j<Y->n; j++)
      Y->matPA[i][j] = 1.0 - sq(X->matPA[i][j]);

  return TCL_OK;
}



/* -------------------------------------------------------------------- */
/*  sigActFct        implementing positive sigmoid activation function  */
/* -------------------------------------------------------------------- */
int  sigActFct (FMatrix *Y, FMatrix *X) {

  register int     i,j;

  assert(Y && X);
  fmatrixResize(Y,X->m,X->n);

  for (i=0; i<Y->m; i++)
    for (j=0; j<Y->n; j++)
      Y->matPA[i][j] = 1.0/(1.0+exp(-X->matPA[i][j]));

  return TCL_OK;
}



/* -------------------------------------------------------------------- */
/*  sigActDerFct     implementing positive sigmoid activation function  */
/* -------------------------------------------------------------------- */
int  sigActDerFct (FMatrix *Y, FMatrix *X) {

  register int     i,j;

  assert(Y && X);
  fmatrixResize(Y,X->m,X->n);

  for (i=0; i<Y->m; i++)
    for (j=0; j<Y->n; j++)
      Y->matPA[i][j] = X->matPA[i][j] * (1.0 - X->matPA[i][j]);

  return TCL_OK;
}



/* -------------------------------------------------------------------- */
/*  expActFct             implementing exponential activation function  */
/* -------------------------------------------------------------------- */
int  expActFct (FMatrix *Y, FMatrix *X) {

  register int     i,j;

  assert(Y && X);
  fmatrixResize(Y,X->m,X->n);

  for (i=0; i<Y->m; i++)
    for (j=0; j<Y->n; j++)
      Y->matPA[i][j] = exp(-X->matPA[i][j]);

  return TCL_OK;
}



/* -------------------------------------------------------------------- */
/*  expActDerFct          implementing exponential activation function  */
/* -------------------------------------------------------------------- */
int  expActDerFct (FMatrix *Y, FMatrix *X) {

  register int     i,j;

  assert(Y && X);
  fmatrixResize(Y,X->m,X->n);

  for (i=0; i<Y->m; i++)
    for (j=0; j<Y->n; j++)
      Y->matPA[i][j] = -X->matPA[i][j];

  return TCL_OK;
}



/* -------------------------------------------------------------------- */
/*  softActFct                implementing softmax activation function  */
/* -------------------------------------------------------------------- */
int  softActFct (FMatrix *Y, FMatrix *X) {

  register int         i,j;
  register double      maxVal,val,sum;
  float                *x,*y;

  assert(Y && X);
  fmatrixResize(Y,X->m,X->n);

  for (i=0; i<Y->m; i++) {

    x = X->matPA[i];
    y = Y->matPA[i];

    /* 1.) compute maximum neuron activation */
    maxVal = -1E20;
    for (j=0; j<Y->n; j++)
      if ((val = x[j]) > maxVal) maxVal = val;

    /* 2.) compute shifted exponentials and total sum */
    sum = 0.0;
    for (j=0; j<Y->n; j++) {
      val = exp(x[j] - maxVal);
      y[j] = val;
      sum += val;
    }
    val = 1.0/sum;   /* no check - sum can never become zero */

    /* 3.) compute normalized softmax activations */
    for (j=0; j<Y->n; j++) y[j] *= val;

  }

  return TCL_OK;
}



/* -------------------------------------------------------------------- */
/*  softActDerFct             implementing softmax activation function  */
/* -------------------------------------------------------------------- */
int  softActDerFct (FMatrix *Y, FMatrix *X) {

  register int        i,j,k;
  float               *x,*y;

  assert(Y && X);
  fmatrixResize(Y,X->m,X->n);
  fmatrixInitConst(Y,0.0);

  for (i=0; i<Y->m; i++) {

    x = X->matPA[i];
    y = Y->matPA[i];

    for (j=0; j<Y->n; j++) {
      for (k=0; k<Y->n; k++) {
        if (j==k)
          y[k] += x[k] * (1.0 - x[j]);
        else
          y[k] += x[k] * (-x[j]);
      }
    }

  }

  return TCL_OK;
}



/* ================= error functions + derivatives ==================== */



/* -------------------------------------------------------------------- */
/*  mseErrFct                 implementing mean squared error function  */
/*                               canonical link to identity activation  */
/* -------------------------------------------------------------------- */
float  mseErrFct (FMatrix *Y, FMatrix *T) {

  register int         i,j;
  double               error = 0.0;

  assert(Y && T);
  
  if ((Y->m != T->m) || (Y->n != T->n)) {
    ERROR("Target matrix T and output matrix Y do not match\n");
    return 0.0;
  }

  if (Y->m == 0) return error;

  for (i=0; i<T->m; i++)
    for (j=0; j<T->n; j++)
      error += (double) sq(Y->matPA[i][j] - T->matPA[i][j]);

  return (float) error / Y->m;
}



/* -------------------------------------------------------------------- */
/*  mseErrDerFct              implementing mean squared error function  */
/*                               canonical link to identity activation  */
/* -------------------------------------------------------------------- */
int  mseErrDerFct (FMatrix *Z, FMatrix *Y, FMatrix *T) {

  register int         i,j;

  assert(Y && T);
  
  if ((Y->m != T->m) || (Y->n != T->n)) {
    ERROR("Target matrix T and output matrix Y do not match\n");
    return 0.0;
  }

  fmatrixResize(Z,Y->m,Y->n);

  for (i=0; i<T->m; i++)
    for (j=0; j<T->n; j++)
      Z->matPA[i][j] = (Y->matPA[i][j] - T->matPA[i][j]);

  return fmatrixPrune(Z);
}



/* -------------------------------------------------------------------- */
/*  binomErrFct                   implementing binomial error function  */
/*                              canonical link to sigmoidal activation  */
/* -------------------------------------------------------------------- */
float  binomErrFct (FMatrix *Y, FMatrix *T) {

  register int         i,j;
  register double      t,y;
  double               error = 0.0;

  assert(Y && T);
  
  if ((Y->m != T->m) || (Y->n != T->n)) {
    ERROR("Target matrix T and output matrix Y do not match\n");
    return 0.0;
  }

  if (Y->m == 0) return error;

  for (i=0; i<T->m; i++)
    for (j=0; j<T->n; j++) {
      t = T->matPA[i][j];
      y = Y->matPA[i][j];
      if (y < MINVAL) y = MINVAL;
      if (y > 1.0-MINVAL) y = 1.0-MINVAL;
      error -= (t*log(y) + (1.0-t)*log(1.0-y));
    }

  return (float) error / Y->m;
}



/* -------------------------------------------------------------------- */
/*  binomErrDerFct                implementing binomial error function  */
/*                              canonical link to sigmoidal activation  */
/* -------------------------------------------------------------------- */
int  binomErrDerFct (FMatrix *Z, FMatrix *Y, FMatrix *T) {

  register int         i,j;
  register double      t,y;

  assert(Y && T);
  
  if ((Y->m != T->m) || (Y->n != T->n)) {
    ERROR("Target matrix T and output matrix Y do not match\n");
    return 0.0;
  }

  fmatrixResize(Z,Y->m,Y->n);

  for (i=0; i<T->m; i++)
    for (j=0; j<T->n; j++) {
      t = T->matPA[i][j];
      y = Y->matPA[i][j];
      Z->matPA[i][j] = (float) (y - t);
    }

  return fmatrixPrune(Z);
}



/* -------------------------------------------------------------------- */
/*  klErrFct     implementing relative entropy (KL-distance) error fct  */
/*                              canonical link to sigmoidal activation  */
/* -------------------------------------------------------------------- */
float  klErrFct (FMatrix *Y, FMatrix *T) {

  register int         i,j;
  register double      t,y;
  double               error = 0.0;

  assert(Y && T);
  
  if ((Y->m != T->m) || (Y->n != T->n)) {
    ERROR("Target matrix T and output matrix Y do not match\n");
    return 0.0;
  }

  if (Y->m == 0) return error;

  for (i=0; i<T->m; i++)
    for (j=0; j<T->n; j++) {
      t = T->matPA[i][j];
      y = Y->matPA[i][j];
      if (y < MINVAL) y = MINVAL;
      if (y > 1.0-MINVAL) y = 1.0-MINVAL;
      
      if (t < MINVAL) 
        error += (1.0-t)*(log(1.0-t)-log(1.0-y));
      else if (t > 1.0-MINVAL)
        error += t*(log(t)-log(y));
      else
        error += (t*(log(t)-log(y)) + (1.0-t)*(log(1.0-t)-log(1.0-y)));
    }

  return (float) error / Y->m;
}



/* -------------------------------------------------------------------- */
/*  klErrDerFct  implementing relative entropy (KL-distance) error fct  */
/*                              canonical link to sigmoidal activation  */
/* -------------------------------------------------------------------- */
int  klErrDerFct (FMatrix *Z, FMatrix *Y, FMatrix *T) {

  register int         i,j;
  double               t,y;

  assert(Y && T);
  
  if ((Y->m != T->m) || (Y->n != T->n)) {
    ERROR("Target matrix T and output matrix Y do not match\n");
    return 0.0;
  }

  fmatrixResize(Z,Y->m,Y->n);
  fmatrixInitConst(Z,0.0);

  for (i=0; i<T->m; i++)
    for (j=0; j<T->n; j++) {
      t = T->matPA[i][j];
      y = Y->matPA[i][j];
      Z->matPA[i][j] += (float) (y - t);
    }

  return fmatrixPrune(Z);
}



/* -------------------------------------------------------------------- */
/*  multiErrFct     implementing multinomial (entropic) error function  */
/*                                canonical link to softmax activation  */
/* -------------------------------------------------------------------- */
float  multiErrFct (FMatrix *Y, FMatrix *T) {

  register int         i,j;
  register double      t,y;
  double               error = 0.0;

  assert(Y && T);
  
  if ((Y->m != T->m) || (Y->n != T->n)) {
    ERROR("Target matrix T and output matrix Y do not match\n");
    return 0.0;
  }

  if (Y->m == 0) return error;

  for (i=0; i<T->m; i++)
    for (j=0; j<T->n; j++) {
      t = T->matPA[i][j];
      y = Y->matPA[i][j];
      if (y < MINVAL) y = MINVAL;
      if (y > 1.0-MINVAL) y = 1.0-MINVAL;
      error -= t*log(y);
    }

  return (float) error / Y->m;
}



/* -------------------------------------------------------------------- */
/*  classErrFct                          classification error function  */
/* -------------------------------------------------------------------- */
float  classErrFct (FMatrix *Y, FMatrix *T) {

  register int       i,j,tIdx,yIdx;
  register float     tVal,yVal;
  double             error = 0.0;

  assert(Y && T);

  if ((Y->m != T->m) || (Y->n != T->n)) {
    ERROR("Target matrix T and output matrix Y do not match\n");
    return 0.0;
  }

  if (Y->m == 0) return error;

  for (i=0; i<T->m; i++) {
    if (T->n > 1) {
      tVal = yVal = MINVAL;
      tIdx = yIdx = 0;
      for (j=0; j<T->n; j++) {
        if (T->matPA[i][j] > tVal) {
          tVal = T->matPA[i][j];
          tIdx = j;
        }
        if (Y->matPA[i][j] > yVal) {
          yVal = Y->matPA[i][j];
          yIdx = j;
        }
      }
      if (tIdx != yIdx) error += 1.0;
    } else {
      if (((T->matPA[i][0] > 0.5) && (Y->matPA[i][0] < 0.5)) ||
          ((T->matPA[i][0] < 0.5) && (Y->matPA[i][0] > 0.5)))
        error += 1.0;
    }
  }

  return (float) error / Y->m;
}



/* -------------------------------------------------------------------- */
/*  multiErrDerFct  implementing multinomial (entropic) error function  */
/*                                canonical link to softmax activation  */
/* -------------------------------------------------------------------- */
int  multiErrDerFct (FMatrix *Z, FMatrix *Y, FMatrix *T) {

  register int         i,j,k;
  float                *t,*y;

  assert(Y && T);
  
  if ((Y->m != T->m) || (Y->n != T->n)) {
    ERROR("Target matrix T and output matrix Y do not match\n");
    return 0.0;
  }

  fmatrixResize(Z,Y->m,Y->n);
  fmatrixInitConst(Z,0.0);

  for (i=0; i<T->m; i++) {

    t = T->matPA[i];
    y = Y->matPA[i];

    for (j=0; j<T->n; j++) {

      if (t[j] < MINVAL) continue;

      for (k=0; k<T->n; k++) {
        if (j==k)
          Z->matPA[i][k] += t[j]*(y[k] - 1.0);
        else
          Z->matPA[i][k] += t[j]*y[k];
      }

    }
  }
 

  return fmatrixPrune(Z);
}



/* ==================================================================== */
/*                                                                      */
/*     PART II : network object creator/destructor + misc. handling     */
/*                                                                      */
/* ==================================================================== */


/* ==================================================================== */
/*                             (1) FFLink                               */
/* ==================================================================== */



/* -------------------------------------------------------------------- */
/*  fflinkInit         (arguments m,n are dimensions of weight matrix)  */
/* -------------------------------------------------------------------- */
int  fflinkInit (FFLink *link, int sourceX, int m, int n, 
                 NetFctPtr netFct) {

  assert((m > 0) && (n > 0));

  link->sourceX = sourceX;

  link->W = fmatrixCreate(m,n);
  fmatrixInitRandom(link->W,2.0/((float) n));

  link->mask   = NULL;
  link->netFct = netFct;
  link->dW     = NULL;
  link->ddW    = NULL;

  link->eval   = 1;
  link->update = 1;

  link->eta    = 0.1;     /* initial learning rate */
  link->alpha  = 0.9;     /* initial momentum factor */
  link->gamma  = 0.0;

  return TCL_OK;
}



/* -------------------------------------------------------------------- */
/*  fflinkCreate       (arguments m,n are dimensions of weight matrix)  */
/* -------------------------------------------------------------------- */
FFLink  *fflinkCreate (int sourceX, int m, int n, NetFctPtr netFct) {

  FFLink       *link = (FFLink *) malloc (sizeof(FFLink));

  fflinkInit(link,sourceX,m,n,netFct);

  return link;
}



/* -------------------------------------------------------------------- */
/*  fflinkDeinit                                                        */
/* -------------------------------------------------------------------- */
int  fflinkDeinit (FFLink *link) {

  if (!link) return TCL_OK;

  if (link->W)    fmatrixDestroy(link->W);
  if (link->mask) imatrixDestroy(link->mask);
  link->W    = NULL;
  link->mask = NULL;

  link->netFct = NULL;

  if (link->dW)  fmatrixDestroy(link->dW);
  if (link->ddW) fmatrixDestroy(link->ddW);
  link->dW  = NULL;
  link->ddW = NULL;

  link->eval   = 1;
  link->update = 1;
  link->eta    = 0.1;
  link->alpha  = 0.9;
  link->gamma  = 0.0;

  return TCL_OK;
}



/* -------------------------------------------------------------------- */
/*  fflinkDestroy                                                       */
/* -------------------------------------------------------------------- */
int  fflinkDestroy (FFLink *link) {

  assert(link);

  fflinkDeinit(link);
  free(link);

  return TCL_OK;
}



/* -------------------------------------------------------------------- */
/*  fflinkReset                                                         */
/* -------------------------------------------------------------------- */
int  fflinkReset (FFLink *link) {

  assert(link);

  if (link->dW) {
    fmatrixDestroy(link->dW);
    link->dW = NULL;
  }
  if (link->ddW) {
    fmatrixDestroy(link->ddW);
    link->ddW = NULL;
  }
 
  return TCL_OK;
}



/* ==================================================================== */
/*                             (2) FFLayer                              */
/* ==================================================================== */


/* -------------------------------------------------------------------- */
/*  fflayerInit                                                         */
/* -------------------------------------------------------------------- */
int  fflayerInit (FFLayer *layer, int nodeN, ActFctPtr actFct, 
                  ActDerFctPtr actDerFct) {

  layer->act = fmatrixCreate(1,nodeN);
  
  layer->actFct    = actFct;
  layer->actDerFct = actDerFct;

  layer->actDer = NULL;
  layer->delta  = NULL;

  layer->linkN = 0;
  layer->link  = NULL;

  return TCL_OK;
}



/* -------------------------------------------------------------------- */
/*  fflayerCreate                                                       */
/* -------------------------------------------------------------------- */
FFLayer  *fflayerCreate (int nodeN, ActFctPtr actFct, 
                         ActDerFctPtr actDerFct) {

  FFLayer         *layer = (FFLayer *) malloc (sizeof(FFLayer));

  fflayerInit(layer,nodeN,actFct,actDerFct);

  return layer;
}



/* -------------------------------------------------------------------- */
/*  fflayerDeinit                                                       */
/* -------------------------------------------------------------------- */
int  fflayerDeinit (FFLayer *layer) {

  register int       linkX;

  if (!layer) return TCL_OK;

  if (layer->act)    fmatrixDestroy(layer->act);
  if (layer->actDer) fmatrixDestroy(layer->actDer);
  if (layer->delta)  fmatrixDestroy(layer->delta);

  layer->act    = NULL;
  layer->actDer = NULL;
  layer->delta  = NULL;

  layer->actFct    = NULL;
  layer->actDerFct = NULL;

  for (linkX=0; linkX<layer->linkN; linkX++)
    fflinkDestroy(layer->link[linkX]);
  if (layer->linkN > 0) free(layer->link);
  layer->link  = NULL;
  layer->linkN = 0;

  return TCL_OK;
}



/* -------------------------------------------------------------------- */
/*  fflayerDestroy                                                      */
/* -------------------------------------------------------------------- */
int  fflayerDestroy (FFLayer *layer) {

  assert(layer);

  fflayerDeinit(layer);
  free(layer);

  return TCL_OK;
}



/* -------------------------------------------------------------------- */
/*  fflayerReset                                                        */
/* -------------------------------------------------------------------- */
int  fflayerReset (FFLayer *layer) {

  assert(layer);

  if (layer->actDer) {
    fmatrixDestroy(layer->actDer);
    layer->actDer = NULL;
  }
  if (layer->delta) {
    fmatrixDestroy(layer->delta);
    layer->delta = NULL;
  }

  return TCL_OK;
}



/* ==================================================================== */
/*                              (3) FFNet                               */
/* ==================================================================== */


/* -------------------------------------------------------------------- */
/*  ffnetReset                                                          */
/* -------------------------------------------------------------------- */
int  ffnetReset (FFNet *net) {

  FFLayer     *layer;
  FFLink      *link;
  int         layerX,linkX;

  for (layerX=0; layerX<net->layerN; layerX++) {
    layer = net->layer[layerX];
    fflayerReset(layer);
    for (linkX=0; linkX<layer->linkN; linkX++) {
      link = layer->link[linkX];
      fflinkReset(link);
    }
  }

  return TCL_OK;
}



/* -------------------------------------------------------------------- */
/*  ffnetAddLink                                                        */
/* -------------------------------------------------------------------- */
int  ffnetAddLink (FFNet *net, int targetX, int sourceX,
                   NetFctPtr netFct) {

  FFLayer           *target;
  int               m,n,bias;

  assert(net);

  /* ------------------------------------------- */
  /*  check for validity of sourceX and targetX  */
  /* ------------------------------------------- */
  if ((targetX < 0) || (targetX >= net->layerN)) {
    ERROR("Invalid target layer index %d\n",targetX);
    return TCL_ERROR;
  }

  if (((sourceX >= 0) && (sourceX <= targetX)) || 
       (sourceX >= net->layerN)) {
    ERROR("Invalid source layer index %d\n",sourceX);
    return TCL_ERROR;
  }

  target = net->layer[targetX];

  /* ------------------------------------------------------------ */
  /*  only the first link in each layer can contain bias columns  */
  /* ------------------------------------------------------------ */
  if (target->linkN == 0) bias = 1; else bias = 0;

  m = target->act->n;
  if (sourceX >= 0)
    n = net->layer[sourceX]->act->n + bias;
  else 
    n = net->inputN + bias;

  /* ------------------------- */
  /*  create and add the link  */
  /* ------------------------- */
  target->link = (FFLink **) realloc (target->link,
                       sizeof(FFLink *) * target->linkN+1);

  target->link[target->linkN++] = fflinkCreate(sourceX,m,n,netFct);

  return TCL_OK;
}



/* -------------------------------------------------------------------- */
/*  ffnetAddLayer                                                       */
/* -------------------------------------------------------------------- */
int  ffnetAddLayer (FFNet *net, int nodeN, ActFctPtr actFct, 
                    ActDerFctPtr actDerFct) {

  ActFctPtr       fct1 = actFct; 
  ActDerFctPtr    fct2 = actDerFct;

  assert(net && (nodeN > 0));

  if (net->layerN == 0) {

    if (nodeN != net->outputN) {
      ERROR("Expecting %d, not %d neurons for output layer\n",
            net->outputN,nodeN);
      return TCL_ERROR;
    }

    if (((actFct == idActFct)   && (net->errFct != mseErrFct))  ||
        ((actFct == sigActFct)  && (net->errFct != binomErrFct)
                                && (net->errFct != klErrFct))   ||
        ((actFct == softActFct) && (net->errFct != multiErrFct)
                                && (net->errFct != classErrFct))) {
      WARN("invalid activation function for output layer\n");

      if (net->errFct == mseErrFct) {
        fct1 = idActFct;
        fct2 = idActDerFct;
        INFO("substituting identity activation function in output layer\n");
      }

      if ((net->errFct == binomErrFct) || (net->errFct == klErrFct)) {
        fct1 = sigActFct;
        fct2 = sigActDerFct;
        INFO("substituting sigmoid activation function in output layer\n");
      }

      if (net->errFct == multiErrFct) {
        fct1 = softActFct;
        fct2 = softActDerFct;
        INFO("substituting softmax activation function in output layer\n");
      }

      if (net->errFct == classErrFct) {
        fct1 = softActFct;
        fct2 = softActDerFct;
        INFO("substituting softmax activation function in output layer\n");
      }

    }

  }

  if (((fct1 == idActFct)   && (fct2 != idActDerFct))   ||
      ((fct1 == tanhActFct) && (fct2 != tanhActDerFct)) ||
      ((fct1 == sigActFct)  && (fct2 != sigActDerFct))  ||
      ((fct1 == expActFct)  && (fct2 != expActDerFct))  ||
      ((fct1 == softActFct) && (fct2 != softActDerFct))) {
    ERROR("mismatch between activation function and its derivative\n");
    return TCL_ERROR;
  }

  net->layer = (FFLayer **) realloc (net->layer,sizeof(FFLayer *) * 
                                     net->layerN+1);
  net->layer[net->layerN++] = fflayerCreate(nodeN,fct1,fct2);

  return TCL_OK;
}



/* -------------------------------------------------------------------- */
/*  ffnetInit                                                           */
/* -------------------------------------------------------------------- */
int  ffnetInit (FFNet *net, int outputN, int inputN,
                ErrFctPtr errFct, ErrDerFctPtr errDerFct) {

  net->outputN   = outputN;
  net->inputN    = inputN;

  if (((errFct == mseErrFct)   && (errDerFct != mseErrDerFct))   ||
      ((errFct == binomErrFct) && (errDerFct != binomErrDerFct)) ||
      ((errFct == klErrFct)    && (errDerFct != klErrDerFct))    ||
      ((errFct == multiErrFct) && (errDerFct != multiErrDerFct))) {
    ERROR("mismatch between error function and its derivative\n");
    return TCL_ERROR;
  }

  net->errFct    = errFct;
  net->errDerFct = errDerFct;

  net->layerN    = 0;
  net->layer     = NULL;

  return TCL_OK;
}



/* -------------------------------------------------------------------- */
/*  ffnetCreate                                                         */
/* -------------------------------------------------------------------- */
FFNet  *ffnetCreate (char *name, int outputN, int inputN,
                     ErrFctPtr errFct, ErrDerFctPtr errDerFct) {

  FFNet      *net = (FFNet *) malloc (sizeof(FFNet));

  net->name = strdup(name);
  net->useN = 0;

  ffnetInit(net,outputN,inputN,errFct,errDerFct);

  return net;
}



/* -------------------------------------------------------------------- */
/*  ffnetDeinit                                                         */
/* -------------------------------------------------------------------- */
int  ffnetDeinit (FFNet *net) {

  register int  layerX;

  if (!net) return TCL_OK;

  net->outputN = 0;
  net->inputN  = 0;

  net->errFct    = NULL;
  net->errDerFct = NULL;

  for (layerX=0; layerX<net->layerN; layerX++)
    fflayerDestroy(net->layer[layerX]);
  if (net->layerN > 0) free(net->layer);

  net->layer  = NULL;
  net->layerN = 0;

  return TCL_OK;
}



/* -------------------------------------------------------------------- */
/*  ffnetDestroy                                                        */
/* -------------------------------------------------------------------- */
int  ffnetDestroy (FFNet *net) {

  assert(net);

  ffnetDeinit(net);
  if (net->name) free(net->name);
  free(net);

  return TCL_OK;
}



/* -------------------------------------------------------------------- */
/*  fflinkCopy                                                          */
/* -------------------------------------------------------------------- */
int  fflinkCopy (FFLink *target, FFLink *source) {

  assert (target && source);
  fflinkDeinit(target);

  target->sourceX = source->sourceX;
  
  if (source->W) {
    if (!target->W) 
      target->W = fmatrixCreate(source->W->m,source->W->n);
    (void) fmatrixCopy(target->W,source->W);
  } else target->W = NULL;
 
  if (source->mask) {
    if (!target->mask) 
      target->mask = imatrixCreate(source->mask->m,source->mask->n);
    (void) imatrixCopy(target->mask,source->mask);
  } else target->mask = NULL;

  target->netFct = source->netFct;

  if (source->dW) {
    if (!target->dW) 
      target->dW = fmatrixCreate(source->dW->m,source->dW->n);
    (void) fmatrixCopy(target->dW,source->dW);
  } else target->dW = NULL;

  if (source->ddW) {
    if (!target->ddW) 
      target->ddW = fmatrixCreate(source->ddW->m,source->ddW->n);
    (void) fmatrixCopy(target->ddW,source->ddW);
  } else target->ddW = NULL;

  target->eval   = source->eval;
  target->update = source->update;
  target->eta    = source->eta;
  target->alpha  = source->alpha;
  target->gamma  = source->gamma;

  return TCL_OK;
}



/* -------------------------------------------------------------------- */
/*  fflayerCopy                                                         */
/* -------------------------------------------------------------------- */
int  fflayerCopy (FFLayer *target, FFLayer *source) {

  register int         linkX;

  assert (target && source);
  fflayerDeinit(target);

  if (source->act) {
    if (!target->act) 
      target->act = fmatrixCreate(source->act->m,source->act->n);
    (void) fmatrixCopy(target->act,source->act);
  } else target->act = NULL;

  target->actFct = source->actFct;
  target->actDerFct = source->actDerFct;
  
  if (source->actDer) {
    if (!target->actDer)
      target->actDer = fmatrixCreate(source->actDer->m,source->actDer->n);
    (void) fmatrixCopy(target->actDer,source->actDer);
  } else target->actDer = NULL;

  if (source->delta) {
    if (!target->delta)
      target->delta = fmatrixCreate(source->delta->m,source->delta->n);
    (void) fmatrixCopy(target->delta,source->delta);
  } else target->delta = NULL;

  target->linkN = source->linkN;
  target->link = (FFLink **) malloc (sizeof(FFLink *) * source->linkN);
  for (linkX=0; linkX<source->linkN; linkX++) {
    target->link[linkX] = fflinkCreate(-1,2,2,NULL);
    fflinkCopy(target->link[linkX],source->link[linkX]);
  }

  return TCL_OK;
}



/* -------------------------------------------------------------------- */
/*  ffnetCopy                                                           */
/* -------------------------------------------------------------------- */
int  ffnetCopy (FFNet *target, FFNet *source) {

  register int        layerX;

  assert(target && source);
  ffnetDeinit(target);

  target->name      = strdup(source->name);
  target->useN      = 0;
  target->outputN   = source->outputN;
  target->inputN    = source->inputN;
  target->errFct    = source->errFct;
  target->errDerFct = source->errDerFct;
  target->layerN    = source->layerN;

  target->layer = (FFLayer **) malloc (sizeof(FFLayer *) * source->layerN);
  for (layerX=0; layerX<source->layerN; layerX++) {
    target->layer[layerX] = fflayerCreate(1,NULL,NULL);
    fflayerCopy(target->layer[layerX],source->layer[layerX]);
  }

  return TCL_OK;
}



/* -------------------------------------------------------------------- */
/*  ffnetRead                                                           */
/* -------------------------------------------------------------------- */
int  ffnetRead (FFNet *net, char *filename) {

  int      n;

  if ((n = freadAdd((ClientData) net,filename,COMMENT_CHAR,
             ffnetAddItf)) < 0) {
    ERROR("ffnetAdd() failure trying to read description file\n");
    return TCL_ERROR;
  }

  return TCL_OK;
}




/* ==================================================================== */
/*                                                                      */
/*        PART III : network forward pass + learning algorithms         */
/*                                                                      */
/* ==================================================================== */



/* -------------------------------------------------------------------- */
/*  ffnetShuffle                                                        */
/* -------------------------------------------------------------------- */
int  ffnetShuffle (FFNet *net, FMatrix *input, FMatrix *target) {

  register int        i,idx1,idx2,shuffleX,m;
  register float      swappy;

  if (input->n != net->inputN) {
    ERROR("input pattern size does not match network specs\n");
    return TCL_ERROR;
  } 

  if (target->n != net->outputN) {
    ERROR("target pattern size does not match network specs\n");
    return TCL_ERROR;
  } 

  if (target->m != input->m) {
    ERROR("number of target patterns != number of input patterns\n");
    return TCL_ERROR;
  } 

  m = target->m;

  for (shuffleX=0; shuffleX<10*m; shuffleX++) {
    idx1 = (int) chaosRandom(0,m-1);
    idx2 = (int) chaosRandom(0,m-1);

    if (idx1 == idx2) continue;
  
    for (i=0; i<input->n; i++) {
      swappy = input->matPA[idx1][i];
      input->matPA[idx1][i] = input->matPA[idx2][i];
      input->matPA[idx2][i] = swappy;
    }
    for (i=0; i<target->n; i++) {
      swappy = target->matPA[idx1][i];
      target->matPA[idx1][i] = target->matPA[idx2][i];
      target->matPA[idx2][i] = swappy;
    }
  }

  return TCL_OK;
}



/* -------------------------------------------------------------------- */
/*  ffnetEval                                   forward network pass    */
/* -------------------------------------------------------------------- */
FMatrix  *ffnetEval (FFNet *net, FMatrix *input) {

  register int        layerX,linkX,i,j,firstLink;
  FFLayer             *layer;
  FFLink              *link;
  FMatrix             *source;

  assert(net);

  if (!input || (input->m * input->n == 0)) {
    ERROR("No input matrix supplied\n");
    return NULL;
  }

  if (input->n != net->inputN) {
    ERROR("input pattern size does not match network specs\n");
    return NULL;
  }

  if (net->layerN < 1) {
    ERROR("FFNet does not contain any layers - nothing to evaluate\n");
    return NULL;
  }

  /* ------------------------------------------------ */
  /*  evaluate the layers from input to output layer  */
  /* ------------------------------------------------ */
  for (layerX=net->layerN-1; layerX>=0; layerX--) {

    layer  = net->layer[layerX];
    fmatrixResize(layer->act,input->m,layer->act->n);
    if (layer->linkN == 0) fmatrixInitConst(layer->act,0.0);

    /* ------------------------------------- */
    /*  evaluate all links to current layer  */
    /* ------------------------------------- */
    firstLink = 1;
    for (linkX=0; linkX<layer->linkN; linkX++) {

      link = layer->link[linkX];
      if (!link->eval) break;

      /* ------------------------ */
      /*  check validity of link  */
      /* ------------------------ */
      if (((link->sourceX >= 0) && (link->sourceX <= layerX)) ||
           (link->sourceX >= net->layerN)) {
        ERROR("Layer %d contains forbidden link coming from layer %d\n",
              layerX,link->sourceX);
        return NULL;
      }

      /* ---------------------------------------------- */
      /*  get a pointer to the input activation matrix  */
      /* ---------------------------------------------- */
      if (link->sourceX < 0)
        source = input;
      else
        source = net->layer[link->sourceX]->act;

      /* ------------------------------------------------------- */
      /*  check if we have a mask for the current weight matrix  */
      /* ------------------------------------------------------- */
      if (link->mask) {
        if ((link->mask->m != link->W->m) || 
            (link->mask->n != link->W->n)) {
          ERROR("Weight matrix and mask matrix incompatible\n");
          return NULL;
        }
        for (i=0; i<link->W->m; i++)
          for (j=0; j<link->W->n; j++)
            if (!link->mask->matPA[i][j]) link->W->matPA[i][j] = 0.0;
      }

      /* ------------------------------ */
      /*  perform network calculations  */
      /* ------------------------------ */
      if (firstLink) {
        link->netFct(layer->act,source,link->W);
        firstLink = 0;
      } else {
        if (!layer->actDer) 
          layer->actDer = fmatrixCreate(layer->act->m,layer->act->n);
        fmatrixResize(layer->actDer,layer->act->m,layer->act->n);
        link->netFct(layer->actDer,source,link->W);
        fmatrixAccu(layer->act,layer->actDer,1.0);
      }

    }

    /* ---------------------------------- */
    /*  compute actual layer activations  */
    /* ---------------------------------- */
    if (firstLink) fmatrixInitConst(layer->act,0.0);
    layer->actFct(layer->act,layer->act);
  }

  return  net->layer[0]->act;
}



/* -------------------------------------------------------------------- */
/*  ffnetError                                                          */
/* -------------------------------------------------------------------- */
float  ffnetError (FFNet *net, FMatrix *input, FMatrix *target, 
                   ErrFctPtr errFct, int noFwd) {

  FMatrix        *output;

  assert(net && input && target);

  if (noFwd) 
    output = net->layer[0]->act;
  else
    output = ffnetEval(net,input);

  if (errFct)
    return errFct(output,target);
  else
    return net->errFct(output,target);
}



/* -------------------------------------------------------------------- */
/*  ffnetBPdelta                 compute error backpropagation delta's  */
/* -------------------------------------------------------------------- */
int  ffnetBPdelta (FFNet *net, FMatrix *output, FMatrix *target, 
                   float *weight, int layerX) {

  FFLayer               *layer,*to;
  FFLink                *link;
  register int          m,n,i,j,bias,firstLink=0;

  assert(net && output && target);

  if ((layerX < 0) || (layerX >= net->layerN)) {
    ERROR("ffnetBPdelta(): invalid layer index %d\n",layerX);
    return TCL_OK;
  }

  layer = net->layer[layerX];
  m     = layer->act->m;
  n     = layer->act->n;

  /* ---------------------------------------- */
  /*  compute delta matrix for current layer  */
  /* ---------------------------------------- */
  if (layerX==0) {

    /* -------------- */
    /*  output layer  */
    /* -------------- */
    if (!layer->delta) layer->delta = fmatrixCreate(m,n);

    net->errDerFct(layer->delta,output,target);
    if (weight)
      for (i=0; i<m; i++)
        for (j=0; j<n; j++)
          layer->delta->matPA[i][j] *= weight[i];

  } else {

    /* -------------------- */
    /*  intermediate layer  */
    /* -------------------- */
    if (!layer->actDer) layer->actDer = fmatrixCreate(m,n);
    if (!layer->delta)  layer->delta  = fmatrixCreate(m,n);

    /* -------------------------- */
    /*  search for outgoing links */
    /* -------------------------- */
    firstLink = 1;
    for (i=0; i<layerX; i++) {
      to = net->layer[i];
      for (j=0; j<to->linkN; j++) {
        link = to->link[j];
        if (link->sourceX == layerX) {

          if (link->netFct != fmatrixMulFFT) {
            ERROR("Backprop only on projective nets (for now)\n");
            return TCL_ERROR;
          } 

          /* ------------------------------------ */
          /*  take care of bias in weight matrix  */
          /* ------------------------------------ */
          bias = 0;
          if (link->W->n == layer->act->n+1) {
            bias = 1;
            link->W->n--;
          }

          /* ------------------------------------------------ */
          /*  update partial delta and restore weight matrix  */
          /* ------------------------------------------------ */
          if (firstLink) {
            fmatrixMulFF(layer->delta,to->delta,link->W);
            firstLink = 0;
          } else {
            fmatrixMulFF(layer->actDer,to->delta,link->W);
            fmatrixAccu(layer->delta,layer->actDer,1.0);
          }
          if (bias) link->W->n++;

        }
      }
    }

    /* ------------------------------------------------- */
    /*  compute final delta using activation derivative  */
    /* ------------------------------------------------- */
    layer->actDerFct(layer->actDer,layer->act);
    if (firstLink) fmatrixInitConst(layer->delta,0.0);
    fmatrixProd(layer->delta,layer->delta,layer->actDer);

  }

  return TCL_OK;
}



/* -------------------------------------------------------------------- */
/*  ffnetBPgrad                         compute weight matrix gradient  */
/* -------------------------------------------------------------------- */
int  ffnetBPgrad (FFNet *net, FMatrix *input, int layerX, int linkX) {

  FFLayer            *layer;
  FFLink             *link;
  FMatrix            *srcAct;
  FMatrix            *deltaT;
  register int       i,j;

  assert(net && input);

  if ((layerX < 0) || (layerX >= net->layerN)) {
    ERROR("invalid layer index %d\n",layerX);
    return TCL_ERROR;
  }

  layer = net->layer[layerX];

  if ((linkX < 0) || (linkX >= layer->linkN)) {
    ERROR("invalid link index %d for layer %d\n",linkX,layerX);
    return TCL_ERROR;
  }

  link = layer->link[linkX];
  if ((!link->eval) || (!link->update)) return TCL_OK;
  if (link->netFct != fmatrixMulFFT) {
    ERROR("Backprop only on projective nets (for now)\n");
    return TCL_ERROR;
  } 

  /* ---------------------------------- */
  /*  get pointer to input activations  */
  /* ---------------------------------- */
  if (link->sourceX < 0)
    srcAct = input;
  else
    srcAct = net->layer[link->sourceX]->act;

  /* ---------------------------------- */
  /*  compute weight matrix derivative  */
  /* ---------------------------------- */
  if (!link->dW) link->dW = fmatrixCreate(link->W->m,link->W->n);

  deltaT = fmatrixCreate(layer->delta->n,layer->delta->m);
  fmatrixTransFF(deltaT,layer->delta);

  if (srcAct->n == link->W->n)
    fmatrixMulFF(link->dW,deltaT,srcAct);
  else if (srcAct->n+1 == link->W->n)
    fmatrixMulFF1(link->dW,deltaT,srcAct);
  else
    ERROR("invalid size of source activation matrix\n");

  fmatrixScale(link->dW,link->dW,1.0/((double) srcAct->m));
  fmatrixDestroy(deltaT);


  /* ------------------------------ */
  /*  check for weight matrix mask  */
  /* ------------------------------ */
  if (link->mask) {
    if ((link->mask->m != link->dW->m) || 
        (link->mask->n != link->dW->n)) {
      ERROR("Weight matrix and mask matrix incompatible\n");
      return TCL_ERROR;
    }
    for (i=0; i<link->dW->m; i++)
      for (j=0; j<link->dW->n; i++)
        if (!link->mask->matPA[i][j]) link->dW->matPA[i][j] = 0.0;
  }

  return TCL_OK;
}



/* -------------------------------------------------------------------- */
/*  ffnetBackprop                       error backpropagation training  */
/*                                     for projective network function  */
/* -------------------------------------------------------------------- */
/*  Arguments:      net           Feed Forward Network                  */
/*                  input         Input Activation Matrix               */
/*                  target        Target Activation Matrix              */
/*                  weight        NULL or array of observation weights  */
/*                  noFwd         assume precomputed forward pass?      */
/*                  useMomentum   use momentum term in updates?         */
/*                  adapt         adapt learning rates                  */
/* -------------------------------------------------------------------- */
int  ffnetBackprop (FFNet *net, FMatrix *input, FMatrix *target,
                    float *weight, int noFwd, int useMomentum,
                    int adapt) {

  FMatrix         *output;
  FFLayer         *layer;
  FFLink          *link;
  register int    layerX,linkX;
  register float  gamma;


  /* ------------------------------------------------ */
  /*  check validity of net,input and target objects  */
  /* ------------------------------------------------ */
  assert(net);

  if (!input || (input->m * input->n == 0)) {
    ERROR("No input matrix supplied\n");
    return TCL_ERROR;
  }

  if (!target || (target->m * target->n == 0)) {
    ERROR("No target matrix supplied\n");
    return TCL_ERROR;
  }

  if (input->m != target->m) {
    ERROR("input pattern batch does not match target pattern batch\n");
    return TCL_ERROR;
  }

  if (input->n != net->inputN) {
    ERROR("input pattern size does not match network specs\n");
    return TCL_ERROR;
  }

  if (target->n != net->outputN) {
    ERROR("target pattern size does not match network specs\n");
    return TCL_ERROR;
  }

  if (net->layerN < 1) {
    ERROR("FFNet does not contain any layers - nothing to evaluate\n");
    return TCL_ERROR;
  }

  /* -------------------------------- */
  /*  if necessary, run forward pass  */
  /* -------------------------------- */
  if (!noFwd) 
    output = ffnetEval(net,input);
  else
    output = net->layer[0]->act;

  /* ------------------------------------------------------------- */
  /*  for all layers, compute delta's and weight matrix gradients  */
  /* ------------------------------------------------------------- */
  for (layerX=0; layerX<net->layerN; layerX++) {

    if (ffnetBPdelta(net,output,target,weight,layerX) == TCL_ERROR)
      return TCL_ERROR;

    layer = net->layer[layerX];

    /* ------------------------------------ */
    /*  for all links to the current layer  */
    /* ------------------------------------ */
    for (linkX=0; linkX<layer->linkN; linkX++) {

      /* ---------------------------------------- */
      /*  compute update matrix for current link  */
      /* ---------------------------------------- */
      if (ffnetBPgrad(net,input,layerX,linkX) == TCL_ERROR)
        return TCL_ERROR;
    }
  }

  /* ----------------------------------------------------- */
  /*  for all links in all layers: update weight matrices  */
  /* ----------------------------------------------------- */
  for (layerX=0; layerX<net->layerN; layerX++) {
    layer = net->layer[layerX];

    for (linkX=0; linkX<layer->linkN; linkX++) {
      link = layer->link[linkX];
      if ((link->eval) && (link->update)) {

        if (useMomentum) {
        
          /* ----------------------- */
          /*  momentum term updates  */
          /* ----------------------- */
          if ((link->alpha < 0.0) || (link->alpha >= 1.0)) {
            ERROR("invalid momentum factor alpha=%f\n",link->alpha);
            return TCL_ERROR;
          }

          if (!link->ddW) 
            link->ddW = fmatrixCreate(link->W->m,link->W->n);
          else
            fmatrixResize(link->ddW,link->W->m,link->W->n);

          fmatrixScale(link->ddW,link->ddW,link->alpha);
          fmatrixAccu(link->ddW,link->dW,1.0);
          fmatrixAccu(link->W,link->ddW,-1.0*link->eta);

        } else {

          /* -------------------------------- */
          /*  standard gradient-based update  */
          /* -------------------------------- */
          if (link->eta < 0.0) {
            ERROR("invalid learning rate eta=%f\n",link->eta);
            return TCL_ERROR;
          }
          fmatrixAccu(link->W,link->dW,-1.0*link->eta);

          /* -------------------------- */
          /*  learning rate adaptation  */
          /* -------------------------- */
          if (adapt) {
            if (link->ddW) {
              gamma = fmatrixCosineFF(link->dW,link->ddW);
              link->gamma *= link->alpha;
              link->gamma += gamma;

              if (link->gamma > 0.0) link->eta += ETA_INCREASE;
              if (link->gamma < 0.0) link->eta *= ETA_DECREASE;
            }

            /* ---------------------------------------------- */
            /*  save current update for next adaptation step  */
            /* ---------------------------------------------- */
            if (!link->ddW) 
              link->ddW = fmatrixCreate(link->W->m,link->W->n);
            else
              fmatrixResize(link->ddW,link->W->m,link->W->n);
            (void) fmatrixCopy(link->ddW,link->dW);
          }

        }

      }
    }
  }

  return  TCL_OK;
}



/* -------------------------------------------------------------------- */
/*  ffnetBackpropBatch                     wrapper for ffnetBackprop()  */
/* -------------------------------------------------------------------- */
int  ffnetBackpropBatch (FFNet *net, FMatrix *input, FMatrix *target,
                         float *weight, int shuffle, int useMomentum, 
                         int adapt, int iterN, int batchN, ErrFctPtr errFct,
                         char *logfile, FMatrix *logmat, int testAll) {

  FILE                 *fp = NULL,*lfp;
  FMatrix              *I,*T;
  float                *w;
  register float       error;
  register int         i,j,stepX,stepN,offset,iterX;
  char                 tmpFile[80] = "/tmp/log.ffnet";

  /* ------------------------------------------------ */
  /*  check validity of net,input and target objects  */
  /* ------------------------------------------------ */
  assert(net && input && target);

  if (input->m != target->m) {
    ERROR("input pattern batch does not match target pattern batch\n");
    return TCL_ERROR;
  }

  if (input->n != net->inputN) {
    ERROR("input pattern size does not match network specs\n");
    return TCL_ERROR;
  }

  if (target->n != net->outputN) {
    ERROR("target pattern size does not match network specs\n");
    return TCL_ERROR;
  }

  if (net->layerN < 1) {
    ERROR("FFNet does not contain any layers - nothing to evaluate\n");
    return TCL_ERROR;
  }

  if ((lfp = fileOpen(tmpFile,"w")) == NULL) {
    ERROR("Can not write standard logfile to %s\n",tmpFile);
    return TCL_ERROR;
  }

  if (logfile) {
    if (!strcmp(logfile,"stderr")) 
      fp = stderr;
    else
      fp = fileOpen(logfile,"w");
    fprintf(fp,"# =================================================\n");
    fprintf(fp,"#  janusNet Version %d.%d   training log\n",
                MAJOR_VERSION,MINOR_VERSION);
    fprintf(fp,"# -------------------------------------------------\n");
    fprintf(fp,"#  patternN    %d\n",input->m);
    fprintf(fp,"#  batchN      %d\n",batchN);
    fprintf(fp,"#  iterationN  %d\n",iterN);
    fprintf(fp,"#  momentum    %d\n",useMomentum);
    fprintf(fp,"#  adapt       %d\n",adapt);
    fprintf(fp,"# =================================================\n\n");
    fprintf(fp,"set LOG {\\\n");
    fflush(fp);
  }

  if ((batchN <= 0) || (batchN >= input->m)) {

    /* ---------------------------------- */
    /*  full batch/true-gradient updates  */
    /* ---------------------------------- */
    if (logmat) fmatrixResize(logmat,1,iterN);
    for (iterX=0; iterX<iterN; iterX++) {

      if (shuffle) ffnetShuffle(net,input,target);

      ffnetBackprop(net,input,target,weight,0,useMomentum,adapt);
      if (fp || logmat) {
        error = ffnetError(net,input,target,errFct,0);
        if (fp) {
          fprintf(fp,"{ %d %d %f }\\\n",iterX,1,error);
          fflush(fp);
        }
        if (logmat) logmat->matPA[0][iterX] = error;
        fprintf(lfp,"%d %f\n",iterX,error); fflush(lfp);
      }
    }

  } else {

    /* ------------------------------------- */
    /*  on-line/stochastic gradient updates  */
    /* ------------------------------------- */
    I = fmatrixCreate(batchN,input->n);
    T = fmatrixCreate(batchN,target->n);
    if (weight)
      w = (float *) malloc (sizeof(float)*batchN);
    else
      w = NULL;

    stepN = input->m / batchN;

    if (logmat) {
      if (testAll) fmatrixResize(logmat,iterN,stepN);
            else   fmatrixResize(logmat,1,iterN);
    }

    for (iterX=0; iterX<iterN; iterX++) {

      if (shuffle) ffnetShuffle(net,input,target);

      for (stepX=1, offset=0; stepX<=stepN; stepX++, offset+=batchN) {
      
        /* ---------------------------------------------------- */
        /*  create batches for input,target and weight objects  */
        /* ---------------------------------------------------- */
        for (i=0; i<batchN; i++)
          for (j=0; j<input->n; j++)
            I->matPA[i][j] = input->matPA[offset+i][j];

        for (i=0; i<batchN; i++)
          for (j=0; j<target->n; j++)
            T->matPA[i][j] = target->matPA[offset+i][j];

        if (w)
          for (i=0; i<batchN; i++)
            w[i] = weight[offset+i];

        /* ----------------------- */
        /*  perform backprop step  */
        /* ----------------------- */
        (void) ffnetEval(net,I);
        ffnetBackprop(net,I,T,w,1,useMomentum,adapt);

        if ((fp || logmat) && testAll) {
          error = ffnetError(net,input,target,errFct,0);
          if (fp) {
            fprintf(fp,"{ %d %d %f }\\\n",iterX,stepX,error);
            fflush(fp);
          }
          if (logmat) logmat->matPA[iterX][stepX] = error;
        }
      }

      if ((fp || logmat) && !testAll) {
        error = ffnetError(net,input,target,errFct,0);
        if (fp) {
          fprintf(fp,"{ %d %d %f }\\\n",iterX,0,error);
          fflush(fp);
        }
        if (logmat) logmat->matPA[0][iterX] = error;
        fprintf(lfp,"%d %f\n",iterX,error); fflush(lfp);
      }

    }

    fmatrixDestroy(I);
    fmatrixDestroy(T);
    if (w) free(w);
  }

  if (fp) {
    fprintf(fp,"}\n");
    if (strcmp(logfile,"stderr") != 0) fileClose(logfile, fp);
  }

  if (lfp) fileClose(tmpFile, lfp);

  return TCL_OK;
}



/* ==================================================================== */
/*                                                                      */
/*              PART IV : network structure/weight I/O                  */
/*                                                                      */
/* ==================================================================== */



/* -------------------------------------------------------------------- */
/*  ffnetWrite                                                          */
/* -------------------------------------------------------------------- */
int  ffnetWrite (FFNet *net, FILE *fp) {

  FFLayer          *layer;
  FFLink           *link;
  register int     layerX,linkX;
  char             *actName = NULL;

  for (layerX=0; layerX<net->layerN; layerX++) {
    layer = net->layer[layerX];
    actFct2char(layer->actFct,layer->actDerFct,&actName);
    fprintf(fp,"layer %d %s\n",layer->act->n,actName);
  }
  
  for (layerX=0; layerX<net->layerN; layerX++) {
    layer = net->layer[layerX];
    for (linkX=0; linkX<layer->linkN; linkX++) {
      link = layer->link[linkX];
      fprintf(fp,"link %d %d\n",layerX,link->sourceX);
    }
  }

  return TCL_OK;
}



/* -------------------------------------------------------------------- */
/*  fflinkSave                                                          */
/* -------------------------------------------------------------------- */
int  fflinkSave (FFLink *link, FILE *fp, int version) {

  char             *name = NULL;

  assert(link);

  switch (version) {
    case  1: /* -------------------------------- */
             /*  file I/O routine for version 1  */
             /* -------------------------------- */
             write_int(fp,link->sourceX);

             if (link->W) {
               write_int(fp,1);
               fmatrixSave(link->W,fp);
             } else write_int(fp,0);

             if (link->mask) {
               write_int(fp,1);
               imatrixSave(link->mask,fp);
             } else write_int(fp,0);

             netFct2char(link->netFct,&name);
             write_string(fp,name);

             if (link->dW) {
               write_int(fp,1);
               fmatrixSave(link->dW,fp);
             } else write_int(fp,0);

             if (link->ddW) {
               write_int(fp,1);
               fmatrixSave(link->ddW,fp);
             } else write_int(fp,0);

             write_int(fp,link->eval);
             write_int(fp,link->update);
             write_float(fp,link->eta);
             write_float(fp,link->alpha);
             write_float(fp,link->gamma);
             break;

    default: ERROR("invalid version number %d\n",version);
             return TCL_ERROR;
  }

  return TCL_OK;
}



/* -------------------------------------------------------------------- */
/*  fflinkLoad                                                          */
/* -------------------------------------------------------------------- */
int  fflinkLoad (FFLink *link, FILE *fp, int version) {

  char            name[256];

  assert(link);

  switch (version) {
    case  1: /* -------------------------------- */
             /*  file I/O routine for version 1  */
             /* -------------------------------- */
             link->sourceX = read_int(fp);
           
             if (read_int(fp)) {
               if (!link->W) link->W = fmatrixCreate(1,1);
               fmatrixLoadFF(link->W,fp);
             }

             if (read_int(fp)) {
               if (!link->mask) link->mask = imatrixCreate(1,1);
               imatrixLoad(link->mask,fp);
             }

             if (read_string(fp,name) == 0) return TCL_ERROR;
             char2netFct(name,&link->netFct);

             if (read_int(fp)) {
               if (!link->dW) link->dW = fmatrixCreate(1,1);
               fmatrixLoadFF(link->dW,fp);
             }
             
             if (read_int(fp)) {
               if (!link->ddW) link->ddW = fmatrixCreate(1,1);
               fmatrixLoadFF(link->ddW,fp);
             }

             link->eval   = read_int(fp);
             link->update = read_int(fp);
             link->eta    = read_float(fp);
             link->alpha  = read_float(fp);
             link->gamma  = read_float(fp);
             break;

    default: ERROR("invalid version number %d\n",version);
             return TCL_ERROR;
  }

  return TCL_OK;
}



/* -------------------------------------------------------------------- */
/*  fflayerSave                                                         */
/* -------------------------------------------------------------------- */
int  fflayerSave (FFLayer *layer, FILE *fp, int version) {

  register int      linkX;
  char              *name;

  assert(layer);

  switch (version) {
    case  1: /* -------------------------------- */
             /*  file I/O routine for version 1  */
             /* -------------------------------- */
             if (layer->act) {
               write_int(fp,1);
               fmatrixSave(layer->act,fp);
             } else write_int(fp,0);

             actFct2char(layer->actFct,layer->actDerFct,&name);
             write_string(fp,name);

             if (layer->actDer) {
               write_int(fp,1);
               fmatrixSave(layer->actDer,fp);
             } else write_int(fp,0);

             if (layer->delta) {
               write_int(fp,1);
               fmatrixSave(layer->delta,fp);
             } else write_int(fp,0);

             write_int(fp,layer->linkN);
             for (linkX=0; linkX<layer->linkN; linkX++)
               fflinkSave(layer->link[linkX],fp,version);
             break;

    default: ERROR("invalid version number %d\n",version);
             return TCL_ERROR;
  }

  return TCL_OK;
}



/* -------------------------------------------------------------------- */
/*  fflayerLoad                                                         */
/* -------------------------------------------------------------------- */
int  fflayerLoad (FFLayer *layer, FILE *fp, int version) {

  register int      linkX;
  char              name[255];

  assert(layer);

  switch (version) {
    case  1: /* -------------------------------- */
             /*  file I/O routine for version 1  */
             /* -------------------------------- */
             fflayerDeinit(layer);

             if (read_int(fp)) {
               if (!layer->act) layer->act = fmatrixCreate(1,1);
               fmatrixLoadFF(layer->act,fp);
             }

             if (read_string(fp,name) == 0) return TCL_ERROR;
             char2actFct(name,&layer->actFct,&layer->actDerFct);

             if (read_int(fp)) {
               if (!layer->actDer) layer->actDer = fmatrixCreate(1,1);
               fmatrixLoadFF(layer->actDer,fp);
             }

             if (read_int(fp)) {
               if (!layer->delta) layer->delta = fmatrixCreate(1,1);
               fmatrixLoadFF(layer->delta,fp);
             }

             layer->linkN = read_int(fp);
             layer->link = (FFLink **) malloc 
                           (sizeof(FFLink *) * layer->linkN);
             for (linkX=0; linkX<layer->linkN; linkX++) {
               layer->link[linkX] = fflinkCreate(1,2,2,NULL);
               fflinkLoad(layer->link[linkX],fp,version);
             }
             break;

    default: ERROR("invalid version number %d\n",version);
             return TCL_ERROR;
  }

  return TCL_OK;
}



/* -------------------------------------------------------------------- */
/*  ffnetSave                                                           */
/* -------------------------------------------------------------------- */
int  ffnetSave (FFNet *net, FILE *fp) {

  register int          version,layerX;
  char                  *name;

  assert(net);
  ffnetReset(net);

  version = MAJOR_VERSION;
  write_int(fp,version);

  switch (version) {
    case  1: /* -------------------------------- */
             /*  file I/O routine for version 1  */
             /* -------------------------------- */
             write_int(fp,net->outputN);
             write_int(fp,net->inputN);

             errFct2char(net->errFct,net->errDerFct,&name);
             write_string(fp,name);

             write_int(fp,net->layerN);
             for (layerX=0; layerX<net->layerN; layerX++)
               fflayerSave(net->layer[layerX],fp,version);
             break;

    default: ERROR("invalid version number %d\n",version);
             return TCL_ERROR;
  }

  return TCL_OK;
}



/* -------------------------------------------------------------------- */
/*  ffnetSaveTcl                                                        */
/* -------------------------------------------------------------------- */
int  ffnetSaveTcl (FFNet *net, FILE *fp) {

  FFLayer               *layer;
  FFLink                *link;
  register int          layerX,linkX;

  assert(net);

  for (layerX=0; layerX<net->layerN; layerX++) {
    layer = net->layer[layerX];
    for (linkX=0; linkX<layer->linkN; linkX++) {
      link = layer->link[linkX];
      fprintf(fp,"%s.layer(%d).link(%d).W := {\\\n",net->name,layerX,linkX);
      fmatrixPuts(link->W,fp);
      fprintf(fp,"}\n");
      if (link->mask) {
        fprintf(fp,"%s.layer(%d).link(%d).mask := {\\\n",net->name,
                layerX,linkX);
        imatrixPuts(link->mask,fp);
        fprintf(fp,"\\\n");
      }
    }
  }

  return TCL_OK;
}



/* -------------------------------------------------------------------- */
/*  ffnetLoad                                                           */
/* -------------------------------------------------------------------- */
int  ffnetLoad (FFNet *net, FILE *fp) {

  register int            version,layerX;
  char                    name[255];

  assert(net);
  ffnetReset(net);

  version = read_int(fp);

  switch (version) {
    case  1: /* -------------------------------- */
             /*  file I/O routine for version 1  */
             /* -------------------------------- */
             ffnetDeinit(net);

             net->outputN = read_int(fp);
             net->inputN  = read_int(fp);

             if (read_string(fp,name) == 0) return TCL_ERROR;
             char2errFct(name,&net->errFct,&net->errDerFct);

             net->layerN = read_int(fp);
             net->layer = (FFLayer **) malloc
                          (sizeof(FFLayer *) * net->layerN);
             for (layerX=0; layerX<net->layerN; layerX++) {
               net->layer[layerX] = fflayerCreate(1,NULL,NULL);
               fflayerLoad(net->layer[layerX],fp,version);
             }
             break;

    default: ERROR("invalid version number %d\n",version);
             return TCL_ERROR;
  }

  return TCL_OK;
}



/* ==================================================================== */
/*                                                                      */
/*                      PART V : JRTk interface                         */
/*                                                                      */
/* ==================================================================== */




/* ==================================================================== */
/*                           Type: FFLink                               */
/* ==================================================================== */


/* -------------------------------------------------------------------- */
/*  fflinkPutsItf                                                       */
/* -------------------------------------------------------------------- */
int  fflinkPutsItf (ClientData cd, int argc, char *argv[]) {

  FFLink          *link = (FFLink *) cd;
  char            *netFctName = NULL;

  netFct2char(link->netFct,&netFctName);

  itfAppendResult("{sourceX %d} {netFct %s} {eval %d} {update %d} ",
       link->sourceX,netFctName,link->eval,link->update);

  itfAppendResult("{eta %f} {alpha %f} {gamma %f} ",
       link->eta,link->alpha,link->gamma);

  return TCL_OK;
}



/* -------------------------------------------------------------------- */
/*  fflinkConfigureItf                                                  */
/* -------------------------------------------------------------------- */
int  fflinkConfigureItf (ClientData cd, char *var, char *val) {

  FFLink        *link = (FFLink *) cd;

  if (! var) {
    ITFCFG(fflinkConfigureItf,cd,"sourceX");
    ITFCFG(fflinkConfigureItf,cd,"eval");
    ITFCFG(fflinkConfigureItf,cd,"update");
    ITFCFG(fflinkConfigureItf,cd,"eta");
    ITFCFG(fflinkConfigureItf,cd,"alpha");
    ITFCFG(fflinkConfigureItf,cd,"gamma");
    return TCL_OK;
  }
  ITFCFG_Int     (var,val,"sourceX",     link->sourceX ,       0);
  ITFCFG_Int     (var,val,"eval",        link->eval ,          0);
  ITFCFG_Int     (var,val,"update",      link->update ,        0);
  ITFCFG_Float   (var,val,"eta",         link->eta ,           0);
  ITFCFG_Float   (var,val,"alpha",       link->alpha ,         0);
  ITFCFG_Float   (var,val,"gamma",       link->gamma ,         1);

  return TCL_OK;
}



/* -------------------------------------------------------------------- */
/*  fflinkAccessItf                                                     */
/* -------------------------------------------------------------------- */
ClientData  fflinkAccessItf (ClientData cd, char *name, TypeInfo **ti) {

  FFLink          *link = (FFLink *) cd;

  if (*name=='.') {
    if (name[1]=='\0') {
      if (link->W)    itfAppendResult("W ");
      if (link->mask) itfAppendResult("mask ");
      if (link->dW)   itfAppendResult("dW ");
      if (link->ddW)  itfAppendResult("ddW ");
    }
    else {
      if ((link->W) && (strcmp(name+1,"W")==0)) {
        *ti = itfGetType("FMatrix");
        return (ClientData) link->W;
      }
      if ((link->mask) && (strcmp(name+1,"mask")==0)) {
        *ti = itfGetType("IMatrix");
        return (ClientData) link->mask;
      }
      if ((link->dW) && (strcmp(name+1,"dW")==0)) {
        *ti = itfGetType("FMatrix");
        return (ClientData) link->dW;
      }
      if ((link->ddW) && (strcmp(name+1,"ddW")==0)) {
        *ti = itfGetType("FMatrix");
        return (ClientData) link->ddW;
      }
    }
  }
  *ti=NULL;
  return NULL;
}



/* -------------------------------------------------------------------- */
/*  fflinkMaskItf                                                       */
/* -------------------------------------------------------------------- */
int  fflinkMaskItf (ClientData cd, int argc, char *argv[]) {

  FFLink     *link = (FFLink *) cd;

  if ( argc != 2) {
    ERROR("FFLink method 'mask' expects 1 matrix parameter\n");
    return TCL_ERROR;
  }

  if (link->mask) {
    imatrixFree(link->mask);
    link->mask = NULL;
  }

  if (!(link->mask = imatrixGetItf(argv[1]))) return TCL_ERROR;

  if ((link->mask->m != link->W->m) || (link->mask->n != link->W->n)) {
    ERROR("link mask does not fit on weight matrix\n");
    imatrixFree(link->mask);
    link->mask = NULL;
  }

  return TCL_OK;
}



/* -------------------------------------------------------------------- */
/*  fflinkRandomItf                                                     */
/* -------------------------------------------------------------------- */
int  fflinkRandomItf (ClientData cd, int argc, char *argv[]) {

  FFLink     *link = (FFLink *) cd;
  float      range = -1.0;
  int        ac    = argc-1;

  if ( itfParseArgv( argv[0], &ac, argv+1, 0,
    "-range",  ARGV_FLOAT,    NULL, &range,  NULL, "absolute range of random values",
  NULL) != TCL_OK) return TCL_ERROR;

  if (range < 0.0)
    fmatrixInitRandom(link->W,2.0/((float) link->W->n));
  else
    fmatrixInitRandom(link->W,range);

  return TCL_OK;
}




/* -------------------------------------------------------------------- */
/*  fflinkIdentItf                                                      */
/* -------------------------------------------------------------------- */
int  fflinkIdentItf (ClientData cd, int argc, char *argv[]) {

  FFLink     *link = (FFLink *) cd;

  return fmatrixInitDiag(link->W,1.0);
}



static Method fflinkMethod[] = 
{
  { "puts",        fflinkPutsItf,        NULL},
  { "mask",        fflinkMaskItf,        "set weight matrix mask" },
  { "randomize",   fflinkRandomItf,      "randomize weights uniformly" },
  { "identity",    fflinkIdentItf,       "identity transform" },
  {  NULL,  NULL, NULL }
};


TypeInfo fflinkInfo = {
                  "FFLink", 0, -1, fflinkMethod, 
                  NULL, NULL,
                  fflinkConfigureItf, fflinkAccessItf,
                  itfTypeCntlDefaultNoLink,
                  "Single link between FFLayer's in a FFNet\n" } ;



/* ==================================================================== */
/*                           Type: FFLayer                              */
/* ==================================================================== */


/* -------------------------------------------------------------------- */
/*  fflayerPutsItf                                                      */
/* -------------------------------------------------------------------- */
int  fflayerPutsItf (ClientData cd, int argc, char *argv[]) {

  FFLayer          *layer = (FFLayer *) cd;
  char             *actFctName = NULL;

  actFct2char(layer->actFct,layer->actDerFct,&actFctName);

  itfAppendResult("{neuronN %d} {actFct %s} {linkN %d}",
        layer->act->n,actFctName,layer->linkN);

  return TCL_OK;
}



/* -------------------------------------------------------------------- */
/*  fflayerConfigureItf                                                 */
/* -------------------------------------------------------------------- */
int  fflayerConfigureItf (ClientData cd, char *var, char *val) {
  
  FFLayer        *layer = (FFLayer *) cd;

  if (! var) {
    ITFCFG(fflayerConfigureItf,cd,"neuronN");
    ITFCFG(fflayerConfigureItf,cd,"linkN");
    return TCL_OK;
  }
  ITFCFG_Int     (var,val,"neuronN",   layer->act->n,       1);
  ITFCFG_Int     (var,val,"linkN",     layer->linkN ,       1);

  return TCL_OK;
}



/* -------------------------------------------------------------------- */
/*  fflayerAccessItf                                                    */
/* -------------------------------------------------------------------- */
ClientData  fflayerAccessItf (ClientData cd, char *name, TypeInfo **ti) {

  FFLayer          *layer = (FFLayer *) cd;
  int              idx;

  if (*name=='.') {
    if (name[1]=='\0') {
      if (layer->act)    itfAppendResult("act ");
      if (layer->actDer) itfAppendResult("actDer ");
      if (layer->delta)  itfAppendResult("delta ");
      if (layer->linkN > 0)
        itfAppendResult("link(0..%d) ",layer->linkN-1);
    }
    else {
      if ((layer->act) && (strcmp(name+1,"act")==0)) {
        *ti = itfGetType("FMatrix");
        return (ClientData) layer->act;
      }
      if ((layer->actDer) && (strcmp(name+1,"actDer")==0)) {
        *ti = itfGetType("FMatrix");
        return (ClientData) layer->actDer;
      }
      if ((layer->delta) && (strcmp(name+1,"delta")==0)) {
        *ti = itfGetType("FMatrix");
        return (ClientData) layer->delta;
      }
      if ((layer->linkN > 0) &&
          (sscanf(name+1,"link(%d)",&idx)==1)) {
        *ti = itfGetType("FFLink");
        if ((idx >= 0) && (idx < layer->linkN))
          return (ClientData) layer->link[idx];
      }
    }
  }
  *ti=NULL;
  return NULL;
}



/* -------------------------------------------------------------------- */
/*  fflayerConfLinksItf                                                 */
/* -------------------------------------------------------------------- */
int  fflayerConfLinksItf (ClientData cd, int argc, char *argv[]) {

  FFLayer           *layer = (FFLayer *) cd;
  FFLink            *link;
  register int      k,linkX;
  char              *var,*val;

  if ((argc % 2) != 1) {
    ERROR("invalid number of parameters\n");
    return TCL_ERROR;
  }
  
  for (k=1; k<argc; k+=2) {
    var = argv[k]+1;
    val = argv[k+1];
    for (linkX=0; linkX<layer->linkN; linkX++) {
      link = layer->link[linkX];
      if (fflinkConfigureItf((ClientData) link,var,val) == TCL_ERROR) 
        return TCL_ERROR;
    }
  }

  return TCL_OK;
}



/* -------------------------------------------------------------------- */
/*  fflayerRandomItf                                                    */
/* -------------------------------------------------------------------- */
int  fflayerRandomItf (ClientData cd, int argc, char *argv[]) {

  FFLayer    *layer = (FFLayer *) cd;
  FFLink     *link;
  float      range = -1.0;
  int        ac    = argc-1;
  int        linkX;

  if ( itfParseArgv( argv[0], &ac, argv+1, 0,
    "-range",  ARGV_FLOAT,    NULL, &range,  NULL, "absolute range of random values",
  NULL) != TCL_OK) return TCL_ERROR;

  for (linkX=0; linkX<layer->linkN; linkX++) {
    link = layer->link[linkX];

    if (range < 0.0)
      fmatrixInitRandom(link->W,2.0/((float) link->W->n));
    else
      fmatrixInitRandom(link->W,range);
  }

  return TCL_OK;
}



static Method fflayerMethod[] = 
{
  { "puts",        fflayerPutsItf,        NULL},
  { "confLinks",   fflayerConfLinksItf,   "configure all links identically" },
  { "randomize",   fflayerRandomItf,      "randomize all links uniformly" },
  {  NULL,  NULL, NULL }
};


TypeInfo fflayerInfo = {
                  "FFLayer", 0, -1, fflayerMethod, 
                  NULL, NULL,
                  fflayerConfigureItf, fflayerAccessItf,
                  itfTypeCntlDefaultNoLink,
                  "Single Layer in a FFNet\n" } ;



/* ==================================================================== */
/*                            Type: FFNet                               */
/* ==================================================================== */


/* -------------------------------------------------------------------- */
/*  ffnetCreateItf                                                      */
/* -------------------------------------------------------------------- */
ClientData  ffnetCreateItf (ClientData cd, int argc, char *argv[]) {

  int              ac        = argc-1;
  int              outputN   = 0;
  int              inputN    = 0;
  char             *errName  = NULL;
  ErrFctPtr        errFct    = NULL;
  ErrDerFctPtr     errDerFct = NULL;

  if ( itfParseArgv( argv[0], &ac, argv+1, 1,
      "<outputN>", ARGV_INT,    NULL, &outputN,  NULL, "size of output layer",
      "<inputN>",  ARGV_INT,    NULL, &inputN,   NULL, "size of input layer",
      "<errFct>",  ARGV_STRING, NULL, &errName,  NULL, "net error function",
  NULL) != TCL_OK) return NULL;

  if (char2errFct(errName,&errFct,&errDerFct) == TCL_ERROR)
    return NULL;

  return (ClientData) ffnetCreate(argv[0],outputN,inputN,errFct,errDerFct);
}



/* -------------------------------------------------------------------- */
/*  ffnetFreeItf                                                        */
/* -------------------------------------------------------------------- */
int  ffnetFreeItf (ClientData cd) {

  return ffnetDestroy((FFNet *) cd);
}



/* -------------------------------------------------------------------- */
/*  ffnetPutsItf                                                        */
/* -------------------------------------------------------------------- */
int  ffnetPutsItf (ClientData cd, int argc, char *argv[]) {

  FFNet      *net  = (FFNet *) cd;
  char       *errName = NULL;

  errFct2char(net->errFct,net->errDerFct,&errName);

  itfAppendResult("{name %s} {outputN %d} {inputN %d} ",
    net->name,net->outputN,net->inputN);

  itfAppendResult("{errFct %s} {layerN %d} ",
    errName,net->layerN);

  return TCL_OK;
}



/* -------------------------------------------------------------------- */
/*  ffnetConfigureItf                                                   */
/* -------------------------------------------------------------------- */
int  ffnetConfigureItf (ClientData cd, char *var, char *val) {

  FFNet        *net = (FFNet *) cd;

  if (! var) {
    ITFCFG(ffnetConfigureItf,cd,"name");
    ITFCFG(ffnetConfigureItf,cd,"useN");
    ITFCFG(ffnetConfigureItf,cd,"outputN");
    ITFCFG(ffnetConfigureItf,cd,"inputN");
    ITFCFG(ffnetConfigureItf,cd,"layerN");
    return TCL_OK;
  }
  ITFCFG_CharPtr (var,val,"name",       net->name ,       1);
  ITFCFG_Int     (var,val,"useN",       net->useN ,       1);
  ITFCFG_Int     (var,val,"outputN",    net->outputN ,    1);
  ITFCFG_Int     (var,val,"inputN",     net->inputN ,     1);
  ITFCFG_Int     (var,val,"layerN",     net->layerN ,     1);

  return TCL_ERROR;
}



/* -------------------------------------------------------------------- */
/*  ffnetAccessItf                                                      */
/* -------------------------------------------------------------------- */
ClientData  ffnetAccessItf (ClientData cd, char *name, TypeInfo **ti) {

  FFNet            *net = (FFNet *) cd;
  int              idx;

  if (*name=='.') {
    if (name[1]=='\0') {
      if (net->layerN > 0) 
        itfAppendResult("layer(0..%d) ",net->layerN-1);
    }
    else { 
      if ((net->layerN > 0) &&
          (sscanf(name+1,"layer(%d)",&idx)==1)) {
        *ti = itfGetType("FFLayer");
        if ((idx >= 0) && (idx < net->layerN))
          return (ClientData) net->layer[idx];
      }
    }
  }
  *ti=NULL;
  return NULL;
}



/* -------------------------------------------------------------------- */
/*  ffnetAddLinkItf                                                     */
/* -------------------------------------------------------------------- */
int  ffnetAddLinkItf (ClientData cd, int argc, char *argv[]) {
 
  FFNet         *net     = (FFNet *) cd;
  int           ac       = argc-1;
  int           targetX  = 0;
  int           sourceX  = -1;
  char          *fctName = NULL;
  NetFctPtr     netFct   = NULL;

  if ( itfParseArgv( argv[0], &ac, argv+1, 1,
      "<targetX>",  ARGV_INT,    NULL, &targetX,  NULL, "target layer index",
      "<sourceX>",  ARGV_INT,    NULL, &sourceX,  NULL, "source layer index",
      "-netFct",    ARGV_STRING, NULL, &fctName,  NULL, "net function name",
  NULL) != TCL_OK) return TCL_ERROR;

  if (fctName == NULL)
    netFct = fmatrixMulFFT;
  else
    if (char2netFct(fctName,&netFct) == TCL_ERROR) return TCL_ERROR;

  return ffnetAddLink(net,targetX,sourceX,netFct);
}



/* -------------------------------------------------------------------- */
/*  ffnetAddLayerItf                                                    */
/* -------------------------------------------------------------------- */
int  ffnetAddLayerItf (ClientData cd, int argc, char *argv[]) {

  FFNet         *net      = (FFNet *) cd;
  int           ac        = argc-1;
  int           neuronN   = 1;
  char          *fctName  = NULL;
  ActFctPtr     actFct    = NULL;
  ActDerFctPtr  actDerFct = NULL;

  if ( itfParseArgv( argv[0], &ac, argv+1, 1,
      "<neuronN>",  ARGV_INT,    NULL, &neuronN,  NULL, "size of layer",
      "<actFct>",   ARGV_STRING, NULL, &fctName,  NULL, "name of activation function",
  NULL) != TCL_OK) return TCL_ERROR;

  if (char2actFct(fctName,&actFct,&actDerFct) == TCL_ERROR) 
    return TCL_ERROR;

  return ffnetAddLayer(net,neuronN,actFct,actDerFct);
}



/* -------------------------------------------------------------------- */
/*  ffnetAddItf                                                         */
/* -------------------------------------------------------------------- */
int  ffnetAddItf (ClientData cd, int argc, char *argv[]) {

  int          ac = (argc<2)?(argc-1):1;
  char         **av = argv;
  char         *type = NULL;

  if ( itfParseArgv( argv[0], &ac, argv+1, 1,
      "<type>", ARGV_STRING,    NULL, &type,  NULL, "type: layer or link",
  NULL) != TCL_OK) return TCL_ERROR;

  if (strcmp(type,"layer") == 0)
    return ffnetAddLayerItf(cd,argc-1,av+1);
  if (strcmp(type,"link")  == 0)
    return ffnetAddLinkItf(cd,argc-1,av+1);

  ERROR("Unknown type (not layer, not link): nothing added\n");
  return TCL_ERROR;
}



/* -------------------------------------------------------------------- */
/*  ffnetWriteItf                                                       */
/* -------------------------------------------------------------------- */
int  ffnetWriteItf (ClientData cd, int argc, char *argv[]) {

  FFNet            *net = (FFNet *) cd;
  int              ac = argc-1;
  char             *filename;
  FILE             *fp;
  
  if ( itfParseArgv (argv[0], &ac, argv+1, 1,
      "<filename>", ARGV_STRING, NULL, &filename, NULL, "filename",
      NULL) != TCL_OK) return TCL_ERROR;

  if ((fp = fileOpen(filename,"w")) == NULL) {
    ERROR("can not open file '%s'\n",filename);
    return TCL_ERROR;
  }

  if (ffnetWrite(net,fp) == TCL_ERROR) return TCL_ERROR;

  fileClose(filename,fp);

  return TCL_OK;
}



/* -------------------------------------------------------------------- */
/*  ffnetReadItf                                                        */
/* -------------------------------------------------------------------- */
int  ffnetReadItf (ClientData cd, int argc, char *argv[]) {

  FFNet            *net = (FFNet *) cd;
  int              ac = argc-1;
  char             *filename;
 
  if ( itfParseArgv (argv[0], &ac, argv+1, 1,
      "<filename>", ARGV_STRING, NULL, &filename, NULL, "filename",
      NULL) != TCL_OK) return TCL_ERROR;

  return ffnetRead(net,filename);
}



/* -------------------------------------------------------------------- */
/*  ffnetSaveItf                                                        */
/* -------------------------------------------------------------------- */
int  ffnetSaveItf (ClientData cd, int argc, char *argv[]) {

  FFNet            *net = (FFNet *) cd;
  int              ac = argc-1;
  int              tcl = 0;
  char             *filename;
  FILE             *fp;
  
  if ( itfParseArgv (argv[0], &ac, argv+1, 1,
      "<filename>", ARGV_STRING, NULL, &filename, NULL, "filename",
      "-tcl",       ARGV_INT,    NULL, &tcl,      NULL, "save in tcl-ascii format",
      NULL) != TCL_OK) return TCL_ERROR;

  if ((fp = fileOpen(filename,"w")) == NULL) {
    ERROR("can not open file '%s'\n",filename);
    return TCL_ERROR;
  }

  if (tcl) {
    if (ffnetSaveTcl(net,fp) == TCL_ERROR) return TCL_ERROR;
  } else {
    if (ffnetSave(net,fp) == TCL_ERROR) return TCL_ERROR;
  }

  fileClose(filename,fp);

  return TCL_OK;
}



/* -------------------------------------------------------------------- */
/*  ffnetLoadItf                                                        */
/* -------------------------------------------------------------------- */
int  ffnetLoadItf (ClientData cd, int argc, char *argv[]) {

  FFNet            *net = (FFNet *) cd;
  int              ac = argc-1;
  char             *filename;
  FILE             *fp;
  
  if ( itfParseArgv (argv[0], &ac, argv+1, 1,
      "<filename>", ARGV_STRING, NULL, &filename, NULL, "filename",
      NULL) != TCL_OK) return TCL_ERROR;

  if ((fp = fileOpen(filename,"r")) == NULL) {
    ERROR("can not open file '%s'\n",filename);
    return TCL_ERROR;
  }

  if (ffnetLoad(net,fp) == TCL_ERROR) return TCL_ERROR;

  fileClose(filename,fp);

  return TCL_OK;
}



/* -------------------------------------------------------------------- */
/*  ffnetCopyItf                                                        */
/* -------------------------------------------------------------------- */
int  ffnetCopyItf (ClientData cd, int argc, char *argv[]) {

  FFNet        *net    = (FFNet *) cd;
  FFNet        *source = NULL;  
  int          ac      = argc-1;

  if ( itfParseArgv( argv[0], &ac, argv+1, 1,
      "<source>",  ARGV_OBJECT,  NULL, &source, "FFNet", "source network",
  NULL) != TCL_OK) return TCL_ERROR;

  return ffnetCopy(net,source);
}



/* -------------------------------------------------------------------- */
/*  ffnetConfLinksItf                                                   */
/* -------------------------------------------------------------------- */
int  ffnetConfLinksItf (ClientData cd, int argc, char *argv[]) {

  FFNet             *net = (FFNet *) cd;
  FFLayer           *layer;
  FFLink            *link;
  register int      k,layerX,linkX;
  char              *var,*val;

  if ((argc % 2) != 1) {
    ERROR("invalid number of parameters\n");
    return TCL_ERROR;
  }
  
  for (k=1; k<argc; k+=2) {
    var = argv[k]+1;
    val = argv[k+1];
    for (layerX=0; layerX<net->layerN; layerX++) {
      layer = net->layer[layerX];
      for (linkX=0; linkX<layer->linkN; linkX++) {
        link = layer->link[linkX];
        if (fflinkConfigureItf((ClientData) link,var,val) == TCL_ERROR) return TCL_ERROR;
      }
    }
  }

  return TCL_OK;
}



/* -------------------------------------------------------------------- */
/*  ffnetRandomItf                                                      */
/* -------------------------------------------------------------------- */
int  ffnetRandomItf (ClientData cd, int argc, char *argv[]) {

  FFNet      *net = (FFNet *) cd;
  FFLayer    *layer;
  FFLink     *link;
  float      range = -1.0;
  int        ac    = argc-1;
  int        layerX,linkX;

  if ( itfParseArgv( argv[0], &ac, argv+1, 1,
    "-range",  ARGV_FLOAT,    NULL, &range,  NULL, "absolute range of random values",
  NULL) != TCL_OK) return TCL_ERROR;

  for (layerX=0; layerX<net->layerN; layerX++) {
    layer = net->layer[layerX];
    for (linkX=0; linkX<layer->linkN; linkX++) {
      link = layer->link[linkX];

      if (range < 0.0)
        fmatrixInitRandom(link->W,2.0/((float) link->W->n));
      else
        fmatrixInitRandom(link->W,range);
    }
  }

  return TCL_OK;
}



/* -------------------------------------------------------------------- */
/*  ffnetResetItf                                                       */
/* -------------------------------------------------------------------- */
int  ffnetResetItf (ClientData cd, int argc, char *argv[]) {

  FFNet       *net = (FFNet *) cd;
  
  return ffnetReset(net);
}



/* -------------------------------------------------------------------- */
/*  ffnetEvalItf                                                        */
/* -------------------------------------------------------------------- */
int  ffnetEvalItf (ClientData cd, int argc, char *argv[]) {

  FFNet           *net      = (FFNet *) cd;
  int             ac        = argc-1;
  FMatrix         *op;
  FMatrix         *input    = NULL;
  FMatrix         *output   = NULL;
  FMatrix         *target   = NULL;
  ErrFctPtr       errFct    = NULL;
  ErrDerFctPtr    errDerFct = NULL;
  char            *errName  = NULL;

  if ( itfParseArgv( argv[0], &ac, argv+1, 1,
      "<input>",  ARGV_OBJECT,  NULL, &input,  "FMatrix", "input data matrix",
      "<output>", ARGV_OBJECT,  NULL, &output, "FMatrix", "output data matrix",
      "-target",  ARGV_OBJECT,  NULL, &target, "FMatrix", "target data matrix",
      "-errFct",  ARGV_STRING,  NULL, &errName, NULL,     "name of error function",
  NULL) != TCL_OK) return TCL_ERROR;

  if (input->n != net->inputN) {
    ERROR("input data matrix does not match network specs\n");
    return TCL_ERROR;
  }

  if ((op = ffnetEval(net,input)) == NULL) return TCL_ERROR;

  fmatrixResize(output,op->m,op->n);
  (void) fmatrixCopy(output,op);  

  if ((target) && (target->n == net->outputN) && 
                  (target->m == output->m)) {
    if (errName) char2errFct(errName,&errFct,&errDerFct);
    itfAppendResult("%f ",ffnetError(net,input,target,errFct,1));
  }

  return TCL_OK;
}



/* -------------------------------------------------------------------- */
/*  ffnetBackpropItf                                                    */
/* -------------------------------------------------------------------- */
int  ffnetBackpropItf (ClientData cd, int argc, char *argv[]) {

  FFNet        *net      = (FFNet *) cd;
  int          ac        = argc-1;
  FMatrix      *input    = NULL;
  FMatrix      *target   = NULL;
  FMatrix      *weight   = NULL;
  float        *fw       = NULL;
  int          shuffle   = 0;
  int          moment    = 0;
  int          adapt     = 0;
  int          iterN     = 1;
  int          batchN    = 0;
  char         *logfile  = NULL;
  FMatrix      *logmat   = NULL;
  int          testAll   = 0;
  ErrFctPtr    errFct    = NULL;
  ErrDerFctPtr errDerFct = NULL;
  char         *errName  = NULL;

  if ( itfParseArgv( argv[0], &ac, argv+1, 1,
      "<input>",  ARGV_OBJECT,  NULL, &input,  "FMatrix", "input data matrix",
      "<target>", ARGV_OBJECT,  NULL, &target, "FMatrix", "target data matrix",
      "-weight",  ARGV_OBJECT,  NULL, &weight, "FMatrix", "weight vector",
      "-shuffle", ARGV_INT,     NULL, &shuffle, NULL,     "shuffle training set?",
      "-moment",  ARGV_INT,     NULL, &moment,  NULL,     "use momentum term?",
      "-adapt",   ARGV_INT,     NULL, &adapt,   NULL,     "adapt learning rate?",
      "-iterN",   ARGV_INT,     NULL, &iterN,   NULL,     "number of iterations",
      "-batchN",  ARGV_INT,     NULL, &batchN,  NULL,     "batch size",
      "-logfile", ARGV_STRING,  NULL, &logfile, NULL,     "log file",
      "-logmat",  ARGV_OBJECT,  NULL, &logmat, "FMatrix", "log matrix",
      "-testAll", ARGV_INT,     NULL, &testAll, NULL,     "test after every batch?",
      "-errFct",  ARGV_STRING,  NULL, &errName, NULL,     "name of test error function",
  NULL) != TCL_OK) return TCL_ERROR;

  if (weight) {
    if (weight->m != 1) {
      ERROR("weight matrix must be 1xn\n");
      return TCL_ERROR;
    }
    fw = weight->matPA[0];
  }

  if (errName) char2errFct(errName,&errFct,&errDerFct);

  return ffnetBackpropBatch(net,input,target,fw,shuffle,moment,adapt,
                            iterN,batchN,errFct,logfile,logmat,testAll);
}



/* -------------------------------------------------------------------- */
/*  ffnetSetSeedItf                                                     */
/* -------------------------------------------------------------------- */
int  ffnetSetSeedItf (ClientData cd, int argc, char *argv[]) {

  int          ac        = argc-1;
  float        seed      = 0.0;

  if ( itfParseArgv( argv[0], &ac, argv+1, 1,
      "<seed>",  ARGV_FLOAT,  NULL, &seed, NULL, "seed value",
  NULL) != TCL_OK) return TCL_ERROR;

  chaosRandomInit(seed);

  return TCL_OK;
}



static Method ffnetMethod[] = 
{
  { "puts",        ffnetPutsItf,        NULL},
  { "add",         ffnetAddItf,         "add layers/links to the net" },
  { "write",       ffnetWriteItf,       "write network description" },
  { "read",        ffnetReadItf,        "read network description" },
  { "save",        ffnetSaveItf,        "save network parameters" },
  { "load",        ffnetLoadItf,        "load network parameters" },
  { ":=",          ffnetCopyItf,        "copy network" },
  { "confLinks",   ffnetConfLinksItf,   "configure all links identically" },
  { "randomize",   ffnetRandomItf,      "randomize all links uniformly" },
  { "reset",       ffnetResetItf,       "reset net (free training data structures)" },
  { "eval",        ffnetEvalItf,        "evaluate net on data" },
  { "backprop",    ffnetBackpropItf,    "apply backpropagation learning" },
  { "seed",        ffnetSetSeedItf,     "set seed for random number generator" },
  {  NULL,  NULL, NULL }
};


TypeInfo ffnetInfo = {
                  "FFNet", 0, -1, ffnetMethod, 
                  ffnetCreateItf, ffnetFreeItf,
                  ffnetConfigureItf, ffnetAccessItf,
                  itfTypeCntlDefaultNoLink,
                  "General Feed Forward Multilayer Neural Network\n" } ;



/* ----------------------------------------------------------------------- */
/*  FFNet_Init                                                             */
/* ----------------------------------------------------------------------- */
static int FFNetInitialized = 0;

int FFNet_Init () {

  if ( !FFNetInitialized ) {

    if ( FFMat_Init() != TCL_OK ) return TCL_ERROR;

    itfNewType      (&fflinkInfo);
    itfNewType      (&fflayerInfo);
    itfNewType      (&ffnetInfo);

    FFNetInitialized = 1;
  }
  
  return TCL_OK;
}
