/* ========================================================================
    JANUS      Speech- to Speech Translation
               ------------------------------------------------------------
               Object: Search Vocabulary
               ------------------------------------------------------------

    Author  :  Hagen Soltau
    Module  :  svocab.c
    Date    :  $Id: svocab.c 2864 2009-02-16 21:18:17Z metze $
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
    Revision 1.6  2004/05/27 11:31:35  metze
    Added '-fTag' option for consistency

    Revision 1.5  2003/08/14 11:20:10  fuegen
    Merged changes on branch jtk-01-01-15-fms (jaguar -> ibis-013)

    Revision 1.4.4.47  2003/06/06 08:06:19  fuegen
    made gcc 3.3 clean

    Revision 1.4.4.46  2003/05/08 16:41:56  soltau
    Purified!

    Revision 1.4.4.45  2003/02/21 13:07:52  soltau
    svocabConfigure : int -> UINT

    Revision 1.4.4.44  2003/01/31 14:07:46  soltau
    did it again

    Revision 1.4.4.43  2003/01/31 14:05:18  soltau
    svocabConfigureItf: cleaned

    Revision 1.4.4.42  2003/01/27 14:43:33  metze
    Bugfix when reading dumps

    Revision 1.4.4.41  2002/11/25 17:37:45  metze
    Made 256 Gaussians per Codebook possible

    Revision 1.4.4.40  2002/11/21 17:10:12  fuegen
    windows code cleaning

    Revision 1.4.4.39  2002/11/20 13:41:51  soltau
    svocabAdd: don't add words without pronunciation in mode 1

    Revision 1.4.4.38  2002/11/13 12:47:40  soltau
    svocabLoad: read svxMax

    Revision 1.4.4.37  2002/11/13 09:48:07  soltau
    Changes for optional words

    Revision 1.4.4.36  2002/10/07 07:33:25  soltau
    svocabLoad: use dictX

    Revision 1.4.4.35  2002/09/02 16:26:12  soltau
    svocabLoad/Save : improved handling of differend svx/lvx typedefs

    Revision 1.4.4.34  2002/08/27 08:49:44  metze
    itfParseArgv now uses <name> and NULL initialization for char*

    Revision 1.4.4.33  2002/06/26 11:57:55  fuegen
    changes for new directory structure support and code cleaning

    Revision 1.4.4.32  2002/06/05 14:54:52  soltau
    svocabCoonfigure : added svxMax

    Revision 1.4.4.31  2002/04/29 13:29:38  metze
    Redesign of LM interface (LingKS)

    Revision 1.4.4.30  2002/04/26 14:52:03  soltau
    removed commentChar

    Revision 1.4.4.29  2002/04/26 13:52:49  soltau
    svocabAdd: don't revert word to a candidate

    Revision 1.4.4.28  2002/04/26 11:56:58  soltau
    svocabIndexItf : fixed return value

    Revision 1.4.4.27  2002/04/26 11:43:00  soltau
    svocabIndex : made it visible
    svocabAdd   : fixed return value

    Revision 1.4.4.26  2002/01/19 11:41:27  metze
    Configuration options implemented

    Revision 1.4.4.25  2002/01/17 16:30:30  soltau
    Check for mandatory words

    Revision 1.4.4.24  2001/11/30 15:00:04  soltau
    beautified

    Revision 1.4.4.23  2001/11/30 14:48:45  soltau
    svocabAdd : added argument dictX
    svocabDel : improved memory efficieny

    Revision 1.4.4.22  2001/11/21 10:56:11  soltau
    Added access to dictionary from svocab

    Revision 1.4.4.21  2001/10/11 11:29:03  soltau
    Added swordConfigureItf

    Revision 1.4.4.20  2001/10/02 17:54:30  metze
    Added svocabConfigureItf to see the effect of -useN

    Revision 1.4.4.19  2001/07/26 17:36:41  soltau
    Made optional silence alwas a filler word

    Revision 1.4.4.18  2001/06/28 10:48:23  soltau
    Added Load/save routines for binary files to support fast decoder startup

    Revision 1.4.4.17  2001/06/07 12:35:51  metze
    Fixed bug in read routine

    Revision 1.4.4.16  2001/05/11 16:41:04  metze
    Cleaned up LCT interface

    Revision 1.4.4.15  2001/04/27 07:37:50  metze
    Separated svocab and svmap

    Revision 1.4.4.14  2001/04/23 13:48:05  soltau
    Added functions to change Vocabulary and Mapper on the fly

    Revision 1.4.4.13  2001/04/03 16:49:40  soltau
    make tcl happy: itfAppendResult -> itfAppendElement

    Revision 1.4.4.12  2001/03/23 13:42:05  metze
    Mapping for lnX implemented

    Revision 1.4.4.11  2001/03/17 17:38:32  soltau
    Repaired mapping finally (last famous words ..)

    Revision 1.4.4.10  2001/03/16 17:15:26  soltau
    Fixed svmapInit problems

    Revision 1.4.4.9  2001/03/16 14:56:18  soltau
    Fixed intialization for special words

    Revision 1.4.4.8  2001/03/16 15:23:26  metze
    Made endSVX etc. configureable, again

    Revision 1.4.4.7  2001/03/16 15:15:28  metze
    Made endSVX etc. configureable

    Revision 1.4.4.6  2001/03/15 18:29:56  metze
    Made unseen pronunciations safe

    Revision 1.4.4.5  2001/03/15 15:42:58  metze
    Re-implementation of LM-Lookahead

    Revision 1.4.4.4  2001/03/05 08:40:15  metze
    Cleaned up treatment of exact LM in LMLA

    Revision 1.4.4.3  2001/02/27 15:10:28  metze
    LModelNJD and LMLA work with floats

    Revision 1.4.4.2  2001/02/02 16:09:19  soltau
    Added access functions

    Revision 1.4.4.1  2001/01/29 18:02:31  soltau
    Fixed bug in special word initalization

    Revision 1.4  2001/01/25 11:00:54  soltau
    temporary checkin to make Christian happy

    Revision 1.3  2001/01/18 13:50:31  soltau
    Fixed a couple of problems

    Revision 1.2  2001/01/11 17:52:58  soltau
    Added Vocab Mapper

    Revision 1.1  2001/01/10 18:47:49  soltau
    Initial revision


   ======================================================================== */

#include "svocab.h"
#include "mach_ind_io.h"

char svocabRCSVersion[]="$Id: svocab.c 2864 2009-02-16 21:18:17Z metze $";

/* ------------------------------------------------------------------------
    forward declarations 
   ------------------------------------------------------------------------ */

extern TypeInfo  swordInfo;
extern TypeInfo  svocabInfo;
static SVocab    svocabDefault;

/* ========================================================================
    Search Word
   ======================================================================== */

/* ------------------------------------------------------------------------
    swordInit
   ------------------------------------------------------------------------ */

static int swordInit( SWord* swordP, ClientData name)
{
  swordP->name      = strdup((char*)name);
  swordP->dictX     = 0;
  swordP->fillerTag = 0;
  return TCL_OK;
}

/* ------------------------------------------------------------------------
    swordDeinit
   ------------------------------------------------------------------------ */

static int swordDeinit( SWord* swordP)
{
  if (swordP->name) free(swordP->name);
  swordP->name      = NULL;
  swordP->dictX     = 0;
  swordP->fillerTag = 0;
  return TCL_OK;
}

/* ------------------------------------------------------------------------
    swordPutsItf
   ------------------------------------------------------------------------ */

static int swordPutsItf (ClientData cd, int argc, char *argv[])
{
  SWord * swordP = (SWord*)cd;
  int         ac = argc-1;

  if (itfParseArgv( argv[0], &ac,  argv+1,  1,
      NULL) != TCL_OK) return TCL_ERROR;

  itfAppendElement( "%s",swordP->name);
  itfAppendElement( "%d",swordP->dictX);
  itfAppendElement( "%d",swordP->fillerTag);

  return TCL_OK;
}

/* ------------------------------------------------------------------------
    swordConfigureItf
   ------------------------------------------------------------------------ */

static int swordConfigureItf (ClientData cd,  char *var, char *val)
{
  SWord * swordP = (SWord*)cd;

  if (! var) {
    ITFCFG(swordConfigureItf,cd,"fTag");
    ITFCFG(swordConfigureItf,cd,"dictX");
    return TCL_OK;
  }
  ITFCFG_Int(var,val,"fTag", swordP->fillerTag,0);
  ITFCFG_Int(var,val,"dictX",swordP->dictX,0);

  ERROR("unknown option '-%s %s'.\n", var, val ? val : "?");
  return TCL_ERROR;   
}

/* ========================================================================
    Search Vocabulary
   ======================================================================== */

/* ------------------------------------------------------------------------
    svocabInit
   ------------------------------------------------------------------------ */

static int svocabInit( SVocab* svocabP, char* name, Dictionary* dictP)
{
  svocabP->name  = strdup(name);
  svocabP->useN  = 0;
  svocabP->dictP = dictP;
  link(svocabP->dictP,"Dictionary");

  if ((UINT) dictP->list.itemN > SVX_MAX) {
    ERROR("Dictionary exceeded SVocab range : %d\n",SVX_MAX);
    return TCL_ERROR;
  }
  listInit((List*)&(svocabP->list),(TypeInfo*) &swordInfo, 
	   sizeof(SWord), svocabDefault.list.blkSize);

  svocabP->list.init    = (ListItemInit  *)swordInit;
  svocabP->list.deinit  = (ListItemDeinit*)swordDeinit;

  svocabP->string.start   = strdup(svocabDefault.string.start);
  svocabP->string.end     = strdup(svocabDefault.string.end);
  svocabP->string.nil     = strdup(svocabDefault.string.nil);
  return TCL_OK;
}

/* ------------------------------------------------------------------------
    svocabCreate
   ------------------------------------------------------------------------ */

static SVocab* svocabCreate (char* name, Dictionary* dictP)
{
  SVocab *svocabP = (SVocab*) malloc(sizeof(SVocab));
  
  if (! svocabP) {
    ERROR("svocabCreate: allocation failure\n");
    return NULL;
  }
  if (TCL_ERROR == (svocabInit(svocabP,name,dictP))) {    
    free(svocabP);
    return NULL;
  }
  return svocabP;
}

static ClientData svocabCreateItf( ClientData clientData, 
                                      int argc, char *argv[])
{
  char* name = NULL;
  Dictionary* dictP =  NULL;

  if ( itfParseArgv( argv[0], &argc, argv, 0,
      "<name>",  ARGV_STRING, NULL, &name, NULL,         "name of the vocabulary",
      "<Dictionary>",ARGV_OBJECT, NULL, &dictP, "Dictionary", "Dictionary",
       NULL) != TCL_OK) return NULL;
  
  return (ClientData) svocabCreate(name,dictP);
}

/* ------------------------------------------------------------------------
    svocabDeinit
   ------------------------------------------------------------------------ */

static int svocabDeinit( SVocab* svocabP)
{
  if (svocabP->useN >0) {
    ERROR("SVocab '%s' still in use by other objects.\n", 
            svocabP->name); 
    return TCL_ERROR;
  }
  delink(svocabP->dictP,"Dictionary");

  if (svocabP->name)         free(svocabP->name);
  if (svocabP->string.start) free(svocabP->string.start);
  if (svocabP->string.end)   free(svocabP->string.end);

  svocabP->name         = NULL;
  svocabP->string.start = NULL;
  svocabP->string.end   = NULL;
  svocabP->dictP        = NULL;

  return listDeinit((List*)&(svocabP->list));
}

/* ------------------------------------------------------------------------
    svocabFree
   ------------------------------------------------------------------------ */

static int svocabFree(SVocab *svocabP)
{
  if (svocabP) {
    if ( svocabDeinit(svocabP) != TCL_OK) return TCL_ERROR;
    free(svocabP);
  }
  return TCL_OK;
}

static int svocabFreeItf (ClientData clientData)
{
  return svocabFree((SVocab*)clientData);
}

/* ------------------------------------------------------------------------
    svocabAdd
   ------------------------------------------------------------------------ */

/* insert mode 0 : insert a word candidate from the language model
                   to guarantee the we can map svX and lvX 1:1
                   set the dict index to SVX_MAX to mark it as a candidate
   insert mode 1 : insert a word from the vocabulary list and set
                   the dictionary index
*/
extern SVX svocabAdd(SVocab *svocabP, char* name, UCHAR fillerTag, int mode, 
		     int dictX)
{
  int   idx = listIndex((List*) &(svocabP->list),name,0);

  /* nothing to do */
  if (idx >= 0 && mode == 0) 
    return (SVX) idx;

  /* Check with the dictionary */
  if (! mode) {
    dictX = SVX_MAX;
  } else {
    if (dictX <0) {
      dictX = dictIndex(svocabP->dictP,name);
    }
    if (dictX < 0 || (UINT) dictX > SVX_MAX) {
      ERROR("svocabAdd: cannot find pronunciation for %s\n", name);
      return SVX_NIL;
    }
  }

  /* Enough space? */
  if ((SVX) svocabP->list.itemN == SVX_MAX) {
    ERROR("svocabAdd: cannot add %s (exceeded SVX_MAX %d)\n",
	  name,SVX_MAX);
    return SVX_NIL;
  }

  if (mode == 0) {
    idx = listIndex((List*) &(svocabP->list),name,1);
  } else {
    /* if we create a new word (not a candidate) check for space
       from deleted words
    */
    if (idx <0) {
      idx = listIndex((List*) &(svocabP->list),svocabP->string.nil,0);
      if (idx >= 0) {
	/* re-use old list entry */
	long       hkey = svocabP->list.hashKey((ClientData)&(svocabP->string.nil));
	long       hmod = hkey % svocabP->list.hashSizeY;
	int         idx = 0;
	HashRec*  hashP = NULL;
	SWord*    itemP = NULL;

	/* get first hash record for nil word */
	if (hmod <0) hmod = -hmod;
	idx   = svocabP->list.hashTable[hmod];
	hashP = svocabP->list.hashXA+idx;

	/* remove hashP from nil's list */
	svocabP->list.hashTable[hmod] = hashP->next;

	/* hash index for new entry */
	hkey = svocabP->list.hashKey((ClientData)&name);
	hmod = hkey % svocabP->list.hashSizeY;
	if (hmod <0) hmod = -hmod;

	hashP->key  = hkey;
	hashP->next = svocabP->list.hashTable[hmod];
	svocabP->list.hashTable[hmod] = idx;

	itemP = svocabP->list.itemA+idx;
	if (itemP->name) free(itemP->name);
	itemP->name = strdup(name);
      } /* end re-use deleted item  */
    } /* end check for deleted item */
    if (idx <0) {
      idx = listIndex((List*) &(svocabP->list),name,1);
    }
  }
   
  svocabP->list.itemA[idx].dictX     = (SVX) dictX;
  svocabP->list.itemA[idx].fillerTag = fillerTag;

  return (SVX) idx;
}

static int svocabAddItf( ClientData clientData, 
			 int argc, char *argv[])
{
  int         ac  = argc - 1;
  SVocab* svocabP = (SVocab*)clientData;
  char*      name = NULL;
  int        ftag = 0;
  int        mode = 1;
  char*      pron = NULL;
  int       dictX = -1;

  if ( itfParseArgv( argv[0], &ac, argv+1, 0,
      "<word>",    ARGV_STRING, NULL, &name, NULL, "name",
      "-ftag",     ARGV_INT,    NULL, &ftag, NULL, "filler tag",
      "-fTag",     ARGV_INT,    NULL, &ftag, NULL, "filler tag (too)",
      "-pron",     ARGV_STRING, NULL, &pron, NULL, "pronunciation",
       NULL) != TCL_OK) return TCL_ERROR;

  if (pron) { 
    dictX = dictIndex(svocabP->dictP,pron);
    if (dictX <0) {
      ERROR("svocabAdd: cannot find dictionary entry for %s\n",pron);
      return TCL_ERROR;
    }
  }
  if (SVX_NIL != (svocabAdd(svocabP, name, (UCHAR)ftag, mode,dictX)))
    return TCL_OK;
  else
    return TCL_ERROR;
}

/* ------------------------------------------------------------------------
    svocabDelete
   ------------------------------------------------------------------------ */

static int svocabDel(SVocab* svocabP, SVX svX)
{
  SWord*   swordP = svocabP->list.itemA+svX;

  /*---------------*/
  /* START FF HACK */
  /*---------------*/
  /* what is the "&" for ? it sucks and it doesn't belong here anyway!!! */
  /* long       hkey = svocabP->list.hashKey((ClientData)&(swordP->name)); */
  long       hkey = svocabP->list.hashKey((ClientData)swordP->name);
  /*-------------*/
  /* END FF HACK */
  /*-------------*/
  long       hmod = hkey % svocabP->list.hashSizeY;
  int         idx = 0;
  HashRec* phashP = NULL;
  HashRec*  hashP = NULL;
  char*      name = NULL;

  if (swordP->dictX == SVX_MAX) {
    WARN("svocabDel: %s is a candidate, and will not be deleted\n",swordP->name);
    return TCL_OK;
  }

  /* get hash record for swordP */
  if (hmod <0) hmod = -hmod;
  idx   = svocabP->list.hashTable[hmod];
  /*---------------*/
  /* START FF HACK */
  /*---------------*/
  /* what if idx is -1 ?!? */
  /* add this !!! */
  if (idx <0) {
    ERROR("sovcabDel: couldn't find record for %s\n",swordP->name);
    return TCL_ERROR;
  }
  /*-------------*/
  /* END FF HACK */
  /*-------------*/
  hashP = svocabP->list.hashXA+idx;
  name  = svocabP->list.itemA[hashP->idx].name;

  while (idx >=0 && ! (0 == strcmp(name,swordP->name) && hashP->key == hkey)) {
    phashP = hashP;
    idx    = phashP->next;
    if (idx <0) break;
    hashP  = svocabP->list.hashXA+idx;
    name   = svocabP->list.itemA[hashP->idx].name;
  }
  if (idx <0) {
    ERROR("sovcabDel: couldn't find record for %s\n",swordP->name);
    return TCL_ERROR;
  }

  /* remove record from swordP list of records */
  if (! phashP) {
    svocabP->list.hashTable[hmod] = hashP->next;
  } else {
    phashP->next = hashP->next;
  }

  /* move hash record to 'IamtheNILword' list of records */
  hkey = svocabP->list.hashKey((ClientData)&(svocabP->string.nil));
  hmod = hkey % svocabP->list.hashSizeY;
  if (hmod <0) hmod = -hmod;
  hashP->next = svocabP->list.hashTable[hmod];
  hashP->key  = hkey;

  svocabP->list.hashTable[hmod] = idx;

  /* reset name */
  if (swordP->name) free (swordP->name);
  swordP->dictX = SVX_NIL;
  swordP->name  = strdup(svocabP->string.nil);

  return TCL_OK;
}

static int svocabDelItf( ClientData clientData, 
                                      int argc, char *argv[])
{
  int         ac  = argc - 1;
  SVocab* svocabP = (SVocab*)clientData;
  char*      name = NULL;
  int         svX = -1;

  if ( itfParseArgv( argv[0], &ac, argv+1, 0,
      "<word>",    ARGV_STRING, NULL, &name, NULL, "word to delete",
       NULL) != TCL_OK) return TCL_ERROR;  

  svX= listIndex((List*) &(svocabP->list),name,0);

  if (svX >= 0) {
    return svocabDel(svocabP,(SVX)svX);
  } else {
    WARN("svocabDelete: %s not found\n",name);
    return TCL_OK;
  }
}


/* ------------------------------------------------------------------------
    svocabSync
   ------------------------------------------------------------------------ */

static int svocabSync(SVocab* svocabP, int f,int v)
{
  SVX svX= 0;

  for (svX=0;svX<(SVX) svocabP->list.itemN;svX++) {
    SWord* swordP = svocabP->list.itemA+svX;
    int     dictX = dictIndex(svocabP->dictP,swordP->name);

    if (dictX == -1) dictX = SVX_MAX;
    if (!f && swordP->dictX == SVX_MAX) continue;

    if (dictX != (int) swordP->dictX)
      INFO("svocabSync: changed dictionary index for %s from %d to %d\n",
	   swordP->name,swordP->dictX,dictX);
    swordP->dictX = dictX;
  }
  return TCL_OK;
}

static int svocabSyncItf( ClientData clientData, 
                                      int argc, char *argv[])
{
  int         ac  = argc - 1;
  SVocab* svocabP = (SVocab*)clientData;
  int           f = 0;
  int           v = 0;

  if ( itfParseArgv( argv[0], &ac, argv+1, 0,
      "-f",    ARGV_INT, NULL, &f, NULL, "force update for word candidates",
      "-v",    ARGV_INT, NULL, &v, NULL, "verbose output",
       NULL) != TCL_OK) return TCL_ERROR;  

  return svocabSync(svocabP,f,v);
}


/* ------------------------------------------------------------------------
    svocabRead
   ------------------------------------------------------------------------ */

#define MAXLINE 2048

static int svocabRead(SVocab *svocabP, char* fileName)
{
  FILE*      fp = NULL;
  char** splitV = NULL;
  int    splitN = 0;
  int      mode = 1;
  char   line[MAXLINE];
  UCHAR fillerTag;

  if ((fp=fileOpen(fileName,"r")) == NULL) return TCL_ERROR;
  
  while ( fgets(line,MAXLINE,fp) != NULL) {
    if (TCL_ERROR == (Tcl_SplitList(itf,line,&splitN,&splitV))) {
      WARN("svocabRead: cannot split line '%s'\n",line);
      continue;
    }
    if (splitN == 1 || splitN == 2) {
      fillerTag = 0;
      if (splitN == 2) {
	fillerTag = (UCHAR) atoi(splitV[1]);
      }
      svocabAdd(svocabP,splitV[0],fillerTag,mode,-1);
    } else {
       WARN("svocabRead: cannot interpret line '%s'\n",line);
    }
    Tcl_Free((char*) splitV);
  }
  fileClose(fileName,fp);
  return TCL_OK;
}

static int svocabReadItf( ClientData clientData, 
                                      int argc, char *argv[])
{
  int         ac  =  argc - 1;
  SVocab* svocabP =  (SVocab*)clientData;
  char*      name = NULL;

  if ( itfParseArgv( argv[0], &ac, argv+1, 0,
      "<filename>",    ARGV_STRING, NULL, &name, NULL, "file name",
       NULL) != TCL_OK) return TCL_ERROR;  
  return (int) svocabRead(svocabP,name);
}

/* ------------------------------------------------------------------------
    svocabPuts
   ------------------------------------------------------------------------ */

static int svocabPutsItf (ClientData cd, int argc, char *argv[]) { 
  SVocab* svocabP = (SVocab*) cd;
  SWord*   swordP = svocabP->list.itemA;
  SVX         svN = svocabP->list.itemN;
  SVX svX;

  for (svX=0;svX<svN;svX++)
    if (swordP[svX].dictX != SVX_MAX && 
	0 != strcmp(swordP[svX].name,svocabP->string.nil))
      itfAppendResult ("%s ",swordP[svX].name);

  return TCL_OK;
}

/* ------------------------------------------------------------------------
    svocabLoad
   ------------------------------------------------------------------------ */

int svocabLoad(SVocab* svocabP, FILE* fp) { 
  int format = 0;
  UINT svxMAX = SVX_MAX;
  UINT  dictX = SVX_NIL;
  UINT    svX = SVX_NIL;
  int    tag = 0;
  SVX svY;
  int svN;
  char str[1024];

  if (! fp) {
    ERROR("svocabLoad: Invalid file pointer.\n");
    return TCL_ERROR;
  }
  read_string(fp,str);
  if (strncmp(str,"SVOCAB-BEGIN", 12)) {
    ERROR("svocabLoad: inconsistent dump file (expected SVocab, got '%s')\n", str);
    return TCL_ERROR;
  }
  if (!strcmp(str,"SVOCAB-BEGIN 1.1")) {
    format = 1;
    svxMAX = read_int(fp);
  }
  if (!strcmp(str,"SVOCAB-BEGIN 1.2")) {
    format = 2;
    svxMAX = read_int(fp);
  }
  if (format < 2) {
    ERROR("svocabLoad: dump-file outdated (found version %d, need version 1.2)\n",format);
    return TCL_ERROR;
  }
  svN= read_int(fp);
  if ((UINT) svN > SVX_MAX) {
    ERROR("svocabLoad: exceeded SVX_MAX (%d), change slimits to load vocabulary\n",
	  SVX_MAX);
    return TCL_ERROR;
  }

  for (svX=0;svX<(SVX) svN;svX++) {
    read_string(fp,str);
    dictX = read_int(fp);
    tag   = read_int(fp);
    if (dictX == svxMAX) {
      svY = svocabAdd(svocabP,str,(UCHAR) tag,0,-1);
    } else {
      svY = svocabAdd(svocabP,str,(UCHAR) tag,1,dictX);
      if (svocabP->list.itemA[svY].dictX != dictX) {
	ERROR("svocabLoad: inconsistent vocabulary index.\n");
	return TCL_ERROR;
      }
    }
    if (svX != svY) {
      ERROR("svocabLoad: inconsistent vocabulary.\n");
      return TCL_ERROR;
    }   
  }
  read_string(fp,str);
  if (0 != strcmp(str,"SVOCAB-END")) {
    ERROR("svocabLoad: inconsistent file.\n");
    return TCL_ERROR;
  }
  return TCL_OK;
}


static int svocabLoadItf( ClientData clientData, 
                                      int argc, char *argv[])
{
  int         ac  =  argc - 1;
  SVocab* svocabP =  (SVocab*)clientData;
  char*      name = NULL;
  FILE*        fp = NULL;

  if ( itfParseArgv( argv[0], &ac, argv+1, 0,
      "<filename>",    ARGV_STRING, NULL, &name, NULL, "file name",
       NULL) != TCL_OK) return TCL_ERROR;  

  fp = fileOpen(name,"r");
  svocabLoad(svocabP,fp);
  fileClose(name,fp);
  return TCL_OK;

}
 
/* ------------------------------------------------------------------------
    svocabSave
   ------------------------------------------------------------------------ */

int svocabSave(SVocab* svocabP, FILE* fp) { 
  SVX svX;
  if (! fp) {
    ERROR("svocabSave: Invalid file pointer\n");
    return TCL_ERROR;
  }

  write_string(fp,"SVOCAB-BEGIN 1.2");
  write_int   (fp,SVX_MAX);
  write_int   (fp,svocabP->list.itemN); 

  for (svX=0;svX<(SVX) svocabP->list.itemN;svX++) {
    SWord* swordP = svocabP->list.itemA+svX;
    write_string(fp,swordP->name);
    write_int   (fp,(int) swordP->dictX);
    write_int   (fp,(int) swordP->fillerTag);
  }
  write_string(fp,"SVOCAB-END");
  fflush(fp);
  return TCL_OK;
}


static int svocabSaveItf( ClientData clientData, 
                                      int argc, char *argv[])
{
  int         ac  =  argc - 1;
  SVocab* svocabP =  (SVocab*)clientData;
  char*      name = NULL;
  FILE*        fp = NULL;

  if ( itfParseArgv( argv[0], &ac, argv+1, 0,
      "<filename>",    ARGV_STRING, NULL, &name, NULL, "file name",
       NULL) != TCL_OK) return TCL_ERROR;  

  fp = fileOpen(name,"w");
  svocabSave(svocabP,fp);
  fileClose(name,fp);
  return TCL_OK;
}
  
/* ------------------------------------------------------------------------
    svocabAccess
   ------------------------------------------------------------------------ */

static ClientData svocabAccessItf( ClientData cd, char *name, TypeInfo **ti)
{ 
  SVocab* svocabP  = (SVocab*)cd;
  if (*name == '.') { 
    if (name[1]=='\0') {
      itfAppendElement( "dict");
      itfAppendElement( "list");
      *ti = NULL; return NULL;
    }
    else { 
      if (! strcmp( name+1, "dict")) {
        *ti = itfGetType("Dictionary");
        return (ClientData)svocabP->dictP;
      }
      if (! strcmp( name+1, "list")) {
	return listAccessItf( (ClientData)&(svocabP->list), name, ti);
      }
    }
  }
  return listAccessItf( (ClientData)&(svocabP->list), name, ti);
}

/* ------------------------------------------------------------------------
    svocabConfigure
   ------------------------------------------------------------------------ */

static int svocabConfigureItf (ClientData cd, char *var, char *val)
{
  SVocab* svocabP = (SVocab*) cd;
  UINT     svxMax = SVX_MAX;

  if (!svocabP) return TCL_ERROR;

  if (!var) {
    ITFCFG (svocabConfigureItf,cd,  "name");
    ITFCFG (svocabConfigureItf, cd, "useN");
    ITFCFG (svocabConfigureItf, cd, "startString");
    ITFCFG (svocabConfigureItf, cd, "endString");
    ITFCFG (svocabConfigureItf, cd, "nilString");
    ITFCFG (svocabConfigureItf, cd, "svxMax");
    return listConfigureItf ((ClientData)&(svocabP->list), var, val);
  }
  ITFCFG_CharPtr (var, val, "name",         svocabP->name,           1);
  ITFCFG_Int     (var, val, "useN",         svocabP->useN,           1);
  ITFCFG_CharPtr (var, val, "startString",  svocabP->string.start,   0);
  ITFCFG_CharPtr (var, val, "endString",    svocabP->string.end,     0);
  ITFCFG_CharPtr (var, val, "nilString",    svocabP->string.nil,     0);
  ITFCFG_Int     (var, val, "svxMax",       svxMax,                  1);
  return listConfigureItf ((ClientData)&(svocabP->list), var, val);   
}


/* ------------------------------------------------------------------------
    svocabIndex
   ------------------------------------------------------------------------ */

SVX svocabIndex (SVocab* svocabP, char* name)
{
  int idx = listIndex((List*) &(svocabP->list),name,0);
  if (idx < 0) 
    return SVX_NIL;
  else
    return (SVX) idx;
}

static int svocabIndexItf (ClientData clientData, int argc, char *argv[])
{
  SVocab* svocabP = (SVocab*)clientData;
  itfAppendResult("%d", svocabIndex(svocabP,argv[1]) );
  return TCL_OK;
}

/* ========================================================================
    Type Information
   ======================================================================== */

Method swordMethod[] = 
{
  { "puts", swordPutsItf, "displays the contents of a search vocabulary word"  },
  { NULL, NULL, NULL }
} ;

Method svocabMethod[] = 
{
  { "puts",   svocabPutsItf, "displays the contents of a search vocabulary"  },
  { "read",   svocabReadItf, "read Vocabulary from file"},
  { "index",  svocabIndexItf,"return the internal index of a search vocab word" },
  { "add",    svocabAddItf,  "add a word to the vocabulary"},
  { "delete", svocabDelItf,  "delete a word from the vocabulary"},
  { "sync",   svocabSyncItf, "synchronize vocabulary with dictionary"},
  { "load",   svocabLoadItf, "load Vocabulary from binary file"},
  { "save",   svocabSaveItf, "save Vocabulary to binary file"},
  { NULL, NULL, NULL }
} ;

TypeInfo swordInfo = { 
        "SWord", 0, 0, swordMethod, 
        NULL, NULL, swordConfigureItf, NULL, 
        itfTypeCntlDefaultNoLink,
        "Search Vocabulary Word" };

TypeInfo svocabInfo = 
{ "SVocab", 0, -1,  svocabMethod, 
   svocabCreateItf, svocabFreeItf, 
   svocabConfigureItf, svocabAccessItf, NULL,
  "Search Vocabulary"
};

static int svocabInitialized = FALSE;

extern int SVocab_Init (void)
{
  if (! svocabInitialized) {
    if ( Dictionary_Init() != TCL_OK) return TCL_ERROR;

    svocabDefault.name            = NULL;
    svocabDefault.useN            = 0;
    svocabDefault.string.start    = "(";
    svocabDefault.string.end      = ")";  
    svocabDefault.string.nil      = "IamtheNILword";

    svocabDefault.list.itemN      = 0;
    svocabDefault.list.blkSize    = 500;
    svocabDefault.dictP           = NULL;

    itfNewType (&swordInfo);
    itfNewType (&svocabInfo);

    svocabInitialized = TRUE;
  }
  return TCL_OK;
}
