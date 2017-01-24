/* ========================================================================
    JANUS      Speech- to Speech Translation
               ------------------------------------------------------------
               Object: hmeStream
               ------------------------------------------------------------

    Author  :  Juergen Fritsch
    Module  :  hmeStream.c
    Date    :  $Id: hmeStream.c 2390 2003-08-14 11:20:32Z fuegen $
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
    Revision 3.4  2003/08/14 11:20:20  fuegen
    Merged changes on branch jtk-01-01-15-fms (jaguar -> ibis-013)

    Revision 3.3.4.1  2002/06/26 12:26:50  fuegen
    go away, boring message

    Revision 3.3  2001/01/15 09:49:57  janus
    Merged Christian's version with WINDOWS support (branch jtk-00-11-14-fuegen)

    Revision 3.2.40.1  2001/01/12 15:16:55  janus
    necessary changes for running janus under WINDOWS

    Revision 3.2  1997/06/17 17:41:43  westphal
    *** empty log message ***

 * Revision 1.2  96/09/27  08:59:15  fritsch
 * *** empty log message ***
 * 

   ======================================================================== */

#include "common.h"
#include "itf.h"
#include "feature.h"
#include "phones.h"
#include "word.h"
#include "stream.h"
#include "mach_ind_io.h"
#include "tree.h"
#include "hme.h"
#include "hmeSet.h"
#include "hmeStream.h"


/* ======================================================================= */
/*                             Implementation                              */
/* ======================================================================= */


/* ----------------------------------------------------------------------- */
/*  hmeStreamCreate                                                        */
/* ----------------------------------------------------------------------- */
HmeStream  *hmeStreamCreate (char *name, Tree *tree, HmeSet *hmeSet) {

  HmeStream     *hmeStream = (HmeStream *) malloc (sizeof(HmeStream));

  hmeStream->name   = strdup(name);
  hmeStream->tree   = tree;
  hmeStream->hmeSet = hmeSet;
  
  return hmeStream;
}



/* ----------------------------------------------------------------------- */
/*  hmeStreamFree                                                          */
/* ----------------------------------------------------------------------- */
int  hmeStreamFree (HmeStream *hmeStream) {

  free(hmeStream->name);
  free(hmeStream);
  
  return TCL_OK;
}



/* ----------------------------------------------------------------------- */
/*  hmeStreamTag                                                           */
/* ----------------------------------------------------------------------- */
int  hmeStreamTag (ClientData cd, char *name) {
  
  HmeStream   *hmeStream = (HmeStream *) cd;
  return  hmeSetTagIndex(hmeStream->hmeSet,name);
}



/* ----------------------------------------------------------------------- */
/*  hmeStreamGet                                                           */
/* ----------------------------------------------------------------------- */
int  hmeStreamGet (ClientData cd, int tag, Word *wP, int left, int right) {

  char                 *tagName;
  register int         treeIdx;
  
  HmeStream   *hmeStream = (HmeStream *) cd;
  tagName = hmeStream->hmeSet->state[tag]->name;
  treeIdx = treeIndex(hmeStream->tree,tagName);
  return  treeGetModel(hmeStream->tree,treeIdx,wP,left,right);
}



/* ----------------------------------------------------------------------- */
/*  hmeStreamIndex                                                         */
/* ----------------------------------------------------------------------- */
int  hmeStreamIndex (ClientData cd, char *name) {

  HmeStream   *hmeStream = (HmeStream *) cd;
  return  hmeSetIndex((ClientData)(hmeStream->hmeSet),name);
}



/* ----------------------------------------------------------------------- */
/*  hmeStreamName                                                          */
/* ----------------------------------------------------------------------- */
char  *hmeStreamName (ClientData cd, int index) {

  HmeStream   *hmeStream = (HmeStream *) cd;
  return  hmeSetName((ClientData)(hmeStream->hmeSet),index);
}



/* ----------------------------------------------------------------------- */
/*  hmeStreamScore                                                         */
/* ----------------------------------------------------------------------- */
float  hmeStreamScore (ClientData cd, int idx, int frameX) {

  HmeStream   *hmeStream = (HmeStream *) cd;
  return  hmeSetScore(hmeStream->hmeSet,idx,frameX);
}



/* ----------------------------------------------------------------------- */
/*  hmeStreamAccu                                                          */
/* ----------------------------------------------------------------------- */
int  hmeStreamAccu (ClientData cd, int idx, int frameX, float factor) {

  HmeStream   *hmeStream = (HmeStream *) cd;
  return  hmeSetAccu(hmeStream->hmeSet,idx,frameX,factor);
}



/* ----------------------------------------------------------------------- */
/*  hmeStreamUpdate                                                        */
/* ----------------------------------------------------------------------- */
int  hmeStreamUpdate (ClientData cd) {

  HmeStream   *hmeStream = (HmeStream *) cd;
  return  hmeSetUpdate(hmeStream->hmeSet);
}



/* ----------------------------------------------------------------------- */
/*  hmeStreamFrameN                                                        */
/* ----------------------------------------------------------------------- */
int  hmeStreamFrameN (ClientData cd, int *from, int *shift, int *ready) {

  HmeStream   *hmeStream = (HmeStream *) cd;
  return  hmeSetFrameN(hmeStream->hmeSet,from,shift,ready);
}



/* ----------------------------------------------------------------------- */
/*  hmeStreamFesUsed                                                       */
/* ----------------------------------------------------------------------- */
FeatureSet  *hmeStreamFesUsed (ClientData cd) {

  HmeStream   *hmeStream = (HmeStream *) cd;
  return  hmeSetFesUsed(hmeStream->hmeSet);
}




/* ======================================================================= */
/*                 Itf/Tcl Interface Functions for HmeStream               */
/* ======================================================================= */


/* ----------------------------------------------------------------------- */
/*  hmeStreamCreateItf                                                     */
/* ----------------------------------------------------------------------- */
ClientData  hmeStreamCreateItf (ClientData cd, int argc, char *argv[]) {

  int               ac = argc-1;
  Tree              *tree;
  HmeSet            *hmeSet;
  
  if ( itfParseArgv( argv[0], &ac, argv+1, 0,
      "<tree>",   ARGV_OBJECT, NULL, &tree,   "Tree",   "context clustering tree",
      "<hmeSet>", ARGV_OBJECT, NULL, &hmeSet, "HmeSet", "set of CI/CD Hme's",
  NULL) != TCL_OK) return NULL;

  return  (ClientData)hmeStreamCreate(argv[0],tree,hmeSet);
}



/* ----------------------------------------------------------------------- */
/*  hmeStreamFreeItf                                                       */
/* ----------------------------------------------------------------------- */
int  hmeStreamFreeItf (ClientData cd) {

  return  hmeStreamFree ((HmeStream *) cd);
}



/* ----------------------------------------------------------------------- */
/*  hmeStreamPutsItf                                                       */
/* ----------------------------------------------------------------------- */
int  hmeStreamPutsItf (ClientData cd, int argc, char *argv[]) {

  HmeStream     *hmeStream = (HmeStream *) cd;
  
  itfAppendElement("%s",hmeStream->name);
  return TCL_OK;
}



/* ----------------------------------------------------------------------- */
/*  hmeStreamConfigureItf                                                  */
/* ----------------------------------------------------------------------- */
int  hmeStreamConfigureItf (ClientData cd,  char *var, char *val) {

  HmeStream          *hmeStream = (HmeStream *) cd;

  if (! var) {
    ITFCFG(hmeStreamConfigureItf,cd,"name");
    return TCL_OK;
  }
  ITFCFG_CharPtr (var,val,"name", hmeStream->name , 1);
  return TCL_OK;
}



/* ----------------------------------------------------------------------- */
/*  hmeStreamAccessItf                                                     */
/* ----------------------------------------------------------------------- */
ClientData  hmeStreamAccessItf (ClientData cd, char *name, TypeInfo **ti) {

  HmeStream       *hmeStream = (HmeStream *) cd;
  
  if (*name=='.') {
    if (name[1]=='\0') {
      itfAppendResult("tree hmeSet");
    }
    else { 
      if (!strcmp(name+1,"tree")) {
        *ti=itfGetType("Tree");
        return (ClientData)(hmeStream->tree);
      } else { 
        if (!strcmp(name+1,"hmeSet")) {
          *ti=itfGetType("HmeSet");
          return (ClientData)(hmeStream->hmeSet);
        }
      }
    }
  }
  *ti=NULL;
  return NULL;
}



/* ----------------------------------------------------------------------- */
/*  hmeStreamIndexItf                                                      */
/* ----------------------------------------------------------------------- */
int  hmeStreamIndexItf (ClientData cd, int argc, char *argv[]) {

  HmeStream      *hmeStream = (HmeStream *) cd;
  int            ac = argc-1;
  char           *name;
  
  if ( itfParseArgv( argv[0], &ac, argv+1, 0,
      "<name>", ARGV_STRING, NULL, &name, NULL, "name of Hme model",
  NULL) != TCL_OK) return TCL_ERROR;

  itfAppendResult("%d",hmeSetIndex((ClientData) hmeStream->hmeSet,name));
  return TCL_OK;
}



/* ----------------------------------------------------------------------- */
/*  hmeStreamNameItf                                                       */
/* ----------------------------------------------------------------------- */
int  hmeStreamNameItf (ClientData cd, int argc, char *argv[]) {

  HmeStream      *hmeStream = (HmeStream *) cd;
  int            ac = argc-1;
  int            index;
  
  if ( itfParseArgv( argv[0], &ac, argv+1, 0,
      "<index>", ARGV_INT, NULL, &index, NULL, "index of Hme model",
  NULL) != TCL_OK) return TCL_ERROR;

  itfAppendResult("%s",hmeSetName((ClientData) hmeStream->hmeSet,index));
  return TCL_OK;
}



/* ----------------------------------------------------------------------- */
/*  hmeStreamGetItf                                                        */
/* ----------------------------------------------------------------------- */
int  hmeStreamGetItf (ClientData cd, int argc, char *argv[]) {

  HmeStream      *hmeStream = (HmeStream *) cd;
  int            ac         = argc - 1;
  int            left=0,right=0,tag=0,index=0;
  Word           word;
  
  wordInit( &word, hmeStream->hmeSet->phones, NULL);
  if ( itfParseArgv( argv[0], &ac, argv+1, 0,
      "<node>",         ARGV_INT, NULL, &tag   , NULL , "HMM tag",
      "<word>",         ARGV_CUSTOM   , getWord, &word, NULL, "Word object",
      "<leftContext>",  ARGV_INT, NULL, &left  , NULL , "left  context",
      "<rightContext>", ARGV_INT, NULL, &right , NULL , "right context",
       NULL) != TCL_OK) {
    wordDeinit( &word);
    return TCL_ERROR;
  }  

  index = hmeStreamGet(cd,tag,&word,left,right);
  wordDeinit( &word);

  itfAppendResult("%d",index);
  return TCL_OK;
}



/* ----------------------------------------------------------------------- */
/*  hmeStreamScoreItf                                                      */
/* ----------------------------------------------------------------------- */
int  hmeStreamScoreItf (ClientData cd, int argc, char *argv[]) {

  HmeStream      *hmeStream = (HmeStream *) cd;
  char           *name      = NULL;
  int            frameX     = 0;
  int            index;
  int            ac         = argc - 1;

  if ( itfParseArgv( argv[0], &ac, argv+1, 0,
      "<name>",   ARGV_STRING, NULL, &name,   NULL, "name of a Hme model",
      "<frameX>", ARGV_INT,    NULL, &frameX, NULL, "index of feature vector",
    NULL) != TCL_OK) return TCL_ERROR;

  index = hmeSetIndex((ClientData) hmeStream->hmeSet,name);
  if (index < 0) return TCL_ERROR;
  
  itfAppendResult("%f",hmeStreamScore(cd,index,frameX));
  return TCL_OK;
}



/* ----------------------------------------------------------------------- */
/*  hmeStreamAccuItf                                                       */
/* ----------------------------------------------------------------------- */
int  hmeStreamAccuItf (ClientData cd, int argc, char *argv[]) {

  HmeStream      *hmeStream = (HmeStream *) cd;
  char           *name      = NULL;
  int            frameX     = 0;
  int            index;
  int            ac         = argc - 1;

  if ( itfParseArgv( argv[0], &ac, argv+1, 0,
      "<name>",   ARGV_STRING, NULL, &name,   NULL, "name of a Hme model",
      "<frameX>", ARGV_INT,    NULL, &frameX, NULL, "index of feature vector",
    NULL) != TCL_OK) return TCL_ERROR;

  index = hmeSetIndex((ClientData) hmeStream->hmeSet,name);
  if (index < 0) return TCL_ERROR;

  return  hmeStreamAccu(cd,index,frameX,1.0);
}



/* ----------------------------------------------------------------------- */
/*  hmeStreamUpdateItf                                                     */
/* ----------------------------------------------------------------------- */
int  hmeStreamUpdateItf (ClientData cd, int argc, char *argv[]) {

  return  hmeStreamUpdate(cd);
}




/* ======================================================================== */
/*                method declaration and module initialization              */
/* ======================================================================== */


static Method hmeStreamMethod[] = 
{
  { "puts",        hmeStreamPutsItf,        NULL},
  { "index",       hmeStreamIndexItf,       "index of a Hme model given its name"},
  { "name",        hmeStreamNameItf,        "name of a Hme model given its index"},
  { "get",         hmeStreamGetItf,         "index of a Hme model given a tagged phone sequence"},
  { "score",       hmeStreamScoreItf,       "compute HmeStream scores"},
  { "accu",        hmeStreamAccuItf,        "accumulate statistics to train Hme's" },
  { "update",      hmeStreamUpdateItf,      "update Hme parameters"},
  {  NULL,  NULL, NULL } 
} ;



TypeInfo hmeStreamInfo = {
                  "HmeStream", 0, -1, hmeStreamMethod, 
                  hmeStreamCreateItf, hmeStreamFreeItf,
                  hmeStreamConfigureItf, hmeStreamAccessItf,
                  itfTypeCntlDefaultNoLink,
                  "HME based neural network MAP stream\n" } ;


static Stream hmeStream = { "HmeStream", &hmeStreamInfo,
                            hmeStreamTag,
                            hmeStreamGet,
                            hmeStreamIndex,
                            hmeStreamName,
                            hmeStreamScore,
                            hmeStreamAccu,
                            hmeStreamUpdate,
                            hmeStreamFrameN,
                            hmeStreamFesUsed };



static int HmeStreamInitialized = 0;

/* ----------------------------------------------------------------------- */
/*  HmeStream_Init                                                         */
/* ----------------------------------------------------------------------- */
int HmeStream_Init () {
  
  if (HmeStreamInitialized) return TCL_OK;

  streamNewType (&hmeStream);
  itfNewType    (&hmeStreamInfo);
  
  HmeStreamInitialized = 1;
  return TCL_OK;
}


