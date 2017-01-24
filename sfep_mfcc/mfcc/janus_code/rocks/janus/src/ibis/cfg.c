/* ========================================================================
    JANUS      Speech- to Speech Translation
               ------------------------------------------------------------
               Object: Context Free Grammar
               ------------------------------------------------------------
   
    Author  :  Christian Fuegen
    Module  :  cfg.c
    Date    :  $Id: cfg.c 3323 2010-10-21 01:19:58Z metze $
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
    Revision 4.8  2007/02/22 16:27:59  fuegen
    removed -pedantic compiler warnings with

    Revision 4.7  2007/01/08 15:21:44  fuegen
    removed compiler warnings

    Revision 4.6  2006/10/24 15:28:23  fuegen
    support for reading SRI-LMs WLAT format for sausages (confusion networks)

    Revision 4.5  2004/09/27 13:59:58  fuegen
    removed bug in cfgClear: cfgSetClear is called, if cfgSP is available

    Revision 4.4  2004/09/23 11:42:43  fuegen
    Made code 'x86-64'-clean
    Necessary changes include the usage of functions
    instead of macros for LCT->node/LCT->lvX conversion
    together with a union

    Revision 4.3  2004/08/31 16:26:49  fuegen
    removed some windows compiler warnings

    Revision 4.2  2004/07/23 17:25:32  fuegen
    - Extended supported grammar definition file formats:
       - FSM:  AT&T's FSM (finite state machine) text file format
       - PFSG: Probabilistic Finite State Graph format used by the
               SRI-LM toolkit
    - Added support for reading weights specified in the JSGF format
    - Build functionality of grammars expanded by a mode for making
      equally distributed transitions instead of using fixed scores
    - Added support for generating terminal sequences for specific
      rules either randomly or fixed

    Revision 4.1  2003/08/14 11:19:58  fuegen
    Merged changes on branch jtk-01-01-15-fms (jaguar -> ibis-013)

    Revision 1.1.2.38  2003/08/13 08:51:18  fuegen
    startover requires now float factor
    CFG dump version 1.1

    Revision 1.1.2.37  2003/08/07 14:36:03  fuegen
    removed some windows compiler warnings

    Revision 1.1.2.36  2003/07/25 09:47:04  fuegen
    added cfgReduce, cfgSetReduce

    Revision 1.1.2.35  2003/07/16 15:47:16  fuegen
    added missing ',' in arcTypeA
    added some optional debugging output

    Revision 1.1.2.34  2003/07/15 17:02:04  fuegen
    some more changes
      - added forward/recursive arc types
      - added dumping of CFGs/CFGSets (together with pt/rs)
      - added compression/determinization of CFGs/CFGSets
      - rule stack belongs no more to a single grammar
      - changed LingKS load/save interface, added LingKSPutsFct

    Revision 1.1.2.33  2003/07/02 17:20:19  fuegen
    grammars are now opened in text mode
    some minor code cleaning

    Revision 1.1.2.32  2003/06/23 13:12:06  fuegen
    new CFG implementation
      - less memory usage
      - activation/deactivation down to rule level
      - adding of paths, grammar rules, grammars on the fly
      - basic JSGF grammar support
      - additional support of CFG as LingKS type
      - standalone support of CFG/CFGSet, when using only for e.g. parsing

    Revision 1.1.2.31  2002/11/21 17:10:08  fuegen
    windows code cleaning

    Revision 1.1.2.30  2002/11/19 13:05:08  fuegen
    added deinit for CFGSet

    Revision 1.1.2.29  2002/11/19 12:41:40  fuegen
    changes in load routines

    Revision 1.1.2.28  2002/11/19 08:31:47  fuegen
    some more beautification

    Revision 1.1.2.27  2002/11/18 15:13:29  fuegen
    beautification

    Revision 1.1.2.26  2002/08/26 11:26:36  fuegen
    Removed bugs in decoding along parallel grammars and
    with shared grammars.
    Added output of the parse tree.
    Added reading of grammar files with windows line endings.

    Revision 1.1.2.25  2002/06/14 08:49:14  fuegen
    added generation procs, removed unused code (old traverse)

    Revision 1.1.2.24  2002/06/06 12:51:25  fuegen
    LingKSPuts now works also for CFG

    Revision 1.1.2.23  2002/06/06 10:02:30  fuegen
    code cleaning, reorganisation of configurable options

    Revision 1.1.2.22  2002/05/21 15:23:53  fuegen
    removed cfgSetCreateItf, added cfgSetScore function

    Revision 1.1.2.21  2002/04/29 14:15:55  metze
    Added NULL-Pointers to yet unsupported functions

    Revision 1.1.2.20  2002/04/29 13:34:55  metze
    Redesign of LM interface, integration of CFGSet

    Revision 1.1.2.19  2002/04/18 09:30:50  fuegen
    major changes in interface and main routines, handling of shared grammars, using of CFGExt

    Revision 1.1.2.18  2002/02/19 16:36:45  fuegen
    added training procedures

    Revision 1.1.2.17  2002/02/06 13:17:35  fuegen
    changed parsing interface, added rudimentary training functions

    Revision 1.1.2.16  2002/01/30 16:57:16  fuegen
    changed LingKS interface, added LCT cache

    Revision 1.1.2.15  2002/01/29 17:23:36  fuegen
    made gcc clean, added statistics, removed some minor bugs in lingKS procs

    Revision 1.1.2.14  2002/01/25 17:16:28  fuegen
    added handling of starting new trees, added handling of shared grammars, added some minor things for saving memory

    Revision 1.1.2.13  2002/01/25 17:10:20  fuegen
    first real working version (only parsing)

    Revision 1.1.2.12  2002/01/25 17:02:35  fuegen
    extended parse tree implementation

    Revision 1.1.2.11  2002/01/25 16:53:26  fuegen
    major and minor changes: lexicon as cache, extension routines, lingKS interface

    Revision 1.1.2.10  2002/01/25 16:38:54  fuegen
    reimplementation of a big part, using now indices for memory saving reasons, splitted CFG-implementation in smaller modules

    Revision 1.1.2.9  2002/01/25 15:52:02  fuegen
    added rule stack and traversing functions

    Revision 1.1.2.8  2002/01/25 15:36:33  fuegen
    added rule stack and traversing functions

    Revision 1.1.2.7  2002/01/25 14:58:02  fuegen
    only minor changes

    Revision 1.1.2.6  2001/07/13 10:44:15  fuegen
    saved current grammar support version

    Revision 1.1.2.5  2001/06/01 10:31:48  fuegen
    renamed pointer to LingKS to lksP

    Revision 1.1.2.4  2001/06/01 10:18:58  fuegen
    removed some warnings for gcc under Linux

    Revision 1.1.2.3  2001/06/01 09:52:21  fuegen
    removed some warnings for gcc under Linux

    Revision 1.1.2.2  2001/06/01 08:53:57  fuegen
    new revision of CFG (many changes)

    Revision 1.1.2.1  2001/04/12 17:04:45  fuegen
    initial revision

   
   ======================================================================== */

#include "common.h"
#include "itf.h"
#include "cfg.h"

/* uncomment this for extensive debugging output */
/* #define CFG_DEBUG */

/* ======================================================================
   Forward Declarations
   ====================================================================== */

extern TypeInfo	cfgArcInfo;
extern TypeInfo	cfgNodeInfo;
extern TypeInfo	cfgRuleInfo;
extern TypeInfo	cfgInfo;
extern TypeInfo	cfgSetInfo;

static CFGArc	cfgArcDefault;
static CFGNode	cfgNodeDefault;
static CFGRule	cfgRuleDefault;
static CFG	cfgDefault;
static CFGSet	cfgSetDefault;


extern CFGArc*  cfgNewArc       (CFG *cfgP);
extern CFGNode* cfgNewNode      (CFG *cfgP);
extern CFGRule* cfgNewRule      (CFG *cfgP);
extern CFGArcX  cfgGetArcX      (CFG *cfgP, CFGArc *cfgAP);
extern CFGNodeX cfgGetNodeX     (CFG *cfgP, CFGNode *cfgNP);
extern CFGRuleX cfgGetRuleX     (CFG *cfgP, CFGRule *cfgRP);
extern int      cfgBuild        (CFG *cfgP, CFGBuildMode mode, int reset, int verbose);
extern int      cfgClear        (CFG *cfgP, int free);
extern int      cfgNodePuts     (CFGNode *cfgNP, CFGPutsFormat f);
extern CFG*     cfgSetNewCFG    (CFGSet *cfgSP, char *name);
extern CFGRule* cfgGetRuleByLVX (CFG *cfgP, LVX lvX);
extern CFG*     cfgSetGetSharedCFG (CFGSet *cfgSP);
extern CFG*     cfgSetGetCFGByName (CFGSet *cfgSP, char *name);
extern int      cfgSetClear     (CFGSet *cfgSP, int free);


/* ======================================================================
   Local Functions
   ====================================================================== */

void* lct2node (LCT lct) {
  convLCT.lct = (lct >> CFG_BITSHIFT);
  return convLCT.node;
}

LVX lct2lvx (LCT lct) {
  convLCT.lct = lct;
  return convLCT.lvX;
}

LCT nl2lct (void *node, LVX lvX) {
  convLCT.node = node;
  convLCT.lct  = convLCT.lct << CFG_BITSHIFT;
  convLCT.lct |= (LCT)lvX;
  return convLCT.lct;
}

/* ------------------------------ arc types */
static CFGArcType arcTypeN   = 11;
static char*      arcTypeA[] = {
  "T_Arc",
  "NT_Arc",
  "Lambda_Arc",
  "",
  "T_RArc",
  "NT_RArc",
  "Lambda_RArc",
  "",
  "T_FArc",
  "NT_FArc",
  "Lambda_FArc"
};

char* cfgArcType2str (CFGArcType type) {

  if ( type > 0 && type <= arcTypeN &&
       type != 4 && type != 8 ) return arcTypeA[type-1];

  return NULL;
}

CFGArcType cfgArcType2int (char *type) {

  CFGArcType i;

  for (i=0; i<arcTypeN; i++)
    if ( i != 3 && i != 7 && !strcasecmp (type, arcTypeA[i]) ) return i+1;

  return 0;
}


/* ------------------------------ node types */
static CFGNodeType nodeTypeN   = 3;
static char*       nodeTypeA[] = {
  "Normal_Node",
  "Root_Node",
  "Leaf_Node"
};

char* cfgNodeType2str (CFGNodeType type) {

  if ( type > 0 && type <= nodeTypeN ) return nodeTypeA[type-1];

  return NULL;
}

CFGNodeType cfgNodeType2int (char *type) {

  CFGNodeType i;

  for (i=0; i<nodeTypeN; i++)
    if ( !strcasecmp (type, nodeTypeA[i]) ) return i+1;

  return 0;
}

/* ------------------------------ rule types */
static CFGRuleType ruleTypeN   = 7;
static char*       ruleTypeA[] = {
  "Public_Rule",
  "Private_Rule",
  "Aux_Rule",
  "Void_Rule",
  "Null_Rule",
  "Undef_Rule",
  "Root_Rule"
};

char* cfgRuleType2str (CFGRuleType type) {

  if ( type > 0 && type <= ruleTypeN ) return ruleTypeA[type-1];

  return NULL;
}

CFGRuleType cfgRuleType2int (char *type) {

  CFGRuleType i;

  for (i=0; i<ruleTypeN; i++)
    if ( !strcasecmp (type, ruleTypeA[i]) ) return i+1;

  return 0;
}

/* ------------------------------ status types */
static CFGStatus statusN   = 3;
static char*     statusA[] = {
  "Active",
  "Inactive",
  "Shared"
};

char* cfgStatus2str (CFGStatus status) {

  if ( status > 0 && status <= statusN ) return statusA[status-1];

  return NULL;
}

CFGStatus cfgStatus2int (char *status) {

  CFGStatus i;

  for (i=0; i<statusN; i++)
    if ( !strcasecmp (status, statusA[i]) ) return i+1;

  return 0;
}

/* ------------------------------ puts formats */
static CFGPutsFormat putsFormatN   = 4;
static char*         putsFormatA[] = {
  "short",
  "long",
  "recursive",
  "fsm"
};

char* cfgPutsFormat2str (CFGPutsFormat format) {

  if ( format > 0 && format >= putsFormatN ) return putsFormatA[format-1];

  return NULL;
}

CFGPutsFormat cfgPutsFormat2int (char *format) {

  CFGPutsFormat i;

  for (i=0; i<putsFormatN; i++)
    if ( !strcasecmp (format, putsFormatA[i]) ) return i+1;

  return 0;
}

/* ------------------------------ grammar formats */
static CFGGrammarFormat grammarFormatN   = 6;
static char*            grammarFormatA[] = {
  "soup",
  "jsgf",
  "fsm",
  "pfsg",
  "wlat",
  "dump"
};

char* cfgGrammarFormat2str (CFGGrammarFormat format) {

  if ( format > 0 && format >= grammarFormatN ) return grammarFormatA[format-1];

  return NULL;
}

CFGGrammarFormat cfgGrammarFormat2int (char *format) {

  CFGGrammarFormat i;

  for (i=0; i<grammarFormatN; i++)
    if ( !strcasecmp (format, grammarFormatA[i]) ) return i+1;

  return 0;
}

/* ------------------------------ build mode */
static CFGBuildMode buildModeN   = 4;
static char*        buildModeA[] = {
  "default",
  "null",
  "fixed",
  "equal"
};

char* cfgBuildMode2str (CFGBuildMode mode) {

  if ( mode > 0 && mode <= buildModeN ) return buildModeA[mode-1];

  return NULL;
}

CFGBuildMode cfgBuildMode2int (char *mode) {

  CFGBuildMode i;

  for (i=0; i<buildModeN; i++)
    if ( !strcasecmp (mode, buildModeA[i]) ) return i+1;

  return 0;
}

/* ======================================================================
   Context Free Grammar Arc
   ====================================================================== */

/* ----------------------------------------------------------------------
   Init/Create Context Free Grammar Arc Object
   ---------------------------------------------------------------------- */

int cfgArcInit (CFGArc *cfgAP) {

  cfgAP->lvX	= cfgArcDefault.lvX;
  cfgAP->type	= cfgArcDefault.type;
  cfgAP->score	= cfgArcDefault.score;
  cfgAP->cfgRP	= cfgArcDefault.cfgRP;
  cfgAP->cfgNP	= cfgArcDefault.cfgNP;

  return TCL_OK;
}

int cfgArcAlloc (CFGArc *cfgAP, LVX lvX, CFGArcType type, CFGScore score, CFGRule *cfgRP) {

  cfgAP->lvX	= lvX;
  cfgAP->type	= type;
  cfgAP->score	= score;
  cfgAP->cfgRP	= cfgRP;

  return TCL_OK;
}

CFGArc* cfgArcCreate (CFG *cfgP, LVX lvX, CFGArcType type, CFGScore score, CFGRule *cfgRP) {
  
  CFGArc* cfgAP = cfgNewArc (cfgP);
  
  if ( !cfgAP || cfgArcInit (cfgAP) != TCL_OK ) {
    if ( cfgAP ) Nulle (cfgAP);
    ERROR ("Cannot create CFGArc object.\n");
    return NULL;
  }

  cfgArcAlloc (cfgAP, lvX, type, score, cfgRP);
  
  return cfgAP;
}

/* ----------------------------------------------------------------------
   Deinit/Free Context Free Grammar Arc Object
   ---------------------------------------------------------------------- */

int cfgArcDeinit (CFGArc *cfgAP) {

  /* set all to default */
  return cfgArcInit (cfgAP);
}

int cfgArcFree (CFGArc *cfgAP) {

  if ( cfgArcDeinit (cfgAP) != TCL_OK ) return TCL_ERROR;
  if ( cfgAP ) Nulle (cfgAP);

  return TCL_OK;
}

/* ----------------------------------------------------------------------
   Configure/Access Context Free Grammar Arc Object
   ---------------------------------------------------------------------- */

int cfgArcConfigureItf (ClientData cd, char *var, char *val) {
  
  CFGArc *cfgAP  = (CFGArc*)cd;
  
  if ( !cfgAP ) cfgAP = &cfgArcDefault;
  
  if ( var == NULL ) {
    ITFCFG (cfgArcConfigureItf, cd, "lvX");
    ITFCFG (cfgArcConfigureItf, cd, "type");
    ITFCFG (cfgArcConfigureItf, cd, "score");
    return TCL_OK;
  }

  ITFCFG_Int     (var, val, "lvX",  cfgAP->lvX,  1);

  if ( !strcmp (var, "type") ) {
    if ( !val ) {
      itfAppendElement ("%s", cfgArcType2str (cfgAP->type));
      return TCL_OK;
    }
    return TCL_OK;
  }
  if ( !strcmp (var, "score") ) {
    if ( !val ) {
      itfAppendElement ("%f", UnCompressScore (cfgAP->score));
      return TCL_OK;
    }
    cfgAP->score = CompressScore (atof (val));
    return TCL_OK;
  }
  
  ERROR("unknown option '-%s %s'.\n", var, val ? val : "?");
  
  return TCL_ERROR;
}

ClientData cfgArcAccessItf (ClientData cd, char *name, TypeInfo **ti) {
  
  CFGArc *cfgAP = (CFGArc*)cd;

  if ( *name == '.' ) {
    if ( name[1] == '\0' ) {
      itfAppendElement ("node");
      itfAppendElement ("rule");
    } else {
      if ( !strcmp (name+1, "node") ) {
	*ti = itfGetType ("CFGNode");
	return (ClientData)cfgAP->cfgNP;
      }
      if ( !strcmp (name+1, "rule") ) {
	*ti = itfGetType ("CFGRule");
	return (ClientData)cfgAP->cfgRP;
      }
    }
  }
  
  *ti = NULL;
  return NULL;
}

/* ----------------------------------------------------------------------
   cfgArcPutsItf
   ---------------------------------------------------------------------- */

int cfgArcPuts (CFGArc *cfgAP, CFGPutsFormat f) {

  CFGLexicon *cfgLP = cfgAP->cfgRP->cfgP->cfgLP;

  switch (f) {
  case Recursive_Puts:
    itfAppendResult ("%s (%f) ",
		     cfgLexiconGetName (cfgLP, cfgAP->lvX, cfgAP->type),
		     UnCompressScore (cfgAP->score));
    return cfgNodePuts (cfgAP->cfgNP, f);
  case Short_Puts:
    itfAppendResult ("%s (%f) ",
		     cfgLexiconGetName (cfgLP, cfgAP->lvX, cfgAP->type),
		     UnCompressScore (cfgAP->score));
    return TCL_OK;
  case Long_Puts:
    itfAppendResult ("{name %s} {type %s} {score %f}",
		     cfgLexiconGetName (cfgLP, cfgAP->lvX, cfgAP->type),
		     cfgArcType2str (cfgAP->type),
		     UnCompressScore (cfgAP->score));
    return TCL_OK;
  }

  ERROR ("Not supported or unknown PutsFormat %d=%s\n", f, cfgPutsFormat2str(f));

  return TCL_ERROR;
}

static int cfgArcPutsItf (ClientData cd, int argc, char *argv[]) {

  CFGArc *cfgAP  = (CFGArc*)cd;
  char   *format = "short";

  argc--;
  if ( itfParseArgv (argv[0], &argc, argv+1, 0,
 		     "-format", ARGV_STRING, NULL, &format, NULL,
		     "output format",
		     NULL) != TCL_OK )
    return TCL_ERROR;

  return cfgArcPuts (cfgAP, cfgPutsFormat2int (format));
}

/* ----------------------------------------------------------------------
   cfgArcIsEqual
   ---------------------------------------------------------------------- */
int cfgArcIsEqual (CFGArc *cfgAP, LVX lvX, CFGArcType type, CFGScore score, CFGRule *cfgRP) {

  if ( !cfgAP ||
       cfgAP->lvX   != lvX   ||
       cfgAP->type  != type  ||
       cfgAP->score != score ||
       cfgAP->cfgRP != cfgRP ) return 0;

  return 1;
}

/* ----------------------------------------------------------------------
   cfgArcSetNode
   ---------------------------------------------------------------------- */
int cfgArcSetNode (CFGArc *cfgAP, CFGNode *cfgNP) {

  assert (!cfgAP->cfgNP || cfgAP->cfgNP == cfgNP);

  cfgAP->cfgNP = cfgNP;

  return TCL_OK;
}


/* ======================================================================
   Context Free Grammar Node
   ====================================================================== */

/* ----------------------------------------------------------------------
   Init/Create Context Free Grammar Node Object
   ---------------------------------------------------------------------- */

int cfgNodeInit (CFGNode *cfgNP) {

  cfgNP->type	= cfgNodeDefault.type;
  cfgNP->arcA	= cfgNodeDefault.arcA;
  cfgNP->arcN	= cfgNodeDefault.arcN;
  
  return TCL_OK;
}

int cfgNodeAlloc (CFGNode *cfgNP, CFGNodeType type) {

  cfgNP->type	= type;

  return TCL_OK;
}

CFGNode* cfgNodeCreate (CFG *cfgP, CFGNodeType type) {
  
  CFGNode* cfgNP = cfgNewNode (cfgP);
  
  if ( !cfgNP || cfgNodeInit (cfgNP) != TCL_OK ) {
    if ( cfgNP ) Nulle (cfgNP);
    ERROR ("Cannot create CFGNode object.\n");
    return NULL;
  }

  cfgNodeAlloc (cfgNP, type);
  
  return cfgNP;
}

/* ----------------------------------------------------------------------
   Deinit/Free Context Free Grammar Node Object
   ---------------------------------------------------------------------- */

int cfgNodeDealloc (CFGNode *cfgNP) {

  if ( cfgNP->arcN ) Nulle (cfgNP->arcA);
  cfgNP->arcN = 0;

  return TCL_OK;
}

int cfgNodeDeinit (CFGNode *cfgNP) {

  cfgNodeDealloc (cfgNP);

  /* set all to default */
  return cfgNodeInit (cfgNP);
}

int cfgNodeFree (CFGNode *cfgNP) {

  if ( cfgNodeDeinit (cfgNP) != TCL_OK ) return TCL_ERROR;
  if ( cfgNP ) Nulle (cfgNP);

  return TCL_OK;
}

/* ----------------------------------------------------------------------
   Configure/Access Context Free Grammar Node Object
   ---------------------------------------------------------------------- */

int cfgNodeConfigureItf (ClientData cd, char *var, char *val) {
  
  CFGNode *cfgNP = (CFGNode*)cd;
  
  if ( !cfgNP ) cfgNP = &cfgNodeDefault;
  
  if ( var == NULL ) {
    ITFCFG (cfgNodeConfigureItf, cd, "type");
    ITFCFG (cfgNodeConfigureItf, cd, "arcN");
    return TCL_OK;
  }
  
  ITFCFG_Int     (var, val, "arcN", cfgNP->arcN, 1);

  if ( !strcmp (var, "type") ) {
    if ( !val ) {
      itfAppendElement ("%s", cfgNodeType2str (cfgNP->type));
      return TCL_OK;
    }
    return TCL_OK;
  }
  
  ERROR("unknown option '-%s %s'.\n", var, val ? val : "?");
  
  return TCL_ERROR;
}

ClientData cfgNodeAccessItf (ClientData cd, char *name, TypeInfo **ti) {
  
  CFGNode *cfgNP = (CFGNode*)cd;
  int      i;

  if ( *name == '.' ) {
    if ( name[1] == '\0' ) {
	itfAppendElement ("arc(0..%d)", cfgNP->arcN-1);
    } else {
      if ( sscanf (name+1, "arc(%d)", &i) == 1 ) {
	if ( i >= 0 && (CFGArcX)i < cfgNP->arcN ) {
	  *ti = itfGetType ("CFGArc");
	  return (ClientData)cfgNP->arcA[i];
	}
      }
    }
  }

  if ( *name == ':' ) {
    CFGLexicon *cfgLP = NULL;
    CFGArc     *cfgAP = NULL;
    CFGArcX     j;

    if ( !cfgNP->arcN ) {
      *ti = NULL;
      return NULL;
    }

    cfgLP = cfgNP->arcA[0]->cfgRP->cfgP->cfgLP;

    if ( name[1] == '\0' ) {
      for (j=0; j<cfgNP->arcN; j++) {
	cfgAP = cfgNP->arcA[j];
	itfAppendElement ("%s", cfgLexiconGetName (cfgLP, cfgAP->lvX, cfgAP->type));
      }
    } else {
      name++;
      for (j=0; j<cfgNP->arcN; j++) {
	cfgAP = cfgNP->arcA[j];

	if ( cfgAP->lvX == cfgLexiconGetIndex (cfgLP, name, cfgAP->type) ) {
	  *ti = itfGetType ("CFGArc");
	  return (ClientData)cfgAP;
	}
      }
    }      
  }

  *ti = NULL;
  return NULL;
}

/* ----------------------------------------------------------------------
   cfgNodePutsItf
   ---------------------------------------------------------------------- */

int cfgNodePuts (CFGNode *cfgNP, CFGPutsFormat f) {

  CFGArcX i;

  switch (f) {
  case Recursive_Puts:
    for (i=0; i<cfgNP->arcN; i++) {
      cfgArcPuts (cfgNP->arcA[i], Short_Puts);
    }
    return TCL_OK;
  case Short_Puts:
  case Long_Puts:
    itfAppendResult ("{type %s} {arcN %d}",
		     cfgNodeType2str (cfgNP->type), cfgNP->arcN);
    return TCL_OK;
  }

  ERROR ("Not supported or unknown PutsFormat %d=%s\n", f, cfgPutsFormat2str(f));

  return TCL_ERROR;
}

static int cfgNodePutsItf (ClientData cd, int argc, char *argv[]) {

  CFGNode *cfgNP  = (CFGNode*)cd;
  char    *format = "short";

  argc--;
  if ( itfParseArgv (argv[0], &argc, argv+1, 0,
 		     "-format", ARGV_STRING, NULL, &format, NULL,
		     "output format",
		     NULL) != TCL_OK )
    return TCL_ERROR;

  return cfgNodePuts (cfgNP, cfgPutsFormat2int (format));
}

/* ----------------------------------------------------------------------
   cfgNodeAddArc
   ---------------------------------------------------------------------- */
CFGArc* cfgNodeAddArc (CFGNode *cfgNP, CFG *cfgP, LVX lvX, CFGArcType type, CFGScore score, CFGRule *cfgRP, int force) {

  CFGArc *cfgAP;
  int     i;

  if ( !force ) {
    /* check if arc already exists in this node */
    for (i=0; (CFGArcX)i<cfgNP->arcN; i++) {
      if ( cfgArcIsEqual (cfgNP->arcA[i], lvX, type, score, cfgRP) )
	return cfgNP->arcA[i];
    }
  }

  /* create new arc and add it to the node */
  cfgAP = cfgArcCreate (cfgP, lvX, type, score, cfgRP);

  cfgNP->arcA = (CFGArc**)realloc (cfgNP->arcA,
				   (cfgNP->arcN+1) * sizeof (CFGArc*));

  if ( !cfgNP->arcA ) {
    ERROR ("cfgNodeAddArc: reallocation failure.\n");
    return NULL;
  }

  cfgNP->arcA[cfgNP->arcN++] = cfgAP;

  return cfgAP;
}


/* ======================================================================
   Context Free Grammar Rule
   ====================================================================== */

/* ----------------------------------------------------------------------
   Init/Create Context Free Grammar Rule Object
   ---------------------------------------------------------------------- */

int cfgRuleInit (CFGRule *cfgRP) {

  cfgRP->lvX		= cfgRuleDefault.lvX;
  cfgRP->type		= cfgRuleDefault.type;
  cfgRP->status		= cfgRuleDefault.status;
  cfgRP->weight		= cfgRuleDefault.weight;
  cfgRP->root           = cfgRuleDefault.root;
  cfgRP->leaf           = cfgRuleDefault.leaf;
  cfgRP->cfgP		= cfgRuleDefault.cfgP;

  return TCL_OK;
}

int cfgRuleAlloc (CFGRule *cfgRP, CFG *cfgP, LVX lvX, CFGRuleType type) {

  cfgRP->lvX   = lvX;
  cfgRP->type  = type;
  cfgRP->cfgP  = cfgP;

  cfgRP->root  = cfgNodeCreate (cfgP, Root_Node);
  cfgRP->leaf  = cfgNodeCreate (cfgP, Leaf_Node);

  return TCL_OK;
}

CFGRule* cfgRuleCreate (CFG *cfgP, LVX lvX, CFGRuleType type) {

  CFGRule *cfgRP = cfgNewRule (cfgP);

  if ( !cfgRP || cfgRuleInit (cfgRP) != TCL_OK ) {
    if ( cfgRP ) Nulle (cfgRP);
    ERROR ("Cannont create CFGRule object.\n");
    return NULL;
  }

  cfgRuleAlloc (cfgRP, cfgP, lvX, type);

  return cfgRP;
}

/* ----------------------------------------------------------------------
   Deinit/Free Context Free Grammar Rule Object
   ---------------------------------------------------------------------- */

int cfgRuleDeinit (CFGRule *cfgRP) {

  /* free root/leaf */
  if ( cfgRP->root ) cfgNodeFree (cfgRP->root);
  if ( cfgRP->leaf ) cfgNodeFree (cfgRP->leaf);

  /* set all to default */
  return cfgRuleInit (cfgRP);
}

int cfgRuleFree (CFGRule *cfgRP) {

  if ( cfgRuleDeinit (cfgRP) != TCL_OK ) return TCL_ERROR;
  if ( cfgRP ) Nulle (cfgRP);

  return TCL_OK;
}

/* ----------------------------------------------------------------------
   Configure/Access Context Free Grammar Rule Object
   ---------------------------------------------------------------------- */

int cfgRuleConfigureItf (ClientData cd, char *var, char *val) {
  
  CFGRule *cfgRP = (CFGRule*)cd;
  
  if ( !cfgRP ) cfgRP = &cfgRuleDefault;
  
  if ( var == NULL ) {
    ITFCFG (cfgRuleConfigureItf, cd, "lvX");
    ITFCFG (cfgRuleConfigureItf, cd, "type");
    ITFCFG (cfgRuleConfigureItf, cd, "status");
    ITFCFG (cfgRuleConfigureItf, cd, "weight");
    return TCL_OK;
  }

  ITFCFG_Int   (var, val, "lvX",        cfgRP->lvX,        1);
  ITFCFG_Float (var, val, "weight",     cfgRP->weight,     0);

  if ( !strcmp (var, "type") ) {
    if ( !val ) {
      itfAppendElement ("%s", cfgRuleType2str (cfgRP->type));
      return TCL_OK;
    }
    return TCL_OK;
  }

  if ( !strcmp (var, "status") ) {
    int i;
    if ( !val ) {
      itfAppendElement ("%s", cfgStatus2str (cfgRP->status));
      return TCL_OK;
    }
    
    i = cfgStatus2int (val);

    if ( cfgRP->status != i ) cfgClear (cfgRP->cfgP, 0);
    cfgRP->status = i;

    return TCL_OK;
  }

  ERROR("unknown option '-%s %s'.\n", var, val ? val : "?");
  
  return TCL_ERROR;
}

ClientData cfgRuleAccessItf (ClientData cd, char *name, TypeInfo **ti) {

  CFGRule      *cfgRP  = (CFGRule*)cd;
  
  if ( *name == '.' ) {
    if ( name[1] == '\0' ) {
      itfAppendElement ("root");
      itfAppendElement ("leaf");
      itfAppendElement ("cfg");
    } else {
      if ( !strcmp (name+1, "root") ) {
	*ti = itfGetType ("CFGNode");
	return (ClientData)cfgRP->root;
      }
      if ( !strcmp (name+1, "leaf") ) {
	*ti = itfGetType ("CFGNode");
	return (ClientData)cfgRP->leaf;
      }
      if ( !strcmp (name+1, "cfg") ) {
	*ti = itfGetType ("CFG");
	return (ClientData)cfgRP->cfgP;
      }
    }
  }

  *ti = NULL;
  return NULL;
}

/* ----------------------------------------------------------------------
   cfgRulePutsItf
   ---------------------------------------------------------------------- */
int cfgRuleFormat (CFG *cfgP, CFGRule *cfgRP, CFGGrammarFormat f, char *nt) {

  switch (f) {
  case SOUP_Format:
    if ( cfgRP->type == Aux_Rule ) {
      char *s = cfgLexiconGetName (cfgP->cfgLP, cfgRP->lvX, NT_Arc);

      if ( strstr (s, "_NT_") == s ) 
	sprintf (nt, "[%s]::%s ( ", s, cfgP->tag);
      else
	sprintf (nt, "%s::%s ( ", s, cfgP->tag);
    } else {
      sprintf (nt, "[%s]::%s ( ",
	       cfgLexiconGetName (cfgP->cfgLP, cfgRP->lvX, NT_Arc),
	       cfgP->tag);
    }
    break;
  case JSGF_Format:
    sprintf (nt, "<%s> ( ",
	     cfgLexiconGetName (cfgP->cfgLP, cfgRP->lvX, NT_Arc));
    break;
  default:
    ERROR ("cfgRuleFormat: unknown grammar format %d=%s\n", f, cfgGrammarFormat2str(f));
    return TCL_ERROR;
  }

  return TCL_OK;
}

int cfgRulePutsDo (CFGTraverse *cfgTP, CFGNode *cfgNP, CFGArc *cfgAP, CFGRSItem *cfgRSIP, CFGScore offset) {

  CFGNodeX from = 0;
  CFGNodeX to   = LVX_NIL;

  from = cfgGetNodeX (cfgTP->cfgP, cfgNP);
  to   = cfgGetNodeX (cfgTP->cfgP, cfgAP->cfgNP);

  itfAppendResult ("%-6d %-6d %-20s\n", from, to,
		   cfgLexiconGetName (cfgTP->cfgP->cfgLP, cfgAP->lvX, cfgAP->type));

  /* skip expansion of repeatable and forward arc */
  if ( cfgAP->type > 3 ) return TCL_OK;

  return cfgTraverse (cfgTP->cfgP, cfgAP->cfgNP, cfgRSIP, offset, cfgTP);
}

int cfgRulePutsDoLeaf (CFGTraverse *cfgTP, CFGNode *cfgNP, CFGArc *cfgAP, CFGRSItem *cfgRSIP, CFGScore offset) {

  CFGNodeX from = cfgGetNodeX (cfgTP->cfgP, cfgNP);

  itfAppendResult ("%-6d\n", from);

  return TCL_OK;
}


int cfgRulePuts (CFGRule *cfgRP, CFGPutsFormat f) {

  CFGLexicon  *cfgLP = cfgRP->cfgP->cfgLP;
  CFGTraverse *cfgTP;

  switch (f) {
  case Recursive_Puts:
    cfgNodePuts (cfgRP->root, f);
    return TCL_OK;
  case Short_Puts:
    itfAppendResult ("%s", cfgLexiconGetName (cfgLP, cfgRP->lvX, NT_Arc));
    return TCL_OK;
  case Long_Puts:
    itfAppendResult ("{name %s} {idx %d} {type %s} {status %s} {weight %f}",
		     cfgLexiconGetName (cfgLP, cfgRP->lvX, NT_Arc),
		     cfgGetRuleX       (cfgRP->cfgP, cfgRP),
		     cfgRuleType2str   (cfgRP->type),
		     cfgStatus2str     (cfgRP->status),
		     cfgRP->weight);
    return TCL_OK;
  case FSM_Puts:
    cfgTP              = cfgTraverseCreate ();
    cfgTP->cfgP        = cfgRP->cfgP;
    cfgTP->doTArc      = cfgRulePutsDo;
    cfgTP->doNTArc     = cfgRulePutsDo;
    cfgTP->doLambdaArc = cfgRulePutsDo;
    cfgTP->doLeafNode  = cfgRulePutsDoLeaf;

    cfgTraverse (cfgRP->cfgP, cfgRP->root, NULL, CFG_NULL_SCORE, cfgTP);

    cfgTraverseFree (cfgTP);

    return TCL_OK;
  }

  ERROR ("Not supported or unknown PutsFormat %d=%s\n", f, cfgPutsFormat2str(f));

  return TCL_ERROR;
}

static int cfgRulePutsItf (ClientData cd, int argc, char *argv[]) {

  CFGRule *cfgRP  = (CFGRule*)cd;
  char    *format = "short";

  argc--;
  if ( itfParseArgv (argv[0], &argc, argv+1, 0,		     
 		     "-format", ARGV_STRING, NULL, &format, NULL,
		     "output format (short, long)",
		     NULL) != TCL_OK )
    return TCL_ERROR;

  return cfgRulePuts (cfgRP, cfgPutsFormat2int (format));
}


/* ----------------------------------------------------------------------
   cfgRuleGenerateItf
   generates terminal sequences under a given rule
   generation is done according to the probability distribution given
   by the arcs, therefore double seqeunces could be generated
   fixed == 1 generates sequences in the order of the CFG structure
   recursions can only be followed for the random generation, however
   strange results can be produced, e.g. 'bye bye bye ...'
   ---------------------------------------------------------------------- */
static struct {
  int   seqX;
  int   seqN;
  int   fixed;
  int   recurse;
  LVX  *lvXA;
  int   lvXN;
  FILE *fp;
} cfgGen;

int cfgRuleGenerateDo (CFGTraverse *cfgTP, CFGNode *cfgNP, CFGArc *cfgAP, CFGRSItem *cfgRSIP, CFGScore offset) {

  CFG *cfgP; 
  int  res;

  if ( !cfgRSIP ) {
    int i;

    for (i=0; i<cfgGen.lvXN; i++) {
      if ( i == 0 ) {
	fprintf (cfgGen.fp, "%f %s",  UnCompressScore (offset), cfgLexiconGetName (cfgTP->cfgP->cfgLP, cfgGen.lvXA[i], T_Arc));
      } else {
	fprintf (cfgGen.fp, " %s", cfgLexiconGetName (cfgTP->cfgP->cfgLP, cfgGen.lvXA[i], T_Arc));
      }
    }
    fprintf (cfgGen.fp, "\n");

    cfgGen.seqX++;

    return TCL_OK;
  }

  cfgP = cfgAP->cfgRP->cfgP;

  /* start over handling */
  if ( cfgNP        == cfgP->eosNP &&
       cfgAP->cfgNP == cfgP->bosNP && cfgP->startover < 0 )
    return TCL_OK;

  /*  if ( cfgTP->startover > 0 ) offset *= cfgTP->startover; */

  /* traverse */
  if ( cfgGen.fixed ) {
    if ( cfgAP->type == Lambda_RArc ) return TCL_OK;

    if ( cfgGen.seqX < cfgGen.seqN ) {
      res = cfgTraverse     (cfgAP->cfgRP->cfgP, cfgAP->cfgNP, cfgRSIP, offset, cfgTP);
    } else {
      res = TCL_OK;
    }

  } else {
    if ( !cfgGen.recurse && cfgAP->type == Lambda_RArc ) return TCL_OK;

    res = cfgTraverseRand (cfgAP->cfgRP->cfgP, cfgAP->cfgNP, cfgRSIP, offset, cfgTP);
  }

  return res;
}

int cfgRuleGenerateDoNT (CFGTraverse *cfgTP, CFGNode *cfgNP, CFGArc *cfgAP, CFGRSItem *cfgRSIP, CFGScore offset) {

  if ( cfgGen.fixed ) {
    if ( cfgAP->type == NT_RArc ) return TCL_OK;

    if ( cfgGen.seqX < cfgGen.seqN ) {
      return cfgTraverse     (cfgAP->cfgRP->cfgP, cfgAP->cfgRP->root, cfgRSIP, offset, cfgTP);
    } else {
      return TCL_OK;
    }

  } else {
    if ( !cfgGen.recurse && cfgAP->type == NT_RArc ) return TCL_OK;

    return cfgTraverseRand (cfgAP->cfgRP->cfgP, cfgAP->cfgRP->root, cfgRSIP, offset, cfgTP);
  }
}

int cfgRuleGenerateDoT (CFGTraverse *cfgTP, CFGNode *cfgNP, CFGArc *cfgAP, CFGRSItem *cfgRSIP, CFGScore offset) {

  int res;

  if ( cfgGen.lvXN == CFG_MAXLINE ) {
    ERROR ("cfgRuleGenerate: sentence length exeeds %d words\n", CFG_MAXLINE);
    return TCL_ERROR;
  }

  if ( cfgGen.fixed ) {
    if ( cfgAP->type == T_RArc ) return TCL_OK;

    cfgGen.lvXA[cfgGen.lvXN++] = cfgAP->lvX;

    if ( cfgGen.seqX < cfgGen.seqN ) {
      res = cfgTraverse     (cfgAP->cfgRP->cfgP, cfgAP->cfgNP, cfgRSIP, offset, cfgTP);
    } else {
      res = TCL_OK;
    }

  } else {
    if ( !cfgGen.recurse && cfgAP->type == T_RArc ) return TCL_OK;

    cfgGen.lvXA[cfgGen.lvXN++] = cfgAP->lvX;

    res = cfgTraverseRand (cfgAP->cfgRP->cfgP, cfgAP->cfgNP, cfgRSIP, offset, cfgTP);
  }

  cfgGen.lvXN--;

  return res;
}

int cfgRuleGenerate (CFGRule *cfgRP, int seqN, int fixed, int recurse, char *fileName, int append) {

  CFGTraverse *cfgTP = cfgTraverseCreate ();

  if ( !cfgRP->cfgP->built ) cfgBuild (cfgRP->cfgP, DEFAULT_SCORES, 0, 0);

  cfgTP->cfgP        = cfgRP->cfgP;
  cfgTP->doTArc      = cfgRuleGenerateDoT;
  cfgTP->doNTArc     = cfgRuleGenerateDoNT;
  cfgTP->doLambdaArc = cfgRuleGenerateDo;
  cfgTP->doLeafNode  = cfgRuleGenerateDo;

  /* initialize cfgGen struct */
  cfgGen.seqX    = 0;
  cfgGen.seqN    = seqN;
  cfgGen.fixed   = fixed;
  cfgGen.recurse = recurse;
  cfgGen.lvXA    = (LVX*)malloc(CFG_MAXLINE*sizeof(LVX));
  cfgGen.lvXN    = 0;

  if ( fileName ) {
    if ( append ) cfgGen.fp = fileOpen (fileName, "a");
    else          cfgGen.fp = fileOpen (fileName, "w");
    if ( !cfgGen.fp ) {
      ERROR ("cfgRuleGenerate: cannot open file '%s' for writing\n", fileName);
      if ( cfgGen.lvXA ) Nulle(cfgGen.lvXA);
      cfgTraverseFree (cfgTP);
      return TCL_ERROR;
    }
  } else {
    cfgGen.fp = stdout;
  }

  if ( fixed ) {
    cfgTraverse (cfgRP->cfgP, cfgRP->root, cfgRP->cfgP->cfgRSP->root,
		 CFG_NULL_SCORE, cfgTP);
  } else {
    srand(12345);

    while (cfgGen.seqX < cfgGen.seqN) {
      cfgTraverseRand (cfgRP->cfgP, cfgRP->root, cfgRP->cfgP->cfgRSP->root,
		       CFG_NULL_SCORE, cfgTP);
    }
  }

  if ( fileName ) fileClose (fileName, cfgGen.fp);

  if ( cfgGen.lvXA ) Nulle(cfgGen.lvXA);
  cfgTraverseFree (cfgTP);

  return TCL_OK;
}

static int cfgRuleGenerateItf (ClientData cd, int argc, char *argv[]) {

  CFGRule *cfgRP    = (CFGRule*)cd;
  char    *mode     = "random";
  int      seqN     = 0;
  int      recurse  = 0;
  int      fixed    = 0;
  int      append   = 0;
  char    *fileName = NULL;

  argc--;
  if ( itfParseArgv (argv[0], &argc, argv+1, 0,		     
 		     "<seqN>",   ARGV_INT,    NULL, &seqN,     NULL,
		     "number of terminal sequences",
		     "-mode",    ARGV_STRING, NULL, &mode,     NULL,
		     "generation mode (random|fixed)",
 		     "-recurse", ARGV_INT,    NULL, &recurse,  NULL,
		     "follow recursions",
		     "-file" ,   ARGV_STRING, NULL, &fileName, NULL,
		     "file to write output",
 		     "-append",  ARGV_INT,    NULL, &append,  NULL,
		     "append to file",
		     NULL) != TCL_OK )
    return TCL_ERROR;

  if ( strcmp (mode,"fixed") && strcmp (mode,"random") ) {
    ERROR ("cfgRuleGenerate: unknown mode '%s'\n", mode);
    return TCL_ERROR;
  }

  if ( !strcmp(mode,"fixed") ) fixed = 1;

  if ( fixed && recurse != 0 ) {
    ERROR ("cfgRuleGenerate: recursion not supported with fixed generation\n");
    return TCL_ERROR;
  }

  return cfgRuleGenerate (cfgRP, seqN, fixed, recurse, fileName, append);
}

/* ----------------------------------------------------------------------
   cfgRuleAddPath
   ---------------------------------------------------------------------- */

int cfgRuleAddPath (CFGRule *cfgRP, char *line, CFGGrammarFormat f) {

  switch (f) {
  case SOUP_Format:
    if ( parseSOUPRuleBody (cfgRP->cfgP, line, cfgRP) != TCL_OK ) {
      ERROR ("cfgRuleAddPath: while adding line to grammer.\n");
      return TCL_ERROR;
    }
    break;
  case JSGF_Format:
    if ( parseJSGFRuleBody (cfgRP->cfgP, line, cfgRP) != TCL_OK ) {
      ERROR ("cfgRuleAddPath: while adding line to grammer.\n");
      return TCL_ERROR;
    }
    break;
  default:
    ERROR ("cfgRuleAddPath: unknown format.\n");
    return TCL_ERROR;
  }

  cfgClear (cfgRP->cfgP, 0);

  return TCL_OK;
}

static int cfgRuleAddPathItf (ClientData cd, int argc, char *argv[]) {

  CFGRule *cfgRP  = (CFGRule*)cd;
  char    *format = "soup";
  char     line[CFG_MAXLINE];
  char    *lP     = &line[0];

  argc--;
  if ( itfParseArgv (argv[0], &argc, argv+1, 0,
                     "<line>",  ARGV_STRING, NULL, &lP,        NULL,
                     "path to add",
		     "-format", ARGV_STRING, NULL, &format,    NULL,
		     "grammar format",
                     NULL) != TCL_OK )
    return TCL_ERROR;

  return cfgRuleAddPath (cfgRP, lP, cfgGrammarFormat2int (format));
}


/* ======================================================================
   Context Free Grammar
   ====================================================================== */

/* ----------------------------------------------------------------------
   Init/Create Context Free Grammar Object
   ---------------------------------------------------------------------- */

int cfgInit (CFG *cfgP, ClientData cd) {

  cfgP->name		= strdup ((char*)cd);
  cfgP->useN		= cfgDefault.useN;
  cfgP->tag		= cfgDefault.tag;

  cfgP->header		= cfgDefault.header;
  cfgP->package		= cfgDefault.package;
  cfgP->importA		= cfgDefault.importA;
  cfgP->importN		= cfgDefault.importN;
  cfgP->ruleRefA	= cfgDefault.ruleRefA;
  cfgP->ruleRefN	= cfgDefault.ruleRefN;

  cfgP->status		= cfgDefault.status;
  cfgP->weight		= cfgDefault.weight;

  cfgP->bosNP		= cfgDefault.bosNP;
  cfgP->eosNP		= cfgDefault.eosNP;
  cfgP->root            = cfgDefault.root;

  cfgP->cfgRSP		= cfgDefault.cfgRSP;
  cfgP->cfgSP		= cfgDefault.cfgSP;
  cfgP->cfgLP		= cfgDefault.cfgLP;
  cfgP->cfgPTP		= cfgDefault.cfgPTP;
  cfgP->lksP		= cfgDefault.lksP;

  cfgP->mem.arcs	= cfgDefault.mem.arcs;
  cfgP->mem.nodes	= cfgDefault.mem.nodes;
  cfgP->mem.rules	= cfgDefault.mem.rules;

  cfgP->allPublic	= cfgDefault.allPublic;
  cfgP->startover	= cfgDefault.startover;
  cfgP->built           = cfgDefault.built;
  cfgP->buildMode       = cfgDefault.buildMode;

  return TCL_OK;
}

int cfgAlloc (CFG *cfgP, char *tag, CFGSet *cfgSP) {

  if ( tag ) {
    cfgP->tag = strdup (tag);
  } else {
    cfgP->tag = strdup (cfgP->name);
  }

  if ( !strcmp (cfgP->tag, CFG_SHARED_TAG) )
    cfgP->status = Shared;

  if ( cfgSP ) {
    cfgP->cfgSP  = cfgSP;
    cfgP->cfgLP  = cfgSP->cfgLP;
    cfgP->cfgRSP = cfgSP->cfgRSP;
    cfgP->cfgPTP = cfgSP->cfgPTP;
  } else {
    cfgP->cfgSP  = NULL;
    cfgP->cfgLP  = cfgLexiconCreate   ();
    cfgP->cfgRSP = cfgRuleStackCreate ();
    cfgP->cfgPTP = cfgParseTreeCreate (cfgP->cfgLP);
  }

  /* create memory blocks */
  cfgP->mem.arcs  = bmemCreate (50, sizeof (CFGArc));
  cfgP->mem.nodes = bmemCreate (50, sizeof (CFGNode));
  cfgP->mem.rules = bmemCreate ( 5, sizeof (CFGRule));

  bmemSetLimits (cfgP->mem.arcs,  CFGArcX_NIL,  CFGArcX_MAX);
  bmemSetLimits (cfgP->mem.nodes, CFGNodeX_NIL, CFGNodeX_MAX);
  bmemSetLimits (cfgP->mem.rules, CFGRuleX_NIL, CFGRuleX_MAX);

  return TCL_OK;
}

CFG* cfgCreate (char *name, char *tag, CFGSet *cfgSP) {

  CFG *cfgP = NULL;

  if ( cfgSP ) {
    cfgP = cfgSetNewCFG (cfgSP, name);
  } else {
    cfgP = (CFG*)malloc (sizeof(CFG));

    if ( !cfgP || cfgInit (cfgP, (ClientData)name) != TCL_OK ) {
      if ( cfgP ) Nulle (cfgP);
      ERROR ("Cannot create CFG object\n");
      return NULL;
    }
  }

  cfgAlloc (cfgP, tag, cfgSP);

  return cfgP;
}

static ClientData cfgCreateItf (ClientData cd, int argc, char *argv[]) {

  CFGSet *cfgSP = NULL;
  LingKS *lksP  = NULL;
  char   *tag   = cfgDefault.tag;
  
  argc--;
  if ( itfParseArgv (argv[0], &argc, argv+1, 0,
		     "-cfgSet",  ARGV_OBJECT, NULL, &cfgSP, "CFGSet",
		     "context free grammar set",
		     "-lks",     ARGV_OBJECT, NULL, &lksP,  "LingKS",
		     "linguistic knowledge source",
		     "-tag",     ARGV_STRING, NULL, &tag,  NULL,
		     "tag of grammar",
		     NULL) != TCL_OK )
    return NULL;

  if ( lksP ) {
    if ( lksP->type != LingKS_CFGSet ) {
      ERROR ("LingKS should be of type CFGSet.\n");
      return NULL;
    }

    if ( cfgSP ) {
      ERROR ("You should either specify CFGSet or LingKS.\n");
      return NULL;
    }

    cfgSP = lksP->data.cfgSP;
  }

  return (ClientData)cfgCreate (argv[0], tag, cfgSP);
}

/* ----------------------------------------------------------------------
   Deinit/Free Context Free Grammar Object
   ---------------------------------------------------------------------- */
int cfgLinkN (CFG *cfgP) {
  
  return cfgP->useN;
}

int cfgDeinit (CFG *cfgP) {

  if ( cfgLinkN (cfgP) ) {
    SERROR ("CFG '%s' still in use by other objects.\n", cfgP->name);
    return TCL_ERROR;
  }

  if ( cfgP->name    ) Nulle (cfgP->name);
  if ( cfgP->tag     ) Nulle (cfgP->tag);
  if ( cfgP->header  ) Nulle (cfgP->header);
  if ( cfgP->package ) Nulle (cfgP->package);

  if ( cfgP->importN ) {
    int i;
    for (i=0; i<cfgP->importN; i++) {
      Nulle (cfgP->importA[i].grammar);
    }
    Nulle (cfgP->importA);
    cfgP->importN = 0;
  }

  if ( cfgP->ruleRefN ) {
    int i;
    for (i=0; i<cfgP->ruleRefN; i++) {
      Nulle (cfgP->ruleRefA[i].grammar);
    }
    Nulle (cfgP->ruleRefA);
    cfgP->ruleRefN = 0;
  }

  /* free lexicon and parse tree */
  if ( !cfgP->cfgSP ) {
    if ( cfgP->cfgLP  ) { cfgLexiconFree   (cfgP->cfgLP);  cfgP->cfgLP  = NULL; }
    if ( cfgP->cfgRSP ) { cfgRuleStackFree (cfgP->cfgRSP); cfgP->cfgRSP = NULL; }
    if ( cfgP->cfgPTP ) { cfgParseTreeFree (cfgP->cfgPTP); cfgP->cfgPTP = NULL; }
  }

  /* free memory blocks */
  if ( cfgP->mem.arcs  ) { bmemFree (cfgP->mem.arcs);  cfgP->mem.arcs  = NULL; }
  if ( cfgP->mem.nodes ) { bmemFree (cfgP->mem.nodes); cfgP->mem.nodes = NULL; }
  if ( cfgP->mem.rules ) { bmemFree (cfgP->mem.rules); cfgP->mem.rules = NULL; }

  return TCL_OK;
}

int cfgFree (CFG *cfgP) {

  if ( cfgDeinit (cfgP) != TCL_OK ) return TCL_ERROR;
  if ( cfgP ) Nulle (cfgP);

  return TCL_OK;
}

static int cfgFreeItf (ClientData cd) {
  
  return cfgFree ((CFG*)cd);
}

/* ----------------------------------------------------------------------
   Configure/Access Context Free Grammar Object
   ---------------------------------------------------------------------- */
int cfgConfigureItf (ClientData cd, char *var, char *val) {
  
  CFG *cfgP = (CFG*)cd;
  
  if ( !cfgP ) cfgP = &cfgDefault;
  
  if ( var == NULL ) {
    ITFCFG (cfgConfigureItf, cd, "name");
    ITFCFG (cfgConfigureItf, cd, "tag");
    ITFCFG (cfgConfigureItf, cd, "status");
    ITFCFG (cfgConfigureItf, cd, "weight");

    ITFCFG (cfgConfigureItf, cd, "arcN");
    ITFCFG (cfgConfigureItf, cd, "nodeN");
    ITFCFG (cfgConfigureItf, cd, "ruleN");

    ITFCFG (cfgConfigureItf, cd, "allPublic");
    ITFCFG (cfgConfigureItf, cd, "startover");
    ITFCFG (cfgConfigureItf, cd, "built");
    ITFCFG (cfgConfigureItf, cd, "buildMode");
    return TCL_OK;
  }
  
  ITFCFG_CharPtr (var, val, "name",          cfgP->name,            1);
  ITFCFG_CharPtr (var, val, "tag",           cfgP->tag,             0);
  ITFCFG_Float   (var, val, "weight",        cfgP->weight,          0);
  ITFCFG_Int     (var, val, "built",         cfgP->built,           1);

  if ( !strcmp (var, "arcN") ) {
    if ( !val ) {
      itfAppendElement ("%d", bmemGetItemN (cfgP->mem.arcs));
      return TCL_OK;
    }
  }

  if ( !strcmp (var, "nodeN") ) {
    if ( !val ) {
      itfAppendElement ("%d", bmemGetItemN (cfgP->mem.nodes));
      return TCL_OK;
    }
  }

  if ( !strcmp (var, "ruleN") ) {
    if ( !val ) {
      itfAppendElement ("%d", bmemGetItemN (cfgP->mem.rules));
      return TCL_OK;
    }
  }

  if ( !strcmp (var, "startover") ) {
    float f;
    if ( !val ) {
      itfAppendElement ("%f", cfgP->startover);
      return TCL_OK;
    }
    
    f = atof (val);

    if ( cfgP->startover != f ) cfgClear (cfgP, 0);
    cfgP->startover = f;

    return TCL_OK;
  }

  if ( !strcmp (var, "status") ) {
    int i;
    if ( !val ) {
      itfAppendElement ("%s", cfgStatus2str (cfgP->status));
      return TCL_OK;
    }
    
    i = cfgStatus2int (val);

    if ( cfgP->status != i ) cfgClear (cfgP, 0);
    cfgP->status = i;

    return TCL_OK;
  }

  if ( !strcmp (var, "allPublic") ) {
    int i;
    if ( !val ) {
      itfAppendElement ("%d", cfgP->allPublic);
      return TCL_OK;
    }
    
    i = atoi (val);

    if ( cfgP->allPublic != i ) {
      cfgClear       (cfgP, 0);
      cfgNodeDealloc (cfgP->bosNP);
    }
    cfgP->allPublic = i;

    return TCL_OK;
  }

  if ( !strcmp (var, "buildMode") ) {
    CFGBuildMode m;
    if ( !val ) {
      itfAppendElement ("%s", cfgBuildMode2str(cfgP->buildMode));
      return TCL_OK;
    }
    
    m = cfgBuildMode2int (val);

    if ( !m ) {
      ERROR ("unknown build mode '%s'\n", val);
      return TCL_ERROR;
    }

    if ( m != cfgP->buildMode ) {
      /* clear and rebuild grammar with new mode */
      if ( cfgP->built ) cfgClear (cfgP, 0);
      cfgBuild (cfgP, m, 1, 0);
    }

    return TCL_OK;
  }
  
  ERROR("unknown option '-%s %s'.\n", var, val ? val : "?");
  
  return TCL_ERROR;
}

ClientData cfgAccessItf (ClientData cd, char *name, TypeInfo **ti) {
  
  CFG *cfgP = (CFG*)cd;
  int  i;
  
  if ( *name == '.' ) {
    if ( name[1] == '\0' ) {
      itfAppendElement ("arc(0..%d)",  bmemGetItemN (cfgP->mem.arcs) -1);
      itfAppendElement ("node(0..%d)", bmemGetItemN (cfgP->mem.nodes)-1);
      itfAppendElement ("rule(0..%d)", bmemGetItemN (cfgP->mem.rules)-1);
      itfAppendElement ("root");
      itfAppendElement ("bos");
      itfAppendElement ("set");
      itfAppendElement ("lex");
      itfAppendElement ("rs");
      itfAppendElement ("pt");
    } else {
      if ( sscanf (name+1, "arc(%d)", &i) == 1 ) {
	*ti = itfGetType ("CFGArc");
	if ( i >= 0 && i < (int)bmemGetItemN (cfgP->mem.arcs) )
	  return (ClientData)bmemGetItem (cfgP->mem.arcs, i);
	else return NULL;
      }
      if ( sscanf (name+1, "node(%d)", &i) == 1 ) {
	*ti = itfGetType ("CFGNode");
	if ( i >= 0 && i < (int)bmemGetItemN (cfgP->mem.nodes) )
	  return (ClientData)bmemGetItem (cfgP->mem.nodes, i);
	else return NULL;
      }
      if ( sscanf (name+1, "rule(%d)", &i) == 1 ) {
	*ti = itfGetType ("CFGRule");
	if ( i >= 0 && i < (int)bmemGetItemN (cfgP->mem.rules) )
	  return (ClientData)bmemGetItem (cfgP->mem.rules, i);
	else return NULL;
      }
      if ( !strcmp(name+1, "root") ) {
	*ti = itfGetType ("CFGRule");
	return (ClientData)cfgP->root;
      }
      if ( !strcmp(name+1, "bos") ) {
	*ti = itfGetType ("CFGNode");
	return (ClientData)cfgP->bosNP;
      }
      if ( !strcmp(name+1, "set") ) {
	*ti = itfGetType ("CFGSet");
	return (ClientData)cfgP->cfgSP;
      }
      if ( !strcmp(name+1, "lex") ) {
	*ti = itfGetType ("CFGLexicon");
	return (ClientData)cfgP->cfgLP;
      }
      if ( !strcmp(name+1, "rs") ) {
	*ti = itfGetType ("CFGRuleStack");
	return (ClientData)cfgP->cfgRSP;
      }
      if ( !strcmp(name+1, "pt") ) {
	*ti = itfGetType ("CFGParseTree");
	return (ClientData)cfgP->cfgPTP;
      }
    }
  }

  if ( *name == ':' ) {
    CFGLexicon *cfgLP = cfgP->cfgLP;
    CFGRule    *cfgRP;

    if ( name[1] == '\0' ) {
      UINT j;
      for (j=0; j<bmemGetItemN (cfgP->mem.rules); j++) {
	cfgRP = (CFGRule*)bmemGetItem (cfgP->mem.rules, j);
	itfAppendElement ("%s", cfgLexiconGetName (cfgLP, cfgRP->lvX, NT_Arc));
      }
    } else {
      name++;

      *ti = itfGetType ("CFGRule");
      return (ClientData)cfgGetRuleByLVX (cfgP, cfgLexiconGetIndex (cfgLP, name, NT_Arc));
    }
  }
  
  *ti = NULL;
  return NULL;
}

/* ----------------------------------------------------------------------
   cfgPutsItf
   ---------------------------------------------------------------------- */
int cfgPuts (CFG *cfgP, CFGPutsFormat f) {

  switch (f) {
  case Short_Puts:
    return cfgLexiconPuts (cfgP->cfgLP, T_Arc, f);
  }

  ERROR ("Not supported or unknown PutsFormat %d=%s\n", f, cfgPutsFormat2str(f));

  return TCL_ERROR;
}

static int cfgPutsItf (ClientData cd, int argc, char *argv[]) {

  CFG  *cfgP   = (CFG*)cd;
  char *format = "short";

  argc--;
  if ( itfParseArgv (argv[0], &argc, argv+1, 0,
 		     "-format", ARGV_STRING, NULL, &format, NULL,
		     "output format (short, long)",
		     NULL) != TCL_OK )
    return TCL_ERROR;

  return cfgPuts (cfgP, cfgPutsFormat2int (format));
}

/* ----------------------------------------------------------------------
   cfgNew* functions for arcs, nodes and rules
   ---------------------------------------------------------------------- */
CFGArc* cfgNewArc (CFG *cfgP) {

  if ( bmemGetItemN (cfgP->mem.arcs) == CFGArcX_MAX ) {
    ERROR ("cfgNewArc: max. number of arcs (CFArcX_MAX=%d) exceeded.\n",
	   CFGArcX_MAX);
    return NULL;
  }

  return (CFGArc*)bmemItem (cfgP->mem.arcs);
}

CFGNode* cfgNewNode (CFG *cfgP) {

  if ( bmemGetItemN (cfgP->mem.nodes) == CFGNodeX_MAX ) {
    ERROR ("cfgNewNode: max. number of nodes (CFGNodeX_MAX=%d) exceeded.\n",
	   CFGNodeX_MAX);
    return NULL;
  }

  return (CFGNode*)bmemItem (cfgP->mem.nodes);
}

CFGRule* cfgNewRule (CFG *cfgP) {

  if ( bmemGetItemN (cfgP->mem.rules) == CFGRuleX_MAX ) {
    ERROR ("cfgNewRule: max. number of rules (CFGRuleX_MAX=%d) exceeded.\n",
	   CFGRuleX_MAX);
    return NULL;
  }

  return (CFGRule*)bmemItem (cfgP->mem.rules);
}

CFGArcX cfgGetArcX (CFG *cfgP, CFGArc *cfgAP) {

  if ( !cfgAP ) return CFGArcX_NIL;

  return (CFGArcX)bmemGetItemX (cfgP->mem.arcs, cfgAP);
}

CFGNodeX cfgGetNodeX (CFG *cfgP, CFGNode *cfgNP) {

  if ( !cfgNP ) return CFGNodeX_NIL;

  return (CFGNodeX)bmemGetItemX (cfgP->mem.nodes, cfgNP);
}

CFGRuleX cfgGetRuleX (CFG *cfgP, CFGRule *cfgRP) {

  if ( !cfgRP ) return CFGRuleX_NIL;

  return (CFGRuleX)bmemGetItemX (cfgP->mem.rules, cfgRP);
}

CFGArc* cfgGetArc (CFG *cfgP, CFGArcX arcX) {

  if ( arcX >= (CFGArcX)bmemGetItemN (cfgP->mem.arcs) ) return NULL;

  return (CFGArc*)bmemGetItem (cfgP->mem.arcs, arcX);
}

CFGNode* cfgGetNode (CFG *cfgP, CFGNodeX nodeX) {

  if ( nodeX >= (CFGNodeX)bmemGetItemN (cfgP->mem.nodes) ) return NULL;

  return (CFGNode*)bmemGetItem (cfgP->mem.nodes, nodeX);
}

CFGRule* cfgGetRule (CFG *cfgP, CFGRuleX ruleX) {

  if ( ruleX >= (CFGRuleX)bmemGetItemN (cfgP->mem.rules) ) return NULL;

  return (CFGRule*)bmemGetItem (cfgP->mem.rules, ruleX);
}

/* ----------------------------------------------------------------------
   cfgLoad
   ---------------------------------------------------------------------- */
static int detectFileFormat (FILE *fp) {

  int  res = 0;
  char line[CFG_MAXLINE];
  char s[CFG_MAXLINE];
  int  i, j;

  /* read first line of file */
  fgets (line, CFG_MAXLINE, fp);

  if      ( strstr (line, "#JSGF")              ) res = JSGF_Format;
  else if ( strstr (line, "#SOUP")              ) res = SOUP_Format;
  else if ( sscanf (line, "%d %d", &i, &j) == 2 ) res =  FSM_Format;
  else if ( sscanf (line, "name %s", s)    == 1 ) res = PFSG_Format;
  else if ( strstr (line, "CFG-BEGIN")          ) res = Dump_Format;
  else {
    WARN ("detectFileFormat: failed, assuming SOUP format!\n");
    res = SOUP_Format;
  }

  /* reset file pointer */
  fseek (fp, 0L, SEEK_SET);

  return res;
}

int cfgLoadDump (CFG *cfgP, FILE *fp, int set) {

  CFGArcX   arcN;
  CFGNodeX  nodeN;
  CFGRuleX  ruleN;
  CFGArc   *cfgAP;
  CFGNode  *cfgNP;
  CFGRule  *cfgRP;
  int       lvX, type, status, arcX, nodeX, ruleX, pt;
  float     score, weight;
  char      s[CFG_MAXLINE];
  UINT      i, j;

  if ( !fp ) {
    ERROR ("cfgLoadDump: invalid file pointer.\n");
    return TCL_ERROR;
  }

  read_string (fp, s);
  if ( strcmp (s, "CFG-BEGIN 1.1") ) {
    ERROR ("cfgLoadDump: inconsistent file.\n");
    return TCL_ERROR;
  }

  /* read CFG */
  i = read_int (fp);
  if ( (int)i != set ) {
    ERROR ("cfgLoadDump: dump belongs%s to a set, but grammar%s.\n",
	   i ? "" : " not", set ? "" : " not");
    return TCL_ERROR;
  }

  pt = read_int (fp);

  read_string (fp, s);

  read_string (fp, s);
  if ( strcmp (cfgP->tag, s) ) {
    Nulle (cfgP->tag);
    cfgP->tag = strdup (s);
  }

  if ( read_int (fp) ) {
    read_string (fp, s);
    cfgP->header  = strdup (s);
  }
  if ( read_int (fp) ) {
    read_string (fp, s);
    cfgP->package = strdup (s);
  }

  cfgP->status    = (CFGStatus)read_int (fp);
  cfgP->weight    = (CFGWeight)read_float (fp);
  cfgP->allPublic = (CFGBool)read_int (fp);
  cfgP->startover = read_float (fp);

  cfgP->importN = read_int (fp);
  cfgP->importA = (CFGRuleRef*)malloc (cfgP->importN * sizeof (CFGRuleRef));
  for (i=0; i<(UINT)cfgP->importN; i++) {
    cfgP->importA[i].lvX     = (LVX)read_int (fp);
    read_string (fp, s);
    cfgP->importA[i].grammar = strdup (s);
  }

  cfgP->ruleRefN = read_int (fp);
  cfgP->ruleRefA = (CFGRuleRef*)malloc (cfgP->ruleRefN * sizeof (CFGRuleRef));
  for (i=0; i<(UINT)cfgP->ruleRefN; i++) {
    cfgP->ruleRefA[i].lvX     = (LVX)read_int (fp);
    read_string (fp, s);
    cfgP->ruleRefA[i].grammar = strdup (s);
  }

  /* read rules */
  ruleN = (CFGRuleX)read_int (fp);
  for (i=0; i<ruleN; i++) {
    lvX    = read_int   (fp);
    type   = read_int   (fp);
    status = read_int   (fp);
    weight = read_float (fp);

    cfgRP         = cfgNewRule (cfgP); cfgRuleInit (cfgRP);
    cfgRP->lvX    = (LVX)lvX;
    cfgRP->type   = (CFGRuleType)type;
    cfgRP->cfgP   = cfgP;
    cfgRP->status = (CFGStatus)status;
    cfgRP->weight = (CFGWeight)weight;
  }

  /* read nodes */
  nodeN = (CFGNodeX)read_int (fp);
  for (i=0; i<nodeN; i++) {
    type  = read_int   (fp);

    cfgNP = cfgNodeCreate (cfgP, (CFGNodeType)type);
  }

  /* read arcs */
  arcN = (CFGArcX)read_int (fp);
  for (i = 0; i < arcN; i++) {
    lvX    = read_int (fp);
    type   = read_int (fp);
    score  = read_float (fp);

    if ( !set || (type & 0x3) != NT_Arc ) {
      ruleX = read_int (fp);
      cfgRP = cfgGetRule (cfgP, (CFGRuleX)ruleX);
    } else
      cfgRP = NULL;

    cfgAP = cfgArcCreate (cfgP, (LVX)lvX, (CFGArcType)type, (CFGScore)score, cfgRP);

    nodeX = read_int (fp);
    cfgNP = cfgGetNode (cfgP, (CFGNodeX)nodeX);
    cfgArcSetNode (cfgAP, cfgNP);
  }

  /* read arcA of nodes */
  for (i=0; i<nodeN; i++) {
    cfgNP = cfgGetNode (cfgP, i);

    cfgNP->arcN = (CFGArcX)read_int (fp);
    cfgNP->arcA = (CFGArc**)malloc (cfgNP->arcN * sizeof (CFGArc*));

    for (j=0; j<cfgNP->arcN; j++) {
      arcX = read_int (fp);
      cfgNP->arcA[j] = cfgGetArc (cfgP, (CFGArcX)arcX);
    }
  }

  /* read roots and leafs of rules */
  for (i=0; i<ruleN; i++) {
    cfgRP = cfgGetRule (cfgP, (CFGRuleX)i);

    nodeX = read_int(fp);
    cfgRP->root = cfgGetNode (cfgP, (CFGNodeX)nodeX);
    nodeX = read_int(fp);
    cfgRP->leaf = cfgGetNode (cfgP, (CFGNodeX)nodeX);
  }

  /* read root, bosNP, eosNP of CFG */
  ruleX = read_int (fp);
  cfgP->root  = cfgGetRule (cfgP, (CFGRuleX)ruleX);
  nodeX = read_int (fp);
  cfgP->bosNP = cfgGetNode (cfgP, (CFGNodeX)nodeX);
  nodeX = read_int (fp);
  cfgP->eosNP = cfgGetNode (cfgP, (CFGNodeX)nodeX);

  /* read lexicon */
  if ( !set ) {
    cfgLexiconLoadDump (cfgP->cfgLP, fp);

    if ( pt ) {
      cfgRuleStackLoadDump (cfgP->cfgRSP, fp, cfgP, NULL);
      cfgParseTreeLoadDump (cfgP->cfgPTP, fp, cfgP, NULL);
    }
  }

  read_string (fp, s);
  if ( strcmp (s, "CFG-END") ) {
    ERROR ("cfgLoadDump: inconsistent file.\n");
    return TCL_ERROR;
  }

  return TCL_OK;
}


int cfgReadFSM (CFG *cfgP, FILE *fp) {

  CFGNode  *fromNP, *toNP;
  CFGArc   *cfgAP;
  CFGRule  *cfgRP;
  int       nXOffset, fromX, toX, argN;
  float     cost;
  CFGScore  score;
  LVX       lvX;
  char      symbol[CFG_MAXLINE];
  char      line[CFG_MAXLINE];
  int      *nodeXA, nodeXN, nodeN, initialX;
  int       i;

  /* build top level rule */
  cfgRP = cfgAddRule (cfgP, "fsmtop", Public_Rule, 1);

  nXOffset = (int)bmemGetItemN (cfgP->mem.nodes);
  nodeN    = -1;
  initialX = -1;
  nodeXN   = 100;
  nodeXA   = (int*)malloc (nodeXN * sizeof(int));
  for (i=0; i<nodeXN; i++) nodeXA[i] = 0;

  /* now we can build the grammar
     we have to read the file twice:
       1. cunstruct all nodes
          and set initial node (first line in file by convention)
       2. link the nodes
     lexicon-index = symbols-index - 1 + lXOffset
     node-index    = node-index + nXOffset
  */
  while ( !feof(fp) && fgets (line, CFG_MAXLINE, fp) ) {
    argN = sscanf (line, "%d %d %s", &fromX, &toX, symbol);

    if ( nodeN == -1 ) initialX = fromX;
    if ( fromX > nodeN || toX > nodeN ) nodeN = (fromX > toX) ? fromX : toX;

    /* we don't need nodes for final transitions */
    if ( argN == 2 ) continue;

    if ( fromX >= nodeXN || toX >= nodeXN ) {
      nodeXN = ((toX > fromX) ? toX : fromX) * 2;
      nodeXA = (int*)realloc (nodeXA, nodeXN * sizeof(int));
      for (;i<nodeXN; i++) nodeXA[i] = 0;
    }

    if ( !nodeXA[fromX] ) {
      cfgNodeCreate (cfgP, Normal_Node);
      nodeXA[fromX] = 1;
    }
  }
  nodeN++;	/* nodeN == last index, so add one */

  fseek (fp, 0L, SEEK_SET);

  /* linking of nodes */
  while ( !feof(fp) && fgets (line, CFG_MAXLINE, fp) ) {
    argN = sscanf (line, "%d %d %s %f", &fromX, &toX, symbol, &cost);

    if ( argN == 4 ) {
      fromNP = cfgGetNode (cfgP, fromX + nXOffset);
      toNP   = cfgGetNode (cfgP,   toX + nXOffset);
      cost   = exp (-1*cost/10000.5);
      score  = CompressScore(P2S(cost));

      if ( !strcmp (symbol, "NULL") ) {
	cfgAP = cfgNodeAddArc (fromNP, cfgP, LVX_NIL, Lambda_Arc, score, cfgRP, 0);
      } else {
	lvX   = cfgLexiconAddItem (cfgP->cfgLP, symbol, T_Arc);
	cfgAP = cfgNodeAddArc (fromNP, cfgP, lvX, T_Arc, score, cfgRP, 0);
      }

      cfgArcSetNode (cfgAP, toNP);

    } else {
      /* final node? */
      argN = sscanf (line, "%d %f", &fromX, &cost);

      if ( argN != 2 ) {
	ERROR ("cfgReadFSM: while reading line '%s'\n", line);
	return TCL_ERROR;
      }

      fromNP = cfgGetNode (cfgP, fromX + nXOffset);
      toNP   = cfgRP->leaf;
      cost   = exp (-1*cost/10000.5);
      score  = CompressScore(P2S(cost));

      /* check if arc already exists, otherwise create new one */
      cfgAP = cfgNodeAddArc (fromNP, cfgP, LVX_NIL, Lambda_Arc, score, cfgRP, 0);
      if ( cfgAP->cfgNP && cfgAP->cfgNP != toNP )
	cfgAP = cfgNodeAddArc (fromNP, cfgP, LVX_NIL, Lambda_Arc, score, cfgRP, 1);
      cfgArcSetNode (cfgAP, toNP);
    }
  }

  /* link rule root node with initial node */
  if ( initialX >= 0 ) {
    toNP   = cfgGetNode (cfgP, initialX + nXOffset); assert (toNP);
    cfgAP  = cfgNodeAddArc (cfgRP->root, cfgP, LVX_NIL, Lambda_Arc, CFG_NULL_SCORE, cfgRP, 0);
    cfgArcSetNode (cfgAP, toNP);
  }

  if ( nodeXA ) Nulle (nodeXA);
  
  return TCL_OK;
}


int cfgReadPFSG (CFG *cfgP, FILE *fp) {

  CFGNode  *fromNP, *toNP;
  CFGArc   *cfgAP;
  CFGRule  *cfgRP;
  int       nXOffset, fromX, toX, argN;
  float     cost;
  CFGScore  score;
  LVX       lvX, *nodeXA;
  int       nodeXN, nodeX, initialX, finalX, transN;
  char     *lP, *w;
  char     *line  = NULL;
  int       lineN = 200000;
  char      name[CFG_MAXLINE];

  line = (char*)malloc (200000 * sizeof(char));

  /* name of pfsg == to level rule name */
  fgets (line, lineN, fp);
  argN = sscanf (line, "name %s", name);

  if ( argN != 1 ) {
    ERROR ("cfgReadPFSG: while reading 'name' line '%s'\n", line);
    return TCL_ERROR;
  }

  /* build top level rule */
  cfgRP    = cfgAddRule (cfgP, name, Public_Rule, 1);
  nXOffset = (int)bmemGetItemN (cfgP->mem.nodes);

  /* now read the nodes line, add all nodes to the lexicon
     and build lookup tables */
  fgets (line, lineN, fp);
  argN = sscanf (line, "nodes %d", &nodeXN);
  if ( argN != 1 ) {
    ERROR ("cfgReadPFSG: while reading 'nodes' line '%s'\n", line);
    return TCL_ERROR;
  }

  nodeX  = 0;
  nodeXA = (LVX*)malloc(nodeXN * sizeof(LVX));

  lP = &line[0];
  w  = strtok (lP, " ");	/* nodes */
  w  = strtok (NULL, " ");	/* nodeN */
  w  = strtok (NULL, " ");	/* first node */
  while ( w ) {
    if ( nodeX == nodeXN - 1 ) { /* remove '\n' for last word */
      char *t = strrchr(w, '\n');
      if ( t ) t[0] = '\0';
    }
    if ( strcmp (w, "NULL") ) {
      lvX = cfgLexiconAddItem (cfgP->cfgLP, w, T_Arc);
      nodeXA[nodeX++] = lvX;
    } else {
      nodeXA[nodeX++] = LVX_NIL;
    }
    w = strtok (NULL, " ");
  }

  /* get the initial state */
  fgets (line, lineN, fp);
  argN = sscanf (line, "initial %d", &initialX);

  if ( argN != 1 ) {
    ERROR ("cfgReadPFSG: while reading 'initial' line '%s'\n", line);
    return TCL_ERROR;
  }

  if ( nodeXA[initialX] != LVX_NIL ) {
    ERROR ("cfgReadPFSG: initial node must be NULL\n");
    return TCL_ERROR;
  }

  /* get the final state */
  fgets (line, lineN, fp);
  argN = sscanf (line, "final %d", &finalX);

  if ( argN != 1 ) {
    ERROR ("cfgReadPFSG: while reading 'final' line '%s'\n", line);
    return TCL_ERROR;
  }

  /* get the number of transitions */
  fgets (line, lineN, fp);
  argN = sscanf (line, "transitions %d", &transN);

  if ( argN != 1 ) {
    ERROR ("cfgReadPFSG: while reading 'transitions' line '%s'\n", line);
    return TCL_ERROR;
  }

  /* now we can start to read all the transitions */
  while ( !feof(fp) && fgets (line, lineN, fp) ) {
    argN = sscanf (line, "%d %d %f", &fromX, &toX, &cost);
    if ( argN != 3 ) {
      ERROR ("cfgReadPFSG: while reading transitions '%s'\n", line);
      return TCL_ERROR;
    }

    /* score and lvX */
    score  = CompressScore(P2S(exp(cost/10000.5)));
    lvX    = nodeXA[toX];
    fromNP = NULL;
    toNP   = NULL;

    /* get from node */
    if ( fromX == finalX ) fromNP = cfgRP->leaf;
    if ( !fromNP ) fromNP = cfgGetNode (cfgP, fromX + nXOffset);
    while ( !fromNP ) {
      cfgNodeCreate (cfgP, Normal_Node);
      fromNP = cfgGetNode (cfgP, fromX + nXOffset);
    }

    /* get to node */
    if ( toX   == finalX ) toNP   = cfgRP->leaf;
    if ( !toNP ) toNP = cfgGetNode (cfgP, toX + nXOffset);
    while ( !toNP ) {
      cfgNodeCreate (cfgP, Normal_Node);
      toNP = cfgGetNode (cfgP, toX + nXOffset);
    }

    /* linking */
    if ( lvX == LVX_NIL ) {
      cfgAP = cfgNodeAddArc (fromNP, cfgP, lvX, Lambda_Arc, score, cfgRP, 0);
      if ( cfgAP->cfgNP && cfgAP->cfgNP != toNP )
	cfgAP = cfgNodeAddArc (fromNP, cfgP, lvX, Lambda_Arc, score, cfgRP, 1);

    } else {
      assert (toX != finalX);
      cfgAP = cfgNodeAddArc (fromNP, cfgP, lvX, T_Arc,      score, cfgRP, 0);
    }
    cfgArcSetNode (cfgAP, toNP);
  }

  /* initial node */
  fromNP = cfgRP->root;
  toNP   = cfgGetNode (cfgP, initialX + nXOffset); assert (toNP);
  cfgAP  = cfgNodeAddArc (fromNP, cfgP, LVX_NIL, Lambda_Arc, CFG_NULL_SCORE, cfgRP, 0);
  cfgArcSetNode (cfgAP, toNP);

  /* final node */
  assert (nodeXA[finalX] == LVX_NIL);

  Nulle (line);

  return TCL_OK;
}

int cfgReadWLAT (CFG *cfgP, FILE *fp) {

  CFGNode  *fromNP = NULL;
  CFGNode  *toNP   = NULL;
  CFGArc   *cfgAP  = NULL;
  CFGRule  *cfgRP  = NULL;
  float     cost, posterior;
  CFGScore  score;
  LVX       lvX;
  int       argN, numaligns, i;
  char     *lP, *w, *p;
  char     *line  = NULL;
  int       lineN = 200000;
  char      name[CFG_MAXLINE];

  line = (char*)malloc (200000 * sizeof(char));

  /* name of pfsg == to level rule name */
  fgets (line, lineN, fp);
  argN = sscanf (line, "name %s", name);
  if ( argN != 1 ) {
    ERROR ("cfgReadWLAT: while reading 'name' line '%s'\n", line);
    return TCL_ERROR;
  }

  /* read numaligns */
  fgets (line, lineN, fp);
  argN = sscanf (line, "numaligns %d", &numaligns);
  if ( argN != 1 ) {
    ERROR ("cfgReadWLAT: while reading 'numaligns' line '%s'\n", line);
    return TCL_ERROR;
  }

  /* read posterior */
  fgets (line, lineN, fp);
  argN = sscanf (line, "posterior %f", &posterior);
  if ( argN != 1 ) {
    ERROR ("cfgReadWLAT: while reading 'posterior' line '%s'\n", line);
    return TCL_ERROR;
  }

  /* build top level rule */
  cfgRP    = cfgAddRule (cfgP, name, Public_Rule, 1);

  /* new we read the aligns */
  for (i=0; i<numaligns; i++) {
    fgets (line, lineN, fp);
    lP = &line[0];
    w  = strtok (lP, " ");	/* align */
    w  = strtok (NULL, " ");	/* align number */

    /* set start and end nodes */
    if (i > 0)              fromNP = toNP;
    else                    fromNP = cfgRP->root;
    if ( i == numaligns-1 ) toNP   = cfgRP->leaf;
    else                    toNP   = cfgNodeCreate (cfgP, Normal_Node);

    w  = strtok (NULL, " ");	/* first word */
    p  = strtok (NULL, " ");	/* first prob */
    while ( w && p ) {
      cost  = atof(p);
      score = CompressScore(P2S(cost));

      if ( strcmp (w, "*DELETE*") ) {
	lvX    = cfgLexiconAddItem (cfgP->cfgLP, w, T_Arc);
	cfgAP  = cfgNodeAddArc (fromNP, cfgP, lvX,     T_Arc,      score, cfgRP, 0);
      } else {
	cfgAP  = cfgNodeAddArc (fromNP, cfgP, LVX_NIL, Lambda_Arc, score, cfgRP, 0);
      }

      cfgArcSetNode (cfgAP, toNP);

      w  = strtok (NULL, " ");	/* next word */
      p  = strtok (NULL, " ");	/* next prob */
    }    
  }

  Nulle (line);

  return TCL_OK;
}


int cfgLoad (CFG *cfgP, char *fileName, CFGGrammarFormat f, int v) {

  FILE *fp  = NULL;
  int   res = TCL_ERROR;

  if ( !(fp = fileOpen (fileName, "rt")) ) {
    ERROR ("cfgLoad: cannot open file '%s' for reading\n", fileName);
    return TCL_ERROR;
  }

  if ( !f ) f = detectFileFormat (fp);

  switch (f) {
  case SOUP_Format: if (v) INFO ("cfgLoad: SOUP\n"); res = cfgReadSOUP (cfgP, fp);    break;
  case JSGF_Format: if (v) INFO ("cfgLoad: JSGF\n"); res = cfgReadJSGF (cfgP, fp, v); break;
  case FSM_Format:  if (v) INFO ("cfgLoad: FSM\n");  res = cfgReadFSM  (cfgP, fp);    break;
  case PFSG_Format: if (v) INFO ("cfgLoad: PFSG\n"); res = cfgReadPFSG (cfgP, fp);    break;
  case WLAT_Format: if (v) INFO ("cfgLoad: WLAT\n"); res = cfgReadWLAT (cfgP, fp);    break;
  case Dump_Format: if (v) INFO ("cfgLoad: DUMP\n"); res = cfgLoadDump (cfgP, fp, 0); break;
  default:          ERROR ("cfgLoad: unknown file format %d=%s\n",
			   f, cfgGrammarFormat2str (f));
  }

  fileClose (fileName, fp);

  cfgClear (cfgP, 0);

  if ( res != TCL_OK )
    ERROR ("cfgLoad: in file %s.\n", fileName);

  return res;
}

static int cfgLoadItf (ClientData cd, int argc, char *argv[]) {

  CFG  *cfgP       = (CFG*)cd;
  char *fileName   = NULL;
  char *format     = "";
  int  verbose     = 0;

  argc--;
  if ( itfParseArgv (argv[0], &argc, argv+1, 0,
                     "<fileName>", ARGV_STRING, NULL, &fileName,   NULL, "file name",
		     "-format",    ARGV_STRING, NULL, &format,     NULL, "grammar format (soup|jsgf|fsm|pfsg|dump)",
		     "-verbose",   ARGV_INT,    NULL, &verbose,    NULL, "verbosity level",
                     NULL) != TCL_OK )
    return TCL_ERROR;

  return cfgLoad (cfgP, fileName, cfgGrammarFormat2int (format), verbose);
}

/* ----------------------------------------------------------------------
   cfg* functions for JSGF support
   ---------------------------------------------------------------------- */

int cfgInitHeader (CFG *cfgP, char *line) {

  cfgP->header = strdup(line);

  return TCL_OK;
}

int cfgInitName (CFG *cfgP, char *line) {

  char *package = NULL;
  char *name    = line;
  char *lP      = NULL;

  if ( (lP = strrchr (line, '.')) ) {
    lP[0]   = '\0';
    package = line;
    name    = lP+1;
  }

  if ( package ) cfgP->package = strdup (package);

  if ( strcmp (cfgP->tag, name) ) {
    Nulle (cfgP->tag);
    cfgP->tag = strdup (name);
  }

  return TCL_OK;
}

int cfgAddImport (CFG *cfgP, char *token) {

  char *lP      = NULL;
  char *grammar = NULL;
  char *rule    = NULL;
  LVX   lvX     = LVX_NIL;

  /* remove surrounding <> */
  if ( (token = strchr(token, '<')) ) {

    if ( token[0] == '<' ) {
      token++;
      token[strcspn(token, ">")] = '\0';
    } else {
      ERROR ("cfgAddImport: wrong import token '%s'.\n", token);
      return TCL_ERROR;
    }
  } else {
    ERROR ("cfgAddImport: wrong import token '%s'.\n", token);
    return TCL_ERROR;
  }

  /* split rule name */
  if ( (lP = strrchr (token, '.')) ) {
    lP[0]   = '\0';
    grammar = token;
    rule    = lP+1;
  } else {
    ERROR ("cfgAddImport: wrong import token '%s'.\n", token);
    return TCL_ERROR;
  }

  /* add pair to import list */
  if ( !grammar ) {
    ERROR ("cfgAddImport: no grammar given to rule '%s'.\n", rule);
    return TCL_ERROR;
  }

  lvX = cfgLexiconAddItem (cfgP->cfgLP, rule, NT_Arc);

  cfgP->importA = (CFGRuleRef*)realloc (cfgP->importA,
					(cfgP->importN+1) * sizeof (CFGRuleRef));
  cfgP->importA[cfgP->importN].lvX     = lvX;
  cfgP->importA[cfgP->importN].grammar = strdup(grammar);
  cfgP->importN++;

  return TCL_OK;
}

int cfgAddRuleReference (CFG *cfgP, LVX lvX, char *grammar) {

  cfgP->ruleRefA = (CFGRuleRef*)realloc (cfgP->ruleRefA,
					 (cfgP->ruleRefN+1) * sizeof (CFGRuleRef));
  cfgP->ruleRefA[cfgP->ruleRefN].lvX     = lvX;
  cfgP->ruleRefA[cfgP->ruleRefN].grammar = strdup(grammar);
  cfgP->ruleRefN++;

  return TCL_OK;
}


/* ----------------------------------------------------------------------
   cfgSave
   ---------------------------------------------------------------------- */
int cfgSaveDump (CFG *cfgP, FILE *fp, int set, int pt) {

  CFGArcX   arcN  = bmemGetItemN (cfgP->mem.arcs);
  CFGNodeX  nodeN = bmemGetItemN (cfgP->mem.nodes);
  CFGRuleX  ruleN = bmemGetItemN (cfgP->mem.rules);
  CFGArc   *cfgAP;
  CFGNode  *cfgNP;
  CFGRule  *cfgRP;
  UINT     i, j;

  if ( !fp ) {
    ERROR ("cfgSaveDump: invalid file pointer.\n");
    return TCL_ERROR;
  }

  if ( cfgP->cfgSP && !set ) {
    ERROR ("cfgSaveDump: dumping of only one grammar of a set.\n");
    return TCL_ERROR;
  }

  assert ( (cfgP->cfgSP && set) || (!cfgP->cfgSP && !set) );

  write_string (fp, "CFG-BEGIN 1.1");

  /* write CFG */
  write_int    (fp, set ? 1 : 0);
  write_int    (fp, pt  ? 1 : 0);
  write_string (fp, cfgP->name);
  write_string (fp, cfgP->tag);
  if ( cfgP->header ) {
    write_int    (fp, 1);
    write_string (fp, cfgP->header);
  } else {
    write_int    (fp, 0);
  }
  if ( cfgP->package ) {
    write_int    (fp, 1);
    write_string (fp, cfgP->package);
  } else {
    write_int    (fp, 0);
  }
  write_int    (fp, (int)cfgP->status);
  write_float  (fp, (float)cfgP->weight);
  write_int    (fp, (int)cfgP->allPublic);
  write_float  (fp, cfgP->startover);

  write_int (fp, cfgP->importN);
  for (i=0; i<(UINT)cfgP->importN; i++) {
    write_int    (fp, (int)cfgP->importA[i].lvX);
    write_string (fp, cfgP->importA[i].grammar);
  }

  write_int (fp, cfgP->ruleRefN);
  for (i=0; i<(UINT)cfgP->ruleRefN; i++) {
    write_int    (fp, (int)cfgP->importA[i].lvX);
    write_string (fp, cfgP->importA[i].grammar);
  }

  /* write rules */
  write_int (fp, (int)ruleN);
  for (i=0; i<ruleN; i++) {
    cfgRP = cfgGetRule (cfgP, (CFGRuleX)i);

    write_int   (fp, (int)cfgRP->lvX);
    write_int   (fp, (int)cfgRP->type);
    write_int   (fp, (int)cfgRP->status);
    write_float (fp, (float)cfgRP->weight);
  }

  /* write nodes */
  write_int (fp, (int)nodeN);
  for (i=0; i<nodeN; i++) {
    cfgNP = cfgGetNode (cfgP, i);

    write_int   (fp, (int)cfgNP->type);
  }

  /* write arcs */
  write_int (fp, (int)arcN);
  for (i=0; i<arcN; i++) {
    cfgAP = cfgGetArc (cfgP, i);

    write_int   (fp, (int)cfgAP->lvX);
    write_int   (fp, (int)cfgAP->type);
    write_float (fp, (float)cfgAP->score);

    if ( !set || (cfgAP->type & 0x3) != NT_Arc )
      write_int (fp, (int)cfgGetRuleX (cfgP, cfgAP->cfgRP));

    write_int   (fp, (int)cfgGetNodeX (cfgP, cfgAP->cfgNP));
  }

  /* write arcA of nodes */
  for (i=0; i<nodeN; i++) {
    cfgNP = cfgGetNode (cfgP, i);

    write_int   (fp, (int)cfgNP->arcN);
    for (j = 0; j < cfgNP->arcN; j++)
      write_int (fp, (int)cfgGetArcX (cfgP, cfgNP->arcA[j]));
  }

  /* write roots and leafs of rules */
  for (i=0; i<ruleN; i++) {
    cfgRP = cfgGetRule (cfgP, (CFGRuleX)i);

    write_int (fp, (int)cfgGetNodeX (cfgP, cfgRP->root));
    write_int (fp, (int)cfgGetNodeX (cfgP, cfgRP->leaf));
  }

  /* write root, bosNP, eosNP of CFG */
  write_int (fp, (int)cfgGetRuleX (cfgP, cfgP->root));
  write_int (fp, (int)cfgGetNodeX (cfgP, cfgP->bosNP));
  write_int (fp, (int)cfgGetNodeX (cfgP, cfgP->eosNP));

  /* write lexicon */
  if ( !set ) {
    cfgLexiconSaveDump (cfgP->cfgLP, fp);
    if ( pt ) {
      cfgRuleStackSaveDump (cfgP->cfgRSP, fp, NULL);
      cfgParseTreeSaveDump (cfgP->cfgPTP, fp, NULL);
    }
  }

  write_string (fp, "CFG-END");

  return TCL_OK;
}

int cfgSaveFSM (CFG *cfgP, FILE *fp) {

  return TCL_OK;
}

int cfgSave (CFG *cfgP, char *fileName, CFGGrammarFormat f, int pt) {

  FILE *fp  = NULL;
  int   res = TCL_ERROR;

  if ( !(fp = fileOpen (fileName, "w")) ) {
    ERROR ("cfgSave: cannot open file '%s' for writing\n", fileName);
    return TCL_ERROR;
  }

  switch (f) {
  case FSM_Format:  res = cfgSaveFSM  (cfgP, fp);        break;
  case Dump_Format: res = cfgSaveDump (cfgP, fp, 0, pt); break;
  default:          ERROR ("cfgSave: unknown file format %d=%s\n",
			   f, cfgGrammarFormat2str (f));
  }

  fileClose (fileName, fp);

  if ( res != TCL_OK )
    ERROR ("cfgSave: in file %s.\n", fileName);

  return res;
}

static int cfgSaveItf (ClientData cd, int argc, char *argv[]) {

  CFG  *cfgP       = (CFG*)cd;
  char *fileName   = NULL;
  char *format     = "dump";
  int   pt         = 0;

  argc--;
  if ( itfParseArgv (argv[0], &argc, argv+1, 0,
                     "<fileName>", ARGV_STRING, NULL, &fileName,   NULL,
                     "file name",
		     "-pt",        ARGV_INT,    NULL, &pt,         NULL,
		     "dump also parse tree and rule stack",
		     "-format",    ARGV_STRING, NULL, &format,     NULL,
		     "grammar format",
                     NULL) != TCL_OK )
    return TCL_ERROR;

  return cfgSave (cfgP, fileName, cfgGrammarFormat2int (format), pt);
}


/* ----------------------------------------------------------------------
   cfgGetRuleByLVX
   returns pointer to rule given by LVX
   ---------------------------------------------------------------------- */
CFGRule* cfgGetRuleByLVX (CFG *cfgP, LVX lvX) {

  CFGRule  *cfgRP = NULL;
  CFGRuleX  i;

  for (i=0; i<bmemGetItemN (cfgP->mem.rules); i++) {
    cfgRP = (CFGRule*)bmemGetItem (cfgP->mem.rules, i);
    if ( lvX == cfgRP->lvX ) return cfgRP;
  }

  return NULL;
}

/* ----------------------------------------------------------------------
   cfgGetRuleByImport
   returns pointer to rule given by LVX
   ---------------------------------------------------------------------- */
CFGRule* cfgGetRuleByImport (CFG *cfgP, LVX lvX) {

  int i = 0;

  if ( !cfgP->cfgSP ) {
    ERROR ("cfgGetRuleByImport: grammar does not belong to a set.\n");
    return NULL;
  }

  while ( i < cfgP->importN && cfgP->importA[i].lvX != lvX ) i++;

  if ( i < cfgP->importN ) {
    CFG *cfg;

    /* search for the right grammar */
    cfg = cfgSetGetCFGByName (cfgP->cfgSP, cfgP->importA[i].grammar);

    if ( cfg ) return cfgGetRuleByLVX (cfg, lvX);
  }

  return NULL;
}

/* ----------------------------------------------------------------------
   cfgAddRule
   ---------------------------------------------------------------------- */

/* difference to cfgRuleCreate is that the LVX is produced in here */
CFGRule* cfgAddRule (CFG *cfgP, char *name, CFGRuleType type, int unique) {

  LVX      lvX   = cfgLexiconAddItem (cfgP->cfgLP, name, NT_Arc);
  CFGRule *cfgRP = cfgGetRuleByLVX   (cfgP, lvX);

  if ( !cfgRP ) cfgRP = cfgRuleCreate (cfgP, lvX, type);
  else {
    if ( unique ) {
      ERROR ("cfgAddRule: rule %s already exists.\n", name);
      return NULL;
    } else {
      if ( cfgRP->type != type ) {
	ERROR ("cfgAddRule: rule %s with different type (%s <-> %s) already exists.\n",
	       name, cfgRuleType2str (cfgRP->type), cfgRuleType2str (type));
	return NULL;
      }
    }
  }

  return cfgRP;
}

/* ----------------------------------------------------------------------
   cfgBuild
   ---------------------------------------------------------------------- */

/* fill all empty rule pointers */
int cfgBuild (CFG *cfgP, CFGBuildMode mode, int reset, int verbose) {

  LVX      lvX;
  CFGArc  *cfgAP, *bosAP, *eosAP;
  CFGRule *cfgRP;
  CFGNode *cfgNP;
  CFGArcX  i;
  CFGRuleX j;
  CFGNodeX k;
  float    prob;
  CFGScore score;
  int      undefN, publicN;
  int      res = TCL_OK;

  if ( cfgP->built == 1 ) return TCL_OK;

  if ( mode != DEFAULT_SCORES ) cfgP->buildMode = mode;

  /* first call of build */
  if ( !cfgP->root ) {

    lvX        = cfgLexiconAddItem (cfgP->cfgLP, "_ROOT_", NT_Arc);
    cfgP->root = cfgRuleCreate (cfgP, lvX, Root_Rule);
    cfgRP      = cfgP->root;

    /* build bos and eos structure */
    cfgP->bosNP = cfgNodeCreate (cfgP, Normal_Node);
    cfgP->eosNP = cfgNodeCreate (cfgP, Normal_Node);

    /* root -> bosAP */
    lvX   = cfgLexiconAddItem (cfgP->cfgLP, cfgP->cfgLP->beginOS, T_Arc);
    bosAP = cfgNodeAddArc     (cfgRP->root, cfgP, lvX, T_Arc,
			       CFG_INIT_SCORE, cfgRP, 0);
    cfgArcSetNode (bosAP, cfgP->bosNP);

    /* eosAP -> leaf */
    lvX   = cfgLexiconAddItem (cfgP->cfgLP, cfgP->cfgLP->endOS, T_Arc);
    eosAP = cfgNodeAddArc     (cfgP->eosNP, cfgP, lvX, T_Arc,
			       CFG_INIT_SCORE, cfgRP, 0);
    cfgArcSetNode (eosAP, cfgRP->leaf);

    /* lambda arc for startover */
    cfgAP = cfgNodeAddArc (cfgP->eosNP, cfgP, LVX_NIL, Lambda_Arc,
			   CFG_FIXED_SCORE, cfgRP, 0);
    cfgArcSetNode (cfgAP, cfgP->bosNP);

    /* add <NULL> and <VOID> rule */
    cfgRP = cfgAddRule    (cfgP, "NULL", Null_Rule, 0);
    cfgAP = cfgNodeAddArc (cfgRP->root, cfgP, LVX_NIL, Lambda_Arc,
			   CFG_NULL_SCORE, cfgRP, 0);
    cfgArcSetNode (cfgAP, cfgRP->leaf);
    
    cfgRP = cfgAddRule    (cfgP, "VOID", Void_Rule, 0);
    cfgAP = cfgNodeAddArc (cfgRP->root, cfgP, LVX_NIL, Lambda_Arc,
			   CFG_MAX_SCORE, cfgRP, 0);
    cfgArcSetNode (cfgAP, cfgRP->leaf);
  } else {
    cfgRP = cfgP->root;
    assert (cfgRP->root->arcN);
    bosAP = cfgRP->root->arcA[0];
    eosAP = cfgP->eosNP->arcA[0];
  }

  /* set scores for bos/eos transitions */
  switch (cfgP->buildMode) {
  case NULL_SCORES:
    bosAP->score = eosAP->score = CFG_NULL_SCORE;
    break;
  case FIXED_SCORES:
    bosAP->score = eosAP->score = CFG_FIXED_SCORE;
    break;
  case EQUAL_SCORES:
    bosAP->score = CompressScore(P2S(1.0));
    assert (cfgP->eosNP->arcN == 2);
    cfgP->eosNP->arcA[0]->score = CompressScore(P2S(0.5));
    cfgP->eosNP->arcA[1]->score = CompressScore(P2S(0.5));
    break;
  }

  /* link NT_Arcs to rules */
  for (i=0; i<bmemGetItemN (cfgP->mem.arcs); i++) {
    cfgAP = (CFGArc*)bmemGetItem (cfgP->mem.arcs, i);

    /* search for rule given by LVX */
    if ( (cfgAP->type & 0x3) == NT_Arc && cfgAP->cfgRP == NULL ) {

      /* in local grammar */
      if ( !(cfgRP = cfgGetRuleByLVX (cfgP, cfgAP->lvX)) ) {

	/* in shared grammar */
	CFG *scfgP = cfgSetGetSharedCFG (cfgP->cfgSP);
	if ( !scfgP || !(cfgRP = cfgGetRuleByLVX (scfgP, cfgAP->lvX)) ) {

	  if ( !cfgP->importN || !(cfgRP = cfgGetRuleByImport (cfgP, cfgAP->lvX)) ) {
	    ERROR ("rule %s referenced, but not defined.\n",
		   cfgLexiconGetName (cfgP->cfgLP, cfgAP->lvX, cfgAP->type));
	    res = TCL_ERROR;
	  } 
	}
      }

      if ( cfgRP ) cfgAP->cfgRP = cfgRP;
    }
  }

  /* reset scores */
  for (k=0; k<bmemGetItemN (cfgP->mem.nodes); k++) {
    cfgNP = (CFGNode*)bmemGetItem (cfgP->mem.nodes, k);
    
    switch (cfgP->buildMode) {
    case NULL_SCORES:
      for (i=0; i<cfgNP->arcN; i++) {
	cfgAP = cfgNP->arcA[i];
	if ( reset || cfgAP->score == CFG_INIT_SCORE )
	  cfgAP->score = CFG_NULL_SCORE;
      }
      break;
    case FIXED_SCORES:
      /* CF: do really nothing
      for (i=0; i<cfgNP->arcN; i++) {
	cfgAP = cfgNP->arcA[i];
	if ( reset || cfgAP->score == CFG_INIT_SCORE ) {
	  if ( (cfgAP->type & 0x3) == T_Arc ) {
	    cfgAP->score = CFG_FIXED_SCORE;
	  } else {
	    cfgAP->score = CFG_NULL_SCORE;
	  }
	}
      }
      */
      break;
    case EQUAL_SCORES:
      prob   = 0.0;
      undefN = 0;
      for (i=0; i<cfgNP->arcN; i++) {
	cfgAP = cfgNP->arcA[i];
	if ( cfgAP->score != CFG_INIT_SCORE ) {
	  prob += S2P(UnCompressScore(cfgAP->score));
	} else {
	  undefN++;
	}
      }
      if ( reset ) undefN = cfgNP->arcN;
      prob = (1.0 - prob) / undefN;
      for (i=0; i<cfgNP->arcN; i++) {
	cfgAP = cfgNP->arcA[i];
	if ( reset || cfgAP->score == CFG_INIT_SCORE )
	  cfgAP->score = CompressScore(P2S(prob));
      }
      break;
    }
  }

  /* add top level rules */
  switch (cfgP->buildMode) {
  case EQUAL_SCORES:
    publicN = 0;
    for (j=0; j<bmemGetItemN (cfgP->mem.rules); j++) {
      cfgRP = (CFGRule*)bmemGetItem (cfgP->mem.rules, j);
      
      if (cfgP->allPublic == 1 || cfgRP->type == Public_Rule ) {
	publicN++;
      }
    }
    prob  = 1.0/publicN;
    score = CompressScore(P2S(prob));
    break;
  default:
    score = CFG_NULL_SCORE;
  }

  for (j=0; j<bmemGetItemN (cfgP->mem.rules); j++) {
    cfgRP = (CFGRule*)bmemGetItem (cfgP->mem.rules, j);
      
    if (cfgP->allPublic == 1 || cfgRP->type == Public_Rule ) {
      cfgAP = cfgNodeAddArc (cfgP->bosNP, cfgP, cfgRP->lvX, NT_Arc, score, cfgRP, 0);
      cfgArcSetNode (cfgAP, cfgP->eosNP);
    }
  }

  /* build rule stack and parse tree */
  if ( !cfgP->cfgSP ) {
    cfgRuleStackBuild (cfgP->cfgRSP);
    cfgParseTreeBuild (cfgP->cfgPTP);
  }
  
  cfgParseTreeBuildCFG (cfgP->cfgPTP, bosAP, cfgP->cfgRSP->root, CFG_NULL_SCORE);

  if ( verbose ) {
    INFO ("Built %10s: nodeN %d, arcN %d, ruleN %d\n", cfgP->name,
	  bmemGetItemN (cfgP->mem.nodes),
	  bmemGetItemN (cfgP->mem.arcs),
	  bmemGetItemN (cfgP->mem.rules));
  }

  cfgP->built = 1;

  return res;
}

static int cfgBuildItf (ClientData cd, int argc, char *argv[]) {

  CFG  *cfgP      = (CFG*)cd;
  int   verbose   = 1;
  char *mode      = "default";
  int   reset     = 0;
  int   m         = 0;
  
  argc--;
  if ( itfParseArgv (argv[0], &argc, argv+1, 0,
		     "-mode",      ARGV_STRING, NULL, &mode,    NULL,
		     "score definition mode (null|fixed|equal|default=use stored build mode)",
		     "-overwrite", ARGV_INT,    NULL, &reset,   NULL,
		     "overwrite predefined scores from grammar file",
		     "-verbose",   ARGV_INT,    NULL, &verbose, NULL,
		     "verbosity",
		     NULL) != TCL_OK )
    return TCL_ERROR;

  m = cfgBuildMode2int (mode);
  if ( !m ) {
    ERROR("unknown build mode '%s'\n", mode);
    return TCL_ERROR;
  }

  if ( cfgP->built ) {
    ERROR("grammar already built\n");
    return TCL_OK;
  }

  return cfgBuild (cfgP, (CFGBuildMode)m, reset, verbose);
}

/* ----------------------------------------------------------------------
   cfgClear
   ---------------------------------------------------------------------- */
int cfgClear (CFG *cfgP, int free) {

  if ( cfgP->built != 0 ) {
    cfgRuleStackClear (cfgP->cfgRSP, free);
    cfgParseTreeClear (cfgP->cfgPTP, free);

    cfgP->built = 0;
    if ( cfgP->lksP ) cfgP->lksP->dirty = 1;

    if ( cfgP->cfgSP ) cfgSetClear (cfgP->cfgSP, free);
  }

  return TCL_OK;
}

static int cfgClearItf (ClientData cd, int argc, char *argv[]) {

  CFG *cfgP = (CFG*)cd;
  int  free = 0;

  argc--;
  if ( itfParseArgv (argv[0], &argc, argv+1, 0,
		     "-free",  ARGV_INT,    NULL, &free,    NULL,
		     "free items instead of clearing",
		     NULL) != TCL_OK )
    return TCL_ERROR;

  return cfgClear (cfgP, free);
}


/* ----------------------------------------------------------------------
   cfgAddPath
   ---------------------------------------------------------------------- */

int cfgAddPath (CFG *cfgP, char *rule, char *line, CFGGrammarFormat f) {

  CFGRule *cfgRP = NULL;
  char    *str   = NULL;
  int      rn    = 0;
  char    *rline;

  switch (f) {
  case SOUP_Format:
    rline = (char*)malloc (strlen(line) * sizeof(char));
    cfgReadFilter (line, &rline, strlen(line), &rn, SOUP_Format);

    if ( parseSOUPRule (cfgP, rule, &cfgRP, 0) != TCL_OK ) {
      ERROR ("cfgAddPath: while adding rule to grammer.\n");
      return TCL_ERROR;
    }
    if ( parseSOUPRuleBody (cfgP, rline, cfgRP) != TCL_OK ) {
      ERROR ("cfgAddPath: while adding line to grammer.\n");
      return TCL_ERROR;
    }

    if ( rline ) Nulle (rline);
    break;
  case JSGF_Format:
    str = (char*)malloc ( (strlen (rule) + strlen (line) + 5) * sizeof (char));
    sprintf (str, "%s = %s", rule, line);

    rline = (char*)malloc (strlen(str) * sizeof(char));
    cfgReadFilter (str, &rline, strlen(str), &rn, JSGF_Format);

    if ( parseJSGFRule (cfgP, rline, &cfgRP, 0) != TCL_OK ) {
      ERROR ("cfgAddPath: while adding line to grammer.\n");
      return TCL_ERROR;
    }

    if ( str )   Nulle (str);
    if ( rline ) Nulle (rline);
    break;
  default:
    ERROR ("cfgAddPath: unknown grammar format.\n");
    return TCL_ERROR;
  }

  cfgClear (cfgP, 0);

  return TCL_OK;
}

static int cfgAddPathItf (ClientData cd, int argc, char *argv[]) {

  CFG     *cfgP   = (CFG*)cd;
  char    *format = "soup";
  char     rule[CFG_MAXLINE];
  char    *rP     = &rule[0];
  char     line[CFG_MAXLINE];
  char    *lP     = &line[0];

  rule[0] = line[0] = '\0';

  argc--;
  if ( itfParseArgv (argv[0], &argc, argv+1, 0,
                     "<rule>",  ARGV_STRING, NULL, &rP,        NULL,
                     "rule to add path",
                     "<line>",  ARGV_STRING, NULL, &lP,        NULL,
                     "path to add",
		     "-format", ARGV_STRING, NULL, &format,    NULL,
		     "grammar format",
                     NULL) != TCL_OK )
    return TCL_ERROR;

  return cfgAddPath (cfgP, rP, lP, cfgGrammarFormat2int (format));
}

/* ----------------------------------------------------------------------
   cfgParse
   ---------------------------------------------------------------------- */
CFGPTNode* cfgParse (CFG *cfgP, char *text, int verbose) {

  char  whiteSep[] = " \t\n";
  char  str[CFG_MAXLINE];
  char *sP         = &str[0];
  char *w          = NULL;

  int   i          = 0;
  int   N;

  LVX   lvXA[CFG_MAXLINE];
  LVX   lvX;
  LCT   lctA[CFG_MAXLINE];

  CFGPTNode  *cfgPTNP;

  /* check if the grammar is in an unstable state */
  if ( !cfgP->built ) cfgBuild (cfgP, DEFAULT_SCORES, 0, 0);
  lvXA[0] = LVX_NIL;

  /* --------------------------------------------------
     build input string -> build lvXA
     -------------------------------------------------- */
  sprintf (sP, "%s %s %s", cfgP->cfgLP->beginOS, text, cfgP->cfgLP->endOS);

  w = strtok (sP, whiteSep);
  while ( w ) {
    lvX     = cfgLexiconGetIndex (cfgP->cfgLP, w, T_Arc);
    lvXA[i] = lvX;

    if ( lvXA[i] == LVX_NIL ) {
      ERROR ("cfgParse: can't find lvX for %s.\n", w);
      return NULL;
    }

    i++;
    w = strtok (NULL, whiteSep);
  }

  /* --------------------------------------------------
     start parsing
     -------------------------------------------------- */
  N = i; i = 0;

  /* create initial LCT */
  lctA[i] = nl2lct(cfgP->cfgPTP->root, lvXA[i]);

  for (i=0; i<N-1; i++) {
    /* extend old LCT */
    cfgPTNP   = cfgPTNodeExtend (lct2node(lctA[i]), lct2lvx(lctA[i]));

    /* build new LCT */
    lctA[i+1] = nl2lct(cfgPTNP, lvXA[i+1]);
  }

  cfgPTNP = cfgPTNodeExtend (lct2node(lctA[i]), lct2lvx(lctA[i]));

  if ( verbose ) {
    printf ("LVX     :");
    for (i=0; i<N; i++) {
      lvX = lct2lvx(lctA[i]);
      printf ("%6d", lvX);
    }
    printf ("\n");

    printf ("PTNodeX :");
    printf ("  root");
    for (i=2; i<N; i++) {
      printf ("%6d", cfgParseTreeGetNodeX (cfgP->cfgPTP, lct2node(lctA[i])));
    }
    printf ("%6d", cfgParseTreeGetNodeX (cfgP->cfgPTP, cfgPTNP));
    printf ("\n"); fflush (stdout);
  }

  return cfgPTNP;
}

static int cfgParseItf (ClientData cd, int argc, char *argv[]) {

  CFG       *cfgP    = (CFG*)cd;
  char       line[CFG_MAXLINE];
  char      *lP      = &line[0];
  int        verbose = 0;
  CFGPTNode *cfgPTNP = NULL;

  line[0] = '\0';

  argc--;
  if ( itfParseArgv (argv[0], &argc, argv+1, 0,
                     "<text>",   ARGV_STRING, NULL, &lP,        NULL,
                     "text to path",
		     "-verbose", ARGV_INT,    NULL, &verbose,   NULL,
		     "verbosity",
                     NULL) != TCL_OK )
    return TCL_ERROR;

  if ( (cfgPTNP = cfgParse (cfgP, lP, verbose)) == NULL ) {
    return TCL_ERROR;
  }

  itfAppendResult ("%d", cfgParseTreeGetNodeX (cfgP->cfgPTP, cfgPTNP));

  return TCL_OK;
}

/* ----------------------------------------------------------------------
   cfgCompress
   ---------------------------------------------------------------------- */
static int cfgCompressDFS (CFG *cfgP, CFGPTNode *cfgPTNP, CFGTraverse *cfgTP, int level, int verbose) {

  while ( cfgPTNP ) {
    if ( verbose )
      fprintf (stderr, "cfgCompressDFS (%d): '%s'\n",
	       level, cfgLexiconGetName (cfgP->cfgLP, cfgPTNP->lvX, T_Arc));

    if ( cfgTP->cacheA ) memset (cfgTP->cacheA, 0, cfgTP->cacheN * sizeof (void*));

    cfgPTNodeCompress (cfgPTNP, cfgTP, verbose);

    if ( cfgPTNP->childP && level > 1 )
      cfgCompressDFS (cfgP, cfgPTNP->childP, cfgTP, level-1, verbose);

    cfgPTNP = cfgPTNP->nextP;
  }

  return TCL_OK;
}

static int cfgCompressBuildDFS (CFG *cfgP, CFGTraverse *cfgTP, CFGPTNode *cfgPTNP, CFGNode *cfgNP, CFGRule *cfgRP, int verbose) {

  CFGArc  *cfgAP = NULL;
  CFGNode *node  = NULL;

  while ( cfgPTNP ) {
    if ( verbose > 1 )
      fprintf (stderr, "cfgCompressBuildDFS: reduced %d to 1 for '%s->%s'\n",
	       cfgPTNP->itemN,
	       cfgPTNP->parentP ? cfgLexiconGetName (cfgP->cfgLP, cfgPTNP->parentP->lvX, T_Arc) : NULL,
	       cfgLexiconGetName (cfgP->cfgLP, cfgPTNP->lvX, T_Arc));

    cfgAP = cfgNodeAddArc (cfgNP, cfgP, cfgPTNP->lvX, T_Arc, cfgPTNP->bestScore, cfgRP, 0);

    node  = cfgNodeCreate (cfgP, Normal_Node);
    cfgArcSetNode (cfgAP, node);

    if ( cfgPTNP->childP ) {
      cfgCompressBuildDFS (cfgP, cfgTP, cfgPTNP->childP, cfgAP->cfgNP, cfgRP, verbose);
    } else {
      cfgTP->cfgP   = cfgP;
      cfgTP->cfgRP  = cfgRP;
      cfgTP->cfgNP  = cfgAP->cfgNP;

      if ( verbose > 1 )
	fprintf (stderr, "cfgCompressBuildDFS: unfold '%s'\n",
		 cfgLexiconGetName (cfgP->cfgLP, cfgPTNP->lvX, T_Arc));

      cfgPTNodeCompressBuild (cfgPTNP, cfgTP, verbose);
    }

    cfgPTNP = cfgPTNP->nextP;
  }

  return TCL_OK;
}

int cfgCompress (CFG *cfgP, int level, int unfold, char *matchFile, int verbose) {

  CFGPTNode   *cfgPTNP = cfgP->cfgPTP->root;
  CFGTraverse *cfgTP   = NULL;
  CFGRule     *cfgRP   = NULL;
  CFGArc      *cfgAP;
  CFGArcX      i;

  /* check if the grammar is in an unstable state */
  if ( !cfgP->built ) cfgBuild (cfgP, DEFAULT_SCORES, 0, 0);

  /* init cfgTraverse */
  cfgTP         = cfgTraverseCreate ();
  cfgTP->cfgP   = cfgP;
  cfgTP->cacheN = cfgLexiconGetItemN (cfgP->cfgLP, T_Arc);
  cfgTP->cacheA = calloc (cfgTP->cacheN, sizeof (void*));

  /* read match file */
  if ( matchFile ) {
    FILE *fp = fileOpen (matchFile, "r");
    LVX   i  = cfgLexiconGetItemN (cfgP->cfgLP, T_Arc);
    int   n;
    char  line[CFG_MAXLINE];

    cfgTP->lvXA = calloc (i, sizeof (LVX));

    if ( verbose )
      fprintf (stderr, "cfgCompress: using matchFile '%s'", matchFile);

    if ( verbose > 1 )
      fprintf (stderr, " with content:\n");
    else
      fprintf (stderr, "\n");

    while ( !feof (fp) && fgets (line, CFG_MAXLINE, fp) ) {
      n = strlen(line);
      i = LVX_NIL;
      if ( n ) {
	line[n-1] = '\0';
	i         = cfgLexiconGetIndex (cfgP->cfgLP, line, T_Arc);
      }
      if ( i == LVX_NIL )
	WARN ("cfgCompress: '%s' is not element of lexicon.\n", line);
      else {
	cfgTP->lvXA[i] = 1;
	if ( verbose > 1 ) fprintf (stderr, "%s ", line);
      }
    }

    fileClose (matchFile, fp);
    if ( verbose > 1 ) fprintf (stderr, "\n");
  }

  if ( verbose )
    fprintf (stderr, "cfgCompress: expanding %d. level Ts\n", level);

  /* compress grammar */
  cfgCompressDFS (cfgP, cfgPTNP, cfgTP, level, verbose);

  cfgTraverseFree (cfgTP);

  if ( !unfold ) return TCL_OK;

  /* now build compressed grammar structures
     add top of the to level rule */
  if ( !(cfgRP = cfgAddRule (cfgP, "_TOP_", Public_Rule, 1)) )
    return TCL_ERROR;

  if ( verbose )
    fprintf (stderr, "cfgCompress: build new top level rule\n");

  /* traversing parse tree and adding nodes to _TOP_ rule */
  cfgTP      = cfgTraverseCreate ();
  cfgTP->tXN = 3000;
  cfgTP->nXA = (CFGNode**)malloc (cfgTP->tXN * sizeof (CFGNode*));
  cfgTP->mXA = (CFGNode**)malloc (cfgTP->tXN * sizeof (CFGNode*));

  cfgCompressBuildDFS (cfgP, cfgTP, cfgPTNP->childP, cfgRP->root, cfgRP, verbose);

  cfgTraverseFree (cfgTP);

  /* setting type for public rules to private */
  for (i=0; i<cfgP->bosNP->arcN; i++) {
    cfgAP = cfgP->bosNP->arcA[i];

    cfgAP->cfgRP->type = Private_Rule;
  }

  cfgNodeDealloc (cfgP->bosNP);

  /* clear and rebuild grammar */
  cfgClear (cfgP, 1);
  cfgBuild (cfgP, DEFAULT_SCORES, 0, verbose);

  return TCL_OK;
}

static int cfgCompressItf (ClientData cd, int argc, char *argv[]) {

  CFG  *cfgP      = (CFG*)cd;
  int   level     = 1;
  int   unfold    = 1;
  char *matchFile = NULL;
  int   verbose   = 0;

  argc--;
  if ( itfParseArgv (argv[0], &argc, argv+1, 0,
		     "-level",     ARGV_INT,    NULL, &level,     NULL,
		     "compress level",
		     "-unfold",    ARGV_INT,    NULL, &unfold,    NULL,
		     "unfold grammar in new top level rule",
		     "-matchFile", ARGV_STRING, NULL, &matchFile, NULL,
		     "file with matching terminals",
		     "-verbose",   ARGV_INT,    NULL, &verbose,   NULL,
		     "verbosity",
                     NULL) != TCL_OK )
    return TCL_ERROR;

  return cfgCompress (cfgP, level, unfold, matchFile, verbose);
}

/* ----------------------------------------------------------------------
   cfgReduce
   ---------------------------------------------------------------------- */
int cfgReduceDo (CFGTraverse *cfgTP, CFGNode *cfgNP, CFGArc *cfgAP, CFGRSItem *cfgRSIP, CFGScore offset) {

  CFGArcX i = 0;
  CFGArcX j = 0;

  /* deactivation of arcs */
  switch ( cfgAP->type & 0x3 ) {
  case T_Arc:
    /* if not in match list, unset arc in node */
    if ( cfgTP->lvXA[cfgAP->lvX] == 0 ) {
      while ( i < cfgNP->arcN && cfgNP->arcA[i] != cfgAP ) i++;
      assert (i<cfgNP->arcN);
      cfgNP->arcA[i] = NULL;

      if ( cfgTP->verbose > 1 )
	fprintf (stderr, "cfgReduceDo: skipped T_Arc %s\n",
		 cfgLexiconGetName (cfgTP->cfgP->cfgLP, cfgAP->lvX, T_Arc));

      /* missused as counter for skipped arcs */
      cfgTP->mXN++;

      return TCL_OK;
    }
    break;
  case NT_Arc:
    /* if not active, unset arc in node */
    if ( cfgAP->cfgRP->status == Inactive ) {
      while ( i < cfgNP->arcN && cfgNP->arcA[i] != cfgAP ) i++;
      assert (i<cfgNP->arcN);
      cfgNP->arcA[i] = NULL;

      if ( cfgTP->verbose > 1 )
	fprintf (stderr, "cfgReduceDo: skipped NT_Arc %s\n",
		 cfgLexiconGetName (cfgTP->cfgP->cfgLP, cfgAP->lvX, NT_Arc));

      /* missused as counter for skipped arcs */
      cfgTP->mXN++;

      return TCL_OK;
    }
    break;
  }

  /* no startover */
  if ( cfgNP        == cfgTP->cfgP->eosNP &&
       cfgAP->cfgNP == cfgTP->cfgP->bosNP ) return TCL_OK;

  /* we are at a leaf node, so it's an allowed path */
  if ( cfgAP->cfgNP->type == Leaf_Node ) return TCL_OK;

  /* skip expansion of repeatable arcs */
  if ( !(cfgAP->type & 0x4) )
    cfgTraverse (cfgTP->cfgP, cfgAP->cfgNP, cfgRSIP, offset, cfgTP);

  /* check if this node is empty */
  for (i=0, j=0; i<cfgAP->cfgNP->arcN; i++) {
    if ( cfgAP->cfgNP->arcA[i] == NULL ) j++;
    else {
      /* count also repeatable arcs */
      if ( (cfgAP->cfgNP->arcA[i]->type & 0x4) ) j++;
    }
  }

  /* node is empty, so skip this arc also
     if it is not the root arc (=<s>) of the root rule */
  if ( j == cfgAP->cfgNP->arcN && cfgNP != cfgTP->cfgP->root->root ) {
    i = 0;
    while ( i < cfgNP->arcN && cfgNP->arcA[i] != cfgAP ) i++;
    assert (i<cfgNP->arcN);
    cfgNP->arcA[i] = NULL;

    if ( cfgTP->verbose > 1 )
      fprintf (stderr, "cfgReduceDo: skipped arc %s due to empty node\n",
	       cfgLexiconGetName (cfgTP->cfgP->cfgLP, cfgAP->lvX, cfgAP->type));
    
    /* missused as counter for skipped arcs */
    cfgTP->mXN++;
  }

  return TCL_OK;
}


int cfgReduceIter (CFG *cfgP, CFGTraverse *cfgTP, int verbose) {

  CFGRule  *cfgRP;
  CFGRuleX  k;
  CFGArcX   i, j;
  int       res = 0;

  for (k=0; k<bmemGetItemN(cfgP->mem.rules); k++) {
    cfgRP = cfgGetRule (cfgP, k);

    if ( cfgRP->status != Inactive ) {
      cfgTraverse (cfgP, cfgRP->root, NULL, CFG_NULL_SCORE, cfgTP);

      /* check if root node is empty */
      for (i=0, j=0; i<cfgRP->root->arcN; i++) {
	if ( cfgRP->root->arcA[i] == NULL ) j++;
      }

      if ( j == cfgRP->root->arcN ) {
	cfgRP->status = Inactive;
	res++;
      }
    }
  }

  return res;
}

int cfgReduce (CFG *cfgP, char *matchFile, int verbose) {

  CFGTraverse *cfgTP = NULL;
  int          changes;
  int          changeN = 0;

  /* init cfgTraverse */
  cfgTP              = cfgTraverseCreate ();
  cfgTP->cfgP        = cfgP;
  cfgTP->doTArc      = cfgReduceDo;
  cfgTP->doNTArc     = cfgReduceDo;
  cfgTP->doLambdaArc = cfgReduceDo;
  cfgTP->verbose     = verbose;

  /* read match file */
  if ( matchFile ) {
    FILE *fp = fileOpen (matchFile, "r");
    LVX   i  = cfgLexiconGetItemN (cfgP->cfgLP, T_Arc);
    int   n;
    char  line[CFG_MAXLINE];

    cfgTP->lvXA = calloc (i, sizeof (LVX));

    if ( verbose )
      fprintf (stderr, "cfgReduce: using matchFile '%s'", matchFile);

    if ( verbose > 1 )
      fprintf (stderr, " with content:\n");
    else
      fprintf (stderr, "\n");

    while ( !feof (fp) && fgets (line, CFG_MAXLINE, fp) ) {
      n = strlen(line);
      i = LVX_NIL;
      if ( n ) {
	line[n-1] = '\0';
	i         = cfgLexiconGetIndex (cfgP->cfgLP, line, T_Arc);
      }
      if ( i == LVX_NIL )
	WARN ("cfgReduce: '%s' is not element of lexicon.\n", line);
      else {
	cfgTP->lvXA[i] = 1;
	if ( verbose > 1 ) fprintf (stderr, "%s ", line);
      }
    }

    fileClose (matchFile, fp);
    if ( verbose > 1 ) fprintf (stderr, "\n");
  } else {
    ERROR ("cfgReduce: matchFile not given\n");
    return TCL_ERROR;
  }

  while ( (changes = cfgReduceIter (cfgP, cfgTP, verbose)) ) {
    if ( verbose )
      fprintf (stderr, "cfgReduce: deactivated %d rules, %d arcs\n",
	       changes, cfgTP->mXN);

    changeN += changes;
    changeN += cfgTP->mXN;

    cfgTP->mXN = 0;
  }

  cfgTraverseFree (cfgTP);

  return changeN;
}

static int cfgReduceItf (ClientData cd, int argc, char *argv[]) {

  CFG  *cfgP      = (CFG*)cd;
  char *matchFile = NULL;
  int   verbose   = 0;

  argc--;
  if ( itfParseArgv (argv[0], &argc, argv+1, 0,
		     "<matchFile>", ARGV_STRING, NULL, &matchFile, NULL,
		     "file with matching terminals",
		     "-verbose",    ARGV_INT,    NULL, &verbose,   NULL,
		     "verbosity",
                     NULL) != TCL_OK )
    return TCL_ERROR;

  cfgReduce (cfgP, matchFile, verbose);

  return TCL_OK;
}


/* ----------------------------------------------------------------------
   LingKS functions
   ---------------------------------------------------------------------- */

/* returns score array over all Ts for a given LCT, n=(order of LM) ignored */
int cfgLingKSScoreArray (LingKS* lksP, LCT lct, lmScore *arr, int n) {

  CFG       *cfgP      = lksP->data.cfgP;
  CFGPTNode *cfgPTNP   = lct2node(lct);
  CFGPTNode *childPTNP = NULL;
  LVX        lvX       = lct2lvx(lct);
  CFGScore  *scoreA    = (CFGScore*)arr;

  /* check if the grammar is in an unstable state */
  if ( !cfgP->built ) cfgBuild (cfgP, DEFAULT_SCORES, 0, 0);

  /* extend given PTNode, by the old lvX */
  childPTNP = cfgPTNodeExtend (cfgPTNP, lvX);

  /* fill the score array */
  cfgPTNodeScoreArray (childPTNP, cfgP->cfgPTP, scoreA);

#ifdef CFG_DEBUG
  {
  int i, N;
  N = cfgLexiconGetItemN (cfgP->cfgLP, T_Arc);
  fprintf (stderr, "scoreArray (%d): ", N);
  for (i=0; i<N; i++) {
    if ( scoreA[i] != CFG_MAX_SCORE ) {
      fprintf (stderr, "%s(%d) ",
	       cfgLexiconGetName (cfgP->cfgLP, i, T_Arc),
	       scoreA[i]);
    }
  }
  fprintf (stderr, "\n");
  }
#endif

  return TCL_OK;
}

/* returns score for a gicen LCT to a LVX w */
lmScore cfgLingKSScore (LingKS* lksP, LCT lct, LVX w, int n) {

  CFG       *cfgP      = lksP->data.cfgP;
  CFGPTNode *cfgPTNP   = lct2node(lct);
  CFGPTNode *childPTNP = NULL;
  LVX        lvX       = lct2lvx(lct);

  /* check if the grammar is in an unstable state */
  if ( !cfgP->built ) cfgBuild (cfgP, DEFAULT_SCORES, 0, 0);

  /* extend given PTNode, by the old lvX */
  childPTNP = cfgPTNodeExtend (cfgPTNP, lvX);

  /* return the score */
  return (lmScore)cfgPTNodeScore (childPTNP, cfgP->cfgPTP, w);
}

/* creates the initial LCT (bos) */
LCT cfgLingKSCreateLCT (LingKS* lksP, LVX w) {

  CFG *cfgP = lksP->data.cfgP;
  LCT  res;

  /* check if the grammar is in an unstable state */
  if ( !cfgP->built ) cfgBuild (cfgP, DEFAULT_SCORES, 0, 0);

  res = nl2lct(cfgP->cfgPTP->root, w);

  return res;
}

/* reduce an LCT by n words */
LCT cfgLingKSReduceLCT (LingKS* lksP, LCT lct, int n) {

  WARN ("not yet implemented\n");
  return lct;
}

/* extend an LCT by a word */
LCT cfgLingKSExtendLCT (LingKS* lksP, LCT lct, LVX w) {

  CFGPTNode *cfgPTNP   = lct2node(lct);
  CFGPTNode *childPTNP = NULL;
  LVX        lvX       = lct2lvx(lct);
  LCT        res;

  /* just in case if there was no score or scoreArray call before */
  childPTNP = cfgPTNodeExtend (cfgPTNP, lvX);

  res = nl2lct(childPTNP, w);

  return res;
}

/* returns the last n words represented by LCT
   if n==-1 the default history will be used (0) */
int cfgLingKSDecodeLCT (LingKS* lksP, LCT lct, LVX* w, int n) {

  WARN ("not yet implemented\n");
  return n;
}

/* free this lct and all its dependent lcts */
int cfgLingKSFreeLCT (LingKS* lksP, LCT lct) {

  WARN ("not yet implemented\n");
  return TCL_OK;
}

/* return the number of LVXs */
int cfgLingKSitemN (LingKS* lksP) {

  CFG *cfgP = lksP->data.cfgP;

  return cfgLexiconGetItemN (cfgP->cfgLP, T_Arc);
}

/* returns the LVX of a given name */
LVX cfgLingKSitemX (LingKS* lksP, char *name) {

  CFG *cfgP = lksP->data.cfgP;

  return cfgLexiconGetIndex (cfgP->cfgLP, name, T_Arc);
}

/* returns the name of a given LVX */
char* cfgLingKSitemA (LingKS* lksP, int i) {

  CFG *cfgP = lksP->data.cfgP;

  return cfgLexiconGetName (cfgP->cfgLP, (LVX)i, T_Arc);
}

int cfgLingKSIsDirty (LingKS* lksP) {

  int dirty = lksP->dirty;

  /* reset dirty tag */
  lksP->dirty = 0;

  return dirty;
}

int cfgLingKSSaveDump (LingKS* lksP, FILE *fp) {

  return cfgSaveDump (lksP->data.cfgP, fp, 0, 0);
}

int cfgLingKSLoadDump (LingKS* lksP, FILE *fp, char *txt) {

  return cfgLoadDump (lksP->data.cfgP, fp, 0);
}

/* initializes LingKS */
int cfgLingKSInit (LingKS *lksP, CFG *cfgP, char *name) {

  cfgInit  (cfgP, (ClientData)name);
  cfgAlloc (cfgP, NULL, NULL);

  cfgP->lksP = lksP;

  lksP->lingKSSaveItfFct = cfgSaveItf;
  lksP->lingKSLoadItfFct = cfgLoadItf;
  lksP->lingKSSaveFct    = cfgLingKSSaveDump;
  lksP->lingKSLoadFct    = cfgLingKSLoadDump;
  lksP->lingKSPutsFct    = cfgPutsItf;
  lksP->scoreArrayFct    = cfgLingKSScoreArray;
  lksP->scoreFct         = cfgLingKSScore;
  lksP->createLCT        = cfgLingKSCreateLCT;
  lksP->reduceLCT        = cfgLingKSReduceLCT;
  lksP->extendLCT        = cfgLingKSExtendLCT;
  lksP->decodeLCT        = cfgLingKSDecodeLCT;
  lksP->freeLCT          = cfgLingKSFreeLCT;
  lksP->itemN            = cfgLingKSitemN;
  lksP->itemX            = cfgLingKSitemX;
  lksP->itemA            = cfgLingKSitemA;
  lksP->isDirty          = cfgLingKSIsDirty;

  return TCL_OK;
}


/* ======================================================================
   Context Free Grammar Traversing Object
   ====================================================================== */

int cfgTraverseInit (CFGTraverse *cfgTP) {

  cfgTP->cfgPTNP     = NULL;
  cfgTP->childPTNP   = NULL;
  cfgTP->lvX         = LVX_NIL;
  cfgTP->score       = CFG_MAX_SCORE;
  cfgTP->scoreA      = NULL;

  cfgTP->doLeafNode  = NULL;
  cfgTP->doTArc      = NULL;
  cfgTP->doNTArc     = NULL;
  cfgTP->doLambdaArc = NULL;

  cfgTP->cfgP        = NULL;
  cfgTP->cfgRP       = NULL;
  cfgTP->cfgNP       = NULL;
  cfgTP->cfgRSIP     = NULL;
  cfgTP->nXA         = NULL;
  cfgTP->mXA         = NULL;
  cfgTP->nXN         = 0;
  cfgTP->mXN         = 0;
  cfgTP->tXN         = 0;
  cfgTP->lvXA        = NULL;
  cfgTP->cacheA      = NULL;
  cfgTP->cacheN      = 0;

  cfgTP->startover   = cfgDefault.startover;
  cfgTP->verbose     = 0;

  return TCL_OK;
}

CFGTraverse* cfgTraverseCreate () {

  CFGTraverse *cfgTP = (CFGTraverse*)malloc (sizeof(CFGTraverse));

  if ( !cfgTP || cfgTraverseInit (cfgTP) != TCL_OK ) {
    if ( cfgTP ) Nulle (cfgTP);
    ERROR ("Cannot create CFGTraverse object.\n");
    return NULL;
  }

  return cfgTP;
}

int cfgTraverseDeinit (CFGTraverse *cfgTP) {

  return cfgTraverseInit (cfgTP);
}

int cfgTraverseFree (CFGTraverse *cfgTP) {

  if ( cfgTP->nXA    ) { Nulle (cfgTP->nXA);    cfgTP->nXN    = 0; }
  if ( cfgTP->mXA    ) { Nulle (cfgTP->mXA);    cfgTP->mXN    = 0; }
  if ( cfgTP->cacheA ) { Nulle (cfgTP->cacheA); cfgTP->cacheN = 0; }
  if ( cfgTP->lvXA   ) { Nulle (cfgTP->lvXA); }

  if ( cfgTraverseDeinit (cfgTP) != TCL_OK ) return TCL_ERROR;
  if ( cfgTP ) Nulle (cfgTP);

  return TCL_OK;
}

/* ----------------------------------------------------------------------
   cfgTraverse
   ---------------------------------------------------------------------- */
int cfgTraverse (CFG *cfgP, CFGNode *cfgNP, CFGRSItem *cfgRSIP, CFGScore offset, CFGTraverse *cfgTP) {

  CFGArc       *cfgAP = NULL;
  CFGRSItem    *rsip  = NULL;
  CFGScore      o     = offset;
  CFGArcX       i;

  /* do we have a leaf node */
  if ( cfgNP->type == Leaf_Node ) {
    /* pop of last rule from stack */
    if ( cfgRSIP ) rsip = cfgRuleStackPop (cfgRSIP, &cfgAP);

    if ( cfgTP->doLeafNode )
      cfgTP->doLeafNode (cfgTP, cfgNP, cfgAP, rsip, offset);

    return TCL_OK;
  }

  /* we have a root or normal node with further arcs */
  for (i=0; i<cfgNP->arcN; i++) {
    cfgAP  = cfgNP->arcA[i];
    rsip   = cfgRSIP;
    offset = o;

    if ( !cfgAP ) continue;

    OVERFLOW_ADD(offset, cfgAP->score, CFG_MAX_SCORE);

    switch (cfgAP->type & 0x3) {
    case T_Arc:
      if ( cfgTP->doTArc )
	cfgTP->doTArc (cfgTP, cfgNP, cfgAP, rsip, offset);
      break;
    case NT_Arc:
      /* push rule to stack */
      if ( cfgRSIP ) rsip = cfgRuleStackPush (cfgP->cfgRSP, cfgRSIP, cfgAP);

      if ( cfgTP->doNTArc )
	cfgTP->doNTArc (cfgTP, cfgNP, cfgAP, rsip, offset);
      break;
    case Lambda_Arc:
      if ( cfgTP->doLambdaArc )
	cfgTP->doLambdaArc (cfgTP, cfgNP, cfgAP, rsip, offset);
      break;
    }
  }

  return TCL_OK;
}


int cfgTraverseRand (CFG *cfgP, CFGNode *cfgNP, CFGRSItem *cfgRSIP, CFGScore offset, CFGTraverse *cfgTP) {

  CFGArc       *cfgAP = NULL;
  CFGRSItem    *rsip  = NULL;
  CFGScore      o     = offset;
  CFGArcX       i;

  /* do we have a leaf node */
  if ( cfgNP->type == Leaf_Node ) {
    /* pop of last rule from stack */
    if ( cfgRSIP ) rsip = cfgRuleStackPop (cfgRSIP, &cfgAP);

    if ( cfgTP->doLeafNode )
      cfgTP->doLeafNode (cfgTP, cfgNP, cfgAP, rsip, offset);

    return TCL_OK;
  }

  /* we have a root or normal node with further arcs
     do not iterate through all arcs, instead do a random selection */
  if ( !cfgNP->arcN ) return TCL_OK;

  if ( cfgTP->cacheN < cfgNP->arcN ) {
    cfgTP->cacheN = cfgNP->arcN * 2;
    cfgTP->cacheA = (void**)realloc (cfgTP->cacheA, cfgTP->cacheN * sizeof(void*));
  }

  /* choose arc according to their distribution */
  if ( cfgP->buildMode == EQUAL_SCORES ) {
    float    r, R;
    CFGScore s;

    /* to guarantee, that this case produces the same result as the else-case
       for equally distributed transitions, we'll have to convert it from
       probs to scores and back (precision loss due to compression) */
    r = (float)(rand() % cfgNP->arcN) / cfgNP->arcN;
    s = CompressScore (P2S(r));
    r = S2P(UnCompressScore(s));
    R = 0.0;
    i = 0;
    while ( i < cfgNP->arcN && R < r ) {
      /* this is a little trick for handling lambda transitions:
	 but traversing along one of these now hardly depends on the
	 position of this transition in the array (not the best trick).
	 we normally do not have many of these */
      if ( (cfgNP->arcA[i]->type & 0x3) == Lambda_Arc &&
	   cfgNP->arcA[i]->score        == CFG_NULL_SCORE )
	R += 1.0 / cfgNP->arcN;
      else
	R += S2P(UnCompressScore(cfgNP->arcA[i]->score));
      i++;
    }
    if ( i > 0 ) i--;
  } else if ( cfgP->buildMode == FIXED_SCORES ) {
    float    r, R;
    CFGScore s;

    r = (float)rand()/(float)RAND_MAX;
    s = CompressScore (P2S(r));
    r = S2P(UnCompressScore(s));
    R = 0.0;
    i = 0;
    while ( i < cfgNP->arcN && R < r ) {
      R += S2P(UnCompressScore(cfgNP->arcA[i]->score));
      i++;
    }
    if ( i > 0 ) i--;
  } else {
    i = rand() % cfgNP->arcN;
  }

  /* traverse arc */
  cfgAP  = cfgNP->arcA[i];
  rsip   = cfgRSIP;
  offset = o;

  OVERFLOW_ADD(offset, cfgAP->score, CFG_MAX_SCORE);

  switch (cfgAP->type & 0x3) {
  case T_Arc:
    if ( cfgTP->doTArc )
      cfgTP->doTArc (cfgTP, cfgNP, cfgAP, rsip, offset);
    break;
  case NT_Arc:
    /* push rule to stack */
    if ( cfgRSIP ) rsip = cfgRuleStackPush (cfgP->cfgRSP, cfgRSIP, cfgAP);
    
    if ( cfgTP->doNTArc )
      cfgTP->doNTArc (cfgTP, cfgNP, cfgAP, rsip, offset);
    break;
  case Lambda_Arc:
    if ( cfgTP->doLambdaArc )
      cfgTP->doLambdaArc (cfgTP, cfgNP, cfgAP, rsip, offset);
    break;
  }

  return TCL_OK;
}


/* ======================================================================
   Context Free Grammar Set
   ====================================================================== */

/* ----------------------------------------------------------------------
   Init/Create Context Free Grammar Set Object
   ---------------------------------------------------------------------- */
int cfgSetInit (CFGSet *cfgSP, ClientData cd) {

  cfgSP->name        = strdup ((char*)cd);
  cfgSP->useN        = cfgSetDefault.useN;

  cfgSP->cfgLP       = cfgSetDefault.cfgLP;
  cfgSP->cfgRSP      = cfgSetDefault.cfgRSP;
  cfgSP->cfgPTP      = cfgSetDefault.cfgPTP;
  cfgSP->lksP        = cfgSetDefault.lksP;

  cfgSP->built       = cfgSetDefault.built;

  /* list structure for CFGs */
  listInit ((List*)&(cfgSP->list), &cfgInfo, sizeof(CFG),
	    cfgSetDefault.list.blkSize);

  cfgSP->list.init   = (ListItemInit  *)cfgInit;
  cfgSP->list.deinit = (ListItemDeinit*)cfgDeinit;

  return TCL_OK;
}

int cfgSetAlloc (CFGSet *cfgSP) {

  /* create lexicon and parse tree*/
  cfgSP->cfgLP       = cfgLexiconCreate   ();
  cfgSP->cfgRSP      = cfgRuleStackCreate ();
  cfgSP->cfgPTP      = cfgParseTreeCreate (cfgSP->cfgLP);

  return TCL_OK;
}

CFGSet* cfgSetCreate (char *name) {

  CFGSet *cfgSP = (CFGSet*)malloc (sizeof(CFGSet));

  if ( !cfgSP || cfgSetInit (cfgSP, (ClientData)name) != TCL_OK ) {
    if ( cfgSP ) Nulle (cfgSP);
    ERROR ("Cannot create CFGSet object\n");
    return NULL;
  }

  cfgSetAlloc (cfgSP);

  return cfgSP;
}

static ClientData cfgSetCreateItf (ClientData cd, int argc, char *argv[]) {

  argc--;
  if ( itfParseArgv (argv[0], &argc, argv+1, 0,
		     NULL) != TCL_OK )
    return NULL;

  return (ClientData)cfgSetCreate (argv[0]);
}

/* ----------------------------------------------------------------------
   Deinit/Free Context Free Grammar Set Object
   ---------------------------------------------------------------------- */
int cfgSetLinkN (CFGSet *cfgSP) {
  
  return cfgSP->useN;
}

int cfgSetDeinit (CFGSet *cfgSP) {

  if ( cfgSetLinkN (cfgSP) ) {
    SERROR ("CFGSet '%s' still in use by other objects.\n", cfgSP->name);
    return TCL_ERROR;
  }

  if ( cfgSP->name ) Nulle (cfgSP->name);

  /*---------------*/
  /* START FF HACK */
  /*---------------*/
  /* !!! remove hash entries from itf */
  /* !!! DELINK !!!*/
  listDelink ((List*)&(cfgSP->list));
  /* REMOVE OBJECT HASHES SO THAT THE ELEMENTS REMOVED BY LISTDEINIT ARE NO LONGER ACCESSIBLE */
  {
    int   i;
    List* L = (List*) &cfgSP->list;
    for ( i = 0; i < L->itemN; i++)
	    itfRemoveObjectHash((ClientData)&(L->itemA[L->itemSizeCP * i]));
  }
  listDeinit ((List*)&(cfgSP->list));
  /*-------------*/
  /* END FF HACK */
  /*-------------*/


  /* free lexicon and parse tree */
  if ( cfgSP->cfgLP  ) { cfgLexiconFree   (cfgSP->cfgLP);  cfgSP->cfgLP  = NULL; }
  if ( cfgSP->cfgRSP ) { cfgRuleStackFree (cfgSP->cfgRSP); cfgSP->cfgRSP = NULL; }
  if ( cfgSP->cfgPTP ) { cfgParseTreeFree (cfgSP->cfgPTP); cfgSP->cfgPTP = NULL; }

  return TCL_OK;
}

int cfgSetFree (CFGSet *cfgSP) {
  
  if ( cfgSetDeinit (cfgSP) != TCL_OK ) return TCL_ERROR;
  if ( cfgSP ) Nulle (cfgSP);
  
  return TCL_OK;
}

static int cfgSetFreeItf (ClientData cd) {
  
  return cfgSetFree ((CFGSet*)cd);
}


/* ----------------------------------------------------------------------
   Configure/Access Context Free Grammar Set Object
   ---------------------------------------------------------------------- */
int cfgSetConfigureItf (ClientData cd, char *var, char *val) {
  
  CFGSet *cfgSP = (CFGSet*)cd;
  
  if ( !cfgSP ) cfgSP = &cfgSetDefault;
  
  if ( var == NULL ) {
    ITFCFG (cfgSetConfigureItf, cd, "name");
    ITFCFG (cfgSetConfigureItf, cd, "cfgN");
    ITFCFG (cfgSetConfigureItf, cd, "built");
    return TCL_OK;
  }

  ITFCFG_CharPtr (var, val, "name",  cfgSP->name,       1);
  ITFCFG_Int     (var, val, "cfgN",  cfgSP->list.itemN, 1);
  ITFCFG_Int     (var, val, "built", cfgSP->built,      1);
  
  ERROR("unknown option '-%s %s'.\n", var, val ? val : "?");
  
  return TCL_ERROR;
}

ClientData cfgSetAccessItf (ClientData cd, char *name, TypeInfo **ti) {
  
  CFGSet *cfgSP = (CFGSet*)cd;
  int     i;
  
  if ( *name == '.' ) {
    if ( name[1] == '\0' ) {
      itfAppendElement ("lex");
      itfAppendElement ("rs");
      itfAppendElement ("pt");
      itfAppendElement ("cfg(0..%d)", cfgSP->list.itemN-1);
    } else {
      if ( !strcmp (name+1, "lex") ) {
        *ti = itfGetType ("CFGLexicon");
        return (ClientData)cfgSP->cfgLP;
      }
      if ( !strcmp (name+1, "rs") ) {
        *ti = itfGetType ("CFGRuleStack");
        return (ClientData)cfgSP->cfgRSP;
      }
      if ( !strcmp (name+1, "pt") ) {
	*ti = itfGetType ("CFGParseTree");
	return (ClientData)cfgSP->cfgPTP;
      }
      if ( sscanf (name+1, "cfg(%d)", &i) == 1 ) {
        *ti = itfGetType ("CFG");
        if ( i >= 0 && i < cfgSP->list.itemN )
          return (ClientData)&(cfgSP->list.itemA[i]);
        else return NULL;
      }
    }
  }  

  return listAccessItf ((ClientData)&(cfgSP->list), name, ti);
}


/* ----------------------------------------------------------------------
   cfgSetPutsItf
   ---------------------------------------------------------------------- */
int cfgSetPuts (CFGSet *cfgSP, CFGPutsFormat f) {

  switch (f) {
  case Short_Puts:
    return cfgLexiconPuts (cfgSP->cfgLP, T_Arc, f);
  }

  ERROR ("Not supported or unknown PutsFormat %d=%s\n", f, cfgPutsFormat2str(f));

  return TCL_ERROR;
}

static int cfgSetPutsItf (ClientData cd, int argc, char *argv[]) {

  CFGSet *cfgSP = (CFGSet*)cd;
  char *format = "short";

  argc--;
  if ( itfParseArgv (argv[0], &argc, argv+1, 0,
 		     "-format", ARGV_STRING, NULL, &format, NULL,
		     "output format (short, long)",
		     NULL) != TCL_OK )
    return TCL_ERROR;

  return cfgSetPuts (cfgSP, cfgPutsFormat2int (format));
}

/* ----------------------------------------------------------------------
   cfgSetNewCFG
   ---------------------------------------------------------------------- */
CFG* cfgSetNewCFG (CFGSet *cfgSP, char *name) {

  int cfgX = listIndex ((List*)&(cfgSP->list), (ClientData)name, 0);

  if ( cfgX < 0 ) {
    if ( cfgSP->list.itemN == CFGX_MAX ) {
      ERROR ("Can't handle more than %d grammars. Redefine CFGX_MAX.\n",
             CFGX_MAX);
      return NULL;
    }
    cfgX = listNewItem ((List*)&(cfgSP->list), (ClientData)name);
  }

  cfgSetClear (cfgSP, 0);

  return &(cfgSP->list.itemA[cfgX]);
}

/* ----------------------------------------------------------------------
   cfgSetGetSharedCFG
   ---------------------------------------------------------------------- */
CFG* cfgSetGetSharedCFG (CFGSet *cfgSP) {

  CFG  *cfgP;
  CFGX  i;

  if ( !cfgSP ) return NULL;

  for (i=0; i<cfgSP->list.itemN; i++) {
    cfgP = &(cfgSP->list.itemA[i]);

    if ( cfgP->status == Shared ) return cfgP;
  }

  return NULL;
}

/* ----------------------------------------------------------------------
   cfgSetGetSharedCFG
   ---------------------------------------------------------------------- */
CFG* cfgSetGetCFGByName (CFGSet *cfgSP, char *name) {

  CFGX i = 0;

  while ( i < cfgSP->list.itemN && strcmp (cfgSP->list.itemA[i].tag, name) )
    i++;

  if ( i < cfgSP->list.itemN ) return &(cfgSP->list.itemA[i]);

  return NULL;
}

/* ----------------------------------------------------------------------
   cfgSetGetCFGX
   ---------------------------------------------------------------------- */
CFGX cfgSetGetCFGX (CFGSet *cfgSP, CFG *cfgP) {

  CFGX cfgX;

  if ( !cfgP ) return CFGX_NIL;

  cfgX = cfgP - &(cfgSP->list.itemA[0]);

  if ( cfgX >= cfgSP->list.itemN ) return CFGX_NIL;

  return cfgX;
}

/* ----------------------------------------------------------------------
   cfgSetGetCFG
   ---------------------------------------------------------------------- */
CFG* cfgSetGetCFG (CFGSet *cfgSP, CFGX cfgX) {

  if ( cfgX >= cfgSP->list.itemN ) return NULL;

  return &(cfgSP->list.itemA[cfgX]);
}

/* ----------------------------------------------------------------------
   cfgSetLoad
   ---------------------------------------------------------------------- */
int cfgSetLoadDump (CFGSet *cfgSP, FILE *fp) {

  CFG    *cfgP;
  CFGArc *cfgAP;
  int     itemN, i, pt;
  UINT    j, cfgX, ruleX;
  char    s[CFG_MAXLINE];

  if ( !fp ) {
    ERROR ("cfgSetLoadDump: invalid file pointer.\n");
    return TCL_ERROR;
  }

  read_string (fp, s);
  if ( strcmp (s, "CFGSET-BEGIN 1.0") ) {
    ERROR ("cfgSetLoadDump: inconsistent file.\n");
    return TCL_ERROR;
  }

  pt = read_int (fp);

  /* read grammars */
  itemN = read_int (fp);
  for (i=0; i<itemN; i++) {
    read_string (fp, s);
    
    cfgP = cfgCreate (s, NULL, cfgSP);
    cfgLoadDump (cfgP, fp, 1);
  }

  /* read cross CFG rule linking */
  for (i=0; i<itemN; i++) {
    cfgP = &(cfgSP->list.itemA[i]);
    for (j=0; j<bmemGetItemN (cfgP->mem.arcs); j++) {
      cfgAP = cfgGetArc (cfgP, j);
      if ( (cfgAP->type & 0x3) == NT_Arc ) {
	cfgX  = read_int (fp);
	ruleX = read_int (fp);
	cfgAP->cfgRP = cfgGetRule (&(cfgSP->list.itemA[cfgX]), (CFGRuleX)ruleX);
      }
    }
  }

  /* read lexicon */
  cfgLexiconLoadDump (cfgSP->cfgLP, fp);

  if ( pt ) {
    cfgRuleStackLoadDump (cfgSP->cfgRSP, fp, NULL, cfgSP);
    cfgParseTreeLoadDump (cfgSP->cfgPTP, fp, NULL, cfgSP);
  }

  read_string (fp, s);
  if ( strcmp (s, "CFGSET-END") ) {
    ERROR ("cfgSetLoadDump: inconsistent file.\n");
    return TCL_ERROR;
  }

  return TCL_OK;
}

int cfgSetLoad (CFGSet *cfgSP, char *fileName) {

  FILE *fp  = NULL;
  int   res = TCL_ERROR;

  if ( !(fp = fileOpen (fileName, "r")) ) {
    ERROR ("cfgSetLoad: cannot open file '%s' for writing\n", fileName);
    return TCL_ERROR;
  }

  res = cfgSetLoadDump (cfgSP, fp);

  fileClose (fileName, fp);

  if ( res != TCL_OK )
    ERROR ("cfgSetLoad: in file %s.\n", fileName);

  return res;
}

static int cfgSetLoadItf (ClientData cd, int argc, char *argv[]) {

  CFGSet *cfgSP      = (CFGSet*)cd;
  char   *fileName   = NULL;

  argc--;
  if ( itfParseArgv (argv[0], &argc, argv+1, 0,
                     "<fileName>", ARGV_STRING, NULL, &fileName,   NULL,
                     "file name",
                     NULL) != TCL_OK )
    return TCL_ERROR;

  return cfgSetLoad (cfgSP, fileName);
}

/* ----------------------------------------------------------------------
   cfgSetSave
   ---------------------------------------------------------------------- */
int cfgSetSaveDump (CFGSet *cfgSP, FILE *fp, int pt) {

  CFG    *cfgP;
  CFGArc *cfgAP;
  int     i;
  UINT    j;
  

  if ( !fp ) {
    ERROR ("cfgSetSaveDump: Invalid file pointer\n");
    return TCL_ERROR;
  }

  write_string (fp, "CFGSET-BEGIN 1.0");

  /* write grammars */
  write_int (fp, pt  ? 1 : 0);
  write_int (fp, cfgSP->list.itemN);
  for (i=0; i<cfgSP->list.itemN; i++) {
    cfgP = &(cfgSP->list.itemA[i]);
    write_string (fp, cfgP->name);
    cfgSaveDump (cfgP, fp, 1, pt);
  }

  /* write cross CFG rule linking */
  for (i=0; i<cfgSP->list.itemN; i++) {
    cfgP = &(cfgSP->list.itemA[i]);
    for (j=0; j<bmemGetItemN (cfgP->mem.arcs); j++) {
      cfgAP = cfgGetArc (cfgP, j);
      if ( (cfgAP->type & 0x3) == NT_Arc ) {
	if ( cfgAP->cfgRP->cfgP == cfgP )
	  write_int (fp, i);
	else
	  write_int (fp, (int)cfgSetGetCFGX (cfgSP, cfgAP->cfgRP->cfgP));
	write_int (fp, (int)cfgGetRuleX   (cfgAP->cfgRP->cfgP, cfgAP->cfgRP));
      }
    }
  }

  /* write lexicon */
  cfgLexiconSaveDump (cfgSP->cfgLP, fp);

  if ( pt ) {
    cfgRuleStackSaveDump (cfgSP->cfgRSP, fp, cfgSP);
    cfgParseTreeSaveDump (cfgSP->cfgPTP, fp, cfgSP);
  }

  write_string (fp, "CFGSET-END");

  return TCL_OK;
}

int cfgSetSave (CFGSet *cfgSP, char *fileName, int pt) {

  FILE *fp  = NULL;
  int   res = TCL_ERROR;

  if ( !(fp = fileOpen (fileName, "w")) ) {
    ERROR ("cfgSetSave: cannot open file '%s' for writing\n", fileName);
    return TCL_ERROR;
  }

  res = cfgSetSaveDump (cfgSP, fp, pt);

  fileClose (fileName, fp);

  if ( res != TCL_OK )
    ERROR ("cfgSetSave: in file %s.\n", fileName);

  return res;
}

static int cfgSetSaveItf (ClientData cd, int argc, char *argv[]) {

  CFGSet *cfgSP      = (CFGSet*)cd;
  char   *fileName   = NULL;
  int     pt         = 0;

  argc--;
  if ( itfParseArgv (argv[0], &argc, argv+1, 0,
                     "<fileName>", ARGV_STRING, NULL, &fileName,   NULL,
                     "file name",
		     "-pt",        ARGV_INT,    NULL, &pt,         NULL,
		     "dump also parse tree and rule stack",
                     NULL) != TCL_OK )
    return TCL_ERROR;

  return cfgSetSave (cfgSP, fileName, pt);
}


/* ----------------------------------------------------------------------
   cfgSetBuild
   ---------------------------------------------------------------------- */
int cfgSetBuild (CFGSet *cfgSP, CFGBuildMode m, int reset, int verbose) {

  int i;
  /* START FF HACK --- added var to forward errors */
  int res;
  /* END FF HACK --- added var to forward errors */

  if ( cfgSP->built ) return TCL_OK;

  cfgRuleStackBuild (cfgSP->cfgRSP);
  cfgParseTreeBuild (cfgSP->cfgPTP);

  for (i=0; i<cfgSP->list.itemN; i++)
  /* START FF HACK --- forward errors */
  /* originally: cfgBuild (&(cfgSP->list.itemA[i]), m, reset, verbose); */
  {
    res = cfgBuild (&(cfgSP->list.itemA[i]), m, reset, verbose);
    if (res!=TCL_OK)
      return res;
  }
  /* END FF HACK --- forward errors */

  cfgSP->built = 1;

  return TCL_OK;
}

static int cfgSetBuildItf (ClientData cd, int argc, char *argv[]) {

  CFGSet *cfgSP      = (CFGSet*)cd;
  int     verbose    = 1;
  char   *mode       = "default";
  int     reset      = 0;
  int     m          = 0;

  argc--;
  if ( itfParseArgv (argv[0], &argc, argv+1, 0,
		     "-mode",      ARGV_STRING, NULL, &mode,    NULL,
		     "score definition mode (null|fixed|equal|default=use stored build mode)",
		     "-overwrite", ARGV_INT,    NULL, &reset,   NULL,
		     "overwrite predefined scores from grammar file",
		     "-verbose",   ARGV_INT,    NULL, &verbose,       NULL,
		     "verbosity",
		     NULL) != TCL_OK )
    return TCL_ERROR;

  m = cfgBuildMode2int (mode);
  if ( !m ) {
    ERROR("unknown build mode '%s'\n", mode);
    return TCL_ERROR;
  }

  return cfgSetBuild (cfgSP, (CFGBuildMode)m, reset, verbose);
}

/* ----------------------------------------------------------------------
   cfgSetClear
   ---------------------------------------------------------------------- */
int cfgSetClear (CFGSet *cfgSP, int free) {

  int i;

  if ( cfgSP->built != 0 ) {
    for (i=0; i<cfgSP->list.itemN; i++)
      cfgClear (&(cfgSP->list.itemA[i]), free);

    cfgSP->built = 0;

    if ( cfgSP->lksP ) {
      cfgSP->lksP->dirty = 1;
    }
  }

  return TCL_OK;
}

static int cfgSetClearItf (ClientData cd, int argc, char *argv[]) {

  CFGSet *cfgSP = (CFGSet*)cd;
  int     free  = 0;

  argc--;
  if ( itfParseArgv (argv[0], &argc, argv+1, 0,
		     "-free",  ARGV_INT,    NULL, &free,    NULL,
		     "free items instead of clearing",
		     NULL) != TCL_OK )
    return TCL_ERROR;

  return cfgSetClear (cfgSP, free);
}

/* ----------------------------------------------------------------------
   cfgSetParse
   ---------------------------------------------------------------------- */
CFGPTNode* cfgSetParse (CFGSet *cfgSP, char *text, int verbose) {

  char  whiteSep[] = " \t\n";
  char  str[CFG_MAXLINE];
  char *sP         = &str[0];
  char *w          = NULL;

  int   i          = 0;
  int   N;

  LVX   lvXA[CFG_MAXLINE];
  LVX   lvX;
  LCT   lctA[CFG_MAXLINE];

  CFGPTNode  *cfgPTNP;

  /* check if the grammar is in an unstable state */
  if ( !cfgSP->built ) cfgSetBuild (cfgSP, DEFAULT_SCORES, 0, 0);
  lvXA[0] = LVX_NIL;

  /* --------------------------------------------------
     build input string -> build lvXA
     -------------------------------------------------- */
  sprintf (sP, "%s %s %s", cfgSP->cfgLP->beginOS, text, cfgSP->cfgLP->endOS);

  w = strtok (sP, whiteSep);
  while ( w ) {
    lvX     = cfgLexiconGetIndex (cfgSP->cfgLP, w, T_Arc);
    lvXA[i] = lvX;

    if ( lvXA[i] == LVX_NIL ) {
      ERROR ("cfgParse: can't find lvX for %s.\n", w);
      return NULL;
    }

    i++;
    w = strtok (NULL, whiteSep);
  }

  /* --------------------------------------------------
     start parsing
     -------------------------------------------------- */
  N = i; i = 0;

  /* create initial LCT */
  lctA[i] = nl2lct(cfgSP->cfgPTP->root, lvXA[i]);

  for (i=0; i<N-1; i++) {
    /* extend old LCT */
    cfgPTNP   = cfgPTNodeExtend (lct2node(lctA[i]), lct2lvx(lctA[i]));

    /* build new LCT */
    lctA[i+1] = nl2lct(cfgPTNP, lvXA[i+1]);
  }

  cfgPTNP = cfgPTNodeExtend (lct2node(lctA[i]), lct2lvx(lctA[i]));

  if ( verbose ) {
    printf ("LVX     :");
    for (i=0; i<N; i++) {
      lvX = lct2lvx(lctA[i]);
      printf ("%6d", lvX);
    }
    printf ("\n");

    printf ("PTNodeX :");
    printf ("  root");
    for (i=2; i<N; i++) {
      printf ("%6d", cfgParseTreeGetNodeX (cfgSP->cfgPTP, lct2node(lctA[i])));
    }
    printf ("%6d", cfgParseTreeGetNodeX (cfgSP->cfgPTP, cfgPTNP));
    printf ("\n"); fflush (stdout);
  }

  return cfgPTNP;
}

static int cfgSetParseItf (ClientData cd, int argc, char *argv[]) {

  CFGSet   *cfgSP    = (CFGSet*)cd;
  char       line[CFG_MAXLINE];
  char      *lP      = &line[0];
  int        verbose = 0;
  CFGPTNode *cfgPTNP = NULL;

  line[0] = '\0';

  argc--;
  if ( itfParseArgv (argv[0], &argc, argv+1, 0,
                     "<text>",   ARGV_STRING, NULL, &lP,        NULL,
                     "text to path",
		     "-verbose", ARGV_INT,    NULL, &verbose,   NULL,
		     "verbosity",
                     NULL) != TCL_OK )
    return TCL_ERROR;

  if ( (cfgPTNP = cfgSetParse (cfgSP, lP, verbose)) == NULL ) {
    return TCL_ERROR;
  }

  itfAppendResult ("%d", cfgParseTreeGetNodeX (cfgSP->cfgPTP, cfgPTNP));

  return TCL_OK;
}

/* ----------------------------------------------------------------------
   cfgSetCompress
   ---------------------------------------------------------------------- */
int cfgSetCompress (CFGSet *cfgSP, int level, int unfold, char *matchFile, int verbose) {

  CFG  *cfgP;
  CFGX  i;

  for (i=0; i<(CFGX)cfgSP->list.itemN; i++) {
    cfgP = &(cfgSP->list.itemA[i]);

    if ( verbose )
      fprintf (stderr, "cfgSetCompress: compressing %s\n", cfgP->name);

    cfgCompress (cfgP, level, unfold, matchFile, verbose);
  }

  return TCL_OK;
}


static int cfgSetCompressItf (ClientData cd, int argc, char *argv[]) {

  CFGSet *cfgSP     = (CFGSet*)cd;
  int     level     = 1;
  int     unfold    = 1;
  char   *matchFile = NULL;
  int     verbose   = 0;

  argc--;
  if ( itfParseArgv (argv[0], &argc, argv+1, 0,
		     "-level",     ARGV_INT,    NULL, &level,     NULL,
		     "compress level",
		     "-unfold",    ARGV_INT,    NULL, &unfold,    NULL,
		     "unfold grammar in new top level rule",
		     "-matchFile", ARGV_STRING, NULL, &matchFile, NULL,
		     "file with matching terminals",
		     "-verbose",   ARGV_INT,    NULL, &verbose,   NULL,
		     "verbosity",
                     NULL) != TCL_OK )
    return TCL_ERROR;

  return cfgSetCompress (cfgSP, level, unfold, matchFile, verbose);
}

/* ----------------------------------------------------------------------
   cfgSetReduce
   ---------------------------------------------------------------------- */
int cfgSetReduce (CFGSet *cfgSP, char *matchFile, int verbose) {

  CFG  *cfgP;
  CFGX  i;
  int   changeN;

  do {
    changeN = 0;

    for (i=0; i<(CFGX)cfgSP->list.itemN; i++) {
      cfgP = &(cfgSP->list.itemA[i]);

      if ( verbose )
	fprintf (stderr, "cfgSetReduce: reducing %s\n", cfgP->name);

      changeN += cfgReduce (cfgP, matchFile, verbose);
    }

    if ( verbose && changeN )
      fprintf (stderr, "cfgSetReduce: next pass due to changes in CFGSet\n");

  } while ( changeN > 0 );

  return TCL_OK;

}

static int cfgSetReduceItf (ClientData cd, int argc, char *argv[]) {

  CFGSet *cfgSP     = (CFGSet*)cd;
  char   *matchFile = NULL;
  int     verbose   = 0;

  argc--;
  if ( itfParseArgv (argv[0], &argc, argv+1, 0,
		     "<matchFile>", ARGV_STRING, NULL, &matchFile, NULL,
		     "file with matching terminals",
		     "-verbose",    ARGV_INT,    NULL, &verbose,   NULL,
		     "verbosity",
                     NULL) != TCL_OK )
    return TCL_ERROR;

  return cfgSetReduce (cfgSP, matchFile, verbose);
}

/* ----------------------------------------------------------------------
   LingKS functions
   ---------------------------------------------------------------------- */

/* returns score array over all Ts for a given LCT, n=(order of LM) ignored */
int cfgSetLingKSScoreArray (LingKS* lksP, LCT lct, lmScore *arr, int n) {

  CFGSet    *cfgSP     = lksP->data.cfgSP;
  CFGPTNode *cfgPTNP   = lct2node(lct);
  CFGPTNode *childPTNP = NULL;
  LVX        lvX       = lct2lvx(lct);
  CFGScore  *scoreA    = (CFGScore*)arr;

  /* check if the grammar set is in an unstable state */
  if ( !cfgSP->built ) cfgSetBuild (cfgSP, DEFAULT_SCORES, 0, 0);

#ifdef CFG_DEBUG
  fprintf (stderr, "scoreArray: extend LCT=%s, LVX=%s\n",
	   cfgLexiconGetName (cfgSP->cfgLP, cfgPTNP->lvX, T_Arc),
	   cfgLexiconGetName (cfgSP->cfgLP, lvX, T_Arc));
#endif

  /* extend given PTNode by the old lvX */
  childPTNP = cfgPTNodeExtend (cfgPTNP, lvX);

#ifdef CFG_DEBUG
  fprintf (stderr, "scoreArray: LCT=%s\n",
	   cfgLexiconGetName (cfgSP->cfgLP, lvX, T_Arc));
#endif

  /* fill the score array */
  cfgPTNodeScoreArray (childPTNP, cfgSP->cfgPTP, scoreA);

#ifdef CFG_DEBUG
  {
  int i, j, N;
  N = cfgLexiconGetItemN (cfgSP->cfgLP, T_Arc);
  j = 0;
  fprintf (stderr, "scoreArray (%d): ", N);
  for (i=0; i<N; i++) {
    if ( scoreA[i] != CFG_MAX_SCORE ) {
      fprintf (stderr, "%s(%d) ",
	       cfgLexiconGetName (cfgSP->cfgLP, i, T_Arc),
	       scoreA[i]);
      j++;
    }
  }
  fprintf (stderr, " ==> %d items\n", j);
  }
#endif

  return TCL_OK;
}

/* returns score for a gicen LCT to a LVX w */
lmScore cfgSetLingKSScore (LingKS* lksP, LCT lct, LVX w, int n) {

  CFGSet    *cfgSP     = lksP->data.cfgSP;
  CFGPTNode *cfgPTNP   = lct2node(lct);
  CFGPTNode *childPTNP = NULL;
  LVX        lvX       = lct2lvx(lct);
  CFGScore   score;

  /* check if the grammar set is in an unstable state */
  if ( !cfgSP->built ) cfgSetBuild (cfgSP, DEFAULT_SCORES, 0, 0);

#ifdef CFG_DEBUG
  fprintf (stderr, "score: extend LCT=%s, LVX=%s\n",
	   cfgLexiconGetName (cfgSP->cfgLP, cfgPTNP->lvX, T_Arc),
	   cfgLexiconGetName (cfgSP->cfgLP, lvX, T_Arc));
#endif

  /* extend given PTNode, by the old lvX */
  childPTNP = cfgPTNodeExtend (cfgPTNP, lvX);

  score = cfgPTNodeScore (childPTNP, cfgSP->cfgPTP, w);

#ifdef CFG_DEBUG
  fprintf (stderr, "score: LCT=%s, LVX=%s, score=%d\n",
	   cfgLexiconGetName (cfgSP->cfgLP, lvX, T_Arc),
	   cfgLexiconGetName (cfgSP->cfgLP, w,   T_Arc),
	   score);
#endif

  /* return the score */
  return (lmScore)score;
}

/* creates the initial LCT (bos) */
LCT cfgSetLingKSCreateLCT (LingKS* lksP, LVX w) {

  CFGSet *cfgSP = lksP->data.cfgSP;
  LCT     res;

  /* check if the grammar set is in an unstable state */
  if ( !cfgSP->built ) cfgSetBuild (cfgSP, DEFAULT_SCORES, 0, 0);

#ifdef CFG_DEBUG
  fprintf (stderr, "create: LVX=%s\n",
	   cfgLexiconGetName (cfgSP->cfgLP, w, T_Arc));
#endif

  res = nl2lct(cfgSP->cfgPTP->root, w);

  return res;
}

/* reduce an LCT by n words */
LCT cfgSetLingKSReduceLCT (LingKS* lksP, LCT lct, int n) {

  WARN ("not yet implemented\n");
  return lct;
}

/* extend an LCT by a word */
LCT cfgSetLingKSExtendLCT (LingKS* lksP, LCT lct, LVX w) {

  CFGPTNode *cfgPTNP   = lct2node(lct);
  CFGPTNode *childPTNP = NULL;
  LVX        lvX       = lct2lvx(lct);
  LCT        res;

#ifdef CFG_DEBUG
  {
    CFGSet *cfgSP = lksP->data.cfgSP;
    fprintf (stderr, "extend: LCT=%s, LVX=%s\n",
	     cfgLexiconGetName (cfgSP->cfgLP, cfgPTNP->lvX, T_Arc),
	     cfgLexiconGetName (cfgSP->cfgLP, lvX, T_Arc));
  }
#endif

  /* just in case if there were no score or scoreArray call before */
  childPTNP = cfgPTNodeExtend (cfgPTNP, lvX);

  res = nl2lct(childPTNP, w);

  return res;
}

/* returns the last n words represented by LCT
   if n==-1 the default history will be used (0) */
int cfgSetLingKSDecodeLCT (LingKS* lksP, LCT lct, LVX* w, int n) {

  WARN ("not yet implemented\n");
  return n;
}

/* free this lct and all its dependent lcts */
int cfgSetLingKSFreeLCT (LingKS* lksP, LCT lct) {

  WARN ("not yet implemented\n");
  return TCL_OK;
}

/* return the number of LVXs */
int cfgSetLingKSitemN (LingKS* lksP) {

  CFGSet *cfgSP = lksP->data.cfgSP;

  return cfgLexiconGetItemN (cfgSP->cfgLP, T_Arc);
}

/* returns the LVX of a given name */
LVX cfgSetLingKSitemX (LingKS* lksP, char *name) {

  CFGSet *cfgSP = lksP->data.cfgSP;

  return cfgLexiconGetIndex (cfgSP->cfgLP, name, T_Arc);
}

/* returns the name of a given LVX */
char* cfgSetLingKSitemA (LingKS* lksP, int i) {

  CFGSet *cfgSP = lksP->data.cfgSP;

  return cfgLexiconGetName (cfgSP->cfgLP, (LVX)i, T_Arc);
}

int cfgSetLingKSIsDirty (LingKS* lksP) {

  int dirty = lksP->dirty;

  lksP->dirty = 0;

  return dirty;
}

int cfgSetLingKSSaveDump (LingKS* lksP, FILE *fp) {

  return cfgSetSaveDump (lksP->data.cfgSP, fp, 0);
}

int cfgSetLingKSLoadDump (LingKS* lksP, FILE *fp, char *txt) {

  return cfgSetLoadDump (lksP->data.cfgSP, fp);
}

/* initialize LingKS */
int cfgSetLingKSInit (LingKS *lksP, CFGSet *cfgSP, char *name) {

  cfgSetInit  (cfgSP, (ClientData)name);
  cfgSetAlloc (cfgSP);

  cfgSP->lksP = lksP;

  lksP->lingKSSaveItfFct = cfgSetSaveItf;
  lksP->lingKSLoadItfFct = cfgSetLoadItf;
  lksP->lingKSSaveFct    = cfgSetLingKSSaveDump;
  lksP->lingKSLoadFct    = cfgSetLingKSLoadDump;
  lksP->lingKSPutsFct    = cfgSetPutsItf;
  lksP->scoreArrayFct    = cfgSetLingKSScoreArray;
  lksP->scoreFct         = cfgSetLingKSScore;
  lksP->createLCT        = cfgSetLingKSCreateLCT;
  lksP->reduceLCT        = cfgSetLingKSReduceLCT;
  lksP->extendLCT        = cfgSetLingKSExtendLCT;
  lksP->decodeLCT        = cfgSetLingKSDecodeLCT;
  lksP->freeLCT          = cfgSetLingKSFreeLCT;
  lksP->itemN            = cfgSetLingKSitemN;
  lksP->itemX            = cfgSetLingKSitemX;
  lksP->itemA            = cfgSetLingKSitemA;
  lksP->isDirty          = cfgSetLingKSIsDirty;

  return TCL_OK;
}


/****************************************************************************/

/* ========================================================================
    Type Information
   ======================================================================== */


/* ---------- itf types for CFGArc */
static Method cfgArcMethod[] = {
  { "puts",  cfgArcPutsItf, "display the contents of CFG arc" },
  { NULL, NULL, NULL }
};

TypeInfo cfgArcInfo = { "CFGArc", 0, -1, cfgArcMethod,
			NULL, NULL,
			cfgArcConfigureItf, cfgArcAccessItf,
			itfTypeCntlDefaultNoLink,
  "A 'CFGArc' object is an arc between two nodes of a context free grammar." };


/* ---------- itf types for CFGNode */
static Method cfgNodeMethod[] = {
  { "puts",  cfgNodePutsItf,  "display the contents of CFG node" },
  { NULL, NULL, NULL }
};

TypeInfo cfgNodeInfo = { "CFGNode", 0, -1, cfgNodeMethod,
			 NULL, NULL,
			 cfgNodeConfigureItf, cfgNodeAccessItf,
			 itfTypeCntlDefaultNoLink,
  "A 'CFGNode' object is a node in a context free grammar." };


/* ---------- itf types for CFGRule */
static Method cfgRuleMethod[] = {
  { "puts",     cfgRulePutsItf,     "display the contents of CFG rule" },
  { "addPath",  cfgRuleAddPathItf,  "adds a path to a rule" },
  { "generate", cfgRuleGenerateItf, "generates sentences starting with rule" },
  { NULL, NULL, NULL }
};

TypeInfo cfgRuleInfo = { "CFGRule", 0, -1, cfgRuleMethod,
			 NULL, NULL,
			 cfgRuleConfigureItf, cfgRuleAccessItf,
			 itfTypeCntlDefaultNoLink,
  "A 'CFGRule' object is a rule of a context free grammar." };


/* ---------- itf types for CFG */
static Method cfgMethod[] = {
  { "addPath",  cfgAddPathItf,  "adds a path to a CFG" },
  { "build",    cfgBuildItf,    "builds a context free grammar" },
  { "clear",    cfgClearItf,    "clears a context free grammar" },
  { "compress", cfgCompressItf, "compress a context free grammar" },
  { "load",     cfgLoadItf,     "loads a context free grammar" },
  { "parse",    cfgParseItf,    "parse a sentence" },
  { "puts",     cfgPutsItf,     "display the contents of CFG" },
  { "reduce",   cfgReduceItf,   "reduces a context free grammar" },
  { "save",     cfgSaveItf,     "saves a context free grammar" },
  { NULL, NULL, NULL }
};

TypeInfo cfgInfo = { "CFG", 0, -1, cfgMethod,
		     cfgCreateItf, cfgFreeItf,
		     cfgConfigureItf, cfgAccessItf,
		     NULL,
  "A 'CFG' object is a context free grammar." };


/* ---------- itf types for CFGSet */
static Method cfgSetMethod[] = {
  { "build",    cfgSetBuildItf,    "builds a context free grammar set" },
  { "clear",    cfgSetClearItf,    "clears a context free grammar set" },
  { "compress", cfgSetCompressItf, "compress a context free grammar set" },
  { "load",     cfgSetLoadItf,     "loads a context free grammar set" },
  { "parse",    cfgSetParseItf,    "parse a sentence" },
  { "puts",     cfgSetPutsItf,     "display the contents of CFG set" },
  { "reduce",   cfgSetReduceItf,   "reduces a context free grammar set" },
  { "save",     cfgSetSaveItf,     "saves a context free grammar set" },
  { NULL, NULL, NULL }
};

TypeInfo cfgSetInfo = { "CFGSet", 0, -1, cfgSetMethod,
			cfgSetCreateItf, cfgSetFreeItf,
			cfgSetConfigureItf, cfgSetAccessItf,
			NULL,
  "A 'CFGSet' object is a set of context free grammar." };



/* ---------- CFG_Init ---------- */

static int cfgInitialized = 0;

int CFG_Init (void) {

  if ( !cfgInitialized ) {

    if ( BMem_Init()         != TCL_OK ) return TCL_ERROR;
    if ( CFGLexicon_Init()   != TCL_OK ) return TCL_ERROR;
    if ( CFGRuleStack_Init() != TCL_OK ) return TCL_ERROR;
    if ( CFGParseTree_Init() != TCL_OK ) return TCL_ERROR;

    cfgArcDefault.lvX		= LVX_NIL;
    cfgArcDefault.type		= 0;
    cfgArcDefault.score		= CFG_INIT_SCORE;
    cfgArcDefault.cfgRP		= NULL;
    cfgArcDefault.cfgNP		= NULL;

    cfgNodeDefault.type		= 0;
    cfgNodeDefault.arcA		= NULL;
    cfgNodeDefault.arcN		= 0;

    cfgRuleDefault.lvX		= LVX_NIL;
    cfgRuleDefault.type		= 0;
    cfgRuleDefault.status	= Active;
    cfgRuleDefault.weight	= 0;
    cfgRuleDefault.cfgP		= NULL;
    cfgRuleDefault.root		= NULL;
    cfgRuleDefault.leaf		= NULL;

    cfgDefault.name		= NULL;
    cfgDefault.useN 		= 0;
    cfgDefault.tag		= NULL;
    cfgDefault.header		= NULL;
    cfgDefault.package		= NULL;
    cfgDefault.importA		= NULL;
    cfgDefault.importN		= 0;
    cfgDefault.ruleRefA		= NULL;
    cfgDefault.ruleRefN		= 0;
    cfgDefault.status		= Active;
    cfgDefault.weight		= 0;
    cfgDefault.bosNP		= NULL;
    cfgDefault.eosNP		= NULL;
    cfgDefault.root		= NULL;
    cfgDefault.cfgRSP		= NULL;
    cfgDefault.cfgSP		= NULL;
    cfgDefault.cfgLP		= NULL;
    cfgDefault.cfgPTP		= NULL;
    cfgDefault.lksP		= NULL;
    cfgDefault.mem.nodes	= NULL;
    cfgDefault.mem.rules	= NULL;
    cfgDefault.allPublic	= 0;
    cfgDefault.startover	= -1.0;
    cfgDefault.built	       	= 0;
    cfgDefault.buildMode	= FIXED_SCORES;

    cfgSetDefault.useN		= 0;
    cfgSetDefault.list.itemN	= 0;
    cfgSetDefault.list.blkSize	= 50;
    cfgSetDefault.cfgLP		= NULL;
    cfgSetDefault.cfgRSP	= NULL;
    cfgSetDefault.cfgPTP	= NULL;
    cfgSetDefault.lksP		= NULL;
    cfgSetDefault.built         = 0;

    itfNewType (&cfgArcInfo);
    itfNewType (&cfgNodeInfo);
    itfNewType (&cfgRuleInfo);
    itfNewType (&cfgInfo);
    itfNewType (&cfgSetInfo);

    cfgInitialized = 1;
  }

  return TCL_OK;
}

