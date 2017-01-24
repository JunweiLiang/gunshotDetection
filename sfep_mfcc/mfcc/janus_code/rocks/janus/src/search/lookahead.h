/* ========================================================================
    JANUS      Speech- to Speech Translation
               ------------------------------------------------------------
               Object: Dynamic Programming Search (Lookahead)
               ------------------------------------------------------------

    Author  :  Michael Finke
    Module  :  treefwd.h
    Date    :  $Id: lookahead.h 512 2000-08-27 15:31:32Z jmcd $
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
    Revision 1.3  2000/08/27 15:31:24  jmcd
    Merging branch 'jtk-00-08-24-jmcd'.

    Revision 1.1.1.1.4.1  2000/08/25 22:18:36  jmcd
    Added extern "C" statements to all header files; compiled main() in
    C++ to ensure proper initialization of C++ libraries.

    Revision 1.1.1.1  2000/03/07 17:22:01  soltau
    two phone lookahead

    Revision 1.1  2000/03/02 14:13:54  soltau
    Initial revision


 
   ======================================================================== */

#ifdef __cplusplus
extern "C" {
#endif

#ifndef _lookahead
#define _lookahead

#include "treefwd.h"

typedef struct LookAhead_S {
  
  int        distN;            /* number of distributions used for la */  
  int        phoneN;           /* number of phonems in la phoneset */

  int*       monoPhoneModelXA; /* for each monophone, give amodelX */

  float      factor;           /* fastmatch weight */
  short      matchfN;          /* number of fastmatch frames     */
  short      frameN;           /* total number of frames in utt */

  float*     activeScoreA;
  char *     activeDistA;
  int*       activeDistXA;     /* array of active distrib IDs    */
  int        activeDistXN;

  float**    distScoreM;       /* dist scores for fastmatch matchN frames */ 
  float*     distScoreA;       /* dist scores for fastmatch current frame */
  float*     distScoreP;       /* space is attached to this one */
  float**    phoneScoreM;      /* lookahead scores for fastmatch matchfN frames */
  float*     phoneScoreA;      /* lookahead scores for fastmatch current frame */
  float*     phoneScore2A;     /* lookahead scores for fastmatch current frame */

  float*     bestPhoneScoreA;  /* store best phone scores for each frame */

  int*       avgDur;           /* average phone duration  */
  float      spkRate;          /* speaking rate (1.0 means average spk.rate */

  AModelSet* amodelSetP;       /* amodel to be used for lookaheads */
  AModel*    amodelA;
  SenoneSet* snsP;             /* senoneSet to be used for lookaheads */
  

} LookAhead;

extern int  LookAhead_Init (void);
extern int  lookAheadInit( LookAhead* laP, Vocab* vocabP);
extern int  lookAheadCreate( void );

extern void   dpsLaReset(     short frameN);
extern void   dpsLaNextFrame( short frameX);

/*new lookahead access */
extern float  dpsLaScoreR(    short frameX, RTrellis* rtrP, int mode); /*lookahead for root trellis */
extern float  dpsLaScore(     short frameX, Trellis* trP, int mode);   /*lookahead for trellis */
extern float  dpsLaScorePhone(short frameX, int phoneX);     /*lookahead for last phones */




#endif

#ifdef __cplusplus
}
#endif
