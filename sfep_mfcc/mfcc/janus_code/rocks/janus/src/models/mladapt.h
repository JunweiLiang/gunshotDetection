/* ========================================================================
    JanusRTk   Janus Recognition Toolkit
               ------------------------------------------------------------
               Object: Maximum Likelihood Adaptation
               ------------------------------------------------------------

    Author  :  Michael Finke
    Module  :  mladapt.h
    Date    :  $Id: mladapt.h 2390 2003-08-14 11:20:32Z fuegen $
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
    Revision 3.4  2003/08/14 11:20:15  fuegen
    Merged changes on branch jtk-01-01-15-fms (jaguar -> ibis-013)

    Revision 3.3.52.3  2002/08/30 12:31:05  soltau
    Added storeAccus, restoreAccus

    Revision 3.3.52.2  2002/04/02 16:49:21  soltau
    2th bem for multi-threading

    Revision 3.3.52.1  2002/03/11 10:42:57  soltau
    Added Block memory management for SAT

    Revision 3.3  2000/08/27 15:31:25  jmcd
    Merging branch 'jtk-00-08-24-jmcd'.

    Revision 3.2.4.2  2000/08/27 09:47:51  jmcd
    Small changes.

    Revision 3.2.4.1  2000/08/25 22:19:45  jmcd
    Just checking.

    Revision 3.2  1997/06/17 17:41:43  westphal
    *** empty log message ***

 
   ======================================================================== */


#ifndef _mladapt
#define _mladapt

#include "codebook.h"
#include "bmem.h"

/* ========================================================================
    MLAdaptItem
   ======================================================================== */

typedef struct {

  int      cbX;      /* index of codebook                   */
  int      refX;     /* index of gaussian                   */
  int      wX;       /* index of transformation matrix      */
  int      wbestX;   /* index of best transformation matrix */
  int      count;    /* count = gamma                       */

  FVector* muAccuV;  /* SAT accumulator buffers             */
  FMatrix* muAccuM;
  FVector* muCovAV;
  float    muGamma;

  float*   mu;       /* store original gaussians            */
  double*  ac;       /* store original accumulators         */ 

} MLAdaptItem;

/* ========================================================================
    MLAdaptNode
   ======================================================================== */

typedef struct MLAdaptNode_S {

  int    refF;              /* first reference in MLAdaptItem list */
  int    refL;              /* last item in the MLAdaptItem list   */
  double count;             /* sample count in that node           */

  int    w;                 /* index of adaptation matrix used     */
  int    idxN;
  char*  idxA;              /* characteristic function             */
  float* accuA;             /* accumulated counts                  */

  struct MLAdaptNode_S* leftP;
  struct MLAdaptNode_S* rightP;

} MLAdaptNode;

/* ========================================================================
    MLAdapt
   ======================================================================== */

typedef struct {

  char*        name;
  int          useN;

  int          itemN;
  MLAdaptItem* itemA;

  CodebookSet* cbsP;

  int          dimN;       /* description of codebook structure */
  int          featX;      /* and feature specification         */
  CovType      type;
  int          mode;

  MLAdaptNode* nodeP;      /* pointer to the regression tree */

  int          wX;         /* next free transformation buff  */
  int          wN;         /* number of save transformations */
  DMatrix**    wA;         /* array of saved transformations */

  BMem*        memP;       /* block memory management for SAT */
  BMem*        mem2P;      /* secondary bmem for threads      */

} MLAdapt;


#endif
