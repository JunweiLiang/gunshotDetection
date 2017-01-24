/* ========================================================================
    JANUS      Speech- to Speech Translation
               ------------------------------------------------------------
               Object: Search Vocabulary
               ------------------------------------------------------------

    Author  :  Hagen Soltau
    Module  :  svocab.h
    Date    :  $Id: svocab.h 2390 2003-08-14 11:20:32Z fuegen $
    Remarks : 

   ========================================================================

    This software was developed by

    the  University of Karlsruhe       and  Carnegie Mellon University
         Dept. of Informatics               Dept. of Computer Science
         ILKD, Lehrstuhl Prof.Waibel        Alex Waibel's NN & Speech Group
         Am Fasanengarten 5                 5000 Forbes Ave
         D-76131 Karlsruhe                  Pittsburgh, PA 15213
         - Germany -                        - USA -

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
    Revision 1.4  2003/08/14 11:20:10  fuegen
    Merged changes on branch jtk-01-01-15-fms (jaguar -> ibis-013)

    Revision 1.3.4.17  2002/11/13 09:48:29  soltau
    Changes for optional words

    Revision 1.3.4.16  2002/06/26 11:57:55  fuegen
    changes for new directory structure support and code cleaning

    Revision 1.3.4.15  2002/04/29 13:29:38  metze
    Redesign of LM interface (LingKS)

    Revision 1.3.4.14  2002/04/26 14:53:01  soltau
    removed commentChar

    Revision 1.3.4.13  2002/04/26 11:43:17  soltau
    Added svocabIndex

    Revision 1.3.4.12  2002/02/01 14:17:21  fuegen
    moved stypes.h into slimits.h

    Revision 1.3.4.11  2001/11/30 14:48:03  soltau
    svocabAdd: Added argument dictX

    Revision 1.3.4.10  2001/06/28 10:48:24  soltau
    Added Load/save routines for binary files to support fast decoder startup

    Revision 1.3.4.9  2001/04/27 07:37:53  metze
    Separated svocab and svmap

    Revision 1.3.4.8  2001/04/23 13:47:21  soltau
    beautified

    Revision 1.3.4.7  2001/03/23 13:42:08  metze
    Mapping for lnX implemented

    Revision 1.3.4.6  2001/03/17 17:38:55  soltau
    Introduced unkString in svmap

    Revision 1.3.4.5  2001/03/16 13:34:49  soltau
    *** empty log message ***

    Revision 1.3.4.4  2001/03/16 14:45:21  metze
    Made endSVX etc. configureable

    Revision 1.3.4.3  2001/03/15 15:42:59  metze
    Re-implementation of LM-Lookahead

    Revision 1.3.4.2  2001/02/27 15:10:28  metze
    LModelNJD and LMLA work with floats

    Revision 1.3.4.1  2001/02/02 16:08:42  soltau
    moved typedefs to sglobal

    Revision 1.3  2001/01/25 11:00:51  soltau
    temporary checkin to make Christian happy

    Revision 1.2  2001/01/11 17:53:21  soltau
    Added Vocab Mapper

    Revision 1.1  2001/01/10 18:47:37  soltau
    Initial revision


   ======================================================================== */

#ifndef _svocab
#define _svocab

#include <limits.h>
#include "slimits.h"
#include "sglobal.h"
#include "list.h"
#include "dictionary.h"
#include "ngramLM.h"

/* ========================================================================
    Search Vocabulary Word
   ======================================================================== */

struct SWord_S {
  char*   name;             /* word name                                */
  SVX     dictX;            /* dictionary word index 
			       a value of SVX_MAX indicate a word candidate,
			       a word that is in a language model, but not
			       not yet in the vocabulary.
			    */
  UCHAR   fillerTag;        /* A filler word is a word that doesn't effect 
			       the lm state in the current theory.
			    */
};

/* ========================================================================
    Search Vocabulary
   ======================================================================== */

typedef struct LIST(SWord) SWordList;

struct SVocab_S {
  char*        name;
  int          useN;
  SWordList    list;
  Dictionary*  dictP;

  struct {                  /* mandatory vocabulary words        */
    char*      start;       /* start of sentence                 */
    char*      end;         /* end of sentence                   */
    char*      nil;         /* NIL word                          */
  } string;

};

/* ========================================================================
    Module Interface
   ======================================================================== */

extern int SVocab_Init();
extern SVX svocabIndex (SVocab* svocabP, char* name);
extern SVX svocabAdd   (SVocab *svocabP, char* name, UCHAR fillerTag, 
			int mode,int dictX);
extern int svocabLoad  (SVocab *svocabP, FILE* fp);
extern int svocabSave  (SVocab *svocabP, FILE* fp);

#endif
