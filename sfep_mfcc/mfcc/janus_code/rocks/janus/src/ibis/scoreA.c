/* ========================================================================
    JanusRTk   Janus Recognition Toolkit
               ------------------------------------------------------------
               Object: extended score routines
               ------------------------------------------------------------

    Author  :  Hagen Soltau
    Module  :  scoreA.c
    Date    :  $Id: scoreA.c 3403 2011-03-22 16:40:02Z metze $
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
    Revision 4.14  2007/04/03 12:34:19  fuegen
    added some asserts to ensure the fixed number of dimensions set by saDimN
    introduced to support the automatic vectorization of ICC

    Revision 4.13  2007/03/02 14:53:59  fuegen
    added SSE support (Florian Metze, Friedrich Faubel)
    made some modifications to the original SSE code for Linux compatibility
    added Gaussian Selection support also to the opt_semCont scoring function

    Revision 4.12  2007/02/22 16:24:24  fuegen
    minor changes in Gaussian Selection code by Thilo Koehler

    Revision 4.11  2007/02/21 16:21:52  fuegen
    some changes for the Gaussian Selection

    Revision 4.10  2007/02/21 12:32:05  fuegen
    added Gaussian Selection code from Thilo Koehler

    Revision 4.9  2005/10/12 13:35:43  metze
    Added ssa_base_str

    Revision 4.8  2005/09/23 12:34:51  fuegen
    free memory in opt_SemCont function

    Revision 4.7  2005/06/27 11:15:28  fuegen
    ssa_opt_semCont: made minDistIdxA static to
    reduce number of mallocs

    Revision 4.6  2005/04/25 13:24:00  metze
    ssa_opt now honours scoreScale

    Revision 4.5  2005/03/04 09:19:48  metze
    64bit & thread cleaned, warning for STCs, cleanup

    Revision 4.4  2004/09/10 15:38:44  metze
    Added base_semCont

    Revision 4.3  2004/08/27 13:22:10  metze
    Added opt_semCount

    Revision 4.2  2003/08/22 09:11:12  metze
    ssa_opt no longer segfaults when pattern==NULL

    Revision 4.1  2003/08/14 11:20:06  fuegen
    Merged changes on branch jtk-01-01-15-fms (jaguar -> ibis-013)

    Revision 1.1.2.28  2003/02/12 13:58:49  metze
    Added warnings for missing codebooks

    Revision 1.1.2.27  2002/11/20 13:40:49  soltau
    Replaced static _tmpI by snsP->refXA

    Revision 1.1.2.26  2002/11/20 09:40:39  soltau
    ssa_opt : static array _tmpI holds CBX_MAX elements

    Revision 1.1.2.25  2002/06/05 12:15:56  metze
    Leaner ICC code

    Revision 1.1.2.24  2002/06/04 17:17:30  metze
    Faster scores with INTEL_COMPILER

    Revision 1.1.2.23  2002/04/12 16:03:31  soltau
    Merged ssa_opt_str with ssa_opt_feat

    Revision 1.1.2.22  2002/04/10 13:26:56  soltau
    ssa_opt : don't steal the last codebook anymore

    Revision 1.1.2.21  2002/04/08 14:33:39  soltau
    beautified

    Revision 1.1.2.20  2002/04/03 11:10:44  soltau
    Added ssa_opt_thread

    Revision 1.1.2.19  2002/03/18 17:48:00  soltau
    Added bbi header

    Revision 1.1.2.18  2002/02/27 09:30:25  metze
    ssa_opt_feat uses smaller cache, uses streamCache->count

    Revision 1.1.2.17  2002/02/26 19:13:44  metze
    Extra info in streamCache->count

    Revision 1.1.2.16  2002/02/24 13:51:37  metze
    Increased snsP->refMax (is it a good idea to set it at all?)

    Revision 1.1.2.15  2002/02/01 14:17:20  fuegen
    moved stypes.h into slimits.h

    Revision 1.1.2.14  2002/01/30 09:28:02  metze
    Added opt_feat score function

    Revision 1.1.2.13  2002/01/18 13:36:28  metze
    Changed opt_str to work with senoneSet.streamCache

    Revision 1.1.2.12  2001/11/20 11:02:29  metze
    Enabled vectorization of score routine for ICC
    Use flags "-O3 -axK -tpp6 -wp_ipo -ipo_obj -restrict"

    Revision 1.1.2.11  2001/11/06 15:42:23  metze
    Check for streamWeights > 0

    Revision 1.1.2.10  2001/10/24 15:20:41  soltau
    Added ssa_opt_str

    Revision 1.1.2.9  2001/10/16 11:53:40  soltau
    Introduced CBX for bbi

    Revision 1.1.2.8  2001/09/19 16:51:17  metze
    Fixed oneForAll BBI problem

    Revision 1.1.2.7  2001/07/09 14:40:05  soltau
    Check compression mode in ssa_comp_all

    Revision 1.1.2.6  2001/07/06 13:12:00  soltau
    Changed compress and scoreFct handling

    Revision 1.1.2.5  2001/07/04 18:10:04  soltau
    Changes according to new compression interface
    Removed mmx support

    Revision 1.1.2.4  2001/07/03 09:46:03  soltau
    Added score functions for compressed codebooks

    Revision 1.1.2.3  2001/06/20 18:35:58  soltau
    *** empty log message ***

    Revision 1.1.2.2  2001/02/27 13:51:18  soltau
    *** empty log message ***

    Revision 1.1.2.1  2001/02/22 16:11:30  soltau
    Initial version


   ======================================================================== */

#include "scoreA.h"
#include "codebook.h"
#include "distrib.h"
#include "distribStream.h"
#include "bbi.h"
#include "slimits.h"

#ifndef DARWIN
#include <omp.h>
#endif

#ifdef MTHREAD
#include <pthread.h>
#include <semaphore.h>
#define  NUM_THREADS 2

#ifndef _REENTRANT
#define  _REENTRANT
#endif

#endif

char scoreARcsVersion[] = 
           "@(#)1$Id: scoreA.c 3403 2011-03-22 16:40:02Z metze $";

static int _WarnedNoCodeBooks = 0;

#ifdef __INTEL_COMPILER
#define saDimN 256
#endif

/* ------------------------------------------------------------------------
   ssa_one : compute score for one senone using the underlying scoreArray fct
   ------------------------------------------------------------------------ */

float ssa_one( SenoneSet* snsP, int senoneX, int frameX)
{
  int   idxA[1];
  float scoreA[1];
  idxA[0] = senoneX;
  snsP->scoreAFctP(snsP,idxA,scoreA-senoneX,1,frameX);
  return scoreA[0];
}

/* ------------------------------------------------------------------------
    ssa_base : base implementation for score array functions
   ------------------------------------------------------------------------ */

int ssa_base( SenoneSet* snsP, int* idxA, float* scoreA, 
	      int  idxN, int frameX)
{
  int idx;
  for ( idx   = 0; idx < idxN; idx++) {
    int snX = idxA[idx];
    scoreA[snX]=  snsScore(snsP,snX,frameX);
  }
  return TCL_OK;
}


/* ------------------------------------------------------------------------
    ssa_opt
   ---------
    optimized implementation for score array functions
   ------------------------------------------------------------------------ */

#ifdef SSE_OPT
int ssa_opt2 (SenoneSet* snsP, int* idxA, float* scoreA, 
	     int  idxN, int frameX)
#else
int ssa_opt  (SenoneSet* snsP, int* idxA, float* scoreA, 
	     int  idxN, int frameX)
#endif
{
  int        streamX  = 0;  
  StreamArray*  staP  = &(snsP->strA); 
  DistribSet*   dssP  = ((DistribStream*) staP->cdA[streamX])->dssP;
  CodebookSet*  cbsP  = dssP->cbsP;
  Senone*        snP  = snsP->list.itemA;
  Distrib*       dsP  = NULL;
  Codebook*      cbP  = cbsP->list.itemA + 0;
  float*     pattern  = NULL;
  BBITree*       bbi  = NULL;
  CBX*       bbilist  = NULL;
  CBX*         _tmpI  = snsP->refXA;
  GSClusterSet* gscsP = cbsP->gscsP;
  int         usegscs = 0;

  /*assume that all codebooks have the same dimN as the first one*/
  int           refN  = 0;
  int           dimN  = (cbP->cfg)->rdimN > 0 ? (cbP->cfg)->rdimN : cbP->dimN;

  int idx, snX, dsX, cbX, bbiN=0, adr=0, depthN=0, cbP_featX;
  float        offset = 0.0;
#ifdef SCOREMAGIC
  int frameY=-1;
#endif

#ifdef __INTEL_COMPILER
  float          sa[saDimN]; /* NOTE: This is hardcoded, because otherwise ICC would
				not VECTORIZE the loop which is very bad for speed! */
  int            minDistIdx, i, bestI;
  float          minDistSum;

  assert (dimN <= saDimN);
#else
  register int   minDistIdx, i, bestI;
  register float minDistSum;
#endif

  /* get pattern (same feature space for all codebooks) */
  idx = 0;
  snX = idxA[idx];
  dsX = snP[snX].classXA[streamX];
  dsP = dssP->list.itemA + dsX;
  cbX = dsP->cbX;
  cbP = cbsP->list.itemA + cbX;
  if (!(pattern = fesFrame (cbsP->feat, cbP_featX = cbP->featX, frameX))) {
    ERROR ("Can't get frame %d of feature %d\n", frameX, cbP->featX);
    for (idx = 0; idx < idxN; idx++)
      scoreA[idxA[idx]] = 0.0;
    return TCL_OK;
  }

  /* get bbi tree if available */ 
  if (cbsP->bbi.itemA && cbP->bbiX >=0) {
    bbi    = cbsP->bbi.itemA + cbP->bbiX;
    depthN = bbi->depthN; 
    i      = 1;     
    for (idx=0; idx<depthN; idx++) {
      BBINode*  actNode = (bbi->node)+i;
      i = (i<<1) + (pattern[(int) actNode->k] > actNode->h);            
    }
    adr = i-(1<<depthN);            
  } 

  /* Activate the Clusters if gsClusterSet is available */
  if (gscsP) {
    if (gscsP->use) {
      gsClusterSetActivate(gscsP,pattern, NULL, gscsP->bestN);
      usegscs = -1;
    }
  }

  if (cbsP->gdft_frame2class && cbsP->gdft_offset && cbsP->gdft_frame2class[frameX] >= 0)
    offset = cbsP->gdft_offset[cbsP->gdft_frame2class[frameX]];

  /* ------------------------------------------------------------------ */
  /* compute scores for all necessary distributions indexed in idxA     */
  /* ------------------------------------------------------------------ */

  for (idx = 0; idx < idxN; idx++) {

    /* find corresponding codebook from senone index idxA[i] */
    snX  = idxA[idx];    
    dsX  = snP[snX].classXA[streamX];
#ifdef SCOREMAGIC
    if (idxN > 1) {
      /* little hack to explore cache locality (FMe) - score cache magic:
	 we assume we might need this senone again in the next frame 
	 (this is definitely true for decoding, not so much during training) */
      if (frameX >= scI[dsX] && frameX < scI[dsX]+scN) {
	/* we have this value in cache, let's use it */
	scoreA[snX] = scA[frameX-scI[dsX] + scN*dsX];
	continue;
      }
      if (frameY < frameX) {
	/* let's start filling the cache and continue */
	scI[dsX] = -scN-1;
	frameY   = frameX+scN;
	/* not enough frames? */
	while (!(pattern = fesFrame (cbsP->feat, cbP_featX, frameY)))
	  frameY--;
	idx--;
      } else if (frameY > frameX) {
	/* we just computed a score, let's store this and do it again */
	scA[frameY-frameX-1 + scN*dsX] = scoreA[snX];
	pattern = fesFrame (cbsP->feat, cbP_featX, --frameY);
	idx--;
      } else {
	/* we're done storing cache lines, let's compute the score we need now */
	scI[dsX] = frameX+1;
	frameY   = -1;
	pattern  = fesFrame (cbsP->feat, cbP_featX, frameX);
      }
    }
#endif
    dsP  = dssP->list.itemA + dsX;
    cbX  = dsP->cbX;
    cbP  = cbsP->list.itemA + cbX;
    refN = cbP->refN;

    if (cbP->featX != cbP_featX) {
      ERROR ("ssa_opt: codebooks use different feature spaces => bogus results.\n");
      return TCL_OK;
    }

    if ((!cbP->rv || !cbP->cv) && !_WarnedNoCodeBooks) {
      ERROR ("ssa_opt: codebook '%s' not loaded => bogus results.\n", cbP->name);
      _WarnedNoCodeBooks = 1;
      continue;
    }

    /*bbi tree*/
    if (cbsP->bbi.itemA && cbP->bbiX >= 0 && cbP->bbiY >= 0) {
      BBILeaf *bucket = &bbi->leaf[adr][cbP->bbiY];
      bbiN     = bucket->cbvN;              
      bbilist  = bucket->cbX;               
    } else {
      bbiN     = refN;
      bbilist  = _tmpI;
    }

    if (gscsP) gscsP->gaussians_total += refN;

    /* ------------------------------------------------------------------*/
    /* Compute all the Mahalanobis distances for the subset of Gaussians */  
    /* ------------------------------------------------------------------*/

    minDistSum = 1E20;
    minDistIdx = 0;  
    bestI      = 0;

    for (i = 0; i < bbiN; i++) {     
      int refX = bbilist[i], dimX;
#ifdef __INTEL_COMPILER
      float distSum = cbP->pi + cbP->cv[refX]->det;
#else
      register float distSum = cbP->pi + cbP->cv[refX]->det;
#endif

      if (!usegscs || gscsP->cluster[gscsP->clusterX[cbX][refX]].active) {
#ifdef SSE_OPT
	__m128 *m128pt = (__m128*) pattern;
	__m128 *m128rv = (__m128*) cbP->rv->matPA[refX];
	__m128 *m128cv = (__m128*) cbP->cv[refX]->m.d;
	__m128  mo     = _mm_set_ps1(0.0);
	__m128  m1, m2;
	int     dimN_8 = dimN >> 3;
	float  *pt, *rv, *cv;
	#if defined __INTEL_COMPILER || defined WINDOWS
	__declspec(align(16)) float m_result[4];
	#else
	float   m_result[4] __attribute__ ((aligned (16)));
	#endif

	/* PROCESS 8 ELEMENTS LOOP */
	for (dimX=0; dimX<dimN_8; dimX++) {
	  m1 = _mm_sub_ps(*m128pt++, *m128rv++);
	  m1 = _mm_mul_ps(m1, m1);
	  m1 = _mm_mul_ps(m1, *m128cv++);
	  mo = _mm_add_ps(mo, m1);
	  m2 = _mm_sub_ps(*m128pt++, *m128rv++);
	  m2 = _mm_mul_ps(m2, m2);
	  m2 = _mm_mul_ps(m2, *m128cv++);
	  mo = _mm_add_ps(mo, m2);
	}
	_mm_store_ps (m_result, mo);
	distSum += m_result[0] + m_result[1] + m_result[2] + m_result[3];
	if (distSum > minDistSum) continue;

	/* PROCESS SINGLE 4 ELEMENTS */
	if ((dimN&4)!=0) {
	  m1 = _mm_sub_ps(*m128pt++, *m128rv++);
	  m1 = _mm_mul_ps(m1, m1);
	  m1 = _mm_mul_ps(m1, *m128cv++);
	  _mm_store_ps (m_result, m1);
	  distSum += m_result[0] + m_result[1] + m_result[2] + m_result[3];
	  if (distSum > minDistSum) continue;
	}

	/* PROCESS REMAINING ELEMENTS */
	pt = (float*)m128pt;
	rv = (float*)m128rv;
	cv = (float*)m128cv;
	if ((dimN&2)!=0) {
	  register float diff0 = *rv++ - *pt++;
	  register float diff1 = *rv++ - *pt++;
	  distSum += diff0*diff0*(*cv++);
	  distSum += diff1*diff1*(*cv++);
	}
	if ((dimN&1)!=0) {
	  register float diff0 = *rv++ - *pt++;
	  distSum += diff0*diff0*(*cv++);
	}
#else
#ifdef __INTEL_COMPILER
	float   *restrict pt   = pattern;
	float   *restrict rv   = cbP->rv->matPA[refX];
	float   *restrict cv   = cbP->cv[refX]->m.d;

        if (gscsP) gscsP->gaussians_computed++;

        for (dimX = 0; dimX < dimN; dimX++)
          sa[dimX] = rv[dimX] - pt[dimX], distSum += sa[dimX]*sa[dimX]*cv[dimX];
#else
	float   *pt            =  pattern;
	float   *rv            =  cbP->rv->matPA[refX];
	float   *cv            =  cbP->cv[refX]->m.d;
	int      dimN_4        = dimN / 4;

        if (gscsP) gscsP->gaussians_computed++;

        for (dimX = 0; dimX < dimN_4; dimX++) {
          register float diff0;
          if (distSum > minDistSum) break;
          diff0 = *rv++ - *pt++;
          distSum += diff0*diff0*(*cv++);
          diff0 = *rv++ - *pt++;
          distSum += diff0*diff0*(*cv++);
          diff0 = *rv++ - *pt++;
          distSum += diff0*diff0*(*cv++);
          diff0 = *rv++ - *pt++;
          distSum += diff0*diff0*(*cv++);
        }
        if (dimX == dimN_4) {
          for (dimX = 4*dimN_4; dimX < dimN; dimX++) {
            register float diff0 = *rv++ - *pt++;
            distSum += diff0*diff0*(*cv++);
          }
        }
#endif /* INTEL_COMPILER */
#endif /* SSE_OPT */
      } else
	distSum += gscsP->backoffValue;

      if (distSum < minDistSum) {
	minDistSum = distSum;
	minDistIdx = refX;
	bestI      = i;
      }
    }

    /* assume topN=1, fully continuous */
    scoreA[snX]= snsP->scoreScale * 0.5 * (minDistSum + 2 * dsP->val[minDistIdx]);

    /* reorder gaussian in the bbi leaf */
    if (bestI != 0 && bbilist != _tmpI) {
      bbilist[bestI] = bbilist[0];
      bbilist[0]     = minDistIdx;
    }
  }

  return TCL_OK;
}

#ifdef SSE_OPT
/* This is from QN_fltvec.h from ICSI's QuickNet package
   It was tested on little endian machines (Intel)
   QN_EXPQ = expf --- but fast (Florian) */
#define QN_EXP_A (1048576/M_LN2)
#define QN_EXP_C 60801
#ifdef QN_WORDS_BIGENDIAN
#define QN_EXPQ(y) (qn_d2i.n.j = (int) (QN_EXP_A*(y)) + (1072693248 - QN_EXP_C), (y > -700.0f && y < 700.0f) ? qn_d2i.d : exp(y) )
#else 
#define QN_EXPQ(y) (qn_d2i.n.i = (int) (QN_EXP_A*(y)) + (1072693248 - QN_EXP_C), (y > -700.0f && y < 700.0f) ? qn_d2i.d : exp(y) )
#endif
#define QN_EXPQ_WORKSPACE union { double d; struct { int j,i;} n; } qn_d2i; qn_d2i.d = 0.0;

/* --------------------------------------------------------------
     ssa_opt_inner
   -----------------
     Computes the inner product for a number of frames
     Call with *mdA = NULL -> get 
   -------------------------------------------------------------- */

int ssa_opt_inner (CodebookSet* cbsP, Codebook* cbP, int dimN, int frameX, int N, 
		   float* scA, unsigned char* mdA, Distrib* dsA)
{
  int      refN = cbP->refN;
  __m128**  ptA = cbsP->swcP->ptA;
  float    tmpA[refN];
  int         i;

  /* Either we return the minDistIdx or we compute the full distribution */
  assert (mdA || dsA);

  /* Loop over all frames */
  for (i = 0; i < N; i++) {
    float minDistSum = 1E20;
    int   minDistIdx = 0;  
    int         refX;

    /* Loop over all ref vectors */
    for (refX = 0; refX < refN; refX++) {
      float  distSum = 0;
      __m128 *m128pt = (__m128*) (ptA[i]);
      __m128 *m128rv = (__m128*) cbP->rv->matPA[refX];
      __m128 *m128cv = (__m128*) cbP->cv[refX]->m.d;
      __m128  mo     = _mm_set_ps1(0.0);
      float  *pt, *rv, *cv;
      float   m_result[4] __attribute__ ((aligned (16)));

      assert (dimN < 64);
      if (dimN&32) {
	__m128 m2, m3, m4, m5, m6, m7, m8;
	mo = _mm_sub_ps(*m128pt++, *m128rv++), mo = _mm_mul_ps(mo, mo), mo = _mm_mul_ps(mo, *m128cv++);
	m2 = _mm_sub_ps(*m128pt++, *m128rv++), m2 = _mm_mul_ps(m2, m2), m2 = _mm_mul_ps(m2, *m128cv++);
	mo = _mm_add_ps(mo, m2);
	m3 = _mm_sub_ps(*m128pt++, *m128rv++), m3 = _mm_mul_ps(m3, m3), m3 = _mm_mul_ps(m3, *m128cv++);
	mo = _mm_add_ps(mo, m3);
	m4 = _mm_sub_ps(*m128pt++, *m128rv++), m4 = _mm_mul_ps(m4, m4), m4 = _mm_mul_ps(m4, *m128cv++);
	mo = _mm_add_ps(mo, m4);
	m5 = _mm_sub_ps(*m128pt++, *m128rv++), m5 = _mm_mul_ps(m5, m5), m5 = _mm_mul_ps(m5, *m128cv++);
	mo = _mm_add_ps(mo, m5);
	m6 = _mm_sub_ps(*m128pt++, *m128rv++), m6 = _mm_mul_ps(m6, m6), m6 = _mm_mul_ps(m6, *m128cv++);
	mo = _mm_add_ps(mo, m6);
	m7 = _mm_sub_ps(*m128pt++, *m128rv++), m7 = _mm_mul_ps(m7, m7), m7 = _mm_mul_ps(m7, *m128cv++);
	mo = _mm_add_ps(mo, m7);
	m8 = _mm_sub_ps(*m128pt++, *m128rv++), m8 = _mm_mul_ps(m8, m8), m8 = _mm_mul_ps(m8, *m128cv++);
	mo = _mm_add_ps(mo, m8);
      }
      if (dimN&16) {
	__m128 m1, m2, m3, m4;
	m1 = _mm_sub_ps(*m128pt++, *m128rv++), m1 = _mm_mul_ps(m1, m1), m1 = _mm_mul_ps(m1, *m128cv++);
	mo = _mm_add_ps(mo, m1);
	m2 = _mm_sub_ps(*m128pt++, *m128rv++), m2 = _mm_mul_ps(m2, m2), m2 = _mm_mul_ps(m2, *m128cv++);
	mo = _mm_add_ps(mo, m2);
	m3 = _mm_sub_ps(*m128pt++, *m128rv++), m3 = _mm_mul_ps(m3, m3), m3 = _mm_mul_ps(m3, *m128cv++);
	mo = _mm_add_ps(mo, m3);
	m4 = _mm_sub_ps(*m128pt++, *m128rv++), m4 = _mm_mul_ps(m4, m4), m4 = _mm_mul_ps(m4, *m128cv++);
	mo = _mm_add_ps(mo, m4);
      }
      if (dimN&8) {
	__m128 m1, m2;
	m1 = _mm_sub_ps(*m128pt++, *m128rv++), m1 = _mm_mul_ps(m1, m1), m1 = _mm_mul_ps(m1, *m128cv++);
	mo = _mm_add_ps(mo, m1);
        m2 = _mm_sub_ps(*m128pt++, *m128rv++) ,m2 = _mm_mul_ps(m2, m2), m2 = _mm_mul_ps(m2, *m128cv++);
	mo = _mm_add_ps(mo, m2);
      }
      if (dimN&4) {
	__m128 m1;
	m1 = _mm_sub_ps(*m128pt++, *m128rv++), m1 = _mm_mul_ps(m1, m1), m1 = _mm_mul_ps(m1, *m128cv++);
	mo = _mm_add_ps(mo, m1);
      } 
      _mm_store_ps (m_result, mo);
      distSum += m_result[0] + m_result[1] + m_result[2] + m_result[3];
      // if (distSum > minDistSum) continue;
      
      /* PROCESS REMAINING ELEMENTS */
      pt = (float*)m128pt;
      rv = (float*)m128rv;
      cv = (float*)m128cv;
      if ((dimN&2)!=0) {
	float diff0 = *rv++ - *pt++;
	float diff1 = *rv++ - *pt++;
	distSum += diff0*diff0*(*cv++);
	distSum += diff1*diff1*(*cv++);
      }
      if ((dimN&1)!=0) {
	float diff0 = *rv++ - *pt++;
	distSum += diff0*diff0*(*cv++);
      }

      /* late to minimize page faults (or should we pre-compute all this stuff?) */
      distSum += cbP->pi + cbP->cv[refX]->det;
      
      /* Handling of gdft (pre-compute as well for page faults?) */
      if (cbsP->gdft_frame2class && cbsP->gdft_offset && cbsP->gdft_frame2class[frameX+i] >= 0)
	distSum += cbsP->gdft_offset[cbsP->gdft_frame2class[frameX+i]];

      /* Store the best distance */
      if (distSum < minDistSum) {
	minDistSum = distSum;
	minDistIdx = refX;
      }
      tmpA[refX] = distSum; /* / (scaleRV*scaleRV*scaleCV); */
    }
    
    if (mdA) { /* topN = 1 */
      scA[i] = minDistSum;
      mdA[i] = minDistIdx;
    } else {   /* topN = 0 */
      float fscore = 0;
      QN_EXPQ_WORKSPACE;

      for (refX = 0; refX < refN; refX++)
	fscore += QN_EXPQ (0.5*minDistSum - 0.5*tmpA[refX] - dsA->val[refX]);
      scA[i] = 0.5*minDistSum - logf (fscore);
    }
  }

  return 0;
}

/* --------------------------------------------------------------
     ssa_matrix
   -----------------
     Computes the full matrix of scores
   -------------------------------------------------------------- */
int ssa_matrix (DistribSet* dssP, FMatrix* scoreP, FMatrix* featP, int topN, char* mode)
{
  CodebookSet*   cbsP = dssP->cbsP;
  __m128**        ptA = cbsP->swcP->ptA;
  int             dsN = dssP->list.itemN;
  int             cbN = cbsP->list.itemN;
  int          frameN = featP->m;

  cbsP->swcP->ptA = (__m128**) featP->matPA;
  if (!strcmp (mode, "cb")) {
    int cbX;
    fmatrixResize (scoreP, frameN, cbN);

    if (scoreP == cbsP->swcP->matP) {
      /* We want to compute everything locally */
      if (cbsP->swcP->mdA) _mm_free (cbsP->swcP->mdA);
      cbsP->swcP->mdA = (unsigned char*) _mm_malloc (cbsP->swcP->matP->n * cbsP->swcP->matP->m * sizeof (char), 16);
    }

#pragma omp parallel for
    for (cbX = 0; cbX < cbN; cbX++) {
      unsigned char   mdA[frameN];
      float           scA[frameN];
      unsigned char* _mdA = (scoreP == cbsP->swcP->matP) ? cbsP->swcP->mdA + cbX*cbsP->swcP->matP->n : mdA;
      int          frameY;
      if (!cbsP->list.itemA[cbX].rv->matPA[0])
	continue;

      ssa_opt_inner (cbsP, cbsP->list.itemA+cbX, featP->n, 0, featP->m, scA, _mdA, NULL);
      for (frameY = 0; frameY < frameN; frameY++) 
	scoreP->matPA[frameY][cbX] = scA[frameY];
    }

  } else if (!strcmp (mode, "ds")) {
    int dsX;
    fmatrixResize (scoreP, frameN, dsN);

#pragma omp parallel for
    for (dsX = 0; dsX < dsN; dsX++) {
      unsigned char mdA[frameN];
      float         scA[frameN];
      Distrib*      dsP = dssP->list.itemA+dsX;
      int           cbX = dsP->cbX;
      int        frameY;
      if (!cbsP->list.itemA[cbX].rv->matPA[0])
	continue;

      /* This has not been optimized for semCont systems yet */
      if (topN) {
	ssa_opt_inner (cbsP, cbsP->list.itemA+cbX, featP->n, 0, featP->m, scA, mdA, NULL);
	for (frameY = 0; frameY < frameN; frameY++) 
	  scoreP->matPA[frameY][dsX] = 0.5 * scA[frameY] + dsP->val[mdA[frameY]];
      } else {
	ssa_opt_inner (cbsP, cbsP->list.itemA+cbX, featP->n, 0, featP->m, scA, NULL, dsP);
	for (frameY = 0; frameY < frameN; frameY++) 
	  scoreP->matPA[frameY][dsX] = scA[frameY];
      }
    }

  } else {
    WARN ("Unknown mode '%s'\n", mode);
    cbsP->swcP->ptA = ptA;
    return TCL_ERROR;  
  }

  cbsP->swcP->ptA = ptA;
  return 0;
}

int ssa_opt (SenoneSet* snsP, int* idxA, float* scoreA, 
	     int  idxN, int frameX)
{
  StreamArray*     staP = &(snsP->strA); 
  Senone*           snP = snsP->list.itemA;
  DistribSet*      dssP = ((DistribStream*) staP->cdA[0])->dssP;
  CodebookSet*     cbsP = dssP->cbsP;
  Codebook*         cbP = cbsP->list.itemA;
  int cbN, idx, snX, dsX, cbX, streamX;

  /* We don't deal with BBI and GS (won't work with multi-stream, though) */
  if (cbsP->gscsP || (cbsP->bbi.itemA && cbP->bbiX >= 0) || cbsP->swc_queries < 0)
    return ssa_opt2 (snsP, idxA, scoreA, idxN, frameX);

  /* Clear this thing */
  for (idx = 0; idx < idxN; idx++)
    scoreA[idxA[idx]] = 0;

  /* Loop over all streams */
  for (streamX = 0; streamX < staP->streamN; streamX++) {
    dssP                  = ((DistribStream*) staP->cdA[streamX])->dssP;
    cbsP                  = dssP->cbsP;
    cbP                   = cbsP->list.itemA;
    Distrib*          dsP = NULL;
    CbWindowCache*   cbcP = cbsP->swcP;
    int              dimN = (cbP->cfg)->rdimN > 0 ? (cbP->cfg)->rdimN : cbP->dimN; /* potentially unsafe */
    int               frM = (snsP->toX < 0) ? cbsP->swc_width/2 : cbsP->swc_width; /* width of window (narrow if we don't decode) */
    int            warned = 0;
    int    frameY, offset;
    int* scI;

    assert (frM);

    /* Sliding Window Cache Allocation */
    if (cbcP->matP->count) { /* fixed window cache case */
      if (cbcP->baseP->n < cbsP->list.itemN) {
	imatrixResize (cbcP->baseP, 1, cbsP->list.itemN);
	imatrixClear (cbcP->baseP); /* do we need this? */
      }
      if (cbcP->cbL) _mm_free (cbcP->cbL);
      if (cbcP->mdA) _mm_free (cbcP->mdA);
      if (cbcP->ptA) _mm_free (cbcP->ptA);
      cbcP->cbL   = (int*)           _mm_malloc (                cbcP->matP->m * sizeof (int),     16);
      cbcP->mdA   = (unsigned char*) _mm_malloc (cbcP->matP->n * cbcP->matP->m * sizeof (char),    16);
      cbcP->ptA   = (__m128 **)      _mm_malloc (cbcP->matP->n                 * sizeof (__m128*), 16);
      cbcP->frX   = frameX+1;      
      frM = cbcP->matP->n;
    } else if (cbsP->list.itemN > cbcP->matP->m || frM > cbcP->matP->n) { /* sliding window cache case */
      fmatrixFree (cbcP->matP);
      imatrixFree (cbcP->baseP);
      if (cbcP->cbL) _mm_free (cbcP->cbL);
      if (cbcP->mdA) _mm_free (cbcP->mdA);
      if (cbcP->ptA) _mm_free (cbcP->ptA);
      cbcP->matP  = fmatrixCreate (   cbsP->list.itemN, frM); /* m, n */
      cbcP->baseP = imatrixCreate (1, cbsP->list.itemN);
      cbcP->cbL   = (int*)           _mm_malloc (                cbcP->matP->m * sizeof (int),     16);
      cbcP->mdA   = (unsigned char*) _mm_malloc (cbcP->matP->n * cbcP->matP->m * sizeof (char),    16);
      cbcP->ptA   = (__m128 **)      _mm_malloc (cbcP->matP->n                 * sizeof (__m128*), 16);
      cbcP->frX   = frameX+1;
    }
    scI = cbcP->baseP->matPA[0];

    /* Sliding Window Cache Invalidation and offset calculation */
    if (!cbcP->matP->count && (cbcP->frX < frameX-1 || cbcP->frX > frameX)) 
      for (idx = 0; idx < cbcP->matP->m; idx++)
	scI[idx] = -cbcP->matP->n-1;
    offset    = (snsP->toX < 0 || snsP->toX - frameX > frM) ? 0 : frM - snsP->toX + frameX;
    cbcP->frX = frameX;

    /* get pattern (same feature space for all codebooks) */
    idx = 0;
    snX = idxA[idx];
    dsX = snP[snX].classXA[streamX];
    dsP = dssP->list.itemA + dsX;
    cbX = dsP->cbX;
    cbP = cbsP->list.itemA + cbX;
    if (!(cbcP->ptA[0] = (__m128*) fesFrame (cbsP->feat, cbP->featX, frameX))) {
      ERROR ("Can't get frame %d of feature %d\n", frameX, cbP->featX);
      for (idx = 0; idx < idxN; idx++)
	scoreA[idx] = 0.0;
      return TCL_OK;
    }
    for (frameY = 1; frameY < frM - offset && 
	   (cbcP->ptA[frameY] = (__m128*) fesFrame (cbsP->feat, cbP->featX, frameX+frameY)); 
	 frameY++);

    /* Map senones to codebooks and check cache */
    cbcP->cbL[0] = cbsP->list.itemN+1;
    for (idx = cbN = 0; idx < idxN; idx++) {
      snX = idxA[idx];
      dsX = snP[snX].classXA[streamX];
      dsP = dssP->list.itemA + dsX;
      cbX = dsP->cbX;
      int cbY, a, o;

      /* Is this codebook already cached? Then don't do anything */
      if (scI[cbX] > frameX - frM)
	    continue;

      /* Is the codebook loaded? */
      if (!cbsP->list.itemA[cbX].rv || !cbsP->list.itemA[cbX].rv->matPA[0] || !cbsP->list.itemA[cbX].rv->matPA[0]) {
	    if (!warned) WARN ("ssa_opt: codebook %s not loaded, ignoring\n", cbsP->list.itemA[cbX].name);
        warned = 1;
	    continue;
      }

      /* Insert it properly */
      a = 0, o = cbN-1;
      while (o-a > 1) {
	    int e = (a+o)/2;
	    if (cbcP->cbL[e] >  cbX) o = e;
	    else                     a = e;
      }
      if (cbcP->cbL[a]   == cbX)
	    continue;
      for (cbY = cbN++; cbY > a; cbY--)
	    cbcP->cbL[cbY]   =       cbcP->cbL[cbY-1];
      if (cbcP->cbL[a]   <  cbX) cbcP->cbL[a+1] = cbX;
      else                       cbcP->cbL[a]   = cbX;
    }
    cbsP->swc_queries += idxN; /* book-keeping (doesn't work for semi-tied systems) */
    cbsP->swc_hits    += idxN-cbN;

    /* Compute distances for all codebooks indexed in cbL */
#pragma omp parallel for if(cbN>32) private(idx,cbX,cbP)
    for (idx = 0; idx < cbN; idx++) {
      cbX  = cbcP->cbL[idx];
      cbP  = cbsP->list.itemA + cbX;
      ssa_opt_inner (cbsP, cbP, dimN, frameX, frameY, cbcP->matP->matPA[cbX]+offset,
		     cbcP->mdA + cbX*cbcP->matP->n + offset, NULL);
      scI[cbX] = frameX - offset;
    }

    /* Now the cache is valid. Compute the scores for this frame */
    for (idx = 0; idx < idxN; idx++) {
      snX    = idxA[idx];
      dsX    = snP[snX].classXA[streamX];
      dsP    = dssP->list.itemA + dsX;
      cbX    = dsP->cbX;
      frameY = cbX*cbcP->matP->n + frameX-scI[cbX];
      if (cbcP->matP->count) /* pre-computed dss */
	scoreA[snX] += snsP->scoreScale * snP[snX].stwA[streamX] * 
	  cbcP->matP->matPA[dsX][frameX];
      else /* current cbs */
	scoreA[snX] += snsP->scoreScale * snP[snX].stwA[streamX] * 
	  (0.5 * cbcP->matP->matPA[cbX][frameX-scI[cbX]] + dsP->val[cbcP->mdA[frameY]]);
      assert (scoreA[snX] < 1000000);
    }

    if (cbcP->matP->count)
      cbcP->matP->count -= 1;
  }
  
  return TCL_OK;
}
#endif

/* ------------------------------------------------------------------------
    ssa_opt_thread
   ---------
    optimized implementation for score array functions using threads
   ------------------------------------------------------------------------ */

#ifdef MTHREAD
typedef struct {
  SenoneSet*  snsP;
  DistribSet* dssP;
  BBITree*    bbi;
  float*      pattern;
  float*      scoreA;
  int*        idxA;  
  int         idxN;
  int         adr;
  int         dimN;
  int         streamX;
} ssaThreadInfo_t;

static int ssa_thread_init = 0;

static ssaThreadInfo_t ssaThreadInfo;
static pthread_attr_t pthreadAttribute;
static pthread_t      ssaThreadTID      [NUM_THREADS];
static sem_t          ssaThreadBeginSem [NUM_THREADS];
static sem_t          ssaThreadReadySem [NUM_THREADS];
static int            ssaThreadIDS      [NUM_THREADS];

void* ssaThread( void* arg) 
{
  int*         tidP  = (int*) arg;
  int           tid  = *tidP;
  SenoneSet*   snsP  = NULL;
  DistribSet*  dssP  = NULL;
  CodebookSet* cbsP  = NULL;
  Distrib*      dsP  = NULL;
  Codebook*     cbP  = NULL;
  BBITree*      bbi  = NULL;
  CBX*      bbilist  = NULL;
  int*         idxA  = NULL;
  float*     scoreA  = NULL;
  CBX*        _tmpI  = NULL;
  int          idxN  = 0;
  int          dimN  = 0;
  int        dimN_4  = 0;
  int idx,idxStart,idxEnd,snX,dsX,cbX,bbiN,refN;

#ifdef __INTEL_COMPILER
  float sa[saDimN]; 			
  int   minDistIdx, i, bestI;
  float minDistSum;
#else
  register int   minDistIdx, i, bestI;
  register float minDistSum;
#endif

  while(1) {
    sem_wait( &ssaThreadBeginSem[tid]);
    snsP   = ssaThreadInfo.snsP;
    dssP   = ssaThreadInfo.dssP;  
    bbi    = ssaThreadInfo.bbi;  
    scoreA = ssaThreadInfo.scoreA;  
    idxA   = ssaThreadInfo.idxA;
    idxN   = ssaThreadInfo.idxN;
    dimN   = ssaThreadInfo.dimN;
    cbsP   = dssP->cbsP;
    _tmpI  = snsP->refXA;
    dimN_4 = dimN / 4;

    idxStart = idxN * tid       / NUM_THREADS;
    idxEnd   = idxN * (tid +1)  / NUM_THREADS;

    for (idx = idxStart; idx < idxEnd; idx++) {
      snX  = idxA[idx];    
      dsX  = snsP->list.itemA[snX].classXA[ssaThreadInfo.streamX];
      dsP  = dssP->list.itemA + dsX;
      cbX  = dsP->cbX;
      cbP  = cbsP->list.itemA + cbX;
      refN = cbP->refN;

      if ((!cbP->rv || !cbP->cv) && !_WarnedNoCodeBooks) {
	ERROR ("ssaThread: codebook '%s' not loaded => bogus results.\n", cbP->name);
	_WarnedNoCodeBooks = 1;
	continue;
      }

      /*bbi tree*/
      if (cbsP->bbi.itemA && cbP->bbiX >= 0 && cbP->bbiY >= 0) {
	BBILeaf *bucket = &bbi->leaf[ssaThreadInfo.adr][cbP->bbiY];
	bbiN     = bucket->cbvN;              
	bbilist  = bucket->cbX;               
      } else {
	bbiN     = refN;
	bbilist  = _tmpI;
      }

      minDistSum = 1E20;
      minDistIdx = 0;  
      bestI      = 0;
      for (i = 0; i < bbiN; i++) {     
	int refX = bbilist[i], dimX;

#ifdef __INTEL_COMPILER
	float        distSum = cbP->pi + cbP->cv[refX]->det;
	float   *restrict pt = ssaThreadInfo.pattern;
	float   *restrict rv = cbP->rv->matPA[refX];
	float   *restrict cv = cbP->cv[refX]->m.d;

	for (dimX = 0; dimX < dimN; dimX++)
	  sa[dimX] = rv[dimX] - pt[dimX], distSum += sa[dimX]*sa[dimX]*cv[dimX];
#else
	register float distSum = cbP->pi + cbP->cv[refX]->det;
	float   *pt      =  ssaThreadInfo.pattern;
	float   *rv      =  cbP->rv->matPA[refX];
	float   *cv      =  cbP->cv[refX]->m.d;

	for (dimX = 0; dimX < dimN_4; dimX++) {
	  register float diff0;
	  if (distSum > minDistSum) break;
	  diff0 = *rv++ - *pt++;
	  distSum += diff0*diff0*(*cv++);
	  diff0 = *rv++ - *pt++;
	  distSum += diff0*diff0*(*cv++);
	  diff0 = *rv++ - *pt++;
	  distSum += diff0*diff0*(*cv++);
	  diff0 = *rv++ - *pt++;
	  distSum += diff0*diff0*(*cv++);
	}
	if (dimX == dimN_4) {
	  for (dimX = 4*dimN_4; dimX < dimN; dimX++) {
	    register float diff0 = *rv++ - *pt++;
	    distSum += diff0*diff0*(*cv++);
	  }
	}
#endif

	if (distSum < minDistSum) {
	  minDistSum = distSum;
	  minDistIdx = refX;
	  bestI      = i;
	}
      }

      /*assume fully continuous system*/
      scoreA[snX]=  0.5 * (minDistSum + 2 * dsP->val[minDistIdx]);

      /* reorder gaussian in the bbi leaf */
      if (bestI != 0 && bbilist != _tmpI) {
	bbilist[bestI] = bbilist[0];
	bbilist[0]     = minDistIdx;
      }
    }
    sem_post(&ssaThreadReadySem[tid]);
  }

  return NULL;
}

static void ssaThreadInit() 
{
  int   i;

  pthread_attr_init(     &pthreadAttribute);
  pthread_attr_setscope( &pthreadAttribute, PTHREAD_SCOPE_SYSTEM);

  for (i = 0; i < NUM_THREADS; i++) {
    ssaThreadIDS[i] = i;
    sem_init( &ssaThreadBeginSem[i], 0, 0);
    sem_init( &ssaThreadReadySem[i], 0, 0);
    pthread_create (&ssaThreadTID[i], &pthreadAttribute, ssaThread, &(ssaThreadIDS[i]));
  }

  ssa_thread_init = 1;
}


int ssa_opt_thread (SenoneSet* snsP, int* idxA, float* scoreA, 
		    int  idxN, int frameX)
{
  int       streamX  = 0;  
  StreamArray* staP  = &(snsP->strA); 
  DistribSet*  dssP  = ((DistribStream*) staP->cdA[streamX])->dssP;
  CodebookSet* cbsP  = dssP->cbsP;
  Senone*       snP  = snsP->list.itemA;
  Distrib*      dsP  = NULL;
  Codebook*     cbP  = cbsP->list.itemA + 0;
  float*    pattern  = NULL;
  BBITree*      bbi  = NULL;
  int         _tmpN  = 0;
  CBX*        _tmpI  = snsP->refXA;
  int          itemN = cbsP->list.itemN;

  /*assume that all codebooks have the same dimN as the first one*/
  int          dimN  = (cbP->cfg)->rdimN > 0 ? (cbP->cfg)->rdimN : cbP->dimN;

  int idx, snX, dsX, cbX, i, adr=0, depthN=0;

  if (_tmpN == 0) {
    cbX = cbsP->list.itemN--;
    while (cbX>=0) {
      if (_tmpN < cbP[cbX].refN) _tmpN = cbP[cbX].refN;
      cbX--;
    }
    if (NULL == (_tmpI = malloc(_tmpN*sizeof(int)))) {
      ERROR("ssa_opt: allocation failure\n");
    }
    for (idx=0;idx<_tmpN;idx++) _tmpI[idx]=idx;
  }

  /* get pattern (same feature space for all codebooks) */
  idx = 0;
  snX = idxA[idx];
  dsX = snP[snX].classXA[streamX];
  dsP = dssP->list.itemA + dsX;
  cbX = dsP->cbX;
  cbP = cbsP->list.itemA + cbX;
  pattern = fesFrame (cbsP->feat, cbP->featX, frameX);

  /* get bbi tree if available */ 
  if (cbsP->bbi.itemA && cbP->bbiX >=0) {
    bbi    = cbsP->bbi.itemA + cbP->bbiX;
    depthN = bbi->depthN; 
    i      = 1;     
    for (idx=0; idx<depthN; idx++) {
      BBINode*  actNode = (bbi->node)+i;
      i = (i<<1) + (pattern[(int) actNode->k] > actNode->h);            
    }
    adr = i-(1<<depthN);            
  } 

  if (! ssa_thread_init) ssaThreadInit();

  ssaThreadInfo.snsP    = snsP;
  ssaThreadInfo.dssP    = dssP;
  ssaThreadInfo.bbi     = bbi;
  ssaThreadInfo.pattern = pattern;
  ssaThreadInfo.scoreA  = scoreA;
  ssaThreadInfo.idxA    = idxA;
  ssaThreadInfo.idxN    = idxN;
  ssaThreadInfo.adr     = adr;
  ssaThreadInfo.dimN    = dimN;
  ssaThreadInfo.streamX = streamX;

  for ( i = 0; i < NUM_THREADS; i++) sem_post( &ssaThreadBeginSem[i]);
  for ( i = 0; i < NUM_THREADS; i++) sem_wait( &ssaThreadReadySem[i]);

  cbsP->list.itemN = itemN;

  return TCL_OK;
}

#else  

int ssa_opt_thread (SenoneSet* snsP, int* idxA, float* scoreA, 
		    int  idxN, int frameX)
{
  WARN("ssa_opt_thread: IBIS wasn't compiled with thread support\n");
  snsP->scoreAFctP = &ssa_opt;
  return ssa_opt(snsP,idxA,scoreA,idxN,frameX);
}
#endif


/* ------------------------------------------------------------------------
    ssa_base_semCont
   ------------------
    optimized implementation for score array functions and semi-continuous
    systems
   ------------------------------------------------------------------------ */

float* mdA = NULL;
int*   giA = NULL;
int    giN = 0;

float* gtA = NULL;
int    gtN = 0;

int ssa_base_semCont (SenoneSet* snsP, int* idxA, float* scoreA, int idxN, int frameX) {
  int       streamX  = 0;  
  StreamArray* staP  = &(snsP->strA); 
  DistribSet*  dssP  = ((DistribStream*) staP->cdA[streamX])->dssP;
  CodebookSet* cbsP  = dssP->cbsP;
  int           cbN  = cbsP->list.itemN; /* number of codebooks in codebookSet */
  Senone*       snP  = snsP->list.itemA;
  Distrib*      dsP  = NULL;
  Codebook*     cbP  = cbsP->list.itemA + 0;
  float     scaleRV  = cbsP->scaleRV;
  float     scaleCV  = cbsP->scaleCV;
  float*    pattern  = NULL;

  /*assume that all codebooks have the same dimN as the first one*/
  int          refN  = 0;
  int          dimN  = (cbP->cfg)->rdimN > 0 ? (cbP->cfg)->rdimN : cbP->dimN;

  int idx, snX, dsX, dsN, cbX, refX;

#ifdef __INTEL_COMPILER
  float          sa[saDimN]; /* NOTE: This is hardcoded, because otherwise ICC would
				not VECTORIZE the loop which is very bad for speed! */
  int            i;

  assert (dimN <= saDimN);
#else
  register int   i;
#endif

  /* get pattern (same feature space for all codebooks) */
  idx = 0;
  snX = idxA[idx];
  dsX = snP[snX].classXA[streamX];
  dsP = dssP->list.itemA + dsX;
  cbX = dsP->cbX;
  cbP = cbsP->list.itemA + cbX;
  if (!(pattern = fesFrame (cbsP->feat, cbP->featX, frameX))) {
    ERROR ("Can't get frame %d of feature %d\n", frameX, cbP->featX);
    for (idx = 0; idx < idxN; idx++)
      scoreA[idx] = 0.0;
    return TCL_OK;
  }

  /* Initialize the cache for the mindistance to all codebooks and the index array*/
  if (cbN > giN) {
    giA = realloc (giA, (giN = cbN) * sizeof (int));
    mdA = realloc (mdA,        cbN  * sizeof (float));
  }
  for (idx = dsN = 0; idx < idxN; idx++) {
    snX  = idxA[idx];    
    dsX  = snP[snX].classXA[streamX];
    dsP  = dssP->list.itemA + dsX;
    cbX  = dsP->cbX;
    cbP  = cbsP->list.itemA + cbX;
    refN = cbP->refN;
    
    giA[cbX]  = dsN;
    dsN      += refN;
  }
  if (dsN > gtN)
    gtA = realloc (gtA, (gtN = dsN) * sizeof (float)); 
  for (idx = 0; idx < cbN; idx++)
    mdA[idx] = -1.0;

  /* ------------------------------------------------------------------ */
  /* compute scores for all necessary distributions indexed in idxA     */
  /* ------------------------------------------------------------------ */
 
  for (idx = 0; idx < idxN; idx++) {
    float  fscore = 0.0;
    float*    scA;

    /* find corresponding codebook from senone index idxA[i] */
    snX  = idxA[idx];    
    dsX  = snP[snX].classXA[streamX];
    dsP  = dssP->list.itemA + dsX;
    cbX  = dsP->cbX;
    cbP  = cbsP->list.itemA + cbX;
    refN = cbP->refN;
    
    if ((!cbP->rv || !cbP->cv) && !_WarnedNoCodeBooks) {
      ERROR ("ssa_opt: codebook '%s' not loaded => bogus results.\n", cbP->name);
      _WarnedNoCodeBooks = 1;
      continue;
    }
    
    scA = gtA + giA[cbX];

    /* ---------------------------------------------------------------- */
    /* Check whether the minDist for this codebook is already cached    */
    /* Compute if necessary                                             */
    /* ---------------------------------------------------------------- */
    if (mdA[cbX] < 0) {
      float     mld = 1E20;

      /* ------------------------------------------------------------------*/
      /* Compute all the Mahalanobis distances for the subset of Gaussians */  
      /* ------------------------------------------------------------------*/
      
#ifdef __INTEL_COMPILER
      for (refX = 0; refX < refN; refX++) {
	float        distSum = (cbP->pi + cbP->cv[refX]->det)*scaleRV*scaleRV*scaleCV;
	float   *restrict pt = pattern;
	float   *restrict rv = cbP->rv->matPA[refX];
	float   *restrict cv = cbP->cv[refX]->m.d;
	
	for (i = 0; i < dimN; i++)
	  sa[i] = rv[i] - pt[i], distSum += sa[i]*sa[i]*cv[i];

	scA[refX] = 0.5 * distSum / (scaleRV*scaleRV*scaleCV);
      }
      for (refX = 0; refX < refN; refX++)
	if (scA[refX] < mld) mld = scA[refX];
#else
      for (refX = 0; refX < refN; refX++) {
	double         distSum = (cbP->pi + cbP->cv[refX]->det)*scaleRV*scaleRV*scaleCV;
	float   *pt            = pattern;
	float   *rv            = cbP->rv->matPA[refX];
	float   *cv            = cbP->cv[refX]->m.d;

	for (i = 0; i < dimN; i++) {
	  register double diff0;
	  diff0    = *rv++ - *pt++;
	  distSum += diff0*diff0*(*cv++);
	}
	distSum   = 0.5 * distSum / (scaleRV*scaleRV*scaleCV);
	scA[refX] = distSum;

	if (distSum < mld)
	  mld = distSum;
      }
#endif

      mdA[cbX] = mld;
    }

    for (i = 0; i < refN; i++)
      if (scA[i] > cbP->cfg->expT)
	fscore += exp (mdA[cbX] - scA[i] - dsP->val[i]);

    scoreA[snX]= (float) (snsP->scoreScale * cbP->cfg->weight * (mdA[cbX] - log (fscore)));
  }

  return TCL_OK;
}

/* ------------------------------------------------------------------------
    ssa_opt_semCont
   -----------------
    optimized implementation for score array functions and semi-continous
    systems
    implies topN = 1; requires all codebooks to have the same dimension
   ------------------------------------------------------------------------ */

int ssa_opt_semCont (SenoneSet* snsP, int* idxA, float* scoreA, int  idxN, int frameX) {
  int       streamX   = 0;  
  StreamArray* staP   = &(snsP->strA); 
  DistribSet*  dssP   = ((DistribStream*) staP->cdA[streamX])->dssP;
  CodebookSet* cbsP   = dssP->cbsP;
  int           cbN   = cbsP->list.itemN; /* number of codebooks in codebookSet */
  Senone*       snP   = snsP->list.itemA;
  Distrib*      dsP   = NULL;
  Codebook*     cbP   = cbsP->list.itemA + 0;
  float*    pattern   = NULL;
  BBITree*      bbi   = NULL;
  CBX*      bbilist   = NULL;
  CBX*        _tmpI   = snsP->refXA;
  GSClusterSet* gscsP = cbsP->gscsP;
  int         usegscs = 0;

  static float*    minDistA = NULL; /* Array of cached minimum distances for all codebooks */
  static int*   minDistIdxA = NULL; /* Array of indeces for the Gaussians within a codebook that has the minimal dist to the feature vector */
  static int       minDistN = 0;

  /*assume that all codebooks have the same dimN as the first one*/
  int          refN   = 0;
  int          dimN   = (cbP->cfg)->rdimN > 0 ? (cbP->cfg)->rdimN : cbP->dimN;

  int idx, snX, dsX, cbX, bbiN=0, adr=0, depthN=0;

#ifdef __INTEL_COMPILER
  float          sa[saDimN]; /* NOTE: This is hardcoded, because otherwise ICC would
				not VECTORIZE the loop which is very bad for speed! */
  int            minDistIdx, i, bestI;
  float          minDistSum;

  assert (dimN <= saDimN);
#else
  register int   minDistIdx, i, bestI;
  register float minDistSum;
#endif

  /* get pattern (same feature space for all codebooks) */
  idx = 0;
  snX = idxA[idx];
  dsX = snP[snX].classXA[streamX];
  dsP = dssP->list.itemA + dsX;
  cbX = dsP->cbX;
  cbP = cbsP->list.itemA + cbX;
  if (!(pattern = fesFrame (cbsP->feat, cbP->featX, frameX))) {
    ERROR ("Can't get frame %d of feature %d\n", frameX, cbP->featX);
    for (idx = 0; idx < idxN; idx++)
      scoreA[idx] = 0.0;
    return TCL_OK;
  }
  
  /* get bbi tree if available */ 
  if (cbsP->bbi.itemA && cbP->bbiX >=0) {
    bbi    = cbsP->bbi.itemA + cbP->bbiX;
    depthN = bbi->depthN; 
    i      = 1;     
    for (idx=0; idx<depthN; idx++) {
      BBINode*  actNode = (bbi->node)+i;
      i = (i<<1) + (pattern[(int) actNode->k] > actNode->h);            
    }
    adr = i-(1<<depthN);            
  } 


  /* Initialize the cache for the mindistance to all codebooks and the index array*/
  if ( cbN > minDistN ) {
    minDistA    = realloc(minDistA,    cbN * sizeof(float));
    minDistIdxA = realloc(minDistIdxA, cbN * sizeof(int));
    minDistN    = cbN;
  }

  if (snsP->streamCache && frameX < snsP->streamCache->m && snsP->streamCache->n == 2*cbN) {
    for (idx = 0; idx < cbN; idx++)
      minDistA[idx]    = snsP->streamCache->matPA[frameX][idx];
    for (idx = 0; idx < cbN; idx++)
      minDistIdxA[idx] = (minDistA[idx] == -FLT_MAX) ? -1 : (int) snsP->streamCache->matPA[frameX][idx+cbN];
  } else for (idx = 0; idx < cbN; idx++) minDistIdxA[idx] = -1;


  /* Activate the Clusters if gsClusterSet is available */
  if (gscsP) {
    if (gscsP->use) {
      gsClusterSetActivate(gscsP,pattern, NULL, gscsP->bestN);
      usegscs = -1;
    }
  }

  /* ------------------------------------------------------------------ */
  /* compute scores for all necessary distributions indexed in idxA     */
  /* ------------------------------------------------------------------ */
  for (idx = 0; idx < idxN; idx++) {

    /*find corresponding codebook from senone index idxA[i] */
    snX  = idxA[idx];    
    dsX  = snP[snX].classXA[streamX];
    dsP  = dssP->list.itemA + dsX;
    cbX  = dsP->cbX;
    cbP  = cbsP->list.itemA + cbX;
    refN = cbP->refN;

    if ((!cbP->rv || !cbP->cv) && !_WarnedNoCodeBooks) {
      ERROR ("ssa_opt: codebook '%s' not loaded => bogus results.\n", cbP->name);
      _WarnedNoCodeBooks = 1;
      continue;
    }

    /* ---------------------------------------------------------------- */
    /* Check whether the minDist for this codebook is already cached    */
    /* Compute if necessary                                             */
    /* ---------------------------------------------------------------- */
    if (minDistIdxA[cbX] == -1) {

      /*bbi tree*/
      if (cbsP->bbi.itemA && cbP->bbiX >= 0 && cbP->bbiY >= 0) {
	BBILeaf *bucket = &bbi->leaf[adr][cbP->bbiY];
	bbiN     = bucket->cbvN;              
	bbilist  = bucket->cbX;               
      } else {
	bbiN     = refN;
	bbilist  = _tmpI;
      }

      if (gscsP) gscsP->gaussians_total += refN;
      
      /* ------------------------------------------------------------------*/
      /* Compute all the Mahalanobis distances for the subset of Gaussians */  
      /* ------------------------------------------------------------------*/
      
      minDistSum = 1E20;
      minDistIdx = 0;  
      bestI      = 0;
      
      for (i = 0; i < bbiN; i++) {     
	int refX = bbilist[i], dimX;
#ifdef __INTEL_COMPILER
	float distSum = cbP->pi + cbP->cv[refX]->det;
#else
	register float distSum = cbP->pi + cbP->cv[refX]->det;
#endif

	if (!usegscs || gscsP->cluster[gscsP->clusterX[cbX][refX]].active) {
#ifdef SSE_OPT
	  __m128 *m128pt = (__m128*) pattern;
	  __m128 *m128rv = (__m128*) cbP->rv->matPA[refX];
	  __m128 *m128cv = (__m128*) cbP->cv[refX]->m.d;
	  __m128  mo     = _mm_set_ps1(0.0);
	  __m128  m1, m2;
	  int     dimN_8 = dimN >> 3;
	  float  *pt, *rv, *cv;
          #if defined __INTEL_COMPILER || defined WINDOWS
	  __declspec(align(16)) float m_result[4];
	  #else
	  float   m_result[4] __attribute__ ((aligned (16)));
	  #endif

	  /* PROCESS 8 ELEMENTS LOOP */
	  for (dimX=0; dimX<dimN_8; dimX++) {
	    m1 = _mm_sub_ps(*m128pt++, *m128rv++);
	    m1 = _mm_mul_ps(m1, m1);
	    m1 = _mm_mul_ps(m1, *m128cv++);
	    mo = _mm_add_ps(mo, m1);
	    m2 = _mm_sub_ps(*m128pt++, *m128rv++);
	    m2 = _mm_mul_ps(m2, m2);
	    m2 = _mm_mul_ps(m2, *m128cv++);
	    mo = _mm_add_ps(mo, m2);
	  }
	  
	  _mm_store_ps (m_result, mo);
	  distSum += m_result[0] + m_result[1] + m_result[2] + m_result[3];
	  if (distSum > minDistSum) continue;
	  
	  /* PROCESS SINGLE 4 ELEMENTS */
	  if ((dimN&4)!=0) {
	    m1 = _mm_sub_ps(*m128pt++, *m128rv++);
	    m1 = _mm_mul_ps(m1, m1);
	    m1 = _mm_mul_ps(m1, *m128cv++);
	    
	    _mm_store_ps (m_result, m1);
	    distSum += m_result[0] + m_result[1] + m_result[2] + m_result[3];
	    if (distSum > minDistSum) continue;
	  }

	  /* PROCESS REMAINING ELEMENTS */
	  pt = (float*)m128pt;
	  rv = (float*)m128rv;
	  cv = (float*)m128cv;
	  if ((dimN&2)!=0) {
	    register float diff0 = *rv++ - *pt++;
	    register float diff1 = *rv++ - *pt++;
	    distSum += diff0*diff0*(*cv++);
	    distSum += diff1*diff1*(*cv++);
	  }
	  if ((dimN&1)!=0) {
	    register float diff0 = *rv++ - *pt++;
	    distSum += diff0*diff0*(*cv++);
	  }
#else
#ifdef __INTEL_COMPILER
	  float   *restrict pt = pattern;
	  float   *restrict rv = cbP->rv->matPA[refX];
	  float   *restrict cv = cbP->cv[refX]->m.d;
	  
	  for (dimX = 0; dimX < dimN; dimX++)
	    sa[dimX] = rv[dimX] - pt[dimX], distSum += sa[dimX]*sa[dimX]*cv[dimX];
#else
	  float   *pt      =  pattern;
	  float   *rv      =  cbP->rv->matPA[refX];
	  float   *cv      =  cbP->cv[refX]->m.d;
	  int      dimN_4  = dimN / 4;
	  
	  for (dimX = 0; dimX < dimN_4; dimX++) {
	    register float diff0;
	    if (distSum > minDistSum) break;
	    diff0 = *rv++ - *pt++;
	    distSum += diff0*diff0*(*cv++);
	    diff0 = *rv++ - *pt++;
	    distSum += diff0*diff0*(*cv++);
	    diff0 = *rv++ - *pt++;
	    distSum += diff0*diff0*(*cv++);
	    diff0 = *rv++ - *pt++;
	    distSum += diff0*diff0*(*cv++);
	  }
	  if (dimX == dimN_4) {
	    for (dimX = 4*dimN_4; dimX < dimN; dimX++) {
	      register float diff0 = *rv++ - *pt++;
	      distSum += diff0*diff0*(*cv++);
	    }
	  }
#endif /* INTEL_COMPILER */
#endif /* SSE_OPT */
	} else { distSum+=gscsP->backoffValue;}

	if (distSum < minDistSum) {
	  minDistSum = distSum;
	  minDistIdx = refX;
	  bestI      = i;
	}
      }

      /* reorder gaussian in the bbi leaf */
      if (bestI != 0 && bbilist != _tmpI) {
	bbilist[bestI] = bbilist[0];
	bbilist[0]     = minDistIdx;
      }

      minDistA[cbX]    = minDistSum;
      minDistIdxA[cbX] = minDistIdx;
    }

    /* topN=1 */
    scoreA[snX]= snsP->scoreScale * 0.5 * (minDistA[cbX] + 2 * dsP->val[minDistIdxA[cbX]]);
  }

  if (snsP->streamCache && frameX < snsP->streamCache->m && snsP->streamCache->n == 2*cbN) {
    for (idx = 0; idx < cbN; idx++)
      snsP->streamCache->matPA[frameX][idx]     =         minDistA[idx];
    for (idx = 0; idx < cbN; idx++)
      snsP->streamCache->matPA[frameX][idx+cbN] = (float) minDistIdxA[idx];
  }

  return TCL_OK;
}

/* ------------------------------------------------------------------------
    ssa_opt_str
   --------------
    optimized implementation for streamed score arrays
   ------------------------------------------------------------------------ */

int ssa_opt_str (SenoneSet* snsP, int* idxA, float* scoreA, 
		  int  idxN, int frameX)
{
  StreamArray*         staP = &(snsP->strA);
  int                   dsN = (staP->streamN > 1) ? 
    staP->cBaseA[staP->streamN] - staP->cBaseA[1] :
    staP->cBaseA[1];
  int              idx, stX;
  float*                 sP;

  /* Create cache if it doesn't exist */
  if (!snsP->streamCache) {
    snsP->streamCache  = fmatrixCreate (1, dsN);
    snsP->streamCacheX = -1;
  }

  /* HACK */
  if (!frameX && staP->streamN == 1 && snsP->streamCache->m == 1) {
#ifdef SSE_OPT
    DistribSet*  dssP = ((DistribStream*) staP->cdA[0])->dssP;
    CodebookSet* cbsP = dssP->cbsP;
    int         featX = cbsP->list.itemA[0].featX;
    Feature*        F = cbsP->feat->featA+featX;
    FMatrix*    featP = F->data.fmat;
    ssa_matrix (dssP, snsP->streamCache, featP, 1, "ds");
    //INFO ("ssa_matrix: %s -> (%dx%d)\n", dssP->name, snsP->streamCache->m, snsP->streamCache->n);
#else
    INFO ("Not implemented\n");
#endif
  }

  if (snsP->streamCache->m == 1 || frameX >= snsP->streamCache->m) {
    sP = snsP->streamCache->matPA[0];

    /* Clear it, if necessary */
    if (snsP->streamCacheX != frameX)
      for (idx = 0; idx < dsN; idx++)
	sP[idx] = -FLT_MAX;
    snsP->streamCacheX = frameX;

  } else /* Use the cache */
    sP = snsP->streamCache->matPA[frameX];

  /* only one stream? */
  if (staP->streamN == 1) {
    for (idx = 0; idx < idxN; idx++) {
      int     snX = idxA[idx];
      Senone* snP = snsP->list.itemA + snX;
      int     dsX = snP->classXA[0];
      int     cbX = dsX;
      if (sP[cbX] == -FLT_MAX) {
	staP->strPA[0]->scoreFct (staP->cdA[0], &dsX, sP+cbX, 1, frameX);
	snsP->streamCache->count++;
      }
      scoreA[snX] = snP->stwA[0] * sP[cbX];
    }
    return TCL_OK;
  }

  /* loop over all requested senones, multiple streams */
  for (idx = 0; idx < idxN; idx++) {
    int     snX = idxA[idx];
    Senone* snP = snsP->list.itemA + snX;

    /* score for main stream */
    if (snP->stwA[0] > 0) {
      staP->strPA[0]->scoreFct (staP->cdA[0], snP->classXA+0, scoreA+snX, 1, frameX);
      scoreA[snX] *= snP->stwA[0];
    } else {
      scoreA[snX] = 0;
    }

    /* compute scores for all remaining streams */
    for (stX = 1; stX < snP->streamN; stX++) {
      int dsX = snP->classXA[stX];
      int cX  = staP->cBaseA[stX] - staP->cBaseA[1] + dsX;

      if (snP->stwA[stX] <= 0 || dsX < 0)
	continue;
      
      if (sP[cX] == -FLT_MAX) {
	staP->strPA[stX]->scoreFct (staP->cdA[stX], &dsX, sP+cX, 1, frameX);
	snsP->streamCache->count++;
      }
      scoreA[snX] += snP->stwA[stX] * sP[cX];

    }
    scoreA[snX] *= snsP->scoreScale;
  }

  return TCL_OK;
}

/* ------------------------------------------------------------------------
    ssa_base_str
   ---------------
    optimized implementation for streamed score arrays
   ------------------------------------------------------------------------ */

int ssa_base_str (SenoneSet* snsP, int* idxA, float* scoreA, 
		  int  idxN, int frameX)
{
  StreamArray*         staP = &(snsP->strA);
  int                   dsN = staP->cBaseA[staP->streamN] - staP->cBaseA[1];
  int              idx, stX;
  float*                 sP;

  /* Create cache if it doesn't exist */
  if (!snsP->streamCache) {
    snsP->streamCache  = fmatrixCreate (1, dsN);
    snsP->streamCacheX = -1;
  }

  if (snsP->streamCache->m == 1 || frameX >= snsP->streamCache->m) {
    sP = snsP->streamCache->matPA[0];

    /* Clear it, if necessary */
    if (snsP->streamCacheX != frameX)
      for (idx = 0; idx < dsN; idx++)
	sP[idx] = -FLT_MAX;
    snsP->streamCacheX = frameX;

  } else /* Use the cache */
    sP = snsP->streamCache->matPA[frameX];

  /* get main stream */
  ssa_base_semCont (snsP, idxA, scoreA, idxN, frameX);

  /* loop over all requested senones */
  for (idx = 0; idx < idxN; idx++) {
    int     snX = idxA[idx];
    Senone* snP = snsP->list.itemA + snX;

    /* score for main stream */
    if (snP->stwA[0] > 0) {
      scoreA[snX] *= snP->stwA[0]/snsP->scoreScale;
    } else {
      scoreA[snX] = 0;
    }

    /* compute scores for all remaining streams */
    for (stX = 1; stX < snP->streamN; stX++) {
      int dsX = snP->classXA[stX];
      int cbX = staP->cBaseA[stX] - staP->cBaseA[1] + dsX;

      if (snP->stwA[stX] <= 0 || dsX < 0)
	continue;
      
      if (sP[cbX] == -FLT_MAX) {
	staP->strPA[stX]->scoreFct (staP->cdA[stX], &dsX, sP+cbX, 1, frameX);
	snsP->streamCache->count++;
      }
      scoreA[snX] += snP->stwA[stX] * sP[cbX];

    }
    scoreA[snX] *= snsP->scoreScale;
  }

  return TCL_OK;
}

/* ------------------------------------------------------------------------
   ssa_compress : direct score computation for compressed codebooks
   ------------------------------------------------------------------------ */

int ssa_compress( SenoneSet* snsP, int* idxA, float* scoreA, 
		  int  idxN, int frameX)
{
  int       streamX  = 0;
  StreamArray* staP  = &(snsP->strA); 
  DistribSet*  dssP  = ((DistribStream*) staP->cdA[streamX])->dssP;
  CodebookSet* cbsP  = dssP->cbsP;
  Senone*       snP  = snsP->list.itemA;
  Distrib*      dsP  = NULL;
  Codebook*     cbP  = cbsP->list.itemA + 0;
  UCHAR*    pattern  = NULL;
  float           r  = cbsP->scaleRV * cbsP->scaleRV * cbsP->scaleCV; 
  float           s  = 1.0 / r;

  /*assume that all codebooks have the same dimN as the first one*/
  int          refN  = cbP->refN;
  int          dimN  = cbP->dimN;
    
  /*fast access*/
  int   minDistIdx, dimX, refX;
  float minDistSum;
 
  /* indices for senone, distrib, codebook*/
  int idx, snX, dsX, cbX;
  
  /* get pattern (same feature space for all codebooks) */
  idx = 0;
  snX = idxA[idx];
  dsX = snP[snX].classXA[streamX];
  dsP = dssP->list.itemA + dsX;
  cbX = dsP->cbX;
  cbP = cbsP->list.itemA + cbX;
  pattern  = (cbsP->feat->featA+cbP->featY)->data.cmat->matPA[frameX];

  /* ------------------------------------------------------------------ */
  /* compute scores for all necessary distributions indexed in idxA     */
  /* ------------------------------------------------------------------ */
 
  for ( idx   = 0; idx < idxN; idx++) {
    
    /*find corresponding codebook from senone index idxA[i] */
    snX = idxA[idx];
    
    dsX  = snP[snX].classXA[streamX];
    dsP  = dssP->list.itemA + dsX;
    cbX  = dsP->cbX;
    cbP  = cbsP->list.itemA + cbX;
    refN = cbP->refN;

    if ((!cbP->rvC || !cbP->cvC) && !_WarnedNoCodeBooks) {
      ERROR ("ssa_compress: codebook '%s' not loaded => bogus results.\n", cbP->name);
      _WarnedNoCodeBooks = 1;
      continue;
    }

    /* ------------------------------------------------------------------*/
    /* Compute all the Mahalanobis distances for the subset of Gaussians */  
    /* ------------------------------------------------------------------*/

    minDistSum = 1E20;
    minDistIdx = 0;  

    if (cbsP->comMode == 1) {
      for (refX=0; refX<refN; refX++) {     
	UCHAR   *pt      =  pattern;
	UCHAR   *rv      =  cbP->rvC[refX];
	UCHAR   *cv      =  cbP->cvC[refX];
	register float   distSum  =  r* (cbP->pi + cbP->detC[refX]);
	
	for ( dimX = 0; dimX < dimN; dimX+=4) {
	  register float diff0;
	  diff0    = (float) rv[dimX] - (float) pt[dimX];
	  distSum += diff0*diff0*cv[dimX];
	  diff0    = (float) rv[dimX+1] - (float) pt[dimX+1];
	  distSum += diff0*diff0*cv[dimX+1];
	  diff0    = (float) rv[dimX+2] - (float) pt[dimX+2];
	  distSum += diff0*diff0*cv[dimX+2];
	  diff0    = (float) rv[dimX+3] - (float) pt[dimX+3];
	  distSum += diff0*diff0*cv[dimX+3];
	  if (distSum > minDistSum) break;
	}
	if (distSum < minDistSum) {
	  minDistSum = distSum;
	  minDistIdx = refX;
	}
      }
    } else {
      for (refX=0; refX<refN; refX++) {     
	UCHAR   *pt      =  pattern;
	UCHAR   *rv      =  cbP->rvC[refX];
	UCHAR   *cv      =  cbP->cvC[refX];
	register float   distSum  =  r* (cbP->pi + cbP->detC[refX]);
	
	for ( dimX = 0; dimX < dimN; dimX+=4) {
	  register float diff0, diff1;
	  diff0    = (float) rv[dimX] - (float) pt[dimX];
	  diff1    = diff0*diff0;
	  diff0    = (float) rv[dimX+1] - (float) pt[dimX+1];
	  diff1   += diff0*diff0;
	  diff0    = (float) rv[dimX+2] - (float) pt[dimX+2];
	  diff1   += diff0*diff0;
	  diff0    = (float) rv[dimX+3] - (float) pt[dimX+3];
	  diff1   += diff0*diff0;
	  distSum += diff1*(*cv++);
	  if (distSum > minDistSum) break;
	}
	if (distSum < minDistSum) {
	  minDistSum = distSum;
	  minDistIdx = refX;
	}
      }
    }

    /*assume fully continuous system*/
    scoreA[snX]=  0.5 * s* (minDistSum + 2 * r * dsP->val[minDistIdx]);
  }

  return TCL_OK;
}

/* ------------------------------------------------------------------------
   ssa_comp_all : compute scores for each component for compressed codebooks
   ------------------------------------------------------------------------ */

int ssa_comp_all( DistribSet* dssP, int dsX, int frameX, float* addCountA)
{
  CodebookSet* cbsP  = dssP->cbsP;
  Distrib*      dsP  = dssP->list.itemA + dsX;
  Codebook*     cbP  = cbsP->list.itemA + dsP->cbX;
  UCHAR*       patC  = (cbsP->feat->featA+cbP->featY)->data.cmat->matPA[frameX];
  float           r  = cbsP->scaleRV * cbsP->scaleRV * cbsP->scaleCV; 
  float           s  = 1.0 / r;
  float        best  = 1E20;
  int          refN  = cbP->refN;
  int          dimN  = cbP->dimN;
  int        dimN_4  = dimN / 4;
  int refX,dimX;

  for (refX=0; refX<refN; refX++) {     
    UCHAR   *pt      =  patC;
    UCHAR   *rv      =  cbP->rvC[refX];
    UCHAR   *cv      =  cbP->cvC[refX];
    float   distSum  =  r* (cbP->pi + cbP->detC[refX]);
    register int dimY= 0;
      
    for ( dimX = 0; dimX < dimN_4; dimX++) {
      register float diff0 = (float) rv[dimY] - (float) pt[dimY++];
      register float diff1 = (float) rv[dimY] - (float) pt[dimY++];
      register float diff2 = (float) rv[dimY] - (float) pt[dimY++];
      register float diff3 = (float) rv[dimY] - (float) pt[dimY++];
      if (cbsP->comMode == 1) {
	distSum += 
	  diff0 * diff0 * (float) cv[0] + diff1 * diff1 * (float) cv[1] +
	  diff2 * diff2 * (float) cv[2] + diff3 * diff3 * (float) cv[3];
	cv=cv+4;
      } else {
	distSum += (float) cv[0] * (diff0 * diff0 + diff1 * diff1 +
				    diff2 * diff2 + diff3 * diff3);
	cv++;
      }
    }
    addCountA[refX] =  0.5 * s* (distSum + 2 * r * dsP->val[refX]);
    if (best > addCountA[refX])
      best = addCountA[refX];
  }
  for (refX=0; refX<refN; refX++) {
    addCountA[refX] = exp(best - addCountA[refX]);
  }
  return TCL_OK;
}
