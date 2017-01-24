/* --------------------------------------------------------------------------
   WordList
 
   a wordlist is an interface between one search (usu tree pass of one search)
   and the flat pass of the same or a different search.
   It defines for each frame of an utterance, which words are allowed
   to start at this frame.
   
   When this module is complete,
   Much like a lattice, a wordlist can be 
     -- created 'empty', 
     -- initialized, 
     -- read from file
     -- written to file
     -- created from the backtrace of a search
   ------------------------------------------------------------------------- */
#include "search.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef _wordlist
#define _wordlist

typedef struct WordNode_S {

  WordX              wordX;
  short              feFrameX;
  short              leFrameX;
  struct WordNode_S *nextP;

} WordNode;

typedef struct WordList_S {
  char * name;
  int    useN;

  Vocab* vocabP;

  WordX*     wordXA;  /* NILWORD terminated list of all words in ffWordLstPA */
  WordNode** wordListPA; /* for each frame, linked list of all words that
                               should start in this frame */

  short minEFrameN;
  short frameN;

  int isDummy; /* 0= normal, 1= pointers for all frames point to same list */

} WordList;

extern int       WordList_Init (void);

extern WordList * wordListCreate (char* name, Vocab* vocabP);
extern int        wordListInit   (WordList * wlP, char* name, Vocab* vocabP);
extern int        wordListCopy   (WordList * toP, WordList * fromP);
extern int        wordListFree   (WordList * wlP);
extern void       wordListGrow   (WordList * toP, int frameN);

#endif

#ifdef __cplusplus
}
#endif
