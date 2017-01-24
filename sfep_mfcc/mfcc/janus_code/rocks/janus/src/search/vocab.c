/* ========================================================================
    JANUS      Speech- to Speech Translation
               ------------------------------------------------------------
               Object: Recognizer Vocabulary
               ------------------------------------------------------------
               
    Author  :  Michael Finke
    Module  :  vocab.c
    Date    :  $Id: vocab.c 2390 2003-08-14 11:20:32Z fuegen $
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
   Revision 3.10  2003/08/14 11:20:24  fuegen
   Merged changes on branch jtk-01-01-15-fms (jaguar -> ibis-013)

   Revision 3.9.20.3  2002/06/26 11:57:58  fuegen
   changes for new directory structure support and code cleaning

   Revision 3.9.20.2  2002/02/13 15:36:50  metze
   Made name read-only

   Revision 3.9.20.1  2002/02/04 14:24:27  metze
   Changed interface to wordPuts

   Revision 3.9  2000/11/14 12:35:26  janus
   Merging branch jtk-00-10-27-jmcd.

   Revision 3.8.34.2  2000/11/08 17:35:21  janus
   Finished making changes required for compilation under 'gcc -Wall'.

   Revision 3.8.34.1  2000/11/06 10:50:42  janus
   Made changes to enable compilation under -Wall.

   Revision 3.8  2000/08/16 11:51:25  soltau
   *** empty log message ***

   Revision 3.8  1998/12/09 16:11:44  tschaaf
   Added the functionality of deleting word from a vocab-object
   new functons are:
     vocabDeleteItf
     vocabDelete
      There are some rules to use this command:
      1) delete Variants befor Main-Forms
         example "the" and "the(2)" exist;
         so first remove "the(2)" and then "the"
         in the other case you get an FATAL message.
      2) It is not allowed to delete the words '(' , ')' and '$'
      3) You have to create a (new) search-Object after deleting words
         a) because the Index of the words may have changed
         b) the search are build over the old vocab
            and modifications are not noticed
     isVariantInVocab
      is a support function
      Checks existens of a variant of vocabular entry vwordX
      0 if no variant of vwordX is in the rest (behinde vwordX) vocab
      or index of first variant that point to vwordX

   renamed function
     index2realPronounciationIndex (old index2realIndex
       given the word index, return the real word index
       this is the index of the main form of a word
       example "the" is the main form of "the(2)"

   Revision 3.7  1998/07/31 12:19:19  kries
   -map flag

 * Revision 3.6  1998/02/20  08:49:54  kemp
 * Added index2realIndex. Supply the word index, get the real word index
 * back (to find out whether two words are really variants of the same
 * root form)
 *
 * Revision 3.5  1997/08/27  10:16:32  monika
 * added add_vocab method for Bernahrd
 *
   Revision 3.4  1997/07/30 13:22:57  kemp
   Changed location of #include "malloc.h" to be the last include

 * Revision 3.3  1997/07/18  17:57:51  monika
 * gcc-clean
 *
   Revision 3.2  1997/06/17 17:41:43  westphal
   *** empty log message ***

    Revision 2.6  1996/12/09  09:03:59  monika
    fixed bug with WE,WB for xwt-contexts

    Revision 2.5  1996/07/08  13:53:09  bsuhm
    Added methods activate/deactivate to vocabulary object

    Revision 2.4  1996/07/03  13:18:42  monika
    changes to work with 64K vocabulary

    Revision 2.3  96/03/07  04:05:42  04:05:42  finkem (Michael Finke)
    removed old invalid assert statement
    
    Revision 2.2  1996/02/27  00:02:35  finkem
    XWorboundaries modified and fixed problems with words
    not being in the dictionary

    Revision 2.1  1996/02/17  19:25:21  finkem
    used real xword tags when building context models

    Revision 2.0  1996/01/31  04:52:23  finkem
    new senoneSet setup, janus compatible read function and build
    xword triphones now while reading the vocab.

    Revision 1.12  1996/01/06  16:25:47  pgeutner
    added classX and vword configure function

    Revision 1.11  1995/09/22  04:37:32  finkem
    added missing return code in vocabRead

    Revision 1.10  1995/09/22  01:59:21  finkem
    introduced useXwt flag in the ConfigureItf

    Revision 1.9  1995/09/06  07:45:48  kemp
    *** empty log message ***

    Revision 1.8  1995/08/27  22:36:23  finkem
    *** empty log message ***

    Revision 1.7  1995/08/22  13:42:41  monika
    make changes required to have vocab work without dictionary
    and cleaned up some info messages

    Revision 1.6  1995/08/17  17:09:20  rogina
    *** empty log message ***

    Revision 1.5  1995/08/10  07:57:19  finkem
    *** empty log message ***

    Revision 1.4  1995/08/06  21:03:42  finkem
    *** empty log message ***

    Revision 1.3  1995/08/03  14:40:58  finkem
    *** empty log message ***

    Revision 1.2  1995/07/25  22:33:21  finkem
    *** empty log message ***

    Revision 1.1  1995/07/25  13:49:57  finkem
    Initial revision


  ========================================================================== */


char vocabRCSVersion[] = "$Id: vocab.c 2390 2003-08-14 11:20:32Z fuegen $";


#include "common.h"
#include "itf.h"
#include "vocab.h"
#include "word.h"


/* ========================================================================
    Forward Declaration
   ======================================================================== */

extern TypeInfo vwordInfo;

/* ========================================================================
    VWord
   ======================================================================== */
/* ------------------------------------------------------------------------
    vwordInit
   ------------------------------------------------------------------------ */

static int vwordInit( VWord* vwordP, ClientData name)
{
  assert( vwordP);

  /* CHANGE by bernhard */
  vwordP->active    =  1;

  vwordP->spellS    =  strdup((char *) name);
  vwordP->realX     =  0;
  vwordP->realPronounciationX =  0;
  vwordP->classX    = -1;
  vwordP->segN      =  0;
  vwordP->segXA     =  NULL;
  vwordP->tagXA     =  NULL;
  vwordP->modelXA   =  NULL;
  vwordP->xwbModelX = -1;
  vwordP->xweModelX = -1;
  return TCL_OK;
}

/* ------------------------------------------------------------------------
    vwordDeinit
   ------------------------------------------------------------------------ */

static int vwordDeinit( VWord * vwordP)
{
  assert( vwordP);

  /* vwordP->segXA & vwordP->tagXA are pointers copied from an AModelSet item.
     Do not free them here... */

  if ( vwordP->modelXA) { free( vwordP->modelXA); vwordP->modelXA = NULL; }
  if ( vwordP->spellS)  { free( vwordP->spellS);  vwordP->spellS  = NULL; }
  return TCL_OK;
}

/* ------------------------------------------------------------------------
    vwordPutsItf
   ------------------------------------------------------------------------ */

static int vwordPutsItf( ClientData cd, int argc, char *argv[])
{
  VWord * vwordP = (VWord*)cd;
  int         ac             = argc-1;

  if (itfParseArgv( argv[0], &ac,  argv+1,  1,
      NULL) != TCL_OK) return TCL_ERROR;


  if ( vwordP->spellS) {
    int sN = vwordP->segN;
    int sX;

    itfAppendResult( "%s {realX: %d}", vwordP->spellS, vwordP->realX);
    itfAppendResult(" {");
    for ( sX = 0; sX < sN; sX++) itfAppendResult(" %d",vwordP->segXA[sX]);
    itfAppendResult("} {");

    for ( sX = 0; sX < sN; sX++) {
      itfAppendResult(" %d",vwordP->modelXA[sX]);
    }
    itfAppendResult("} %d %d", vwordP->xwbModelX, vwordP->xweModelX);
  }
  return TCL_OK;
}

/* ------------------------------------------------------------------------
    vwordConfigureItf
   ------------------------------------------------------------------------ */

static int vwordConfigureItf( ClientData cd, char *var, char *val)
{
  VWord*        vwordP = (VWord*)cd;
  if (! vwordP) return TCL_ERROR;

  if (! var) {
    ITFCFG(vwordConfigureItf,cd,"realX");
    ITFCFG(vwordConfigureItf,cd,"realPronounciationX");
    ITFCFG(vwordConfigureItf,cd,"classX");
    ITFCFG(vwordConfigureItf,cd,"segN");
    /* CHANGE by bernhard */
    ITFCFG(vwordConfigureItf,cd,"active");  
    return TCL_OK;
  }
  ITFCFG_Int    ( var,val,"realX",  vwordP->realX,       1);
  ITFCFG_Int    ( var,val,"realPronounciationX",  vwordP->realPronounciationX,       1);
  ITFCFG_Int    ( var,val,"classX", vwordP->classX,      0);
  ITFCFG_Int    ( var,val,"segN",   vwordP->segN,        1);
  /* CHANGE by bernhard */
  ITFCFG_Int    ( var,val,"active", vwordP->active,      0);
  ERROR("unknown option '-%s %s'.\n", var, val ? val : "?");
  return TCL_ERROR;
}

/* ------------------------------------------------------------------------
    vocabCreateMonoPhoneModelXA
   ------------------------------------------------------------------------ */

static int* vocabCreateMonoPhoneModelXA( Vocab * vocabP)
{ 
  int*  monoPhoneModelXA = malloc( vocabP->phonesN * sizeof (int));
  int   monoX;
  int   dummytag = 0;
  Word  word;

  wordInit( &word, amodelSetPhones(vocabP->amodelP), 
                   amodelSetTags(  vocabP->amodelP));

  word.phoneA  = &monoX;
  word.tagA    = &dummytag;
  word.itemN   =  1;

  for (monoX = 0; monoX < vocabP->phonesN; monoX ++) {

    int erg = amodelSetGet(vocabP->amodelP,&word,0,0);
    if (erg < 0) ERROR("amodelSetGet returned %d for monophone %d modelX (%s)\n",
                        erg,monoX,wordPuts(&word, 1));
    monoPhoneModelXA[monoX] = erg;
  }
  return monoPhoneModelXA;
}

/* ========================================================================
    Vocab
   ======================================================================== */

static Vocab vocabDefault;

/* ------------------------------------------------------------------------
    vocabCreate
   ------------------------------------------------------------------------ */

int vocabInit( Vocab* vocabP, char* name, Dictionary* dictP, 
                                          AModelSet*  amodelP,
                                          int         useXwt,
	                                  int         mapPronounciations)
{
  vocabP->name     = strdup(name);
  vocabP->useN     = 0;
  
  listInit( (List*)&(vocabP->vwordList), &vwordInfo, sizeof(VWord), 500);

  vocabP->vwordList.init   = (ListItemInit  *) vwordInit;
  vocabP->vwordList.deinit = (ListItemDeinit*) vwordDeinit;
  
  vocabP->startWordX       = 0;
  vocabP->finishWordX      = 0;
  vocabP->silenceWordX     = 0;
  vocabP->firstWordX       = 0;
  vocabP->dictP            = NULL;
  vocabP->amodelP          = NULL;
  vocabP->xwbModelP        = NULL;
  vocabP->xweModelP        = NULL;

  vocabP->phonesM          = 0;
  vocabP->phonesN          = 0;
  vocabP->monoPhoneModelXA = NULL;
  vocabP->useXwt           = vocabDefault.useXwt;
  vocabP->mapPronounciations = mapPronounciations;
  vocabP->commentChar      = vocabDefault.commentChar;

  if ((dictP != NULL) && (amodelP != NULL)) {

    vocabP->dictP            = dictP;
    vocabP->amodelP          = amodelP;
    vocabP->xwbModelP        = xwmodelSetCreate(name,amodelP);
    vocabP->xweModelP        = xwmodelSetCreate(name,amodelP);

    vocabP->useXwt           =  useXwt;
    vocabP->phonesN          =  dictGetPhoneN(dictP);
    vocabP->phonesM          = (useXwt && dictP->xwTags) ? 4 : 1;
    vocabP->monoPhoneModelXA =  vocabCreateMonoPhoneModelXA(vocabP);

    link( vocabP->dictP,     "Dictionary");
    link( vocabP->amodelP,   "AModelSet");
    link( vocabP->xwbModelP, "XWModelSet");
    link( vocabP->xweModelP, "XWModelSet");
  }
  else {
    WARN("Vocab \"%s\" can only be used for testing LMs - not search\n",
          vocabP->name);
  }
  return TCL_OK;
}

Vocab* vocabCreate( char* name, Dictionary* dictP, AModelSet* amodelP,
                                int useXwt,int  mapPronounciations)
{ 
  Vocab* vocabP    = malloc(sizeof(Vocab));

  if (! vocabP || vocabInit( vocabP, name, dictP, amodelP,
                                                  useXwt, mapPronounciations) != TCL_OK) {
     if ( vocabP) free( vocabP);
     ERROR("Failed to allocate Vocab object '%s'.\n", name);
     return NULL; 
  }
  return vocabP;
}
/* ------------------------------------------------------------------------

   Build info needed for XWT:

     For the combination of each _word_ with every monophone the
     allophone-index for the word's last phoneme is requested,
     giving one we-table. After creating a new we-table it is inserted
     into a list of the uniq we-tables, and the resulting index
     is written to a special variable in the word's representation
     of the search. modelXA is not overwritten.

   ------------------------------------------------------------------------ */

static int vocabBuildXwtInfo(Vocab * vocabP, int vocabX)
{
  AModelSet*  amodelP   = vocabP->amodelP;
  XWModelSet* xwbModelP = vocabP->xwbModelP;
  XWModelSet* xweModelP = vocabP->xweModelP;
  int         phoneN    = vocabP->phonesN;

  VWord*      vwordP    = vocabP->vwordList.itemA + vocabX;
  int         segN      = vwordP->segN;
  int         fullXA[60000];
  int         phoneX;
  Word        word;

  int         saveActive = 0; /* for xwt-cache */

  assert(phoneN < 60000);

  wordInit( &word, amodelSetPhones(vocabP->amodelP),
                   amodelSetTags(  vocabP->amodelP));

  if ( vocabP->useXwt) {
    int    segXA[100];
    int    tagXA[100];
    int    d = 1;

    assert(segN < 98);
    if (amodelP->contextCache) {
      saveActive = amodelP->contextCache->active;
      amodelP->contextCache->active = 1;
    }

    memcpy( segXA+1, vwordP->segXA, sizeof(int) * segN);
    memcpy( tagXA+1, vwordP->tagXA, sizeof(int) * segN);

    tagXA[0]      = vocabP->dictP->weTags; /* HIER */
    tagXA[segN+1] = vocabP->dictP->wbTags; /* HIER */
 
    word.phoneA   = segXA;
    word.tagA     = tagXA;
    word.itemN    = segN+1;
    
    for ( phoneX = 0; phoneX < phoneN; phoneX++) {
      segXA[0]       = phoneX;
      fullXA[phoneX] = amodelSetGet(amodelP,&word,-1,segN-1);
    }
    vwordP->xwbModelX = xwmodelSetAdd( xwbModelP,d*phoneN,fullXA);

    word.phoneA  = segXA+1;
    word.tagA    = tagXA+1;

    for ( phoneX = 0; phoneX < phoneN; phoneX++) {
      segXA[segN+1]  = phoneX;
      fullXA[phoneX] = amodelSetGet(amodelP,&word,-segN+1,1);
    }
    vwordP->xweModelX = xwmodelSetAdd( xweModelP,d*phoneN,fullXA);
    if (amodelP->contextCache) {
      amodelP->contextCache->active = saveActive;
    }
  }
  else {
    for ( phoneX = 0; phoneX < phoneN; phoneX++) {
      fullXA[phoneX] = vwordP->modelXA[0];
    }
    vwordP->xwbModelX = xwmodelSetAdd( xwbModelP,phoneN,fullXA);

    for ( phoneX = 0; phoneX < phoneN; phoneX++) {
      fullXA[phoneX] = vwordP->modelXA[segN-1];
    }
    vwordP->xweModelX = xwmodelSetAdd( xweModelP,phoneN,fullXA);
  }
  return TCL_OK;
}

/* ------------------------------------------------------------------------
    index2realPronounciationIndex     
    given the word index, return the real word index
    this is the index of the main form of a word
    example "the" is the main form of "the(2)"
   ------------------------------------------------------------------------ */

int index2realPronounciationIndex(Vocab *vocabP, int index) {
  VWord *vwordP;
  if (index < 0 || index >= vocabP->vwordList.itemN) return -1;
  vwordP = vocabP->vwordList.itemA + index;
  return(vwordP->realPronounciationX);
}

/* ------------------------------------------------------------------------
    vocabRead
   ------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------
    vocabAdd     add a new word to the vocabulary
   ------------------------------------------------------------------------ */

static int vocabAdd( Vocab* vocabP, char *spellP)
{
  char*  paren;
  int    vwordX = vocabIndex(vocabP, spellP);
  VWord* vwordA;

  if ( vwordX != NILVWORD) return     vwordX;
  else vwordX  = listNewItem( (List*)&(vocabP->vwordList), spellP);

  vwordA = vocabP->vwordList.itemA; 

  /* compute real word index when this is a pronunciation variant */

  if ((paren = strchr(spellP+1,'('))) /* ) this comment keeps emacs happy */ {

    spellP[paren-spellP] = '\0';
    vwordA[vwordX].realPronounciationX = vocabIndex(vocabP,spellP);
    vwordA[vwordX].realX = vocabP->mapPronounciations ? vwordA[vwordX].realPronounciationX : vwordX;
    spellP[paren-spellP] = '(';

    if (vwordA[vwordX].realX < 0) {
      ERROR("Variant '%s' occured before base form.\n",spellP); 
      vwordA[vwordX].realX = vwordA[vwordX].realPronounciationX = vwordX;
    }
  }        
  else vwordA[vwordX].realX = vwordA[vwordX].realPronounciationX = vwordX;

  if (vocabP->dictP != NULL) {

    int erg = dictGet( vocabP->dictP, spellP, &(vwordA[vwordX].segN), 
                     &(vwordA[vwordX].segXA), &(vwordA[vwordX].tagXA));

    if (erg < 0) {
      if ( (strcmp (spellP,"$") == 0 ) || 
           (strcmp (spellP,"(") == 0 ) || (strcmp (spellP,")")  == 0 ) ) {
        FATAL("'%s' is REQUIRED in a dictionary '%s'.\n",spellP,
              vocabP->dictP->name);
      }
      ERROR("Vocab word '%s' is not in dictionary '%s'.\n",
             spellP,vocabP->dictP->name);
      listDelete((List*)&(vocabP->vwordList), spellP);      
      return vwordX;
    }
    else {
      int segN = vwordA[vwordX].segN;

      if (segN <= 0) {
        ERROR("vocabAdd: word %s has %d segments\n", spellP, segN);
        return vwordX;
      }
      else {
        Word   word;
        int    segX;

        wordInit( &word, amodelSetPhones(vocabP->amodelP),
                         amodelSetTags(  vocabP->amodelP));
        word.phoneA  = vwordA[vwordX].segXA;
        word.tagA    = vwordA[vwordX].tagXA;
        word.itemN   = segN;

        vwordA[vwordX].modelXA = (int*)malloc( segN * sizeof(int));

        for (segX=0; segX < segN; segX++) {

          vwordA[vwordX].modelXA[segX] = amodelSetGet(vocabP->amodelP,
                                                     &word,-segX,segN-segX-1); 
        }
      }
    }
    if ( vocabP->amodelP) vocabBuildXwtInfo(vocabP, vwordX);
  }
  return vwordX;
}

static int vocabAddItf( ClientData clientData, int argc, char *argv[])
{
  Vocab* vocabP = (Vocab*)clientData;
  char*  name   =  NULL;
  int    ac     =  argc - 1;
  int    idx;

  if ( itfParseArgv( argv[0], &ac, argv+1, 1,
      "<name>", ARGV_STRING,NULL,&name, NULL, "spelling",
       NULL) != TCL_OK) return TCL_ERROR;

  if ((idx = vocabAdd( vocabP, name)) == NILVWORD) return TCL_ERROR;
  else                                     return TCL_OK;
}


static int vocabRead( Vocab* vocabP, char* vwordListFile, char* fillerListFile)
{
  int n;

  /* comments are lines that start with a single ; character; it need not
     be the first character in the line, but has to be separated from the
     rest of the comment by a space. In other words: it is not possible to
     have a word that has the spelling ";" but words that do start with
     a ; like ;semicolon are possible
  */
 
  vocabP->silenceWordX = vocabAdd(vocabP,"$");

  if (vocabP->dictP != NULL) 
    assert( vocabP->vwordList.itemA[vocabP->silenceWordX].segN == 1);
 
  if ( fillerListFile != 0 && strlen(fillerListFile) > 0) {
    if ((n = freadAdd((ClientData)vocabP, fillerListFile,
                       vocabP->commentChar, vocabAddItf)) < 0)
         return TCL_ERROR;
  }
 
  vocabP->startWordX  = vocabAdd(vocabP,"(");
  vocabP->finishWordX = vocabAdd(vocabP,")");
  vocabP->firstWordX  = vocabP->vwordList.itemN;

  if ((n = freadAdd((ClientData)vocabP, vwordListFile,
                     vocabP->commentChar, vocabAddItf)) < 0)
       return TCL_ERROR;
  else return TCL_OK;
}

/* ------------------------------------------------------------------------
    vocabDel     remove a word from to the vocabulary
   ------------------------------------------------------------------------ */

/* Checks existens of a variant of Vocabular entry vwordX 
   0 if no variant of vwordX is in the rest (behinde vwordX) vocab
   or index of first variant that point to vwordX */
static int isVariantInVocab (Vocab* vocabP, int vwordX) {

    VWord* vwordA = vocabP->vwordList.itemA;
    int    itemN  = vocabP->vwordList.itemN;
    int    idx;

    assert(vwordX >= 0);

    /* variants only "after" vwordX possible */
    for (idx = vwordX +1;  idx < itemN; idx++) {
      if (vwordX == vwordA[idx].realX) {
	return idx;
      }
      if (vwordX == vwordA[idx].realPronounciationX) {
	return idx;
      }
    }
    return 0;  
}

static int vocabDelete( Vocab* vocabP, char *spellP)
{
  int    vwordX = vocabIndex(vocabP, spellP);
  VWord* vwordA;
  
  int    idx;
  int    itemN;

  if ( (strcmp (spellP,"$") == 0 ) || 
       (strcmp (spellP,"(") == 0 ) || (strcmp (spellP,")")  == 0 ) ) {
    ERROR("It is not allowed to delete '%s' from a Vocab (%s).\n",spellP,
	  vocabP->name);
    return TCL_ERROR;
  }

  if ( vwordX == NILVWORD) {
    WARN("Word %s not in vocab %s\n",spellP, vocabP->name);
    return TCL_OK;
  }

  /* acces to the vocabulary list */
  vwordA = vocabP->vwordList.itemA;

  /* check if we remove a main form and a variant is still in vocab */
  if ( (idx = isVariantInVocab(vocabP,vwordX)) ) {
    FATAL("Want to remove main form (%s) but variant (%s) still exist!\n", 
	  spellP, vwordA[idx].spellS);
    return TCL_ERROR;
  }

  /* renumber the Vocab Indices:
     Words "before" the deleted don't move (see list.c !)
     Words "after"  the deleted word will ne shift by one.
     We have to renumber the indices behinde the deleted word. 
     All indices bigger than vwordX will shift by one */
  itemN  = vocabP->vwordList.itemN;
  for (idx = vwordX +1;  idx < itemN; idx++) {
    if (vwordX < vwordA[idx].realX) {
      vwordA[idx].realX --;
    }
    if (vwordX < vwordA[idx].realPronounciationX) {
      vwordA[idx].realPronounciationX --;
    }
  }

  /* remove the word from the vocab */
  return (listDelete((List*)&(vocabP->vwordList), spellP));
}

/* There are some rules to use this command:
   1) delete Variants befor Main-Forms
      example "the" and "the(2)" exist; 
      so first remove "the(2)" and then "the"
      in the other case you get an FATAL message.
   2) It is not allowed to delete the words '(' , ')' and '$'
   3) You have to create a (new) search-Object after deleting words
      a) because the Index of the words may have changed
      b) the search are build over the old vocab 
         and modifications are not noticed                                  */
static int vocabDeleteItf( ClientData clientData, int argc, char *argv[])
{
  Vocab* vocabP = (Vocab*)clientData;
  char*  name   =  NULL;
  int    ac     =  argc - 1;
  int    idx;

  if ( itfParseArgv( argv[0], &ac, argv+1, 1,
      "<name>", ARGV_STRING,NULL,&name, NULL, "spelling",
       NULL) != TCL_OK) return TCL_ERROR;

  if ((idx = vocabDelete( vocabP, name)) == NILVWORD) return TCL_ERROR;
  else                                     return TCL_OK;
}



/* CHANGE by bernhard: added implementation of (de)activation method here */

/* ------------------------------------------------------------------------
    vocabActivate: activate word <name> for search
   ------------------------------------------------------------------------ */

int vocabActivate ( Vocab* vocabP, char* name)
{ 
  int idx = listIndex((List*)&(vocabP->vwordList), name, 0);
 
  if (idx < 0) { return TCL_ERROR;
  } else { 
    vocabP->vwordList.itemA[idx].active = 1; 
    return TCL_OK;
  }
}

static ClientData vocabCreateItf (ClientData cd, int argc, char* argv[])
{ 
  Vocab*      vocabP;
  int         ac             = argc-1;
  char*       wordFileName   = NULL;
  char*       fillerFileName = NULL;

  Dictionary* dictP          = NULL;
  AModelSet*  amodelP        = NULL;
  int         useXwt         = vocabDefault.useXwt;
  int         mapPronounciations = 1;

  assert (cd == cd); /* unused, keep compiler quiet */

  if (itfParseArgv( argv[0], &ac,  argv+1,  1,
     "vocablist",   ARGV_STRING, NULL, &wordFileName,   NULL, 
     "filename for vocabulary",
     "-fillerlist", ARGV_STRING, NULL, &fillerFileName, NULL,
     "filename for fillers",
     "-dictionary", ARGV_OBJECT, NULL, &dictP, "Dictionary",
     "phoneme dictionary object to use for search",
     "-acousticModel", ARGV_OBJECT, NULL, &amodelP, "AModelSet",
     "acoustic model to use for search",
     "-useXwt", ARGV_INT, NULL, &useXwt, NULL, "use xword triphones",
     "-map", ARGV_INT, NULL, &mapPronounciations, NULL, "map pronounciations on baseform (default on)",
      NULL) != TCL_OK) return NULL;
              
  if ( (vocabP = vocabCreate(argv[0], dictP, amodelP, useXwt, mapPronounciations))) {


    if ( vocabRead( vocabP, wordFileName, fillerFileName) == TCL_ERROR) {
      vocabFree(vocabP);
      return NULL;
    }
  }
  return (ClientData) vocabP;
}

/* ------------------------------------------------------------------------
    vocabFree
   ------------------------------------------------------------------------ */

int vocabLinkN( Vocab* vocabP)
{
  int  useN = listLinkN((List*)&(vocabP->vwordList)) - 1;
  if ( useN < vocabP->useN) return vocabP->useN;
  else                      return useN;
}

int vocabDeinit( Vocab* vocabP)
{

  assert( vocabP);

  if ( vocabLinkN( vocabP)) {
    SERROR("Vocab '%s' still in use by other objects.\n", vocabP->name);
    return TCL_ERROR;
  }
  
  if (vocabP->name != NULL)     { free (vocabP->name); vocabP->name = NULL; }
  if (vocabP->monoPhoneModelXA) { free(vocabP->monoPhoneModelXA); }

  if (vocabP->dictP)      unlink( vocabP->dictP,     "Dictionary");
  if (vocabP->amodelP)    unlink( vocabP->amodelP,   "AModelSet");

  if (vocabP->xwbModelP)  unlink( vocabP->xwbModelP, "XWModelSet");
  if (vocabP->xweModelP)  unlink( vocabP->xweModelP, "XWModelSet");

  vocabP->dictP     = NULL;
  vocabP->amodelP   = NULL;
  vocabP->xwbModelP = NULL;
  vocabP->xweModelP = NULL;

  return listDeinit( (List*)&(vocabP->vwordList));
}

int vocabFree(Vocab* vocabP)
{
  if (vocabDeinit(vocabP)!=TCL_OK) return TCL_ERROR;
  free(vocabP);
  return TCL_OK;
}

static int vocabFreeItf (ClientData cd)
{
  return vocabFree((Vocab*)cd);
}

/* ------------------------------------------------------------------------
    vocabPutsItf
   ------------------------------------------------------------------------ */

static int vocabPutsItf( ClientData cd, int argc, char* argv[])
{ 
  Vocab* vocabP  = (Vocab*) cd;
  return listPutsItf( (ClientData)&(vocabP->vwordList), argc, argv);
}

/* ------------------------------------------------------------------------
    vocabConfigureItf
   ------------------------------------------------------------------------ */

static int vocabConfigureItf( ClientData cd, char *var, char *val)
{
  Vocab*        vocabP = (Vocab*)cd;
  if (! vocabP) vocabP = &vocabDefault;

  if (! var) {
    ITFCFG(vocabConfigureItf,cd,"name");
    ITFCFG(vocabConfigureItf,cd,"useN");
    ITFCFG(vocabConfigureItf,cd,"phonesN");
    ITFCFG(vocabConfigureItf,cd,"silenceWord");
    ITFCFG(vocabConfigureItf,cd,"startWord");
    ITFCFG(vocabConfigureItf,cd,"finishWord");
    ITFCFG(vocabConfigureItf,cd,"firstWord");
    ITFCFG(vocabConfigureItf,cd,"useXwt");
    ITFCFG(vwordConfigureItf,cd,"mapPronounciations");
    return listConfigureItf((ClientData)&(vocabP->vwordList), var, val);
  }
  ITFCFG_CharPtr( var,val,"name",       vocabP->name,         1);
  ITFCFG_Int    ( var,val,"useN",       vocabP->useN,         1);
  ITFCFG_Int    ( var,val,"phonesN",    vocabP->phonesN,      1);
  ITFCFG_Int    ( var,val,"silenceWord",vocabP->silenceWordX, 1);
  ITFCFG_Int    ( var,val,"startWord",  vocabP->startWordX,   1);
  ITFCFG_Int    ( var,val,"finishWord", vocabP->finishWordX,  1);
  ITFCFG_Int    ( var,val,"firstWord",  vocabP->firstWordX,   1);
  ITFCFG_Int    ( var,val,"useXwt",     vocabP->useXwt,       0);
  ITFCFG_Int    ( var,val,"mapPronounciations",  vocabP->mapPronounciations,       1);
  return listConfigureItf((ClientData)&(vocabP->vwordList), var, val);
}

/* ------------------------------------------------------------------------
    vocabAccess
   ------------------------------------------------------------------------ */

static ClientData vocabAccessItf( ClientData cd, char *name, TypeInfo **ti)
{ 
  Vocab* vocabP  = (Vocab*)cd;

  if (*name == '.') { 
    if (name[1]=='\0') {
       if ( vocabP->dictP)     itfAppendElement( "dictionary");
       if ( vocabP->amodelP)   itfAppendElement( "amodelSet");
       if ( vocabP->xwbModelP) itfAppendElement( "xwbModelSet");
       if ( vocabP->xweModelP) itfAppendElement( "xweModelSet");
       *ti = NULL;
    }
    else { 

      if (vocabP->dictP && ! strcmp( name+1, "dictionary")) {
        *ti = itfGetType("Dictionary");
        return (ClientData)vocabP->dictP;
      }
      else if (vocabP->amodelP && ! strcmp( name+1, "amodelSet")) {
        *ti = itfGetType("AModelSet");
        return (ClientData)vocabP->amodelP;
      }
      else if (vocabP->xwbModelP && ! strcmp( name+1, "xwbModelSet")) {
        *ti = itfGetType("XWModelSet");
        return (ClientData)vocabP->xwbModelP;
      }
      else if (vocabP->xweModelP && ! strcmp( name+1, "xweModelSet")) {
        *ti = itfGetType("XWModelSet");
        return (ClientData)vocabP->xweModelP;
      }
    }
  }
  return listAccessItf( (ClientData)&(vocabP->vwordList), name, ti);
}

/* ------------------------------------------------------------------------
    vocabIndex
   ------------------------------------------------------------------------ */

VWordX vocabIndex( Vocab* vocabP,  char* name) 
{
  int idx = listIndex((List*)&(vocabP->vwordList), name, 0);
 
  if (idx < 0) {
    if (strcmp(name,"<s>" )==0) {
      MSGCLEAR(NULL);
      return listIndex((List*)&(vocabP->vwordList), "(", 0);
    }
    else if (strcmp(name,"</s>")==0) {
      MSGCLEAR(NULL);
      return listIndex((List*)&(vocabP->vwordList), ")", 0);
    }
    else return NILVWORD;
  }
  return idx;
}

static int vocabIndexItf( ClientData clientData, int argc, char *argv[] )
{
  Vocab* vocabP = (Vocab*)clientData;
  return listName2IndexItf((ClientData)&(vocabP->vwordList), argc, argv);
}

/* ------------------------------------------------------------------------
    vocabName
   ------------------------------------------------------------------------ */

char* vocabName( Vocab* vocabP, int i) 
{ return (i < 0 || i >= vocabP->vwordList.itemN) ? "(null)" : 
                        vocabP->vwordList.itemA[i].spellS; }

static int vocabNameItf( ClientData clientData, int argc, char *argv[] )
{
  Vocab* vocabP = (Vocab*)clientData;
  return listIndex2NameItf((ClientData)&(vocabP->vwordList), argc, argv);
}

/* ------------------------------------------------------------------------
    vocabNumber
   ------------------------------------------------------------------------ */

int vocabNumber( Vocab* vocabP) { return vocabP->vwordList.itemN; }



static int vocabActivateItf( ClientData clientData, int argc, char *argv[] )
{
  Vocab* vocabP = (Vocab*)clientData;
  char* name = argv[1];
  if (argc < 1) WARN("nonsense call of vocabActivateItf\n");
  return vocabActivate(vocabP, name);
}


/* ------------------------------------------------------------------------
    vocabDectivate: deactivate word <name> for search
   ------------------------------------------------------------------------ */

int vocabDeactivate ( Vocab* vocabP, char *name)
{ 
  int idx = listIndex((List*)&(vocabP->vwordList), name, 0);
 
  if (idx < 0) { return TCL_ERROR;
  } else { 
    vocabP->vwordList.itemA[idx].active = 0; 
    return TCL_OK;
  }
}

static int vocabDeactivateItf( ClientData clientData, int argc, char *argv[] )
{
  Vocab* vocabP = (Vocab*)clientData;
  char* name = argv[1];
  if (argc < 1) WARN("nonsense call of vocabDeactivateItf\n");
  return vocabDeactivate(vocabP, name);

}


/* ========================================================================
    Vocab ITF Initialization
   ======================================================================== */

Method vwordMethod[] = 
{ 
  { "puts",   vwordPutsItf,   "displays the contents of a vocab entry" },
  { NULL,         NULL,           NULL                           } 
};

TypeInfo vwordInfo = { "VWord",   /* name of type                            */
		       0,         /* size of type                            */
		       -1,        /* number of paramete rs                   */
		       vwordMethod,/* set of functions that work on this type*/
		       NULL,      /* creator function                        */
		       NULL,      /* destructor function                     */
		       vwordConfigureItf,
		       0,         /* subtype accessing function              */
		       itfTypeCntlDefaultNoLink,

                       "Vocab <my_vocab> <wordfilename> <fillerfilename>\n" } ;

Method vocabMethod[] = 
{ 
  /* CHANGE by bernhard: 3 methods to (de)activate and add words */
  { "activate",   vocabActivateItf,   "activates indexed vocab entry"  },
  { "deactivate", vocabDeactivateItf, "deactivates indexed vocab entry"  },
  { "add",    vocabAddItf,    "add a vocab entry"  },
  { "delete", vocabDeleteItf, "remove a vocab entry \n"
                              "   (you have to create a new search!)"  },
  { "puts",   vocabPutsItf,   "displays the contents of a vocabulary"  },
  { "index",  vocabIndexItf,  "returns indices of named vocab words"   },
  { "name",   vocabNameItf,   "returns names of indexed vocab entries" },
  {  NULL,    NULL,            NULL                                    } 
};

TypeInfo vocabInfo = { "Vocab",         /* name of type               */
		       0,               /* size of type               */
		       -1,              /* number of parameters       */
		       vocabMethod,     /* functions that work on this type */
		       vocabCreateItf,  /* creator function                 */
		       vocabFreeItf,    /* destructor function              */
		       vocabConfigureItf, /* configure function             */
		       vocabAccessItf,  /* subtype accessing function       */
		       NULL,

              "A Vocab is the list of words the recognizer can recognize\n"
              "To build a Vocab instance you have to specify:\n"
              "vocablist       filename for vocabulary\n"
              "-fillerlist     filename for fillers\n"
              "-dictionary     phoneme dictionary object to use for search\n"
              "-acousticModel  acoustic model to use for search\n"} ;
                       
static int vocabInitialized = 0;  

int Vocab_Init()
{
  if (! vocabInitialized) {
    if ( XWModel_Init() != TCL_OK) return TCL_ERROR;

    vocabDefault.name                 = NULL;
    vocabDefault.useN                 = 0;
    vocabDefault.vwordList.itemN      = 0;
    vocabDefault.vwordList.blkSize    = 100;
    vocabDefault.startWordX           = 0;
    vocabDefault.finishWordX          = 0;
    vocabDefault.silenceWordX         = 0;
    vocabDefault.firstWordX           = 0;
    vocabDefault.commentChar          = ';';

    itfNewType ( &vocabInfo );
    vocabInitialized = 1;
  }
  return (TCL_OK);
}
