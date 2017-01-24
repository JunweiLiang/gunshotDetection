/* ========================================================================
    JANUS      Speech- to Speech Translation
               ------------------------------------------------------------
               Object: Search Tree
               ------------------------------------------------------------

    Author  :  Hagen Soltau
    Module  :  stree.c
    Date    :  $Id: stree.c 3416 2011-03-23 03:02:18Z metze $
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
    Revision 1.10  2005/11/08 12:40:19  metze
    Weird double comparison removed

    Revision 1.9  2005/03/04 09:17:31  metze
    Extra verbosity

    Revision 1.8  2003/11/06 17:44:57  metze
    Bugfix when adding SIps later

    Revision 1.7  2003/09/05 15:33:21  soltau
    streeLoad : fixed svx -> ptr mapping

    Revision 1.6  2003/08/14 11:20:09  fuegen
    Merged changes on branch jtk-01-01-15-fms (jaguar -> ibis-013)

    Revision 1.5.4.75  2003/07/02 08:04:19  metze
    *** empty log message ***

    Revision 1.5.4.74  2003/06/30 15:23:43  metze
    didn't found -> didn't find

    Revision 1.5.4.73  2003/05/08 16:41:55  soltau
    Purified!

    Revision 1.5.4.72  2002/12/11 09:45:27  soltau
    Added canonical start index

    Revision 1.5.4.71  2002/11/22 08:46:21  fuegen
    now streeBuildTree also works without having XCM

    Revision 1.5.4.70  2002/11/21 17:10:11  fuegen
    windows code cleaning

    Revision 1.5.4.69  2002/11/19 17:57:16  metze
    Ignore RCM message

    Revision 1.5.4.68  2002/11/13 09:59:19  soltau
    - Start and End words rely on LVX now
    - Changes for optional words
    - single phone are allowed to have dynamic LVX now

    Revision 1.5.4.67  2002/10/15 11:07:43  soltau
    streeAddSPhone: fixed backing off to context independent models

    Revision 1.5.4.66  2002/10/07 07:39:33  soltau
    srootDeactivate: added deactivation of instances

    Revision 1.5.4.65  2002/08/27 08:49:12  metze
    itfParseArgv now uses <name> and NULL initialization for char*

    Revision 1.5.4.64  2002/08/01 13:42:32  metze
    Fixes for clean documentation.

    Revision 1.5.4.63  2002/07/18 13:47:16  soltau
    optimized stree structures: bestscore -> bestX, worstscore -> worstX

    Revision 1.5.4.62  2002/06/28 16:05:21  soltau
    Added   streeAddPronGraph
    Deleted streeCheck_*

    Revision 1.5.4.61  2002/06/26 11:57:54  fuegen
    changes for new directory structure support and code cleaning

    Revision 1.5.4.60  2002/06/10 09:58:04  soltau
    streeLoad: checking return values now

    Revision 1.5.4.59  2002/04/26 14:53:37  soltau
    cleaned streeConfigure

    Revision 1.5.4.58  2002/04/26 13:52:20  soltau
    enhanced streeConfigure

    Revision 1.5.4.57  2002/04/08 14:00:29  soltau
    streeDump: initialize vX.s instead of vx.l to cover different SVX/LVX combinations

    Revision 1.5.4.56  2002/01/27 15:13:59  soltau
    streeTrace
     - support for single phone words
    streeAddSPhone:
     - support for ignoreRCM flag
     - fixed allocation problem in hmmXA array to build xword

    Revision 1.5.4.55  2002/01/25 11:32:44  soltau
    *** empty log message ***

    Revision 1.5.4.54  2002/01/17 09:57:58  metze
    Option -dumpLM works again in streeDump

    Revision 1.5.4.53  2002/01/14 14:13:54  soltau
    streeAdd/Del_Word : check for compressed networks
    Added streeConfigure

    Revision 1.5.4.52  2002/01/14 10:31:04  metze
    Cleaned up reading/ writing of dumps

    Revision 1.5.4.51  2002/01/11 15:54:02  soltau
    streeCompress : Added verbose option
    streeLoad     : Added verbose option

    Revision 1.5.4.50  2002/01/10 13:40:14  soltau
    Added stree compression, search space is now represented as a shared
    prefix and suffix structure

    Revision 1.5.4.49  2002/01/09 17:06:40  soltau
    changed child access in stree

    Revision 1.5.4.48  2001/12/17 12:00:34  soltau
    Removed position mapper

    Revision 1.5.4.47  2001/12/17 09:44:02  soltau
    Deactivation of position mapper

    Revision 1.5.4.46  2001/12/07 11:19:47  soltau
    streeDelWord: fixed search for sleaf

    Revision 1.5.4.45  2001/12/06 15:31:53  soltau
    streeDelWord: fixed deattachment for leafs in 2-phone words

    Revision 1.5.4.44  2001/11/30 14:49:32  soltau
    streeDelWord: Fixed deallocation of nodes

    Revision 1.5.4.43  2001/11/23 09:33:28  soltau
    streeDelWord: Fixed diphoneMapper
    streeDelWord: Added Deallocation of SDPhones

    Revision 1.5.4.42  2001/11/22 19:10:11  soltau
    Fixed another minor bug in streeTrace

    Revision 1.5.4.41  2001/11/22 16:39:34  soltau
    Fixed bug in streeTrace

    Revision 1.5.4.40  2001/11/20 16:35:44  soltau
    Changed smem configuration, support for optimized bmem/smem functions

    Revision 1.5.4.39  2001/10/22 08:41:43  metze
    Changed interface to score functions

    Revision 1.5.4.38  2001/10/01 10:44:32  soltau
    Use polymorphism defines from slimits

    Revision 1.5.4.37  2001/09/27 14:55:33  soltau
    *** empty log message ***

    Revision 1.5.4.36  2001/09/26 14:38:28  metze
    Changed Interface to LModelNJD dump function

    Revision 1.5.4.35  2001/09/26 14:33:16  soltau
    Add phmm's for sdphones only if necessary

    Revision 1.5.4.34  2001/09/25 14:52:59  soltau
    Added streeLoad output

    Revision 1.5.4.33  2001/09/24 14:11:46  soltau
    Added ltree argument for check routines

    Revision 1.5.4.32  2001/07/31 10:09:16  soltau
    Fixed srootPuts

    Revision 1.5.4.31  2001/07/30 15:20:15  soltau
    Fixed problems in streeAddSphones if we call it twice

    Revision 1.5.4.30  2001/07/14 15:33:55  soltau
    Support for SDPhones in streeLoad/Save

    Revision 1.5.4.29  2001/07/12 18:50:35  soltau
    Added fully context dependent single phone words
    Incorporated fast lm read routines in stree dump

    Revision 1.5.4.28  2001/06/28 10:48:23  soltau
    Added Load/save routines for binary files to support fast decoder startup

    Revision 1.5.4.27  2001/06/25 21:05:46  soltau
    Changed bmem interface

    Revision 1.5.4.26  2001/06/22 15:32:37  soltau
    Added Tree memory management

    Revision 1.5.4.25  2001/06/22 14:47:53  soltau
    check for word candidates in streeAddSIPhone

    Revision 1.5.4.24  2001/06/20 17:26:36  soltau
    *** empty log message ***

    Revision 1.5.4.23  2001/06/18 08:00:16  metze
    Works with svmapGet which only returns indices, not scores

    Revision 1.5.4.22  2001/06/11 14:09:41  soltau
    Use always svmapGet

    Revision 1.5.4.21  2001/04/27 07:39:21  metze
    General access functions to LM via 'LCT' implemented

    Revision 1.5.4.20  2001/04/23 13:46:45  soltau
    Added streeReinit to support vocabulary changes on the fly
    Added functions to free roots, nodes, and leafs

    Revision 1.5.4.19  2001/04/12 19:21:39  soltau
    removed same_pron table

    Revision 1.5.4.18  2001/04/12 18:42:33  soltau
    same_pron support

    Revision 1.5.4.17  2001/03/22 17:10:50  soltau
    Added siphoneCheck
    Set lvX for all filler words (including opt. sil) to LVX_NIL

    Revision 1.5.4.16  2001/03/20 17:07:58  soltau
    Added sort routines

    Revision 1.5.4.15  2001/03/20 13:33:11  soltau
    Added sanity check routines
    Added trace function
    Init lnX with Ltree null node

    Revision 1.5.4.14  2001/03/16 14:55:17  soltau
    endSVX changes

    Revision 1.5.4.13  2001/03/16 11:06:22  soltau
    Added trace function
    checking for words without pronunciation in streeAdd

    Revision 1.5.4.12  2001/03/13 18:01:02  soltau
    Removed p2l

    Revision 1.5.4.11  2001/03/12 18:47:08  soltau
    - Changed memory allocation strategie
    - Disabled optimized handling of same_pron_words
      (each word has now a unique leaf)

    Revision 1.5.4.10  2001/03/06 06:45:35  soltau
    purified

    Revision 1.5.4.9  2001/02/27 09:03:11  soltau
    Fixed memory overflow in diphone Mapper

    Revision 1.5.4.8  2001/02/21 18:06:07  soltau
    Changed pos2pos handling

    Revision 1.5.4.7  2001/02/16 17:45:26  soltau
    Added typeinfo

    Revision 1.5.4.6  2001/02/15 18:02:00  soltau
    Added routines to deactivate nodes
    Added position mapper
    Delayed memory allocations

    Revision 1.5.4.5  2001/02/02 16:08:11  soltau
    Added map SVocab -> SLeaf
    Added XHMM_Init

    Revision 1.5.4.4  2001/02/01 18:39:02  soltau
    Fixed bug with same_pronunciation_words

    Revision 1.5.4.3  2001/01/31 14:09:19  soltau
    redesign

    Revision 1.5.4.2  2001/01/30 08:32:17  soltau
    Fixed initialization of spTbl

    Revision 1.5.4.1  2001/01/29 18:04:09  soltau
    Added SinglePhone words
    Added Memory Management

    Revision 1.5  2001/01/25 11:00:28  soltau
    temporary checkin to make Christian happy

    Revision 1.4  2001/01/25 08:53:16  soltau
    Added routines to extract UniqueInfo

    Revision 1.3  2001/01/18 13:43:25  soltau
    Added init, deinit for SRoot, SNode, SLeaf and STree
    Added streeAdd, streeBuildTree
    Added access functions

    Revision 1.2  2001/01/16 07:19:54  soltau
    Saved wrong idea (diphone and LCM mapper not needed)

    Revision 1.1  2001/01/12 18:20:00  soltau
    Initial revision


   ======================================================================== */

char  streeRcsVersion[]=
    "$Id: stree.c 3416 2011-03-23 03:02:18Z metze $";

#include "stree.h"
#include "lmla.h"
#include "mach_ind_io.h"


/* ========================================================================
    Search Root
   ======================================================================== */

extern TypeInfo  srootInfo;
extern TypeInfo  snodeInfo;
extern TypeInfo  smemInfo;

/* ------------------------------------------------------------------------
    srootInit
   ------------------------------------------------------------------------ */

static int srootInit(SRoot* srootP, STree* streeP, PHMMX diphoneX, CTX phoneX) 
{
  srootP->phoneX     = phoneX;
  srootP->xhmmX      = diphoneX;
  srootP->morphN     = 0;
  srootP->bestX      = LPM_NIL;
  srootP->worstX     = LPM_NIL;
  srootP->iceA       = NULL;
  srootP->lnX        = LTREE_NULL_NODE;
  srootP->childPA    = NULL;
  srootP->childN     = 0;
  srootP->allocN     = 0;
  srootP->lct2pos    = NULL;

  if (streeP->smem.level >= 0) {
    srootP->allocN  = streeP->smem.morphBlkN;
  }
  smemAllocRoot(srootP,streeP);

  /* add pointer to root pointer table */
  if (streeP->rootTbl.N == streeP->rootTbl.allocN) {
    streeP->rootTbl.allocN += streeP->rootTbl.blkSize;
    if (NULL == (streeP->rootTbl.PA = (SRoot**) realloc( (void*)
        streeP->rootTbl.PA,streeP->rootTbl.allocN*sizeof(SRoot*)))) {
      ERROR("srootInit: allocation failure\n");
      return TCL_ERROR;
    }
  }
  streeP->rootTbl.PA[streeP->rootTbl.N] = srootP;
  streeP->rootTbl.N++;

  return TCL_OK;
}

/* ------------------------------------------------------------------------
    srootCreate
   ------------------------------------------------------------------------ */

static SRoot* srootCreate(STree* streeP, PHMMX diphoneX, CTX phoneX) 
{
  SRoot *srootP = (SRoot*) bmemItem(streeP->mem.root);

  if (! srootP) {
    ERROR("srootCreate: allocation failure\n");
    return NULL;
  }
  if (TCL_ERROR == (srootInit(srootP,streeP,diphoneX,phoneX))) {    
    return NULL;
  }
  return srootP;
}

/* ------------------------------------------------------------------------
    srootDeinit
   ------------------------------------------------------------------------ */

static int srootDeinit(SRoot* srootP,STree* streeP) 
{  
  smemFreeRoot(srootP,streeP);
  return TCL_OK;
}

/* ------------------------------------------------------------------------
    srootFree
   ------------------------------------------------------------------------ */

static int srootFree(SRoot *srootP, STree* streeP)
{
  if (srootP) {
    if ( srootDeinit(srootP,streeP) != TCL_OK) return TCL_ERROR;
    bmemDelete(streeP->mem.root,(void*) srootP);
  }
  return TCL_OK;
}

/* ------------------------------------------------------------------------
    srootDeactivate
   ------------------------------------------------------------------------ */

void srootDeactivate (SRoot* srootP, STree* streeP)
{

  LCM*   lcmP = streeP->lcmsP->list.itemA + srootP->xhmmX;
  PHMM*  hmmP = streeP->hmmsP->list.itemA + lcmP->hmmXA[0];
  int  stateN = hmmP->stateN + 1;
  int stateX,morphX;

  for (morphX=0;morphX<srootP->morphN;morphX++) {
    RTok* path = srootP->iceA[morphX]->path;
    srootP->iceA[morphX]->lct = LCT_NIL;
    for (stateX=0;stateX<stateN;stateX++) {
      path[stateX].s = SCORE_MAX;
    }
  }    

  if (srootP->lct2pos) {
    for (morphX=0;morphX<streeP->lct2pos.hashSize;morphX++)
      srootP->lct2pos[morphX]= LPM_NIL;
  }
  srootP->morphN     = 0;
  return;
}

/* ------------------------------------------------------------------------
    srootSort
   ------------------------------------------------------------------------ */

static void riceSwap(SRoot* srootP, int morphX, int morphY)
{
  RIce**   iceA = srootP->iceA;
  RIce*    iceP = iceA[morphX];
  iceA[morphX] = iceA[morphY];  
  iceA[morphY] = iceP;
  return;
}

static void riceQSort(SRoot* srootP, int left, int right)
{
  RIce** iceA = srootP->iceA;
  int     mid = (left+right)/2;
  int i,last;

  if (left >= right) return;

  riceSwap(srootP,left,mid);
  last = left;
  for (i=left+1;i<=right;i++) {
    if (iceA[i]->score < iceA[left]->score) 
      riceSwap(srootP,++last,i);
  }
  riceSwap(srootP,left,last);
  riceQSort(srootP,left,last-1);
  riceQSort(srootP,last+1,right);
  return;
}

void srootSort (SRoot* srootP, STree* streeP)
{
  if (srootP->morphN < 2) return;
  riceQSort(srootP,0,srootP->morphN-1);
  return;
}

/* ------------------------------------------------------------------------
    srootPuts
   ------------------------------------------------------------------------ */

static int srootPutsItf (ClientData clientData, int argc, char *argv[])
{
  SRoot* srootP = (SRoot*) clientData;
  itfAppendResult("{phoneX %d} {xhmmX %d} {lvX %d}",
		  srootP->phoneX,srootP->xhmmX,srootP->lnX);
  return TCL_OK;    
}

/* ------------------------------------------------------------------------
    srootAccess
   ------------------------------------------------------------------------ */

static ClientData srootAccessItf (ClientData clientData, char *name, TypeInfo **ti)
{
  int childX;
  SRoot* srootP = (SRoot*) clientData;
  if (*name == '.') { 
    if (name[1]=='\0') {
      itfAppendElement( "child(%d)",srootP->childN);
      *ti = NULL; return NULL;
    }
    else { 
      if (sscanf(name+1,"child(%d)", &childX) == 1) {
	if (childX >=0 && childX < srootP->childN) {
	  *ti = itfGetType("SNode");
	  return (ClientData)srootP->childPA[childX];
	} else {
	  return NULL;
	}
      }
    }
  }
  return NULL;
}

/* ========================================================================
    Search Node
   ======================================================================== */

/* ------------------------------------------------------------------------
    snodeInit
   ------------------------------------------------------------------------ */

static int snodeInit(SNode* snodeP, STree* streeP, PHMMX hmmX, CTX phoneX, 
		     LEVEL level) 
{
  snodeP->phoneX     = phoneX;
  snodeP->X.hmm      = hmmX;
  snodeP->morphN     = 0;
  snodeP->allocN     = 0;
  snodeP->bestX      = LPM_NIL;
  snodeP->worstX     = LPM_NIL;
  snodeP->iceA.n     = NULL; 
  snodeP->level      = level;
  snodeP->info       = WordIsNotUnique;
  snodeP->childN     = 0;
  snodeP->childPA    = NULL;
  snodeP->vX.s       = SVX_NIL;

  if (level < streeP->smem.level) {
    snodeP->allocN  = streeP->smem.morphBlkN;
    smemAllocNode(snodeP,streeP);
  }

  return TCL_OK;
}

/* ------------------------------------------------------------------------
    snodeCreate
   ------------------------------------------------------------------------ */

static SNode* snodeCreate(STree* streeP, PHMMX hmmX, CTX phoneX, LEVEL level) 
{
  SNode *snodeP = (SNode*) bmemItem(streeP->mem.node);
  
  if (! snodeP) {
    ERROR("snodeCreate: allocation failure\n");
    return NULL;
  }
  if (TCL_ERROR == (snodeInit(snodeP,streeP,hmmX,phoneX,level))) {    
    return NULL;
  }
  return snodeP;
}

/* ------------------------------------------------------------------------
    snodeDeinit
   ------------------------------------------------------------------------ */

static int snodeDeinit(SNode* snodeP,STree* streeP) 
{  
  smemFreeNode(snodeP,streeP);
  return TCL_OK;
}

/* ------------------------------------------------------------------------
    snodeFree
   ------------------------------------------------------------------------ */

static int snodeFree(SNode *snodeP, STree* streeP)
{ 
  if (snodeP) {
    if ( snodeDeinit(snodeP,streeP) != TCL_OK) return TCL_ERROR;
    bmemDelete(streeP->mem.node,(void*)snodeP);
  }
  return TCL_OK;
}

/* ------------------------------------------------------------------------
    snodeDeactivate
   ------------------------------------------------------------------------ */

void snodeDeactivate (SNode* snodeP, STree* streeP)
{
  if (streeP->smem.smemFree == 1) {
    smemDeallocNode(snodeP,streeP);
  } else {
    PHMM*  hmmP = streeP->hmmsP->list.itemA + snodeP->X.hmm;
    int  stateN = hmmP->stateN+1;
    int stateX,morphX;

    for (morphX=0;morphX<snodeP->morphN;morphX++) {
      NTok*    path = snodeP->iceA.n[morphX]->path;
      snodeP->iceA.n[morphX]->lct = LCT_NIL;   
      for (stateX=0;stateX<stateN;stateX++)
	path[stateX].s = SCORE_MAX;
    }
  }
  snodeP->morphN = 0;
  return;
}

/* ------------------------------------------------------------------------
    snodeIsNode
   ------------------------------------------------------------------------ */

static int snodeIsNode(SNode *snodeP)
{
  return (snodeP && snodeP->childN > 0) ? 1 : 0;
}

static int snodeIsNodeItf (ClientData clientData, int argc, char *argv[])
{
  SNode* snodeP = (SNode*) clientData;
  itfAppendResult("%d",snodeIsNode(snodeP));
  return TCL_OK;
}

/* ------------------------------------------------------------------------
    snodeIsLeaf
   ------------------------------------------------------------------------ */

static int snodeIsLeaf(SNode *snodeP)
{
  return (snodeP && ! snodeP->childN == 0) ? 1 : 0;
}

static int snodeIsLeafItf (ClientData clientData, int argc, char *argv[])
{
  SNode* snodeP = (SNode*) clientData;
  itfAppendResult("%d",snodeIsLeaf(snodeP));
  return TCL_OK;
}

/* ------------------------------------------------------------------------
    snodePuts
   ------------------------------------------------------------------------ */

static int snodePutsItf (ClientData clientData, int argc, char *argv[])
{
  SNode* snodeP = (SNode*) clientData;
  
  if (snodeP->childN >0)
    itfAppendResult("{phoneX %d} {hmmX %d} {lvX %d} {level %d} {info %d}",
		    snodeP->phoneX,snodeP->X.hmm, snodeP->vX.l,
		    snodeP->level, snodeP->info);
  else 
    itfAppendResult("{phoneX %d} {xhmmX %d} {svX %d} {level %d} {info %d}",
		    snodeP->phoneX,snodeP->X.xhmm, snodeP->vX.s, 
		    snodeP->level, snodeP->info);
  return TCL_OK;    
}

/* ------------------------------------------------------------------------
    snodeAccess
   ------------------------------------------------------------------------ */

static ClientData snodeAccessItf (ClientData clientData, char *name, TypeInfo **ti)
{
  SNode* snodeP = (SNode*) clientData;
  int    childX = 0;
  if (*name == '.') { 
    if (name[1]=='\0') {
      itfAppendElement( "child(%d)",snodeP->childN);
      *ti = NULL; return NULL;
    }
    else { 
      if (sscanf(name+1,"child(%d)", &childX) == 1) {
	if (childX >= 0 && childX < snodeP->childN) {
	  *ti = itfGetType("SNode");
	  return (ClientData)snodeP->childPA[childX];
	} else {
	  return NULL;
	}
      }
    }
  }
  return NULL;
}

/* ========================================================================
    Search Leaf
   ======================================================================== */

/* ------------------------------------------------------------------------
    sleafInit
   ------------------------------------------------------------------------ */

static int sleafInit(SNode* sleafP, STree* streeP, PHMMX hmmX, 
		     CTX phoneX, SVX svX, LEVEL level) 
{
  sleafP->phoneX     = phoneX;
  sleafP->X.xhmm     = hmmX;
  sleafP->morphN     = 0;
  sleafP->allocN     = 0;
  sleafP->bestX      = LPM_NIL;
  sleafP->worstX     = LPM_NIL;
  sleafP->iceA.l     = NULL;
  sleafP->level      = level;
  sleafP->info       = WordIsNotUnique;
  sleafP->vX.s       = svX;
  sleafP->childN     = 0;
  sleafP->childPA    = NULL;
  return TCL_OK;
}

/* ------------------------------------------------------------------------
    sleafCreate
   ------------------------------------------------------------------------ */

static SNode* sleafCreate(STree* streeP, PHMMX hmmX, CTX phoneX, SVX svX,
			  LEVEL level)
{
  SNode *sleafP = (SNode*) bmemItem(streeP->mem.node);
  
  if (! sleafP) {
    ERROR("sleafCreate: allocation failure\n");
    return NULL;
  }
  if (TCL_ERROR == (sleafInit(sleafP,streeP,hmmX,phoneX,svX,level))) {    
    return NULL;
  }
  return sleafP;
}

/* ------------------------------------------------------------------------
    sleafDeinit
   ------------------------------------------------------------------------ */

static int sleafDeinit(SNode* sleafP, STree* streeP) 
{  
  smemFreeLeaf(sleafP,streeP);
  return TCL_OK;
}

/* ------------------------------------------------------------------------
    sleafFree
   ------------------------------------------------------------------------ */

static int sleafFree(SNode *sleafP, STree* streeP)
{
  if (sleafP) {
    if ( sleafDeinit(sleafP,streeP) != TCL_OK) return TCL_ERROR;
    bmemDelete(streeP->mem.node,(void*) sleafP);
  }
  return TCL_OK;
}

/* ------------------------------------------------------------------------
    sleafDeactivate
   ------------------------------------------------------------------------ */

void sleafDeactivate (SNode* sleafP, STree* streeP)
{
 if (streeP->smem.smemFree == 1) {
    smemDeallocLeaf(sleafP,streeP);
  } else {
    RCM*    rcmP = streeP->rcmsP->list.itemA + sleafP->X.xhmm;
    PHMM*   hmmP = streeP->hmmsP->list.itemA + rcmP->hmmXA[0];
    int   stateN = (hmmP->stateN+1)*rcmP->hmmN;
    int stateX,morphX;

    for (morphX=0;morphX<sleafP->morphN;morphX++) {
      NTok* path = sleafP->iceA.l[morphX]->path;
      sleafP->iceA.l[morphX]->lct = LCT_NIL;
      for (stateX=0;stateX<stateN;stateX++)
	path[stateX].s = SCORE_MAX;
    }
  }
  sleafP->morphN = 0;
  return;
}

/* ========================================================================
    Single Phone Word
   ======================================================================== */

/* We don't need Create/Free methods for Single Phone Words here, 
   because it's organized in streeAddSPhone.
*/

/* ------------------------------------------------------------------------
    siphoneInit
   ------------------------------------------------------------------------ */

static int siphoneInit(SIPhone* sipP, STree* streeP, PHMMX hmmX, 
		       CTX phoneX, SVX svX) 
{
  sipP->phoneX     = phoneX;
  sipP->hmmX       = hmmX;
  sipP->morphN     = 0;
  sipP->allocN     = streeP->smem.morphBlkN;
  sipP->bestX      = LPM_NIL; 
  sipP->worstX     = LPM_NIL; 
  sipP->lct2pos    = NULL;
  sipP->iceA       = NULL;
  sipP->svX        = svX;

  smemAllocSIPhone(sipP,streeP);

  return TCL_OK;
}

/* ------------------------------------------------------------------------
    siphoneDeinit
   ------------------------------------------------------------------------ */

static int siphoneDeinit(SIPhone* sipP, STree* streeP) 
{ 
  smemFreeSIPhone(sipP,streeP);
  return TCL_OK;
}

/* ------------------------------------------------------------------------
    siphoneDeactivate
   ------------------------------------------------------------------------ */

void siphoneDeactivate (SIPhone* sipP, STree* streeP)
{
  PHMM* hmmP   = streeP->hmmsP->list.itemA + sipP->hmmX;
  int   stateN = hmmP->stateN+1;
  int stateX,morphX;

  if (sipP->lct2pos) {
    for (morphX=0;morphX<streeP->lct2pos.hashSize;morphX++) 
      sipP->lct2pos[morphX]= LPM_NIL;
  }

  for (morphX=0;morphX<sipP->morphN;morphX++) {
    NTok* path = sipP->iceA[morphX]->path;
    sipP->iceA[morphX]->lct = LCT_NIL;
    for (stateX=0;stateX<stateN;stateX++)
      path[stateX].s = SCORE_MAX;
  }
  sipP->morphN = 0;
  return;
}

/* ------------------------------------------------------------------------
    siphoneSort
   ------------------------------------------------------------------------ */

static void niceSwap(SIPhone* sipP, int morphX, int morphY)
{
  NIce**  iceA = sipP->iceA;
  NIce*   iceP = iceA[morphX];

  iceA[morphX] = iceA[morphY];  
  iceA[morphY] = iceP;

  return;
}

static void niceQSort(SIPhone* sipP, int left, int right)
{
  NIce** iceA = sipP->iceA;
  int     mid = (left+right)/2;
  int i,last;

  if (left >= right) return;

  niceSwap(sipP,left,mid);
  last = left;
  for (i=left+1;i<=right;i++) {
    if (iceA[i]->score < iceA[left]->score) 
      niceSwap(sipP,++last,i);
  }
  niceSwap(sipP,left,last);
  niceQSort(sipP,left,last-1);
  niceQSort(sipP,last+1,right);
  return;
}

void siphoneSort (SIPhone* sipP, STree* streeP)
{
  if (sipP->morphN < 2) return;
  niceQSort(sipP,0,sipP->morphN-1);
  return;
}

/* ------------------------------------------------------------------------
    sdphoneInit
   ------------------------------------------------------------------------ */

static int sdphoneInit(SDPhone* sdpP, STree* streeP, PHMMX hmmX, 
                       CTX phoneX, SVX svX) 
{
  sdpP->phoneX     = phoneX;
  sdpP->hmmX       = hmmX;
  sdpP->morphN     = 0;
  sdpP->allocN     = streeP->smem.morphBlkN;
  sdpP->bestX      = LPM_NIL;
  sdpP->worstX     = LPM_NIL;
  sdpP->lct2pos    = NULL;
  sdpP->iceA       = NULL;
  sdpP->svX        = svX;

  smemAllocSDPhone(sdpP,streeP);

  return TCL_OK;
}


/* ------------------------------------------------------------------------
    sdphoneDeinit
   ------------------------------------------------------------------------ */

static int sdphoneDeinit(SDPhone* sdpP, STree* streeP) 
{  
  smemFreeSDPhone(sdpP,streeP);
  return TCL_OK;
}

/* ------------------------------------------------------------------------
    sdphoneDeactivate
   ------------------------------------------------------------------------ */

void sdphoneDeactivate (SDPhone* sdpP, STree* streeP)
{
  XCM*    xcmP = streeP->xcmsP->list.itemA + sdpP->hmmX;
  PHMM*   hmmP = streeP->hmmsP->list.itemA + xcmP->hmmXA[0][0];
  int   stateN = (hmmP->stateN+1)*xcmP->hmmMax;
  int stateX,morphX;

  if (sdpP->lct2pos) {
    for (morphX=0;morphX<streeP->lct2pos.hashSize;morphX++)
      sdpP->lct2pos[morphX]=LPM_NIL;
  }
  for (morphX=0;morphX<sdpP->morphN;morphX++) {
    NTok* path = sdpP->iceA[morphX]->path;
    sdpP->iceA[morphX]->lct = LCT_NIL;
    for (stateX=0;stateX<stateN;stateX++)
      path[stateX].s = SCORE_MAX;
  }
  sdpP->morphN = 0;
  return;
}

/* ------------------------------------------------------------------------
    sdphoneSort
   ------------------------------------------------------------------------ */

static void xiceSwap(SDPhone* sdpP, int morphX, int morphY)
{
  XIce**  iceA = sdpP->iceA;
  XIce*   iceP = iceA[morphX];

  iceA[morphX] = iceA[morphY];  
  iceA[morphY] = iceP;

  return;
}

static void xiceQSort(SDPhone* sdpP, int left, int right)
{
  XIce** iceA = sdpP->iceA;
  int     mid = (left+right)/2;
  int i,last;

  if (left >= right) return;

  xiceSwap(sdpP,left,mid);
  last = left;
  for (i=left+1;i<=right;i++) {
    if (iceA[i]->score < iceA[left]->score) 
      xiceSwap(sdpP,++last,i);
  }
  xiceSwap(sdpP,left,last);
  xiceQSort(sdpP,left,last-1);
  xiceQSort(sdpP,last+1,right);
  return;
}

void sdphoneSort (SDPhone* sdpP, STree* streeP)
{
  if (sdpP->morphN < 2) return;
  xiceQSort(sdpP,0,sdpP->morphN-1);
  return;
}

/* ========================================================================
    Search Tree
   ======================================================================== */

/* ------------------------------------------------------------------------
    forward declarations 
   ------------------------------------------------------------------------ */

extern TypeInfo  streeInfo;
static STree     streeDefault;

static int streeBuildTree(STree* streeP);
static int streeReadDump (STree* streeP,char* filename, int verbose);
static int streeInitStart(STree* streeP);

/* ------------------------------------------------------------------------
    streeInit
   ------------------------------------------------------------------------ */

static int streeInit(STree* streeP, char* name, SVMap* svmapP, 
		     LCMSet* lcmsP, RCMSet* rcmsP, XCMSet* xcmsP,
		     char* filename, int level, int morphBlk, int smemFree,
		     int verbose)
{
  if (NULL == svmapP) {
    ERROR("streeInit: no Vocabulary Mapper\n");
    return TCL_ERROR;
  }
  if (NULL == svmapP->svocabP) {
    ERROR("streeInit: no Search Vocabulary\n");
    return TCL_ERROR;
  }
  if (NULL == svmapP->svocabP->dictP) {
    ERROR("streeInit: no Dictionary\n");
    return TCL_ERROR;
  }
  if (NULL == lcmsP || NULL == rcmsP) {
    ERROR("streeInit: no Context Models\n");
    return TCL_ERROR;
  }
  if (lcmsP->hmmsP != rcmsP->hmmsP)  {
    ERROR("streeInit: Context Models must be defined over the same PHMMSet\n");
    return TCL_ERROR;
  }
    
  streeP->name    = strdup(name);
  streeP->useN    = 0;
  streeP->svmapP  = svmapP;
  streeP->svocabP = svmapP->svocabP;
  streeP->dictP   = svmapP->svocabP->dictP;
  streeP->lcmsP   = lcmsP;
  streeP->rcmsP   = rcmsP;
  streeP->xcmsP   = xcmsP;
  streeP->hmmsP   = lcmsP->hmmsP;

  streeP->compress = 0;

  link(streeP->svmapP,"SVMap");
  link(streeP->lcmsP, "LCMSet");
  link(streeP->rcmsP, "RCMSet");
  if (streeP->xcmsP)
    link(streeP->xcmsP,"XCMSet");

  streeP->rootTbl.N       = 0;
  streeP->rootTbl.allocN  = 0;
  streeP->rootTbl.blkSize = streeDefault.rootTbl.blkSize;
  streeP->rootTbl.PA      = NULL;

  streeP->nodeTbl.nodeN   = 0;
  streeP->nodeTbl.leafN   = 0;
  streeP->nodeTbl.N       = 0;
  streeP->nodeTbl.A       = NULL;
  streeP->nodeTbl.typeA   = NULL;

  streeP->startTbl.N     = 0; 
  streeP->startTbl.A     = NULL;
  streeP->startTbl.typeA = NULL;
  streeP->startTbl.svX   = SVX_NIL;

  streeP->sTbl.iN         = 0;
  streeP->sTbl.dN         = 0;
  streeP->sTbl.iA         = NULL;
  streeP->sTbl.dA         = NULL;

  streeP->smem.streeP     = streeP;
  streeP->smem.level      = level;
  streeP->smem.morphBlkN  = morphBlk;
  streeP->smem.smemFree   = smemFree;

  streeP->lct2pos.hashSize = streeDefault.lct2pos.hashSize;
  streeP->lct2pos.bitmask  = streeDefault.lct2pos.bitmask;
  streeP->lct2pos.offset   = streeDefault.lct2pos.offset;
  streeP->lct2pos.tryMax   = streeDefault.lct2pos.tryMax;

  smemInit(&(streeP->smem));

  streeP->mem.root = bmemCreate(400,sizeof(SRoot));
  streeP->mem.node = bmemCreate(2000,sizeof(SNode));
  streeP->mem.ptr  = bmemCreate(4000,0);

  if (filename) {
    if (TCL_OK != streeReadDump(streeP,filename,verbose)) {
      ERROR("streeInit: couldn't read dump file\n");
      return TCL_ERROR;
    }
  } else {
    if (TCL_OK != svmapCheck(svmapP))  {
      ERROR("streeInit: Invalid Vocabulary Mapper\n");
      return TCL_ERROR;
    }
    if (TCL_OK != streeBuildTree(streeP)) {
      ERROR("streeInit: couldn't build search network\n");
      return TCL_ERROR;
    }
  }

  return TCL_OK;
}

/* ------------------------------------------------------------------------
    streeCreate
   ------------------------------------------------------------------------ */

static STree* streeCreate(char* name, SVMap* svmapP, LCMSet* lcmsP, 
			  RCMSet* rcmsP,XCMSet* xcmsP,char* filename,
			  int level, int morphBlk, int smemFree, int verbose)
{
  STree* streeP = (STree*) malloc(sizeof(STree));

  if (NULL == streeP) {
    ERROR("streeCreate: allocation failure\n");
    return NULL;
  }
  if (TCL_ERROR == (streeInit(streeP,name,svmapP,lcmsP,rcmsP,xcmsP,filename,
			      level,morphBlk,smemFree,verbose))) {
    free(streeP);
    return NULL;
  }
  return streeP;
}

static ClientData streeCreateItf( ClientData clientData, 
				  int argc, char *argv[])
{
  SVMap*  svmapP = NULL;
  LCMSet*  lcmsP = NULL;
  RCMSet*  rcmsP = NULL;
  XCMSet*  xcmsP = NULL;
  char*     name = NULL;
  char*    dname = NULL;
  int      level = streeDefault.smem.level;
  int   morphBlk = streeDefault.smem.morphBlkN;
  int   smemFree = streeDefault.smem.smemFree;
  int    verbose = 0;

  if ( itfParseArgv( argv[0], &argc, argv, 0,
      "<name>",  ARGV_STRING, NULL, &name, NULL,         "name of the search tree",
      "<SVMap>",  ARGV_OBJECT, NULL, &svmapP, "SVMap",  \
		     "Vocabulary Mapper",
      "<LCMSet>", ARGV_OBJECT, NULL, &lcmsP,  "LCMSet", \
		     "Set of left context models",
      "<RCMSet>", ARGV_OBJECT, NULL, &rcmsP,  "RCMSet", \
		     "Set of right context models",
      "-XCMSet",  ARGV_OBJECT, NULL, &xcmsP,  "XCMSet", \
		     "Set of left and right context models",
      "-dump",    ARGV_STRING, NULL, &dname,  "", \
		     "Search Tree dump file",
      "-level",   ARGV_INT, NULL, &level,  "",   \
		     "tree level for memory management",
      "-morphBlk",ARGV_INT, NULL, &morphBlk,  "",   \
 		     "block size for memory management",
      "-smemFree",ARGV_INT, NULL, &smemFree,  "",   \
		     "memory management mode",
      "-v",       ARGV_INT, NULL, &verbose,  "",   \
		     "verbose tree dump",

      NULL) != TCL_OK) return NULL;
  
  return (ClientData) streeCreate(name,svmapP,lcmsP,rcmsP,xcmsP,dname,
				  level,morphBlk,smemFree,verbose);
}

/* ------------------------------------------------------------------------
    streeDeinit
   ------------------------------------------------------------------------ */

static int streeDeinit(STree* streeP) 
{
  int childX,rootX,sipX,sdpX;

  for (rootX=0;rootX<streeP->rootTbl.N;rootX++) {
    SRoot* srootP = streeP->rootTbl.PA[rootX];
    for (childX=0;childX<srootP->childN;childX++)
      snodeFree(srootP->childPA[childX],streeP);
    srootFree(srootP,streeP);
  }

  for (sipX=0;sipX<streeP->sTbl.iN;sipX++) {
    siphoneDeinit(streeP->sTbl.iA+sipX,streeP);
  }
  for (sdpX=0;sdpX<streeP->sTbl.dN;sdpX++) {
    sdphoneDeinit(streeP->sTbl.dA+sdpX,streeP);
  }

  delink(streeP->svmapP, "SVMap");
  delink(streeP->lcmsP,  "LCMSet");
  delink(streeP->rcmsP,  "RCMSet");
  if (streeP->xcmsP)
    delink(streeP->xcmsP,"XCMSet");

  if (streeP->name)           free(streeP->name);
  if (streeP->rootTbl.PA)     free(streeP->rootTbl.PA); 
  if (streeP->nodeTbl.A)      free(streeP->nodeTbl.A); 
  if (streeP->nodeTbl.typeA)  free(streeP->nodeTbl.typeA); 
  if (streeP->startTbl.A)     free(streeP->startTbl.A); 
  if (streeP->startTbl.typeA) free(streeP->startTbl.typeA); 
  if (streeP->sTbl.iA)        free(streeP->sTbl.iA); 
  if (streeP->sTbl.dA)        free(streeP->sTbl.dA); 

  streeP->name            = NULL;  
  streeP->svmapP          = NULL;
  streeP->svocabP         = NULL;
  streeP->dictP           = NULL;
  streeP->hmmsP           = NULL;
  streeP->lcmsP           = NULL;
  streeP->rcmsP           = NULL;
  streeP->xcmsP           = NULL;

  streeP->rootTbl.N       = 0;
  streeP->rootTbl.allocN  = 0;
  streeP->rootTbl.blkSize = 0;
  streeP->rootTbl.PA      = NULL;

  streeP->nodeTbl.A       = NULL;
  streeP->nodeTbl.N       = 0;
  streeP->nodeTbl.typeA   = NULL;

  streeP->startTbl.A      = NULL;
  streeP->startTbl.N      = 0;
  streeP->startTbl.typeA  = NULL;

  streeP->sTbl.iA         = NULL;
  streeP->sTbl.dA         = NULL;

  smemDeinit(&(streeP->smem));

  bmemFree(streeP->mem.root);
  bmemFree(streeP->mem.node);
  bmemFree(streeP->mem.ptr);

  return TCL_OK;
}

/* ------------------------------------------------------------------------
    streeFree
   ------------------------------------------------------------------------ */

static int streeFree(STree* streeP) 
{
  if (streeP) {
    if ( streeDeinit(streeP) != TCL_OK) return TCL_ERROR;
    free(streeP);
  }
  return TCL_OK;
}
 
static int streeFreeItf (ClientData clientData)
{
  return streeFree((STree*)clientData);
}

/* ------------------------------------------------------------------------
    streePuts
   ------------------------------------------------------------------------ */

static int streePutsItf (ClientData clientData, int argc, char *argv[])
{
  return TCL_OK;    
}

/* ------------------------------------------------------------------------
    streeAccess
   ------------------------------------------------------------------------ */

static ClientData streeAccessItf (ClientData clientData, char *name, TypeInfo **ti)
{
  STree* streeP = (STree*) clientData;
  if (*name == '.') { 
    if (name[1]=='\0') {
      itfAppendElement( "svMap");
      itfAppendElement( "lcmSet");
      itfAppendElement( "rcmSet");
      if (streeP->xcmsP)
	itfAppendElement( "xcmSet");
      itfAppendElement( "smem");
      itfAppendElement( "root(0..%d)",streeP->rootTbl.N-1);
      *ti = NULL; return NULL;
    }
    else { 
      if (! strcmp( name+1, "svMap")) {
        *ti = itfGetType("SVMap");
        return (ClientData)streeP->svmapP;
      }
      if (! strcmp( name+1, "lcmSet")) {
        *ti = itfGetType("LCMSet");
        return (ClientData)streeP->lcmsP;
      }
      if (! strcmp( name+1, "rcmSet")) {
        *ti = itfGetType("RCMSet");
        return (ClientData)streeP->rcmsP;
      } 
      if (streeP->xcmsP && ! strcmp( name+1, "xcmSet")) {
        *ti = itfGetType("XCMSet");
        return (ClientData)streeP->xcmsP;
      } 
      if (! strcmp( name+1, "smem")) {
        *ti = itfGetType("SMem");
        return (ClientData)&(streeP->smem);
      } 
      else {
	int i = 0;
	if (sscanf(name+1,"root(%d)", &i) == 1) {
	  *ti = &srootInfo;
	  if (i>=0 && i<streeP->rootTbl.N) 
	    return (ClientData) (streeP->rootTbl.PA[i]);
	  else return NULL;
	}
      }
    }
  }
  return NULL;
}

/* ------------------------------------------------------------------------
    streeAdd
   ------------------------------------------------------------------------ */

static int streeAdd(STree* streeP,SVX fromX, SVX toX, int* rootN,\
		    int* nodeN, int* leafN) 
{
  SVX          svN = streeP->svocabP->list.itemN;
  int       phoneN = streeP->dictP->phones->list.itemN;
  PHMMX*     hmmXA = NULL;
  SRoot**    diMap = NULL;

  Word xword;
  SVX svX;
  int rootX,phoneX,diphoneX,diphoneMax,diphoneOffset;

  /* alloc map SWord -> SLeaf/Siphone/Sdphone */
  if (svN > (SVX) streeP->nodeTbl.N) {   
    if (NULL == (streeP->nodeTbl.A = (void**)
		 realloc(streeP->nodeTbl.A,svN*sizeof(void*)))) {
      ERROR("streeAdd: allocation failure\n");
      return TCL_ERROR;
    }
    if (NULL == (streeP->nodeTbl.typeA = (UCHAR*)
		 realloc(streeP->nodeTbl.typeA,svN*sizeof(UCHAR)))) {
      ERROR("streeAdd: allocation failure\n");
      return TCL_ERROR;
    }
    for (svX=streeP->nodeTbl.N;svX<svN;svX++) {
      streeP->nodeTbl.A[svX]     = NULL;
      streeP->nodeTbl.typeA[svX] = type_NIL;
    }
    streeP->nodeTbl.N = svN;
  }

  for (svX=fromX;svX<=toX;svX++) {
    streeP->nodeTbl.A[svX]     = NULL;
    streeP->nodeTbl.typeA[svX] = type_NIL;
  }

  /* build tmp. diphone mapper : diphone -> sroot */
  diphoneOffset = streeP->lcmsP->list.itemN;
  diphoneMax    = diphoneOffset+toX-fromX+1;
  if (NULL == (diMap= (SRoot**) malloc (sizeof(SRoot*) * diphoneMax))) {
    ERROR("streeAdd: allocation failure\n");
    return TCL_ERROR;
  }

  for (diphoneX=0; diphoneX<diphoneMax; diphoneX++) {
    diMap[diphoneX] = NULL;
  }
  for (rootX=0; rootX < streeP->rootTbl.N;rootX++) {
    diMap[ streeP->rootTbl.PA[rootX]->xhmmX ] = streeP->rootTbl.PA[rootX];
  }

  /* build tmp. crossword (needed for phmmSetAdd) */
  if (NULL == (xword.phoneA = (int*) malloc (sizeof(int)*(LEVEL_MAX+1)))) {
    ERROR("streeAdd: allocation failure\n");
    return TCL_ERROR;
  }
  if (NULL == (xword.tagA   = (int*) malloc (sizeof(int)*(LEVEL_MAX+1)))) {
    ERROR("streeAdd: allocation failure\n");
    return TCL_ERROR;
  }
  
  /* build tmp. hmmXA (needed for lcmSetAdd) */
  if (NULL == (hmmXA = (PHMMX*) malloc(phoneN*sizeof(PHMMX)))) {
    ERROR("streeAdd: allocation failure\n");
    return TCL_ERROR;
  }

  for (svX=fromX;svX<=toX;svX++) {
    SRoot*    srootP = NULL;
    SNode*    snodeP = NULL;
    SNode*    sleafP = NULL;
    SNode*     lastP = NULL;
    SWord*    swordP = streeP->svocabP->list.itemA + svX;
    DictWord* dwordP = NULL;
    int         segN = 0;
    int       childX = 0;
    int       childN = 0;
    SNode**  childPA = NULL;
 
    /* ignore search words without pronunciation */
    if (swordP->dictX == SVX_MAX) continue;

    dwordP = streeP->dictP->list.itemA   + swordP->dictX;
    segN   = dwordP->word.itemN;

    /* ignore single phone words */
    if (segN == 1) continue;

    if (segN >= LEVEL_MAX) {
      WARN("streeAdd: %s exceeded max. number of phones (%d)\n",
      swordP->name,segN);
      continue;
    }

    if (!swordP->fillerTag && svmapGet (streeP->svmapP, svX) == LVX_NIL) {
	  WARN("streeAdd: couldn't find lmword for %s\n",swordP->name);
	  continue;
    }

    /* --------------------------------------------- */
    /* 1. compute diphone index :                    */
    /*    left context model index for a given sword */
    /*    and all possible ending phones.            */
    /* --------------------------------------------- */
    xword.phonesP = dwordP->word.phonesP;
    xword.tagsP   = dwordP->word.tagsP;
    xword.itemN   = segN+1;
    xword.tagA[0] = streeP->dictP->weTags;

    memcpy(xword.phoneA+1,dwordP->word.phoneA,sizeof(int)*dwordP->word.itemN);
    memcpy(xword.tagA+1,  dwordP->word.tagA,  sizeof(int)*dwordP->word.itemN);
    
    for (phoneX=0;phoneX<phoneN;phoneX++) {
      xword.phoneA[0] = phoneX;
      hmmXA[phoneX]   = (PHMMX) phmmSetAdd(streeP->hmmsP,&xword,
					   -1,segN-1,hmmEvalFnRoot);
    }
    diphoneX = (PHMMX) lcmSetAdd(streeP->lcmsP,(CTX)phoneN,hmmXA);

    /* --------------------------------------------- */
    /* 2. create root                                */
    /* --------------------------------------------- */
    if (diMap[diphoneX] == NULL) {
      srootP = srootCreate(streeP,diphoneX,(CTX)(dwordP->word.phoneA[0]));    
      diMap[diphoneX] = srootP;
      (*rootN)++;
    } else {
      srootP = diMap[diphoneX];
    }

    childN  = srootP->childN;
    childPA = srootP->childPA;

    /* ---------------------------------------------- */
    /* 3. create nodes                                */
    /* ---------------------------------------------- */
    for (phoneX= 1;phoneX< segN-1; phoneX++) {
      PHMMX hmmX = (PHMMX)phmmSetAdd(streeP->hmmsP,&(dwordP->word),
				     -phoneX,segN-phoneX-1,hmmEvalFnNode);

      /* don't share nodes in compressed networks */
      if (streeP->compress) 
	childX = childN;
      else
	childX = 0;
      while ( childX < childN && 
	      ! (childPA[childX]->X.hmm == hmmX && childPA[childX]->childN >0)) childX++;

      /* case 1: there is aleady a valid child */
      if (childN > 0 && childX < childN) {
	lastP   = childPA[childX];
	childN  = lastP->childN;
	childPA = lastP->childPA;
	continue;
      }

      snodeP = snodeCreate(streeP,hmmX,(CTX)(dwordP->word.phoneA[phoneX]),
			   (LEVEL) phoneX);
      (*nodeN)++;
      streeP->nodeTbl.nodeN++;

      /* case 2a : create new node directly from root */
      if (phoneX == 1) {
	SNode** tmpA = bmemAlloc(streeP->mem.ptr,(srootP->childN+1)*sizeof(SNode*));
	SNode** tmpB = srootP->childPA;
	if (srootP->childN>0) {
	  memcpy(tmpA,srootP->childPA,srootP->childN*sizeof(SNode*));
	  bmemDealloc(streeP->mem.ptr,tmpB,srootP->childN*sizeof(SNode*));
	}
	srootP->childPA = tmpA;
	srootP->childPA[srootP->childN++] = snodeP;
      } else {
	/* case 2b : create new node from last node */
	SNode** tmpA = bmemAlloc(streeP->mem.ptr,(lastP->childN+1)*sizeof(SNode*));
	SNode** tmpB = lastP->childPA;
	if (lastP->childN>0) {
	  memcpy(tmpA,lastP->childPA,lastP->childN*sizeof(SNode*));
	  bmemDealloc(streeP->mem.ptr,tmpB,lastP->childN*sizeof(SNode*));
	}
	lastP->childPA = tmpA;
	lastP->childPA[lastP->childN++] = snodeP;
      }
      lastP   = snodeP;
      childN  = lastP->childN;
      childPA = lastP->childPA;
    }      

    /* --------------------------------------------- */
    /* 4. compute diphone index :                    */
    /*    right context model index for a given      */
    /*    sword and all possible starting phones.    */
    /* --------------------------------------------- */
    xword.phonesP    = dwordP->word.phonesP;
    xword.tagsP      = dwordP->word.tagsP;
    xword.itemN      = segN+1;
    xword.tagA[segN] = streeP->dictP->wbTags;

    memcpy(xword.phoneA,dwordP->word.phoneA,sizeof(int)*dwordP->word.itemN);
    memcpy(xword.tagA,  dwordP->word.tagA,  sizeof(int)*dwordP->word.itemN);
    
    for (phoneX=0;phoneX<phoneN;phoneX++) {
      xword.phoneA[segN] = phoneX;
      hmmXA[phoneX]   = (PHMMX) phmmSetAdd(streeP->hmmsP,&xword,
					   -segN+1,1,hmmEvalFnLeaf);
    }
    diphoneX = (PHMMX) rcmSetAdd(streeP->rcmsP,(CTX)phoneN,hmmXA);

    /* --------------------------------------------- */
    /* 5. create leaf                                */
    /* --------------------------------------------- */

    sleafP = sleafCreate(streeP,diphoneX,(CTX)(dwordP->word.phoneA[segN-1]),svX,(LEVEL)(segN-1));        

    /* don't propagate word id's for compressed networks */
    if (streeP->compress)
      sleafP->info = WordIsUnique;

    streeP->nodeTbl.A[svX]     = (void*) sleafP;
    streeP->nodeTbl.typeA[svX] = type_SLeaf;
    (*leafN)++;
    streeP->nodeTbl.leafN++;

    if (segN == 2) {
      SNode** tmpA = bmemAlloc(streeP->mem.ptr,(srootP->childN+1)*sizeof(SNode*));
      SNode** tmpB = srootP->childPA;
      if (srootP->childN>0) {
	memcpy(tmpA,srootP->childPA,srootP->childN*sizeof(SNode*));
	bmemDealloc(streeP->mem.ptr,tmpB,srootP->childN*sizeof(SNode*));
      }
      srootP->childPA = tmpA;
      srootP->childPA[srootP->childN++] = sleafP;
    } else {
      assert (lastP);
      SNode** tmpA = bmemAlloc(streeP->mem.ptr,(lastP->childN+1)*sizeof(SNode*));
      SNode** tmpB = lastP->childPA;
      if (lastP->childN>0) {
	memcpy(tmpA,lastP->childPA,lastP->childN*sizeof(SNode*));
	bmemDealloc(streeP->mem.ptr,tmpB,lastP->childN*sizeof(SNode*));
      }
      lastP->childPA = tmpA;
      lastP->childPA[lastP->childN++] = sleafP;      
    }
  }

  if (diMap)        free(diMap);
  if (hmmXA)        free(hmmXA);  
  if (xword.tagA)   free(xword.tagA);
  if (xword.phoneA) free(xword.phoneA);

  return TCL_OK;
}

/* ------------------------------------------------------------------------
   streeAddSPhone
   ------------------------------------------------------------------------ */

static int streeAddSPhone(STree* streeP,SVX fromX, SVX toX, int* iN, int* dN) 
{
  SVX          svN = streeP->svocabP->list.itemN;
  int       phoneN = streeP->dictP->phones->list.itemN;
  int     padPhone = streeP->hmmsP->treeP->questions.padPhoneX;
  int rightContext = 1;
  int      sphoneN = 0;
  PHMMX**    hmmXA = NULL;
  Word       xword;
  SVX          svX;
  int phoneX,phoneY,spX;

  if (svN < toX) {
    ERROR("streeAddSPhone: wrong svocab indices\n");
    return TCL_ERROR;
  }

  /* check if right context is wanted */ 
  if (streeP->xcmsP && streeP->xcmsP->ignoreRCM) {
    rightContext = 0;
  }

  /* count single phone words and allocate memory */
  for (svX=fromX;svX<=toX;svX++) {
    SWord*    swordP = streeP->svocabP->list.itemA + svX;

    /* ignore search words without pronunciation */
    if (swordP->dictX != SVX_MAX) {
      DictWord* dwordP = streeP->dictP->list.itemA   + swordP->dictX;
      int         segN = dwordP->word.itemN;
      if (segN == 1) sphoneN++;
    }
  }

  if (sphoneN == 0) {
    return TCL_OK;
  }

  if (streeP->sTbl.iN > streeP->sTbl.dN)
    sphoneN += streeP->sTbl.iN;
  else
    sphoneN += streeP->sTbl.dN;

  if (NULL == (streeP->sTbl.iA = (SIPhone*) 
	       realloc(streeP->sTbl.iA,sphoneN*sizeof(SIPhone)))) {
    ERROR("streeAddSPhone: allocation failure\n");
    return TCL_ERROR;
  }
  if (NULL == (streeP->sTbl.dA = (SDPhone*) 
	       realloc(streeP->sTbl.dA,sphoneN*sizeof(SDPhone)))) {
    ERROR("streeAddSPhone: allocation failure\n");
    return TCL_ERROR;
  }

  /* build tmp. crossword (needed for phmmSetAdd) */
  if (NULL == (xword.phoneA = (int*) malloc (sizeof(int)*(LEVEL_MAX+1)))) {
    ERROR("streeAddSPhone: allocation failure\n");
    return TCL_ERROR;
  }
  if (NULL == (xword.tagA   = (int*) malloc (sizeof(int)*(LEVEL_MAX+1)))) {
    ERROR("streeAddSPhone: allocation failure\n");
    return TCL_ERROR;
  }
  
  /* dual map for xhmm's */
  if (NULL == (hmmXA= malloc(phoneN*sizeof(PHMMX*)))) {
    ERROR("streeAddSPhone: allocation failure\n");
    return TCL_ERROR;
  }
  if (NULL == (hmmXA[0]= malloc(phoneN*phoneN*sizeof(PHMMX)))) {
    ERROR("streeAddSPhone: allocation failure\n");
    return TCL_ERROR;
  }
  for (phoneX=1;phoneX<phoneN;phoneX++)
    hmmXA[phoneX]=hmmXA[phoneX-1] + phoneN;

  /* create single phone words */
  for (svX=fromX;svX<=toX;svX++) {
    SIPhone*    sipP = streeP->sTbl.iA + streeP->sTbl.iN;
    SDPhone*    sdpP = streeP->sTbl.dA + streeP->sTbl.dN;
    SWord*    swordP = streeP->svocabP->list.itemA + svX;
    DictWord* dwordP = NULL;
    int         segN = 0;
    int         hmmX = 0;

    /* ignore search words without pronunciation */
    if (swordP->dictX == SVX_MAX) continue;

    dwordP = streeP->dictP->list.itemA   + swordP->dictX;
    segN = dwordP->word.itemN;

    if (segN != 1) continue;

    if (!swordP->fillerTag && svmapGet (streeP->svmapP, svX) == LVX_NIL) {
      WARN("streeAddSPhone: couldn't find lmword for %s\n", swordP->name);
      continue;
    }

    /* left and right context dependency check */
    if (streeP->xcmsP) {
      xword.phonesP   = dwordP->word.phonesP;
      xword.tagsP     = dwordP->word.tagsP;
      xword.itemN     = 3;
      xword.tagA[0]   = streeP->dictP->weTags;
      xword.tagA[2]   = streeP->dictP->wbTags;
      xword.tagA[1]   = dwordP->word.tagA[0];
      xword.phoneA[1] = dwordP->word.phoneA[0];

      hmmX = -1;

      /* for each left context phoneX */
      for (phoneX=0;phoneX<phoneN;phoneX++) {

	for (phoneY=0;phoneY<phoneN;phoneY++)
	  hmmXA[phoneX][phoneY] = PHMM_MAX;

	if (phoneX == padPhone) continue;
	xword.phoneA[0] = phoneX;

	/* for each right context phoneY */
	for (phoneY=0;phoneY<phoneN;phoneY++) {
	  if (phoneY == padPhone) continue;
	  xword.phoneA[2] = phoneY;
	  hmmXA[phoneX][phoneY] = (PHMMX) phmmSetAdd(streeP->hmmsP,&xword,
						     -1,rightContext,hmmEvalFnSdp);
	  if (hmmX == -1) 
	    hmmX = hmmXA[phoneX][phoneY];
	  else
	    if (hmmX != (int)(hmmXA[phoneX][phoneY])) hmmX = -2;
	}
      }

      /* add counter for context in/dependent models */
      if (hmmX >=  0) (*iN)++;
      if (hmmX == -2) (*dN)++;

      if (hmmX == -1) {
	WARN("streeAddSPhone: couldn't find acoustic models for %s\n",swordP->name);
	continue;
      }
    } else {
      (*iN)++;
    }
     if (streeP->xcmsP) {
      if (hmmX == -2) {
	/* get context dependent model */
	hmmX = (PHMMX) xcmSetAdd(streeP->xcmsP,(CTX)phoneN,(CTX)phoneN,hmmXA);
	streeP->sTbl.dN++;
	sdphoneInit(sdpP,streeP,hmmX,(CTX)(dwordP->word.phoneA[0]),svX);
      } else {
	/* get context dependent model - but there is only one unique model */
	hmmX = (PHMMX) phmmSetAdd(streeP->hmmsP,&xword,
				  -1,rightContext,hmmEvalFnSip);
	streeP->sTbl.iN++;
	siphoneInit(sipP,streeP,hmmX,(CTX)(dwordP->word.phoneA[0]),svX);
      }
    } else {
      /* get context independent model */
      hmmX = (PHMMX) phmmSetAdd(streeP->hmmsP,&(dwordP->word),
			      0,0,hmmEvalFnSip);  
      streeP->sTbl.iN++;
      siphoneInit(sipP,streeP,hmmX,(CTX)(dwordP->word.phoneA[0]),svX);
    }
 
 }

  if (streeP->sTbl.iN == 0) {
    if (streeP->sTbl.iA) free(streeP->sTbl.iA);
    streeP->sTbl.iA = NULL;
  } else 
    /* reduce oversized memory block */
    streeP->sTbl.iA = (SIPhone*) 
      realloc(streeP->sTbl.iA,streeP->sTbl.iN*sizeof(SIPhone));
  
  if (streeP->sTbl.dN == 0) {
    if (streeP->sTbl.dA) free(streeP->sTbl.dA);
    streeP->sTbl.dA = NULL;
  } else 
    /* reduce oversized memory block */
    streeP->sTbl.dA = (SDPhone*) 
      realloc(streeP->sTbl.dA,streeP->sTbl.dN*sizeof(SDPhone));
  
  if (xword.phoneA) free(xword.phoneA);
  if (xword.tagA)   free(xword.tagA);

  if (hmmXA) {
    if (hmmXA[0]) free(hmmXA[0]);
    free(hmmXA);
  }

  /* update node table after single phone table reallocation */
  for (spX=0;spX<streeP->sTbl.iN;spX++) {
    svX = streeP->sTbl.iA[spX].svX;
    streeP->nodeTbl.A[svX]     = (void*) (streeP->sTbl.iA + spX);
    streeP->nodeTbl.typeA[svX] = type_SIPhone;
  }
  for (spX=0;spX<streeP->sTbl.dN;spX++) {
    svX = streeP->sTbl.dA[spX].svX;
    streeP->nodeTbl.A[svX]     = (void*) (streeP->sTbl.dA + spX);
    streeP->nodeTbl.typeA[svX] = type_SDPhone;
  }

  return TCL_OK;

}

/* ------------------------------------------------------------------------
   streeAddPronGraph : SIPhone with given PHMM Index
   ------------------------------------------------------------------------ */

static int streeAddPronGraph(STree* streeP,SVX svX, PHMMX phmmX) 
{
  SVX          svN = streeP->svocabP->list.itemN;
  int          lvX = svmapGet(streeP->svmapP,svX);
  SWord*    swordP = streeP->svocabP->list.itemA + svX;
  DictWord* dwordP = NULL;
  SIPhone*    sipP = NULL;
  int spX;

  /* check dictionary */
  if (swordP->dictX == SVX_MAX) { 
    WARN("streeAddWord: no dictionary entry for %s\n",swordP->name);
    return TCL_OK;
  }
  
  dwordP = streeP->dictP->list.itemA + swordP->dictX;
  if (dwordP->word.itemN != 1) {
    WARN("streeAddWord: inconsistent dictionary for pron. network %s\n",swordP->name);
    return TCL_OK;
  }
  /* check svmap */
  if (lvX == LVX_NIL && swordP->fillerTag == 0) {
    WARN("streeAddPronGraph: couldn't find lmword for %s\n",swordP->name);
    return TCL_OK;
  }
    
  /* create new siphone */
  streeP->sTbl.iN++;
  if (NULL == (streeP->sTbl.iA = (SIPhone*) 
	       realloc(streeP->sTbl.iA,streeP->sTbl.iN*sizeof(SIPhone)))) {
    ERROR("streeAddPronGraph: allocation failure\n");
    return TCL_ERROR;
  }
  sipP = streeP->sTbl.iA + streeP->sTbl.iN-1;
  
  /* init new siphone */
  siphoneInit(sipP,streeP,phmmX,(CTX)(dwordP->word.phoneA[0]),svX);
  
  /* realloc map SWord -> SLeaf/Siphone/Sdphone */
  if (svN > (SVX) streeP->nodeTbl.N) {   
    SVX svY;
    if (NULL == (streeP->nodeTbl.A = (void**)
		 realloc(streeP->nodeTbl.A,svN*sizeof(void*)))) {
      ERROR("streeAddPronGraph: allocation failure\n");
      return TCL_ERROR;
    }
    if (NULL == (streeP->nodeTbl.typeA = (UCHAR*)
		 realloc(streeP->nodeTbl.typeA,svN*sizeof(UCHAR)))) {
      ERROR("streeAddPronGraph: allocation failure\n");
      return TCL_ERROR;
    }
    for (svY=(SVX)streeP->nodeTbl.N;svY<svN;svY++) {
      streeP->nodeTbl.A[svY]     = NULL;
      streeP->nodeTbl.typeA[svY] = type_NIL;
    }
    streeP->nodeTbl.N = svN;
  }
  
  /* update node table after single phone table reallocation */
  for (spX=0;spX<streeP->sTbl.iN;spX++) {
    svX = streeP->sTbl.iA[spX].svX;
    streeP->nodeTbl.A[svX]     = (void*) (streeP->sTbl.iA + spX);
    streeP->nodeTbl.typeA[svX] = type_SIPhone;
  }
  for (spX=0;spX<streeP->sTbl.dN;spX++) {
    svX = streeP->sTbl.dA[spX].svX;
    streeP->nodeTbl.A[svX]     = (void*) (streeP->sTbl.dA + spX);
    streeP->nodeTbl.typeA[svX] = type_SDPhone;
  }

  if (TCL_ERROR == (streeInitStart(streeP))) {
    return TCL_ERROR;
  }  

  return TCL_OK;
}

/* ------------------------------------------------------------------------
   streeExtractUnqiueInfo
   ------------------------------------------------------------------------ */

/* set the UniqueFlag for :
   - leafs that do not cover more than one words (same pronunciation)
   - nodes with descendants with fan-out of 1
     and propagate the sword index from the leafs to the unqiue nodes to
     feature an early exact lm score access.
*/
static int snodeIsUnique(SNode* snodeP, STree* streeP) 
{
  int   childN = snodeP->childN;
  snodeP->info = WordIsNotUnique;

  /* no child */
  if (childN == 0) {
    snodeP->info = WordIsUnique;
    return snodeP->info;
  }

  /* only one child */
  if (childN == 1) {
    int childFlag = snodeIsUnique(snodeP->childPA[0],streeP);
    if (childFlag == WordIsUnique) {
      snodeP->info = WordIsUnique;
      snodeP->vX.s = snodeP->childPA[0]->vX.s;
    }
    return childFlag;
  } 
  /* more than one child */
  childN--;
  while (childN>=0) {
    snodeIsUnique(snodeP->childPA[childN],streeP);
    childN--;
  }
  return snodeP->info;
}

/* set the WordWasUnique flag for nodes with unique parents 
   to avoid multiple exact lm score calls.
*/

static int snodeWasUnique(SNode* snodeP, int parentFlag) 
{
  int childFlag = parentFlag;
  int    childN = snodeP->childN-1;

  if (parentFlag == WordIsUnique || parentFlag == WordWasUnique)
    snodeP->info = WordWasUnique;

  if (snodeP->info == WordIsUnique)
    childFlag = WordIsUnique;

  /* hack, should be cleaned */
  if (snodeP->info == WordIsNotUnique)
    snodeP->vX.l = LTREE_NULL_NODE;

  while(childN>=0) {
    snodeWasUnique(snodeP->childPA[childN],childFlag);
    childN--;
  }
  return TCL_OK;
}

static int streeExtractUniqueInfo(STree* streeP) 
{
  int rootX;
  
  for (rootX=0;rootX<streeP->rootTbl.N;rootX++) {
    SRoot* srootP = streeP->rootTbl.PA[rootX];
    int    childN = srootP->childN-1; 
    
    while(childN>=0) {
      snodeIsUnique (srootP->childPA[childN],streeP);
      snodeWasUnique(srootP->childPA[childN],-1);
      childN--;
    }
  }
  return TCL_OK;
}

/* ------------------------------------------------------------------------
   streeInitStart
   ------------------------------------------------------------------------ */

static int streeInitStart(STree* streeP)
{
  SVMap*    svmapP = streeP->svmapP;
  SVocab*  svocabP = svmapP->svocabP;
  int       phoneN = streeP->dictP->phones->list.itemN;
  PHMMX*     hmmXA = NULL;
  SVX          svX = 0;
  int         filN = 0;
  Word xword;

  if (streeP->startTbl.A)     free(streeP->startTbl.A);
  if (streeP->startTbl.typeA) free(streeP->startTbl.typeA);

  streeP->startTbl.N     = 0; 
  streeP->startTbl.A     = NULL;
  streeP->startTbl.typeA = NULL;

  /* build tmp. crossword (needed for phmmSetAdd) */
  if (NULL == (xword.phoneA = (int*) malloc (sizeof(int)*LEVEL_MAX+1))) {
    ERROR("streeInitStart: allocation failure\n");
    return TCL_ERROR;
  }
  if (NULL == (xword.tagA   = (int*) malloc (sizeof(int)*LEVEL_MAX+1))) {
    ERROR("streeInitStart: allocation failure\n");
    return TCL_ERROR;
  }
  
  /* build tmp. hmmXA (needed for lcmSetAdd) */
  if (NULL == (hmmXA = (PHMMX*) malloc(phoneN*sizeof(PHMMX)))) {
    ERROR("streeInitStart: allocation failure\n");
    return TCL_ERROR;
  }
 
  for(svX=0;svX<(SVX) svmapP->svN;svX++) {
    SWord*    swordP = svocabP->list.itemA+svX;   
    DictWord* dwordP = NULL;
    LVX          lvX = svmapGet(svmapP,svX);
    int         segN = 0;
    int       phoneX = 0;

    if (swordP->dictX == SVX_MAX) continue;

    if (swordP->fillerTag == 1) filN++;

    if (lvX != svmapP->idx.start) continue;

    streeP->startTbl.N++;
    if (NULL == (streeP->startTbl.A= 
		 realloc(streeP->startTbl.A,sizeof(void*)*streeP->startTbl.N))) {
      ERROR("streeInitStart: allocation failure\n");
      return TCL_ERROR;
    }
    if (NULL == (streeP->startTbl.typeA= 
		 realloc(streeP->startTbl.typeA,sizeof(UCHAR)*streeP->startTbl.N))) {
      ERROR("streeInitStart: allocation failure\n");
      return TCL_ERROR;
    }

    dwordP = streeP->dictP->list.itemA   + swordP->dictX;
    segN   = dwordP->word.itemN;

    /* canonical start index */
    if (streeP->startTbl.svX == SVX_NIL) {
      streeP->startTbl.svX = svX;
    }

    if (segN == 1) {
      streeP->startTbl.A[streeP->startTbl.N-1]     = streeP->nodeTbl.A[svX];
      streeP->startTbl.typeA[streeP->startTbl.N-1] = streeP->nodeTbl.typeA[svX];
    } else {
      int diphoneX;

      /* get root */
      xword.phonesP = dwordP->word.phonesP;
      xword.tagsP   = dwordP->word.tagsP;
      xword.itemN   = segN+1;
      xword.tagA[0] = streeP->dictP->weTags;
      memcpy(xword.phoneA+1,dwordP->word.phoneA,sizeof(int)*dwordP->word.itemN);
      memcpy(xword.tagA+1,  dwordP->word.tagA,  sizeof(int)*dwordP->word.itemN);    
      for (phoneX=0;phoneX<phoneN;phoneX++) {
	xword.phoneA[0] = phoneX;
	hmmXA[phoneX]   = (PHMMX) phmmSetAdd(streeP->hmmsP,&xword,
					     -1,segN-1,hmmEvalFnRoot);
      }
      diphoneX = (PHMMX) lcmSetAdd(streeP->lcmsP,(CTX)phoneN,hmmXA);
      assert(diphoneX<streeP->rootTbl.N);
      streeP->startTbl.A[streeP->startTbl.N-1]     = streeP->rootTbl.PA[diphoneX];
      streeP->startTbl.typeA[streeP->startTbl.N-1] = type_SRoot;
    }
  }
  if (hmmXA)        free(hmmXA);  
  if (xword.tagA)   free(xword.tagA);
  if (xword.phoneA) free(xword.phoneA);

  if (streeP->startTbl.N == 0) {
    ERROR("streeInitStart: couldn't detect a start word\n");
    return TCL_ERROR;
  }

  if (filN == 0)
    WARN("streeInitStart: detected %d start words, but no filler words\n",streeP->startTbl.N);
  else 
    INFO("streeInitStart: detected %d start words, %d fillers\n",streeP->startTbl.N,filN);

  return TCL_OK;
}

/* ------------------------------------------------------------------------
   streeBuildTree
   ------------------------------------------------------------------------ */

static int streeBuildTree(STree* streeP) 
{
  int rootN     = 0;
  int nodeN     = 0;
  int leafN     = 0;
  int siphoneN  = 0;
  int sdphoneN  = 0;

  if (TCL_ERROR == (streeAdd(streeP,0,(SVX)(streeP->svocabP->list.itemN-1),\
			     &rootN,&nodeN,&leafN))) {
    return TCL_ERROR;
  }
  INFO("streeBuildTree: created %d roots, %d nodes, %d leafs\n",\
       rootN,nodeN,leafN);

  if (TCL_ERROR == (streeAddSPhone(streeP,0,(SVX)(streeP->svocabP->list.itemN-1),\
			     &siphoneN,&sdphoneN))) {
    return TCL_ERROR;
  }
  if (streeP->xcmsP && streeP->xcmsP->ignoreRCM) {
    INFO("streeBuildTree: created %d / %d single phone words (ignoring RCM)\n",\
	 siphoneN,sdphoneN);
  } else {
    INFO("streeBuildTree: created %d / %d single phone words\n",\
	 siphoneN,sdphoneN);
  }
  if (TCL_ERROR == (phmmSetClearCtx(streeP->hmmsP))) {
    return TCL_ERROR;
  }
  if (TCL_ERROR == (streeExtractUniqueInfo(streeP))) {
    return TCL_ERROR;
  }  
  if (TCL_ERROR == (streeInitStart(streeP))) {
    return TCL_ERROR;
  }  
  return TCL_OK;
}

/* ------------------------------------------------------------------------
   streeAddWord
   ------------------------------------------------------------------------ */

static int streeAddWord(STree* streeP,SVX svX, PHMMX phmmX) 
{
  int rootN    = 0;
  int nodeN    = 0;
  int leafN    = 0;
  int siphoneN = 0;
  int sdphoneN = 0;

  if (svX < (SVX) streeP->nodeTbl.N && streeP->nodeTbl.A[svX]) {
    WARN("streeAddWord: %s already known\n",
	 streeP->svocabP->list.itemA[svX].name);
    return TCL_OK;
  }

  /* Pronunciation network as single phone word */
  if (phmmX != PHMM_MAX) {
    return streeAddPronGraph(streeP,svX,phmmX);
  }

  if (TCL_ERROR == (streeAdd(streeP,svX,svX,&rootN,&nodeN,&leafN))) {
    return TCL_ERROR;
  }
  if (TCL_ERROR == (streeAddSPhone(streeP,svX,svX,&siphoneN,&sdphoneN))) {
    return TCL_ERROR;
  } 
  if (TCL_ERROR == (phmmSetClearCtx(streeP->hmmsP))) {
    return TCL_ERROR;
  }
  /* don't forward word id's for compressed networks */
  if (streeP->compress == 0) {
    if (TCL_ERROR == (streeExtractUniqueInfo(streeP))) {
      return TCL_ERROR;
    }  
  }
  return TCL_OK;
}

static int streeAddWordItf (ClientData clientData, int argc, char *argv[]) 
{
  STree *streeP = (STree*) clientData;
  int       ac  =  argc-1;
  int       svX = -1;
  PHMMX    hmmX = PHMM_MAX;

  if ( itfParseArgv (argv[0], &ac, argv+1, 0,
      "<word>", ARGV_LINDEX, NULL, &svX,&(streeP->svocabP->list), "word",
      "-phmmX", ARGV_INT,    NULL, &hmmX,NULL, "PHMM index",
        NULL) != TCL_OK) return TCL_ERROR;

  return streeAddWord(streeP,(SVX)svX,hmmX);
}


/* ------------------------------------------------------------------------
    streeDelWord
   ------------------------------------------------------------------------ */

static int streeDelWord(STree* streeP,SVX svX)
{
  int       phoneN = streeP->dictP->phones->list.itemN;
  PHMMX*     hmmXA = NULL;
  SRoot*    srootP = NULL;
  SNode**  childPA = NULL;
  int       childN = 0;
  SWord*    swordP = streeP->svocabP->list.itemA + svX;
  DictWord* dwordP = NULL;
  void**      ptrA = NULL;
  PHMMX   diphoneX = PHMM_MAX;

  Word xword;
  int rootX,phoneX,segN,childX;

  if (svX >= (SVX) streeP->nodeTbl.N || ! streeP->nodeTbl.A[svX]) {
    WARN("streeDelWord: %s not known\n",
	 streeP->svocabP->list.itemA[svX].name);
    return TCL_OK; 
  }

  /* check for compressed network */
  if (streeP->compress) {
    ERROR("streeDelWord: cannot delete words from compressed network\n");
    return TCL_ERROR;
  }

  /* build tmp. crossword (needed for phmmSetAdd) */
  if (NULL == (xword.phoneA = (int*) malloc (sizeof(int)*LEVEL_MAX+1))) {
    ERROR("streeDelWord: allocation failure\n");
    return TCL_ERROR;
  }
  if (NULL == (xword.tagA   = (int*) malloc (sizeof(int)*LEVEL_MAX+1))) {
    ERROR("streeDelWord: allocation failure\n");
    return TCL_ERROR;
  }
  
  /* build tmp. hmmXA (needed for lcmSetAdd) */
  if (NULL == (hmmXA = (PHMMX*) malloc(phoneN*sizeof(PHMMX)))) {
    ERROR("streeDelWord: allocation failure\n");
    return TCL_ERROR;
  }

  if (streeP->nodeTbl.A[svX] == NULL || swordP->dictX == SVX_MAX) {
     WARN("streeDelWord: %s doesn't exist\n",swordP->name);
     if (hmmXA)        free(hmmXA);  
     if (xword.tagA)   free(xword.tagA);
     if (xword.phoneA) free(xword.phoneA);
     return TCL_OK;
  }

  dwordP = streeP->dictP->list.itemA   + swordP->dictX;
  segN   = dwordP->word.itemN;

  if (segN == 1) {
    int sipX,sdpX;

    for (sipX=0;sipX<streeP->sTbl.iN  && streeP->sTbl.iA[sipX].svX != svX ;sipX++);
    if (sipX < streeP->sTbl.iN) {
      streeP->sTbl.iN--;
      siphoneDeinit(streeP->sTbl.iA+sipX,streeP);
      streeP->sTbl.iA[sipX] = streeP->sTbl.iA[ sipX<streeP->sTbl.iN ];
      streeP->nodeTbl.A[svX] = NULL;

      if (NULL == (streeP->sTbl.iA = realloc(streeP->sTbl.iA,streeP->sTbl.iN))) {
	ERROR("streeDelWord: allocation failure\n");
	return TCL_ERROR;
      }
      if (streeP->sTbl.iN == 0) {
	streeP->sTbl.iA = NULL;
      }
      return TCL_OK;
    }

    for (sdpX=0;sdpX<streeP->sTbl.dN  && streeP->sTbl.dA[sdpX].svX != svX ;sdpX++);
    if (sdpX < streeP->sTbl.dN) {
      streeP->sTbl.dN--;
      sdphoneDeinit(streeP->sTbl.dA+sdpX,streeP);
      streeP->sTbl.dA[sdpX] = streeP->sTbl.dA[ sipX<streeP->sTbl.dN ];
      streeP->nodeTbl.A[svX] = NULL;

      if (NULL == (streeP->sTbl.dA = realloc(streeP->sTbl.dA,streeP->sTbl.dN))) {
	ERROR("streeDelWord: allocation failure\n");
	return TCL_ERROR;
      }
      if (streeP->sTbl.dN == 0) {
	streeP->sTbl.dA = NULL;
      }
      return TCL_OK;
    }
    WARN("streeDelWord: Couldn't find single phone word\n");
    return TCL_OK;
  }

  if (NULL == (ptrA= (void**) malloc (segN*sizeof(void*)))) {
    ERROR("streeDelWord: allocation failure\n");
  }

  /* get root */
  xword.phonesP = dwordP->word.phonesP;
  xword.tagsP   = dwordP->word.tagsP;
  xword.itemN   = segN+1;
  xword.tagA[0] = streeP->dictP->weTags;
  memcpy(xword.phoneA+1,dwordP->word.phoneA,sizeof(int)*dwordP->word.itemN);
  memcpy(xword.tagA+1,  dwordP->word.tagA,  sizeof(int)*dwordP->word.itemN);    
  for (phoneX=0;phoneX<phoneN;phoneX++) {
    xword.phoneA[0] = phoneX;
    hmmXA[phoneX]   = (PHMMX) phmmSetAdd(streeP->hmmsP,&xword,
					 -1,segN-1,hmmEvalFnRoot);
  }
  diphoneX = (PHMMX) lcmSetAdd(streeP->lcmsP,(CTX)phoneN,hmmXA);
  rootX = 0;
  while (rootX < streeP->rootTbl.N && streeP->rootTbl.PA[rootX]->xhmmX != diphoneX) {
    rootX++;
  }
  if (rootX == streeP->rootTbl.N) {
    ERROR("streeDelWord: didn't find root node\n");
    return TCL_ERROR;
  } else {
    srootP = streeP->rootTbl.PA[rootX];
  } 
  ptrA[0] = (void*) srootP;

  /* get root's children */
  childPA = srootP->childPA;
  childN  = srootP->childN;

  /* walk trough the tree, get the nodes */
  for (phoneX= 1;phoneX< segN-1; phoneX++) {
    PHMMX hmmX = (PHMMX) phmmSetAdd(streeP->hmmsP,&(dwordP->word),
				  -phoneX,segN-phoneX-1,hmmEvalFnNode);
    
    childX = 0;
    while (childX < childN && ! (childPA[childX]->X.hmm == hmmX && childPA[childX]->childN >0)) 
      childX++;

    if (childN == 0 || childX == childN) {
      ERROR("streeDelWord: didn't find node for phoneX= %d\n",phoneX);
      return TCL_ERROR;
    }
    ptrA[phoneX] = (void*) childPA[childX];
    childN  = childPA[childX]->childN;
    childPA = childPA[childX]->childPA;

  }      

  /* walk trough the tree, get the leaf */
  childX = 0;
  while (childX < childN && ! (childPA[childX]->vX.s == svX && childPA[childX]->childN ==0)) 
    childX++;

  if (childN == 0 || childX == childN) {
    ERROR("streeDelWord: didn't find leaf\n");
    return TCL_ERROR;
  }
  ptrA[segN-1] = (void*) childPA[childX];

  /* delete all unnecessary nodes */
  for (phoneX= segN-2;phoneX > 0;phoneX--) {
    SNode* snodeP  = (SNode*) ptrA[phoneX];
    SNode* sleafP  = (SNode*) ptrA[phoneX+1];
    childN  = snodeP->childN;
    childPA = snodeP->childPA;

    /* sleaf can be deleted because it's a (unique) leaf or 
       a node without children
    */

    /* delete link snodeP->sleafP */
    snodeP->childN--;
    childX = 0;
    while (childX < childN && childPA[childX] != sleafP)
      childX++;
    if(childN == 0 || childX == childN) {
      ERROR("streeDelWord: didn't find child\n");
      return TCL_ERROR;
    }
    if (childX < childN-1) 
      childPA[childX] =  childPA[childN-1];
    
    /* delete sleaf */
    if (phoneX == segN-2) {
      sleafFree(sleafP,streeP);
      streeP->nodeTbl.leafN--;
    } else {
      snodeFree(sleafP,streeP);
      streeP->nodeTbl.nodeN--;
    }

    /* delete snode only if snode has no child anymore */
    if (snodeP->childN >0) break;
    
    sleafP = snodeP;
  }

  /* check if we can delete child of root */
  if (phoneX == 0 || segN == 2) {
    SNode* sleafP = (SNode*) ptrA[1];

    if (sleafP->childN == 0) {
      srootP= (SRoot*) ptrA[0];
      childN  = srootP->childN;
      childPA = srootP->childPA;

      /* delete link from parent */
      srootP->childN--;
      childX = 0;
      while (childX < childN && childPA[childX] != ptrA[1])
	childX++;
      if(childN == 0 || childX == childN) {
	ERROR("streeDelWord: didn't find child\n");
	return TCL_ERROR;
      }
      if (childX < childN-1) 
	childPA[childX] =  childPA[childN-1];

      /* delete child */
      if (segN == 2) {
	sleafFree(sleafP,streeP);   
	streeP->nodeTbl.leafN--;
      } else {
	snodeFree(sleafP,streeP);   
	streeP->nodeTbl.nodeN--;
      }
    }
  }

  /* do not delete root */
  if (srootP->childN == 0) 
    srootDeactivate(srootP,streeP);

  /* delete map entry */
  streeP->nodeTbl.A[svX] = NULL;

  if (hmmXA)        free(hmmXA);  
  if (xword.tagA)   free(xword.tagA);
  if (xword.phoneA) free(xword.phoneA);
  if (ptrA)         free(ptrA);

  return TCL_OK;
}

static int streeDelWordItf (ClientData clientData, int argc, char *argv[]) 
{
  STree *streeP = (STree*) clientData;
  int      ac   =  argc-1;
  int      svX  = -1;

  if ( itfParseArgv (argv[0], &ac, argv+1, 0,
      "<word>", ARGV_LINDEX, NULL, &svX,&(streeP->svocabP->list), "word",
        NULL) != TCL_OK) return TCL_ERROR;

  return streeDelWord(streeP,(SVX)svX);
}

/* ------------------------------------------------------------------------
   streeReinit
   ------------------------------------------------------------------------ */

extern int streeReinit(STree* streeP) 
{
  if (TCL_ERROR == (streeInitStart(streeP))) {
    return TCL_ERROR;
  }
  /* check for compressed network, no reinit possible */
  if (streeP->compress) {
    return TCL_OK;
  }
  if (TCL_ERROR == (streeExtractUniqueInfo(streeP))) {
    return TCL_ERROR;
  }
  if (TCL_ERROR == (phmmSetClearCtx(streeP->hmmsP))) {
    return TCL_ERROR;
  }

  return TCL_OK;
}

/* ------------------------------------------------------------------------
    streeTrace
   ------------------------------------------------------------------------ */

static int streeTrace(STree* streeP,SVX svX)
{
  Phone*    phoneP = streeP->dictP->phones->list.itemA;
  int       phoneN = streeP->dictP->phones->list.itemN;
  PHMMX*     hmmXA = NULL;
  SRoot*    srootP = NULL;
  SNode*    snodeP = NULL;
  SNode**  childPA = NULL;
  SWord*    swordP = streeP->svocabP->list.itemA + svX;
  DictWord* dwordP = NULL;
  LVX          lvX = svmapGet(streeP->svmapP,svX);
  PHMMX   diphoneX = PHMM_MAX;

  Word xword;
  int rootX,phoneX,segN,childX,childN;

  if (svX >= (SVX) streeP->nodeTbl.N || ! streeP->nodeTbl.A[svX]) {
    WARN("streeTrace: %s not known\n",
	 streeP->svocabP->list.itemA[svX].name);
    return TCL_OK; 
  }

  /* ignore search words without pronunciation */
  if (swordP->dictX == SVX_MAX) {
     ERROR("streeTrace: cannot trace %s\n",swordP->name);
     return TCL_ERROR;
  }

  dwordP = streeP->dictP->list.itemA   + swordP->dictX;
  segN   = dwordP->word.itemN;

  /* check single phone words */
  if (segN == 1) {
    int sipX,sdpX;
    for (sipX=0;sipX<streeP->sTbl.iN  && streeP->sTbl.iA[sipX].svX != svX ;sipX++);
    if (sipX < streeP->sTbl.iN) {
      SIPhone* sipP = streeP->sTbl.iA +sipX;
      itfAppendResult("{%s : hmmX= %d : lvX= %d}\n",
		      (phoneP+sipP->phoneX)->name,sipP->hmmX,lvX);
      return TCL_OK;
    }
    for (sdpX=0;sdpX<streeP->sTbl.dN  && streeP->sTbl.dA[sdpX].svX != svX ;sdpX++);
    if (sdpX < streeP->sTbl.dN) {
      SDPhone* sdpP = streeP->sTbl.dA +sdpX;
      itfAppendResult("{%s : xhmmX= %d : lvX= %d}\n",
		      (phoneP+sdpP->phoneX)->name,sdpP->hmmX,lvX);
      return TCL_OK;
    }
    ERROR("streeTrace: cannot trace %s\n",swordP->name);
    return TCL_ERROR;
  }

  /* build tmp. crossword (needed for phmmSetAdd) */
  if (NULL == (xword.phoneA = (int*) malloc (sizeof(int)*LEVEL_MAX+1))) {
    ERROR("streeTrace: allocation failure\n");
    return TCL_ERROR;
  }
  if (NULL == (xword.tagA   = (int*) malloc (sizeof(int)*LEVEL_MAX+1))) {
    ERROR("streeTrace: allocation failure\n");
    return TCL_ERROR;
  }
  
  /* build tmp. hmmXA (needed for lcmSetAdd) */
  if (NULL == (hmmXA = (PHMMX*) malloc(phoneN*sizeof(PHMMX)))) {
    ERROR("streeTrace: allocation failure\n");
    return TCL_ERROR;
  }

  /* get root */
  xword.phonesP = dwordP->word.phonesP;
  xword.tagsP   = dwordP->word.tagsP;
  xword.itemN   = segN+1;
  xword.tagA[0] = streeP->dictP->weTags;
  memcpy(xword.phoneA+1,dwordP->word.phoneA,sizeof(int)*dwordP->word.itemN);
  memcpy(xword.tagA+1,  dwordP->word.tagA,  sizeof(int)*dwordP->word.itemN);    
  for (phoneX=0;phoneX<phoneN;phoneX++) {
    xword.phoneA[0] = phoneX;
    hmmXA[phoneX]   = (PHMMX) phmmSetAdd(streeP->hmmsP,&xword,
					 -1,segN-1,hmmEvalFnRoot);
  }
  diphoneX = (PHMMX) lcmSetAdd(streeP->lcmsP,(CTX)phoneN,hmmXA);
  rootX = 0;
  while (rootX < streeP->rootTbl.N && streeP->rootTbl.PA[rootX]->xhmmX != diphoneX) {
    rootX++;
  }
  if (rootX == streeP->rootTbl.N) {
    ERROR("streeTrace: didn't find root node\n");
    return TCL_ERROR;
  } else {
    srootP = streeP->rootTbl.PA[rootX];
  }

  itfAppendResult("{%s : rootX= %d : lnX= %d}\n",
		  (phoneP+srootP->phoneX)->name,diphoneX,srootP->lnX);

  /* get nodes */
  childPA = srootP->childPA;
  childN  = srootP->childN;
  for (phoneX= 1;phoneX< segN-1; phoneX++) {
    PHMMX hmmX = (PHMMX) phmmSetAdd(streeP->hmmsP,&(dwordP->word),
				    -phoneX,segN-phoneX-1,hmmEvalFnNode);
    
    childX=0;
    while (childX <childN && ! (childPA[childX]->X.hmm == hmmX && childPA[childX]->childN >0)) {
      childX++;
    }    
    if (childN == 0 || childX == childN) {
      ERROR("streeTrace: didn't find node for phoneX= %d\n",phoneX);
      return TCL_ERROR;
    }
    snodeP  = childPA[childX];
    childPA = snodeP->childPA;
    childN  = snodeP->childN;
    itfAppendResult("{%s : nodeX= %d : lnX= %d childN= %d info= %d}\n",
		    (phoneP+snodeP->phoneX)->name,hmmX,snodeP->vX.l,
		    snodeP->childN,snodeP->info);
  }      

  /* leaf */
  childX=0;
  while (childX <childN && ! (childPA[childX]->vX.s && childPA[childX]->childN ==0)) {
    childX++;
  }
  if (childN == 0 || childX == childN) {
    ERROR("streeTrace: didn't find leaf\n");
    return TCL_ERROR;
  }
  snodeP  = childPA[childX];
  itfAppendResult("{%s : leafX= %d : svX= %d (%d) lvX= %d childN= %d info= %d}\n",
		  (phoneP+snodeP->phoneX)->name,snodeP->X.hmm,svX,lvX,
		  snodeP->vX.s,snodeP->childN,snodeP->info);

  if (hmmXA)        free(hmmXA);  
  if (xword.tagA)   free(xword.tagA);
  if (xword.phoneA) free(xword.phoneA);
  return TCL_OK;
}

static int streeTraceItf (ClientData clientData, int argc, char *argv[]) 
{
  STree *streeP = (STree*) clientData;
  int      ac   =  argc-1;
  int      svX  = -1;

  if ( itfParseArgv (argv[0], &ac, argv+1, 0,
      "<word>", ARGV_LINDEX, NULL, &svX,&(streeP->svocabP->list), "word",
        NULL) != TCL_OK) return TCL_ERROR;
 
  return streeTrace (streeP,(SVX)svX);
}

/* ------------------------------------------------------------------------
    streeLoad
   ------------------------------------------------------------------------ */

static int streeLoad(STree* streeP, FILE* fp, int verbose) 
{  
  int rootX, nodeX, leafX, sipX, sdpX, childX, totalX, svX;
  int rootN, nodeN, leafN, sipN, sdpN, svN, levelMax=0;
  int cntA[LEVEL_MAX];
  char str[1024];
  void** ptrA;

  if (! fp) {
    ERROR("streeLoad: Invalid file pointer\n");
    return TCL_ERROR;
  }
  read_string(fp,str);
  if (0 != strcmp(str,"STREE-BEGIN")) {
    ERROR("streeLoad: inconsistent file\n");
    return TCL_ERROR;
  }

  if (verbose) {
    for (nodeX=0;nodeX<LEVEL_MAX;nodeX++) cntA[nodeX]=0;
    levelMax=0;
  }

  rootN = read_int(fp);
  nodeN = read_int(fp);
  leafN = read_int(fp);
  sipN = streeP->sTbl.iN = read_int(fp);
  sdpN = streeP->sTbl.dN = read_int(fp);

  INFO("streeLoad: load %d roots, %d nodes, %d leafs, %d/%d single phones \n",\
       rootN,nodeN,leafN,sipN,sdpN);

  if (NULL == (ptrA= malloc((rootN+nodeN+leafN+sipN+sdpN)*sizeof(void*)))) {
    ERROR("streeLoad: allocation failure\n");
    return TCL_ERROR;
  }
  totalX = rootN+sipN+sdpN;


  /* allocate node table */
  streeP->nodeTbl.nodeN = nodeN;
  streeP->nodeTbl.leafN = leafN;
  streeP->nodeTbl.N     = streeP->svocabP->list.itemN;
  if (NULL == (streeP->nodeTbl.typeA = 
	       malloc(streeP->nodeTbl.N*sizeof(UCHAR)))) {
    ERROR("streeLoad: allocation failure\n");
    return TCL_ERROR;
  }
  if (NULL == (streeP->nodeTbl.A = 
	       malloc(streeP->nodeTbl.N*sizeof(void*)))) {
    ERROR("streeLoad: allocation failure\n");
    return TCL_ERROR;
  }  
  for (nodeX=0;nodeX<streeP->nodeTbl.N;nodeX++) {
    streeP->nodeTbl.A[nodeX]     = NULL;
    streeP->nodeTbl.typeA[nodeX] = type_NIL;
  }

  /* create roots */
  for (rootX=0;rootX<rootN;rootX++) {
    int phoneX = read_int(fp);
    int hmmX   = read_int(fp);
    srootCreate(streeP,hmmX,(CTX)phoneX);
  }

  /* create sip's */
  if (streeP->sTbl.iN > 0) {
    if (NULL == (streeP->sTbl.iA = malloc(streeP->sTbl.iN*sizeof(SIPhone)))) {
      ERROR("streeLoad: allocation failure\n");
    }
    for (sipX=0;sipX<streeP->sTbl.iN;sipX++) {
      int phoneX = read_int(fp);
      int hmmX   = read_int(fp);
      int lvX    = read_int(fp);
      int svX    = read_int(fp);
      /* lvX is not longer used but must be read in to be compatible with dump files */
      assert(lvX >= 0 || lvX <= 0); 
      siphoneInit(streeP->sTbl.iA+sipX,streeP,hmmX,(CTX)phoneX,(SVX)svX);
      streeP->nodeTbl.typeA[svX] = type_SIPhone;
      streeP->nodeTbl.A[svX]     = (void*) (streeP->sTbl.iA+sipX);
    }
  }

  /* create sdp's */
  if (streeP->sTbl.dN > 0) {
    if (NULL == (streeP->sTbl.dA = malloc(streeP->sTbl.dN*sizeof(SDPhone)))) {
      ERROR("streeLoad: allocation failure\n");
    }
    for (sdpX=0;sdpX<streeP->sTbl.dN;sdpX++) {
      int phoneX = read_int(fp);
      int hmmX   = read_int(fp);
      int lvX    = read_int(fp);
      int svX    = read_int(fp);
      /* lvX is not longer used but must be read in to be compatible with dump files */
      assert(lvX >= 0 || lvX <= 0); 
      sdphoneInit(streeP->sTbl.dA+sdpX,streeP,hmmX,(CTX)phoneX,(SVX)svX);
      streeP->nodeTbl.typeA[svX] = type_SDPhone;
      streeP->nodeTbl.A[svX]     = (void*) (streeP->sTbl.dA+sdpX);
    }
  }

  /* create nodes */
  for (nodeX=0;nodeX<nodeN;nodeX++) {
    int phoneX = read_int(fp);
    int level  = read_int(fp);
    int info   = read_int(fp);
    int hmmX   = read_int(fp);
    int svX    = read_int(fp);      
    SNode* snodeP = snodeCreate(streeP,hmmX,(CTX)phoneX,(LEVEL)level);
    ptrA[totalX++]= snodeP;
    snodeP->info = info;
    if (snodeP->info == WordIsUnique) 
      snodeP->vX.s = (SVX) svX;
    else
      snodeP->vX.l = (LVX) svX;
    if (verbose) {
      cntA[level]++;
      if (level>levelMax) levelMax=level;
    }
  }

  /* create leafs */
  for (leafX=0;leafX<leafN;leafX++) {
    int phoneX = read_int(fp);
    int level  = read_int(fp);
    int info   = read_int(fp);
    int hmmX   = read_int(fp);
    int svX    = read_int(fp);
    SNode* snodeP = sleafCreate(streeP,hmmX,(CTX)phoneX,(SVX)svX,(LEVEL)level);
    ptrA[totalX++]= snodeP;
    snodeP->info = info;
    if (snodeP->info == WordIsUnique) {
      snodeP->vX.s = (SVX) svX;
      streeP->nodeTbl.typeA[svX] = type_SLeaf;
      streeP->nodeTbl.A[svX]     = (void*) snodeP;
    } else {
      snodeP->vX.l = (LVX) svX;
    }
  }

  /* connections from roots */
  for (rootX=0;rootX<rootN;rootX++) {
    SRoot* srootP = streeP->rootTbl.PA[rootX];
    int    childN = read_int(fp);

    srootP->childPA = bmemAlloc(streeP->mem.ptr,childN*sizeof(SNode*));
    srootP->childN  = childN;

    for (childX=0;childX<childN;childX++) {
      int      cIdx = read_int(fp);
      SNode* childP = (SNode*) ptrA[cIdx];
      srootP->childPA[childX] = childP;
    }
  }

  /* connections from nodes */
  for (nodeX=0;nodeX<nodeN;nodeX++) {
    SNode* snodeP = ptrA[rootN+sipN+sdpN+nodeX];
    int    childN = read_int(fp);

    snodeP->childPA = bmemAlloc(streeP->mem.ptr,childN*sizeof(SNode*));
    snodeP->childN  = childN;
    for (childX=0;childX<childN;childX++) {
      int      cIdx = read_int(fp);
      SNode* childP = (SNode*) ptrA[cIdx];
      snodeP->childPA[childX] = childP;
    }
  }

  /* mapper svX -> node type */
  for (svX=0;svX<streeP->nodeTbl.N;svX++) {
    int typeX = read_int(fp);
    int  ptrX = read_int(fp);    
    if (typeX == type_SLeaf) {
      streeP->nodeTbl.typeA[svX]= typeX;
      streeP->nodeTbl.A[svX]    = ptrA[ptrX];
    } 
  }

  read_string(fp,str);
  if (0 != strcmp(str,"STREE-END")) {
    ERROR("streeLoad: inconsistent file\n");
    return TCL_ERROR;
  }

  if (ptrA) free(ptrA);

  if (verbose) {
    for (nodeX=1;nodeX<levelMax;nodeX++)
      INFO("streeLoad: nodes at level %d = %d\n",nodeX,cntA[nodeX]);
  }

  /* set compression flag */
  svN=0;
  for (svX=0;svX<streeP->nodeTbl.N;svX++)
    if (streeP->nodeTbl.A[svX] != NULL) svN++;
  if (streeP->nodeTbl.leafN + streeP->sTbl.iN +  streeP->sTbl.dN < svN) 
    streeP->compress = 1;
  else
    streeP->compress = 0;

  if (TCL_ERROR == (streeInitStart(streeP))) {
    return TCL_ERROR;
  }  

  return TCL_OK;
}

/* ------------------------------------------------------------------------
    streeSave
   ------------------------------------------------------------------------ */

static int streeSave(STree* streeP, FILE* fp) 
{
  SNode** x2p = NULL;
  BMemT  blkX = BMemTNIL;
  int rootX, nodeX, sipX, sdpX, checkX, totalX, svX;
  
  if (! fp) {
    ERROR("streeSave: Invalid file pointer\n");
    return TCL_ERROR;
  }
  write_string(fp,"STREE-BEGIN");

  write_int(fp,streeP->rootTbl.N);
  write_int(fp,streeP->nodeTbl.nodeN);
  write_int(fp,streeP->nodeTbl.leafN);
  write_int(fp,streeP->sTbl.iN);
  write_int(fp,streeP->sTbl.dN);

  /* roots */
  for (rootX=0;rootX<streeP->rootTbl.N;rootX++) {
    SRoot* rootP = streeP->rootTbl.PA[rootX];
    write_int(fp,(int) rootP->phoneX);
    write_int(fp,(int) rootP->xhmmX);
  }

  /* sip */
  for (sipX=0;sipX<streeP->sTbl.iN;sipX++) {
    SIPhone* sipP = streeP->sTbl.iA+sipX;
    LVX       lvX = svmapGet(streeP->svmapP,sipP->svX);
    write_int(fp,(int) sipP->phoneX);
    write_int(fp,(int) sipP->hmmX);
    write_int(fp,(int) lvX);
    write_int(fp,(int) sipP->svX);
  }

  /* sdp */
  for (sdpX=0;sdpX<streeP->sTbl.dN;sdpX++) {
    SDPhone* sdpP = streeP->sTbl.dA+sdpX;
    LVX       lvX = svmapGet(streeP->svmapP,sdpP->svX);
    write_int(fp,(int) sdpP->phoneX);
    write_int(fp,(int) sdpP->hmmX);
    write_int(fp,(int) lvX);
    write_int(fp,(int) sdpP->svX);
  }

  /* allocate map table : nodeX -> ptr */
  nodeX = streeP->nodeTbl.nodeN+streeP->nodeTbl.leafN;
  if (NULL ==( x2p = malloc(nodeX*sizeof(SNode*)))) {
    ERROR("streeSave: Allocation failure\n");
    return TCL_ERROR;
  } 
  nodeX--;
  while (nodeX >=0) x2p[nodeX--] = NULL; 

  /* nodes */
  totalX=streeP->rootTbl.N + streeP->sTbl.iN + streeP->sTbl.dN;
  x2p -= totalX;
  checkX=0;
  for (blkX=0;blkX<streeP->mem.node->blkN;blkX++) {
    MBlk*  mblkP = streeP->mem.node->blkA+blkX;
    SNode* nodeA =  mblkP->memP;
    for (nodeX=0;nodeX<(int)(streeP->mem.node->itemN);nodeX++) {
      SNode* snodeP = nodeA+nodeX;
      if (bmemCheck(streeP->mem.node,snodeP) == TCL_ERROR || snodeP->childN ==0) continue;

      /* define a unique node index */
      x2p[totalX] = snodeP;
      checkX++;
      totalX++;
      write_int(fp,(int) snodeP->phoneX);
      write_int(fp,(int) snodeP->level);
      write_int(fp,(int) snodeP->info);
      write_int(fp,(int) snodeP->X.hmm);
      if (snodeP->info == WordIsUnique) 
	write_int(fp,(int) snodeP->vX.s);
      else
	write_int(fp,(int) snodeP->vX.l);
    }
  }
  assert(checkX == streeP->nodeTbl.nodeN);

  /* leafs */
  checkX=0;
  for (blkX=0;blkX<streeP->mem.node->blkN;blkX++) {
    MBlk*  mblkP = streeP->mem.node->blkA+blkX;
    SNode* nodeA =  mblkP->memP;
    for (nodeX=0;nodeX<(int)(streeP->mem.node->itemN);nodeX++) {
      SNode* snodeP = nodeA+nodeX;
      if (bmemCheck(streeP->mem.node,snodeP) == TCL_ERROR || snodeP->childN >0) continue;

      /* hack to define a unique node index */
      x2p[totalX] = snodeP;
      checkX++;
      totalX++;
      write_int(fp,(int) snodeP->phoneX);
      write_int(fp,(int) snodeP->level);
      write_int(fp,(int) snodeP->info);
      write_int(fp,(int) snodeP->X.hmm);
      if (snodeP->info == WordIsUnique) 
	write_int(fp,(int) snodeP->vX.s);
      else
	write_int(fp,(int) snodeP->vX.l);
    }
  }
  assert(checkX == streeP->nodeTbl.leafN);

  /* connections from roots */
  for (rootX=0;rootX<streeP->rootTbl.N;rootX++) {
    SRoot*  rootP = streeP->rootTbl.PA[rootX];
    int    childX = 0;
    write_int(fp,(int) rootP->childN);
    for (childX=0;childX<rootP->childN;childX++) {
      SNode* childP = rootP->childPA[childX];
      int     nodeX = streeP->rootTbl.N + streeP->sTbl.iN + streeP->sTbl.dN;
      while (nodeX < totalX && x2p[nodeX] != childP) nodeX++;
      assert(x2p[nodeX] == childP);
      write_int(fp,nodeX);
    }
  }
 
  /* connections from nodes */
  for (blkX=0;blkX<streeP->mem.node->blkN;blkX++) {
    MBlk*  mblkP = streeP->mem.node->blkA+blkX;
    SNode* nodeA = mblkP->memP;

    for (nodeX=0;nodeX<(int)(streeP->mem.node->itemN);nodeX++) {
      SNode* snodeP = nodeA+nodeX;
      int    childX = 0;
      if (bmemCheck(streeP->mem.node,snodeP) == TCL_ERROR|| ! snodeP->childN) continue;

      write_int(fp,(int) snodeP->childN);
      for (childX=0;childX<snodeP->childN;childX++) {
	SNode* childP = snodeP->childPA[childX];
	int     nodeX = streeP->rootTbl.N + streeP->sTbl.iN + streeP->sTbl.dN;
	while (nodeX < totalX && x2p[nodeX] != childP) nodeX++;
	assert(x2p[nodeX] == childP);
	write_int(fp,nodeX);
      }
    }
  }

  /* mapper svX -> node type */
  for (svX=0;svX<streeP->nodeTbl.N;svX++) {
    write_int(fp,(int) streeP->nodeTbl.typeA[svX]);
    if (streeP->nodeTbl.typeA[svX] == type_SLeaf) {
      SNode* snodeP = streeP->nodeTbl.A[svX];
      int     nodeX = streeP->rootTbl.N + streeP->sTbl.iN + streeP->sTbl.dN;
      while (nodeX < totalX && x2p[nodeX] != snodeP) nodeX++;
      assert(x2p[nodeX] == snodeP);

      write_int(fp,(int) nodeX);
    } else {
      write_int(fp,-1);
    }
  }
  
  /* free map table */
  x2p += streeP->rootTbl.N + streeP->sTbl.iN + streeP->sTbl.dN;
  if (x2p) free(x2p);

  write_string(fp,"STREE-END");
  return TCL_OK;
}

/* ------------------------------------------------------------------------
    streeDump
   ------------------------------------------------------------------------ */

static int streeReadDump(STree* streeP,char* filename, int verbose)
{
  int dumpLM;

  FILE* fp = fileOpen(filename,"r");
 
  if (!fp) {
    ERROR ("streeReadDump: read failure.\n");
    return TCL_ERROR;
  }

  if ((dumpLM = read_int (fp)) && !streeP->svmapP->lksP->lingKSLoadFct) {
    ERROR ("streeReadDump: inconsistent lm type (no load fct).\n");
    fileClose(filename,fp);
    return TCL_ERROR;
  } else if (dumpLM)
    if (TCL_OK != ( streeP->svmapP->lksP->lingKSLoadFct (streeP->svmapP->lksP, fp, NULL)))
      return TCL_ERROR;

  if (TCL_OK != (dictLoad(streeP->dictP,fp)))          return TCL_ERROR;
  if (streeP->dictP->list.itemN > SVX_MAX) {
    ERROR ("streeReadDump: itemN=%d > SVX_MAX\n", streeP->dictP->list.itemN);
    return TCL_ERROR;
  }
  if ((SVX_MAX>>16) > streeP->dictP->list.itemN)
    INFO ("streeReadDump: itemN=%d << SVX_MAX -- suboptimal\n", streeP->dictP->list.itemN);
  if (TCL_OK != (snsLoad    (streeP->hmmsP->snsP,fp))) return TCL_ERROR;
  if (TCL_OK != (phmmSetLoad(streeP->hmmsP,      fp))) return TCL_ERROR;
  if (TCL_OK != (lcmSetLoad (streeP->lcmsP,      fp))) return TCL_ERROR;
  if (TCL_OK != (rcmSetLoad (streeP->rcmsP,      fp))) return TCL_ERROR;

  if (streeP->xcmsP)
    if (TCL_OK != (xcmSetLoad (streeP->xcmsP,    fp))) return TCL_ERROR;

  if (dumpLM)
    if (TCL_OK != (svmapInitLMWords(streeP->svmapP)))  return TCL_ERROR;

  if (TCL_OK != (svocabLoad (streeP->svocabP,    fp))) return TCL_ERROR;
  if (TCL_OK != (svmapLoad  (streeP->svmapP,     fp))) return TCL_ERROR;
  if (TCL_OK != (streeLoad  (streeP,             fp, verbose))) return TCL_ERROR
;
  fileClose(filename,fp);
  return TCL_OK;
}

static int streeDump(STree* streeP,FILE* fp, int dumpLM)
{
  /* Check if it is save to dump the LM */
  if (dumpLM && !streeP->svmapP->lksP->lingKSSaveFct) {
    WARN("streeDump: lm dump for this type of LingKS not supported, ignoring.\n");
    dumpLM = 0;
  }

  write_int (fp, dumpLM);
  if (dumpLM && streeP->svmapP->lksP->lingKSSaveFct)
    streeP->svmapP->lksP->lingKSSaveFct (streeP->svmapP->lksP, fp);

  dictSave   (streeP->dictP,      fp);
  snsSave    (streeP->hmmsP->snsP,fp);
  phmmSetSave(streeP->hmmsP,      fp);
  lcmSetSave (streeP->lcmsP,      fp);
  rcmSetSave (streeP->rcmsP,      fp);
  if (streeP->xcmsP)
    xcmSetSave (streeP->xcmsP,    fp);

  svocabSave (streeP->svocabP,    fp);
  svmapSave  (streeP->svmapP,     fp);
  streeSave  (streeP,             fp);
  return TCL_OK;
}

static int streeDumpItf( ClientData cd, int argc, char *argv[])
{
  STree*    streeP = (STree*) cd;
  int        ac    =  argc - 1;
  char*      name  =  NULL;
  FILE*      fp    = NULL;
  int       dumpLM = 1;

  if ( itfParseArgv( argv[0], &ac, argv+1, 0,
      "<filename>", ARGV_STRING, NULL, &name,   cd,  "file to dump",
      "-dumpLM",    ARGV_INT,    NULL, &dumpLM, NULL, "dump lm",
       NULL) != TCL_OK) return TCL_ERROR;

  fp = fileOpen(name,"w");
  streeDump(streeP,fp,dumpLM);
  fileClose(name,fp);
  return TCL_OK;
}

/* ========================================================================
    STree compression
   ======================================================================== */

/* ------------------------------------------------------------------------
    reverse STree
   ------------------------------------------------------------------------ */

static void rnodeGetParents (RNode* rnodeP, SNode* snodeP, RArray* rarrayP)
{
  RNode*   nodeP = rarrayP->A + rarrayP->X++;
  int     childX = 0;
 
  nodeP->nodeP   = (void*) snodeP;
  nodeP->parentP = rnodeP;
  nodeP->nextP   = NULL;

  /* attach leafs to virtual end node */
  if (snodeP->childN == 0) {
    RNode* endP = rarrayP->A;
    if (!endP->parentP) {
      endP->parentP = nodeP;
    } else {
      RNode* tmpP =  endP->parentP;
      endP->parentP = nodeP;
      nodeP->nextP  = tmpP;
    }
    return;
  }
  for (childX=0;childX<snodeP->childN;childX++) {
    rnodeGetParents(nodeP,snodeP->childPA[childX],rarrayP);
  }
  return;
}

static RArray* streeMakeRTree(STree* streeP)
{
  int     rootX;
  RArray* rarrayP;

  if (NULL == (rarrayP = malloc(sizeof(RArray)))) {
    ERROR("streeMakeRTreee: allocation failure\n");
    return NULL;
  }
  if (NULL == (rarrayP->A = 
      malloc((streeP->nodeTbl.nodeN+streeP->nodeTbl.leafN+1)*sizeof(RNode)))) {
    ERROR("streeMakeRTreee: allocation failure\n");
    return NULL;
  }

  /* first rnode is the virtual end node for the leafs */
  rarrayP->X = 0;
  rarrayP->A[0].nodeP   = NULL;
  rarrayP->A[0].parentP = NULL;
  rarrayP->A[0].nextP   = NULL;

  /* walk to the stree, and fill the rtree */
  for (rootX=0;rootX<streeP->rootTbl.N;rootX++) {
    SRoot*  rootP = streeP->rootTbl.PA[rootX];
    int    childN = rootP->childN-1;
    for (;childN>=0;childN--) {
      rnodeGetParents(NULL,rootP->childPA[childN],rarrayP);
    }
  }

  return rarrayP;
}

/* ------------------------------------------------------------------------
    rnodeMergeParents
   ------------------------------------------------------------------------ */

static void rnodeMergeParents (RNode* rnodeP, STree* streeP)
{
  RNode* p1 = rnodeP->parentP;

  /* loop over all parent combinations */
  while (p1) {
    SNode* s1 = p1->nodeP;
    RNode* p2 = p1->nextP;

    /* p1 is already merged */
    if (!s1)  { 
      p1=p1->nextP; 
      continue; 
    }

    /* don't merge unique nodes */
    if (s1->info == WordIsUnique)  { 
      p1=p1->nextP; 
      continue; 
    }

    while (p2) {
      SNode* s2 = p2->nodeP;

      /* p2 is already merged */
      if (!s2)  { 
	p2=p2->nextP; 
	continue; 
      }

      /* don't merge unique nodes */
      if (s2->info == WordIsUnique) { 
	p2 = p2->nextP; 
	continue; 
      }

      /* check phmm index */
      if (s1->X.hmm != s2->X.hmm)  { 
	p2 = p2->nextP; 
	continue; 
      } 

      /* move all parents from s2 to s1 in the stree */
      {
	RNode* p3 = p2->parentP;
	while (p3) {
	  SNode*  s3 = p3->nodeP;
	  int childN = s3->childN-1;
	  while (childN>=0 && s3->childPA[childN] != s2) childN--;
	  assert(childN>=0);
	  s3->childPA[childN] = s1;
	  p3=p3->nextP;
	}
      }
      /* move all parents from p2 to p1 in the rtree */
      {
	RNode* p3 = p1->parentP;
	while (p3->nextP) p3=p3->nextP;
	p3->nextP=p2->parentP;
      }

      /* update map svX -> sleaf */
      if (s2->childN == 0) {
	streeP->nodeTbl.A[s2->vX.s] = s1;
      }

      /* delete s2 */
      if (s2->childN>0) {
	bmemDealloc(streeP->mem.ptr,s2,s2->childN*sizeof(SNode*));
	snodeFree(s2,streeP);   
	streeP->nodeTbl.nodeN--;
      } else {
	sleafFree(s2,streeP);   
	streeP->nodeTbl.leafN--;
      }

      p2->nodeP   = NULL;
      p2->parentP = NULL;

      p2 = p2->nextP;
    }
    p1=p1->nextP;
  }
}

/* ------------------------------------------------------------------------
    streeCompressTree
   ------------------------------------------------------------------------ */

static int streeCompressTree(STree* streeP, int verbose)
{
  RArray* rarrayP = NULL;
  int      nodeN  = streeP->nodeTbl.nodeN+ streeP->nodeTbl.leafN;
  int      nodeM  = 0;
  int          x  = 0;

  if (streeP->compress) {
    WARN("streeCompress: cannot compress compressed network\n");
    return TCL_OK;
  }

  streeP->compress = 1;
  rarrayP = streeMakeRTree(streeP);

  if (verbose)
    INFO("nodeN = %d  leafN= %d\n",streeP->nodeTbl.nodeN,streeP->nodeTbl.leafN);

  while (nodeN != nodeM) {
    for (x=0;x<rarrayP->X;x++) 
      rnodeMergeParents(rarrayP->A+x,streeP);
    if (verbose==2)
      INFO("nodeN = %d  leafN= %d\n",streeP->nodeTbl.nodeN,streeP->nodeTbl.leafN);
    nodeM  = nodeN;
    nodeN  = streeP->nodeTbl.nodeN+ streeP->nodeTbl.leafN;
  }

  if (verbose==1)
    INFO("nodeN = %d  leafN= %d\n",streeP->nodeTbl.nodeN,streeP->nodeTbl.leafN);

  if (rarrayP) {
    if (rarrayP->A) free(rarrayP->A);
    free(rarrayP);
  }

  return TCL_OK;
}

static int streeCompressTreeItf( ClientData cd, int argc, char *argv[])
{
  STree*    streeP = (STree*) cd;
  int        ac    =  argc - 1;
  int      verbose = 0;

  if ( itfParseArgv( argv[0], &ac, argv+1, 0,
      "-v",    ARGV_INT,    NULL, &verbose, NULL, "verbose output",
       NULL) != TCL_OK) return TCL_ERROR;

  streeCompressTree(streeP,verbose);
  return TCL_OK;
}

/* ------------------------------------------------------------------------
    streeConfigure
   ------------------------------------------------------------------------ */

static int streeConfigureItf(ClientData cd, char *var, char *val) {
  STree* streeP   = (STree*)cd; 
  if (! streeP) return TCL_ERROR;
  if (! var) {
    ITFCFG(streeConfigureItf,cd,"name");
    ITFCFG(streeConfigureItf,cd,"useN");
    ITFCFG(streeConfigureItf,cd,"rootN");
    ITFCFG(streeConfigureItf,cd,"nodeN");
    ITFCFG(streeConfigureItf,cd,"leafN");
    ITFCFG(streeConfigureItf,cd,"sipN");
    ITFCFG(streeConfigureItf,cd,"sdpN");
    ITFCFG(streeConfigureItf,cd,"compress");
    return TCL_OK;
  }
  ITFCFG_CharPtr(var,val,"name",streeP->name,1);
  ITFCFG_Int(var,val,"useN", streeP->useN,1);
  ITFCFG_Int(var,val,"nodeN",streeP->nodeTbl.nodeN,1);
  ITFCFG_Int(var,val,"rootN",streeP->rootTbl.N,1);
  ITFCFG_Int(var,val,"nodeN",streeP->nodeTbl.nodeN,1);
  ITFCFG_Int(var,val,"leafN",streeP->nodeTbl.leafN,1);
  ITFCFG_Int(var,val,"sipN", streeP->sTbl.iN,1);
  ITFCFG_Int(var,val,"sdpN", streeP->sTbl.dN,1);
  ITFCFG_Int(var,val,"compress", streeP->compress,1);

  ERROR("unknown option '-%s %s'.\n", var, val ? val : "?");
  return TCL_ERROR;   
}



/* ========================================================================
    Type Information
   ======================================================================== */

Method srootMethod[] = 
{
  { "puts", srootPutsItf,   "displays the contents of a search root"  },
  { NULL, NULL, NULL }
} ;

TypeInfo srootInfo = 
{ "SRoot", 0, -1,  srootMethod, 
   NULL, NULL, 
   NULL, srootAccessItf, NULL,
  "Search Root"
};

Method snodeMethod[] = 
{
  { "puts",   snodePutsItf,   "displays the contents of a search node/leaf"},
  { "isNode", snodeIsNodeItf, "return 1 if node is a non-leaf"},
  { "isLeaf", snodeIsLeafItf, "return 1 if node is a leaf"},
  { NULL, NULL, NULL }
} ;

TypeInfo snodeInfo = 
{ "SNode", 0, -1,  snodeMethod, 
   NULL, NULL, 
   NULL, snodeAccessItf, NULL,
  "Search Root"
};


Method streeMethod[] = 
{
  { "puts",   streePutsItf,    "puts information"             },
  { "trace",  streeTraceItf,   "trace search tree"            },
  { "add",    streeAddWordItf, "add word to search tree"      },
  { "delete", streeDelWordItf, "delete word from search tree" },
  { "dump",   streeDumpItf,    "dump search tree"             },
  { "compress", streeCompressTreeItf, "compress search tree, convert tree into generalized graph structure"  },
  { NULL, NULL, NULL }
} ;

TypeInfo streeInfo = 
{ "STree", 0, -1,  streeMethod, 
   streeCreateItf, streeFreeItf, 
   streeConfigureItf, streeAccessItf, NULL,
  "Search Tree"
};

static int streeInitialized = 0;

int STree_Init (void)
{
  if (! streeInitialized) {
    if ( PHMM_Init()   != TCL_OK) return TCL_ERROR;
    if ( XHMM_Init()   != TCL_OK) return TCL_ERROR;
    if ( SMem_Init()   != TCL_OK) return TCL_ERROR;
    if ( SVMap_Init()  != TCL_OK) return TCL_ERROR;
    if ( LMLA_Init()   != TCL_OK) return TCL_ERROR;

    streeDefault.name            = NULL;
    streeDefault.useN            = 0;

    streeDefault.smem.level      = -1;
    streeDefault.smem.morphBlkN  =  2;
    streeDefault.smem.smemFree   =  0;
    streeDefault.rootTbl.blkSize = 50;
  
    streeDefault.lct2pos.hashSize = 256;
    streeDefault.lct2pos.bitmask  = 0x000f;
    streeDefault.lct2pos.offset   = 4;
    streeDefault.lct2pos.tryMax   = 8;
    
    itfNewType (&srootInfo);
    itfNewType (&snodeInfo);
    itfNewType (&streeInfo);
    streeInitialized = 1;
  }
  return TCL_OK;
}
