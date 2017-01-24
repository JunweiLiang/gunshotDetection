/* ========================================================================
    JANUS      Speech- to Speech Translation
               ------------------------------------------------------------
               Object: Consensus rescoring code
               ------------------------------------------------------------

    Author  :  Florian Metze
    Module  :  glatConsensus.c
    Date    :  $Id: glatConsensus.c 3416 2011-03-23 03:02:18Z metze $
    Remarks : 

   ========================================================================

    This software was developed by

    the  University of Karlsruhe       and  Carnegie Mellon University
         Dept. of Informatics               Dept. of Computer Science
         ILKD, Lehrstuhl Prof.Waibel        Alex Waibel's NN & Speech Group
         Am Fasanengarten 5                 5000 Forbes Ave
         D-76131 Karlsruhe                  Pittsburgh, PA 15213
         - Germany -                        - USA -

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
   Revision 4.3  2005/03/04 09:21:17  metze
   Bugfix for sorter, added 'dictWords' option

   Revision 4.2  2004/05/27 11:28:36  metze
   Increased table size for larger lattices

   Revision 4.1  2003/08/14 11:20:02  fuegen
   Merged changes on branch jtk-01-01-15-fms (jaguar -> ibis-013)

   Revision 1.1.2.24  2003/05/08 16:41:51  soltau
   Purified!

   Revision 1.1.2.23  2003/04/05 15:16:12  metze
   More checks

   Revision 1.1.2.22  2003/04/01 08:53:11  metze
   Consensus changes

   Revision 1.1.2.21  2003/03/23 16:25:43  metze
   Faster Consensus

   Revision 1.1.2.20  2003/03/19 14:24:37  metze
   Faster consensus for multiple lattices using same frame width

   Revision 1.1.2.19  2003/02/28 16:17:34  metze
   A posteriori rescoring of lattices

   Revision 1.1.2.18  2003/02/18 18:18:16  metze
   Final (?) version of Consensus

   Revision 1.1.2.17  2003/02/12 13:51:08  metze
   Rewrite of Consensus code

   Revision 1.1.2.16  2003/02/05 09:20:35  soltau
   Closed memory leak

   Revision 1.1.2.15  2003/01/28 16:20:35  metze
   Consensus with BitField code

   Revision 1.1.2.14  2003/01/17 09:37:26  metze
   Where did that ";" go?

   Revision 1.1.2.13  2003/01/17 09:25:55  metze
   Inter-word merging now uses time information

   Revision 1.1.2.12  2003/01/10 10:39:54  metze
   Removed C++ style comments

   Revision 1.1.2.11  2003/01/07 14:59:38  metze
   Consensus changes, added frameShift

   Revision 1.1.2.10  2002/12/20 14:53:03  metze
   Error messages

   Revision 1.1.2.9  2002/12/13 10:51:46  metze
   Consensus similar to Lidia's default case

   Revision 1.1.2.8  2002/12/12 15:29:48  metze
   In simpler cases we can now recreate Lidia's behaviour exactly

   Revision 1.1.2.7  2002/12/05 15:20:52  metze
   Stable version with doubtful scaling

   Revision 1.1.2.6  2002/12/04 17:07:57  metze
   Fixed a bug in Intra-merging

   Revision 1.1.2.5  2002/12/03 13:48:46  metze
   Sorry!

   Revision 1.1.2.4  2002/12/03 13:48:09  metze
   Still has problems with order of cluster

   Revision 1.1.2.3  2002/12/02 10:59:51  metze
   Seems to be ok, want to re-organize the structure

   Revision 1.1.2.2  2002/12/01 17:36:07  metze
   Works, but ordering is broken

   Revision 1.1.2.1  2002/12/01 13:50:58  metze
   Added glatConsensus


   ======================================================================== */


char  glatConsensusRcsVersion[]=
    "$Id: glatConsensus.c 3416 2011-03-23 03:02:18Z metze $";

#include <signal.h>
#include "itf.h"
#include "glat.h"
#include "svmap.h"
#include "stree.h"
#include "spass.h"


/* ========================================================================
    Definitions local to glatConsensus
   ======================================================================== */


typedef struct GLATARRAY_S   GLatArray;
typedef struct CLUSTER_S     Cluster;
typedef struct CLUSTERITEM_S ClusterItem;

#define lvXname(p,x) (LVX_NIL == (x)) ? (p)->string.unk : (p)->lksP->itemA ((p)->lksP, (x))


/* Definitions for ordering information:
   If the cell is ORDER_NONE, it is being computed, because they are unordered
   Any value greater than ORDER_NONE is regarded as a valid similarity
   PRE/ POST/ SAME define a relationship between clusters,
   ORDER_TEMP means that the clusters cannot be merged for now (but they
   are not ordered) */

#define ORDER_PRE  -50
#define ORDER_POST -40
#define ORDER_SAME -30
#define ORDER_TEMP -20
#define ORDER_NONE -10


typedef unsigned int CONS_FIELD_TYPE;

#define FieldSize (sizeof(CONS_FIELD_TYPE)*8)
#define SETBIT(c,i)   ((c)[i/FieldSize] |= (                     (1 << (i%FieldSize))))
#define TESTBIT(c,i)  ((c)[i/FieldSize] &  (                     (1 << (i%FieldSize))))
#define UNSETBIT(c,i) ((c)[i/FieldSize] &= (0xffffffffffffffff ^ (1 << (i%FieldSize))))


/* A GLatArray  contains several lattices and  the respective SVMap to
   use for Consensus. The SVMaps can be different, but the LVX and SVX
   themselves should be identical (no  checks are made). The output is
   done using the LVX/    SVX/ frameShift information from the   first
   GLatArray (the one to which the "consensus" method was applied) */

struct GLATARRAY_S {
  int               itemN;
  GLat**            itemA;
  SVMap**           mapA;
  Cluster***        clusterPA;
  CONS_FIELD_TYPE** fieldPA;
  int*              fieldMaxA;
};


/* In the beginning, a "consensus" cluster consists  of a set of words
   with identical LVX,  frameS, and  frameE.  The GNodes belonging  to
   this set  are  linked via ClusterItems  (itemP). When  merging  two
   clusters, one cluster is removed from the  list of clusters (nextP/
   childP)  and added   to the   other  cluster's childP  in case   of
   different LVXs or the cluster is deleted and the items are appended
   to the  other cluster's itemP. The  gammas in a  cluster are scaled
   while the gammas in the GNodes are not. */

struct CLUSTER_S {
  int          idx;
  LVX          lvX;
  double       gamma;
  int          postN;
  Cluster*     nextP;
  Cluster*     txenP;
  Cluster*     childP;
  ClusterItem* itemP;
  int          frameS;
  int          frameE;
};


/* Interface structure between consensus Clusters and GNodes */

struct CLUSTERITEM_S {
  GNode*       nodeP;
  int          latIdx;
  ClusterItem* nextP;
};


/* Forward declarations*/

static int putsCluster (GLatArray* arrP, Cluster* clusterP);


/* ========================================================================
    Helper functions
   ======================================================================== */

static double timeSim (GNode* iN, GNode* jN)
{
  int s = (iN->frameS < jN->frameS) ? jN->frameS : iN->frameS;
  int e = (iN->frameE < jN->frameE) ? iN->frameE : jN->frameE;
      
  if (s < e)
    return 2.0 * (e-s) / (iN->frameE+jN->frameE-iN->frameS-jN->frameS);

  return 0.0;
}


/* ------------------------------------------------------------------------
     intraScore
   --------------
     Compute the score for (intra-word) merging of two clusters (step 1):
     = max overlap(w1,w2)*p(w1)*p(w2)
     = ORDER_TEMP if no overlap
   ------------------------------------------------------------------------ */

static float intraScore (Cluster* ci, Cluster* cj, int mode, double scale)
{
  double      res = ORDER_TEMP;
  ClusterItem* iP = ci->itemP;

  while (iP) {
    GNode*       iN = iP->nodeP;
    ClusterItem* jP = cj->itemP;

    while (jP) {
      GNode*    jN = jP->nodeP;
      double     d = timeSim (iN, jN);
      
      if (d) {
	double sim  = iN->gamma + jN->gamma;
	sim         = d * ((sim < 0.0) ? 1.0 : exp (-scale*sim));
	if (sim > res)
	  res = sim;
      }

      jP = jP->nextP;
    }

    iP = iP->nextP;
  }

  return (float) res;
}


/* ------------------------------------------------------------------------
     Compute the score for the inter-word merging of clusters (step 2)
     At present, we don't use phonetic similarity
     FLT_MAX = they are ordered
       other = their score (higher is better)
   ------------------------------------------------------------------------ */

static float interScore (Cluster* ci, Cluster* ct, int mode, double scale)
{
  if        (mode == 3) {
    /* ----------------------------------------
         Maximum score function, but you need
         at least some temporal overlap
	 (new default)
       ---------------------------------------- */
    double res = SCORE_MAX;

    for ( ; ci; ci = ci->childP) {
      Cluster* cj = ct;      
      for (cj = ct; cj; cj = cj->childP) {
	double loc = ci->gamma + cj->gamma;
	if (ci->frameE > cj->frameS && ci->frameS < cj->frameE && loc < res)
	  res = loc;
      }
    }
    
    return (float) ((res <= 0.0) ? 1.0 : exp(-res));

  } else if (mode == 2) {
    /* ----------------------
         'Average' function
       ---------------------- */
    double res = 0.0;
    int    num = 0;
    
    while (ci) {
      Cluster* cj = ct;
      
      while (cj) {
	double loc = -ci->gamma -cj->gamma;
	res += (loc > 0) ? 1.0 : exp(loc);
	num++;
	cj = cj->childP;
      }
      
      ci = ci->childP;
    }
    
    return (float) (res/num);

  } else if (mode == 1) {
    /* --------------------------------------------------
         'Maximum' score function with time information
       -------------------------------------------------- */
    double res = ORDER_TEMP;
    
    while (ci) {
      Cluster* cj = ct;
      
      while (cj) {
	float loc = intraScore (ci, cj, 0, scale);
	if (loc > res)
	  res = loc;	
	cj = cj->childP;
      }
      
      ci = ci->childP;
    }
    
    return (float) res;
    
  } else {
    /* ---------------------------------------------------------------
         'Maximum' score function without time information (old default)
       --------------------------------------------------------------- */
    double res = ci->gamma + ct->gamma;
    
    while (ci) {
      Cluster* cj = ct;
      
      while (cj) {
	double loc = ci->gamma + cj->gamma;
	if (loc < res)
	  res = loc;
	cj = cj->childP;
      }
      
      ci = ci->childP;
    }
    
    return (float) ((res <= 0.0) ? 1.0 : exp(-res));
  }
}


/* ------------------------------------------------------------------------
     Compute the ordering constraints for all cluster pairs, i.e. write
     'FLT_MAX' whenever two pairs should not be merged. In this stage,
     all pointers are valid, because we haven't merged any clusters yet
   ------------------------------------------------------------------------ */

static CONS_FIELD_TYPE *latticeConstraint (GLat* latP, int* maxIdx)
{
#ifdef __INTEL_COMPILER
  CONS_FIELD_TYPE *restrict base;
  CONS_FIELD_TYPE *restrict ptr;
  CONS_FIELD_TYPE *restrict ptr2;
#else
  register CONS_FIELD_TYPE *base, *ptr, *ptr2;
#endif
  int i, j;
  GLink* linkP;
  
  *maxIdx = latP->nodeM / FieldSize +1;
  base    = (CONS_FIELD_TYPE*) calloc (latP->nodeM*(*maxIdx), sizeof (CONS_FIELD_TYPE));

  for (i = latP->nodeM-1; i >= 0; i--) {
    if (!latP->mapping.x2n[i])
      continue;

    linkP = latP->mapping.x2n[i]->childP;
    ptr   = base+(*maxIdx)*i;
    SETBIT (ptr, i);

    while (linkP) {
      if (linkP->childP->nodeX >= 0) {
	ptr2 = base+(*maxIdx)*linkP->childP->nodeX;

	for (j = 0; j < *maxIdx; j++)
	  ptr[j] |= ptr2[j];
      }

      linkP = linkP->nextP;
    }
  }

  return base;
}

static int orderCluster (Cluster* firstP, Cluster* cluster1P, Cluster** orderA, GLatArray *arrP, int idx)
{
  ClusterItem* item1P = cluster1P->itemP;

  /* We don't need to do any work, iff
     - This cluster has already been sorted:    cluster1P->idx >= 0
     - This cluster is currently being visited: cluster1P->idx == -2
     However, we already check for this before we do the recursion,
     so we don't need to check it here */
  /*
  if (cluster1P->idx >= 0)
    return idx;
  if (cluster1P->idx == -2)
    return idx;
  */

  /* This is the </s> cluster, so it's the last one and we're okay */
  if (!cluster1P->nextP) {
    cluster1P->idx = --idx;
    orderA[idx]    = cluster1P;
    return idx;
  }

  /* Mark this cluster as 'currently being visited' to
   avoid infinite recursion */
  cluster1P->idx = -2;

  /* Check all clusters, if they follow on this one and dive into them,
     if need be */
  while (item1P) {
    Cluster* cluster2P = firstP;

    while (cluster2P) {
      ClusterItem* item2P = cluster2P->itemP;

      if (cluster2P == cluster1P) {
	/* Don't check with this cluster if the two are identical
	   (or the second one has already been visited) */
	cluster2P = cluster2P->nextP;
	continue;
      }

      while (item2P) {
	if (item1P->latIdx == item2P->latIdx) {
	  /* Ok, check if we can deduce from these two GNodes,
	     that these clusters are ordered */
          CONS_FIELD_TYPE* fieldP = arrP->fieldPA[item1P->latIdx];
          int              maxIdx = arrP->fieldMaxA[item1P->latIdx];
          int                   i = item1P->nodeP->nodeX;
          int                   j = item2P->nodeP->nodeX;
	  if (cluster2P->idx == -1                  /* The cluster isn't visited already */
	      && arrP->clusterPA[item2P->latIdx][j] /* The second node is mapped to a cluster */
	      && TESTBIT (fieldP + maxIdx*i, j))    /* The nodes are followers */
	    idx = orderCluster (firstP, cluster2P, orderA, arrP, idx);
	}

	item2P = item2P->nextP;
      }

      cluster2P = cluster2P->nextP;
    }

    item1P = item1P->nextP;
  }
 
  /* Set ourselves on the new position */
  cluster1P->idx = --idx;
  orderA[idx]    = cluster1P;

  return idx;
}

static int constraintFromGNode (GNode* nodeP, Cluster** clusterPA, int i, int clusterM,
				float** mScoreA, char* did)
{
  GLink* linkP;
  float*    MM = mScoreA[i];

  for (linkP = nodeP->childP; linkP; linkP = linkP->nextP) {
    int nodeX = linkP->childP->nodeX;

    if (nodeX < 0 || did[nodeX])
      continue;

    if (clusterPA[nodeX]) {
      /* This child is mapped to a cluster */
      int  idx = clusterPA[nodeX]->idx;

      if (idx >= 0) {
	float* M = mScoreA[idx];
	int    j;
	
	for (j = i + 1; j < clusterM; j++)
	  if (M[j] == ORDER_POST || M[j] == ORDER_SAME)
	    MM[j] = ORDER_POST, mScoreA[j][i] = ORDER_PRE;
      }

    } else {
      /* This child is not mapped to a cluster -> recursion */
      constraintFromGNode (linkP->childP, clusterPA, i, clusterM, mScoreA, did);
    }
    
    did[nodeX] = 1;
  }
  
  return TCL_OK;
}

static int doConstraints (int clusterM, float** mScoreA, Cluster** orderA, GLatArray* arrP)
{
  char*  did = NULL;
  float*   m = mScoreA[0];
  int   i, j;

  for (i = j = 0; i < arrP->itemN; i++)
    if (arrP->itemA[i]->nodeM > j)
      j = arrP->itemA[i]->nodeM;
  did = (char*) malloc (sizeof (char) * j);

  /* Set constraints to 'NONE' */
  for (i = 1; i < clusterM; i++)
    mScoreA[i] = m + i * clusterM;
  for (i = 0; i < clusterM * clusterM; i++)
    m[i] = ORDER_NONE;

  /* Now do the constraints */
  for (i = 0; i < clusterM; i++)
    mScoreA[i][0] = mScoreA[clusterM-1][i]          = ORDER_PRE, 
    mScoreA[0][i] = mScoreA[i][clusterM-1]          = ORDER_POST;
  mScoreA[0][0]   = mScoreA[clusterM-1][clusterM-1] = ORDER_SAME;

  for (i = clusterM-2; i > 0; i--) {
    ClusterItem* itemP = orderA[i]->itemP;
     mScoreA[i][i] = ORDER_SAME;

    for (j = 0; j < arrP->itemA[itemP->latIdx]->nodeM; j++)
      did[j] = 0;

    /* Find all the clusters which we can reach from here */
    while (itemP) {
      constraintFromGNode (itemP->nodeP, arrP->clusterPA[itemP->latIdx], i, clusterM, mScoreA, did);
      itemP = itemP->nextP;
    }
  }

  free (did);

  return TCL_OK;
}

static float** fastConstraints (Cluster* firstP, int *clusterM, GLatArray* arrP, int v)
{
  float**    mScoreA = NULL;
  Cluster*  clusterP = firstP;
  Cluster**   orderA = (Cluster**) malloc (*clusterM * sizeof (Cluster*));
  int      i, j, min;
  
  /* Now, let's order our clusters according to topology: recursive calls to
     orderCluster, starting with the first cluster */
  while (clusterP)
    clusterP->idx = -1, clusterP = clusterP->nextP;
  if ((min = orderCluster (firstP, firstP, orderA, arrP, *clusterM)) || firstP != orderA[0]) {
    /* This can happen if we prune nodes and this leads to clusters not being
       visited by the ordering algorithm */
    for (j = min; j < *clusterM; j++) {
      orderA[j]->idx -= min;
      orderA[j-min]   = orderA[j];
    }
    *clusterM -= min;
  }

  /* Check for GNodes that are mapped to an unconnected cluster */
  for (i = 0; i < arrP->itemN; i++) {
    Cluster** cP =  arrP->clusterPA[i];
    for (j = 0; j < arrP->itemA[i]->nodeM; j++)
      if (cP[j] && cP[j]->idx < 0)
       	cP[j] = NULL;
  }

  /* Link our clusters in the new order */
  for (i = 1; i < *clusterM; i++) {
    assert (orderA[i-1]); // XCode says could be NULL
    orderA[i-1]->nextP = orderA[i];
    orderA[i]->txenP   = orderA[i-1];
  }
  assert (orderA[0]); // XCode says could be NULL
  orderA[0]->txenP           = NULL;
  orderA[*clusterM-1]->nextP = NULL;
  
  /* Allocate Memory for final lookup-table/ cache */
  mScoreA    = (float**) malloc (*clusterM *             sizeof (float*));
  if (!mScoreA) {
    INFO ("fastConstraints: couldn't allocate memory.\n");
    return NULL;
  }
  mScoreA[0] = (float*)  malloc (*clusterM * *clusterM * sizeof (float));
  if (!mScoreA[0]) {
    INFO ("fastConstraints: couldn't allocate mScoreA.\n");
    free (mScoreA);
    return NULL;
  }

  /* Now do the constraints */
  doConstraints (*clusterM, mScoreA, orderA, arrP);

  /* Free memory */
  for (i = 0; i < arrP->itemN; i++) {
    free (arrP->clusterPA[i]);
    free (arrP->fieldPA[i]);
  }
  free (arrP->clusterPA);
  free (arrP->fieldPA);
  free (arrP->fieldMaxA);
  free (orderA);
  
  return mScoreA;
}


/* ------------------------------------------------------------------------
     Add a node to a cluster
   ------------------------------------------------------------------------ */

static double myAddLogProbs (double p1,double p2) {
  if (p1 == 0.0) return p2;
  if (p2 == 0.0) return p1;
  if (p1 < p2) {
    return p1 -log(exp(p1-p2) + 1.0);  
  } else {
    return p2 -log(exp(p2-p1) + 1.0);
  }
}

static Cluster* addNode2Cluster (Cluster* clusterP, GNode* nodeP, GLat* latP, int idx, LVX lvX,
				 BMem* cluster, BMem* clusterItem, double scale, double offset)
{
  ClusterItem* nextP = NULL;
  double           g = offset + scale * ((nodeP->gamma < 0.0001) ? 0.0001 : nodeP->gamma);

  if (clusterP) { /* ------------------- Add an item to an existing cluster */
    nextP                 = clusterP->itemP;
    clusterP->gamma       = myAddLogProbs (clusterP->gamma, g);
    if (nodeP->frameS < clusterP->frameS)
      clusterP->frameS = nodeP->frameS;
    if (nodeP->frameE > clusterP->frameE)
      clusterP->frameE = nodeP->frameE;

  } else { /* ------------------------------------------ Create new cluster */
    clusterP              = bmemItem (cluster);
    clusterP->gamma       = g;
    clusterP->frameS      = nodeP->frameS;
    clusterP->frameE      = nodeP->frameE;
  }

  clusterP->itemP         = bmemItem (clusterItem);
  clusterP->itemP->nextP  = nextP;
  clusterP->itemP->nodeP  = nodeP;
  clusterP->itemP->latIdx = idx;

  if (lvX == latP->svmapP->idx.start || lvX == latP->svmapP->idx.end)
    clusterP->gamma = 0.0;

  return clusterP;
}


/* ------------------------------------------------------------------------
     Initialize the clusters
   ------------------------------------------------------------------------ */

static Cluster* add2Cluster (GLatArray* arrP, int idx, GNode* nodeP,
			     Cluster** frameA, Cluster* lastP, 
			     BMem* cluster, BMem* clusterItem, double scale, double offset)
{
  GLat*          latP = arrP->itemA[idx];
  SVMap*       svmapP = arrP->mapA[idx];
  Cluster** clusterPA = arrP->clusterPA[idx];
  LVX             lvX = (svmapP->svocabP->list.itemA[nodeP->svX].fillerTag)
                      ? LVX_NIL
                      : svmapGet (svmapP, nodeP->svX);
  Cluster*   clusterP = frameA[(lvX == svmapP->idx.start || lvX == svmapP->idx.end) ? 0 : nodeP->frameS];

  /* frameA[] points to the first node starting at a given frame to
     speed up the search. However, we start at the very first node for <s> and </s>
     to allow for the merging of these nodes */
  while (clusterP) {
    GLat* lP = arrP->itemA[clusterP->itemP->latIdx];
    if (lvX == clusterP->lvX &&
	((nodeP->frameS == clusterP->itemP->nodeP->frameS &&
	  nodeP->frameE == clusterP->itemP->nodeP->frameE &&
	  latP          == lP)                            ||
	 lvX == svmapP->idx.start || lvX == svmapP->idx.end)) {
      /* Found a suitable cluster, append an item to it. We need:
         - equal lvX AND
           - EITHER the lvX is <s>
           - OR     the lvX is </s>
           - OR     the times match exactly AND the lattices are the same */
      addNode2Cluster (clusterP, nodeP, latP, idx, lvX, cluster, clusterItem, scale, offset);
      break;
    }

    clusterP = clusterP->nextP;
  }

  if (!clusterP) {
    /* Didn't find a suitable cluster, create a new one */
    clusterP          = addNode2Cluster (NULL, nodeP, latP, idx, lvX, cluster, clusterItem, scale, offset);
    clusterP->lvX     = lvX;

    clusterP->nextP   = NULL;
    clusterP->childP  = NULL; 
    if (lastP) {
      clusterP->txenP = lastP;
      clusterP->idx   = lastP->idx+1;
      lastP->nextP    = clusterP;
    } else {
      clusterP->txenP = NULL;
      clusterP->idx   = 0;
    }

    lastP = clusterP;
  }

  clusterPA[nodeP->nodeX] = clusterP;

  if (!frameA[nodeP->frameS])
    frameA[nodeP->frameS] = lastP;

  return lastP;
}


/* ------------------------------------------------------------------------
     Merge two clusters (both steps)
   -----------------------------------
     bP -> aP, j -> i
   ------------------------------------------------------------------------ */

static int fixConstraints (float** mScoreA, int clusterM, int i, int j, Cluster* firstP)
{
  float* mScoreI = mScoreA[i];
  float* mScoreJ = mScoreA[j];

  int*     Ipre  = (int*) malloc (clusterM * sizeof (int));
  int*     Ipost = (int*) malloc (clusterM * sizeof (int));
  int*     Jpre  = (int*) malloc (clusterM * sizeof (int));
  int*     Jpost = (int*) malloc (clusterM * sizeof (int));

  float**    IA  = (float**) malloc (clusterM * sizeof (float*));
  float**    IO = (float**) malloc (clusterM * sizeof (float*));
  float**    JA  = (float**) malloc (clusterM * sizeof (float*));
  float**    JO = (float**) malloc (clusterM * sizeof (float*));

  Cluster*     P;

  int ia, io, ja, jo;

  for (P = firstP, ia = io = ja = jo = 0; P; P = P->nextP) {
    if (mScoreI[P->idx] == ORDER_PRE)  IA[ia] = mScoreA[P->idx], Ipre[ia++]  = P->idx; 
    if (mScoreI[P->idx] == ORDER_POST) IO[io] = mScoreA[P->idx], Ipost[io++] = P->idx; 
    if (mScoreJ[P->idx] == ORDER_PRE)  JA[ja] = mScoreA[P->idx], Jpre[ja++]  = P->idx; 
    if (mScoreJ[P->idx] == ORDER_POST) JO[jo] = mScoreA[P->idx], Jpost[jo++] = P->idx; 
  }

  for (P = firstP; P; P = P->nextP) {
    int          k = P->idx;
    float* mScoreK = mScoreA[k];
    int          l;

    if        (mScoreI[k] == ORDER_POST) {
      /* -------------------------------
	   Iff 'k' is a child of 'i'
         ------------------------------- */

      /* Add 'j' as a new parent */
      mScoreJ[k] = ORDER_POST, mScoreK[j] = ORDER_PRE;
      /* Add all parents of 'j' as new parents to 'k' */
      for (l = 0; l < ja; l++)
	JA[l][k] = ORDER_POST, mScoreK[Jpre[l]]  = ORDER_PRE;

    } else if (mScoreI[k] == ORDER_PRE) {
      /* -------------------------------
	   Iff 'k' is a parent of 'i'
         ------------------------------- */

      /* Add 'j' as a new child */
      mScoreJ[k] = ORDER_PRE, mScoreK[j] = ORDER_POST;
      /* Add all children of 'j' as new children to 'k' */
      for (l = 0; l < jo; l++)
	JO[l][k] = ORDER_PRE, mScoreK[Jpost[l]] = ORDER_POST;
    }

    if        (mScoreJ[k] == ORDER_POST) {
      /* -------------------------------
	   Iff 'k' is a child of 'j'
         ------------------------------- */

      /* Add 'i' as a new parent */
      mScoreI[k] = ORDER_POST, mScoreK[i] = ORDER_PRE;
      /* Add all parents of 'i' as new parents to 'k' */
      for (l = 0; l < ia; l++)
	IA[l][k] = ORDER_POST, mScoreK[Ipre[l]]  = ORDER_PRE;

    } else if (mScoreJ[k] == ORDER_PRE) {
      /* -------------------------------
	   Iff 'k' is a parent of 'j'
         ------------------------------- */

      /* Add 'i' as a new child */
      mScoreI[k] = ORDER_PRE, mScoreK[i] = ORDER_POST;
      /* Add all children of 'i' as new children to 'k' */
      for (l = 0; l < io; l++)
	IO[l][k] = ORDER_PRE, mScoreK[Ipost[l]] = ORDER_POST;
    }
  }

  free (IA), free (IO), free (JA), free (JO);
  free (Ipre), free (Ipost), free (Jpre), free (Jpost);

  return TCL_OK;
}

static int mergeClusters (GLatArray* arrP, Cluster* aP, Cluster* bP, Cluster* firstP,
			  float** mScoreA, int clusterM, int v, double score)
{
  Cluster*    oP = aP;
  Cluster*    tP = aP;
  int          i = aP->idx;
  int          j = bP->idx;
  int          k = 1;
  GLat*    lataP = arrP->itemA[aP->itemP->latIdx];
  GLat*    latbP = arrP->itemA[bP->itemP->latIdx];
  Cluster* nextP;
  Cluster* txenP;
  
  if (v & 8) {
    printf (">>> Merging %4d & %4d: %e %f %f %s %s\n    --------------------\n",
	    aP->idx, bP->idx, score, aP->gamma, bP->gamma,
	    lvXname (lataP->svmapP, aP->lvX),
	    lvXname (latbP->svmapP, bP->lvX));
    putsCluster (arrP, aP);
    putsCluster (arrP, bP);
  }

  /* Unlink bP */
  if (bP->txenP) bP->txenP->nextP = bP->nextP;
  if (bP->nextP) bP->nextP->txenP = bP->txenP;
  bP->txenP = bP->nextP = NULL;

  /* Remember the links from aP (free too) */
  nextP = aP->nextP;
  txenP = aP->txenP;
  aP->txenP = aP->nextP = NULL;

  /* Attach bP (and its children) to the children of aP */
  while (tP->childP) tP = tP->childP;
  tP->childP = bP;
  
  /* 1) We don't want the same LVX to appear twice */
  while (oP) {
    Cluster* uP = oP;
    tP          = oP->childP;
    while (tP) {
      if (oP->lvX == tP->lvX) {
	ClusterItem* iP  = oP->itemP;

	/* Merge the gammas */
	oP->gamma = myAddLogProbs (oP->gamma, tP->gamma);

	/* Attach the items from bP to oP */
	while (iP->nextP) iP = iP->nextP;
	iP->nextP = tP->itemP;

	/* Unlink tP (just to be sure) */
	uP->childP = tP->childP;
      }
      uP = tP;
      tP = tP->childP;
    }
    oP = oP->childP;
  }

  /* 2) Sort the list of children (gamma) */
  while (k) {
    Cluster* lP = NULL;
    oP = aP;
    k  = 0;
    while (oP->childP) {
      tP = oP->childP;
      if (oP->gamma > tP->gamma) {
	oP->childP = tP->childP;
	tP->childP = oP;
	if (lP) lP->childP = tP;
	else    aP         = tP;
	k = 1;
      }
      lP = oP;
      oP = tP;
    }
  }  

  /* Splice our top node 'aP' back in */
  aP->txenP = txenP;
  aP->nextP = nextP;
  if (txenP) txenP->nextP = aP;
  if (nextP) nextP->txenP = aP;

  /* Fix the constraints (new ones introduced by merging j and i) */
  fixConstraints (mScoreA, clusterM, i, j, firstP);

  /* Fix the score cache */
  for (k = 0; k < clusterM; k++)
    if (mScoreA[k][i] >= ORDER_TEMP || mScoreA[k][i] >= ORDER_TEMP ||
	mScoreA[k][j] >= ORDER_TEMP || mScoreA[j][k] >= ORDER_TEMP)
      mScoreA[k][i] = mScoreA[i][k] = ORDER_NONE;

  return TCL_OK;
}


/* ------------------------------------------------------------------------
     Sort the clusters according to the number of followers they have
   ------------------------------------------------------------------------ */

static int vgl (const void *a, const void *b)
{
  Cluster* A = *(Cluster**) a;
  Cluster* B = *(Cluster**) b;
  if (A->postN < B->postN) return  1;
  if (A->postN > B->postN) return -1;
  return 0;
}

static int sortClusters (Cluster* firstP, float** mScoreA, int clusterN, int clusterM)
{
  Cluster* clusterP = firstP;
  Cluster**  orderA = (Cluster**) malloc (clusterN * sizeof (Cluster*));
  Cluster**      cP = orderA;
  int              i;

  /* Create an array of our cluster pointers */
  while (clusterP) {
    *(cP++)  = clusterP;

    clusterP->postN = 0;
    for (i = 0; i < clusterM; i++)
      if (mScoreA[clusterP->idx][i] == ORDER_POST)
	clusterP->postN++;

    clusterP = clusterP->nextP;
  }

  /* Sort our clusterN clusters in descending order of number of followers */
  qsort (orderA, clusterN, sizeof (Cluster*), vgl);

  /* Relink our clusters */
  for (i = 1; i < clusterN; i++) {
    orderA[i-1]->nextP = orderA[i];
    orderA[i]->txenP   = orderA[i-1];
  }
  orderA[0]->txenP = orderA[clusterN-1]->nextP = NULL;

  free (orderA);
  return TCL_OK;
}


/* ------------------------------------------------------------------------
     Print out clusters and the sets - what else?
   ------------------------------------------------------------------------ */

static int putsCluster (GLatArray* arrP, Cluster* clusterP)
{
  Cluster* childP = clusterP;

  while (childP) {
    ClusterItem* itemP = childP->itemP;
    GLat*         latP = arrP->itemA[itemP->latIdx];
    SWord*      swordP = latP->svmapP->svocabP->list.itemA; 
    LingKS*       lksP = latP->svmapP->lksP;
    char*          txt = (childP->lvX == LVX_NIL) ? "$" : lksP->itemA (lksP, childP->lvX);
    if (childP == clusterP) printf ("CL ");
    else                    printf ("   ");
    printf ("%4d (%s %.3f)\n       ", childP->idx, txt, childP->gamma);
    
    while (itemP) {
      printf (" [%s %.3f %d %d %d]", swordP[itemP->nodeP->svX].name, itemP->nodeP->gamma, 
	      itemP->nodeP->frameS, itemP->nodeP->frameE, itemP->nodeP->nodeX);
      itemP = itemP->nextP;
    }
    printf ("\n\n");
    
    childP = childP->childP;
  }

  return TCL_OK;
}


static int putsStatus (GLatArray* arrP, Cluster* firstP, int clusterN, int clusterM)
{
  Cluster* clusterP = firstP;

  printf ("\n*************************************\n");
  printf ("***  Now have %4d/ %4d clusters ***\n", clusterN, clusterM);
  printf ("*************************************\n\n");

  while (clusterP) {
    putsCluster (arrP, clusterP);
    clusterP = clusterP->nextP;
  }

  return TCL_OK;
}


static int putsSets (GLatArray* arrP, Cluster* firstP, int clusterN, int clusterM,
		     double cutoff, double silScale, char* silWord, int v, int sl)
{
  char     probS[524288];
  char     bestS[524288];
  char*    pS = probS;
  char*    bS = bestS;
  char*    ok = NULL;
  double conf = 0.0;
  int    cnfC = 0.0;
  Cluster* clusterP = firstP;
  
  ok = (char *) calloc (clusterM, sizeof (char));

  /* This function prints out the final result of the computation. 
     The words get written directly, the rest is buffered.
     Edit it, if you need extra output (verbosity) */
  itfAppendResult ("{ ");
  
  /* Loop over all clusters (these are now ordered topologically) */
  while (clusterP) {
    Cluster* childP = clusterP;
    double      max = FLT_MAX;
    double  thismax = 0.0;
    double     prob = 0.0;
    int       itemN = 0;

    /* Compute the silence probability */
    while (childP) {
      thismax  = exp(-childP->gamma);
      prob    += thismax;
      if (thismax > cutoff) itemN++;
      childP = childP->childP;
    }
    prob = (prob > 1.0) ? 0.0 : 1.0 - prob/silScale;

    /* Did we find something interesting here? */
    if (!itemN) {
      clusterP = clusterP->nextP;
      continue;
    }
    
    /* Print out the list of alternatives */
    if (prob > cutoff) itemN++;
    if (itemN > 1) {
      itfAppendResult ("{ ");
      sprintf     (pS, "{ "), pS += strlen (pS);
    }

    childP = clusterP;
    while (childP && (max = exp(-childP->gamma)) > cutoff) {
      ClusterItem* itemP = childP->itemP;
      GLat*         latP = arrP->itemA[itemP->latIdx];
      SVMap*      svmapP = latP->svmapP;
      SWord*      swordP = svmapP->svocabP->list.itemA; 
      double      lgamma = FLT_MAX;
      GNode*       nodeP = itemP->nodeP;
      char*         ltxt = (childP->lvX == LVX_NIL)
	                 ? svmapP->string.unk
	                 : svmapP->lksP->itemA (svmapP->lksP, childP->lvX);
      char*         stxt = ltxt;

      if (max > 1.0) max = 1.0;

      /* Find the most likely node (svX) */
      while (itemP) {
	if (itemP->nodeP->gamma < lgamma && itemP->nodeP->svX < (SVX) svmapP->svocabP->list.itemN) {
	  lgamma = itemP->nodeP->gamma;
	  stxt   = swordP[itemP->nodeP->svX].name;
	  nodeP  = itemP->nodeP;
	}
	itemP = itemP->nextP;
      }
      
      /* Append the word to the hypo, if it's the best (first) word */
      if (childP == clusterP) {
	if (max > prob) {
	  conf += max; cnfC++;
	  if (v&1) sprintf (bS, "{%s %d %d %f} ",
			    stxt, nodeP->frameS, nodeP->frameE, max), bS += strlen (bS);
	  else     sprintf (bS, "%s ", stxt), bS += strlen (bS);
	} else {
	  /* conf += prob; (This we don't take into account,
	                   because it's not part of the best path) */ 
	  if (strlen (silWord)) {
	    if (v&1) sprintf (bS, "{%s %d %d %f} ", silWord,
			      nodeP->frameS, nodeP->frameE, prob), bS += strlen(bS);
	    else     sprintf (bS, "%s ", silWord), bS += strlen(bS);
	  }
	}
      }

      /* Print out silence, if necessary */
      if (prob > max) {
	if (sl) itfAppendResult ("$ ");
	else    itfAppendResult ("@ ");
	sprintf     (pS, "%.3f ", prob), pS += strlen (pS);
	prob = 0.0;
      }

      /* Append our word to the list */
      itfAppendResult ("%s ",   ((sl) ? stxt : ltxt));
      sprintf     (pS, "%.3f ", max), pS += strlen (pS);

      childP = childP->childP;
    }

    /* Print out silence, if we haven't done so before */
    if (prob > cutoff) {
      if (sl) itfAppendResult ("$ ");
      else    itfAppendResult ("@ ");
      sprintf     (pS, "%.3f ", prob), pS += strlen (pS);
      prob = 0.0;
    }

    /* Close our list, if necessary */
    if (itemN > 1) {  
      itfAppendResult ("} ");
      sprintf     (pS, "} "), pS += strlen (pS);
    }
    
    clusterP = clusterP->nextP;
  }
  
  free (ok);

  if (cnfC == 2) cnfC = 3;

  itfAppendResult ("} { %s} { %d %f %s}", probS, clusterM, (conf-2.0)/(cnfC-2), bestS);

  return TCL_OK;
}


static int putsPairs (float** mScoreA, int clusterM)
{
  int i, j;

  printf (">>>> Ordered pairs: ");
  for (i = 0; i < clusterM; i++) {
    printf ("\n%d ->", i);
    for (j = 0; j < clusterM; j++)
      if (mScoreA[i][j] == ORDER_POST) printf (" %d", j);
  }
  printf ("\n");

  return TCL_OK;
}

static int putsField (GLatArray* arrP, int idx, float beam)
{
  CONS_FIELD_TYPE* base = arrP->fieldPA[idx];
  GLat*            latP = arrP->itemA[idx];
  int            maxIdx = latP->nodeM / FieldSize + 1;
  int              i, j;

  printf ("%8d: ", latP->nodeN);
  for (i = 0; i < latP->nodeM; i++) {
    if (!latP->mapping.x2n[i] || latP->mapping.x2n[i]->gamma > beam) printf (" ");
    else if (!(i%5))                                         printf (":");
    else                                                     printf (".");
  }
  printf ("\n");
  for (i = 0; i < latP->nodeM; i++) {
    CONS_FIELD_TYPE* ptr = base+maxIdx*i;

    printf ("%8d: ", i);
    for (j = 0; j < latP->nodeM; j++) {
      if TESTBIT (ptr, j) printf ("*");
      else                printf (".");
    }
    printf ("\n");
  }
  printf ("\n");

  return TCL_OK;
}


/* ========================================================================
     GLAT code to do consensus rescoring
   ---------------------------------------
     This code is inspired by Lidia Mangu's work
     We reach roughly the same performance by writing lattices in
     SLF format and using Lidia's consensus or by using our 
     implementation.
   ======================================================================== */

static int glatComputeConsensus (GLatArray* arrP, double cutoff, char* silWord,
				 double beam, double scale, double silScale, 
				 int intraM, int interM, int v, int sl)
{
  BMem*       clusterMem = bmemCreate (1000, sizeof (Cluster));
  BMem*          itemMem = bmemCreate (5000, sizeof (ClusterItem));
  Cluster*        firstP = NULL;
  Cluster*         lastP = firstP;
  float**        mScoreA = NULL;
  float            score = ORDER_NONE+1;
  int           clusterN = 0;
  int           clusterM = 0;
  int             frameM = 0;
  Cluster**       frameA = NULL;
  int             i, idx;
  clock_t           zeit;

  MostRecentlyReceivedSignal = 0;

  /* --------------------
       0) Sanity checks
     -------------------- */

  for (idx = 1; idx < arrP->itemN; idx++) {
    SVMap* map1P = arrP->mapA[idx-1];
    SVMap* map2P = arrP->mapA[idx];
    if (map1P->idx.start != map2P->idx.start ||
	map1P->idx.end   != map2P->idx.end   ||
	map1P->idx.unk   != map2P->idx.unk) {
      ERROR ("Please make sure the SVMaps are compatible!\n");
      return TCL_ERROR;
    }
  }
  arrP->clusterPA = (Cluster***)        malloc (arrP->itemN * sizeof (Cluster**));
  arrP->fieldPA   = (CONS_FIELD_TYPE**) malloc (arrP->itemN * sizeof (CONS_FIELD_TYPE*));
  arrP->fieldMaxA = (int*)              malloc (arrP->itemN * sizeof (int));


  /* ------------------------------
       1) Create initial clusters
     ------------------------------ */
  if (v & 2) printf ("> Creating initial clusters ...\n");
  zeit = clock ();

  for (idx = 0; idx < arrP->itemN; idx++) {
    GLat*    latP = arrP->itemA[idx];
    double offset = log (arrP->itemN);
    int      useN = 0;
    int    frameX;

    arrP->clusterPA[idx] = (Cluster**) calloc (latP->nodeM, sizeof (Cluster*));
    arrP->fieldPA[idx]   = latticeConstraint  (latP, arrP->fieldMaxA+idx);

    if (v & 32) putsField (arrP, idx, beam);

    /* Generate x2n table */
    glatAssureMapTable (latP, latP->nodeN+2);
    if (!latP->mapping.x2n)
      glatCount (latP);
    
    /* Create the 'sorter' structure */
    glatSortNodes (latP);
    
    /* Create the lookup table */
    if (frameM < latP->sorter.frameN) {
      frameA = (Cluster**) realloc (frameA, latP->sorter.frameN * sizeof (Cluster*));
      for (frameX = frameM; frameX < latP->sorter.frameN; frameX++)
	frameA[frameX] = NULL;
      frameM = latP->sorter.frameN;
    }

    /* Loop over all start frames */
    for (frameX = 0; frameX < latP->sorter.frameN; frameX++) {
      for (i = 0; i < latP->sorter.endCntA[frameX]; i++) {
	GNode* nodeP = latP->sorter.endPA[frameX][i];

	/* Pruning, simple and effective */
	if (nodeP->gamma > beam) {
	  arrP->clusterPA[idx][nodeP->nodeX] = NULL;
	  continue;
	}

	lastP = add2Cluster (arrP, idx, nodeP, frameA, lastP, 
			     clusterMem, itemMem, scale, offset);
	if (!firstP) firstP = lastP;
	if (lastP->gamma > 0.001)
	  useN++;
      }
      
      if (MostRecentlyReceivedSignal == SIGINT) {
	free (frameA);
	goto abgang;
      }
    }
    if (!useN) WARN ("glatComputeConsensus: all gammas zero in %s.\n", latP->name);
  }
  if (!lastP) {
    INFO ("glatComputeConsensus: couldn't find any nodes, sorry.\n");
    return TCL_OK;
  }
  clusterM = lastP->idx+1;
  free (frameA);

  if (v & 4) putsStatus (arrP, firstP, clusterM, clusterM);


  /* ---------------------------------------------------------
       2) Compute the ordering (later the merge scores, too)
     --------------------------------------------------------- */
  if (v & 2) printf ("  t: %f\n> Re-sorting and computing constraints ...\n",
		     (float)(clock()-zeit)/CLOCKS_PER_SEC);
  zeit = clock ();

  mScoreA  = fastConstraints (firstP, &clusterM, arrP, v);
  clusterN = clusterM;

  if (v & 4) putsStatus (arrP, firstP, clusterN, clusterM);

  if (v & 16) putsPairs (mScoreA, clusterM);


  /* ----------------------------
       3) Intra-word clustering
     ---------------------------- */
  if (v & 2) printf ("  t: %f\n> Intra-word clustering (%d) ...\n",
		     (float)(clock()-zeit)/CLOCKS_PER_SEC, clusterN);
  zeit = clock ();

  while (score > ORDER_NONE) {
    Cluster* iP = firstP;
    Cluster* aP = NULL;
    Cluster* bP = NULL;
    score       = ORDER_NONE;

    while (iP->nextP) {
      Cluster* jP = iP->nextP;
      while (jP) {
	float* scoreP = (iP->idx < jP->idx) ? mScoreA[iP->idx]+jP->idx : mScoreA[jP->idx]+iP->idx;

	if (iP->lvX != jP->lvX || *scoreP < ORDER_NONE) {
	  jP = jP->nextP;
	  continue;
	}

	/* Fill the merge score cache:
	   ORDER_* or score (has to be greater than ORDER_NONE) */
	if (*scoreP == ORDER_NONE)
	  *scoreP = intraScore (iP, jP, intraM, scale);

	/* Score is better? */
	if (*scoreP > score)
	  aP = iP, bP = jP, score = *scoreP;

	jP = jP->nextP;
      }
      iP = iP->nextP;
    }

    /* Merge two clusters */
    if (score > ORDER_NONE) {
      clusterN--;
      mergeClusters (arrP, aP, bP, firstP, mScoreA, clusterM, v, score);
    }

    if (MostRecentlyReceivedSignal == SIGINT)
      goto abgang;
  }

  if (v & 4) putsStatus (arrP, firstP, clusterN, clusterM);


  /* ----------------------------
       4) Inter-word clustering
     ---------------------------- */
  if (v & 2) printf ("  t: %f\n> Preparing for inter-word clustering.\n",
		     (float)(clock()-zeit)/CLOCKS_PER_SEC);
  zeit = clock ();

  /* Let's make sure that we don't have any scores 
     floating around from the intra-word clustering step
     (but we need to retain the ordering information) */

  for (i = 0; i < clusterM*clusterM; i++)
    if (mScoreA[0][i] >= ORDER_TEMP)
      mScoreA[0][i] = ORDER_NONE;

  if (v & 16) putsPairs (mScoreA, clusterM);

  if (v & 2) printf ("  t: %f\n> Inter-word clustering (%d) ...\n",
		     (float)(clock()-zeit)/CLOCKS_PER_SEC, clusterN);
  zeit = clock ();

  score = ORDER_NONE+1.0;
  while (score > ORDER_NONE) {
    Cluster* iP = firstP;
    Cluster* aP = NULL;
    Cluster* bP = NULL;
    score       = ORDER_NONE;

    while (iP->nextP) {
      Cluster* jP = iP->nextP;
      while (jP) {
	float* scoreP = (iP->idx < jP->idx) ? mScoreA[iP->idx]+jP->idx : mScoreA[jP->idx]+iP->idx;

	if (*scoreP < ORDER_NONE) {
	  jP = jP->nextP;
	  continue;
	}

	/* Fill the merge score cache */
	if (*scoreP == ORDER_NONE)
	  *scoreP = interScore (iP, jP, interM, scale);

	/* Score is better? */
	if (*scoreP > score)
	  aP = iP, bP = jP, score = *scoreP;

	jP = jP->nextP;
      }
      iP = iP->nextP;
    }

    /* Merge two clusters */
    if (score > ORDER_NONE) {
      clusterN--;
      mergeClusters (arrP, aP, bP, firstP, mScoreA, clusterM, v, score);
    }

    if (MostRecentlyReceivedSignal == SIGINT)
      goto abgang;
  }


  /* ---------------------------------
       5) Sorting, Output & Clean up
     --------------------------------- */
  if (v & 2) printf ("  t: %f\n> Sorting and output.\n",
		     (float)(clock()-zeit)/CLOCKS_PER_SEC);

  sortClusters (firstP, mScoreA, clusterN, clusterM);

  if (v & 4) putsStatus (arrP, firstP, clusterN, clusterM);
  
  if (v & 16) putsPairs (mScoreA, clusterM);

  putsSets (arrP, firstP, clusterN, clusterM, cutoff, silScale, silWord, v, sl);

 abgang:
  assert (mScoreA); // XCode says could be NULL
  free (mScoreA[0]);
  free (mScoreA);

  bmemFree (clusterMem);
  bmemFree (itemMem);

  for (idx = 0; idx < arrP->itemN; idx++)
    arrP->itemA[idx]->sorter.frameN = 0;

  return TCL_OK;
}


/* ========================================================================
     Interface for glatConsensus
   ======================================================================== */

int getGLatsItf (ClientData CD,     char* key, 
                 ClientData retPtr, int*  acPtr, char* av[])
{
  GLatArray* latA = (GLatArray*) retPtr;
  int      offset = latA->itemN;
  int     argc;
  char**  argv;

  if (*acPtr < 1)
    return -1;

  /* Did we get any lattices (with SVMap)? */
  if (Tcl_SplitList (itf, av[0], &argc, &argv) == TCL_OK) {
    int i;

    /* Reserve some space */
    latA->itemN += argc;
    latA->itemA  = (GLat**)  realloc (latA->itemA, latA->itemN*sizeof (GLat*));
    latA->mapA   = (SVMap**) realloc (latA->mapA,  latA->itemN*sizeof (SVMap*));

    for (i = 0; i < argc; i++) {
      int     tokc;
      char**  tokv;

      if (Tcl_SplitList (itf, argv[i], &tokc, &tokv) == TCL_OK) {
        int    ac = tokc;
        char** av = tokv;
        char*  sn = NULL;
        char*  st = NULL;
	
        if (itfParseArgv (argv[0], &ac, av, 0,
          "<GLat>",      ARGV_OBJECT, NULL, &sn, "GLat",  "GLat object",
          "-map",        ARGV_OBJECT, NULL, &st, "SVMap", "SVMap to use",
          NULL) != TCL_OK) {
          Tcl_Free ((char*)argv);
	  Tcl_Free ((char*)tokv);
          return TCL_ERROR;
        }

	/* We got some objects */
	latA->itemA[offset+i] =        (GLat*)  sn;
	latA->mapA[offset+i]  = (st) ? (SVMap*) st : latA->itemA[offset+i]->svmapP;
      }
      
      Tcl_Free ((char*)tokv);
    }
  }

  Tcl_Free ((char*)argv);

  (*acPtr)--;
  return 0;
}

int glatConsensusItf (ClientData cd, int argc, char *argv[])
{
  GLat*      latP = (GLat*) cd;
  SVMap*   svmapP = latP->svmapP;
  int         ac  = argc -1;
  float    cutoff = 0.01;
  char*    intraM = "max";
  char*    interM = "max";
  char*   silWord = "";
  int         iaM = 0;
  int         ieM = 3; 
  float      beam = 100.0;
  float     scale = 1.0;
  float  silScale = 1.0;
  int     verbose = 0;
  int          sl = 0;
  GLatArray  latA;
  int           rc;
  
  latA.itemA = (GLat**)  malloc (sizeof (GLat*));
  latA.mapA  = (SVMap**) malloc (sizeof (SVMap*));
  latA.itemN = 1;

  if (itfParseArgv (argv[0], &ac, argv+1, 0,
    "-lats",        ARGV_CUSTOM, getGLatsItf, &latA,     cd,      "extra list of lattices",
    "-map",         ARGV_OBJECT, NULL,        &svmapP,   "SVMap", "Vocabulary Mapper",    
    "-beam",        ARGV_FLOAT,  NULL,        &beam,     "",      "pruning beam",    
    "-scale",       ARGV_FLOAT,  NULL,        &scale,    "",      "score scaling factor",    
    "-silScale",    ARGV_FLOAT,  NULL,        &silScale, "",      "silence prob scaling factor",    
    "-cutoff",      ARGV_FLOAT,  NULL,        &cutoff,   "",      "cutoff probability for output",    
    "-silWord",     ARGV_STRING, NULL,        &silWord,  "",      "word to use for missed words",
    "-intra",       ARGV_STRING, NULL,        &intraM,   "",      "intra-class merging method (max or avg)",    
    "-inter",       ARGV_STRING, NULL,        &interM,   "",      "inter-class merging method (max, avg, old, or time)",    
    "-verbose",     ARGV_INT,    NULL,        &verbose,  "",      "verbosity",
    "-dictWords",   ARGV_INT,    NULL,        &sl,       "",      "output dictionary words instead of lm words",
    NULL) != TCL_OK) return TCL_ERROR;

  latA.itemA[0] = latP;
  latA.mapA[0]  = svmapP;

  if (beam <= 0.0) {
    beam = 1.0;
    INFO ("glatConsensusItf: setting beam to %f.\n", beam);
  }

  if      (streq (intraM, "avg"))  iaM = 1;
  if      (streq (interM, "avg"))  ieM = 1;
  else if (streq (interM, "time")) ieM = 2;
  else if (streq (interM, "max"))  ieM = 3;
  else if (streq (interM, "old"))  ieM = 0;

  rc = glatComputeConsensus (&latA, (double) cutoff, silWord,
			     (double) beam, (double) scale, (double) silScale,
			     iaM, ieM, verbose, sl);

  free (latA.mapA);
  free (latA.itemA);

  return rc;
}
