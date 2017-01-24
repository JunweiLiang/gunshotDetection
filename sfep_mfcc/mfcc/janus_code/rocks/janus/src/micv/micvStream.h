/* ========================================================================
    JANUS      Speech- to Speech Translation
               ------------------------------------------------------------
               Object: MicvStream
               ------------------------------------------------------------

    Author  :  Andreas Eller
    Module  :  micvStream.h

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
    Revision 1.1  2005/03/08 17:30:28  metze
    Initial code as of Andreas Eller


   ======================================================================== */

#ifdef __cplusplus
extern "C" {
#endif

#ifndef _micvStream
#define _micvStream

#include "stream.h"
#include "micv.h"

/* ========================================================================
   MicvStream
   ======================================================================== */

typedef struct MicvStream_S
{
  char*         name;
  int           useN;

  MicvSet*      pMicvSet;
  Tree*         pTree;

} MicvStream;

/* ========================================================================
   Module Interface
   ======================================================================== */

extern int          micvStreamInit   (MicvStream* pMicvStream, char* name, MicvSet* pMicvSet, Tree* pTree);
extern MicvStream*  micvStreamCreate (char* name, MicvSet* pMicvSet, Tree* pTree);
extern int          micvStreamIndex  (ClientData cd, char* name);
extern char*        micvStreamName   (ClientData cd, int i);

extern int          MicvStream_Init ( );

#endif

#ifdef __cplusplus
}
#endif

