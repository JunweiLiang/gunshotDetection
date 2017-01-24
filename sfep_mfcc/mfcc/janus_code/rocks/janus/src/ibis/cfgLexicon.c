/* ========================================================================
    JANUS      Speech- to Speech Translation
               ------------------------------------------------------------
               Object: Context Free Grammar Lexicon
               ------------------------------------------------------------

    Author  :  Christian Fuegen
    Module  :  cfgLexicon.c
    Date    :  $Id: cfgLexicon.c 3135 2010-02-20 02:58:22Z metze $
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
    Revision 4.3  2006/10/24 15:28:23  fuegen
    support for reading SRI-LMs WLAT format for sausages (confusion networks)

    Revision 4.2  2004/09/21 09:50:19  fuegen
    removed spelling error

    Revision 4.1  2003/08/14 11:19:59  fuegen
    Merged changes on branch jtk-01-01-15-fms (jaguar -> ibis-013)

    Revision 1.1.2.20  2003/08/07 14:36:04  fuegen
    removed some windows compiler warnings

    Revision 1.1.2.19  2003/07/25 10:06:45  fuegen
    moved declaration in cfgLexiconGetName to top

    Revision 1.1.2.18  2003/07/15 17:02:04  fuegen
    some more changes
      - added forward/recursive arc types
      - added dumping of CFGs/CFGSets (together with pt/rs)
      - added compression/determinization of CFGs/CFGSets
      - rule stack belongs no more to a single grammar
      - changed LingKS load/save interface, added LingKSPutsFct

    Revision 1.1.2.17  2003/06/26 14:45:46  fuegen
    access to Lists of Ts, NTs now without '_Arc'

    Revision 1.1.2.16  2003/06/23 13:12:06  fuegen
    new CFG implementation
      - less memory usage
      - activation/deactivation down to rule level
      - adding of paths, grammar rules, grammars on the fly
      - basic JSGF grammar support
      - additional support of CFG as LingKS type
      - standalone support of CFG/CFGSet, when using only for e.g. parsing

    Revision 1.1.2.15  2002/11/19 09:33:54  fuegen
    some minor beautification

    Revision 1.1.2.14  2002/08/27 14:54:13  fuegen
    changed configure option for Ts, NTs, ... (for Florian)

    Revision 1.1.2.13  2002/08/27 09:31:02  fuegen
    removed bug in cfgLexiconGetTNSet with name==NULL

    Revision 1.1.2.12  2002/06/14 08:51:29  fuegen
    added implicit conversion to upper cases for Ts

    Revision 1.1.2.11  2002/06/06 12:51:26  fuegen
    LingKSPuts now works also for CFG

    Revision 1.1.2.10  2002/06/06 10:02:31  fuegen
    code cleaning, reorganisation of configurable options

    Revision 1.1.2.9  2002/02/19 16:35:24  fuegen
    changed result format of cfgLexiconPuts

    Revision 1.1.2.8  2002/01/29 17:21:17  fuegen
    made gcc clean

    Revision 1.1.2.7  2002/01/25 17:16:28  fuegen
    added handling of starting new trees, added handling of shared grammars, added some minor things for saving memory

    Revision 1.1.2.6  2002/01/25 17:10:21  fuegen
    first real working version (only parsing)

    Revision 1.1.2.5  2002/01/25 17:02:36  fuegen
    extended parse tree implementation

    Revision 1.1.2.4  2002/01/25 16:53:27  fuegen
    major and minor changes: lexicon as cache, extension routines, lingKS interface

    Revision 1.1.2.3  2002/01/25 16:38:55  fuegen
    reimplementation of a big part, using now indices for memory saving reasons, splitted CFG-implementation in smaller modules

    Revision 1.1.2.2  2001/07/13 10:44:14  fuegen
    saved current grammar support version

    Revision 1.1.2.1  2001/06/01 08:55:37  fuegen
    new revision of CFG (many changes)


 
   ======================================================================== */

#include "common.h"
#include "itf.h"
#include "cfg.h"

/* =======================================================================
   Forward Declarations
   ======================================================================= */

extern TypeInfo		cfgLexiconItemInfo;
extern TypeInfo		cfgLexiconInfo;

static CFGLexiconItem	cfgLexiconItemDefault;
static CFGLexicon	cfgLexiconDefault;

extern CFGLexiconItem* cfgLexiconGetItem (CFGLexicon *cfgLP, LVX lvX, CFGArcType type);

/* ======================================================================
   Context Free Grammar Lexicon Item
   ====================================================================== */

/* ----------------------------------------------------------------------
   Init/Create Context Free Grammar Lexicon Item Object
   ---------------------------------------------------------------------- */

int cfgLexiconItemInit (CFGLexiconItem *cfgLIP, ClientData cd) {

  cfgLIP->name       = strdup((char*)cd);
  
  return TCL_OK;
}

CFGLexiconItem* cfgLexiconItemAlloc (CFGLexiconItem *cfgLIP) {

  return cfgLIP;
}

CFGLexiconItem* cfgLexiconItemCreate (char *name) {

  CFGLexiconItem* cfgLIP = (CFGLexiconItem*)malloc (sizeof(CFGLexiconItem));

  if ( !cfgLIP || cfgLexiconItemInit (cfgLIP, (ClientData)name) != TCL_OK ) {
    if ( cfgLIP ) free (cfgLIP);
    ERROR ("Cannot create CFG Lexicon object.\n");
    return NULL;
  }

  return cfgLexiconItemAlloc (cfgLIP);
}

/* ----------------------------------------------------------------------
   Deinit/Free Context Free Grammar Lexicon Item Object
   ---------------------------------------------------------------------- */

int cfgLexiconItemDealloc (CFGLexiconItem *cfgLIP) {

  return TCL_OK;
}

int cfgLexiconItemDeinit (CFGLexiconItem *cfgLIP) {

  if ( cfgLIP->name ) Nulle (cfgLIP->name);
  
  return cfgLexiconItemDealloc (cfgLIP);
}

int cfgLexiconItemFree (CFGLexiconItem *cfgLIP) {
  
  if ( cfgLexiconItemDeinit (cfgLIP) != TCL_OK ) return TCL_ERROR;
  if ( cfgLIP ) free (cfgLIP);
  
  return TCL_OK;
}

/* ----------------------------------------------------------------------
   Configure/Access Context Free Grammar Lexicon Item Object
   ---------------------------------------------------------------------- */

static int cfgLexiconItemConfigureItf (ClientData cd, char *var, char *val) {
  
  CFGLexiconItem *cfgLIP = (CFGLexiconItem*)cd;
  
  if ( !cfgLIP ) cfgLIP = &cfgLexiconItemDefault;
  
  if ( var == NULL ) {
    ITFCFG (cfgLexiconItemConfigureItf, cd, "name");
    return TCL_OK;
  }

  ITFCFG_CharPtr (var, val, "name",         cfgLIP->name,       1);

  ERROR("unknown option '-%s %s'.\n", var, val ? val : "?");
  
  return TCL_ERROR;
}

static ClientData cfgLexiconItemAccessItf (ClientData cd, char *name, TypeInfo **ti) {

  /*
  CFGLexiconItem *cfgLIP = (CFGLexiconItem*)cd;

  if ( *name == '.' ) {
    if ( name[1] == '\0' ) {
    } else {
    }
  }
  */

  *ti = NULL;
  return NULL;
}

/* ----------------------------------------------------------------------
   cfgLexiconItemPutsItf
   ---------------------------------------------------------------------- */
int cfgLexiconItemPuts (CFGLexiconItem *cfgLIP, CFGPutsFormat f) {

  switch (f) {
  case Short_Puts:
    itfAppendResult ("%s", cfgLIP->name);
    return TCL_OK;
  }

  ERROR ("Not supported or unknown PutsFormat %d=%s", f, cfgPutsFormat2str (f));

  return TCL_ERROR;
}


static int cfgLexiconItemPutsItf (ClientData cd, int argc, char *argv[]) {

  CFGLexiconItem *cfgLIP = (CFGLexiconItem*)cd;
  char           *format = "short";

  argc--;
  if ( itfParseArgv (argv[0], &argc, argv+1, 0,
 		     "-format", ARGV_STRING, NULL, &format, NULL,
		     "output format",
		     NULL) != TCL_OK )
    return TCL_ERROR;

  return cfgLexiconItemPuts (cfgLIP, cfgPutsFormat2int (format));
}


/* ======================================================================
   Context Free Grammar Lexicon
   ====================================================================== */

/* ----------------------------------------------------------------------
   Init/Create Context Free Grammar Lexicon Object
   ---------------------------------------------------------------------- */

int cfgLexiconInit (CFGLexicon *cfgLP) {

  int i;

  cfgLP->beginOS    = cfgLexiconDefault.beginOS;
  cfgLP->endOS      = cfgLexiconDefault.endOS;

  for (i=0; i<CFG_LEXICON_N; i++) {
    listInit((List*)&(cfgLP->list[i]), &cfgLexiconItemInfo,
	     sizeof(CFGLexiconItem), cfgLexiconDefault.list[i].blkSize);

    cfgLP->list[i].init   = (ListItemInit  *)cfgLexiconItemInit;
    cfgLP->list[i].deinit = (ListItemDeinit*)cfgLexiconItemDeinit;
  }

  return TCL_OK;
}

CFGLexicon* cfgLexiconCreate () {

  CFGLexicon* cfgLP = (CFGLexicon*)malloc (sizeof(CFGLexicon));

  if ( !cfgLP || cfgLexiconInit (cfgLP) != TCL_OK ) {
    if ( cfgLP ) free (cfgLP);
    ERROR ("Cannot create CFG Lexicon object.\n");
    return NULL;
  }

  /* add bos and eos to list */
  cfgLexiconAddItem (cfgLP, cfgLP->beginOS, T_Arc);
  cfgLexiconAddItem (cfgLP, cfgLP->endOS,   T_Arc);

  return cfgLP;
}


/* ----------------------------------------------------------------------
   Deinit/Free Context Free Grammar Lexicon Object
   ---------------------------------------------------------------------- */

int cfgLexiconDeinit (CFGLexicon *cfgLP) {

  int i;

  for (i=0; i<CFG_LEXICON_N; i++) {
    listDeinit ((List*)&(cfgLP->list[i]));
  }

  return TCL_OK;
}

int cfgLexiconFree (CFGLexicon *cfgLP) {
  
  if ( cfgLexiconDeinit (cfgLP) != TCL_OK ) return TCL_ERROR;
  if ( cfgLP ) free (cfgLP);
  
  return TCL_OK;
}


/* ----------------------------------------------------------------------
   Configure/Access Context Free Grammar Lexicon Object
   ---------------------------------------------------------------------- */

static int cfgLexiconConfigureItf (ClientData cd, char *var, char *val) {
  
  CFGLexicon *cfgLP = (CFGLexicon*)cd;
  char        type[20];
  char       *p;
  int         i;
  
  if ( !cfgLP ) cfgLP = &cfgLexiconDefault;
  
  if ( var == NULL ) {
    ITFCFG (cfgLexiconConfigureItf, cd, "beginOS");
    ITFCFG (cfgLexiconConfigureItf, cd, "endOS");

    for (i=0; i<CFG_LEXICON_N; i++) {
      sprintf (type, "%s", cfgArcType2str((CFGArcType)(i+1)));
      p = strchr (type, '_'); p[0] = 'N'; p[1] = '\0';
      ITFCFG (cfgLexiconConfigureItf, cd, type);
    }
    return TCL_OK;
  }

  ITFCFG_CharPtr (var, val, "beginOS",    cfgLP->beginOS,    0);
  ITFCFG_CharPtr (var, val, "endOS",      cfgLP->endOS,      0);

  for (i=0; i<CFG_LEXICON_N; i++) {
    sprintf (type, "%s", cfgArcType2str((CFGArcType)(i+1)));
    p = strchr (type, '_'); p[0] = 'N'; p[1] = '\0';
    ITFCFG_Int (var, val, type, cfgLP->list[i].itemN, 1);
  }

  ERROR("unknown option '-%s %s'.\n", var, val ? val : "?");
  
  return TCL_ERROR;
}

static ClientData cfgLexiconAccessItf (ClientData cd, char *name, TypeInfo **ti) {
  
  CFGLexicon *cfgLP = (CFGLexicon*)cd;
  char        type[20];
  char       *p;
  int         i;

  if ( *name == '.' ) {
    if ( name[1] == '\0' ) {
      for (i=0; i<CFG_LEXICON_N; i++) {
	sprintf (type, "%s", cfgArcType2str((CFGArcType)(i+1)));
	p = strchr (type, '_'); p[0] = '\0';
	itfAppendElement("%s(0..%d)", type, cfgLP->list[i].itemN-1);
      }
    } else {
      if ( sscanf (name+1, "%[^(](%d)", type, &i) == 2 ) {
	*ti = itfGetType ("CFGLexiconItem");

	p = &type[strlen(type)];
	sprintf (p, "_Arc");

	if ( i >= 0 && i < cfgLP->list[cfgArcType2int(type)-1].itemN )
	  return (ClientData)&(cfgLP->list[cfgArcType2int(type)-1].itemA[i]);
      }
    }
  }
  
  *ti = NULL;
  return NULL;
}


/* ----------------------------------------------------------------------
   cfgLexiconPutsItf
   ---------------------------------------------------------------------- */
int cfgLexiconPuts (CFGLexicon *cfgLP, CFGArcType t, CFGPutsFormat f) {

  int i;

  t &= 0x3;

  for (i=0; i<cfgLP->list[t-1].itemN; i++) {
    cfgLexiconItemPuts (cfgLexiconGetItem (cfgLP, (LVX)i, t), f);
    itfAppendResult(" ");
  }

  return TCL_OK;
}

int cfgLexiconPutsItf (ClientData cd, int argc, char *argv[]) {

  CFGLexicon     *cfgLP  = (CFGLexicon*)cd;
  char           *type   = cfgArcType2str (T_Arc);
  char           *format = "short";

  argc--;
  if ( itfParseArgv (argv[0], &argc, argv+1, 0,
		     "-type",   ARGV_STRING, NULL, &type,   NULL,
		     "type of arc",
 		     "-format", ARGV_STRING, NULL, &format, NULL,
		     "output format",
		     NULL) != TCL_OK )
    return TCL_ERROR;

  return cfgLexiconPuts (cfgLP, cfgArcType2int (type),
			 cfgPutsFormat2int (format));
}

/* ----------------------------------------------------------------------
   cfgLexiconGetItem
   ---------------------------------------------------------------------- */

/* returns a lexicon item, given it's lvX and type */
CFGLexiconItem* cfgLexiconGetItem (CFGLexicon *cfgLP, LVX lvX, CFGArcType type) {

  type &= 0x3;

  if ( lvX != LVX_NIL && type != Lambda_Arc && lvX < cfgLP->list[type-1].itemN )
    return &(cfgLP->list[type-1].itemA[lvX]);

  return NULL;
}

int cfgLexiconGetItemN (CFGLexicon *cfgLP, CFGArcType type) {

  type &= 0x3;

  if ( type == Lambda_Arc ) return 0;

  return cfgLP->list[type-1].itemN;
}

/* ----------------------------------------------------------------------
   cfgLexiconGetName
   ---------------------------------------------------------------------- */

/* returns the name of an lexicon item, given it's lvX and type */
char* cfgLexiconGetName (CFGLexicon *cfgLP, LVX lvX, CFGArcType type) {

  CFGLexiconItem *cfgLIP;

  type   &= 0x3;
  cfgLIP  = cfgLexiconGetItem (cfgLP, lvX, type);

  return cfgLIP ? cfgLIP->name : NULL;
}

static int cfgLexiconGetNameItf (ClientData cd, int argc, char *argv[]) {

  CFGLexicon *cfgLP = (CFGLexicon*)cd;
  int         lvX   = LVX_NIL;
  char       *type  = cfgArcType2str (T_Arc);

  argc--;
  if ( itfParseArgv (argv[0], &argc, argv+1, 0,
		     "<lvX>",       ARGV_INT,    NULL, &lvX,     NULL,
		     "vocabulary index",
		     "-type",       ARGV_STRING, NULL, &type,     NULL,
		     "type of arc",
		     NULL) != TCL_OK )
    return TCL_ERROR;

  itfAppendResult ("%s",
		   cfgLexiconGetName (cfgLP, (LVX)lvX, cfgArcType2int (type)));

  return TCL_OK;
}


/* ----------------------------------------------------------------------
   cfgLexiconGetIndex
   ---------------------------------------------------------------------- */

/* returns the lvX of a lexicon item given it's name and type */
LVX cfgLexiconGetIndex (CFGLexicon *cfgLP, char *name, CFGArcType type) {

  int i;

  type &= 0x3;

  if ( !name )              return LVX_NIL;
  if ( type == Lambda_Arc ) return LVX_NIL;

  i = listName2Index ((List*)&(cfgLP->list[type-1]), name);

  /* convert to LVX */
  return (i < 0) ? LVX_NIL : (LVX)i;
}

static int cfgLexiconGetIndexItf (ClientData cd, int argc, char *argv[]) {

  CFGLexicon *cfgLP = (CFGLexicon*)cd;
  char       *word  = NULL;
  char       *type  = cfgArcType2str (T_Arc);

  argc--;
  if ( itfParseArgv (argv[0], &argc, argv+1, 0,
		     "<word>",      ARGV_STRING, NULL, &word,     NULL,
		     "word",
		     "-type",       ARGV_STRING, NULL, &type,     NULL,
		     "type of arc",
		     NULL) != TCL_OK )
    return TCL_ERROR;

  itfAppendResult ("%d",
		   cfgLexiconGetIndex (cfgLP, word, cfgArcType2int (type)));

  return TCL_OK;
}

/* ----------------------------------------------------------------------
   cfgLexiconAddItf
   ---------------------------------------------------------------------- */

/* adds an lexicon item */
LVX cfgLexiconAddItem (CFGLexicon *cfgLP, char *word, CFGArcType type) {

  int i;

  type &= 0x3;

  if ( type == Lambda_Arc ) return LVX_NIL;

  if ( cfgLP->list[type-1].itemN >= LVX_MAX ) {
    ERROR ("cfgLexiconAddItem: max. number of items (LVX_MAX=%d) exceeded.\n",
	   LVX_MAX);
    return LVX_NIL;
  }

  i = listIndex ((List*)&(cfgLP->list[type-1]), (ClientData)word, 0);

  if ( i < 0 ) {
    i = listNewItem ((List*)&(cfgLP->list[type-1]), (ClientData)word);
  }

  return (LVX)i;
}

static int cfgLexiconAddItemItf (ClientData cd, int argc, char *argv[]) {

  CFGLexicon *cfgLP = (CFGLexicon*)cd;
  char       *word  = NULL;
  char       *type  = cfgArcType2str (T_Arc);

  argc--;
  if ( itfParseArgv (argv[0], &argc, argv+1, 0,
		     "<word>",      ARGV_STRING, NULL, &word,     NULL,
		     "word to add",
		     "-type",       ARGV_STRING, NULL, &type,     NULL,
		     "type of arc",
		     NULL) != TCL_OK )
    return TCL_ERROR;

  itfAppendResult ("%d",
		   cfgLexiconAddItem (cfgLP, word, cfgArcType2int (type)));

  return TCL_OK;
}

/* ----------------------------------------------------------------------
   cfgLexiconWriteItf
   ---------------------------------------------------------------------- */
static int compare_strings (const void *st1, const void *st2) {

  int result;
  char **s1 = (char**)st1;
  char **s2 = (char**)st2;

  result = **s1 - **s2;
  if (result == 0)
    result = strcmp (*s1, *s2);

  return result;
}

static int cfgLexiconWriteItf (ClientData cd, int argc, char *argv[]) {

  CFGLexicon      *cfgLP    = (CFGLexicon*)cd;
  char            *fileName = NULL;
  char            *type     = cfgArcType2str (T_Arc);
  char           **arr      = NULL;
  CFGLexiconItem  *cfgLIP;
  LVX              i;
  CFGArcType       t;
  FILE            *fp;

  argc--;
  if ( itfParseArgv (argv[0], &argc, argv+1, 0,
		     "<filename>", ARGV_STRING, NULL, &fileName, NULL, 
		     "file to write into", 
		     "-type",      ARGV_STRING, NULL, &type,     NULL,
		     "type of arc",
		     NULL) != TCL_OK )
    return TCL_ERROR;

  /* get lexicon type */
  t  = cfgArcType2int(type) & 0x3;

  /* open file */
  if (! (fp = fileOpen(fileName,"w"))) return TCL_ERROR;

  /* print header */
  fprintf (fp, "; -------------------------------------------------------\n");
  fprintf (fp, ";  Type            : CFG Lexicon\n");
  fprintf (fp, ";  Item type       : %s\n",     type);
  fprintf (fp, ";  Number of Items : %d\n",     cfgLP->list[t-1].itemN);
  fprintf (fp, ";  Date            : %s",       dateString());
  fprintf (fp, "; -------------------------------------------------------\n");

  /* fill secondary array to print sorted list */
  arr = (char**)malloc (cfgLP->list[t-1].itemN * sizeof (char*));
  for (i=0; i<cfgLP->list[t-1].itemN; i++) {
    cfgLIP = cfgLexiconGetItem (cfgLP, i, t);
    arr[i] = cfgLIP->name;
  }

  /* sort array */
  qsort (arr, cfgLP->list[t-1].itemN, sizeof (char*), compare_strings);

  /* print array */
  for (i=0; i<cfgLP->list[t-1].itemN; i++) {
    fprintf (fp, "%s\n", arr[i]);
  }

  /* close file and free array */
  fileClose (fileName, fp);
  free (arr);

  return TCL_OK;
}

/* ----------------------------------------------------------------------
   cfgLexiconLoadDump
   ---------------------------------------------------------------------- */
int cfgLexiconLoadDump (CFGLexicon *cfgLP, FILE *fp) {

  int  i, j, itemN;
  char s[CFG_MAXLINE];

  if ( !fp ) {
    ERROR ("cfgLexiconLoadDump: invalid file pointer.\n");
    return TCL_ERROR;
  }

  read_string (fp, s);
  if ( strcmp (s, "CFG-LEXICON-BEGIN 1.0") ) {
    ERROR ("cfgLexiconLoadDump: inconsistent file.\n");
    return TCL_ERROR;
  }

  itemN = read_int (fp);
  if ( itemN != CFG_LEXICON_N ) {
    ERROR ("cfgLexiconLoadDump: inconsistent file.\n");
    return TCL_ERROR;
  }

  for (i=0; i<CFG_LEXICON_N; i++) {

    itemN = read_int (fp);
    for (j=0; j<itemN; j++) {
      read_string (fp, s);
      cfgLexiconAddItem (cfgLP, s, (CFGArcType)(i+1));
    }
  }

  read_string (fp, s);
  if ( strcmp (s, "CFG-LEXICON-END") ) {
    ERROR ("cfgLexiconLoadDump: inconsistent file.\n");
    return TCL_ERROR;
  }

  return TCL_OK;
}

/* ----------------------------------------------------------------------
   cfgLexiconSaveDump
   ---------------------------------------------------------------------- */
int cfgLexiconSaveDump (CFGLexicon *cfgLP, FILE *fp) {

  int             i, j, itemN;
  CFGLexiconItem *cfgLIP;

  if ( !fp ) {
    ERROR ("cfgLexiconSaveDump: invalid file pointer.\n");
    return TCL_ERROR;
  }

  write_string (fp, "CFG-LEXICON-BEGIN 1.0");

  write_int (fp, CFG_LEXICON_N);

  for (i=0; i<CFG_LEXICON_N; i++) {

    itemN = cfgLexiconGetItemN (cfgLP, (CFGArcType)(i+1));
    write_int (fp, itemN);

    for (j=0; j<itemN; j++) {
      cfgLIP = cfgLexiconGetItem (cfgLP, (LVX)j, (CFGArcType)(i+1));

      write_string (fp, cfgLIP->name);
    }
  }

  write_string (fp, "CFG-LEXICON-END");

  return TCL_OK;
}



/****************************************************************************/

/* ========================================================================
    Type Information
   ======================================================================== */

/* ---------- itf types for CFGRLexiconItem */
static Method cfgLexiconItemMethod[] = {
  { "puts",  cfgLexiconItemPutsItf, "display the contents of CFG lexicon" },
  { NULL, NULL, NULL }
};

TypeInfo cfgLexiconItemInfo = { "CFGLexiconItem", 0, -1, cfgLexiconItemMethod,
				NULL, NULL,
				cfgLexiconItemConfigureItf,
				cfgLexiconItemAccessItf,
				itfTypeCntlDefaultNoLink,
   "A 'CFGLexiconItem' object is a item of a CFG lexicon." };


/* ---------- itf types for CFGRLexicon */
static Method cfgLexiconMethod[] = {
  { "add",   cfgLexiconAddItemItf,  "adds an item to the CFG lexicon" },
  { "index", cfgLexiconGetIndexItf, "get lvX of item with given name" },
  { "name",  cfgLexiconGetNameItf,  "get name of item with given lvX" },
  { "puts",  cfgLexiconPutsItf,     "display the contents of CFG lexicon" },
  { "write", cfgLexiconWriteItf,    "writes a lexicon to file" },
  { NULL, NULL, NULL }
};

TypeInfo cfgLexiconInfo = { "CFGLexicon", 0, -1, cfgLexiconMethod,
			    NULL, NULL,
			    cfgLexiconConfigureItf, cfgLexiconAccessItf,
			    itfTypeCntlDefaultNoLink,
   "A 'CFGLexicon' object is a lexicon of a Context Free Grammar." };


/* ---------- CFGLexicon_Init ---------- */

static int cfgLexiconInitialized = 0;

int CFGLexicon_Init (void) {

  int i;

  if ( !cfgLexiconInitialized ) {

    cfgLexiconItemDefault.name		= NULL;

    cfgLexiconDefault.beginOS		= "<s>";
    cfgLexiconDefault.endOS		= "</s>";

    for (i=0; i<CFG_LEXICON_N; i++) {
      cfgLexiconDefault.list[i].itemN	= 0;
      cfgLexiconDefault.list[i].blkSize	= 500;
    }

    itfNewType (&cfgLexiconItemInfo);
    itfNewType (&cfgLexiconInfo);

    cfgLexiconInitialized = 1;
  }

  return TCL_OK;
}
