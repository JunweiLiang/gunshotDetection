/* ========================================================================
    JANUS      Speech- to Speech Translation
               ------------------------------------------------------------
               Object: Dynamic Programming Flat Forward Search
               ------------------------------------------------------------

    Author  :  Michael Finke
    Module  :  flatfwd.c
    Date    :  $Id: flatfwd.h 512 2000-08-27 15:31:32Z jmcd $
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
    Revision 3.5  2000/08/27 15:31:15  jmcd
    Merging branch 'jtk-00-08-24-jmcd'.

    Revision 3.4.4.1  2000/08/25 22:19:43  jmcd
    Just checking.

    Revision 3.4  1997/07/18 17:57:51  monika
    gcc-clean

    Revision 3.3  1997/07/18 14:00:01  monika
    dfki-clean code

    Revision 3.2  1997/06/17 17:41:43  westphal
    *** empty log message ***

    Revision 1.5  1996/07/03 13:19:26  monika
    changes to work with 64K vocabulary

    Revision 1.4  96/04/23  19:52:59  19:52:59  maier (Martin Maier)
    monika: partial hypos and RUNON, fullFlatFwd
    
    Revision 1.3  96/01/01  14:21:59  14:21:59  monika (Monika Woszczyna)
    *** empty log message ***
    
    Revision 1.2  1995/09/12  19:51:01  finkem
    Handling of hypo publishing changed

    Revision 1.1  1995/07/25  13:49:57  finkem
    Initial revision

 
   ======================================================================== */

#ifdef __cplusplus
extern "C" {
#endif

#ifndef _flatfwd
#define _flatfwd

#include "search.h"
#include "wordlist.h"
/* ------------------------------------------------------------------------
    FlatFwd
   ------------------------------------------------------------------------ */

typedef struct FlatFwd_S {

  int        trellisN;
  WordX      wordN;

  char*      expWordA;
  WordX*     expWordXA;

  float      beamWidth;         /* beam width for flat forward pass */
  float      phoneBeamWidth;
  float      lastPhoneBeamWidth;
  float      wordBeamWidth;     /* flat forward word beam           */
  float      lmZ;               /* language model weight            */

  int        topN;              /* evaluate top N word transitions  */

  short      minEFrameN;        /* */
  short      maxWindow;

  HypoList   hypoList;          /* list of hypothesis               */

} FlatFwd;


/* ------------------------------------------------------------------------
    FlatFwd Modul Interface
   ------------------------------------------------------------------------ */


extern int   FlatFwd_Init(void);
extern int   flatFwdInit(   FlatFwd* ffP, Vocab* vocabP);
extern int   flatFwdCreate( void );
extern void  flatFwdFree( void );
extern int   flatFwdDeinit( FlatFwd* ffP);
extern void  flatFwdFree( void );
extern float dpsFlatForward( void* sP, short mode,char * evalS, WordList * wlP);

#endif






#ifdef __cplusplus
}
#endif
