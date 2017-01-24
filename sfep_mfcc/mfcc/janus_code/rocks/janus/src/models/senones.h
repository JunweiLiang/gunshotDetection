/* ========================================================================
    JANUS      Speech- to Speech Translation
               ------------------------------------------------------------
               Object: Senones
               ------------------------------------------------------------

    Author  :  Michael Finke & Ivica Rogina
    Module  :  senones.h
    Date    :  $Id: senones.h 3151 2010-02-22 23:13:39Z metze $
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
    Revision 3.12  2005/12/20 15:14:49  metze
    Code for AF training (snStreamWeights.c|h)

    Revision 3.11  2005/05/18 08:14:37  metze
    Added streamWghtAccuMode

    Revision 3.10  2003/08/14 11:20:18  fuegen
    Merged changes on branch jtk-01-01-15-fms (jaguar -> ibis-013)

    Revision 3.9.4.18  2002/11/20 13:26:19  soltau
    Removed score cache

    Revision 3.9.4.17  2002/08/30 12:31:37  soltau
    Added MLE estimation of stream weights

    Revision 3.9.4.16  2002/06/28 16:06:56  soltau
    Added snsIndex

    Revision 3.9.4.15  2002/04/22 09:38:14  soltau
    Added accu for stream weights

    Revision 3.9.4.14  2002/04/12 16:13:37  soltau
    *** empty log message ***

    Revision 3.9.4.13  2002/04/12 16:11:12  soltau
    *** empty log message ***

    Revision 3.9.4.12  2002/04/12 16:02:51  soltau
    Cleaned interface

    Revision 3.9.4.11  2002/04/03 11:07:02  soltau
    Removed unnessary declarations

    Revision 3.9.4.10  2002/02/21 07:05:36  metze
    The Header to match Hagen's Sources

    Revision 3.9.4.9  2002/01/30 09:28:44  metze
    Added opt_feat score function

    Revision 3.9.4.8  2002/01/18 13:38:58  metze
    Added SenoneSet.streamCache for opt_str score function and dump files

    Revision 3.9.4.7  2001/11/06 15:43:03  metze
    indexCache is now a char (saves memory)

    Revision 3.9.4.6  2001/09/19 19:41:51  soltau
    Made snsInlineScoreArray public available

    Revision 3.9.4.5  2001/07/06 13:12:00  soltau
    Changed compress and scoreFct handling

    Revision 3.9.4.4  2001/07/03 09:42:51  soltau
    Changed score function interface

    Revision 3.9.4.3  2001/06/28 10:48:23  soltau
    Added Load/save routines for binary files to support fast decoder startup

    Revision 3.9.4.2  2001/06/26 19:29:34  soltau
    beautified/fixed a couple of ugly hacker routines

    Revision 3.9.4.1  2001/05/25 13:23:19  soltau
    Added declaration for senoneTagArrayGetItf

    Revision 3.3  1999/08/11 15:53:46  soltau
    fast score computation

    Revision 3.2  1997/06/17 17:41:43  westphal
    *** empty log message ***

    Revision 2.5  96/09/20  14:13:03  fritsch
    *** empty log message ***
    
    Revision 2.4  1996/07/30  12:51:24  rogina
    added warp structure to senoneSet structure

    Revision 2.3  1996/04/23  19:54:11  maier
    RUNON, ready

    Revision 2.2  1996/03/05  13:03:17  rogina
    removed bogus define statement

    Revision 2.1  1996/02/05  09:30:11  manke
    Added #ifndef WINDOWS to make Visual C++ happy.

    Revision 2.0  1996/01/31  05:04:30  finkem
    new senoneSet setup

    Revision 1.8  1995/11/25  20:26:40  finkem
    added generic senones and changed file format for the senone
    description file such that the list of distributions really
    has to be a list (if more than one element you need braces)

    Revision 1.7  1995/10/04  22:28:20  finkem
    removed a couple of severe bugs and added write function
    for senones description

    Revision 1.6  1995/08/28  16:20:21  rogina
    *** empty log message ***

    Revision 1.5  1995/08/28  16:10:27  rogina
    *** empty log message ***

    Revision 1.4  1995/08/10  08:11:09  rogina
    *** empty log message ***

    Revision 1.3  1995/08/04  14:19:09  rogina
    *** empty log message ***

    Revision 1.2  1995/08/03  14:41:57  finkem
    *** empty log message ***

    Revision 1.1  1995/07/25  13:49:57  finkem
    Initial revision

 
   ======================================================================== */

#ifndef _senones
#define _senones

#include "path.h"
#include "list.h"
#include "feature.h"
#include "matrix.h"
#include "stream.h"
#include "codebook.h"

#define SNS_UPDATE 1 /* verbosity info */

/* ========================================================================
    SenoneTag
   ======================================================================== */

typedef struct SenoneTag_S {
  char* name;
  int*  tagA;
  int   tagN;
} SenoneTag;

typedef struct LIST(SenoneTag) SenoneTagList;

/* ========================================================================
    SenoneTagArray
   ======================================================================== */

typedef struct SenoneTagArray_S {
  int*                tagA;
  int                 tagN;
  struct SenoneSet_S* snsP;
} SenoneTagArray;

extern int senoneTagArrayGetItf( SenoneTagArray* staP, char* value);
extern int getSenoneTagArray   (ClientData cd, char* key, ClientData result,
				int *argcP, char *argv[]);

/* ========================================================================
    SenoneKey
   ======================================================================== */

typedef struct {
  int*                classXA;
  float*              stwA;
  int                 streamN;
  struct SenoneSet_S* snsP;
} SenoneKey;

/* ========================================================================
    Senone
   ======================================================================== */

typedef struct  {
  char*   name;
  int*    classXA;           /* distribution/neuron or whatever index array */
  float*  stwA;              /* stream weight array                         */
  double* accu;              /* stream weight accumulator                   */
  double* dccu;              /* discr stream weight accumulator (den)       */
  float   cnt;               /* training count for stream weights           */
  int     streamN;           /* number of underlying streams                */
  int     snX;               /* index of the generic senone to use          */
} Senone;

/* ========================================================================
    StreamNormItem
   ======================================================================== */

typedef struct {
  char          *name;            /* name of this normalizer */
  int           streamX;          /* stream index */
  int           histN;            /* histogram resolution */
  long          samplesN;         /* number of samples observed */
  long          minmaxN;          /* number of samples for min/max estimation */
  double        minScore;         /* minimum of all observed scores */
  double        maxScore;         /* maximum of all observed scores */
  double        *hist;            /* array of histogram counts */
  double        from;             /* range of */
  double        to;               /* histogram */
  double        *pmf;             /* array of empiric prob mass function */
} StreamNormItem;

typedef struct LIST(StreamNormItem) StreamNormList;

/* ========================================================================
    Score Array function
   ======================================================================== */

typedef struct SenoneSet_S SenoneSet;

typedef int   ssa_fct (SenoneSet *snsP, int *idxA, float *scoresA,
		       int  idxN, int frameX);
typedef float sso_fct (SenoneSet *snsP, int snX, int frameX);

/* ========================================================================
    SenoneSet
   ======================================================================== */

typedef struct LIST(Senone) SenoneList;

struct SenoneSet_S {
  char*           name;             /* name of the senone set               */
  int             useN;             /* number of references to this object  */

  SenoneList      list;             /* list of senones                      */
  SenoneTagList   tagList;          /* list of senone tags                  */
  StreamNormList  streamNormList;   /* stream normalization item list       */
  StreamArray     strA;
  Phones*         phones;
  Tags*           tags;
  int             fesN;
  FeatureSet**    fesPA;            /* pointer of the feature sets that have
                                       to be evaluated before computing the
                                       the scores.                          */

  char            commentChar;      /* sns description comment              */
  int             verbosity;        /* verbosity info tag                   */

  int             normalize;      /* flag to turn on/off score normalization */
  int             mixMode;        /* mode of mixing streams (currently 0,1,2) 
                                     0 = regular static mixing
                                     1 = static mixing with estimation
                                         of stream confidences enabled.
                                     2 = dynamic mixing using stream
                                         confidences, computed with 1  
			             3 = best of ( not normalized )
                                     NOTE: 1,2 only when normalize>0         */

  float           scoreScale;                  /* score scaling factor       */
  FMatrix         *streamWght;                 /* dynamic stream weights     */
  FMatrix         *streamCache;                /* cache for stream scores    */
  int             streamCacheX;                /* cached frame               */
  int             streamWghtAccuMode;          /* for stream weight accus    */
  int             toX;                         /* last frame to be cached    */

  double          likelihood;                  /* likelihood during training */
  CBX*            refXA;                       /* index array for score fct  */
  ssa_fct*        scoreAFctP;                  /* pointer to score function  */
  sso_fct*        scoreFctP;                   /* pointer to score function  */

};

/* ========================================================================
    Module Interface
   ======================================================================== */

extern int Senones_Init(void);

extern char*      snsName   (SenoneSet* sns, int i);
extern int        snsIndex  (SenoneSet* sns, char* name);
extern int        snsNumber (SenoneSet* sns);
extern int        snsAdd    (SenoneSet* snsP, SenoneKey* skeyP, char* name);
extern int        snsLoad   (SenoneSet* snsP, FILE* fp);
extern int        snsSave   (SenoneSet* snsP, FILE* fp);

extern int        snsFesEval (SenoneSet *snsP, char *sampleList);
extern int        snsFrameN  (SenoneSet *snsP, int  *from, int *shift, int *ready);
extern int        snsGetModel(SenoneSet* snsP, int tag, Word* wP, int left, int right);

extern float snsScore        (SenoneSet *snsP, int senoneX, int frameX);
extern int   snsScoreArray   (SenoneSet *snsP, int *idxA, float *scoresA,
                              int  idxN, int frameX);
extern int   snsInlineScoreArray  (SenoneSet *snsP, int *idxA, float *scoresA,
                                   int  idxN, int frameX);
#endif
