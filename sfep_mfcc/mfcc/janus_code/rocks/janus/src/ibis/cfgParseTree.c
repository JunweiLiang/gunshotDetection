/* ========================================================================
    JANUS      Speech- to Speech Translation
               ------------------------------------------------------------
               Object: Context Free Grammar Parse Tree
               ------------------------------------------------------------

    Author  :  Christian Fuegen
    Module  :  cfgParseTree.c
    Date    :  $Id: cfgParseTree.c 3416 2011-03-23 03:02:18Z metze $
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
    Revision 4.6  2004/10/26 11:52:33  fuegen
    cfgParseTreeClear now deinits nodes before freeing memory

    Revision 4.5  2004/09/23 11:59:57  fuegen
    Made code 'x86-64'-clean
    Necessary changes include the usage of functions
    instead of macros for LCT->node/LCT->lvX conversion
    together with a union

    Revision 4.4  2004/07/23 17:25:32  fuegen
    - Extended supported grammar definition file formats:
       - FSM:  AT&T's FSM (finite state machine) text file format
       - PFSG: Probabilistic Finite State Graph format used by the
               SRI-LM toolkit
    - Added support for reading weights specified in the JSGF format
    - Build functionality of grammars expanded by a mode for making
      equally distributed transitions instead of using fixed scores
    - Added support for generating terminal sequences for specific
      rules either randomly or fixed

    Revision 4.3  2004/03/05 14:23:20  fuegen
    removed bug with score and offset computation

    Revision 4.2  2003/08/18 14:39:35  fuegen
    added nodeN option to cfgParseTreeConfigure

    Revision 4.1  2003/08/14 11:19:59  fuegen
    Merged changes on branch jtk-01-01-15-fms (jaguar -> ibis-013)

    Revision 1.1.2.26  2003/08/13 08:53:58  fuegen
    startover requires now float factor
    removed bug in cfgPTNodeScore
    added cfgParseTreeTraceItf to get parse tree via spass

    Revision 1.1.2.25  2003/07/25 09:48:22  fuegen
    removed fprintf in cfgPTNodeTrace

    Revision 1.1.2.24  2003/07/15 17:02:04  fuegen
    some more changes
      - added forward/recursive arc types
      - added dumping of CFGs/CFGSets (together with pt/rs)
      - added compression/determinization of CFGs/CFGSets
      - rule stack belongs no more to a single grammar
      - changed LingKS load/save interface, added LingKSPutsFct

    Revision 1.1.2.23  2003/07/02 17:20:43  fuegen
    some minor code cleaning

    Revision 1.1.2.22  2003/06/26 14:32:53  fuegen
    typos

    Revision 1.1.2.21  2003/06/26 14:31:17  fuegen
    added PTItemPuts

    Revision 1.1.2.20  2003/06/23 13:12:07  fuegen
    new CFG implementation
      - less memory usage
      - activation/deactivation down to rule level
      - adding of paths, grammar rules, grammars on the fly
      - basic JSGF grammar support
      - additional support of CFG as LingKS type
      - standalone support of CFG/CFGSet, when using only for e.g. parsing

    Revision 1.1.2.19  2002/11/21 17:10:08  fuegen
    windows code cleaning

    Revision 1.1.2.18  2002/11/19 08:31:14  fuegen
    some minor bugfixes in cfgParseTreeReset

    Revision 1.1.2.17  2002/11/18 15:13:30  fuegen
    beautification

    Revision 1.1.2.16  2002/08/26 11:26:37  fuegen
    Removed bugs in decoding along parallel grammars and
    with shared grammars.
    Added output of the parse tree.
    Added reading of grammar files with windows line endings.

    Revision 1.1.2.15  2002/06/14 08:52:04  fuegen
    added a more flexible FSGraph traversing (removed old extend)

    Revision 1.1.2.14  2002/06/10 11:36:12  metze
    Go away, boring error message

    Revision 1.1.2.13  2002/06/06 10:02:31  fuegen
    code cleaning, reorganisation of configurable options

    Revision 1.1.2.12  2002/05/21 15:25:45  fuegen
    added cfgTNSetScore function, made gcc clean

    Revision 1.1.2.11  2002/04/18 09:20:14  fuegen
    made gcc clean

    Revision 1.1.2.10  2002/04/18 09:30:50  fuegen
    major changes in interface and main routines, handling of shared grammars, using of CFGExt

    Revision 1.1.2.9  2002/02/19 16:33:57  fuegen
    added training procedures

    Revision 1.1.2.8  2002/02/06 13:20:56  fuegen
    changed memory management (bmem)

    Revision 1.1.2.7  2002/01/30 16:57:17  fuegen
    changed LingKS interface, added LCT cache

    Revision 1.1.2.6  2002/01/29 17:20:32  fuegen
    made gcc clean, removed rootSetX-Bug for extend

    Revision 1.1.2.5  2002/01/25 17:16:28  fuegen
    added handling of starting new trees, added handling of shared grammars, added some minor things for saving memory

    Revision 1.1.2.4  2002/01/25 17:10:21  fuegen
    first real working version (only parsing)

    Revision 1.1.2.3  2002/01/25 17:02:36  fuegen
    extended parse tree implementation

    Revision 1.1.2.2  2002/01/25 16:53:27  fuegen
    major and minor changes: lexicon as cache, extension routines, lingKS interface

    Revision 1.1.2.1  2002/01/25 16:38:55  fuegen
    reimplementation of a big part, using now indices for memory saving reasons, splitted CFG-implementation in smaller modules


 
   ======================================================================== */

#include "common.h"
#include "itf.h"
#include "cfg.h"
#include "spass.h"

/* =======================================================================
   Forward Declarations
   ======================================================================= */

extern TypeInfo			cfgPTItemInfo;
extern TypeInfo			cfgPTNodeInfo;
extern TypeInfo			cfgParseTreeInfo;

static CFGPTItem		cfgPTItemDefault;
static CFGPTNode		cfgPTNodeDefault;
static CFGParseTree		cfgParseTreeDefault;

extern CFGPTNode* cfgParseTreeNewNode (CFGParseTree *cfgPTP);


/* ======================================================================
   Context Free Grammar Parse Tree Node
   ====================================================================== */

/* ----------------------------------------------------------------------
   Init/Create Context Free Grammar Parse Tree Node Object
   ---------------------------------------------------------------------- */
int cfgPTNodeInit (CFGPTNode *cfgPTNP) {

  cfgPTNP->lvX       = cfgPTNodeDefault.lvX;
  cfgPTNP->itemA     = cfgPTNodeDefault.itemA;
  cfgPTNP->itemN     = cfgPTNodeDefault.itemN;
  cfgPTNP->bestX     = cfgPTNodeDefault.bestX;
  cfgPTNP->bestScore = cfgPTNodeDefault.bestScore;
  cfgPTNP->childP    = cfgPTNodeDefault.childP;
  cfgPTNP->nextP     = cfgPTNodeDefault.nextP;
  cfgPTNP->parentP   = cfgPTNodeDefault.parentP;

  return TCL_OK;
}

int cfgPTNodeAlloc (CFGPTNode *cfgPTNP, LVX lvX, CFGPTNode *parentP) {

  cfgPTNP->lvX = lvX;

  if ( parentP ) {
    cfgPTNP->parentP = parentP;
    cfgPTNP->nextP   = parentP->childP;
    parentP->childP  = cfgPTNP;
  }

  return TCL_OK;
}

CFGPTNode* cfgPTNodeCreate (CFGParseTree *cfgPTP, LVX lvX, CFGPTNode *parentP) {

  CFGPTNode *cfgPTNP = cfgParseTreeNewNode (cfgPTP);

  if ( !cfgPTNP || cfgPTNodeInit (cfgPTNP) != TCL_OK ) {
    if ( cfgPTNP ) free (cfgPTNP);
    ERROR ("Cannot create CFGPTNode object\n");
    return NULL;
  }

  cfgPTNodeAlloc (cfgPTNP, lvX, parentP);

  return cfgPTNP;
}

/* ----------------------------------------------------------------------
   Deinit/Free Context Free Grammar Parse Tree Node Object
   ---------------------------------------------------------------------- */
int cfgPTNodeDeinit (CFGPTNode *cfgPTNP) {

  if ( cfgPTNP->itemN ) Nulle (cfgPTNP->itemA);
  
  /* START FF HACK */
  memset(cfgPTNP, 0, sizeof(CFGPTNode));
  /* END FF HACK */

  /* set all to default */
  return cfgPTNodeInit (cfgPTNP);
}

int cfgPTNodeFree (CFGPTNode *cfgPTNP) {

  if ( cfgPTNodeDeinit (cfgPTNP) != TCL_OK ) return TCL_ERROR;
  /* !!! START FF HACK !!! */
  /* DON'T FREE THIS WAY !!! THE ROOT WAS ALLOCATED WITH BMEM !!! */
  /* if ( cfgPTNP ) free (cfgPTNP); */
  /* !!! END FF HACK !!! */

  return TCL_OK;
}

/* ----------------------------------------------------------------------
   Configure/Access Context Free Grammar Parse Tree Node Object
   ---------------------------------------------------------------------- */
int cfgPTNodeConfigureItf (ClientData cd, char *var, char *val) {

  CFGPTNode *cfgPTNP = (CFGPTNode*)cd;

  if ( !cfgPTNP ) cfgPTNP = &cfgPTNodeDefault;

  if ( var == NULL ) {
    ITFCFG (cfgPTNodeConfigureItf, cd, "lvX");
    ITFCFG (cfgPTNodeConfigureItf, cd, "itemN");
    ITFCFG (cfgPTNodeConfigureItf, cd, "bestX");
    ITFCFG (cfgPTNodeConfigureItf, cd, "bestScore");
    return TCL_OK;
  }

  ITFCFG_Int    (var, val, "lvX",   cfgPTNP->lvX,   1);
  ITFCFG_Int    (var, val, "itemN", cfgPTNP->itemN, 1);
  ITFCFG_Int    (var, val, "bestX", cfgPTNP->bestX, 1);

  if ( !strcmp (var, "bestScore") ) {
    if ( !val ) {
      itfAppendElement ("%f", UnCompressScore (cfgPTNP->bestScore));
      return TCL_OK;
    }
    return TCL_OK;
  }

  ERROR("unknown option '-%s %s'.\n", var, val ? val : "?");
  
  return TCL_ERROR;
}

ClientData cfgPTNodeAccessItf (ClientData cd, char *name, TypeInfo **ti) {

  CFGPTNode *cfgPTNP = (CFGPTNode*)cd;
  int        i;

  if ( *name == '.' ) {
    if ( name[1] == '\0' ) {
      itfAppendElement ("item(0..%d)", cfgPTNP->itemN-1);
      itfAppendElement ("child");
      itfAppendElement ("next");
      itfAppendElement ("parent");
    } else {
      if ( sscanf (name+1, "item(%d)", &i) == 1 ) {
	*ti = itfGetType ("CFGPTItem");
	if ( i >= 0 && i < (int)cfgPTNP->itemN )
	  return (ClientData)&(cfgPTNP->itemA[i]);
	else return NULL;
      }
      if ( !strcmp(name+1, "child") ) {
	*ti = itfGetType ("CFGPTNode");
	return (ClientData)cfgPTNP->childP;
      }
      if ( !strcmp(name+1, "next") ) {
	*ti = itfGetType ("CFGPTNode");
	return (ClientData)cfgPTNP->nextP;
      }
      if ( !strcmp(name+1, "parent") ) {
	*ti = itfGetType ("CFGPTNode");
	return (ClientData)cfgPTNP->parentP;
      }
    }
  }

  *ti = NULL;
  return NULL;
}

/* PTItem configure/access */
int cfgPTItemConfigureItf (ClientData cd, char *var, char *val) {

  CFGPTItem *cfgPTIP = (CFGPTItem*)cd;

  if ( !cfgPTIP ) cfgPTIP = &cfgPTItemDefault;

  if ( var == NULL ) {
    ITFCFG (cfgPTItemConfigureItf, cd, "offset");
    ITFCFG (cfgPTItemConfigureItf, cd, "parentX");
    return TCL_OK;
  }

  ITFCFG_Int    (var, val, "parentX",   cfgPTIP->parentX,   1);

  if ( !strcmp (var, "offset") ) {
    if ( !val ) {
      itfAppendElement ("%f", UnCompressScore (cfgPTIP->offset));
      return TCL_OK;
    }
    return TCL_OK;
  }

  ERROR("unknown option '-%s %s'.\n", var, val ? val : "?");
  
  return TCL_ERROR;
}

ClientData cfgPTItemAccessItf (ClientData cd, char *name, TypeInfo **ti) {

  CFGPTItem *cfgPTIP = (CFGPTItem*)cd;

  if ( *name == '.' ) {
    if ( name[1] == '\0' ) {
      itfAppendElement ("arc");
      itfAppendElement ("rsitem");
    } else {
      if ( !strcmp(name+1, "arc") ) {
	*ti = itfGetType ("CFGArc");
	return (ClientData)cfgPTIP->cfgAP;
      }
      if ( !strcmp(name+1, "rsitem") ) {
	*ti = itfGetType ("CFGRSItem");
	return (ClientData)cfgPTIP->cfgRSIP;
      }
    }
  }

  *ti = NULL;
  return NULL;
}

/* ----------------------------------------------------------------------
   cfgPTItemPutsItf
   ---------------------------------------------------------------------- */
int cfgPTItemPuts (CFGPTItem *cfgPTIP, CFGPutsFormat f) {

  switch (f) {
  case Short_Puts:
  case Long_Puts:
    itfAppendResult ("%f", UnCompressScore (cfgPTIP->offset));
    return TCL_OK;
  }

  ERROR ("Not supported or unknown PutsFormat %d=%s", f, cfgPutsFormat2str(f));

  return TCL_ERROR;
}

static int cfgPTItemPutsItf (ClientData cd, int argc, char *argv[]) {

  CFGPTItem *cfgPTIP = (CFGPTItem*)cd;
  char      *format  = "short";

  argc--;
  if ( itfParseArgv (argv[0], &argc, argv+1, 0,
 		     "-format", ARGV_STRING, NULL, &format, NULL,
		     "output format (SHORT, LONG)",
		     NULL) != TCL_OK )
    return TCL_ERROR;

  return cfgPTItemPuts (cfgPTIP, cfgPutsFormat2int (format));
}

/* ----------------------------------------------------------------------
   cfgPTNodePutsItf
   ---------------------------------------------------------------------- */
int cfgPTNodePuts (CFGPTNode *cfgPTNP, CFGPutsFormat f) {

  CFG *cfgP = NULL;

  switch (f) {
  case Short_Puts:
    if ( cfgPTNP->itemN ) {
      cfgP = cfgPTNP->itemA[0].cfgAP->cfgRP->cfgP;
      itfAppendResult ("%s",
		       cfgLexiconGetName (cfgP->cfgLP, cfgPTNP->lvX, T_Arc));
    }
    return TCL_OK;
  case Long_Puts:
    return TCL_OK;
  }

  ERROR ("Not supported or unknown PutsFormat %d=%s", f, cfgPutsFormat2str(f));

  return TCL_ERROR;
}

static int cfgPTNodePutsItf (ClientData cd, int argc, char *argv[]) {

  CFGPTNode *cfgPTNP = (CFGPTNode*)cd;
  char      *format  = "short";

  argc--;
  if ( itfParseArgv (argv[0], &argc, argv+1, 0,
 		     "-format", ARGV_STRING, NULL, &format, NULL,
		     "output format (SHORT, LONG)",
		     NULL) != TCL_OK )
    return TCL_ERROR;

  return cfgPTNodePuts (cfgPTNP, cfgPutsFormat2int (format));
}

/* ----------------------------------------------------------------------
   cfgPTNodeAddItem
   ---------------------------------------------------------------------- */
CFGPTItem* cfgPTNodeAddItem (CFGPTNode *cfgPTNP, CFGArc *cfgAP, CFGRSItem *cfgRSIP, CFGScore score, CFGPTItemX parentX, float startover, int check) {

  CFGPTItemX i;

  /* check if right PTNode */
  if ( cfgPTNP->lvX != cfgAP->lvX ) {
    WARN ("cfgPTNodeAddItem: wrong lvX (%d != %d).\n", cfgPTNP->lvX, cfgAP->lvX);
    return NULL;
  }

  /* check if item already exists */
  if ( check ) {
    for (i=0; i<cfgPTNP->itemN; i++) {
      if (cfgPTNP->itemA[i].cfgAP     == cfgAP   &&
	  cfgPTNP->itemA[i].cfgRSIP   == cfgRSIP &&
	  cfgPTNP->itemA[i].parentX   == parentX &&
	  cfgPTNP->itemA[i].startover == startover ) return &(cfgPTNP->itemA[i]);
    }
  }

  /* add item */
  if ( cfgPTNP->itemN == CFGPTItemX_MAX ) {
    ERROR ("cfgPTNodeAddItem: max. number of items (CFGPTItemX_MAX=%d) exceeded.\n",
	   CFGPTItemX_MAX);
    return NULL;
  }

  cfgPTNP->itemA = (CFGPTItem*)realloc (cfgPTNP->itemA,
					(cfgPTNP->itemN+1) * sizeof(CFGPTItem));

  cfgPTNP->itemA[cfgPTNP->itemN].cfgAP     = cfgAP;
  cfgPTNP->itemA[cfgPTNP->itemN].cfgRSIP   = cfgRSIP;
  cfgPTNP->itemA[cfgPTNP->itemN].offset    = CFG_NULL_SCORE;
  cfgPTNP->itemA[cfgPTNP->itemN].parentX   = parentX;
  cfgPTNP->itemA[cfgPTNP->itemN].startover = startover;

  /* check scores (we have neg. log probs) */
  if ( score > cfgPTNP->bestScore ) {
    CFGScore o = cfgPTNP->bestScore - score;

    for (i=0; i<cfgPTNP->itemN; i++) cfgPTNP->itemA[i].offset += o;
    cfgPTNP->bestScore = score;
    cfgPTNP->bestX     = cfgPTNP->itemN;

  } else {
    cfgPTNP->itemA[cfgPTNP->itemN].offset = score - cfgPTNP->bestScore;
  }

  cfgPTNP->itemN++;

  return &(cfgPTNP->itemA[cfgPTNP->itemN-1]);
}

/* ----------------------------------------------------------------------
   cfgPTNodeTrace
   ---------------------------------------------------------------------- */
int cfgPTNodeTrace (CFGPTNode *cfgPTNP, int topN, int auxNT, CFGGrammarFormat f) {

  CFGPTNode  **nodeA  = NULL;
  CFGPTItemX  *itemXA = NULL;
  int          nodeN  = 0;
  CFGPTNode   *ptnp   = cfgPTNP;
  CFGScore     score  = CFG_NULL_SCORE;
  CFG         *cfgP;
  CFGArc      *cfgAP;
  int          i;

  if ( topN == 1 ) {
    /* collect nodes */
    while ( ptnp ) {
      nodeA  = (CFGPTNode**)realloc (nodeA,
				     (nodeN + 1) * sizeof (CFGPTNode*));
      itemXA = (CFGPTItemX*)realloc (itemXA,
				     (nodeN + 1) * sizeof (CFGPTItemX));
      nodeA[nodeN] = ptnp;

      if ( nodeN == 0 ) {
	itemXA[nodeN] = ptnp->bestX;
	score         = ptnp->bestScore + ptnp->itemA[itemXA[nodeN]].offset;
      } else {
	itemXA[nodeN] = nodeA[nodeN-1]->itemA[itemXA[nodeN-1]].parentX;
	score        += ptnp->bestScore + ptnp->itemA[itemXA[nodeN]].offset;
      }
      ptnp = ptnp->parentP;
      nodeN++;
    }
  } else {
    ERROR ("cfgPTNodeTrace: topN > 1 currently not implemented.\n");
    Nulle (nodeA); Nulle (itemXA);
    return TCL_ERROR;
  }

  /* trace
     at i=nodeN-1 there is a Root_Rule, so start at nodeN-2
     at i=0       there is a Root_Rule, so only trace rule stack */
  itfAppendResult ("%f ", UnCompressScore (score));
  for (i=nodeN-2; i>0; i--) {
    /* print only best path */
    cfgAP     = nodeA[i]->itemA[itemXA[i]].cfgAP;
    cfgP      = cfgAP->cfgRP->cfgP;

    /* print rule stack */
    cfgRuleStackTrace (nodeA[i+1]->itemA[itemXA[i+1]].cfgRSIP,
		       nodeA[i]->itemA[itemXA[i]].cfgRSIP,
		       nodeA[i]->itemA[itemXA[i]].startover,
		       auxNT, f);

    /* print terminal */
    itfAppendResult ("%s ", 
		     cfgLexiconGetName (cfgP->cfgLP, nodeA[i]->lvX, T_Arc));
  }

  /* process last, i.e. i=1 */
  assert (nodeA); // XCode says could be NULL
  cfgRuleStackTrace (nodeA[i+1]->itemA[itemXA[i+1]].cfgRSIP,
		     nodeA[i]->itemA[itemXA[i]].cfgRSIP,
		     nodeA[i]->itemA[itemXA[i]].startover,
		     auxNT, f);

  Nulle (nodeA);
  return TCL_OK;
}

static int cfgPTNodeTraceItf (ClientData cd, int argc, char *argv[]) {

  CFGPTNode *cfgPTNP = (CFGPTNode*)cd;
  char      *format  = "jsgf";
  int        topN    = 1;
  int        auxNT   = 0;

  argc--;
  if ( itfParseArgv (argv[0], &argc, argv+1, 0,
 		     "-auxNT",  ARGV_INT,    NULL, &auxNT,  NULL,
		     "print also auxilliary NTs",
 		     "-topN",   ARGV_INT,    NULL, &topN,   NULL,
		     "print the topN parse trees",
 		     "-format", ARGV_STRING, NULL, &format, NULL,
		     "output format (jsgf, soup)",
		     NULL) != TCL_OK )
    return TCL_ERROR;

  return cfgPTNodeTrace (cfgPTNP, topN, auxNT, cfgGrammarFormat2int (format));
}

/* ----------------------------------------------------------------------
   cfgPTNodeExtend
   ---------------------------------------------------------------------- */
int cfgPTNodeDo (CFGTraverse *cfgTP, CFGNode *cfgNP, CFGArc *cfgAP, CFGRSItem *cfgRSIP, CFGScore offset) {

  if ( !cfgRSIP ) return TCL_OK;

  return cfgTraverse (cfgAP->cfgRP->cfgP, cfgAP->cfgNP, cfgRSIP, offset, cfgTP);
}

int cfgPTNodeDoLambda (CFGTraverse *cfgTP, CFGNode *cfgNP, CFGArc *cfgAP, CFGRSItem *cfgRSIP, CFGScore offset) {

  CFG *cfgP = cfgAP->cfgRP->cfgP;
  int  res;

  if ( cfgNP        == cfgP->eosNP &&
       cfgAP->cfgNP == cfgP->bosNP && cfgP->startover < 0 )
    return TCL_OK;

  if ( cfgNP        == cfgP->eosNP &&
       cfgAP->cfgNP == cfgP->bosNP )
    cfgTP->startover = cfgP->startover;

  if ( cfgTP->startover > 0 ) offset *= cfgTP->startover;
  res     = cfgTraverse (cfgAP->cfgRP->cfgP, cfgAP->cfgNP, cfgRSIP, offset, cfgTP);

  if ( cfgNP        == cfgP->eosNP &&
       cfgAP->cfgNP == cfgP->bosNP )
    cfgTP->startover = -1.0;

  return res;
}

int cfgPTNodeDoNT (CFGTraverse *cfgTP, CFGNode *cfgNP, CFGArc *cfgAP, CFGRSItem *cfgRSIP, CFGScore offset) {

  /* check for activity */
  if ( cfgAP->cfgRP->status       != Inactive && 
       cfgAP->cfgRP->cfgP->status != Inactive ) {
    if ( cfgTP->verbose > 2 )
      fprintf (stderr, "cfgPTNodeDoNT: accessing rule '%s'.\n",
	      cfgLexiconGetName (cfgAP->cfgRP->cfgP->cfgLP, cfgAP->cfgRP->lvX, NT_Arc));

    return cfgTraverse (cfgAP->cfgRP->cfgP, cfgAP->cfgRP->root, cfgRSIP, offset, cfgTP);
  }

  return TCL_OK;
}

int cfgPTNodeExtendDoT (CFGTraverse *cfgTP, CFGNode *cfgNP, CFGArc *cfgAP, CFGRSItem *cfgRSIP, CFGScore offset) {

  CFG *cfgP = cfgAP->cfgRP->cfgP;

  /* check for right lvX */
  if ( cfgTP->lvX != cfgAP->lvX ) return TCL_OK;

  /* add child node, if not already exist */
  if ( !cfgTP->childPTNP )
    cfgTP->childPTNP = cfgPTNodeCreate (cfgP->cfgPTP, cfgTP->lvX, cfgTP->cfgPTNP);

  /* add new item */
  cfgPTNodeAddItem (cfgTP->childPTNP, cfgAP, cfgRSIP, offset, cfgTP->ptitemX, cfgTP->startover, 1);

  return TCL_OK;
}

CFGPTNode* cfgPTNodeExtend (CFGPTNode *cfgPTNP, LVX lvX) {

  CFGPTItem   *cfgPTIP;
  CFGPTItemX   i;
  CFGPTNode   *childPTNP = NULL;
  CFGTraverse *cfgTP     = NULL;

  /* --------------------------------------------------
     check if we have a valid node
     -------------------------------------------------- */
  if ( !cfgPTNP ) return NULL;

  /* --------------------------------------------------
     check for root node and bos lvX
     -------------------------------------------------- */
  if ( cfgPTNP->parentP == NULL && cfgPTNP->lvX == lvX ) return cfgPTNP;

  /* --------------------------------------------------
     check cache, assuming no activity change
     -------------------------------------------------- */

  /* --------------------------------------------------
     check if child already exists, assuming no activity change
     -------------------------------------------------- */
  childPTNP = cfgPTNP->childP;
  while ( childPTNP && childPTNP->lvX != lvX ) childPTNP = childPTNP->nextP;

  if ( childPTNP ) return childPTNP;

  /* --------------------------------------------------
     child does not exists, so start traversing
     -------------------------------------------------- */

  /* init cfgTraverse */
  cfgTP              = cfgTraverseCreate ();
  cfgTP->cfgPTNP     = cfgPTNP;
  cfgTP->childPTNP   = NULL;
  cfgTP->lvX         = lvX;
  cfgTP->doTArc      = cfgPTNodeExtendDoT;
  cfgTP->doNTArc     = cfgPTNodeDoNT;
  cfgTP->doLambdaArc = cfgPTNodeDoLambda;
  cfgTP->doLeafNode  = cfgPTNodeDo;

  /* extend all items */
  for (i=0; i<cfgPTNP->itemN; i++) {
    cfgPTIP = &(cfgPTNP->itemA[i]);

    cfgTP->ptitemX = i;

    /* check for activity */
    if ( cfgPTIP->cfgAP->cfgRP->status       != Inactive && 
	 cfgPTIP->cfgAP->cfgRP->cfgP->status != Inactive ) {
      cfgTraverse (cfgPTIP->cfgAP->cfgRP->cfgP, cfgPTIP->cfgAP->cfgNP,
		   cfgPTIP->cfgRSIP, cfgPTIP->offset, cfgTP);
    }
  }

  childPTNP = cfgTP->childPTNP;
  cfgTraverseFree (cfgTP);

  return childPTNP;
}

/* ----------------------------------------------------------------------
   cfgPTNodeScoreArray
   ---------------------------------------------------------------------- */
int cfgPTNodeScoreDoT (CFGTraverse *cfgTP, CFGNode *cfgNP, CFGArc *cfgAP, CFGRSItem *cfgRSIP, CFGScore offset) {

  /* check for right lvX */
  if ( cfgTP->lvX != cfgAP->lvX ) return TCL_OK;

  /* offset = score to reach cfgAP */
  if ( offset > cfgTP->score ) cfgTP->score = offset;

  return TCL_OK;
}

CFGScore cfgPTNodeScore (CFGPTNode *cfgPTNP, CFGParseTree *cfgPTP, LVX lvX) {

  CFGPTItem   *cfgPTIP;
  CFGPTItemX   i;
  CFGTraverse *cfgTP     = NULL;
  CFGScore     score;

  /* --------------------------------------------------
     check if we have a valid node
     -------------------------------------------------- */
  if ( !cfgPTNP ) return TCL_ERROR;

  /* --------------------------------------------------
     check cache, assuming no activity change
     -------------------------------------------------- */

  /* --------------------------------------------------
     child does not exists, so start traversing
     -------------------------------------------------- */

  /* init cfgTraverse */
  cfgTP              = cfgTraverseCreate ();
  cfgTP->cfgPTNP     = cfgPTNP;
  cfgTP->lvX         = lvX;
  cfgTP->score       = CFG_MAX_SCORE;
  cfgTP->doTArc      = cfgPTNodeScoreDoT;
  cfgTP->doNTArc     = cfgPTNodeDoNT;
  cfgTP->doLambdaArc = cfgPTNodeDoLambda;
  cfgTP->doLeafNode  = cfgPTNodeDo;

  /* extend all items */
  for (i=0; i<cfgPTNP->itemN; i++) {
    cfgPTIP = &(cfgPTNP->itemA[i]);

    cfgTP->ptitemX = i;

    /* check for activity */
    if ( cfgPTIP->cfgAP->cfgRP->status       != Inactive && 
	 cfgPTIP->cfgAP->cfgRP->cfgP->status != Inactive ) {
      cfgTraverse (cfgPTIP->cfgAP->cfgRP->cfgP, cfgPTIP->cfgAP->cfgNP,
		   cfgPTIP->cfgRSIP, cfgPTIP->offset, cfgTP);
    }
  }

  score = cfgTP->score;

  cfgTraverseFree (cfgTP);

  return score;
}

/* ----------------------------------------------------------------------
   cfgPTNodeScoreArray
   ---------------------------------------------------------------------- */
int cfgPTNodeScoreArrayDoT (CFGTraverse *cfgTP, CFGNode *cfgNP, CFGArc *cfgAP, CFGRSItem *cfgRSIP, CFGScore offset) {

  /* offset = score to reach cfgAP */
  if ( offset > cfgTP->scoreA[cfgAP->lvX] )
    cfgTP->scoreA[cfgAP->lvX] = offset;

  return TCL_OK;
}

int cfgPTNodeScoreArray (CFGPTNode *cfgPTNP, CFGParseTree *cfgPTP, CFGScore *scoreA) {

  CFGPTItem   *cfgPTIP;
  CFGPTItemX   i;
  CFGTraverse *cfgTP     = NULL;

  /* --------------------------------------------------
     check if we have a valid node
     -------------------------------------------------- */
  if ( !cfgPTNP ) return TCL_ERROR;

  /* --------------------------------------------------
     check cache, assuming no activity change
     -------------------------------------------------- */

  /* --------------------------------------------------
     child does not exists, so start traversing
     -------------------------------------------------- */

  /* init scoreA */
  memcpy (scoreA, cfgPTP->initScoreA,
	  cfgLexiconGetItemN (cfgPTP->cfgLP, T_Arc) * sizeof (CFGScore));

  /* init cfgTraverse */
  cfgTP              = cfgTraverseCreate ();
  cfgTP->cfgPTNP     = cfgPTNP;
  cfgTP->scoreA      = scoreA;
  cfgTP->doTArc      = cfgPTNodeScoreArrayDoT;
  cfgTP->doNTArc     = cfgPTNodeDoNT;
  cfgTP->doLambdaArc = cfgPTNodeDoLambda;
  cfgTP->doLeafNode  = cfgPTNodeDo;

  /* extend all items */
  for (i=0; i<cfgPTNP->itemN; i++) {
    cfgPTIP = &(cfgPTNP->itemA[i]);

    cfgTP->ptitemX = i;

    /* check for activity */
    if ( cfgPTIP->cfgAP->cfgRP->status       != Inactive && 
	 cfgPTIP->cfgAP->cfgRP->cfgP->status != Inactive ) {
      cfgTraverse (cfgPTIP->cfgAP->cfgRP->cfgP, cfgPTIP->cfgAP->cfgNP,
		   cfgPTIP->cfgRSIP, cfgPTIP->offset, cfgTP);
    }
  }

  cfgTraverseFree (cfgTP);

  return TCL_OK;
}

/* ----------------------------------------------------------------------
   cfgPTNodeCompress
   ---------------------------------------------------------------------- */
int cfgPTNodeCompressDoT (CFGTraverse *cfgTP, CFGNode *cfgNP, CFGArc *cfgAP, CFGRSItem *cfgRSIP, CFGScore offset) {

  CFG       *cfgP    = cfgAP->cfgRP->cfgP;
  CFGPTNode *cfgPTNP = NULL;

  if ( cfgTP->lvXA && cfgTP->lvXA[cfgAP->lvX] == 0 ) return TCL_OK;

  /* get matching node */
  if ( cfgTP->cacheA[cfgAP->lvX] != NULL )
    cfgPTNP = (CFGPTNode*)cfgTP->cacheA[cfgAP->lvX];

  /* does it exist? */
  if ( !cfgPTNP ) {
    cfgPTNP = cfgPTNodeCreate (cfgP->cfgPTP, cfgAP->lvX, cfgTP->cfgPTNP);
    cfgTP->cacheA[cfgAP->lvX] = cfgPTNP;
  }

  assert ( cfgPTNP->parentP == cfgTP->cfgPTNP );

  /* add new item */
  cfgPTNodeAddItem (cfgPTNP, cfgAP, cfgRSIP, offset, cfgTP->ptitemX, cfgTP->startover, 0);

  if ( cfgTP->verbose > 2 )
    fprintf (stderr, "cfgPTNodeCompressDoT: add T '%s' (%d).\n",
	    cfgLexiconGetName (cfgP->cfgLP, cfgAP->lvX, T_Arc),
	    cfgPTNP->itemN);


  return TCL_OK;
}

int cfgPTNodeCompress (CFGPTNode *cfgPTNP, CFGTraverse *cfgTP, int verbose) {

  CFG        *cfgP = cfgTP->cfgP;
  CFGPTItem  *cfgPTIP;
  CFGPTItemX  i;

  cfgTP->cfgPTNP     = cfgPTNP;
  cfgTP->doTArc      = cfgPTNodeCompressDoT;
  cfgTP->doNTArc     = cfgPTNodeDoNT;
  cfgTP->doLambdaArc = cfgPTNodeDoLambda;
  cfgTP->doLeafNode  = cfgPTNodeDo;
  cfgTP->cfgP        = NULL;
  cfgTP->verbose     = verbose;

  for (i=0; i<cfgPTNP->itemN; i++) {
    cfgPTIP = &(cfgPTNP->itemA[i]);

    cfgTP->ptitemX = i;

    if ( !cfgP || cfgPTIP->cfgAP->cfgRP->cfgP == cfgP )
      cfgTraverse (cfgPTIP->cfgAP->cfgRP->cfgP, cfgPTIP->cfgAP->cfgNP,
		   cfgPTIP->cfgRSIP, cfgPTIP->offset, cfgTP);
  }

  return TCL_OK;
}


/* ----------------------------------------------------------------------
   cfgPTNodeCompressBuild
   ---------------------------------------------------------------------- */
int cfgPTNodeCompressBuildDo (CFGTraverse *cfgTP, CFGNode *cfgNP, CFGArc *cfgAP, CFGRSItem *cfgRSIP, CFGScore offset) {

  CFGArc     *arc;
  CFGNode    *node;
  CFGArcType  type = cfgAP->type & 0x3;

  /* arc belongs to Root Rule, i.e. eos arc => lambda arc to leaf node */
  if ( cfgAP->cfgRP->type == Root_Rule ) {
    arc = cfgNodeAddArc (cfgTP->cfgNP, cfgTP->cfgP, LVX_NIL, Lambda_Arc,
			 CFG_NULL_SCORE, cfgTP->cfgRP, 0);
    cfgArcSetNode (arc, cfgTP->cfgRP->leaf);

    return TCL_OK;
  }

  /* add new arc */
  if ( type != NT_Arc )
    arc  = cfgNodeAddArc (cfgTP->cfgNP, cfgTP->cfgP, cfgAP->lvX, cfgAP->type, cfgAP->score, cfgTP->cfgRP, 0);
  else
    arc  = cfgNodeAddArc (cfgTP->cfgNP, cfgTP->cfgP, cfgAP->lvX, cfgAP->type, cfgAP->score, cfgAP->cfgRP, 0);

  /* do we have a recursive arc */
  if ( (cfgAP->type & 0x4) ) {
    int i = cfgTP->nXN-1;

    while ( i >= 0 && cfgTP->nXA[i] != cfgAP->cfgNP ) i--;

    if ( i >= 0 ) {
      cfgArcSetNode (arc, cfgTP->mXA[i]);
      return TCL_OK;
    }
  }

  /* normal arc or first occurence of recursive arc,
     so add new node and add it to history array nXA/mXA */
  node = cfgTP->cfgNP;

  if ( !arc->cfgNP ) {
    cfgTP->cfgNP = cfgNodeCreate (cfgTP->cfgP, cfgAP->cfgNP->type);
    cfgArcSetNode (arc, cfgTP->cfgNP);
  } else {
    cfgTP->cfgNP = arc->cfgNP;
  }

  if ( cfgTP->nXN == cfgTP->tXN ) {
    cfgTP->tXN += 1000;
    cfgTP->nXA  = (CFGNode**)realloc (cfgTP->nXA,
				      cfgTP->tXN * sizeof (CFGNode*));
    cfgTP->mXA  = (CFGNode**)realloc (cfgTP->mXA,
				      cfgTP->tXN * sizeof (CFGNode*));
  }

  cfgTP->nXA[cfgTP->nXN++] = cfgAP->cfgNP;
  cfgTP->mXA[cfgTP->mXN++] = cfgTP->cfgNP;
  
  cfgTraverse (cfgAP->cfgRP->cfgP, cfgAP->cfgNP, cfgRSIP, offset, cfgTP);

  cfgTP->cfgNP = node;

  return TCL_OK;
}

int cfgPTNodeCompressBuildDoLeaf (CFGTraverse *cfgTP, CFGNode *cfgNP, CFGArc *cfgAP, CFGRSItem *cfgRSIP, CFGScore offset) {

  CFGArc    *arc  = NULL;
  CFGRSItem *rsip = cfgTP->cfgRSIP;

  if ( !cfgTP->cfgRSIP ) return TCL_OK;

  cfgTP->cfgRSIP = cfgRuleStackPop (cfgTP->cfgRSIP, &arc);

  if ( arc->cfgRP->type != Root_Rule )
    cfgTraverse (arc->cfgRP->cfgP, arc->cfgNP, cfgRSIP, offset, cfgTP);

  cfgTP->cfgRSIP = rsip;

  return TCL_OK;
}

int cfgPTNodeCompressBuild (CFGPTNode *cfgPTNP, CFGTraverse *cfgTP, int verbose) {

  CFGPTItem  *cfgPTIP;
  CFGPTItemX  i;

  cfgTP->cfgPTNP     = cfgPTNP;
  cfgTP->doTArc      = cfgPTNodeCompressBuildDo;
  cfgTP->doNTArc     = cfgPTNodeCompressBuildDo;
  cfgTP->doLambdaArc = cfgPTNodeDoLambda;
  cfgTP->doLeafNode  = cfgPTNodeCompressBuildDoLeaf;
  cfgTP->verbose     = verbose;

  for (i=0; i<cfgPTNP->itemN; i++) {
    cfgPTIP = &(cfgPTNP->itemA[i]);

    cfgTP->cfgRSIP = cfgPTIP->cfgRSIP;

    cfgTraverse (cfgPTIP->cfgAP->cfgRP->cfgP, cfgPTIP->cfgAP->cfgNP,
		 NULL, cfgPTIP->offset, cfgTP);

    cfgTP->nXN = 0;
    cfgTP->mXN = 0;
  }

  return TCL_OK;
}


/* ======================================================================
   Context Free Grammar Parse Tree
   ====================================================================== */

/* ----------------------------------------------------------------------
   Init/Create Context Free Grammar Parse Tree Object
   ---------------------------------------------------------------------- */
int cfgParseTreeInit (CFGParseTree *cfgPTP) {

  cfgPTP->mem.nodes  = cfgParseTreeDefault.mem.nodes;
  cfgPTP->initScoreA = cfgParseTreeDefault.initScoreA;
  cfgPTP->cfgLP      = cfgParseTreeDefault.cfgLP;
  cfgPTP->root       = cfgParseTreeDefault.root;

  return TCL_OK;
}

CFGParseTree* cfgParseTreeCreate (CFGLexicon *cfgLP) {

  CFGParseTree *cfgPTP = (CFGParseTree*)malloc (sizeof(CFGParseTree));

  if ( !cfgPTP || cfgParseTreeInit (cfgPTP) != TCL_OK ) {
    if ( cfgPTP ) free (cfgPTP);
    ERROR ("Cannot create CFGParseTree object\n");
    return NULL;
  }

  cfgPTP->mem.nodes = bmemCreate (50, sizeof (CFGPTNode));

  bmemSetLimits (cfgPTP->mem.nodes, CFGPTNodeX_NIL, CFGPTNodeX_MAX);

  cfgPTP->cfgLP = cfgLP;

  return cfgPTP;
}

/* ----------------------------------------------------------------------
   Deinit/Free Context Free Grammar Parse Tree Object
   ---------------------------------------------------------------------- */
int cfgParseTreeDeinit (CFGParseTree *cfgPTP) {

  if ( cfgPTP->root ) cfgPTNodeFree (cfgPTP->root);

  if ( cfgPTP->mem.nodes ) { bmemFree (cfgPTP->mem.nodes); cfgPTP->mem.nodes = NULL; }

  if ( cfgPTP->initScoreA ) { Nulle (cfgPTP->initScoreA); }

  return TCL_OK;
}

int cfgParseTreeFree (CFGParseTree *cfgPTP) {

  if ( cfgParseTreeDeinit (cfgPTP) != TCL_OK ) return TCL_ERROR;
  if ( cfgPTP ) free (cfgPTP);

  return TCL_OK;
}

/* ----------------------------------------------------------------------
   Configure/Access Context Free Grammar Parse Tree Object
   ---------------------------------------------------------------------- */
int cfgParseTreeConfigureItf (ClientData cd, char *var, char *val) {

  CFGParseTree *cfgPTP = (CFGParseTree*)cd;

  if ( !cfgPTP ) cfgPTP = &cfgParseTreeDefault;

  if ( var == NULL ) {
    ITFCFG (cfgParseTreeConfigureItf, cd, "nodeN");
    return TCL_OK;
  }

  if ( !strcmp (var, "nodeN") ) {
    if ( !val ) {
      itfAppendElement ("%d", bmemGetItemN (cfgPTP->mem.nodes));
      return TCL_OK;
    }
  }

  ERROR("unknown option '-%s %s'.\n", var, val ? val : "?");
  
  return TCL_ERROR;
}

ClientData cfgParseTreeAccessItf (ClientData cd, char *name, TypeInfo **ti) {

  CFGParseTree *cfgPTP = (CFGParseTree*)cd;
  int           i;

  if ( *name == '.' ) {
    if ( name[1] == '\0' ) {
      itfAppendElement ("node(0..%d)", bmemGetItemN (cfgPTP->mem.nodes)-1);
      itfAppendElement ("root");
    } else {
      if ( sscanf (name+1, "node(%d)", &i) == 1 ) {
	*ti = itfGetType ("CFGPTNode");
	if ( i >= 0 && i < (int)bmemGetItemN (cfgPTP->mem.nodes) )
	  return (ClientData)bmemGetItem (cfgPTP->mem.nodes, i);
	else return NULL;
      }
      if ( !strcmp(name+1, "root") ) {
	*ti = itfGetType ("CFGPTNode");
	return (ClientData)cfgPTP->root;
      }
    }
  }

  *ti = NULL;
  return NULL;
}

/* ----------------------------------------------------------------------
   cfgParseTreePutsItf
   ---------------------------------------------------------------------- */
int cfgParseTreePuts (CFGParseTree *cfgPTP, CFGPTNode *cfgPTNP, CFGPutsFormat f) {

  switch (f) {
  case Short_Puts:
    return TCL_OK;
  case Long_Puts:
    return TCL_OK;
  }

  ERROR ("Not supported or unknown PutsFormat %d=%s", f, cfgPutsFormat2str(f));

  return TCL_ERROR;
}

static int cfgParseTreePutsItf (ClientData cd, int argc, char *argv[]) {

  CFGParseTree *cfgPTP = (CFGParseTree*)cd;
  char         *format = "short";

  argc--;
  if ( itfParseArgv (argv[0], &argc, argv+1, 0,
 		     "-format", ARGV_STRING, NULL, &format, NULL,
		     "output format (SHORT, LONG)",
		     NULL) != TCL_OK )
    return TCL_ERROR;

  return cfgParseTreePuts (cfgPTP, NULL, cfgPutsFormat2int (format));
}

/* ----------------------------------------------------------------------
   cfgParseTreeNewNode
   ---------------------------------------------------------------------- */
CFGPTNode* cfgParseTreeNewNode (CFGParseTree *cfgPTP) {

  if ( bmemGetItemN (cfgPTP->mem.nodes) == CFGPTNodeX_MAX ) {
    ERROR ("cfgParseTreeNewNode: max. number of nodes (CFGPTNodeX_MAX=%d) exceeded.\n",
	   CFGPTNodeX_MAX);
    return NULL;
  }

  return (CFGPTNode*)bmemItem (cfgPTP->mem.nodes);
}

/* ----------------------------------------------------------------------
   cfgParseTreeGetNodeX
   ---------------------------------------------------------------------- */
CFGPTNodeX cfgParseTreeGetNodeX (CFGParseTree *cfgPTP, CFGPTNode *cfgPTNP) {

  if ( !cfgPTNP ) return CFGPTNodeX_NIL;

  return (CFGPTNodeX)bmemGetItemX (cfgPTP->mem.nodes, cfgPTNP);
}

/* ----------------------------------------------------------------------
   cfgParseTreeGetNode
   ---------------------------------------------------------------------- */
CFGPTNode* cfgParseTreeGetNode (CFGParseTree *cfgPTP, CFGPTNodeX nodeX) {

  if ( nodeX >= (CFGPTNodeX)bmemGetItemN(cfgPTP->mem.nodes) ) return NULL;

  return (CFGPTNode*)bmemGetItem (cfgPTP->mem.nodes, nodeX);
}

/* ----------------------------------------------------------------------
   cfgParseTreeBuild
   ---------------------------------------------------------------------- */
int cfgParseTreeBuild (CFGParseTree *cfgPTP) {

  LVX lvXN, i;

  if ( !cfgPTP->root )
    cfgPTP->root = cfgPTNodeCreate (cfgPTP, cfgLexiconGetIndex (cfgPTP->cfgLP, cfgPTP->cfgLP->beginOS, T_Arc), NULL);

  /* init score array */
  if ( !cfgPTP->initScoreA ) {
    lvXN               = cfgLexiconGetItemN (cfgPTP->cfgLP, T_Arc);
    cfgPTP->initScoreA = (CFGScore*)malloc (lvXN * sizeof (CFGScore));

    for (i=0; i<lvXN; i++) cfgPTP->initScoreA[i] = CFG_MAX_SCORE;
  }

  return TCL_OK;
}

int cfgParseTreeBuildCFG (CFGParseTree *cfgPTP, CFGArc *cfgAP, CFGRSItem *cfgRSIP, CFGScore score) {

  if ( !cfgPTNodeAddItem (cfgPTP->root, cfgAP, cfgRSIP, score, CFGPTItemX_NIL, 0, 1) )
    return TCL_ERROR;

  return TCL_OK;
}

/* ----------------------------------------------------------------------
   cfgParseTreeClear
   ---------------------------------------------------------------------- */
int cfgParseTreeClear (CFGParseTree *cfgPTP, int free) {

  /* remove all existing nodes */
  if ( cfgPTP->mem.nodes ) {
    CFGPTNodeX  i;
    CFGPTNode  *cfgPTNP;

    for (i=0; i<(CFGPTNodeX)bmemGetItemN(cfgPTP->mem.nodes); i++) {
      cfgPTNP = cfgParseTreeGetNode (cfgPTP, i);
      cfgPTNodeDeinit (cfgPTNP);
    }

    if ( free ) {
      bmemFree  (cfgPTP->mem.nodes);

      cfgPTP->mem.nodes = bmemCreate (50, sizeof (CFGPTNode));

      bmemSetLimits (cfgPTP->mem.nodes, CFGPTNodeX_NIL, CFGPTNodeX_MAX);
    } else {
      bmemClear (cfgPTP->mem.nodes);
    }
  }

  cfgPTP->root = cfgParseTreeDefault.root;

  return TCL_OK;
}

/* ----------------------------------------------------------------------
   cfgParseTreeTraceItf
   ---------------------------------------------------------------------- */
static int cfgParseTreeTraceItf (ClientData cd, int argc, char *argv[]) {

  /* CFGParseTree *cfgPTP  = (CFGParseTree*)cd; */
  SPass        *spassP    = NULL;
  char         *format    = "jsgf";
  int           topN      = 1;
  int           auxNT     = 0;
  LCT           lct       = LCT_NIL;
  LVX           lvX       = LVX_NIL;
  CFGPTNode    *cfgPTNP   = NULL;
  CFGPTNode    *childPTNP = NULL;

  argc--;
  if ( itfParseArgv (argv[0], &argc, argv+1, 0,
 		     "<spass>", ARGV_OBJECT, NULL, &spassP, "SPass",
		     "single pass",
 		     "-auxNT",  ARGV_INT,    NULL, &auxNT,  NULL,
		     "print also auxilliary NTs",
 		     "-topN",   ARGV_INT,    NULL, &topN,   NULL,
		     "print the topN parse trees",
 		     "-format", ARGV_STRING, NULL, &format, NULL,
		     "output format (jsgf, soup)",
		     NULL) != TCL_OK )
    return TCL_ERROR;

  /* get final LCT */
  lct     = spassGetFinalLCT (spassP);
  lvX     = lct2lvx(lct);
  cfgPTNP = lct2node(lct);

  if ( !cfgPTNP ) {
    ERROR ("cfgParseTreeTrace: no parse tree found.\n");
    return TCL_ERROR;
  }

  /* extend given PTNode by the old lvX */
  childPTNP = cfgPTNodeExtend (cfgPTNP, lvX);

  return cfgPTNodeTrace (childPTNP, topN, auxNT, cfgGrammarFormat2int (format));
}

/* ----------------------------------------------------------------------
   cfgParseTreeSaveDump
   ---------------------------------------------------------------------- */
int cfgParseTreeSaveDump (CFGParseTree *cfgPTP, FILE *fp, CFGSet *cfgSP) {

  CFGPTNode  *cfgPTNP = NULL;
  CFGPTNodeX  nodeN   = bmemGetItemN (cfgPTP->mem.nodes);
  CFGPTItem  *cfgPTIP = NULL;
  CFG        *cfgP    = NULL;
  UINT        i, j;

  if ( !fp ) {
    ERROR ("cfgParseTreeSaveDump: invalid file pointer.\n");
    return TCL_ERROR;
  }

  write_string (fp, "CFG-PARSETREE-BEGIN 1.1");

  /* write nodes */
  write_int (fp, nodeN);
  for (i=0; i<nodeN; i++) {
    cfgPTNP = cfgParseTreeGetNode (cfgPTP, i);

    write_int   (fp, (int)cfgPTNP->lvX);
    write_int   (fp, (int)cfgPTNP->bestX);
    write_float (fp, (float)cfgPTNP->bestScore);

    write_int   (fp, (int)cfgPTNP->itemN);
    for (j=0; j<cfgPTNP->itemN; j++) {
      cfgPTIP = &(cfgPTNP->itemA[j]);
      cfgP    = cfgPTIP->cfgAP->cfgRP->cfgP;
      if ( cfgSP )
	write_int (fp, cfgSetGetCFGX (cfgSP, cfgP));
      write_int   (fp, (int)cfgGetArcX (cfgP, cfgPTIP->cfgAP));
      write_int   (fp, (int)cfgRuleStackGetItemX (cfgP->cfgRSP, cfgPTIP->cfgRSIP));
      write_float (fp, (float)cfgPTIP->offset);
      write_int   (fp, (int)cfgPTIP->parentX);
      write_float (fp, cfgPTIP->startover);
    }
  }
  
  /* write linking between them */
  for (i=0; i<nodeN; i++) {
    cfgPTNP = cfgParseTreeGetNode (cfgPTP, i);
    write_int (fp, (int)cfgParseTreeGetNodeX (cfgPTP, cfgPTNP->childP));
    write_int (fp, (int)cfgParseTreeGetNodeX (cfgPTP, cfgPTNP->nextP));
    write_int (fp, (int)cfgParseTreeGetNodeX (cfgPTP, cfgPTNP->parentP));
  }

  write_int (fp, (int)cfgParseTreeGetNodeX (cfgPTP, cfgPTP->root));

  write_string (fp, "CFG-PARSETREE-END");

  return TCL_OK;
}

/* ----------------------------------------------------------------------
   cfgParseTreeLoadDump
   ---------------------------------------------------------------------- */
int cfgParseTreeLoadDump (CFGParseTree *cfgPTP, FILE *fp, CFG *cfgP, CFGSet *cfgSP) {

  CFGPTNode  *cfgPTNP = NULL;
  CFGPTNodeX  nodeN;
  CFGPTItemX  itemN;
  char        s[CFG_MAXLINE];
  int         lvX, arcX, rsitemX, parentX, nodeX, cfgX;
  float       offset, startover;
  CFGArc     *cfgAP;
  CFGRSItem  *cfgRSIP;
  UINT        i, j;

  if ( !fp ) {
    ERROR ("cfgParseTreeLoadDump: invalid file pointer.\n");
    return TCL_ERROR;
  }

  read_string (fp, s);
  if ( strcmp (s, "CFG-PARSETREE-BEGIN 1.1") ) {
    ERROR ("cfgParseTreeLoadDump: inconsistent file.\n");
    return TCL_ERROR;
  }

  /* read node */
  nodeN = (CFGPTNodeX)read_int (fp);
  for (i=0; i<nodeN; i++) {
    lvX     = read_int (fp);
    cfgPTNP = cfgPTNodeCreate (cfgPTP, (LVX)lvX, NULL);
    cfgPTNP->bestX     = (CFGPTItemX)read_int (fp);
    cfgPTNP->bestScore = (CFGScore)read_float (fp);

    itemN              = (CFGPTItemX)read_int (fp);
    for (j=0; j<itemN; j++) {
      if ( cfgSP ) {
	cfgX    = read_int   (fp);
	cfgP    = cfgSetGetCFG (cfgSP, (CFGX)cfgX);
      }
      arcX      = read_int   (fp);
      rsitemX   = read_int   (fp);
      offset    = read_float (fp);
      parentX   = read_int   (fp);
      startover = read_float (fp);

      cfgAP   = cfgGetArc (cfgP, arcX);
      cfgRSIP = cfgRuleStackGetItem (cfgP->cfgRSP, rsitemX);

      cfgPTNodeAddItem (cfgPTNP, cfgAP, cfgRSIP, (CFGScore)offset, (CFGPTItemX)parentX, startover, 0);
    }
  }

  /* read linking between them */
  for (i=0; i<nodeN; i++) {
    cfgPTNP = cfgParseTreeGetNode (cfgPTP, i);

    nodeX            = read_int (fp);
    cfgPTNP->childP  = cfgParseTreeGetNode (cfgPTP, (CFGNodeX)nodeX);
    nodeX            = read_int (fp);
    cfgPTNP->nextP   = cfgParseTreeGetNode (cfgPTP, (CFGNodeX)nodeX);
    nodeX            = read_int (fp);
    cfgPTNP->parentP = cfgParseTreeGetNode (cfgPTP, (CFGNodeX)nodeX);
  }

  nodeX        = read_int (fp);
  cfgPTP->root = cfgParseTreeGetNode (cfgPTP, (CFGNodeX)nodeX);

  read_string (fp, s);
  if ( strcmp (s, "CFG-PARSETREE-END") ) {
    ERROR ("cfgParseTreeLoadDump: inconsistent file.\n");
    return TCL_ERROR;
  }

  return TCL_OK;
}

/****************************************************************************/

/* ========================================================================
    Type Information
   ======================================================================== */

/* ---------- itf types for CFGPTItem */
static Method cfgPTItemMethod[] = {
  { "puts",   cfgPTItemPutsItf,  "display the contents of parse tree item" },
  { NULL, NULL, NULL }
};

TypeInfo cfgPTItemInfo = { "CFGPTItem", 0, -1, cfgPTItemMethod,
			   NULL, NULL,
			   cfgPTItemConfigureItf, cfgPTItemAccessItf,
			   itfTypeCntlDefaultNoLink,
  "A 'CFGPTItem' object is a item in a parse tree node." };

/* ---------- itf types for CFGPTNode */
static Method cfgPTNodeMethod[] = {
  { "puts",   cfgPTNodePutsItf,  "display the contents of parse tree node" },
  { "trace",  cfgPTNodeTraceItf, "returns parse tree by tracing back node" },
  { NULL, NULL, NULL }
};

TypeInfo cfgPTNodeInfo = { "CFGPTNode", 0, -1, cfgPTNodeMethod,
			   NULL, NULL,
			   cfgPTNodeConfigureItf, cfgPTNodeAccessItf,
			   itfTypeCntlDefaultNoLink,
  "A 'CFGPTNode' object is a node of a parse tree." };

/* ---------- itf types for CFGParseTree */
static Method cfgParseTreeMethod[] = {
  { "puts",   cfgParseTreePutsItf,  "display the contents of parse tree" },
  { "trace",  cfgParseTreeTraceItf, "returns parse tree by tracing back" },
  { NULL, NULL, NULL }
};

TypeInfo cfgParseTreeInfo = { "CFGParseTree", 0, -1, cfgParseTreeMethod,
			      NULL, NULL,
			      cfgParseTreeConfigureItf, cfgParseTreeAccessItf,
			      itfTypeCntlDefaultNoLink,
  "A 'CFGParseTree' object is a parse tree." };


/* -------------------- CFGParseTree_Init ---------- */

static int cfgParseTreeInitialized = 0;

int CFGParseTree_Init (void) {

  if ( !cfgParseTreeInitialized ) {

    cfgPTNodeDefault.lvX		= LVX_NIL;
    cfgPTNodeDefault.itemA		= NULL;
    cfgPTNodeDefault.itemN		= 0;
    cfgPTNodeDefault.bestX		= CFGPTItemX_NIL;
    cfgPTNodeDefault.bestScore		= CFG_MAX_SCORE;
    cfgPTNodeDefault.childP		= NULL;
    cfgPTNodeDefault.nextP		= NULL;
    cfgPTNodeDefault.parentP		= NULL;

    cfgParseTreeDefault.mem.nodes	= NULL;
    cfgParseTreeDefault.initScoreA      = NULL;
    cfgParseTreeDefault.root		= NULL;
    cfgParseTreeDefault.cfgLP		= NULL;

    itfNewType (&cfgParseTreeInfo);
    itfNewType (&cfgPTNodeInfo);
    itfNewType (&cfgPTItemInfo);

    cfgParseTreeInitialized = 1;
  }

  return TCL_OK;
}



