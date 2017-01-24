/* ========================================================================
    JANUS      Speech- to Speech Translation
               ------------------------------------------------------------
               Object: TCL <-> C-Objects Interface Functions
               ------------------------------------------------------------

    Author  :  Michael Finke & Ivica Rogina
    Module  :  itf.c
    Date    :  $Id: itf.c 3428 2011-04-15 18:00:28Z metze $
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
    Revision 4.23  2007/03/19 10:46:41  fuegen
    ouput information about system errors occuring during fileOpen (Mark Fuhs)

    Revision 4.22  2007/03/02 13:08:19  fuegen
    bugfix to prevent stack corruption (Friedrich Faubel)

    Revision 4.21  2007/02/21 16:22:55  fuegen
    changes for Windows from Thilo Koehler

    Revision 4.20  2007/02/07 17:11:36  fuegen
    In difference to the comments in time.h CLOCKS_PER_SEC is
    not the same as CLK_TCK. In the manual page of times(2) it
    is mentioned that sysconf(_SC_CLK_TCK) should be used instead.

    Revision 4.19  2007/01/08 15:18:33  fuegen
    replaced CLK_TCK by CLOCKS_PER_SEC for gcc 4.1 compatibility

    Revision 4.18  2006/11/10 10:16:02  fuegen
    merged changes from branch jtk-05-02-02-shajith
    - all modification related to MMIE training
    - first unverified functions for MPE training
    - modifications made by Shajith, Roger, Wilson, and Sebastian

    Revision 4.17  2005/12/14 08:35:45  stueker
    Introduced fgets_maxline

    Revision 4.16  2005/06/27 11:15:53  fuegen
    added -append option to fputsItf

    Revision 4.15  2005/04/25 13:26:46  metze
    Removed janusSignalHandler for DEBUG code

    Revision 4.14  2005/03/17 15:04:34  fuegen
    made again file open working under windows

    Revision 4.13  2005/03/04 15:40:15  metze
    Added check for non-existing file in fileOpen
    (should have been added in the last century)

    Revision 4.12  2005/03/04 10:32:38  metze
    Added DEBUG around error handler (define in Makefile!)

    Revision 4.11  2005/03/04 09:11:20  metze
    Code for 'NFS-SSH' (commented out), fixes for SPHERE

    Revision 4.10.8.1  2006/11/03 12:44:20  stueker
    Increase of itfBufferSize to avoid overflows

    Revision 4.10  2004/05/27 12:26:23  metze
    Added fputs

    Revision 4.9  2004/05/10 14:14:04  metze
    Removed Messages

    Revision 4.8  2003/09/09 08:14:12  metze
    Removed itf-code moved to tcl-lib

    Revision 4.7  2003/08/18 10:11:32  metze
    Moved 'rm mv cp mkdir rmdir touch date sleep wait' into tcl-lib

    Revision 4.6  2003/08/14 11:20:11  fuegen
    Merged changes on branch jtk-01-01-15-fms (jaguar -> ibis-013)

    Revision 4.5.4.17  2003/07/15 16:40:35  fuegen
    changed fileOpen in itfReadFile back to fopen, due to conflicts
    with @STRING arguments to ARGV_STRING in itfParseArgv

    Revision 4.5.4.16  2003/07/07 12:04:45  fuegen
    changed number of retries in fileOpen to 1

    Revision 4.5.4.15  2003/07/04 14:40:01  fuegen
    removed gcc warning

    Revision 4.5.4.14  2003/07/04 13:44:58  fuegen
    removed bugs in fileOpen

    Revision 4.5.4.13  2003/07/02 17:40:08  fuegen
    fixes for file handling under Windows
    - generalized fileOpen/fileClose functions
    - file locking and fgets support
    - fixes in makePath
    removed some memory leaks

    Revision 4.5.4.12  2003/06/06 07:58:43  fuegen
    made gzip/gunzip work under windows

    Revision 4.5.4.11  2003/02/02 12:29:02  soltau
    beautifed itfDestroyAllObjects

    Revision 4.5.4.10  2003/01/15 14:34:45  metze
    Added janusSignalHandler

    Revision 4.5.4.9  2002/11/15 14:32:51  metze
    Added defines for USE_COMPAT_CONST

    Revision 4.5.4.8  2002/11/05 09:03:55  metze
    More code cleaning (for SUNs)

    Revision 4.5.4.7  2002/11/04 14:17:19  metze
    Type information for arguments

    Revision 4.5.4.6  2002/06/26 11:57:55  fuegen
    changes for new directory structure support and code cleaning

    Revision 4.5.4.5  2002/04/25 15:28:24  metze
    *** empty log message ***

    Revision 4.5.4.4  2002/02/28 15:43:42  soltau
    Elegant abort

    Revision 4.5.4.3  2001/11/19 14:49:49  metze
    Go away, boring message!

    Revision 4.5.4.2  2001/06/20 08:52:59  metze
    Added 'bzip2' to supported filters for 'fileOpen'

    Revision 4.5.4.1  2001/05/17 08:20:43  metze
    Added function to get object type

    Revision 4.5  2001/01/15 09:49:57  janus
    Merged Christian's version with WINDOWS support (branch jtk-00-11-14-fuegen)

    Revision 4.4.4.1  2001/01/12 15:16:55  janus
    necessary changes for running janus under WINDOWS

    Revision 4.4  2000/11/14 12:29:33  janus
    Merging branch jtk-00-10-27-jmcd.

    Revision 4.3.6.4  2000/11/14 11:37:42  janus
    More changes to allow for clean compilation with `gcc -Wall'.

    Revision 4.3.6.3  2000/11/09 12:58:58  janus
    More Wall changes.

    Revision 4.3.6.2  2000/11/08 17:15:58  janus
    Finished making changes required for compilation under 'gcc -Wall'.

    Revision 4.3.6.1  2000/11/06 10:50:30  janus
    Made changes to enable compilation under -Wall.

    Revision 4.3  2000/10/24 14:01:19  janus
    Merged changes on branch jtk-00-10-12-tschaaf

    Revision 4.2.28.1  2000/10/24 13:24:12  janus
    moved NDEBUG definition to optimizing definition.
    if NDEBUG is defined assert statements are removed during compilation

    Revision 4.2  2000/08/16 09:24:36  soltau
    added itf for memory_report (see malloc.c)
    free -> Tcl_Free

    Revision 4.1  2000/01/12 10:02:02  fuegen
    *** empty log message ***

    Revision 4.0  1998/11/30 18:07:14  fuegen
    *** empty log message ***

    Revision 3.10  1998/11/04 10:26:10  kries
    Added more support for Python

    Revision 3.9  1998/06/30 01:07:43  kries
    *** empty log message ***

    Revision 3.8  1998/05/26 11:11:29  kries
    added ARGV_LONG

 * Revision 3.7  1998/01/14  09:55:31  tschaaf
 * Removed Bug in unknownHandler (Body)
 * The bug occurs if the ':'-operatore is used
 * With bug    'dict:relative' produce 'invalid command name "dict:relative"'
 * Without bug 'dict:relative' produce 'invalid command name "dict"'
 * All commands within an 'ti' did not work properly
 * The special handling of TIX-Objects introduced the bug
 *
    Revision 3.6  1997/10/30 13:52:31  rogina
    during the DFKI-cleaning, obviously some function that were used without
    itf-parameters (e.g. dbasePutsItf) got a dummy itfParseArgv with an
    incorrect "argc--" ahead. This caused e.g. the following script to segfault:
    "DBase db ; db" Since I did not want to check every module for such bugs,
    I have modified the while(argc) statement to while(argc>0) such that we will
    not get into an endless loop when argc is negative.

    Revision 3.5  1997/07/15 11:24:07  westphal
    removed bug with %

    Revision 3.4  1997/06/26 09:07:57  tschaaf
    added warning to user/system time

    Revision 3.3  1997/06/26 08:23:32  tschaaf
    added -total Falg to usertime and systemtime (thats new timequery)

    Revision 3.2  1997/06/17 17:41:43  westphal
    *** empty log message ***

 
   ======================================================================== */

#include "common.h"
#include "itf.h"
#include "list.h"
#include "array.h"

#include <stdarg.h>
#include <time.h>
#include <locale.h>
#include <math.h>
#include <signal.h>
#include <errno.h>
#include <tcl.h>

#define redefineBuffer unlink	/* because we already have an unlink */
#undef unlink

#ifndef WINDOWS
	#include <sys/times.h>
	#include <dirent.h>
        #include <utime.h>
        #include <sys/stat.h>
        #include <sys/mman.h>
        #include <readline/history.h>
        #include <readline/readline.h>
#else
	#include <sys/types.h>
	#include <sys/stat.h>
	#include <direct.h>
	#include <io.h>
	#include <process.h>
	#include <sys/utime.h>
#endif

char itfRCSVersion[]= "@(#)1$Id: itf.c 3428 2011-04-15 18:00:28Z metze $";


/* ========================================================================
    Global Declarations
   ======================================================================== */

Tcl_Interp* itf = NULL;              /* global pointer to TCL interpreter   */

typedef struct {

  ClientData     item;               /* pointer to instantiated object      */
  TypeInfo      *type;               /* pointer to related type information */
  Tcl_HashEntry *entry;              /* pointer to hash table position      */

} ObjectEntry;

static Tcl_HashTable itfTypes;       /* table of defined types              */
static Tcl_HashTable itfObjects;     /* hash table of instantiated objects  */

#define itfBufferSize 1000000

static char itfBuffer[itfBufferSize];

#define MAXCPARG 511             /* max characters per commandline argument */

extern int init_mach_ind_io();

/*
  FILE *STDOUT = stdout;
  FILE *STDIN  = stdin; 
  FILE *STDERR = stderr;
*/

/* ========================================================================
    Type Definition and Object Construction
   ======================================================================== */

static int  itfCreate(     ClientData clientData, Tcl_Interp *interp, 
                           int argc, char *argv[]);

static int  itfMethodHelp( TypeInfo* ti);

static int  itfDispatcher( ClientData clientData, Tcl_Interp *interp, 
                           int argc, char *argv[]);
static void itfDestroy (   ClientData clientData);

/* ------------------------------------------------------------------------
    itfNewType   declare a new type
   ------------------------------------------------------------------------ */

int itfNewType( TypeInfo *typeInfo)
{
  int            newEntry;
  Tcl_HashEntry *entry = Tcl_CreateHashEntry ( &itfTypes, typeInfo->name, 
                                               &newEntry );
  if (newEntry) {

    Tcl_SetHashValue(  entry, (ClientData)typeInfo );
    Tcl_CreateCommand( itf, typeInfo->name, itfCreate, 
                       (ClientData)typeInfo, 0);
    return TCL_OK;
  }
  WARN( "<ITF,TAE>Type '%s' already exists.\n", typeInfo->name);
  return TCL_ERROR;
}

/* ------------------------------------------------------------------------
    itfConfigure
   ------------------------------------------------------------------------ */

static int itfConfigure ( ClientData item, Configurer* configProc, 
			  Tcl_Interp *interp, 
			  int argc, char *argv[] )
{
  if (! configProc) {
    WARN( "<ITF,CCO>Can't configure object.\n");
    return TCL_ERROR;
  }
  if (argc==0) return configProc ( item, NULL, NULL ); 

  while (argc>0)
    {
      int dummy;
      char *variable, *value;

      if (argv[0][0]!='-') {
        SWARN( "<ITF,CVM>Configure variable '%s' must start with -.\n", argv[0]);
        return TCL_ERROR;
      }
      variable = argv[0]+1;
      if (argc>1) value = argv[1]; else value = NULL;
      
      if  ((dummy = configProc (item, variable, value)) != TCL_OK)
	return dummy;

      argc-=2; argv+=2;
    }
  return TCL_OK;
}

/* ------------------------------------------------------------------------
    itfCreate
   ------------------------------------------------------------------------ */

static int itfCreate( ClientData clientData, Tcl_Interp *interp, 
                      int argc, char *argv[])
{
  int            newEntry;
  Tcl_HashEntry *entry;
  ClientData     item;
  ObjectEntry   *object;
  TypeInfo      *ti = (TypeInfo*)clientData;

  itf = interp;
  argc--; argv++; /* argv[0] is redundant, we have all we need in clientData */

  if (argc>0) {
    char* name = *argv;

    if (! strcmp( name, "configure")) {

      return itfConfigure( NULL, ti->configure, interp, argc-1, argv+1);
    }
    else if (! strcmp( name, "-help") || ! strcmp( name, "help") ||
             ! strcmp( name, "?")) {
      return itfMethodHelp( ti);
    }
    else if (! strcmp( name, "method")) {
      if (itfTclMethod (ti,argc,argv)!=TCL_OK) {
         ERROR("<ITF,CCM>Can't create method for %s.\n",ti->name);
         return TCL_ERROR;
      } 
      return TCL_OK;
    }

    if ( Tcl_FindHashEntry ( &itfObjects, name) != NULL ) {
      WARN( "<ITF,OAE>Object %s already exists.\n", name);
      return TCL_ERROR;
    }

    if (! ti->constructor) {

      if (ti->size <1 || !( item = (ClientData)calloc(1,ti->size))) {

        ERROR( "<ITF,CAO>Failed to allocate '%s' object.\n", name);
        return TCL_ERROR;
      }
    }
    else {

      if ( ti->argN >= 0 && argc-1 < ti->argN) {

        ERROR( "<ITF,NMA>Need %d argument(s) to create '%s' object.\n",
                ti->argN, ti->name);
        return TCL_ERROR;
      }
      if (! (item = ti->constructor( clientData,  
                                     ti->argN<0 ? argc : ti->argN+1, argv ))) {
        ERROR( "<ITF,FCO>Failed to create '%s' object.\n", name);
        return TCL_ERROR;
      }
    }

    /* Save the newly created object in the itfObjects hash table to
       make retrieval of typed TCL objects possible (see itfGetObject) */

    entry = Tcl_CreateHashEntry ( &itfObjects, name, &newEntry );
    assert( newEntry);

    object        = (ObjectEntry*)malloc( sizeof(ObjectEntry));
    object->type  = (TypeInfo*)clientData;
    object->item  =  item;
    object->entry =  entry;

    Tcl_SetHashValue(  entry, object );
    Tcl_CreateCommand( interp, name, itfDispatcher, (ClientData)object, 
                       itfDestroy);

    itfTypeCntl(item,object->type,T_LINK);

    itfAppendResult("%s",name);

    if ( ti->argN >= 0 && argc > ti->argN+2 && ti->configure!=NULL) { 

       (void)itfConfigure( item, ti->configure, interp, 
                           argc-ti->argN-1,argv+ti->argN+1 ); 
    }
  }
  else {

    return itfMethodHelp( ti);
  }
  return TCL_OK;
}

/* ------------------------------------------------------------------------
    itfTclMethod
   ------------------------------------------------------------------------ */

int itfTclMethod (TypeInfo* ti, int argc, char **argv)
{
  int ac=argc-1;
  char *method=NULL, *proc=NULL, *help="";
  Method *m = ti->method;

  if (itfParseArgv( argv[0], &ac, argv+1, 0,
     "<method>", ARGV_STRING,  NULL, &method, NULL, "name of method",
     "<proc>",   ARGV_STRING,  NULL, &proc,   NULL, "name of procedure",
     "-text",    ARGV_STRING,  NULL, &help,   NULL, "help text",
     NULL) != TCL_OK) return TCL_ERROR;

  if (!strcmp(proc,"destroy")) 
     return Tcl_VarEval(itf,"unset ",ti->name,"Methods(",method,")",NULL);

  while (m->name) 
  {
	if (!strcmp(m->name,method)) 
           INFO("<ITF,RM>Redefining method %s. Use _%s for original method.\n",method,method);
	m++;
  }
  return Tcl_VarEval(itf,"set ",ti->name,"Methods(",method,") { ",proc," \"",help,"\" }",NULL);
}

/* ------------------------------------------------------------------------
    itfMethodHelp
   ------------------------------------------------------------------------ */

static int itfMethodHelp( TypeInfo* ti)
{
  Method *method = ti->method;

  if ( ti->help) itfAppendResult( "\nDESCRIPTION\n\n%s\n", ti->help);

  if ( method) {
    itfAppendResult( "\nMETHODS\n\n");
    while ( method->name) {

      itfAppendResult("%-18s %s\n", method->name, 
                     ( method->help) ? method->help : "");
      method++;
    }
  }

  /* show TCL-DEFINED METHODS */
  {  
     char *oldResult;
#ifdef USE_COMPAT_CONST
     CONST char *newResult;
#else
     char *newResult;
#endif

     //oldResult = (itf->result) ? strdup(itf->result) : NULL;
     oldResult = (strlen (Tcl_GetStringResult (itf))) ? strdup (Tcl_GetStringResult (itf)) : NULL;

     Tcl_VarEval(itf, 
       "set itfResult \"\" ; ",
       "foreach itfArrName [array names ", ti->name, "Methods] { ",
       "set itfArrVal $", ti->name, "Methods($itfArrName); ",
       "set itfResult [format \"%s\n%-18s %s (%s)\" $itfResult ",
       "$itfArrName [lindex $itfArrVal 1] [lindex $itfArrVal 0] ] }",NULL);

     newResult = Tcl_GetVar(itf,"itfResult",TCL_GLOBAL_ONLY);
     Tcl_ResetResult(itf);
     if ( oldResult) Tcl_AppendResult(itf,oldResult,NULL); 
     if ( newResult && newResult[0])
     {
       Tcl_AppendResult(itf,"\nTCL-DEFINED METHODS\n",0);
       Tcl_AppendResult(itf,newResult,NULL);
     }
     if ( oldResult) free(oldResult);
  }

  return TCL_OK;
}

/* ------------------------------------------------------------------------
    read file into string
   ------------------------------------------------------------------------ */

static char *readFileBuffer = NULL;

void itfFreeReadFileBuffer()
{
  if (readFileBuffer) { free(readFileBuffer); readFileBuffer=NULL; }
}

char *itfReadFile (char *filename)
{
  FILE *fp; 
  int   size;

  if (!filename) return NULL;
#ifdef WINDOWS
  if ((fp = fopen(filename,"rb"))==NULL) return NULL;
#else
  if ((fp = fopen(filename,"r"))==NULL) return NULL;
#endif
  fseek(fp,0,SEEK_END); size=ftell(fp); rewind(fp);
  if (readFileBuffer != NULL) itfFreeReadFileBuffer();
  readFileBuffer = (char*)malloc(sizeof(char)*(size+1));
  if (readFileBuffer == NULL) return NULL;
  fread(readFileBuffer,sizeof(char),size,fp);
  readFileBuffer[size] ='\0';
  fclose(fp);
  return readFileBuffer;
}

/* ------------------------------------------------------------------------
    itfDispatcher
    This is the C-function which is executed for any new method - it's common
    to all of them. This is to establish a common behaviour for things like
    'configure', call-without-arguments, and so on.
    ClientData is a 'ObjectEntry'-type pointer, argc/argv contains all parameters
    including the command name itself.
   ------------------------------------------------------------------------ */

#define cmdLen 1000
static char cmd[cmdLen];

static int itfDispatcher ( ClientData clientData, Tcl_Interp *interp, 
                           int argc, char *argv[])
{
  TypeInfo*  ti   = ((ObjectEntry*)clientData)->type;
  ClientData item = ((ObjectEntry*)clientData)->item;
  char       *command;
  char       *saveTheStack[2]; /* dummy variable to prevent stack corruption */

  itfFreeReadFileBuffer();

  itfResetResult();
  itf = interp;

/*---------------*/
/* START FF HACK */  
/*---------------*/
/* ORIGINALLY: if (strlen(*argv)>=cmdLen)*/
/* length is string-length + 1 null character */
  if (strlen(*argv)+1>=cmdLen) 
/*-------------*/
/* END FF HACK */  
/*-------------*/
  {  ERROR("<ITF,CNL>command name too long\n"); return TCL_ERROR; }
  strcpy(cmd,*argv);
  /* ------------------------------------------------- */
  /* cmd is now the name of the command, in most cases */
  /* the name of an instantiated object. E.g., if you  */
  /* type 'dict get Muenchen', it would be 'dict'.     */
  /* ------------------------------------------------- */

  argc--; argv++;

  /* --------------------------------------------------------- */
  /* Handle the no-argument-case: this is equivalent to 'puts' */
  /* --------------------------------------------------------- */
  if (argc==0) command = "puts"; 
  else         command = *argv;

  /* ------------------------------------------------------ */
  /* If the user enters 'dict.' or 'dict:', the TCL unknown */
  /* procedure will split this into 'dict .' or 'dict :'.   */
  /* (see main.c for details of the unknown procedure).     */
  /* Therefore, the . or : will be treated as a command.    */
  /* Do we have such a reference to a sub-object?           */
  /* ------------------------------------------------------ */
  if (argc && (strcmp(*argv,":")==0 || strcmp(*argv,".")==0)) {
    /* ----------------------------------- */
    /* yes, this is a sub-object access... */
    /* ----------------------------------- */
    TypeInfo   *subti;
    ClientData subitem;
    char       destroyString[MAXCPARG] = { '\0' };

    while (argc && (! strcmp(*argv,":") || ! strcmp(*argv,"."))) {
      char arg[MAXCPARG];

      if (argc>1 && strlen(argv[0])+strlen(argv[1])+strlen(cmd)>=cmdLen) 
      {  ERROR("<ITF,CNL>command name too long\n"); return TCL_ERROR; }

      strcpy(arg,*argv); if (argc>1) strcat(arg,argv[1]);
      strcat(cmd,*argv); if (argc>1) strcat(cmd,argv[1]);

      strcat(destroyString,argv[-1]); strcat(destroyString," "); 
      if (argc>3) { strcat(destroyString,*argv);  strcat(destroyString," "); }

      if (! ti->access) {
        ERROR( "<ITF,NSO>'%s' has no sub-objects.\n", argv[-1]);
        return TCL_ERROR;
      }

      subti   = (TypeInfo*)1;
      subitem = ti->access(item, arg, &subti);

      /* The access() function convention is to set the type pointer to
         NULL to signal that something useful has been done but there was
         no way to return a specific object. */

      if (!subti) return TCL_OK;

      if (subitem==NULL && subti) {
        ERROR( "<ITF,CAS>Can't access sub-object '%s'.\n",arg);
        return TCL_ERROR;
      }

      item = subitem; ti = subti;
      argc-=2; argv+=2;
    }

    /* ----------------------------------------------- */
    /* Again, no-parameter has to be handled as 'puts' */
    /* ----------------------------------------------- */
    if (argc==0) command = "puts";
    else         command = *argv;

    /* ------------------------------------------------------- */
    /* Handle 'destroy-a-subobject'. We do not want to have a  */
    /* sub-object destroyed which is really part of some C     */
    /* struct, although that might look reasonable to the user.*/
    /* The only destruction that can be allowed is if we have  */
    /* a list of sub-*objects* of some parent-objects: we might*/
    /* destroy one element of this list (but, to stick to the  */
    /* example, not the list itself). We can therefore destroy */
    /* only things like 'dict:xxx', but not 'dict.xxx'. Check  */
    /* for that:                                               */
    /* ------------------------------------------------------- */
    if (strcmp(command,"destroy")==0) {
      /* --------------------------------------------------- */
      /* The destroy itself is performed by calling Tcl_Eval */
      /* Note that this will probably call the ItfDispatcher */
      /* again, so this is recursion here.                   */
      /* --------------------------------------------------- */
      strcat(destroyString,"delete "); strcat(destroyString,argv[-1]);
      if (argv[-2][0]==':') return Tcl_Eval(interp,destroyString);
      else { 
        WARN( "<ITF,CD>Can't destroy %s.\n", destroyString);
        return TCL_ERROR;
      }
    }
  }
  /* ------------------------------------------------------------- */
  /* Handle all commands that are common to all objects. These are */
  /* 'configure', 'destroy', 'help', and 'type':                   */
  /* ------------------------------------------------------------- */
  if (strcmp(command,"configure")==0) {
    int retCode = itfConfigure ( item, ti->configure, interp, argc-1, argv+1 );
    if ( retCode != TCL_OK) MSGPRINT(NULL);
    return retCode;
  }

  if (strcmp(command,"destroy")==0)
    return Tcl_DeleteCommand ( interp, argv[-1] );

  /* --------------------------------------------------- */
  /* 'dict type' will return 'Dictionary'.               */
  /* This is simply the 'name' field of the typeinfo ti: */
  /* --------------------------------------------------- */
  if (strcmp(command,"type")==0) {
    Tcl_SetResult(interp,ti->name,TCL_VOLATILE);
    return TCL_OK;
  }
  /* We don't need that 'else', do we??? */
  else {
    /* ---------------------------------------------------------- */
    /* Well, it is none of the standard methods. For example, for */ 
    /* 'dict get Muenchen', we would get here with 'command' being*/
    /* 'get' and 'cmd' being 'dict'.                              */
    /* ---------------------------------------------------------- */
    Method *method = ti->method;
    Method *m      = NULL;
    int     retCode = TCL_ERROR, i = 0, l = strlen(command);
    /* --------------------------------------- */
    /* 'help' method, may be -help, help, or ? */
    /* --------------------------------------- */
    if (! strcmp( command, "-help") || ! strcmp( command, "help") ||
        ! strcmp( command, "?")) {
      return itfMethodHelp( ti);
    }

    /* ---------------------------------------------------- */
    /* The name of the method might be ambiguous, in case   */
    /* one TCL-defines a 'get' method for the Dictionary    */
    /* type. As we are parsing TCL methods first, the TCL   */
    /* definition would hide the original one. If a '_' is  */
    /* put in front of the command (e.g. _get), it is not   */
    /* tried as a TCL-defined method.                       */
    /* ---------------------------------------------------- */
    if (command[0]=='_') command++;
    else 
    {
      /* ----------------------------------------------------- */
      /* Scan whether the to-be-executed method is TCL-defined */
      /* All TCL-defined methods are stored in a TCL array     */
      /* named after the class. For the dictionary, that would */
      /* be 'DictionaryMethods()'.                             */
      /* ----------------------------------------------------- */
        char arrayName[100], **tokv;
#ifdef USE_COMPAT_CONST
	CONST char *methodDesc;
#else
	char *methodDesc;
#endif
        int  tokc, argx;
        DString tclCommand;

        dstringInit(&tclCommand);

        sprintf(arrayName,"%sMethods",ti->name);
	/* -------------------------------------------------- */
	/* Ask TCL if an array element DictionaryMethods(get) */
	/* exists or not.                                     */
	/* -------------------------------------------------- */
        if ((methodDesc = Tcl_GetVar2(itf,arrayName,command,TCL_GLOBAL_ONLY))){
	  /* --------------------------------------------- */
	  /* yes, it exists, it is a TCL-defined procedure */
	  /* --------------------------------------------- */
	  if ( Tcl_SplitList( itf, methodDesc, &tokc, &tokv) == TCL_OK) {
	    if (tokc>0) {
              Tcl_VarEval(itf,"info commands ",tokv[0],"<",cmd,">",NULL);
              //if (itf->result[0]){
	      if (strlen (Tcl_GetStringResult (itf))) {
		dstringAppend (&tclCommand, "%s<%s> %s",tokv[0],cmd,cmd);
	      }
              else {
		dstringAppend (&tclCommand, "%s %s",tokv[0],cmd);
	      }
	      for (argx=1; argx<argc; argx++) {
		dstringAppendElement(&tclCommand, "%s", argv[argx]);
	      }
	      /* ------------------------- */
	      /* Execute the TCL procedure */
	      /* ------------------------- */
	      retCode = Tcl_Eval(itf,dstringValue(&tclCommand));
              /* if ( retCode != TCL_OK && itf->result) {
		 char *ir = malloc (strlen (itf->result) + 4);
		 strcpy (ir, itf->result);
                 ERROR( "%s: %s\n", dstringValue(&tclCommand), ir);
		 free (ir);
                 Tcl_ResetResult( itf);
              } */
              if (retCode != TCL_OK && strlen (Tcl_GetStringResult (itf))) {
		 char *ir = malloc (strlen (Tcl_GetStringResult (itf)) + 4);
		 strcpy (ir, Tcl_GetStringResult (itf));
		 ERROR( "%s: %s\n", dstringValue(&tclCommand), ir);
		 free (ir);
                 Tcl_ResetResult( itf);
              }
	    }
	    Tcl_Free((char*)tokv);
	  }
	  /* -------------------------- */
	  /* That's it, tidy up, return */
	  /* -------------------------- */
	  dstringFree(&tclCommand);
	  MSGPRINT(NULL);
	  return retCode;
	} /* was it a TCL-defined method? */
    }

    /* ---------------------------------------------------- */
    /* method is a pointer to a list of C-defined Methods   */
    /* for the current type. The list is terminated by NULL */
    /* ---------------------------------------------------- */
    if (method == NULL) {
        WARN( "<ITF,NMD>No methods defined for type '%s'.\n", ti->name);
        return TCL_ERROR;
    }
    /* go thru the list of methods. */
    while (method->name) 
      {
	if (! strncasecmp(method->name,command,l)) {
	  i++; /* count the hits if upper/lower case don't matter */
          m = method;
	  if (!strcmp(m->name,command)) { i=1; break; }
        }
	method++;
      }

    if (! m) {
      WARN( "<ITF,MND>No method '%s' is defined for type %s.\n",command,ti->name);
      return TCL_ERROR;
    }
    else if (i>1)
      {
	ERROR( "<ITF,AMN>Ambiguous method name '%s' is defined for type %s.\n",
	       command,ti->name);
	return TCL_ERROR;
      }
    /* ------------------------------------------------ */
    /* Alright, now 'm' is a pointer to a 'Method' type */
    /* m->proc is the C-function associated with the    */
    /* method. Call it, return, finished.               */
    /* ------------------------------------------------ */
    if (argc==0)
      {
	/* save the old stack to pass it to the next function */
	saveTheStack[0] = command;
	saveTheStack[1] = NULL;
	retCode = m->proc(item, 1, saveTheStack);
      }
    else
      retCode = m->proc(item, argc, argv);
    
    MSGPRINT(NULL);
    return retCode;
  }
  /* return TCL_ERROR; */
}

/* ------------------------------------------------------------------------
    itfCommandName
   ------------------------------------------------------------------------ */

char *itfCommandName() { return cmd; }

/* ------------------------------------------------------------------------
    itfDestroy
   ------------------------------------------------------------------------ */

static void itfDestroy ( ClientData clientData )
{
  TypeInfo         *ti = ((ObjectEntry*)clientData)->type;
  ClientData      item = ((ObjectEntry*)clientData)->item;
  Tcl_HashEntry *entry = ((ObjectEntry*)clientData)->entry;
  char           *name = Tcl_GetHashKey ( &itfObjects, entry );
  char        *newName = NULL;
  int         newEntry = 0;
  ObjectEntry*  object = (ObjectEntry*)clientData;

  /* sanity check */
  {
    Tcl_HashEntry *entry2 = Tcl_FindHashEntry(&itfObjects,name);
    if (entry != entry2 || ! entry2)
      return;
  }

  if (itfTypeCntl(item,ti,T_UNLINK)!=0)
    { WARN("<ITF,CDO>could not destroy object %s\n",name);
      newName = (char*)malloc(strlen(name)+2);
      sprintf(newName,"%s~",name);
      Tcl_DeleteHashEntry(entry);
      object->entry = Tcl_CreateHashEntry ( &itfObjects, newName, &newEntry );
      assert(newEntry);
      Tcl_SetHashValue(object->entry, (ClientData)object);
      Tcl_CreateCommand( itf, newName, itfDispatcher, (ClientData)object, 
                       itfDestroy);
      INFO("<ITF,LTA>you've lost tcl access to object %s, use %s instead\n",name,newName);
    }
  else Tcl_DeleteHashEntry(entry);
  MSGPRINT(NULL);
}

/* ------------------------------------------------------------------------
    itfDestroy
   ------------------------------------------------------------------------ */

void itfDestroyAllObjects (void)
{ 
  int CanDelete =1;

  while (CanDelete) {
    Tcl_HashSearch search;
    Tcl_HashEntry *entry =  Tcl_FirstHashEntry( &itfObjects,&search);

    CanDelete = 0;
    while (entry) {
      ObjectEntry* object = (ObjectEntry*) Tcl_GetHashValue (entry);
      ClientData     item =  object->item;
      GenericObject  *obj = (GenericObject*) item;
      
      if (obj->useN == 1) {
	INFO("destroyed object '%s'\n",obj->name);
	itfTypeCntl(item,object->type,T_UNLINK);
	Tcl_DeleteHashEntry(entry);
	CanDelete = 1;
	entry =  NULL;
      } else {
	entry = Tcl_NextHashEntry(&search);
      }
    }
  }

  if (0) {
    Tcl_HashSearch search;
    Tcl_HashEntry *entry =  Tcl_FirstHashEntry( &itfObjects,&search);
    while (entry) {
      ObjectEntry* object = (ObjectEntry*) Tcl_GetHashValue (entry);
      ClientData     item =  object->item;
      GenericObject  *obj = (GenericObject*) item;
      INFO("couldn't destroy object '%s'\n",obj->name);
      entry = Tcl_NextHashEntry(&search);
    }
  }

  return;
}

/*---------------*/
/* START FF HACK */
/*---------------*/
/* NECESSARY FOR REMOVING CFGs from CFGSet */
/* ------------------------------------------------------------------------
    itfRemoveObjectHash
   ------------------------------------------------------------------------ */
int itfRemoveObjectHash(ClientData clientData) {

  GenericObject *obj = (GenericObject*)clientData;
  ClientData cd = Tcl_FindHashEntry(&itfObjects, obj->name);
  if (cd!=NULL)
  {
    Tcl_DeleteHashEntry(cd);
    return TCL_OK;
  }
  return TCL_ERROR;
}
/*-------------*/
/* END FF HACK */
/*-------------*/

/* ------------------------------------------------------------------------
    itfTypeCntl
   ------------------------------------------------------------------------ */

int itfTypeCntlDefaultNoLink( ClientData clientData, TypeInfo *typeInfo, 
                              int        command, ...)
{
  int ret = 0;
  va_list ap;
  va_start(ap,command);

  assert(typeInfo);

  switch (command)
  {
    case T_LINK:     
    case T_LINKN:
    case T_DELINK:   break;
    case T_UNLINK:   if (typeInfo->destructor) typeInfo->destructor(clientData);
                     else if (typeInfo->size>0) free(clientData);
                     break;
    default:         ret = itfTypeCntlDefault(clientData, typeInfo, command, ap);
  }
  va_end(ap);
  return ret;
}

int itfTypeCntlDefault( ClientData clientData, TypeInfo *typeInfo, 
                        int        command, ...)
{
  GenericObject *obj = (GenericObject*)clientData;
  int ret = -1;

  va_list ap;
  va_start(ap,command);

  assert(typeInfo);

  switch (command)
  {
    case T_LINK:     if (obj->useN>=0) ++obj->useN;
                     ret = obj->useN;
                     break;

    case T_DELINK:   if ( obj->useN < 0) {
                       WARN("<ITF,ONL>Object %s not linked\n", (obj->name) ?
                                                       obj->name : "(nil)");
                       goto endItfTypeCntlDefault;
                     }
                     if ( obj->useN > 0) --obj->useN;
                     ret = obj->useN;
                     break;

    case T_UNLINK:   if (obj->useN<0) 
                     { WARN("<ITF,ONL>Object %s not linked\n",obj->name?obj->name:"(nil)"); goto endItfTypeCntlDefault; }
                     if (obj->useN>0) --obj->useN; 
		     if (obj->useN==0) {
                        if (typeInfo->destructor) typeInfo->destructor(clientData);
                        else if (typeInfo->size>0) free(clientData);
			ret = 0;
		     } else {
		        ret = obj->useN;
		     }
                     break;

    case T_LINKN:    ret = obj->useN; break;

    default:         FATAL("command %d undefined\n",command);
  }
  endItfTypeCntlDefault:
  va_end(ap);
  return ret;
}

int itfTypeCntl (ClientData clientData, TypeInfo *typeInfo, int command, ...)
{
  int ret = -1;
  va_list  ap;
  va_start(ap,command);
  assert(typeInfo);
  if (typeInfo->typeCntl) ret = typeInfo->typeCntl(clientData,typeInfo,command,ap);
  else ret = itfTypeCntlDefault(clientData,typeInfo,command,ap);
  va_end(ap);

  return ret;
}

/* ------------------------------------------------------------------------
    itfListTypes      list all types currently declared and stored in
                      hash table itfTypes
   ------------------------------------------------------------------------ */

int itfListTypes ( ClientData clientData, Tcl_Interp *interp,
                   int argc, char *argv[])
{
  Tcl_HashEntry *entry;
  Tcl_HashSearch search;

  for ( entry = Tcl_FirstHashEntry( &itfTypes, &search ); entry; 
        entry = Tcl_NextHashEntry(  &search )) {
    Tcl_AppendElement ( interp, Tcl_GetHashKey ( &itfTypes, entry ));
  }
  return TCL_OK;
}

/* ------------------------------------------------------------------------
    itfListObjects    list all objects currently declared and stored in
                      hash table itfObjects
   ------------------------------------------------------------------------ */

int itfListObjects ( ClientData clientData, Tcl_Interp *interp, 
                     int argc, char *argv[])
{
  Tcl_HashEntry *entry;
  Tcl_HashSearch search;

  for ( entry = Tcl_FirstHashEntry( &itfObjects, &search ); entry; 
        entry = Tcl_NextHashEntry(  &search )) {
    char* objName = Tcl_GetHashKey ( &itfObjects, entry );

    if ( argc > 1 && (! itfGetObject(objName,argv[1]) &&
                        strcmp("*",argv[1]))) { 
         MSGCLEAR(NULL); continue;
    }
    else MSGCLEAR(NULL);
    if ( argc > 2 &&   strcmp(objName,argv[2]))       continue;

    Tcl_AppendElement ( interp, objName);
  }
  return TCL_OK;
}

/* ------------------------------------------------------------------------
    itfGetType
   ------------------------------------------------------------------------ */

TypeInfo* itfGetType( char *typeName )
{
  Tcl_HashEntry *entry;

  if ((entry = Tcl_FindHashEntry( &itfTypes, typeName)) == NULL) { 
    SERROR( "<ITF,TND>No type '%s' defined.\n", typeName); 
    return NULL; 
  }
  else return (TypeInfo*)Tcl_GetHashValue( entry);
}


/* ------------------------------------------------------------------------
    itfGetType
   ------------------------------------------------------------------------ */

char* itfGetObjectType( char *objectName )
{
   Tcl_HashEntry *entry;
   if ((entry = Tcl_FindHashEntry ( &itfObjects, objectName)) == NULL) { 
     SERROR( "<ITF,TND>No object type '%s' defined.\n", objectName); 
     return NULL;
   } else {
     ObjectEntry* object = (ObjectEntry*) Tcl_GetHashValue (entry);
     return object->type->name;
   }
}


/* ------------------------------------------------------------------------
    itfGetObject
   ------------------------------------------------------------------------ */

ClientData itfGetObject ( char *objectName, char *typeName )
{
  Tcl_HashEntry *entry;
  TypeInfo      *subti;
  ObjectEntry   *object;
  ClientData     subitem;

  char *p, *q, subName[MAXCPARG];
  int  cnt = 0;

  if ((entry = Tcl_FindHashEntry ( &itfObjects, objectName)) != NULL) { 
    object  = (ObjectEntry*) Tcl_GetHashValue (entry);
    subitem =  object->item;
    subti   =  object->type;
    if (strcmp(subti->name,typeName)!=0) {

      SERROR( "<ITF,TM>Type mismatch: %s is of type %s and not of type %s.\n",
	      objectName, subti->name, typeName); 
      return NULL;
    }
    return subitem;
  }

  p = objectName; q = subName;

  while (*p!=':' && *p!='.' && *p!='\0' && ++cnt<MAXCPARG) *q++ = *p++;
  *q = '\0';
  if (cnt==MAXCPARG) return NULL;

  if ((entry = Tcl_FindHashEntry ( &itfObjects, subName)) == NULL) { 
    SERROR( "<ITF,OND>No object '%s' of type '%s' defined.\n", subName, typeName); 
    return NULL; 
  }

  object  = (ObjectEntry*) Tcl_GetHashValue (entry);
  subitem =  object->item;
  subti   =  object->type;
  
  while ( *p!='\0' )
    {
      ClientData  subsubitem;
      TypeInfo   *subsubti;

      q = subName;
      *q++ = *p++;

      while (*p!=':' && *p!='.' && *p!='\0') *q++ = *p++;   
      *q = '\0';

      if (subti->access==NULL) { 
        SERROR( "<ITF,SOND>'%s' is no sub-object in '%s'.\n", subName, objectName); 
        return NULL;
      }

      if (subName[1] == '\0') {
        SERROR( "<ITF,UIO>Uninterpretable object name '%s'.\n",objectName); 
        return NULL;
      }

      subsubti   = (TypeInfo*)1;
      subsubitem = subti->access ( subitem, subName, &subsubti );

      if (! subsubti) {
        SERROR( "<ITF,UIO>Uninterpretable object name '%s'.\n",objectName); 
        return NULL;
      }

      if (subsubitem==NULL && subsubti) { 
        SERROR( "<ITF,CASO>Can't access sub-object '%s' in '%s'.\n",
                 subName, objectName); 
        return NULL; 
      }
      subitem = subsubitem; subti = subsubti;
    }

  if (strcmp(subti->name,typeName)!=0) {
 
    SERROR( "<ITF,TM>Type mismatch: %s is of type %s and not of type %s.\n",
             objectName, subti->name, typeName); 
    return NULL;
  }
  return subitem;
}

/* ------------------------------------------------------------------------
    itfMakeArray
   ------------------------------------------------------------------------ */

static int itfMakeArray( ClientData clientData, Tcl_Interp *interp, 
                         int argc, char *argv[])
{
  int    tokc;
  char** tokv;
  int    i;
  char   name[2000];
  char*  vname =  NULL;
  char*  value =  NULL;
  char*  string;
  
  argc--;
  if (itfParseArgv( argv[0], &argc, argv+1, 0,
		   "<name>", ARGV_STRING,  NULL, &vname, NULL, "name of array",
		   "<list>", ARGV_STRING,  NULL, &value, NULL, "value",
		   NULL) != TCL_OK) return TCL_ERROR;

  sprintf( name, "if { [array exists %s]} { unset %s} ", vname, vname);
  Tcl_Eval( interp, name);

  if (Tcl_SplitList( interp, value, &tokc, &tokv) == TCL_ERROR) {
    /* In this case, no memory was allocated */
    SERROR ("Tcl_SplitList unsuccessful with '%s'\n", value);
    return TCL_ERROR;
  }
  for ( i = 0; i < tokc; i++) {
    int    tc;
    char** tv;

    if (Tcl_SplitList( interp, tokv[i], &tc, &tv) == TCL_ERROR)
      continue;

    if ( tc > 1) {
      string = Tcl_Merge( tc-1, tv+1);
      Tcl_SetVar2(interp, vname, tv[0], string, TCL_LEAVE_ERR_MSG);
      Tcl_Free((char*) string);
    } else {
      Tcl_SetVar2(interp, vname, tv[0], "", TCL_LEAVE_ERR_MSG);
    }
    Tcl_Free((char*) tv);
  }
  Tcl_Free((char*) tokv);
  return TCL_OK;
}

/* ========================================================================
    Dynamic Strings
   ======================================================================== */

void  dstringInit(   DString* dsP) { Tcl_DStringInit( dsP); }
void  dstringFree(   DString* dsP) { Tcl_DStringFree( dsP); }
void  dstringResult( DString* dsP) { Tcl_DStringResult( itf, dsP); }

void  dstringGetResult(    DString* dsP) { Tcl_DStringGetResult(    itf,dsP); }
void  dstringStartSublist( DString* dsP) { Tcl_DStringStartSublist( dsP); }
void  dstringEndSublist(   DString* dsP) { Tcl_DStringEndSublist(   dsP); }

void  dstringSetLength( DString* dsP, int len)
{
  Tcl_DStringSetLength( dsP, len);
}

char* dstringAppend( DString* dsP, char* format, ...)
{
  va_list  ap;
  int      n;

  va_start(ap,format);
  n = vsprintf( itfBuffer, format, ap);
  assert (itfBufferSize > n);
  va_end(ap);
  return Tcl_DStringAppend( dsP, itfBuffer, -1);
}

char* dstringAppendElement( DString* dsP, char* format, ...)
{
  va_list  ap;
  int      n;

  va_start(ap,format);
  n = vsprintf( itfBuffer, format, ap);
  assert (itfBufferSize > n);
  va_end(ap);

  return Tcl_DStringAppendElement( dsP, itfBuffer);
}

/* ========================================================================
    C-impelementation of shell commands for reducing the need for forks
   ======================================================================== */
/*
static int returnSysError(char *command) {

  char *err = NULL;
 
  if ( (err = strerror (errno)) )
    ERROR("<ITF,SHC> %s: %s\n",            command,err);
  else
    ERROR("<ITF,SHC>%s: undefined error\n",command);
 
  MSGPRINT(NULL);
  return TCL_ERROR;
}
*/

/* ========================================================================
    Argument Parsing
   ======================================================================== */
/* ------------------------------------------------------------------------
    itfParseArgv
   ------------------------------------------------------------------------ */

static void itfPrintUsage( char* method, ArgvInfo* argTable);

int itfParseArgv( char* method, int* argcPtr, char* argv[], int leftOver, ...)
{
  ArgvInfo  argTable[200];

  ArgvInfo* infoPtr =  argTable;
  int       argc    = *argcPtr;
  int       prmIdx  =  0;
  int       srcIdx  =  0;
  int       dstIdx  =  0;
  char*     curArgv;
  int       len;
  va_list   ap;
  char*     str;

  /* fill the argument table */

  va_start(ap,leftOver);
  while ( (str = va_arg(ap,char *)) ) {

    infoPtr->key     = str;
    infoPtr->type    = va_arg(ap,ArgvType);
    infoPtr->handler = va_arg(ap,ArgvHandler*);
    infoPtr->dst     = va_arg(ap,ClientData);
    infoPtr->cd      = va_arg(ap,ClientData);
    infoPtr->help    = va_arg(ap,char*);
    infoPtr++;
  }
  infoPtr->key  = NULL;
  infoPtr->type = ARGV_END;
  va_end(ap);

  /* parse the fixed parameters */

  while ( argc > 0) {
    ArgvInfo* matchPtr = NULL;

    if ((curArgv = argv[srcIdx]) == NULL) break;
    len      = strlen(curArgv);

    if (! strcmp(curArgv,"-help")) {
      itfPrintUsage(method,argTable);

      if ( argTable[prmIdx].key && argTable[prmIdx].key[0] != '-')
           return TCL_ERROR;
      else return TCL_ERROR; /* was originally TCL_OK */
    }
    else if ( argTable[prmIdx].key && argTable[prmIdx].key[0] != '-') {

      matchPtr = argTable + prmIdx; 
      prmIdx++;
    }
    else {
      char c  = curArgv[1];
      infoPtr = argTable + prmIdx;

      argc--; srcIdx++;

      for ( ; infoPtr->key; infoPtr++) {
        if ((infoPtr->key[1] != c) || strncmp( infoPtr->key, curArgv, len))
          continue;
        if (infoPtr->key[len] == '\0') {
          matchPtr = infoPtr;
          goto gotMatch;
        }
        if ( matchPtr != NULL) {
          SERROR("<ITF,AO>%s: ambiguous option \"%s\".\n", method, curArgv);
          return TCL_ERROR;
        }
        matchPtr = infoPtr;
      }
    }
    if ( matchPtr == NULL) {

      if (! leftOver) {

        SERROR("<ITF,UA>%s: unrecognized argument \"%s\".\n", method, curArgv);
        return TCL_ERROR;
      }
      if ( leftOver > 1) {
        argv[dstIdx] = curArgv;
        dstIdx++;
      }
      srcIdx++;
      continue;
    }

    gotMatch:

    infoPtr = matchPtr;
    switch ( infoPtr->type) {

    case ARGV_SHORT :

      if ( argc == 0) { goto missingArg; }
      else {
        char* endPtr;
        *((short*)infoPtr->dst) = (short)strtol( argv[srcIdx], &endPtr, 0);
        if ( endPtr == argv[srcIdx] || *endPtr) {
          SERROR("<ITF,ESH>%s: expected short argument for %s but got \"%s\".\n",
                 method, infoPtr->key, argv[srcIdx]);
          return TCL_ERROR;
        }
        srcIdx++;
        argc--;
      }
      break;

    case ARGV_LONG :

      if ( argc == 0) { goto missingArg; }
      else {
        char* endPtr;
        *((long*)infoPtr->dst) = strtol( argv[srcIdx], &endPtr, 0);
        if ( endPtr == argv[srcIdx] || *endPtr) {
          SERROR("<ITF,EIN>%s: expected long argument for %s but got \"%s\".\n",
                 method, infoPtr->key, argv[srcIdx]);
          return TCL_ERROR;
        }
        srcIdx++;
        argc--;
      }
      break;

    case ARGV_INT :

      if ( argc == 0) { goto missingArg; }
      else {
        char* endPtr;
        *((int*)infoPtr->dst) = strtol( argv[srcIdx], &endPtr, 0);
        if ( endPtr == argv[srcIdx] || *endPtr) {
          SERROR("<ITF,EIN>%s: expected integer argument for %s but got \"%s\".\n",
                 method, infoPtr->key, argv[srcIdx]);
          return TCL_ERROR;
        }
        srcIdx++;
        argc--;
      }
      break;

    case ARGV_IARRAY :

      if ( argc == 0) { goto missingArg; }
      else {
        if ( iarrayGetItf((IArray*)infoPtr->dst, argv[srcIdx]) != TCL_OK)
             return TCL_ERROR;
        srcIdx++;
        argc--;
      }
      break;

    case ARGV_FARRAY :

      if ( argc == 0) { goto missingArg; }
      else {
        if ( farrayGetItf((FArray*)infoPtr->dst, argv[srcIdx]) != TCL_OK)
             return TCL_ERROR;
        srcIdx++;
        argc--;
      }
      break;

    case ARGV_LIARRAY :

      if ( argc == 0) { goto missingArg; }
      else {
        if ( iarrayGetListItf((IArray*)infoPtr->dst, argv[srcIdx],
                              (List  *)infoPtr->cd) != TCL_OK) {
          SERROR("<ITF,CRLIA>Can't read list index array for %s.\n", infoPtr->key);
          return TCL_ERROR;
        }
        srcIdx++;
        argc--;
      }
      break;

    case ARGV_MIARRAY :

      if ( argc < ((IArray*)infoPtr->dst)->itemN ) { goto missingArg; }
      else {
	int i;
	IArray * warray = (IArray*)infoPtr->dst;
	List   * L      = (List*) infoPtr->cd;
	if(warray->itemA==0)	
	  if(NULL==(warray->itemA = calloc(warray->itemN,sizeof(int)))) FATAL("Not enough memory\n");

	for(i=0;i<warray->itemN;i++) {
      assert (warray->itemA);
	  warray->itemA[i]=listIndex( L, (ClientData)argv[srcIdx], 0);
	  if(warray->itemA[i]<0) {
	    SERROR("Can't find '%s' in the list.\n", argv[srcIdx]);
	    return TCL_ERROR;
	  }
	  srcIdx++;
	  argc--;
	}
      }
      break;

    case ARGV_FLOAT :

      if ( argc == 0) { goto missingArg; }
      else {
        char* endPtr;
        *((float*)infoPtr->dst) = strtod( argv[srcIdx], &endPtr);
        if ( endPtr == argv[srcIdx] || *endPtr) {
          SERROR("<ITF,EXFL>%s: expected float argument for %s but got \"%s\".\n",
                 method, infoPtr->key, argv[srcIdx]);
          return TCL_ERROR;
        }
        srcIdx++;
        argc--;
      }
      break;

    case ARGV_DOUBLE :

      if ( argc == 0) { goto missingArg; }
      else {
        char* endPtr;
        *((double*)infoPtr->dst) = strtod( argv[srcIdx], &endPtr);
        if ( endPtr == argv[srcIdx] || *endPtr) {
          SERROR("<ITF,EXDB>%s: expected double argument for %s but got \"%s\".\n",
                  method,infoPtr->key, argv[srcIdx]);
          return TCL_ERROR;
        }
        srcIdx++;
        argc--;
      }
      break;

    case ARGV_STRING :

      if ( argc == 0) { goto missingArg; }
      else {
	if (argv[srcIdx][0]=='@') 
	  {
	    *((char**)infoPtr->dst) = itfReadFile(argv[srcIdx]+1); 

	    /* ------------------------------------------------------ */
	    /* add *((char**)infoPtr->dst) to garbage collection list */
	    /* to be freed, when argv is freed (between commands), or */
	    /* do this:      free(argv[srcIdx]);                      */
	    /*     and:      argv[srcIdx] = *((char**)infoPtr->dst);  */
	    /* No, I tried the latter version, won't do ...           */
	    /* ------------------------------------------------------ */

            if (! *((char**)infoPtr->dst)) 
                  *((char**)infoPtr->dst) = argv[srcIdx];
	  }
	else                      
	  *((char**)infoPtr->dst) = argv[srcIdx]; 
        srcIdx++;
        argc--;
      }
      break;

    case ARGV_OBJECT :

      if ( argc == 0) { goto missingArg; }
      else {
        ClientData objPtr = itfGetObject( argv[srcIdx],(char*)infoPtr->cd);
        if (! objPtr) return TCL_ERROR;
        *((ClientData*)infoPtr->dst) = objPtr; 
        srcIdx++;
        argc--;
      }
      break;

    case ARGV_LINDEX :

      if ( argc == 0) { goto missingArg; }
      else { 
        int ac  = argc;
        int idx = listIndexItf(infoPtr->cd, infoPtr->key, infoPtr->dst, 
                              &ac, argv+srcIdx);

        if ( idx < 0) return TCL_ERROR;
        srcIdx += argc - ac;
        argc    = ac;
      }
      break;

    case ARGV_CUSTOM :
      {
      int ac = argc;

      if (  infoPtr->handler( infoPtr->cd, infoPtr->key, infoPtr->dst, 
                              &ac, argv+srcIdx)) {
        return TCL_ERROR;
      }
      srcIdx += argc - ac;
      argc    = ac;
      }
      break;

    case ARGV_REST :

      *((int*)infoPtr->dst) = dstIdx;
      goto argsDone;

    default:

      SERROR("<ITF,BAT>%s: bad argument type %d in ArgvInfo.\n", 
              method, infoPtr->type);
      return TCL_ERROR;
    }
  }

  argsDone:

  if ( argTable[prmIdx].key && argTable[prmIdx].key[0] != '-') {

    SERROR( "<ITF,MA>%s: missing argument \"%s\".\n", method, 
             argTable[prmIdx].key);
    return TCL_ERROR;
  }

  if ( leftOver < 2) {

    while ( argc>0) {
      argv[dstIdx++] = argv[srcIdx++];
      argc--;
    }
    argv[dstIdx] = (char*)NULL;
    *argcPtr     =  dstIdx++;
  }
  return TCL_OK;

  missingArg:

  SERROR("<ITF,ORAA>option \"%s\" requires an additional argument.\n", curArgv);
  return TCL_ERROR;
}

static void itfPrintUsage( char* method, ArgvInfo* argTable)
{
  ArgvInfo*   infoPtr  = argTable;
  static char spaces[] = "                    ";
  int         width    = 4;

#define NUM_SPACES 20

  for ( ; infoPtr->key; infoPtr++) {
    int len  = strlen( infoPtr->key);
    if ( len > width) width = len;
  }

  itfAppendResult("Options of \'%s\' are:", method);

  for ( infoPtr = argTable; infoPtr->key; infoPtr++) {
    int numSpaces;

    itfAppendResult("\n %s", infoPtr->key);
    numSpaces = width + 1 - strlen(infoPtr->key);
    while ( numSpaces > 0) {

      itfAppendResult( ( numSpaces >= NUM_SPACES) ? spaces :
                       ( spaces + NUM_SPACES - numSpaces));
      numSpaces -= NUM_SPACES;
    }
    itfAppendResult(" %s", infoPtr->help);

    switch ( infoPtr->type) {
    case ARGV_INT :
      itfAppendResult(" (int:%d)", *((int*)infoPtr->dst));
      break;
    case ARGV_FLOAT :
      itfAppendResult(" (float:%f)", *((float*)infoPtr->dst));
      break;
    case ARGV_DOUBLE :
      itfAppendResult(" (double:%lf)", *((double*)infoPtr->dst));
      break;
    case ARGV_STRING :
      itfAppendResult(" (string:\"%s\")", 
         *((char**)infoPtr->dst) ? *((char**)infoPtr->dst) : "NULL");
      break;
    case ARGV_OBJECT :
      if (infoPtr->cd) itfAppendResult(" (%s:)", (char*)infoPtr->cd);
      break;
    default :
      break;
    }
  }
}

/* ------------------------------------------------------------------------
    itfAppendResult
   ------------------------------------------------------------------------ */

void itfAppendResult( char *format, ... )
{
  va_list  ap;
  int n;

  va_start(ap,format);
  n = vsprintf( itfBuffer, format, ap);
  assert (itfBufferSize > n);
  va_end(ap);

  Tcl_AppendResult( itf, itfBuffer, NULL);
}

void itfAppendElement( char *format, ... )
{
  va_list  ap;
  int n;

  va_start(ap,format);
  n = vsprintf( itfBuffer, format, ap);  
  assert (itfBufferSize > n);
  va_end(ap);

  Tcl_AppendElement( itf, itfBuffer);
}

void itfResetResult()
{
  Tcl_ResetResult (itf);
}

static int itfMakePath(char *name, int mode)
{

  char   *p = name;
  char    c;
  size_t  i;

#ifndef WINDOWS
  char sep[] = "/";

  /* Florian: 
     we're seeing 'mkdir: Permission denied' on accessible dirs
     avoid these cases? (this is crappy code ...) */
  if (!access (p, F_OK))
    return TCL_OK;
  {
#include <sys/types.h>
#include <dirent.h>
    DIR* p = opendir (name);
    if (p) {
      closedir (p);
      return TCL_OK;
    }
    //closedir (p);
  }

#else
  char sep[] = "/\\";
#endif

  i = strcspn (p, sep);
  c = p[i]; p[i] = '\0';

  while ( c != '\0' ) {
#ifndef WINDOWS
    if ( i && mkdir (name, mode) != 0 ) {
#else
    if ( i && mkdir (name)       != 0 ) {
#endif
      if ( errno != EEXIST ) {
	INFO ("itfMakePath: '%s' could not be created(?)", name);
	// return returnSysError("mkdir");
      }
    }

    p[i] = c; p += i + 1;

    i = strcspn (p, sep);
    c = p[i]; p[i] = '\0';
  }

  p[i] = c; p += i + 1;

  return TCL_OK;
}


/* ------------------------------------------------------------------------
    printO/E
   ------------------------------------------------------------------------ */

void printO( char *format, ... )
{
  va_list  ap;

  va_start(ap,format);
  vfprintf( STDOUT, format, ap);
  va_end(ap);
}

void printE( char *format, ... )
{
  va_list  ap;

  va_start(ap,format);
  vfprintf( STDERR, format, ap);
  va_end(ap);
}

/* ========================================================================
    Timer
   ======================================================================== */

char *dateString() { time_t t=time(0); return (ctime(&t)); }

#ifndef WINDOWS
void timerReset( Timer* timerP)
{
  timerP->start.tv_sec = 0L;
  timerP->start.tv_usec= 0L;
  timerP->stop.tv_sec  = 0L;
  timerP->stop.tv_usec = 0L;
  timerP->delta        = 0.0;
}

void timerStart( Timer* timerP)
{
  struct timeval   * tp = &(timerP->start);
  struct timezone  * tz =   NULL;

  gettimeofday(tp, tz);
}

float timerStop( Timer* timerP)
{
  const long       one_million = 1000000L;
  long             s, us;

  struct timeval   * tp = &(timerP->stop);
  struct timezone  * tz =   NULL;

  gettimeofday(tp, tz);

  s  = (timerP->stop.tv_sec)  - (timerP->start.tv_sec);
  us = (timerP->stop.tv_usec) - (timerP->start.tv_usec);

  while (us < 0L) { us += one_million; s  -= 1L; }
  timerP->delta += ((float)s + ((float)us / 1E6) );   

  return timerP->delta;
}
#else /* WINDOWS */
void timerReset( Timer* timerP) {

	timerP->start = timerP->stop = (clock_t)0;
	timerP->delta = 0.0;

}

void timerStart( Timer* timerP) {

	timerP->start = clock();
}

float timerStop( Timer* timerP) {

	timerP->stop   = clock();
	timerP->delta += (timerP->stop - timerP->start) / (float) CLK_TCK;

  return timerP->delta;
}
#endif /* WINDOWS */


/* ========================================================================
    File Utilities
   ======================================================================== */

#include <fcntl.h>
#ifdef WINDOWS
#include <sys/locking.h>
#include <share.h>
#endif

/* ------------------------------------------------------------------------
    fileLock      opens a file handle and locks it for writing
   ------------------------------------------------------------------------ */

#ifndef WINDOWS
FILE* fileLock (char *name, char *mode) {

  struct flock  lock;
  FILE         *fp  = NULL;
  int           ret = -1;
  char         *p;
  char          m[10];

  lock.l_type   = F_WRLCK;
  lock.l_whence = SEEK_SET;
  lock.l_start  = 0;
  lock.l_len    = 0;

  if ( strchr (mode, 'b') ) {
    strcpy (m, mode);
  } else if ( (p = strchr (mode, 't')) ) {
    int i = mode - p + 1;
    strcpy (m, mode);
    while ( mode[i] != '\0' ) {
      m[i-1] = mode[i];
      i++;
    }
    m[i-1] = '\0';
  } else {
    sprintf (m, "%sb", mode);
  }

  while ( ret < 0 ) {
    if ( !(fp = fopen (name, m)) ) {
      ERROR("<ITF,COF>Can't open file %s.\n",name);
      return NULL;
    }

    if ( (ret = fcntl (fileno(fp), F_SETLK, &lock)) < 0 ) {
      fclose(fp);
      sleep(1);
    }
  }

  return (fp);
}

#else

FILE* fileLock (char *name, char *mode) {

  FILE *fp  = NULL;
  int   ret = -1;
  char  m[10];

  if ( strchr (mode, 'b') ) {
    strcpy (m, mode);
  } else if ( strchr (mode, 't') ) {
    strcpy (m, mode);
  } else {
    sprintf (m, "%sb", mode);
  }

  while ( ret < 0 ) {
    if ( !(fp = _fsopen (name, m, _SH_DENYNO)) ) {
      ERROR("<ITF,COF>Can't open file %s.\n",name);
      return NULL;
    }

    if ( (ret = _locking (fileno(fp), _LK_NBLCK, 1L)) < 0 ) {
      fclose(fp);
      sleep(1);
    }
  }

  return (fp);
}
#endif

/* ------------------------------------------------------------------------
    fileUnlock   removes the write lock and closes the file
   ------------------------------------------------------------------------ */

#ifndef WINDOWS
int fileUnlock (FILE *fp) {

  struct flock lock;

  lock.l_type   = F_UNLCK;
  lock.l_whence = SEEK_SET;
  lock.l_start  = 0;
  lock.l_len    = 0;

  if ( fcntl (fileno(fp), F_SETLK, &lock) < 0 ) {
    WARN("<ITF,CUL>Could not unlock file.\n");
    return TCL_ERROR;
  }

  fclose (fp);
  return TCL_OK;
}

#else

int fileUnlock (FILE *fp) {

  fseek (fp, 0L, SEEK_SET);
  if ( _locking (fileno(fp), _LK_UNLCK, 1L) < 0 ) {
    WARN("<ITF,CUL>Could not unlock file.\n");
    return TCL_ERROR;
  }

  fclose (fp);
  return TCL_OK;
}
#endif

/* ------------------------------------------------------------------------
    fgets    get the next string from a file using a lock to make sure
             that only one process at a time has read/write access on the
             file. The line read is marked by replacing the first character
             by a comment character #
   ------------------------------------------------------------------------ */
//#define EXT_FGETS
//#define FLOCK_NO

#ifdef EXT_FGETS
static int fgets_posixA (char* name, char* line) {
  /* 
     NFS-clean implementation of fgets without fileLock, compliant with POSIX:
     We should not use functions from stdio.h in locked portions of a file
     This seems to be much better at CMU, but is still not perfect - 
     I am afraid, this is due to NFS.
   */
  struct flock  lock;
  FILE           *fp = NULL;
  int            ret = -1;
  char    *p, *q, *r = NULL;
  int         length = 0;
  char         *mode = "r+b";
  int             fd, offset;
  struct stat  stbuf;

  /* Open and lock the file */
  if (!(fp = fopen (name, mode))) {
    ERROR ("<ITF,COF>Can't open file %s.\n", name);
    return TCL_ERROR;
  }
  stat (name, &stbuf);
  length = stbuf.st_size;
  fd     = fileno (fp);

  /* Now, let's map the file to memory - no exclusive access yet */
  p = q = (char*) mmap (0, length, PROT_READ, MAP_SHARED, fd, 0);
  r = p+length;

  /* Let's find the first empty line */
  while   (q < r && *q == '#') {
    while (q < r && *q != '\n') 
      q++;
    q++;
  }
  munmap ((void*) p, length);
  if (q >= r) { /* we found no new key */
    strcpy (line, "");
    fclose (fp);
    return (TCL_ERROR);
  }
  offset = q-p;

  /* now we need locking */
  lock.l_type   = F_WRLCK;
  lock.l_whence = SEEK_SET;
  lock.l_start  = 0;
  lock.l_len    = 0;

  /* No file locking on Intel */
#ifdef FLOCK_NO
  ret = 0;
#else
  while ((ret = fcntl (fd, F_SETLK, &lock)) == -1);
#endif
  stat (name, &stbuf);
  length = stbuf.st_size;

  /* Now, let's map the file to memory - exclusive access */
  p = (char*) mmap (0, length, PROT_WRITE|PROT_READ, MAP_SHARED, fd, 0);
  r = p+length;
  q = p+offset;

  /* Let's find the first empty line - after locking */
  while   (q < r && *q == '#') {
    while (q < r && *q != '\n') 
      q++;
    q++;
  }
  if (q < r) { /* we found a new key */
    strncpy (line, q, (size_t) (strchr (q, '\n')-q));
    line[(size_t) (strchr (q, '\n')-q)] = 0;
    ret = TCL_OK;
    *q = '#';
  } else { /* no new keys */
    strcpy (line, "");
    ret = TCL_ERROR;
  }

  /* Sync the map, release the lock, and close */
  msync (p, length, MS_SYNC);
  munmap ((void*) p, length);
  lock.l_type   = F_UNLCK;
#ifndef FLOCK_NO
  if (fcntl (fd, F_SETLK, &lock) == -1)
    WARN ("<ITF,CUL>Could not unlock file %s.\n", name);
#endif
  fclose (fp);

  /* remove '\n' */
  length = strlen (line);
  if (line[length-1] == '\n') line[--length] = '\0';
  if (line[length-1] == '\r') line[--length] = '\0';

  return (ret);
}

struct flock *posix_lock = NULL;
char         posix_name[1024];
FILE*        posix_fp  = NULL;
char*        posix_map = NULL;
int          posix_len = 0;

static int fgets_posixB (char* name, char* line) {
  /* 
     NFS-clean implementation of fgets without fileLock, compliant with POSIX:
     We should not use functions from stdio.h in locked portions of a file
     This version locks the first character of a key while it processes it
     It only commits a key (with #) as soon as we start processing a new key
   */
  char    *p, *q, *r = NULL;
  int         length = 0;
  char         *mode = "r+b";

  if (!posix_lock) {
    posix_lock = malloc (sizeof (struct flock));
    posix_lock->l_type   = F_UNLCK;
    posix_lock->l_whence = SEEK_SET;
    posix_lock->l_start  = 0;
    posix_lock->l_len    = 1;
  }

  if (posix_lock->l_type == F_WRLCK) {
    /* Let's make an existing lock permanent */
    posix_map[posix_lock->l_start] = '#';
    msync (posix_map+posix_lock->l_start, 1, MS_SYNC);
    posix_lock->l_type   = F_UNLCK;
    
    if (fcntl (fileno (posix_fp), F_SETLK, posix_lock) < 0) {
      WARN ("<ITF,CUL>Could not unlock file '%s'.\n", posix_name);
    }
  }

  if (posix_fp && strcmp (name, posix_name)) {
    /* We need to close the old file */
    msync (posix_map, posix_len, MS_SYNC);
    munmap ((void*) posix_map, posix_len);
    fclose (posix_fp);
    posix_len = 0;
    posix_map = NULL;
    posix_fp  = NULL;
  }

  if (!posix_fp) {
    /* Open the file */
    if (!(posix_fp = fopen (name, mode))) {
      ERROR ("<ITF,COF>Can't open file %s.\n", name);
      return TCL_ERROR;
    }

    fseek (posix_fp, 0, SEEK_END);
    posix_len = ftell (posix_fp);
    posix_map = (char*) mmap (0, posix_len, PROT_WRITE|PROT_READ, MAP_SHARED, fileno (posix_fp), 0);
    strncpy (posix_name, name, 1000);
  }

  q = p = posix_map;
  r = posix_map+posix_len;

  /* Let's test the file */
  posix_lock->l_type   = F_WRLCK;
  while (q < r) {
    if (*q == '#') {
      /* This key has already been processed */
      while (q < r && *q != '\n') q++;
      q++;
      continue;
    }

    posix_lock->l_start  = q-p;
    if (fcntl (fileno (posix_fp), F_SETLK, posix_lock) < 0) {
      /* Couldn't lock this key */
      while (q < r && *q != '\n') q++;
      q++;
      continue;
    }

    /* This is our key */
    break;
  }

  if (q >= r) { /* No key could be found */
    posix_lock->l_type = F_UNLCK;
    strcpy (line, "");
    munmap ((void*) posix_map, posix_len);
    fclose (posix_fp);
    posix_len = 0;
    posix_map = NULL;
    posix_fp  = NULL;
    return TCL_ERROR;
  }

  strncpy (line, q, (size_t) (strchr (q, '\n')-q));
  line[(size_t) (strchr (q, '\n')-q)] = 0;

  /* remove '\n' */
  length = strlen (line);
  if (line[length-1] == '\n') line[--length] = '\0';
  if (line[length-1] == '\r') line[--length] = '\0';

  return TCL_OK;
}
#endif

static int fgetsItf( ClientData cd, Tcl_Interp *interp, int argc, char *argv[]) {

  char  *name   = NULL;
  char  *var    = NULL;
  char  *log    = NULL;
  int    ac     = argc-1;
#ifndef EXT_FGETS
  FILE  *fp     = NULL;
  char   c;
  int    pos2;
#endif
  int    tstamp = 0;
  int    rc     = TCL_OK;
  char   line[fgets_maxline];
#ifdef WINDOWS
  fpos_t pos;
#endif
  Tcl_Obj* ex;

  if ( itfParseArgv(argv[0],&ac,argv+1,1,
      "<fileName>",  ARGV_STRING,NULL, &name,  NULL, "filename to read from",
      "<varName>",   ARGV_STRING,NULL, &var,   NULL, "name of the variable",
      "-log",        ARGV_STRING,NULL, &log,   NULL, "log file",
      "-timeStamp",  ARGV_INT   ,NULL, &tstamp,NULL, "include timestamp in log",
       NULL) != TCL_OK) return TCL_ERROR;

  if ((ex = Tcl_GetVar2Ex (itf, "fgetsEXHAUST", NULL, TCL_GLOBAL_ONLY)) && atoi (ex->bytes)) {
    INFO ("fgetsItf: fgetsEXHAUST == 1 -- finishing up.\n"); /* Florian */
    itfAppendResult ("%d", -1);
    return rc;
  }

#ifdef EXT_FGETS /* use an external routine */

  if (fgets_posixA (name, line) == TCL_ERROR) {
    if (!Tcl_SetVar (itf, var, line, 0)) 
      rc = TCL_ERROR;
    else 
      itfAppendResult ("%d", -1);
  } else if (!Tcl_SetVar (itf, var, line, 0)) 
    rc = TCL_ERROR;
  else 
    itfAppendResult ("%d", strlen (line));

#else /* EXT_FGETS */

  /* open and lock a stream */
  if ( !(fp = fileLock (name, "r+")) )
    return TCL_ERROR;

#ifdef WINDOWS
  /* Windows-specific code */
  fgetpos (fp, &pos);
  fgets (line, fgets_maxline, fp);
  c = line[0];
  while ( c == '#' && !feof (fp) ) {
    fgetpos (fp, &pos);
    fgets (line, fgets_maxline, fp);
    c = line[0];
  }

  if ( !feof (fp) ) {
    int n;

    /* when switching between read and write on a stream, there must
       be an intervening fsetpos, fseek or rewind operation (WINDOWS)
       no use of fflush, because it sets the file pointer to end
       of stream (WINDOWS) */
    fsetpos (fp, &pos);
    putc ('#', fp);

#else
  /* on UNIX, we need to avoid the use of buffered I/O functions 
     in the locked part of the file -- the current implementation is
     not very efficient, and is also unreliable, it seems */
  pos2 = read (fileno (fp), line, fgets_maxline);
  c = line[0];
  while ( c == '#' && !feof (fp) && pos2) {
    int n = (int) ((strchr (line, 10) < strchr (line, 13) ? strchr (line, 13) : strchr (line, 10)) - line);
    if ((int) lseek (fileno (fp), n+1-pos2, SEEK_CUR) == -1) {
      pos2 = 0;
      break;
    }
    pos2 = read (fileno (fp), line, fgets_maxline);
    c = line[0];
  }

  if ( !feof (fp) && pos2 ) {
    int n;

    c = '#';
    lseek (fileno (fp), -pos2, SEEK_CUR);
    write (fileno (fp), &c, 1);
    fsync (fileno (fp));
    n = (int) ((strchr (line, '\n') < strchr (line, '\r') ? strchr (line, '\r') : strchr (line, '\n')) - line);
    line[n] = '\0';

#endif /* Windows or Unix specific part ends */

    /* remove '\n' */
    n = strlen (line);
    if ( line[n-1] == '\n' ) line[--n] = '\0';
    if ( line[n-1] == '\r' ) line[--n] = '\0';

    if ( !Tcl_SetVar( itf, var, line, 0) ) rc = TCL_ERROR;
    else itfAppendResult ("%d", n);

    if ( log && rc == TCL_OK ) {
      FILE *fq;

      if ( !(fq = fileLock (log, "a+")) ) rc = TCL_ERROR;
      else {
	char *d = dateString ();
	char  hname[256];

	gethostname (hname, 256);

	d[strlen(d)-1] = 0;
	if ( tstamp )
	  fprintf (fq, "[%-16s %05d %s] %s\n", hname, getpid(), d, line);
	else
	  fprintf (fq, "[%-16s %05d] %s\n",    hname, getpid(), line);

	fflush (fq);

	if ( fileUnlock (fq) != TCL_OK ) rc = TCL_ERROR;
      }
    }
  } else {
    itfAppendResult ("%d", -1);
  }

  /* close and unlock a locked stream */
  if ( fileUnlock (fp) != TCL_OK ) rc = TCL_ERROR;

#endif /* FGETS_EXT ends */

  MSGPRINT (NULL);
  return rc;
}

/* ------------------------------------------------------------------------
    fputs    opposite of fgets
   ------------------------------------------------------------------------ */
static int fputsItf( ClientData cd, Tcl_Interp *interp, int argc, char *argv[]) {

  char  *name   = NULL;
  char  *var    = NULL;
  int    ac     = argc-1;
  FILE  *fp     = NULL;
  int    rc     = TCL_OK;
  int    append = 0;
  char   line[1024];
  const char *c;
  fpos_t pos;

  if ( itfParseArgv(argv[0],&ac,argv+1,1,
      "<fileName>",  ARGV_STRING, NULL, &name,   NULL, "filename to write to",
      "<varName>",   ARGV_STRING, NULL, &var,    NULL, "name of the variable",
      "-append",     ARGV_INT,    NULL, &append, NULL, "append value to file",
       NULL) != TCL_OK) return TCL_ERROR;

  /* Get the variable */
  if (!(c = Tcl_GetVar (itf, var, 0))) {
    INFO ("Tcl variable %s undefined.\n", var);
    return TCL_ERROR;
  }

  if ( append ) {
    /* shortcut for this case */
    if ( !(fp = fileLock (name, "a")) )
      return TCL_ERROR;

    fseek (fp, 0L, SEEK_END);
    /* FMe: we want to avoid the use of stdio.h functions here
    fputs (c, fp);
    fputc ('\n', fp);
    */
    sprintf (line, "%s\n", c);
    write (fileno (fp), line, strlen (line));

    /* close and unlock a locked stream */
    if ( fileUnlock (fp) != TCL_OK ) 
      rc = TCL_ERROR;
    
    return rc;    
  }
  INFO ("fputs without -append may be unsafe on NFS.\n"); /* Florian */

  /* open and lock a stream */
  if ( !(fp = fileLock (name, "r+")) )
    return TCL_ERROR;

  /* Search for it */
  fgetpos (fp, &pos);
  fgets (line, 1024, fp);
  if (strchr (line, '\n')) *(strchr (line, '\n')) = 0;
  while ( strcmp (c+1, line+1) != 0 && !feof (fp) ) {
    fgetpos (fp, &pos);
    fgets (line, 1024, fp);
    if (strchr (line, '\n')) *(strchr (line, '\n')) = 0;
  }
  
  if ( !feof (fp) ) {
    /* when switching between read and write on a stream, there must
       be an intervening fsetpos, fseek or rewind operation (WINDOWS)
       no use of fflush, because it sets the file pointer to end
       of stream (WINDOWS) */
    fsetpos (fp, &pos);
    fputs (c, fp);
    
    strcpy (line, "");
    if ( !Tcl_SetVar( itf, var, line, 0) ) rc = TCL_ERROR;
    else itfAppendResult ("%d", strlen (c));
    
  } else {
    rc = TCL_ERROR;
    itfAppendResult ("%d", -1);
  }

  /* close and unlock a locked stream */
  if ( fileUnlock (fp) != TCL_OK ) 
    rc = TCL_ERROR;

  MSGPRINT (NULL);
  return rc;
}

/* ------------------------------------------------------------------------
    fileOpen   open a file for reading/writing
   ------------------------------------------------------------------------ */

FILE* fileOpen(char* fileName, char* mode) {
  int   retry = 3;
  int   count = 0;
  int   l     = strlen(fileName);
  int   pipe  = 0;
  char  m[10];
  char *p;
  FILE *fp;
#ifdef WINDOWS
  char *nullDev = "2>NUL:";
#else
  char *nullDev = "2>/dev/null";
#endif

  if ((!strncmp (fileName, "scp:", 4) || !strncmp (fileName, "ssh:", 4)) && 
      strchr (mode, 'r')) {
    /* Use scp to get the data from a remote machine,
       alternative syntax */
    char*  host = fileName+4;
    char* dfile = strchr (host, ':');
    char line[1024];
    
    if (!dfile) {
      ERROR ("<ITF,COF> fileOpen '%s' parse '%s' failed.\n", fileName, host);
      return NULL;
    }

    *dfile++ = 0;
    if (!strcmp (dfile+strlen(dfile)-3, ".gz"))
      sprintf (line, "ssh -nc  blowfish %s \"cat %s\" | gzip -d", host, dfile);
    else
      sprintf (line, "ssh -nCc blowfish %s \"cat %s\"", host, dfile);
    *--dfile = ':';

    INFO ("fileOpen: '%s'\n", line);
    if (!(fp = popen (line, "r"))) {
      ERROR ("<ITF,COF> fileOpen '%s' '%s' failed.\n", fileName, "r");
      return NULL;
    }

    return fp;
  }

  if ((!strncmp (fileName, "scp:", 4) || !strncmp (fileName, "ssh:", 4)) && 
      strchr (mode, 'w')) {
    /* Use scp to get the data to a remote machine,
       alternative syntax */
    char*  host = fileName+4;
    char* dfile = strchr (host, ':');
    char line[1024];
    
    if (!dfile) {
      ERROR ("<ITF,COF> fileOpen '%s' parse '%s' failed.\n", fileName, host);
      return NULL;
    }

    *dfile++ = 0;
    if (!strcmp (dfile+strlen(dfile)-3, ".gz"))
      sprintf (line, "gzip | ssh -nc  blowfish %s \"cat > %s\"", host, dfile);
    else
      sprintf (line,        "ssh -nCc blowfish %s \"cat > %s\"", host, dfile);

    INFO ("fileOpen: '%s'\n", line);
    if (!(fp = popen (line, "w"))) {
      ERROR ("<ITF,COF> fileOpen '%s' '%s' failed.\n", fileName, "r");
      return NULL;
    }

    return fp;
  }

  /* Some checks */
  if      (strchr(mode,'w')) 
    itfMakePath(fileName,0755);
  else if (strchr(mode,'r')) {
    struct stat buf;
    if (stat (fileName, &buf) == -1) {
      ERROR ("<ITF,COF> fileOpen '%s' mode '%s' failed: '%s'\n", fileName, mode, strerror(errno));
      return NULL;
    }
  }

  if        (! strcmp( fileName + l - 2, ".Z")) {
    if      ( strchr( mode, 'r')) sprintf(itfBuffer,"zcat       %s %s", fileName, nullDev);
    else if ( strchr( mode, 'w')) sprintf(itfBuffer,"compress > %s",    fileName);
    else {
      ERROR("<ITF,COF> Can't popen %s using mode %s.\n", fileName, mode);
      return NULL;
    }
    pipe = 1;

  } else if (! strcmp( fileName + l - 3, ".gz")) {
    if      ( strchr( mode, 'r')) sprintf(itfBuffer,"gunzip -c    %s %s", fileName, nullDev);
    else if ( strchr( mode, 'w')) sprintf(itfBuffer,"gzip   -c >  %s",    fileName);
    else if ( strchr( mode, 'a')) sprintf(itfBuffer,"gzip   -c >> %s",    fileName);
    else {
      ERROR("<ITF,COF> Can't popen %s using mode %s.\n", fileName, mode);
      return NULL;
    }
    pipe = 1;

  } else if (! strcmp( fileName + l - 4, ".bz2")) {
    if      ( strchr( mode, 'r')) sprintf(itfBuffer,"bzip2 -cd    %s %s", fileName, nullDev);
    else if ( strchr( mode, 'w')) sprintf(itfBuffer,"bzip2 -cz >  %s",    fileName);
    else if ( strchr( mode, 'a')) sprintf(itfBuffer,"bzip2 -cz >> %s",    fileName);
    else {
      ERROR("<ITF,COF> Can't popen %s using mode %s.\n", fileName, mode);
      return NULL;
    }
    pipe = 1;
  }

  /* WINDOWS: append 'b' for binary if it does not exists and
              if no 't' for text mode is specified
     LINUX:   'b' for binary mode doesn't matter, but 't' for
              text mode has to be removed */
  if ( strchr (mode, 'b') ) {
    strcpy (m, mode);
  } else if ( (p = strchr (mode, 't')) ) {
#ifdef WINDOWS
    strcpy (m, mode);
#else
    int i = p - mode + 1;
    strcpy (m, mode);
    while ( mode[i] != '\0' ) {
      m[i-1] = mode[i];
      i++;
    }
    m[i-1] = '\0';
#endif
  } else {
    if ( pipe ) {
#ifdef WINDOWS
      sprintf (m, "%sb", mode);
#else
      sprintf (m, "%s", mode);
#endif
    } else
      sprintf (m, "%sb", mode);
  }

  while (count <= retry) {
    if (! (fp = ( pipe) ? popen( itfBuffer, m) :
                          fopen( fileName,  m))) {
      sleep(60); count++;
      WARN("<ITF,COF> fileOpen '%s' '%s' failed: '%s'. %d attempts remain.\n",
           fileName, m, strerror(errno), retry - count + 1);
    }
    else break;
  }

  if ( count > retry) {
    ERROR("<ITF,COF> fileOpen '%s' '%s' failed.\n", fileName, m);
    return NULL;
  }

  /* if we are in append mode make windows behaviour compatible with unix */
  if ( !pipe && strchr (mode, 'a') ) fseek (fp, 0, SEEK_END);

  return fp;
}

/* ------------------------------------------------------------------------
    fileClose  close open file
   ------------------------------------------------------------------------ */

int fileClose(char* fileName, FILE* fp)
{
  int l = strlen(fileName);

  if (! fp) return TCL_OK;

  fflush( fp);

  if      (! strcmp( fileName + l - 2, ".Z"))   pclose( fp);
  else if (! strcmp( fileName + l - 3, ".gz"))  pclose( fp);
  else if (! strcmp( fileName + l - 4, ".bz2")) pclose( fp);
  else if (! strcmp( fileName + l - 4, ".SCP")) pclose( fp);
  else if (! strcmp( fileName,         "scp:")) pclose( fp);
  else if (! strcmp( fileName,         "ssh:")) pclose( fp);
  else                                          fclose( fp);

  return TCL_OK;
}

/* ------------------------------------------------------------------------
    freadAdd
   ------------------------------------------------------------------------ */

int freadAdd( ClientData cd, char* fileName, char commentChar,
              int (*readFn)(ClientData, int, char **))
{
  FILE *fp;
  char  line[100000];
  int   n  = 0, ln = 0;
  int   rc = TCL_OK;

  if (! (fp = fileOpen(fileName,"rt"))) return -1;

  strcpy(line,"add ");

  while (1) {
    int   argc; char **argv;
    unsigned char* p;                                /* needs to be unsigned to be compatible with utf-8 */
    int   f = fscanf(fp,"%[^\n]",&(line[4]));

    assert( f < 100000);

    ln++;

    if      ( f <  0)   break;
    else if ( f == 0) { fscanf(fp,"%*c"); continue; }

    if ( line[4] == commentChar) continue; 

    for (p=(unsigned char*)&(line[4]); *p!='\0'; p++) 
         if (*p>' ') break; if (*p=='\0') continue; 

    if ( Tcl_SplitList (itf,line,&argc,&argv) == TCL_OK) {
      if ( readFn(cd, argc, argv) != TCL_OK) {
           ERROR("<ITF,CRL>Can't read line %d: \"%s\"\n", ln, line+4);
           rc = TCL_ERROR;
      }
      else n++;
      Tcl_Free((char*) argv);
    }
    else {
      ERROR("<ITF,CPL>Can't parse line: %s\n", line+4);
      rc = TCL_ERROR;
    }
  }
  fileClose( fileName, fp);
  return (rc != TCL_OK) ? -1 : n;
}


/* ========================================================================
    Signal Handling
   ======================================================================== */

#ifndef DEBUG
#ifdef  DEBUG
static void janusSignalHandler (int sig)
{
  char buf[512];

  /* Call the default routine from now on to do a 
     standard exit after we've done our work (or
     to avoid recursion) */
  signal (sig, SIG_DFL);
  INFO ("janusSignalHandler: active on signal %d.\n", sig);

  /* if (streq (Tcl_GetVar (itf, "tcl_interactive", TCL_GLOBAL_ONLY), "0")) */
  sprintf (buf, "catch { janusErrorHandler %d }\n", sig);
  Tcl_Eval (itf, buf);

  /* Send ourself the signal we just received */
  raise (sig);
}
#endif
#endif

static Tcl_AsyncHandler asyncSigInt, asyncSigUsr1, asyncSigUsr2, asyncSigTerm;

void cSignalHandler(int sig);

void cSignalHandlerLevel2(int sig)
{ printf("Can't handle nested interrupts.\n"); 
  (void)signal(SIGINT ,(void (*)(int))cSignalHandlerLevel2); 
}

int tclSigIntHandler(ClientData clientData, Tcl_Interp *interp, int currentCode)
{
  int tty = isatty(0);
  int code, returnCode = currentCode;
  Tcl_DString command;
#ifdef DISABLE_READLINE
  #define LINESIZE 1024
  //char line[LINESIZE];
#endif

  (void)signal(SIGINT ,(void (*)(int))cSignalHandlerLevel2);

  Tcl_DStringInit(&command);
  
  if (tty && interp){
    int count, gotPartial = 0;
    char *cmd, *input = NULL;

    printO("CONT to continue (returning %d), BREAK to abort.\n",currentCode);

    while (1){
    prompt:
#ifndef DISABLE_READLINE
      input = readline (gotPartial ? ">> " : "%% ");
#else
      //input = gets (line);
      fgets (input, LINESIZE-1, stdin);
#endif
      if (input){
	count = strlen (input);
#ifndef DISABLE_READLINE
	add_history (input);
#endif
	cmd = Tcl_DStringAppend (&command, input, count);
	if (!Tcl_CommandComplete (cmd)){
	  cmd = Tcl_DStringAppend (&command, "\n",  1);
	  gotPartial = 1;
	  goto prompt;
	}
	gotPartial = 0;
	if (strcmp(cmd,"CONT")==0)      { returnCode = currentCode; break; }
	if (strcmp(cmd,"CONTOK")==0)    { returnCode = TCL_OK;      break; }
	if (strcmp(cmd,"CONTERROR")==0) { returnCode = TCL_ERROR;   break; }
	if (strcmp(cmd,"BREAK")==0)     { returnCode = TCL_BREAK;   break; }
	code = Tcl_RecordAndEval (interp, cmd, 0);
	Tcl_DStringFree (&command);
	//if (*interp->result != 0) {
	if (strlen (Tcl_GetStringResult (interp))) {
	  if ((code != TCL_OK) || (tty)){
	    //printf ("%s\n", interp->result);
	    printf ("%s\n", Tcl_GetStringResult (interp));
	  }
	}
      }
      else { Tcl_Eval(interp,""); returnCode = TCL_BREAK; break; }
    }
  }

  Tcl_DStringFree(&command);
  printf("\n"); /* make next prompt appear at beginning of line */
  (void)signal(SIGINT ,(void (*)(int))cSignalHandler);
  return returnCode;
}

int tclSigUsr1Handler(ClientData clientData, Tcl_Interp *interp, int code)
{
  Tcl_Eval(itf,"catch { sigUsr1Script }");
  return code;
}

int tclSigUsr2Handler(ClientData clientData, Tcl_Interp *interp, int code)
{
  Tcl_Eval(itf,"catch { sigUsr2Script }");
  return code;
}

int tclSigTermHandler(ClientData clientData, Tcl_Interp *interp, int code)
{
  Tcl_SavedResult state;
  Tcl_DString ei;
  int ec;

  //INFO ("tclSigTermHandler: result_in  = '%s'\n", Tcl_GetStringResult (itf));

  /* 8.5 has nicer code to do this, it seems */
  ec = Tcl_GetErrno ();
  Tcl_DStringInit (&ei);
  Tcl_DStringAppend (&ei, Tcl_GetVar (itf, "errorInfo", TCL_GLOBAL_ONLY), -1);
  Tcl_SaveResult (itf, &state);
  //INFO ("tclSigTermHandler: errorInfo  = '%s'\n", Tcl_DStringValue (&ei));

  /* This needs to be executed asynchronously */
  Tcl_EvalEx (itf, "catch { sigTermScript }", -1, TCL_EVAL_GLOBAL);
  
  /* 8.5 has nicer code to do this, it seems */
  Tcl_RestoreResult (itf, &state);
  Tcl_SetVar (itf, "errorInfo", Tcl_DStringValue (&ei), TCL_GLOBAL_ONLY);
  Tcl_DStringFree (&ei);
  Tcl_SetErrno (ec);

  //INFO ("tclSigTermHandler: result_out = '%s'\n", Tcl_GetStringResult (itf));

  /*
WARNING

It is almost always a bad idea for an asynchronous event handler to modify the interpreter's result or return a code different from its code argument. This sort of behavior can disrupt the execution of scripts in subtle ways and result in bugs that are extremely difficult to track down. If an asynchronous event handler needs to evaluate Tcl scripts then it should first save the interpreter's result plus the values of the variables errorInfo and errorCode (this can be done, for example, by storing them in dynamic strings). When the asynchronous handler is finished it should restore the interpreter's result, errorInfo, and errorCode, and return the code argument.
  */
  return code;
}

void cSignalHandler (int sig)
{
  fprintf (stderr, "Received interrupt request (%d).\n", sig);
  fflush  (stderr);
  
  /* Allow the Ibis decoder (or other C modules) to be interrupted */
  MostRecentlyReceivedSignal = sig;

  /* Arrange for async treatment */
  switch (sig) {
  case SIGINT :  Tcl_AsyncMark(asyncSigInt);  break;
#ifndef WINDOWS
  case SIGUSR1:  Tcl_AsyncMark(asyncSigUsr1); break;
  case SIGUSR2:  Tcl_AsyncMark(asyncSigUsr2); break;
#endif
  case SIGTERM:  Tcl_AsyncMark(asyncSigTerm); break;
  default     :  FATAL("Impossible Error");
  }
  
  (void)signal(SIGINT ,(void (*)(int))cSignalHandler);
#ifndef WINDOWS
  (void)signal(SIGUSR1,(void (*)(int))cSignalHandler);
  (void)signal(SIGUSR2,(void (*)(int))cSignalHandler);
#endif
  (void)signal(SIGTERM,(void (*)(int))cSignalHandler);

  return;
}

/* ------------------------------------------------------------- */
/* Monika wants to have the user time (like time) given by clock */
/* ------------------------------------------------------------- */
#ifndef WINDOWS
int usertimeItf (ClientData cd, Tcl_Interp *interp, int argc, char *argv[])
{
  static clock_t oldtime;
  clock_t newtime = 0;
  struct tms mytime;
  int ac = argc-1;
  int totalFlag = 0;

  if ( itfParseArgv(argv[0],&ac,argv+1,0,
    "-total", ARGV_INT, NULL, &totalFlag, NULL,"total of usertime (Flag 0/1)",
  NULL) != TCL_OK) {
    WARN("Flag value missing or unknown option\n");
    return TCL_ERROR;
  }
  times(&mytime);
  newtime = mytime.tms_utime;
  if (totalFlag) {
    itfAppendResult("%f",(newtime)/ (float)sysconf(_SC_CLK_TCK));
  } else {
    itfAppendResult("%f",(newtime-oldtime)/ (float)sysconf(_SC_CLK_TCK));
    oldtime = newtime;
  }
  return TCL_OK;
}

/* ------------------------------------------------------------- */
/* Fore IceIntegration we need also the system time              */
/* ------------------------------------------------------------- */
int systemtimeItf (ClientData cd, Tcl_Interp *interp, int argc, char *argv[])
{
  static clock_t oldtime;
  clock_t newtime = 0;
  struct tms mytime;
  int ac = argc-1;
  int totalFlag = 0;

  if ( itfParseArgv(argv[0],&ac,argv+1,0,
      "-total", ARGV_INT, NULL, &totalFlag, NULL,"total of systemtime (Flag 0/1)",
    NULL) != TCL_OK) {
    WARN("Flag value missing or unknown option\n");
    return TCL_ERROR;
  }

  times(&mytime);
  newtime = mytime.tms_stime;
  if (totalFlag) {
    itfAppendResult("%f",(newtime)/ (float)sysconf(_SC_CLK_TCK));
  } else {
    itfAppendResult("%f",(newtime-oldtime)/ (float)sysconf(_SC_CLK_TCK));
    oldtime = newtime;
  }
  return TCL_OK;
}

#else

int usertimeItf (ClientData cd, Tcl_Interp *interp, int argc, char *argv[]) {

	HANDLE     hCurrentProcess;

	FILETIME   creationTime;
	FILETIME   exitTime;
	FILETIME   kernelTime;
	FILETIME   userTime;

	SYSTEMTIME sysTime;

	static double oldTime;
	       double time;

	int ac        = argc-1;
	int totalFlag = 0;

	if ( itfParseArgv(argv[0],&ac,argv+1,0,
		"-total", ARGV_INT, NULL, &totalFlag, NULL,"total of usertime (Flag 0/1)",
		NULL) != TCL_OK) {
			WARN("Flag value missing or unknown option\n");
			return TCL_ERROR;
	 }
	 
	hCurrentProcess = GetCurrentProcess();
	GetProcessTimes(hCurrentProcess, &creationTime, &exitTime, &kernelTime, &userTime);
	FileTimeToSystemTime(&userTime, &sysTime);

	time = (double)sysTime.wSecond + sysTime.wMinute*60.0 + sysTime.wHour*60.0*60.0 + (double)sysTime.wMilliseconds/1000.0;

  if (totalFlag) {
    itfAppendResult("%f", time);
  } else {
    itfAppendResult("%f", time - oldTime);
    oldTime = time;
  }
  return TCL_OK;

}

int systemtimeItf (ClientData cd, Tcl_Interp *interp, int argc, char *argv[]) {

	HANDLE     hCurrentProcess;

	FILETIME   creationTime;
	FILETIME   exitTime;
	FILETIME   kernelTime;
	FILETIME   userTime;

	SYSTEMTIME sysTime;

	static double oldTime;
	double time;

	int ac        = argc-1;
  int totalFlag = 0;

	if ( itfParseArgv(argv[0],&ac,argv+1,0,
		"-total", ARGV_INT, NULL, &totalFlag, NULL,"total of usertime (Flag 0/1)",
		NULL) != TCL_OK) {
			WARN("Flag value missing or unknown option\n");
			return TCL_ERROR;
	 }
	 
	hCurrentProcess = GetCurrentProcess();
	GetProcessTimes(hCurrentProcess, &creationTime, &exitTime, &kernelTime, &userTime);
	FileTimeToSystemTime(&kernelTime, &sysTime);

	time = (double)sysTime.wSecond + sysTime.wMinute*60.0 + sysTime.wHour*60.0*60.0 + (double)sysTime.wMilliseconds/1000.0;

  if (totalFlag) {
    itfAppendResult("%f", time);
  } else {
    itfAppendResult("%f", time - oldTime);
    oldTime = time;
  }
  return TCL_OK;

}
#endif /* ifndef WINDOWS */


/* ------------------------------------------------------------------------
    unknownHandler   this functions extends an existing TCL "unknown"
                     handler procedure such that a dot separated command
                     name is expanded to a janus " . " separated one
   ------------------------------------------------------------------------ */

char unknownBody[] = "\n\
    set cmd [lindex $args 0]\n\
    if {[string first \".\" [lindex $args 0]]} {\n\
      regsub -all {\\.} $cmd \" . \" cmd\n\
      if {![regexp \"^tix.*:.*\" $cmd]} {\n\
        regsub -all {\\:} $cmd \" : \" cmd\n\
      }\n\
      if { $cmd != [lindex $args 0]} {\n\
        set cmd \"$cmd [lrange $args 1 end]\"\n\
        return [eval $cmd]\n\
      }\n\
    }";

int unknownHandler()
{
  DString hndl;

  dstringInit(&hndl);
  dstringAppend( &hndl, "proc unknown args {\n", -1);
  dstringAppend( &hndl, unknownBody);
  if ( Tcl_Eval( itf, "info body unknown") == TCL_OK) {
    //dstringAppend( &hndl, itf->result, -1);
    dstringAppend (&hndl, (char*) Tcl_GetStringResult (itf), -1);
  }
  dstringAppend( &hndl, "}", -1);

  if ( Tcl_Eval( itf, dstringValue(&hndl)) != TCL_OK) return TCL_ERROR;
  return TCL_OK;
}


/* ========================================================================
    Initialization
   ======================================================================== */

int Itf_Init ( Tcl_Interp *interp )
{
  itf = interp;

  if (Error_Init()     == TCL_ERROR) return TCL_ERROR;
 
  Tcl_InitHashTable ( &itfTypes,   TCL_STRING_KEYS );
  Tcl_InitHashTable ( &itfObjects, TCL_STRING_KEYS );

  Tcl_CreateCommand ( interp, "types",      itfListTypes,   0, 0 );
  Tcl_CreateCommand ( interp, "objects",    itfListObjects, 0, 0 );
  Tcl_CreateCommand ( interp, "makeArray",  itfMakeArray,   0, 0 );
  Tcl_CreateCommand ( interp, "fgets",      fgetsItf,       0, 0 );
  Tcl_CreateCommand ( interp, "fputs",      fputsItf,       0, 0 );
  Tcl_CreateCommand ( interp, "usertime",   usertimeItf,    0, 0 );
  Tcl_CreateCommand ( interp, "systemtime", systemtimeItf,  0, 0 );

  if (List_Init()      == TCL_ERROR) return TCL_ERROR;
  if (Array_Init()     == TCL_ERROR) return TCL_ERROR;
  if (unknownHandler() == TCL_ERROR) return TCL_ERROR;

  (void)init_mach_ind_io();

  asyncSigTerm = Tcl_AsyncCreate(tclSigTermHandler, NULL);
  asyncSigInt  = Tcl_AsyncCreate(tclSigIntHandler,  NULL);
#ifndef WINDOWS
  asyncSigUsr1 = Tcl_AsyncCreate(tclSigUsr1Handler, NULL);
  asyncSigUsr2 = Tcl_AsyncCreate(tclSigUsr2Handler, NULL);
#endif

  if (strcmp(Tcl_GetVar(itf,"tcl_interactive",TCL_GLOBAL_ONLY),"1")==0)
     (void)signal(SIGINT ,(void (*)(int))cSignalHandler);

#ifndef WINDOWS
  (void)signal(SIGUSR1,(void (*)(int))cSignalHandler);
  (void)signal(SIGUSR2,(void (*)(int))cSignalHandler);
#endif
  (void)signal(SIGTERM,(void (*)(int))cSignalHandler);

  /* ------------------------------------------------------
       Advanced Error Handling
     ---------------------------
       Here, we set an error handler for signals,
       so that we can catch them and maybe send e-mail,
       notify the user in other ways and so on.
     ------------------------------------------------------ */

#ifndef DEBUG
  /* Hangup detected on controlling terminal or death of controlling process */
  /* (void) signal (SIGHUP,  (void (*)(int)) janusSignalHandler); */

  /* Abort signal from abort(3) */
  /* (void) signal (SIGABRT, (void (*)(int)) janusSignalHandler); */

  /* Floating point exception */
  /* (void) signal (SIGFPE,  (void (*)(int)) janusSignalHandler); */

  /* Invalid memory reference */
  /* (void) signal (SIGSEGV, (void (*)(int)) janusSignalHandler); */
#endif

  return TCL_OK;
}
