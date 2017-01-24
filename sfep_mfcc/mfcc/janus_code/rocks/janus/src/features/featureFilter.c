/* ========================================================================
    JANUS      Speech- to Speech Translation
               ------------------------------------------------------------
               Object: Filter Type & Operators
               ------------------------------------------------------------

    Author  :  Martin Westphal
    Module  :  featureFilter.c
    Date    :  $Id: featureFilter.c 2743 2007-02-08 09:49:23Z stueker $
    Remarks :

   ========================================================================

    This software was developed by

    the  University of Karlsruhe       and  Carnegie Mellon University
         Dept. of Informatics               Dept. of Computer Science
         ILKD, Lehrstuhl Prof.Waibel        Alex Waibel's NN & Speech Group
         Am Fasanengarten 5                 5000 Forbes Ave
         D-76131 Karlsruhe                  Pittsburgh, PA 15213
         - West Germany -                   - USA -

    Copyright (C) 1990-1994.   All rights reserved.

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
    Revision 3.6  2007/02/08 09:49:23  stueker
    Added additional code from Wichael Wand

    Revision 3.5  2003/08/14 11:19:55  fuegen
    Merged changes on branch jtk-01-01-15-fms (jaguar -> ibis-013)

    Revision 3.4.4.1  2002/06/26 11:57:53  fuegen
    changes for new directory structure support and code cleaning

    Revision 3.4  2001/01/15 09:49:56  janus
    Merged Christian's version with WINDOWS support (branch jtk-00-11-14-fuegen)

    Revision 3.3.40.1  2001/01/12 15:16:53  janus
    necessary changes for running janus under WINDOWS

    Revision 3.3  2000/08/16 09:21:23  soltau
    free -> Tcl_Free

    Revision 3.2  1997/07/09 17:42:22  westphal
    *** empty log message ***

  
   ======================================================================== */

#include "featureFilter.h"
#include <math.h>


char filterRcsVersion[]= "@(#)1$Id:";

/* ------------------------------------------------------------------------
    Create
   ------------------------------------------------------------------------ */

Filter* filterCreate(int n, int m)
{
   Filter* f = (Filter*)malloc(sizeof(Filter));
   assert(n>=0 && m>=0);

   /* --- allocate structure --- */
   if (! f) {
      SERROR("allocation failure 1 filterCreate.\n");
      return NULL;
   }
   f->offset = 0;
   f->n = 0;
   f->m = 0;
   f->a = NULL;
   f->b = NULL;
   
   if (n>0) {
      int i;
      f->n = n;
      f->a = (double*)malloc(sizeof(double)*n);
      if (! f->a) {
	 SERROR("allocation failure 2 filterCreate.\n");
	 free(f); return NULL;
      }
      for (i=0; i<n; i++) f->a[i] = 0.0;
   }
   else {
      return f; 
   }

   if (m>0) {
      int i;
      f->m = m;
      f->b = (double*)malloc(sizeof(double)*m);
      if (! f->b) {
	 SERROR("allocation failure 3 filterCreate.\n");
	 free(f->a); free(f); return NULL;
      }
      for (i=0; i<m; i++) f->b[i] = 0.0;
   }
   return f;
}

ClientData filterCreateItf( ClientData cd, int argc, char *argv[])
{
   argc--;
   if ( argc == 1) {
      char *string = NULL;
      if (itfParseArgv( argv[0], &argc, argv+1, 0,
		"@<filename>|<definition>", ARGV_STRING,  NULL, &string, cd,
		"@ and file name or filter definition: \"<offset> {a0 a1 ..} [{b1 b2 ..}]\".",
		NULL) != TCL_OK) return NULL;
      return (ClientData)filterGetItf( string);
   }
   if ( argc == 2) {
      int n = 0, m = 0;
      if (itfParseArgv( argv[0], &argc, argv+1, 0,
	       "<n>",   ARGV_INT,  NULL,  &n, cd, "#of coefficients in a",
	       "<m>",   ARGV_INT,  NULL,  &m, cd, "#of coefficients in b",
	       NULL) != TCL_OK) return NULL;
      n = (n<0) ? 0 : n;
      m = (m<0) ? 0 : m;
      return (ClientData)filterCreate(n,m);
   }
   
   ERROR("Need 1 or 2 arguments: @<filename> | \"<offset> {a0 a1 ..} [{b1 b2 ..}]\" | <n> <m>\n");
   return NULL;
}

/* ------------------------------------------------------------------------
    Free
   ------------------------------------------------------------------------ */

void filterFree( Filter* f)
{
  if (f) {
    if ( f->a) free( f->a);
    if ( f->b) free( f->b);
    free(f);
  }
}

static int filterFreeItf( ClientData cd )
{
  filterFree((Filter*)cd);
  return TCL_OK;
}

/* ------------------------------------------------------------------------
    filterPutsItf
   ------------------------------------------------------------------------ */

int filterPutsItf( ClientData cd, int argc, char *argv[])
{
  Filter *f = (Filter*)cd;
  int      i;

  argc--;
  if (itfParseArgv(argv[0], &argc, argv+1, 0, 
		   NULL) != TCL_OK) return TCL_ERROR;
  itfAppendResult( "%d { ",f->offset);
  for ( i = 0; i < f->n; i++) {
      itfAppendResult( "% 2.4e ", f->a[i]);
  }
  if (f->m > 0) itfAppendResult( "} { ");
  for ( i = 0; i < f->m; i++) {
      itfAppendResult( "% 2.4e ", f->b[i]);
  }
  itfAppendResult( "}");
  return TCL_OK;
}

/* ------------------------------------------------------------------------
    filterCopy - implemented by Michael Wand.
    dest must be initialized with the corrent size.
   ------------------------------------------------------------------------ */
void filterCopy(Filter* dest,Filter* src)
{
	int i;
	dest->offset = src->offset;
	dest->n = src->n;
	dest->m = src->m;
	for (i = 0;i < dest->n;++i) {
		dest->a[i] = src->a[i];
	}
	for (i = 0;i < dest->m;++i) {
		dest->b[i] = src->b[i];
	}
}

/* ------------------------------------------------------------------------
    filterGetItf
    Give a name of a short vector object 'av[0]' and get a pointer to it
    with '(Filter*)result'. Return value is < 0 if object couldn't be found.
    Use this function together with itfParseArgv().
   ------------------------------------------------------------------------ */

Filter* filterGetItf( char* value)
{
  Filter* f = NULL;
	
  /* ------------------------------------------------------------------------
	If 'value' is not an existing filter object scan filter elements.
	------------------------------------------------------------------------ */
  if ((f = (Filter*)itfGetObject( value, "Filter")) == NULL) {
		int     argc,n,m = 0,offset,i;
		char    **argv,**aa,**ab;
		
		MSGCLEAR(NULL);
		
		if ( Tcl_SplitList( itf, value, &argc, &argv) != TCL_OK) {
			ERROR("1. parsing definition\n"); return NULL;
		}
		
		if (argc < 2) {
			ERROR("expected \"<offset> {a0 a1 ..} ..\".\n");
			Tcl_Free((char*)argv); return NULL;
		}
		
		if ( sscanf( argv[0], "%d", &offset) != 1) {
			ERROR("offset is no integer: %s.\n",argv[0]);
			Tcl_Free((char*)argv); return NULL;
		}
		if ( Tcl_SplitList( itf, argv[1], &n, &aa) != TCL_OK) {
			ERROR("2. parsing a\n");
			Tcl_Free((char*) argv); return NULL;
		}
		if (argc > 2  &&  Tcl_SplitList( itf, argv[2], &m, &ab) != TCL_OK) {
			ERROR("3. parsing b\n");
			Tcl_Free((char*) aa); Tcl_Free((char*) argv); return NULL;
		}
		
		if (! (f = filterCreate( n, m))) {
			ERROR("4. allocating filter\n");
			if (m) Tcl_Free((char*) ab); Tcl_Free((char*) aa); Tcl_Free((char*) argv); return NULL;
		}
		f->offset = offset;
		for ( i = 0; i < n; i++) {
			double d;
			if ( sscanf( aa[i], "%lf", &d) != 1) {
				ERROR("expected 'float' type elements in a (%d/%d).\n",i,n);
				if (m) Tcl_Free((char*) ab); Tcl_Free((char*) aa); Tcl_Free((char*)argv);
				filterFree( f);  return NULL;
			}
			f->a[i] = d;
		}
		for ( i = 0; i < m; i++) {
			double d;
			if ( sscanf( ab[i], "%lf", &d) != 1) {
				ERROR("expected 'float' type elements in b (%d/%d).\n",i,n);
				if (m) Tcl_Free((char*) ab); Tcl_Free((char*) aa); Tcl_Free((char*)argv);
				filterFree( f);  return NULL;
			}
			f->b[i] = d;
		}
		if (m) Tcl_Free((char*) ab); Tcl_Free((char*) aa); Tcl_Free((char*)argv);
		return f;
  }
  /* ------------------------------------------------------------------------
	'value' is an existing filter object.
	------------------------------------------------------------------------ */
  else {
    Filter* g = filterCreate( f->n, f->m);
		
    if (! g) return NULL;
    filterCopy( g, f);
    return g;
  }
}


/* --- to be used with 'itfParseArgv()' --- */
int getFilter( ClientData cd, char* key, ClientData result, int *argcP, char *argv[])
{
   Filter *f;

   assert(cd);
   if ( (*argcP<1) || ((f = filterGetItf( argv[0]))  ==  NULL)) {
     if (!key) SERROR("Can't get filter '%s' as %s\n", argv[0], key);
     return -1;
   }

   *((Filter**)result) = f;
   (*argcP)--;   return 0;
}

/* ========================================================================
    Type Declaration
   ======================================================================== */

Method filterMethod[] = 
{ 
  { "puts",   filterPutsItf,   "print filter" }, 
  { NULL,     NULL, NULL } 
};

TypeInfo filterInfo = { "Filter", 0, -1, filterMethod, 
                          filterCreateItf, filterFreeItf, NULL, NULL, itfTypeCntlDefaultNoLink, 
		         "LTI filter\n" } ;

int Filter_Init ( void)
{
  itfNewType ( &filterInfo );
  return TCL_OK;
}
