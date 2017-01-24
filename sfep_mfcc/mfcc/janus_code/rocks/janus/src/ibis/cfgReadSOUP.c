/* ========================================================================
    JANUS      Speech- to Speech Translation
               ------------------------------------------------------------
               Object: Context Free Grammar
               ------------------------------------------------------------

    Author  :  Christian Fuegen
    Module  :  cfgReadSOUP.c
    Date    :  $Id: cfgReadSOUP.c 3108 2010-01-30 20:41:26Z metze $
    Remarks :  reads a grammar in SOUP format

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
    Revision 4.2  2004/07/23 17:25:33  fuegen
    - Extended supported grammar definition file formats:
       - FSM:  AT&T's FSM (finite state machine) text file format
       - PFSG: Probabilistic Finite State Graph format used by the
               SRI-LM toolkit
    - Added support for reading weights specified in the JSGF format
    - Build functionality of grammars expanded by a mode for making
      equally distributed transitions instead of using fixed scores
    - Added support for generating terminal sequences for specific
      rules either randomly or fixed

    Revision 4.1  2003/08/14 11:20:00  fuegen
    Merged changes on branch jtk-01-01-15-fms (jaguar -> ibis-013)

    Revision 1.1.2.17  2003/07/25 09:51:33  fuegen
    only terminals are added with CFG_DEFAULR_SCORE

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

    Revision 1.1.2.14  2002/06/14 08:53:02  fuegen
    removed bug in handling of * and + characters

    Revision 1.1.2.13  2002/06/06 10:02:31  fuegen
    code cleaning, reorganisation of configurable options

    Revision 1.1.2.12  2002/04/18 09:24:07  fuegen
    minor changes

    Revision 1.1.2.11  2002/01/29 17:17:38  fuegen
    made gcc clean

    Revision 1.1.2.10  2002/01/25 17:16:28  fuegen
    added handling of starting new trees, added handling of shared grammars, added some minor things for saving memory

    Revision 1.1.2.9  2002/01/25 17:10:21  fuegen
    first real working version (only parsing)

    Revision 1.1.2.8  2002/01/25 17:02:36  fuegen
    extended parse tree implementation

    Revision 1.1.2.7  2002/01/25 16:53:27  fuegen
    major and minor changes: lexicon as cache, extension routines, lingKS interface

    Revision 1.1.2.6  2002/01/25 16:38:55  fuegen
    reimplementation of a big part, using now indices for memory saving reasons, splitted CFG-implementation in smaller modules

    Revision 1.1.2.5  2002/01/25 15:52:03  fuegen
    added rule stack and traversing functions

    Revision 1.1.2.4  2002/01/25 14:58:02  fuegen
    only minor changes

    Revision 1.1.2.3  2001/07/13 10:44:14  fuegen
    saved current grammar support version

    Revision 1.1.2.2  2001/06/01 08:54:38  fuegen
    new revision of CFG (many changes)

    Revision 1.1.2.1  2001/04/12 17:04:45  fuegen
    initial revision

 
   ======================================================================== */

#include "common.h"
#include "itf.h"
#include "cfg.h"

/* Example grammar

  s[toprule]
	( word1 RULE1 word3 Rule1 )
	( [rule1] *[rule2] word3 )
	( word1 +[rule2] [_NT_rule1] )
	( +*word1 )

  [rule1]
	( a )

  [rule2]
	( b )

  RULE1
	( c )

  Rule1
	( d )

  [_NT_rule1]
	( e )
  
*/

/* !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
   !!! currently NO SUPPORT for speaker side and character set rules !!!!
   !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! */

/* ----------------------------------------------------------------------
   Forward Declarations
   ---------------------------------------------------------------------- */
static NodeInfo root;			/* root node info */
static char     whiteSep[]  = " \t\n";	/* white space separators */
static char     cmtSep[]    = ";#%{";
static char     lhsSep[]    = "[";
static char     ucSep[]     = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
static char     anyString[] = "$any$";
static int      mode        = 0;	/* current input mode */


/* ======================================================================
   helping functions for parsing
   ====================================================================== */

/* ----------------------------------------------------------------------
   detect current line type (mode)
   ---------------------------------------------------------------------- */
static int detectMode (char *token) {

  if ( token[0] == '(' ) return RHS_Mode;

  return LHS_Mode;
}

/* ----------------------------------------------------------------------
   detect type of a rule, given by token
   ---------------------------------------------------------------------- */
static CFGRuleType detectRuleType (char **otoken) {

  CFGRuleType  ruleType = 0;
  char        *token    = *otoken;

  /* detect type of LHS (s, s1, v, c) - starting with lower case letter */
  if ( token[0] == 's' ) {		/* top level */
    token++;
    if ( token[0] == '1' ||
	 token[0] == '2' ||
	 token[0] == '3' ||
	 token[0] == '4' ||
	 token[0] == '5' ||
	 token[0] == '6' ||
	 token[0] == '7' ||
	 token[0] == '8' ||
	 token[0] == '9'    ) {		/* speaker side */
      token++;
      WARN ("Speaker sides currently not supported. Treated as one speaker.\n");
    }
    ruleType = Public_Rule;
  } else if ( token[0] == 'v' ) {		/* lookup */
    token++;
    ruleType = Private_Rule;
    WARN ("Lookup LHS currently not supported. Treated as normal LHS.\n");
  } else if ( token[0] == 'c' ) {		/* character level */
    token++;
    ruleType = Private_Rule;
    ERROR ("Character level LHS currently not supported!\n");
    return 0;
  } else if ( token[0] == 'o' ) {		/* open class */
    token++;
    ruleType = Private_Rule;
    WARN ("Open class LHS currently not supported. Treated as normal LHS.\n");
  }

  /* LHS starting with upper case letter */
  if ( strspn(token, ucSep) >= 1 )
    ruleType = Aux_Rule;
  
  /* detect type in [] */
  if ( token[0] == '[' ) {
    /* remove [] */
    token++;
    token[strcspn(token, "]")] = '\0';
    
    /* Aux_Rule or Private_Rule in [] */
    if ( !ruleType ) {
      if ( !strncmp (token, "_NT_", 4) )
	ruleType = Aux_Rule;
      else
	ruleType = Private_Rule;
    }
  }

  *otoken = token;

  return ruleType;
}

/* ----------------------------------------------------------------------
   detect type of an arc, given by token
   ---------------------------------------------------------------------- */
static CFGArcType detectArcType (char **otoken) {

  CFGArcType  arcType = T_Arc;
  char       *token   = *otoken;

  if ( !strcmp(token, anyString) ) {
    ERROR ("detectArcType: %s not supported, added as normal terminal\n",
	   anyString);
    arcType = T_Arc;

  } else if ( strstr(token, lhsSep) ||
	      strspn(token, ucSep)  >= 1 ) {
    arcType = NT_Arc;
    
    /* remove surrounding [] */
    if ( token[0] == '[' ) {
      int i;

      token++;

      i = strcspn (token, "]");
      if ( token[i] != ']' ) {
	ERROR ("detectArcType: no SOUP NT token '%s'.\n", *otoken);
	arcType = 0;
	token--;
	return arcType;
      }

      token[i] = '\0';
    }
  }

  *otoken = token;

  return arcType;
}


/* ======================================================================
   parsing functions
   ====================================================================== */

/* parse token into sub tokens */
int parseSOUPTokens (CFG *cfgP, char *lP, NodeInfo *niP, CFGRule *cfgRP) {

  CFGArcType  arcType;
  CFGRule    *rule;
  LVX         lvX;
  char       *token;
  int         i;

#ifdef CFG_DEBUG
  fprintf (stderr, ">>> '%s'\n", lP);
#endif
  token = cfgReadSplitToken (lP, niP, SOUP_Format);
#ifdef CFG_DEBUG
  fprintf (stderr, "    %d\n", niP->tokenN);
#endif

  if ( niP->tokenN ) {
    /* process sub tokens */
    niP->childA = (NodeInfo*)malloc (niP->tokenN * sizeof (NodeInfo));

    for (i=0; i<niP->tokenN; i++) {
      nodeInfoInit    (&(niP->childA[i]), niP);
      if ( parseSOUPTokens (cfgP, niP->tokenA[i], &(niP->childA[i]), cfgRP) != TCL_OK )
	return TCL_ERROR;
    }
  } else {
    /* add node */
    char     *str   = strdup (token);
    char     *ostr  = str;
    CFGScore  score = CFG_INIT_SCORE;

    arcType    = detectArcType (&str);
    rule       = cfgRP;

    if ( !arcType ) {
      if (ostr) free (ostr);
      return TCL_ERROR;
    }

    if ( arcType == NT_Arc ) {
      rule  = NULL;
    }

    lvX        = cfgLexiconAddItem (cfgP->cfgLP, str, arcType);
    cfgArcAlloc (&(niP->arc), lvX, arcType, score, rule);

    if (ostr) free (ostr);
  }

  return TCL_OK;
}

/* parse rule body (RHS) */
int parseSOUPRuleBody (CFG *cfgP, char *lP, CFGRule *cfgRP) {

  NodeInfo *niP = NULL;
  NodeInfo *rSP = NULL;
  NodeInfo *lSP = NULL;
  int       i;
  char     *p   = NULL;

  /* remove surrounding '()' and ' ' */
  if ( (p = strchr (lP, '(')) == NULL ) {
    ERROR ("parseSOUPRuleBody: file doesn't follow SOUP specification.\n");
    return TCL_ERROR;
  }
  lP    = p + 1;
  if ( (p = strrchr (lP, ')')) == NULL ) {
    ERROR ("parseSOUPRuleBody: file doesn't follow SOUP specification.\n");
    return TCL_ERROR;
  }
  i     = p - lP;
  lP[i] = '\0';

  /* init nodeInfo (root lP leaf) */
  nodeInfoInit (&root, NULL);
  niP = &root;

  niP->tokenN = 3;
  niP->tokenA = (char**)malloc (niP->tokenN * sizeof (char*));

  niP->tokenA[0] = NULL;
  niP->tokenA[1] = strdup (lP);
  niP->tokenA[2] = NULL;
  niP->_and       = 1;

  niP->childA    = (NodeInfo*)malloc (niP->tokenN * sizeof (NodeInfo));

  nodeInfoInit (&(niP->childA[0]), niP);
  nodeInfoInit (&(niP->childA[2]), niP);
  niP->childA[0].cfgNP = cfgRP->root;
  niP->childA[2].cfgNP = cfgRP->leaf;

  niP->childA[2].arc.type  = Lambda_Arc;
  niP->childA[2].arc.cfgRP = cfgRP;
  
  nodeInfoInit (&(niP->childA[1]), niP);
  if ( parseSOUPTokens (cfgP, niP->tokenA[1], &(niP->childA[1]), cfgRP) != TCL_OK ) {
    ERROR ("parseSOUPRuleBody: file doesn't follow SOUP specification.\n");
    return TCL_ERROR;
  }

  /* linking nodes */
  if ( cfgReadLinkTokens (cfgP, niP, &rSP, &lSP, SOUP_Format) != TCL_OK ) {
    ERROR ("parseSOUPRuleBody: file doesn't follow SOUP specification.\n");
    return TCL_ERROR;
  }

  /* deinit nodeInfo */
  nodeInfoDeinit (&root);

  return TCL_OK;
}


/* parse rule (LHS) */
int parseSOUPRule (CFG *cfgP, char *lP, CFGRule **cfgRPP, int unique) {

  char        *token    = strtok (lP, whiteSep);
  CFGRuleType  ruleType = 0;

  /* check if really LHS
     LHS could start with s[, c[, s1[, oHI, ... */
  if ( !(strstr (token, lhsSep) || strspn (token, ucSep) >= 1 ||
	 (token[0] == 'o' && strcspn (token, ucSep) == 1)) ) {
    ERROR ("parseSOUPRule: could not detect LHS.\n");
    return TCL_ERROR;
  }

  /* detect rule type */
  ruleType = detectRuleType (&token);
      
  if ( !ruleType ) {
    ERROR ("parseSOUPRule: could not detect rule type.\n");
    return TCL_ERROR;
  }
      
  /* create rule */
  if ( (*cfgRPP = cfgAddRule (cfgP, token, ruleType, unique)) == NULL ) {
    ERROR ("parseSOUPLine: file doesn't follow SOUP specification.\n");
    return TCL_ERROR;
  }

  return TCL_OK;
}

/* parse a SOUP line */
int parseSOUPLine (CFG *cfgP, char *lP, CFGRule **cfgRPP) {

  char *token = lP;
  char  c;
  int   i;

  /* detect mode */
  i     = strcspn (lP, whiteSep);
  c     = lP[i];
  lP[i] = '\0';
  mode  = detectMode (token);
  lP[i] = c;

  switch (mode) {
  case LHS_Mode:
    return parseSOUPRule     (cfgP, lP,  cfgRPP, 1);
  case RHS_Mode:
    return parseSOUPRuleBody (cfgP, lP, *cfgRPP);
  }

  ERROR ("parseSOUPLine: file doesn't follow SOUP specification.\n");
  return TCL_ERROR;
}


/* ======================================================================
   reading SOUP file
   ====================================================================== */
int cfgReadSOUP (CFG *cfgP, FILE *fp) {

  CFGRule *cfgRP      = NULL;
  int      lineN      = 0;
  char     line[CFG_MAXLINE];
  char    *lP         = &line[0];
  char    *rline;
  int      i, rN, rn;

  rN    = CFG_MAXLINE;
  rline = (char*)malloc (rN * sizeof(char));
  rn    = 0;

  while ( !feof (fp) && fgets (line, CFG_MAXLINE, fp) ) {
    lineN++;

    /* skip leading white spaces and empty lines */
    i = strspn (line, whiteSep);
    if ( i == (int)strlen (line) ) continue;
    else lP = &line[i];

    /* skip comment lines */
    if ( strspn (lP, cmtSep) > 0 ) continue;

    /* delete comments in line */
    i = strcspn (line, cmtSep);
    if ( i >= 1 ) line[i] = '\0';

    /* filter/normalize line */
    cfgReadFilter (lP, &rline, strlen(lP), &rn, SOUP_Format);

#ifdef CFG_DEBUG
    fprintf (stderr, "cfgReadSOUP (%d):\n%s\n\n", lineN, rline);
#endif
    /* parse rline */
    if ( parseSOUPLine (cfgP, rline, &cfgRP) != TCL_OK ) {
      ERROR ("cfgReadSOUP: parsing failed at least in line %d.\n", lineN);
      Nulle (rline);
      return TCL_ERROR;
    }

    rn = 0;
  }

  Nulle (rline);

  return TCL_OK;
}


