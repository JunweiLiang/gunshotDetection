/* ========================================================================
    JanusRTk   Janus Recognition Toolkit
               ------------------------------------------------------------
               Object: Language Model LookAhead
               ------------------------------------------------------------

    Author  :  Florian Metze
    Module  :  lmla.c
    Date    :  $Id: lmla.c 3414 2011-03-22 19:22:48Z metze $

    Remarks :  This code realizes a language model lookahead for more
               efficient pruning during decoding.
               It works with the "NJD generic language model" in file
               lmodelNJD

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

    $Log: lmla.c,v $
    Revision 1.1.1.2  2008/09/10 17:07:51  metze.florian
    FMe+FFa

    Revision 1.1.1.1  2007/06/20 12:34:52  metze.florian
    Initial import of Ibis 5.1 from UKA at T-Labs

    Revision 4.9  2005/10/12 13:35:02  metze
    Made 'ltree.queries|cachehits|nodecachehits' accessible again

    Revision 4.8  2004/09/23 14:46:06  metze
    Fixes for x86_64

    Revision 4.7  2004/09/11 12:40:43  metze
    Cleaned LTree configure/ access

    Revision 4.6  2004/09/02 17:15:31  metze
    LTree access improved

    Revision 4.5  2003/11/14 17:51:43  metze
    Confused svxHash and lctHash ... fixed it

    Revision 4.4  2003/11/13 12:35:19  metze
    Code cleaning

    Revision 4.3  2003/08/14 11:20:03  fuegen
    Merged changes on branch jtk-01-01-15-fms (jaguar -> ibis-013)

    Revision 4.2.2.99  2003/05/08 16:41:53  soltau
    Purified!

    Revision 4.2.2.98  2003/03/18 18:09:56  soltau
    Added time based constrains

    Revision 4.2.2.97  2003/01/29 10:37:29  soltau
    Fixed size of lmlaP->latlm.tmpA for glatConstrain

    Revision 4.2.2.96  2003/01/17 10:05:06  soltau
    Fixed lattice constrain for lvX classes

    Revision 4.2.2.95  2002/11/13 09:55:18  soltau
    Start and End words rely on LVX now

    Revision 4.2.2.94  2002/08/27 08:48:14  metze
    itfParseArgv now uses <name> and NULL initialization for char*

    Revision 4.2.2.93  2002/08/01 13:42:30  metze
    Fixes for clean documentation.

    Revision 4.2.2.92  2002/07/18 12:09:36  soltau
    Added ringbuffer to optimize hashtables for svxList, lctList

    Revision 4.2.2.91  2002/06/26 11:57:54  fuegen
    changes for new directory structure support and code cleaning

    Revision 4.2.2.90  2002/06/18 16:42:22  metze
    The cache is clean upon creation

    Revision 4.2.2.89  2002/06/13 08:07:10  metze
    Added dirty as a function in LMs

    Revision 4.2.2.88  2002/06/10 13:26:29  metze
    LTree now checks svmapIsDirty

    Revision 4.2.2.87  2002/05/02 11:59:20  soltau
    Added constrain method, options for mode and type
    Removed unnecessary ltree construction structures

    Revision 4.2.2.86  2002/04/29 13:29:36  metze
    Redesign of LM interface (LingKS)

    Revision 4.2.2.85  2002/04/10 15:11:14  soltau
    lmlaInit: initialize lmlaP->mode, latlmP->mode, latlmP->latP

    Revision 4.2.2.84  2002/03/07 10:26:40  metze
    Oops, now it compiles :-)

    Revision 4.2.2.83  2002/03/07 10:16:57  metze
    Added LCT handling with hashtable for LModelNJD

    Revision 4.2.2.82  2002/03/03 12:33:06  metze
    loop -> memcpy (beautification)

    Revision 4.2.2.81  2002/02/27 09:16:41  metze
    We no longer allocate new memory with every ExtendLCT

    Revision 4.2.2.80  2002/02/24 09:30:28  metze
    Rewrite of Lattice LM: now works with Hash Fct

    Revision 4.2.2.79  2002/02/21 07:06:02  metze
    Added GetOneLMScore

    Revision 4.2.2.78  2002/02/15 16:24:55  metze
    Fixes for compilation under Solaris

    Revision 4.2.2.77  2002/02/14 18:30:51  soltau
    *** empty log message ***

    Revision 4.2.2.76  2002/02/14 15:16:15  metze
    Documented errors in old lmlaFillCtx code

    Revision 4.2.2.75  2002/02/14 12:08:27  soltau
    CreateLCT: call svmapClearCache

    Revision 4.2.2.74  2002/02/14 10:54:45  metze
    Rewrite of lmlaFillCtx, new interface to spass, ltree with svMap

    Revision 4.2.2.73  2002/02/13 08:00:38  metze
    Added lctHash stuff, new lmlaFillCtx still commented out as default

    Revision 4.2.2.72  2002/02/10 17:50:55  metze
    Rewrite of lmlaFillCtx: working, but slower (#define alterkram)

    Revision 4.2.2.71  2002/02/07 12:13:04  metze
    Changes in interface to LM score functions

    Revision 4.2.2.70  2002/01/26 17:09:03  soltau
    should first compile, then check-in

    Revision 4.2.2.69  2002/01/26 15:30:34  soltau
    CreateLCT: deactivated lattice constraint for empty lattices

    Revision 4.2.2.68  2002/01/09 17:06:31  soltau
    changed child access in stree

    Revision 4.2.2.67  2001/11/30 08:16:37  metze
    Made the constraint-mode self-explanatory

    Revision 4.2.2.66  2001/11/23 09:32:59  metze
    MError messages now report 'ltree' instead of 'lmla'

    Revision 4.2.2.65  2001/11/06 15:40:44  metze
    Arguments are now called '-ncacheN' ... for consistency

    Revision 4.2.2.64  2001/10/22 08:39:58  metze
    Changed interface to score functions

    Revision 4.2.2.63  2001/10/11 16:56:51  soltau
    Added 'Flat_Constraint' Mode

    Revision 4.2.2.62  2001/10/09 18:34:12  soltau
    Added constraint_mode for acoustic rescoring
    Removed some debug output

    Revision 4.2.2.61  2001/10/03 09:59:23  metze
    Added code for useN support in LMs

    Revision 4.2.2.60  2001/09/27 14:54:31  soltau
    support for non-purified lattices as constraint

    Revision 4.2.2.59  2001/09/26 08:38:34  metze
    Renamed interpolLM to metaLM

    Revision 4.2.2.58  2001/09/26 10:09:33  soltau
    *** empty log message ***

    Revision 4.2.2.57  2001/09/25 15:53:29  soltau
    Check for empty lattice in CreateLCT

    Revision 4.2.2.56  2001/09/25 14:52:33  soltau
    Fixed caching problems
    Added aggressive lattice constraint (but disabled)

    Revision 4.2.2.55  2001/09/24 19:48:47  soltau
    *** empty log message ***

    Revision 4.2.2.54  2001/09/24 19:31:34  soltau
    support Path Recombination during constrained lattice decoding

    Revision 4.2.2.53  2001/09/24 14:10:53  soltau
    Deactivate node cache for lattice constrains
    Compacted lmlaInit output

    Revision 4.2.2.52  2001/08/13 14:27:04  metze
    Implemented acoustic rescoring with smeared LMLA
    (still doesn't work 100% ok)

    Revision 4.2.2.51  2001/06/28 11:52:25  metze
    Forgot bmem changes

    Revision 4.2.2.50  2001/06/28 11:44:20  metze
    Bug fix for lmodelNJD on SUNs
    Acoustic rescoring does still not work

    Revision 4.2.2.49  2001/06/25 21:05:25  soltau
    Changed bmem Interface

    Revision 4.2.2.48  2001/06/21 18:27:51  soltau
    joint effort from Florian and me (Hagen) to fix lct handling in
    lmodelNJD.c and lmla.c and to get the 'Best Hacker' award

    Revision 4.2.2.47  2001/06/20 18:30:37  soltau
    Check latP in configureConstraint

    Revision 4.2.2.46  2001/06/20 16:05:31  metze
    Bugfix for configureCache

    Revision 4.2.2.45  2001/06/19 16:24:42  metze
    Bugfix: initialize all node-cache lines, not only the lower ones

    Revision 4.2.2.44  2001/06/18 12:23:13  metze
    Changed reference to bmemDeinit - again

    Revision 4.2.2.43  2001/06/18 08:02:26  metze
    Introduced bmem, LCT handling works on SUNs, new svmapGet

    Revision 4.2.2.42  2001/06/08 18:25:16  metze
    Decoding along lattice (LatticeLM, Acoustic Rescoring) works

    Revision 4.2.2.41  2001/06/01 12:51:38  metze
    Moved LMSCORE_MAX to sglobal.h

    Revision 4.2.2.40  2001/06/01 10:06:46  metze
    Implemented decoding along lattices

    Revision 4.2.2.39  2001/06/01 09:03:25  fuegen
    removed LatticeLM

    Revision 4.2.2.38  2001/06/01 08:53:19  fuegen
    removed LatticeLM

    Revision 4.2.2.37  2001/05/28 14:47:36  metze
    Added LatticeLM

    Revision 4.2.2.36  2001/05/23 08:09:21  metze
    LingKScheck does not give unneccessary output any longer

    Revision 4.2.2.35  2001/05/11 16:40:02  metze
    Cleaned up LCT interface

    Revision 4.2.2.34  2001/05/03 07:17:16  metze
    Made different LMs for Exact/ Lookahead scores working in principal

    Revision 4.2.2.33  2001/04/27 07:37:29  metze
    General access functions to LM via 'LCT' implemented

    Revision 4.2.2.32  2001/04/25 09:05:44  metze
    Last version without checks for reduced lct in node-cache

    Revision 4.2.2.31  2001/04/23 14:31:39  soltau
    Added lmlaReinit

    Revision 4.2.2.30  2001/04/23 11:54:48  metze
    Implemented second-level cache for nodes only

    Revision 4.2.2.29  2001/04/12 19:18:49  soltau
    Disabled pcache

    Revision 4.2.2.28  2001/04/12 18:41:38  soltau
    same_pron support

    Revision 4.2.2.27  2001/04/04 16:51:51  soltau
    *** empty log message ***

    Revision 4.2.2.26  2001/04/04 08:53:47  soltau
    Removed delta's

    Revision 4.2.2.25  2001/04/03 16:44:54  soltau
    separate cache for lmlaPhones

    Revision 4.2.2.24  2001/03/27 18:05:26  metze
    Introduced lmlaPhones and removed phones' score cache from lmlaFillCtx

    Revision 4.2.2.23  2001/03/23 13:25:59  metze
    Ageing cache, different M-Grams for exact and LA LM

    Revision 4.2.2.22  2001/03/22 16:12:33  metze
    Added aging cache

    Revision 4.2.2.21  2001/03/20 13:29:37  soltau
    - Fixed kill-all-children loops
    - parent search node get now the correct language tree node index
    - allocate and init lm tree roots

    Revision 4.2.2.20  2001/03/17 17:35:48  soltau
    Fixed memory overflow (RCM lm lookahead)

    Revision 4.2.2.19  2001/03/17 11:06:30  metze
    Beauty is Best

    Revision 4.2.2.18  2001/03/16 17:16:47  soltau
    renamed lmla object (yes, I will fight for this)

    Revision 4.2.2.17  2001/03/16 12:15:02  metze
    Bugfix for SearchMGrams

    Revision 4.2.2.16  2001/03/15 17:41:54  metze
    Switched on vocabulary mapping

    Revision 4.2.2.15  2001/03/15 15:42:58  metze
    Re-implementation of LM-Lookahead

    Revision 4.2.2.14  2001/03/13 17:58:24  soltau
    cache basephone lookaheads and made it working (Hagen and Florian)

    Revision 4.2.2.13  2001/03/12 18:40:59  soltau
    lmlaFillCtx:
     - made cache working
     - optimized program loops
     - TODO: cache basephone lookaheads
    compressSTree:
     - enabled tree level pruning

    (Hagen and Florian)

    Revision 4.2.2.12  2001/03/09 16:32:59  soltau
    link also non unique leafs (same_pron_words) in LinkLTreeWithSTree

    Revision 4.2.2.11  2001/03/06 15:17:50  metze
    Fixed problems in lmla: Single-phone words and multiple use of nodes

    Revision 4.2.2.10  2001/03/06 06:50:00  soltau
    Fixed lmlaDeinit

    Revision 4.2.2.9  2001/03/05 08:55:49  metze
    Order of arguments for FillCtx cleaned up

    Revision 4.2.2.8  2001/03/05 08:39:31  metze
    Cleaned up treatment of exact LM in LMLA

    Revision 4.2.2.7  2001/02/28 17:52:41  metze
    Removed pragmas to avoid confusion of the SUN-compiler

    Revision 4.2.2.6  2001/02/28 16:32:41  metze
    Bugfix

    Revision 4.2.2.5  2001/02/28 16:12:48  metze
    lmlaInit now returns correct lnX

    Revision 4.2.2.4  2001/02/27 15:10:27  metze
    LModelNJD and LMLA work with floats

    Revision 4.2.2.3  2001/02/09 09:36:40  metze
    New style snapshot, working fairly well

    Revision 4.2.2.2  2001/02/05 18:33:46  metze
    LM-LookAhead old style, but working

    Revision 4.2.2.1  2001/02/02 17:02:24  metze
    Made LM-LookAhead Cache work

    Revision 4.2  2001/01/25 15:14:30  metze
    Testing for the Name tag

    Revision 4.1  2001/01/25 13:45:07  metze
    Initial revision of language model look-ahead

   ======================================================================== */


char lmlaRCSVersion[]="@(#)1$Id: lmla.c 3414 2011-03-22 19:22:48Z metze $";

#include "lks.h"
#include "lmla.h"
#include "svocab.h"
#include "svmap.h"
#include "bmem.h"
#include "ffmat.h"

/* ----------------------- Type declarations ------------------------------ */
extern TypeInfo lmlaInfo;
static int      lmlaInitialized = FALSE;

/* ========================================================================
    LCT/ NCT handling by hash table
   ======================================================================== */

static int hlctInit (HashLCT* hlctP, HashLCT* hashkey)
{
  (*hlctP) = (*hashkey);
  return TCL_OK;
}
  
static int hlctHashCmp (HashLCT* hlctP, HashLCT* hashkey)
{
  return (hlctP->lct == hashkey->lct);
}

#define lctMask 0xFFFF
static long hlctHashKey (const HashLCT *key)
{
  unsigned long ret = key->lct & lctMask;
  return ret;
}

static int hsvxInit (HashSVX* hsvxP, HashSVX* hashkey)
{
  (*hsvxP) = (*hashkey);
  return TCL_OK;
}
  
static int hsvxHashCmp (HashSVX* hsvxP, HashSVX* hashkey)
{
  return (hsvxP->svx == hashkey->svx && hsvxP->lct == hashkey->lct);
}

#define svxMask 0xFFFF
static long hsvxHashKey (const HashSVX *key)
{
  unsigned long ret = key->svx & svxMask;
  return ret;
}

/* ========================================================================
    Lattice Constraint
   ======================================================================== */

#define CONSTRAINT_EXACT(l) ((l)->latlm.latP && (l)->latlm.mode == Exact_Constraint)

static int LatLMHashInit (LatLMNode* ptr, LatLMNode* key)
{
  (*ptr) = (*key);
  return TCL_OK;
}
  
static int LatLMHashCmp  (LatLMNode* ptr, LatLMNode* key)
{
  if (ptr->lct    != key->lct)    return 0;
  if (ptr->gnodeN != key->gnodeN) return 0;
  else {
    int i;
    for (i = 0; i < ptr->gnodeN; i++)
      if (ptr->gnodeA[i] != key->gnodeA[i])
	return 0;
  }
  assert (ptr->svX == key->svX);
  return 1;
}

#define latlmMask 0xFFFF
static long LatLMHashKey (const LatLMNode *key)
{
  return (unsigned long) (key->svX & latlmMask)+key->gnodeN;
}

/* ------------------------------------------------------------------------
    addGNodes
   ------------------------------------------------------------------------ */

union {
  LCT        l;
  LatLMNode *p;
} c;

static int addGNodes (LMLA* lmlaP, GNode* nodeP, SVX svX, GNode** ptrA, int ptrN)
{
  SVMap*   svmapP = lmlaP->streeP->svmapP;
  SVocab* svocabP = svmapP->svocabP;
  GLink*    linkP = nodeP->childP;
  LVX         lvX = svmapGet(svmapP,svX);
  /* Let's visit all our children */
  while (linkP) {
    GNode* nodeP = linkP->childP;

    if (svocabP->list.itemA[nodeP->svX].fillerTag) {
      /* Recursion with all children of this node */
      ptrN = addGNodes (lmlaP, nodeP, svX, ptrA, ptrN);

    } else if ( (lmlaP->latlm.what == 0 && svX == nodeP->svX) ||
		(lmlaP->latlm.what == 1 && lvX == svmapGet(svmapP,nodeP->svX))) {

      if (ptrN) {
	/* Insert 'nodeP' into our list */
	int i;
	for (i = 0; i < ptrN && nodeP > ptrA[i]; i++);
	if (i == ptrN)
	  ptrA[ptrN++] = nodeP;
	else if (nodeP < ptrA[i]) {
	  int j;
	  for (j = ptrN++; j > i; j--)
	    ptrA[j] = ptrA[j-1];
	  ptrA[i] = nodeP;
	}
      } else {
	/* First entry */
	ptrA[ptrN++] = nodeP;
      }
    }
    linkP = linkP->nextP;
  }
  return ptrN;  
}

/* ------------------------------------------------------------------------
    lmlaClearConstraint
   ------------------------------------------------------------------------ */

static int lmlaClearConstraint (LMLA* lmlaP)
{
  listClear ((List*) &lmlaP->latlm.list);
  bmemClear (lmlaP->latlm.buffer);
  lmlaP->latlm.frameX = -1;
  return TCL_OK;
}

/* ------------------------------------------------------------------------
    lmlaCreateConstraint
   ------------------------------------------------------------------------ */

static int lmlaCreateConstraint (LMLA* lmlaP, GLat* latP, int mode, int what)
{
  if (latP && latP->svmapP != lmlaP->streeP->svmapP) {
    ERROR("lmlaCreateConstraint: GLat (%s) and LTree (%s) should base on the same vocabulary mapper\n",
	  latP->svmapP->name,lmlaP->streeP->svmapP->name);
    return TCL_ERROR;
  }

  if (lmlaP->latlm.latP)
    lmlaClearConstraint(lmlaP);

  lmlaP->latlm.mode = mode;
  lmlaP->latlm.what = what;
  lmlaP->latlm.latP = latP;

  return TCL_OK;
}

static int lmlaCreateConstraintItf (ClientData clientData, int argc, char *argv[]) 
{
  LMLA*   lmlaP = (LMLA*) clientData;
  int        ac =  argc-1; 
  GLat*    latP = lmlaP->latlm.latP;
  int      mode = lmlaP->latlm.mode;
  int      what = lmlaP->latlm.what;
  char*  latName = (latP) ? latP->name : "NULL";
  char* modeName = "NULL";
  char* whatName = "NULL";
  
  switch(mode) {
  case Flat_Constraint  : modeName = "flat";  break;
  case Weak_Constraint  : modeName = "weak";  break;
  case Exact_Constraint : modeName = "exact"; break;
  case Time_Constraint :  modeName = "time";  break;
  default : break;
  }
  switch(what) {
  case 0 : whatName = "SVX"; break;
  case 1 : whatName = "LVX"; break;
  default : break;
  }

  if (itfParseArgv (argv[0], &ac, argv+1, 0,
      "<GLat>", ARGV_STRING, NULL, &latName, NULL, "GLat (or NULL to deactivate constraint)",
      "-mode",  ARGV_STRING, NULL, &modeName,NULL, "flat|weak|exact|time",
      "-type",  ARGV_STRING, NULL, &whatName,NULL, "SVX|LVX",
      "-padX",  ARGV_INT,    NULL, &lmlaP->latlm.padX,NULL, "padding for time based constraints",
        NULL) != TCL_OK) return TCL_ERROR;

  if (! streq(latName,"NULL") && ! streq(latName,"NONE")) {
      latP = (GLat*) itfGetObject (latName, "GLat");
      if (! latP) {
	ERROR("lmlaCreateConstraintItf: invalid GLat '%s'\n",latName);
	return TCL_ERROR;
      } 
  } else {
    latP = NULL;
  }

  mode = -1;
  if (streq(modeName,"flat"))  mode = Flat_Constraint;  
  if (streq(modeName,"weak"))  mode = Weak_Constraint;  
  if (streq(modeName,"exact")) mode = Exact_Constraint;  
  if (streq(modeName,"time"))  mode = Time_Constraint;  
  if (mode == -1) {
    ERROR("lmlaCreateConstraintItf: invalid mode '%s'\n",modeName);
    return TCL_ERROR;
  }

  what = -1;
  if (streq(whatName,"SVX"))  what = 0;  
  if (streq(whatName,"LVX"))  what = 1;  
  if (what == -1) {
    ERROR("lmlaCreateConstraintItf: invalid type '%s'\n",whatName);
    return TCL_ERROR;
  }

  return lmlaCreateConstraint(lmlaP,latP,mode,what);
}

/* ========================================================================
    Procedures for LCT handling (transitions into leafs)
   ------------------------------------------------------
    These  functions are to  be provided by  every LM  (in the form of
    function  pointers  in the LingKS  structure).  However, if  we do
    acoustic lattice rescoring using the LTreeCon constraints, we need
    the information on svX, i.e.  which  leaf are we entering into, so
    that we need to   wrap  these functions  here,  where we  can  map
    between svX  and lvX.  In the  case of LModelNJD, we replicate the
    functionality  here to avoid the  overhead  of additional function
    calls.  If   we decode with a constraint,   this  function will be
    called with  lksP->type == LatticeLM_Type, so  we can fall back to
    the   original  language   model   which    we  have   stored   in
    constraintP.
   ======================================================================== */

/* -------------------------------------------
     Get the nct (lct for the nodes LM), if:
   -------------------------------------------
     We're not usingLatLM until we use it in
     Flat mode. Get the reduced lct if we use
     the same LM, otherwise use the hash fct
   ------------------------------------------- */

static LCT lct2nct (LMLA* ltreeP, LCT lct)
{
  HashLCT    hkey;
  int        idx;

  if (CONSTRAINT_EXACT(ltreeP))
    return lct;

  if (ltreeP->streeP->svmapP == ltreeP->svmapP)
    return (ltreeP->reduced) ? ReduceLCT (ltreeP, lct, 0) : lct;
  
  hkey.lct = lct;
  idx      = listIndex ((List*) &ltreeP->hlctList, (void*) &hkey, 0);
  idx      = idx % ltreeP->hlctList.itemMax;

  assert (idx >= 0); /* This can happen when hlctList.itemMax is exceeded, should be unsigned? */
  assert (idx < ltreeP->hlctList.itemN);

  return ltreeP->hlctList.itemA[idx].nct;
}

/* Check if we have to get the lct via latlm */
static LCT TrueLCT (LMLA* ltreeP, LCT lct)
{
  if (CONSTRAINT_EXACT(ltreeP)) {
    c.p = ltreeP->latlm.list.itemA+lct;
    return c.p->lct;
  }
  return lct;
}

static LCT TrueNCT (LMLA* ltreeP, LCT lct, LCT nct)
{
  if (CONSTRAINT_EXACT(ltreeP)) {
    c.p = ltreeP->latlm.list.itemA+lct;
    return c.p->nct;
  }
  return nct;
}


/* ------------------------------------------------------------------------
    ReduceLCT
   -----------
    Finds the LCT pointing to the n-th predecessor of an LCT, i.e. 
    reduces the history by n words. Used to find the history for a 
    Bigram lm look-ahead when decoding with the full trigram
   ------------------------------------------------------------------------ */

extern LCT ReduceLCT (LMLA* ltreeP, LCT lct, int n)
{
  LingKS* lksP = ltreeP->streeP->svmapP->lksP;
  return lksP->reduceLCT (lksP, lct, n);
}


/* ------------------------------------------------------------------------
    ExtendLCT
   -----------
    Extends the history of LCTs by an svX, we need to know the mapper, too
   ------------------------------------------------------------------------ */

extern LCT ExtendLCT (LMLA* ltreeP, LCT lct, SVX svX)
{
  SVMap*   svmapP = ltreeP->streeP->svmapP;
  SVocab* svocabP = svmapP->svocabP;
  LingKS*    lksP = svmapP->lksP;
  LVX         lvX = svmapGet (svmapP, svX);
  LCT      oldLCT = lct;

  /* check for hashed lct,svx -> lct2 map */
  if (ltreeP->svxHash) {
    int idx;
    HashSVX    hkey;
    hkey.lct = oldLCT;
    hkey.svx = svX;
    idx      = listIndex ((List*) & ltreeP->hsvxList, (void*) &hkey, 0);   
    if (idx >= 0) {
      idx = idx % ltreeP->hsvxList.itemMax;
      return ltreeP->hsvxList.itemA[idx].lct2; 
    }
  }

  if (CONSTRAINT_EXACT(ltreeP)) {
    /* -----------------------------------------------
         YES, we use the exact lattice LM constraint
	 We want to extend lct with svX
       ----------------------------------------------- */
    LatLMNode hkey, *nodeP, *childP;
    int       i, idx;

    nodeP  = ltreeP->latlm.list.itemA+lct;

    /* Create a list of allowable GNodes */
    hkey.svX    = svX;
    hkey.lct    = lksP->extendLCT (lksP, nodeP->lct, lvX);
    hkey.nct    = (ltreeP->svmapP == svmapP) ? hkey.lct : 
	ltreeP->svmapP->lksP->extendLCT (ltreeP->svmapP->lksP, nodeP->nct,
					 svmapGet (ltreeP->svmapP, svX));
    hkey.gnodeA = ltreeP->latlm.tmpA;
    hkey.gnodeN = 0;
    for (i = 0; i < nodeP->gnodeN; i++)
      hkey.gnodeN = addGNodes (ltreeP, nodeP->gnodeA[i], svX, ltreeP->latlm.tmpA, hkey.gnodeN);

    /* Get the matching GNode */
    idx = listIndex ((List*) &(ltreeP->latlm.list), (void*) &hkey, 1);
    
    childP = ltreeP->latlm.list.itemA+idx;
    if (childP->gnodeA == ltreeP->latlm.tmpA) {
      /* Create a new GNode */
      childP->gnodeA = bmemAlloc (ltreeP->latlm.buffer, childP->gnodeN*sizeof(GNode*));
      memcpy (childP->gnodeA, ltreeP->latlm.tmpA, childP->gnodeN*sizeof (GNode*));
    }

    if (ltreeP->svxHash) {
      HashSVX     hkey;
      hkey.lct  = oldLCT;
      hkey.lct2 = (LCT) idx;
      hkey.svx  = svX;
      listIndex ((List*) & ltreeP->hsvxList, (void*) &hkey, 1);
    }

    return (LCT) idx;

  } else { 
    /* ----------------------------------------
         NO lattice LM, simply extend the LCT
       ---------------------------------------- */
    LCT         nct = LCT_NIL;

    if (ltreeP->svmapP != svmapP) {
      int        idx;
      HashLCT    hkey;
      hkey.lct = lct;
      idx      = listIndex ((List*) & ltreeP->hlctList, (void*) &hkey, 0);
      idx      = idx % ltreeP->hlctList.itemMax;
      nct      = ltreeP->svmapP->lksP->extendLCT (ltreeP->svmapP->lksP, ltreeP->hlctList.itemA[idx].nct,
						  svmapGet (ltreeP->svmapP, svX));
    }
    
    if (!svocabP->list.itemA[svX].fillerTag)
      lct = lksP->extendLCT (lksP, lct, lvX);

    if (ltreeP->svmapP != svmapP) {
      int        idx;
      HashLCT    hkey;
      hkey.lct = lct;
      hkey.nct = nct;
      idx      = listIndex ((List*) & ltreeP->hlctList, (void*) &hkey, 1);
    }

    if (ltreeP->svxHash) {
      HashSVX     hkey;
      int idx;
      hkey.lct  = oldLCT;
      hkey.lct2 = lct;
      hkey.svx  = svX;
      idx       = listIndex ((List*) & ltreeP->hsvxList, (void*) &hkey, 1);
    }

    return lct;
  }
}


/* ------------------------------------------------------------------------
    DecodeLCT
   -----------
    Produces a list of lvX from an lct
   ------------------------------------------------------------------------ */

extern int DecodeLCT (LMLA* ltreeP, LCT lct, LVX* lvxA, int n)
{
  LingKS* lksP = ltreeP->streeP->svmapP->lksP;
  return lksP->decodeLCT (lksP, TrueLCT (ltreeP, lct), lvxA, n);
}


/* ------------------------------------------------------------------------
    CreateLCT
   -----------
    Starts a new LCT (i.e. sth like <s> <s>), might reset the LM/ Grammar
    Also, sets bitmask to a sensible value for LModelNJD and PhraseLM
   ------------------------------------------------------------------------ */

extern LCT CreateLCT (LMLA* ltreeP, SVX svX)
{
  SVMap* svmapP = ltreeP->svmapP;
  LingKS*  lksP = ltreeP->streeP->svmapP->lksP;
  LVX       lvX = svmapGet (ltreeP->streeP->svmapP, svX);
  LCT       lct = lksP->createLCT (lksP, lvX);

  /* clear cache of lm scores in svmap */
  svmapClearCache(ltreeP->streeP->svmapP);

  /* Deal with the hashed LCTs */
  ltreeP->lctHash = (svmapP != ltreeP->streeP->svmapP) ? 1 : 0;

  listClear ((List*) &ltreeP->hlctList);
  listClear ((List*) &ltreeP->hsvxList);

  if (ltreeP->latlm.latP && ltreeP->latlm.latP->nodeN +  ltreeP->latlm.latP->linkN < 2) {
    WARN("CreateLCT: detected empty lattice, constraint deacivated!\n");
    lmlaClearConstraint (ltreeP);
    ltreeP->latlm.latP = NULL;
  }

  if (ltreeP->latlm.latP) {
    if (NULL == (ltreeP->latlm.tmpA = (GNode**) 
		 realloc(ltreeP->latlm.tmpA,ltreeP->latlm.latP->nodeN*sizeof(GNode*)))) {
      ERROR("CreateLCT: allocation failure\n");
      return LCT_NIL;
    } 
    lmlaClearConstraint (ltreeP);

    if (ltreeP->latlm.mode == Exact_Constraint) {
      /* Exact constraint */
      LatLMNode  hkey;
      LatLMNode* nodeP;
     
      /* Create a 'dead' node */
      hkey.lct             = LCT_NIL;
      hkey.nct             = LCT_NIL;
      hkey.svX             = SVX_NIL;
      hkey.gnodeN          = 1;
      hkey.gnodeA          = (GNode**) bmemAlloc (ltreeP->latlm.buffer, sizeof (GNode*));
      ltreeP->latlm.deadX  = listIndex ((List*) &(ltreeP->latlm.list), (void*) &hkey, 1);
      nodeP = ltreeP->latlm.list.itemA+ltreeP->latlm.deadX;
      nodeP->gnodeA[0]     = NULL;

      /* Create the 'start' node */
      hkey.lct             = lct;
      hkey.nct             = (ltreeP->streeP->svmapP == svmapP) ? lct : 
	svmapP->lksP->createLCT (svmapP->lksP, svmapGet (svmapP, svX));
      hkey.svX             = svX;
      hkey.gnodeN          = 1;
      hkey.gnodeA          = (GNode**) bmemAlloc (ltreeP->latlm.buffer, sizeof (GNode*));
      ltreeP->latlm.startX = listIndex ((List*) &(ltreeP->latlm.list), (void*) &hkey, 1);
      nodeP = ltreeP->latlm.list.itemA+ltreeP->latlm.startX;
      nodeP->gnodeA[0]     = ltreeP->latlm.latP->startP;

      lct = (LCT) ltreeP->latlm.startX;

    } else {

      /* Weak or Flat or Time constraint */
      glatExtractConstraint (ltreeP->latlm.latP,ltreeP->streeP->svmapP,ltreeP->latlm.mode);  
    }
  }

  if (ltreeP->latlm.latP                    ||
      svmapIsDirty                 (svmapP) || 
      svmapIsDirty (ltreeP->streeP->svmapP)) {
    int i;

    /* ------------------
         Clear cache
    ------------------ */
    for (i = 0; i < ltreeP->cache.N; i++)
      ltreeP->cache.lctA[i] = ltreeP->cache.nctA[i] = LCT_NIL;
    for (i = 0; i < ltreeP->nodeCache.N; i++)
      ltreeP->nodeCache.nctA[i] = LCT_NIL;

    svmapClearCache (svmapP);
    svmapClearCache (ltreeP->streeP->svmapP);

    if (ltreeP->expert && !ltreeP->latlm.latP)
      INFO ("discarded cache because SVMap or LingKS was dirty.\n");
  }

  /* Deal with the hash function */
  if (ltreeP->streeP->svmapP != svmapP) {
    HashLCT    hkey;
    int        idx;
    hkey.lct = lct;
    hkey.nct = svmapP->lksP->createLCT (svmapP->lksP, svmapGet (svmapP, svX));
    idx      = listIndex ((List*) &ltreeP->hlctList, (void*) &hkey, 1);
  }

  return lct;
}


/* ------------------------------------------------------------------------
     FreeLCT
   -----------
     Frees the LCT, i.e. declares that the search no longer wants to use it
   ------------------------------------------------------------------------ */

extern int FreeLCT (LMLA* ltreeP, LCT lct)
{
  if (ltreeP->svmapP != ltreeP->streeP->svmapP) {
    HashLCT    hkey;
    int        idx;
    LingKS*    lksP = ltreeP->streeP->svmapP->lksP;
    hkey.lct = lct;
    idx      = listIndex ((List*) & ltreeP->hlctList, (void*) &hkey, 0);
    idx      = idx % ltreeP->hlctList.itemMax;
    lksP->freeLCT (lksP,  ltreeP->hlctList.itemA[idx].nct);
  }
  return ltreeP->streeP->svmapP->lksP->freeLCT (ltreeP->streeP->svmapP->lksP, lct);
}


/* ========================================================================
    LTree Score comoutations
   ======================================================================== */

/* ------------------------------------------------------------------------
    lmlaScoreArray
   ----------------
    This procedure filters a scoreArray through the skeleton provided by 
    a lattice - if we use LatticeLM
   ------------------------------------------------------------------------ */

static int copyLeafs (lmScore* tA, lmScore* sA, GLink* glinkP,  SVMap* svmapP, int what)
{
  SWord* swordP = svmapP->svocabP->list.itemA;
  while (glinkP) {
    SVX svX = glinkP->childP->svX;
    if (swordP[svX].fillerTag) {
      copyLeafs (tA, sA, glinkP->childP->childP, svmapP,what);
    } else {
      if (what == 0) {
	sA[svX] = tA[svX];
      } else {
	LVX lvX  = svmapGet(svmapP,svX);
	int idx  = svmapP->rev.N[lvX-1]+1;
	int idxB = svmapP->rev.N[lvX];
	while (idx<=idxB) {
	  sA[ svmapP->rev.X[idx] ] = tA[ svmapP->rev.X[idx] ];
	  idx++;
	}
      }
    }
    glinkP = glinkP->nextP;
  }
  return TCL_OK;
}

static int zeroLeafs (lmScore* sA, GLink* glinkP, SVMap* svmapP, int what)
{
  SWord* swordP = svmapP->svocabP->list.itemA;
  while (glinkP) {
    SVX svX = glinkP->childP->svX;
    if (swordP[svX].fillerTag) {
      zeroLeafs (sA, glinkP->childP->childP, svmapP,what);
    } else {
      if (what == 0) {
	sA[svX] = 0;
      } else {
	LVX lvX  = svmapGet(svmapP,svX);
	int idx  = svmapP->rev.N[lvX-1]+1;
	int idxB = svmapP->rev.N[lvX];
	while (idx<=idxB) {
	  sA[ svmapP->rev.X[idx] ] = 0;
	  idx++;
	}
      }
    }
    glinkP = glinkP->nextP;
  }
  return TCL_OK;
}

static int lmlaScoreArray (LMLA* ltreeP, SVMap* svmapP, LCT lct, lmScore* sA, int reduced)
{
  svmapP->lksP->scoreArrayFct (svmapP->lksP, lct, sA, reduced);
  svmapDoMapping (svmapP, sA);
  return TCL_OK;
}

/* ------------------------------------------------------------------------
    lmlaComputeNodes
   ------------------
    Does the computation of an LTree for a given LCT
   ------------------------------------------------------------------------ */

static int lmlaComputeNodes (LMLA *lmlaP, lmScore *baseA, lmScore *nodeA)
{
  lmScore   *nodeP, *nodeE;
  int       *typeP, *argsA, *argsP, *argsE;

  /* Initialize pointers */
  nodeA[0] = 0; /* Dummy node with increment 0 */
  nodeE    = nodeA + lmlaP->nodesN;
  nodeP    = nodeA + 1;
  typeP    = lmlaP->typeA + 1;
  argsA    = argsP = argsE = lmlaP->argsA;

  /* Main loop */
  while (nodeP < nodeE) {
    argsE += *typeP++;
    *nodeP = baseA[*argsP++];
    while (argsP < argsE) {
      if (baseA[*argsP++] > *nodeP)
	*nodeP = baseA[argsP[-1]];
    }
    nodeP++;
  }

  return TCL_OK;
}

/* Indirection for svmapGetLMScore */
static lmScore searchLat (LMLA* ltreeP, GLink* glinkP, SVX svY, LVX lvY)
{
  SVMap* svmapP = ltreeP->streeP->svmapP;
  SWord* swordP = svmapP->svocabP->list.itemA;
 
  while (glinkP) {
    SVX svX = glinkP->childP->svX;

    if ( (ltreeP->latlm.what == 0 && svY == svX) || 
	 (ltreeP->latlm.what == 1 && lvY == svmapGet(svmapP,svX)) ||
	 (swordP[svX].fillerTag && 
	  searchLat (ltreeP,glinkP->childP->childP, svY,lvY) == TCL_OK))
      return TCL_OK;

    glinkP = glinkP->nextP;
  }
  return TCL_ERROR;
}

lmScore GetOneLMScore (LMLA* ltreeP, LCT lct, SVX svX)
{
  SVMap* svmapP = ltreeP->streeP->svmapP;
 
  if (CONSTRAINT_EXACT(ltreeP)) {
    LVX  lvX = LVX_NIL;
    int k;
    if (ltreeP->latlm.what == 1) {
      lvX = svmapGet(svmapP,svX);
    }
    lct = TrueLCT (ltreeP, lct);
    for (k = 0; k < c.p->gnodeN; k++) {
      if (searchLat (ltreeP, c.p->gnodeA[k]->childP, svX, lvX) == TCL_OK)
	return svmapGetLMScore (svmapP, lct, svX);
    }
    return LMSCORE_MAX;
  }
  return svmapGetLMScore (svmapP, lct, svX);
}
  

/* ------------------------------------------------------------------------
    lmlaFillContext
   -----------------
    This procedure fills the LMLA structure with the scores for a specific
    linguistic context, it will first find a place in the cache for it.
    The pointer 'lmScore *lnX' on exit points to the base address of the
    table of internal node scores of the LTree/ STree, while 'lmScore *svX'
    will point to a table of scores for search words.
    If 'svX == NULL' on entry, the leafs won't be calculated, if the nodes
    were found in the cache.
   ------------------------------------------------------------------------ */

#define searchCacheM(LCTA,USEA,LIN,VAR,UX) \
for (VAR = 0, t = (USEA)[0], UX = 0; (LCTA)[VAR] != LIN && (LCTA)[VAR] != LCT_NIL; VAR++) { \
  if ((USEA)[VAR] < t)  { t = (USEA)[VAR], UX = VAR; } \
}

enum LMLAStatus { STATUS_NEEDED, STATUS_OK, STATUS_UNWANTED };

static int recomputeNodes (LMLA* lmlaP, lmScore* sA, LCT nct)
{
  if (!lmlaP->latlm.latP || lmlaP->latlm.mode == Time_Constraint) {
    lmlaScoreArray   (lmlaP, lmlaP->svmapP, nct, sA, lmlaP->reduced); 
    lmlaComputeNodes (lmlaP, sA, sA+lmlaP->leafsN);

  } else {
    lmScore* tA = lmlaP->cache.scorePA[lmlaP->cache.N];
    int       k;

    /* Clear the final array of scores */
    for (k = 0; k < lmlaP->leafsN; k++)
      sA[k] = LMSCORE_MAX;  

    /* Compute the scores and the nodes,
       in case of exact constraint, do look-up,
       get 0/1 leaf scores and compute tree */
    lmlaScoreArray   (lmlaP, lmlaP->svmapP, nct, tA, lmlaP->reduced); 
    lmlaComputeNodes (lmlaP, tA, tA+lmlaP->leafsN);  
    if (lmlaP->latlm.mode == Exact_Constraint) {
      for (k = 0; k < c.p->gnodeN; k++)                         
	zeroLeafs      (sA, c.p->gnodeA[k]->childP, lmlaP->streeP->svmapP,lmlaP->latlm.what);
    } else {
      glatFillCtx      (lmlaP->latlm.latP, nct, sA, lmlaP->latlm.mode);
    }
    lmlaComputeNodes   (lmlaP, sA, sA+lmlaP->leafsN);             
    
    /* Smear the lookahead */
    for (k = 0; k < lmlaP->leafsN + lmlaP->nodesN; k++)
      if (sA[k] > LMSCORE_MAX)
	sA[k] = tA[k];
  }
  
  return TCL_OK;
}

static int recomputeLeafs (LMLA* lmlaP, lmScore* sA, LCT lct)
{
  if (!lmlaP->latlm.latP || lmlaP->latlm.mode == Time_Constraint) {
    lmlaScoreArray (lmlaP, lmlaP->streeP->svmapP, lct, sA, 0);
    
  } else {
    lmScore* tA = lmlaP->cache.scorePA[lmlaP->cache.N];
    int       k;

    /* Clear the final array of scores */
    for (k = 0; k < lmlaP->leafsN; k++)
      sA[k] = LMSCORE_MAX;  

    /* Compute scores,
       get 0/1 leaf score and compute tree in tA */
    lmlaScoreArray (lmlaP, lmlaP->streeP->svmapP, lct, tA, 0);
    if (lmlaP->latlm.mode == Exact_Constraint) {
      for (k = 0; k < c.p->gnodeN; k++)
	copyLeafs (tA, sA, c.p->gnodeA[k]->childP, lmlaP->streeP->svmapP, lmlaP->latlm.what);
    } else {
      glatFillCtx (lmlaP->latlm.latP, lct, sA, lmlaP->latlm.mode);
      for (k = 0; k <lmlaP->leafsN ; k++)
	if (!sA[k]) sA[k] = tA[k];
    }
  }

  return TCL_OK;
}


int lmlaFillCtx (LMLA *lmlaP, LCT lct, lmScore **lnX, lmScore **svX)
{
  int  leafsStatus = (svX) ? STATUS_NEEDED : STATUS_UNWANTED;
  int  nodesStatus = (lnX) ? STATUS_NEEDED : STATUS_UNWANTED;
  LCT          nct = lct2nct (lmlaP, lct);
  int                i, j, vX, uX, t;
  lmScore*           sA;

  lmlaP->cache.queries++;
  if (leafsStatus == STATUS_NEEDED) {
    /* =============================
         Search main cache for lct
       ============================= */
    searchCacheM (lmlaP->cache.lctA, lmlaP->cache.useA, lct, i, uX);
    if (lct == lmlaP->cache.lctA[i]) {
      *svX = lmlaP->cache.scorePA[i];
      if (nodesStatus == STATUS_NEEDED)
	*lnX = lmlaP->cache.scorePA[i] + lmlaP->leafsN;
      lmlaP->cache.useA[i] = lmlaP->cache.queries;
      lmlaP->cache.lastX   = i;
      lmlaP->cache.hits++;
      return TCL_OK;
    }
  } else if (nodesStatus == STATUS_NEEDED) {
    /* =============================
         Search main cache for nct
       ============================= */
    searchCacheM (lmlaP->cache.nctA, lmlaP->cache.useA, nct, i, uX);
    if (nct == lmlaP->cache.nctA[i]) {
      *lnX = lmlaP->cache.scorePA[i] + lmlaP->leafsN;
      /* lmlaP->cache.useA[i] = lmlaP->cache.queries;
         lmlaP->cache.lastX   = i; */
      lmlaP->cache.hits++;
      return TCL_OK;
    }
  } else {
    /* If we need neither leafs nor nodes, what are we doing here? */
    return TCL_OK;
  }
  
  /* No luck in main cache, do we have unused cache lines there? */
  if (i < lmlaP->cache.N) uX = i;
  sA = lmlaP->cache.scorePA[uX];
  
  
  /* =====================
       Search node cache
     ===================== */
  if (nodesStatus == STATUS_NEEDED) {
    searchCacheM (lmlaP->nodeCache.nctA, lmlaP->nodeCache.useA, nct, j, vX);
    
    if (nct == lmlaP->nodeCache.nctA[j]) {
      /* -----------------------------
	   Cache hit for nodes cache 
	 ----------------------------- */
      if (leafsStatus != STATUS_NEEDED) {
	/* ----------------------------------------
	     We don't need the leafs, so we're ok
	   ---------------------------------------- */
	*lnX = lmlaP->nodeCache.scorePA[j];
	lmlaP->nodeCache.useA[j] = lmlaP->cache.queries;
	lmlaP->nodeCache.hits++;
	return TCL_OK;
      } else {
	/* --------------------------------------
	     Get the nodes into the main cache,
	     we no longer need to compute them
	   -------------------------------------- */
	lmScore* nA = lmlaP->nodeCache.scorePA[j];
	lmScore* tA = lmlaP->cache.scorePA[lmlaP->cache.N];
	int       n = lmlaP->nodesN*sizeof(lmScore);
	
	memcpy (tA,               sA+lmlaP->leafsN, n);
	memcpy (sA+lmlaP->leafsN, nA,               n);
	memcpy (nA,               tA,               n);
	lmlaP->nodeCache.nctA[j] = lmlaP->cache.nctA[uX];
	lmlaP->nodeCache.useA[j] = lmlaP->cache.useA[uX];
      }
      
    } else { 
      /* -------------------------------------------
	   Save the nodes we're going to overwrite
	   to the nodes cache, if need be
         ------------------------------------------- */
      if (j < lmlaP->nodeCache.N) vX = j;
      if (lmlaP->cache.lctA[uX] != LCT_NIL) {
	memcpy (lmlaP->nodeCache.scorePA[vX], sA+lmlaP->leafsN, lmlaP->nodesN*sizeof(lmScore));
	lmlaP->nodeCache.nctA[vX] = lmlaP->cache.nctA[uX];
	lmlaP->nodeCache.useA[vX] = lmlaP->cache.useA[uX];
      }
      
      /* ---------------------------------
	   We need to fill in the nodes,
	   are we computing the leafs at
	   the same time?
	 --------------------------------- */
      recomputeNodes (lmlaP, sA, TrueNCT (lmlaP, lct, nct));    
      if (leafsStatus == STATUS_NEEDED && (lmlaP->streeP->svmapP == lmlaP->svmapP && !lmlaP->reduced))
	leafsStatus = STATUS_OK;
    }
    nodesStatus = STATUS_OK;
  }


  /* ================================
       We need to fill in the leafs
     ================================ */
  if (leafsStatus == STATUS_NEEDED) {
    recomputeLeafs (lmlaP, sA, TrueLCT (lmlaP, lct));
    leafsStatus = STATUS_OK;
  }
  
  /* Book-keeping */
  lmlaP->cache.lctA[uX] = lct;
  lmlaP->cache.nctA[uX] = nct;
  lmlaP->cache.useA[uX] = lmlaP->cache.queries;
  lmlaP->cache.lastX    = uX;

  if (nodesStatus == STATUS_OK) *lnX = sA + lmlaP->leafsN;
  if (leafsStatus == STATUS_OK) *svX = sA;
  
  return TCL_OK;
}


/* ------------------------------------------------------------------------
    lmlaPhones
   ------------
    This procedure computes LmLa-scores for the basephones
    (It has not been cleaned thoroughly for multiple LingKSs)
   ------------------------------------------------------------------------ */

lmScore* lmlaPhones (LMLA* lmlaP, LCT lct)
{
  lmScore   *lmA = lmlaP->cache.scorePA[lmlaP->cache.N];
  lmScore   *phA = NULL;

  CTX*   phoneXA = lmlaP->lmrcm.w2p;
  int      leafN = lmlaP->leafsN;
  int          u = lmlaP->lmrcm.queries;
  int         uX = 0;
  int          i;

  lct = lct2nct (lmlaP, lct);

  /* check lm-rcm cache */
  lmlaP->lmrcm.queries++;
  for (i = 0; lct     != lmlaP->lmrcm.lctA[i] && 
	      LCT_NIL != lmlaP->lmrcm.lctA[i] &&
	            i <  lmlaP->lmrcm.cacheN;    i++)
    if (lmlaP->lmrcm.useA[i] < u) u = lmlaP->lmrcm.useA[i], uX = i;


  if (lct == lmlaP->lmrcm.lctA[i]) {
    return lmlaP->lmrcm.scoreA+lmlaP->lmrcm.phoneN*i;
  }
 
  i = (lmlaP->lmrcm.lctA[i] == LCT_NIL || lmlaP->lmrcm.useA[i] < u) ? i : uX;
  lmlaP->lmrcm.lctA[i] = lct;
  lmlaP->lmrcm.useA[i] = lmlaP->lmrcm.queries;
  phA= lmlaP->lmrcm.scoreA+lmlaP->lmrcm.phoneN*i;


  /* reset phone cache */
  for (i = 0; i < lmlaP->lmrcm.phoneN; i++)
    phA[i] = LMSCORE_MAX;  

  /* check ltree cache */
  i = 0;
  while(lct != lmlaP->cache.nctA[i] && i < lmlaP->cache.N-1) i++;
  if (lct == lmlaP->cache.nctA[i]) { 
    SRoot**  rootPA = lmlaP->streeP->rootTbl.PA;
    lmScore*  nodeA = lmlaP->cache.scorePA[i] + lmlaP->leafsN;
    for (i = 0; i < lmlaP->streeP->rootTbl.N; i++) {
      int phoneX = rootPA[i]->phoneX;
      int lnX    = rootPA[i]->lnX;
      if (nodeA[lnX] > phA[phoneX])
	phA[phoneX] = nodeA[lnX];
    }
    return phA;
  }

  /* otherwise compute score directly from the leafs */
  lmlaScoreArray (lmlaP, lmlaP->streeP->svmapP, lct, lmA, lmlaP->reduced);

  for (i = 0; i < leafN; i++) {
    int phoneX = phoneXA[i];
    if (lmA[i] > phA[phoneX])
      phA[phoneX] = lmA[i];
  }

  return phA;
}

/* ------------------------------------------------------------------------
    lmlaTimeConstraintSVX
   ------------------------------------------------------------------------ */

int lmlaTimeConstraint_SVX (LMLA* lmlaP, int frameX, SVX svX)
{
  if (! lmlaP->latlm.latP ||  lmlaP->latlm.mode != Time_Constraint) return 1;
  if (! lmlaP->latlm.validA) {
    if (NULL == (lmlaP->latlm.validA = (lmScore*) 
		 malloc(sizeof(lmScore)*(lmlaP->nodesN+lmlaP->leafsN)))) {
      ERROR("lmlaTimeConstraint_SVX: allocation failure\n");
      return 1;
    }
  }

  if (frameX != lmlaP->latlm.frameX) {
    SVMap*   svmapP = lmlaP->svmapP;
    GLat*      latP = lmlaP->latlm.latP;
    int        padX = lmlaP->latlm.padX;
    lmScore* scoreA = lmlaP->latlm.validA;    
    int i;
    /* make all words invalid */
    for (i=0;i<lmlaP->leafsN;i++) scoreA[i] = LMSCORE_MAX;
    /* loop over all lattice nodes to decide which words are valid */
    for (i=0;i<latP->nodeN;i++) {
      GNode* nodeP = latP->mapping.x2n[i];
      if (lmlaP->svmapP->svocabP->list.itemA[nodeP->svX].fillerTag == 1) {
	scoreA[(int) nodeP->svX] = 0;
      } else {
	if (nodeP && nodeP->frameS-padX <= frameX &&  nodeP->frameE+padX >= frameX) {
	  if (lmlaP->latlm.what == 0) {
	    scoreA[(int) nodeP->svX] = 0;
	  } else {
	    LVX lvX  = svmapGet(svmapP,nodeP->svX);
	    int idx  = svmapP->rev.N[lvX-1]+1;
	    int idxB = svmapP->rev.N[lvX];
	    while (idx<=idxB) {
	      scoreA[ svmapP->rev.X[idx++] ] = 0;
	    }
	  }
	}
      }
    }
    /* compute lookahead tree over valid words */
    lmlaComputeNodes(lmlaP,scoreA, scoreA+lmlaP->leafsN);   
    /* update frame index */
    lmlaP->latlm.frameX = frameX;
  }

  /* return 1 if word is valid */
  if (lmlaP->latlm.validA[(int) svX] == 0) return 1; else return 0;

}

/* ------------------------------------------------------------------------
    lmlaTimeConstraintLNX
   ------------------------------------------------------------------------ */

int lmlaTimeConstraint_LNX (LMLA* lmlaP, int frameX, LVX lnX)
{
  if (! lmlaP->latlm.latP ||  lmlaP->latlm.mode != Time_Constraint) return 1;
  if (! lmlaP->latlm.validA) {
    if (NULL == (lmlaP->latlm.validA = (lmScore*) 
		 malloc(sizeof(lmScore)*(lmlaP->nodesN+lmlaP->leafsN)))) {
      ERROR("lmlaTimeConstraint_LNX: allocation failure\n");
      return 1;
    }
  }
  if (frameX != lmlaP->latlm.frameX) {
    SVMap*   svmapP = lmlaP->svmapP;
    GLat*      latP = lmlaP->latlm.latP;
    int        padX = lmlaP->latlm.padX;
    lmScore* scoreA = lmlaP->latlm.validA;    
    int i;
    /* make all words invalid */
    for (i=0;i<lmlaP->leafsN;i++) scoreA[i] = LMSCORE_MAX;
    /* loop over all lattice nodes to decide which words are valid */
    for (i=0;i<latP->nodeN;i++) {
      GNode* nodeP = latP->mapping.x2n[i];
      if (lmlaP->svmapP->svocabP->list.itemA[nodeP->svX].fillerTag == 1) {
	scoreA[(int) nodeP->svX] = 0;
      } else {
	if (nodeP && nodeP->frameS-padX <= frameX &&  nodeP->frameE+padX >= frameX) {
	  if (lmlaP->latlm.what == 0) {
	    scoreA[(int) nodeP->svX] = 0;
	  } else {
	    LVX lvX  = svmapGet(svmapP,nodeP->svX);
	    int idx  = svmapP->rev.N[lvX-1]+1;
	    int idxB = svmapP->rev.N[lvX];
	    while (idx<=idxB) {
	      scoreA[ svmapP->rev.X[idx++] ] = 0;
	    }
	  }
	}
      }
    }
    /* compute lookahead tree over valid words */
    lmlaComputeNodes(lmlaP,scoreA, scoreA+lmlaP->leafsN);   
    /* update frame index */
    lmlaP->latlm.frameX = frameX;
  }

  /* return 1 if word is valid */
  if (lmlaP->latlm.validA[lmlaP->leafsN+(int) lnX] == 0) return 1; else return 0;

}

/* ========================================================================
    LTree Construction Area
   ======================================================================== */

/* ------------------------------------------------------------------------
    compressSTree
   ---------------
    Procedure to compress a STree into the corresponding LTree structure
    Also fills in the pointers from the STree to the LTree
   ------------------------------------------------------------------------ */

static int* _stackP = NULL;
static int* _stackA = NULL;

static int compressSTree (SNode *snP, LMLA* lmlaP, CTX basephoneX, int level)
{
  int     *stackIn = _stackP;
  int     *p;

  /* ---------------------------------- */
  /* Create new LTree leaf              */
  /* ---------------------------------- */
  if (snP->info == WordIsUnique || snP->childN == 0) {
    SVX w               = snP->vX.s;  /* This is our word index */
    lmlaP->lmrcm.w2p[w] = basephoneX; /* attach basephone index
                                         for phone lm lookahead */
    *_stackP++    = (int)w;          /* Put words on the stack */
  } else if (snP->childN > 0) { 
    int childN = snP->childN-1;
    /* kind of depth first strategy */
    for(;childN>=0;childN--)
      compressSTree (snP->childPA[childN], lmlaP, basephoneX, level+1);
  }

  /* ---------------------------------- */
  /* Collect info from children         */
  /* ---------------------------------- */
  if (snP->info == WordIsNotUnique) {

    /* ---------------------------------- */
    /* case 1: create node                */
    /* ---------------------------------- */

    if (_stackP - stackIn > 1 && level < lmlaP->depth) {
      /* Link ltree with stree */
      snP->vX.l = (LVX)(lmlaP->typeP - lmlaP->typeA);

      /* Copy the arguments from stack to argA */
      for (p = stackIn; p < _stackP; p++)
	*lmlaP->argsP++ = *p;

      /* Put the new node's link on the stack  */
      *stackIn = lmlaP->typeP - lmlaP->typeA + lmlaP->leafsN;

      /* Create the node in typeA */
      *lmlaP->typeP++ = _stackP - stackIn;

      /* Shrink stack */
      _stackP = 1 + stackIn;
     
    } else {
    
    /* ---------------------------------- */
    /* case 2: bypass information         */
    /* ---------------------------------- */

      /* Kill the children (kill 'em all) */
      int childN = snP->childN-1;
      for (;childN>=0;childN--) {
	SNode* childP = snP->childPA[childN];
	if (childP->info == WordIsNotUnique) {
	  snP->vX.l    = childP->vX.l;
	  childP->vX.l = LTREE_NULL_NODE;
	}
      }
    }
  }

  return TCL_OK;
}

/* ------------------------------------------------------------------------
    compressSTree_SRoot
   ---------------------
    Wrapper for compressSTree
   ------------------------------------------------------------------------ */

static int compressSTree_SRoot (SRoot* srootP, LMLA* lmlaP)
{
  int*  stackIn = _stackP;
  int*        p = NULL;
  int    childN = srootP->childN-1;

  if (childN <0) {
    srootP->lnX= LTREE_NULL_NODE;
    return TCL_OK;
  }

  for(;childN>=0;childN--) 
    compressSTree (srootP->childPA[childN],lmlaP,srootP->phoneX,1);

  /* Link ltree with stree */
  srootP->lnX = (LVX)(lmlaP->typeP - lmlaP->typeA);

  for (p = stackIn; p < _stackP; p++)
    *lmlaP->argsP++ = *p;

  *stackIn = lmlaP->typeP - lmlaP->typeA + lmlaP->leafsN;
  *lmlaP->typeP++ = _stackP - stackIn;

  return TCL_OK;
}

/* ------------------------------------------------------------------------
    lmlaConfigureCache
   --------------------
    Configures the cacheing of the language model in the LookAhead tree
   ------------------------------------------------------------------------ */

static int deleteLeafC (LMLA* lmlaP)
{
  int i;
  if (lmlaP->cache.N > 0) {
    if (lmlaP->cache.scorePA)
      for (i = 0; i <= lmlaP->cache.N; i++)
	Nulle (lmlaP->cache.scorePA[i]);
    Nulle (lmlaP->cache.scorePA);
    Nulle (lmlaP->cache.lctA);
    Nulle (lmlaP->cache.nctA);
    Nulle (lmlaP->cache.useA);
  }
  return TCL_OK;
}

static int deleteNodeC (LMLA* lmlaP)
{
  int i;
  if (lmlaP->nodeCache.N > 0) {
    if (lmlaP->nodeCache.scorePA)
      for (i = 0; i <= lmlaP->nodeCache.N; i++)
	Nulle (lmlaP->nodeCache.scorePA[i]);
    Nulle (lmlaP->nodeCache.scorePA);
    Nulle (lmlaP->nodeCache.nctA);
    Nulle (lmlaP->nodeCache.useA);
  }
  return TCL_OK;
}

static int lmlaConfigureCache (LMLA *lmlaP, int cacheN, int ncacheN)
{
  int i;
  int cacheSize = lmlaP->leafsN + lmlaP->nodesN;

  if (cacheN == 0 && ncacheN == 0) {
    deleteLeafC (lmlaP);
    deleteNodeC (lmlaP);
    lmlaP->cache.N = lmlaP->nodeCache.N = -1;
    return TCL_OK;
  }

  if (cacheN <= 0 || ncacheN <= 0) {
    ERROR ("lmlaConfigureCache: invalid arguments %d / %d.\n", cacheN, ncacheN);
    return TCL_ERROR;
  }

  /* Initialize cache */
  if (lmlaP->cache.N == -1) {
    lmlaP->cache.scorePA = NULL;
    lmlaP->cache.lctA    = NULL;
    lmlaP->cache.nctA    = NULL;
    lmlaP->cache.useA    = NULL;
    lmlaP->nodeCache.scorePA = NULL;
    lmlaP->nodeCache.nctA    = NULL;
    lmlaP->nodeCache.useA    = NULL;
    lmlaP->nodeCache.N       = -1;
  }

  lmlaP->cache.queries  = 0;
  lmlaP->cache.lastX    = 0;
  lmlaP->cache.hits     = 0;
  lmlaP->nodeCache.hits = 0;

  if (lmlaP->cache.N != cacheN) {
    /* Reset leafs cache */
    deleteLeafC (lmlaP);

    /* Re-built leafs cache */
    lmlaP->cache.lctA    = (LCT*)      malloc ((cacheN+1) * sizeof (LCT));
    lmlaP->cache.nctA    = (LCT*)      malloc ((cacheN+1) * sizeof (LCT));
    lmlaP->cache.useA    = (int*)      malloc ((cacheN+1) * sizeof (int));
    lmlaP->cache.scorePA = (lmScore**) malloc ((cacheN+1) * sizeof (lmScore*));
    for (i = 0; i <= cacheN; i++)
      lmlaP->cache.scorePA[i] = (lmScore*) malloc (cacheSize * sizeof (lmScore));
  }
  
  /* Initialize leafs cache */
  for (i = 0; i <= cacheN; i++)
    lmlaP->cache.lctA[i] = lmlaP->cache.nctA[i] = LCT_NIL, lmlaP->cache.useA[i] = 0;
  lmlaP->cache.N       = cacheN;
  lmlaP->cache.hits    = 0;
  
  if (lmlaP->nodeCache.N != ncacheN) {
    /* Reset nodes cache */
    deleteNodeC (lmlaP);

    /* Re-built nodes cache */
    lmlaP->nodeCache.nctA    = (LCT*)      malloc ((ncacheN+1) * sizeof (LCT));
    lmlaP->nodeCache.useA    = (int*)      malloc ((ncacheN+1) * sizeof (int));
    lmlaP->nodeCache.scorePA = (lmScore**) malloc ((ncacheN+1) * sizeof (lmScore*));
    for (i = 0; i <= ncacheN; i++)
      lmlaP->nodeCache.scorePA[i] = (lmScore*) malloc (lmlaP->nodesN * sizeof (lmScore));
  }

  /* Initialize nodes cache */
  for (i = 0; i <= ncacheN; i++)
    lmlaP->nodeCache.nctA[i] = LCT_NIL, lmlaP->nodeCache.useA[i] = 0;
  lmlaP->nodeCache.N       = ncacheN;
  lmlaP->nodeCache.hits    = 0;

  return TCL_OK;
}


static int lmlaConfigurePhoneCache (LMLA *lmlaP, int cacheN)
{
  int i;

  if (lmlaP->lmrcm.cacheN == 0 && cacheN == 0) return TCL_OK; 

  lmlaP->lmrcm.cacheN   = cacheN;
  lmlaP->lmrcm.queries  = 0;

  /* cacheN= 0 -> realloc(0) = free */
  if (NULL == (lmlaP->lmrcm.lctA = realloc(lmlaP->lmrcm.lctA,
	       lmlaP->lmrcm.cacheN*sizeof(LCT)))) {
    ERROR("lmlaConfigurePhoneCache: allocation failure\n");
    return TCL_ERROR;
  }
  if (NULL == (lmlaP->lmrcm.scoreA = realloc(lmlaP->lmrcm.scoreA,
               lmlaP->lmrcm.cacheN*lmlaP->lmrcm.phoneN*sizeof(lmScore)))) {
    ERROR("lmlaConfigurePhoneCache: allocation failure\n");
    return TCL_ERROR;
  }
  if (NULL == (lmlaP->lmrcm.useA = realloc(lmlaP->lmrcm.useA,
               lmlaP->lmrcm.cacheN*sizeof(int)))) {
    ERROR("lmlaConfigurePhoneCache: allocation failure\n");
    return TCL_ERROR;
  }
  for (i=0;i<lmlaP->lmrcm.cacheN;i++)
    lmlaP->lmrcm.lctA[i] = LCT_NIL;

  return TCL_OK;
}

static int lmlaClearCache (LMLA *lmlaP)
{
  int i;

  for (i=0;i<=lmlaP->cache.N;i++) {
    lmlaP->cache.lctA[i]= LCT_NIL;
    lmlaP->cache.nctA[i]= LCT_NIL;
  }
  for (i=0;i<=lmlaP->nodeCache.N;i++) {
    lmlaP->nodeCache.nctA[i]= LCT_NIL;
  }
  for (i=0;i<lmlaP->lmrcm.cacheN;i++) {
    lmlaP->lmrcm.lctA[i]= LCT_NIL;
  }
  return TCL_OK;
}

/* ------------------------------------------------------------------------
    lmlaInit
   ------------------------------------------------------------------------ */

static int lmlaInit (LMLA* lmlaP, char* name, STree* streeP, SVMap* svmapP,
		     int reduced, int depth, int silent)
{
  /* default number of cache trees */
  int  cacheN = 100;
  int ncacheN =  10;
  int pcacheN =   0;
  int i;

  assert (streeP);

  /* -----------------------------------
       Basics & links to other objects 
     ----------------------------------- */
  lmlaP->name        = strdup(name);
  lmlaP->useN        = 0;
  lmlaP->expert      = 0;
  lmlaP->streeP      = streeP;
  lmlaP->svmapP      = svmapP;
  lmlaP->depth       = depth;
  lmlaP->reduced     = reduced;
  lmlaP->mode        = Array_Mode;
  lmlaP->cache.N     = -1;

  link (lmlaP->streeP, "STree");
  link (lmlaP->svmapP, "SVMap");

  /* ---------------------------
       allocate lookahead tree
     --------------------------- */
  lmlaP->leafsN = streeP->svocabP->list.itemN; 
  lmlaP->nodesN = LTREE_NULL_NODE+1;

  if (NULL == (lmlaP->typeA = (int*) calloc (lmlaP->leafsN*2,sizeof(int)))) {
    ERROR("ltreeInit: allocation failure\n");
    return TCL_ERROR;
  }  
  if (NULL == (lmlaP->argsA   = (int*) calloc (lmlaP->leafsN*2,sizeof(int))))  {
    ERROR("ltreeInit: allocation failure\n");
    return TCL_ERROR;
  }
  if (NULL == (lmlaP->lmrcm.w2p = malloc(lmlaP->leafsN*sizeof(CTX)))) {
    ERROR("ltreeInit: allocation failure\n");
    return TCL_ERROR;
  }
  memset (lmlaP->lmrcm.w2p,  streeP->svocabP->dictP->phones->list.itemN, lmlaP->leafsN);
  if (NULL == (_stackA = (int*) calloc (lmlaP->leafsN + 2,sizeof(int)))) {
    ERROR("ltreeInit: allocation failure\n");
    return TCL_ERROR;
  }

  _stackP       = _stackA;
  lmlaP->argsP  = lmlaP->argsA;

  lmlaP->typeA[LTREE_NULL_NODE] = 0;
  lmlaP->typeP  = lmlaP->typeA+1;

  /* --------------------------------
       Loop to initialize the trees
     -------------------------------- */
  for (i = 0; i <  lmlaP->streeP->rootTbl.N; i++) {
    SRoot* srootP = streeP->rootTbl.PA[i];
    int*        t =  lmlaP->typeP;

    compressSTree_SRoot (srootP, lmlaP);
  
    lmlaP->nodesN += (lmlaP->typeP  - t);

  }
  if (_stackA) free(_stackA);
  _stackA = NULL;
  _stackP = NULL;

  if (!silent) INFO ("ltreeInit: created %d leafs, %d nodes, %d args\n",
		     lmlaP->leafsN, lmlaP->nodesN, lmlaP->argsP-lmlaP->argsA);

  listInit ((List*) &(lmlaP->hlctList), NULL, sizeof (HashLCT), 20000);
  lmlaP->hlctList.init    = (ListItemInit *)hlctInit;
  lmlaP->hlctList.deinit  = NULL;
  lmlaP->hlctList.hashKey = (HashKeyFn    *)hlctHashKey;
  lmlaP->hlctList.hashCmp = (HashCmpFn    *)hlctHashCmp;
  lmlaP->hlctList.itemMax = 100000;

  /* hash table for fast access of constrained lct's  */
  lmlaP->svxHash = 1;
  listInit ((List*) &(lmlaP->hsvxList), NULL, sizeof (HashSVX), 20000);
  lmlaP->hsvxList.init    = (ListItemInit *)hsvxInit;
  lmlaP->hsvxList.deinit  = NULL;
  lmlaP->hsvxList.hashKey = (HashKeyFn    *)hsvxHashKey;
  lmlaP->hsvxList.hashCmp = (HashCmpFn    *)hsvxHashCmp;
  lmlaP->hsvxList.itemMax = 100000;

  /* Cache for Nodes and Leafs */
  lmlaConfigureCache (lmlaP, cacheN, ncacheN);

  /* Cache for (context independent) Roots */
  lmlaP->lmrcm.phoneN = streeP->svocabP->dictP->phones->list.itemN;
  lmlaP->lmrcm.lctA   = NULL;
  lmlaP->lmrcm.scoreA = NULL;
  lmlaP->lmrcm.useA   = NULL;
  lmlaP->lmrcm.cacheN = 0; 
  lmlaConfigurePhoneCache (lmlaP, pcacheN);

  /* The cache is clean, so tell svmap to be happy */
  svmapIsDirty (lmlaP->svmapP);
  svmapIsDirty (lmlaP->streeP->svmapP);

  /* Lattice constraint */
  if (listInit ((List*) &(lmlaP->latlm.list), NULL, 
	       sizeof (LatLMNode), 10000) != TCL_OK) {
    ERROR ("Couldn't allocate LatLMNode list.");
    return TCL_ERROR;
  }
  lmlaP->latlm.list.deinit  = NULL;
  lmlaP->latlm.list.init    = (ListItemInit  *) LatLMHashInit;
  lmlaP->latlm.list.hashKey = (HashKeyFn     *) LatLMHashKey;
  lmlaP->latlm.list.hashCmp = (HashCmpFn     *) LatLMHashCmp;
  lmlaP->latlm.mode         = Exact_Constraint;
  lmlaP->latlm.what         = 0;
  lmlaP->latlm.latP         = NULL;

  lmlaP->latlm.buffer       = bmemCreate (65536, 0);
  lmlaP->latlm.tmpA         = (GNode**) malloc (1024*sizeof (GNode*));

  lmlaP->latlm.padX         = 0; 
  lmlaP->latlm.frameX       = 0; 
  lmlaP->latlm.validA       = NULL; 

  lmlaClearConstraint(lmlaP);

  return TCL_OK;
}

/* ------------------------------------------------------------------------
    lmlaCreate
   ------------------------------------------------------------------------ */

LMLA* lmlaCreate (char* name, STree* streeP, SVMap* svmapP, int reduced, 
		  int depth)
{
  LMLA* lmlaP = (LMLA*) malloc(sizeof(LMLA));

  if (!lmlaP) {
    ERROR("lmlaCreate: allocation failure.\n");
    return NULL;
  }
  if (lmlaInit (lmlaP, name, streeP, svmapP, reduced, depth, 0) == TCL_ERROR) {    
    free (lmlaP);
    return NULL;
  }
  return lmlaP;
}

static ClientData lmlaCreateItf( ClientData clientData, int argc, char *argv[])
{
  int     ac    = argc;
  STree* streeP = NULL;
  SVMap* svmapP = NULL;
  int     depth = 5;
  int   reduced = 0;
  char* name = NULL;

  if ( itfParseArgv( argv[0], &ac, argv, 0,
      "<name>",  ARGV_STRING, NULL, &name, NULL,         "name of the LTree",
      "<SearchTree>", ARGV_OBJECT, NULL, &streeP,  "STree", "Search tree",
      "-map",         ARGV_OBJECT, NULL, &svmapP,  "SVMap", "Vocabulary mapper to use for LookAhead only",
      "-depth",       ARGV_INT,    NULL, &depth,   NULL,    "Maximum depth of LookAhead tree",
      "-reduced",     ARGV_INT,    NULL, &reduced, NULL,    "Set 'reduce' flag for LookAhead nodes",
       NULL) != TCL_OK) return NULL;


  /* Guarantee a correct vocab mapper for LookAhead */
  if (!svmapP)
    svmapP = streeP->svmapP;
  else if (svmapP->svocabP != streeP->svmapP->svocabP) {
    ERROR ("You want to build %s and %s over the same vocabulary.\n",
	   svmapP->name, streeP->svmapP->name);
    return NULL;
  }

  return (ClientData) lmlaCreate (name, streeP, svmapP, reduced, depth);
}

/* ------------------------------------------------------------------------
    lmlaDeinit
   ------------------------------------------------------------------------ */

static int lmlaDeinit (LMLA* lmlaP, int silent)
{
  if (lmlaP->useN && !silent) {
    ERROR ("lmla '%s' still in use by other objects.\n", lmlaP->name); 
    return TCL_ERROR;
  }

  /* --------------------------
       GLat Constraint Filter
     -------------------------- */
  lmlaClearConstraint (lmlaP);
  listClear ((List*) &lmlaP->latlm.list);
  if (lmlaP->latlm.tmpA)   { free (lmlaP->latlm.tmpA);   lmlaP->latlm.tmpA   = NULL; }
  if (lmlaP->latlm.validA) { free (lmlaP->latlm.validA); lmlaP->latlm.validA = NULL; }


  /* ---------
       Cache
     --------- */
  lmlaConfigureCache      (lmlaP, 0, 0);
  lmlaConfigurePhoneCache (lmlaP, 0);

  if (lmlaP->lmrcm.w2p) free(lmlaP->lmrcm.w2p);
  lmlaP->lmrcm.w2p = NULL;

  if (lmlaP->argsA)   free (lmlaP->argsA);
  if (lmlaP->typeA)   free (lmlaP->typeA);
  
  lmlaP->argsA   = NULL;
  lmlaP->typeA   = NULL;

  listDeinit ((List*) &(lmlaP->hlctList));
  listDeinit ((List*) &(lmlaP->hsvxList));

  if (lmlaP->name) free(lmlaP->name);
  lmlaP->name    = NULL;

  delink (lmlaP->streeP, "STree");
  lmlaP->streeP  = NULL;

  delink (lmlaP->svmapP, "SVMap");
  lmlaP->svmapP  = NULL;

  return TCL_OK;
}


/* ------------------------------------------------------------------------
    lmlaFree
   ------------------------------------------------------------------------ */

static int lmlaFree(LMLA* lmlaP)
{
  if (lmlaDeinit (lmlaP, 0) != TCL_OK) return TCL_ERROR;
  free (lmlaP);
  return TCL_OK;
}

static int lmlaFreeItf (ClientData clientData)
{
  return lmlaFree((LMLA*)clientData);
}

/* ------------------------------------------------------------------------
    lmlaReinit
   ------------
    Rebuilds the LM lookahead tree
   ------------------------------------------------------------------------ */

extern int lmlaReinit(LMLA* lmlaP)
{
  char*   name     = strdup(lmlaP->name);
  STree*  streeP   = lmlaP->streeP;
  SVMap*  svmapP   = lmlaP->svmapP;
  GLat*   glatP    = lmlaP->latlm.latP;
  int     depth    = lmlaP->depth;
  int     reduced  = lmlaP->reduced;
  int     cache    = lmlaP->cache.N;
  int     ncache   = lmlaP->nodeCache.N;
  int     pcache   = lmlaP->lmrcm.cacheN;
  int     useN     = lmlaP->useN;

  /* deinitialize ltree */
  lmlaP->useN       = 0;
  lmlaP->latlm.latP = NULL;
  if (lmlaDeinit (lmlaP, 1) != TCL_OK) return TCL_ERROR;

  /* rebuild new ltree */
  if (lmlaInit (lmlaP, name, streeP, svmapP, reduced, depth, 1)  == TCL_ERROR) {
    free(lmlaP);
    return TCL_ERROR;
  }
  if (name) free(name);
  lmlaP->useN = useN;
  
  /* reinitialize lattice constraint */
  lmlaP->latlm.latP   = glatP;
 
  /* reinitialize cache trees */
  lmlaConfigureCache      (lmlaP, cache, ncache);
  lmlaConfigurePhoneCache (lmlaP, pcache);

  return TCL_OK;
}

/* ------------------------------------------------------------------------
    lmlaPuts
   ------------------------------------------------------------------------ */

static int lmlaPutsItf (ClientData clientData, int argc, char *argv[])
{
  LMLA* lmlaP = (LMLA*)clientData;

  itfAppendResult("%d", lmlaP->streeP->rootTbl.N);

  return TCL_OK;    
}


/* ------------------------------------------------------------------------
    lmlaConfigureItf
   ------------------------------------------------------------------------ */

static int lmlaConfigureItf (ClientData cd, char *var, char *val)
{
  LMLA *lmlaP = (LMLA*) cd;
  int cacheN  = lmlaP->cache.N ;
  int ncacheN = lmlaP->nodeCache.N;
  int pcacheN = lmlaP->lmrcm.cacheN;

  if (!lmlaP) return TCL_ERROR;

  if (!var) {
    ITFCFG(lmlaConfigureItf,cd,"name");
    ITFCFG(lmlaConfigureItf,cd,"useN");
    ITFCFG(lmlaConfigureItf,cd,"expert");
    ITFCFG(lmlaConfigureItf,cd,"depth");
    ITFCFG(lmlaConfigureItf,cd,"mode");
    ITFCFG(lmlaConfigureItf,cd,"lm(nodes)");
    ITFCFG(lmlaConfigureItf,cd,"lm(leafs)");
    ITFCFG(lmlaConfigureItf,cd,"map(nodes)");
    ITFCFG(lmlaConfigureItf,cd,"map(leafs)");
    ITFCFG(lmlaConfigureItf,cd,"reduced");
    ITFCFG(lmlaConfigureItf,cd,"cacheN");
    ITFCFG(lmlaConfigureItf,cd,"ncacheN");
    ITFCFG(lmlaConfigureItf,cd,"pcacheN");
    ITFCFG(lmlaConfigureItf,cd,"svxHash");
    ITFCFG(lmlaConfigureItf,cd,"svxMax");
    ITFCFG(lmlaConfigureItf,cd,"lctMax");
    ITFCFG(lmlaConfigureItf,cd,"queries");
    ITFCFG(lmlaConfigureItf,cd,"cachehits");
    ITFCFG(lmlaConfigureItf,cd,"nodecachehits");

    return TCL_OK;
  }

  /* Only read-only cases can safely use this function */
  ITFCFG_Int (var, val, "useN",          lmlaP->useN,  1);
  ITFCFG_Int (var, val, "depth",         lmlaP->depth, 1);
  ITFCFG_Int (var, val, "expert",        lmlaP->expert, 0);
  ITFCFG_Int (var, val, "svxHash",       lmlaP->svxHash, 0);
  ITFCFG_Int (var, val, "svxMax",        lmlaP->hsvxList.itemMax, 0);
  ITFCFG_Int (var, val, "lctMax",        lmlaP->hlctList.itemMax, 0);
  ITFCFG_Int (var, val, "queries",       lmlaP->cache.queries, 1);
  ITFCFG_Int (var, val, "cachehits",     lmlaP->cache.hits, 1);
  ITFCFG_Int (var, val, "nodecachehits", lmlaP->nodeCache.hits, 1);
  ITFCFG_Int (var, val, "latlmN",        lmlaP->latlm.list.itemN, 1);

  ITFCFG_CharPtr(var,val,"name", lmlaP->name,  1);
  if (streq        (var, "cacheN")) {
    cacheN  = (val) ? atoi(val) : lmlaP->cache.N;
    itfAppendResult ("%d ", cacheN); 
  } else if (streq (var, "ncacheN")) {
    ncacheN  = (val) ? atoi(val) : lmlaP->nodeCache.N;
    itfAppendResult ("%d ", ncacheN); 
  } else if (streq (var,"pcacheN")) {
    pcacheN = (val) ? atoi(val) : lmlaP->lmrcm.cacheN;
    itfAppendResult("%d ", pcacheN);
  } else if (streq (var, "lm(leafs)")) {
    itfAppendResult ("%s ", lmlaP->streeP->svmapP->lksP->name);
  } else if (streq (var, "lm(nodes)")) {
    if (lmlaP->svmapP)
      itfAppendResult ("%s ", lmlaP->svmapP->lksP->name);
    else
      itfAppendResult ("%s ", lmlaP->streeP->svmapP->lksP->name);
  } else if (streq (var, "map(leafs)")) {
    itfAppendResult ("%s ", lmlaP->streeP->svmapP->name);
  } else if (streq (var, "map(nodes)")) {
    if (lmlaP->svmapP)
      itfAppendResult ("%s ", lmlaP->svmapP->name);
    else
      itfAppendResult ("%s ", lmlaP->streeP->svmapP->name);
  } else if (streq (var, "lm")) {
    itfAppendResult ("%s ", lmlaP->streeP->svmapP->lksP->name);
  } else if (streq (var, "reduced")) {
    if (val) {
      lmlaP->reduced = !(0 == atoi (val));
      lmlaClearCache(lmlaP);
    }
    itfAppendResult ("%d ", lmlaP->reduced);
  } else if (streq (var, "mode")) {
    if (val) {
      if (streq (val, "single")) lmlaP->mode = Single_Mode;
      else                       lmlaP->mode = Array_Mode;
    } else {
      if (lmlaP->mode == Single_Mode) itfAppendResult ("single");
      else                            itfAppendResult ("array");
    }  
  }

  /* re-init the cache */
  if (cacheN != lmlaP->cache.N || ncacheN != lmlaP->nodeCache.N)
    lmlaConfigureCache      (lmlaP, cacheN, ncacheN);
  if (pcacheN != lmlaP->lmrcm.cacheN)
    lmlaConfigurePhoneCache (lmlaP, pcacheN);

  return TCL_OK;
}


/* ------------------------------------------------------------------------
    lmlaAccessItf
   ------------------------------------------------------------------------ */

static ClientData lmlaAccessItf (ClientData cd, char *name, TypeInfo **ti)
{
  LMLA *lmlaP = (LMLA*)cd;
  int i;

  if (!lmlaP) return NULL;

  if (*name == '.') { 
    if (name[1]=='\0') {
      if (lmlaP->expert) {
	itfAppendElement ("leafScore(0..%d)",  lmlaP->leafsN-1);
	itfAppendElement ("nodeScore(0..%d)",  lmlaP->nodesN-1);      
	/* itfAppendElement ("phoneScore(0..%d)", lmlaP->phonesN-1); */  
	itfAppendElement ("nodeType(0..%d)",   lmlaP->nodesN-1);      
	itfAppendElement ("nodeArg(0..%d)",    lmlaP->nodesN+lmlaP->leafsN-1);
	itfAppendElement ("test(1..)");
	itfAppendElement ("cache");
	itfAppendElement ("nodeCache");
      }
      itfAppendElement ("queries");
      itfAppendElement ("cachehits");
      itfAppendElement ("nodecachehits");
      itfAppendElement ("latlmN");
      itfAppendElement ("latlmM");
      *ti = NULL;
    } else {
      if (sscanf(name+1,"leafScore(%d)",  &i) == 1 && i >=0 && i < lmlaP->leafsN) {
	itfAppendElement("%f", UnCompressScore (lmlaP->cache.scorePA[lmlaP->cache.lastX][i]));
	*ti = NULL;
      } else if (sscanf(name+1,"nodeScore(%d)",  &i) == 1 && i >=0 && i < lmlaP->nodesN) {
	itfAppendElement("%f", UnCompressScore (lmlaP->cache.scorePA[lmlaP->cache.lastX][i+lmlaP->leafsN]));
	*ti = NULL;
      } else if (sscanf(name+1,"nodeType(%d)",   &i) == 1 && i >=0 && i < lmlaP->nodesN) {
	itfAppendElement("%d", lmlaP->typeA[i]);
	*ti = NULL;
      } else if (sscanf(name+1,"nodeArg(%d)",    &i) == 1 && i >=0 && i < lmlaP->leafsN+lmlaP->nodesN) {
	itfAppendElement("%d", lmlaP->argsA[i]);
	*ti = NULL;
      } else if (!strcmp(name+1,"cache")) {
	LVX* w = (LVX*) malloc (4*sizeof(LVX));
	for (i = 0; i < lmlaP->cache.N && lmlaP->cache.lctA[i] != LCT_NIL; i++) {
	  DecodeLCT (lmlaP, lmlaP->cache.lctA[i], w, -1);
	  itfAppendElement("%d %d %d (%d %d)", lmlaP->cache.useA[i], 
			   (int) lmlaP->cache.lctA[i], (int) lmlaP->cache.nctA[i],
			   (int) w[0], (int) w[1]);
	}
	free (w);
	*ti = NULL;
      } else if (!strcmp(name+1,"nodeCache")) {
	LVX* w = (LVX*) malloc (4*sizeof(LVX));
	for (i = 0; i < lmlaP->nodeCache.N && lmlaP->nodeCache.nctA[i] != LCT_NIL; i++) {
	  DecodeLCT (lmlaP, lmlaP->nodeCache.nctA[i], w, -1);
	  itfAppendElement("%d %d (%d %d)", lmlaP->nodeCache.useA[i], 
			   (int) lmlaP->nodeCache.nctA[i],
			   (int) w[0], (int) w[1]);
	}
	free (w);
	*ti = NULL;
      } else if (streq (name+1, "latlmM")) {
	*ti = itfGetType ("BMem");
        return (ClientData) lmlaP->latlm.buffer;
      } else if (sscanf(name+1,"test(%d)", &i) == 1 && i > 0) {
	clock_t a;
	SVX sw1, sw2;
	int j;
	lmScore *svX, *lnX;
	LCT lct;

	a = clock();
	for (j = 0; j < i; j++) {
	  sw1 = chaosRandom(0.0, 1.0)*(lmlaP->streeP->svocabP->list.itemN-1);
	  sw2 = chaosRandom(0.0, 1.0)*(lmlaP->streeP->svocabP->list.itemN-1);
	  lct = CreateLCT (lmlaP, sw1);
	  lmlaFillCtx (lmlaP, ExtendLCT (lmlaP, lct, sw2), &lnX, &svX);
	}
	itfAppendElement("%f queries per second", (double)i/(double)(clock()-a)*(double)CLOCKS_PER_SEC);
	*ti = NULL;
      }
    }
  }

  return NULL;
}

/* ------------------------------------------------------------------------
    LTreeFillCtx
   ------------------------------------------------------------------------ */

static int lmlaFillCtxItf (ClientData cd, int argc, char *argv[])
{
  int      ac = argc - 1;
  LMLA    *lmlaP = (LMLA*) cd;
  char    *w1 = NULL, *w2 = NULL;
  LVX      lvx1, lvx2;
  SVX      svx1, svx2;
  lmScore *svX, *lnX;
  LCT      lct;
  LingKS* lksP = lmlaP->streeP->svmapP->lksP;

  if ( itfParseArgv( argv[0], &ac, argv+1, 0,
       "w1", ARGV_STRING, NULL, &w1, NULL, "w1 context",
       "w2", ARGV_STRING, NULL, &w2, NULL, "w2 context",
       NULL) != TCL_OK) return TCL_ERROR;
  
  /* Heute (2001-01-30) weiss ich noch, was das bedeutet (Florian):
     Ich suche den Such-Wort-Index fuer meine Worte w1 und w2 */
  svx1 = listIndex((List*)&lmlaP->streeP->svocabP->list, w1, 0);
  svx2 = listIndex((List*)&lmlaP->streeP->svocabP->list, w2, 0);

  if (svx1 == SVX_NIL || svx2 == SVX_NIL) {
    ERROR("ltreeFillCtx: svX(%s)=%d, svX(%s)=%d: not in vocab.\n", w1, svx1, w2, svx2);
    return TCL_ERROR;
  }

  lvx1 = svmapGet (lmlaP->streeP->svmapP, svx1);
  lvx2 = svmapGet (lmlaP->streeP->svmapP, svx2);

  if (lvx1 == LVX_NIL || lvx2 == LVX_NIL) {
    ERROR("ltreeFillCtx: lvX(%s)=%d, lvX(%s)=%d: not in LM-vocab.\n", w1, lvx1, w2, lvx2);
    return TCL_ERROR;
  }

  lct = CreateLCT (lmlaP,      svx1);
  lct = ExtendLCT (lmlaP, lct, svx2);
  printf ("FillCtx: %s(%d) -> %s(%d), %s(%d) -> %s(%d), lct=%d\n", 
	  w1, svx1, lksP->itemA (lksP, lvx1), lvx1, w2, svx2, lksP->itemA (lksP, lvx2), lvx2, (int) lct);

  return lmlaFillCtx (lmlaP, lct, &lnX, &svX);
}


/* ========================================================================
     Type Information
   ======================================================================== */

Method lmlaMethod[] = {
  { "puts",     lmlaPutsItf,            "displays the contents of a LTree"                    },
  { "constrain",lmlaCreateConstraintItf,"create GLat constraint for LTree"                    },
  { "fillCtx",  lmlaFillCtxItf,         "fills a LTree object with scores for a specific lct" },  
  { NULL, NULL, NULL }
} ;

TypeInfo lmlaInfo = { 
  "LTree", 0, -1, lmlaMethod, 
  lmlaCreateItf, lmlaFreeItf, 
  lmlaConfigureItf, lmlaAccessItf, NULL,
  "Language-Model Look-Ahead object (Lexical tree)\n"
};

extern int LMLA_Init (void)
{
  if (! lmlaInitialized) {

    itfNewType(&lmlaInfo);
    
    lmlaInitialized = TRUE;
  }
  return TCL_OK;
}
