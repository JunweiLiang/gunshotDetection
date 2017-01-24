/* ========================================================================
    JANUS      Speech- to Speech Translation
               ------------------------------------------------------------
               Object: Optimized Matrix Operations for Neural Nets
               ------------------------------------------------------------

    Author  :  Juergen Fritsch
    Module  :  ffmat.c
    Date    :  $Id: ffmat.c 2390 2003-08-14 11:20:32Z fuegen $
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
    Revision 4.1  2003/08/14 11:19:50  fuegen
    Merged changes on branch jtk-01-01-15-fms (jaguar -> ibis-013)

    Revision 1.1.2.3  2002/06/26 11:57:52  fuegen
    changes for new directory structure support and code cleaning

    Revision 1.1.2.2  2002/04/29 11:57:05  metze
    Cleaner compiling on gcc

    Revision 1.1.2.1  2002/03/26 10:05:34  soltau
    added to NJD

    Revision 4.1  2001/02/20 17:34:44  hyu
    *** empty log message ***

 * Revision 1.1  98/06/15  19:21:52  hyu
 * Initial revision
 * 
 * Revision 1.1  1996/09/27  08:59:05  fritsch
 * Initial revision
 *

   ======================================================================== */

char RcsVersion[]= 
     "@(#)1$Id: ffmat.c 2390 2003-08-14 11:20:32Z fuegen $";


#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "common.h"
#include "itf.h"

#include "error.h"
#include "mach_ind_io.h"
#include "matrix.h"
#include "ffmat.h"


/* ----------------------------------------------------------------------- */
/*  Macro for checking sanity of IMatrix pointer                           */
/* ----------------------------------------------------------------------- */
#define  ASSERT_IMATRIX(X)   {\
	   if ((X) == NULL) {\
             ERROR("Assertion failure: IMatrix pointer X is NULL\n");\
             return TCL_ERROR;\
	   }\
	 }


/* ----------------------------------------------------------------------- */
/*  Macro for checking sanity of FMatrix pointer                           */
/* ----------------------------------------------------------------------- */
#define  ASSERT_FMATRIX(X)   {\
           if ((X) == NULL) {\
             ERROR("Assertion failure: FMatrix pointer X is NULL\n");\
             return TCL_ERROR;\
	   }\
	 }



/* ----------------------------------------------------------------------- */
/*  Macro for checking sanity of DMatrix pointer                           */
/* ----------------------------------------------------------------------- */
#define  ASSERT_DMATRIX(X)   {\
	   if ((X) == NULL) {\
             ERROR("Assertion failure: DMatrix pointer X is NULL\n");\
             return TCL_ERROR;\
	   }\
	 }



#define  sq(X)   ((X)*(X))



/* ======================================================================== */
/*               custom chaotic pseudo=random number generator              */
/* ======================================================================== */

static double GlobalRandX;
static double GlobalRandY;
static double GlobalRandZ;



/* ------------------------------------------------------------------------ */
/*               custom chaotic pseudo-random number generator              */
/* ------------------------------------------------------------------------ */
int  _chaosRandomInit (double seed) {

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
double  _chaosRandom (double a, double b) {

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




/* ======================================================================== */
/*                       Numerical Recipes routines                         */
/* ======================================================================== */


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

/* Computes sqrt(a^2 + b^2) without destructive underflow or overflow */
static double dpythag(double a, double b)
{
  double absa,absb;
  absa=fabs(a);
  absb=fabs(b);
  if (absa > absb) return absa*sqrt(1.0+DSQR(absb/absa));
  else return (absb == 0.0 ? 0.0 : absb*sqrt(1.0+DSQR(absa/absb)));
}

static void newDVector( int m, double** vecP)
{
  int i=0;

  *vecP = (double*)malloc(m * sizeof(double));
  if (*vecP) for ( i = 0; i < m; i++) (*vecP)[i] = 0.0;
}

static void freeDVector( double* vec)
{
  if (vec) free(vec);
}


static void _dsvdcmp(double **a, int m, int n, double *w, double **v)
{
  /* double dpythag(double a, double b); */
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



static int _dsvclean( double* w, int n)
{
  int i;
  double wmax = 0, wmin;

  for ( i = 0; i < n; i++) if ( w[i] > wmax) wmax = w[i];
  wmin = wmax * 1.0e-19;
  for ( i = n - 1; i >= 0; i--) if ( w[i] < wmin) { w[i] = 0.0; n--; }
  return n; 
}



static void tred2 (double **a, int n, double *d, double *e) {
  
  register int       l,k,j,i;
  register double    scale,hh,h,g,f;

  for (i=n-1; i>=1; i--) {
    l = i-1;
    h = scale = 0.0;
    if (l > 0) {
      for (k=0; k<=l; k++)
        scale += fabs(a[i][k]);
      if (scale == 0.0)
        e[i] = a[i][l];
      else {
        for (k=0; k<=l; k++) {
          a[i][k] /= scale;
          h += a[i][k]*a[i][k];
        }
        f = a[i][l];
        g = (f >= 0.0 ? -sqrt(h) : sqrt(h));
        e[i] = scale*g;
        h -= f*g;
        a[i][l] = f-g;
        f = 0.0;
        for (j=0; j<=l; j++) {
          a[j][i] = a[i][j]/h;
          g = 0.0;
          for (k=0; k<=j; k++)
            g += a[j][k]*a[i][k];
          for (k=j+1;k<=l;k++)
            g += a[k][j]*a[i][k];
          e[j] = g/h;
          f += e[j]*a[i][j];
        }
        hh = f/(h+h);
        for (j=0; j<=l; j++) {
          f = a[i][j];
          e[j] = g = e[j]-hh*f;
          for (k=0; k<=j; k++)
            a[j][k] -= (f*e[k]+g*a[i][k]);
        }
      }
    } else
      e[i] = a[i][l];
    d[i] = h;
  }
  d[0] = 0.0;
  e[0] = 0.0;

  for (i=0; i<n; i++) {
    l = i-1;
    if (d[i]) {
      for (j=0; j<=l; j++) {
        g = 0.0;
        for (k=0; k<=l; k++)
          g += a[i][k]*a[k][j];
        for (k=0; k<=l; k++)
          a[k][j] -= g*a[k][i];
      }
    }
    d[i] = a[i][i];
    a[i][i] = 1.0;
    for (j=0; j<=l; j++)
      a[j][i] = a[i][j] = 0.0;
  }
}


static double  pythag (double a, double b) {

  register double absa = fabs(a);
  register double absb = fabs(b);

  if (absa > absb)
    return absa*sqrt(1.0+DSQR(absb/absa));
  else
    return (absb == 0.0 ? 0.0 : absb*sqrt(1.0+DSQR(absa/absb)));
}


static void tqli (double *d, double *e, int n, double **z) {

  register int        m,l,iter,i,k;
  register double     s,r,p,g,f,dd,c,b;

  for (i=1; i<n; i++)
    e[i-1] = e[i];
  e[n-1] = 0.0;

  for (l=0; l<n; l++) {
    iter = 0;
    do {
      for (m=l; m<n-1; m++) {
        dd = fabs(d[m]) + fabs(d[m+1]);
        if ((fabs(e[m])+dd) == dd) break;
      }
      if (m != l) {
        if (iter++ == 30) {
          ERROR("Max iterations exceeded in tqli()\n");
          return;
        }
        g = (d[l+1]-d[l])/(2.0*e[l]);
        r = pythag(g,1.0);
        g = d[m]-d[l]+e[l]/(g+SIGN(r,g));
        s = c = 1.0;
        p = 0.0;
        for (i=m-1; i>=l; i--) {
          f = s*e[i];
          b = c*e[i];
          e[i+1] = (r=pythag(f,g));
          if (r == 0.0) {
            d[i+1] -= p;
            e[m] = 0.0;
            break;
          }
          s = f/r;
          c = g/r;
          g = d[i+1]-p;
          r = (d[i]-g)*s+2.0*c*b;
          d[i+1] = g+(p=s*r);
          g = c*r-b;
          for (k=0; k<n; k++) {
            f = z[k][i+1];
            z[k][i+1] = s*z[k][i]+c*f;
            z[k][i] = c*z[k][i]-s*f;
          }
        }
        if (r == 0.0 && i >= l) continue;
        d[l] -= p;
        e[l] = g;
        e[m] = 0.0;
      }
    } while (m != l);
  }
}



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



/* ======================================================================== */
/*                             IMatrix routines                             */
/* ======================================================================== */



/* -------------------------------------------------------------------- */
/*  _imatrixDeinit                                                      */
/* -------------------------------------------------------------------- */
int  _imatrixDeinit (IMatrix *imat) {
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



/* -------------------------------------------------------------------- */
/*  _imatrixDestroy                                                     */
/* -------------------------------------------------------------------- */
int  _imatrixDestroy (IMatrix *imat) {

  if (!imat) return TCL_OK;
  if (_imatrixDeinit(imat) == TCL_ERROR) return TCL_ERROR;
  free(imat);

  return TCL_OK;
}


/* -------------------------------------------------------------------- */
/*  _imatrixInitConst                                                   */
/* -------------------------------------------------------------------- */
int  _imatrixInitConst (IMatrix *imat, int initVal) {

  register int    i,j;

  for (i=0; i<imat->m; i++)
    for (j=0; j<imat->n; j++)
      imat->matPA[i][j] = initVal;

  return TCL_OK;
}



/* -------------------------------------------------------------------- */
/*  _imatrixInitDiag                                                    */
/* -------------------------------------------------------------------- */
int  _imatrixInitDiag (IMatrix *imat, int initVal) {

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
/*  _imatrixInitRandom                                                  */
/* -------------------------------------------------------------------- */
int  _imatrixInitRandom (IMatrix *imat, float maxAbsVal) {

  register int    i,j;

  for (i=0; i<imat->m; i++)
    for (j=0; j<imat->n; j++)
      imat->matPA[i][j] = (int) _chaosRandom(-maxAbsVal,+maxAbsVal);

  return TCL_OK;
}



/* -------------------------------------------------------------------- */
/*  _imatrixResize  --  resize Imatrix, leave content unchanged         */
/* -------------------------------------------------------------------- */
int  _imatrixResize (IMatrix *imat, int m, int n) {

  register int    k;
  int             **p;  

  ASSERT_IMATRIX(imat);

  if ((imat->m == m) && (imat->n == n)) return TCL_OK;
  if (!(m*n)) return TCL_ERROR;

  p    = (int **) malloc (sizeof(int *) * m);
  p[0] = (int *)  malloc (sizeof(int)   * (m*n));
  for (k=1; k<m; k++)
    p[k] = p[k-1] + n;

  if (imat->m*imat->n) {
    free(imat->matPA[0]);
    free(imat->matPA);
  }

  imat->matPA = p;
  imat->m = m;
  imat->n = n;

  return TCL_OK;
}



/* -------------------------------------------------------------------- */
/*  _imatrixCreate                                                      */
/* -------------------------------------------------------------------- */
IMatrix  *_imatrixCreate (int m, int n) {

  IMatrix  *imat;

  if ((imat = (IMatrix *) malloc (sizeof(IMatrix))) == NULL) {
    ERROR("_imatrixCreate(): Out of memory\n");
    return NULL;
  }

  imat->matPA  = NULL;
  imat->m  = 0;
  imat->n  = 0;
  
  _imatrixResize(imat,m,n);
  _imatrixInitConst(imat,0.0);

  return imat;
}



/* -------------------------------------------------------------------- */
/*  _imatrixPrint                                                       */
/* -------------------------------------------------------------------- */
int  _imatrixPrint (IMatrix *imat) {

  register int    i,j;

  ASSERT_IMATRIX(imat);
  
  for (i=0; i<imat->m; i++) {
    for (j=0; j<imat->n; j++)
      printf("%12.7d  ",imat->matPA[i][j]);
    printf("\n");
  }
  printf("\n");
    
  return TCL_OK;
}



/* -------------------------------------------------------------------- */
/*  _imatrixPuts                                                        */
/* -------------------------------------------------------------------- */
int  _imatrixPuts (IMatrix *imat, FILE *fp) {

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



/* -------------------------------------------------------------------- */
/*  _imatrixCopy      C = A                                             */
/* -------------------------------------------------------------------- */
int  _imatrixCopy (IMatrix *C, IMatrix *A) {

  register int   i,j;
  
  ASSERT_IMATRIX(C);
  ASSERT_IMATRIX(A);

  if (C==A) return TCL_OK;

  _imatrixResize(C,A->m,A->n);
  for (i=0; i<A->m; i++)
    for (j=0; j<A->n; j++)
      C->matPA[i][j] = A->matPA[i][j];
  
  return TCL_OK;
}



/* -------------------------------------------------------------------- */
/*  _imatrixTrans      C = At        (C=A is possible)                  */
/* -------------------------------------------------------------------- */
int  _imatrixTrans (IMatrix *C, IMatrix *A) {

  IMatrix           *B;
  register int      i,j;
  
  ASSERT_IMATRIX(C);
  ASSERT_IMATRIX(A);

  if (C==A) {
    B = _imatrixCreate(A->n,A->m);
    for (i=0; i<A->m; i++)
      for (j=0; j<A->n; j++)
	B->matPA[j][i] = A->matPA[i][j];
    _imatrixResize(A,B->m,B->n);
    _imatrixCopy(C,B);
    _imatrixDestroy(B);
  } else {
    _imatrixResize(C,A->n,A->m);
    for (i=0; i<A->m; i++)
      for (j=0; j<A->n; j++)
	C->matPA[j][i] = A->matPA[i][j];
  }
  
  return TCL_OK;
}



/* ------------------------------------------------------------------------ */
/*  _imatrixSave                                                            */
/* ------------------------------------------------------------------------ */
int  _imatrixSave (IMatrix *A, FILE *fp) {

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
/*  _imatrixLoad                                                            */
/* ------------------------------------------------------------------------ */
int  _imatrixLoad (IMatrix *A, FILE *fp) {

  register int         i,j,m,n;

  ASSERT_IMATRIX(A);

  m = read_int(fp);
  n = read_int(fp);

  _imatrixResize(A,m,n);

  for (i=0; i<A->m; i++)
    for (j=0; j<A->n; j++)
      A->matPA[i][j] = (unsigned char) fgetc(fp);

  return TCL_OK;
}



/* ======================================================================== */
/*                             FMatrix routines                             */
/* ======================================================================== */



/* -------------------------------------------------------------------- */
/*  _fmatrixDeinit                                                      */
/* -------------------------------------------------------------------- */
int  _fmatrixDeinit (FMatrix *fmat) {

  ASSERT_FMATRIX(fmat);

  if (fmat->matPA != NULL) {
    free(fmat->matPA[0]);
    free(fmat->matPA);
  }

  fmat->matPA = NULL;
  fmat->m = 0;
  fmat->n = 0;

  return TCL_OK;
}



/* -------------------------------------------------------------------- */
/*  _fmatrixDestroy                                                     */
/* -------------------------------------------------------------------- */
int  _fmatrixDestroy (FMatrix *fmat) {

  if (!fmat) return TCL_OK;
  if (_fmatrixDeinit(fmat) == TCL_ERROR) return TCL_ERROR;
  free(fmat);

  return TCL_OK;
}


/* -------------------------------------------------------------------- */
/*  _fmatrixInitConst                                                   */
/* -------------------------------------------------------------------- */
int  _fmatrixInitConst (FMatrix *fmat, float initVal) {

  register int    i,j;

  for (i=0; i<fmat->m; i++)
    for (j=0; j<fmat->n; j++)
      fmat->matPA[i][j] = initVal;

  return TCL_OK;
}



/* -------------------------------------------------------------------- */
/*  _fmatrixInitDiag                                                    */
/* -------------------------------------------------------------------- */
int  _fmatrixInitDiag (FMatrix *fmat, float initVal) {

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
/*  _fmatrixInitRandom                                                  */
/* -------------------------------------------------------------------- */
int  _fmatrixInitRandom (FMatrix *fmat, float maxAbsVal) {

  register int    i,j;

  for (i=0; i<fmat->m; i++)
    for (j=0; j<fmat->n; j++)
      fmat->matPA[i][j] = (float) _chaosRandom(-maxAbsVal,+maxAbsVal);

  return TCL_OK;
}



/* -------------------------------------------------------------------- */
/*  _fmatrixPrune    --   set all items < eta to zero                   */
/* -------------------------------------------------------------------- */
#define   ETA   1E-5
int  _fmatrixPrune (FMatrix *fmat) {

  register int        i,j;

  ASSERT_FMATRIX(fmat);

  for (i=0; i<fmat->m; i++)
    for (j=0; j<fmat->n; j++)
      if (fabs(fmat->matPA[i][j]) < ETA)
        fmat->matPA[i][j] = 0.0;

  return TCL_OK;
}



/* -------------------------------------------------------------------- */
/*  _fmatrixResize    --    resize FMatrix                              */
/* -------------------------------------------------------------------- */
int  _fmatrixResize (FMatrix *fmat, int m, int n) {

  register int   k;
  float          **p;

  ASSERT_FMATRIX(fmat);

  if ((fmat->m == m) && (fmat->n == n)) return TCL_OK;
  if (!(m*n)) return TCL_ERROR;

  p    = (float **) malloc (sizeof(float *) * m);
  p[0] = (float *)  malloc (sizeof(float)   * (m*n));
  for (k=1; k<m; k++)
    p[k] = p[k-1] + n;

  if (fmat->m*fmat->n) {
    free(fmat->matPA[0]);
    free(fmat->matPA);
  }

  fmat->matPA = p;
  fmat->m = m;
  fmat->n = n;

  return TCL_OK;
}



/* -------------------------------------------------------------------- */
/*  _fmatrixCreate                                                      */
/* -------------------------------------------------------------------- */
FMatrix  *_fmatrixCreate (int m, int n) {

  FMatrix  *fmat;

  if ((fmat = (FMatrix *) malloc (sizeof(FMatrix))) == NULL) {
    ERROR("_fmatrixCreate(): Out of memory\n");
    return NULL;
  }

  fmat->matPA  = NULL;
  fmat->m  = 0;
  fmat->n  = 0;
  
  _fmatrixResize(fmat,m,n);
  _fmatrixInitConst(fmat,0.0);

  return fmat;
}



/* -------------------------------------------------------------------- */
/*  _fmatrixPrint                                                       */
/* -------------------------------------------------------------------- */
int  _fmatrixPrint (FMatrix *fmat) {

  register int    i,j;

  ASSERT_FMATRIX(fmat);
  
  for (i=0; i<fmat->m; i++) {
    for (j=0; j<fmat->n; j++)
      printf("%12.7f  ",fmat->matPA[i][j]);
    printf("\n");
  }
  printf("\n");
    
  return TCL_OK;
}



/* -------------------------------------------------------------------- */
/*  _fmatrixPuts                                                        */
/* -------------------------------------------------------------------- */
int  _fmatrixPuts (FMatrix *fmat, FILE *fp) {

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
/*  _fmatrixCopy      C = A                                             */
/* -------------------------------------------------------------------- */
int  _fmatrixCopy (FMatrix *C, FMatrix *A) {

  register int   i,j;
  
  ASSERT_FMATRIX(C);
  ASSERT_FMATRIX(A);

  if (C==A) return TCL_OK;

  _fmatrixResize(C,A->m,A->n);
  for (i=0; i<A->m; i++)
    for (j=0; j<A->n; j++)
      C->matPA[i][j] = A->matPA[i][j];
  
  return TCL_OK;
}



/* -------------------------------------------------------------------- */
/*  _fmatrixTrans      C = At        (C=A is possible)                  */
/* -------------------------------------------------------------------- */
int  _fmatrixTrans (FMatrix *C, FMatrix *A) {

  FMatrix           *B;
  register int      i,j;
  
  ASSERT_FMATRIX(C);
  ASSERT_FMATRIX(A);

  if (C==A) {
    B = _fmatrixCreate(A->n,A->m);
    for (i=0; i<A->m; i++)
      for (j=0; j<A->n; j++)
	B->matPA[j][i] = A->matPA[i][j];
    _fmatrixResize(A,B->m,B->n);
    _fmatrixCopy(C,B);
    _fmatrixDestroy(B);
  } else {
    _fmatrixResize(C,A->n,A->m);
    for (i=0; i<A->m; i++)
      for (j=0; j<A->n; j++)
	C->matPA[j][i] = A->matPA[i][j];
  }
  
  return TCL_OK;
}



/* -------------------------------------------------------------------- */
/*  _fmatrixTrans1      C = At + additional row of 1s (C=A is possible) */
/* -------------------------------------------------------------------- */
int  _fmatrixTrans1 (FMatrix *C, FMatrix *A) {

  FMatrix           *B;
  register int      i,j;
  
  ASSERT_FMATRIX(C);
  ASSERT_FMATRIX(A);

  if (C==A) {
    B = _fmatrixCreate(A->n+1,A->m);
    for (i=0; i<A->m; i++) {
      for (j=0; j<A->n; j++)
	B->matPA[j][i] = A->matPA[i][j];
      B->matPA[A->n][i] = 1.0;
    }
    _fmatrixResize(A,B->m,B->n);
    _fmatrixCopy(C,B);
    _fmatrixDestroy(B);
  } else {
    _fmatrixResize(C,A->n+1,A->m);
    for (i=0; i<A->m; i++) {
      for (j=0; j<A->n; j++)
	C->matPA[j][i] = A->matPA[i][j];
      C->matPA[A->n][i] = 1.0;
    }
  }
  
  return TCL_OK;
}



/* -------------------------------------------------------------------- */
/*  _fmatrixMSE                                                         */
/* -------------------------------------------------------------------- */
float  _fmatrixMSE (FMatrix *fmat) {

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
/*  _fmatrixScale     C = A * gamma     (C=A is possible)               */
/* -------------------------------------------------------------------- */
int  _fmatrixScale (FMatrix *C, FMatrix *A, float gamma) {

  register int      i,j;
  
  ASSERT_FMATRIX(C);
  ASSERT_FMATRIX(A);

  _fmatrixResize(C,A->m,A->n);
  for (i=0; i<A->m; i++)
    for (j=0; j<A->n; j++)
      C->matPA[i][j] = A->matPA[i][j] * gamma;
  
  return TCL_OK;
}



/* -------------------------------------------------------------------- */
/*  _fmatrixScaleCol                                                    */
/* -------------------------------------------------------------------- */
int  _fmatrixScaleCol (FMatrix *C, FMatrix *A, FMatrix *W) {

  register int      i,j;
  
  ASSERT_FMATRIX(C);
  ASSERT_FMATRIX(A);

  if (W->m != 1) {
    ERROR("_fmatrixScaleCol(): WARNING: W has more than one row\n");
  }

  if (W->n != A->n) {
    ERROR("_fmatrixScaleCol(): A,W dimensions do not match\n");
    return TCL_ERROR;
  }
  
  _fmatrixResize(C,A->m,A->n);
  for (i=0; i<A->m; i++)
    for (j=0; j<A->n; j++)
      C->matPA[i][j] = A->matPA[i][j] * W->matPA[0][j];
  
  return TCL_OK;
}



/* -------------------------------------------------------------------- */
/*  _fmatrixAdd     C = A + B                                           */
/* -------------------------------------------------------------------- */
int  _fmatrixAdd (FMatrix *C, FMatrix *A, FMatrix *B) {

  register int        i,j;
  
  ASSERT_FMATRIX(C);
  ASSERT_FMATRIX(A);
  ASSERT_FMATRIX(B);

  if ((A->m != B->m) || (A->n != B->n)) {
    ERROR("_fmatrixAdd(): dimensions do not match\n");
    return TCL_ERROR;
  }

  _fmatrixResize(C,A->m,A->n);

  for (i=0; i<A->m; i++)
    for (j=0; j<A->n; j++)
      C->matPA[i][j] = A->matPA[i][j] + B->matPA[i][j];

  return TCL_OK;
}



/* -------------------------------------------------------------------- */
/*  _fmatrixAccu     C = C + alpha*A                                    */
/* -------------------------------------------------------------------- */
int  _fmatrixAccu (FMatrix *C, FMatrix *A, float alpha) {

  register int        i,j;
  
  ASSERT_FMATRIX(C);
  ASSERT_FMATRIX(A);

  if ((A->m != C->m) || (A->n != C->n)) {
    ERROR("_fmatrixAccu(): dimensions do not match\n");
    return TCL_ERROR;
  }

  for (i=0; i<C->m; i++)
    for (j=0; j<C->n; j++)
      C->matPA[i][j] += alpha * A->matPA[i][j];

  return TCL_OK;
}



/* -------------------------------------------------------------------- */
/*  _fmatrixAccuScaled   C = C + g_i*A  (rather specialized operation   */
/*                                       used in HME's evaluation)      */
/* -------------------------------------------------------------------- */
int  _fmatrixAccuScaled (FMatrix *C, FMatrix *A, FMatrix *g, int index) {

  register int        i,j;
  
  ASSERT_FMATRIX(C);
  ASSERT_FMATRIX(A);
  ASSERT_FMATRIX(g);

  if ((A->m != C->m) || (A->n != C->n) || (A->m != g->m)) {
    ERROR("_fmatrixAccuScaled(): dimensions do not match\n");
    return TCL_ERROR;
  }

  if ((index < 0) || (index >= g->n)) {
    ERROR("_fmatrixAccuScaled(): invalid gating index %d\n",index);
    return TCL_ERROR;
  }

  for (i=0; i<C->m; i++)
    for (j=0; j<C->n; j++)
      C->matPA[i][j] += g->matPA[i][index] * A->matPA[i][j];

  return TCL_OK;
}



/* -------------------------------------------------------------------- */
/*  _fmatrixSub     C = A - B                                           */
/* -------------------------------------------------------------------- */
int  _fmatrixSub (FMatrix *C, FMatrix *A, FMatrix *B) {

  register int        i,j;
  
  ASSERT_FMATRIX(C);
  ASSERT_FMATRIX(A);
  ASSERT_FMATRIX(B);

  if ((A->m != B->m) || (A->n != B->n)) {
    ERROR("_fmatrixSub(): dimensions do not match\n");
    return TCL_ERROR;
  }

  _fmatrixResize(C,A->m,A->n);

  for (i=0; i<A->m; i++)
    for (j=0; j<A->n; j++)
      C->matPA[i][j] = A->matPA[i][j] - B->matPA[i][j];

  return TCL_OK;
}



/* -------------------------------------------------------------------- */
/*  _fmatrixProd     C = A * B  (component-wise)                        */
/* -------------------------------------------------------------------- */
int  _fmatrixProd (FMatrix *C, FMatrix *A, FMatrix *B) {

  register int        i,j;
  
  ASSERT_FMATRIX(C);
  ASSERT_FMATRIX(A);
  ASSERT_FMATRIX(B);

  if ((A->m != B->m) || (A->n != B->n)) {
    ERROR("_fmatrixProd(): dimensions do not match\n");
    return TCL_ERROR;
  }

  _fmatrixResize(C,A->m,A->n);

  for (i=0; i<A->m; i++)
    for (j=0; j<A->n; j++)
      C->matPA[i][j] = A->matPA[i][j] * B->matPA[i][j];

  return TCL_OK;
}



/* -------------------------------------------------------------------- */
/*  _fmatrixCosine             Computes the cosine of the angle between */
/*                          two given matrices, interpreted as vectors  */
/* -------------------------------------------------------------------- */
float  _fmatrixCosine (FMatrix *A, FMatrix *B) {

  float               *a,*b;
  register double     sum,sumA,sumB;
  register int        i,j;

  ASSERT_FMATRIX(A);
  ASSERT_FMATRIX(B);

  if ((A->m != B->m) || (A->n != B->n)) {
    ERROR("_fmatrixCosine(): matrices have different size\n");
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
/*  _fmatrixMulTT  C = A * Bt   B might be a NN weight matrix containing */
/*                            a bias weight column at the far right     */
/* -------------------------------------------------------------------- */
int  _fmatrixMulTT (FMatrix *C, FMatrix *A, FMatrix *B) {

  register int       i,j,k,bias;

  ASSERT_FMATRIX(C);
  ASSERT_FMATRIX(A);
  ASSERT_FMATRIX(B);

  if ((C==A) || (C==B)) {
    ERROR("_fmatrixMulT(): C must be different from A and B\n");
    return TCL_ERROR;
  }

  if ((A->n != B->n) && (A->n+1 != B->n)) {
    ERROR("_fmatrixMulT(): dimensions do not match\n");
    return TCL_ERROR;
  }

  if (A->n+1 == B->n) bias=1; else bias=0;
  _fmatrixResize(C,A->m,B->m);
  _fmatrixInitConst(C,0.0);

  for (i=0; i<A->m; i++) {
    for (j=0; j<B->m; j++) {
      for (k=0; k<A->n; k++)
        C->matPA[i][j] += A->matPA[i][k] * B->matPA[j][k];
      if (bias) C->matPA[i][j] += B->matPA[j][B->n-1];
    }
  }

  return TCL_OK;
}



/* -------------------------------------------------------------------- */
/*  _fmatrixMulT  C = A * Bt   B might be a NN weight matrix containing */
/*                            a bias weight column at the far right     */
/* -------------------------------------------------------------------- */
int  _fmatrixMulT (FMatrix *C, FMatrix *A, FMatrix *B) {

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
    ERROR("_fmatrixMulT(): C must be different from A and B\n");
    return TCL_ERROR;
  }

  if ((A->n != B->n) && (A->n+1 != B->n)) {
    ERROR("_fmatrixMulT(): dimensions do not match\n");
    return TCL_ERROR;
  }

  if (A->n+1 == B->n) bias=1; else bias=0;
  _fmatrixResize(C,A->m,B->m);
  _fmatrixInitConst(C,0.0);

  /* ------------------------------------ */
  /*  first pass in junks of 4x4 elements */
  /* ------------------------------------ */
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
/*  _fmatrixMul   C = A * B    B might be a NN weight matrix containing */
/*                            a bias weight column at the far right     */
/* -------------------------------------------------------------------- */
int  _fmatrixMul (FMatrix *C, FMatrix *A, FMatrix *B) {

  FMatrix     *D;
  
  ASSERT_FMATRIX(C);
  ASSERT_FMATRIX(A);
  ASSERT_FMATRIX(B);

  if ((C==A) || (C==B)) {
    ERROR("_fmatrixMul(): C must be different from A and B\n");
    return TCL_ERROR;
  }

  if ((A->n != B->m) && (A->n != B->m+1)) {
    ERROR("_fmatrixMul(): dimensions do not match\n");
    return TCL_ERROR;
  }

  D = _fmatrixCreate(B->n,B->m);
  _fmatrixTrans(D,B);
  _fmatrixMulT(C,A,D);
  _fmatrixDestroy(D);

  return TCL_OK;
}



/* -------------------------------------------------------------------- */
/*  _fmatrixMul1   C = A * B      B is considered to have an additional */
/*                             implicit column of 1's on the far right  */
/* -------------------------------------------------------------------- */
int  _fmatrixMul1 (FMatrix *C, FMatrix *A, FMatrix *B) {

  FMatrix     *D;
  
  ASSERT_FMATRIX(C);
  ASSERT_FMATRIX(A);
  ASSERT_FMATRIX(B);

  if ((C==A) || (C==B)) {
    ERROR("_fmatrixMul1(): C must be different from A and B\n");
    return TCL_ERROR;
  }

  if (A->n != B->m) {
    ERROR("_fmatrixMul1(): dimensions do not match\n");
    return TCL_ERROR;
  }

  D = _fmatrixCreate(B->n+1,B->m);
  _fmatrixTrans1(D,B);
  _fmatrixMulT(C,A,D);
  _fmatrixDestroy(D);

  return TCL_OK;
}



/* -------------------------------------------------------------------- */
/*  _fmatrixRadT  C = (A - Bt)^2          B might be a NN weight matrix */
/*                           a variance weight column at the far right  */
/* -------------------------------------------------------------------- */
int  _fmatrixRadT (FMatrix *C, FMatrix *A, FMatrix *B) {

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
    ERROR("_fmatrixRadT(): C must be different from A and B\n");
    return TCL_ERROR;
  }

  if ((A->n != B->n) && (A->n+1 != B->n)) {
    ERROR("_fmatrixRadT(): dimensions do not match\n");
    return TCL_ERROR;
  }

  if (A->n+1 == B->n) bias=1; else bias=0;
  _fmatrixResize(C,A->m,B->m);
  _fmatrixInitConst(C,0.0);

  /* ------------------------------------ */
  /*  first pass in junks of 4x4 elements */
  /* ------------------------------------ */
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
          ERROR("_fmatrixRadT(): negative variance\n");
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
/*  _fmatrixRad   C = A * B    B might be a NN weight matrix containing */
/*                           a variance weight column at the far right  */
/* -------------------------------------------------------------------- */
int  _fmatrixRad (FMatrix *C, FMatrix *A, FMatrix *B) {

  FMatrix     *D;
  
  ASSERT_FMATRIX(C);
  ASSERT_FMATRIX(A);
  ASSERT_FMATRIX(B);

  if ((C==A) || (C==B)) {
    ERROR("_fmatrixRad(): C must be different from A and B\n");
    return TCL_ERROR;
  }

  if ((A->n != B->m) && (A->n+1 != B->m)) {
    ERROR("_fmatrixRad(): dimensions do not match\n");
    return TCL_ERROR;
  }

  D = _fmatrixCreate(B->n,B->m);
  _fmatrixTrans(D,B);
  _fmatrixRadT(C,A,D);
  _fmatrixDestroy(D);

  return TCL_OK;
}



/* -------------------------------------------------------------------- */
/*  _fmatrixInv     C = A^-1   (C=A is possible)                        */
/* -------------------------------------------------------------------- */
int  _fmatrixInv (FMatrix *C, FMatrix *A) {

  DMatrix           *U,*V,*W;
  register int      i,j,n = A->n;
  
  ASSERT_FMATRIX(C);
  ASSERT_FMATRIX(A);

  if (A->m != A->n) {
    ERROR("_fmatrixInv(): A is not a square matrix\n");
    return TCL_ERROR;
  }

  /* ----------------------------- */
  /*  allocate temporary matrices  */
  /* ----------------------------- */
  U = _dmatrixCreate(n,n);
  V = _dmatrixCreate(n,n);
  W = _dmatrixCreate(1,n);
  _fmatrix2dmatrix(U,A);

  /* -------------------------------------- */
  /*  compute singular value decomposition  */
  /* -------------------------------------- */
  _dsvdcmp(U->matPA,n,n,W->matPA[0],V->matPA);
  _dsvclean(W->matPA[0],n);
  
  /* --------------------- */
  /*  compute the inverse  */
  /* --------------------- */
  for (j=0; j<n; j++) {
    if (W->matPA[0][j] < 1E-12) {
      W->matPA[0][j] = 1E-12;
      ERROR("_fmatrixInv(): WARNING: Matrix is ill-conditioned\n");
    }
    for (i=0; i<n; i++)
      U->matPA[i][j] /= W->matPA[0][j];
  }

  _dmatrixMulT(W,V,U);
  _dmatrix2fmatrix(C,W);
  _dmatrixDestroy(U);
  _dmatrixDestroy(V);
  _dmatrixDestroy(W);

  return TCL_OK;
}



/* -------------------------------------------------------------------- */
/*  _fmatrixLS      Linear Least Squares       D = XWX^-1 YWX           */
/* -------------------------------------------------------------------- */
int  _fmatrixLS (FMatrix *D, FMatrix *W, FMatrix *Y, FMatrix *X) {

  FMatrix     *Xt,*WX,*XWX,*YWX;
  
  ASSERT_FMATRIX(D);
  ASSERT_FMATRIX(W);
  ASSERT_FMATRIX(Y);
  ASSERT_FMATRIX(X);

  if ((W->m != 1) || (W->n != X->m)) {
    ERROR("_fmatrixLS(): W is not a 1x%d matrix\n",X->m);
    return TCL_ERROR;
  }

  if (Y->m != X->m) {
    ERROR("_fmatrixLS(): Y is not a %dx* matrix\n",X->m);
    return TCL_ERROR;
  }

  Xt = _fmatrixCreate(X->n,X->m);
  WX = _fmatrixCreate(X->n,X->m);
  XWX = _fmatrixCreate(X->n,X->n);
  YWX = _fmatrixCreate(Y->n,X->n);
  
  _fmatrixTrans(Xt,X);
  _fmatrixScaleCol(WX,Xt,W);
  _fmatrixMulT(XWX,Xt,WX);
  _fmatrixInv(XWX,XWX);

  _fmatrixMul(YWX,WX,Y);
  _fmatrixMul(D,XWX,YWX);
  _fmatrixTrans(D,D);

  _fmatrixDestroy(Xt);
  _fmatrixDestroy(WX);
  _fmatrixDestroy(XWX);
  _fmatrixDestroy(YWX);

  return TCL_OK;
}



/* -------------------------------------------------------------------- */
/*  _fmatrixEigen                  Eigenvectors V and Eigenvalues D of  */
/*                                real symetric matrices A              */
/* -------------------------------------------------------------------- */
int  _fmatrixEigen (FMatrix *V, FMatrix *D, FMatrix *A) {

  DMatrix       *E,*T,*X;

  ASSERT_FMATRIX(V);  
  ASSERT_FMATRIX(D);  
  ASSERT_FMATRIX(A);  

  if (A->m != A->n) {
    ERROR("_fmatrixEigen(): A is not a square matrix\n");
    return TCL_ERROR;
  }
  
  E = _dmatrixCreate(1,A->n);
  T = _dmatrixCreate(1,A->n);
  X = _dmatrixCreate(A->m,A->n);
  _fmatrix2dmatrix(X,A);
  
  tred2(X->matPA,X->n,T->matPA[0],E->matPA[0]);
  tqli(T->matPA[0],E->matPA[0],X->n,X->matPA);

  _dmatrix2fmatrix(V,X);
  _dmatrix2fmatrix(D,T);
  _dmatrixDestroy(E);
  _dmatrixDestroy(T);
  _dmatrixDestroy(X);
  
  return TCL_OK;
}



/* -------------------------------------------------------------------- */
/*  _fmatrixDet                          log of determinant of matrix A */
/* -------------------------------------------------------------------- */
int  _fmatrixDet (FMatrix *A, double *logdet, double *sign) {

  DMatrix        *D;
  int            k;
  double         d,sum,val;

  ASSERT_FMATRIX(A);

  if (A->m != A->n) {
    ERROR("_fmatrixDet(): A must be squared\n");
    return 0.0;
  }

  D = _dmatrixCreate(A->m,A->n);
  _fmatrix2dmatrix(D,A);
  
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

  _dmatrixDestroy(D);

  return TCL_OK;
}



/* ------------------------------------------------------------------------ */
/*  _fmatrixSave                                                            */
/* ------------------------------------------------------------------------ */
int  _fmatrixSave (FMatrix *A, FILE *fp) {

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
/*  _fmatrixLoad                                                            */
/* ------------------------------------------------------------------------ */
int  _fmatrixLoad (FMatrix *A, FILE *fp) {

  register int         i,j,m,n;

  ASSERT_FMATRIX(A);

  m = read_int(fp);
  n = read_int(fp);

  _fmatrixResize(A,m,n);

  for (i=0; i<A->m; i++)
    for (j=0; j<A->n; j++)
      A->matPA[i][j] = read_float(fp);

  return TCL_OK;
}



/* ======================================================================== */
/*                               DMAT routines                              */
/* ======================================================================== */



/* -------------------------------------------------------------------- */
/*  _dmatrixDeinit                                                      */
/* -------------------------------------------------------------------- */
int  _dmatrixDeinit (DMatrix *dmat) {

  ASSERT_DMATRIX(dmat);

  if (dmat->matPA != NULL) {
    free(dmat->matPA[0]);
    free(dmat->matPA);
  }

  dmat->matPA = NULL;
  dmat->m = 0;
  dmat->n = 0;
  
  return TCL_OK;
}



/* -------------------------------------------------------------------- */
/*  _dmatrixDestroy                                                     */
/* -------------------------------------------------------------------- */
int  _dmatrixDestroy (DMatrix *dmat) {

  if (!dmat) return TCL_OK;
  if (_dmatrixDeinit(dmat) == TCL_ERROR) return TCL_ERROR;
  free(dmat);

  return TCL_OK;
}


/* -------------------------------------------------------------------- */
/*  _dmatrixInitConst                                                   */
/* -------------------------------------------------------------------- */
int  _dmatrixInitConst (DMatrix *dmat, double initVal) {

  register int    i,j;

  for (i=0; i<dmat->m; i++)
    for (j=0; j<dmat->n; j++)
      dmat->matPA[i][j] = initVal;

  return TCL_OK;
}



/* -------------------------------------------------------------------- */
/*  _dmatrixInitDiag                                                    */
/* -------------------------------------------------------------------- */
int  _dmatrixInitDiag (DMatrix *dmat, double initVal) {

  register int    i,j;

  for (i=0; i<dmat->m; i++)
    for (j=0; j<dmat->n; j++)
      if (i==j)
        dmat->matPA[i][j] = initVal;
      else
	dmat->matPA[i][j] = 0.0;

  return TCL_OK;
}



/* -------------------------------------------------------------------- */
/*  _dmatrixInitRandom                                                  */
/* -------------------------------------------------------------------- */
int  _dmatrixInitRandom (DMatrix *dmat, double maxAbsVal) {

  register int    i,j;

  for (i=0; i<dmat->m; i++)
    for (j=0; j<dmat->n; j++)
      dmat->matPA[i][j] = _chaosRandom(-maxAbsVal,+maxAbsVal);

  return TCL_OK;
}



/* -------------------------------------------------------------------- */
/*  _dmatrixResize  --  resize DMatrix, content may change              */
/* -------------------------------------------------------------------- */
int  _dmatrixResize (DMatrix *dmat, int m, int n) {

  register int   k;
  double         **p;
  
  ASSERT_DMATRIX(dmat);

  if ((dmat->m == m) && (dmat->n == n)) return TCL_OK;
  if (!(m*n)) return TCL_ERROR;

  p    = (double **) malloc (sizeof(double *) * m);
  p[0] = (double *)  malloc (sizeof(double)   * (m*n));
  for (k=1; k<m; k++)
    p[k] = p[k-1] + n;

  if (dmat->m*dmat->n) {
    free(dmat->matPA[0]);
    free(dmat->matPA);
  }

  dmat->matPA = p;
  dmat->m = m;
  dmat->n = n;

  return TCL_OK;
}



/* -------------------------------------------------------------------- */
/*  _dmatrixCreate                                                      */
/* -------------------------------------------------------------------- */
DMatrix  *_dmatrixCreate (int m, int n) {

  DMatrix  *dmat;

  if ((dmat = (DMatrix *) malloc (sizeof(DMatrix))) == NULL) {
    ERROR("_dmatrixCreate(): Out of memory\n");
    return NULL;
  }

  dmat->matPA  = NULL;
  dmat->m  = 0;
  dmat->n  = 0;
  
  _dmatrixResize(dmat,m,n);
  _dmatrixInitConst(dmat,0.0);
  
  return dmat;
}



/* -------------------------------------------------------------------- */
/*  _dmatrixPrint                                                       */
/* -------------------------------------------------------------------- */
int  _dmatrixPrint (DMatrix *dmat) {

  register int    i,j;

  ASSERT_DMATRIX(dmat);
  
  for (i=0; i<dmat->m; i++) {
    for (j=0; j<dmat->n; j++)
      printf("%12.7f  ",dmat->matPA[i][j]);
    printf("\n");
  }
  printf("\n");
    
  return TCL_OK;
}



/* -------------------------------------------------------------------- */
/*  _dmatrixPuts                                                        */
/* -------------------------------------------------------------------- */
int  _dmatrixPuts (DMatrix *dmat, FILE *fp) {

  register int    i,j;

  ASSERT_DMATRIX(dmat);
  
  for (i=0; i<dmat->m; i++) {
    fprintf(fp,"{ ");
    for (j=0; j<dmat->n; j++)
      fprintf(fp,"%12.7f ",dmat->matPA[i][j]);
    fprintf(fp,"}\\\n");
  }
    
  return TCL_OK;
}



/* -------------------------------------------------------------------- */
/*  _dmatrixCopy      C = A                                             */
/* -------------------------------------------------------------------- */
int  _dmatrixCopy (DMatrix *C, DMatrix *A) {

  register int   i,j;
  
  ASSERT_DMATRIX(C);
  ASSERT_DMATRIX(A);

  if (C==A) return TCL_OK;

  _dmatrixResize(C,A->m,A->n);
  for (i=0; i<A->m; i++)
    for (j=0; j<A->n; j++)
      C->matPA[i][j] = A->matPA[i][j];
  
  return TCL_OK;
}



/* -------------------------------------------------------------------- */
/*  _dmatrixTrans      C = At        (C=A is possible)                  */
/* -------------------------------------------------------------------- */
int  _dmatrixTrans (DMatrix *C, DMatrix *A) {

  DMatrix              *B;
  register int      i,j;
  
  ASSERT_DMATRIX(C);
  ASSERT_DMATRIX(A);

  if (C==A) {
    B = _dmatrixCreate(A->n,A->m);
    for (i=0; i<A->m; i++)
      for (j=0; j<A->n; j++)
	B->matPA[j][i] = A->matPA[i][j];
    _dmatrixResize(A,B->m,B->n);
    _dmatrixCopy(C,B);
    _dmatrixDestroy(B);
  } else {
    _dmatrixResize(C,A->n,A->m);
    for (i=0; i<A->m; i++)
      for (j=0; j<A->n; j++)
	C->matPA[j][i] = A->matPA[i][j];
  }
  
  return TCL_OK;
}



/* -------------------------------------------------------------------- */
/*  _dmatrixTrans1      C = At + additional row of 1s (C=A is possible) */
/* -------------------------------------------------------------------- */
int  _dmatrixTrans1 (DMatrix *C, DMatrix *A) {

  DMatrix           *B;
  register int      i,j;
  
  ASSERT_DMATRIX(C);
  ASSERT_DMATRIX(A);

  if (C==A) {
    B = _dmatrixCreate(A->n+1,A->m);
    for (i=0; i<A->m; i++) {
      for (j=0; j<A->n; j++)
	B->matPA[j][i] = A->matPA[i][j];
      B->matPA[A->n][i] = 1.0;
    }
    _dmatrixResize(A,B->m,B->n);
    _dmatrixCopy(C,B);
    _dmatrixDestroy(B);
  } else {
    _dmatrixResize(C,A->n+1,A->m);
    for (i=0; i<A->m; i++) {
      for (j=0; j<A->n; j++)
	C->matPA[j][i] = A->matPA[i][j];
      C->matPA[A->n][i] = 1.0;
    }
  }
  
  return TCL_OK;
}



/* -------------------------------------------------------------------- */
/*  _dmatrixMSE                                                         */
/* -------------------------------------------------------------------- */
double  _dmatrixMSE (DMatrix *dmat) {

  register int       i,j;
  register double    sum;
  
  ASSERT_DMATRIX(dmat);

  sum = 0.0;
  for (i=0; i<dmat->m; i++)
    for (j=0; j<dmat->n; j++)
      sum += sq(dmat->matPA[i][j]);
  
  return (sum / (dmat->m * dmat->n));
}



/* -------------------------------------------------------------------- */
/*  _dmatrixScale     C = A * gamma     (C=A is possible)               */
/* -------------------------------------------------------------------- */
int  _dmatrixScale (DMatrix *C, DMatrix *A, double gamma) {

  register int      i,j;
  
  ASSERT_DMATRIX(C);
  ASSERT_DMATRIX(A);

  _dmatrixResize(C,A->m,A->n);
  for (i=0; i<A->m; i++)
    for (j=0; j<A->n; j++)
      C->matPA[i][j] = A->matPA[i][j] * gamma;

  return TCL_OK;
}



/* -------------------------------------------------------------------- */
/*  _dmatrixScaleCol                                                    */
/* -------------------------------------------------------------------- */
int  _dmatrixScaleCol (DMatrix *C, DMatrix *A, DMatrix *W) {

  register int      i,j;
  
  ASSERT_DMATRIX(C);
  ASSERT_DMATRIX(A);

  if (W->m != 1) {
    ERROR("_dmatrixScaleCol(): WARNING: W has more than one row\n");
  }

  if (W->n != A->n) {
    ERROR("_dmatrixScaleCol(): A,W dimensions do not match\n");
    return TCL_ERROR;
  }
  
  _dmatrixResize(C,A->m,A->n);
  for (i=0; i<A->m; i++)
    for (j=0; j<A->n; j++)
      C->matPA[i][j] = A->matPA[i][j] * W->matPA[0][j];
  
  return TCL_OK;
}



/* -------------------------------------------------------------------- */
/*  _dmatrixAdd     C = A + B                                           */
/* -------------------------------------------------------------------- */
int  _dmatrixAdd (DMatrix *C, DMatrix *A, DMatrix *B) {

  register int        i,j;
  
  ASSERT_DMATRIX(C);
  ASSERT_DMATRIX(A);
  ASSERT_DMATRIX(B);

  if ((A->m != B->m) || (A->n != B->n)) {
    ERROR("_dmatrixAdd(): dimensions do not match\n");
    return TCL_ERROR;
  }

  _dmatrixResize(C,A->m,A->n);

  for (i=0; i<A->m; i++)
    for (j=0; j<A->n; j++)
      C->matPA[i][j] = A->matPA[i][j] + B->matPA[i][j];

  return TCL_OK;
}



/* -------------------------------------------------------------------- */
/*  _dmatrixAccu     C = C + alpha*A                                    */
/* -------------------------------------------------------------------- */
int  _dmatrixAccu (DMatrix *C, DMatrix *A, double alpha) {

  register int        i,j;
  
  ASSERT_DMATRIX(C);
  ASSERT_DMATRIX(A);

  if ((A->m != C->m) || (A->n != C->n)) {
    ERROR("_dmatrixAccu(): dimensions do not match\n");
    return TCL_ERROR;
  }

  for (i=0; i<C->m; i++)
    for (j=0; j<C->n; j++)
      C->matPA[i][j] += alpha*A->matPA[i][j];

  return TCL_OK;
}



/* -------------------------------------------------------------------- */
/*  _dmatrixSub     C = A - B                                           */
/* -------------------------------------------------------------------- */
int  _dmatrixSub (DMatrix *C, DMatrix *A, DMatrix *B) {

  register int        i,j;
  
  ASSERT_DMATRIX(C);
  ASSERT_DMATRIX(A);
  ASSERT_DMATRIX(B);

  if ((A->m != B->m) || (A->n != B->n)) {
    ERROR("_dmatrixSub(): dimensions do not match\n");
    return TCL_ERROR;
  }

  _dmatrixResize(C,A->m,A->n);

  for (i=0; i<A->m; i++)
    for (j=0; j<A->n; j++)
      C->matPA[i][j] = A->matPA[i][j] - B->matPA[i][j];

  return TCL_OK;
}



/* -------------------------------------------------------------------- */
/*  _dmatrixProd     C = A * B  (component-wise)                        */
/* -------------------------------------------------------------------- */
int  _dmatrixProd (DMatrix *C, DMatrix *A, DMatrix *B) {

  register int        i,j;
  
  ASSERT_DMATRIX(C);
  ASSERT_DMATRIX(A);
  ASSERT_DMATRIX(B);

  if ((A->m != B->m) || (A->n != B->n)) {
    ERROR("_dmatrixProd(): dimensions do not match\n");
    return TCL_ERROR;
  }

  _dmatrixResize(C,A->m,A->n);

  for (i=0; i<A->m; i++)
    for (j=0; j<A->n; j++)
      C->matPA[i][j] = A->matPA[i][j] * B->matPA[i][j];

  return TCL_OK;
}



/* -------------------------------------------------------------------- */
/*  _dmatrixCosine             Computes the cosine of the angle between */
/*                          two given matrices, interpreted as vectors  */
/* -------------------------------------------------------------------- */
double  _dmatrixCosine (DMatrix *A, DMatrix *B) {

  double              *a,*b;
  register double     sum,sumA,sumB;
  register int        i,j;

  ASSERT_DMATRIX(A);
  ASSERT_DMATRIX(B);

  if ((A->m != B->m) || (A->n != B->n)) {
    ERROR("_dmatrixCosine(): matrices have different size\n");
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

  return (sum/(sqrt(sumA)*sqrt(sumB)));
}



/* -------------------------------------------------------------------- */
/*  _dmatrixMulT    C=A*Bt     B might be a NN weight matrix containing */
/*                            a bias weight column at the far right     */
/* -------------------------------------------------------------------- */
int  _dmatrixMulT (DMatrix *C, DMatrix *A, DMatrix *B) {

  register double  a00,a01,a02,a03,a10,a11,a12,a13,
                   a20,a21,a22,a23,a30,a31,a32,a33;
  register double  b00,b01,b02,b03,b10,b11,b12,b13,
                   b20,b21,b22,b23,b30,b31,b32,b33;
  register double  c00,c01,c02,c03,c10,c11,c12,c13,
                   c20,c21,c22,c23,c30,c31,c32,c33;
  double           *a,*b,*c;
  register int     i,j,k,mA,mB,n,bias;

  ASSERT_DMATRIX(C);
  ASSERT_DMATRIX(A);
  ASSERT_DMATRIX(B);

  if ((C==A) || (C==B)) {
    ERROR("_dmatrixMulT(): C must be different from A and B\n");
    return TCL_ERROR;
  }

  if ((A->n != B->n) && (A->n+1 != B->n)) {
    ERROR("_dmatrixMulT(): dimensions do not match\n");
    return TCL_ERROR;
  }

  if (A->n+1 == B->n) bias=1; else bias=0;
  _dmatrixResize(C,A->m,B->m);
  _dmatrixInitConst(C,0.0);

  /* ------------------------------------ */
  /*  first pass in junks of 4x4 elements */
  /* ------------------------------------ */
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

        c00 = a00*b00 + a01*b01 + a02*b02 + a03*b03;
	c01 = a00*b10 + a01*b11 + a02*b12 + a03*b13;
	c02 = a00*b20 + a01*b21 + a02*b22 + a03*b23;
	c03 = a00*b30 + a01*b31 + a02*b32 + a03*b33;

        c10 = a10*b00 + a11*b01 + a12*b02 + a13*b03;
	c11 = a10*b10 + a11*b11 + a12*b12 + a13*b13;
	c12 = a10*b20 + a11*b21 + a12*b22 + a13*b23;
	c13 = a10*b30 + a11*b31 + a12*b32 + a13*b33;
        
        c20 = a20*b00 + a21*b01 + a22*b02 + a23*b03;
	c21 = a20*b10 + a21*b11 + a22*b12 + a23*b13;
	c22 = a20*b20 + a21*b21 + a22*b22 + a23*b23;
	c23 = a20*b30 + a21*b31 + a22*b32 + a23*b33;
        
        c30 = a30*b00 + a31*b01 + a32*b02 + a33*b03;
	c31 = a30*b10 + a31*b11 + a32*b12 + a33*b13;
	c32 = a30*b20 + a31*b21 + a32*b22 + a33*b23;
	c33 = a30*b30 + a31*b31 + a32*b32 + a33*b33;

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

        c00 = a00*b00 + a01*b01 + a02*b02 + a03*b03;
	c01 = a00*b10 + a01*b11 + a02*b12 + a03*b13;
	c02 = a00*b20 + a01*b21 + a02*b22 + a03*b23;
	c03 = a00*b30 + a01*b31 + a02*b32 + a03*b33;

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
        a = A->matPA[j];
        b = B->matPA[k];
	c = C->matPA[j] + k;
        for (i=n; i<A->n; i++)
  	  *c += a[i] * b[i];
        *c += b[B->n-1];
      }
    }
  } else {
    for (j=0; j<A->m; j++) {
      for (k=0; k<B->m; k++) {
        a = A->matPA[j];
        b = B->matPA[k];
	c = C->matPA[j] + k;
        for (i=n; i<B->n; i++)
  	  *c += a[i] * b[i];
      }
    }
  }

  return TCL_OK;
}



/* -------------------------------------------------------------------- */
/*  _dmatrixMul   C=A*B        B might be a NN weight matrix containing */
/*                            a bias weight column at the far right     */
/* -------------------------------------------------------------------- */
int  _dmatrixMul (DMatrix *C, DMatrix *A, DMatrix *B) {

  DMatrix     *D;
  
  ASSERT_DMATRIX(C);
  ASSERT_DMATRIX(A);
  ASSERT_DMATRIX(B);

  if ((C==A) || (C==B)) {
    ERROR("_dmatrixMul(): C must be different from A and B\n");
    return TCL_ERROR;
  }

  if ((A->n != B->m) && (A->n+1 != B->m)) {
    ERROR("_dmatrixMul(): dimensions do not match\n");
    return TCL_ERROR;
  }

  D = _dmatrixCreate(B->n,B->m);
  _dmatrixTrans(D,B);
  _dmatrixMulT(C,A,D);
  _dmatrixDestroy(D);

  return TCL_OK;
}



/* -------------------------------------------------------------------- */
/*  _dmatrixMul1   C = A * B      B is considered to have an additional */
/*                             implicit column of 1's on the far right  */
/* -------------------------------------------------------------------- */
int  _dmatrixMul1 (DMatrix *C, DMatrix *A, DMatrix *B) {

  DMatrix     *D;
  
  ASSERT_DMATRIX(C);
  ASSERT_DMATRIX(A);
  ASSERT_DMATRIX(B);

  if ((C==A) || (C==B)) {
    ERROR("_dmatrixMul1(): C must be different from A and B\n");
    return TCL_ERROR;
  }

  if (A->n != B->m) {
    ERROR("_dmatrixMul1(): dimensions do not match\n");
    return TCL_ERROR;
  }

  D = _dmatrixCreate(B->n,B->m);
  _dmatrixTrans1(D,B);
  _dmatrixMulT(C,A,D);
  _dmatrixDestroy(D);

  return TCL_OK;
}



/* -------------------------------------------------------------------- */
/*  _dmatrixRadT  C = (A - Bt)^2          B might be a NN weight matrix */
/*                           a variance weight column at the far right  */
/* -------------------------------------------------------------------- */
int  _dmatrixRadT (DMatrix *C, DMatrix *A, DMatrix *B) {

  register double  a00,a01,a02,a03,a10,a11,a12,a13,
                   a20,a21,a22,a23,a30,a31,a32,a33;
  register double  b00,b01,b02,b03,b10,b11,b12,b13,
                   b20,b21,b22,b23,b30,b31,b32,b33;
  register double  c00,c01,c02,c03,c10,c11,c12,c13,
                   c20,c21,c22,c23,c30,c31,c32,c33;
  double           *a,*b,*c;
  register double  var;
  register int     i,j,k,mA,mB,n,bias;

  ASSERT_DMATRIX(C);
  ASSERT_DMATRIX(A);
  ASSERT_DMATRIX(B);

  if ((C==A) || (C==B)) {
    ERROR("_dmatrixRadT(): C must be different from A and B\n");
    return TCL_ERROR;
  }

  if ((A->n != B->n) && (A->n+1 != B->n)) {
    ERROR("_dmatrixRadT(): dimensions do not match\n");
    return TCL_ERROR;
  }

  if (A->n+1 == B->n) bias=1; else bias=0;
  _dmatrixResize(C,A->m,B->m);
  _dmatrixInitConst(C,0.0);

  /* ------------------------------------ */
  /*  first pass in junks of 4x4 elements */
  /* ------------------------------------ */
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
          ERROR("_dmatrixRadT(): negative variance\n");
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
/*  _dmatrixRad   C = A * B    B might be a NN weight matrix containing */
/*                           a variance weight column at the far right  */
/* -------------------------------------------------------------------- */
int  _dmatrixRad (DMatrix *C, DMatrix *A, DMatrix *B) {

  DMatrix     *D;
  
  ASSERT_DMATRIX(C);
  ASSERT_DMATRIX(A);
  ASSERT_DMATRIX(B);

  if ((C==A) || (C==B)) {
    ERROR("_dmatrixRad(): C must be different from A and B\n");
    return TCL_ERROR;
  }

  if ((A->n != B->m) && (A->n+1 != B->m)) {
    ERROR("_dmatrixRad(): dimensions do not match\n");
    return TCL_ERROR;
  }

  D = _dmatrixCreate(B->n,B->m);
  _dmatrixTrans(D,B);
  _dmatrixRadT(C,A,D);
  _dmatrixDestroy(D);

  return TCL_OK;
}



/* -------------------------------------------------------------------- */
/*  _dmatrixInv     C = A^-1   (C=A is possible)                        */
/* -------------------------------------------------------------------- */
int  _dmatrixInv (DMatrix *C, DMatrix *A) {

  DMatrix              *U,*V,*W;
  register int      i,j,n = A->n;
  
  ASSERT_DMATRIX(C);
  ASSERT_DMATRIX(A);

  if (A->m != A->n) {
    ERROR("_dmatrixInv(): A is not a square matrix\n");
    return TCL_ERROR;
  }

  /* ----------------------------- */
  /*  allocate temporary matrices  */
  /* ----------------------------- */
  U = _dmatrixCreate(n,n);
  V = _dmatrixCreate(n,n);
  W = _dmatrixCreate(1,n);
  _dmatrixCopy(U,A);

  /* -------------------------------------- */
  /*  compute singular value decomposition  */
  /* -------------------------------------- */
  _dsvdcmp(U->matPA,n,n,W->matPA[0],V->matPA);
  _dsvclean(W->matPA[0],n);
  
  /* --------------------- */
  /*  compute the inverse  */
  /* --------------------- */
  for (j=0; j<n; j++) {
    if (W->matPA[0][j] < 1E-12) {
      W->matPA[0][j] = 1E-12;
      ERROR("_dmatrixInv(): WARNING: matrix is ill-conditioned\n");
    }
    for (i=0; i<n; i++)
      U->matPA[i][j] /= W->matPA[0][j];
  }

  _dmatrixMulT(C,V,U);
  _dmatrixDestroy(U);
  _dmatrixDestroy(V);
  _dmatrixDestroy(W);

  return TCL_OK;
}



/* -------------------------------------------------------------------- */
/*  _dmatrixLS        Linear Least Squares       D = XWX^-1 YWX         */
/* -------------------------------------------------------------------- */
int  _dmatrixLS (DMatrix *D, DMatrix *W, DMatrix *Y, DMatrix *X) {

  DMatrix     *Xt,*WX,*XWX,*YWX;
  
  ASSERT_DMATRIX(D);
  ASSERT_DMATRIX(W);
  ASSERT_DMATRIX(Y);
  ASSERT_DMATRIX(X);

  if ((W->m != 1) || (W->n != X->m)) {
    ERROR("_dmatrixLS(): W is not a 1x%d matrix\n",X->m);
    return TCL_ERROR;
  }

  if (Y->m != X->m) {
    ERROR("_dmatrixLS(): Y is not a %dx* matrix\n",X->m);
    return TCL_ERROR;
  }

  Xt = _dmatrixCreate(X->n,X->m);
  WX = _dmatrixCreate(X->n,X->m);
  XWX = _dmatrixCreate(X->n,X->n);
  YWX = _dmatrixCreate(Y->n,X->n);
  
  _dmatrixTrans(Xt,X);
  _dmatrixScaleCol(WX,Xt,W);
  _dmatrixMulT(XWX,Xt,WX);
  _dmatrixInv(XWX,XWX);

  _dmatrixMul(YWX,WX,Y);
  _dmatrixMul(D,XWX,YWX);
  _dmatrixTrans(D,D);

  _dmatrixDestroy(Xt);
  _dmatrixDestroy(WX);
  _dmatrixDestroy(XWX);
  _dmatrixDestroy(YWX);
  
  return TCL_OK;
}



/* -------------------------------------------------------------------- */
/*  _dmatrixEigen                  Eigenvectors V and Eigenvalues D of  */
/*                                real symetric matrices A              */
/* -------------------------------------------------------------------- */
int  _dmatrixEigen (DMatrix *V, DMatrix *D, DMatrix *A) {

  DMatrix       *E;

  ASSERT_DMATRIX(V);  
  ASSERT_DMATRIX(D);  
  ASSERT_DMATRIX(A);  

  if (A->m != A->n) {
    ERROR("_dmatrixEigen(): A is not a square matrix\n");
    return TCL_ERROR;
  }
  
  if (V != A) {
    _dmatrixResize(V,A->m,A->n);
    _dmatrixCopy(V,A);
  }
  
  _dmatrixResize(D,1,A->n);
  E = _dmatrixCreate(1,A->n);
  
  tred2(V->matPA,V->n,D->matPA[0],E->matPA[0]);
  tqli(D->matPA[0],E->matPA[0],V->n,V->matPA);

  _dmatrixDestroy(E);
  
  return TCL_OK;
}



/* -------------------------------------------------------------------- */
/*  _dmatrixDet                          log of determinant of matrix A */
/* -------------------------------------------------------------------- */
int  _dmatrixDet (DMatrix *A, double *logdet, double *sign) {

  DMatrix        *D;
  int            k;
  double         d,sum,val;

  ASSERT_DMATRIX(A);

  if (A->m != A->n) {
    ERROR("_dmatrixDet(): A must be squared\n");
    return 0.0;
  }

  D = _dmatrixCreate(A->m,A->n);
  _dmatrixCopy(D,A);

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

  _dmatrixDestroy(D);

  return TCL_OK;
}



/* ------------------------------------------------------------------------ */
/*  _dmatrixSave                                                            */
/* ------------------------------------------------------------------------ */
int  _dmatrixSave (DMatrix *A, FILE *fp) {

  register int        i,j;

  ASSERT_DMATRIX(A);

  write_int(fp,A->m);
  write_int(fp,A->n);
  
  for (i=0; i<A->m; i++)
    for (j=0; j<A->n; j++)
      write_float(fp,A->matPA[i][j]);

  return TCL_OK;
}



/* ------------------------------------------------------------------------ */
/*  _dmatrixLoad                                                            */
/* ------------------------------------------------------------------------ */
int  _dmatrixLoad (DMatrix *A, FILE *fp) {

  register int         i,j,m,n;

  ASSERT_DMATRIX(A);

  m = read_int(fp);
  n = read_int(fp);

  _dmatrixResize(A,m,n);

  for (i=0; i<A->m; i++)
    for (j=0; j<A->n; j++)
      A->matPA[i][j] = read_float(fp);

  return TCL_OK;
}




/* ======================================================================== */
/*                            conversion routines                           */
/* ======================================================================== */



/* -------------------------------------------------------------------- */
/*  _fmatrix2dmatrix                                                    */
/* -------------------------------------------------------------------- */
int  _fmatrix2dmatrix (DMatrix *C, FMatrix *A) {

  register int   i,j;

  ASSERT_DMATRIX(C);
  ASSERT_FMATRIX(A);

  _dmatrixResize(C,A->m,A->n);

  for (i=0; i<A->m; i++)
    for (j=0; j<A->n; j++)
      C->matPA[i][j] = (double) A->matPA[i][j];

  return TCL_OK;
}



/* -------------------------------------------------------------------- */
/*  _dmatrix2fmatrix                                                    */
/* -------------------------------------------------------------------- */
int  _dmatrix2fmatrix (FMatrix *C, DMatrix *A) {

  register int   i,j;
  
  ASSERT_FMATRIX(C);
  ASSERT_DMATRIX(A);

  _fmatrixResize(C,A->m,A->n);

  for (i=0; i<A->m; i++)
    for (j=0; j<A->n; j++)
      C->matPA[i][j] = (float) A->matPA[i][j];

  return TCL_OK;
}


/* ======================================================================
   init
   ====================================================================== */

static int ffmatInitialized = 0;

int FFMat_Init (void) {

  if ( !ffmatInitialized ) {

    chaosRandomInit(0.123456789);

    ffmatInitialized = 1;
  }

  return TCL_OK;
}

