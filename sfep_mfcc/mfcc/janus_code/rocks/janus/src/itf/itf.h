/* ========================================================================
    JANUS      Speech- to Speech Translation
               ------------------------------------------------------------
               Object: TCL <-> C-Objects Interface Functions
               ------------------------------------------------------------

    Author  :  Michael Finke & Ivica Rogina
    Module  :  itf.h
    Date    :  $Id: itf.h 2864 2009-02-16 21:18:17Z metze $
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
    Revision 3.12  2007/02/22 16:33:11  fuegen
    increased fgets_maxline size

    Revision 3.11  2005/12/14 08:35:29  stueker
    Introduced fgets_maxline, set it to 2048

    Revision 3.10  2005/03/04 09:11:20  metze
    Code for 'NFS-SSH' (commented out), fixes for SPHERE

    Revision 3.9  2003/08/14 11:20:11  fuegen
    Merged changes on branch jtk-01-01-15-fms (jaguar -> ibis-013)

    Revision 3.8.4.4  2002/06/26 11:57:55  fuegen
    changes for new directory structure support and code cleaning

    Revision 3.8.4.3  2002/02/28 15:44:57  soltau
    Elegant abort

    Revision 3.8.4.2  2001/05/17 08:20:46  metze
    Added function to get object type

    Revision 3.8.4.1  2001/01/29 17:23:30  fuegen
    removed some errors on WINDOWS

    Revision 3.8  2001/01/15 09:49:58  janus
    Merged Christian's version with WINDOWS support (branch jtk-00-11-14-fuegen)

    Revision 3.7.40.1  2001/01/12 15:16:55  janus
    necessary changes for running janus under WINDOWS

    Revision 3.7  2000/08/27 15:31:18  jmcd
    Merging branch 'jtk-00-08-24-jmcd'.

    Revision 3.6.2.1  2000/08/25 22:19:44  jmcd
    Just checking.

    Revision 3.6  2000/08/24 13:32:26  jmcd
    Merging changes from branch jtk-00-08-23-jmcd.

    Revision 3.5.2.1  2000/08/24 09:32:44  jmcd
    More revision fixes.

    Revision 3.5  1998/06/30 01:08:20  kries
    *** empty log message ***

 * Revision 3.4  1998/05/26  11:15:02  kries
 * added ARGV_LONG
 *
 * Revision 3.3  1997/07/07  18:25:05  westphal
 * pedantic clean
 *
    Revision 3.2  1997/06/17 17:41:43  westphal
    *** empty log message ***

    Revision 1.21  96/06/05  14:23:21  finkem
    stderr/... moved from mainc to intf.c
    
    Revision 1.20  1996/03/24  05:04:23  kries
    added ITFCFG_IArray

    Revision 1.19  1996/02/05  09:34:08  manke
     Added #ifndef WINDOWS to make Visual C++ happy.

    Revision 1.18  1996/01/31  04:10:07  finkem
    added fileOpen and fileClose

    Revision 1.17  1996/01/17  17:20:37  rogina
    added ARGV_FARRAY

    Revision 1.16  96/01/12  15:39:09  15:39:09  finkem (Michael Finke)
    added freadAdd function to read a file to object add function
    
    Revision 1.15  1996/01/05  15:08:12  finkem
    added Itf_Init extern declaration

    Revision 1.14  1996/01/05  15:00:24  rogina
    added predeclaration of itfTclMethod()

    Revision 1.13  96/01/04  21:01:26  21:01:26  rogina (Ivica Rogina)
    added itfCommandName()
    
    Revision 1.12  95/09/12  19:48:39  19:48:39  finkem (Michael Finke)
    Modified handling of dynamic strings (Append gets variable number of
    arguments now).
    
    Revision 1.11  1995/08/18  09:34:43  rogina
    *** empty log message ***

    Revision 1.10  1995/08/18  08:22:00  finkem
    *** empty log message ***

    Revision 1.9  1995/08/17  17:39:46  finkem
    *** empty log message ***

    Revision 1.8  1995/08/17  17:06:25  rogina
    *** empty log message ***

    Revision 1.7  1995/08/16  12:57:13  finkem
    *** empty log message ***

    Revision 1.6  1995/08/07  20:56:01  finkem
    *** empty log message ***

    Revision 1.5  1995/08/04  13:12:48  finkem
    *** empty log message ***

    Revision 1.4  1995/08/04  09:11:29  finkem
    *** empty log message ***

    Revision 1.3  1995/07/27  17:25:36  rogina
    *** empty log message ***

    Revision 1.2  1995/07/27  14:53:18  finkem
    READ_ONLY

    Revision 1.1  1995/07/25  13:49:57  finkem
    Initial revision

 
   ======================================================================== */

#ifdef __cplusplus
extern "C" {
#endif

#ifndef _ITF
#define _ITF

#include <sys/types.h>

#ifndef WINDOWS
	#include <sys/time.h>
#endif

#include <time.h>

#include "common.h"

#ifndef DISABLE_TCLTK
#include <tcl.h>
  extern Tcl_Interp* itf;

#else

  /* ==================== Tcl_Interp ==================== */
#ifndef _CLIENTDATA
#   if defined(__STDC__) || defined(__cplusplus)
    typedef void *ClientData;
#   else
    typedef int *ClientData;
#   endif /* __STDC__ */
#define _CLIENTDATA
#endif

  typedef struct Tcl_Interp {
    char *result;
  } Tcl_Interp;
  
  Tcl_Interp* itf;

  /* ==================== DString ==================== */
#define TCL_DSTRING_STATIC_SIZE 200
typedef struct Tcl_DString {
    char *string;               /* Points to beginning of string:  either
                                 * staticSpace below or a malloced array. */
    int length;                 /* Number of non-NULL characters in the
                                 * string. */
    int spaceAvl;               /* Total number of bytes available for the
                                 * string and its terminating NULL char. */
    char staticSpace[TCL_DSTRING_STATIC_SIZE];
                                /* Space to use in common case where string
                                 * is small. */
} Tcl_DString;

  /* ==================== Other Typedefs ==================== */
typedef void Tcl_FreeProc(char *blockPtr);

  /* ==================== Defines ==================== */
#define TCL_OK             0
#define TCL_ERROR          1
#define TCL_RETURN         2
#define TCL_BREAK          3
#define TCL_CONTINUE       4

#define TCL_VOLATILE       NULL
#define TCL_GLOBAL_ONLY    1
#define TCL_LEAVE_ERR_MSG  0x200

#define Tcl_CreateCommand(a,b,c,d,e)

  /* ==================== Mapped Functions ==================== */
  extern int   Tcl_SplitList   (Tcl_Interp *itf, char *list, int *tokc, char ***tokv);
  extern char* Tcl_Merge       (int argc, char **argv);
  extern void  Tcl_Free        (char *ptr);
  extern int   Tcl_Eval        (Tcl_Interp *itf, char *script);
  extern int   Tcl_GlobalEval  (Tcl_Interp *itf, char *script);
  extern int Tcl_VarEval       (Tcl_Interp *itf, char *s1, char *s2, ...);
  extern void  Tcl_SetResult   (Tcl_Interp *itf, char *string, Tcl_FreeProc *freeProc);
  extern char* Tcl_SetVar      (Tcl_Interp *itf, char *varName, char *newValue, int flags);
  extern int   Tcl_RegExpMatch (Tcl_Interp *tf, char *string, char *pattern);
#endif

#define STDOUT stdout
#define STDIN  stdin
#define STDERR stderr 

/* ------------------------------------------------------------------------
    Method Description Type
   ------------------------------------------------------------------------ */

typedef int MethodProc ( ClientData clientData, int argc, char *argv[] );

typedef struct
{
  char         *name;
  MethodProc   *proc;
  char         *help;

} Method;

/* ------------------------------------------------------------------------
    Type Information
   ------------------------------------------------------------------------ */

typedef ClientData Constructor( ClientData clientData, int argc, char *argv[] );
typedef int        Destructor(  ClientData clientData );
typedef int        Configurer(  ClientData clientData, char *variable, char *value );

typedef struct TypeInfoStruct {
  char        *name;
  int          size;
  int          argN;
  Method      *method;

  Constructor *constructor;
  Destructor  *destructor;
  Configurer  *configure;
  ClientData (*access)( ClientData clientData, char *name, struct TypeInfoStruct **ti );
  int        (*typeCntl)( ClientData clientData, struct TypeInfoStruct *ti, int command, ...);
  char        *help;

} TypeInfo;

/* ------------------------------------------------------------------------
    Generic Object Type
   ------------------------------------------------------------------------ */

typedef struct
{
  char *name;
  int   useN;
}
GenericObject;


/* ========================================================================
    Dynamic Strings
   ======================================================================== */

typedef Tcl_DString DString;

#define dstringLength(dsPtr) ((dsPtr)->length)
#define dstringValue(dsPtr)  ((dsPtr)->string)

extern void  dstringInit(         DString* dsP);
extern void  dstringFree(         DString* dsP);
extern void  dstringSetLength(    DString* dsP, int len);
extern void  dstringResult(       DString* dsP);
extern void  dstringGetResult(    DString* dsP);
extern void  dstringStartSublist( DString* dsP);
extern void  dstringEndSublist(   DString* dsP);

extern char* dstringAppend(        DString* dsP, char* str, ...);
extern char* dstringAppendElement( DString* dsP, char* str, ...);

/* ========================================================================
    Argument Parsing
   ======================================================================== */
/* ------------------------------------------------------------------------
    Automatic parsing of argv arguments
   ------------------------------------------------------------------------ */

typedef enum { ARGV_END,
               ARGV_SHORT,
               ARGV_INT,
               ARGV_LONG,
               ARGV_FLOAT,
               ARGV_DOUBLE,
               ARGV_STRING,
               ARGV_IARRAY,
               ARGV_FARRAY,
               ARGV_LIARRAY,
               ARGV_MIARRAY,
               ARGV_OBJECT,
               ARGV_LINDEX,
               ARGV_CUSTOM,
               ARGV_REST } ArgvType;

typedef int ArgvHandler( ClientData cd, char* key, ClientData dst,
                         int* argc, char*argv[]);

typedef struct {

  char*        key;                /* the key string / name of the option */
  ArgvType     type;               /* argument type                       */
  ArgvHandler* handler;            /* handler function                    */
  ClientData   dst;
  ClientData   cd;
  char*        help;

} ArgvInfo;

extern int itfParseArgv( char* method,
                         int*  argcPtr,  char* argv[], int leftOver, ...);

/* ------------------------------------------------------------------------
    Macros for the configuration method :

    int typeConfigureItf( ClientData cd, char *variable, char *value )
    {
      Type* F = (Type*)cd;

      if (! variable) {
        ITFCFG(fesConfigureItf,cd,"verbosity");
        ITFCFG(fesConfigureItf,cd,"adcByteOrder");
        ITFCFG(fesConfigureItf,cd,"adcHeader");
        return TCL_OK;
      }

      ITFCFG_Int(    variable,value,"verbosity",   F->verbosity, 0);
      ITFCFG_CharPtr(variable,value,"adcByteOrder",F->adcByteOrder, 0);
      ITFCFG_CharPtr(variable,value,"adcHeader",   F->adcHeader, 1);

      ERR(-1,ERR_DEF,"%s(%d) unknown option '-%s %s'.\n", variable,
                     value ? value : "?");
      return TCL_ERROR;
    }

   ------------------------------------------------------------------------ */

#define ITFCFG(FUNC,CD,NAME) \
    itfAppendResult( "{-%s ",NAME); \
    FUNC ( CD, NAME, NULL); itfAppendResult( "} ");

#define ITFCFG_Int(VAR,VALUE,S,P,READ_ONLY) \
  if (! strcmp(VAR,S)) { \
    if (! VALUE) { itfAppendResult("%d",P); return TCL_OK; } \
    if (! READ_ONLY) { P = atoi(VALUE); } \
    return TCL_OK; \
  }

#define ITFCFG_Float(VAR,VALUE,S,P,READ_ONLY) \
  if (! strcmp(VAR,S)) { \
    if (! VALUE) { itfAppendResult("%f",P); return TCL_OK; } \
    if (! READ_ONLY) { P = atof(VALUE); } \
    return TCL_OK; \
  }

#define ITFCFG_Char(VAR,VALUE,S,P,READ_ONLY) \
  if (! strcmp(VAR,S)) { \
    if (! VALUE) { itfAppendElement("%c",P); return TCL_OK; } \
    if (! READ_ONLY) { P = *(char*)VALUE; } \
    return TCL_OK; \
  }

#define ITFCFG_CharPtr(VAR,VALUE,S,P,READ_ONLY) \
  if (! strcmp(VAR,S)) { \
    if (! VALUE) { itfAppendElement("%s",(P)?P:"(null)"); return TCL_OK; } \
    if (! READ_ONLY) { if ( P) free(P); \
      if ((VALUE)[0]=='@') P = itfReadFile((VALUE)+1); \
      else P = strdup(VALUE); \
    } \
    return TCL_OK; \
  }

#define ITFCFG_Object(VAR,VALUE,S,P,NAME,TYPE,READ_ONLY) \
  if (! strcmp(VAR,S)) { \
    if (! VALUE) { itfAppendElement("%s",(P && P->NAME)?P->NAME:"(null)"); \
                   return TCL_OK; } \
    if (! READ_ONLY) { \
      TYPE* objP = (TYPE*)itfGetObject(VALUE,#TYPE); \
      if (! objP) return TCL_ERROR; \
      else { if (P) unlink(P,#TYPE); P = objP; link(P,#TYPE); }} \
    return TCL_OK; \
  }

#define ITFCFG_IArray(VAR,VALUE,S,P,VOCABP,READ_ONLY) \
  if (! strcmp(VAR,S)) { \
    if (! VALUE) { int i; \
                   itfAppendResult("\"");\
                   for(i=0;i<P->itemN-1;i++) \
                       itfAppendResult("%s ",vocabName(VOCABP,P->itemA[i])); \
                   if(P->itemN>0) \
                     itfAppendResult("%s\"",vocabName(VOCABP,P->itemA[i])); \
                   else \
                     itfAppendResult("\""); \
                   return TCL_OK; } \
    if (! READ_ONLY) { \
      if(iarrayGetListItf(P, VALUE, (List *)&(VOCABP->vwordList)) != TCL_OK) { \
        SERROR("Can't read list index array for %s.\n", VALUE); \
        return TCL_ERROR; \
      }  \
    } \
    return TCL_OK;  \
  }

/* ------------------------------------------------------------------------
    Timer
   ------------------------------------------------------------------------ */

char *dateString(void);

#ifndef WINDOWS
typedef struct Timer_S
{
  struct timeval  start;
  struct timeval  stop;
  float           delta;

} Timer;
#else
typedef struct Timer_S
{
	clock_t	start;
	clock_t	stop;
  float   delta;

} Timer;
#endif /* WINDOWS */

extern void  timerReset( Timer* timerP);
extern void  timerStart( Timer* timerP);
extern float timerStop(  Timer* timerP);

/* ------------------------------------------------------------------------
    fgets
   ------------------------------------------------------------------------ */


/* ------------------------------------------------------------------------
    Global Variables
   ------------------------------------------------------------------------ */
#define fgets_maxline 4096

/* ------------------------------------------------------------------------
    typeCntl command constants and macroes
   ------------------------------------------------------------------------ */

#define T_LINK          1
#define T_DELINK        2
#define T_UNLINK        3
#define T_LINKN         4

#define link(X,Y)   itfTypeCntl((ClientData)(X),itfGetType(Y),T_LINK)
#define delink(X,Y) itfTypeCntl((ClientData)(X),itfGetType(Y),T_DELINK)
#define unlink(X,Y) itfTypeCntl((ClientData)(X),itfGetType(Y),T_UNLINK)
#define linkN(X,Y)  itfTypeCntl((ClientData)(X),itfGetType(Y),T_LINKN)

/* ------------------------------------------------------------------------
    Functions
   ------------------------------------------------------------------------ */

extern int        Itf_Init(     Tcl_Interp* interp);
#ifndef DISABLE_TCLTK
extern void       Itf_Main(int argc, char **argv, Tcl_AppInitProc *appInitProc);
#endif

extern int        itfNewType(   TypeInfo *typeInfo );
extern TypeInfo*  itfGetType(   char *typeName );
extern ClientData itfGetObject( char *objectName, char *typeName );
extern char*      itfGetObjectType( char *objectName );
extern int        itfTypeCntl (ClientData clientData, TypeInfo *typeInfo, int command, ...);
extern int        itfTypeCntlDefault (ClientData clientData, TypeInfo *typeInfo, int command, ...);
extern int        itfTypeCntlDefaultNoLink (ClientData clientData, TypeInfo *typeInfo, int command, ...);
extern void       itfDestroyAllObjects (void);
extern int        itfRemoveObjectHash(ClientData clientData);

extern void       itfAppendResult(  char *format, ...);
extern void       itfAppendElement( char *format, ...);
extern void       itfResetResult( void);

extern void printO( char *format, ... );
extern void printE( char *format, ... );

extern char *itfReadFile( char *filename );

extern FILE* fileOpen( char* fileName, char* mode);
extern int   fileClose(char* fileName, FILE* fp);
extern int   freadAdd( ClientData cd, char* filename, char commentChar,
                       int (*readFn)(ClientData, int, char **));

/* The NFS-SSH hack */
int fileNFSSSH (char*, char*);

extern char *itfCommandName(void);
extern int   itfTclMethod (TypeInfo* ti, int argc, char **argv);

#endif

#ifdef __cplusplus
}
#endif
