/* ========================================================================
    JANUS      Speech- to Speech Translation
               ------------------------------------------------------------
               Object: Dynamic Programming Search
               ------------------------------------------------------------

    Author  :  Michael Finke
    Module  :  lattice.h
    Date    :  $Id: lattice.h 2390 2003-08-14 11:20:32Z fuegen $
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
    Revision 3.8  2003/08/14 11:20:22  fuegen
    Merged changes on branch jtk-01-01-15-fms (jaguar -> ibis-013)

    Revision 3.7.20.1  2002/06/26 11:57:58  fuegen
    changes for new directory structure support and code cleaning

    Revision 3.7  2000/11/14 12:29:35  janus
    Merging branch jtk-00-10-27-jmcd.

    Revision 3.6.30.1  2000/11/06 10:50:32  janus
    Made changes to enable compilation under -Wall.

    Revision 3.6  2000/08/27 15:31:20  jmcd
    Merging branch 'jtk-00-08-24-jmcd'.

    Revision 3.5.4.1  2000/08/25 22:19:44  jmcd
    Just checking.

    Revision 3.5  1998/06/11 17:50:32  kries
    adaptation of new LM interface

 * Revision 3.4  1998/05/26  18:27:23  kries
 * adaptation to new language model interface
 * addition of new A^* search
 *
 * Revision 3.3  1997/07/18  17:57:51  monika
 * gcc-clean
 *
    Revision 3.2  1997/06/17 17:41:43  westphal
    *** empty log message ***

    Revision 1.10  96/10/23  16:58:14  kries
    fixed mbest search: exact lookahead, better memory managment
    
    Revision 1.9  96/07/14  18:39:07  monika
    added stuff required for vm lattices
    
    Revision 1.8  96/07/03  13:19:26  13:19:26  monika (Monika Woszczyna)
    changes to work with 64K vocabulary
    
    Revision 1.7  96/03/24  23:22:44  23:22:44  kries (Klaus Ries)
    added support for cachemodel in rescoring
    
    Revision 1.6  1995/10/29  23:52:24  finkem
    got rid of stupid oldZ and oldP flags by reading the
    original values from the LM

    Revision 1.5  1995/10/26  20:44:28  finkem
    find N-best implemented

    Revision 1.4  1995/09/21  21:39:43  finkem
    lattice rescoring bug removed

    Revision 1.3  1995/09/13  18:51:04  finkem
    finalAScore sort of fixed...

    Revision 1.2  1995/09/12  19:49:53  finkem
    First  Janus3 version of the lattice procedure introduced...

    Revision 1.1  1995/07/25  13:49:57  finkem
    Initial revision

 
   ======================================================================== */

#ifdef __cplusplus
extern "C" {
#endif

#ifndef _lattice
#define _lattice

#include "array.h"
#include "search.h"
#include "treefwd.h"
#include "lm.h"

/* -------------------------------------------------------------------------
  Basic structure of a lattice in internal representation:
    There are nodes which contain the word information and a pointer linked
    list of arcs (links); The node always contains the startframe for the word,
    the link the end-frame; Missing frames inbetween were spend in optional
    silence (not included in the lattice). There is an additional nextP
    in the nodes (not shown) to link all nodes in the lattice. The accoustic
    score is held in the nodes, the lm-score in the links.
  
 startP-->+-------+     +-------+   +-------+   +-------+     +-------+
          | NODE  |     | LINK  |   | NODE  |   | LINK  |     | NODE  |
          | linkP------>| toP------>| linkP---->| toP-------->| linkP--->NULL
          |       |<------fromP |   |       |<----fromP |     |       |
          | fX=0  |     | fX=10 |   | fX=11 |   | fX=20 |     | fX=21 |
          | "("   |     | nextP |   | "yes" |   | nextP |     | ")"   |
          +-------+<-+  +-----|-+   +-------+   +-----|-+  +->+-------+
                     |        |                       v    |          ^
                     |        |                      NULL  |          | 
                     |        v                            |     finalP
                     |  +-------+   +-------+   +-------+  |
                     |  | LINK  |   | NODE  |   | LINK  |  |
                     |  | toP------>| linkP---->| toP------+
                     +----fromP |   |       |<----fromP |  
                        | fX=12 |   | fX=13 |   | fX=19 |
                        | nextP |   | "no"  |   | nextP |  
                        +-----|-+   +-------+   +-----|-+  
                              v                       v    
                             NULL                    NULL
                             
                             
  For the arc-based VerbMobil-Fileformat, we need an arc that can hold the
  word-string in the arc rather than in the node. The corresponding
  data-structures should ONLY be used to prepare lattices for writing.
  -------------------------------------------------------------------------*/


/* ------------------------------------------------------------------------
    LatLink: Type used to represent an arc in internal processing of
             lattices where the word ids are on the nodes.
   ------------------------------------------------------------------------ */

typedef struct LatLink_S { 

  float             lscore;       /* link score: the acoustic score of the
                                     "from" word considering the "to" word
                                     as right context                       */
  float             pscore;
  int               frameX;       /* end frame of the "from" word           */

  struct LatNode_S* fromP;        /* from node in the lattice               */
  struct LatNode_S* toP;          /* to node   in the lattice               */
  struct LatLink_S* nextP;        /* list of links (of the same node)       */
  struct LatLink_S* bestP;        /* best link                              */
  struct LatLink_S* qnextP;       /* next link in queue                     */

} LatLink;

typedef struct VmLink_S {
  int    fromNX;
  int    toNX;
  int    fromFX;
  int    toFX;
  float  score;
  float  lmScore;
  short  isNoise;
  char*  wordS;
} VmLink;


/* ------------------------------------------------------------------------
    LatNode
   ------------------------------------------------------------------------ */

typedef struct LatNode_S {

  WordX             wordX;        /* word index                             */
  WordX             lmWordX;      /* language model word index              */

  short             frameX;       /* index of the word's start frame        */
  float             score;        /* backward pass score                    */
  union {
    int   fanIn;                  /* number of links into the node (temp.)  */
    int   nodeX;                  /* index of the node                      */
    float breakScore;             /* lookahead score for hmm3gram           */
  } info;
  LatLink*          linkP;        /* list of all links starting from here   */
  struct LatNode_S* nextP;        /* next node in the list                  */

} LatNode;

/* ------------------------------------------------------------------------
    LatPath
   ------------------------------------------------------------------------ */

typedef struct LatPath_S {

  float             score;
  int useN;
  LatLink*          linkP;

  struct LatPath_S* parentP;
  struct LatPath_S* nextP;

} LatPath;

/* ------------------------------------------------------------------------
    Lattice
   ------------------------------------------------------------------------ */

typedef struct Lattice_S
{
  char*     name;
  int       useN;

  Vocab*    vocabP;        /* pointer to vocabulary used  */
  float     beamWidth;     /* lattice allocation beam     */
  float     noiseBeamWidth;     /* lattice allocation beam     */
  int       topN;

  LatNode*  nodeP;         /* list of all lattice nodes   */
  LatNode*  startP;        /* lattice start node          */
  LatNode*  finalP;        /* final lattice node          */
  short     frameN;

  float     finalAScore;   /* acoustic score of the final node */
  int       nodeN;         /* number of nodes                  */ 
  int       linkN;         /* number of links in lattice       */

  HypoList  hypoList;      /* list of hypothesis               */

  float     z;
  float     p;
  char* format;        /* file format for writing */
  LMPtr     lmPtr;
  
} Lattice;

/* ------------------------------------------------------------------------
    LatConfig
   ------------------------------------------------------------------------ */


/* ------------------------------------------------------------------------
    Function Declarations
   ------------------------------------------------------------------------ */

extern int      Lattice_Init (void);

extern int      latNodeInit(   LatNode* lnP,  short frameX, WordX wordX);
extern LatNode* latNodeCreate( Lattice* latP, short frameX, WordX w);

extern int      latNodeDeinit( LatNode* lnP);
extern int      latNodeFree(   LatNode* lnP);

extern LatNode* latNodeFind(   Lattice* latP, short frameX, WordX w);

extern int      latLinkInit(   LatLink* llP, LatNode* fromP, LatNode* toP,
                               float score, short frameX);
extern LatLink* latLinkCreate( LatNode* fromP, LatNode* toP, float score,
                               float pscore,   short frameX, int*  nlinks);

extern int      latLinkDeinit( LatLink* lnP);
extern int      latLinkFree(   LatLink* lnP);

extern int      latticeInit(   Lattice* latP, char* name, Vocab* vocabP);
extern Lattice *latticeCreate(                char* name, Vocab* vocabP);

extern int      latLinksAlloc( Lattice* latP, LatNode* toP, short frameX, 
			       float beamWidth, float z);
extern int      latticeAlloc(  Lattice* latP, short frameN, 
			       float z, float beamWidth);
extern int	latticeCheck( Lattice* latP);
extern void     latticeDeleteUnreachable( Lattice* latP);

extern void     dplTuneLattice(   Lattice* latP, CHAR* id);

extern int      latticeFindMBest( Lattice* latP,  LmStruct* lmP, int n, 
                                  float zn, float pn,
                                  int maxPathN, int maxHypTriedN);

#endif

#ifdef __cplusplus
}
#endif
