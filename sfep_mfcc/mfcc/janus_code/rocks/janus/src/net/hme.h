/* ========================================================================
    JANUS      Speech- to Speech Translation
               ------------------------------------------------------------
               Object: Hierarchical Mixtures of Experts
               ------------------------------------------------------------

    Author  :  Juergen Fritsch
    Module  :  hme.h
    Date    :  $Id: hme.h 739 2001-01-15 09:50:00Z janus $
    Remarks :  Hierarchical modular neural networks

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
    Revision 3.4  2001/01/15 09:49:57  janus
    Merged Christian's version with WINDOWS support (branch jtk-00-11-14-fuegen)

    Revision 3.3.36.1  2001/01/12 15:16:55  janus
    necessary changes for running janus under WINDOWS

    Revision 3.3  2000/08/27 15:31:16  jmcd
    Merging branch 'jtk-00-08-24-jmcd'.

    Revision 3.2.4.1  2000/08/25 22:19:43  jmcd
    Just checking.

    Revision 3.2  1997/06/17 17:41:43  westphal
    *** empty log message ***

 * Revision 1.2  96/09/27  08:58:40  fritsch
 * redesigned the HME system
 * 
    Revision 1.1  1995/12/21  13:49:57  fritsch
    Initial revision
 
   ======================================================================== */

#ifdef __cplusplus
extern "C" {
#endif

#ifndef _hme
#define _hme

#include "classifier.h"


typedef enum { GATE, EXPERT } NodeType;


/* ==================================================================== */
/*  HmeNode                                                             */
/* ==================================================================== */

typedef struct HmeNode_S HmeNode;

typedef struct HmeNode_S {

  NodeType             nodeType;         /* node type */
  Classifier           *classInfo;       /* classifier access info */
  ClientData           classData;        /* classifier data */

  int                  classN;           /* number of output classes */
  int                  dimN;             /* dimension of feature vectors */
  
  double               *mu;              /* HmeNode output activation */

  int                  childN;           /* number of children */
  HmeNode              **childPA;        /* array of pointers to children */
  
  double               hcum;             /* cummulative node posterior */
  double               prob;             /* expert probability */
  double               pen;              /* expert penalty */
  double               *g;               /* gating probabilities */
  double               *h;               /* posterior probabilities */
  
} HmeNode_Fake;  


/* ==================================================================== */
/*  HmeTestEnv                                                          */
/* ==================================================================== */

typedef struct {

  int            dataN;             /* maximal number of test vectors */
  int            skipN;             /* number of skips when extracting */
  int            skipX;             /* counter for skipping detection */
  int            gateDimN;          /* gate feature dimension */
  int            expDimN;           /* expert feature dimension */
  float          **gateData;        /* gate test vectors */
  float          **expData;         /* expert test vectors */
  int            *target;           /* target indices */

  int              testN;           /* number of test vectors */
  double           xentropy;        /* cross-entropy */
  double           mseMean;         /* mean of MSE */
  double           classError;      /* classification error */

  double           dxentropy;       /* delta of xentropy */
  double           dmseMean;        /* delta of MSE */
  double           dclassError;     /* delta of class error */
  
} HmeTestEnv;


/* ==================================================================== */
/*  Hme                                                                 */
/* ==================================================================== */

typedef struct {

  char             *name;           /* name of the object */
  int              useN;            /* usage counter */
  
  int              classN;          /* number of output classes */
  int              gateDimN;        /* dimension of gate feature vectors */
  int              expDimN;         /* dimension of expert feature vectors */

  int              frameX;          /* current frame */
  double           *count;          /* counts for priors */
  double           *prior;          /* array of log of priors */

  int              xtrTest;         /* boolean, extract test vectors? */
  int              evalPen;         /* boolean, evaluate penalty function? */
  int              initLayer;       /* if > 0 : init nodes in given layer */
  double           pruneT;          /* pruning threshold */
  
  int              trainN;          /* number of training vectors */
  double           likelihood;      /* log-likelihood of training set */

  HmeTestEnv       *testEnv;        /* pointer to test environment */
  
  char             *defClass;       /* default classifier for growing */
  char             *defClassOpts;   /* default classifier options */
  int              defChildN;       /* default number of children */
  double           defPerturbe;     /* default max random perturberation */

  HmeNode          *root;           /* root node in the Hme tree */  

} Hme;



char        **createArgv      (char *methodName, char *options, int *argc);



int         hmeFree           (Hme *hme);
int         hmeSave           (Hme *hme, FILE *fp);
Hme         *hmeLoad          (Hme *hme, FILE *fp);
int         hmeReset          (Hme *hme);
int         hmeConfHme        (Hme *hme, int argc, char *argv[]);
int         hmeConfGates      (Hme *hme, int argc, char *argv[]);
int         hmeConfExperts    (Hme *hme, int argc, char *argv[]);
double      hmeScore          (Hme *hme, float *gatePat, float *expPat,
			       int frameX, int classX, int divPrior);
int         hmeCreateAccus    (Hme *hme);
int         hmeFreeAccus      (Hme *hme);
int         hmeClearAccus     (Hme *hme);
int         hmeSaveAccus      (Hme *hme, FILE *fp);
int         hmeLoadAccus      (Hme *hme, FILE *fp);
int         hmeAccu           (Hme *hme, float *gatePat, float *expPat,
			       int classX);
int         hmeUpdate         (Hme *hme);
int         hmeCreateTest     (Hme *hme, int dataN, int skipN);
int         hmeTestEnvFree    (HmeTestEnv *testEnv);
int         hmeSaveTestEnv    (Hme *hme, FILE *fp);
int         hmeLoadTestEnv    (Hme *hme, FILE *fp);
int         hmeTest           (Hme *hme, int testN);

int         hmeNodePutsItf    (ClientData cd, int argc, char *argv[]);
ClientData  hmeNodeAccessItf  (ClientData cd, char *name, TypeInfo **ti);
int         hmeNodePlugItf    (ClientData cd, int argc, char *argv[]);
int         hmeNodeGrowItf    (ClientData cd, int argc, char *argv[]);
int         hmeNodeExpandItf  (ClientData cd, int argc, char *argv[]);



ClientData  hmeCreateItf      (ClientData cd, int argc, char *argv[]);
int         hmeFreeItf        (ClientData cd);
int         hmePutsItf        (ClientData cd, int argc, char *argv[]);
int         hmeConfigureItf   (ClientData cd, char *var, char *val);
ClientData  hmeAccessItf      (ClientData cd, char *name, TypeInfo **ti);
int         hmeSaveItf        (ClientData cd, int argc, char *argv[]);
int         hmeLoadItf        (ClientData cd, int argc, char *argv[]);
int         hmeResetItf       (ClientData cd, int argc, char *argv[]);
int         hmeConfGatesItf   (ClientData cd, int argc, char *argv[]);
int         hmeConfExpertsItf (ClientData cd, int argc, char *argv[]);
int         hmeScoreItf       (ClientData cd, int argc, char *argv[]);
int         hmeCreateAccusItf (ClientData cd, int argc, char *argv[]);
int         hmeFreeAccusItf   (ClientData cd, int argc, char *argv[]);
int         hmeClearAccusItf  (ClientData cd, int argc, char *argv[]);
int         hmeSaveAccusItf   (ClientData cd, int argc, char *argv[]);
int         hmeLoadAccusItf   (ClientData cd, int argc, char *argv[]);
int         hmePosteriorItf   (ClientData cd, int argc, char *argv[]);
int         hmeAccuItf        (ClientData cd, int argc, char *argv[]);
int         hmeUpdateItf      (ClientData cd, int argc, char *argv[]);


  
#endif








#ifdef __cplusplus
}
#endif
