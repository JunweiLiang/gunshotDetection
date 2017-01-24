/* ========================================================================
    JANUS      Speech- to Speech Translation
               ------------------------------------------------------------
               Object: WordGraphs, PhoneGraphs, StateGraphs
               ------------------------------------------------------------

    Author  :  Michael Finke & Ivica Rogina
    Module  :  hmm.c
    Date    :  $Id: hmm.c 3411 2011-03-22 19:18:50Z metze $
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
    Revision 4.9  2006/11/10 10:15:39  fuegen
    merged changes from branch jtk-05-02-02-shajith
    - all modification related to MMIE training
    - first unverified functions for MPE training
    - modifications made by Shajith, Roger, Wilson, and Sebastian

    Revision 4.8  2005/11/08 13:12:36  metze
    HMM fixes from Shajith

    Revision 4.7.12.1  2006/11/03 12:19:51  stueker
    Initial version of mmie modifications from Shajith, Wilson, and Roger. Contains the function pgraphExpandTmp

    Revision 4.7  2003/08/14 11:19:56  fuegen
    Merged changes on branch jtk-01-01-15-fms (jaguar -> ibis-013)

    Revision 4.6.20.12  2003/06/12 15:57:37  metze
    Warnings for padPhones

    Revision 4.6.20.11  2003/02/05 09:24:18  soltau
    Exported pgraphExpand

    Revision 4.6.20.10  2002/11/19 12:40:05  fuegen
    removed warning belonging to padPhoneX

    Revision 4.6.20.9  2002/11/19 11:24:59  soltau
    *** empty log message ***

    Revision 4.6.20.8  2002/11/19 08:37:59  soltau
    Changed construction of phone graphs, deactivated padPhone context

    Revision 4.6.20.7  2002/11/03 14:52:21  soltau
    pgraphBuild        : reorganized phone transitions
    wgraphBuildContext : supplied padPhone when no left context available in all cases

    Revision 4.6.20.6  2002/10/17 11:27:52  soltau
    pgaphBuild: fixed single phone words in pronunciation variants

    Revision 4.6.20.5  2002/10/09 09:05:49  soltau
    pgraphExpandRec : fixed exit transitions for 2-phone words

    Revision 4.6.20.4  2002/10/08 08:03:44  soltau
    pgraphExpandRec : rewrote model setup to support context for single phone words

    Revision 4.6.20.3  2002/08/27 08:46:04  metze
    itfParseArgv now uses <name> and NULL initialization for char*

    Revision 4.6.20.2  2002/08/01 13:42:30  metze
    Fixes for clean documentation.

    Revision 4.6.20.1  2002/02/04 20:59:59  soltau
    Support for full context dependent single phone words

    Revision 4.5.30.2  2000/11/08 17:14:44  janus
    Finished making changes required for compilation under 'gcc -Wall'.

    Revision 4.5.30.1  2000/11/06 10:50:29  janus
    Made changes to enable compilation under -Wall.

    Revision 4.5  2000/08/16 08:42:29  soltau
    removed a memory overflow bug in wgraphMake

    Revision 4.4  2000/01/12 10:02:02  fuegen
    make clustering faster
    integrate tags into hmm structure

    Revision 4.3  1999/06/18 09:12:32  fuegen
    first version of hmmSetModTags

    Revision 4.2  1999/06/10 12:48:51  fuegen
    added hmmSetModalityTags

    Revision 4.1  1999/05/18 12:16:18  fuegen
    *** empty log message ***

    Revision 4.0  1998/11/30 18:07:14  fuegen
    *** empty log message ***

    Revision 3.3  1997/07/31 17:08:08  rogina
    made code gcc -DFKIclean

    Revision 3.2  1997/06/17 17:41:43  westphal
    *** empty log message ***

    Revision 1.28  96/11/27  13:17:40  monika
    patch for training without cross word triphones added by Michael Finke
    
    Revision 1.27  96/02/17  19:25:21  finkem
    amodelSetGet now no longer needs a root index...
    
    Revision 1.26  1996/01/31  05:05:39  finkem
    new senoneSet setup and ptree moved to tree (finally?)

    Revision 1.25  1996/01/12  14:47:15  rogina
    added access function for stateGraph objects
    added access to dict as subobjecgt of  hmm

    Revision 1.24  1995/12/17  04:34:11  finkem
    minor changes

    Revision 1.23  1995/12/04  04:43:09  finkem
    minor corrections

    Revision 1.22  1995/11/30  15:06:11  finkem
    handling pronunciation variants corrected.

    Revision 1.21  1995/11/23  08:00:00  finkem
    finished the ptree eavesdropper in hmm.c

    Revision 1.20  1995/11/17  22:25:42  finkem
    *** empty log message ***

    Revision 1.19  1995/11/14  06:05:14  finkem
    removed bug

    Revision 1.18  1995/10/29  23:55:23  finkem
    moved TmGetItf to trans.c

    Revision 1.17  1995/10/16  21:39:49  finkem
    redefined exit points out of state/phone graphs

    Revision 1.16  1995/10/16  15:53:37  finkem
    create context dependend phone/state graphs

    Revision 1.15  1995/10/05  21:11:04  rogina
    *** empty log message ***

    Revision 1.14  1995/10/05  16:58:34  rogina
    *** empty log message ***

    Revision 1.13  1995/10/05  16:44:29  rogina
    *** empty log message ***

    Revision 1.12  1995/10/04  23:42:02  torsten
    *** empty log message ***

    Revision 1.11  1995/09/27  17:49:52  rogina
    *** empty log message ***

    Revision 1.10  1995/09/12  18:01:34  rogina
    *** empty log message ***

    Revision 1.9  1995/09/06  07:29:25  kemp
    *** empty log message ***

    Revision 1.8  1995/08/17  17:54:58  rogina
    *** empty log message ***

    Revision 1.7  1995/08/16  16:59:15  rogina
    *** empty log message ***

    Revision 1.6  1995/08/11  17:22:12  rogina
    *** empty log message ***

    Revision 1.5  1995/08/10  08:11:57  rogina
    *** empty log message ***

    Revision 1.4  1995/08/04  14:17:10  rogina
    *** empty log message ***

    Revision 1.3  1995/07/27  18:43:40  rogina
    *** empty log message ***

    Revision 1.2  1995/07/26  09:10:56  rogina
    *** empty log message ***

    Revision 1.1  1995/07/25  13:49:57  finkem
    Initial revision

 
   ======================================================================== */

#include "common.h"
#include "hmm.h"
#include "error.h"
#include "array.h"
#include "word.h"
#include "dictionary.h"
#include "modalities.h"

#define USE_PAD 0

char hmmRCSVersion[]= "@(#)1$Id: hmm.c 3411 2011-03-22 19:18:50Z metze $ USE_PAD= 0";

/* ========================================================================
    StateGraph
   ======================================================================== */

static int sgraphPutsItf (ClientData cd, int argc, char *argv[]);

/* ------------------------------------------------------------------------
    Create StateGraph Structure:
   ------------------------------------------------------------------------ */

int sgraphInit( StateGraph* SG, SenoneSet* snsP, AModel* amP)
{
  SG->stateN  = 0;
  SG->senoneX = NULL;
  SG->transX  = NULL;
  SG->phoneX  = NULL;
  SG->wordX   = NULL;

  SG->snsP    = snsP; link( snsP, "SenoneSet");

  SG->initN   = 0;
  SG->init    = NULL;

  SG->fwdTrN  = NULL;
  SG->fwdIdx  = NULL;
  SG->fwdPen  = NULL;

  if ( amP) {
    int stateX;

    SG->stateN  = amP->stateN;
    SG->initN   = 1;
  
    SG->senoneX = (int*)malloc( SG->stateN * sizeof(int));
    SG->transX  = (int*)malloc( SG->stateN * sizeof(int));
    SG->phoneX  = (int*)malloc( SG->stateN * sizeof(int));
    SG->wordX   = (int*)malloc( SG->stateN * sizeof(int));

    SG->init    = (int*)malloc( SG->initN*sizeof(int));

    SG->fwdTrN  =    (int*)malloc( SG->stateN * sizeof(int));
    SG->fwdIdx  =   (int**)malloc( SG->stateN * sizeof(int*));
    SG->fwdPen  = (float**)malloc( SG->stateN * sizeof(float*));

    SG->init[0] = 0;

    for ( stateX = 0; stateX < SG->stateN; stateX++) {
      int i;

      SG->senoneX[stateX] = amP->senone[stateX];
      SG->transX [stateX] = amP->trans[stateX];

      SG->fwdTrN [stateX] = amP->fwdTrN[stateX];

      SG->fwdIdx [stateX] = (int  *)malloc( amP->fwdTrN[stateX] * sizeof(int));
      SG->fwdPen [stateX] = (float*)malloc( amP->fwdTrN[stateX] * sizeof(float));

      for ( i = 0; i < amP->fwdTrN[stateX]; i++) {
        SG->fwdIdx [stateX][i] = amP->fwdIdx[stateX][i];
        SG->fwdPen [stateX][i] = amP->fwdPen[stateX][i];
      }
      SG->phoneX [stateX] = -1;
      SG->wordX  [stateX] = -1; 
    }
  }
  return TCL_OK;
}

StateGraph* sgraphCreate( SenoneSet* snsP, AModel* amP)
{
  StateGraph* sgraphP = (StateGraph*)malloc(sizeof(StateGraph));

  if (! sgraphP || sgraphInit( sgraphP, snsP, amP) != TCL_OK) {
    if ( sgraphP) free( sgraphP);
    ERROR("Failed to allocate StateGraph.\n"); 
    return NULL;
  }
  return sgraphP;
}

static ClientData sgraphCreateItf (ClientData clientData, 
                                   int argc, char *argv[])
{ 
  SenoneSet* snsP    = NULL;
  char*      name    = NULL;

  if (itfParseArgv (argv[0], &argc, argv, 0,
      "<name>",      ARGV_STRING, NULL, &name, NULL,        "name of the state graph",
      "<senoneSet>", ARGV_OBJECT, NULL, &snsP, "SenoneSet", "senone set",
      NULL) != TCL_OK) return NULL;

  return (ClientData) sgraphCreate (snsP, NULL);
}

/* ------------------------------------------------------------------------
    Free StateGraph Structure
   ------------------------------------------------------------------------ */

int sgraphReset( StateGraph* SG)
{
  int i;

  if (! SG) return TCL_OK;

  if ( SG->senoneX) { free( SG->senoneX); SG->senoneX = NULL; } 
  if ( SG->transX)  { free( SG->transX);  SG->transX  = NULL; }
  if ( SG->phoneX)  { free( SG->phoneX);  SG->phoneX  = NULL; }
  if ( SG->wordX)   { free( SG->wordX);   SG->wordX   = NULL; }
  if ( SG->init)    { free( SG->init);    SG->init    = NULL; }

  if ( SG->fwdIdx)  { 
    for ( i = 0; i < SG->stateN; i++) if (SG->fwdIdx[i]) free(SG->fwdIdx[i]); 
    free( SG->fwdIdx);  SG->fwdIdx = NULL;
  }
  if ( SG->fwdPen)  { 
    for ( i = 0; i < SG->stateN; i++) if (SG->fwdPen[i]) free(SG->fwdPen[i]); 
    free( SG->fwdPen);  SG->fwdPen  = NULL;
  }
  if ( SG->fwdTrN)  { free( SG->fwdTrN);  SG->fwdTrN  = NULL; }

  SG->stateN  = 0;
  SG->initN   = 0;

  return TCL_OK;
}

int sgraphDeinit( StateGraph* SG)
{
  if (!SG) return TCL_OK;
  if ( SG->snsP) { unlink( SG->snsP, "SenoneSet"); SG->snsP = NULL; }
  return sgraphReset( SG);
}

int sgraphFree( StateGraph* SG)
{
  if (!SG) return TCL_OK;
  if ( sgraphDeinit( SG) != TCL_OK) return TCL_ERROR;
  if ( SG) free( SG);
  return TCL_OK;
}

static int sgraphFreeItf (ClientData clientData)
{ 
  return sgraphFree ((StateGraph*)clientData);
}

/* ------------------------------------------------------------------------
    sgraphPutsItf
   ------------------------------------------------------------------------ */

static int sgraphPutsItf (ClientData cd, int argc, char *argv[])
{
  StateGraph* sgraphP = (StateGraph*)cd;
  int        i, j;

  itfAppendResult( "{");
  for ( i = 0; i < sgraphP->stateN; i++) {
    itfAppendElement( "%s", snsName(sgraphP->snsP,
                                    sgraphP->senoneX[i]));
  }
  itfAppendResult( "} {");
  for ( i = 0; i < sgraphP->stateN; i++) {
    itfAppendResult( " {");
    for ( j = 0; j < sgraphP->fwdTrN[i]; j++) {
      itfAppendElement( "%d %f", sgraphP->fwdIdx[i][j], 
                                 sgraphP->fwdPen[i][j]);
    }
    itfAppendResult( "}");
  }
  itfAppendResult( "} {");
  if ( sgraphP->transX) {
    for ( i = 0; i < sgraphP->stateN; i++)
      itfAppendElement( "%d", sgraphP->transX[i]);
  }
  itfAppendResult( "} {"); 
  for ( i = 0; i < sgraphP->initN; i++) {
    itfAppendElement( "%d", sgraphP->init[i]);
  }
  itfAppendResult( "} {");
  if ( sgraphP->phoneX) {
    for ( i = 0; i < sgraphP->stateN; i++)
      itfAppendElement( "%d", sgraphP->phoneX[i]);
  }
  itfAppendResult( "} {");
  if ( sgraphP->wordX) {
    for ( i = 0; i < sgraphP->stateN; i++)
      itfAppendElement( "%d", sgraphP->wordX[i]);
  }
  itfAppendResult( "}");
  return TCL_OK;
}

/* ------------------------------------------------------------------------
    sgraphBuild     converts a phoneGraph to a stateGraph
   ------------------------------------------------------------------------ */

int sgraphBuild( StateGraph* sgraphP, PhoneGraph* pgraphP, int logPen)
{
  StateGraph** SG    = pgraphP->stateGraph;
  int*         offXA = NULL;
  int          stateN, i, j, k, l, m;
  int          idx;

  assert(      sgraphP && pgraphP);
  sgraphReset( sgraphP);

  if (! pgraphP->amodel) {
    ERROR( "PhoneGraph has no AModels allocated.\n");
    return TCL_ERROR;
  }
  if ( amodelSetSenoneSet( pgraphP->amsP) != sgraphP->snsP) {
    ERROR( "SenoneSet does not fit to AModels in phoneGraph.\n");
    return TCL_ERROR;
  }

  offXA = (int*)malloc( pgraphP->phoneN * sizeof(int));

  for ( stateN = 0, i = 0; i < pgraphP->phoneN; i++) {
    offXA[i]  = stateN;
    stateN   += pgraphP->stateGraph[i]->stateN;
  }

  /* allocate the new state graph */

  sgraphP->stateN  =  stateN;
  sgraphP->senoneX = (int*   )malloc( stateN * sizeof(int));
  sgraphP->transX  = (int*   )malloc( stateN * sizeof(int));
  sgraphP->phoneX  = (int*   )malloc( stateN * sizeof(int));
  sgraphP->wordX   = (int*   )malloc( stateN * sizeof(int));

  sgraphP->fwdTrN  = (int*   )malloc( stateN * sizeof(int));
  sgraphP->fwdIdx  = (int**  )malloc( stateN * sizeof(int*));
  sgraphP->fwdPen  = (float**)malloc( stateN * sizeof(float*));

  sgraphP->initN   =  0;
  sgraphP->init    =  NULL;

  for ( idx = 0, i = 0; i < pgraphP->phoneN; i++) {
    for ( j = 0; j < SG[i]->stateN; j++) {

      sgraphP->senoneX[idx] = pgraphP->stateGraph[i]->senoneX[j];
      sgraphP->transX [idx] = pgraphP->stateGraph[i]->transX [j];
      sgraphP->phoneX [idx] = pgraphP->stateGraph[i]->phoneX [j];
      sgraphP->wordX  [idx] = pgraphP->stateGraph[i]->wordX  [j];
      sgraphP->fwdTrN [idx] = 0;

      /* Pass 1: count the number of transition from a state */

      for ( k = 0; k < SG[i]->fwdTrN[j]; k++) {
        if ( SG[i]->fwdIdx[j][k] + j >= SG[i]->stateN) {
          int initX = SG[i]->fwdIdx[j][k] + j - SG[i]->stateN;

          /* For transitions leaving a phone model jump to all possible
             successor words */

          for ( l = 0; l < pgraphP->fwdTrN[i]; l++) {
            int fwdIdx = pgraphP->fwdIdx[i][l];
            if ( fwdIdx + i >= pgraphP->phoneN) {
              sgraphP->fwdTrN[idx] += 1;
            }
            else if ( initX < SG[fwdIdx+i]->initN) {
              sgraphP->fwdTrN[idx] += 1; /* += SG[fwdIdx+i]->initN (Shajith) ? */
            }
          }
        }
        else {
          sgraphP->fwdTrN[idx] += 1;
        }
      }

      /* allocate the transition model */

      if ( sgraphP->fwdTrN[idx]) {
        sgraphP->fwdIdx[idx] = (int*  )malloc(sgraphP->fwdTrN[idx]*sizeof(int));
        sgraphP->fwdPen[idx] = (float*)malloc(sgraphP->fwdTrN[idx]*sizeof(float));
      }
      else {
        sgraphP->fwdIdx[idx] = NULL;
        sgraphP->fwdPen[idx] = NULL;
      }

      /* Pass 2: Fill the transition structures */

      for ( m = 0, k = 0; k < SG[i]->fwdTrN[j]; k++) {
        if ( SG[i]->fwdIdx[j][k] + j >= SG[i]->stateN) {
          int initX = SG[i]->fwdIdx[j][k] + j - SG[i]->stateN;

          for ( l = 0; l < pgraphP->fwdTrN[i]; l++) {
            int fwdIdx = pgraphP->fwdIdx[i][l];
            if ( fwdIdx + i >= pgraphP->phoneN) {

              /* exit transition out of the state graph. Since we do
                 not know anything about the following state graph we
                 cannot find exact entry points */

              sgraphP->fwdIdx[idx][m] = sgraphP->stateN - idx +
                                        fwdIdx + i - pgraphP->phoneN;
              sgraphP->fwdPen[idx][m] = ( logPen) ? 
                       SG[i]->fwdPen[j][k] + pgraphP->fwdPen[i][l] :
                       SG[i]->fwdPen[j][k] * pgraphP->fwdPen[i][l];
              m++;
            }
            else if ( initX < SG[fwdIdx+i]->initN) {  

              sgraphP->fwdIdx[idx][m] = offXA[fwdIdx+i] + initX - idx;
	      /* sgraphP->fwdIdx[idx][m] computed from SG[fwdIdx+i]->init[n] (Shajith) ? */
              sgraphP->fwdPen[idx][m] = ( logPen) ? 
                        SG[i]->fwdPen[j][k] + pgraphP->fwdPen[i][l] :
                        SG[i]->fwdPen[j][k] * pgraphP->fwdPen[i][l];
              m++;
            }
          }
        }
        else {
          sgraphP->fwdIdx[idx][m] = SG[i]->fwdIdx[j][k];
          sgraphP->fwdPen[idx][m] = SG[i]->fwdPen[j][k];
          m++;
        }
      }
      idx++;
    }
  }

  /* define entry points to the phone graph */

  for ( i = 0; i < pgraphP->initN; i++) {
    int initX = pgraphP->init[i];

    sgraphP->init = (int*)realloc( sgraphP->init, sizeof(int) *
                                 ( sgraphP->initN + SG[initX]->initN));

    for ( j = 0; j < SG[initX]->initN; j++) {
      sgraphP->init[sgraphP->initN] = offXA[initX] + SG[initX]->init[j];
      sgraphP->initN++;
    }
  }

  if ( offXA) free( offXA);

  return TCL_OK;
}

static int sgraphBuildItf (ClientData cd, int argc, char *argv[])
{
  StateGraph* sgraphP = (StateGraph*)cd;
  PhoneGraph* pgraphP =  NULL;
  int         logPen  =  1;
  int         ac      =  argc - 1;

  if ( itfParseArgv( argv[0], &ac, argv+1, 0,
      "<phoneGraph>",ARGV_OBJECT, NULL, &pgraphP, "PhoneGraph", "phone graph", 
      "-logPen",     ARGV_INT,    NULL, &logPen,   NULL, "log penalties",
       NULL) != TCL_OK) return TCL_ERROR;

  return sgraphBuild( sgraphP, pgraphP, logPen);
}

/* ------------------------------------------------------------------------
    sgraphLogPen2Prob  assuming the transitions in the stateGraph are log
                       probabilities an array of the penalities in terms
                       of probabilities is allocated. This procedure is
                       called by forwardBackward() in order to translate
                       the transitions only onces.
   ------------------------------------------------------------------------ */

float** sgraphLogPen2Prob( StateGraph* SG)
{
  float** fwdPen = (float**)malloc( SG->stateN * sizeof(float*));

  if ( SG->fwdPen)  { 
    int   i;
    for ( i = 0; i < SG->stateN; i++) 
      if (SG->fwdPen[i]) {
        int j;
        fwdPen[i] = (float*)malloc( SG->fwdTrN[i] * sizeof(float));

        for ( j = 0; j < SG->fwdTrN[i]; j++) 
          fwdPen[i][j] = exp(- SG->fwdPen[i][j]);
      }
  }
  return fwdPen;
}

/* ------------------------------------------------------------------------
    sgraphAccessItf
   ------------------------------------------------------------------------ */

static ClientData sgraphAccessItf (ClientData cd, char *name, TypeInfo **ti)
{ 
  StateGraph* sgraphP  = (StateGraph*)cd;

  if (*name == '.') { 
    if (name[1]=='\0') {
      if ( sgraphP->snsP) itfAppendElement("senoneSet");
      *ti = NULL; return NULL;
    }
    else {
      name++;
      if (sgraphP->snsP && strcmp(name,"senoneSet")==0) {
	*ti = itfGetType("SenoneSet");
        return (ClientData)(sgraphP->snsP);
      }
      return NULL;
    }
  }
  return NULL;
}

/* ========================================================================
    PhoneGraph
   ======================================================================== */

static PhoneGraph phoneGraphDefault;

/* ------------------------------------------------------------------------
    Create PhoneGraph Structure:
   ------------------------------------------------------------------------ */

int pgraphInit( PhoneGraph* PG, AModelSet* amsP, Dictionary* dict, 
                int idx, int variants, int modTags)
{
  assert( PG && amsP);

  PG->phoneN     = 0;
  PG->phone      = NULL;
  PG->tag        = NULL;
  PG->word       = NULL;
  PG->stateGraph = NULL;
  PG->amodel     = NULL;
  PG->amsP       = amsP; link( amsP, "AModelSet");

  PG->initN      = 0;
  PG->init       = NULL;

  PG->fwdTrN     = NULL;
  PG->fwdIdx     = NULL;
  PG->fwdPen     = NULL;
  PG->modTags    = modTags;

  /* create modMask (inverse of modTags) */
  PG->modMask    = modTags ^ -1;

  if ( dict && idx > -1) {

    char* wordS  = strdup(dictName( dict, idx)); 
    size_t l     = strlen(wordS);
    int*  phone  = NULL;
    int*  tag    = NULL;
    int   phoneX, phoneN, varN = 0, trX, pidx, vidx;
    int   i, k;

    /* lookup to compute phoneN */
    for ( i  = listFirstItem((List*)&(dict->list), idx, &k), phoneN = 0; 
          i >= 0 && (varN == 0 || variants); 
          i  = listNextItem((List*)&(dict->list), &k)) {
      char* p = (char*)dictName( dict, i);
      if ( l > strlen( p) || strncmp( wordS, p, l) || 
         ( l < strlen( p) && p[l]  > '(')) break;
      if ((l < strlen( p) && p[l] == '(') || l == strlen( p)) {

        dictGetFast( dict, i, &(PG->phoneN), &(phone), &(tag));

        phoneN += PG->phoneN;
        varN   += 1;
      }
    }

    PG->phoneN  =  phoneN;
    PG->phone   = (int*   )malloc( phoneN * sizeof(int));
    PG->tag     = (int*   )malloc( phoneN * sizeof(int));

    PG->fwdTrN  = (int*   )malloc( phoneN * sizeof(int));
    PG->fwdIdx  = (int**  )malloc( phoneN * sizeof(int*));
    PG->fwdPen  = (float**)malloc( phoneN * sizeof(float*));

    PG->initN   =  varN;
    PG->init    = (int*)malloc(PG->initN*sizeof(int));

    for ( i  = listFirstItem((List*)&(dict->list), idx, &k), pidx=0, vidx=0; 
          i >= 0 && vidx < varN;
          i  = listNextItem((List*)&(dict->list), &k)) {

      char* p = (char*)dictName( dict, i);
      int   phnN;

      if ( l > strlen( p) || strncmp( wordS, p, l) || 
         ( l < strlen( p) && p[l]  > '(')) break;
      if ((l < strlen( p) && p[l] == '(') || l == strlen( p)) {
        PG->init[vidx] = pidx;

        dictGetFast( dict, i, &(phnN), &(phone), &(tag));

        for ( phoneX = 0; phoneX < phnN; phoneX++) {
          PG->phone [pidx+phoneX] =  phone[phoneX];
	  /* modTags default value is 0, so set always */
          PG->tag   [pidx+phoneX] =  tag[phoneX] | modTags;
          PG->fwdTrN[pidx+phoneX] =  1;
          PG->fwdIdx[pidx+phoneX] = (int*  )malloc(sizeof(int  )*PG->fwdTrN[pidx+phoneX]);
          PG->fwdPen[pidx+phoneX] = (float*)malloc(sizeof(float)*PG->fwdTrN[pidx+phoneX]);

          for ( trX = 0; trX < PG->fwdTrN[pidx+phoneX]; trX++) {
	    PG->fwdIdx[pidx+phoneX][trX] = (phoneX+1 == phnN) ?
                                            phoneN - pidx - phoneX : 1;
	    PG->fwdPen[pidx+phoneX][trX] =  0.0;
          }
        }
        pidx += phnN;
        vidx += 1;
      }
    }
    if ( wordS) free( wordS);
  }
  return TCL_OK;
}

PhoneGraph* pgraphCreate( AModelSet* amsP, Dictionary* dict, int idx, 
			  int variants, int modTags)
{
  PhoneGraph* pgraphP = (PhoneGraph*)malloc(sizeof(PhoneGraph));

  if (! pgraphP ||
      pgraphInit( pgraphP, amsP, dict, idx, variants, modTags) != TCL_OK) {
    if ( pgraphP) free( pgraphP);
    ERROR("Failed to allocate PhoneGraph.\n"); 
    return NULL;
  }
  return pgraphP;
}

static ClientData pgraphCreateItf( ClientData clientData, 
                                   int argc, char *argv[])
{
  AModelSet*  amsP    = NULL;
  char*       name    = NULL;

  if (itfParseArgv (argv[0], &argc, argv, 0,
      "<name>",      ARGV_STRING, NULL, &name, NULL,        "name of the phone graph",
      "<amodelSet>", ARGV_OBJECT, NULL, &amsP, "AModelSet", "acoustic model set",
       NULL) != TCL_OK) return NULL;

  return (ClientData) pgraphCreate (amsP, NULL, 0, 0, phoneGraphDefault.modTags);
}

/* ------------------------------------------------------------------------
    pgraphConfigureItf
   ------------------------------------------------------------------------ */

static int pgraphConfigureItf (ClientData cd, char *var, char *val)
{
  PhoneGraph*  pgraphP = (PhoneGraph*)cd;

  if (!pgraphP) pgraphP = &(phoneGraphDefault);

  if (!var)  {
    ITFCFG(pgraphConfigureItf,cd,"modTags");
    return TCL_OK;
  }
  ITFCFG_Int(var,val,"modTags", pgraphP->modTags,  0);
  return TCL_OK;
}


/* ------------------------------------------------------------------------
    Free PhoneGraph Structure
   ------------------------------------------------------------------------ */

int pgraphReset(  PhoneGraph* PG)
{
  int i;

  if (!PG) return TCL_OK;

  if ( PG->stateGraph) {
    for ( i = 0; i < PG->phoneN; i++) sgraphFree(PG->stateGraph[i]);
    free( PG->stateGraph);  PG->stateGraph = NULL;
  }

  if ( PG->phone)   { free( PG->phone);   PG->phone   = NULL; }
  if ( PG->tag)     { free( PG->tag);     PG->tag     = NULL; }
  if ( PG->word)    { free( PG->word);    PG->word    = NULL; }
  if ( PG->amodel)  { free( PG->amodel);  PG->amodel  = NULL; }
  if ( PG->init)    { free( PG->init);    PG->init    = NULL; }

  if ( PG->fwdIdx)  { 
    for ( i = 0; i < PG->phoneN; i++) if (PG->fwdIdx[i]) free(PG->fwdIdx[i]); 
    free( PG->fwdIdx);  PG->fwdIdx = NULL;
  }
  if ( PG->fwdPen)  { 
    for ( i = 0; i < PG->phoneN; i++) if (PG->fwdPen[i]) free(PG->fwdPen[i]); 
    free( PG->fwdPen);  PG->fwdPen  = NULL;
  }
  if ( PG->fwdTrN)  { free( PG->fwdTrN);  PG->fwdTrN  = NULL; }

  PG->phoneN = 0;
  PG->initN  = 0;

  return TCL_OK;
}

int pgraphDeinit( PhoneGraph* PG)
{
  if (!PG) return TCL_OK;
  if ( PG->amsP)   { unlink( PG->amsP,  "AModelSet"); PG->amsP   = NULL; }
  return pgraphReset( PG);
}

int pgraphFree( PhoneGraph* PG)
{
  if (!PG) return TCL_OK;
  if ( pgraphDeinit( PG) != TCL_OK) return TCL_ERROR;
  if ( PG) free( PG);
  return TCL_OK;
}

static int pgraphFreeItf (ClientData clientData)
{ 
  return pgraphFree ((PhoneGraph*)clientData);
}

/* ------------------------------------------------------------------------
    pgraphPutsItf
   ------------------------------------------------------------------------ */

static int pgraphPutsItf (ClientData cd, int argc, char *argv[])
{
  PhoneGraph* pgraphP = (PhoneGraph*)cd;
  int        i, j;

  itfAppendResult( "{");
  for ( i = 0; i < pgraphP->phoneN; i++) {
    itfAppendElement( "%s", phonesName(amodelSetPhones(pgraphP->amsP),
                                       pgraphP->phone[i]));
  }
  itfAppendResult( "} {");
  if ( pgraphP->tag) {
    for ( i = 0; i < pgraphP->phoneN; i++)
      itfAppendElement( "%d", pgraphP->tag[i]);
  }
  itfAppendResult( "} {");
  for ( i = 0; i < pgraphP->phoneN; i++) {
    itfAppendResult( " {");
    for ( j = 0; j < pgraphP->fwdTrN[i]; j++) {
      itfAppendElement( "%d %f", pgraphP->fwdIdx[i][j], 
                                 pgraphP->fwdPen[i][j]);
    }
    itfAppendResult( "}");
  }
  itfAppendResult( "} {"); 
  for ( i = 0; i < pgraphP->initN; i++) {
    itfAppendElement( "%d", pgraphP->init[i]);
  }
  itfAppendResult( "} {"); 
  if ( pgraphP->amodel) {
    for ( i = 0; i < pgraphP->phoneN; i++)
      itfAppendElement( "%d", pgraphP->amodel[i]);
  }
  itfAppendResult( "} {");
  if ( pgraphP->word) {
    for ( i = 0; i < pgraphP->phoneN; i++)
      itfAppendElement( "%d", pgraphP->word[i]);
  }
  itfAppendResult( "}");
  return TCL_OK;
}

/* ------------------------------------------------------------------------
    pgraphAccessItf
   ------------------------------------------------------------------------ */

static ClientData pgraphAccessItf (ClientData cd, char *name, TypeInfo **ti)
{ 
  PhoneGraph* pgraphP  = (PhoneGraph*)cd;
  int         i;

  if (*name == '.') { 
    if (name[1]=='\0') {
      if ( pgraphP->amodel)
           itfAppendElement( "amodel(0..%d)",pgraphP->phoneN-1);
      if ( pgraphP->stateGraph)
           itfAppendElement( "stateGraph(0..%d)",pgraphP->phoneN-1);
      *ti = NULL; return NULL;
    }
    else {
      name++;
      if (pgraphP->amodel && sscanf(name,"amodel(%d)",&i)==1) {
	*ti = itfGetType("AModel");

	 if ( i >= 0 && i < pgraphP->phoneN)
              return (ClientData)&(pgraphP->amsP->list.itemA[pgraphP->amodel[i]]);
	 else return  NULL;
      }
      else if (pgraphP->stateGraph && sscanf(name,"stateGraph(%d)",&i)==1) {
	*ti = itfGetType("StateGraph");

	 if ( i >= 0 && i < pgraphP->phoneN)
              return (ClientData)(pgraphP->stateGraph[i]);
	 else return  NULL;
      }
      return NULL;
    }
  }
  return NULL;
}

/* ------------------------------------------------------------------------
    pgraphMake    fill a PhoneGraph object according to the specification
                  of the phone sequence and optionally the transition 
                  structure as well as the initial states
   ------------------------------------------------------------------------ */

int pgraphMake( PhoneGraph* pgraphP, IArray* parray, char* transS,
                                     IArray* iarray)
{
  int phnX;

  assert(      pgraphP && parray);
  pgraphReset( pgraphP);

  pgraphP->phoneN     =  parray->itemN;
  pgraphP->phone      =  parray->itemA;
  pgraphP->tag        = (int        *)malloc( pgraphP->phoneN * sizeof(int));
  pgraphP->stateGraph = (StateGraph**)malloc( pgraphP->phoneN * 
                                              sizeof(StateGraph*));

  for ( phnX = 0; phnX < pgraphP->phoneN; phnX++) {

    pgraphP->tag[phnX]        = 0;
    pgraphP->stateGraph[phnX] = sgraphCreate( 
                                amodelSetSenoneSet(pgraphP->amsP), NULL);

    /* TODO: initial setup of state graph structures */
  }

  /* Define the transition structure either according to the string
     specification or (if none is specified) as a simple list of words */

  if (! transS) {
    int trX;

    pgraphP->fwdTrN = (int*   )malloc(pgraphP->phoneN * sizeof(int));
    pgraphP->fwdIdx = (int**  )malloc(pgraphP->phoneN * sizeof(int*));
    pgraphP->fwdPen = (float**)malloc(pgraphP->phoneN * sizeof(float*));

    for ( phnX = 0; phnX < pgraphP->phoneN; phnX++) {

      pgraphP->fwdTrN[phnX] = 1;
      pgraphP->fwdIdx[phnX] = (int*  )malloc(pgraphP->fwdTrN[phnX] * sizeof(int));
      pgraphP->fwdPen[phnX] = (float*)malloc(pgraphP->fwdTrN[phnX] * sizeof(float));
    
      for (trX=0; trX<pgraphP->fwdTrN[phnX]; trX++) {
        pgraphP->fwdIdx[phnX][trX] = 1;
        pgraphP->fwdPen[phnX][trX] = 0.0;
      }
    }
  }
  else {
    int n = tmArrayGetItf( transS, &(pgraphP->fwdTrN), &(pgraphP->fwdIdx), 
                                   &(pgraphP->fwdPen));
    if ( n != pgraphP->phoneN) {
      ERROR( "Wrong number of transition models (%d).\n", n);
      pgraphReset( pgraphP);
      return TCL_ERROR;
    }
  }

  /* define the set of initial states */

  if ( iarray->itemA) {
    pgraphP->initN   = iarray->itemN;
    pgraphP->init    = iarray->itemA;
  }
  else {
    pgraphP->initN   = 1;
    pgraphP->init    = (int*)malloc(pgraphP->initN*sizeof(int));
    pgraphP->init[0] = 0;
  }

  pgraphMakeSGraph( pgraphP);
  return TCL_OK;
}

static int pgraphMakeItf ( ClientData clientData, int argc, char *argv[] )
{
  PhoneGraph* pgraphP   = (PhoneGraph*)clientData;
  int         ac        =  argc-1;
  IArray      parray    = {NULL, 0};
  IArray      iarray    = {NULL, 0};
  char*       transS    =  NULL;

  if (itfParseArgv(argv[0], &ac, argv+1, 0,
     "<phones>",ARGV_LIARRAY,NULL,&parray,
     &(amodelSetPhones(pgraphP->amsP)->list),   "list of phone nodes", 
     "-trans",  ARGV_STRING, NULL,&transS,NULL, "transition model",
     "-init",   ARGV_IARRAY, NULL,&iarray,NULL, "initial states", NULL) != 
TCL_OK) {
     if ( parray.itemA) free( parray.itemA);
     if ( iarray.itemA) free( iarray.itemA);
     return TCL_ERROR;
  }
  if ( pgraphMake( pgraphP, &parray, transS, &iarray) != TCL_OK) {
     if ( parray.itemA) free( parray.itemA);
     if ( iarray.itemA) free( iarray.itemA);
     return TCL_ERROR;
  }
  return TCL_OK;
}

/* ------------------------------------------------------------------------
    pgraphMakeSGraph    fill a phone graph with stateGraph information
   ------------------------------------------------------------------------ */

int pgraphMakeSGraph( PhoneGraph* pgraphP)
{
  int i;

  if (! pgraphP->amodel) return TCL_ERROR;

  if (! pgraphP->stateGraph) {
    pgraphP->stateGraph = (StateGraph**)malloc( pgraphP->phoneN * 
                                                sizeof(StateGraph*));
    memset( pgraphP->stateGraph, 0, sizeof(StateGraph*) * pgraphP->phoneN);
  }
  else {
    for ( i = 0; i < pgraphP->phoneN; i++) {
      if ( pgraphP->stateGraph[i]) {
        sgraphReset( pgraphP->stateGraph[i]);
        pgraphP->stateGraph[i] = NULL;
      }
    }
  }
  for ( i = 0; i < pgraphP->phoneN; i++) {
    if (! pgraphP->stateGraph[i]) {

      pgraphP->stateGraph[i] = sgraphCreate( pgraphP->amsP->snsP,
                                             pgraphP->amsP->list.itemA + 
                                             pgraphP->amodel[i]);
    }
  }
  return TCL_OK;
}

/* ------------------------------------------------------------------------
    pgraphExpandCount  count the number of states in an expanded (unrolled)
                       phoneGraph starting from phone[idx]. Only forward
                       transitions will be considered (no self-loops or
                       even backward transitions). The entry and exit
                       nodes of the phone graph get multiple counts
                       depending on the number of left/right contexts
                       given by leftN/rightN.
   ------------------------------------------------------------------------ */

static int pgraphExpandCount( PhoneGraph* pgraphP, int idx, 
                                                   int leftN, int rightN,
                                                   int count, int rcmSdp)
{
  int n = 0;
  int i;

  leftN  = (! leftN)  ? 1 : leftN;
  rightN = (! rightN) ? 1 : rightN;

  for ( i = 0; i < pgraphP->fwdTrN[idx]; i++) {
    int  fwdIdx  = pgraphP->fwdIdx[idx][i]; 
    if ( fwdIdx >= pgraphP->phoneN - idx) { 
      if (count ==1) {
	if (rcmSdp >0  && rightN >0) {
	  n += leftN * rightN;
	} else {
	  n += leftN;
	}
      } else {
	n += leftN + (count-2) + rightN; 
      }
    }
    else if ( fwdIdx <= 0);
    else {
      n += pgraphExpandCount( pgraphP, idx + fwdIdx, leftN, rightN, count + 1,rcmSdp);
    }
  }
  return n;
}

/* ------------------------------------------------------------------------
    pgraphExpandRec  recursive procedure to unroll a phoneGraph, create
                     context dependent entry and exit nodes and fill out
                     the acoustic model indices.
                    (for more info refer to pgraphExpand)
   ------------------------------------------------------------------------ */

/* MAKRO to delete modalityTags in context phones (mid phone is -left) */
#define MOD_DEMASK(left, right) \
   do { \
	 if ( maskedWordP->tagA ) { \
	     free (maskedWordP->tagA); maskedWordP->tagA = NULL; \
         } \
	 maskedWordP->itemN         = (right)-(left)+1; \
	 maskedWordP->tagA          = (int*)malloc(maskedWordP->itemN * sizeof(int)); \
	 maskedWordP->phoneA        = word.phoneA; \
	 maskedWordP->tagA[-(left)] = word.tagA[-(left)]; \
	 for (w=0; w<maskedWordP->itemN; w++) \
	     if (w != -(left)) \
                 maskedWordP->tagA[w] = word.tagA[w] & pgraphP->modMask; \
   } while (0)

static int pgraphExpandRec( PhoneGraph*        pgraphP, int pidx, 
                            PhoneGraph*        ngraphP, int nidx, 
                            int*               stateXA, int*relXA,
                            PhoneGraphContext* left,    int leftN,
                            PhoneGraphContext* right,   int rightN,
                            int* phnXA, int* tagXA, int* idxXA, int* trnXA, 
                            int  idx,   int  xwmodels, int rcmSdp, int* sdpXA)
{
  AModelSet* amsP      = ngraphP->amsP;
  int        padPhoneX = (amodelSetTree(amsP))->questions.padPhoneX;
  int        i, j, k, w;
  Word       word, maskedWord, *maskedWordP;

  wordInit( &word, amodelSetPhones(amsP),
                   amodelSetTags(  amsP));
  wordInit( &maskedWord, amodelSetPhones(amsP),
	                 amodelSetTags(  amsP));

  if ( pgraphP->modTags )
      maskedWordP = &maskedWord;
  else
      maskedWordP = &word;

  phnXA[idx+1] = pgraphP->phone[pidx];
  tagXA[idx+1] = pgraphP->tag  [pidx];
  idxXA[idx+1] = pidx;

  for ( i = 0; i < pgraphP->fwdTrN[pidx]; i++) {
    int fwdIdx   = pgraphP->fwdIdx[pidx][i];
    trnXA[idx+1] = i;

    /* ---------------------------------------------------- */
    /* unroll phoneme graph                                 */
    /* ---------------------------------------------------- */

    if ( fwdIdx <= 0) continue;
    if ( fwdIdx < pgraphP->phoneN - pidx) {
      /* Let's go recursive...: the current transition seems to lead to 
         another state in the phoneGraph, so we increase the current phone
         index, keep the new-graph index and increase the array index
         idx by one
      */      
      nidx = pgraphExpandRec( pgraphP, pidx + fwdIdx,
                              ngraphP, nidx, stateXA, relXA,
                              left,  leftN, right, rightN,
                              phnXA, tagXA, idxXA, trnXA, idx + 1, 
			      xwmodels, rcmSdp,sdpXA);

    }
    if ( fwdIdx >= pgraphP->phoneN - pidx) {
      /* we reached an exit phone, setup now the context models */
	
      /* -------------------------------------------------- */
      /* setup first phone in phoneGraph                    */
      /* -------------------------------------------------- */

      int leftM  = (leftN >0)                           ? leftN  : 1;
      int rightM = (rcmSdp >0 && rightN >0 && idx == 0) ? rightN : 1;
      int  exitN = 0;
      int   etrn = 0;
      int exitX;
	
      /* setup exit transitions for 1-phone words */
      if (idx == 0) {
	etrn  =  fwdIdx - (pgraphP->phoneN - pidx) + (ngraphP->phoneN - nidx);
	exitN = 1;
      } 
      /* setup exit transitions for 2-phone words */
      if (idx == 1) {
	etrn  = leftM;
	exitN = (rightN > 0) ? rightN : 1;
      }
      /* setup exit transitions for regular words */
      if (idx > 1) {
	etrn  = leftM;
	exitN = 1;
      }

      word.phoneA  = phnXA;
      word.tagA    = tagXA;
      word.itemN   = idx+3;

      for ( j = 0; j < leftM; j++) {	  
	if (leftN >0) {
	  phnXA[0] = left[j].phone;
	  tagXA[0] = left[j].tag;
	} else {
	  phnXA[0] = padPhoneX;
	  tagXA[0] = 0;
	}
	for ( k = 0; k < rightM; k++, nidx++) {
	  if (rcmSdp >0 && idx == 0 && rightN > 0) {
	    phnXA[idx+2] = right[k].phone;
	    tagXA[idx+2] = right[k].tag;
	  } else {
	    phnXA[idx+2] = padPhoneX;
	    tagXA[idx+2] = 0;
	  }

	  /* store context information */
	  if (idx == 0 && rcmSdp > 0) {
	    sdpXA[nidx]   = 1;
	    relXA[nidx]   = (j>0) ? (-j) : idxXA[1];
	    stateXA[nidx] = -k;
	  } else {
	    sdpXA[nidx]   = 0;
	    stateXA[nidx] = (j) ? (-1*j) : idxXA[1];
	    relXA[nidx]   =  0;
	  }

	  ngraphP->phone [nidx] =  phnXA[1];
	  ngraphP->tag   [nidx] =  tagXA[1];
	  
	  if ( xwmodels) {
	    if ( pgraphP->modTags ) 
	      MOD_DEMASK (-1, idx);

	    if (idx == 0) {
	      /* single phone word */
	      if (rcmSdp == 2)
		/* mode 2 -> backoff to -1,0 context to access trained models */
		ngraphP->amodel[nidx] =  amodelSetGet(amsP,maskedWordP,-1,0);
	      else
		ngraphP->amodel[nidx] =  amodelSetGet(amsP,maskedWordP,-1,1);
	    } else {
	      	ngraphP->amodel[nidx] =  amodelSetGet(amsP,maskedWordP,-1,idx+1);
	    } 

	  } else {
	    word.phoneA++;
	    word.tagA++;
	    word.itemN--;
	    if ( pgraphP->modTags )
	      MOD_DEMASK (0, idx);
	    ngraphP->amodel[nidx] =  amodelSetGet(amsP,maskedWordP,0,idx);
	    word.phoneA--;
	    word.tagA--;
	    word.itemN++;
	  }
	  ngraphP->fwdTrN[nidx] =  exitN;
	  ngraphP->fwdIdx[nidx] = (int*  )malloc(exitN * sizeof(int));
	  ngraphP->fwdPen[nidx] = (float*)malloc(exitN * sizeof(float));
	  
	  for ( exitX = 0; exitX < exitN; exitX++) {
	    ngraphP->fwdIdx[nidx][exitX] = etrn + exitX;
	    ngraphP->fwdPen[nidx][exitX] = pgraphP->fwdPen[idxXA[1]][trnXA[1]];
	  }
	  etrn--;
	}
      }

      /* ----------------------------------------------------------------- */
      /* fill the inner phones with within-word polyphonic acoustic models */
      /* ----------------------------------------------------------------- */

      for ( j = 1; j < idx; j++) {
        int exitN = ( j == idx-1 && rightN) ? rightN : 1;
        int exitX;
	
        word.phoneA  = phnXA+1;
        word.tagA    = tagXA+1;
        word.itemN   = idx+1;
	
        stateXA[nidx]         =  idxXA[1+j];
        relXA[  nidx]         =  j;
        ngraphP->phone [nidx] =  phnXA[1+j];
        ngraphP->tag   [nidx] =  tagXA[1+j];
	if ( pgraphP->modTags )
	  MOD_DEMASK (-1*j, idx-j);
        ngraphP->amodel[nidx] =  amodelSetGet(amsP,maskedWordP, -1 * j, idx - j);
        ngraphP->fwdTrN[nidx] =  exitN;
        ngraphP->fwdIdx[nidx] = (int*  )malloc(exitN * sizeof(int));
        ngraphP->fwdPen[nidx] = (float*)malloc(exitN * sizeof(float));
	
        for ( exitX = 0; exitX < exitN; exitX++) {
          ngraphP->fwdIdx[nidx][exitX] = exitX + 1;
          ngraphP->fwdPen[nidx][exitX] = pgraphP->fwdPen[idxXA[1+j]][trnXA[1+j]];
        }
        nidx++;
      }

      /* ---------------------------------------------------- */
      /* setup ending phone with right context models         */
      /* ---------------------------------------------------- */
      
      if (idx) {
        int etrn = fwdIdx - (pgraphP->phoneN - pidx) +
	  (ngraphP->phoneN - nidx);
	
        if ( rightN) {
          for ( j = 0; j < rightN; j++) {
            phnXA[idx+2] = right[j].phone;
            tagXA[idx+2] = right[j].tag;
	    
            word.phoneA  = phnXA+1;
            word.tagA    = tagXA+1;
            word.itemN   = idx+2;
	    
            stateXA[nidx+j]         = (j) ? (-1*j) : idxXA[1+idx];
            relXA[  nidx+j]         =  idx;
            ngraphP->phone [nidx+j] =  phnXA[1+idx];
            ngraphP->tag   [nidx+j] =  tagXA[1+idx];
	    
            if ( xwmodels) {
	      if ( pgraphP->modTags )
		MOD_DEMASK (-1*idx, 1);
	      ngraphP->amodel[nidx+j] =  amodelSetGet(amsP,maskedWordP,-1*idx,1);
            } else {
              word.itemN--;
	      if ( pgraphP->modTags )
		MOD_DEMASK (-1*idx, 0);
	      
              ngraphP->amodel[nidx+j] =  amodelSetGet(amsP,maskedWordP,-1*idx,0);
              word.itemN++;
            }
	    
            ngraphP->fwdTrN[nidx+j] =  1;
            ngraphP->fwdIdx[nidx+j] = (int*  )malloc(sizeof(int));
            ngraphP->fwdPen[nidx+j] = (float*)malloc(sizeof(float));
	    
            ngraphP->fwdIdx[nidx+j][0] = etrn--;
            ngraphP->fwdPen[nidx+j][0] = pgraphP->fwdPen[idxXA[1+idx]][trnXA[1+idx]];
          }
          nidx += rightN;
        }
        else {

          word.phoneA  = phnXA+1;
          word.tagA    = tagXA+1;
          word.itemN   = idx+1;
	  
          stateXA[nidx]         =  idxXA[1+idx];
          relXA[  nidx]         =  idx;
          ngraphP->phone [nidx] =  phnXA[1+idx];
          ngraphP->tag   [nidx] =  tagXA[1+idx];
	  if ( pgraphP->modTags )
	    MOD_DEMASK (-1*idx, 0);
          ngraphP->amodel[nidx] =  amodelSetGet(amsP,maskedWordP, -1*idx, 0);
          ngraphP->fwdTrN[nidx] =  1;
          ngraphP->fwdIdx[nidx] = (int*  )malloc(sizeof(int));
          ngraphP->fwdPen[nidx] = (float*)malloc(sizeof(float));

          ngraphP->fwdIdx[nidx][0] = etrn;

          ngraphP->fwdPen[nidx][0] = pgraphP->fwdPen[idxXA[1+idx]][trnXA[1+idx]];

          nidx++;
        }
      }
    }         
  }

  if ( pgraphP->modTags ) {
      if ( maskedWordP->tagA ) {
	  free (maskedWordP->tagA); maskedWordP->tagA = NULL;
      }
  }

  return nidx;
}

/* ------------------------------------------------------------------------
    pgraphExpand    creates an unrolled version of a given phoneGraph which
                    has context dependent entry and exit states. The lists
                    of potential left and right context phones are <left>
                    and <right>. The init[] structure of the new phone
                    pgraph is yet to be defined. Instead of this, a state
                    index array <stateXA> is used to save for each state
                    in the unrolled graph the state index of the original
                    state within pgraph. For replicated entry and exit
                    states we store the state index for the first instance 
                    only and set the replicated states to -offset from the
                    that first one. Thus, we achieve two things: we can
                    detect entry point to the unrolled graph by looking up
                    pgraph->init[] and scanning the stateXA to find these
                    indices. The -offset stuff is used later in the build
                    phoneGraph procedure, where we have to match the right
                    and left context arrays for each word transition again.
   ------------------------------------------------------------------------ */

PhoneGraph* pgraphExpand( PhoneGraph*        pgraphP,
			  PhoneGraphContext* left,  int leftN,
			  PhoneGraphContext* right, int rightN,
			  int**              pstateXA,
			  int**              prelXA,
			  int                xwmodels,
			  int                rcmSdp,
			  int**              psdpXA)
{
  int*        phnXA   = (int  *)malloc((pgraphP->phoneN+2) * sizeof(int)); 
  int*        tagXA   = (int  *)malloc((pgraphP->phoneN+2) * sizeof(int));
  int*        idxXA   = (int  *)malloc((pgraphP->phoneN+2) * sizeof(int));
  int*        trnXA   = (int  *)malloc((pgraphP->phoneN+2) * sizeof(int));
  PhoneGraph* ngraphP = NULL;
  int*        stateXA;
  int*        relXA;
  int*        sdpXA;
  int         i, phnN = 0, nidx;

  /* count the number of phones that need to be allocated for the expanded
     phone graph */

  for ( i = 0; i < pgraphP->initN; i++)
     phnN += pgraphExpandCount( pgraphP, pgraphP->init[i], leftN, rightN, 1,rcmSdp);

  /* allocate new phoneGraph */

  ngraphP = pgraphCreate( pgraphP->amsP, NULL, 0, 0, phoneGraphDefault.modTags);

  ngraphP->phoneN     =  phnN;
  ngraphP->phone      = (int*   )malloc( phnN * sizeof(int));
  ngraphP->tag        = (int*   )malloc( phnN * sizeof(int));
  ngraphP->amodel     = (int*   )malloc( phnN * sizeof(int));
  ngraphP->fwdTrN     = (int*   )malloc( phnN * sizeof(int));
  ngraphP->fwdIdx     = (int**  )malloc( phnN * sizeof(int*));
  ngraphP->fwdPen     = (float**)malloc( phnN * sizeof(float*));
  stateXA             = (int*   )malloc( phnN * sizeof(int));
  relXA               = (int*   )malloc( phnN * sizeof(int));
  sdpXA               = (int*   )malloc( phnN * sizeof(int));

  for (i=0;i<phnN;i++) sdpXA[i]=0;

  /* expand/unroll the phoneGraph also considering all possible left
     and right context models */

  for ( nidx = 0, i = 0; i < pgraphP->initN; i++) {

     nidx = pgraphExpandRec( pgraphP, pgraphP->init[i], 
                             ngraphP, nidx, stateXA, relXA,
                             left,  leftN, right, rightN,
                             phnXA, tagXA, idxXA, trnXA, 0, xwmodels, rcmSdp,sdpXA);
  }

  if ( phnXA)    { free( phnXA);   phnXA   = NULL; }
  if ( tagXA)    { free( tagXA);   tagXA   = NULL; }
  if ( idxXA)    { free( idxXA);   idxXA   = NULL; }
  if ( trnXA)    { free( trnXA);   trnXA   = NULL; }

  if ( pstateXA) { *pstateXA = stateXA; }
  else           {  free( stateXA); stateXA = NULL; }

  if ( prelXA)   { *prelXA = relXA; }
  else           {  free( relXA); relXA = NULL; }

  if ( psdpXA)   { *psdpXA = sdpXA; }
  else           {  free( sdpXA); sdpXA = NULL; }

  return ngraphP;
}

/* ------------------------------------------------------------------------
    pgraphBuild     converts a wordGraph to a phoneGraph by expanding all
                    phoneGraphs within wgraph, making them context depend
                    and then merging them into a single graph structure.
   ------------------------------------------------------------------------ */

static int padPhoneWarned = 0;

int pgraphBuild( PhoneGraph* pgraphP, WordGraph* wgraphP, int logPen, 
                 int full, int xwmodels, int rcmSdp)
{
  PhoneGraph**        ngraphP = NULL;
  int**               stateXA = NULL;
  int**               relXA   = NULL;
  int*                offXA   = NULL;
  int**               sdpXA   = NULL;
  PhoneGraphContext** left    = NULL;
  PhoneGraphContext** right   = NULL;
  int*                leftN   = NULL;
  int*                rightN  = NULL;
  int                 i, j, k, l, m, idx;
  int                 phnN;
  int  padPhoneX = (amodelSetTree(wgraphP->amsP))->questions.padPhoneX;

  assert(      pgraphP && wgraphP);
  pgraphReset( pgraphP);

  if (! wgraphP->phoneGraph) {
    ERROR( "WordGraph has no PhoneGraphs allocated.\n");
    return TCL_ERROR;
  }
  if (  wgraphBuildContext( wgraphP, &left,  &leftN, 
                                     &right, &rightN) != TCL_OK) {
    ERROR( "Failed to build the phone context table.\n");
    return TCL_ERROR;
  }

  ngraphP = (PhoneGraph**)malloc( sizeof(PhoneGraph*) * wgraphP->wordN);
  stateXA = (int**       )malloc( sizeof(int*)        * wgraphP->wordN);
  relXA   = (int**       )malloc( sizeof(int*)        * wgraphP->wordN);
  offXA   = (int*        )malloc( sizeof(int)         * wgraphP->wordN);
  sdpXA   = (int**       )malloc( sizeof(int*)        * wgraphP->wordN);

  for ( phnN = 0, i = 0; i < wgraphP->wordN; i++) {
    offXA[i]   = phnN;
    ngraphP[i] = pgraphExpand( wgraphP->phoneGraph[i], 
                               left[i],  leftN[i], 
                               right[i], rightN[i],
                             &(stateXA[i]),&(relXA[i]), 
			       xwmodels, rcmSdp,  &(sdpXA[i]));
    phnN      += ngraphP[i]->phoneN;
  }

  /* allocate the new phone graph */

  pgraphP->phoneN     =  phnN;
  pgraphP->phone      = (int*   )malloc( phnN * sizeof(int));
  pgraphP->tag        = (int*   )malloc( phnN * sizeof(int));
  pgraphP->word       = (int*   )malloc( phnN * sizeof(int));
  pgraphP->amodel     = (int*   )malloc( phnN * sizeof(int));
  pgraphP->fwdTrN     = (int*   )malloc( phnN * sizeof(int));
  pgraphP->fwdIdx     = (int**  )malloc( phnN * sizeof(int*));
  pgraphP->fwdPen     = (float**)malloc( phnN * sizeof(float*));

  for ( idx = 0, i = 0; i < wgraphP->wordN; i++) {
    for ( j = 0; j < ngraphP[i]->phoneN; j++) {

      pgraphP->phone[idx]  = ngraphP[i]->phone[j];
      pgraphP->tag[idx]    = ngraphP[i]->tag[j];
      pgraphP->word[idx]   = i;
      pgraphP->amodel[idx] = ngraphP[i]->amodel[j];
      pgraphP->fwdTrN[idx] = 0;

      /* Pass 1: go through all transition models and count the number
         of transitions that are to be allocated */

      for ( k = 0; k < ngraphP[i]->fwdTrN[j]; k++) {
        if ( ngraphP[i]->fwdIdx[j][k] + j >= ngraphP[i]->phoneN) {
          int initX = ngraphP[i]->fwdIdx[j][k] + j - ngraphP[i]->phoneN;

          /* For transitions leaving a phone model jump to all possible
             successor words */

          for ( l = 0; l < wgraphP->fwdTrN[i]; l++) {
            int  fwdIdx =      wgraphP->fwdIdx[i][l];
            if ( fwdIdx + i >= wgraphP->wordN) {
              pgraphP->fwdTrN[idx] += 1;
            }
            else if ( initX <  wgraphP->phoneGraph[fwdIdx+i]->initN) {
              int fromX  = (!initX && full) ? 0 : initX;
              int toX    = (!initX && full) ? 
                             wgraphP->phoneGraph[fwdIdx+i]->initN : initX+1;
              int entryX;

              for ( entryX = fromX; entryX < toX; entryX++) {
                int entry     =  wgraphP->phoneGraph[fwdIdx+i]->init[entryX];

		/* regular words or right context dependent single phone words */
                if ( relXA[i][j] || rcmSdp > 0) {  
                  int rc      = (stateXA[i][j] < 0) ? (-1*stateXA[i][j]) : 0;
                  int rphone  =  right[i][rc].phone;
                  int rtag    =  right[i][rc].tag;
                  int s;

                  /* entry point for a transition into another phone graph */
                  for ( s = 0; s < ngraphP[fwdIdx + i]->phoneN; s++) {
		    int phoneX  = stateXA[fwdIdx+i][s];
		    int   tmpS  = s-1;

		    /* phoneX = index in original pgraph for index /s/ in ngraphP */
		    while (phoneX < 0) {
		      assert(tmpS>=0);
		      phoneX = stateXA[fwdIdx+i][tmpS--];		     
		    }

		    /* check if we go into a single phone word */
		    if (sdpXA[fwdIdx+i][s] == 1) {
		      int lc      = (relXA[fwdIdx+i][s] < 0) ? (-1*relXA[fwdIdx+i][s]) : 0;
		      int lphone  =  left[fwdIdx+i][lc].phone;
		      int ltag    =  left[fwdIdx+i][lc].tag;

		      /* check left and right context, and index in phonegraph */
		      if (pgraphP->phone[idx]         == lphone && 
			  pgraphP->tag[idx]           == ltag   &&
			  ngraphP[fwdIdx+i]->phone[s] == rphone &&
			  ngraphP[fwdIdx+i]->tag[s]   == rtag   &&
			  phoneX == entry) {
       			pgraphP->fwdTrN[idx] += 1;
		      }
		    } else {		      
		      if ( stateXA[fwdIdx+i][s]                        == entry &&
			   wgraphP->phoneGraph[fwdIdx+i]->phone[entry] == rphone &&
			   wgraphP->phoneGraph[fwdIdx+i]->tag[entry]   == rtag) {
			pgraphP->fwdTrN[idx] += 1;
		      }
		    }
                  }
                }
		/* single phone words without right context  */
                else {
		  int s;
		  for ( s = 0; s < ngraphP[fwdIdx + i]->phoneN; s++) {
		    if ( stateXA[fwdIdx+i][s] == entry) 
		      pgraphP->fwdTrN[idx] += 1;
		  }
                }
	      }
            }
	  }
        }
        else {
          pgraphP->fwdTrN[idx] += 1;
        }
      }

      /* allocate the transition model */

      if ( pgraphP->fwdTrN[idx]) {
        pgraphP->fwdIdx[idx] = (int*  )malloc(pgraphP->fwdTrN[idx]*sizeof(int));
        pgraphP->fwdPen[idx] = (float*)malloc(pgraphP->fwdTrN[idx]*sizeof(float));

        for ( k = 0; k < pgraphP->fwdTrN[idx]; k++) {
          pgraphP->fwdIdx[idx][k] = 0;
          pgraphP->fwdPen[idx][k] = 1.0;
        }
      }
      else {
        pgraphP->fwdIdx[idx] = NULL;
        pgraphP->fwdPen[idx] = NULL;
      }

      /* Pass 2: fill the transition models */

      for ( m = 0, k = 0; k < ngraphP[i]->fwdTrN[j]; k++) {
        if ( ngraphP[i]->fwdIdx[j][k] + j >= ngraphP[i]->phoneN) {
          int initX = ngraphP[i]->fwdIdx[j][k] + j - ngraphP[i]->phoneN;

          for ( l = 0; l < wgraphP->fwdTrN[i]; l++) {
            int fwdIdx = wgraphP->fwdIdx[i][l];
            if ( fwdIdx + i >= wgraphP->wordN) {

              /* since we do know nothing about the following word graph
                 there is no way to make use of ngraphP[i]->fwdIdx[j][k]
                 to find the entry point into the next word */

              pgraphP->fwdIdx[idx][m] = pgraphP->phoneN - idx +
                                        fwdIdx + i - wgraphP->wordN;
              pgraphP->fwdPen[idx][m] = ( logPen) ? 
                       ngraphP[i]->fwdPen[j][k] + wgraphP->fwdPen[i][l] :
                       ngraphP[i]->fwdPen[j][k] * wgraphP->fwdPen[i][l];
              m++;
            }
            else if ( initX < wgraphP->phoneGraph[fwdIdx+i]->initN) {  
              int fromX  = (!initX && full) ? 0 : initX;
              int toX    = (!initX && full) ? 
                             wgraphP->phoneGraph[fwdIdx+i]->initN : initX+1;
              int entryX;

              for ( entryX = fromX; entryX < toX; entryX++) {
                int entry     =  wgraphP->phoneGraph[fwdIdx+i]->init[entryX];

		/* regular words or single phone words with right context */
                if ( relXA[i][j] || rcmSdp > 0) { 
                  int rc     = (stateXA[i][j] < 0) ? (-1*stateXA[i][j]) : 0;
                  int rphone =  right[i][rc].phone;
                  int rtag   =  right[i][rc].tag;
                  int phone  =  ngraphP[i]->phone[j];
                  int tag    =  ngraphP[i]->tag[j];
                  int s, lc;

                  for ( lc = 0; lc < leftN[fwdIdx+i]; lc++) {
                    if ( left[fwdIdx+i][lc].phone == phone && 
                         left[fwdIdx+i][lc].tag   == tag) break;
                  }

                  assert( lc < leftN[fwdIdx+i]);

                  for ( s = 0; s < ngraphP[fwdIdx + i]->phoneN; s++) {
		    int phoneX = stateXA[fwdIdx+i][s];
		    int   tmpS = s-1;

		    /* phoneX = index in original pgraph for index /s/ in ngraphP */
		    while (phoneX < 0) {
		      assert(tmpS>=0);
		      phoneX = stateXA[fwdIdx+i][tmpS--];		     
		    }

		    /* check if we go into a single phone word */
		    if (sdpXA[fwdIdx+i][s] == 1) {
		      int lc      = (relXA[fwdIdx+i][s] < 0) ? (-1*relXA[fwdIdx+i][s]) : 0;
		      int lphone  =  left[fwdIdx+i][lc].phone;
		      int ltag    =  left[fwdIdx+i][lc].tag;

		      /* check left and right context, and index in phonegraph */
		      if (pgraphP->phone[idx]         == lphone && 
			  pgraphP->tag[idx]           == ltag   &&
			  ngraphP[fwdIdx+i]->phone[s] == rphone &&
			  ngraphP[fwdIdx+i]->tag[s]   == rtag   &&
			  phoneX == entry) {
			pgraphP->fwdIdx[idx][m] = offXA[fwdIdx+i]+s-idx;
			pgraphP->fwdPen[idx][m] = ( logPen) ? 
			  ngraphP[i]->fwdPen[j][k] + wgraphP->fwdPen[i][l] :
			  ngraphP[i]->fwdPen[j][k] * wgraphP->fwdPen[i][l];
			m++;
		      }
		    } else {		      
		      if ( stateXA[fwdIdx+i][s]                        == entry &&
			   wgraphP->phoneGraph[fwdIdx+i]->phone[entry] == rphone &&
			   wgraphP->phoneGraph[fwdIdx+i]->tag[entry]   == rtag) {

			pgraphP->fwdIdx[idx][m] = offXA[fwdIdx+i]+s+lc-idx;
			pgraphP->fwdPen[idx][m] = ( logPen) ? 
			  ngraphP[i]->fwdPen[j][k] + wgraphP->fwdPen[i][l] :
			  ngraphP[i]->fwdPen[j][k] * wgraphP->fwdPen[i][l];
			m++;
		      }
		    }
		  }
		}
		/* single phone words without right context */
                else {
                  int phone  =  ngraphP[i]->phone[j];
                  int tag    =  ngraphP[i]->tag[j];
                  int s, lc;

                  for ( lc = 0; lc < leftN[fwdIdx+i]; lc++) {
                    if ( left[fwdIdx+i][lc].phone == phone && 
                         left[fwdIdx+i][lc].tag   == tag) break;
                  }

                  assert( lc < leftN[fwdIdx+i]);

                  for ( s = 0; s < ngraphP[fwdIdx + i]->phoneN; s++) {
                    if ( stateXA[fwdIdx+i][s] == entry) {

                      pgraphP->fwdIdx[idx][m] = offXA[fwdIdx+i]+s+lc-idx;
                      pgraphP->fwdPen[idx][m] = ( logPen) ? 
                               ngraphP[i]->fwdPen[j][k] + wgraphP->fwdPen[i][l] :
                               ngraphP[i]->fwdPen[j][k] * wgraphP->fwdPen[i][l];
                      m++;
		    }
                  }
                }
	      }
            }
          }
        }
        else {
          pgraphP->fwdIdx[idx][m] = ngraphP[i]->fwdIdx[j][k];
          pgraphP->fwdPen[idx][m] = ngraphP[i]->fwdPen[j][k];
          m++;
        }
      }
      assert ( m == pgraphP->fwdTrN[idx]);
      idx++;
    }
  }

#if USE_PAD
  /* define entry points to the phone graph by looking for padPhone context */
  for ( i = 0; i < wgraphP->initN; i++) {
    int initX = wgraphP->init[i];
    if ( initX >= wgraphP->wordN) { continue; }

    /* no left context available: go into all starting phones */
    if (leftN[initX] == 0) {
      for ( j = 0; j < wgraphP->phoneGraph[initX]->initN; j++) {
        int entry = wgraphP->phoneGraph[initX]->init[j];
       
	for ( k = 0; k < ngraphP[initX]->phoneN; k++) {
	  if ( ( sdpXA[initX][k] == 1 && relXA[initX][k]   == entry ) ||
	       ( sdpXA[initX][k] == 0 && stateXA[initX][k] == entry ) ) {

	    pgraphP->init = (int*)realloc( pgraphP->init, (pgraphP->initN + 1) * sizeof(int));
	    pgraphP->init[pgraphP->initN] = offXA[initX] + k;
	    pgraphP->initN++;
	  }
	}
      }
      continue;
    }

    for ( k = 0; k < ngraphP[initX]->phoneN; k++) {
      int lphone = -1;

      if (sdpXA[initX][k] == 0 ) {
	/* left context of first phone of a regular word is stored in stateXA */
	int lc = (stateXA[initX][k] >0) ? 0 : -1*stateXA[initX][k];
	/* check if this a starting phone (and not an ending phone) */
	if (relXA[initX][k] == 0) {
	  lphone = left[initX][lc].phone;
	}
      } else {
	/* left context of a single phone word is stored in relXA */
	int lc = (relXA[initX][k] >0)   ? 0 : -1*relXA[initX][k];
	lphone = left[initX][lc].phone;
      }
      if (lphone == padPhoneX) {
	pgraphP->init = (int*)realloc( pgraphP->init, (pgraphP->initN + 1) * sizeof(int));
	pgraphP->init[pgraphP->initN] = offXA[initX] + k;
	pgraphP->initN++;
      }
    }
  }

#else 
  if (padPhoneX < 0 && !padPhoneWarned) {
    padPhoneWarned = 1;
    WARN ("pgraphBuild: -padPhoneX < 0 => CI-system?\n");
  }

  /* --------------------- */
  /* OLD INIT STYLE        */
  /* --------------------- */

  for ( i = 0; i < wgraphP->initN; i++) {
    int initX = wgraphP->init[i];
    int leftM  = 0;
    int rightM = 1;

    /* exit the word graph, nothing to do */
    if ( initX >= wgraphP->wordN) continue;

    /* number of left and right context replications */
    leftM  = (! leftN  || leftN[initX]  == 0) ? 1 : leftN[initX];
    //rightM = 1;

    for ( j = 0; j < wgraphP->phoneGraph[initX]->initN; j++) {
      int entry = wgraphP->phoneGraph[initX]->init[j];
	
        for ( k = 0; k < ngraphP[initX]->phoneN; k++) {

	  /* check entry points for regular words */
	  if (sdpXA[initX][k] == 0 && stateXA[initX][k] != entry) continue;

	  /* check entry points for single phone words */
	  if (sdpXA[initX][k] == 1 && (relXA[initX][k] != entry || stateXA[initX][k] < 0)) continue;

	  /* check if we are in a single phone word : go into each instance */
	  if (sdpXA[initX][k] == 1 && rcmSdp > 0) {
	    rightM = (! rightN || rightN[initX] == 0) ? 1 : rightN[initX];
	  } else {
	    rightM = 1;
	  }

	  /* set up starting phones */
	  pgraphP->init = (int*)realloc( pgraphP->init,
					 (pgraphP->initN + leftM*rightM) * sizeof(int));
	  for ( m = 0; m < leftM*rightM; m++) {
	    pgraphP->init[pgraphP->initN] = offXA[initX] + k + m;
	    pgraphP->initN++;
	  }
        }
    }
  }

#endif

  /* build the initial state graphs for each phone and define the 
     relative word and relative phone index */

  pgraphMakeSGraph( pgraphP);

  for ( i = 0; i < pgraphP->phoneN; i++) {
    for ( j = 0; j < pgraphP->stateGraph[i]->stateN; j++) {
      pgraphP->stateGraph[i]->phoneX[j] = i;
      pgraphP->stateGraph[i]->wordX [j] = pgraphP->word[i];
    }
  }

  /* free allocated data structures */

  if ( left) {
    for ( i = 0; i < wgraphP->wordN; i++) if ( left[i]) free( left[i]);
    free( left);
  }
  if ( right) {
    for ( i = 0; i < wgraphP->wordN; i++) if ( right[i]) free( right[i]);
    free( right);
  }
  if ( leftN)  free( leftN);
  if ( rightN) free( rightN);

  if ( ngraphP) {
    for ( i = 0; i < wgraphP->wordN; i++) pgraphFree( ngraphP[i]);
    free( ngraphP);
  }
  if ( stateXA) { 
    for ( i = 0; i < wgraphP->wordN; i++) if ( stateXA[i]) free( stateXA[i]);
    free( stateXA);
  }
  if ( relXA) { 
    for ( i = 0; i < wgraphP->wordN; i++) if ( relXA[i]) free( relXA[i]);
    free( relXA);
  }
  if ( sdpXA) { 
    for ( i = 0; i < wgraphP->wordN; i++) if ( sdpXA[i]) free( sdpXA[i]);
    free( sdpXA);
  }
  if ( offXA) free( offXA);

  return TCL_OK;
}

static int pgraphBuildItf (ClientData cd, int argc, char *argv[])
{
  PhoneGraph* pgraphP  = (PhoneGraph*)cd;
  WordGraph*  wgraphP  =  NULL;
  int         logPen   =  1;
  int         full     =  1;
  int         xwmodels =  1;
  int         rcmSdp   =  0;
  int         ac       =  argc - 1;

  if ( itfParseArgv( argv[0], &ac, argv+1, 0,
      "<wordGraph>", ARGV_OBJECT, NULL, &wgraphP, "WordGraph", "word graph", 
      "-logPen",     ARGV_INT,    NULL, &logPen,   NULL, "log penalties",
      "-full",       ARGV_INT,    NULL, &full,     NULL, "full PGhraph to PGraph transitions",
      "-xwmodels",   ARGV_INT,    NULL, &xwmodels, NULL, "xword models",
      "-rcmSdp",     ARGV_INT,    NULL, &rcmSdp,   NULL, "right context models for single phone words",
      NULL) != TCL_OK) return TCL_ERROR;

  return pgraphBuild( pgraphP, wgraphP, logPen, full, xwmodels,rcmSdp);
}

/* ========================================================================
    WordGraph
   ======================================================================== */

static ClientData wgraphCreateItf( ClientData clientData, int   argc, 
                                                          char *argv[]);
static int wgraphFreeItf (ClientData clientData);

/* ------------------------------------------------------------------------
    Create WordGraph Structure:
   ------------------------------------------------------------------------ */

int wgraphInit( WordGraph* WG, Dictionary* dict, AModelSet* amsP, IArray* waP)
{
  assert( dict && amsP);

  WG->wordN      = 0;
  WG->word       = NULL;
  WG->phoneGraph = NULL;

  WG->dict       = dict; link( dict, "Dictionary");
  WG->amsP       = amsP; link( amsP, "AModelSet");

  WG->initN      = 0;
  WG->init       = NULL;

  WG->fwdTrN     = NULL;
  WG->fwdIdx     = NULL;
  WG->fwdPen     = NULL;

  WG->modTagsA   = NULL;

  if ( waP) return wgraphMake( WG, waP, NULL, NULL, -1, 0);
  else      return TCL_OK;
}

WordGraph* wgraphCreate( Dictionary* dict, AModelSet* amsP, IArray* waP)
{
  WordGraph* wgraphP = (WordGraph*)malloc(sizeof(WordGraph));

  if (! wgraphP || wgraphInit( wgraphP, dict, amsP, waP) != TCL_OK) {
    if ( wgraphP) free( wgraphP);
    ERROR("Failed to allocate WordGraph.\n"); 
    return NULL;
  }
  return wgraphP;
}

static ClientData wgraphCreateItf( ClientData clientData, 
                                   int argc, char *argv[])
{ 
  Dictionary* dict    = NULL;
  AModelSet*  amsP    = NULL;
  char*       name    = NULL;

  if (itfParseArgv (argv[0], &argc, argv, 0,
      "<name>",        ARGV_STRING, NULL, &name, NULL,         "name of the word graph",
      "<dictionary>",  ARGV_OBJECT, NULL, &dict, "Dictionary", "name of the Dictionary object", 
      "<amodelSet>",   ARGV_OBJECT, NULL, &amsP, "AModelSet",  "acoustic model set",
      NULL) != TCL_OK) return NULL;
  
  return (ClientData) wgraphCreate (dict, amsP, NULL);
}

/* ------------------------------------------------------------------------
    Free WordGraph Structure
   ------------------------------------------------------------------------ */

int wgraphReset(  WordGraph* WG)
{
  int i;

  if ( WG->phoneGraph) {
    for ( i = 0; i < WG->wordN; i++) pgraphFree( WG->phoneGraph[i]);
    free( WG->phoneGraph); WG->phoneGraph = NULL;
  }
  if ( WG->word)    { free( WG->word);    WG->word    = NULL; }
  if ( WG->init)    { free( WG->init);    WG->init    = NULL; }
  if ( WG->fwdIdx)  { 
    for ( i = 0; i < WG->wordN; i++) if (WG->fwdIdx[i]) free(WG->fwdIdx[i]); 
    free( WG->fwdIdx);  WG->fwdIdx = NULL;
  }
  if ( WG->fwdPen)  { 
    for ( i = 0; i < WG->wordN; i++) if (WG->fwdPen[i]) free(WG->fwdPen[i]); 
    free( WG->fwdPen);  WG->fwdPen  = NULL;
  }
  if ( WG->fwdTrN)  { free( WG->fwdTrN);  WG->fwdTrN  = NULL; }

  WG->wordN = 0;
  WG->initN = 0;

  return TCL_OK;
}

int wgraphDeinit( WordGraph* WG)
{
  if ( WG->dict) { unlink( WG->dict, "Dictionary"); WG->dict = NULL; }
  if ( WG->amsP) { unlink( WG->amsP, "AModelSet");  WG->amsP = NULL; }

  if ( WG->modTagsA ) { free (WG->modTagsA); WG->modTagsA = NULL; }

  return wgraphReset( WG);
}

int wgraphFree( WordGraph* WG)
{
  if ( wgraphDeinit( WG) != TCL_OK) return TCL_ERROR;
  if ( WG) free( WG);
  return TCL_OK;
}

static int wgraphFreeItf (ClientData clientData)
{ 
  return wgraphFree ((WordGraph*)clientData);
}

/* ------------------------------------------------------------------------
    wgraphAccessItf
   ------------------------------------------------------------------------ */

static ClientData wgraphAccessItf (ClientData cd, char *name, TypeInfo **ti)
{ 
  WordGraph* wgraphP  = (WordGraph*)cd;
  int   i;

  if (*name == '.') { 
    if (name[1]=='\0') {
      if ( wgraphP->dict) itfAppendElement( "dictionary");
      if ( wgraphP->amsP) itfAppendElement( "amodelSet");
      if ( wgraphP->phoneGraph)
           itfAppendElement( "phoneGraph(0..%d)",wgraphP->wordN-1);
      *ti = NULL; return NULL;
    }
    else {
      name++;
      if ( wgraphP->dict      && ! strcmp( name, "dictionary")) {
        *ti = itfGetType("Dictionary");
        return (ClientData)wgraphP->dict;
      }
      else if ( wgraphP->amsP && ! strcmp( name, "amodelSet")) {
        *ti = itfGetType("AModelSet");
        return (ClientData)wgraphP->amsP;
      }
      else if (wgraphP->phoneGraph && sscanf(name,"phoneGraph(%d)",&i)==1) {
	*ti = itfGetType("PhoneGraph");

	 if ( i >= 0 && i < wgraphP->wordN)
              return (ClientData)(wgraphP->phoneGraph[i]);
	 else return  NULL;
      }
      return NULL;
    }
  }
  return NULL;
}

/* ------------------------------------------------------------------------
    wgraphPutsItf
   ------------------------------------------------------------------------ */

static int wgraphPutsItf( ClientData cd, int argc, char *argv[])
{
  WordGraph* wgraphP = (WordGraph*)cd;
  int        i, j;

  itfAppendResult( "{");
  for ( i = 0; i < wgraphP->wordN; i++) {
    itfAppendElement( "%s", dictName( wgraphP->dict, wgraphP->word[i]));
  }
  itfAppendResult( "} {");
  for ( i = 0; i < wgraphP->wordN; i++) {
    itfAppendResult( " {");
    for ( j = 0; j < wgraphP->fwdTrN[i]; j++) {
      itfAppendElement( "%d %f", wgraphP->fwdIdx[i][j], 
                                 wgraphP->fwdPen[i][j]);
    }
    itfAppendResult( "}");
  }
  itfAppendResult( "} {");
  for ( i = 0; i < wgraphP->initN; i++) {
    itfAppendElement( "%d", wgraphP->init[i]);
  }
  itfAppendResult( "}");
  return TCL_OK;
}

/* ------------------------------------------------------------------------
    wgraphMake  creates a word graph from a description consisting at least
                of a list of words (given as index array). Given that 
                information only, the resp. phone graphs are generated and
                the word graph is defined as a list of words. In order to
                define a graph like transition structure, a list of the
                transition models for each word can be defined. It is also
                possible to setup an array of inital state indices.
   ------------------------------------------------------------------------ */

int wgraphMake( WordGraph* wgraphP, IArray* warray, char* transS,
                                    IArray* iarray, int   optWord,
                                    int     variants)
{
  int wordX;

  assert(      wgraphP && warray);
  wgraphReset( wgraphP);

  /* CF: zwischen jedes Wort (incl. Anfang und Ende) wird ein optWord gesetzt */
  if ( optWord >= 0) {
    wgraphP->wordN      =  2 * warray->itemN + 1;
    wgraphP->word       = (int*)malloc(wgraphP->wordN * sizeof(int));
    wgraphP->word[0]    =  optWord;

    for ( wordX = 0; wordX < warray->itemN; wordX++) {
      wgraphP->word[2*wordX+1] = warray->itemA[wordX];
      wgraphP->word[2*wordX+2] = optWord;
    }
    if ( warray->itemA) free( warray->itemA); 
  }
  else {
    if (! warray->itemA) return TCL_ERROR;

    wgraphP->wordN = warray->itemN;
    wgraphP->word  = warray->itemA;
  }
  wgraphP->phoneGraph = (PhoneGraph**)malloc( wgraphP->wordN * 
                                              sizeof(PhoneGraph*));

  /* setup the initial phone graphs for each word by reading the dictionary
     entries. The relative word indices for each phoneGraph are defined */

  for ( wordX = 0; wordX < wgraphP->wordN; wordX++) {
    int  phnX;
    int  modTags = phoneGraphDefault.modTags;

    if ( wgraphP->modTagsA )
	modTags = wgraphP->modTagsA[wordX];

    wgraphP->phoneGraph[wordX] = pgraphCreate( wgraphP->amsP, wgraphP->dict,
                                               wgraphP->word[wordX],
                                               variants, modTags);
    if (! wgraphP->phoneGraph[wordX]->word)
          wgraphP->phoneGraph[wordX]->word = (int*)malloc(sizeof(int) *
          wgraphP->phoneGraph[wordX]->phoneN);

    for ( phnX = 0; phnX < wgraphP->phoneGraph[wordX]->phoneN; phnX++)
      wgraphP->phoneGraph[wordX]->word[phnX] = wordX;
  }

  /* Define the transition structure either according to the string
     specification or (if none is specified) as a simple list of words */

  if (! transS) {
    int trX;

    wgraphP->fwdTrN = (int*   )malloc(wgraphP->wordN * sizeof(int));
    wgraphP->fwdIdx = (int**  )malloc(wgraphP->wordN * sizeof(int*));
    wgraphP->fwdPen = (float**)malloc(wgraphP->wordN * sizeof(float*));

    for ( wordX = 0; wordX < wgraphP->wordN; wordX++) {

      wgraphP->fwdTrN[wordX] = (optWord>=0 && (wordX%2)) ? 2 : 1;
      wgraphP->fwdIdx[wordX] = (int*  )malloc(wgraphP->fwdTrN[wordX] * sizeof(int));
      wgraphP->fwdPen[wordX] = (float*)malloc(wgraphP->fwdTrN[wordX] * sizeof(float));
    
      for (trX=0; trX<wgraphP->fwdTrN[wordX]; trX++) {
        wgraphP->fwdIdx[wordX][trX] = 1+trX;
        wgraphP->fwdPen[wordX][trX] = 0.0;
      }
    }
  }
  else {
    int  n  = tmArrayGetItf( transS, &(wgraphP->fwdTrN), &(wgraphP->fwdIdx), 
                                     &(wgraphP->fwdPen));
    if ( n != wgraphP->wordN) {
      ERROR( "Wrong number of transition models (%d).\n", n);
      wgraphReset( wgraphP);
      return TCL_ERROR;
    }

    /* TODO: translate a given transition structure, if optWord is defined */
  }

  /* define the set of initial states */

  if ( iarray->itemA) {
    wgraphP->initN   = iarray->itemN;
    wgraphP->init    = iarray->itemA;
  }
  else {
    wgraphP->initN   = (optWord >= 0 && wgraphP->wordN > 1) ? 2 : 1;
    wgraphP->init    = (int*)malloc(wgraphP->initN*sizeof(int));
    wgraphP->init[0] = 0;
    if ( optWord >= 0  && wgraphP->wordN > 1) wgraphP->init[1] = 1;
  }
  return TCL_OK;
}

static int wgraphMakeItf ( ClientData clientData, int argc, char *argv[] )
{
  WordGraph*  wgraphP   = (WordGraph*)clientData;
  int         ac        =  argc-1;
  IArray      warray    = {NULL, 0};
  IArray      iarray    = {NULL, 0};
  char*       transS    =  NULL;
  int         variants  =  0;
  int         optWord   = -1;

  if (itfParseArgv(argv[0], &ac, argv+1, 0,
     "<words>",  ARGV_LIARRAY,NULL,&warray,&(wgraphP->dict->list),
     "list of word nodes", 
     "-trans",   ARGV_STRING, NULL,&transS,NULL, "transition model",
     "-init",    ARGV_IARRAY, NULL,&iarray,NULL, "initial states",
     "-optWord", ARGV_LINDEX, NULL,&optWord,&(wgraphP->dict->list),
     "optional word",
     "-variants",ARGV_INT,    NULL,&variants,NULL, "pronunciation variants",
     NULL) != TCL_OK) {
     if ( warray.itemA) free( warray.itemA);
     if ( iarray.itemA) free( iarray.itemA);
     return TCL_ERROR;
  }
  if ( wgraphMake( wgraphP, &warray, transS, &iarray, 
                   optWord,  variants) != TCL_OK) {
    if ( warray.itemA) free( warray.itemA);
    if ( iarray.itemA) free( iarray.itemA);
    return TCL_ERROR;
  }
  return TCL_OK;
}

/* ------------------------------------------------------------------------
    wgraphLattice
   ------------------------------------------------------------------------ */

static int wgraphLattice( WordGraph* wgraphP, char* lattice)
{
  char *q, *p = strtok(lattice,"\n");
  int   wordN = 0, vertN = 0, initX, wordX, vertX, trX, from, to;
  int  *fromA, *toA, *firstA;
  char **wordA;

  wgraphReset( wgraphP);

  /* ------------------------------------------------------------------- */
  /* first count the words (edges) in the lattice, and the initial words */
  /* ------------------------------------------------------------------- */

  wgraphP->initN=0;

  while (p) { if (strstr(p,"BEGIN_LATTICE")) break; p=strtok(NULL,"\n"); }
  while (p) { 
    if (strstr(p,"END_LATTICE")) break;  
    if (sscanf(p,"%d%d",&from,&to)!=2) { p=strtok(NULL,"\n"); continue; }
    wordN++; 
    p=strtok(NULL,"\n"); 
    if (to>vertN) vertN=to;
    if (from==1) wgraphP->initN++;
  }
  q=lattice; while (q<p) { if (*q=='\0') *q='\n'; q++; }   /* undo tokenization */
  vertN++;

  if (wordN==0 || vertN==0) { ERROR("illegal lattice\n"); return TCL_ERROR; }

  /* ------------------------------------------- */
  /* allocate the memory for storing the lattice */
  /* ------------------------------------------- */

  fromA  =   (int*)malloc(wordN*sizeof(int));
    toA  =   (int*)malloc(wordN*sizeof(int));
  wordA  = (char**)malloc(wordN*sizeof(char*));
  firstA =   (int*)malloc(vertN*sizeof(int));

  wgraphP->init = (int*)malloc(wgraphP->initN*sizeof(int));

  /* --------------------------------------------------------------------------- */
  /* now fill the fromA, toA, firstA, and worA arrays, as well as the init array */
  /* --------------------------------------------------------------------------- */

  for (vertX=0; vertX<vertN; vertX++) firstA[vertX]=(-1);

  wordX = 0; initX = 0; p = strtok(lattice,"\n");
  while (p) { if (strstr(p,"BEGIN_LATTICE")) break; p=strtok(NULL,"\n"); }
  while (p) 
  {
    if (strstr(p,"END_LATTICE")) break; 
    if (sscanf(p,"%d%d",&from,&to)!=2)  { p = strtok(NULL,"\n"); continue; }

    while (*p<=' ') p++; sscanf(p,"%d",fromA+wordX); while (*p>' ') p++;
    while (*p<=' ') p++; sscanf(p,"%d",  toA+wordX); while (*p>' ') p++;
    while (*p<=' ') p++; wordA[wordX] = p;           while (*p>' ') p++;
    *p = '\0'; 
    p = strtok(NULL,"\n");
    if (firstA[fromA[wordX]]==(-1)) firstA[fromA[wordX]]=wordX;
    if (from==1) wgraphP->init[initX++]=wordX;
    wordX++;
  }

  /* -------------------------------------------------- */
  /* allocate the memory for the word graph and fill it */
  /* -------------------------------------------------- */

  wgraphP->wordN  = wordN;

  if (wordN==0) { ERROR("empty graph\n"); goto exitWordGraphLattice; }

  wgraphP->word   =    (int*)malloc(wordN*sizeof(int));
  wgraphP->fwdTrN =    (int*)malloc(wordN*sizeof(int));
  wgraphP->fwdIdx =   (int**)malloc(wordN*sizeof(int*));
  wgraphP->fwdPen = (float**)malloc(wordN*sizeof(float*));

  for (wordX=0; wordX<wordN; wordX++) 
  { 
    wgraphP->fwdTrN[wordX] = 0;
    wgraphP->word  [wordX] = dictIndex(wgraphP->dict, wordA[wordX]);
      
    if (wgraphP->word[wordX]<0) SERROR("undefined word %s\n",wordA[wordX]);

    for ( vertX=firstA[toA[wordX]]; fromA[vertX]==toA[wordX]; vertX++) 
          wgraphP->fwdTrN[wordX]++;

    if (wgraphP->fwdTrN[wordX]>0)
    {  wgraphP->fwdIdx[wordX] =   (int*)malloc(wgraphP->fwdTrN[wordX]*sizeof(int));
       wgraphP->fwdPen[wordX] = (float*)malloc(wgraphP->fwdTrN[wordX]*sizeof(float));
    }
    else { wgraphP->fwdIdx[wordX] = NULL; wgraphP->fwdPen[wordX] = NULL; }

    for (trX=0, vertX=firstA[toA[wordX]]; fromA[vertX]==toA[wordX]; vertX++, trX++) 
    {   wgraphP->fwdIdx[wordX][trX] = vertX - wordX;
        wgraphP->fwdPen[wordX][trX] = 0.0; /* better ask lm module about that */
    }
  }

  /* -------------------------- */
  /* release the lattice memory */
  /* -------------------------- */

  exitWordGraphLattice: 

  if (fromA) free(fromA); 
  if (toA)   free(toA); 
  if (wordA) free(wordA);

  return TCL_OK;
}

static int wgraphLatticeItf ( ClientData clientData, int argc, char *argv[] )
{
  WordGraph*  wgraphP   = (WordGraph*)clientData;
  char*       lattice   =  NULL;
  int         ac        =  argc-1;

  if (itfParseArgv(argv[0], &ac, argv+1, 0,
		  "<lattice>",ARGV_STRING,NULL,&lattice,NULL,
                  "Verbmobil style lattice", NULL) != TCL_OK)
       return TCL_ERROR;

  return wgraphLattice( wgraphP, lattice);
}

/* ------------------------------------------------------------------------
    wgraphBuildContext  builds the initial context tables for each of the
                        phoneGraphs in the given wordGraph.
   ------------------------------------------------------------------------ */

int wgraphBuildContext( WordGraph*           wgraphP,
                        PhoneGraphContext*** ppleft,  int** ppleftN,
                        PhoneGraphContext*** ppright, int** pprightN)
{
  PhoneGraphContext** pleft  = (PhoneGraphContext**)malloc( wgraphP->wordN *
                                sizeof(PhoneGraphContext*));
  PhoneGraphContext** pright = (PhoneGraphContext**)malloc( wgraphP->wordN *
                                sizeof(PhoneGraphContext*));
  int* pleftN    = (int*)malloc( wgraphP->wordN * sizeof(int));
  int* prightN   = (int*)malloc( wgraphP->wordN * sizeof(int));
  int  padPhoneX = (amodelSetTree(wgraphP->amsP))->questions.padPhoneX;
  int  i, j, k, l;

  if (! wgraphP->phoneGraph) return TCL_ERROR;

  for ( i = 0; i < wgraphP->wordN; i++) {
    pleft[i]  = NULL; pleftN[i]  = 0;
    pright[i] = NULL; prightN[i] = 0;
  }

  for ( i = 0; i < wgraphP->wordN; i++) {

    PhoneGraph* pgraphP = wgraphP->phoneGraph[i];

    /* Fill left context tables of words: in order to detect exit nodes
       within the pgraph we have to go through all phones and check for
       every transition whether it points beyond the graph. For every
       end phone we find that way to it is checked for every word the current 
       word points to whether it is already in the left context list. */

    for ( j = 0; j < pgraphP->phoneN; j++) {
      for ( k = 0; k < pgraphP->fwdTrN[j]; k++) {
        if ( pgraphP->fwdIdx[j][k] + j >= pgraphP->phoneN) {
          int phone = pgraphP->phone[j];
          int tag   = pgraphP->tag[j];

          for ( l = 0; l < wgraphP->fwdTrN[i]; l++) {
            int w = i + wgraphP->fwdIdx[i][l];
            int m;

            if (  w < wgraphP->wordN) {
              for ( m = 0; m < pleftN[w]; m++) {
                if ( pleft[w][m].phone == phone && 
                     pleft[w][m].tag   == tag) break;
              }
              if ( m == pleftN[w]) {
                pleft[w] = (PhoneGraphContext*)realloc( pleft[w], 
                            (pleftN[w]+1) * sizeof(PhoneGraphContext));

                pleft[w][pleftN[w]].phone   = phone;
                pleft[w][pleftN[w]].tag     = tag;
                pleft[w][pleftN[w]].stateN  = 0;
                pleft[w][pleftN[w]].stateXA = NULL;
                pleftN[w]++;
              }
            }
          }
        }
      }
    }

    /* Fill right context tables of words: every initial state of each
       phoneGraph is considered as possible right context to another word.
       Thus, we have through each word in order to find out, if it points
       to the current word. If it does and the right context table does not
       already contain the considered phone, it will be inserted. */

    for ( j = 0; j < pgraphP->initN; j++) {
      int idx   = pgraphP->init[j];
      int phone = pgraphP->phone[idx];
      int tag   = pgraphP->tag[idx];

      for ( k = 0; k < wgraphP->wordN; k++) {
        for ( l = 0; l < wgraphP->fwdTrN[k]; l++) {
          if ( wgraphP->fwdIdx[k][l] + k == i) {
            int m;
            for ( m = 0; m < prightN[k]; m++) {
              if ( pright[k][m].phone == phone && 
                   pright[k][m].tag   == tag) break;
            }
            if ( m == prightN[k]) {
              pright[k] = (PhoneGraphContext*)realloc( pright[k], 
                          (prightN[k]+1) * sizeof(PhoneGraphContext));

              pright[k][prightN[k]].phone   = phone;
              pright[k][prightN[k]].tag     = tag;
              pright[k][prightN[k]].stateN  = 0;
              pright[k][prightN[k]].stateXA = NULL;
              prightN[k]++;
            }
          }
        }
      }
    }
  }

#if USE_PAD
  /* add dummy left context for initial words */
  for ( i = 0; i < wgraphP->initN; i++) {
    int w = wgraphP->init[i];    
    if (w < wgraphP->wordN && pleftN[w] > 0) {
      pleft[w] = (PhoneGraphContext*)realloc( pleft[w], (pleftN[w]+1) * sizeof(PhoneGraphContext));
      pleft[w][pleftN[w]].phone   = padPhoneX;
      pleft[w][pleftN[w]].tag     = 0;
      pleft[w][pleftN[w]].stateN  = 0;
      pleft[w][pleftN[w]].stateXA = NULL;
      pleftN[w]++;
    }
  }
#else
  if (padPhoneX < 0 && !padPhoneWarned) {
    padPhoneWarned = 1;
    WARN ("wgraphBuildContext: -padPhoneX < 0 => CI-system?\n");
  }
#endif

  if ( ppleft)   *ppleft   = pleft;
  if ( ppleftN)  *ppleftN  = pleftN;
  if ( ppright)  *ppright  = pright;
  if ( pprightN) *pprightN = prightN;

  return TCL_OK;
}

/* ========================================================================
    HMM
   ======================================================================== */

static ClientData hmmCreateItf(ClientData clientData, int argc, char *argv[]);
static int        hmmFreeItf(  ClientData clientData);
static ClientData hmmAccessItf(ClientData cd, char *name, TypeInfo **ti);

static HMM hmmDefault;

/* ------------------------------------------------------------------------
    Create HMM Structure:
   ------------------------------------------------------------------------ */

int hmmInit( HMM* hmm, char* name, Dictionary *dict, AModelSet *amodelSet)
{
  hmm->name            = strdup(name);
  hmm->useN            = 0;

  hmm->dict            = dict;      link( dict,      "Dictionary");
  hmm->amodels         = amodelSet; link( amodelSet, "AModelSet");

  hmm->wordGraph       = wgraphCreate( dict, amodelSet, NULL);
  hmm->phoneGraph      = pgraphCreate( amodelSet, dict, -1, 0,
				       phoneGraphDefault.modTags);
  hmm->stateGraph      = sgraphCreate( amodelSetSenoneSet( amodelSet), NULL);

  hmm->logPen          = hmmDefault.logPen;
  hmm->full            = hmmDefault.full;
  hmm->xwmodels        = hmmDefault.xwmodels;
  hmm->rcmSdp          = hmmDefault.rcmSdp;
  return TCL_OK;
}

HMM *hmmCreate (char *name, Dictionary *dict, AModelSet *amodelSet)
{
  HMM *hmmP = (HMM*)malloc(sizeof(HMM));

  if (! hmmP || hmmInit( hmmP, name, dict, amodelSet) != TCL_OK) {
    if ( hmmP) free( hmmP);
    ERROR("Failed to allocate HMM %s.\n", name); 
    return NULL;
  }
  return hmmP;
}

static ClientData hmmCreateItf (ClientData clientData, int argc, char *argv[])
{ 
  HMM*        hmmP       = NULL;
  Dictionary* dict       = NULL;
  AModelSet*  amodelSet  = NULL;
  char       *name       = NULL; 

  if ( itfParseArgv( argv[0], &argc, argv, 0,
      "<name>",        ARGV_STRING, NULL, &name, NULL, "name of the HMM",
      "<dictionary>",  ARGV_OBJECT, NULL, &dict,       "Dictionary","name of the Dictionary object",
      "<amodelset>",   ARGV_OBJECT, NULL, &amodelSet,  "AModelSet", "name of the AmodelSet object",
       NULL) != TCL_OK) return NULL;

  hmmP = hmmCreate (name, dict, amodelSet);

  return (ClientData)hmmP;
}

/* ------------------------------------------------------------------------
    Free HMM Structure
   ------------------------------------------------------------------------ */

int hmmDeinit( HMM *hmm)
{
  if (! hmm) return TCL_OK;

  if ( hmm->useN) {
    SERROR("HMM object still in use by %d other object(s).\n", hmm->useN);
    return TCL_ERROR;
  }
  if ( hmm->wordGraph)  { wgraphFree( hmm->wordGraph);  hmm->wordGraph  = NULL; }
  if ( hmm->phoneGraph) { pgraphFree( hmm->phoneGraph); hmm->phoneGraph = NULL; }
  if ( hmm->stateGraph) { sgraphFree( hmm->stateGraph); hmm->stateGraph = NULL; }
  if ( hmm->name)       { free( hmm->name);             hmm->name       = NULL; }

  if ( hmm->dict)    unlink( hmm->dict,    "Dictionary");
  if ( hmm->amodels) unlink( hmm->amodels, "AModelSet");

  return TCL_OK;
}

int hmmFree (HMM *hmm)
{
  if ( hmmDeinit(hmm)!=TCL_OK) return TCL_ERROR;
  if ( hmm) free(hmm);
  return TCL_OK;
}

static int hmmFreeItf (ClientData clientData)
{ 
  return hmmFree ((HMM*)clientData);
}

/* ------------------------------------------------------------------------
    hmmAccessItf
   ------------------------------------------------------------------------ */

static ClientData hmmAccessItf (ClientData cd, char *name, TypeInfo **ti)
{ 
  HMM* hmmP  = (HMM*)cd;

  if (*name == '.') { 
    if (name[1]=='\0') {
      if ( hmmP->wordGraph)  itfAppendElement( "wordGraph");
      if ( hmmP->phoneGraph) itfAppendElement( "phoneGraph");
      if ( hmmP->stateGraph) itfAppendElement( "stateGraph");
      if ( hmmP->dict)       itfAppendElement( "dict");
      *ti = NULL; return NULL;
    }
    else {
      name++;
      if ( hmmP->wordGraph && ! strcmp( name, "wordGraph")) {
        *ti = itfGetType("WordGraph");
        return (ClientData)hmmP->wordGraph;
      }
      else if ( hmmP->phoneGraph && ! strcmp( name, "phoneGraph")) {
        *ti = itfGetType("PhoneGraph");
        return (ClientData)hmmP->phoneGraph;
      }
      else if ( hmmP->stateGraph && ! strcmp( name, "stateGraph")) {
        *ti = itfGetType("StateGraph");
        return (ClientData)hmmP->stateGraph;
      }
      else if ( hmmP->dict && ! strcmp( name, "dict")) {
        *ti = itfGetType("Dictionary");
        return (ClientData)hmmP->dict;
      }
      return NULL;
    }
  }
  return NULL;
}

/* ------------------------------------------------------------------------
    hmmConfigureItf
   ------------------------------------------------------------------------ */

static int hmmConfigureItf (ClientData cd, char *var, char *val)
{
  HMM*  hmm = (HMM*)cd;

  if (!hmm) hmm = &(hmmDefault);

  if (!var)  {
    ITFCFG(hmmConfigureItf,cd,"logPen");
    ITFCFG(hmmConfigureItf,cd,"full");
    ITFCFG(hmmConfigureItf,cd,"xwmodels");
    ITFCFG(hmmConfigureItf,cd,"rcmSdp");
    return TCL_OK;
  }
  ITFCFG_Int(var,val,"logPen",   hmm->logPen,  0);
  ITFCFG_Int(var,val,"full",     hmm->full,    0);
  ITFCFG_Int(var,val,"xwmodels", hmm->xwmodels,0);
  ITFCFG_Int(var,val,"rcmSdp",   hmm->rcmSdp,  0);

  return TCL_OK;
}

/* ------------------------------------------------------------------------
    hmmPutsItf
   ------------------------------------------------------------------------ */

static int hmmPutsItf (ClientData cd, int argc, char *argv[])
{
  HMM* hmm = (HMM*)cd;
  return wgraphPutsItf((ClientData)(hmm->wordGraph), argc, argv);
}

/* ------------------------------------------------------------------------
    hmmMakeItf
   ------------------------------------------------------------------------ */

static int hmmMakeItf( ClientData clientData, int argc, char *argv[] )
{
  HMM* hmm = (HMM*)clientData;

  if ( wgraphMakeItf((ClientData)(hmm->wordGraph), 
                      argc, argv) != TCL_OK) return TCL_ERROR;

  if ( pgraphBuild( hmm->phoneGraph, hmm->wordGraph, hmm->logPen,
                    hmm->full, hmm->xwmodels, hmm->rcmSdp) != TCL_OK)
       return TCL_ERROR;

  return sgraphBuild( hmm->stateGraph, hmm->phoneGraph, hmm->logPen);
}

/* ------------------------------------------------------------------------
    hmmLattice
   ------------------------------------------------------------------------ */

static int hmmLattice( HMM* hmm, char* lattice)
{
  if ( wgraphLattice( hmm->wordGraph, lattice) != TCL_OK) return TCL_ERROR;

  if ( pgraphBuild( hmm->phoneGraph, hmm->wordGraph, hmm->logPen,
                    hmm->full,       hmm->xwmodels,  hmm->rcmSdp) != TCL_OK)
       return TCL_ERROR;
         
  return sgraphBuild( hmm->stateGraph, hmm->phoneGraph, hmm->logPen);
}

static int hmmLatticeItf( ClientData clientData, int argc, char *argv[] )
{
  HMM*  hmm     = (HMM*)clientData;
  char* lattice =  NULL;
  int   ac      =  argc-1;

  if (itfParseArgv(argv[0], &ac, argv+1, 0,
		  "<lattice>",ARGV_STRING,NULL,&lattice,NULL,
                  "Verbmobil style lattice", NULL) != TCL_OK)
       return TCL_ERROR;

  return hmmLattice( hmm, lattice);
}


/* -------------------------------------------------------------------------
    hmmResetModalityTagsItf, hmmSetModalityTagsItf
   ------------------------------------------------------------------------- */

int hmmResetModalityTags (WordGraph *wg) {

    int wordX;

    if ( wg->modTagsA ) {
	free (wg->modTagsA); wg->modTagsA = NULL;

	for (wordX = 0; wordX < wg->wordN; wordX++) {
	    if ( wg->phoneGraph[wordX] ) {
		wg->phoneGraph[wordX]->modTags = phoneGraphDefault.modTags;
		wg->phoneGraph[wordX]->modMask = phoneGraphDefault.modTags ^ -1;
	    }
	}
    }

    return TCL_OK;
}

/* set tagA in wordGraph to NULL */
int hmmResetModalityTagsItf (ClientData cd, int argc, char *argv[]) {

    HMM         *hmmP        = (HMM*)cd;
    WordGraph   *wg          =  NULL;

    argc--;
    if (itfParseArgv (argv[0], &argc, argv+1, 0,
		      NULL) != TCL_OK)
	return TCL_ERROR;

    wg = hmmP->wordGraph;

    return hmmResetModalityTags (wg);
}

/* set the tagA in wordGraph
   set modality tags for all words in hmm (including variants)
   all variants of a given word becomes the same tags */
int hmmSetModalityTagsItf (ClientData cd, int argc, char *argv[]) {

    HMM          *hmmP        = (HMM*)cd;
    Path         *pathP       =  NULL;
    ModalitySet  *modsP       =  NULL;
    WordGraph    *wg          =  NULL;

    int          wordN;        /* number of words in hmm */
    int          wordX;        /* index of word in hmm (wordGraph) */
    int          i, j, nodeN, startFrameX=0, endFrameX, answer;

    argc--;
    if (itfParseArgv (argv[0], &argc, argv+1, 0,
		      "<path>", ARGV_OBJECT, NULL, &pathP, "Path",
		      		"name of reference path object",
		      "<modalitySet>", ARGV_OBJECT, NULL, &modsP, "ModalitySet",
		      		"set of modalities",
		      NULL) != TCL_OK)
	return TCL_ERROR;

    /* consitency test */
    if ( (pathP->lastFrame - pathP->firstFrame) != 
	 (modsP->endFrameX - modsP->startFrameX) ) {
	ERROR ("ModalitySet '%s' and Path '%s' don't match.\n",
	       modsP->name, pathP->name);
	return TCL_ERROR;
    }

    /* pre: viterbi path is calculated and modalities are updated
       go through path */
    wg           =  hmmP->wordGraph;
    /* i don't have here the number of words in hmm, but there are usual more
       frames than words in a sentence */
    if ( wg->modTagsA ) { free (wg->modTagsA); wg->modTagsA = NULL; }
    wordN        = pathP->pitemListN; /* equal to number of frames */
    wg->modTagsA = (int*)malloc (wordN * sizeof(int));
    nodeN        =  0;
    wordX        = -1;

    /* init modTagsA */
    for ( i = 0; i < wordN; i++ ) wg->modTagsA[i] = phoneGraphDefault.modTags;

    /* there are as many items in path as frames in recording */
    for ( i = 0; i < pathP->pitemListN; i++ ) {

	/* don't know why, but there is always only one item in pathItemListA */
	for ( j = 0; j < pathP->pitemListA[i].itemN; j++ ) {

	    PathItem *pi = &(pathP->pitemListA[i].itemA[j]);

	    /* new word? */
	    if ( wordX != pi->wordX ) {

		/* process last word (exists anly if nodeN > 0) */
		if ( nodeN > 0 ) {
		    endFrameX           = i+pathP->firstFrame-1;
		    answer              = 
			modalitySetMajorityAnswer (modsP, startFrameX, endFrameX);
		    wg->modTagsA[wordX] = 
			modalitySetAnswer2CodedTags (modsP, hmmP->dict->tags,answer);
		    if ( wg->modTagsA[wordX] < 0 ) return TCL_ERROR;
		}

		nodeN++;
		wordX       = pi->wordX;
		startFrameX = i+pathP->firstFrame;
		if ( wordX >= wordN ) {
		    int n;
		    wordN *= 2;
		    wg->modTagsA = (int*)realloc (wg->modTagsA, wordN * sizeof(int));
		    for ( n = wordX; n < wordN; n++ )
			wg->modTagsA[n] = phoneGraphDefault.modTags;
		}
	    }
	}
    }

    /* process last word */
    endFrameX           = i+pathP->firstFrame-1;
    answer              = modalitySetMajorityAnswer (modsP, startFrameX, endFrameX);
    wg->modTagsA[wordX] = modalitySetAnswer2CodedTags (modsP, hmmP->dict->tags,
						       answer);
    if ( wg->modTagsA[wordX] < 0 ) return TCL_ERROR;

    return TCL_OK;
}

/* returns real tags for a given tags-object
   firstable gets answer from modality set between a given intervall */
int hmmGetRealModTags (ModalitySet *modsP, Tags *tags, int startFrameX,
		       int endFrameX) {

    Modality *modP;
    int       m, realTagX, realModTags, answer;

    answer      = modalitySetMajorityAnswer (modsP, startFrameX, endFrameX);
    realModTags = 0;

    for (m = 0; m < modsP->list.itemN; m++) {
	if ( (1<<m) == (answer & (1<<m)) ) {
	    modP         = &(modsP->list.itemA[m]);
	    realTagX     = listName2Index ((List*)&(tags->list), modP->tagName);
	    realModTags |= (1<<realTagX);
	}
    }

    return realModTags;
}

/* ========================================================================
    Type Declaration
   ======================================================================== */


static Method stateGraphMethod[] = 
{ 
  { "puts",  sgraphPutsItf,   NULL },
  { "build", sgraphBuildItf, "create StateGraph from PhoneGraph" },
  {  NULL,  NULL, NULL } 
};

TypeInfo stateGraphInfo = { 
        "StateGraph", 0, -1, stateGraphMethod, 
         sgraphCreateItf,sgraphFreeItf,
         NULL,           sgraphAccessItf, itfTypeCntlDefaultNoLink,
        "StateGraph\n" } ;

static Method phoneGraphMethod[] = 
{ 
  { "puts",  pgraphPutsItf,   NULL },
  { "make",  pgraphMakeItf,  "create PhoneGraph"}, 
  { "build", pgraphBuildItf, "create PhoneGraph from WordGraph" },
  {  NULL,  NULL, NULL } 
};

TypeInfo phoneGraphInfo = { 
        "PhoneGraph", 0, -1, phoneGraphMethod, 
         pgraphCreateItf, pgraphFreeItf,
         pgraphConfigureItf, pgraphAccessItf, itfTypeCntlDefaultNoLink,
        "PhoneGraph\n" } ;

static Method wordGraphMethod[] = 
{ 
  { "puts",    wgraphPutsItf,    NULL },
  { "make",    wgraphMakeItf,    "create WordGraph"}, 
  { "lattice", wgraphLatticeItf, "create WordGraph from lattice" },
  {  NULL,  NULL, NULL } 
};

TypeInfo wordGraphInfo = { 
        "WordGraph", 0,-1, wordGraphMethod, 
         wgraphCreateItf, wgraphFreeItf,
         NULL,            wgraphAccessItf, itfTypeCntlDefaultNoLink,
        "WordGraph\n" } ;

Method hmmMethod[] = 
{
  { "puts",    hmmPutsItf,    "displays the contents of an HMM"  },
  { "make",    hmmMakeItf,    "create full detail HMM" },
  { "lattice", hmmLatticeItf, "create full detail HMM from a lattice" },
  { "setModTags", hmmSetModalityTagsItf, "set modality Tags for hmm" },
  { "resetModTags", hmmResetModalityTagsItf, "reset modality Tags for hmm" },
  { NULL, NULL, NULL }
} ;

static TypeInfo hmmInfo = 
{ "HMM", 0, -1,     hmmMethod, 
   hmmCreateItf,    hmmFreeItf, 
   hmmConfigureItf, hmmAccessItf, NULL,
  "An 'HMM' object contains states, transitions and acoustic references" };

static int hmmInitialized = 0;

extern int AModel_Init(void), TextGraph_Init(void);

int HMM_Init (void)
{
  if (! hmmInitialized) {
    if ( AModel_Init()     != TCL_OK) return TCL_ERROR;
    if ( Dictionary_Init() != TCL_OK) return TCL_ERROR;
    if ( TextGraph_Init()  != TCL_OK) return TCL_ERROR;

    itfNewType(&stateGraphInfo);
    itfNewType(&phoneGraphInfo);
    itfNewType(&wordGraphInfo);
    itfNewType(&hmmInfo);

    hmmDefault.xwmodels = 1;
    hmmDefault.rcmSdp   = 0;
    hmmDefault.full     = 1;
    hmmDefault.logPen   = 1;
    hmmInitialized      = 1;

    phoneGraphDefault.modTags = 0;

  }
  return TCL_OK;
}
