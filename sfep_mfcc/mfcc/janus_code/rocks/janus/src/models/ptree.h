/* ========================================================================
    JANUS      Speech- to Speech Translation
               ------------------------------------------------------------
               Object: Polyphone Tree
               ------------------------------------------------------------

    Author  :  Michael Finke
    Module  :  ptree.h
    Date    :  $Id: ptree.h 2390 2003-08-14 11:20:32Z fuegen $
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
    Revision 4.5  2003/08/14 11:20:16  fuegen
    Merged changes on branch jtk-01-01-15-fms (jaguar -> ibis-013)

    Revision 4.4.20.1  2002/02/28 15:46:05  soltau
    declared ptreeSetDeinit

    Revision 4.4  2000/11/14 12:35:25  janus
    Merging branch jtk-00-10-27-jmcd.

    Revision 4.3.34.1  2000/11/06 10:50:39  janus
    Made changes to enable compilation under -Wall.

    Revision 4.3  2000/08/27 15:31:27  jmcd
    Merging branch 'jtk-00-08-24-jmcd'.

    Revision 4.2.4.1  2000/08/25 22:19:45  jmcd
    Just checking.

    Revision 4.2  2000/01/12 10:12:49  fuegen
    add fast clustering and verbosity

    Revision 4.1  1999/04/19 20:55:25  fuegen
    *** empty log message ***

    Revision 3.2  1997/06/17 17:41:43  westphal
    *** empty log message ***

    Revision 2.2  96/02/26  06:11:29  finkem
    added a couple of prototypes
    
    Revision 2.1  1996/02/11  06:19:57  finkem
    added a couple pf fundtion and made find question a lot
    faster by not really splitting trees but reading the
    model out of the tree

    Revision 2.0  1996/01/31  05:07:03  finkem
    new senoneSet setup

    Revision 1.1  1995/10/30  00:24:59  finkem
    Initial revision

 
   ======================================================================== */

#ifdef __cplusplus
extern "C" {
#endif

#ifndef _ptree
#define _ptree

#include "list.h"
#include "word.h"
#include "modelSet.h"
#include "questions.h"

/* ------------------------------------------------------------------------
    Forward Declaration
   ------------------------------------------------------------------------ */

/* ========================================================================
    PTreeNode
   ======================================================================== */

typedef struct PTreeNode_S
{
  int    phoneA[2];         /* left and right phone                         */
  int    tagA[2];           /* left and right tag                           */
  float  count;             /* number of polyphones going through this node */
  int    model;             /* model index modelling the polyphones         */

  struct PTreeNode_S* nextP;
  struct PTreeNode_S* childP;

} PTreeNode;

/* ========================================================================
    PTree
   ======================================================================== */

typedef int    (GetIndexFn)( ClientData CD, char* name);
typedef char*  (GetNameFn)(  ClientData CD, int i);

typedef struct
{
  char*         name;
  float         count;
  int           maxContext;
  char*         addProc;

  Phones*       phones;
  Tags*         tags;

  ModelSetPtr   mdsP;

  struct PTreeNode_S* childP;

} PTree;

extern int    ptreeInit(   PTree* TN, char* name);
extern PTree* ptreeCreate( char* name, Phones* phones, Tags* tags,
                                       ModelSetPtr* mdsP);
extern int    ptreeDealloc( PTree* TN);
extern int    ptreeFree(    PTree* TN);

extern float  ptreeWrite(  PTree* ptrP, float minCount, FILE* fp);
extern float  ptreeList(   PTree* ptP,  float minCount);

extern PTreeNode* ptreeAdd( PTree *ptrP, Word* wP, 
                                         int   leftContext, int  rightContext,
                                         float count,       int  model,
                                         int*  modelP);
extern PTreeNode* ptreeGet( PTree *ptrP, Word* wP, 
                                         int   leftContext, int  rightContext,
                                         int*  modelP);

extern float ptreeModels(PTree*  ptrP, float minCount, ModelArray* mdaP);
extern float ptreeSplit( PTree*  ptrP, float minCount, Question*   quP,
                         PTree** yesP, PTree** noP, PTree** unkP);
extern float ptreeMerge( PTree*  ptrP, float minCount, PTree*  ptrP2);

extern int   ptreeFindQuestion( PTree* ptrP, QuestionSet* QS, float  minCount,
                                                              float* score);
extern int   ptreeFindQuestionFast (PTree *ptrP, QuestionSet *QS, float minCount,
				    float *score, int *excludeA);

/* ========================================================================
    PTreeSet
   ======================================================================== */

typedef struct LIST(PTree) PTreeList;

typedef struct
{
  char*         name;
  int           useN;
  PTreeList     list;

  Phones*       phones;
  Tags*         tags;

  ModelSetPtr   mdsP;

  char          commentChar;

} PTreeSet;

extern int ptreeSetAdd( PTreeSet *ptree, char* name);
extern int ptreeSetInit( PTreeSet* treeSet, char* name, Phones* phones,
			 Tags* tags, ModelSetPtr* mdsP);
extern int ptreeSetDeinit( PTreeSet* ptree);
extern int PTree_Init ();

#endif

#ifdef __cplusplus
}
#endif
