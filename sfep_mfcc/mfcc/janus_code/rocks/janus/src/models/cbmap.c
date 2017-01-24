/* ========================================================================
    JANUS      Speech- to Speech Translation
               ------------------------------------------------------------
               Object: Codebook
               ------------------------------------------------------------

    Author  :  Michael Finke & Ivica Rogina
    Module  :  codebook.c
    Date    :  $Id: cbmap.c 2390 2003-08-14 11:20:32Z fuegen $
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
    Revision 3.4  2003/08/14 11:20:13  fuegen
    Merged changes on branch jtk-01-01-15-fms (jaguar -> ibis-013)

    Revision 3.3.56.1  2002/06/26 11:57:55  fuegen
    changes for new directory structure support and code cleaning

    Revision 3.3  1997/07/30 13:40:47  rogina
    made code gcc -DFKIclean

    Revision 3.2  1997/06/17 17:41:43  westphal
    *** empty log message ***

 * Revision 1.1  95/12/27  16:03:50  rogina
 * Initial revision
 * 

   ======================================================================== */

#include "common.h"
#include "itf.h"
#include "codebook.h" /* includes cbmap.h */


/* ========================================================================
    CodebookMapItem
   ======================================================================== */
/* ------------------------------------------------------------------------
    cbMapItemInit  initialize a CodebookMapItem object
   ------------------------------------------------------------------------ */

int cbMapItemInit( CodebookMapItem* cbmiP)
{
  assert(cbmiP);
  cbmiP->from   = -1;
  cbmiP->to     = -1;
  cbmiP->subX   = -1;
  cbmiP->alpha0 =  0.0;
  cbmiP->alpha  =  1.0;
  cbmiP->beta0  =  0.0;
  cbmiP->beta   =  1.0;
  return TCL_OK;
}

/* ------------------------------------------------------------------------
    cbMapItemDeinit  deinitialize a CodebookMapItem object
   ------------------------------------------------------------------------ */

int cbMapItemDeinit( CodebookMapItem* cbmiP)
{
  assert( cbmiP);
  return TCL_OK;
}

/* ------------------------------------------------------------------------
    cbMapItemConfigureItf   configure a CodebookMapItem
   ------------------------------------------------------------------------ */

static int cbMapItemConfigureItf(ClientData cd, char *var, char *val)
{
  CodebookMapItem *cbmiP = (CodebookMapItem*)cd;
  if (! cbmiP) return TCL_ERROR;

  if (! var) {

    ITFCFG(cbMapItemConfigureItf,cd,"from");
    ITFCFG(cbMapItemConfigureItf,cd,"to");
    ITFCFG(cbMapItemConfigureItf,cd,"subX");
    ITFCFG(cbMapItemConfigureItf,cd,"alpha0");
    ITFCFG(cbMapItemConfigureItf,cd,"alpha");
    ITFCFG(cbMapItemConfigureItf,cd,"beta0");
    ITFCFG(cbMapItemConfigureItf,cd,"beta");
    return TCL_OK;
  }
  ITFCFG_Int(    var,val,"from",   cbmiP->from,   0);
  ITFCFG_Int(    var,val,"to",     cbmiP->to,     0);
  ITFCFG_Int(    var,val,"subX",   cbmiP->subX,   0);
  ITFCFG_Float(  var,val,"alpha0", cbmiP->alpha0, 0);
  ITFCFG_Float(  var,val,"alpha",  cbmiP->alpha,  0);
  ITFCFG_Float(  var,val,"beta0",  cbmiP->beta0,  0);
  ITFCFG_Float(  var,val,"beta",   cbmiP->beta,   0);
  ERROR("unknown option '-%s %s'.\n", var, val ? val : "?");
  return TCL_ERROR;   
}

/* ------------------------------------------------------------------------
    cbMapItemPutsItf
   ------------------------------------------------------------------------ */

static int cbMapItemPutsItf (ClientData cd, int argc, char *argv[]) 
{
  CodebookMapItem *cbmiP = (CodebookMapItem*)cd;
  itfAppendResult("%d %d %e %e %e %e",
                   cbmiP->from,   cbmiP->to,
                   cbmiP->alpha0, cbmiP->alpha, cbmiP->beta0, cbmiP->beta);
  return TCL_OK;
}

/* ========================================================================
    CodebookMapItem
   ======================================================================== */
/* ------------------------------------------------------------------------
    cbMapInit  initialize a CodebookMap object
   ------------------------------------------------------------------------ */

int cbMapInit( CodebookMap* cbmP)
{
  assert(cbmP);
  cbmP->itemN    = 0;
  cbmP->itemA    = NULL;
  return TCL_OK;
}

CodebookMap* cbMapCreate( int n)
{
  CodebookMap *cbmP = (CodebookMap*)malloc(sizeof(CodebookMap));

  if ((!cbmP) || (cbMapInit(cbmP) != TCL_OK)) {
    if (cbmP) free (cbmP);
    ERROR("Cannot create CodebookMap object.\n");
    return NULL;
  }
  cbMapAdd( cbmP, n, -1, -1);
  return cbmP;
}

/* ------------------------------------------------------------------------
    cbMapDeinit  deinitialize a CodebookMap object
   ------------------------------------------------------------------------ */

int cbMapDeinit( CodebookMap* cbmP)
{
  assert( cbmP);

  if ( cbmP->itemA) {
    int i;
    for ( i = 0; i < cbmP->itemN; i++) cbMapItemDeinit( cbmP->itemA + i);
    free( cbmP->itemA);
    cbmP->itemA = NULL; cbmP->itemN = 0;
  }
  return TCL_OK;
}

int cbMapFree( CodebookMap* cbmP)
{
  if (cbMapDeinit(cbmP) != TCL_OK) return TCL_ERROR;
  free(cbmP);
  return TCL_OK;
}

/* ------------------------------------------------------------------------
    cbMapConfigureItf     configure items in the CodebookMap object
   ------------------------------------------------------------------------ */

static int cbMapConfigureItf(ClientData cd, char *var, char *val)
{
  CodebookMap *cbmP = (CodebookMap*)cd;
  if (! cbmP) return TCL_ERROR;
  if (! var) {

    ITFCFG(cbMapConfigureItf,cd,"itemN");
    return TCL_OK;
  }
  ITFCFG_Int(   var,val,"itemN",    cbmP->itemN,    1);
  ERROR("unknown option '-%s %s'.\n", var, val ? val : "?");
  return TCL_ERROR;   
}

/* ------------------------------------------------------------------------
    cbMapAccessItf     access subitems in the CodebookMap object
   ------------------------------------------------------------------------ */

static ClientData cbMapAccessItf (ClientData cd, char *name, TypeInfo **ti)
{
  CodebookMap* cbmP = (CodebookMap*)cd;
  int          idx;

  if ( *name == '.') {
    if ( name[1] =='\0') {
      if ( cbmP->itemN > 0) {
        itfAppendElement("item(0..%d)", cbmP->itemN-1);
      }
      *ti=NULL; return NULL; 
    }
    else {
      if (sscanf(name+1,"item(%d)", &idx) == 1) {
        *ti = itfGetType("CodebookMapItem"); 
        if ( idx >= 0 && idx < cbmP->itemN)
             return (ClientData)(cbmP->itemA+idx);
        else return  NULL;
      }
      *ti=NULL; return NULL;
    }
  }
  return NULL;
}

/* ------------------------------------------------------------------------
    cbMapPuts
   ------------------------------------------------------------------------ */

static int cbMapPutsItf (ClientData clientData, int argc, char *argv[]) 
{
  CodebookMap* cbmP = (CodebookMap*)clientData;
  int          i;

  for ( i = 0; i < cbmP->itemN; i++) {
    itfAppendResult("{ ");
    cbMapItemPutsItf((ClientData)(cbmP->itemA+i), argc, argv);
    itfAppendResult("}\n");
  }
  return TCL_OK;
}

/* ------------------------------------------------------------------------
    cbMapAdd   adds a single or a number of entries to the list in the
               CodebookMap (itemA) and fills it with idxX -> idxY values
               after having them initialized
   ------------------------------------------------------------------------ */

int cbMapAdd( CodebookMap* cbmP, int n, int from, int to)
{
  int i;

  cbmP->itemA = (CodebookMapItem*)realloc( cbmP->itemA,(cbmP->itemN+n)*
                                           sizeof(CodebookMapItem));

  for ( i = cbmP->itemN; i < cbmP->itemN+n; i++) {
    cbMapItemInit( cbmP->itemA + i);

    cbmP->itemA[i].from  = from;
    cbmP->itemA[i].to    = to;
  }
  i            = cbmP->itemN;
  cbmP->itemN += n;
  return i;
}

static int cbMapAddItf( ClientData clientData, int argc, char *argv[]) 
{
  CodebookMap *cbmP  = (CodebookMap*)clientData;
  int          ac    =  argc-1;
  int          from  = -1;
  int          to    = -1;
  int          n     =  1;

  if ( itfParseArgv (argv[0], &ac, argv+1, 0,
      "<n>",      ARGV_INT, NULL, &n,       NULL, "number of pathItems to add",
      "-from",    ARGV_INT, NULL, &from,    NULL, "map from index",
      "-to",      ARGV_INT, NULL, &to,      NULL, "map to index",
       NULL) != TCL_OK) return TCL_ERROR;

  itfAppendResult( "%d", cbMapAdd( cbmP, n, from, to));
  return TCL_OK;
}

/* ------------------------------------------------------------------------
    cbMapClear  clear the map which means that all entries are deleted
                from the itemA list
   ------------------------------------------------------------------------ */

int cbMapClear( CodebookMap* cbmP)
{
  int i;
  for ( i = 0; i < cbmP->itemN; i++) cbMapItemDeinit( cbmP->itemA + i);

  if ( cbmP->itemA) { free( cbmP->itemA); cbmP->itemA = NULL; }
  cbmP->itemN = 0;
  return TCL_OK;
}

static int cbMapClearItf (ClientData clientData, int argc, char *argv[]) 
{
  CodebookMap *cbmP = (CodebookMap*)clientData;
  int          ac   =  argc-1;

  if ( itfParseArgv (argv[0], &ac, argv+1, 0,
       NULL) != TCL_OK) return TCL_ERROR;

  return cbMapClear( cbmP);
}

/* ========================================================================
    Type Declaration
   ======================================================================== */

static Method cbMapItemMethod[] = 
{
  { "puts", cbMapItemPutsItf,   NULL },
  {  NULL,  NULL, NULL } 
};

static TypeInfo cbMapItemInfo = { 
      "CodebookMapItem", 0, 0, cbMapItemMethod, 
       NULL, NULL, 
       cbMapItemConfigureItf, NULL, 
       itfTypeCntlDefaultNoLink,
      "CodebookMapItem\n" };

static Method cbMapMethod[] = 
{
  { "puts",   cbMapPutsItf,   NULL },
  { "add",    cbMapAddItf,   "add items to the map"},
  { "clear",  cbMapClearItf, "remove all items from the map"},
  {  NULL,  NULL, NULL } 
};

static TypeInfo cbMapInfo = { 
      "CodebookMap", 0, 0, cbMapMethod, 
       NULL, NULL, 
       cbMapConfigureItf, cbMapAccessItf, 
       itfTypeCntlDefaultNoLink,
      "CodebookMap\n" };


static int cbmapInitialized = 0;

int Cbmap_Init(void)
{
  if ( cbmapInitialized) return TCL_OK;

  itfNewType( &cbMapItemInfo);
  itfNewType( &cbMapInfo);

  cbmapInitialized = 1;
  return TCL_OK;
}
