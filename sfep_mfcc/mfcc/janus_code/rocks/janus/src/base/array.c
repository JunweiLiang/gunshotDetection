/* ========================================================================
    JANUS      Speech- to Speech Translation
               ------------------------------------------------------------
               Object: Int/Float Arrays
               ------------------------------------------------------------

    Author  :  Michael Finke
    Module  :  array.c
    Date    :  $Id: array.c 2390 2003-08-14 11:20:32Z fuegen $
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
    Revision 3.6  2003/08/14 11:19:50  fuegen
    Merged changes on branch jtk-01-01-15-fms (jaguar -> ibis-013)

    Revision 3.5.20.3  2003/01/27 14:44:29  metze
    calloc fix

    Revision 3.5.20.2  2002/06/26 11:57:52  fuegen
    changes for new directory structure support and code cleaning

    Revision 3.5.20.1  2002/04/29 11:56:39  metze
    Cleaner compiling on gcc

    Revision 3.5  2000/11/14 12:01:09  janus
    Merged branch jtk-00-10-27-jmcd.

    Revision 3.4.34.1  2000/11/06 10:50:23  janus
    Made changes to enable compilation under -Wall.

    Revision 3.4  2000/08/16 09:02:52  soltau
    replaced free by Tcl_Free if memory was alloacted by Tcl_*

    Revision 3.3  1997/07/21 16:03:54  tschaaf
    gcc / DFKI Clean

    Revision 3.2  1997/06/17 17:41:43  westphal
    *** empty log message ***

    Revision 1.8  96/03/05  14:00:50  rogina
    added return value and type for the init function,
    made gcc -Wall warnings go away
    
    Revision 1.7  1996/02/27  00:20:06  rogina
    added sorting of iarrays

    Revision 1.6  1996/01/17  17:12:02  rogina
    fixed identifier name mixup

    Revision 1.5  96/01/17  17:07:40  17:07:40  rogina (Ivica Rogina)
    reset itemN counter to 0 after Deinit
    
    Revision 1.4  95/09/06  07:12:32  07:12:32  kemp (Thomas Kemp)
    *** empty log message ***
    
    Revision 1.3  1995/08/17  17:07:06  rogina
    *** empty log message ***

    Revision 1.2  1995/07/25  14:45:47  rogina
    *** empty log message ***

    Revision 1.1  1995/07/25  13:49:57  finkem
    Initial revision

 
   ======================================================================== */

#include "common.h"
#include "itf.h"
#include "array.h"
#include "list.h"


char arrayRcsVersion[]= "@(#)1$Id: array.c 2390 2003-08-14 11:20:32Z fuegen $";

/* ========================================================================
    Global Declarations
   ======================================================================== */

extern TypeInfo iarrayInfo;
extern TypeInfo farrayInfo;

/* ========================================================================
    Integer Array
   ======================================================================== */

static int iarrayPutsItf(      ClientData cd, int argc, char *argv[]);
static int iarrayConfigureItf( ClientData cd, char *var, char *val);

/* ------------------------------------------------------------------------
    iarrayInit
   ------------------------------------------------------------------------ */

int iarrayInit( IArray* IA, int n)
{
  assert(IA);
  IA->itemA = (int*)calloc( n, sizeof(int));
  IA->itemN =  n;
  return TCL_OK;
}

/* ------------------------------------------------------------------------
    iarrayDeinit
   ------------------------------------------------------------------------ */

int iarrayDeinit( IArray* IA)
{
  assert (IA);
  if (IA->itemA) { free( IA->itemA); IA->itemA = NULL; }
  IA->itemN = 0;
  return TCL_OK;
}

/* ------------------------------------------------------------------------
    iarrayPutsItf
   ------------------------------------------------------------------------ */

static int iarrayPutsItf( ClientData cd, int argc, char *argv[])
{
  IArray* IA = (IArray*)cd;

  argc--;
 
  if ( itfParseArgv( argv[0], &argc, argv+1, 0,  NULL) != TCL_OK) {
         return TCL_ERROR;
  }

  if (IA)
  {  int i;
     for (i=0; i<IA->itemN; i++) itfAppendResult("%d ",IA->itemA[i]);
  }
  return TCL_OK;
}

/* ------------------------------------------------------------------------
    iarrayConfigureItf
   ------------------------------------------------------------------------ */

static int iarrayConfigureItf( ClientData cd, char *var, char *val)
{
  IArray* IA = (IArray*)cd;

  if (! var) {

    ITFCFG(iarrayConfigureItf,cd,"itemN");
    return TCL_OK;
  }
  ITFCFG_Int(     var,val,"itemN",        IA->itemN,       1);
  ERROR("Unknown option '-%s %s'.\n", var, val ? val : "?");
  return TCL_ERROR;
}

/* ------------------------------------------------------------------------
    iarrayGetItf   parse an array from the <value> string
   ------------------------------------------------------------------------ */

int iarrayGetItf( IArray* IA, char* value)
{
  int     argc;
  char**  argv;

  if ( Tcl_SplitList( itf, value, &argc, &argv) == TCL_OK) {
    int i;

    if ( IA->itemN > 0) {
      if ( IA->itemN != argc) {
        ERROR("Expected IArray of size %d but got size %d.\n", IA->itemN, argc);
        return TCL_ERROR;
      }
    }
    else {
      IA->itemA = (int*)calloc( argc, sizeof(int));
      IA->itemN = argc;
    }
    for ( i = 0; i < argc; i++) {
      char* endPtr;
      IA->itemA[i] = strtol( argv[i], &endPtr, 0);
      if ( endPtr == argv[i] || *endPtr) {
        SERROR("Expected integer but got \"%s\".\n", argv[i]);
        return TCL_ERROR;
      }
    }
    Tcl_Free((char*)argv);
    return TCL_OK;
  }
  return TCL_ERROR;
}

/* ------------------------------------------------------------------------
    iarrayGetListItf   parse an array of list items from the <value> string
                       and store the result in an integer array
   ------------------------------------------------------------------------ */

int iarrayGetListItf( IArray* IA, char* value, List* L)
{
  int     argc;
  char**  argv;

  if ( Tcl_SplitList( itf, value, &argc, &argv) == TCL_OK) {
    int i;

    if ( IA->itemN > 0) {
      if ( IA->itemN != argc) {
        SERROR("Expected LArray of size %d but got size %d.\n", IA->itemN, argc);
        return TCL_ERROR;
      }
    }
    else {
      IA->itemA = (int*)calloc( argc, sizeof(int));
      IA->itemN = argc;
    }
    for ( i = 0; i < argc; i++) {
      IA->itemA[i] = listIndex( L, (ClientData)argv[i], 0);
      if ( IA->itemA[i] < 0) {
        SERROR("Can't find '%s' in the list.\n", argv[i]);
        return TCL_ERROR;
      }
    }
    Tcl_Free((char*)argv);
    return TCL_OK;
  }
  return TCL_ERROR;
}

/* ------------------------------------------------------------------------
    iarraySort         sort an integer array in ascending order
   ------------------------------------------------------------------------ */
/*
static int iarrayCmp (const void *a, const void *b) 
{ return *((int*)a) - *((int*)b); }

static int iarraySort (IArray *IA)
{
  qsort (IA->itemA, IA->itemN, sizeof(int), iarrayCmp);
  return TCL_OK;
}
*/
/* ========================================================================
    Float Array
   ======================================================================== */

static int farrayPutsItf(      ClientData cd, int argc, char *argv[]);
static int farrayConfigureItf( ClientData cd, char *var, char *val);

/* ------------------------------------------------------------------------
    farrayInit
   ------------------------------------------------------------------------ */

int farrayInit( FArray* FA, int n)
{
  assert(FA);
  FA->itemA = (float*)calloc( n, sizeof(float));
  FA->itemN =  n;
  return TCL_OK;
}

/* ------------------------------------------------------------------------
    farrayDeinit
   ------------------------------------------------------------------------ */

int farrayDeinit( FArray* FA)
{
  assert(FA);
  if (FA->itemA) { free( FA->itemA); FA->itemA = NULL; }
  FA->itemN = 0;
  return TCL_OK;
}

/* ------------------------------------------------------------------------
    farrayPutsItf
   ------------------------------------------------------------------------ */

static int farrayPutsItf( ClientData cd, int argc, char *argv[])
{
  FArray* FA = (FArray*)cd;

  argc--;
 
  if ( itfParseArgv( argv[0], &argc, argv+1, 0,  NULL) != TCL_OK) {
         return TCL_ERROR;
  }

  if (FA)
  {  int i;
     for (i=0; i<FA->itemN; i++) itfAppendResult("%e ",FA->itemA[i]);
  }
  return TCL_OK;
}

/* ------------------------------------------------------------------------
    farrayConfigureItf
   ------------------------------------------------------------------------ */

static int farrayConfigureItf( ClientData cd, char *var, char *val)
{
  FArray* FA = (FArray*)cd;

  if (! var) {

    ITFCFG(farrayConfigureItf,cd,"itemN");
    return TCL_OK;
  }
  ITFCFG_Int(     var,val,"itemN",        FA->itemN,       1);
  ERROR("Unknown option '-%s %s'.\n", var, val ? val : "?");
  return TCL_ERROR;
}

/* ------------------------------------------------------------------------
    farrayGetItf   parse an array from the <value> string
   ------------------------------------------------------------------------ */

int farrayGetItf( FArray* FA, char* value)
{
  int     argc;
  char**  argv;

  if ( Tcl_SplitList( itf, value, &argc, &argv) == TCL_OK) {
    int i;

    if ( FA->itemN > 0) {
      if ( FA->itemN != argc) {
        SERROR("Expected FArray of size %d but got size %d.\n", FA->itemN, argc);
        return TCL_ERROR;
      }
    }
    else {
      FA->itemA = (float*)calloc( argc, sizeof(float));
      FA->itemN =  argc;
    }
    for ( i = 0; i < argc; i++) {
      char* endPtr;
      FA->itemA[i] = strtod( argv[i], &endPtr);
      if ( endPtr == argv[i] || *endPtr) {
        SERROR("Expected integer but got \"%s\".\n", argv[i]);
        return TCL_ERROR;
      }
    }
    Tcl_Free((char*)argv);
    return TCL_OK;
  }
  return TCL_ERROR;
}

/* ========================================================================
    Type Declaration
   ======================================================================== */

static Method iarrayMethod[] = 
{ 
  { "puts", iarrayPutsItf, NULL },
  {  NULL,  NULL, NULL } 
};

TypeInfo iarrayInfo = { "IArray", 0, 0, iarrayMethod, 
                                NULL,               NULL,
                                iarrayConfigureItf, NULL, itfTypeCntlDefaultNoLink,
	                       "Array of integers\n" } ;

static Method farrayMethod[] = 
{ 
  { "puts", farrayPutsItf, NULL },
  {  NULL,  NULL, NULL } 
};

TypeInfo farrayInfo = { "FArray", 0, 0, farrayMethod, 
                                NULL,               NULL,
                                farrayConfigureItf, NULL, itfTypeCntlDefaultNoLink,
	                       "Array of floats\n" } ;

static int arrayInitialized = 0;

int Array_Init ( void )
{
  if (! arrayInitialized) {

    itfNewType ( &iarrayInfo );
    itfNewType ( &farrayInfo );
    arrayInitialized = 1;
  }
  return TCL_OK;
}
