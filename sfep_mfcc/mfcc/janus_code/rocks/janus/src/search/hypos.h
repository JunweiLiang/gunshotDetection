/* ========================================================================
    JANUS      Speech- to Speech Translation
               ------------------------------------------------------------
               Object: Hypotheses Support
               ------------------------------------------------------------
               
    Author  :  Monika Woszczyna
    Date    :  $Id: hypos.h 512 2000-08-27 15:31:32Z jmcd $
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
   Revision 3.5  2000/08/27 15:31:17  jmcd
   Merging branch 'jtk-00-08-24-jmcd'.

   Revision 3.4.4.1  2000/08/25 22:19:44  jmcd
   Just checking.

   Revision 3.4  1998/05/26 18:27:23  kries
   adaptation to new language model interface
   added support for state information

 * Revision 3.3  1997/07/18  17:57:51  monika
 * gcc-clean
 *
   Revision 3.2  1997/06/17 17:41:43  westphal
   *** empty log message ***

    Revision 1.8  96/04/23  19:42:42  maier
    monika: changes for partial hypos
    
    Revision 1.7  96/02/17  19:25:21  19:25:21  finkem (Michael Finke)
    finally cleaned up the old hypoList stuff
    
    Revision 1.6  1996/01/07  17:57:50  monika
    complete rework to configure hypoLists with setPublish
    not compatible with old scripts !!

    Revision 1.5  1995/09/13  18:53:54  finkem
    introduced utterance id string for publishing methods

    Revision 1.4  1995/08/29  02:45:16  finkem
    *** empty log message ***

    Revision 1.3  1995/08/27  22:36:57  finkem
    *** empty log message ***

    Revision 1.2  1995/08/07  20:57:24  finkem
    *** empty log message ***

    Revision 1.1  1995/07/25  13:49:57  finkem
    Initial revision

   ======================================================================== */

#ifdef __cplusplus
extern "C" {
#endif

#ifndef _hypos
#define _hypos

#include "vocab.h" /* we only need this to have Vocab for function calls */

#define NONE  NULL
#define MINI  "{other stderr unfiltered}"
#define PLAIN "{other stderr unfiltered} {file hypo_F.all append filtered}"

#define TEMP 0
#define FINAL 1

/* ========================================================================
    MoreInfo
   ======================================================================== */

typedef struct 
{
  int   rank;          /* bpX */
  int   maxRank;       /* bpN */
  float bestScore;     /* global best score at end frame */
  float lastPhoneBestScore; /* comparing only last phones */
  float bestWEndScore; /* best word end score in stack */
  float totalScore;    /* total score of hypo up to here */
} MoreInfo;

/* ========================================================================
    HypoWord
   ======================================================================== */

typedef struct  
{                                  /* hypothesized word type:               */
  VWordX vocabX;                   /* vocabulary index of word              */
  short  wsFrameXF;                /* sentence-relative start frame of word */
  short  weFrameXF;                /* sentence-relative end   frame of word */
  float  wordScore;                /* score cumulated within this word      */
  char * stateString;              /* String representation of the state    */
} HWord;

/* ========================================================================
    Hypo
   ======================================================================== */

typedef struct 
{                               /* hypothesized sentence type:              */
  float            score;       /* DP score for this sentence               */
  short            hwordN;      /* number of words in the sentence          */
  HWord*           hwordA;
  MoreInfo*        moreInfoA;

  struct HypoList* hypoListP;   /* pointer points back to it's container    */
} Hypo;

/* ========================================================================
    HypoList
   ======================================================================== */

typedef struct HypoList
{
  char*   name;
  int     useN;
  short   hypoN;
  short   allocN;
  short   allocBlockSize;
  Hypo**  hypoPA;
  char*   idS;                   /* utterance ID string */

  FILE*   tempHandle;
  int     useTempHandle;
  Vocab*  vocabP;

} HypoList;

/* ========================================================================
    Module Interface
   ======================================================================== */

extern int Hypos_Init (void);

extern int   hypoInit(   Hypo* hypoP, int hwordN);
extern Hypo* hypoCreate( int   hwordN);

extern int   hypoDeinit( Hypo* hypoP);
extern int   hypoFree(   Hypo* hypoP);

extern int   hypoGetItf( ClientData cd, char* key, ClientData result, 
                                        int*  acP, char* av[]);

extern int   hypoListInit(   HypoList* hypoListP, char* name, Vocab* vocabP,
                             int blockSize);
extern int   hypoListReinit( HypoList* hypoListP);

extern HypoList* hypoListCreate( char* name, Vocab *vocabP, int blocksize);

extern int   hypoListDeinit(  HypoList* hypoListP);
extern int   hypoListDestroy( HypoList* hypoListP);
extern int   hypoListAppend(  HypoList* hypoListP, Hypo *hypoP);
extern int   hypoListPublish( char* objectname);
extern int   hypoListPuts(HypoList* hypoListP, char * mode, char * name);
#endif



#ifdef __cplusplus
}
#endif
