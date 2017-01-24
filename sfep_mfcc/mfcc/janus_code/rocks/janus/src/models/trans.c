/* ========================================================================
    JANUS      Speech- to Speech Translation
               ------------------------------------------------------------
               Object: Transition Models
               ------------------------------------------------------------
               
    Author: Monika Woszczyna
    Date:   $Id: trans.c 2390 2003-08-14 11:20:32Z fuegen $
    Remark: further documentation needs to be added

 ==========================================================================
               
   This software was developed by
   The Interactive Systems Laboratories at

   the  University of Karlsruhe       and  Carnegie Mellon University
        Dept. of Informatics               Dept. of Computer Science
        ILKD, Lehrstuhl Prof.Waibel        Alex Waibel's NN & Speech Group
        Am Fasanengarten 5                 5000 Forbes Ave
        D-76131 Karlsruhe                  Pittsburgh, PA 15213
        - West Germany -                   - USA -

   Copyright (C) 1990-1995.   All rights reserved.

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
 ==========================================================================

   $Log$
   Revision 3.5  2003/08/14 11:20:19  fuegen
   Merged changes on branch jtk-01-01-15-fms (jaguar -> ibis-013)

   Revision 3.4.20.2  2002/08/27 08:40:31  metze
   itfParseArgv now uses <name> and NULL initialization for char*

   Revision 3.4.20.1  2002/06/26 11:57:57  fuegen
   changes for new directory structure support and code cleaning

   Revision 3.4  2000/11/14 12:35:26  janus
   Merging branch jtk-00-10-27-jmcd.

   Revision 3.3.34.2  2000/11/08 17:32:59  janus
   Finished making changes required for compilation under 'gcc -Wall'.

   Revision 3.3.34.1  2000/11/06 10:50:41  janus
   Made changes to enable compilation under -Wall.

   Revision 3.3  2000/08/16 11:45:41  soltau
   free -> TclFree

   Revision 3.2  1997/06/17 17:41:43  westphal
   *** empty log message ***

    Revision 1.9  96/01/31  04:23:48  finkem
    changed read function to freadAdd function
    
    Revision 1.8  1996/01/01  16:28:20  finkem
    handling of parsing errors in the Read function
    remove INFO output when reading

    Revision 1.7  1995/10/29  23:54:36  finkem
    changed file format to janus3 style

    Revision 1.6  1995/09/06  07:42:32  kemp
    *** empty log message ***

    Revision 1.5  1995/08/27  22:35:00  finkem
    *** empty log message ***

    Revision 1.4  1995/08/17  18:00:50  rogina
    *** empty log message ***

    Revision 1.3  1995/08/14  13:19:02  finkem
    *** empty log message ***

    Revision 1.2  1995/08/01  13:27:29  finkem
    *** empty log message ***

    Revision 1.1  1995/07/25  13:49:57  finkem
    Initial revision


 =========================================================================== */

char transRCSVersion[] = "$Id: trans.c 2390 2003-08-14 11:20:32Z fuegen $";


#include "common.h"
#include "itf.h"
#include "trans.h"


/* ------------------------------------------------------------------------
    Forward Declaration
   ------------------------------------------------------------------------ */

extern TypeInfo tmInfo;

/* ========================================================================
    Transition Array
   ======================================================================== */

int tmGetItf( char* value, int** pidx, float** ppen)
{
  Tm* tm;

  if ( pidx) *pidx = NULL;
  if ( ppen) *ppen = NULL;

  if ((tm = (Tm*)itfGetObject( value, "Tm")) == NULL) {
    int     argc;
    char**  argv;
    int     n = 0;

    MSGCLEAR(NULL);

    if ( Tcl_SplitList( itf, value, &argc, &argv) == TCL_OK) {
      int    i, j;
      int*   idx = (argc) ? (int  *)malloc( argc * sizeof(int))   : NULL;
      float* pen = (argc) ? (float*)malloc( argc * sizeof(float)) : NULL;

      n = argc;

      for ( i = 0; i < argc; i++) {
        int    tokc;
        char** tokv;

        if ( Tcl_SplitList( itf, argv[i], &tokc, &tokv) == TCL_OK) {
          idx[i]  = 0;
          pen[i]  = 0.0;

          for ( j = 0; j < tokc; j++) {
            if ( j == 0) {
              int d;
              if ( sscanf( tokv[j], "%d", &d) != 1) {
	        ERROR("expected 'int' type transition offset.\n");
	        Tcl_Free((char*)tokv); Tcl_Free((char*)argv);
	        free( idx);        free( pen);
	        return -1;
              }
              idx[i] = d;
	    }
            else if ( j == 1) {
              float d;
              if ( sscanf( tokv[j], "%f", &d) != 1) {
	        ERROR("expected 'float' type transition penalty.\n");
	        Tcl_Free((char*)tokv); Tcl_Free((char*)argv);
	        free( idx);        free( pen);
	        return -1;
              }
              pen[i] = d;
	    }
          }
          Tcl_Free((char*)tokv);
        }
        if ( pidx) *pidx = idx;
        if ( ppen) *ppen = pen; 
      }
      Tcl_Free((char*)argv);
    }
    return n;
  }
  else {
    int    n   =  tm->trN;
    int*   idx = (int  *)malloc( n * sizeof(int));
    float* pen = (float*)malloc( n * sizeof(float));
    int    i;

    for ( i = 0; i < n; i++) {
      idx[i] = tm->offA[i];
      pen[i] = tm->penA[i];
    }
    if ( pidx) *pidx = idx;
    if ( ppen) *ppen = pen; 
    return n;
  }
}

/* ------------------------------------------------------------------------
    tmArrayGetItf
   ------------------------------------------------------------------------ */

int tmArrayGetItf( char* value, int** pn, int*** pidx, float*** ppen)
{
  int     argc;
  char**  argv;

  if ( pidx) *pidx = NULL;
  if ( ppen) *ppen = NULL;
  if ( pn)   *pn   = NULL;

  if ( Tcl_SplitList( itf, value, &argc, &argv) == TCL_OK) {
     int     i;
     int*    n   = (int   *)malloc( argc * sizeof(int));
     int**   idx = (int  **)malloc( argc * sizeof(int*));
     float** pen = (float**)malloc( argc * sizeof(float*));

     for ( i = 0; i < argc; i++) {
       if ((n[i] = tmGetItf( argv[i], &(idx[i]), &(pen[i]))) < 0) {
         int k;
         for ( k = 0; k < i; k++) {
           if ( idx[k]) free( idx[k]);
           if ( pen[k]) free( pen[k]);
         }
         free( n); free( idx); free( pen); Tcl_Free((char*)argv);
         return -1;
       }
     }
     if ( pidx) *pidx = idx;
     if ( ppen) *ppen = pen;
     if ( pn)   *pn   = n;
     Tcl_Free((char*)argv);
     return argc;
  }
  return -1;
}

/* ========================================================================
    Tm
   ======================================================================== */

int tmInit( Tm* tm, ClientData CD)
{
  tm->name = strdup((char*)CD);
  tm->trN  = 0;
  tm->offA = NULL;
  tm->penA = NULL; 
  return TCL_OK;
}

int tmDeinit( Tm* tm)
{
  if ( tm->name) { free(tm->name); tm->name = NULL; }
  if ( tm->offA) { free(tm->offA); tm->offA = NULL; }
  if ( tm->penA) { free(tm->penA); tm->penA = NULL; }
  return TCL_OK;
}

static int tmPutsItf( ClientData clientData, int argc, char *argv[]) 
{
  Tm* tmP = (Tm*)clientData;
  int trX;
  int trN = tmP->trN;

  itfAppendResult( "%s {", tmP->name, tmP->trN);
  for (trX = 0; trX < trN; trX ++)
    itfAppendResult( " { %d %f}",tmP->offA[trX],tmP->penA[trX]);
  itfAppendResult( "}");
  return TCL_OK;
}

/* ========================================================================
    TmSet
   ======================================================================== */

static TmSet tmSetDefault;

/* ------------------------------------------------------------------------
    Create/Init TmSet object
   ------------------------------------------------------------------------ */

int tmSetInit( TmSet* tmSetP, ClientData CD)
{
 tmSetP->name   = strdup((char*)CD);
 tmSetP->useN   = 0;

 listInit( (List*)&(tmSetP->list), &tmInfo, sizeof(Tm),
                    tmSetDefault.list.blkSize);

 tmSetP->list.init   = (ListItemInit   *)tmInit;
 tmSetP->list.deinit = (ListItemDeinit *)tmDeinit;
 
 tmSetP->commentChar = tmSetDefault.commentChar;
 
 return TCL_OK;
}

TmSet* tmSetCreate( char * name)
{
  TmSet* tmSetP = (TmSet*)malloc(sizeof(TmSet));

  if (! tmSetP || tmSetInit( tmSetP, (ClientData)name) != TCL_OK) {
     if ( tmSetP) free( tmSetP);
     SERROR("Failed to allocate TmSet object '%s'.\n", name);
     return NULL; 
  }
  return tmSetP;
}

static ClientData tmSetCreateItf( ClientData clientData, 
                                  int argc, char *argv[]) 
{ 
  char* name = NULL;

  if (itfParseArgv (argv[0], &argc, argv, 0,
      "<name>",  ARGV_STRING, NULL, &name, NULL,         "name of the tmset",
       NULL) != TCL_OK) return NULL;

  return (ClientData)tmSetCreate(name);
}

/* ------------------------------------------------------------------------
    Free/Deinit TmSet Object
   ------------------------------------------------------------------------ */

int tmSetLinkN( TmSet* tmSetP)
{
  int  useN = listLinkN((List*)&(tmSetP->list)) - 1;
  if ( useN < tmSetP->useN) return tmSetP->useN;
  else                      return useN;
}

int tmSetDeinit( TmSet* tmSetP)
{
  if ( tmSetLinkN( tmSetP)) {
    SERROR("TmSet '%s' still in use by other objects.\n", tmSetP->name);
    return TCL_ERROR;
  }
  if ( tmSetP->name) { free(tmSetP->name); tmSetP->name = NULL; }
  return listDeinit((List*)&(tmSetP->list));
}

int tmSetFree( TmSet* tmSetP)
{
  if (tmSetDeinit( tmSetP) != TCL_OK) return TCL_ERROR;
  free(tmSetP);
  return TCL_OK;
}

static int tmSetFreeItf (ClientData clientData) 
{ return tmSetFree( (TmSet*)clientData); }

/* ------------------------------------------------------------------------
    tmSetConfigureItf
   ------------------------------------------------------------------------ */

static int tmSetConfigureItf( ClientData cd, char *var, char *val)
{
  TmSet* tmSetP = (TmSet*)cd;
  if (!  tmSetP) tmSetP = &tmSetDefault;

  if (! var) {
    ITFCFG(tmSetConfigureItf,cd,"useN");
    ITFCFG(tmSetConfigureItf,cd,"commentChar");
    return listConfigureItf((ClientData)&(tmSetP->list), var, val);
  }
  ITFCFG_Int    ( var,val,"useN",tmSetP->useN,          1);
  ITFCFG_Char(    var,val,"commentChar", tmSetP->commentChar,0);
  return listConfigureItf((ClientData)&(tmSetP->list), var, val);
}

/* ------------------------------------------------------------------------
    tmSetAccessItf
   ------------------------------------------------------------------------ */

static ClientData tmSetAccessItf( ClientData cd, char *name, TypeInfo **ti)
{ 
  TmSet* tmSetP  = (TmSet*)cd;
  return listAccessItf( (ClientData)&(tmSetP->list), name, ti);
}

/* ------------------------------------------------------------------------
    tmSetPuts
   ------------------------------------------------------------------------ */

static int tmSetPutsItf (ClientData cd, int argc, char* argv[])
{ 
  TmSet* tmSetP = (TmSet*) cd;
  return listPutsItf( (ClientData)&(tmSetP->list), argc, argv);
}

/* ------------------------------------------------------------------------
    tmSetIndex
   ------------------------------------------------------------------------ */

int tmSetIndex( TmSet* tmSet, char* tm) 
{
  return listIndex((List*)&(tmSet->list), tm, 0);
}

static int tmSetIndexItf( ClientData clientData, int argc, char *argv[] )
{
  TmSet* tmSet = (TmSet*)clientData;
  return listName2IndexItf((ClientData)&(tmSet->list), argc, argv);
}

/* ------------------------------------------------------------------------
    tmSetName
   ------------------------------------------------------------------------ */

char* tmSetName( TmSet* tmSet, int i) 
{ 
  return (i < 0) ? "(null)" : tmSet->list.itemA[i].name;
}

int tmSetNameItf( ClientData clientData, int argc, char *argv[])
{
  TmSet* tmSet = (TmSet*)clientData;
  return listIndex2NameItf((ClientData)&(tmSet->list), argc, argv);
}

/* ------------------------------------------------------------------------
    tmSetNumber
   ------------------------------------------------------------------------ */

int tmSetNumber( TmSet* tmSet) { return tmSet->list.itemN; }

/* ------------------------------------------------------------------------
    tmSetAdd
   ------------------------------------------------------------------------ */

int tmSetAdd( TmSet* tmSetP, char* name, char* desc)
{
  int  idx  = listIndex((List*)&(tmSetP->list), (ClientData)name, 0);
  if ( idx >= 0) { 
    ERROR("Tm '%s' already exists in '%s'.\n", name, tmSetP->name); 
    return idx;
  }
  MSGCLEAR(NULL);

  idx = listNewItem((List*)&(tmSetP->list), (ClientData)name);

  tmSetP->list.itemA[idx].trN = tmGetItf( desc, 
                              &(tmSetP->list.itemA[idx].offA),
                              &(tmSetP->list.itemA[idx].penA));
  return idx;
}

static int tmSetAddItf( ClientData cd, int argc, char* argv[])
{ 
  TmSet* tmSetP  = (TmSet*) cd;
  int    ac      = argc-1;
  char*  name    = NULL;
  char*  tmDescS = NULL;
  int    n;
  
  if (itfParseArgv( argv[0], &ac,  argv+1,  2,
     "<name>",ARGV_STRING, NULL, &name, NULL, "name of the transition model",
     "<tm>",  ARGV_STRING, NULL, &tmDescS, NULL, 
     "transition model description", NULL)
    != TCL_OK) return TCL_ERROR;

  itfAppendResult( "%d", n = tmSetAdd( tmSetP, name, tmDescS));
  return (n >= 0) ? TCL_OK : TCL_ERROR;
}

/* ------------------------------------------------------------------------
    tmSetRead
   ------------------------------------------------------------------------ */

static int tmSetReadItf (ClientData cd, int argc, char* argv[])
{ 
  TmSet* tmSetP    = (TmSet*) cd;
  int    ac        =  argc-1;
  char*  filenameS =  NULL;
  int    n;
  
  if (itfParseArgv( "filename", &ac,  argv+1,  2,
     "<filename>" , ARGV_STRING, NULL, &filenameS, NULL, 
     "name of transition model description file", NULL) != TCL_OK) return TCL_ERROR;

  itfAppendResult( "%d", n = freadAdd((ClientData)tmSetP, filenameS,
                                       tmSetP->commentChar, tmSetAddItf));
  return (n >= 0) ? TCL_OK : TCL_ERROR;
}

/* ------------------------------------------------------------------------
    tmSetWrite
   ------------------------------------------------------------------------ */

int tmSetWrite( TmSet* tmsP, char* fileName)
{
  FILE*      fp;
  int        tmX;

  assert( tmsP && fileName);

  if (! ( fp = fileOpen( fileName,"w"))) return TCL_ERROR;

  fprintf( fp, "%c -------------------------------------------------------\n", tmsP->commentChar);
  fprintf( fp, "%c  Name            : %s\n",                                   tmsP->commentChar, tmsP->name);
  fprintf( fp, "%c  Type            : TmSet\n" ,                               tmsP->commentChar);
  fprintf( fp, "%c  Number of Items : %d\n",                                   tmsP->commentChar, tmsP->list.itemN);
  fprintf( fp, "%c  Date            : %s",                                     tmsP->commentChar, dateString());
  fprintf( fp, "%c -------------------------------------------------------\n", tmsP->commentChar);

  for ( tmX = 0; tmX < tmsP->list.itemN; tmX++) {
    int trX;
    fprintf( fp, "%-20s", tmsP->list.itemA[tmX].name);
    fprintf( fp, " {");
    for ( trX = 0; trX < tmsP->list.itemA[tmX].trN; trX++) {
      fprintf( fp, " {%d %f}", tmsP->list.itemA[tmX].offA[trX],
                               tmsP->list.itemA[tmX].penA[trX]);
    }
    fprintf( fp, "}\n");
  }
  fileClose( fileName, fp);
  return TCL_OK;
}

static int tmSetWriteItf (ClientData cd, int argc, char *argv[])
{
  TmSet* tmsP  = (TmSet*)cd;
  int    ac    =  argc - 1;
  char*  fname =  NULL;

  if ( itfParseArgv( argv[0], &ac, argv+1, 0,
      "<filename>", ARGV_STRING, NULL, &fname, cd, "file to read from",
       NULL) != TCL_OK) return TCL_ERROR;

  return tmSetWrite( tmsP, fname);
}

/* ========================================================================
    Type Declaration
   ======================================================================== */

static Method tmMethod[] = 
{ 
  { "puts",   tmPutsItf,   "displays the contents of a Tm entry" },
  {  NULL,    NULL,         NULL                                 } 
};

static Method tmSetMethod[] = 
{ 
  { "puts", tmSetPutsItf, "displays the contents of a transition model" },
  { "add",    tmSetAddItf,    "add a Tm to the list"              },
  { "read",   tmSetReadItf,   "reads a TmSet from a file"         },
  { "write",  tmSetWriteItf,  "writes a TmSet to a file"          },
  { "index",  tmSetIndexItf,  "return index of named Tm(s)"       },
  { "name",   tmSetNameItf,   "return the name of indexed Tm(s)"  },
  { NULL,           NULL,           NULL                          } 
};

TypeInfo tmInfo = {   "Tm",      /* name of type                            */
                       0,        /* size of type                            */
                       -1,       /* number of parameters                    */
                       tmMethod, /* set of functions that work on this type */
                       NULL,     /* creator function                        */
                       NULL,     /* destructor function                     */
                       0,        /* configure function                      */
                       0,        /* subtype accessing function              */
		       itfTypeCntlDefaultNoLink,
                       "a Tm gives offsets and penalties for transitions \n"};
                       
TypeInfo TmSetInfo = { "TmSet",          /* name of type                     */
                       0,                /* size of type                     */
                       -1,               /* number of parameters             */
                       tmSetMethod,      /* functions that work on this type */
                       tmSetCreateItf,   /* creator function                 */
                       tmSetFreeItf,     /* destructor function              */
                       tmSetConfigureItf,/* configure function               */
                       tmSetAccessItf,   /* subtype accessing function       */
		       NULL,             /* type control function            */

              "A TmSet is a set of state transition model objects (Tm) \n" } ;

static int transInitialized = 0;

int Tm_Init()
{
  if (! transInitialized) {
    tmSetDefault.name            = NULL;
    tmSetDefault.useN            = 0;
    tmSetDefault.list.itemN      = 0;
    tmSetDefault.list.blkSize    = 20;
    tmSetDefault.commentChar     = ';';

    itfNewType( &TmSetInfo);
    transInitialized = 1;
  }
  return TCL_OK;
}
