/* ========================================================================
    JANUS      Speech- to Speech Translation
               ------------------------------------------------------------
               Object: Multinomial Generalized Linear Model Classifier
               ------------------------------------------------------------

    Author  :  Juergen Fritsch
    Module  :  glimClass.c
    Date    :  $Id: glimClass.c 2390 2003-08-14 11:20:32Z fuegen $
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
    Revision 3.4  2003/08/14 11:20:20  fuegen
    Merged changes on branch jtk-01-01-15-fms (jaguar -> ibis-013)

    Revision 3.3.4.2  2002/06/26 12:26:50  fuegen
    go away, boring message

    Revision 3.3.4.1  2002/06/26 11:57:57  fuegen
    changes for new directory structure support and code cleaning

    Revision 3.3  2001/01/15 09:49:57  janus
    Merged Christian's version with WINDOWS support (branch jtk-00-11-14-fuegen)

    Revision 3.2.40.1  2001/01/12 15:16:54  janus
    necessary changes for running janus under WINDOWS

    Revision 3.2  1997/06/17 17:41:43  westphal
    *** empty log message ***

 * Revision 1.1  96/09/27  08:59:45  fritsch
 * Initial revision
 * 
    Revision 1.1  1995/12/21  13:49:57  fritsch
    Initial revision
 
   ======================================================================== */


#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "common.h"
#include "itf.h"

#include "error.h"
#include "matrix.h"
#include "mach_ind_io.h"
#include "feature.h"
#include "classifier.h"
#include "glimClass.h"



/* ======================================================================= */
/*                       (1)  local implementation                         */
/* ======================================================================= */


#define          square(X)             ((X)*(X))
#define          MINVAL                1E-20
#define          MAXVAL                1E+20
#define          GLIM_MAGIC            1010
#define          GLIM_ACCU_MAGIC       2020
#define          SVD_THRESHOLD         1E-6

int  glimClassFreeAccus (GlimClass *glimClass);
int  glimClassClearAccus (GlimClass *glimClass);
int  glimClassUpdateGA (GlimClass *glimClass);

/* ------------------------------------------------------------------------ */
/*  initializing the custom chaotic pseudo-random number generator          */
/* ------------------------------------------------------------------------ */

static double GlobalRandX;
static double GlobalRandY;
static double GlobalRandZ;

int  myRandomInit (double seed) {

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
float   myRandom (double a, double b) {

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



/* ------------------------------------------------------------------------ */
/*  matInitRandom -- initializes a mxn matrix with random numbers in the    */
/*                   range [-initMax,+initMax]                              */
/* ------------------------------------------------------------------------ */
int  matInitRandom (double **mat, int m, int n, double initMax) {

  register int      i,j;

  if (mat == NULL) {
    ERROR("matrix does not exist\n");
    return TCL_ERROR;
  }

  if (initMax < 0.0) initMax = -initMax;
  
  for (i=0; i<m; i++)
    for (j=0; j<n; j++)
      mat[i][j] = myRandom(-initMax,+initMax);

  return TCL_OK;
}



/* ----------------------------------------------------------------------- */
/*  matInvert -- uses SVD algorithm (see matrix.[ch]) to invert a matrix   */
/* ----------------------------------------------------------------------- */
double  matInvert (double **a, int n) {

  register int         i,j,k;
  register double      sum,mse;
  double               *w,**b,**u,**v;

  if (a==NULL) return 0.0;

  /* ---------------------------------------- */
  /*  allocate memory for temporary matrices  */
  /* ---------------------------------------- */
  w = (double *) malloc (sizeof(double) * n);
  for (i=0; i<n; i++) w[i] = 0.0;

  b = (double **) malloc (sizeof(double *) * n);
  for (i=0; i<n; i++) {
    b[i] = (double *) malloc (sizeof(double) * n);
    for (j=0; j<n; j++)
      b[i][j] = 0.0;
  }

  u = (double **) malloc (sizeof(double *) * n);
  for (i=0; i<n; i++) {
    u[i] = (double *) malloc (sizeof(double) * n);
    for (j=0; j<n; j++)
      u[i][j] = 0.0;
  }

  v = (double **) malloc (sizeof(double *) * n);
  for (i=0; i<n; i++) {
    v[i] = (double *) malloc (sizeof(double) * n);
    for (j=0; j<n; j++)
      v[i][j] = 0.0;
  }

  /* ----------------------------- */
  /*  copy matrix a into matrix u  */
  /* ----------------------------- */
  for (i=0; i<n; i++)
    for (j=0; j<n; j++)
      u[i][j] = a[i][j];

  /* -------------------------------------- */
  /*  compute singular value decomposition  */
  /* -------------------------------------- */
  dsvdcmp(u,n,n,w,v);
  dsvclean(w,n);

  /* --------------------- */
  /*  compute the inverse  */
  /* --------------------- */
  for (j=0; j<n; j++) {
    if (w[j] < MINVAL) w[j] = MINVAL;
    for (i=0; i<n; i++)
      u[i][j] /= w[j];
  }

  for (i=0; i<n; i++) {
    for (j=0; j<n; j++) {
      sum = 0.0;
      for (k=0; k<n; k++)
        sum += v[i][k] * u[j][k];
      b[i][j] = sum;
    }
  }

  /* ---------------------------------------- */
  /*  measure the mean square error of a*b^-1 */
  /* ---------------------------------------- */
  mse = 0.0;
  for (i=0; i<n; i++) {
    for (j=0; j<n; j++) {
      sum = 0.0;
      for (k=0; k<n; k++)
        sum += a[i][k] * b[k][j];
      if (i==j) mse += square(1.0-sum);
          else  mse += square(sum);
    }
  }

  /* ------------------ */
  /*  copy b back to a  */
  /* ------------------ */
  for (i=0; i<n; i++)
    for (j=0; j<n; j++)
      a[i][j] = b[i][j];

  /* ----------------------------- */
  /*  free all temporary matrices  */
  /* ----------------------------- */
  free(w);
  
  for (i=0; i<n; i++) free(b[i]);
  free(b);

  for (i=0; i<n; i++) free(u[i]);
  free(u);

  for (i=0; i<n; i++) free(v[i]);
  free(v);
  
  return mse;
}



/* ----------------------------------------------------------------------- */
/*  glimClassCreate                                                        */
/* ----------------------------------------------------------------------- */
GlimClass  *glimClassCreate (int classN, int dimN, int trainMode,
			     int weightMode, double eta,
			     int batchN, double initMax, double logZero) {

  GlimClass              *glimClass;
  register int           classX;
  
  /* ------------------------------ */
  /*  create empty GLIM classifier  */
  /* ------------------------------ */
  glimClass = (GlimClass *) malloc (sizeof(GlimClass));
  glimClass->accu = NULL;

  /* ----------------------------- */
  /*  initialize scalar variables  */
  /* ----------------------------- */
  glimClass->classN     = classN;
  glimClass->dimN       = dimN;
  glimClass->trainMode  = trainMode;
  glimClass->weightMode = weightMode;
  glimClass->eta        = eta;
  glimClass->batchN     = batchN;
  glimClass->initMax    = initMax;
  glimClass->logZero    = logZero;
  
  /* ---------------------- */
  /*  create weight matrix  */
  /* ---------------------- */
  glimClass->w = (double **) malloc (sizeof(double *) * classN);
  for (classX=0; classX<classN; classX++)
    glimClass->w[classX] = (double *) malloc (sizeof(double) * (dimN+1));

  /* -------------------------- */
  /*  initialize weight matrix  */
  /* -------------------------- */
  matInitRandom(glimClass->w,classN,dimN+1,initMax);
  
  return glimClass;
}



/* ----------------------------------------------------------------------- */
/*  glimClassReset                                                         */
/* ----------------------------------------------------------------------- */
int  glimClassReset (GlimClass *glimClass) {

  if (glimClass)
    return matInitRandom(glimClass->w,glimClass->classN,glimClass->dimN+1,
	  	         glimClass->initMax);
  else return TCL_ERROR;
}



/* ----------------------------------------------------------------------- */
/*  glimClassFree                                                          */
/* ----------------------------------------------------------------------- */
int  glimClassFree (GlimClass *glimClass) {

  register int       classX;
  
  /* -------------------- */
  /*  free weight matrix  */
  /* -------------------- */
  for (classX=0; classX<glimClass->classN; classX++)
    free(glimClass->w[classX]);
  free(glimClass->w);

  /* ------------ */
  /*  free accus  */
  /* ------------ */
  glimClassFreeAccus(glimClass);

  /* ----------------------- */
  /*  free glimClass itself  */
  /* ----------------------- */
  free(glimClass);
  
  return TCL_ERROR;
}



/* ----------------------------------------------------------------------- */
/*  glimClassCopy                                                          */
/* ----------------------------------------------------------------------- */
GlimClass  *glimClassCopy (GlimClass *src, double perturbe) {

  GlimClass          *target;
  register int       classX,dimX;

  if (!src) return NULL;

  /* ------------------------------- */
  /*  create a new GlimClass object  */
  /* ------------------------------- */
  target = glimClassCreate(src->classN,src->dimN,src->trainMode,
			   src->weightMode,src->eta,
			   src->batchN,src->initMax,src->logZero);

  /* --------------------------------------------------- */
  /*  copy weight matrix and add random perturberations  */
  /* --------------------------------------------------- */
  for (classX=0; classX<src->classN; classX++)
    for (dimX=0; dimX<=src->dimN; dimX++)
      target->w[classX][dimX] = src->w[classX][dimX] +
	                        myRandom(-perturbe,+perturbe);
  
  return target;
}



/* ----------------------------------------------------------------------- */
/*  glimClassSave                                                          */
/* ----------------------------------------------------------------------- */
int  glimClassSave (GlimClass *glimClass, FILE *fp) {

  register int             classX,dimX;

  /* ------------- */
  /*  save header  */
  /* ------------- */
  write_int(fp,GLIM_MAGIC);
  write_int(fp,glimClass->classN);
  write_int(fp,glimClass->dimN);
  write_int(fp,glimClass->trainMode);
  write_int(fp,glimClass->weightMode);
  write_float(fp,(float) glimClass->eta);
  write_int(fp,glimClass->batchN);
  write_float(fp,(float) glimClass->initMax);
  write_float(fp,(float) glimClass->logZero);
  
  /* -------------------- */
  /*  save weight matrix  */
  /* -------------------- */
  for (classX=0; classX<glimClass->classN; classX++)
    for (dimX=0; dimX<=glimClass->dimN; dimX++)
      write_float(fp,(float) glimClass->w[classX][dimX]);

  return TCL_OK;
}



/* ----------------------------------------------------------------------- */
/*  glimClassLoad                                                          */
/* ----------------------------------------------------------------------- */
GlimClass  *glimClassLoad (GlimClass *glimClass, FILE *fp) {

  GlimClass              *newGlim;
  register int           classX,dimX;
  
  if (read_int(fp) != GLIM_MAGIC) {
    ERROR("couldn't find GLIM magic\n");
    return NULL;
  }

  /* ------------------------- */
  /*  check for existing GLIM  */
  /* ------------------------- */
  if (glimClass) {

    /* -------------------- */
    /*  free weight matrix  */
    /* -------------------- */
    for (classX=0; classX<glimClass->classN; classX++)
      free(glimClass->w[classX]);
    free(glimClass->w);

    /* ------------ */
    /*  free accus  */
    /* ------------ */
    glimClassFreeAccus(glimClass);

    newGlim = glimClass;

  } else {

    /* ------------------- */
    /*  create a new GLIM  */
    /* ------------------- */
    newGlim = (GlimClass *) malloc (sizeof(GlimClass));
    newGlim->accu = NULL;
    
  }

  /* ------------- */
  /*  load header  */
  /* ------------- */
  newGlim->classN     = read_int(fp);
  newGlim->dimN       = read_int(fp);
  newGlim->trainMode  = read_int(fp);
  newGlim->weightMode = read_int(fp);
  newGlim->eta        = (double) read_float(fp);
  newGlim->batchN     = read_int(fp);
  newGlim->initMax    = (double) read_float(fp);
  newGlim->logZero    = (double) read_float(fp);
  
  /* -------------------- */
  /*  load weight matrix  */
  /* -------------------- */
  newGlim->w = (double **) malloc (sizeof(double *) * newGlim->classN);
  for (classX=0; classX<newGlim->classN; classX++) {
    newGlim->w[classX] = (double *) malloc (sizeof(double) * (newGlim->dimN+1));
    for (dimX=0; dimX<=newGlim->dimN; dimX++)
      newGlim->w[classX][dimX] = (double) read_float(fp);
  }
  
  return newGlim;
}



/* ----------------------------------------------------------------------- */
/*  glimClassCreateAccus                                                   */
/* ----------------------------------------------------------------------- */
int  glimClassCreateAccus (GlimClass *glimClass) {

  register int             classX,classN,dimX,dimN;
  
  if (glimClass->accu) return TCL_OK;

  /* ----------------------- */
  /*  create accu structure  */
  /* ----------------------- */
  classN = glimClass->classN;
  dimN   = glimClass->dimN;

  glimClass->accu = (GlimClassAccu *) malloc (sizeof(GlimClassAccu));
  glimClass->accu->classN = classN;
  glimClass->accu->dimN   = dimN;

  /* ------------------------ */
  /*  create XWX matrix accu  */
  /* ------------------------ */
  glimClass->accu->xwx = (double **) malloc (sizeof(double *) * (dimN+1));
  for (dimX=0; dimX<=dimN; dimX++)
    glimClass->accu->xwx[dimX] = (double *) malloc (sizeof(double) * (dimN+1));

  /* ------------------------ */
  /*  create XWV matrix accu  */
  /* ------------------------ */
  glimClass->accu->xwv = (double **) malloc (sizeof(double *) * (classN));
  for (classX=0; classX<classN; classX++)
    glimClass->accu->xwv[classX] = (double *) malloc (sizeof(double) * (dimN+1));
  
  return glimClassClearAccus(glimClass);
}



/* ----------------------------------------------------------------------- */
/*  glimClassFreeAccus                                                     */
/* ----------------------------------------------------------------------- */
int  glimClassFreeAccus (GlimClass *glimClass) {

  GlimClassAccu          *accu;
  register int           classX,dimX;
  
  if (glimClass->accu) {

    accu = glimClass->accu;
    
    /* ---------------------- */
    /*  free XWX matrix accu  */
    /* ---------------------- */
    for (dimX=0; dimX<=accu->dimN; dimX++)
      free(accu->xwx[dimX]);
    free(accu->xwx);
    
    /* ---------------------- */
    /*  free XWV matrix accu  */
    /* ---------------------- */
    for (classX=0; classX<accu->classN; classX++)
      free(accu->xwv[classX]);
    free(accu->xwv);
    
    /* --------------------- */
    /*  free accu structure  */
    /* --------------------- */
    free(accu);
    glimClass->accu = NULL;
  }
  
  return TCL_OK;
}



/* ----------------------------------------------------------------------- */
/*  glimClassClearAccus                                                    */
/* ----------------------------------------------------------------------- */
int  glimClassClearAccus (GlimClass *glimClass) {

  GlimClassAccu           *accu;
  register int            classX,dimX,k;
  
  if (glimClass->accu) {

    accu = glimClass->accu;
    
    /* ----------------------- */
    /*  clear XWX matrix accu  */
    /* ----------------------- */
    for (dimX=0; dimX<=accu->dimN; dimX++)
      for (k=0; k<=accu->dimN; k++)
	accu->xwx[dimX][k] = 0.0;
      
    /* ----------------------- */
    /*  clear XWV matrix accu  */
    /* ----------------------- */
    for (classX=0; classX<accu->classN; classX++)
      for (dimX=0; dimX<=accu->dimN; dimX++)
	accu->xwv[classX][dimX] = 0.0;
      
    /* ------------------ */
    /*  clear count accu  */
    /* ------------------ */
    accu->trainN = 0;
    
  } else WARN("no accus allocated for GLIM\n");
  
  return TCL_OK;
}



/* ----------------------------------------------------------------------- */
/*  glimClassSaveAccus                                                     */
/* ----------------------------------------------------------------------- */
int  glimClassSaveAccus (GlimClass *glimClass, FILE *fp) {
 
  GlimClassAccu                 *accu;
  register int                  classX,dimX,k;
  
  if (!glimClass->accu) {
    ERROR("no accus allocated for GLIM\n");
    return TCL_ERROR;
  }

  accu = glimClass->accu;

  /* -------------- */
  /*  write header  */
  /* -------------- */
  write_int(fp,GLIM_ACCU_MAGIC);
  write_int(fp,accu->classN);
  write_int(fp,accu->dimN);

  /* ---------------------- */
  /*  save XWX matrix accu  */
  /* ---------------------- */
  for (dimX=0; dimX<=accu->dimN; dimX++)
    for (k=0; k<=accu->dimN; k++)
      write_float(fp,(float) accu->xwx[dimX][k]);
      
  /* ---------------------- */
  /*  save XWV matrix accu  */
  /* ---------------------- */
  for (classX=0; classX<accu->classN; classX++)
    for (dimX=0; dimX<=accu->dimN; dimX++)
      write_float(fp,(float) accu->xwv[classX][dimX]);
      
  /* ----------------- */
  /*  save count accu  */
  /* ----------------- */
  write_int(fp,accu->trainN);
  
  return TCL_OK;
}



/* ----------------------------------------------------------------------- */
/*  glimClassLoadAccus                                                     */
/* ----------------------------------------------------------------------- */
int  glimClassLoadAccus (GlimClass *glimClass, FILE *fp) {

  GlimClassAccu                 *accu;
  register int                  classX,dimX,k;
  
  if (!glimClass->accu) {
    ERROR("no accus allocated for GLIM\n");
    return TCL_ERROR;
  }

  accu = glimClass->accu;

  /* ------------- */
  /*  read header  */
  /* ------------- */
  if (read_int(fp) != GLIM_ACCU_MAGIC) {
    ERROR("couldn't find GLIM_ACCU magic\n");
    return TCL_ERROR;
  }  
  if (read_int(fp) != accu->classN) {
    ERROR("incompatible number of output classes\n");
    return TCL_ERROR;
  }
  if (read_int(fp) != accu->dimN) {
    ERROR("incompatible feature dimensions\n");
    return TCL_ERROR;
  }

  /* ----------------------------- */
  /*  load + accu XWX matrix accu  */
  /* ----------------------------- */
  for (dimX=0; dimX<=accu->dimN; dimX++)
    for (k=0; k<=accu->dimN; k++)
      accu->xwx[dimX][k] += (double) read_float(fp);

  /* ----------------------------- */
  /*  load + accu XWV matrix accu  */
  /* ----------------------------- */
  for (classX=0; classX<accu->classN; classX++)
    for (dimX=0; dimX<=accu->dimN; dimX++)
      accu->xwv[classX][dimX] += (double) read_float(fp);

  /* ------------------------ */
  /*  load + accu count accu  */
  /* ------------------------ */
  accu->trainN += read_int(fp);
  
  return TCL_OK;
}




/* ----------------------------------------------------------------------- */
/*  glimClassAccuSoftGA                                                    */
/* ----------------------------------------------------------------------- */
int  glimClassAccuSoftGA (GlimClass *glimClass, float *pattern, int dimN,
			  double *target, double *act,
			  int classN, double weight) {

  GlimClassAccu        *accu;
  register int         classX,dimX;
  register double      factor;
  
  accu = glimClass->accu;
  assert(accu);

  /* ------------------------------- */
  /*  update sum of local gradients  */
  /* ------------------------------- */
  for (classX=0; classX<classN; classX++) {
    factor = weight * (target[classX] - act[classX]);
    for (dimX=0; dimX<dimN; dimX++)
      accu->xwv[classX][dimX] += factor*pattern[dimX];
    accu->xwv[classX][dimN] += factor;
  }

  accu->trainN++;

  /* --------------------- */
  /*  check for an update  */
  /* --------------------- */
  if (accu->trainN == glimClass->batchN) {
    glimClassUpdateGA(glimClass);
    glimClassClearAccus(glimClass);
  }
    
  return TCL_OK;
}



/* ----------------------------------------------------------------------- */
/*  glimClassAccuHardGA                                                    */
/* ----------------------------------------------------------------------- */
int  glimClassAccuHardGA (GlimClass *glimClass, float *pattern, int dimN,
			  int targetX, double *act,
			  int classN, double weight) {

  GlimClassAccu        *accu;
  static double        *f = NULL;
  static int           fN = 0;
  register int         classX,dimX;
  register double      factor,*xwv;
  register float       *p;
  
  accu = glimClass->accu;
  assert(accu);

  if (dimN % 16 == 0) {

    register float   p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15;

    if (classN > fN) {
      if (f != NULL) free(f);
      f = (double *) malloc (sizeof(double) * classN);
      fN = classN;
    }
    
    for (classX=0; classX<classN; classX++) {
      if (classX == targetX)
        f[classX] = weight * (1.0 - act[targetX]);
      else
        f[classX] = weight * (-act[classX]);

      accu->xwv[classX][dimN] += f[classX]; 
    }

    p = pattern;  
    for (dimX=0; dimX<dimN; dimX+=16, p+=16) {

      p0  = p[0];  p1  = p[1];  p2  = p[2];  p3  = p[3];
      p4  = p[4];  p5  = p[5];  p6  = p[6];  p7  = p[7];
      p8  = p[8];  p9  = p[9];  p10 = p[10]; p11 = p[11];
      p12 = p[12]; p13 = p[13]; p14 = p[14]; p15 = p[15];
    
      for (classX=0; classX<classN; classX++) {
        factor = f[classX];
        xwv = accu->xwv[classX]+dimX;
    
        xwv[0] += factor*p0;
        xwv[1] += factor*p1;
        xwv[2] += factor*p2;
        xwv[3] += factor*p3;
        xwv[4] += factor*p4;
        xwv[5] += factor*p5;
        xwv[6] += factor*p6;
        xwv[7] += factor*p7;
        xwv[8] += factor*p8;
        xwv[9] += factor*p9;
        xwv[10] += factor*p10;
        xwv[11] += factor*p11;
        xwv[12] += factor*p12;
        xwv[13] += factor*p13;
        xwv[14] += factor*p14;
        xwv[15] += factor*p15;
      
      }
    }
    
  } else {
  
    /* ---------------------------------------------- */
    /*  update sum of local gradients for all classes */
    /* ---------------------------------------------- */
    for (classX=0; classX<classN; classX++) {
      if (classX == targetX)
        factor = weight * (1.0 - act[targetX]);
      else
        factor = weight * (-act[classX]);
      xwv = accu->xwv[classX];
      p   = pattern;
      for (dimX=0; dimX<dimN; dimX++, xwv++, p++) {
        *xwv += factor*(*p);
      }
      *xwv += factor;
    }
  }
  
  accu->trainN++;

  /* --------------------- */
  /*  check for an update  */
  /* --------------------- */
  if (accu->trainN == glimClass->batchN) {
    glimClassUpdateGA(glimClass);
    glimClassClearAccus(glimClass);
  }  
  
  return TCL_OK;
}



/* ----------------------------------------------------------------------- */
/*  glimClassUpdateGA                                                      */
/* ----------------------------------------------------------------------- */
int  glimClassUpdateGA (GlimClass *glimClass) {

  GlimClassAccu        *accu;
  register int         classX,dimX;
  register double      factor;
  
  accu = glimClass->accu;
  assert(accu);

  if (accu->trainN == 0) {
    WARN("no training information accumulated\n");
    return TCL_OK;
  }

  /* ------------------------------ */
  /*  update using learning factor  */
  /* ------------------------------ */
  for (classX=0; classX<accu->classN; classX++) {
    factor = glimClass->eta / ((double) accu->trainN);
    for (dimX=0; dimX<=accu->dimN; dimX++)
      glimClass->w[classX][dimX] += factor * accu->xwv[classX][dimX];
  }

  return TCL_OK;
}



/* ----------------------------------------------------------------------- */
/*  glimClassAccuSoftLS                                                    */
/* ----------------------------------------------------------------------- */
int  glimClassAccuSoftLS (GlimClass *glimClass, float *pattern, int dimN,
			  double *target, int classN, double weight) {

  GlimClassAccu        *accu;
  static double        *feat = NULL;
  static int           featN = 0;
  register double      val,logTarget;
  register int         x,y;
  
  accu = glimClass->accu;
  assert(accu);
	 
  /* -------------------------- */
  /*  check size of feat array  */
  /* -------------------------- */
  if (dimN > featN) {
    if (feat != NULL) free(feat);
    feat = (double *) malloc (sizeof(double) * (dimN+1));
    featN = dimN;
  }
  
  /* --------------------------------- */
  /*  compute weighted feature vector  */
  /* --------------------------------- */
  for (x=0; x<dimN; x++)
    feat[x] = pattern[x] * weight;
  feat[dimN] = weight;

  /* ----------------------- */
  /*  accumulate XWX matrix  */
  /* ----------------------- */
  for (x=0; x<dimN; x++)
    for (y=x; y<=dimN; y++)
      accu->xwx[x][y] += pattern[x] * feat[y];
  
  accu->xwx[dimN][dimN] += weight;

  /* ----------------------- */
  /*  accumulate XWV matrix  */
  /* ----------------------- */
  for (x=0; x<classN; x++) {
    if ((val = target[x]) < MINVAL) val = MINVAL;
    logTarget = log(val);
    for (y=0; y<=dimN; y++)
      accu->xwv[x][y] += logTarget * feat[y];
  }

  accu->trainN++;
  
  return TCL_OK;
}



/* ----------------------------------------------------------------------- */
/*  glimClassAccuHardLS                                                    */
/* ----------------------------------------------------------------------- */
int  glimClassAccuHardLS (GlimClass *glimClass, float *pattern, int dimN,
			  int targetX, int classN, double weight) {

  GlimClassAccu        *accu;
  static double        *feat = NULL;
  static int           featN = 0;
  register double      logTarget;
  register int         x,y;

  accu = glimClass->accu;
  assert(accu);
  logTarget = glimClass->logZero;

  /* -------------------------- */
  /*  check size of feat array  */
  /* -------------------------- */
  if (dimN > featN) {
    if (feat != NULL) free(feat);
    feat = (double *) malloc (sizeof(double) * (dimN+1));
    featN = dimN;
  }
  
  /* --------------------------------- */
  /*  compute weighted feature vector  */
  /* --------------------------------- */
  for (x=0; x<dimN; x++)
    feat[x] = pattern[x] * weight;
  feat[dimN] = weight;
  
  /* ----------------------- */
  /*  accumulate XWX matrix  */
  /* ----------------------- */
  for (x=0; x<dimN; x++)
    for (y=x; y<=dimN; y++)
      accu->xwx[x][y] += pattern[x] * feat[y];
  
  accu->xwx[dimN][dimN] += weight;

  /* ----------------------- */
  /*  accumulate XWV matrix  */
  /* ----------------------- */
  for (x=0; x<classN; x++) {
    if (x != targetX) {
      for (y=0; y<=dimN; y++)
        accu->xwv[x][y] += logTarget * feat[y];
    }
  }

  accu->trainN++;

  return TCL_OK;
}



/* ----------------------------------------------------------------------- */
/*  glimClassUpdateLS                                                      */
/* ----------------------------------------------------------------------- */
int  glimClassUpdateLS (GlimClass *glimClass) {

  GlimClassAccu        *accu;
  register double      sum,mse;
  register int         classX,dimX,k;
  
  accu = glimClass->accu;
  assert(accu);

  if (accu->trainN == 0) {
    WARN("no training information accumulated\n");
    return TCL_OK;
  }
  
  /* --------------------- */
  /*  complete XWX matrix  */ 
  /* --------------------- */
  for (dimX=0; dimX<accu->dimN; dimX++)
    for (k=dimX+1; k<=accu->dimN; k++)
      accu->xwx[k][dimX] = accu->xwx[dimX][k];

  /* ------------------------------- */
  /*  compute inverse of XWX matrix  */
  /* ------------------------------- */
  mse = matInvert(accu->xwx,accu->dimN+1);
  if (mse > SVD_THRESHOLD)
    WARN("matrix inversion: MSE (%15.12f) exceeding threshold\n",mse);

  /* ---------------------- */
  /*  compute (XWX)^-1 XWV  */
  /* ---------------------- */
  for (classX=0; classX<accu->classN; classX++) {
    for (dimX=0; dimX<=accu->dimN; dimX++) {
      sum = 0.0;
      for (k=0; k<=accu->dimN; k++)
	sum += (accu->xwx[dimX][k] * accu->xwv[classX][k]);
      glimClass->w[classX][dimX] = sum;
    }
  }
      
  return TCL_OK;
}




/* ======================================================================= */
/*                     (2) janus3 classifier functions                     */
/* ======================================================================= */



/* ----------------------------------------------------------------------- */
/*  glimClassCreateClf                                                     */
/* ----------------------------------------------------------------------- */
ClientData  glimClassCreateClf (int classN, int dimN, int argc, char *argv[]) {

  int                ac = argc-1;
  int                trainMode = 0;
  int                weightMode = 1;
  float              eta = 1.0;
  int                batchN = 100;
  float              initMax = 0.01;
  float              logZero = -10.0;

  if ( itfParseArgv( argv[0], &ac, argv+1, 0,
      "-trainMode",  ARGV_INT,   NULL, &trainMode,  NULL, "training modus",
      "-weightMode", ARGV_INT,   NULL, &weightMode, NULL, "weighting modus",
      "-eta",        ARGV_FLOAT, NULL, &eta,        NULL, "learning rate",
      "-batchN",     ARGV_INT,   NULL, &batchN,     NULL, "batch size",
      "-initMax",    ARGV_FLOAT, NULL, &initMax,    NULL, "random init maximum",
      "-logZero",    ARGV_FLOAT, NULL, &logZero,    NULL, "approximation to log(zero)",
  NULL) != TCL_OK) return NULL;

  return (ClientData) glimClassCreate(classN,dimN,trainMode,weightMode,
				      eta,batchN,initMax,logZero);
}



/* ----------------------------------------------------------------------- */
/*  glimClassCopyClf                                                       */
/* ----------------------------------------------------------------------- */
ClientData  glimClassCopyClf (ClientData cd, double perturbe) {

  return (ClientData) glimClassCopy((GlimClass *) cd,perturbe);
}



/* ----------------------------------------------------------------------- */
/*  glimClassInitClf                                                       */
/* ----------------------------------------------------------------------- */
int  glimClassInitClf (ClientData cd) {

  return glimClassReset((GlimClass *) cd);
}



/* ----------------------------------------------------------------------- */
/*  glimClassFreeClf                                                       */
/* ----------------------------------------------------------------------- */
int  glimClassFreeClf (ClientData cd) {

  return glimClassFree((GlimClass *) cd);
}



/* ----------------------------------------------------------------------- */
/*  glimClassSaveClf                                                       */
/* ----------------------------------------------------------------------- */
int  glimClassSaveClf (ClientData cd, FILE *fp) {

  return glimClassSave((GlimClass *) cd,fp);
}



/* ----------------------------------------------------------------------- */
/*  glimClassLoadClf                                                       */
/* ----------------------------------------------------------------------- */
ClientData  glimClassLoadClf (FILE *fp) {

  return (ClientData)glimClassLoad(NULL,fp);
}



/* ----------------------------------------------------------------------- */
/*  glimClassCreateAccusClf                                                */
/* ----------------------------------------------------------------------- */
int  glimClassCreateAccusClf (ClientData cd) {

  return glimClassCreateAccus((GlimClass *) cd);
}



/* ----------------------------------------------------------------------- */
/*  glimClassFreeAccusClf                                                  */
/* ----------------------------------------------------------------------- */
int  glimClassFreeAccusClf (ClientData cd) {

  return glimClassFreeAccus((GlimClass *) cd);
}



/* ----------------------------------------------------------------------- */
/*  glimClassClearAccusClf                                                 */
/* ----------------------------------------------------------------------- */
int  glimClassClearAccusClf (ClientData cd) {

  return glimClassClearAccus((GlimClass *) cd);
}



/* ----------------------------------------------------------------------- */
/*  glimClassSaveAccusClf                                                  */
/* ----------------------------------------------------------------------- */
int  glimClassSaveAccusClf (ClientData cd, FILE *fp) {

  return glimClassSaveAccus((GlimClass *) cd,fp);
}



/* ----------------------------------------------------------------------- */
/*  glimClassLoadAccusClf                                                  */
/* ----------------------------------------------------------------------- */
int  glimClassLoadAccusClf (ClientData cd, FILE *fp) {

  return glimClassLoadAccus((GlimClass *) cd,fp);
}



/* ----------------------------------------------------------------------- */
/*  glimClassConfigureClf                                                  */
/* ----------------------------------------------------------------------- */
int  glimClassConfigureClf (ClientData cd, int argc, char *argv[]) {

  register int      k;
  char              *var,*val;

  if ((argc % 2) != 1) {
    ERROR("invalid number of parameters\n");
    return TCL_ERROR;
  }
  
  for (k=1; k<argc; k+=2) {
    var = argv[k]+1;
    val = argv[k+1];
    if (glimClassConfigureItf(cd,var,val) == TCL_ERROR) return TCL_ERROR;
  }
  
  return TCL_OK;
}



/* ----------------------------------------------------------------------- */
/*  glimClassScoreClf                                                      */
/* ----------------------------------------------------------------------- */
int  glimClassScoreClf (ClientData cd, float *pattern, int dimN, double *score,
		       int classN) {

  GlimClass           *glimClass = (GlimClass *) cd;  
  register int        classX,dimX;
  register double     *w,*sc;
  register float      *p;
  register double     sum,maxSum,val;

  if (classN != glimClass->classN) return TCL_ERROR;
  if (dimN != glimClass->dimN)     return TCL_ERROR;
  
  /* --------------------------- */
  /*  compute linear prediction  */
  /* --------------------------- */
  if (dimN % 16 == 0) {

    /* ------------------------------------------ */
    /*  register optimized matrix-vector product  */
    /* ------------------------------------------ */

    register float   p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15;
    p = pattern;
    for (dimX=0; dimX<dimN; dimX+=16, p+=16) {

      p0  = p[0];  p1  = p[1];  p2  = p[2];  p3  = p[3];
      p4  = p[4];  p5  = p[5];  p6  = p[6];  p7  = p[7];
      p8  = p[8];  p9  = p[9];  p10 = p[10]; p11 = p[11];
      p12 = p[12]; p13 = p[13]; p14 = p[14]; p15 = p[15];

      for (classX=0; classX<classN; classX++) {
        if (dimX==0) {
          w = glimClass->w[classX];
          score[classX] = w[dimN];
        } else
          w = glimClass->w[classX]+dimX;

        score[classX] += p0*w[0]   + p1*w[1]   + p2*w[2]   + p3*w[3]   +
                         p4*w[4]   + p5*w[5]   + p6*w[6]   + p7*w[7]   +
                         p8*w[8]   + p9*w[9]   + p10*w[10] + p11*w[11] +
                         p12*w[12] + p13*w[13] + p14*w[14] + p15*w[15];

      }
    }
    
  } else {
    
    /* ---------------------------------------------------------------- */
    /*  backoff matrix-vector product for features with dimN % 16 != 0  */
    /* ---------------------------------------------------------------- */
    for (classX=0; classX<classN; classX++) {
      w = glimClass->w[classX];
      sum = w[dimN];
      for (dimX=0; dimX<dimN; dimX++)
        sum += w[dimX]*pattern[dimX];
      score[classX] = sum;
    }
    
  }
  
  /* ------------------------------- */
  /*  compute softmax non-linearity  */
  /* ------------------------------- */
  maxSum = -MAXVAL;
  for (classX=0; classX<classN; classX++)
    if (score[classX] > maxSum) maxSum = score[classX];

  sum = 0.0;
  sc = score;
  for (classX=0; classX<classN; sc++, classX++) {
    val = exp(score[classX] - maxSum);
    *sc = val;
    sum += val;
  }

  sc = score;
  val = 1.0/sum;
  for (classX=0; classX<classN; sc++, classX++)
    *sc *= val;
  
  return TCL_OK;
}



/* ----------------------------------------------------------------------- */
/*  glimClassAccuSoftClf                                                   */
/* ----------------------------------------------------------------------- */
int  glimClassAccuSoftClf (ClientData cd, float *pattern, int dimN,
			   double *target, double *act,
			   int classN, double g, double h) {

  GlimClass        *glimClass = (GlimClass *) cd;

  if (glimClass->trainMode == 0) return TCL_OK;

  switch (glimClass->trainMode) {
    case 1:  /* -------------------------- */
             /*  Gradient Ascent learning  */
             /* -------------------------- */
             if (glimClass->weightMode)
               glimClassAccuSoftGA(glimClass,pattern,dimN,target,act,classN,h);
	     else
               glimClassAccuSoftGA(glimClass,pattern,dimN,target,act,classN,g);
	       
	     break;
	     
    case 2:  /* --------------------------------- */
	     /*  Weighted Least Squares learning  */
             /* --------------------------------- */
	     if (glimClass->weightMode)
  	       glimClassAccuSoftLS(glimClass,pattern,dimN,target,classN,h);
	     else
               glimClassAccuSoftLS(glimClass,pattern,dimN,target,classN,g); 
	     break;

    default: ERROR("Unknown trainMode = %d\n",glimClass->trainMode);
	     return TCL_ERROR;
  }
  
  return TCL_OK;
}



/* ----------------------------------------------------------------------- */
/*  glimClassAccuHardClf                                                   */
/* ----------------------------------------------------------------------- */
int  glimClassAccuHardClf (ClientData cd, float *pattern, int dimN,
			   int targetX, double *act,
			   int classN, double g, double h) {

  GlimClass        *glimClass = (GlimClass *) cd;

  if (glimClass->trainMode == 0) return TCL_OK;

  switch (glimClass->trainMode) {
    case 1:  /* -------------------------- */
             /*  Gradient Ascent learning  */
             /* -------------------------- */
             if (glimClass->weightMode)
               glimClassAccuHardGA(glimClass,pattern,dimN,targetX,act,classN,h);
	     else
               glimClassAccuHardGA(glimClass,pattern,dimN,targetX,act,classN,g);
	     break;
	     
    case 2:  /* --------------------------------- */
	     /*  Weighted Least Squares learning  */
             /* --------------------------------- */
	     if (glimClass->weightMode)
  	       glimClassAccuHardLS(glimClass,pattern,dimN,targetX,classN,h);
	     else
  	       glimClassAccuHardLS(glimClass,pattern,dimN,targetX,classN,g);
	     break;

    default: ERROR("Unknown trainMode = %d\n",glimClass->trainMode);
	     return TCL_ERROR;
  }
  
  return TCL_OK;
}



/* ----------------------------------------------------------------------- */
/*  glimClassUpdateClf                                                     */
/* ----------------------------------------------------------------------- */
int  glimClassUpdateClf (ClientData cd) {

  GlimClass        *glimClass = (GlimClass *) cd;

  if (glimClass->trainMode == 0) return TCL_OK;

  switch (glimClass->trainMode) {
    case 1:  /* -------------------------- */
             /*  Gradient Ascent learning  */
             /* -------------------------- */
             if ((glimClass->accu) && (glimClass->accu->trainN > 0))
               glimClassUpdateGA(glimClass);
	     break;
	     
    case 2:  /* --------------------------------- */
	     /*  Weighted Least Squares learning  */
             /* --------------------------------- */
	     glimClassUpdateLS(glimClass);
	     break;

    default: ERROR("Unknown trainMode = %d\n",glimClass->trainMode);
	     return TCL_ERROR;
  }
  
  return TCL_OK;
}




/* ====================================================================== */
/*                        (3) janus3 itf functions                        */
/* ====================================================================== */



/* ----------------------------------------------------------------------- */
/*  glimClassAccuPutsItf                                                   */
/* ----------------------------------------------------------------------- */
int  glimClassAccuPutsItf (ClientData cd, int argc, char *argv[]) {

  GlimClassAccu     *accu = (GlimClassAccu *) cd;
  register int      classX,dimX,k;
  
  itfAppendResult("{ XWX { ");
  for (dimX=0; dimX<=accu->dimN; dimX++) {
    itfAppendResult("{ "); 
    for (k=0; k<=accu->dimN; k++)
      itfAppendResult("%f ",accu->xwx[dimX][k]);
    itfAppendResult("} ");
  }  
  itfAppendResult("} } ");

  itfAppendResult("{ XWV { ");
  for (classX=0; classX<accu->classN; classX++) {
    itfAppendResult("{ "); 
    for (dimX=0; dimX<=accu->dimN; dimX++)
      itfAppendResult("%f ",accu->xwv[classX][dimX]);
    itfAppendResult("} ");
  }  
  itfAppendResult("} } ");
  
  itfAppendResult("{trainN %d} ",accu->trainN);

  return TCL_OK;
}



/* ---------------------------------------------------------------------- */
/*  glimClassCreateItf                                                    */
/* ---------------------------------------------------------------------- */
ClientData  glimClassCreateItf (ClientData cd, int argc, char *argv[]) {

  int                ac;
  int                classN,dimN;

  ac = (argc < 3)?(argc-1):2;
  if ( itfParseArgv( argv[0], &ac, argv+1, 1,
      "<classN>",    ARGV_INT,   NULL, &classN,     NULL, "number of output classes",
      "<dimN>",      ARGV_INT,   NULL, &dimN,       NULL, "feature dimension",
  NULL) != TCL_OK) return NULL;
		    
  return  glimClassCreateClf(classN,dimN,argc-2,argv+2);
}



/* ----------------------------------------------------------------------- */
/*  glimClassFreeItf                                                       */
/* ----------------------------------------------------------------------- */
int  glimClassFreeItf (ClientData cd) {

  return glimClassFree((GlimClass *) cd);
}



/* ----------------------------------------------------------------------- */
/*  glimClassPutsItf                                                       */
/* ----------------------------------------------------------------------- */
int  glimClassPutsItf (ClientData cd, int argc, char *argv[]) {

  GlimClass         *glimClass = (GlimClass *) cd;
  int               ac = argc-1;
  int               classX = -1;
  int               dimX;
  
  if ( itfParseArgv( argv[0], &ac, argv+1, 0,
      "-classX", ARGV_INT, NULL, &classX,   NULL, "weight vector for classX",
  NULL) != TCL_OK) return TCL_ERROR;

  if ((classX < 0) || (classX >= glimClass->classN)) {

    itfAppendResult("{classN %d} {dimN %d} {trainMode %d} {weightMode %d} ",
       glimClass->classN,glimClass->dimN,glimClass->trainMode,glimClass->weightMode);
    itfAppendResult("{eta %f} {batchN %d} ",
		    glimClass->eta,glimClass->batchN);
    itfAppendResult("{initMax %f} {logZero %f} ",
                    glimClass->initMax,glimClass->logZero);

  } else {

    itfAppendResult("{ ");
    for (dimX=0; dimX<=glimClass->dimN; dimX++)
      itfAppendResult("%f ",glimClass->w[classX][dimX]);
    itfAppendResult("}");

  }
  
  return TCL_OK;
}



/* ----------------------------------------------------------------------- */
/*  glimClassWeightsItf                                                    */
/* ----------------------------------------------------------------------- */
int  glimClassWeightsItf (ClientData cd, int argc, char *argv[]) {

  GlimClass         *glimClass = (GlimClass *) cd;
  int               ac = argc-1;
  register int      classX,dimX;
  DMatrix           *matrix;

  if ( itfParseArgv( argv[0], &ac, argv+1, 0,
      "<DMatrix>", ARGV_OBJECT, NULL, &matrix, "DMatrix", "weight matrix",
  NULL) != TCL_OK) return TCL_ERROR;

  if ((matrix->m != glimClass->classN) || (matrix->n != glimClass->dimN+1)) {
    ERROR("invalid matrix dimensions (%d,%d), must be (%d,%d)\n",
	  matrix->m,matrix->n,glimClass->classN,glimClass->dimN+1);
    return TCL_ERROR;
  }
  
  for (classX=0; classX<glimClass->classN; classX++)
    for (dimX=0; dimX<=glimClass->dimN; dimX++)
      glimClass->w[classX][dimX] = matrix->matPA[classX][dimX];
    
  return TCL_OK;
}



/* ----------------------------------------------------------------------- */
/*  glimClassConfigureItf                                                  */
/* ----------------------------------------------------------------------- */
int  glimClassConfigureItf (ClientData cd, char *var, char *val) {

  GlimClass         *glimClass = (GlimClass *) cd;

  if (! var) {
    ITFCFG(glimClassConfigureItf,cd,"classN");
    ITFCFG(glimClassConfigureItf,cd,"dimN");
    ITFCFG(glimClassConfigureItf,cd,"trainMode");
    ITFCFG(glimClassConfigureItf,cd,"weightMode");
    ITFCFG(glimClassConfigureItf,cd,"eta");
    ITFCFG(glimClassConfigureItf,cd,"batchN");
    ITFCFG(glimClassConfigureItf,cd,"initMax");
    ITFCFG(glimClassConfigureItf,cd,"logZero");
    return TCL_OK;
  }
  ITFCFG_Int   (var,val,"classN",     glimClass->classN ,     1);
  ITFCFG_Int   (var,val,"dimN",       glimClass->dimN ,       1);
  ITFCFG_Int   (var,val,"trainMode",  glimClass->trainMode ,  0);
  ITFCFG_Int   (var,val,"weightMode", glimClass->weightMode , 0);
  ITFCFG_Float (var,val,"eta",        glimClass->eta ,        0);
  ITFCFG_Int   (var,val,"batchN",     glimClass->batchN ,     0);
  ITFCFG_Float (var,val,"initMax",    glimClass->initMax ,    0);
  ITFCFG_Float (var,val,"logZero",    glimClass->logZero ,    0);

  return TCL_OK;
}



/* ----------------------------------------------------------------------- */
/*  glimClassAccessItf                                                     */
/* ----------------------------------------------------------------------- */
ClientData  glimClassAccessItf (ClientData cd, char *name, TypeInfo **ti) {

  GlimClass         *glimClass = (GlimClass *) cd;  
  
  if (*name=='.') {
    if (name[1]=='\0') {
      if (glimClass->accu) itfAppendElement("accu");
    }
    else {
      if ((!strcmp(name+1,"accu")) && (glimClass->accu)) {
        *ti=itfGetType("GlimAccu");
        return (ClientData)glimClass->accu;
      }
    }
  }
  *ti=NULL;
  return NULL;
}



/* ----------------------------------------------------------------------- */
/*  glimClassCopyItf                                                       */
/* ----------------------------------------------------------------------- */
int  glimClassCopyItf (ClientData cd, int argc, char *argv[]) {

  GlimClass         *glimClass = (GlimClass *) cd;
  int               ac = argc-1;
  GlimClass         *source,*target;
  float             perturbe = 0.0;
  int               classX;
  
  if ( itfParseArgv( argv[0], &ac, argv+1, 0,
      "<source>",  ARGV_OBJECT, NULL, &source,    "Glim", "source Glim",
      "-perturbe", ARGV_FLOAT,  NULL, &perturbe,  NULL,   "additional random perturberation",
  NULL) != TCL_OK) return TCL_ERROR;  
  
  target = glimClassCopy(source,perturbe);

  /* -------------------- */
  /*  free existing Glim  */
  /* -------------------- */
  if (glimClass->accu) glimClassFreeAccus(glimClass);
  for (classX=0; classX<glimClass->classN; classX++)
    free(glimClass->w[classX]);
  free(glimClass->w);

  /* ------------------------------ */
  /*  copy new Glim into glimClass  */
  /* ------------------------------ */
  glimClass->classN     = target->classN;
  glimClass->dimN       = target->dimN;
  glimClass->trainMode  = target->trainMode;
  glimClass->weightMode = target->weightMode;
  glimClass->eta        = target->eta;
  glimClass->batchN     = target->batchN;
  glimClass->initMax    = target->initMax;
  glimClass->logZero    = target->logZero;
  glimClass->w          = target->w;
  glimClass->accu       = NULL;

  free(target);
  return TCL_OK;
}



/* ----------------------------------------------------------------------- */
/*  glimClassResetItf                                                      */
/* ----------------------------------------------------------------------- */
int  glimClassResetItf (ClientData cd, int argc, char *argv[]) {

  return  glimClassReset((GlimClass *) cd);
}



/* ----------------------------------------------------------------------- */
/*  glimClassSaveItf                                                       */
/* ----------------------------------------------------------------------- */
int  glimClassSaveItf (ClientData cd, int argc, char *argv[]) {

  GlimClass             *glimClass = (GlimClass *) cd;
  int                   ac         = argc-1;
  char                  *filename;
  FILE                  *fp;

  if ( itfParseArgv( argv[0], &ac, argv+1, 0,
      "<filename>", ARGV_STRING, NULL, &filename, NULL, "name of file",
  NULL) != TCL_OK) return TCL_ERROR;

  if ((fp = fileOpen(filename,"w")) == NULL) {
    ERROR("couldn't open file '%s'\n",filename);
    return TCL_ERROR;
  }
  
  if (glimClassSave(glimClass,fp) == TCL_ERROR) return TCL_ERROR;
  fileClose(filename,fp);
  
  return TCL_OK;
}



/* ----------------------------------------------------------------------- */
/*  glimClassLoadItf                                                       */
/* ----------------------------------------------------------------------- */
int  glimClassLoadItf (ClientData cd, int argc, char *argv[]) {

  GlimClass             *glimClass = (GlimClass *) cd;
  int                   ac         = argc-1;
  char                  *filename;
  FILE                  *fp;

  if ( itfParseArgv( argv[0], &ac, argv+1, 0,
      "<filename>", ARGV_STRING, NULL, &filename, NULL, "name of file",
  NULL) != TCL_OK) return TCL_ERROR;

  if ((fp = fileOpen(filename,"r")) == NULL) {
    ERROR("couldn't open file '%s'\n",filename);
    return TCL_ERROR;
  }
  
  if (glimClassLoad(glimClass,fp) == NULL) return TCL_ERROR;
  fileClose(filename,fp);

  return TCL_OK;
}



/* ----------------------------------------------------------------------- */
/*  glimClassCreateAccusItf                                                */
/* ----------------------------------------------------------------------- */
int  glimClassCreateAccusItf (ClientData cd, int argc, char *argv[]) {

  return  glimClassCreateAccus((GlimClass *) cd);
}



/* ----------------------------------------------------------------------- */
/*  glimClassFreeAccusItf                                                  */
/* ----------------------------------------------------------------------- */
int  glimClassFreeAccusItf (ClientData cd, int argc, char *argv[]) {

  return  glimClassFreeAccus((GlimClass *) cd);
}



/* ----------------------------------------------------------------------- */
/*  glimClassClearAccusItf                                                 */
/* ----------------------------------------------------------------------- */
int  glimClassClearAccusItf (ClientData cd, int argc, char *argv[]) {

  return  glimClassClearAccus((GlimClass *) cd);
}



/* ----------------------------------------------------------------------- */
/*  glimClassSaveAccusItf                                                  */
/* ----------------------------------------------------------------------- */
int  glimClassSaveAccusItf (ClientData cd, int argc, char *argv[]) {

  GlimClass             *glimClass = (GlimClass *) cd;
  int                   ac         = argc-1;
  char                  *filename;
  FILE                  *fp;

  if ( itfParseArgv( argv[0], &ac, argv+1, 0,
      "<filename>", ARGV_STRING, NULL, &filename, NULL, "name of file",
  NULL) != TCL_OK) return TCL_ERROR;

  if ((fp = fileOpen(filename,"w")) == NULL) {
    ERROR("couldn't open file '%s'\n",filename);
    return TCL_ERROR;
  }
  
  if (glimClassSaveAccus(glimClass,fp) == TCL_ERROR) return TCL_ERROR;
  fileClose(filename,fp);
  
  return TCL_OK;
}



/* ----------------------------------------------------------------------- */
/*  glimClassLoadAccusItf                                                  */
/* ----------------------------------------------------------------------- */
int  glimClassLoadAccusItf (ClientData cd, int argc, char *argv[]) {

  GlimClass             *glimClass = (GlimClass *) cd;
  int                   ac         = argc-1;
  char                  *filename;
  FILE                  *fp;

  if ( itfParseArgv( argv[0], &ac, argv+1, 0,
      "<filename>", ARGV_STRING, NULL, &filename, NULL, "name of file",
  NULL) != TCL_OK) return TCL_ERROR;

  if ((fp = fileOpen(filename,"r")) == NULL) {
    ERROR("couldn't open file '%s'\n",filename);
    return TCL_ERROR;
  }
  
  if (glimClassLoadAccus(glimClass,fp) == TCL_ERROR) return TCL_ERROR;
  fileClose(filename,fp);

  return TCL_OK;
}



/* ----------------------------------------------------------------------- */
/*  glimClassScoreItf                                                      */
/* ----------------------------------------------------------------------- */
int  glimClassScoreItf (ClientData cd, int argc, char *argv[]) {

  GlimClass      *glimClass  = (GlimClass *) cd;
  int            ac          = argc-1;
  int            frameX      = 0;
  FeatureSet     *fes        = NULL;
  int            featIdx     = 0;
  float          *pattern    = NULL;
  double         *score;
  int            classX;

  if ( itfParseArgv (argv[0], &ac, argv+1, 0,
      "<fes>",         ARGV_OBJECT, NULL, &fes,         "FeatureSet", "FeatureSet",
      "<featIdx>",     ARGV_INT,    NULL, &featIdx,     NULL,         "index of feature",
      "<frameX>",      ARGV_INT,    NULL, &frameX,      NULL,         "frame index",
    NULL) != TCL_OK) return TCL_ERROR;

  pattern = fesFrame(fes,featIdx,frameX);

  if (pattern == NULL) {
    ERROR("couldn't get feature vector for frameX=%d from FeatureSet",frameX);
    return TCL_ERROR;
  }

  score = (double *) malloc (sizeof(double) * glimClass->classN);
  glimClassScoreClf(cd,pattern,glimClass->dimN,score,glimClass->classN);
  for (classX=0; classX<glimClass->classN; classX++)
    itfAppendResult("%f ",score[classX]);
  free(score);
  
  return TCL_OK;
}



/* ----------------------------------------------------------------------- */
/*  glimClassAccuHardItf                                                   */
/* ----------------------------------------------------------------------- */
int  glimClassAccuHardItf (ClientData cd, int argc, char *argv[]) {

  GlimClass      *glimClass  = (GlimClass *) cd;
  int            ac          = argc-1;
  int            frameX      = 0;
  FeatureSet     *fes        = NULL;
  int            featIdx     = 0;
  float          *pattern    = NULL;
  int            targetX     = -1;
  float          weight      = 1.0;
  double         *score      = NULL;
  

  if ( itfParseArgv (argv[0], &ac, argv+1, 0,
      "<fes>",         ARGV_OBJECT, NULL, &fes,         "FeatureSet", "FeatureSet",
      "<featIdx>",     ARGV_INT,    NULL, &featIdx,     NULL,         "index of feature",
      "<frameX>",      ARGV_INT,    NULL, &frameX,      NULL,         "frame index",
      "<classX>",      ARGV_INT,    NULL, &targetX,     NULL,         "target class",
      "-weight",       ARGV_FLOAT,  NULL, &weight,      NULL,         "observation weight",
  NULL) != TCL_OK) return TCL_ERROR;

  pattern = fesFrame(fes,featIdx,frameX);

  if (pattern == NULL) {
    ERROR("couldn't get feature vector for frameX=%d from FeatureSet",frameX);
    return TCL_ERROR;
  }

  /* --------------------------------------------------------------- */
  /*  if we train GA we need to provide scores for the actual frame  */
  /* --------------------------------------------------------------- */
  if (glimClass->trainMode == 1) {
    score = (double *) malloc (sizeof(double) * glimClass->classN);
    glimClassScoreClf(cd,pattern,glimClass->dimN,score,glimClass->classN);
  }

  glimClassAccuHardClf(cd,pattern,glimClass->dimN,targetX,score,
		       glimClass->classN,weight,weight);

  if (score) free(score);
  return TCL_OK;
}



/* ----------------------------------------------------------------------- */
/*  glimClassUpdateItf                                                     */
/* ----------------------------------------------------------------------- */
int  glimClassUpdateItf (ClientData cd, int argc, char *argv[]) {

  return  glimClassUpdateClf(cd);
}




/* ====================================================================== */
/*                  (4) type declarations + initialization                */
/* ====================================================================== */


static Method glimClassAccuMethod[] = 
{ 
  { "puts",           glimClassAccuPutsItf,         NULL },
  {  NULL,  NULL, NULL } 
};



TypeInfo glimClassAccuInfo = { 
        "GlimAccu", 0, -1, glimClassAccuMethod, 
         NULL, NULL,
         NULL, NULL, itfTypeCntlDefaultNoLink,
        "Training accumulators for a Glim\n" } ;



static Method glimClassMethod[] = 
{ 
  { "puts",           glimClassPutsItf,         NULL },
  { "setWeights",     glimClassWeightsItf,      "set Glim weights from matrix" },
  { "copy",           glimClassCopyItf,         "copy Glim" },
  { "reset",          glimClassResetItf,        "reset Glim" },
  { "save",           glimClassSaveItf,         "save parameters" },
  { "load",           glimClassLoadItf,         "load parameters" },
  { "createAccus",    glimClassCreateAccusItf,  "create accus for training" },
  { "freeAccus",      glimClassFreeAccusItf,    "free accus for training" },
  { "clearAccus",     glimClassClearAccusItf,   "clear accus" },
  { "saveAccus",      glimClassSaveAccusItf,    "save accus" },
  { "loadAccus",      glimClassLoadAccusItf,    "load and accu accus" },
  { "score",          glimClassScoreItf,        "score a given frame" },
  { "accu",           glimClassAccuHardItf,     "accu a given frame" },
  { "update",         glimClassUpdateItf,       "update parameters" },
  {  NULL,  NULL, NULL } 
};



static Classifier classifierInfo = {
        "Glim",
	 glimClassCreateClf,
	 glimClassCopyClf,
	 glimClassInitClf,
	 glimClassFreeClf,
	 glimClassSaveClf,
	 glimClassLoadClf,
	 glimClassCreateAccusClf,
	 glimClassFreeAccusClf,
	 glimClassClearAccusClf,
	 glimClassSaveAccusClf,
	 glimClassLoadAccusClf,
	 glimClassConfigureClf,
	 glimClassScoreClf,
	 glimClassAccuSoftClf,
 	 glimClassAccuHardClf,
 	 glimClassUpdateClf };



TypeInfo glimClassInfo = { 
        "Glim", 0, -1, glimClassMethod, 
         glimClassCreateItf, glimClassFreeItf,
         glimClassConfigureItf, glimClassAccessItf, itfTypeCntlDefaultNoLink,
        "A multinomial generalized linear model classifier\n" } ;




static int glimInitialized = 0;

int Glim_Init ( )
{
  if ( glimInitialized) return TCL_OK;

  myRandomInit(0.123456789);      /* any seed for initializing the RNG */

  itfNewType    ( &glimClassAccuInfo );
  itfNewType    ( &glimClassInfo );
  classNewType  ( &classifierInfo );
  
  glimInitialized = 1;
  return TCL_OK;
}





