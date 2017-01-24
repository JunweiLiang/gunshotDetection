/* ========================================================================
    JANUS      Speech- to Speech Translation
               ------------------------------------------------------------
               Object: Acoustic Dictionary
               ------------------------------------------------------------

    Author  :  Ivica Rogina
    Module  :  dictionary.h
    Date    :  $Id: dictionary.h 2390 2003-08-14 11:20:32Z fuegen $
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
    Revision 3.5  2003/08/14 11:20:14  fuegen
    Merged changes on branch jtk-01-01-15-fms (jaguar -> ibis-013)

    Revision 3.4.56.1  2001/06/28 10:48:21  soltau
    Added Load/save routines for binary files to support fast decoder startup

    Revision 3.4  2000/08/27 15:31:11  jmcd
    Merging branch 'jtk-00-08-24-jmcd'.

    Revision 3.3.4.1  2000/08/25 22:19:43  jmcd
    Just checking.

    Revision 3.3  1997/07/25 17:38:31  tschaaf
    gcc / DFKI Clean

    Revision 3.2  1997/06/17 17:41:43  westphal
    *** empty log message ***

    Revision 2.1  96/02/17  19:25:21  finkem
    added wbTags, weTags and xwTags to be used in the search and vocab
    
    Revision 2.0  1996/01/31  05:20:55  finkem
    new senoneSet setup, new dictionary words and pron. variants

    Revision 1.6  1995/11/17  20:14:06  finkem
    added external declaration of dictName

    Revision 1.5  1995/08/27  22:36:11  finkem
    *** empty log message ***

    Revision 1.4  1995/08/15  16:10:34  rogina
    *** empty log message ***

    Revision 1.3  1995/08/01  22:16:03  finkem
    *** empty log message ***

    Revision 1.2  1995/07/25  22:33:21  finkem
    *** empty log message ***

    Revision 1.1  1995/07/25  13:49:57  finkem
    Initial revision

 
   ======================================================================== */

#ifdef __cplusplus
extern "C" {
#endif

#ifndef _dictionary
#define _dictionary

#include "list.h"
#include "word.h"
#include "phones.h"
#include "tags.h"

/* ========================================================================
    DictWord
   ======================================================================== */

typedef struct
{
  char*    name;

  Word     word;
  int      varX;

} DictWord;

/* ========================================================================
    Dictionary
   ======================================================================== */

typedef struct LIST(DictWord) DictWordList;

typedef struct
{
  char        *name;      /* name of the dictionary */
  int          useN;      /* use counter            */

  DictWordList list;

  Phones      *phones;
  Tags        *tags;

  int          wbTags;    /* word begin tag           */
  int          weTags;    /* word end tag             */
  int          xwTags;    /* other xword tags to test */

  char         commentChar;

} Dictionary;

/* ========================================================================
    Module Interface
   ======================================================================== */

extern int         Dictionary_Init ( void );
extern int         dictInit(   Dictionary* dict, char* name, 
                                           Phones* phones, Tags* tags);
extern Dictionary* dictCreate( char* name, Phones* phones, Tags* tags);

extern int   dictLinkN     (Dictionary* dict);
extern int   dictDeinit    (Dictionary* dict);
extern int   dictFree      (Dictionary* dict);
extern int   dictGetPhone  (Dictionary* dict, char* name);
extern int   dictGetPhoneN (Dictionary* dict);
extern int   dictIndex     (Dictionary* dict,  char* phone);
extern char* dictName      (Dictionary* dict, int i);
extern int   dictGet       (Dictionary *dict, char *spell, int *phonesN, 
			    int **phones, int **tags);
extern int   dictGetFast   (Dictionary *dict, int idx, int *phonesN, 
			    int **phones, int **tags);
extern int   dictLoad     (Dictionary *dict, FILE* fp);
extern int   dictSave     (Dictionary *dict, FILE* fp);

#endif


#ifdef __cplusplus
}
#endif
