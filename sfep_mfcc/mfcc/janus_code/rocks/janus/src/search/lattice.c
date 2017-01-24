/* ========================================================================
    JANUS      Speech- to Speech Translation
               ------------------------------------------------------------
               Object: Dynamic Programming Tree Forward
               ------------------------------------------------------------

    Author  :  Michael Finke
    Module  :  lattice.c
    Date    :  $Id: lattice.c 3418 2011-03-23 15:07:34Z metze $
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
    Revision 3.29  2005/02/24 16:06:31  metze
    Cosmetics for x86

    Revision 3.28  2003/08/14 11:20:22  fuegen
    Merged changes on branch jtk-01-01-15-fms (jaguar -> ibis-013)

    Revision 3.27.20.3  2003/07/02 17:59:23  fuegen
    fixes for file handling under Windows
    using general functions for file open/close

    Revision 3.27.20.2  2002/04/29 11:58:30  metze
    Cleaner compiling on gcc

    Revision 3.27.20.1  2001/11/19 14:46:27  metze
    Go away, boring message!

    Revision 3.27  2000/11/14 12:29:35  janus
    Merging branch jtk-00-10-27-jmcd.

    Revision 3.26.6.2  2000/11/08 17:19:37  janus
    Finished making changes required for compilation under 'gcc -Wall'.

    Revision 3.26.6.1  2000/11/06 10:50:32  janus
    Made changes to enable compilation under -Wall.

    Revision 3.26  2000/10/24 14:01:19  janus
    Merged changes on branch jtk-00-10-12-tschaaf

    Revision 3.25.20.1  2000/10/24 13:24:12  janus
    moved NDEBUG definition to optimizing definition.
    if NDEBUG is defined assert statements are removed during compilation

    Revision 3.25  2000/09/14 17:47:12  janus
    Merging branch jtk-00-09-14-jmcd.

    Revision 3.24.8.1  2000/09/14 16:20:55  janus
    Merged branches jtk-00-09-10-tschaaf, jtk-00-09-12-hyu, and jtk-00-09-12-metze.

    Revision 3.24.6.1  2000/09/14 16:00:42  janus
    Incorporated bug fixes from Klaus Ries.

    Revision 3.24.2.1  2000/09/13 22:18:57  hyu
    lattice.clear changed to be functionally equivalent to deinit()+init()

    Revision 3.24  2000/09/12 15:06:17  janus
    Merged branch jtk-00-09-08-hyu.

    Revision 3.23.16.1  2000/09/12 01:16:02  hyu
    Added lattice.clear method.

    Revision 3.23  2000/08/16 09:27:30  soltau
    reactivated lattice2hmm but didn't test it
    free -> Tcl_Free

    Revision 3.22  1998/12/09 16:16:44  tschaaf
    in vocab.c the index2realIndex was renamed to
    index2realPronounciationIndex.
    Renaming is here also done.

    Revision 3.21  1998/09/21 13:37:20  kemp
    Removed bug in lattice rgamma: with TCL 8.0, a string is passed as pointer
    to the TCL-internal string, and not copied before passing it. Using strtok()
    results in chopping a TCL variable into words. Now the passed hypo is copied
    before strtok() calls are being made.

 * Revision 3.20  1998/08/25  14:44:46  tschaaf
 * Bugfix in NBest generation.
 * Fixed by kries.
 *
    Revision 3.19  1998/07/31 12:19:19  kries
    -map flag introduced in vocab class made changes necessary

    Revision 3.18  1998/07/17 18:43:16  tschaaf
    Added scaling factor for scores that can be applyed during lattice
    puts ore write (lattice puts -factor 0.5).
    made some code cleaning like adding static by missing prototypes and
    removing unused variables.

    Revision 3.17  1998/07/13 18:57:11  kries
    made VM write more secure, more sensible defaults for astar

 * Revision 3.16  1998/07/03  15:29:36  kries
 * fix for gamma (removed lmReset
 *
 * Revision 3.15  1998/06/11  18:54:36  kries
 * fixes
 *
 * Revision 3.14  1998/06/11  17:50:32  kries
 * adaptation of new LM interface
 *
 * Revision 3.13  1998/05/26  18:27:23  kries
 * adaptation to new language model interface
 * addition of new A^* search
 *
 * Revision 3.12  1998/03/04  17:26:07  kemp
 * removed a tiny bug in the -nl and -nn flags that could effect
 * them to effect nothing...
 *
 * Revision 3.11  1998/03/04  17:16:21  kemp
 * Added new flags -nl (number of links) and -nn (number of nodes)
 * to the prune method. The number of links/nodes after pruning
 * can be set with them.
 *
 * Revision 3.10  1998/03/03  11:54:34  kemp
 * Minor correction: removed one old comment; if latticeCheck encounters an empty lattice,
 * a warning is issued (and TCL_OK returned). Although it's only a warning, there is not
 * much one can do with an empty lattice like this...
 *
 * Revision 3.9  1998/02/19  11:15:09  kemp
 * Removed bug in latticeDeleteUnreachable. Enhanced latticeDeleteUnreachable
 * so that all nodes with zero fan-out are also deleted.
 * Added mode 1 and mode 2 (new default) to the rgamma method.
 * Added configure -nodeN and configure -linkN to the lattice object.
 * Now lattice add increments lat->linkN and lat->nodeN
 *
 * Revision 3.8  1997/09/30  17:31:20  kemp
 * Introduced the methods
 * gamma
 * rgamma
 * N
 * scale
 *
 * Revision 3.7  1997/09/11  14:37:59  tschaaf
 * new option in latticeWrite and latticePuts
 * frame_offset is used to shift the frameindex in the output
 *
    Revision 3.6  1997/08/27 09:09:09  tschaaf
    removed Bug in latticeCheck that made writeVM crash
    the latP->frameN fas not filled correct
    Also removed the undefined behavior in writeVM if no id was given

    Revision 3.5  1997/07/18 17:57:51  monika
    gcc-clean

    Revision 3.4  1997/06/24 11:52:44  tschaaf
    removed some forgotten INFOs
    .in puts

    Revision 3.3  1997/06/20 18:22:11  tschaaf
    [B[Aputs got -format option
    we can now puts VM lattice Format

    Revision 1.24  96/11/30  20:25:41  kries
    Fixed bug in 1.23, don;t use 1.23
    
    Revision 1.23  1996/11/26  09:10:21  koll
    removed output of debug-info.

    Revision 1.22  1996/11/24  01:59:02  kries
    fixed memory leak in nbest rescorer

    Revision 1.21  1996/11/22  15:17:44  koll
    added method [Lattice] prune

    Revision 1.20  1996/10/23  16:58:14  kries
    fixed mbest search: exact lookahead, better memory managment

    Revision 1.19  1996/09/11  10:53:57  monika
    fixed some bugs if writing vm-type lattices from lattices that
    have NOT been just produced by search but have been read with add

    Revision 1.18  96/07/14  18:38:16  18:38:16  monika (Monika Woszczyna)
    Lattices can be written in Verbmobil format -- 
    warning: this is the version 0.0 of the routines to
    write those lattices, so expect one or two bugs.
    
    Revision 1.17  96/07/03  13:18:42  13:18:42  monika (Monika Woszczyna)
    changes to work with 64K vocabulary
    
    Revision 1.16  96/03/25  23:33:23  23:33:23  kries (Klaus Ries)
    added support for cache model in rescoring
    
    Revision 1.15  1996/03/25  18:56:40  finkem
    added lattice2Hmm function and removed bug in the n-best
    rescoring procedure.

    Revision 1.14  1996/02/17  19:25:21  finkem
    removed old hypoList stuff

    Revision 1.13  1996/01/28  18:26:54  kries
    ask trigram model at begin of sentence

    Revision 1.12  1996/01/21  22:39:21  finkem
    gave the right language model pointers to lm score routines!

    Revision 1.11  1996/01/14  14:17:19  monika
    always give lm pointer when calling lm routhinesines

    Revision 1.10  1995/10/29  23:52:24  finkem
    got rid of stupid oldZ and oldP flags by reading the
    original values from the LM

    Revision 1.9  1995/10/26  20:44:28  finkem
    find N-best implemented

    Revision 1.8  1995/09/24  01:54:10  torsten
    *** empty log message ***

    Revision 1.7  1995/09/21  21:39:43  finkem
    lattice rescoring bug removed

    Revision 1.6  1995/09/13  18:50:33  finkem
    finalAScore sort of fixed...

    Revision 1.5  1995/09/12  19:49:19  finkem
    First  Janus3 version of the lattice procedure introduced...

    Revision 1.4  1995/08/29  02:46:05  finkem
    *** empty log message ***

    Revision 1.3  1995/08/17  17:05:23  rogina
    *** empty log message ***

    Revision 1.2  1995/08/03  14:42:38  finkem
    *** empty log message ***

    Revision 1.1  1995/07/25  13:49:57  finkem
    Initial revision

 
   ======================================================================== */

#define   SEARCH_MODULE

#include "search.h"
#include "math.h"
#include "treefwd.h"
#include "lattice.h"
#include "hmm3gram.h"

char latticeRCSVersion[]= "@(#)1$Id: lattice.c 3418 2011-03-23 15:07:34Z metze $";

/* ========================================================================
    Local language model calling
   ======================================================================== */


static float local_lm_TgScore ( int w1, int w2, int w3 ) {
  int w[3];

  if(w1==NILWORD) {
    w[0] = SD.lmXA[w2]; w[1] = SD.lmXA[w3];
    return SC.lmodel->lmP->scoreFct(SC.lmodel->cd,w,2,1);
  } else {
    w[0] = SD.lmXA[w1]; w[1] = SD.lmXA[w2]; w[2]= SD.lmXA[w3];
    return SC.lmodel->lmP->scoreFct(SC.lmodel->cd,w,3,2);
  }    
}


/* ========================================================================
    Static Data
   ======================================================================== */

static LatNode** nodePA  = NULL;
static float**   scorePA = NULL;

/* ========================================================================
    Static Functions
   ======================================================================== */
static double addLogProbs(double p1, double p2);

/* ------------------------------------------------------------------------
    latticeLM          map word indices to LM word indices
   ------------------------------------------------------------------------ */

static int latticeLM( Lattice* latP, LMPtr* lmPtr, int mapFlag)
{
  Vocab*    vocabP = latP->vocabP;
  VWord*    vocabA = latP->vocabP->vwordList.itemA;
  LatNode*  nodeP;
  
  assert( latP);
  assert( lmPtr);

  if((lmPtr->cd == latP->lmPtr.cd) && (lmPtr->lmP == latP->lmPtr.lmP) && lmPtr->cd)
    return TCL_OK;


  for ( nodeP = latP->nodeP; nodeP; nodeP = nodeP->nextP) {
    int idx   = -1;

    if (      nodeP->wordX == vocabP->startWordX) {
      idx = lmPtr->lmP->indexFct( lmPtr->cd, "<s>");
    }
    else if ( nodeP->wordX == vocabP->finishWordX) {
      idx = lmPtr->lmP->indexFct( lmPtr->cd, "</s>");
    }
    else if (mapFlag) {
      idx = lmPtr->lmP->indexFct( lmPtr->cd, 
                                  vocabA[vocabA[nodeP->wordX].realPronounciationX].spellS);
    }
    else {
      idx = lmPtr->lmP->indexFct( lmPtr->cd, 
                                  vocabA[nodeP->wordX].spellS);
    }
    if ( idx < 0) {
      if ((idx = lmPtr->lmP->indexFct( lmPtr->cd, "<UNK>")) < 0) {
        ERROR("Couldn't map '%s' (%ld) to LM word.\n", 
               vocabA[nodeP->wordX].spellS,nodeP->wordX);
        return TCL_ERROR;
      }
      INFO("Mapped '%s' (%ld) to LM word '%s'.\n", 
            vocabA[nodeP->wordX].spellS,nodeP->wordX,
            lmPtr->lmP->nameFct( lmPtr->cd, idx));
    }
    nodeP->lmWordX = idx;
  }

  latP->lmPtr.cd=lmPtr->cd;
  latP->lmPtr.lmP=lmPtr->lmP;

  return TCL_OK;
}

/* ========================================================================
    Lattice Node
   ======================================================================== */
/* ------------------------------------------------------------------------
    Create/Init LatNode Object
   ------------------------------------------------------------------------ */

int latNodeInit( LatNode* lnP, short frameX, WordX wordX)
{
  lnP->wordX  = wordX;
  lnP->frameX = frameX;
  lnP->score  = WORSTSCORE;
  lnP->linkP  = NULL;
  lnP->nextP  = NULL;
  return TCL_OK;
}

LatNode* latNodeCreate( Lattice* latP, short frameX, WordX w)
{
  LatNode* fromP = latNodeFind( latP, frameX, w);

  if (! fromP) {
    fromP = (LatNode*)malloc( sizeof(LatNode));

    if (! fromP || latNodeInit( fromP, frameX, w) != TCL_OK) {
      if ( fromP) free( fromP);
      ERROR("Failed to allocate LatNode object for word %d.\n", w);
      return NULL; 
    }
    if ( nodePA) {   /* if there is a list of nodes for each frame
                        like in the latticeAlloc routine, insert the
                        new node into it */

      fromP->nextP   = nodePA[frameX];
      nodePA[frameX] = fromP;
    }
    else {           /* the order in which the nodes are stored is
                        backward in time, starting with the last frame
                        til the 0 frame */
      LatNode* nodeP  = NULL;
      LatNode* pnodeP = NULL;

      for ( nodeP  = latP->nodeP; nodeP && nodeP->frameX >= frameX; 
            pnodeP = nodeP, nodeP = nodeP->nextP);
      if ( pnodeP) { fromP->nextP = pnodeP->nextP; pnodeP->nextP = fromP; }
      else         { fromP->nextP = latP->nodeP;   latP->nodeP   = fromP; }

      if (! latP->finalP || pnodeP == latP->finalP) latP->finalP = fromP;
    }
    latP->nodeN++;
  }
  return fromP;
}

/* ------------------------------------------------------------------------
    Free/Deinit LatNode Object
   ------------------------------------------------------------------------ */

int latNodeDeinit( LatNode* lnP)
{
  assert (lnP == lnP); /* keep the compiler quiet */
  return TCL_OK;
}

int latNodeFree( LatNode* lnP)
{
  if ( latNodeDeinit( lnP) != TCL_OK) return TCL_ERROR;
  free(lnP);
  return TCL_OK;
}

/* ------------------------------------------------------------------------
    latNodeFind         Find LatNode Object
   ------------------------------------------------------------------------ */

LatNode* latNodeFind( Lattice* latP, short frameX, WordX w)
{
  LatNode* fromP;

  if ( nodePA) {
    for ( fromP = nodePA[frameX]; fromP && fromP->wordX != w; 
          fromP = fromP->nextP);
  }
  else {
    for ( fromP = latP->nodeP;  fromP && 
        ( fromP->wordX != w || fromP->frameX != frameX); 
          fromP = fromP->nextP);
  }
  return fromP;
}

/* ------------------------------------------------------------------------
    latNodePutsItf      puts LatNode Object
   ------------------------------------------------------------------------ */
/*
static int latNodePutsItf( ClientData cd, int argc, char *argv[])
{
  LatNode* nodeP = (LatNode*)cd;
  LatLink* linkP = nodeP->linkP;
  int       ac     =  argc - 1;

  if ( itfParseArgv( argv[0], &ac, argv, 0,
       NULL) != TCL_OK) return TCL_ERROR;

  itfAppendResult(  "%d %d", nodeP->frameX, nodeP->wordX);

  if ( nodeP->score < WORSTSCORE)
    itfAppendResult(  " -score % 2.4e", nodeP->score);

  if ( linkP) {
    itfAppendResult(  " -links {");
    for ( ; linkP; linkP = linkP->nextP) {
      itfAppendResult(" {%d %d",linkP->toP->frameX, linkP->toP->wordX); 
      itfAppendResult( " %d % 2.4e}", (int)linkP->frameX, linkP->lscore);
    }
    itfAppendResult( "}\n");
  }
  return TCL_OK;
}
*/
/* ------------------------------------------------------------------------
    latNodeAdd          add links to latNode object
   ------------------------------------------------------------------------ */

static int latNodeAdd( LatNode* lnP, Lattice* latP, int argc, char** argv)
{
  int       ac     =  argc;
  int       frameX =  lnP->frameX + 2;
  int       lto    =  lnP->frameX + 1;
  int       word;
  float     score  =  WORSTSCORE;

  if ( itfParseArgv( argv[0], &ac, argv, 0,
      "<wfrom>", ARGV_INT,    NULL, &frameX, NULL,"starting point of word",
      "<word>",  ARGV_LINDEX, NULL, &word,  &(latP->vocabP->vwordList),
                                                  "word in vocabulary",
      "<lto>",   ARGV_INT,    NULL, &lto,    NULL,"link end point",
      "<score>", ARGV_FLOAT,  NULL, &score,  NULL,"path score",
       NULL) != TCL_OK) return TCL_ERROR;
  else {
    LatNode* toP    = latNodeCreate( latP, frameX, word);

    latLinkCreate( lnP, toP, score, WORSTSCORE, lto, &(latP->linkN) );
    /* latLinkCreate( lnP, toP, score, WORSTSCORE, lto, &nlinks ); */
  }
  return TCL_OK;
}

/* ========================================================================
    Lattice Link
   ======================================================================== */
/* ------------------------------------------------------------------------
    Create/Init LatLink Object
   ------------------------------------------------------------------------ */

int latLinkInit( LatLink* llP, LatNode* fromP, LatNode* toP,
                 float score, short frameX)
{
  llP->lscore = score;
  llP->pscore = WORSTSCORE;
  llP->frameX = frameX;
  llP->fromP  = fromP;
  llP->toP    = toP;

  llP->nextP  = NULL;            /* list of links (of the same node) */
  llP->bestP  = NULL;            /* best link */
  llP->qnextP = NULL;
  return TCL_OK;
}

/* ------------------------------------------------------------------------
    latLinkCreate      insert a directed link between "from" and "to" node.
                       If such a link already exists choose the one with
                       the better link score.
   ------------------------------------------------------------------------ */

LatLink* latLinkCreate( LatNode* fromP, LatNode* toP, float score,
                        float pscore,   short frameX, int*  nlinks)
{
  LatLink* linkP  = fromP->linkP;

  /* Look for an existing link node between toP and fromP */

  for ( ; linkP && linkP->toP != toP; linkP = linkP->nextP);

  if (! linkP) { /* create a new link */

    linkP          = (LatLink*)malloc( sizeof(LatLink));

    if (! linkP || latLinkInit( linkP, fromP, toP, score, frameX) != TCL_OK) { 
      if ( linkP) free( linkP);
      ERROR("Failed to allocate LatLink object.\n");
      return NULL; 
    }
    linkP->nextP   = fromP->linkP;
    fromP->linkP   = linkP;

    if ( pscore+score < fromP->score) fromP->score = pscore+score;
    *nlinks += 1;
  }
  else if ( score < linkP->lscore) {
    linkP->lscore = score;
    linkP->frameX = frameX;

    if ( pscore+score < fromP->score) fromP->score = pscore+score;
    *nlinks += 1;
  }
  return linkP;
}

/* ------------------------------------------------------------------------
    Free/Deinit LatLink Object
   ------------------------------------------------------------------------ */

int latLinkDeinit( LatLink* lnP)
{
  assert (lnP == lnP); /* keep the compiler quiet */
  return TCL_OK;
}

int latLinkFree( LatLink* lnP)
{
  if ( latLinkDeinit( lnP) != TCL_OK) return TCL_ERROR;
  free(lnP);
  return TCL_OK;
}

/* ========================================================================
    Lattice
   ======================================================================== */

static Lattice latticeDefault;

/* ------------------------------------------------------------------------
    Create/Init Lattice Object
   ------------------------------------------------------------------------ */

int latticeInit( Lattice* latP, char* name, Vocab* vocabP)
{
  latP->name   = strdup(name);
  latP->useN   = 0;

  latP->nodeP  = NULL;
  latP->startP = NULL;
  latP->finalP = NULL;
  latP->frameN = 0;

  latP->nodeN = 0; /* HIER */
  latP->linkN = 0; /* HIER */

  latP->finalAScore = 0.0;
  latP->vocabP      = vocabP; link( vocabP, "Vocab");
  latP->beamWidth     = latticeDefault.beamWidth;
  latP->noiseBeamWidth = latticeDefault.noiseBeamWidth;
  latP->topN        = latticeDefault.topN;
  latP->format      = latticeDefault.format;
  
  lmPtrInit( &(latP->lmPtr));
  
  hypoListInit( &(latP->hypoList), "hypoList", vocabP, 1);
  return TCL_OK;
}

Lattice *latticeCreate( char* name, Vocab* vocabP)
{
  Lattice *latP = (Lattice*)malloc(sizeof(Lattice));

  if (! latP || latticeInit( latP, name, vocabP) != TCL_OK) {

     if ( latP) free( latP);
     ERROR("Failed to allocate Lattice object '%s'.\n", name);
     return NULL; 
  }
  return latP;
}

static ClientData latticeCreateItf(ClientData clientData, 
                                   int argc, char *argv[])
{
  int       ac     =  argc - 1;
  Vocab*    vocabP =  NULL;

  assert (clientData == clientData); /* keep the compiler quiet */

  if ( itfParseArgv( argv[0], &ac, argv+1, 0,
      "<Vocab>", ARGV_OBJECT, NULL, &vocabP, "Vocab","vocabulary",
       NULL) != TCL_OK) return NULL;

  return (ClientData)latticeCreate(argv[0], vocabP);
}

/* ------------------------------------------------------------------------
    Free/Deinit Lattice Object
   ------------------------------------------------------------------------ */

static int latticeLinkN( Lattice* latP)
{
  return latP->useN;
}

static void latticeDealloc( Lattice* latP)
{
  if ( latP) {
    LatNode* nodeP, *tnodeP;

    for ( nodeP = latP->nodeP; nodeP; nodeP = tnodeP) {
      LatLink* linkP, *tlinkP;

      tnodeP = nodeP->nextP;
      for ( linkP = nodeP->linkP; linkP; linkP = tlinkP) {
        tlinkP = linkP->nextP;
        latLinkFree(linkP);
      }
      latNodeFree(nodeP);
    }
    latP->nodeP = NULL;
    lmPtrDeinit( &(latP->lmPtr));
  }
}

static int latticeDeinit( Lattice* latP)
{
  if ( latticeLinkN( latP)) {
    SERROR("Lattice '%s' still in use by other objects.\n", latP->name);
    return TCL_ERROR;
  }
  if ( latP->name) { free( latP->name); latP->name = NULL; }
  latticeDealloc( latP);
  hypoListDeinit( &(latP->hypoList));
  unlink( latP->vocabP, "Vocab");
  return TCL_OK;
}

static int latticeFree( Lattice* latP)
{
  if ( latticeDeinit( latP) != TCL_OK) return TCL_ERROR;
  free(latP);
  return TCL_OK;
}

static int latticeFreeItf (ClientData clientData) 
{ return latticeFree( (Lattice*)clientData); }

/* ------------------------------------------------------------------------
    latticeConfigureItf
   ------------------------------------------------------------------------ */

static int latticeConfigureItf( ClientData cd, char *var, char *val)
{
  Lattice*    latP = (Lattice*)cd;
  if (! latP) latP = &latticeDefault;

  if (! var) {

    ITFCFG(latticeConfigureItf,cd,"beamWidth");
    ITFCFG(latticeConfigureItf,cd,"noiseBeamWidth");
    ITFCFG(latticeConfigureItf,cd,"topN");
    ITFCFG(latticeConfigureItf,cd,"nodeN");
    ITFCFG(latticeConfigureItf,cd,"linkN");
    ITFCFG(latticeConfigureItf,cd,"fileFormat");
    return TCL_OK;
  }
  ITFCFG_Float(    var,val,   "beamWidth",      
                         latP->beamWidth, 0);
  ITFCFG_Float(    var,val,   "noiseBeamWidth",      
                         latP->noiseBeamWidth, 0);
  ITFCFG_Int(      var,val,   "topN",      
                         latP->topN, 0);
  ITFCFG_Int(      var,val,   "linkN",      
                         latP->linkN, 1);
  ITFCFG_Int(      var,val,   "nodeN",      
                         latP->nodeN, 1);
  ITFCFG_CharPtr(      var,val,   "fileFormat",      
                         latP->format, 0);
  return TCL_ERROR;
}

/* ------------------------------------------------------------------------
    latticeAccessItf
   ------------------------------------------------------------------------ */

static ClientData latticeAccessItf( ClientData cd, char *name, TypeInfo **ti)
{ 
  Lattice* latP = (Lattice*)cd;

  if (*name == '.') { 
    if (name[1]=='\0') {
      itfAppendElement( "hypoList");
      *ti = NULL; return NULL;
    }
    else { 
      if (! strcmp( name+1, "hypoList")) {
        *ti = itfGetType("HypoList");
        return (ClientData)&(latP->hypoList);
      }
    }
  }
  return NULL;
}

static int latticePutsAppendResult(VmLink *vmLinkA, int vmLinkN, char *id, int withLM, int frame_offset, float factor) {

  int l;

  /* ah, and NOW write it */

  if (id != NULL) {
    itfAppendResult("%%TURN: %s\n",id);
  } else {
    itfAppendResult("%%TURN: %s\n","(none)");
  }
  itfAppendResult("BEGIN_LATTICE\n");
  
  for (l = 0; l < vmLinkN; l ++) {
    VmLink * linkP = vmLinkA + l;
    if (withLM) {
      itfAppendResult("%d %d %s %f %d %d (AP (lm %f))\n",
                   linkP->fromNX, linkP->toNX, linkP->wordS,
                   linkP->score * factor, linkP->fromFX + frame_offset, linkP->toFX + frame_offset, linkP->lmScore * factor);
    } else {
      itfAppendResult("%d %d %s %f %d %d\n",
                   linkP->fromNX, linkP->toNX, linkP->wordS,
                   (linkP->score - linkP->lmScore) * factor, linkP->fromFX + frame_offset, linkP->toFX + frame_offset);
    }
  }
  itfAppendResult("END_LATTICE\n\n");  
  return TCL_OK;
}


/* ------------------------------------------------------------------------
    latticeCheck   checks consistency of the lattice
                   calls latticeDeleteUnreachable,
                   sets latP->startP, latP->frameN, latP->linkN, latP->nodeN
   ------------------------------------------------------------------------ */

int latticeCheck( Lattice* latP)
{
  LatNode* nodeP;

  latP->nodeN  = 0;
  latP->linkN  = 0;
  latP->frameN = 0;
  latP->startP = NULL;

  latticeDeleteUnreachable( latP);
  
  latP->finalP = latP->nodeP;
  if ( 0 == latP->nodeP ) {
    WARN( "latticeCheck: empty lattice\n");
    return TCL_OK;
  }

  if ( latP->finalP->wordX != latP->vocabP->finishWordX) return TCL_ERROR;

  for ( nodeP = latP->nodeP; nodeP; nodeP = nodeP->nextP) {
    LatLink* linkP = nodeP->linkP;
    
    if ( nodeP->wordX == latP->vocabP->startWordX  && nodeP->frameX  == 0) 
         latP->startP = nodeP;

    for ( ; linkP; linkP = linkP->nextP) {
     if (linkP->frameX >= latP->frameN) latP->frameN = linkP->frameX +1;

     assert(linkP->toP != NULL);
     if (linkP->toP->frameX >= latP->frameN) latP->frameN = linkP->toP->frameX + 1; /* because optional Silence ! */
     latP->linkN++;
    }

    latP->nodeN++;
  }
  return (latP->startP) ? TCL_OK : TCL_ERROR;
}

/* ------------------------------------------------------------------------
    latticeAlloc   allocates a lattice structure from a backpointer table
                   generated in the forward pass.
   ------------------------------------------------------------------------ */

int latticeAlloc( Lattice* latP, short frameN, float z, float beamWidth)
{
  LatNode** pnodeP;
  LatNode*  nodeP;
  short     frameX = frameN - 1;
  BackP*    bp     = BP.tablePA[frameX];
  short     bpN    = BP.tableN[ frameX], bpX;
  short     fX;
  float     finalAScore = WORSTSCORE;

  int       colN = 10;
  Timer     stopit;
  
  timerReset(&stopit);
  timerStart(&stopit);

  latticeDealloc( latP);

  lmPtrInit(&(latP->lmPtr));

  INFO( "Allocating lattice\n");

  for ( bpX  = 0; bpX < bpN; bpX++, bp++) 
    if ( bp->wordX == SD.finishWordX) break;

  if (  bpX >= bpN) {
    /* Use artificial final node created during Tree/Flat backtrace */

    bp          = BP.tablePA[frameX+1];
    frameX      = bp->path.frameX + 1;
    finalAScore = dpsBackPointerScores( bp, z, NULL);
  }
  else {
    frameX      = bp->path.frameX + 1;
    finalAScore = dpsBackPointerScores( bp, z, NULL);
  }

  /* Allocate lattice structure */

  nodePA       = (LatNode**)malloc((frameN+1) * sizeof(LatNode*));
  latP->nodeP  = NULL;
  latP->frameN = frameN;

  memset((char*)nodePA, 0, (frameN+1) * sizeof(LatNode*));
 

  /* Find the final node in the back pointer table */

  nodeP = latP->finalP = (LatNode*)malloc( sizeof(LatNode));  

  latP->finalP->lmWordX = latP->finalP->wordX  = SD.finishWordX;
  latP->finalAScore    = finalAScore;
  scorePA              = dpsCreateBPScoreTable( frameN, z);

  nodeP->score         = finalAScore;
  nodeP->frameX        = frameX;
  nodeP->linkP         = NULL;
  nodeP->nextP         = NULL;

  nodePA[frameX]       =   nodeP;
  pnodeP               = &(latP->nodeP);

  /* For all frames test the possible word predecessors */

  if ( SC.verbose < 2) colN = dpsPrintScale( frameN);

  for ( fX = frameN-1; fX > frameX; fX--) {
    if (SC.verbose < 2 && fX % colN == 0) fprintf( STDERR, " ");
  }
  for ( fX = frameX; fX >= 0; fX--) {
    LatNode* tnodeP;
    int      nlinks;

    if (SC.verbose < 2 && fX % colN == 0) fprintf( STDERR, "|");

    /* For all nodes/words starting in the current frame we create the
       links. Filler nodes are removed immediately after all links
       departing from them have been transferred to potential preceeding
       nodes */

    for ( nodeP = nodePA[fX]; nodeP; nodeP = tnodeP) {
      WordX wX = nodeP->wordX;
      tnodeP   = nodeP->nextP;

      if ( fX > 0 && (wX == SD.finishWordX || nodeP->linkP)) 
           nlinks = latLinksAlloc( latP, nodeP, fX, beamWidth, z);
      else nlinks = 0;

      /* If no new links pointing to nodeP were created by latticeCreateLinks
         the node itself and all links can be removed. But we do not consider
         nodes in the to-list of nodeP, which might also loose their pointer. 
         So there might remain some nodes which have no path from the 
         start node */

      if ( ISA_FILLER(wX) || (  fX && ! nlinks) || 
                             (! fX && wX != SD.startWordX)) {

        LatLink* linkP  = nodeP->linkP;
        LatLink* tlinkP;

        for ( ; linkP; linkP = tlinkP) {
          tlinkP = linkP->nextP;
          latLinkFree(linkP);
        }
        latNodeFree(nodeP);        
      }
      else {
        *pnodeP      =   nodeP;
        nodeP->nextP =   NULL;
        pnodeP       = &(nodeP->nextP);
      }
    }
    lm_NextFrame(SC.lmodel);
  }

  if ( nodePA) { free( nodePA); nodePA = NULL; }

  if ( scorePA) {
    dpsFreeBPScoreTable( scorePA, frameN);
    scorePA = NULL;
  }
  fprintf( stderr, "\n");

  /* Find start node within the list of first nodes and
     the number of nodes and links in the lattice */

  latticeCheck( latP);

  INFO( "Lattice %d nodes %d links\n", latP->nodeN, latP->linkN);
  INFO( "End of Create (%f secs)\n", timerStop(&stopit));

  if (! latP->startP) {
    ERROR( "Couldn't find <s>.0\n");
    latticeDeinit( latP);
    return TCL_ERROR;
  }
  return TCL_OK;
}

/* ------------------------------------------------------------------------
    latLinksAlloc  create all links to a given node by considering all
                   possible word transitions in the preceeding frame. If
                   the current word is a filler word all links starting
                   from that node are transferred to the preceeding words.
   ------------------------------------------------------------------------ */

int latLinksAlloc( Lattice* latP, LatNode* toP, short frameX, 
                                  float beamWidth, float z)
{
  short  fX      = frameX-1;
  WordX  wX      = toP->wordX;
  BackP* bp      = BP.tablePA[fX];
  float* stackA  = BP.stackPA[fX];
  float* scoreA  = scorePA[fX];
  WordX  bpN     = BP.tableN[ fX], bpX;
  float  thresh  = bestTotalScore + beamWidth;
  /*  float  noisethresh = bestTotalScore + latP->noiseBeamWidth; */
  int    nlinks  = 0;

  bpN = (bpN < latP->topN || latP->topN < 1) ? bpN : latP->topN;

  /* All words ending the frame before frameX will be considered as possible
     words preceeding toP->wordX */

  for ( bpX = 0; bpX < bpN; bpX++, bp++) {
    short    sframeX  =   bp->path.frameX + 1;
    WordX    w        =   bp->wordX;
    float    wPenalty =  (ISA_FILLER(w)) ? (
                         (w == SD.silenceWordX) ? SC.silenceWordPenalty :
                                                  SC.fillerWordPenalty) : 0.0;
    int      rstackX  = ( bp->weDiX >= 0) ? SD.xweModelA[bp->weDiX].relXA[
                          SD.vocabA[wX].segXA[0]] : 0;
    float    pscore   =   stackA[  bp->stackX + rstackX];
    float    lscore   =   pscore - bp->score  + scoreA[bpX];
    LatNode* fromP    =   NULL;
    int      newLink  =   0;

    if ((w == SD.startWordX && sframeX != 0) || w == SD.finishWordX) continue;

    if ( ISA_FILLER(wX)) {

      LatLink* linkP   =  toP->linkP;

      for (; linkP; linkP = linkP->nextP) {
        float nscore  = lscore + linkP->lscore + wPenalty;
        float lmscore = local_lm_TgScore( bp->prevWordX, bp->realWordX,SD.vocabA[linkP->toP->wordX].realX);
        float tscore  = linkP->toP->score;

        if ( tscore + linkP->lscore + lmscore + pscore <= thresh) {
          if (! fromP) fromP = latNodeCreate( latP, sframeX, w);

          if (! ISA_FILLER(w)) tscore += lmscore * z;

          latLinkCreate( fromP, linkP->toP, nscore, tscore, fX, &newLink);
        }
      }
    }
    else {

      float lmscore = local_lm_TgScore( bp->prevWordX, bp->realWordX,SD.vocabA[toP->wordX].realX);
      float tscore  = toP->score;

      if ( tscore + lmscore + pscore <= thresh) {
        if (! fromP) fromP = latNodeCreate( latP, sframeX, w);

        if (! ISA_FILLER(w)) tscore += lmscore * z;

        latLinkCreate( fromP, toP, lscore + wPenalty, tscore, fX, &newLink);
      }
    }
    nlinks += newLink;
  }
  return nlinks;
}

/* ------------------------------------------------------------------------
    latticeDeleteUnreachable  remove nodes with zero fan-in or zero fan-out
   ------------------------------------------------------------------------ */

void latticeDeleteUnreachable( Lattice* latP)
{
  LatNode* nodeP, *tnodeP, **prevnodeP;
  int      nullNodeN, n=0;
  LatLink* linkP, *tlinkP, **prevlinkP;

  /* ----------------------------------------------- */
  /* pass 1: delete all nodes with a fan-out of zero */
  /* for this, misuse nodeP->info.fanIn to store the */
  /* fan-out instead of the fan-in of the nodes      */
  /* ----------------------------------------------- */
  for ( nodeP = latP->nodeP; nodeP; nodeP = nodeP->nextP) nodeP->info.fanIn = 0;

  for ( nodeP = latP->nodeP; nodeP; nodeP = nodeP->nextP) {
    for ( linkP = nodeP->linkP; linkP; linkP = linkP->nextP) nodeP->info.fanIn++;
  }
  /* ---------------------------------------------------- */
  /* delete all links that point to a node with fan-out 0 */
  /* since the deletion can lead to new nodes having a    */
  /* fan-out of zero, several iterations may be required  */
  /* ---------------------------------------------------- */
  do {
    nullNodeN = 0;
    for ( nodeP = latP->nodeP; nodeP; nodeP = nodeP->nextP) {
      prevlinkP = &(nodeP->linkP);
      for (linkP = nodeP->linkP; linkP; linkP = tlinkP) {
	tlinkP = linkP->nextP;
	if ( (linkP->toP->info.fanIn == 0) && (linkP->toP->wordX != latP->vocabP->finishWordX) ) {
	  if (--nodeP->info.fanIn == 0) nullNodeN++;
	  *prevlinkP = linkP->nextP;
	  latLinkFree(linkP);
	} else {
	  prevlinkP = &(linkP->nextP);
	}
      }
    }
  } while (nullNodeN);
  /* ------------------------------------------------- */
  /* now all links to nodes with fan-out 0 are deleted */
  /* we can now delete the nodes themselves            */
  /* ------------------------------------------------- */
  prevnodeP = &(latP->nodeP);
  for ( nodeP = latP->nodeP; nodeP; nodeP = tnodeP) {
    tnodeP = nodeP->nextP;
    if (nodeP->info.fanIn == 0 && nodeP->wordX != latP->vocabP->finishWordX) {
      *prevnodeP = nodeP->nextP;
      latNodeFree(nodeP);
      n++;
    } else {
      prevnodeP = &(nodeP->nextP);
    }
  }

  /* ---------------------------------------------- */
  /* pass 2: delete all nodes with a fan-in of zero */
  /* ---------------------------------------------- */
  for ( nodeP = latP->nodeP; nodeP; nodeP = nodeP->nextP) nodeP->info.fanIn = 0;

  for ( nodeP = latP->nodeP; nodeP; nodeP = nodeP->nextP) {
    LatLink* linkP = nodeP->linkP;
    for ( ; linkP; linkP = linkP->nextP) linkP->toP->info.fanIn++;
  }

  do {
    nullNodeN = 0;
    prevnodeP = &(latP->nodeP);
    for ( nodeP = latP->nodeP; nodeP; nodeP = tnodeP) {
      /* ------------------------------------------------- */
      /* maybe nodeP will no longer be there after the     */
      /* body of the loop, so avoid accessing nodeP->nextP */
      /* in the for(...) part but do it now:               */
      /* ------------------------------------------------- */
      tnodeP = nodeP->nextP;

      if ( nodeP->wordX != latP->vocabP->startWordX && nodeP->info.fanIn == 0) {
	/* -------------------------------------------------- */
	/* OK, delete node 'nodeP'. Delete all links emerging */
	/* from 'nodeP'. Decrement fan-in for the nodes that  */
	/* are targeted by the links emerging from 'nodeP'    */
	/* -------------------------------------------------- */
        for (linkP = nodeP->linkP; linkP; linkP = tlinkP) {
          tlinkP = linkP->nextP;
          --(linkP->toP->info.fanIn);
          if (linkP->toP->info.fanIn < 1) nullNodeN++; /* another node has to be deleted! */
          latLinkFree(linkP);
        }
	*prevnodeP = nodeP->nextP;
        latNodeFree(nodeP);
        n++;
      } else {
	prevnodeP = &(nodeP->nextP);
      }
    }
  } while ( nullNodeN);

  if ( n > 0) INFO("latticeDeleteUnreachable %d.\n", n);

}

/* ------------------------------------------------------------------------
    latticeAdd
   ------------------------------------------------------------------------ */

static int latticeAdd( Lattice* latP, short frameX, WordX w, 
                               float score, char* links)
{
  LatNode* lnP = latNodeCreate( latP, frameX, w);

  if ( lnP->score > score) lnP->score = score;
  if ( links) {
    int   argc; char **argv;
    int   i;

    if ( Tcl_SplitList (itf,links,&argc,&argv) != TCL_OK) return TCL_ERROR;
    for ( i = 0; i < argc; i++) {
      int tokc; char** tokv;
      if ( Tcl_SplitList (itf,argv[i],&tokc,&tokv) != TCL_OK) {
        Tcl_Free((char*) argv);
        return TCL_ERROR;
      }
      if ( latNodeAdd( lnP, latP, tokc, tokv) != TCL_OK) return TCL_ERROR;
      Tcl_Free((char*) tokv);
    }
    Tcl_Free((char*) argv);
  }
  return TCL_OK;
}

static int latticeAddItf( ClientData cd, int argc, char *argv[])
{
  Lattice*  latP   = (Lattice*)cd;
  int       ac     =  argc - 1;
  float     score  =  WORSTSCORE;
  int       from   =  0, word = 0;
  char*     links  =  NULL; 

  if ( itfParseArgv( argv[0], &ac, argv+1, 0,
      "<from>",  ARGV_INT,    NULL, &from,  NULL,"starting point of word",
      "<word>",  ARGV_LINDEX, NULL, &word, &(latP->vocabP->vwordList),
                                                 "word in vocabulary",
      "-score",  ARGV_FLOAT,  NULL, &score, NULL,"path score",
      "-links",  ARGV_STRING, NULL, &links, NULL,"list of links",
       NULL) != TCL_OK) return TCL_ERROR;

  return latticeAdd( latP, (short)from, (WordX)word, score, links);
}

/* ------------------------------------------------------------------------
    latticeClear       Clear a lattice so we don't need to Deinit()+Init()
   ------------------------------------------------------------------------ */

int latticeClearItf( ClientData cd, int argc, char *argv[])
{
  Lattice*  latP   = (Lattice*)cd;
  int       ac     =  argc - 1;

  if ( itfParseArgv( argv[0], &ac, argv+1, 0,
       NULL) != TCL_OK) return TCL_ERROR;

  latticeDealloc( latP);
  lmPtrInit( &(latP->lmPtr));  /* so latticeLM() WILL do the mapping later on */
  return TCL_OK;
}

/* ------------------------------------------------------------------------
    latticeRescore     1-best rescoring of the lattice
   ------------------------------------------------------------------------ */

/* ------------------------------------------------------------------------
    latticeRescore     1-best rescoring of the lattice
                       z,p: new language model weight and penalty
   ------------------------------------------------------------------------ */

static Hypo* latticeBacktrace( Lattice* latP, LatLink* linkP, WordX wordN)
{
  if ( linkP) {
    Hypo* hP    = latticeBacktrace( latP, linkP->bestP, wordN+1);
    WordX wordX = hP->hwordN++;

    if ( linkP->fromP->wordX >= latP->vocabP->firstWordX) {
      hP->hwordA[wordX].vocabX    =  linkP->fromP->wordX;
      hP->hwordA[wordX].wordScore =  linkP->pscore - hP->score;
      hP->hwordA[wordX].wsFrameXF =  linkP->fromP->frameX;
      hP->hwordA[wordX].weFrameXF =  linkP->frameX;
      hP->score                   =  linkP->pscore;
    }
    return hP;
  }
  else {
    Hypo* hP   = hypoCreate( wordN-1);
    hP->hwordN = -1;
    hP->score  =  0.0;
    return hP;
  }
}

static int latticeRescore( Lattice* latP, LMPtr* lmPtr, int mapFlag, float norm)
{
  LatNode*  nodeP;
  LatLink*  linkP, *qheadP = NULL, *qtailP = NULL, *bestP = NULL;
  LatLink*  poorP;
  float     score;
  int       wA[1000];
  int       wN=5;
  int       i;
  int       lastX;

  if ( latticeCheck( latP) != TCL_OK) {
    ERROR("Can't rescore since lattice is inconsistent.\n");
    return TCL_ERROR; 
  }

  if ( latticeLM( latP, lmPtr, mapFlag) != TCL_OK) return TCL_ERROR; 

  /* Initialize FanIn counts and path scores */

  for ( nodeP = latP->nodeP; nodeP; nodeP = nodeP->nextP) 
        nodeP->info.fanIn = 0;

  for ( nodeP = latP->nodeP; nodeP; nodeP = nodeP->nextP) {
    for ( linkP = nodeP->linkP; linkP; linkP = linkP->nextP) {

      (linkP->toP->info.fanIn)++;
       linkP->pscore = WORSTSCORE;
    }
  }


  /* Form the initial queue of the optimal partial paths. These are the
     links out of the start nodes. Path scores include the language model
     scores for the "to" node of the last link in the path, but not the
     acoustic score for that node */

  lmPtr->lmP->resetFct( lmPtr->cd);


  for ( linkP = latP->startP->linkP; linkP; linkP = linkP->nextP) {
    for(i=0;i<wN-1;i++)
      wA[i] = linkP->fromP->lmWordX;
    wA[wN-1] = linkP->toP->lmWordX;
    linkP->pscore = linkP->lscore + lmPtr->lmP->scoreFct( lmPtr->cd, wA, wN, wN-1);
    linkP->bestP  = NULL;

    if (! qheadP) qheadP         = linkP;
    else          qtailP->qnextP = linkP;

    qtailP = linkP;
  }
  qtailP->qnextP = NULL;

  while ( qheadP) {
    nodeP = qheadP->toP;  /* Update path scores for all links out of
                             the node on top of the queue */

    lmPtr->lmP->nextFrameFct( lmPtr->cd);

    wA[wN-2] = nodeP->lmWordX;
    wA[wN-3] = qheadP->fromP->lmWordX;
    for(i=wN-4,poorP=qheadP->bestP,lastX=wA[wN-3];i>=0 && poorP;lastX=wA[i--],poorP=poorP->bestP)
      wA[i]=poorP->fromP->lmWordX;
    for(;i>=0;i--)
      wA[i]=lastX;

    for ( linkP = nodeP->linkP; linkP; linkP = linkP->nextP) {
      wA[wN-1] = linkP->toP->lmWordX;

      score = qheadP->pscore + linkP->lscore + 
              lmPtr->lmP->scoreFct( lmPtr->cd, wA, wN, wN-1);

      if ( score < linkP->pscore) {
        linkP->pscore = score;
        linkP->bestP  = qheadP; 
      }
    }

    if ( --(nodeP->info.fanIn) == 0) {

      /* Links out of the qheadP->toP node are updated wrt all incoming
         links at that node. They all now have optimal partial path scores;
         insert them in the optimal path queue */

      for ( linkP = nodeP->linkP; linkP; linkP = linkP->nextP) {
        qtailP->qnextP = linkP;
        qtailP         = linkP;
      }
      qtailP->qnextP = NULL;
    }
    qheadP = qheadP->qnextP;
  }
  
  /* Rescored all path to the end silence node. Traceback the optimal path.
     First find the best link entering the end silence node */

  score = WORSTSCORE;

  for ( nodeP = latP->nodeP; nodeP; nodeP = nodeP->nextP) {
    for ( linkP = nodeP->linkP; linkP; linkP = linkP->nextP) {
      if ( linkP->toP == latP->finalP && linkP->pscore < score) {
        score = linkP->pscore;
        bestP = linkP;
      }
    }
  }
 
  if ( bestP) {
    Hypo* hP= latticeBacktrace( latP,bestP, 0);
    hP->score = bestP->pscore + latP->finalAScore;
    hypoListReinit( &(latP->hypoList));    
    hypoListAppend( &(latP->hypoList),hP);
  }
  else { ERROR( "Cannot reach final node.\n"); return TCL_ERROR; }

  return TCL_OK;
}

static int latticeRescoreItf( ClientData cd, int argc, char *argv[])
{
  Lattice*  latP   = (Lattice*)cd;
  int       ac     =  argc - 1;
  int       rc     =  TCL_OK;
  int       map    =  1;
  LMPtr     lmP;
  float     norm=0;

  lmPtrInit( &lmP);

  if ( itfParseArgv( argv[0], &ac, argv+1, 0,
      "<lm>",  ARGV_CUSTOM, getLMPtr, &lmP, NULL,"language model",
      "-norm", ARGV_FLOAT,  NULL,     &norm,NULL,"normalize lattice",
      "-map",  ARGV_INT,    NULL,     &map, NULL,"map on baseform (default on)",
       NULL) != TCL_OK) {
    lmPtrDeinit( &lmP);
    return TCL_ERROR;
  }

  rc = latticeRescore( latP, &lmP, map, norm);

  lmPtrDeinit( &lmP);
  return rc;
}


/* ------------------------------------------------------------------------
    latticeWrite -- our own format
   ------------------------------------------------------------------------ */

static int latticeWrite( Lattice* latP, char* fileName, char* mode, char* id, int frame_offset, float factor)
{
  FILE *fp;

  if ((fp=fileOpen(fileName, mode))==NULL) {
    ERROR( "Can't open lattice file '%s' for writing.\n",fileName);
    return TCL_ERROR;
  }
  else {
    LatNode* nodeP;
  
    if ( id) fprintf( fp, "%%TURN: %s\n", id);
    fprintf( fp, "BEGIN_LATTICE\n");

    for ( nodeP = latP->nodeP; nodeP; nodeP = nodeP->nextP) {
      LatLink* linkP = nodeP->linkP;

      fprintf( fp, "%d {%s}", nodeP->frameX + frame_offset,
                              vocabName(latP->vocabP,nodeP->wordX));
      if ( nodeP->score < WORSTSCORE)
        fprintf( fp, " -score % 2.4e", nodeP->score * factor);

        if ( linkP) {
          fprintf( fp, " -links {");
          for ( ; linkP; linkP = linkP->nextP) {
            fprintf( fp, " {%d {%s}",linkP->toP->frameX + frame_offset,  
                     vocabName(latP->vocabP,linkP->toP->wordX)); 
            fprintf( fp, " %d % 2.4e}", (int)linkP->frameX + frame_offset, linkP->lscore * factor);
          }
          fprintf( fp, "}");
       }
       fprintf( fp, "\n");
     }
     fprintf( fp, "END_LATTICE\n");
  }
  fileClose(fileName, fp);
  return TCL_OK;
}
/* ------------------------------------------------------------------------
    latticeWrite -- VERBMOBIL formats and required stuff.
   ------------------------------------------------------------------------ */
/*-------------------------------------------------------------------------
  vmLinkCompare is used to sort Links in the way required for VM lattices.
  It also sorts equivalent Links by score so we keep the best if redundant
  Links are removed.
  --------------------------------------------------------------------------*/
static int vmLinkCompare(const VOID * a, const VOID *b)
{
  VmLink* la = (VmLink *)a;
  VmLink* lb = (VmLink *)b;
  
  if      ( la->fromNX < lb->fromNX ) return -1;
  else if ( la->fromNX > lb->fromNX ) return  1;
  else if ( la->toNX   < lb->toNX   ) return -1;
  else if ( la->toNX   > lb->toNX   ) return  1;
  else {
   int res = strcmp(la->wordS, lb->wordS);
   if (res != 0) return res;
   else if (la->score > lb->score) return -1;
   else if (la->score < lb->score) return 1;
   else return 0;
  }
}

/*---------------------------------------------------------------------------
  vmLinkDiffer is used to eliminate Links that are not considered to differ
  from Verbmobil Lattices prior to writing them to a file.
  --------------------------------------------------------------------------*/

/* return 0 if same from node, to node and spelling, else 1 */
static INT vmLinkDiffer(const VOID * a, const VOID *b)
{
  VmLink* la = (VmLink *)a;
  VmLink* lb = (VmLink *)b;
  
  if      ( la->fromNX != lb->fromNX) return 1;
  else if ( la->toNX   != lb->toNX  ) return 1;
  else if ( la->isNoise && lb->isNoise) return 0;
  else if (strcmp(la->wordS,lb->wordS)!=0) return 1;
  else return 0;
}




/*---------------------------------------------------------------------------
  it's much more work to manipulate the lattice into the correct form,
  remove redundant arcs etc than to write it...
  --------------------------------------------------------------------------
  latticeWriteVM with a NULL FileName returns resault via itfResultAppend 
  --------------------------------------------------------------------------*/

static int latticeWriteVM( Lattice* latP, char* fileName, 
                    char* filemode, int withLM, char* id, int frame_offset, LMPtr * lmP, int mapFlag, float factor)
{

  FILE*      fp = NULL;
  LatNode*   nodeP;
  int        vmLinkN;
  int        inDeg, i, l, n;
  int        (*cmp)(const void* a, const void *b) = vmLinkCompare;
  VmLink *   vmLinkA;

  if(!lmP || lmP->cd==0)
    FATAL("Writing VM lattices requires to state a language model via the \"-lm\" option of writeVM/putsVM/puts methods\n");

  if (fileName) {
    if ((fp=fileOpen(fileName, filemode))==NULL) {
      ERROR( "Can't open lattice file '%s' for writing.\n",fileName);
      return TCL_ERROR;
    }
  }

  latticeCheck( latP);
  INFO("lattice information for DEBUGGING: n:%d l:%d f:%d \n",latP->nodeN, latP->linkN, latP->frameN);
  if ( latticeLM( latP, lmP, mapFlag) != TCL_OK) return TCL_ERROR; 


  if (fileName) {
    INFO ("writing lattice to %s mode %d\n",fileName,withLM);
    /* Enumerate nodes (differs depending on withLM) and find in-degree */
  }
 
  n = latP->nodeN;
  inDeg = 0;

  if (withLM) {
    for ( nodeP = latP->nodeP; nodeP; nodeP = nodeP->nextP) {
      LatLink* linkP = nodeP->linkP;

      for ( ; linkP; linkP = linkP->nextP)
        if ( linkP->toP == latP->finalP) inDeg++;
      nodeP->info.nodeX = n--;
    }
  } else {
    int * newNodeXA = malloc((latP->frameN+1)*sizeof(int)); 
    int fX;
    int newX = 1;
    
    /* first mark all frames where words start -- those will be new nodes */
    
    memset((char*) newNodeXA, 0, (latP->frameN+1) *sizeof(int)); 
    for ( nodeP = latP->nodeP; nodeP; nodeP = nodeP->nextP)
                                     newNodeXA[nodeP->frameX]= 1;
    
    /* now renumber them as we need consecutive node numbers */
    
    for (fX = 0; fX < latP->frameN+1; fX ++) 
                      if (newNodeXA[fX]) newNodeXA[fX] = newX++;
    
    /* now use these numbers in the nodes */
    
    for ( nodeP = latP->nodeP; nodeP; nodeP = nodeP->nextP) {
      LatLink* linkP = nodeP->linkP;
      for ( ; linkP; linkP = linkP->nextP) 
                    if ( linkP->toP == latP->finalP) inDeg++;
      nodeP->info.nodeX = newNodeXA[nodeP->frameX];
    }
    free (newNodeXA);
  }

  /* now convert nodes+links into vm-links: */
  
  vmLinkN = inDeg + latP->linkN;
  i       = inDeg;
  vmLinkA = (VmLink*)malloc( vmLinkN * sizeof(VmLink));

  if (lmP->cd) lm_CacheReset(lmP);

  l=latP->linkN;
  for ( nodeP = latP->nodeP; nodeP; nodeP = nodeP->nextP) {
    LatLink* linkP = nodeP->linkP;

    for ( ; linkP; linkP = linkP->nextP) {
      float lmScore;
      lmScore = lmP->cd ? lm_BgScore(lmP,linkP->fromP->lmWordX, linkP->toP->lmWordX) : 0;
      l--;
      vmLinkA[l].fromNX   = linkP->fromP->info.nodeX;
      vmLinkA[l].toNX     = linkP->toP->info.nodeX;
      vmLinkA[l].wordS    = latP->vocabP->vwordList.itemA[linkP->fromP->wordX].spellS;
      vmLinkA[l].fromFX   = nodeP->frameX;
      vmLinkA[l].toFX     = linkP->frameX;
      vmLinkA[l].score    = linkP->lscore + lmScore;
      vmLinkA[l].lmScore  = lmScore;
      vmLinkA[l].isNoise  = (vmLinkA[l].wordS[0] == '+'); /* HACK ATTACK !! */

      if ( i > 0 && linkP->toP == latP->finalP) {
        INT k = latP->linkN + (--i);
        
        vmLinkA[k].fromNX   = linkP->toP->info.nodeX;
        vmLinkA[k].toNX     = linkP->toP->info.nodeX + 1;
        vmLinkA[k].wordS    = latP->vocabP->vwordList.itemA[linkP->toP->wordX].spellS;
        vmLinkA[k].fromFX   = linkP->toP->frameX;
        vmLinkA[k].toFX     = latP->frameN-1;
        vmLinkA[k].score    = latP->finalAScore;
        vmLinkA[k].lmScore  = 0.0;
        vmLinkA[k].isNoise  = 0;
      }
    }
  }

 /* Sort edges according to Verbmobil conventions -- also better scores first */

  qsort( vmLinkA, vmLinkN, sizeof(VmLink), cmp);
  
  /* Remove redundant arcs */
  {
    int newX = 1;
    for (l = 1;l < vmLinkN;l ++) {
      if (vmLinkDiffer(vmLinkA+l,vmLinkA+newX-1)) {
         if (l !=newX) memcpy(vmLinkA+newX,vmLinkA+l,sizeof(VmLink));
         newX++;
      } 
    }
    vmLinkN = newX;  
  }

  /* ah, and NOW write it */
  
  if (fileName) {

    if (id != NULL) {
      fprintf(fp, "%%TURN: %s\n",id);
    } else {
      fprintf(fp, "%%TURN: %s\n","(none)");
    }
    fprintf(fp, "BEGIN_LATTICE\n");
    
    for (l = 0; l < vmLinkN; l ++) {
      VmLink * linkP = vmLinkA + l;
      if (withLM) {
	fprintf(fp,"%d %d %s %f %d %d (AP (lm %f))\n",
		linkP->fromNX, linkP->toNX, linkP->wordS,
		linkP->score * factor, linkP->fromFX + frame_offset, linkP->toFX+frame_offset, linkP->lmScore * factor);
      } else {
	fprintf(fp,"%d %d %s %f %d %d\n",
		linkP->fromNX, linkP->toNX, linkP->wordS,
		(linkP->score - linkP->lmScore) * factor, linkP->fromFX+frame_offset, linkP->toFX+frame_offset);
      }
    }
  
    fprintf(fp, "END_LATTICE\n\n");
    fileClose(fileName, fp);
  } else {
    /* Without Filename we get the result via itfAppendResult */
    latticePutsAppendResult(vmLinkA, vmLinkN, id, withLM, frame_offset, factor);
  }
  free (vmLinkA);

  return TCL_OK;
}


static int latticePutsVM( Lattice* latP, int withLM, char* id, int frame_offset, LMPtr * lmP, int mapFlag, float factor)
{
  /* latticeWriteVM with a NULL-Ptr FileName returns the result via itfResultAppend */
  return latticeWriteVM(latP, NULL, NULL, withLM, id, frame_offset, lmP, mapFlag, factor);
}
/* ------------------------------------------------------------------------
    latticePutsItf
   ------------------------------------------------------------------------ */

static int latticePutsPROPItf(Lattice* latP, int frame_offset, float factor) {
  LatNode* nodeP;

  for ( nodeP = latP->nodeP; nodeP; nodeP = nodeP->nextP) {
    LatLink* linkP = nodeP->linkP;
    
    itfAppendResult(  "%d", nodeP->frameX + frame_offset);
    itfAppendElement( "%s", vocabName(latP->vocabP,nodeP->wordX));
    
    if ( nodeP->score < WORSTSCORE)
      itfAppendResult(  " -score % 2.4e", nodeP->score * factor);
    
    if ( linkP) {
      itfAppendResult(  " -links {");
      for ( ; linkP; linkP = linkP->nextP) {
        itfAppendResult(" {%d",linkP->toP->frameX + frame_offset); 
        itfAppendElement( "%s", vocabName(latP->vocabP,linkP->toP->wordX)); 
	itfAppendResult( " %d % 2.4e}", (int)linkP->frameX + frame_offset, linkP->lscore * factor); 
	/* itfAppendResult( " %d % 2.4e", (int)linkP->frameX + frame_offset, linkP->lscore); 
	   itfAppendResult( " -pscore  % 2.4e", linkP->pscore); */
      }
      itfAppendResult( "}");
    }
    itfAppendResult("\n");
  }
  return TCL_OK;
}


static int latticePutsItf( ClientData cd, int argc, char *argv[])
{

  Lattice* latP   = (Lattice*)cd;


  int      ac           =  argc - 1;
  char*    format_str   = "PROP";
  char*    id_str       = "none";
  int      frame_offset = 0;
  float    s_factor     = 1.0;
  int      mapFlag      = 1;
  LMPtr    lmP;


  lmPtrInit(&lmP);

  if ( itfParseArgv( argv[0], &ac, argv+1, 0,
      "-format",       ARGV_STRING, NULL, &format_str, NULL,   "Output Format PROP/VM1/VM2 ",
      "-id",           ARGV_STRING, NULL, &id_str, NULL,       "id of VM1/VM2-Lattice ",
      "-frame_offset", ARGV_INT,    NULL, &frame_offset, NULL, "offset added to each frame indices ",
      "-factor",    ARGV_FLOAT,     NULL, &s_factor, NULL,     "multiply scores with this factor",
      "-lm",           ARGV_CUSTOM, getLMPtr, &lmP, NULL,      "language model",
      "-mapFlag",      ARGV_INT,    NULL, &mapFlag, NULL,      "map on baseform (default on)",

       NULL) != TCL_OK) return TCL_ERROR;

  if (strcmp(format_str,"PROP") == 0) { 
    latticePutsPROPItf(latP, frame_offset,s_factor);
  } else if (strcmp(format_str,"VM1") == 0) {
    /* Output Lattice in VM format without LM */
    return latticePutsVM(latP,0,id_str, frame_offset,&lmP,mapFlag,s_factor);
  } else if (strcmp(format_str,"VM2") == 0) {
    /* Output Lattice in VM format with LM */
    latticePutsVM(latP,1,id_str, frame_offset,&lmP,mapFlag,s_factor);
  } else {
    ERROR("Unknown Lattice Format (%s)!\n",format_str);
    lmPtrDeinit(&lmP);
    return TCL_ERROR;
  }

  lmPtrDeinit(&lmP);
  return TCL_OK;
}


static int latticeWriteItf( ClientData cd, int argc, char *argv[])
{
  Lattice* latP  = (Lattice*)cd;
  int      ac    =  argc - 1;
  char*    fname =  NULL;
  char*    mode  =  "a";
  char*    id    =  NULL;
  int      frame_offset = 0;
  float    s_factor = 1.0;
  char *   format = latP->format;
  int      mapFlag = 1;
  LMPtr    lmP;
  int      ret   = TCL_ERROR;
  
  lmPtrInit(&lmP);
  if ( itfParseArgv( argv[0], &ac, argv + 1, 0,
      "<filename>", ARGV_STRING, NULL, &fname,  NULL,
                    "filename to write to ",
      "-mode",      ARGV_STRING, NULL, &mode,   NULL,
                    "filemode a=apend,w=overwrite ",
      "-format",    ARGV_STRING, NULL, &format,     NULL,
                    "fileformat PROP | VM1 | VM2 ",
      "-id",        ARGV_STRING, NULL, &id, NULL,
                    "utterance ID in file ",
      "-frame_offset", ARGV_INT,    NULL, &frame_offset, NULL,
		    "offset added to each frame indices ",
      "-factor",    ARGV_FLOAT,     NULL, &s_factor, NULL,
		    "multiply scores with this factor",
      "-lm",         ARGV_CUSTOM, getLMPtr, &lmP, NULL,"language model",
      "-mapFlag",      ARGV_INT,    NULL, &mapFlag, NULL, "map on baseform (default on)",
       NULL) != TCL_OK) return TCL_ERROR;

  if      (!strcmp(format,"PROP"))   ret= latticeWrite  (latP,fname,mode,id,frame_offset,s_factor);
  else if(!strcmp(format,"VM1"))     ret= latticeWriteVM(latP,fname,mode,0,id,frame_offset,&lmP,mapFlag,s_factor);
  else if(!strcmp(format,"VM2"))     ret= latticeWriteVM(latP,fname,mode,1,id,frame_offset,&lmP,mapFlag,s_factor);
  else                               ERROR("unknown lattice format %s (must be PROP or VM1 or VM2\n",format);

  lmPtrDeinit(&lmP);
  return ret;
}



/* ========================================================================
    latticeHMM
   ======================================================================== */

typedef struct {

  int wordX;
  int nextX;

} BGNODE;

/* ------------------------------------------------------------------------
    latticeFindBGNode  find all word bigram nodes that need to be allocated
                       in the HMM.
   ------------------------------------------------------------------------ */

static int latticeFindBGNode( int*     wordA,  int  wordN,
                       BGNODE** wordBA, int* wordBN, int w, int v)
{
  if ( w < 0 || w >= wordN) return -1;
  if ( wordA[w] < 0) {
     *wordBA                 = (BGNODE*)realloc(*wordBA, sizeof(BGNODE) * 
                                               (*wordBN+1));
    (*wordBA)[*wordBN].wordX =  v;
    (*wordBA)[*wordBN].nextX = -1;
    wordA[w]                 = (*wordBN)++;
    return wordA[w];
  } else {
    BGNODE* wordB = *wordBA;

    int     k     =  wordA[w];
    while ( wordB[k].nextX > -1 && wordB[k].wordX != v) { k = wordB[k].nextX; }
    if ( wordB[k].wordX != v) {
       *wordBA                 = (BGNODE*)realloc(*wordBA, sizeof(BGNODE) * 
                                                 (*wordBN+1));
      (*wordBA)[k].nextX       =  *wordBN;
      (*wordBA)[*wordBN].wordX =   v;
      (*wordBA)[*wordBN].nextX =  -1;
      return (*wordBN)++;
    }
    else return k;
  }
}

/* ------------------------------------------------------------------------
    latticeHMM  translates a lattice to an HMM incorporating a trigram
                language model (i.e. each word node represents a bigram)
   ------------------------------------------------------------------------ */

static int latticeHMM( LMPtr* lmP, Lattice* latP, float zn, float pn, float silPen)
{

  Vocab*     vocabP = latP->vocabP;
  VWord*     vocabA = latP->vocabP->vwordList.itemA;
  LatNode*   nodeP;


  int        i,m;
  int        inDeg  = 0;
  int        wordN  = latP->vocabP->vwordList.itemN;
  int*       wordA  = NULL;
  BGNODE*    wordBA = NULL;
  int        wordBN = 0;

  int*       hmmA   = NULL;
  int        hmmN   = 0;
  int*       trN    = NULL;
  int**      trIdx  = NULL;
  float**    trPen  = NULL;

  if (! lmP) {
    ERROR("No current LM defined.\n");
    return TCL_ERROR; 
  }
  if ( latticeCheck( latP) != TCL_OK) {
    ERROR("Can\'t make HMM since lattice is inconsistent.\n");
    return TCL_ERROR; 
  }

  wordA = (int*)malloc( wordN * sizeof(int));
  assert( wordA );

  for ( i = 0; i < wordN; i++) wordA[i] = -1;

  /* In order to find the number of nodes and the list of all bigrams
     or bigram nodes in the lattice, we go through the lattice once and
     add each link to the BGNODE list wordBA */

  for ( nodeP = latP->nodeP; nodeP; nodeP = nodeP->nextP) {
    LatLink* linkP = nodeP->linkP;

    for ( ; linkP; linkP = linkP->nextP) {
      if ( linkP->toP == latP->finalP) inDeg++;

      latticeFindBGNode( wordA, wordN, &wordBA, &wordBN,
                         vocabA[linkP->toP->wordX].realX,
                         vocabA[linkP->fromP->wordX].realX);
    }
 }

  /* allocate the HMM node array */

  hmmN    =  2*wordBN+1;
  hmmA    = (int   *)malloc(hmmN * sizeof(int));
  trN     = (int   *)malloc(hmmN * sizeof(int));
  trIdx   = (int  **)malloc(hmmN * sizeof(int*));
  trPen   = (float**)malloc(hmmN * sizeof(float*));

  for (  i =  0; i < hmmN;  i++) { hmmA[i] = -1; trN[i]   = 0; 
                                                 trIdx[i] = NULL;
                                                 trPen[i] = NULL; }

  hmmA[0] =  vocabP->startWordX;

  for (  i =  0; i < wordN; i++) {
    int  k =  wordA[i];
    if ( k > -1) {
      do { 
        hmmA[2*k+1] = i;
        hmmA[2*k+2] = vocabP->silenceWordX;
      } while ( ( k = wordBA[k].nextX) >= 0);
    }
  }

  for ( i = 0; i < hmmN; i++) {
    if ( i % 2) {
      trN  [i]    = 1;
      trIdx[i]    = (int  *)malloc( sizeof(int));
      trPen[i]    = (float*)malloc( sizeof(float));
      trIdx[i][0] = 1;
      trPen[i][0] = silPen;
    }
  }

  for ( nodeP = latP->nodeP; nodeP; nodeP = nodeP->nextP) {
    LatLink* linkP = nodeP->linkP;

    for ( ; linkP; linkP = linkP->nextP) {
      LatLink* lP = linkP->toP->linkP;
      int      k  = latticeFindBGNode( wordA, wordN, &wordBA, &wordBN,
                                       vocabA[linkP->toP->wordX].realX,
                                       vocabA[linkP->fromP->wordX].realX);

      if ( linkP->fromP->wordX == vocabP->startWordX) {
        int   o  = 2*k+1;
        for ( m  = 0; m < trN[0] && trIdx[0][m] != o; m++);
        if (  m >= trN[0]) {
          trN[0]++;
          trIdx[0] = (int  *)realloc( trIdx[0], trN[0] * sizeof(int));
          trPen[0] = (float*)realloc( trPen[0], trN[0] * sizeof(float));
          trIdx[0][trN[0]-1] =  o;
          trPen[0][trN[0]-1] = (lm_TgScore( lmP, vocabP->startWordX,
                                            vocabP->startWordX,
                                            vocabA[linkP->toP->wordX].realX));
        }
      }
      if ( linkP->toP->wordX == vocabP->finishWordX) { 
        int   o  = hmmN-2*k-1;
        int   p  = 2*k+1;

        for ( m  = 0; m < trN[p] && trIdx[p][m] != o; m++);
        if (  m >= trN[p]) {
          trN[p]++;
          trIdx[p] = (int  *)realloc( trIdx[p], trN[p] * sizeof(int));
          trPen[p] = (float*)realloc( trPen[p], trN[p] * sizeof(float));
          trIdx[p][trN[p]-1] = o;
          trPen[p][trN[p]-1] = 0.0;
        }
        p = 2*k+2;

        for ( m  = 0; m < trN[p] && trIdx[p][m] != o; m++);
        if (  m >= trN[p]) {
          trN[p]++;
          trIdx[p] = (int  *)realloc( trIdx[p], trN[p] * sizeof(int));
          trPen[p] = (float*)realloc( trPen[p], trN[p] * sizeof(float));
          trIdx[p][trN[p]-1] = o;
          trPen[p][trN[p]-1] = 0.0;
        }
      }
      for ( ; lP; lP = lP->nextP) {
        int l = latticeFindBGNode( wordA, wordN, &wordBA, &wordBN,
                                   vocabA[lP->toP->wordX].realX,
                                   vocabA[lP->fromP->wordX].realX);
        int   o  = 2*(l-k);
        int   p  = 2*k+1;

        for ( m  = 0; m < trN[p] && trIdx[p][m] != o; m++);
        if (  m >= trN[p]) {
          trN[p]++;
          trIdx[p] = (int  *)realloc( trIdx[p], trN[p] * sizeof(int));
          trPen[p] = (float*)realloc( trPen[p], trN[p] * sizeof(float));
          trIdx[p][trN[p]-1] =  o;
          trPen[p][trN[p]-1] = (lm_TgScore( lmP, vocabP->startWordX,
                                            vocabP->startWordX,
                                            vocabA[linkP->toP->wordX].realX));
        }
        o = o - 1;
        p = 2*k+2;

        for ( m  = 0; m < trN[p] && trIdx[p][m] != o; m++);
        if (  m >= trN[p]) {
          trN[p]++;
          trIdx[p] = (int  *)realloc( trIdx[p], trN[p] * sizeof(int));
          trPen[p] = (float*)realloc( trPen[p], trN[p] * sizeof(float));
          trIdx[p][trN[p]-1] =  o;
          trPen[p][trN[p]-1] = (lm_TgScore( lmP, vocabP->startWordX,
                                            vocabP->startWordX,
                                            vocabA[linkP->toP->wordX].realX));
        }
      }
    }
  }

  itfAppendResult("{");
  for ( i = 0; i < hmmN; i++) {
    itfAppendElement( "%s", vocabA[hmmA[i]].spellS);
  }
  itfAppendResult("} {");
  for ( i = 0; i < hmmN; i++) {
    int k;
    itfAppendResult(" {");
    for ( k = 0; k < trN[i]; k++) {
      itfAppendElement("%d %f",trIdx[i][k], trPen[i][k]);
    }
    itfAppendResult("}");

    if ( trIdx[i]) { free( trIdx[i]); trIdx[i] = NULL; }
    if ( trPen[i]) { free( trPen[i]); trPen[i] = NULL; }
  }
  itfAppendResult("} {0}");

  if ( wordA)  { free( wordA);  wordA  = NULL; }
  if ( wordBA) { free( wordBA); wordBA = NULL; }
  if ( hmmA)   { free( hmmA);   hmmA   = NULL; }
  if ( trN)    { free( trN);    trN    = NULL; }
  if ( trIdx)  { free( trIdx);  trIdx  = NULL; }
  if ( trPen)  { free( trPen);  trPen  = NULL; }

  return TCL_OK;
}

static int latticeHMMItf( ClientData cd, int argc, char *argv[])
{
  LMPtr     lmP;
  Lattice*  latP   = (Lattice*)cd;
  int       ac     =  argc - 1;
  float     z      =  0, 
            p      =  0;
  float     silPen =  0.5;
  int rz;
  lmPtrInit( &lmP);
 
  if ( itfParseArgv( argv[0], &ac, argv+1, 0,
      "<lm>",  ARGV_CUSTOM, getLMPtr, &lmP, NULL,"language model",
      "-lz",  ARGV_FLOAT, NULL, &z, NULL,"new Lm weight",
      "-lp",  ARGV_FLOAT, NULL, &p, NULL,"new Lm insertion penalty",
      "-silenceWordPenalty",  ARGV_FLOAT, NULL, &silPen, NULL,
      "silence word insertion penalty",
       NULL) != TCL_OK) return TCL_ERROR;

  rz= latticeHMM(&lmP, latP, z, p, silPen);

  lmPtrDeinit( &lmP);
  return rz;
}


/* ------------------------------------------------------------------------
    latticeNodeElement
   ------------------------------------------------------------------------ */

static int latticeNodeElement(AStar*  searchObjectP, AStarNodeP nodeP) {
  return ((LatNode*)nodeP)->lmWordX;
}


/* ------------------------------------------------------------------------
    latticePathEnd
   ------------------------------------------------------------------------ */

static int latticePathEnd(AStar* searchObjectP, AStarNode * nodeP) {
  Lattice * latP = (Lattice *)searchObjectP->searchStructureP;
  return latP->finalP == (LatNode*)(nodeP->AStarElement);
}

static float latticeLookahead(AStar * searchObjectP, AStarNode* pP) {

  /* Check if it is an HMM of ngrams.
     If yes, a special lookahead is used */

  if(pP && pP->AStarElement) {
    LatNode * lnP = (LatNode*)(pP->AStarElement);
    if(searchObjectP->lm.lmP == &HMM3gramLM) {
      HMM3gramState* hmmStateP = (HMM3gramState*)pP->State;
      if(hmmStateP->subState==HMM3gramWord || hmmStateP->subState==HMM3gramTerminatedSpeechAct)
	return lnP->info.breakScore;
    }
    return lnP->score;
  }  
  return 0.0;
}

static int latticeNodeTime(AStar * searchObjectP, AStarNode* nodeP) {
  if(!nodeP) return  ((Lattice*)searchObjectP->searchStructureP)->startP->frameX;
  return nodeP->AStarElement ? ((LatNode*)(nodeP->AStarElement))->frameX : 0;
}

static AStarArcP  latticeNodeOutputArcs(AStar * searchObjectP, AStarNode * nodeP) {
  AStarNodeP nP;
  
  if(nodeP && nodeP->AStarElement)
    nP = nodeP->AStarElement;
  else
    nP = (AStarNodeP)((Lattice*)searchObjectP->searchStructureP)->startP;
  return (AStarArcP)((LatNode*)nP)->linkP;
}

static AStarArcP  latticeNextOutputArc(AStar * searchObjectP, AStarArcP arcP) {
  return arcP==NULL ? (AStarArcP)NULL : (AStarArcP)(((LatLink*)arcP)->nextP);
}

static AStarNodeP latticeArcNode(AStar * searchObjectP, AStarArcP arcP) {
  return (AStarNodeP)(((LatLink *)arcP)->toP);
}

static float latticeArcTransition(AStar * searchObjectP, AStarArcP arcP) {
  return ((LatLink *)arcP)->lscore;
}

/* ------------------------------------------------------------------------
    latticePathBacktrace
   ------------------------------------------------------------------------ */

static LatLink * findArc(LatNode * fromP, LatNode * toP) {
  LatLink * retP;
  for(retP = fromP->linkP; retP; retP = retP->nextP)
    if(retP->toP==toP)
      return retP;
  FATAL("findArc failed\n");
  return NULL;
}
	

static void latticePathBacktraceNew( AStar * searchObjectP, int hypoN, AStarPath** hypoP, AStarNode * nodeP)
{
  Lattice * latP= (Lattice*)searchObjectP->searchStructureP;
  int wordX;
  float score = nodeP->score;

  Hypo* hP =  hypoCreate(hypoN+1);

  for(wordX=0;wordX<hypoN+1;wordX++)
    hP->hwordA[wordX].wordScore=0;

  for(wordX=0;wordX<hypoN+1;wordX++) {

    LatNode * nodeP =  wordX==0 ? latP->startP : (LatNode*) (hypoP[wordX-1]->AStarElement);
    LatLink * linkP =  wordX==hypoN ? NULL : findArc(nodeP, (LatNode*)(hypoP[wordX]->AStarElement));

    hP->hwordA[wordX].vocabX    =  nodeP->wordX;
    /*
    if(wordX>0) {
      hP->hwordA[wordX].wordScore +=  hypoP[wordX-1]->score - nodeP->score; 
      if(wordX-1>0)
        hP->hwordA[wordX].wordScore -= hypoP[wordX-2]->score - ((LatNode*) (hypoP[wordX-2]->AStarElement))->score;
    }
    */
    hP->hwordA[wordX].wordScore = 0;
    if(wordX+1<hypoN+1) {
      hP->hwordA[wordX+1].wordScore -= linkP->lscore;
      hP->hwordA[wordX].wordScore += linkP->lscore;      
    }
    hP->hwordA[wordX].wsFrameXF =  nodeP->frameX;
    hP->hwordA[wordX].weFrameXF =  linkP ? linkP->frameX : hP->hwordA[wordX].wsFrameXF;

    if(searchObjectP->lm.lmP->stateMemPrintFct) { /* Copy object state */
      char stateString[100000];

      if(searchObjectP->lm.lmP->stateMemPrintFct(searchObjectP->lm.cd,
                                                 wordX==0 ? NULL : hypoP[wordX-1]->StateMem,
                                                 stateString)) {
	hP->hwordA[wordX].stateString = strdup(stateString);
      } else {
	hP->hwordA[wordX].stateString = 0;
      }
    }
  }
  hP->score = score;

  hypoListAppend( &(latP->hypoList), hP); 
}


/* ------------------------------------------------------------------------
    latticeFindMBest   generates a list of the N best hypothesis stored in 
                       the lattice.

                       zn,pn: new language model weight and penalty
   ------------------------------------------------------------------------ */

static int latticeAStar(Lattice * latP, LMPtr* lmPtr, LMPtr* lmLookahead, int mapFlag)
{
  LatLink*  linkP  = NULL;
  LatLink** topsort= NULL;
  int       topsort_current;
  int wordA[1024];
  int wordB[1024];
  int       startX;

  float score;

  LatNode*  nodeP;
  LatLink*  qheadP = NULL;

  if(lmLookahead->cd==NULL)
    lmLookahead=lmPtr;

  if ( latticeCheck( latP) != TCL_OK) {
    ERROR("Can't extract N-best list since lattice is inconsistent.\n");  return TCL_ERROR; 
  }

  if(!latP->startP->linkP) {  INFO("Lattice is empty"); return TCL_OK; }
  if ( latticeLM( latP, lmLookahead, mapFlag) != TCL_OK) return TCL_ERROR; 

  /* Initialize FanIn counts and path scores */

  for ( nodeP = latP->nodeP; nodeP; nodeP = nodeP->nextP) { nodeP->info.fanIn = 0; if(nodeP!=latP->finalP) nodeP->score = 0; }
  for ( nodeP = latP->nodeP; nodeP; nodeP = nodeP->nextP)
    for ( linkP = nodeP->linkP; linkP; linkP = linkP->nextP)
      (linkP->toP->info.fanIn)++;

  /* Form the initial queue of the partial paths. These are the
     links out of the start nodes.  */

  for ( linkP = latP->startP->linkP; linkP; linkP = linkP->nextP)
    linkP->qnextP=linkP->nextP;

  topsort = calloc(latP->linkN,sizeof(LatLink *));

  for ( qheadP =  latP->startP->linkP, topsort_current=0;
	qheadP && topsort_current<latP->linkN;
	topsort_current++) {
    topsort[topsort_current]=qheadP; nodeP = qheadP->toP;
    qheadP->pscore = 0;	
    qheadP = qheadP->qnextP;
    if ( --(nodeP->info.fanIn) == 0)
      for ( linkP = nodeP->linkP; linkP; linkP = linkP->nextP) {
	linkP->qnextP = qheadP; qheadP = linkP;
      }
  }

  if(topsort_current!=latP->linkN) {
    FATAL("topsort_current(%ld)!=latP->linkN(%ld)\n",topsort_current,latP->linkN);
  }

  
  /* Calculate the optimal backward path starting at the last node */

  for ( nodeP = latP->nodeP; nodeP; nodeP = nodeP->nextP) { nodeP->info.breakScore = nodeP->score = 0; }

  lmLookahead->lmP->nextFrameFct( lmLookahead->cd);
  startX = lmLookahead->lmP->indexFct( lmLookahead->cd, "<s>");

  for(topsort_current=latP->linkN-1;topsort_current>=0;topsort_current--) {
    qheadP=topsort[topsort_current];

    nodeP = qheadP->toP;  /* Update path scores for all links out of the node on top of the queue */
    wordA[0] = qheadP->fromP->lmWordX;
    wordA[1] = wordB[0] = nodeP->lmWordX; 
    wordB[1] = startX; 

    for ( linkP = nodeP->linkP;
	  linkP;
	  linkP = linkP->nextP) {

      wordA[2] = wordB[2] = linkP->toP->lmWordX;

      score = linkP->lscore + linkP->pscore + lmLookahead->lmP->scoreFct( lmLookahead->cd, wordA, 3, 2);
      if( (score < qheadP->pscore) || (qheadP->pscore==0)) { /* Pick best path possible */
	qheadP->pscore = score;
	qheadP->bestP  = linkP;

	if( (score < nodeP->score) || (nodeP->score==0)) { 
	  nodeP->score = score;	  
	}	
      }

      score = linkP->lscore + linkP->pscore + lmLookahead->lmP->scoreFct( lmLookahead->cd, wordB, 3, 2);
      if( (score < nodeP->info.breakScore) || (nodeP->info.breakScore==0)) { 
	nodeP->info.breakScore = score;	  
      }	      
    }
  }
  free(topsort); /* The traversal has been done */

  if((lmPtr!=lmLookahead) && (latticeLM( latP, lmPtr, mapFlag) != TCL_OK)) return TCL_ERROR; 

  return TCL_OK;
}


static int latticeAStarItf( ClientData cd, int argc, char *argv[])
{
  AStar     searchObject;
  int       ac     =  argc - 1;
  int       ret=TCL_OK;
  LMPtr     lmLookahead;
  Lattice * latP  = (Lattice *)cd;
  int       mapFlag = 1;
  Timer     stopit;

  searchObject.searchStructureP   = cd;

  /* Configuration */

  searchObject.n            =  1;
  searchObject.context      =  -1;
  searchObject.maxPathN     =  1000;
  searchObject.maxHypTriedN =  1000000;

  lmPtrInit( &searchObject.lm); lmPtrInit( & lmLookahead);

  if ( itfParseArgv( argv[0], &ac, argv+1, 0,
      "<lm>",          ARGV_CUSTOM, getLMPtr, &searchObject.lm, NULL,"language model",
      "<n>",           ARGV_INT,    NULL, &searchObject.n, NULL,"number of alternative hypos",
      "-map",          ARGV_INT,    NULL, &mapFlag,     NULL, "map on baseform (default on)",
      "-lmLookahead",  ARGV_CUSTOM, getLMPtr, &lmLookahead, NULL,"language model for the lookahead",
      "-maxPathN",     ARGV_INT,    NULL, &searchObject.maxPathN,     NULL, "number of paths in stack",
      "-maxHypTriedN", ARGV_INT,    NULL, &searchObject.maxHypTriedN, NULL, "number of hypos to test",
      "-context",      ARGV_INT,    NULL, &searchObject.context, NULL, "size of equivalence class context for 1-best extraction (set to 2 for trigrams, <0 for nbest)",
       NULL) != TCL_OK) return TCL_ERROR;

  timerReset(&stopit); timerStart(&stopit);

  ret = latticeAStar( latP, &searchObject.lm,& lmLookahead, mapFlag);

  /* Configure A* */

  searchObject.pathBacktrace = latticePathBacktraceNew;
  searchObject.nodeElement = latticeNodeElement;
  searchObject.pathEnd     = latticePathEnd; 
  searchObject.nodeLookahead = latticeLookahead;
  searchObject.nodeOutputArcs = latticeNodeOutputArcs; /* Begin of search == NULL */
  searchObject.nextOutputArcs = latticeNextOutputArc;
  searchObject.arcNode = latticeArcNode;
  searchObject.arcTransition = latticeArcTransition;
  searchObject.nodeTime = latticeNodeTime;
  searchObject.skipOutputArcs = NULL;

  searchObject.searchStateCreate = NULL;
  searchObject.searchStateFree   = NULL;
  searchObject.searchStateHash   = NULL;
  searchObject.searchStateEq     = NULL;
  searchObject.searchStateCopy   = NULL;
  searchObject.arcSearchState    = NULL;  


  hypoListReinit( &(latP->hypoList));

  AStarSearch(&searchObject);

  lmPtrDeinit( &searchObject.lm); lmPtrDeinit( & lmLookahead);
  INFO("hypTriedN %d hypRejectN %d hypReplacedN %d secs %f\n",
       searchObject.hypTriedN, searchObject.hypRejectN,searchObject.hypoReplacedN,
       timerStop(&stopit));

  return ret;
}





/* ========================================================================
    Lattice pruning

    remove all links from a given lattice for which the best score
    of any path through the lattice using this link is
       a) worse than bestPathScore -a    (absolute threshold)
       b) worse than r*bestPathScore     (relative threshold)
    ======================================================================== */

/*
-----------------------------------------------------------------------------  
  latticeBestLpath:

  for each link: compute score of best path through lattice using
                 this link

  return path score in [linkP]->pscore
  best path can be found by backtracing [linkP]->bestP  
-----------------------------------------------------------------------------  
*/  

static int latticeBestLpath( Lattice* latP, LMPtr* lmP, int mapFlag)
{

  LatLink*  linkP  = NULL;
  LatLink**  topsort = NULL;
  int       topsort_current;

  float  score, rbscore;
  float *rscoreP;
  
  LatNode*  nodeP;
  LatLink*  qheadP = NULL, *qtailP = NULL;
  int       startWordX = lmP->lmP->indexFct( lmP->cd, "<s>");


  if (! lmP) {
    ERROR("No current LM defined.\n");
    return TCL_ERROR; 
  }

  if ( latticeLM( latP, lmP, mapFlag) != TCL_OK) return TCL_ERROR; 

  /* The lookahead calculation is split in
     * calculating a topological sort of the links of the lattice
     * finding the best path backwards through the lattice
  */

  /* Find a topological traversal through the lattice for the backward pass */
  topsort = calloc(latP->linkN,sizeof(LatLink *));
  if (! topsort) {
      ERROR("Failed to allocate auxiliary data structures.\n");
      return TCL_ERROR; 
    }

  /* Initialize FanIn counts */
  for ( nodeP = latP->nodeP; nodeP; nodeP = nodeP->nextP) 
        nodeP->info.fanIn = 0;

  for ( nodeP = latP->nodeP; nodeP; nodeP = nodeP->nextP) {
    for ( linkP = nodeP->linkP; linkP; linkP = linkP->nextP) {
      /* assert(! ISA_FILLER(linkP->toP->wordX)); */

      (linkP->toP->info.fanIn)++;
    }
  }

  /* topological sorting of links, reset path-scores  */
  for ( linkP = latP->startP->linkP; linkP; linkP = linkP->nextP) {
    /* assert(! ISA_FILLER(linkP->toP->wordX)); */

    if (! qheadP) qheadP         = linkP;
    else          qtailP->qnextP = linkP;

    qtailP = linkP;
  }
  qtailP->qnextP = NULL;

  topsort_current=0;
  while ( qheadP) {
    qheadP->pscore = 0.0;

    topsort[topsort_current++]=qheadP;
    nodeP = qheadP->toP;

    if ( --(nodeP->info.fanIn) == 0) {

      /* Links out of the qheadP->toP node are updated wrt all incoming
         links at that node. All incoming links marked ---> 
         insert outgoing links of this node in the queue */

      for ( linkP = nodeP->linkP; linkP; linkP = linkP->nextP) {
        qtailP->qnextP = linkP;
        qtailP         = linkP;
      }
      qtailP->qnextP = NULL;
    }
    qheadP = qheadP->qnextP;
  }

  assert(topsort_current==latP->linkN);


  /* Calculate the optimal backward path starting at the last node */

  /* KR: elminated, what is this good for ? 

  lm_ResetContext(lmP, 0);
  */

  lm_NextFrame(lmP);                /* 'aging' in lm-caches */ 
  for(topsort_current=latP->linkN-1;topsort_current>=0;topsort_current--) {
    /* Keeping it as uniform as possible with the 1-best rescorer */

    qheadP=topsort[topsort_current];

    nodeP = qheadP->toP;  /* Update path scores for all links out of
			     the node on top of the queue */
    
    for ( linkP = nodeP->linkP; linkP; linkP = linkP->nextP) {
      /* assert(! ISA_FILLER(linkP->toP->wordX)); */

      score = linkP->lscore+linkP->pscore
	+ lm_TgScore(lmP,qheadP->fromP->lmWordX,nodeP->lmWordX,linkP->toP->lmWordX);

      if ( score < qheadP->pscore || qheadP->pscore==0) {
	qheadP->pscore = score;
      }
    }
  }
  
  /* compute the optimal forward path score */

  /* buffer the pscore in auxiliary array, reset pscore (in order to avoid additional
     float-value in "LatLink" */
  rscoreP = (float *) calloc(latP->linkN,sizeof(float));
  if (! rscoreP) {
    free(topsort);
    ERROR("Failed to allocate auxiliary data structures.\n");
    return TCL_ERROR; 
  }
  for(topsort_current=0; topsort_current < latP->linkN;topsort_current++) {
    qheadP=topsort[topsort_current];
    rscoreP[topsort_current] = qheadP->pscore;
    qheadP->pscore =WORSTSCORE;
  }
  
  /* initialize links from start-node */ 
  for ( linkP = latP->startP->linkP; linkP; linkP = linkP->nextP) {
    linkP->pscore  = linkP->lscore +
      lm_TgScore(lmP,startWordX,linkP->fromP->lmWordX,linkP->toP->lmWordX);
    linkP->bestP = NULL;    
  }

  for(topsort_current=0; topsort_current < latP->linkN;topsort_current++) {
    qheadP=topsort[topsort_current];

    /* KR: removed
    lm_ResetContext(lmP, qheadP);
    */
    lm_NextFrame(lmP);          /* 'aging' in lm-caches */ 
    nodeP = qheadP->toP;  /* Update path scores for all links out of node qheadP->toP */
    for ( linkP = nodeP->linkP; linkP; linkP = linkP->nextP) {
      /* assert(! ISA_FILLER(linkP->toP->wordX)); */

      score = qheadP->pscore + linkP->lscore +
	      lm_TgScore(lmP,qheadP->fromP->lmWordX,nodeP->lmWordX,linkP->toP->lmWordX);

      if ( score < linkP->pscore ) {
	linkP->pscore = score;
	linkP->bestP  = qheadP;
      }
    }
  } 

  /* now: score of best path using link *linkP :  rscoreP[]          (backward scores)
                                                 +linkP->pscore      (forward scores)
    						
  */
  rbscore = WORSTSCORE;
  for(topsort_current=0; topsort_current < latP->linkN;topsort_current++) {
    qheadP=topsort[topsort_current];
    qheadP->pscore += rscoreP[topsort_current];
    /* get best path score at the end of the lattice and DEFINE (!) it as the
       best score through the lattice */
    if (qheadP->toP == latP->finalP && qheadP->pscore < rbscore)
      rbscore =  qheadP->pscore;

    qheadP->pscore = - qheadP->pscore;     /* tricky */
  }

  /* pscore on the best path through the lattice (as well as partial pathes
     that are locally optimal) should be identical. But they won't be due to
     float-imprecisions. So it might be dangerous to prune a lattice with
     those scores (disconnected components, dead ends, nodes with fanIn 0, ...)
     ---> ensure that path-scores along partial connected pathes are identical 
 
     'rbscore' is the best allowed score */
  for(topsort_current= latP->linkN-1; topsort_current >=0 ;topsort_current--) {
    qheadP=topsort[topsort_current];
    if (qheadP->pscore < 0.0 ) qheadP->pscore = - qheadP->pscore;
    if (qheadP->pscore < rbscore) qheadP->pscore = rbscore;
    if (qheadP->bestP) {
      if (  qheadP->bestP->pscore < 0.0
	    || qheadP->pscore < qheadP->bestP->pscore ) 
	qheadP->bestP->pscore = qheadP->pscore;
    }
  }
  
  free(rscoreP);
  free(topsort); /* The traversal has been done */

  return TCL_OK;
}

static int cmpfun_float(const void *a, const void *b) { 
  float *aa, *ab;
  aa = (float *)a; ab = (float *)b;
  if (*aa < *ab) return -1;
  if (*aa == *ab) return 0;
  return 1;
}

/* ------------------------------------------------------------ */
/* compute absolute threshold that is required to have nl links */
/* or nn nodes (whatever will prune less) after pruning.        */
/* Note that it is - in general - impossible to prune to the    */
/* exact value as several links and/or nodes may require identi-*/
/* cal pruning thresholds to be deleted. In such cases, the     */
/* resulting lattice will be slightly larger than desired.      */
/* ------------------------------------------------------------ */
static int computeThreshN(Lattice *latP, LMPtr *lmP, int mapFlag, float optscore, int nl, int nn, float *aprune) {
  LatLink *linkP;
  LatNode *nodeP;
  float *scoreL, *scoreN, bestNodeScore;
  int linkX = 0, nodeX = 0;

  if ( 0== latP || 0 == latP->linkN || 0== latP->nodeP) { 
    INFO("computeThreshN: empty lattice"); return TCL_ERROR; 
  }
  if ( latticeLM( latP, lmP, mapFlag) != TCL_OK) return TCL_ERROR; 

  if (nl <= 0 && nn <= 0) { *aprune = -1.0; return TCL_OK; }
  /* ------------------------------------ */
  /* do we have less links than desired?  */
  /* if yes, return negative (no pruning) */
  /* ------------------------------------ */
  if ( (latP->linkN <= nl || nl <= 0) && (latP->nodeN <= nn || nn <= 0) ) { 
    *aprune = -1.0; return TCL_OK; 
  }
  /* ------------------------------------ */
  /* set nn and nl to their maximum value */
  /* (one of them can be out of range)    */
  /* ------------------------------------ */
  nl = MIN(latP->linkN, nl);
  nn = MIN(latP->nodeN, nn);

  scoreL = (float *)malloc(sizeof(float) * latP->linkN);
  if (0 == scoreL) {
    ERROR("computeThreshN: could not malloc %d bytes", (sizeof(float) * latP->linkN) );
    return TCL_ERROR;
  }
  scoreN = (float *)malloc(sizeof(float) * latP->linkN);
  if (0 == scoreN) {
    ERROR("computeThreshN: could not malloc %d bytes", (sizeof(float) * latP->linkN) );
    free(scoreL); return TCL_ERROR;
  }

  for ( nodeP = latP->nodeP; nodeP; nodeP = nodeP->nextP) {
    bestNodeScore = -1.0;
    for (linkP = nodeP->linkP; linkP; linkP = linkP->nextP) {
      if ( linkX >= latP->linkN) { /* safety check */
	ERROR("computeThreshN INTERNAL: more links than latP->linkN (%d)!", latP->linkN);
	return TCL_ERROR;
      }
      scoreL[linkX++] = linkP->pscore;
      if (bestNodeScore < 0.0 || linkP->pscore < bestNodeScore) bestNodeScore = linkP->pscore;
    }
    scoreN[nodeX++] = bestNodeScore;
  }

  qsort((void *)scoreL, linkX, sizeof(float), cmpfun_float);
  qsort((void *)scoreN, nodeX, sizeof(float), cmpfun_float);
  /* now arrays scoreL and scoreN are available */

  if (nl <= 0) nl = 0;
  while ( (nl < linkX-1) && (scoreL[nl-1] == scoreL[nl]) ) nl++;

  if (nn <= 0) nn = 0;
  while ( (nn < nodeX-1) && (scoreN[nn-1] == scoreN[nn]) ) nn++;

  *aprune = MAX(scoreL[nl], scoreN[nn]);
  
  free(scoreL); free(scoreN);
  return TCL_OK;
}


static int latticePrune(Lattice* latP, LMPtr * lmP, int mapFlag, float a, float r, int nl, int nn)
{
  LatLink*  linkP, *llinkP;
  LatNode*  nodeP;
  float optscore,rprune,aprune,prune;
  int       n;

  if ( latticeCheck( latP) != TCL_OK) {
    ERROR("Can't prune since lattice is inconsistent.\n");
    return TCL_ERROR; 
  }
  if ( latticeLM( latP, lmP, mapFlag) != TCL_OK) return TCL_ERROR; 

  INFO( "Pruning a = %f r = %f nl = %d nn = %d .\n",a, r, nl, nn);

  /* compute best path for each link */
  if ( latticeBestLpath( latP, lmP, mapFlag) != TCL_OK ) {
    return TCL_ERROR;
  }

  /* find score of best path */
  optscore = 0.0;
  for ( linkP = latP->startP->linkP; linkP; linkP = linkP->nextP) {
    if (optscore == 0.0 || linkP->pscore<optscore) {
      optscore = linkP->pscore;
    }
  }

  if (a>=0.0) aprune = a+optscore;
  else     aprune = -1.0;
  if (r>=1.0) rprune = r*optscore;
  else     rprune = -1.0;

  if (nl >= 0 || nn >= 0)  {
    float athresh;
    if (computeThreshN(latP, lmP, mapFlag, optscore, nl, nn, &athresh) != TCL_OK) return TCL_ERROR;
    INFO("Pruning to get %d links / %d nodes results in threshold %g\n", nl, nn, athresh);
    if (athresh < aprune || aprune < 0.0) aprune = athresh;
  }

  INFO("Best path through lattice %f  pruning score threshold: abs %f rel %f\n"
       ,optscore,aprune,rprune); 
  prune = (aprune > rprune)?aprune:rprune;
  n =0;
  if (prune >=0) {
    /* remove links */
    for ( nodeP = latP->nodeP; nodeP; nodeP = nodeP->nextP) {
      llinkP = NULL;
      linkP  = nodeP->linkP;
      while (linkP) {
	if (linkP->pscore > prune) {
	  /* bye bye, *linkP */
	  n ++;
	  if (llinkP) llinkP->nextP = linkP->nextP;
	  else        nodeP->linkP  = linkP->nextP;

	  latLinkFree( linkP );

	  if (llinkP) linkP = llinkP->nextP;
	  else        linkP = nodeP->linkP;
	} else {
	  llinkP = linkP;
	  linkP = linkP->nextP;
	}
      }
    }
    INFO ("%d links pruned.\n",n);
    
    /* check for consistency, remove isolated nodes, recount nodes and links */
    latticeCheck( latP);
  } else {
    INFO ("No Pruning Threshold.\n");
  }
  return TCL_OK;
}


static int latticePruneItf( ClientData cd, int argc, char *argv[])
{
  Lattice*  latP   = (Lattice*)cd;
  LMPtr     lmP;
  int       ac     =  argc - 1;
  float     a	   =  -1.0,      /* absolute threshold, default: no absolute pruning */
            r      =  -1.0;      /* relative threshold, default: no relative pruning */ 
  int       nl     =  -1,        /* desired number of resulting links, default: no pruning */
            nn     =  -1;        /* desired number of resulting nodes, default: no pruning */
  int       mapFlag= 1;

  lmP.cd = latP->lmPtr.cd;
  lmP.lmP = latP->lmPtr.lmP;

  if ( itfParseArgv( argv[0], &ac, argv+1, 0,
        "<lm>",   ARGV_CUSTOM,getLMPtr, &lmP, NULL,"language model",
        "-map",  ARGV_INT,   NULL, &mapFlag, NULL,"map on baseform (default on)",
        "-a" ,   ARGV_FLOAT, NULL, &a, NULL,"absolute pruning threshold",
        "-r" ,   ARGV_FLOAT, NULL, &r, NULL,"relative pruning threshold",
        "-nl",   ARGV_INT,   NULL, &nl,NULL,"desired number of links",
        "-nn",   ARGV_INT,   NULL, &nn,NULL,"desired number of nodes",
      NULL) != TCL_OK) return TCL_ERROR;
  
  latticePrune(latP,&lmP,mapFlag,a,r,nl,nn);

  return TCL_OK;
}


/* ------------------------------------------------------------------------
   latticeN:
   
    compute the number of crossing links for each frame of the utterance
   ------------------------------------------------------------------------ */
static int latticeN( Lattice *latP ) {
  LatLink *linkP;
  int frameX =0;
  LatNode *nodeP;
  int *nA;

  /* set frameN first... */
  for ( nodeP = latP->nodeP; nodeP; nodeP = nodeP->nextP) {
    for ( linkP = nodeP->linkP; linkP; linkP = linkP->nextP) {
      if (linkP->toP->frameX > frameX ) frameX = linkP->toP->frameX;
    }
  }
  latP->frameN = frameX;

  nA = (int*)malloc(sizeof(int) * latP->frameN);
  for (frameX=0; frameX < latP->frameN; frameX++) nA[frameX] = 0;
  for ( nodeP = latP->nodeP; nodeP; nodeP = nodeP->nextP) {
    for ( linkP = nodeP->linkP; linkP; linkP = linkP->nextP) {
      for (frameX = nodeP->frameX; frameX < linkP->toP->frameX; frameX++) nA[frameX]++;
    }
  }
  for (frameX=0; frameX < latP->frameN; frameX++) {
    itfAppendResult("%d ", nA[frameX]);
  }
  free(nA);
  return TCL_OK;
}

static int latticeNItf( ClientData cd, int argc, char *argv[])
{
  Lattice*  latP   = (Lattice*)cd;
  int       ac     =  argc - 1;

  if ( itfParseArgv( argv[0], &ac, argv+1, 0, NULL) != TCL_OK) return TCL_ERROR;

  return(latticeN(latP));
}
  

static int latticeScaleLscore(Lattice *latP, float scale) {
  LatNode *nodeP;
  LatLink *linkP;
  for ( nodeP = latP->nodeP; nodeP; nodeP = nodeP->nextP) {
    for ( linkP = nodeP->linkP; linkP; linkP = linkP->nextP) {
      linkP->lscore *= scale;
    }
  }
  return 0;
}

static int latticeScaleLscoreItf( ClientData cd, int argc, char *argv[])
{
  Lattice*  latP   = (Lattice*)cd;
  int       ac     =  argc - 1;
  float     scale  = 1.0;

  if ( itfParseArgv( argv[0], &ac, argv+1, 0, 
		    "<scale>",ARGV_FLOAT, NULL, &scale, NULL, "scaling factor for all lscores",
		    NULL) != TCL_OK) return TCL_ERROR;

  latticeScaleLscore(latP, scale);
  return TCL_OK;
}
/*
-----------------------------------------------------------------------------  
  latticeGamma:

  for each link: compute its forward-backward-probability

  return forward probability (should equal backward probability ;-) )
   in total_prob
-----------------------------------------------------------------------------  
*/  

static int latticeGamma( Lattice* latP, LMPtr* lmP, int mapFlag, double zn, double pn, 
		     double *total_prob, double scale) {

  LatLink*  linkP  = NULL;
  LatLink**  topsort = NULL;
  int       topsort_current;
  double    *alphaA, *betaA, *gammaA;
  int       startWordX = lmP->lmP->indexFct( lmP->cd, "<s>");

  double score;

  LatNode*  nodeP;
  LatLink*  qheadP = NULL, *qtailP = NULL;

  *total_prob = 0.0;

  if (! lmP) {
    ERROR("No current LM defined.\n");
    return TCL_ERROR; 
  }
  if ( latticeCheck( latP) != TCL_OK) {
    ERROR("Can't gamma since lattice is inconsistent.\n");
    return TCL_ERROR; 
  }
  if ( latticeLM( latP, lmP, mapFlag) != TCL_OK) return TCL_ERROR; 


  /* ---------------------------------------- */
  /* compute the fan-In for each of the nodes */
  /* ---------------------------------------- */
  for ( nodeP = latP->nodeP; nodeP; nodeP = nodeP->nextP) /* for all nodes do */
    nodeP->info.fanIn = 0;

  for ( nodeP = latP->nodeP; nodeP; nodeP = nodeP->nextP) {
    for ( linkP = nodeP->linkP; linkP; linkP = linkP->nextP) {
      (linkP->toP->info.fanIn)++;
    }
  }

  /* ------------------------------------------------------------------ */
  /* to compute the betas, we must traverse the lattice in the backward */
  /* direction. To make this feasible, set up a help array topsort that */
  /* contains all links in the topological order. Topological order     */
  /* means, that the target node of all links with a higher index is    */
  /* never the source node of a link with a lower index:  all           */
  /* outgoing links of a node have higher indices than all incoming     */
  /* links.                                                             */
  /* This array is then evaluated inversely for the beta computation.   */
  /* ------------------------------------------------------------------ */

  topsort = calloc(latP->linkN,sizeof(LatLink *));
  if (! topsort) {
    ERROR("Failed to allocate auxiliary data structures.\n");
    fprintf(stderr,"Trying to allocate %d items of size %d each\n", latP->linkN, (int) sizeof(LatLink *));
    return TCL_ERROR; 
  }

  /* -------------------------------------------------------------------- */
  /* set up the topological forward array by setting up the qnextP fields */
  /* of all arcs to point to their topological successor. Do in two steps */
  /* First is, set the qnextP fields in all links starting in the root    */
  /* node, set last qnextP to zero:                                       */
  /* -------------------------------------------------------------------- */
  qheadP = latP->startP->linkP; qtailP = qheadP;
  for ( linkP = latP->startP->linkP; linkP; linkP = linkP->nextP) {
    qtailP->qnextP = linkP;
    qtailP = linkP;
  }
  qtailP->qnextP = NULL;

  /* ----------------------------------------------------------------- */
  /* Second step: follow the existing qnextP's to the target nodes and */
  /* add the links that are going there. After all links into a given  */
  /* target node are added, add the successor links of this target     */
  /* nodes to the topologically sorted list.                           */
  /* The alphaA, betaA and gammaA arrays are all sorted according to   */
  /* this topologically sorted list (topsort[]). Therefore, betaA[7]   */
  /* is the beta value of the link which has also been copied to       */
  /* topsort[7]. To find from the actual lattice links into the        */
  /* topsort[] array, the pscore field is misused and serves as an     */
  /* index into topsort. Therefore, topsort[mlink->pscore] equals mlink*/
  /* ----------------------------------------------------------------- */
  topsort_current=0;
  while ( qheadP) {
    qheadP->pscore = (float)topsort_current; /* set up the index into topsort */
    topsort[topsort_current++]=qheadP;       /* add this link to the list */
    nodeP = qheadP->toP;                     /* look where it points to?  */

    /* ----------------------------------------------------------- */
    /* did we add all links that point to 'nodeP' to the list yet? */
    /* ----------------------------------------------------------- */
    if ( --(nodeP->info.fanIn) == 0) {
      /* --------------------------------------------------------- */
      /* yes, we did! Now we are free to add the outgoing links to */
      /* the list. Do so.                                          */
      /* --------------------------------------------------------- */
      for ( linkP = nodeP->linkP; linkP; linkP = linkP->nextP) {
        qtailP->qnextP = linkP;
        qtailP         = linkP;
      }
      qtailP->qnextP = NULL;
    }

    /* ------------------------------------------ */
    /* finished with this link, go on to the next */
    /* ------------------------------------------ */
    qheadP = qheadP->qnextP;
  }

  assert(topsort_current==latP->linkN);  /* did we get all links into the list? */

  /* ----------------------------------------------------------- */
  /* alloc auxiliary arrays: alphaA, betaA, gammaA               */
  /* ----------------------------------------------------------- */
  betaA = (double *) malloc(latP->linkN * sizeof(double));
  if (! betaA) {
    free(topsort);
    ERROR("Failed to allocate auxiliary data structure betaA.\n");
    return TCL_ERROR; 
  }
  alphaA = (double *) calloc(latP->linkN,sizeof(double));
  if (! alphaA) {
    free(topsort); free(betaA);
    ERROR("Failed to allocate auxiliary data structure alphaA.\n");
    return TCL_ERROR; 
  }
  gammaA = (double *) calloc(latP->linkN,sizeof(double));
  if (! gammaA) {
    free(topsort); free(alphaA); free(betaA);
    ERROR("Failed to allocate auxiliary data structure gammaA.\n");
    return TCL_ERROR; 
  }

  /* ================ */
  /* Beta computation */
  /* ================ */

  /* ---------------- */
  /* clear beta array */
  /* ---------------- */
  for (topsort_current=0; topsort_current < latP->linkN; topsort_current++) betaA[topsort_current] = 0.0l;

  /* ----------------------------------------------------- */
  /* the betas are the accumulated scores up to the        */
  /* link without the lscore of the link itself. This is   */
  /* to simplify things at the lattice's end.              */
  /* ----------------------------------------------------- */
  lm_CacheReset(lmP);
  lm_NextFrame(lmP);                /* 'aging' in lm-caches */ 
  for(topsort_current=latP->linkN-1;topsort_current>=0;topsort_current--) {

    /* ---------------------------------------------------- */
    /* get current link: this link's beta is to be computed */
    /* ---------------------------------------------------- */
    qheadP=topsort[topsort_current];

    /* -------------------------------------------------- */
    /* loop over all outgoing links of the target node of */
    /* the current link. All these links are possible     */
    /* 'sources' of the current link (we are going back-  */
    /* wards in time), so all their probs have to be added*/
    /* -------------------------------------------------- */
    nodeP = qheadP->toP; 
    for ( linkP = nodeP->linkP; linkP; linkP = linkP->nextP) {
      score = betaA[(int)linkP->pscore] + scale * ( linkP->lscore + (double)(
	      lm_TgScore(lmP,qheadP->fromP->lmWordX,nodeP->lmWordX,linkP->toP->lmWordX)));
      if (betaA[(int)qheadP->pscore] == 0.0) { 
	betaA[(int)qheadP->pscore] = score; 
      } else { 
	betaA[(int)qheadP->pscore] = addLogProbs(score, betaA[(int)qheadP->pscore] );
      }
    }
  }

  /* ================= */
  /* Alpha computation */
  /* ================= */

  /* ---------------------------------------------------------- */
  /* we compute the alphas of all links starting at the to-node */
  /* of the current link. Therefore, the links starting at the  */
  /* root node must be set separately, 'cause the root node is  */
  /* by definition the target node of no node and no links be-  */
  /* ginning at the root node would be computed.                */
  /* ---------------------------------------------------------- */

  for ( linkP = latP->startP->linkP; linkP; linkP = linkP->nextP) {
    alphaA[(int)linkP->pscore]  = linkP->lscore + (double)(lm_TgScore(lmP,startWordX,linkP->fromP->lmWordX,linkP->toP->lmWordX));
    alphaA[(int)linkP->pscore] *= scale;
    linkP->bestP = NULL;    
  }

  /* ---------------------------------------------------------- */
  /* for all links: compute the alphas of all links starting at */
  /* the to-node of the current link.                           */
  /* ---------------------------------------------------------- */
  for(topsort_current=0; topsort_current < latP->linkN; topsort_current++) {
    /* get current link */
    qheadP=topsort[topsort_current];

    lm_NextFrame(lmP);          /* 'aging' in lm-caches */ 
    nodeP = qheadP->toP;  /* Update path scores for all links out of node qheadP->toP */
    for ( linkP = nodeP->linkP; linkP; linkP = linkP->nextP) {

      score = alphaA[(int)qheadP->pscore] + scale * (linkP->lscore +
	(double)(lm_TgScore(lmP,qheadP->fromP->lmWordX,nodeP->lmWordX,linkP->toP->lmWordX)));
      if (alphaA[(int)linkP->pscore] == 0.0) { 
	alphaA[(int)linkP->pscore] = score; 
      } else {
	alphaA[(int)linkP->pscore] = addLogProbs(score, alphaA[(int)linkP->pscore] );
      }
      
    }
  } 

  /* -------------------------------------------- */
  /* now all alphas for the links are in alphaA[] */
  /* The betas are in the betaA array             */
  /* Now: find forward score of the whole thing   */
  /* by adding the alphas of all links ending in  */
  /* the lattice's final node latP->finalP        */
  /* -------------------------------------------- */
  score = 0.0;
  for ( nodeP = latP->nodeP; nodeP; nodeP = nodeP->nextP) {
    for ( linkP = nodeP->linkP; linkP; linkP = linkP->nextP) {
      if ( linkP->toP == latP->finalP) {
        if (score == 0.0) { 
	  score = alphaA[(int)linkP->pscore];
	} else {
	  score = addLogProbs(score, alphaA[(int)linkP->pscore] );
	}
      }
    }
  }
  *total_prob = score;

  /* wanna have safety? Then check whether the forward prob equals the backward prob . no (Hagen) */
#if 1
  score = 0.0l;
  for (topsort_current=0; topsort_current < latP->linkN; topsort_current++) {
    linkP = topsort[topsort_current];
    if (linkP->fromP == latP->startP) {
      double help = betaA[topsort_current];
      help +=  scale * (linkP->lscore + (double)(lm_TgScore(lmP,startWordX,linkP->fromP->lmWordX,linkP->toP->lmWordX)));
	
      if (score == 0.0) score = help;  else score = addLogProbs(score, help);
      
    }
  }

    
  if (fabs(score - *total_prob) > fabs(score / 10000.0) ) {
    fprintf(stderr, "INTERNAL ERROR: alphas (%g) do not equal betas (%g)\n", 
	    *total_prob, score);
  }
  *total_prob = score;
#endif



#ifdef DEBUG
  for (topsort_current=0; topsort_current < latP->linkN; topsort_current++) {
    linkP = topsort[topsort_current];
    fprintf(stderr,"lscore %g / alpha %lg / beta %lg \n", linkP->lscore, alphaA[topsort_current], betaA[topsort_current]);
  }
  fprintf(stderr,"Alphas: %g, Betas: %g\n", *total_prob, score);
#endif

  /* --------------------------------------------------- */
  /* we have the overall score, the alphas and the betas */
  /* compute the gammas and store them in gammaA[.]      */
  /* --------------------------------------------------- */
  for (topsort_current=0; topsort_current < latP->linkN; topsort_current++) {
    gammaA[topsort_current] = betaA[topsort_current] - (*total_prob) + alphaA[topsort_current];
  }

  /* ------------------------------------------------------------------ */
  /* fill the score field of the node with the node fwd-bwd probability */
  /* this is the fwd-bwd probability of this node, and hence of this    */
  /* word. It is computed by adding the gamma values of all links that  */
  /* leave the node.                                                    */
  /* ------------------------------------------------------------------ */
  
  for ( nodeP = latP->nodeP; nodeP; nodeP = nodeP->nextP) nodeP->score = 0.0; 
  for (topsort_current=0; topsort_current < latP->linkN; topsort_current++) {
    linkP = topsort[topsort_current];
    linkP->pscore = (float)gammaA[topsort_current];
    nodeP = linkP->fromP;
    if ( nodeP->score == 0.0 ) { 
      nodeP->score = gammaA[topsort_current];
    } else {
      nodeP->score = (float)addLogProbs((double)nodeP->score, (double)gammaA[topsort_current]);
    }
  }

  free(alphaA); free(betaA); free(gammaA); free(topsort);
  return TCL_OK;
}

/* -------------------------------------------- */
/* add two negative logarithms of probabilities */
/* -------------------------------------------- */
static double addLogProbs(double p1, double p2) {
  double sum, mx = 0.0;
  mx = ( (p1 > p2) ? p2 : p1 );
  sum = -log( exp(mx-p1) + exp(mx-p2) );
  return(sum + mx);
}

  
/* -------------------------------------------- */
/* align the 'hypo' string against the lattice  */
/* return a list of the words in the hypostring */
/* along with the corresponding gammas          */
/* If mode is 0, just the node scores are re-   */
/* turned. If mode is 1, all link scores that   */
/* begin within +-b_int frames AND end within   */
/* +-e_int frames (negative: don't care) and    */
/* have identical word entities are added       */
/* If mode is 2, all links that exist in the    */
/* center frame of the hypostring word will be  */
/* added together; this is the recommended mode */
/* -------------------------------------------- */
static int latticeRgamma(Lattice *latP, char *hypoOrig, int b_int, int e_int, int mode) {
  /* go thru the hypo list and create a vocabX list */
  int hypoN=0, hypoX=0, maxWordN;
  int latN = 0, latX;
  char *p;
  LatLink*  linkP;
  LatLink** latA;
  VWordX * hypoA;
  char *hypo;

  hypo = (char *)malloc(strlen(hypoOrig)+1); if (!hypo) return(TCL_ERROR); 
  strcpy(hypo,hypoOrig);

  maxWordN = (strlen(hypo) + 4) / 2 ; /* one-letter-word, one letter space */
  latA = (LatLink**)malloc(sizeof(LatLink*) * maxWordN);
  if (!latA) { free(hypo); return(TCL_ERROR); }
  hypoA = (VWordX *)malloc(sizeof(VWordX) * maxWordN);
  if (!hypoA) { free(hypo); free(latA); return(TCL_ERROR); }

  p = strtok(hypo," \t\n");
  do {
    hypoA[hypoN++] = vocabIndex(latP->vocabP, p);
  }  while ( (p = strtok(0, " \t\n")) );

  /* if in mode >0, the final word will not be scored, hence     */
  /* we need the final word explicitely in this case. Enforce it */
  if (mode > 0) {
    if (hypoA[hypoN-1] != latP->vocabP->finishWordX) {
      hypoA[hypoN++] = latP->vocabP->finishWordX;
    }
  }

  /* have it, now go thru lattice */
  /* set up the beginning */
  for (linkP = latP->startP->linkP; linkP != 0; linkP = linkP->nextP) {
    if (linkP->toP->wordX == hypoA[0]) break;
  }
  if (linkP == 0) {
    fprintf(stderr, "Sorry, first word in hypo %s not in lattice\n", hypo);
    free(hypo); free(latA); free(hypoA); return(TCL_ERROR);
  }
  latA[latN++] = linkP; hypoX = 1;
  
  
  /* go on, search for our path */

  linkP = linkP->toP->linkP; 
  while (hypoX < hypoN) {

    if (linkP == 0) {
      /* didn't find the word originating from current node. 
	 Do backtracking. Try the node before this one. */
      latN--; hypoX--;
      if (latN < 0) {
	fprintf(stderr, "Sorry, hypo not in lattice (%s)\n", hypoOrig);
	free(hypo); free(latA); free(hypoA); return(TCL_ERROR);
      }
      linkP = (latA[latN])->nextP;
      continue;
    }

    if (linkP->toP->wordX == hypoA[hypoX]) {
      latA[latN++] = linkP; hypoX++;
      linkP = linkP->toP->linkP;
    } else {
      linkP = linkP->nextP;
    }
  }
  
  /* mode >0, add the pscores of all corresponding links */
  if (mode > 0) {
    LatNode *nodeP;
    for ( nodeP = latP->nodeP; nodeP; nodeP = nodeP->nextP) { nodeP->score = 0.0 ; }
    for (latX=0; latX < latN-1; latX++) { 
      int vocabX, sframeX, eframeX, cframeX;

      linkP = latA[latX];
      sframeX = linkP->toP->frameX;
      eframeX = (latA[latX+1])->toP->frameX;
      cframeX = sframeX + (eframeX - sframeX) / 2; /* center frame */
      vocabX  = linkP->toP->wordX;
      vocabX = index2realPronounciationIndex(latP->vocabP, vocabX);
      
      /* for all links do */
      for ( nodeP = latP->nodeP; nodeP; nodeP = nodeP->nextP) {
	LatLink *llinkP;
	for (llinkP = nodeP->linkP; llinkP; llinkP = llinkP->nextP) {
	  int sframe,eframe;
	  if (0 == llinkP->fromP) continue; /* start node ? */
	  if (0 == llinkP->toP)   continue; /* end node? */
	  sframe = llinkP->fromP->frameX;
	  eframe = llinkP->toP->frameX;
	    
	  if (
	      ( (mode==1) && ( b_int < 0 || abs(sframe-sframeX) <= b_int ) && ( e_int < 0 || abs(eframe-eframeX) <= e_int) ) 
	      ||
	      ( (mode==2) && ( (sframe <= cframeX) && (eframe > cframeX) ) )
	     ) {
	    int voc;
	    voc = llinkP->fromP->wordX; 
	    voc = index2realPronounciationIndex(latP->vocabP, voc);
	    if (voc == vocabX) {
	      if ( linkP->toP->score == 0.0 ) { 
		linkP->toP->score = llinkP->pscore;
	      } else {
		linkP->toP->score = (float)addLogProbs((double)linkP->toP->score, (double)llinkP->pscore);
	      }
	    }
	  }
	} /* for llinkP */
      } /* for all nodes */
    } /* for all the links of the asked-for hypo */
  } /* if mode == 1 */

  /* have it, read it out and print to TCL */
  for (latX=0; latX < latN; latX++) { 
    linkP = latA[latX];
    itfAppendResult(" { {%s} %lg %d } ", vocabName(latP->vocabP,linkP->toP->wordX), 
		    linkP->toP->score, linkP->toP->frameX);
  } 


  free(hypo); free(latA); free(hypoA);
  return(TCL_OK);
}




static int latticeRgammaItf( ClientData cd, int argc, char *argv[])
{

  Lattice*  latP   = (Lattice*)cd;
  int       ac     =  argc - 1;
  char *    hypo   = NULL;
  int       b_int  = 3;
  int       e_int  = 3;
  int       mode   = 2;
  if ( itfParseArgv( argv[0], &ac, argv+1, 0, 
		    "<hypo>",       ARGV_STRING, NULL, &hypo,     NULL,   "hypo to align",
		    "-bint",        ARGV_INT,    NULL, &b_int,    NULL,   "allowed begin delta",
		    "-eint",        ARGV_INT,    NULL, &e_int,    NULL,   "allowed end delta",
		    "-mode",        ARGV_INT,    NULL, &mode,     NULL,   "0-classic; 1-add all links",
		    NULL) != TCL_OK) return TCL_ERROR;

  latticeRgamma(latP, hypo, b_int, e_int, mode);
  
  return TCL_OK;
}

static int latticeGammaItf( ClientData cd, int argc, char *argv[])
{

  Lattice*  latP   = (Lattice*)cd;
  LMPtr     lmP;
  int       ac     =  argc - 1;
  int       mapFlag = 1;
  double    prob;
  double    scale = 1.0;

  if ( itfParseArgv( argv[0], &ac, argv+1, 0, 
		     "<lm>",          ARGV_CUSTOM, getLMPtr, &lmP, NULL,"language model",
		     "-map",          ARGV_INT,    NULL, &mapFlag,     NULL, "map on baseform (default on)",
 		     "-scale",ARGV_DOUBLE, NULL, &scale, NULL,"prob scaling factor",

		    NULL) != TCL_OK) return TCL_ERROR;

  latticeGamma(latP, &lmP, mapFlag, 1, 0, &prob, scale);
  itfAppendResult("%g", prob);

  return TCL_OK;
}


/* ========================================================================
    Lattice Initialization
   ======================================================================== */

static Method latNodeMethod[] = 
{ 
  {  NULL,  NULL,           NULL } 
};

static TypeInfo latNodeInfo = { "LatNode", 0, 0, latNodeMethod, 
                                 NULL, NULL,
                                 NULL, NULL, NULL,
	                        "Lattice Node" } ;
/*
static Method latLinkMethod[] = 
{ 
  {  NULL,  NULL,           NULL } 
};

static TypeInfo latLinkInfo = { "LatLink", 0, 0, latLinkMethod, 
                                 NULL, NULL,
                                 NULL, NULL, NULL,
	                        "Lattice Link" } ;
*/

static Method latticeMethod[] = 
{ 
  { "puts",      latticePutsItf,       NULL },
  { "clear",     latticeClearItf,     "remove all items from lattice" },
  { "add",       latticeAddItf,       "add lattice nodes/links"   },
  { "hmm",       latticeHMMItf,       "Lattice to HMM conversion" },
  { "rescore",   latticeRescoreItf,   "Lattice trigram rescoring" },
  { "astar",     latticeAStarItf, "create N-best list"        },
  { "write",     latticeWriteItf,     "write lattice to file"     },
  { "prune",     latticePruneItf,     "prune lattice "            },
  { "gamma",     latticeGammaItf,     "compute link probabilities"},
  { "rgamma",    latticeRgammaItf,    "gamma rescoring"           },
  { "N",         latticeNItf,         "# of links per frame"      },
  { "scale",     latticeScaleLscoreItf, "scale lscores"         },
  {  NULL,  NULL,           NULL } 
};

static TypeInfo latticeInfo = { "Lattice", 0, -1,     latticeMethod, 
                                 latticeCreateItf,    latticeFreeItf,
                                 latticeConfigureItf, latticeAccessItf, 
                                 NULL,
	                        "Lattice" } ;

static int latInitialized = 0;

int Lattice_Init()
{
  if (! latInitialized) {
    latticeDefault.beamWidth = 100.0;
    latticeDefault.noiseBeamWidth = 100.0;
    latticeDefault.topN      =     0;
    latticeDefault.format    = "PROP";
    
    itfNewType( &latNodeInfo);
    itfNewType( &latticeInfo);
    latInitialized = 1;
  }
  return TCL_OK;
}



