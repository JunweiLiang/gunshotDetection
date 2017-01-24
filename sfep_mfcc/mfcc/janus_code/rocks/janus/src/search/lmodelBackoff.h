/* ========================================================================
    JanusRTk   Janus Recognition Toolkit
               ------------------------------------------------------------
               Object: Backoff with intermediate classes
               ------------------------------------------------------------

    Author  :  Klaus Ries
    Module  :  lmodelBackoff.h
    Date    :  $Id: lmodelBackoff.h 700 2000-11-14 12:35:27Z janus $
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
    Revision 1.4  2000/11/14 12:29:36  janus
    Merging branch jtk-00-10-27-jmcd.

    Revision 1.3.34.1  2000/11/06 10:50:34  janus
    Made changes to enable compilation under -Wall.

    Revision 1.3  2000/08/27 15:31:22  jmcd
    Merging branch 'jtk-00-08-24-jmcd'.

    Revision 1.2.4.1  2000/08/25 22:19:44  jmcd
    Just checking.

    Revision 1.2  1998/08/02 10:58:31  kries
    changed modeling

 * Revision 1.1  1998/06/30  00:34:09  kries
 * Initial revision
 *

   ======================================================================== */

#ifdef __cplusplus
extern "C" {
#endif

#ifndef _lmodelBackoff_h
#define _lmodelBackoff_h

#include "list.h"
#include "lm.h"

/* ========================================================================
    Forward Declarations
   ======================================================================== */

/* ========================================================================
    LModelBackoffItem
   ======================================================================== */

typedef struct LModelBackoffItem_S {	/* word list entry */
  
  char *name;
  int   wordIdxToClass;		/* index of word  in toClassLmP */
  int   classIdxToClass;	/* index of class in toClassLmP */

  int   wordIdxToWord;		/* index of word  in toWordLmP */
  int   classIdxToWord;	 	/* index of class in toWordLmP */
   
} LModelBackoffItem;

/* ========================================================================
    LModelBackoff
   ======================================================================== */

typedef struct LIST(LModelBackoffItem) LModelBackoffItemList;

typedef struct LModelBackoff_S {

  char*         	name;   /* name of the language model */
  int           	useN;   /* useN counter               */

  LModelBackoffItemList	w;	/* list of words */

  float                 lz;
  float                 lp;
  
  char                  commentChar;

  LMPtr                 toClassLmP;
  LMPtr                 toWordLmP;

} LModelBackoff;

extern char* LModelBackoffName(  LModelBackoff* lmodel, int i); 
extern int   LModelBackoffIndex( LModelBackoff* lmodel, char* name);
extern float LModelBackoffScore( LModelBackoff* lmodel, int* w, int n, int k);

extern int   LModelBackoff_Init();

#endif

#ifdef __cplusplus
}
#endif
