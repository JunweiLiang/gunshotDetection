/* ========================================================================
    JANUS      Speech- to Speech Translation
               ------------------------------------------------------------
               Object: SPASS lattice
               ------------------------------------------------------------

    Author  :  Hagen Soltau & Florian Metze
    Module  :  glat.h
    Date    :  $Id: glat.h 3108 2010-01-30 20:41:26Z metze $
    Remarks : 

   ========================================================================

    This software was developed by

    the  University of Karlsruhe       and  Carnegie Mellon University
         Dept. of Informatics               Dept. of Computer Science
         ILKD, Lehrstuhl Prof.Waibel        Alex Waibel's NN & Speech Group
         Am Fasanengarten 5                 5000 Forbes Ave
         D-76131 Karlsruhe                  Pittsburgh, PA 15213
         - Germany -                        - USA -

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
   Revision 4.3  2007/01/09 11:07:25  fuegen
   - bugfixes for alpha/ beta computation which influences the gamma computation
     and also the connect
     To be still compatible with old scripts a factor of 4.5 is applied to the
     alpha lattice beam during connect and prune (-factor).
     For confusion network combination in comparison with old results, slightly
     better results could be achieved, when reducing the postScale from 2.0 to 1.8.
   - removed the MPE code

   Revision 4.2  2006/11/10 10:15:58  fuegen
   merged changes from branch jtk-05-02-02-shajith
   - all modification related to MMIE training
   - first unverified functions for MPE training
   - modifications made by Shajith, Roger, Wilson, and Sebastian

   Revision 4.1.12.1  2006/11/03 12:41:29  stueker
   Initial check-in of MMIE from Shajith, Wilson, and Roger.

   Revision 4.1  2003/08/14 11:20:02  fuegen
   Merged changes on branch jtk-01-01-15-fms (jaguar -> ibis-013)

   Revision 1.1.2.44  2003/06/06 08:05:25  fuegen
   made gcc 3.3 clean

   Revision 1.1.2.43  2003/03/11 16:37:23  metze
   Cacheing for MetaLM, Map for GLat computeLCT

   Revision 1.1.2.42  2003/02/18 18:18:15  metze
   Final (?) version of Consensus

   Revision 1.1.2.41  2003/01/07 14:59:36  metze
   Consensus changes, added frameShift

   Revision 1.1.2.40  2002/12/13 10:51:46  metze
   Consensus similar to Lidia's default case

   Revision 1.1.2.39  2002/12/01 13:50:57  metze
   Added glatConsensus

   Revision 1.1.2.38  2002/11/13 10:09:59  soltau
   GLat base now SVMap directly

   Revision 1.1.2.37  2002/06/18 16:54:03  metze
   Fixed inconsistencies in map.x2n handling
   Replaced "nodeN" by "nodeM and checks" in loops where neccessary
   Cleaned up lattice status and added it to lattice files

   Revision 1.1.2.36  2002/06/10 09:52:22  soltau
   GKey             : added svX to support compresses search networks
   glatComputeGamma : changed arguments

   Revision 1.1.2.35  2002/04/25 17:25:44  soltau
   Renamed Create/Free Node/Link functions

   Revision 1.1.2.34  2002/04/08 11:54:22  metze
   Changes in 'recombine'

   Revision 1.1.2.33  2002/03/14 09:08:41  metze
   Made 'recombine' much faster by using glatSortNodes

   Revision 1.1.2.32  2002/03/07 10:22:43  metze
   Introduced -singularLCT (purify still has problems with this)

   Revision 1.1.2.31  2002/02/27 09:26:13  metze
   Changed interface to glatComputeGamma

   Revision 1.1.2.30  2002/02/24 09:31:28  metze
   Rewrite of Lattice LM: now works with Hash Fct
   llnP pointer no longer needed

   Revision 1.1.2.29  2002/02/13 13:47:45  soltau
   included svmap

   Revision 1.1.2.28  2002/02/11 13:50:59  soltau
   optimized rcm memory management

   Revision 1.1.2.27  2002/02/01 14:17:20  fuegen
   moved stypes.h into slimits.h

   Revision 1.1.2.26  2002/01/27 17:48:30  soltau
   support for sdphones

   Revision 1.1.2.25  2001/12/21 16:31:31  soltau
   glatComputeGamma: Added sum/max mode

   Revision 1.1.2.24  2001/12/06 14:18:52  soltau
   changed data type for scale

   Revision 1.1.2.23  2001/10/11 16:57:18  soltau
   Added 'Flat_Constraint' Mode

   Revision 1.1.2.22  2001/10/10 18:40:44  soltau
   Added confidence measure

   Revision 1.1.2.21  2001/10/09 18:32:11  soltau
   Added Hashkey for linguistic constraint

   Revision 1.1.2.20  2001/10/03 13:55:55  soltau
   Made glatCount public

   Revision 1.1.2.19  2001/10/02 15:22:15  soltau
   Made gnodeDelete public

   Revision 1.1.2.18  2001/10/01 10:44:55  soltau
   Use polymorphism defines from slimits

   Revision 1.1.2.17  2001/09/24 19:29:13  soltau
   Added Link to LatLmNode

   Revision 1.1.2.16  2001/09/21 14:48:18  soltau
   Removed gammaBeam

   Revision 1.1.2.15  2001/09/19 17:59:12  soltau
   Redesigned a-posteriori computation

   Revision 1.1.2.14  2001/07/26 09:31:10  metze
   Better memory management for glatRescore

   Revision 1.1.2.13  2001/07/25 11:35:32  soltau
   Added expert mode

   Revision 1.1.2.12  2001/07/11 15:10:09  metze
   Added language model rescoring

   Revision 1.1.2.11  2001/06/22 14:48:13  soltau
   Added memory management

   Revision 1.1.2.10  2001/06/20 17:51:45  soltau
   Added G'Lat beams

   Revision 1.1.2.9  2001/06/14 16:43:38  soltau
   redesigned lattice pruning

   Revision 1.1.2.8  2001/06/11 14:20:34  soltau
   Added forward and backward probabilities in lattice nodes
   Removed alignment array

   Revision 1.1.2.7  2001/06/06 10:57:29  soltau
   *** empty log message ***

   Revision 1.1.2.6  2001/06/05 19:20:09  soltau
   Added lattice alignment

   Revision 1.1.2.5  2001/06/05 10:11:53  soltau
   Added final lattice node
   Added sorter

   Revision 1.1.2.4  2001/06/01 10:36:34  soltau
   Added backward links

   Revision 1.1.2.3  2001/05/29 18:36:49  soltau
   Added a couple of glatAdd* functions

   Revision 1.1.2.2  2001/05/28 18:53:57  soltau
   *** empty log message ***

   Revision 1.1.2.1  2001/05/05 18:31:28  soltau
   initial revision


   ======================================================================== */

#ifndef _glat
#define _glat

#include "slimits.h"
#include "sglobal.h"
#include "svmap.h"
#include "bmem.h"
#include "list.h"
#include "path.h"

/* ------------------------------------------------------------------------
    flags for nodeP->status to apply DFS for word graphs
   ------------------------------------------------------------------------ */

enum { 
  DFS_INIT      = 0, /* node, inital state                                   */
  DFS_PURE_DEL  = 1, /* purified node, which do not terminate                */
  DFS_PURE_OK   = 2, /* purified node, where there is a path to a final node */
  DFS_CLEAR     = 3, /* node, which is already traversed by DeleteSubtree    */
  DFS_COUNT     = 4, /* node, which is already traversed by count            */
  DFS_PUTS      = 5, /* node, which is already traversed by puts             */
  DFS_ALPHA     = 6, /* node, which is already traversed by forward pass     */
  DFS_BETA      = 7, /* node, which is already traversed by backward pass    */
  DFS_LCT       = 8  /* node, which is already traversed by computing LCT    */
};

enum {
  LAT_INIT      = 0, /* lattice, inital state                                */
  LAT_CREATE    = 1, /* lattice, during generation/loading                   */
  LAT_PURE      = 2  /* lattice, purified                                    */
};

/* ===========================================================================
 *    Structure containing temporary variables to be used during the MPE training
 *       =========================================================================== */

typedef struct GLatMPEComp_S{
  int monoPhoneN;              /* Number of monophones that make up the word node */
  int *phone;                  /* list of phone indices */
  int *startFrame;             /* list of start frame indices of all the phones */
  int *endFrame;               /* list of end frame indices of all the phones */
  float *approxPhoneAcc;       /* list of local approx. phone accuracies */
  float approxWordPhoneAcc;    /* Word level approx. phone acc. */
  float approxMPEAlpha;        /* Word level MPE alpha */
  float approxMPEBeta;         /* Word level MPE beta */

} GLatMPEComp;

typedef struct GLatMPECDStats_S{
  double *approxCq;             /* list of approx. c(q) values for all the phones */
  double *approxMPEGamma;       /* list of approx. MPE gamma values for all the phones */
  double approxMPEAlpha;        /* Word level MPE alpha */
  double approxMPEBeta;         /* Word level MPE beta */
  Path* path;
} GLatMPECDStats;

/* ========================================================================
    Lattice Node
   ======================================================================== */

struct GNode_S {
  SVX     svX;       /* Search Word Index                              */
  int     nodeX;     /* node id                                        */
  float   score;     /* acoustic score w.r.t to the best right context */
  float   alpha;     /* forward probabilities                          */
  float   beta;      /* backward probabilities (sum of)                */
  float   gamma;     /* a posteriori's (sometimes misused ...)         */
  int     frameS;    /* start frame for this word                      */
  int     frameE;    /* end   frame for this word                      */
  GLink*  childP;    /* link to child node                             */
  GLink*  parentP;   /* link to parent node                            */

  struct {
    unsigned char isRefNode;
    int parentCtxN;
    int childCtxN;
    GNode **parentP;
    GNode **childP;
    float **score;
    float **alpha;
    float **beta;
    float **gamma;
    float *lmScore;
    GLatMPEComp *MPEComp; /* For use in MPE training */
    GLatMPECDStats ***MPECDStats; /* For use in MPE training */
  } addon;

  struct {
    float*  A;       /* array of rcm scores                            */
    CTX*    X;       /* array of rcm indices                           */
    CTX     N;       /* number of scores                               */
  } rcm;

  UCHAR   status;    /* internal tag                                   */
  struct {           /* array of corresponding linguistic states       */
    USHORT      N;
    LCT*        A;
    float* alphaA;   /* the alpha depends on the LCT                   */
    float*  betaA;   /* the beta  depends on the LCT                   */
  } lct;

};


/* ========================================================================
    Lattice Link
   ======================================================================== */

struct GLink_S {
  float   score;    /* acoustic delta score for a specific right model  */
  GNode*  childP;   /* link to the next word                            */
  GNode*  parentP;  /* link to the previous word                        */
  GLink*  nextP;    /* pointer to the next link from parent             */
  GLink*  txenP;    /* pointer to the next link from child              */
  USHORT* map;
};


/* ========================================================================
    Hashkey for lattice node
   ======================================================================== */

typedef struct GKey_S {
  SVX     svX;   
  LPM     morphX;
  GNode*  nodeP;
} GKey;

typedef struct LIST(GKey) GKeyList;

typedef struct GItem_S {
  SVX     svX;
  int     frameX; 
  CTX     phoneX;
  GNode*  nodeP;
} GItem;


/* ========================================================================
    Hashkey for linguistic constraint
   ======================================================================== */

typedef struct LCon_S {
  LCT     lct;
  SVX     svN;
  SVX     svM;
  SVX*    svXA;
} LCon;

typedef struct LIST(LCon) LConList;


/* ========================================================================
    Item for Lattice alignment
   ======================================================================== */

typedef struct GBP_S {
  float   score;                /* costs for aligment                    */
  int     hypX;                 /* backpointer : previous hypo. node     */ 
  int     refX;                 /* backpointer : previous ref. node      */ 
} GBP;


/* ========================================================================
    Lattice
   ======================================================================== */

struct GLat_S {
  char*        name;
  int          useN;

  SVMap*       svmapP;          /* search vocabulary mapper              */
  SPass*       spassP;          /* search pass decoder                   */
  GNode*       startP;          /* begin of sentence node                */  
  GNode*       endP;            /* end of sentence node                  */  

  int          nodeN;           /* number of nodes                       */
  int          nodeM;           /* max. node index                       */
  int          linkN;           /* number of links                       */
  UCHAR        status;          /* lattice status                        */
  int          expert;          /* expert mode                           */
  int          singularLCT;     /* each node contains exactly one LCT    */
  float        frameShift;      /* frame shift, 0.01 = 10ms              */

  float approxGLatPhoneAcc;   /* lattice level approx. phone acc. in MPE training */
  int mpeFlag;

  struct {                   
    int        frameX;          /* final frame                           */
    LVX        lvX;             /* final node                            */
  } end;

  struct {                      /* mapper for lattice nodes              */
    GNode**          b2n;       /* bpIdx           -> gnode              */
    GKeyList         s2n;       /* (svX,morphX)    -> gnode              */
    GItem p2n[LPM_MAX];         /* (phoneX,frameX) -> gnode              */
    GNode**          x2n;       /* nodeX           -> gnode              */
    int           allocN;       /* x2n allocation info                   */
  } mapping;

  struct {                      /* container for temporary lattice nodes */
    int        nodeN;           /* number of inserted nodes at this frame*/
    int        allocN;          /* size of the nodePA array              */
    GNode**    nodePA;          /* pointer array for new created nodes   */
  } stack;

  struct {                      /* container to connect gnodes           */                   
    int        frameN;          /* allocation size                       */
    int*       startCntA;       /* nr. of starting nodes in this frame   */
    int*       endCntA;         /* nr. of ending   nodes in this frame   */
    int*       startAllocA;   
    int*       endAllocA;         
    GNode***   startPA;
    GNode***   endPA;
  } sorter;

  struct {                      /* lattice alignment                     */
    float      delCost;         /* cost to delete one word in lattice    */
    float      insCost;         /* cost to insert one word in lattice    */
    float      subCost;
  } align;

  LConList     LConL;           /* list of linguistic constraints         */
  lmScore*     lmA;             /* lmA[svX] = (svX in lat)? 0 : nil       */

  struct {                      /* lattice beams                          */
    int        nodeN;           /* nr. of nodes during generation         */
    float      alpha;           /* liklihood threshold during generation  */
  } beam;

  struct {                      /* lattice memory management              */
    BMem*     node;             /* blocks of GNodes                       */
    BMem*     link;             /* blocks of GLinks                       */
    BMem*     rcm;              /* block of rcm scores                    */
    BMem*     idx;              /* block of rcm indices                   */
    BMem*     paths;            /* paths defined in this lattice          */
    BMem*     tables;           /* tables for partial paths               */
    BMem*     lct;              /* blocks of linguistic states            */
    
    int       rcmNodeN;
    int       rcmHmmN;
    float**   rcmAA;

  } mem;

};


/* ========================================================================
    Lattice Path (for rescoring)
   ======================================================================== */

typedef struct GLATPATHITEM_S GLatPathItem;

struct GLATPATHITEM_S {
  GLatPathItem* fromP;
  GNode*        nodeP;
  float         score;
  int           n;
  LCT           lct;
};


/* ========================================================================
    Module Interface
   ======================================================================== */

extern int   GLat_Init       (void);
extern GLat* glatCreate      (char* name, SVMap* svmapP, SPass* spassP);
extern int   glatFree        (GLat* latP);
extern int   glatClear       (GLat* latP);

/* spass interface functions */
extern int   glatAdd_SLeaf   (GLat* latP);
extern int   glatAdd_SIPhone (GLat* latP);
extern int   glatAdd_SDPhone (GLat* latP);
extern int   glatMapBP       (GLat* latP);
extern int   glatCut         (GLat* latP);

/* generation interface functions */
extern int    glatCreateNode     (GLat* latP, SVX svX, int fromX, int toX);
extern GLink* glatCreateLink     (GLat* latP, GNode* fromP, GNode* toP, float score);
extern int    glatFreeNode       (GLat* latP, GNode* nodeP);
extern int    glatFreeLink       (GLat* latP, GLink* linkP);

extern int    glatAssureMapTable (GLat* latP, int N);
extern int    glatFinalize       (GLat* latP);
extern int    glatPurify         (GLat* latP);
extern int    glatSortNodes      (GLat* latP);
extern void   glatCount          (GLat* latP);
extern int    glatLeftContextOk  (GLat* latP, GNode* fromP, GNode* toP);
extern float  glatGetRightContext(GLat* latP, GNode* fromP, GNode* toP); 
extern double addLogProbs        (double p1, double p2);
extern int    glatPrune          (GLat* latP, SVMap* svmapP, float beam, 
				  int nodeN,int link,double scale,int prob_sum);
extern float  glatComputeGamma   (GLat* latP, SVMap* svmapP, int prob_sum, 
				  double scale, int modus, int tieFiller);
extern int    glatConnectItf     (ClientData cd, int argc, char *argv[]);
extern int    glatPurifyItf      (ClientData cd, int argc, char *argv[]);
extern int    glatPruneItf       (ClientData cd, int argc, char *argv[]);

/* ltree interface functions */
int  glatExtractConstraint(GLat* latP, SVMap* svmapP, int mode);
void glatFillCtx          (GLat* latP, LCT lct, lmScore* svA, int mode);

/* consensus functions */
extern int    glatConsensusItf   (ClientData cd, int argc, char *argv[]);

#endif
