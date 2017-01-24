/* ========================================================================
    JANUS      Speech- to Speech Translation
               ------------------------------------------------------------
               Object: Common Includes
               ------------------------------------------------------------

    Author  :  Ivica Rogina
    Module  :  common.h
    Date    :  $Id: common.h 3095 2010-01-26 15:15:42Z metze $
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
    Revision 4.7  2007/03/02 12:58:04  fuegen
    added SSE support (Florian Metze, Friedrich Faubel)
    added some function mappings for Windows

    Revision 4.6  2007/02/21 16:19:17  fuegen
    moved macro Nulle from lmla.h to common.h

    Revision 4.5  2006/02/21 15:02:51  fuegen
    added log1p for windows

    Revision 4.4  2003/08/14 11:19:50  fuegen
    Merged changes on branch jtk-01-01-15-fms (jaguar -> ibis-013)

    Revision 4.3.2.1.2.10  2003/01/15 14:33:31  metze
    Added MostRecentlyReceivedSignal

    Revision 4.3.2.1.2.9  2002/07/18 12:17:48  soltau
    Added malloc.h

    Revision 4.3.2.1.2.8  2002/06/26 11:57:52  fuegen
    changes for new directory structure support and code cleaning

    Revision 4.3.2.1.2.7  2002/04/18 09:21:10  fuegen
    replaced windows definition of strcasecmp by _stricmp

    Revision 4.3.2.1.2.6  2002/02/01 14:19:57  fuegen
    made it compilable under cygwin

    Revision 4.3.2.1.2.5  2002/01/25 10:01:34  fuegen
    Added define for M_LOG10 for Windows

    Revision 4.3.2.1.2.4  2001/10/04 09:08:19  metze
    If you define -DOSF1, it is possible to compile on Alpha (memalign problem)

    Revision 4.3.2.1.2.3  2001/07/19 17:27:56  metze
    Added #include <float.h> which is needed on Linux with glibc 2.2.2

    Revision 4.3.2.1.2.2  2001/07/06 08:20:41  fuegen
    map memalign to malloc for windows

    Revision 4.3.2.1.2.1  2001/06/01 08:24:59  fuegen
    redefine pragma settings for windows
    added include of rand48.h for windows

    Revision 4.3.2.1  2001/01/25 10:26:43  fuegen
    removed some errors on WINDOWS

    Revision 4.3  2001/01/15 09:49:55  janus
    Merged Christian's version with WINDOWS support (branch jtk-00-11-14-fuegen)

    Revision 4.2.36.1  2001/01/12 15:16:50  janus
    necessary changes for running janus under WINDOWS

    Revision 4.2  2000/08/27 15:31:10  jmcd
    Merging branch 'jtk-00-08-24-jmcd'.

    Revision 4.1.4.1  2000/08/25 22:19:43  jmcd
    Just checking.

    Revision 4.1  2000/01/12 10:02:02  fuegen
    set MAX_NAME to 256 to handle long distrib-names

    Revision 4.0  1998/11/30 18:07:14  fuegen
    *** empty log message ***

    Revision 3.2  1997/06/17 17:41:43  westphal
    *** empty log message ***

    Revision 1.6  96/06/04  15:51:21  finkem
    #ifdef HAVE_UNISTD_H
    
    Revision 1.5  1996/02/05  09:33:23  manke
     Added #ifndef WINDOWS to make Visual C++ happy.

    Revision 1.4  1995/11/17  19:52:12  finkem
    Finally included math.h as common include

    Revision 1.3  1995/09/23  22:56:53  torsten
    Added TRUE & FALSE

    Revision 1.2  1995/07/27  15:31:29  finkem
    *** empty log message ***

    Revision 1.1  1995/07/25  13:49:57  finkem
    Initial revision

 
   ======================================================================== */

#ifdef __cplusplus
extern "C" {
#endif

#ifndef _common
#define _common

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#ifdef DARWIN
#include <malloc/malloc.h>
#include <float.h>
#define memalign(a,b) malloc ((b))
#else
#include <malloc.h>
#endif

/* This is needed for glibc 2.2.2 */
#ifdef LINUX
#include <float.h>
#endif

/* (fuegen) assuming that all platforms except windows have unistd.h
 * I have removed all HAVE_UNISTD_H defines in other files */
#ifndef WINDOWS
	#include <unistd.h>
#else
	#include <float.h>
	#undef  ERROR
	#include <windows.h>
	#undef  ERROR
	#define ERROR msgHandlerPtr(__FILE__,__LINE__,-3,0)
	#undef  MEM_FREE
#endif

#ifdef SSE_OPT
#include <xmmintrin.h>
#include <emmintrin.h>
#endif

#ifdef DISABLE_TCLTK
#define DISABLE_TK
#endif

extern FILE *STDERR;
extern FILE *STDOUT;
extern FILE *STDIN;

#include "error.h"

#define MAX_NAME 256

  /* A variable to hold the most recent signal received by the
     program and handled by cSignalHandler (allows the decoder to
     be interrupted
  */
extern int MostRecentlyReceivedSignal;

  /* ----------------------------------------------------------------------
     platform specific definitions
     ---------------------------------------------------------------------- */
#ifdef WINDOWS
#define popen(x,y)         _popen (x, y)
#define pclose(x)          _pclose (x)
#define sleep(x)            Sleep (1000*x)
#define usleep(x)           Sleep (x/1000)
#define gethostname(x,y)   -1
#define getgid()            0
#define getuid()            0
#define strncasecmp(x,y,z) _strnicmp (x,y,z)
#define strcasecmp(x,y)    _stricmp (x,y)
#define isnan(x)           _isnan (x)
#define finite(x)          _finite (x)
#define memalign(x,y)       malloc (y)
#define M_PI   3.14159265358979323846
#define M_LN10 2.30258509299404568402
#define log1p(x)            log (1+x)
#ifdef SPHERE
  #define mkstemp           mktemp     /* for Windows SPHERE support */
  #define snprintf          sprintf_s  /* for Windows SPHERE support */
#endif
#ifdef SSE_OPT
  #define _mm_malloc        _aligned_malloc
  #define _mm_free          _aligned_free
#endif

/* pragma directives */
#ifndef _DEBUG
  #define DISABLE_WARNINGS
  #pragma warning( disable : 4305 4244 4101 )
#else
  #define DISABLE_WARNINGS
  #pragma warning( disable : 4305 4244 )
#endif /* _DEBUG */
#endif /* WINDOWS */

#ifdef CYGWIN
#define memalign(x,y)       malloc (y)
#endif

#ifdef OSF1
#define memalign(x,y)       malloc (y)
#endif

  /* ------------------------------------------------------------------------
     Common macros:
     ------------------------------------------------------------------------ */
#define streq(s1,s2)    (strcmp (s1,s2) == 0)     /* string equality        */
#define streqc(s1,s2)   (strcasecmp (s1,s2) == 0) /* not case sensitive     */
#define strneq(s1,s2)   (strcmp (s1,s2) != 0)     /* string inequality      */
#define ABS(x)          ((x)>=0?(x):-(x))         /* absolute value         */
#define SGN(s1)		(s1==0 ? 0 : (s1>0 ? 1:(-1) )) /* signum function   */
#define MIN(a,b)        (((a)<(b))?(a):(b))            /* minimum           */
#define MAX(a,b)        (((a)>(b))?(a):(b))            /* maximum           */
#define Nulle(a)        if (a) { free (a); (a) = NULL; }

#ifndef TRUE
#define TRUE  1
#endif
#ifndef FALSE
#define FALSE 0
#endif


#endif /* _common */


#ifdef __cplusplus
}
#endif
