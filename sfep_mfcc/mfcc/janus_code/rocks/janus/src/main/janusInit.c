/* ========================================================================
    JANUS      Speech- to Speech Translation
               ------------------------------------------------------------
               Object: Package Initialization Module
               ------------------------------------------------------------

    Author  :  Michael Finke
    Module  :  init.c
    Date    :  $Id: janusInit.c 3372 2011-02-28 00:26:20Z metze $
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
#ifndef DISABLE_TCLTK
#include "tcl.h"
#endif

#include "matrix.h"
#include "dbase.h"
#include "feature.h"
#include "sample.h"
#include "bbi.h"
#include "hmm.h"
#include "path.h"
#include "cbnewStream.h"
#include "spass.h"

#ifndef IBIS
#include "keyspot.h"
#include "keyspotLP.h"
#include "ffnet.h"
/* I have no idea, why this should be included here exactly (Hagen) */
#include "search.h"
#include "micvStream.h"
#endif

#ifdef CMUJRTK
/* Wilson: LDA module for online LM adaptation */
//#include "janus_elda.h"
//#include "janus_lsa.h"
//#include "boost.h"
//#include "janus_exp.h"
//#include "janus_cachelm.h"
/* Roger: AEGIS module */
#include "code_v9a.kemllr_lite/janus_aegis.h"
#endif

/*
 * The following string is the startup script executed in new
 * interpreters.  It looks on disk in several different directories
 * for a script "init.tcl" that is compatible with this version
 * of Tcl.  The init.tcl script does all of the real work of
 * initialization.
 */

static char initScript[] =
"proc janusInit {} {\n\
    global janus_library janus_version janus_patchLevel auto_path env\n\
    rename janusInit {}\n\
    set dirs {}\n\
    if [info exists env(JANUS_LIBRARY)] {\n\
        lappend dirs $env(JANUS_LIBRARY)\n\
    }\n\
    set parentDir [file dirname [file dirname [info nameofexecutable]]]\n\
    if [string match {*[ab]*} $janus_patchLevel] {\n\
        set lib janus$janus_patchLevel\n\
    } else {\n\
        set lib janus$janus_version\n\
    }\n\
	lappend dirs [file normalize [file join $parentDir .. .. library]]\n\
	lappend dirs [file normalize [file join $parentDir .. .. .. library]]\n\
    foreach i $dirs {\n\
		set janus_init [file join [set janus_library $i] init.tcl]\n\
		if {![catch {uplevel #0 source \"\\\"$janus_init\\\"\"} msg]} {\n\
            lappend auto_path $i\n\
            return\n\
	    }\n\
    }\n\
    set msg \"Can't find a usable init.tcl in the following directories: \n\"\n\
    append msg \"    $dirs\n\"\n\
    append msg \"This probably means that JanusRTk wasn't installed properly.\n\"\n\
    error $msg\n\
}\n\
janusInit";

/*
 * the search strategy for init.tcl is roughly as follows (see 'initScript'):
 *
 *   1.) look in $env(JANUS_LIBRARY)
 *   2.) look in source tree (relative to the executable)
 *   3.) look in JANUS_LIBRARY/janusJANUS_PATCH_LEVEL
 *   4.) look in JANUS_LIBRARY/janusJANUS_VERSION
 *   5.) look in JANUS_LIBRARY/janus
 */

#ifndef JANUS_LIBRARY
#define JANUS_LIBRARY         "/usr/local/lib"
#endif

static char defaultLibraryDir[200] = JANUS_LIBRARY;

#define JANUS_VERSION         "5.1.1"
#define JANUS_PATCH_LEVEL     "000"
#define JANUS_TAG             ""

#ifndef JANUS_SVN_REVISION
#define JANUS_SVN_REVISION    "n/a"
#endif

int Janus_Init (Tcl_Interp *interp)
{
  fprintf( stderr, "# ===========================================================================\n");
  fprintf( stderr, "#  ____  ____  _____ _                                                  \n");
  fprintf( stderr, "# |__  ||  _ \\|_   _| | _      V%s P%s %s [%s %s]\n",
                   JANUS_VERSION, JANUS_PATCH_LEVEL, JANUS_TAG, __DATE__, __TIME__);
  fprintf( stderr, "#    | || |_| | | | | |/ |     svn revision: %s\n", JANUS_SVN_REVISION);
  fprintf( stderr, "#    | ||  _ <  | | |   <   ------------------------------------------------\n");
  fprintf( stderr, "#    | ||_| |_| |_| |_|\\_|    Karlsruhe Institute of Technology, Germany\n");
  fprintf( stderr, "#   _| | JANUS Recognition        Carnegie Mellon University, USA          \n");
  fprintf( stderr, "#  \\__/        Toolkit                                                  \n");
  fprintf( stderr, "#                              (c) 1993-2011  Interactive Systems Labs   \n");
  fprintf( stderr, "# ===========================================================================\n");

  Tcl_SetVar(interp, "janus_library",    defaultLibraryDir, TCL_GLOBAL_ONLY);
  Tcl_SetVar(interp, "janus_version",    JANUS_VERSION,     TCL_GLOBAL_ONLY);
  Tcl_SetVar(interp, "janus_patchLevel", JANUS_PATCH_LEVEL, TCL_GLOBAL_ONLY);
  Tcl_SetVar(interp, "janus_lmInterface",      "2",         TCL_GLOBAL_ONLY);

  if (Matrix_Init()      == TCL_ERROR) return TCL_ERROR;
  if (DBase_Init()       == TCL_ERROR) return TCL_ERROR;

  if (Feature_Init()     == TCL_ERROR) return TCL_ERROR;

  if (SampleSet_Init()   == TCL_ERROR) return TCL_ERROR;
  if (BBI_Init()         == TCL_ERROR) return TCL_ERROR;

  if (HMM_Init()         == TCL_ERROR) return TCL_ERROR;
  if (Path_Init()        == TCL_ERROR) return TCL_ERROR;

#ifndef IBIS
  if (Keyspot_Init()     == TCL_ERROR) return TCL_ERROR;
  if (KeyspotLP_Init()   == TCL_ERROR) return TCL_ERROR;
  if (Search_Init()      == TCL_ERROR) return TCL_ERROR;
  if (FFNet_Init()       == TCL_ERROR) return TCL_ERROR;
  if (MicvStream_Init()  == TCL_ERROR) return TCL_ERROR;
#endif

#ifdef CMUJRTK
  /* Start Wilson Tam 18 Jun 04 */
//  if (Maxent_Init()      == TCL_ERROR) return TCL_ERROR;
//  if (eLDA_Init()        == TCL_ERROR) return TCL_ERROR;
//  if (cacheLM_Init()     == TCL_ERROR) return TCL_ERROR;
//  if (LSA_Init()         == TCL_ERROR) return TCL_ERROR;
//  if (Boost_Init()       == TCL_ERROR) return TCL_ERROR;
//  if (EXP_Init()         == TCL_ERROR) return TCL_ERROR;
  /* Start Roger Hsiao 17 Apr 06 */
  if(Aegis_Init()        == TCL_ERROR) return TCL_ERROR;
#endif

  if (CBNewStream_Init() == TCL_ERROR) return TCL_ERROR;
  if (SPass_Init()       == TCL_ERROR) return TCL_ERROR;

  return Tcl_Eval(interp, initScript);
}



