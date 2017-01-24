/* ========================================================================
    JanusRTk   Janus Recognition Toolkit
               ------------------------------------------------------------
               Object: Language Model
               ------------------------------------------------------------

    Author  :  Michael Finke
    Module  :  lmodel.h
    Date    :  $Id: lmodel.h 2390 2003-08-14 11:20:32Z fuegen $
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
    Revision 1.4  2003/08/14 11:20:23  fuegen
    Merged changes on branch jtk-01-01-15-fms (jaguar -> ibis-013)

    Revision 1.3.20.1  2001/02/27 15:10:27  metze
    LModelNJD and LMLA work with floats

    Revision 1.3  2000/11/14 12:29:35  janus
    Merging branch jtk-00-10-27-jmcd.

    Revision 1.2.34.1  2000/11/06 10:50:34  janus
    Made changes to enable compilation under -Wall.

    Revision 1.2  2000/08/27 15:31:22  jmcd
    Merging branch 'jtk-00-08-24-jmcd'.

    Revision 1.1.4.1  2000/08/25 22:19:44  jmcd
    Just checking.

    Revision 1.1  1998/05/26 18:27:23  kries
    Initial revision


   ======================================================================== */

#ifdef __cplusplus
extern "C" {
#endif

#ifndef _lmodel_h
#define _lmodel_h

#include "list.h"
#include "array.h"

/* ========================================================================
    Forward Declarations
   ======================================================================== */

typedef unsigned short WordIdx;
typedef unsigned short NGramIdx;
typedef unsigned short LineIdx;

#define  NO_WORD  ((WordIdx)-1)
#define  NO_NGRAM ((NGramIdx)-1)

#define  SMALLPROB             -9999.0
#define  ILLEGAL_PROB           99999.0
#define  MAX_FLOATX             65534

typedef enum { LM_LINE, LM_DATA, LM_1GRAMS, LM_2GRAMS, LM_3GRAMS, LM_NGRAMS, LM_UNIGRAMS, LM_SUBS, LM_END } ArpaboLModelSection;


/* ========================================================================
    BigramItem
   ======================================================================== */

typedef struct BigramItem_S {

  WordIdx   wordX;          /* vocab word-id        */
  NGramIdx  probX;          /* pointer to probility */
  NGramIdx  backOffX;       /* back-off weight      */
  NGramIdx  tgramX;

} BigramItem;

/* ========================================================================
    TrigramItem
   ======================================================================== */

typedef struct TrigramItem_S {

  WordIdx   wordX;          /* vocab word-id        */
  NGramIdx  probX;          /* pointer to probility */

} TrigramItem;

/* ========================================================================
    FloatHeap
   ======================================================================== */

typedef struct LIST(double) FloatHeap;

/* ========================================================================
    LModelItem
   ======================================================================== */

typedef struct LModelItem_S {

  char*    name;

  float    prob[2];        /* probability     */
  float    backOff;        /* back-off weight */

  int      fbgramX;        /* first bigram index */
  int      lbgramX;        /* last  bigram index */

} LModelItem;

/* ========================================================================
    LModel
   ======================================================================== */

typedef struct LIST(LModelItem) LModelList;

typedef struct LModel_S {

  char*         name;   /* name of the language model */
  int           useN;   /* useN counter               */

  LModelList    list;

  BigramItem*   bgramA;
  int           bgramN;

  FArray        bprobA;
  FArray        bbackOffA;

  TrigramItem*  tgramA;
  int           tgramN;

  int           tgramSegSize;
  int*          tgramXA;
  int           tgramXN;

  FArray        tprobA;

  float         lz;
  float         lp;

  int           begItemX;
  int           endItemX;

  struct LModelCache_S* cacheP;

} LModel;

extern int lmodelFindBGram( LModel* lmP, WordIdx w1, WordIdx w2);
extern int LModel_Init( );

#endif

#ifdef __cplusplus
}
#endif
