/* ========================================================================
    JanusRTk   Janus Recognition Toolkit
               ------------------------------------------------------------
               Object: Language Model
               ------------------------------------------------------------

    Author  :  Florian Metze
    Module  :  ngramLM.h
    Date    :  $Id: ngramLM.h 3108 2010-01-30 20:41:26Z metze $

    Remarks :  ngram Language Model for the New Janus Decoder
               Derived from LModelLong

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
    Revision 4.5  2004/10/29 09:34:07  metze
    Fixed problems with 'typedef UINT LVX'

    Revision 4.4  2004/09/22 16:26:36  fuegen
    Mapping of log(0) representation used by the SRILM-Toolkit (== -99)
    to a usable value. Can be configured with -log0 and -log0Val.

    Revision 4.3  2004/09/16 09:08:25  fuegen
    Added functionality for reading LMs with unsorted n-gram section,
    e.g. produced by the SRILM-Toolkit.

    Revision 4.2  2004/07/23 17:13:08  fuegen
    Moved Un/CompressScore macro definitions to lks.h to remove
    redefinition warnings during compilation.

    Revision 4.1  2003/08/14 11:20:05  fuegen
    Merged changes on branch jtk-01-01-15-fms (jaguar -> ibis-013)

    Revision 1.1.2.3  2002/05/03 14:01:31  metze
    Freeing of LingKS now works properly

    Revision 1.1.2.2  2002/05/02 12:00:47  soltau
    Removed include lmla.h

    Revision 1.1.2.1  2002/04/29 12:05:12  metze
    Renaming of LModelNJD to NGramLM

    Revision 1.1.2.23  2002/03/07 10:17:31  metze
    Added LCT handling with hashtable for LModelNJD

    Revision 1.1.2.22  2002/02/07 12:12:09  metze
    Changes in interface to LM score functions

    Revision 1.1.2.21  2002/02/01 14:17:20  fuegen
    moved stypes.h into slimits.h

    Revision 1.1.2.20  2002/01/19 11:38:10  metze
    Added ListReInit needed in -segSize-tolerant loading of LMs

    Revision 1.1.2.19  2002/01/14 10:30:39  metze
    Cleaned up reading/ writing of dumps

    Revision 1.1.2.18  2001/10/22 08:40:37  metze
    Changed interface to score functions

    Revision 1.1.2.17  2001/09/26 14:39:22  metze
    Made read method work for both ARPABO and dump files

    Revision 1.1.2.16  2001/07/18 15:42:55  metze
    Made quantization a run-time option

    Revision 1.1.2.15  2001/07/11 15:09:33  metze
    Added compression to bytes for all MGrams

    Revision 1.1.2.14  2001/06/13 09:43:52  metze
    Made LCT handling work on SUNs

    Revision 1.1.2.13  2001/06/01 10:07:31  metze
    Made necessary changes for decoding along lattices to work

    Revision 1.1.2.12  2001/05/23 08:10:49  metze
    PartFill included, cleaned up code

    Revision 1.1.2.11  2001/05/11 16:39:33  metze
    Cleaned up LCT interface

    Revision 1.1.2.10  2001/04/27 07:38:37  metze
    General access functions to LM via 'LCT' implemented

    Revision 1.1.2.9  2001/04/24 10:21:38  metze
    Last stable version that uses old LM-Interface

    Revision 1.1.2.8  2001/03/23 13:41:48  metze
    Compressed link arrays (two-level addressing)

    Revision 1.1.2.7  2001/03/21 12:26:25  metze
    Added option to quantize highest-order probabilities

    Revision 1.1.2.6  2001/03/17 17:35:04  soltau
    Changed LCT_NIL (to make solaris happy)

    Revision 1.1.2.5  2001/03/15 15:42:58  metze
    Re-implementation of LM-Lookahead

    Revision 1.1.2.4  2001/03/12 18:01:08  metze
    Made ScoreArray faster

    Revision 1.1.2.3  2001/03/05 08:39:42  metze
    Cleaned up treatment of exact LM in LMLA

    Revision 1.1.2.2  2001/02/27 15:10:28  metze
    LModelNJD and LMLA work with floats


   ======================================================================== */



#ifdef __cplusplus
extern "C" {
#endif

#ifndef _ngramLM
#define _ngramLM

#include <limits.h>
#include "list.h"
#include "array.h"
#include "sglobal.h"
#include "slimits.h"
#include "bmem.h"



/* ========================================================================
    Forward Declarations
   ======================================================================== */

/* Link the two-level addressing */
#define NJDGetLMLinkIdx(m,w)    lmP->mgram[(m)].linkA[(w)] + lmP->mgram[(m)].segTbl[(w)>>(lmP->segSize)]
#define NJDSetLMLinkIdx(m,w,v) { \
  unsigned int i; \
  if ((w)-(((w)>>(lmP->segSize))<<lmP->segSize) == 0) \
    lmP->mgram[(m)].segTbl[(w)>>(lmP->segSize)] = (v); \
  i = (v) - lmP->mgram[(m)].segTbl[(w)>>(lmP->segSize)]; \
  lmP->mgram[(m)].linkA[(w)] = i; \
}


#define MAXLMLINE 1024

/* -------------------------------------------------------------------------
     HashLCT
   ------------------------------------------------------------------------- */

typedef struct NJDLCT_S     NJDLCT;
typedef struct LIST(NJDLCT) NJDLCTList;

struct NJDLCT_S {
  LVX* lvxA;
  int  lvxN;
};

/* ========================================================================
    LModelNJDItem
   ---------------
    One language model word
   ======================================================================== */

typedef struct NGramLMItem_S {

  char             *name;
  struct NGramLM_S *lmP;

} NGramLMItem;



/* ========================================================================
    M-Gram structure
   ------------------
    This structure holds the information for Uni-, Bi-, Trigrams ...
    Note that the idA field won't be defined for Unigrams (m=0) and
    the backoff entry won't be defined for N-Grams (m=n-1)
   ======================================================================== */

typedef struct MGram_S {

  int                itemN; /* Number of entries in this level     */
  unsigned char*    cprobA; /* Indices for lookup table of scores  */
  lmScore*           probA; /* The table of scores                 */
  unsigned char* cbackoffA; /* Compressed table of backoff values */
  lmScore*        backoffA; /* The table of backoff values         */
  LVX*                 idA; /* The table of LM-Words for M=1...N-2 */
  unsigned int*     segTbl; /* Base offsets for M+1-gram table     */
  LVX*               linkA; /* Start item in the M+1-gram table    */
} MGram;



/* ========================================================================
     LModelNJD
   -------------
     The standard language model for the New Janus Decoder 'Ibis'
     If history > order, we have a distance (history-mgram) LM!
   ======================================================================== */

typedef struct NGramLMSubs_S { char *line; } NGramLMSubs;

typedef struct LIST(NGramLMItem) NGramLMList;
typedef struct LIST(NGramLMSubs) NGramLMSubsList;

typedef struct NGramScorePair_S { char* ngram; double prob; } NGramScorePair;
typedef struct LIST(NGramScorePair) NGramLMNGSPCacheList;

typedef struct NGramLCTAPair_S { LCT lct; int itemN; lmScore* arr;} NGramLCTAPair;
typedef struct LIST(NGramLCTAPair) NGramLMLCTAPCacheList;

struct NGramLM_S {

  /*  char*                 name; */ /* The name                        */
  /* int                    useN; */ /* useN counter                    */
  LingKS*                   lksP; /* the common information block for
                                     all linguistic knowledge sources   */

  NGramLMSubsList       subslist; /* List of substitutions in text form */
  NGramLMList               list; /* The list of word names             */
  NJDLCTList             lctList; /* List of hashed LCTs                */
  BMem*                   lctMem; /* Word indices for the hashed LCTs   */

  int                      order; /* the order of the N-Gram model      */
  int                    history; /* how long is the history we need?   */
  MGram*                   mgram; /* The different parts                */
  int                    segSize; /* segSize for two-level addressing   */
  int                       hash; /* Do we use the hashed LCTs          */
  int                   quantize;

  float                     log0; /* dummy value for log(0), e.g. for SRILM */
  float                  log0Val; /* value to which log0 is mapped during read */

  LCT                    bitmask; /* Needed for LCT handling            */
  int                   bitshift;

  NGramLMNGSPCacheList    ngspCL; /* A cache for pairs of ngram and probability defined over NGramScorePair */
  NGramLMLCTAPCacheList  lctapCL; /* Cache for pairs of LCT and Arrays of lmScores */

  void*                    mmapP; /* position and size for memory mapping */
  long int                 mmapS;
  Tcl_Channel      remoteChannel; /* A Channel for TCP/IP connections e.g. to SRI LM Server */

  // init this obj if we want marginal adaptation on the cache table (scoreArray) during decoding
  void* lda_obj;
  void* ngram_lda_obj;

};

extern int        NGramLM_Init        ();
extern int        NGramLMInit         (LingKS* lksP, NGramLM* lmP, char *name);
extern int        NGramLMDeinit                     (NGramLM *lmP);
extern int        NGramLMConfigureItf (ClientData cd, char *var, char *val);
extern ClientData NGramLMAccessItf    (ClientData cd, char *name, TypeInfo **ti);


#endif


#ifdef __cplusplus
}
#endif
