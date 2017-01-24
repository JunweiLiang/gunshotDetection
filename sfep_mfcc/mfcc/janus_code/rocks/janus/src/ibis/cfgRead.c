/* ========================================================================
    JANUS      Speech- to Speech Translation
               ------------------------------------------------------------
               Object: Context Free Grammar
               ------------------------------------------------------------

    Author  :  Christian Fuegen
    Module  :  cfgRead.c
    Date    :  $Id: cfgRead.c 3416 2011-03-23 03:02:18Z metze $
    Remarks :  common grammar reading functions

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

    Revision 1.1.2.5  2003/08/07 14:36:04  fuegen
    removed some windows compiler warnings

    Revision 1.1.2.4  2003/08/07 14:29:29  fuegen
    removed bug in cfReadLinkTokens (optionality)

    Revision 1.1.2.3  2003/07/15 16:50:04  fuegen
    added forward/recursive arc types

    Revision 1.1.2.2  2003/07/03 09:26:38  fuegen
    removed gcc warning of uninitialized variable

    Revision 1.1.2.1  2003/06/23 13:12:07  fuegen
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

/* ======================================================================
   NodeInfo Init / Deinit
   ====================================================================== */

int nodeInfoInit (NodeInfo *niP, NodeInfo *parentP) {

  niP->childA   = NULL;
  niP->tokenA   = NULL;
  niP->tokenN   = 0;
  niP->cfgNP    = NULL;
  niP->cfgAP    = NULL;
  niP->parentP  = parentP;
  niP->tag      = NULL;
  niP->asterisk = 0;
  niP->plus     = 0;
  niP->paren    = 0;
  niP->bracket  = 0;
  niP->_and      = 0;
  niP->_or       = 0;
  niP->forward  = 0;

  niP->weight   = CFG_DEFAULT_WEIGHT;

  niP->nextrP    = NULL;
  niP->nextlP    = NULL;

  cfgArcInit (&(niP->arc));

  return TCL_OK;
}

int nodeInfoDeinit (NodeInfo *niP) {

  int i;

  if ( niP->childA ) {
    for (i=0; i<niP->tokenN; i++) nodeInfoDeinit (&(niP->childA[i]));
    Nulle (niP->childA);
  }

  if ( niP->tokenA ) {
    for (i=0; i<niP->tokenN; i++) Nulle (niP->tokenA[i]);
    Nulle (niP->tokenA);
  }

  if ( niP->tag ) Nulle (niP->tag);

  return nodeInfoInit (niP, NULL);
}


/* ======================================================================
   Common Grammar Reading Functions
   ====================================================================== */

/* Florian: otherwise, some grammars won't be read correctly (doesn't work for all compilers) */
#pragma optimization_level 0

/* ----------------------------------------------------------------------
   filter/normalize rule
   ---------------------------------------------------------------------- */
int cfgReadFilter (char *from, char **to, int n, int *tpos, CFGGrammarFormat f) {

  char tline[CFG_MAXLINE];
  int  tn  = 0;
  int  tag = 0;
  int  nbs = 1;
  int  m   = 0;
  char c, b;
  int  i;

  for (i=0; i<n; i++) {
    c = from[i];

    /* handle back slash (nbs = 1 if not back slashed)
       !! we don't handle back slashed ' ', i.e. '\ *' -> '\*' !! */
    if ( nbs == 0 )         nbs = 1;
    if ( nbs == 2 )         nbs = 0;
    if ( nbs && c == '\\' ) nbs = 2;

    /* handle tags */
    if ( nbs && c == '{' ) tag++;
    if ( nbs && c == '}' ) {
      tag--;
      if ( !tag ) { tline[tn++] = c; tn = 0; continue; }
    }
    if ( tag ) { tline[tn++] = c; continue; }

    /* if we are not in a tag and if this char is not back slashed */
    if ( nbs && !tag ) {
      if ( c == '\t' || c == '\r' || c == '\n' ) c = ' ';

      /* check history */
      if ( *tpos > 0 ) {
	b = (*to)[(*tpos)-1];

	if ( b == ' ' && c == ' ' ) continue;
	switch (f) {
	case JSGF_Format:
	  if ( b == ' ' && 
	       (c == ')' || c == ']' || c == '*' || c == '+') ) {
	    (*to)[(*tpos)-1] = c;
	    continue;
	  }
	  if ( (b == '(' || b == '[' || b == '/' || b == '|') &&
	       c == ' ' ) continue;
	  break;
	case SOUP_Format:
	  if ( b == ' ' && 
	       (c == ')' || c == ']') ) {
	    (*to)[(*tpos)-1] = c;
	    continue;
	  }
	  if ( (b == '(' || b == '[' || b == '*' || b == '+') &&
	       c == ' ' ) continue;
	  break;
	default:
	  ERROR ("cfgReadFilter: unknown file format.\n");
	}
      }
    }

    (*to)[*tpos] = c;
    (*tpos)++;
    m++;
  }

  (*to)[*tpos] = '\0';

  return m;
}

/* ----------------------------------------------------------------------
   split token into subtokens, returns modifiers and tokenA
   possible tokens, e.g.
	'(a b) (c d)', 'a b* (c) d*', '(a b c d)*', 'a b | c d'
   ---------------------------------------------------------------------- */
char* cfgReadSplitToken (char *token, NodeInfo *niP, CFGGrammarFormat f) {

  char  *tP       = token;
  int    tN       = strlen(token);
  int    paren[3];
  int    bracket[3];
  int    asterisk[2];
  int    plus[2];
  int    weightStart = 0;
  int    i, j = 0;
  int    bpc;
  char   c;

#ifdef CFG_DEBUG
  fprintf (stderr, ">>> SPLITJSGF '%s'\n", tP);
#endif

  /* init */
  for (i=1; i<3; i++) paren[i]    = bracket[i] = -1;
  for (i=1; i<2; i++) asterisk[i] = plus[i]    = -1;
  paren[0]    = bracket[0] = 0;
  asterisk[0] = plus[0]    = 0;

  /* search for ANDed tokens 'a | b', asterisk '(a)*' and pluses '(b)+' */
  for (i=0; i<tN; i++) {
    c = token[i];

    if ( c == '(' ) {
      paren[0]++;
      if ( paren[0]   == 1 && paren[1]   == -1 ) paren[1]   = i;
    }
    if ( c == '[' ) {
      bracket[0]++;
      if ( bracket[0] == 1 && bracket[1] == -1 ) bracket[1] = i;
    }

    if ( !paren[0] && !bracket[0] ) {
      if ( c == '*' ) { asterisk[0]++; asterisk[1] = i; }
      if ( c == '+' ) { plus[0]++;     plus[1]     = i; }
    }

    /* check for a starting weight */
    if ( i == 0 && c == '/' ) weightStart = 1;

    if ( !paren[0] && !bracket[0] && c == '|' ) {
      char d;

      j = i-1;

      while ( j >= 0 && token[j] == ' ' ) j--;
      d = token[j+1]; token[j+1] = '\0';

      niP->tokenA = (char**)realloc (niP->tokenA,
				     (niP->tokenN + 1) * sizeof (char*));
      niP->tokenA[niP->tokenN++] = strdup (tP);
#ifdef CFG_DEBUG
      fprintf (stderr, "token+ '%s'\n", tP);
#endif
      token[j+1] = d; j = i+1;

      while ( j < tN && token[j] == ' ' ) j++;
      tP = token+j;
    }

    if ( c == ']' ) {
      bracket[0]--;
      if ( bracket[0] == 0 && bracket[2] == -1 ) bracket[2] = i;
    }
    if ( c == ')' ) {
      paren[0]--;
      if ( paren[0]   == 0 && paren[2]   == -1 ) paren[2]   = i;
    }
  }

  /* do we have ORed subtokens */
  if ( niP->tokenN > 0 ) {
    /* process last */
    j = i-1;

    while ( j >= 0 && token[j] == ' ' ) j--;
    c = token[j+1]; token[j+1] = '\0';

    niP->tokenA = (char**)realloc (niP->tokenA,
				   (niP->tokenN + 1) * sizeof (char*));
    niP->tokenA[niP->tokenN++] = strdup (tP);
#ifdef CFG_DEBUG
    fprintf (stderr, "token+ '%s'\n", tP);
#endif
    niP->_or    = 1;
    token[j+1] = c;

    return tP;
  }

  /* no we don't have ORed tokens, so try to split at ' ' */
  tP          = token;
  paren[0]    = bracket[0] = 0;
  bpc         = 0;

  for (i=0; i<tN; i++) {
    c = token[i];

    if ( c == '(' ) paren[0]++;
    if ( c == '[' ) bracket[0]++;

    /* START FF-HACK --- fixed prob with missing spaces */
    if ((bpc && (c!='*') && (c!='+')) || (!paren[0] && !bracket[0] && (c == ' '))) {
      char d;

      j = i-1;

      while ( j >= 0 && token[j] == ' ' ) j--;
      d = token[j+1]; token[j+1] = '\0';

      niP->tokenA = (char**)realloc (niP->tokenA,
				     (niP->tokenN + 1) * sizeof (char*));
      niP->tokenA[niP->tokenN++] = strdup (tP);
#ifdef CFG_DEBUG
      fprintf (stderr, "token+ '%s'\n", tP);
#endif

      token[j+1] = d; 
      j = i;
      if (c == ' ')
        j = i+1;

      while ( j < tN && token[j] == ' ' ) j++;
      tP = token+j;
    }

    if ( c == ']' ) bracket[0]--;
    if ( c == ')' ) paren[0]--;

    bpc = 0;
    if (!paren[0] && !bracket[0] && (c == ']' || c == ')')) bpc = 1;
    /* END FF-HACK --- fixed prob with missing spaces */

  }

  /* do we have ANDed subtokens */
  if ( niP->tokenN > 0 ) {
    /* process last */
    j = i-1;

    while ( j >= 0 && token[j] == ' ' ) j--;
    c = token[j+1]; token[j+1] = '\0';

    if ( strlen (tP) ) {
      niP->tokenA = (char**)realloc (niP->tokenA,
				     (niP->tokenN + 1) * sizeof (char*));
      niP->tokenA[niP->tokenN++] = strdup (tP);
#ifdef CFG_DEBUG
      fprintf (stderr, "token+ '%s'\n", tP);
#endif
    }

    niP->_and   = 1;
    token[j+1] = c;

    return tP;
  }

  /* no we don't have ORed or ANDed subtokens, so do some further analysis */
  switch (f) {
  case SOUP_Format:
    niP->asterisk = (asterisk[1] == 0);
    if ( niP->asterisk ) {
      niP->plus = (plus[1] == 1);
    } else {
      niP->plus = (plus[1] == 0);
      if ( niP->plus ) niP->asterisk = (asterisk[1] == 1);
    }
    i = 0;
    j = niP->asterisk + niP->plus;
    break;
  case JSGF_Format:
    niP->asterisk = (asterisk[1] == tN-1);
    niP->plus     = (plus[1]     == tN-1);
    i = niP->asterisk + niP->plus;
    j = 0;
    break;
  default:
    ERROR ("cfgReadSplitToken: unknown file format.\n");
  }

  if ( paren[1]   == 0 && paren[2]   == tN-1-i ) {
    /* we have a token surrounded by '()' */
    tP            = tP+1+j;
    c             = token[tN-1-i];
    token[tN-1-i] = '\0';
    niP->paren    = 1;

    niP->tokenA   = (char**)realloc (niP->tokenA,
				     (niP->tokenN + 1) * sizeof (char*));
    niP->tokenA[niP->tokenN++] = strdup (tP);
#ifdef CFG_DEBUG
    fprintf (stderr, "token+ '%s'\n", tP);
#endif

    token[tN-1-i] = c;
    return tP;
  } else if ( f == JSGF_Format && bracket[1] == 0 && bracket[2] == tN-1-i ) {
    /* we have a token surrounded by '[]' */
    tP            = tP+1+j;
    c             = token[tN-1-i];
    token[tN-1-i] = '\0';
    niP->bracket  = 1;

    niP->tokenA   = (char**)realloc (niP->tokenA,
				     (niP->tokenN + 1) * sizeof (char*));
    niP->tokenA[niP->tokenN++] = strdup (tP);

    token[tN-1-i] = c;
    return tP;
  } else if ( (niP->asterisk || niP->plus) && !weightStart ) {
    /* we have tokens with modifiers */
    tP          = tP+j;
    c           = token[tN-i];
    token[tN-i] = '\0';

    niP->tokenA = (char**)realloc (niP->tokenA,
				   (niP->tokenN + 1) * sizeof (char*));
    niP->tokenA[niP->tokenN++] = strdup (tP);
#ifdef CFG_DEBUG
    fprintf (stderr, "token+ '%s'\n", tP);
#endif

    token[tN-i] = c;
    return tP;
  } else if ( niP->asterisk || niP->plus ) {
    /* recover everything, because we first have to remove the weight */
    niP->asterisk = niP->plus = 0;
    i = j = 0;
  }

  /* we have already a single token */
  tP          = tP+j;
  c           = token[tN-i];
  token[tN-i] = '\0';
  /* token[tN-i] = c; we do not recover here!! */

  return tP;
}

/* start FF Hack --- introduced this function */
void lpMerge(NodeInfo* dest, NodeInfo* add) {
  NodeInfo *tSP1       = NULL;
  NodeInfo *tSP2       = NULL;

  assert (dest); // XCode says could be NULL
    
  /* remove joint nodes from tlSP */
  tSP1 = dest;
  while (tSP1) {
    if (tSP1==add)
      add = add->nextlP;
    tSP2 = add;
    while (tSP2) {
      if (tSP1==tSP2->nextlP)
        tSP2->nextlP = tSP2->nextlP->nextlP;
      tSP2 = tSP2->nextlP;
    }
    tSP1 = tSP1->nextlP;
  }

  /* append elements */
  while (dest->nextlP) dest = dest->nextlP;
  dest->nextlP = add;
}
/* end FF Hack --- introduced this function */

/* link nodes in collected NodeInfo structs
   while linking:  rootSP -> BLOCK -> leafSP
   after linking:  rootSP = start nodes of BLOCK
                   leafSP = end   nodes of BLOCK */
int cfgReadLinkTokens (CFG *cfgP, NodeInfo *niP, NodeInfo **rootSP, NodeInfo **leafSP, CFGGrammarFormat f) {

  NodeInfo *rSP        = NULL;
  NodeInfo *lSP        = NULL;
  NodeInfo *trSP       = NULL;
  NodeInfo *tlSP       = NULL;
  NodeInfo *tSP        = NULL;
  CFGArc   *cfgAP      = NULL;
  int       optional   = 0;
  int       repeatable = 0;
  int       i;

  switch (f) {
  case SOUP_Format:
    if ( niP->plus     ) repeatable = 1;
    if ( niP->asterisk ) optional   = 1;
    break;
  case JSGF_Format:
    if ( niP->bracket  ) optional   = 1;
    if ( niP->plus     ) repeatable = 1;
    if ( niP->asterisk ) optional   = repeatable = 1;
    break;
  default:
    ERROR ("cfgReadLinkTokens: unknown file format.\n");
  }

  if ( niP->_and ) {
    /* items in childA are ANDed (row)
       new rootSP = first returned rSP
       new leadSP = last  returned lSP */
    rSP = *rootSP;
    lSP =  NULL;

    for (i=0; i<niP->tokenN; i++) {
      /* propagate weight of current node forward to the first child */
      if ( i == 0 && niP->weight != CFG_DEFAULT_WEIGHT ) {
	if ( niP->childA[0].weight == CFG_DEFAULT_WEIGHT ) {
	  niP->childA[0].weight  = niP->weight;
	} else {
	  niP->childA[0].weight *= niP->weight;
	}
      }

      cfgReadLinkTokens (cfgP, &(niP->childA[i]), &rSP, &lSP, f);

      if ( i == 0 ) *rootSP = rSP;

      rSP =  lSP;
      lSP =  NULL;
    }

    *leafSP = rSP;
  } else if ( niP->_or ) {
    /* items in childA are ORed (parallel)
       new rootSP = merging of all returned rSPs
       new leafSP = merging of all returned lSPs */
    float w = 0;

    rSP  = *rootSP;
    lSP  =  NULL;

    /* normalize weights in all child nodes */
    for (i=0; i<niP->tokenN; i++) {
      if ( niP->childA[i].weight != CFG_DEFAULT_WEIGHT ) {
	w += niP->childA[i].weight;
      } else {
	if ( w != 0 ) {
	  ERROR ("cfgReadLinkTokens: weights not defined for all alternatives.\n");
	  return TCL_ERROR;
	}
      }
    }
    for (i=0; i<niP->tokenN && w != 0; i++) {  niP->childA[i].weight /= w; }

    /* propagate weight of current node forward to the child */
    if ( niP->weight != CFG_DEFAULT_WEIGHT ) {
      for (i=0; i<niP->tokenN && w != 0; i++) {
	if ( niP->childA[i].weight == CFG_DEFAULT_WEIGHT ) {
	  niP->childA[i].weight  = niP->weight;
	} else {
	  niP->childA[i].weight *= niP->weight;
	}
      }
    }

    for (i=0; i<niP->tokenN; i++) {
      cfgReadLinkTokens (cfgP, &(niP->childA[i]), &rSP, &lSP, f);

      if ( lSP ) {
	/* START FF HACK */
	lpMerge(lSP, tlSP);
	/* tSP = lSP;
	   while ( tSP->nextlP ) tSP = tSP->nextlP;
	   tSP->nextlP = tlSP; */
	/* END FF HACK */
	tlSP        = lSP;
      }
      
      if ( rSP ) {
	tSP = rSP;
	while ( tSP->nextrP ) tSP = tSP->nextrP;
	tSP->nextrP = trSP;
	trSP        = rSP;
      }

      rSP = *rootSP;
      lSP =  NULL;
    }

    *rootSP = trSP;
    *leafSP = tlSP;
  } else if ( niP->tokenN == 0 ) {
    /* we have a single node
       link with all available rSPs
       new rooSP = new leafSP = current nodeInfo */
    rSP = *rootSP;
    lSP = *leafSP;

    /* set score of arc to node weight */
    if ( niP->weight != CFG_DEFAULT_WEIGHT ) {
      niP->arc.score = CompressScore(P2S(niP->weight));
    }

    while ( rSP ) {
      cfgAP = cfgNodeAddArc (rSP->cfgNP, cfgP, niP->arc.lvX, (CFGArcType)(niP->arc.type ^ rSP->forward), niP->arc.score, niP->arc.cfgRP, 0);
      /* if this is an already existing arc */
      if ( cfgAP->cfgNP ) {
	/* we have already a different arc, so add new node info */
	if ( niP->cfgNP && niP->cfgNP != cfgAP->cfgNP ) {
	  int j = niP->tokenN;
	  niP->childA = (NodeInfo*)realloc (niP->childA, ++niP->tokenN * sizeof (NodeInfo));

	  nodeInfoInit (&(niP->childA[j]), NULL);
	  niP->childA[j].cfgNP     = cfgAP->cfgNP;
	  niP->childA[j].cfgAP     = NULL;
	  niP->childA[j].parentP   = niP->parentP;
	  niP->childA[j].tag       = niP->tag;
	  niP->childA[j].asterisk  = niP->asterisk;
	  niP->childA[j].plus      = niP->plus;
	  niP->childA[j].paren     = niP->paren;
	  niP->childA[j].bracket   = niP->bracket;
	  niP->childA[j]._and       = niP->_and;
	  niP->childA[j]._or        = niP->_or;
	  niP->childA[j].forward   = niP->forward;
	  niP->childA[j].nextrP    = niP->nextrP;
	  niP->childA[j].nextlP    = niP->nextlP;

	  niP->childA[j].arc.lvX   = niP->arc.lvX;
	  niP->childA[j].arc.type  = niP->arc.type;
	  niP->childA[j].arc.score = niP->arc.score;
	  niP->childA[j].arc.cfgRP = niP->arc.cfgRP;

	} else {
	  niP->cfgNP = cfgAP->cfgNP;
	}
      } else {
	if ( !niP->cfgNP )
	  niP->cfgNP = cfgNodeCreate (cfgP, Normal_Node);
	cfgArcSetNode (cfgAP, niP->cfgNP);
      }

      rSP = rSP->nextlP;
    }

    *rootSP = niP;
    *leafSP = niP;
  } else if ( optional || repeatable ) {
    /* items in childA are repeatable or optional */
    rSP = *rootSP;
    lSP = *leafSP;

    assert (niP->tokenN == 1);

    /* propagate weight of current node forward to the child */
    if ( niP->weight != CFG_DEFAULT_WEIGHT ) {
      if ( niP->childA[0].weight == CFG_DEFAULT_WEIGHT ) {
	niP->childA[0].weight  = niP->weight;
      } else {
	niP->childA[0].weight *= niP->weight;
      }

      if ( optional ) niP->childA[0].weight /= 2.0;
    }

    cfgReadLinkTokens (cfgP, &(niP->childA[0]), &rSP, &lSP, f);

    /* backward arc (repeatabilty)
       insert link from current leafSPs to old rootSPs */
    if ( repeatable ) {
      trSP = rSP;
      while ( trSP ) {
	tlSP = lSP;
	while ( tlSP ) {
	  cfgAP = cfgNodeAddArc (tlSP->cfgNP, cfgP, trSP->arc.lvX, (CFGArcType)(trSP->arc.type ^ 0x4), trSP->arc.score, trSP->arc.cfgRP, 0);
	  assert (trSP->cfgNP);
	  cfgArcSetNode (cfgAP, trSP->cfgNP);
	  tlSP = tlSP->nextlP;
	}
	trSP = trSP->nextrP;
      }
    }

    /* forward arc (optionality)
       append all old rootSPs to current leafSPs */
    if ( optional ) {
      if ( lSP ) {
	tlSP = lSP;
	while ( tlSP->nextlP ) tlSP = tlSP->nextlP;
	trSP = *rootSP;
	/* START FF HACK - introduced this if */
	if (tlSP!=trSP) {
	  /* END FF HACK - introduced this if */
	  while ( trSP ) {
	    trSP->forward   = 0x8;
	    /* START FF HACK */
	    /* tlSP->nextlP    = trSP;*/
	    /* lpAdd(lSP, tlSP->nextlP, trSP); */
	    lpMerge(lSP, trSP);
	    trSP            = trSP->nextrP;
	    /*tlSP            = tlSP->nextlP; */
	    /* END FF HACK - introduced this if */
	  }
	}
      }
    }
    
    *leafSP = lSP;
    *rootSP = rSP;
  } else {
    /* e.g. paren, return rootSP and leafSP */
    assert (niP->tokenN == 1);

    /* propagate weight of current node forward to the child */
    if ( niP->weight != CFG_DEFAULT_WEIGHT ) {
      if ( niP->childA[0].weight == CFG_DEFAULT_WEIGHT ) {
	niP->childA[0].weight  = niP->weight;
      } else {
	niP->childA[0].weight *= niP->weight;
      }
    }

    cfgReadLinkTokens (cfgP, &(niP->childA[0]), rootSP, leafSP, f);
  }

  return TCL_OK;
}
