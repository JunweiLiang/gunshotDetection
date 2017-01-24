/* ========================================================================
    JANUS      Speech- to Speech Translation
               ------------------------------------------------------------
               Object: Bucket Box Intersection search trees
               ------------------------------------------------------------

    Author  :  Juergen Fritsch
    Module  :  bbi.c
    Date    :  $Id: bbi.c 3416 2011-03-23 03:02:18Z metze $
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
    Revision 3.6  2003/08/14 11:20:12  fuegen
    Merged changes on branch jtk-01-01-15-fms (jaguar -> ibis-013)

    Revision 3.5.20.5  2002/06/26 11:57:55  fuegen
    changes for new directory structure support and code cleaning

    Revision 3.5.20.4  2001/10/16 11:53:28  soltau
    Introduced CBX for bbi

    Revision 3.5.20.3  2001/06/25 20:57:15  soltau
    Changed bbi interface

    Revision 3.5.20.2  2001/06/20 08:54:28  metze
    Removed unused variable, enabled history logging again

    Revision 1.1  1995/07/25  13:49:57  fritsch
    Initial revision
 
   ======================================================================== */


#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "common.h"
#include <limits.h>
#include "itf.h"
#include "codebook.h"
#include "mach_ind_io.h"
#include "distrib.h"

#include "list.h"
#include "bbi.h"

char bbiRCSVersion[]=
           "@(#)1$Id: bbi.c,v 1.1 1995/11/14 06:05:14 fritsch Exp fritsch" ;



/* ----------------------------------------------------------------------- */
/*  bbiCreateCbBox                                                         */
/* ----------------------------------------------------------------------- */
CbBox  *bbiCreateCbBox (Codebook *cb, float gamma) {

  CbBox    *cbBox = (CbBox *) malloc (sizeof(CbBox));
  int      cbvX,dimX;
  double   mean,ivar=0.0,delta;

  if ((gamma <= 0.0) || (gamma >= 1.0)) {
    ERROR("Invalid gamma, must be in range (0,1)\n");
    return NULL;
  }

  if ((cb->type != COV_DIAGONAL) && (cb->type != COV_RADIAL)) {
    ERROR("BBI trees can only be build for radial or diagonal covariance Gaussians\n");
    return NULL;
  }

  cbBox->cbvN = cb->refN;
  cbBox->dimN = cb->dimN;

  cbBox->active = (char *) malloc (sizeof(char) * cbBox->cbvN);
  cbBox->min = (float **) malloc (sizeof(float *) * cbBox->cbvN);
  cbBox->max = (float **) malloc (sizeof(float *) * cbBox->cbvN);
  for (cbvX=0; cbvX<cbBox->cbvN; cbvX++) {
    cbBox->active[cbvX] = 1;
    cbBox->min[cbvX] = (float *) malloc (sizeof(float) * cbBox->dimN);
    cbBox->max[cbvX] = (float *) malloc (sizeof(float) * cbBox->dimN);
  }

  for (cbvX=0; cbvX<cbBox->cbvN; cbvX++) {
    for (dimX=0; dimX<cbBox->dimN; dimX++) {
      mean = cb->rv->matPA[cbvX][dimX];
      switch (cb->type) {
        case COV_RADIAL:
          ivar = cb->cv[cbvX]->m.r;
          break;
        case COV_DIAGONAL: 
          ivar = cb->cv[cbvX]->m.d[dimX];
          break;
        default: ;
      }
      delta = sqrt(-2.0*log(gamma)/ivar);
      cbBox->min[cbvX][dimX] = mean - delta;
      cbBox->max[cbvX][dimX] = mean + delta;
    }
  }

  return cbBox;
}



/* ----------------------------------------------------------------------- */
/*  bbiFreeCbBox                                                           */
/* ----------------------------------------------------------------------- */
int  bbiFreeCbBox (CbBox *cbBox) {

  int        cbvX;

  assert (cbBox);

  for (cbvX=0; cbvX<cbBox->cbvN; cbvX++) {
    free(cbBox->min[cbvX]);
    free(cbBox->max[cbvX]);
  }
  free(cbBox->min);
  free(cbBox->max);
  free(cbBox->active);
  free(cbBox);

  return TCL_OK;
}







/* ----------------------------------------------------------------------- */
/*  bbiNodeFree                                                            */
/* ----------------------------------------------------------------------- */
int  bbiNodeFree (BBINode *node) {

  return TCL_OK;
}



/* ----------------------------------------------------------------------- */
/*  bbiNodePutsItf                                                         */
/* ----------------------------------------------------------------------- */
int  bbiNodePutsItf (ClientData cd, int argc, char **argv) {

  BBINode        *node = (BBINode *) cd;

  itfAppendResult("{k %d} {h %f} ",node->k,node->h);

  return TCL_OK;
}



/* ----------------------------------------------------------------------- */
/*  bbiNodeConfigureItf                                                    */
/* ----------------------------------------------------------------------- */
int  bbiNodeConfigureItf (ClientData cd, char *var, char *val) {

  BBINode       *node = (BBINode *) cd;

  if (! var) {
    ITFCFG(bbiNodeConfigureItf,cd,"k");
    ITFCFG(bbiNodeConfigureItf,cd,"h");
    return TCL_OK;
  }
  ITFCFG_Int    (var,val,"k",  node->k,      1);
  ITFCFG_Float  (var,val,"h",  node->h,      1);

  return TCL_OK;
}



/* ----------------------------------------------------------------------- */
/*                                 BBINode                                 */
/* ----------------------------------------------------------------------- */

static Method bbiNodeMethod[] = 
{ 
  { "puts",      bbiNodePutsItf,        NULL },
  {  NULL,  NULL, NULL } 
};


TypeInfo bbiNodeInfo = {
        "BBINode", 0, -1, bbiNodeMethod,
         NULL, NULL,
         bbiNodeConfigureItf, NULL, 
         itfTypeCntlDefaultNoLink,
	"node in a BBI search tree\n" } ;






/* ----------------------------------------------------------------------- */
/*  bbiLeafFree                                                            */
/* ----------------------------------------------------------------------- */
int  bbiLeafFree (BBILeaf *leaf) {

  if (leaf->cbvN > 0) free(leaf->cbX);

  return TCL_OK;
}



/* ----------------------------------------------------------------------- */
/*  bbiLeafPutsItf                                                         */
/* ----------------------------------------------------------------------- */
int  bbiLeafPutsItf (ClientData cd, int argc, char **argv) {

  BBILeaf        *leaf = (BBILeaf *) cd;
  int            cbvX;

  itfAppendResult("{ ");
  for (cbvX=0; cbvX<leaf->cbvN; cbvX++) 
    itfAppendResult("%d ",leaf->cbX[cbvX]);
  itfAppendResult("} ");

  return TCL_OK;
}



/* ----------------------------------------------------------------------- */
/*  bbiLeafConfigureItf                                                    */
/* ----------------------------------------------------------------------- */
int  bbiLeafConfigureItf (ClientData cd, char *var, char *val) {

  BBILeaf       *leaf = (BBILeaf *) cd;

  if (! var) {
    ITFCFG(bbiLeafConfigureItf,cd,"cbvN");
    return TCL_OK;
  }
  ITFCFG_Int    (var,val,"cbvN",  leaf->cbvN,      1);
  return TCL_OK;
}



/* ----------------------------------------------------------------------- */
/*                                 BBILeaf                                 */
/* ----------------------------------------------------------------------- */

static Method bbiLeafMethod[] = 
{ 
  { "puts",      bbiLeafPutsItf,        NULL },
  {  NULL,  NULL, NULL } 
};


TypeInfo bbiLeafInfo = {
        "BBILeaf", 0, -1, bbiLeafMethod,
         NULL, NULL, 
         bbiLeafConfigureItf, NULL, 
         itfTypeCntlDefaultNoLink,
	"leaf in a BBI search tree\n" } ;






/* ----------------------------------------------------------------------- */
/*  bbiInit                                                                */
/* ----------------------------------------------------------------------- */
#define BBI_SINGLETONS_MAX   512
#define BBI_MEMCHUNK_SIZE   (512*1024)

int  bbiInit (BBITree  *bbi, ClientData cd) {
  int cbvX;

  assert(bbi);

  bbi->name   = strdup((char *) cd);
  bbi->active = 1;
  bbi->cbN    = 0;
  bbi->depthN = 0;
  bbi->node   = NULL;
  bbi->leaf   = NULL;

  /* hyu@cs */
  bbi->memChunk = bmemCreate( BBI_MEMCHUNK_SIZE, 0);
  assert( bbi->memChunk);

  bbi->singletonChunk = (CBX*) malloc( BBI_SINGLETONS_MAX * sizeof(CBX));
  assert( bbi->singletonChunk);
  for (cbvX=0; cbvX < BBI_SINGLETONS_MAX; cbvX++) 
    bbi->singletonChunk[cbvX] = cbvX;

  return TCL_OK;
}



/* ----------------------------------------------------------------------- */
/*  bbiDeinit                                                              */
/* ----------------------------------------------------------------------- */
int  bbiDeinit (BBITree  *bbi) {

  int              leafN,nodeX,leafX;

  assert(bbi);

  if (bbi->name) free(bbi->name);
  if (bbi->depthN == 0) return TCL_OK;

  leafN = 1<<bbi->depthN;
  for (nodeX=1; nodeX<leafN; nodeX++)
    bbiNodeFree(&bbi->node[nodeX]);
  free(bbi->node);

  for (leafX=0; leafX<leafN; leafX++) {
    free(bbi->leaf[leafX]);
  }
  free(bbi->leaf);

  bbi->node   = NULL;
  bbi->leaf   = NULL;
  bbi->depthN = 0;
  bbi->cbN    = 0;

  /* hyu@cs */
  if ( bbi->memChunk)       bmemFree( bbi->memChunk);
  if ( bbi->singletonChunk) free( bbi->singletonChunk);

  return TCL_OK;
}



/* ----------------------------------------------------------------------- */
/*  itemCompare                                                            */
/* ----------------------------------------------------------------------- */
int  itemCompare (const void *a, const void *b)
{
  SortItem    *i1 = (SortItem *)a;
  SortItem    *i2 = (SortItem *)b;

  if      (i1->value < i2->value) return -1;
  else if (i1->value > i2->value) return  1;
  else return 0;
}



/* -----------------------------------------------------------------------   
    bbiHyperplane    modified by hyu@cs to allow some (controlled by spread) 
         search to reduce #intersections rather than purely balance the tree
   ----------------------------------------------------------------------- */

int  bbiHyperplane (ProjData *pd, int *k, float *h, int *sepa) {

  int      dimX,minIdx,minSplits,gaussN,spread;
  float    minVal;

  minIdx    = 0;
  minVal    = 0.0;
  minSplits = pd->projN;
  gaussN    = pd->projN>>1;
  spread    = floor(gaussN/3);

  for (dimX=0; dimX<pd->dimN; dimX++) {
    int lminSplits=pd->projN, lminIdx=-1, splits, i;

    /* ------------------ */
    /*  sort projections  */
    /* ------------------ */
    qsort(pd->proj[dimX],pd->projN,sizeof(SortItem),itemCompare);

    /* ----------------------------------- */
    /*  determine optimal splitting point  */
    /* ----------------------------------- */
    splits = 0;
    for (i=0; i<gaussN-spread; i++)
      if (pd->proj[dimX][i].label == 0) splits++; else splits--;
    for (     ; i<gaussN+spread; i++) {
      if (pd->proj[dimX][i].label == 0) splits++; else splits--;
      if (splits < lminSplits) { lminSplits = splits; lminIdx = i; }
    }

    if (lminSplits < minSplits) {
      minSplits = lminSplits;
      minVal    = 0.5*(pd->proj[dimX][lminIdx-1].value + 
                       pd->proj[dimX][lminIdx].value);
      minIdx    = dimX;
    }
  }

  *k = minIdx;
  *h = minVal;
  *sepa = (gaussN + minSplits)>>1;

  return TCL_OK;
}



/* ----------------------------------------------------------------------- */
/*  bbiOptimizeNode                                                        */
/* ----------------------------------------------------------------------- */
int  bbiOptimizeNode (BBITree *bbi, CbsBox *locBox, int nodeX, int verbose,
                      int *avebs) {

  ProjData      *pd;
  CbBox         *box=NULL;
  int           dimX,idx,cbX,cbvX,count,k,sepa;
  float         h;

  /* ---------------------------------- */
  /*  count number of active Gaussians  */
  /* ---------------------------------- */
  count = 0;
  for (cbX=0; cbX<locBox->cbN; cbX++) {
    box = locBox->box[cbX];
    for (cbvX=0; cbvX<box->cbvN; cbvX++)
      if (box->active[cbvX]) count++;
  }

  /* ----------------------------------------- */
  /*  create and initialize projection struct  */
  /* ----------------------------------------- */
  assert (box);
  pd = (ProjData *) malloc (sizeof(ProjData));
  pd->dimN  = box->dimN;
  pd->projN = 2*count;
  pd->proj = (SortItem **) malloc (sizeof(SortItem *) * pd->dimN);
  for (dimX=0; dimX<pd->dimN; dimX++) {
    pd->proj[dimX] = (SortItem *) malloc (sizeof(SortItem) * pd->projN);
    for (cbX=0, idx=0; cbX<locBox->cbN; cbX++) {
      box = locBox->box[cbX];
      for (cbvX=0; cbvX<box->cbvN; cbvX++) {
        if (box->active[cbvX]) {
          pd->proj[dimX][idx].value   = box->min[cbvX][dimX];
          pd->proj[dimX][idx].label   = 0;
          pd->proj[dimX][idx+1].value = box->max[cbvX][dimX];
          pd->proj[dimX][idx+1].label = 1;
          idx += 2;
        }
      }
    }
  }

  /* -------------------------------------------------------------- */
  /*  compute optimal division hyperplane according to proj struct  */
  /* -------------------------------------------------------------- */
  bbiHyperplane(pd,&k,&h,&sepa);
  bbi->node[nodeX].k = k;
  bbi->node[nodeX].h = h;
  *avebs = sepa;
  if (verbose > 2)
    fprintf(stderr,"    Optimal Split in Node %d: k=%d h=%f abs=%d\n",
                              nodeX,k,h,sepa);

  /* ------------------ */
  /*  free proj struct  */
  /* ------------------ */
  for (dimX=0; dimX<pd->dimN; dimX++) free(pd->proj[dimX]);
  free(pd->proj);
  free(pd);

  return TCL_OK;
}



/* ----------------------------------------------------------------------- */
/*  bbiMakeLeaf                                                            */
/* ----------------------------------------------------------------------- */
int  bbiMakeLeaf (BBITree *bbi, CbsBox *cbsBox, int leafX, int verbose) {

  CbBox       *box;
  BBILeaf     *leaf;
  Bucket      *bucket;
  int         idx,nodeX,up,turn,k,cbX,dimX,cbvX,count,minIdx;
  float       h,min,max,dist,minDist,maxDist;

  assert (bbi && cbsBox);

  bucket = (Bucket *) malloc (sizeof(Bucket));
  bucket->dimN = cbsBox->box[0]->dimN;
  bucket->min = (float *) malloc (sizeof(float) * bucket->dimN);
  bucket->max = (float *) malloc (sizeof(float) * bucket->dimN);
  for (dimX=0; dimX<bucket->dimN; dimX++) {
    bucket->min[dimX] = -1E10;
    bucket->max[dimX] = +1E10;
  }

  /* ------------------------------------------------- */
  /*  go down the tree to actual node, adapting boxes  */
  /* ------------------------------------------------- */
  idx = 1;
  nodeX = leafX + (1<<bbi->depthN);
  while (idx != nodeX) {

    /* determine turn */
    up = nodeX;
    do {
      turn = up & 1;
      up = up >> 1;
    } while (up > idx);
    k = bbi->node[idx].k;
    h = bbi->node[idx].h;

    /* adapt boxes */
    for (cbX=0; cbX<cbsBox->cbN; cbX++) {
      box = cbsBox->box[cbX];
      for (cbvX=0; cbvX<box->cbvN; cbvX++) {

        if (!box->active[cbvX]) continue;

        min = box->min[cbvX][k];
        max = box->max[cbvX][k];

        /* de-activating */
        if ((turn && (max <= h)) ||
            (!turn && (min >= h))) {
          box->active[cbvX] = 0;
          continue;
        }

        /* moving borders */
        if ((min < h) && (max > h)) {
          if (turn) box->min[cbvX][k] = h;
              else  box->max[cbvX][k] = h;
        } 
      }
    }

    /* go down */
    idx = 2*idx + turn;
    if (turn) bucket->min[k] = h;
        else  bucket->max[k] = h;
  }

  /* ------------------------------------------- */
  /*  build BBI lists, taking care of back-offs  */
  /* ------------------------------------------- */
  for (cbX=0; cbX<bbi->cbN; cbX++) {
    leaf = &bbi->leaf[leafX][cbX];
    box  = cbsBox->box[cbX];

    /* ---------------------------------- */
    /*  count remaining active Gaussians  */
    /* ---------------------------------- */
    count = 0;
    for (cbvX=0; cbvX<box->cbvN; cbvX++)
      if (box->active[cbvX]) count++;

    /* -------------------------- */
    /*  create and fill BBI list  */
    /* -------------------------- */
    if (count > 0) {

      /* ------------------ */
      /*  regular BBI list  */
      /* ------------------ */
      leaf->cbvN = count;
      leaf->cbX  = (CBX *) malloc (sizeof(CBX) * count);
      count = 0;
      for (cbvX=0; cbvX<box->cbvN; cbvX++)
        if (box->active[cbvX]) leaf->cbX[count++] = (CBX) cbvX;

      if (verbose > 2)
        fprintf(stderr,"    Creating BBI list for leaf[%d][%d], %d Gaussians\n",leafX,cbX,count);

    } else {

      /* ------------------- */
      /*  back-off strategy  */
      /* ------------------- */
      minDist = 1E10;
      minIdx  = 0;
      for (cbvX=0; cbvX<box->cbvN; cbvX++) {
        maxDist = 0.0;
        for (dimX=0; dimX<bucket->dimN; dimX++) {
          if (box->max[cbvX][dimX] < bucket->min[dimX]) {
            dist = bucket->min[dimX] - box->max[cbvX][dimX];
          } else {
            if (box->min[cbvX][dimX] > bucket->max[dimX]) {
              dist = box->min[cbvX][dimX] - bucket->max[dimX];
            } else {
              dist = 0.0;
            }
          }
          if (dist > maxDist) maxDist = dist;
        }
        if (maxDist < minDist) {
          minDist = maxDist;
          minIdx  = cbvX;
        }
      }

      leaf->cbvN = 1;
      leaf->cbX = (CBX *) malloc (sizeof(CBX));
      leaf->cbX[0] = minIdx;

      if (verbose > 2)
        fprintf(stderr,"    Creating BBI list for leaf[%d][%d], Using Back-Off Gaussian\n",leafX,cbX);

    }
  }

  free(bucket->min);
  free(bucket->max);
  free(bucket);

  return TCL_OK;
}



/* ----------------------------------------------------------------------- */
/*  bbiMakeTree                                                            */
/* ----------------------------------------------------------------------- */
int  bbiMakeTree (BBITree *bbi, CbsBox *cbsBox, int verbose) {

  int         cbX,cbvX,dimX,nodeX,leafN,idx,turn,up,k,bucketsize;
  int         nodes,minbs,maxbs,meanbs,level=0,oldlevel,leafX,count;
  float       h,min,max,redu;
  CbsBox      *locBox;
  CbBox       *box;

  assert (bbi && cbsBox);

  /* ------------------------------------ */
  /*  create an additional CbsBox struct  */
  /* ------------------------------------ */
  count = 0;
  locBox = (CbsBox *) malloc (sizeof(CbsBox));
  locBox->cbN = cbsBox->cbN;
  locBox->box = (CbBox **) malloc (sizeof(CbBox *) * locBox->cbN);
  for (cbX=0; cbX<locBox->cbN; cbX++) {
    locBox->box[cbX] = (CbBox *) malloc (sizeof(CbBox));
    box = locBox->box[cbX];
    box->cbvN = cbsBox->box[cbX]->cbvN;
    box->dimN = cbsBox->box[cbX]->dimN;
    box->active = (char *) malloc (sizeof(char) * box->cbvN);
    box->min = (float **) malloc (sizeof(float *) * box->cbvN);
    box->max = (float **) malloc (sizeof(float *) * box->cbvN);
    for (cbvX=0; cbvX<box->cbvN; cbvX++) {
      if (cbsBox->box[cbX]->active[cbvX]) count++;
      box->min[cbvX] = (float *) malloc (sizeof(float) * box->dimN);
      box->max[cbvX] = (float *) malloc (sizeof(float) * box->dimN);
    }
  }

  if (verbose > 1)
    fprintf(stderr,"    Initial number of Gaussians = %d\n",count);

  /* --------------------------------------- */
  /*  create tree in a breadth-first search  */
  /* --------------------------------------- */
  leafN = 1<<(bbi->depthN);
  oldlevel = -1;
  nodes  = 0;
  meanbs = 0.0;
  minbs  = +INT_MAX;
  maxbs  = -INT_MAX;
  for (nodeX=1; nodeX<leafN; nodeX++) {

    /* ----------------------- */
    /*  initialize level info  */
    /* ----------------------- */
    level = (int) (log(nodeX) / log(2.0));
    if (level != oldlevel) {

      if (nodes > 0) meanbs /= nodes;
      if ((oldlevel >= 0) && (verbose > 1)) { 
        redu = 100.0 * meanbs / ((double) count);    
        fprintf(stderr,"    Level %3d  :  min=%6d  mean=%6d (%5.1f%%)  max=%6d\n",
                level,minbs,meanbs,redu,maxbs);
      }

      oldlevel = level;
      nodes  = 0;
      meanbs = 0.0;
      minbs  = +INT_MAX;
      maxbs  = -INT_MAX;
    }

    /* ------------------------------- */
    /*  make a copy of initial cbsBox  */
    /* ------------------------------- */
    for (cbX=0; cbX<locBox->cbN; cbX++) {
      box = locBox->box[cbX];
      for (cbvX=0; cbvX<box->cbvN; cbvX++) {
        box->active[cbvX] = cbsBox->box[cbX]->active[cbvX];
        for (dimX=0; dimX<box->dimN; dimX++) {
          box->min[cbvX][dimX] = cbsBox->box[cbX]->min[cbvX][dimX];
          box->max[cbvX][dimX] = cbsBox->box[cbX]->max[cbvX][dimX];
        }
      }
    }

    /* ------------------------------------------------- */
    /*  go down the tree to actual node, adapting boxes  */
    /* ------------------------------------------------- */
    idx = 1;
    while (idx < nodeX) {

      /* determine turn */
      up = nodeX;
      do {
        turn = up & 1;
        up = up >> 1;
      } while (up > idx);
      k = bbi->node[idx].k;
      h = bbi->node[idx].h;

      /* adapt boxes */
      for (cbX=0; cbX<locBox->cbN; cbX++) {
        box = locBox->box[cbX];
        for (cbvX=0; cbvX<box->cbvN; cbvX++) {

          if (!box->active[cbvX]) continue;

          min = box->min[cbvX][k];
          max = box->max[cbvX][k];

          /* de-activating */
          if ((turn && (max <= h)) ||
              (!turn && (min >= h))) {
            box->active[cbvX] = 0;
            continue;
          }

          /* moving borders */
          if ((min < h) && (max > h)) {
            if (turn) box->min[cbvX][k] = h;
                else  box->max[cbvX][k] = h;
          }

        }
      }

      /* go down */
      idx = 2*idx + turn;
    }

    /* ------------------------------------- */
    /*  compute optimal division hyperplane  */
    /* ------------------------------------- */
    bbiOptimizeNode(bbi,locBox,nodeX,verbose,&bucketsize);
    meanbs += bucketsize;
    if (bucketsize < minbs) minbs = bucketsize;
    if (bucketsize > maxbs) maxbs = bucketsize;
    nodes++;

  }

  if (nodes > 0) meanbs /= nodes;
  if ((oldlevel >= 0) && (verbose > 1)) {
    redu = 100.0 * meanbs / ((double) count);
    fprintf(stderr,"    Level %3d  :  min=%6d  mean=%6d (%5.1f%%)  max=%6d\n",
              level+1,minbs,meanbs,redu,maxbs);
  }

  /* ------------------------------------------------- */
  /*  create bucket-box intersection lists for leaves  */
  /* ------------------------------------------------- */
  for (leafX=0; leafX<leafN; leafX++) {

    /* ------------------------------- */
    /*  make a copy of initial cbsBox  */
    /* ------------------------------- */
    for (cbX=0; cbX<locBox->cbN; cbX++) {
      box = locBox->box[cbX];
      for (cbvX=0; cbvX<box->cbvN; cbvX++) {
        box->active[cbvX] = cbsBox->box[cbX]->active[cbvX];
        for (dimX=0; dimX<box->dimN; dimX++) {
          box->min[cbvX][dimX] = cbsBox->box[cbX]->min[cbvX][dimX];
          box->max[cbvX][dimX] = cbsBox->box[cbX]->max[cbvX][dimX];
        }
      }
    }

    bbiMakeLeaf(bbi,locBox,leafX,verbose);
  }

  /* ------------------------- */
  /*  free local boxes struct  */
  /* ------------------------- */
  for (cbX=0; cbX<locBox->cbN; cbX++)
    bbiFreeCbBox(locBox->box[cbX]);
  free(locBox->box);
  free(locBox);

  return TCL_OK;
}



/* ----------------------------------------------------------------------- */
/*  bbiMake                                                                */
/* ----------------------------------------------------------------------- */
int  bbiMake (BBITree *bbi, CbsBox *cbsBox, int depth, int verbose) {

  char            name[255];
  int             cbN,leafN,leafX,k;
  BBILeaf         *leaf;

  assert(bbi && cbsBox);

  if (depth < 1) {
    ERROR("Invalid tree depth\n");
    return TCL_ERROR;
  }

  strcpy(name,bbi->name);
  cbN = bbi->cbN;
  bbiDeinit(bbi);
  bbiInit(bbi,(ClientData) name);

  bbi->active = 1;
  bbi->cbN    = cbN;
  bbi->depthN = depth;

  /* ----------------------------- */
  /*  create tree data structures  */
  /* ----------------------------- */
  leafN = 1<<(bbi->depthN);
  bbi->node = (BBINode *) malloc (sizeof(BBINode) * leafN);
  bbi->leaf = (BBILeaf **) malloc (sizeof(BBILeaf *) * leafN);
  for (leafX=0; leafX<leafN; leafX++) {
    bbi->leaf[leafX] = (BBILeaf *) malloc (sizeof(BBILeaf) * bbi->cbN);
    for (k=0; k<bbi->cbN; k++) {
      leaf = &bbi->leaf[leafX][k];
      leaf->cbvN = 0;
      leaf->cbX  = NULL;
    }
  }

  /* --------------------- */
  /*  call tree generator  */
  /* --------------------- */
  if (verbose > 0) 
    fprintf(stderr,"    Computing BBI tree '%s' with depth=%d\n",name,depth);

  return bbiMakeTree(bbi,cbsBox,verbose);
}



/* ----------------------------------------------------------------------- */
/*  bbiSave                                                                */
/* ----------------------------------------------------------------------- */
int  bbiSave (BBITree *bbi, FILE *fp) {

  int         leafN,nodeX,leafX,k,cbvX;
  BBILeaf     *leaf;

  assert(bbi);

  write_string(fp,bbi->name);
  write_int(fp,bbi->cbN);
  write_int(fp,bbi->depthN);

  leafN = 1<<(bbi->depthN);

  for (nodeX=1; nodeX<leafN; nodeX++) {
    write_int(fp,bbi->node[nodeX].k);
    write_float(fp,bbi->node[nodeX].h);
  }

  for (leafX=0; leafX<leafN; leafX++) {
    for (k=0; k<bbi->cbN; k++) {
      leaf = &bbi->leaf[leafX][k];
      write_int(fp,(int) leaf->cbvN);
      for (cbvX=0; cbvX<leaf->cbvN; cbvX++)
        write_int(fp,(int) leaf->cbX[cbvX]);
    }
  }

  return TCL_OK;
}



/* ----------------------------------------------------------------------- 
    bbiLoad     hyu@cs   memory efficient
   ----------------------------------------------------------------------- */

int  bbiLoad (BBITree *bbi, FILE *fp) {

  char            name[255],name2[255];
  int             leafN,nodeX,leafX,k,cbvX;
  BBILeaf         *leaf;

  assert(bbi);

  strcpy(name,bbi->name);
  bbiDeinit(bbi);
  bbiInit(bbi,(ClientData) name);

  read_string(fp,name2);
  if (strcmp(name,name2) != 0) {
    ERROR("naming conflict while loading BBI tree %s\n",name);
    return TCL_ERROR;
  }

  bbi->cbN    = read_int(fp);
  bbi->depthN = read_int(fp);

  leafN = 1<<(bbi->depthN);

  bbi->node = (BBINode *) malloc (sizeof(BBINode) * leafN);
  for (nodeX=1; nodeX<leafN; nodeX++) {
    bbi->node[nodeX].k = read_int(fp);
    bbi->node[nodeX].h = read_float(fp);
  }

  bbi->leaf = (BBILeaf **) malloc (sizeof(BBILeaf *) * leafN);
  for (leafX=0; leafX<leafN; leafX++) {
    bbi->leaf[leafX] = (BBILeaf *) malloc (sizeof(BBILeaf) * bbi->cbN);
    for (k=0; k<bbi->cbN; k++) {
      leaf = &bbi->leaf[leafX][k];
      leaf->cbvN = (CBX) read_int(fp);
      /*leaf->cbX  = (CBX *) malloc (sizeof(CBX) * leaf->cbvN);*/

      if (leaf->cbvN == 0) {
	leaf->cbX = NULL; 

      } else if (leaf->cbvN == 1) {
	cbvX = (CBX) read_int(fp);
	assert( cbvX < BBI_SINGLETONS_MAX);

	leaf->cbX = bbi->singletonChunk + cbvX; 

      } else {

	leaf->cbX    = bmemAlloc( bbi->memChunk, leaf->cbvN * sizeof(CBX));
	for (cbvX=0; cbvX<leaf->cbvN; cbvX++)
	  leaf->cbX[cbvX] = (CBX) read_int(fp);
      }
    }
  }

  return TCL_OK;
}


/* ----------------------------------------------------------------------- */
/*  bbiLoad                                                                */
/* ----------------------------------------------------------------------- */
int  OrgbbiLoad (BBITree *bbi, FILE *fp) {

  char            name[255],name2[255];
  int             leafN,nodeX,leafX,k,cbvX;
  BBILeaf         *leaf;

  assert(bbi);

  strcpy(name,bbi->name);
  bbiDeinit(bbi);
  bbiInit(bbi,(ClientData) name);

  read_string(fp,name2);
  if (strcmp(name,name2) != 0) {
    ERROR("naming conflict while loading BBI tree %s\n",name);
    return TCL_ERROR;
  }

  bbi->cbN    = read_int(fp);
  bbi->depthN = read_int(fp);

  leafN = 1<<(bbi->depthN);

  bbi->node = (BBINode *) malloc (sizeof(BBINode) * leafN);
  for (nodeX=1; nodeX<leafN; nodeX++) {
    bbi->node[nodeX].k = read_int(fp);
    bbi->node[nodeX].h = read_float(fp);
  }

  bbi->leaf = (BBILeaf **) malloc (sizeof(BBILeaf *) * leafN);
  for (leafX=0; leafX<leafN; leafX++) {
    bbi->leaf[leafX] = (BBILeaf *) malloc (sizeof(BBILeaf) * bbi->cbN);
    for (k=0; k<bbi->cbN; k++) {
      leaf = &bbi->leaf[leafX][k];
      leaf->cbvN = (CBX) read_int(fp);
      leaf->cbX  = (CBX *) malloc (sizeof(CBX) * leaf->cbvN);
      for (cbvX=0; cbvX<leaf->cbvN; cbvX++)
        leaf->cbX[cbvX] = (CBX) read_int(fp);
    }
  }

  return TCL_OK;
}



/* ----------------------------------------------------------------------- */
/*  bbiPruneLevel                                                          */
/* ----------------------------------------------------------------------- */
int  bbiPruneLevel (BBITree *bbi) {

  BBITree        *pbbi;
  int            depthN,leafN,leafX,nodeX,cbX,k,idxL,idxR,jointN;
  BBILeaf        *bucketL,*bucketR,*leaf;

  depthN = bbi->depthN - 1;
  leafN  = 1<<depthN;

  pbbi = (BBITree *) malloc (sizeof(BBITree));
  bbiInit(pbbi,(ClientData) bbi->name);
  pbbi->depthN = depthN;
  pbbi->cbN = bbi->cbN;
  
  pbbi->node = (BBINode *) malloc (sizeof(BBINode) * leafN);
  pbbi->leaf = (BBILeaf **) malloc (sizeof(BBILeaf *) * leafN);
  for (leafX=0; leafX<leafN; leafX++) {
    pbbi->leaf[leafX] = (BBILeaf *) malloc (sizeof(BBILeaf) * pbbi->cbN);
    for (k=0; k<pbbi->cbN; k++) {
      leaf = &pbbi->leaf[leafX][k];
      leaf->cbvN = 0;
      leaf->cbX  = NULL;
    }
  }

  /* ------------------ */
  /*  create new nodes  */
  /* ------------------ */
  for (nodeX=1; nodeX<leafN; nodeX++) {
    pbbi->node[nodeX].k = bbi->node[nodeX].k;
    pbbi->node[nodeX].h = bbi->node[nodeX].h;
  }

  /* ------------------- */
  /*  create new leaves  */
  /* ------------------- */
  for (leafX=0; leafX<leafN; leafX++) {
    for (cbX=0; cbX<pbbi->cbN; cbX++) {
      bucketL = &bbi->leaf[2*leafX][cbX];
      bucketR = &bbi->leaf[2*leafX+1][cbX];
      
      /* ------------------------------------------------- */
      /*  determine number of codevectors in joint bucket  */
      /* ------------------------------------------------- */
      jointN = 0;
      idxL = 0;
      idxR = 0;
      while ((idxL < bucketL->cbvN) && (idxR < bucketR->cbvN)) {
        if (bucketL->cbX[idxL] < bucketR->cbX[idxR]) {
          idxL++;
          jointN++;
        }
        if (bucketL->cbX[idxL] > bucketR->cbX[idxR]) {
          idxR++;
          jointN++;
        }
        if (bucketL->cbX[idxL] == bucketR->cbX[idxR]) {
          idxL++;
        }
      }
      jointN += ((bucketL->cbvN - idxL) + (bucketR->cbvN - idxR));

      /* ----------------------- */
      /*  recompute bucket list  */
      /* ----------------------- */
      pbbi->leaf[leafX][cbX].cbvN = jointN;
      pbbi->leaf[leafX][cbX].cbX = (CBX *) malloc (sizeof(CBX) * jointN);
      jointN = 0;
      idxL = 0;
      idxR = 0;
      while ((idxL < bucketL->cbvN) && (idxR < bucketR->cbvN)) {
        if (bucketL->cbX[idxL] < bucketR->cbX[idxR]) {
          pbbi->leaf[leafX][cbX].cbX[jointN] = bucketL->cbX[idxL];
          idxL++;
          jointN++;
        }
        if (bucketL->cbX[idxL] > bucketR->cbX[idxR]) {
          pbbi->leaf[leafX][cbX].cbX[jointN] = bucketR->cbX[idxR];
          idxR++;
          jointN++;
        }
        if (bucketL->cbX[idxL] == bucketR->cbX[idxR]) {
          idxL++;
        }
      }

      if (idxL == bucketL->cbvN) {
        for (k=idxR; k<bucketR->cbvN; k++)
          pbbi->leaf[leafX][cbX].cbX[jointN++] = bucketR->cbX[k];
      } else {
        for (k=idxL; k<bucketL->cbvN; k++)
          pbbi->leaf[leafX][cbX].cbX[jointN++] = bucketL->cbX[k];
      }

    }
  }

  /* --------------------------- */
  /*  replace existing BBI tree  */
  /* --------------------------- */
  bbiDeinit(bbi);
  bbi->name   = pbbi->name;
  bbi->active = pbbi->active;
  bbi->depthN = pbbi->depthN;
  bbi->cbN    = pbbi->cbN;
  bbi->node   = pbbi->node;
  bbi->leaf   = pbbi->leaf;
  free(pbbi);

  return TCL_ERROR;
}



/* ----------------------------------------------------------------------- */
/*  bbiPrune                                                               */
/* ----------------------------------------------------------------------- */
int  bbiPrune (BBITree *bbi, int levelN) {

  int             levelX;

  assert(bbi);

  if (bbi->depthN <= 0) {
    ERROR("No BBI tree available, or BBI tree empty\n");
    return TCL_ERROR;
  }

  if (bbi->depthN - levelN <= 0) {
    ERROR("Can not prune %d levels of BBI tree with depth %d\n",
            levelN,bbi->depthN);
    return TCL_ERROR;
  }

  for (levelX=0; levelX<levelN; levelX++)
    bbiPruneLevel(bbi);

  return TCL_OK;
}



/* ----------------------------------------------------------------------- */
/*  bbiPutsItf                                                             */
/* ----------------------------------------------------------------------- */
int  bbiPutsItf (ClientData cd, int argc, char **argv) {

  BBITree       *bbi = (BBITree *) cd;

  itfAppendResult("{name %s} {active %d} {cbN %d} {depth %d}",
        bbi->name,bbi->active,bbi->cbN,bbi->depthN);

  return TCL_OK;
}



/* ----------------------------------------------------------------------- */
/*  bbiConfigureItf                                                        */
/* ----------------------------------------------------------------------- */
int  bbiConfigureItf (ClientData cd, char *var, char *val) {

  BBITree       *bbi = (BBITree *) cd;

  if (! var) {
    ITFCFG(bbiConfigureItf,cd,"name");
    ITFCFG(bbiConfigureItf,cd,"active");
    ITFCFG(bbiConfigureItf,cd,"cbN");
    ITFCFG(bbiConfigureItf,cd,"depth");
    return TCL_OK;
  }
  ITFCFG_CharPtr(var,val,"name",         bbi->name,                 1);
  ITFCFG_Int    (var,val,"active",       bbi->active,               0);
  ITFCFG_Int    (var,val,"cbN",          bbi->cbN,                  1);
  ITFCFG_Int    (var,val,"depth",        bbi->depthN,               1);
  return TCL_OK;
}



/* ----------------------------------------------------------------------- */
/*  bbiAccessItf                                                           */
/* ----------------------------------------------------------------------- */
ClientData  bbiAccessItf (ClientData cd, char *name, TypeInfo **ti) {

  BBITree       *bbi   = (BBITree *) cd;
  int           leafN  = 1<<(bbi->depthN);
  int           cbN    = bbi->cbN;
  int           nodeX,leafX,cbX;

  if (*name == '.') { 
    if (name[1] == '\0') {
      itfAppendElement( "mem");
      if ( bbi->depthN > 0) itfAppendElement( "node(1..%d)",leafN-1);
      if ( bbi->depthN > 0) itfAppendElement( "leaf(0..%d,0..%d)",
                                            leafN-1,cbN-1);
      *ti=NULL;
      return NULL; 
    } else {
      if (strcmp(name+1,"mem") == 0) {
	*ti = itfGetType("BMem");
	return (ClientData) bbi->memChunk;
      } 
      if (sscanf(name+1,"node(%d)",&nodeX) == 1) {
	*ti = itfGetType("BBINode");
	if (nodeX >= 1 && nodeX < leafN) 
            return (ClientData) &bbi->node[nodeX];
        else return NULL;
      }                
      else if (sscanf(name+1,"leaf(%d,%d)",&leafX,&cbX) == 2) {
	*ti = itfGetType("BBILeaf");
	if ((leafX >= 0 && leafX < leafN) &&
            (cbX >= 0 && cbX < cbN))
            return (ClientData) &bbi->leaf[leafX][cbX];
        else return NULL;
      }                
    }
  }

  return NULL;
}

/* ----------------------------------------------------------------------- */
/*  bbiCompareItf                                                          */
/* ----------------------------------------------------------------------- */

int  bbiCompareItf (ClientData cd,int argc,char *argv[])
{
  BBITree *bbi1 = (BBITree *) cd;
  BBITree *bbi2 = NULL;

  BBILeaf *leaf1 = NULL;
  BBILeaf *leaf2 = NULL;
 
  int ac     = argc-1;
  int cbN    = bbi1->cbN;
  int depthN = bbi1->depthN;
  int leafN  = 1<<(depthN);
  int nodeX,leafX;

  if (itfParseArgv (argv[0],&ac,argv+1,1,
                    "<BBI>",   ARGV_OBJECT, NULL, &bbi2, "BBITree", "name of bbi tree to compare", 
                    NULL) != TCL_OK) return TCL_ERROR;

  if (bbi1->cbN != bbi2->cbN) {
    INFO("bbi1->cbN= %d != bbi2->cbN=%d\n",bbi1->cbN,bbi2->cbN);
    return TCL_OK;
  }
  if (bbi1->depthN != bbi2->depthN) {
    INFO("bbi1->depthN= %d != bbi2->depthN=%d\n",bbi1->depthN,bbi2->depthN);
    return TCL_OK;
  }

  INFO("check nodes\n");
  for (nodeX=1;nodeX<leafN;nodeX++) {
    if (bbi1->node[nodeX].k != bbi2->node[nodeX].k) {
      INFO("node %d coordinate index: %d != %d\n",bbi1->node[nodeX].k,bbi2->node[nodeX].k);
    }
    if (bbi1->node[nodeX].h != bbi2->node[nodeX].h) {
      INFO("node %d hyperplane: %f != %f\n",bbi1->node[nodeX].h,bbi2->node[nodeX].h);
    }
  }

  INFO("check leafs\n");  
  for (leafX=0; leafX<leafN; leafX++) {
    int k, cbvX;
    for (k=0; k<cbN; k++) {
      leaf1 = &bbi1->leaf[leafX][k];
      leaf2 = &bbi2->leaf[leafX][k];
      if (leaf1->cbvN != leaf2->cbvN) {
	INFO("leaf=%d cb=%d cbvN : %d != %d\n",leafX,k,leaf1->cbvN,leaf2->cbvN);
      } else {
	for (cbvX=0; cbvX<leaf1->cbvN; cbvX++) {
	  if (leaf1->cbX[cbvX] != leaf2->cbX[cbvX]) {
	    INFO("leaf=%d cb=%d cbvX=%d : %d != %d\n",leafX,k,cbvX,leaf1->cbX[cbvX],leaf2->cbX[cbvX]);
	  }
	}
      }
    }
  }
  return TCL_OK;
}


/* ----------------------------------------------------------------------- */
/*                                 BBITree                                 */
/* ----------------------------------------------------------------------- */

static Method bbiMethod[] = 
{ 
  { "puts",      bbiPutsItf,        NULL },
  { "compare",   bbiCompareItf,     NULL },
  {  NULL,  NULL, NULL } 
};


TypeInfo bbiInfo = {
        "BBITree", 0, -1, bbiMethod,
         NULL, NULL, 
         bbiConfigureItf, bbiAccessItf, 
         itfTypeCntlDefaultNoLink,
	"BBI search tree\n" } ;



static int bbiInitialized = 0;

int BBI_Init ( )
{
  if ( bbiInitialized) return TCL_OK;

  itfNewType ( &bbiNodeInfo );
  itfNewType ( &bbiLeafInfo );
  itfNewType ( &bbiInfo );

  bbiInitialized = 1;
  return TCL_OK;
}
