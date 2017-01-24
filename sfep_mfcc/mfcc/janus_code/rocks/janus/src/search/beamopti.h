#ifdef __cplusplus
extern "C" {
#endif

#ifndef _beamopti
#define _beamopti

#include "search.h"

typedef struct FRTrace_S {
  float scoreA[STATESperTRELLIS];  /* cummulated score so far */ 
  int   fromA[STATESperTRELLIS];   /* index of previous state */
  float bestScore;                 /* bestScore in this trellis prior to phoneme tranis */
} FRTrace;


typedef struct Forced_S {

  /* -------- configurable parameter section ------------------------------- */

  float beamWidth;
  long transactionX;
  short treeMode;        /* simulate tree or flat pass    */
  short useBacktrace;    /* only useful in treeMode       */
  short useSegments;     /* force word boundaries in hypo */
  
  /* ------------------------------------------------------------------------*/
 
  int rTrellisN;       /* number of root trellis elements */
  RTrellis* rTrellisA; /* pointer to all the root trellises --
                         rest of words are appended to these;
                         we use root trellises as we want to adapt
                         the left context for xwt's depending on
                         which was the best word ending in previous pass*/
  
  FRTrace *** backTraceM;
  HypoList  hypoList; /* hypo along which we want to force the recognition */

  float * transiPenA;  /* transition penalty into this word */
  float ** diffScoreM;  
  
  /* the next coupue of  float matrices are the output of this pass... */
  
  FMatrix * scoreFMP;       /* float matrix for BT-score for each frame */
  FMatrix * inLastPhoneFMP; /* was BT in a last phone here -- an FMatrix ? */
  FMatrix * phoneXFMP;      /* index of phone in word -- an FMatrix ? */
  FMatrix * stateXFMP;      /* index of state in phone -- an FMatrix ? */
  FMatrix * hWordXFMP;      /* index of word in hypo -- an FMatrix ? */

} Forced;

  /* -------- configurable parameter section ------------------------------- */

extern int Forced_Init (void );

extern int forcedInit (Forced * frP, Vocab * vocabP);
extern int forcedCreate ( void );
extern float dpsForced(void* SR, char* evalS);

#endif

#ifdef __cplusplus
}
#endif
