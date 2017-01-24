/* ========================================================================
    JANUS      Speech- to Speech Translation
               ------------------------------------------------------------
               Object: Linguistic Knowledge Source
               ------------------------------------------------------------

    Author  :  Florian Metze
    Module  :  lks.c
    Date    :  $Id: lks.c 3277 2010-07-01 22:10:21Z metze $
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
     Revision 4.6  2005/11/08 13:10:30  metze
     <s> checked in LM scoring

     Revision 4.5  2005/04/05 11:11:15  metze
     Changed text scoring

     Revision 4.4  2005/03/17 15:08:28  fuegen
     Made default behaviour of the LingKS score function to ignore <s> in the
     scoring, like it is the case during decoding, even if now SVMap is used.
     Can be deactivated by using -ignoreS.

     Revision 4.3  2003/11/17 10:20:21  metze
     Clear svmapCache in interactive calls

     Revision 4.2  2003/11/06 17:45:47  metze
     CLeaned up vocab mapping

     Revision 4.1  2003/08/14 11:20:03  fuegen
     Merged changes on branch jtk-01-01-15-fms (jaguar -> ibis-013)

     Revision 1.1.2.20  2003/07/15 16:37:52  fuegen
     changed LingKS load/save interface
     added lingKSPutsFct

     Revision 1.1.2.19  2003/07/10 08:51:58  fuegen
     fixed bug when creating LingKS with CFG type

     Revision 1.1.2.18  2003/06/23 13:03:17  fuegen
     made it compatible to new CFG implementation
     added LingKS type CFG

     Revision 1.1.2.17  2003/03/11 16:37:23  metze
     Cacheing for MetaLM, Map for GLat computeLCT

     Revision 1.1.2.16  2003/01/29 16:29:48  metze
     Output beautification

     Revision 1.1.2.15  2002/11/22 10:42:50  metze
     Meaningful info

     Revision 1.1.2.14  2002/11/22 10:40:31  metze
     Avoid -segSize message for dumps

     Revision 1.1.2.13  2002/11/21 17:10:10  fuegen
     windows code cleaning

     Revision 1.1.2.12  2002/11/21 14:09:52  metze
     Beautified error messages

     Revision 1.1.2.11  2002/11/19 13:05:08  fuegen
     added deinit for CFGSet

     Revision 1.1.2.10  2002/11/13 09:54:25  soltau
     Start and End words rely on LVX now

     Revision 1.1.2.9  2002/08/27 08:47:56  metze
     itfParseArgv now uses <name> and NULL initialization for char*

     Revision 1.1.2.8  2002/06/24 09:11:08  metze
     Cleaner interface for score function

     Revision 1.1.2.7  2002/06/10 13:25:49  metze
     Added 'dirty' tag

     Revision 1.1.2.6  2002/06/06 12:51:26  fuegen
     LingKSPuts now works also for CFG

     Revision 1.1.2.5  2002/05/03 14:01:02  metze
     Freeing of LingKS now works properly

     Revision 1.1.2.4  2002/05/02 17:14:16  metze
     Fixed configuration

     Revision 1.1.2.3  2002/05/02 12:52:37  metze
     Bugfix

     Revision 1.1.2.2  2002/04/29 13:30:05  metze
     Redesign of LM interface (LingKS)

     Revision 1.1.2.1  2002/04/29 12:07:41  metze
     Added lks.{c|h} to repository


   ======================================================================== */

char  lksRcsVersion[]= "$Id: lks.c 3277 2010-07-01 22:10:21Z metze $";

#include "lks.h"


/* ========================================================================
    Linguistic Knowledge Sources 
   ======================================================================== */

static LingKS LingKSDefault;
static int LingKSInitialized = FALSE;

char *lksTypesA[] = { "NULL", "LatticeLM", "NGramLM", "PhraseLM", "MetaLM", "CFG", "CFGSet" };
int   lksTypesN   = 7;

static int lksGetType (char *name)
{
  int i;
  for (i = lksTypesN-1; i > 0; i--)
    if (streq (name, lksTypesA[i]))
      return i;
  return 0;
}

/* ------------------------------------------------------------------------
    LingKSFreeItf
   ---------------
    Free LingKS Structure
   ------------------------------------------------------------------------ */

int LingKSDeinit (LingKS *lksP)
{
  int rc = TCL_ERROR;
  assert (lksP);

  if (lksP->useN > 0) {
    ERROR ("Linguistic Knowledge Source '%s' still in use by other objects.\n", lksP->name);
    return rc;
  } 

  switch (lksP->type) {
  case LingKS_NGramLM:
    rc = NGramLMDeinit  (lksP->data.ngramLM);
    break;
  case LingKS_PhraseLM:
    rc = PhraseLMDeinit (lksP->data.phraseLM);
    break;
  case LingKS_MetaLM:
    rc = MetaLMDeinit   (lksP->data.metaLM);
    break;
  case LingKS_CFG:
    rc = cfgDeinit      (lksP->data.cfgP);
    break;
  case LingKS_CFGSet:
    rc = cfgSetDeinit   (lksP->data.cfgSP);
    break;
  default:
    WARN ("Don't know how to deinit '%s'.\n", lksTypesA[lksP->type]);
  }

  if (rc == TCL_OK) {
    lksP->cacheN = 0;
    Nulle (lksP->cacheA);
    Nulle (lksP->name);
  }

  return rc;
}

int LingKSFree (LingKS *lksP)
{
  int rc = TCL_ERROR;
  assert (lksP);

  if (lksP->useN > 0) {
    ERROR ("Linguistic Knowledge Source '%s' still in use by other objects.\n", lksP->name);
    return rc;
  }
  
  if (LingKSDeinit (lksP) == TCL_OK) {
    free (lksP);
  }

  return rc;
}

static int LingKSFreeItf (ClientData cd)
{
  return LingKSFree ((LingKS*) cd);
}


/* ------------------------------------------------------------------------
    LingKSCreateItf
   -----------------
    Create a new Linguistic Knowledge Source
   ------------------------------------------------------------------------ */

extern LingKS* LingKSCreate (char* name, int type)
{
  LingKS* lksP = (LingKS*) malloc (sizeof (LingKS));
  int       ok = TCL_ERROR;

  if (!lksP) {
    ERROR ("Failed to allocate Linguistic knowledge source '%s'\n", name);
    return NULL;
  }

  lksP->useN   = 0;
  lksP->name   = strdup (name);
  lksP->type   = type;
  lksP->cacheN = 0;
  lksP->cacheA = NULL;
  lksP->dirty  = FALSE;

  lksP->interpol_wgt = 0.0;
  lksP->global_interpol = 0;

  switch (type) {
  case LingKS_NGramLM:
    lksP->data.ngramLM  = (NGramLM*)  malloc (sizeof (NGramLM));
    ok = NGramLMInit      (lksP, lksP->data.ngramLM,  name);
    break;
  case LingKS_PhraseLM:
    lksP->data.phraseLM = (PhraseLM*) malloc (sizeof (PhraseLM));
    ok = PhraseLMInit     (lksP, lksP->data.phraseLM, name);
    break;
  case LingKS_MetaLM:
    lksP->data.metaLM   = (MetaLM*)   malloc (sizeof (MetaLM));
    ok = MetaLMInit       (lksP, lksP->data.metaLM,   name);
    break;
  case LingKS_CFG:
    lksP->data.cfgP     = (CFG*)      malloc (sizeof (CFG));
    ok = cfgLingKSInit    (lksP, lksP->data.cfgP,     name);
    break;
  case LingKS_CFGSet:
    lksP->data.cfgSP    = (CFGSet*)   malloc (sizeof (CFGSet));
    ok = cfgSetLingKSInit (lksP, lksP->data.cfgSP,    name);
    break;   
  }

  if (ok != TCL_OK) {
    ERROR ("Failed to create Linguistic Knowledge Source %s of type %d.\n", name, type);
    LingKSFree (lksP);
    lksP = NULL;
  }

  return lksP;
}

static ClientData LingKSCreateItf (ClientData cd, int argc, char *argv[])
{
  int       ac = argc;
  int     type = 0;
  char*     tn = NULL;
  LingKS* lksP = NULL;
  char* name = NULL;

  if (itfParseArgv(argv[0], &ac, argv, 0,
    "<name>", ARGV_STRING, NULL, &name, NULL,   "name of the linguistic knowledge source",
    "<type>", ARGV_STRING, NULL, &tn,   "Type", "Kind of LingKS: NGramLM|PhraseLM|MetaLM|CFG|CFGSet",
      NULL) != TCL_OK) return NULL;

  if ((type = lksGetType (tn)) == LingKS_NULL) {
    ERROR ("Can't create Linguistic Knowledge source of type '%s'.\n", tn);
    return NULL;
  } else {
    lksP = LingKSCreate (name, type);
  }

  return (ClientData) lksP;
}


/* ------------------------------------------------------------------------
    LingKSConfigureItf
   --------------------
    Configuration of the Linguistic Knowledge source
   ------------------------------------------------------------------------ */

static int LingKSConfigureItf (ClientData cd, char *var, char *val)
{
  LingKS* lksP = (LingKS*) cd;

  if (!lksP)
    lksP = &LingKSDefault;

  if (!var) {
    ITFCFG (LingKSConfigureItf, cd, "name");
    ITFCFG (LingKSConfigureItf, cd, "type");
    ITFCFG (LingKSConfigureItf, cd, "useN");
    ITFCFG (LingKSConfigureItf, cd, "dirty");
    ITFCFG (LingKSConfigureItf, cd, "weight");
    ITFCFG (LingKSConfigureItf, cd, "gInterpol");
    return TCL_OK;
  } else {
    ITFCFG_CharPtr (var, val, "name",  lksP->name,            1);
    ITFCFG_CharPtr (var, val, "type",  lksTypesA[lksP->type], 1);
    ITFCFG_Int     (var, val, "useN",  lksP->useN,            1);
    ITFCFG_Int     (var, val, "dirty", lksP->dirty,           0);
    ITFCFG_Float   (var, val, "weight", lksP->interpol_wgt,   0);
    ITFCFG_Int     (var, val, "gInterpol", lksP->global_interpol,   0);
    ERROR ("Unknown option '-%s %s'.\n", var, val ? val : "?");
    return TCL_ERROR;
  }
  /* The following code allows you to access the configuration options
     of the underlying LM at top level, which is not what we want */
  /*
  switch (lksP->type) {
  case LingKS_NGramLM:
    return NGramLMConfigureItf  ((ClientData) lksP->data.ngramLM,  var, val);
  case LingKS_PhraseLM:
    return PhraseLMConfigureItf ((ClientData) lksP->data.phraseLM, var, val);
  case LingKS_MetaLM:
    return MetaLMConfigureItf   ((ClientData) lksP->data.metaLM,   var, val);
  }
  */
}


/* ------------------------------------------------------------------------
    LingKSAccessItf
   -----------------
    Access to the Linguistic Knowledge Source
   ------------------------------------------------------------------------ */

static ClientData LingKSAccessItf (ClientData cd, char *name, TypeInfo **ti)
{ 
  LingKS*   lksP = (LingKS*) cd;

  if (*name == '.') {
    if (name[1] == '\0') {
      itfAppendElement ("data");
      itfAppendElement (lksTypesA[lksP->type]);
      if (lksP->cacheA)
	itfAppendElement ("cacheA(0..%d)", lksP->itemN (lksP)-1);
    } else {
      int i;
      if (streq (name+1, "data") || streq (name+1, lksTypesA[lksP->type])) {
	*ti = itfGetType (lksTypesA[lksP->type]);
	return lksP->data.cd;
      }
      if (sscanf (name+1, "cacheA(%d)", &i) == 1 && lksP->cacheA &&
		 i >= 0 && i < lksP->itemN (lksP)) {
	itfAppendElement ("%f", UnCompressScore (lksP->cacheA[i]));
      }
    }
  }
  *ti = NULL;
  return NULL;
  /* The following code allows you to access sub-objects and the like
     also on the top-level */
  /*
  *ti = itfGetType (lksTypesA[lksP->type]);
  switch (lksP->type) {
  case LingKS_NGramLM:
    return NGramLMAccessItf  ((ClientData) lksP->data.ngramLM,  name, ti);
  case LingKS_PhraseLM:
    return PhraseLMAccessItf ((ClientData) lksP->data.phraseLM, name, ti);
  case LingKS_MetaLM:
    return MetaLMAccessItf   ((ClientData) lksP->data.metaLM,   name, ti);
  }
  return NULL;
  */
}


/* ------------------------------------------------------------------------
    LingKSPutsItf
   ---------------
    Prints out the item list
   ------------------------------------------------------------------------ */

static int LingKSPutsItf (ClientData cd, int argc, char *argv[]) {

  LingKS *lksP = (LingKS*) cd;

  if ( !lksP->lingKSPutsFct ) {
    ERROR ("Sorry, function pointer for puts is NULL.\n");
    return TCL_ERROR;
  }

  return lksP->lingKSPutsFct (lksP->data.cd, argc, argv);
}

/* ------------------------------------------------------------------------
    LingKSIndexItf
   ----------------
    Return the index of an Item
   ------------------------------------------------------------------------ */

static int LingKSIndexItf (ClientData cd, int argc, char *argv[])
{
  LingKS *lksP = (LingKS*) cd;
  char*   name = NULL;
  int       ac =  argc - 1;

  if (itfParseArgv (argv[0], &ac, argv+1, 0, 
      "<word>", ARGV_STRING, NULL, &name, cd, "word you want the index for", 
      NULL) != TCL_OK) return TCL_ERROR;

  itfAppendElement ("%d", (int) lksP->itemX (lksP, name));
  return TCL_OK;
}


/* ------------------------------------------------------------------------
    LingKSNameItf
   ---------------
    Returns the name of an item
   ------------------------------------------------------------------------ */

static int LingKSNameItf (ClientData cd, int argc, char *argv[])
{
  LingKS *lksP = (LingKS*) cd;
  int      idx = 0;
  int       ac =  argc - 1;

  if (itfParseArgv (argv[0], &ac, argv+1, 0, 
      "<index>", ARGV_INT, NULL, &idx, cd, "index of element to print", 
      NULL) != TCL_OK) return TCL_ERROR;

  itfAppendElement ("%s", lksP->itemA (lksP, idx));
  return TCL_OK;
}


/* ========================================================================
    Specialized Functions
   ======================================================================== */

/* ------------------------------------------------------------------------
    LingKSLoadItf
   ---------------
    load the LM from File
   ------------------------------------------------------------------------ */

static int LingKSLoadItf (ClientData cd, int argc, char *argv[]) {

  LingKS *lksP     = (LingKS*) cd;
  int     res      = TCL_OK;

  if ( !lksP->lingKSLoadItfFct ) {
    ERROR ("Sorry, function pointer for loading is NULL.\n");
    return TCL_ERROR;
  }

  res = lksP->lingKSLoadItfFct (lksP->data.cd, argc, argv);

  lksP->dirty = TRUE;

  return res;
}


/* ------------------------------------------------------------------------
    LingKSSaveItf
   ---------------
    dump the LM to file
   ------------------------------------------------------------------------ */

static int LingKSSaveItf (ClientData cd, int argc, char *argv[]) {

  LingKS *lksP   = (LingKS*) cd;

  if (!lksP->lingKSSaveItfFct) {
    ERROR ("Sorry, function pointer for saving is NULL.\n");
    return TCL_ERROR;
  }

  return lksP->lingKSSaveItfFct (lksP->data.cd, argc, argv);
}


/* ------------------------------------------------------------------------
    LingKSScoreItf
   ----------------
    Tcl Interface for all LM scoring functions
   ------------------------------------------------------------------------ */

int LingKSScoreItf (ClientData cd, int argc, char * argv[])
{
  LingKS*    lksP    = (LingKS*) cd;
  char      **av;
  int        ac      = argc-1;
  int        idx     = 0;
  int        useh    = 0;
  int        array   = 0;
  float      scoreF  = 0.0;
  SVMap*     svmapP  = NULL;
  LVX*       lvxA    = NULL;
  char*      startS  = NULL;
  char*      worte   = NULL;
  lmScore    score   = 0;
  int        ignoreS = 1;
  int        i;
  LCT        lct;

  if (itfParseArgv( argv[0], &ac,  argv+1,  0,
     "<word sequence>", ARGV_STRING, NULL, &worte,   NULL,    "sequence of words", 
     "-idx",            ARGV_INT,    NULL, &idx,     NULL,    "start index for conditional probabilities",
     "-array",          ARGV_INT,    NULL, &array,   NULL,    "use ScoreArray, implies idx == n-1",
     "-usehistory",     ARGV_INT,    NULL, &useh,    NULL,    "use the stored reduced history",
     "-map",            ARGV_OBJECT, NULL, &svmapP,  "SVMap", "use vocab mapper",
     "-startString",    ARGV_STRING, NULL, &startS,  NULL,    "different start string than <s>",
     "-ignoreS",        ARGV_INT,    NULL, &ignoreS, NULL,    "ignore initial start string in scoring",
    NULL) != TCL_OK) {
    return TCL_ERROR;
  }

  if (Tcl_SplitList (itf, worte, &ac, &av) != TCL_OK) {
    ERROR ("LingKSScore: could not split list of words\n");
    return TCL_ERROR;
  } else if (!lksP->itemN (lksP)) {
    ERROR ("LingKSScore: language model has not been read yet\n");	
    return TCL_ERROR;
  }

  if (svmapP) {
    /* -----------------------------
         Use the vocabulary mapper
       ----------------------------- */

    if (idx || array || useh || startS)
      WARN ("LingKSScore: ignoring -idx, -array, -usehistory, -startString when using -map\n");
    if ( !ignoreS ) 
      WARN ("start string cannot be scored when -map is used.\n");

    /* Check the word IDs */
    for (i = 0; i < ac; i++) {
      int svX;
      if ((svX = listIndex ((List*) &(svmapP->svocabP->list), av[i], 0)) < 0) {
	ERROR ("LingKSScore: couldn't find word '%s'.\n", av[i]);
	Tcl_Free ((char*) av);
	return TCL_ERROR;
      } else if (!svmapP->svocabP->list.itemA[(SVX)svX].fillerTag &&
		 svmapGet (svmapP, (SVX) svX) == LVX_NIL) {
	ERROR ("LingKSScore: word '%s' not mapped.\n", av[i]);
	Tcl_Free ((char*) av);
	return TCL_ERROR;
      }
    }

    /* clear cache of lm scores in svmap */
    svmapClearCache (svmapP);

    /* Loop over the words */
    lct = lksP->createLCT (lksP,svmapP->idx.start);
    for (i = 0; i < ac; i++) {
      SVX svX = listIndex ((List*) &(svmapP->svocabP->list), av[i], 0);
      LVX lvX = svmapGet (svmapP, svX);
      int isf = svmapP->svocabP->list.itemA[svX].fillerTag;

      if (i == 0 && lvX != svmapP->idx.start) {
	ERROR("LingKSScore: begin of sentence '%s' missing.\n",svmapP->string.start);
	Tcl_Free ((char*) av);
	return TCL_ERROR;
      }
      if (i == ac-1 && lvX != svmapP->idx.end) {
	ERROR("LingKSScore: end of sentence '%s' missing.\n",svmapP->string.end);
	Tcl_Free ((char*) av);
	return TCL_ERROR;
      }

      /* Different kinds of transitions */
      if (i == 0)
	scoreF += 0.0;
      else if (isf)
	scoreF += svmapP->penalty.fil;
      else {
	lmScore lms = svmapGetLMScore (svmapP, lct, svX);
	if (lms == LMSCORE_MAX) {
	  scoreF = SCORE_MAX;
	  break;
	}
	scoreF += svmapP->penalty.word + svmapP->lz * lms;
      }
      lct = (isf) ? lct : lksP->extendLCT (lksP, lct, lvX);
    }
 
  } else {
    /* ------------------------
         No vocabulary mapper
       ------------------------ */
    if ( !startS ) startS = "<s>";

    lvxA = (LVX*) malloc (ac * sizeof (LVX));

    /* Get the word IDs */
    for (i = 0; i < ac; i++) {
      int j;
      if ((j = lksP->itemX (lksP, av[i])) != LVX_NIL)
	lvxA[i]  = (LVX) j;
      else {
	ERROR ("LingKSScore: could not find word '%s'\n", av[i]);
	free (lvxA);
	Tcl_Free ((char*) av);
	return TCL_ERROR;
      }
    }
    if (lksP->itemX (lksP, startS) == LVX_NIL) {
      ERROR ("Could not find begin-of-sentence '%s'\n", startS);
      free (lvxA);
      Tcl_Free ((char*) av);
      return TCL_ERROR;
    }

    /* Initialize the lct */
    lct = lksP->createLCT (lksP, lksP->itemX (lksP, startS));

    for (i = 0; i < idx; i++)
      lct = lksP->extendLCT (lksP, lct, lvxA[i]);
    
    /* Allocate the cache if needed */
    if (array && lksP->cacheN < lksP->itemN (lksP)) {
      lksP->cacheA = (lmScore*) realloc (lksP->cacheA, lksP->itemN (lksP) * sizeof (lmScore));
      lksP->cacheN = lksP->itemN (lksP);
    }
    
    /* Loop over the words */
    for (i = idx; i < ac; i++) {
      if (!array)
	score = lksP->scoreFct (lksP, lct, lvxA[i], useh);
      else {
	lksP->scoreArrayFct    (lksP, lct, lksP->cacheA, useh);
	score = lksP->cacheA[lvxA[i]];
      }

      /* different transitions -- is this correct? */
      if (ignoreS && i == idx && lvxA[i] == lksP->itemX (lksP, startS) )
	scoreF -= 0.0;
      else
	scoreF -= UnCompressScore (score);

      lct = lksP->extendLCT (lksP, lct, lvxA[i]);
    }
  }

  free (lvxA);
  Tcl_Free ((char*) av);
  itfAppendResult ("%f", scoreF);

  return TCL_OK;
}


/* ========================================================================
    Type Information
   ======================================================================== */

static Method LingKSMethod[] = { 
  { "puts",      LingKSPutsItf,      "display the contents of an LingKS"          },
  { "index",     LingKSIndexItf,     "return the internal index of an LingKSItem" },
  { "name",      LingKSNameItf,      "return the name of an LingKSItem"           },
  { "score",     LingKSScoreItf,     "return the score of a text string"          },
  { "save",      LingKSSaveItf,      "create binary dump of LM"                   },
  { "load",      LingKSLoadItf,      "loads an LM-file (dump and generic files)"  },
  {  NULL,       NULL,            NULL } 
};

TypeInfo LingKSInfo = {
  "LingKS", 0, -1,    LingKSMethod, 
  LingKSCreateItf,    LingKSFreeItf,
  LingKSConfigureItf, LingKSAccessItf, NULL,
  "Generic Linguistic Knowledge Source:\n  type-specific methods are\n  accessible via sub-objects." 
};

extern int LKS_Init()
{
  if (!LingKSInitialized) {
    LingKSDefault.name   = NULL;
    LingKSDefault.useN   = 0;
    LingKSDefault.type   = LingKS_NULL;
    LingKSDefault.cacheA = NULL;
    LingKSDefault.cacheN = 0;

    if (   NGramLM_Init() != TCL_OK) return TCL_ERROR;
    if (  PhraseLM_Init() != TCL_OK) return TCL_ERROR;
    if (    MetaLM_Init() != TCL_OK) return TCL_ERROR;
    if (       CFG_Init() != TCL_OK) return TCL_ERROR;

    itfNewType (&LingKSInfo);

    LingKSInitialized = TRUE;
  }
  return TCL_OK;
}
