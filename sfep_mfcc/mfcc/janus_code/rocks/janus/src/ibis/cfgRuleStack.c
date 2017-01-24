/* ========================================================================
    JANUS      Speech- to Speech Translation
               ------------------------------------------------------------
               Object: Context Free Grammar Rule Stack
               ------------------------------------------------------------

    Author  :  Christian Fuegen
    Module  :  cfgRuleStack.c
    Date    :  $Id: cfgRuleStack.c 3416 2011-03-23 03:02:18Z metze $
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
    Revision 4.2  2004/09/16 14:36:37  metze
    Made code 'x86_64'-clean

    Revision 4.1  2003/08/14 11:20:01  fuegen
    Merged changes on branch jtk-01-01-15-fms (jaguar -> ibis-013)

    Revision 1.1.2.18  2003/08/13 08:54:38  fuegen
    startover requires now float factor

    Revision 1.1.2.17  2003/07/25 09:50:38  fuegen
    removed fprintf in cfgRuleStackTrace
    removed some bugs in cfgRuleStackSaveDump

    Revision 1.1.2.16  2003/07/15 17:02:05  fuegen
    some more changes
      - added forward/recursive arc types
      - added dumping of CFGs/CFGSets (together with pt/rs)
      - added compression/determinization of CFGs/CFGSets
      - rule stack belongs no more to a single grammar
      - changed LingKS load/save interface, added LingKSPutsFct

    Revision 1.1.2.15  2003/06/23 13:12:07  fuegen
    new CFG implementation
      - less memory usage
      - activation/deactivation down to rule level
      - adding of paths, grammar rules, grammars on the fly
      - basic JSGF grammar support
      - additional support of CFG as LingKS type
      - standalone support of CFG/CFGSet, when using only for e.g. parsing

    Revision 1.1.2.14  2002/11/21 17:10:09  fuegen
    windows code cleaning

    Revision 1.1.2.13  2002/08/26 11:26:37  fuegen
    Removed bugs in decoding along parallel grammars and
    with shared grammars.
    Added output of the parse tree.
    Added reading of grammar files with windows line endings.

    Revision 1.1.2.12  2002/06/14 08:54:20  fuegen
    cfgRuleStackItemPuts now checks cfgGLP exists

    Revision 1.1.2.11  2002/06/10 11:36:20  metze
    Go away, boring error message

    Revision 1.1.2.10  2002/05/21 15:25:19  fuegen
    made gcc clean

    Revision 1.1.2.9  2002/04/18 09:23:29  fuegen
    changed rule stack interface, to handle also crosslinks between grammars

    Revision 1.1.2.8  2002/02/19 16:32:57  fuegen
    added CFGRuleStackGetTop, removed bug in cfgRuleStackPop

    Revision 1.1.2.7  2002/02/06 13:20:56  fuegen
    changed memory management (bmem)

    Revision 1.1.2.6  2002/02/01 14:46:00  fuegen
    changed rule stack handling

    Revision 1.1.2.5  2002/01/25 17:16:28  fuegen
    added handling of starting new trees, added handling of shared grammars, added some minor things for saving memory

    Revision 1.1.2.4  2002/01/25 17:10:21  fuegen
    first real working version (only parsing)

    Revision 1.1.2.3  2002/01/25 17:01:19  fuegen
    made rule stack implementation faster and better

    Revision 1.1.2.2  2002/01/25 16:53:27  fuegen
    major and minor changes: lexicon as cache, extension routines, lingKS interface

    Revision 1.1.2.1  2002/01/25 16:38:55  fuegen
    reimplementation of a big part, using now indices for memory saving reasons, splitted CFG-implementation in smaller modules


 
   ======================================================================== */

#include "common.h"
#include "itf.h"
#include "cfg.h"

/* =======================================================================
   Forward Declarations
   ======================================================================= */

extern TypeInfo			cfgRuleStackInfo;
extern TypeInfo			cfgRSItemInfo;

static CFGRuleStack		cfgRuleStackDefault;
static CFGRSItem		cfgRSItemDefault;

extern CFGRSItem* cfgRuleStackNewItem (CFGRuleStack *cfgRSP);

/* ======================================================================
   Context Free Grammar Rule Stack Item
   ====================================================================== */

/* ----------------------------------------------------------------------
   Init/Create Context Free Grammar Rule Stack Item Object
   ---------------------------------------------------------------------- */

int cfgRSItemInit (CFGRSItem *cfgRSIP) {

  cfgRSIP->cfgAP   = cfgRSItemDefault.cfgAP;
  cfgRSIP->childP  = cfgRSItemDefault.childP;
  cfgRSIP->nextP   = cfgRSItemDefault.nextP;
  cfgRSIP->parentP = cfgRSItemDefault.parentP;

  return TCL_OK;
}

CFGRSItem* cfgRSItemCreate (CFGRuleStack *cfgRSP, CFGArc *cfgAP, CFGRSItem *parentP){

  CFGRSItem *cfgRSIP = cfgRuleStackNewItem (cfgRSP);

  if ( !cfgRSIP || cfgRSItemInit (cfgRSIP) != TCL_OK ) {
    if ( cfgRSIP ) free (cfgRSIP);
    ERROR ("Cannot create CFGRSItem object.\n");
    return NULL;
  }

  cfgRSIP->cfgAP = cfgAP;

  if ( parentP ) {
    cfgRSIP->parentP = parentP;
    cfgRSIP->nextP   = parentP->childP;
    parentP->childP  = cfgRSIP;
  }

  return cfgRSIP;
}

/* ----------------------------------------------------------------------
   Deinit/Free Context Free Grammar Rule Stack Item Object
   ---------------------------------------------------------------------- */

int cfgRSItemDeinit (CFGRSItem *cfgRSIP) {

  /* set all to default */
  return cfgRSItemInit (cfgRSIP);
}

int cfgRSItemFree (CFGRSItem *cfgRSIP) {

  if ( cfgRSItemDeinit (cfgRSIP) != TCL_OK ) return TCL_ERROR;
  /* !!! START FF HACK !!! */
  /* DON'T FREE THIS WAY !!! THE ROOT WAS ALLOCATED WITH BMEM !!! */
  /* if ( cfgRSIP ) free (cfgRSIP); */
  /* !!! END FF HACK !!! */

  return TCL_OK;
}

/* ----------------------------------------------------------------------
   Access/Configure Context Free Grammar Rule Stack Items
   ---------------------------------------------------------------------- */

static int cfgRSItemConfigureItf (ClientData cd, char *var, char *val) {
  
  CFGRSItem *cfgRSIP = (CFGRSItem*)cd;
  
  if ( !cfgRSIP ) cfgRSIP = &cfgRSItemDefault;
  
  if ( var == NULL ) {
    return TCL_OK;
  }

  ERROR("unknown option '-%s %s'.\n", var, val ? val : "?");
  
  return TCL_ERROR;
}

static ClientData cfgRSItemAccessItf (ClientData cd, char *name, TypeInfo **ti) {
  
  CFGRSItem *cfgRSIP = (CFGRSItem*)cd;

  if ( *name == '.' ) {
    if ( name[1] == '\0' ) {
      itfAppendElement ("arc");
      itfAppendElement ("child");
      itfAppendElement ("next");
      itfAppendElement ("parent");
    } else {
      if ( !strcmp (name+1, "arc") ) {
	*ti = itfGetType ("CFGArc");
	return (ClientData)cfgRSIP->cfgAP;
      }
      if ( !strcmp (name+1, "child") ) {
	*ti = itfGetType ("CFGRSItem");
	return (ClientData)cfgRSIP->childP;
      }
      if ( !strcmp (name+1, "next") ) {
	*ti = itfGetType ("CFGRSItem");
	return (ClientData)cfgRSIP->nextP;
      }
      if ( !strcmp (name+1, "parent") ) {
	*ti = itfGetType ("CFGRSItem");
	return (ClientData)cfgRSIP->parentP;
      }
    }
  }
  
  *ti = NULL;
  return NULL;
}

/* ----------------------------------------------------------------------
   cfgRSItemPuts
   ---------------------------------------------------------------------- */
int cfgRSItemPuts (CFGRSItem *cfgRSIP, CFGPutsFormat f) {

  switch (f) {
  case Recursive_Puts:
    return TCL_OK;
  case Short_Puts:
    return TCL_OK;
  case Long_Puts:
    return TCL_OK;
  }

  ERROR ("Not supported or unknown PutsFormat %d=%s", f, cfgPutsFormat2str(f));

  return TCL_ERROR;
}

static int cfgRSItemPutsItf (ClientData cd, int argc, char *argv[]) {

  CFGRSItem *cfgRSIP = (CFGRSItem*)cd;
  char      *format  = "short";

  argc--;
  if ( itfParseArgv (argv[0], &argc, argv+1, 0,
		     "-format",  ARGV_STRING, NULL, &format, NULL,
		     "output format (SHORT, LONG)",
		     NULL) != TCL_OK )
    return TCL_ERROR;

  return cfgRSItemPuts (cfgRSIP, cfgPutsFormat2int (format));
}


/* ======================================================================
   Context Free Grammar Rule Stack
   ====================================================================== */

/* ----------------------------------------------------------------------
   Init/Create Context Free Grammar Rule Stack Object
   ---------------------------------------------------------------------- */

int cfgRuleStackInit (CFGRuleStack *cfgRSP) {

  cfgRSP->mem.items = cfgRuleStackDefault.mem.items;
  cfgRSP->root      = cfgRuleStackDefault.root;

  return TCL_OK;
}

CFGRuleStack* cfgRuleStackCreate () {

  CFGRuleStack *cfgRSP = (CFGRuleStack*)malloc (sizeof (CFGRuleStack));

  if ( !cfgRSP || cfgRuleStackInit (cfgRSP) != TCL_OK ) {
    if ( cfgRSP ) free (cfgRSP);
    ERROR ("Cannot create CFGRuleStack object.\n");
    return NULL;
  }

  /* create bmem object and set limits */
  cfgRSP->mem.items = bmemCreate (50, sizeof(CFGRSItem));

  bmemSetLimits (cfgRSP->mem.items, CFGRSItemX_NIL, CFGRSItemX_MAX);

  return cfgRSP;
}

/* ----------------------------------------------------------------------
   Deinit/Free Context Free Grammar Rule Stack Object
   ---------------------------------------------------------------------- */

int cfgRuleStackDeinit (CFGRuleStack *cfgRSP) {

  /* free root */
  if ( cfgRSP->root ) cfgRSItemFree (cfgRSP->root);

  /* free memory block */
  if ( cfgRSP->mem.items ) { bmemFree (cfgRSP->mem.items); cfgRSP->mem.items = NULL; }

  /* set all to default */
  return cfgRuleStackInit (cfgRSP);
}

int cfgRuleStackFree (CFGRuleStack *cfgRSP) {

  if ( cfgRuleStackDeinit (cfgRSP) != TCL_OK ) return TCL_ERROR;
  if ( cfgRSP ) free (cfgRSP);

  return TCL_OK;
}

/* ----------------------------------------------------------------------
   Configure/Access Context Free Grammar Rule Stack
   ---------------------------------------------------------------------- */

static int cfgRuleStackConfigureItf (ClientData cd, char *var, char *val) {
  
  CFGRuleStack *cfgRSP = (CFGRuleStack*)cd;
  
  if ( !cfgRSP ) cfgRSP = &cfgRuleStackDefault;
  
  if ( var == NULL ) {
    ITFCFG (cfgRuleStackConfigureItf, cd, "itemN");
    return TCL_OK;
  }

  if ( !strcmp (var, "itemN") ) {
    if ( !val ) {
      itfAppendElement ("%d", bmemGetItemN (cfgRSP->mem.items));
      return TCL_OK;
    }
  }

  ERROR("unknown option '-%s %s'.\n", var, val ? val : "?");
  
  return TCL_ERROR;
}

static ClientData cfgRuleStackAccessItf (ClientData cd, char *name, TypeInfo **ti) {
  
  CFGRuleStack  *cfgRSP = (CFGRuleStack*)cd;
  int            i;
  
  if ( *name == '.' ) {
    if ( name[1] == '\0' ) {
      itfAppendElement ("item(0..%d)", bmemGetItemN (cfgRSP->mem.items)-1);
      itfAppendElement ("root");
    } else {
      if ( sscanf (name+1, "item(%d)", &i) == 1 ) {
	*ti = itfGetType ("CFGRSItem");
	if ( i >= 0 && i < (int)bmemGetItemN (cfgRSP->mem.items) )
	  return (ClientData)bmemGetItem (cfgRSP->mem.items, i);
	else return NULL;
      }
      if ( !strcmp (name+1, "root") ) {
	*ti = itfGetType ("CFGRSItem");
	return (ClientData)cfgRSP->root;
      }
    }
  }
  
  *ti = NULL;
  return NULL;
}

/* ----------------------------------------------------------------------
   cfgRuleStackPutsItf
   ---------------------------------------------------------------------- */
int cfgRuleStackPuts (CFGRuleStack *cfgRSP, CFGPutsFormat f) {

  switch (f) {
  case Short_Puts:
    return TCL_OK;
  case Long_Puts:
    return TCL_OK;
  }

  ERROR ("Not supported or unknown PutsFormat %d=%s", f, cfgPutsFormat2str(f));

  return TCL_ERROR;
}

static int cfgRuleStackPutsItf (ClientData cd, int argc, char *argv[]) {

  CFGRuleStack *cfgRSP = (CFGRuleStack*)cd;
  char         *format = "short";

  argc--;
  if ( itfParseArgv (argv[0], &argc, argv+1, 0,
 		     "-format", ARGV_STRING, NULL, &format, NULL,
		     "output format (SHORT, LONG)",
		     NULL) != TCL_OK )
    return TCL_ERROR;

  return cfgRuleStackPuts (cfgRSP, cfgPutsFormat2int (format));
}

/* ----------------------------------------------------------------------
   cfgRuleStackNewItem/GetItem/GetItemX
   ---------------------------------------------------------------------- */
CFGRSItem* cfgRuleStackNewItem (CFGRuleStack *cfgRSP) {

  if ( bmemGetItemN (cfgRSP->mem.items) == CFGRSItemX_MAX ) {
    ERROR ("cfgRuleStackNewItem: max. number of items (CFGRSItemX_MAX=%d) exceeded.\n",
	   CFGRSItemX_MAX);
    return NULL;
  }

  return (CFGRSItem*)bmemItem (cfgRSP->mem.items);
}

CFGRSItem* cfgRuleStackGetItem (CFGRuleStack *cfgRSP, CFGRSItemX itemX) {

  if ( itemX >= (CFGRSItemX)bmemGetItemN (cfgRSP->mem.items) ) return NULL;

  return(CFGRSItem*) bmemGetItem (cfgRSP->mem.items, itemX);
}

CFGRSItemX cfgRuleStackGetItemX (CFGRuleStack *cfgRSP, CFGRSItem *cfgRSIP) {

  if ( !cfgRSIP ) return CFGRSItemX_NIL;

  return (CFGRSItemX)bmemGetItemX (cfgRSP->mem.items, cfgRSIP);
}

/* ----------------------------------------------------------------------
   cfgRuleStackPush, cfgRuleStackPop
   ---------------------------------------------------------------------- */

CFGRSItem* cfgRuleStackPush (CFGRuleStack *cfgRSP, CFGRSItem *cfgRSIP, CFGArc *cfgAP) {

  CFGRSItem *nextP = cfgRSIP->childP;

  /* check if arc already exists */
  while ( nextP ) {
    if ( nextP->cfgAP == cfgAP ) return nextP;
    nextP = nextP->nextP;
  }

  return cfgRSItemCreate (cfgRSP, cfgAP, cfgRSIP);
}

CFGRSItem* cfgRuleStackPop (CFGRSItem *cfgRSIP, CFGArc **cfgAP) {

  *cfgAP = cfgRSIP->cfgAP;

  return cfgRSIP->parentP;
}


/* ----------------------------------------------------------------------
   cfgRuleStackBuild
   ---------------------------------------------------------------------- */

int cfgRuleStackBuild (CFGRuleStack *cfgRSP) {

  if ( !cfgRSP->root )
    cfgRSP->root = cfgRSItemCreate (cfgRSP, NULL, NULL);

  return TCL_OK;
}

/* ----------------------------------------------------------------------
   cfgRuleStackClear
   ---------------------------------------------------------------------- */
int cfgRuleStackClear (CFGRuleStack *cfgRSP, int free) {

  /* remove all existing items */
  if ( cfgRSP->mem.items ) {
    if ( free ) {
      bmemFree  (cfgRSP->mem.items);

      cfgRSP->mem.items = bmemCreate (50, sizeof(CFGRSItem));

      bmemSetLimits (cfgRSP->mem.items, CFGRSItemX_NIL, CFGRSItemX_MAX);
    } else {
      bmemClear (cfgRSP->mem.items);
    }
  }

  cfgRSP->root = NULL;

  return TCL_OK;
}

/* ----------------------------------------------------------------------
   cfgRuleStackTrace
   ---------------------------------------------------------------------- */
int cfgRuleStackTrace (CFGRSItem *from, CFGRSItem *to, float startover, int auxNT, CFGGrammarFormat f) {

  CFGRSItem **fitemA   = NULL;
  int         fitemN   = 0;
  CFGRSItem **titemA   = NULL;
  int         titemN   = 0;
  CFGRSItem  *parentP;
  CFGArc     *cfgAP;
  CFG        *cfgP;
  char        nt[CFG_MAXLINE];
  int         i, j;

  /* is 'to' parent of 'from' */
  parentP = from;
  while ( parentP &&
	  (parentP != to || (startover >= 0 && parentP->cfgAP ))) {
    titemA = (CFGRSItem**)realloc (titemA, (titemN + 1) * sizeof (CFGRSItem*));

    titemA[titemN++] = parentP;
    parentP          = parentP->parentP;
  }

  if ( parentP || startover >= 0 ) {
    titemA = (CFGRSItem**)realloc (titemA, (titemN + 1) * sizeof (CFGRSItem*));

    titemA[titemN++] = parentP;

    for (i=0; i<titemN-1; i++) {
      cfgAP = titemA[i]->cfgAP;

      if ( cfgAP && (auxNT == 1 || cfgAP->cfgRP->type != Aux_Rule) )
	itfAppendResult (") ");
    }

    if ( startover < 0 ) {
      if ( fitemA ) Nulle (fitemA);
      if ( titemA ) Nulle (titemA);
      return TCL_OK;
    }
  }

  /* is 'from' parent of 'to' */
  parentP = to;
  while ( parentP &&
	  (parentP != from || (startover >= 0 && parentP->cfgAP )) ) {
    fitemA = (CFGRSItem**)realloc (fitemA, (fitemN + 1) * sizeof (CFGRSItem*));

    fitemA[fitemN++] = parentP;
    parentP          = parentP->parentP;
  }

  if ( parentP || startover >= 0 ) {
    fitemA = (CFGRSItem**)realloc (fitemA, (fitemN + 1) * sizeof (CFGRSItem*));

    fitemA[fitemN++] = parentP;

    /* 'to' != 'from' */
    if ( fitemN > 1 ) {
      for (i=fitemN-2; i>=0; i--) {
	cfgAP = fitemA[i]->cfgAP;

	if ( cfgAP && (auxNT == 1 || cfgAP->cfgRP->type != Aux_Rule) ) {
	  cfgP  = cfgAP->cfgRP->cfgP;
	  cfgRuleFormat (cfgP, cfgAP->cfgRP, f, nt);
	  itfAppendResult ("%s", nt);
	}
      }
    }

    if ( fitemA ) Nulle (fitemA);
    if ( titemA ) Nulle (titemA);
    return TCL_OK;
  }

  /* 'from' and 'to' both are having a same parent */
  i = 0;
  while ( i < fitemN && i < titemN &&
	  fitemA[fitemN-i-1] == titemA[titemN-i-1] ) i++;

  assert ( i < fitemN && i < titemN );

  for (j=0; j<titemN-i; j++) {
    assert (titemA[j]); // XCode says could be NULL
    cfgAP = titemA[j]->cfgAP;
    assert (cfgAP); // XCode says could be NULL
    if ( cfgAP->cfgRP->type != Root_Rule &&
	 (auxNT == 1 || cfgAP->cfgRP->type != Aux_Rule) )
      itfAppendResult (") ");
  }

  for (j=fitemN-i-1; j>=0; j--) {
    cfgAP = fitemA[j]->cfgAP;
    cfgP  = cfgAP->cfgRP->cfgP;

    if ( cfgAP->cfgRP->type != Root_Rule &&
	 (auxNT == 1 || cfgAP->cfgRP->type != Aux_Rule) ) {
      cfgRuleFormat (cfgP, cfgAP->cfgRP, f, nt);
      itfAppendResult ("%s", nt);
    }
  }

  if ( fitemA ) Nulle (fitemA);
  if ( titemA ) Nulle (titemA);
  return TCL_OK;
}

/* ----------------------------------------------------------------------
   cfgRuleStackSaveDump
   ---------------------------------------------------------------------- */
static CFG* getCFG (CFGArc *cfgAP) {

  CFGArcX i;

  assert (cfgAP->cfgNP && cfgAP->cfgNP->arcN);

  for (i=0; i<cfgAP->cfgNP->arcN; i++) {

    /* we need a terminal or lambda arc for getting the correct CFG */
    if ( cfgAP->cfgNP->arcA[i] &&
	 (cfgAP->cfgNP->arcA[i]->type & 0x3) != NT_Arc )
      return cfgAP->cfgNP->arcA[i]->cfgRP->cfgP;

    /* skip recursive arcs */
    if ( cfgAP->cfgNP->arcA[i] &&
	 !(cfgAP->cfgNP->arcA[i]->type & 0x4) )
      return getCFG (cfgAP->cfgNP->arcA[i]);
  }

  return NULL;
}

int cfgRuleStackSaveDump (CFGRuleStack *cfgRSP, FILE *fp, CFGSet *cfgSP) {

  CFGRSItem  *cfgRSIP = NULL;
  CFGRSItemX  itemN   = bmemGetItemN (cfgRSP->mem.items);
  CFG        *cfgP    = NULL;
  UINT        i;

  if ( !fp ) {
    ERROR ("cfgRuleStackSaveDump: invalid file pointer.\n");
    return TCL_ERROR;
  }

  write_string (fp, "CFG-RULESTACK-BEGIN 1.0");

  /* write items */
  write_int (fp, (int)itemN);
  for (i=0; i<itemN; i++) {
    cfgRSIP = cfgRuleStackGetItem (cfgRSP, i);
    cfgP    = NULL;

    /* ugly hack to get cfgP, if no root */
    if ( cfgRSIP != cfgRSP->root ) {
      cfgP = getCFG (cfgRSIP->cfgAP);
      assert (cfgP);
    }

    if ( cfgSP )
      write_int (fp, (int)cfgSetGetCFGX (cfgSP, cfgP));
    write_int (fp, (int)cfgGetArcX (cfgP, cfgRSIP->cfgAP));
  }

  /* write linking between them */
  for (i=0; i<itemN; i++) {
    cfgRSIP = cfgRuleStackGetItem (cfgRSP, i);
    write_int (fp, (int)cfgRuleStackGetItemX (cfgRSP, cfgRSIP->childP));
    write_int (fp, (int)cfgRuleStackGetItemX (cfgRSP, cfgRSIP->nextP));
    write_int (fp, (int)cfgRuleStackGetItemX (cfgRSP, cfgRSIP->parentP));
  }

  write_int (fp, (int)cfgRuleStackGetItemX (cfgRSP, cfgRSP->root));

  write_string (fp, "CFG-RULESTACK-END");

  return TCL_OK;
}

/* ----------------------------------------------------------------------
   cfgRuleStackLoadDump
   ---------------------------------------------------------------------- */
int cfgRuleStackLoadDump (CFGRuleStack *cfgRSP, FILE *fp, CFG *cfgP, CFGSet *cfgSP) {

  CFGRSItem  *cfgRSIP = NULL;
  CFGRSItemX  itemN;
  CFGArc     *cfgAP;
  char        s[CFG_MAXLINE];
  int         arcX, itemX, cfgX;
  UINT        i;

  if ( !fp ) {
    ERROR ("cfgRuleStackLoadDump: invalid file pointer.\n");
    return TCL_ERROR;
  }

  read_string (fp, s);
  if ( strcmp (s, "CFG-RULESTACK-BEGIN 1.0") ) {
    ERROR ("cfgRuleStackLoadDump: inconsistent file.\n");
    return TCL_ERROR;
  }

  /* read items */
  itemN = (CFGRSItemX)read_int (fp);
  for (i=0; i<itemN; i++) {
    if ( cfgSP ) {
      cfgX = read_int (fp);
      cfgP = cfgSetGetCFG (cfgSP, (CFGX)cfgX);
    }
    arcX  = read_int (fp);
    if ( !cfgP && (CFGArcX)arcX == CFGArcX_NIL )
      cfgAP = NULL;
    else
      cfgAP = cfgGetArc (cfgP, (CFGArcX)arcX);
    cfgRSItemCreate (cfgRSP, cfgAP, NULL);
  }

  /* read linking between them */
  for (i=0; i<itemN; i++) {
    cfgRSIP = cfgRuleStackGetItem (cfgRSP, i);

    itemX            = read_int (fp);
    cfgRSIP->childP  = cfgRuleStackGetItem (cfgRSP, (CFGRSItemX)itemX);
    itemX            = read_int (fp);
    cfgRSIP->nextP   = cfgRuleStackGetItem (cfgRSP, (CFGRSItemX)itemX);
    itemX            = read_int (fp);
    cfgRSIP->parentP = cfgRuleStackGetItem (cfgRSP, (CFGRSItemX)itemX);
  }

  itemX        = read_int (fp);
  cfgRSP->root = cfgRuleStackGetItem (cfgRSP, (CFGRSItemX)itemX);

  read_string (fp, s);
  if ( strcmp (s, "CFG-RULESTACK-END") ) {
    ERROR ("cfgRuleStackLoadDump: inconsistent file.\n");
    return TCL_ERROR;
  }

  return TCL_OK;
}


/****************************************************************************/

/* ========================================================================
    Type Information
   ======================================================================== */

/* ---------- itf types for CFGRSItem */
static Method cfgRSItemMethod[] = {
  { "puts",  cfgRSItemPutsItf, "display the contents of this item" },
  { NULL, NULL, NULL }
};

TypeInfo cfgRSItemInfo = { "CFGRSItem", 0, -1,
			   cfgRSItemMethod,
			   NULL, NULL,
			   cfgRSItemConfigureItf,
			   cfgRSItemAccessItf,
			   itfTypeCntlDefaultNoLink,
  "A 'CFGRSItem' object is an item in the stack of CFG rules." };


/* ---------- itf types for CFGRuleStack */
static Method cfgRuleStackMethod[] = {
  { "puts",  cfgRuleStackPutsItf, "display the contents of CFG rule stack" },
  { NULL, NULL, NULL }
};

TypeInfo cfgRuleStackInfo = { "CFGRuleStack", 0, -1, cfgRuleStackMethod,
			      NULL, NULL,
			      cfgRuleStackConfigureItf, cfgRuleStackAccessItf,
			      itfTypeCntlDefaultNoLink,
  "A 'CFGRuleStack' object is a stack of CFG rules." };


/* -------------------- CFGRuleStack_Init ---------- */

static int cfgRuleStackInitialized = 0;

int CFGRuleStack_Init (void) {

  if ( !cfgRuleStackInitialized ) {

    cfgRSItemDefault.cfgAP		= NULL;
    cfgRSItemDefault.childP		= NULL;
    cfgRSItemDefault.nextP		= NULL;
    cfgRSItemDefault.parentP		= NULL;

    cfgRuleStackDefault.mem.items	= NULL;
    cfgRuleStackDefault.root		= NULL;

    itfNewType (&cfgRSItemInfo);
    itfNewType (&cfgRuleStackInfo);

    cfgRuleStackInitialized = 1;
  }

  return TCL_OK;
}

