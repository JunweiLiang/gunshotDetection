/* ========================================================================
    JANUS      Speech- to Speech Translation
               ------------------------------------------------------------
               Object: Search Module
               ------------------------------------------------------------

    Author  :  Monika Woszczyna
    Module  :  search.h
    Date    :  $Id: search.h 700 2000-11-14 12:35:27Z janus $
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
    Revision 3.9  2000/11/14 12:35:25  janus
    Merging branch jtk-00-10-27-jmcd.

    Revision 3.8.28.1  2000/11/06 10:50:40  janus
    Made changes to enable compilation under -Wall.

    Revision 3.8  2000/09/12 15:06:30  janus
    Merged branch jtk-00-09-08-hyu.

    Revision 3.7.10.1  2000/09/12 01:18:05  hyu
    Added acoustic lattice rescoring from Michael Finke.

    Revision 3.7  2000/08/27 15:31:29  jmcd
    Merging branch 'jtk-00-08-24-jmcd'.

    Revision 3.6.2.1  2000/08/25 22:18:44  jmcd
    Added extern "C" statements to all header files; compiled main() in
    C++ to ensure proper initialization of C++ libraries.

    Revision 3.6  2000/08/24 13:32:26  jmcd
    Merging changes from branch jtk-00-08-23-jmcd.

    Revision 3.5.2.1  2000/08/24 00:18:06  jmcd
    Fixed the hairy elbows Hagen left for me to find.

    Revision 3.5  1998/06/11 17:50:32  kries
    adaptation of new LM interface

 * Revision 3.4  1997/07/18  17:57:51  monika
 * gcc-clean
 *
    Revision 3.3  1997/07/18 14:00:01  monika
    dfki-clean code

    Revision 3.2  1997/06/17 17:41:43  westphal
    *** empty log message ***

    Revision 1.11  1996/07/24 10:08:55  monika
    removed reference to LatConfig Type

    Revision 1.10  1996/07/03  13:19:26  monika
    changes to work with 64K vocabulary

    Revision 1.9  96/07/01  14:56:39  14:56:39  monika (Monika Woszczyna)
    includes beamopti.h and defines FR; fr is made part of the search object.
    all this is for the forced recognition in beamopti.c
    
    Revision 1.8  96/04/23  19:43:18  19:43:18  maier (Martin Maier)
    monika: partial hypos and RUNON
    
    Revision 1.7  96/02/17  19:25:21  19:25:21  finkem (Michael Finke)
    hypoList and duration modelling changed/edited
    
    Revision 1.6  1996/01/14  14:20:12  monika
    new calls

    Revision 1.5  95/09/12  19:51:51  19:51:51  finkem (Michael Finke)
    Rudimentary handling of a complete search transaction
    
    Revision 1.4  1995/08/29  02:45:29  finkem
    *** empty log message ***

    Revision 1.3  1995/08/06  19:26:18  finkem
    *** empty log message ***

    Revision 1.2  1995/08/03  14:42:02  finkem
    *** empty log message ***

    Revision 1.1  1995/07/25  13:49:57  finkem
    Initial revision

 
   ======================================================================== */

#ifdef __cplusplus
extern "C" {
#endif

#ifndef _search
#define _search

#include "common.h"
#include "itf.h"
#include "vocab.h"
#include "hypos.h"
#include "amodel.h"
#include "matrix.h"
#include "lm.h"

/* ------------------------------------------------------------------------
    Global Defines
   ------------------------------------------------------------------------ */

#define MAX_MONOPHONES       600
#define WORSTSCORE           9999999.0
#define STATESperMODEL       6       /* number of states in a segment       */
#define STATESperTRELLIS     7       /* plus one NULL state to collect info */
#define FINALSTATE           6

#define MAX_FRAMES           50000

typedef unsigned short WordX;
typedef int   BpX;
typedef short DurCnt;

struct BPtr_S {
  short  frameX;
  WordX  bpX;
  DurCnt cnt;
};

#define NILWORD 65535
#define NILBPTR -1

/* Neither BEG_SIL nor END_SIL should be defined as filler words, since
   some of the program loops might become "instable" (eg. SetBPRealWord) */

#define ISA_FILLER(w) (w < SD.firstX-2)

/* ------------------------------------------------------------------------
    Search Includes
   ------------------------------------------------------------------------ */

#include "senones.h"
#include "lm_3g.h"

#include "lookahead.h"
#include "treefwd.h"
#include "flatfwd.h"
#include "lattice.h"
#include "beamopti.h"
#include "wordlist.h"
#include "lmodel.h"

/* ------------------------------------------------------------------------
    SConfig   search module configuration object
   ------------------------------------------------------------------------ */

typedef struct SConfig_S
{
  SDict      sd;                        /* search dictionary                */
  DistScore  ds;                        /* distribution scoring             */
  LookAhead  la;                        /* stuff for lookaheads/fast match */
  Forced     fr;
  Root       rt;                        /* trellis objects                  */
  LMPtr*     lmodel;                    /* language model                   */

  BPTable    bp;                        /* table of backpointers            */
  TreeFwd    tf;                        /* tree forward configuration       */
  FlatFwd    ff;                        /* flat forward information         */

/* LatConfig  lt;                  */    /* lattice rescoring                */
  Lattice    lattice;                   /* lattice                          */
  WordList   wl;                        /* wordlist to communicate between
                                           search passes */ 

  SenoneSet* snsP;

  long       transactionX;              /* index of the current transaction */
  int        frameN;                    /* size of input of last search     */
  int        from;
  int        shift;

  float      phoneInsertionPenalty;
  float      wordPenalty;
  float      silenceWordPenalty;
  float      fillerWordPenalty;

  int        use3gFwd;

  int        verbose;         /* verbosity level                */
  int        printWidth;      /* width of progress report       */

  int demoMode;

} SConfig;

/* ------------------------------------------------------------------------
    Search Configuration
   ------------------------------------------------------------------------ */

extern SConfig sc;

#define SC sc
#define SD sc.sd
#define RT sc.rt
#define TF sc.tf
#define DS sc.ds
#define LA sc.la
#define BP sc.bp
#define FP sc.fp
#define FF sc.ff
#define LT sc.lattice
#define FR sc.fr

/* ========================================================================
    Search
   ======================================================================== */

typedef struct
{
  char*     name;          /* name of the search module            */
  int       useN;          /* number of references to this word    */

  SConfig   searchCfg;     /* local search configuration structure */

  Vocab*    vocabP;        /* search vocabulary                    */
  LMPtr     lmP;           /* language model                       */

} Search;

/* ========================================================================
    Module Interface
   ======================================================================== */

extern int     Search_Init ( void );

extern int     searchInit(   Search* SP, char*  name, 
                                         Vocab* vocabP, LMPtr* lmP);
extern Search* searchCreate( char* name, Vocab* vocabP, LMPtr* lmP);

extern int     searchLinkN(  Search* SP);
extern int     searchDeinit( Search* SP);
extern int     searchFree(   Search* searchP);
extern int     searchCfgUpdate( Search* sP); /* copies back info to main struct */
extern int     wordListAlloc  (WordList * wlP, Search *searchP, int minEFrameN);
extern int     wordListAllocLAT  (WordList * wlP, Search *searchP, int minEFrameN);
extern int     wordListAllocDummy  (WordList * wlP, Search *searchP);
extern int     dpsPrintScale( short frameN);
#endif

#ifdef __cplusplus
}
#endif
