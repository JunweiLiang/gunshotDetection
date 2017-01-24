/* ========================================================================
    JANUS      Speech- to Speech Translation
               ------------------------------------------------------------
               Object: Phone Tags
               ------------------------------------------------------------

    Author  :  Ivica Rogina & Michael Finke
    Module  :  tags.c
    Date    :  $Id: tags.c 2390 2003-08-14 11:20:32Z fuegen $
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
    Revision 4.4  2003/08/14 11:20:18  fuegen
    Merged changes on branch jtk-01-01-15-fms (jaguar -> ibis-013)

    Revision 4.3.20.4  2003/07/02 17:49:27  fuegen
    fixes for file handling under Windows
    using general functions for file open/close

    Revision 4.3.20.3  2002/10/04 09:39:38  metze
    Improved error message.

    Revision 4.3.20.2  2002/08/27 08:41:16  metze
    itfParseArgv now uses <name> and NULL initialization for char*

    Revision 4.3.20.1  2002/06/26 11:57:56  fuegen
    changes for new directory structure support and code cleaning

    Revision 4.3  2000/11/14 12:35:26  janus
    Merging branch jtk-00-10-27-jmcd.

    Revision 4.2.34.2  2000/11/08 17:31:38  janus
    Finished making changes required for compilation under 'gcc -Wall'.

    Revision 4.2.34.1  2000/11/06 10:50:40  janus
    Made changes to enable compilation under -Wall.

    Revision 4.2  2000/08/16 11:44:33  soltau
    Free -> TclFree

    Revision 4.1  2000/01/12 10:12:49  fuegen
    add a Maks to demask modality tags

    Revision 4.0  1998/11/30 18:07:14  fuegen
    *** empty log message ***

    Revision 3.3  1997/07/25 17:28:16  tschaaf
    gcc / DFKI -Clean

    Revision 3.2  1997/06/17 17:41:43  westphal
    *** empty log message ***

 * Revision 1.8  96/02/07  17:05:47  rogina
 * added checking for out-of-bounds in name method
 * 
 * Revision 1.7  1996/01/01  16:26:47  finkem
 * *** empty log message ***
 *
 * Revision 1.6  1996/01/01  16:20:47  finkem
 *  handling of parsing errors in the Read function
 *  remove INFO output when reading
 *
 * Revision 1.5  1995/09/06  07:40:56  kemp
 * *** empty log message ***
 *
 * Revision 1.4  1995/08/27  22:33:48  finkem
 * *** empty log message ***
 *
 * Revision 1.3  1995/08/17  18:01:06  rogina
 * *** empty log message ***
 *
 * Revision 1.2  1995/08/10  13:38:22  finkem
 * *** empty log message ***
 *
 * Revision 1.1  1995/07/25  22:33:21  finkem
 * Initial revision
 *
 
   ======================================================================== */

char tagsRCSVersion[]= 
           "@(#)1$Id: tags.c 2390 2003-08-14 11:20:32Z fuegen $";

#include "common.h"
#include "itf.h"
#include "tags.h"


/* ------------------------------------------------------------------------
    Forward Declaration
   ------------------------------------------------------------------------ */

extern TypeInfo tagInfo;
extern TypeInfo phonesInfo;

/* ========================================================================
    Tag
   ======================================================================== */

int tagInit( Tag* tag, ClientData CD)
{
  tag->name = strdup((char*)CD);
  return TCL_OK;
}

int tagDeinit( Tag* tag)
{
  if ( tag->name) { free(tag->name); tag->name = NULL; }
  return TCL_OK;
}

static int tagPutsItf( ClientData clientData, int argc, char *argv[]) 
{
  Tag* tag = (Tag*)clientData;

  argc--;
  if (itfParseArgv(argv[0], &argc, argv+1, 0,
        NULL) != TCL_OK)  return TCL_ERROR;

  itfAppendResult("%s", tag->name);
  return TCL_OK;
}

/* ========================================================================
    Tags
   ======================================================================== */

static ClientData tagsCreateItf( ClientData clientData, 
                                 int argc, char *argv[]);
static int        tagsFreeItf(   ClientData clientData);

static Tags       tagsDefault;

/* ------------------------------------------------------------------------
    Create/Init Tags object
   ------------------------------------------------------------------------ */

int tagsInit( Tags* tags, ClientData CD)
{
  tags->name         = strdup((char*)CD);
  tags->useN         = 0;
  tags->wordBeginTag = strdup(tagsDefault.wordBeginTag);
  tags->wordEndTag   = strdup(tagsDefault.wordEndTag);
  tags->modMask      = tagsDefault.modMask;

  listInit((List*)&(tags->list), &tagInfo, sizeof(Tag),
                    tagsDefault.list.blkSize);

  tags->list.init   = (ListItemInit  *)tagInit;
  tags->list.deinit = (ListItemDeinit*)tagDeinit;

  tags->commentChar =  tagsDefault.commentChar;
  return TCL_OK;
}

Tags *tagsCreate( char* name)
{
  Tags *tags = (Tags*)malloc(sizeof(Tags));

  if (! tags || tagsInit( tags, (ClientData)name) != TCL_OK) {
     if ( tags) free( tags);
     ERROR("Failed to allocate Tags object '%s'.\n", name);
     return NULL; 
  }
  return tags;
}

static ClientData tagsCreateItf( ClientData clientData, 
                                   int argc, char *argv[]) 
{ 
  char* name = NULL;
  
  if (itfParseArgv(argv[0], &argc, argv, 0,
      "<name>",  ARGV_STRING, NULL, &name, NULL,         "name of the tags set",
		   NULL) != TCL_OK)
    return NULL;

  return (ClientData)tagsCreate(name); 
}

/* ------------------------------------------------------------------------
    Free/Deinit Tags Object
   ------------------------------------------------------------------------ */

int tagsLinkN( Tags* tags)
{
  int  useN = listLinkN((List*)&(tags->list)) - 1;
  if ( useN < tags->useN) return tags->useN;
  else                    return useN;
}

int tagsDeinit(Tags* tags)
{
  if ( tagsLinkN( tags)) {
    SERROR("Tags '%s' still in use by other objects.\n", tags->name);
    return TCL_ERROR;
  }
  if ( tags->name)         free( tags->name);
  if ( tags->wordBeginTag) free( tags->wordBeginTag);
  if ( tags->wordEndTag)   free( tags->wordEndTag);
  return listDeinit((List*)&(tags->list));
}

int tagsFree(Tags* tags)
{
  if (tagsDeinit(tags)!=TCL_OK) return TCL_ERROR;
  free(tags);
  return TCL_OK;
}

static int tagsFreeItf (ClientData clientData) 
{ return tagsFree( (Tags*)clientData); }


/* ------------------------------------------------------------------------
    tagsConfigureItf
   ------------------------------------------------------------------------ */

static int tagsConfigureItf(ClientData cd, char *var, char *val)
{
  Tags *tags = (Tags*)cd;
  if (!tags) tags = &tagsDefault;

  if (var==NULL) {
    ITFCFG(tagsConfigureItf,cd,"useN");
    ITFCFG(tagsConfigureItf,cd,"commentChar");
    ITFCFG(tagsConfigureItf,cd,"wordBeginTag");
    ITFCFG(tagsConfigureItf,cd,"wordEndTag");
    ITFCFG(tagsConfigureItf,cd,"modMask");
    return listConfigureItf((ClientData)&(tags->list), var, val);
  }
  ITFCFG_Int(    var,val,"useN",         tags->useN,         1);
  ITFCFG_Char(   var,val,"commentChar",  tags->commentChar,  0);
  ITFCFG_CharPtr(var,val,"wordBeginTag", tags->wordBeginTag, 0);
  ITFCFG_CharPtr(var,val,"wordEndTag",   tags->wordEndTag,   0);
  ITFCFG_Int(    var,val,"modMask",      tags->modMask,      0);
  return listConfigureItf((ClientData)&(tags->list), var, val);
}

/* ------------------------------------------------------------------------
    tagsAccessItf
   ------------------------------------------------------------------------ */

static ClientData tagsAccessItf (ClientData cd, char *name, TypeInfo **ti)
{ 
  Tags* tags = (Tags*)cd;
  return listAccessItf((ClientData)&(tags->list),name,ti);
}

/* ------------------------------------------------------------------------
    tagsPuts
   ------------------------------------------------------------------------ */

static int tagsPutsItf (ClientData clientData, int argc, char *argv[]) 
{
  Tags* tags = (Tags*)clientData;

  argc--;
  if (itfParseArgv(argv[0], &argc, argv+1, 0,
        NULL) != TCL_OK)  return TCL_ERROR;

  return listPutsItf((ClientData)&(tags->list), argc, argv);
}

/* ------------------------------------------------------------------------
    phonesIndex
   ------------------------------------------------------------------------ */

int tagsIndex( Tags* tags,  char* phone) 
{
  return listIndex((List*)&(tags->list), phone, 0);
}

static int tagsIndexItf( ClientData clientData, int argc, char *argv[] )
{
  Tags* tags = (Tags*)clientData;

  argc--;
  if (itfParseArgv(argv[0], &argc, argv+1, 0,
        NULL) != TCL_OK)  return TCL_ERROR;

  return listName2IndexItf((ClientData)&(tags->list), argc, argv);
}

/* ------------------------------------------------------------------------
    tagsName
   ------------------------------------------------------------------------ */

char* tagsName( Tags* tags, int i) 
{ 
  return (i>=0 && i<tags->list.itemN) ? tags->list.itemA[i].name : "(null)";
}

static int tagsNameItf( ClientData clientData, int argc, char *argv[])
{
  Tags* tags = (Tags*)clientData;

  argc--;
  if (itfParseArgv(argv[0], &argc, argv+1, 0,
        NULL) != TCL_OK)  return TCL_ERROR;

  return listIndex2NameItf((ClientData)&(tags->list), argc, argv);
}

/* ------------------------------------------------------------------------
    tagsNumber
   ------------------------------------------------------------------------ */

int tagsNumber( Tags* tags) { return tags->list.itemN; }

/* ------------------------------------------------------------------------
    tagsGetWordBeginTag
   ------------------------------------------------------------------------ */

int tagsGetWordBeginTag( Tags* tags)
{
  return tagsIndex( tags, tags->wordBeginTag);
}

/* ------------------------------------------------------------------------
    tagsWordEndTag
   ------------------------------------------------------------------------ */

int tagsGetWordEndTag( Tags* tags)
{
  return tagsIndex( tags, tags->wordEndTag);
}

/* ------------------------------------------------------------------------
    tagsAdd
   ------------------------------------------------------------------------ */

int tagsAdd( Tags* tags, int argc, char* argv[])
{
  int i;
  for ( i = 0; i < argc; i++) {
    listIndex((List*)&(tags->list), argv[i], 1);
  }
  return TCL_OK; 
}

static int tagsAddItf ( ClientData clientData, int argc, char *argv[] )
{
  Tags *tags = (Tags*)clientData;
  int     ac     =  argc-1;
  int     i;

  if (itfParseArgv(argv[0], &ac, argv+1, 0,
		  "<tag*>",ARGV_REST,NULL,&i,NULL,"list of tags",
		   NULL) != TCL_OK)
    return TCL_ERROR;

  while ( i++ < ac) {
    int     tokc;
    char  **tokv;

    if ( Tcl_SplitList (itf,argv[i],&tokc,&tokv) != TCL_OK) return TCL_ERROR;

    tagsAdd( tags, tokc, tokv);
    Tcl_Free((char*)tokv);
  }
  return TCL_OK;
}

/* ------------------------------------------------------------------------
    tagsDelete
   ------------------------------------------------------------------------ */

int tagsDelete( Tags *tags, int argc, char* argv[])
{
  int i;
  for ( i = 0; i < argc; i++) {
    if ( listDelete((List*)&(tags->list), argv[i]) != TCL_OK) {
      SERROR( "Can't remove tag '%s' from '%s'.\n", argv[i], tags->name);
      return TCL_ERROR;
    }
  }
  return TCL_OK; 
}

static int tagsDeleteItf (ClientData clientData, int argc, char *argv[])
{
  Tags *tags  = (Tags*)clientData;
  int     retCode =  TCL_OK;
  int     ac      =  argc-1;
  int     i;

  if (itfParseArgv(argv[0], &ac, argv+1, 0,
		   "<tag*>",ARGV_REST,NULL,&i,NULL,"list of tags",
		   NULL) != TCL_OK)
    return TCL_ERROR;

  while ( i++ < ac) {
    int     tokc;
    char  **tokv;

    if ( Tcl_SplitList (itf,argv[i],&tokc,&tokv) != TCL_OK) return TCL_ERROR;

    if ( tagsDelete( tags, tokc, tokv) != TCL_OK) retCode = TCL_ERROR;
    Tcl_Free((char*)tokv);
  }
  return retCode;
}

/* ------------------------------------------------------------------------
    tagsRead
   ------------------------------------------------------------------------ */

int tagsRead( Tags *tags, char *filename)
{ 
  FILE *fp;
  char  line[1000];
  int   n  = 0;
  int   rc = TCL_OK;

  if ((fp=fileOpen(filename,"r"))==NULL) {
    ERROR( "Can't open tags file '%s' for reading.\n",filename);
    return -1;
  }

  while (1) {
    char *p = line;
    char**argv;
    int   argc;

    if ( fscanf(fp,"%[^\n]",line)!=1) break; 
    else fscanf(fp,"%*c");

    if ( line[0] == tags->commentChar) continue; 

    for (p=line; *p!='\0'; p++) 
         if (*p>' ') break; if (*p=='\0') continue; 

    Tcl_SplitList( itf, line, &argc, &argv);
    if ( tagsAdd(tags, argc, argv) != TCL_OK) rc = TCL_ERROR;
    else n += argc;
    Tcl_Free((char*) argv);
  }
  fileClose(filename, fp);
  return (rc != TCL_OK) ? -1 : n;
}

static int tagsReadItf ( ClientData clientData, int argc, char *argv[] )
{
  Tags*  tags    = (Tags*)clientData;
  char*  fname   =  NULL;
  int    ac      =  argc-1;
  int    n;

  if (itfParseArgv(argv[0], &ac, argv+1, 0,
		  "<filename>",ARGV_STRING,NULL,&fname,NULL,
                  "name of tags file", NULL) != TCL_OK)
    return TCL_ERROR;

  itfAppendResult( "%d", n = tagsRead( tags, fname));
  return (n >= 0) ? TCL_OK : TCL_ERROR;
}

/* ------------------------------------------------------------------------
    tagsWrite
   ------------------------------------------------------------------------ */

static void tagsWriteFile(Tags *tags, FILE *fp)
{
  Tag* tag  = tags->list.itemA;
  int  tagN = tags->list.itemN;

  for ( ; tagN > 0; tagN--, tag++) fprintf(fp,"%s ",tag->name);
  fprintf(fp,"\n");
}

int tagsWrite (Tags *tags, char *filename)
{
  FILE *fp;

  if ((fp=fileOpen(filename,"w"))==NULL) {
    ERROR( "Can't open tags file '%s' for writing.\n",filename);
    return TCL_ERROR;
  }

  fprintf( fp, "%c -------------------------------------------------------\n",
                tags->commentChar);
  fprintf( fp, "%c  Name            : %s\n",  tags->commentChar, 
                tags->name);
  fprintf( fp, "%c  Type            : Tags\n", tags->commentChar);
  fprintf( fp, "%c  Number of Items : %d\n",  tags->commentChar, 
                tags->list.itemN);
  fprintf( fp, "%c  Date            : %s", tags->commentChar,
                dateString());
  fprintf( fp, "%c -------------------------------------------------------\n",
                tags->commentChar);

  tagsWriteFile (tags,fp);
  fileClose(filename, fp);
  return TCL_OK;
}

static int tagsWriteItf (ClientData clientData, int argc, char *argv[] )
{
  char   *fname   =  NULL;
  int     ac      =  argc-1;

  if (itfParseArgv(argv[0], &ac, argv+1, 0,
		  "<filename>",ARGV_STRING,NULL,&fname,NULL,
                  "name of tags file", NULL) != TCL_OK)
    return TCL_ERROR;

  return tagsWrite( (Tags*)clientData, fname);
}

/* ========================================================================
    Type Declaration
   ======================================================================== */

static Method tagMethod[] = 
{ 
  { "puts", tagPutsItf,  "print information about tag" },
  {  NULL,  NULL, NULL } 
};

TypeInfo tagInfo = { "Tag", 0, 0, tagMethod, 
                      NULL, NULL, NULL, NULL, itfTypeCntlDefaultNoLink,
                     "Tag\n" };

Method tagsMethod[] = 
{
  { "puts",   tagsPutsItf,   "displays the contents of a tags-set" },
  { "add",    tagsAddItf,    "add new tag(s) to a tags-set"        },
  { "delete", tagsDeleteItf, "delete tag(s) from a tags-set"       },
  { "read",   tagsReadItf,   "read a tag-set from a file"          },
  { "write",  tagsWriteItf,  "write a tag-set into a file"         },
  { "index",  tagsIndexItf,  "return index of named tag(s)"        },
  { "name",   tagsNameItf,   "return the name of indexed tag(s)"   },
  { NULL, NULL, NULL }
} ;

TypeInfo tagsInfo = { "Tags", 0, 0, tagsMethod,
                       tagsCreateItf, tagsFreeItf, 
                       tagsConfigureItf, tagsAccessItf, NULL,
                      "A 'Tags' object is an array of strings." };

static int tagsInitialized = 0;

int Tags_Init (void)
{
  if (! tagsInitialized) {
    tagsDefault.name            = NULL;
    tagsDefault.useN            = 0;
    tagsDefault.list.itemN      = 0;
    tagsDefault.list.blkSize    = 10;
    tagsDefault.wordBeginTag    = strdup("WB");
    tagsDefault.wordEndTag      = strdup("WE");
    tagsDefault.commentChar     = ';';
    tagsDefault.modMask         = 0 ^ -1;

    itfNewType(&tagInfo);
    itfNewType(&tagsInfo);
    tagsInitialized = 1;
  }
  return TCL_OK;
}
