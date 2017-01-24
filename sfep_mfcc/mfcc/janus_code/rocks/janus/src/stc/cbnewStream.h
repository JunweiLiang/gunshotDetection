/* ========================================================================
    JANUS      Speech- to Speech Translation
               ------------------------------------------------------------
               Object: CBNewStream
               ------------------------------------------------------------

    Author  :  Matthias Seeger
    Module  :  cbnewStream.h
    Date    :
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

   ======================================================================== */

#ifdef __cplusplus
extern "C" {
#endif

#ifndef _cbnewStream
#define _cbnewStream

#include "stream.h"
#include "cbnew.h"
#include "tree.h"
#include "word.h"

/* ========================================================================
   CBNewStream
   ======================================================================== */

typedef struct CBNewStream_S
{
  char*         name;
  int           useN;

  CBNewSet*     cbsP;
  Tree*         treeP;

} CBNewStream;

/* ========================================================================
   Module Interface
   ======================================================================== */

extern int          cbnewStreamInit   (CBNewStream* cstP,char* name,
				       CBNewSet* cbsP,Tree* treeP);
extern CBNewStream* cbnewStreamCreate (char* name,CBNewSet* cbsP,Tree* treeP);
extern int          cbnewStreamIndex  (ClientData cd, char* name);
extern char*        cbnewStreamName   (ClientData cd, int i);

extern int          CBNewStream_Init ( );

#endif

#ifdef __cplusplus
}
#endif
