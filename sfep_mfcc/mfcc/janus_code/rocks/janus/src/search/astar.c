/* ========================================================================
    JANUS      Speech- to Speech Translation
               ------------------------------------------------------------
               Object: A^* Search
               ------------------------------------------------------------

    Author  :  Klaus Ries
    Module  :  astar.c
    Date    :  $Id: astar.c 3418 2011-03-23 15:07:34Z metze $
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
    Revision 1.8  2003/08/14 11:20:21  fuegen
    Merged changes on branch jtk-01-01-15-fms (jaguar -> ibis-013)

    Revision 1.7.20.1  2001/11/19 14:46:55  metze
    Go away, boring message!

    Revision 1.7  2000/11/14 12:01:09  janus
    Merged branch jtk-00-10-27-jmcd.

    Revision 1.6.26.2  2000/11/08 17:04:29  janus
    Finished making changes required for compilation under 'gcc -Wall'.

    Revision 1.6.26.1  2000/11/06 10:50:23  janus
    Made changes to enable compilation under -Wall.

    Revision 1.6  2000/09/14 17:47:12  janus
    Merging branch jtk-00-09-14-jmcd.

    Revision 1.5.30.1  2000/09/14 16:20:55  janus
    Merged branches jtk-00-09-10-tschaaf, jtk-00-09-12-hyu, and jtk-00-09-12-metze.

    Revision 1.5.28.1  2000/09/14 16:00:41  janus
    Incorporated bug fixes from Klaus Ries.

    Revision 1.5  1998/08/02 11:09:10  kries
    new tie-breaker and changes to nodeTime

    Revision 1.2  1998/06/11 15:45:40  kries
    changes for lat - class

 * Revision 1.1  1998/05/26  18:38:33  kries
 * Initial revision
 *

 
   ======================================================================== */

#include "astar.h"
#include "search.h"
#include "math.h"
#include "treefwd.h"
#include "lm.h"
#include "lattice.h"
#include "lat.h"

/*
#define DEBUG 3
*/


char astarRcsVersion[]= "@(#)1$Id: astar.c 3418 2011-03-23 15:07:34Z metze $";

/* ========================================================================
   A^* Lattice Search -- hidden state LMs and n-best extraction
   ======================================================================== */

static long astar_prime  = 1048571;


/* ------------------------------------------------------------------------
    AStarPathCompare
   ------------------------------------------------------------------------ */

/* ------------------------------------------------------------------------
    AStarPathDelete   delete a path but respect counts
   ------------------------------------------------------------------------ */

static void AStarPathFree(AStar* searchObjectP,  AStarPath* pathP) {
  if(pathP && --(pathP->useN)<=0) {
    LMPtr * lmP = &(searchObjectP->lm);
    AStarPathFree(searchObjectP,pathP->parentP);
    pathP->parentP=searchObjectP->free_list;
    searchObjectP->free_list=pathP;
    if(lmP->lmP->stateMemFreeFct && pathP->StateMem)
      lmP->lmP->stateMemFreeFct(lmP->cd, pathP->StateMem);
    pathP->StateMem = NULL;
 }
}

static AStarPath* AStarPathCreate(AStar* searchObjectP,AStarPath * parentP, AStarNodeP AStarElement, LMState State) {
  LMPtr * lmP = &(searchObjectP->lm);
  AStarPath* npathP;

  if(!searchObjectP->free_list) {
    AStarPathMemory* mem = (AStarPathMemory*)malloc(sizeof(AStarPathMemory));
    int i;
    int blocking = searchObjectP->mem_list ?  searchObjectP->mem_list->size*2 : INITASTARMEMORYBLOCKING;
    
    if(mem==0)
      FATAL("Not enough memory to allocate path memory header structure\n");
    mem->next = searchObjectP->mem_list;
    searchObjectP->mem_list = mem;

    if(blocking>MAXASTARMEMORYBLOCKING)
      blocking = MAXASTARMEMORYBLOCKING;

    mem->size = blocking;
    mem->paths=(AStarPath*)calloc(blocking,sizeof(AStarPath));
    if(!mem->paths) FATAL("Not enough memory to allocate paths\n");

    for(i=0;i<blocking-1;i++)
      mem->paths[i].parentP = &(mem->paths[i+1]);      
    mem->paths[i].parentP =  searchObjectP->free_list;      
    searchObjectP->free_list = &(mem->paths[0]);

    for(i=0;i<blocking;i++) {
      mem->paths[i].AStarElement=NULL; mem->paths[i].StateMem=NULL;      
    }
  }
  npathP=searchObjectP->free_list; searchObjectP->free_list=searchObjectP->free_list->parentP;
  assert(npathP);

  npathP->useN=1;
  if((npathP->parentP = parentP))
    parentP->useN++;
  npathP->AStarElement = AStarElement;
  
  if(lmP->lmP->stateMemCreateFct) {
    if(lmP->lmP->stateMemFreeFct && npathP->StateMem)
      lmP->lmP->stateMemFreeFct(lmP->cd, npathP->StateMem);
    npathP->StateMem = lmP->lmP->stateMemCreateFct(lmP->cd, State);
  }
      
  return npathP;
}

#define element(PATH) (searchObjectP->nodeElement(searchObjectP,PATH->AStarElement))

static int AStarPathEqual( AStar * searchObjectP, AStarPath* pP1,  AStarPath* pP2) {

  if(searchObjectP->context<0) {    
    while(pP1!=pP2) {
      if((pP1==0) || (pP2==0) || (element(pP1) != element(pP1))) 
	return 0;
      pP1 = pP1->parentP; pP2 = pP2->parentP;
    }
  } else {
    int context=searchObjectP->context-2;

    while((pP1!=pP2) && context>=0) {
      if((pP1==0) || (pP2==0) || (element(pP1) != element(pP1))) 
	return 0;
      pP1 = pP1->parentP; pP2 = pP2->parentP;
      context--;
    }
  }

  return 1;
}


/* ------------------------------------------------------------------------
   ------------------------------------------------------------------------

   Hash and equality relations on search paths that can be
   backtracked

   The history can be restricted in length ( functions with "Context") 
   or not (which is good for nbest extraction and some algorithms
   that need he entire history such as cache models

   ------------------------------------------------------------------------
   ------------------------------------------------------------------------ */



/* ------------------------------------------------------------------------
    AStarPathEqual    determines whether two pathes contain the same
                        suffix of lenght context
   ------------------------------------------------------------------------ */


static AStarPath ** AStarBacktrace2(AStar* searchObjectPtr, AStarPath* path, int * hypoSize,int depth);
static AStarPath ** AStarBacktrace(AStar* searchObjectPtr, AStarNode* nodeP, int * hypoSize);


#if defined(DEBUG) && DEBUG>2

static void AStarPrintStack( AStar * searchObjectP) {
   RBT_Node* P;
   int hypoSize;
   int i, stack=1;
   char str[1000000];
   LMPtr * lmPtr = &(searchObjectP->lm);
   int speechlattice=0;

   printf("Stack (%ld) ==========================================================================================\n",searchObjectP->boundedQueueP->nodesN);
   for(P=RBT_FindHigh(searchObjectP->boundedQueueP);P;P=RBT_FindPrev(searchObjectP->boundedQueueP,P)) {
     AStarNode * nodeP = (AStarNode *)(P->Data);     
     if(nodeP) {
       AStarPath ** hypoP = AStarBacktrace2(searchObjectP, nodeP->pathP, &hypoSize,0);
       float score = nodeP->score;
       float pathScore = nodeP->pathScore;
       float lookahead = searchObjectP->nodeLookahead(searchObjectP,nodeP);

       str[0]='\000';
       if(searchObjectP->lm.lmP->statePrintFct) { /* Copy object state */
	 char stateString[100000];
	 searchObjectP->lm.lmP->statePrintFct( searchObjectP->lm.cd,nodeP->State, stateString);
	 strcat(str,stateString); strcat(str,"\n");
       }
       for(i=0;i<hypoSize;i++) {
	 char stateString[100000];
	 if(searchObjectP->lm.lmP->stateMemPrintFct) { /* Copy object state */
	   searchObjectP->lm.lmP->stateMemPrintFct( searchObjectP->lm.cd, hypoP[i]->StateMem, stateString);
	   strcat(str,"<<"); strcat(str,stateString); strcat(str,">> ");
	 }	
	 if(speechlattice) {
	   LatNode * nodeP =  (LatNode*) hypoP[i]->AStarElement;
	   strcat(str,lmPtr->lmP->nameFct( lmPtr->cd, nodeP->lmWordX)); strcat(str," ");
	 }
	 else {
	   LatSearchNode       * lP = (LatSearchNode *)hypoP[i]->AStarElement;
	   if(lP==(LatSearchNode *)searchObjectP)
	     strcat(str," [EOS] ");
	   else {
	     sprintf(stateString,"%d %ld %s [startTimeState] ",lP->state.parentP->channelX,lP->state.parentP->startFrameX,lmPtr->lmP->nameFct( lmPtr->cd, lP->state.stateX));
	     strcat(str,stateString);
	   }
	 }
       }
       if(!speechlattice) {
	 LatSearchNode       * lP = (LatSearchNode *)nodeP->AStarElement;
	 LatSubState       subState = ((LatSearchState *)nodeP->searchState)->subState;
	 LNodeTime       * timeP  = NULL;
	 LNodeState      * stateP = NULL;
	 LNodeTimeList   * timeLP = NULL;
	 char              String[100000];

	 if(lP==(LatSearchNode *)searchObjectP)
	   subState = LatSubState_EOS;

	 switch(((LatSearchState *)nodeP->searchState)->subState) { /* transition function */
	 case LatSubState_startTime:
	   sprintf(String,"%d %ld [startTime]",lP->time.channelX,lP->time.startFrameX);
	   break;
	 case LatSubState_startTimeState:	
	   timeP = lP->state.parentP;
	   sprintf(String,"%d %ld %s [startTimeState]",timeP->channelX,timeP->startFrameX,lmPtr->lmP->nameFct( lmPtr->cd, lP->state.stateX));
	   break;
	 case LatSubState_startTimeStateEndTime:
	   if(lP) {
	     timeLP = &(lP->timeL); stateP = timeLP->parentP ; timeP = stateP->parentP;
	     sprintf(String,"%d %ld %s %ld [startTimeStateEndTime]",timeP->channelX,timeP->startFrameX,lmPtr->lmP->nameFct( lmPtr->cd, stateP->stateX),timeLP->endFrameX);
	   }
	   else {
	     sprintf(String,"[[latinit]]");
	   }
	   break;
	 case LatSubState_EOS:
	   sprintf(String," [EOS]");
	   break;
	 case LatSubState_FINAL:
	   sprintf(String,"[FINAL]");
	   break;	   
	 }
	 strcat(str,String); strcat(str," ");
       }	 

       printf("Stack [%ld] scoretotal=%f score=%f lookahead=%f:\n%s\n",stack++,score,pathScore,lookahead,str);
       free(hypoP);
     }
     else {
       printf("Stack [%ld] Initial state\n",stack);
     }       
     /* break; */
   }
}
  
#endif


 /* ------------------------------------------------------------------------
    AStarPathEqual    determines whether two pathes contain the same
                        suffix of lenght context
   ------------------------------------------------------------------------ */

static int AStarNode_is_less( AStar* searchObjectP, AStarNode*  p1, AStarNode* p2) {
  if(p1==p2)                     return 0;
  if(p1==0)                      return 0;
  if(p2==0)                      return 1;

  return p1->score  > p2->score;
}
    
static long AStarNodeHash(AStar * searchObjectP, AStarNode* nodeP) {
  return nodeP ? nodeP->hashValue : 0;
}

static int AStarNodeEqual( AStar * searchObjectP, AStarNode* pP1,  AStarNode* pP2) {
  LMPtr* lmP = &(searchObjectP->lm);

  if(pP1==pP2)                                                return 1;
  if((pP1==NULL) || (pP2==NULL))                              return 0;

  if(pP1->AStarElement!=pP2->AStarElement)                    return 0;
  
  if(searchObjectP->searchStateEq &&
     !searchObjectP->searchStateEq(searchObjectP, pP1->searchState,  pP2->searchState)) return 0;

  if(lmP->lmP->stateEqFct && !lmP->lmP->stateEqFct(lmP->cd,pP1->State,pP2->State))
    return 0;

  if(!AStarPathEqual(searchObjectP, pP1->pathP,  pP2->pathP)) return 0;

  return 1;
}

/* ------------------------------------------------------------------------
   AStarPathHash  calculate hash-key for AStarPath fast
   AStarPathHashCalculate make initial calculation
   ------------------------------------------------------------------------ */

static long AStarNodeHashCalculate(AStar * searchObjectP, AStarNode* nodeP, long dpHashIn, int dpChange) {
  LMPtr* lmP = &(searchObjectP->lm);

  if(nodeP==NULL) return 0;

  nodeP->hashValue  = (long)nodeP->AStarElement;
  nodeP->hashValue %= astar_prime;

  if(searchObjectP->context<0) {
    if(dpChange && nodeP->pathP) {
      nodeP->un.hashValueDynamic  = (dpHashIn<<4) + element(nodeP->pathP);
      nodeP->un.hashValueDynamic %= astar_prime;
    }
    else {
      nodeP->un.hashValueDynamic  = dpHashIn;
    }
    nodeP->hashValue  = (nodeP->hashValue<<4) + nodeP->un.hashValueDynamic;
    nodeP->hashValue %= astar_prime;
  }
  else {
    int context = searchObjectP->context-1;
    AStarPath* monkey=nodeP->pathP;
    while(context>=0 && monkey) {
      nodeP->hashValue  = (nodeP->hashValue<<4) + element(monkey);
      nodeP->hashValue %= astar_prime;
      context--;
    }
    nodeP->un.hashValueDynamic  = 0;
  }

  if(searchObjectP->searchStateHash) {
    nodeP->hashValue  = (nodeP->hashValue<<4) + searchObjectP->searchStateHash(searchObjectP,nodeP->searchState);
    nodeP->hashValue %= astar_prime;
  }
  
  if(lmP->lmP->stateHashFct) {
    nodeP->hashValue  = (nodeP->hashValue<<4) + lmP->lmP->stateHashFct(lmP->cd,nodeP->State);
    nodeP->hashValue %= astar_prime;
  }  

  return nodeP->hashValue;
}

/* ------------------------------------------------------------------------
    AStarPathCreate   create a path but respect counts
   ------------------------------------------------------------------------ */
static void AStarNodeFree( AStar* searchObjectP, AStarNode* nodeP) {
  if(nodeP) {
    AStarPathFree(searchObjectP,nodeP->pathP);
    
    nodeP->un.nextFree = searchObjectP->free_node_list;
    searchObjectP->free_node_list = nodeP;
  }
}

static AStarNode * AStarNodeCreate( AStar* searchObjectP, AStarNode* parentP, AStarNodeP AStarElement, AStarSearchStateP nextSearchState, float score, LMState nextState, int extend) {

  LMPtr* lmP = &(searchObjectP->lm);

  AStarNode* nnodeP = searchObjectP->free_node_list;

  if(!nnodeP) FATAL("Node list exceeded, no free nodes\n");
  searchObjectP->free_node_list = nnodeP->un.nextFree;

  nnodeP->AStarElement    = AStarElement;

  if(extend) {
    nnodeP->pathP = AStarPathCreate(searchObjectP,parentP ? parentP->pathP : NULL,AStarElement,nextState);
  }
  else {
    nnodeP->pathP = parentP ? parentP->pathP : NULL;
    if(nnodeP->pathP) parentP->pathP->useN++;
  }


  if(lmP->lmP->copyStateFct) { /* Copy LM state */
    if((!nnodeP->State) && lmP->lmP->stateCreateFct) /* Allocate memory if necessary */
      nnodeP->State = lmP->lmP->stateCreateFct(lmP->cd);
    lmP->lmP->copyStateFct( lmP->cd, nnodeP->State, nextState);
  }

  if(searchObjectP->searchStateCopy) { /* Copy LM state */
    if((!nnodeP->searchState) && searchObjectP->searchStateCreate) /* Allocate memory if necessary */
      nnodeP->searchState = searchObjectP->searchStateCreate(searchObjectP);
    searchObjectP->searchStateCopy( searchObjectP, nnodeP->searchState, nextSearchState);
  }

  nnodeP->pathScore       = score;
  nnodeP->score           = score + searchObjectP->nodeLookahead(searchObjectP, nnodeP);

  AStarNodeHashCalculate(searchObjectP, nnodeP, parentP ? parentP->un.hashValueDynamic : 0, extend);

  return nnodeP;
}

AStarSearchStateP AStarArcSearchStateNULL(AStar *, AStarArcP);

static int AStarNodeCreateNPush( AStar* searchObjectP, AStarNode * parentP, AStarArcP arcP, IArray * warray, float score, LMState nextState, int extend) {
  AStarNode* nnodeP;
  AStarNodeP NodeP = arcP ? searchObjectP->arcNode(searchObjectP, arcP) : (parentP ? parentP->AStarElement : NULL);
  RBT_Node * stackNodeP;

  if(warray) {    /* Assume the LMState is set if there is any hidden state involved */
    assert(arcP);
    warray->itemA[warray->itemN-1] = searchObjectP->nodeElement(searchObjectP,NodeP);
    score += searchObjectP->lm.lmP->scoreFct(searchObjectP->lm.cd , warray->itemA,  warray->itemN, warray->itemN-1);
  }

  if(arcP)
    score  += searchObjectP->arcTransition(searchObjectP, arcP);


  assert( (!extend) || NodeP);

  if(searchObjectP->arcSearchState!=AStarArcSearchStateNULL) {
    nnodeP = AStarNodeCreate( searchObjectP, parentP, NodeP,
			      arcP ? searchObjectP->arcSearchState(searchObjectP, arcP) : (parentP ? parentP->searchState : NULL),
			      score,
			      nextState ? nextState : (parentP ? parentP->State : NULL),
			      extend);
  }
  else {
    nnodeP = AStarNodeCreate( searchObjectP, parentP, NodeP, NULL, score,
			      nextState ? nextState : (parentP ? parentP->State : NULL),
			      extend);
  }



  stackNodeP = RBT_InsertNode_(searchObjectP->boundedQueueP, (ClientData)nnodeP,NULL);
  searchObjectP->hypTriedN++;  
  return (stackNodeP!=RBT_NIL) && (stackNodeP->Data==(ClientData)nnodeP);
}  

    
static int AStarNodeCreateFollower( AStar* searchObjectP, AStarNode* nodeP) {

  int i;
  int states=0;

  IArray warray;
  float score;
  int   time;

  AStarPath *monkey;
  AStarArcP arcP;

  int wordA[1024];
  int wN=5;

  LMPtr * lmP = &(searchObjectP->lm);

  warray.itemA = wordA;
  warray.itemN = wN;

  /* For the LM, notify that something was popped from the stack */

  if(lmP->lmP->pathCreateFct!=NULL)
    lmP->lmP->pathCreateFct(lmP->cd,searchObjectP,nodeP);
    
  score = nodeP ? nodeP->pathScore : 0;
  time  = searchObjectP->nodeTime(searchObjectP,nodeP);

  if (nodeP && searchObjectP->pathEnd(searchObjectP,nodeP))  { /* Final node reached */
    /* nodeP->pathP = AStarPathCreate(searchObjectP, nodeP->pathP, nodeP->AStarElement, nodeP->State); */
    return   RBT_InsertNode(searchObjectP->outputQueueP, (ClientData)nodeP);
  }

  assert(wN>1);

  /* Add links */

  if(searchObjectP->skipOutputArcs!=0 && searchObjectP->skipOutputArcs(searchObjectP, nodeP)) {
    /* The lattice does not allow the next item to be passed to the LM */
    for (arcP=searchObjectP->nodeOutputArcs(searchObjectP, nodeP);arcP;arcP = searchObjectP->nextOutputArcs(searchObjectP,arcP))
      states += AStarNodeCreateNPush(searchObjectP, nodeP,  arcP, NULL, score, NULL, 0);
  }
  else {
    /* Calculate history */
    i = wN-2;
    if(nodeP) {
      monkey = nodeP->pathP;
      
      while(monkey!=0 && monkey->AStarElement!=0 && i>=0) {
	wordA[i--]=element(monkey);
	monkey=monkey->parentP;
      }
    }
    if(i>=0) {
      int startX = lmP->lmP->indexFct( lmP->cd, "<s>");
      if(startX<0) FATAL("Language model does not contain <s>");
      for(;i>=0;i--) wordA[i]=startX;
    }

    if(searchObjectP->lm.lmP->setStateFct==0 ||
       searchObjectP->lm.lmP->setStateFct( searchObjectP->lm.cd, nodeP ? nodeP->State : NULL)==0) {  /* The LM does not contain a hidden state but the LM is used */
      for (arcP=searchObjectP->nodeOutputArcs(searchObjectP, nodeP); arcP; arcP = searchObjectP->nextOutputArcs(searchObjectP,arcP))
 	states += AStarNodeCreateNPush(searchObjectP, nodeP,  arcP, &warray, score, NULL, 1);
    }
    else {
      LMStateTransitionArray * tA;

      /* Calculate next states from LM */
      warray.itemN--;
      tA = searchObjectP->lm.lmP->getNextStateFct( searchObjectP->lm.cd, &warray, time);   
      warray.itemN++;

      for(i=0;i<tA->itemN;i++) { /* Set state first */
	searchObjectP->lm.lmP->setStateFct( searchObjectP->lm.cd, tA->itemA[i].state);

	if(searchObjectP->lm.lmP->finalStateFct==NULL || searchObjectP->lm.lmP->finalStateFct( searchObjectP->lm.cd)) {
	  for (arcP=searchObjectP->nodeOutputArcs(searchObjectP, nodeP); arcP; arcP = searchObjectP->nextOutputArcs(searchObjectP,arcP))
	    states += AStarNodeCreateNPush(searchObjectP, nodeP,  arcP, &warray, score+tA->itemA[i].penalty, tA->itemA[i].state, 1);
	}
	else {
	  /* Make a transition without changing the path or the search state, only the LM state changes */
	  states += AStarNodeCreateNPush(searchObjectP, nodeP,  NULL, NULL, score+tA->itemA[i].penalty, tA->itemA[i].state, 0);
	}
      }
    }
  }

  /* The world is bad and we kill our parents */

  AStarNodeFree( searchObjectP, nodeP);

  return TCL_OK;
}

AStarSearchStateP AStarArcSearchStateNULL(AStar * searchObjectPtr, AStarArcP arcP) {
  return NULL;
}

/* ------------------------------------------------------------------------
    AStarFindMBest   generates a list of the N best hypothesis stored in 
                       the lattice.

                       zn,pn: new language model weight and penalty
   ------------------------------------------------------------------------ */

int AStarSearch(AStar* searchObjectPtr )
{
  int n =       searchObjectPtr->n;
  int i;

  RBT       boundedQueue;
  RBT       outputQueue;
  RBT_Node* P;

  int maxNodes = n+searchObjectPtr->maxPathN+1;

  searchObjectPtr->hypTriedN=0;
  searchObjectPtr->hypRejectN=0;

  searchObjectPtr->free_list=0;
  searchObjectPtr->mem_list=0;

  searchObjectPtr->boundedQueueP = &boundedQueue;
  searchObjectPtr->outputQueueP  = &outputQueue;

  if(!searchObjectPtr->arcSearchState)
    searchObjectPtr->arcSearchState = AStarArcSearchStateNULL;

  /* Allocate memory for AStarNode's */

  if(!(searchObjectPtr->mem_node_array = calloc(maxNodes,sizeof(AStarNode))))
     FATAL("Cannot alloc memory for node array\n");

  searchObjectPtr->free_node_list = searchObjectPtr->mem_node_array;
  for(i=0;i<maxNodes-1;i++)
    searchObjectPtr->mem_node_array[i].un.nextFree = &(searchObjectPtr->mem_node_array[i+1]);
  searchObjectPtr->mem_node_array[i].un.nextFree = NULL;
  for(i=0;i<maxNodes;i++) {
    searchObjectPtr->mem_node_array[i].searchState = NULL;
    searchObjectPtr->mem_node_array[i].State = NULL;
  }

  /* Initialize the bounded search priority queue & output queue */

  RBT_Init(&boundedQueue,(RBTOrderingFct*)AStarNode_is_less,(RBTDeinitFct*)AStarNodeFree,
	   (ClientData)searchObjectPtr,searchObjectPtr->maxPathN,0,
	   (HashKeyFn2*)AStarNodeHash,
	   (HashCmpFn2*)AStarNodeEqual
	   );
  RBT_Init(&outputQueue,(RBTOrderingFct*)AStarNode_is_less,(RBTDeinitFct*)AStarNodeFree,
	   (ClientData)searchObjectPtr,n,0,
	   (HashKeyFn2*)AStarNodeHash,
	   (HashCmpFn2*)AStarNodeEqual
	   );
  
  /* Main A^* algorithm, works until stack is empty or all n hypos fell out */

#if defined(DEBUG) && DEBUG>2
  for(RBT_InsertNode(&boundedQueue, (ClientData)NULL);
      boundedQueue.nodesN && ( outputQueue.nodesN < n) && (searchObjectPtr->hypTriedN < searchObjectPtr->maxHypTriedN);
      1) {
    AStarPrintStack(searchObjectPtr);
    AStarNodeCreateFollower( searchObjectPtr, (AStarNode*)RBT_PopHighNode(&boundedQueue,1));
  }
#else
  for(RBT_InsertNode(&boundedQueue, (ClientData)NULL); boundedQueue.nodesN && ( outputQueue.nodesN < n);) {
    AStarNode * nodeP = (AStarNode*)RBT_PopHighNode(&boundedQueue,1);
    if(searchObjectPtr->hypTriedN >= searchObjectPtr->maxHypTriedN) { /* Just try to get to a terminal node */
      while(boundedQueue.nodesN>0) {
	RBT_PopHighNode(&boundedQueue,0); searchObjectPtr->hypRejectN++;
      }
    }
    AStarNodeCreateFollower( searchObjectPtr, nodeP);
  }
  INFO("Output %ld bounded %ld\n",outputQueue.nodesN,boundedQueue.nodesN);
  
#endif

  
  searchObjectPtr->hypoReplacedN =
    boundedQueue.nodesReplaced;
  searchObjectPtr->hypRejectN =
    boundedQueue.nodesPruned;
    
  /* Remove pending path structures from A^* "stack" and the "stack" itself */

  if(RBT_isOK(&boundedQueue)!=TCL_OK)
    FATAL("RBT contains internal problems in boundedQueue\n");
  RBT_Deinit(&boundedQueue);

  /* Built up the hypolist in the right order (high probablity of hypo to low) */

  for(P=RBT_FindHigh(&outputQueue);P;P=RBT_FindPrev(&outputQueue,P)) {
    int hypoSize; 
    AStarNode* nodeP = (AStarNode*)(P->Data);
    AStarPath ** hypothesis = AStarBacktrace( searchObjectPtr, nodeP, &hypoSize);
    searchObjectPtr->pathBacktrace( searchObjectPtr, hypoSize, hypothesis, nodeP);
  }	

  if(RBT_isOK(&outputQueue)!=TCL_OK)
    FATAL("RBT contains internal problems outputQueue\n");
  RBT_Deinit(&outputQueue);
       
  /* Remove elements from free_list */
  
  if(searchObjectPtr->mem_list) {
    while(searchObjectPtr->mem_list) {
      int i;
      AStarPathMemory* memP = searchObjectPtr->mem_list;

      if(searchObjectPtr->lm.lmP->stateMemFreeFct) {
	for(i=0;i<memP->size;i++) {
	  AStarPath * pP = memP->paths+i;
	  searchObjectPtr->lm.lmP->stateMemFreeFct( searchObjectPtr->lm.cd, pP->StateMem);
	}
      }

      searchObjectPtr->mem_list = memP->next;
      free(memP->paths);
      free(memP);
    }
  }
  searchObjectPtr->free_list=NULL;

  /* Free AStarNode's */

  for(i=0;i<maxNodes;i++) {
    LMPtr* lmP = &(searchObjectPtr->lm);
    if(lmP->lmP->stateFreeFct)
      lmP->lmP->stateFreeFct(lmP->cd, searchObjectPtr->mem_node_array[i].State);
    if(searchObjectPtr->searchStateFree)
      searchObjectPtr->searchStateFree(searchObjectPtr,searchObjectPtr->mem_node_array[i].searchState);
  }	
  free(searchObjectPtr->mem_node_array); searchObjectPtr->mem_node_array=searchObjectPtr->free_node_list=0;

  return TCL_OK;
}


static AStarPath ** AStarBacktrace2(AStar* searchObjectPtr, AStarPath* path, int * hypoSize,int depth) {
  AStarPath ** ret;
  if(path) {
    depth++;
    ret = AStarBacktrace2(searchObjectPtr, path->parentP, hypoSize, depth);
    ret[(*hypoSize) - depth]=path;
    return ret;
  }
  else {
    *hypoSize = depth;
    return depth ? (AStarPath **)calloc(depth,sizeof(AStarPath*)) : NULL;
  }
}

static AStarPath ** AStarBacktrace(AStar* searchObjectPtr, AStarNode* nodeP, int * hypoSize) {  
  AStarPath ** ret;

  if(nodeP)
    ret = AStarBacktrace2(searchObjectPtr, nodeP->pathP, hypoSize,0);
  else {
    ret = NULL; *hypoSize=0;
  }
  
#if defined(DEBUG) && DEBUG>5

  /* Check if the score calculation is correct */
  IArray warray; float score=0, scorediff;
  int i,j;
  List * nextState;
  FArray * transitionPenalty;
  int time = 0;


  /* Calculate the score */

  iarrayInit(&warray,*hypoSize+1);
  warray.itemA[0] = searchObjectPtr->lm.lmP->indexFct( searchObjectPtr->lm.cd, "<s>");

  if(searchObjectPtr->setStateFct) searchObjectPtr->lm.lmP->setStateFct( searchObjectPtr->lm.cd, NULL);

  for(i=0;i<*hypoSize;i++) {
    AStarNodeP nodeP = ret[i]->AStarElement;
    AStarNodeP parentP = i==0 ? NULL : ret[i-1]->AStarElement;
    AStarArcP arcP;

    warray.itemA[i+1] = searchObjectPtr->nodeElement(searchObjectPtr,nodeP);
    for (arcP=searchObjectPtr->nodeOutputArcs(searchObjectPtr, parentP);
	 arcP && (searchObjectPtr->arcNode(searchObjectPtr, arcP)!=nodeP);
	 arcP = searchObjectPtr->nextOutputArcs(searchObjectPtr,arcP));
    if(!arcP)
      FATAL("Not a valid hypothesis\n");

    if(searchObjectPtr->setStateFct) {
      int warraySize = warray.itemN;
      warray.itemN = i+1;
      searchObjectPtr->lm.lmP->getNextStateFct( searchObjectPtr->lm.cd, &transitionPenalty, &nextState, &warray, time);   
      warray.itemN = warraySize;
      for(j=0;j<nextState->itemN;j++) {
	ClientData stateP = (ClientData)&(nextState->itemA[nextState->itemSizeCP*j]);
	if(searchObjectPtr->lm.lmP->stateEqFct(searchObjectPtr->lm.cd, stateP, ret[i]->State))
	  break;
      }
      if(j>=nextState->itemN) {
	char stateString[100000];
	searchObjectPtr->lm.lmP->statePrintFct( searchObjectPtr->lm.cd, ret[i]->State, stateString);
	INFO("States avaible for current state %s:\n",stateString);
	for(j=0;j<nextState->itemN;j++) {
	  ClientData stateP = (ClientData)&(nextState->itemA[nextState->itemSizeCP*j]);
	  searchObjectPtr->lm.lmP->statePrintFct( searchObjectPtr->lm.cd, stateP, stateString);
	  INFO("States %ld: %s\n",i,stateString);
	}
	FATAL("Next state not correct, element %ld, nextState->itemN %ld\n",i,nextState->itemN);
      }
      score += transitionPenalty->itemA[j];
      searchObjectPtr->lm.lmP->setStateFct( searchObjectPtr->lm.cd, ret[i]->State);
    }
    score += searchObjectPtr->arcTransition(searchObjectPtr, arcP);
    score += searchObjectPtr->lm.lmP->scoreFct(searchObjectPtr->lm.cd , warray.itemA, i+2, i+1);

    time  = searchObjectPtr->nodeTime(searchObjectPtr,nodeP);
  }

  scorediff = score-path->score;
  if(scorediff<0) scorediff = -scorediff;
  if(scorediff/path->score>0.001)
    WARN("score (%g) != path->score (%g)\n",score,path->score);
  iarrayDeinit(&warray);  
#endif
  return ret;
}


