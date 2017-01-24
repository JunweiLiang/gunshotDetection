/* ========================================================================
    JANUS      Speech- to Speech Translation
               ------------------------------------------------------------
               Object: DistribStream
               ------------------------------------------------------------

    Author  :  Michael Finke
    Module  :  distribStream.h
    Date    :  $Id: distribStream.h 512 2000-08-27 15:31:32Z jmcd $
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
    Revision 3.4  2000/08/27 15:31:12  jmcd
    Merging branch 'jtk-00-08-24-jmcd'.

    Revision 3.3.4.1  2000/08/25 22:19:43  jmcd
    Just checking.

    Revision 3.3  1997/07/31 16:33:23  rogina
    made code gcc -DFKIclean

    Revision 3.2  1997/06/17 17:41:43  westphal
    *** empty log message ***

     Revision 1.1  96/01/31  05:15:33  finkem
     Initial revision
     

   ======================================================================== */

#ifdef __cplusplus
extern "C" {
#endif

#ifndef _distribStream
#define _distribStream

#include "stream.h"
#include "distrib.h"
#include "tree.h"
#include "word.h"

/* ========================================================================
    DistribStream
   ======================================================================== */

typedef struct DistribStream_S
{
  char*         name;
  int           useN;

  DistribSet*   dssP;
  Tree*         treeP;

} DistribStream;

/* ========================================================================
    Module Interface
   ======================================================================== */

extern int distribStreamInit( DistribStream* dstP, char* name, 
                              DistribSet*    dssP, Tree* treeP);

extern DistribStream* distribStreamCreate( char* name, DistribSet* dssP, 
                                                       Tree*       treeP);

extern int   distribStreamIndex( ClientData cd, char* name);
extern char* distribStreamName(  ClientData cd, int i);
extern int   DistribStream_Init (void);

#endif

#ifdef __cplusplus
}
#endif
