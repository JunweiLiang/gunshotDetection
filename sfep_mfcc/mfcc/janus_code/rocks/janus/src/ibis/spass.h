/* ========================================================================
    JANUS      Speech- to Speech Translation
               ------------------------------------------------------------
               Object: Single pass decoder
               ------------------------------------------------------------

    Author  :  Hagen Soltau
    Module  :  spass.h
    Date    :  $Id: spass.h 2390 2003-08-14 11:20:32Z fuegen $
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
    Revision 4.1  2003/08/14 11:20:08  fuegen
    Merged changes on branch jtk-01-01-15-fms (jaguar -> ibis-013)

    Revision 1.1.2.26  2003/08/13 08:58:29  fuegen
    added spassGetFinalLCT to get the LCT of the final BP

    Revision 1.1.2.25  2002/02/01 14:17:20  fuegen
    moved stypes.h into slimits.h

    Revision 1.1.2.24  2001/07/14 15:29:45  soltau
    Added XCM

    Revision 1.1.2.23  2001/06/20 17:49:37  soltau
    removed glat beams

    Revision 1.1.2.22  2001/06/11 14:11:40  soltau
    Removed spassP->lvxA

    Revision 1.1.2.21  2001/06/01 10:35:23  soltau
    Added G'Lat support

    Revision 1.1.2.20  2001/05/25 13:22:06  soltau
    Added phonetic fast match strategie

    Revision 1.1.2.19  2001/05/11 16:40:48  metze
    Cleaned up LCT interface

    Revision 1.1.2.18  2001/04/27 07:39:06  metze
    General access functions to LM via 'LCT' implemented

    Revision 1.1.2.17  2001/04/23 13:43:21  soltau
    Added restart flag

    Revision 1.1.2.16  2001/04/19 15:36:27  soltau
    - Changed final frame handling
    - Made run-on working
    - Made partial trace back working

    Revision 1.1.2.15  2001/04/12 19:22:09  soltau
    removed same_pron helper

    Revision 1.1.2.14  2001/04/03 16:47:40  soltau
    *** empty log message ***

    Revision 1.1.2.13  2001/03/27 20:49:52  soltau
    lmrcmA is now of type lmScore

    Revision 1.1.2.12  2001/03/23 13:36:20  soltau
    Removed phone beam

    Revision 1.1.2.11  2001/03/20 13:34:24  soltau
    Added temporary frameY to mark final frame

    Revision 1.1.2.10  2001/03/16 14:57:08  soltau
    Added topN overflow information

    Revision 1.1.2.9  2001/03/14 12:52:47  soltau
    Added   lmrcmA
    Changed strace interface

    Revision 1.1.2.8  2001/03/13 09:57:25  soltau
    removed some container for word transitions

    Revision 1.1.2.7  2001/03/06 13:49:06  soltau
    Added phoneX

    Revision 1.1.2.6  2001/02/27 13:48:14  soltau
    Changed sns interface

    Revision 1.1.2.5  2001/02/21 18:03:28  soltau
    Added container for word transitions

    Revision 1.1.2.4  2001/02/20 12:36:56  soltau
    *** empty log message ***

    Revision 1.1.2.3  2001/02/16 17:49:42  soltau
    Added helper for same_pronunciation_words

    Revision 1.1.2.2  2001/02/15 18:03:16  soltau
    *** empty log message ***

    Revision 1.1.2.1  2001/02/02 16:11:13  soltau
    initial revision


   ======================================================================== */

#ifndef _spass
#define _spass

#include "slimits.h"
#include "phmm.h"
#include "xhmm.h"
#include "svocab.h"
#include "svmap.h"
#include "strace.h"
#include "sglobal.h"
#include "smem.h"
#include "stree.h"
#include "lmla.h"
#include "glat.h"

/* ========================================================================
    Container for Word transitions
   ======================================================================== */

typedef struct {
  float  score;
  LCT    lct;
  int    bpIdx;
  CTX    phoneX;
} LIceExt;


/* ========================================================================
    Single pass decoder
   ======================================================================== */

struct SPass_S {
  char* name;
  int   useN;

  STree* streeP;                   /* underlying search tree                */
  LMLA*  lmlaP;                    /* language model lookahead              */
  STab*  stabP;                    /* backpointer table                     */
  GLat*  latP;                     /* word lattice                          */

  PHMM*  hmmP;                     /* pointer to the set of PHMM models     */
  LCM*   lcmP;                     /* pointer to the set of LCM  models     */
  RCM*   rcmP;                     /* pointer to the set of RCM  models     */
  XCM*   xcmP;                     /* pointer to the set of XCM  models     */

  int    frameX;                   /* current frame to process              */
  int    frameN;                   /* nr. of frames                         */
  int    restart;                  /* flag for restart mode                 */

  struct {                         /* sorted backpointer container for each */
    int*      N;                   /* right context to start new transitions*/
    LIceExt** A;                   /* A[phoneX][topX] = backpointer, score  */
    float**   lmscore;             /* lm scores [bpX][rootX]                */
  } trans;              
  struct {                         /* container for active senones          */
    SenoneSet*  snsP;              /* pointer to senoneSet object           */
    int       totalN;              /* total number of senones               */
    int       activeN;             /* number of active senones              */
    float*    scoreA;              /* array of scores  for active senones   */
    int*      indexA;              /* array of indices for active senones   */
    UCHAR*    activeA;             /* flat to mark senone active            */
  } senone;
  struct {                         /* container for fast match models       */
    SenoneSet*  snsP;              /* pointer to senoneSet object           */
    int       activeN;             /* nr. of (permanently) active senones   */
    int       frameN;              /* number of lookahead frames            */
    int       stateN;              /* nr. of states for fast match model    */
    float**   scoreA;              /* array of scores  for active senones   */
    int*      indexA;              /* array of indices for active senones   */
    int       phoneN;              /* number of fast match models           */
    int*      snXA;                /* flaten array of senone model indices  */
    float*    s;                   /* array of fast match scores            */
    float     factor;              /* weighting factor for fm scores        */
  } fmatch;
  struct {                         /* container for active nodes            */
    int        rootN;              /* number of active SRoots               */
    int        nodeN;              /* number of active SNodes               */
    int        leafN;              /* number of active SLeafs               */
    SRoot**    rootPA;             /* array of pointer to active SRoots     */
    SNode**    nodePA;             /* array of pointer to active SNodes     */
    SNode**    leafPA;             /* array of pointer to active SLeafs     */
  } active;
  struct {                         /* active nodes in next frame            */
    int        rootN;              /* number of active SRoots               */
    int        nodeN;              /* number of active SNodes               */
    int        leafN;              /* number of active SLeafs               */
    SRoot**    rootPA;             /* array of pointer to active SRoots     */
    SNode**    nodePA;             /* array of pointer to active SNodes     */
    SNode**    leafPA;             /* array of pointer to active SLeafs     */
  } actNext;
  struct {                         /* thresholds for beam search            */
    float      best;               /* best overall score                    */
    float      morph;              /* threshold for instance level pruning  */
    float      state;              /* threshold for state    level pruning  */
    float      word;               /* threshold for word     level pruning  */
    float      lat;                /* threshold for lattice  level pruning  */
    int        topN;
    int        morphN;
  } beam;
};

/* ========================================================================
    Module Interface
   ======================================================================== */

extern int SPass_Init       ();
extern LCT spassGetFinalLCT (SPass *spassP);

#endif
