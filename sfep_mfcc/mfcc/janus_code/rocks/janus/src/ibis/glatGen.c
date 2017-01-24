/* ========================================================================
    JANUS      Speech- to Speech Translation
               ------------------------------------------------------------
               Object: SPASS lattice generation and pruning
               ------------------------------------------------------------

    Author  :  Hagen Soltau & Florian Metze
    Module  :  glatGen.c
    Date    :  $Id: glatGen.c 3416 2011-03-23 03:02:18Z metze $

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
    Revision 4.5  2007/02/22 16:27:59  fuegen
    removed -pedantic compiler warnings with

    Revision 4.4  2007/01/09 11:07:26  fuegen
    - bugfixes for alpha/ beta computation which influences the gamma computation
      and also the connect
      To be still compatible with old scripts a factor of 4.5 is applied to the
      alpha lattice beam during connect and prune (-factor).
      For confusion network combination in comparison with old results, slightly
      better results could be achieved, when reducing the postScale from 2.0 to 1.8.
    - removed the MPE code

    Revision 4.3  2005/11/08 13:11:43  metze
    Fixes for connect and alpha/beta computation

    Revision 4.2  2005/03/04 09:21:16  metze
    Bugfix for sorter, added 'dictWords' option

    Revision 4.1  2003/08/14 11:20:02  fuegen
    Merged changes on branch jtk-01-01-15-fms (jaguar -> ibis-013)

    Revision 1.1.2.65  2003/05/08 16:41:52  soltau
    Purified!

    Revision 1.1.2.64  2003/04/08 10:01:53  metze
    Cleaner splitMW code

    Revision 1.1.2.63  2003/04/07 18:05:44  soltau
    glatCount : last famous SWB fixes

    Revision 1.1.2.62  2003/04/01 08:53:11  metze
    Consensus changes

    Revision 1.1.2.61  2003/02/18 18:18:15  metze
    Final (?) version of Consensus

    Revision 1.1.2.60  2003/01/28 16:20:35  metze
    Consensus with BitField code

    Revision 1.1.2.59  2003/01/24 14:21:38  soltau
    fixed glatRepair

    Revision 1.1.2.57  2003/01/22 17:08:06  soltau
    Fixed generation of filler nodes for sleaf instances

    Revision 1.1.2.56  2003/01/22 16:28:01  soltau
    Fixed generation of filler nodes

    Revision 1.1.2.55  2003/01/07 14:59:37  metze
    Consensus changes, added frameShift

    Revision 1.1.2.54  2002/12/13 10:51:46  metze
    Consensus similar to Lidia's default case

    Revision 1.1.2.53  2002/11/21 17:10:10  fuegen
    windows code cleaning

    Revision 1.1.2.52  2002/11/13 10:15:32  soltau
    - GLat base now SVMap directly
    - Start and End words rely on LVX now
    - Changes for optional words
    - single phone are allowed to have dynamic LVX now

    Revision 1.1.2.51  2002/07/25 07:26:54  soltau
    glatPrune: check number of nodes during pruning

    Revision 1.1.2.50  2002/07/18 12:08:16  soltau
    glatPrune: check for empty links
    GlatAdd_ : changes according optimized stree structures
               bestscore -> bestX, worstscore -> worstX

    Revision 1.1.2.49  2002/06/26 11:57:54  fuegen
    changes for new directory structure support and code cleaning

    Revision 1.1.2.48  2002/06/25 07:28:00  metze
    Made glatCount backwards depth-first instead of forward breadth-first (oder so)

    Revision 1.1.2.47  2002/06/18 16:53:56  metze
    Fixed inconsistencies in mapping.x2n handling
    Replaced "nodeN" by "nodeM and checks" in loops where neccessary
    Cleaned up lattice status and added it to lattice files

    Revision 1.1.2.46  2002/06/13 08:20:57  soltau
    call svmapClearCache after createLCT calls

    Revision 1.1.2.45  2002/06/12 14:39:18  soltau
    glatRepair : improved handling of lct's

    Revision 1.1.2.44  2002/06/10 09:57:20  soltau
    - glatAdd_* routines
      checking svX while processing list of instances now
      reduced precision for local acoustic scores
    - Changes according to modified glatCompputeGamma

    Revision 1.1.2.43  2002/05/02 14:36:09  soltau
    glatConnect: optimized to handle complex lks score calls more efficiently
                 support for alternative svmap's

    Revision 1.1.2.42  2002/05/02 13:02:23  metze
    Check for SPass in ContextOk

    Revision 1.1.2.41  2002/04/29 13:29:35  metze
    Redesign of LM interface (LingKS)

    Revision 1.1.2.40  2002/04/25 17:26:14  soltau
    Renamed Create/Free Node/Link functions

    Revision 1.1.2.39  2002/04/11 09:37:43  soltau
    Removed defines BMEM_FREE, RCM_OPT, use it always

    Revision 1.1.2.38  2002/04/10 15:02:49  soltau
    Removed assertion to check negative acoustic scores

    Revision 1.1.2.37  2002/04/08 11:54:25  metze
    Changes in 'recombine'

    Revision 1.1.2.36  2002/03/14 09:08:21  metze
    Made 'recombine' much faster by using glatSortNodes

    Revision 1.1.2.35  2002/03/13 16:04:22  metze
    Fixes in line with splitMW and recombine

    Revision 1.1.2.34  2002/03/07 10:22:24  metze
    Introduced -singularLCT (purify still has problems with this)

    Revision 1.1.2.33  2002/02/27 09:26:26  metze
    Changed interface to glatComputeGamma

    Revision 1.1.2.32  2002/02/24 09:31:51  metze
    Rewrite of Lattice LM: now works with Hash Fct
    llnP no longer needed

    Revision 1.1.2.31  2002/02/18 15:44:28  soltau
    glatRead: support for non-purified lattices

    Revision 1.1.2.30  2002/02/15 17:13:04  soltau
    call svmapClearCache

    Revision 1.1.2.29  2002/02/13 13:47:08  soltau
    Use svmapGetLMscore instead of calling lksP->scoreFct directly

    Revision 1.1.2.28  2002/02/11 19:52:36  soltau
    glatRepair: take care about svmap->lvxSA scores

    Revision 1.1.2.27  2002/02/11 13:51:43  soltau
    optimized rcm memory management

    Revision 1.1.2.26  2002/02/07 12:11:13  metze
    Changes in interface to LM score functions

    Revision 1.1.2.25  2002/01/30 13:22:32  soltau
    *** empty log message ***

    Revision 1.1.2.24  2002/01/29 17:33:55  soltau
    Sanity check in glatGen for threshold

    Revision 1.1.2.23  2002/01/29 10:42:16  soltau
    Fixed SDPhone handling

    Revision 1.1.2.22  2002/01/28 18:50:47  soltau
    Fixed a couple of sdphone problems

    Revision 1.1.2.21  2002/01/27 17:48:14  soltau
    support for sdphons

    Revision 1.1.2.20  2002/01/26 14:47:24  soltau
    Fixed initilization problems in glatFinalize and glatPurify

    Revision 1.1.2.19  2002/01/10 13:37:04  soltau
    Changes according to the decoding along shared suffixes

    Revision 1.1.2.18  2002/01/08 11:26:16  soltau
    glatRepair: Call correct lct manipulation functions

    Revision 1.1.2.17  2001/12/21 16:44:59  soltau
    *** empty log message ***

    Revision 1.1.2.16  2001/12/21 16:35:52  soltau
    glatConnect: Use max mode

    Revision 1.1.2.15  2001/12/21 16:30:24  soltau
    glatAdd_  : take care about same nodes with different scores
    glatPrune : added nodeN option, prune according to max instead of sum

    Revision 1.1.2.14  2001/12/21 11:52:18  soltau
    Added GLAT_EXACT option to not share lattice nodes even with the same phonetic context

    Revision 1.1.2.13  2001/12/17 09:45:10  soltau
    Beautified

    Revision 1.1.2.12  2001/12/07 16:05:32  soltau
    *** empty log message ***

    Revision 1.1.2.11  2001/10/22 08:39:09  metze
    Changed interface to score functions

    Revision 1.1.2.10  2001/10/10 18:41:06  soltau
    Support for confidence measure

    Revision 1.1.2.9  2001/10/03 13:56:18  soltau
    Maed glatCount public

    Revision 1.1.2.8  2001/10/02 15:22:45  soltau
    Made gnodeDelete public

    Revision 1.1.2.7  2001/09/25 17:51:55  metze
    Alpha and Betas are initialized

    Revision 1.1.2.6  2001/09/24 19:37:25  soltau
    Added Link to LatLmNode

    Revision 1.1.2.5  2001/09/21 14:37:47  soltau
    Improved handling of left context in glatConnect
    Changed interface in glatConnect, glatPrune

    Revision 1.1.2.4  2001/09/20 20:22:21  soltau
    Well, it' late and I'm not sure what kind of problems I've fixed.

    Revision 1.1.2.3  2001/09/20 10:37:14  soltau
    Changed handling of language models in latConnect and latPrune for links

    Revision 1.1.2.2  2001/09/19 17:58:47  soltau
    Redesigned a-poseriories

    Revision 1.1.2.1  2001/09/18 18:43:43  soltau
    Initial version (separated from glat.c)


   ======================================================================== */

char  glatGenRcsVersion[]=
    "$Id: glatGen.c 3416 2011-03-23 03:02:18Z metze $";

#include "glat.h"
#include "smem.h"
#include "stree.h"
#include "spass.h"


/* ========================================================================
    GLAT basic function to create and delete nodes / links
   ======================================================================== */

/* ------------------------------------------------------------------------
    glatAssureMapTable : nodeX -> nodeP
   ------------------------------------------------------------------------ */

int glatAssureMapTable (GLat* latP, int N)
{
  if (latP->mapping.allocN <= N) {
    int nodeY = latP->mapping.allocN;

    latP->mapping.allocN += 1000;

    if (latP->mapping.allocN < N)
      latP->mapping.allocN = N;

    if (latP->mapping.x2n)
      latP->mapping.x2n -=2;
    
    if (!(latP->mapping.x2n = realloc (latP->mapping.x2n, (latP->mapping.allocN+2)*sizeof(GNode*)))) {
      ERROR ("glatAssureMapTable: allocation failure.\n");
      return TCL_ERROR;
    }
    latP->mapping.x2n +=2;
    
    latP->mapping.x2n[-1] = latP->startP;
    latP->mapping.x2n[-2] = latP->endP;
    
    for (; nodeY < latP->mapping.allocN; nodeY++)
      latP->mapping.x2n[nodeY] = NULL;
    
  }
  return TCL_OK;
}

/* ------------------------------------------------------------------------
    glatCreateNode
   ------------------------------------------------------------------------ */

int glatCreateNode (GLat* latP, SVX svX, int fromX, int toX)
{
  GNode* nodeP;
  int    nodeX = latP->nodeN;

  glatAssureMapTable (latP, ++latP->nodeN);
  if (latP->mapping.x2n[nodeX])
    for (nodeX = 0; nodeX < latP->mapping.allocN && latP->mapping.x2n[nodeX]; nodeX++);
  if (latP->nodeM <= nodeX) latP->nodeM = nodeX+1;
  nodeP = latP->mapping.x2n[nodeX] = (GNode*) bmemItem(latP->mem.node);

  nodeP->nodeX   = nodeX;
  nodeP->svX     = svX;
  nodeP->frameS  = fromX;
  nodeP->frameE  = toX;

  nodeP->childP  = NULL;
  nodeP->parentP = NULL;
  nodeP->rcm.A   = NULL;
  nodeP->rcm.N   = CTX_MAX;
  nodeP->status  = DFS_INIT;

  return nodeX;
}

/* ------------------------------------------------------------------------
    delete lattice nodes
   ------------------------------------------------------------------------ */

int glatFreeNode(GLat* latP, GNode* nodeP)
{
  GLink* linkP = nodeP->parentP;

  if (nodeP->rcm.A) {
    bmemDealloc(latP->mem.rcm,nodeP->rcm.A,nodeP->rcm.N*sizeof(float));
  }
  if (nodeP->rcm.X) {
    bmemDealloc(latP->mem.idx,nodeP->rcm.X,nodeP->rcm.N*sizeof(CTX));
  }

  /* ------------------------------------- */
  /* delete all links to this node         */
  /* ------------------------------------- */
  while (linkP) {
    GNode* parentP = linkP->parentP;
    GLink*  plinkP = linkP->txenP;
    
    /* remove link (parent -> node) in parent's list of child links */
    if (parentP->childP == linkP ) {
      parentP->childP = linkP->nextP;
    } else {
      GLink* nlinkP = parentP->childP;
      assert(nlinkP);
      while (nlinkP->nextP && nlinkP->nextP != linkP) {
	nlinkP = nlinkP->nextP;
      }
      assert (nlinkP->nextP == linkP);
      nlinkP->nextP = nlinkP->nextP->nextP;
    }
    latP->linkN--;
    bmemDelete(latP->mem.link,linkP);

    /* next link to this node */
    linkP = plinkP;
  }
  
  /* ------------------------------------- */
  /* delete all links from this node       */
  /* ------------------------------------- */

  linkP= nodeP->childP;
  while (linkP) {
    GLink* nlinkP = linkP->nextP;
    GNode* childP = linkP->childP;
    
    /* remove link (node <- child) in child's list of parent links */
    if (childP->parentP == linkP ) {
      childP->parentP = linkP->txenP;
    } else {
      GLink* nlinkP = childP->parentP;
      assert(nlinkP);
      while (nlinkP->txenP && nlinkP->txenP != linkP) {
	nlinkP = nlinkP->txenP;
      }
      assert (nlinkP->txenP == linkP);
      nlinkP->txenP = nlinkP->txenP->txenP;
    }
    latP->linkN--;
    bmemDelete(latP->mem.link,linkP);
    linkP = nlinkP;
  }
  
  /* ------------------------------------- */
  /* delete node                           */
  /* ------------------------------------- */

  nodeP->frameS  = 0;
  nodeP->frameE  = 0;
  nodeP->childP  = NULL;
  nodeP->parentP = NULL;

  /* latP->mapping.x2n[nodeP->nodeX] = NULL; */

  latP->nodeN--;
  bmemDelete(latP->mem.node,nodeP);

  return TCL_OK;
}

/* ------------------------------------------------------------------------
    create a link for a new lattice node
   ------------------------------------------------------------------------ */

/* return  NULL if there is already a link to that node, 
   otherwise return new created object
*/

GLink* glatCreateLink (GLat* latP,GNode* fromP, GNode* toP, float score)
{
  GLink* linkP = fromP->childP;
 
  /* check if we have the link already */
  while (linkP && linkP->childP != toP) {
    linkP = linkP->nextP;
  }
  if (linkP) {
    return NULL;
  }

  linkP= (GLink*) bmemItem(latP->mem.link);
  latP->linkN++;

  linkP->parentP = fromP;
  linkP->childP  = toP;
  linkP->score   = score;
  linkP->nextP   = fromP->childP;
  linkP->txenP   = toP->parentP;

  fromP->childP  = linkP;
  toP->parentP   = linkP;

  return linkP;
}

/* ------------------------------------------------------------------------
    glatFreeLink
   ------------------------------------------------------------------------ */

int glatFreeLink (GLat* latP, GLink* ourP)
{
  GLink* linkP;
  
  /* Unlink 'from' node */
  if ((linkP = ourP->parentP->childP) == ourP)
    ourP->parentP->childP = ourP->nextP;
  else {
    while (linkP->nextP != ourP)
      linkP = linkP->nextP;
    linkP->nextP = ourP->nextP;
  }

  /* Unlink 'to' node */
  if ((linkP = ourP->childP->parentP) == ourP)
    ourP->childP->parentP = ourP->txenP;
  else {
    while (linkP->txenP != ourP)
      linkP = linkP->txenP;
    linkP->txenP = ourP->txenP;
  }

  bmemDelete(latP->mem.link,ourP);

  latP->linkN--;
  return TCL_OK;
}

/* ========================================================================
    GLAT generation from SPASS
   ======================================================================== */

/* ------------------------------------------------------------------------
    word transactions from leafs to lattice nodes
   ------------------------------------------------------------------------ */

static int glatAllocRcm(GLat* latP, int nodeN, int hmmN)
{
  int i;
  if (latP->mem.rcmNodeN >= nodeN && latP->mem.rcmHmmN >= hmmN) {
    return TCL_OK;
  }
  nodeN += 10;
  hmmN  += 10;
  latP->mem.rcmNodeN = nodeN;
  latP->mem.rcmHmmN  = hmmN;

  if (latP->mem.rcmAA) {
    if (latP->mem.rcmAA[0]) free(latP->mem.rcmAA[0]);
    free(latP->mem.rcmAA);
  }
  if (NULL ==(latP->mem.rcmAA=malloc(nodeN*sizeof(float*)))) {
    ERROR("glatAllocRcm: allocation failure\n");
    return TCL_ERROR;
  }
  if (NULL ==(latP->mem.rcmAA[0]=malloc(nodeN*hmmN*sizeof(float)))) {
    ERROR("glatAllocRcm: allocation failure\n");
    return TCL_ERROR;
  }
  for (i=1;i<nodeN;i++)
    latP->mem.rcmAA[i]=latP->mem.rcmAA[i-1]+hmmN;

  return TCL_OK;
}

int glatAdd_SLeaf (GLat* latP)
{
  SPass*     spassP;
  SVMap*     svmapP;
  int         leafX;
  int         leafN;
  SNode**    leafPA;
  BP*           bpA;
  DictWord*  dwordP;
  float      thresh;

  GNode *newP,  *oldP;
  GLink *linkP, *nlinkP;

  int morphX,hmmX,phoneX,phoneY,nodeX,frameX,idx;
  SVX svY;
  LVX endLVX;

  if (! latP->spassP) {
    ERROR("glatAdd_SLeaf: couldn't find decoder (aetsch)\n");
  }

  spassP = latP->spassP;
  svmapP = latP->svmapP;
  leafX  = 0;
  leafN  = spassP->active.leafN;
  leafPA = spassP->active.leafPA;
  bpA    = spassP->stabP->A;
  dwordP = NULL;
  thresh = spassP->beam.best + latP->beam.alpha;
  endLVX = latP->svmapP->idx.end;
  latP->status = LAT_CREATE;

  for (leafX=0;leafX<leafN;leafX++) {
    SNode* sleafP = leafPA[leafX];
    int    morphN = sleafP->morphN;
    RCM*     rcmP = spassP->rcmP + sleafP->X.xhmm;
    PHMM*    hmmP = spassP->hmmP + rcmP->hmmXA[0];
    int    stateN = hmmP->stateN+1;
    int      hmmN = rcmP->hmmN;
    int     nodeN = 0;

    if (morphN == 0 || sleafP->iceA.l[sleafP->bestX]->score >= thresh) continue;

    glatAllocRcm(latP,morphN,hmmN);

    /* We need a temporary map (phoneX,frameX) -> gnode to recombine linguistic
       instances with the same left phonetic context and start frame. */
    for (morphX=0;morphX<morphN;morphX++) {
      latP->mapping.p2n[morphX].frameX = -1;
    }

    for (morphX=0;morphX<morphN;morphX++) {
      float score    = SCORE_MAX;
      int   bpIdx    = 0;
      NTok*  path    = sleafP->iceA.l[morphX]->path;
      float delta    = sleafP->iceA.l[morphX]->lms + spassP->streeP->svmapP->penalty.word;
      SVX     svX    = sleafP->iceA.l[morphX]->svX;
      int     lvX    = svmapGet(latP->svmapP,svX);
      float rcmdelta = 0.0;
      GKey  gkey;

      if (sleafP->iceA.l[morphX]->score >= thresh && lvX != endLVX) continue;
      for (hmmX=0;hmmX<hmmN;hmmX++) {
	if (path[hmmX*stateN].s < score) {
	  score  = path[hmmX*stateN].s;
	  bpIdx  = path[hmmX*stateN].bpIdx;
	}
      }
      if (score > thresh) continue;

      if (svmapP->svocabP->list.itemA[svX].fillerTag) {
	/* the filler penalty is already included in lms for sleaf instances
	   delta += svmapP->penalty.fil - svmapP->penalty.word;
	*/
      } else if (lvX == svmapP->idx.end)
	delta -= svmapP->penalty.word;

      /* We need the first phone for this word to extract the correct rcm score 
	 from the previous word.
      */
      dwordP = 
	spassP->streeP->dictP->list.itemA +
	spassP->streeP->svocabP->list.itemA[svX].dictX;
      phoneY = dwordP->word.phoneA[0];

      /* Find the last phone from the previous word to get the left phoentic
	 context for this word. The previous word can be found via the bpIdx
	 from the token stored in the final state of the last phone in this 
	 word. The bpIdx should be the same for each rcm, so we can use
	 the bpIdx from the best rcm.
      */
      svY    = 0;
      dwordP = NULL;
      phoneX = 0;

      if (bpIdx >=0) {
	svY = bpA[bpIdx].svX;
	dwordP = 
	  spassP->streeP->dictP->list.itemA +
	  spassP->streeP->svocabP->list.itemA[svY].dictX;
	phoneX = dwordP->word.phoneA[dwordP->word.itemN-1];
      }

      /* Find the start frame for this word */
      frameX = stabGetFrame(spassP->stabP,bpIdx)+1;

      /* get the corresponding lattice node from the previous word */
      if (bpIdx >= 0) {
	oldP = latP->mapping.b2n[bpIdx];
      } else {
	oldP = latP->startP;
      }

      /* If we use a more aggressive pruning strategie for the lattice
	 than for the backpointer table, we will have some bp's without
	 corresponding lattice nodes.
      */
      if (! oldP) continue;

      /* To get the word-only score, we have to subtract the lm score and
	 the accumulated score up to the start from of this word. */
      if (bpIdx >= 0) {
	delta +=  bpA[bpIdx].score;
      }

      /* Now, we have to correct the rcm score from the previous word since 
	 the score from the bp table contain the score over all possible right
	 context models but the accumulated score rely on the particular
	 rcm's for the phonetic context for this word.
      */
      if (bpIdx >= 0) {
	if (spassP->streeP->nodeTbl.typeA[svY] == type_SLeaf) {
	  SNode*  slP = (SNode*) spassP->streeP->nodeTbl.A[svY];
	  RCM*    rmP = spassP->rcmP + slP->X.xhmm;
	  CTX     hmX = rmP->mapXA[phoneY];
	  CTX*   idxA = oldP->rcm.X;
	  float* rcmA = oldP->rcm.A;
	  int     idx = (idxA) ?  oldP->rcm.N-1 : -1;
	  rcmdelta = SCORE_MAX;
	  while (idx>=0 && idxA[idx] != hmX) idx--;
	  if (idx>=0)
	    rcmdelta=rcmA[idx];
	  
	  delta += rcmdelta;
	} else if (spassP->streeP->nodeTbl.typeA[svY] == type_SDPhone) {
	  SDPhone* sdpP = (SDPhone*) spassP->streeP->nodeTbl.A[svY];
	  XCM*     xcmP = spassP->xcmP + sdpP->hmmX;
	  
	  /* now it becomes complicated: I need the left context from
	     the last word (=sdpP) to determine the xcmP map entry.
	     trace back to the parent from the last word and get
	     the last phone from that word
	  */
	  if (oldP->parentP) {
	    /* FF HACK - REORDERED FOR COMPILABILITY WITH VISUAL STUDIO */
	    GNode*   oldoldP = oldP->parentP->parentP;
	    SVX          svZ = oldoldP->svX;
	    rcmdelta = SCORE_MAX;
	    if (svZ != SVX_NIL) {
	      DictWord* mwordP = 
		spassP->streeP->dictP->list.itemA +
		spassP->streeP->svocabP->list.itemA[svZ].dictX;
	      CTX        leftX = mwordP->word.phoneA[mwordP->word.itemN-1];
	      CTX          hmX = xcmP->mapXA[leftX][phoneY];
	      CTX*   idxA = oldP->rcm.X;
	      float* rcmA = oldP->rcm.A;
	      int     idx = (idxA) ?  oldP->rcm.N-1 : -1;

	      while (idx>=0 && idxA[idx] != hmX) idx--;
	      if (idx>=0)
		rcmdelta=rcmA[idx];
	      delta += rcmdelta;
	    } else {
	      break;
	    }
	  } else {
	    if (latP->expert)
	      WARN("glatAdd_SLeaf: cannot get phonetic context for %d\n",svY);
	  }	
	} else if (spassP->streeP->nodeTbl.typeA[svY] != type_SIPhone) {
	  ERROR("glatAdd_SLeaf: cannot handle search node type for %d\n",svY);
	  break;
	}

	/* no phonetic context */
	if (rcmdelta >= SCORE_MAX)
	  continue;
      }

      /* Check if we have already a GNode with the same (phoneX,frameX)
	 Allocate a new node or update rcm scores.
         We generate a new node in any case in 'singularLCT' mode */
      nodeX = (latP->singularLCT) ? nodeN : 0;
      while (nodeX < nodeN && !(latP->mapping.p2n[nodeX].svX    == svX    &&
				latP->mapping.p2n[nodeX].frameX == frameX &&
			        latP->mapping.p2n[nodeX].phoneX == phoneX &&
				fabs((latP->mapping.p2n[nodeX].nodeP->score - score + delta) / (score-delta))<1e-4)) {
	nodeX++;
      }
      if (nodeX < nodeN) {
	newP = latP->mapping.p2n[nodeX].nodeP;
      } else {
	/* update map (svX,frameX,phoneX) -> nodeP */
	latP->mapping.p2n[nodeX].svX    = svX;
	latP->mapping.p2n[nodeX].frameX = frameX;
	latP->mapping.p2n[nodeX].phoneX = phoneX;
	latP->mapping.p2n[nodeX].nodeP  = (GNode*) bmemItem(latP->mem.node);

	/* update map nodeX  -> nodeP */
	glatAssureMapTable (latP, latP->nodeN+1);

	latP->mapping.x2n[latP->nodeN] = latP->mapping.p2n[nodeX].nodeP;

	newP = latP->mapping.p2n[nodeX].nodeP;
	newP->score   = SCORE_MAX;
	newP->gamma   = SCORE_MAX;
	newP->alpha   = 0.0;
	newP->beta    = 0.0;
	newP->frameS  = frameX;
	newP->frameE  = spassP->frameX;
	newP->svX     = svX;
	newP->childP  = NULL;
	newP->parentP = NULL;
	newP->nodeX   = latP->nodeN;
	newP->status  = DFS_INIT;
	newP->rcm.A   = latP->mem.rcmAA[morphX];
	newP->rcm.X   = NULL;
	newP->rcm.N   = hmmN;
	newP->lct.N   = 0;
	newP->lct.A   = NULL;

	latP->nodeN++;
	latP->nodeM++;
	nodeN++;

	for (hmmX=0;hmmX<hmmN;hmmX++) {
	  newP->rcm.A[hmmX]= SCORE_MAX;
	}

	/* store new allocated node on the stack */
	if (latP->stack.nodeN == latP->stack.allocN) {
	  latP->stack.allocN += leafN;
	  if (NULL == (latP->stack.nodePA = 
	       realloc(latP->stack.nodePA,latP->stack.allocN*sizeof(GNode*)))) {
	    ERROR("glatAdd_SLeaf: allocation failure\n");
	    return TCL_ERROR;
	  }
	}
	latP->stack.nodePA[latP->stack.nodeN++] = newP;	
      }
      
      /* we add here a temporary map entry (svX,morphX) -> nodeP since
	 we don't know the bpIdx yet. If a backpointer is created for
	 this instance, we can then define a direct map bpIdx -> nodeP
	 via this temporary mapping.
      */
      gkey.svX    = svX;
      gkey.morphX = morphX;
      gkey.nodeP  = newP;
      idx = listIndex((List*) &(latP->mapping.s2n),(void*) &gkey,1);
      
      /* update acoustic scores 
	 newP->score = word only score
	 newP->gamma = theory score, temporary needed for prune operations
      */
      newP->score = score - delta;
      newP->gamma = score;
      for (hmmX=0;hmmX<hmmN;hmmX++) {
	if (path[hmmX*stateN].s < SCORE_MAX && path[hmmX*stateN].s -score < newP->rcm.A[hmmX]) {
	  newP->rcm.A[hmmX] =  path[hmmX*stateN].s -score;
	}
      }

      /* check if we have already a link from oldP to newP */
      if ((linkP = oldP->childP))
	while (linkP->childP != newP && linkP->nextP) 
	  linkP = linkP->nextP;

      /* add a link from lattice node oldP to newP */
      nlinkP = linkP;
      if (! linkP || linkP->childP != newP) {
	nlinkP = (GLink*) bmemItem(latP->mem.link);
	latP->linkN++;
	
	nlinkP->score   = rcmdelta;
	nlinkP->nextP   = NULL;
	nlinkP->txenP   = NULL;
	nlinkP->childP  = newP;
	nlinkP->parentP = oldP;
	if (! linkP) 
	  oldP->childP = nlinkP;
	else
	  linkP->nextP = nlinkP;
      } 
      
      /* add the nlink=(oldP->newP) to the list of backward-links */
      if (! newP->parentP) {
	newP->parentP = nlinkP;
      } else {
	linkP = newP->parentP;
	while (linkP->parentP != oldP && linkP->txenP) 
          linkP = linkP->txenP;
	if (linkP->parentP != oldP) {
	  linkP->txenP = nlinkP;
	}
      }
    } /* end for each instance */

    /* postprocess rcm scores */
    for (nodeX=0;nodeX<nodeN;nodeX++) {
      CTX*   idxA = NULL;
      float* rcmA = NULL;
      int     cnt = 0;
      newP = latP->mapping.p2n[nodeX].nodeP;

      /* count 'real' scores */
      for (hmmX=0;hmmX<newP->rcm.N;hmmX++) {
        if (newP->rcm.A[hmmX] < SCORE_MAX) cnt++;
      }
      if (cnt == 0) {
        newP->rcm.A = NULL;
	newP->rcm.X = NULL;
        newP->rcm.N = CTX_MAX;
      }
      /* store rcm's more memory efficient */
      rcmA = bmemAlloc(latP->mem.rcm,cnt*(sizeof(float)));
      idxA = bmemAlloc(latP->mem.idx,cnt*(sizeof(CTX)));
      cnt  = 0;
      for (hmmX=0;hmmX<newP->rcm.N;hmmX++) {
        if (newP->rcm.A[hmmX] < SCORE_MAX) {
          idxA[cnt]= hmmX;
          rcmA[cnt]= newP->rcm.A[hmmX];
          cnt++;
        }
      }
      newP->rcm.A = rcmA;
      newP->rcm.X = idxA;
      newP->rcm.N = cnt;
    } /* end of postprocess rcm scores */

  } /* end for each leaf */

  return TCL_OK;
}

/* ------------------------------------------------------------------------
    word transactions from single phone words to lattice nodes
   ------------------------------------------------------------------------ */

int glatAdd_SIPhone (GLat* latP)
{
  SPass*     spassP;
  SVMap*     svmapP;
  int          sipX;
  int          sipN;
  SIPhone*     sipP;
  BP*           bpA;
  float      thresh;

  GNode *newP, *oldP;
  GLink *linkP, *nlinkP; 

  int morphX,phoneX,phoneY,nodeX,frameX,idx;
  SVX svY;

  if (! latP->spassP) {
    ERROR("glatAdd_SIPhone: couldn't find decoder (aetsch)\n");
  }

  latP->status = LAT_CREATE;

  spassP = latP->spassP;
  svmapP = spassP->streeP->svmapP;

  sipX   = 0;
  sipN   = spassP->streeP->sTbl.iN;
  sipP   = spassP->streeP->sTbl.iA;
  bpA    = spassP->stabP->A;
  thresh = spassP->beam.best + latP->beam.alpha;

  for (sipX=0;sipX<sipN;sipX++, sipP++) {
    int    morphN = sipP->morphN;
    SVX       svX = sipP->svX;
    LVX       lvX = svmapGet(svmapP,svX);
    int     nodeN = 0;

    if ((morphN == 0 || sipP->iceA[sipP->bestX]->score >= thresh) && lvX != latP->end.lvX) continue;

    /* We need a temporary map (phoneX,frameX) -> gnode to recombine linguistic
       instances with the same left phonetic context and start frame.
    */

    for (morphX=0;morphX<morphN;morphX++)
      latP->mapping.p2n[morphX].frameX = -1;

    phoneY = sipP->phoneX;

    for (morphX=0;morphX<morphN;morphX++) {
      float score = SCORE_MAX;
      int   bpIdx = 0;
      NTok*  path = sipP->iceA[morphX]->path;
      float delta = sipP->iceA[morphX]->lms + svmapP->penalty.word;
      float rcmdelta = 0.0;
      GKey  gkey;

      if (svmapP->svocabP->list.itemA[svX].fillerTag) {
	delta += svmapP->penalty.fil - svmapP->penalty.word;
      } else if (lvX == svmapP->idx.end)
	delta -= svmapP->penalty.word;

      if (sipP->iceA[morphX]->score >= thresh) continue;
      score  = path[0].s;
      bpIdx  = path[0].bpIdx;

      if (score > thresh) continue;

      /* The final phone from the previous word doesn't matter here since
	 we have here a context independent word.
      */
      if (bpIdx >=0) {
	svY = bpA[bpIdx].svX;
      } else { 
	svY = 0;
      }

      phoneX = 0;

      /* Find the start frame for this word */
      frameX = stabGetFrame(spassP->stabP,bpIdx)+1;

      /* get the corresponding lattice node from the previous word */
      if (bpIdx >= 0) {
	oldP = latP->mapping.b2n[bpIdx];
      } else {
	oldP = latP->startP;
      }

      /* If we use a more aggressive pruning strategie for the lattice
	 than for the backpointer table, we will have some bp's without
	 corresponding lattice nodes.
      */
      if (! oldP) continue;

      /* To get the word-only score, we have to subtract the lm score and
	 the accumulated score up to the start from of this word.
      */
      if (bpIdx >=0) {	
	delta +=  bpA[bpIdx].score;
      }

      /* Now, we have to correct the rcm score from the previous word since 
	 the score from the bp table contain the score over all possible right
	 context models but the accumulated score rely on the particular
	 rcm's for the phonetic context for this word.
      */
      if (bpIdx >= 0) {
	if (spassP->streeP->nodeTbl.typeA[svY] == type_SLeaf) {
	  SNode*  slP = (SNode*) spassP->streeP->nodeTbl.A[svY];
	  RCM*    rmP = spassP->rcmP + slP->X.xhmm;
	  CTX     hmX = rmP->mapXA[phoneY];
	  CTX*   idxA = oldP->rcm.X;
	  float* rcmA = oldP->rcm.A;
	  int     idx = (idxA) ?  oldP->rcm.N-1 : -1;
	  rcmdelta = SCORE_MAX;
	  while (idx>=0 && idxA[idx] != hmX) idx--;
	  if (idx>=0)
	    rcmdelta=rcmA[idx];
	  delta += rcmdelta;
	} else if (spassP->streeP->nodeTbl.typeA[svY] == type_SDPhone) {
	  SDPhone* sdpP = (SDPhone*) spassP->streeP->nodeTbl.A[svY];
	  XCM*     xcmP = spassP->xcmP + sdpP->hmmX;

	  /* now it's becomes complicated: I need the left context from
	     the last word (=sdpP) to determine the xcmP map entry.
	     trace back to the parent from the last word and get
	     the last phone from that word
	  */
	  if (oldP->parentP) {
	    /* FF HACK - REORDERED FOR COMPILABILITY WITH VISUAL STUDIO */
	    GNode*   oldoldP = oldP->parentP->parentP;
	    SVX          svZ = oldoldP->svX;
	    rcmdelta = SCORE_MAX;
	    if (svZ != SVX_NIL) {
	      DictWord* mwordP = 
		spassP->streeP->dictP->list.itemA +
		spassP->streeP->svocabP->list.itemA[svZ].dictX;
	      CTX        leftX = mwordP->word.phoneA[mwordP->word.itemN-1];
	      CTX          hmX = xcmP->mapXA[leftX][phoneY];
	      CTX*   idxA = oldP->rcm.X;
	      float* rcmA = oldP->rcm.A;
	      int     idx = (idxA) ?  oldP->rcm.N-1 : -1;

	      while (idx>=0 && idxA[idx] != hmX) idx--;
	      if (idx>=0)
		rcmdelta=rcmA[idx];
	      delta += rcmdelta;
	    } else {
	      break;
	    }
	  } else {
	    if (latP->expert)
	      WARN("glatAdd_SIPhone: cannot get phonetic context for %d\n",svY);
	  }	
	} else if (spassP->streeP->nodeTbl.typeA[svY] != type_SIPhone) {
	  ERROR("glatAdd_SIPhone: cannot handle search node type for %d\n",svY);
	  break;
	}
      }

      /* no phonetic context */
      if (rcmdelta >= SCORE_MAX)
	continue;

      /* Check if we have already a GNode with the same (phoneX,frameX)
	 Replace it we have a better score or allocate a new gnode
      */
      nodeX = (latP->singularLCT) ? nodeN : 0;
      while (nodeX < nodeN && ! (latP->mapping.p2n[nodeX].svX    == svX &&
				 latP->mapping.p2n[nodeX].frameX == frameX &&
				 latP->mapping.p2n[nodeX].phoneX == phoneX &&
				 fabs((latP->mapping.p2n[nodeX].nodeP->score - score + delta) / (score-delta))<1e-4)) {
	nodeX++;
      }
      if (nodeX < nodeN) {
	newP = latP->mapping.p2n[nodeX].nodeP;
      } else {
	/* update map (svX,frameX,phoneX) -> nodeP */
	latP->mapping.p2n[nodeX].svX    = svX;
	latP->mapping.p2n[nodeX].frameX = frameX;
	latP->mapping.p2n[nodeX].phoneX = phoneX;
	latP->mapping.p2n[nodeX].nodeP  = (GNode*) bmemItem(latP->mem.node);

	/* update map nodeX  -> nodeP */
	glatAssureMapTable(latP,latP->nodeN+1);

	latP->mapping.x2n[latP->nodeN] = latP->mapping.p2n[nodeX].nodeP;

	newP = latP->mapping.p2n[nodeX].nodeP;
	newP->score   = SCORE_MAX;
	newP->gamma   = SCORE_MAX;
	newP->alpha   = 0.0;
	newP->beta    = 0.0;
	newP->frameS  = frameX;
	newP->frameE  = spassP->frameX;
	newP->svX     = svX;
	newP->rcm.A   = NULL;
	newP->rcm.X   = NULL;
	newP->rcm.N   = CTX_MAX;
	newP->childP  = NULL;
	newP->parentP = NULL;
	newP->nodeX   = latP->nodeN;
	newP->status  = DFS_INIT;
	newP->lct.N   = 0;
	newP->lct.A   = NULL;

	latP->nodeN++;
	latP->nodeM++;
	nodeN++;

	/* store new allocated node on the stack */
	if (latP->stack.nodeN == latP->stack.allocN) {
	  latP->stack.allocN += sipN;
	  if (NULL == (latP->stack.nodePA = 
	       realloc(latP->stack.nodePA,latP->stack.allocN*sizeof(GNode*)))) {
	    ERROR("glatAdd_SIPhone: allocation failure\n");
	    return TCL_ERROR;
	  }
	}
	latP->stack.nodePA[latP->stack.nodeN++] = newP;	
      } 

      /* we add here a temporay map entry (svX,morphX) -> nodeP since
	 we don't know the bpIdx yet. If a backpointer is created for
	 this instance, we can then define a direct map bpIdx -> nodeP
	 via this temporary mapping.
      */
      gkey.svX    = svX;
      gkey.morphX = morphX;
      gkey.nodeP  = newP;
      idx = listIndex((List*) &(latP->mapping.s2n),(void*) &gkey,1);
      
      /* update acoustic scores 
	 newP->score = word only score
	 newP->gamma = theory score, temporary needed for prune operations
      */
      newP->score = score - delta;
      newP->gamma = score;
      /* check if we have already a link from oldP to newP */
      if ((linkP = oldP->childP))
	while (linkP->childP != newP && linkP->nextP) 
	  linkP = linkP->nextP;

      /* add a link from lattice node oldP to newP */
      nlinkP = linkP;
      if (! linkP || linkP->childP != newP) {
	nlinkP = (GLink*) bmemItem(latP->mem.link);
	latP->linkN++;

	nlinkP->score   = rcmdelta;
	nlinkP->nextP   = NULL;
	nlinkP->txenP   = NULL;
	nlinkP->childP  = newP;
	nlinkP->parentP = oldP;

	if (! linkP) 
	  oldP->childP = nlinkP;
	else
	  linkP->nextP = nlinkP;
      } 

      /* add the nlink=(oldP->newP) to the list of backward-links */
      if (! newP->parentP) {
	newP->parentP = nlinkP;
      } else {
	linkP = newP->parentP;
	while (linkP->parentP != oldP && linkP->txenP) 
          linkP = linkP->txenP;
	if (linkP->parentP != oldP) {
	  linkP->txenP = nlinkP;
	}
      }

      /* connect to end_of_sentence node */
      if (lvX == svmapP->idx.end) {
	/* check if we have already a link from oldP to newP */
	linkP = newP->childP;
	if (linkP && linkP->childP != latP->endP) 
	  while (linkP->childP != latP->endP && linkP->nextP) 
	    linkP = linkP->nextP;

	/* add a link from lattice node newP to endP */
	nlinkP = linkP;
	if (! linkP || linkP->childP != latP->endP) {
	  nlinkP = (GLink*) bmemItem(latP->mem.link);
	  latP->linkN++;

	  nlinkP->score   = 0.0;
	  nlinkP->nextP   = NULL;
	  nlinkP->txenP   = NULL;
	  nlinkP->childP  = latP->endP;
	  nlinkP->parentP = newP;

	  if (! linkP) 
	    newP->childP = nlinkP;
	  else
	    linkP->nextP = nlinkP;
	} 

	/* add the nlink=(newP->end) to the list of backward-links */
	if (! latP->endP->parentP) {
	  latP->endP->parentP = nlinkP;
	} else {
	  linkP = latP->endP->parentP;
	  while (linkP->parentP != newP && linkP->txenP) 
	    linkP = linkP->txenP;
	  if (linkP->parentP != newP) {
	    linkP->txenP = nlinkP;
	  }
	}
      }
    } /* end for each instance */
  } /* end for each sip */

  return TCL_OK;
}

/* ------------------------------------------------------------------------
    word transactions from single phone words to lattice nodes
   ------------------------------------------------------------------------ */

int glatAdd_SDPhone (GLat* latP)
{
  SPass*     spassP = NULL;
  SVMap*     svmapP = NULL;
  XCM*         xcmP = NULL;
  PHMM*        hmmP = NULL;
  int          sdpX;
  int          sdpN;
  SDPhone*     sdpP;
  BP*           bpA;
  float      thresh;

  GNode *newP, *oldP;
  GLink *linkP, *nlinkP; 

  int hmmX,morphX,phoneX,phoneY,nodeX,frameX,idx;
  SVX svY;

  if (! latP->spassP) {
    ERROR("glatAdd_SDPhone: couldn't find decoder (aetsch)\n");
  }
  if (! latP->spassP->streeP->xcmsP) {
    return TCL_OK;
  }

  latP->status = LAT_CREATE;

  spassP = latP->spassP;
  svmapP = spassP->streeP->svmapP;
  xcmP   = spassP->streeP->xcmsP->list.itemA;
  hmmP   = spassP->streeP->hmmsP->list.itemA;

  sdpX   = 0;
  sdpN   = spassP->streeP->sTbl.dN;
  sdpP   = spassP->streeP->sTbl.dA;
  bpA    = spassP->stabP->A;
  thresh = spassP->beam.best + latP->beam.alpha;

  for (sdpX=0;sdpX<sdpN;sdpX++, sdpP++) {
    int    morphN = sdpP->morphN;
    SVX       svX = sdpP->svX;
    LVX       lvX = svmapGet(svmapP,(SVX)svX);
    int     nodeN = 0;
    int    stateN = hmmP[xcmP[sdpP->hmmX].hmmXA[0][0]].stateN+1;

    if ((morphN == 0 || sdpP->iceA[sdpP->bestX]->score >= thresh) && lvX != latP->end.lvX) continue;

    glatAllocRcm(latP,morphN,svmapP->svocabP->dictP->phones->list.itemN);

    /* We need a temporary map (phoneX,frameX) -> gnode to recombine linguistic
       instances with the same left phonetic context and start frame.
    */
    for (morphX=0;morphX<morphN;morphX++)
      latP->mapping.p2n[morphX].frameX = -1;

    phoneY = sdpP->phoneX;

    for (morphX=0;morphX<morphN;morphX++) {
      float score = SCORE_MAX;
      int   bpIdx = 0;
      NTok*  path = sdpP->iceA[morphX]->path;
      float delta = sdpP->iceA[morphX]->lms + svmapP->penalty.word;
      int  phoneA = sdpP->iceA[morphX]->phoneX;
      int    hmmN = xcmP[sdpP->hmmX].hmmNA[phoneA];
      float rcmdelta = 0.0;
      GKey  gkey;

      if (svmapP->svocabP->list.itemA[svX].fillerTag) {
	delta += svmapP->penalty.fil - svmapP->penalty.word;
      } else if (lvX == svmapP->idx.start)
	delta -= svmapP->penalty.word;

      if (sdpP->iceA[morphX]->score >= thresh) continue;
      for (hmmX=0;hmmX<hmmN;hmmX++) {
        if (path[hmmX*stateN].s < score) {
          score  = path[hmmX*stateN].s;
          bpIdx  = path[hmmX*stateN].bpIdx;
        }
      }
      if (score > thresh) continue;

      if (bpIdx >=0) {
	svY = bpA[bpIdx].svX;
      } else { 
	svY = 0;
      }

      /* last phone from the previous word */
      phoneX = phoneA;

      /* Find the start frame for this word */
      frameX = stabGetFrame(spassP->stabP,bpIdx)+1;

      /* get the corresponding lattice node from the previous word */
      if (bpIdx >= 0) {
	oldP = latP->mapping.b2n[bpIdx];
      } else {
	oldP = latP->startP;
      }

      /* If we use a more aggressive pruning strategie for the lattice
	 than for the backpointer table, we will have some bp's without
	 corresponding lattice nodes.
      */
      if (! oldP) continue;

      /* To get the word-only score, we have to subtract the lm score and
	 the accumulated score up to the start from of this word.
      */
      if (bpIdx >=0) {	
	delta +=  bpA[bpIdx].score;
      }

      /* Now, we have to correct the rcm score from the previous word since 
	 the score from the bp table contain the score over all possible right
	 context models but the accumulated score rely on the particular
	 rcm's for the phonetic context for this word.
      */
      if (bpIdx >= 0) {
	if (spassP->streeP->nodeTbl.typeA[svY] == type_SLeaf) {
	  SNode*  slP = (SNode*) spassP->streeP->nodeTbl.A[svY];
	  RCM*    rmP = spassP->rcmP + slP->X.xhmm;
	  CTX     hmX = rmP->mapXA[phoneY];
	  CTX*   idxA = oldP->rcm.X;
	  float* rcmA = oldP->rcm.A;
	  int     idx = (idxA) ?  oldP->rcm.N-1 : -1;
	  rcmdelta = SCORE_MAX;
	  while (idx>=0 && idxA[idx] != hmX) idx--;
	  if (idx>=0)
	    rcmdelta=rcmA[idx];
	  delta += rcmdelta;
	} else if (spassP->streeP->nodeTbl.typeA[svY] == type_SDPhone) {
	  SDPhone* sdpP = (SDPhone*) spassP->streeP->nodeTbl.A[svY];
	  XCM*     xcmP = spassP->xcmP + sdpP->hmmX;
	  
	  /* now it's becomes complicated: I need the left context from
	     the last word (=sdpP) to determine the xcmP map entry.
	     trace back to the parent from the last word and get
	     the last phone from that word
	  */
	  if (oldP->parentP) {
	    /* FF HACK - REORDERED FOR COMPILABILITY WITH VISUAL STUDIO */
	    GNode*   oldoldP = oldP->parentP->parentP;
	    SVX          svZ = oldoldP->svX;
	    rcmdelta = SCORE_MAX;

	    if (svZ != SVX_NIL) {
	      DictWord* mwordP = 
		spassP->streeP->dictP->list.itemA +
		spassP->streeP->svocabP->list.itemA[svZ].dictX;
	      CTX        leftX = mwordP->word.phoneA[mwordP->word.itemN-1];
	      CTX          hmX = xcmP->mapXA[leftX][phoneY];
	      CTX*   idxA = oldP->rcm.X;
	      float* rcmA = oldP->rcm.A;
	      int     idx = (idxA) ?  oldP->rcm.N-1 : -1;
	      
	      while (idx>=0 && idxA[idx] != hmX) idx--;
	      if (idx>=0)
		rcmdelta=rcmA[idx];
	      delta += rcmdelta;
	    } else {
	      break;
	    }
	  } else {
	    if (latP->expert)
	      WARN("glatAdd_SDPhone: cannot get phonetic context for %d\n",svY);
	  }
	
	}
	else if (spassP->streeP->nodeTbl.typeA[svY] != type_SIPhone) {
	  ERROR("glatAdd_SDPhone: cannot handle search node type for %d\n",svY);
	  break;
	}
      }

      /* no phonetic context */
      if (rcmdelta >= SCORE_MAX)
	continue;

      /* Check if we have already a GNode with the same (phoneX,frameX)
	 Replace it we have a better score or allocate a new gnode
      */
      nodeX = (latP->singularLCT) ? nodeN : 0;
      while (nodeX < nodeN && ! (latP->mapping.p2n[nodeX].svX    == svX    &&
				 latP->mapping.p2n[nodeX].frameX == frameX &&
				 latP->mapping.p2n[nodeX].phoneX == phoneX &&
				 fabs((latP->mapping.p2n[nodeX].nodeP->score - score + delta) / (score-delta))<1e-4)) {
	nodeX++;
      }
      if (nodeX < nodeN) {
	newP = latP->mapping.p2n[nodeX].nodeP;
      } else {
	/* update map (svX,frameX,phoneX) -> nodeP */
	latP->mapping.p2n[nodeX].svX    = svX;
	latP->mapping.p2n[nodeX].frameX = frameX;
	latP->mapping.p2n[nodeX].phoneX = phoneX;
	latP->mapping.p2n[nodeX].nodeP  = (GNode*) bmemItem(latP->mem.node);

	/* update map nodeX  -> nodeP */
	glatAssureMapTable(latP,latP->nodeN+1);

	latP->mapping.x2n[latP->nodeN] = latP->mapping.p2n[nodeX].nodeP;

	newP = latP->mapping.p2n[nodeX].nodeP;
	newP->score   = SCORE_MAX;
	newP->gamma   = SCORE_MAX;
	newP->alpha   = 0.0;
	newP->beta    = 0.0;
	newP->frameS  = frameX;
	newP->frameE  = spassP->frameX;
	newP->svX     = svX;
	newP->rcm.A   = latP->mem.rcmAA[morphX];
	newP->rcm.X   = NULL;
	newP->rcm.N   = hmmN;
	newP->childP  = NULL;
	newP->parentP = NULL;
	newP->nodeX   = latP->nodeN;
	newP->status  = DFS_INIT;
	newP->lct.N   = 0;
	newP->lct.A   = NULL;

	latP->nodeN++;
	latP->nodeM++;
	nodeN++;

	for (hmmX=0;hmmX<hmmN;hmmX++) {
          newP->rcm.A[hmmX]= SCORE_MAX;
        }

	/* store new allocated node on the stack */
	if (latP->stack.nodeN == latP->stack.allocN) {
	  latP->stack.allocN += sdpN;
	  if (NULL == (latP->stack.nodePA = 
	       realloc(latP->stack.nodePA,latP->stack.allocN*sizeof(GNode*)))) {
	    ERROR("glatAdd_SDPhone: allocation failure\n");
	    return TCL_ERROR;
	  }
	}
	latP->stack.nodePA[latP->stack.nodeN++] = newP;	
      } 

      /* we add here a temporay map entry (svX,morphX) -> nodeP since
	 we don't know the bpIdx yet. If a backpointer is created for
	 this instance, we can then define a direct map bpIdx -> nodeP
	 via this temporary mapping.
      */
      gkey.svX    = svX;
      gkey.morphX = morphX;
      gkey.nodeP  = newP;
      idx = listIndex((List*) &(latP->mapping.s2n),(void*) &gkey,1);
      
      /* update acoustic scores 
	 newP->score = word only score
	 newP->gamma = theory score, temporary needed for prune operations
      */
      newP->score = score - delta;
      newP->gamma = score;
      for (hmmX=0;hmmX<hmmN;hmmX++) {
        if (path[hmmX*stateN].s < SCORE_MAX && path[hmmX*stateN].s -score < newP->rcm.A[hmmX]) {
          newP->rcm.A[hmmX] =  path[hmmX*stateN].s -score;
        }
      }

      /* check if we have already a link from oldP to newP */
      if ((linkP = oldP->childP))
	while (linkP->childP != newP && linkP->nextP) 
	  linkP = linkP->nextP;

      /* add a link from lattice node oldP to newP */
      nlinkP = linkP;
      if (!linkP || linkP->childP != newP) {
	nlinkP = (GLink*) bmemItem(latP->mem.link);
	latP->linkN++;

	nlinkP->score   = rcmdelta;
	nlinkP->nextP   = NULL;
	nlinkP->txenP   = NULL;
	nlinkP->childP  = newP;
	nlinkP->parentP = oldP;

	if (!linkP) 
	  oldP->childP = nlinkP;
	else
	  linkP->nextP = nlinkP;
      } 

      /* add the nlink=(oldP->newP) to the list of backward-links */
      if (!newP->parentP) {
	newP->parentP = nlinkP;
      } else {
	linkP = newP->parentP;
	while (linkP->parentP != oldP && linkP->txenP) 
          linkP = linkP->txenP;
	if (linkP->parentP != oldP) {
	  linkP->txenP = nlinkP;
	}
      }

      /* connect to end_of_sentence node */
      if (lvX == svmapP->idx.end) {
	/* check if we have already a link from oldP to newP */
	linkP = newP->childP;
	if (linkP && linkP->childP != latP->endP) 
	  while (linkP->childP != latP->endP && linkP->nextP) 
	    linkP = linkP->nextP;

	/* add a link from lattice node newP to endP */
	nlinkP = linkP;
	if (! linkP || linkP->childP != latP->endP) {
	  nlinkP = (GLink*) bmemItem(latP->mem.link);
	  latP->linkN++;

	  nlinkP->score   = 0.0;
	  nlinkP->nextP   = NULL;
	  nlinkP->txenP   = NULL;
	  nlinkP->childP  = latP->endP;
	  nlinkP->parentP = newP;

	  if (! linkP) 
	    newP->childP = nlinkP;
	  else
	    linkP->nextP = nlinkP;
	} 

	/* add the nlink=(newP->end) to the list of backward-links */
	if (! latP->endP->parentP) {
	  latP->endP->parentP = nlinkP;
	} else {
	  linkP = latP->endP->parentP;
	  while (linkP->parentP != newP && linkP->txenP) 
	    linkP = linkP->txenP;
	  if (linkP->parentP != newP) {
	    linkP->txenP = nlinkP;
	  }
	}
      }
    } /* end for each instance */
    /* postprocess rcm scores */
    for (nodeX=0;nodeX<nodeN;nodeX++) {
      CTX*   idxA = NULL;
      float* rcmA = NULL;
      int     cnt = 0;
      newP = latP->mapping.p2n[nodeX].nodeP;

      /* count 'real' scores */
      for (hmmX=0;hmmX<newP->rcm.N;hmmX++) {
        if (newP->rcm.A[hmmX] < SCORE_MAX) cnt++;
      }
      if (cnt == 0) {
        newP->rcm.A = NULL;
	newP->rcm.X = NULL;
        newP->rcm.N = CTX_MAX;
      }
      /* store rcm's more memory efficient */
      rcmA = bmemAlloc(latP->mem.rcm,cnt*(sizeof(float)));
      idxA = bmemAlloc(latP->mem.idx,cnt*(sizeof(CTX)));
      cnt  = 0;
      for (hmmX=0;hmmX<newP->rcm.N;hmmX++) {
        if (newP->rcm.A[hmmX] < SCORE_MAX) {
          idxA[cnt]= hmmX;
          rcmA[cnt]= newP->rcm.A[hmmX];
          cnt++;
        }
      }
      newP->rcm.A = rcmA;
      newP->rcm.X = idxA;
      newP->rcm.N = cnt;
    } /* end of postprocess rcm scores */
  } /* end for each sdp */

  return TCL_OK;
}

/* ------------------------------------------------------------------------
    update map bpIdx -> gnodeP
   ------------------------------------------------------------------------ */

int glatMapBP (GLat* latP)
{
  SPass* spassP;
  STab*   stabP;
  GKey     gkey;
  int bpX, bpN, idx;

  if (! latP->spassP) {
    ERROR("glatMapBP: couldn't find decoder\n");
    return TCL_ERROR;
  }

  spassP = latP->spassP;
  stabP  = spassP->stabP;

  /* no bp's in this frame */
  if (stabP->frameN < spassP->frameX) 
    return TCL_OK;

  bpN = stabP->useNA[spassP->frameX];

  if (NULL == (latP->mapping.b2n = 
	       realloc(latP->mapping.b2n,stabP->allocN*sizeof(GNode*)))) {
    ERROR("glatMapBP: allocation failure\n");
    return TCL_ERROR;
  }
  for (bpX=bpN;bpX<stabP->allocN;bpX++)
    latP->mapping.b2n[bpX] = NULL;

  for (bpX=stabP->useNA[spassP->frameX-1];bpX<bpN;bpX++) {
    gkey.svX    = stabP->A[bpX].svX;
    gkey.morphX = stabP->A[bpX].morphX;

    idx = listIndex((List*) &(latP->mapping.s2n),(void*) &gkey,0);
    if (idx >= 0) {
      latP->mapping.b2n[bpX] = latP->mapping.s2n.itemA[idx].nodeP;

      if (latP->mapping.b2n[bpX]) {
	assert(spassP->frameX == latP->mapping.b2n[bpX]->frameE);
      }
    } else {
      latP->mapping.b2n[bpX] = NULL;
    }
  }

  listClear((List*) &(latP->mapping.s2n));
  return TCL_OK;
}

/* ------------------------------------------------------------------------
    glatRepair : ensure that we have the best hypothesis from the bp table
   ------------------------------------------------------------------------ */

static int glatRepair (GLat* latP)
{
  GNode*   fromP = latP->startP;
  SWord*  swordP = latP->svmapP->svocabP->list.itemA;
  SPass*  spassP = NULL;
  STree*  streeP = NULL;
  SVMap*  svmapP = NULL;
  STab*     tabP = NULL;
  int     frameN = 0;
  BP*        bpA = NULL;
  int   bpOffset = 0;
  int        bpN = 0;
  int        bpX = 0;

  /* score check */
  int       hypX = 0;
  int       hypN = 0;
  int*      bpXA = NULL;
  LingKS*   lksP = latP->spassP ? latP->spassP->streeP->svmapP->lksP : NULL;
  LCT        lct = LCT_NIL;

  if (! latP->spassP) {
    ERROR("glatRepair: couldn't find decoder interface\n");
    return TCL_ERROR;
  }

  spassP   = latP->spassP;
  streeP   = spassP->streeP;
  svmapP   = streeP->svmapP;
  tabP     = spassP->stabP;
  frameN   = tabP->frameN;
  bpA      = tabP->A;
  bpOffset = tabP->useNA[frameN-1];
  bpN      = tabP->useNA[frameN];
  
  if (NULL == (bpXA=malloc(bpN*sizeof(int)))) {
    ERROR("glatRepair: allocation failure\n");
    return TCL_ERROR;
  }

  /* look for final word */
  bpX = bpOffset;
  while (bpX < bpN && svmapGet(svmapP,bpA[bpX].svX) != svmapP->idx.end) bpX++;
    
  if (bpX == bpN) {
    ERROR("glatRepair: no final state in strace module\n");
    /* take best backpointer from final frame */
    bpX    = bpOffset;
  } 

  /* trace back best hypothesis from backpointer table */
  while (bpX != -1) {
    BP*       bp = bpA + bpX;
    bpXA[hypN++] = bpX;
    bpX  = bp->bpIdx;
  }

  /* create initial lct */
  lct = lksP->createLCT(lksP, svmapP->idx.start);

  /* dynamic lct generation -> clear lm cache */
  svmapClearCache(svmapP);

  /* create missing nodes and links */
  for (hypX=hypN-1;hypX>=0;hypX--) {
    BP*       bp = bpA + bpXA[hypX];
    GNode* nodeP = latP->mapping.b2n[bpXA[hypX]];

    if (! nodeP) {
      int nodeX = glatCreateNode(latP,bp->svX,
				 stabGetFrame(tabP,bp->bpIdx)+1,
				 stabGetFrame(tabP,bpXA[hypX]));
      int   lvX = svmapGet(latP->svmapP,bp->svX);

      nodeP = latP->mapping.x2n[nodeX];

      latP->mapping.b2n[bpXA[hypX]] = nodeP;

      nodeP->alpha = nodeP->beta  = nodeP->gamma = 0.0;
      nodeP->rcm.A = NULL;
      nodeP->rcm.X = NULL;
      nodeP->rcm.N = 0;

      if (bp->bpIdx < 0) {
	/* start of sentence */
	nodeP->score = bp->score;
      } else {
	/* get within-word score */
	nodeP->score = bp->score - bpA[bp->bpIdx].score;

	/* subtract lm penalty and score */
	if (swordP[nodeP->svX].fillerTag)	
	  nodeP->score -= svmapP->penalty.fil;
	else  {
	  nodeP->score -= svmapP->lz * svmapGetLMScore(svmapP,lct,nodeP->svX);
	  nodeP->score -= svmapP->penalty.word;
	}
	if (lvX == svmapP->idx.end) {
	  nodeP->score += svmapP->penalty.word;
	}
      }
      if (latP->expert == 1)
	INFO("glatRepair: created node %s from %d to %d : %2.2f\n",
	     swordP[nodeP->svX].name,
	     nodeP->frameS,nodeP->frameE,nodeP->score);
    }

    if (glatCreateLink (latP,fromP,nodeP,0.0))
      if (latP->expert == 1) 
	INFO("glatRepair: created link from %s to %s\n",
	     swordP[fromP->svX].name, swordP[nodeP->svX].name);

    if (svmapGet(svmapP,nodeP->svX) !=  svmapP->idx.start && ! swordP[nodeP->svX].fillerTag)
      lct = lksP->extendLCT(lksP,lct,svmapGet(svmapP,nodeP->svX));

    fromP = nodeP;
  }

  /* connect end of sentence node */
  if (glatCreateLink (latP,fromP,latP->endP,0.0))
    if (latP->expert == 1) 
      INFO("glatRepair: created link from %s to end of lattice\n",
	   swordP[fromP->svX].name);


  /* score check */
  if (latP->expert) {
    float score = 0.0;
    lct = lksP->createLCT(lksP, svmapP->idx.start);

    /* dynamic lct generation -> clear lm cache */
    svmapClearCache(svmapP);

    for (hypX=hypN-1;hypX>=0;hypX--) {
      BP*       bp = bpA + bpXA[hypX];
      GNode* nodeP = latP->mapping.b2n[bpXA[hypX]];
      LVX      lvX = svmapGet (svmapP, nodeP->svX);
      int      isf = swordP[nodeP->svX].fillerTag;
    
      /* acoustic score */
      score += nodeP->score;
    
      /* language model score */
      if (lvX == svmapP->idx.start) {
	score += svmapP->penalty.word + svmapP->lz * svmapP->lvxSA[nodeP->svX];
      } else if (lvX == svmapP->idx.end) {
	score += svmapP->lz * svmapGetLMScore(svmapP,lct,nodeP->svX);
      } else {
	if (isf) 
	  score += svmapP->penalty.fil;
	else {
	  score += svmapP->penalty.word + svmapP->lz * svmapGetLMScore(svmapP,lct,nodeP->svX);
	  lct    = lksP->extendLCT (lksP, lct, lvX);
	}
      }
    
      if (fabs(bp->score-score) > 1e-2)
	WARN("%s : bp= %2.2f  glat= %2.2f at %d  DIFFER\n",
	     swordP[nodeP->svX].name, bp->score,score,nodeP->frameE);
      else
	INFO("%s : bp= %2.2f  glat= %2.2f at %d\n",
	     swordP[nodeP->svX].name, bp->score,score,nodeP->frameE);
    
      /* add rcm score */
      if (hypX > 0) {
	GNode*   toP = latP->mapping.b2n[bpXA[hypX-1]];
	GLink* linkP = nodeP->childP;
	while (linkP && linkP->childP != toP) linkP = linkP->nextP;
	assert(linkP);
	score += linkP->score;
      }
    }
  }
  
  if (bpXA) free(bpXA);

  return TCL_OK;
}

/* ------------------------------------------------------------------------
    glatFinalize : ensure that we have a correct final node at final frame
   ------------------------------------------------------------------------ */

int glatFinalize (GLat* latP)
{
  int   latFrameX = 0; /* final frame extracted from lattice */
  int spassFrameX = 0; /* final frame extracted from spass   */
  int  stabFrameX = 0; /* final frame extracted from stab    */
  SVMap*   svmapP = latP->svmapP;
  GLink*    linkP = NULL;
  GNode*     newP = NULL;
  GLink*  parentP = NULL;
  GLink*   childP = NULL;
  SVX      endSVX = SVX_NIL;
  float     score = SCORE_MAX;
  int    nodeBest = 0;
  int nodeX;

  if (latP->end.frameX > 0) return latP->end.frameX;

  if (latP->spassP && latP->mapping.b2n)
    glatRepair(latP);

  /* get canonical endSVX */
  if (!svmapP->rev.N) {
    svmapP->dirty = 1;
    svmapIsDirty (svmapP);
    INFO ("glatFinalize: building reverse mapping for %s.\n", svmapP->name);
  }
  {
    LVX endLVX = svmapP->idx.end;
    int   idx  = svmapP->rev.N[endLVX];
    endSVX     = svmapP->rev.X[idx];
  }

  /* check if there is a final node at final frame */
  linkP = latP->endP->parentP;
  while (linkP) {
    if (linkP->parentP->frameE > latFrameX && 
	svmapGet(svmapP,linkP->parentP->svX) == svmapP->idx.end) {
      latFrameX = linkP->parentP->frameE;
    }
    linkP = linkP->txenP;
  }

  if (! latP->spassP || !latP->mapping.b2n) {
    latP->end.frameX= latFrameX;
    return  latFrameX;
  }

  spassFrameX = latP->spassP->frameN-1;
  stabFrameX  = latP->spassP->stabP->frameN;

  if (latFrameX == stabFrameX) {
    latP->end.frameX= latFrameX;
    return  latFrameX;
  }

  if (stabFrameX != spassFrameX) {
    WARN("glatFinalize: inconsistent final frame in %s.stab (%d) and %s (%d)\n",
	 latP->spassP->name,stabFrameX,latP->spassP->name,spassFrameX);
  }


  /* search for best node at final frame = stabFrameX */
  WARN("glatFinalize: try to find final node at %d\n",stabFrameX);
  latP->end.frameX = -1;

  for (nodeX=0;nodeX<latP->nodeN;nodeX++) {
    if (latP->mapping.x2n[nodeX]->frameE == stabFrameX) {
      if (latP->mapping.x2n[nodeX]->gamma < SCORE_MAX) {
	score    = latP->mapping.x2n[nodeX]->gamma;
	nodeBest = nodeX;
      }
    }
  }
  if (score >= SCORE_MAX) {
    ERROR("glatFinalize: couldn't find any useful node at frame %d\n",stabFrameX);
    return latP->end.frameX;
  }
  nodeX = nodeBest;
  INFO("glatFinalize: Create final node %d for %s at frame %d\n",latP->nodeN,
       latP->spassP->streeP->svocabP->list.itemA[latP->mapping.x2n[nodeX]->svX].name,stabFrameX);
  
  newP    = (GNode*) bmemItem(latP->mem.node);
  parentP = (GLink*) bmemItem(latP->mem.link);
  childP  = (GLink*) bmemItem(latP->mem.link);
  
  latP->end.frameX = stabFrameX;
  
  parentP->parentP = latP->mapping.x2n[nodeX];
  parentP->childP  = newP;
  parentP->nextP   = latP->mapping.x2n[nodeX]->childP;
  parentP->txenP   = NULL;
  parentP->score   = 0.0;
  
  latP->mapping.x2n[nodeX]->childP  = parentP;
  
  childP->parentP = newP;
  childP->childP  = latP->endP;
  childP->nextP   = NULL;
  childP->txenP   = latP->endP->parentP;
  childP->score   = 0.0;

  latP->endP->parentP = childP;

  newP->svX     = endSVX;
  newP->score   = 0.0;
  newP->gamma   = 0.0;
  newP->alpha   = 0.0;
  newP->beta    = 0.0;
  newP->frameS  = stabFrameX;
  newP->frameE  = stabFrameX;
  newP->rcm.A   = NULL;
  newP->rcm.N   = CTX_MAX;
  newP->childP  = childP;
  newP->parentP = parentP;
  newP->nodeX   = latP->nodeN;
  newP->status  = DFS_INIT;
  newP->lct.N   = 0;
  newP->lct.A   = NULL;

  /* update map nodeX  -> nodeP */
  if (latP->mapping.allocN <= latP->nodeN) {
    int nodeY = latP->nodeN;

    latP->mapping.allocN += 100;
    if (latP->mapping.x2n) latP->mapping.x2n -=2;
    if (NULL == (latP->mapping.x2n = realloc(latP->mapping.x2n,(latP->mapping.allocN+2)*sizeof(GNode*)))) {
      ERROR("glatFinalize: allocation failure\n");
      return TCL_ERROR;
    }
    latP->mapping.x2n +=2;
    
    latP->mapping.x2n[-1] = latP->startP;
    latP->mapping.x2n[-2] = latP->endP;
    for (;nodeY<latP->mapping.allocN;nodeY++)
      latP->mapping.x2n[nodeY] = NULL;
  }
  latP->mapping.x2n[latP->nodeN] = newP;
  latP->nodeN++;
  latP->nodeM++;

  return latP->end.frameX;
}


/* ========================================================================
    GLAT extended generation
   ======================================================================== */

/* To generate more dense word graphs, we connect all lattice nodes
   with matching start and end points of time. However to avoid millions
   of useless links, we create links with respect to their a-posteriori
   probabilities only.
*/

/* ------------------------------------------------------------------------
    sort lattice nodes according to their starting point
   ------------------------------------------------------------------------ */

extern int glatSortNodes (GLat* latP)
{
  int frameX, nodeX;
  int frameN = latP->end.frameX;

  if (!latP->spassP) {
    for (nodeX = 0; nodeX < latP->nodeM; nodeX++) {
      if (latP->mapping.x2n[nodeX] && latP->mapping.x2n[nodeX]->frameE > frameN)
	frameN = latP->mapping.x2n[nodeX]->frameE;
      if (latP->mapping.x2n[nodeX] && latP->mapping.x2n[nodeX]->frameS > frameN)
	frameN = latP->mapping.x2n[nodeX]->frameS;
    }
  } else if (latP->spassP->frameX > frameN)
    frameN = latP->spassP->frameX;
  frameN++;

  if (frameN > latP->sorter.frameN || !latP->spassP) {
    if (NULL == (latP->sorter.startCntA = 
		 realloc(latP->sorter.startCntA,frameN*sizeof(int)))) {
      ERROR("glatSortNodes: allocation failure\n");
      return TCL_ERROR;
    }
    if (NULL == (latP->sorter.endCntA = 
		 realloc(latP->sorter.endCntA,frameN*sizeof(int)))) {
      ERROR("glatSortNodes: allocation failure\n");
      return TCL_ERROR;
    }
    if (NULL == (latP->sorter.startAllocA = 
		 realloc(latP->sorter.startAllocA,frameN*sizeof(int)))) {
      ERROR("glatSortNodes: allocation failure\n");
      return TCL_ERROR;
    }
    if (NULL == (latP->sorter.endAllocA = 
		 realloc(latP->sorter.endAllocA,frameN*sizeof(int)))) {
      ERROR("glatSortNodes: allocation failure\n");
      return TCL_ERROR;
    }
    if (NULL == (latP->sorter.startPA = 
		 realloc(latP->sorter.startPA,frameN*sizeof(GNode**)))) {
      ERROR("glatSortNodes: allocation failure\n");
      return TCL_ERROR;
    }
    if (NULL == (latP->sorter.endPA = 
		 realloc(latP->sorter.endPA,frameN*sizeof(GNode**)))) {
      ERROR("glatSortNodes: allocation failure\n");
      return TCL_ERROR;
    }
    for (frameX=latP->sorter.frameN;frameX<frameN;frameX++) {
      latP->sorter.startAllocA[frameX] = 0;
      latP->sorter.endAllocA[frameX]   = 0;
      latP->sorter.startPA[frameX]     = NULL;
      latP->sorter.endPA[frameX]       = NULL;
    }
    latP->sorter.frameN = frameN;
  }

  for (frameX=0;frameX<latP->sorter.frameN;frameX++) {
    latP->sorter.startCntA[frameX]   = 0;
    latP->sorter.endCntA[frameX]     = 0; 
  }

  for (nodeX=0;nodeX<latP->nodeM;nodeX++) {
    GNode* nodeP = latP->mapping.x2n[nodeX];

    if (!nodeP) continue;

    if (latP->sorter.startCntA[nodeP->frameS] == 
	latP->sorter.startAllocA[nodeP->frameS]) {
      latP->sorter.startAllocA[nodeP->frameS] += 100;
      if (NULL == (latP->sorter.startPA[nodeP->frameS] = 
		   realloc(latP->sorter.startPA[nodeP->frameS],
			   latP->sorter.startAllocA[nodeP->frameS]*sizeof(GNode**)))) {
	ERROR("glatSortNodes: allocation failure\n");
	return TCL_ERROR;
      }
    }

    if (latP->sorter.endCntA[nodeP->frameE] == 
	latP->sorter.endAllocA[nodeP->frameE]) {
      latP->sorter.endAllocA[nodeP->frameE] += 100;
      if (NULL == (latP->sorter.endPA[nodeP->frameE] = 
		   realloc(latP->sorter.endPA[nodeP->frameE],
			   latP->sorter.endAllocA[nodeP->frameE]*sizeof(GNode**)))) {
	ERROR("glatSortNodes: allocation failure\n");
	return TCL_ERROR;
      }
    }

    latP->sorter.startPA[nodeP->frameS][latP->sorter.startCntA[nodeP->frameS]++] = nodeP;
    latP->sorter.endPA  [nodeP->frameE][latP->sorter.endCntA  [nodeP->frameE]++] = nodeP;
  }

  return TCL_OK;
}

/* ------------------------------------------------------------------------
    connect lattice nodes
   -----------------------
    some helper routines come first
   ------------------------------------------------------------------------ */

int glatLeftContextOk (GLat* latP, GNode* fromP, GNode* toP)
{
  STree* streeP = NULL;

  /* If we don't have the spass, we assume it's ok */
  if (!latP->spassP) return 1;
  else               streeP = latP->spassP->streeP;

  if (streeP->nodeTbl.typeA[toP->svX] == type_SIPhone) {
    return 1;
  } else {
    GNode*   parentP = toP->parentP->parentP; 

    /* get left context for toP = right context from previous parent */
    DictWord* dwordP = streeP->dictP->list.itemA +
                       streeP->svocabP->list.itemA[parentP->svX].dictX;
    int       phoneN = dwordP->word.itemN-1;
    int       phoneY = dwordP->word.phoneA[phoneN];
  
    /* get right context from fromP */
    dwordP = streeP->dictP->list.itemA +
             streeP->svocabP->list.itemA[fromP->svX].dictX;
    phoneN = dwordP->word.itemN-1;
  
    return (phoneY == dwordP->word.phoneA[phoneN]);
  }
}

float glatGetRightContext (GLat* latP, GNode* fromP, GNode* toP)
{
  STree*      streeP = NULL;
  float        score = SCORE_MAX;
  SVX            svX = fromP->svX;
  SVX            svY = toP->svX;

  /* If we don't have the spass, we assume '0' */
  if (!latP->spassP) return 0;
  else               streeP = latP->spassP->streeP;

  /* find the delta acoustic score for the right context model */
  switch (streeP->nodeTbl.typeA[svX]) {
  case type_SIPhone:
    score = 0.0;
    break;
  case type_SLeaf: 
    {
      DictWord* dwordP = streeP->dictP->list.itemA +
	                 streeP->svocabP->list.itemA[svY].dictX;
      int       phoneY = dwordP->word.phoneA[0];
      
      SNode*       slP = (SNode*) streeP->nodeTbl.A[svX];
      RCM*         rmP = latP->spassP->rcmP + slP->X.xhmm;
      CTX          hmX = rmP->mapXA[phoneY];
      CTX*        idxA = fromP->rcm.X;
      float*      rcmA = fromP->rcm.A;
      int          idx = (idxA) ? fromP->rcm.N-1 : -1;
      while (idx >= 0 && idxA[idx] != hmX)
	idx--;
      if (idx >= 0)
	score = rcmA[idx];
    }
    break;
  case type_SDPhone: 
    {
      DictWord* dwordP = streeP->dictP->list.itemA +
	                 streeP->svocabP->list.itemA[svY].dictX;
      int       phoneY = dwordP->word.phoneA[0];
      
      SDPhone*    sdpP = (SDPhone*) streeP->nodeTbl.A[svX];
      XCM*        xcmP = latP->spassP->xcmP + sdpP->hmmX;
      
      if (fromP->parentP) {
	GNode*   oldoldP = fromP->parentP->parentP;
	SVX          svZ = oldoldP->svX;
	DictWord* mwordP = streeP->dictP->list.itemA +
	  streeP->svocabP->list.itemA[svZ].dictX;
	CTX        leftX = mwordP->word.phoneA[mwordP->word.itemN-1];
	CTX          hmX = xcmP->mapXA[leftX][phoneY];
	CTX*        idxA = fromP->rcm.X;
	float*      rcmA = fromP->rcm.A;
	int          idx = (idxA) ? fromP->rcm.N-1 : -1;
	while (idx >= 0 && idxA[idx] != hmX)
	  idx--;
	if (idx >= 0)
	  score = rcmA[idx];
      } 
    }
    break;
  default:
    ERROR ("glatGetRightContext: cannot handle search node type for svX=%d.\n", svX);
    break;
  }
  return score;
}

static int glatConnect (GLat* latP, SVMap* svmapP, int prob_sum, float beam, int filLink)
{
  SWord* swordP = svmapP->svocabP->list.itemA;

  float  thresh;
  int frameX, nodeX;

  if (! latP->spassP) {
    ERROR("glatConnect: Couldn't find decoder\n");
    return TCL_ERROR;
  }
  if (latP->end.frameX == 0) {
    glatFinalize(latP);
  }
  if (latP->end.frameX == -1) {
    ERROR("glatConnect: Couldn't find final node\n");
    return TCL_ERROR;
  }
  svmapClearCache(svmapP);

  for (nodeX=0;nodeX<latP->nodeM;nodeX++)
    if (latP->mapping.x2n[nodeX])
      latP->mapping.x2n[nodeX]->status = DFS_INIT;

  glatSortNodes(latP);

  /* can we connect any end nodes? */
  frameX = latP->end.frameX;
  for (nodeX = 0; nodeX < latP->sorter.endCntA[frameX]; nodeX++) {
    GNode* nodeP = latP->sorter.endPA[frameX][nodeX];
    LVX      lvX = svmapGet (svmapP, nodeP->svX);
    
    if (lvX == svmapP->idx.end && !nodeP->childP)
      glatCreateLink (latP, nodeP, latP->endP, 0.0);
  }

  glatComputeGamma(latP,svmapP,prob_sum,1.0,0,0);
  thresh = latP->endP->alpha +  latP->endP->beta + beam;

  for (frameX=latP->end.frameX-1;frameX>=0;frameX--) {
    int fromN = latP->sorter.endCntA[frameX];
    int toN   = latP->sorter.startCntA[frameX+1];
    int linkN = 0;
    int fromX,toX;

    for (fromX=0;fromX<fromN;fromX++) {
      GNode* fromP = latP->sorter.endPA[frameX][fromX];
      SVX      svX = fromP->svX;
      LVX      lvX = svmapGet(svmapP,svX);
      int     lctX = 0;
      int     lctN = fromP->lct.N;
      LCT*    lctA = fromP->lct.A;

      /* do not connect filler words */
      if (! filLink && swordP[svX].fillerTag == 1) 
	continue;

      /* do not connect from end of sentence */
      if (lvX == svmapP->idx.end)
	continue;

      for (toX=0;toX<toN;toX++) {
	GLink* linkP = fromP->childP;
	GNode*   toP = latP->sorter.startPA[frameX+1][toX];
	SVX      svY = toP->svX;
	LVX      lvY = svmapGet(svmapP,svY);
	float probLM = svmapP->lz*svmapP->lvxSA[svY];
	float   beta = toP->score + toP->beta + probLM + svmapP->penalty.word;
	float  score = 0.0;
	lmScore   lm = LMSCORE_MAX;

	/* do not connect filler words */
	if (!filLink && swordP[svY].fillerTag == 1) 
	  continue;

	/* do not connect to wrong end of sentence */
	if (lvY == svmapP->idx.end && toP->frameE != latP->end.frameX)
	  continue;

	if (thresh < fromP->alpha + beta) continue;

        /* check if the left context match */
        if (!glatLeftContextOk (latP, fromP, toP))
          continue;

        /* check if we have the link already */
        while (linkP && linkP->childP != toP) {
          linkP = linkP->nextP;
        }
        if (linkP) continue;

	/* find the best lm score over all linguistic states of this node */
	for (lctX=0;lctX<lctN;lctX++) {
	  lmScore lm2 = svmapGetLMScore(svmapP,lctA[lctX],svY);
	  /* higher is better */
	  if (lm2 > lm) lm = lm2;
	}

	beta +=  svmapP->lz * lm;
	
	if (thresh < fromP->alpha + beta) continue;

	/* find the delta acoustic score for the right context model */
	score = glatGetRightContext (latP, fromP, toP);

	if (fromP->alpha + beta + score < thresh) {
	  if (glatCreateLink (latP, fromP, toP, score)) {
	    /* update beta if we have a new link */
	    linkN++;
	    if (fromP->beta < SCORE_MAX)
	      fromP->beta = addLogProbs(fromP->beta,beta);
	    else
	      fromP->beta = beta;
	  }
	}
      } /* for each toP */
    } /* for each fromP */
  }

  for (nodeX=0;nodeX<latP->nodeM;nodeX++)
    if (latP->mapping.x2n[nodeX])
      latP->mapping.x2n[nodeX]->status = DFS_INIT;

  return TCL_OK;
}

int glatConnectItf (ClientData cd, int argc, char *argv[])
{
  GLat*    latP = (GLat*) cd;
  SVMap* svmapP = latP->svmapP;
  float    beam = latP->beam.alpha;
  float  factor = 4.5;
  int   filLink = 0;
  int  prob_sum = 0;
  int       ac  = argc -1;

 if ( itfParseArgv( argv[0], &ac, argv+1, 0,
      "-map",    ARGV_OBJECT,NULL, &svmapP, "SVMap", "Vocabulary Mapper",    
      "-beam",   ARGV_FLOAT, NULL, &beam,      NULL, "lattice beam",
      "-factor", ARGV_FLOAT, NULL, &factor,    NULL, "multiplication factor for beam to make beam settings in old scripts compatible with bugfixes",
      "-filler", ARGV_INT,   NULL, &filLink,   NULL, "connect filler words",
      "-sum",    ARGV_INT,   NULL, &prob_sum,  NULL, "sum the probabilities",
   NULL) != TCL_OK) return TCL_ERROR;

  if (latP->nodeN+latP->linkN < 2) {
    ERROR("glatConnectItf: Empty Lattice\n");
    return TCL_ERROR;
  }
  if (!svmapP) {
    ERROR("glatConnectItf: no Vocabulary Mapper\n");
    return TCL_ERROR;
  }
  glatConnect(latP,svmapP,prob_sum,factor*beam,filLink);

  return TCL_OK;
}

/* ========================================================================
    GLAT pruning
   ======================================================================== */

/* ------------------------------------------------------------------------
    perform topN pruning on the stack
   ------------------------------------------------------------------------ */

static int cmpNode(const void* a,const void* b)
{
  float s1 = ((*(GNode**) a))->gamma;
  float s2 = ((*(GNode**) b))->gamma;

  if (s1<s2) return -1;
  if (s1>s2) return  1;
  return 0;
}

int glatCut (GLat* latP)
{
  SPass* spassP = latP->spassP;
  SVMap* svmapP = latP->svmapP;
  LVX    endLVX = svmapP->idx.end;
  GKey gkey;
  int nodeX, idx;

  if (! latP->spassP) {
    ERROR("glatCut: Couldn't find decoder\n");
    return TCL_ERROR;
  }
  
  if (latP->stack.nodeN >= latP->beam.nodeN) {
    qsort((void*)latP->stack.nodePA,latP->stack.nodeN,sizeof(GNode*),cmpNode);

    latP->nodeN -= latP->stack.nodeN;

    /* Re-organize top-N nodes in mapping.x2n-table */
    for (nodeX=0;nodeX<latP->beam.nodeN;nodeX++) {
      latP->stack.nodePA[nodeX]->nodeX = latP->nodeN;
      latP->mapping.x2n[latP->nodeN] = latP->stack.nodePA[nodeX];
      latP->nodeN++;
    }

    /* Make sure that the map-table contains zero for invalid entries */
    for (nodeX = 0; nodeX < latP->stack.nodeN - latP->beam.nodeN; nodeX++)
      latP->mapping.x2n[latP->nodeN+nodeX] = NULL;

    /* Delete pruned nodes */
    for (nodeX=latP->beam.nodeN;nodeX<latP->stack.nodeN;nodeX++) {
      SVX      svX = latP->stack.nodePA[nodeX]->svX;
      LVX      lvX = svmapGet(svmapP,svX);
      int morphX,morphN;

      if (lvX == endLVX) {
	/* we do not delete final nodes */
	latP->stack.nodePA[nodeX]->nodeX = latP->nodeN;
	latP->mapping.x2n[latP->nodeN] = latP->stack.nodePA[nodeX];
	latP->nodeN++;
	continue;
      }

      switch (spassP->streeP->nodeTbl.typeA[svX]) {
      case type_SLeaf : 
	morphN = ( (SNode*)   spassP->streeP->nodeTbl.A[svX])->morphN;
	break;
      case type_SIPhone :
	morphN = ( (SIPhone*) spassP->streeP->nodeTbl.A[svX])->morphN;
	break;
      case type_SDPhone :
	morphN = ( (SDPhone*) spassP->streeP->nodeTbl.A[svX])->morphN;
	break;
      default :
	ERROR("glatCut: cannot handle the search node type for %d\n",svX);
	continue;
      }

      /* If we delete this node, we have also to remove all corresponding
	 entries in the temporary map (svX,morphX) -> nodeP
      */

      for (morphX=0;morphX<morphN;morphX++) {
	gkey.svX    = svX;
	gkey.morphX = morphX;
	idx = listIndex((List*) &(latP->mapping.s2n),(void*) &gkey,0);
	if (idx >= 0 && latP->mapping.s2n.itemA[idx].nodeP == latP->stack.nodePA[nodeX]) {
	  latP->mapping.s2n.itemA[idx].nodeP = NULL;
	}
      }
      glatFreeNode(latP,latP->stack.nodePA[nodeX]);
      /* hack: glatFreeNode itself decrease the counter, 
	 however, we did it already
      */
      latP->nodeN++;
    }
  }
  latP->stack.nodeN = 0;
  latP->nodeM = latP->nodeN;

  return TCL_OK;
}

/* ------------------------------------------------------------------------
    glatPrune
   ------------------------------------------------------------------------ */

static int cmpFlt(const void *a, const void *b) { 
  float x = *((float*) a);
  float y = *((float*) b);
  if (x < y) return -1;
  if (x > y) return  1;
  return 0;
}

int glatPrune (GLat* latP, SVMap* svmapP, float beam, int nodeN, int pruneLinks, double scale, int prob_sum)
{
  SWord* swordP = svmapP->svocabP->list.itemA;
  float* scoreA = NULL;
  float  thresh = 0.0;
  int   nodeAll = 0;
  int     nodeX;

  if (latP->end.frameX == 0) {
    glatFinalize(latP);
  }
  if (latP->end.frameX == -1) {
    ERROR("glatPrune: Couldn't find final node\n");
    return TCL_ERROR;
  }

  glatPurify(latP);

  nodeAll = latP->nodeM;

  if (latP->nodeN <= nodeN) {
    if (latP->expert)
      WARN("glatPrune: have only %d nodes, cannot go down to %d\n",latP->nodeN,nodeN);
    return TCL_OK;
  }

  /* ----------------------------------------- */
  /* compute a (max) posteriories              */
  /* ----------------------------------------- */

  glatComputeGamma(latP,svmapP,prob_sum,scale,0,0);
  thresh = latP->endP->alpha +  latP->endP->beta + beam;

  if (nodeN >= 0) {
    if (NULL== (scoreA=malloc(latP->nodeN*sizeof(float)))) {
      ERROR("glatPrune: allocation failure\n");
      return TCL_ERROR;
    }
    for (nodeX=0;nodeX<latP->nodeN;nodeX++) {
      scoreA[nodeX] = latP->mapping.x2n[nodeX]->alpha+latP->mapping.x2n[nodeX]->beta;
    }
    qsort(scoreA,latP->nodeN,sizeof(float),cmpFlt);
    thresh=scoreA[nodeN-1]; 
    if (latP->expert) {
      INFO("glatPrune: threshold= %2.2f\n",thresh);
    }
    free(scoreA);
    /* sanity check */
    if (thresh < 0) {
      return TCL_OK;
    } 
  }

  /* ----------------------------------------- */
  /* check all nodes                           */
  /* ----------------------------------------- */

  for (nodeX=0;nodeX<nodeAll;nodeX++) {
    GNode* nodeP = latP->mapping.x2n[nodeX];

    /* check if we pruned enough nodes */
    if (nodeN >0 && nodeN >= latP->nodeN) break;

    /* ----------------------------------------- */
    /* delete node and all links                 */
    /* ----------------------------------------- */

    if (nodeP->alpha + nodeP->beta >= thresh || 
	nodeP->parentP == NULL || nodeP->childP == NULL) {

      glatFreeNode(latP,nodeP);
      latP->mapping.x2n[nodeX] = NULL;

    } else if (pruneLinks) {
      
      /* ----------------------------------------- */
      /* check all links                           */
      /* ----------------------------------------- */
      GLink*  linkP = nodeP->childP;
      GLink* plinkP = NULL;
      int      lctX = 0;
      int      lctN = nodeP->lct.N;
      LCT*     lctA = nodeP->lct.A;

      while (linkP) {
	GLink* nlinkP = linkP->nextP;
	GNode*    toP = linkP->childP;
	SVX       svY = toP->svX;
	float  probLM = svmapP->lz*svmapP->lvxSA[svY];
	float    beta = toP->score + toP->beta + probLM + linkP->score;
	lmScore    lm = LMSCORE_MAX;

	/* include word penalty and language model score */
	if (swordP[svY].fillerTag == 1) {
	  beta += svmapP->penalty.fil;
	} else {
	  beta += svmapP->penalty.word;
	
	  /* find the best lm score over all linguistic states of this node */
	  for (lctX=0;lctX<lctN;lctX++) {
	    lmScore lm2 = svmapGetLMScore(svmapP,lctA[lctX],svY);
	    if (lm < lm2) lm = lm2;
	  }
	  beta +=  svmapP->lz * lm;
	}

	if (nodeP->alpha + beta >= thresh) {

	  /* ----------------------------------------- */
	  /* delete link                               */
	  /* ----------------------------------------- */

	  /* remove link (node <- child) in child's list of parent links */
	  if (toP->parentP == linkP ) {
	    toP->parentP = linkP->txenP;
	  } else {
	    GLink* nnlinkP = toP->parentP;
	    assert(nnlinkP);
	    while (nnlinkP->txenP && nnlinkP->txenP != linkP) {
	      nnlinkP = nnlinkP->txenP;
	    }
	    assert (nnlinkP->txenP == linkP);
	    nnlinkP->txenP = nnlinkP->txenP->txenP;
	  }
	  
	  /* remove link from our list */
	  if (! plinkP) 
	    nodeP->childP = nlinkP;
	  else
	    plinkP->nextP = nlinkP;

	  bmemDelete(latP->mem.link,linkP);
	  linkP = nlinkP;
	  latP->linkN--;
	} else {	  
	  plinkP = linkP;
	  linkP  = nlinkP;
	}
      } /* for each link */
    } /* else (don't delete node) */
  } /* for each node */

  /* clean up remaining nodes */
  for (nodeX=0;nodeX<latP->nodeM;nodeX++) {
    GNode* nodeP = latP->mapping.x2n[nodeX];
    if (nodeP && ( ! nodeP->childP || ! nodeP->parentP)) {
      glatFreeNode(latP,nodeP);
      latP->mapping.x2n[nodeX] = NULL;
    }
  }

  /* purify lattice */
  latP->status = LAT_CREATE; 
  glatPurify(latP); 

  return TCL_OK;
}

int glatPruneItf (ClientData cd, int argc, char *argv[])
{
  GLat*     latP = (GLat*) cd;
  float     beam = latP->beam.alpha;
  float   factor = 4.5;
  float    scale = 1.0;
  int   prob_sum = 0;
  int      nodeN = -1;
  int pruneLinks = 0;
  SVMap*  svmapP = latP->svmapP;

  int    ac  = argc -1;

  if ( itfParseArgv( argv[0], &ac, argv+1, 0,
      "-beam",     ARGV_FLOAT,  NULL, &beam,      NULL,    "lattice beam",
      "-factor",   ARGV_FLOAT,  NULL, &factor,    NULL,    "multiplication factor for beam to make beam settings in old scripts compatible with bugfixes",
      "-scale",    ARGV_FLOAT,  NULL, &scale,     NULL,    "scaling factor",
      "-sum",      ARGV_INT,    NULL, &prob_sum,  NULL,    "sum the probabilities",
      "-nodeN",    ARGV_INT,    NULL, &nodeN,     NULL,    "prune to absolute nr. of nodes",
      "-link",     ARGV_INT,    NULL, &pruneLinks,NULL,    "prune lattice links",
      "-map",      ARGV_OBJECT, NULL, &svmapP,    "SVMap", "Vocabulary Mapper",    
   NULL) != TCL_OK) return TCL_ERROR;

  if (latP->nodeN+latP->linkN < 2) {
    ERROR("glatPruneItf: Empty Lattice\n");
    return TCL_ERROR;
  }
  if (!svmapP) {
    ERROR("glatPruneItf: no Vocabulary Mapper\n");
    return TCL_ERROR;
  }

  if (nodeN > 0 && nodeN >= latP->nodeN) {
    glatPurify(latP);
    if (latP->expert)
      WARN("glatPruneItf: have only %d nodes, cannot go down to %d\n",latP->nodeN,nodeN);   
    return TCL_OK;
  }

  glatPrune(latP,svmapP,factor*beam,nodeN,pruneLinks,scale,prob_sum);
  
  return TCL_OK;
}

/* ------------------------------------------------------------------------
    gnodeCheckStart
   ------------------------------------------------------------------------ */

static int gnodeCheckStart(GNode* nodeP, GLat* latP, LVX lvX, int frameX) 
{
  GLink      *linkP = nodeP->parentP;
  int        result = DFS_PURE_DEL;
  int    pathResult = DFS_PURE_DEL;
  LVX         mylvX = LVX_NIL;

  if ( nodeP->status  == DFS_PURE_OK)  return DFS_PURE_OK;
  if ( nodeP->status  == DFS_PURE_DEL) return DFS_PURE_DEL;

  mylvX = svmapGet(latP->svmapP,nodeP->svX);

  if (mylvX == lvX) {
    linkP = NULL; 
    if (nodeP->frameS == frameX)
      result = DFS_PURE_OK;
    else
      result = DFS_PURE_DEL;
  }

  while (linkP) {
    GLink* nlinkP =  linkP->txenP;
    pathResult    = (linkP->parentP->status == DFS_INIT) ? 
      gnodeCheckStart(linkP->parentP,latP,lvX,frameX) :
      linkP->parentP->status;

    /* delete start nodes with wrong startLVX */
    if (mylvX != lvX) {
      GLink* tmpP = linkP;
      while (tmpP && tmpP->parentP != latP->startP) tmpP= tmpP->txenP;
      if (tmpP) pathResult = DFS_PURE_DEL;
    }
    linkP  = nlinkP;

    if (pathResult == DFS_PURE_OK)
      result = DFS_PURE_OK;
  } 

  nodeP->status = result;
  if (nodeP->status == DFS_PURE_DEL && nodeP->nodeX >= 0) {
    latP->mapping.x2n[nodeP->nodeX] = NULL;
    glatFreeNode(latP,nodeP);
  }

  return result;
}

/* ------------------------------------------------------------------------
    gnodeCheckEnd
   ------------------------------------------------------------------------ */

static int gnodeCheckEnd(GNode* nodeP, GLat* latP, LVX lvX, int frameX) 
{
  GLink      *linkP = nodeP->childP;
  int        result = DFS_PURE_DEL;
  int    pathResult = DFS_PURE_DEL;
  LVX         mylvX = LVX_NIL;

  if (nodeP->status  == DFS_PURE_OK)  return DFS_PURE_OK;
  if (nodeP->status  == DFS_PURE_DEL) return DFS_PURE_DEL;

  mylvX= svmapGet(latP->svmapP,nodeP->svX);

  if (mylvX == lvX) {
    linkP = NULL; 
    if (nodeP->frameE == frameX)
      result = DFS_PURE_OK;
    else
      result = DFS_PURE_DEL;
  }

  while (linkP) {
    GLink* nlinkP =  linkP->nextP;
    pathResult    = (linkP->childP->status == DFS_INIT) ? 
      gnodeCheckEnd(linkP->childP,latP,lvX,frameX) :
      linkP->childP->status;

    /* delete end nodes with wrong endLVX */
    if (mylvX != lvX) {
      GLink* tmpP = linkP;
      while (tmpP && tmpP->childP != latP->endP) tmpP = tmpP->nextP;
      if (tmpP) pathResult = DFS_PURE_DEL;
    }
    linkP  = nlinkP;
    if (pathResult == DFS_PURE_OK)
      result = DFS_PURE_OK;

  } 

  nodeP->status = result;
  if (nodeP->status == DFS_PURE_DEL && nodeP->nodeX >= 0) {
    latP->mapping.x2n[nodeP->nodeX] = NULL;
    glatFreeNode(latP,nodeP);
  }

  return result;
}

/* ------------------------------------------------------------------------
    glatCount
   ------------------------------------------------------------------------ */

static void gnodeCount (GLat* latP, GNode* nodeP) 
{
  GLink* linkP = nodeP->parentP;

  /* 1: If we were already visited, the better */
  if (nodeP->status == DFS_COUNT)
    return;

  /* 2: We're ok, if we're the root node,
        otherwise visit our parents */
  if (nodeP != latP->startP) while (linkP) {
    if (nodeP == linkP->parentP) {
      GLink* tmpP = linkP->txenP;
      WARN ("gnodeCount: node %d in %s contains self-link, deleting.\n",
	    nodeP->nodeX, latP->name);
      glatFreeLink (latP, linkP);
      linkP = tmpP;
      continue;
    }
    gnodeCount (latP, linkP->parentP);
    latP->linkN++;
    linkP = linkP->txenP;
  }

  /* 3: Count ourselves, unless we're the final node */
  if (nodeP != latP->endP) {
    nodeP->nodeX = latP->nodeN++;
    latP->mapping.x2n[nodeP->nodeX] = nodeP;
  }
  nodeP->status = DFS_COUNT;

  return;
}

void glatCount (GLat* latP)
{
  int nodeX;

  latP->nodeN = -1;
  latP->linkN = 0;

  gnodeCount (latP, latP->endP);  

  /* Florian: for safety */
  if (latP->nodeN < 0) {
    INFO ("glatCount: latP->nodeN floored at 0\n");
    latP->nodeN = 0;
  }

  for (nodeX = latP->nodeN; nodeX < latP->nodeM; nodeX++)
    latP->mapping.x2n[nodeX] = NULL;

  latP->nodeM = latP->nodeN;
  return;   
}


/* ------------------------------------------------------------------------
    glatPurify
   ------------------------------------------------------------------------ */

int glatPurify_old (GLat* latP)
{
  int nodeN = 0;
  int nodeX;

  if (latP->end.frameX == 0) {
    glatFinalize(latP);
  }
  if (latP->end.frameX == -1) {
    ERROR("glatPurify: Couldn't find final node\n");
    return TCL_OK;
  }

  /* reset status */
  nodeN = latP->nodeM;
  for (nodeX=0;nodeX<nodeN;nodeX++) 
    if (latP->mapping.x2n[nodeX])
      latP->mapping.x2n[nodeX]->status = DFS_INIT;

  /* check if everybody can reach the final node */
  gnodeCheckEnd(latP->startP,latP,latP->end.lvX, latP->end.frameX);

  /* reset status */
  for (nodeX=0;nodeX<nodeN;nodeX++)
    if (latP->mapping.x2n[nodeX])
      latP->mapping.x2n[nodeX]->status = DFS_INIT;

  /* check if everybody came from the start node */
  gnodeCheckStart(latP->endP,latP,latP->svmapP->idx.start,0);

  glatCount(latP);

  /* reset status */
  latP->startP->status = DFS_INIT;
  for (nodeX=0;nodeX<latP->nodeN;nodeX++)
    latP->mapping.x2n[nodeX]->status = DFS_INIT;

  latP->status = LAT_PURE;
  return TCL_OK;
}

/* mode = 0 start node direction
   mode = 1 final node direction */
int glatDeleteSubPath (GLat *latP, GNode *nodeP, int direction) {
  GLink *tmpLP = NULL;
  GNode *tmpNP = NULL;

  if ( direction == 0 ) {
    tmpLP = nodeP->childP;

    /* check for other childs */
    if ( tmpLP ) {
      tmpNP = tmpLP->childP;
      if ( tmpNP->childP ) return TCL_OK;

      /* get parent, so that this node can be removed */
      tmpLP = nodeP->parentP;
      tmpNP = tmpLP->parentP;

      latP->mapping.x2n[nodeP->nodeX] = NULL;
      glatFreeNode(latP,nodeP);

      glatDeleteSubPath (latP, tmpNP, direction);
    }
  } else if ( direction == 1 ) {
    tmpLP = nodeP->parentP;

    /* check for other parents */
    if ( tmpLP ) {
      tmpNP = tmpLP->parentP;
      if ( tmpNP->parentP ) return TCL_OK;

      /* get child, so that this node can be removed */
      tmpLP = nodeP->childP;
      tmpNP = tmpLP->childP;

      latP->mapping.x2n[nodeP->nodeX] = NULL;
      glatFreeNode(latP,nodeP);

      glatDeleteSubPath (latP, tmpNP, direction);
    }
  }

  return TCL_OK;
}

int glatPurify (GLat* latP)
{
  int nodeN = 0;
  int nodeX;

  if (latP->end.frameX == 0) {
    glatFinalize(latP);
  }
  if (latP->end.frameX == -1) {
    ERROR("glatPurify: Couldn't find final node\n");
    return TCL_OK;
  }

  /* remove disconnected nodes */
  nodeN = latP->nodeM;
  for (nodeX=0;nodeX<nodeN;nodeX++) {
    GNode *nodeP = latP->mapping.x2n[nodeX];

    if ( nodeP ) {
      /* single independent node */
      if ( !nodeP->childP && !nodeP->parentP ) {
	latP->mapping.x2n[nodeP->nodeX] = NULL;
	glatFreeNode (latP,nodeP);
      }
	
      /* unable to reach final node, i.e. no child links */
      if ( !nodeP->childP )  glatDeleteSubPath (latP, nodeP, 0);

      /* unable to reach final node, i.e. no parents */
      if ( !nodeP->parentP ) glatDeleteSubPath (latP, nodeP, 1);
    }
  }

  /* reset status */
  nodeN = latP->nodeM;
  for (nodeX=0;nodeX<nodeN;nodeX++) 
    if (latP->mapping.x2n[nodeX])
      latP->mapping.x2n[nodeX]->status = DFS_INIT;

  /* check if everybody can reach the final node */
  gnodeCheckEnd(latP->startP,latP,latP->end.lvX, latP->end.frameX);

  /* reset status */
  for (nodeX=0;nodeX<nodeN;nodeX++)
    if (latP->mapping.x2n[nodeX])
      latP->mapping.x2n[nodeX]->status = DFS_INIT;

  /* check if everybody came from the start node */
  gnodeCheckStart(latP->endP,latP,latP->svmapP->idx.start,0);

  glatCount(latP);

  /* reset status */
  latP->startP->status = DFS_INIT;
  for (nodeX=0;nodeX<latP->nodeN;nodeX++)
    latP->mapping.x2n[nodeX]->status = DFS_INIT;

  latP->status = LAT_PURE;
  return TCL_OK;
}


int glatPurifyItf (ClientData cd, int argc, char *argv[])
{
  GLat* latP = (GLat*) cd;
  int     ac = argc-1;

  if ( itfParseArgv( argv[0], &ac, argv+1, 0,
       NULL) != TCL_OK) return TCL_ERROR;

  if (latP->nodeN+latP->linkN < 2) {
    ERROR("glatPurifyItf: Empty Lattice\n");
    return TCL_ERROR;
  }
  return glatPurify(latP);
}
