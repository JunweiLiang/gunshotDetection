/* ========================================================================
    JANUS      Speech- to Speech Translation
               ------------------------------------------------------------
               Object: Main Module
               ------------------------------------------------------------

    Author  :  Michael Finke & Ivica Rogina
    Module  :  main.c
    Date    :  $Id: janusMain.c 3114 2010-01-30 21:11:38Z metze $
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

   
   ======================================================================== */

#include "common.h"
#include "itf.h"

#ifndef DISABLE_TK
#include <tk.h>
#endif

#include "common.h"

#ifdef DISABLE_TCLTK
#include "itfWrapper.h"
#endif

char janusRcsVersion[]= 
            "@(#)1$Id: janusMain.c 3114 2010-01-30 21:11:38Z metze $";

/* ------------------------------------------------------------------------
    Tcl_AppInit
   ------------------------------------------------------------------------ */

extern int Janus_Init (Tcl_Interp *interp);

#ifndef DISABLE_TCLTK
int Tcl_AppInit(Tcl_Interp * interp)
{
    /*
     * Call the init procedures for included packages.  Each call should
     * look like this:
     *
     * if (Mod_Init(interp) == TCL_ERROR) return TCL_ERROR;
     *
     * where "Mod" is the name of the module.
     */

    if (Tcl_Init(interp)   == TCL_ERROR) return TCL_ERROR;
#ifndef DISABLE_TK
    if (Tk_Init(interp)    == TCL_ERROR) return TCL_ERROR;

    Tcl_StaticPackage(interp, "Tk", Tk_Init, (Tcl_PackageInitProc *) NULL);
#endif

    if (Itf_Init(interp)   == TCL_ERROR) return TCL_ERROR;
    if (Janus_Init(interp) == TCL_ERROR) return TCL_ERROR;

    Tcl_SetVar (interp, "tcl_rcFileName", "~/.janusrc", TCL_GLOBAL_ONLY);
    return TCL_OK;
}
#endif

#ifndef DISABLE_TCLTK
/* ----------------------------------------------------------------------
   main subroutine with TCL/TK
   ---------------------------------------------------------------------- */
int main( int argc, char** argv) {
#ifndef DISABLE_TK
  Itf_Main (argc, argv, Tcl_AppInit);
#else
  /*  Tcl_Main (argc, argv, Tcl_AppInit); */
  Itf_Main (argc, argv, Tcl_AppInit);
#endif
  return 0;
}

#else

/* ----------------------------------------------------------------------
   main subroutine without TCL/TK
   ---------------------------------------------------------------------- */
int main (int argc, char **argv) {

  int   ac = argc-1;

  char *descFile      = NULL;
  char *featDescFile  = NULL;
  int   verbose       = 1;

  FILE *fp;
  char  line[MAXLINE];
  char  whiteSep[]    = " \t\n";
  char  cmtSep[]      = ";#%";
  int   i, lineN;
  char *lP;

  /* --------------------------------------------------
     Initialize IBIS
     -------------------------------------------------- */
  itfInit ();
  if (Janus_Init(NULL) == TCL_ERROR) return TCL_ERROR;

  /* --------------------------------------------------
     Parsing Arguments
     -------------------------------------------------- */
  if ( itfParseArgv (argv[0], &ac, argv+1, 0,
		     "<descFile>", ARGV_STRING, NULL, &descFile, NULL,
		     "description file for starting IBIS",
		     "-featDesc",  ARGV_STRING, NULL, &featDescFile, NULL,
		     "feature description file for IBIS",
		     "-v",         ARGV_INT,    NULL, &verbose,  NULL,
		     "verbosity",
		     NULL) != TCL_OK )
    return TCL_ERROR;

  /* --------------------------------------------------
     Start IBIS
     -------------------------------------------------- */
  if ( (fp=fileOpen (descFile, "r")) == NULL ) {
    ERROR ("Can't open file %s\n", descFile);
    return TCL_ERROR;
  }

  lineN = 0;

  while ( !feof (fp) && fgets (line, MAXLINE, fp) ) {
    lineN++;

    /* skip leading white spaces and empty lines */
    i = strspn (line, whiteSep);
    if ( i == (int)strlen (line) ) continue;
    else lP = &line[i];

    /* skip comment lines */
    if ( strspn (lP, cmtSep) > 0 ) continue;

    /* delete comments at end of line and remove "\n" */
    i = strcspn (line, cmtSep);
    if ( i >= 1 && i < (int)strlen (line) ) {
      line[i] = '\0';
    } else {
      i = strlen (line) - 1;
      line[i] = '\0';
    }

    /* NOW WE CAN START */
    if ( verbose ) {
      printf ("%s\n", lP);
    }
    if ( itfDo (lP) != TCL_OK ) {
      ERROR ("While reading descFile %s in line %d\n", descFile, lineN);
      return TCL_ERROR;
    }
  }

  /* just for debugging */
  getc (stdin);

  return TCL_OK;
}

#endif /* DISABLE_TCLTK */

