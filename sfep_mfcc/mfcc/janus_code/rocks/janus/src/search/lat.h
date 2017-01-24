/* ========================================================================
    JANUS      Speech- to Speech Translation
               ------------------------------------------------------------
               Object: Tagger for an input lattice with language model
               ------------------------------------------------------------

    Author  :  Klaus Ries
    Module  :  lat.h
    Date    :  $Id: lat.h 512 2000-08-27 15:31:32Z jmcd $
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
    Revision 1.3  2000/08/27 15:31:20  jmcd
    Merging branch 'jtk-00-08-24-jmcd'.

    Revision 1.2.4.1  2000/08/25 22:19:44  jmcd
    Just checking.

    Revision 1.2  1998/06/29 23:46:43  kries
    *** empty log message ***

 * Revision 1.1  1998/06/11  14:52:10  kries
 * Initial revision
 *
 
   ======================================================================== */

#ifdef __cplusplus
extern "C" {
#endif

#ifndef _lat
#define _lat

#include "astar.h"
#include "lm.h"
#include "hash.h"

/* ------------------------------------------------------------------------
    Lattice links
   ------------------------------------------------------------------------ */
typedef struct LNodeTimeList_S   LNodeTimeList;
typedef struct LNodeState_S      LNodeState;
typedef struct LNodeTime_S       LNodeTime;

struct LNodeTime_S {

  long              startFrameX;  /* index of the word's start frame        */
  int               channelX;     /* the channel on which the               */

  float             lscore;       /* lookahead score                        */
  float             lBscore;      /* lookahead score if a break is ahead    */

  int               fanIn;        /* fan in from other search nodes         */

  LNodeState*       linkP;        /* list of all links starting from here   */

};

struct LNodeState_S { 

  LNodeTime        * parentP;
  int                stateX;


  float             lscore;       /* lookahead score */
  float             lBscore;      /* lookahead score if a break is ahead    */

  LNodeTimeList   * linkP;    /* list of all links starting from here with this state */
  LNodeState      * nextP;        /* list of all states for a start-time */
};

struct LNodeTimeList_S {

  LNodeState          * parentP;
  long                  endFrameX; 
  char                * rep;

  float score;

  LNodeTime           * timeP; /* This is redundant but makes hashing unnecessary during the search */


  LNodeTimeList       * nextP;
};


/* ------------------------------------------------------------------------
    Lat search state
   ------------------------------------------------------------------------ */

typedef enum { LatSubState_startTime, LatSubState_startTimeState, LatSubState_startTimeStateEndTime, LatSubState_EOS, LatSubState_FINAL  }  LatSubState;


/* AStarNodeP */ 
typedef union {          /* The search node is always a LNodeTimeList if in a path */
  LNodeTime              time;
  LNodeState             state;
  LNodeTimeList          timeL;
} LatSearchNode;

/* AStarSearchState */
typedef struct LatSearchState_S {
  /* either we have decided on a stateP OR a timeLP OR we must select channels to expand */
  float             lookaheadPA;
  float             lookaheadBreakPA;

  LatSubState subState;    /* select the pointertype */
  LNodeTime      ** timePA; /* foreach channel a pointer to the next start-node */

} LatSearchState;


/* AStarArc */
typedef struct LatSearchArcElement_S {
  LatSearchState state;
  float penalty;

  /* for the LatSubState_startTime subState we need to go through nodeP->timePA */

  int minTime, channel;
  union { LNodeState * stateP; LNodeTimeList * timeLP; } nextP;

} LatSearchArc;


typedef struct LatHypo_S LatHypo;
typedef struct LatHypoElement_S LatHypoElement;

struct LatHypo_S {
  int itemN;
  float score;
  LatHypoElement * itemA;
  LatHypo * nextP;
};

struct LatHypoElement_S {
  long              startFrameX;
  int               channelX;
  int               stateX;
  LMStateMem        StateMem;
  char*             rep;
};


/* ------------------------------------------------------------------------
    Lattice
   ------------------------------------------------------------------------ */

typedef struct Lat_S
{
  char*     name;
  int       useN;

  int       channelN;
  int       bigramLookahead;

  Hash      startTimes, startTimesStates, startTimesStatesEndTimes;

  LMPtr     lm, lmLookahead; 

  LatSearchArc   searchArc;
  LatSearchState initialState;

  AStar     searchObject;
  LatHypo * hypoP;
} Lat;

/* ------------------------------------------------------------------------
    Function Declarations
   ------------------------------------------------------------------------ */

                      /* char * rep is still owned by the caller to latNodeCreate */
extern int      lNodeCreate( Lat* latP, int channelX, long startFrameX, long endFrameX, int stateX, float score, char *rep);

extern int      latInit(   Lat* latP, char* name, LMPtr * lmP, LMPtr * lmLookaheadP, int channels);
extern int      latSearch( Lat* latP);

extern Lat     *latCreate( char* name, LMPtr * lmP, LMPtr * lmLookaheadP, int channels);
extern int      Lat_Init();

#endif








#ifdef __cplusplus
}
#endif
