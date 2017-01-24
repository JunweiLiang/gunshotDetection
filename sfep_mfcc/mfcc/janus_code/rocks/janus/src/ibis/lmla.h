/* ========================================================================
    JanusRTk   Janus Recognition Toolkit
               ------------------------------------------------------------
               Object: Language Model LookAhead
               ------------------------------------------------------------

    Author  :  Florian Metze
    Module  :  lmla.h
    Date    :  $Id: lmla.h 2755 2007-02-21 16:19:17Z fuegen $

    Remarks :  This is the new Language-Model Look-Ahead code.

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
    Revision 4.3  2007/02/21 16:19:17  fuegen
    moved macro Nulle from lmla.h to common.h

    Revision 4.2  2003/08/14 11:20:03  fuegen
    Merged changes on branch jtk-01-01-15-fms (jaguar -> ibis-013)

    Revision 4.1.2.47  2003/03/18 18:09:45  soltau
    Added time based constrains

    Revision 4.1.2.46  2002/08/01 13:42:31  metze
    Fixes for clean documentation.

    Revision 4.1.2.45  2002/05/02 11:59:46  soltau
    Removed unnecessary ltree construction structures

    Revision 4.1.2.44  2002/04/29 13:30:05  metze
    Redesign of LM interface (LingKS)

    Revision 4.1.2.43  2002/04/10 15:12:12  soltau
    Added svxHash

    Revision 4.1.2.42  2002/02/24 09:30:21  metze
    Rewrite of Lattice LM: now works with Hash Fct

    Revision 4.1.2.41  2002/02/21 07:06:05  metze
    Added GetOneLMScore

    Revision 4.1.2.40  2002/02/14 10:54:54  metze
    Rewrite of lmlaFillCtx, new interface to spass, ltree with svMap

    Revision 4.1.2.39  2002/02/13 08:00:51  metze
    Added lctHash stuff

    Revision 4.1.2.38  2002/02/10 17:51:23  metze
    Rewrite of lmlaFillCtx: working, but slower (#define alterkram)

    Revision 4.1.2.37  2002/02/07 12:13:12  metze
    Changes in interface to LM score functions

    Revision 4.1.2.36  2002/01/14 10:30:02  metze
    Cleaned up reading/ writing of dumps
    Added lingKS{Save|Load}Fct

    Revision 4.1.2.35  2001/10/22 08:40:12  metze
    Changed interface to score functions

    Revision 4.1.2.34  2001/10/11 16:56:26  soltau
    Added 'Flat_Constraint' Mode

    Revision 4.1.2.33  2001/10/09 18:33:49  soltau
    Added constraint_mode for acoustic rescoring

    Revision 4.1.2.32  2001/10/03 09:59:44  metze
    Added code for useN support in LMs

    Revision 4.1.2.31  2001/09/26 08:38:44  metze
    Renamed interpolLM to metaLM

    Revision 4.1.2.30  2001/09/26 10:09:41  soltau
    *** empty log message ***

    Revision 4.1.2.29  2001/09/25 14:53:33  soltau
    Moved Nulle to lmla.c

    Revision 4.1.2.28  2001/09/24 19:31:04  soltau
    Added equiP in latLmNode to support Path Recombination during constrained lattice decoding

    Revision 4.1.2.27  2001/08/13 14:27:32  metze
    Implemented acoustic rescoring with smeared LMLA
    (still doesn't work 100% ok)

    Revision 4.1.2.26  2001/06/18 08:02:41  metze
    Introduced bmem, LCT handling works on SUNs, new svmapGet

    Revision 4.1.2.25  2001/06/08 18:25:38  metze
    Acoustic rescoring of lattices works

    Revision 4.1.2.24  2001/06/01 10:21:56  metze
    Added dependency for cfg.h

    Revision 4.1.2.23  2001/06/01 10:02:44  metze
    Implemented decoding along lattices

    Revision 4.1.2.22  2001/06/01 08:57:04  fuegen
    removed LatticeLM

    Revision 4.1.2.21  2001/06/01 08:36:02  fuegen
    added CFGSet_type (Context Free Grammar Set Type) to LingKS

    Revision 4.1.2.20  2001/05/28 14:47:40  metze
    Added LatticeLM

    Revision 4.1.2.19  2001/05/23 08:09:25  metze
    LingKScheck does not give unneccessary output any longer

    Revision 4.1.2.18  2001/05/11 16:40:06  metze
    Cleaned up LCT interface

    Revision 4.1.2.17  2001/04/27 07:37:32  metze
    General access functions to LM via 'LCT' implemented

    Revision 4.1.2.16  2001/04/23 14:32:23  soltau
    Added lmlaReinit

    Revision 4.1.2.15  2001/04/23 11:54:42  metze
    Implemented second-level cache for nodes only

    Revision 4.1.2.14  2001/04/03 16:44:08  soltau
    *** empty log message ***

    Revision 4.1.2.13  2001/03/27 18:05:31  metze
    Introduced lmlaPhones and removed phones' score cache from lmlaFillCtx

    Revision 4.1.2.12  2001/03/22 16:12:36  metze
    Added aging cache

    Revision 4.1.2.11  2001/03/15 15:42:58  metze
    Re-implementation of LM-Lookahead

    Revision 4.1.2.10  2001/03/13 17:57:26  soltau
    Added phoneN for extended rcm lm lookahead

    Revision 4.1.2.9  2001/03/06 15:17:50  metze
    Fixed problems in lmla: Single-phone words and multiple use of nodes

    Revision 4.1.2.8  2001/03/05 08:55:53  metze
    Order of arguments for FillCtx cleaned up

    Revision 4.1.2.7  2001/03/05 08:39:34  metze
    Cleaned up treatment of exact LM in LMLA

    Revision 4.1.2.6  2001/02/28 16:33:44  metze
    Bugfix for lmlaInit (lnX)

    Revision 4.1.2.5  2001/02/27 15:10:27  metze
    LModelNJD and LMLA work with floats

    Revision 4.1.2.4  2001/02/09 09:36:48  metze
    New style snapshot, working fairly well

    Revision 4.1.2.3  2001/02/05 18:33:52  metze
    LM-LookAhead old style, but working

    Revision 4.1.2.2  2001/02/02 17:07:57  metze
    Bugfix

    Revision 4.1.2.1  2001/02/02 17:02:29  metze
    Made LM-LookAhead Cache work

    Revision 4.1  2001/01/25 13:45:23  metze
    Initial revision of language model lookahead


   ======================================================================== */

#ifndef _lmla
#define _lmla

#include "sglobal.h"
#include "slimits.h"
#include "stree.h"
#include "lks.h"
#include "ngramLM.h"
#include "phraseLM.h"
#include "metaLM.h"
#include "cfg.h"
#include "glat.h"


/* ========================================================================
     Various Defines
   ======================================================================== */


extern  TypeInfo   lmlaInfo;

#define LTREE_NULL_NODE 0

enum compressmode   { Child, Next, RootNode, CutOff };
enum ConstraintMode { Exact_Constraint, Weak_Constraint, Flat_Constraint, Time_Constraint };
enum LMLAMode       { Array_Mode, Single_Mode };


/* ======================================================================= 
     Structures
   ======================================================================= */

/* -----------------------------------------------------------------------
     Structure for the search constraint (lattice)
   ----------------------------------------------------------------------- */

typedef struct LIST(LatLMNode) LatLMList;

struct LATLMNODE_S {
  LCT            lct; /* The LCT belonging to this LatLMNode               */
  LCT            nct;
  SVX            svX; /* The svX                                           */
  GNode     **gnodeA; /* An array of GNodes belonging to this LatLMNode    */
  int         gnodeN; /*   (all must have identical svX)                   */
};


/* -------------------------------------------------------------------------
   HashLCT
   ------------------------------------------------------------------------- */

typedef struct HashLCT_S     HashLCT;
typedef struct LIST(HashLCT) HashLCTList;

struct HashLCT_S {
  LCT    lct;  /*    full lct */
  LCT    nct;  /* reduced lct */
};

/* -------------------------------------------------------------------------
   HashSVX
   ------------------------------------------------------------------------- */

typedef struct HashSVX_S     HashSVX;
typedef struct LIST(HashSVX) HashSVXList;

struct HashSVX_S {
  LCT    lct;   /* to expanding lct    */
  LCT    lct2;  /* expanded lct by svX */
  SVX    svx;   /* svx                 */
};

/* -----------------------------------------------------------------------
     Structure for a Language-Model-LookAhead
   ----------------------------------------------------------------------- */

struct LMLA_S {
  char*      name;    /* Object name                                       */
  int        useN;    /* Used by how many other objects?                   */

  STree*     streeP;  /* The corresponding Search Tree                     */
  SVMap*     svmapP;  /* The Vocab-Mapper for the LookAhead                */

  int        expert;  /* Hide nasty code */
  int        mode;    /* mode for LookAhead                                */
  int        depth;   /* maximum depth of the tree                         */
  int        reduced; /* Use the reduced history of the LMs                */

  int        lctHash; /* Do we need to use the hashed LCT handling?        */
  int        svxHash; /* additional hashtable to cache lct,svx->lct map    */
  HashLCTList hlctList; /* A list of hashed LCTs                           */
  HashSVXList hsvxList; /* A list of hashed SVXs                           */

                      /* -------- Tree structure: ------------------------ */
  int        leafsN;  /* number of leafs                                   */
  int        nodesN;  /* number of nodes                                   */
  int       *argsP;   /* arguments for the nodes                           */
  int       *argsA;
  int       *typeA;   /* types for the nodes                               */
  int       *typeP;

  struct {             /* A pointer to the search constraint structure     */
    GLat*      latP;   /* underlying Lattice                               */
    int        mode;   /* rescoring mode                                   */
    int        what;   /* constrain svx (what=0) or lvx (what=1)           */
    LatLMList  list;
    int        startX;
    int        deadX;
    GNode**    tmpA;
    BMem*      buffer;

    int        padX;   /* padding for time-constraint rescoring            */
    int        frameX; /* frame index to process                           */
    lmScore*   validA; /* define whether a word is valid in current frame  */
  } latlm;

  struct {             /* ---- Cache structure for leafs and nodes: ------ */
    int       N;       /* The number of cache lines                        */
    LCT*      lctA;    /* Array of stored LCTs                             */
    LCT*      nctA;    /* Array of stored node LCTs                        */
    int*      useA;    /* Age-array of stored LCTs                         */
    int       hits;    /* Number of hits in this cache                     */
    int       queries; /* Times we were called                              */
    int       lastX;   /* Last index queried                               */
    lmScore** scorePA; /* The cache itself                                 */
  } cache;

  struct {             /* ---- Extra cache for nodes in the LA-tree: ----- */
    int       N;       /* The number of cache lines                        */
    LCT*      nctA;    /* Array of stored reduced LCTs                     */
    int*      useA;    /* Age-array of stored LCTs                         */
    int       hits;    /* Number of hits in this cache                     */
    lmScore** scorePA; /* The cache itself                                 */
  } nodeCache;

  struct {             /* ---- lm lookahead for extended RCM pruning: ---- */
    int      phoneN;   /* number phones for extended rcm lookahead         */
    int      cacheN;   /* number of cachelines                             */
    int      queries;  /* rcm la queries                                   */
    CTX*     w2p;      /* map word to basephone                            */
    LCT*     lctA;     /* lct for each cache line                          */
    int*     useA;     /* active counter for aging cache                   */
    lmScore* scoreA;   /* float score array for each cache line            */
  } lmrcm;

};


/* =======================================================================
     Functions Prototypes
   ======================================================================= */

extern LCT        CreateLCT          (LMLA* ltreeP,          SVX  w);
extern LCT        ExtendLCT          (LMLA* ltreeP, LCT lct, SVX  w);
extern LCT        ReduceLCT          (LMLA* ltreeP, LCT lct,          int    n);
extern int        DecodeLCT          (LMLA* ltreeP, LCT lct, LVX* w,  int    n);
extern int        FreeLCT            (LMLA* ltreeP, LCT lct);

extern lmScore    GetOneLMScore      (LMLA* ltreeP, LCT lct, SVX svX);

extern int        lmlaFillCtx        (LMLA *lmlaP, LCT lct, 
				      lmScore **lnX, lmScore **svX);
extern lmScore*   lmlaPhones         (LMLA *lmlaP, LCT lct);
extern int        lmlaReinit         (LMLA* lmlaP);

extern int        lmlaTimeConstraint_SVX (LMLA* lmlaP, int frameX, SVX svX);
extern int        lmlaTimeConstraint_LNX (LMLA* lmlaP, int frameX, LVX lnX);


extern int        LMLA_Init          (void);


#endif
