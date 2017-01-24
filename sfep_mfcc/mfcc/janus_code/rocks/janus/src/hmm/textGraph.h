/* ========================================================================
    JanusRTk   Janus Recognition Toolkit
               ------------------------------------------------------------
               Object: Text Graph
               ------------------------------------------------------------

    Author  :  Michael Finke
    Module  :  textGraph.h
    Date    :  $Id: textGraph.h 512 2000-08-27 15:31:32Z jmcd $
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
    Revision 3.3  2000/08/27 15:31:29  jmcd
    Merging branch 'jtk-00-08-24-jmcd'.

    Revision 3.2.4.1  2000/08/25 22:19:45  jmcd
    Just checking.

    Revision 3.2  1997/06/17 17:41:43  westphal
    *** empty log message ***

 
   ======================================================================== */

#ifdef __cplusplus
extern "C" {
#endif

#ifndef _textGraph
#define _textGraph

/* ========================================================================
    Text Graph
   ======================================================================== */

typedef struct
{
  char**     stateA;
  int        stateN;

  int*       fwdTrN;   /* number of forward transitions for every word */
  int**      fwdIdx;   /* a vertex-index for every words successors    */
  float**    fwdPen;   /* a penalty for every forward transition       */ 

  int        initN;
  int*       init;

} TextGraph;

#endif

#ifdef __cplusplus
}
#endif
