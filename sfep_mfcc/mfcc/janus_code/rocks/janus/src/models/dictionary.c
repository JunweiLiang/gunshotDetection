/* ========================================================================
    JANUS      Speech- to Speech Translation
               ------------------------------------------------------------
               Object: Acoustic Dictionary
               ------------------------------------------------------------

    Author  :  Ivica Rogina & Michael Finke
    Module  :  dictionary.c
    Date    :  $Id: dictionary.c 2390 2003-08-14 11:20:32Z fuegen $
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
    Revision 3.5  2003/08/14 11:20:14  fuegen
    Merged changes on branch jtk-01-01-15-fms (jaguar -> ibis-013)

    Revision 3.4.20.7  2003/04/05 15:14:12  metze
    Cosmetics

    Revision 3.4.20.6  2002/12/20 14:53:28  metze
    Error messages

    Revision 3.4.20.5  2002/11/15 13:54:49  metze
    Removed warnings on SunOS

    Revision 3.4.20.4  2002/08/27 08:44:21  metze
    itfParseArgv now uses <name> and NULL initialization for char*

    Revision 3.4.20.3  2002/02/04 14:26:38  metze
    Added option to dictWrite (pronunciation file for
    Lidia Mangu's consensus scoring)

    Revision 3.4.20.2  2002/02/04 14:25:22  metze
    Changed interface to wordPuts

    Revision 3.4.20.1  2001/06/28 10:48:21  soltau
    Added Load/save routines for binary files to support fast decoder startup

    Revision 3.4  2000/11/14 12:01:11  janus
    Merged branch jtk-00-10-27-jmcd.

    Revision 3.3.30.2  2000/11/08 17:06:56  janus
    Finished making changes required for compilation under 'gcc -Wall'.

    Revision 3.3.30.1  2000/11/06 10:50:26  janus
    Made changes to enable compilation under -Wall.

    Revision 3.3  1997/07/25 17:38:31  tschaaf
    gcc / DFKI Clean

    Revision 3.2  1997/06/17 17:41:43  westphal
    *** empty log message ***

    Revision 2.4  96/02/26  06:11:29  finkem
    modified findQuestion to give alarm if there are less than two
    successors to a node
    
    Revision 2.3  1996/02/19  16:30:24  kemp
    Subtle change with heavy consequences: for the puts method of the dictionary,
    no longer itfAppendResult is used, but itfAppendElement. Now each dictionary
    entry is printed as a two-element list <name> <transcription>, and getting
    either of it with lindex is okay. This was necessary for GSST, where we have
    lots of words that begin with " and the like.

    Revision 2.2  1996/02/17  19:25:21  finkem
    added wbTags, weTags and xwTags to be used in the search and vocab

    Revision 2.1  1996/02/03  05:54:13  finkem
    removed GetItf sind a Janus3 does that job already (and does not
    end in a Seg.Fault as Get did, btw.)

    Revision 2.0  1996/01/31  05:20:55  finkem
    new senoneSet setup, new dictionary words and pron. variants

    Revision 1.13  1996/01/05  17:19:27  finkem
    removed ref to old ERR function

    Revision 1.12  1995/11/15  01:00:17  torsten
    added dict write

    Revision 1.11  1995/10/14  15:44:38  torsten
    Got rid of listSOrt warning message

    Revision 1.10  1995/09/05  16:05:51  kemp
    *** empty log message ***

    Revision 1.9  1995/08/27  22:36:00  finkem
    *** empty log message ***

    Revision 1.8  1995/08/17  17:06:02  rogina
    *** empty log message ***

    Revision 1.7  1995/08/14  13:18:49  finkem
    *** empty log message ***

    Revision 1.6  1995/08/01  22:16:03  finkem
    *** empty log message ***

    Revision 1.5  1995/08/01  15:43:06  rogina
    *** empty log message ***

    Revision 1.4  1995/07/26  13:35:17  finkem
    *** empty log message ***

    Revision 1.3  1995/07/25  22:33:21  finkem
    *** empty log message ***

    Revision 1.2  1995/07/25  14:45:33  rogina
    *** empty log message ***

    Revision 1.1  1995/07/25  13:49:57  finkem
    Initial revision

 
   ======================================================================== */

#include "common.h"
#include "dictionary.h"
#include "itf.h"
#include "error.h"
#include "mach_ind_io.h"

char dictionaryRCSVersion[]= 
           "@(#)1$Id: dictionary.c 2390 2003-08-14 11:20:32Z fuegen $";

/* ------------------------------------------------------------------------
    Forward Declaration
   ------------------------------------------------------------------------ */

extern TypeInfo dictionaryInfo;



/* ========================================================================
    DictWord
   ======================================================================== */

static int dictWordPutsItf (ClientData cd, int argc, char *argv[]);

/* ------------------------------------------------------------------------
    dictWordInit   initialize dictWord structure
   ------------------------------------------------------------------------ */

static int dictWordInit (DictWord* dictWord, ClientData CD)
{
  assert(dictWord);
  dictWord->name    =  ( CD) ? strdup((char*)CD) : NULL;
  dictWord->varX    =   -1;

  return wordInit( &(dictWord->word), NULL, NULL);
}

/* ------------------------------------------------------------------------
    dictWordDeinit  deinitialize dictWord structure
   ------------------------------------------------------------------------ */

static int dictWordDeinit (DictWord* dictWord)
{
  if (!dictWord) return TCL_OK;
  if ( dictWord->name)   { free(dictWord->name);   dictWord->name   = NULL; }
  return TCL_OK;
}

/* ------------------------------------------------------------------------
    dictWordConfigureItf
   ------------------------------------------------------------------------ */

static int dictWordConfigureItf( ClientData cd, char *var, char *val)
{
  DictWord* dictWord = (DictWord*)cd;
  if (! dictWord) return TCL_ERROR;

  if (! var) {
    ITFCFG(dictWordConfigureItf,cd,"name");
    ITFCFG(dictWordConfigureItf,cd,"variant");
    ITFCFG(dictWordConfigureItf,cd,"itemN");
    return TCL_OK;
  }
  ITFCFG_CharPtr( var,val,"name",         dictWord->name,       1);
  ITFCFG_Int(     var,val,"variant",      dictWord->varX,       0);
  ITFCFG_Int(     var,val,"itemN",        dictWord->word.itemN, 1);
  ERROR("Unknown option '-%s %s'.\n", var, val ? val : "?");
  return TCL_ERROR;
}

/* ------------------------------------------------------------------------
    dictWordPutsItf
   ------------------------------------------------------------------------ */

static int dictWordPuts( DictWord* dictWord)
{
  itfAppendElement("%s", dictWord->name);
  itfAppendElement("%s", wordPuts(&(dictWord->word), 1));
  /* itfAppendResult("%s {%s}", dictWord->name, wordPuts(&(dictWord->word), 1)); */
  return TCL_OK;
}

static int dictWordPutsItf( ClientData cd, int argc, char *argv[])
{
  int   ac   =  argc-1;
  DictWord* dictWord = (DictWord*)cd;

  if ( itfParseArgv( argv[0], &ac, argv+1, 0, NULL) != TCL_OK)
       return TCL_ERROR;

  return dictWordPuts( dictWord);
}

/* ========================================================================
    Dictionary
   ======================================================================== */

static Dictionary dictionaryDefault;

/* ------------------------------------------------------------------------
    Create Dictionary Structure
   ------------------------------------------------------------------------ */

int dictInit( Dictionary* dict, char* name, Phones* phones, Tags* tags)
{
  int idx;

  assert( phones);
  assert( tags);

  dict->name = strdup(name);
  dict->useN = 0;

  listInit((List*)&(dict->list), itfGetType("DictWord"), sizeof(DictWord),
                    dictionaryDefault.list.blkSize);

  dict->list.init    = (ListItemInit  *)dictWordInit;
  dict->list.deinit  = (ListItemDeinit*)dictWordDeinit;

  dict->phones       =  phones; link( phones, "Phones");
  dict->tags         =  tags;   link( tags,   "Tags");

  /* initialize word begin, word end and the other cross word relevant
     tags to their default values */

  dict->wbTags       = ((idx = tagsIndex( tags, "WB")) > -1) ? 1<<idx : 0;
  dict->weTags       = ((idx = tagsIndex( tags, "WE")) > -1) ? 1<<idx : (
                       ((idx = tagsIndex( tags, "WB")) > -1) ? 1<<idx : 0);
  dict->xwTags       =  0;

  for (   idx = 0; idx < tagsNumber(tags); idx++) {
    int   tag = 1 << idx;
    if (!(tag & (dict->wbTags | dict->weTags))) dict->xwTags |= tag;
  }

  dict->commentChar  =  dictionaryDefault.commentChar;
  return TCL_OK;
}

Dictionary* dictCreate (char* name, Phones* phones, Tags* tags)
{
  Dictionary *dict = (Dictionary*)malloc(sizeof(Dictionary));

  if (! dict || dictInit( dict, name, phones, tags) != TCL_OK) {
     if ( dict) free( dict);
     ERROR("Failed to allocate Dictionary object '%s'.\n", name);
     return NULL; 
  }
  return dict;
}

static ClientData dictCreateItf (ClientData cd, int argc, char *argv[])
{
  Phones *phones = NULL;
  Tags   *tags   = NULL;
  char     *name = NULL; 

  if (itfParseArgv(argv[0], &argc, argv, 0,
      "<name>",   ARGV_STRING, NULL, &name, NULL, "name of the dictionary",
      "<Phones>", ARGV_OBJECT, NULL,&phones,"Phones","phones",
      "<Tags>",   ARGV_OBJECT, NULL,&tags,  "Tags",  "tags",
      NULL) != TCL_OK)
    return NULL;

  return (ClientData)dictCreate(name, phones, tags);
}

/* ------------------------------------------------------------------------
    Free Dictionary Structure
   ------------------------------------------------------------------------ */

int dictLinkN( Dictionary* dict)
{
  int  useN = listLinkN((List*)&(dict->list)) - 1;
  if ( useN < dict->useN) return dict->useN;
  else                    return useN;
}

int dictDeinit (Dictionary *dict)
{
  if (dict==NULL) { ERROR("null argument"); return TCL_ERROR; }

  if ( dictLinkN( dict)) {
    SERROR("Dictionary '%s' still in use by other objects.\n", dict->name);
    return TCL_ERROR;
  } 
  if (dict->name) { free(dict->name); dict->name = NULL; }

  if (dict->phones) unlink( dict->phones, "Phones");
  if (dict->tags)   unlink( dict->tags,   "Tags");

  return listDeinit((List*)&(dict->list));
}

int dictFree (Dictionary *dict)
{
  if (dictDeinit(dict)!=TCL_OK) return TCL_ERROR;
  free(dict);
  return TCL_OK;
}

static int dictFreeItf(ClientData cd)
{
  return dictFree((Dictionary*)cd);
}

/* ------------------------------------------------------------------------
    dictConfigureItf
   ------------------------------------------------------------------------ */

static int dictConfigureItf (ClientData cd, char *var, char *val)
{
  Dictionary *dict = (Dictionary*)cd;
  if (dict==NULL) dict = &dictionaryDefault;

  if (var==NULL) 
    {
      ITFCFG(dictConfigureItf,cd,"useN");
      ITFCFG(dictConfigureItf,cd,"phones");
      ITFCFG(dictConfigureItf,cd,"tags");
      ITFCFG(dictConfigureItf,cd,"wbTags");
      ITFCFG(dictConfigureItf,cd,"weTags");
      ITFCFG(dictConfigureItf,cd,"xwTags");
      ITFCFG(dictConfigureItf,cd,"commentChar");
      return listConfigureItf((ClientData)&(dict->list), var, val);
    }
  ITFCFG_Int(    var,val,"useN",   dict->useN,               1);
  ITFCFG_Object( var,val,"phones", dict->phones,name,Phones, 1);
  ITFCFG_Object( var,val,"tags",   dict->tags,  name,Tags,   1);
  ITFCFG_Char(   var,val,"commentChar",  dict->commentChar,  0);

  if (! strcmp(var,"wbTags")) {
    if (! val) { 
      unsigned int j; 
      int t = dict->wbTags;
      itfAppendResult("{");
      for ( j = 0; j < 8*sizeof(int); j++)
        if ( t & (1 << j)) itfAppendResult(" %s", tagsName(dict->tags,j));
      itfAppendResult("}");
      return TCL_OK;
    }
    return TCL_ERROR;
  }
  if (! strcmp(var,"weTags")) {
    if (! val) { 
      unsigned int j;
      int t = dict->weTags;
      itfAppendResult("{");
      for ( j = 0; j < 8*sizeof(int); j++)
        if ( t & (1 << j)) itfAppendResult(" %s", tagsName(dict->tags,j));
      itfAppendResult("}");
      return TCL_OK;
    }
    return TCL_ERROR;
  }
  if (! strcmp(var,"xwTags")) {
    if (! val) { 
      unsigned int j; 
      int t = dict->xwTags;
      itfAppendResult("{");
      for ( j = 0; j < 8*sizeof(int); j++)
        if ( t & (1 << j)) itfAppendResult(" %s", tagsName(dict->tags,j));
      itfAppendResult("}");
      return TCL_OK;
    }
    return TCL_ERROR;
  }
  return listConfigureItf((ClientData)&(dict->list), var, val);   
}

/* ------------------------------------------------------------------------
    dictAccessItf
   ------------------------------------------------------------------------ */

static ClientData dictAccessItf (ClientData cd, char *name, TypeInfo **ti)
{ 
  Dictionary* dict = (Dictionary*)cd;

  if (*name == '.') { 
    if (name[1]=='\0') {
       itfAppendElement( "phones");
       itfAppendElement( "tags");
       *ti = NULL;
    }
    else { 
      if (! strcmp( name+1, "phones")) {
        *ti = itfGetType("Phones");
        return (ClientData)dict->phones;
      }
      else if (! strcmp( name+1, "tags")) {
        *ti = itfGetType("Phones");
        return (ClientData)dict->tags;
      }
    }
  }
  return listAccessItf((ClientData)&(dict->list),name,ti);
}

/* ------------------------------------------------------------------------
    dictPutsItf
   ------------------------------------------------------------------------ */

static int dictPutsItf (ClientData cd, int argc, char *argv[])
{
  Dictionary *dict  = (Dictionary*)cd;
  return listPutsItf((ClientData)&(dict->list), argc, argv);
}

/* ------------------------------------------------------------------------
    dictGetPhone, dictGetPhoneN
   ------------------------------------------------------------------------ */

int dictGetPhone( Dictionary* dict, char* name)
{ return phonesIndex( dict->phones, name); }

int dictGetPhoneN( Dictionary* dict)
{ return phonesNumber( dict->phones); }

/* ------------------------------------------------------------------------
    Get
   ------------------------------------------------------------------------ */

int dictGetFast( Dictionary *dict, int idx, 
                 int *phonesN, int **phones, int **tags)
{
  if (idx<0) { *phonesN=0; *phones=NULL; *tags=NULL; return idx; }

  *phonesN = dict->list.itemA[idx].word.itemN;
  *phones  = dict->list.itemA[idx].word.phoneA;
  *tags    = dict->list.itemA[idx].word.tagA;

  return idx;
}

int dictGet( Dictionary *dict, char *spell, 
                               int *phonesN, int **phones, int **tags)
{
  return dictGetFast( dict, dictIndex( dict, spell), 
                      phonesN, phones, tags); 
}

/* ------------------------------------------------------------------------
    dictIndex
   ------------------------------------------------------------------------ */

int dictIndex( Dictionary* dict,  char* phone) 
{
  return listIndex((List*)&(dict->list), phone, 0);
}

static int dictIndexItf( ClientData clientData, int argc, char *argv[] )
{
  Dictionary* dict = (Dictionary*)clientData;
  return listName2IndexItf((ClientData)&(dict->list), argc, argv);
}

/* ------------------------------------------------------------------------
    dictName
   ------------------------------------------------------------------------ */

char* dictName( Dictionary* dict, int i) 
{ 
  return (i < 0) ? "(null)" : dict->list.itemA[i].name;
}

static int dictNameItf( ClientData clientData, int argc, char *argv[])
{
  Dictionary* dict = (Dictionary*)clientData;
  return listIndex2NameItf((ClientData)&(dict->list), argc, argv);
}

/* ------------------------------------------------------------------------
    dictNumber
   ------------------------------------------------------------------------ */

static int dictNumber( Dictionary* dict) { return dict->list.itemN; }


/* ------------------------------------------------------------------------
    dictDeleteItf
   ------------------------------------------------------------------------ */

static int dictDeleteItf (ClientData cd, int argc, char *argv[])
{
  Dictionary* dict  = (Dictionary*)cd;
  return listDeleteItf((ClientData)&(dict->list), argc, argv);
}

/* ------------------------------------------------------------------------
    dictAdd
   ------------------------------------------------------------------------ */

static int dictAdd (Dictionary *dict, char* name, Word* wordP)
{
  char* cP;
  int   idx;

  if ( wordP->phonesP != dict->phones || wordP->tagsP != dict->tags) {
    ERROR("Word is defined over %s/%s and not over %s/%s.\n",
           wordP->phonesP->name, wordP->tagsP->name,
           dict->phones->name,   dict->tags->name);
    return TCL_ERROR;
  }
  if ((idx = listIndex((List*)&(dict->list), (ClientData)name, 0)) >= 0) {
    ERROR("DictWord '%s' already exists in '%s'.\n", name, dict->name);
    wordDeinit( wordP);
    return idx;
  }
  MSGCLEAR(NULL);

  idx = listNewItem((List*)&(dict->list), (ClientData)name);

  /* handle pronunciation variants */

  if ( (cP = strchr(name+1,'(')) ) /* ) this comment keeps emacs happy */ {
    int i;

    name[cP-name] = '\0';
    i = listIndex((List*)&(dict->list), (ClientData)name, 0);
    name[cP-name] = '(';

    if (i < 0) WARN("Variant '%s' occured before base form.\n",name); 
    else {
      dict->list.itemA[idx].varX = dict->list.itemA[i].varX;
      dict->list.itemA[i].varX   = idx; 
    }
  } 
  dict->list.itemA[idx].word.itemN   = wordP->itemN;
  dict->list.itemA[idx].word.phoneA  = wordP->phoneA;
  dict->list.itemA[idx].word.tagA    = wordP->tagA;
  dict->list.itemA[idx].word.phonesP = wordP->phonesP;
  dict->list.itemA[idx].word.tagsP   = wordP->tagsP;
  return idx;
}

static int dictAddItf (ClientData cd, int argc, char *argv[])
{
  Dictionary *dict   = (Dictionary*)cd;
  int         ac     =  argc - 1;
  char       *name   =  NULL;
  Word        word;
  int         idx;

  wordInit( &word, dict->phones, dict->tags);

  if ( itfParseArgv( argv[0], &ac, argv+1, 0,
      "<name>", ARGV_STRING, NULL, &name, cd, 
      "name (spelling) of the word",
      "<pronunciation>", ARGV_CUSTOM, getWord, &word, cd, 
      "pronunciation of the word", NULL) != TCL_OK) {
    wordDeinit( &word);
    return TCL_ERROR;
  }
  if ((idx = dictAdd(dict, name, &word)) < 0) return TCL_ERROR;
  else                                        return TCL_OK;
}

/* ------------------------------------------------------------------------
    dictRead
   ------------------------------------------------------------------------ */

static int dictReadItf (ClientData cd, int argc, char *argv[])
{
  Dictionary *dict     = (Dictionary*)cd;
  int         ac       =  argc - 1;
  char*       fileName =  NULL;
  int         n;

  if (itfParseArgv(argv[0], &ac, argv+1, 0, 
     "<filename>", ARGV_STRING, NULL, &fileName, cd, "file to read from", 
       NULL) != TCL_OK) return TCL_ERROR;

  itfAppendResult( "%d", n = freadAdd( cd, fileName, dict->commentChar, 
                                                     dictAddItf));
  return (n >= 0) ? TCL_OK : TCL_ERROR;
}

/* ------------------------------------------------------------------------
    dictWrite
   ------------------------------------------------------------------------ */

static int dictWrite (Dictionary* dict, char* fileName)
{
  int w;
  FILE *fp;

  if (! (fp = fileOpen(fileName,"w"))) return TCL_ERROR;

  fprintf(fp, "%c -------------------------------------------------------\n", dict->commentChar);
  fprintf(fp, "%c  Name            : %s\n", dict->commentChar, dict->name);
  fprintf(fp, "%c  Type            : Dictionary\n", dict->commentChar);
  fprintf(fp, "%c  Number of Items : %d\n", dict->commentChar, dict->list.itemN);
  fprintf(fp, "%c  Date            : %s", dict->commentChar, dateString());
  fprintf(fp, "%c -------------------------------------------------------\n", dict->commentChar);

  for(w=0;w < dictNumber (dict); w++) {
    fprintf(fp,"{%s} {%s}\n", dict->list.itemA[w].name,
                              wordPuts(&(dict->list.itemA[w].word), 1));
  }
  fileClose( fileName, fp);
  return TCL_OK;
}

/* Lidia Mangu's Consensus code requires baseform pronunciations for every
   word along with the number of pronunciation variants. This procedure allows
   our dictionary object to produce this file. However, Lidia says:

   # output FORMAT: <word>=<best_pron>;<#prons>
   # If the file has no probability distribution on pronunciations, the 
   # assumption is that the first listed pronunciation is the most likely one
   # If you have a file with probabilities, you have to modify this script
   # But NOTE: it seems that the phonetic similarity doesn't play a very important 
   # role in the clustering procedure, so I don't think that it is really worth doing it
*/

static int dictWriteLidia (Dictionary* dictP, char* fileName)
{
  int dictX, dictY, dictZ;
  char buf[512];
  FILE *fp;

  if (!(fp = fileOpen (fileName, "w")))
    return TCL_ERROR;

  for (dictX = 0; dictX < dictNumber (dictP); dictX++) {
    if (dictP->list.itemA[dictX].varX == -1) {
      int pv = 1;
      dictZ = dictX;
      strncpy (buf, dictP->list.itemA[dictX].name, 512);
      if      (streq (buf, "(")) strcpy (buf, "!SENT_START");
      else if (streq (buf, ")")) strcpy (buf, "!SENT_END");
      else if (strchr (buf+1, '(')) { /* ')' keeps emacs happy */
	*strchr (buf, '(') = 0;
	dictY = dictZ = listIndex ((List*)&(dictP->list), (ClientData) buf, 0);
	while (dictY != dictX) 
	  pv++, dictY = dictP->list.itemA[dictY].varX;
      }
      
      fprintf (fp, "%s=%s;%d\n", buf,
	       wordPuts (&(dictP->list.itemA[dictZ].word), 0)+1, pv);
    }
  }

  fileClose (fileName, fp);
  return TCL_OK;
}

static int dictWriteItf (ClientData cd, int argc, char *argv[])
{
  Dictionary *dict = (Dictionary*)cd;
  int         ac         =  argc - 1;
  char*       fileName   =  NULL;
  char*       fmt        =  "janus";

  if (itfParseArgv(argv[0], &ac, argv+1, 0, 
                  "<filename>", ARGV_STRING, NULL, &fileName, cd,   "file to write to",
		   "-format",   ARGV_STRING, NULL, &fmt,      NULL, "file format (janus or htk)",
		   NULL) != TCL_OK) 
    return TCL_ERROR;

  if (streq (fmt, "janus")) return dictWrite      (dict, fileName);
  else                      return dictWriteLidia (dict, fileName);
}

/* ------------------------------------------------------------------------
    dictLoad
   ------------------------------------------------------------------------ */

int dictLoad (Dictionary* dictP, FILE* fp)
{
  int  dictX, dictN;
  char str[1024];
  Word word;

  wordInit( &word, dictP->phones, dictP->tags);

  if (! fp) {
    ERROR("dictLoad: Invalid file pointer\n");
    return TCL_ERROR;
  }
  if (dictP->list.itemN > 0) {
    ERROR("dictLoad: expected empty dictionary\n");
    return TCL_ERROR;
  }
  read_string(fp,str);
  if (0 != strcmp(str,"DICTIONARY-BEGIN")) {
    ERROR("dictLoad: inconsistent file\n");
    return TCL_ERROR;
  }

  dictN= read_int(fp);
  for (dictX=0;dictX<dictN;dictX++) {
    read_string(fp,str);
    word.itemN = read_int(fp);
    if (NULL == (word.phoneA = malloc(word.itemN*sizeof(int)))) {
      ERROR("dictLoad: allocation failure\n");
      return TCL_ERROR;
    }
    if (NULL == (word.tagA = malloc(word.itemN*sizeof(int)))) {
      ERROR("dictLoad: allocation failure\n");
      return TCL_ERROR;
    }
    read_ints(fp,word.phoneA,word.itemN);
    read_ints(fp,word.tagA,  word.itemN);
    dictAdd(dictP,str,&word);
  }

  read_string(fp,str);
  if (0 != strcmp(str,"DICTIONARY-END")) {
    ERROR("dictLoad: inconsistent file\n");
    return TCL_ERROR;
  }

  return TCL_OK;
}

static int dictLoadItf (ClientData cd, int argc, char *argv[])
{
  Dictionary *dict = (Dictionary*)cd;
  int         ac         =  argc - 1;
  char*       fileName   =  NULL;
  FILE        *fp        =  NULL;

  if (itfParseArgv(argv[0], &ac, argv+1, 0, 
                  "<filename>", ARGV_STRING, NULL, &fileName, cd, 
                  "file to read from", NULL) != TCL_OK) 
    return TCL_ERROR;

  fp = fileOpen(fileName,"r");
  dictLoad(dict,fp);
  fileClose(fileName,fp);

  return TCL_OK;
}

/* ------------------------------------------------------------------------
    dictSave
   ------------------------------------------------------------------------ */

int dictSave (Dictionary* dictP, FILE* fp)
{
  int dictX;
  if (! fp) {
    ERROR("dictSave: Invalid file pointer\n");
    return TCL_ERROR;
  }

  write_string(fp,"DICTIONARY-BEGIN");
  write_int   (fp,dictP->list.itemN); 

  for (dictX=0;dictX<dictP->list.itemN;dictX++) {
    DictWord* dwordP = dictP->list.itemA+dictX;
    write_string(fp,dwordP->name);
    write_int   (fp,dwordP->word.itemN);
    write_ints  (fp,dwordP->word.phoneA,dwordP->word.itemN);
    write_ints  (fp,dwordP->word.tagA,  dwordP->word.itemN);
  }
  write_string(fp,"DICTIONARY-END");
  fflush(fp);
  return TCL_OK;
}

static int dictSaveItf (ClientData cd, int argc, char *argv[])
{
  Dictionary *dict = (Dictionary*)cd;
  int         ac         =  argc - 1;
  char*       fileName   =  NULL;
  FILE        *fp        =  NULL;

  if (itfParseArgv(argv[0], &ac, argv+1, 0, 
                  "<filename>", ARGV_STRING, NULL, &fileName, cd, 
                  "file to write into", NULL) != TCL_OK) 
    return TCL_ERROR;

  fp = fileOpen(fileName,"w");
  dictSave(dict,fp);
  fileClose(fileName,fp);

  return TCL_OK;
}

/* ========================================================================
    Type Declaration
   ======================================================================== */

static Method dictWordMethod[] = 
{ 
  { "puts", dictWordPutsItf,   NULL },
  {  NULL,  NULL, NULL } 
};

TypeInfo dictWordInfo = { 
        "DictWord", 0, -1, dictWordMethod, 
         NULL, NULL,
         dictWordConfigureItf, NULL,
         itfTypeCntlDefaultNoLink, 
        "Word with tagged phone transcription\n" };

static Method dictionaryMethod[] = 
{ 
  { "puts",   dictPutsItf,   "display the contents of a dictionary" },
  { "add",    dictAddItf,    "add a new word to the set"            },
  { "delete", dictDeleteItf, "remove word from the set"             },
  { "read",   dictReadItf,   "reads a dictionary file"              },
  { "write",  dictWriteItf,  "writes a dictionary file"             },
  { "index",  dictIndexItf,  "return the internal index of a word"        },
  { "name",   dictNameItf,   "return the spelled word given the index"    },
  { "load",   dictLoadItf,   "load a dictionary from a binary file"       },
  { "save",   dictSaveItf,   "save a dictionary file into a binary file"   },
  {  NULL,    NULL,           NULL } 
};

TypeInfo dictionaryInfo = {
        "Dictionary", 0, -1, dictionaryMethod, 
         dictCreateItf,dictFreeItf,
         dictConfigureItf,dictAccessItf, NULL,
	"Set of words\n" } ;

static int dictionaryInitialized = 0;

int Dictionary_Init ( void )
{
  if (! dictionaryInitialized) {
    if (Tags_Init()    != TCL_OK) return TCL_ERROR;
    if (Phones_Init()  != TCL_OK) return TCL_ERROR;
    if (Word_Init()    != TCL_OK) return TCL_ERROR;

    dictionaryDefault.name            = NULL;
    dictionaryDefault.useN            = 0;
    dictionaryDefault.list.itemN      = 0;
    dictionaryDefault.list.blkSize    = 100;
    dictionaryDefault.phones          = NULL;
    dictionaryDefault.tags            = NULL;
    dictionaryDefault.commentChar     = ';';

    itfNewType (&dictWordInfo);
    itfNewType (&dictionaryInfo);

    dictionaryInitialized = 1;
  }
  return TCL_OK;
}
