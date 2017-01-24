/* ========================================================================
    JANUS      Speech- to Speech Translation
               ------------------------------------------------------------
               Object: Rewrite Rules
               ------------------------------------------------------------

    Author  :  Michael Finke
    Module  :  rewrite.c
    Date    :  $Id: rewrite.c 2390 2003-08-14 11:20:32Z fuegen $
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
    Revision 3.5  2003/08/14 11:20:17  fuegen
    Merged changes on branch jtk-01-01-15-fms (jaguar -> ibis-013)

    Revision 3.4.20.4  2003/07/02 17:45:19  fuegen
    fixes for file handling under Windows
    using general functions for file open/close

    Revision 3.4.20.3  2003/04/01 08:52:12  metze
    Bugfix for sscanf

    Revision 3.4.20.2  2002/08/27 08:42:08  metze
    itfParseArgv now uses <name> and NULL initialization for char*

    Revision 3.4.20.1  2002/06/26 11:57:56  fuegen
    changes for new directory structure support and code cleaning

    Revision 3.4  2000/11/14 12:35:25  janus
    Merging branch jtk-00-10-27-jmcd.

    Revision 3.3.34.2  2000/11/08 17:29:37  janus
    Finished making changes required for compilation under 'gcc -Wall'.

    Revision 3.3.34.1  2000/11/06 10:50:39  janus
    Made changes to enable compilation under -Wall.

    Revision 3.3  1997/07/23 12:41:33  tschaaf
    dcc / DFKI - clean

    Revision 3.2  1997/06/17 17:41:43  westphal
    *** empty log message ***

    Revision 1.10  96/02/02  10:39:16  rogina
    make writing files work
    
    Revision 1.9  1996/01/05  17:02:40  finkem
    removed ref to old ERR function

    Revision 1.8  1995/10/24  17:25:16  torsten
    fixed fix from 1.7

    Revision 1.7  1995/10/23  22:40:22  torsten
    got rid of annoying compile warning in rwsRead

    Revision 1.6  1995/10/04  23:40:28  torsten
    *** empty log message ***

    Revision 1.5  1995/09/24  01:53:15  finkem
    dito.

    Revision 1.4  1995/09/23  19:10:11  finkem
    changed static to extern

    Revision 1.3  1995/09/06  07:37:49  kemp
    *** empty log message ***

    Revision 1.2  1995/08/17  17:08:15  rogina
    *** empty log message ***

    Revision 1.1  1995/07/25  13:49:57  finkem
    Initial revision

 
   ======================================================================== */

#include "common.h"
#include "itf.h"
#include "rewrite.h"
#include "array.h"



char rewriteRCSVersion[]= "@(#)1$Id: rewrite.c 2390 2003-08-14 11:20:32Z fuegen $";


/* ------------------------------------------------------------------------
    Forward Declarations
   ------------------------------------------------------------------------ */

extern TypeInfo rewriteInfo;
extern TypeInfo rewriteSetInfo;

/* ========================================================================
    Rewrite
   ======================================================================== */

static int        rwInit         (Rewrite* RW, ClientData CD);
static int        rwDeinit       (Rewrite* RW);
static int        rwPutsItf      (ClientData cd, int argc, char *argv[]);
static int        rwConfigureItf (ClientData cd, char *var, char *val);
static ClientData rwAccessItf    (ClientData cd, char *name, TypeInfo **ti);

/* ------------------------------------------------------------------------
    rwInit
   ------------------------------------------------------------------------ */

static int rwInit (Rewrite* RW, ClientData CD)
{
  assert( RW);

  RW->from =  strdup((char*)CD);
  RW->to   =  NULL;

  return TCL_OK;
}

/* ------------------------------------------------------------------------
    rwDeinit  deinitialize rewrite structure
   ------------------------------------------------------------------------ */

static int rwDeinit (Rewrite* RW)
{
  assert( RW);

  if ( RW->from) free((char*)RW->from);
  if ( RW->to)   free((char*)RW->to);

  return TCL_OK;
}

/* ------------------------------------------------------------------------
    rwPutsItf
   ------------------------------------------------------------------------ */

static int rwPutsItf (ClientData cd, int argc, char *argv[])
{
  Rewrite* RW = (Rewrite*)cd;

  argc--;
  if (itfParseArgv(argv[0], &argc, argv+1, 0, 
      NULL) != TCL_OK)  return TCL_ERROR;

  if ( RW->to) itfAppendResult( "%s", RW->to);
  return TCL_OK;
}

/* ------------------------------------------------------------------------
    rwConfigureItf
   ------------------------------------------------------------------------ */

static int rwConfigureItf (ClientData cd, char *var, char *val)
{
  /* Rewrite* RW = (Rewrite*)cd; */

  if (! var) {
    return TCL_OK;
  }
  ERROR("Unknown option '-%s %s'.\n", var, val ? val : "?");
  return TCL_ERROR;
}

/* ------------------------------------------------------------------------
    rwAccessItf
   ------------------------------------------------------------------------ */

static ClientData rwAccessItf (ClientData cd, char *name, TypeInfo **ti)
{
  /* Rewrite* RW = (Rewrite*)cd; */
  return NULL;
}

/* ========================================================================
    RewriteSet
   ======================================================================== */

static ClientData rwsCreateItf    (ClientData cd, int argc, char *argv[]);
static int        rwsConfigureItf (ClientData cd, char *var, char *val);
static ClientData rwsAccessItf    (ClientData cd, char *name, TypeInfo **ti);
static int        rwsPutsItf      (ClientData cd, int argc, char *argv[]);
static int        rwsDeleteItf    (ClientData cd, int argc, char *argv[]);
static int        rwsAddItf       (ClientData cd, int argc, char *argv[]);
static int        rwsReadItf      (ClientData cd, int argc, char *argv[]);
static int        rwsWriteItf     (ClientData cd, int argc, char *argv[]);

/* ------------------------------------------------------------------------
    Create Rewrite Set Structure
   ------------------------------------------------------------------------ */

char *rwsRewrite(RewriteSet *rwsP, char *string)
{
  int idx = rwsP ? listIndex((List*)&(rwsP->list),(ClientData)string,0) : -1;
  return idx>=0 ? rwsP->list.itemA[idx].to : string;
}

RewriteSet* rwsCreate (char* name)
{
  RewriteSet* rwsP = (RewriteSet*)malloc( sizeof(RewriteSet));

  if (! rwsP) { ERROR("Failed to allocate rewrite set '%s'\n.", name); return NULL; }

  rwsP->name         = strdup( name);
  rwsP->useN         = 0;

  listInit( (List*)&(rwsP->list), &rewriteInfo, sizeof(Rewrite), 100);

  rwsP->list.init    = (ListItemInit  *)rwInit;
  rwsP->list.deinit  = (ListItemDeinit*)rwDeinit;

  return rwsP;
}

static ClientData rwsCreateItf( ClientData cd, int argc, char *argv[])
{
  char       *name = NULL; 

  if (itfParseArgv (argv[0], &argc, argv, 0,
      "<name>",  ARGV_STRING, NULL, &name, NULL,         "name of the rewrite set",
       NULL) != TCL_OK) return NULL;

  return (ClientData)rwsCreate(name);
}

/* ------------------------------------------------------------------------
    Free RewriteSet Structure
   ------------------------------------------------------------------------ */

static int rwsDeinit (RewriteSet* rwsP)
{
  if (rwsP==NULL) { ERROR("null argument"); return TCL_ERROR; }

  if (rwsP->useN) 
  { SERROR("RewriteSet object still in use by %d other object(s).\n", rwsP->useN);
    return TCL_ERROR;
  }
  
  listDeinit((List*)&(rwsP->list));
  if (rwsP->name) free(rwsP->name);

  return TCL_OK;
}

int rwsFree (RewriteSet* rwsP)
{
  if (rwsDeinit(rwsP)!=TCL_OK) return TCL_ERROR;
  if (rwsP) free(rwsP);
  return TCL_OK;
}

static int rwsFreeItf(ClientData cd)
{
  rwsFree((RewriteSet*)cd);
  return TCL_OK;
}

/* ------------------------------------------------------------------------
    rwsConfigureItf
   ------------------------------------------------------------------------ */

static int rwsConfigureItf (ClientData cd, char *var, char *val)
{
  RewriteSet* rwsP = (RewriteSet*)cd;
  if (! rwsP) return TCL_ERROR;

  if (! var) 
    {
      ITFCFG(rwsConfigureItf,cd,"useN");
      return listConfigureItf( (ClientData)&(rwsP->list), var, val);
    }
  ITFCFG_Int(var,val,"useN",rwsP->useN,1);
  return listConfigureItf((ClientData)&(rwsP->list), var, val);
}

/* ------------------------------------------------------------------------
    rwsAccessItf
   ------------------------------------------------------------------------ */

static ClientData rwsAccessItf (ClientData cd, char *name, TypeInfo **ti)
{ 
  RewriteSet* rwsP  = (RewriteSet*)cd;
  return listAccessItf((ClientData)&(rwsP->list), name, ti);
}

/* ------------------------------------------------------------------------
    rwsPutsItf
   ------------------------------------------------------------------------ */

static int rwsPutsItf(ClientData cd, int argc, char *argv[])
{
  RewriteSet* rwsP  = (RewriteSet*)cd;
  return listPutsItf( (ClientData)&(rwsP->list), argc, argv);
}

/* ------------------------------------------------------------------------
    rwsDeleteItf
   ------------------------------------------------------------------------ */

static int rwsDeleteItf(ClientData cd, int argc, char *argv[])
{
  RewriteSet* rwsP  = (RewriteSet*)cd;
  return listDeleteItf( (ClientData)&(rwsP->list), argc, argv);
}

/* ------------------------------------------------------------------------
    rwsAdd
   ------------------------------------------------------------------------ */

int rwsAdd (RewriteSet* rwsP, char* from, char* to)
{
  int idx  = listIndex((List*)&(rwsP->list), (ClientData)from, 0);
  if (idx >= 0) {
    SERROR("Rewrite rule for '%s' already exists in '%s'.\n", 
            from, rwsP->name);
    return idx;
  }
  MSGCLEAR(NULL);

  idx = listNewItem( (List*)&(rwsP->list), (ClientData)from);

  rwsP->list.itemA[idx].to = strdup(to);
  return idx;
}

static int rwsAddItf (ClientData cd, int argc, char *argv[])
{
  RewriteSet* rwsP   = (RewriteSet*)cd;
  int         ac     =  argc - 1;
  char*       from   =  NULL;
  char*       to     =  NULL;
  int         idx;

  if ( itfParseArgv( argv[0], &ac, argv+1, 0,
      "<from>", ARGV_STRING, NULL, &from, NULL, "left side of the rewrite rule",
      "<to>",   ARGV_STRING, NULL, &to,   NULL, "right side of the rewrite rule",
       NULL) != TCL_OK) return TCL_ERROR;

  if ((idx = rwsAdd( rwsP, from, to)) < 0) return TCL_ERROR; else return TCL_OK;
}

/* ------------------------------------------------------------------------
    rwsRead
   ------------------------------------------------------------------------ */

#define MAXLINE 1000

int rwsRead (RewriteSet* rwsP, char* fileName)
{
  FILE*    fp;
  char   from[MAXLINE];
  char     to[MAXLINE];
  char   line[MAXLINE];
  int     idx;

  if ((fp = fileOpen(fileName,"r")) == NULL) { 
    ERROR ("Can't read rewrite set file \"%s\".\n", fileName);
    return TCL_ERROR;
  }

  while (!feof(fp)) {
    if (fscanf (fp, "%[^\n]", line)     != 1) { 
      fscanf (fp, "%*c");
      continue;
    }

    if (line[0] == ';')                                  continue;
    if (sscanf (line, "%s%s", from, to) !=2)             continue;
    
    idx = listNewItem ((List*)&(rwsP->list), (ClientData) from);
    rwsP->list.itemA[idx].from = strdup (from);
    rwsP->list.itemA[idx].to   = strdup (to);
  }
  fileClose (fileName, fp);

  listSort((List *) &(rwsP->list));

  return 0;
}

static int rwsReadItf (ClientData cd, int argc, char *argv[])
{
  RewriteSet* rwsP  = (RewriteSet*)cd;
  int         ac    =  argc - 1;
  char*       fname =  NULL;
  int         idx;

  if ( itfParseArgv( argv[0], &ac, argv+1, 0,
      "<filename>", ARGV_STRING, NULL, &fname, cd, "file to read from",
       NULL) != TCL_OK) return TCL_ERROR;

  if ((idx = rwsRead(rwsP, fname)) < 0) return TCL_ERROR; else return TCL_OK;
}

/* ------------------------------------------------------------------------
    rwsWrite
   ------------------------------------------------------------------------ */

int rwsWrite (RewriteSet* rwsP, char* fileName)
{
  FILE*      fp;
  int        rwX;

  assert( rwsP && fileName);

  if ((fp=fileOpen( fileName,"w"))==NULL) { 
    ERROR( "Can't write rewrite set file \"%s\".\n", fileName); 
    return TCL_ERROR;
  }

  fprintf( fp, "; -------------------------------------------------------\n");
  fprintf( fp, ";  Name            : %s\n", rwsP->name);
  fprintf( fp, ";  Type            : RewriteSet\n");
  fprintf( fp, ";  Number of Items : %d\n", rwsP->list.itemN);
  fprintf( fp, ";  Date            : %s\n", dateString());
  fprintf( fp, "; -------------------------------------------------------\n");

  for ( rwX = 0; rwX < rwsP->list.itemN; rwX++)
    fprintf( fp, "%-25s %-25s\n", rwsP->list.itemA[rwX].from, rwsP->list.itemA[rwX].to);

  fileClose(fileName, fp);
  return 0;
}

static int rwsWriteItf (ClientData cd, int argc, char *argv[])
{
  RewriteSet* rwsP  = (RewriteSet*)cd;
  int         ac    =  argc - 1;
  char*       fname =  NULL;
  int         idx;

  if ( itfParseArgv( argv[0], &ac, argv+1, 0,
      "<filename>", ARGV_STRING, NULL, &fname, cd, "file to write to",
       NULL) != TCL_OK) return TCL_ERROR;

  if ((idx = rwsWrite(rwsP,fname))<0) return TCL_ERROR; else return TCL_OK;
}

/* ========================================================================
    Type Declaration
   ======================================================================== */

static Method rewriteMethod[] = 
{ 
  { "puts", rwPutsItf, NULL },
  {  NULL,  NULL,      NULL } 
};

TypeInfo rewriteInfo = { "Rewrite", 0, 0, rewriteMethod, 
                          NULL,           NULL,
                          rwConfigureItf, rwAccessItf, itfTypeCntlDefaultNoLink,
	                 "Rewrite Rule\n" } ;

static Method rewriteSetMethod[] = 
{ 
  { "puts",   rwsPutsItf,    NULL },
  { "add",    rwsAddItf,    "add a new rewrite rule to the set" },
  { "delete", rwsDeleteItf, "remove rewrite rule from the set"  },
  { "read",   rwsReadItf,   "reads a rewrite rules file" },
  { "write",  rwsWriteItf,  "writes a rewrite rules file" },
  {  NULL,    NULL,          NULL } 
};

TypeInfo rewriteSetInfo = { "RewriteSet", 0, 0, rewriteSetMethod, 
                             rwsCreateItf,     rwsFreeItf,
                             rwsConfigureItf,  rwsAccessItf, NULL,
	                    "Set of rewrite rules\n" } ;

static int rewriteInitialized = 0;

int Rewrite_Init(void)
{
  if (!rewriteInitialized) 
    {
      rewriteInitialized = 1;
      itfNewType (&rewriteInfo);
      itfNewType (&rewriteSetInfo);
    }
  return TCL_OK;
}

