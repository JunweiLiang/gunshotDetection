/* ========================================================================
    JANUS      Speech- to Speech Translation
               ------------------------------------------------------------
               Object: SPASS lattice
               ------------------------------------------------------------

    Author  :  Hagen Soltau & Florian Metze
    Module  :  glat.c
    Date    :  $Id: glat.c 3428 2011-04-15 18:00:28Z metze $
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
    Revision 4.18  2007/03/19 14:31:08  fuegen
    some more MMIE related changes/ optimizations from Roger

    Revision 4.17  2007/03/19 10:41:39  fuegen
    changes from Wilson for writing SLF lattices
    - convert all scores to log base 10
    - added flag to give utterance start time offset

    Revision 4.16  2007/02/21 17:03:36  fuegen
    code cleaning for windows from Roger and Christian

    Revision 4.15  2007/01/09 11:07:25  fuegen
    - bugfixes for alpha/ beta computation which influences the gamma computation
      and also the connect
      To be still compatible with old scripts a factor of 4.5 is applied to the
      alpha lattice beam during connect and prune (-factor).
      For confusion network combination in comparison with old results, slightly
      better results could be achieved, when reducing the postScale from 2.0 to 1.8.
    - removed the MPE code

    Revision 4.14  2006/11/10 10:15:58  fuegen
    merged changes from branch jtk-05-02-02-shajith
    - all modification related to MMIE training
    - first unverified functions for MPE training
    - modifications made by Shajith, Roger, Wilson, and Sebastian

    Revision 4.13  2005/11/08 13:11:42  metze
    Fixes for connect and alpha/beta computation

    Revision 4.12  2005/04/07 08:23:18  metze
    Re-use existing nodes in glatAddNode with -beam

    Revision 4.11.8.1  2006/11/03 12:29:31  stueker
    Initial check-in of MMIE from Shajith, Wilson, and Roger.

    Revision 4.11  2004/09/23 14:46:06  metze
    Fixes for x86_64

    Revision 4.10  2004/08/23 11:32:56  metze
    Changed output in confidence -v 1

    Revision 4.9  2004/08/11 10:21:03  fuegen
    changed type of idx in glatWriteNode to SVX

    Revision 4.8  2004/08/10 10:04:53  fuegen
    removed bug, that when writing 'self made' lattices produced by addPath
    with no time information every word is replaced by ')'

    Revision 4.7  2004/05/10 14:23:37  metze
    Sebastians's fix

    Revision 4.6  2004/05/10 14:19:38  metze
    Discuss 'isf' business

    Revision 4.5  2004/03/05 14:17:50  fuegen
    removed bug with empty rescoring results
    under certain conditions

    Revision 4.4  2003/12/17 16:07:07  fuegen
    glatRescore: do not extend non-existing words also if nodeP != latP->endP

    Revision 4.3  2003/11/06 17:45:46  metze
    CLeaned up vocab mapping

    Revision 4.2  2003/09/05 15:35:19  soltau
    gnodeComputeLCT: check for lvX = LVX_NIL

    Revision 4.1  2003/08/14 11:20:01  fuegen
    Merged changes on branch jtk-01-01-15-fms (jaguar -> ibis-013)

    Revision 1.1.2.125  2003/08/13 08:57:13  fuegen
    changed initialization for latP->[startP/endP]->svX to SVX_NIL
    to work with CFGs

    Revision 1.1.2.124  2003/06/06 08:04:41  fuegen
    changed allocation behaviour for Windows in Read functions
    made gcc 3.3 clean

    Revision 1.1.2.123  2003/04/09 15:11:11  soltau
    glatConfidence : more node tying options

    Revision 1.1.2.122  2003/04/08 10:01:53  metze
    Cleaner splitMW code

    Revision 1.1.2.121  2003/04/05 15:15:17  metze
    Fixed splitMW for pron-probs

    Revision 1.1.2.120  2003/04/01 08:53:11  metze
    Consensus changes

    Revision 1.1.2.119  2003/03/20 15:22:50  metze
    Implemented time-axis warping of lattices

    Revision 1.1.2.118  2003/03/19 14:24:36  metze
    Faster consensus for multiple lattices using same frame width

    Revision 1.1.2.117  2003/03/18 18:11:23  soltau
    Support for time constrained rescoring

    Revision 1.1.2.116  2003/03/14 16:24:53  metze
    Memory bug in lattice rescoring fixed (hope so, only shows up on Solaris)

    Revision 1.1.2.115  2003/03/11 16:37:22  metze
    Cacheing for MetaLM, Map for GLat computeLCT

    Revision 1.1.2.114  2003/03/09 11:31:21  soltau
    gnodeComputeAlpha : re-introduced (!) end_of_sentence check

    Revision 1.1.2.113  2003/03/08 17:56:49  soltau
    Revert to previuos version

    Revision 1.1.2.111  2003/02/28 16:17:33  metze
    A posteriori rescoring of lattices

    Revision 1.1.2.110  2003/02/19 14:59:33  metze
    Fixed pruning (prob_

    Revision 1.1.2.109  2003/02/18 18:18:14  metze
    Final (?) version of Consensus

    Revision 1.1.2.108  2003/02/12 13:50:54  metze
    Fixed singularLCT

    Revision 1.1.2.107  2003/01/28 16:20:34  metze
    Consensus with BitField code

    Revision 1.1.2.106  2003/01/10 10:40:10  metze
    Removed C++ style comments

    Revision 1.1.2.105  2003/01/07 14:59:36  metze
    Consensus changes, added frameShift

    Revision 1.1.2.104  2002/12/13 10:51:45  metze
    Consensus similar to Lidia's default case

    Revision 1.1.2.103  2002/12/12 15:29:18  metze
    In simpler cases we can now re-create Lidia's behaviour

    Revision 1.1.2.102  2002/12/01 13:50:56  metze
    Added glatConsensus

    Revision 1.1.2.101  2002/11/21 17:10:09  fuegen
    windows code cleaning

    Revision 1.1.2.100  2002/11/13 10:14:32  soltau
    GLat base now on SVMap directly

    Revision 1.1.2.99  2002/11/05 08:46:13  metze
    Set default values for rescoring to sensible values

    Revision 1.1.2.98  2002/11/04 14:18:06  metze
    Info for lattice type

    Revision 1.1.2.97  2002/09/04 13:03:12  soltau
    glatCompress : fixed glatCount problems

    Revision 1.1.2.96  2002/08/27 08:47:26  metze
    itfParseArgv now uses <name> and NULL initialization for char*

    Revision 1.1.2.95  2002/07/18 12:06:09  soltau
    Changed return value in glatConfidence if lattice check fails

    Revision 1.1.2.94  2002/06/25 07:29:13  metze
    Fixes and catches for glatRescore

    Revision 1.1.2.93  2002/06/18 16:53:43  metze
    Fixed inconsistencies in mapping.x2n handling
    Replaced "nodeN" by "nodeM and checks" in loops where neccessary
    Cleaned up lattice status and added it to lattice files

    Revision 1.1.2.92  2002/06/13 08:21:10  soltau
    call svmapClearCache after createLCT calls

    Revision 1.1.2.91  2002/06/12 14:37:41  soltau
    glatAddPath   : fixed missing links, checking begin/end of sentence
    glatAddLink   : cleaned
    glatConfigure : removed aligment costs

    Revision 1.1.2.90  2002/06/10 10:06:09  soltau
    Remove parse error

    Revision 1.1.2.89  2002/06/10 09:54:10  soltau
    glatComputeGamma : changed arguments
    glatPuts         : added option to write probs instead of log-probs

    Revision 1.1.2.88  2002/05/28 11:21:17  metze
    Hashed LMs work in lattices, too.

    Revision 1.1.2.87  2002/05/03 14:56:09  soltau
    glatRescore : added beam pruning

    Revision 1.1.2.86  2002/05/02 14:37:27  metze
    More checks in glatRescore

    Revision 1.1.2.85  2002/05/02 12:04:21  soltau
    glatReadItf: not try to set finalframe anymore

    Revision 1.1.2.84  2002/04/29 13:30:05  metze
    Redesign of LM interface (LingKS)

    Revision 1.1.2.83  2002/04/25 17:27:33  soltau
    glatRescore    : fixed some recombination issues
    glatAdd/Delete : cleaned code

    Revision 1.1.2.82  2002/04/11 09:39:45  soltau
    Removed defines BMEM_FREE, use it always

    Revision 1.1.2.81  2002/04/11 08:46:48  soltau
    glatComputeConfidenceItf : closed memory leak

    Revision 1.1.2.80  2002/04/08 11:54:19  metze
    Changes in 'recombine'

    Revision 1.1.2.79  2002/03/15 16:47:09  soltau
    *** empty log message ***

    Revision 1.1.2.78  2002/03/14 09:09:00  metze
    Made 'recombine' much faster by using glatSortNodes

    Revision 1.1.2.77  2002/03/13 16:02:42  metze
    Added recombine and splitMW methods

    Revision 1.1.2.76  2002/03/07 10:22:33  metze
    Introduced -singularLCT (purify still has problems with this)

    Revision 1.1.2.75  2002/02/28 16:55:50  soltau
    cleaned glatDeinit

    Revision 1.1.2.74  2002/02/27 09:24:49  metze
    First effort towards integrating consensus scoring

    Revision 1.1.2.73  2002/02/24 09:30:55  metze
    Rewrite of Lattice LM: now works with Hash Fct
    Extra output options for glatAlign

    Revision 1.1.2.72  2002/02/18 15:45:10  soltau
    glatPrune: call glatPurify even if there are too less nodes to prune

    Revision 1.1.2.71  2002/02/15 17:12:41  soltau
    call svmapClearCache

    Revision 1.1.2.70  2002/02/13 13:46:48  soltau
    Use svmapGetLMscore instead of calling lksP->scoreFct directly

    Revision 1.1.2.69  2002/02/11 13:51:13  soltau
    optimized rcm memory management

    Revision 1.1.2.68  2002/02/07 12:10:57  metze
    Changes in interface to LM score functions

    Revision 1.1.2.67  2002/02/04 14:27:41  metze
    Better handling of pronunciation variants in writeHTK

    Revision 1.1.2.66  2002/01/30 09:27:22  metze
    Writing of HTK lattices now conforms to what Lidia Mangu's
    Consensus-Code expects (needs svmap)

    Revision 1.1.2.65  2002/01/23 11:39:54  soltau
    gnodeComputeLCT: fixed lct problems

    Revision 1.1.2.64  2002/01/10 16:56:31  soltau
    Changed naming convention : compact  -> compress

    Revision 1.1.2.63  2001/12/21 16:31:00  soltau
    glatComputeGamma: Added sum/max mode

    Revision 1.1.2.62  2001/12/17 09:44:52  soltau
    Repaired handling of filler words in forward/backward pass

    Revision 1.1.2.61  2001/12/08 12:33:39  soltau
    Added ignoreFTag option in glatAlign

    Revision 1.1.2.60  2001/12/07 16:07:46  soltau
    made summing over similar nodes optinonally

    Revision 1.1.2.59  2001/12/06 16:27:54  soltau
    Beautified gnodeComputeLCT

    Revision 1.1.2.58  2001/12/06 16:07:16  soltau
    glatRescore : verbose output

    Revision 1.1.2.57  2001/12/06 14:20:14  soltau
    Changed data types for some internal variable in forward-backward pass
    Fixed sum over 'similiar' nodes in confidence measure

    Revision 1.1.2.56  2001/10/22 08:39:24  metze
    Changed interface to score functions

    Revision 1.1.2.55  2001/10/17 14:17:16  soltau
    fixed array bounds read in glatAlign

    Revision 1.1.2.54  2001/10/11 16:56:38  soltau
    Added 'Flat_Constraint' Mode

    Revision 1.1.2.53  2001/10/11 15:34:18  soltau
    Improved memory efficieny in glatRescore

    Revision 1.1.2.52  2001/10/11 14:30:00  soltau
    *** empty log message ***

    Revision 1.1.2.51  2001/10/11 11:29:27  soltau
    *** empty log message ***

    Revision 1.1.2.50  2001/10/10 19:35:10  soltau
    Added Access functions

    Revision 1.1.2.49  2001/10/10 18:40:20  soltau
    Added confidence measure

    Revision 1.1.2.48  2001/10/09 18:33:00  soltau
    Added hash table for linguistic constraint
    Made Lattice Alignment more efficient

    Revision 1.1.2.47  2001/10/04 17:36:26  soltau
    Made delFil = 1 as default

    Revision 1.1.2.46  2001/10/03 18:17:35  soltau
    Added adjustTime option in glatCompact

    Revision 1.1.2.45  2001/10/03 17:25:20  soltau
    *** empty log message ***

    Revision 1.1.2.44  2001/10/03 15:07:39  soltau
    *** empty log message ***

    Revision 1.1.2.43  2001/10/03 14:50:22  soltau
    Closed memleak in ReadNJD
    Improved (hmm, at least modified) glatCompact

    Revision 1.1.2.42  2001/10/02 17:08:22  soltau
    Added option ignoreLCT in glatCompact

    Revision 1.1.2.41  2001/10/02 15:23:02  soltau
    Added glatCompact

    Revision 1.1.2.40  2001/09/26 17:24:14  soltau
    Closed some memleaks in glatReadJANUS

    Revision 1.1.2.39  2001/09/26 16:13:25  soltau
    Call purify after reading old janus format

    Revision 1.1.2.38  2001/09/26 10:09:59  soltau
    *** empty log message ***

    Revision 1.1.2.37  2001/09/25 17:51:27  metze
    Frame Alignment for addPath

    Revision 1.1.2.36  2001/09/24 19:29:28  soltau
    Added link to LatLmNode

    Revision 1.1.2.35  2001/09/21 14:36:28  soltau
    Fixed segfault in glatRescore if maxN is exceeded

    Revision 1.1.2.34  2001/09/20 20:22:34  soltau
    *** empty log message ***

    Revision 1.1.2.33  2001/09/19 17:59:34  soltau
    Redesigned a-posteriori computation

    Revision 1.1.2.32  2001/09/18 18:42:17  soltau
    Separated lattice generation

    Revision 1.1.2.31  2001/09/18 10:47:33  soltau
    Support for filler words in glatRepair
    Check starting point of time in gnodeCreateLink

    Revision 1.1.2.30  2001/07/26 17:40:16  soltau
    Removed mysterious 109

    Revision 1.1.2.29  2001/07/26 09:30:49  metze
    Better memory management for glatRescore

    Revision 1.1.2.28  2001/07/25 17:27:48  soltau
    Fixed problems in glatSortNodes

    Revision 1.1.2.27  2001/07/25 09:50:24  metze
    Re-wrote code for rescoring of lattices

    Revision 1.1.2.26  2001/07/25 11:36:10  soltau
    Added expert mode
    Fixed glatRepair

    Revision 1.1.2.25  2001/07/24 17:13:59  soltau
    Added glatRepair

    Revision 1.1.2.24  2001/07/18 17:08:18  soltau
    Fixed problems in glatRescore

    Revision 1.1.2.23  2001/07/11 15:09:53  metze
    Added language model rescoring

    Revision 1.1.2.22  2001/06/28 11:53:41  metze
    Added utterance name to lattice file format

    Revision 1.1.2.21  2001/06/25 20:58:35  soltau
    Changed bmem interface

    Revision 1.1.2.20  2001/06/22 14:48:32  soltau
    Added memory management

    Revision 1.1.2.19  2001/06/20 18:27:07  soltau
    Improved alignment

    Revision 1.1.2.18  2001/06/20 16:05:59  metze
    Added manual adding and removing of nodes and links

    Revision 1.1.2.17  2001/06/19 16:25:12  metze
    Reading and writing of NJD lattices, reading of JANUS lattices works again

    Revision 1.1.2.16  2001/06/18 09:08:44  metze
    Merged Hagen's and Florian's versions

    Revision 1.1.2.15  2001/06/18 09:56:50  soltau
    fixed alpha- and beta computation for filler words

    Revision 1.1.2.14  2001/06/14 17:38:03  soltau
    glatConnect: compute backward probability based on a particular link only
    glatPrune  : prune links as well

    Revision 1.1.2.13  2001/06/14 16:42:54  soltau
    Redesigned lattice pruning

    Revision 1.1.2.12  2001/06/12 20:07:20  soltau
    Increased surviving chance for lattice nodes

    Revision 1.1.2.11  2001/06/11 14:40:01  soltau
    Removed and introduced a couple of problems

    Revision 1.1.2.10  2001/06/08 18:24:28  metze
    1st version of NJD lattice file format implemented

    Revision 1.1.2.9  2001/06/07 15:28:27  soltau
    Added preliminary glatAlign

    Revision 1.1.2.8  2001/06/06 10:57:52  soltau
    Cleaned up

    Revision 1.1.2.7  2001/06/06 07:47:15  metze
    Added writing of lattices
    Added possibility to add nodes

    Revision 1.1.2.6  2001/06/05 19:20:44  soltau
    Added Lattice Alignment
    Fixed rcm score problems

    Revision 1.1.2.5  2001/06/05 10:13:19  soltau
    Added glatDeleteParents, gnodeDeleteSubtree, glatConnect, glatResetInfo

    Revision 1.1.2.4  2001/06/01 14:27:18  soltau
    Made it working (hopefully)

    Revision 1.1.2.3  2001/06/01 10:38:21  soltau
    Added Backward links, topN pruning, glatPurify, but still not working

    Revision 1.1.2.2  2001/05/29 18:36:56  soltau
    Added a couple of glatAdd* functions

    Revision 1.1.2.1  2001/05/28 18:38:51  soltau
    Initial revision
 

   ======================================================================== */

char  glatRcsVersion[]=
    "$Id: glat.c 3428 2011-04-15 18:00:28Z metze $";

#include <math.h>
#include "glat.h"
#include "smem.h"
#include "stree.h"
#include "spass.h"
#include "lmla.h"


extern TypeInfo  glatInfo;
static GLat      glatDefault;


/* ========================================================================
    GLAT Mapper routines
   ======================================================================== */

static int gkeyInit( GKey* gkeyP, GKey* hashkey)
{
  *gkeyP = *hashkey;
  return TCL_OK;
}

static int gkeyDeinit( GKey* gkeyP)
{
  return TCL_OK;
}

static int gkeyHashCmp( const GKey* hashkey, const GKey* gkeyP)
{
  if (hashkey->svX    != gkeyP->svX)    return 0;
  if (hashkey->morphX != gkeyP->morphX) return 0;

  return 1;
}

#define gkeyBit 8
static long gkeyHashKey( const GKey* key)
{
  unsigned long ret = key->svX; 

  ret = ((ret<<gkeyBit) | (ret >> (8*sizeof(long)-gkeyBit))) \
    ^ key->morphX;
  return ret;
}

/* ========================================================================
    Linguistic constraints : Hash functions
   ======================================================================== */

static int lconInit( LCon* lconP, LCon* hashkey)
{
  *lconP = *hashkey;
  return TCL_OK;
}

static int lconDeinit( LCon* lconP)
{
  if (lconP->svXA) free(lconP->svXA);
  lconP->svXA =NULL;
  return TCL_OK;
}

static int lconHashCmp( const LCon* hashkey, const LCon* lconP)
{
  return (hashkey->lct == lconP->lct);
}

#define lconBit 8
static long lconHashKey( const LCon* key)
{
  unsigned long ret = key->lct; 

  ret = (ret<< (8*sizeof(long)-lconBit));
  return ret;
}


/* ========================================================================
    GLAT init routines
   ======================================================================== */

/* ------------------------------------------------------------------------
    glatInit
   ------------------------------------------------------------------------ */

static int glatInit(GLat* latP, char* name, SVMap* svmapP, SPass* spassP)
{
  latP->name   = strdup(name);
  latP->useN   = 0;
  latP->svmapP = svmapP;
  latP->spassP = spassP;

  link(svmapP,"SVMap");
  if (spassP) {
    assert(spassP->streeP->svocabP == svmapP->svocabP);
  }

  latP->nodeM       = 0;
  latP->nodeN       = 0;
  latP->linkN       = 0;
  latP->status      = LAT_INIT;
  latP->expert      = 0;
  latP->singularLCT = 0;
  latP->frameShift  = 0.01;

  latP->end.frameX = 0;
  latP->end.lvX    = svmapP->idx.end;

  latP->mapping.b2n    = NULL;
  latP->mapping.x2n    = NULL;
  latP->mapping.allocN = 0;

  listInit((List*) &(latP->mapping.s2n),(TypeInfo*) NULL,sizeof(GKey),1000);
  latP->mapping.s2n.init    = (ListItemInit  *)gkeyInit;
  latP->mapping.s2n.deinit  = (ListItemDeinit*)gkeyDeinit;
  latP->mapping.s2n.hashKey = (HashKeyFn     *)gkeyHashKey;
  latP->mapping.s2n.hashCmp = (HashCmpFn     *)gkeyHashCmp;

  /* lattice constraint */
  listInit((List*) &(latP->LConL),(TypeInfo*) NULL,sizeof(LCon),1000);
  latP->LConL.init    = (ListItemInit  *)lconInit;
  latP->LConL.deinit  = (ListItemDeinit*)lconDeinit;
  latP->LConL.hashKey = (HashKeyFn     *)lconHashKey;
  latP->LConL.hashCmp = (HashCmpFn     *)lconHashCmp;
  latP->lmA           = NULL;

  if (NULL == (latP->startP = malloc(sizeof(GNode)))) {
    ERROR("glatInit: allocation failure\n");
    return TCL_ERROR;
  }
  if (NULL == (latP->endP = malloc(sizeof(GNode)))) {
    ERROR("glatInit: allocation failure\n");
    return TCL_ERROR;
  }

  latP->mem.node   = bmemCreate (5000, sizeof (GNode));
  latP->mem.link   = bmemCreate (5000, sizeof (GLink));
  latP->mem.rcm    = bmemCreate (32768, 0);
  latP->mem.idx    = bmemCreate (1000, 0);
  latP->mem.paths  = bmemCreate (1000, sizeof (GLatPathItem));
  latP->mem.lct    = bmemCreate (16384, 0);

  latP->mem.rcmNodeN = 0;
  latP->mem.rcmHmmN  = 0;
  latP->mem.rcmAA    = NULL;

  latP->startP->svX     = SVX_NIL;
  latP->startP->nodeX   = -1;
  latP->startP->score   = 0.0;
  latP->startP->frameS  = 0;
  latP->startP->frameE  = 0;
  latP->startP->status  = DFS_INIT;
  latP->startP->parentP = NULL;
  latP->startP->childP  = NULL;
  latP->startP->rcm.A   = NULL;
  latP->startP->rcm.X   = NULL;
  latP->startP->rcm.N   = CTX_MAX;
  latP->startP->alpha   = 0.0;
  latP->startP->beta    = 0.0;
  latP->startP->gamma   = 0.0;
  latP->startP->lct.N         = 1;
  latP->startP->lct.A         = bmemAlloc (latP->mem.lct, sizeof (LCT));
  latP->startP->lct.A[0]      = 0; /* probably not correct */
  latP->startP->lct.betaA     = bmemAlloc (latP->mem.lct, sizeof (float));
  latP->startP->lct.betaA[0]  = 0.0;
  latP->startP->lct.alphaA    = bmemAlloc (latP->mem.lct, sizeof (float));
  latP->startP->lct.alphaA[0] = 0.0;

  latP->endP->svX     = SVX_NIL;
  latP->endP->nodeX   = -2;
  latP->endP->score   = 0.0;
  latP->endP->frameS  = 0;
  latP->endP->frameE  = 0;
  latP->endP->status  = DFS_PURE_OK;
  latP->endP->parentP = NULL;
  latP->endP->childP  = NULL;
  latP->endP->rcm.A   = NULL;
  latP->endP->rcm.X   = NULL;
  latP->endP->rcm.N   = CTX_MAX;
  latP->endP->alpha   = 0.0;
  latP->endP->beta    = 0.0;
  latP->endP->gamma   = 0.0;
  latP->endP->lct.N         = 1;
  latP->endP->lct.A         = bmemAlloc (latP->mem.lct, sizeof (LCT));
  latP->endP->lct.A[0]      = 0; /* probably not correct */
  latP->endP->lct.betaA     = bmemAlloc (latP->mem.lct, sizeof (float));
  latP->endP->lct.betaA[0]  = 0.0;
  latP->endP->lct.alphaA    = bmemAlloc (latP->mem.lct, sizeof (float));
  latP->endP->lct.alphaA[0] = 0.0;

  latP->stack.nodeN  = 0;
  latP->stack.allocN = 0;
  latP->stack.nodePA = NULL;

  latP->sorter.startCntA   = NULL;
  latP->sorter.endCntA     = NULL;
  latP->sorter.startAllocA = NULL;
  latP->sorter.endAllocA   = NULL;
  latP->sorter.startPA     = NULL;
  latP->sorter.endPA       = NULL;
  latP->sorter.frameN      = 0;

  latP->align = glatDefault.align;
  latP->beam  = glatDefault.beam;

  return TCL_OK;
}

/* ------------------------------------------------------------------------
    glatCreate
   ------------------------------------------------------------------------ */

GLat* glatCreate(char* name, SVMap* svmapP, SPass* spassP)
{
  GLat* latP = malloc(sizeof(GLat));

  if (NULL == latP) {
    ERROR("glatCreate: allocation failure\n");
    return TCL_OK;
  }
  if (TCL_ERROR == (glatInit(latP,name,svmapP,spassP))) {
    free(latP);
    return NULL;
  }
  return latP;
}

static ClientData glatCreateItf( ClientData clientData, 
                                      int argc, char *argv[])
{
  int          ac = argc;
  SVMap*   svmapP = NULL;
  SPass*   spassP = NULL;
  char      *name = NULL; 

  if ( itfParseArgv( argv[0], &ac, argv, 0,
      "<name>",  ARGV_STRING, NULL, &name, NULL,      "name of the lattice",
      "<SVMap>", ARGV_OBJECT, NULL, &svmapP, "SVMap", "Search Vocabulary Mapper",
      "-spass",  ARGV_OBJECT, NULL, &spassP, "SPass", "Search Pass Decoder",
       NULL) != TCL_OK) return NULL;
  
  return (ClientData) glatCreate(name,svmapP,spassP);
}

/* ------------------------------------------------------------------------
    glatClear
   ------------------------------------------------------------------------ */

int glatClear (GLat* latP)
{
  int nodeX;

  if (latP->mapping.x2n) {
    for (nodeX=0;nodeX<latP->mapping.allocN;nodeX++)
      latP->mapping.x2n[nodeX] = NULL;
  }

  bmemClear (latP->mem.node);
  bmemClear (latP->mem.link);
  bmemClear (latP->mem.rcm);
  bmemClear (latP->mem.idx);
  bmemClear (latP->mem.paths);
  bmemClear (latP->mem.lct);

  listClear((List*) &(latP->LConL));
  listClear((List*) &(latP->mapping.s2n));
  if (latP->mapping.b2n) free(latP->mapping.b2n);
  latP->mapping.b2n = NULL;

  latP->nodeM  = 0;
  latP->nodeN  = 0;
  latP->linkN  = 0;
  latP->status = LAT_INIT;

  latP->end.frameX      = 0;

  latP->startP->svX     = SVX_NIL;
  latP->startP->nodeX   = -1;
  latP->startP->score   = 0.0;
  latP->startP->frameS  = 0;
  latP->startP->frameE  = 0;
  latP->startP->status  = DFS_INIT;
  latP->startP->parentP = NULL;
  latP->startP->childP  = NULL;
  latP->startP->rcm.A   = NULL;
  latP->startP->rcm.X   = NULL;
  latP->startP->rcm.N   = CTX_MAX; 
  latP->startP->lct.N   = 0;
  latP->startP->lct.A   = NULL;

  latP->endP->svX     = SVX_NIL;
  latP->endP->nodeX   = -2;
  latP->endP->score   = 0.0;
  latP->endP->frameS  = 0;
  latP->endP->frameE  = 0;
  latP->endP->status  = DFS_INIT;
  latP->endP->parentP = NULL;
  latP->endP->childP  = NULL;
  latP->endP->rcm.A   = NULL;
  latP->endP->rcm.X   = NULL;
  latP->endP->rcm.N   = CTX_MAX;
  latP->endP->lct.N   = 0;
  latP->endP->lct.A   = NULL;

  latP->stack.nodeN = 0;
  return TCL_OK;
}

static int glatClearItf (ClientData cd, int argc, char *argv[])
{
  GLat* latP = (GLat*) cd;
  glatClear(latP);

  return TCL_OK;
}

/* ------------------------------------------------------------------------
    glatDeinit
   ------------------------------------------------------------------------ */

static int glatDeinit (GLat* latP)
{
  int frameX;

  delink(latP->svmapP,"SVMap");
  glatClear(latP);

  bmemFree(latP->mem.node);
  bmemFree(latP->mem.link);
  bmemFree(latP->mem.rcm);
  bmemFree(latP->mem.idx);
  bmemFree(latP->mem.paths);
  bmemFree(latP->mem.lct);

  if (latP->mem.rcmAA) {
    if (latP->mem.rcmAA[0]) free(latP->mem.rcmAA[0]);
    free(latP->mem.rcmAA);
  }

  if (latP->startP) free(latP->startP);
  if (latP->endP)   free(latP->endP);

  listDeinit((List*) &(latP->mapping.s2n));
  if (latP->mapping.x2n) free(latP->mapping.x2n-2);

  for (frameX = 0; frameX < latP->sorter.frameN; frameX++) {
    if (latP->sorter.startPA[frameX]) free(latP->sorter.startPA[frameX]);
    if (latP->sorter.endPA[frameX])   free(latP->sorter.endPA[frameX]);
  }
  if (latP->sorter.startCntA)   free(latP->sorter.startCntA);
  if (latP->sorter.endCntA)     free(latP->sorter.endCntA);
  if (latP->sorter.startAllocA) free(latP->sorter.startAllocA);
  if (latP->sorter.endAllocA)   free(latP->sorter.endAllocA);
  if (latP->sorter.startPA)     free(latP->sorter.startPA);
  if (latP->sorter.endPA)       free(latP->sorter.endPA);

  if (latP->stack.nodePA)       free(latP->stack.nodePA);
  if (latP->lmA)                free(latP->lmA);

  return TCL_OK;
}

/* ------------------------------------------------------------------------
    glatFree
   ------------------------------------------------------------------------ */

int glatFree(GLat* latP)
{
  if (latP) {
    if (glatDeinit(latP) != TCL_OK) return TCL_ERROR;
    free(latP);
  }
  return TCL_OK;
}

static int glatFreeItf (ClientData clientData)
{
  GLat* glatP = (GLat*)clientData;
  if (glatP->spassP) {
    ERROR("glatFreeItf: cannot destroy glat, detected associated SPASS object\n");
    return TCL_ERROR;
  }
  return glatFree(glatP);
}

/* ========================================================================
    GLAT a-posteriori probabilities
   ======================================================================== */

/* ------------------------------------------------------------------------
    compute corresponding linguistic states
   ------------------------------------------------------------------------ */

static void gnodeComputeLCT(GNode* nodeP, GLat* latP, SVMap* svmapP)
{
  LingKS* lksP  = svmapP->lksP;
  GLink* linkP  = nodeP->parentP;
  int    filTag = (nodeP->nodeX) < 0 ? 0 : svmapP->svocabP->list.itemA[nodeP->svX].fillerTag;
  LVX       lvX = LVX_NIL;
  int  _lctN = 0;
  LCT* _lctA = NULL;
  int      i;

  /* nothing to do */
  if (nodeP->status == DFS_LCT) return;

  nodeP->status = DFS_LCT;

  if (!filTag)
    lvX = svmapGet(svmapP,nodeP->svX);

  /* handle begin of sentence */
  if (lvX == svmapP->idx.start) {
    assert (linkP->parentP->nodeX == -1);

    nodeP->lct.N         = 1;
    nodeP->lct.A         = bmemAlloc (latP->mem.lct, sizeof (LCT));
    nodeP->lct.A[0]      = linkP->parentP->lct.A[0];
    nodeP->lct.betaA     = bmemAlloc (latP->mem.lct, sizeof (float));
    nodeP->lct.betaA[0]  = 0.0;
    nodeP->lct.alphaA    = bmemAlloc (latP->mem.lct, sizeof (float));
    nodeP->lct.alphaA[0] = 0.0;

    linkP->map           = bmemAlloc (latP->mem.lct, sizeof (USHORT));
    linkP->map[0]        = 0;

    return;
  }

  /* do nothing for 'deleted' words - they don't exist */
  if (0 && lvX == LVX_NIL && !filTag) {
    nodeP->lct.N = 0;
    nodeP->lct.A = NULL;
    linkP->map   = NULL;
    WARN ("gnodeComputeLCT: word not mapped - untested.\n");
    return;
  }

  /* loop over all parent's lct into static lctA */
  while (linkP) {
    int              lctX, lctY;
    GNode* parentP = linkP->parentP;

    gnodeComputeLCT (parentP, latP, svmapP);

    /* Space to record the indices for the LCT polymorphism */
    linkP->map = bmemAlloc (latP->mem.lct, sizeof (USHORT)*parentP->lct.N);

    for (lctX = 0; lctX < parentP->lct.N; lctX++) {
      LCT lct = parentP->lct.A[lctX];

      /* extend lct (lvX != LVX_NIL shouldn't be here) */
      if (!filTag && lvX != LVX_NIL)
	lct = lksP->extendLCT (lksP, lct, lvX);

      /* lct recombination */
      assert (_lctA || !nodeP->lct.N); // XCode says _lctA could be NULL
      for (lctY = 0; lctY < nodeP->lct.N; lctY++)
	if (_lctA[lctY] == lct)
	  break;

      linkP->map[lctX] = lctY;

      /* allocate static memory */
      if (nodeP->lct.N == _lctN) {
	_lctN += 100;
	if (NULL == (_lctA = realloc (_lctA, _lctN * sizeof(LCT)))) {
	  ERROR ("gnodeComputeLCT: allocation failure.\n");
	  return;
	}
      }
      if (lctY == nodeP->lct.N) {
	_lctA[lctY] = lct;
	nodeP->lct.N++;
      }     
    }

    if (nodeP->nodeX == -2)
      for (lctX = 0; lctX < parentP->lct.N; lctX++) 
	linkP->map[lctX] = 0;

    linkP = linkP->txenP;
  }

  if (nodeP->lct.N == 0)
    WARN ("gnodeComputeLCT: '%s' nodeX= %d couldn't find any linguistic states.\n",
	  (nodeP->svX < svmapP->svocabP->list.itemN) ? 
	  svmapP->svocabP->list.itemA[nodeP->svX].name : "NIL", nodeP->nodeX);

  if (latP->singularLCT && nodeP->lct.N > 1 && latP->expert)
    WARN ("gnodeComputeLCT: '%s' nodeX= %d lctN= %d singularLCT not observed.\n",
	  (nodeP->svX < svmapP->svocabP->list.itemN) ? 
	  svmapP->svocabP->list.itemA[nodeP->svX].name : "NIL", nodeP->nodeX, nodeP->lct.N);

  nodeP->lct.A      = bmemAlloc (latP->mem.lct, nodeP->lct.N*sizeof(LCT));
  nodeP->lct.betaA  = bmemAlloc (latP->mem.lct, nodeP->lct.N*sizeof(float));
  nodeP->lct.alphaA = bmemAlloc (latP->mem.lct, nodeP->lct.N*sizeof(float));
  memcpy (nodeP->lct.A, _lctA, nodeP->lct.N*sizeof(LCT));
  for (i = 0; i < nodeP->lct.N; i++)
    nodeP->lct.betaA[i] = nodeP->lct.alphaA[i] = 0.0;

  if (_lctA) free(_lctA);

  return;
}

static int glatComputeLCT(GLat* latP, SVMap* svmapP) 
{
  int    nodeX;
  GNode* nodeP = latP->startP;

  for (nodeX = -2; nodeX < latP->nodeM; nodeX++) {
    if (latP->mapping.x2n[nodeX]) {
      latP->mapping.x2n[nodeX]->status = DFS_INIT;
      latP->mapping.x2n[nodeX]->lct.N  = 0;
    }
  }
  bmemClear (latP->mem.lct);

  /* handle start of sentence */
  nodeP->lct.N         = 1;
  nodeP->lct.A         = bmemAlloc (latP->mem.lct, sizeof (LCT));
  nodeP->lct.A[0]      = svmapP->lksP->createLCT (svmapP->lksP, svmapP->idx.start);
  nodeP->lct.betaA     = bmemAlloc (latP->mem.lct, sizeof (float));
  nodeP->lct.betaA[0]  = 0.0;
  nodeP->lct.alphaA    = bmemAlloc (latP->mem.lct, sizeof (float));
  nodeP->lct.alphaA[0] = 0.0;
  nodeP->status        = DFS_LCT;

  /* dynamic lct generation -> clear lm cache */
  svmapClearCache(svmapP);

  /* compute the LCTs */
  gnodeComputeLCT (latP->endP, latP, svmapP);

  /* do it for loose nodes, too */
  for (nodeX = 0; nodeX < latP->nodeM; nodeX++) {
    if (latP->mapping.x2n[nodeX] && latP->mapping.x2n[nodeX]->status != DFS_LCT && 
        latP->mapping.x2n[nodeX]->childP == NULL)
      gnodeComputeLCT (latP->mapping.x2n[nodeX], latP, svmapP);
  }

  return TCL_OK;
}


/* ------------------------------------------------------------------------
    add log. probabilities
   ------------------------------------------------------------------------ */

static int PROB_SUM = 1;

/* maybe use 'log1p' (log x + log1p(x/y) or so) ? */
double addLogProbs (double p1,double p2) {
  if (PROB_SUM == 1) {
    if (p1 == 0.0) return p2;
    if (p2 == 0.0) return p1;
    if (p1 < p2) {
      return p1 - log1p (exp (p1-p2));
      /* log(exp(p1-p2) + 1.0); */
    } else {
      return p2 - log1p (exp (p2-p1));
      /* log(exp(p2-p1) + 1.0); */
    }
  } else {
    if (p1 == 0.0) return p2;
    return (p1 < p2) ? p1 : p2;
  }
}


/* ------------------------------------------------------------------------
    compute forward probabilities
   ------------------------------------------------------------------------ */

static int gnodeComputeAlphaNew(GNode *nodeP, SVMap *svmapP, float scale)
{
  GLink *linkP=nodeP->parentP;
  int i,j,k;
  float **score;
  float **alpha;
  float *lm;
  GNode **parentPA = NULL;

  /* start of lattice, no parents : terminate recursion */
  if(nodeP->nodeX==-1) { nodeP->alpha=0.0; nodeP->status=DFS_ALPHA; return TCL_OK; }

  if(!linkP) {
    for(i=0;i<nodeP->addon.parentCtxN;i++) {
      for(j=0;j<nodeP->addon.childCtxN;j++)
	nodeP->addon.alpha[i][j]=NEG_LOG_ZERO;
    }
    nodeP->status=DFS_ALPHA;
    return TCL_OK;
  }

  /* loop over all parents */
  while(linkP) {
    GNode *parentP=linkP->parentP;

    /* end of sentence : check for valid parents */
    /* if(nodeP->nodeX == -2 && parentP->frameE != finalFrame) {
       linkP = linkP->txenP;
       continue;
       } */

    if(parentP->status!=DFS_ALPHA)
      gnodeComputeAlphaNew(parentP,svmapP,scale);

    linkP=linkP->txenP;
  }

  linkP=nodeP->parentP;
  if(nodeP->nodeX==-2) {
    nodeP->alpha=0.0;
    while(linkP) {
      GNode *parentP=linkP->parentP;

      /* if(parentP->frameE!=finalFrame) {
	 linkP=linkP->txenP;
	 continue;
	 } */

      int parChdX=-1;
      for(j=0;j<parentP->addon.childCtxN;j++) {
	if(parentP->addon.childP[j]->nodeX==nodeP->nodeX) {
	  parChdX=j;
	  break;
	}
      }
      if(parChdX==-1) {
	ERROR("gnodeComputeAlphaNew: addon vars not initialized properly.\n");
	return TCL_ERROR;
      }
      for(k=0;k<parentP->addon.parentCtxN;k++) {
	/* if(parentP->addon.alpha[k][parChdX]>=SCORE_MAX) {
	   continue;
	   } else { */
	  nodeP->alpha=addLogProbs(nodeP->alpha,parentP->addon.alpha[k][parChdX]);
	  /* } */
      }

      linkP=linkP->txenP;
    }
    nodeP->status=DFS_ALPHA;
    return TCL_OK;
  }

  score=nodeP->addon.score;
  alpha=nodeP->addon.alpha;
  lm=nodeP->addon.lmScore;

  for(i=0;i<nodeP->addon.parentCtxN;i++) {
    for(j=0;j<nodeP->addon.childCtxN;j++)
      alpha[i][j]=0.0;
  }

  parentPA=nodeP->addon.parentP;
  for(i=0;i<nodeP->addon.parentCtxN;i++) {
    float prevAlpha=0.0;
    if(parentPA[i]->nodeX!=-1) {
      int parChdX=-1;
      for(j=0;j<parentPA[i]->addon.childCtxN;j++) {
	if(parentPA[i]->addon.childP[j]->nodeX==nodeP->nodeX) {
	  parChdX=j;
	  break;
	}
      }
      if(parChdX==-1) {
	ERROR("gnodeComputeAlphaNew: addon vars not initialized properly.\n");
	return TCL_ERROR;
      }
      for(k=0;k<parentPA[i]->addon.parentCtxN;k++) {
	/*
	if(parentP[i]->addon.alpha[k][parChdX]>=SCORE_MAX) {
	  continue;
	} else { */
	  prevAlpha=addLogProbs(prevAlpha,parentPA[i]->addon.alpha[k][parChdX]);
	/* } */
      }
    }

    for(j=0;j<nodeP->addon.childCtxN;j++) {
      /* if(score[i][j]>=SCORE_MAX) {
      alpha[i][j]=SCORE_MAX;
      } else { */
	alpha[i][j]=addLogProbs(alpha[i][j],prevAlpha+scale*score[i][j]+lm[i]);
      /* } */
    }
  }

  nodeP->status=DFS_ALPHA;
  return TCL_OK;
} 

static float gnodeComputeAlpha (GNode* nodeP, SVMap* svmapP, int finalFrame, double scale) 
{
  GLink*   linkP = nodeP->parentP;
  SVX        svX = (nodeP->nodeX >= 0) ? nodeP->svX : SVX_NIL;
  double  probLM = 0.0;
  double   lmPen = 0.0;
  double   alpha = SCORE_MAX;
  int     filTag = (nodeP->nodeX >= 0) && svmapP->svocabP->list.itemA[svX].fillerTag;
  LVX      mylvX = LVX_NIL;
  float*  alphaA = nodeP->lct.alphaA;
  int       lctX;

  /* Did we compute already the forward prob for this node */
  if (nodeP->status == DFS_ALPHA) return nodeP->alpha;
  
  nodeP->alpha  = 0.0;
  nodeP->status = DFS_ALPHA;

  /* start of lattice, no parents : terminate recursion */
  if (nodeP->nodeX == -1) {
    for (lctX = 0; lctX < nodeP->lct.N; lctX++)
      nodeP->lct.alphaA[lctX] = nodeP->alpha;
    return nodeP->alpha;
  }

  if (!linkP || nodeP->score >= SCORE_MAX) {
    nodeP->alpha = SCORE_MAX;
    for (lctX = 0; lctX < nodeP->lct.N; lctX++)
      nodeP->lct.alphaA[lctX] = nodeP->alpha;
    return nodeP->alpha;
  }

  /* get pronunciation probability */
  if (svX != SVX_NIL && nodeP->nodeX >= 0) {
    mylvX  = svmapGet (svmapP, svX); 
    probLM = (nodeP->nodeX == -2) ? 0.0 : svmapP->lvxSA[svX] * svmapP->lz;
  }

  /* get lm penalty */
  if      (nodeP->nodeX == -2) lmPen = 0.0;
  else if (filTag)             lmPen = svmapP->penalty.fil; 
  else                         lmPen = (mylvX == svmapP->idx.start) ? 0.0 : svmapP->penalty.word;

  /* loop over all parents */
  while (linkP) {
    GNode* parentP = linkP->parentP;
    int       lctN = parentP->lct.N;
    LCT*      lctA = parentP->lct.A;
    double   score = 0.0;
    USHORT*   mapA = linkP->map;
    int       lctX;

    /* end of sentence : check for valid parents */
    if (nodeP->nodeX == -2 && parentP->frameE != finalFrame) {
      linkP = linkP->txenP;
      continue;
    }
    
    if (gnodeComputeAlpha (parentP, svmapP, finalFrame, scale) >= SCORE_MAX) {
      linkP = linkP->txenP;
      continue;
    }

    /* parent is start_of_lattice -> don't apply lm score,
       node   is   end_of_lattice -> no lm score
       node   is a filler word    -> don't apply lm score */
    score = scale * (nodeP->score + linkP->score + lmPen);
    if (parentP->nodeX == -1 || nodeP->nodeX == -2 || filTag) {
      for (lctX = 0; lctX < lctN; lctX++) {
	alphaA[mapA[lctX]] = addLogProbs (alphaA[mapA[lctX]], parentP->lct.alphaA[lctX]+score+probLM);
	/* if (alphaA[mapA[lctX]] < alpha) alpha = alphaA[mapA[lctX]]; */
      }
    } else {
      /* treat each linguistic state of the parent as a virtual parent node */
      for (lctX = 0; lctX < lctN; lctX++) {
	double lm = svmapGetLMScore (svmapP, lctA[lctX], svX) * svmapP->lz * scale;
	alphaA[mapA[lctX]] = addLogProbs (alphaA[mapA[lctX]], parentP->lct.alphaA[lctX]+score+lm);
	/* if (alphaA[mapA[lctX]] < alpha) alpha = alphaA[mapA[lctX]]; */
      }
    }

    linkP = linkP->txenP;
  } 

  /* summing (depending on PROB_SUM) */
  if ( PROB_SUM )
    alpha = 0.0;
  else
    alpha = SCORE_MAX;
  for (lctX = 0; lctX < nodeP->lct.N; lctX++) {
    /* For the end node, normally only those LCTs comming from nodes
       with no valid parents should be skipped. Since it is impossible
       to find that out we just skip those with an alpha == 0
       We also set all alphas with 0 to SCORE_MAX */
    if ( alphaA[lctX] == 0 ) {
      alphaA[lctX] = SCORE_MAX;
      if ( nodeP->nodeX == -2 ) continue;
    }
    alpha = addLogProbs (alpha, alphaA[lctX]);
  }

  /* if all parents were out of range */
  return  nodeP->alpha = (alpha == 0.0) ? SCORE_MAX : alpha;
}

/* ------------------------------------------------------------------------
    compute backward probabilities
   ------------------------------------------------------------------------ */

static int gnodeComputeBetaNew (GNode *nodeP, SVMap *svmapP, float scale)
{
  GLink *linkP=nodeP->childP;
  int i,j,k;
  float **beta;
  GNode **childPA = NULL;


  /* end of lattice: terminate recursion */
  if(nodeP->nodeX==-2) { nodeP->beta=0.0; nodeP->status=DFS_BETA; return TCL_OK; };
  
  if(!linkP) {
    for(j=0;j<nodeP->addon.childCtxN;j++) {
      for(i=0;i<nodeP->addon.parentCtxN;i++)
	nodeP->addon.beta[i][j]=NEG_LOG_ZERO;
    }
    nodeP->status=DFS_BETA;
    return TCL_OK;
  }

  /* loop over all children */
  while(linkP) {
    GNode *childP=linkP->childP;
    
    if(childP->status!=DFS_BETA)
      gnodeComputeBetaNew(childP,svmapP,scale);

    linkP=linkP->nextP;
  }

  linkP=nodeP->childP;
  if(nodeP->nodeX==-1) {
    nodeP->beta=0.0;
    while(linkP) {
      GNode *childP=linkP->childP;

      int chdParX=-1;
      for(i=0;i<childP->addon.parentCtxN;i++) {
	if(childP->addon.parentP[i]->nodeX==nodeP->nodeX) {
	  chdParX=i;
	  break;
	}
      }
      if(chdParX==-1) {
	ERROR("gnodeComputeBetaNew: addon vars not initialized properly.\n");
	return TCL_ERROR;
      }
      for(k=0;k<childP->addon.childCtxN;k++) {
	  nodeP->beta=addLogProbs(nodeP->beta,childP->addon.beta[chdParX][k]+scale*childP->addon.score[chdParX][k]+childP->addon.lmScore[chdParX]);
      }

      linkP=linkP->nextP;
    }
    nodeP->status=DFS_BETA;
    return TCL_OK;
  }

  beta=nodeP->addon.beta;

  for(j=0;j<nodeP->addon.childCtxN;j++) {
    for(i=0;i<nodeP->addon.parentCtxN;i++)
      beta[i][j]=0.0;
  }

  childPA=nodeP->addon.childP;
  for(j=0;j<nodeP->addon.childCtxN;j++) {
    float follBeta=0.0;

    if(childPA[j]->nodeX!=-2) {
      int chdParX=-1;
      for(i=0;i<childPA[j]->addon.parentCtxN;i++) {
	if(childPA[j]->addon.parentP[i]->nodeX==nodeP->nodeX) {
	  chdParX=i;
	  break;
	}
      }
      if(chdParX==-1) {
	ERROR("gnodeComputeBetaNew: addon vars not initialized properly.\n");
	return TCL_ERROR;
      }
      for(k=0;k<childPA[j]->addon.childCtxN;k++) {
	  follBeta=addLogProbs(follBeta,childPA[j]->addon.beta[chdParX][k]+scale*childPA[j]->addon.score[chdParX][k]+childPA[j]->addon.lmScore[chdParX]);
      }
    }

    for(i=0;i<nodeP->addon.parentCtxN;i++)
      beta[i][j]=addLogProbs(beta[i][j],follBeta);
  }

  nodeP->status=DFS_BETA;
  return TCL_OK;
}
  
  

static float gnodeComputeBeta (GNode* nodeP, SVMap* svmapP, int finalFrame, double scale)
{
  GLink*   linkP = nodeP->childP;
  int       lctN = nodeP->lct.N;
  LCT*      lctA = nodeP->lct.A;  
  double    beta = SCORE_MAX;
  LVX      mylvX = LVX_NIL;
  float*  betaA = nodeP->lct.betaA;
  int       lctX;

  /* Did we compute already the backward prob for this node */
  if (nodeP->status == DFS_BETA) return nodeP->beta;

  nodeP->beta   = 0.0;
  nodeP->status = DFS_BETA;
  mylvX         = svmapGet (svmapP, nodeP->svX);

  /* end of lattice, no children, final node : terminate recursion */ 
  if (nodeP->nodeX == -2) {
    for (lctX = 0; lctX < lctN; lctX++)
      nodeP->lct.betaA[lctX] = nodeP->beta;
    return nodeP->beta;
  }
  if (!linkP || nodeP->score >= SCORE_MAX || mylvX == svmapP->idx.end) {
    nodeP->beta = (mylvX == svmapP->idx.end && nodeP->frameE == finalFrame) ? 0.0 : SCORE_MAX;
    for (lctX = 0; lctX < lctN; lctX++)
      nodeP->lct.betaA[lctX] = nodeP->beta;
    return nodeP->beta;
  }

  /* loop over children */
  while (linkP) {
    GNode* childP = linkP->childP;
    double probLM = (childP->nodeX >= 0) ?  svmapP->lvxSA[childP->svX] * svmapP->lz : 0.0;
    int   filTag2 = (childP->nodeX >= 0) && svmapP->svocabP->list.itemA[childP->svX].fillerTag;
    double  score = 0.0;
    USHORT*  mapA = linkP->map;

    /* For some pathologic links */
    if (!mapA) {
      WARN ("Empty link between nodes %d and %d.\n", linkP->parentP->nodeX, linkP->childP->nodeX);
      linkP = linkP->nextP;
      continue;
    }

    if (gnodeComputeBeta (childP, svmapP, finalFrame, scale) >= SCORE_MAX) {
      linkP = linkP->nextP;
      continue;
    }

    /* node is start_of_lattice or child is end_of_lattice -> no transition score,
                                      child is filler word -> no lm score */
    score = scale * (childP->score + linkP->score);
    if (nodeP->nodeX != -1 && childP->nodeX != -2)
      score += scale * ((filTag2) ? svmapP->penalty.fil : svmapP->penalty.word);
    
    if (filTag2 || nodeP->nodeX == -1 || childP->nodeX == -2) {
      for (lctX = 0; lctX < lctN; lctX++) {
        betaA[lctX] = addLogProbs (betaA[lctX], childP->lct.betaA[mapA[lctX]]+score+probLM);
        /* if (betaA[lctX] < beta) beta = betaA[lctX]; */
      }
    } else {
      for (lctX = 0; lctX < lctN; lctX++) {
        double   lm = svmapGetLMScore (svmapP, lctA[lctX], childP->svX) * svmapP->lz * scale;
        betaA[lctX] = addLogProbs (betaA[lctX], childP->lct.betaA[mapA[lctX]]+score+lm);
        /* if (betaA[lctX] < beta) beta = betaA[lctX]; */
      }
    }

    linkP = linkP->nextP;
  }

  /* if children were out of range */
  for (lctX = 0; lctX < lctN; lctX++)
    if (nodeP->lct.betaA[lctX] == 0)
      nodeP->lct.betaA[lctX] = SCORE_MAX;

  /* summing (depending on PROB_SUM) */
  beta = nodeP->lct.betaA[0];
  for (lctX = 1; lctX < nodeP->lct.N; lctX++)
    beta = addLogProbs (beta, nodeP->lct.betaA[lctX]);

  return nodeP->beta = (beta == 0) ? SCORE_MAX : beta;
}

/* ------------------------------------------------------------------------
    compute a- posterioris
   ------------------------------------------------------------------------ */

float gnodeComputeGamma (GNode* nodeP)
{
  float gamma = 0.0;
  int    lctX;
  for (lctX = 0; lctX < nodeP->lct.N; lctX++)
    gamma = addLogProbs (gamma, nodeP->lct.alphaA[lctX] + nodeP->lct.betaA[lctX]);
  return gamma;
}

float glatComputeGammaNew(GLat* latP, SVMap* svmapP, int prob_sum, float scale, 
		       int clusterModus, int clusterFiller)
{
  float likelihood = 0.0;
  int    nodeX;
  int parentX,childX;

  PROB_SUM = prob_sum;

  /* ----------------------------------------- */
  /* get forward / backward probs              */
  /* ----------------------------------------- */

  svmapClearCache (svmapP);
  glatComputeLCT  (latP, svmapP);

  for (nodeX = 0; nodeX < latP->nodeN; nodeX++)
    if (latP->mapping.x2n[nodeX]) 
      latP->mapping.x2n[nodeX]->status = DFS_INIT;

  for(nodeX=0;nodeX<latP->nodeN;nodeX++) {
    GNode *nodeP=latP->mapping.x2n[nodeX];
    SVX svX=(nodeP->nodeX>=0) ? nodeP->svX : SVX_NIL;
    int filTag=(nodeP->nodeX>=0) && svmapP->svocabP->list.itemA[svX].fillerTag;
    LVX mylvX=LVX_NIL;
    float probLM=0.0;
    float lmPen;
    int i;
    GNode **parentPA = NULL;

    if(svX!=SVX_NIL)
      mylvX=svmapGet(svmapP,svX);

    if(svX!=SVX_NIL)
      probLM=svmapP->lvxSA[svX]*svmapP->lz*scale;

    if(nodeP->nodeX==-1 || nodeP->nodeX==-2) lmPen=0.0;
    else if(filTag) lmPen=svmapP->penalty.fil*scale;
    else lmPen=(mylvX==svmapP->idx.start) ? 0.0: svmapP->penalty.word*scale;

    parentPA=nodeP->addon.parentP;
    for(i=0;i<nodeP->addon.parentCtxN;i++) {
      LCT *lctA=parentPA[i]->lct.A;
      float lm;

      if(parentPA[i]->nodeX==-1||filTag) {
	lm=probLM;
      } else {
	lm=svmapGetLMScore(svmapP,lctA[0],svX)*svmapP->lz*scale;
      }

      nodeP->addon.lmScore[i]=lm+lmPen;
    }
  } 

  gnodeComputeBetaNew  (latP->startP, svmapP, scale);
  gnodeComputeAlphaNew (latP->endP,   svmapP, scale);

  /* ----------------------------------------- */
  /* extract posterioris                       */
  /* ----------------------------------------- */

  /* likelihood : best of forward/backward score */
  likelihood = latP->endP->alpha;
  if (latP->startP->beta < likelihood)
      likelihood=latP->startP->beta;

  for(nodeX=0;nodeX<latP->nodeN;nodeX++) {
    GNode *nodeP=latP->mapping.x2n[nodeX];
    if(nodeP) {
      for(parentX=0;parentX<nodeP->addon.parentCtxN;parentX++) {
	for(childX=0;childX<nodeP->addon.childCtxN;childX++) {
	  nodeP->addon.gamma[parentX][childX]= nodeP->addon.alpha[parentX][childX]+nodeP->addon.beta[parentX][childX]-likelihood;
	}
      }
    }
  }
  return likelihood;
}


float glatComputeGamma(GLat* latP, SVMap* svmapP, int prob_sum, double scale, 
		       int clusterModus, int clusterFiller)
{
  double offset = 0;
  int    nodeX, refX;

  PROB_SUM = prob_sum;

  /* ----------------------------------------- */
  /* get forward / backward probs              */
  /* ----------------------------------------- */

  svmapClearCache (svmapP);
  glatComputeLCT  (latP, svmapP);

  for (nodeX = 0; nodeX < latP->nodeM; nodeX++)
    if (latP->mapping.x2n[nodeX]) 
      latP->mapping.x2n[nodeX]->status = DFS_INIT;

  gnodeComputeBeta  (latP->startP, svmapP, latP->end.frameX, scale);
  gnodeComputeAlpha (latP->endP,   svmapP, latP->end.frameX, scale);

  /* ----------------------------------------- */
  /* check also for partial paths              */
  /* ----------------------------------------- */

  for (nodeX=0;nodeX<latP->nodeM;nodeX++) {
    if (latP->mapping.x2n[nodeX] && latP->mapping.x2n[nodeX]->status != DFS_ALPHA && 
	latP->mapping.x2n[nodeX]->childP == NULL) {
	gnodeComputeAlpha(latP->mapping.x2n[nodeX],svmapP,latP->end.frameX,scale);
    }
  }

  /* ----------------------------------------- */
  /* extract posterioris                       */
  /* ----------------------------------------- */

  /* posteriori offset : best of forward/backward score */
  offset = latP->endP->alpha;
  if (latP->startP->beta < offset)
      offset=latP->startP->beta;

  /* We have two choices how to compute gamma:
     1) We compute gamma for each lct separately and take the best one (prob_sum=1)
        This should be the case for posteriori computation
     2) We compute gamma over alpha and beta, so that the best path is
        guaranteed to stay in the lattice (prob_sum=0, used for pruning) */
  for (refX = 0; refX < latP->nodeM; refX++)
    if (latP->mapping.x2n[refX]) 
      latP->mapping.x2n[refX]->gamma = (prob_sum) 
	? gnodeComputeGamma (latP->mapping.x2n[refX]) - offset 
	: latP->mapping.x2n[refX]->alpha + latP->mapping.x2n[refX]->beta  - offset;

  /* ----------------------------------------- */
  /* sum over 'similar' nodes                  */
  /* ----------------------------------------- */

  if (clusterModus > 0) {   
    double*  tmpA = (double*) calloc (latP->nodeM, sizeof(double));

    for (refX = 0; refX < latP->nodeM; refX++)
      if (latP->mapping.x2n[refX])
	tmpA[refX] = latP->mapping.x2n[refX]->gamma;

    for (refX = 0; refX < latP->nodeM; refX++) {
      GNode* nodeP = latP->mapping.x2n[refX];
      int   frameX = nodeP->frameS + (nodeP->frameE-nodeP->frameS)/2;
      LVX      lvX = svmapGet (svmapP, nodeP->svX);
      int    nodeX;

      if (!nodeP) continue;

      /* don't cluster filler words */
      if (!clusterFiller && svmapP->svocabP->list.itemA[nodeP->svX].fillerTag)
	continue;

      for (nodeX = 0; nodeX < latP->nodeM; nodeX++) {
	GNode* node2P = latP->mapping.x2n[nodeX];

	/* Hagen says, it can happen that this doesn't exist */
	if (!node2P) continue;

	/* don't cluster identical nodes */
	if (nodeX == refX) continue;

	/* don't cluster nodes with different time frame */
	if (node2P->frameS >= frameX || node2P->frameE <= frameX) continue;

	/* don't cluster nodes with different vocab words */
	if (clusterModus == 1 && nodeP->svX != node2P->svX)
	  continue;

	/* don't cluster nodes with different lm words */
	if (clusterModus == 2 && lvX != svmapGet (svmapP, node2P->svX))
	  continue;

	if (tmpA[refX] == 0.0) tmpA[refX] = FLT_MIN;
	tmpA[refX] = addLogProbs (tmpA[refX], (double) node2P->gamma);
      }
    }

    for (refX = 0; refX < latP->nodeM; refX ++)
      if (latP->mapping.x2n[refX])
	latP->mapping.x2n[refX]->gamma = tmpA[refX];

    free (tmpA);
  }
  
  return latP->startP->beta;
}


static int glatComputeGammaItf (ClientData cd, int argc, char *argv[])
{
  GLat*  latP   = (GLat*) cd;
  SVMap* svmapP = latP->svmapP;
  float  scale  = 1.0;
  int       ac  = argc-1;
  char*   psstr = "sum";
  int  prob_sum = 1;
  char*   tystr = "none";
  int     modus = 0;
  int tieFiller = 0;
  int      mmie = 0;
  float likelihood;

  if (itfParseArgv( argv[0], &ac, argv+1, 0,
      "-map",     ARGV_OBJECT, NULL, &svmapP, "SVMap", "Vocabulary Mapper",
      "-scale",   ARGV_FLOAT,  NULL, &scale,     NULL, "mystic scaling factor",  
      "-sum",     ARGV_STRING, NULL, &psstr,     NULL, "sum or max over prob's",  
      "-tie",     ARGV_STRING, NULL, &tystr,     NULL, "node tying: none, svX, lvX",  
      "-tieFiller",  ARGV_INT, NULL, &tieFiller, NULL, "include filler words for clustering",
      "-mmie",       ARGV_INT, NULL, &mmie,      NULL, "use functions for MMIE training",
       NULL) != TCL_OK) return TCL_ERROR;
  
  if (!svmapP) {
    ERROR("glatComputeGammaItf: no Vocabulary Mapper\n");
    return TCL_ERROR;
  }
  if (latP->nodeN+latP->linkN < 2) {
    WARN ("glatComputeGammaItf: Empty Lattice.\n");
    return TCL_OK;
  }
  if (latP->status == LAT_CREATE) {
    WARN("glatComputeGamma: forced purifying\n");
    glatPurify(latP);
  }
  if        (streq (psstr, "sum")  || streq (psstr, "1")) {
    prob_sum = 1;
  } else if (streq (psstr, "max")  || streq (psstr, "0")) {
    prob_sum = 0;
  } else {
    ERROR ("How do you want your probs treated: '%s'?\n", psstr);
    return TCL_ERROR;
  }
  if        (streq (tystr, "none") || streq (tystr, "0")) {
    modus = 0;
  } else if (streq (tystr, "svX")  || streq (tystr, "1")) {
    modus = 1;
  } else if (streq (tystr, "lvX")  || streq (tystr, "2")) {
    modus = 2;
  } else {
    ERROR ("Tying mode '%s' unknown.\n", tystr);
    return TCL_ERROR;
  }

  if ( mmie ) {
    INFO ("Using MMIE code.\n");
    glatComputeGamma(latP,svmapP,prob_sum,scale,modus,tieFiller); /* from Roger */
    likelihood=glatComputeGammaNew(latP,svmapP,prob_sum,scale,modus,tieFiller);
  } else {
    likelihood=glatComputeGamma(latP,svmapP,prob_sum,scale,modus,tieFiller);
  }

  itfAppendResult("%e",likelihood);
  return TCL_OK;
}

/* ------------------------------------------------------------------------
    Use posterioris as confidence measure
   ------------------------------------------------------------------------ */

/* forward declaration */
static int glatAlign (GLat* latP, int svN, SVX* svxA, int verbose, int ignoreFil,
                      int* corN, int* delN, int* insN, int* subN, GBP*** gbptr);

static int glatComputeConfidence (GLat* latP, SVMap* svmapP, int svN, SVX* svxA,
                                  float scale, int norm, int prob_sum, int modus,
                                  int verbose)
{
  SWord* swordP = svmapP->svocabP->list.itemA;
  int      refX = 0;
  int      corN = 0;
  int      errN = 0;
  GBP**     gbA = NULL;
  int*      nxA = NULL;
  double* gammaA = NULL;
  LVX        lvX = LVX_NIL;

  PROB_SUM = prob_sum;

  svmapClearCache(svmapP);

  /* Check path */ 
  lvX = svmapGet(svmapP,svxA[0]);
  if (lvX != latP->svmapP->idx.start) {
    ERROR ("glatComputeConfidence: no begin of sentence '%s' at begin of path\n",
          latP->svmapP->string.start);
    return TCL_ERROR;
  }
  lvX = svmapGet(svmapP,svxA[svN-1]);
  if (lvX != latP->svmapP->idx.end) {
    ERROR ("glatComputeConfidence: no end of sentence '%s' at end of path\n",
          latP->svmapP->string.end);
    return TCL_ERROR;
  }

  /* compute a-posterioris */
  if (scale >0)
    glatComputeGamma(latP,svmapP,prob_sum,scale,0,0);

  if (latP->expert && fabs(latP->startP->beta - latP->endP->alpha) > 0.1) {
    INFO("glatComputeConfidence: inconsistent Forward-Backward pass alpha= %2.2f  beta= %2.2f\n",
	 latP->endP->alpha,latP->startP->beta);
  }

  /* compute lattice alignment */
  glatAlign (latP,svN, svxA, 0,1,&corN, &errN, &errN, &errN, &gbA);
  if (corN != svN || errN > 0)  {
    ERROR("glatComputeConfidence: couldn't find hypothesis in lattice\n");
    if (gbA) {
      if (gbA[0]) free(gbA[0]);
      free(gbA);
    }
    /* align in 'verbose' mode */
    glatAlign (latP,svN, svxA, 1, 1,&corN, &errN, &errN, &errN, NULL);
    return TCL_ERROR;
  }

  /* backtrace : fill nxA[refX] = hypX */
  if (NULL== (nxA    = malloc(svN*sizeof(int))) || 
      NULL== (gammaA = malloc(svN*sizeof(double)))) {
    ERROR("glatComputeConfidence: allocation failure\n");
    return TCL_ERROR;
  } else {
    float finalscore = SCORE_MAX;
    GLink*     linkP = latP->endP->parentP;
    int         refX = svN;
    int         hypX = 0;

    while (linkP) {
      float score = gbA[refX][linkP->parentP->nodeX+1].score;
      if (score < finalscore) {
	finalscore = score;
	hypX       = linkP->parentP->nodeX+1;
      }
      linkP = linkP->txenP;
    }
    while (hypX >= 1 || refX >= 1) {
      GBP*  gbP      = gbA[refX]+hypX;    
      nxA[refX-1]    = hypX-1;
      gammaA[refX-1] = (double) latP->mapping.x2n[hypX-1]->gamma;
      hypX           = gbP->hypX;
      refX           = gbP->refX;
    }
  }

  /* sum over 'similiar' nodes */
  if (modus != 0) {
    for (refX=0;refX<svN;refX++) {
      GNode* nodeP = latP->mapping.x2n[nxA[refX]];
      LVX      lvX = (modus == 2) ? svmapGet(svmapP,nodeP->svX) : LVX_NIL;
      int   frameX = nodeP->frameS + (nodeP->frameE-nodeP->frameS)/2;
      int    nodeX = 0;
      
      for (nodeX=0;nodeX<latP->nodeN;nodeX++) {
	GNode* node2P = latP->mapping.x2n[nodeX];
        LVX      lv2X = (modus == 2) ? svmapGet(svmapP,node2P->svX) : LVX_NIL;

        if (nodeP == node2P)                         continue;
        if (modus == 1 && nodeP->svX != node2P->svX) continue;
        if (modus == 2 && lvX        != lv2X)        continue;

        if (node2P->frameS < frameX && node2P->frameE > frameX) {
          /* gammaA[refX] == 0 means that we have already a probability of 1
             and not an initial state */
	  if (gammaA[refX] == 0.0) gammaA[refX] = FLT_MIN;
	  gammaA[refX] = addLogProbs (gammaA[refX], (double) node2P->gamma);
	}
      }
    }
  }

  /* dump posterioris */
  for (refX=0;refX<svN;refX++) {
    int    x = latP->mapping.x2n[nxA[refX]]->frameS;
    int    y = latP->mapping.x2n[nxA[refX]]->frameE;
    double s = (norm) ? exp(-1.0*gammaA[refX]) : gammaA[refX];
    if (verbose)
      itfAppendResult(" { {%s} %d %d %lg} ",swordP[svxA[refX]].name,x,y,s);
    else
      itfAppendResult(" { {%s} %lg} ",swordP[svxA[refX]].name,s);
  }

  if (gbA) {
    if (gbA[0]) free(gbA[0]);
      free(gbA);
  }
  if (nxA)    free(nxA);
  if (gammaA) free(gammaA);

  return TCL_OK;
}

static int glatComputeConfidenceItf (ClientData cd, int argc, char *argv[])
{
  GLat*    latP = (GLat*) cd;
  int        ac = argc -1;
  SVMap* svmapP = latP->svmapP;
  SVX*     svxA = NULL;
  float   scale = 1.0;
  int      norm = 1;
  char*   psstr = "sum";
  int  prob_sum = 1;
  char*   tystr = "svX";
  int     modus = 1;
  int   verbose = 0;
  int      refX = 0;
  int      refN = 0;
  int        rc = TCL_ERROR;
  char*     ref = NULL;
  char** refv;

  if ( itfParseArgv( argv[0], &ac, argv+1, 0,
     "<ref>",   ARGV_STRING, NULL, &ref,     NULL, "sequence of words",
      "-map",   ARGV_OBJECT, NULL, &svmapP, "SVMap", "Vocabulary Mapper",
      "-sum",   ARGV_STRING, NULL, &psstr,   NULL, "sum or max over prob's",
      "-tie",   ARGV_STRING, NULL, &tystr,   NULL, "node tying: none, svX, lvX",
      "-scale", ARGV_FLOAT,  NULL, &scale,   NULL, "mystic scaling factor",
      "-norm",  ARGV_INT,    NULL, &norm,    NULL, "puts real probabilities instead of negative log",
      "-v",     ARGV_INT,    NULL, &verbose, NULL, "puts time information",
       NULL) != TCL_OK) return TCL_ERROR;

  if (latP->nodeN+latP->linkN < 2) {
    ERROR("glatComputeConfidenceItf: Empty Lattice\n");
    return TCL_OK;
  }
  if (!svmapP) {
    ERROR("glatComputeConfidenceItf: no Vocabulary Mapper\n");
    return TCL_ERROR;
  }
  if (latP->status == LAT_CREATE) {
    WARN("glatComputeConfidence: forced purifying\n");
    glatPurify(latP);
  }
  if        (streq (tystr, "none") || streq (tystr, "0")) {
    modus = 0;
  } else if (streq (tystr, "svX")  || streq (tystr, "1")) {
    modus = 1;
  } else if (streq (tystr, "lvX")  || streq (tystr, "2")) {
    modus = 2;
  } else {
    ERROR ("Tying mode '%s' unknown.\n", tystr);
    return TCL_ERROR;
  }

  if (Tcl_SplitList( itf,ref, &refN, &refv) == TCL_ERROR || refN < 1) {
    ERROR("glatComputeConfidenceItf: cannot split sequence %s\n",ref);
    return TCL_ERROR;
  }

  if (NULL == (svxA = malloc(refN*sizeof(SVX)))) {
    ERROR("glatComputeConfidenceItf: allocation error\n");
    Tcl_Free((char*) refv);
    return TCL_ERROR;
  }
  for (refX = 0;refX<refN;refX++) {
    int svX = listIndex((List*) &(svmapP->svocabP->list),refv[refX],0);
    if (svX < 0) {
      ERROR("glatComputeConfidenceItf: %s not in vocabulary\n",refv[refX]);
      if (svxA) free(svxA);
      Tcl_Free((char*) refv);
      return TCL_ERROR;
    }
    svxA[refX]=svX;
  }

  rc = glatComputeConfidence(latP,svmapP,refN,svxA,scale,norm,prob_sum,modus,verbose);

  if (svxA) free(svxA);
  Tcl_Free((char*) refv);

  return rc;
}


/* ========================================================================
    GLAT alignment
   ======================================================================== */

/* ------------------------------------------------------------------------
    glatGetRealParents for filler words 
   ------------------------------------------------------------------------ */

static int glatGetRealParents(GLat* latP,GNode*** parentPLA, int* parentNA,
			      int ignoreFil)
{
  SWord* swordP = latP->svmapP->svocabP->list.itemA;
  int nodeX;

  for (nodeX=0;nodeX<latP->nodeN ;nodeX++) {
    GNode* nodeP = latP->mapping.x2n[nodeX];
    GLink* linkP = nodeP->parentP;

    /* first loop over non-filler parents */
    while (linkP) {
      GNode* parentP = linkP->parentP;

      if (! ignoreFil && swordP[parentP->svX].fillerTag == 1 && parentP->nodeX >=0) {
	linkP = linkP->txenP;
	continue;
      }
      if (parentNA[nodeX] % 5 ==0) {
	if (NULL ==(parentPLA[nodeX] = 
		    realloc(parentPLA[nodeX],(6+parentNA[nodeX])*sizeof(GNode*)))) {
	  ERROR("glatGetRealParents: allocation failure\n");
	  return TCL_ERROR;
	}
      }
      parentPLA[nodeX][parentNA[nodeX]++] = parentP;
      linkP = linkP->txenP;
    }

    /* second loop over filler parents */
    linkP = nodeP->parentP;
    while (linkP) {
      GNode* parentP = linkP->parentP;
      int    parentX =0;

      if (ignoreFil || swordP[parentP->svX].fillerTag == 0 ||parentP->nodeX <0) {
	linkP = linkP->txenP;
	continue;
      }

      /* loop over each real parent */
      for (parentX=0;parentX<parentNA[parentP->nodeX];parentX++) {
	GNode* rparentP = parentPLA[parentP->nodeX][parentX];
	int         myX = 0;

	/* check if have that parent already */
	while (myX <parentNA[nodeX] && rparentP != parentPLA[nodeX][myX]) myX++;
	if (myX < parentNA[nodeX]) continue;
	
	if (parentNA[nodeX] % 5 ==0) {
	  if (NULL ==(parentPLA[nodeX] = 
		      realloc(parentPLA[nodeX],(6+parentNA[nodeX])*sizeof(GNode*)))) {
	    ERROR("glatGetRealParents: allocation failure\n");
	    return TCL_ERROR;
	  }
	}
	parentPLA[nodeX][parentNA[nodeX]++] = rparentP;
      }
      linkP = linkP->txenP;
    }

    /* ugly hack to incorporate final node */
    if (nodeX == -2)            nodeX = latP->nodeN;
    if (nodeX == latP->nodeN-1) nodeX = -3;

  }

  return TCL_OK;
}

/* ------------------------------------------------------------------------
    glatAlign
   ------------------------------------------------------------------------ */

static int glatAlignPrint (char* result, GNode* nodeP, char* refW, char* hypW, int verbose)
{
  if (verbose > 0) itfAppendResult
		     ("[%s] \t%s \t%s", result, refW, hypW);
  if (verbose > 1) itfAppendResult
		     (" \t%d", nodeP->nodeX);
  if (verbose > 2) itfAppendResult
		     (" \tframeS= %d  frameE= %d", nodeP->frameS, nodeP->frameE);
  if (verbose > 0) itfAppendResult ("\n");
  return TCL_OK;
}

static int glatAlign (GLat* latP, int svN, SVX* svxA, int verbose, int ignoreFil,
		      int* corN, int* delN, int* insN, int* subN, GBP*** gbptr)
{
  int       hypN   = latP->nodeN+1;
  int       refN   = svN+1;
  SWord*  swordP   = latP->svmapP->svocabP->list.itemA;
  GBP**      gbA   = NULL;
  GBP*       gbP   = NULL;
  GNode*   nodeP   = NULL;
  GLink*   linkP   = NULL;

  GNode**   parentPL = NULL;
  GNode*** parentPLA = NULL;
  int*      parentNA = NULL;

  float finalscore;
  int refX,hypX,parentX,parentN;

  *corN = 0;
  *insN = 0;
  *delN = 0;
  *subN = 0;

  if (latP->status == LAT_CREATE) {
    WARN("glatAlign: forced purifying\n");
    glatPurify(latP);
    hypN = latP->nodeN+1;
  }

  if (latP->end.frameX == 0) {
    glatFinalize(latP);
  }
  if (latP->end.frameX == -1) {
    ERROR("glatAlign: Couldn't find final node\n");
    return TCL_ERROR;
  }

  if (latP->nodeN == 0 || latP->linkN == 0) {
    ERROR("glatAlign: no lattice (aetsch)\n");
    return TCL_OK;
  }
  if (svN < 1) {
    ERROR("glatAlign: nothing to align\n");
    return TCL_OK;
  }

  /* Allocate memory for real parents for each node */
  if (NULL == (parentPLA = malloc((latP->nodeN+2)*sizeof(GNode**)))) {
    ERROR("glatAlign: allocation failure\n");
    return TCL_ERROR;
  }
  if (NULL == (parentNA = malloc((latP->nodeN+2)*sizeof(int)))) {
    ERROR("glatAlign: allocation failure\n");
    return TCL_ERROR;
  }
  for (parentX=0;parentX<latP->nodeN+2;parentX++) {
    parentPLA[parentX]= NULL;
    parentNA[parentX] = 0;
  }
  parentPLA +=2;
  parentNA  +=2;
  glatGetRealParents(latP,parentPLA,parentNA,ignoreFil);

  /* allocate memory for confusion matrix */
  if (NULL == (gbA = malloc(refN*sizeof(GBP*)))) {
    ERROR("glatAlign: allocation failure\n");
    return TCL_ERROR;
  }
  if (NULL == (gbA[0] = malloc(refN*hypN*sizeof(GBP)))) {
    ERROR("glatAlign: allocation failure\n");
    return TCL_ERROR;
  }
  for (refX=1;refX<refN;refX++)
    gbA[refX] = gbA[refX-1]+hypN;

  if (gbptr) *gbptr = gbA;

  refX=0;
  for (hypX=0;hypX<hypN;hypX++) {
    gbA[refX][hypX].score = hypX*latP->align.delCost;
    gbA[refX][hypX].hypX  = -1;
    gbA[refX][hypX].refX  = -1;  
  }

  hypX=0;
  for (refX=0;refX<refN;refX++) {
    gbA[refX][hypX].score = refX*latP->align.insCost;
    gbA[refX][hypX].hypX  = -1;
    gbA[refX][hypX].refX  = -1;
  }

  for (refX=1;refX<refN;refX++) {
    for (hypX=1;hypX<hypN;hypX++) {
      gbA[refX][hypX].score = SCORE_MAX;
      gbA[refX][hypX].hypX  = -1;
      gbA[refX][hypX].refX  = -1;
    }
  }

  for (refX=1;refX<refN;refX++) {
    SVX svX = svxA[refX-1];
    for (hypX=1;hypX<hypN;hypX++) {
      int    bestParentX = -1;
      int    bestParentY = -1;
      float  best    = SCORE_MAX;
      float  score   = SCORE_MAX;
      float  subCost = SCORE_MAX;
      float  delCost = SCORE_MAX;
      float  insCost = SCORE_MAX;

      nodeP = latP->mapping.x2n[hypX-1];
      gbP   = gbA[refX]+nodeP->nodeX+1;

      /* no transitions into filler words */
      if (! ignoreFil && svX != SVX_NIL && swordP[nodeP->svX].fillerTag == 1 && swordP[svX].fillerTag == 0) {
	gbA[refX][hypX].score = SCORE_MAX;
	continue;
      }

      /* get real parents */
      parentN  = parentNA [nodeP->nodeX];
      parentPL = parentPLA[nodeP->nodeX];

      assert(parentN<latP->nodeN);

      /* --------------------------------------------------- */
      /* case 1 : ref and hypo do match                      */
      /* --------------------------------------------------- */
      if (svX == nodeP->svX) {
	if (parentN == 0) {	    
	  if (nodeP->parentP && nodeP->parentP->parentP->nodeX == -1) {
	    best = 0;
	    bestParentX = 0;
	  } else {
	    WARN("glatAlign: found node without parents: nodeX= %d  svX= %d  frameS= %d  frameE= %d\n",
		 nodeP->nodeX,nodeP->svX,nodeP->frameS,nodeP->frameE);
	    best        = SCORE_MAX;
	    bestParentX = 0;
	  }
	}
	for (parentX=0;parentX<parentN;parentX++) {
	  score = gbA[refX-1][parentPL[parentX]->nodeX+1].score;
	  if (score < best) {
	    best        = score;
	    bestParentX = parentPL[parentX]->nodeX+1;
	  }
	}
	if (best < gbP->score) {
	  gbP->score = best;
	  gbP->refX  = refX-1;
	  gbP->hypX  = bestParentX;
	}
	continue;
      }
      
      /* --------------------------------------------------- */
      /* compute substition cost                             */
      /* --------------------------------------------------- */
      best  = SCORE_MAX;
      for (parentX=0;parentX<parentN;parentX++) {
	score = gbA[refX-1][parentPL[parentX]->nodeX+1].score;
	if (score < best) {
	  best        = score;
	  bestParentX = parentPL[parentX]->nodeX+1;
	}
      }
      subCost = best + latP->align.subCost;
      
      /* --------------------------------------------------- */
      /* compute deletion cost                               */
      /* --------------------------------------------------- */
      best  = SCORE_MAX; 
      for (parentX=0;parentX<parentN;parentX++) {
	score = gbA[refX][parentPL[parentX]->nodeX+1].score;
	if (score < best) {
	  best        = score;
	  bestParentY = parentPL[parentX]->nodeX+1;
	}
      }
      delCost = best + latP->align.delCost;
      
      /* --------------------------------------------------- */
      /* compute insertion cost                              */
      /* --------------------------------------------------- */
      insCost = gbA[refX-1][hypX].score + latP->align.insCost;
      
      /* --------------------------------------------------- */
      /* fill costmatrix                                     */
      /* --------------------------------------------------- */
      if (subCost < delCost) {
	if (subCost < insCost) {
	  /* substition */
	  if (subCost < gbP->score) {
	    gbP->score = subCost;
	    gbP->refX  = refX-1;
	    gbP->hypX  = bestParentX;
	  }
	} else {
	  /* insertion */
	  if (insCost <gbP->score) {
	    gbP->score = insCost;
	    gbP->refX  = refX-1;
	    gbP->hypX  = hypX;
	  }
	}
      } else {
	if (delCost < insCost) {
	  /* deletion */
	  if (delCost < gbP->score) {
	    gbP->score = delCost;
	    gbP->refX  = refX;
	    gbP->hypX  = bestParentY;
	  }
	} else {
	  /* insertion */
	  if (insCost < gbP->score) {
	    gbP->score = insCost;
	    gbP->refX  = refX-1;
	    gbP->hypX  = hypX;
	  }
	}
      }
    } /* for each hypX */
  } /* for each refX */

  /* --------------------------------------------------- */
  /* backtrace                                           */
  /* --------------------------------------------------- */
  finalscore = SCORE_MAX;
  linkP = latP->endP->parentP;
  refX  = refN-1;
  while (linkP) {
    float score = gbA[refX][linkP->parentP->nodeX+1].score;
    if (score < finalscore) {
      finalscore = score;
      hypX       = linkP->parentP->nodeX+1;
    }
    linkP = linkP->txenP;
  }

  if (finalscore >= SCORE_MAX) {
    ERROR("glatAlign: couldn't trace back\n");
    refX=-1;
  }

  while (hypX >= 1 || refX >= 1) {
    char* refWord = (svxA[refX-1] != SVX_NIL) ? swordP[svxA[refX-1]].name : "(NULL)";
    
    gbP   = gbA[refX]+hypX;
    nodeP = latP->mapping.x2n[hypX-1];    
    if (gbP->refX < refX && gbP->hypX < hypX) {
      if (svxA[refX-1] == nodeP->svX) {
	(*corN)++;
	glatAlignPrint ("COR", nodeP, refWord, swordP[nodeP->svX].name, verbose);
      } else {
	(*subN)++;
	glatAlignPrint ("SUB", nodeP, refWord, swordP[nodeP->svX].name, verbose);
      }
    }
    if (gbP->refX == refX && gbP->hypX < hypX) {
      (*delN)++;
      glatAlignPrint ("INS", nodeP, "(NULL)", swordP[nodeP->svX].name, verbose);
    }
    if (gbP->refX < refX && gbP->hypX == hypX) {
      (*insN)++;
      glatAlignPrint ("DEL", nodeP, refWord, "(NULL)", verbose);
    }
    refX = gbP->refX;
    hypX = gbP->hypX;
  }

  if (gbA && !gbptr) {
    if (gbA[0]) free(gbA[0]);
    free(gbA);
  }

  parentPLA -=2;
  parentNA  -=2;

  if (parentPLA) {
    for (parentX=0;parentX<latP->nodeN+2;parentX++)
      if (parentPLA[parentX]) free(parentPLA[parentX]);
    free(parentPLA);
  }
  if (parentNA)
    free(parentNA);

  return TCL_OK;
}


static int glatAlignItf (ClientData cd, int argc, char *argv[])
{
  GLat*    latP = (GLat*) cd;
  SVMap* svmapP = latP->svmapP;
  int        ac = argc-1;
  SVX*     svxA = NULL;
  LVX       lvX = LVX_NIL;
  int      refN = 0;
  char*     ref = NULL;
  int   verbose = 0;
  int ignoreFil = 0;
  char** refv;
  int corN= 0, insN= 0, delN= 0, subN= 0;
  int refX;

  if ( itfParseArgv( argv[0], &ac, argv+1, 0,
      "<ref>",       ARGV_STRING, NULL, &ref,      NULL, "sequence of words",    	     
      "-ignoreFtag", ARGV_INT,    NULL, &ignoreFil,NULL, "treat filler words as regular words",		     
      "-v",          ARGV_INT,    NULL, &verbose,  NULL, "verbose",		     
       NULL) != TCL_OK) return TCL_ERROR;

  if (latP->nodeN+latP->linkN < 2) {
    ERROR("glatAlignItf: Empty Lattice\n");
    return TCL_ERROR;
  }

  if (Tcl_SplitList( itf,ref, &refN, &refv) == TCL_ERROR || refN < 1) {
    ERROR("glatAlignItf: cannot split sequence %s\n",ref);
    return TCL_ERROR;
  }

  if (NULL == (svxA = malloc(refN*sizeof(SVX)))) {
    ERROR("glatAlignItf: allocation error\n");
    return TCL_ERROR;
  }
  for (refX = 0;refX<refN;refX++) {
    int svX = listIndex((List*) &(latP->svmapP->svocabP->list),refv[refX],0);
    
    if (svX < 0) {
      WARN("glatAlignItf: %s not in vocabulary\n",refv[refX]);
      svxA[refX] = SVX_NIL;
    } else {
      svxA[refX]=svX;
    }
  }
  lvX = svmapGet(svmapP,svxA[0]);
  if (lvX != svmapP->idx.start) {
    ERROR("glatAlign: no begin of sentence '%s' at begin of path\n",
	  svmapP->string.start);
    return TCL_ERROR;
  }
  lvX = svmapGet(svmapP,svxA[refN-1]);
  if (lvX != latP->svmapP->idx.end) {
    ERROR("glatAlign: no end of sentence '%s' at end of path\n",
	  svmapP->string.end);
    return TCL_ERROR;
  }

  glatAlign (latP, refN, svxA, verbose, ignoreFil, &corN, &delN, &insN, &subN, NULL);
  itfAppendResult ("corN= %d  subN= %d  insN= %d  delN= %d\n",
		   corN, subN, insN, delN);

  Tcl_Free((char*) refv);
  free(svxA);
  
  return TCL_OK;
}

/* ------------------------------------------------------------------------
    glatMap
   ------------------------------------------------------------------------ */

static int glatMap(GLat* latP, SVMap* svmapP) 
{
  int nodeX;

  if (latP->status == LAT_CREATE) {
    WARN("glatMap: forced purifying\n");
    glatPurify(latP);
  }

  for (nodeX=0;nodeX<latP->nodeM;nodeX++) {
    SVX svX = latP->mapping.x2n[nodeX]->svX;
    if (svX != SVX_NIL) {
      SVX svY = (SVX) svmapGet(svmapP,svX);
      if (svY != SVX_NIL) 
	latP->mapping.x2n[nodeX]->svX = (SVX) svmapGet(svmapP,svX);
      else
	WARN("glatMap: Couldn't map %s\n",svmapP->svocabP->list.itemA[svX].name);
    } else {
      WARN("glatMap: Couldn't map node %d\n",nodeX);
    }
  }
  return TCL_OK;
}


static int glatMapItf (ClientData cd, int argc, char *argv[])
{
  GLat*    latP = (GLat*) cd;
  int        ac = argc-1;
  SVMap* svmapP = latP->svmapP;

  if ( itfParseArgv( argv[0], &ac, argv+1, 0,
      "-map", ARGV_OBJECT, NULL, &svmapP, "SVMap", "Vocabulary Mapper",
       NULL) != TCL_OK) return TCL_ERROR;

  if (latP->nodeN+latP->linkN < 2) {
    ERROR("glatMapItf: Empty Lattice\n");
    return TCL_ERROR;
  }

  if (!svmapP) {
    ERROR("glatMapItf: no Vocabulary Mapper\n");
    return TCL_ERROR;
  }

  glatMap(latP,svmapP);
  
  return TCL_OK;
}


/* ------------------------------------------------------------------------
    glatSplit
   -----------
    Can be used to split nodes which contain MultiWords (phraseLM)
   ------------------------------------------------------------------------ */

static int glatSplit (GLat* latP, SVMap* svmapP) 
{
  PhraseLM*     lmP = svmapP->lksP->data.phraseLM;
  Dictionary* dictP = svmapP->svocabP->dictP;
  int         nodeN = latP->nodeN;
  int         nodeX;

  if (latP->status == LAT_CREATE) {
    WARN("glatSplit: forced purifying\n");
    glatPurify(latP);
    nodeN = latP->nodeN;
  }

  /* Purified, so we know all these nodes exist! */
  for (nodeX = 0; nodeX < nodeN; nodeX++) {
    GNode*     nodeP = latP->mapping.x2n[nodeX];
    GLink*     linkP = nodeP->childP;
    float      score = nodeP->score + svmapP->lz * svmapP->lvxSA[nodeP->svX];
    int       frameS = nodeP->frameS;
    int       frameE = nodeP->frameE;
    int      framesN = frameE - frameS;
    LVX          lvX = svmapGet (svmapP, nodeP->svX);
    int        nodeY=0, nodeZ=0, nodeT=0, i=0, l=0, pX=0, idx=0, phonesN=0;
    float        fpp;

    /* No multi-word => do nothing */
    if (!framesN || lvX == LVX_NIL || (idx = lvX - lmP->baseN) < 0 ||
	(l = lmP->list.itemA[idx].lnxN) == 1)
      continue;

    /* We want to have the total length in framesN and the
       number of phones in phonesN to split it correctly */
    for (i = 0, phonesN = 0; i < l; i++) {
      SVX svX  = lmP->list.itemA[idx].lnxA[i];
      phonesN += dictP->list.itemA[svmapP->svocabP->list.itemA[svX].dictX].word.itemN;
      score   -= svmapP->lz * svmapP->lvxSA[svX]; /* Correct for pron probs */
      pX       = (i) ? pX : phonesN;
    }
    fpp  = 1.0*framesN/phonesN; /* frames per phone */

    /* Fix nodeX */
    nodeP->svX     = lmP->list.itemA[idx].lnxA[0];
    nodeP->score   = score*pX/phonesN;
    nodeP->frameE  = frameS+fpp*pX-1;

    /* ----------------------------------------------
         Create the single nodes, nodeY will be the
	 first one created, nodeZ the last one
       ---------------------------------------------- */
    for (i = 1, nodeT = nodeX, nodeY = -1; i < l; i++) {
      SVX svX = lmP->list.itemA[idx].lnxA[i];
      int  pL = dictP->list.itemA[svmapP->svocabP->list.itemA[svX].dictX].word.itemN;

      /* Add the node and link it to its predecessor */
      nodeZ   = glatCreateNode (latP, lmP->list.itemA[idx].lnxA[i],
				frameS+fpp*pX, frameS+fpp*(pX+pL)-1);
      if (nodeY == -1) { /* The first node */
	nodeY = nodeZ;
      } else { /* 2nd, 3rd and so on, link it to before */
	glatCreateLink (latP, latP->mapping.x2n[nodeT], latP->mapping.x2n[nodeZ], 0.0);
      }
      latP->mapping.x2n[nodeZ]->score = score*pL/phonesN;

      /* This is not quite kosher
      latP->mapping.x2n[nodeZ]->alpha = nodeP->alpha;
      latP->mapping.x2n[nodeZ]->beta  = nodeP->beta; */
      latP->mapping.x2n[nodeZ]->gamma = nodeP->gamma;

      /* Bookkeeping */
      pX    += pL;
      nodeT  = nodeZ;
    }

    /* -----------------------------------------------
         Add the links from nodeX into its followers
	 to nodeZ and delete the original one
       ----------------------------------------------- */
    while (linkP) {
      GLink* tmpP = linkP->nextP;
      if (linkP->childP->nodeX == nodeY) {
	/* This should never happen, because nodeX and nodeY are not yet linked */
	WARN ("nodeX = %d == nodeY = %d\n", nodeX, nodeY);
	linkP = tmpP;
	continue;
      }
      glatCreateLink (latP, latP->mapping.x2n[nodeZ], linkP->childP, linkP->score);
      glatFreeLink   (latP, linkP);
      linkP = tmpP;
    }

    /* Link the first node */
    glatCreateLink (latP, latP->mapping.x2n[nodeX], latP->mapping.x2n[nodeY], 0.0);

    /* Move the RCM's to our new end node */
    latP->mapping.x2n[nodeZ]->rcm.N = nodeP->rcm.N;
    latP->mapping.x2n[nodeZ]->rcm.A = nodeP->rcm.A;
    latP->mapping.x2n[nodeZ]->rcm.X = nodeP->rcm.X;
    nodeP->rcm.N = 0;
    nodeP->rcm.A = NULL;
    nodeP->rcm.X = NULL;

    /* Make sure that the end frame is correct */
    latP->mapping.x2n[nodeZ]->frameE = frameE;
  }

  /* Make sure the ordering is ok */
  latP->status = LAT_CREATE;
  glatPurify (latP);

  return TCL_OK;
}


static int glatSplitItf (ClientData cd, int argc, char *argv[])
{
  GLat*    latP = (GLat*) cd;
  int        ac = argc-1;
  SVMap* svmapP = latP->svmapP;

  if (itfParseArgv (argv[0], &ac, argv+1, 0,
      "-map", ARGV_OBJECT, NULL, &svmapP, "SVMap", "Vocabulary Mapper",
       NULL) != TCL_OK) return TCL_ERROR;

  if (latP->nodeN+latP->linkN < 2) {
    WARN ("glatSplitItf: empty Lattice\n");
    return TCL_OK;
  } else if (!svmapP) {
    ERROR ("glatSplitItf: no Vocabulary Mapper\n");
    return TCL_ERROR;
  } else if (svmapP->lksP->type != LingKS_PhraseLM) {
    ERROR ("glatSplitItf: no PhraseLM\n");
    return TCL_ERROR;
  }

  glatSplit (latP, svmapP);
  
  return TCL_OK;
}


/* ------------------------------------------------------------------------
     glatWarp
   ------------
     Warp (scale) the time axis of the GLat
   ------------------------------------------------------------------------ */

static int glatWarp (GLat* latP, float factor, int scores)
{
  int nodeX;

  for (nodeX = -2; nodeX < latP->nodeM; nodeX++) {
    GNode* nodeP;
    if ((nodeP = latP->mapping.x2n[nodeX])) {
      if (scores) {
	/* Is not necessary
	GLink* linkP = nodeP->childP;
	for ( ; linkP; linkP = linkP->nextP)
	linkP->score *= factor; */
	nodeP->score *= factor;
      }
      nodeP->frameS = 0.5 + factor * nodeP->frameS;
      nodeP->frameE = 0.5 + factor * nodeP->frameE;
    }
  }

  latP->end.frameX  = 0.5 + factor * latP->end.frameX;
  latP->frameShift /= factor;

  return latP->end.frameX;
}

static int glatWarpItf (ClientData cd, int argc, char *argv[])
{
  GLat*    latP = (GLat*) cd;
  int        ac = argc-1;

  float   shift = 0.0;
  float  factor = 1.0;
  int    frameN = 0;
  int    scores = 0;

  if (itfParseArgv (argv[0], &ac, argv+1, 0,
      "-shift",  ARGV_FLOAT, NULL, &shift,  "NULL", "frame shift after warping",
      "-factor", ARGV_FLOAT, NULL, &factor, "NULL", "relative scaling factor",
      "-frameN", ARGV_INT,   NULL, &frameN, "NULL", "number of frames after warping",
      "-scores", ARGV_INT,   NULL, &scores, "NULL", "scale scores",
      NULL) != TCL_OK) return TCL_ERROR;

  if (latP->nodeN + latP->linkN < 2 || latP->end.frameX < 1 || latP->frameShift <= 0.0) {
    WARN ("glatWarpItf: empty Lattice.\n");
    return TCL_OK;
  }

  if      (frameN)      factor = (float)latP->end.frameX / (float)frameN;
  else if (shift > 0.0) factor =        latP->frameShift /         shift;

  itfAppendElement ("%d", glatWarp (latP, factor, scores));

  return TCL_OK;
}


/* ========================================================================
    GLAT Compact functions
   ======================================================================== */

/* ------------------------------------------------------------------------
    merge two nodes
   ------------------------------------------------------------------------ */

static void gnodeMerge(GLat* latP, GNode* node1P, GNode* node2P)
{
  GLink* linkP = node2P->childP;

  /* make node2P' children to node1P' children */
  while (linkP) {  
    GLink*  link1P = node1P->childP;
    GLink*  nlinkP = linkP->nextP;
    GNode*  childP = linkP->childP;
    GLink* parentP = childP->parentP;

    /* child is already deleted */
    assert (childP->status != DFS_CLEAR);
    if (childP->status == DFS_CLEAR) {      
      linkP = nlinkP;
      continue;
    }

    /* check if we have this child already at node1P */
    while (link1P && link1P->childP != childP) 
      link1P=link1P->nextP;

    if (! link1P) {
      /* move parent node2P to node1P for this link */
      linkP->parentP = node1P;
      
      /* add childP to node1P's list of children */
      linkP->nextP   = node1P->childP;
      node1P->childP = linkP;

    }  else {
      /* remove node2P from childP's list of parents */
      if (parentP == linkP) {
	childP->parentP = parentP->txenP;
      } else {
	while (parentP->txenP && parentP->txenP != linkP) 
	  parentP = parentP->txenP;
	assert(parentP->txenP == linkP);
	parentP->txenP = parentP->txenP->txenP;	
      }
      
      /* delete link if node1 had already a link to that child */ 
      bmemDelete(latP->mem.link,linkP);
      latP->linkN--;
    }

    linkP = nlinkP;
  }

  /* make node2P' parents to node1P' parents */
  linkP = node2P->parentP;

  while (linkP) {  
    GLink*  link1P = node1P->parentP;
    GLink*  nlinkP = linkP->txenP;
    GNode* parentP = linkP->parentP;
    GLink*  childP = parentP->childP;

    /* parent is already deleted */
    assert(parentP->status != DFS_CLEAR);
    if (parentP->status == DFS_CLEAR) {
      linkP = nlinkP;
      continue;
    }

    /* check if we have this parent already at node1P */
    while (link1P && link1P->parentP != parentP) 
      link1P=link1P->txenP;
    
    if (! link1P) {
      /* move parent node2P to node1P for this link */
      linkP->childP = node1P;
      
      /* add parentP to node1P's list of parents  */
      linkP->txenP     = node1P->parentP;
      node1P->parentP  = linkP;
    } else {   

      /* remove node2P from parentP's list of children */
      if (childP == linkP) {
	parentP->childP = childP->nextP;
      } else {
	while (childP->nextP && childP->nextP != linkP) 
	  childP = childP->nextP;
	assert(childP->nextP == linkP);
	childP->nextP = childP->nextP->nextP;	
      }
   
      /* delete linkP if node1 had already that parent */
      bmemDelete(latP->mem.link,linkP);
      latP->linkN--;
    }
    linkP = nlinkP;
  }

  /* delete node2P */
  node2P->status  = DFS_CLEAR;
  node2P->parentP = NULL;
  node2P->childP  = NULL;

  latP->nodeN--;
  return;
}

/* ------------------------------------------------------------------------
    check if two nodes can be merged
   ------------------------------------------------------------------------ */

static int gnodeCheckMerge(GNode* node1P, GNode* node2P)
{
  GLink* linkP = node2P->childP;
  
  if (node1P->svX != node2P->svX) return 0;

  /* node1 is already or cannot be deleted */
  if (node1P->status == DFS_CLEAR || node1P->nodeX < 0)
    return 0;

  /* node2 is already or cannot be deleted */
  if (node2P->status == DFS_CLEAR || node2P->nodeX < 0)
    return 0;

  /* check end frame for all parents from node2P */
  linkP = node2P->parentP;
  while (linkP) { 
    if (linkP->parentP->frameE >= node1P->frameS) return 0;
    linkP=linkP->txenP;
  }

  /* check start frame for all children of node2P */
  linkP = node2P->childP;
  while (linkP) { 
    if (linkP->childP->frameS <= node1P->frameE) return 0;
    linkP=linkP->nextP;
  }
  return 1;
}

/* ------------------------------------------------------------------------
    merge children with different phonetic and time context
   ------------------------------------------------------------------------ */

static int glatMergeChildren(GLat* latP, int adjust)
{
  int   nodeX=0, frameX=0;
  GNode  *nodeP = NULL;
  GLink  *linkP = NULL;

  if (latP->status == LAT_CREATE) {
    WARN("glatMergeChildren: forced purifying\n");
    glatPurify(latP);
  }
  if (latP->end.frameX == 0) {
    glatFinalize(latP);
  }
  if (latP->end.frameX == -1) {
    ERROR("glatMergeChildren: Couldn't find final node\n");
    return TCL_ERROR;
  }

  for (nodeX = -2; nodeX < latP->nodeM; nodeX++) {
    nodeP = latP->mapping.x2n[nodeX];
    if (nodeP) nodeP->status = DFS_INIT;
  }
  
  for (nodeX = -1; nodeX < latP->nodeM; nodeX++) {
    nodeP = latP->mapping.x2n[nodeX];
    
    if (!nodeP || nodeP->status == DFS_CLEAR) continue;

    linkP = nodeP->childP;
    while (linkP) {
      GLink* link2P  = linkP->nextP;
      
      if (linkP->childP->status == DFS_INIT) {
	while (link2P) {
	  GLink* nlinkP = link2P->nextP;
	  
	  if (gnodeCheckMerge(linkP->childP,link2P->childP))
	    gnodeMerge(latP,linkP->childP,link2P->childP);
	  else if (gnodeCheckMerge(link2P->childP,linkP->childP))
	    gnodeMerge(latP,link2P->childP,linkP->childP);

	  link2P = nlinkP;
	}
      }

      linkP = linkP->nextP;
    }
  }

  /* delete merged nodes */
  for (nodeX = 0; nodeX < latP->nodeM; nodeX++) {
    nodeP = latP->mapping.x2n[nodeX];
    assert (nodeP); // XCode says could be NULL
    if (nodeP->status == DFS_CLEAR) {
      latP->mapping.x2n[nodeP->nodeX] = NULL;
      bmemDelete(latP->mem.node,nodeP);
    }
  }

  /* re-count lattice nodes */
  for (nodeX=-2;nodeX<latP->nodeM;nodeX++)
    if (latP->mapping.x2n[nodeX])
      latP->mapping.x2n[nodeX]->status = DFS_INIT;

  glatCount(latP);

  latP->startP->status = DFS_INIT;
  for (nodeX=0;nodeX<latP->nodeN;nodeX++)
    latP->mapping.x2n[nodeX]->status = DFS_INIT;

  if (! adjust) return TCL_OK;

  /* check start frames */
  for (nodeX=0;nodeX<latP->nodeN;nodeX++) {
    nodeP = latP->mapping.x2n[nodeX];
    linkP = nodeP->parentP;

    frameX = -1;
    while (linkP) {
      if (linkP->parentP->frameE > frameX) 
	frameX = linkP->parentP->frameE;
      linkP = linkP->txenP;
    }
    nodeP->frameS = frameX+1;
  }

  /* check end frames */
  for (nodeX=latP->nodeN-1;nodeX>0;nodeX--) {
    nodeP = latP->mapping.x2n[nodeX];
    linkP = nodeP->childP;

    frameX = INT_MAX;
    while (linkP) {
      if (linkP->childP->frameS < frameX) 
	frameX = linkP->childP->frameS;
      linkP = linkP->nextP;
    }
    nodeP->frameE = frameX-1;
  }

  return TCL_OK;
}

/* ------------------------------------------------------------------------
    merge nodes with different parents
   ------------------------------------------------------------------------ */

static int glatMergeAll(GLat* latP, int adjust)
{
  int   nodeX=0,nodeY=0, frameX=0;
  GNode  *nodeP = NULL;
  GLink  *linkP = NULL;

  if (latP->status == LAT_CREATE) {
    WARN("glatMergeAll: forced purifying\n");
    glatPurify(latP);
  }
  if (latP->end.frameX == 0) {
    glatFinalize(latP);
  }
  if (latP->end.frameX == -1) {
    ERROR("glatMergeAll: Couldn't find final node\n");
    return TCL_ERROR;
  }

  for (nodeX = -2; nodeX < latP->nodeN; nodeX++) {
    nodeP = latP->mapping.x2n[nodeX];
    nodeP->status = DFS_INIT;
  }
  
  for (nodeX = 0; nodeX < latP->nodeN; nodeX++) {
    nodeP = latP->mapping.x2n[nodeX];
    
    if (nodeP->status == DFS_CLEAR) continue;

     for (nodeY = nodeX+1; nodeY < latP->nodeN; nodeY++) {
       GNode* node2P = latP->mapping.x2n[nodeY];

       if (gnodeCheckMerge(nodeP,node2P)) 
	 gnodeMerge(latP,nodeP,node2P);
       else if (gnodeCheckMerge(node2P,nodeP)) 
	 gnodeMerge(latP,node2P,nodeP);

     }
  }

  /* delete merged nodes */
  for (nodeX = 0; nodeX < latP->nodeM; nodeX++) {
    nodeP = latP->mapping.x2n[nodeX];
    if (nodeP->status == DFS_CLEAR) {
      latP->mapping.x2n[nodeP->nodeX] = NULL;
      bmemDelete(latP->mem.node,nodeP);
    } else {
      nodeP->status = DFS_INIT;
    }
  }

  /* re-count lattice nodes */
  glatCount(latP);

  latP->startP->status = DFS_INIT;
  for (nodeX=-2;nodeX<latP->nodeN;nodeX++)
    latP->mapping.x2n[nodeX]->status = DFS_INIT;

  if (! adjust) return TCL_OK;

  /* check start frames */
  for (nodeX=0;nodeX<latP->nodeN;nodeX++) {
    nodeP = latP->mapping.x2n[nodeX];
    linkP = nodeP->parentP;

    frameX = -1;
    while (linkP) {
      if (linkP->parentP->frameE > frameX) 
	frameX = linkP->parentP->frameE;
      linkP = linkP->txenP;
    }
    nodeP->frameS = frameX+1;
  }

  /* check end frames */
  for (nodeX=latP->nodeN-1;nodeX>0;nodeX--) {
    nodeP = latP->mapping.x2n[nodeX];
    linkP = nodeP->childP;

    frameX = INT_MAX;
    while (linkP) {
      if (linkP->childP->frameS < frameX) 
	frameX = linkP->childP->frameS;
      linkP = linkP->nextP;
    }
    nodeP->frameE = frameX-1;
  }

  return TCL_OK;
}

/* ------------------------------------------------------------------------
    remove filler words
   ------------------------------------------------------------------------ */

static int glatRemoveFiller(GLat* latP)
{
  int   nodeX=0;
  SWord  *swordP = latP->svmapP->svocabP->list.itemA;
  GNode   *nodeP = NULL;
  GLink   *linkP = NULL;

  if (latP->status == LAT_CREATE) {
    WARN("glatRemoveFiller: forced purifying\n");
    glatPurify(latP);
  }
  if (latP->end.frameX == 0) {
    glatFinalize(latP);
  }
  if (latP->end.frameX == -1) {
    ERROR("glatRemoveFiller: Couldn't find final node\n");
    return TCL_ERROR;
  }
  for (nodeX = 0; nodeX < latP->nodeN; nodeX++) {
    nodeP = latP->mapping.x2n[nodeX];
    
    if (!nodeP)                         continue;
    if (! swordP[nodeP->svX].fillerTag) continue;

    /* direct connect parents to children */
    linkP = nodeP->parentP;
    while (linkP) {
      GNode* parentP = linkP->parentP;
      GLink*  link2P = nodeP->childP;
      while (link2P) {
	GNode* childP = link2P->childP;
	glatCreateLink(latP,parentP,childP,0);
	link2P = link2P->nextP;
      }
      linkP = linkP->txenP;
    }

    /* delete node */
    glatFreeNode(latP,nodeP);
  }

  for (nodeX=0;nodeX<latP->nodeM;nodeX++)
    if (latP->mapping.x2n[nodeX])
      latP->mapping.x2n[nodeX]->status = DFS_INIT;

  glatCount(latP);

  latP->startP->status = DFS_INIT;
  for (nodeX=0;nodeX<latP->nodeN;nodeX++)
    latP->mapping.x2n[nodeX]->status = DFS_INIT;

  return TCL_OK;
}

/* ------------------------------------------------------------------------
    glatCompact
   ------------------------------------------------------------------------ */

static int glatCompact(GLat* latP, int iter, int delFil, int ignoreLCT, 
		       int adjust)
{
  int i;
  for (i=0;i<iter;i++) {
    glatMergeChildren(latP,adjust);
    if (ignoreLCT)
      glatMergeAll(latP,adjust);
    if (delFil)
      glatRemoveFiller(latP);
  }
  return TCL_OK;
}

static int glatCompactItf (ClientData cd, int argc, char *argv[])
{
  GLat*    latP = (GLat*) cd;
  int        ac = argc-1;
  int    delFil = 1;
  int ignoreLCT = 1;
  int      iter = 3;
  int    adjust = 1;

  if ( itfParseArgv( argv[0], &ac, argv+1, 0,
       "-iter",       ARGV_INT, NULL, &iter,      NULL, "nr. of iterations",
       "-delFil",     ARGV_INT, NULL, &delFil,    NULL, "delete filler words",
       "-ignoreLCT",  ARGV_INT, NULL, &ignoreLCT, NULL, "ignore linguistic context",
       "-adjustTime", ARGV_INT, NULL, &adjust,    NULL, "adjust start and end points",
       NULL) != TCL_OK) return TCL_ERROR;

  return glatCompact(latP,iter,delFil,ignoreLCT,adjust);
}


/* ---------------------------------------------------------------------
     glatRecombine
   -----------------
     performs merging of nodes with equal LCT as during search
   --------------------------------------------------------------------- */

static int glatRecombine (GLat* latP, SVMap* svmapP, int c, int v)
{
  int    i, j;
  SWord* swordP;

  if (!svmapP) {
    ERROR ("glatRecombine: Need vocab mapper!\n");
    return TCL_ERROR;
  }
  swordP = svmapP->svocabP->list.itemA;  

  /* Generate x2n table */
  glatAssureMapTable (latP, latP->nodeN+2);
  if (!latP->mapping.x2n)
    glatCount (latP);

  /* Compute the language model states */
  glatComputeLCT (latP, svmapP);

  /* Create a sorted list of nodes */
  glatSortNodes (latP);

  for (i = 0; i < latP->nodeM; i++) {
    GNode* node1P = latP->mapping.x2n[i];

    /* Do not continue, if already deleted */
    if (!node1P || node1P->status == DFS_CLEAR || node1P->nodeX < 0)
      continue;

    for (j = 0; j < latP->sorter.startCntA[node1P->frameS]; j++) {
      GNode* node2P = latP->sorter.startPA[node1P->frameS][j];

      /* Do not merge, if already deleted or equal */
      if (node2P->status == DFS_CLEAR ||
	  node2P->nodeX  <  0         ||
	  node1P->nodeX  == node2P->nodeX)
	continue;
      
      /* Do not merge, if they differ (only check for A[0]) */
      if (node1P->frameS   != node2P->frameS   ||  node1P->svX   != node2P->svX   ||
	  node1P->frameE   != node2P->frameE   ||  node1P->lct.N != node2P->lct.N ||
	 !node1P->lct.N                        || !node2P->lct.N                  ||
	  node1P->lct.A[0] != node2P->lct.A[0])
	continue;
      
      if (v) printf ("(%d %d)", node1P->nodeX, node2P->nodeX);
      
      /* Merge the two nodes */
      gnodeMerge (latP, node1P, node2P);
    }
  }

  /* ----------------------------------------------------
       Connect only, if this node doesn't have children
       Also, check for context and so on
     ---------------------------------------------------- */
  if (c) for (i = 0; i < latP->nodeM; i++) {
    GNode*  node1P = latP->mapping.x2n[i];
    lmScore lscore = LMSCORE_MAX;
    int       bidx = i;
    float    score = 0.0;
    float   bscore = SCORE_MAX;

    /* Do not continue, if already deleted or has children */
    if (!node1P || node1P->status == DFS_CLEAR || node1P->nodeX < 0 || node1P->childP)
      continue;

    /* do not connect filler words or end of sentence */
    if (swordP[node1P->svX].fillerTag || svmapGet(svmapP,node1P->svX) == svmapP->idx.end) 
      continue;

    /* Do not link, if we don't have LM information */
    if (!node1P->lct.N)
      continue;

    /* Do not link, if already at EOS */
    if (node1P->frameE+1 == latP->sorter.frameN)
      continue;

    for (j = 0; j < latP->sorter.startCntA[node1P->frameE+1]; j++) {
      GNode* node2P = latP->sorter.startPA[node1P->frameE+1][j];
      LVX       lvX = svmapGet(svmapP,node2P->svX);
      /* Do not link, if already deleted */
      if (node2P->status == DFS_CLEAR || node2P->nodeX < 0)
	continue;
      
      /* keep end of sentence, but check for left context */
      if (lvX != svmapP->idx.end && ! glatLeftContextOk (latP, node1P, node2P))
	  continue;

      /* find the delta acoustic score for the right context model */
      score = glatGetRightContext (latP, node1P, node2P);

      /* calculate LM transition score */
      lscore = svmapGetLMScore (svmapP, node1P->lct.A[0], node2P->svX);
      if (score + svmapP->lz*lscore < bscore) {
	bscore = score + svmapP->lz*lscore;
	bidx   = node2P->nodeX;
      }
    }
    
    /* Add the link */
    if (bidx != i) {
      if (v) printf ("<%d %d>", i, bidx);
      glatCreateLink (latP, node1P, latP->mapping.x2n[bidx],
		       glatGetRightContext (latP, node1P, latP->mapping.x2n[bidx]));
    }
  }
  
  latP->status = LAT_CREATE;
  
  if (v) printf ("\n");

  return TCL_OK;
}

static int glatRecombineItf (ClientData cd, int argc, char *argv[])
{
  GLat*    latP = (GLat*) cd;
  SVMap* svmapP = latP->svmapP;
  int        ac = argc-1;
  int         v = 0;
  int         c = 0;

  if (itfParseArgv (argv[0], &ac, argv+1, 0,
       "-map",     ARGV_OBJECT, NULL, &svmapP, "SVMap", "mapper object",
       "-connect", ARGV_INT,    NULL, &c,      NULL,    "connect nodes",
       "-verbose", ARGV_INT,    NULL, &v,      NULL,    "verbosity",
       NULL) != TCL_OK) return TCL_ERROR;

  return glatRecombine(latP,svmapP,c,v);
}


/* ========================================================================
     Expand GLats with respect to LCTs
   ======================================================================== */

static int glatSingularLCT (GLat* newP, GLat* latP, SVMap* svmapP, int verbose)
{
  LingKS*     lksP = svmapP->lksP;
  SVocab*  svocabP = svmapP->svocabP;
  SWord*    swordP = svocabP->list.itemA;
  GNode**    newNA = NULL;
  int*       oldNA = NULL;
  int         lctN = 0;
  int      didWarn = 0;
  int node_, nodeX;

  /* Sanity check */
  if (!latP->linkN || !latP->mapping.x2n) {
    INFO ("glatSingularLCT: input lattice empty\n");
    return TCL_ERROR;
  }

  /* Prepare the new lattice */
  glatClear (newP);

  /* Compute the language model states */
  glatComputeLCT  (latP, svmapP);

  /* Compute the Betas (to check for unconnected nodes) */
  for (nodeX = 0; nodeX < latP->nodeM; nodeX++) 
    if (latP->mapping.x2n[nodeX]) 
      latP->mapping.x2n[nodeX]->status = DFS_INIT;

  gnodeComputeBeta (latP->startP, svmapP, latP->end.frameX, 1.0);

  /* Generate look-up table for LCTs */
  for (nodeX = -2; nodeX < latP->nodeM; nodeX++)
    if (latP->mapping.x2n[nodeX])
      lctN += latP->mapping.x2n[nodeX]->lct.N;
  oldNA  =    (int*) calloc (latP->nodeM+2, sizeof (int));
  newNA  = (GNode**) calloc (lctN,          sizeof (GNode*));
  oldNA += 2;
  for (nodeX = -2, lctN = 0; nodeX < latP->nodeM; nodeX++)
    if (latP->mapping.x2n[nodeX])
      oldNA[nodeX] = lctN, lctN += latP->mapping.x2n[nodeX]->lct.N;

  /* Simply create all nodes (just to be sure), link start and end node */
  for (nodeX = 0; nodeX < latP->nodeM; nodeX++) {
    SVX     x = latP->mapping.x2n[nodeX]->svX;
    int     s = latP->mapping.x2n[nodeX]->frameS;
    int     e = latP->mapping.x2n[nodeX]->frameE;
    if (x == SVX_NIL) 
      WARN ("glatSingularLCT: node %d (%d %d) has SVX_NIL\n", nodeX, s, e);
    for (node_ = 0; node_ < latP->mapping.x2n[nodeX]->lct.N; node_++) {
      int    tX = glatCreateNode (newP, x, s, e);
      GNode* nP = newNA[oldNA[nodeX]+node_] = newP->mapping.x2n[tX];
      nP->alpha = nP->beta = nP->gamma = 0.0;
    }
  }
  for (nodeX = oldNA[-2]; nodeX < oldNA[-1]; nodeX++)
    newNA[nodeX]   = newP->endP;
  newNA[oldNA[-1]] = newP->startP;

  /* Loop over all nodes */
  for (node_ = 0; node_ <= latP->nodeN; node_++) {
    int    nodeX = (node_ == latP->nodeN) ? -2 : node_;
    GNode* nodeP = latP->mapping.x2n[nodeX];
    GLink* linkP = nodeP->parentP;
    LCT*    lctA = nodeP->lct.A;
    LVX     lvxE = svmapGet (svmapP, nodeP->svX);
    int isFiller = (nodeP->svX == SVX_NIL) ? 1 : swordP[nodeP->svX].fillerTag;
    int    linkN = 0;
    int     lctX;
  
    /* Node not connected to the end */
    if (!nodeP || (nodeX != -2 && nodeP->status != DFS_BETA))
      continue;

    /* Loop over all parent nodes (and their LCTs),
       can they generate this LCT? */
    while (linkP) {
      LCT* lctB = linkP->parentP->lct.A;
      int  base = oldNA[linkP->parentP->nodeX];
      int  lctY;

      /* Loop over all LCTs of this parent */
      for (lctY = 0; lctY < linkP->parentP->lct.N; lctY++) {
	LCT linkLCT = (isFiller) ? lctB[lctY]
	            : lksP->extendLCT (lksP, lctB[lctY], lvxE);

	/* Loop over all LCTs of our node */
	for (lctX = 0; lctX < nodeP->lct.N; lctX++) {

	  /* Do we have a matching LCT? */
	  if (linkLCT == lctA[lctX] || (nodeX == -2 && newNA[base+lctY])) {
	    GNode *nP = newNA[oldNA[nodeX]+lctX];
	    nP->score = nodeP->score;
	    glatCreateLink (newP, newNA[base+lctY], nP, linkP->score);
	    linkN++;
	  }
	}

	/* Create a link, if we didn't find one single matching LCT */
	if (!linkN) {
	  GNode *nP = newNA[oldNA[nodeX]];
	  nP->score = nodeP->score;
	  glatCreateLink (newP, newNA[base], nP, linkP->score);
	  linkN++;
	  if (!didWarn && (latP->expert || newP->expert))
	    WARN ("No matching LCTs found between nodes %d and %d (...)\n", linkP->parentP->nodeX, nodeX);
	  didWarn = 1;
	}
      }

      linkP = linkP->txenP;
    }
  }
  
  /* Clean up */
  free (newNA);
  free (oldNA-2);

  newP->frameShift  = latP->frameShift;
  newP->singularLCT = 1;
  glatPurify (newP);

  return TCL_OK;
}


int glatExpandItf (ClientData cd, int argc, char *argv[])
{
  GLat*      latP = (GLat*) cd;
  GLat*      orgP = NULL;
  SVMap*   svmapP = NULL;
  int         ac  = argc -1;
  int     verbose = 0;
  int          rc;

  if (itfParseArgv (argv[0], &ac, argv+1, 0,
    "<lattice>",    ARGV_OBJECT, NULL,        &orgP,   "GLat",  "Lattice to process",
    "-map",         ARGV_OBJECT, NULL,        &svmapP, "SVMap", "Vocabulary Mapper to use",    
    "-verbose",     ARGV_INT,    NULL,        &verbose,"",      "verbosity",    
    NULL) != TCL_OK) return TCL_ERROR;

  if (!svmapP) svmapP = orgP->svmapP;

  rc = glatSingularLCT (latP, orgP, svmapP, verbose);

  return rc;
}

/* ========================================================================
    Linguistic Constraint
   ======================================================================== */

int glatExtractConstraint(GLat* latP, SVMap* svmapP, int mode) {
  if (mode == Time_Constraint) {
    return TCL_OK;
  }
  if (mode == Flat_Constraint) {
    int nodeX, svX;
    if (!(latP->lmA = realloc (latP->lmA, svmapP->svocabP->list.itemN*sizeof(lmScore)))) {
      ERROR("glatExtractConstraint: allocation failure\n");
      return TCL_ERROR;
    }
    for (svX=0;svX<svmapP->svocabP->list.itemN;svX++)
      latP->lmA[svX]=LMSCORE_MAX;
    for (nodeX=0;nodeX<latP->nodeN;nodeX++) {
      GNode* nodeP = latP->mapping.x2n[nodeX];
      latP->lmA[nodeP->svX] = 0;
    }
    return TCL_OK;
  }
  if (mode == Weak_Constraint) {
    int nodeX,lctX,idx;
    LCon key;
    glatComputeLCT(latP,svmapP);
    listClear((List*) &(latP->LConL));
    for (nodeX=0;nodeX<latP->nodeN;nodeX++) {
      GNode* nodeP = latP->mapping.x2n[nodeX];
      if (!nodeP) continue;
      for (lctX=0;lctX<nodeP->lct.N;lctX++) {
	LCon*  lconP = NULL;
	GLink* linkP = nodeP->childP;

	key.lct  = nodeP->lct.A[lctX];
	key.svN  = 0;
	key.svM  = 0;
	key.svXA = NULL;
	
	idx = listIndex( (List*) &latP->LConL,&key,1);
	lconP = latP->LConL.itemA+idx;
      
	while (linkP) {
	  SVX svX = linkP->childP->svX;
	  idx =0;
	  while ((UINT)idx < lconP->svN && lconP->svXA[idx] != svX) idx++;
	  if ((UINT)idx == lconP->svN) {
	    if (lconP->svN == lconP->svM) {
	      lconP->svM +=5;
	      if (NULL == (lconP->svXA = realloc(lconP->svXA,lconP->svM*sizeof(SVX)))) {
		ERROR("glatExtractConstraint: Allocation failure.\n");
		return TCL_ERROR;
	      }
	      for (idx=lconP->svN;(UINT)idx<lconP->svM;idx++)
		lconP->svXA[idx]= SVX_NIL;
	    }
	    lconP->svXA[lconP->svN++] = svX;	    
	  }
	  linkP=linkP->nextP;
	}
      }
    }    
    return TCL_OK;
  }

  ERROR ("glatExtractConstraint: Invalid Mode.\n");
  return TCL_ERROR;
}

/* ------------------------------------------------------------------------
    glatFillCtx
   ------------------------------------------------------------------------ */

void glatFillCtx (GLat* latP, LCT lct, lmScore* svA, int mode) 
{  
  if (mode == Time_Constraint) {
    return;
  }
  if (mode == Flat_Constraint) {
    if (!(latP->lmA = realloc (latP->lmA, latP->svmapP->svocabP->list.itemN*sizeof(lmScore)))) {
      ERROR("glatFillCtx: allocation failure\n");
      return;
    }
    memcpy(svA,latP->lmA,latP->svmapP->svocabP->list.itemN*sizeof(lmScore));
    return;
  }
  if (mode == Weak_Constraint) {
    int  idx = 0;
    LCon key;
    
    key.lct  = lct;
    key.svN  = 0;
    key.svXA = NULL;
    idx      = listIndex( (List*) &latP->LConL,&key,0);
    
    if (idx>=0) {
      LCon* lconP = latP->LConL.itemA+idx;
      for(idx=0;(UINT)idx<lconP->svN;idx++)
	svA[lconP->svXA[idx]]= 0;
    }
    return;
  }

  ERROR("glatFillCtx: Invalid Mode.\n");
  return;
}

/* ========================================================================
    Lattice Rescoring
   ======================================================================== */

/* ------------------------------------------------------------------------
    helper functions for glatRescore
   ------------------------------------------------------------------------ */

static int piPrint (GLatPathItem* itemP, SVocab* svocabP, int verbose)
{
  if (itemP->fromP) {
    piPrint (itemP->fromP, svocabP, verbose);
    if (verbose == -1) {
      itfAppendResult("%d ", itemP->nodeP->nodeX);
    } 
    if (verbose == 0) {  
      itfAppendResult("%s ", svocabP->list.itemA[itemP->nodeP->svX].name);
    }
    if (verbose == 1) {  
      itfAppendResult("{%s %d %d %2.2f} ", svocabP->list.itemA[itemP->nodeP->svX].name,
		      itemP->nodeP->frameS,itemP->nodeP->frameE,itemP->nodeP->score);
    }
    if (verbose == 2) {  
      itfAppendResult("{%s %d %d %2.2f} ", svocabP->list.itemA[itemP->nodeP->svX].name,
		      itemP->nodeP->frameS,itemP->nodeP->frameE,exp(-itemP->nodeP->gamma));
    }
  }
  return TCL_OK;
}

/* ------------------------------------------------------------------------
    glatRescore
   ------------------------------------------------------------------------ */

static int glatRescore (GLat* latP, SVMap* svmapP, int conf, int topN, int maxN, 
			float beam, int verbose)
{
  LingKS*      lksP = (svmapP) ? svmapP->lksP : NULL ;
  GLatPathItem** pt = NULL; /* flat array of pathItem pointers      */
  int*           pb = NULL; /* base address in pt for each node     */
  int*           pn = NULL; /* number of path items for each node i */
  int        allocN = 100000;
  int       didWarn = (latP->expert) ? 0 : 1;
  int           i, j;

  if (!svmapP) {
    ERROR ("glatRescore: No Vocabulary Mapper.\n");
    return TCL_ERROR;
  } 
  if (latP->nodeN+latP->linkN < 2) {
    WARN ("glatRescore: Empty Lattice.\n");
    itfAppendResult ("{ 0 0.0 ( ) }");
    return TCL_OK;
  } 
  if (latP->end.frameX == -1) {
    ERROR ("glatRescore: Couldn't find final node.\n");
    return TCL_ERROR;
  } 
  if (latP->status == LAT_CREATE || latP->end.frameX == 0) {
    WARN  ("glatRescore: Forced purifying.\n");
    glatPurify(latP);
    if (latP->nodeN+latP->linkN < 2) {
      WARN ("glatRescore: Empty Lattice.\n");
      itfAppendResult ("{ 0 0.0 ( ) }");
      return TCL_OK;
    }
  }

  /* Get memory */
  if (NULL == (pn = (int*) calloc(latP->nodeN+2, sizeof (int)))) {
    ERROR ("glatRescore: Allocation failure (pn).\n");
    return TCL_ERROR;
  } 
  if (NULL == (pb = (int*) calloc(latP->nodeN+2, sizeof (int)))) {
    ERROR ("glatRescore: Allocation failure (pb).\n");
    return TCL_ERROR;
  } 
  if (NULL == (pt = (GLatPathItem**) malloc(allocN*sizeof (GLatPathItem*)))) {
    ERROR ("glatRescore: Allocation failure (pt).\n");
    return TCL_ERROR;
  }

  svmapClearCache(svmapP);

  /* Create initial node */
  pn[0] = 1;
  pb[0] = 0;
  pt[0] = (GLatPathItem*) bmemItem (latP->mem.paths);
  pt[0]->score = 0.0;
  pt[0]->n     = 0;
  pt[0]->lct   = lksP->createLCT (lksP, svmapP->idx.start);
  pt[0]->nodeP = latP->startP;
  pt[0]->fromP = NULL;

  /* Loop over all nodes */
  for (i = 1; i < latP->nodeN+2; i++) {
    GNode*        nodeP;
    GLink*        linkP;
    int           nodeX, isf, pbN;
    LVX           lvX;
    float         best = SCORE_MAX;

    pb[i] = pbN = pb[i-1]+pn[i-1];
    nodeP = (i == latP->nodeN+1) ? latP->endP : latP->mapping.x2n[i-1];
    linkP = nodeP->parentP;
    nodeX = nodeP->nodeX;
    isf   =  (nodeP      == latP->endP) ? TRUE : 
            ((nodeP->svX == SVX_NIL)    ? 0    : svmapP->svocabP->list.itemA[nodeP->svX].fillerTag);
    lvX   = svmapGet (svmapP, nodeP->svX);

    /* Do not extend 'non-existing' words */
    if (nodeP != latP->endP && lvX == LVX_NIL && !isf)
      continue;

    /* Loop over all links into this node */
    while (linkP) {
      int idx = linkP->parentP->nodeX+1;
      int pbX = pb[idx];

      /* For all partial paths stored in this link */
      for (j = 0; j < pn[idx]; j++) {
	GLatPathItem*    pa = pt[pbX++];
	float         score = (conf) ? pa->score : pa->score + linkP->score + nodeP->score;
	LCT             lct = (isf)  ? pa->lct   : lksP->extendLCT (lksP, pa->lct, lvX);
	int         ins_pos = pbN;           /* position to insert         */ 
	int         del_pos = pbN + pn[i]-1; /* position to delete         */  
	int            lctc = 0;             /* lct counter                */
	GLatPathItem*  newP = NULL;          /* new or re-usable path item */
	int               n = pa->n + 1;     /* The length of the path     */
	int          skipIt = 0;
	int               k;

	if (score > best + beam) continue;

	if (pbX >= maxN*(latP->nodeN+2)) {
	  ERROR ("glatRescore: Please increase maxN!\n");
	  return TCL_ERROR;
	}

	/* Do a-posteriori rescoring instead of ML approach: */
	if (conf) {
	  if (nodeX != -2)
	    score  = (score * pa->n + nodeP->gamma) / n;
	} else {
	  /* What kind of transition do we have here? */
	  if (idx == 0 || nodeX == -2)
	    score += 0.0;
	  else if (isf) 
	    score += svmapP->penalty.fil + svmapP->lz * svmapP->lvxSA[nodeP->svX];
	  else
	    score += svmapP->penalty.word + svmapP->lz *
	      svmapGetLMScore(svmapP,pa->lct,nodeP->svX);
	}

	if (score > best + beam) continue;

	/* get lct counter */
	for (k=pbN;k<pbN+pn[i];k++) {
	  if (pt[k]->lct == lct) {
	    lctc++;
	    del_pos = k;
	    /* same theories with same scores: skip it */
	    if (fabs(pt[k]->score - score) < 1e-9) {
	      skipIt = 1;
	      break;
	    }
	  }
	  if (pt[k]->score < score) ins_pos = k+1;
	}

	if (skipIt)              continue;
	if (score > best + beam) continue;

	if (score < best) best = score;

	/* nothing to do */
	if (lctc == topN && score >= pt[del_pos]->score) continue;

	if (pn[i] == maxN && ! didWarn) {
	  WARN("glatRescore: maxN (%d) exceeded.\n",maxN);
	  didWarn = 1;
	}
	if (lctc < topN && pn[i] < maxN) {
	  /* we need a new path item */
	  if (pbN + pn[i] >= allocN-1) {
	    allocN += 100000;
	    if (NULL == (pt = (GLatPathItem**) realloc (pt, allocN*sizeof (GLatPathItem*)))) {
	      ERROR("glatRescore: Allocation failure.\n");
	      return TCL_ERROR;
	    }
	  }	  
	  del_pos = pbN + pn[i];
	  pn[i]++;
	  newP = (GLatPathItem*) bmemItem (latP->mem.paths);	 
	} else {
	  /* re-usable path item */
	  newP = pt[del_pos];
	}

	for (k = del_pos; k > ins_pos; k--) 
	  pt[k] = pt[k-1];
	pt[ins_pos] = newP;

	pt[ins_pos]->score = score;
	pt[ins_pos]->lct   = lct;
	pt[ins_pos]->nodeP = nodeP;
	pt[ins_pos]->fromP = pa;
	pt[ins_pos]->n     = n;
      }
      linkP = linkP->txenP;
    }
  }

  /* Output */
  if (pn[--i] < topN)
    topN = pn[i];

  for (j = 0; j < topN; j++) {
    itfAppendResult ("{ %d %f ", j, pt[pb[i]+j]->score);
    piPrint (pt[pb[i]+j]->fromP, svmapP->svocabP, verbose);
    itfAppendResult ("} ");
  }

  bmemClear (latP->mem.paths);
  free (pb);
  free (pt);
  free (pn);

  return TCL_OK;
}

static int glatRescoreItf (ClientData cd, int argc, char *argv[])
{
  GLat*    latP = (GLat*) cd;
  SVMap* svmapP = latP->svmapP;
  int        ac = argc-1;
  int      topN = 1;
  int      maxN = 20;
  float    beam = 50;
  int   verbose = 0;
  int      conf = 0;

  if ( itfParseArgv( argv[0], &ac, argv+1, 0,
       "-map",     ARGV_OBJECT, NULL, &svmapP, "SVMap", "mapper object between svocab and language model",
       "-conf",    ARGV_INT,    NULL, &conf,   NULL,    "do posteriori rescoring",
       "-topN",    ARGV_INT,    NULL, &topN,   NULL,    "how many hypotheses do we want",
       "-maxN",    ARGV_INT,    NULL, &maxN,   NULL,    "size of hypotheses stack",
       "-beam",    ARGV_FLOAT,  NULL, &beam,   NULL,    "beam threshold to prune hypotheses stack",
       "-v",       ARGV_INT, NULL, &verbose,   NULL,    "verbose output (-1 = index only, 0 = name only, 1 = name, pos, and score, 2 = gamma)",
       NULL) != TCL_OK) return TCL_ERROR;

  return glatRescore (latP, svmapP, conf, topN, maxN, beam, verbose);
}


/* ========================================================================
    Lattice Manipulation functions
   ======================================================================== */

/* ------------------------------------------------------------------------
    glatAddNode
   ------------------------------------------------------------------------ */

static int glatAddNode (GLat* latP, SVX idx, int fromX, int toX, int nodeX,
		       float score, float alpha, float beta, float gamma, float beam)
{
  GNode* nodeP = NULL;

  /* Sanity checks: */
  if (fromX > toX) {
    ERROR ("Frame indices incompatible, ignoring.\n");
    return -1;
  }
  if (toX > latP->end.frameX)
    latP->end.frameX = toX;

  if (nodeX >= 0 && nodeX < latP->nodeM && (nodeP = latP->mapping.x2n[nodeX]) != NULL) {

    /* reconfigure node */
    if (idx)   nodeP->svX    = (SVX) idx;
    if (fromX) nodeP->frameS = fromX;
    if (toX)   nodeP->frameE = toX;

  } else if (beam > 0.0) {

    /* Check for existing node */
    int refX;

    for (refX = 0; refX < latP->nodeM; refX++) {
      GNode* nodeP = latP->mapping.x2n[refX];
      if (nodeP && nodeP->frameS == fromX && nodeP->frameE == toX &&
	  nodeP->svX == (SVX) idx && fabs (nodeP->score - score) < beam)
	return refX;
    }

    return glatAddNode (latP, idx, fromX, toX, nodeX, score, alpha, beta, gamma, 0);

  } else {

    /* create node */
    glatAssureMapTable (latP, ++latP->nodeN);
    for (nodeX = 0; nodeX < latP->mapping.allocN && latP->mapping.x2n[nodeX]; nodeX++);
    if (latP->nodeM <= nodeX) latP->nodeM = nodeX+1;
    nodeP = latP->mapping.x2n[nodeX] = (GNode*) bmemItem(latP->mem.node);

    nodeP->nodeX   = nodeX;
    nodeP->svX     = (SVX) idx;
    nodeP->frameS  = fromX;
    nodeP->frameE  = toX;

    nodeP->childP  = NULL;
    nodeP->parentP = NULL;
    nodeP->rcm.A   = NULL;
    nodeP->rcm.N   = CTX_MAX;
    nodeP->status  = DFS_INIT;
  }

  if (score >= 0) nodeP->score = score;
  if (alpha >= 0) nodeP->alpha = alpha;
  if (beta  >= 0) nodeP->beta  = beta;
  if (gamma >= 0) nodeP->gamma = gamma;

  return nodeX;
}

static int glatAddNodeItf (ClientData cd, int argc, char *argv[])
{
  GLat*      latP = (GLat*) cd;
  SVocab* svocabP = latP->svmapP->svocabP;
  int          ac = argc-1;
  int    fromX, toX, idx, nodeX;
  float  score, gamma, alpha, beta;
  float  beam = 0.0;

  fromX  = toX    = 0;
  nodeX  = -3;
  score  = gamma  = beta   = alpha = -1.0;

  if ( itfParseArgv( argv[0], &ac, argv+1, 0,
       "<word>",  ARGV_LINDEX, NULL, &idx,    &(svocabP->list), "search word",
       "<start>", ARGV_INT,    NULL, &fromX,  NULL,             "start frame",
       "<end>",   ARGV_INT,    NULL, &toX,    NULL,             "end frame",
       "-nodeX",  ARGV_INT,    NULL, &nodeX,  NULL,             "don't add, but configure nodeX",
       "-score",  ARGV_FLOAT,  NULL, &score,  NULL,             "acoustic score",
       "-alpha",  ARGV_FLOAT,  NULL, &alpha,  NULL,             "forward probability",
       "-beta",   ARGV_FLOAT,  NULL, &beta,   NULL,             "backward probability",
       "-gamma",  ARGV_FLOAT,  NULL, &gamma,  NULL,             "a posteriori probability",
       "-beam",   ARGV_FLOAT,  NULL, &beam,   NULL,             "beam to re-use existing node",
       NULL) != TCL_OK) return TCL_ERROR;

  nodeX = glatAddNode(latP,(SVX)idx,fromX,toX,nodeX,score,alpha,beta,gamma,beam);
  if (nodeX < 0) return TCL_ERROR;
  itfAppendElement ("%d", nodeX);
  return TCL_OK;
}


/* ------------------------------------------------------------------------
    glatDeleteNode
   ------------------------------------------------------------------------ */

static int glatDeleteNode(GLat* latP, int nodeX)
{
  GNode* nodeP = NULL;

  if (nodeX >= 0 && nodeX < latP->nodeM) 
    nodeP = latP->mapping.x2n[nodeX];
    
  if (nodeP)
    return glatFreeNode(latP,nodeP);
  else {
    WARN("glatDeleteNode: node '%d' doesn't exists\n",nodeX);
    return TCL_OK;
  }
}

static int glatDeleteNodeItf (ClientData cd, int argc, char *argv[])
{
  GLat*   latP = (GLat*) cd;
  int       ac = argc-1;
  int  idx;

  if ( itfParseArgv( argv[0], &ac, argv+1, 0,
       "nodeX",  ARGV_INT, NULL, &idx, NULL, "node index",
       NULL) != TCL_OK) return TCL_ERROR;

  return glatDeleteNode(latP,idx);

}

/* ------------------------------------------------------------------------
    glatAddLink
   ------------------------------------------------------------------------ */

static float glatAddLink (GLat* latP, int fromX, int toX, float score)
{
  GNode*  fromP = NULL;
  GNode*    toP = NULL;
  GLink*  linkP = NULL;

  if (fromX > latP->nodeM || toX > latP->nodeM || !latP->mapping.x2n[fromX] || !latP->mapping.x2n[toX]) {
    WARN ("One of the nodes is in-existent.\n");
    return FLT_MAX;
  }

  fromP = latP->mapping.x2n[fromX];
  toP   = latP->mapping.x2n[toX];
  linkP = fromP->childP;

  while (linkP && linkP->childP->nodeX != toX)
    linkP = linkP->nextP;

  if (linkP) { 
    /* update score if link already exists */
    linkP->score = score;
  } else {
    /* create new link */
    linkP=glatCreateLink (latP, fromP, toP, score);
  }

  return score;
}

static int glatAddLinkItf (ClientData cd, int argc, char *argv[])
{
  GLat*   latP = (GLat*) cd;
  int       ac = argc-1;
  int    fromX, toX;
  float  score;

  fromX  = toX = 0;
  score  = 0.0;

  if ( itfParseArgv( argv[0], &ac, argv+1, 0,
       "<start>", ARGV_INT,    NULL, &fromX,  NULL, "start node",
       "<end>",   ARGV_INT,    NULL, &toX,    NULL, "end node",
       "-score",  ARGV_FLOAT,  NULL, &score,  NULL, "acoustic (delta) score",
       NULL) != TCL_OK) return TCL_ERROR;

  score = glatAddLink(latP,fromX,toX,score);
  if (score == FLT_MAX) return TCL_ERROR;
  itfAppendResult ("%f", score);
  return TCL_OK;
}


/* ------------------------------------------------------------------------
    glatDeleteLink
   ------------------------------------------------------------------------ */

static int glatDeleteLink(GLat* latP, int fromX, int toX)
{
  GNode*  fromP = NULL;
  GNode*    toP = NULL;
  GLink*  linkP = NULL;

  if (fromX > latP->nodeM || toX > latP->nodeM || !latP->mapping.x2n[fromX] || !latP->mapping.x2n[toX]) {
    WARN ("One of the nodes is in-existent.\n");
    return TCL_ERROR;
  }

  fromP = latP->mapping.x2n[fromX];
  toP   = latP->mapping.x2n[toX];
  linkP = fromP->childP;

  while (linkP && linkP->childP->nodeX != toX)
    linkP = linkP->nextP;

  if (linkP) 
    return glatFreeLink (latP, linkP);
  else { 
    WARN ("Cannot delete non-existent link.\n");
    return TCL_OK;
  }
}

static int glatDeleteLinkItf (ClientData cd, int argc, char *argv[])
{
  GLat*  latP = (GLat*) cd;
  int      ac = argc-1;
  int   fromX = 0;
  int     toX = 0;

  if ( itfParseArgv( argv[0], &ac, argv+1, 0,
       "<start>", ARGV_INT,    NULL, &fromX,  NULL, "start node",
       "<end>",   ARGV_INT,    NULL, &toX,    NULL, "end node",
       NULL) != TCL_OK) return TCL_ERROR;

  return glatDeleteLink(latP,fromX,toX);
}

/* ------------------------------------------------------------------------
    glatAddPath
   ------------------------------------------------------------------------ */

static int glatAddPath(GLat* latP, int lc, SVX* svxA)
{
  SVMap*     svmapP = latP->svmapP;
  GBP**         gbA = NULL;
  GBP*          gbP = NULL;
  GNode*      nodeP = NULL;
  GLink*      linkP = NULL;
  float  finalscore = SCORE_MAX;  
  LVX           lvX = LVX_NIL;

  int corN= 0, insN= 0, delN= 0, subN= 0;
  int  i, nextX, thisX, refX, hypX;

  if (!svxA) {
    ERROR("glatAddPath: no path\n");
    return TCL_ERROR;
  }
  lvX= svmapGet(svmapP,svxA[0]);
  if (lvX != svmapP->idx.start) {
    ERROR("glatAddPath: no begin of sentence '%s' at begin of path\n",svmapP->string.start);
    return TCL_ERROR;
  }
  lvX= svmapGet(svmapP,svxA[lc-1]);
  if (lvX != svmapP->idx.end) {
    ERROR("glatAddPath: no end of sentence '%s' at end of path\n",svmapP->string.end);
    return TCL_ERROR;
  }

  /* Call the align procedure */
  if (latP->nodeN)
    glatAlign (latP, lc, svxA, 0, 0,&corN, &delN, &insN, &subN, &gbA);
  
  /* --------------------------------------------------- */
  /* backtrace                                           */
  /* --------------------------------------------------- */
  assert (gbA); // XCode says could be NULL
  linkP      = latP->endP->parentP;
  refX       = lc;
  hypX       = latP->nodeN+1;
  while (linkP) {
    float score = gbA[refX][linkP->parentP->nodeX+1].score;
    if (score < finalscore) {
      finalscore = score;
      hypX       = linkP->parentP->nodeX+1;
    }
    linkP = linkP->txenP;
  }

  if (finalscore >= SCORE_MAX) {
    thisX = -1;
    for (i = 0; i < lc; i++) {
      nextX = glatCreateNode (latP, svxA[i], 0, 0);
      glatCreateLink (latP, latP->mapping.x2n[thisX], latP->mapping.x2n[nextX], 0.0);
      thisX = nextX;
    }
    glatCreateLink (latP, latP->mapping.x2n[thisX], latP->mapping.x2n[-2], 0.0);

  } else {
    assert (gbA); // XCode says could be NULL
    thisX = nextX = -2;
    while (hypX >= 1 || refX >= 1) {
      gbP   = gbA[refX]+hypX;
      nodeP = latP->mapping.x2n[hypX-1];
      linkP = nodeP->childP;

      /* Nothing to do for Insertions */
      if (gbP->refX < refX && gbP->hypX < hypX) {
	if (svxA[refX-1] == nodeP->svX) {
	  thisX = nodeP->nodeX;
	  /* Correct Alignment */
	} else {
	  thisX = glatCreateNode (latP, svxA[refX-1], nodeP->frameS, nodeP->frameE);
	  linkP = latP->mapping.x2n[thisX]->childP;
	  /* Substitution */
	}
      } else if (gbP->refX < refX && gbP->hypX == hypX) {
	thisX = glatCreateNode (latP, svxA[refX-1], nodeP->frameE+1, nodeP->frameE);
	linkP = latP->mapping.x2n[thisX]->childP;
	/* Deletion Hack-Alert: the time alignment is bad, bad, bad */
      }
      
      /* link new node to next one */
      if (thisX != nextX)
	glatCreateLink (latP, latP->mapping.x2n[thisX], latP->mapping.x2n[nextX], 0.0);
      
      nextX = thisX;
      refX  = gbP->refX;
      hypX  = gbP->hypX;
    }

    /* connect startP to first node if not yet done */
    if (latP->mapping.x2n[thisX] && latP->mapping.x2n[thisX] != latP->startP)
      glatCreateLink (latP, latP->startP, latP->mapping.x2n[thisX], 0.0);

  }    

  /* Free the memory */
  if (gbA) {
    if (gbA[0]) free(gbA[0]);
    free(gbA);
  }
  
  latP->status = LAT_CREATE;
  glatPurify (latP);

  itfAppendElement ("%d", lc);
  return TCL_OK;
}

static int glatAddPathItf (ClientData cd, int argc, char *argv[])
{
  GLat*      latP = (GLat*) cd;
  SVMap*   svmapP = latP->svmapP;
  SVocab* svocabP = svmapP->svocabP;
  int          ac = argc-1;
  char*      text = NULL;
  SVX*       svxA = NULL;
  char       **lv = NULL;
  int lc,i,retValue;

  if (itfParseArgv( argv[0], &ac, argv+1, 0,
       "<path>",  ARGV_STRING, NULL, &text,   NULL, "the path to add",
       NULL) != TCL_OK) return TCL_ERROR;

  if (Tcl_SplitList (itf, text, &lc, &lv) != TCL_OK) {
    ERROR ("Couldn't split list '%s'.\n", text);
    return TCL_ERROR;
  }

  if (NULL == (svxA = (SVX*) malloc (sizeof(SVX)*lc))) {
    Tcl_Free ((char*) lv);
    ERROR ("Allocation failure.\n");
    return TCL_ERROR;
  }

  for (i = 0; i < lc; i++) {
    int svX = listIndex ((List*) &(svocabP->list), lv[i], 0);
    if (svX == -1) {
      ERROR ("Couldn't find word '%s' in vocabulary.\n", lv[i]);
      Tcl_Free ((char*) lv);
      return TCL_ERROR;
    }
    svxA[i] = (SVX) svX;
  }

  retValue=glatAddPath(latP,lc,svxA);

  if (svxA) free(svxA);
  if (lv)   Tcl_Free((char*) lv);

  return retValue;

}

/* ------------------------------------------------------------------------
    glatRead
   ----------
    These functions are called to read a lattice from a file.
    NJD_LATTICE format is incompatible with the old format in that
    it contains OPTWORDS too and is written from left to right, i.e. in
    the order of creation during search. The 'links' attached to each
    node therefore are the links leading into this node.
   ------------------------------------------------------------------------ */

#define MAXLINE 4000000

char *lat_status[] = { "INIT", "CREATE", "PURE", "" };


/* ------------------------------------------------------------------------
    Read in the current NJD lattice format
   ------------------------------------------------------------------------ */

static int glatReadNJD (GLat* latP, FILE* fp, int silent, SVX gX)
{
  SVMap*   svmapP = latP->svmapP;
  SVocab* svocabP = svmapP->svocabP;

  int    didWarn        = 0;
  int    svX            = 0;
  int    nodeX, i, j, idx;
  float  score;
  char  *line           = NULL;
  int    lc;
  char **lv;
  int    fc;
  char **fv;
  GNode *nodeP          = NULL;
  int    checkLinksOnly = 0;

  if ( NULL == (line = (char*)malloc (MAXLINE * sizeof(char))) ) {
    ERROR ("glatReadNJD: allocation failure.\n");
    return TCL_ERROR;
  }

  while (fgets (line, MAXLINE, fp) && strcmp (line, "END_NJD_LATTICE\n")) {
    if (Tcl_SplitList (itf, line, &lc, &lv) != TCL_OK) {
      WARN ("Couldn't split list '%s'.\n", line);
      Tcl_Free((char*) lv);
      continue;
    }
    
    nodeX = atoi(lv[0]);
    if (nodeX < -2) {
      WARN ("Invalid node '%d'.\n", nodeX);
      continue;

    } else if (nodeX >= latP->nodeM) {
      WARN ("nodeX (%d) >= nodeM (%d) . (increased nodeM)\n", nodeX,latP->nodeM);
      glatAssureMapTable (latP, nodeX);
      latP->nodeM = nodeX;
      if (nodeX > latP->nodeN)
	latP->nodeN = nodeX;

    } else if (nodeX == -2) {
      svX   = (SVX) svmapP->idx.end;
      nodeP = latP->endP;

    } else if (nodeX == -1) {
      svX   = (SVX) svmapP->idx.start;
      nodeP = latP->startP;

    } else {
      svX = listIndex ((List*) &(svocabP->list), lv[1], 0);
      if (svX == -1 && strstr (lv[1], "(")) { /* Couldn't find word, try baseform */
	if (!(didWarn & 1)) {
	  INFO ("Couldn't find '%s' in '%s', trying baseform.\n", lv[1], svocabP->name);
	  didWarn |= 1;
	}
	*(strstr (lv[1], "(")) = '\0';
	svX = listIndex ((List*) &(svocabP->list), lv[1], 0);
      }
      if (svX == -1) { /* Still no word */
	if (!(didWarn & 2)) {
	  WARN ("Couldn't find '%s' in '%s', setting to idx %d.\n", lv[1], svocabP->name, gX);
	  didWarn |= 2;
	}
	svX = gX;
	/*
	  svX = listIndex ((List*) &(svocabP->list), svocabP->string.nil, 0);
	  svX = listIndex ((List*) &(svocabP->list), "$", 0);
	  svX = 0;
	*/
      }
      nodeP = latP->mapping.x2n[nodeX];
      if (! nodeP) {
	nodeP = (GNode*) bmemItem(latP->mem.node);
	checkLinksOnly = 0;
      } else {
	checkLinksOnly = 1;
      }
    }

    if (! checkLinksOnly) {
      latP->mapping.x2n[nodeX] = nodeP;
      nodeP->status  = DFS_INIT;
      nodeP->nodeX   = nodeX;
      nodeP->frameS  = atof (lv[2]);
      nodeP->frameE  = atof (lv[3]);
      nodeP->score   = atof (lv[4]);
      nodeP->svX     = svX;
      nodeP->alpha   = nodeP->gamma = 0.0; /* nodeP->beta */
      nodeP->childP  = NULL;
      nodeP->parentP = NULL;
      nodeP->rcm.A   = NULL;
      nodeP->rcm.N   = CTX_MAX;
    }

    for (i = 5; i < lc-1; i++) {
      if      (!(strcmp (lv[i], "-alpha")))
	nodeP->alpha  = atof (lv[++i]);
      else if (!(strcmp (lv[i], "-beta")))
	 nodeP->beta  = atof (lv[++i]);
      else if (!(strcmp (lv[i], "-gamma")))
	nodeP->gamma  = atof (lv[++i]);
      else if (!(strcmp (lv[i], "-from"))) {
	if (Tcl_SplitList (itf, lv[++i], &fc, &fv) != TCL_OK)
	  WARN ("Couldn't interpret argument '%s' to '-from'.\n", lv[i]);
	else for (j = fc-1; j >= 0; j--) {
	  if (sscanf (fv[j], "%d %f", &idx, &score) != 2)
	    WARN ("Couldn't parse string '%s'.\n", fv[j]);
	  else if (idx < -2 || idx > latP->nodeM || latP->mapping.x2n[idx] == NULL) {
	    if (! silent) WARN ("Node %d invalid, skipping link.\n", idx);
	  } else {
	    glatCreateLink (latP, latP->mapping.x2n[idx], latP->mapping.x2n[nodeX], score);
	  }
	}
	Tcl_Free((char*) fv);
      } else
	WARN ("Couldn't interpret token '%s'.\n", lv[i]);
    }
    Tcl_Free((char*) lv);
  }

  if ( line ) Nulle (line);

  return TCL_OK;
}


/* ------------------------------------------------------------------------
    Read in the old JANUS lattice format
    CAVEAT: The old lattice format compresses several word-instances with
      identical start-times, but different end-times into one node and
      also doesn't contain optional words - we don't recover this
      information. If you want to have optwords or time-alignments, do
      acoustic rescoring
   ------------------------------------------------------------------------ */

static int glatFindNodeFromFrame (GLat* latP, int m, SVX svX, int frameS)
{
  /* This is a little helper routine to find a node according to its
     svX and its start frame, as it is marked in the old lattice format */
  int i;
  for (i = 0; i < m && i < latP->mapping.allocN; i++) {
    if (latP->mapping.x2n[i]->svX    == svX &&
        latP->mapping.x2n[i]->frameS == frameS)
      return i;
  }

  return m;
}

static int glatReadJANUS (GLat* latP, FILE* fp)
{
  SVMap*   svmapP = latP->svmapP;
  SVocab* svocabP = svmapP->svocabP;
  int     i, j, nodeX, svX, idx, svY, frameS;
  char  *line     = NULL;
  int    lc;
  char **lv;
  int    fc;
  char **fv;
  int    gc;
  char **gv;
  GNode *nodeP;

  if ( NULL == (line = (char*)malloc (MAXLINE * sizeof(char))) ) {
    ERROR ("glatReadJANUS: allocation failure.\n");
    return TCL_ERROR;
  }

  nodeX = 0;
  while (fgets (line, MAXLINE, fp) && strcmp (line, "END_LATTICE\n")) {
    if (Tcl_SplitList (itf, line, &lc, &lv) != TCL_OK) {
      Tcl_Free((char*) lv);
      WARN ("Couldn't split list '%s'.\n", line);
      continue;
    }

    svX = listIndex ((List*) &(svocabP->list), lv[1], 0);
    if (svX == -1) {
      WARN ("Couldn't find '%s' in '%s'.\n", lv[1], svocabP->name);
      svX = listIndex ((List*) &(svocabP->list), svocabP->string.nil, 0);
    }

    nodeP = (GNode*) bmemItem(latP->mem.node);

    latP->nodeN++;
    latP->mapping.x2n[nodeX] = nodeP;
    nodeP->status  = DFS_INIT;
    nodeP->nodeX   = nodeX;
    nodeP->frameS  = atof (lv[0]);
    nodeP->frameE  = nodeP->frameS+1;
    nodeP->score   = nodeP->alpha = nodeP->gamma = 0.0; /* nodeP->beta */
    nodeP->svX     = svX;
    nodeP->childP  = NULL;
    nodeP->parentP = NULL;
    nodeP->rcm.A   = NULL;
    nodeP->rcm.N   = CTX_MAX;

    for (i = 2; i < lc-1; i++) {
      if (!strcmp (lv[i], "-score"))
	nodeP->score = atof (lv[++i]);
      else if (!strcmp (lv[i], "-links")) {
	if (Tcl_SplitList (itf, lv[++i], &fc, &fv) == TCL_OK) {
	  for (j = 0; j < fc; j++) {
	    if (Tcl_SplitList (itf, fv[j], &gc, &gv) != TCL_OK || gc != 4)	    
	      WARN ("Couldn't parse '%s'.\n", fv[i]);
	    else {
	      frameS = atoi (gv[0]);
	      svY    = listIndex ((List*) &(svocabP->list), gv[1], 0);
	      if (svY == -1) {
		WARN ("Couldn't find '%s' in '%s'.\n", gv[1], svocabP->name);
		svY  = listIndex ((List*) &(svocabP->list), svocabP->string.nil, 0);
	      }
	      if ((idx = glatFindNodeFromFrame (latP, nodeX, (SVX)svY, frameS)) < nodeX)
		glatCreateLink (latP, latP->mapping.x2n[nodeX], latP->mapping.x2n[idx], atof (gv[3]));
	    }
	    Tcl_Free((char*) gv);
	  }
	}
	Tcl_Free((char*) fv);
      }
    }
    Tcl_Free((char*) lv); 
    nodeX++;
  }

  /* Create links to start and end nodes */
  glatCreateLink (latP, latP->startP,     latP->mapping.x2n[nodeX-1], 0);
  glatCreateLink (latP, latP->mapping.x2n[0], latP->endP,             0);

  /* purify necessary to ensure correct node order */
  latP->status = LAT_CREATE;
  glatPurify(latP);

  if ( line ) Nulle (line);

  return TCL_OK;
}

static int glatReadItf (ClientData cd, int argc, char *argv[])
{
  GLat*  latP = (GLat*) cd;
  int      ac = argc-1;
  char  *file = NULL;
  char  *line = NULL;
  char    *gw = NULL;
  int      gX = 0;
  FILE    *fp = NULL;
  int      ok = 0;
  int   linkN = 0;

  if (itfParseArgv( argv[0], &ac, argv+1, 0,
      "<file>",       ARGV_STRING, NULL, &file, NULL, "file to read from",
      "-garbageWord", ARGV_STRING, NULL, &gw,   NULL, "garbage word",
       NULL) != TCL_OK) return TCL_ERROR;

  glatClear (latP);

  if (!(fp = fileOpen (file, "r"))) {
    WARN ("Cannot open '%s' for reading, aborting.\n", file);
    return TCL_ERROR;
  }

  if ( NULL == (line = (char*)malloc (MAXLINE * sizeof(char))) ) {
    ERROR ("glatReadItf: allocation failure.\n");
    return TCL_ERROR;
  }
  
  if (gw) {
    if ((gX = listIndex ((List*) &(latP->svmapP->svocabP->list), gw, 0)) < 0) {
      WARN ("Couldn't find '%s', setting to NIL.\n", gw);
      gX = listIndex ((List*) &(latP->svmapP->svocabP->list), latP->svmapP->svocabP->string.nil, 0);
    }
  }

  latP->end.frameX = 0;
  while (line && !ok) {
    if (!fgets (line, MAXLINE, fp)) {
      INFO ("File '%s' doesn't seem to contain data, ignored.\n", file);
      itfAppendResult ("? 0 0 0");
      ok = 1;
    } else if (!strcmp (line, "BEGIN_NJD_LATTICE\n")) {
      int    lc;
      char **lv;

      latP->status = LAT_PURE;

      if (!fgets (line, MAXLINE, fp) || 
	  Tcl_SplitList (itf, line, &lc, &lv) != TCL_OK || strcmp (lv[0], "INFO")) { 
	INFO ("Info line not found or unreadable in '%s'.\n", file);
	Tcl_Free((char*) lv);
      } else {
	int i;

	for (i = 1; i < lc-1; i++) {
	  if      (streq (lv[i], "-nodeN"))      latP->nodeN      = atoi (lv[++i]);
	  else if (streq (lv[i], "-nodeM"))      latP->nodeM      = atoi (lv[++i]);
	  else if (streq (lv[i], "-linkN"))      linkN            = atoi (lv[++i]);
	  else if (streq (lv[i], "-frameN"))     latP->end.frameX = atoi (lv[++i]);
	  else if (streq (lv[i], "-frameShift")) latP->frameShift = atof (lv[++i]);
	  else if (streq (lv[i], "-utt"))        INFO ("Reading lat '%s'.\n", lv[++i]);
	  else if (streq (lv[i], "-status")) {
	    int j;
	    i++;
	    for (j = 0; strlen (lat_status[j]); j++)
	      if (streq (lv[i], lat_status[j])) {
		latP->status = j;
		break;
	      }
	    if (!strlen (lat_status[j]))
	      WARN ("Unknown lattice status '%s'.\n", lv[i]);
	  } else WARN ("Info line contains token '%s' - whassat?\n", lv[++i]);
	}
	Tcl_Free((char*) lv);

	/* Make sure we don't read junk */
	if (latP->nodeN <= 0 || latP->nodeM <= 0) {
	  INFO ("glatReadItf: empty lattice, clearing.\n");
	  fileClose (file, fp);
	  glatClear (latP);
	  if ( line ) Nulle (line);
	  return TCL_OK;
	}

	glatAssureMapTable (latP, latP->nodeM);
	glatReadNJD        (latP, fp, 1, (SVX) gX);

	/* read the lattice again */
	if (linkN != latP->linkN) {
	  fileClose (file, fp);
	  fp = fileOpen (file, "r");
	  while (fgets (line, MAXLINE, fp) && strcmp (line, "BEGIN_NJD_LATTICE\n"));
	  if (!fgets (line, MAXLINE, fp) || 
	      Tcl_SplitList (itf, line, &lc, &lv) != TCL_OK || strcmp (lv[0], "INFO")) { 
	    INFO ("Info line not found or unreadable in '%s'.\n", file);
	    Tcl_Free((char*) lv);
	  }
	  glatReadNJD (latP, fp, 0, (SVX) gX);

	  if (linkN == latP->linkN) {
	    latP->status = LAT_CREATE;
	    WARN ("glatRead: lattice doesn't seem to be purified yet.\n");
	  } else {
	    ERROR ("glatRead: inconsistent number of links (expected %d but got %d).\n",
		  linkN,latP->linkN);
	    glatClear (latP);
	    if ( line ) Nulle (line);
	    return TCL_ERROR;
	  }
	}
	
	itfAppendResult ("njd %d %d %d", latP->nodeN, latP->nodeM, latP->linkN);
	ok = 1;
      }
      
    } else if (!strcmp (line, "BEGIN_LATTICE\n")) {
      glatClear (latP);
      while (fgets (line, MAXLINE, fp) && strcmp (line, "END_LATTICE\n"))
	latP->nodeM++;

      /* Re-open the file */
      fileClose (file, fp);
      fp = fileOpen (file, "r");
      while (fgets (line, MAXLINE, fp) && strcmp (line, "BEGIN_LATTICE\n"));
	
      glatAssureMapTable (latP, latP->nodeM);
      glatReadJANUS      (latP, fp);

      itfAppendResult ("janus %d %d %d", latP->nodeN, latP->nodeM, latP->linkN);
      ok = 1;
    }
  }

  fileClose (file, fp);

  if ( line ) Nulle (line);

  return TCL_OK;
}


/* ------------------------------------------------------------------------
    glatWrite
   -----------
    These functions are called to write a lattice to a file.
    NJD_LATTICE format is incompatible with the old format in that
    it contains OPTWORDS too and is written from left to right, i.e. in
    the order of creation during search. The 'links' attached to each
    node therefore are the links leading into this node.
   ------------------------------------------------------------------------ */

static int glatWriteNode (GLat* latP, GNode* nodeP, FILE* fp, char* mode)
{
  SVMap*   svmapP = latP->svmapP;
  SVocab* svocabP = svmapP->svocabP;
  SWord*   swordP = svocabP->list.itemA; 
  SVX       idx   = nodeP->svX;

  if (nodeP->frameS == 0 && nodeP->frameE == 0 && idx == SVX_NIL)
    idx = (SVX) svmapP->idx.end;

  fprintf (fp, "%d {%s} %d %d %.4e",
	   nodeP->nodeX, swordP[idx].name, nodeP->frameS, nodeP->frameE, nodeP->score);

  if (mode) {
    int imode = atoi (mode);
    if ((imode & 1) || strchr (mode, 'a')) fprintf (fp, " -alpha %.4e", nodeP->alpha);
    if ((imode & 2) || strchr (mode, 'b')) fprintf (fp, " -beta %.4e",  nodeP->beta);
    if ((imode & 4) || strchr (mode, 'g')) fprintf (fp, " -gamma %.4e", nodeP->gamma);
    if ((imode & 8) || strchr (mode, 'l')) {
      int i;
      fprintf (fp, " -lct %d {", nodeP->lct.N);
      for (i = 0; i < nodeP->lct.N; i++)
	fprintf (fp, " %d", (int) nodeP->lct.A[i]);
      fprintf (fp, "}");
    }
  }

  if (nodeP->parentP) {
    GLink* linkP = nodeP->parentP;   
    fprintf (fp, " -from {");
    while (linkP) {
      GNode *fromP = linkP->parentP;
      idx = (fromP->frameS == 0 && fromP->frameE == 0) ? svmapP->idx.start : fromP->svX;
      fprintf (fp, "{%d %.4e} ",
	       fromP->nodeX, linkP->score);
      linkP = linkP->txenP;
    }
    fprintf (fp, "}\n");
  } else 
    fprintf (fp, "\n");

  return TCL_OK;
}

static int glatWriteNJD (GLat* latP, FILE* fp, char* utt, char* mode)
{
  int i;

  if (strcmp (utt, "?"))
    fprintf (fp, "BEGIN_NJD_LATTICE\nINFO -nodeN %d -nodeM %d -linkN %d -frameN %d -status %s -frameShift %f -utt %s\n",
	     latP->nodeN, latP->nodeM, latP->linkN, latP->end.frameX, lat_status[latP->status], latP->frameShift, utt);
  else
    fprintf (fp, "BEGIN_NJD_LATTICE\nINFO -nodeN %d -nodeM %d -linkN %d -frameN %d -status %s -frameShift %f\n",
	     latP->nodeN, latP->nodeM, latP->linkN, latP->end.frameX, lat_status[latP->status], latP->frameShift);
  
  for (i = 0; i < latP->nodeM; i++)
    if (latP->mapping.x2n[i])
      glatWriteNode (latP, latP->mapping.x2n[i], fp, mode);
  glatWriteNode (latP, latP->endP, fp, mode);

  fprintf (fp, "END_NJD_LATTICE\n");
  return TCL_OK;
}


/* -----------------------------------------------------------
     SLF lattices:
   -----------------
     We can also write HTK lattices in two flavours

     HTK only uses a "wdprob", which is a penalty applied to all word transitions.
     In order to rescore lattices externally (e.g., in SRI's lattice-tool),
     we store the word and filler penalties separately as "extra" binary scores.
     The score "x1" should be 1.0 when the word penalty is to be applied, and
     0.0 otherwise. Similarly, the "x2" score should be 1.0 iff the filler
     penalty is to be applied. One can then specify separate scaling factors
     for each term:
        acscale=1.0
        lmscale=lz
        x1scale=-log10(wordpen)
        x2scale=-log10(fillpen)
     so that the final weight of each link is:
        w = acscale * a + lmscale * l + x1scale * x1 + x2scale * x2
          = 1.0 * log10(acoustic score) + lz * log10(LM score)
               + (word) ? -log10(wordpen) : 0
               + (filler) ? -log10(fillpen) : 0

    An example call to the SRI-LM Toolkit lattice-tool would be:

    lattice-tool -read-htk -htk-acscale $a -htk-lmscale $l -htk-x1scale $wp \
    -htk-x2scale $fp -in-lattice-list lats.list

   ----------------------------------------------------------- */

static int getPV (SVMap* svmapP, SVX svX, char* buf)
{
  LVX       lvX = svmapGet(svmapP,svX);
  SWord* swordP = svmapP->svocabP->list.itemA;
  int        pv = 1;

  if      (lvX == svmapP->idx.end)   strcpy (buf, "!SENT_END");
  else if (lvX == svmapP->idx.start) strcpy (buf, "!SENT_START");
  else {
    strncpy (buf, swordP[svX].name, 512);
    if (strchr (buf+1, '(')) { /* ')' keeps emacs happy */
      int dictX, dictY;
      *strchr (buf, '(') = 0;
      dictX = swordP[svX].dictX;
      dictY = listIndex ((List*)&(svmapP->svocabP->dictP->list), (ClientData) buf, 0);
      while (dictY != dictX) 
	pv++, dictY = svmapP->svocabP->dictP->list.itemA[dictY].varX;
    }
  }
  
  return pv;
}


static int printSLFNode (FILE* fp, int i, int f, char* buf, int pv, float fs, float t_offset)
{
  if (pv != 1) fprintf (fp, "I=%d t=%.3f W=%s v=%d\n",
			i, t_offset + fs*f, buf, pv);
  else         fprintf (fp, "I=%d t=%.3f W=%s\n",
			i, t_offset + fs*f, buf);
  return TCL_OK;
}


static int printSLFLink (FILE* fp, int idx, int s, int e, float aScore, float lScore, 
			 float tPenalty, float lScale, float wordPenScore, 
			 float fillPenScore)
{
  float lProb = lScore + tPenalty / lScale;
  
  /* HTK only uses a "wdprob", which is a penalty applied to all word transitions.
     It seems that Lidia wants her LM scores unscaled and usually sets the scaling factor 
     separately from the command line, neglecting the stored information.
     Because we have more than just "wdpenalty", we scale down the transition penalty
     and so pretend to have an 'unscaled' lm-score and no further remaining transition
     penalties. */

  fprintf (fp, "J=%d S=%d E=%d a=%.3f l=%.3f x1=%.1f x2=%.1f\n",
	   idx, s, e, -aScore / M_LN10, lProb, wordPenScore, fillPenScore);

  return TCL_OK;
}


static int glatWriteSLF (GLat* latP, FILE* fp, char* utt, char* mode, SVMap* svmapP, float t_offset)
{
  int     nodeX, idx;
  char    buf[512];

  int        nodeN = 1;
  int        linkN = 0;
  LingKS*     lksP = svmapP->lksP;
  SVocab*  svocabP = svmapP->svocabP;
  SWord*    swordP = svocabP->list.itemA;
  
  /* Allocate an array of base indices and link numbers */
  int* idxA = (int*) calloc (latP->nodeN+2, sizeof (int));

  svmapClearCache(svmapP);

  /* Compute the language model states */
  glatComputeLCT (latP, svmapP);


  /* ---------------------------------------------------
      "Inflation"
     -------------
      We have to create virtual multiple instances
      for our nodes because we need the LCT information
     --------------------------------------------------- */
  for (nodeX = 0; nodeX <= latP->nodeN; nodeX++) {
    int       sn = (nodeX == latP->nodeN) ? -2 : nodeX;
    GNode* nodeP = latP->mapping.x2n[sn];
    GLink* linkP = nodeP->parentP;
    LVX     lvxE = svmapGet (svmapP, nodeP->svX);

    if (sn == -2) {
      /* **** Virtual end node **** */
      nodeN++;
      while (linkP)
	linkN++, linkP = linkP->txenP;

    } else if (svmapGet(svmapP,nodeP->svX) == svmapP->idx.end) {
      /* **** !SENT_END **** */
      nodeN++;
      while (linkP)
	linkN += linkP->parentP->lct.N, linkP = linkP->txenP;

    } else {
      /* **** Normal case **** */
      LCT* aE  = nodeP->lct.A;
      nodeN   += nodeP->lct.N;
      while (linkP) {
	LCT* aS = linkP->parentP->lct.A;
	int lctX, lctY;
	float trS = LMSCORE_MAX;

	for (lctX = 0; lctX < nodeP->lct.N; lctX++) {
	  for (lctY = 0; lctY < linkP->parentP->lct.N; lctY++) {
	    LCT e = aE[lctX];
	    LCT s = (swordP[nodeP->svX].fillerTag)
	          ? aS[lctY]
                  : lksP->extendLCT (lksP, aS[lctY], lvxE);

	    /* In order to guarantee at least one transition in the resulting
	       lattice for every link in our current lattice, I insert the one
	       for lctX = 0 || lctY = 0, but with a bad language
	       model score (if a better one can't be found) */
	    if (!lctX && !lctY) trS = 0.0;

	    if (s == e) {
	      trS = LMSCORE_MAX;
	      linkN++;
	    }
	  }
	}
	if (trS != LMSCORE_MAX)
	  linkN++;

	linkP = linkP->txenP;
      }
    }
  }


  /* -------------------------------------------------
      Print Header
     ------------------------------------------------- */
  fprintf (fp, "########################################################################\n");
  fprintf (fp, "#                                                                      #\n");
  fprintf (fp, "#   SLF lattice written by Ibis                                        #\n");
  fprintf (fp, "#                                                                      #\n");
  fprintf (fp, "####### $Id: glat.c 3428 2011-04-15 18:00:28Z metze $ ########\n#\n");
  fprintf (fp, "VERSION=1.0\n");
  fprintf (fp, "lmname=%s\n",     svmapP->lksP->name);
  fprintf (fp, "lmscale=%.2f\t# LM weight: %.1f\n", 
	   -svmapP->lz*LMSCORE_SCALE_FACTOR / M_LN10, -svmapP->lz*LMSCORE_SCALE_FACTOR);
  /* fprintf (fp, "wdpenalty=%.1f\n",  svmapP->penalty.word); */
  fprintf (fp, "x1scale=-%.2f\t# word penalty in log_10\n", svmapP->penalty.word / M_LN10);
  fprintf (fp, "x2scale=-%.2f\t# filler penalty in log_10\n", svmapP->penalty.fil / M_LN10);
  fprintf (fp, "UTTERANCE=%s\n", utt);
  fprintf (fp, "base=10\n"); 
  fprintf (fp, "start=0\n"); /* first printed node is always start lattice node */
  fprintf (fp, "end=%d\n", nodeN - 1); /* last printed node is always final lattice node */
  fprintf (fp, "NODES=%d LINKS=%d\n", nodeN, linkN);
  fprintf (fp, "#\n#  SVMap=%s (Penalties: phone=%.1f word=%.1f fil=%.1f)\n", 
	   svmapP->name, svmapP->penalty.phone, svmapP->penalty.word,svmapP->penalty.fil);
  fprintf (fp, "# LingKS=%s\n# SVocab=%s\n#   Dict=%s\n",
	   svmapP->lksP->name, svmapP->svocabP->name, svmapP->svocabP->dictP->name);
  if (mode)
    fprintf (fp, "#\n# COMMENT: %s\n", mode);

  if (latP->status != LAT_PURE) {
    WARN ("Lattice impure => might be impossible to read in.\n");
    fprintf (fp, "#\n# WARNING: Lattice was written in an impure state!\n");
  }


  /* -------------------------------------------------
      Print Nodes
     -------------
      We assume a frameshift of 10ms for time info
      Also, note positions
     ------------------------------------------------- */
  fprintf (fp, "#\n# Nodes follow:\n#\n");
  printSLFNode (fp, 0, 0, "!NULL", -1, latP->frameShift, t_offset);
  for (nodeX = 0, idx = 1; nodeX < latP->nodeN; nodeX++) {
    GNode* nodeP = latP->mapping.x2n[nodeX];
    int    nodeF = (svmapGet(svmapP,nodeP->svX) == svmapP->idx.end) ? 1 : nodeP->lct.N;
    int       pv = getPV (svmapP, nodeP->svX, buf);
    int     lctX;

    idxA[nodeX+1] = idx;
    for (lctX = 0; lctX < nodeF; lctX++)
      printSLFNode (fp, idx++, nodeP->frameE, buf, pv, latP->frameShift, t_offset);
  }
  idxA[nodeX+1] = idx;
  printSLFNode (fp, idx, latP->end.frameX, "!NULL", -1, latP->frameShift, t_offset);
  fprintf (fp, "#\n# Links follow:\n#\n");


  /* ------------------------------------ 
      Print Links
     -------------
      Links go into nodes
     ------------------------------------ */
  for (nodeX = 0, idx = 0; nodeX <= latP->nodeN; nodeX++) {
    int       sn = (nodeX == latP->nodeN) ? -2 : nodeX;
    GNode* nodeP = latP->mapping.x2n[sn];
    GLink* linkP = nodeP->parentP;
    LVX     lvxE = svmapGet (svmapP, nodeP->svX);

    if (sn == -2) {
      /* ---------------------------------------------------
	  Virtual end node: No further computation required
         --------------------------------------------------- */
      while (linkP) {
	printSLFLink (fp, idx++, idxA[linkP->parentP->nodeX+1], nodeN-1, 0.0, 0.0, 0.0, 
		      1.0, 0.0, 0.0);
	linkP = linkP->txenP;
      }

    } else if (svmapGet(svmapP,nodeP->svX) == svmapP->idx.end) {
      /* ------------------------------
 	  !SENT_END: Only one instance
         ------------------------------ */
      int nodeE = idxA[nodeP->nodeX+1];
      while (linkP) {
	LCT*       aS = linkP->parentP->lct.A;
	int     nodeS = idxA[linkP->parentP->nodeX+1];
	/* float trScore = (linkP->parentP->nodeX == -1) ? 0.0 : svmapP->penalty.word; */
	float wordPenScore = (linkP->parentP->nodeX == -1) ? 0.0 : 1.0;
	int      lctY;

	for (lctY = 0; lctY < linkP->parentP->lct.N; lctY++)
	  printSLFLink (fp, idx++, nodeS + lctY, nodeE, 
			nodeP->score+linkP->score,
			UnCompressScore (lksP->scoreFct (lksP, aS[lctY], lvxE, 0)),
			0.0, 1.0, wordPenScore, 0.0);

	linkP = linkP->txenP;
      }

    } else {
      /* ---------------------------------------------------------
	  Normal case: Check for valid LM transition or equal LCT
         --------------------------------------------------------- */
      LCT*     aE = nodeP->lct.A;
      int   nodeE = idxA[nodeP->nodeX+1];
      float trPen = svmapP->lz * svmapP->lvxSA[nodeP->svX];
      float wordPenScore = 1.0, fillPenScore = 0.0;

      if (swordP[nodeP->svX].fillerTag) {
	wordPenScore = 0.0;  fillPenScore = 1.0;
      }

      /*
      float trPen = 0.0;

      if (swordP[nodeP->svX].fillerTag) {
	trPen = svmapP->penalty.fil;
      } else if (nodeP->nodeX != -1)
        trPen = svmapP->penalty.word;
      trPen += svmapP->lz * svmapP->lvxSA[nodeP->svX];
      */

      while (linkP) {
	LCT*     aS = linkP->parentP->lct.A;
	int   nodeS = idxA[linkP->parentP->nodeX+1];
	int lctX, lctY;
	float trS, lmS, wpS, fpS;
	trS = lmS = wpS = fpS = LMSCORE_MAX;

	for (lctX = 0; lctX < nodeP->lct.N; lctX++) {
	  for (lctY = 0; lctY < linkP->parentP->lct.N; lctY++) {
	    /*
	    float trScore = (nodeP->nodeX <  0 || linkP->parentP->nodeX < 0)
	                  ? 0.0 : trPen;
	    */
	    float trScore = trPen, wdScore = wordPenScore, filScore = fillPenScore;
	    if (nodeP->nodeX < 0 || linkP->parentP->nodeX < 0) {
	      trScore = 0.0; wdScore = 0.0; filScore = 0.0;
	    }

	    float lmScore = (nodeP->nodeX >= 0            &&
			     linkP->parentP->nodeX != -1  &&
			     !swordP[nodeP->svX].fillerTag)
	                  ? UnCompressScore (lksP->scoreFct (lksP, aS[lctY], lvxE, 0))
	                  : 0.0;
	    LCT         e = aE[lctX];
	    LCT         s = (swordP[nodeP->svX].fillerTag)
	                  ? aS[lctY]
	                  : lksP->extendLCT (lksP, aS[lctY], lvxE);

	    /* In order to guarantee at least one transition in the resulting
	       lattice for every link in our current lattice, I insert the one
	       for lctX = 0 || lctY = 0, but with a bad language
	       model score (if a better one can't be found) */
	    if (!lctX && !lctY) {
	      lmS = lmScore;  trS = trScore;  wpS = wdScore;  fpS = filScore;
	    }

	    if (s == e) {
	      lmS = trS = LMSCORE_MAX;
	      printSLFLink (fp, idx++, nodeS+lctY, nodeE+lctX,
			    nodeP->score+linkP->score, lmScore, trScore,
			    svmapP->lz*LMSCORE_SCALE_FACTOR, wdScore, filScore);
	    }
	  }
	}
	if (trS != LMSCORE_MAX && lmS != LMSCORE_MAX)
	  printSLFLink (fp, idx++, nodeS, nodeE,
			nodeP->score+linkP->score, lmS, trS,
			svmapP->lz*LMSCORE_SCALE_FACTOR, wpS, fpS);
	  
	linkP = linkP->txenP;
      }
    }
  }

  fprintf (fp, "#\n# End of '%s'.\n#\n", utt);

  if (idx != linkN)
    WARN ("glatWriteSLF: unequal number of links: %d != %d.\n", linkN, idx);

  free (idxA);

  itfAppendResult ("{%d %d} {%d %d} %f", latP->nodeN, nodeN, latP->linkN, linkN,
		   1.0*(nodeN+linkN)/(latP->nodeN+latP->linkN));

  return TCL_OK;
}


static int glatWriteItf (ClientData cd, int argc, char *argv[])
{
  GLat  *latP    = (GLat*) cd;
  int        ac  = argc-1;
  float t_offset = 0.0;
  char    *file  = NULL;
  char    *utt   = "?";
  char    *fmt   = "njd";
  char    *mode  = NULL;
  FILE      *fp  = NULL;
  SVMap  *svmapP = latP->svmapP;

  if ( itfParseArgv( argv[0], &ac, argv+1, 0,
      "<file>",  ARGV_STRING, NULL, &file,   NULL,    "file to write to",
      "-format", ARGV_STRING, NULL, &fmt,    NULL,    "file format (njd or slf)",
      "-utt",    ARGV_STRING, NULL, &utt,    NULL,    "utterance ID (optional)",
      "-mode",   ARGV_STRING, NULL, &mode,   NULL,    "mode (njd:0-7|[a][b][g][l], slf: comment)",
      "-map",    ARGV_OBJECT, NULL, &svmapP, "SVMap", "Vocabulary Mapper (for SLF)",
      "-from",   ARGV_FLOAT,  NULL, &t_offset, NULL,  "Start time of this utterance",
       NULL) != TCL_OK) return TCL_ERROR;
  
  if (!(fp = fileOpen (file, "w"))) {
    WARN ("Cannot open '%s' for writing, aborting.\n", file);
    return TCL_ERROR;
  }

  /* Make sure our lattice is in a reasonable state */
  if (latP->nodeM <= 0 || latP->nodeN <= 0) {
    INFO ("glatWriteItf: empty lattice, clearing.\n");
    glatClear (latP);
  }

  if      (streq (fmt, "njd"))
    glatWriteNJD   (latP, fp, utt, mode);
  else if (streq (fmt, "slf")) {
    if (svmapP) glatWriteSLF (latP, fp, utt, mode, svmapP, t_offset);
    else        ERROR ("Cannot write lattice with LM information without SVMap object.\n");
  } else
    INFO ("Format '%s' not yet implemented, sorry for any inconvenience.\n", fmt);

  fileClose (file, fp);

  return TCL_OK;
}


/* ------------------------------------------------------------------------
    gnodePuts
   ------------------------------------------------------------------------ */

static void gnodePuts (GNode* fromP, SVMap* svmapP, int putsLinks, int norm)
{
  SVocab* svocabP = svmapP->svocabP;
  SWord*   swordP = svocabP->list.itemA; 
  GLink*    linkP = fromP->childP;
  int       linkX = 0;
  double    gamma = (norm) ? exp(-fromP->gamma) : fromP->gamma;


  if (fromP->nodeX >= 0) {
    itfAppendResult("node %d : %s frame= %d %d score= %2.2f  alpha= %2.2f  beta= %2.2f gamma= %2.2f\n",
		    fromP->nodeX,swordP[fromP->svX].name,fromP->frameS,fromP->frameE,
		    fromP->score,fromP->alpha,fromP->beta,gamma);
  } 
  if (fromP->nodeX == -1) {
    itfAppendResult("node %d : START frame= %d %d score= %2.2f  alpha= %2.2f  beta= %2.2f  gamma= %2.2f\n",
		    fromP->nodeX,fromP->frameS,fromP->frameE,fromP->score,
		    fromP->alpha,fromP->beta,gamma);
  }
  if (fromP->nodeX == -2) {
    itfAppendResult("node %d : END frame= %d %d score= %2.2f  alpha= %2.2f  beta= %2.2f  gamma= %2.2f\n",
		    fromP->nodeX,fromP->frameS,fromP->frameE,fromP->score,
		    fromP->alpha,fromP->beta,gamma); 

    if (! putsLinks) return;
    linkP = fromP->parentP;
    while (linkP) {
      GNode*    toP = linkP->parentP;
      itfAppendResult( "   link  %d : from = %s frame= %d %d score= %2.2f node= %d\n",
		       linkX,swordP[toP->svX].name,toP->frameS,toP->frameE,linkP->score,
		       toP->nodeX);
      linkP = linkP->txenP;
      linkX++;
    }
    return;
  }

  if (! putsLinks) return;

  while (linkP) {
    GNode*    toP = linkP->childP;
    if (toP->nodeX == -2) {
      itfAppendResult( "   link  %d : to = END frame= %d %d score= %2.2f node= %d\n",
		       linkX,toP->frameS,toP->frameE,linkP->score,
		       toP->nodeX);
    } else {
      if (toP->childP || svmapGet(svmapP,toP->svX) == svmapP->idx.end) {
	itfAppendResult( "   link  %d : to = %s frame= %d %d score= %2.2f node= %d\n",
			 linkX,swordP[toP->svX].name,toP->frameS,toP->frameE,linkP->score,
			 toP->nodeX);
      }
    }

    linkP = linkP->nextP;
    linkX++;
  }

  return;
}

static int glatPutsItf (ClientData cd, int argc, char *argv[])
{
  GLat* latP = (GLat*) cd;
  int     ac = argc-1;
  int  putsLinks =  1;
  int      nodeX = -3;
  int       norm = 0;

  if (latP->nodeN == 0 || latP->nodeM == 0) return TCL_OK;

  if ( itfParseArgv( argv[0], &ac, argv+1, 0,
      "-nodeX",ARGV_INT,  NULL, &nodeX,    NULL,"puts only nodeX (start = -1, end = -2)",
      "-link", ARGV_INT,  NULL, &putsLinks,NULL,"puts links",
      "-norm", ARGV_INT,  NULL, &norm,     NULL,"probs instead of log-probs for posteriories",
       NULL) != TCL_OK) return TCL_ERROR;

  if (nodeX > latP->nodeN) {
    ERROR("glatPutsItf: Invalid nodeX %d (should be below %d)\n",nodeX,latP->nodeN);
    return TCL_ERROR;
  }
  if (nodeX > -3) {
    if (latP->mapping.x2n[nodeX])
      gnodePuts(latP->mapping.x2n[nodeX],latP->svmapP,putsLinks,norm);
    return TCL_OK;
  }
  for (nodeX=-2;nodeX<latP->nodeM;nodeX++)
    if (latP->mapping.x2n[nodeX])
      gnodePuts(latP->mapping.x2n[nodeX],latP->svmapP,putsLinks,norm);

  return TCL_OK;
}

/* ------------------------------------------------------------------------
    glatConfigure
   ------------------------------------------------------------------------ */

static int glatConfigureItf(ClientData cd, char *var, char *val) {
  GLat* latP   = (GLat*)cd; 
  if (! latP) return TCL_ERROR;
  if (! var) {
    ITFCFG(glatConfigureItf,cd,"name");
    ITFCFG(glatConfigureItf,cd,"useN");
    ITFCFG(glatConfigureItf,cd,"nodeN");
    ITFCFG(glatConfigureItf,cd,"linkN");
    ITFCFG(glatConfigureItf,cd,"topN");
    ITFCFG(glatConfigureItf,cd,"alphaBeam");
    ITFCFG(glatConfigureItf,cd,"singularLCT");
    ITFCFG(glatConfigureItf,cd,"expert");
    ITFCFG(glatConfigureItf,cd,"status");
    ITFCFG(glatConfigureItf,cd,"frameShift");
    return TCL_OK;
  }
  ITFCFG_CharPtr(var,val,"name",      latP->name,          1);
  ITFCFG_Int(var,val,"useN",          latP->useN,          1);
  ITFCFG_Int(var,val,"nodeN",         latP->nodeN,         1);
  ITFCFG_Int(var,val,"linkN",         latP->linkN,         1);
  ITFCFG_Int  (var,val,"topN",        latP->beam.nodeN,    0);
  ITFCFG_Float(var,val,"alphaBeam",   latP->beam.alpha,    0);
  ITFCFG_Int  (var,val,"singularLCT", latP->singularLCT,   0);
  ITFCFG_Int  (var,val,"expert",      latP->expert,        0);
  ITFCFG_Float(var,val,"frameShift",  latP->frameShift,    0);
  ITFCFG_CharPtr(var,val,"status",    lat_status[latP->status], 1);

  ERROR("unknown option '-%s %s'.\n", var, val ? val : "?");
  return TCL_ERROR;   
}

/* ------------------------------------------------------------------------
    glatAccess
   ------------------------------------------------------------------------ */

static ClientData glatAccessItf (ClientData clientData, char *name, TypeInfo **ti)
{
  GLat* glatP = (GLat*) clientData;
  if (*name == '.') { 
    if (name[1]=='\0') {
      itfAppendElement( "nodeMem");
      itfAppendElement( "linkMem");
      itfAppendElement( "rcmMem");
      itfAppendElement( "pathMem");
      if (glatP->mem.tables)
	itfAppendElement( "tableMem");
      itfAppendElement( "lctMem");
      *ti = NULL; return NULL;
    } else { 
      if (! strcmp( name+1, "nodeMem")) {
        *ti = itfGetType("BMem");
        return (ClientData)glatP->mem.node;
      }
      if (! strcmp( name+1, "linkMem")) {
        *ti = itfGetType("BMem");
        return (ClientData)glatP->mem.link;
      }
      if (! strcmp( name+1, "rcmMem")) {
        *ti = itfGetType("BMem");
        return (ClientData)glatP->mem.rcm;
      }
      if (! strcmp( name+1, "pathMem")) {
        *ti = itfGetType("BMem");
        return (ClientData)glatP->mem.paths;
      }
      if (! strcmp( name+1, "tableMem") && glatP->mem.tables) {
        *ti = itfGetType("BMem");
        return (ClientData)glatP->mem.tables;
      }
      if (! strcmp( name+1, "lctMem")) {
        *ti = itfGetType("BMem");
        return (ClientData)glatP->mem.lct;
      }
    }
  }
  return NULL;
}

/* forward declaration */
extern int glatInitAddonItf( ClientData clientData, int argc, char *argv[]);
extern int glatClearAddonItf(ClientData clientData, int argc, char *argv[]);

/* ========================================================================
    Type Information
   ======================================================================== */

Method glatMethod[] = 
{
  { "puts",        glatPutsItf,              "displays the contents of a lattice"     },
  { "read",        glatReadItf,              "read a lattice from file"               },
  { "write",       glatWriteItf,             "write a lattice to file"                },
  { "clear",       glatClearItf,             "clear lattice"                          },
  { "purify",      glatPurifyItf,            "delete non-terminating nodes and links" },
  { "compress",    glatCompactItf,           "compress lattice"                       },
  { "recombine",   glatRecombineItf,         "recombine lattice nodes with equal LCT" },
  { "posteriori",  glatComputeGammaItf,      "compute a-posteriori probabilities"     },
  { "initAddon",   glatInitAddonItf,         "initialize glat addon vars"             },
  { "clearAddon",  glatClearAddonItf,        "clear glat addon vars"                  },
  { "confidence",  glatComputeConfidenceItf, "compute confidence measure"             },
  { "connect",     glatConnectItf,           "connect matching nodes"                 },
  { "prune",       glatPruneItf,             "prune lattice nodes"                    },
  { "rescore",     glatRescoreItf,           "rescore a lattice using svMap"          },
  { "align",       glatAlignItf,             "align lattice with reference"           },
  { "addNode",     glatAddNodeItf,           "add a node to a lattice"                },
  { "delNode",     glatDeleteNodeItf,        "delete a node from a lattice"           },
  { "addLink",     glatAddLinkItf,           "add a link to a lattice"                },
  { "delLink",     glatDeleteLinkItf,        "delete a link from a lattice"           },
  { "addPath",     glatAddPathItf,           "add a path to a lattice"                },
  { "map",         glatMapItf,               "map vocabulary words in lattice nodes"  },
  { "splitMW",     glatSplitItf,             "split nodes which contain multiwords"   },
  { "warp",        glatWarpItf,              "warp (scale) time axis"                 },
  { "singularLCT", glatExpandItf,            "expand the lattice with respect to LCT" },
  { "consensus",   glatConsensusItf,         "find consensus in lattice(s)"           },
  { NULL, NULL, NULL }
} ;

TypeInfo glatInfo = 
{ "GLat", 0, -1,  glatMethod, 
   glatCreateItf, glatFreeItf, 
   glatConfigureItf, glatAccessItf, NULL,
  "Generic Lattice (pronounced 'Gillette, everything a man ...')"
};

static int glatInitialized = FALSE;

int GLat_Init (void)
{
  if (! glatInitialized) {
    if ( SVocab_Init() != TCL_OK) return TCL_ERROR;

    glatDefault.align.delCost = 1.0;
    glatDefault.align.insCost = 1.0;
    glatDefault.align.subCost = 1.0;

    glatDefault.beam.nodeN    = 0;
    glatDefault.beam.alpha    = 150.0;

    itfNewType (&glatInfo);
    glatInitialized = TRUE;
  }
  return TCL_OK;
}
