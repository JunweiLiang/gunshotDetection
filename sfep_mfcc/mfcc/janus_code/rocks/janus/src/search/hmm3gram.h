/* ========================================================================
    JANUS      Speech- to Speech Translation
               ------------------------------------------------------------
               Object: HMM of 3-grams
               ------------------------------------------------------------

    Author  :  Klaus Ries
    Module  :  hmm3gram.h
    Date    :  $Id: hmm3gram.h 700 2000-11-14 12:35:27Z janus $
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
    Revision 1.5  2000/11/14 12:29:33  janus
    Merging branch jtk-00-10-27-jmcd.

    Revision 1.4.34.1  2000/11/06 10:50:29  janus
    Made changes to enable compilation under -Wall.

    Revision 1.4  2000/08/27 15:31:17  jmcd
    Merging branch 'jtk-00-08-24-jmcd'.

    Revision 1.3.4.1  2000/08/25 22:19:44  jmcd
    Just checking.

    Revision 1.3  1998/08/02 11:21:21  kries
    produce meaningful representations in lattice generation

 * Revision 1.2  1998/06/11  14:14:26  kries
 * cosmetique changes
 *
 * Revision 1.1  1998/05/26  18:27:23  kries
 * Initial revision
 *
 
   ======================================================================== */

#ifdef __cplusplus
extern "C" {
#endif

#ifndef _hmm3gram
#define _hmm3gram

#include "vocab.h"
#include "list.h"
#include "array.h"
#include "hash.h"

/* -------------------------------------------------------------------------

   The HMM of 3grams is a model where one hidden state is generating
   sequences of words.
   The hidden state can be an utterance type, for example

  -------------------------------------------------------------------------*/


/* ------------------------------------------------------------------------    
  HMM3gramLatticeNode
   ------------------------------------------------------------------------ */

typedef struct HMM3gramLatticeNode_S { 
  int beginTime, endTime;
  VWordX state;
  float score;
  int final;
  char * rep;
} HMM3gramLatticeNode;

/* ------------------------------------------------------------------------    
  HMM3gramSubState
   ------------------------------------------------------------------------ */

typedef enum {
  HMM3gramWord, HMM3gramStay, HMM3gramTerminatedSpeechAct, HMM3gramInitial
} HMM3gramSubState;

/* ------------------------------------------------------------------------    
  HMM3gramState
   ------------------------------------------------------------------------ */

typedef struct HMM3gramState_S { 
  VWordX stateX0, stateX1, stateX2;
  unsigned short lastChange;
  int timeStamp;
  HMM3gramSubState subState;
} HMM3gramState;

typedef struct LIST(HMM3gramState) HMM3gramStateList;

/* ------------------------------------------------------------------------    
  HMM3gramVocabItem
   ------------------------------------------------------------------------ */

typedef struct HMM3gramVocabItem_S {
  char*    name;
  int*     indexA;
} HMM3gramVocabItem;

/* ------------------------------------------------------------------------    
  HMM3gramModel
   ------------------------------------------------------------------------ */

typedef struct HMM3gramModel_S {
  LMPtr lm;
  float mean;
  float stddev;
} HMM3gramModel;

typedef struct LIST(HMM3gramVocabItem) HMM3gramVocab;

/* ------------------------------------------------------------------------
    
   HMM3gram

   ------------------------------------------------------------------------ */

typedef struct HMM3gram_S { 
  char*     name;
  int       useN;

  float	    lp, lz;        /* Standard LM parameters */
  int       startStateX, finalStateX;
  int       cutoffN;       /* for passing to submodels */
  int       hmmStates;     /* Number of states to be kept in context (1==bigram==default) */

  int       statesN;       /* #State  (as in hmm3grams) */
  HMM3gramState       currentState;       
  LMStateTransitionArray nextStates;

  int       timeStamping;  /* Should the state be indexed with its beginning time ? */

  LMPtr     hmm3grams;     /* A bigram describing HMM trans. probabilities */
  HMM3gramModel * TriGrams;      /* The trigrams describing each state */

  HMM3gramVocab vocab;
  int       vocabStartX;
  int       vocabFinalX;

  /* Lattice generation */

  int       generateLattice;
  Hash*     latticeNodes;

  
} HMM3gram;

LM HMM3gramLM;

/* ------------------------------------------------------------------------
    Function Declarations
   ------------------------------------------------------------------------ */
extern char* HMM3gramSubStateString(HMM3gramSubState subState);
extern int   hmm3gram_Init();

#endif

#ifdef __cplusplus
}
#endif
