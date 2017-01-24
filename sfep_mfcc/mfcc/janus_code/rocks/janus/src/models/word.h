/* ========================================================================
    JANUS      Speech- to Speech Translation
               ------------------------------------------------------------
               Object: Word
               ------------------------------------------------------------

    Author  :  Michael Finke
    Module  :  word.h
    Date    :  $Id: word.h 2390 2003-08-14 11:20:32Z fuegen $
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
    Revision 3.5  2003/08/14 11:20:19  fuegen
    Merged changes on branch jtk-01-01-15-fms (jaguar -> ibis-013)

    Revision 3.4.56.1  2002/02/04 14:24:15  metze
    Changed interface to wordPuts

    Revision 3.4  2000/08/27 15:31:31  jmcd
    Merging branch 'jtk-00-08-24-jmcd'.

    Revision 3.3.4.1  2000/08/25 22:19:46  jmcd
    Just checking.

    Revision 3.3  1997/07/25 17:35:03  tschaaf
    gcc / DFKI - Clean

    Revision 3.2  1997/06/17 17:41:43  westphal
    *** empty log message ***

     Revision 1.1  96/01/31  05:11:37  finkem
     Initial revision
     
 
   ======================================================================== */

#ifdef __cplusplus
extern "C" {
#endif

#ifndef _word
#define _word

#include "list.h"
#include "phones.h"
#include "tags.h"

/* ========================================================================
    Word
   ======================================================================== */

typedef struct
{
  int*     phoneA;
  int*     tagA;
  int      itemN;

  Phones*  phonesP;
  Tags*    tagsP;

} Word;

/* ========================================================================
    Module Interface
   ======================================================================== */

extern int Word_Init ( void );
extern int wordInit   (Word* word, Phones* PS, Tags* TP);
extern int wordDeinit (Word* word);
extern int wordCheck(  Word* word, Phones* PS, Tags* TP);

extern int   wordGetItf( Word* wordP, char* value);
extern char* wordPuts(   Word* word, int showtags);
extern int   getWord(    ClientData cd, char* key, ClientData result,
	                 int *argcP, char *argv[]);

extern char* wordModelName( Word* word, int left, int right);

#endif

#ifdef __cplusplus
}
#endif
