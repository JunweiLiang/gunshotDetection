/* ========================================================================
    JANUS      Speech- to Speech Translation
               ------------------------------------------------------------
               Object: Language model that can do phrases (multi-words)
               ------------------------------------------------------------

    Author  :  Florian Metze
    Module  :  phraseLM.c
    Date    :  $Id: phraseLM.c 2480 2004-08-12 12:31:42Z fuegen $

    Remarks :  This module implements a phrase language model.

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
    Revision 4.2  2004/08/12 12:31:42  fuegen
    isDirty function: ask also for the dirty flag of the baseLM

    Revision 4.1  2003/08/14 11:20:05  fuegen
    Merged changes on branch jtk-01-01-15-fms (jaguar -> ibis-013)

    Revision 1.1.2.34  2003/07/15 16:37:52  fuegen
    changed LingKS load/save interface
    added lingKSPutsFct

    Revision 1.1.2.33  2003/04/01 08:52:35  metze
    Less output

    Revision 1.1.2.32  2002/11/21 17:10:10  fuegen
    windows code cleaning

    Revision 1.1.2.31  2002/08/01 13:42:32  metze
    Fixes for clean documentation.

    Revision 1.1.2.30  2002/06/18 16:30:32  metze
    Changed return behaviour of 'base' method

    Revision 1.1.2.29  2002/06/13 08:07:58  metze
    Added dirty as a function in LMs

    Revision 1.1.2.28  2002/06/10 13:28:15  metze
    Added 'dirty' tag

    Revision 1.1.2.27  2002/05/21 15:21:34  fuegen
    added overflow checking for score functions

    Revision 1.1.2.26  2002/05/03 14:01:39  metze
    Freeing of LingKS now works properly

    Revision 1.1.2.25  2002/05/02 12:52:02  metze
    Made -lks in readSubs optional

    Revision 1.1.2.24  2002/04/29 13:29:37  metze
    Redesign of LM interface (LingKS)

    Revision 1.1.2.23  2002/04/26 12:07:11  soltau
    PhraseLMReadSubs : renamed

    Revision 1.1.2.22  2002/02/26 11:01:57  metze
    Replaced fclose by fileClose

    Revision 1.1.2.21  2002/02/07 12:12:34  metze
    Changes in interface to LM score functions

    Revision 1.1.2.20  2002/02/06 14:20:10  soltau
    Added verbose option

    Revision 1.1.2.19  2002/02/05 11:30:17  soltau
    Configure: added order

    Revision 1.1.2.18  2002/02/02 17:01:07  soltau
    Added PhraseLMReadLMMap

    Revision 1.1.2.17  2002/01/19 13:06:27  metze
    Cosmetics

    Revision 1.1.2.16  2002/01/14 10:30:55  metze
    Cleaned up reading/ writing of dumps

    Revision 1.1.2.15  2002/01/07 10:31:18  soltau
    Fixed lct problem in PhraseLMScoreArray

    Revision 1.1.2.14  2001/12/21 11:46:14  soltau
    phraseLMScore/Array: fixed loop over multiwords

    Revision 1.1.2.13  2001/11/19 14:46:55  metze
    Go away, boring message!

    Revision 1.1.2.12  2001/11/13 16:19:31  metze
    Added warning if mapping a word onto itself

    Revision 1.1.2.11  2001/11/07 15:57:28  metze
    Added configure method to read out baseLM

    Revision 1.1.2.10  2001/11/07 12:40:24  metze
    Made list relocatable

    Revision 1.1.2.9  2001/10/22 08:41:19  metze
    Changed interface to score functions

    Revision 1.1.2.8  2001/10/15 16:09:32  soltau
    Removed bitmask

    Revision 1.1.2.7  2001/10/12 09:57:38  soltau
    set history

    Revision 1.1.2.6  2001/10/03 15:54:46  metze
    Correct setting of freeLCT

    Revision 1.1.2.5  2001/10/02 18:13:47  metze
    Added linking to base LM

    Revision 1.1.2.4  2001/09/27 14:56:40  soltau
    Update baseN in PhraseLMAdd

    Revision 1.1.2.3  2001/06/18 07:56:09  metze
    Compilation and LCT handling under SUN works

    Revision 1.1.2.2  2001/06/01 10:09:20  metze
    Made changes necessary for decoding along lattices

    Revision 1.1.2.1  2001/05/23 08:20:25  metze
    First revision of multi-word language model (might eventually become included in lmodelNJD)


   ======================================================================== */


#include "phraseLM.h"
#include "mach_ind_io.h"


char phraseLMRCSVersion[]="$Id: phraseLM.c 2480 2004-08-12 12:31:42Z fuegen $";


/* ------------------------------------------------------------------------
    forward declarations 
   ------------------------------------------------------------------------ */

static int PhraseLMInitialized = 0;


/* ========================================================================
    LCT handling
   --------------
    We could have different functions for different N-Grams to speed up
    the calculations, it would be possible to use LCT as an HashKey if
    using LVX > 16bit or n > 3 (and not use LCT = 8byte)

    CAVEAT: don't expect that expand(reduce(extend(i)))=i, you won't be
      able to extend an LCT once you reduced it!
   ======================================================================== */


static LCT reduceLCT (LingKS* lksP, LCT lct, int n)
{
  LingKS* bP = lksP->data.phraseLM->baseLM;
  return bP->reduceLCT (bP, lct, n);
}

static LCT extendLCT (LingKS* lksP, LCT lct, LVX lvX)
{
  PhraseLM* lmP = lksP->data.phraseLM;
  LingKS*    bP = lmP->baseLM;

  if (lvX < lmP->baseN) {
    return bP->extendLCT (bP, lct, lvX);
  } else {
    int i, g = lmP->list.itemA[lvX-lmP->baseN].lnxN;
    LVX* lvxA = lmP->list.itemA[lvX-lmP->baseN].lnxA;
    for (i = 0; i < g; i++)
      lct = bP->extendLCT (bP, lct, lvxA[i]);
    return lct;
  }
}

static int decodeLCT (LingKS* lksP, LCT lct, LVX* lvxA, int n)
{
  LingKS* bP = lksP->data.phraseLM->baseLM;
  return bP->decodeLCT (bP, lct, lvxA, -1);
}

static LCT createLCT (LingKS* lksP, LVX lvX)
{
  PhraseLM* lmP = lksP->data.phraseLM;
  LingKS*    bP = lmP->baseLM;
  if (lvX < lmP->baseN)
    return bP->createLCT (bP, lvX);
  ERROR ("Can't use multi-word for <s>.\n");
  return TCL_ERROR;
}

static int freeLCT (LingKS* lksP, LCT lct)
{
  LingKS* bP = lksP->data.phraseLM->baseLM;
  return bP->freeLCT (bP, lct);
}


/* ========================================================================
    LM Interface functions
   ======================================================================== */

static LVX PhraseLMItemX (LingKS *lksP, char *name)
{
  PhraseLM *lmP = lksP->data.phraseLM;
  int         l = listIndex ((List*)&(lmP->list), name, 0);
  assert (lmP->baseN);
  if (l != -1) return (LVX) (l+lmP->baseN);
  else         return lmP->baseLM->itemX (lmP->baseLM, name);
}

static char *PhraseLMItemA (LingKS* lksP, int i)
{
  int b = lksP->data.phraseLM->baseN;
  if (i < b)
    return lksP->data.phraseLM->baseLM->itemA (lksP->data.phraseLM->baseLM, i);
  else
    return lksP->data.phraseLM->list.itemA[i-b].name;
}

static int PhraseLMItemN (LingKS* lksP)
{
  return lksP->data.phraseLM->list.itemN + lksP->data.phraseLM->baseN;
}

static int PhraseLMIsDirty (LingKS* lksP)
{
  int dirty = lksP->dirty;

  dirty |= lksP->data.phraseLM->baseLM->isDirty (lksP->data.phraseLM->baseLM);

  lksP->dirty = FALSE;
  return dirty;
}
  

/* ------------------------------------------------------------------------
    PhraseLMPutsItf
   ------------------------------------------------------------------------ */

static int PhraseLMPutsItf (ClientData cd, int argc, char *argv[])
{
  PhraseLM *lmP  = (PhraseLM*) cd;
  return listPutsItf((ClientData)&(lmP->list), argc, argv);
}


/* ========================================================================
    LM Item Interface functions
   ======================================================================== */

/* ------------------------------------------------------------------------
    PhraseLMItemConfigureItf
   ------------------------------------------------------------------------ */

static int PhraseLMItemConfigureItf (ClientData cd, char *var, char *val)
{
  PhraseLMItem* lmiP = (PhraseLMItem*)cd;
  if (!lmiP) return TCL_ERROR;

  if (!var) {
    ITFCFG (PhraseLMItemConfigureItf,cd,"name");
    ITFCFG (PhraseLMItemConfigureItf,cd,"prob");
    return TCL_OK;
  }
  ITFCFG_CharPtr( var,val,"name",  lmiP->name, 1);
  if (!strcmp (var, "prob")) {
    if (val) lmiP->score = CompressScore (atof(val));
    else {
      itfAppendResult ("%f", UnCompressScore (lmiP->score));
      return TCL_OK;
    }
    return TCL_OK;
  }

  ERROR("Unknown option '-%s %s'.\n", var, val ? val : "?");

  return TCL_ERROR;
}


/* ------------------------------------------------------------------------
    PhraseLMItemPuts
   ------------------------------------------------------------------------ */

static int PhraseLMItemPutsItf (ClientData cd, int argc, char *argv[])
{
  int             ac = argc-1;
  PhraseLMItem* lmiP = (PhraseLMItem*) cd;
  int i;
  char buf[1024];
  char *lword = buf;

  if (itfParseArgv( argv[0], &ac, argv+1, 0, NULL) != TCL_OK)
      return TCL_ERROR;

  itfAppendElement ("%s", lmiP->name);
  for (i = 0; i < lmiP->lnxN; i++)
    lword += sprintf (lword, "%s ",
		      lmiP->lmP->baseLM->itemA (lmiP->lmP->baseLM, lmiP->lnxA[i]));
  itfAppendElement ("%s", buf);
  itfAppendElement ("%f", UnCompressScore (lmiP->score));

  return TCL_OK;
}


/* ========================================================================
    Interesting functions
   ======================================================================== */

/* ------------------------------------------------------------------------
    PhraseLMScoreArray
   --------------------
    Produces an array of scores.
   ------------------------------------------------------------------------ */

static int PhraseLMScoreArray (LingKS *lksP, LCT lct, lmScore *arr, int usehistory)
{
  PhraseLM*    lmP = lksP->data.phraseLM;
  LingKS*   baseLM = lmP->baseLM;
  lmScore*      sp = arr + lmP->baseN - 1;
  int            i;

  /* Get the scores from the base-LM */
  baseLM->scoreArrayFct (baseLM, lct, arr, usehistory);

  /* Add the scores for the new LM */
  for (i = 0; i < lmP->list.itemN; i++) {
    LCT          mylct = lct;
    PhraseLMItem *lmiP = &(lmP->list.itemA[i]);
    LVX             *l = lmiP->lnxA;
    int              n = (usehistory && lmP->history < lmiP->lnxN) ? lmP->history : lmiP->lnxN;
    int              j;

    if ( LMSCORE_MAX - arr[*l] >= lmiP->score ) {
      *(++sp) = LMSCORE_MAX;
    } else {
      lmScore s;

      *(++sp) = arr[*l] + lmiP->score;
      mylct   = baseLM->extendLCT    (baseLM, mylct, *l++);

      for (j = 1; j < n; j++) {
	s = baseLM->scoreFct  (baseLM, mylct, *l, usehistory);
	if ( LMSCORE_MAX - *sp >= s ) {
	  *sp  = LMSCORE_MAX;
	  break;
	} else {
	  *sp += s;
	    s  = lmP->bias*LMSCORE_SCALE_FACTOR;
	  if ( LMSCORE_MAX - *sp >= s ) {
	    *sp  = LMSCORE_MAX;
	    break;
	  } else {
	    *sp += s;
	  }
	}
	mylct  = baseLM->extendLCT (baseLM, mylct, *l++);
      }
    }
  }
  
  return TCL_OK;
}


/* ------------------------------------------------------------------------
    PhraseLMScore
   ---------------
    Scores an utterance.
    If you use the LCT-handling functions defined above, you'll never have
    an LCT with a multi-word in the history, all you can have is a
    multi-word in the last word (lvx)
   ------------------------------------------------------------------------ */

static lmScore PhraseLMScore (LingKS* lksP, LCT lct, LVX w, int usehistory)
{
  PhraseLM* lmP    = lksP->data.phraseLM;
  LingKS*   baseLM = lmP->baseLM;

  if (w < lmP->baseN) /* No multi-word in last word */
    return lmP->baseLM->scoreFct (lmP->baseLM, lct, w, usehistory);

  else { /* Multi word */
    PhraseLMItem *lmiP = &(lmP->list.itemA[w-lmP->baseN]);
    int              n = (usehistory && lmP->history < lmiP->lnxN) ? lmP->history : lmiP->lnxN;
    lmScore       prob = baseLM->scoreFct (baseLM, lct, lmiP->lnxA[0], usehistory);
    int              j;

    if ( LMSCORE_MAX - prob >= lmiP->score ) {
      prob  = LMSCORE_MAX;
    } else {
      lmScore s;

      prob += lmiP->score;
      lct   = baseLM->extendLCT (baseLM, lct, lmiP->lnxA[0]);

      for (j = 1; j < n; j++) {
	s = baseLM->scoreFct  (baseLM, lct, lmiP->lnxA[j], usehistory);
	if ( LMSCORE_MAX - prob >= s ) {
	  prob = LMSCORE_MAX;
	  break;
	} else {
	  prob += s;
	  s     = lmP->bias*LMSCORE_SCALE_FACTOR;
	  if ( LMSCORE_MAX - prob >= s ) {
	    prob = LMSCORE_MAX;
	    break;
	  } else {
	    prob += s;
	  }
	}
	lct   = baseLM->extendLCT (baseLM, lct, lmiP->lnxA[j]);
      }
    }

    return prob;
  }
}


/* ------------------------------------------------------------------------
    PhraseLMAdd
   -------------
    Add an item to the phrase language model
   ------------------------------------------------------------------------ */

extern LVX PhraseLMAdd (PhraseLM* lmP, char* name, char* text, lmScore prob)
{
  int idx;
  int i, ac;
  char **av;

  if (listIndex ((List*)&(lmP->list), name, 0) >= 0) {
    WARN ("Word '%s' already exists in '%s', ignoring.\n", name, lmP->lksP->name);
    return LVX_NIL;
  }
  if ((idx = listNewItem ((List*)&(lmP->list), (ClientData) name)) == -1) {
    WARN ("Couldn't add word '%s' to %s, ignoring.\n", name, lmP->lksP->name);
    return LVX_NIL;
  }
  if (Tcl_SplitList (itf, text, &ac, &av) != TCL_OK) {
    WARN ("Couldn't parse line '%s', ignoring.\n", text);
    listDelete ((List*)&(lmP->list), name);
    return LVX_NIL;
  }
  for (i = 0; i < ac; i++) {
    if (lmP->baseLM->itemX (lmP->baseLM, av[i]) == LVX_NIL) {
      WARN ("Couldn't find word '%s' in lm %s, ignoring.\n",
	    av[i], lmP->baseLM->name);
      listDelete ((List*)&(lmP->list), name);
      return LVX_NIL;      
    }
  }

  lmP->list.itemA[idx].lmP   = lmP;
  lmP->list.itemA[idx].score = prob;
  lmP->list.itemA[idx].lnxN  = ac;
  lmP->list.itemA[idx].lnxA  = (LVX*) malloc (ac*sizeof(LVX));
  for (i = 0; i < ac; i++)
    lmP->list.itemA[idx].lnxA[i] = lmP->baseLM->itemX (lmP->baseLM, av[i]);

  if (ac > lmP->order) {
    lmP->order   = ac;
    lmP->history = lmP->order;
  }

  Tcl_Free ((char*)av);

  lmP->lksP->dirty = TRUE;

  return (LVX) idx;
}

static int PhraseLMAddItf (ClientData cd, int argc, char * argv[])
{
  PhraseLM *lmP = (PhraseLM*) cd;
  int        ac = argc-1;
  float       p = 0;
  int         v = 1;    
  char      *sw = NULL;
  char      *lw = NULL;

  if (itfParseArgv (argv[0], &ac, argv+1, 0,
      "<search word>",    ARGV_STRING, NULL, &sw, NULL, "search vocabulary word",
      "<LM word string>", ARGV_STRING, NULL, &lw, NULL, "language-model word(s)",
      "-prob",            ARGV_FLOAT,  NULL, &p,  NULL, "probability",
      "-v",               ARGV_INT,    NULL, &v,  NULL, "verbose",
      NULL) != TCL_OK) return TCL_ERROR;

  if (!lmP->baseN) {
    ERROR ("Set base LM first.\n");
    return TCL_ERROR;
  }
  if (lmP->baseN != lmP->baseLM->itemN (lmP->baseLM)) {
    ERROR ("PhraseLMAdd : Detected changes in underlying language model\n");
    itfAppendResult ("-1");
  }

  if (v && !strcmp (sw, lw))
    WARN ("You really want to map '%s' onto itself?\n", sw);

  if (PhraseLMAdd (lmP, sw, lw, CompressScore(p)) == LVX_NIL) {
    /* ERROR ("PhraseLMAdd returned error!\n"); */
    itfAppendResult ("-1");
  } else {
    itfAppendResult ("%d", PhraseLMItemX (lmP->lksP, sw) - lmP->baseN);
  }

  return TCL_OK;
}


/* ------------------------------------------------------------------------
    PhraseLMBase
   --------------
    Set the basis LM
   ------------------------------------------------------------------------ */

static int PhraseLMBaseItf (ClientData cd, int argc, char * argv[])
{
  LingKS*  lksP = NULL;
  PhraseLM *lmP = (PhraseLM*) cd;
  int        ac = argc-1;

  if (itfParseArgv (argv[0], &ac, argv+1, 0,
      "<LingKS>",    ARGV_OBJECT, NULL, &lksP, "LingKS", "Base Linguistic Knowledge source",
      NULL) != TCL_OK) return TCL_ERROR;

  lmP->baseLM = lksP;
  lmP->baseN  = lksP->itemN (lksP);

  itfAppendElement ("%s", lmP->lksP->name);

  return TCL_OK;
}


/* ------------------------------------------------------------------------
    PhraseLMSave
   --------------
    Dump the contents of the LM to a file
   ------------------------------------------------------------------------ */

extern int PhraseLMSave (LingKS* lksP, FILE* fp)
{
  int      i, j;
  PhraseLM* lmP = lksP->data.phraseLM;
  char txt[MAXLMLINE];

  /* Hello world & type/ version information*/
  write_string   (fp, "PhraseLM-BEGIN");
  write_string   (fp, "v1");

  /* Basic parameters */
  write_float    (fp, lmP->bias);
  write_int      (fp, lmP->history);
  write_int      (fp, lmP->order);
  write_int      (fp, lmP->list.itemN);

  /* Items (list) */
  for (i = 0; i < lmP->list.itemN; i++) {
    PhraseLMItem* lmiP = lmP->list.itemA+i;
    txt[0] = 0;
    for (j = 0; j < lmiP->lnxN; j++)
      strcat (txt, lmiP->lmP->baseLM->itemA (lmiP->lmP->baseLM, lmiP->lnxA[j])),
      strcat (txt, " ");
    write_string (fp, lmiP->name);
    write_string (fp, txt);
    write_float  (fp, lmiP->score);
  }

  /* Bye, bye */
  write_string   (fp, "PhraseLM-END");

  return TCL_OK;
}

int PhraseLMSaveItf (ClientData cd, int argc, char *argv[]) {

  PhraseLM *lmP  = (PhraseLM*)cd;
  int      ac   = argc - 1;
  char    *name = NULL;
  FILE    *fp   = NULL;
  int      res  = TCL_OK;

  if (itfParseArgv (argv[0], &ac, argv+1, 0,
      "<filename>", ARGV_STRING, NULL, &name, NULL, "file to save (dump) LM to",
      NULL) != TCL_OK) return TCL_ERROR;

  if ( !(fp = fileOpen (name, "w")) ) {
    ERROR ("Could not open '%s' for writing.\n", name);
    return TCL_ERROR;
  }

  res = PhraseLMSave (lmP->lksP, fp);

  fileClose (name, fp);

  return res;
}


/* ------------------------------------------------------------------------
    PhraseLMLoad
   --------------
    Read the contents of a dumped LM
   ------------------------------------------------------------------------ */

extern int PhraseLMLoad (LingKS* lksP, FILE* fp, char *txt) 
{
  int i, itemN;
  PhraseLM* lmP = lksP->data.phraseLM;
  char str[MAXLMLINE], str2[MAXLMLINE];

  /* Consistency checks */
  if (lmP->list.itemN) {
    ERROR ("Can only load into empty LMs.\n"); 
    return TCL_ERROR;
  } else if (!fp) {
    ERROR ("Invalid file pointer.\n");
    return TCL_ERROR;
  } else if (txt) {
    if (strcmp (str, "PhraseLM-BEGIN")) {
      ERROR ("Inconsistent file.\n");
      return TCL_ERROR;
    }
  } else {
    read_string  (fp, str);
    if (strcmp (str, "PhraseLM-BEGIN")) {
      ERROR ("Inconsistent file.\n");
      return TCL_ERROR;
    }
  }
  read_string    (fp, str);
  if (strcmp (str, "v1")) {
    ERROR ("Cannot read PhraseLM file of type and version '%s.\n", str);
    return TCL_ERROR;
  }

  /* Basic parameters */
  lmP->bias    = read_float (fp);
  lmP->history = read_int   (fp);
  lmP->order   = read_int   (fp);
  itemN        = read_int   (fp);

  /* Items (list) */
  for (i = 0; i < itemN; i++) {
    read_string (fp, str);
    read_string (fp, str2);
    if (PhraseLMAdd (lmP, str, str2, (lmScore)read_float (fp)) == LVX_NIL)
      WARN ("Insertion of word '%s' failed.\n", str);
  }

  /* Bye, bye */
  read_string    (fp, str);
  if (strcmp (str, "PhraseLM-END")) {
    WARN ("Didn't find end token.\n");
    return TCL_ERROR;
  }

  return TCL_OK;
}

static int PhraseLMLoadItf (ClientData cd, int argc, char *argv[]) {

  PhraseLM *lmP      = (PhraseLM*)cd;
  char     *name     = NULL;
  int       ac       = argc - 1;
  int       r        = 0;
  FILE     *fp       = NULL;

  if (itfParseArgv(argv[0], &ac, argv+1, 0, 
      "<filename>", ARGV_STRING, NULL, &name,     cd,   "file to read from", 
      NULL) != TCL_OK) return TCL_ERROR;

  /* Try to open the file */
  if ( !(fp = fileOpen (name, "r")) ) {
    ERROR ("Cannot open file '%s' for reading.\n", name);
    return TCL_ERROR;
  }

  r = PhraseLMLoad (lmP->lksP, fp, NULL);

  fileClose (name, fp);

  return r;
}


/* ------------------------------------------------------------------------
    PhraseLMReadSubs
   ------------------------------------------------------------------------ */

static int PhraseLMReadSubs (PhraseLM *lmP, LingKS* lksP) 
{
  NGramLM*   lm2P = NULL;
  int         cnt = 0;
  int i=0;

  /* Check, if this is ok */
  if (lksP->type != LingKS_NGramLM) {
    ERROR ("PhraseLMReadSubs: unsupported LingKS type (need NGramLM).\n");
    return TCL_ERROR;
  }
  lm2P = lksP->data.ngramLM;

  if (lm2P->subslist.itemN == 0) {
    WARN ("PhraseLMReadSubs: empty substitution section\n");
    return TCL_OK;
  }

  /* Parse it */
  for (i = 0; i < lm2P->subslist.itemN; i++) {
    int    ac = 0;
    char **av = NULL;
    if (Tcl_SplitList (itf, lm2P->subslist.itemA[i].line, &ac, &av) == TCL_OK) {
      if (ac == 3) {
	PhraseLMAdd (lmP,av[1],av[2], CompressScore (atof(av[0])));
	cnt++;
      }
    }
    if (av) Tcl_Free ((char*)av);
  }

  if (cnt< lm2P->subslist.itemN) {
    WARN("PhraseLMReadSubs: read %d from %d substitions only\n",cnt,lm2P->subslist.itemN);
  }

  return TCL_OK;
}

static int PhraseLMReadSubsItf (ClientData clientData, int argc, char *argv[]) 
{
  PhraseLM     *lmP = (PhraseLM*) clientData;
  LingKS      *lksP = lmP->baseLM;
  int            ac = argc-1;

  if (itfParseArgv (argv[0], &ac, argv+1, 0,
      "-lks", ARGV_OBJECT, NULL, &lksP, "LingKS", "Linguistic Knowledge Source",
       NULL) != TCL_OK) return TCL_ERROR;

  return PhraseLMReadSubs (lmP, lksP);
}


/* ========================================================================
    Object functions
   ======================================================================== */

/* ------------------------------------------------------------------------
    PhraseLMItemInit/ DeInit
   ------------------------------------------------------------------------ */

int PhraseLMItemInit (PhraseLMItem* lmiP, ClientData cd)
{
  assert (lmiP);
  lmiP->name       =  (cd) ? strdup((char*)cd) : NULL;
  return TCL_OK;
}

int PhraseLMItemDeinit (PhraseLMItem* lmiP)
{
  if (!lmiP) return TCL_OK;
  if (lmiP->name) Nulle (lmiP->name);
  return TCL_OK;
}


/* ------------------------------------------------------------------------
    PhraseLMInit/ DeInit
   ------------------------------------------------------------------------ */

int PhraseLMInit (LingKS* lksP, PhraseLM* lmP, char* name)
{
  lksP->lingKSLoadFct    = PhraseLMLoad;
  lksP->lingKSSaveFct    = PhraseLMSave;
  lksP->lingKSLoadItfFct = PhraseLMLoadItf;
  lksP->lingKSSaveItfFct = PhraseLMSaveItf;
  lksP->lingKSPutsFct    = PhraseLMPutsItf;
  lksP->scoreArrayFct    = PhraseLMScoreArray;
  lksP->scoreFct         = PhraseLMScore;
  lksP->createLCT        = createLCT;
  lksP->reduceLCT        = reduceLCT;
  lksP->extendLCT        = extendLCT;
  lksP->decodeLCT        = decodeLCT;
  lksP->freeLCT          = freeLCT;
  lksP->itemN            = PhraseLMItemN;
  lksP->itemA            = PhraseLMItemA;
  lksP->itemX            = PhraseLMItemX;
  lksP->isDirty          = PhraseLMIsDirty;

  lmP->baseLM  = NULL;
  lmP->baseN   = 0;
  lmP->order   = 1;
  lmP->history = 1;
  lmP->bias    = 0.0;
  lmP->lksP    = lksP;

  if (listInit((List*)&(lmP->list), itfGetType("PhraseLMItem"), 
	       sizeof(PhraseLMItem), 500) != TCL_OK) {
    ERROR("Couldn't allocate item list.");
    return TCL_ERROR;
  }
  lmP->list.init    = (ListItemInit  *)PhraseLMItemInit;
  lmP->list.deinit  = (ListItemDeinit*)PhraseLMItemDeinit;

  return TCL_OK;
}

int PhraseLMLinkN (PhraseLM* lmP)
{
  int useN = listLinkN((List*)&(lmP->list)) - 1;
  if (useN < lmP->lksP->useN) return lmP->lksP->useN;
  else                        return            useN;
}

int PhraseLMDeinit (PhraseLM* lmP)
{
  int i;

  assert (lmP);

  if (PhraseLMLinkN (lmP)) {
    SERROR ("PhraseLM '%s' still in use by other objects.\n", lmP->lksP->name);
    return TCL_ERROR;
  }

  delink (lmP->baseLM, "LingKS");

  for (i = 0; i < lmP->list.itemN; i++)
    Nulle (lmP->list.itemA[i].lnxA);

  return listDeinit ((List*) &(lmP->list));
}


/* ------------------------------------------------------------------------
    PhraseLMCreateItf
   ------------------------------------------------------------------------ */

static ClientData PhraseLMCreateItf (ClientData cd, int argc, char *argv[])
{
  int               ac = argc-1;
  LingKS*         lksP = NULL;
  LingKS*        baseP = NULL;

  if (itfParseArgv (argv[0], &ac, argv+1, 0,
      "<LingKS>", ARGV_OBJECT, NULL, &baseP, "LingKS", "Base Language Model",
       NULL) != TCL_OK) return NULL;

  lksP = LingKSCreate (argv[0], LingKS_PhraseLM);
  lksP->data.phraseLM->baseLM = baseP;
  lksP->data.phraseLM->baseN  = lksP->itemN (baseP);
  printf ("Hi %s %s\n", baseP->name, lksP->name);

  return (ClientData) lksP;
}


/* ------------------------------------------------------------------------
    PhraseLMConfigureItf
   ------------------------------------------------------------------------ */

extern int PhraseLMConfigureItf (ClientData cd, char *var, char *val)
{
  PhraseLM* lmP = (PhraseLM*) cd;
  if (!var) {
    ITFCFG (PhraseLMConfigureItf, cd, "baseLM");
    ITFCFG (PhraseLMConfigureItf, cd, "baseN");
    ITFCFG (PhraseLMConfigureItf, cd, "itemN");
    ITFCFG (PhraseLMConfigureItf, cd, "history");
    ITFCFG (PhraseLMConfigureItf, cd, "order");
    ITFCFG (PhraseLMConfigureItf, cd, "bias");
    return TCL_OK;
  }
  if (lmP->baseLM)
    ITFCFG_CharPtr (var, val, "baseLM", lmP->baseLM->name, 1);
  ITFCFG_Int     (var, val, "baseN",   lmP->baseN,      1);
  ITFCFG_Int     (var, val, "order",   lmP->order,      1);
  ITFCFG_Int     (var, val, "itemN",   lmP->list.itemN, 1);
  ITFCFG_Int     (var, val, "history", lmP->history,    0);
  ITFCFG_Float   (var, val, "bias",    lmP->bias,       0);
  if (lmP->history < 1 || lmP->history > lmP->order)
    lmP->history = lmP->order;
  return listConfigureItf ((ClientData) &(lmP->list), var, val);   
}


/* ------------------------------------------------------------------------
    PhraseLMAccessItf
   ------------------------------------------------------------------------ */

extern ClientData PhraseLMAccessItf (ClientData cd, char *name, TypeInfo **ti)
{ 
  PhraseLM* lmP = (PhraseLM*) cd;
  return listAccessItf((ClientData)&(lmP->list), name, ti);
}


/* ========================================================================
    Type Information
   ======================================================================== */

static Method PhraseLMItemMethod[] = { 
  { "puts", PhraseLMItemPutsItf, NULL },
  {  NULL, NULL, NULL } 
};

TypeInfo PhraseLMItemInfo = { 
  "PhraseLMItem", 0, -1, PhraseLMItemMethod, 
  NULL, NULL,
  PhraseLMItemConfigureItf, NULL,
  itfTypeCntlDefaultNoLink, "Phrase language model item.\n" 
};

static Method PhraseLMMethod[] = { 
  { "base",      PhraseLMBaseItf,      "define the base LingKS"                        },
  { "puts",      PhraseLMPutsItf,      "display the contents of a PhraseLM"            },
  { "add",       PhraseLMAddItf,       "add a mapping for a phrase"                    },
  { "readSubs",  PhraseLMReadSubsItf,  "read map-table from 'NGramLM' object"        },
  {  NULL,       NULL,                 NULL } 
};

TypeInfo PhraseLMInfo = {
  "PhraseLM", 0, -1,    PhraseLMMethod, 
  PhraseLMCreateItf,    NULL,
  PhraseLMConfigureItf, PhraseLMAccessItf, NULL,
  "This module takes a LM and adds phrases (aka. multi-words) to it.\n"
};


extern int PhraseLM_Init()
{
  if (!PhraseLMInitialized) {

    itfNewType (&PhraseLMInfo);
    itfNewType (&PhraseLMItemInfo);

    PhraseLMInitialized = TRUE;
  }
  return TCL_OK;
}
