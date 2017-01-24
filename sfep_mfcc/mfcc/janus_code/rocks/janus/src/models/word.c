/* ========================================================================
    JANUS      Speech- to Speech Translation
               ------------------------------------------------------------
               Object: Word
               ------------------------------------------------------------

    Author  :  Michael Finke
    Module  :  word.c
    Date    :  $Id: word.c 3245 2010-05-17 18:24:11Z metze $
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
    Revision 3.6  2003/08/14 11:20:19  fuegen
    Merged changes on branch jtk-01-01-15-fms (jaguar -> ibis-013)

    Revision 3.5.20.1  2002/02/04 14:23:45  metze
    Changed interface to wordPuts

    Revision 3.5  2000/11/14 12:35:27  janus
    Merging branch jtk-00-10-27-jmcd.

    Revision 3.4.30.2  2000/11/08 17:35:43  janus
    Finished making changes required for compilation under 'gcc -Wall'.

    Revision 3.4.30.1  2000/11/06 10:50:42  janus
    Made changes to enable compilation under -Wall.

    Revision 3.4  2000/08/16 11:51:55  soltau
    free -> TclFree

    Revision 3.3  1997/07/25 17:35:03  tschaaf
    gcc / DFKI - Clean

    Revision 3.2  1997/06/17 17:41:43  westphal
    *** empty log message ***

     Revision 1.3  96/06/07  17:32:15  rogina
     fixed the usage of wordPtr from wordModelName() and wordPuts() at the same time
     
     Revision 1.2  1996/02/19  15:25:38  kemp
     Some comments.

     Revision 1.1  1996/01/31  05:11:37  finkem
     Initial revision

 
   ======================================================================== */

#include "common.h"
#include "word.h"
#include "itf.h"
#include "error.h"

char wordRCSVersion[]= 
           "@(#)1$Id: word.c 3245 2010-05-17 18:24:11Z metze $";


/* ------------------------------------------------------------------------
    Forward Declaration
   ------------------------------------------------------------------------ */

extern TypeInfo wordInfo;

/* ========================================================================
    Word
   ======================================================================== */

static int wordPutsItf (ClientData cd, int argc, char *argv[]);

/* ------------------------------------------------------------------------
    wordInit   initialize word structure
   ------------------------------------------------------------------------ */

int wordInit (Word* word, Phones* PS, Tags* TP)
{
  assert(word);
  word->phoneA  =  NULL;
  word->tagA    =  NULL;
  word->itemN   =  0;
  word->phonesP =  PS;
  word->tagsP   =  TP;
  return TCL_OK;
}

/* ------------------------------------------------------------------------
    wordDeinit  deinitialize word structure
   ------------------------------------------------------------------------ */

int wordDeinit (Word* word)
{
  if (!word) return TCL_OK;
  if ( word->phoneA) { free(word->phoneA); word->phoneA = NULL; }
  if ( word->tagA)   { free(word->tagA);   word->tagA   = NULL; }
  return TCL_OK;
}

/* ------------------------------------------------------------------------
    wordCheck  check whether a word specification has a certain Phones
               and Tags specification such that the indices match.
   ------------------------------------------------------------------------ */

int wordCheck( Word* wP, Phones* PS, Tags* TP)
{
  if ( wP->phonesP && PS && wP->phonesP != PS) {
    ERROR("Phones do not match.\n");
    return TCL_ERROR;
  }
  if ( wP->tagsP && TP && wP->tagsP != TP) {
    ERROR("Tags do not match.\n");
    return TCL_ERROR;
  }
  return TCL_OK;
}

/* ------------------------------------------------------------------------
    wordConfigureItf
   ------------------------------------------------------------------------ */

static int wordConfigureItf( ClientData cd, char *var, char *val)
{
  Word* word = (Word*)cd;
  if (! word) return TCL_ERROR;

  if (! var) {
    ITFCFG(wordConfigureItf,cd,"itemN");
    return TCL_OK;
  }
  ITFCFG_Int(     var,val,"itemN",        word->itemN,       1);
  ERROR("Unknown option '-%s %s'.\n", var, val ? val : "?");
  return TCL_ERROR;
}

/* ------------------------------------------------------------------------
    wordPutsItf
   ------------------------------------------------------------------------ */

/* ------------------------------------------------------------------------
    wordPuts()
    This function will return a pointer to a memory region (allocated by
    the function itself, but NOT to be freed by the caller) that contains
    the dictionary transcription of the word 'word'.
   ------------------------------------------------------------------------ */

char* wordPuts (Word* word, int showtags)
{
  static char* wordPtr = NULL;
  DString dstr;
  int     i;
  /* ------------------------------------------ */
  /* Handle free-ing of our static memory block */
  /* ------------------------------------------ */
  if ( wordPtr) free( wordPtr);

  dstringInit( &dstr);

  /* --------------------------------------------- */
  /* for each phone in the word's transcription do */
  /* --------------------------------------------- */
  for ( i = 0; i < word->itemN; i++) {
    /* ----------------------------------------- */
    /* non-tagged phone needs no TCL bracing { } */
    /* ----------------------------------------- */
    if (showtags && word->tagA[i]) dstringAppend(&dstr," {");
    /* ----------------------------------------- */
    /* append phone transcription (call index to */
    /* string function phonesName)               */
    /* ----------------------------------------- */
    dstringAppend(&dstr," %s", phonesName(word->phonesP,word->phoneA[i]));
    /* --------------------------------------------- */
    /* if we have tags (up to #of bits-in-a-int tags */
    /* are supported), add them all now:             */
    /* --------------------------------------------- */
    if (showtags && word->tagA[i]) {
      unsigned int j;
      int t = word->tagA[i];
      for ( j = 0; j < 8*sizeof(int); j++) {
        if ( t & (1 << j)) {
          dstringAppend(&dstr," %s", tagsName(word->tagsP,j));
        }
      }
      dstringAppend(&dstr,"}");
    }
  }
  wordPtr = strdup( dstringValue(&dstr));
  dstringFree(&dstr);
  return wordPtr;
}

static int wordPutsItf( ClientData cd, int argc, char *argv[])
{
  int   ac   =  argc-1;
  Word* word = (Word*)cd;

  if ( itfParseArgv( argv[0], &ac, argv+1, 0, NULL) != TCL_OK)
       return TCL_ERROR;

  itfAppendResult( "%s", wordPuts( word, 1));
  return TCL_OK;
}

/* ------------------------------------------------------------------------
    wordGetItf
   ------------------------------------------------------------------------ */

int wordGetItf( Word* wordP, char* value)
{
  int     argc;
  char**  argv;

  if ( Tcl_SplitList( itf, value, &argc, &argv) == TCL_OK) {
    int i;

    if ( wordP->itemN > 0) {
      if ( wordP->itemN != argc) {
        SERROR("Expected Word of size %d but got size %d.\n", 
                wordP->itemN, argc);
        return TCL_ERROR;
      }
    }
    else {
      wordP->phoneA = (int*)calloc( argc, sizeof(int));
      wordP->tagA   = (int*)calloc( argc, sizeof(int));
      wordP->itemN  =  argc;
    }
    for ( i = 0; i < argc; i++) {
      int     tokc;
      char**  tokv;

      if ( Tcl_SplitList( itf, argv[i], &tokc, &tokv) == TCL_OK) {
        int j;

        if ( tokc < 1) {
          SERROR("Missing phone name.\n");
          Tcl_Free((char*)argv); Tcl_Free((char*)tokv);
          return TCL_ERROR;
        }
        wordP->phoneA[i] = listIndex((List*)&(wordP->phonesP->list), 
                                     (ClientData)tokv[0], 0);
        wordP->tagA[i]   = 0;
        if ( wordP->phoneA[i] < 0) {
          SERROR("Can't find phone '%s'.\n", tokv[0]);
          Tcl_Free((char*)argv); Tcl_Free((char*)tokv);
          return TCL_ERROR;
        }        
        for ( j = 1; j < tokc; j++) {
          int t = listIndex((List*)&(wordP->tagsP->list), 
                            (ClientData)tokv[j], 0);
          if ( t < 0) {
            SERROR("Can't find tag '%s'.\n", tokv[j]);
            Tcl_Free((char*)argv); Tcl_Free((char*)tokv);
            return TCL_ERROR;
          }
          wordP->tagA[i] |= (1 << t);
        }
        Tcl_Free((char*)tokv);
      }
    }
    Tcl_Free((char*)argv);
    return TCL_OK;
  }
  return TCL_ERROR;
}

int getWord( ClientData cd, char* key, ClientData result,
	     int *argcP, char *argv[])
{
   Word* ptaP = (Word*)result;
   
   if ((*argcP < 1) || 
       ( wordGetItf(ptaP, argv[0]) != TCL_OK)) return -1;
   (*argcP)--; return 0;
}

/* ------------------------------------------------------------------------
    wordModelName
   ------------------------------------------------------------------------ */

char* wordModelName( Word* word, int left, int right)
{
  static char* wordPtr = NULL;
  DString dstr;
  int     i, k=0;

  if ( left>0 || right<0 || left>right || right-left+1 > word->itemN) {
    ERROR("Left/Right context specification %s %d %d is bogus.\n",
           wordPuts(word, 1),left,right);
    return NULL;
  }

  if ( wordPtr) free( wordPtr);

  dstringInit( &dstr);

  i = -left;
  dstringAppend(&dstr,"%s",phonesName(word->phonesP,word->phoneA[i]));
  if ( word->tagA[i]) {
    unsigned int j;
    int t = word->tagA[i], c=0;
    dstringAppend(&dstr,"<");
    for ( j = 0; j < 8*sizeof(int); j++) {
      if ( t & (1 << j)) {
        if (c++) dstringAppend(&dstr,"|");
        dstringAppend(&dstr,"%s", tagsName(word->tagsP,j));
      }
    }
    dstringAppend(&dstr,">");
  }
  dstringAppend(&dstr,"(");

  for ( i = 0; i < right-left+1; i++) {
    if ( i == -left) {dstringAppend(&dstr,"|"); k=0; }
    else {
      if (k++) dstringAppend(&dstr,",");
      dstringAppend(&dstr,"%s", phonesName(word->phonesP,word->phoneA[i]));
      if ( word->tagA[i]) {
	unsigned int j;
        int t = word->tagA[i], c=0;
        dstringAppend(&dstr,"<");
        for ( j = 0; j < 8*sizeof(int); j++) {
          if ( t & (1 << j)) {
            if (c++) dstringAppend(&dstr,"|");
            dstringAppend(&dstr,"%s", tagsName(word->tagsP,j));
          }
        }
        dstringAppend(&dstr,">");
      }
    }
  }
  dstringAppend(&dstr,")");
  wordPtr = strdup( dstringValue(&dstr));
  dstringFree(&dstr);
  return wordPtr;
}

/* ========================================================================
    Type Declaration
   ======================================================================== */

static Method wordMethod[] = 
{ 
  { "puts", wordPutsItf,   NULL },
  {  NULL,  NULL, NULL } 
};

TypeInfo wordInfo = { "Word", 0, 0, wordMethod, 
                       NULL, NULL,
                       wordConfigureItf, NULL,
                       itfTypeCntlDefaultNoLink, 
                      "Word with tagged phone transcription\n" };

static int wordInitialized = 0;

int Word_Init ( void )
{
  if (! wordInitialized) {
    if (Tags_Init()    != TCL_OK) return TCL_ERROR;
    if (Phones_Init()  != TCL_OK) return TCL_ERROR;

    itfNewType (&wordInfo);

    wordInitialized = 1;
  }
  return TCL_OK;
}
