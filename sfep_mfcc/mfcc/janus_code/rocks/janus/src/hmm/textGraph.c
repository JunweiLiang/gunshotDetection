/* ========================================================================
    JanusRTk   Janus Recognition Toolkit
               ------------------------------------------------------------
               Object: Text Graph
               ------------------------------------------------------------

    Author  :  Michael Finke
    Module  :  textGraph.h
    Date    :  $Id: textGraph.c 2390 2003-08-14 11:20:32Z fuegen $
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
    Revision 3.5  2003/08/14 11:19:57  fuegen
    Merged changes on branch jtk-01-01-15-fms (jaguar -> ibis-013)

    Revision 3.4.20.6  2003/02/05 09:25:17  soltau
    Checking for SVX_NIL

    Revision 3.4.20.5  2003/01/27 14:34:51  soltau
    support for lm-scores

    Revision 3.4.20.4  2002/11/21 17:09:53  fuegen
    windows code cleaning

    Revision 3.4.20.3  2002/11/15 13:04:51  metze
    Fixed function call in textGraphFreeItf

    Revision 3.4.20.2  2002/08/27 08:46:49  metze
    itfParseArgv now uses <name> and NULL initialization for char*

    Revision 3.4.20.1  2002/02/06 14:21:06  soltau
    *** empty log message ***

    Revision 3.3  2000/08/16 11:45:05  soltau
    Free -> TclFree

    Revision 3.2  1997/06/17 17:41:43  westphal
    *** empty log message ***

 
   ======================================================================== */

#include "common.h"
#include "itf.h"
#include "textGraph.h"
#include "error.h"
#include "hmm.h"
#include "svmap.h"
#include "lks.h"

char textGraphRCSVersion[]= 
           "@(#)1$Id: textGraph.c 2390 2003-08-14 11:20:32Z fuegen $";


/* ========================================================================
    Text Graph
   ======================================================================== */

static int textGraphPutsItf (ClientData cd, int argc, char *argv[]);

/* ------------------------------------------------------------------------
    Create TextGraph Structure:
   ------------------------------------------------------------------------ */

int textGraphInit( TextGraph* TG)
{
  TG->stateN  = 0;
  TG->stateA  = NULL;

  TG->initN   = 0;
  TG->init    = NULL;

  TG->fwdTrN  = NULL;
  TG->fwdIdx  = NULL;
  TG->fwdPen  = NULL;

  return TCL_OK;
}

TextGraph* textGraphCreate()
{
  TextGraph* tgraphP = (TextGraph*)malloc(sizeof(TextGraph));

  if (! tgraphP || textGraphInit( tgraphP) != TCL_OK) {
    if ( tgraphP) free( tgraphP);
    ERROR("Failed to allocate TextGraph.\n"); 
    return NULL;
  }
  return tgraphP;
}

static ClientData textGraphCreateItf( ClientData clientData, 
                                      int argc, char *argv[])
{ 
  char* name;

  if (itfParseArgv (argv[0], &argc, argv, 0,
      "<name>",  ARGV_STRING, NULL, &name, NULL,         "name of the TextGraph object",
       NULL) != TCL_OK) return NULL;

  return (ClientData) textGraphCreate ();
}

/* ------------------------------------------------------------------------
    Free TextGraph Structure
   ------------------------------------------------------------------------ */

int textGraphReset( TextGraph* TG)
{
  int i;
  if (! TG) return TCL_OK;

  if ( TG->stateN)  { 
    for ( i = 0; i < TG->stateN; i++) if (TG->stateA[i]) free(TG->stateA[i]);
    free( TG->stateA); TG->stateA = NULL;
  } 
  if ( TG->init)    { free( TG->init);    TG->init    = NULL; }

  if ( TG->fwdIdx)  { 
    for ( i = 0; i < TG->stateN; i++) if (TG->fwdIdx[i]) free(TG->fwdIdx[i]); 
    free( TG->fwdIdx);  TG->fwdIdx = NULL;
  }
  if ( TG->fwdPen)  { 
    for ( i = 0; i < TG->stateN; i++) if (TG->fwdPen[i]) free(TG->fwdPen[i]); 
    free( TG->fwdPen);  TG->fwdPen  = NULL;
  }
  if ( TG->fwdTrN)  { free( TG->fwdTrN);  TG->fwdTrN  = NULL; }

  TG->stateN  = 0;
  TG->initN   = 0;

  return TCL_OK;
}

int textGraphDeinit( TextGraph* TG)
{
  if (!TG) return TCL_OK;
  return textGraphReset( TG);
}

int textGraphFree( TextGraph* TG)
{
  if (!TG) return TCL_OK;
  if ( textGraphDeinit( TG) != TCL_OK) return TCL_ERROR;
  if ( TG) free( TG);
  return TCL_OK;
}

static int textGraphFreeItf (ClientData clientData)
{ 
  return textGraphFree ((TextGraph*)clientData);
}

/* ------------------------------------------------------------------------
    textGraphPutsItf
   ------------------------------------------------------------------------ */

static int textGraphPutsItf (ClientData cd, int argc, char *argv[])
{
  TextGraph* TG = (TextGraph*)cd;
  int        i, j;
  int        ac = argc - 1;

  if ( itfParseArgv( argv[0], &ac, argv+1, 0, NULL)) return TCL_ERROR;

  itfAppendResult( "STATES {");
  if ( TG->stateN)
    for ( i = 0; i < TG->stateN; i++) {
      itfAppendElement( "%s", TG->stateA[i]);
    }

  itfAppendResult( "} TRANS {");
  if ( TG->stateN) 
    for ( i = 0; i < TG->stateN; i++) {
      itfAppendResult( " {");
      for ( j = 0; j < TG->fwdTrN[i]; j++) {
        itfAppendElement( "%d %f", TG->fwdIdx[i][j], 
                                   TG->fwdPen[i][j]);
      }
      itfAppendResult( "}");
    }
 
  itfAppendResult( "} INIT {");
  if ( TG->initN)
    for ( i = 0; i < TG->initN; i++) {
      itfAppendElement( "%d", TG->init[i]);
    }

  itfAppendResult( "}");
  return TCL_OK;
}

/* ------------------------------------------------------------------------
    textGraphConcat
   ------------------------------------------------------------------------ */

int textGraphConcat( TextGraph* TG, TextGraph* NG)
{
  int stateN = TG->stateN + NG->stateN;
  int i      = 0;

  if ( NG->stateN > 0) {

  /* ------------------------------ */
  /* combine states and transitions */
  /* ------------------------------ */

  TG->stateA = (char**) realloc( TG->stateA, sizeof(char*)  * stateN);
  TG->fwdTrN = (int*)   realloc( TG->fwdTrN, sizeof(int)    * stateN);
  TG->fwdIdx = (int**)  realloc( TG->fwdIdx, sizeof(int*)   * stateN);
  TG->fwdPen = (float**)realloc( TG->fwdPen, sizeof(float*) * stateN);

  for ( i = TG->stateN; i < stateN; i++) {
    int j = i - TG->stateN;
    int k;

    TG->stateA[i] = strdup(NG->stateA[j]);
    TG->fwdTrN[i] = NG->fwdTrN[j];
    TG->fwdIdx[i] = (int*)  malloc( sizeof(int)   * TG->fwdTrN[i]);
    TG->fwdPen[i] = (float*)malloc( sizeof(float) * TG->fwdTrN[i]);
    for ( k = 0; k < NG->fwdTrN[j]; k++) {
      TG->fwdIdx[i][k] = NG->fwdIdx[j][k];
      TG->fwdPen[i][k] = NG->fwdPen[j][k];
    }
  }

  /* ----------------------------- */
  /* correct the transitions of TG */
  /* ----------------------------- */

  for ( i = 0; i < TG->stateN; i++) {
    int k;

    for ( k = 0; k < TG->fwdTrN[i]; k++) {
      if (     i + TG->fwdIdx[i][k] == TG->stateN) {
        float pen;
        int   off;
        int   j;

        TG->fwdTrN[i] +=  NG->initN-1;
        TG->fwdIdx[i]  = (int*)  realloc(TG->fwdIdx[i], sizeof(int)   * 
                                         TG->fwdTrN[i]);
        TG->fwdPen[i]  = (float*)realloc(TG->fwdPen[i], sizeof(float) * 
                                         TG->fwdTrN[i]);

        for ( j = TG->fwdTrN[i]-1; j > k; j--) {
          TG->fwdIdx[i][j] = TG->fwdIdx[i][j-NG->initN+1];
          TG->fwdPen[i][j] = TG->fwdPen[i][j-NG->initN+1];
        }

        off = TG->fwdIdx[i][k];
        pen = TG->fwdPen[i][k];

        for ( j = 0; j < NG->initN; j++, k++) {
          TG->fwdIdx[i][k] = off + NG->init[j];
          TG->fwdPen[i][k] = pen;
        }
        k--;
      }
      else if (i + TG->fwdIdx[i][k]  > TG->stateN) {
        TG->fwdIdx[i][k] += NG->stateN;
      }
    }
  }

  }

  /* --------------------------- */
  /* redefine entry points of TG */
  /* --------------------------- */

  for ( i = 0; i < TG->initN; i++) {
    if (      TG->init[i]  == TG->stateN) {
      int j;

      TG->initN +=  NG->initN-1;
      TG->init   = (int*)realloc( TG->init, sizeof(int) * TG->initN);
      for ( j = TG->initN-1; j > i; j--) {
        TG->init[j] = TG->init[j-NG->initN+1];
      }
      for ( j = 0; j < NG->initN; j++, i++) {
        TG->init[i] = NG->init[j] + TG->stateN;
      }
    }
    else if ( TG->init[i]  >  TG->stateN) {
      TG->init[i] += NG->stateN;
    }
  }

  if ( TG->initN < 1) {
    int j;

    TG->init  = (int*)malloc( sizeof(int) * NG->initN);
    TG->initN =  NG->initN;
    for (j = 0; j < TG->initN; j++) {
      TG->init[j] = NG->init[j] + TG->stateN;
    }
  }

  TG->stateN = stateN;

  return TCL_OK;
}

/* ------------------------------------------------------------------------
    textGraphAlternative
   ------------------------------------------------------------------------ */

int textGraphAlternative( TextGraph* TG, TextGraph* NG)
{
  int stateN = TG->stateN + NG->stateN;
  int initN  = TG->initN  + NG->initN;
  int i      = 0;

  /* ------------------------------ */
  /* combine states and transitions */
  /* ------------------------------ */

  TG->stateA = (char**) realloc( TG->stateA, sizeof(char*)  * stateN);
  TG->fwdTrN = (int*)   realloc( TG->fwdTrN, sizeof(int)    * stateN);
  TG->fwdIdx = (int**)  realloc( TG->fwdIdx, sizeof(int*)   * stateN);
  TG->fwdPen = (float**)realloc( TG->fwdPen, sizeof(float*) * stateN);

  for ( i = TG->stateN; i < stateN; i++) {
    int j = i - TG->stateN;
    int k;

    TG->stateA[i] = strdup(NG->stateA[j]);
    TG->fwdTrN[i] = NG->fwdTrN[j];
    TG->fwdIdx[i] = (int*)  malloc( sizeof(int)   * TG->fwdTrN[i]);
    TG->fwdPen[i] = (float*)malloc( sizeof(float) * TG->fwdTrN[i]);
    for ( k = 0; k < NG->fwdTrN[j]; k++) {
      TG->fwdIdx[i][k] = NG->fwdIdx[j][k];
      TG->fwdPen[i][k] = NG->fwdPen[j][k];
    }
  }

  /* ----------------------------- */
  /* correct the transitions of TG */
  /* ----------------------------- */

  for ( i = 0; i < TG->stateN; i++) {
    int k;
    for ( k = 0; k < TG->fwdTrN[i]; k++) {
      if (i + TG->fwdIdx[i][k] >= TG->stateN) TG->fwdIdx[i][k] += NG->stateN;
    }
  }

  /* --------------------------- */
  /* redefine entry points of TG */
  /* --------------------------- */

  for ( i = 0; i < TG->initN; i++) {
    if ( TG->init[i]  >=  TG->stateN) { TG->init[i] += NG->stateN; }
  }

  /* ---------------------- */
  /* add entry points of NG */
  /* ---------------------- */

  TG->init = (int*)realloc( TG->init, sizeof(int) * initN);

  for ( ; i < initN; i++) { 
    TG->init[i] = NG->init[i-TG->initN] + TG->stateN;
  }

  TG->stateN = stateN;
  TG->initN  = initN;

  return TCL_OK;
}

/* ------------------------------------------------------------------------
    textGraphMake
   ------------------------------------------------------------------------ */

int textGraphMake( TextGraph* TG, char* text, char* optWord, char* skipWord, SVMap* svmapP)
{
  int     argc;
  char**  argv;
  LCT  lct = svmapP ? svmapP->lksP->createLCT(svmapP->lksP,svmapP->idx.start) : LCT_NIL;

  textGraphReset( TG);

  if ( Tcl_SplitList( itf, text, &argc, &argv) == TCL_OK) {
    int i;

    /* ---------------------- */
    /* strip off extra braces */
    /* ---------------------- */

    if ( argc == 1) {
      if ( strlen(text) != strlen(argv[0])) {
        int rc = textGraphMake( TG, argv[0], optWord, skipWord, svmapP);
        Tcl_Free((char*)argv);
        return rc;
      }
    }

    /* ----------------- */
    /* recursive descent */
    /* ----------------- */

    if ( argc > 1) {
      textGraphMake( TG, argv[0], optWord, skipWord, svmapP);

      for ( i = 1; i < argc; ) {
        if (! strcmp( argv[i], "/")) {
          TextGraph             TG2;
          textGraphInit(       &TG2);
          for ( i++; i < argc && strcmp( argv[i], "/"); i++) {
            TextGraph              tgraph;
            textGraphInit(        &tgraph);
            textGraphMake(        &tgraph, argv[i], optWord, skipWord, svmapP);
            textGraphConcat(&TG2, &tgraph);
            textGraphReset(       &tgraph);
	  }
          textGraphAlternative( TG, &TG2);
          textGraphReset(           &TG2);
        } else {
          TextGraph             tgraph;
          textGraphInit(       &tgraph);
          textGraphMake(       &tgraph, argv[i], optWord, skipWord, svmapP);
          textGraphConcat( TG, &tgraph);
          textGraphReset(      &tgraph);
          i++;
        }
      }
    } else if ( argc == 1) {

      if (     skipWord && ! strcmp( argv[0], skipWord)) {
        TG->initN   =  1;
        TG->init    = (int*)malloc( sizeof(int));
        TG->init[0] =  0;
      }
      else if (optWord && strlen(optWord) && strcmp( argv[0], optWord)) {
	SVX       svx = (svmapP)         ? svocabIndex(svmapP->svocabP,argv[0]) : SVX_NIL;
	float lmscore = (svx != SVX_NIL) ? -UnCompressScore(svmapGetLMScore(svmapP,lct,svx)) : 0.0;
	if (svmapP && svx == SVX_NIL)
	  WARN("textGraphMake: couldn't find '%s' in %s\n",argv[0],svmapP->svocabP->name);

        TG->stateN      =  2;
        TG->stateA      = (char**) malloc( sizeof(char*) * 2);
        TG->stateA[0]   =  strdup(argv[0]);
        TG->stateA[1]   =  strdup(optWord);
        TG->fwdTrN      = (int*)   malloc( sizeof(int) * 2);
        TG->fwdTrN[0]   =  2;
        TG->fwdTrN[1]   =  1;
        TG->fwdIdx      = (int**)  malloc( sizeof(int*)  * 2);
        TG->fwdPen      = (float**)malloc( sizeof(float*)* 2);
        TG->fwdIdx[0]   = (int*)   malloc( sizeof(int)   * TG->fwdTrN[0]);
        TG->fwdIdx[1]   = (int*)   malloc( sizeof(int)   * TG->fwdTrN[1]);
        TG->fwdPen[0]   = (float*) malloc( sizeof(float) * TG->fwdTrN[0]);
        TG->fwdPen[1]   = (float*) malloc( sizeof(float) * TG->fwdTrN[1]);

        TG->fwdIdx[0][0] = 1;
        TG->fwdIdx[0][1] = 2;
        TG->fwdPen[0][0] = lmscore;
        TG->fwdPen[0][1] = lmscore;
        TG->fwdIdx[1][0] = 1;
        TG->fwdPen[1][0] = 0.0;

        TG->initN     =  1;
        TG->init      = (int*)malloc( sizeof(int));
        TG->init[0]   =  0;
      }
      else {
	SVX       svx = svmapP ? svocabIndex(svmapP->svocabP,argv[0]) : SVX_NIL;
	float lmscore = (svx != SVX_NIL) ? -UnCompressScore(svmapGetLMScore(svmapP,lct,svx)) : 0.0;
	if (svmapP && svx == SVX_NIL)
	  WARN("textGraphMake: couldn't find '%s' in %s\n",argv[0],svmapP->svocabP->name);


        TG->stateN      =  1;
        TG->stateA      = (char**) malloc( sizeof(char*) * 1);
        TG->stateA[0]   =  strdup(argv[0]);
        TG->fwdTrN      = (int*)   malloc( sizeof(int) * 1);
        TG->fwdTrN[0]   =  1;
        TG->fwdIdx      = (int**)  malloc( sizeof(int*));
        TG->fwdPen      = (float**)malloc( sizeof(float*));
        TG->fwdIdx[0]   = (int*)   malloc( sizeof(int)   * TG->fwdTrN[0]);
        TG->fwdPen[0]   = (float*) malloc( sizeof(float) * TG->fwdTrN[0]);

        TG->fwdIdx[0][0] = 1;
        TG->fwdPen[0][0] = lmscore;

        TG->initN   =  1;
        TG->init    = (int*)malloc( sizeof(int));
        TG->init[0] =  0;
      }
    }
    Tcl_Free((char*)argv);
    return TCL_OK;
  }
  return TCL_ERROR;
}

int textGraphMakeItf ( ClientData clientData, int argc, char *argv[] )
{
  TextGraph* TG     = (TextGraph*)clientData;
  int        ac     =  argc - 1;
  char*      text   =  NULL;
  char*      opt    =  NULL;
  char*      skip   = "@";
  SVMap*     svmapP = NULL;

  if ( itfParseArgv( argv[0], &ac, argv+1, 0,
       "<text>",    ARGV_STRING, NULL, &text, NULL, "text transcription",
       "-optWord",  ARGV_STRING, NULL, &opt,  NULL, "optional word",
       "-skipWord", ARGV_STRING, NULL, &skip, NULL, "skip word",
       "-svmap",    ARGV_OBJECT, NULL, &svmapP, "SVMap", "Search Vocabulary Mapper",
        NULL)) return TCL_ERROR;

  if ( opt && strlen(opt)) {
     TextGraph             tgraph;
     char                  optWord[100];
     sprintf( optWord, "%s / %s", opt, skip);
     textGraphMake(   TG, optWord, opt, skip, svmapP);
     textGraphInit(       &tgraph);
     textGraphMake(       &tgraph, text, opt, skip, svmapP);
     textGraphConcat( TG, &tgraph);
     textGraphReset(      &tgraph);
     return TCL_OK;
  }
  return textGraphMake( TG, text, opt, skip,svmapP); 
}

/* ========================================================================
    Type Declaration
   ======================================================================== */


static Method textGraphMethod[] = 
{ 
  { "puts",  textGraphPutsItf,   NULL },
  { "make",  textGraphMakeItf,   NULL },
  {  NULL,  NULL, NULL } 
};

TypeInfo textGraphInfo = { 
        "TextGraph", 0, -1, textGraphMethod, 
         textGraphCreateItf,textGraphFreeItf,
         NULL,           NULL, itfTypeCntlDefaultNoLink,
        "Text Graph\n" } ;

static int textGraphInitialized = 0;

int TextGraph_Init ()
{
  if (! textGraphInitialized) {

    itfNewType(&textGraphInfo);
    textGraphInitialized      = 1;
  }
  return TCL_OK;
}
