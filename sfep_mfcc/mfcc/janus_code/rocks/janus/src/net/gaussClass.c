/* ========================================================================
    JANUS      Speech- to Speech Translation
               ------------------------------------------------------------
               Object: Multivariate Diagonal Covariance Gauss Classifier
               ------------------------------------------------------------

    Author  :  Juergen Fritsch
    Module  :  gaussClass.c
    Date    :  $Id: gaussClass.c 2390 2003-08-14 11:20:32Z fuegen $
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

 * Revision 1.1  96/09/27  09:00:13  fritsch
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
#include "gaussClass.h"



/* ======================================================================= */
/*                       (1)  local implementation                         */
/* ======================================================================= */


#define   FSWAP(A,B)   { register float swappy = A; A = B; B = swappy; }
#define   ISWAP(A,B)   { register int   swappy = A; A = B; B = swappy; }
#define   square(X)    ((X)*(X))

#define   PI                    3.141592653589793238462643383279502884
#define   square(X)             ((X)*(X))
#define   MINVAL                1E-20
#define   MAXVAL                1E+20
#define   GAUSS_MAGIC           3030
#define   GAUSS_ACCU_MAGIC      4040

int  gaussClassClearAccus (GaussClass *gaussClass);
int  gaussClassFreeAccus (GaussClass *gaussClass);


/* ----------------------------------------------------------------------- */
/*  boxSetCreate                                                           */
/* ----------------------------------------------------------------------- */
BoxSet  *boxSetCreate (int rbfN, int dimN) {

  BoxSet               *boxSet;
  register int         dimX;

  boxSet = (BoxSet *) malloc (sizeof(BoxSet));
  boxSet->rbfN = rbfN;
  boxSet->dimN = dimN;

  boxSet->act = (int *) malloc (sizeof(int) * rbfN);
  
  boxSet->min = (float **) malloc (sizeof(float *) * dimN);
  boxSet->max = (float **) malloc (sizeof(float *) * dimN);
  for (dimX=0; dimX<dimN; dimX++) {
    boxSet->min[dimX] = (float *) malloc (sizeof(float) * rbfN);
    boxSet->max[dimX] = (float *) malloc (sizeof(float) * rbfN);
  }

  return boxSet;
}



/* --------------------------------------------------------------------- */
/*  boxSetFree                                                           */
/* --------------------------------------------------------------------- */
int  boxSetFree (BoxSet *boxSet) {

  register int        dimX;

  if (!boxSet) return TCL_OK;

  for (dimX=0; dimX<boxSet->dimN; dimX++) {
    free(boxSet->min[dimX]);
    free(boxSet->max[dimX]);
  }
  free(boxSet->min);
  free(boxSet->max);
  free(boxSet->act);
  free(boxSet);
  
  return TCL_OK;
}



/* ---------------------------------------------------------------------- */
/*  boxSetCopy                                                            */
/* ---------------------------------------------------------------------- */
int  boxSetCopy (BoxSet *target, BoxSet *source) {

  register int      dimX,rbfX;
  
  if (!source) {
    ERROR("No source BoxSet supplied\n");
    return TCL_ERROR;
  }
  if (!target) {
    ERROR("No target BoxSet supplied\n");
    return TCL_ERROR;
  }

  if (source->rbfN != target->rbfN) {
    ERROR("number of RBFs incompatible\n");
    return TCL_ERROR;
  }
  if (source->dimN != target->dimN) {
    ERROR("feature dimensions incompatible\n");
    return TCL_ERROR;
  }

  for (dimX=0; dimX<source->dimN; dimX++) {
    for (rbfX=0; rbfX<source->rbfN; rbfX++) {
      target->min[dimX][rbfX] = source->min[dimX][rbfX];
      target->max[dimX][rbfX] = source->max[dimX][rbfX];
    }
  }

  for (rbfX=0; rbfX<source->rbfN; rbfX++)
    target->act[rbfX] = source->act[rbfX];
  
  return TCL_OK;
}



/* --------------------------------------------------------------------- */
/*  boxSetCutOff                                                         */
/* --------------------------------------------------------------------- */
int  boxSetCutOff (BoxSet *boxSet, int splitX, float splitVal, int right) {

  register float     *min,*max;
  register int       *act;
  register int       rbfX;

  if ((splitX < 0) || (splitX >= boxSet->dimN)) {
    ERROR("splitX out of range\n");
    return TCL_ERROR;
  }

  min = boxSet->min[splitX];
  max = boxSet->max[splitX];
  act = boxSet->act;
  
  if (right) {
    for (rbfX=0; rbfX<boxSet->rbfN; rbfX++)
      if (act[rbfX]) {
        if (max[rbfX] < splitVal)
          act[rbfX] = 0;
        else
          if (min[rbfX] < splitVal)
            min[rbfX] = splitVal;
      }
  } else {
    for (rbfX=0; rbfX<boxSet->rbfN; rbfX++)
      if (act[rbfX]) {
        if (min[rbfX] > splitVal)
          act[rbfX] = 0;
        else
          if (max[rbfX] > splitVal)
            max[rbfX] = splitVal;
      }
  }

  return TCL_OK;
}



/* -------------------------------------------------------------------- */
/*  boxSetInit                                                          */
/* -------------------------------------------------------------------- */
int  boxSetInit (BoxSet *boxSet, GaussClass *gc, float threshold) {

  register int        dimX,rbfX;
  register double     dist,factor;
  
  if (!boxSet) {
    ERROR("no BoxSet supplied\n");
    return TCL_ERROR;
  }
  if (!gc) {
    ERROR("no Gauss classifier supplied\n");
    return TCL_ERROR;
  }
  if (boxSet->dimN != gc->dimN) {
    ERROR("feature dimensions incompatible\n");
    return TCL_ERROR;
  }
  if (boxSet->rbfN != gc->classN) {
    ERROR("number of Gaussians incompatible\n");
    return TCL_ERROR;
  }
  if ((threshold >= 1.0) || (threshold <= 0.0)) {
    ERROR("threshold must be in range (0,1)\n");
    return TCL_ERROR;
  }
  
  factor = -2.0*log(threshold);
  for (rbfX=0; rbfX<boxSet->rbfN; rbfX++) {
    boxSet->act[rbfX] = 1;
    for (dimX=0; dimX<boxSet->dimN; dimX++) {
      dist = sqrt(factor/gc->ivar[rbfX][dimX]);
      boxSet->min[dimX][rbfX] = gc->mean[rbfX][dimX] - dist;
      boxSet->max[dimX][rbfX] = gc->mean[rbfX][dimX] + dist;
    }
  }

  return TCL_OK;
}



/* --------------------------------------------------------------------- */
/*  mergeSort                                                            */
/* --------------------------------------------------------------------- */
int  mergeSort (float *bnd, int *lbl, float *tmpBnd, int *tmpLbl,
                int left, int right) {

  register int     elN,center,k,leftPnt,rightPnt;

  elN = right-left+1;
  if (elN > 2) {
    center = (left+right)>>1;
    mergeSort(bnd,lbl,tmpBnd,tmpLbl,left,center-1);
    mergeSort(bnd,lbl,tmpBnd,tmpLbl,center,right);
    leftPnt  = left;
    rightPnt = center;
    for (k=left; k<=right; k++) {
      if ((leftPnt < center) && (rightPnt <= right)) {
        if (bnd[leftPnt] < bnd[rightPnt]) {
          tmpBnd[k] = bnd[leftPnt];
          tmpLbl[k] = lbl[leftPnt];
          leftPnt++;
        } else {
          tmpBnd[k] = bnd[rightPnt];
          tmpLbl[k] = lbl[rightPnt];
          rightPnt++;
        }
      } else {
        if (leftPnt == center) {
          tmpBnd[k] = bnd[rightPnt];
          tmpLbl[k] = lbl[rightPnt];
          rightPnt++;
        } else {
          tmpBnd[k] = bnd[leftPnt];
          tmpLbl[k] = lbl[leftPnt];
          leftPnt++;
        }
      }
    }
    for (k=left; k<=right; k++) {
      bnd[k] = tmpBnd[k];
      lbl[k] = tmpLbl[k];
    }
  } else {
    if (elN == 2) {
      if (bnd[left] > bnd[right]) {
        FSWAP(bnd[left],bnd[right]);
        ISWAP(lbl[left],lbl[right]);
      }
    }
  }
  
  return TCL_OK;
}



/* --------------------------------------------------------------------- */
/*  bspNodeSplit                                                         */
/* --------------------------------------------------------------------- */
int  bspNodeSplit (BSPNode *node, BoxSet *boxSet) {

  register int   dimX,dimN,rbfX,rbfN,nL,nR,k,minMin,minIdx,actN;
  float          *splitVal,*splitDist,*sortBnd,minDist;
  float          *min,*max,*tmpBnd;
  int            *splitMin,*sortLbl,*tmpLbl,*act;
  
  rbfN = boxSet->rbfN;
  dimN = boxSet->dimN;

  splitVal  = (float *) malloc (sizeof(float) * dimN);
  splitDist = (float *) malloc (sizeof(float) * dimN);
  splitMin  = (int *)   malloc (sizeof(int)   * dimN);
  sortBnd   = (float *) malloc (sizeof(float) * 2 * rbfN);
  sortLbl   = (int *)   malloc (sizeof(int)   * 2 * rbfN);
  tmpBnd    = (float *) malloc (sizeof(float) * 2 * rbfN);
  tmpLbl    = (int *)   malloc (sizeof(int)   * 2 * rbfN);

  /* ---------------------------------- */
  /*  create sortBnd and sortLbl lists  */
  /* ---------------------------------- */
  for (dimX=0; dimX<boxSet->dimN; dimX++) {
    min = boxSet->min[dimX];
    max = boxSet->max[dimX];
    act = boxSet->act;

    actN = 0;
    for (rbfX=0; rbfX<rbfN; rbfX++) {
      if (act[rbfX]) {
        sortBnd[actN]   = min[rbfX];
        sortLbl[actN]   = 0;
        sortBnd[actN+1] = max[rbfX];
        sortLbl[actN+1] = 1;
        actN+=2;
      }
    }
    
    /* ---------------- */
    /*  sort the lists  */
    /* ---------------- */
    mergeSort(sortBnd,sortLbl,tmpBnd,tmpLbl,0,actN-1);

    /* --------------------------------------- */
    /*  determine optimal division hyperplane  */ 
    /* --------------------------------------- */
    nL = 0;
    nR = actN>>1;
    k = 0;
    while (nL < nR) {
      if (sortLbl[k]) nL++; else nR--;
      k++;
    }
    splitVal[dimX]  = (sortBnd[k-1] + sortBnd[k]) / 2.0;
    splitDist[dimX] = (sortBnd[actN-1] - sortBnd[0]);
    splitMin[dimX]  = rbfN - (nL + nR);
  }

  /* ------------------------------------- */
  /*  choose minimum splitting coordinate  */
  /* ------------------------------------- */
  minMin  = rbfN;
  minDist = 0.0;
  minIdx  = 0;
  for (dimX=0; dimX<dimN; dimX++) {
    if (splitMin[dimX] < minMin) {
      minMin  = splitMin[dimX];
      minDist = splitDist[dimX];
      minIdx  = dimX;
    } else if (splitMin[dimX] == minMin) {
      if (splitDist[dimX] > minDist) {
        minMin  = splitMin[dimX];
        minDist = splitDist[dimX];
        minIdx  = dimX;
      }
    }
  }

  /* -------------------------- */
  /*  set the node's parameter  */
  /* -------------------------- */
  node->splitX   = minIdx;
  node->splitVal = splitVal[minIdx];
  
  free(splitVal);
  free(splitDist);
  free(splitMin);
  free(sortBnd);
  free(sortLbl);
  free(tmpBnd);
  free(tmpLbl);

  return TCL_OK;
}



/* --------------------------------------------------------------------- */
/*  bspTreeMake  --  recursive tree generator                            */
/* --------------------------------------------------------------------- */
int  bspTreeMake (BSPTree *tree, BoxSet *boxSet, int nodeIdx,
                  int barMod, int *counter, int *barN) {

  BSPNode              *node;
  BoxSet               *childBox;
  IndexList            *leaf;
  register int         leafX,rbfX,count;

  
  (*counter)++;
  if ((*counter % barMod) == 0) {
    if (*barN < 50) {
      (*barN)++;
      printO("|");
      fflush(stdout);
    }
  }

  if (nodeIdx < tree->leafN) {

    /* ----------------------------------- */
    /*  get a pointer to the current node  */
    /* ----------------------------------- */
    node = tree->node + nodeIdx;

    /* ---------------------------------------------------- */
    /*  create a splitting hyperplane for the current node  */
    /* ---------------------------------------------------- */
    bspNodeSplit(node,boxSet);

    /* --------------------------------------------------------- */
    /*  create a BoxSet for the left child and compute the node  */
    /* --------------------------------------------------------- */
    childBox = boxSetCreate(boxSet->rbfN,boxSet->dimN);
    boxSetCopy(childBox,boxSet);
    boxSetCutOff(childBox,node->splitX,node->splitVal,0);
    bspTreeMake(tree,childBox,2*nodeIdx,barMod,counter,barN);
    boxSetFree(childBox);
  
    /* ---------------------------------------------------------- */
    /*  create a BoxSet for the right child and compute the node  */
    /* ---------------------------------------------------------- */
    childBox = boxSetCreate(boxSet->rbfN,boxSet->dimN);
    boxSetCopy(childBox,boxSet);
    boxSetCutOff(childBox,node->splitX,node->splitVal,1);
    bspTreeMake(tree,childBox,2*nodeIdx+1,barMod,counter,barN);
    boxSetFree(childBox);

  } else {

    /* -------------------------------------------------------- */
    /*  we reached a leaf node - compute the intersection list  */
    /* -------------------------------------------------------- */
    leafX = nodeIdx - tree->leafN;
    leaf  = tree->leaf + leafX;
    
    count = 0;
    for (rbfX=0; rbfX<boxSet->rbfN; rbfX++)
      if (boxSet->act[rbfX]) count++;

    leaf->listN = count;
    leaf->list  = (int *) malloc (sizeof(int) * count);

    count = 0;
    for (rbfX=0; rbfX<boxSet->rbfN; rbfX++)
      if (boxSet->act[rbfX])
        leaf->list[count++] = rbfX;
  }

  return TCL_OK;
}



/* --------------------------------------------------------------------- */
/*  bspTreeCreate                                                        */
/* --------------------------------------------------------------------- */
BSPTree  *bspTreeCreate (int depthN, BoxSet *boxSet) {

  BSPTree        *tree;
  int            leafX,listN,minRbf,maxRbf,count,k,barMod,barN;
  float          meanRbf,varRbf;
  
  /* --------------------- */
  /*  checking for errors  */
  /* --------------------- */
  if (depthN < 1) {
    ERROR("invalid depthN=%d\n",depthN);
    return NULL;
  }
  if (!boxSet) {
    ERROR("no BoxSet supplied\n");
    return NULL;
  }
  
  /* ----------------------------------------- */
  /*  allocating the BSP tree and its members  */
  /* ----------------------------------------- */
  tree = (BSPTree *) malloc (sizeof(BSPTree));
  tree->depthN = depthN;
  tree->leafN  = 1<<depthN;

  tree->node = (BSPNode *)   malloc (sizeof(BSPNode)   * (tree->leafN));
  tree->leaf = (IndexList *) malloc (sizeof(IndexList) * (tree->leafN));

  /* ---------------------------------------------- */
  /*  generate the BSP tree using the set of boxes  */
  /* ---------------------------------------------- */
  for (k=0; k<=50; k++) {
    if (k % 5 == 0)
      if ((k==0) || (k==50))
        printO("|");
      else
	printO("+");
    else
      printO("-");
  }
  printO("\n|");
  barMod = (1<<(depthN+1)) / 50;
  if (barMod == 0) barMod = 1;
  
  count = 0;
  barN = 0;
  if (bspTreeMake(tree,boxSet,1,barMod,&count,&barN) == TCL_ERROR) {
    ERROR("error while generating BSP tree\n");
    return NULL;
  }

  printO("\n");
  
  /* --------------------------------------------------- */
  /*  compute and print out statistics for the BSP tree  */
  /* --------------------------------------------------- */
  minRbf  = boxSet->rbfN;
  maxRbf  = 0;
  meanRbf = 0.0;
  varRbf  = 0.0;
  for (leafX=0; leafX<tree->leafN; leafX++) {
    listN = tree->leaf[leafX].listN;
    if (listN < minRbf) minRbf = listN;
    if (listN > maxRbf) maxRbf = listN;
    meanRbf += listN;
    varRbf  += square(listN);
  }
  meanRbf /= tree->leafN;
  varRbf  = sqrt((varRbf / tree->leafN) - square(meanRbf));

  INFO("min=%d  max=%d  mean=%f  stddev=%f\n",minRbf,maxRbf,meanRbf,varRbf);
  
  return tree;
}



/* --------------------------------------------------------------------- */
/*  bspTreeFree                                                          */
/* --------------------------------------------------------------------- */
int  bspTreeFree (BSPTree *tree) {

  register int        leafX;
  
  if (!tree) return TCL_OK;

  for (leafX=0; leafX<tree->leafN; leafX++)
    free(tree->leaf[leafX].list);
  free(tree->leaf);
  free(tree->node);
  free(tree);
  
  return TCL_OK;
}


  
/* --------------------------------------------------------------------- */
/*  bspTreeUse                                                           */
/* --------------------------------------------------------------------- */
IndexList  *bspTreeUse (BSPTree *tree, float *pattern) {

  register int            nodeX,depthX,leafX;
  BSPNode                 *node;
  IndexList               *leaf;

  if (!tree) return NULL;
  if (!pattern) return NULL;

  nodeX = 1;
  for (depthX=0; depthX<tree->depthN; depthX++) {
    node  = tree->node + nodeX;
    nodeX = nodeX<<1;
    if (pattern[node->splitX] > node->splitVal)
      nodeX++;
  }
  leafX = nodeX - tree->leafN;
  leaf  = tree->leaf + leafX;
  
  return leaf;
}



/* ------------------------------------------------------------------ */
/*  gaussClassPre - pre-compute Gaussian constants (including prior)  */
/* ------------------------------------------------------------------ */
int  gaussClassPre (GaussClass *gaussClass) {

  register int       classX,dimX;
  register double    a,b;

  if (!gaussClass) {
    ERROR("no Gaussian classifier allocated\n");
    return TCL_ERROR;
  }

  b = 1.0;
  for (dimX=0; dimX<gaussClass->dimN; dimX++) {
    b *= (2*PI);
  }

  for (classX=0; classX<gaussClass->classN; classX++) {
    a = 1.0;
    for (dimX=0; dimX<gaussClass->dimN; dimX++) {
      a *= gaussClass->ivar[classX][dimX];
    }
    gaussClass->pre[classX] = gaussClass->alpha[classX] * sqrt(a/b);
  }
  
  return TCL_OK;
}

/* ----------------------------------------------------------------------- */
/*  gaussClassInit                                                         */
/* ----------------------------------------------------------------------- */
int  gaussClassInit (GaussClass *gaussClass) {

  register int          classX,dimX;
  
  for (classX=0; classX<gaussClass->classN; classX++) {
    for (dimX=0; dimX<gaussClass->dimN; dimX++) {
      gaussClass->mean[classX][dimX] = myRandom(-1.0,1.0);
      gaussClass->ivar[classX][dimX] = 1.0;
    }
    gaussClass->alpha[classX] = 1.0/gaussClass->classN;
  }
  
  return gaussClassPre(gaussClass);
}



/* ----------------------------------------------------------------------- */
/*  gaussClassCreate                                                       */
/* ----------------------------------------------------------------------- */
GaussClass  *gaussClassCreate (int classN, int dimN,
			       int trainMode, int weightMode) {

  GaussClass         *gaussClass;
  register int       classX;

  if ((classN <= 0) || (dimN <= 0)) {
    ERROR("invalid parameters\n");
    return NULL;
  }
  
  /* ------------------------------------ */
  /*  create empty new GaussClass struct  */
  /* ------------------------------------ */
  gaussClass = (GaussClass *) malloc (sizeof(GaussClass));

  gaussClass->classN     = classN;
  gaussClass->dimN       = dimN;
  gaussClass->trainMode  = trainMode;
  gaussClass->weightMode = weightMode;
  gaussClass->accu       = NULL;
  gaussClass->tree       = NULL;

  /* --------------------------------------------------- */
  /*  create arrays for Gaussian pre-factors and priors  */
  /* --------------------------------------------------- */
  gaussClass->pre   = (double *) malloc (sizeof(double) * classN);
  gaussClass->alpha = (double *) malloc (sizeof(double) * classN);

  /* ------------------------------- */
  /*  create mean and ivar matrices  */
  /* ------------------------------- */
  gaussClass->mean = (double **) malloc (sizeof(double *) * classN);
  gaussClass->ivar = (double **) malloc (sizeof(double *) * classN);

  for (classX=0; classX<classN; classX++) {
    gaussClass->mean[classX] = (double *) malloc (sizeof(double) * dimN);
    gaussClass->ivar[classX] = (double *) malloc (sizeof(double) * dimN);
  }

  gaussClassInit(gaussClass);

  return gaussClass;
}



/* ----------------------------------------------------------------------- */
/*  gaussClassFree                                                         */
/* ----------------------------------------------------------------------- */
int  gaussClassFree (GaussClass *gaussClass) {

  register int            classX;
  
  free(gaussClass->pre);
  free(gaussClass->alpha);

  for (classX=0; classX<gaussClass->classN; classX++) {
    free(gaussClass->mean[classX]);
    free(gaussClass->ivar[classX]);
  }
  free(gaussClass->mean);
  free(gaussClass->ivar);

  gaussClassFreeAccus(gaussClass);
  if (gaussClass->tree) bspTreeFree(gaussClass->tree);

  free(gaussClass);
  
  return TCL_OK;
}



/* ----------------------------------------------------------------------- */
/*  gaussClassCopy                                                         */
/* ----------------------------------------------------------------------- */
GaussClass  *gaussClassCopy (GaussClass *src, double perturbe) {

  GaussClass            *target;
  register int          classX,dimX;

  if (!src) return NULL;

  /* -------------------------------- */
  /*  create a new GaussClass object  */
  /* -------------------------------- */
  target = gaussClassCreate(src->classN,src->dimN,src->trainMode,src->weightMode);

  /* ------------------- */
  /*  copy alpha vector  */
  /* ------------------- */
  for (classX=0; classX<src->classN; classX++)
    target->alpha[classX] = src->alpha[classX];

  /* ------------------ */
  /*  copy mean matrix  */
  /* ------------------ */
  for (classX=0; classX<src->classN; classX++)
    for (dimX=0; dimX<src->dimN; dimX++)
      target->mean[classX][dimX] = src->mean[classX][dimX] +
	                           myRandom(-perturbe,perturbe);

  /* ------------------ */
  /*  copy ivar matrix  */
  /* ------------------ */
  for (classX=0; classX<src->classN; classX++)
    for (dimX=0; dimX<src->dimN; dimX++)
      target->ivar[classX][dimX] = src->ivar[classX][dimX];

  gaussClassPre(target);
  
  return  target;
}



/* ----------------------------------------------------------------------- */
/*  gaussClassSave                                                         */
/* ----------------------------------------------------------------------- */
int  gaussClassSave (GaussClass *gaussClass, FILE *fp) {

  register int             classX,dimX;

  /* ------------- */
  /*  save header  */
  /* ------------- */
  write_int(fp,GAUSS_MAGIC);
  write_int(fp,gaussClass->classN);
  write_int(fp,gaussClass->dimN);
  write_int(fp,gaussClass->trainMode);
  write_int(fp,gaussClass->weightMode);
  
  /* ------------------- */
  /*  save alpha vector  */
  /* ------------------- */
  for (classX=0; classX<gaussClass->classN; classX++)
    write_float(fp,(float) gaussClass->alpha[classX]);

  /* ------------------ */
  /*  save mean matrix  */
  /* ------------------ */
  for (classX=0; classX<gaussClass->classN; classX++)
    for (dimX=0; dimX<gaussClass->dimN; dimX++)
      write_float(fp,(float) gaussClass->mean[classX][dimX]);

  /* ------------------ */
  /*  save ivar matrix  */
  /* ------------------ */
  for (classX=0; classX<gaussClass->classN; classX++)
    for (dimX=0; dimX<gaussClass->dimN; dimX++)
      write_float(fp,(float) gaussClass->ivar[classX][dimX]);

  return TCL_OK;
}



/* ----------------------------------------------------------------------- */
/*  gaussClassLoad                                                         */
/* ----------------------------------------------------------------------- */
GaussClass  *gaussClassLoad (GaussClass *gaussClass, FILE *fp) {

  GaussClass             *newGauss;
  register int           classX,dimX;
  
  if (read_int(fp) != GAUSS_MAGIC) {
    ERROR("couldn't find GAUSS magic\n");
    return NULL;
  }

  /* ------------------------------------ */
  /*  check for existing GaussClassifier  */
  /* ------------------------------------ */
  if (gaussClass) {

    /* ---------------------------- */
    /*  free pre and alpha vectors  */
    /* ---------------------------- */
    free(gaussClass->pre);
    free(gaussClass->alpha);
    
    /* ------------------ */
    /*  free mean matrix  */
    /* ------------------ */
    for (classX=0; classX<gaussClass->classN; classX++)
      free(gaussClass->mean[classX]);
    free(gaussClass->mean);

    /* ------------------ */
    /*  free ivar matrix  */
    /* ------------------ */
    for (classX=0; classX<gaussClass->classN; classX++)
      free(gaussClass->ivar[classX]);
    free(gaussClass->ivar);

    /* --------------------- */
    /*  free accus and tree  */
    /* --------------------- */
    gaussClassFreeAccus(gaussClass);
    if (gaussClass->tree) bspTreeFree(gaussClass->tree);
    gaussClass->tree = NULL;
    newGauss = gaussClass;

  } else {

    /* ------------------------------- */
    /*  create a new Gauss classifier  */
    /* ------------------------------- */
    newGauss = (GaussClass *) malloc (sizeof(GaussClass));
    
    newGauss->accu = NULL;
    newGauss->tree = NULL;
    
  }

  /* ------------- */
  /*  load header  */
  /* ------------- */
  newGauss->classN     = read_int(fp);
  newGauss->dimN       = read_int(fp);
  newGauss->trainMode  = read_int(fp);
  newGauss->weightMode = read_int(fp);

  /* --------------------------------------------------- */
  /*  create arrays for Gaussian pre-factors and priors  */
  /* --------------------------------------------------- */
  newGauss->pre   = (double *) malloc (sizeof(double) * newGauss->classN);
  newGauss->alpha = (double *) malloc (sizeof(double) * newGauss->classN);

  /* ------------------------------- */
  /*  create mean and ivar matrices  */
  /* ------------------------------- */
  newGauss->mean = (double **) malloc (sizeof(double *) * newGauss->classN);
  newGauss->ivar = (double **) malloc (sizeof(double *) * newGauss->classN);

  for (classX=0; classX<newGauss->classN; classX++) {
    newGauss->mean[classX] = (double *) malloc (sizeof(double) * newGauss->dimN);
    newGauss->ivar[classX] = (double *) malloc (sizeof(double) * newGauss->dimN);
  }

  /* ------------------- */
  /*  load alpha vector  */
  /* ------------------- */
  for (classX=0; classX<newGauss->classN; classX++)
    newGauss->alpha[classX] = (double) read_float(fp);

  /* ------------------ */
  /*  load mean matrix  */
  /* ------------------ */
  for (classX=0; classX<newGauss->classN; classX++)
    for (dimX=0; dimX<newGauss->dimN; dimX++)
      newGauss->mean[classX][dimX] = (double) read_float(fp);

  /* ------------------ */
  /*  load ivar matrix  */
  /* ------------------ */
  for (classX=0; classX<newGauss->classN; classX++)
    for (dimX=0; dimX<newGauss->dimN; dimX++)
      newGauss->ivar[classX][dimX] = (double) read_float(fp);

  gaussClassPre(newGauss);
  
  return newGauss;
}



/* ----------------------------------------------------------------------- */
/*  gaussClassCreateAccus                                                  */
/* ----------------------------------------------------------------------- */
int  gaussClassCreateAccus (GaussClass *gaussClass) {

  register int          classX,classN,dimN;
  
  if (gaussClass->accu) return TCL_OK;

  /* ----------------------- */
  /*  create accu structure  */
  /* ----------------------- */
  classN = gaussClass->classN;
  dimN   = gaussClass->dimN;

  gaussClass->accu = (GaussClassAccu *) malloc (sizeof(GaussClassAccu));
  gaussClass->accu->classN = classN;
  gaussClass->accu->dimN   = dimN;

  /* -------------------------- */
  /*  create alpha vector accu  */
  /* -------------------------- */
  gaussClass->accu->a = (double *) malloc (sizeof(double) * classN);
  
  /* ---------------------- */
  /*  create X matrix accu  */
  /* ---------------------- */
  gaussClass->accu->x = (double **) malloc (sizeof(double *) * classN);
  for (classX=0; classX<classN; classX++)
    gaussClass->accu->x[classX] = (double *) malloc (sizeof(double) * dimN);

  /* ----------------------- */
  /*  create XX matrix accu  */
  /* ----------------------- */
  gaussClass->accu->xx = (double **) malloc (sizeof(double *) * classN);
  for (classX=0; classX<classN; classX++)
    gaussClass->accu->xx[classX] = (double *) malloc (sizeof(double) * dimN);

  return gaussClassClearAccus(gaussClass);
}


/* ----------------------------------------------------------------------- */
/*  gaussClassFreeAccus                                                    */
/* ----------------------------------------------------------------------- */
int  gaussClassFreeAccus (GaussClass *gaussClass) {

  GaussClassAccu         *accu;
  register int           classX;
  
  if (gaussClass->accu) {

    accu = gaussClass->accu;

    /* ------------------------ */
    /*  free alpha vector accu  */
    /* ------------------------ */
    free(accu->a);
  
    /* -------------------- */
    /*  free X matrix accu  */
    /* -------------------- */
    for (classX=0; classX<gaussClass->classN; classX++)
      free(accu->x[classX]);
    free(accu->x);

    /* --------------------- */
    /*  free XX matrix accu  */
    /* --------------------- */
    for (classX=0; classX<gaussClass->classN; classX++)
      free(accu->xx[classX]);
    free(accu->xx);

    free(accu);
    gaussClass->accu = NULL;
    
  }
  
  return TCL_OK;
}



/* ----------------------------------------------------------------------- */
/*  gaussClassClearAccus                                                   */
/* ----------------------------------------------------------------------- */
int  gaussClassClearAccus (GaussClass *gaussClass) {

  GaussClassAccu           *accu;
  register int             classX,dimX;
  
  if (gaussClass->accu) {

    accu = gaussClass->accu;

    /* -------------------------- */
    /*  create alpha vector accu  */
    /* -------------------------- */
    for (classX=0; classX<accu->classN; classX++)
      accu->a[classX] = 0.0;
  
    /* ---------------------- */
    /*  create X matrix accu  */
    /* ---------------------- */
    for (classX=0; classX<accu->classN; classX++)
      for (dimX=0; dimX<accu->dimN; dimX++)
        accu->x[classX][dimX] = 0.0;

    /* ----------------------- */
    /*  create XX matrix accu  */
    /* ----------------------- */
    for (classX=0; classX<accu->classN; classX++)
      for (dimX=0; dimX<accu->dimN; dimX++)
        accu->xx[classX][dimX] = 0.0;

    accu->dist   = 0.0;
    accu->lh     = 0.0;
    accu->trainN = 0;
    
  } else WARN("no accus allocated for Gauss classifier\n");
  
  return TCL_OK;
}



/* ----------------------------------------------------------------------- */
/*  gaussClassSaveAccus                                                    */
/* ----------------------------------------------------------------------- */
int  gaussClassSaveAccus (GaussClass *gaussClass, FILE *fp) {

  GaussClassAccu                *accu;
  register int                  classX,dimX;
  
  if (!gaussClass->accu) {
    ERROR("no accus allocated for Gauss classifier\n");
    return TCL_ERROR;
  }

  accu = gaussClass->accu;

  /* -------------- */
  /*  write header  */
  /* -------------- */
  write_int(fp,GAUSS_ACCU_MAGIC);
  write_int(fp,accu->classN);
  write_int(fp,accu->dimN);

  /* ------------------------ */
  /*  save alpha vector accu  */
  /* ------------------------ */
  for (classX=0; classX<accu->classN; classX++)
    write_float(fp,(float) accu->a[classX]);
  
  /* -------------------- */
  /*  save X matrix accu  */
  /* -------------------- */
  for (classX=0; classX<accu->classN; classX++)
    for (dimX=0; dimX<accu->dimN; dimX++)
      write_float(fp,(float) accu->x[classX][dimX]);

  /* --------------------- */
  /*  save XX matrix accu  */
  /* --------------------- */
  for (classX=0; classX<accu->classN; classX++)
    for (dimX=0; dimX<accu->dimN; dimX++)
      write_float(fp,(float) accu->xx[classX][dimX]);

  write_float(fp,(float) accu->dist);
  write_float(fp,(float) accu->lh);
  write_int(fp,accu->trainN);  

  return TCL_OK;
}



/* ----------------------------------------------------------------------- */
/*  gaussClassLoadAccus                                                    */
/* ----------------------------------------------------------------------- */
int  gaussClassLoadAccus (GaussClass *gaussClass, FILE *fp) {

  GaussClassAccu                 *accu;
  register int                  classX,dimX;
  
  if (!gaussClass->accu) {
    ERROR("no accus allocated for Gauss classifier\n");
    return TCL_ERROR;
  }

  accu = gaussClass->accu;

  /* ------------- */
  /*  read header  */
  /* ------------- */
  if (read_int(fp) != GAUSS_ACCU_MAGIC) {
    ERROR("couldn't find GAUSS_ACCU magic\n");
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

  /* ------------------------ */
  /*  load alpha vector accu  */
  /* ------------------------ */
  for (classX=0; classX<accu->classN; classX++)
    accu->a[classX] += (double) read_float(fp); 

  /* -------------------- */
  /*  load X matrix accu  */
  /* -------------------- */
  for (classX=0; classX<accu->classN; classX++)
    for (dimX=0; dimX<accu->dimN; dimX++)
      accu->x[classX][dimX] += (double) read_float(fp);

  /* --------------------- */
  /*  load XX matrix accu  */
  /* --------------------- */
  for (classX=0; classX<accu->classN; classX++)
    for (dimX=0; dimX<accu->dimN; dimX++)
      accu->xx[classX][dimX] += (double) read_float(fp);

  accu->dist   += read_float(fp);
  accu->lh     += read_float(fp);
  accu->trainN += read_int(fp);
  
  return TCL_OK;
}



/* ------------------------------------------------------------------------- */
/*  gaussClassEval  -  compute exp(-0.5*Mahalanobis-dist) for each Gaussian  */
/* ------------------------------------------------------------------------- */
int  gaussClassEval (GaussClass *gc, float *pattern, int dimN,
		     double *act, int classN) {

  IndexList          *ilist=NULL;
  register int       gaussX,dimX,k;
  register double    sum;
  register double    *iv,*m;
  register float     *p;

  assert(!(dimN & 1));
  assert(dimN == gc->dimN);
  assert(classN == gc->classN);
  
  if (gc->tree) {

    /* ---------------------------------------------------------- */
    /*  use BSP tree to compute set of Gaussians to be evaluated  */
    /* ---------------------------------------------------------- */
    ilist = bspTreeUse (gc->tree,pattern);

    /* ----------------------------------- */
    /*  reset activations for all classes  */
    /* ----------------------------------- */
    for (gaussX=0; gaussX<gc->classN; gaussX++) act[gaussX] = 0.0;
    
    /* ------------------------------------------- */
    /*  compute activations for selected Gaussians */
    /* ------------------------------------------- */
    for (k=0; k<ilist->listN; k++) {
      gaussX = ilist->list[k];
      sum = 0.0;
      for (dimX=0, iv=gc->ivar[gaussX], m=gc->mean[gaussX], p=pattern;
           dimX<gc->dimN;
           dimX+=2, iv+=2, p+=2, m+=2)
        sum += iv[0]*square(p[0]-m[0]) + iv[1]*square(p[1]-m[1]);
      act[gaussX] = exp(-0.5*sum);
    }

  } else {

    /* ---------------------------------------------------------------- */
    /*  no BSP tree available -- compute activations for all Gaussians  */
    /* ---------------------------------------------------------------- */
    for (gaussX=0; gaussX<gc->classN; gaussX++) {
      sum = 0.0;
      for (dimX=0, iv=gc->ivar[gaussX], m=gc->mean[gaussX], p=pattern;
           dimX<gc->dimN;
           dimX+=2, iv+=2, p+=2, m+=2)
        sum += iv[0]*square(p[0]-m[0]) + iv[1]*square(p[1]-m[1]);
      act[gaussX] = exp(-0.5*sum);
    }
  }    
  
  return TCL_OK;
}



/* ----------------------------------------------------------------------- */
/*  gaussClassPost  -  compute posterior class probabilities               */
/*                     returns the likelihood of the Gaussian mixture      */
/* ----------------------------------------------------------------------- */
double  gaussClassPost (GaussClass *gc, float *pattern, int dimN,
	   	        double *act, int classN) {

  IndexList          *ilist=NULL;
  register int       gaussX,dimX,k;
  register double    sum,maxExpo,expo,post,factor;
  register double    *iv,*m;
  register float     *p;

  assert(!(dimN & 1));
  assert(dimN == gc->dimN);
  assert(classN == gc->classN);
  
  if (gc->tree) {

    /* ---------------------------------------------------------- */
    /*  use BSP tree to compute set of Gaussians to be evaluated  */
    /* ---------------------------------------------------------- */
    ilist = bspTreeUse (gc->tree,pattern);

    /* ----------------------------------- */
    /*  reset activations for all classes  */
    /* ----------------------------------- */
    for (gaussX=0; gaussX<gc->classN; gaussX++) act[gaussX] = 0.0;

    /* ----------------------------------- */
    /*  1.) compute Mahalanobis distances  */
    /* ----------------------------------- */
    maxExpo = -MAXVAL;
    for (k=0; k<ilist->listN; k++) {
      gaussX = ilist->list[k];
      sum = 0.0;
      for (dimX=0, iv=gc->ivar[gaussX], m=gc->mean[gaussX], p=pattern;
           dimX<gc->dimN;
           dimX+=2, iv+=2, p+=2, m+=2)
        sum += iv[0]*square(p[0]-m[0]) + iv[1]*square(p[1]-m[1]);
      expo = -0.5*sum;
      if (expo > maxExpo) maxExpo = expo;
      act[gaussX] = expo;
    }

    /* ---------------------------------------------------------------- */
    /*  2.) compute numerator of posterior using normalized Mahal.dist  */
    /* ---------------------------------------------------------------- */
    sum = 0.0;
    for (k=0; k<ilist->listN; k++) {
      gaussX = ilist->list[k]; 
      post = gc->pre[gaussX] * exp(act[gaussX] - maxExpo);
      sum += post;
      act[gaussX] = post;
    }

    /* --------------------------------- */
    /*  3.) normalize to get posteriors  */ 
    /* --------------------------------- */
    if (sum > 0.0) factor = 1.0/sum; else factor = 0.0;
    for (k=0; k<ilist->listN; k++) {
      gaussX = ilist->list[k];
      act[gaussX] *= factor;
    }
    
  } else {

    /* ---------------------------------------------------------------- */
    /*  no BSP tree available -- compute activations for all Gaussians  */
    /* ---------------------------------------------------------------- */

    /* ----------------------------------- */
    /*  1.) compute Mahalanobis distances  */
    /* ----------------------------------- */
    maxExpo = -MAXVAL;
    for (gaussX=0; gaussX<gc->classN; gaussX++) {
      sum = 0.0;
      for (dimX=0, iv=gc->ivar[gaussX], m=gc->mean[gaussX], p=pattern;
           dimX<gc->dimN;
           dimX+=2, iv+=2, p+=2, m+=2)
        sum += iv[0]*square(p[0]-m[0]) + iv[1]*square(p[1]-m[1]);
      expo = -0.5*sum;
      if (expo > maxExpo) maxExpo = expo;
      act[gaussX] = expo;
    }

    /* ---------------------------------------------------------------- */
    /*  2.) compute numerator of posterior using normalized Mahal.dist  */
    /* ---------------------------------------------------------------- */
    sum = 0.0;
    for (gaussX=0; gaussX<gc->classN; gaussX++) {
      post = gc->pre[gaussX] * exp(act[gaussX] - maxExpo);
      sum += post;
      act[gaussX] = post;
    }

    /* --------------------------------- */
    /*  3.) normalize to get posteriors  */ 
    /* --------------------------------- */
    if (sum > 0.0) factor = 1.0/sum; else factor = 0.0;
    for (gaussX=0; gaussX<gc->classN; gaussX++)
      act[gaussX] *= factor;
  }    
  
  return (sum*exp(maxExpo));
}



/* ----------------------------------------------------------------------- */
/*  gaussClassAccuExtract                                                  */
/* ----------------------------------------------------------------------- */
int  gaussClassAccuExtract (GaussClass *gc, float *pattern, int dimN) {

  GaussClassAccu            *accu;
  register int              blockN,classX,dimX;
  register double           prob;

  accu = gc->accu;
  assert(accu);

  /* ---------------------------------------------------- */
  /*  compute the probability of extracting at this time  */
  /* ---------------------------------------------------- */
  blockN = 1 + (accu->trainN / accu->classN);
  prob   = 1.0 / (double) blockN;

  /* --------------------------------------------- */
  /*  enable extraction based on this probability  */
  /* --------------------------------------------- */
  if (myRandom(0,1) <= prob) {

    /* -------------------------- */
    /*  extract a feature vector  */
    /* -------------------------- */
    classX = (accu->trainN % accu->classN);
    for (dimX=0; dimX<dimN; dimX++)
      accu->x[classX][dimX] = pattern[dimX];
  }

  accu->trainN++;
  
  return TCL_OK;
}



/* ----------------------------------------------------------------------- */
/*  gaussClassAccuCluster                                                  */
/* ----------------------------------------------------------------------- */
int  gaussClassAccuCluster (GaussClass *gc, float *pattern, int dimN,
			    double weight) {

  GaussClassAccu            *accu;
  register int              classX,dimX,minIdx;
  register double           minDist,dist;
  double                    *m;
  float                     *p;
  
  accu = gc->accu;
  assert(accu);
  assert(!(dimN & 1));

  /* ---------------------------------------------------------- */
  /*  determine the nearest neighbor using Euclidean distances  */
  /* ---------------------------------------------------------- */
  minDist = MAXVAL;
  minIdx  = 0;
  for (classX=0; classX<accu->classN; classX++) {
    dist = 0.0;
    p = pattern;
    m = gc->mean[classX];
    for (dimX=0; dimX<dimN; dimX+=2, p+=2, m+=2) {
      dist += square(p[0] - m[0]) + square(p[1] - m[1]);
      if (dist > minDist) break;
    }
    if (dist < minDist) {
      minDist = dist;
      minIdx  = classX;
    }
  }

  /* --------------------------------------------------------- */
  /*  accu the current vector into the nearest neighbor class  */
  /* --------------------------------------------------------- */
  accu->a[minIdx] += weight;
  for (dimX=0; dimX<dimN; dimX++) {
    accu->x[minIdx][dimX]  += weight * pattern[dimX];
    accu->xx[minIdx][dimX] += weight * square(pattern[dimX]);
  }
  
  accu->dist += minDist;
  accu->trainN++;
  
  return TCL_OK;
}



/* ----------------------------------------------------------------------- */
/*  gaussClassAccuML                                                       */
/* ----------------------------------------------------------------------- */
int  gaussClassAccuML (GaussClass *gc, float *pattern, int dimN,
		       double weight) {

  GaussClassAccu            *accu;
  register int              classX,dimX;
  static double             *h = NULL;
  static int                hN = 0;
  register double           w,lh;
  
  accu = gc->accu;
  assert(accu);

  /* --------------------------------- */
  /*  compute posterior probabilities  */
  /* --------------------------------- */
  if (gc->classN > hN) {
    if (h != NULL) free(h);
    h = (double *) malloc (sizeof(double) * gc->classN);
    hN = gc->classN;
  }
  
  lh = gaussClassPost(gc,pattern,dimN,h,gc->classN);

  /* --------------------------------- */
  /*  accumulate according to weights  */
  /* --------------------------------- */
  for (classX=0; classX<accu->classN; classX++) {
    if (h[classX] > 0.0) {
      w = weight * h[classX];
      accu->a[classX] += w;
      for (dimX=0; dimX<dimN; dimX++) {
        accu->x[classX][dimX]  += w * pattern[dimX];
        accu->xx[classX][dimX] += w * square(pattern[dimX]);
      }
    }
  }

  /* -------------------------------------- */
  /*  accumulate log likelihood of mixture  */
  /* -------------------------------------- */
  if (lh < MINVAL) lh = MINVAL;
  accu->lh += log(lh);
  accu->trainN++;
  
  return TCL_OK;
}



/* ----------------------------------------------------------------------- */
/*  gaussClassAccuSoftEM                                                   */
/* ----------------------------------------------------------------------- */
int  gaussClassAccuSoftEM (GaussClass *gc, float *pattern, int dimN,
			   double *target, int classN, double weight) {

  GaussClassAccu            *accu;
  register int              classX,dimX;
  register double           w;
  
  accu = gc->accu;
  assert(accu);

  /* --------------------------------- */
  /*  accumulate according to weights  */
  /* --------------------------------- */
  for (classX=0; classX<classN; classX++) {
    w = weight * target[classX];
    accu->a[classX] += w;
    for (dimX=0; dimX<dimN; dimX++) {
      accu->x[classX][dimX]  += w * pattern[dimX];
      accu->xx[classX][dimX] += w * square(pattern[dimX]);
    }
  }

  accu->trainN++;

  return TCL_OK;
}



/* ----------------------------------------------------------------------- */
/*  gaussClassAccuHardEM                                                   */
/* ----------------------------------------------------------------------- */
int  gaussClassAccuHardEM (GaussClass *gc, float *pattern, int dimN,
			   int targetX, int classN, double weight) {

  GaussClassAccu            *accu;
  register int              dimX;
  
  accu = gc->accu;
  assert(accu);

  /* -------------------------------------------- */
  /*  accumulate according to weight and targetX  */
  /* -------------------------------------------- */
  accu->a[targetX] += weight;
  for (dimX=0; dimX<dimN; dimX++) {
    accu->x[targetX][dimX]  += weight * pattern[dimX];
    accu->xx[targetX][dimX] += weight * square(pattern[dimX]);
  }

  accu->trainN++;

  return TCL_OK;
}



/* ----------------------------------------------------------------------- */
/*  gaussClassUpdateExtract                                                */
/* ----------------------------------------------------------------------- */
int  gaussClassUpdateExtract (GaussClass *gc) {

  GaussClassAccu             *accu;
  register int               classX,dimX;

  accu = gc->accu;
  assert(accu);

  if (accu->trainN == 0) {
    WARN("no training information accumulated\n");
    return TCL_OK;
  }
  
  for (classX=0; classX<accu->classN; classX++) {
    gc->alpha[classX] = 1.0 / ((double) accu->classN);
    for (dimX=0; dimX<accu->dimN; dimX++) {
      gc->mean[classX][dimX] = accu->x[classX][dimX];
      gc->ivar[classX][dimX] = 1.0;
    }
  }

  return  gaussClassPre(gc);
}



/* ----------------------------------------------------------------------- */
/*  gaussClassUpdateMixture                                                */
/* ----------------------------------------------------------------------- */
int  gaussClassUpdateMixture (GaussClass *gc) {

  GaussClassAccu             *accu;
  register int               classX,dimX;
  register double            alphaSum,count,ex,exx,var;
  
  accu = gc->accu;
  assert(accu);

  if (accu->trainN == 0) {
    WARN("no training information accumulated\n");
    return TCL_OK;
  }

  alphaSum = 0.0;
  for (classX=0; classX<accu->classN; classX++)
    alphaSum += accu->a[classX];

  if (alphaSum > 0.0) {
    for (classX=0; classX<accu->classN; classX++) {
      count = accu->a[classX];
      gc->alpha[classX] = count / alphaSum;
      if (count > 0.0) {
        for (dimX=0; dimX<accu->dimN; dimX++) {
  	  ex  = accu->x[classX][dimX]  / count;
          gc->mean[classX][dimX] = ex;
	  exx = accu->xx[classX][dimX] / count;
	  var = exx - square(ex);
	  if (var < MINVAL) {
	    WARN("insufficient data to estimate variances reliably\n");
	  } else
            gc->ivar[classX][dimX] = 1.0 / var;
        }
      }
    }
  }

  if (accu->dist > 0.0) accu->dist /= accu->trainN;
  
  return  gaussClassPre(gc);
}

 

/* ======================================================================= */
/*                     (2) janus3 classifier functions                     */
/* ======================================================================= */



/* ----------------------------------------------------------------------- */
/*  gaussClassCreateClf                                                    */
/* ----------------------------------------------------------------------- */
ClientData  gaussClassCreateClf (int classN, int dimN, int argc, char *argv[]) {

  int                ac         = argc-1;
  int                trainMode  = 0;
  int                weightMode = 1;

  if ( itfParseArgv( argv[0], &ac, argv+1, 0,
      "-trainMode",  ARGV_INT,   NULL, &trainMode,  NULL, "training modus",
      "-weightMode", ARGV_INT,   NULL, &weightMode, NULL, "weight modus",
  NULL) != TCL_OK) return NULL;

  return (ClientData) gaussClassCreate(classN,dimN,trainMode,weightMode);
}



/* ----------------------------------------------------------------------- */
/*  gaussClassCopyClf                                                      */
/* ----------------------------------------------------------------------- */
ClientData  gaussClassCopyClf (ClientData cd, double perturbe) {

  return (ClientData) gaussClassCopy((GaussClass *) cd,perturbe);
}



/* ----------------------------------------------------------------------- */
/*  gaussClassInitClf                                                      */
/* ----------------------------------------------------------------------- */
int  gaussClassInitClf (ClientData cd) {

  return gaussClassInit((GaussClass *) cd);
}



/* ----------------------------------------------------------------------- */
/*  gaussClassFreeClf                                                      */
/* ----------------------------------------------------------------------- */
int  gaussClassFreeClf (ClientData cd) {

  return gaussClassFree((GaussClass *) cd);
}



/* ----------------------------------------------------------------------- */
/*  gaussClassSaveClf                                                      */
/* ----------------------------------------------------------------------- */
int  gaussClassSaveClf (ClientData cd, FILE *fp) {

  return gaussClassSave((GaussClass *) cd,fp);
}



/* ----------------------------------------------------------------------- */
/*  gaussClassLoadClf                                                      */
/* ----------------------------------------------------------------------- */
ClientData  gaussClassLoadClf (FILE *fp) {

  return (ClientData)gaussClassLoad(NULL,fp);
}



/* ----------------------------------------------------------------------- */
/*  gaussClassCreateAccusClf                                               */
/* ----------------------------------------------------------------------- */
int  gaussClassCreateAccusClf (ClientData cd) {

  return gaussClassCreateAccus((GaussClass *) cd);
}



/* ----------------------------------------------------------------------- */
/*  gaussClassFreeAccusClf                                                 */
/* ----------------------------------------------------------------------- */
int  gaussClassFreeAccusClf (ClientData cd) {

  return gaussClassFreeAccus((GaussClass *) cd);
}



/* ----------------------------------------------------------------------- */
/*  gaussClassClearAccusClf                                                */
/* ----------------------------------------------------------------------- */
int  gaussClassClearAccusClf (ClientData cd) {

  return gaussClassClearAccus((GaussClass *) cd);
}



/* ----------------------------------------------------------------------- */
/*  gaussClassSaveAccusClf                                                 */
/* ----------------------------------------------------------------------- */
int  gaussClassSaveAccusClf (ClientData cd, FILE *fp) {

  return gaussClassSaveAccus((GaussClass *) cd,fp);
}



/* ----------------------------------------------------------------------- */
/*  gaussClassLoadAccusClf                                                 */
/* ----------------------------------------------------------------------- */
int  gaussClassLoadAccusClf (ClientData cd, FILE *fp) {

  return gaussClassLoadAccus((GaussClass *) cd,fp);
}



/* ----------------------------------------------------------------------- */
/*  gaussClassConfigureClf                                                 */
/* ----------------------------------------------------------------------- */
int  gaussClassConfigureClf (ClientData cd, int argc, char *argv[]) {

  register int      k;
  char              *var,*val;

  if ((argc % 2) != 1) {
    ERROR("invalid number of parameters\n");
    return TCL_ERROR;
  }
  
  for (k=1; k<argc; k+=2) {
    var = argv[k]+1;
    val = argv[k+1];
    if (gaussClassConfigureItf(cd,var,val) == TCL_ERROR) return TCL_ERROR;
  }
  
  return TCL_OK;
}



/* ----------------------------------------------------------------------- */
/*  gaussClassScoreClf                                                     */
/* ----------------------------------------------------------------------- */
int  gaussClassScoreClf (ClientData cd, float *pattern, int dimN, double *score,
		       int classN) {
 
  return gaussClassPost((GaussClass *) cd,pattern,dimN,score,classN);
}



/* ----------------------------------------------------------------------- */
/*  gaussClassAccuSoftClf                                                  */
/* ----------------------------------------------------------------------- */
int  gaussClassAccuSoftClf (ClientData cd, float *pattern, int dimN,
			   double *target, double *act,
			   int classN, double g, double h) {

  GaussClass        *gaussClass = (GaussClass *) cd;

  if (gaussClass->trainMode == 0) return TCL_OK;

  switch (gaussClass->trainMode) {
    case 1:  /* ----------------- */
             /*  extract samples  */
             /* ----------------- */
             gaussClassAccuExtract(gaussClass,pattern,dimN);
	     break;
	     
    case 2:  /* -------------------- */
	     /*  k-means clustering  */
	     /* -------------------- */
	     gaussClassAccuCluster(gaussClass,pattern,dimN,g);
	     break;

    case 3:  /* ------------------------------------ */
	     /*  ML mixture of Gaussians estimation  */
	     /* ------------------------------------ */
             gaussClassAccuML(gaussClass,pattern,dimN,g);
	     break;
	     
    case 4:  /* --------------------------------- */
	     /*  EM estimation for Gaussian gate  */
	     /* --------------------------------- */
             if (gaussClass->weightMode) 
               gaussClassAccuSoftEM(gaussClass,pattern,dimN,target,classN,h);
	     else
               gaussClassAccuSoftEM(gaussClass,pattern,dimN,target,classN,g);
	     break;
	     
    default: ERROR("Unknown trainMode = %d\n",gaussClass->trainMode);
	     return TCL_ERROR;
  }
  
  return TCL_OK;
}



/* ----------------------------------------------------------------------- */
/*  gaussClassAccuHardClf                                                  */
/* ----------------------------------------------------------------------- */
int  gaussClassAccuHardClf (ClientData cd, float *pattern, int dimN,
			   int targetX, double *act,
			   int classN, double g, double h) {

  GaussClass        *gaussClass = (GaussClass *) cd;

  if (gaussClass->trainMode == 0) return TCL_OK;

  switch (gaussClass->trainMode) {
    case 4:  /* ----------------------------------- */
	     /*  EM estimation for Gaussian expert  */
	     /* ----------------------------------- */
             if (gaussClass->weightMode) 
               gaussClassAccuHardEM(gaussClass,pattern,dimN,targetX,classN,h);
	     else
               gaussClassAccuHardEM(gaussClass,pattern,dimN,targetX,classN,g);
	     break;

    default: ERROR("Invalid or unknown trainMode = %d\n",gaussClass->trainMode);
	     return TCL_ERROR;
  }
  
  return TCL_OK;
}



/* ----------------------------------------------------------------------- */
/*  gaussClassUpdateClf                                                    */
/* ----------------------------------------------------------------------- */
int  gaussClassUpdateClf (ClientData cd) {

  GaussClass        *gaussClass = (GaussClass *) cd;

  if (gaussClass->trainMode == 0) return TCL_OK;

  switch (gaussClass->trainMode) {
    case 1:  /* ----------------- */
	     /*  extract samples  */
	     /* ----------------- */
	     gaussClassUpdateExtract(gaussClass);
	     break;
    
    case 2:  /* -------------------- */
	     /*  k-means clustering  */
	     /* -------------------- */
	     gaussClassUpdateMixture(gaussClass);
	     break;

    case 3:  /* ------------------------------------ */
	     /*  ML mixture of Gaussians estimation  */
	     /* ------------------------------------ */
             gaussClassUpdateMixture(gaussClass);
	     break;
	     
    case 4:  /* --------------------------------- */
	     /*  EM estimation for Gaussian gate  */
	     /* --------------------------------- */
             gaussClassUpdateMixture(gaussClass);
	     break;

    default: ERROR("Unknown trainMode = %d\n",gaussClass->trainMode);
	     return TCL_ERROR;
  }

  /* -------------------------- */
  /*  remove obsolete BSP tree  */
  /* -------------------------- */
  if (gaussClass->tree) {
    INFO("Removing obsolete BSP tree\n");
    bspTreeFree(gaussClass->tree);
    gaussClass->tree = NULL;
  }
  
  return TCL_OK;
}




/* ====================================================================== */
/*                        (3) janus3 itf functions                        */
/* ====================================================================== */



/* ----------------------------------------------------------------------- */
/*  gaussClassAccuPutsItf                                                  */
/* ----------------------------------------------------------------------- */
int  gaussClassAccuPutsItf (ClientData cd, int argc, char *argv[]) {

  GaussClassAccu       *accu = (GaussClassAccu *) cd;
  register int         classX,dimX;
  
  itfAppendResult("{ a { ");
  for (classX=0; classX<accu->classN; classX++)
    itfAppendResult("%f ",accu->a[classX]);
  itfAppendResult("} } ");
  
  itfAppendResult("{ X { ");
  for (classX=0; classX<accu->classN; classX++) {
    itfAppendResult("{ ");
    for (dimX=0; dimX<accu->dimN; dimX++)
      itfAppendResult("%f ",accu->x[classX][dimX]);
    itfAppendResult("} ");
  }
  itfAppendResult("} } ");
  
  itfAppendResult("{ XX { ");
  for (classX=0; classX<accu->classN; classX++) {
    itfAppendResult("{ ");
    for (dimX=0; dimX<accu->dimN; dimX++)
      itfAppendResult("%f ",accu->xx[classX][dimX]);
    itfAppendResult("} ");
  }
  itfAppendResult("} } ");

  itfAppendResult("{dist %f} {lh %f} {trainN %d} ",
		  accu->dist,accu->lh,accu->trainN);
  
  return TCL_OK;
}



/* ---------------------------------------------------------------------- */
/*  gaussClassCreateItf                                                   */
/* ---------------------------------------------------------------------- */
ClientData  gaussClassCreateItf (ClientData cd, int argc, char *argv[]) {

  int                ac;
  int                classN,dimN;

  ac = (argc < 3)?(argc-1):2;
  if ( itfParseArgv( argv[0], &ac, argv+1, 1,
      "<classN>",    ARGV_INT,   NULL, &classN,     NULL, "number of output classes",
      "<dimN>",      ARGV_INT,   NULL, &dimN,       NULL, "feature dimension",
  NULL) != TCL_OK) return NULL;

  return  gaussClassCreateClf(classN,dimN,argc-2,argv+2);
}



/* ----------------------------------------------------------------------- */
/*  gaussClassFreeItf                                                      */
/* ----------------------------------------------------------------------- */
int  gaussClassFreeItf (ClientData cd) {

  return gaussClassFree((GaussClass *) cd);
}



/* ----------------------------------------------------------------------- */
/*  gaussClassPutsItf                                                      */
/* ----------------------------------------------------------------------- */
int  gaussClassPutsItf (ClientData cd, int argc, char *argv[]) {

  GaussClass        *gaussClass = (GaussClass *) cd;
  int               ac = argc-1;
  int               preX   = -1;
  int               alphaX = -1;
  int               meanX  = -1;
  int               ivarX  = -1;
  int               dimX;
  
  if ( itfParseArgv( argv[0], &ac, argv+1, 0,
      "-preX",   ARGV_INT, NULL, &preX,     NULL, "Gaussian pre-factor for classX",
      "-alphaX", ARGV_INT, NULL, &alphaX,   NULL, "prior probability for classX",
      "-meanX",  ARGV_INT, NULL, &meanX,    NULL, "mean vector for classX",
      "-ivarX",  ARGV_INT, NULL, &ivarX,    NULL, "inverse variance vector for classX",
  NULL) != TCL_OK) return TCL_ERROR;

  if (((preX   < 0) || (preX   >= gaussClass->classN)) &&
      ((alphaX < 0) || (alphaX >= gaussClass->classN)) &&
      ((meanX  < 0) || (meanX  >= gaussClass->classN)) &&
      ((ivarX  < 0) || (ivarX  >= gaussClass->classN))) {

    itfAppendResult("{classN %d} {dimN %d} {trainMode %d} {weightMode %d} ",
		    gaussClass->classN,gaussClass->dimN,gaussClass->trainMode,
		    gaussClass->weightMode);
  } else {

    if (preX >= 0)
      itfAppendResult("{ pre %f } ",gaussClass->pre[preX]);
    
    if (alphaX >= 0)
      itfAppendResult("{ alpha %f } ",gaussClass->alpha[alphaX]);
    
    if (meanX >= 0) {
      itfAppendResult("{ mean { ");
      for (dimX=0; dimX<gaussClass->dimN; dimX++)
        itfAppendResult("%f ",gaussClass->mean[meanX][dimX]);
      itfAppendResult("} } ");
    }
  
    if (ivarX >= 0) {
      itfAppendResult("{ ivar { ");
      for (dimX=0; dimX<gaussClass->dimN; dimX++)
        itfAppendResult("%f ",gaussClass->ivar[ivarX][dimX]);
      itfAppendResult("} } ");
    }
  }
  
  return TCL_OK;
}



/* ----------------------------------------------------------------------- */
/*  gaussClassMeansItf                                                     */
/* ----------------------------------------------------------------------- */
int  gaussClassMeansItf (ClientData cd, int argc, char *argv[]) {

  GaussClass        *gaussClass = (GaussClass *) cd;
  int               ac = argc-1;
  register int      classX,dimX;
  DMatrix           *matrix;

  if ( itfParseArgv( argv[0], &ac, argv+1, 0,
      "<DMatrix>", ARGV_OBJECT, NULL, &matrix, "DMatrix", "matrix of mean vectors",
  NULL) != TCL_OK) return TCL_ERROR;

  if ((matrix->m != gaussClass->classN) || (matrix->n != gaussClass->dimN)) {
    ERROR("invalid matrix dimensions (%d,%d), must be (%d,%d)\n",
	  matrix->m,matrix->n,gaussClass->classN,gaussClass->dimN);
    return TCL_ERROR;
  }
  
  for (classX=0; classX<gaussClass->classN; classX++)
    for (dimX=0; dimX<gaussClass->dimN; dimX++)
      gaussClass->mean[classX][dimX] = matrix->matPA[classX][dimX];
    
  return gaussClassPre(gaussClass);
}



/* ----------------------------------------------------------------------- */
/*  gaussClassIvarsItf                                                     */
/* ----------------------------------------------------------------------- */
int  gaussClassIvarsItf (ClientData cd, int argc, char *argv[]) {

  GaussClass        *gaussClass = (GaussClass *) cd;
  int               ac = argc-1;
  register int      classX,dimX;
  DMatrix           *matrix;

  if ( itfParseArgv( argv[0], &ac, argv+1, 0,
      "<DMatrix>", ARGV_OBJECT, NULL, &matrix, "DMatrix", "matrix of inverse variance vectors",
  NULL) != TCL_OK) return TCL_ERROR;

  if ((matrix->m != gaussClass->classN) || (matrix->n != gaussClass->dimN)) {
    ERROR("invalid matrix dimensions (%d,%d), must be (%d,%d)\n",
	  matrix->m,matrix->n,gaussClass->classN,gaussClass->dimN);
    return TCL_ERROR;
  }
  
  for (classX=0; classX<gaussClass->classN; classX++)
    for (dimX=0; dimX<gaussClass->dimN; dimX++)
      gaussClass->ivar[classX][dimX] = matrix->matPA[classX][dimX];

  return gaussClassPre(gaussClass);
}



/* ----------------------------------------------------------------------- */
/*  gaussClassConfigureItf                                                 */
/* ----------------------------------------------------------------------- */
int  gaussClassConfigureItf (ClientData cd, char *var, char *val) {

  GaussClass         *gaussClass = (GaussClass *) cd;

  if (! var) {
    ITFCFG(gaussClassConfigureItf,cd,"classN");
    ITFCFG(gaussClassConfigureItf,cd,"dimN");
    ITFCFG(gaussClassConfigureItf,cd,"trainMode");
    ITFCFG(gaussClassConfigureItf,cd,"weightMode");
    return TCL_OK;
  }
  ITFCFG_Int   (var,val,"classN",     gaussClass->classN ,     1);
  ITFCFG_Int   (var,val,"dimN",       gaussClass->dimN ,       1);
  ITFCFG_Int   (var,val,"trainMode",  gaussClass->trainMode ,  0);
  ITFCFG_Int   (var,val,"weightMode", gaussClass->weightMode , 0);

  return TCL_OK;
}



/* ----------------------------------------------------------------------- */
/*  gaussClassAccessItf                                                     */
/* ----------------------------------------------------------------------- */
ClientData  gaussClassAccessItf (ClientData cd, char *name, TypeInfo **ti) {

  GaussClass         *gaussClass = (GaussClass *) cd;  
  
  if (*name=='.') {
    if (name[1]=='\0') {
      if (gaussClass->accu) itfAppendElement("accu");
    }
    else {
      if ((!strcmp(name+1,"accu")) && (gaussClass->accu)) {
        *ti=itfGetType("GaussAccu");
        return (ClientData)(gaussClass->accu);
      }
    }
  }
  *ti=NULL;
  return NULL;
}



/* ----------------------------------------------------------------------- */
/*  gaussClassCopyItf                                                      */
/* ----------------------------------------------------------------------- */
int  gaussClassCopyItf (ClientData cd, int argc, char *argv[]) {

  GaussClass        *gaussClass = (GaussClass *) cd;
  int               ac = argc-1;
  GaussClass        *source,*target;
  float             perturbe = 0.0;
  int               classX;
  
  if ( itfParseArgv( argv[0], &ac, argv+1, 0,
      "<source>",  ARGV_OBJECT, NULL, &source,    "Gauss", "source Gauss",
      "-perturbe", ARGV_FLOAT,  NULL, &perturbe,  NULL,   "additional random perturberation",
  NULL) != TCL_OK) return TCL_ERROR;  
  
  target = gaussClassCopy(source,perturbe);

  /* -------------------------------- */
  /*  free existing Gauss classifier  */
  /* -------------------------------- */
  free(gaussClass->pre);
  free(gaussClass->alpha);

  for (classX=0; classX<gaussClass->classN; classX++) {
    free(gaussClass->mean[classX]);
    free(gaussClass->ivar[classX]);
  }
  free(gaussClass->mean);
  free(gaussClass->ivar);

  gaussClassFreeAccus(gaussClass);
  if (gaussClass->tree) bspTreeFree(gaussClass->tree);

  /* -------------------------------- */
  /*  copy new Gauss into gaussClass  */
  /* -------------------------------- */
  gaussClass->classN     = target->classN;
  gaussClass->dimN       = target->dimN;
  gaussClass->trainMode  = target->trainMode;
  gaussClass->weightMode = target->weightMode;
  gaussClass->pre        = target->pre;
  gaussClass->alpha      = target->alpha;
  gaussClass->mean       = target->mean;
  gaussClass->ivar       = target->ivar;
  gaussClass->accu       = NULL;
  gaussClass->tree       = NULL;

  free(target);
  return TCL_OK;
}



/* ----------------------------------------------------------------------- */
/*  gaussClassInitItf                                                      */
/* ----------------------------------------------------------------------- */
int  gaussClassInitItf (ClientData cd, int argc, char *argv[]) {

  return  gaussClassInit((GaussClass *) cd);
}



/* ----------------------------------------------------------------------- */
/*  gaussClassSaveItf                                                      */
/* ----------------------------------------------------------------------- */
int  gaussClassSaveItf (ClientData cd, int argc, char *argv[]) {

  GaussClass            *gaussClass = (GaussClass *) cd;
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
  
  if (gaussClassSave(gaussClass,fp) == TCL_ERROR) return TCL_ERROR;
  fileClose(filename,fp);
  
  return TCL_OK;
}



/* ----------------------------------------------------------------------- */
/*  gaussClassLoadItf                                                      */
/* ----------------------------------------------------------------------- */
int  gaussClassLoadItf (ClientData cd, int argc, char *argv[]) {

  GaussClass            *gaussClass = (GaussClass *) cd;
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
  
  if (gaussClassLoad(gaussClass,fp) == NULL) return TCL_ERROR;
  fileClose(filename,fp);

  return TCL_OK;
}



/* ----------------------------------------------------------------------- */
/*  gaussClassMakeBbiItf                                                   */
/* ----------------------------------------------------------------------- */
int  gaussClassMakeBbiItf (ClientData cd, int argc, char *argv[]) {

  GaussClass            *gaussClass = (GaussClass *) cd;
  BoxSet                *boxSet;
  int                   ac         = argc-1;
  int                   depth      = 10;
  float                 thresh     = 0.1;
  
  if ( itfParseArgv( argv[0], &ac, argv+1, 0,
      "-depth", ARGV_INT,   NULL, &depth,  NULL, "depth of BBI tree",
      "-T",     ARGV_FLOAT, NULL, &thresh, NULL, "threshold for box generating",
  NULL) != TCL_OK) return TCL_ERROR;

  if (gaussClass->tree) {
    WARN("replacing existing BBI tree\n");
    bspTreeFree(gaussClass->tree);
  }

  printO("Creating BBI tree (depth=%d, T=%f)\n",depth,thresh);
  boxSet = boxSetCreate(gaussClass->classN,gaussClass->dimN);
  boxSetInit(boxSet,gaussClass,thresh);
  gaussClass->tree = bspTreeCreate(depth,boxSet);
  boxSetFree(boxSet);

  return TCL_OK;
}



/* ----------------------------------------------------------------------- */
/*  gaussClassFreeBbiItf                                                   */
/* ----------------------------------------------------------------------- */
int  gaussClassFreeBbiItf (ClientData cd, int argc, char *argv[]) {

  GaussClass            *gaussClass = (GaussClass *) cd;
  
  if (gaussClass->tree) {
    bspTreeFree(gaussClass->tree);
    gaussClass->tree = NULL;
  }
  
  return  TCL_OK;
}

  
  
/* ----------------------------------------------------------------------- */
/*  gaussClassCreateAccusItf                                               */
/* ----------------------------------------------------------------------- */
int  gaussClassCreateAccusItf (ClientData cd, int argc, char *argv[]) {

  return  gaussClassCreateAccus((GaussClass *) cd);
}



/* ----------------------------------------------------------------------- */
/*  gaussClassFreeAccusItf                                                 */
/* ----------------------------------------------------------------------- */
int  gaussClassFreeAccusItf (ClientData cd, int argc, char *argv[]) {

  return  gaussClassFreeAccus((GaussClass *) cd);
}



/* ----------------------------------------------------------------------- */
/*  gaussClassClearAccusItf                                                */
/* ----------------------------------------------------------------------- */
int  gaussClassClearAccusItf (ClientData cd, int argc, char *argv[]) {

  return  gaussClassClearAccus((GaussClass *) cd);
}



/* ----------------------------------------------------------------------- */
/*  gaussClassSaveAccusItf                                                 */
/* ----------------------------------------------------------------------- */
int  gaussClassSaveAccusItf (ClientData cd, int argc, char *argv[]) {

  GaussClass            *gaussClass = (GaussClass *) cd;
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
  
  if (gaussClassSaveAccus(gaussClass,fp) == TCL_ERROR) return TCL_ERROR;
  fileClose(filename,fp);
  
  return TCL_OK;
}



/* ----------------------------------------------------------------------- */
/*  gaussClassLoadAccusItf                                                 */
/* ----------------------------------------------------------------------- */
int  gaussClassLoadAccusItf (ClientData cd, int argc, char *argv[]) {

  GaussClass            *gaussClass = (GaussClass *) cd;
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
  
  if (gaussClassLoadAccus(gaussClass,fp) == TCL_ERROR) return TCL_ERROR;
  fileClose(filename,fp);

  return TCL_OK;
}



/* ----------------------------------------------------------------------- */
/*  gaussClassScoreItf                                                     */
/* ----------------------------------------------------------------------- */
int  gaussClassScoreItf (ClientData cd, int argc, char *argv[]) {

  GaussClass      *gaussClass  = (GaussClass *) cd;
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

  score = (double *) malloc (sizeof(double) * gaussClass->classN);
  gaussClassScoreClf(cd,pattern,gaussClass->dimN,score,gaussClass->classN);
  for (classX=0; classX<gaussClass->classN; classX++)
    itfAppendResult("%f ",score[classX]);
  free(score);
  
  return TCL_OK;
}



/* ----------------------------------------------------------------------- */
/*  gaussClassAccuHardItf                                                  */
/* ----------------------------------------------------------------------- */
int  gaussClassAccuHardItf (ClientData cd, int argc, char *argv[]) {

  GaussClass      *gaussClass  = (GaussClass *) cd;
  int            ac          = argc-1;
  int            frameX      = 0;
  FeatureSet     *fes        = NULL;
  int            featIdx     = 0;
  float          *pattern    = NULL;
  int            targetX     = -1;
  float          weight      = 1.0;
  

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

  gaussClassAccuHardClf(cd,pattern,gaussClass->dimN,targetX,NULL,
		       gaussClass->classN,weight,weight);

  return TCL_OK;
}



/* ----------------------------------------------------------------------- */
/*  gaussClassUpdateItf                                                    */
/* ----------------------------------------------------------------------- */
int  gaussClassUpdateItf (ClientData cd, int argc, char *argv[]) {

  return  gaussClassUpdateClf(cd);
}




/* ====================================================================== */
/*                  (4) type declarations + initialization                */
/* ====================================================================== */


static Method gaussClassAccuMethod[] = 
{ 
  { "puts",           gaussClassAccuPutsItf,         NULL },
  {  NULL,  NULL, NULL } 
};



TypeInfo gaussClassAccuInfo = { 
        "GaussAccu", 0, -1, gaussClassAccuMethod, 
         NULL, NULL,
         NULL, NULL, itfTypeCntlDefaultNoLink,
        "Training accumulators for a Gauss classifier\n" } ;



static Method gaussClassMethod[] = 
{ 
  { "puts",         gaussClassPutsItf,         NULL },
  { "setMeans",     gaussClassMeansItf,        "set means from a matrix" },
  { "setIvars",     gaussClassIvarsItf,        "set inv. variances from a matrix" },
  { "copy",         gaussClassCopyItf,         "copy Gauss classifier" },
  { "init",         gaussClassInitItf,         "initialize Gauss classifier" },
  { "save",         gaussClassSaveItf,         "save parameters" },
  { "load",         gaussClassLoadItf,         "load parameters" },
  { "makeBBI",      gaussClassMakeBbiItf,      "compute a BBI tree" },
  { "freeBBI",      gaussClassFreeBbiItf,      "remove a BBI tree" },
  { "createAccus",  gaussClassCreateAccusItf,  "create accus for training" },
  { "freeAccus",    gaussClassFreeAccusItf,    "free accus for training" },
  { "clearAccus",   gaussClassClearAccusItf,   "clear accus" },
  { "saveAccus",    gaussClassSaveAccusItf,    "save accus" },
  { "loadAccus",    gaussClassLoadAccusItf,    "load and accu accus" },
  { "score",        gaussClassScoreItf,        "score a given frame" },
  { "accu",         gaussClassAccuHardItf,     "accu a given frame" },
  { "update",       gaussClassUpdateItf,       "update parameters" },
  {  NULL,  NULL, NULL } 
};



static Classifier classifierInfo = {
        "Gauss",
	 gaussClassCreateClf,
	 gaussClassCopyClf,
	 gaussClassInitClf,
	 gaussClassFreeClf,
	 gaussClassSaveClf,
	 gaussClassLoadClf,
	 gaussClassCreateAccusClf,
	 gaussClassFreeAccusClf,
	 gaussClassClearAccusClf,
	 gaussClassSaveAccusClf,
	 gaussClassLoadAccusClf,
	 gaussClassConfigureClf,
	 gaussClassScoreClf,
	 gaussClassAccuSoftClf,
 	 gaussClassAccuHardClf,
 	 gaussClassUpdateClf };



TypeInfo gaussClassInfo = { 
        "Gauss", 0, -1, gaussClassMethod, 
         gaussClassCreateItf, gaussClassFreeItf,
         gaussClassConfigureItf, gaussClassAccessItf, itfTypeCntlDefaultNoLink,
        "A multivariate diagonal covariance Gauss classifier\n" } ;




static int gaussInitialized = 0;

int Gauss_Init ( )
{
  if ( gaussInitialized) return TCL_OK;

  itfNewType    ( &gaussClassAccuInfo );
  itfNewType    ( &gaussClassInfo );
  classNewType  ( &classifierInfo );
  
  gaussInitialized = 1;
  return TCL_OK;
}





