/* ========================================================================
    JANUS      Speech- to Speech Translation
               ------------------------------------------------------------
               Object: Feature Module
               ------------------------------------------------------------

    Author  :  Hagen Soltau
    Module  :  formants.c
    Date    :  $Id: formants.c 700 2000-11-14 12:35:27Z janus $
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
    Revision 1.3  2000/11/14 12:29:33  janus
    Merging branch jtk-00-10-27-jmcd.

    Revision 1.2.30.2  2000/11/08 17:13:42  janus
    Finished making changes required for compilation under 'gcc -Wall'.

    Revision 1.2.30.1  2000/11/06 10:50:29  janus
    Made changes to enable compilation under -Wall.

    Revision 1.2  2000/08/16 09:23:00  soltau
    *** empty log message ***

    Revision 1.1  1999/02/02 13:51:25  soltau
    Initial revision


   ======================================================================== */

char formantsRCSVersion[]= 
           "@(#)1$Id: formants.c 700 2000-11-14 12:35:27Z janus $";


#include <math.h>
#include "common.h" 
#include "itf.h"

/* ------------------------------------------------------------------------
    auto correlation 
   ------------------------------------------------------------------------ */

int fesAutoCor(float *X,float *R,float *A,float *B,int N, int K) {
  int k,m;
  double norm = 1.0 * N;

  for (k=0;k<K;k++) {
     double sum=0.0;
     for (m=0;m<N-k-1;m++)
       sum+=X[m]*X[m+k] / norm;
     R[k]=sum;
   }

  return TCL_OK;
}


/* ------------------------------------------------------------------------
    durbin's recursion for lpc: see  Rabiner pp. 411
   ------------------------------------------------------------------------ */

int fesLPC(float *R,float **A,float *E,float *K,int p_) {
  int p= p_ -1;
  int i,j;

  E[0]=R[0];

  for (i=1;i<=p;i++) {

    /*eq. 8.68 */
    K[i]=R[i];
    for (j=1;j<=i-1;j++) 
      K[i]-= A[j][i-1] * R[i-j];
    K[i] /= E[i-1];

    /*eq. 8.69 */
    A[i][i]=K[i];

    /*eq. 8.70 */
    for (j=1;j<=i-1;j++) 
      A[j][i]= A[j][i-1] - K[i]*A[i-j][i-1];

   /*eq. 8.71 */
    E[i]=(1-K[i]*K[i]) * E[i-1];
  }
  return TCL_OK;
}

/* ------------------------------------------------------------------------
   laguerre (Roots finding) : see Numerical recipes pp. 280
   ------------------------------------------------------------------------ */

typedef struct {
  float r,i;
} fcomplex;

float absC(fcomplex *a) {return sqrt(a->r*a->r + a->i*a->i);}

void mulC(fcomplex *c,fcomplex *a, fcomplex *b) 
{
  fcomplex t;
  t.r= a->r*b->r - a->i*b->i;
  t.i= a->r*b->i + a->i*b->r;
  c->r=t.r; c->i=t.i;
}

void divC(fcomplex *c,fcomplex *a,fcomplex *b) 
{
  float r,den;
  fcomplex t;

  if (fabs(b->r) >= fabs(b->i)) {
    r=b->i/b->r; 
    den=b->r+r*b->i;
    if (den == 0) {
       ERROR("divC: divison by zero\n"); 
       c->r=c->i=0.0;
       return;
    }
    t.r=(a->r+r*a->i)/den;
    t.i=(a->i-r*a->r)/den;    
  } else {
    r=b->r/b->i; 
    den=b->i+r*b->r;
    if (den == 0) {
      ERROR("divC: divison by zero\n"); 
      c->r=c->i=0.0;
      return;
    }
    t.r=(r*a->r+a->i)/den;
    t.i=(r*a->i-a->r)/den;
  }
  c->r=t.r; c->i=t.i;
}

void sqrtC(fcomplex *c,fcomplex *z) {
  float r,w;
  float x=fabs(z->r); 
  float y=fabs(z->i);
  fcomplex t;

  if (x == 0.0 && y == 0.0) {
    c->r=c->i=0.0;
    return;
  }

  if (x >= y) {
    r = y/x;
    w = sqrt(x)*sqrt(0.5*(1.0+sqrt(1.0+r*r)));
  }
  else {
    r = x/y;
    w = sqrt(y)*sqrt(0.5*(r+sqrt(1.0+r*r)));
  }
  if (z->r >= 0.0) {
    t.r = w;
    t.i = z->i/(2.0*w);
  } else {
    t.i = (z->i>=0) ? w : -w;
    t.r = z->i / (2.0*t.i);
  }
  c->r=t.r; c->i=t.i;
}
  
#define EPSS  3.e-7
#define MAXIT 50

void laguerre (fcomplex* a,int m,fcomplex* x,int polish, float eps) 
{
  int j,iter;
  float err,dxold,cdx,abx;
  fcomplex sq,h,gp,gm,g2,g,b,d,dx,f,x1;
  fcomplex t;

  dxold=absC(x);
  for (iter=1;iter<=MAXIT;iter++) {
    b.r=a[m].r; b.i=a[m].i;
    err=absC(&b);
    d.r=d.i=0.0;
    f.r=f.i=0.0;
    abx=absC(x);
    for (j=m-1;j>=0;j--) {
      mulC(&f,&f,x);
      f.r+=d.r; f.i+=d.i;
      mulC(&d,&d,x);
      d.r+=b.r; d.i+=b.i;
      mulC(&b,&b,x);
      b.r+=a[j].r; b.i+=a[j].i;
      err=absC(&b)+abx*err;
    }
    err *=EPSS;
    if(absC(&b) <= err) return;
    divC(&g,&d,&b);
    mulC(&g2,&g,&g);

    divC(&h,&f,&b);
    h.r=g2.r - 2.0*h.r;  
    h.i=g2.i - 2.0*h.i;
    
    h.r= (m-1.0)* (m*h.r -g2.r);
    h.i= (m-1.0)* (m*h.i -g2.i);
    sqrtC(&sq,&h);
    
    gp.r=g.r+sq.r; gp.i=g.i+sq.i; 
    gm.r=g.r-sq.r; gm.i=g.i-sq.i; 
    
    if (absC(&gp) < absC(&gm)) {gp.r=gm.r; gp.i=gm.i;}
    t.r=m; t.i=0;  divC(&dx,&t,&gp);
    
    x1.r=x->r-dx.r; x1.i=x->i-dx.i;    

    if (x->r == x1.r && x->i == x1.i) return;

    x->r=x1.r; x->i=x1.i;
    cdx=absC(&dx);
    dxold=cdx;
    if (! polish)
      if (cdx <= eps*absC(x)) return;
  }
}

#define EPS 1.0e-5


static int oldM = 0;
static fcomplex *ad = NULL;

void zroots (fcomplex *a,int m, fcomplex *roots, int polish) 
{
  int jj,j,i;
  fcomplex x,b,c;

  if (oldM < m +1) {
    oldM=m+1;
    ad =(fcomplex*) realloc(ad,(m+1)*sizeof(fcomplex));
  }
  assert (ad !=NULL);

  for (j=0;j<=m;j++) {
    ad[j].r=a[j].r; ad[j].i=a[j].i;
  }

  for (j=m;j>=1;j--) {
    x.r=x.i=0.0;
    laguerre(ad,j,&x,EPS,0);
    if(fabs(x.i) <= (2.0*EPS*fabs(x.r))) x.i=0.0;
    roots[j].r=x.r; roots[j].i=x.i; 
    b.r=ad[j].r; b.i=ad[j].i;
    for (jj=j-1;jj>=0;jj--) {
      c.r=ad[jj].r; c.i=ad[jj].i;
      ad[jj].r =b.r; ad[jj].i =b.i;

      mulC(&b,&b,&x);
      b.r+= c.r; b.i+=c.i;
    }
  }

  if (polish) 
    for (j=1;j<=m;j++) 
      laguerre(a,m,roots+j,EPS,1);    

  for (j=2;j<=m;j++) {
    x.r=roots[j].r;  x.i=roots[j].i; 
    for (i=j-1;i>=1;i--) {
      if (roots[i].r <= x.r) break;
      roots[i+1].r=roots[i].r; roots[i+1].i=roots[i].i; 
    }
    roots[i+1].r=x.r;  roots[i+1].i=x.i; 
  }
  
  if (0) {
    fprintf(stderr,"Polynom:\n");
    for (j=0;j<=m;j++) fprintf(stderr,"  %4.4f  %4.4f\n",a[j].r,a[j].i);
    fprintf(stderr,"Roots:\n");
    for (j=0;j<=m;j++) fprintf(stderr,"  %4.4f  %4.4f\n",roots[j].r,roots[j].i);
  }

}

/* ------------------------------------------------------------------------
    eval complex polynom a(x)
   ------------------------------------------------------------------------ */

void evalPoly(fcomplex *r,fcomplex *a, int coeffN,fcomplex *x) {
  int i;
  fcomplex t,y;
  
  y.r=1.0; y.i=0.0;
  r->r=r->i=0.0;
  for (i=0;i<=coeffN;i++) {
    mulC(&t,a+i,&y);
    r->r += t.r;
    r->i += t.i;
    mulC(&y,&y,x);
  }
}


/* ------------------------------------------------------------------------
    formants : see Rabiner p. 442
   ------------------------------------------------------------------------ */

typedef struct {
  float a,b;
} f2;

static int cmp1(const void *x_,const void *y_)
{
  f2* xf2 = (f2*) x_;
  f2* yf2 = (f2*) y_;
  float x= xf2->a;
  float y= yf2->a;
  if (x > y)  return 1;
  if (x < y)  return -1;
  return 0;
}

static int cmp2(const void *x_,const void *y_)
{
  f2* xf2 = (f2*) x_;
  f2* yf2 = (f2*) y_;
  float x= xf2->b;
  float y= yf2->b;
  if (x > y)  return 1;
  if (x < y)  return -1;
  return 0;
}

static int oldCoeffN=0;
static fcomplex *poly    = NULL;
static fcomplex *roots   = NULL;
static f2       *rootsFB = NULL;


int fesFormants(float *A,float *F, int coeffN,float samplingrate,int fN, float fMin, float fMax, float bandMax) {
  float o,s,t;
  int i, rootsN=0;

  if (oldCoeffN<coeffN) {
    oldCoeffN=coeffN;
    poly=  (fcomplex*) realloc(poly,coeffN*sizeof(fcomplex));
    roots= (fcomplex*) realloc(roots,coeffN*sizeof(fcomplex));
    rootsFB= (f2*)     realloc(rootsFB,coeffN*sizeof(f2));
  }

  assert (poly != NULL && roots!=NULL && rootsFB != NULL);

  for (i=0;i<coeffN;i++) {
    poly[i].r=A[i];
    poly[i].i=0.0;
    roots[i].r=roots[i].i=0.0;
  }

  zroots(poly,coeffN-1, roots, 1);
  
  if (0) {
    for (i=0;i< coeffN;i++) {
      fcomplex r;
      evalPoly(&r,poly,coeffN,roots+i);
      fprintf(stderr,"eval poly at root %d: %4.4f %4.4f\n",i,r.r,r.i);
    }
  }
    
  for (i=1;i< coeffN;i++) {
    t=absC(roots+i);
    if (t==0)  continue;      
    if (roots[i].i < 1.e-10) continue; 

    o=atan2(roots[i].i,roots[i].r);
    o=fabs(o* samplingrate/ (2*M_PI));

    s=log(roots[i].r*roots[i].r + roots[i].i*roots[i].i);
    s=fabs(s* samplingrate/ (4*M_PI));

    if (0) {
      fprintf(stderr,"root %d: %4.4f %4.4f  %4.4f %4.4f\n",i,roots[i].r,roots[i].i,o,s);
    }

    if (o>fMin && o<fMax && s<bandMax) {
      rootsFB[rootsN].a= o;    
      rootsFB[rootsN].b= s;    
      rootsN++;
    }
  }
  
  if (rootsN<fN) fN=rootsN;

  qsort(rootsFB,rootsN,sizeof(f2),cmp2);  
  qsort(rootsFB,fN,sizeof(f2),cmp1);

  for (i=0;i<fN;i++)
    F[i]=rootsFB[i].a;

  return TCL_OK;
}
    




