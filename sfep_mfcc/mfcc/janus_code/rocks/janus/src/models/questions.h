/* ========================================================================
    JANUS      Speech- to Speech Translation
               ------------------------------------------------------------
               Object: Phoneme Set Maintainance Functions
               ------------------------------------------------------------

    Author  :  Ivica Rogina & Michael Finke
    Module  :  questions.h
    Date    :  $Id: questions.h 700 2000-11-14 12:35:27Z janus $
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
    Revision 4.3  2000/11/14 12:35:25  janus
    Merging branch jtk-00-10-27-jmcd.

    Revision 4.2.30.1  2000/11/06 10:50:39  janus
    Made changes to enable compilation under -Wall.

    Revision 4.2  2000/08/27 15:31:27  jmcd
    Merging branch 'jtk-00-08-24-jmcd'.

    Revision 4.1.4.1  2000/08/25 22:19:45  jmcd
    Just checking.

    Revision 4.1  2000/01/12 10:12:49  fuegen
    remove bug for AND and OR questions (answers were wrong)

    Revision 4.0  1998/11/30 18:07:14  fuegen
    *** empty log message ***

    Revision 3.2  1997/06/17 17:41:43  westphal
    *** empty log message ***

    Revision 1.5  96/01/31  05:00:06  finkem
    finallay questions on tags and word-type argument for questions
    
    Revision 1.4  1995/10/30  00:23:19  finkem
    new questionAsk

    Revision 1.3  1995/08/18  08:23:17  finkem
    *** empty log message ***

    Revision 1.2  1995/07/25  22:33:21  finkem
    *** empty log message ***

    Revision 1.1  1995/07/25  13:49:57  finkem
    Initial revision

 
   ======================================================================== */

#ifdef __cplusplus
extern "C" {
#endif

#ifndef _questions
#define _questions

#include "word.h"

/* ========================================================================
    Question
   ======================================================================== */

typedef struct
{
  char	*name;		   /* name of this question                         */
  int   leftContext;       /* position of this question's leftmost context 
                              (could be right of the phone)                 */
  int   rightContext;      /* position of this question's rightmost context 
                              (could be left of the phone)                  */
  int   clauseN;           /* number of clauses for this question           */
  char	***charFunc;	   /* [clauseX][contextX][phoneX]                   */
  int   **tagsBitSet;      /* [clauseX][contextX] (is a bitset)             */

  int   tagOperation;      /* 0=conjunction, 1=disjunction */

  struct QuestionSet_S* qsetP;

} Question;

/* ========================================================================
    QuestionSet
   ======================================================================== */

typedef struct LIST(Question) QuestionList;

typedef struct QuestionSet_S
{
  char*        name;       /* name of this questionSet object */
  int          useN;

  QuestionList list;

  Phones*      phones;
  PhonesSet*   phonesSet;
  Tags*        tags;
  int          padPhoneX;

  int          tagOperation;  /* 0=conjunction, 1=disjunction */

  char         commentChar;   /* a character to itentify comment lines */

} QuestionSet;

/* ========================================================================
    Module Interface
   ======================================================================== */

extern int questionInit(   Question* QU, char* name);
extern int questionDeinit( Question* QU);
extern int questionDefine( Question* QU, char* name, QuestionSet* QS);

extern int questionAnswer( Question* QU, Word* wP, int left, int  right);

extern int questionSetInit( QuestionSet* questionSet, char *name, 
                          Phones *phones, PhonesSet *phonesSet, Tags *tags);

extern QuestionSet *questionSetCreate( char *name, 
                                   Phones *phones, PhonesSet *phonesSet, 
                                   Tags *tags);

extern int   questionSetDeinit( QuestionSet* questionSet);
extern int   questionSetFree(   QuestionSet* questionSet);
extern int   questionSetLinkN(  QuestionSet* questionSet);

extern int   questionSetIndex(  QuestionSet* questionSet, char* name);
extern char* questionSetName(   QuestionSet* questionSet, int i);
extern int   questionSetNumber( QuestionSet* questionSet);
extern int   questionSetAdd(    QuestionSet* questionSet, char* name);
extern int   questionSetRead(   QuestionSet *questionSet, char *filename);
extern int   questionSetWrite(  QuestionSet *questionSet, char *filename);

extern int questionSetAnswer( QuestionSet *questionSet, int index, 
                              Word* wP,    int left, int right);

extern int Questions_Init ();

#endif

#ifdef __cplusplus
}
#endif
