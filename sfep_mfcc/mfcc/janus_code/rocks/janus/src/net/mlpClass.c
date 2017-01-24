/* ========================================================================
    JANUS      Speech- to Speech Translation
               ------------------------------------------------------------
               Object: Multi Layer Perceptron Classifier
               ------------------------------------------------------------

    Author  :  Juergen Fritsch
    Module  :  mlpClass.c
    Date    :  $Id: mlpClass.c 2390 2003-08-14 11:20:32Z fuegen $
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
    Revision 3.4  2003/08/14 11:20:21  fuegen
    Merged changes on branch jtk-01-01-15-fms (jaguar -> ibis-013)

    Revision 3.3.4.2  2002/06/26 12:26:50  fuegen
    go away, boring message

    Revision 3.3.4.1  2002/06/26 11:57:57  fuegen
    changes for new directory structure support and code cleaning

    Revision 3.3  2001/01/15 09:49:59  janus
    Merged Christian's version with WINDOWS support (branch jtk-00-11-14-fuegen)

    Revision 3.2.40.1  2001/01/12 15:16:59  janus
    necessary changes for running janus under WINDOWS

    Revision 3.2  1997/06/17 17:41:43  westphal
    *** empty log message ***

 * Revision 1.1  96/09/27  09:00:33  fritsch
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
#include "mlpClass.h"



/* ======================================================================= */
/*                       (1)  local implementation                         */
/* ======================================================================= */


#define        MLP_MAGIC          5050
#define        MLP_ACCU_MAGIC     6060
#define        square(X)          ((X)*(X))
#define        sigmoid(X,A)       ((A)+((1.0-(2.0*(A)))/(1.0+exp(-(X)))))

int  mlpClassFreeAccus (MlpClass *mlp);
int  mlpClassClearAccus (MlpClass *mlp);
int  mlpClassUpdate (MlpClass *mlp);

/* ----------------------------------------------------------------------- */
/*  mlpClassReset                                                          */
/* ----------------------------------------------------------------------- */
int  mlpClassReset (MlpClass *mlp) {

  register int         hidX;
  
  matInitRandom( mlp->w , mlp->hiddenN , mlp->dimN+1    , mlp->initMax );
  matInitRandom( mlp->v , mlp->classN  , mlp->hiddenN+1 , mlp->initMax );

  for (hidX=0; hidX<mlp->hiddenN; hidX++) mlp->act[hidX] = 0.0;
  
  return TCL_OK;
}



/* ----------------------------------------------------------------------- */
/*  mlpClassInit                                                           */
/* ----------------------------------------------------------------------- */
int  mlpClassInit (MlpClass *mlp, int classN, int dimN, int hiddenN,
		   int trainMode, int weightMode, double eta,
		   int batchN, double initMax, double nu, double flatSpot) {

  register int           hidX,classX;
  
  mlp->classN     = classN;
  mlp->dimN       = dimN;
  mlp->hiddenN    = hiddenN;
  mlp->accu       = NULL;
  mlp->trainMode  = trainMode;
  mlp->weightMode = weightMode;
  mlp->batchN     = batchN;
  mlp->initMax    = initMax;
  mlp->eta        = eta;
  mlp->nu         = nu;
  mlp->flatSpot   = flatSpot;

  mlp->w = (double **) malloc (sizeof(double *) * hiddenN);
  for (hidX=0; hidX<hiddenN; hidX++)
    mlp->w[hidX] = (double *) malloc (sizeof(double) * (dimN+1));

  mlp->act = (double *) malloc (sizeof(double) * hiddenN);

  mlp->v = (double **) malloc (sizeof(double *) * classN);
  for (classX=0; classX<classN; classX++)
    mlp->v[classX] = (double *) malloc (sizeof(double) * (hiddenN+1));
  
  return mlpClassReset(mlp);
}



/* ----------------------------------------------------------------------- */
/*  mlpClassCreate                                                         */
/* ----------------------------------------------------------------------- */
MlpClass  *mlpClassCreate (int classN, int dimN, int hiddenN, int trainMode,
			   int weightMode, double eta,
			   int batchN, double initMax, double nu,
			   double flatSpot) {

  MlpClass      *mlp = (MlpClass *) malloc (sizeof(MlpClass));

  if (mlpClassInit (mlp,classN,dimN,hiddenN,trainMode,weightMode,eta,
	        batchN,initMax,nu,flatSpot) == TCL_OK)
    return mlp;
  else
    return NULL;
}



/* ----------------------------------------------------------------------- */
/*  mlpClassDeinit                                                         */
/* ----------------------------------------------------------------------- */
int  mlpClassDeinit (MlpClass *mlp) {

  register int          hidX,classX;
  
  for (hidX=0; hidX<mlp->hiddenN; hidX++)
    free(mlp->w[hidX]);
  free(mlp->w);

  free(mlp->act);
  
  for (classX=0; classX<mlp->classN; classX++)
    free(mlp->v[classX]);
  free(mlp->v);

  return mlpClassFreeAccus(mlp);
}



/* ----------------------------------------------------------------------- */
/*  mlpClassFree                                                           */
/* ----------------------------------------------------------------------- */
int  mlpClassFree (MlpClass *mlp) {

  mlpClassDeinit(mlp);
  free(mlp);
  
  return TCL_OK;
}



/* ----------------------------------------------------------------------- */
/*  mlpClassCopy                                                           */
/* ----------------------------------------------------------------------- */
MlpClass  *mlpClassCopy (MlpClass *src, double perturbe) {

  MlpClass       *target;
  int            classX,hidX,dimX;
  
  target = mlpClassCreate(src->classN,src->dimN,src->hiddenN,src->trainMode,
			  src->weightMode,src->eta,src->batchN,
			  src->initMax,src->nu,src->flatSpot);

  for (classX=0; classX<src->classN; classX++)
    for (hidX=0; hidX<=src->hiddenN; hidX++)
      target->v[classX][hidX] = src->v[classX][hidX] +
	                        myRandom(-perturbe,perturbe);

  for (hidX=0; hidX<src->hiddenN; hidX++)
    target->act[hidX] = 0.0;
  
  for (hidX=0; hidX<src->hiddenN; hidX++)
    for (dimX=0; dimX<=src->dimN; dimX++)
      target->w[hidX][dimX] = src->w[hidX][dimX] +
	                      myRandom(-perturbe,perturbe);
  
  return target;
}



/* ----------------------------------------------------------------------- */
/*  mlpClassSave                                                           */
/* ----------------------------------------------------------------------- */
int  mlpClassSave (MlpClass *mlp, FILE *fp) {

  register int         classX,hidX,dimX;
  
  write_int(fp,MLP_MAGIC);
  write_int(fp,mlp->classN);
  write_int(fp,mlp->dimN);
  write_int(fp,mlp->hiddenN);
  write_int(fp,mlp->trainMode);
  write_int(fp,mlp->weightMode);
  write_int(fp,mlp->batchN);
  write_float(fp,(float) mlp->initMax);
  write_float(fp,(float) mlp->eta);
  write_float(fp,(float) mlp->nu);
  write_float(fp,(float) mlp->flatSpot);

  for (classX=0; classX<mlp->classN; classX++)
    for (hidX=0; hidX<=mlp->hiddenN; hidX++)
      write_float(fp,(float) mlp->v[classX][hidX]);

  for (hidX=0; hidX<mlp->hiddenN; hidX++)
    for (dimX=0; dimX<=mlp->dimN; dimX++)
      write_float(fp,(float) mlp->w[hidX][dimX]);

  return TCL_OK;
}



/* ----------------------------------------------------------------------- */
/*  mlpClassLoad                                                           */
/* ----------------------------------------------------------------------- */
MlpClass  *mlpClassLoad (MlpClass *mlp, FILE *fp) {

  MlpClass         *newMlp;
  register int     classX,hidX,dimX;
  
  if (read_int(fp) != MLP_MAGIC) {
    ERROR("couldn't read Mlp magic\n");
    return NULL;
  }
  
  if (mlp) {
    mlpClassDeinit(mlp);
    newMlp = mlp;
  } else
    newMlp = (MlpClass *) malloc (sizeof(MlpClass));

  newMlp->classN     = read_int(fp);
  newMlp->dimN       = read_int(fp);
  newMlp->hiddenN    = read_int(fp);
  newMlp->trainMode  = read_int(fp);
  newMlp->weightMode = read_int(fp);
  newMlp->batchN     = read_int(fp);
  newMlp->initMax    = (double) read_float(fp);
  newMlp->eta        = (double) read_float(fp);
  newMlp->nu         = (double) read_float(fp);
  newMlp->flatSpot   = (double) read_float(fp);

  newMlp->v = (double **) malloc (sizeof(double *)*newMlp->classN);
  for (classX=0; classX<newMlp->classN; classX++) {
    newMlp->v[classX] = (double *) malloc (sizeof(double)*(newMlp->hiddenN+1));
    for (hidX=0; hidX<=newMlp->hiddenN; hidX++)
      newMlp->v[classX][hidX] = (double) read_float(fp); 
  }

  newMlp->act = (double *) malloc (sizeof(double) * newMlp->hiddenN);
  for (hidX=0; hidX<newMlp->hiddenN; hidX++)
    newMlp->act[hidX] = 0.0;
  
  newMlp->w = (double **) malloc (sizeof(double *)*newMlp->hiddenN);
  for (hidX=0; hidX<newMlp->hiddenN; hidX++) {
    newMlp->w[hidX] = (double *) malloc (sizeof(double)*(newMlp->dimN+1));
    for (dimX=0; dimX<=newMlp->dimN; dimX++)
      newMlp->w[hidX][dimX] = (double) read_float(fp);
  }

  newMlp->accu = NULL;
  
  return newMlp;
}



/* ----------------------------------------------------------------------- */
/*  mlpClassCreateAccus                                                    */
/* ----------------------------------------------------------------------- */
int  mlpClassCreateAccus (MlpClass *mlp) {

  MlpClassAccu          *accu;
  register int          classX,hidX;
  
  if (mlp->accu) return TCL_OK;

  mlp->accu = (MlpClassAccu *) malloc (sizeof(MlpClassAccu));
  accu = mlp->accu;
  accu->classN  = mlp->classN;
  accu->dimN    = mlp->dimN;
  accu->hiddenN = mlp->hiddenN;

  accu->pv = (double *)  malloc (sizeof(double)   * accu->classN);
  accu->av = (double **) malloc (sizeof(double *) * accu->classN);
  accu->dv = (double **) malloc (sizeof(double *) * accu->classN);
  for (classX=0; classX<accu->classN; classX++) {
    accu->av[classX] = (double *) malloc (sizeof(double) * (accu->hiddenN+1));
    accu->dv[classX] = (double *) malloc (sizeof(double) * (accu->hiddenN+1));
  }

  accu->pw = (double *)  malloc (sizeof(double)   * accu->hiddenN);
  accu->aw = (double **) malloc (sizeof(double *) * accu->hiddenN);
  accu->dw = (double **) malloc (sizeof(double *) * accu->hiddenN);
  for (hidX=0; hidX<accu->hiddenN; hidX++) {
    accu->aw[hidX] = (double *) malloc (sizeof(double) * (accu->dimN+1));
    accu->dw[hidX] = (double *) malloc (sizeof(double) * (accu->dimN+1));
  }
  
  return  mlpClassClearAccus(mlp);
}



/* ----------------------------------------------------------------------- */
/*  mlpClassFreeAccus                                                      */
/* ----------------------------------------------------------------------- */
int  mlpClassFreeAccus (MlpClass *mlp) {

  MlpClassAccu      *accu;
  register int      classX,hidX;
  
  if (!mlp->accu) return TCL_OK;
  accu = mlp->accu;
  
  for (classX=0; classX<accu->classN; classX++) {
    free(accu->av[classX]);
    free(accu->dv[classX]);
  }
  free(accu->av);
  free(accu->dv);
  free(accu->pv);

  for (hidX=0; hidX<accu->hiddenN; hidX++) {
    free(accu->aw[hidX]);
    free(accu->dw[hidX]);
  }
  free(accu->aw);
  free(accu->dw);
  free(accu->pw);
  
  mlp->accu = NULL;
  return TCL_OK;
}



/* ----------------------------------------------------------------------- */
/*  mlpClassClearAccus                                                     */
/* ----------------------------------------------------------------------- */
int  mlpClassClearAccus (MlpClass *mlp) {

  MlpClassAccu         *accu;
  register int         classX,hidX,dimX;
  
  if (!mlp->accu) {
    ERROR("no accus allocated\n");
    return TCL_ERROR;
  }

  accu = mlp->accu;
  
  for (classX=0; classX<accu->classN; classX++) {
    for (hidX=0; hidX<=accu->hiddenN; hidX++) {
      accu->av[classX][hidX] = 0.0;
      accu->dv[classX][hidX] = 0.0;
    }
    accu->pv[classX] = 0.0;
  }

  for (hidX=0; hidX<accu->hiddenN; hidX++) {
    for (dimX=0; dimX<=accu->dimN; dimX++) {
      accu->aw[hidX][dimX] = 0.0;
      accu->dw[hidX][dimX] = 0.0;
    }
    accu->pw[hidX] = 0.0;
  }

  accu->trainN = 0;
  
  return TCL_OK;
}



/* ----------------------------------------------------------------------- */
/*  mlpClassClearAccusOnline                                               */
/* ----------------------------------------------------------------------- */
int  mlpClassClearAccusOnline (MlpClass *mlp) {

  MlpClassAccu         *accu;
  register int         classX,hidX,dimX;
  
  if (!mlp->accu) {
    ERROR("no accus allocated\n");
    return TCL_ERROR;
  }

  accu = mlp->accu;
  
  for (classX=0; classX<accu->classN; classX++)
    for (hidX=0; hidX<=accu->hiddenN; hidX++)
      accu->av[classX][hidX] = 0.0;

  for (hidX=0; hidX<accu->hiddenN; hidX++)
    for (dimX=0; dimX<=accu->dimN; dimX++)
      accu->aw[hidX][dimX] = 0.0;

  accu->trainN = 0;
  
  return TCL_OK;
}



/* ----------------------------------------------------------------------- */
/*  mlpClassSaveAccus                                                     */
/* ----------------------------------------------------------------------- */
int  mlpClassSaveAccus (MlpClass *mlp, FILE *fp) {
 
  MlpClassAccu             *accu;
  register int             classX,hidX,dimX;

  if (!mlp->accu) {
    ERROR("no accus allocated\n");
    return TCL_ERROR;
  }
  accu = mlp->accu;

  write_int(fp,MLP_ACCU_MAGIC);
  write_int(fp,accu->classN);
  write_int(fp,accu->dimN);
  write_int(fp,accu->hiddenN);

  for (classX=0; classX<accu->classN; classX++) {
    for (hidX=0; hidX<=accu->hiddenN; hidX++) {
      write_float(fp,(float) accu->av[classX][hidX]);
      write_float(fp,(float) accu->dv[classX][hidX]);
    }
  }

  for (hidX=0; hidX<accu->hiddenN; hidX++) {
    for (dimX=0; dimX<=accu->dimN; dimX++) {
      write_float(fp,(float) accu->aw[hidX][dimX]);
      write_float(fp,(float) accu->dw[hidX][dimX]);
    }
  }

  write_int(fp,accu->trainN);
  
  return TCL_OK;
}



/* ----------------------------------------------------------------------- */
/*  mlpClassLoadAccus                                                     */
/* ----------------------------------------------------------------------- */
int  mlpClassLoadAccus (MlpClass *mlp, FILE *fp) {

  MlpClassAccu             *accu;
  register int             classX,hidX,dimX;
  
  if (read_int(fp) != MLP_ACCU_MAGIC) {
    ERROR("couldn't read Mlp accu magic\n");
    return TCL_ERROR;
  }
  
  if (!mlp->accu) mlpClassCreateAccus(mlp);
  accu = mlp->accu;

  if (read_int(fp) != accu->classN) {
    ERROR("invalid number of output classes in accu file\n");
    return TCL_ERROR;
  }

  if (read_int(fp) != accu->dimN) {
    ERROR("invalid input dimension in accu file\n");
    return TCL_ERROR;
  }

  if (read_int(fp) != accu->hiddenN) {
    ERROR("invalid number of hidden nodes in accu file\n");
    return TCL_ERROR;
  }

  for (classX=0; classX<accu->classN; classX++) {
    for (hidX=0; hidX<=accu->hiddenN; hidX++) {
      accu->av[classX][hidX] += (double) read_float(fp);
      accu->dv[classX][hidX] += (double) read_float(fp);
    }
  }

  for (hidX=0; hidX<accu->hiddenN; hidX++) {
    for (dimX=0; dimX<=accu->dimN; dimX++) {
      accu->aw[hidX][dimX] += (double) read_float(fp);
      accu->dw[hidX][dimX] += (double) read_float(fp);
    }
  }

  accu->trainN += read_int(fp);
  
  return TCL_OK;
}



/* ------------------------------------------------------------------------ */
/*  matFloatVecOne       Matrix x Vector (taking care of implicit bias 1)  */
/* ------------------------------------------------------------------------ */
int  matFloatVecOne (double **mat, float *vec, double *out, int m, int n) {

  register int     classX,dimX;


  /* --------------------------- */
  /*  compute linear prediction  */
  /* --------------------------- */
  if (n % 16 == 0) {

    /* ------------------------------------------ */
    /*  register optimized matrix-vector product  */
    /* ------------------------------------------ */

    register double  *w;
    register float   *p;
    register float   p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15;

    p = vec;
    for (dimX=0; dimX<n; dimX+=16, p+=16) {

      p0  = p[0];  p1  = p[1];  p2  = p[2];  p3  = p[3];
      p4  = p[4];  p5  = p[5];  p6  = p[6];  p7  = p[7];
      p8  = p[8];  p9  = p[9];  p10 = p[10]; p11 = p[11];
      p12 = p[12]; p13 = p[13]; p14 = p[14]; p15 = p[15];

      for (classX=0; classX<m; classX++) {
        if (dimX==0) {
          w = mat[classX];
          out[classX] = w[n];
        } else
          w = mat[classX]+dimX;

        out[classX] += p0*w[0]   + p1*w[1]   + p2*w[2]   + p3*w[3]   +
                       p4*w[4]   + p5*w[5]   + p6*w[6]   + p7*w[7]   +
                       p8*w[8]   + p9*w[9]   + p10*w[10] + p11*w[11] +
                       p12*w[12] + p13*w[13] + p14*w[14] + p15*w[15];
	
      }
    }
    
  } else {
    
    /* ---------------------------------------------------------------- */
    /*  backoff matrix-vector product for features with dimN % 16 != 0  */
    /* ---------------------------------------------------------------- */
    for (classX=0; classX<m; classX++) {

      register double        sum;
      double                 *w;
      
      w = mat[classX];
      sum = w[n];
      for (dimX=0; dimX<n; dimX++)
        sum += w[dimX]*vec[dimX];
      out[classX] = sum;
    }

  }

  return TCL_OK;
}



/* ------------------------------------------------------------------------ */
/*  matDoubleVecOne       Matrix x Vector (taking care of implicit bias 1)  */
/* ------------------------------------------------------------------------ */
int  matDoubleVecOne (double **mat, double *vec, double *out, int m, int n) {

  register int     classX,dimX;
  
  /* --------------------------- */
  /*  compute linear prediction  */
  /* --------------------------- */
  if (n % 16 == 0) {

    /* ------------------------------------------ */
    /*  register optimized matrix-vector product  */
    /* ------------------------------------------ */

    register double  *w,*p;
    register double  p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15;

    p = vec;
    for (dimX=0; dimX<n; dimX+=16, p+=16) {

      p0  = p[0];  p1  = p[1];  p2  = p[2];  p3  = p[3];
      p4  = p[4];  p5  = p[5];  p6  = p[6];  p7  = p[7];
      p8  = p[8];  p9  = p[9];  p10 = p[10]; p11 = p[11];
      p12 = p[12]; p13 = p[13]; p14 = p[14]; p15 = p[15];

      for (classX=0; classX<m; classX++) {
        if (dimX==0) {
          w = mat[classX];
          out[classX] = w[n];
        } else
          w = mat[classX]+dimX;

        out[classX] += p0*w[0]   + p1*w[1]   + p2*w[2]   + p3*w[3]   +
                       p4*w[4]   + p5*w[5]   + p6*w[6]   + p7*w[7]   +
                       p8*w[8]   + p9*w[9]   + p10*w[10] + p11*w[11] +
                       p12*w[12] + p13*w[13] + p14*w[14] + p15*w[15];
	
      }
    }
    
  } else {
    
    /* ---------------------------------------------------------------- */
    /*  backoff matrix-vector product for features with dimN % 16 != 0  */
    /* ---------------------------------------------------------------- */
    for (classX=0; classX<m; classX++) {

      register double        sum;
      double                 *w;
      
      w = mat[classX];
      sum = w[n];
      for (dimX=0; dimX<n; dimX++)
        sum += w[dimX]*vec[dimX];
      out[classX] = sum;
    }

  }

  return TCL_OK;
}



/* ----------------------------------------------------------------------- */
/*  vecSigmoid                                                             */
/* ----------------------------------------------------------------------- */
int  vecSigmoid (double *vec, int vecN, double flatSpot) {

  register int    vecX;

  for (vecX=0; vecX<vecN; vecX++)
    vec[vecX] = sigmoid(vec[vecX],flatSpot);
  
  return TCL_OK;
}



/* ----------------------------------------------------------------------- */
/*  vecSoftmax                                                             */
/* ----------------------------------------------------------------------- */
int  vecSoftmax (double *vec, int vecN) {

  register int          vecX;
  register double       val,maxVal,sum,factor;

  maxVal = -1E20;
  for (vecX=0; vecX<vecN; vecX++)
    if (vec[vecX] > maxVal) maxVal = vec[vecX];

  sum = 0.0;
  for (vecX=0; vecX<vecN; vecX++) {
    val = exp(vec[vecX] - maxVal);
    vec[vecX] = val;
    sum += val;
  }
  factor = 1.0/sum;
  
  for (vecX=0; vecX<vecN; vecX++)
    vec[vecX] *= factor;  
    
  return TCL_OK;
}



/* ----------------------------------------------------------------------- */
/*  mlpClassAccuSoft                                                       */
/* ----------------------------------------------------------------------- */
int  mlpClassAccuSoft (MlpClass *mlp, float *pattern, int dimN,
		       double *target, double *act, int classN, double w) {

  MlpClassAccu        *accu;
  register int        k,i,hiddenN;
  register double     sum,actk,delta;
  
  accu = mlp->accu;
  assert(mlp->dimN == dimN);
  assert(mlp->classN == classN);
  hiddenN = mlp->hiddenN;
  
  /* -------------------------------------- */
  /*  compute delta's for the output layer  */
  /* -------------------------------------- */
  for (k=0; k<classN; k++) {
    sum = 0.0;
    actk = -act[k];
    for (i=0; i<classN; i++) {
      if (i==k)
        sum += (target[i] * (1.0 + actk));
      else
	sum += (target[i] * actk);
    }
    accu->pv[k] = w*sum;
  }

  /* ----------------------------------------------------------------- */
  /*  compute and accu updates for the hidden to output layer weights  */
  /* ----------------------------------------------------------------- */
  for (k=0; k<classN; k++) {
    delta = accu->pv[k];
    for (i=0; i<hiddenN; i++)
      accu->av[k][i] += (delta * mlp->act[i]);
    accu->av[k][hiddenN] += delta;
  }

  /* -------------------------------------- */
  /*  compute delta's for the hidden layer  */
  /* -------------------------------------- */
  for (k=0; k<hiddenN; k++)
    accu->pw[k] = 0.0;
  
  for (k=0; k<classN; k++) {
    delta = accu->pv[k];
    for (i=0; i<hiddenN; i++)
      accu->pw[i] += (mlp->v[k][i] * delta);
  }

  for (k=0; k<hiddenN; k++)
    accu->pw[k] *= (mlp->act[k] * (1.0 - mlp->act[k]));

  /* ---------------------------------------------------------------- */
  /*  compute and accu updates for the input to hidden layer weights  */
  /* ---------------------------------------------------------------- */
  for (k=0; k<hiddenN; k++) {
    delta = accu->pw[k];
    for (i=0; i<dimN; i++)
      accu->aw[k][i] += (delta * pattern[i]);
    accu->aw[k][dimN] += delta;
  }

  /* --------------------- */
  /*  check for an update  */
  /* --------------------- */
  accu->trainN++;
  if (accu->trainN == mlp->batchN) {
    mlpClassUpdate(mlp);
    mlpClassClearAccusOnline(mlp);
  }  

  return TCL_OK;
}



/* ----------------------------------------------------------------------- */
/*  mlpClassAccuHard                                                       */
/* ----------------------------------------------------------------------- */
int  mlpClassAccuHard (MlpClass *mlp, float *pattern, int dimN,
		       int targetX, double *act, int classN, double w) {

  MlpClassAccu        *accu;
  register int        k,i,hiddenN;
  register double     delta;
  
  accu = mlp->accu;
  assert(mlp->dimN == dimN);
  assert(mlp->classN == classN);
  hiddenN = mlp->hiddenN;
  
  /* -------------------------------------- */
  /*  compute delta's for the output layer  */
  /* -------------------------------------- */
  for (k=0; k<classN; k++) {
    if (k==targetX)
      accu->pv[k] = (w * (1.0 - act[k]));
    else
      accu->pv[k] = (w * (-act[k]));
  }

  /* ----------------------------------------------------------------- */
  /*  compute and accu updates for the hidden to output layer weights  */
  /* ----------------------------------------------------------------- */
  for (k=0; k<classN; k++) {
    delta = accu->pv[k];
    for (i=0; i<hiddenN; i++)
      accu->av[k][i] += (delta * mlp->act[i]);
    accu->av[k][hiddenN] += delta;
  }

  /* -------------------------------------- */
  /*  compute delta's for the hidden layer  */
  /* -------------------------------------- */
  for (k=0; k<hiddenN; k++)
    accu->pw[k] = 0.0;
  
  for (k=0; k<classN; k++) {
    delta = accu->pv[k];
    for (i=0; i<hiddenN; i++)
      accu->pw[i] += (mlp->v[k][i] * delta);
  }

  for (k=0; k<hiddenN; k++)
    accu->pw[k] *= (mlp->act[k] * (1.0 - mlp->act[k]));

  /* ---------------------------------------------------------------- */
  /*  compute and accu updates for the input to hidden layer weights  */
  /* ---------------------------------------------------------------- */
  for (k=0; k<hiddenN; k++) {
    delta = accu->pw[k];
    for (i=0; i<dimN; i++)
      accu->aw[k][i] += (delta * pattern[i]);
    accu->aw[k][dimN] += delta;
  }  
  
  /* --------------------- */
  /*  check for an update  */
  /* --------------------- */
  accu->trainN++;
  if (accu->trainN == mlp->batchN) {
    mlpClassUpdate(mlp);
    mlpClassClearAccusOnline(mlp);
  }

  return TCL_OK;
}



/* ----------------------------------------------------------------------- */
/*  mlpClassUpdate                                                         */
/* ----------------------------------------------------------------------- */
int  mlpClassUpdate (MlpClass *mlp) {

  MlpClassAccu        *accu;
  register int        classX,hidX,dimX;
  register double     factor;
  
  accu = mlp->accu;

  if (accu->trainN == 0) return TCL_OK;

  factor = (1.0 - mlp->nu) * (mlp->eta / accu->trainN);

  /* ----------------------------------------------------- */
  /*  computing new weight update matrix for output layer  */
  /* ----------------------------------------------------- */
  for (classX=0; classX<mlp->classN; classX++) {
    for (hidX=0; hidX<=mlp->hiddenN; hidX++) {
      accu->dv[classX][hidX] = mlp->nu * accu->dv[classX][hidX] +
	                        factor * accu->av[classX][hidX];
      mlp->v[classX][hidX] += accu->dv[classX][hidX];
    }
  }
  
  /* ----------------------------------------------------- */
  /*  computing new weight update matrix for hidden layer  */
  /* ----------------------------------------------------- */
  for (hidX=0; hidX<mlp->hiddenN; hidX++) {
    for (dimX=0; dimX<=mlp->dimN; dimX++) {
      accu->dw[hidX][dimX] = mlp->nu * accu->dw[hidX][dimX] +
	                      factor * accu->aw[hidX][dimX];
      mlp->w[hidX][dimX] += accu->dw[hidX][dimX];
    }
  }
  
  return TCL_OK;
}




/* ======================================================================= */
/*                     (2) janus3 classifier functions                     */
/* ======================================================================= */



/* ----------------------------------------------------------------------- */
/*  mlpClassCreateClf                                                      */
/* ----------------------------------------------------------------------- */
ClientData  mlpClassCreateClf (int classN, int dimN, int argc, char *argv[]) {

  int                ac = argc-1;
  int                hiddenN    = classN;
  int                trainMode  = 0;
  int                weightMode = 1;
  float              eta        = 1.0;
  int                batchN     = 100;
  float              initMax    = 0.01;
  float              nu         = 0.0;
  float              flatSpot   = 0.0;

  if ( itfParseArgv( argv[0], &ac, argv+1, 0,
      "-hiddenN",    ARGV_INT,   NULL, &hiddenN,    NULL, "no of hidden nodes",
      "-trainMode",  ARGV_INT,   NULL, &trainMode,  NULL, "training modus",
      "-weightMode", ARGV_INT,   NULL, &weightMode, NULL, "weighting modus",
      "-eta",        ARGV_FLOAT, NULL, &eta,        NULL, "learning rate",
      "-batchN",     ARGV_INT,   NULL, &batchN,     NULL, "batch size",
      "-initMax",    ARGV_FLOAT, NULL, &initMax,    NULL, "random init maximum",
      "-nu",         ARGV_FLOAT, NULL, &nu,         NULL, "momentum factor",
      "-flatSpot",   ARGV_FLOAT, NULL, &flatSpot,   NULL, "flat spot elimination offset",
  NULL) != TCL_OK) return NULL;

  return (ClientData) mlpClassCreate(classN,dimN,hiddenN,trainMode,
				     weightMode,eta,batchN,
				     initMax,nu,flatSpot);
}



/* ----------------------------------------------------------------------- */
/*  mlpClassCopyClf                                                        */
/* ----------------------------------------------------------------------- */
ClientData  mlpClassCopyClf (ClientData cd, double perturbe) {

  return (ClientData) mlpClassCopy((MlpClass *) cd,perturbe);
}



/* ----------------------------------------------------------------------- */
/*  mlpClassInitClf                                                        */
/* ----------------------------------------------------------------------- */
int  mlpClassInitClf (ClientData cd) {

  return mlpClassReset((MlpClass *) cd);
}



/* ----------------------------------------------------------------------- */
/*  mlpClassFreeClf                                                        */
/* ----------------------------------------------------------------------- */
int  mlpClassFreeClf (ClientData cd) {

  return mlpClassFree((MlpClass *) cd);
}



/* ----------------------------------------------------------------------- */
/*  mlpClassSaveClf                                                        */
/* ----------------------------------------------------------------------- */
int  mlpClassSaveClf (ClientData cd, FILE *fp) {

  return mlpClassSave((MlpClass *) cd,fp);
}



/* ----------------------------------------------------------------------- */
/*  mlpClassLoadClf                                                        */
/* ----------------------------------------------------------------------- */
ClientData  mlpClassLoadClf (FILE *fp) {

  return (ClientData)mlpClassLoad(NULL,fp);
}



/* ----------------------------------------------------------------------- */
/*  mlpClassCreateAccusClf                                                 */
/* ----------------------------------------------------------------------- */
int  mlpClassCreateAccusClf (ClientData cd) {

  return mlpClassCreateAccus((MlpClass *) cd);
}



/* ----------------------------------------------------------------------- */
/*  mlpClassFreeAccusClf                                                  */
/* ----------------------------------------------------------------------- */
int  mlpClassFreeAccusClf (ClientData cd) {

  return mlpClassFreeAccus((MlpClass *) cd);
}



/* ----------------------------------------------------------------------- */
/*  mlpClassClearAccusClf                                                  */
/* ----------------------------------------------------------------------- */
int  mlpClassClearAccusClf (ClientData cd) {

  return mlpClassClearAccus((MlpClass *) cd);
}



/* ----------------------------------------------------------------------- */
/*  mlpClassSaveAccusClf                                                   */
/* ----------------------------------------------------------------------- */
int  mlpClassSaveAccusClf (ClientData cd, FILE *fp) {

  return mlpClassSaveAccus((MlpClass *) cd,fp);
}



/* ----------------------------------------------------------------------- */
/*  mlpClassLoadAccusClf                                                   */
/* ----------------------------------------------------------------------- */
int  mlpClassLoadAccusClf (ClientData cd, FILE *fp) {

  return mlpClassLoadAccus((MlpClass *) cd,fp);
}



/* ----------------------------------------------------------------------- */
/*  mlpClassConfigureClf                                                   */
/* ----------------------------------------------------------------------- */
int  mlpClassConfigureClf (ClientData cd, int argc, char *argv[]) {

  register int      k;
  char              *var,*val;

  if ((argc % 2) != 1) {
    ERROR("invalid number of parameters\n");
    return TCL_ERROR;
  }
  
  for (k=1; k<argc; k+=2) {
    var = argv[k]+1;
    val = argv[k+1];
    if (mlpClassConfigureItf(cd,var,val) == TCL_ERROR) return TCL_ERROR;
  }
  
  return TCL_OK;
}



/* ----------------------------------------------------------------------- */
/*  mlpClassScoreClf                                                       */
/* ----------------------------------------------------------------------- */
int  mlpClassScoreClf (ClientData cd, float *pattern, int dimN, double *score,
		       int classN) {

  MlpClass          *mlp = (MlpClass *) cd;

  assert(dimN == mlp->dimN);
  assert(classN == mlp->classN);
  
  matFloatVecOne(mlp->w,pattern,mlp->act,mlp->hiddenN,dimN);
  vecSigmoid(mlp->act,mlp->hiddenN,mlp->flatSpot);
  matDoubleVecOne(mlp->v,mlp->act,score,classN,mlp->hiddenN);
  vecSoftmax(score,classN);

  return TCL_OK;
}



/* ----------------------------------------------------------------------- */
/*  mlpClassAccuSoftClf                                                    */
/* ----------------------------------------------------------------------- */
int  mlpClassAccuSoftClf (ClientData cd, float *pattern, int dimN,
			   double *target, double *act,
			   int classN, double g, double h) {

  MlpClass            *mlp = (MlpClass *) cd;

  if (mlp->trainMode == 0) return TCL_OK;

  if (mlp->weightMode)
    mlpClassAccuSoft (mlp,pattern,dimN,target,act,classN,h);
  else
    mlpClassAccuSoft (mlp,pattern,dimN,target,act,classN,g);

  return TCL_OK;
}



/* ----------------------------------------------------------------------- */
/*  mlpClassAccuHardClf                                                    */
/* ----------------------------------------------------------------------- */
int  mlpClassAccuHardClf (ClientData cd, float *pattern, int dimN,
			   int targetX, double *act,
			   int classN, double g, double h) {

  MlpClass            *mlp = (MlpClass *) cd;

  if (mlp->trainMode == 0) return TCL_OK;

  if (mlp->weightMode)
    mlpClassAccuHard (mlp,pattern,dimN,targetX,act,classN,h);
  else
    mlpClassAccuHard (mlp,pattern,dimN,targetX,act,classN,g);
   
  return TCL_OK;
}



/* ----------------------------------------------------------------------- */
/*  mlpClassUpdateClf                                                      */
/* ----------------------------------------------------------------------- */
int  mlpClassUpdateClf (ClientData cd) {

  MlpClass    *mlp = (MlpClass *) cd;

  if (mlp->trainMode == 0) return TCL_OK;

  return  mlpClassUpdate(mlp);
}




/* ====================================================================== */
/*                        (3) janus3 itf functions                        */
/* ====================================================================== */



/* ----------------------------------------------------------------------- */
/*  mlpClassAccuPutsItf                                                    */
/* ----------------------------------------------------------------------- */
int  mlpClassAccuPutsItf (ClientData cd, int argc, char *argv[]) {

  MlpClassAccu         *accu = (MlpClassAccu *) cd;
  register int         classX,hidX,dimX;
  
  itfAppendResult("{ deltaO { ");
  for (classX=0; classX<accu->classN; classX++)
    itfAppendResult("%f ",accu->pv[classX]);
  itfAppendResult("} } ");

  itfAppendResult("{ accuO { ");
  for (classX=0; classX<accu->classN; classX++) {
    itfAppendResult("{ ");
    for (hidX=0; hidX<=accu->hiddenN; hidX++)
      itfAppendResult("%f ",accu->av[classX][hidX]);
    itfAppendResult("} ");
  }
  itfAppendResult("} } ");

  itfAppendResult("{ dwO { ");
  for (classX=0; classX<accu->classN; classX++) {
    itfAppendResult("{ ");
    for (hidX=0; hidX<=accu->hiddenN; hidX++)
      itfAppendResult("%f ",accu->dv[classX][hidX]);
    itfAppendResult("} ");
  }
  itfAppendResult("} } ");

  
  itfAppendResult("{ deltaH { ");
  for (hidX=0; hidX<accu->hiddenN; hidX++)
    itfAppendResult("%f ",accu->pw[hidX]);
  itfAppendResult("} } ");

  itfAppendResult("{ accuH { ");
  for (hidX=0; hidX<accu->hiddenN; hidX++) {
    itfAppendResult("{ ");
    for (dimX=0; dimX<=accu->dimN; dimX++)
      itfAppendResult("%f ",accu->aw[hidX][dimX]);
    itfAppendResult("} ");
  }
  itfAppendResult("} } ");

  itfAppendResult("{ dwH { ");
  for (hidX=0; hidX<accu->hiddenN; hidX++) {
    itfAppendResult("{ ");
    for (dimX=0; dimX<=accu->dimN; dimX++)
      itfAppendResult("%f ",accu->dw[hidX][dimX]);
    itfAppendResult("} ");
  }
  itfAppendResult("} } ");


  itfAppendResult("{ trainN %d } ",accu->trainN);
  
  return TCL_OK;
}



/* ---------------------------------------------------------------------- */
/*  mlpClassCreateItf                                                     */
/* ---------------------------------------------------------------------- */
ClientData  mlpClassCreateItf (ClientData cd, int argc, char *argv[]) {

  int                ac;
  int                classN,dimN;
  
  ac = (argc < 3)?(argc-1):2;	
  if ( itfParseArgv( argv[0], &ac, argv+1, 1,
      "<classN>",    ARGV_INT,   NULL, &classN,     NULL, "number of output classes",
      "<dimN>",      ARGV_INT,   NULL, &dimN,       NULL, "feature dimension",
  NULL) != TCL_OK) return NULL;
	    
  return  mlpClassCreateClf(classN,dimN,argc-2,argv+2);
}



/* ----------------------------------------------------------------------- */
/*  mlpClassFreeItf                                                        */
/* ----------------------------------------------------------------------- */
int  mlpClassFreeItf (ClientData cd) {

  return mlpClassFree((MlpClass *) cd);
}



/* ----------------------------------------------------------------------- */
/*  mlpClassPutsItf                                                        */
/* ----------------------------------------------------------------------- */
int  mlpClassPutsItf (ClientData cd, int argc, char *argv[]) {

  MlpClass   *mlp    = (MlpClass *) cd;
  int        ac      = argc-1;
  int        classX  = -1;
  int        hiddenX = -1;
  int        hidX,dimX;
  
  if ( itfParseArgv( argv[0], &ac, argv+1, 1,
      "-classX",  ARGV_INT, NULL, &classX,  NULL, "weight vector of class X",
      "-hiddenX", ARGV_INT, NULL, &hiddenX, NULL, "weight vector of hidden node X",
  NULL) != TCL_OK) return TCL_ERROR;  

  if (((classX < 0) || (classX >= mlp->classN)) &&
      ((hiddenX < 0) || (hiddenX >= mlp->hiddenN))) {
  
    itfAppendResult("{classN %d} {dimN %d} {hiddenN %d} ",
	  	    mlp->classN,mlp->dimN,mlp->hiddenN);
    itfAppendResult("{trainMode %d} {weightMode %d} {batchN %d} ",
		    mlp->trainMode,mlp->weightMode,mlp->batchN);
    itfAppendResult("{initMax %f} {eta %f} ",
		    mlp->initMax,mlp->eta);
    itfAppendResult("{nu %f} {flatSpot %f} ",
		    mlp->nu,mlp->flatSpot);

  } else {

    if (classX >= 0) {
      itfAppendResult("{ ");
      for (hidX=0; hidX<=mlp->hiddenN; hidX++)
	itfAppendResult("%f ",mlp->v[classX][hidX]);
      itfAppendResult("} ");
    }
    if (hiddenX >= 0) {
      itfAppendResult("{ ");
      for (dimX=0; dimX<=mlp->dimN; dimX++)
	itfAppendResult("%f ",mlp->w[hiddenX][dimX]);
      itfAppendResult("} ");
    }

  }
  
  return TCL_OK;
}



/* ----------------------------------------------------------------------- */
/*  mlpClassConfigureItf                                                   */
/* ----------------------------------------------------------------------- */
int  mlpClassConfigureItf (ClientData cd, char *var, char *val) {

  MlpClass         *mlpClass = (MlpClass *) cd;

  if (! var) {
    ITFCFG(mlpClassConfigureItf,cd,"classN");
    ITFCFG(mlpClassConfigureItf,cd,"dimN");
    ITFCFG(mlpClassConfigureItf,cd,"hiddenN");    
    ITFCFG(mlpClassConfigureItf,cd,"trainMode");
    ITFCFG(mlpClassConfigureItf,cd,"weightMode");
    ITFCFG(mlpClassConfigureItf,cd,"eta");
    ITFCFG(mlpClassConfigureItf,cd,"batchN");
    ITFCFG(mlpClassConfigureItf,cd,"initMax");
    ITFCFG(mlpClassConfigureItf,cd,"nu");
    ITFCFG(mlpClassConfigureItf,cd,"flatSpot");
    return TCL_OK;
  }
  ITFCFG_Int   (var,val,"classN",     mlpClass->classN ,     1);
  ITFCFG_Int   (var,val,"dimN",       mlpClass->dimN ,       1);
  ITFCFG_Int   (var,val,"hiddenN",    mlpClass->hiddenN ,    1);
  ITFCFG_Int   (var,val,"trainMode",  mlpClass->trainMode ,  0);
  ITFCFG_Int   (var,val,"weightMode", mlpClass->weightMode , 0);
  ITFCFG_Float (var,val,"eta",        mlpClass->eta ,        0);
  ITFCFG_Int   (var,val,"batchN",     mlpClass->batchN ,     0);
  ITFCFG_Float (var,val,"initMax",    mlpClass->initMax ,    0);
  ITFCFG_Float (var,val,"nu",         mlpClass->nu ,         0);
  ITFCFG_Float (var,val,"flatSpot",   mlpClass->flatSpot ,   0);

  return TCL_OK;
}



/* ----------------------------------------------------------------------- */
/*  mlpClassAccessItf                                                      */
/* ----------------------------------------------------------------------- */
ClientData  mlpClassAccessItf (ClientData cd, char *name, TypeInfo **ti) {

  MlpClass         *mlpClass = (MlpClass *) cd;  
  
  if (*name=='.') {
    if (name[1]=='\0') {
      if (mlpClass->accu) itfAppendElement("accu");
    }
    else {
      if ((!strcmp(name+1,"accu")) && (mlpClass->accu)) {
        *ti=itfGetType("MlpAccu");
        return (ClientData)(mlpClass->accu);
      }
    }
  }
  *ti=NULL;
  return NULL;
}



/* ----------------------------------------------------------------------- */
/*  mlpClassCopyItf                                                        */
/* ----------------------------------------------------------------------- */
int  mlpClassCopyItf (ClientData cd, int argc, char *argv[]) {

  int               ac = argc-1;
  MlpClass         *source;
  float             perturbe = 0.0;
  
  if ( itfParseArgv( argv[0], &ac, argv+1, 0,
      "<source>",  ARGV_OBJECT, NULL, &source,    "Mlp", "source Mlp",
      "-perturbe", ARGV_FLOAT,  NULL, &perturbe,  NULL,   "additional random perturberation",
  NULL) != TCL_OK) return TCL_ERROR;  
  
  return TCL_OK;
}



/* ----------------------------------------------------------------------- */
/*  mlpClassResetItf                                                       */
/* ----------------------------------------------------------------------- */
int  mlpClassResetItf (ClientData cd, int argc, char *argv[]) {

  return  mlpClassReset((MlpClass *) cd);
}



/* ----------------------------------------------------------------------- */
/*  mlpClassSaveItf                                                        */
/* ----------------------------------------------------------------------- */
int  mlpClassSaveItf (ClientData cd, int argc, char *argv[]) {

  MlpClass              *mlp       = (MlpClass *) cd;
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
  
  if (mlpClassSave(mlp,fp) == TCL_ERROR) return TCL_ERROR;
  fileClose(filename,fp);
  
  return TCL_OK;
}



/* ----------------------------------------------------------------------- */
/*  mlpClassLoadItf                                                        */
/* ----------------------------------------------------------------------- */
int  mlpClassLoadItf (ClientData cd, int argc, char *argv[]) {

  MlpClass              *mlp       = (MlpClass *) cd;
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
  
  if (mlpClassLoad(mlp,fp) == NULL) return TCL_ERROR;
  fileClose(filename,fp);

  return TCL_OK;
}



/* ----------------------------------------------------------------------- */
/*  mlpClassCreateAccusItf                                                 */
/* ----------------------------------------------------------------------- */
int  mlpClassCreateAccusItf (ClientData cd, int argc, char *argv[]) {

  return  mlpClassCreateAccus((MlpClass *) cd);
}



/* ----------------------------------------------------------------------- */
/*  mlpClassFreeAccusItf                                                   */
/* ----------------------------------------------------------------------- */
int  mlpClassFreeAccusItf (ClientData cd, int argc, char *argv[]) {

  return  mlpClassFreeAccus((MlpClass *) cd);
}



/* ----------------------------------------------------------------------- */
/*  mlpClassClearAccusItf                                                  */
/* ----------------------------------------------------------------------- */
int  mlpClassClearAccusItf (ClientData cd, int argc, char *argv[]) {

  return  mlpClassClearAccus((MlpClass *) cd);
}



/* ----------------------------------------------------------------------- */
/*  mlpClassSaveAccusItf                                                   */
/* ----------------------------------------------------------------------- */
int  mlpClassSaveAccusItf (ClientData cd, int argc, char *argv[]) {

  MlpClass              *mlp       = (MlpClass *) cd;
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
  
  if (mlpClassSaveAccus(mlp,fp) == TCL_ERROR) return TCL_ERROR;
  fileClose(filename,fp);
  
  return TCL_OK;
}



/* ----------------------------------------------------------------------- */
/*  mlpClassLoadAccusItf                                                   */
/* ----------------------------------------------------------------------- */
int  mlpClassLoadAccusItf (ClientData cd, int argc, char *argv[]) {

  MlpClass              *mlp       = (MlpClass *) cd;
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
  
  if (mlpClassLoadAccus(mlp,fp) == TCL_ERROR) return TCL_ERROR;
  fileClose(filename,fp);

  return TCL_OK;
}



/* ----------------------------------------------------------------------- */
/*  mlpClassScoreItf                                                       */
/* ----------------------------------------------------------------------- */
int  mlpClassScoreItf (ClientData cd, int argc, char *argv[]) {

  MlpClass       *mlp        = (MlpClass *) cd;
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

  score = (double *) malloc (sizeof(double) * mlp->classN);
  mlpClassScoreClf(cd,pattern,mlp->dimN,score,mlp->classN);
  for (classX=0; classX<mlp->classN; classX++)
    itfAppendResult("%f ",score[classX]);
  free(score);
  
  return TCL_OK;
}



/* ----------------------------------------------------------------------- */
/*  mlpClassAccuHardItf                                                    */
/* ----------------------------------------------------------------------- */
int  mlpClassAccuHardItf (ClientData cd, int argc, char *argv[]) {

  MlpClass       *mlp        = (MlpClass *) cd;
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

  score = (double *) malloc (sizeof(double) * mlp->classN);
  mlpClassScoreClf(cd,pattern,mlp->dimN,score,mlp->classN);
  mlpClassAccuHardClf(cd,pattern,mlp->dimN,targetX,score,
		       mlp->classN,weight,weight);

  free(score);
  return TCL_OK;
}



/* ----------------------------------------------------------------------- */
/*  mlpClassUpdateItf                                                      */
/* ----------------------------------------------------------------------- */
int  mlpClassUpdateItf (ClientData cd, int argc, char *argv[]) {

  return  mlpClassUpdateClf(cd);
}




/* ====================================================================== */
/*                  (4) type declarations + initialization                */
/* ====================================================================== */


static Method mlpClassAccuMethod[] = 
{ 
  { "puts",           mlpClassAccuPutsItf,         NULL },
  {  NULL,  NULL, NULL } 
};



TypeInfo mlpClassAccuInfo = { 
        "MlpAccu", 0, -1, mlpClassAccuMethod, 
         NULL, NULL,
         NULL, NULL, itfTypeCntlDefaultNoLink,
        "Training accumulators for a Mlp\n" } ;



static Method mlpClassMethod[] = 
{ 
  { "puts",           mlpClassPutsItf,         NULL },
  { "copy",           mlpClassCopyItf,         "copy Mlp" },
  { "reset",          mlpClassResetItf,        "reset Mlp weights" },
  { "save",           mlpClassSaveItf,         "save parameters" },
  { "load",           mlpClassLoadItf,         "load parameters" },
  { "createAccus",    mlpClassCreateAccusItf,  "create accus for training" },
  { "freeAccus",      mlpClassFreeAccusItf,    "free accus for training" },
  { "clearAccus",     mlpClassClearAccusItf,   "clear accus" },
  { "saveAccus",      mlpClassSaveAccusItf,    "save accus" },
  { "loadAccus",      mlpClassLoadAccusItf,    "load and accu accus" },
  { "score",          mlpClassScoreItf,        "score a given frame" },
  { "accu",           mlpClassAccuHardItf,     "accu a given frame" },
  { "update",         mlpClassUpdateItf,       "update parameters" },
  {  NULL,  NULL, NULL } 
};



static Classifier classifierInfo = {
        "Mlp",
	 mlpClassCreateClf,
	 mlpClassCopyClf,
	 mlpClassInitClf,
	 mlpClassFreeClf,
	 mlpClassSaveClf,
	 mlpClassLoadClf,
	 mlpClassCreateAccusClf,
	 mlpClassFreeAccusClf,
	 mlpClassClearAccusClf,
	 mlpClassSaveAccusClf,
	 mlpClassLoadAccusClf,
	 mlpClassConfigureClf,
	 mlpClassScoreClf,
	 mlpClassAccuSoftClf,
 	 mlpClassAccuHardClf,
 	 mlpClassUpdateClf };



TypeInfo mlpClassInfo = { 
        "Mlp", 0, -1, mlpClassMethod, 
         mlpClassCreateItf, mlpClassFreeItf,
         mlpClassConfigureItf, mlpClassAccessItf, itfTypeCntlDefaultNoLink,
        "A multi layer perceptron (MLP) for classification\n" } ;




static int mlpInitialized = 0;

int Mlp_Init ( )
{
  if ( mlpInitialized) return TCL_OK;


  itfNewType    ( &mlpClassAccuInfo );
  itfNewType    ( &mlpClassInfo );
  classNewType  ( &classifierInfo );
  
  mlpInitialized = 1;
  return TCL_OK;
}





