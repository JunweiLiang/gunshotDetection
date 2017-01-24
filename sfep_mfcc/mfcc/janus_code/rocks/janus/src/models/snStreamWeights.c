/* ========================================================================
    JanusRTk   Janus Recognition Toolkit
               ------------------------------------------------------------
               Object: Senone Stream Weights
               ------------------------------------------------------------

    Author  :  Florian Metze
    Module  :  snStreamWeights.c
    Date    :  $Id: snStreamWeights.c 2762 2007-02-22 16:28:00Z fuegen $

    Remarks :  Code for discriminative training of Stream Weights (AFs)

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
    Revision 4.4  2007/02/22 16:28:00  fuegen
    removed -pedantic compiler warnings with

    Revision 4.3  2006/02/21 15:04:17  fuegen
    changes for Windows

    Revision 4.2  2005/12/20 15:14:49  metze
    Code for AF training (snStreamWeights.c|h)

    Revision 4.1  2005/04/26 14:41:19  metze
    SenoneSet Stream Training

   ======================================================================== */


#include "snStreamWeights.h"


/* ------------------------------------------------------------------------
     DMC/MMI-based functions
   ---------------------------
     Similar to DMC and other discriminative approaches
   ------------------------------------------------------------------------ */


/* ------------------------------------------------------------------------ */
/*  snsWeightsAccu                                                          */
/* ------------------------------------------------------------------------ */

/*
  return 1 if senones have the same basephones 
*/
static int cmpSenoneName (char* name1, char* name2) 
{
  char *base1, *base2;
  int len1,len2;

  assert(name1 != NULL);
  assert(name2 != NULL);
  if (0 == strcmp(name1,name2)) return 1;
      
  base1= NULL;
  base1= strchr(name1,'(');
  if (!base1) base1= strchr(name1,'-');
  if (base1)
    len1 = base1-name1;
  else
    len1 = strlen(name1);

  base2= NULL;
  base2= strchr(name2,'(');
  if (!base2) base2= strchr(name2,'-');
  if (base2)
    len2 = base2-name2;
  else
    len2 = strlen(name2);

  if (len1 != len2) return 0;
  if (0 == strncmp(name1,name2,len1)) return 1;

  return 0;

}

/*
  map senone name to index of underlying basephone
*/
static int senone2phoneX (char* name_org, Phones* ps)
{
  char* name = strdup (name_org);
  int    idx;

  if (strchr (name, '('))
    *(strchr (name, '(')) = 0;
  if (strchr (name, '-'))
    *(strchr (name, '-')) = 0;
  idx = phonesIndex (ps, name);
  if (name)
    free(name);
  return idx;
}

extern int snsWeightsAccu (SenoneSet* snsP, Path* pathP, int idx,
			   int fromX, int toX, int v, int zm)
{
  StreamArray* staP = &(snsP->strA);
  int        frameX;

  if (toX==-1) toX = pathP->lastFrame;
  if (fromX < pathP->firstFrame || toX > pathP->lastFrame) {
    ERROR("snsWeightsAccu: inconsistent start or end point in '%s'\n",
	  pathP->name);
    return TCL_ERROR;
  }

  /* Set to MMI */
  snsP->streamWghtAccuMode = 2;
  if (idx < 0 || idx > 1) {
    ERROR ("snsWeightsAccu: idx=%d not supported\n", idx);
    return TCL_ERROR;
  }

  for (frameX = fromX; frameX <= toX; frameX++) {
    float*  sP = snsP->streamCache->matPA[frameX];
    int  itemX;

    /* Check if cache is useable */
    if (snsP->streamCache->m == 1 || frameX >= snsP->streamCache->m) {
      int dsN = staP->cBaseA[staP->streamN] - staP->cBaseA[1];
           sP = snsP->streamCache->matPA[0];
      if (snsP->streamCacheX != frameX)
	for (itemX = 0; itemX < dsN; itemX++)
	  sP[itemX] = -FLT_MAX;
      snsP->streamCacheX = frameX;
    }

    for (itemX = 0; itemX < pathP->pitemListA[frameX].itemN; itemX++) {
      PathItem*    itemP = pathP->pitemListA[frameX].itemA+itemX;
      Senone*        snP = snsP->list.itemA+itemP->senoneX;
      int        streamN = snP->streamN;
      double*      accuP = snP->accu+idx*streamN;
      float        gamma = itemP->gamma;
      int        streamX;
      
      if (!snP->accu) {
	snP->dccu = NULL;
	if (!(snP->accu = (double*) realloc (snP->accu, 2*streamN*sizeof(double)))) {
	  ERROR("snsWeightsAccu: allocation failure\n");
	  return TCL_ERROR;
	}
	snP->dccu = snP->accu + streamN;
	for (streamX = 0; streamX < snsP->streamWghtAccuMode*streamN; streamX++)
	  snP->accu[streamX] = 0.0;
	accuP = snP->accu+idx*streamN;
      }

      /* update training counts */
      snP->cnt += gamma;

      if (v > 0) INFO ("frameX= %d itemX= %d sn= %s gamma= %g\n",
		       frameX, itemX, snP->name, gamma);

      /* score and accu main stream */
      if (snP->stwA[0] > 0) {
	float flt;
	staP->strPA[0]->scoreFct (staP->cdA[0], snP->classXA+0, &flt, 1, frameX);
	accuP[0] += gamma * flt;
      }

      /* compute scores and accu all remaining streams */
      for (streamX = 1; streamX < streamN; streamX++) {
	int dsX = snP->classXA[streamX];
	int cX  = staP->cBaseA[streamX] - staP->cBaseA[1] + dsX;
	
	/* nothing to do */
	if (dsX < 0 || snP->stwA[streamX] <= 0 || (zm && snsP->strA.stwA[streamX] == 0.0))
	  continue;

	if (sP[cX] == -FLT_MAX)
	  staP->strPA[streamX]->scoreFct (staP->cdA[streamX], &dsX, sP+cX, 1, frameX);
	accuP[streamX] += gamma * sP[cX];
      }
    }
  }

  return TCL_OK;
}


/* ------------------------------------------------------------------------
     snsWeightsUpdate
   ------------------------------------------------------------------------ */

/*
  Do an mmi-like discriminative update for training of stream weights
 */
static int discrWeightUpdate (float* newA, float* orgA, double* numA, double* denA,
			      int startX, int streamN, float mass)
{
  double*  kA = malloc (streamN*sizeof(double));

  int       m, p;
  double    k, D, e;

  /* compute the constant */
  for (m = startX, k = 0; m < streamN; m++)
    if (orgA[m] > 0 && denA[m]/orgA[m] > k)
      k = denA[m]/orgA[m];
  for (m = startX; m < streamN; m++)
    if (orgA[m] > 0) 
      kA[m] = k - denA[m]/orgA[m];
  for (m = startX, D = 0; m < streamN; m++)
    D += numA[m];

  memmove (newA, orgA, streamN*sizeof(float));

  /* loop, exit if nothing changes */
  for (p = 0, e = 1; p < 100 && e > 1e-6; p++) {
    double d = D;

    /* compute denominator, renormalize */
    for (m = startX; m < streamN; m++)
      d += kA[m] * newA[m];
    d /= mass;

    for (m = startX, e = 0.0; m < streamN; m++) {
      double t  = (numA[m] + kA[m] * newA[m]) / d;
             e += fabs (t-newA[m]);
       newA[m]  = t;
    }
  }

  /* INFO ("discrWeightUpdate: k= %g D= %g e= %1.1g\n", k, D, e); */

  free (kA);
  return TCL_OK;
}


/*
  Collect statistics for Stream Weights update
  in nom/ den array
*/
static int snsCollectStats (SenoneSet* snsP, int snX,
		     double* numC, double* denC, double* numA, double* denA) {
  Senone*            snP = snsP->list.itemA;
  int                streamX;

  /* Checks */
  if (snX > snsP->list.itemN)
    return TCL_ERROR;

  /* numerator: get absolute sum of changes */
  for (streamX = 0; streamX < snP[snX].streamN; streamX++)
    *numC         += fabs (snP[snX].accu[streamX]);
  /*accuMassNum   += fabs (snP[snX].accu[streamX]*snP->stwA[streamX]); */

  /* numerator: normalize changes to 'movable' probability mass */
  for (streamX = 0; streamX < snP[snX].streamN; streamX++)
    numA[streamX] +=       snP[snX].accu[streamX];
  /*numA[streamX] +=       snP[snX].accu[streamX]*snP->stwA[streamX]; */

  /* denominator: get absolute sum of changes */
  for (streamX = 0; streamX < snP[snX].streamN; streamX++)
    *denC         += fabs (snP[snX].accu[snP[snX].streamN+streamX]);
  /*accuMassDen   += fabs (snP[snX].accu[streamN+streamX]*snP->stwA[streamX]); */

  /* denominator: normalize changes to 'movable' probability mass */
  for (streamX = 0; streamX < snP[snX].streamN; streamX++)
    denA[streamX] +=       snP[snX].accu[snP[snX].streamN+streamX];
  /*denA[streamX] +=       snP[snX].accu[streamN+streamX]*snP->stwA[streamX]; */

  return TCL_OK;
}

/*
  Tying of accus
*/
static int snsTreeTie (SenoneSet* snsP, Tree* treeP, int streamY, int streamN,
		       int nodeX, double* wA, float c)
{
  TreeNode* nodeP = treeP->list.itemA + nodeX;
  int             i;

  /* Leaf (it's possible we're ignoring some nodeP->model!) */
  if (nodeP->yes == -1 && nodeP->no == -1) {
    Senone* snP;
    
    for (i = snsP->list.itemN-1; i >= 0 && 
	   snsP->list.itemA[i].classXA[streamY] != nodeP->model; i--);
    
    /* No senone? */
    if (i == -1)
      return TCL_OK;

    /* Copy the senone weight accu */
    snP = snsP->list.itemA + i;
    snP->cnt = c;
    if (!snP->accu) return TCL_OK;
    memcpy (snP->accu, wA, 2*streamN*sizeof(double));
    /*printf ("Tying %s n_%d\n", nodeP->name, nodeX); */

    return TCL_OK;
  }

  /* Recursion */
  if (nodeP->yes >= 0) 
    snsTreeTie (snsP, treeP, streamY, streamN, nodeP->yes, wA, c);
  if (nodeP->no  >= 0)
    snsTreeTie (snsP, treeP, streamY, streamN, nodeP->no,  wA, c);

  return TCL_OK;
}

/*
  Cluster the stream weight accus using a tree and minCounts
  Recursive
*/
static float snsTreeCluster (SenoneSet* snsP, Tree* treeP, int minCount,
			     int streamY, int streamN, int nodeX, double* wA)
{
  TreeNode* nodeP = treeP->list.itemA + nodeX;
  float   cy, cn, c;
  double* wAy, *wAn;
  int             i;

  /* Leaf */
  if (nodeP->yes == -1 && nodeP->no == -1) {
    Senone* snP;
    
    for (i = snsP->list.itemN-1; i >= 0 && 
	   snsP->list.itemA[i].classXA[streamY] != nodeP->model; i--);

    /* No senone? */
    if (i == -1) {
      int j;
      INFO ("snsTreeCluster: faking %s %d\n", nodeP->name, nodeX);
      for (j = 0; j < 2*streamN; j++)
	wA[j] = 0;
      return 0;
    }

    /* Copy the senone weight accu */
    snP = snsP->list.itemA + i;
    if (!snP->accu) return 0;
    memcpy (wA, snP->accu, 2*streamN*sizeof(double));
    /*printf ("Model %s n_%d %g\n", nodeP->name, nodeX, snP->cnt); */

    return snP->cnt;
  }

  wAy = malloc (2*streamN*sizeof(double));
  wAn = malloc (2*streamN*sizeof(double));
  cy = cn = 0;

  if (nodeP->yes >= 0) 
    cy = snsTreeCluster (snsP, treeP, minCount, streamY, streamN, 
			 nodeP->yes, wAy);
  if (nodeP->no  >= 0)
    cn = snsTreeCluster (snsP, treeP, minCount, streamY, streamN, 
			 nodeP->no,  wAn);

  /* Tying */
  c = cy + cn;
  for (i = 0; i < 2*streamN; i++)
    wA[i] = wAy[i] + wAn[i];

  /* Do we have enough data? */
  free (wAy), free (wAn);
  if (c < minCount) {
    /*printf ("Not enough %g at %s n_%d\n", c, nodeP->name, nodeX); */
    return c;
  }

  /* if ((nodeP->yes >= 0 && cn < minCount) || (nodeP->no >= 0 && cy < minCount)) 
    printf ("Creating cluster (%g %g %g) to %d %d at n_%d %s\n", 
    c, cy, cn, nodeP->yes, nodeP->no, nodeX, nodeP->name); */

  /* Ok, there's some tying to do */
  if (cy < minCount && nodeP->yes >= 0)
    snsTreeTie (snsP, treeP, streamY, streamN, nodeP->yes, wA, c);
  if (cn < minCount && nodeP->no  >= 0)
    snsTreeTie (snsP, treeP, streamY, streamN, nodeP->no,  wA, c);

  return c;
}

/*
  Do the actual stream weight update
*/
extern int snsWeightsUpdate (SenoneSet* snsP, char* mode, char* range_mode, double minw,
			     double mass, double contrast, int startX, float minCount, float* q, int v) 
{
  int                snN = snsP->list.itemN;
  Senone*            snP = snsP->list.itemA;
  int            streamN = 0;
  float*         streamA = NULL;
  double*           numA = NULL;
  double*           denA = NULL;
  int           modelCnt = 0;
  double    totalNumMass = 0;
  double    totalDenMass = 0;
  int       snX, streamX;

  /* Checks */
  *q = 0;
  for (snX = 0; snX < snN; snX++)
    if (snP[snX].streamN > streamN) 
      streamN = snP[snX].streamN;
  streamA = malloc (streamN*sizeof (float));
  numA    = malloc (streamN*sizeof(double));
  denA    = malloc (streamN*sizeof(double));
  if (!streamA || !numA || !denA) {
    ERROR("snsWeightsUpdate: allocation failure.\n");
    return TCL_ERROR;
  }


  /* -------------------------------------------------------------- */
  if        (streq (range_mode, "global") || streq (range_mode, "Global")) {
    StreamArray*      staP = &(snsP->strA);
    
    for (streamX = 0; streamX < streamN; streamX++)
      numA[streamX] = 0.0, denA[streamX] = 0.0;

    /* accumulate stream weights (standard part) */
    for (snX = 0; snX < snN; snX++) {
      double accuMassNum = 0.0;
      double accuMassDen = 0.0;

      if (streamN != snP[snX].streamN) {
	ERROR ("snsWeightsUpdate: inconsistent nr. of streams.\n");
	free (numA), free (denA), free (streamA);
	return TCL_ERROR;
      } else if (!snP[snX].stwA) {
	ERROR ("snsWeightsUpdate: '%s' has no weights.\n", snP[snX].name);
	free (numA), free (denA), free (streamA);
	return TCL_ERROR;
      } else if (snP[snX].cnt < 1e-6 || !snP[snX].accu)
	continue;

      /* if (streq (snP[snX].name, "SIL(|)-m")) {
	INFO ("Ignoring %s\n", snP[snX].name);
	continue;
      } */

      snsCollectStats (snsP, snX, &accuMassNum, &accuMassDen, numA, denA);
      
      modelCnt++;
      totalNumMass += accuMassNum;
      totalDenMass += accuMassDen;
      if (v > 1) INFO ("%s num= %g den= %g cnt= %g\n", snP[snX].name, accuMassNum, accuMassDen, snP[snX].cnt);
    }
    
    if (modelCnt < 1) {
      INFO ("snsWeightsUpdate: no counts, no update\n");
      free (numA), free (denA), free (streamA);
      return TCL_OK;
    }

    /* do update */
    if (streq (mode, "mmi")) {
      /* MMI-like update */
      if (v) INFO ("snsWeightsUpdate: mmi -> num= %g den= %g (%d)\n", totalNumMass, totalDenMass, modelCnt);
      /*
	Bad HACK:        num <-> den because I don't know the math well enough!
	Not-so-bad HACK: Loop around Update
      */
      for (streamX = 0; streamX < 100; streamX++) {
	discrWeightUpdate (streamA, staP->stwA, denA, numA, startX, streamN, mass);
	*q = totalNumMass-totalDenMass;
	
	/* write weights into global array */
	memcpy (staP->stwA, streamA, streamN*sizeof (float));
      }

    } else if (streq (mode, "dmc")) {
      double tmpct = 0;

      /* DMC-like update */
      if (v) INFO ("snsWeightsUpdate: dmc -> num= %g den= %g (%d)\n", totalNumMass, totalDenMass, modelCnt);
      for (streamX = startX; streamX < streamN; streamX++) {
	if (v > 1) INFO ("%d < %g %g > %g + %g\n", streamX, numA[streamX], denA[streamX], 
			 staP->stwA[streamX], mass * (numA[streamX] - denA[streamX]));
	streamA[streamX] = staP->stwA[streamX] -= mass * (numA[streamX] - denA[streamX]);

	/* To avoid negative weights */
	if (minw < 1 && staP->stwA[streamX] < fabs(minw))
	    streamA[streamX] = staP->stwA[streamX] = fabs(minw);

	tmpct += numA[streamX] - denA[streamX];
      }
      if (v) INFO ("Total Change: %g %g\n", tmpct, tmpct/modelCnt);

    } else {
      ERROR ("snsWeightsUpdate: unknown mode '%s'\n", mode);
      free (numA), free (denA), free (streamA);
      return TCL_ERROR;
    }

    if (contrast > 0) {
      double m = 0;
      double w = 1;
      double v;

      if (minw < 0) { /* auto gain control */
	for (streamX = 1; streamX < streamN; streamX++)
	  m += streamA[streamX], w = (streamA[streamX] < w) ? streamA[streamX] : w;
	m /= (streamN-1);
	if ((minw-w)/(w-m) > 0 && (minw-w)/(w-m) < contrast) {
	  contrast = (minw-w)/(w-m);
	  INFO ("Contrast set to %g\n", contrast);
	}
	for (streamX = 1; streamX < streamN; streamX++)
	  staP->stwA[streamX] = streamA[streamX] += contrast * (streamA[streamX]-m);

      } else if (minw < 1) { /* set minimum */
	for (streamX = 1; streamX < streamN; streamX++)
	  m += streamA[streamX];
	m /= (streamN-1);
	for (streamX = 1; streamX < streamN; streamX++)
	  if ((v = streamA[streamX] + contrast * (streamA[streamX]-m)) > minw)
	    streamA[streamX] = staP->stwA[streamX] = v;
	  else
	    streamA[streamX] = staP->stwA[streamX] = minw;

      } else { /* We don't care about negative weights */
	for (streamX = 1; streamX < streamN; streamX++)
	  m += streamA[streamX];
	m /= (streamN-1);
	for (streamX = 1; streamX < streamN; streamX++)
	  streamA[streamX] = staP->stwA[streamX] += contrast * (streamA[streamX]-m);
      }

      /*
      for (streamX = 1; streamX < streamN; streamX++)
	if (streamA[streamX] > 0)
	  m += streamA[streamX], M++;
      if (M) m /= M;
      for (streamX = 1; streamX < streamN; streamX++)
	if (streamA[streamX] > 0)
	  if ((v = staP->stwA[streamX] + contrast * (streamA[streamX]-m)) > 0)
	    streamA[streamX] = staP->stwA[streamX] = v;
	  else
	    streamA[streamX] = staP->stwA[streamX] = 1e-4;
      */

    }
      
    /* write weights into local arrays */
    if (streq (range_mode, "Global"))
      INFO ("Not writing into local arrays.\n");
    else for (snX = 0; snX < snN; snX++)
      memcpy (snP[snX].stwA, streamA, streamN*sizeof (float));

    /* Hack alert */
    if (minCount < 1) {
      INFO ("CD-CI HACK (%g)\n", minCount);
      for (snX = 0; snX < snN; snX++) {
	for (streamX = 0; streamX < streamN; streamX++)
	  snP[snX].stwA[streamX] -= minCount * ( snP[snX].accu[streamX] - snP[snX].accu[streamX+streamN] );
      }
    }


    /* -------------------------------------------------------------- */
  } else if (streq (range_mode, "phone")) {
    DistribStream*    dstP = (DistribStream*)snsP->strA.cdA[0];
    Phones*        phonesP = dstP->treeP->ptreeSet.phones;
    Phone*             phP = phonesP->list.itemA;
    int             phoneN = phonesP->list.itemN;
    int phoneX;
    
    for (phoneX = 0; phoneX < phoneN; phoneX++) {
      int    streamN = 0;
      float* streamA = NULL;    
      int   modelCnt = 0.0;
      float accuMass = 0.0;
      float      sum = 0.0;
      
      /* accumulate stream weights (standard part) */
      for (snX = 0; snX < snN; snX++) {
	if (0 == cmpSenoneName(phP[phoneX].name,snP[snX].name))
	  continue;

	if (streamN == 0) {
	  streamN = snP[snX].streamN;
	  if (NULL == (streamA = malloc(streamN*sizeof(float)))) {
	    ERROR("snsWeightsUpdate: allocation failure.\n");
	    return TCL_ERROR;
	  }
	  for (streamX=0;streamX<streamN;streamX++) streamA[streamX] = 0.0;
	} else {
	  if (streamN != snP[snX].streamN) {
	    ERROR("snsWeightsUpdate: inconsistent nr. of streams.\n");
	    return TCL_ERROR;
	  }
	}
	if (! snP->stwA) {
	  ERROR("snsWeightsUpdate: '%s' has no weights.\n",snP[snX].name);
	  return TCL_ERROR;
	}
	
	if ( snP[snX].cnt < 1e-6 || ! snP[snX].accu) continue;

	modelCnt++;

	/* get absolute sum of changes */
	accuMass = 0.0;
	for (streamX=0;streamX<streamN;streamX++) accuMass += fabs(snP[snX].accu[streamX]);

	/* normalize changes to 'movable' probability mass */
	for (streamX=0;streamX<streamN;streamX++)
	  streamA[streamX] = snP[snX].stwA[streamX] + mass * snP[snX].accu[streamX]/accuMass;

      }
      
      if (modelCnt < 1) continue;
      
      /* normalize stream weight accus by the occurance statistics */
      for (streamX=0;streamX<streamN;streamX++) 
	streamA[streamX] /= modelCnt;

      /* sum of stream weights should be 1 */
      for (streamX=0;streamX<streamN;streamX++) 
	sum += streamA[streamX];
      if (sum != 0.0) {
	for (streamX=0;streamX<streamN;streamX++) 
	  streamA[streamX] /= sum;
      }
      
      /*write weights into local arrays */
      for (snX=0;snX<snN;snX++) {
	if (1 == cmpSenoneName(phP[phoneX].name,snP[snX].name)) {
	  memcpy(snP[snX].stwA,streamA,streamN*sizeof(float));
	}
      }
      if (streamA) free(streamA);
    }


    /* -------------------------------------------------------------- */
  } else if (streq (range_mode, "state")) {
    ERROR("snsWeightsUpdate: mode not supported yet\n");
    return TCL_OK;


    /* -------------------------------------------------------------- */
  } else if (streq (range_mode, "tree")) {
    int         streamY = 0;
    DistribStream* dsmP = (DistribStream*) snsP->strA.cdA[streamY];
    Tree*         treeP = dsmP->treeP;
    int            tagX;
    TreeNode       *tn1;
    double*          wA = malloc (2*streamN*sizeof(double));

    /* sanity check (may not be necessary) */
    if (!itfGetObject (dsmP->name, "DistribStream")) {
      ERROR ("snsWeightsUpdate: '%s' not a DistribStream (%d)\n", dsmP->name, streamY);
      return TCL_ERROR;
    }

    for (streamX = 0; streamX < streamN; streamX++)
      numA[streamX] = denA[streamX] = 0.0;

    INFO ("Tree is %s, streamY is %d\n", treeP->name, streamY);
    for (tagX = 0; tagX < snsP->tagList.itemN; tagX++) {
      SenoneTag* sntP = (SenoneTag*) snsP->tagList.itemA+tagX;
      float         c;
                  tn1 =  (TreeNode*) treeP->list.itemA+sntP->tagA[streamY];

      if (tn1->yes != tn1->no || tn1->yes != tn1->undef)
	WARN ("snsWeightsUpdate: root-node has different yes/no/undef successors\n");

      c = snsTreeCluster (snsP, treeP, minCount, streamY, streamN, tn1->yes, wA);

      INFO ("snsWeightsUpdate: tagX = %d -> %s %d : %g\n", 
	    tagX, tn1->name, sntP->tagA[streamY], c);
    }
    
    free (wA);

    /* accumulate stream weights (standard part) */
    for (snX = 0; snX < snN; snX++) {
      double accuMassNum = 0.0;
      double accuMassDen = 0.0;

      if (!snP[snX].stwA || snP[snX].cnt < minCount || !snP[snX].accu)
	continue;

      for (streamX = 0; streamX < snP[snX].streamN; streamX++)
	numA[streamX] = 0.0, denA[streamX] = 0.0;

      snsCollectStats (snsP, snX, &accuMassNum, &accuMassDen, numA, denA);

      totalNumMass += accuMassNum;
      totalDenMass += accuMassDen;
      if (v > 1) INFO ("%s num= %g den= %g cnt= %g\n",
		       snP[snX].name, accuMassNum, accuMassDen, snP[snX].cnt);

      /* do update */
      if (streq (mode, "mmi")) {
	/* MMI-like update */
	if (v) INFO ("snsWeightsUpdate: mmi -> num= %g den= %g (%d)\n",
		     totalNumMass, totalDenMass, modelCnt);
	/*
	  Bad HACK:        num <-> den because I don't know the math well enough!
	  Not-so-bad HACK: Loop around Update
	*/
	for (streamX = 0; streamX < 100; streamX++) {
	  discrWeightUpdate (streamA, snP[snX].stwA, denA, numA, startX, streamN, mass);
	  *q = totalNumMass-totalDenMass;
	  memcpy (snP[snX].stwA, streamA, streamN*sizeof (float));
	}
	
      } else if (streq (mode, "dmc")) {
	/* DMC-like update */
	if (v) INFO ("snsWeightsUpdate: %s dmc -> num= %g den= %g (%g)\n",
		     snP[snX].name, accuMassNum, accuMassDen, snP[snX].cnt);
	for (streamX = startX; streamX < streamN; streamX++) {
	  snP[snX].stwA[streamX] -= mass * (numA[streamX] - denA[streamX]);
	  if (snP[snX].stwA[streamX] < 0) snP[snX].stwA[streamX] = 1e-6;
	}	

      } else {
	ERROR ("snsWeightsUpdate: unknown mode '%s'\n", mode);
	free (numA), free (denA), free (streamA);
	return TCL_ERROR;
      }

      *q += totalNumMass-totalDenMass;
    }


    /* -------------------------------------------------------------- */
  } else if (streq (range_mode, "senone")) {

    /* accumulate stream weights (standard part) */
    for (snX = 0; snX < snN; snX++) {
      double accuMassNum = 0.0;
      double accuMassDen = 0.0;

      if (!snP[snX].stwA || snP[snX].cnt < minCount || !snP[snX].accu)
	continue;

      for (streamX = 0; streamX < snP[snX].streamN; streamX++)
	numA[streamX] = denA[streamX] = 0.0;

      snsCollectStats (snsP, snX, &accuMassNum, &accuMassDen, numA, denA);

      totalNumMass += accuMassNum;
      totalDenMass += accuMassDen;
      if (v > 1) INFO ("%s num= %g den= %g cnt= %g\n",
		       snP[snX].name, accuMassNum, accuMassDen, snP[snX].cnt);

      /* do update */
      if (streq (mode, "mmi")) {
	/* MMI-like update */
	if (v) INFO ("snsWeightsUpdate: mmi -> num= %g den= %g (%d)\n",
		     totalNumMass, totalDenMass, modelCnt);
	/*
	  Bad HACK:        num <-> den because I don't know the math well enough!
	  Not-so-bad HACK: Loop around Update
	*/
	for (streamX = 0; streamX < 100; streamX++) {
	  discrWeightUpdate (streamA, snP[snX].stwA, denA, numA, startX, streamN, mass);
	  *q = totalNumMass-totalDenMass;
	  memcpy (snP[snX].stwA, streamA, streamN*sizeof (float));
	}
	
      } else if (streq (mode, "dmc")) {
	/* DMC-like update */
	if (v) INFO ("snsWeightsUpdate: %s dmc -> num= %g den= %g (%g)\n",
		     snP[snX].name, accuMassNum, accuMassDen, snP[snX].cnt);
	for (streamX = startX; streamX < streamN; streamX++) {
	  snP[snX].stwA[streamX] -= mass * (numA[streamX] - denA[streamX]);
	  if (snP[snX].stwA[streamX] < 0) snP[snX].stwA[streamX] = 1e-6;
	}	

      } else {
	ERROR ("snsWeightsUpdate: unknown mode '%s'\n", mode);
	free (numA), free (denA), free (streamA);
	return TCL_ERROR;
      }

      *q += totalNumMass-totalDenMass;
    }


  } else {
    /* -------------------------------------------------------------- */
    ERROR ("snsWeightsUpdate: mode '%s' not supported yet\n", range_mode);
    free (numA), free (denA), free (streamA);
    return TCL_ERROR;
  }

  free (numA), free (denA), free (streamA);
  return TCL_OK;
}


/* ------------------------------------------------------------------------
     MLE-based functions
   -----------------------
     This can not and doesn't work
   ------------------------------------------------------------------------ */


/* ------------------------------------------------------------------------ */
/*  snsWeightsAccuMLE                                                       */
/* ------------------------------------------------------------------------ */

extern int snsWeightsAccuMLE (SenoneSet* snsP, Path* pathP, int v, int zm)
{
  int fromX   = pathP->firstFrame;
  int toX     = pathP->lastFrame;
  int frameX,streamX;

  /* Set to MLE */
  snsP->streamWghtAccuMode = 1;

  for (frameX=fromX;frameX<toX;frameX++) {
    PathItem* itemP = pathP->pitemListA[frameX].itemA;
    int       itemN = pathP->pitemListA[frameX].itemN-1;

    for (;itemN>=0;itemN--,itemP++) {
      Senone*     snP = snsP->list.itemA+itemP->senoneX;
      int     streamN = snP->streamN;

      if (!snP->accu) {
	snP->dccu = NULL;
	if (!(snP->accu = (double*) realloc (snP->accu, streamN * sizeof(double)))) {
	  ERROR("snsWeightsAccuMLE: allocation failure\n");
	  return TCL_ERROR;
	}
	for (streamX=0; streamX < streamN; streamX++)
	  snP->accu[streamX]=0.0;
      }
      snP->cnt += itemP->gamma;
      for (streamX=0; streamX<streamN; streamX++) {
	int     idx = snP->classXA[streamX];
	float score = 0.0;
	if (!zm && !snsP->strA.stwA[streamX])
	  continue;
	snsP->strA.strPA[streamX]->scoreFct( snsP->strA.cdA[streamX], &idx, &score, 1, frameX);
	snP->accu[streamX] -= itemP->gamma*score;
      }
    }
  }
  return TCL_OK;
}


/* ------------------------------------------------------------------------ */
/*  snsWeightsUpdateMLE                                                       */
/* ------------------------------------------------------------------------ */

extern int snsWeightsUpdateMLE (SenoneSet* snsP, float minCnt, float M, float K,
				int mode, int zm, int nm, int sIdx)
{
  int             snN = snsP->list.itemN;
  double           m1 = (M != 1) ? M/(M-1) : 0.0;
  double           m2 = (M != 0) ? 1.0/M   : 0.0;
  int         streamN = snsP->strA.streamN;
  DistribStream* dstP = (DistribStream*)snsP->strA.cdA[0];
  Phones*     phonesP = dstP->treeP->ptreeSet.phones;
  int snX, streamX;
  double min = 0.0;

  if (snsP->streamWghtAccuMode != 1)
    INFO ("snsWeightsUpdateMLE: this doesn't look like an MLE accu, continuing anyway\n");

  if (m1 == 0 || m2 == 0) {
    ERROR("snsWeightsUpdateMLE: Invalid %d-norm (M > 1) \n",M);
    return TCL_ERROR;
  }

  /* -------------------- */
  /*   zero noise accus   */
  /* -------------------- */
  if (nm) for (snX = 0; snX < snN; snX++) {
    Senone*   snP = snsP->list.itemA+snX;
    Phone* phoneP = phonesP->list.itemA+senone2phoneX (snP->name, phonesP);
    if (!streq (phoneP->name, "SIL") && phoneP->name[0] != '+')
      continue;
    for (streamX = 0; streamX < streamN; streamX++)
      if (snP->accu) snP->accu[streamX] = 0.0;
    snP->cnt = 0.0;
  }


  /* ------------------------------ */
  /* Accumulator Smoothing : global */
  /* ------------------------------ */

  if (mode == SMOOTH_GLOBAL) {
    double* accu = NULL;
    if (NULL == (accu = malloc((streamN+1)*sizeof(double)))) {
      ERROR("snsWeightsUpdateMLE: allocation failure\n");
      return TCL_ERROR;
    }
    for (streamX=0;streamX<=streamN;streamX++) 
      accu[streamX]=0.0;
    for (snX = 0; snX < snN; snX++) {
      Senone* snP = snsP->list.itemA+snX;
      if (snP->streamN != streamN) {
	ERROR("snsWeightsUpdateMLE: inconsistent nr. of streams\n");
	return TCL_ERROR;
      }
      if (! snP->accu) continue;
      accu[streamN] += snP->cnt;
      for (streamX=0;streamX<streamN;streamX++)
	accu[streamX] += snP->accu[streamX];
    }
    for (snX = 0;snX < snN; snX++) {
      Senone* snP = snsP->list.itemA+snX;
      if (! snP->accu) continue;
      snP->cnt = accu[streamN];
      for (streamX=0;streamX<streamN;streamX++)
	snP->accu[streamX] = accu[streamX];
    }
    if (accu) free(accu);
  }

  /* ------------------------------ */
  /* Accumulator Smoothing : phone  */
  /* ------------------------------ */

  if (mode == SMOOTH_PHONE) {
    int             phoneN = phonesP->list.itemN;
    double**          accu = NULL;
    int phoneX;

    if (NULL == (accu = malloc(phoneN*sizeof(double*)))) {
      ERROR("snsWeightsUpdateMLE: allocation failure\n");
      return TCL_ERROR;
    }
    if (NULL == (accu[0] = malloc(phoneN*(streamN+1)*sizeof(double)))) {
      ERROR("snsWeightsUpdateMLE: allocation failure\n");
      return TCL_ERROR;
    }
    for (phoneX=0;phoneX<phoneN;phoneX++) {
      if (phoneX >0) accu[phoneX]=accu[phoneX-1]+streamN;
      for (streamX=0;streamX<=streamN;streamX++) 
	accu[phoneX][streamX]=0.0;
    }

    for (snX = 0; snX < snN; snX++) {
      Senone*    snP = snsP->list.itemA+snX;
      int     phoneX = senone2phoneX(snP->name,phonesP);

      if (snP->streamN != streamN) {
	ERROR("snsWeightsUpdateMLE: inconsistent nr. of streams\n");
	return TCL_ERROR;
      }
      if (! snP->accu) continue;
      accu[phoneX][streamN] += snP->cnt;
      for (streamX=0;streamX<streamN;streamX++)
	accu[phoneX][streamX] += snP->accu[streamX];
    }
    for (snX = 0; snX < snN; snX++) {
      Senone*    snP = snsP->list.itemA+snX;
      int     phoneX = senone2phoneX(snP->name,phonesP);
      if (! snP->accu) continue;
      snP->cnt = accu[phoneX][streamN];
      for (streamX=0;streamX<streamN;streamX++)
	snP->accu[streamX] = 	accu[phoneX][streamX];
    }
    if (accu[0]) free(accu[0]);
    if (accu)    free(accu);
  }

  if (mode == SMOOTH_STATE) {
    ERROR("snsWeightsUpdateMLE: SMOOTH_STATE not supported yet\n");
    return TCL_ERROR;
  }

  /* ------------------------------ */
  /* ML -update for each senone     */
  /* ------------------------------ */

  for (snX = 0; snX < snN; snX++) {
    Senone*    snP = snsP->list.itemA+snX;
    int    streamN = snP->streamN;
    float      sum = 0.0;

    if (! snP->accu) continue;

    if (snP->cnt < minCnt) {
     for (streamX=0;streamX<streamN;streamX++) 
       snP->accu[streamX] = 0.0;
     snP->cnt = 0.0; 
     continue; 
    }

    /* ensure positive scores */
    for (streamX=sIdx;streamX<streamN;streamX++) {
      if (snP->accu[streamX] < min)
        min = snP->accu[streamX];
    }
    min = (min < 0) ? 1.0 - min : 0.0;

    for (streamX=sIdx;streamX<streamN;streamX++) {
      if (!zm && !snsP->strA.stwA[streamX])
	continue;
      snP->accu[streamX] = pow(snP->accu[streamX]+min,m1);
      sum += snP->accu[streamX];
    }

    if (sum == 0) {
      for (streamX=0;streamX<streamN;streamX++) 
	snP->accu[streamX] = 0.0;
      snP->cnt = 0.0; 
      continue; 
    }

    for (streamX=sIdx;streamX<streamN;streamX++) {
      snP->stwA[streamX] = K*snP->accu[streamX]/sum;
      snP->stwA[streamX] = pow(snP->stwA[streamX],m2);
      if (!zm && !snsP->strA.stwA[streamX])
	snP->stwA[streamX] = 0.0;
      snP->accu[streamX] = 0.0;
    }
    snP->cnt = 0.0;

    /*re-normalize weight to 1.0 */
    sum = 0.0;
    for (streamX=0;streamX<streamN;streamX++) 
      sum += snP->stwA[streamX];
    for (streamX=sIdx;streamX<streamN;streamX++) 
      snP->stwA[streamX] /= sum;
  }

  return TCL_OK;
}


/* ------------------------------------------------------------------------
     Input/ Output functions
   ---------------------------
     Clearing included here as well.
   ------------------------------------------------------------------------ */


/* ------------------------------------------------------------------------ */
/*  snsWeightsLoad                                                          */
/* ------------------------------------------------------------------------ */

extern int snsWeightsLoad (SenoneSet *snsP, char* name)
{
  FILE* fp;
  char  str[1024];
  int snN, snX, streamN, streamX;
  float cnt;
  int snD = 0;

  if (!name) { ERROR ("NULL filename\n"); return TCL_ERROR; }
  if (! (fp = fileOpen (name,"r")))
    return TCL_ERROR;

  read_string (fp, str);
  if        (streq (str, "MLE-ACCU-V1")) {
    snsP->streamWghtAccuMode = 1;
    /* INFO ("snsWeightsLoad: found MLE accu\n"); */
  } else if (streq (str, "MMI-ACCU-V1")) {
    snsP->streamWghtAccuMode = 2;
    /* INFO ("snsWeightsLoad: found MMI accu\n"); */
  } else {
    ERROR ("Bad file type.\n");
    fileClose (name, fp);
    return TCL_ERROR;
  }
  snN     = read_int (fp);
  streamN = read_int (fp);
  
  if (streamN != snsP->strA.streamN) {
    ERROR ("Bad data %d %d (%d %d).\n", snN, snsP->list.itemN, streamN, snsP->strA.streamN);
    fileClose (name, fp);
    return TCL_ERROR;
  }

  for (snX = 0; snX < snN; snX++) {
    Senone* snP;
    int     snI;
    read_string (fp, str);
    cnt = read_float (fp);
    snI = snX+snD;

    /* If we 'guessed' snD right, we don't have to call snsIndex */
    if (snI < 0 || snI >= snsP->list.itemN || !streq (str, snsP->list.itemA[snI].name))
      snI = snsIndex (snsP, str);

    if (snI < 0) {
      WARN ("snsWeightsLoad: ignoring unknown accu '%s' (%g)\n", str, cnt);
      for (streamX = 0; streamX <  snsP->streamWghtAccuMode*streamN; streamX++)
	cnt = read_float (fp);
      continue;
    } else snP = snsP->list.itemA+snI;

    snD = snI-snX;

    if (!snP->accu) {
      snP->dccu = NULL;
      if (!(snP->accu = realloc (snP->accu, snsP->streamWghtAccuMode*streamN*sizeof(double)))) {
	ERROR ("snsWeightsLoadMLE: allocation failure.\n");
	fileClose (name, fp);
	return TCL_ERROR;
      }
      snP->dccu = (snsP->streamWghtAccuMode == 2) ? snP->accu + streamN : NULL;
      for (streamX = 0; streamX < snsP->streamWghtAccuMode*streamN; streamX++)
	snP->accu[streamX] = 0.0;
      snP->cnt = 0;
    }

    for (streamX = 0; streamX < snsP->streamWghtAccuMode*streamN; streamX++)
      snP->accu[streamX] += (double) read_float (fp);
    snP->cnt += cnt;
  }

  if (snX < snsP->list.itemN)
    INFO ("snsWeightsLoad: %d accus missing\n", snsP->list.itemN-snX);

  return fileClose (name, fp); 
}


/* ------------------------------------------------------------------------ */
/*  snsWeightsSave                                                          */
/* ------------------------------------------------------------------------ */

extern int snsWeightsSave (SenoneSet *snsP, char* name)
{
  FILE* fp;
  int   snX, streamX;

  if (!name) { 
    ERROR ("snsWeightsSave: NULL filename!\n");
    return TCL_ERROR; 
  } else if (! (fp = fileOpen (name,"w"))) {
    return TCL_ERROR;
  } else if (snsP->streamWghtAccuMode < 1 || snsP->streamWghtAccuMode > 2) {
    INFO ("snsWeightsSave: invalid accu mode, ignoring\n");
    return TCL_OK;
  }

  if (snsP->streamWghtAccuMode == 1) write_string (fp, "MLE-ACCU-V1");
  else                               write_string (fp, "MMI-ACCU-V1");
  write_int    (fp, snsP->list.itemN);
  write_int    (fp, snsP->strA.streamN);
  for (snX = 0; snX < snsP->list.itemN; snX++) {
    Senone* snP = snsP->list.itemA+snX;
    write_string (fp, snP->name);
    write_float  (fp, snP->cnt);
    for (streamX = 0; streamX < snsP->streamWghtAccuMode*snsP->strA.streamN; streamX++)
      write_float (fp, (float) (snP->accu ? snP->accu[streamX] : 0.0));
  }

  return fileClose (name, fp); 
}

/* ------------------------------------------------------------------------ */
/*  snsWeightsClear                                                         */
/* ------------------------------------------------------------------------ */

extern int snsWeightsClear (SenoneSet* snsP)
{
  int snX, streamX;

  for (snX = 0; snX < snsP->list.itemN; snX++) {
    Senone*   snP = snsP->list.itemA+snX;
    if (snP->accu)
      for (streamX = 0; streamX < snsP->streamWghtAccuMode*snsP->strA.streamN; streamX++)
	snP->accu[streamX] = 0.0;
    snP->cnt = 0.0;
  }

  return TCL_OK;
}
