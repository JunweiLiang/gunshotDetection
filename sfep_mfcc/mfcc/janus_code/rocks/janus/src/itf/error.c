/* ========================================================================
    JANUS      Speech- to Speech Translation
               ------------------------------------------------------------
               Object: Error Handling
               ------------------------------------------------------------

    Author  :  Michael Finke & Ivica Rogina
    Module  :  error.c
    Date    :  $Id: error.c 3332 2010-11-06 20:39:54Z metze $
    Remarks :

   ========================================================================

    This software was developed by the Interactive Systems Laboratories at

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

    $Log: error.c,v $
    Revision 3.10  2003/08/14 11:20:11  fuegen
    Merged changes on branch jtk-01-01-15-fms (jaguar -> ibis-013)

    Revision 3.9.4.1  2003/07/02 17:25:04  fuegen
    changed error message handling for Windows

    Revision 3.9  2001/01/15 09:49:56  janus
    Merged Christian's version with WINDOWS support (branch jtk-00-11-14-fuegen)

    Revision 3.8.4.1  2001/01/12 15:16:52  janus
    necessary changes for running janus under WINDOWS

    Revision 3.8  2000/11/14 12:01:12  janus
    Merged branch jtk-00-10-27-jmcd.

    Revision 3.7.30.2  2000/11/08 17:09:02  janus
    Finished making changes required for compilation under 'gcc -Wall'.

    Revision 3.7.30.1  2000/11/06 10:50:27  janus
    Made changes to enable compilation under -Wall.

    Revision 3.7  1999/02/04 09:39:31  soltau
    bugfix

    Revision 3.6  1998/08/02 11:10:27  kries
    faster processing of silent messages

 * Revision 3.5  1997/07/31  16:52:45  rogina
 * made code gcc-DFKIclean
 *
    Revision 3.4  1997/07/15 11:23:51  westphal
    removed bug with %

    Revision 3.3  1997/07/02 22:04:12  tschaaf
    *** empty log message ***

    Revision 3.2  1997/06/17 17:41:43  westphal
    *** empty log message ***

 
   ======================================================================== */

#include "common.h"
#include "tcl.h"
#include "itf.h"
#include <stdarg.h>

char errorRCSVersion[]= "@(#)1$Id: error.c 3332 2010-11-06 20:39:54Z metze $";

/* ------------------------------------------------------------------------
    Local Variables
   ------------------------------------------------------------------------ */

#define MAXERRACCU 65536 /* 10000 */

static char *errorFileName;
static int   errorLine;
static int   errorType;
static int   errorMode;

/* ------------------------------------------------------------------------
    messageHandlerPtr
   ------------------------------------------------------------------------ */

MsgHandler* msgHandlerPtr(char* file, int line, int type, int mode)
{

  errorFileName = file;
  errorLine     = line;
  errorType     = type;
  errorMode     = mode;

  /* to get rid of the long path names under windows */
#ifdef WINDOWS
  {
    int i, pos;
    for (i=pos=0; file[i] != '\0'; i++) {
      if ( file[i] == '/' || file[i] == '\\' ) pos = i;
    }

    if ( pos )
      errorFileName = &file[pos+1];
  }
#endif

  return &msgHandler;
}

/* ------------------------------------------------------------------------
    msgHandler
   ------------------------------------------------------------------------ */

int msgHandler( char *format, ... )
{
  va_list  ap;
  DString  str;
  char     buf[MAXERRACCU] = "", *format2 = format;
  char     errorMajor[60]  = "", errorMinor[60] = "";
  char     *result;
  const char *oldResult;

  if(format==0 && errorType==0) {
    if(errorMode==2 || ((errorMode==3) && (! Tcl_GetVar(itf,"itfOutputBuffer", TCL_GLOBAL_ONLY)))) {
      /* The command MSGPRINT(null) is frequently used and very expensive
	 The only effect of this is to "unset itfOutputBuffer" -- that can be done more directly */
      Tcl_UnsetVar(itf,"itfOutputBuffer",TCL_GLOBAL_ONLY);
      return 0;
    }
  }

  result = NULL;
  oldResult = Tcl_GetStringResult(itf);
  if (oldResult) {
    result = Tcl_Alloc(strlen(oldResult)+1);
    if (result!=NULL)
      strcpy(result, oldResult);
  }

  Tcl_ResetResult(itf);
  dstringInit( &str);
  dstringAppend(&str,"itfOutput ");

  if (format && sscanf(format,"<%[^,>],%[^>]>",errorMajor,errorMinor)==2)
    format2 = strchr(format,'>')+1;
  else *errorMajor = *errorMinor = '\0';

  switch (errorType)
  {
    case  0: if      (errorMode == 2) dstringAppend(&str,"CLEAR");
             else if (errorMode == 3) dstringAppend(&str,"DUMP");
             else                     dstringAppend(&str,"PRINT");
             break;
    case -1: dstringAppend(&str,"INFO");     break;
    case -2: dstringAppend(&str,"WARNING");  break;
    case -3: dstringAppend(&str,"ERROR");    break;
    case -4: dstringAppend(&str,"FATAL");    break;
    default: dstringAppend(&str,"UNKNOWN");  break;
  }

  dstringAppend(&str," %d",   errorMode);
  if ( format) {
       va_start(ap,format);
       vsprintf( buf, format2, ap);
       va_end(ap);
       dstringAppendElement(&str, "%s", buf);
  }
  else dstringAppend(&str," {}");

  dstringAppend(&str," {%s} %d {%s} {%s}", errorFileName, errorLine, errorMajor, errorMinor);
  dstringAppend(&str,"");

  if ( Tcl_Eval( itf, dstringValue(&str)) != TCL_OK ) {
    fprintf( STDERR, "MSG (%s,%d): %s", errorFileName, errorLine, buf);
  }
  dstringFree( &str);

  Tcl_ResetResult(itf);
  if ( result) Tcl_SetResult( itf, result, TCL_DYNAMIC);
  return 0;
}


/* ------------------------------------------------------------------------
    Error_Init
   ------------------------------------------------------------------------ */

int Error_Init(void)
{
  static int errorInitialized = 0;

  if (! errorInitialized) {
    errorInitialized = 1;
  }
  return TCL_OK;
}
