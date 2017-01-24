/* ========================================================================
    JANUS      Speech- to Speech Translation
               ------------------------------------------------------------
               Object: HMM of 3-grams
               ------------------------------------------------------------

    Author  :  Klaus Ries
    Module  :  hmm3gram.c
    Date    :  $Id: hmm3gram.c 2390 2003-08-14 11:20:32Z fuegen $
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
    Revision 1.5  2003/08/14 11:20:21  fuegen
    Merged changes on branch jtk-01-01-15-fms (jaguar -> ibis-013)

    Revision 1.4.20.2  2002/06/26 11:57:58  fuegen
    changes for new directory structure support and code cleaning

    Revision 1.4.20.1  2001/11/19 14:46:27  metze
    Go away, boring message!

    Revision 1.4  2000/11/14 12:29:33  janus
    Merging branch jtk-00-10-27-jmcd.

    Revision 1.3.34.2  2000/11/08 17:15:18  janus
    Finished making changes required for compilation under 'gcc -Wall'.

    Revision 1.3.34.1  2000/11/06 10:50:29  janus
    Made changes to enable compilation under -Wall.

    Revision 1.3  1998/08/02 11:26:29  kries
    produce meaningful representations in lattice generation

 * Revision 1.2  1998/06/11  14:14:26  kries
 * cosmetique changes
 *
 * Revision 1.1  1998/05/26  18:27:23  kries
 * Initial revision
 *
 
   ======================================================================== */

#define   SEARCH_MODULE

#include <stdlib.h>
#include "search.h"
#include "math.h"
#include "treefwd.h"
#include "lm.h"
#include "hmm3gram.h"


char hmm3gramRCSVersion[]= 
           "@(#)1$Id: hmm3gram.c 2390 2003-08-14 11:20:32Z fuegen $";


char * HMM3gramSubStateString(HMM3gramSubState subState) {

  switch(subState) {
  case HMM3gramWord: return "WORD";
  case HMM3gramStay: return "STAY";
  case HMM3gramTerminatedSpeechAct: return "TerminatedSpeechact";
  case HMM3gramInitial: return "Initial";
  }
  return "ERRROR";
}


/* ========================================================================
    Hmm3gramState
   ======================================================================== */

/* ------------------------------------------------------------------------
    HMM3gramItemInit
   ------------------------------------------------------------------------ */

void  HMM3gramCopyState(HMM3gram* hmmP, HMM3gramState *to, HMM3gramState *from);

int HMM3gramStateInit( HMM3gramState * stateP, HMM3gramState * initP)
{ 
  assert( stateP);
  HMM3gramCopyState(NULL, stateP, initP); 
  return TCL_OK;
}

/* ------------------------------------------------------------------------
     HMM3gramStateConfigureItf
   ------------------------------------------------------------------------ */

int HMM3gramStateConfigureItf( ClientData cd, char *var, char *val)
{
  HMM3gramState* stateP = (HMM3gramState*)cd;
  if (! stateP) return TCL_ERROR;

  if (! var) {
    ITFCFG(HMM3gramStateConfigureItf,cd,"stateX0");
    ITFCFG(HMM3gramStateConfigureItf,cd,"stateX1");
    ITFCFG(HMM3gramStateConfigureItf,cd,"stateX2");
    ITFCFG(HMM3gramStateConfigureItf,cd,"lastChange");
    ITFCFG(HMM3gramStateConfigureItf,cd,"timeStamp");
    ITFCFG(HMM3gramStateConfigureItf,cd,"subState");
    return TCL_OK;
  }
  ITFCFG_Int(     var,val,"stateX0",      stateP->stateX0,    1);
  ITFCFG_Int(     var,val,"stateX1",      stateP->stateX1,    1);
  ITFCFG_Int(     var,val,"stateX2",      stateP->stateX2,    1);
  ITFCFG_Int(     var,val,"lastChange",  stateP->lastChange,    1);
  ITFCFG_Int(     var,val,"timeStamp",  stateP->timeStamp,    1);
  ITFCFG_Int(     var,val,"subState",  stateP->subState,    1);
  ERROR("Unknown option '-%s %s'.\n", var, val ? val : "?");
  return TCL_ERROR;
}

/* ------------------------------------------------------------------------
    HMM3gramStateItemPuts/Itf
   ------------------------------------------------------------------------ */

int  HMM3gramStateItemPuts(HMM3gramState* stateP )
{
  itfAppendElement("STATE");
  itfAppendElement("%d", stateP->stateX0);
  itfAppendElement("STATE1");
  itfAppendElement("%d", stateP->stateX1);
  itfAppendElement("STATE2");
  itfAppendElement("%d", stateP->stateX2);
  itfAppendElement("LASTCHANGE");
  itfAppendElement("%d", stateP->lastChange);
  itfAppendElement("TIMESTAMP");
  itfAppendElement("%d", stateP->timeStamp);
  itfAppendElement("SUBSTATE");
  itfAppendElement("%s", HMM3gramSubStateString(stateP->subState));
  return TCL_OK;
}


int HMM3gramStatePutsItf( ClientData cd, int argc, char *argv[])
{
  int   ac   =  argc-1;
  HMM3gramState* stateP = (HMM3gramState*)cd;

  if ( itfParseArgv( argv[0], &ac, argv+1, 0, NULL) != TCL_OK)
       return TCL_ERROR;

  return HMM3gramStateItemPuts( stateP);
}

int     LISTHMM3gramStateEqFct( HMM3gramState *State1, HMM3gramState *State2) {
  /* substate is == Stay */

  if(State1 == State2)
    return 1;
  if(State1==0 || State2==0)
    return 0;
  return
            (State1->stateX0==State2->stateX0)
    &&      (State1->stateX1==State2->stateX1)
    &&      (State1->stateX2==State2->stateX2)
    &&   (State1->lastChange==State2->lastChange)
    &&    (State1->timeStamp==State2->timeStamp)
    &&     (State1->subState==State2->subState);
}


long   	LISTHMM3gramStateHashFct( HMM3gramState *State) {
  if(State) /* Substate == Stay */
    return (State->subState<<12) + (State->timeStamp<<8) + (State->lastChange<<6) + State->stateX0 + (State->stateX1<<2) + (State->stateX2<<4);
  else
    return 0;
}

int     HMM3gramStateEqFct( HMM3gram* hmmP, HMM3gramState *State1, HMM3gramState *State2) {
  if(State1 == State2)
    return 1;
  if(State1==0 || State2==0)
    return 0;

  if((State1->subState!=State2->subState)     ||
     (State1->timeStamp!=State2->timeStamp))
    return 0;

  if(State1->subState==HMM3gramWord) {
    if(State1->stateX2!=State2->stateX2) return 0;
    if(hmmP->hmmStates>1) {
      if(State1->stateX0!=State2->stateX0) return 0;
      if(hmmP->hmmStates>2)
	if(State1->stateX1!=State2->stateX1) return 0;
    }
  }
  else {
    if((State1->subState!=HMM3gramTerminatedSpeechAct) && (State1->lastChange!=State2->lastChange)) return 0;
    if(State1->stateX0!=State2->stateX0) return 0;
    if(hmmP->hmmStates>1) {
      if(State1->stateX1!=State2->stateX1) return 0;
      if(hmmP->hmmStates>2)
	if(State1->stateX2!=State2->stateX2) return 0;
    }
  }
  return 1;
}

long   	HMM3gramStateHashFct( HMM3gram* hmmP, HMM3gramState *State) {

  if(State) {
    long hash = (State->subState<<12) + (State->timeStamp<<8);
    if(State->subState==HMM3gramWord) {
      hash += State->stateX2;
      if(hmmP->hmmStates>1) {
	hash += State->stateX0<<2;
	if(hmmP->hmmStates>2) hash += (State->stateX1<<4);
      }
    }
    else {
      if(State->subState!=HMM3gramTerminatedSpeechAct) hash += State->lastChange<<6;
      hash += State->stateX0;
      if(hmmP->hmmStates>1) {
	hash += State->stateX1<<2;
	if(hmmP->hmmStates>2) hash += (State->stateX2<<4);
      }
    }
    return hash;
  }
  else
    return 0;
}

/* ========================================================================
    Hmm3gram
   ======================================================================== */

static HMM3gram hmm3gramDefault;

/* ------------------------------------------------------------------------
    Create/Init Hmm3gram Object
   ------------------------------------------------------------------------ */

/* ------------------------------------------------------------------------
    LModelItemInit
   ------------------------------------------------------------------------ */

int HMM3gramVocabItemInit(HMM3gramVocabItem* viP, ClientData CD)
{
  assert( viP);

  viP->name       =  ( CD) ? strdup((char*)CD) : NULL;
  viP->indexA     =    NULL;

  return TCL_OK;
}

/* ------------------------------------------------------------------------
    LModelItemDeinit
   ------------------------------------------------------------------------ */

int HMM3gramVocabItemDeinit(HMM3gramVocabItem * viP)
{
  if (!viP) return TCL_OK;
  if ( viP->name)   { free(viP->name);   viP->name   = NULL; }
  if ( viP->indexA)   { free(viP->indexA);   viP->indexA   = NULL; }
  return TCL_OK;
}


/* ------------------------------------------------------------------------
    hmm3gramInit
   ------------------------------------------------------------------------ */

HMM3gramState * HMM3gramSetState(HMM3gram* hmmP, HMM3gramState *current);

long latticeNodes_hashKey(HMM3gram* hmmP, HMM3gramLatticeNode * nP) {
  return nP->beginTime + (nP->endTime>>4) + (nP->state>>6) + (nP->final>>8);
}

int latticeNodes_hashCmp(HMM3gram* hmmP, HMM3gramLatticeNode * n1P, HMM3gramLatticeNode * n2P) {
  if(n1P==n2P) return 1;
  if(n1P==0 || n2P==0) return 0;
  return (n1P->beginTime == n2P->beginTime) &&
         (n1P->endTime   == n2P->endTime) &&
         (n1P->state     == n2P->state) &&
         (n1P->final     == n2P->final) ;
}

int latticeNodes_hashConflict(HMM3gram* hmmP, HMM3gramLatticeNode * n1P, HMM3gramLatticeNode * n2P) {
  if(n1P==n2P || n1P==0) return 0;
  if(n2P==0) return 1;
  return n1P->score<n2P->score;
}

void latticeNodes_deinit(ClientData p, ClientData nP) {
  free(((HMM3gramLatticeNode *) nP)->rep);
  free((HMM3gramLatticeNode *) nP);
}

/* ------------------------------------------------------------------------
    Free/Deinit Hmm3gram Object
   ------------------------------------------------------------------------ */

int hmm3gramLinkN( HMM3gram* hmmP)
{
  return hmmP->useN;
}

static int hmm3gramDeinit( HMM3gram* hmmP)
{
  int i;
  if ( hmm3gramLinkN( hmmP)) {
    SERROR("HMM3gram '%s' still in use by other objects.\n", hmmP->name);
    return TCL_ERROR;
  }
  if ( hmmP->name) { free( hmmP->name); hmmP->name = NULL; }

  /* unlinkLMPtr(&hmmP->hmm3grams); */

  if(hmmP->TriGrams) {    
  /*
    for(i=0;i<hmmP->statesN;i++)
      if(hmmP->TriGrams[i].cd!=NULL)
	unlinkLMPtr((&hmmP->TriGrams[i])); */
    free(hmmP->TriGrams);
  }

  for(i=0;i<hmmP->statesN+1;i++)
    free(hmmP->nextStates.itemA[i].state);
  free(hmmP->nextStates.itemA);

  hashClear(hmmP->latticeNodes); free(hmmP->latticeNodes); hmmP->latticeNodes=0;
  return TCL_OK;
}

int hmm3gramFree( HMM3gram* hmmP)
{
  if ( hmm3gramDeinit( hmmP) != TCL_OK) return TCL_ERROR;
  free(hmmP);
  return TCL_OK;
}

int hmm3gramFreeItf (ClientData clientData) 
{ return hmm3gramFree( (HMM3gram*)clientData); }


/* ------------------------------------------------------------------------
    Init/Create Hmm3gram Object
   ------------------------------------------------------------------------ */

int hmm3gramInit( HMM3gram* hmmP, char* name, LMPtr * lmP)
{
  int	i;

  hmmP->name   = strdup(name);
  hmmP->useN   = 0;

  hmmP->hmm3grams.cd = lmP->cd;
  hmmP->hmm3grams.lmP = lmP->lmP;

  /* Initalize the vocabulary */

  if ( listInit((List*)&(hmmP->vocab), NULL, sizeof(HMM3gramVocabItem), 5000) != TCL_OK)
       return TCL_ERROR;
  hmmP->vocab.init    = (ListItemInit  *)HMM3gramVocabItemInit;
  hmmP->vocab.deinit  = (ListItemDeinit*)HMM3gramVocabItemDeinit;  
  
  hmmP->lp = hmm3gramDefault.lp;
  hmmP->lz = hmm3gramDefault.lz;
  hmmP->cutoffN  = hmm3gramDefault.cutoffN;


  hmmP->statesN  = hmmP->hmm3grams.lmP->nameNFct(hmmP->hmm3grams.cd);
  hmmP->timeStamping = hmm3gramDefault.timeStamping;
  hmmP->hmmStates = hmm3gramDefault.hmmStates;
  
  if((hmmP->TriGrams = calloc(hmmP->statesN,sizeof(HMM3gramModel)))==0) {
    ERROR("Failed to allocate HMM3gram object '%s'.\n", name);
    return TCL_ERROR;
  }

  for(i=0;i<hmmP->statesN;i++) {
    lmPtrInit(&(hmmP->TriGrams[i].lm));
    hmmP->TriGrams[i].mean = hmmP->TriGrams[i].stddev = 0;
  }

  /* Initialize the state */

  
  hmmP->nextStates.itemN = 0;
  hmmP->nextStates.itemA = calloc(sizeof(LMStateTransition),hmmP->statesN+2);
  if(!hmmP->nextStates.itemA) FATAL("Cannot alloc memory for state transition\n");
  for(i=0;i<hmmP->statesN+1;i++)
    if(!(hmmP->nextStates.itemA[i].state = malloc(sizeof(HMM3gramState))))
      FATAL("Cannot alloc memory for state transition\n");

  if((hmmP->startStateX = hmmP->hmm3grams.lmP->indexFct( hmmP->hmm3grams.cd, "<s>")) <0) {
    ERROR("No start state <s>"); return TCL_ERROR;
  }
  if((hmmP->finalStateX = hmmP->hmm3grams.lmP->indexFct( hmmP->hmm3grams.cd, "</s>")) <0) {
    ERROR("No final state </s>"); return TCL_ERROR;
  }

  if(HMM3gramSetState(hmmP,0)==NULL)
    return TCL_ERROR;


  /* Initialize the lattice hash-structure */

  hmmP->latticeNodes = (Hash*)malloc(sizeof(Hash));
  if(hmmP->latticeNodes==NULL) {
    ERROR("Cannot allocate latticeNodes\n");
    hmm3gramDeinit(hmmP);
    return TCL_ERROR;
  }
  if(hashInit(hmmP->latticeNodes)!=TCL_OK)
    return TCL_ERROR;

  hmmP->latticeNodes->hashKey = (HashKeyFn2*)latticeNodes_hashKey;
  hmmP->latticeNodes->hashCmp = (HashCmpFn2*)latticeNodes_hashCmp;
  hmmP->latticeNodes->data_deinit  = latticeNodes_deinit;
  hmmP->latticeNodes->hashConflict = (HashConflictFn2*)latticeNodes_hashConflict;
  hmmP->latticeNodes->DataHolder = (ClientData)hmmP;

  hmmP->generateLattice = 0;

  return TCL_OK;
}

HMM3gram *hmm3gramCreate( char* name, LMPtr * lmP)
{
  HMM3gram *hmm3gramP = (HMM3gram*)malloc(sizeof(HMM3gram));

  if (! hmm3gramP || hmm3gramInit( hmm3gramP, name, lmP) != TCL_OK) {
     if ( hmm3gramP) free( hmm3gramP);
     ERROR("Failed to allocate HMM3gram object '%s'.\n", name);
     return NULL; 
  }
  return hmm3gramP;
}

ClientData hmm3gramCreateItf(ClientData clientData, 
                                   int argc, char *argv[])
{
  int       ac     =  argc - 1;
  LMPtr     lmP;

  lmPtrInit(&lmP);
  if ( itfParseArgv( argv[0], &ac, argv+1, 0,
      "<lm>",  ARGV_CUSTOM, getLMPtr, &lmP, NULL,"HMM",
       NULL) != TCL_OK) { lmPtrDeinit(&lmP); return NULL; }

  
  return (ClientData)hmm3gramCreate(argv[0], &lmP);

}

/* ------------------------------------------------------------------------
    hmm3gramConfigureItf
   ------------------------------------------------------------------------ */

int hmm3gramConfigureItf( ClientData cd, char *var, char *val)
{
  HMM3gram*    hmmP = (HMM3gram*)cd;
  if (! hmmP) hmmP = &hmm3gramDefault;

  if (! var) {

    ITFCFG(hmm3gramConfigureItf,cd,"useN");
    ITFCFG(hmm3gramConfigureItf,cd,"lz");
    ITFCFG(hmm3gramConfigureItf,cd,"lp");
    ITFCFG(hmm3gramConfigureItf,cd,"statesN");
    ITFCFG(hmm3gramConfigureItf,cd,"cutoffN");
    ITFCFG(hmm3gramConfigureItf,cd,"timeStamping");
    ITFCFG(hmm3gramConfigureItf,cd,"hmmStates");

    return TCL_OK;
  }
  ITFCFG_Int(    var,val,"useN",     hmmP->useN,             1);
  ITFCFG_Float(  var,val,"lz",       hmmP->lz,               0);
  ITFCFG_Float(  var,val,"lp",       hmmP->lp,               0);
  ITFCFG_Int(    var,val,"statesN",  hmmP->statesN,          1);
  ITFCFG_Int(    var,val,"cutoffN",  hmmP->cutoffN,          0);
  ITFCFG_Int(    var,val,"timeStamping",  hmmP->timeStamping,          0);
  ITFCFG_Int(    var,val,"hmmStates",          hmmP->hmmStates,        0);
  return TCL_ERROR;
}

/* ------------------------------------------------------------------------
    hmm3gramAccessItf
   ------------------------------------------------------------------------ */

ClientData hmm3gramAccessItf( ClientData cd, char *name, TypeInfo **ti)
{ 
  HMM3gram* hmmP = (HMM3gram*)cd;
  int i;
  LMPtr * current = &(hmmP->TriGrams[hmmP->currentState.stateX0].lm);
		      
  if(!current->cd)
    current=NULL;

  if (*name == '.') { 
    if (name[1]=='\0') {
      if ( hmmP->hmm3grams.cd  ) itfAppendElement( "hmm");
      if(current!=NULL) itfAppendElement( "current");
      itfAppendElement("currentState");
      *ti = NULL;
    }
    else {  
      if ( ! strcmp( name+1, "hmm")) {
        *ti = hmmP->hmm3grams.lmP->tiP;
        return (ClientData)hmmP->hmm3grams.cd;
      }
      if(! strcmp( name+1, "current")) {
	if(!current)
	  return NULL;
	*ti = current->lmP->tiP;
	return current->cd;
      }
      if(! strcmp( name+1, "currentState")) {
	*ti = itfGetType("HMM3gramState");
	return (ClientData)&(hmmP->currentState);
      }
    }
  }
  if (*name == ':') { 
    if (name[1]=='\0') {      
      for ( i = 0; i <hmmP->statesN; i++) {
	if(hmmP->TriGrams[i].lm.cd!=NULL)
	  itfAppendElement( "%s",hmmP->hmm3grams.lmP->nameFct(hmmP->hmm3grams.cd,i));
	*ti = NULL;
      }
    }	
    else {
      name++;
      if ((i = hmmP->hmm3grams.lmP->indexFct(hmmP->hmm3grams.cd,name)) >= 0) {
	if(hmmP->TriGrams[i].lm.cd==NULL) return NULL;
	*ti = hmmP->TriGrams[i].lm.lmP->tiP;
	return (ClientData)hmmP->TriGrams[i].lm.cd;
      }
    }
  }
  return NULL;
}

/* ------------------------------------------------------------------------
    hmm3gramPutsItf
   ------------------------------------------------------------------------ */

int hmm3gramPutsItf( ClientData cd, int argc, char *argv[])
{
  /*  HMM3gram* hmmP   = (HMM3gram*)cd; */
  return TCL_OK;
}

/* ------------------------------------------------------------------------
    HMM3gramIndex
   ------------------------------------------------------------------------ */

int HMM3gramIndex( HMM3gram* hmmP,  char* name) 
{
  return listIndex((List*)&(hmmP->vocab), name, 0);
}

int HMM3gramIndexItf( ClientData clientData, int argc, char *argv[] )
{
  HMM3gram* hmmP = (HMM3gram*)clientData;
  return listName2IndexItf((ClientData)&(hmmP->vocab), argc, argv);
}

/* ------------------------------------------------------------------------
    HMM3gramName
   ------------------------------------------------------------------------ */

char* HMM3gramName( HMM3gram* hmmP, int i) 
{
  return (i < 0) ? "(null)" : hmmP->vocab.itemA[i].name;
}

int HMM3gramNameItf( ClientData clientData, int argc, char *argv[] )
{
  HMM3gram* hmmP =  (HMM3gram*)clientData;
  return listIndex2NameItf((ClientData)&(hmmP->vocab), argc, argv);
}

/* ------------------------------------------------------------------------
    HMM3gramNameN
   ------------------------------------------------------------------------ */

int HMM3gramNameN( HMM3gram* hmmP) 
{
  return hmmP->vocab.itemN;
}


/* ------------------------------------------------------------------------
    HMM3gramScore    compute the score of the 3G language model for a
                   sequence of n words.
   ------------------------------------------------------------------------ */

float HMM3gramScore( HMM3gram* hmmP, int* w, int n, int m)
{  
  IArray w2;
  int i,j;
  float prob=0;
  LMPtr * lmP = &(hmmP->TriGrams[hmmP->currentState.stateX0].lm);
  int stateX = hmmP->currentState.stateX0;

  

  if(lmP->cd==NULL) {
    /* There are no probabilities associated  with this state */

    FATAL("No language model defined for state %s(%ld)\n",
	  hmmP->hmm3grams.lmP->nameFct(hmmP->hmm3grams.cd,hmmP->currentState.stateX0),
	  hmmP->currentState.stateX0);
    return -1;
  }

  /* Check for end of utterance -- make transition in </s> state */
  if(w[n-1] == hmmP->vocabFinalX) {
    int transition[4];

    if(n==m+1 && hmmP->currentState.lastChange==0)
      return 99999; /* Enforce one word per speech act constraint */
    
    transition[0] = hmmP->currentState.stateX2; transition[1] = hmmP->currentState.stateX1;
    transition[2] = hmmP->currentState.stateX0; transition[3] = hmmP->finalStateX;
    
    prob += hmmP->hmm3grams.lmP->scoreFct(hmmP->hmm3grams.cd,transition,4,3);
  }	


  iarrayInit(&w2,n+1); i=j=0;

  if(hmmP->currentState.lastChange<hmmP->cutoffN && hmmP->currentState.lastChange<m ) {
    while(j<m-hmmP->currentState.lastChange)
      w2.itemA[j++]=hmmP->vocab.itemA[w[i++]].indexA[stateX];   
    w2.itemA[j++]=hmmP->vocab.itemA[hmmP->vocabStartX].indexA[stateX]; 
    n++; m++;
  }
  while(j<n)
    w2.itemA[j++]=hmmP->vocab.itemA[w[i++]].indexA[stateX];   

  prob += lmP->lmP->scoreFct(lmP->cd,w2.itemA,n,m);
  iarrayDeinit(&w2);


  return hmmP->lz * prob + (n-m) * hmmP->lp;
}

int HMM3gramScoreItf( ClientData cd, int argc, char * argv[])
{
  HMM3gram*   hmmP    = (HMM3gram *) cd;
  IArray    warray = {NULL, 0};
  int       ac     =  argc-1;
  int       i      =  0;

  if (itfParseArgv( argv[0], &ac,  argv+1,  0,
     "<word sequence>", ARGV_LIARRAY, NULL, &warray, 
     &(hmmP->vocab), "sequence of words", 
     "-idx", ARGV_INT, NULL, &i, NULL, "index of cond. prob.",
     NULL) != TCL_OK) {
    if ( warray.itemA) free( warray.itemA);
    return TCL_ERROR;
  }

  itfAppendElement( "%f", HMM3gramScore(hmmP, warray.itemA, warray.itemN, i));

  if ( warray.itemA) free( warray.itemA);
  return TCL_OK;
} 

/* ------------------------------------------------------------------------
    HMM3gramNextFrame   update cache frame index
   ------------------------------------------------------------------------ */

int HMM3gramNextFrame( HMM3gram* lmP)
{
  int i;
  for(i=0;i<lmP->statesN;i++)
    if(lmP->TriGrams[i].lm.cd)
      if(TCL_OK!=lmP->TriGrams[i].lm.lmP->nextFrameFct(lmP->TriGrams[i].lm.cd))
	return TCL_ERROR;
  return TCL_OK;
}

int HMM3gramNextFrameItf( ClientData cd, int argc, char * argv[])
{
  HMM3gram*   lmP    = (HMM3gram *) cd;
  int       ac     =  argc-1;

  if (itfParseArgv( argv[0], &ac,  argv+1,  0,
       NULL) != TCL_OK) return TCL_ERROR;

  return HMM3gramNextFrame(lmP);
} 

/* ------------------------------------------------------------------------
    HMM3gramReset   reset language model cache
   ------------------------------------------------------------------------ */

int HMM3gramReset( HMM3gram* lmP)
{
  int i;
  for(i=0;i<lmP->statesN;i++)
    if(lmP->TriGrams[i].lm.cd)
      if(TCL_OK!=lmP->TriGrams[i].lm.lmP->resetFct(lmP->TriGrams[i].lm.cd))
	return TCL_ERROR;
  return TCL_OK;
}

int HMM3gramResetItf( ClientData cd, int argc, char * argv[])
{
  HMM3gram*   lmP    = (HMM3gram *) cd;
  int       ac     =  argc-1;

  if (itfParseArgv( argv[0], &ac,  argv+1,  0,
       NULL) != TCL_OK) return TCL_ERROR;

  return HMM3gramReset(lmP);
} 

/* ------------------------------------------------------------------------
    hmm3gramAddItf      add a TriGram
   ------------------------------------------------------------------------ */

int hmm3gramAdd(HMM3gram* hmmP, int stateX, LMPtr * lmP, float mean, float stddev) {
  int i,j;
  int namesN = hmmP->vocab.itemN;

  /* Initalize the language model per se */

  /*
  if(hmmP->TriGrams[stateX].lm.cd)
    unlinkLMPtr(&(hmmP->TriGrams[stateX].lm));
  */


  if(stateX<0 || stateX>=hmmP->statesN) {
    FATAL("stateX=%ld (hmmP->statesN=%ld)\n",stateX,hmmP->statesN);
  }

  hmmP->TriGrams[stateX].lm.cd=lmP->cd;
  hmmP->TriGrams[stateX].lm.lmP=lmP->lmP;
  hmmP->TriGrams[stateX].mean=mean;
  hmmP->TriGrams[stateX].stddev=stddev;

  /*
  linkLMPtr(&(hmmP->TriGrams[stateX].lm));
  */

  /* Map the vocabularies onto the standard format */

  if(namesN==0) { /* No vocabulary defined yet, add "base" vocabulary */
    namesN = lmP->lmP->nameNFct(lmP->cd);
    for(i=0;i<namesN;i++) {      
      int idx = listIndex((List*)&(hmmP->vocab),lmP->lmP->nameFct(lmP->cd,i),1);
      if ( idx != i) {
	ERROR("Could not add vocabulary mapping %ld(idx)!=%ld(i)\n",idx,i);
	return TCL_ERROR;
      }
      if((hmmP->vocab.itemA[i].indexA=(int*)malloc(sizeof(int)*hmmP->statesN))==NULL) {
	ERROR("Could not allocate memory of the vocabulary mapping\n");
	return TCL_ERROR;
      }
      for(j=0;j<hmmP->statesN;j++) hmmP->vocab.itemA[i].indexA[j]=0;
      hmmP->vocab.itemA[i].indexA[stateX]=i;
    }
  
    if((hmmP->vocabStartX = listIndex((List*)&(hmmP->vocab),(ClientData)"<s>",0)) <0) {
      ERROR("No start element in vocabulary <s>"); return TCL_ERROR;
    }
    if((hmmP->vocabFinalX = listIndex((List*)&(hmmP->vocab),(ClientData)"</s>",0)) <0) {
      ERROR("No final element in vocabulary </s>"); return TCL_ERROR;
    }
  }
  else { /* There is already a vocabulary defined, so we have to define a
	    mapping */    
    for(i=0;i<namesN;i++) {      
      hmmP->vocab.itemA[i].indexA[stateX] =
	lmP->lmP->indexFct(lmP->cd,hmmP->vocab.itemA[i].name);
      if(hmmP->vocab.itemA[i].indexA[stateX]<0) {
	ERROR("%s not in all vocabularies\n",hmmP->vocab.itemA[i].name);
	return TCL_ERROR;
      }
    }
  }

  return TCL_OK;
}   
	
int HMM3gramInitLatticeItf(ClientData cd, int argc, char *argv[])
{
  HMM3gram* hmmP   = (HMM3gram*)cd;
  int       ac     =  argc - 1;


  if ( itfParseArgv( argv[0], &ac, argv+1, 0,
       NULL) != TCL_OK) {
    return TCL_ERROR;
  }
  hashClear(hmmP->latticeNodes); hmmP->generateLattice++;
  return TCL_OK;
}



static int HMM3gramLatticeNodeCMP2 (const void *a, const void *b) 
{
  HMM3gramLatticeNode * aP = *(HMM3gramLatticeNode **)a;
  HMM3gramLatticeNode * bP = *(HMM3gramLatticeNode **)b;

  if(aP->beginTime<bP->beginTime) return -1;
  if(aP->beginTime>bP->beginTime) return  1;

  if(aP->endTime<bP->endTime) return -1;
  if(aP->endTime>bP->endTime) return  1;

  if(aP->state<bP->state) return -1;
  if(aP->state>bP->state) return  1;

  if(aP->final<bP->final) return -1;
  if(aP->final>bP->final) return  1;

  return 0;
}

static int HMM3gramLatticeNodeCMP1 (const void *a, const void *b) 
{
  HMM3gramLatticeNode * aP = *(HMM3gramLatticeNode **)a;
  HMM3gramLatticeNode * bP = *(HMM3gramLatticeNode **)b;

  if(aP->endTime<bP->endTime) return -1;
  if(aP->endTime>bP->endTime) return  1;

  if(aP<bP) return -1;
  if(aP>bP) return  1;

  return 0;
}

static long  timesHashKey( ClientData cd, ClientData iP) { return *(int*)iP; }
static int   timesHashCmp( ClientData cd, ClientData iP1, ClientData iP2) { return *(int*)iP1==*(int*)iP2; }
static void  timesHashDeinit( ClientData cd, ClientData iP) { free((int*)iP); }

int HMM3gramFinalLatticeItf(ClientData cd, int argc, char *argv[])
{
  HMM3gram* hmmP   = (HMM3gram*)cd;
  LMPtr *   lmP    =  &(hmmP->hmm3grams);
  int       ac     =  argc - 1;
  HashIter iH;
  HMM3gramLatticeNode ** nodeA;
  int i;
  int * nodeP; Hash times;

  if ( itfParseArgv( argv[0], &ac, argv+1, 0,
       NULL) != TCL_OK) {
    return TCL_ERROR;
  }

  if(hmmP->latticeNodes->itemN==0)
    return TCL_OK;

  /* Extract and sort the nodes */

  if(!(nodeA = (HMM3gramLatticeNode **)malloc(sizeof(HMM3gramLatticeNode *)*hmmP->latticeNodes->itemN)))
    FATAL("Cannot allocate lattice\n");
  for( hashIterInit(hmmP->latticeNodes,&iH),i=0; iH.Current; hashIterNext(&iH),i++)
    nodeA[i] = *(HMM3gramLatticeNode **)iH.Current;

  assert(i==hmmP->latticeNodes->itemN);

  /* Sort only by end-time */
  qsort( nodeA, hmmP->latticeNodes->itemN, sizeof(HMM3gramLatticeNode *), HMM3gramLatticeNodeCMP1);

  /* Find nodes reachable by a final state (all nodes are reachable by the initial state by definition) */

  hashInit(&times);

  times.hashKey     = timesHashKey;
  times.hashCmp     = timesHashCmp;
  times.data_deinit = timesHashDeinit;

  if(!(nodeP = (int*)malloc(sizeof(int)))) FATAL("Cannot allocate lattice\n");


  for( i=hmmP->latticeNodes->itemN-1; i>=0; i--) {
    if(nodeA[i]->final) {   /* final nodes are always reachable */
      *nodeP = nodeA[i]->beginTime; hashItem( &times, nodeP, 1);
      if(!(nodeP = (int*)malloc(sizeof(int)))) FATAL("Cannot allocate lattice\n");
    }
  }    
  for( i=hmmP->latticeNodes->itemN-1; i>=0; i--) {
    *nodeP = nodeA[i]->endTime; 
    if(hashItem( &times, nodeP, 0)) { /* reachable targets generate reachable sources */
      *nodeP = nodeA[i]->beginTime; hashItem( &times, nodeP, 1);
      if(!(nodeP = (int*)malloc(sizeof(int)))) FATAL("Cannot allocate lattice\n");
    }
  }

  /* Do a normal sort */
  qsort( nodeA, hmmP->latticeNodes->itemN, sizeof(HMM3gramLatticeNode *), HMM3gramLatticeNodeCMP2);

  for(i=0;i<hmmP->latticeNodes->itemN;i++) {
    *nodeP = nodeA[i]->endTime;
    if((nodeA[i]->final || hashItem( &times, nodeP, 0)) && nodeA[i]->beginTime!=nodeA[i]->endTime) {     
      itfAppendResult("{ %ld %ld %s %f %ld {%s} } ",
		      nodeA[i]->beginTime,nodeA[i]->endTime,
		      lmP->lmP->nameFct(lmP->cd,nodeA[i]->state),
		      nodeA[i]->score,
		      nodeA[i]->final,
		      nodeA[i]->rep);
    }
  }


  free(nodeP);
  hashClear(&times);
  
  hmmP->generateLattice=0;
  free(nodeA); hashClear(hmmP->latticeNodes);
  return TCL_OK;
}

int hmm3gramAddItf(ClientData cd, int argc, char *argv[])
{
  HMM3gram* hmmP   = (HMM3gram*)cd;
  int       ac     =  argc - 1;

  int 	    stateX=-1;
  LMPtr     lmP;
  int       ret;
  float     mean=0, stddev=0;


  lmPtrInit( &lmP);

  if ( itfParseArgv( argv[0], &ac, argv+1, 0,
      "<state>",  ARGV_CUSTOM, LMPtrIndexItf, &stateX,  &(hmmP->hmm3grams), 
                                                  "state of the HMM of 3gram",
      "<lm>",  ARGV_CUSTOM, getLMPtr, &lmP, NULL,"language model (3gram)",
      "-mean", ARGV_FLOAT,  NULL,     &mean, NULL,"mean of utterance length",
      "-stddev",  ARGV_FLOAT, NULL, &stddev,  NULL,"standard deviation of utterance length",
       NULL) != TCL_OK) {
    lmPtrDeinit( &lmP);
    return TCL_ERROR;
  }
  else {
    ret = hmm3gramAdd( hmmP, stateX, &lmP, mean, stddev);
    lmPtrDeinit( &lmP);
  }
  return TCL_OK;
}

LMStateTransitionArray *  HMM3gramGetNextState(HMM3gram* hmmP, IArray * warray, int timeStamp);
int hmm3gramGetNextStateItf(ClientData cd, int argc, char *argv[])
{
  HMM3gram* hmmP   = (HMM3gram*)cd;
  int       ac     =  argc - 1;

  IArray    warray = {NULL, 0};

  LMStateTransitionArray *     nextState;
  int i;
  int timeStamp;

  if (itfParseArgv(argv[0], &ac, argv+1, 0,
      "<word sequence>", ARGV_LIARRAY, NULL, &warray, 
		     &(hmmP->vocab), "sequence of words",
      "-time", ARGV_INT, NULL, &warray, 
		     &(timeStamp), "sequence of words",
		   NULL) != TCL_OK)
    return TCL_ERROR;

  nextState = HMM3gramGetNextState(hmmP, &warray, timeStamp);

  for( i=0; i<nextState->itemN;i++) {
    HMM3gramState * stateP = (HMM3gramState *)nextState->itemA[i].state;
    itfAppendElement("lastChange %ld stateX0 %ld stateX1 %ld stateX2 %ld  timeStamp %ld subState %s penalty %f",
		     stateP->lastChange,
		     stateP->stateX0, stateP->stateX1, stateP->stateX2,
		     timeStamp, HMM3gramSubStateString(stateP->subState),
		     nextState->itemA[i].penalty
		     );
  }
  iarrayDeinit(&warray);
  return TCL_OK;
}


/* ------------------------------------------------------------------------
    hmm3gram State control functions
   ------------------------------------------------------------------------ */

int HMM3gramStateSize(HMM3gram*  hmmP) {
  return sizeof(HMM3gramState);
}


char * HMM3gramPathRepresentation(HMM3gram* hmmP, AStar * searchObjectP, AStarPath* pathP, char * str) {
  char * strNew = str;
  char * entry;

  if(!pathP) return str;
  if(!pathP->StateMem) strNew = HMM3gramPathRepresentation(hmmP, searchObjectP, pathP->parentP, str);

  entry =  HMM3gramName(hmmP,searchObjectP->nodeElement(searchObjectP, pathP->AStarElement));
  
  if(strNew!=str)
    *(strNew++) = ' ';
  while(*entry) {
    *(strNew++) = *(entry++);
  };
  *strNew = '\000';

  return strNew;
}

void HMM3gramPathCreateFct(HMM3gram* hmmP, AStar * searchObjectP, AStarNode* anodeP) {
  if(anodeP && hmmP->generateLattice) {
    HMM3gramState * current = (HMM3gramState *)anodeP->State;

    if(current->subState==HMM3gramTerminatedSpeechAct || searchObjectP->pathEnd(searchObjectP,anodeP)) {
      char temp[1000000];
      HMM3gramLatticeNode * nodeP = malloc(sizeof(HMM3gramLatticeNode));
      if(!nodeP) FATAL("Cannot create lattice node");
      nodeP->beginTime = current->timeStamp;
      nodeP->endTime   = searchObjectP->nodeTime(searchObjectP,anodeP);
      nodeP->score     = anodeP->score;
      nodeP->final     = searchObjectP->pathEnd(searchObjectP,anodeP);
      nodeP->state     = current->stateX0;


      /* Use concatentation of lower representations */
      temp[0]='\000';
      HMM3gramPathRepresentation(hmmP, searchObjectP, anodeP->pathP, temp); 
      nodeP->rep       = strdup(temp);

      hashItem( hmmP->latticeNodes,  (ClientData) nodeP, 1);
    }
  }
}  

LMStateTransitionArray *  HMM3gramGetNextState(HMM3gram* hmmP, IArray * warray, int timeStamp) {

  HMM3gramState * current = &(hmmP->currentState);

  int i;


  /* Recalculate the follower states */

  /* Clear the old results */
  hmmP->nextStates.itemN = 0;

  if(hmmP->finalStateX==hmmP->currentState.stateX0) /* final state reached */
    return &(hmmP->nextStates);

  /* Foreach targetstate do an actions */

  /* go to " stay - state " which consumes a word */
  if((current->subState==HMM3gramStay) || (current->subState==HMM3gramWord)) {
    HMM3gramState * nextP = (HMM3gramState *)hmmP->nextStates.itemA[hmmP->nextStates.itemN].state;

    nextP->subState = HMM3gramStay;
    if(current->subState == HMM3gramWord) {
      nextP->lastChange = 0;

      nextP->timeStamp = hmmP->timeStamping ? timeStamp : current->timeStamp;
      
      nextP->stateX0   = hmmP->currentState.stateX2;
      nextP->stateX1   = hmmP->hmmStates>1  ? hmmP->currentState.stateX0 : hmmP->startStateX;
      nextP->stateX2   = hmmP->hmmStates>2  ? hmmP->currentState.stateX1 : hmmP->startStateX;
    }
    else {
      nextP->lastChange = current->lastChange+1;
      if(hmmP->cutoffN>=0 && nextP->lastChange>hmmP->cutoffN)
	nextP->lastChange = hmmP->cutoffN;
      nextP->timeStamp = current->timeStamp;

      nextP->stateX0 = current->stateX0;
      nextP->stateX1 = current->stateX1;
      nextP->stateX2 = current->stateX2;      
    }
    hmmP->nextStates.itemA[hmmP->nextStates.itemN++].penalty = 0;
  }

  /* Assume we stay in the current state */
  
  /* go to " terminateSpeechAct - state " which does not consume a word */
  /* Calculate end-of-segment penalty first */

  /* Note that we enforce that at least one word is contained in a speech act */
  /* therefore termination is only possible from the HMM3gramStay state       */

  if(current->subState==HMM3gramStay ) {
   /* Calculate "moving to next state penalty" */

    float  endOfStatePenalty=0;
    LMPtr  * lmP = &(hmmP->TriGrams[hmmP->currentState.stateX0].lm);
    HMM3gramState * nextP = (HMM3gramState *)hmmP->nextStates.itemA[hmmP->nextStates.itemN].state;

    if(lmP->cd!=NULL) {
      int lastChange = hmmP->currentState.lastChange+1;
      IArray endOfState;
      int i=0,j=0;
      int m;
      int * w = warray->itemA;
      int stateX = hmmP->currentState.stateX0;

      iarrayInit(&endOfState,warray->itemN+2);


      m=warray->itemN;

      if(lastChange<hmmP->cutoffN && lastChange<m ) {
	while(j<m-lastChange)
	  endOfState.itemA[j++]=hmmP->vocab.itemA[w[i++]].indexA[stateX];   
	endOfState.itemA[j++]=hmmP->vocab.itemA[hmmP->vocabStartX].indexA[stateX]; 
	m++;
      }
      while(j<m)
	endOfState.itemA[j++]=hmmP->vocab.itemA[w[i++]].indexA[stateX];   

      /* append </s> */
      endOfState.itemA[j++] = hmmP->vocab.itemA[hmmP->vocabFinalX].indexA[stateX];

      endOfStatePenalty = lmP->lmP->scoreFct(lmP->cd,endOfState.itemA,j,j-1);
      iarrayDeinit(&endOfState);
    }

    nextP->stateX0 = current->stateX0;
    nextP->stateX1 = current->stateX1;
    nextP->stateX2 = current->stateX2;      
    nextP->subState   = HMM3gramTerminatedSpeechAct;
    nextP->lastChange = current->lastChange;
    nextP->timeStamp  = current->timeStamp;
    hmmP->nextStates.itemA[hmmP->nextStates.itemN++].penalty =endOfStatePenalty;
  }
   
  /* go to " newWord - state " which consumes no word */
  /* this state is basically just the result of moving to a new speech act after the
     decision to leave the current one is done */

  if(current->subState==HMM3gramTerminatedSpeechAct || current->subState==HMM3gramInitial ) {

    /* Move on to next state */
    int transition[4];

    transition[2] = hmmP->currentState.stateX0;
    transition[1] = hmmP->currentState.stateX1;
    transition[0] = hmmP->currentState.stateX2;

    for(transition[3]=0;transition[3]<hmmP->statesN;transition[3]++) {
      HMM3gramState * nextP = (HMM3gramState *)hmmP->nextStates.itemA[hmmP->nextStates.itemN].state;
      if(hmmP->TriGrams[transition[3]].lm.cd) {
	if(current->subState==HMM3gramTerminatedSpeechAct) {
	  nextP->stateX0 = current->stateX0; nextP->stateX1 = current->stateX1;
	  nextP->stateX2 = transition[3];        /* Dirty trick warning: Remember to-state in stateX2 !!! */
	  nextP->subState   = HMM3gramWord;
	  nextP->lastChange = current->lastChange; nextP->timeStamp  = current->timeStamp;
	}
	else {
	  nextP->stateX0 = transition[3];
	  nextP->stateX1 = hmmP->hmmStates>1  ? hmmP->currentState.stateX0 : hmmP->startStateX;
	  nextP->stateX2 = hmmP->hmmStates>2  ? hmmP->currentState.stateX1 : hmmP->startStateX;
	  nextP->subState   = HMM3gramStay;
	  nextP->lastChange = 0; nextP->timeStamp  = hmmP->timeStamping ? timeStamp : current->timeStamp;
	}
	hmmP->nextStates.itemA[hmmP->nextStates.itemN++].penalty =
	  hmmP->hmm3grams.lmP->scoreFct(hmmP->hmm3grams.cd,transition,4,3);
      }
    }
  }

  /* Now fold in lz parameter */

  for(i=0;i<hmmP->nextStates.itemN;i++)
    hmmP->nextStates.itemA[i].penalty*=hmmP->lz;
 
  return &(hmmP->nextStates);
}

int    HMM3gramStatePrintFct( HMM3gram* hmmP, HMM3gramState *State, char * printString) {
  
  if(hmmP->hmm3grams.cd==0) return TCL_ERROR;
  if(State)
    sprintf(printString,"%s %s %s %d %d %s",
	    hmmP->hmm3grams.lmP->nameFct(hmmP->hmm3grams.cd,State->stateX0),
	    hmmP->hmm3grams.lmP->nameFct(hmmP->hmm3grams.cd,State->stateX1),
	    hmmP->hmm3grams.lmP->nameFct(hmmP->hmm3grams.cd,State->stateX2),
	    State->lastChange,
	    State->timeStamp,
	    HMM3gramSubStateString(State->subState));
  else {
    char * start = hmmP->hmm3grams.lmP->nameFct(hmmP->hmm3grams.cd,hmmP->startStateX);
    sprintf(printString,"%s %s %s %d %d %s",
	    start,start,start,0,0,HMM3gramSubStateString(HMM3gramInitial));
  }
  return TCL_OK;
}


void HMM3gramCopyState(HMM3gram* hmmP, HMM3gramState *to, HMM3gramState *from) {
  if(from) {
    to->stateX0 = from->stateX0; to->stateX1 = from->stateX1; to->stateX2 = from->stateX2;
    to->lastChange = from->lastChange;
    to->timeStamp = from->timeStamp;
    to->subState = from->subState;
  }
  else {
    to->stateX0 = to->stateX1 = to->stateX2 = hmmP->startStateX;
    to->lastChange = 0; to->timeStamp = 0; to->subState = HMM3gramInitial;
  }    
  if(hmmP) {
    if(to->stateX0>=hmmP->statesN) ERROR("StateX exceeds the number of states in model (%d>=%d)\n",to->stateX0,hmmP->statesN);
    if(to->stateX1>=hmmP->statesN) ERROR("StateX exceeds the number of states in model (%d>=%d)\n",to->stateX1,hmmP->statesN);
    if(to->stateX2>=hmmP->statesN) ERROR("StateX exceeds the number of states in model (%d>=%d)\n",to->stateX2,hmmP->statesN);
  }
}

HMM3gramState * HMM3gramSetState(HMM3gram* hmmP, HMM3gramState *current) {
  HMM3gramCopyState(hmmP,&(hmmP->currentState),current);
  return &(hmmP->currentState);
}
  
int HMM3gramFinalState(HMM3gram* hmmP) {
  return hmmP->currentState.subState==HMM3gramStay;
}

int hmm3gramSetStateItf(ClientData cd, int argc, char *argv[])
{
  HMM3gram* hmmP   = (HMM3gram*)cd;
  int       ac     =  argc - 1;
  int       state;

  HMM3gramState current;
  current.stateX0 =  hmmP->currentState.stateX0;
  current.stateX2 =  hmmP->currentState.stateX1;
  current.stateX2 =  hmmP->currentState.stateX2;
  current.lastChange =  hmmP->currentState.lastChange;
  current.timeStamp =  hmmP->currentState.timeStamp;
  current.subState = hmmP->currentState.subState;

  if ( itfParseArgv( argv[0], &ac, argv+1, 0,
      "<stateX0>", ARGV_SHORT, NULL, &(current.stateX0), NULL, "index of state of the HMM of 3gram",
      "<stateX1>", ARGV_SHORT, NULL, &(current.stateX1), NULL, "index of state of the HMM of 3gram",
      "<stateX2>", ARGV_SHORT, NULL, &(current.stateX2), NULL, "index of state of the HMM of 3gram",
      "<lastChange>",  ARGV_SHORT, NULL, &(current.lastChange),  NULL, "last change",
      "<timeStamp>",   ARGV_INT, NULL, &(current.timeStamp),  NULL, "timeStamp",
      "-subState",   ARGV_INT, NULL, &(state),  NULL, "subState",		     
       NULL) != TCL_OK)
    return TCL_ERROR;

  

  if(HMM3gramSetState(hmmP, &current)==NULL)
    return TCL_ERROR;
  return TCL_OK;
}

HMM3gramState * HMM3gramStateCreateFct(HMM3gram * hmmP) {
  HMM3gramState * ret = malloc(sizeof(HMM3gramState));
  if(ret==NULL) FATAL("Cannot allocate HMM3gramState\n");
  return ret;
}

void HMM3gramStateFreeFct(HMM3gram * hmmP, HMM3gramState * stateP) { free(stateP); }

char * HMM3gramStateMemCreateFct(HMM3gram * hmmP, HMM3gramState * stateP) {
   if(stateP==NULL || stateP->lastChange!=0) return NULL;
   return hmmP->hmm3grams.lmP->nameFct(hmmP->hmm3grams.cd,stateP->stateX0);
}


void HMM3gramStateMemFreeFct(ClientData cd, LMStateMem mem) { }

int    HMM3gramStateMemPrintFct( HMM3gram * hmmP, char * StateMem, char * printString) {
  if(StateMem) strcpy(printString,StateMem); else strcpy(printString,"");
  return TCL_OK;
}


/* ========================================================================
    Hmm3gram Initialization
   ======================================================================== */


Method hmm3gramMethod[] = 
{ 
  { "puts",      hmm3gramPutsItf,       NULL },
  { "add",       hmm3gramAddItf,      	"Add a 3gram model"},
  { "set",       hmm3gramSetStateItf,   "Set the state of model"},
  { "next",      hmm3gramGetNextStateItf,"Return follower states with penalties"},

  { "index",  HMM3gramIndexItf,  "return the internal index of an HMM3gramItem" },
  { "name",   HMM3gramNameItf,   "return the name of an HMM3gramItem" },
  { "score",  HMM3gramScoreItf,  "return the score of a text string"     },

  { "nextFrame", HMM3gramNextFrameItf,  "incr. cache frame index"  },
  { "reset",     HMM3gramResetItf,      "reset language model"     },


  { "initLattice", HMM3gramInitLatticeItf,  "initialize a lattice generation run"  },
  { "finalLattice", HMM3gramFinalLatticeItf,  "end a lattice generation run"  },

  {  NULL,  NULL,           NULL } 
};

TypeInfo HMM3gramInfo = { "HMM3gram", 0, 1,     hmm3gramMethod, 
                                 hmm3gramCreateItf,    hmm3gramFreeItf,
                                 hmm3gramConfigureItf, hmm3gramAccessItf, 
			         NULL,
	                        "HMM3gram" } ;

LM HMM3gramLM = 
       { "HMM3gramLM", &HMM3gramInfo, &HMM3gramInfo,
         (LMIndexFct*)    HMM3gramIndex,
         (LMNameFct *)    HMM3gramName,
         (LMNameNFct *)   HMM3gramNameN,
         (LMScoreFct*)    HMM3gramScore,
         (LMNextFrameFct*)HMM3gramNextFrame,
         (LMResetFct*)    HMM3gramReset,
	 (LMStateCreateFct*) HMM3gramStateCreateFct,
	 (LMStateFreeFct*) HMM3gramStateFreeFct,
	 (LMStatePrintFct*) HMM3gramStatePrintFct,
	 (LMSetStateFct*) HMM3gramSetState,
	 (LMCopyStateFct*) HMM3gramCopyState,
	 (LMFinalStateFct*) HMM3gramFinalState,
	 (LMGetNextStateFct*) HMM3gramGetNextState,
	 (LMStateEqFct*)  HMM3gramStateEqFct,
	 (LMStateHashFct*)HMM3gramStateHashFct,
	 (LMStateMemCreateFct*) HMM3gramStateMemCreateFct,
	 (LMStateMemFreeFct*) HMM3gramStateMemFreeFct,
	 (LMStateMemPrintFct*) HMM3gramStateMemPrintFct,
	 (LMpathCreateFct*) HMM3gramPathCreateFct
       };

Method HMM3gramStateMethod[] = 
{ 
  { "puts", HMM3gramStatePutsItf,   NULL },
  {  NULL,  NULL, NULL } 
};

TypeInfo HMM3gramStateInfo = { 
         "HMM3gramState", 0, -1,  HMM3gramStateMethod, 
         NULL, NULL,
         HMM3gramStateConfigureItf, NULL,
         itfTypeCntlDefaultNoLink, 
        "HMM3gram State\n" };

static int initialized = 0;

int hmm3gram_Init()
{
  if (! initialized) {
    hmm3gramDefault.statesN=0;
    hmm3gramDefault.lp=0;
    hmm3gramDefault.lz=1;
    hmm3gramDefault.timeStamping=0;
    hmm3gramDefault.hmmStates=3; /* 4-gram */
    hmm3gramDefault.cutoffN=-1; /* no cutoff */
                                /* =2 is trigram */
    itfNewType( &HMM3gramStateInfo);
    itfNewType( &HMM3gramInfo);
    lmNewType(  &HMM3gramLM);
    initialized = 1;
  }
  return TCL_OK;
}






