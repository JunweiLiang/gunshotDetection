/* ========================================================================
    JANUS      Speech- to Speech Translation
               ------------------------------------------------------------
               Object: Error Handling
               ------------------------------------------------------------

    Author  :  Michael Finke & Ivica Rogina
    Module  :  error.h
    Date    :  $Id: error.h 512 2000-08-27 15:31:32Z jmcd $
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

    $Log$
    Revision 3.4  2000/08/27 15:31:12  jmcd
    Merging branch 'jtk-00-08-24-jmcd'.

    Revision 3.3.4.1  2000/08/25 22:19:43  jmcd
    Just checking.

    Revision 3.3  1997/07/31 16:52:45  rogina
    made code gcc-DFKIclean

    Revision 3.2  1997/06/17 17:41:43  westphal
    *** empty log message ***

    Revision 1.2  96/01/07  10:29:38  finkem
    moved error handling to a Tcl function called itfOutput which
    is predefined to act exactly like the original C code
    
    Revision 1.1  1995/07/25  13:49:57  finkem
    Initial revision

 
   ======================================================================== */

#ifdef __cplusplus
extern "C" {
#endif

#ifndef _ERROR
#define _ERROR

#define INFO     msgHandlerPtr(__FILE__,__LINE__,-1,0)
#define WARN     msgHandlerPtr(__FILE__,__LINE__,-2,0)
#define SWARN    msgHandlerPtr(__FILE__,__LINE__,-2,1)
#define ERROR    msgHandlerPtr(__FILE__,__LINE__,-3,0)
#define SERROR   msgHandlerPtr(__FILE__,__LINE__,-3,1)
#define FATAL    msgHandlerPtr(__FILE__,__LINE__,-4,0)

#define MSGCLEAR msgHandlerPtr(__FILE__,__LINE__,0,2)
#define MSGPRINT msgHandlerPtr(__FILE__,__LINE__,0,3)

typedef int MsgHandler(   char*, ... );

extern MsgHandler*   msgHandlerPtr(char* file, int line, int type, int mode);
extern int           msgHandler(   char*, ... );
extern int           Error_Init(void);

#endif



#ifdef __cplusplus
}
#endif
