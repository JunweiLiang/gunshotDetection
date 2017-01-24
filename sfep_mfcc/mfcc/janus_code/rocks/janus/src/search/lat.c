/* ========================================================================
    JANUS      Speech- to Speech Translation
               ------------------------------------------------------------
               Object: Tagger for an input lattice with language model
               ------------------------------------------------------------

    Author  :  Klaus Ries
    Module  :  lat.c
    Date    :  $Id: lat.c 3418 2011-03-23 15:07:34Z metze $
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
    Revision 1.5  2003/08/14 11:20:22  fuegen
    Merged changes on branch jtk-01-01-15-fms (jaguar -> ibis-013)

    Revision 1.4.20.1  2001/11/19 14:46:27  metze
    Go away, boring message!

    Revision 1.4  2000/11/14 12:29:34  janus
    Merging branch jtk-00-10-27-jmcd.

    Revision 1.3.30.1  2000/11/06 10:50:32  janus
    Made changes to enable compilation under -Wall.

    Revision 1.3  1998/08/02 11:19:19  kries
    New time handling for lattices, handling representations

 * Revision 1.2  1998/06/29  23:46:43  kries
 * *** empty log message ***
 *
 * Revision 1.1  1998/06/11  14:52:10  kries
 * Initial revision
 *

 
   ======================================================================== */


#include "lat.h"
#include "hmm3gram.h"

static Lat latDefault;

int     LNodeTimeListEqFct( Lat* latP, LNodeTimeList * p1, LNodeTimeList * p2) {
  return (p1->endFrameX == p2->endFrameX)  && (p1->parentP == p2->parentP);
}

long     LNodeTimeListHashFct( Lat* latP, LNodeTimeList * p) {
  return (p->endFrameX>>6) + (long)p->parentP;
}

int     LNodeStateEqFct( Lat* latP, LNodeState * p1, LNodeState * p2) {
  return (p1->stateX == p2->stateX) && (p1->parentP == p2->parentP);
}

long     LNodeStateHashFct( Lat* latP, LNodeState * p) {
  return (p->stateX>>6) + (long)p->parentP;
}

int     LNodeTimeEqFct( Lat* latP, LNodeTime * p1, LNodeTime * p2) {
  return (p1->startFrameX == p2->startFrameX) && (p1->channelX == p2->channelX);
}

long     LNodeTimeHashFct( Lat* latP, LNodeTime * p) {
  return p->startFrameX + (p->channelX>>8);
}


static void data_deinit(ClientData Hash, ClientData data) { free(data); }
static void node_deinit(ClientData Hash, ClientData data) { free(((LNodeTimeList *)data)->rep); free(data); }


int     lNodeCreate( Lat* latP, int channelX, long startFrameX, long endFrameX, int stateX, float score, char *rep) {

  LNodeTimeList * timeLP = (LNodeTimeList*)malloc(sizeof(LNodeTimeList));
  LNodeTime     * timeP  =     (LNodeTime*)malloc(sizeof(LNodeTime));
  LNodeState    * stateP =    (LNodeState*)malloc(sizeof(LNodeState));

  void * t;

  if(timeLP==0 || timeP==0 || stateP==0) {
    free(timeLP); free(timeP); free(stateP);
    SERROR("Cannot allocate memory for lattice node creation\n");
    return TCL_ERROR;
  }

  
  timeP->startFrameX = startFrameX; timeP->channelX = channelX; timeP->fanIn=0; timeP->linkP=NULL;
  timeP = (LNodeTime*)hashEnter(&(latP->startTimes), (ClientData)timeP);

  assert(timeP);
  stateP->parentP = timeP; stateP->stateX = stateX; stateP->linkP=NULL; stateP->nextP=NULL; t=stateP;
  stateP = (LNodeState*)hashEnter(&(latP->startTimesStates),(ClientData)stateP);
  if(stateP==t) { /* needs to be queued since it is a new entry */
    stateP->nextP = timeP->linkP; timeP->linkP  = stateP;
  }

  assert(stateP);
  timeLP->parentP = stateP; timeLP->endFrameX = endFrameX; timeLP->score = score; timeLP->rep = rep ? strdup(rep) : rep; timeLP->timeP=NULL; t=timeLP;
  timeLP = (LNodeTimeList*)hashEnter(&(latP->startTimesStatesEndTimes),(ClientData)timeLP);
  if(t==timeLP) { /* needs to be queued since it is a new entry */
    timeLP->nextP = stateP->linkP; stateP->linkP = timeLP;
  }

  /* No free(.) necessary, the space allocated is memory-wise owned by the respective hash array */

  return TCL_OK;
}

int latAddItf( ClientData cd, int argc, char *argv[])
{
  Lat*  latP   = (Lat*)cd;
  int       ac     =  argc - 1;
  long      from   =  0, to=0;
  int       word = 0, channelX;
  float     score;
  char *    rep = 0;

  if ( itfParseArgv( argv[0], &ac, argv+1, 0,
      "<channelX>",  ARGV_INT,     NULL, &channelX,  NULL,"channel id [0..n-1]",
      "<from>",      ARGV_LONG,    NULL, &from,      NULL,"starting point of word",
      "<to>",        ARGV_LONG,    NULL, &to,        NULL,"ending point of word",
      "<word>",      ARGV_CUSTOM,  LMPtrIndexItf, &word,  &(latP->lm), "word",
      "<score>",     ARGV_FLOAT,   NULL, &score, NULL,"link score",
      "-rep",        ARGV_STRING,  NULL, &rep, NULL,"representation",
       NULL) != TCL_OK) return TCL_ERROR;

  return lNodeCreate( latP, channelX, from, to, word, score, rep);
}


static int LNodeTimeCMP(const void *a, const void *b) 
{
  LNodeTime * aP = *(LNodeTime **)a;
  LNodeTime * bP = *(LNodeTime **)b;

  if(aP->channelX<bP->channelX) return -1;
  if(aP->channelX>bP->channelX) return  1;

  if(aP->startFrameX<bP->startFrameX) return -1;
  if(aP->startFrameX>bP->startFrameX) return  1;

  return 0;
}


int      latSearch( Lat* latP )
{
  int       warray[2];
  int       endStateX;
  
  LNodeTime ** topsort;
  LNodeTimeList ** endpoints;

  /* Link all endTime nodes with successor */
  /* and calculate fanIn counts */

  HashIter iH;  
  int i;

  free(latP->searchArc.state.timePA);
  free(latP->initialState.timePA);
  latP->hypoP = NULL;

  latP->searchArc.state.timePA = (LNodeTime **)calloc(latP->channelN,sizeof(LNodeTime *));
  if(!latP->searchArc.state.timePA) { SERROR("Cannot alloc memory\n"); return TCL_ERROR; };
  latP->initialState.timePA = (LNodeTime **)calloc(latP->channelN,sizeof(LNodeTime *));
  if(!latP->initialState.timePA) { SERROR("Cannot alloc memory\n"); return TCL_ERROR; };

  for(i=0;i<latP->channelN;i++)
    latP->initialState.timePA[i]=NULL;
  latP->initialState.subState =   LatSubState_startTimeStateEndTime;

  endpoints = (LNodeTimeList **)calloc(latP->startTimesStatesEndTimes.itemN,sizeof(LNodeTimeList *));
  if(!endpoints) { SERROR("Cannot alloc memory\n"); return TCL_ERROR; };
  for(i=0;i<latP->startTimesStatesEndTimes.itemN;i++)
    endpoints[i]=0;

  for( hashIterInit(&(latP->startTimesStatesEndTimes),&iH); iH.Current; hashIterNext(&iH)) {
    LNodeTime timeNode;    
    LNodeTimeList * timeLP = *(LNodeTimeList **)(iH.Current);
    LNodeState *    stateP = timeLP->parentP;
    LNodeTime *      timeP = stateP->parentP;
    HashHandle h;

    timeNode.channelX      = timeP->channelX;
    timeNode.startFrameX   = timeLP->endFrameX;

    if( (h=hashItem(&(latP->startTimes), (ClientData)&timeNode, 0)) ) {
      timeLP->timeP = *(LNodeTime **)hashRecall(&(latP->startTimes),h);
      timeLP->timeP->fanIn++;
    } else {
      timeLP->timeP = NULL;
    }

    if((latP->initialState.timePA[timeP->channelX]==NULL) || (latP->initialState.timePA[timeP->channelX]->startFrameX>timeP->startFrameX))
       latP->initialState.timePA[timeP->channelX]=timeP;

    if(!timeLP->timeP) {
      if(endpoints[timeP->channelX]) {
	if(timeLP->endFrameX!=endpoints[timeP->channelX]->endFrameX)
	  SERROR("Both frame %ld and %ld have zero output node in channel %d\n",timeLP->endFrameX,endpoints[timeP->channelX]->endFrameX,timeP->channelX);
      }
      else	
	endpoints[timeP->channelX]=timeLP;
    }
  }

  for(i=0;i<latP->channelN;i++)
    if(!latP->initialState.timePA[i] ||
       latP->initialState.timePA[i]->startFrameX<0) {
      FATAL("Channel %ld does not contain a start node\n",i);
    }

  /* Calculate lookaheads ("full bigram search") */

  topsort = (LNodeTime **)calloc(latP->startTimes.itemN,sizeof(LNodeTime *));
  if(!topsort) { SERROR("Cannot alloc memory for sorting\n"); return TCL_ERROR; };
  
  for( hashIterInit(&(latP->startTimes),&iH), i=0;
       iH.Current;
       hashIterNext(&iH),i++) {

    LNodeTime * timeP =*(LNodeTime **)(iH.Current);
    topsort[i] = timeP;

    if(latP->initialState.timePA[timeP->channelX]->startFrameX==timeP->startFrameX) {
      if(timeP->fanIn>0)
	FATAL("Earliest node (%ld) in channel %ld does have fanIn==%ld\n",
	      timeP->startFrameX,timeP->channelX,timeP->fanIn);
    }
    else {
      if(timeP->fanIn==0)
	FATAL("Node (%ld) in channel %ld has fanIn==0, but is not earliest node, which is %ld\n",
	      timeP->startFrameX,timeP->channelX,latP->initialState.timePA[timeP->channelX]->startFrameX);
    }
  }
  qsort(topsort, latP->startTimes.itemN, sizeof(LNodeTime *), LNodeTimeCMP);

  endStateX = latP->lmLookahead.lmP->indexFct(latP->lmLookahead.cd,"</s>");

  if(latP->bigramLookahead) {
    int warrayBreak[2];
    warrayBreak[0] = latP->lmLookahead.lmP->indexFct(latP->lmLookahead.cd,"<s>");

    for(i=latP->startTimes.itemN-1; i>=0; i--) {
      LNodeState * stateP;
      topsort[i]->lBscore = topsort[i]->lscore = 1e20;
      for(stateP=topsort[i]->linkP; stateP; stateP=stateP->nextP) {
	LNodeTimeList * timeLP;
	float lscore, lscoreTemp, lbreak, lBscore;
	stateP->lBscore = stateP->lscore = 1e20;
	warrayBreak[1] = warray[0] = stateP->stateX;
	lbreak = latP->lmLookahead.lmP->scoreFct(latP->lmLookahead.cd,warrayBreak,2,1);	
	
	for(timeLP = stateP->linkP; timeLP; timeLP=timeLP->nextP) {
	  if(timeLP->timeP) { /* this is not a final state */
	    LNodeState * stateP2;
	    lscore = 1e20;
	    for( stateP2=timeLP->timeP->linkP; stateP2; stateP2=stateP2->nextP) {
	      warray[1] = stateP2->stateX;
	      lscoreTemp = stateP2->lscore+latP->lmLookahead.lmP->scoreFct(latP->lmLookahead.cd,warray,2,1);
	      if(lscoreTemp<lscore) lscore=lscoreTemp;
	    }	
	    lBscore = timeLP->timeP->lscore;
	  }
	  else { /* this is a final state */
	    warray[1] =  endStateX;
	    lscore  = latP->lmLookahead.lmP->scoreFct(latP->lmLookahead.cd,warray,2,1);
	    lBscore = 0;
	  }
	  if(lscore + timeLP->score<stateP->lscore ) stateP->lscore = lscore + timeLP->score;
	  if(lBscore+ timeLP->score<stateP->lBscore) stateP->lBscore= lBscore+ timeLP->score;
	}
	if(stateP->lscore        <topsort[i]->lscore)  topsort[i]->lscore  = stateP->lscore;	
	if(stateP->lBscore+lbreak<topsort[i]->lBscore) topsort[i]->lBscore = stateP->lBscore+lbreak;
      }
    }
  }
  else { /* unigram-lookahead only */
    float endLookahead = 0;
    warray[0] = endStateX;
    endLookahead = latP->lmLookahead.lmP->scoreFct(latP->lmLookahead.cd,warray,1,0);

    for(i=latP->startTimes.itemN-1;i>=0;i--) {
      LNodeState * stateP;
      topsort[i]->lscore = 1e20;
      for(stateP=topsort[i]->linkP;stateP;stateP=stateP->nextP) {
	LNodeTimeList * timeLP;
	float nodeScore;

	stateP->lscore = 1e20;
	warray[0] = stateP->stateX;
	nodeScore = latP->lmLookahead.lmP->scoreFct(latP->lmLookahead.cd,warray,1,0);

	for(timeLP = stateP->linkP; timeLP; timeLP = timeLP->nextP) {
	  float lscore = timeLP->timeP ? timeLP->timeP->lscore : endLookahead;
	  if(lscore+timeLP->score<stateP->lscore) stateP->lscore=lscore+timeLP->score;
	}

	if(stateP->lscore+nodeScore<topsort[i]->lscore) topsort[i]->lscore=stateP->lscore+nodeScore;
	stateP->lBscore = stateP->lscore;
      }
      topsort[i]->lBscore = topsort[i]->lscore;
    }
  }

  latP->initialState.lookaheadBreakPA = latP->initialState.lookaheadPA = 0;
  for(i=0;i<latP->channelN;i++) {
    latP->initialState.lookaheadPA      += latP->initialState.timePA[i]->lscore;
    latP->initialState.lookaheadBreakPA += latP->initialState.timePA[i]->lBscore;
  }

  AStarSearch(&(latP->searchObject));

  free(latP->initialState.timePA); latP->initialState.timePA=NULL;
  free(latP->searchArc.state.timePA); latP->searchArc.state.timePA=NULL;

  return TCL_OK;
}

static int latBuildHypo(Lat * latP, LatHypo * hypoP) {
  int i;
  char stateString[1000000];

  /* Built the hypothesis recursively */

  if(hypoP==NULL) return TCL_OK;
  if(latBuildHypo(latP,hypoP->nextP)!=TCL_OK) return TCL_ERROR;
  if(hypoP->nextP) {
    free(hypoP->nextP->itemA);
    free(hypoP->nextP);
    hypoP->nextP=NULL;
  }

  stateString[0]='\000';

  itfAppendResult("{ %f ",hypoP->score);
  for(i=0;i<hypoP->itemN;i++) {    
    LatHypoElement * eP = &(hypoP->itemA[i]);    
    if(eP!=(LatHypoElement *)&(latP->searchObject)) {
      itfAppendResult(" {%d %ld ",
		      eP->channelX,eP->startFrameX);
      itfAppendElement(latP->lm.lmP->nameFct(latP->lm.cd,eP->stateX));
      if(latP->lm.lmP->stateMemPrintFct) { /* Copy object state */
	latP->lm.lmP->stateMemPrintFct( latP->lm.cd, hypoP->itemA[i].StateMem,stateString);
	itfAppendElement(stateString);
      }
      itfAppendResult(" {%s}} ",eP->rep ? eP->rep:"");
    }
  }
  itfAppendResult("}\n");

  return TCL_OK;
}
  
static int latSearchItf( ClientData cd, int argc, char *argv[])
{
  Lat    * latP  = (Lat *)cd;  
  int       ac     =  argc - 1;
  int       ret=TCL_OK;
  Timer     stopit;
  int       info = 1;
  LatHypo * hypoP;

  if ( itfParseArgv( argv[0], &ac, argv+1, 0,
		     "-info" ,   ARGV_INT   , NULL, &info, NULL, "print info (default 1)",
       NULL) != TCL_OK) return TCL_ERROR;

  timerReset(&stopit); timerStart(&stopit);

  ret = latSearch(latP);
  if(ret!=TCL_OK) return ret;


  if(info)
    INFO("hypTriedN %d hypRejectN %d hypReplacedN %d secs %f\n",
	 latP->searchObject.hypTriedN, latP->searchObject.hypRejectN,latP->searchObject.hypoReplacedN,
	 timerStop(&stopit));

  hypoP = latP->hypoP;
  latP->hypoP = NULL;

  return latBuildHypo(latP,hypoP);
}


/* ------------------------------------------------------------------------
    latPathBacktrace
   ------------------------------------------------------------------------ */

void latPathBacktrace( AStar * searchObjectP, int hypoN, AStarPath** pathP, AStarNode * nodeP)
{
  Lat * latP= (Lat*)searchObjectP->searchStructureP;
  int wordX, i;
  

  LatHypo * hypoP = malloc(sizeof(LatHypo));
  LatHypoElement * itemA = calloc(hypoN,sizeof(LatHypoElement));

  if(hypoP==NULL || itemA==NULL) FATAL("Cannot alloc memory for latPathBacktrace\n");

  for(wordX=0;wordX<hypoN;wordX++) {    
    LNodeState * stateP   = (LNodeState *)pathP[wordX]->AStarElement;
    LNodeTimeList * timeLP;
    LNodeState * nextStateP;

    if(stateP==(LNodeState *)searchObjectP) break;

    itemA[wordX].startFrameX  = stateP->parentP->startFrameX;
    itemA[wordX].channelX     = stateP->parentP->channelX;
    itemA[wordX].stateX       = stateP->stateX;
    itemA[wordX].StateMem     = pathP[wordX]->StateMem;


    nextStateP = (LNodeState *)searchObjectP;
    for(i=wordX+1;i<hypoN && ((LNodeState *)pathP[i]->AStarElement)!=((LNodeState *)searchObjectP);i++) {
      /* this is not quadratic in runtime even if it looks like that ;-) */
      LNodeState * nP = (LNodeState *)pathP[i]->AStarElement;
      if(nP==((LNodeState *)searchObjectP)) break;
      if(nP->parentP->channelX==itemA[wordX].channelX) { nextStateP = nP; break; }
    }

    /* Search for the succesor LatTimeList */

    if(nextStateP!=(LNodeState *)searchObjectP) { /* within sentence, hashing */
      LNodeTimeList searchMe;
      HashHandle    h;

      searchMe.parentP = stateP; searchMe.endFrameX = nextStateP->parentP->startFrameX;

      if(!(h = hashItem(&(latP->startTimesStatesEndTimes), (ClientData)&searchMe, 0)))
	FATAL("Follower timeList not found start=%ld end=%ld channelX=%ld\n",itemA[wordX].startFrameX,searchMe.endFrameX,itemA[wordX].channelX);
      timeLP = *(LNodeTimeList**)hashRecall(&(latP->startTimesStatesEndTimes),h);
      /*
      if(h = hashItem(&(latP->startTimesStatesEndTimes), (ClientData)&searchMe, 0))
	timeLP = *(LNodeTimeList**)hashRecall(&(latP->startTimesStatesEndTimes),h);
      else {
	for(timeLP = stateP->linkP;timeLP && timeLP->timeP;timeLP=timeLP->nextP);
	if(!timeLP)
	  FATAL("Follower timeList not found start=%ld end=%ld channelX=%ld and not a last node\n",itemA[wordX].startFrameX,searchMe.endFrameX,itemA[wordX].channelX);
      }
      */
    }
    else { /* Do a linear search */
      for(timeLP = stateP->linkP;timeLP && timeLP->timeP;timeLP=timeLP->nextP);
      if(!timeLP) FATAL("No last node !\n");
    }
    itemA[wordX].rep          = timeLP->rep;
  }

  hypoP->itemA = itemA;
  hypoP->itemN = wordX;
  hypoP->nextP = latP->hypoP;
  hypoP->score = nodeP->score;

  latP->hypoP  = hypoP;
}

int lNodeElement(AStar * searchObjectPtr, AStarNodeP nodeP) {
  Lat * latP = (Lat *)(searchObjectPtr->searchStructureP);
  LNodeState *stateP = (LNodeState *)nodeP;
  assert(stateP);

  return ((AStarNodeP)searchObjectPtr==nodeP) ?
              latP->lm.lmP->indexFct(latP->lm.cd,"</s>") :
              stateP->stateX;
}

int latPathEnd(AStar * searchObjectPtr, AStarNode * anodeP)
{
  if(anodeP) {
    LatSearchState * stateP = (LatSearchState *) anodeP->searchState;
    return stateP->subState==LatSubState_FINAL;
  }
  return 0;
}

float latLookahead(AStar * searchObjectPtr, AStarNode * anodeP)
{
  LatSearchNode  *  nodeP = (LatSearchNode *)   anodeP->AStarElement;
  LatSearchState * stateP = (LatSearchState *) anodeP->searchState;  

  int breakscore = 0;

  if(!nodeP) return 0;

  if(anodeP && nodeP) {
    if(searchObjectPtr->lm.lmP == &HMM3gramLM) {
      HMM3gramState* hmmStateP = (HMM3gramState*)anodeP->State;
      if(hmmStateP->subState==HMM3gramWord || hmmStateP->subState==HMM3gramTerminatedSpeechAct)
	breakscore++;
    }
  }  

  switch(stateP->subState) { /* transition function */

  case LatSubState_startTime:	
    if(breakscore)
      return stateP->lookaheadBreakPA + (nodeP ? nodeP->time.lBscore : 0); 
    else
      return stateP->lookaheadPA      + (nodeP ? nodeP->time.lscore : 0); 

  case LatSubState_startTimeState:
    if(breakscore) {
      /* printf("\nLooakead score %f\n",stateP->lookaheadBreakPA + (nodeP ? nodeP->state.lBscore : 0)); */
      return stateP->lookaheadBreakPA + (nodeP ? nodeP->state.lBscore : 0); 
    }
    else
      return stateP->lookaheadPA      + (nodeP ? nodeP->state.lscore : 0); 
    
  case LatSubState_startTimeStateEndTime:      
    if(breakscore)
      return stateP->lookaheadBreakPA + (nodeP->timeL.timeP  ? nodeP->timeL.timeP->lBscore : 0); 
    else
      return stateP->lookaheadPA      + (nodeP->timeL.timeP  ? nodeP->timeL.timeP->lscore : 0); 

  case LatSubState_EOS:      
  case LatSubState_FINAL:
    break;
  default:
    FATAL("Illegal subState %d\n",stateP->subState);
  }
      
  return 0.0;
}

AStarArcP lNodeOutputArcs(AStar * searchObjectPtr, AStarNode * anodeP) {
  Lat * latP = (Lat*)(searchObjectPtr->searchStructureP);

  int i;
  long minCount=0;
  LatSubState subState = LatSubState_startTimeStateEndTime; 

  latP->searchArc.penalty = 0;

  if(anodeP && anodeP->searchState) {
    LatSearchState * searchStateP = (LatSearchState *)(anodeP->searchState);

    for(i=0;i<latP->channelN;i++)
      latP->searchArc.state.timePA[i] = searchStateP->timePA[i];
    subState = searchStateP->subState;
    latP->searchArc.state.lookaheadPA = searchStateP->lookaheadPA;
    latP->searchArc.state.lookaheadBreakPA = searchStateP->lookaheadBreakPA;
  }
  else {
    /* Initial node */
    latP->searchArc.state.subState =   LatSubState_startTimeStateEndTime;
    latP->searchArc.state.lookaheadPA = latP->initialState.lookaheadPA;
    latP->searchArc.state.lookaheadBreakPA = latP->initialState.lookaheadBreakPA;

    for(i=0;i<latP->channelN;i++)
      latP->searchArc.state.timePA[i] = latP->initialState.timePA[i];
  }



  switch(subState) { /* transition function */

  case LatSubState_startTime:	
    assert(anodeP);
    latP->searchArc.state.subState = LatSubState_startTimeState;
    latP->searchArc.nextP.stateP   = ((LatSearchNode *)anodeP->AStarElement)->time.linkP;
    break;
    
  case LatSubState_startTimeState:
    assert(anodeP);
    latP->searchArc.state.subState = LatSubState_startTimeStateEndTime;
    latP->searchArc.nextP.timeLP   = ((LatSearchNode *)anodeP->AStarElement)->state.linkP;
    break;

  case LatSubState_startTimeStateEndTime:
    if(anodeP && anodeP->AStarElement) {
      LatSearchNode  * nodeP = ((LatSearchNode *)anodeP->AStarElement);
      latP->searchArc.state.timePA[nodeP->timeL.parentP->parentP->channelX] = nodeP->timeL.timeP;
      if(latP->searchArc.state.timePA[nodeP->timeL.parentP->parentP->channelX]) {
	latP->searchArc.state.lookaheadPA += latP->searchArc.state.timePA[nodeP->timeL.parentP->parentP->channelX]->lscore;
	latP->searchArc.state.lookaheadBreakPA += latP->searchArc.state.timePA[nodeP->timeL.parentP->parentP->channelX]->lBscore;
      }
    }

    for(i=0;i<latP->channelN && !latP->searchArc.state.timePA[i];i++);
    if(i<latP->channelN) {
      latP->searchArc.state.subState = LatSubState_startTime;

      minCount=1;
      latP->searchArc.channel=i;
      latP->searchArc.minTime=latP->searchArc.state.timePA[latP->searchArc.channel]->startFrameX;
    
      for(i++;i<latP->channelN;i++) {
	if(!latP->searchArc.state.timePA[i]) continue;
	if(latP->searchArc.state.timePA[i]->startFrameX==latP->searchArc.minTime)
	  minCount++;
	else if(latP->searchArc.state.timePA[i]->startFrameX<latP->searchArc.minTime) {
	  minCount=1;
	  latP->searchArc.channel=i;
	  latP->searchArc.minTime=latP->searchArc.state.timePA[latP->searchArc.channel]->startFrameX;
	}
      }
      latP->searchArc.state.lookaheadPA -= latP->searchArc.state.timePA[latP->searchArc.channel]->lscore;
      latP->searchArc.state.lookaheadBreakPA -= latP->searchArc.state.timePA[latP->searchArc.channel]->lBscore;
    }
    else
      latP->searchArc.state.subState = LatSubState_EOS;
    break;
  case LatSubState_EOS:
    latP->searchArc.state.subState = LatSubState_FINAL;    
    break;
  default:
    FATAL("Illegal subState %d\n",subState);
  }


  /* Try to move directly from state to state if there is just one output arc */

  if(latP->searchArc.state.subState == LatSubState_startTimeStateEndTime) {

    if(latP->searchArc.nextP.timeLP->nextP)
      return (AStarArcP)&(latP->searchArc);

    latP->searchArc.state.timePA[latP->searchArc.nextP.timeLP->parentP->parentP->channelX] = latP->searchArc.nextP.timeLP->timeP;
    latP->searchArc.penalty += latP->searchArc.nextP.timeLP->score;
    
    if(latP->searchArc.nextP.timeLP->timeP) {
      latP->searchArc.state.lookaheadPA += latP->searchArc.nextP.timeLP->timeP->lscore;
      latP->searchArc.state.lookaheadBreakPA += latP->searchArc.nextP.timeLP->timeP->lBscore;
    }

    for(i=0;i<latP->channelN && latP->searchArc.state.timePA[i]==NULL;i++);
    if(i<latP->channelN) {
      latP->searchArc.state.subState = LatSubState_startTime;

      minCount=1;
      latP->searchArc.channel=i;
      latP->searchArc.minTime=latP->searchArc.state.timePA[latP->searchArc.channel]->startFrameX;
    
      for(i++;i<latP->channelN;i++) {
	if(!latP->searchArc.state.timePA[i]) continue;
	if(latP->searchArc.state.timePA[i]->startFrameX==latP->searchArc.minTime)
	  minCount++;
	else if(latP->searchArc.state.timePA[i]->startFrameX<latP->searchArc.minTime) {
	  minCount=1;
	  latP->searchArc.channel=i;
	  latP->searchArc.minTime=latP->searchArc.state.timePA[latP->searchArc.channel]->startFrameX;
	}
      }
      latP->searchArc.state.lookaheadPA -= latP->searchArc.state.timePA[latP->searchArc.channel]->lscore;
      latP->searchArc.state.lookaheadBreakPA -= latP->searchArc.state.timePA[latP->searchArc.channel]->lBscore;
    }
    else {
      latP->searchArc.state.subState = LatSubState_EOS;
    }

  }
  if(latP->searchArc.state.subState == LatSubState_startTime) {
    if(minCount==1) {
      latP->searchArc.state.subState = LatSubState_startTimeState;
      latP->searchArc.nextP.stateP = latP->searchArc.state.timePA[latP->searchArc.channel]->linkP;
    }
  }
  /* Would eliminate our ability to add the link to the history 
     and to ask the LM for its score

  if(latP->searchArc.state.subState == LatSubState_startTimeState) {

    if(latP->searchArc.nextP.stateP->nextP)
      return (AStarArcP)&(latP->searchArc);

    latP->searchArc.state.subState = LatSubState_startTimeStateEndTime;
    latP->searchArc.nextP.timeLP = latP->searchArc.nextP.stateP->tlinkP;
  }

  */
  

  return (AStarArcP)&(latP->searchArc);

}

AStarArcP latNextOutputArcs(AStar * searchObjectPtr, AStarArcP aP) {
  Lat * latP = (Lat*)(searchObjectPtr->searchStructureP);
  LatSearchArc* arcP = (LatSearchArc*) aP;

  assert((&latP->searchArc==arcP) || arcP==NULL);

  if(arcP==NULL) return NULL;

  switch(arcP->state.subState) { /* transition function */

  case LatSubState_startTime:	
    assert(latP->searchArc.channel<latP->channelN);

    latP->searchArc.state.lookaheadPA += latP->searchArc.state.timePA[latP->searchArc.channel]->lscore;
    latP->searchArc.state.lookaheadBreakPA += latP->searchArc.state.timePA[latP->searchArc.channel]->lBscore;
    for(latP->searchArc.channel++;latP->searchArc.channel<latP->channelN; latP->searchArc.channel++)
      if(latP->searchArc.state.timePA[latP->searchArc.channel] &&
	 latP->searchArc.state.timePA[latP->searchArc.channel]->startFrameX==latP->searchArc.minTime)
	break;
    
    if(latP->searchArc.channel>=latP->channelN) return NULL;
    latP->searchArc.state.lookaheadPA -= latP->searchArc.state.timePA[latP->searchArc.channel]->lscore;
    latP->searchArc.state.lookaheadBreakPA -= latP->searchArc.state.timePA[latP->searchArc.channel]->lBscore;
    break;
	
  case LatSubState_startTimeState:	
    assert(latP->searchArc.nextP.stateP);
    latP->searchArc.nextP.stateP = latP->searchArc.nextP.stateP->nextP;
    if(latP->searchArc.nextP.stateP==NULL) return NULL;
    break;
    
  case LatSubState_startTimeStateEndTime:
    assert(latP->searchArc.nextP.timeLP);
    latP->searchArc.nextP.timeLP = latP->searchArc.nextP.timeLP->nextP;
    if(latP->searchArc.nextP.timeLP==NULL) return NULL;
    break;
  case LatSubState_EOS:
  case LatSubState_FINAL:
    return NULL;
    
  default:
    FATAL("Illegal subState\n");
  }      

  return (AStarArcP)&(latP->searchArc);
}


int latSkipOutputArcs(AStar * searchObjectPtr, AStarNode * anodeP) {

  LatSearchArc * arcP = (LatSearchArc *)searchObjectPtr->nodeOutputArcs(searchObjectPtr, anodeP);  /* Use the same skip-state heuristic ! */
  assert(arcP);

  switch(arcP->state.subState) { /* transition function */      
  case LatSubState_startTimeState:
  case LatSubState_EOS:
    return 0;
  default:
    return 1;
  }
}

AStarNodeP latArcNode(AStar * searchObjectPtr, AStarArcP arcP)
{
  LatSearchArc* LatArcP = (LatSearchArc*)arcP;

  if(arcP) {
    switch(LatArcP->state.subState) { /* transition function */      
    case LatSubState_startTime:	
     return (AStarNodeP)  LatArcP->state.timePA[LatArcP->channel];
    case LatSubState_startTimeState:	
      return (AStarNodeP)LatArcP->nextP.stateP;
    case LatSubState_startTimeStateEndTime:	
      return (AStarNodeP)LatArcP->nextP.timeLP;
    case LatSubState_EOS:
    case LatSubState_FINAL:
      return (AStarNodeP)searchObjectPtr;
    default:
      FATAL("Illegal subState %d\n",LatArcP->state.subState);
    }
  }
  return NULL;
}

float latArcTransition(AStar * searchObjectPtr, AStarArcP aP)
{
  LatSearchArc* arcP = (LatSearchArc*) aP;
  float ret=0;
  assert(arcP);

  if(arcP) {
    ret = arcP->penalty;
    if(arcP->state.subState==LatSubState_startTimeStateEndTime)
      ret += arcP->nextP.timeLP->score;
  }
  return ret;
}


int lNodeTime(AStar * searchObjectPtr, AStarNode * nodeP) {
  Lat * latP = (Lat *)(searchObjectPtr->searchStructureP);


  if(nodeP) {
    LatSearchState *   stateP = (LatSearchState *)nodeP->searchState;
    LatSearchNode  * latNodeP = (LatSearchNode  *)nodeP->AStarElement;

    /* Only in cases where a startTimeState will be the followerstate it is important to
       return a sensible value.
       If the current subState can reach a startTimeState only indirectly via
       a determinstic chaining (it is not startTime) the value needs to be correct.
       Otherwise it should be the LAST timeFrame, for efficiency reasons
       */     

    switch(stateP->subState) {           
    case LatSubState_startTime:
      return latNodeP->time.startFrameX;
    case LatSubState_startTimeState:
      assert(latNodeP); assert(latNodeP->state.linkP);
      if(latNodeP->state.linkP->nextP!=NULL) return latNodeP->state.parentP->startFrameX; /* not deterministic */
      latNodeP=(LatSearchNode *)latNodeP->state.linkP;    /* still deterministic - go to startTimeStateEndTime */
    case LatSubState_startTimeStateEndTime:
      {
	int  i;
	long minTime=latNodeP->timeL.endFrameX;
	int  minCount=1;
	int  channelX = latNodeP->timeL.parentP->parentP->channelX;
	LNodeTime * timeP = NULL;

	for(i=0;i<latP->channelN;i++) {	
	  timeP = i==channelX ? latNodeP->timeL.timeP : stateP->timePA[i];
	  if(timeP) break;
	}
	if(i>=latP->channelN) return 0; /* EOS */

	minTime = timeP->startFrameX; minCount=1; i++;

	for(;i<latP->channelN;i++) {
	  timeP = i==channelX ? latNodeP->timeL.timeP : stateP->timePA[i];
	  if(!timeP) continue;
	  if(timeP->startFrameX == minTime)
	    minCount++;
	  else if(timeP->startFrameX < minTime) {
	    minTime = timeP->startFrameX;
	    minCount=1;
	  }
	}
	return minCount==1 ? minTime : latNodeP->timeL.parentP->parentP->startFrameX;
      }
    case LatSubState_EOS:
    case LatSubState_FINAL: 
    default:
      return 0;
    }
  }
  else {
    int i;
    long minTime;

    for(i=0;i<latP->channelN;i++)
      if(latP->initialState.timePA[i])
	break;
    if(i>=latP->channelN) return 0;
    minTime = latP->initialState.timePA[i]->startFrameX; i++;
    for(;i<latP->channelN;i++)
      if(latP->initialState.timePA[i]->startFrameX<minTime) minTime = latP->initialState.timePA[i]->startFrameX;
    return minTime;
  }   
}

AStarSearchStateP  latSearchStateCreate(AStar * searchObjectPtr) {
  Lat * latP = (Lat*)(searchObjectPtr->searchStructureP);
  LatSearchState * searchStateP = malloc(sizeof(LatSearchState));
  if(!searchStateP) FATAL("Cannot allocate memory for search state\n");
  searchStateP->timePA = calloc(latP->channelN,sizeof(LNodeTime*));
  if(!searchStateP->timePA) FATAL("Cannot allocate memory for search state\n");

  return (AStarSearchStateP)searchStateP;
}

void latSearchStateCopy(AStar * searchObjectPtr, AStarSearchStateP searchStateP1, AStarSearchStateP searchStateP2)  {

  Lat * latP = (Lat*)(searchObjectPtr->searchStructureP);
  LatSearchState * sP1 = (LatSearchState *)searchStateP1;
  LatSearchState * sP2 = searchStateP2 ? (LatSearchState *)searchStateP2 : &(latP->initialState);
  int i;

  sP1->subState = sP2->subState;
  sP1->lookaheadPA = sP2->lookaheadPA;
  sP1->lookaheadBreakPA = sP2->lookaheadBreakPA;
  for( i=0; i<latP->channelN; i++) 
    sP1->timePA[i] = sP2->timePA[i];
}

void               latSearchStateFree(AStar * searchObjectPtr, AStarSearchStateP searchStateP) {
  LatSearchState * sP = (LatSearchState *)searchStateP;
  if(searchStateP) free(sP->timePA);
  free(searchStateP);
}

long               latSearchStateHash(AStar * searchObjectPtr, AStarSearchStateP searchStateP) {

  long ret=0;
  Lat * latP = (Lat*)(searchObjectPtr->searchStructureP);
  LatSearchState * sP  = (LatSearchState *)searchStateP;
  int i;

  /* Assume lookaheadPA is a function of timePA */

  for( i=0; i<latP->channelN; i++) 
    ret = 2*ret + (long)sP->timePA[i];
  ret = 4*ret + sP->subState;
  return ret;
}

int                latSearchStateEq(  AStar * searchObjectPtr, AStarSearchStateP searchStateP1,AStarSearchStateP searchStateP2) {

  Lat * latP = (Lat*)(searchObjectPtr->searchStructureP);
  LatSearchState * sP1 = (LatSearchState *)searchStateP1;
  LatSearchState * sP2 = (LatSearchState *)searchStateP2;
  int i;

  /* Assume lookaheadPA is a function of timePA */

  if(sP1->subState != sP2->subState) return 0;
  for( i=0; i<latP->channelN; i++) 
    if(sP1->timePA[i] != sP2->timePA[i])
      return 0;
  return 1;
}

AStarSearchStateP  latArcSearchState( AStar * searchObjectPtr, AStarArcP arcP) {
  LatSearchArc * latArcP = (LatSearchArc *)arcP;
  return (AStarSearchStateP) (latArcP ? &(latArcP->state) : NULL);
}

int      latInit(   Lat* latP, char* name, LMPtr * lmP, LMPtr * lmLookaheadP, int channels) {

  latP->name = strdup(name);
  latP->useN = 0;
  
  latP->lm.cd = lmP->cd;
  latP->lm.lmP = lmP->lmP;

  latP->lmLookahead.cd = lmLookaheadP->cd;
  latP->lmLookahead.lmP = lmLookaheadP->lmP;

  latP->channelN = channels;
  latP->bigramLookahead = 0;
  latP->searchArc.state.timePA = NULL;
  latP->initialState.timePA = NULL;

  hashInit(&(latP->startTimes));
  hashInit(&(latP->startTimesStates));
  hashInit(&(latP->startTimesStatesEndTimes));  

  latP->startTimes.data_deinit = data_deinit;
  latP->startTimesStates.data_deinit =  data_deinit;
  latP->startTimesStatesEndTimes.data_deinit = node_deinit;

  latP->startTimes.hashKey = (HashKeyFn2*)LNodeTimeHashFct; latP->startTimes.hashCmp = (HashCmpFn2*)LNodeTimeEqFct;
  latP->startTimesStates.hashKey = (HashKeyFn2*)LNodeStateHashFct; latP->startTimesStates.hashCmp = (HashCmpFn2*)LNodeStateEqFct;
  latP->startTimesStatesEndTimes.hashKey = (HashKeyFn2*)LNodeStateHashFct; latP->startTimesStatesEndTimes.hashCmp = (HashCmpFn2*)LNodeStateEqFct;  

  /* Preconfigure the search */

  latP->searchObject.n            =  1;
  latP->searchObject.context      =  0;
  latP->searchObject.maxPathN     =  1000;
  latP->searchObject.maxHypTriedN =  1000000;
  latP->searchObject.searchStructureP   = (ClientData)latP;
  latP->searchObject.lm.lmP        = lmP->lmP;
  latP->searchObject.lm.cd        = lmP->cd;


  latP->searchObject.pathBacktrace = latPathBacktrace;
  latP->searchObject.nodeElement = lNodeElement;
  latP->searchObject.pathEnd     = latPathEnd; 
  latP->searchObject.nodeLookahead = latLookahead;
  latP->searchObject.nodeOutputArcs = lNodeOutputArcs;
  latP->searchObject.nextOutputArcs = latNextOutputArcs;
  latP->searchObject.arcNode = latArcNode;
  latP->searchObject.arcTransition = latArcTransition;
  latP->searchObject.nodeTime = lNodeTime;
  latP->searchObject.skipOutputArcs = latSkipOutputArcs;

  latP->searchObject.searchStateCreate = latSearchStateCreate;
  latP->searchObject.searchStateFree   = latSearchStateFree;
  latP->searchObject.searchStateHash   = latSearchStateHash;
  latP->searchObject.searchStateEq     = latSearchStateEq;
  latP->searchObject.searchStateCopy   = latSearchStateCopy;
  latP->searchObject.arcSearchState    = latArcSearchState;  


  return TCL_OK;
}

Lat *latCreate( char* name, LMPtr * lmP, LMPtr * lmLookaheadP, int channels) {
  Lat *latP = (Lat*)malloc(sizeof(Lat));

  if (! latP || latInit( latP, name, lmP, lmLookaheadP, channels) != TCL_OK) {

     if ( latP) free( latP);
     ERROR("Failed to allocate Lattice object '%s'.\n", name);
     return NULL; 
  }
  return latP;
}  

/* ------------------------------------------------------------------------
    Free/Deinit Lattice Object
   ------------------------------------------------------------------------ */

int latLinkN( Lat* latP)
{
  return latP->useN;
}

void latDealloc( Lat* latP)
{
  if ( latP) {
    /* the hash table deinit also deletes the related items referred to by the hash table */
    hashClear(&(latP->startTimes));                
    hashClear(&(latP->startTimesStates));
    hashClear(&(latP->startTimesStatesEndTimes));
    free(latP->searchArc.state.timePA);
    free(latP->initialState.timePA);
  }
}

int latDeinit( Lat* latP)
{
  if ( latLinkN( latP)) {
    SERROR("Lattice '%s' still in use by other objects.\n", latP->name);
    return TCL_ERROR;
  }
  if ( latP->name) { free( latP->name); latP->name = NULL; }
  latDealloc( latP);
  lmPtrDeinit( &latP->lm);
  return TCL_OK;
}

int latFree( Lat* latP)
{
  if ( latDeinit( latP) != TCL_OK) return TCL_ERROR;
  free(latP);
  return TCL_OK;
}

static int latFreeItf (ClientData clientData) 
{ return latFree( (Lat*)clientData); }


static ClientData latCreateItf(ClientData clientData, 
                                   int argc, char *argv[])
{
  int       ac     =  argc - 1;
  int channelN;
  LMPtr     lm, lmLooakehead;

  if ( itfParseArgv( argv[0], &ac, argv+1, 0,
      "<lm>", ARGV_CUSTOM, getLMPtr, &lm, NULL,"language model (vocabulary)",
      "<lmLookahead>", ARGV_CUSTOM, getLMPtr, &lmLooakehead, NULL,"lookahead model",
      "<channelN>",  ARGV_INT,    NULL, &channelN, NULL,"number of alternative hypos",
       NULL) != TCL_OK) return NULL;

  return (ClientData)latCreate(argv[0], &lm,&lmLooakehead,channelN);
}


static int LNodeTimeListCMP(const void *a, const void *b) 
{
  LNodeTimeList * aP = *(LNodeTimeList **)a;
  LNodeTimeList * bP = *(LNodeTimeList **)b;

  if(aP->parentP->parentP->startFrameX<bP->parentP->parentP->startFrameX) return -1;
  if(aP->parentP->parentP->startFrameX>bP->parentP->parentP->startFrameX) return  1;

  if(aP->endFrameX<bP->endFrameX) return -1;
  if(aP->endFrameX>bP->endFrameX) return  1;

  if(aP->parentP->parentP->channelX<bP->parentP->parentP->channelX) return -1;
  if(aP->parentP->parentP->channelX>bP->parentP->parentP->channelX) return  1;

  if(aP->parentP->stateX<bP->parentP->stateX) return -1;
  if(aP->parentP->stateX>bP->parentP->stateX) return  1;

  return 0;
}

static int latPutsItf( ClientData cd, int argc, char *argv[])
{
  Lat* latP   = (Lat*)cd;
  LNodeTimeList ** topsort;
  int i;
  HashIter iH;

  /* Calculate lookaheads ("full bigram search") */

  if(latP->startTimesStatesEndTimes.itemN==0) return TCL_OK;
  topsort = (LNodeTimeList **)calloc(latP->startTimesStatesEndTimes.itemN,sizeof(LNodeTimeList *));
  if(!topsort) { SERROR("Cannot alloc memory for sorting\n"); return TCL_ERROR; };
  
  for( hashIterInit(&(latP->startTimesStatesEndTimes),&iH), i=0;
       iH.Current;
       hashIterNext(&iH),i++) {
    topsort[i] = *(LNodeTimeList **)(iH.Current);
  }
  qsort(topsort, latP->startTimesStatesEndTimes.itemN, sizeof(LNodeTime *), LNodeTimeListCMP);

  for(i=0;i<latP->startTimesStatesEndTimes.itemN;i++) {
    LNodeTimeList * nP = topsort[i];
    itfAppendElement("%d %d %d %s %f %s",
		     nP->parentP->parentP->channelX,
		     nP->parentP->parentP->startFrameX,
		     nP->endFrameX,
		     latP->lm.lmP->nameFct(latP->lm.cd,nP->parentP->stateX),
		     nP->score,
		     nP->rep ? nP->rep : "");
  }
  free(topsort);
  return TCL_OK;
}


/* ------------------------------------------------------------------------
    latticeConfigureItf
   ------------------------------------------------------------------------ */

static int latConfigureItf( ClientData cd, char *var, char *val)
{
  Lat*    latP = (Lat*)cd;
  if (! latP) latP = &latDefault;

  if (! var) {
    ITFCFG(latConfigureItf,cd,"topN");
    ITFCFG(latConfigureItf,cd,"context");
    ITFCFG(latConfigureItf,cd,"maxPathN");
    ITFCFG(latConfigureItf,cd,"bigramLookahead");
    ITFCFG(latConfigureItf,cd,"maxHypTriedN");
    return TCL_OK;
  }
  ITFCFG_Int(      var,val,   "topN", latP->searchObject.n , 0)
  ITFCFG_Int(      var,val,   "context", latP->searchObject.context , 0);
  ITFCFG_Int(      var,val,   "maxPathN", latP->searchObject.maxPathN , 0);
  ITFCFG_Int(      var,val,   "bigramLookahead", latP->bigramLookahead , 0);
  ITFCFG_Int(      var,val,   "maxHypTriedN", latP->searchObject.maxHypTriedN , 0);

  return TCL_ERROR;
}

/* ------------------------------------------------------------------------
    latAccessItf
   ------------------------------------------------------------------------ */

static ClientData latAccessItf( ClientData cd, char *name, TypeInfo **ti)
{ 
  /*  Lat* latP = (Lat*)cd; */

  if (*name == '.') { 
    if (name[1]=='\0') {
      *ti = NULL; return NULL;
    }
    else { 
      /*
      if (! strcmp( name+1, "hypoList")) {
        *ti = itfGetType("HypoList");
        return (ClientData)&(latP->hypoList);
      }
      */
    }
  }
  return NULL;
}

/* ========================================================================
   Lat Initialization
   ======================================================================== */

static Method LatMethod[] = 
{ 
  { "puts",      latPutsItf,       NULL },
  { "add",       latAddItf,        "add lattice links"   },
  { "astar",    latSearchItf,     "run A^* search" },
  {  NULL,  NULL,           NULL } 
};

static TypeInfo LatInfo = { "Lat", 0, -1,     LatMethod, 
                                 latCreateItf,    latFreeItf,
                                 latConfigureItf, latAccessItf, 
                                 NULL,
	                        "Lat" } ;

static int lInitialized = 0;

int Lat_Init()
{
  if (! lInitialized) {
    
    itfNewType( &LatInfo);
    lInitialized = 1;
  }
  return TCL_OK;
}

