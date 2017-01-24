/* ========================================================================
    JANUS      Speech- to Speech Translation
               ------------------------------------------------------------
               Object: Search Vocabulary Mapper
               ------------------------------------------------------------

    Author  :  Florian Metze
    Module  :  svmap.c
    Date    :  $Id: svmap.c 3282 2010-07-02 16:32:19Z metze $

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
    Revision 4.8  2007/03/19 15:12:51  fuegen
    made start, end and unk string configurable

    Revision 4.7  2007/02/22 16:50:02  fuegen
    svmapConfigure now uses default object when nothing is given

    Revision 4.6  2005/07/13 08:57:41  stueker
    mappedTo method now only outputs the list of svocab words that are mapped to a specific lm word; not the lvx of the target word

    Revision 4.5  2005/03/04 09:16:39  metze
    Extra warning

    Revision 4.4  2004/05/27 11:30:47  metze
    Cleaned up 'unk' word, interface

    Revision 4.3  2003/12/17 16:04:25  fuegen
    svmapGetPuts returns again LVX_NIL instead of and error for
    not already mapped words

    Revision 4.2  2003/11/06 17:45:48  metze
    CLeaned up vocab mapping

    Revision 4.1  2003/08/14 11:20:09  fuegen
    Merged changes on branch jtk-01-01-15-fms (jaguar -> ibis-013)

    Revision 1.1.2.70  2003/06/06 11:53:26  soltau
    Fixed scores for words mapped to <UNK>

    Revision 1.1.2.69  2003/06/03 14:52:00  metze
    Avoid integer overflow

    Revision 1.1.2.68  2003/05/08 16:41:56  soltau
    Purified!

    Revision 1.1.2.67  2003/03/12 10:44:18  metze
    Memory alignment fixed in bmemAlloc, SVX types fixed for reading

    Revision 1.1.2.66  2003/03/09 11:31:56  soltau
    svmapClearCache: check for calls

    Revision 1.1.2.65  2003/03/08 17:56:51  soltau
    Revert to previuos version

    Revision 1.1.2.63  2003/02/05 18:25:53  soltau
    svmapLoad: added casts

    Revision 1.1.2.62  2003/01/17 10:03:45  soltau
    Added svmapBuildRevMap

    Revision 1.1.2.61  2003/01/15 14:35:10  metze
    Corrected typo

    Revision 1.1.2.60  2002/11/25 17:37:45  metze
    Made 256 Gaussians per Codebook possible

    Revision 1.1.2.59  2002/11/21 17:10:12  fuegen
    windows code cleaning

    Revision 1.1.2.58  2002/11/19 11:27:15  soltau
    svmapBasicMap    : reduced verbosity
    svmapMappedToItf : checking for unk

    Revision 1.1.2.57  2002/11/18 17:40:01  fuegen
    removed warning missing lm words in svmapGetPuts

    Revision 1.1.2.56  2002/11/15 13:16:04  metze
    Removed superfluous output

    Revision 1.1.2.55  2002/11/14 15:45:16  fuegen
    added list like access function, check for filler words in svmapGet

    Revision 1.1.2.54  2002/11/13 09:57:45  soltau
    Start and End words rely on LVX now
    Changes for optional words

    Revision 1.1.2.53  2002/10/17 13:22:58  soltau
    code cleaning

    Revision 1.1.2.52  2002/10/07 07:34:28  soltau
    svmapAddItf: delete svX = unkIdx checking

    Revision 1.1.2.51  2002/09/02 16:26:32  soltau
    svmapLoad/Save : improved handling of differend svx/lvx typedefs

    Revision 1.1.2.50  2002/08/27 08:49:29  metze
    itfParseArgv now uses <name> and NULL initialization for char*

    Revision 1.1.2.49  2002/08/01 13:42:32  metze
    Fixes for clean documentation.

    Revision 1.1.2.48  2002/06/26 11:57:55  fuegen
    changes for new directory structure support and code cleaning

    Revision 1.1.2.47  2002/06/18 16:44:56  metze
    Changed order of checks for dirty cache

    Revision 1.1.2.46  2002/06/13 15:07:07  soltau
    svmapExtMap : removed prehistoric endSVX checking

    Revision 1.1.2.45  2002/06/13 08:07:25  metze
    Added dirty as a function in LMs

    Revision 1.1.2.44  2002/06/12 10:10:10  metze
    Fixed WARNINGS

    Revision 1.1.2.43  2002/06/10 13:26:48  metze
    Added 'dirty' tag and svmapIsDirty

    Revision 1.1.2.42  2002/06/10 09:59:55  soltau
    svmapLoad/svmapSave : LVX -> lmScore

    Revision 1.1.2.41  2002/05/02 12:53:21  metze
    Bugfix in interface

    Revision 1.1.2.40  2002/04/29 13:29:38  metze
    Redesign of LM interface (LingKS)

    Revision 1.1.2.39  2002/04/26 14:51:48  soltau
    cleaned svmapConfigure

    Revision 1.1.2.38  2002/04/26 11:44:47  soltau
    svmapInitLMWords : improved checking of svocab words
    svmapRead        : reimplemented to avoid freadAdd
    svmapReadSubs    : renamed

    Revision 1.1.2.37  2002/04/17 08:15:59  metze
    Improved 'map base' behaviour

    Revision 1.1.2.36  2002/04/08 14:05:40  soltau
    svmapInit: check for SVX/LVX typedefs

    Revision 1.1.2.35  2002/04/08 11:53:35  metze
    Added check for base probabilities

    Revision 1.1.2.34  2002/02/14 18:32:14  soltau
    *** empty log message ***

    Revision 1.1.2.33  2002/02/14 12:03:14  soltau
    Added svmapClearCache

    Revision 1.1.2.32  2002/02/13 13:48:25  soltau
    Added svmapGetLMScore

    Revision 1.1.2.31  2002/01/21 10:20:18  soltau
    Improved checking for start/end map entries

    Revision 1.1.2.30  2002/01/19 11:40:40  metze
    Added whole bunch of convenience interface function
    Extra configuration options
    Checking for "invalid" LM words

    Revision 1.1.2.29  2002/01/18 11:40:36  soltau
    Cleared 'special treatment' table,
    Cleared svmapGet/Add/Del
    Added symbolic names in svmapBasicMap

    Revision 1.1.2.28  2002/01/17 17:08:44  soltau
    svmapLoad: set svN

    Revision 1.1.2.27  2002/01/17 16:29:36  soltau
    svmapInit      : added svmapAllocTable
    svmapBasicMap  : checking for inefficient configuration
    svmapBasicMap  : forced adding of mandatory words in underlying svocab
    svmapDelMap    : fixed deletion of words from the 'lower' range
    svmapDoMapping : support for deleted map entries
    svmapDoMapping : support for inefficient configuration

    Revision 1.1.2.26  2001/11/30 14:59:53  soltau
    Changes according to svocabAdd

    Revision 1.1.2.25  2001/11/21 10:32:31  soltau
    Added Access to svocab and lks

    Revision 1.1.2.24  2001/11/07 15:58:00  metze
    Added configure method to get back baseLM and baseVocab

    Revision 1.1.2.23  2001/10/02 18:19:53  metze
    Made useN state visible

    Revision 1.1.2.22  2001/10/02 17:55:20  metze
    Finally added linking/ delinking with Language Model

    Revision 1.1.2.21  2001/09/24 14:09:54  soltau
    Removed annoying warning (cannot link LingKS)

    Revision 1.1.2.20  2001/08/08 18:07:46  fuegen
    display now verbose output of to UNK mapped words in all modes

    Revision 1.1.2.19  2001/08/08 14:55:09  metze
    Checked for filler tags

    Revision 1.1.2.18  2001/08/08 14:16:58  metze
    Added 'svmap map 2' to map variants to baseform's class

    Revision 1.1.2.17  2001/07/31 18:29:26  metze
    Checked all the Itf routines,

    Revision 1.1.2.16  2001/07/30 15:19:45  soltau
    map unknown words to <UNK> and not to LVX_NIL

    Revision 1.1.2.15  2001/07/26 17:37:20  soltau
    Fixed lnN settings if read binaries

    Revision 1.1.2.14  2001/07/26 08:19:03  metze
    Typo corrected

    Revision 1.1.2.13  2001/07/25 15:35:33  metze
    Added warning for non-existing mapping

    Revision 1.1.2.12  2001/07/12 18:39:13  soltau
    Added svmapInitLMWords

    Revision 1.1.2.11  2001/06/28 10:48:23  soltau
    Added Load/save routines for binary files to support fast decoder startup

    Revision 1.1.2.10  2001/06/18 07:59:30  metze
    svmapGet only returns the index, not the score

    Revision 1.1.2.9  2001/06/11 14:34:01  soltau
    *** empty log message ***

    Revision 1.1.2.8  2001/06/11 14:09:00  soltau
    svmapDel : map to identity for deleted map entries
    svmapGet : fixed return value

    Revision 1.1.2.7  2001/06/07 19:40:11  soltau
    exported svmapGet

    Revision 1.1.2.6  2001/06/01 10:08:33  metze
    Made changes necessary for decoding along lattices

    Revision 1.1.2.5  2001/05/25 11:44:10  metze
    Bugfix in LMMap

    Revision 1.1.2.4  2001/05/23 08:17:20  metze
    Removed ruins of attempts to include multi-words here

    Revision 1.1.2.3  2001/05/11 16:40:58  metze
    Cleaned up LCT interface

    Revision 1.1.2.2  2001/05/11 09:46:16  metze
    Moved lz/lp to svMap

    Revision 1.1.2.1  2001/04/27 07:37:58  metze
    Separated svocab and svmap


   ======================================================================== */

#include "itf.h"
#include "svocab.h"

#include "lks.h"
#include "mach_ind_io.h"


char svmapRCSVersion[]="$Id: svmap.c 3282 2010-07-02 16:32:19Z metze $";


/* ------------------------------------------------------------------------
    forward declarations 
   ------------------------------------------------------------------------ */

extern TypeInfo  svmapInfo;
static SVMap     svmapDefault;

extern int svmapGetPuts (SVMap *svmapP, char *name);

/* ========================================================================
    Search Vocabulary Mapper
   ======================================================================== */

/* ------------------------------------------------------------------------
    svmapAllocCache
   ------------------------------------------------------------------------ */

static int svmapAllocCache (SVMap* svmapP, int N)
{
  int svX,lctX;

  if (svmapP->cache.lA) free(svmapP->cache.lA);
  if (svmapP->cache.sA) free(svmapP->cache.sA);
  if (svmapP->cache.cA) free(svmapP->cache.cA);

  svmapP->cache.lA = NULL;
  svmapP->cache.sA = NULL;
  svmapP->cache.cA = NULL;

  svmapP->cache.calls = 0;
  svmapP->cache.hits  = 0;
  svmapP->cache.N     = N;

  if (svmapP->cache.N == 0 || svmapP->svN == 0) return TCL_OK;

  if (NULL == (svmapP->cache.lA=malloc(svmapP->svN*N*sizeof(LCT)))) {
    ERROR("svmapAllocCache: allocation failure\n");
    return TCL_ERROR;
  }
  if (NULL == (svmapP->cache.sA=malloc(svmapP->svN*N*sizeof(lmScore)))) {
    ERROR("svmapAllocCache: allocation failure\n");
    return TCL_ERROR;
  }
  if (NULL == (svmapP->cache.cA=malloc(svmapP->svN*sizeof(int)))) {
    ERROR("svmapAllocCache: allocation failure\n");
    return TCL_ERROR;
  }
  if (NULL == (svmapP->cache.xA=malloc(svmapP->svN*sizeof(int)))) {
    ERROR("svmapAllocCache: allocation failure\n");
    return TCL_ERROR;
  }
  for (svX=0;svX<svmapP->svN;svX++) {
    lmScore* sA = svmapP->cache.sA + svX*N;
    LCT*     lA = svmapP->cache.lA + svX*N;
    svmapP->cache.cA[svX]=0;
    svmapP->cache.xA[svX]=0;
    for (lctX=0;lctX<N;lctX++) {
      sA[lctX] = LMSCORE_MAX;
      lA[lctX] = LCT_NIL;
    }
  }
  return TCL_OK;
}

int svmapClearCache (SVMap* svmapP)
{
  int svX,lctX;
  if (svmapP->cache.N ==0 || svmapP->cache.calls == 0) return TCL_OK;

  svmapP->cache.calls = 0;
  svmapP->cache.hits  = 0;

  for (svX=0;svX<svmapP->svN;svX++) {
    lmScore* sA = svmapP->cache.sA + svX*svmapP->cache.N;
    LCT*     lA = svmapP->cache.lA + svX*svmapP->cache.N;
    svmapP->cache.cA[svX]=0;
    svmapP->cache.xA[svX]=0;
    for (lctX=0;lctX<svmapP->cache.N;lctX++) {
      sA[lctX] = LMSCORE_MAX;
      lA[lctX] = LCT_NIL;
    }
  }
  return TCL_OK;
}

/* ------------------------------------------------------------------------
    svmapAllocTable
   ------------------------------------------------------------------------ */

static int svmapAllocTable(SVMap* svmapP) {
  SVX svN = svmapP->svocabP->list.itemN;
  SVX svX = SVX_NIL;

  if (svN == (SVX) svmapP->svN) return TCL_OK;

  if (NULL == (svmapP->lvxA  = realloc(svmapP->lvxA,svN*sizeof(LVX)))) {
    ERROR("svmapAllocTable: allocation failure\n");
    return TCL_ERROR;
  }
  if (NULL == (svmapP->lvxSA = realloc(svmapP->lvxSA,svN*sizeof(lmScore)))) {
    ERROR("svmapAllocTable: allocation failure\n");
    return TCL_ERROR;
  }
  for (svX=svmapP->svN; svX<svN; svX++) {
    svmapP->lvxA[svX] = LVX_NIL;
    svmapP->lvxSA[svX]= 0;
  }
  svmapP->svN = svN;

  svmapAllocCache (svmapP,svmapP->cache.N);

  return TCL_OK;
}

/* ------------------------------------------------------------------------
    svmapBuildRevMap
   ------------------------------------------------------------------------ */


typedef struct {
  SVX svX;
  LVX lvX;
} Pair;

int cmpPair(const void* a, const void* b) {
  LVX lvA = ( (Pair*) a)->lvX;
  LVX lvB = ( (Pair*) b)->lvX;
  SVX svA = ( (Pair*) a)->svX;
  SVX svB = ( (Pair*) b)->svX;
  if (lvA < lvB) return -1;
  if (lvA > lvB) return  1;
  if (svA < svB) return -1;
  if (svA > svB) return  1;
  return 0;
}

static int svmapBuildRevMap(SVMap* svmapP) {
  Pair* pairA = NULL;
  int     lnN = (int) svmapP->lksP->itemN(svmapP->lksP);
  int i;

  if (lnN == 0 || svmapP->svN == 0) return TCL_OK;

  if (svmapP->rev.N) free(svmapP->rev.N-1);
  if (svmapP->rev.X) free(svmapP->rev.X);

  if (NULL == (svmapP->rev.N= malloc((lnN+1)*sizeof(int)))) {
    ERROR("svmapBuildRevMap: allocation failure\n");
  }
  if (NULL == (svmapP->rev.X= malloc(svmapP->svN*sizeof(SVX)))) {
    ERROR("svmapBuildRevMap: allocation failure\n");
  }
  if (NULL == (pairA= malloc(svmapP->svN*sizeof(Pair)))) {
    ERROR("svmapBuildRevMap: allocation failure\n");
  }

  /* initialize tables */
  svmapP->rev.N[0]=0;
  svmapP->rev.N++;
  for (i=0;i<lnN;i++)         svmapP->rev.N[i] = -1;
  for (i=0;i<svmapP->svN;i++) svmapP->rev.X[i] = SVX_NIL;

  for (i=0;i<svmapP->svN;i++) {
    pairA[i].svX = i;
    pairA[i].lvX = svmapGet(svmapP,(SVX) i);
  }
  qsort(pairA,svmapP->svN,sizeof(Pair),cmpPair);

  for (i=0;i<svmapP->svN;i++) {
    SVX svX = pairA[i].svX;
    LVX lvX = pairA[i].lvX;

    if (lvX == LVX_NIL) continue;
    svmapP->rev.N[lvX]= i;
    svmapP->rev.X[i]  = svX;

  }

  /* check for lks-words without corresponding svocab words */
  for (i=0;i<lnN;i++) {
    if ( svmapP->rev.N[i] == -1)
       svmapP->rev.N[i] =  svmapP->rev.N[i-1];
  }

  if (pairA) free(pairA);

  return TCL_OK;
}

/* ------------------------------------------------------------------------
    svmapInitLMWords
   ------------------------------------------------------------------------ */

int svmapInitLMWords (SVMap* svmapP)
{
  LingKS*    lksP = svmapP->lksP;
  SVocab* svocabP = svmapP->svocabP;
  LVX         lvN =  lksP->itemN (lksP);
  int        mode = (svmapP->svocabP->dictP->list.itemN > 0) ? 1 : 0;
  LVX         lvX = LVX_NIL;

  for (lvX = 0; lvX < lvN; lvX++) {
    char *name = lksP->itemA (lksP, lvX);

    if (streq (name, svmapP->string.unk)) {
      if (lvX != svocabAdd(svocabP,svmapP->string.unk, 0,0,-1)) break;
      continue;
    }
    if (streq (name, svmapP->string.start)) {
      if (lvX != svocabAdd(svocabP,svocabP->string.start,0,mode,-1)) break;
      continue;
    }
    if (streq (name, svmapP->string.end)) {
      if (lvX != svocabAdd(svocabP,svocabP->string.end,0,mode,-1)) break;
      continue;
    }
    if (lvX != (svocabAdd (svocabP, name, 0, 0, -1))) {
      WARN ("svmapInitLMWords: dubious mapping for lvX %d '%s'\n", lvX, name);
      break;
    }
  }

  svmapP->lnN = lvX;
  if (svmapP->lnN !=  lvN) 
    WARN ("svmapInitLMWords: established bijective map for %d of %d words only\n",
	  svmapP->lnN, lvN);

  svmapAllocTable(svmapP);

  svmapP->idx.start = lksP->itemX(lksP,svmapP->string.start);
  svmapP->idx.end   = lksP->itemX(lksP,svmapP->string.end);
  svmapP->idx.unk   = lksP->itemX(lksP,svmapP->string.unk);

  svmapP->dirty = TRUE;

  return TCL_OK;
}

/* ------------------------------------------------------------------------
    svmapInit
   ------------------------------------------------------------------------ */

static int svmapInit (SVMap* svmapP, char* name, SVocab* svocabP, LingKS* lksP)
{
  if ( sizeof(SVX) < sizeof(LVX) || sizeof(SVX) > sizeof(int) || 
       sizeof(LVX) > sizeof(int)) {
    ERROR("svmapInit: Invalid SVX/LVX typedefs\n");
    return TCL_ERROR;
  }

  svmapP->name    = strdup (name);
  svmapP->useN    = 0;
  svmapP->svocabP = svocabP;
  svmapP->lksP    = lksP;
  svmapP->svN     = 0;
  svmapP->lnN     = 0;
  svmapP->lvxA    = NULL;
  svmapP->lvxSA   = NULL;

  svmapP->XTbl.svN   = 0;
  svmapP->XTbl.svXA  = NULL;

  svmapP->rev.N = NULL;
  svmapP->rev.X = NULL;

  svmapP->lz            = svmapDefault.lz;
  svmapP->lalz          = svmapDefault.lalz;
  svmapP->penalty.phone = svmapDefault.penalty.phone;
  svmapP->penalty.word  = svmapDefault.penalty.word;
  svmapP->penalty.fil   = svmapDefault.penalty.fil;
 
  svmapP->string.start = strdup(svmapDefault.string.start);
  svmapP->string.end   = strdup(svmapDefault.string.end);
  svmapP->string.unk   = strdup(svmapDefault.string.unk);

  svmapP->cache.N     = 0;
  svmapP->cache.sA    = NULL;
  svmapP->cache.lA    = NULL;
  svmapP->cache.cA    = NULL;
  svmapP->cache.xA    = NULL;
  svmapP->cache.hits  = 0;
  svmapP->cache.calls = 0;

  svmapP->dirty       = FALSE;

  link (svocabP,"SVocab");
  link (lksP,   "LingKS");

  svmapInitLMWords(svmapP);  

  return TCL_OK;
}

/* ------------------------------------------------------------------------
    svmapCreate
   ------------------------------------------------------------------------ */

static SVMap* svmapCreate (char* name, SVocab* svocabP, LingKS *lksP)
{
  SVMap *svmapP = (SVMap*) malloc (sizeof(SVMap));

  if (!svmapP) {
    ERROR("svmapCreate: allocation failure\n");
    return NULL;
  }

  if (TCL_ERROR == (svmapInit (svmapP, name, svocabP, lksP))) {    
    free (svmapP);
    return NULL;
  }

  return svmapP;
}

static ClientData svmapCreateItf( ClientData clientData, 
                                      int argc, char *argv[])
{
  char*      name = NULL;
  SVocab* svocabP = NULL;
  LingKS*    lksP;

  if ( itfParseArgv( argv[0], &argc, argv, 0,
      "<name>",  ARGV_STRING, NULL, &name, NULL,         "name of the SVMap",
      "<SVocab>", ARGV_OBJECT, NULL, &svocabP, "SVocab", "Search Vocabulary",
      "<LingKS>", ARGV_OBJECT, NULL, &lksP,    "LingKS", "Linguistic Knowledge Source",
       NULL) != TCL_OK) return NULL;

  return (ClientData) svmapCreate (name, svocabP, lksP);
}


/* ------------------------------------------------------------------------
    svmapConfigure
   ------------------------------------------------------------------------ */

static int svmapConfigureItf (ClientData cd, char *var, char *val)
{
  SVMap* svmapP = (SVMap*) cd;

  if (!svmapP) {
    svmapP = &svmapDefault;

    if (!var) {
      ITFCFG (svmapConfigureItf, cd, "startString");
      ITFCFG (svmapConfigureItf, cd, "endString");
      ITFCFG (svmapConfigureItf, cd, "unkString");
      return TCL_OK;
    }

    ITFCFG_CharPtr (var, val, "startString", svmapP->string.start, 0);
    ITFCFG_CharPtr (var, val, "endString",   svmapP->string.end,   0);
    ITFCFG_CharPtr (var, val, "unkString",   svmapP->string.unk,   0);

    ERROR ("Unknown option '-%s %s'.\n", var, val ? val : "?");

    return TCL_ERROR;
  }

  svmapAllocTable(svmapP);

  if (!var) {
    ITFCFG (svmapConfigureItf, cd, "name");
    ITFCFG (svmapConfigureItf, cd, "useN");
    ITFCFG (svmapConfigureItf, cd, "phonePen");
    ITFCFG (svmapConfigureItf, cd, "wordPen");
    ITFCFG (svmapConfigureItf, cd, "filPen");
    ITFCFG (svmapConfigureItf, cd, "lz");
    ITFCFG (svmapConfigureItf, cd, "lalz");
    ITFCFG (svmapConfigureItf, cd, "baseVocab");
    ITFCFG (svmapConfigureItf, cd, "baseLM");
    ITFCFG (svmapConfigureItf, cd, "svN");
    ITFCFG (svmapConfigureItf, cd, "lvN");
    ITFCFG (svmapConfigureItf, cd, "xN");
    ITFCFG (svmapConfigureItf, cd, "startString");
    ITFCFG (svmapConfigureItf, cd, "endString");
    ITFCFG (svmapConfigureItf, cd, "unkString");
    ITFCFG (svmapConfigureItf, cd, "cacheN");
    ITFCFG (svmapConfigureItf, cd, "hits");
    ITFCFG (svmapConfigureItf, cd, "calls");
    ITFCFG (svmapConfigureItf, cd, "dirty");
    return TCL_OK;
  }

  ITFCFG_CharPtr (var, val, "baseVocab",svmapP->svocabP->name, 1);
  ITFCFG_CharPtr (var, val, "baseLM",   svmapP->lksP->name,    1);
  ITFCFG_CharPtr (var, val, "name",     svmapP->name,          1);
  ITFCFG_Int     (var, val, "useN",     svmapP->useN,          1);
  ITFCFG_Int     (var, val, "svN",      svmapP->svN,           1);
  ITFCFG_Int     (var, val, "lvN",      svmapP->lnN,           1);
  ITFCFG_Int     (var, val, "xN",       svmapP->XTbl.svN,      1);
  ITFCFG_CharPtr (var, val, "startString", svmapP->string.start, 0);
  ITFCFG_CharPtr (var, val, "endString", svmapP->string.end,   0);
  ITFCFG_CharPtr (var, val, "unkString", svmapP->string.unk,   0);
  ITFCFG_Float   (var, val, "phonePen", svmapP->penalty.phone, 0);
  ITFCFG_Float   (var, val, "wordPen",  svmapP->penalty.word,  0);
  ITFCFG_Float   (var, val, "filPen",   svmapP->penalty.fil,   0);
  ITFCFG_Int     (var, val, "hits",     svmapP->cache.hits,    0);
  ITFCFG_Int     (var, val, "calls",    svmapP->cache.calls,   0);
  ITFCFG_Int     (var, val, "dirty",    svmapP->dirty,         1);
  if (streq (var, "lz")) {
    if (val) svmapP->lz = svmapP->lalz = -atof(val)/LMSCORE_SCALE_FACTOR;
    itfAppendResult (" %f", -svmapP->lz*LMSCORE_SCALE_FACTOR);
    return TCL_OK;
  }
  if (streq (var, "lalz")) {
    if (val) svmapP->lalz = -atof(val)/LMSCORE_SCALE_FACTOR;
    itfAppendResult (" %f", -svmapP->lalz*LMSCORE_SCALE_FACTOR);
    return TCL_OK;
  }
  if (streq (var, "cacheN")) {
    if (val) {
      svmapAllocCache(svmapP,atoi(val));
    }
    itfAppendResult (" %d",svmapP->cache.N);
    return TCL_OK;
  }

  ERROR ("Unknown option '-%s %s'.\n", var, val ? val : "?");

  return TCL_ERROR;
}

/* ------------------------------------------------------------------------
    svmapAccess
    ------------------------------------------------------------------------ */

static ClientData svmapAccessItf( ClientData cd, char *name, TypeInfo **ti)
{ 
  SVMap* svmapP  = (SVMap*)cd;
  if (*name == '.') { 
    if (name[1]=='\0') {
      itfAppendElement( "svocab");
      itfAppendElement( "lingks");
      *ti = NULL; return NULL;
    }
    else { 
      if (! strcmp( name+1, "svocab")) {
        *ti = itfGetType("SVocab");
        return (ClientData)svmapP->svocabP;
      }
      if (! strcmp( name+1, "lingks")) {
        *ti = itfGetType("LingKS");
        return (ClientData)svmapP->lksP;
      }
      return NULL;
    }
  }

  if (*name == ':') {
    SVX svX = 0;
    SVX svN = svmapP->svocabP->list.itemN;

    if (name[1]=='\0') {
        svmapAllocTable(svmapP);
	for (svX = 0; svX < svN; svX++) {
	  SWord *swordP = svmapP->svocabP->list.itemA+svX;
	  if (swordP->dictX != SVX_MAX && svmapP->lvxA[svX] != LVX_NIL)
	    itfAppendElement ("%s", swordP->name);
	}
    } else {
      name++;
      svmapGetPuts (svmapP, name);
    }
    *ti = NULL;
  }

  return NULL;
}

/* ------------------------------------------------------------------------
    svmapDeinit
   ------------------------------------------------------------------------ */

static int svmapDeinit(SVMap* svmapP) {

  delink (svmapP->svocabP,"SVocab");
  delink (svmapP->lksP,   "LingKS"); 

  if (svmapP->name)         free(svmapP->name);
  if (svmapP->lvxA)         free(svmapP->lvxA);
  if (svmapP->lvxSA)        free(svmapP->lvxSA);
  if (svmapP->XTbl.svXA)    free(svmapP->XTbl.svXA);
  if (svmapP->string.start) free(svmapP->string.start);
  if (svmapP->string.end)   free(svmapP->string.end);
  if (svmapP->string.unk)   free(svmapP->string.unk);

  if (svmapP->rev.N) free(svmapP->rev.N-1);
  if (svmapP->rev.X) free(svmapP->rev.X);

  svmapP->name         = NULL;
  svmapP->lvxA         = NULL;
  svmapP->lvxSA        = NULL;
  svmapP->XTbl.svXA    = NULL;
  svmapP->rev.N        = NULL;
  svmapP->rev.X        = NULL;
  svmapP->string.start = NULL;
  svmapP->string.end   = NULL;
  svmapP->string.unk   = NULL;
 
  svmapAllocCache(svmapP,0);
  return TCL_OK;
}

/* ------------------------------------------------------------------------
    svmapFree
   ------------------------------------------------------------------------ */

static int svmapFree(SVMap *svmapP)
{
  if (svmapP) {
    if ( svmapDeinit(svmapP) != TCL_OK) return TCL_ERROR;
    free(svmapP);
  }
  return TCL_OK;
}

static int svmapFreeItf (ClientData clientData)
{
  return svmapFree((SVMap*)clientData);
}

/* ------------------------------------------------------------------------
    svmapBasicMap : Create (inital) map between svX and lvX
   ------------------------------------------------------------------------ */

enum {
  idMap    = 0,
  pronMap  = 1,
  classMap = 2
};

static int svmapBasicMap(SVMap *svmapP, int mapFlag, int verbosity)
{
  LVX      lvN = svmapP->lnN;
  LVX      lvM = svmapP->lksP->itemN (svmapP->lksP);
  int   unkIdx = svmapP->lksP->itemX (svmapP->lksP, svmapP->string.unk);
  int NILcount = 0;
  SVX svX;
  
  assert(unkIdx >=0);
  
  if (lvN*4 < lvM) {
    /* Florian's Rule of Thumb: if you treat less than 25% of
       your pronunciation variants/ classes in the svMap, it's 
       probably going to be inefficient */
    WARN("svmapMap: detected inefficient mapper initilization.\n");
    INFO("svmapMap: -> load LingKS before creating SVMap.\n");
    INFO("svmapMap: -> load SVocab after  creating SVMap.\n");
  }

  svmapAllocTable(svmapP);

  if (mapFlag == classMap) {

    /* ---------------------------------------------- */
    /* case 3: map pronunciations to baseform's class */
    /* ---------------------------------------------- */

    for (svX = svmapP->lnN; svX < (SVX) svmapP->svocabP->list.itemN; svX++) {
      int lmIdx, svY;
      SWord* swordP = svmapP->svocabP->list.itemA + svX;  
	char* pronP = strchr(swordP->name+1,'(');  /* ) keeps emacs happy */

      if (pronP != NULL) {
	*pronP = '\0';
	svY = listIndex ((List*) &(svmapP->svocabP->list), swordP->name, 0);
	*pronP = '('; /* ) keeps emacs happy */
	if (svY != -1) {
	  if (svmapP->svocabP->list.itemA[(SVX) svY].fillerTag)
	    continue;
	  lmIdx = svmapGet (svmapP, (SVX) svY);
	  svmapP->lvxSA[svX] = svmapP->lvxSA[svY];
	  svmapP->lvxA[svX]  = (LVX) (lmIdx != LVX_NIL) ? lmIdx : unkIdx;

	} else if (verbosity)
	  WARN ("svmapMap: couldn't find baseform for '%s'.\n", swordP->name);
      }
    }

  } else {

    /* ---------------------------------------- */
    /* map all svX less than nr. of lmwords 1:1 */
    /* ---------------------------------------- */
    
    for (svX = 0; svX < (SVX) svmapP->lnN; svX++) {
      svmapP->lvxA[svX]  = (LVX) svX;
      svmapP->lvxSA[svX] = 0;
    }
    
    /* ------------------------------------------ */
    /* map all svX without corresponding lm words */
    /* ------------------------------------------ */

    for (svX = (SVX) svmapP->lnN; svX < (SVX) svmapP->svocabP->list.itemN; svX++) {
      SWord* swordP = svmapP->svocabP->list.itemA + svX;
      LVX lmIdx = unkIdx;
      
      svmapP->lvxA[svX]= unkIdx;
      if (swordP->fillerTag == 1) continue; 

      /* check for mandatory words */
      if (streq (swordP->name,svmapP->svocabP->string.start))
	lmIdx = svmapP->lksP->itemX (svmapP->lksP, svmapP->string.start);
      if (streq (swordP->name,svmapP->svocabP->string.end))
	lmIdx = svmapP->lksP->itemX (svmapP->lksP, svmapP->string.end);
      if (lmIdx != unkIdx) {
	svmapP->lvxSA[svX] = 0;
	svmapP->lvxA[svX]  = lmIdx;
	continue;
      }

      if (mapFlag == idMap) { 
	
	/* ------------------------------------ */
	/* case 1: No mapping of pronunciations */
	/* ------------------------------------ */
	lmIdx = svmapP->lksP->itemX (svmapP->lksP, swordP->name);
	svmapP->lvxSA[svX] = 0;
	svmapP->lvxA[svX]  = (lmIdx != LVX_NIL) ? lmIdx : unkIdx;      
	
      } else if (mapFlag == pronMap) {
	
	/* -------------------------------------- */
	/* case 2: map pronunciations to baseform */
	/* -------------------------------------- */
	char* pronP = strchr(swordP->name+1,'(');  /* ) keeps emacs happy */       

	lmIdx = svmapP->lksP->itemX (svmapP->lksP, swordP->name);
	if (pronP != NULL) {
	  if (lmIdx != LVX_NIL) {
	    if (verbosity)
	      INFO ("svmapMap: mapped %s directly to LM.\n", swordP->name);
	  } else {
	    /* it's a pronunciation variant and
	       there is no identical LM word for it */
	    swordP->name[pronP - swordP->name] = '\0';
	    lmIdx = svmapP->lksP->itemX (svmapP->lksP, swordP->name);
	    swordP->name[pronP - swordP->name] = '('; /* ) keeps emacs happy */ 
	  }
	}
	svmapP->lvxSA[svX] = 0;
	svmapP->lvxA[svX]  = (lmIdx != LVX_NIL) ? lmIdx : unkIdx;
      }
    }
  }

  for (svX = svmapP->lnN; svX < (SVX) svmapP->svocabP->list.itemN; svX++) {
    if (svmapP->lvxA[svX] == unkIdx && svmapP->svocabP->list.itemA[svX].fillerTag == 0) {
      if (verbosity)
	WARN ("svmapMap: mapped %s to %s\n", 
	      svmapP->svocabP->list.itemA[svX].name,svmapP->string.unk);
      NILcount++;
    }
  }

  svmapP->dirty = TRUE;

  return TCL_OK;
}

static int svmapMapItf (ClientData clientData, int argc, char *argv[]) 
{
  SVMap *svmapP  = (SVMap*) clientData;
  int      ac    =  argc-1;
  char*  mapName = NULL;
  int    mapFlag = -1;
  int          v = 0;

  if ( itfParseArgv (argv[0], &ac, argv+1, 0,
      "<mapType>", ARGV_STRING, NULL, &mapName, NULL, "id, base, class",
      "-verbose",  ARGV_INT,    NULL, &v,       NULL, "verbosity",
        NULL) != TCL_OK) return TCL_ERROR;

  if (0 == strcmp(mapName,"id")    || 0 == strcmp(mapName,"0"))
    mapFlag = idMap;
  if (0 == strcmp(mapName,"base")  || 0 == strcmp(mapName,"1"))
    mapFlag = pronMap;
  if (0 == strcmp(mapName,"class") || 0 == strcmp(mapName,"2"))
    mapFlag = classMap;

  if (mapFlag == -1) {
    ERROR("svmapMap: unknown map type '%s'\n",mapName);
    return TCL_ERROR;
  }
  return svmapBasicMap (svmapP, mapFlag, v);
}


/* ------------------------------------------------------------------------
    svmapExtMap : create extended map
   ------------------------------------------------------------------------ */

static int svmapExtMap (SVMap *svmapP, LVX lnX, SVX svX, lmScore score)
{
  if (svX == SVX_NIL) {
    ERROR ("svmapExtMap: invalid svX (%d)\n", svX);
    return TCL_ERROR;
  }

  if (lnX >= svmapP->lksP->itemN (svmapP->lksP)  && lnX != LVX_NIL) { 
    ERROR ("svmapExtMap: Cannot map LM-word #%d to Non-LM-word #%d, ignoring\n", svX, lnX);
    return TCL_ERROR;
  }

  if (svX >= (SVX) svmapP->lnN) {
    /* ------------------------------------------------
       svX without corresponding lm word => easy to map
       ----------------------------------------------- */
    svmapAllocTable(svmapP);

    svmapP->lvxSA[svX] = score;
    svmapP->lvxA[ svX] = lnX;

  } else {
    /* --------------------------------------------------------
        svX assuming corresponding lm word => special treatment 
       -------------------------------------------------------- */
    LVX l;
    for (l = 0; l < svmapP->XTbl.svN && svmapP->XTbl.svXA[l] != svX; l++);
    if (l == svmapP->XTbl.svN) {
      /* -------------------------------------------
          add svX into the 'special treatment' table
	  ------------------------------------------- */
      if (score || svX != lnX) {
	svmapP->XTbl.svN++;
	if (NULL == (svmapP->XTbl.svXA = (SVX*) 
		     realloc (svmapP->XTbl.svXA, svmapP->XTbl.svN*sizeof(SVX)))) {
	  ERROR("svmapExtMap: allocation failure\n");
	  return TCL_ERROR;
	}
	svmapP->XTbl.svXA[l]  = svX;
	svmapP->lvxA[svX]  = lnX;
	svmapP->lvxSA[svX] = score;
      }
    } else {
      if (score || svX != lnX) {
	/* update 'special treatment' table */
	svmapP->lvxA [svX] = lnX;
	svmapP->lvxSA[svX] = score;
      } else {
	/* reset 1:1 mapping */
	svmapP->lvxA [svX] = (LVX) svX;
	svmapP->lvxSA[svX] = 0.0;
	
	/* remove svX from 'special treatment' table */
	if (--svmapP->XTbl.svN != 0) {
	  LVX i;
	  for (i = l; i < svmapP->XTbl.svN; i++) {
	    svmapP->XTbl.svXA[i] = svmapP->XTbl.svXA[i+1];
	  }
	} else {
	  Nulle (svmapP->XTbl.svXA);
	}
      }
    }
  }

  svmapP->dirty = TRUE;
  
  return TCL_OK;
}

/* ------------------------------------------------------------------------
    svmapReadSubs : Reads map table from an NGramLM's substitutions section
   ------------------------------------------------------------------------ */

static int svmapReadSubs (SVMap* svmapP, LingKS* lksP) 
{
  NGramLM*   lmP = NULL;
  char**      av = NULL;
  int        cnt = 0;
  int i,ac;

  /* Check, if this is ok */
  if (lksP->type != LingKS_NGramLM) {
    ERROR ("svmapReadSubs: unsupported LingKS type (need NGramLM).\n");
    return TCL_ERROR;
  }
  lmP = lksP->data.ngramLM;

  if (lmP->subslist.itemN == 0) {
    WARN ("svmapReadSubs: empty substitution section.\n");
    return TCL_OK;
  }

  /* Parse it */
  for (i = 0; i < lmP->subslist.itemN; i++) {
    if (Tcl_SplitList (itf, lmP->subslist.itemA[i].line, &ac, &av) == TCL_OK) {
      if (ac == 3) {
	LVX lvX = svmapP->lksP->itemX (svmapP->lksP, av[2]);
	SVX svX = listIndex((List*)&(svmapP->svocabP->list), av[1], 0);
	if (lvX != LVX_NIL && svX != SVX_NIL) {
	  svmapExtMap (svmapP, lvX, svX, CompressScore (atof(av[0])));
	  cnt++;
	}
      }
      Tcl_Free ((char*)av);
    }
  }

  if (cnt< lmP->subslist.itemN)
    WARN("svmapReadSubs: only read %d of %d substitions.\n",cnt,lmP->subslist.itemN);

  svmapP->dirty = TRUE;

  return TCL_OK;
}

static int svmapReadSubsItf (ClientData clientData, int argc, char *argv[]) 
{
  SVMap     *svmapP = (SVMap*) clientData;
  LingKS      *lksP = svmapP->lksP;
  int            ac = argc-1;
 
  if (itfParseArgv (argv[0], &ac, argv+1, 0,
      "-lks", ARGV_OBJECT, NULL, &lksP, "LingKS", "Linguistic Knowledge Source",
       NULL) != TCL_OK) return TCL_ERROR;

  return svmapReadSubs (svmapP, lksP);
}


/* ------------------------------------------------------------------------
     svmapIsDirty
   ----------------
     This procedure tells the ltree, if something in the svmap or the
     underlying LMs has changed and therefore the ltree's cache should
     be discarded
   ------------------------------------------------------------------------ */
int svmapIsDirty (SVMap* svmapP)
{
  int dirty = svmapP->lksP->isDirty (svmapP->lksP) || svmapP->dirty;

  if (dirty)
    svmapBuildRevMap(svmapP);

  svmapP->dirty = FALSE;
  return dirty;
}


/* ------------------------------------------------------------------------
   svmapPuts
   ------------------------------------------------------------------------ */

static int svmapPutsItf (ClientData clientData, int argc, char *argv[]) 
{
  SVMap* svmapP = (SVMap*) clientData;
  SVX       svX = 0;
  SVX       svN = svmapP->svocabP->list.itemN;

  svmapAllocTable(svmapP);

  for (svX=0; svX < svN; svX++) {
    LVX       lvX = svmapGet (svmapP, svX);
    SWord* swordP = svmapP->svocabP->list.itemA+svX;
    char*   lword = NULL;
    lmScore  score;

    if (lvX == LVX_NIL) {
      continue;
      /*      score = 0;
	      lword = svmapP->string.unk; */
    } else { 
      score = svmapP->lvxSA[svX];
      lword = svmapP->lksP->itemA (svmapP->lksP, lvX);
    }
    if (swordP->dictX != SVX_MAX) {
      itfAppendResult("{ ");
      itfAppendElement ("%s", swordP->name);
      itfAppendElement ("%s", lword);
      itfAppendElement ("%f", UnCompressScore (score));
      itfAppendResult(" }\n");
    }
  }

  return TCL_OK;
}

/* ------------------------------------------------------------------------
   svmapGet
   ------------------------------------------------------------------------ */

LVX svmapGet(SVMap* svmapP, SVX svX)
{
  if (svX >= (SVX) svmapP->svocabP->list.itemN) return LVX_NIL;
  return svmapP->lvxA[svX];
}

int svmapGetPuts (SVMap* svmapP, char *sw) {

  int    svX;
  char  *lvName = svmapP->string.unk;
  int    lvX;
  lmScore score;

  if ((svX = listIndex((List*)&(svmapP->svocabP->list), sw, 0)) < 0) {
    ERROR ("Word '%s' not found in SVocab %s.\n", sw, svmapP->svocabP->name);
    return TCL_ERROR;
  }

  svmapAllocTable(svmapP);

  lvX   = svmapGet (svmapP, (SVX)svX);
  score = svmapP->lvxSA[svX];

  if (lvX != LVX_NIL)
    lvName = svmapP->lksP->itemA (svmapP->lksP, lvX);

  itfAppendElement ("%s", svmapP->svocabP->list.itemA[svX].name);
  itfAppendElement ("%s", lvName);
  itfAppendElement ("%f", UnCompressScore(score));

  return TCL_OK;
}

static int svmapGetItf (ClientData clientData, int argc, char *argv[]) 
{
  SVMap *svmapP = (SVMap*) clientData;
  int    ac     = argc-1;
  char  *sw     = NULL;

  if (itfParseArgv (argv[0], &ac, argv+1, 0,
      "<search word>", ARGV_STRING, NULL, &sw, NULL, "the search word",
      NULL) != TCL_OK) return TCL_ERROR;

  return svmapGetPuts (svmapP, sw);
}

/* ------------------------------------------------------------------------
   svmapAdd
   ------------------------------------------------------------------------ */

int svmapAdd(SVMap* svmapP, SVX svX, LVX lvX, lmScore score)
{
  return  svmapExtMap (svmapP, lvX, svX,score);
}

static int svmapAddItf (ClientData clientData, int argc, char *argv[]) 
{
  SVMap *svmapP = (SVMap*) clientData;
  int        ac =  argc-1;
  int       svX = -1;
  LVX       lvX = LVX_NIL;
  char      *sw = NULL;
  char      *lw = NULL;
  float       p = 0.0;

  if (itfParseArgv (argv[0], &ac, argv+1, 0,
      "<search word>", ARGV_STRING, NULL, &sw, NULL, "search vocabulary word",
      "<LM word>",     ARGV_STRING, NULL, &lw, NULL, "language-model word",
      "-prob",         ARGV_FLOAT,  NULL, &p,  NULL, "'probability' (>0 is higher)",
        NULL) != TCL_OK) return TCL_ERROR;

  if ((svX = listIndex((List*)&(svmapP->svocabP->list), sw, 0)) < 0) {
    ERROR ("svmapAdd: '%s' not found in svocab, ignoring.\n", sw);
    return TCL_OK;
  }

  if ((lvX = svmapP->lksP->itemX (svmapP->lksP, lw)) == LVX_NIL) {
    ERROR ("svmapAdd: '%s' not found in LM, ignoring.\n", lw);
    return TCL_OK;
  }

#ifdef COMPRESSEDLM
  if (p >  32) p =  32000/LMSCORE_SCALE_FACTOR;
  if (p < -32) p = -32000/LMSCORE_SCALE_FACTOR;
#endif

  svmapAllocTable(svmapP);
  return svmapAdd(svmapP, (SVX) svX, (LVX) lvX, CompressScore(p));
}


/* ------------------------------------------------------------------------
    svmapDelete: Delete mappings
   ------------------------------------------------------------------------ */

int svmapDel (SVMap* svmapP, SVX svX)
{
  int i;

  svmapP->dirty = TRUE;
    
  /* delete mapping from the 'upper' lm word range */
  if (svX >= (SVX) svmapP->lnN) {
    svmapP->lvxA[svX]  = LVX_NIL;
    svmapP->lvxSA[svX] = 0.0;
    return TCL_OK;
  } 

  /* check for 1:1 word candidates */
  for (i = 0; i < svmapP->XTbl.svN && svmapP->XTbl.svXA[i] != svX; i++);

  /* add extended mapping to LVX_NIL */
  if (i == svmapP->XTbl.svN) {   
    svmapExtMap(svmapP,LVX_NIL,svX,0.0);
    return TCL_OK;
  } 
  
  /* svX has already a special treatment, set it to LVX_NIL */
  svmapP->lvxA [svX] = LVX_NIL;
  svmapP->lvxSA[svX] = 0;

  svmapP->dirty = TRUE;

  return TCL_OK;
}

static int svmapDelItf (ClientData clientData, int argc, char *argv[]) 
{
  int          ac =  argc-1;
  SVMap*   svmapP = (SVMap*) clientData;
  SVocab* svocabP = svmapP->svocabP;
  char*      name = NULL;
  int svX;

  if (itfParseArgv (argv[0], &ac, argv+1, 0,
      "<word>", ARGV_STRING, NULL, &name, NULL, "vocabulary word",
        NULL) != TCL_OK) return TCL_ERROR;

  svX = listIndex((List*) &(svocabP->list),name,0);
  if (svX < 0) {
    ERROR("svmapDel: '%s' not found in underlying vocabulary\n", name);
    return TCL_OK;
  }
  svmapAllocTable(svmapP);
  return svmapDel(svmapP,(SVX) svX);
}

/* ------------------------------------------------------------------------
    svmapIndex : Prints a mapping
   ------------------------------------------------------------------------ */

static int svmapIndexItf (ClientData cd, int argc, char *argv[]) 
{
  SVMap* svmapP = (SVMap*) cd;
  int     ac = argc-1;
  int    svX = 0;

  if (itfParseArgv (argv[0], &ac, argv+1, 0,
      "<n>", ARGV_INT, NULL, &svX, cd, "index",
      NULL) != TCL_OK) return TCL_ERROR;

  if (svX < svmapP->svN) {
    LVX       lvX = svmapGet (svmapP, (SVX)svX);
    SWord* swordP = svmapP->svocabP->list.itemA+svX;
    char*   lword = NULL;
    lmScore  score;

    if (lvX == LVX_NIL) {
      score = 0;
      lword = svmapP->svocabP->string.nil;
    } else { 
      score = svmapP->lvxSA[svX];
      lword = svmapP->lksP->itemA (svmapP->lksP, lvX);
    }
    if (swordP->dictX != SVX_MAX) {
      itfAppendElement ("%s", swordP->name);
      itfAppendElement ("%s", lword);
      itfAppendElement ("%f", UnCompressScore (score));
    }
  }

  return TCL_OK;    
}

/* ------------------------------------------------------------------------
    svmapCheck : Check start and end of sentence mappings
   ------------------------------------------------------------------------ */

int svmapCheck (SVMap *svmapP)
{
  SVocab* svocabP = svmapP->svocabP;
  int      startN = 0;
  int        endN = 0;
  SVX svX;

  for (svX=0;svX< (SVX) svmapP->svN; svX++) {
    if (svmapP->idx.start == svmapGet (svmapP, svX)) startN++;
    if (svmapP->idx.end   == svmapGet (svmapP, svX)) endN++;
  }

  if (startN == 0) {   
    SVX startX = svocabIndex(svocabP,svocabP->string.start);
    if (startX == SVX_NIL)
      startX= svocabAdd(svocabP,svocabP->string.start,1,0,-1);
    if (startX == SVX_NIL) {
      ERROR("svmapCheck: couldn't find any start of sentence symbols '%s'\n",svmapP->string.start);
      return TCL_ERROR;
    }
    svmapAdd(svmapP,startX,svmapP->idx.start,0.0);
  }

  if (endN == 0) {
    SVX endX = svocabIndex(svocabP,svocabP->string.end);
    if (endX == SVX_NIL)
      endX= svocabAdd(svocabP,svocabP->string.end,1,0,-1);
    if (endX == SVX_NIL) {
      ERROR("svmapCheck: couldn't find any end of sentence symbols '%s'\n",svmapP->string.end);
      return TCL_ERROR;
    }
    svmapAdd(svmapP,endX,svmapP->idx.end,0.0);
  }

  return TCL_OK;
}

/* -----------------------------------------------------------------------------------------------------------------------------------------------
    svmapMappedToItf: Given a language model word, print out the list of words in the search vocabulary that are mapped to it.
   ----------------------------------------------------------------------------------------------------------------------------------------------- */
static int svmapMappedToItf (ClientData cd, int argc, char *argv[]) 
{
  SVMap *svmapP = (SVMap*) cd;
  char    *name = NULL;
  int        ac = argc-1;
  int       svX;
  LVX       lvX = LVX_NIL;

  if (itfParseArgv (argv[0], &ac, argv+1, 0,
      "<word>", ARGV_STRING, NULL, &name, NULL, "word to search for, empty string for filler words",
      NULL) != TCL_OK) return TCL_ERROR;

  if (streq(name,"")) {
    for (svX = 0; svX < svmapP->svN; svX++) {
      if (svmapP->svocabP->list.itemA[svX].fillerTag == 1)
	itfAppendElement ("%s", (svmapP->svocabP->list.itemA+svX)->name);
    }
    return TCL_OK;
  }

  if (name)
    lvX = svmapP->lksP->itemX (svmapP->lksP, name);

  for (svX = 0; svX < svmapP->svN; svX++) {
    SWord* swordP = svmapP->svocabP->list.itemA+svX;
    LVX       lvY = svmapGet (svmapP, (SVX)svX);
    if (swordP->fillerTag == 1)                               continue;
    if (lvX != lvY)                                           continue;
    if (streq(swordP->name,svmapP->string.unk) && lvX == lvY) continue;
    itfAppendElement ("%s", (svmapP->svocabP->list.itemA+svX)->name);
  }

  return TCL_OK;
}

/* ------------------------------------------------------------------------
    svmapRead : Read a file containing mappings
   ------------------------------------------------------------------------ */

#define MAXLINE 2048

static int svmapRead (SVMap* svmapP, char* name) 
{
  FILE*      fp = NULL;
  char** splitV = NULL;
  int    splitN = 0;
  SVX      svX = SVX_NIL;
  LVX      lvX = LVX_NIL;
  float    prob = SCORE_MAX;
  char   line[MAXLINE];
  
  if ((fp=fileOpen(name,"r")) == NULL) return TCL_ERROR;
  
  while ( fgets(line,MAXLINE,fp) != NULL) {
    if (TCL_ERROR == (Tcl_SplitList(itf,line,&splitN,&splitV))) {
      WARN("svmapRead: cannot split line '%s'\n",line);
      continue;
    }
    if (splitN !=2 || splitN != 4) {
      WARN("svmapRead: cannot interpret line '%s'\n",line);
      if (splitV) Tcl_Free((char*) splitV);
      continue;
    }

    svX = svocabIndex(svmapP->svocabP,splitV[0]);
    lvX = svmapP->lksP->itemX (svmapP->lksP,  splitV[1]);
    if (splitN == 4) 
      prob =atof(splitV[3]);

    svmapAdd(svmapP,(SVX)svX,lvX,(lmScore)prob);
    if (splitV) Tcl_Free((char*) splitV);

  }
  fileClose(name,fp);
  return TCL_OK;
}

static int svmapReadItf (ClientData cd, int argc, char *argv[]) 
{
  SVMap* svmapP = (SVMap*)cd;
  char*    name = NULL;
  int        ac = argc-1;

  if (itfParseArgv (argv[0], &ac, argv+1, 0,
      "<filename>",   ARGV_STRING, NULL, &name, cd, "file to read from",
      NULL) != TCL_OK) return TCL_ERROR;
  
  return svmapRead(svmapP,name);
}

/* ------------------------------------------------------------------------
    svmapLoad
   ------------------------------------------------------------------------ */

int svmapLoad(SVMap* svmapP, FILE* fp) { 
  size_t   sizeSVX = sizeof(SVX);
  size_t   sizeLVX = sizeof(LVX);
  size_t   sizeLMS = sizeof(lmScore);
  int       format = 0;
  unsigned int svN = SVX_MAX;
  char               str[1024];
  unsigned int       svX;

  if (! fp) {
    ERROR("svmapLoad: Invalid file pointer\n");
    return TCL_ERROR;
  }
  read_string(fp,str);
  if (0 != strncmp(str,"SVMAP-BEGIN",11)) {
    ERROR("svmapLoad: inconsistent file\n");
    return TCL_ERROR;
  }
  if (0 == strcmp(str,"SVMAP-BEGIN 1.1")) {
    format  = 1;
    sizeSVX = read_int(fp);
    sizeLVX = read_int(fp);
    sizeLMS = read_int(fp);
    if (sizeof(lmScore) < sizeLMS) {
      ERROR("svmapLoad:  inconsistent LM Compression\n");
      return TCL_ERROR;
    }
  }

  svmapP->svN = svN = read_int(fp);
  svmapP->XTbl.svN  = read_int(fp);

  if (svN > SVX_MAX) {
    ERROR("svmapLoad: SVX_MAX exceeded\n");
    return TCL_ERROR;
  }
  if (svN != (SVX) svmapP->svocabP->list.itemN) {
    WARN("svmapLoad: underlying vocabulary seems to be inconsistent\n");
  }
  if (NULL == (svmapP->lvxA= realloc(svmapP->lvxA,svN*sizeof(LVX)))) {
    ERROR("svmapLoad: allocation failure\n");
    return TCL_ERROR;
  }
  if (NULL == (svmapP->lvxSA= realloc(svmapP->lvxSA,svN*sizeof(lmScore)))) {
    ERROR("svmapLoad: allocation failure\n");
    return TCL_ERROR;
  }
  if (svmapP->XTbl.svN > 0) {
    if (NULL == (svmapP->XTbl.svXA= 
	 realloc(svmapP->XTbl.svXA,svmapP->XTbl.svN*sizeof(SVX)))) {
      ERROR("svmapLoad: allocation failure\n");
      return TCL_ERROR;
    }
  }
  
  for (svX=0;svX<svN;svX++) {
    if (sizeLVX == sizeof(short)) svmapP->lvxA[svX] = (LVX) ((USHORT) read_short(fp));
    if (sizeLVX == sizeof(int))   svmapP->lvxA[svX] = (LVX) ((UINT)   read_int(fp));
  }
  if (sizeof(lmScore)==sizeof(short))
    read_shorts (fp, (short*)svmapP->lvxSA, svmapP->svN);
  else if (sizeof(lmScore)==sizeof(char))
    fread (svmapP->lvxSA, sizeof(char), svmapP->svN, fp); 
  else if (sizeof(lmScore)==sizeof(float))
    read_floats (fp, (float*)svmapP->lvxSA, svmapP->svN);
  else {
    ERROR("svmapLoad: unknown lmScore type\n");
    return TCL_ERROR;
  }
  /*
  for (svX=0;svX<svN;svX++) {
    if (sizeLMS == sizeof(short)) svmapP->lvxSA[svX] = (lmScore) read_short(fp);
    if (sizeLMS == sizeof(float)) svmapP->lvxSA[svX] = (lmScore) read_float(fp);
  }
  */
  for (svX=0;svX<(SVX) svmapP->XTbl.svN;svX++) {
    if (sizeSVX == sizeof(short)) svmapP->XTbl.svXA[svX] = (SVX) ((USHORT) read_short(fp));
    if (sizeSVX == sizeof(int))   svmapP->XTbl.svXA[svX] = (SVX) ((UINT)   read_int(fp));
  }

  read_string(fp,str);
  if (0 != strcmp(str,"SVMAP-END")) {
    ERROR("svmapLoad: inconsistent file\n");
    if (format == 0)
      ERROR("svmapLoad: inconsistent file (wrong SVX typedefs ?) \n");
    else
      ERROR("svmapLoad: inconsistent file\n");
    return TCL_ERROR;
  }

  svmapP->dirty = TRUE;  
  return TCL_OK;
}


static int svmapLoadItf( ClientData clientData, 
                                      int argc, char *argv[])
{
  int         ac  =  argc - 1;
  SVMap*   svmapP =  (SVMap*)clientData;
  char*      name = NULL;
  FILE*        fp = NULL;

  if ( itfParseArgv( argv[0], &ac, argv+1, 0,
      "<filename>",    ARGV_STRING, NULL, &name, NULL, "file name",
       NULL) != TCL_OK) return TCL_ERROR;  

  fp = fileOpen(name,"r");
  svmapLoad(svmapP,fp);
  fileClose(name,fp);
  return TCL_OK;
}
 
/* ------------------------------------------------------------------------
    svmapSave
   ------------------------------------------------------------------------ */

int svmapSave(SVMap* svmapP, FILE* fp) { 
  if (! fp) {
    ERROR("svmapSave: Invalid file pointer\n");
    return TCL_ERROR;
  }

  write_string(fp,"SVMAP-BEGIN 1.1");
  write_int   (fp,sizeof(SVX));
  write_int   (fp,sizeof(LVX));
  write_int   (fp,sizeof(lmScore));
  write_int   (fp,svmapP->svN); 
  write_int   (fp,svmapP->XTbl.svN); 

  if (sizeof(LVX) == sizeof(short)) 
    write_shorts(fp,(short*)svmapP->lvxA,svmapP->svN); 
  else if (sizeof(LVX) == sizeof(int)) 
    write_ints(fp,(int*) svmapP->lvxA,svmapP->svN); 
  else {
    ERROR("svmapSave: unknown LVX type\n");
    return TCL_ERROR;
  }
  if (sizeof(lmScore) == sizeof(char)) 
    fwrite (svmapP->lvxSA, sizeof(char), svmapP->svN, fp); 
  else if (sizeof(lmScore) == sizeof(short)) 
    write_shorts(fp,(short*)svmapP->lvxSA,svmapP->svN); 
  else if (sizeof(lmScore) == sizeof(float)) 
    write_floats(fp,(float*) svmapP->lvxSA,svmapP->svN); 
  else {
    ERROR("svmapSave: unknown lmScore type\n");
    return TCL_ERROR;
  }
  if (sizeof(SVX) == sizeof(short)) 
    write_shorts(fp,(short*) svmapP->XTbl.svXA,svmapP->XTbl.svN); 
  else if (sizeof(SVX) == sizeof(int)) 
    write_ints(fp,(int*) svmapP->XTbl.svXA,svmapP->XTbl.svN); 
  else {
    ERROR("svmapSave: unknown SVX type\n");
    return TCL_ERROR;
  }

  write_string(fp,"SVMAP-END");
  fflush(fp);

  return TCL_OK;
}


static int svmapSaveItf( ClientData clientData, 
                                      int argc, char *argv[])
{
  int         ac  =  argc - 1;
  SVMap*   svmapP =  (SVMap*)clientData;
  char*      name = NULL;
  FILE*        fp = NULL;

  if ( itfParseArgv( argv[0], &ac, argv+1, 0,
      "<filename>",    ARGV_STRING, NULL, &name, NULL, "file name",
       NULL) != TCL_OK) return TCL_ERROR;  

  fp = fileOpen(name,"w");
  svmapSave(svmapP,fp);
  fileClose(name,fp);
  return TCL_OK;

}

/* ------------------------------------------------------------------------
   svmapGetLMScore
   ------------------------------------------------------------------------ */

lmScore svmapGetLMScore (SVMap* svmapP, LCT lct, SVX svX)
{
  int    fTag = (svX != SVX_NIL) ? svmapP->svocabP->list.itemA[svX].fillerTag : 1;
  int    lctN = svmapP->cache.N;
  LCT*     lA = (lctN) ? svmapP->cache.lA+svX*lctN : NULL;
  lmScore* sA = (lctN) ? svmapP->cache.sA+svX*lctN : NULL;
  
  assert(svX != SVX_NIL);

  svmapP->cache.calls++;
  
  if (fTag) return svmapP->lvxSA[svX];

  if (lctN) {
    int n = svmapP->cache.cA[svX]-1;
    while(n>=0 && lA[n] != lct) n--;
    if (n>=0) {
      svmapP->cache.hits++;
      return sA[n];
    } else {
      LVX      lvX  = svmapGet(svmapP,svX);
      lmScore score = svmapP->lvxSA[svX];
      if (lvX == LVX_NIL)
	return LMSCORE_MAX;
      score +=  svmapP->lksP->scoreFct (svmapP->lksP, lct, lvX, 0);
      if (svmapP->cache.cA[svX] < lctN) {
	svmapP->cache.cA[svX]++;
      }
      n = svmapP->cache.xA[svX];
      svmapP->cache.xA[svX]++;
      if (svmapP->cache.xA[svX] == lctN) {
	svmapP->cache.xA[svX]=0;
      }
      sA[n] = score;
      lA[n] = lct;
      return score;
    }
  } else {
    LVX      lvX  = svmapGet(svmapP,svX);
    lmScore score = svmapP->lvxSA[svX];
    if (lvX == LVX_NIL)
      return LMSCORE_MAX;
    score +=  svmapP->lksP->scoreFct (svmapP->lksP, lct, lvX, 0);
    return score;
  }
}

/* ------------------------------------------------------------------------
   svmapDoMapping
   ------------------------------------------------------------------------ */

static lmScore* tmpA = NULL;

int svmapDoMapping (SVMap *svmapP, lmScore *lnA)
{  
  LVX*      lvxA = svmapP->lvxA;
  lmScore* lvxSA = svmapP->lvxSA;
  lmScore*   inA = lnA;
  int        lvN = svmapP->lnN;
  int        lvM = svmapP->lksP->itemN (svmapP->lksP);
  int        svN = svmapP->svN;
  int i,svX,lvX;

  /* need copying of the lm scores */
  if (lvN < lvM) {
    tmpA= realloc(tmpA,lvM*sizeof(lmScore));
    memcpy(tmpA,lnA,lvM*sizeof(lmScore));
    inA = tmpA;
    for (svX=lvN;svX < lvM; svX++) {
      lvX = lvxA[svX];
      if (lvX != LVX_NIL)
	lnA[svX] = inA[lvX] + lvxSA[svX];
      else
	lnA[svX] = LMSCORE_MAX;
    }
  }

  /* svX, without a corresponding lm word */
  for (svX=lvN;svX <svN; svX++) {
    lvX = lvxA[svX];
    if (lvX == LVX_NIL) 
      lnA[svX] = LMSCORE_MAX;
    else 
      lnA[svX] = inA[lvX] + lvxSA[svX];
  }

  /* svX, with a corresponding lm word, but not valid (special cases) */
  for (i = 0; i < svmapP->XTbl.svN; i++) {
    svX = svmapP->XTbl.svXA[i];
    lvX = lvxA[svX];
    if (lvX == LVX_NIL) 
      lnA[svX] = LMSCORE_MAX;
    else 
      lnA[svX] = inA[lvX] + lvxSA[svX];
  }

  return TCL_OK;
}

/* ========================================================================
    Type Information
   ======================================================================== */

Method svmapMethod[] = 
{
  { "map",       svmapMapItf,      "map SVocab indices to LM indices"},
  { "load",      svmapLoadItf,     "load Mapping from binary file"},
  { "save",      svmapSaveItf,     "save Mapping to binary file"},
  { "read",      svmapReadItf,     "read an LMMap file"},
  { "readSubs",  svmapReadSubsItf, "read map-table from 'NGramLM' object"},
  { "get",       svmapGetItf,      "prints out mapping for vocabulary word"},
  { "index",     svmapIndexItf,    "show mapping entry"},
  { "mappedto",  svmapMappedToItf, "list words mapped to a particular word"},
  { "puts",      svmapPutsItf,     "prints out map table"},
  { "add",       svmapAddItf,      "add or alter map entry"},
  { "delete",    svmapDelItf,      "delete map entry"},
  { NULL, NULL, NULL }
} ;

TypeInfo svmapInfo = 
{ "SVMap", 0, -1,  svmapMethod, 
   svmapCreateItf, svmapFreeItf, 
   svmapConfigureItf, svmapAccessItf, NULL,
  "Search Vocabulary Mapper"
};

static int svmapInitialized = FALSE;

int SVMap_Init (void)
{
  if (! svmapInitialized) {
    if ( SVocab_Init() != TCL_OK) return TCL_ERROR;
    if ( LKS_Init()    != TCL_OK) return TCL_ERROR;

    svmapDefault.name          = NULL;
    svmapDefault.useN          = 0;
    svmapDefault.string.start  = strdup ("<s>");
    svmapDefault.string.end    = strdup ("</s>");
    svmapDefault.string.unk    = strdup ("<UNK>");
    svmapDefault.svocabP       = NULL;

    svmapDefault.lz            = -30.0/LMSCORE_SCALE_FACTOR;
    svmapDefault.lalz          = svmapDefault.lz;

    svmapDefault.penalty.phone =   0.0;
    svmapDefault.penalty.word  =  10.0;
    svmapDefault.penalty.fil   =  10.0;

    itfNewType (&svmapInfo);

    svmapInitialized = TRUE;
  }
  return TCL_OK;
}
