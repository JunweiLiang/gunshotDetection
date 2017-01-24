/* ========================================================================
    JANUS      Speech- to Speech Translation
               ------------------------------------------------------------
               Object: Context Free Grammar
               ------------------------------------------------------------

    Author  :  Christian Fuegen
    Module  :  cfg.h
    Date    :  $Id: cfg.h 3137 2010-02-20 03:01:11Z metze $
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
    Revision 4.4  2006/10/24 15:28:23  fuegen
    support for reading SRI-LMs WLAT format for saussages (confusion networks)

    Revision 4.3  2004/09/23 11:42:43  fuegen
    Made code 'x86-64'-clean
    Necessary changes include the usage of functions
    instead of macros for LCT->node/LCT->lvX conversion
    together with a union

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

    Revision 4.1  2003/08/14 11:19:59  fuegen
    Merged changes on branch jtk-01-01-15-fms (jaguar -> ibis-013)

    Revision 1.1.2.30  2003/08/13 08:52:00  fuegen
    startover requires now float factor

    Revision 1.1.2.29  2003/07/15 17:02:04  fuegen
    some more changes
      - added forward/recursive arc types
      - added dumping of CFGs/CFGSets (together with pt/rs)
      - added compression/determinization of CFGs/CFGSets
      - rule stack belongs no more to a single grammar
      - changed LingKS load/save interface, added LingKSPutsFct

    Revision 1.1.2.28  2003/06/23 13:12:06  fuegen
    new CFG implementation
      - less memory usage
      - activation/deactivation down to rule level
      - adding of paths, grammar rules, grammars on the fly
      - basic JSGF grammar support
      - additional support of CFG as LingKS type
      - standalone support of CFG/CFGSet, when using only for e.g. parsing

    Revision 1.1.2.27  2002/11/19 13:05:08  fuegen
    added deinit for CFGSet

    Revision 1.1.2.26  2002/08/26 11:26:37  fuegen
    Removed bugs in decoding along parallel grammars and
    with shared grammars.
    Added output of the parse tree.
    Added reading of grammar files with windows line endings.

    Revision 1.1.2.25  2002/06/14 08:55:26  fuegen
    added a more flexible FSGraph traversing (removed old extend/traverse)

    Revision 1.1.2.24  2002/06/06 12:51:25  fuegen
    LingKSPuts now works also for CFG

    Revision 1.1.2.23  2002/06/06 10:02:31  fuegen
    code cleaning, reorganisation of configurable options

    Revision 1.1.2.22  2002/05/21 15:24:21  fuegen
    added tmpScoreA for cfgSetScore function

    Revision 1.1.2.21  2002/04/29 13:34:56  metze
    Redesign of LM interface, integration of CFGSet

    Revision 1.1.2.20  2002/04/18 09:30:50  fuegen
    major changes in interface and main routines, handling of shared grammars, using of CFGExt

    Revision 1.1.2.19  2002/02/19 16:36:45  fuegen
    added training procedures

    Revision 1.1.2.18  2002/02/06 13:19:27  fuegen
    changed memory management (bmem)

    Revision 1.1.2.17  2002/02/01 14:46:00  fuegen
    changed rule stack handling

    Revision 1.1.2.16  2002/01/30 16:57:17  fuegen
    changed LingKS interface, added LCT cache

    Revision 1.1.2.15  2002/01/29 17:22:43  fuegen
    made gcc clean, added statistics

    Revision 1.1.2.14  2002/01/25 17:16:28  fuegen
    added handling of starting new trees, added handling of shared grammars, added some minor things for saving memory

    Revision 1.1.2.13  2002/01/25 17:10:21  fuegen
    first real working version (only parsing)

    Revision 1.1.2.12  2002/01/25 17:02:36  fuegen
    extended parse tree implementation

    Revision 1.1.2.11  2002/01/25 16:53:27  fuegen
    major and minor changes: lexicon as cache, extension routines, lingKS interface

    Revision 1.1.2.10  2002/01/25 16:38:55  fuegen
    reimplementation of a big part, using now indices for memory saving reasons, splitted CFG-implementation in smaller modules

    Revision 1.1.2.9  2002/01/25 15:52:02  fuegen
    added rule stack and traversing functions

    Revision 1.1.2.8  2002/01/25 14:58:02  fuegen
    only minor changes

    Revision 1.1.2.7  2001/07/13 10:44:15  fuegen
    saved current grammar support version

    Revision 1.1.2.6  2001/06/01 10:31:48  fuegen
    renamed pointer to LingKS to lksP

    Revision 1.1.2.5  2001/06/01 10:18:58  fuegen
    removed some warnings for gcc under Linux

    Revision 1.1.2.4  2001/06/01 09:52:32  fuegen
    removed some warnings for gcc under Linux

    Revision 1.1.2.3  2001/06/01 09:20:49  fuegen
    changed definition of cfgNodeCreate

    Revision 1.1.2.2  2001/06/01 08:54:09  fuegen
    new revision of CFG (many changes)

    Revision 1.1.2.1  2001/04/12 17:04:45  fuegen
    initial revision

 
   ======================================================================== */

#ifdef __cplusplus
extern "C" {
#endif
  
#ifndef _cfg_010205_CF
#define _cfg_010205_CF

#include <math.h>
#include "list.h"
#include "mach_ind_io.h"
#include "sglobal.h"
#include "bmem.h"
#include "cfgTypes.h"
#include "lks.h"

/* display debugging information */
/*#define CFG_DEBUG*/

  /* ========================================================================
     defines of values and macros
     ======================================================================== */

#define CFG_LEXICON_N		 2		/* for T_Arcs and NT_Arcs */
#define CFG_MAXLINE		 2048
#define CFG_MAXNODE              100
#define CFG_BITSHIFT            (sizeof(LCT)>>1)*8

#define CFG_SHARED_TAG           "SHARED"	/* shared CFG tag */


  /* probabilities and scores and weights
   * scores are defined as log-probabilities to a base of 10 */

#define CFG_INIT_PROB		 1.0
#define CFG_FIXED_PROB           1.0
#define CFG_DEFAULT_WEIGHT	 1.0

#define P2S(prob)        log(prob)/M_LN10
#define S2P(score)       exp(score*M_LN10)

#ifdef COMPRESSEDLM
  /*  #define   CompressScore(score) (lmScore)((score)*LMSCORE_SCALE_FACTOR-0.5)
      #define UnCompressScore(score) (float)  ((score)/LMSCORE_SCALE_FACTOR) */
  #define CFG_MAX_SCORE		 LMSCORE_MAX
  #define CFG_NULL_SCORE	 0
#else
  /*  #define   CompressScore(score) (score)
      #define UnCompressScore(score) (score)*/
  #define CFG_MAX_SCORE		 LMSCORE_MAX
  #define CFG_NULL_SCORE	 0.0
#endif

  #define CFG_INIT_SCORE	 CompressScore(P2S(CFG_INIT_PROB))
  #define CFG_FIXED_SCORE        CompressScore(P2S(CFG_FIXED_PROB))

#define OVERFLOW_ADD(VAL,OFFSET,MAX) \
  if ( ((MAX) - (OFFSET)) < (VAL) ) (VAL) += (OFFSET);	\
  else                            (VAL)  = (MAX);

  /* an LCT consists of a node pointer and an lvX (LCT = [*node,lvX])
     this union is for the conversion between all three types with
     the help of the functions lct2node, lct2lvX, nl2lct */

#ifndef JANUS_Aegis_H
/* Roger: To prevent linkage problem when linking with Wilson's library 
TODO: FIX THIS NASTY HACK !!!!!!*/
  union convLCT_p {
    void      *node;
    LVX        lvX;
    LCT        lct;
  } convLCT;
#endif

  /* ======================================================================
     Enumerations
     ====================================================================== */

  /* arc types */
  enum { T_Arc		= 1,	/* terminal symbol */
	 NT_Arc		= 2,	/* non terminal symbol */
	 Lambda_Arc	= 3,	/* empty symbol */
	 T_RArc		= 5,	/* recursive terminal symbol */
	 NT_RArc	= 6,	/* recursive non terminal symbol */
	 Lambda_RArc	= 7,	/* recursive empty symbol */
	 T_FArc		= 9,	/* forward terminal symbol */
	 NT_FArc	= 10,	/* forward non terminal symbol */
	 Lambda_FArc	= 11	/* forward empty symbol */
  };

  /* node types */
  enum { Normal_Node	= 1,	/* normal node */
	 Root_Node	= 2,	/* root node */
	 Leaf_Node	= 3	/* leaf node */
  };

  /* rule types */
  enum { Public_Rule	= 1,	/* could also be referenced by other grammars
				   outside this grammar; parse could start with */
	 Private_Rule	= 2,	/* could only be referenced in local grammar */
	 Aux_Rule	= 3,	/* auxilliary rule: same as private, but does
				   not occur in parse tree output */
	 Void_Rule	= 4,	/* can never be spoken */
	 Null_Rule	= 5,	/* matches automatically */
	 Undef_Rule	= 6,	/* currently undefined rule */
	 Root_Rule	= 7	/* root rule */
  };

  /* status types of grammars, rules */
  enum { Active		= 1,
	 Inactive	= 2,
	 Shared		= 3	/* implies always active */
  };
	 
  /* puts formats */
  enum { Short_Puts	= 1,	/* short */
	 Long_Puts	= 2,	/* long */
	 Recursive_Puts	= 3,	/* recursive */
	 FSM_Puts       = 4	/* AT&T FSM format */
  };

  /* grammar file formats */
  enum { SOUP_Format	= 1,	/* SOUP grammar file format */
	 JSGF_Format	= 2,	/* Java Speech Grammar file format */
	 FSM_Format     = 3,	/* AT&T FSM file format */
	 PFSG_Format    = 4,	/* SRI-LM PFSG file format */
	 WLAT_Format    = 5,	/* SRI-LM WLAT file format for word-meshes */
	 Dump_Format	= 6	/* Dumped Grammar file format */
  };

  /* reading modes */
  enum {
    LHS_Mode = 1,		/* left hand side (rule head) */
    RHS_Mode = 2,		/* right hand side (rule body) */
    GRA_Mode = 3,		/* grammar token */
    IMP_Mode = 4,		/* import token */
    HDR_Mode = 5		/* grammar header */
  };

  /* build modes */
  enum {
    DEFAULT_SCORES = 1,		/* use stored build mode */
    NULL_SCORES    = 2,		/* transition scores are set to null */
    FIXED_SCORES   = 3,		/* transition scores are set to fixed vals */
    EQUAL_SCORES   = 4		/* transition scores are equal distributed */
  };
    

  /* ========================================================================
     Context Free Grammar Arc
     ======================================================================== */

  struct CFGArc_S {
    LVX		  lvX;

    CFGArcType	  type;		/* type of arc */
    CFGScore	  score;	/* score of this arc */

    CFGRule	 *cfgRP;	/* for NTs:   pointer to rule, arc points to
				   otherwise: pointer to rule, arc belongs to */
    CFGNode	 *cfgNP;	/* pointer to next node */
  };


  /* ========================================================================
     Context Free Grammar Node
     ======================================================================== */

  struct CFGNode_S {
    CFGNodeType	  type;		/* type of node */

    CFGArc	**arcA;		/* array of arcs to next nodes */
    CFGArcX	  arcN;		/* number of arcs in array */
  };

  /* ========================================================================
     Context Free Grammar Rule (NTs) Definitions
     ======================================================================== */

  struct CFGRule_S {
    LVX			 lvX;		/* index of rule in lexicon */
    CFGRuleType		 type;		/* rule type */
    CFGStatus		 status;	/* status of rule (ACTIVE, INACTIVE) */
    CFGWeight		 weight;	/* weight for weighting this rule */

    CFGNode		*root;		/* root node to CFG rule body */
    CFGNode		*leaf;		/* common leaf for CFG rule body */

    CFG			*cfgP;		/* pointer to CFG, rule belongs to */
  };


  /* ========================================================================
     Context Free Grammar
     ======================================================================== */
  typedef struct {
    LVX   lvX;
    char *grammar;
  } CFGRuleRef;

  struct CFG_S {
    char		*name;
    int                  useN;
    char		*tag;		/* grammar tag (for grouping grammars) */

    char		*header;	/* JSGF header */
    char		*package;	/* JSGF package name,
					   tag=grammar name */
    CFGRuleRef		*importA;	/* JSGF imports */
    int                  importN;
    CFGRuleRef		*ruleRefA;	/* rule references */
    int                  ruleRefN;

    CFGStatus		 status;	/* status of grammar ((IN)ACTIVE, SHARED) */
    CFGWeight		 weight;	/* weight for weighting this grammar */

    CFGRule		*root;		/* root rule with bos/eos structure */
    CFGNode		*bosNP, *eosNP;	/* bos/eos nodes for cfgBuild */

    CFGRuleStack	*cfgRSP;	/* rule stack */
    CFGSet		*cfgSP;		/* grammar set, CFG belongs to */
    CFGLexicon		*cfgLP;		/* lexicon,    usually cfgSP->cfgLP  */
    CFGParseTree	*cfgPTP;	/* parse tree, usually cfgSP->cfgPTP */

    LingKS		*lksP;		/* the common information block for all
					   linguistic knowledge sources */
    struct {
      BMem		*arcs;
      BMem		*nodes;
      BMem		*rules;
    } mem;

    CFGBool		 allPublic;	/* set all rules public */
    float		 startover;	/* start over parsing */
    CFGBool		 built;		/* grammar built */
    CFGBuildMode	 buildMode;	/* build mode */
  };


  /* ========================================================================
     Context Free Grammar Set
     ======================================================================== */

  typedef struct LIST(CFG) CFGList;

  struct CFGSet_S {
    char	 *name;
    int		  useN;

    CFGList	  list;		/* list of context free grammars */

    CFGLexicon	 *cfgLP;	/* lexicon for all CFGs */
    CFGRuleStack *cfgRSP;	/* pointer to rule stack */
    CFGParseTree *cfgPTP;	/* pointer to the parse tree */

    LingKS	 *lksP;		/* the common information block for all
				   linguistic knowledge sources */

    CFGBool	  built;	/* are all grammars in set built */
  };

  /* ========================================================================
     Context Free Grammar Lexicon
     ======================================================================== */

  typedef struct LIST(CFGLexiconItem) CFGLexiconItemList;

  struct CFGLexiconItem_S {
    char		*name;		/* name of item (word) */
  };

  struct CFGLexicon_S {
    CFGLexiconItemList	 list[CFG_LEXICON_N];	/* lexicon lists of Ts and NTs */

    char		*beginOS;	/* symbol of begin of sentence */
    char		*endOS;		/* symbol of end   of sentence */
  };

  /* ======================================================================
     Context Free Grammar Rule Stack
     ====================================================================== */

  struct CFGRSItem_S {
    CFGArc		*cfgAP;		/* NT_Arc */
    CFGRSItem		*childP;	/* pointer to child item */
    CFGRSItem		*nextP;		/* pointer to next item */
    CFGRSItem		*parentP;	/* pointer to parent item */
  };

  struct CFGRuleStack_S {
    struct {				/* memory block with rule stack items */
      BMem		*items;
    } mem;

    CFGRSItem		*root;		/* root item */
  };

  /* ======================================================================
     Context Free Grammar Parse Tree
     ====================================================================== */

  typedef struct {
    CFGArc		*cfgAP;		/* cfg arc */
    CFGRSItem		*cfgRSIP;	/* pointer to rule stack item */
    CFGScore		 offset;	/* offset to best score of node */
    CFGPTItemX		 parentX;	/* index of parent item in parent node */
    float		 startover;	/* startover level */
  } CFGPTItem;

  struct CFGPTNode_S {
    LVX			 lvX;		/* lvX of this node (ID) */

    CFGPTItem		*itemA;		/* array of node items */
    CFGPTItemX		 itemN;		/* number of nodes */

    CFGPTItemX		 bestX;		/* index of best item */
    CFGScore		 bestScore;	/* score of best item */

    CFGPTNode		*childP;	/* child node */
    CFGPTNode		*nextP;		/* next nodes with same parent */
    CFGPTNode		*parentP;	/* parent node */
  };

  struct CFGParseTree_S {
    struct {				/* memory block with parse tree items */
      BMem		*nodes;
    } mem;

    CFGPTNode		*root;		/* root item */

    CFGScore		*initScoreA;	/* initial scoreA */

    CFGLexicon		*cfgLP;		/* lexicon */
  };

  /* ======================================================================
     Context Free Grammar Traverse
     ====================================================================== */

  typedef int CFGTraverseDo  (CFGTraverse *cfgTP, CFGNode *cfgNP, CFGArc *cfgAP,
			      CFGRSItem *cfgRSIP, CFGScore offset);

  struct CFGTraverse_S {
    CFGPTNode		 *cfgPTNP;
    CFGPTNode		 *childPTNP;
    CFGPTItemX		  ptitemX;
    LVX			  lvX;
    CFGScore		  score;
    CFGScore		 *scoreA;

    CFGTraverseDo	 *doLeafNode;
    CFGTraverseDo	 *doTArc;
    CFGTraverseDo	 *doNTArc;
    CFGTraverseDo	 *doLambdaArc;

    CFG			 *cfgP;
    CFGRule		 *cfgRP;
    CFGNode		 *cfgNP;
    CFGRSItem		 *cfgRSIP;
    CFGNode		**nXA;
    CFGNode		**mXA;
    CFGNodeX		  nXN;
    CFGNodeX		  mXN;
    CFGNodeX		  tXN;
    void		**cacheA;
    UINT                  cacheN;
    LVX			 *lvXA;

    float		  startover;
    CFGBool		  verbose;
  };

  /* ======================================================================
     Nodeinfo struct for reading/parsing grammar files
     ====================================================================== */

typedef struct NodeInfo_S NodeInfo;	/* node info structure to store
					   collected information */
struct NodeInfo_S {

  NodeInfo *childA;	/* node info struct for sub tokens */
  char    **tokenA;	/* sub tokens */
  int       tokenN;	/* number of sub tokens */

  int       _and;	/* tokens in tokenA are AND concatenated */
  int       _or;		/* tokens in tokenA are OR  concatenated */

  CFGNode  *cfgNP;	/* pointer to grammar node (set only when tokenN==0) */
  CFGArc   *cfgAP;	/* pointer to grammar arc  (set only when tokenN==0) */
  NodeInfo *parentP;	/* parent node info */

  CFGArc    arc;	/* grammar arc, used to store arc infos */

  char     *tag;	/* optional tag */
  int       asterisk;	/* asterisk modifier exists for this token */
  int       plus;	/* plus     modifier exists for this token */
  int       paren;	/* token surrounded by parens */
  int       bracket;	/* token surrounded by brackets */

  int       forward;

  float     weight;	/* weight for this node */

  NodeInfo *nextlP;	/* used when linking nodes (stack/list) */
  NodeInfo *nextrP;	/* used when linking nodes (stack/list) */
};

  /* ========================================================================
     Module Interface
     ======================================================================== */

  /* initialization functions */
  extern int CFG_Init		();
  extern int CFGLexicon_Init	();
  extern int CFGRuleStack_Init	();
  extern int CFGParseTree_Init	();

  /* ----------------------------------------------------------------------
     CFG
     ---------------------------------------------------------------------- */
  extern void* lct2node (LCT lct);
  extern LVX   lct2lvx  (LCT lct);
  extern LCT   nl2lct   (void *node, LVX lvX);

  extern char*            cfgPutsFormat2str    (CFGPutsFormat format);
  extern CFGPutsFormat    cfgPutsFormat2int    (char *format);
  extern char*            cfgArcType2str       (CFGArcType type);
  extern CFGArcType       cfgArcType2int       (char *type);
  extern char*            cfgNodeType2str      (CFGNodeType type);
  extern CFGNodeType      cfgNodeType2int      (char *type);
  extern char*            cfgGrammarFormat2str (CFGGrammarFormat format);
  extern CFGGrammarFormat cfgGrammarFormat2int (char *format);

  extern int      cfgDeinit        (CFG *cfgP);
  extern int      cfgSetDeinit     (CFGSet *cfgSP);
  extern int      cfgLingKSInit    (LingKS *lksP, CFG *cfgP, char *name);
  extern int      cfgSetLingKSInit (LingKS *lksP, CFGSet *cfgSP, char *name);
  extern CFGX     cfgSetGetCFGX    (CFGSet *cfgSP, CFG *cfgP);
  extern CFG*     cfgSetGetCFG     (CFGSet *cfgSP, CFGX cfgX);

  extern int      cfgArcInit    (CFGArc *cfgAP);
  extern int      cfgArcAlloc   (CFGArc *cfgAP, LVX lvX, CFGArcType type,
				 CFGScore score, CFGRule *cfgRP);
  extern CFGNode* cfgNodeCreate (CFG *cfgP, CFGNodeType type);
  extern CFGArc*  cfgNodeAddArc (CFGNode *cfgNP, CFG *cfgP, LVX lvX,
				 CFGArcType type, CFGScore score,
				 CFGRule *cfgRP, int force);
  extern int      cfgArcSetNode (CFGArc *cfgAP, CFGNode *cfgNP);

  extern CFGRule* cfgAddRule    (CFG *cfgP, char *name, CFGRuleType type, int unique);
  extern int      cfgAddRuleReference (CFG *cfgP, LVX lvX, char *grammar);

  extern int      cfgRuleFormat  (CFG *cfgP, CFGRule *cfgRP, CFGGrammarFormat f, char *nt);


  extern int      cfgInitHeader (CFG *cfgP, char *line);
  extern int      cfgInitName   (CFG *cfgP, char *line);
  extern int      cfgAddImport  (CFG *cfgP, char *token);

  extern CFGArcX  cfgGetArcX    (CFG *cfgP, CFGArc *cfgAP);
  extern CFGArc*  cfgGetArc     (CFG *cfgP, CFGArcX arcX);

  /* ----------------------------------------------------------------------
     Reading Grammars
     ---------------------------------------------------------------------- */
  extern int      cfgReadSOUP  (CFG *cfgP, FILE *fp);
  extern int      cfgReadJSGF  (CFG *cfgP, FILE *fp, int v);

  extern int parseSOUPRuleBody (CFG *cfgP, char *lP, CFGRule  *cfgRP);
  extern int parseJSGFRuleBody (CFG *cfgP, char *lP, CFGRule  *cfgRP);
  extern int parseSOUPRule     (CFG *cfgP, char *lP, CFGRule **cfgRPP, int unique);
  extern int parseJSGFRule     (CFG *cfgP, char *lP, CFGRule **cfgRPP, int unique);

  extern int nodeInfoInit   (NodeInfo *niP, NodeInfo *parentP);
  extern int nodeInfoDeinit (NodeInfo *niP);

  extern int   cfgReadFilter     (char *from, char **to, int n, int *tpos, CFGGrammarFormat f);
  extern char* cfgReadSplitToken (char *token, NodeInfo *niP, CFGGrammarFormat f);
  extern int   cfgReadLinkTokens (CFG *cfgP, NodeInfo *niP, NodeInfo **rootSP, NodeInfo **leafSP, CFGGrammarFormat f);

  /* ----------------------------------------------------------------------
     CFGLexicon
     ---------------------------------------------------------------------- */
  extern CFGLexicon* cfgLexiconCreate   ();
  extern int         cfgLexiconFree     (CFGLexicon *cfgLP);
  extern int         cfgLexiconGetItemN (CFGLexicon *cfgLP, CFGArcType type);
  extern char*       cfgLexiconGetName  (CFGLexicon *cfgLP, LVX lvX,
					 CFGNodeType type);
  extern LVX         cfgLexiconGetIndex (CFGLexicon *cfgLP, char *name,
					 CFGNodeType type);
  extern LVX         cfgLexiconAddItem  (CFGLexicon *cfgLP, char *word,
					 CFGNodeType type);
  extern int         cfgLexiconPuts     (CFGLexicon *cfgLP, CFGArcType t, CFGPutsFormat f);
  extern int         cfgLexiconLoadDump (CFGLexicon *cfgLP, FILE *fp);
  extern int         cfgLexiconSaveDump (CFGLexicon *cfgLP, FILE *fp);

  /* ----------------------------------------------------------------------
     CFGRuleStack
     ---------------------------------------------------------------------- */
  extern CFGRuleStack* cfgRuleStackCreate ();
  extern int           cfgRuleStackFree   (CFGRuleStack *cfgRSP);
  extern CFGRSItem*    cfgRuleStackPush   (CFGRuleStack *cfgRSP,
					   CFGRSItem *cfgRSIP, CFGArc *cfgAP);
  extern CFGRSItem*    cfgRuleStackPop    (CFGRSItem *cfgRSIP, CFGArc **cfgAP);
  extern int           cfgRuleStackBuild  (CFGRuleStack *cfgRSP);
  extern int           cfgRuleStackClear  (CFGRuleStack *cfgRSP, int free);
  extern int           cfgRuleStackTrace  (CFGRSItem *from, CFGRSItem *to, float startover, int auxNT, CFGGrammarFormat f);
  extern int           cfgRuleStackLoadDump (CFGRuleStack *cfgRSP, FILE *fp, CFG *cfgP, CFGSet *cfgSP);
  extern int           cfgRuleStackSaveDump (CFGRuleStack *cfgRSP, FILE *fp, CFGSet *cfgSP);
  extern CFGRSItem*    cfgRuleStackGetItem (CFGRuleStack *cfgRSP, CFGRSItemX itemX);
  extern CFGRSItemX    cfgRuleStackGetItemX (CFGRuleStack *cfgRSP, CFGRSItem *cfgRSIP);

  /* ----------------------------------------------------------------------
     CFGTraverse
     ---------------------------------------------------------------------- */
  extern CFGTraverse* cfgTraverseCreate ();
  extern int          cfgTraverseFree   (CFGTraverse *cfgTP);
  extern int          cfgTraverse       (CFG *cfgP, CFGNode *cfgNP, CFGRSItem *cfgRSIP, CFGScore offset, CFGTraverse *cfgTP);
  extern int          cfgTraverseRand   (CFG *cfgP, CFGNode *cfgNP, CFGRSItem *cfgRSIP, CFGScore offset, CFGTraverse *cfgTP);

  /* ----------------------------------------------------------------------
     CFGParseTree
     ---------------------------------------------------------------------- */
  extern CFGParseTree* cfgParseTreeCreate  (CFGLexicon *cfgLP);
  extern int           cfgParseTreeFree    (CFGParseTree *cfgPTP);
  extern int           cfgParseTreeBuild   (CFGParseTree *cfgPTP);
  extern int           cfgParseTreeBuildCFG (CFGParseTree *cfgPTP, CFGArc *cfgAP,
					     CFGRSItem *cfgRSIP, CFGScore score);
  extern int           cfgParseTreeClear   (CFGParseTree *cfgPTP, int free);
  extern CFGPTNodeX    cfgParseTreeGetNodeX (CFGParseTree *cfgPTP, CFGPTNode *cfgPTNP);
  extern int           cfgParseTreeLoadDump (CFGParseTree *cfgPTP, FILE *fp, CFG *cfgP, CFGSet *cfgSP);
  extern int           cfgParseTreeSaveDump (CFGParseTree *cfgPTP, FILE *fp, CFGSet *cfgSP);

  extern CFGPTNode*    cfgPTNodeExtend     (CFGPTNode *cfgPTNP, LVX lvX);
  extern CFGScore      cfgPTNodeScore      (CFGPTNode *cfgPTNP, CFGParseTree *cfgPTP, LVX lvX);
  extern int           cfgPTNodeScoreArray (CFGPTNode *cfgPTNP, CFGParseTree *cfgPTP, CFGScore *scoreA);
  extern int           cfgPTNodeCompress   (CFGPTNode *cfgPTNP, CFGTraverse *cfgTP, int verbose);
  extern int           cfgPTNodeCompressBuild (CFGPTNode *cfgPTNP, CFGTraverse *cfgTP, int verbose);


#endif /* _cfg_010205_CF */
  
#ifdef __cplusplus
}
#endif
