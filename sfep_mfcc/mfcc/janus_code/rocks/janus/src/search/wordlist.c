/* ========================================================================
    JANUS      Speech- to Speech Translation
               ------------------------------------------------------------
               Object: Wordlist Module (part of search)
               ------------------------------------------------------------

    Author  :  Monika Woszczyna
    Module  :  wordlist.c
    Date    :  $Id: wordlist.c 700 2000-11-14 12:35:27Z janus $
    Remarks :  under development

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
   Revision 3.6  2000/11/14 12:35:27  janus
   Merging branch jtk-00-10-27-jmcd.

   Revision 3.5.28.2  2000/11/08 17:36:10  janus
   Finished making changes required for compilation under 'gcc -Wall'.

   Revision 3.5.28.1  2000/11/06 10:50:42  janus
   Made changes to enable compilation under -Wall.

   Revision 3.5  2000/09/12 15:06:30  janus
   Merged branch jtk-00-09-08-hyu.

   Revision 3.4.16.1  2000/09/12 01:18:08  hyu
   Added acoustic lattice rescoring from Michael Finke.

   Revision 3.4  2000/08/16 11:53:01  soltau
   Added warping function for worlist structure (for fast rover)
   Free -> TclFree

   Revision 3.3  1997/07/18 17:57:51  monika
   gcc-clean

   Revision 3.2  1997/06/17 17:41:43  westphal
   *** empty log message ***

   ======================================================================== */

#include "search.h"
#include "wordlist.h"

char wordlistRcsVersion[]= "@(#)1$Id: wordlist.c 700 2000-11-14 12:35:27Z janus $";


static WordList wordListDefault;

/*---------------------------------------------------------------------------
  wordListFree: free all structures allocated by the wordListAlloc functions,
                but not the main WL as part of the search or allocated
                by wordListCreate;
 ---------------------------------------------------------------------------*/
int wordListFree(WordList* wlP)
{
  WordNode* nodeP, *nnodeP;
  int       i;

  if (wlP->wordListPA) {
    if (wlP->isDummy) {
     for ( nodeP = wlP->wordListPA[0]; nodeP; nodeP = nnodeP) {
       nnodeP = nodeP->nextP;
       free( (char*)nodeP);
      }
    } else {
      for ( i = 0; i <= wlP->frameN; i++) {
        for ( nodeP = wlP->wordListPA[i]; nodeP; nodeP = nnodeP) {
           nnodeP = nodeP->nextP;
           free( (char*)nodeP);
        }
      }
    }
    free (wlP->wordListPA);
    wlP->wordListPA = NULL;
  }
  if (wlP->wordXA) {
       free(wlP->wordXA);
       wlP->wordXA = NULL;
  }
  return TCL_OK;
}

/* ==========================================================================
   Create/Destroy and Itf-functions
   ========================================================================= */

static int wordListDeinit (WordList * wlP) 
{
  wordListFree(wlP);
  if (wlP->name) free(wlP->name);
  wlP->name = NULL;
  return TCL_OK;
}

static int wordListDestroy (WordList * wlP)
{
  wordListDeinit(wlP);
  free(wlP);
  return TCL_OK; 
}

static int wordListDestroyItf (ClientData clientData)
{
  return (wordListDestroy( (WordList *) clientData));
}

int wordListInit (WordList * wlP, char* name, Vocab* vocabP)
{
 wlP->name = strdup(name);
 wlP->useN = 0;
 wlP->vocabP = vocabP;
 wlP->wordXA = NULL;
 wlP->wordListPA = NULL;
 wlP->minEFrameN = wordListDefault.minEFrameN;
 wlP->frameN = 0;
 wlP->isDummy = 0;

 return TCL_OK;
}

/* ------------------------------------------------------------------------
  wordListGrow   grow word list (append 0s) to match input signal
   ------------------------------------------------------------------------ */

void wordListGrow (WordList * wlP, int frameN)
{
  wlP->wordListPA = (WordNode **) realloc( wlP->wordListPA, 
					   sizeof(WordNode *) * (frameN+1));
  if (frameN > wlP->frameN) {
    memset( (char*) (wlP->wordListPA + wlP->frameN), 0, 
	    (frameN+1 - wlP->frameN) * sizeof(WordNode*));
  }
  wlP->frameN     = frameN;
}

int wordListCopy (WordList * toWlP, WordList * fromWlP)
{
 if (toWlP->wordListPA == fromWlP->wordListPA) {
   INFO ("not copying wordlist (source == dest)\n");
   assert (toWlP->wordXA == fromWlP->wordXA);
   return TCL_OK;
 }
 assert (toWlP->wordXA != fromWlP->wordXA);

 wordListFree(toWlP);

 fromWlP->useN ++;

 toWlP->wordXA     = fromWlP->wordXA;
 toWlP->wordListPA = fromWlP->wordListPA;
 toWlP->isDummy    = fromWlP->isDummy;
 toWlP->frameN     = fromWlP->frameN;

 return TCL_OK;
}

WordList * wordListCreate (char* name, Vocab* vocabP) 
{
 WordList* wlP = (WordList*) malloc(sizeof(WordList));

 if (! wlP || wordListInit(wlP,name,vocabP) != TCL_OK) {
   if (wlP) free (wlP);
   ERROR("Failed to allocate WordList object '%s'.\n",name);
   return NULL;
 }
 return wlP;
}

static ClientData wordListCreateItf (ClientData clientData, int argc, char *argv[])
{
  int       ac     =  argc - 1;
  Vocab*    vocabP =  NULL;

  assert (clientData==clientData); /*not used, keep compiler quiet */ 

  if ( itfParseArgv( argv[0], &ac, argv+1, 0,
      "<Vocab>", ARGV_OBJECT, NULL, &vocabP, "Vocab","vocabulary",
       NULL) != TCL_OK) return NULL;

  return (ClientData)wordListCreate(argv[0], vocabP);
}

static int wordListReadItf(ClientData clientData, int argc, char * argv[])
{
  FILE *fp;
  int ac = argc -1;
  char * fileName = NULL;
  WordList * wlP = (WordList *) clientData;
  WordNode * nodeP;
  char * tmpUseA;
  int vocabN = wlP->vocabP->vwordList.itemN;
  int wX, fX, tmp;
  char line[64000];


  if ( itfParseArgv( argv[0], &ac, argv+1, 0,
      "filename", ARGV_STRING, NULL, &fileName, NULL,"filename to read from",     
       NULL) != TCL_OK) return TCL_ERROR;

  if ((fp=fileOpen(fileName, "r"))==NULL) {
    ERROR( "Can't open wordlist file '%s' for reading.\n",fileName);
    return TCL_ERROR;
  }

  wordListFree(wlP); /* just make sure that we can write into this */
  fscanf(fp,"%d%*[\n]",&tmp); /* number of frames */

  wlP->frameN = tmp;  

  tmpUseA = (char*) malloc (sizeof(char) * vocabN+1);
  memset( (char*)tmpUseA,  0, (vocabN+1)  * sizeof(char));

  wlP->wordListPA = (WordNode **) malloc( sizeof(WordNode *) * (wlP->frameN +1));
  wlP->wordXA     = (WordX *) malloc (sizeof(WordX) * (vocabN +1));
  memset( (char*)wlP->wordListPA, 0, (wlP->frameN +1) * sizeof(WordNode*));

  while (fgets(line,2000,fp)) {
    int largc;
    char ** largv;

  
    if (Tcl_SplitList (itf, line, &largc, &largv) == TCL_OK) {
     int fX = -1;
     sscanf(largv[0],"%d",&fX);
     if (fX >= wlP->frameN) {
       ERROR("too many frames %d >= %d\n",fX,wlP->frameN);
       return TCL_ERROR;
     }
     wX = 1;
     if (largc > 1) {
        nodeP = (WordNode*) malloc(sizeof(WordNode));
        wlP->wordListPA[fX] = nodeP;
        wlP->wordListPA[fX]->wordX = vocabIndex(wlP->vocabP,largv[wX]); 
        if (nodeP->wordX >= wlP->vocabP->vwordList.itemN) {
           ERROR("don't know word '%s' in frame %d\n",largv[wX],fX);
           return TCL_ERROR;
        }
        for (wX = 2; wX < largc; wX ++) {
          nodeP->nextP = (WordNode*) malloc(sizeof(WordNode));
          nodeP = nodeP->nextP;
          nodeP->wordX = vocabIndex(wlP->vocabP,largv[wX]);
          if (nodeP->wordX >= wlP->vocabP->vwordList.itemN) {
             ERROR("don't know word '%s' in frame %d\n",largv[wX],fX);
             return TCL_ERROR;
          }
        }
        nodeP->nextP = NULL;
     }
     Tcl_Free ((char*) largv);
    }
  }

  fileClose(fileName,fp);
  wX = 0;
  for ( fX = 0; fX < wlP->frameN; fX++) {
    for ( nodeP = wlP->wordListPA[fX]; nodeP; nodeP = nodeP->nextP) {
      if (! tmpUseA[nodeP->wordX]) {
        tmpUseA[nodeP->wordX] = 1;
        wlP->wordXA[ wX++] = nodeP->wordX;
      }
    }
  }
  wlP->wordXA[wX] = NILWORD;

  free(tmpUseA);


  return TCL_OK;
}

static int wordListWriteItf(ClientData clientData, int argc, char * argv[])
{
  FILE *fp;
  int ac = argc -1;

  WordList * wlP = (WordList *) clientData;
  int fX;
  char* fileName = NULL;
  WordNode * wnP;

  if ( itfParseArgv( argv[0], &ac, argv+1, 0,
      "filename", ARGV_STRING, NULL, &fileName, NULL,"filename to write to",     
       NULL) != TCL_OK) return TCL_ERROR;

  if ((fp=fileOpen(fileName, "w"))==NULL) {
    ERROR( "Can't open wordlist file '%s' for writing.\n",fileName);
    return TCL_ERROR;
  }


  fprintf(fp,"%d\n",wlP->frameN);

  for (fX = 0; fX < wlP->frameN; fX ++) {
    if (wlP->wordListPA[fX]) {
      fprintf(fp,"%d ",fX);
      for (wnP = wlP->wordListPA[fX]; wnP; wnP=wnP->nextP) {
        fprintf(fp,"%s ",wlP->vocabP->vwordList.itemA[wnP->wordX].spellS);
      }
      fprintf(fp,"\n");
    }
  }

  fileClose (fileName,fp);

  return TCL_OK;
}

/* ------------------------------------------------------------------------
    wordListWarpItf
   ------------------------------------------------------------------------ */

static int wordListWarpItf( ClientData cd, int argc, char *argv[] )
{
  float           warp  = 1.0;
  int               ac  = argc -1;
  WordList*         wlP = (WordList*)cd;
  WordNode** wordListPA = NULL;
  int            frameX = 0;
  int            frameN = 0;

  if ( itfParseArgv( argv[0], &ac, argv+1, 0,
      "warp", ARGV_FLOAT, NULL, &warp, NULL ,"warp factor",
       NULL) != TCL_OK) return TCL_ERROR;

  frameN = (int) (warp * wlP->frameN + 0.5);
  
  if (NULL == (wordListPA = malloc( (frameN +1) * sizeof(WordNode*)))) {
    ERROR("allocation failure\n");
    return TCL_ERROR;
  }
  
  /* init new wordlistPA */
  for (frameX=0;frameX<=frameN;frameX++) {
    wordListPA[frameX]= NULL;
  }

  /*copy from old to new wordlist and warp */
  for (frameX=0;frameX<wlP->frameN;frameX++) {
    int newframeX       =  (int) (warp * frameX);
    WordNode* last_wnP  =  NULL;
    WordNode* wnP       =  NULL;
  
    for (wnP = wlP->wordListPA[frameX]; wnP;) { 
      WordNode* new_wnP  = NULL;
      WordNode* nextP    = wnP->nextP;

      if (NULL == ( new_wnP = malloc( sizeof(WordNode)))) {
	ERROR("allocation failure\n");
	return TCL_ERROR;
      }
      /* store first wordPointer */
      if (wordListPA[newframeX] == NULL) wordListPA[newframeX] = new_wnP; 

      new_wnP->wordX    = wnP->wordX;
      new_wnP->feFrameX = (int) (warp * wnP->feFrameX);
      new_wnP->leFrameX = (int) (warp * wnP->leFrameX);

      new_wnP->nextP    = NULL;
      if (last_wnP) {
	last_wnP->nextP = new_wnP;
      }
      last_wnP = new_wnP;

      free(wnP); 
      wnP= nextP;
    }
  }
  
  free (wlP->wordListPA); 
  wlP->wordListPA = wordListPA;

  wlP->frameN= frameN;

  return TCL_OK;

}

/* ------------------------------------------------------------------------
    wordListConfigureItf
   ------------------------------------------------------------------------ */

static int wordListConfigureItf( ClientData cd, char *var, char *val)
{
  WordList*    wlP = (WordList*)cd;
  if (! wlP)  wlP = &wordListDefault;

  if (! var) {

    ITFCFG(wordListConfigureItf,cd,"minEFrameN");
    ITFCFG(wordListConfigureItf,cd,"frameN");
    return TCL_OK;
  }
  ITFCFG_Float(    var,val,   "minEFrameN",      
                         wlP->minEFrameN, 0);
  ITFCFG_Int(      var,val,   "frameN",      
                         wlP->frameN, 1);
  return TCL_ERROR;
}

/* ------------------------------------------------------------------------
    wordListAccessItf
   ------------------------------------------------------------------------ */

static ClientData wordListAccessItf( ClientData cd, char *name, TypeInfo **ti)
{ 
  WordList* wlP = (WordList*)cd;

  if (*name == '.') { 
    if (name[1]=='\0') {
      itfAppendElement( "vocab");
      *ti = NULL; return NULL;
    }
    else { 
      if (! strcmp( name+1, "vocab")) {
        *ti = itfGetType("Vocab");
        return (ClientData)&(wlP->vocabP);
      }
    }
  }
  return NULL;
}

/* ------------------------------------------------------------------------
    wordListPutsItf
   ------------------------------------------------------------------------ */

static int wordListPutsItf (ClientData cd, int argc, char * argv[])
{
  int ac = argc -1;
  WordList * wlP = (WordList *) cd;
  int fX;
  WordNode * wnP;

  if ( itfParseArgv( argv[0], &ac, argv+1, 0,
       NULL) != TCL_OK) return TCL_ERROR;

  for (fX = 0; fX < wlP->frameN; fX ++) {
    itfAppendResult("{ ");
    for (wnP = wlP->wordListPA[fX]; wnP; wnP=wnP->nextP) {
        itfAppendResult("%s ",wlP->vocabP->vwordList.itemA[wnP->wordX].spellS);
    }
    itfAppendResult("}\n");
  }
  return TCL_OK;
}

/* ==========================================================================
   user interface definitions
   ========================================================================= */

static Method wordListMethod[] = {
  {"puts",  wordListPutsItf, NULL},
  {"read",   wordListReadItf, "add all words for one frame"},
  {"write", wordListWriteItf, "write wordlist to file"},
  {"warp",  wordListWarpItf,  "warp frame indices in wordlist (Why? -> ask Hagen)"},
  {NULL, NULL, NULL}
};

static TypeInfo wordListInfo = {
    "WordList", 0, -1, wordListMethod,
    wordListCreateItf, wordListDestroyItf,
    wordListConfigureItf, wordListAccessItf,
    NULL,
    "WordList to communicate between tree and flat pass of searches"
};

static int wordListInitialized = 0;

int WordList_Init()
{
  if (! wordListInitialized) {
     wordListDefault.minEFrameN = 4;
     wordListDefault.frameN = 0;

     itfNewType (&wordListInfo);

     wordListInitialized = 1;
  }
  return TCL_OK;
}

