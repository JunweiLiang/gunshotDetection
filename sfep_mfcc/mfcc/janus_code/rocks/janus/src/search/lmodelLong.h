/* ========================================================================
    JanusRTk   Janus Recognition Toolkit
               ------------------------------------------------------------
               Object: Language Model
               ------------------------------------------------------------

    Author  :  Klaus Ries
    Module  :  LModelLong.h
    Date    :  $Id: lmodelLong.h 700 2000-11-14 12:35:27Z janus $
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
    Revision 1.3  2000/11/14 12:32:23  janus
    Merging branch jtk-00-10-27-jmcd.

    Revision 1.2.34.1  2000/11/06 10:50:35  janus
    Made changes to enable compilation under -Wall.

    Revision 1.2  2000/08/27 15:31:23  jmcd
    Merging branch 'jtk-00-08-24-jmcd'.

    Revision 1.1.4.1  2000/08/25 22:19:45  jmcd
    Just checking.

    Revision 1.1  1998/05/26 18:27:23  kries
    Initial revision


   ======================================================================== */

#ifdef __cplusplus
extern "C" {
#endif

#ifndef _LModelLong_h
#define _LModelLong_h

#include "list.h"
#include "array.h"
#include "lmodel.h"

/* ========================================================================
    Forward Declarations
   ======================================================================== */

#define  SMALLPROB             -9999.0
#define  ILLEGAL_PROB           99999.0
#define  MAX_FLOATX             65534

/* ========================================================================
    BigramItem
   ======================================================================== */

typedef struct NgramItem_S {
  WordIdx   wordX;          /* vocab word-id        */
  NGramIdx  probX;          /* pointer to probility */
  NGramIdx  backOffX;       /* back-off weight      */
  NGramIdx  nngramX;	    /* first index of next level*/
} NgramItem;

/* ========================================================================
    TrigramItemLong
   ======================================================================== */

typedef struct TrigramItemLong_S {

  WordIdx   wordX;          /* vocab word-id        */
  NGramIdx  probX;          /* pointer to probility */

} TrigramItemLong;

/* ========================================================================
    cache
   ======================================================================== */

/*
  The hash funtcion is the wordX,cachelineX (which is the entryX of
  one node lower).
  Since entryX has to be between fbgramX and lbgramX we can check for that
  again and get a unique answer during hashing.
*/
  

typedef struct CacheItem_S {
  LineIdx	entryX;       /* the index in the original table */
  WordIdx	wordX;        /* the word being stored           */
  int           frameX;       /* -1 if no cacheline is installed */
} CacheItem;

typedef struct LIST(CacheItem) CacheHeap;

/* ========================================================================
    Lastcache
   ======================================================================== */

/*
  Same as cache, but frameX is not needed
*/
  

typedef struct LastCacheItem_S {
  LineIdx	entryX;       /* the index in the original table */
  WordIdx	wordX;        /* the word being stored           */
} LastCacheItem;

typedef struct LIST(LastCacheItem) LastCacheHeap;

/* ========================================================================
    LModelLongItem
   ======================================================================== */

typedef struct LModelLongItem_S {

  char*    name;

  float    prob[2];        /* probability     */
  float    backOff;        /* back-off weight */

  int      class;          /* The class of the word */
  float    penalty;        /* p(w|c) */

  int      frameX;         /* -1 if no cacheline is installed */
  int      nngramX;        /* first bigram index */
} LModelLongItem;

/* ========================================================================
    Ngram
   ======================================================================== */

typedef struct Ngram_S {

  NgramItem*   ngramA;
  int          ngramN;

  FArray       nprobA;
  FArray       nbackOffA;

  /* The two level addressing table for indices in the next level */

  int*          ngramXA;
  int           ngramXN;
} Ngram;

/* ========================================================================
    LModelLong
   ======================================================================== */

typedef struct LIST(LModelLongItem) LModelLongList;

typedef struct LModelLong_S {

  char*         name;   /* name of the language model */
  int           useN;   /* useN counter               */


  /* ======================================================================

             The unigram part of the model -- fixed lists

     ====================================================================== */

  LModelLongList    list;
  int           listN; /* Just to remember the expected #unigrams */

  /* ======================================================================

             The middle part of the model -- tree including backoff

     ====================================================================== */

  int           n;      /* the order of the mgram model */
  Ngram    *    ngrams;     


  /* ======================================================================

             The last part of the model -- tree excluding backoff

     ====================================================================== */

  TrigramItemLong* tgramA;
  int              tgramN;

  FArray           tprobA;

  /* ======================================================================

             General use variables

     ====================================================================== */


  float         lz;
  float         lp;

  int           begItemX;
  int           endItemX;
  int           ngramSegSize;


} LModelLong;

extern int LModelLong_Init( );

#endif

#ifdef __cplusplus
}
#endif
