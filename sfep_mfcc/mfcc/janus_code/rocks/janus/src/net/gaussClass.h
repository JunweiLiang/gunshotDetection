/* ========================================================================
    JANUS      Speech- to Speech Translation
               ------------------------------------------------------------
               Object: Multivariate Diagonal Covariance Gauss Classifier
               ------------------------------------------------------------

    Author  :  Juergen Fritsch
    Module  :  gaussClass.h
    Date    :  $Id: gaussClass.h 512 2000-08-27 15:31:32Z jmcd $
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

 * Revision 1.1  96/09/27  09:00:25  fritsch
 * Initial revision
 * 
    Revision 1.1  1995/12/21  13:49:57  fritsch
    Initial revision
 
   ======================================================================== */

#ifdef __cplusplus
extern "C" {
#endif

#ifndef _gaussClass
#define _gaussClass




/* =============================================================== */
/*  IndexList                                                      */
/* =============================================================== */
typedef struct {

  int       listN;  /* size of list */
  int       *list;  /* array of list elements (indices) */

} IndexList;



/* =============================================================== */
/*  BoxSet                                                         */
/* =============================================================== */
typedef struct {

  int            rbfN;       /* number of radial basis functions */
  int            dimN;       /* dimension of feature vectors */
  int            *act;       /* array of active indicators */
  float          **min;      /* min[dimX][rbfX] contains lower boundaries */
  float          **max;      /* max[dimX][rbfX] contains upper boundaries */

}  BoxSet;



/* ============================================================== */
/*  BSPNode                                                       */
/* ============================================================== */
typedef struct {

  int           splitX;       /* splitting coordinate index */
  float         splitVal;     /* splitting value */

}  BSPNode;



/* ============================================================== */
/*  BSPTree -- Binary space partitioning tree                     */
/* ============================================================== */
typedef struct {

  int           depthN;      /* depth of the tree */
  int           leafN;       /* number of leafs in the tree */
  BSPNode       *node;       /* array of internal tree nodes */
  IndexList     *leaf;       /* array of tree leafs */

}  BSPTree;



/* ====================================================================== */
/*  GaussClassAccu       training accumulators                            */
/* ====================================================================== */

typedef struct {

  int               classN;     /* number of output classes */
  int               dimN;       /* feature dimension */

  double            *a;         /* accus for apriori distributions */
  double            **x;        /* accus for expected value of x   */
  double            **xx;       /* accus for expected value of x*x */

  double            dist;       /* distortion during clustering */
  double            lh;         /* log likelihood of mixture density */

  int               trainN;     /* number of training vectors */
  
} GaussClassAccu;



/* ===================================================================== */
/*  GaussClass          multivariate diagonal variance Gauss classifier  */
/* ===================================================================== */

typedef struct {

  int               classN;      /* number of output classes */
  int               dimN;        /* feature dimension */

  double            *pre;        /* Gaussian pre=factors */
  double            *alpha;      /* array of a-priori probabilities */
  double            **mean;      /* array of mean vectors */
  double            **ivar;      /* array of inverse variance vectors */

  int               trainMode;   /* training mode/estimation algorithm */
  int               weightMode;  /* 0=gating probs / 1=posterior probs */

  GaussClassAccu    *accu;       /* pointer to training accumulators */
  BSPTree           *tree;       /* pointer to BSP-tree */

} GaussClass;



/* ================================================================== */
/*                       misc prototypes                              */
/* ================================================================== */

BoxSet       *boxSetCreate     (int rbfN, int dimN);
int          boxSetFree        (BoxSet *boxSet);
int          boxSetInit        (BoxSet *boxSet, GaussClass *gc,
				float threshold);
BSPTree      *bspTreeCreate    (int depthN, BoxSet *boxSet);
int          bspTreeFree       (BSPTree *tree);
IndexList    *bspTreeUse       (BSPTree *tree, float *pattern);

int          gaussClassEval    (GaussClass *gc, float *pattern, int dimN,
		                double *act, int classN);


  
/* ================================================================= */
/*                      classifier prototypes                        */
/* ================================================================= */

ClientData  gaussClassCreateClf       (int classN, int dimN, int argc, char *argv[]);
ClientData  gaussClassCopyClf         (ClientData cd, double perturbe);
int         gaussClassInitClf         (ClientData cd);
int         gaussClassFreeClf         (ClientData cd);
int         gaussClassSaveClf         (ClientData cd, FILE *fp);
ClientData  gaussClassLoadClf         (FILE *fp);
int         gaussClassCreateAccusClf  (ClientData cd);
int         gaussClassFreeAccusClf    (ClientData cd);
int         gaussClassClearAccusClf   (ClientData cd);
int         gaussClassSaveAccusClf    (ClientData cd, FILE *fp);
int         gaussClassLoadAccusClf    (ClientData cd, FILE *fp);
int         gaussClassConfigureClf    (ClientData cd, int argc, char *argv[]);
int         gaussClassScoreClf        (ClientData cd, float *pattern, int dimN,
		        	       double *score, int classN);
int         gaussClassAccuSoftClf     (ClientData cd, float *pattern, int dimN,
		        	       double *target, double *act,
			               int classN, double g, double h);
int         gaussClassAccuHardClf     (ClientData cd, float *pattern, int dimN,
			               int targetX, double *act,
			               int classN, double g, double h);
int         gaussClassUpdateClf       (ClientData cd);



/* ================================================================= */
/*                           itf prototypes                          */
/* ================================================================= */

int         gaussClassAccuPutsItf     (ClientData cd, int argc, char *argv[]);

ClientData  gaussClassCreateItf       (ClientData cd, int argc, char *argv[]);
int         gaussClassFreeItf         (ClientData cd);
int         gaussClassConfigureItf    (ClientData cd, char *var, char *val);
ClientData  gaussClassAccessItf       (ClientData cd, char *name, TypeInfo **ti);

int         gaussClassPutsItf         (ClientData cd, int argc, char *argv[]);
int         gaussClassMeansItf        (ClientData cd, int argc, char *argv[]);
int         gaussClassIvarsItf        (ClientData cd, int argc, char *argv[]);
int         gaussClassCopyItf         (ClientData cd, int argc, char *argv[]);
int         gaussClassInitItf         (ClientData cd, int argc, char *argv[]);

int         gaussClassSaveItf         (ClientData cd, int argc, char *argv[]);
int         gaussClassLoadItf         (ClientData cd, int argc, char *argv[]);

int         gaussClassCreateAccusItf  (ClientData cd, int argc, char *argv[]);
int         gaussClassFreeAccusItf    (ClientData cd, int argc, char *argv[]);
int         gaussClassClearAccusItf   (ClientData cd, int argc, char *argv[]);
int         gaussClassSaveAccusItf    (ClientData cd, int argc, char *argv[]);
int         gaussClassLoadAccusItf    (ClientData cd, int argc, char *argv[]);

int         gaussClassScoreItf        (ClientData cd, int argc, char *argv[]);
int         gaussClassAccuHardItf     (ClientData cd, int argc, char *argv[]);
int         gaussClassUpdateItf       (ClientData cd, int argc, char *argv[]);




#endif

#ifdef __cplusplus
}
#endif
