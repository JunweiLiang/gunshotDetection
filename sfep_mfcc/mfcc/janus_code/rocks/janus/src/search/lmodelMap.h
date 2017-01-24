/* ========================================================================
    JanusRTk   Janus Recognition Toolkit
               ------------------------------------------------------------
               Object: Language Model Word Mapper
               ------------------------------------------------------------

    Author  :  Michael Finke
    Module  :  lmodelMap.h
    Date    :  $Id: lmodelMap.h 700 2000-11-14 12:35:27Z janus $
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
    Revision 1.4  2000/11/14 12:32:23  janus
    Merging branch jtk-00-10-27-jmcd.

    Revision 1.3.34.1  2000/11/06 10:50:35  janus
    Made changes to enable compilation under -Wall.

    Revision 1.3  2000/08/27 15:31:23  jmcd
    Merging branch 'jtk-00-08-24-jmcd'.

    Revision 1.2.4.1  2000/08/25 22:19:45  jmcd
    Just checking.

    Revision 1.2  1998/07/02 11:01:09  kries
    introduced lmzMap

 * Revision 1.1  1998/05/26  18:27:23  kries
 * Initial revision
 *

   ======================================================================== */

#ifdef __cplusplus
extern "C" {
#endif

#ifndef _lmodelMap_h
#define _lmodelMap_h

#include "list.h"
#include "array.h"

/* ========================================================================
    Forward Declarations
   ======================================================================== */

/* ========================================================================
    LModelMapItem
   ======================================================================== */

typedef struct LModelMapItem_S {

  char*    name;
  IArray   idxA;
  float    prob;

} LModelMapItem;

/* ========================================================================
    LModelMap
   ======================================================================== */

typedef struct LIST(LModelMapItem) LModelMapList;

typedef struct LModelMap_S {

  char*         name;   /* name of the language model */
  int           useN;   /* useN counter               */

  LModelMapList list;

  LMPtr         lmPtr;
  float         lz;
  float         lzMap;
  float         lp;

  char          commentChar;

} LModelMap;

extern char* lmodelMapName(  LModelMap* lmodel, int i); 
extern int   lmodelMapIndex( LModelMap* lmodel, char* name);
extern float lmodelMapScore( LModelMap* lmodel, int* w, int n, int k);

extern int   lmodelMapNextFrame( LModelMap* lmP);
extern int   lmodelMapReset(     LModelMap* lmP);

extern int   LModelMap_Init( );

#endif

#ifdef __cplusplus
}
#endif
