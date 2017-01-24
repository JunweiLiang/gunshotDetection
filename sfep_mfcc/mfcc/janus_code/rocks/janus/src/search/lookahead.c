/* ========================================================================
    JANUS      Speech- to Speech Translation
               ------------------------------------------------------------
               Object: Lookahead
               ------------------------------------------------------------

    Author  :  Michael Finke
    Module  :  lookahead.c
    Date    :  $Id: lookahead.c 2390 2003-08-14 11:20:32Z fuegen $
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
    Revision 1.5  2003/08/14 11:20:23  fuegen
    Merged changes on branch jtk-01-01-15-fms (jaguar -> ibis-013)

    Revision 1.4.24.3  2001/07/06 13:11:59  soltau
    Changed compress and scoreFct handling

    Revision 1.4.24.2  2001/07/03 09:39:56  soltau
    Changes according to the new acoustic score function interface

    Revision 1.4.24.1  2001/06/26 19:29:33  soltau
    beautified/fixed a couple of ugly hacker routines

    Revision 1.4  2000/10/27 16:11:26  janus
    Merged branch jtk-00-10-24-jmd.

    Revision 1.3.28.1  2000/10/27 15:05:04  janus
    Made SIMD a run-time option.

    Revision 1.3  2000/08/24 13:32:26  jmcd
    Merging changes from branch jtk-00-08-23-jmcd.

    Revision 1.1.1.3.2.1  2000/08/24 09:32:45  jmcd
    More revision fixes.

    Revision 1.2  2000/03/04 16:37:52  soltau
    same behavior as in orginal treefwd code

    Revision 1.1  2000/03/02 09:46:11  soltau
    Initial revision

  
   ======================================================================== */

#define   SEARCH_MODULE

#include <assert.h>

#include "search.h"
#include "lookahead.h"
#include "scoreA.h"

static char* RCS = 
          "@(#)1$Id: lookahead.c 2390 2003-08-14 11:20:32Z fuegen $";



/* =========================================================================
    move this to lookahead.c at one time
   ========================================================================= */

static LookAhead lookAheadDefaults;

int lookAheadInit (LookAhead *laP, Vocab * vocabP)
{
  laP->matchfN      =  lookAheadDefaults.matchfN;              /* size of fastmatch window. Size 0
                                             means no lookahead will be done */
  laP->amodelSetP       = lookAheadDefaults.amodelSetP;
  laP->phoneN           = 0;
  laP->distN            = 0;
  laP->factor           = lookAheadDefaults.factor;

  laP->monoPhoneModelXA = NULL;
  laP->activeScoreA  = NULL;       /* active phone distributions      */
  laP->activeDistXA = NULL;
  laP->activeDistA = NULL;
  laP->distScoreM       = NULL;
  laP->phoneScoreA      = NULL;       /* phone penalty score             */
  laP->phoneScoreM      = NULL;
  laP->snsP             = NULL;

  if (laP->amodelSetP) {
    if (amodelSetPhones(vocabP->amodelP) != amodelSetPhones(laP->amodelSetP)) {
      ERROR("search and lookaheads must be defined over same phoneSet\n");
    }
  }

  return TCL_OK;
}


static int* lookAheadCreateMonoPhoneModelXA( LookAhead * laP)
{ 
  int*  monoPhoneModelXA;
  int   monoX;
  int   dummytag = 0;
  Word  word;

  monoPhoneModelXA = malloc( laP->phoneN * sizeof (int));

  wordInit( &word, amodelSetPhones(laP->amodelSetP), 
                   amodelSetTags(  laP->amodelSetP));

  word.phoneA  = &monoX;
  word.tagA    = &dummytag;
  word.itemN   = 1;

  for (monoX = 0; monoX < laP->phoneN; monoX ++) {

    int erg = amodelSetGet(laP->amodelSetP,&word,0,0);
    if (erg < 0) ERROR("amodelSetGet returned %d for monophone %d modelX\n",
                                                               erg,monoX);
    monoPhoneModelXA[monoX] = erg;
  }
  return monoPhoneModelXA;
}

int lookAheadCreate()
{

       if (!LA.amodelSetP) return TCL_OK; /* no amodel, no lookaheads */
  else if (LA.matchfN<=0) return TCL_OK; /* 0 frames means don't */
  else {
    Phones* phonesP = amodelSetPhones(LA.amodelSetP);
    int i;

    LA.phoneN           = phonesNumber(phonesP);
    LA.monoPhoneModelXA = lookAheadCreateMonoPhoneModelXA(&(LA));
    LA.amodelA          = LA.amodelSetP->list.itemA;
    LA.snsP             = amodelSetSenoneSet(LA.amodelSetP);
    LA.distN            = snsNumber(LA.snsP);

    LA.distScoreM = (float**)malloc( LA.matchfN * sizeof(float*));
    LA.distScoreP = (float*) malloc((LA.distN+1)*LA.matchfN * sizeof(float))+1;

    LA.distScoreM[0] = LA.distScoreA = LA.distScoreP;

    for ( i = 1; i < LA.matchfN; i++) 
      LA.distScoreM[i] = LA.distScoreM[i-1] + (LA.distN + 1);

    LA.activeDistA = (char*)  malloc((LA.distN+1) * sizeof(char)) + 1;
    LA.activeDistXA = (int*)  malloc((LA.distN+1) * sizeof(int)) + 1;
    LA.activeScoreA = (float *) malloc((LA.distN+1) * sizeof(float)) + 1;
    memset((char*)LA.activeDistA, 0, LA.distN * sizeof(char));

  /* Initialize the phone distribution array for all defined 
     monophone models */
    LA.activeDistXN = 0;

    for ( i = 0; i < LA.phoneN; i++) {
      int*  distXA = LA.amodelA[LA.monoPhoneModelXA[i]].senone;
      int   stateN = LA.amodelA[LA.monoPhoneModelXA[i]].stateN;

      if (! LA.activeDistA[distXA[0]] && (distXA[0] >= 0)) {
         LA.activeDistA[distXA[0]] = 1;
         LA.activeDistXA[LA.activeDistXN++] = distXA[0];
      } 
      if ((LA.matchfN >= 4) && (stateN > 1)) {
	if (! LA.activeDistA[distXA[1]] && (distXA[1] >= 0)) {
          LA.activeDistA[distXA[1]] = 1;
          LA.activeDistXA[LA.activeDistXN++] = distXA[1];
        }
      }
      if ((LA.matchfN >= 6) && (stateN > 2)) {
	if (! LA.activeDistA[distXA[2]] && (distXA[2] >= 0)) {
          LA.activeDistA[distXA[2]] = 1;
          LA.activeDistXA[LA.activeDistXN++] = distXA[2];
        }
      }
    }
  }
  return TCL_OK;
}

/* ------------------------------------------------------------------------
    dpsLaReset      switches score vectors to the next frame
   ------------------------------------------------------------------------ */

void dpsLaReset (short frameN)
{
  int i;
  if (LA.phoneScoreM) free((char*)LA.phoneScoreM);

  if (LA.matchfN <= 0) {
    /* even if LA.matchfN is not valid, LA.phoneScoreA WILL be accessed */

    LA.phoneScoreM    = (float**)malloc( 1 * sizeof(float*) +
                                         1 * SD.phoneN * sizeof(float));
    LA.phoneScoreM[0] = (float*) (LA.phoneScoreM+ 1);
    LA.phoneScoreA =  LA.phoneScoreM[0];
    for ( i = 0; i < SD.phoneN; i++) LA.phoneScoreA[i] = 0.0;
    return;
  }
  
  if ( frameN < LA.matchfN) {
    ERROR( "FASTMATCH UNDERFLOW\n");
    LA.matchfN = frameN; /* yuk. no. fix. DEBUG */
  }

  LA.phoneScoreM    = (float**)malloc( frameN  * sizeof(float*) +
                                       frameN  * LA.phoneN * sizeof(float));
  LA.phoneScoreM[0] = (float*)(LA.phoneScoreM + frameN);
  LA.phoneScoreA    =  LA.phoneScoreM[0]; 

  LA.frameN = frameN;

  for ( i = 1; i < frameN; i++)
    LA.phoneScoreM[i] = LA.phoneScoreM[i-1] + LA.phoneN;

  for ( i = 0; i < frameN * LA.phoneN; i++) LA.phoneScoreA[i] = 0.0;
}

/* ------------------------------------------------------------------------
   dpsLaNextFrame : compute all necessary senones
   ------------------------------------------------------------------------ */

void dpsLaNextFrame(short frameX)
{
  if ( LA.matchfN > 0) {
    short  fX = 0;
    int d;

    if ( frameX + LA.matchfN >= LA.frameN) {
      assert ( frameX < LA.frameN);
      LA.distScoreA = LA.distScoreM[LA.matchfN - (LA.frameN - frameX)];
      return;
    }

    if ( frameX == 0) {     /* For the first fast match frame we have to
                                 initialize LA.matchfN frames */
      for ( fX = 0; fX < LA.matchfN; fX++) {
        LA.distScoreA = LA.distScoreM[fX];
        LA.snsP->scoreAFctP( LA.snsP, LA.activeDistXA, LA.activeScoreA, 
			    LA.activeDistXN,fX);
      }
    } else {
      float* tmp = LA.distScoreM[0];  /* a spare to help rotate vectors */

      for ( fX = 0; fX < LA.matchfN-1; fX++) 
            LA.distScoreM[fX] = LA.distScoreM[fX+1];

      LA.distScoreA = LA.distScoreM[LA.matchfN-1] = tmp;
      LA.snsP->scoreAFctP( LA.snsP, LA.activeDistXA, LA.activeScoreA, 
			  LA.activeDistXN,frameX+LA.matchfN-1);
    }

    for (d = 0; d< LA.activeDistXN; d++) {
       LA.distScoreA[LA.activeDistXA[d]] = LA.activeScoreA[d];
    }

    LA.distScoreA = LA.distScoreM[0];
  }

  return;
}

/* ------------------------------------------------------------------------
    EvaluatePhoneScores : compute LA.phoneScoreA[phoneX]
   ------------------------------------------------------------------------ */

float dpsEvaluatePhoneScores( short frameX)
{

  float   best       = WORSTSCORE;
  int     phX;
  int     phN = LA.phoneN;

  if (LA.matchfN < 3) return 0.0; /* hm, I'd like to fix this later */

  for (phX = 0; phX < phN; phX ++) {
    int*  distXA = LA.amodelA[LA.monoPhoneModelXA[phX]].senone;
    if (LA.matchfN <= 4) {
     if (distXA[0] <0) LA.phoneScoreA[phX] = WORSTSCORE;
     else LA.phoneScoreA[phX] = 
        LA.factor *(LA.distScoreM[1][distXA[0]] + LA.distScoreM[2][distXA[0]]);
    } else {
      int   stateN = LA.amodelA[LA.monoPhoneModelXA[phX]].stateN;
      if ((distXA[0] <0)||(distXA[1] <0) || (stateN <2)) {
	LA.phoneScoreA[phX] = WORSTSCORE;
      } else {
        float slow =
           LA.distScoreM[1][distXA[0]] + LA.distScoreM[2][distXA[0]] +
           LA.distScoreM[3][distXA[1]] + LA.distScoreM[4][distXA[1]];
	/* LA.distScoreM[4][distXA[2]] + LA.distScoreM[5][distXA[2]]; */
        float fast =
           LA.distScoreM[1][distXA[0]] + LA.distScoreM[1][distXA[0]] +
           LA.distScoreM[2][distXA[1]] + LA.distScoreM[2][distXA[1]];
        /*   LA.distScoreM[3][distXA[2]] + LA.distScoreM[3][distXA[2]] */ 

         if (fast < slow) LA.phoneScoreA[phX] = LA.factor *fast;
                     else LA.phoneScoreA[phX] = LA.factor *slow;
 
      }
    }
    if (LA.phoneScoreA[phX] < best) best = LA.phoneScoreA[phX];
  }

  for (phX = 0; phX < phN; phX ++) {

    if (LA.phoneScoreA[phX] == WORSTSCORE) LA.phoneScoreA[phX] = 0.0;
    else LA.phoneScoreA[phX] -= best;
  }


  return best;
}

/* ------------------------------------------------------------------------
   dpsLaScore
   ------------------------------------------------------------------------ */

float  dpsLaScoreR(short frameX, RTrellis* trP, int mode) 
{ 
  return LA.phoneScoreA[trP->phoneX];
}

float  dpsLaScore(short frameX, Trellis* trP, int mode) 
{  
  return LA.phoneScoreA[trP->phoneX];
}

float  dpsLaScorePhone(short frameX, int phoneX) {
  return LA.phoneScoreA[phoneX];
}


/* ------------------------------------------------------------------------
    lookAheadConfigureItf
   ------------------------------------------------------------------------ */

static int lookAheadConfigureItf( ClientData cd, char *var, char *val)
{
  LookAhead* laP = (LookAhead*)cd;
  if (! laP) laP = &lookAheadDefaults;

  if (! var) {
    ITFCFG(lookAheadConfigureItf,cd,"matchfN");
    ITFCFG(lookAheadConfigureItf,cd,"amodel");
    ITFCFG(lookAheadConfigureItf,cd,"factor");
    return TCL_OK;
  }

  ITFCFG_Int(      var,val,   "matchfN", laP->matchfN, 0);
  ITFCFG_Object(   var,val,   "amodel", laP->amodelSetP,name,AModelSet, 0);
  ITFCFG_Float(    var,val,   "factor", laP->factor, 0);

  return TCL_ERROR;
}

/* ------------------------------------------------------------------------
   lookAheadAccessItf
   ------------------------------------------------------------------------ */

static ClientData lookAheadAccessItf (ClientData cd, char *name, TypeInfo **ti)
{ 
  LookAhead *laP = (LookAhead*)cd;
  if (*name == '.') { 
    if (name[1]=='\0') {
      itfAppendElement( "amodel");
      *ti = NULL; return NULL;
    }
    else { 
      if (! strcmp( name+1, "amodel")) {
        *ti = itfGetType("AModelSet");
        return (ClientData)&(laP->amodelSetP);
      }
    }
  }
  return NULL;
}

/* ------------------------------------------------------------------------
    lookAheadPutsItf
   ------------------------------------------------------------------------ */

static int lookAheadPutsItf( ClientData cd, int argc, char *argv[])
{
  LookAhead* laP   = (LookAhead*)cd;
  int       ac     =  argc - 1;
  if ( itfParseArgv( argv[0], &ac, argv+1, 0,
       NULL) != TCL_OK) return TCL_OK;

 WARN("lookahead in RCS version: %s (%X)\n",RCS,laP);

  return TCL_OK;
}

static Method lookAheadMethod[] = 
{ 
  { "puts", lookAheadPutsItf, NULL },
  {  NULL,  NULL,           NULL } 
};

static TypeInfo lookAheadInfo = { "LookAhead", 0, -1, lookAheadMethod, 
                                 NULL, NULL,
                                lookAheadConfigureItf, lookAheadAccessItf, itfTypeCntlDefaultNoLink,
	                        "LookAhead (fast match) part for tree and flat search" } ;

static int lookAheadInitialized = 0;

int LookAhead_Init( )
{
  if (! lookAheadInitialized) {

    lookAheadDefaults.matchfN = 0;
    lookAheadDefaults.factor = 0.2;
    lookAheadDefaults.amodelSetP = NULL;

    itfNewType( &lookAheadInfo);
    lookAheadInitialized = 1;
  }
  return TCL_OK;
}
