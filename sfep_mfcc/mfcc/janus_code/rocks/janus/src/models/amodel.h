/* ========================================================================
    JANUS      Speech- to Speech Translation
               ------------------------------------------------------------
               Object: Acoustic Model
               ------------------------------------------------------------

    Author  :  Michael Finke & Ivica Rogina
    Module  :  amodel.h
    Date    :  $Id: amodel.h 512 2000-08-27 15:31:32Z jmcd $
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
    Revision 3.6  2000/08/27 15:31:07  jmcd
    Merging branch 'jtk-00-08-24-jmcd'.

    Revision 3.5.4.1  2000/08/25 22:19:42  jmcd
    Just checking.

    Revision 3.5  2000/02/04 09:47:32  soltau
    Added fwdPen copies for scale function

    Revision 3.4  1997/07/21 16:07:18  tschaaf
    gcc / DFKI Clean

    Revision 3.3  1997/07/18 18:00:34  monika
    partially rcs-clean

    Revision 3.2  1997/06/17 17:41:43  westphal
    *** empty log message ***

    Revision 2.3  96/03/11  00:01:50  finkem
     removed old evals
    
    Revision 2.2  1996/02/17  19:25:21  finkem
    added duration models and general upt to 6 state eval function

    Revision 2.1  1996/02/13  04:52:17  rogina
    added duration model index

    Revision 2.0  1996/01/31  05:06:31  finkem
    new senoneSet setup

    Revision 1.9  1995/11/17  22:23:37  finkem
    moved ptree eavesdrpping to hmm.c

    Revision 1.8  1995/10/30  00:22:38  finkem
    Introduced PTree eavesdropper

    Revision 1.7  1995/10/13  22:40:24  finkem
    amodel Phones/Tags Pointer function added

    Revision 1.6  1995/08/27  22:35:16  finkem
    *** empty log message ***

    Revision 1.5  1995/08/04  13:13:13  finkem
    *** empty log message ***

    Revision 1.4  1995/08/01  22:16:11  finkem
    *** empty log message ***

    Revision 1.3  1995/08/01  15:44:17  rogina
    *** empty log message ***

    Revision 1.2  1995/08/01  13:28:05  finkem
    *** empty log message ***

    Revision 1.1  1995/07/25  13:49:57  finkem
    Initial revision

 
   ======================================================================== */

#ifdef __cplusplus
extern "C" {
#endif

#ifndef _amodel
#define _amodel

#include "list.h"
#include "word.h"
#include "tree.h"
#include "topo.h"
#include "context.h"

/* ------------------------------------------------------------------------
    Forward Declaration
   ------------------------------------------------------------------------ */

typedef struct BPtr_S      BPtr;
typedef struct AModel_S    AModel;
typedef struct AModelSet_S AModelSet;

/* ========================================================================
    Acoustic Model Evaluation
   ======================================================================== */

typedef float (*EvalTrellis)(    AModel*,       float*, float*, BPtr*,
                                 DurationSet*);
typedef float (*EvalMpxTrellis)( AModel*, int*, float*, float*, BPtr*,
                                 DurationSet*);

typedef struct {

  EvalTrellis    eval;
  EvalMpxTrellis evalMpx;

} EvalFunc;

/* ========================================================================
    Acoustic Model
   ======================================================================== */

struct AModel_S
{
  int       stateN;   /* number of states in the graph                      */

  int*      senone;   /* one seone index for every state                    */
  int*      trans;    /* one initial transition model index for every state */

  float*    tprobA; 

  int*      fwdTrN;
  int**     fwdIdx;
  float**   fwdPen;
  float**   fwdPen_org;

  int       topoX;    /* index of the underlying topology */
  int       durX;     /* index of the used duration model */

  EvalFunc* evalFn;

};

/* ========================================================================
    Set of Acoustic Models
   ======================================================================== */

typedef struct LIST(AModel) AModelList;

struct AModelSet_S
{
  char*         name;
  int           useN;

  AModelList    list;

  Tree*         treeP;
  int           treeX;

  SenoneSet*    snsP;
  TmSet*        tmsP;

  Tree*         durP;
  int           durX;

  ContextCache *contextCache;
};

/* ========================================================================
    Module Interface
   ======================================================================== */

extern int        AModel_Init (void);

extern int        amodelSetInit(   AModelSet* amodelSet, 
                                   char *name, Tree* treeP, int treeX,
                                               Tree* durP,  int durX,
                                               int createContextCache);
extern AModelSet* amodelSetCreate( char *name, Tree* treeP, int treeX,
                                               Tree* durP,  int durX,
				               int createContextCache);
extern int        amodelSetLinkN(  AModelSet* amodelSet);
extern int        amodelSetDeinit( AModelSet* amodelSet);
extern int        amodelSetFree(   AModelSet* amodelSet);
extern int        amodelSetAdd(    AModelSet *amodelSet, int stateN, 
                                   int *senone, int *trans, int topoX, int durX);

extern Tree*      amodelSetTree      (AModelSet* amodelSet);
extern TopoSet*   amodelSetTopoSet   (AModelSet* amodelSet);
extern SenoneSet* amodelSetSenoneSet (AModelSet* amodelSet);
extern TmSet*     amodelSetTmSet     (AModelSet* amodelSet);
extern Phones*    amodelSetPhones    (AModelSet* amodelSet);
extern Tags*      amodelSetTags      (AModelSet* amodelSet);

extern int amodelSetGet( AModelSet* amP,
                         Word* wP, int left, int right);

extern int AModel_Init( void);
#endif


#ifdef __cplusplus
}
#endif
