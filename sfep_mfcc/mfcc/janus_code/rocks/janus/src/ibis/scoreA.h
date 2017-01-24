/* ========================================================================
    JANUS      Speech- to Speech Translation
               ------------------------------------------------------------
               Object: extended score routines
               ------------------------------------------------------------

    Author  :  Hagen Soltau
    Module  :  scoreA.h
    Date    :  $Id: scoreA.h 3256 2010-05-26 18:38:28Z metze $
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
    Revision 4.4  2005/10/12 13:35:40  metze
    Added ssa_base_str

    Revision 4.3  2004/09/10 15:38:44  metze
    Added base_semCont

    Revision 4.2  2004/08/27 13:22:10  metze
    Added opt_semCount

    Revision 4.1  2003/08/14 11:20:06  fuegen
    Merged changes on branch jtk-01-01-15-fms (jaguar -> ibis-013)

    Revision 1.1.2.7  2002/04/12 16:03:15  soltau
    Merged ssa_opt_str with ssa_opt_feat

    Revision 1.1.2.6  2002/04/03 11:08:23  soltau
     Added ssa_opt_thread

    Revision 1.1.2.5  2002/01/30 09:28:15  metze
    Added opt_feat score function

    Revision 1.1.2.4  2001/10/24 15:20:56  soltau
    Added ssa_opt_str

    Revision 1.1.2.3  2001/07/04 18:11:14  soltau
    Added ssa_comp_all to accumulate compressed statistics
    Removed ssa_mmx

    Revision 1.1.2.2  2001/07/03 09:45:48  soltau
    Added score functions for compressed codebooks

    Revision 1.1.2.1  2001/02/22 16:11:12  soltau
    Initial version

 
   ======================================================================== */

#ifndef _scorea
#define _scorea

#include "senones.h"
#include "distrib.h"

extern float ssa_one      (SenoneSet *snsP, int senoneX, int frameX);

extern int ssa_base       (SenoneSet *snsP, int *idxA, float *scoresA,
			   int  idxN, int frameX);
extern int ssa_opt        (SenoneSet *snsP, int *idxA, float *scoresA,
			   int  idxN, int frameX);
extern int ssa_gdft        (SenoneSet *snsP, int *idxA, float *scoresA,
			   int  idxN, int frameX);
extern int ssa_opt_thread (SenoneSet *snsP, int *idxA, float *scoresA,
			   int  idxN, int frameX);
extern int ssa_opt_str    (SenoneSet *snsP, int *idxA, float *scoresA,
			   int  idxN, int frameX);
extern int ssa_base_str   (SenoneSet *snsP, int *idxA, float *scoresA,
			   int  idxN, int frameX);
extern int ssa_compress   (SenoneSet *snsP, int *idxA, float *scoresA,
			   int  idxN, int frameX);
extern int ssa_comp_all   (DistribSet *dssP, int dsX, int frameX,
			   float* addCountA);

extern int ssa_opt_semCont(SenoneSet *snsP, int *idxA, float *scoresA,
                           int  idxN, int frameX);

extern int ssa_base_semCont(SenoneSet *snsP, int *idxA, float *scoresA,
                           int  idxN, int frameX);

extern int ssa_matrix (DistribSet* dssP, FMatrix* scoreP, FMatrix* featP, int topN, char* mode);
#endif
