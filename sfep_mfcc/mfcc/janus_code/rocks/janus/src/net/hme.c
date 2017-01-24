/* ========================================================================
    JANUS      Speech- to Speech Translation
               ------------------------------------------------------------
               Object: Hierarchical Mixtures of Experts
               ------------------------------------------------------------

    Author  :  Juergen Fritsch
    Module  :  hme.c
    Date    :  $Id: hme.c 2390 2003-08-14 11:20:32Z fuegen $
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
    Revision 3.4  2003/08/14 11:20:20  fuegen
    Merged changes on branch jtk-01-01-15-fms (jaguar -> ibis-013)

    Revision 3.3.4.2  2002/06/26 12:26:50  fuegen
    go away, boring message

    Revision 3.3.4.1  2002/06/26 11:57:57  fuegen
    changes for new directory structure support and code cleaning

    Revision 3.3  2001/01/15 09:49:57  janus
    Merged Christian's version with WINDOWS support (branch jtk-00-11-14-fuegen)

    Revision 3.2.40.1  2001/01/12 15:16:55  janus
    necessary changes for running janus under WINDOWS

    Revision 3.2  1997/06/17 17:41:43  westphal
    *** empty log message ***

 * Revision 1.2  96/09/27  08:58:24  fritsch
 * redesigned the HME system
 * 
    Revision 1.1  1995/12/21  13:49:57  fritsch
    Initial revision
 
   ======================================================================== */

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "common.h"
#include "itf.h"

#include "error.h"
#include "matrix.h"
#include "mach_ind_io.h"
#include "feature.h"
#include "classifier.h"
#include "glimClass.h"
#include "gaussClass.h"
#include "mlpClass.h"
#include "hme.h"


#define   HME_MAGIC                           1234
#define   HME_ACCUS_MAGIC                     2345
#define   TESTENV_MAGIC                       9876
#define   MINVAL                              1E-20
#define   MAXVAL                              1E+20
#define   DEFAULT_CLASSIFIER                  "Glim"
#define   DEFAULT_CLASSIFIER_OPTIONS          "-trainMode 0"
#define   square(X)                           ((X)*(X))



/* ======================================================================= */
/*                            Implementation                               */
/* ======================================================================= */


/* ------------------------------------------------------------------- */
/*  converts a string of optional arguments into an (argc,argv) array  */
/*  suitable for parsing with itfParseArgv()                           */
/* ------------------------------------------------------------------- */
char  **createArgv (char *methodName, char *options, int *argc) {

  char               *token;
  int                argcNew;
  char               **argvNew;
  char               *optStr;
  
  optStr = strdup(options);
  argcNew = 1;
  token = strtok(optStr," \t");
  while (token != NULL) {
    argcNew++;
    token = strtok(NULL," \t");
  } 

  argvNew = (char **) malloc (sizeof(char *) * argcNew);
  argvNew[0] = strdup(methodName);
  strcpy(optStr,options);
  argcNew = 1;
  token = strtok(optStr," \t");
  while (token != NULL) {
    argvNew[argcNew++] = strdup(token);
    token = strtok(NULL," \t");
  }

  free(optStr);
  *argc = argcNew;
  return argvNew;
}
 


/* ----------------------------------------------------------------------- */
/*  hmeNodeCreate                                                          */
/* ----------------------------------------------------------------------- */
HmeNode  *hmeNodeCreate (NodeType nodeType, int classN, int dimN, int childN,
			 char *className, char *options) {

  HmeNode            *hmeNode;
  register int       childX,k,classX,outN;
  int                argcNew;
  char               **argvNew;

  /* ---------------------------------- */
  /*  allocate empty HmeNode structure  */ 
  /* ---------------------------------- */
  hmeNode = (HmeNode *) malloc (sizeof(HmeNode));

  /* ----------------------------- */
  /*  fill in basic scalar values  */
  /* ----------------------------- */
  hmeNode->nodeType  = nodeType;
  hmeNode->classN    = classN;
  hmeNode->dimN      = dimN;
  hmeNode->childN    = childN;

  /* -------------------------------------- */
  /*  check if classifier type is existing  */
  /* -------------------------------------- */
  if ((hmeNode->classInfo = classGetByName(className)) == NULL) {
    ERROR("no classifier type '%s' existing\n",className);
    return NULL;
  }

  /* ------------------------------------------------------------- */
  /*  fill argc and argv for the following classifier constructor  */
  /* ------------------------------------------------------------- */
  argvNew = createArgv("createFct",options,&argcNew);
  
  /* ---------------------------------------------- */
  /*  create an instance of classifier 'className'  */
  /* ---------------------------------------------- */
  if (nodeType == GATE) outN = childN; else outN = classN;
  if ((hmeNode->classData = (ClientData)
       hmeNode->classInfo->createFct(outN,dimN,argcNew,
				     (char **) argvNew)) == NULL) {
    ERROR("error trying to create classifier of type %s\n",className);
    return NULL;
  }

  /* -------------------------------- */
  /*  free memory for argument lists  */
  /* -------------------------------- */
  for (k=0; k<argcNew; k++)
    free(argvNew[k]);
  free(argvNew);
  
  /* -------------------------------- */
  /*  create output activation array  */
  /* -------------------------------- */
  hmeNode->mu = (double *) malloc (sizeof(double) * classN);
  for (classX=0; classX<classN; classX++) hmeNode->mu[classX] = 0.0;
  
  /* -------------------- */
  /*  init scalar values  */ 
  /* -------------------- */
  hmeNode->hcum = 0.0;
  hmeNode->prob = 0.0;
  hmeNode->pen  = 0.0;
  
  if (nodeType == GATE) {
    
    /* --------------------------------- */
    /*  additional gate data structures  */
    /* --------------------------------- */
    hmeNode->childPA = (HmeNode **) malloc (sizeof(HmeNode *) * childN);
    for (childX=0; childX<childN; childX++)
      hmeNode->childPA[childX] = NULL;

    hmeNode->g = (double *) malloc (sizeof(double) * childN);
    for (childX=0; childX<childN; childX++) hmeNode->g[childX] = 0.0;
    
    hmeNode->h = (double *) malloc (sizeof(double) * childN);
    for (childX=0; childX<childN; childX++) hmeNode->h[childX] = 0.0;
  }

  return hmeNode;
}



/* ----------------------------------------------------------------------- */
/*  hmeNodeFree  -- recursive node destructor                              */
/* ----------------------------------------------------------------------- */
int  hmeNodeFree (HmeNode *hmeNode) {

  register int                  childX;
  
  /* ----------------- */
  /*  free classifier  */
  /* ----------------- */
  hmeNode->classInfo->freeFct(hmeNode->classData);
  
  /* ---------------------------------- */
  /*  free array of output activations  */
  /* ---------------------------------- */
  free(hmeNode->mu);

  /* -------------------------------------------------------- */
  /*  free additional data structures in case this is a gate  */
  /* -------------------------------------------------------- */
  if (hmeNode->nodeType == GATE) {
    free(hmeNode->g);
    free(hmeNode->h);

    /* ---------------------------------------- */
    /*  recursive calls for the children nodes  */
    /* ---------------------------------------- */
    for (childX=0; childX<hmeNode->childN; childX++)
      if (hmeNode->childPA[childX])
        hmeNodeFree(hmeNode->childPA[childX]);

    free(hmeNode->childPA);    
  }

  free(hmeNode);
  return TCL_OK;
}


/* --------------------------------------------------------------------- */
/*  hmeNodePlug                                                          */
/* --------------------------------------------------------------------- */
int  hmeNodePlug (HmeNode *hmeNode, char *className, ClientData classData) {

  /* -------------------------- */
  /*  free existing classifier  */
  /* -------------------------- */
  hmeNode->classInfo->freeFct(hmeNode->classData);

  /* ------------------------ */
  /*  plug in new classifier  */
  /* ------------------------ */
  if ((hmeNode->classInfo = classGetByName(className)) == NULL) {
    ERROR("no classifier type '%s' existing\n",className);
    return TCL_ERROR;
  }
  hmeNode->classData = classData;

  return TCL_OK;
}



/* --------------------------------------------------------------------- */
/*  hmeNodeGrow                                                          */
/* --------------------------------------------------------------------- */
int  hmeNodeGrow (HmeNode *hmeNode, int childN, int gateDimN,
		  char *gateClass, char *gateOpt, double perturbe) {

  HmeNode           *node;
  Classifier        *classInfo;
  ClientData        classData;
  int               argcNew,childX,classX,dimN,k;
  char              **argvNew;
  
  /* -------------------------------------- */
  /*  save pointers to existing classifier  */
  /* -------------------------------------- */
  classInfo = hmeNode->classInfo;
  classData = hmeNode->classData;
  dimN      = hmeNode->dimN;
  
  /* -------------------------------------- */
  /*  change node from an expert to a gate  */
  /* -------------------------------------- */
  hmeNode->nodeType = GATE;

  /* -------------------------------------- */
  /*  check if classifier type is existing  */
  /* -------------------------------------- */
  if ((hmeNode->classInfo = classGetByName(gateClass)) == NULL) {
    ERROR("no classifier type '%s' existing\n",gateClass);
    return TCL_ERROR;
  }

  /* ------------------------------------------------------------- */
  /*  fill argc and argv for the following classifier constructor  */
  /* ------------------------------------------------------------- */
  argvNew = createArgv("createFct",gateOpt,&argcNew);

  /* ---------------------------------------------- */
  /*  create an instance of classifier 'gateClass'  */
  /* ---------------------------------------------- */
  if ((hmeNode->classData = (ClientData)
       hmeNode->classInfo->createFct(childN,gateDimN,argcNew,
				     (char **) argvNew)) == NULL) {
    ERROR("error trying to create classifier of type %s\n",gateClass);
    return TCL_ERROR;
  }

  /* -------------------------------- */
  /*  free memory for argument lists  */
  /* -------------------------------- */
  for (k=0; k<argcNew; k++)
    free(argvNew[k]);
  free(argvNew);
  
  /* ---------------------------------------- */
  /*  create additional gate data structures  */
  /* ---------------------------------------- */
  hmeNode->dimN    = gateDimN;
  hmeNode->childN  = childN;
  hmeNode->childPA = (HmeNode **) malloc (sizeof(HmeNode *) * childN);
  hmeNode->g       = (double *)   malloc (sizeof(double)    * childN);
  for (childX=0; childX<childN; childX++) hmeNode->g[childX] = 0.0;
  hmeNode->h = (double *) malloc (sizeof(double) * childN);
  for (childX=0; childX<childN; childX++) hmeNode->h[childX] = 0.0;
  
  /* -------------------------------------------------------------- */
  /*  copy old expert classifier into new experts for the new gate  */
  /* -------------------------------------------------------------- */
  for (childX=0; childX<hmeNode->childN; childX++) {

    /* ---------------------------------- */
    /*  allocate empty HmeNode structure  */ 
    /* ---------------------------------- */
    hmeNode->childPA[childX] = (HmeNode *) malloc (sizeof(HmeNode));
    node = hmeNode->childPA[childX];
  
    /* ----------------------------- */
    /*  fill in basic scalar values  */
    /* ----------------------------- */
    node->nodeType  = EXPERT;
    node->classN    = hmeNode->classN;
    node->dimN      = dimN;
    node->childN    = 0;
    node->hcum      = 0.0;
    node->prob      = 0.0;
    node->pen       = 0.0;
    
    /* -------------------------------- */
    /*  create output activation array  */
    /* -------------------------------- */
    node->mu = (double *) malloc (sizeof(double) * hmeNode->classN);
    for (classX=0; classX<node->classN; classX++) hmeNode->mu[classX] = 0.0;

    /* ---------------------------- */
    /*  copy old expert classifier  */
    /* ---------------------------- */
    node->classInfo = classInfo;
    node->classData = (ClientData) classInfo->copyFct(classData,perturbe);
  }
    
  /* ------------------------------- */
  /*  destroy old expert classifier  */
  /* ------------------------------- */
  classInfo->freeFct(classData);
  
  return TCL_OK;
}



/* --------------------------------------------------------------------- */
/*  hmeNodeExpand                                                        */
/* --------------------------------------------------------------------- */
int  hmeNodeExpand (HmeNode *hmeNode, char *expClass, char *expOpt) {

  register int             childX;
  
  /* ------------------------------------------ */
  /*  convert the expert node into a gate node  */
  /* ------------------------------------------ */
  hmeNode->nodeType = GATE;

  /* ---------------------------------------- */
  /*  create additional gate data structures  */
  /* ---------------------------------------- */
  hmeNode->childN  = hmeNode->classN;
  hmeNode->childPA = (HmeNode **) malloc (sizeof(HmeNode *) * hmeNode->childN);
  hmeNode->g       = (double *)   malloc (sizeof(double)    * hmeNode->childN);
  for (childX=0; childX<hmeNode->childN; childX++) hmeNode->g[childX] = 0.0;
  hmeNode->h = (double *) malloc (sizeof(double) * hmeNode->childN);
  for (childX=0; childX<hmeNode->childN; childX++) hmeNode->h[childX] = 0.0;

  /* -------------------------------------- */
  /*  create new expert nodes for the gate  */
  /* -------------------------------------- */
  for (childX=0; childX<hmeNode->childN; childX++)
    hmeNode->childPA[childX] = hmeNodeCreate(EXPERT,hmeNode->classN,
					     hmeNode->dimN,hmeNode->childN,
					     expClass,expOpt);

  return TCL_OK;
}



/* --------------------------------------------------------------------- */
/*  hmeNodeHme                                                           */
/* --------------------------------------------------------------------- */
int  hmeNodeHme (HmeNode *hmeNode, Hme *hme) {

  /* -------------------------------------------- */
  /*  free current expert node's data structures  */
  /* -------------------------------------------- */
  free(hmeNode->mu);

  /* ------------------------------------------------------ */
  /*  copy / reference data structures in root node of Hme  */
  /* ------------------------------------------------------ */
  hmeNode->nodeType   = hme->root->nodeType;
  hmeNode->classInfo  = hme->root->classInfo;
  hmeNode->classData  = hme->root->classData;
  hmeNode->classN     = hme->root->classN;
  hmeNode->dimN       = hme->root->dimN;
  hmeNode->mu         = hme->root->mu;
  hmeNode->childN     = hme->root->childN;
  hmeNode->childPA    = hme->root->childPA;
  hmeNode->hcum       = hme->root->hcum;
  hmeNode->prob       = hme->root->prob;
  hmeNode->pen        = hme->root->pen;
  hmeNode->g          = hme->root->g;
  hmeNode->h          = hme->root->h;
  
  return TCL_OK;
}



/* --------------------------------------------------------------------- */
/*  hmeTestEnvInit                                                       */
/* --------------------------------------------------------------------- */
int  hmeTestEnvInit (HmeTestEnv *testEnv, int dataN,
		     int gateDimN, int expDimN,
		     int skipN) {

  int         dataX;

  if (!testEnv) return TCL_ERROR;
  
  testEnv->dataN      = dataN;
  testEnv->gateDimN   = gateDimN;
  testEnv->expDimN    = expDimN;
  testEnv->skipN      = skipN;
  testEnv->skipX      = 0;
  
  testEnv->testN      = 0;
  testEnv->xentropy   = 0.0;
  testEnv->mseMean    = 0.0;
  testEnv->classError = 0.0;

  testEnv->dxentropy   = 0.0;
  testEnv->dmseMean    = 0.0;
  testEnv->dclassError = 0.0;

  testEnv->gateData  = (float **) malloc (sizeof(float *) * dataN);
  testEnv->expData   = (float **) malloc (sizeof(float *) * dataN);
  testEnv->target    = (int *)    malloc (sizeof(int)     * dataN);
  
  for (dataX=0; dataX<dataN; dataX++) {
    testEnv->gateData[dataX] = (float *) malloc (sizeof(float) * gateDimN);
    testEnv->expData[dataX]  = (float *) malloc (sizeof(float) * expDimN);
  }
  
  return TCL_OK;
}



/* --------------------------------------------------------------------- */
/*  hmeTestEnvCreate                                                     */
/* --------------------------------------------------------------------- */
HmeTestEnv  *hmeTestEnvCreate (int dataN, int gateDimN, int expDimN,
			       int skipN) {

  HmeTestEnv      *testEnv;
  
  testEnv = (HmeTestEnv *) malloc (sizeof(HmeTestEnv));
  hmeTestEnvInit(testEnv,dataN,gateDimN,expDimN,skipN);
  
  return testEnv;
}



/* --------------------------------------------------------------------- */
/*  hmeTestEnvDeinit                                                     */
/* --------------------------------------------------------------------- */
int  hmeTestEnvDeinit (HmeTestEnv *testEnv) {

  int        dataX;
  
  for (dataX=0; dataX<testEnv->dataN; dataX++) {
    free(testEnv->gateData[dataX]);
    free(testEnv->expData[dataX]);
  }
  free(testEnv->gateData);
  free(testEnv->expData);
  free(testEnv->target);

  return TCL_OK;
}



/* --------------------------------------------------------------------- */
/*  hmeTestEnvFree                                                       */
/* --------------------------------------------------------------------- */
int  hmeTestEnvFree (HmeTestEnv *testEnv) {

  if (!testEnv) return TCL_OK;
  hmeTestEnvDeinit(testEnv);
  free(testEnv);
  return TCL_OK;
}



/* --------------------------------------------------------------------- */
/*  hmeTestEnvExtract                                                    */
/* --------------------------------------------------------------------- */
int  hmeTestEnvExtract (HmeTestEnv *testEnv, float *gf, float *ef,
			int target) {

  register int            dimX;

  if (!testEnv) return TCL_ERROR;
  
  if (testEnv->skipX == testEnv->skipN) {
    if (testEnv->testN < testEnv->dataN) {
      if (testEnv->testN == testEnv->dataN-1)
        INFO("Reaching capacity of HmeTestEnv\n");
      for (dimX=0; dimX<testEnv->gateDimN; dimX++)
        testEnv->gateData[testEnv->testN][dimX] = gf[dimX];
      for (dimX=0; dimX<testEnv->expDimN; dimX++)
        testEnv->expData[testEnv->testN][dimX] = ef[dimX];
      testEnv->target[testEnv->testN] = target;
      testEnv->skipX = 0;
      testEnv->testN++;
    }
  } else {
    testEnv->skipX++;
  }
  
  return TCL_OK;
}



/* --------------------------------------------------------------------- */
/*  hmeSaveTestEnv                                                       */
/* --------------------------------------------------------------------- */
int  hmeSaveTestEnv (Hme *hme, FILE *fp) {

  HmeTestEnv          *testEnv;
  register int        testX,dimX;
  
  if (!hme->testEnv) return TCL_ERROR;
  testEnv = hme->testEnv;

  write_int(fp,TESTENV_MAGIC);
  write_int(fp,testEnv->dataN);
  write_int(fp,testEnv->testN);
  write_int(fp,testEnv->skipN);
  write_int(fp,testEnv->gateDimN);
  write_int(fp,testEnv->expDimN);

  for (testX=0; testX<testEnv->testN; testX++) {
    for (dimX=0; dimX<testEnv->gateDimN; dimX++)
      write_float(fp,testEnv->gateData[testX][dimX]);
    for (dimX=0; dimX<testEnv->expDimN; dimX++)
      write_float(fp,testEnv->expData[testX][dimX]);
    write_int(fp,testEnv->target[testX]);
  }
  
  return TCL_OK;
}



/* --------------------------------------------------------------------- */
/*  hmeLoadTestEnv                                                       */
/* --------------------------------------------------------------------- */
int  hmeLoadTestEnv (Hme *hme, FILE *fp) {

  HmeTestEnv        *testEnv;
  register int      dataN,testN,skipN,gateDimN,expDimN,testX,dimX;
  
  if (read_int(fp) != TESTENV_MAGIC) {
    ERROR("Couldn't find HmeTestEnv magic\n");
    return TCL_ERROR;
  }

  dataN    = read_int(fp);
  testN    = read_int(fp);
  skipN    = read_int(fp);
  gateDimN = read_int(fp);
  expDimN  = read_int(fp);

  if (!hme->testEnv)
    hme->testEnv = hmeTestEnvCreate(dataN,gateDimN,expDimN,skipN);
  else {
    hmeTestEnvDeinit(hme->testEnv);
    hmeTestEnvInit(hme->testEnv,dataN,gateDimN,expDimN,skipN);
  }
  testEnv = hme->testEnv;
  testEnv->testN = testN;
  
  for (testX=0; testX<testEnv->testN; testX++) {
    for (dimX=0; dimX<testEnv->gateDimN; dimX++)
      testEnv->gateData[testX][dimX] = read_float(fp);
    for (dimX=0; dimX<testEnv->expDimN; dimX++)
      testEnv->expData[testX][dimX] = read_float(fp);
    testEnv->target[testX] = read_int(fp);
  }
  
  return TCL_OK;
}



/* --------------------------------------------------------------------- */
/*  hmeClearHeader                                                       */
/* --------------------------------------------------------------------- */
int  hmeClearHeader (Hme *hme) {

  register int      classX;
  
  hme->trainN     = 0;
  hme->likelihood = 0.0;

  for (classX=0; classX<hme->classN; classX++)
    hme->count[classX] = 0.0;

  return TCL_OK;
}



/* --------------------------------------------------------------------- */
/*  hmeCreateTree                                                        */
/* --------------------------------------------------------------------- */
HmeNode  *hmeCreateTree (int classN, int depthN, int branchN,
   	      	         int gateDimN, int expDimN,
                         char *gateClass, char *gateOptions,
		         char *expClass, char *expOptions,
			 int depthX) {

  HmeNode            *hmeNode;
  register int       childX;
  
  if (depthX < depthN) {

    /* ---------------------- */
    /*  create a gating node  */
    /* ---------------------- */
    hmeNode = hmeNodeCreate (GATE,classN,gateDimN,branchN,
			     gateClass,gateOptions);

    /* ----------------------------------- */
    /*  recursively create children nodes  */
    /* ----------------------------------- */
    for (childX=0; childX<branchN; childX++)
      hmeNode->childPA[childX] = hmeCreateTree(classN,depthN,branchN,
					       gateDimN,expDimN,
					       gateClass,gateOptions,
					       expClass,expOptions,
					       depthX+1);
      
  } else {

    /* ----------------------- */
    /*  create an expert node  */
    /* ----------------------- */
    hmeNode = hmeNodeCreate (EXPERT,classN,expDimN,branchN,
			     expClass,expOptions);

  }
    
  return hmeNode;
}



/* --------------------------------------------------------------------- */
/*  hmeInit                                                              */
/* --------------------------------------------------------------------- */
int  hmeInit (Hme *hme) {

  register int             classX;

  /* ----------------------- */
  /*  init scalar variables  */
  /* ----------------------- */
  hme->xtrTest   = 0;           /* no test vector extraction */
  hme->evalPen   = 0;           /* no tree growing */
  hme->initLayer = 0;           /* no layer initialization */
  hme->pruneT    = 0.0;         /* no pruning */
  hme->frameX    = -1;

  /* --------------------- */
  /*  create prior arrays  */
  /* --------------------- */
  hme->count  = (double *) malloc (sizeof(double) * hme->classN);
  hme->prior  = (double *) malloc (sizeof(double) * hme->classN);
  
  /* ------------------------- */
  /*  init prior distribution  */
  /* ------------------------- */
  for (classX=0; classX<hme->classN; classX++) hme->prior[classX] = 0.0;

  /* ------------------------------ */
  /*  init default classifier info  */
  /* ------------------------------ */
  hme->defClass     = strdup(DEFAULT_CLASSIFIER);
  hme->defClassOpts = strdup(DEFAULT_CLASSIFIER_OPTIONS);
  hme->defChildN    = 0;
  hme->defPerturbe  = 0.01;
  
  /* ------------------------- */
  /*  init training variables  */
  /* ------------------------- */
  hmeClearHeader(hme);

  return TCL_OK;
}



/* ----------------------------------------------------------------------- */
/*  hmeDeinit                                                              */
/* ----------------------------------------------------------------------- */
int  hmeDeinit (Hme *hme) {

  free(hme->name);
  free(hme->count);
  free(hme->prior);
  if (hme->defClass) free(hme->defClass);
  if (hme->defClassOpts) free(hme->defClassOpts);
  if (hme->testEnv) hmeTestEnvFree(hme->testEnv);
  hmeNodeFree(hme->root);

  return TCL_OK;
}


  
/* --------------------------------------------------------------------- */
/*  hmeCreate                                                            */
/* --------------------------------------------------------------------- */
Hme  *hmeCreate (char *name, int classN, int depthN, int branchN,
		 int gateDimN, int expDimN,
                 char *gateClass, char *gateOptions,
		 char *expClass, char *expOptions) {

  Hme              *hme;
  
  /* -------------------------------------- */
  /*  check for errors / invalid arguments  */
  /* -------------------------------------- */
  if ((classN <= 0) || (depthN < 0) || (branchN <= 1) ||
      (gateDimN <= 0) || (expDimN <= 0)) {
    ERROR("one or more invalid scalar argument(s)\n");
    return NULL;
  }

  if (classGetByName(gateClass) == NULL) {
    ERROR("unknown gate classifier type '%s'\n",gateClass);
    return NULL;
  }

  if (classGetByName(expClass) == NULL) {
    ERROR("unknown expert classifier type '%s'\n",expClass);
    return NULL;
  }

  hme = (Hme *) malloc (sizeof(Hme));

  hme->name     = strdup(name);
  hme->useN     = 0;
  hme->classN   = classN;
  hme->gateDimN = gateDimN;
  hme->expDimN  = expDimN;
  
  /* ---------- */
  /*  init Hme  */
  /* ---------- */
  hmeInit(hme);
  hme->defChildN = branchN;
  hme->testEnv   = NULL;

  /* ----------------- */
  /*  create the tree  */
  /* ----------------- */
  hme->root = hmeCreateTree(classN,depthN,branchN,gateDimN,expDimN,
			    gateClass,gateOptions,expClass,expOptions,0);

  return hme;
}



/* --------------------------------------------------------------------- */
/*  hmeFree                                                              */
/* --------------------------------------------------------------------- */
int  hmeFree (Hme *hme) {

  if (!hme) return TCL_OK;

  hmeDeinit(hme);
  free(hme);
  
  return TCL_OK;
}



/* ----------------------------------------------------------------------- */
/*  hmeNodeSave                                                            */
/* ----------------------------------------------------------------------- */
int  hmeNodeSave (HmeNode *hmeNode, FILE *fp) {

  register int     childX;
  
  /* ----------------------------- */
  /*  save basic scalar variables  */
  /* ----------------------------- */
  write_int(fp,(int) hmeNode->nodeType);
  write_int(fp,hmeNode->classN);
  write_int(fp,hmeNode->dimN);
  write_int(fp,hmeNode->childN);
  
  /* ------------------------------------------ */
  /*  save classifier info and classifier data  */
  /* ------------------------------------------ */
  write_int(fp,classIndex(hmeNode->classInfo->name));
  if (hmeNode->classInfo->saveFct(hmeNode->classData,fp) == TCL_ERROR)
    return TCL_ERROR;

  /* --------------------------------------------- */
  /*  in case of a gate node, save children nodes  */
  /* --------------------------------------------- */
  if (hmeNode->nodeType == GATE)
    for (childX=0; childX<hmeNode->childN; childX++)
      if (hmeNodeSave(hmeNode->childPA[childX],fp) == TCL_ERROR)
	return TCL_ERROR;

  return TCL_OK;
}



/* ----------------------------------------------------------------------- */
/*  hmeSave                                                                */
/* ----------------------------------------------------------------------- */
int  hmeSave  (Hme *hme, FILE *fp) {

  register int      classX;
  
  write_int(fp,HME_MAGIC);
  write_string(fp,hme->name);
  write_int(fp,hme->classN);
  write_int(fp,hme->gateDimN);
  write_int(fp,hme->expDimN);
  write_int(fp,hme->defChildN);
  
  for (classX=0; classX<hme->classN; classX++)
    write_float(fp,(float) hme->prior[classX]);
  
  return  hmeNodeSave(hme->root,fp);
}



/* ----------------------------------------------------------------------- */
/*  hmeNodeLoad                                                            */
/* ----------------------------------------------------------------------- */
HmeNode  *hmeNodeLoad (FILE *fp) {

  HmeNode             *hmeNode;
  register int        childX,classX;
  
  /* ------------------- */
  /*  create a new node  */
  /* ------------------- */
  hmeNode = (HmeNode *) malloc (sizeof(HmeNode));
  
  /* ----------------------------- */
  /*  load basic scalar variables  */
  /* ----------------------------- */
  hmeNode->nodeType = (NodeType) read_int(fp);
  hmeNode->classN   = read_int(fp);
  hmeNode->dimN     = read_int(fp);
  hmeNode->childN   = read_int(fp);

  /* -------------------------------- */
  /*  create output activation array  */
  /* -------------------------------- */
  hmeNode->mu = (double *) malloc (sizeof(double) * hmeNode->classN);
  for (classX=0; classX<hmeNode->classN; classX++)
    hmeNode->mu[classX] = 0.0;
  
  /* ------------------------------------------ */
  /*  read classifier info and classifier data  */
  /* ------------------------------------------ */
  if ((hmeNode->classInfo = classGetByIdx(read_int(fp))) == NULL) {
    ERROR("could not resolve classifier type index\n");
    return NULL;
  }
  
  if ((hmeNode->classData = hmeNode->classInfo->loadFct(fp)) == NULL)
    return NULL;

  /* --------------------------------------------- */
  /*  in case of a gate node, load children nodes  */
  /* --------------------------------------------- */
  if (hmeNode->nodeType == GATE) {

    /* ---------------------------------------- */
    /*  create gating and posterior prob arrays */
    /* ---------------------------------------- */
    hmeNode->g = (double *) malloc (sizeof(double) * hmeNode->childN);
    for (childX=0; childX<hmeNode->childN; childX++) hmeNode->g[childX] = 0.0;
    hmeNode->h = (double *) malloc (sizeof(double) * hmeNode->childN);
    for (childX=0; childX<hmeNode->childN; childX++) hmeNode->h[childX] = 0.0;

    hmeNode->childPA = (HmeNode **) malloc (sizeof(HmeNode *) * hmeNode->childN);
    for (childX=0; childX<hmeNode->childN; childX++)
      if ((hmeNode->childPA[childX] = hmeNodeLoad(fp)) == NULL)
	return NULL;
  }
  
  return hmeNode;
}



/* ----------------------------------------------------------------------- */
/*  hmeLoad                                                                */
/* ----------------------------------------------------------------------- */
Hme  *hmeLoad  (Hme *hme, FILE *fp) {

  Hme                *newHme;
  register int       classX;
  char               l_name[255];
  
  /* ----------------- */
  /*  read file magic  */
  /* ----------------- */
  if (read_int(fp) != HME_MAGIC) {
    ERROR("did not find Hme magic\n");
    return hme;
  }
  
  /* ------------------------------------- */
  /*  free body of existing Hme structure  */
  /* ------------------------------------- */
  if (hme) {
    hmeDeinit(hme);
    newHme = hme;
  } else
    newHme = (Hme *) malloc (sizeof(Hme));
  
  /* ------------------------ */
  /*  read new Hme from file  */
  /* ------------------------ */
  read_string(fp,l_name);
  newHme->name      = strdup(l_name);
  newHme->useN      = 0;
  newHme->classN    = read_int(fp);
  newHme->gateDimN  = read_int(fp);
  newHme->expDimN   = read_int(fp);
  newHme->defChildN = read_int(fp);

  /* -------------- */
  /*  init new Hme  */
  /* -------------- */
  hmeInit(newHme);

  /* ------------------- */
  /*  read class priors  */
  /* ------------------- */
  for (classX=0; classX<newHme->classN; classX++)
    newHme->prior[classX] = (double) read_float(fp);

  newHme->testEnv = NULL;
  
  /* -------------------------- */
  /*  read and create Hme tree  */
  /* -------------------------- */
  if ((newHme->root = hmeNodeLoad(fp)) == NULL) return NULL;

  return newHme;
}



/* ----------------------------------------------------------------------- */
/*  hmeNodeReset                                                           */
/* ----------------------------------------------------------------------- */
int  hmeNodeReset (HmeNode *hmeNode) {

  register int           childX;
  
  /* -------------------------------------- */
  /*  init parameters in node's classifier  */
  /* -------------------------------------- */
  hmeNode->classInfo->initFct(hmeNode->classData);

  /* ----------------------------------------- */
  /*  recursively init parameters in sub-tree  */
  /* ----------------------------------------- */
  if (hmeNode->nodeType == GATE)
    for (childX=0; childX<hmeNode->childN; childX++)
      hmeNodeReset(hmeNode->childPA[childX]);
  
  return TCL_OK;
}



/* ----------------------------------------------------------------------- */
/*  hmeReset                                                                */
/* ----------------------------------------------------------------------- */
int  hmeReset (Hme *hme) {

  return  hmeNodeReset(hme->root);
}



/* ----------------------------------------------------------------------- */
/*  hmeConfHme                                                             */
/* ----------------------------------------------------------------------- */
int  hmeConfHme (Hme *hme, int argc, char *argv[]) {

  register int      k;
  char              *var,*val;

  if ((argc % 2) != 1) {
    ERROR("invalid number of parameters\n");
    return TCL_ERROR;
  }
  
  for (k=1; k<argc; k+=2) {
    var = argv[k]+1;
    val = argv[k+1];
    if (hmeConfigureItf((ClientData) hme,var,val) == TCL_ERROR)
      return TCL_ERROR;
  }
  
  return TCL_OK;
}



/* ----------------------------------------------------------------------- */
/*  hmeNodeConfGates                                                       */
/* ----------------------------------------------------------------------- */
int  hmeNodeConfGates (HmeNode *hmeNode, int argc, char *argv[]) {

  register int           childX;

  /* ----------------------------------------- */
  /*  recursively init parameters in sub-tree  */
  /* ----------------------------------------- */
  if (hmeNode->nodeType == GATE) {

    hmeNode->classInfo->configureFct(hmeNode->classData,argc,argv);
    
    for (childX=0; childX<hmeNode->childN; childX++)
      hmeNodeConfGates(hmeNode->childPA[childX],argc,argv);

  }
  
  return TCL_OK;
}


  
/* ----------------------------------------------------------------------- */
/*  hmeConfGates                                                           */
/* ----------------------------------------------------------------------- */
int  hmeConfGates (Hme *hme, int argc, char *argv[]) {

  return  hmeNodeConfGates(hme->root,argc,argv);
}



/* ----------------------------------------------------------------------- */
/*  hmeNodeConfExperts                                                     */
/* ----------------------------------------------------------------------- */
int  hmeNodeConfExperts (HmeNode *hmeNode, int argc, char *argv[]) {

  register int           childX;
  
  /* ----------------------------------------- */
  /*  recursively init parameters in sub-tree  */
  /* ----------------------------------------- */
  if (hmeNode->nodeType == GATE)

    for (childX=0; childX<hmeNode->childN; childX++)
      hmeNodeConfExperts(hmeNode->childPA[childX],argc,argv);

  else

    hmeNode->classInfo->configureFct(hmeNode->classData,argc,argv);
  
  return TCL_OK;
}


  
/* ----------------------------------------------------------------------- */
/*  hmeConfExperts                                                         */
/* ----------------------------------------------------------------------- */
int  hmeConfExperts (Hme *hme, int argc, char *argv[]) {

  return  hmeNodeConfExperts(hme->root,argc,argv);
}



/* ----------------------------------------------------------------------- */
/*  hmeNodeScore - recursive forward activation pass for an Hme subtree    */
/* ----------------------------------------------------------------------- */
int  hmeNodeScore (HmeNode *hmeNode, float *gatePat, float *expPat,
		   double gcum, double pruneT) {

  register int            childX,classX,activeNodeN,maxIdx;
  register double         g,childProb,maxProb;
  double                  *childMu;
  
  if (hmeNode->nodeType == GATE) {

    /* ------------------------------------------------ */
    /*  compute output activations for gate classifier  */
    /* ------------------------------------------------ */
    hmeNode->classInfo->scoreFct(hmeNode->classData,gatePat,hmeNode->dimN,
                                 hmeNode->g,hmeNode->childN);

    /* --------------------- */
    /*  reset gate mu array  */
    /* --------------------- */
    for (classX=0; classX<hmeNode->classN; classX++)
      hmeNode->mu[classX] = 0.0;
    
    /* --------------------------------------------- */
    /*  compute forward pass for all children nodes  */
    /* --------------------------------------------- */
    activeNodeN = 0;
    maxProb = 0.0;
    maxIdx  = 0;
    for (childX=0; childX<hmeNode->childN; childX++) {

      /* ---------------------------------------------------- */
      /*  compute child probability according to gating prob  */
      /* ---------------------------------------------------- */
      g = hmeNode->g[childX];
      childProb = gcum*g;
      if (childProb > maxProb) {
	maxProb = childProb;
	maxIdx  = childX;
      }
      
      /* ----------------------------- */
      /*  check for pruning threshold  */
      /* ----------------------------- */
      if (childProb >= pruneT) {

	/* ---------------------------------- */
	/*  compute scores for children node  */
	/* ---------------------------------- */
	hmeNodeScore(hmeNode->childPA[childX],gatePat,expPat,childProb,pruneT);
        activeNodeN++;
	
        /* ---------------------------------------- */
        /*  accumulate weighted sum of activations  */
        /* ---------------------------------------- */
        childMu = hmeNode->childPA[childX]->mu;
        for (classX=0; classX<hmeNode->classN; classX++)
  	  hmeNode->mu[classX] += (g * childMu[classX]);
      }
    }

    /* ------------------------------------------------------------------ */
    /*  if all children fell below threshold, compute at least the top-1  */
    /* ------------------------------------------------------------------ */
    if (activeNodeN == 0) { 
      g = hmeNode->g[maxIdx];
      childProb = g * gcum;
      hmeNodeScore(hmeNode->childPA[maxIdx],gatePat,expPat,childProb,pruneT);
      childMu = hmeNode->childPA[maxIdx]->mu;
      for (classX=0; classX<hmeNode->classN; classX++)
        hmeNode->mu[classX] = (g * childMu[classX]);
    }
    
  } else {

    /* -------------------------------------------------- */
    /*  compute output activations for expert classifier  */
    /* -------------------------------------------------- */
    hmeNode->classInfo->scoreFct(hmeNode->classData,expPat,hmeNode->dimN,
				 hmeNode->mu,hmeNode->classN);
  }
  
  return TCL_OK;
}



/* ----------------------------------------------------------------------- */
/*  hmeScore                                                               */
/* ----------------------------------------------------------------------- */
double  hmeScore (Hme *hme, float *gatePat, float *expPat,
		  int frameX, int classX, int divPrior) {

  register double           score,act;

  /* ---------------------------------------------- */
  /*  check if new activations have to be computed  */
  /* ---------------------------------------------- */
  if (frameX != hme->frameX) {

    /* -------------------------------------------------- */
    /*  compute new output activations for current frame  */
    /* -------------------------------------------------- */
    hmeNodeScore(hme->root,gatePat,expPat,1.0,hme->pruneT);
    hme->frameX = frameX;
  }

  /* ----------------------- */
  /*  compute logprob score  */
  /* ----------------------- */
  act = hme->root->mu[classX];
  if (act < MINVAL) act = MINVAL;
  score = -log(act);
  if (divPrior) score += hme->prior[classX];
  
  return  score;
}



/* ----------------------------------------------------------------------- */
/*  hmeNodeCreateAccus                                                     */
/* ----------------------------------------------------------------------- */
int  hmeNodeCreateAccus (HmeNode *hmeNode) {

  register int         childX;

  /* ----------------------------------- */
  /*  create accus in node's classifier  */
  /* ----------------------------------- */
  hmeNode->classInfo->createAccusFct(hmeNode->classData);

  /* -------------------------------------- */
  /*  recursively create accus in sub-tree  */
  /* -------------------------------------- */
  if (hmeNode->nodeType == GATE)
    for (childX=0; childX<hmeNode->childN; childX++)
      hmeNodeCreateAccus(hmeNode->childPA[childX]);

  return TCL_OK;
}



/* ----------------------------------------------------------------------- */
/*  hmeCreateAccus                                                         */
/* ----------------------------------------------------------------------- */
int  hmeCreateAccus (Hme *hme) {

  /* seems like there is nothing to do on the Hme level here */
  
  return  hmeNodeCreateAccus(hme->root);
}



/* ----------------------------------------------------------------------- */
/*  hmeNodeFreeAccus                                                       */
/* ----------------------------------------------------------------------- */
int  hmeNodeFreeAccus (HmeNode *hmeNode) {

  register int         childX;

  /* --------------------------------- */
  /*  free accus in node's classifier  */
  /* --------------------------------- */
  hmeNode->classInfo->freeAccusFct(hmeNode->classData);

  /* ------------------------------------ */
  /*  recursively free accus in sub-tree  */
  /* ------------------------------------ */
  if (hmeNode->nodeType == GATE)
    for (childX=0; childX<hmeNode->childN; childX++)
      hmeNodeFreeAccus(hmeNode->childPA[childX]);

  return TCL_OK;
}



/* ----------------------------------------------------------------------- */
/*  hmeFreeAccus                                                           */
/* ----------------------------------------------------------------------- */
int  hmeFreeAccus (Hme *hme) {

  /* seems like there is nothing to do on the Hme level here */
  
  return  hmeNodeFreeAccus(hme->root);
}



/* ----------------------------------------------------------------------- */
/*  hmeNodeClearAccus                                                      */
/* ----------------------------------------------------------------------- */
int  hmeNodeClearAccus (HmeNode *hmeNode) {

  register int         childX;

  /* ---------------------------------- */
  /*  clear accus in node's classifier  */
  /* ---------------------------------- */
  hmeNode->classInfo->clearAccusFct(hmeNode->classData);

  hmeNode->pen = 0.0;   /* reset penalty accu */
  
  /* ------------------------------------- */
  /*  recursively clear accus in sub-tree  */
  /* ------------------------------------- */
  if (hmeNode->nodeType == GATE)
    for (childX=0; childX<hmeNode->childN; childX++)
      hmeNodeClearAccus(hmeNode->childPA[childX]);

  
  return TCL_OK;
}



/* ----------------------------------------------------------------------- */
/*  hmeClearAccus                                                          */
/* ----------------------------------------------------------------------- */
int  hmeClearAccus (Hme *hme) {

  hmeClearHeader(hme);
  
  return  hmeNodeClearAccus(hme->root);
}



/* ----------------------------------------------------------------------- */
/*  hmeNodeSaveAccus                                                       */
/* ----------------------------------------------------------------------- */
int  hmeNodeSaveAccus (HmeNode *hmeNode, FILE *fp) {

  register int         childX;

  /* --------------------------------- */
  /*  save accus in node's classifier  */
  /* --------------------------------- */
  hmeNode->classInfo->saveAccusFct(hmeNode->classData,fp);

  /* ------------------- */
  /*  save penalty accu  */
  /* ------------------- */
  write_float(fp,(float) hmeNode->pen);
  
  /* ------------------------------------ */
  /*  recursively save accus in sub-tree  */
  /* ------------------------------------ */
  if (hmeNode->nodeType == GATE)
    for (childX=0; childX<hmeNode->childN; childX++)
      hmeNodeSaveAccus(hmeNode->childPA[childX],fp);

  return TCL_OK;
}



/* ----------------------------------------------------------------------- */
/*  hmeSaveAccus                                                           */
/* ----------------------------------------------------------------------- */
int  hmeSaveAccus (Hme *hme, FILE *fp) {

  register int           classX;
  
  write_int(fp,HME_ACCUS_MAGIC);
  write_int(fp,hme->trainN);
  write_float(fp,(float) hme->likelihood);

  for (classX=0; classX<hme->classN; classX++)
    write_float(fp,(float) hme->count[classX]);
  
  return  hmeNodeSaveAccus(hme->root,fp);
}



/* ----------------------------------------------------------------------- */
/*  hmeNodeLoadAccus                                                       */
/* ----------------------------------------------------------------------- */
int  hmeNodeLoadAccus (HmeNode *hmeNode, FILE *fp) {

  register int         childX;

  /* --------------------------------- */
  /*  load accus in node's classifier  */
  /* --------------------------------- */
  hmeNode->classInfo->loadAccusFct(hmeNode->classData,fp);

  /* ------------------- */
  /*  load penalty accu  */
  /* ------------------- */
  hmeNode->pen += (double) read_float(fp);
  
  /* ------------------------------------ */
  /*  recursively load accus in sub-tree  */
  /* ------------------------------------ */
  if (hmeNode->nodeType == GATE)
    for (childX=0; childX<hmeNode->childN; childX++)
      hmeNodeLoadAccus(hmeNode->childPA[childX],fp);
  
  return TCL_OK;
}



/* ----------------------------------------------------------------------- */
/*  hmeLoadAccus -- this routine does not just load but also accumulate!   */
/* ----------------------------------------------------------------------- */
int  hmeLoadAccus (Hme *hme, FILE *fp) {

  register int      classX;
  
  if (read_int(fp) != HME_ACCUS_MAGIC) {
    ERROR("did not find Hme accus magic\n");
    return TCL_ERROR;
  }

  hme->trainN     += read_int(fp);
  hme->likelihood += (double) read_float(fp);

  for (classX=0; classX<hme->classN; classX++)
    hme->count[classX] += (double) read_float(fp);
       
  return  hmeNodeLoadAccus(hme->root,fp);
}



/* ----------------------------------------------------------------------- */
/*  hmeNodePosterior                                                       */
/* ----------------------------------------------------------------------- */
int  hmeNodePosterior (HmeNode *hmeNode, float *gatePat, float *expPat,
		       int targetX, double gcum, double pruneT) {

  register int         maxIdx,activeNodeN,childX;
  register double      maxProb,g,childProb,newProb,factor,probSum;
  
  if (hmeNode->nodeType == GATE) {

    /* -------------------------- */
    /*  compute gate activations  */
    /* -------------------------- */
    hmeNode->classInfo->scoreFct(hmeNode->classData,gatePat,hmeNode->dimN,
                                 hmeNode->g,hmeNode->childN);

    probSum       = 0.0;
    maxProb       = 0.0;
    maxIdx        = 0;
    activeNodeN   = 0;

    /* -------------------------------------------- */
    /*  recursively call this routine for sub-tree  */
    /* -------------------------------------------- */
    for (childX=0; childX<hmeNode->childN; childX++) {

      /* ----------------------------- */
      /*  compute child's probability  */
      /* ----------------------------- */
      g = hmeNode->g[childX];
      childProb = g * gcum;
      if (childProb > maxProb) {
	maxProb = childProb;
	maxIdx  = childX;
      }

      /* ----------------------------- */
      /*  check for pruning threshold  */
      /* ----------------------------- */
      if (childProb >= pruneT) {
	activeNodeN++;
        hmeNodePosterior(hmeNode->childPA[childX],gatePat,expPat,targetX,
	  	         childProb,pruneT);

        /* ---------------------------------- */
        /*  accumulate posterior information  */
        /* ---------------------------------- */
        newProb = g * hmeNode->childPA[childX]->prob;
        hmeNode->h[childX] = newProb;
        probSum += newProb;
      } else
	hmeNode->h[childX] = 0.0;
    }

    /* ------------------------------------------------------------------- */
    /*  in case all children nodes fell below threshold, backoff to top-1  */
    /* ------------------------------------------------------------------- */
    if (activeNodeN == 0) {
      g = hmeNode->g[maxIdx];
      childProb = g * gcum;
      hmeNodePosterior(hmeNode->childPA[maxIdx],gatePat,expPat,targetX,
	               childProb,pruneT);
      newProb = g * hmeNode->childPA[maxIdx]->prob;
      hmeNode->h[childX] = newProb;
      probSum = newProb;
    }
    
    /* -------------------- */
    /*  compute posteriors  */
    /* -------------------- */
    if (probSum > 0.0)
      factor = 1.0 / probSum;
    else {
      WARN("floating point underflow: denominator for posteriors too small\n");
      factor = 1.0 / MINVAL;
    }
    
    for (childX=0; childX<hmeNode->childN; childX++)
      if (hmeNode->h[childX] > 0.0)
        hmeNode->h[childX] *= factor;

    hmeNode->prob = probSum;       /* likelihood for this node */
    
  } else {

    /* --------------------------------------------------- */
    /*  compute expert activations and expert probability  */
    /* --------------------------------------------------- */
    hmeNode->classInfo->scoreFct(hmeNode->classData,expPat,hmeNode->dimN,
				 hmeNode->mu,hmeNode->classN);
    hmeNode->prob = hmeNode->mu[targetX];
  }

  return TCL_OK;
}



/* ----------------------------------------------------------------------- */
/*  hmePosterior                                                           */
/* ----------------------------------------------------------------------- */
int  hmePosterior (Hme *hme, float *gatePat, float *expPat, int targetX) {

  return  hmeNodePosterior(hme->root,gatePat,expPat,targetX,1.0,hme->pruneT);
}



/* ----------------------------------------------------------------------- */
/*  hmeNodeEvalPen                                                         */
/* ----------------------------------------------------------------------- */
int  hmeNodeEvalPen (HmeNode *hmeNode, double penalty, double gcum) {

  register int           childX;
  register double        g;
  
  if (hmeNode->nodeType == GATE) {
    for (childX=0; childX<hmeNode->childN; childX++) {
      g = hmeNode->g[childX];
      if (hmeNode->h[childX] > 0.0)
        hmeNodeEvalPen(hmeNode->childPA[childX],penalty,g*gcum);
    }
  } else

    hmeNode->pen += (gcum*penalty);

  return TCL_OK;
}



/* ----------------------------------------------------------------------- */
/*  hmeNodeEvalPartial                                                     */
/* ----------------------------------------------------------------------- */
int  hmeNodeEvalPartial (HmeNode *hmeNode, float *gatePat, float *expPat,
			 int depthX, int initN) {

  register int           childX;

  if (depthX == initN) return TCL_OK;
  
  if (hmeNode->nodeType == GATE) {
    hmeNode->classInfo->scoreFct(hmeNode->classData,gatePat,hmeNode->dimN,
				 hmeNode->g,hmeNode->childN);    
    for (childX=0; childX<hmeNode->childN; childX++)
      hmeNodeEvalPartial(hmeNode->childPA[childX],gatePat,expPat,
		         depthX+1,initN);
  } else
    hmeNode->classInfo->scoreFct(hmeNode->classData,expPat,hmeNode->dimN,
				 hmeNode->mu,hmeNode->classN);

  return TCL_OK;
}



/* ----------------------------------------------------------------------- */
/*  hmeNodeAccuPartial                                                     */
/* ----------------------------------------------------------------------- */
int  hmeNodeAccuPartial (HmeNode *hmeNode, float *gatePat, float *expPat,
			 int classX, int depthX, int initN, double gcum) {

  register int           childX;
  register double        g;

  if (depthX == initN) {
    if (hmeNode->nodeType == GATE)
      hmeNode->classInfo->accuSoftFct(hmeNode->classData,gatePat,
		hmeNode->dimN,hmeNode->g,NULL,hmeNode->childN,gcum,gcum);
    else
      hmeNode->classInfo->accuHardFct(hmeNode->classData,expPat,
	        hmeNode->dimN,classX,NULL,hmeNode->classN,gcum,gcum);
    return TCL_OK;
  }
  
  if (hmeNode->nodeType == GATE) {
    for (childX=0; childX<hmeNode->childN; childX++) {
      g = hmeNode->g[childX];
      hmeNodeAccuPartial(hmeNode->childPA[childX],gatePat,expPat,
		         classX,depthX+1,initN,g*gcum);
    }
  }
  return TCL_OK;
}



/* ----------------------------------------------------------------------- */
/*  hmeNodeAccu                                                            */
/* ----------------------------------------------------------------------- */
int  hmeNodeAccu (HmeNode *hmeNode,float *gatePat, float *expPat,
		  int classX, double gcum, double hcum) {

  register double     g,h;
  register int        childX;

  hmeNode->hcum = hcum;

  if (hmeNode->nodeType == GATE) {
    hmeNode->classInfo->accuSoftFct(hmeNode->classData,gatePat,hmeNode->dimN,
				    hmeNode->h,hmeNode->g,
				    hmeNode->childN,gcum,hcum);
    for (childX=0; childX<hmeNode->childN; childX++) {
      g = hmeNode->g[childX];
      h = hmeNode->h[childX];
      if (h > 0.0)    /* if not pruned by preceeding posterior pass */
        hmeNodeAccu(hmeNode->childPA[childX],gatePat,expPat,classX,g*gcum,h*hcum);
    }
  } else {
    hmeNode->classInfo->accuHardFct(hmeNode->classData,expPat,hmeNode->dimN,
				    classX,hmeNode->mu,
				    hmeNode->classN,gcum,hcum);
  }
  
  return TCL_OK;
}



/* ----------------------------------------------------------------------- */
/*  hmeAccu                                                                */
/* ----------------------------------------------------------------------- */
int  hmeAccu (Hme *hme, float *gatePat, float *expPat, int classX) {

  register double         lh,penalty;

  /* ---------------------- */
  /*  extract test vectors  */
  /* ---------------------- */
  if (hme->xtrTest) {
    if (hme->testEnv) hmeTestEnvExtract(hme->testEnv,gatePat,expPat,classX);
    return TCL_OK;
  }

  /* -------------------------------------------- */
  /*  initialization of top-level gating network  */
  /* -------------------------------------------- */
  if (hme->initLayer) {
    hmeNodeEvalPartial(hme->root,gatePat,expPat,1,hme->initLayer);
    hmeNodeAccuPartial(hme->root,gatePat,expPat,classX,1,hme->initLayer,1.0);
    hme->trainN++;
    hme->count[classX] += 1.0;
    return TCL_OK;
  }

  /* ------------------------------------------- */
  /*  evaluation of scaled likelihood penalties  */
  /* ------------------------------------------- */
  if (hme->evalPen) {
    hmeNodePosterior(hme->root,gatePat,expPat,classX,1.0,hme->pruneT);
    lh = hme->root->prob;
    if (lh < MINVAL) lh = MINVAL;
    penalty = -log(lh);
    hmeNodeEvalPen(hme->root,penalty,1.0);
    return TCL_OK;
  }
  
  /* ------------------------------- */
  /*  regular Hme accumulation pass  */
  /* ------------------------------- */
  hmeNodePosterior(hme->root,gatePat,expPat,classX,1.0,hme->pruneT);
  hmeNodeAccu(hme->root,gatePat,expPat,classX,1.0,1.0);
  
  hme->trainN++;
  hme->count[classX] += 1.0;
  lh = hme->root->prob;
  if (lh < MINVAL) lh = MINVAL;
  hme->likelihood += log(lh);
  
  return TCL_OK;
}



/* ----------------------------------------------------------------------- */
/*  hmeNodeFindWorst                                                       */
/* ----------------------------------------------------------------------- */
int  hmeNodeFindWorst (HmeNode *hmeNode, HmeNode **worstNode, double *maxPen) {

  register int               childX;
  
  if (hmeNode->nodeType == GATE) {
    for (childX=0; childX<hmeNode->childN; childX++)
        hmeNodeFindWorst(hmeNode->childPA[childX],worstNode,maxPen);

  } else {

    /* ------------------- */
    /*  check this expert  */
    /* ------------------- */
    if (hmeNode->pen > *maxPen) {
      *maxPen    = hmeNode->pen;
      *worstNode = hmeNode;
    }
  }
    
  return TCL_OK;
}



/* ----------------------------------------------------------------------- */
/*  hmeNodeUpdatePartial                                                   */
/* ----------------------------------------------------------------------- */
int  hmeNodeUpdatePartial (HmeNode *hmeNode, int depthX, int initN) {

  register int           childX;

  if (depthX == initN) {
    hmeNode->classInfo->updateFct(hmeNode->classData);
    return TCL_OK;
  }
  
  if (hmeNode->nodeType == GATE)
    for (childX=0; childX<hmeNode->childN; childX++)
      hmeNodeUpdatePartial(hmeNode->childPA[childX],depthX+1,initN);

  return TCL_OK;
}



/* ----------------------------------------------------------------------- */
/*  hmeNodeUpdate                                                          */
/* ----------------------------------------------------------------------- */
int  hmeNodeUpdate (HmeNode *hmeNode) {

  register int        childX;
  
  hmeNode->classInfo->updateFct(hmeNode->classData);

  if (hmeNode->nodeType == GATE)
    for (childX=0; childX<hmeNode->childN; childX++)
        hmeNodeUpdate(hmeNode->childPA[childX]);

  return TCL_OK;
}



/* ----------------------------------------------------------------------- */
/*  hmeUpdate                                                              */
/* ----------------------------------------------------------------------- */
int  hmeUpdate (Hme *hme) {

  HmeNode           *worstNode;
  register double   prior;
  double            maxPen;
  register int      classX;
  
  if ((hme->initLayer) && (hme->evalPen)) {
    ERROR("both initLayer and evalPen together make no sense\n");
    return TCL_ERROR;
  }

  /* -------------------------------------------- */
  /*  initialization of top-level gating network  */
  /* -------------------------------------------- */
  if (hme->initLayer) {
    hmeNodeUpdatePartial(hme->root,1,hme->initLayer);

    /* --------------------------- */
    /*  update prior distribution  */
    /* --------------------------- */
    for (classX=0; classX<hme->classN; classX++) {
      prior = hme->count[classX] / hme->trainN;
      if (prior > MINVAL)
	hme->prior[classX] = log(prior);
      else
	hme->prior[classX] = 0.0;
    }
    
    return TCL_OK;
  }

  /* ------------------------- */
  /*  maximum penalty growing  */
  /* ------------------------- */
  if (hme->evalPen) {
    
    /* ---------------------------------- */
    /*  find expert with maximum penalty  */
    /* ---------------------------------- */
    maxPen    = -MAXVAL;
    worstNode = NULL;
    hmeNodeFindWorst(hme->root,&worstNode,&maxPen);
    
    /* ------------- */
    /*  grow expert  */ 
    /* ------------- */
    hmeNodeGrow(worstNode,hme->defChildN,hme->gateDimN,
		hme->defClass,hme->defClassOpts,hme->defPerturbe);
    
    return TCL_OK;
  }

  /* ------------------------- */
  /*  regular training update  */
  /* ------------------------- */
  if (hme->trainN > 0) {

    /* ------------------------------ */
    /*  recursively update all nodes  */
    /* ------------------------------ */
    hmeNodeUpdate(hme->root);

    /* --------------------------- */
    /*  update prior distribution  */
    /* --------------------------- */
    for (classX=0; classX<hme->classN; classX++) {
      prior = hme->count[classX] / hme->trainN;
      if (prior > MINVAL)
	hme->prior[classX] = log(prior);
      else
	hme->prior[classX] = 0.0;
    }
  }
  
  return TCL_OK;
}



/* ----------------------------------------------------------------------- */
/*  hmeNodeShow                                                            */
/* ----------------------------------------------------------------------- */
int  hmeNodeShow (HmeNode *hmeNode, char *prefix, int last, int top) {

  char                 *newPrefix;
  register int         childX;
  
  if (hmeNode->nodeType == GATE) {

    if (!top) {
      printO("%s|\n",prefix);
      printO("%s+----Gate(%s)\n",prefix,hmeNode->classInfo->name);
    } else {
      printO("%s\n",prefix);
      printO("%s<----Gate(%s)\n",prefix,hmeNode->classInfo->name);
    }
    
    if (!last) {
      printO("%s|    |\n",prefix);
      newPrefix = strdup(prefix);
      strcat(newPrefix,"|    ");
    } else {
      printO("%s     |\n",prefix);
      newPrefix = strdup(prefix);
      strcat(newPrefix,"     ");
    }      

    for (childX=0; childX<hmeNode->childN; childX++)
        hmeNodeShow(hmeNode->childPA[childX],newPrefix,
		    childX==hmeNode->childN-1,0);

    free(newPrefix);
    
  } else {

    printO("%s+----Expert(%s)\n",prefix,hmeNode->classInfo->name);

  }
  
  return TCL_OK;
}



/* ----------------------------------------------------------------------- */
/*  hmeShow                                                                */
/* ----------------------------------------------------------------------- */
int  hmeShow (Hme  *hme) {

  hmeNodeShow(hme->root,"   ",1,1);
  printO("\n");
  return TCL_OK;
}



/* ----------------------------------------------------------------------- */
/*  hmeCreateTest                                                          */
/* ----------------------------------------------------------------------- */
int  hmeCreateTest (Hme *hme, int dataN, int skipN) {

  if (hme->testEnv) return TCL_OK;
  hme->testEnv = hmeTestEnvCreate(dataN,hme->gateDimN,hme->expDimN,skipN);

  return TCL_OK;
}



/* ----------------------------------------------------------------------- */
/*  hmeTest                                                                */
/* ----------------------------------------------------------------------- */
int  hmeTest (Hme *hme, int testN) {

  HmeTestEnv      *testEnv;
  int             n,testX,targetX,idx,classX;
  double          score,max,err,*mu;
  double          xentropyOld,mseMeanOld,classErrorOld;
  
  
  if (!hme->testEnv) {
    ERROR("No test environment allocated\n");
    return TCL_ERROR;
  }

  testEnv = hme->testEnv;
  n = testEnv->testN;
  if ((testN > 0) && (testN < testEnv->testN)) n = testN;
  
  if (n==0) {
    ERROR("No test vectors extracted\n");
    return TCL_ERROR;
  }

  /* ------------------------------------------ */
  /*  test the vectors in the test environment  */
  /* ------------------------------------------ */
  xentropyOld   = testEnv->xentropy;
  mseMeanOld    = testEnv->mseMean;
  classErrorOld = testEnv->classError;
  testEnv->xentropy   = 0.0;
  testEnv->mseMean    = 0.0;
  testEnv->classError = 0.0;
  for (testX=0; testX<n; testX++) {
    targetX = testEnv->target[testX];
    score = hmeScore(hme,testEnv->gateData[testX],testEnv->expData[testX],
	             hme->frameX+1,targetX,0);
    mu = hme->root->mu;

    /* ----------------- */
    /*  update xentropy  */
    /* ----------------- */
    if (mu[targetX] > 1E-6)
      testEnv->xentropy += log(mu[targetX]);
    else
      testEnv->xentropy += log(1E-6);

    /* --------------------------- */
    /*  update MSE and classError  */
    /* --------------------------- */
    max = 0.0;
    idx = 0;
    for (classX=0; classX<hme->classN; classX++) {
      if (classX != targetX)
        err = square(mu[classX]);
      else
	err = square(1.0 - mu[classX]);
      testEnv->mseMean   += err;

      if (mu[classX] > max) {
	max = mu[classX];
	idx = classX;
      }      
    }
    if (idx != targetX) testEnv->classError += 1.0;
  }

  /* -------------------------------------------- */
  /*  compute error measures over whole test set  */
  /* -------------------------------------------- */
  testEnv->xentropy   /= n;
  testEnv->mseMean    /= n;
  testEnv->classError /= n;
  
  INFO ("TESTRESULT:   XTPY=%8.5f  MSE=%8.5f  ERR=%8.5f\n",
	testEnv->xentropy,testEnv->mseMean,testEnv->classError);

  /* ---------------------------------------- */
  /*  compute delta's for all error measures  */
  /* ---------------------------------------- */
  if (xentropyOld != 0.0)
    testEnv->dxentropy = 1.0 - (testEnv->xentropy / xentropyOld);
  else
    testEnv->dxentropy = 1.0;

  if (mseMeanOld != 0.0)
    testEnv->dmseMean = (testEnv->mseMean / mseMeanOld) -1.0;
  else
    testEnv->dmseMean = -1.0;

  if (classErrorOld != 0.0)
    testEnv->dclassError = (testEnv->classError / classErrorOld) -1.0;
  else
    testEnv->dclassError = -1.0;

  return TCL_OK;
}




/* ======================================================================= */
/*                Itf/Tcl Interface Functions for HmeNode                  */
/* ======================================================================= */


/* ----------------------------------------------------------------------- */
/*  hmeNodePutsItf                                                         */
/* ----------------------------------------------------------------------- */
int  hmeNodePutsItf (ClientData cd, int argc, char *argv[]) {

  HmeNode             *hmeNode = (HmeNode *) cd;
  register int        classX,childX;
  
  if (hmeNode->nodeType == GATE)
    itfAppendResult("{nodeType GATE} ");
  else
    itfAppendResult("{nodeType EXPERT} ");

  itfAppendResult("{classifier %s} ",hmeNode->classInfo->name);

  itfAppendResult("{classN %d} {dimN %d} ",
		  hmeNode->classN,hmeNode->dimN);

  itfAppendResult("{hcum %f} {prob %f} ",hmeNode->hcum,hmeNode->prob);
  
  itfAppendResult("{mu { ");
  for (classX=0; classX<hmeNode->classN; classX++)
    itfAppendResult("%f ",hmeNode->mu[classX]);
  itfAppendResult("} } ");

  if (hmeNode->nodeType == GATE) {
    itfAppendResult("{childN %d} ",hmeNode->childN);
    itfAppendResult("{g { ");
    for (childX=0; childX<hmeNode->childN; childX++)
      itfAppendResult("%f ",hmeNode->g[childX]);
    itfAppendResult("} } ");

    itfAppendResult("{h { ");
    for (childX=0; childX<hmeNode->childN; childX++)
      itfAppendResult("%f ",hmeNode->h[childX]);
    itfAppendResult("} } ");
  } else
    itfAppendResult("{pen %f} ",hmeNode->pen);
  
  return TCL_OK;
}



/* ----------------------------------------------------------------------- */
/*  hmeNodeAccessItf                                                       */
/* ----------------------------------------------------------------------- */
ClientData  hmeNodeAccessItf (ClientData cd, char *name, TypeInfo **ti) {

  HmeNode        *hmeNode = (HmeNode *) cd;
  int            childX;
  
  if (*name=='.') {
    if (name[1]=='\0') {
      itfAppendElement("clf");
      if (hmeNode->nodeType == GATE)
	itfAppendElement("child(0..%d)",hmeNode->childN-1);
    }
    else { 
      if (!strcmp(name+1,"clf")) {
	*ti=itfGetType(hmeNode->classInfo->name);
	return hmeNode->classData;
      } else
      if ((hmeNode->nodeType == GATE) && (sscanf(name+1,"child(%d)",&childX)==1)) {
	*ti=itfGetType("HmeNode");
	if ((childX >=0) && (childX < hmeNode->childN))
	  return (ClientData)(hmeNode->childPA[childX]);
	else
	  return NULL;
      }	
    }
  }

  *ti=NULL;
  return NULL;
}



/* ----------------------------------------------------------------------- */
/*  hmeNodePlugItf                                                         */
/* ----------------------------------------------------------------------- */
int  hmeNodePlugItf (ClientData cd, int argc, char *argv[]) {

  HmeNode              *hmeNode = (HmeNode *) cd;
  ClientData           classifier;
  char                 *className;
  int                  ac;

  ac = (argc < 2)?(argc-1):1;
  if ( itfParseArgv( argv[0], &ac, argv+1, 1,
    "<classType>",  ARGV_STRING, NULL, &className,  NULL, "type of classifier",
  NULL) != TCL_OK) return TCL_ERROR;

  ac = argc-2;
  if ( itfParseArgv( argv[0], &ac, argv+2, 0,
    "<classifier>", ARGV_OBJECT, NULL, &classifier, className, "classifier object",
  NULL) != TCL_OK) return TCL_ERROR;

  return hmeNodePlug (hmeNode,className,classifier);
}



/* ----------------------------------------------------------------------- */
/*  hmeNodeGrowItf                                                         */
/* ----------------------------------------------------------------------- */
int  hmeNodeGrowItf (ClientData cd, int argc, char *argv[]) {

  HmeNode          *hmeNode = (HmeNode *) cd;
  int              ac       = argc-1;
  int              childN,gateDimN;
  char             *gateClass,*gateOpt;
  float            perturbe = 0.0;
  
  if ( itfParseArgv( argv[0], &ac, argv+1, 0,
    "<childN>",    ARGV_INT,    NULL, &childN,    NULL, "number of child nodes",
    "<gateDimN>",  ARGV_INT,    NULL, &gateDimN,  NULL, "gate feature dimension",
    "<gateClass>", ARGV_STRING, NULL, &gateClass, NULL, "gate classifier type",
    "<gateOpt>",   ARGV_STRING, NULL, &gateOpt,   NULL, "options for gate classifier",
    "-perturbe",   ARGV_FLOAT,  NULL, &perturbe,  NULL, "perturberation maximum",
  NULL) != TCL_OK) return TCL_ERROR;
  
  if (hmeNode->nodeType == GATE) {
    ERROR("can not grow GATE nodes\n");
    return TCL_ERROR;
  }

  if (childN <= 1) {
    ERROR("illegal number of children (%d)\n",childN);
    return TCL_ERROR;
  }
  
  return  hmeNodeGrow (hmeNode,childN,gateDimN,gateClass,gateOpt,perturbe);
}



/* ----------------------------------------------------------------------- */
/*  hmeNodeExpandItf                                                       */
/* ----------------------------------------------------------------------- */
int  hmeNodeExpandItf (ClientData cd, int argc, char *argv[]) {

  HmeNode          *hmeNode = (HmeNode *) cd;
  int              ac       = argc-1;
  char             *expClass,*expOpt;

  if ( itfParseArgv( argv[0], &ac, argv+1, 0,
    "<expClass>",  ARGV_STRING, NULL, &expClass,  NULL, "expert classifier type",
    "<expOpt>",    ARGV_STRING, NULL, &expOpt,    NULL, "options for expert classifier",
  NULL) != TCL_OK) return TCL_ERROR;
  
  if (hmeNode->nodeType == GATE) {
    ERROR("can not grow GATE nodes\n");
    return TCL_ERROR;
  }
  
  return  hmeNodeExpand (hmeNode,expClass,expOpt);
}



/* ----------------------------------------------------------------------- */
/*  hmeNodeHmeItf                                                          */
/* ----------------------------------------------------------------------- */
int  hmeNodeHmeItf (ClientData cd, int argc, char *argv[]) {

  HmeNode          *hmeNode = (HmeNode *) cd;
  int              ac       = argc-1;
  Hme              *hme;
  
  if ( itfParseArgv( argv[0], &ac, argv+1, 0,
    "<Hme>",  ARGV_OBJECT, NULL, &hme,  "Hme", "Hme to plug in",
  NULL) != TCL_OK) return TCL_ERROR;
  
  if (hmeNode->nodeType == GATE) {
    ERROR("can not plug Hme's into GATE nodes\n");
    return TCL_ERROR;
  }

  return hmeNodeHme(hmeNode,hme);
}




/* ======================================================================= */
/*               Itf/Tcl Interface Functions for HmeTestEnv                */
/* ======================================================================= */


/* ----------------------------------------------------------------------- */
/*  hmeTestEnvPutsItf                                                      */
/* ----------------------------------------------------------------------- */
int  hmeTestEnvPutsItf (ClientData cd, int argc, char *argv[]) {

  HmeTestEnv       *testEnv = (HmeTestEnv *) cd;
  int              dimX;
  int              testX    = -1;
  int              ac       = argc-1;

  if ( itfParseArgv( argv[0], &ac, argv+1, 0,
    "-testX",  ARGV_INT, NULL, &testX,  NULL, "index of a test vector",
  NULL) != TCL_OK) return TCL_ERROR;

  if ((testX < 0) || (testX >= testEnv->testN)) {
    itfAppendResult("{dataN %d} {testN %d} {skipN %d} ",
		    testEnv->dataN,testEnv->testN,testEnv->skipN);
    itfAppendResult("{xentropy %8.5f} {dxentropy %8.5f} ",
		    testEnv->xentropy,testEnv->dxentropy);
    itfAppendResult("{mseMean %8.5f} {dmseMean %8.5f} ",
		    testEnv->mseMean,testEnv->dmseMean);
    itfAppendResult("{classError %8.5f} {dclassError %8.5f} ",
		    testEnv->classError,testEnv->dclassError);
  } else {
    itfAppendResult("{ ");
    for (dimX=0; dimX<testEnv->gateDimN; dimX++)
      itfAppendResult("%f ",testEnv->gateData[testX][dimX]);
    itfAppendResult("} { ");
    for (dimX=0; dimX<testEnv->expDimN; dimX++)
      itfAppendResult("%f ",testEnv->expData[testX][dimX]);
    itfAppendResult("} %d ",testEnv->target[testX]);    
  }
  
  return TCL_OK;
}




/* ======================================================================= */
/*                 Itf/Tcl Interface Functions for Hme                     */
/* ======================================================================= */


/* ----------------------------------------------------------------------- */
/*  hmeCreateItf                                                           */
/* ----------------------------------------------------------------------- */
ClientData  hmeCreateItf (ClientData cd, int argc, char *argv[]) {

  int            ac = argc-1;
  int            classN,depthN,branchN,gateDimN,expDimN;
  char           *gateClass = DEFAULT_CLASSIFIER;
  char           *gateOpt   = DEFAULT_CLASSIFIER_OPTIONS;
  char           *expClass  = DEFAULT_CLASSIFIER;
  char           *expOpt    = DEFAULT_CLASSIFIER_OPTIONS;

  if ( itfParseArgv( argv[0], &ac, argv+1, 0,
      "<classN>",    ARGV_INT,    NULL, &classN,    NULL, "number of output classes",
      "<depthN>",    ARGV_INT,    NULL, &depthN,    NULL, "depth of Hme tree",
      "<branchN>",   ARGV_INT,    NULL, &branchN,   NULL, "branching factor",
      "<gateDimN>",  ARGV_INT,    NULL, &gateDimN,  NULL, "gate feature dimension",
      "<expDimN>",   ARGV_INT,    NULL, &expDimN,   NULL, "expert feature dimension",
      "-gateClass",  ARGV_STRING, NULL, &gateClass, NULL, "gate classifier type",
      "-gateOpt",    ARGV_STRING, NULL, &gateOpt,   NULL, "options for gate classifier",
      "-expClass",   ARGV_STRING, NULL, &expClass,  NULL, "expert classifier type",
      "-expOpt",     ARGV_STRING, NULL, &expOpt,    NULL, "options for expert classifier",
  NULL) != TCL_OK) return NULL;

  return  (ClientData)hmeCreate(argv[0],classN,depthN,branchN,gateDimN,expDimN,
		    gateClass,gateOpt,expClass,expOpt);
}



/* ----------------------------------------------------------------------- */
/*  hmeFreeItf                                                             */
/* ----------------------------------------------------------------------- */
int  hmeFreeItf (ClientData cd) {

  return hmeFree((Hme *) cd);
}



/* ----------------------------------------------------------------------- */
/*  hmeShowItf                                                             */
/* ----------------------------------------------------------------------- */
int  hmeShowItf (ClientData cd, int argc, char *argv[]) {

  return hmeShow((Hme *) cd);
}



/* ----------------------------------------------------------------------- */
/*  hmePutsItf                                                             */
/* ----------------------------------------------------------------------- */
int  hmePutsItf (ClientData cd, int argc, char *argv[]) {

  Hme            *hme = (Hme *) cd;
  register int   classX;

  itfAppendResult("{name %s} {useN %d} ",hme->name,hme->useN);
  itfAppendResult("{classN %d} {gateDimN %d} {expDimN %d} ",
		  hme->classN,hme->gateDimN,hme->expDimN);
  itfAppendResult("{frameX %d} {xtrTest %d} {evalPen %d} {initLayer %d} {pruneT %f} ",
       hme->frameX,hme->xtrTest,hme->evalPen,hme->initLayer,hme->pruneT);
  itfAppendResult("{trainN %d} ",hme->trainN);
  itfAppendResult("{likelihood %f} ",hme->likelihood);
  itfAppendResult("{defClass %s} {defClassOpts %s} {defChildN %d} {defPerturbe %f} ",
         hme->defClass,hme->defClassOpts,hme->defChildN,hme->defPerturbe);
  
  itfAppendResult("{prior { ");
  for (classX=0; classX<hme->classN; classX++)
    itfAppendResult("%f ",hme->prior[classX]);
  itfAppendResult("} } ");
  
  return TCL_OK;
}



/* ----------------------------------------------------------------------- */
/*  hmeConfigureItf                                                        */
/* ----------------------------------------------------------------------- */
int  hmeConfigureItf (ClientData cd, char *var, char *val) {

  Hme                *hme = (Hme *) cd;

  if (! var) {
    ITFCFG(hmeConfigureItf,cd,"name");
    ITFCFG(hmeConfigureItf,cd,"useN");
    ITFCFG(hmeConfigureItf,cd,"classN");
    ITFCFG(hmeConfigureItf,cd,"gateDimN");
    ITFCFG(hmeConfigureItf,cd,"expDimN");
    ITFCFG(hmeConfigureItf,cd,"frameX");
    ITFCFG(hmeConfigureItf,cd,"xtrTest");
    ITFCFG(hmeConfigureItf,cd,"evalPen");
    ITFCFG(hmeConfigureItf,cd,"initLayer");
    ITFCFG(hmeConfigureItf,cd,"pruneT");
    ITFCFG(hmeConfigureItf,cd,"trainN");
    ITFCFG(hmeConfigureItf,cd,"likelihood");
    ITFCFG(hmeConfigureItf,cd,"defClass");
    ITFCFG(hmeConfigureItf,cd,"defClassOpts");
    ITFCFG(hmeConfigureItf,cd,"defChildN");
    ITFCFG(hmeConfigureItf,cd,"defPerturbe");
    return TCL_OK;
  }
  ITFCFG_CharPtr (var,val,"name",         hme->name ,        1);
  ITFCFG_Int     (var,val,"useN",         hme->useN ,        1);
  ITFCFG_Int     (var,val,"classN",       hme->classN ,      1);
  ITFCFG_Int     (var,val,"gateDimN",     hme->gateDimN ,    1);
  ITFCFG_Int     (var,val,"expDimN",      hme->expDimN ,     1);
  ITFCFG_Int     (var,val,"frameX",       hme->frameX ,      0);
  ITFCFG_Int     (var,val,"xtrTest",      hme->xtrTest ,     0);
  ITFCFG_Int     (var,val,"evalPen",      hme->evalPen ,     0);
  ITFCFG_Int     (var,val,"initLayer",    hme->initLayer ,   0);
  ITFCFG_Float   (var,val,"pruneT",       hme->pruneT ,      0);
  ITFCFG_Int     (var,val,"trainN",       hme->trainN ,      1);
  ITFCFG_Float   (var,val,"likelihood",   hme->likelihood ,  1);
  ITFCFG_CharPtr (var,val,"defClass",     hme->defClass,     0);
  ITFCFG_CharPtr (var,val,"defClassOpts", hme->defClassOpts, 0);
  ITFCFG_Int     (var,val,"defChildN",    hme->defChildN ,   0);
  ITFCFG_Float   (var,val,"defPerturbe",  hme->defPerturbe , 0);

  return TCL_OK;
}



/* ----------------------------------------------------------------------- */
/*  hmeAccessItf                                                           */
/* ----------------------------------------------------------------------- */
ClientData  hmeAccessItf (ClientData cd, char *name, TypeInfo **ti) {

  Hme          *hme = (Hme *) cd;
  
  if (*name=='.') {
    if (name[1]=='\0') {
      itfAppendResult("root ");
      if (hme->testEnv) itfAppendResult("testEnv ");
    }
    else { 
      if (!strcmp(name+1,"root")) {
	*ti=itfGetType("HmeNode");
	return (ClientData)(hme->root);
      }
      if ((!strcmp(name+1,"testEnv")) && (hme->testEnv)) {
	*ti=itfGetType("HmeTestEnv");
	return (ClientData)(hme->testEnv);
      }
    }
  }
  *ti=NULL;
  return NULL;
}



/* ------------------------------------------------------------------------ */
/*  hmeSaveItf                                                              */
/* ------------------------------------------------------------------------ */
int  hmeSaveItf (ClientData cd, int argc, char *argv[]) {

  Hme              *hme = (Hme *) cd;
  int              ac   = argc-1;
  char             *filename;
  FILE             *fp;
  
  if ( itfParseArgv( argv[0], &ac, argv+1, 0,
      "<filename>", ARGV_STRING, NULL, &filename, NULL, "name of file",
  NULL) != TCL_OK) return TCL_ERROR;

  if ((fp = fileOpen(filename,"w")) == NULL) {
    ERROR("couldn't open file '%s'\n",filename);
    return TCL_ERROR;
  }
  
  if (hmeSave(hme,fp) == TCL_ERROR) return TCL_ERROR;
  fileClose(filename,fp);

  return TCL_OK;
}



/* ------------------------------------------------------------------------ */
/*  hmePriorItf                                                             */
/* ------------------------------------------------------------------------ */
int  hmePriorItf (ClientData cd, int argc, char *argv[]) {

  Hme              *hme = (Hme *) cd;
  int              ac   = argc-1;
  int              classX;
  float            logprior;
  
  if ( itfParseArgv( argv[0], &ac, argv+1, 0,
      "<classX>",   ARGV_INT,   NULL, &classX,   NULL, "output class index",
      "<logprior>", ARGV_FLOAT, NULL, &logprior, NULL, "log(prior) of class",
  NULL) != TCL_OK) return TCL_ERROR;

  if ((classX < 0) || (classX >= hme->classN)) {
    ERROR("Invalid class index %d\n",classX);
    return TCL_ERROR;
  }

  hme->prior[classX] = logprior;
  return TCL_OK;
}



/* ------------------------------------------------------------------------ */
/*  hmeLoadItf                                                              */
/* ------------------------------------------------------------------------ */
int  hmeLoadItf (ClientData cd, int argc, char *argv[]) {

  Hme              *hme = (Hme *) cd;
  int              ac   = argc-1;
  char             *filename;
  FILE             *fp;
  
  if ( itfParseArgv( argv[0], &ac, argv+1, 0,
      "<filename>", ARGV_STRING, NULL, &filename, NULL, "name of file",
  NULL) != TCL_OK) return TCL_ERROR;

  if ((fp = fileOpen(filename,"r")) == NULL) {
    ERROR("couldn't open file '%s'\n",filename);
    return TCL_ERROR;
  }
  
  if (hmeLoad(hme,fp) == NULL) return TCL_ERROR;
  fileClose(filename,fp);

  return TCL_OK;
}



/* ------------------------------------------------------------------------ */
/*  hmeResetItf                                                             */
/* ------------------------------------------------------------------------ */
int  hmeResetItf (ClientData cd, int argc, char *argv[]) {

  return  hmeReset((Hme *) cd);
}



/* ------------------------------------------------------------------------ */
/*  hmeConfGatesItf                                                         */
/* ------------------------------------------------------------------------ */
int  hmeConfGatesItf (ClientData cd, int argc, char *argv[]) {

  return  hmeConfGates((Hme *) cd,argc,argv);
}



/* ------------------------------------------------------------------------ */
/*  hmeConfExpertsItf                                                       */
/* ------------------------------------------------------------------------ */
int  hmeConfExpertsItf (ClientData cd, int argc, char *argv[]) {

  return  hmeConfExperts((Hme *) cd,argc,argv);
}



/* ------------------------------------------------------------------------ */
/*  hmeScoreItf                                                             */
/* ------------------------------------------------------------------------ */
int  hmeScoreItf (ClientData cd, int argc, char *argv[]) {

  Hme            *hme        = (Hme *) cd;
  int            ac          = argc-1;
  int            classX      = 0;
  int            frameX      = 0;
  int            divPrior    = 0;
  FeatureSet     *fes        = NULL;
  int            gateFeatIdx = 0;
  int            expFeatIdx  = 0;
  float          *gf         = NULL;
  float          *ef         = NULL;
  double         score;

  if ( itfParseArgv (argv[0], &ac, argv+1, 0,
      "<fes>",         ARGV_OBJECT, NULL, &fes,         "FeatureSet", "FeatureSet",
      "<gateFeatIdx>", ARGV_INT,    NULL, &gateFeatIdx, NULL,         "index of feature for gates",
      "<expFeatIdx>",  ARGV_INT,    NULL, &expFeatIdx,  NULL,         "index of feature for experts",
      "<frameX>",      ARGV_INT,    NULL, &frameX,      NULL,         "frame index",
      "<classX>",      ARGV_INT,    NULL, &classX,      NULL,         "output class index",
      "-divPrior",     ARGV_INT,    NULL, &divPrior,    NULL,         "divide by priors?",
    NULL) != TCL_OK) return TCL_ERROR;

  gf = fesFrame(fes,gateFeatIdx,frameX);
  ef = fesFrame(fes,expFeatIdx,frameX);

  if ((gf == NULL) || (ef == NULL)) {
    ERROR("couldn't get feature vector for frameX=%d from FeatureSet",frameX);
    return TCL_ERROR;
  }

  score = hmeScore(hme,gf,ef,frameX,classX,divPrior);
  itfAppendResult("%10.7f",score);  
  
  return TCL_OK;
}



/* ------------------------------------------------------------------------ */
/*  hmeCreateAccusItf                                                       */
/* ------------------------------------------------------------------------ */
int  hmeCreateAccusItf (ClientData cd, int argc, char *argv[]) {

  return  hmeCreateAccus((Hme *) cd);
}



/* ------------------------------------------------------------------------ */
/*  hmeFreeAccusItf                                                         */
/* ------------------------------------------------------------------------ */
int  hmeFreeAccusItf (ClientData cd, int argc, char *argv[]) {

  return  hmeFreeAccus((Hme *) cd);
}



/* ------------------------------------------------------------------------ */
/*  hmeClearAccusItf                                                        */
/* ------------------------------------------------------------------------ */
int  hmeClearAccusItf (ClientData cd, int argc, char *argv[]) {

  return  hmeClearAccus((Hme *) cd);
}



/* ------------------------------------------------------------------------ */
/*  hmeSaveAccusItf                                                         */
/* ------------------------------------------------------------------------ */
int  hmeSaveAccusItf (ClientData cd, int argc, char *argv[]) {

  Hme              *hme = (Hme *) cd;
  int              ac   = argc-1;
  char             *filename;
  FILE             *fp;
  
  if ( itfParseArgv( argv[0], &ac, argv+1, 0,
      "<filename>", ARGV_STRING, NULL, &filename, NULL, "name of file",
  NULL) != TCL_OK) return TCL_ERROR;

  if ((fp = fileOpen(filename,"w")) == NULL) {
    ERROR("couldn't open file '%s'\n",filename);
    return TCL_ERROR;
  }
  
  if (hmeSaveAccus(hme,fp) == TCL_ERROR) return TCL_ERROR;
  fileClose(filename,fp);

  return TCL_OK;
}



/* ------------------------------------------------------------------------ */
/*  hmeLoadAccusItf                                                         */
/* ------------------------------------------------------------------------ */
int  hmeLoadAccusItf (ClientData cd, int argc, char *argv[]) {

  Hme              *hme = (Hme *) cd;
  int              ac   = argc-1;
  char             *filename;
  FILE             *fp;
  
  if ( itfParseArgv( argv[0], &ac, argv+1, 0,
      "<filename>", ARGV_STRING, NULL, &filename, NULL, "name of file",
  NULL) != TCL_OK) return TCL_ERROR;

  if ((fp = fileOpen(filename,"r")) == NULL) {
    ERROR("couldn't open file '%s'\n",filename);
    return TCL_ERROR;
  }
  
  if (hmeLoadAccus(hme,fp) == TCL_ERROR) return TCL_ERROR;
  fileClose(filename,fp);

  return TCL_OK;
}



/* ------------------------------------------------------------------------ */
/*  hmePosteriorItf                                                         */
/* ------------------------------------------------------------------------ */
int  hmePosteriorItf (ClientData cd, int argc, char *argv[]) {

  Hme            *hme        = (Hme *) cd;
  FeatureSet     *fes        = NULL;
  int            gateFeatIdx = 0;
  int            expFeatIdx  = 0;
  int            frameX      = 0;
  int            classX      = 0;
  int            ac          = argc-1;
  float          *gf         = NULL;
  float          *ef         = NULL;

  if ( itfParseArgv (argv[0], &ac, argv+1, 0,
      "<fes>",         ARGV_OBJECT, NULL, &fes,         "FeatureSet", "FeatureSet",
      "<gateFeatIdx>", ARGV_INT,    NULL, &gateFeatIdx, NULL,         "index of feature for gates",
      "<expFeatIdx>",  ARGV_INT,    NULL, &expFeatIdx,  NULL,         "index of feature for experts",
      "<frameX>",      ARGV_INT,    NULL, &frameX,      NULL,         "frame index",
      "<classX>",      ARGV_INT,    NULL, &classX,      NULL,         "target index",
    NULL) != TCL_OK) return TCL_ERROR;
  
  gf = fesFrame(fes,gateFeatIdx,frameX);
  ef = fesFrame(fes,expFeatIdx,frameX);

  if ((gf == NULL) || (ef == NULL)) {
    ERROR("couldn't get feature vector for frameX=%d from FeatureSet",frameX);
    return TCL_ERROR;
  }  

  return  hmePosterior(hme,gf,ef,classX);
}



/* ------------------------------------------------------------------------ */
/*  hmeAccuItf                                                              */
/* ------------------------------------------------------------------------ */
int  hmeAccuItf (ClientData cd, int argc, char *argv[]) {

  Hme            *hme        = (Hme *) cd;
  FeatureSet     *fes        = NULL;
  int            gateFeatIdx = 0;
  int            expFeatIdx  = 0;
  int            frameX      = 0;
  int            classX      = 0;
  int            ac          = argc-1;
  float          *gf         = NULL;
  float          *ef         = NULL;

  if ( itfParseArgv (argv[0], &ac, argv+1, 0,
      "<fes>",         ARGV_OBJECT, NULL, &fes,         "FeatureSet", "FeatureSet",
      "<gateFeatIdx>", ARGV_INT,    NULL, &gateFeatIdx, NULL,         "index of feature for gates",
      "<expFeatIdx>",  ARGV_INT,    NULL, &expFeatIdx,  NULL,         "index of feature for experts",
      "<frameX>",      ARGV_INT,    NULL, &frameX,      NULL,         "frame index",
      "<classX>",      ARGV_INT,    NULL, &classX,      NULL,         "target index",
    NULL) != TCL_OK) return TCL_ERROR;
  
  gf = fesFrame(fes,gateFeatIdx,frameX);
  ef = fesFrame(fes,expFeatIdx,frameX);

  if ((gf == NULL) || (ef == NULL)) {
    ERROR("couldn't get feature vector for frameX=%d from FeatureSet",frameX);
    return TCL_ERROR;
  }  

  return  hmeAccu(hme,gf,ef,classX);
}



/* ------------------------------------------------------------------------ */
/*  hmeUpdateItf                                                            */
/* ------------------------------------------------------------------------ */
int  hmeUpdateItf (ClientData cd, int argc, char *argv[]) {

  return  hmeUpdate((Hme *) cd);
}


  
/* ------------------------------------------------------------------------ */
/*  hmeCreateTestItf                                                        */
/* ------------------------------------------------------------------------ */
int  hmeCreateTestItf (ClientData cd, int argc, char *argv[]) {

  Hme     *hme = (Hme *) cd;
  int     dataN = 100;
  int     skipN = 0;
  int     ac    = argc-1;

  if ( itfParseArgv (argv[0], &ac, argv+1, 0,
      "-dataN", ARGV_INT,    NULL, &dataN, NULL, "max size of test buffer",
      "-skipN", ARGV_INT,    NULL, &skipN, NULL, "skipping value for test vector extraction",
    NULL) != TCL_OK) return TCL_ERROR;
  
  return  hmeCreateTest(hme,dataN,skipN);
}


  
/* ------------------------------------------------------------------------ */
/*  hmeFreeTestItf                                                          */
/* ------------------------------------------------------------------------ */
int  hmeFreeTestItf (ClientData cd, int argc, char *argv[]) {

  Hme         *hme = (Hme *) cd;
  
  return  hmeTestEnvFree(hme->testEnv);
}


  
/* ------------------------------------------------------------------------ */
/*  hmeSaveTestItf                                                          */
/* ------------------------------------------------------------------------ */
int  hmeSaveTestItf (ClientData cd, int argc, char *argv[]) {

  Hme              *hme = (Hme *) cd;
  int              ac   = argc-1;
  char             *filename;
  FILE             *fp;
  
  if ( itfParseArgv( argv[0], &ac, argv+1, 0,
      "<filename>", ARGV_STRING, NULL, &filename, NULL, "name of file",
  NULL) != TCL_OK) return TCL_ERROR;

  if ((fp = fileOpen(filename,"w")) == NULL) {
    ERROR("couldn't open file '%s'\n",filename);
    return TCL_ERROR;
  }
  
  if (hmeSaveTestEnv(hme,fp) == TCL_ERROR) return TCL_ERROR;
  fileClose(filename,fp);

  return  TCL_OK;
}


  
/* ------------------------------------------------------------------------ */
/*  hmeLoadTestItf                                                          */
/* ------------------------------------------------------------------------ */
int  hmeLoadTestItf (ClientData cd, int argc, char *argv[]) {

  Hme              *hme = (Hme *) cd;
  int              ac   = argc-1;
  char             *filename;
  FILE             *fp;
  
  if ( itfParseArgv( argv[0], &ac, argv+1, 0,
      "<filename>", ARGV_STRING, NULL, &filename, NULL, "name of file",
  NULL) != TCL_OK) return TCL_ERROR;

  if ((fp = fileOpen(filename,"r")) == NULL) {
    ERROR("couldn't open file '%s'\n",filename);
    return TCL_ERROR;
  }
  
  if (hmeLoadTestEnv(hme,fp) == TCL_ERROR) return TCL_ERROR;
  fileClose(filename,fp);

  return TCL_OK;
}


  
/* ------------------------------------------------------------------------ */
/*  hmeTestItf                                                              */
/* ------------------------------------------------------------------------ */
int  hmeTestItf (ClientData cd, int argc, char *argv[]) {

  int              testN = 0;
  int              ac = argc-1;
  
  if ( itfParseArgv( argv[0], &ac, argv+1, 0,
      "-testN", ARGV_INT, NULL, &testN, NULL, "number of test vectors",
  NULL) != TCL_OK) return TCL_ERROR;

  return  hmeTest((Hme *) cd,testN);
}

  

/* ======================================================================== */
/*                method declaration and module initialization              */
/* ======================================================================== */


static Method hmeNodeMethod[] = 
{ 
  { "puts",       hmeNodePutsItf,    NULL },
  { "plug",       hmeNodePlugItf,    "plug in a new classifier" },
  { "grow",       hmeNodeGrowItf,    "grow an expert to a subtree of depth 1" },
  { "expand",     hmeNodeExpandItf,  "expand an expert by switching it to a gate" },
  { "hme",        hmeNodeHmeItf,     "discard expert and plug in an Hme" },
  {  NULL,  NULL, NULL } 
};


TypeInfo hmeNodeInfo = { 
        "HmeNode", 0, -1, hmeNodeMethod, 
         NULL, NULL,
         NULL, hmeNodeAccessItf, itfTypeCntlDefaultNoLink,
	"A computing node in a Hierarchical Mixture of Experts tree\n" } ;


static Method hmeTestEnvMethod[] =
{
  { "puts",        hmeTestEnvPutsItf,   NULL },
  { NULL,  NULL,  NULL },
};



TypeInfo hmeTestEnvInfo = { 
        "HmeTestEnv", 0, -1, hmeTestEnvMethod, 
         NULL, NULL,
         NULL, NULL, itfTypeCntlDefaultNoLink,
	"Test environment for an Hme\n" } ;


static Method hmeMethod[] = 
{ 
  { "puts",        hmePutsItf,        NULL },
  { "show",        hmeShowItf,        "show Hme tree structure" },
  { "save",        hmeSaveItf,        "save Hme" },
  { "load",        hmeLoadItf,        "load Hme" },
  { "prior",       hmePriorItf,       "set prior class probabilities" },
  { "reset",       hmeResetItf,       "reset Hme gates and experts" },
  { "confGates",   hmeConfGatesItf,   "send configure commands to gates" },
  { "confExperts", hmeConfExpertsItf, "send configure commands to experts" },
  { "score",       hmeScoreItf,       "compute class score given a frame" },
  { "createAccus", hmeCreateAccusItf, "create training accumulators" },
  { "freeAccus",   hmeFreeAccusItf,   "free training accumulators" },
  { "clearAccus",  hmeClearAccusItf,  "clear training accumulators" },
  { "saveAccus",   hmeSaveAccusItf,   "save training accumulators" },
  { "loadAccus",   hmeLoadAccusItf,   "load and accu training accumulators" },
  { "posterior",   hmePosteriorItf,   "compute node/branch posteriors given a frame" },
  { "accu",        hmeAccuItf,        "accumulate training information for one frame" },
  { "update",      hmeUpdateItf,      "update Hme parameters using accus" },
  { "createTest",  hmeCreateTestItf,  "create a test environment for the Hme" },
  { "freeTest",    hmeFreeTestItf,    "free test environment" },
  { "saveTest",    hmeSaveTestItf,    "save test environment" },
  { "loadTest",    hmeLoadTestItf,    "load test environment" },
  { "test",        hmeTestItf,        "test Hme using test environment" },
  {  NULL,  NULL, NULL } 
};


TypeInfo hmeInfo = { 
        "Hme", 0, -1, hmeMethod, 
         hmeCreateItf, hmeFreeItf,
         hmeConfigureItf, hmeAccessItf, itfTypeCntlDefaultNoLink,
	"A single Hierarchical Mixture of Experts (HME)\n" } ;



static int hmeInitialized = 0;

int Hme_Init ( )
{
  if ( hmeInitialized) return TCL_OK;

  itfNewType ( &hmeNodeInfo );
  itfNewType ( &hmeTestEnvInfo );
  itfNewType ( &hmeInfo );

  hmeInitialized = 1;
  return TCL_OK;
}

