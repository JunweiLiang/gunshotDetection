/* ========================================================================
    JANUS      Speech- to Speech Translation
               ------------------------------------------------------------
               Object: Dynamic Programming Search (TreeFwd)
               ------------------------------------------------------------

    Author  :  Michael Finke
    Module  :  treefwd.h
    Date    :  $Id: treefwd.h 512 2000-08-27 15:31:32Z jmcd $
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
    Revision 3.7  2000/08/27 15:31:31  jmcd
    Merging branch 'jtk-00-08-24-jmcd'.

    Revision 3.6.2.1  2000/08/25 22:19:45  jmcd
    Just checking.

    Revision 3.6  2000/08/24 13:32:26  jmcd
    Merging changes from branch jtk-00-08-23-jmcd.

    Revision 3.5.2.1  2000/08/24 00:18:09  jmcd
    Fixed the hairy elbows Hagen left for me to find.

    Revision 3.5  2000/04/25 12:32:12  soltau
    Removed lookahead stuff

    Revision 3.4  1998/06/11 17:50:32  kries
    adaptation of new LM interface

 * Revision 3.3  1997/07/18  17:57:51  monika
 * gcc-clean
 *
    Revision 3.2  1997/06/17 17:41:43  westphal
    *** empty log message ***

    Revision 1.12  1996/11/27 15:04:51  monika
    new trellisTopN pruning

    Revision 1.11  1996/07/03 13:19:26  monika
    changes to work with 64K vocabulary

    Revision 1.10  96/07/01  14:54:06  14:54:06  monika (Monika Woszczyna)
    changes for more detailed bookkeeping
    
    Revision 1.9  96/04/23  19:51:52  19:51:52  maier (Martin Maier)
    monika: partial hypos and RUNON
    
    Revision 1.8  96/02/17  19:23:12  19:23:12  finkem (Michael Finke)
    removed old hypoList stuff and added phone duration models
    
    Revision 1.7  1996/01/18  16:50:52  monika
    removed unnecessary matrix from bactrace t.

    Revision 1.6  1996/01/14  14:18:53  monika
    FMatrix element to save bestscoreA

    Revision 1.5  96/01/01  14:22:25  14:22:25  monika (Monika Woszczyna)
    *** empty log message ***
    
    Revision 1.4  1995/09/12  19:52:34  finkem
    Handling of hypo publishing changed

    Revision 1.3  1995/08/06  19:26:23  finkem
    *** empty log message ***

    Revision 1.2  1995/08/03  14:41:17  finkem
    *** empty log message ***

    Revision 1.1  1995/07/25  13:49:57  finkem
    Initial revision

 
   ======================================================================== */

#ifdef __cplusplus
extern "C" {
#endif

#ifndef _treefwd
#define _treefwd

/* ------------------------------------------------------------------------
    Trellis   used to model all nodes and leafs in the tree structure
              except the potentially multiplexed root nodes which are
              represented by RTrellis objects.
   ------------------------------------------------------------------------ */

typedef struct Trellis_S {

  float    scoreA[STATESperTRELLIS];     /* score/probability per state    */
  BPtr     pathA[ STATESperTRELLIS];     /* backpointer per state. This is
                                            an index into BPTABLE.tableA.  */
  int      modelX;                       /* senone sequence identification */

  short    phoneX;                       /* monophone index                */

  union {
    WordX  wordX;                        /* list of words whose last phone
                                            follows this trellis. This is
                                            the first word in the list, the
                                            rest follows in the rtHPhoneXA 
                                            list */
    short  weDiId;                       /* word end context id generated
                                            when creating the list of leaf
                                            trellis objects. Negative ids.
                                            will be used to denote node
                                            trellis objects in the flat
                                            forward approach.  */
  }        info;
  WordX    wordY;                        /*lookahead wordID */

  float    lp;                           /* language model unigram penalty */
  float    lpMin;

  float    bestScore;                    /* best score within a trellis    */
  short    active;                       /* last frame in which this trellis
                                            has been active */

  struct Trellis_S* childP;  /* next trellis on the same tree level */
  struct Trellis_S* nextP;   /* child trellis or in case of the last phone
                                of a word the next alternative right context
                                trellis */

} Trellis;

/* ------------------------------------------------------------------------
    RTrellis  root trellis for the first phone of all words. In this sort
              of trellis each node has its own pointer to a senone sequence
              because we have to consider multiple left contexts.
   ------------------------------------------------------------------------ */

typedef struct RTrellis_S {

  float    scoreA[ STATESperTRELLIS];    /* score/probability per state    */
  BPtr     pathA[  STATESperTRELLIS];    /* backpointer per state. This is
                                            an index into BPTABLE.tableA   */
  int      modelXA[STATESperTRELLIS-1];  /* senone sequence identifications
                                            per state due to multiplexing  */

  short    phoneX;                       /* monophone index                */
  short    dPhoneX;                      /* diphone index                  */
  WordX    wordX;                        /* list of words whose last phone
                                            follows this trellis. This is
                                            the first word in the list, the
                                            rest follows in the rtHPhoneXA 
                                            list                           */

  float    lp;                           /* language model unigram penalty */
  float    lpMin;

  float    bestScore;                    /* best score within a trellis    */
  short    active;                       /* last frame in which this trellis
                                            has been active */

  Trellis* childP;   /* first non-root trellis level */

} RTrellis;

Trellis* dpsAllocLastPhone( WordX w);
void     dpsInitRTrellis( RTrellis* trellis, VWord* vword);
void     dpsInitTrellis( Trellis* trellis, int phoneX, int modelX);
void     dpsDeactivateTrellis( int frameX);

/* ------------------------------------------------------------------------
    TreeFwd  tree forward configuration object
   ------------------------------------------------------------------------ */

typedef struct TreeFwd_S {

  float      beamWidth;
  float      phoneBeamWidth;
  float      wordBeamWidth;
  float      lastPhoneBeamWidth;
  float      lastPhoneAloneBeamWidth;

  int        topN;                     /* evaluate top N word transitions  */

  int        trellisTopN;

  int        pruneMethod;              /* codes for experimentalstrategies */

  int        skip;                     /* codes for experimental skipping */

  int        maxFramesRunon;           /* maximal number of frames in runon mode */
  int        publishFrameN;            /* every FrameN the publish function will be called */

  HypoList   hypoList;                 /* list of hypothesis               */

} TreeFwd;

extern int  treeFwdInit(    TreeFwd* tfP, Vocab* vocabP);
extern int  treeFwdDeinit(  TreeFwd* tfP);

extern float dpsTreeForward(void* searchP,char* evalS); /* dont have Search type yet */

/* ------------------------------------------------------------------------
    SDict     dictionary containing all words in the vocabulary including
              silence, <s> and </s>. The special word are assumed to be
              the first entries in the list such that the first real word
              has the index <firstX>.
   ------------------------------------------------------------------------ */

typedef struct SDict_S {

  WordX     vocabN;       /* total number of words in the dictionary */
  VWord*    vocabA;       /* array of words                          */

  int       modelN;       /* number of transition models             */
  AModel*   modelA;       /* array of transition models              */

  DurationSet* durP;

  XWModel*  xwbModelA;
  XWModel*  xweModelA;

  short     phoneN;       /* maximum number of monophones */
  int*      phoneA;       /* array of phone model indices for fastmatch */
  short     wBoundaryX;   /* word boundary index      */

  int       xwTags;

  WordX     startWordX;   /* index of LM start word */
  WordX     finishWordX;  /* index of LM finish word */
  WordX     silenceWordX; /* index of LM silence word */
  WordX     firstX;       /* index of the first real word in dictionary */

  Trellis** trellisPA;    /* word trellis: for each word there is potentially
                             a linked list of trellis objects; one for each
                             possible right context of the word */

  int*      lmXA;         /* language model index of word            */
  char*     activeA;      /* is word active */
  WordX*    activeXA[2];  /* words active in frame */
  WordX     activeN[ 2];  /* number of active words in frame */

  WordX     sPhoneN;      /* total number single phone words in dictionary */
  WordX     sPhoneLmN;    /* number of single phone words also in LM       */
  WordX*    sPhoneXA;     /* single phone trellisPA index.  */

} SDict;

extern int  sdictInit(  SDict * sdP, Vocab* vocabP);
extern int  sdictReInit(  SDict * sdP, Vocab* vocabP);
extern int  sdictDeinit( SDict * sdP);
extern int  sdictCreate( void );
extern void sdictFree( void );

/* ------------------------------------------------------------------------
    LPhoneCand   candidate words for entering their last phone. While 
                 expanding root and node trellis object it is necessary
                 to keep a list of all words whose penultimate trellis
                 needs to be expanded. This list is later used by 
                 ExpandToLastPhone() to allocate the context dependent
                 leaf nodes.
   ------------------------------------------------------------------------ */

typedef struct LPhoneCand_S {

  WordX wordX;           /* index of the word, whose penultimate phone
                            needs to be expanded */
  float score;
  BPtr  path;            /* copy of the penult.phones path[FINALSTATE] */
  int   nextX;           /* index of the next last phone in a list of such
                            candidates [=words] which all start from the same 
                            frame */

} LPhoneCand;

/* ------------------------------------------------------------------------
    LWordTrans  since the same instance of a word (i.e.,<word,start-frame>)
                reaches its last phone several times, we can compute its 
                best BP and LM transition score info just the first time 
                and cache it for future occurrences.
   ------------------------------------------------------------------------ */

typedef struct {

  short sFrameX;               /* index of start frame                      */
  float dscore;                /* Delta-score [LM] upon entering last phone */
  BPtr  path;                  /* index into BPTable for the best path      */

} LWordTrans;

/* ------------------------------------------------------------------------
    Root object structure
   ------------------------------------------------------------------------ */

typedef struct Root_S {

  int         allocN;         /* number of allocated roots     */
  int         trellisN;       /* number of used roots          */
  RTrellis*   trellisA;       /* array of root trellis objects */
  int         nodeN;          /* number of nodes in the tree   */
  int         maxNodeN;       /* max number of nodes           */

  int         activeN[ 2];    /* number of active trellis obj. */
  Trellis**   activePA[2];    /* active trellis lists allocated as one
                                 block [free activePA[0] only !]. This
                                 list is used to keep track of all node
                                 trellis object active in current frame
                                 and of the nodes supposed to be active
                                 in the next frame */

  char        mpx;            /* use multiplexed trellis objects */

  WordX*      hPhoneXA;       /* list of words having the same model 
                                 sequence up to the last phone        */
  int*        dPhoneXA;       /* diphone to root trellis object index */

  int         lPhoneCandN;    /* number of actual last phone candidates */
  LPhoneCand* lPhoneCandA;    /* last phone candidate list with max.
                                 size = sdict->vocabN */
  LWordTrans* lWordTransA;    /* for each word in the vocabulary we store
                                 the last <start-frame,dscore,bp> item
                                 to reduce the number of LM requests,
                                 because each <word,start-frame> combination
                                 is considered only once */

} Root;

extern int  rootInit(   Root* rtP);
extern int  rootDeinit( Root* rtP);
extern int  rootCreate( void );
extern void rootFree( void );


/* ------------------------------------------------------------------------
    DistScore   search module configuration object
   ------------------------------------------------------------------------ */

typedef struct DistScore_S {
  int        distN;          /* total number of distribution   */
  short      frameX;         /* current first frame            */
  short      frameN;         /* max. number of frames          */
  char       compAllDist;    /* compute all distributions      */
  char*      activeDistA;    /* active distribution array. It 
                                is allocated, such that index -1
                                is valid, too. Pay attentention
                                when freeing the array         */

  float*     distScoreA;     /* array of distribution scores   */

  float*     activeScoreA;
  int*       activeDistXA;   /* array of active distrib IDs    */
  int        activeDistXN;

  FMatrix *  bestSenoneFMP;    /* float matrix for best score per frame */
  FMatrix *  bestScoreFMP;    /* float matrix for best score per frame */
  FMatrix *  lPBestScoreFMP;  /* for last phone best score per frame */
  FMatrix *  rTrellisFMP;     /* number of active root trellises per frame */
  FMatrix *  nTrellisFMP;     /* number of active node trellises per frame */
  FMatrix *  wTrellisFMP;     /* number of active word trellises per frame */
  FMatrix *  lTrellisFMP;     /* number of active leaf trellises per frame */
  FMatrix *  scoreCallFMP;    /* number of score computed per frame */
  SVector *  senoneMapSVP;
  
} DistScore;

extern int  distScoreInit(   DistScore* dsP);
extern int  distScoreDeinit(   DistScore* dsP);
extern int  distScoreCreate( void );
extern void distScoreFree( void );

extern void    dpsDsReset           (  short frameN);
extern void    dpsComputeTActiveDist(  short frameX);
extern void    dpsComputeFActiveDist(  short frameX);
extern int     dpsComputeDistScore(    short frameX, char* activeDist);
extern float   dpsEvaluatePhoneScores( short frameX);

/* ------------------------------------------------------------------------
    BPTbl     table of backpointers
   ------------------------------------------------------------------------ */

typedef struct BackP_S {

  WordX wordX;             /* index of the word                            */
  WordX realWordX;         /* index of real word (no silence/filler)       */
  WordX prevWordX;         /* index of previous real word. Here is the point
                              where we have to decide for one specific
                              history though it is in principal necessary
                              to consider all possible histories. If there
                              is no prev word, prevWordX = -1 [should be
                              handled by LM module as NO_WORD]             */

  BPtr  path;              /* backpointer                                  */
  float score;             /* best score of the cell over all word ends    */

  int   stackX;            /* start index for the various word end context
                              scores in BPTABLE.stackA                     */
  int   weDiX;             /* word end diphone index used when leaving the
                              word. This is used to lookup the exact score
                              in the score stack. */

} BackP;

typedef struct BPTable_S {

  short   frameN;               /* number of frames */

  BackP*  tableA;               /* sdVocabN size table column */
  WordX*  tableN;               /* size of column */ 
  BackP** tablePA;              /* forward pass lattice */

  WordX*  wordTblXA;            /* backpointer table index for any word in
                                   the current frame */
  WordX   idx;                  /* first free table entry */

  int     stackSize;            /* max size of frame score stack */
  float*  stackA;               /* frame score stack */
  int*    stackN;               /* size of score stack */
  float** stackPA;              /* score stack */

  int     topX;                 /* top of stack */

  char   overflow;              /* table or stack overflow */

} BPTable;

extern int  bpTableInit(   BPTable* bptP);
extern int  bpTableDeinit( BPTable* bptP);

extern int  bpTableCreate( short frameN);
extern void bpTableFree( void );

extern void    dpsBpNextFrame( short frameX);
extern void    dpsBpSaveFrame( short frameX, int topN);
extern void    dpsBpSort( void );

extern WordX   dpsSaveBackPointer( short frameX, WordX w, float score, 
                                   BPtr path, int   weRelDiX);
extern void    dpsSetBPRealWord(   BPtr path);
extern float   dpsBackPointerScores( BackP* bp, float z, float* plmScore);

extern float** dpsCreateBPScoreTable( short frameN, float z);
extern void    dpsFreeBPScoreTable(   float** scorePA, short frameN);

extern int dpsBacktraceHypothesis( short frameN, char repairFinal, float z,
                            HypoList* hlP);

extern int TreeFwd_Init (void);

extern int wordTransC;   /* number of word transitions: # of frames   */
extern int wordTransN;   /* number of word transitions: # endpoints   */
extern int wordTransM;   /* number of word transitions: max # points  */

/* ------------------------------------------------------------------------
    Global Variables
   ------------------------------------------------------------------------ */

extern float bestScore;
extern float bestPhoneScore;
extern float bestTotalScore;
extern float lastPhoneBestScore;

extern int   compDistN;
extern int   compDistPerFrameN;

extern int   rtrellisEvalN;
extern int   ntrellisEvalN;
extern int   ltrellisEvalN;
extern int   wtrellisEvalN;

extern int   zeroA[MAX_MONOPHONES];

#endif

#ifdef __cplusplus
}
#endif
