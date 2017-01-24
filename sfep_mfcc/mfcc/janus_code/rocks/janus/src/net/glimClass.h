/* ========================================================================
    JANUS      Speech- to Speech Translation
               ------------------------------------------------------------
               Object: Multinomial Generalized Linear Model Classifier
               ------------------------------------------------------------

    Author  :  Juergen Fritsch
    Module  :  glimClass.h
    Date    :  $Id: glimClass.h 512 2000-08-27 15:31:32Z jmcd $
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
    Revision 3.3  2000/08/27 15:31:15  jmcd
    Merging branch 'jtk-00-08-24-jmcd'.

    Revision 3.2.4.1  2000/08/25 22:19:43  jmcd
    Just checking.

    Revision 3.2  1997/06/17 17:41:43  westphal
    *** empty log message ***

 * Revision 1.1  96/09/27  09:00:05  fritsch
 * Initial revision
 * 
    Revision 1.1  1995/12/21  13:49:57  fritsch
    Initial revision
 
   ======================================================================== */

#ifdef __cplusplus
extern "C" {
#endif

#ifndef _glimClass
#define _glimClass


/* ====================================================================== */
/*  GlimAccu     training accumulators                                    */
/* ====================================================================== */

typedef struct {

  int               classN;     /* number of output classes */
  int               dimN;       /* feature dimension */
  
  double            **xwx;      /* Least Squares XWX matrix */
  double            **xwv;      /* Least Squares XWV matrix */

  int               trainN;     /* number of training vectors */
  
} GlimClassAccu;



/* ===================================================================== */
/*  Glim       Multinomial Logit Model / linear predictor + softmax      */
/* ===================================================================== */

typedef struct {

  int               classN;      /* number of output classes */
  int               dimN;        /* feature dimension */
  
  double            **w;         /* weight matrix w[classX][dimX] */
  GlimClassAccu     *accu;       /* pointer to training accumulators */

  int               trainMode;   /* training mode/estimation algorithm */
  int               weightMode;  /* 0=use gating probs / 1=use posteriors */
  double            eta;         /* learning rate (for GA updates) */
  int               batchN;      /* batch size (for GA updates) */
  double            initMax;     /* random initialization maximum */
  double            logZero;     /* approximation to log(zero) */
  
} GlimClass;



/* ================================================================== */
/*                       misc prototypes                              */
/* ================================================================== */

int       myRandomInit        (double seed);
float     myRandom            (double a, double b);

int       matInitRandom       (double **mat, int m, int n, double initMax);
double    matInvert           (double **a, int n);



/* ================================================================= */
/*                      classifier prototypes                        */
/* ================================================================= */

ClientData  glimClassCreateClf        (int classN, int dimN, int argc, char *argv[]);
ClientData  glimClassCopyClf          (ClientData cd, double perturbe);
int         glimClassInitClf          (ClientData cd);
int         glimClassFreeClf          (ClientData cd);
int         glimClassSaveClf          (ClientData cd, FILE *fp);
ClientData  glimClassLoadClf          (FILE *fp);
int         glimClassCreateAccusClf   (ClientData cd);
int         glimClassFreeAccusClf     (ClientData cd);
int         glimClassClearAccusClf    (ClientData cd);
int         glimClassSaveAccusClf     (ClientData cd, FILE *fp);
int         glimClassLoadAccusClf     (ClientData cd, FILE *fp);
int         glimClassConfigureClf     (ClientData cd, int argc, char *argv[]);
int         glimClassScoreClf         (ClientData cd, float *pattern, int dimN,
		        	       double *score, int classN);
int         glimClassAccuSoftClf      (ClientData cd, float *pattern, int dimN,
		        	       double *target, double *act,
			               int classN, double g, double h);
int         glimClassAccuHardClf      (ClientData cd, float *pattern, int dimN,
			               int targetX, double *act,
			               int classN, double g, double h);
int         glimClassUpdateClf        (ClientData cd);



/* ================================================================= */
/*                           itf prototypes                          */
/* ================================================================= */

int         glimClassAccuPutsItf     (ClientData cd, int argc, char *argv[]);

ClientData  glimClassCreateItf       (ClientData cd, int argc, char *argv[]);
int         glimClassFreeItf         (ClientData cd);
int         glimClassConfigureItf    (ClientData cd, char *var, char *val);
ClientData  glimClassAccessItf       (ClientData cd, char *name, TypeInfo **ti);

int         glimClassPutsItf         (ClientData cd, int argc, char *argv[]);
int         glimClassWeightsItf      (ClientData cd, int argc, char *argv[]);
int         glimClassCopyItf         (ClientData cd, int argc, char *argv[]);
int         glimClassInitItf         (ClientData cd, int argc, char *argv[]);

int         glimClassSaveItf         (ClientData cd, int argc, char *argv[]);
int         glimClassLoadItf         (ClientData cd, int argc, char *argv[]);

int         glimClassCreateAccusItf  (ClientData cd, int argc, char *argv[]);
int         glimClassFreeAccusItf    (ClientData cd, int argc, char *argv[]);
int         glimClassClearAccusItf   (ClientData cd, int argc, char *argv[]);
int         glimClassSaveAccusItf    (ClientData cd, int argc, char *argv[]);
int         glimClassLoadAccusItf    (ClientData cd, int argc, char *argv[]);

int         glimClassScoreItf        (ClientData cd, int argc, char *argv[]);
int         glimClassAccuHardItf     (ClientData cd, int argc, char *argv[]);
int         glimClassUpdateItf       (ClientData cd, int argc, char *argv[]);




#endif

#ifdef __cplusplus
}
#endif
