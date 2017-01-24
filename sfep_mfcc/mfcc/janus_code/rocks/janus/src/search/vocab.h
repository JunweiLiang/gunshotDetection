/* ========================================================================
    JANUS      Speech- to Speech Translation
               ------------------------------------------------------------
               Object: Recognizer Vocabulary.
               ------------------------------------------------------------
               
    Author  :  Michael Finke
    Module  :  vocab.h
    Date    :  $Id: vocab.h 512 2000-08-27 15:31:32Z jmcd $
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
    Revision 3.7  2000/08/27 15:31:31  jmcd
    Merging branch 'jtk-00-08-24-jmcd'.

    Revision 3.6.2.1  2000/08/25 22:19:46  jmcd
    Just checking.

    Revision 3.6  2000/08/24 13:32:26  jmcd
    Merging changes from branch jtk-00-08-23-jmcd.

    Revision 3.5.2.1  2000/08/24 09:32:50  jmcd
    More revision fixes.

    Revision 3.6  1998/12/09 16:12:42  tschaaf
    renamed function
      index2realPronounciationIndex (old index2realIndex
        given the word index, return the real word index
        this is the index of the main form of a word
        example "the" is the main form of "the(2)"

    Revision 3.5  1998/07/31 12:19:19  kries
    -map flag

 * Revision 3.4  1998/02/20  08:49:44  kemp
 * Added index2realIndex
 *
 * Revision 3.3  1997/07/18  17:57:51  monika
 * gcc-clean
 *
    Revision 3.2  1997/06/17 17:41:43  westphal
    *** empty log message ***

    Revision 1.11  1996/07/08  13:53:44  bsuhm
    Added methods activate/deactivate to vocabulary object

    Revision 1.10  1996/07/03  13:19:26  monika
    changes to work with 64K vocabulary

    Revision 1.9  96/02/27  00:02:35  00:02:35  finkem (Michael Finke)
    XWorboundaries modified and fixed problems with words
    not being in the dictionary
    
    Revision 1.8  1996/02/17  19:25:21  finkem
    used real xword tags when building context models

    Revision 1.7  1996/01/06  16:25:47  pgeutner
    added classX and vword configure function

    Revision 1.6  1995/08/27  22:36:42  finkem
    *** empty log message ***

    Revision 1.5  1995/08/10  07:57:13  finkem
    *** empty log message ***

    Revision 1.4  1995/08/06  21:03:42  finkem
    *** empty log message ***

    Revision 1.3  1995/08/03  14:40:58  finkem
    *** empty log message ***

    Revision 1.2  1995/07/25  22:33:21  finkem
    *** empty log message ***

     Revision 1.1  1995/07/25  13:49:57  finkem
     Initial revision


   ========================================================================= */

#ifdef __cplusplus
extern "C" {
#endif

#ifndef _vocab
#define _vocab

#include "list.h"
#include "itf.h"
#include "dictionary.h"
#include "amodel.h"
#include "xwmodel.h"
        
/* ========================================================================
    VWord
   ======================================================================== */

typedef struct {

  char*   spellS;     /* (unique) spelling for word                         */
  int     realX;      /* real index of word (for pron. variants if configured as such) */
  int     realPronounciationX;      /* real index of word (for pron. variants)            */
  int     classX;     /* class index                                        */
  int     segN;       /* number of subsegments in word for new search       */
  int*    segXA;      /* gives the base phone as required for x-word modell */
  int*    tagXA;      /* gives tags like wb, we as required                 */
  int*    modelXA;    /* statesequence index array: one sequence_id per seg */

  int     xwbModelX;
  int     xweModelX;

  /* CHANGE by bernhard */
  short   active;     /* to turn word on/off for LM (search) purposes */

} VWord;

typedef unsigned short VWordX;

#define NILVWORD 65535

/* ========================================================================
    Vocab
   ======================================================================== */

typedef struct LIST(VWord) VWordList;

typedef struct {

  char*        name;
  int          useN;
  VWordList    vwordList;

  Dictionary*  dictP;
  AModelSet*   amodelP;

  XWModelSet*  xwbModelP;
  XWModelSet*  xweModelP;

  int          phonesM;
  int          phonesN;
  VWordX       startWordX;
  VWordX       finishWordX;
  VWordX       silenceWordX;
  VWordX       firstWordX;
  
  int*         monoPhoneModelXA;
  int          useXwt;       /* should not be here forever */
  int          mapPronounciations; /* default on, if off (==0) HELLO(3) is not mapped to HELLO for the LM */
  char         commentChar;

} Vocab;

/* ========================================================================
    Modul Interface
   ======================================================================== */

extern int    Vocab_Init(void);

extern int    vocabInit( Vocab* vocabP, char* name, Dictionary* dictP, 
                                                    AModelSet*  amodelP,
                                                    int         useXwt,
			                            int         mapPronounciations);
extern Vocab* vocabCreate( char* name, Dictionary* dictP,
                                       AModelSet*  amodelP,
                                       int         useXwt,
	                               int         mapPronounciations);

extern int    vocabLinkN(  Vocab* vocabP);
extern int    vocabDeinit( Vocab* vocabP);
extern int    vocabFree(   Vocab* vocabP);

extern VWordX vocabIndex(  Vocab* vocabP, char* name); 
extern char*  vocabName(   Vocab* vocabP, int i); 
extern int    vocabNumber( Vocab* vocabP);
extern int    index2realPronounciationIndex(Vocab *vocabP, int index);

/* CHANGE by bernhard */
extern int    vocabActivate( Vocab* vocabP, char* name);
extern int    vocabDeactivate( Vocab* vocabP, char* name);

#endif

#ifdef __cplusplus
}
#endif
