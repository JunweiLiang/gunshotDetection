/* ========================================================================
    JanusRTk   Janus Recognition Toolkit
               ------------------------------------------------------------
               Object: Senone Stream Weights
               ------------------------------------------------------------

    Author  :  Florian Metze
    Module  :  snStreamWeights.h
    Date    :  $Id: snStreamWeights.h 2704 2005-12-20 15:14:50Z metze $

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
    Revision 4.2  2005/12/20 15:14:50  metze
    Code for AF training (snStreamWeights.c|h)

    Revision 4.1  2005/04/26 14:41:19  metze
    SenoneSet Stream Training

   ======================================================================== */

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>
#include "common.h"
#include "itf.h"
#include "array.h"
#include "matrix.h"
#include "senones.h"
#include "codebook.h"
#include "distrib.h"
#include "distribStream.h"
#include "mach_ind_io.h"
#include "bbi.h"
#include "scoreA.h"

extern int snsWeightsClear     (SenoneSet* snsP);
extern int snsWeightsAccu      (SenoneSet* snsP, Path* pathP, int idx,
				int fromX, int toX, int v, int zm);
extern int snsWeightsUpdate    (SenoneSet* snsP, char* mode, char* range_mode, double min,
				double mass, double contrast, int startX, float minCount, float* q, int v);
extern int snsWeightsAccuMLE   (SenoneSet* snsP, Path* pathP, int v, int zm);
extern int snsWeightsUpdateMLE (SenoneSet* snsP, float minCnt, float M, float K,
				int mode, int zm, int nm, int sIdx);
extern int snsWeightsLoad      (SenoneSet *snsP, char* name);
extern int snsWeightsSave      (SenoneSet *snsP, char* name);

enum { SMOOTH_GLOBAL, SMOOTH_PHONE, SMOOTH_STATE, SMOOTH_SENONE };
