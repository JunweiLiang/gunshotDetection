/* ========================================================================
    JanusRTk   Janus Recognition Toolkit
               ------------------------------------------------------------
               Object: Language Model Cache
               ------------------------------------------------------------

    Author  :  Michael Finke
    Module  :  lmCache.h
    Date    :  $Id: lmodelCache.h 512 2000-08-27 15:31:32Z jmcd $
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
    Revision 1.2  2000/08/27 15:31:22  jmcd
    Merging branch 'jtk-00-08-24-jmcd'.

    Revision 1.1.4.1  2000/08/25 22:19:45  jmcd
    Just checking.

    Revision 1.1  1998/05/26 18:27:23  kries
    Initial revision


   ======================================================================== */

#ifdef __cplusplus
extern "C" {
#endif

#ifndef _lmCache_h
#define _lmCache_h

#include "list.h"
#include "array.h"

/* ========================================================================
    LModelCacheTgItem    trigram cache item
   ======================================================================== */

typedef struct LModelCacheTgItem_S {

  WordIdx                     wordX;   /* index of word w2 */
  int                         lineX;   /* cache line       */

  int                         ftgramX; /* index of first trigram    */
  int                         ltgramX; /* index of last  trigram    */

  float                       backOff; /* backOff weight for bigram */

  struct LModelCacheTgItem_S* nextP;

} LModelCacheTgItem;

/* ========================================================================
    LModelCacheBgItem    bigram cache item
   ======================================================================== */

typedef struct LModelCacheBgItem_S {

  int                lineX;
  LModelCacheTgItem* tgItemP;

} LModelCacheBgItem;

/* ========================================================================
    LModelCacheItem
   ======================================================================== */

typedef struct LModelCacheItem_S {

  WordIdx            wordX[2];     /* wordX[0] == NO_WORD -> line inactive */
                                   /* wordX[1] == NO_WORD -> bigram probs  */
  int                frameX;       /* frame in which last used             */

  float*             probA;
  int                probN;        /* valid number of probs in prob.cache  */
  LModelCacheTgItem* tgItemP;

} LModelCacheItem;

/* ========================================================================
    LModelCache
   ======================================================================== */

typedef struct LModelCache_S {

  struct LModel_S*   lmodelP;   /* pointer to LModel */

  LModelCacheItem*   itemA;
  int                itemN;

  LModelCacheBgItem* bgCacheA;  /* wordN bigram cache array        */
  float*             probA;     /* wordN * itemN probability cache */
  int                wordN;     /* size of word list in LM         */

  int                frameX;    /* index of the current frame      */

} LModelCache;

/* ========================================================================
    Public Function
   ======================================================================== */

extern LModelCache* lmodelCacheCreate( struct LModel_S* lmP, 
                                       int itemN, int wordN);

extern int lmodelCacheReset(  LModelCache* lmcP);
extern int lmodelCacheDeinit( LModelCache* lmcP);
extern int lmodelCacheFree(   LModelCache* lmcP);

extern float lmodelCacheUgScore( LModelCache* lmcP, WordIdx w1);
extern float lmodelCacheBgScore( LModelCache* lmcP, WordIdx w1, WordIdx w2);
extern float lmodelCacheTgScore( LModelCache* lmcP, WordIdx w1, WordIdx w2, 
                                                    WordIdx w3);

#endif

#ifdef __cplusplus
}
#endif
