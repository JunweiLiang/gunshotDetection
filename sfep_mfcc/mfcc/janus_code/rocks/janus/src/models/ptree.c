/* ========================================================================
    JANUS      Speech- to Speech Translation
               ------------------------------------------------------------
               Object: Polyphone Tree
               ------------------------------------------------------------

    Author  :  Michael Finke
    Module  :  ptree.c
    Date    :  $Id: ptree.c 3372 2011-02-28 00:26:20Z metze $
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

    Revision 4.4.20.3  2002/09/11 17:34:34  metze
    Bugfix Hyu: Integer overflow avoided

    Revision 4.4.20.2  2002/02/28 15:46:17  soltau
    cleaned ptreeSetDeinit

    Revision 4.4.20.1  2002/02/04 14:25:02  metze
    Changed interface to wordPuts

    Revision 4.4  2000/11/14 12:35:25  janus
    Merging branch jtk-00-10-27-jmcd.

    Revision 4.3.34.2  2000/11/08 17:28:46  janus
    Finished making changes required for compilation under 'gcc -Wall'.

    Revision 4.3.34.1  2000/11/06 10:50:39  janus
    Made changes to enable compilation under -Wall.

    Revision 4.3  2000/02/08 15:31:49  soltau
    debug info

    Revision 4.2  2000/01/12 10:12:49  fuegen
    add fast clustering and verbosity

    Revision 4.1  1999/07/06 09:49:43  fuegen
    added QUESTION_VERBOSE flag

    Revision 4.0  1998/11/30 18:07:14  fuegen
    *** empty log message ***

    Revision 3.2  1997/06/17 17:41:43  westphal
    *** empty log message ***

    Revision 2.4  1996/02/26  06:11:29  finkem
    minor changes to FindQuestion

    Revision 2.3  1996/02/17  19:25:21  finkem
    findQuestion no longer considers questions that don't have
    two or more answer ptrees with model indices in it

    Revision 2.2  1996/02/11  06:19:57  finkem
    added a couple pf fundtion and made find question a lot
    faster by not really splitting trees but reading the
    model out of the tree

    Revision 2.1  1996/02/01  18:52:29  finkem
    correct model return value bug in ptreeAdd and changed the addProc stuff
    such that it is only called if the function ptreeAdd is called with model = -1.

    Revision 2.0  1996/01/31  05:07:03  finkem
    new senoneSet setup

    Revision 1.3  1995/12/17  04:34:11  finkem
    minor changes

    Revision 1.2  1995/11/14  06:05:14  finkem
    count clustering of polyphones

    Revision 1.1  1995/10/30  00:24:56  finkem
    Initial revision

 
   ======================================================================== */

#include <math.h>
#include <limits.h>
#include "common.h"
#include "ptree.h"
#include "tree.h"
#include "array.h"
#include "word.h"
#include "distrib.h"

char ptreeRcsVersion[]= "@(#)1$Id: ptree.c 3372 2011-02-28 00:26:20Z metze $";


/* ========================================================================
    Global Declarations
   ======================================================================== */

extern TypeInfo ptreeNodeInfo;
extern TypeInfo ptreeInfo;
extern TypeInfo ptreeSetInfo;

/* ========================================================================
    PTreeNode
   ======================================================================== */
/* ------------------------------------------------------------------------
    ptreeNodeInit
   ------------------------------------------------------------------------ */

int ptreeNodeInit( PTreeNode* TN, int lphnX, int ltagX, int rphnX, int rtagX)
{
  assert( TN);
  TN->phoneA[0]  =  lphnX;
  TN->tagA[0]    =  ltagX;
  TN->phoneA[1]  =  rphnX;
  TN->tagA[1]    =  rtagX;
  TN->count      =  0;
  TN->model      = -1;
  TN->nextP      =  NULL;
  TN->childP     =  NULL;
  return TCL_OK;
}

/* ------------------------------------------------------------------------
    ptreeNodeCreate
   ------------------------------------------------------------------------ */

PTreeNode* ptreeNodeCreate( int lphnX, int ltagX, int rphnX, int rtagX)
{
  PTreeNode* ptnP = (PTreeNode*)malloc(sizeof(PTreeNode));

  if (! ptnP || ptreeNodeInit( ptnP, lphnX, ltagX, rphnX, rtagX) != TCL_OK) {
    if ( ptnP) free( ptnP);
    ERROR("Failed to allocate PTreeNode object.\n");
    return NULL; 
  }
  return ptnP;
}

/* ------------------------------------------------------------------------
    ptreeNodeDeinit  deinitialize PTreeNode structure
   ------------------------------------------------------------------------ */

int ptreeNodeDeinit( PTreeNode* TN)
{
  /* TODO: DEALLOCATE CHILDP TREE */

  return TCL_OK;
}

int ptreeNodeFree( PTreeNode* TN)
{
  if ( ptreeNodeDeinit( TN) != TCL_OK) return TCL_ERROR;
  if ( TN) free( TN);
  return TCL_OK;
}

/* ------------------------------------------------------------------------
    ptreeNodeConfigureItf
   ------------------------------------------------------------------------ */

static int ptreeNodeConfigureItf(ClientData cd, char *var, char *val)
{
  PTreeNode *TN = (PTreeNode*)cd;
  if (! TN) return TCL_ERROR;

  if (var==NULL) {

    ITFCFG(ptreeNodeConfigureItf,cd,"count");
    ITFCFG(ptreeNodeConfigureItf,cd,"model");
    return TCL_OK;
  }
  ITFCFG_Float(  var,val,"count", TN->count, 0);
  ITFCFG_Int(    var,val,"model", TN->model, 0);
  ERROR("unknown option '-%s %s'.\n", var, val ? val : "?");
  return TCL_ERROR;   
}

/* ------------------------------------------------------------------------
    ptreeNodeAccessItf
   ------------------------------------------------------------------------ */

ClientData ptreeNodeAccessItf( ClientData cd, char *name, TypeInfo **ti)
{ 
  PTreeNode* ptnP = (PTreeNode*)cd;

  if (*name == '.') { 
    if (name[1]=='\0') {
       if ( ptnP->childP) itfAppendElement( "child");
       if ( ptnP->nextP)  itfAppendElement( "next");
       *ti = NULL;
    }
    else { 
      if ( ptnP->childP && ! strcmp( name+1, "child")) {
        *ti = itfGetType("PTreeNode");
        return (ClientData)ptnP->childP;
      }
      else if ( ptnP->nextP && ! strcmp( name+1, "next")) {
        *ti = itfGetType("PTreeNode");
        return (ClientData)ptnP->nextP;
      }
    }
  }
  return NULL;
}

/* ------------------------------------------------------------------------
    ptreeNodePuts
   ------------------------------------------------------------------------ */

static int ptreeNodePutsItf (ClientData clientData, int argc, char *argv[]) 
{
  PTreeNode* TN = (PTreeNode*)clientData;
  itfAppendResult("{%d %d} {%d %d} %f", TN->phoneA[0], TN->tagA[0], 
                                        TN->phoneA[1], TN->tagA[1], TN->count);
  return TCL_OK;
}

/* ------------------------------------------------------------------------
    ptreeNodeCopy
   ------------------------------------------------------------------------ */

PTreeNode* ptreeNodeCopy( PTreeNode* TN)
{
  PTreeNode* nodeP = ptreeNodeCreate( TN->phoneA[0], TN->tagA[0], 
                                      TN->phoneA[1], TN->tagA[1]);
  nodeP->count = TN->count;
  nodeP->model = TN->model;
  return nodeP;
}

/* ========================================================================
    PTree
   ======================================================================== */

static PTree ptreeDefaults;

/* ------------------------------------------------------------------------
    ptreeInit    initialize a polyphonic tree
   ------------------------------------------------------------------------ */

int ptreeInit( PTree* TN, char* name)
{
  assert( TN);
  TN->name         = (name) ? strdup(name) : NULL;
  TN->count        =  0.0;
  TN->maxContext   =  ptreeDefaults.maxContext;
  TN->phones       =  NULL;
  TN->tags         =  NULL;
  TN->mdsP.cd      =  NULL;
  TN->addProc      = (ptreeDefaults.addProc) ? strdup(ptreeDefaults.addProc) :
                                               NULL;
  TN->childP       =  NULL;

  return TCL_OK;
}

/* ------------------------------------------------------------------------
    ptreeCreate     create a polyphonic tree object
   ------------------------------------------------------------------------ */

PTree* ptreeCreate( char* name, Phones* phones, Tags* tags, 
                                ModelSetPtr*    mdsP)
{
  PTree* ptrP = (PTree*)malloc(sizeof(PTree));

  assert( phones && tags && mdsP);

  if (! ptrP || ptreeInit( ptrP, name) != TCL_OK) {
    if ( ptrP) free( ptrP);
    ERROR("Failed to allocate PTree object.\n");
    return NULL; 
  }
  ptrP->phones = phones;
  ptrP->tags   = tags;

  /* initialization of the modelSet array */

  ptrP->mdsP.cd        = mdsP->cd;
  ptrP->mdsP.modelSetP = mdsP->modelSetP;

  return ptrP;
}

static ClientData ptreeCreateItf( ClientData clientData, 
                                  int argc, char *argv[]) 
{
  Phones*     phones    = NULL;
  Tags*       tags      = NULL;
  int         ac        = argc - 1;
  char*       addProc   = NULL;
  PTree*      ptP       = NULL;
  ModelSetPtr mdsP;

  modelSetPtrInit( &mdsP);

  if ( itfParseArgv( argv[0], &ac, argv+1, 0,
      "<phones>",   ARGV_OBJECT,NULL,&phones, "Phones", "set of phones",
      "<tags>",     ARGV_OBJECT,NULL,&tags,   "Tags",   "set of tags",
      "<modelSet>", ARGV_CUSTOM,getModelSetPtr,&mdsP,NULL,"set of models",
      "-addProc",   ARGV_STRING,NULL,&addProc, NULL,    "TCL add model proc",
       NULL) != TCL_OK) {
    modelSetPtrDeinit(&mdsP);
    return NULL;
  }
  if ( (ptP = ptreeCreate(argv[0],phones,tags,&mdsP)) ) {
    if ( addProc) {
      if ( ptP->addProc) free( ptP->addProc);
      ptP->addProc = strdup(addProc);
    }
  }
  return (ClientData)ptP;
}

/* ------------------------------------------------------------------------
    ptreeDeinit  deinitialize PTree structure
   ------------------------------------------------------------------------ */

int ptreeDeallocRec( PTreeNode* TN)
{
  PTreeNode* childP;
  for ( childP = TN->childP; childP; ) {
    PTreeNode* nextP = childP->nextP;
    ptreeDeallocRec( childP);
    childP = nextP;
  }
  ptreeNodeFree( TN);
  return TCL_OK;
}

int ptreeDealloc( PTree* TN)
{
  PTreeNode* trnP = TN->childP;  
  while ( trnP) {
    PTreeNode* nextP = trnP->nextP;
    ptreeDeallocRec( trnP);
    trnP = nextP;
  }
  TN->childP = NULL;
  return TCL_OK;
}

int ptreeDeinit( PTree* TN)
{
  if ( TN->childP)  { ptreeDealloc( TN); TN->childP  = NULL; } 
  if ( TN->name)    { free( TN->name);   TN->name    = NULL; }
  if ( TN->addProc) { free( TN->addProc);TN->addProc = NULL; }

  return modelSetPtrDeinit( &(TN->mdsP));
}

/* ------------------------------------------------------------------------
    ptreeFree
   ------------------------------------------------------------------------ */

int ptreeFree( PTree* TN)
{
  if ( ptreeDeinit( TN) != TCL_OK) return TCL_ERROR;
  if ( TN) free( TN);
  return TCL_OK;
}

static int ptreeFreeItf (ClientData clientData)
{
  PTree* ptree = (PTree*)clientData;
  return ptreeFree( ptree);
}

/* ------------------------------------------------------------------------
    ptreeConfigureItf
   ------------------------------------------------------------------------ */

static int ptreeConfigureItf(ClientData cd, char *var, char *val)
{
  PTree *TN = (PTree*)cd;
  if (! TN) TN = &ptreeDefaults;

  if (var==NULL) {
    ITFCFG(ptreeConfigureItf,cd,"name");
    ITFCFG(ptreeConfigureItf,cd,"count");
    ITFCFG(ptreeConfigureItf,cd,"maxContext");
    ITFCFG(ptreeConfigureItf,cd,"addProc");
    return TCL_OK;
  }
  ITFCFG_CharPtr( var,val,"name",         TN->name,         1);
  ITFCFG_Float(   var,val,"count",        TN->count,        1);
  ITFCFG_Int(     var,val,"maxContext",   TN->maxContext,   0);
  ITFCFG_CharPtr( var,val,"addProc",      TN->addProc,      0);
  ERROR("unknown option '-%s %s'.\n", var, val ? val : "?");
  return TCL_ERROR;   
}

/* ------------------------------------------------------------------------
    ptreeAccessItf
   ------------------------------------------------------------------------ */

ClientData ptreeAccessItf( ClientData cd, char *name, TypeInfo **ti)
{ 
  PTree* ptrP = (PTree*)cd;

  if (*name == '.') { 
    if (name[1]=='\0') {
       if ( ptrP->childP) itfAppendElement( "child");
       itfAppendElement( "modelSet");
       *ti = NULL;
    }
    else { 
      if ( ptrP->childP && ! strcmp( name+1, "child")) {
        *ti = itfGetType("PTreeNode");
        return (ClientData)ptrP->childP;
      }
      else if (! strcmp( name+1, "modelSet")) {
        *ti = ptrP->mdsP.modelSetP->tiP;
        return ptrP->mdsP.cd;
      }
    }
  }
  return NULL;
}

/* ------------------------------------------------------------------------
    ptreePuts
   ------------------------------------------------------------------------ */

static int ptreePutsItf (ClientData clientData, int argc, char *argv[]) 
{
  PTree* TN       = (PTree*)clientData;
  int    ac       =  argc-1;
  float  minCount =  1;

  if ( itfParseArgv( argv[0], &ac, argv+1, 0,
      "-minCount", ARGV_FLOAT, NULL, &minCount, NULL, "minimum count",
       NULL) != TCL_OK) {
    return TCL_ERROR;
  }
  ptreeList( TN, minCount);
  return TCL_OK;
}

/* ------------------------------------------------------------------------
    ptreeAdd    add another polyphone to the tree
   ------------------------------------------------------------------------ */

PTreeNode* ptreeAdd( PTree* ptrP, Word* wP,
                     int    leftContext, int  rightContext,
                     float  count,       int  model, int* modelP)
{
  PTreeNode* pptnP      =  ptrP->childP;
  PTreeNode* ptnP       =  pptnP;
  int*       phoneA     =  wP->phoneA;
  int*       tagA       =  wP->tagA;
  int        mid        = -leftContext;

  if ( leftContext > 0 || rightContext < 0) {
    ERROR("Context specification must include mid-phone.\n");
    return NULL;
  }

  ptrP->count += count;

  /* fill out the mid-phone node */
  for ( ; ptnP && (ptnP->phoneA[0] <  phoneA[mid] || 
                   ptnP->tagA[0]   <  tagA[mid]); 
          pptnP = ptnP, ptnP = ptnP->nextP);

  if  ( ! ptnP || (ptnP->phoneA[0] != phoneA[mid] || 
                   ptnP->tagA[0]   != tagA[mid])) {
    PTreeNode* q = ptnP;
    if ( ptnP == pptnP)
         ptnP =  ptrP->childP = ptreeNodeCreate(phoneA[mid],tagA[mid],-1,0);
    else ptnP =  pptnP->nextP = ptreeNodeCreate(phoneA[mid],tagA[mid],-1,0);
    ptnP->nextP = q;
  }
  ptnP->count += count;

  if ( modelP && ptnP->model > -1) *modelP = ptnP->model;

  /* fill out the left/right-phone nodes */

  if ((leftContext < 0 || rightContext > 0) && ptrP->maxContext) {
    int        n    = (-leftContext > rightContext) ? 
                       -leftContext : rightContext;
    int        i;

    n = (ptrP->maxContext < 0) ? n : ptrP->maxContext;

    for ( i = 0; i < n; i++) {
      PTreeNode* parentP = ptnP;
      int lphn = (leftContext  < -i)         ? phoneA[mid-i-1] : -1;
      int rphn = (rightContext >  i)         ? phoneA[mid+i+1] : -1;
      int ltag = (leftContext  < -i && tagA) ? tagA[  mid-i-1] :  0;
      int rtag = (rightContext >  i && tagA) ? tagA[  mid+i+1] :  0;

      pptnP = ptnP = ptnP->childP;
 
      for ( ; ptnP && (ptnP->phoneA[0] <  lphn || ptnP->tagA[0] <  ltag ||
                       ptnP->phoneA[1] <  rphn || ptnP->tagA[1] <  rtag) ;
              pptnP = ptnP, ptnP = ptnP->nextP);

      if  ( ! ptnP || (ptnP->phoneA[0] != lphn || ptnP->tagA[0] != ltag ||
                       ptnP->phoneA[1] != rphn || ptnP->tagA[1] != rtag)) {
        PTreeNode* q = ptnP;
        if ( ptnP == pptnP)
             ptnP =  parentP->childP = ptreeNodeCreate(lphn,ltag,rphn,rtag);
        else ptnP =  pptnP->nextP    = ptreeNodeCreate(lphn,ltag,rphn,rtag);

        ptnP->nextP = q;
      }
      ptnP->count += count;
      if ( modelP && ptnP->model > -1) *modelP = ptnP->model;
    }
  }

  /* if this polyphone is new and there is a Tcl add procedure defined
     to be used to find the model index call this procedure and use its
     return code as model name/index */ 

  if ( model < 0 && ptnP->model < 0 && ptrP->addProc) {
    DString dstr;
    Word    word;
    int     left  = leftContext;
    int     right = rightContext;

    wordInit( &word, ptrP->phones, ptrP->tags);
    word.phoneA  = phoneA;
    word.tagA    = tagA;
    word.itemN   = rightContext-leftContext+1;

    if ( ptrP->maxContext > -1) {
      left  = (-leftContext  > ptrP->maxContext) ? -ptrP->maxContext : leftContext;
      right = ( rightContext > ptrP->maxContext) ?  ptrP->maxContext : rightContext;

      word.phoneA  = phoneA + ((left  > leftContext)  ? left-leftContext : 0);
      word.tagA    = tagA   + ((left  > leftContext)  ? left-leftContext : 0);
      word.itemN   = right-left+1;
    }

    dstringInit(  &dstr);
    dstringAppend(&dstr,"%s %s {%s} {%s} %d %d %f",
                   ptrP->addProc, ptrP->name, 
                   wordModelName (&word,     left, right),
                   wordPuts      (&word, 1), left, right, ptnP->count);

    if ( ptnP->model >= 0) {
      if ( ptrP->mdsP.modelSetP->nameFct) 
           dstringAppend( &dstr, " -model %s",ptrP->mdsP.modelSetP->nameFct(
                                              ptrP->mdsP.cd, ptnP->model));
      else dstringAppend( &dstr, " -model %d",ptnP->model);
    }
    if ( Tcl_Eval( itf, dstringValue(&dstr)) != TCL_OK) {
      WARN("Tcl Error calling PTree addProc '%s'.\n", dstringValue(&dstr));
    }
    else {
      //if (! itf->result || ! strcmp(itf->result,"-")) model = -1;
      if (!strcmp (Tcl_GetStringResult (itf), "-"))
	model = -1;
      else {
        if ( ptrP->mdsP.modelSetP->indexFct) {
             model = ptrP->mdsP.modelSetP->indexFct( 
		     ptrP->mdsP.cd, (char*) Tcl_GetStringResult (itf));
             if ( model < 0) {
               char* name = strdup(Tcl_GetStringResult (itf));
               WARN("Model %s is not defined.\n", name);
               free( name);
             }
	}
        else model = atoi(Tcl_GetStringResult (itf));
      }
    }
    word.phoneA  = NULL;  /* reset to NULL because they would otherwise */
    word.tagA    = NULL;  /* be freed by the wordDeinit function        */
    wordDeinit(&word);
    dstringFree(&dstr);
  }
  if ( model > -1) { ptnP->model = model; if (modelP) *modelP = model; }

  return ptnP;
}

int ptreeAddItf( ClientData clientData, int argc, char *argv[])
{
  PTree*     tree   = (PTree*)clientData;
  int        left   = -1, right = 0;
  int        ac     =  argc - 1;
  float      count  =  1;
  int        index;
  char*      model  =  NULL;
  PTreeNode* ptnP   =  NULL;
  Word       word;
  int        modelX = -1;

  wordInit( &word, tree->phones, tree->tags);

  if ( itfParseArgv( argv[0], &ac, argv+1, 0,
      "<tagged phones>",ARGV_CUSTOM, getWord, &word,NULL,
      "list of tagged phones",
      "<leftContext>",  ARGV_INT,    NULL, &left,  NULL, "left  context",
      "<rightContext>", ARGV_INT,    NULL, &right, NULL, "right context",
      "-count",         ARGV_FLOAT,  NULL, &count, NULL, "count",
      "-model",         ARGV_STRING, NULL, &model, NULL, "model",
       NULL) != TCL_OK) {
    wordDeinit( &word);
    return TCL_ERROR;
  }

  if (! model || ! strcmp(model,"-")) index = -1;
  else {
    if ( tree->mdsP.modelSetP->indexFct) 
         index = tree->mdsP.modelSetP->indexFct( tree->mdsP.cd, model);
    else index = atoi(model);
  }

  ptnP = ptreeAdd( tree, &word, left, right, count, index, &modelX);
  wordDeinit( &word);
  return (ptnP) ? TCL_OK : TCL_ERROR;
}

/* ------------------------------------------------------------------------
    ptreeGet    find a polyphone in the tree
   ------------------------------------------------------------------------ */

PTreeNode* ptreeGet( PTree *ptrP, Word* wP,
                     int  leftContext, int  rightContext, int* modelP)
{
  PTreeNode* ptnP   =  NULL;
  int*       phoneA =  wP->phoneA;
  int*       tagA   =  wP->tagA;
  int        mid    = -leftContext;

  if ( wordCheck( wP, ptrP->phones, ptrP->tags) != TCL_OK) return NULL;
  if ( leftContext > 0 || rightContext < 0) {
    ERROR("Context specification must include mid-phone.\n");
    return NULL;
  }

  /* fill out the mid-phone node */

  if (! (ptnP =  ptrP->childP)) return NULL;
  else {
    PTreeNode* pptnP = NULL;
 
    for ( ; ptnP && (ptnP->phoneA[0] <  phoneA[mid] || 
                     ptnP->tagA[0]   <  tagA[mid]); 
            pptnP = ptnP, ptnP = ptnP->nextP);
    if  ( ! ptnP || (ptnP->phoneA[0] != phoneA[mid] || 
                     ptnP->tagA[0]   != tagA[mid])) return NULL;
  }
  if ( modelP && ptnP->model > -1) *modelP = ptnP->model;

  /* fill out the left/right-phone nodes */

  if ((leftContext < 0 || rightContext > 0) && ptrP->maxContext) {
    int        n    = (-leftContext > rightContext) ? 
                       -leftContext : rightContext;
    int        i;

    n = (ptrP->maxContext < 0) ? n : ptrP->maxContext;

    for ( i = 0; i < n; i++) {
      int lphn = (leftContext  < -i)         ? phoneA[mid-i-1] : -1;
      int rphn = (rightContext >  i)         ? phoneA[mid+i+1] : -1;
      int ltag = (leftContext  < -i && tagA) ? tagA[  mid-i-1] :  0;
      int rtag = (rightContext >  i && tagA) ? tagA[  mid+i+1] :  0;

      if (! ptnP->childP) return NULL;
      else {
        PTreeNode* pptnP = ptnP = ptnP->childP;
 
        for ( ; ptnP && (ptnP->phoneA[0] <  lphn || ptnP->tagA[0] <  ltag ||
                         ptnP->phoneA[1] <  rphn || ptnP->tagA[1] <  rtag) ;
                pptnP = ptnP, ptnP = ptnP->nextP);
        if  ( ! ptnP || (ptnP->phoneA[0] != lphn || ptnP->tagA[0] != ltag ||
                         ptnP->phoneA[1] != rphn || ptnP->tagA[1] != rtag))
                return NULL;
      }
      if ( modelP && ptnP->model > -1) *modelP = ptnP->model;
    }
  }
  return ptnP;
}

int ptreeGetItf( ClientData clientData, int argc, char *argv[])
{
  PTree*     tree   = (PTree*)clientData;
  int        left   = -1, right = 0;
  int        ac     =  argc - 1;
  PTreeNode* ptnP   =  NULL;
  Word       word;
  int        modelX = -1;

  wordInit( &word, tree->phones, tree->tags);

  if ( itfParseArgv( argv[0], &ac, argv+1, 0,
      "<tagged phones>",ARGV_CUSTOM, getWord, &word,NULL,
      "list of tagged phones",
      "<leftContext>",  ARGV_INT, NULL, &left,  NULL, "left  context",
      "<rightContext>", ARGV_INT, NULL, &right, NULL, "right context",
       NULL) != TCL_OK) {
    wordDeinit( &word);
    return TCL_ERROR;
  }
  
  if ( (ptnP = ptreeGet( tree, &word, left, right, &modelX)) ) {

    itfAppendResult(" %f %d %s", ptnP->count, modelX,
                    tree->mdsP.modelSetP->nameFct( tree->mdsP.cd, modelX));
  }
  wordDeinit( &word);
  return (ptnP) ? TCL_OK : TCL_ERROR;
}

/* ------------------------------------------------------------------------
    ptreeDFS    ptree Depth First Search. This function is used by several
                other PTree function to traverse the tree in DFS fashion,
                fill out a word object for each model in the tree and call
                finally the dfsFct (as defined by the ptree function)
   ------------------------------------------------------------------------ */

static PTreeNode*  ptnPA[100];

typedef float (PTreeDFSFct)( PTree* ptrP, PTreeNode* ptnP, 
                             float count, int model,
                             ClientData CD,
                             Word* wP, int left, int right);

float ptreeDFSRec( PTree* ptrP, PTreeNode* ptnP, int i, float minCount,
                   PTreeDFSFct* dfsFct, ClientData  CD)
{
  PTreeNode* childP =  NULL;
  float      count  =  0;
  int        model  = -1;

  for ( childP = ptnP->childP; childP;) {
    ptnPA[i] = childP;
    count   += ptreeDFSRec( ptrP, childP, i+1, minCount, dfsFct, CD);
    childP   = childP->nextP;
  }
  model = ptnPA[i-1]->model;

  if ( ptnP->count-count >= minCount || model >= 0) {
    int  lc   = 0;
    int  rc   = 0;
    int  k, l;
    Word word;

    wordInit( &word, ptrP->phones, ptrP->tags);

    for ( k = 0; k < i; k++) if ( ptnPA[i-k-1]->phoneA[0] >= 0) lc--;
    for ( k = 1; k < i && ptnPA[k]->phoneA[1] >= 0; k++)        rc++;

    lc++;
    word.itemN   =  rc - lc + 1;
    word.phoneA  = (int*)malloc(word.itemN * sizeof(int));
    word.tagA    = (int*)malloc(word.itemN * sizeof(int));

    for ( k = 0, l = 0; k < i; k++)
      if ( ptnPA[i-k-1]->phoneA[0] >= 0) {
        word.phoneA[l] = ptnPA[i-k-1]->phoneA[0];
        word.tagA  [l] = ptnPA[i-k-1]->tagA[0];
        l++;
      }
    for ( k = 1; k < i && ptnPA[k]->phoneA[1] >= 0; k++) {
      word.phoneA[l] = ptnPA[k]->phoneA[1];
      word.tagA  [l] = ptnPA[k]->tagA[1];
      l++;
    }

    count = dfsFct( ptrP, ptnP, ptnP->count-count, model, CD, &word, lc, rc);

    wordDeinit( &word);
    return ptnP->count;
  }
  return count;
}

float ptreeDFS( PTree*       ptrP,   float       minCount, 
                PTreeDFSFct* dfsFct, ClientData  CD)
{
  PTreeNode* ptnP  = ptrP->childP;
  float      count = 0;

  while ( ptnP) {
    ptnPA[0] = ptnP;
    count   += ptreeDFSRec( ptrP, ptnP, 1, minCount, dfsFct, CD);
    ptnP     = ptnP->nextP;
  }

  return count;
}

/* ------------------------------------------------------------------------
    ptreeWrite   writes a single ptree to a file using DFS
   ------------------------------------------------------------------------ */

float ptreeWriteFct( PTree* ptrP, PTreeNode* ptnP, float count, int model,
                     ClientData CD, Word* wP, int lc, int rc)
{
  FILE* fp = (FILE*)CD;

  fprintf( fp, "%s {%s} %d %d -count %f", ptrP->name , wordPuts( wP, 1),
           lc, rc, count);

  if ( model > -1) { 
    if ( ptrP->mdsP.modelSetP->nameFct) 
         fprintf( fp, " -model %s",ptrP->mdsP.modelSetP->nameFct(
                                   ptrP->mdsP.cd, model));
    else fprintf( fp, " -model %d",model);
  }
  fprintf( fp, "\n");
  return count;
}

float ptreeWrite( PTree* ptrP, float minCount, FILE* fp)
{
  return ptreeDFS( ptrP, minCount, ptreeWriteFct, (ClientData)fp);
}

/* ------------------------------------------------------------------------
    ptreeList   lists all polyphones within a tree using DFS
   ------------------------------------------------------------------------ */

float ptreeListFct( PTree* ptrP, PTreeNode* ptnP, float count, int model,
                    ClientData CD, Word* wP, int lc, int rc)
{
  itfAppendResult("{{%s} %d %d %f", wordPuts( wP, 1), lc, rc, count);

  if ( model > -1) { 
    if ( ptrP->mdsP.modelSetP->nameFct) 
         itfAppendResult(" %s",ptrP->mdsP.modelSetP->nameFct(
                               ptrP->mdsP.cd, model));
    else itfAppendResult(" %d",model);
  }
  itfAppendResult("}\n");
  return count;
}

float ptreeList( PTree* ptrP, float minCount)
{
  return ptreeDFS( ptrP, minCount, ptreeListFct, (ClientData)0);
}

/* ------------------------------------------------------------------------
    ptreeModels   fills a model array with all models in the polyphonic
                  tree (including the counts)
   ------------------------------------------------------------------------ */

float ptreeModelsFct( PTree* ptrP, PTreeNode* ptnP, float count, int model,
                      ClientData CD, Word* wP, int lc, int rc)
{
  ModelArray*    mdaP = (ModelArray*)CD;

  if ( mdaP->mdsP.cd == ptrP->mdsP.cd && model > -1)
       modelArrayAdd( mdaP, model, count);
  return count;
}

float ptreeModels( PTree*  ptrP, float minCount, ModelArray* mdaP)
{
  float count;

  count =  ptreeDFS( ptrP, minCount, ptreeModelsFct, (ClientData)mdaP);

  return count;
}

int ptreeModelsItf( ClientData clientData, int argc, char *argv[])
{
  PTree*       tree     = (PTree*)clientData;
  ModelArray*  mdaP     =  NULL;
  int          ac       =  argc - 1;
  float        minCount =  0;

  if ( itfParseArgv( argv[0], &ac, argv+1, 0,
      "<modelArray>", ARGV_OBJECT, NULL, &mdaP, "ModelArray", "model array", 
      "-minCount",    ARGV_FLOAT,  NULL, &minCount, NULL, "minimum count",
       NULL) != TCL_OK) return TCL_ERROR;

  ptreeModels( tree, minCount, mdaP);
  return TCL_OK;
}

/* ------------------------------------------------------------------------
    ptreeSplit   split a polyphonic tree according to a given question
                 into three different trees: "Yes", "No", and 
                 "Don't Know". This is done by asking the question for
                 each path in the tree and adding the path to the 
                 tree corresponding to the answer.
   ------------------------------------------------------------------------ */

typedef struct {

  Question* quP;

  PTree*    yesP;
  PTree*    noP;
  PTree*    unkP;

} PTreeSplit;

float ptreeSplitFct( PTree* ptrP, PTreeNode* ptnP, float count, int model,
                     ClientData CD, Word* wP, int lc, int rc)
{
  PTreeSplit* ptsP = (PTreeSplit*)CD;
  int         l    =  questionAnswer( ptsP->quP, wP, lc, rc);

  if ( count > 0.0 || model > -1) {
    if ( l ==  1) {
      if (! ptsP->yesP) {
        ptsP->yesP = ptreeCreate(ptrP->name,ptrP->phones,ptrP->tags,&(ptrP->mdsP));
      }
      ptreeAdd( ptsP->yesP, wP, lc, rc, count, model,NULL);
    } 
    else if ( l == -1) {
      if (! ptsP->noP) {
        ptsP->noP = ptreeCreate(ptrP->name,ptrP->phones,ptrP->tags,&(ptrP->mdsP));
      }
      ptreeAdd( ptsP->noP, wP, lc, rc, count, model,NULL);
    }
    else if ( l == 0) {
      if (! ptsP->unkP) {
        ptsP->unkP = ptreeCreate(ptrP->name,ptrP->phones,ptrP->tags,&(ptrP->mdsP));
      }
      ptreeAdd( ptsP->unkP, wP, lc, rc, count, model,NULL);
    }
  }
  return count;
}

float ptreeSplit( PTree*  ptrP, float minCount, Question*   quP,
                  PTree** noP,  PTree** yesP,   PTree** unkP)
{
  PTreeSplit pts;
  float      count;

  pts.quP  =  quP;
  pts.yesP = *yesP;
  pts.noP  = *noP;
  pts.unkP = *unkP;

  count    =  ptreeDFS( ptrP, minCount, ptreeSplitFct, (ClientData)&pts);

  *yesP    =  pts.yesP;
  *noP     =  pts.noP;
  *unkP    =  pts.unkP;

  return count;
}

int ptreeSplitItf( ClientData clientData, int argc, char *argv[])
{
  PTree*       tree     = (PTree*)clientData;
  int          ac       =  argc - 1;
  char*        questS   =  NULL;
  QuestionSet* qsetP    =  NULL;
  float        minCount =  0;

  if ( itfParseArgv( argv[0], &ac, argv+1, 0,
      "<questionSet>",ARGV_OBJECT, NULL, &qsetP, "QuestionSet", "question set", 
      "<question>",   ARGV_STRING, NULL, &questS,   NULL, "question", 
      "-minCount",    ARGV_FLOAT,  NULL, &minCount, NULL, "minimum count",
       NULL) != TCL_OK) return TCL_ERROR;
  else {
    PTree     *yesP = NULL, *noP = NULL, *unkP = NULL;
    Question   quest;

    questionInit(   &quest, questS);

    if ( questionDefine( &quest, questS, qsetP) != TCL_OK) {
         questionDeinit( &quest);
         return TCL_ERROR;
    }
    ptreeSplit( tree, minCount, &quest, &noP, &yesP, &unkP);

    questionDeinit( &quest);

    itfAppendResult("{\n");
    if ( noP)  { ptreeList( noP,  1); ptreeFree( noP);  }
    itfAppendResult("} {\n");
    if ( yesP) { ptreeList( yesP, 1); ptreeFree( yesP); }
    itfAppendResult("} {\n");
    if ( unkP) { ptreeList( unkP, 1); ptreeFree( unkP); }
    itfAppendResult("}");
  }
  return TCL_OK;
}

/* ------------------------------------------------------------------------
    ptreeMerge   merges the current tree into another one
   ------------------------------------------------------------------------ */

float ptreeMergeFct( PTree* ptrP, PTreeNode* ptnP, float count, int model,
                     ClientData CD, Word* wP, int lc, int rc)
{
  ptreeAdd( (PTree*)CD, wP, lc, rc, count, model,NULL);

  return count;
}

float ptreeMerge( PTree*  ptrP, float minCount, PTree*  ptrP2) 
{
  return ptreeDFS( ptrP, minCount, ptreeMergeFct, (ClientData)ptrP2);
}

/* ------------------------------------------------------------------------
    ptreeFindQuestion
   ------------------------------------------------------------------------ */

typedef struct {

    Question   *quP;

    ModelArray  yesA;
    ModelArray  noA;
    ModelArray  unkA;

} PTreeFindQuestion;


float ptreeFindQuestionFct( PTree* ptrP, PTreeNode* ptnP, float count, 
                            int model,
                            ClientData CD, Word* wP, int lc, int rc)
{
  PTreeFindQuestion* pfqP = (PTreeFindQuestion*)CD;
  int                l    =  questionAnswer( pfqP->quP, wP, lc, rc);

  if ( model > -1) {
    if (      l ==  1) { modelArrayAdd(&(pfqP->yesA), model, count); } 
    else if ( l == -1) { modelArrayAdd(&(pfqP->noA),  model, count); }
    else if ( l ==  0) { modelArrayAdd(&(pfqP->unkA), model, count); }
  }
  return count;
}

int ptreeFindQuestion( PTree* ptrP, QuestionSet* QS, float  minCount, 
                                                     float* score)
{
  ModelSetDistFct*  distFct =  ptrP->mdsP.modelSetP->distFct;
  int               N       =  QS->list.itemN, i;
  double            maxDist = -9e19;
  int               idx     = -1;
  PTreeFindQuestion pfq;

#ifdef QUESTION_VERBOSE
  float normDistMean =  0.0,     modDistMean =  0.0;
  float normDistMin  =  FLT_MAX, modDistMin  =  FLT_MAX;
  float normDistMax  = -FLT_MAX, modDistMax  = -FLT_MAX;
  int   normIdx      =  0;
  int   modIdx       = -1;
  int   rank         =  0;
  int   modN         =  0;
  int   normN        =  0;

  /* all modality questions are contextindependent (0=)
     all modality questions are at the begin of the questionSet
     the first normal question is (0=WB)
     ==> normIdx is the index of (0=WB) in QS */
  normIdx = questionSetIndex (QS, "0=WB");
#endif

  if (! distFct) {
    ERROR("Cannot find question because modelSet has no distance function.\n");
    return -1;
  }

  modelArrayInit( &(pfq.yesA), &(ptrP->mdsP));
  modelArrayInit( &(pfq.noA),  &(ptrP->mdsP));
  modelArrayInit( &(pfq.unkA), &(ptrP->mdsP));

  /* for all available questions */
  for ( i = 0; i < N; i++) {
    float     dist = 0.0;

    pfq.quP = QS->list.itemA + i;

    modelArrayClear( &(pfq.yesA));
    modelArrayClear( &(pfq.noA));
    modelArrayClear( &(pfq.unkA));

    /* divide tree in three disjunctive modelArrays */
    ptreeDFS( ptrP, minCount, ptreeFindQuestionFct, (ClientData)&pfq);
    
    /*
      INFO("Question: %s  yesN= %d  noN= %d  unk=%d  minCnt= %d\n",\
      pfq.quP->name,pfq.yesA.modelN,pfq.noA.modelN,pfq.unkA.modelN,minCount);
    */

    /* find out best distance */
    if ((pfq.noA.modelN  * pfq.yesA.modelN > 0 || 
         pfq.yesA.modelN * pfq.unkA.modelN > 0 || 
         pfq.noA.modelN  * pfq.unkA.modelN > 0) &&
         distFct(ptrP->mdsP.cd,&(pfq.noA),&(pfq.yesA),
                                          &(pfq.unkA), &dist) == TCL_OK) {

#ifdef QUESTION_VERBOSE
	if ( i < normIdx ) {
	    modDistMean += dist;
	    modN++;
	    if ( dist > modDistMax ) { modDistMax = dist; modIdx = i; }
	    if ( dist < modDistMin )   modDistMin = dist;
	} else {
	    normDistMean += dist;
	    normN++;
	    if ( dist > normDistMax ) normDistMax = dist;
	    if ( dist < normDistMin ) normDistMin = dist;
	    if ( dist > modDistMax )  rank++;
	}
	/*
	  printf (" -> {%s} [%i, %i, %i] -> %e\n", pfq.quP->name, pfq.noA.modelN,
	  pfq.yesA.modelN, pfq.unkA.modelN, dist);
	*/
#endif

      if ( dist > maxDist) { maxDist = dist; idx = i; }
    } else {
	/*
	  #ifdef QUESTION_VERBOSE
	  printf (" -> {%s} [%i, %i, %i]\n", pfq.quP->name, pfq.noA.modelN,
	  pfq.yesA.modelN, pfq.unkA.modelN);
	  #endif
	*/
    }
  }

#ifdef QUESTION_VERBOSE
  if ( modN  > 0 ) modDistMean  /= modN;
  if ( normN > 0 ) normDistMean /= normN;
  printf ("NORMSTAT %s : #%3i [%e, %e] av %e (%i {%s})\n", ptrP->name, normN,
	  normDistMin, normDistMax, normDistMean, 0,
	  (idx>=0)?QS->list.itemA[idx].name:"");
  printf ("MODSTAT  %s : #%3i [%e, %e] av %e (%i {%s})\n", ptrP->name, modN,
	  modDistMin, modDistMax, modDistMean, rank,
	  (modIdx>=0)?QS->list.itemA[modIdx].name:"");
  fflush(stdout);
#endif

  modelArrayDeinit( &(pfq.yesA));
  modelArrayDeinit( &(pfq.noA));
  modelArrayDeinit( &(pfq.unkA));

  if ( score) *score = maxDist;
  return idx;
}

int ptreeFindQuestionFast (PTree *ptrP, QuestionSet *QS, float minCount,
			   float *score, int *excludeA) {

    ModelSetDistFct   *distFct =  ptrP->mdsP.modelSetP->distFct;
    int                N       =  QS->list.itemN;
    double             maxDist = -9e19;
    int                idx     = -1;
    PTreeFindQuestion  pfq;
    int                i;

#ifdef QUESTION_VERBOSE
    float normDistMean =  0.0,     modDistMean =  0.0;
    float normDistMin  =  FLT_MAX, modDistMin  =  FLT_MAX;
    float normDistMax  = -FLT_MAX, modDistMax  = -FLT_MAX;
    int   normIdx      =  0;
    int   modIdx       = -1;
    int   rank         =  0;
    int   modN         =  0;
    int   normN        =  0;
    
    /* all modality questions are contextindependent (0=)
       all modality questions are at the begin of the questionSet
       the first normal question is (0=WB)
       ==> normIdx is the index of (0=WB) in QS */
    normIdx = questionSetIndex (QS, "0=WB");
#endif

    if ( !distFct ) {
	ERROR("Cannot find question because modelSet has no distance function.\n");
	return -1;
    }

    /* initialize pfq */
    modelArrayInit (&(pfq.yesA), &(ptrP->mdsP));
    modelArrayInit (&(pfq.noA),  &(ptrP->mdsP));
    modelArrayInit (&(pfq.unkA), &(ptrP->mdsP));

#ifdef QUESTION_VERBOSE
    printf("excludeA %i {", excludeA);
    for (i=0; i<N; i++) printf ("%i", excludeA[i]);
    printf("}\n");
#endif

    /* for all available questions */
    for (i = 0; i < N; i++) {
	float dist = 0.0;

	/* is the question in excludeA */
	if ( !excludeA[i] ) {

	  pfq.quP = QS->list.itemA + i;

	    modelArrayClear (&(pfq.yesA));
	    modelArrayClear (&(pfq.noA));
	    modelArrayClear (&(pfq.unkA));

	    /* divide tree in three disjunctive modelArrays */
	    ptreeDFS (ptrP, minCount, ptreeFindQuestionFct, (ClientData)&pfq);

	    /* find out best distance */
	    if ( ((pfq.noA.modelN  > 0 && pfq.yesA.modelN > 0) || 
		  (pfq.yesA.modelN > 0 && pfq.unkA.modelN > 0) || 
		  (pfq.noA.modelN  > 0 && pfq.unkA.modelN > 0)) &&
		 distFct (ptrP->mdsP.cd, &(pfq.noA), &(pfq.yesA), &(pfq.unkA),
			  &dist) == TCL_OK ) {

#ifdef QUESTION_VERBOSE
		if ( i < normIdx ) {
		    modDistMean += dist;
		    modN++;
		    if ( dist > modDistMax ) { modDistMax = dist; modIdx = i; }
		    if ( dist < modDistMin )   modDistMin = dist;
		} else {
		    normDistMean += dist;
		    normN++;
		    if ( dist > normDistMax ) normDistMax = dist;
		    if ( dist < normDistMin ) normDistMin = dist;
		    if ( dist > modDistMax )  rank++;
		}
		printf (" -> {%s} [%i, %i, %i] -> %e\n", pfq.quP->name,
			pfq.noA.modelN, pfq.yesA.modelN, pfq.unkA.modelN, dist);
#endif

		if ( dist > maxDist ) { maxDist = dist; idx = i; }

	    } else {
		/* distance can't be calculated because of minCount OR
		   two of the modelArrays has no models */
		excludeA[i] = 1;

#ifdef QUESTION_VERBOSE
		printf (" -> {%s} [%i, %i, %i]\n", pfq.quP->name, pfq.noA.modelN,
			pfq.yesA.modelN, pfq.unkA.modelN);
#endif
	    }
	}
    }

#ifdef QUESTION_VERBOSE
  if ( modN  > 0 ) modDistMean  /= modN;
  if ( normN > 0 ) normDistMean /= normN;
  printf ("NORMSTAT %s : #%3i [%e, %e] av %e (%i {%s})\n", ptrP->name, normN,
	  normDistMin, normDistMax, normDistMean, 0,
	  (idx>=0)?QS->list.itemA[idx].name:"");
  printf ("MODSTAT  %s : #%3i [%e, %e] av %e (%i {%s})\n", ptrP->name, modN,
	  modDistMin, modDistMax, modDistMean, rank,
	  (modIdx>=0)?QS->list.itemA[modIdx].name:"");
  fflush(stdout);
#endif

    modelArrayDeinit (&(pfq.yesA));
    modelArrayDeinit (&(pfq.noA));
    modelArrayDeinit (&(pfq.unkA));

    /* add best splitting question to excludeA */
    if ( idx >= 0 ) excludeA[idx] = 1;

    if ( score ) *score = maxDist;

    return idx;
}


int ptreeFindQuestionItf( ClientData clientData, int argc, char *argv[])
{
  PTree*       tree     = (PTree*)clientData;
  int          ac       =  argc - 1;
  QuestionSet* qsetP    =  NULL;
  float        minCount =  0;

  if ( itfParseArgv( argv[0], &ac, argv+1, 0,
      "<questionSet>",ARGV_OBJECT, NULL, &qsetP, "QuestionSet", "question set", 
       "-minCount",   ARGV_FLOAT,  NULL, &minCount, NULL, "minimum count",
       NULL) != TCL_OK) return TCL_ERROR;
  else {
    float score;
    int   idx   = ptreeFindQuestion( tree, qsetP, minCount, &score);

    if ( idx < 0) {
      ERROR( "Can't find a question for splitting the polyphonic tree.\n");
      return TCL_ERROR;
    }
    itfAppendResult("{%s} %e", qsetP->list.itemA[idx].name, score);
  }
  return TCL_OK;
}

/* ========================================================================
    PTreeSet
   ======================================================================== */

static PTreeSet ptreeSetDefault;

/* ------------------------------------------------------------------------
    Create/Init PTreeSet
   ------------------------------------------------------------------------ */

int ptreeSetInit( PTreeSet* treeSet, char* name, Phones* phones, 
                                     Tags* tags, ModelSetPtr* mdsP)
{
  int retCode = TCL_OK;

  treeSet->name         = strdup(name);
  treeSet->useN         = 0;

  listInit((List*)&(treeSet->list), &ptreeInfo, sizeof(PTree),
                    ptreeSetDefault.list.blkSize);

  treeSet->list.init    = (ListItemInit  *)ptreeInit;
  treeSet->list.deinit  = (ListItemDeinit*)ptreeDeinit;

  treeSet->phones       =  phones;  link( phones, "Phones");
  treeSet->tags         =  tags;    link( tags,   "Tags");
  treeSet->commentChar  =  ptreeSetDefault.commentChar;

  /* initialization of the modelSet array */

  treeSet->mdsP.cd        = mdsP->cd;
  treeSet->mdsP.modelSetP = mdsP->modelSetP;

  return retCode;
}

PTreeSet* ptreeSetCreate( char *name, Phones *phones, Tags *tags, 
                          ModelSetPtr* mdsP)
{
  PTreeSet* ptreeSet = (PTreeSet*)malloc(sizeof(PTreeSet));

  if (! ptreeSet || ptreeSetInit( ptreeSet, name, phones, tags, 
                                  mdsP) != TCL_OK) {
     if ( ptreeSet) free( ptreeSet);
     ERROR("Failed to allocate PTreeSet object '%s'.\n", name);
     return NULL; 
  }
  return ptreeSet;
}

static ClientData ptreeSetCreateItf( ClientData clientData, 
                                     int argc, char *argv[]) 
{
  Phones*     phones    = NULL;
  Tags*       tags      = NULL;
  int         ac        = argc - 1;
  ModelSetPtr mdsP;

  modelSetPtrInit( &mdsP);

  if ( itfParseArgv( argv[0], &ac, argv+1, 0,
      "<phones>",   ARGV_OBJECT,NULL,&phones, "Phones",    "set of phones",
      "<tags>",     ARGV_OBJECT,NULL,&tags,   "Tags",      "set of tags",
      "<modelSet>", ARGV_CUSTOM,getModelSetPtr,&mdsP,NULL,"set of models",
       NULL) != TCL_OK) {
    modelSetPtrDeinit( &mdsP);
    return NULL;
  }
  return (ClientData)ptreeSetCreate(argv[0],phones,tags,&mdsP);
}

/* ------------------------------------------------------------------------
    Free PTreeSet
   ------------------------------------------------------------------------ */

int ptreeSetLinkN( PTreeSet* ptree)
{
  int  useN =  listLinkN((List*)&(ptree->list)) - 1;

  if ( useN < ptree->useN) return ptree->useN;
  else                     return useN;
}

int ptreeSetDeinit( PTreeSet* ptree) 
{
  int lrc;

  if ( ptreeSetLinkN( ptree) >1) {
    SERROR("PTreeSet '%s' still in use by other objects.\n", ptree->name);
    return TCL_ERROR;
  }
  if ( ptree->name) { free( ptree->name); ptree->name = NULL; }

  if ( ptree->phones) { unlink(ptree->phones,"Phones"); ptree->phones = NULL; } 
  if ( ptree->tags)   { unlink(ptree->tags,  "Tags");   ptree->tags   = NULL; } 

  lrc = listDeinit((List*)&(ptree->list));
  return (lrc != TCL_OK) ? TCL_ERROR : TCL_OK;
}

int ptreeSetFree( PTreeSet* ptree)
{
  if ( ptreeSetDeinit( ptree) != TCL_OK) return TCL_ERROR;
  free(ptree);
  return TCL_OK;
}

static int ptreeSetFreeItf (ClientData clientData)
{
  PTreeSet* ptreeSet = (PTreeSet*)clientData;
  return ptreeSetFree( ptreeSet);
}

/* ------------------------------------------------------------------------
    ptreeSetConfigureItf
   ------------------------------------------------------------------------ */

int ptreeSetConfigureItf(ClientData cd, char *var, char *val)
{
  PTreeSet *TR = (PTreeSet*)cd;
  if (! TR) TR = &ptreeSetDefault;

  if (var==NULL) {

    ITFCFG(ptreeSetConfigureItf,cd,"name");
    ITFCFG(ptreeSetConfigureItf,cd,"useN");
    ITFCFG(ptreeSetConfigureItf,cd,"commentChar");
    return listConfigureItf((ClientData)&(TR->list), var, val);
  }
  ITFCFG_CharPtr(var,val,"name",         TR->name,         1);
  ITFCFG_Int(    var,val,"useN",         TR->useN,         1);
  ITFCFG_Char(   var,val,"commentChar",  TR->commentChar,  0);
  return listConfigureItf((ClientData)&(TR->list), var, val);   
}

/* ------------------------------------------------------------------------
    ptreeSetAccessItf
   ------------------------------------------------------------------------ */

ClientData ptreeSetAccessItf( ClientData cd, char *name, TypeInfo **ti)
{ 
  PTreeSet* ptree = (PTreeSet*)cd;

  if (*name == '.') { 
    if (name[1]=='\0') {
       itfAppendElement("modelSet");
       *ti = NULL;
    }
    else {
      if (! strcmp( name+1, "modelSet")) {
        *ti = ptree->mdsP.modelSetP->tiP;
        return ptree->mdsP.cd;
      } 
    }
  }
  return listAccessItf((ClientData)&(ptree->list),name,ti);
}

/* ------------------------------------------------------------------------
    ptreeSetPuts
   ------------------------------------------------------------------------ */

int ptreeSetPutsItf (ClientData clientData, int argc, char *argv[]) 
{
  PTreeSet* ptree = (PTreeSet*)clientData;
  return listPutsItf((ClientData)&(ptree->list), argc, argv);
}

/* ------------------------------------------------------------------------
    ptreeSetIndex
   ------------------------------------------------------------------------ */

int ptreeSetIndex( PTreeSet* ptreeSet,  char* name) 
{
  return listIndex((List*)&(ptreeSet->list), name, 0);
}

int ptreeSetIndexItf( ClientData clientData, int argc, char *argv[] )
{
  PTreeSet* ptreeSet = (PTreeSet*)clientData;
  return listName2IndexItf((ClientData)&(ptreeSet->list), argc, argv);
}

/* ------------------------------------------------------------------------
    ptreeSetName
   ------------------------------------------------------------------------ */

char* ptreeSetName( PTreeSet* ptreeSet, int i) 
{ 
  return (i < 0) ? "(null)" : ptreeSet->list.itemA[i].name;
}

int ptreeSetNameItf( ClientData clientData, int argc, char *argv[] )
{
  PTreeSet* ptreeSet = (PTreeSet*)clientData;
  return listIndex2NameItf((ClientData)&(ptreeSet->list), argc, argv);
}

/* ------------------------------------------------------------------------
    ptreeSetAdd
   ------------------------------------------------------------------------ */

int ptreeSetAdd( PTreeSet *ptree, char* name)
{
  int idx = ptreeSetIndex( ptree, name);

  if ( idx >= 0) return idx;
  if ((idx  = listNewItem((List*)&(ptree->list),(ClientData)name)) >= 0) { 

    ptree->list.itemA[idx].phones         = ptree->phones;
    ptree->list.itemA[idx].tags           = ptree->tags;
    ptree->list.itemA[idx].mdsP.cd        = ptree->mdsP.cd;
    ptree->list.itemA[idx].mdsP.modelSetP = ptree->mdsP.modelSetP;
  }
  return idx;
}

int ptreeSetAddItf( ClientData clientData, int argc, char *argv[])
{
  PTreeSet* treeSet = (PTreeSet*)clientData;
  char*     name    =  NULL;
  int       ac      =  argc - 1;
  int       idx, i;

  if ( itfParseArgv( argv[0], &ac, argv+1, 1,
      "<name>",      ARGV_STRING,NULL,&name, NULL, "name of polyphonic tree",
      "<polyphone>", ARGV_REST,  NULL,&i,    NULL, "polyphone description",
       NULL) != TCL_OK) return TCL_ERROR;

  if ((idx = ptreeSetAdd( treeSet, name)) < 0) return TCL_ERROR;

  if ( i < ac)
       return ptreeAddItf((ClientData)(treeSet->list.itemA+idx), ac-i+1, argv+i);
  else return TCL_OK;
}

/* ------------------------------------------------------------------------
    ptreeSetRead
   ------------------------------------------------------------------------ */

int ptreeSetReadItf ( ClientData clientData, int argc, char *argv[] )
{
  PTreeSet*  ptreeP  = (PTreeSet*)clientData;
  char*      fname   =  NULL;
  int        ac      =  argc-1, n;

  if (itfParseArgv(argv[0], &ac, argv+1, 0,
     "<filename>",ARGV_STRING,NULL,&fname,NULL,"name of PTreeSet file", 
       NULL) != TCL_OK) return TCL_ERROR;

  itfAppendResult( "%d", n = freadAdd( clientData, fname, ptreeP->commentChar,
                                       ptreeSetAddItf));
  return (n >= 0) ? TCL_OK : TCL_ERROR;
}

/* ------------------------------------------------------------------------
    ptreeSetWrite
   ------------------------------------------------------------------------ */

int ptreeSetWrite( PTreeSet *tree, char *fileName)
{
  PTree* ptP   = tree->list.itemA;
  int    treeN = tree->list.itemN, treeX;
  FILE*  fp;

  if (! (fp = fileOpen(fileName,"w"))) return TCL_ERROR;

  fprintf( fp, "%c -------------------------------------------------------\n",
                tree->commentChar);
  fprintf( fp, "%c  Name            : %s\n", tree->commentChar,
                tree->name);
  fprintf( fp, "%c  Type            : PTreeSet\n", tree->commentChar);
  fprintf( fp, "%c  Date            : %s", tree->commentChar,
                dateString());
  fprintf( fp, "%c -------------------------------------------------------\n",
                tree->commentChar);

  for ( treeX = 0; treeX < treeN; treeX++, ptP++) ptreeWrite( ptP, 1, fp);
  fileClose( fileName, fp);
  return TCL_OK;
}

int ptreeSetWriteItf (ClientData clientData, int argc, char *argv[] )
{
  char   *fname    =  NULL;
  int     ac       =  argc-1;
  int     minCount =  1;

  if (itfParseArgv(argv[0], &ac, argv+1, 0,
     "<filename>",ARGV_STRING,NULL,&fname,    NULL, "name of tree file", 
     "-minCount", ARGV_INT,   NULL,&minCount, NULL, "minimum count",
      NULL) != TCL_OK)
    return TCL_ERROR;

  return ptreeSetWrite((PTreeSet*)clientData, fname);
}


float ptreeModelArrayAddFct (PTree *ptrP, PTreeNode *ptnP, float count, int model,
			     ClientData cd, Word *wP, int lc, int rc) {

    ModelArray* mArrP = (ModelArray*)cd;

    if ( ptrP->childP != ptnP && model > -1 )
	modelArrayAdd (mArrP, model, count);

    return count;
}

float ptreeModelArrayAdd (PTree *ptrP, float minCount, ClientData cd) {

    return ptreeDFS (ptrP, minCount, ptreeModelArrayAddFct, cd);
}

/* ========================================================================
    Type Declaration
   ======================================================================== */

static Method ptreeNodeMethod[] = 
{
  { "puts", ptreeNodePutsItf,   NULL },
  {  NULL,  NULL, NULL } 
};

TypeInfo ptreeNodeInfo = {
        "PTreeNode", 0, 0, ptreeNodeMethod, 
         NULL, NULL, 
         ptreeNodeConfigureItf, ptreeNodeAccessItf, itfTypeCntlDefaultNoLink,
        "PTreeNode\n" };

static Method ptreeMethod[] = 
{
  { "puts",     ptreePutsItf,   NULL },
  { "add",      ptreeAddItf,   "adds another polyphone to the tree"           },
  { "get",      ptreeGetItf,   "find polyphone in the tree"                   },
  { "models",   ptreeModelsItf,"returns a model array of models in the tree"  },
  { "split",    ptreeSplitItf, "split a tree by asking a question"            },
  { "question", ptreeFindQuestionItf, "find a question for splitting" },
  {  NULL,  NULL, NULL } 
};

TypeInfo ptreeInfo = {
        "PTree", 0, -1,     ptreeMethod, 
         ptreeCreateItf,    ptreeFreeItf, 
         ptreeConfigureItf, ptreeAccessItf, itfTypeCntlDefaultNoLink,
        "Polyphonic Tree\n" };

static Method ptreeSetMethod[] = 
{
  { "puts",   ptreeSetPutsItf,  "displays the contents of a PTreeSet object" },
  { "add",    ptreeSetAddItf,   "adds another polyphonic tree"               }, 
  { "name",   ptreeSetNameItf,  "find name of a polyphone tree"              },
  { "index",  ptreeSetIndexItf, "find index of a polyphone tree"             },
  { "read",   ptreeSetReadItf,  "reads polyphone tree from a file"           },
  { "write",  ptreeSetWriteItf, "writes polyphone tree to a file"            },
  { NULL, NULL, NULL }
};

TypeInfo ptreeSetInfo = { 
        "PTreeSet", 0, -1,     ptreeSetMethod, 
         ptreeSetCreateItf,    ptreeSetFreeItf, 
         ptreeSetConfigureItf, ptreeSetAccessItf, NULL,
        "A 'PTreeSet' object is a set of polyphone context trees." };

static int ptreeInitialized = 0;

int PTree_Init ()
{
  if (! ptreeInitialized) {
    ptreeDefaults.maxContext      = -1;
    ptreeDefaults.addProc         =  NULL;

    ptreeSetDefault.name          =  NULL;
    ptreeSetDefault.useN          =  0;
    ptreeSetDefault.list.itemN    =  0;
    ptreeSetDefault.list.blkSize  =  100;
    ptreeSetDefault.commentChar   =  ';';

    itfNewType(&ptreeNodeInfo);
    itfNewType(&ptreeInfo);
    itfNewType(&ptreeSetInfo);
    ptreeInitialized = 1;
  }
  return TCL_OK;
}
