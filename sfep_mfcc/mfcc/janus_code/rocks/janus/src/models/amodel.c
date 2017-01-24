/* ========================================================================
    JANUS      Speech- to Speech Translation
               ------------------------------------------------------------
               Object: Acoustic Model
               ------------------------------------------------------------

    Author  :  Michael Finke & Ivica Rogina
    Module  :  amodel.c
    Date    :  $Id: amodel.c 3065 2010-01-20 00:31:48Z metze $
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
    Revision 3.10  2003/08/14 11:20:12  fuegen
    Merged changes on branch jtk-01-01-15-fms (jaguar -> ibis-013)

    Revision 3.9.20.3  2002/08/27 08:45:18  metze
    itfParseArgv now uses <name> and NULL initialization for char*

    Revision 3.9.20.2  2002/06/26 11:57:55  fuegen
    changes for new directory structure support and code cleaning

    Revision 3.9.20.1  2001/07/31 10:24:54  soltau
    changed default value for amodelCache to 0

    Revision 3.9  2000/11/14 12:01:09  janus
    Merged branch jtk-00-10-27-jmcd.

    Revision 3.8.24.2  2000/11/08 17:03:53  janus
    Finished making changes required for compilation under 'gcc -Wall'.

    Revision 3.8.24.1  2000/11/06 10:50:23  janus
    Made changes to enable compilation under -Wall.

    Revision 3.8  2000/09/20 22:55:39  janus
    Merging branch jtk-00-09-15-hyu.

    Revision 3.7.2.1  2000/09/20 20:32:39  hyu
    *** empty log message ***

    Revision 3.7  2000/09/12 15:06:10  janus
    Merged branch jtk-00-09-08-hyu.

    Revision 3.6.16.1  2000/09/12 01:13:47  hyu
    Fixed amodelEvalInit to support all topologies.

    Revision 3.6  2000/03/08 08:27:29  soltau
    BigJump Topo's

    Revision 3.5  2000/02/04 09:46:35  soltau
    -use contextcache as default
     configure cache automatically
    -scale function for transition penalties

    Revision 3.4  1999/08/11 15:51:10  soltau
    hardcoded eval function
    context cache

    Revision 3.3  1997/07/21 16:07:03  tschaaf
    gcc / DFKI Clean

    Revision 3.2  1997/06/17 17:41:43  westphal
    *** empty log message ***

    Revision 2.3  96/03/11  00:01:09  finkem
    removed old evals
    
    Revision 2.2  1996/02/17  19:25:21  finkem
    added duration models and general upt to 6 state eval function

    Revision 2.1  1996/02/13  04:51:49  rogina
    added duration model index

    Revision 2.0  1996/01/31  05:06:31  finkem
    new senoneSet setup

    Revision 1.18  1995/11/17  22:23:37  finkem
    moved ptree eavesdrpping to hmm.c

    Revision 1.17  1995/10/31  02:21:07  finkem
    removed possible problems with hashKey on HPs

    Revision 1.16  1995/10/30  00:22:38  finkem
    Introduced PTree eavesdropper

    Revision 1.15  1995/10/13  22:40:24  finkem
    amodel Phones/Tags Pointer function added

    Revision 1.14  1995/09/06  07:11:25  kemp
    *** empty log message ***

    Revision 1.13  1995/08/27  22:35:11  finkem
    *** empty log message ***

    Revision 1.12  1995/08/18  08:23:50  finkem
    *** empty log message ***

    Revision 1.11  1995/08/17  17:09:41  rogina
    *** empty log message ***

    Revision 1.10  1995/08/12  10:21:11  finkem
    *** empty log message ***

    Revision 1.9  1995/08/07  13:55:56  rogina
    *** empty log message ***

    Revision 1.8  1995/08/04  13:13:13  finkem
    *** empty log message ***

    Revision 1.7  1995/08/03  14:43:00  finkem
    *** empty log message ***

    Revision 1.6  1995/08/01  15:42:41  rogina
    *** empty log message ***

    Revision 1.5  1995/08/01  13:28:05  finkem
    *** empty log message ***

    Revision 1.4  1995/07/31  14:34:50  finkem
    *** empty log message ***

    Revision 1.3  1995/07/25  22:33:21  finkem
    *** empty log message ***

    Revision 1.2  1995/07/25  14:45:19  rogina
    

    Revision 1.1  1995/07/25  13:49:57  finkem
    Initial revision

 
   ======================================================================== */

#include "amodel.h"
#include "array.h"
#include "distribStream.h"
#ifndef IBIS
#include "hmeStream.h"
#endif

#include "senones.h"
#include "topo.h"
#include "trans.h"

char amodelRCSVersion[]= 
           "@(#)1$Id: amodel.c 3065 2010-01-20 00:31:48Z metze $";

typedef short WordX;
typedef short DurCnt;

struct BPtr_S {
  short  frameX;
  WordX  bpX;
  DurCnt cnt;
};

#define TRANS_MAX 4 /* max. nr of transitons for 3state topo's */

/* ------------------------------------------------------------------------
    Forward Declarations
   ------------------------------------------------------------------------ */

typedef struct
{
  int  stateN;
  int* senone;
  int* trans;

  int  topoX;
  int  durX;

} HashKey;

extern TypeInfo amodelInfo;

/* ========================================================================
    Acoustic Model
   ======================================================================== */
/* ------------------------------------------------------------------------
    amodelInit
   ------------------------------------------------------------------------ */

static int amodelInit (AModel *amodel, HashKey *key)
{
  amodel->stateN = key->stateN;
  amodel->senone = key->senone;
  amodel->trans  = key->trans;

  amodel->tprobA = NULL;

  amodel->fwdTrN = NULL;
  amodel->fwdIdx = NULL;
  amodel->fwdPen = NULL;
  amodel->fwdPen_org = NULL;

  amodel->evalFn = NULL;
  amodel->topoX  = -1;
  amodel->durX   = -1;
  return TCL_OK;
}

/* ------------------------------------------------------------------------
    amodelDeinit
   ------------------------------------------------------------------------ */

static int amodelDeinit (AModel *amodel)
{
  int sX;

  if (amodel->senone) { free (amodel->senone); amodel->senone = NULL; }
  if (amodel->trans)  { free (amodel->trans);  amodel->trans  = NULL; }
  if (amodel->tprobA) { free (amodel->tprobA); amodel->tprobA = NULL; }

  for (sX=0; sX<amodel->stateN; sX++) {
    if (amodel->fwdIdx[sX]) free (amodel->fwdIdx[sX]); 
    if (amodel->fwdPen[sX]) free (amodel->fwdPen[sX]);
    if (amodel->fwdPen_org[sX]) free (amodel->fwdPen_org[sX]);
  }
  if (amodel->fwdTrN) { free (amodel->fwdTrN); amodel->fwdTrN = NULL; }
  if (amodel->fwdIdx) { free (amodel->fwdIdx); amodel->fwdIdx = NULL; }
  if (amodel->fwdPen) { free (amodel->fwdPen); amodel->fwdPen = NULL; }
  if (amodel->fwdPen_org) { free (amodel->fwdPen_org); amodel->fwdPen_org = NULL; }

  amodel->stateN =  0;
  amodel->evalFn =  NULL;
  amodel->topoX  = -1;
  amodel->durX   = -1;

  return TCL_OK;
}

/* ------------------------------------------------------------------------
    amodelHashKey & amodelHashCmp
   ------------------------------------------------------------------------ */

static long amodelHashKey( const HashKey *key)
{
  unsigned long ret = 0; 
  int           stateX;

  for (stateX=0; stateX<key->stateN; stateX++) {
    ret = ((ret<<5) | (ret >> (8*sizeof(long)-5))) ^ key->senone[stateX];
    ret = ((ret<<5) | (ret >> (8*sizeof(long)-5))) ^ key->trans[stateX];
  }
  return ret;
}

static int amodelHashCmp( const HashKey *key, const AModel *amodel)
{
  int stateX;

  if (amodel->senone[0] != key->senone[0]) return 0;
  if (amodel->stateN    != key->stateN   ) return 0;

  for (stateX=1; stateX<key->stateN; stateX++)
    if (amodel->senone[stateX] != key->senone[stateX]) return 0;
  for (stateX=0; stateX<key->stateN; stateX++)
    if (amodel->trans[stateX] != key->trans[stateX]) return 0;

  if ( key->topoX != amodel->topoX) return 0;
  if ( key->durX  != amodel->durX)  return 0;
  return 1;
}

/* ------------------------------------------------------------------------
    amodelPutsItf
   ------------------------------------------------------------------------ */

static int amodelPutsItf (ClientData clientData, int argc, char *argv[])
{
  AModel* amP = (AModel*)clientData;
  int     stateX;

  argc--;
 
  if ( itfParseArgv( argv[0], &argc, argv+1, 0,  NULL) != TCL_OK) {
         return TCL_ERROR;
  }

  itfAppendResult("%d {",amP->stateN);
  for (stateX=0; stateX< amP->stateN; stateX++)
    itfAppendElement("%d",amP->senone[stateX]);
  itfAppendResult(" } {");
  for (stateX=0; stateX<amP->stateN; stateX++)
    itfAppendElement("%d",amP->trans[stateX]);
  itfAppendResult(" }");

  return TCL_OK;    
}

static int amodelConfigureItf( ClientData cd, char *var, char *val)
{
  AModel *amP = (AModel*)cd;
  if (!amP) return TCL_ERROR;

  if (! var) {
    ITFCFG(amodelConfigureItf,cd,"topoX");
    ITFCFG(amodelConfigureItf,cd,"durX");
    return TCL_OK;
  }
  ITFCFG_Int(    var,val,"topoX",     amP->topoX, 0);
  ITFCFG_Int(    var,val,"durX",      amP->durX,  0);
  return TCL_ERROR;
}

/* ========================================================================
    Acoustic Model Evaluation
   ======================================================================== */

#define WORSTSCORE 9999999.0
#define FINALSTATE 6

/* ------------------------------------------------------------------------
    EvaluateStateMpxTrellis
   ------------------------------------------------------------------------ */

float evalStateMpxTrellis(  AModel* amlP, int* modelXA, 
                            float*  distScoreA, float*  scoreA, BPtr* pathA,
                            DurationSet* durP)
{
  float   best   = WORSTSCORE;
  int     N      = amlP[modelXA[0]].stateN;
  float   s[FINALSTATE+1];
  AModel* m[FINALSTATE+1];
  int     model=0, i;

  for ( i = 0; i < N; i++) { 
    m[i]      = amlP      + modelXA[i];
    s[i]      = scoreA[i] + distScoreA[m[i]->senone[i]];
    scoreA[i] = WORSTSCORE;
  }
  scoreA[FINALSTATE] = WORSTSCORE;

  for ( i = N-1; i >= 0; i--) { 
    int   trN = m[i]->fwdTrN[i], trX;
    for ( trX = 0; trX < trN; trX++) {
      int   idx = m[i]->fwdIdx[i][trX] + i;
      float pen = m[i]->fwdPen[i][trX] + s[i];  
      if(  idx >= N) idx = FINALSTATE;
 
      if ( scoreA[idx] > pen) {
        scoreA[idx]  = pen;
        if ( i != idx) {
          if (idx < N) modelXA[idx] = modelXA[i];
          else         model        = modelXA[i];
          pathA[idx]   = pathA[i];
        }
      }
    }
  }
  if ( durP && scoreA[FINALSTATE] < WORSTSCORE && amlP[model].durX >= 0) {
       scoreA[FINALSTATE] += durScore( durP, amlP[model].durX, 
                                       pathA[FINALSTATE].cnt);
  }
  best = scoreA[FINALSTATE];
  for ( i = 0; i < N; i++) {
    pathA[i].cnt++;
    if ( best > scoreA[i]) best = scoreA[i];
  }
  return best;
}

/* ------------------------------------------------------------------------
    EvaluateStateTrellis
   ------------------------------------------------------------------------ */

float evalStateTrellis( AModel* amP,    float* distScoreA,
                        float*  scoreA, BPtr*  pathA, DurationSet* durP)
{
  int*   distXA = amP->senone;
  int    N      = amP->stateN;
  float  s[FINALSTATE+1];
  float  best;
  int    i;

  for ( i = 0; i < N; i++) { 
    s[i]      = scoreA[i] + distScoreA[distXA[i]];
    scoreA[i] = WORSTSCORE;
  }
  scoreA[FINALSTATE] = WORSTSCORE;

  for ( i = N-1; i >= 0; i--) { 
    int   trN = amP->fwdTrN[i], trX;
    for ( trX = 0; trX < trN; trX++) {
      int   idx = amP->fwdIdx[i][trX] + i;
      float pen = amP->fwdPen[i][trX] + s[i];  
      if(  idx >= N) idx = FINALSTATE;
 
      if ( scoreA[idx] > pen) {
        scoreA[idx] = pen;
        if ( i != idx) pathA[idx] = pathA[i];
      }
    }
  }
  if ( durP && scoreA[FINALSTATE] < WORSTSCORE && amP->durX  >= 0) {
       scoreA[FINALSTATE] += durScore( durP, amP->durX,pathA[FINALSTATE].cnt);
  }
  best = scoreA[FINALSTATE];
  for ( i = 0; i < N; i++) {
    pathA[i].cnt++;
    if ( best > scoreA[i]) best = scoreA[i];
  }
  return best;
}

/* ------------------------------------------------------------------------
   Special eval functions from rev. 2.2
   ------------------------------------------------------------------------ */

/* ------------------------------------------------------------------------
    Evaluate3StateMpxTrellis
   ------------------------------------------------------------------------ */

float eval3StateMpxTrellis( AModel* amlP, int* modelXA, 
                            float*  distScoreA, float*  scoreA, BPtr* pathA,
                            DurationSet* durP)
{
  AModel *m2,*m1,*m0;
  float   s6,s2,s1,s0, t0,t1;
  float   best;

  m2 = amlP + modelXA[2];
  m1 = amlP + modelXA[1];
  m0 = amlP + modelXA[0];

  s2 = scoreA[2] + distScoreA[m2->senone[2]];
  s1 = scoreA[1] + distScoreA[m1->senone[1]];
  s0 = scoreA[0] + distScoreA[m0->senone[0]];

  /* tprobA[9] = 2th trans. from last state : MAX_TRANS * stateX + transX */
  best = s6 = s2 + m2->tprobA[9]; pathA[6] = pathA[2];

  t0 = s2 + m2->tprobA[8]; /* self loop into last state */
  t1 = s1 + m1->tprobA[5]; /* jump from s1 to s2 */

  if ( t0 < t1) { s2 = t0; }
  else          { s2 = t1; pathA[2] = pathA[1]; modelXA[2] = modelXA[1]; }
  if ( s2 < best) best = s2;

  t0 = s1 + m1->tprobA[4]; /* self loop into middle state */
  t1 = s0 + m0->tprobA[1]; /* jump from s0 to s1 */

  if ( t0 < t1) { s1 = t0; }
  else          { s1 = t1; pathA[1] = pathA[0]; modelXA[1] = modelXA[0]; }
  if ( s1 < best) best = s1;

  s0 += m0->tprobA[0];    /* self loop into first state */
  if ( s0 < best) best = s0;

  scoreA[6] = s6;  scoreA[2] = s2;  scoreA[1] = s1; scoreA[0] = s0;
  return best;
}

/* ------------------------------------------------------------------------
    Evaluate3StateTrellis
   ------------------------------------------------------------------------ */

float eval3StateTrellis( AModel* amP,    float* distScoreA,
                         float*  scoreA, BPtr*  pathA, DurationSet* durP)
{
  int*   distXA = amP->senone;
  float* tprobA = amP->tprobA;
  float  best;
  float  s6,s2,s1,s0, t0,t1;

  s2 = scoreA[2] + distScoreA[distXA[2]];
  s1 = scoreA[1] + distScoreA[distXA[1]];
  s0 = scoreA[0] + distScoreA[distXA[0]];

  best = s6 = s2 + tprobA[9]; pathA[6] = pathA[2];

  t0 = s2 + tprobA[8];
  t1 = s1 + tprobA[5];

  if ( t0 < t1) { s2 = t0; }
  else          { s2 = t1; pathA[2] = pathA[1]; }
  if ( s2 < best) best = s2;

  t0 = s1 + tprobA[4];
  t1 = s0 + tprobA[1];

  if ( t0 < t1) { s1 = t0; }
  else          { s1 = t1; pathA[1] = pathA[0]; }
  if ( s1 < best) best = s1;

  s0 += tprobA[0];    /* state 0 */
  if ( s0 < best) best = s0;

  scoreA[6] = s6; scoreA[2] = s2; scoreA[1] = s1; scoreA[0] = s0;
  return best;
}

/* ------------------------------------------------------------------------
    Evaluate3StateMpxTrellis II
   ------------------------------------------------------------------------ */

float eval3StateMpxTrellis2( AModel* amlP, int* modelXA, 
                            float*  distScoreA, float*  scoreA, BPtr* pathA,
                            DurationSet* durP)
{
  AModel *m2,*m1,*m0;
  float   s6,s2,s1,s0, t0,t1,t2;
  float   best;  

  m2 = amlP + modelXA[2];
  m1 = amlP + modelXA[1];
  m0 = amlP + modelXA[0];

  s2 = scoreA[2] + distScoreA[m2->senone[2]];
  s1 = scoreA[1] + distScoreA[m1->senone[1]];
  s0 = scoreA[0] + distScoreA[m0->senone[0]];

  t1 = s2 + m2->tprobA[9];  /*simple loop from s2 to final*/
  t2 = s1 + m1->tprobA[6];  /*double loop from s1 to final*/
  if (t1 < t2) {
    best = s6 = t1; pathA[6] = pathA[2]; 
  } else {
    best = s6 = t2; pathA[6] = pathA[1]; 
  }

  t0 = s2 + m2->tprobA[8];
  t1 = s1 + m1->tprobA[5];
  t2 = s0 + m0->tprobA[2];

  if ( t0 < t2) {
    if (t0 < t1) {
      { s2 = t0; }                                                 /*go from s2*/
    } else 
      { s2 = t1; pathA[2] = pathA[1];  modelXA[2] = modelXA[1];}   /*go from s1*/
  } else {
    if (t2 < t1) {
      { s2 = t0; pathA[2] = pathA[0];  modelXA[2] = modelXA[0];}   /*go from s0*/
    } else
      { s2 = t1; pathA[2] = pathA[1];  modelXA[2] = modelXA[1];}   /*go from s1*/
  }
  if ( s2 < best) best = s2;

  t0 = s1 + m1->tprobA[4];
  t1 = s0 + m0->tprobA[1];

  if ( t0 < t1) { s1 = t0; }
  else          { s1 = t1; pathA[1] = pathA[0]; modelXA[1] = modelXA[0]; }
  if ( s1 < best) best = s1;

  s0 += m0->tprobA[0];    /* state 0 */
  if ( s0 < best) best = s0;

  scoreA[6] = s6;  scoreA[2] = s2;  scoreA[1] = s1; scoreA[0] = s0;
  return best;
}

/* ------------------------------------------------------------------------
    Evaluate3StateTrellis II
   ------------------------------------------------------------------------ */

float eval3StateTrellis2( AModel* amP,    float* distScoreA,
                         float*  scoreA, BPtr*  pathA, DurationSet* durP)
{
  int*   distXA = amP->senone;
  float* tprobA = amP->tprobA;
  float  best;
  float  s6,s2,s1,s0, t0,t1,t2;

  s2 = scoreA[2] + distScoreA[distXA[2]];
  s1 = scoreA[1] + distScoreA[distXA[1]];
  s0 = scoreA[0] + distScoreA[distXA[0]];

  t1 = s2 + tprobA[9];  /*simple loop from s2 to final*/
  t2 = s1 + tprobA[6];  /*double loop from s1 to final*/
  if (t1 < t2) {
    best = s6 = t1; pathA[6] = pathA[2];
  } else {
    best = s6 = t2; pathA[6] = pathA[1];
  }

  t0 = s2 + tprobA[8];  /*self   loop from s2 to s2*/
  t1 = s1 + tprobA[5];  /*simple loop from s1 to s2*/
  t2 = s0 + tprobA[2];  /*double loop from s0 to s2*/

  if ( t0 < t2) {
    if (t0 < t1) {
      { s2 = t0; }                        /*go from s2*/
    } else 
      { s2 = t1; pathA[2] = pathA[1]; }   /*go from s1*/
  } else {
    if (t2 < t1) {
      { s2 = t0; pathA[2] = pathA[0]; }   /*go from s0*/
    } else
      { s2 = t1; pathA[2] = pathA[1]; }   /*go from s1*/
  }

  if ( s2 < best) best = s2;

  t0 = s1 + tprobA[4];
  t1 = s0 + tprobA[1];

  if ( t0 < t1) { s1 = t0; }
  else          { s1 = t1; pathA[1] = pathA[0]; }
  if ( s1 < best) best = s1;

  s0 += tprobA[0];    /* state 0 */
  if ( s0 < best) best = s0;

  scoreA[6] = s6; scoreA[2] = s2; scoreA[1] = s1; scoreA[0] = s0;
  return best;
}

/* ------------------------------------------------------------------------
    Evaluate3StateMpxTrellis III
   ------------------------------------------------------------------------ */

float eval3StateMpxTrellis3( AModel* amlP, int* modelXA, 
                            float*  distScoreA, float*  scoreA, BPtr* pathA,
                            DurationSet* durP)
{
  AModel *m2,*m1,*m0;
  float   s6,s2,s1,s0, t0,t1,t2;
  float   best;  

  m2 = amlP + modelXA[2];
  m1 = amlP + modelXA[1];
  m0 = amlP + modelXA[0];

  s2 = scoreA[2] + distScoreA[m2->senone[2]];
  s1 = scoreA[1] + distScoreA[m1->senone[1]];
  s0 = scoreA[0] + distScoreA[m0->senone[0]];

  t0 = s2 + m2->tprobA[9];  /*simple loop from s2 to final*/
  t1 = s1 + m1->tprobA[6];  /*double loop from s1 to final*/
  t2 = s0 + m0->tprobA[3];  /*big jump from s0 to final*/
  
  if ( t0 < t2) {
    if (t0 < t1) {
      best = s6 = t0; pathA[6] = pathA[2];                          /* t0 has minimum (from state 2) */
    } else {
      best = s6 = t1; pathA[6] = pathA[1]; modelXA[2] = modelXA[1]; /* t1 has minimum (from state 1) */
    }
  } else {
    if (t2 < t1) {
      best = s6 = t2; pathA[6] = pathA[0]; modelXA[2] = modelXA[9]; /* t2 has minimum (from state 0) */
    } else {
      best = s6 = t1; pathA[6] = pathA[1]; modelXA[2] = modelXA[1]; /* t2 has minimum (from state 1) */
    }
  }

  t0 = s2 + m2->tprobA[8];
  t1 = s1 + m1->tprobA[5];
  t2 = s0 + m0->tprobA[2];

  if ( t0 < t2) {
    if (t0 < t1) {
      { s2 = t0; }                                                 /*go from s2*/
    } else 
      { s2 = t1; pathA[2] = pathA[1];  modelXA[2] = modelXA[1];}   /*go from s1*/
  } else {
    if (t2 < t1) {
      { s2 = t0; pathA[2] = pathA[0];  modelXA[2] = modelXA[0];}   /*go from s0*/
    } else
      { s2 = t1; pathA[2] = pathA[1];  modelXA[2] = modelXA[1];}   /*go from s1*/
  }
  if ( s2 < best) best = s2;

  t0 = s1 + m1->tprobA[4];
  t1 = s0 + m0->tprobA[1];

  if ( t0 < t1) { s1 = t0; }
  else          { s1 = t1; pathA[1] = pathA[0]; modelXA[1] = modelXA[0]; }
  if ( s1 < best) best = s1;

  s0 += m0->tprobA[0];    /* state 0 */
  if ( s0 < best) best = s0;

  scoreA[6] = s6;  scoreA[2] = s2;  scoreA[1] = s1; scoreA[0] = s0;
  return best;
}

/* ------------------------------------------------------------------------
    Evaluate3StateTrellis III
   ------------------------------------------------------------------------ */

float eval3StateTrellis3( AModel* amP,    float* distScoreA,
                         float*  scoreA, BPtr*  pathA, DurationSet* durP)
{
  int*   distXA = amP->senone;
  float* tprobA = amP->tprobA;
  float  best;
  float  s6,s2,s1,s0, t0,t1,t2;

  s2 = scoreA[2] + distScoreA[distXA[2]];
  s1 = scoreA[1] + distScoreA[distXA[1]];
  s0 = scoreA[0] + distScoreA[distXA[0]];
  t0 = s2 + tprobA[9];  /*simple loop from s2 to final*/
  t1 = s1 + tprobA[6];  /*double loop from s1 to final*/
  t2 = s0 + tprobA[3];  /*big jump from s0 to final*/
  
  if ( t0 < t2) {
    if (t0 < t1) {
      best = s6 = t0; pathA[6] = pathA[2]; /* t0 has minimum (from state 2) */
    } else {
      best = s6 = t1; pathA[6] = pathA[1]; /* t1 has minimum (from state 1) */
    }
  } else {
    if (t2 < t1) {
      best = s6 = t2; pathA[6] = pathA[0]; /* t2 has minimum (from state 0) */
    } else {
      best = s6 = t1; pathA[6] = pathA[1]; /* t2 has minimum (from state 1) */
    }
  }

  t0 = s2 + tprobA[8];
  t1 = s1 + tprobA[5];
  t2 = s0 + tprobA[2];

  if ( t0 < t2) {
    if (t0 < t1) {
      { s2 = t0; }                        /*go from s2*/
    } else 
      { s2 = t1; pathA[2] = pathA[1]; }   /*go from s1*/
  } else {
    if (t2 < t1) {
      { s2 = t0; pathA[2] = pathA[0]; }   /*go from s0*/
    } else
      { s2 = t1; pathA[2] = pathA[1]; }   /*go from s1*/
  }
  if ( s2 < best) best = s2;
  
  t0 = s1 + tprobA[4];
  t1 = s0 + tprobA[1];

  if ( t0 < t1) { s1 = t0; }
  else          { s1 = t1; pathA[1] = pathA[0]; }
  if ( s1 < best) best = s1;

  s0 += tprobA[0];    /* state 0 */
  if ( s0 < best) best = s0;

  scoreA[6] = s6;  scoreA[2] = s2;  scoreA[1] = s1; scoreA[0] = s0;
  return best;
}

/* ------------------------------------------------------------------------
    Evaluate Function Table
   ------------------------------------------------------------------------ */

EvalFunc evalFuncA[] =
{
 { evalStateTrellis,   evalStateMpxTrellis},
 { eval3StateTrellis,  eval3StateMpxTrellis},
 { eval3StateTrellis2, eval3StateMpxTrellis2},
 { eval3StateTrellis3, eval3StateMpxTrellis3}
};

/* ------------------------------------------------------------------------
    Assign Evaluation Function
   ------------------------------------------------------------------------ */

static int amodelEvalInit( AModelSet* amsP, int idx)
{
  AModel* amP    = &(amsP->list.itemA[idx]);
  TmSet*  tmsP   =   amsP->tmsP;
  int*    tmXA   =   amP->trans;
  int     sX, tX;
 
  if ( amP->stateN == 3 && \
       tmsP->list.itemA[tmXA[0]].trN > 1 &&  \
       tmsP->list.itemA[tmXA[0]].trN < 5) {

    int    tr0    = tmsP->list.itemA[tmXA[0]].trN;
    float* tprobA = (float*)malloc((TRANS_MAX*amP->stateN) * sizeof(float));

    assert (tprobA != NULL);
    amsP->list.itemA[idx].tprobA =   tprobA;

    for (tX=0;tX<TRANS_MAX*amP->stateN;tX++)
       tprobA[tX]=0;

    switch (tr0) {
    case 2:  {amsP->list.itemA[idx].evalFn = &(evalFuncA[1]); break;}
    case 3:  {amsP->list.itemA[idx].evalFn = &(evalFuncA[2]); break;}
    case 4:  {amsP->list.itemA[idx].evalFn = &(evalFuncA[3]); break;}
    default: {amsP->list.itemA[idx].evalFn = NULL; break;}
    }

    for ( sX = 0; sX < amP->stateN; sX++) {
      float* pen = tmsP->list.itemA[tmXA[sX]].penA;
      int    trN = tmsP->list.itemA[tmXA[sX]].trN;

      assert(trN<=TRANS_MAX);
      for(tX = 0; tX < trN; tX++) {
	tprobA[(TRANS_MAX*sX) + tX] = pen[tX];
      }
    }
  } else { 
    if ( amP->stateN < 7) amsP->list.itemA[idx].evalFn = &(evalFuncA[0]);
    else                  amsP->list.itemA[idx].evalFn =   NULL;
  }

  /* TODO: For now we use the general eval function for all models since
           only this function has the cnt increment in it necessary for
           duration modelling 
	   */

  amP->fwdTrN  =    (int*)malloc(amP->stateN*sizeof(int));
  amP->fwdIdx  =   (int**)malloc(amP->stateN*sizeof(int*));
  amP->fwdPen  = (float**)malloc(amP->stateN*sizeof(float*));
  amP->fwdPen_org  = (float**)malloc(amP->stateN*sizeof(float*));

  for (sX=0; sX<amP->stateN; sX++) {
    amP->fwdTrN [sX] = tmsP->list.itemA[tmXA[sX]].trN;
    amP->fwdIdx [sX] =   (int*)malloc(amP->fwdTrN[sX]*sizeof(int));
    amP->fwdPen [sX] = (float*)malloc(amP->fwdTrN[sX]*sizeof(float));
    amP->fwdPen_org [sX] = (float*)malloc(amP->fwdTrN[sX]*sizeof(float));
     
    for (tX=0; tX<amP->fwdTrN[sX]; tX++) {
      amP->fwdIdx[sX][tX] = tmsP->list.itemA[tmXA[sX]].offA[tX];
      amP->fwdPen[sX][tX] = tmsP->list.itemA[tmXA[sX]].penA[tX];
      amP->fwdPen_org[sX][tX] = amP->fwdPen[sX][tX];
    }
  }
  return TCL_OK;
}

/* ========================================================================
    Set of Acoustic Models
   ======================================================================== */

static ClientData amodelSetCreateItf(    ClientData clientData, 
                                         int argc, char *argv[]);
static int        amodelSetFreeItf(      ClientData clientData);
static int        amodelSetAddItf(       ClientData clientData, 
                                         int argc, char *argv[]);
static int        amodelSetConfigureItf( ClientData cd, char *var, char *val);

static AModelSet  amodelSetDefault;

/* ------------------------------------------------------------------------
    amodelSetCreate
   ------------------------------------------------------------------------ */

int amodelSetInit( AModelSet* amodelSet, char* name, Tree* treeP, int treeX,
                                                     Tree* durP,  int durX,
		                                     int createContextCache)
{
  assert( amodelSet && name && treeP);

  if ( treeP->mdsP.modelSetP->tiP != itfGetType("TopoSet")) {
    ERROR("AModelSet need to be defined over Tree(%s).\n", 
           treeP->mdsP.modelSetP->tiP->name);
    return TCL_ERROR;
  }

  amodelSet->name   = strdup(name);
  amodelSet->useN   = 0;

  amodelSet->treeP  = treeP;
  amodelSet->treeX  = treeX;
  amodelSet->durP   = durP; if (durP) link(durP,"Tree");
  amodelSet->durX   = durX;
  amodelSet->snsP   = topoSetSenoneSet((TopoSet*)(treeP->mdsP.cd));
  amodelSet->tmsP   = topoSetTmSet(    (TopoSet*)(treeP->mdsP.cd));

  link( treeP,           "Tree");
  link( amodelSet->snsP, "SenoneSet");
  link( amodelSet->tmsP, "TmSet");

  listInit((List*)&(amodelSet->list), &amodelInfo, sizeof(AModel), 
                    amodelSetDefault.list.blkSize);

  amodelSet->list.init    = (ListItemInit  *)amodelInit;
  amodelSet->list.deinit  = (ListItemDeinit*)amodelDeinit;
  amodelSet->list.hashKey = (HashKeyFn     *)amodelHashKey;
  amodelSet->list.hashCmp = (HashCmpFn     *)amodelHashCmp;

  /* create context cache */

  if (createContextCache) {
    /* find maximum context size */
    SenoneSet*   snsP  = amodelSet->snsP;
    StreamArray* staP  = &(snsP->strA); 
    int streamX,qsX;
    int maxLeftContext  = -1;
    int maxRightContext = -1;

    for (streamX=0;streamX<staP->streamN;streamX++) {
      TypeInfo*    tiP   = ((Stream*) staP->strPA[streamX])->tiP;
      Tree*        treeP = NULL;
      QuestionSet* qssP  = NULL;

      if ( streq(tiP->name,"DistribStream") )
	treeP= ((DistribStream*) staP->cdA[streamX])->treeP;
#ifndef IBIS
      if ( streq(tiP->name,"HmeStream") )
	treeP= ((HmeStream*) staP->cdA[streamX])->tree;
#endif

      /*cannot handle this stream type */
      if (treeP == NULL) {
	maxLeftContext  = -1;
	maxRightContext = -1;
	break ;
      }

      qssP= & (treeP->questions );
      for (qsX=0;qsX<qssP->list.itemN;qsX++) {
	Question* qsP = qssP->list.itemA + qsX;
	if (qsP->leftContext > maxLeftContext && qsP->leftContext != 99999)
	  maxLeftContext = qsP->leftContext;
	if (qsP->rightContext > maxRightContext && qsP->rightContext != -99999)
	  maxRightContext = qsP->rightContext;
      }
    }
    amodelSet->contextCache = contextCacheCreate(name);

    (amodelSet->contextCache)->maxLeftContext  = maxLeftContext;
    (amodelSet->contextCache)->maxRightContext = maxRightContext;

    INFO("Create contextCache: maxLeft=%d  maxRight= %d\n",\
	 maxLeftContext,maxRightContext);

  } else {
    amodelSet->contextCache = NULL;
  }
  return TCL_OK;
}

AModelSet* amodelSetCreate( char *name, Tree* treeP, int treeX,
                                        Tree* durP,  int durX, 
			                int createContextCache)
{
  AModelSet *amodelSet = (AModelSet*)malloc(sizeof(AModelSet));

  if (! amodelSet || amodelSetInit( amodelSet, name, treeP, treeX,
                                                     durP,  durX,
                                                     createContextCache) != TCL_OK) {
     if ( amodelSet) free( amodelSet);
     ERROR("Failed to allocate AModelSet object '%s'.\n", name);
     return NULL; 
  }
  return amodelSet;
}

static ClientData amodelSetCreateItf (ClientData clientData, int argc, char *argv[])
{
  int          ac    =  argc;
  Tree*        treeP =  NULL;
  char*        treeR =  NULL;
  int          treeX = -1;
  Tree*        durP  =  NULL;
  char*        durS  =  NULL;
  int          durX  = -1;
  int          conF  =  0;
  char        *name  = NULL; 

  if (itfParseArgv (argv[0], &ac, argv, 0,
      "<name>",        ARGV_STRING, NULL, &name,   NULL,  "name of the amodel set",
      "<TTree>",       ARGV_OBJECT, NULL, &treeP, "Tree", "topology tree",
      "<TTreeRoot>",   ARGV_STRING, NULL, &treeR,  NULL,  "root name in TTree",
      "-durationTree", ARGV_OBJECT, NULL, &durP,  "Tree", "duration tree",
      "-durationRoot", ARGV_STRING, NULL, &durS,   NULL,  "duration tree root",
      "-contextCache", ARGV_INT, NULL, &conF,   NULL,  "1 = create context cache",
       NULL) != TCL_OK) return NULL;

  if ((treeX = treeIndex( treeP, treeR)) < 0) {
    ERROR("Root node %s not found in %s.\n", treeR, treeP->name);
    return NULL;
  }
  if ( durP && (! durS || (durX = treeIndex( durP, durS)) < 0)) {
    ERROR("Bad/Missing duration tree %s node.\n", durP->name);
    return NULL;
  }
  return (ClientData) amodelSetCreate (name, treeP, treeX, durP, durX, conF);
}


/* ------------------------------------------------------------------------
    amodelSetReset
   ------------------------------------------------------------------------ */

static int amodelSetReset( AModelSet* amodelSet)
{
  listDeinit((List*)&(amodelSet->list));
  listInit(  (List*)&(amodelSet->list), &amodelInfo, sizeof(AModel), 
                      amodelSetDefault.list.blkSize);

  amodelSet->list.init    = (ListItemInit  *)amodelInit;
  amodelSet->list.deinit  = (ListItemDeinit*)amodelDeinit;
  amodelSet->list.hashKey = (HashKeyFn     *)amodelHashKey;
  amodelSet->list.hashCmp = (HashCmpFn     *)amodelHashCmp;

  return TCL_OK;
}

static int amodelSetResetItf(ClientData cd, int argc, char *argv[])
{
  int         ac    =  argc - 1;

  if (  itfParseArgv( argv[0], &ac, argv+1, 0, NULL) != TCL_OK) 
        return TCL_ERROR;
  else  return amodelSetReset((AModelSet*)cd);
}

/* ------------------------------------------------------------------------
    amodelSetFree
   ------------------------------------------------------------------------ */

int amodelSetLinkN( AModelSet* amodelSet)
{
  int  useN = listLinkN((List*)&(amodelSet->list)) - 1;

  if ( useN < amodelSet->useN) return amodelSet->useN;
  else                         return useN;
}

int amodelSetDeinit( AModelSet* amodelSet)
{
  if (! amodelSet) { ERROR("null argument"); return TCL_ERROR; }

  if ( amodelSetLinkN( amodelSet)) { 
    SERROR("AModelSet '%s' still in use by other objects.\n", 
            amodelSet->name); 
    return TCL_ERROR;
  }
  unlink(   amodelSet->snsP,   "SenoneSet");
  unlink(   amodelSet->tmsP,   "TmSet");
  unlink(   amodelSet->treeP,  "Tree");

  if (amodelSet->durP) unlink(   amodelSet->durP,   "Tree");

  if ( amodelSet->name) { free(amodelSet->name); amodelSet->name = NULL; }

  if (amodelSet->contextCache) {
    if (TCL_OK != contextCacheFree(amodelSet->contextCache)) return TCL_ERROR;
    amodelSet->contextCache = NULL;
  }

  return listDeinit((List*)&(amodelSet->list));
}

int amodelSetFree( AModelSet* amodelSet)
{
  if ( amodelSetDeinit( amodelSet) != TCL_OK) return TCL_ERROR;
  free(amodelSet);
  return TCL_OK;
}

static int amodelSetFreeItf (ClientData clientData)
{
  return amodelSetFree((AModelSet*)clientData);
}

/* ------------------------------------------------------------------------
    amodelSetAdd
   ------------------------------------------------------------------------ */

int amodelSetAdd( AModelSet *amodelSet, int stateN, int *senone, int *trans, 
                                        int topoX,  int durX)
{
  HashKey key;
  int     idx;

  key.stateN = stateN;
  key.senone = (int*)malloc(sizeof(int)*stateN);  /* freed by amodelDeinit */
  key.trans  = (int*)malloc(sizeof(int)*stateN);  /* freed by amodelDeinit */
  key.topoX  =  topoX;
  key.durX   =  durX;

  memcpy(key.senone,senone,sizeof(int)*stateN);
  memcpy(key.trans ,trans ,sizeof(int)*stateN);

  idx = listIndex((List*) &(amodelSet->list), &key, 1);

  assert(idx>=0);

  if (amodelSet->list.itemA[idx].senone != key.senone)
    {
      free (key.senone);
      free (key.trans);
    }
  else {
    amodelSet->list.itemA[idx].topoX = topoX;
    amodelSet->list.itemA[idx].durX  = durX;
    amodelEvalInit( amodelSet, idx);
    if (! amodelSet->list.itemA[idx].evalFn) {
      WARN( "Model %d can't be used with search (no evaluation function).\n",
             idx);
    }
  }
  return idx;
}

static int amodelSetAddItf (ClientData clientData, int argc, char *argv[])
{
  AModelSet *amsP   = (AModelSet*)clientData;
  IArray     senone = { NULL, 0 }, trans = { NULL, 0 };

  argc--;
 
  if ( itfParseArgv( argv[0], &argc, argv+1, 0,
      "<senones>",  ARGV_LIARRAY, NULL, &senone, &(amsP->snsP->list),      
                   "list of senones",
      "<trans>",    ARGV_LIARRAY, NULL, &trans,  &(amsP->tmsP->list),
                   "list of transition models",
       NULL) != TCL_OK || (senone.itemN != trans.itemN) ) {

    if (senone.itemA) free(senone.itemA); 
    if (trans.itemA)  free(trans.itemA); 
    return TCL_ERROR;
  }
  return amodelSetAdd (amsP,senone.itemN,senone.itemA,trans.itemA,-1,-1);
}

/* ------------------------------------------------------------------------
    amodelSetScaleItf
   ------------------------------------------------------------------------ */

static int amodelSetScaleItf (ClientData clientData, int argc, char *argv[])
{
  AModelSet *amodelSet = (AModelSet*)clientData;
  int   itemX;
  float scale;

  if ( itfParseArgv( argv[0], &argc, argv+1, 0,
      "<scale>",  ARGV_FLOAT, NULL, &scale, NULL, "scale factor",
      NULL) != TCL_OK) return TCL_ERROR;

  for (itemX=0;itemX<amodelSet->list.itemN;itemX++) {
    AModel* am = amodelSet->list.itemA + itemX;
    int stateX;
    for (stateX=0;stateX<am->stateN;stateX++) {
      int trX;
      for (trX=0;trX<am->fwdTrN[stateX];trX++) {
	am->fwdPen[stateX][trX] = scale * am->fwdPen_org[stateX][trX];
      }
    }
  }
  return TCL_OK;
}

/* ------------------------------------------------------------------------
    amodelSetPutsItf
   ------------------------------------------------------------------------ */

static int amodelSetPutsItf (ClientData clientData, int argc, char *argv[])
{
  AModelSet *amodelSet = (AModelSet*)clientData;

  argc--;
 
  if ( itfParseArgv( argv[0], &argc, argv+1, 0,  NULL) != TCL_OK) {
         return TCL_ERROR;
  }

  itfAppendResult("%d",amodelSet->list.itemN);
  return TCL_OK;
}

/* ------------------------------------------------------------------------
    amodelSetConfigureItf
   ------------------------------------------------------------------------ */

static int amodelSetConfigureItf( ClientData cd, char *var, char *val)
{
  AModelSet*  amsP = (AModelSet*)cd;
  if (! amsP) amsP = &amodelSetDefault;

  if (! var) {
    ITFCFG(amodelSetConfigureItf,cd,"name");
    ITFCFG(amodelSetConfigureItf,cd,"useN");
    ITFCFG(amodelSetConfigureItf,cd,"tree");
    ITFCFG(amodelSetConfigureItf,cd,"treeRoot");
    ITFCFG(amodelSetConfigureItf,cd,"senoneSet");
    ITFCFG(amodelSetConfigureItf,cd,"tmSet");
    ITFCFG(amodelSetConfigureItf,cd,"durTree");
    ITFCFG(amodelSetConfigureItf,cd,"durRoot");
    return TCL_OK;
  }
  ITFCFG_CharPtr(var,val,"name",      amsP->name, 1);
  ITFCFG_Int(    var,val,"useN",      amsP->useN, 1);

  ITFCFG_Object( var,val,"tree",      amsP->treeP,name,Tree,     1);
  ITFCFG_Int(    var,val,"treeRoot",  amsP->treeX,               1);
  ITFCFG_Object( var,val,"durTree",   amsP->durP, name,Tree,     1);
  ITFCFG_Int(    var,val,"durRoot",   amsP->durX,                1);
  ITFCFG_Object( var,val,"senoneSet", amsP->snsP,name,SenoneSet, 1);
  ITFCFG_Object( var,val,"tmSet",     amsP->tmsP,name,TmSet,     1);
  return TCL_ERROR;
}

/* ------------------------------------------------------------------------
    amodelSetAccessItf
   ------------------------------------------------------------------------ */

static ClientData amodelSetAccessItf (ClientData cd, char *name, TypeInfo **ti)
{ 
  AModelSet *amodelSet = (AModelSet*)cd;

  if (*name == '.') { 
    if (name[1]=='\0') {
      itfAppendElement( "tree");
      if ( amodelSet->durP) itfAppendElement( "durTree");
      itfAppendElement( "senoneSet");
      itfAppendElement( "tmSet");
      if (NULL != amodelSet->contextCache) {
	itfAppendElement( "contextCache");
      }
      *ti = NULL; return NULL;
    }
    else { 
      if (! strcmp( name+1, "tree")) {
        *ti = itfGetType("Tree");
        return (ClientData)amodelSet->treeP;
      }
      else if (amodelSet->durP && ! strcmp( name+1, "durTree")) {
        *ti = itfGetType("Tree");
        return (ClientData)amodelSet->durP;
      }
      else if (! strcmp( name+1, "senoneSet")) {
        *ti = itfGetType("SenoneSet");
        return (ClientData)amodelSet->snsP;
      }
      else if (! strcmp( name+1, "tmSet")) {
        *ti = itfGetType("TmSet");
        return (ClientData)amodelSet->tmsP;
      } 
      else if (! strcmp( name+1, "contextCache")) {
	*ti = itfGetType("ContextCache");
	return (ClientData) amodelSet->contextCache;
      }
    }
  }
  else if (*name == ':') { 
    if (name[1]=='\0') { 
      itfAppendElement( "0 %d",amodelSet->list.itemN-1);
      *ti = NULL; return NULL;
    }
    else { 
      int i = atoi(name+1);
      *ti = &amodelInfo;
      if (i>=0 && i<amodelSet->list.itemN) 
           return (ClientData) &(amodelSet->list.itemA[i]);
      else return NULL;
    }
  }
  return NULL;
}

Tree*      amodelSetTree(      AModelSet* amodelSet)
{ return amodelSet->treeP; }

TopoSet*   amodelSetTopoSet(   AModelSet* amodelSet) 
{ return (TopoSet*)(amodelSet->treeP->mdsP.cd); }

SenoneSet* amodelSetSenoneSet( AModelSet* amodelSet)
{ return amodelSet->snsP; }

TmSet*     amodelSetTmSet(     AModelSet* amodelSet)
{ return amodelSet->tmsP; }

Phones*    amodelSetPhones(    AModelSet* amodelSet)
{ return amodelSet->treeP->questions.phones; }

Tags*      amodelSetTags(      AModelSet* amodelSet)
{ return amodelSet->treeP->questions.tags; }

/* ------------------------------------------------------------------------
    amodelSetGet
   ------------------------------------------------------------------------ */

int amodelSetGet( AModelSet* amP, Word* wP, int left, int right)
{
  TopoSet*   topoSet = (TopoSet*)(amP->treeP->mdsP.cd);
  SenoneSet* snsP    =  topoSet->snsP;
  Topo*      topo;
  int*       senone  = NULL;
  int*       trans   = NULL;
  int        topoX, amodelX, durX=-1;
  int        stateN, sX, lnX = -1;
  int        idx;

  /* query cache */
  if (amP->contextCache && amP->contextCache->active) {
    idx = indexInContextCache(amP->contextCache, wP, left, right);
    if (idx >= 0) {
      /* found */
      return getModelOfContextCacheItem(amP->contextCache, idx);
    } 
    /* not found -> normal way and insert in cache later*/
  }
  

  if ((topoX = treeGetModel( amP->treeP, amP->treeX, wP, left, right)) < 0) 
       return -1;

  if (amP->durP) {
    durX = treeGetModel( amP->durP, amP->durX, wP, left, right);
  }

  topo   = topoSet->list.itemA + topoX;
  stateN = topo->tmN;

  senone = (int*)malloc( stateN * sizeof(int));
  trans  = (int*)malloc( stateN * sizeof(int));

  for ( sX = 0; sX < stateN; sX++) {
    int nX = topo->stnXA[sX];

    if ( nX != lnX) {
         lnX        = nX;
         senone[sX] = snsGetModel( snsP, nX, wP, left, right);
    }
    else senone[sX] = senone[sX-1];
    trans[sX] = topo->tmXA[sX];
  }

  amodelX = amodelSetAdd(amP,stateN,senone,trans,topoX,durX);
  /* insert in context cache */
  if (amP->contextCache && amP->contextCache->active) {
    insertInContextCache(amP->contextCache, wP, left, right, amodelX);
  }

  free( senone);
  free( trans);
  return amodelX;
}

static int amodelSetGetItf( ClientData clientData, int argc, char *argv[])
{
  AModelSet* amsP   =  (AModelSet*)clientData;
  Tree*      tree  =    amsP->treeP;
  int        left   = -1, right = 0;
  int        ac     = argc - 1;
  Word       word;

  wordInit( &word, tree->questions.phones, tree->questions.tags);

  if ( itfParseArgv( argv[0], &ac, argv+1, 0,
      "<tagged phones>",ARGV_CUSTOM, getWord, &word,NULL,
      "list of tagged phones",
      "<leftContext>",  ARGV_INT, NULL, &left,  NULL, "left  context",
      "<rightContext>", ARGV_INT, NULL, &right, NULL, "right context",
       NULL) != TCL_OK) {
    wordDeinit( &word);
    return TCL_ERROR;
  }
  itfAppendResult("%d", amodelSetGet( amsP, &word, left, right));
  wordDeinit( &word);
  return TCL_OK;
}

/* ------------------------------------------------------------------------
    amodelSetSkipTopo
   ------------------------------------------------------------------------ */


static int amodelSetSkipTopoItf( ClientData clientData, int argc, char *argv[])
{
  AModelSet* amsP   =  (AModelSet*)clientData;
  int     amodelX   =  0;
  int        skip   =  0;
  int        ac     = argc - 1;

  EvalFunc* evalFn0 = &(evalFuncA[1]); /*3 state topo without skip*/
  EvalFunc* evalFn1 = &(evalFuncA[2]); /*3 state topo with    skip*/


  if ( itfParseArgv( argv[0], &ac, argv+1, 0,
      "<skip>",  ARGV_INT, NULL, &skip,  NULL, "0/1 use skip architecture",
       NULL) != TCL_OK) {
    return TCL_ERROR;
  }

  for (amodelX=0; amodelX<amsP->list.itemN;amodelX++) {
    AModel* amP = amsP->list.itemA + amodelX;

    if (skip && amP->evalFn == evalFn0)
      amP->evalFn = evalFn1;

    if (! skip && amP->evalFn == evalFn1)
      amP->evalFn = evalFn0;
  }
  return TCL_OK;

}

/* ========================================================================
    Type Information
   ======================================================================== */

Method amodelMethod[] = 
{
  { "puts",   amodelPutsItf,   "displays the contents of an amodel"  },
  { NULL, NULL, NULL }
} ;

Method amodelSetMethod[] = 
{
  { "puts",   amodelSetPutsItf,   "displays the contents of an amodel set" },
  { "add",    amodelSetAddItf,    "add a state graph to a set" },
  { "get",    amodelSetGetItf,    
    "find acoustic model given a phonetic context"},
  { "scale",  amodelSetScaleItf,  "scale transition penalties" },
  { "reset",  amodelSetResetItf,  "remove all amodels from the set" },
  { "skip",   amodelSetSkipTopoItf,  "switch to 3state skip topologies" },

  { NULL, NULL, NULL }
} ;

TypeInfo amodelInfo = { 
        "AModel", 0, 0, amodelMethod, 0, 0, amodelConfigureItf, 
         0, itfTypeCntlDefaultNoLink,
        "acoustic model" };

TypeInfo amodelSetInfo = 
{ "AModelSet", 0, -1,     amodelSetMethod, 
   amodelSetCreateItf,    amodelSetFreeItf, 
   amodelSetConfigureItf, amodelSetAccessItf, NULL,
  "set of acoustic models"
};

static int amodelInitialized = 0;

int AModel_Init (void)
{
  if (! amodelInitialized) {
    if ( Senones_Init() != TCL_OK) return TCL_ERROR;
    if ( Tree_Init()    != TCL_OK) return TCL_ERROR;
    if ( Topo_Init()    != TCL_OK) return TCL_ERROR;
    if ( Tm_Init()      != TCL_OK) return TCL_ERROR;

    amodelSetDefault.name            = NULL;
    amodelSetDefault.useN            = 0;
    amodelSetDefault.list.itemN      = 0;
    amodelSetDefault.list.blkSize    = 1000;
    amodelSetDefault.treeP           = NULL;
    amodelSetDefault.snsP            = NULL;
    amodelSetDefault.tmsP            = NULL;

    itfNewType (&amodelInfo);
    itfNewType (&amodelSetInfo);

    amodelInitialized = 1;
  }
  return TCL_OK;
}
