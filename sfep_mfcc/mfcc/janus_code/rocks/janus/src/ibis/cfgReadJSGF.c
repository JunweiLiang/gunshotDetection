/* ========================================================================
    JANUS      Speech- to Speech Translation
               ------------------------------------------------------------
               Object: Context Free Grammar
               ------------------------------------------------------------

    Author  :  Christian Fuegen
    Module  :  cfgReadJSGF.c
    Date    :  $Id: cfgReadJSGF.c 3178 2010-03-05 16:52:47Z metze $
    Remarks :  reads a grammar in JSGF format

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

    Revision 1.1.2.11  2003/07/25 09:51:33  fuegen
    only terminals are added with CFG_DEFAULR_SCORE

    Revision 1.1.2.10  2003/07/15 17:02:05  fuegen
    some more changes
      - added forward/recursive arc types
      - added dumping of CFGs/CFGSets (together with pt/rs)
      - added compression/determinization of CFGs/CFGSets
      - rule stack belongs no more to a single grammar
      - changed LingKS load/save interface, added LingKSPutsFct

    Revision 1.1.2.9  2003/06/23 13:12:07  fuegen
    new CFG implementation
      - less memory usage
      - activation/deactivation down to rule level
      - adding of paths, grammar rules, grammars on the fly
      - basic JSGF grammar support
      - additional support of CFG as LingKS type
      - standalone support of CFG/CFGSet, when using only for e.g. parsing

 
   ======================================================================== */

#include "common.h"
#include "itf.h"
#include "cfg.h"

/* #define CFG_DEBUG */

/* ----------------------------------------------------------------------
   Forward Declarations
   ---------------------------------------------------------------------- */
static NodeInfo root;			/* root node info */
static char     whiteSep[] = " \t\n";	/* white space separators */
static int      mode       = 0;		/* current input mode */


/* ======================================================================
   helping functions for parsing
   ====================================================================== */

/* ----------------------------------------------------------------------
   detect current line type (mode)
   ---------------------------------------------------------------------- */
static int detectMode (char *token) {

  if ( mode == 0 ) {
    if      ( !strcmp (token, "#JSGF")   ) mode = HDR_Mode;
    else {
      ERROR ("detectMode: file doesn't follow JSGF specification.\n");
      return 0;
    }
  } else if ( mode == HDR_Mode ) {
    if      ( !strcmp (token, "grammar") ) mode = GRA_Mode;
    else {
      ERROR ("detectMode: file doesn't follow JSGF specification.\n");
      return 0;
    }
  } else if ( mode == GRA_Mode ) {
    if      ( !strcmp (token, "import")  ) mode = IMP_Mode;
    else if ( !strcmp (token, "public")  ) mode = LHS_Mode;
    else if ( token[0] == '<'            ) mode = LHS_Mode;
    else {
      ERROR ("detectMode: file doesn't follow JSGF specification.\n");
      return 0;
    }
  } else if ( mode == IMP_Mode ) {
    if      ( !strcmp (token, "import")  ) mode = IMP_Mode;
    else if ( !strcmp (token, "public")  ) mode = LHS_Mode;
    else if ( token[0] == '<'            ) mode = LHS_Mode;
    else {
      ERROR ("detectMode: file doesn't follow JSGF specification.\n");
      return 0;
    }
  } else {
    mode = LHS_Mode;
  }

  return mode;
}

/* ----------------------------------------------------------------------
   detect type of an arc, given by token
   ---------------------------------------------------------------------- */
static CFGArcType detectArcType (char **otoken) {

  CFGArcType  arcType  = T_Arc;
  char       *token    = *otoken;

  /* do we have a NT_Node */
  if ( (token = strchr(token, '<')) ) {
    arcType = NT_Arc;
    
    /* remove surrounding <> */
    if ( token[0] == '<' ) {
      int i;

      token++;

      i = strcspn (token, ">");
      if ( token[i] != '>' ) {
	ERROR ("detectArcType: no JSGF NT token '%s'.\n", *otoken);
	arcType = 0;
	token--;
	return arcType;
      }

      token[i] = '\0';
      *otoken = token;
    } else {
      ERROR ("detectArcType: no JSGF NT token '%s'.\n", *otoken);
      arcType = 0;
    }
  }

  return arcType;
}

/* ----------------------------------------------------------------------
   split rule name into it's parts
   ---------------------------------------------------------------------- */
static int splitNames (char *token, char **ruleP, char **grammarP) {

  char *tP = NULL;

  *ruleP    = token;
  *grammarP = NULL;

  if ( (tP = strrchr (token, '.')) ) {
    tP[0]     = '\0';
    *grammarP = token;
    *ruleP    = tP+1;
  }

  return TCL_OK;
}


/* ----------------------------------------------------------------------
   get the weight of current token
   ---------------------------------------------------------------------- */
static float getWeight (char **token) {

  char* lP = strchr (*token, '/');

  if ( lP == *token ) {
    char* tP = strchr (lP+1, '/');

    tP[0]  = '\0';
    *token = tP+1;

     return atof (lP+1);
  }

  return CFG_DEFAULT_WEIGHT;
}


/* ======================================================================
   parsing functions
   ====================================================================== */

/* parse token into sub tokens */
int parseJSGFTokens (CFG *cfgP, char *lP, NodeInfo *niP, CFGRule *cfgRP) {

  CFGArcType  arcType;
  CFGRule     *rule;
  LVX          lvX;
  char        *token;
  float        weight=CFG_DEFAULT_WEIGHT;
  int i;

#ifdef CFG_DEBUG
  fprintf (stderr, "JSGF >>> '%s' %p\n", lP, lP);
  assert (strlen (lP));
#endif
  token       = cfgReadSplitToken (lP, niP, JSGF_Format);

  if ( niP->tokenN == 0 ) {
    weight = getWeight (&token);

    if ( weight != CFG_DEFAULT_WEIGHT ) {
      token = cfgReadSplitToken (token, niP, JSGF_Format);
      niP->weight = weight;
    }
#ifdef CFG_DEBUG
    fprintf (stderr, "JSGF    weight = %f\n", weight);
#endif
  }

#ifdef CFG_DEBUG
  fprintf (stderr, "JSGF    tokenN = %d\n", niP->tokenN);
  for (i = 0; i < niP->tokenN; i++)
    fprintf (stderr, "  %d= %p '%s'\n", i, niP->tokenA[i], niP->tokenA[i]);
#endif

  if ( niP->tokenN ) {
    /* process sub tokens */
    niP->childA = (NodeInfo*)malloc (niP->tokenN * sizeof (NodeInfo));

    for (i=0; i<niP->tokenN; i++) {
      nodeInfoInit    (niP->childA+i, niP);
      if ( parseJSGFTokens (cfgP, niP->tokenA[i], niP->childA+i, cfgRP) != TCL_OK )
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
      char *name, *grammar;

      splitNames (str, &name, &grammar);

      rule  = NULL;
      lvX   = cfgLexiconAddItem (cfgP->cfgLP, name, arcType);

      if ( grammar ) cfgAddRuleReference (cfgP, lvX, grammar);
    } else {
      lvX   = cfgLexiconAddItem (cfgP->cfgLP, str,   arcType);
    }

    if ( weight != CFG_DEFAULT_WEIGHT ) score = CompressScore (P2S(weight));

    cfgArcAlloc (&(niP->arc), lvX, arcType, score, rule);

    if (ostr) free (ostr);
  }

  return TCL_OK;
}


/* parse rule body (RHS) */
int parseJSGFRuleBody (CFG *cfgP, char *lP, CFGRule *cfgRP) {

  NodeInfo *niP = NULL;
  NodeInfo *rSP = NULL;
  NodeInfo *lSP = NULL;

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
  if ( parseJSGFTokens (cfgP, niP->tokenA[1], &(niP->childA[1]), cfgRP) != TCL_OK ) {
    ERROR ("parseJSGFRuleBody: file doesn't follow JSGF specification.\n");
    return TCL_ERROR;
  }

  /* linking nodes */
  if ( cfgReadLinkTokens (cfgP, niP, &rSP, &lSP, JSGF_Format) != TCL_OK ) {
    ERROR ("parseJSGFRuleBody: file doesn't follow JSGF specification.\n");
    return TCL_ERROR;
  }

  /* deinit nodeInfo */
  nodeInfoDeinit (&root);

  return TCL_OK;
}

/* parse rule (LHS+RHS) */
int parseJSGFRule (CFG *cfgP, char *lP, CFGRule **cfgRPP, int unique) {
  CFGRule     *cfgRP;
  CFGRuleType  ruleType = Private_Rule;
  int         len;

  /* first token hast to be either 'public' followed by a <rule>
     or only a '<rule>' */
  /* START FF HACK */
  /* originally: 
  char* token = NULL;
  token = strtok(lP, whiteSep);

  if ( !strcmp (token, "public") ) {
    ruleType = Public_Rule;
    token    = strtok (NULL, whiteSep);
  }

  if ( token[0] == '<' ) {
    token++;
    token[strcspn(token, ">")] = '\0';

    if ( (cfgRP = cfgAddRule (cfgP, token, ruleType, unique)) == NULL ) {
      ERROR ("parseJSGFRule: file doesn't follow JSGF specification.\n");
      return TCL_ERROR;
    }
  } else {
    ERROR ("parseJSGFRule: file doesn't follow JSGF specification.\n");
    return TCL_ERROR;
  }
  *cfgRPP = cfgRP;

  token = strtok (NULL, whiteSep);
  if ( token[0] != '=' ) {
    ERROR ("parseJSGFRule: file doesn't follow JSGF specification.\n");
    return TCL_ERROR;
  }
  return parseJSGFRuleBody (cfgP, token+2, cfgRP);
  */  

  /* new Version: */
  len = strcspn(lP, " <");
  if (len>0 && strncmp("public", lP, ((len<6) ? len : 6))==0) {
    ruleType = Public_Rule;
    lP += len + strcspn(&lP[len], "<");
  }

  if ( *lP == '<' ) {
    lP++;
    len = strcspn(lP, ">");
    if (len>1) {
      lP[len] = 0;
      if ( (cfgRP = cfgAddRule (cfgP, lP, ruleType, unique)) == NULL ) {
        ERROR ("parseJSGFRule: file doesn't follow JSGF specification.\n");
        return TCL_ERROR;
      }
      lP += len + 1;
      lP += strcspn(lP, "=");
    } else {
        ERROR ("parseJSGFRule: empty rule (\"<>\") or missing \">\".\n");
        return TCL_ERROR;
      }
  } else {
    ERROR ("parseJSGFRule: file doesn't follow JSGF specification.\n");
    return TCL_ERROR;
  }

  *cfgRPP = cfgRP;

  if (*lP != '=' ) {
    ERROR ("parseJSGFRule: file doesn't follow JSGF specification.\n");
    return TCL_ERROR;
  }
  lP++;
  lP += strspn(lP, " ");

  return parseJSGFRuleBody (cfgP, lP, cfgRP);
  /* END FF HACK */
}

/* parse a JSGF line */
int parseJSGFLine (CFG *cfgP, char *lP, CFGRule **cfgRPP) {

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
  case HDR_Mode:
    return cfgInitHeader (cfgP, lP+i+1);
  case GRA_Mode:
    return cfgInitName   (cfgP, lP+i+1);
  case IMP_Mode:
    return cfgAddImport  (cfgP, lP+i+1);
  case LHS_Mode:
    return parseJSGFRule (cfgP, lP, cfgRPP, 1);
  }

  ERROR ("parseJSGFLine: file doesn't follow JSGF specification.\n");
  return TCL_ERROR;
}


/* ======================================================================
   reading JSGF file
   ====================================================================== */
/* ----------------------------------------------------------------------
   appending new line, checking also for tags
   ---------------------------------------------------------------------- */
static int append (char *from, char **to, int n, int *tpos, int *tmax, int *tag) {

  int  nbs = 1;
  int  i;
  char c;

  if ( *tpos + n > *tmax ) {
    *tmax += CFG_MAXLINE;
    *to    = (char*)realloc (*to, *tmax * sizeof (char));
  }

  memcpy (*to+*tpos, from, n);

  /* update tag */
  for (i=0; i<n; i++) {
    c = (*to)[(*tpos)+i];

    /* handle back slash (nbs = 1 if not back slashed)
       !! we don't handle back slashed ' ': '\ *' -> '\*' !! */
    if ( nbs == 0 )          nbs = 1;
    if ( nbs == 2 )          nbs = 0;
    if ( nbs && c == '\\' )  nbs = 2;

    /* handle tags */
    if ( nbs && c == '{' )  (*tag)++;
    if ( nbs && c == '}' )  (*tag)--;
  }    

  (*tpos) += n;
  (*to)[*tpos] = '\0';

  return n;
}

int cfgReadJSGF (CFG *cfgP, FILE *fp, int v) {

  CFGRule *cfgRP    = NULL;
  int      lineN    = 0;
  char     line[CFG_MAXLINE];
  char    *lP       = &line[0];
  char    *rline;
  char    *tlP, *ulP;
  int      i, rN, rn, n, tag;

  rN    = CFG_MAXLINE;
  rline = (char*)malloc (rN * sizeof(char));
  rn    = 0;
  tag   = 0;
  mode  = 0;

  while ( !feof (fp) && fgets (line, CFG_MAXLINE, fp) ) {
    lineN++;

    if (strlen(line) > 1 && line[strlen(line)-2] == '\r')
      line[strlen(line)-2] = ' ';
    if (strlen(line) > 1 && line[strlen(line)-1] == '\n')
      line[strlen(line)-1] = ' ';
    while (line[strlen(line)-1] == ' ')
      line[strlen(line)-1] = 0;

    /* skip leading white spaces and empty lines */
    i = strspn (line, whiteSep);
    if ( i == (int)strlen (line) ) continue;
    else lP = &line[i];

    /* handling of comments */
    if ( strspn (lP, "/") > 1 ) continue;

    tlP = strstr (lP, "//");
    if ( tlP ) tlP[0] = '\0';

    tlP = lP;
    tlP = strstr (lP, "/*");
    while ( tlP ) {
      ulP = strstr (tlP, "*/");

      if ( !ulP ) {
	ERROR ("cfgReadJSGF (ulP): parsing failed at least in line %d.\n", lineN);
	Nulle (rline);
	return TCL_ERROR;
      }

      n   = tlP - lP;
      append (lP, &rline, n, &rn, &rN, &tag);

      lP  = ulP + 2;
      tlP = strstr (lP, "/*");
    }

    /* append everything to rline */
    n  = strlen (lP);
    append (lP, &rline, n, &rn, &rN, &tag);

    /* detect ';' */
    n  = strlen (rline);
    lP = strrchr (rline, ';');
    if ( lP > rline && lP[-1] == '\\' ) lP = NULL;
    if ( !tag && lP && n-(lP-rline) <= 2 ) {
      lP[0] = '\0';
      n     = 0;

      /* filter/normalize line */
      cfgReadFilter (rline, &rline, lP-rline, &n, JSGF_Format);

#ifdef CFG_DEBUG
      fprintf (stderr, "cfgReadJSGF (%d):\n%s\n\n", lineN, rline);
#endif
      /* parse rline */
      if ( parseJSGFLine (cfgP, rline, &cfgRP) != TCL_OK ) {
	ERROR ("cfgReadJSGF: parsing failed at least in line %d.\n", lineN);
	Nulle (rline);
	return TCL_ERROR;
      }

      rn   = 0;
    }
  }

  Nulle (rline);

  return TCL_OK;
}
