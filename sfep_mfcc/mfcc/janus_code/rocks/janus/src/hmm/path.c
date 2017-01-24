/* ========================================================================
    JANUS      Speech- to Speech Translation
               ------------------------------------------------------------
               Object: Path Maintainance Functions
               ------------------------------------------------------------

    Author  :  Michael Finke & Ivica Rogina
    Module  :  path.c
    Date    :  $Id: path.c 3428 2011-04-15 18:00:28Z metze $
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
    Revision 3.19  2007/02/22 16:27:59  fuegen
    removed -pedantic compiler warnings with

    Revision 3.18  2007/01/09 10:57:28  fuegen
    removed MPE code

    Revision 3.17  2006/11/10 10:15:39  fuegen
    merged changes from branch jtk-05-02-02-shajith
    - all modification related to MMIE training
    - first unverified functions for MPE training
    - modifications made by Shajith, Roger, Wilson, and Sebastian

    Revision 3.16  2005/12/20 15:13:13  metze
    Added pjkdmc

    Revision 3.15  2005/12/14 14:40:23  fuegen
    splitted from*to in two parts because of possible overflows
    with too large indices

    Revision 3.14  2005/04/18 15:28:02  metze
    Improved output for pathWordLabels (Sebastian)

    Revision 3.13.12.1  2006/11/03 12:23:24  stueker
    Initial check in of the MMIE version from Shajith, Wilson, and Roger. Contains new pathCopy function.

    Revision 3.13  2003/08/14 11:19:57  fuegen
    Merged changes on branch jtk-01-01-15-fms (jaguar -> ibis-013)

    Revision 3.12.4.8  2003/07/30 11:14:47  metze
    Fixes for pathWordLabels/ pathPhoneLabels ...

    Revision 3.12.4.7  2003/03/19 14:22:20  metze
    Improved info messages

    Revision 3.12.4.6  2003/02/05 09:03:10  soltau
    Added interface for pathAlignGLatItf

    Revision 3.12.4.5  2002/12/20 14:52:37  metze
    Error messages

    Revision 3.12.4.4  2002/11/04 14:19:59  metze
    Can use Gamma for lscore

    Revision 3.12.4.3  2002/08/27 08:46:26  metze
    itfParseArgv now uses <name> and NULL initialization for char*

    Revision 3.12.4.2  2001/07/06 13:11:59  soltau
    Changed compress and scoreFct handling

    Revision 3.12.4.1  2001/07/03 09:39:09  soltau
    Changes according to the new acoustic score function interface

 * Revision 3.8  2000/08/08  06:57:15  soltau
 * Fixed bug in pathBload (pathItemListAdd) :
 * removed non-deterministic behavior in function-function-calls
 * 
    Revision 3.7  2000/02/08 09:00:36  soltau
    Added labelboxes

    Revision 3.6  1998/01/21 11:31:01  kemp
    Added security if for path map. Also, if illegal senone indices occur, path labels -what senon
    no longer dumps core.

 * Revision 3.5  1997/11/06  09:11:46  kemp
 * Added security check for path labels method: if no path is present,
 * TCL_OK is returned instead of segmentation fault
 *
    Revision 3.4  1997/07/24 14:38:18  tschaaf
    labels.h removed

    Revision 3.3  1997/07/18 16:49:26  tschaaf
    gcc / DFKI -clean

    Revision 3.2  1997/06/17 17:41:43  westphal
    *** empty log message ***

    Revision 2.10  1996/09/23  15:27:50  finkem
    added words/variants and phone labels to the path object

    Revision 2.9  1996/06/19  12:36:23  maier
    -from and -to for method lscore, removed bug in return value

    Revision 2.8  1996/05/28  13:14:41  rogina
    fixed bug in output of ASCII labels

    Revision 2.7  1996/05/06  18:05:26  rogina
    fixed last state index of last word for the "labels" method

    Revision 2.6  1996/04/23  19:54:11  maier
    RUNON, ready

    Revision 2.5  1996/04/22  13:33:38  rogina
    added flag -codebookX to Path's map method for mapping senone
    indices down to codebook indices

    Revision 2.4  1996/03/22  12:11:47  rogina
    added feof check to path read to detect corrupt files rather than
    result in some strange segmentation fault somewhere

    Revision 2.3  1996/03/20  23:15:06  finkem
    lscore returns the accumlative path score

    Revision 2.2  1996/03/13  21:56:18  finkem
    added scor to path.itemList as best path score in viterbi

    Revision 2.1  1996/01/31  04:51:21  finkem
    replace fclose by fileClose in BLoad

    Revision 2.0  1996/01/31  04:40:04  finkem
    New senoneSet setup & bsave/bload with fileOpen/Close

    Revision 1.25  1996/01/29  11:03:27  finkem
    remove reference to senone set strA (future release of Janus3)

    Revision 1.24  1996/01/28  21:16:32  finkem
    added bsave and bload of path object as well as map

    Revision 1.23  1996/01/22  18:33:38  maier
    removed bug in pathAccessItf, more configures

    Revision 1.22  1996/01/11  12:57:44  rogina
    added method labels which does what the puts of the old label module did

    Revision 1.21  1995/12/06  02:27:31  finkem
    add senoneMatrix output

    Revision 1.20  1995/11/29  13:55:43  rogina
    reset firstFrame and lastFrame in pathReset,
    added Tcl method 'reset' for manual resetting

    Revision 1.19  1995/11/29  13:32:03  rogina
    added 'best'-field to pathList object where it was missing

    Revision 1.18  1995/11/18  18:14:59  finkem
    stateMatrix phoneMatrix wordMatrix added

    Revision 1.17  1995/11/17  22:25:42  finkem
    FMatrix stuff ... yet unfinished.

    Revision 1.16  1995/11/14  06:05:14  finkem
    changed path structure to make it itf accessible

    Revision 1.15  1995/10/31  10:14:30  rogina
    *** empty log message ***

    Revision 1.14  1995/10/18  08:52:03  rogina
    prepaaration for guided alignment

    Revision 1.13  1995/10/16  16:19:28  finkem
    made viterbi Tcl function return the path score

    Revision 1.12  1995/10/16  15:53:37  finkem
    made it work with new hmm.c

    Revision 1.11  1995/10/06  18:32:53  rogina
    *** empty log message ***

    Revision 1.10  1995/10/06  01:09:22  torsten
    *** empty log message ***

    Revision 1.9  1995/10/04  23:42:02  torsten
    *** empty log message ***

    Revision 1.8  1995/09/06  08:02:10  kemp
    *** empty log message ***

    Revision 1.7  1995/09/06  07:09:57  kemp
    *** empty log message ***

    Revision 1.6  1995/08/28  16:10:59  rogina
    *** empty log message ***

    Revision 1.5  1995/08/17  17:54:49  rogina
    *** empty log message ***

    Revision 1.4  1995/08/10  08:18:36  finkem
    *** empty log message ***

    Revision 1.3  1995/08/10  08:12:10  rogina
    *** empty log message ***

    Revision 1.2  1995/08/04  14:17:20  rogina
    *** empty log message ***

    Revision 1.1  1995/07/25  13:49:57  finkem
    Initial revision

 
   ======================================================================== */

char pathRCSVersion[]= "@(#)1$Id: path.c 3428 2011-04-15 18:00:28Z metze $";

#include "common.h"
#include "itf.h"
#include "error.h"
#include "hmm.h"
#include "path.h"
#include "mach_ind_io.h"
#include "distribStream.h"
#include "scoreA.h"
#include <ctype.h>

/* ========================================================================
    Forward Declarations
   ======================================================================== */

extern int   pathMake(   Path* path, SenoneSet* snsP, int from,  int to, 
                                     int skipl, int skipt);

extern float pathLScore( Path* path, HMM* hmm, int from, int to, int gamma);
extern int   pathViterbi(Path* path, HMM* hmm, int from,   int to, 
                                               int skipl,  int skipt, 
                                     float beam, int topN, int label,
			             int bpCleanUpMod, int bpCleanUpMul);
extern int   pathFwdBwd( Path* path, HMM* hmm, int from,   int to, 
                                               int skipl,  int skipt, 
                                     int topN, int width,  int label);

extern int viterbi(          Path* path, HMM* hmm,                      int topN, int bpCleanUpMod, int bpCleanUpMul);
extern int viterbiMMIE(      Path* path, HMM* hmm, PhoneGraph *pgraphP, int topN, int bpCleanUpMod, int bpCleanUpMul);
extern int viterbiMMIE_OMP(      Path* path, HMM* hmm, StateGraph* stateGraph, PhoneGraph *pgraphP, int topN, int bpCleanUpMod, int bpCleanUpMul);
extern int forwardBackward(  Path* path, HMM* hmm, int topN, int width);

/* ========================================================================
    PathItem
   ======================================================================== */
/* ------------------------------------------------------------------------
    pathItemInit  initialize a PathItem object
   ------------------------------------------------------------------------ */

int pathItemInit( PathItem* pi)
{
  assert(pi);
  pi->stateX   = -1;
  pi->senoneX  = -1;
  pi->phoneX   = -1;
  pi->wordX    = -1;
  pi->alpha    =  0.0;
  pi->beta     =  0.0;
  pi->gamma    =  0.0;
  pi->lscore   =  0.0;
  return TCL_OK;
}

/* ------------------------------------------------------------------------
    pathItemDeinit  deinitialize a PathItem object
   ------------------------------------------------------------------------ */

int pathItemDeinit( PathItem* pi)
{
  assert( pi);
  return TCL_OK;
}

/* ------------------------------------------------------------------------
    pathItemConfigureItf
   ------------------------------------------------------------------------ */

static int pathItemConfigureItf(ClientData cd, char *var, char *val)
{
  PathItem *pi = (PathItem*)cd;
  if (! pi) return TCL_ERROR;

  if (var==NULL) {

    ITFCFG(pathItemConfigureItf,cd,"stateX");
    ITFCFG(pathItemConfigureItf,cd,"senoneX");
    ITFCFG(pathItemConfigureItf,cd,"phoneX");
    ITFCFG(pathItemConfigureItf,cd,"wordX");
    ITFCFG(pathItemConfigureItf,cd,"alpha");
    ITFCFG(pathItemConfigureItf,cd,"beta");
    ITFCFG(pathItemConfigureItf,cd,"gamma");
    ITFCFG(pathItemConfigureItf,cd,"lscore");
    return TCL_OK;
  }
  ITFCFG_Int(    var,val,"stateX",  pi->stateX,  0);
  ITFCFG_Int(    var,val,"senoneX", pi->senoneX, 0);
  ITFCFG_Int(    var,val,"phoneX",  pi->phoneX,  0);
  ITFCFG_Int(    var,val,"wordX",   pi->wordX,   0);
  ITFCFG_Float(  var,val,"alpha",   pi->alpha,   0);
  ITFCFG_Float(  var,val,"beta",    pi->beta,    0);
  ITFCFG_Float(  var,val,"gamma",   pi->gamma,   0);
  ITFCFG_Float(  var,val,"lscore",  pi->lscore,  0);

  ERROR("unknown option '-%s %s'.\n", var, val ? val : "?");
  return TCL_ERROR;   
}

/* ------------------------------------------------------------------------
    pathItemPuts
   ------------------------------------------------------------------------ */

static int pathItemPutsItf (ClientData clientData, int argc, char *argv[]) 
{
  PathItem* pi = (PathItem*)clientData;

  argc--;
  if (itfParseArgv(argv[0], &argc, argv+1, 0,
        NULL) != TCL_OK)  return TCL_ERROR;

  itfAppendResult("%d %d %d %d", pi->stateX, pi->senoneX, pi->phoneX,
                                 pi->wordX);
  return TCL_OK;
}

/* ========================================================================
    PathItemList
   ======================================================================== */
/* ------------------------------------------------------------------------
    pathItemListInit  initialize a PathItemList object
   ------------------------------------------------------------------------ */

int pathItemListInit( PathItemList* pi)
{
  assert(pi);
  pi->itemN    = 0;
  pi->itemA    = NULL;
  pi->logScale = 0.0;
  pi->beam     = 0.0;
  pi->score    = 0.0;
  pi->best     = 0.0;
  return TCL_OK;
}

PathItemList *pathItemListCreate( int n)
{
  PathItemList *pil = (PathItemList*)malloc(sizeof(PathItemList));

  if ((!pil) || (pathItemListInit(pil) != TCL_OK)) {
    if (pil) free (pil);
    ERROR("Cannot create pathItemList object.\n");
    return NULL;
  }
  pathItemListAdd( pil, n, -1, -1, -1, -1);
  return pil;
}

/* ------------------------------------------------------------------------
    pathItemListDeinit  deinitialize a PathItemList object
   ------------------------------------------------------------------------ */

int pathItemListDeinit( PathItemList* pi)
{
  assert( pi);

  if ( pi->itemA) {
    int i;
    for ( i = 0; i < pi->itemN; i++) pathItemDeinit( pi->itemA + i);
    free( pi->itemA);
    pi->itemA = NULL; pi->itemN = 0;
  }
  return TCL_OK;
}

int pathItemListFree( PathItemList* pil)
{
  if (pathItemListDeinit(pil) != TCL_OK) return TCL_ERROR;
  free(pil);
  return TCL_OK;
}

/* ------------------------------------------------------------------------
    pathItemListConfigureItf
   ------------------------------------------------------------------------ */

static int pathItemListConfigureItf(ClientData cd, char *var, char *val)
{
  PathItemList *pi = (PathItemList*)cd;
  if (! pi) return TCL_ERROR;

  if (var==NULL) {

    ITFCFG(pathItemListConfigureItf,cd,"itemN");
    ITFCFG(pathItemListConfigureItf,cd,"logScale");
    ITFCFG(pathItemListConfigureItf,cd,"beam");
    ITFCFG(pathItemListConfigureItf,cd,"score");
    ITFCFG(pathItemListConfigureItf,cd,"best");
    return TCL_OK;
  }
  ITFCFG_Float( var,val,"logScale", pi->logScale, 0);
  ITFCFG_Float( var,val,"beam",     pi->beam,     0);
  ITFCFG_Float( var,val,"score",    pi->score,    0);
  ITFCFG_Float( var,val,"best",     pi->best,     0);
  ITFCFG_Int(   var,val,"itemN",    pi->itemN,    1);

  ERROR("unknown option '-%s %s'.\n", var, val ? val : "?");
  return TCL_ERROR;   
}

/* ------------------------------------------------------------------------
    pathItemListAccessItf
   ------------------------------------------------------------------------ */

static ClientData pathItemListAccessItf (ClientData cd, char *name, TypeInfo **ti)
{
  PathItemList* pil = (PathItemList*)cd;
  int           idx;

  if ( *name == '.') {
    if ( name[1] =='\0') {
      if ( pil->itemN > 0) {
        itfAppendElement("item(0..%d)", pil->itemN-1);
      }
      *ti=NULL;
      return NULL; 
    }
    else {
      if (sscanf(name+1,"item(%d)", &idx) == 1) {
        *ti = itfGetType("PathItem"); 
        if ( idx >= 0 && idx < pil->itemN)
             return (ClientData)(pil->itemA+idx);
        else return  NULL;
      }
      *ti=NULL; return NULL;
    }
  }
  return NULL;
}

/* ------------------------------------------------------------------------
    pathItemListPuts
   ------------------------------------------------------------------------ */

static int pathItemListPutsItf (ClientData clientData, int argc, char *argv[]) 
{
  PathItemList* pi = (PathItemList*)clientData;
  int           i;
  for ( i = 0; i < pi->itemN; i++) {
    itfAppendResult("{ ");
    pathItemPutsItf((ClientData)(pi->itemA+i), argc, argv);
    itfAppendResult("}\n");
  }
  return TCL_OK;
}

/* ------------------------------------------------------------------------
    pathItemListAdd
   ------------------------------------------------------------------------ */

int pathItemListAdd( PathItemList* pil, int n,
                     int stateX, int senoneX, int phoneX, int wordX)
{
  int i;

  pil->itemA = (PathItem*)realloc( pil->itemA,(pil->itemN+n)*sizeof(PathItem));

  for ( i = pil->itemN; i < pil->itemN+n; i++) {
    pathItemInit( pil->itemA + i);

    pil->itemA[i].stateX  = stateX;
    pil->itemA[i].senoneX = senoneX;
    pil->itemA[i].phoneX  = phoneX;
    pil->itemA[i].wordX   = wordX;
  }
  i           = pil->itemN;
  pil->itemN += n;
  return i;
}

static int pathItemListAddItf (ClientData clientData, int argc, char *argv[]) 
{
  PathItemList *pi      = (PathItemList*)clientData;
  int           ac      =  argc-1;
  int           stateX  = -1;
  int           senoneX = -1;
  int           phoneX  = -1;
  int           wordX   = -1;
  int           n       =  1;

  if ( itfParseArgv (argv[0], &ac, argv+1, 0,
      "<n>",      ARGV_INT, NULL, &n,       NULL, "number of pathItems to add",
      "-stateX",  ARGV_INT, NULL, &stateX,  NULL, "state index",
      "-senoneX", ARGV_INT, NULL, &senoneX, NULL, "relative senone index",
      "-phoneX",  ARGV_INT, NULL, &phoneX,  NULL, "relative phone index",
      "-wordX",   ARGV_INT, NULL, &wordX,   NULL, "relative word index",
       NULL) != TCL_OK) return TCL_ERROR;

  itfAppendResult( "%d", pathItemListAdd( pi, n, stateX, senoneX, phoneX,
                                                 wordX));
  return TCL_OK;
}

/* ------------------------------------------------------------------------
    pathItemListClear
   ------------------------------------------------------------------------ */

int pathItemListClear( PathItemList* pil)
{
  int i;

  for ( i = 0; i < pil->itemN; i++) pathItemDeinit( pil->itemA + i);

  if ( pil->itemA) { free( pil->itemA); pil->itemA = NULL; }
  pil->itemN = 0;
  return i;
}

static int pathItemListClearItf (ClientData clientData, int argc, char *argv[]) 
{
  PathItemList *pi      = (PathItemList*)clientData;
  int           ac      =  argc-1;

  if ( itfParseArgv (argv[0], &ac, argv+1, 0,
       NULL) != TCL_OK) return TCL_ERROR;

  return pathItemListClear( pi);
}

/* ========================================================================
    Path
   ======================================================================== */

static ClientData pathCreateItf (ClientData clientData, int argc, char *argv[]) ;
static int pathFreeItf (ClientData clientData);

/* ------------------------------------------------------------------------
    Create Path Structure:
   ------------------------------------------------------------------------ */

int pathInit (Path *path, ClientData CD)
{
  path->name          = ((char*)CD)? strdup((char*) CD) : NULL;

  path->useN          = 0;
  path->firstFrame    = path->lastFrame = (-1);
  path->gscore        = 0.0;
  path->pitemListA    = NULL;
  path->pitemListN    = 0;

  path->senoneMissPen = 0.0;
  path->phoneMissPen  = 0.0;
  path->wordMissPen   = 0.0;

  return TCL_OK;
}

Path *pathCreate (char *name)
{
  Path *path = (Path*)malloc(sizeof(Path));

  if ((!path) || (pathInit(path,(ClientData)name) != TCL_OK)) {
    if (path) free (path);
    ERROR("cannot create %s\n",name);
    return NULL;
  }
  return path;
}

static ClientData pathCreateItf (ClientData clientData, int argc, char *argv[]) 
{ 
  char* name = NULL;

  if (itfParseArgv (argv[0], &argc, argv, 0,
      "<name>",  ARGV_STRING, NULL, &name, NULL,         "name of the path",
       NULL) != TCL_OK) return NULL;

  return (ClientData)pathCreate(name);
}

/* ------------------------------------------------------------------------
    Free StateGraph Structure
   ------------------------------------------------------------------------ */

int pathReset( Path* path)
{
  int frameX;

  if (path->lastFrame==(-1) && path->firstFrame==(-1)) return TCL_OK;
  if (path->lastFrame       <  path->firstFrame      ) return TCL_OK;

  for (frameX=0; frameX<=(path->lastFrame-path->firstFrame); frameX++) { 
    pathItemListDeinit( path->pitemListA + frameX);
  }
  if ( path->pitemListA) { free( path->pitemListA); path->pitemListA = NULL; }

  path->lastFrame  = path->firstFrame = (-1);
  path->pitemListN = 0;
  return TCL_OK;
}

static int pathResetItf (ClientData clientData, int argc, char *argv[])
{
  argc--;
  if (itfParseArgv(argv[0], &argc, argv+1, 0,
        NULL) != TCL_OK)  return TCL_ERROR;

  return pathReset ((Path*)clientData);
}

int pathDeinit( Path *path)
{


  if ( path->useN) {
    SERROR("Path '%s' still in use by other objects.\n",path->name);
    return TCL_ERROR;
  }
  if (path->lastFrame==(-1) && path->firstFrame==(-1)) return TCL_OK;
  if (path->lastFrame       <  path->firstFrame      ) return TCL_OK;

  if (path->name) free(path->name);
  return pathReset( path);
}

int pathFree (Path *path)
{
  if (pathDeinit(path) != TCL_OK) return TCL_ERROR;
  free(path);
  return TCL_OK;
}

static int pathFreeItf (ClientData clientData)
{
  return pathFree ((Path*)clientData);
}

/* ------------------------------------------------------------------------
    pathMake
   ------------------------------------------------------------------------ */
 
int pathMake( Path* path, SenoneSet* snsP, int from,  int to, 
                                           int skipl, int skipt)
{
  int i, frameN;

  pathReset( path);
  /* Either to and from contain valid values or they are both < 0;
     skipl and skipt are only meaningful if to and from < 0 */ 
  if (( from >= 0 && to < from) || (from > 0 && to < 0) ||
      (from < 0 && to > 0) || (skipl < 0 || skipt < 0)  ||
      ((skipl!=0 || skipt!=0) && (from >= 0 || to >= 0)) ) {
    ERROR("Unreasonable alignment limits: %d[%d...%d]%d.\n", skipl, from, to, skipt); 
    return TCL_ERROR;
  }

  frameN = snsFrameN ( snsP, &(path->fromFrame), &(path->frameShift), NULL);

  if ( from >= 0) { path->firstFrame = from;  path->lastFrame = to; }
  else            { path->firstFrame = skipl; path->lastFrame = frameN-skipt-1; }

  if (path->lastFrame >= frameN) { 
    ERROR("Don't have frames from %d to %d, only from %d to %d.\n", path->firstFrame,path->lastFrame,0,frameN-1);
    path->lastFrame  = path->firstFrame = (-1);
    path->pitemListN = 0;
    return TCL_ERROR;
  }

  path->pitemListN =  path->lastFrame - path->firstFrame + 1;
  path->pitemListA = (PathItemList*)malloc( path->pitemListN * 
                                            sizeof(PathItemList));

  for ( i = 0; i < path->pitemListN; i++) 
    pathItemListInit( path->pitemListA+i);

  return TCL_OK;
}

static int pathMakeItf( ClientData clientData, int argc, char *argv[])
{
  Path*      path  = (Path*)clientData;
  SenoneSet* snsP  =  NULL;
  int        ac    =  argc-1;
  int        from  = -1, to = -1, skipl = 0, skipt = 0;
  char*      evalS =  NULL;

  if ( itfParseArgv( argv[0], &ac, argv+1, 0,
      "<senoneSet>", ARGV_OBJECT, NULL, &snsP,"SenoneSet", "name of the SenoneSet object",
      "-eval",   ARGV_STRING, NULL, &evalS, NULL,"feature set eval string",
      "-from",   ARGV_INT, NULL, &from,   NULL,  "frame where to start alignment",
      "-to",     ARGV_INT, NULL, &to,     NULL,  "frame where to end alignment",
      "-skipl",  ARGV_INT, NULL, &skipl,  NULL,  "leading  frames to skip",
      "-skipt",  ARGV_INT, NULL, &skipt,  NULL,  "trailing frames to skip",
       NULL) != TCL_OK) return TCL_ERROR;

  if ( evalS && (snsFesEval( snsP, evalS) != TCL_OK)) {
    ERROR("Error evaluating feature description file.\n");
    return TCL_ERROR;
  }
  return pathMake( path, snsP, from, to, skipl, skipt);
}

/* ------------------------------------------------------------------------
    pathConfigureItf
   ------------------------------------------------------------------------ */

static int pathConfigureItf (ClientData clientData, char *var, char *val)
{
  Path *path = (Path*)clientData;
  if (! path) return TCL_ERROR;

  if (var==NULL) {

    ITFCFG(pathConfigureItf,clientData,"name");
    ITFCFG(pathConfigureItf,clientData,"useN");
    ITFCFG(pathConfigureItf,clientData,"firstFrame");
    ITFCFG(pathConfigureItf,clientData,"lastFrame");
    ITFCFG(pathConfigureItf,clientData,"wordMissPen");
    ITFCFG(pathConfigureItf,clientData,"phoneMissPen");
    ITFCFG(pathConfigureItf,clientData,"senoneMissPen");
    return TCL_OK;
  }
  ITFCFG_CharPtr(var,val,"name",         path->name,          1);
  ITFCFG_Int(    var,val,"useN",         path->useN,          1);
  ITFCFG_Int(    var,val,"firstFrame",   path->firstFrame,    1);
  ITFCFG_Int(    var,val,"lastFrame",    path->lastFrame,     1);
  ITFCFG_Float(  var,val,"wordMissPen",  path->wordMissPen,   0);
  ITFCFG_Float(  var,val,"phoneMissPen", path->phoneMissPen,  0);
  ITFCFG_Float(  var,val,"senoneMissPen",path->senoneMissPen, 0);

  ERROR("unknown option '-%s %s'.\n", var, val ? val : "?");
  return TCL_ERROR;   
}

/* ------------------------------------------------------------------------
    pathAccessItf
   ------------------------------------------------------------------------ */

static ClientData pathAccessItf (ClientData cd, char *name, TypeInfo **ti)
{
  Path* path = (Path*)cd;
  int   idx;

  if ( *name == '.') {
    if ( name[1] =='\0') {
      if ( path->pitemListN > 0) {
        itfAppendElement("itemList(0..%d)", path->pitemListN - 1);
      }
      *ti=NULL;
      return NULL; 
    }
    else {
      if (sscanf(name+1,"itemList(%d)", &idx) == 1) {
        *ti = itfGetType("PathItemList"); 
        if ( idx >= 0 && idx < path->pitemListN)
             return (ClientData)(path->pitemListA+idx);
        else return  NULL;
      }
      *ti=NULL; return NULL;
    }
  }
  return NULL;
}

/* ------------------------------------------------------------------------
    pathPuts
   ------------------------------------------------------------------------ */

static int pathPuts (Path *path, int argc, char *argv[])
{
  int ac = argc-1;
  int  from = -1, to = -1, frX;

  if ( itfParseArgv (argv[0], &ac, argv+1, 0,
      "-from",   ARGV_INT,    NULL, &from,   NULL,   "frame where to start output",
      "-to",     ARGV_INT,    NULL, &to,     NULL,   "frame where to end output",
       NULL) != TCL_OK) return TCL_ERROR;

  if (from == (-1))             from = path->firstFrame;
  if (from <  path->firstFrame) from = path->firstFrame;
  if (to   == (-1))             to   = path->lastFrame;
  if (to   >  path->lastFrame)  to   = path->lastFrame;

  /* ------------------------------------------------------ */
  /* produce output to tcl interpreter (that's the default) */
  /* ------------------------------------------------------ */

  itfAppendResult("%s %d ",path->name?path->name:"(null)",path->useN);
  itfAppendResult("%d %d %+e\n",path->firstFrame,path->lastFrame,path->gscore);

  if(path->firstFrame == -1) return TCL_OK;
	
  for (frX = from; frX <= to; frX++) {

    int i, relX = frX + from - path->firstFrame;
    itfAppendResult("{ %+e %+e %+e %d ",
                    path->pitemListA[relX].logScale,
                    path->pitemListA[relX].beam,
                    path->pitemListA[relX].best,
                    path->pitemListA[relX].itemN);

    for ( i = 0; i < path->pitemListA[relX].itemN; i++) {

      itfAppendResult("{ %d %d ",path->pitemListA[relX].itemA[i].stateX,
                                 path->pitemListA[relX].itemA[i].senoneX);
      itfAppendResult("%d %d ",  path->pitemListA[relX].itemA[i].phoneX,
                                 path->pitemListA[relX].itemA[i].wordX);

      itfAppendResult("%+e ",  path->pitemListA[relX].itemA[i].alpha); 
      itfAppendResult("%+e ",  path->pitemListA[relX].itemA[i].beta); 
      itfAppendResult("%+e ",  path->pitemListA[relX].itemA[i].gamma); 
      itfAppendResult("%+e } ",path->pitemListA[relX].itemA[i].lscore); 
    }
    itfAppendResult("}\n");
  }
  return TCL_OK;
}

static int pathPutsItf (ClientData clientData, int argc, char *argv[]) 
{ 
  return pathPuts((Path*)clientData,argc,argv);
}

/* ========================================================================= */
/* show the contents of a path using the underlying hmm (rather verbose)     */
/* ========================================================================= */

/* ------------------------------------------------------------------------
    pathWordLabels
   ------------------------------------------------------------------------ */

static int pathWordLabelsVariant( Dictionary* dict, int wordX, int* phonesA, 
                                                        int* tagsA, 
                                                        int  phonesN)
{
  while ( wordX >= 0) {
    int  n    = dict->list.itemA[wordX].word.itemN;
    int* pA   = dict->list.itemA[wordX].word.phoneA;
    int* tA   = dict->list.itemA[wordX].word.tagA;

    if ( n == phonesN) {
      int i;
      for ( i =  0; i < n && pA[i] == phonesA[i] &&
                             tA[i] == (tagsA[i] & dict->tags->modMask); i++);
      if (  i == n) return wordX;
    }
    wordX = dict->list.itemA[wordX].varX;
  }
  return -1;
}

static int pathWordLabels (Path* path, HMM* hmm, int from, int to, int mode)
{
  WordGraph*  wgP     =  hmm->wordGraph;
  PhoneGraph* pgP     =  hmm->phoneGraph;
  StateGraph* sgP     =  hmm->stateGraph;
  Dictionary* dictP   =  hmm->dict; 
  int         wordX   = -1, frX;
  int         phoneX  = -1;
  int         phonesN =  0;
  int         phonesX = 0;
  int*        phonesA =  NULL;
  int*        tagsA   =  NULL;
  int         fromX   = -1;

  if( path->firstFrame == -1) return TCL_ERROR;
  if (from < 0) from = 0;
  if (to < 0 || to >= path->pitemListN) to = path->pitemListN - 1;
  if (from > to) return TCL_ERROR;

  /* Verbose output */
  if (mode) itfAppendResult ("words {\n");

  for (frX = from; frX <= to; frX++) {
    if (path->pitemListA[frX].itemN) {
      int i = path->pitemListA[frX].itemA[0].wordX;

      if (i != wordX) { /* new word starts here */
        if (wordX >= 0) {
          int varX = pathWordLabelsVariant (dictP, wgP->word[wordX], 
                                            phonesA, tagsA, phonesX+1);
          if (varX < 0) varX = wgP->word[wordX];
	  if (mode) { /* Include state information */
	    int x1, x2;
	    for (x1 = 0;  x1 < sgP->stateN; x1++) if (sgP->wordX[x1] == wordX) break;
	    for (x2 = x1; x2 < sgP->stateN; x2++) if (sgP->wordX[x2] != wordX) break;
	    if (sgP->wordX[x2] != wordX) x2--;
	    itfAppendResult (" { %s { %d %d } { %d %d } }\n", dictP->list.itemA[varX].name,
			     fromX, frX-1, x1, x2); 
	  } else {
	    itfAppendResult (" {%s %d %d}", dictP->list.itemA[varX].name,
			     fromX, frX-1); 
	  }
        }

        wordX   = i;
        fromX   = frX;
        phoneX  = path->pitemListA[frX].itemA[0].phoneX;
        phonesX = 0;

        if ( phonesX >= phonesN) {
          phonesN =  phonesX + 1;
          phonesA = (int*)realloc( phonesA, phonesN * sizeof(int));
          tagsA   = (int*)realloc( tagsA,   phonesN * sizeof(int));
        }
        phonesA[phonesX] = pgP->phone[phoneX];
        tagsA[phonesX]   = pgP->tag[phoneX];
      } else {
        if ( phoneX != path->pitemListA[frX].itemA[0].phoneX) {
          phoneX = path->pitemListA[frX].itemA[0].phoneX;

          if ( ++phonesX >= phonesN) {
            phonesN =  phonesX + 1;
            phonesA = (int*)realloc( phonesA, phonesN * sizeof(int));
            tagsA   = (int*)realloc( tagsA,   phonesN * sizeof(int));
          }
          phonesA[phonesX] = pgP->phone[phoneX];
          tagsA[phonesX]   = pgP->tag[phoneX];
	}
      }
    }
  }

  if (wordX >= 0) {
    int varX = pathWordLabelsVariant( dictP, wgP->word[wordX], phonesA, tagsA, 
                                      phonesX+1);
    if ( varX < 0) varX = wgP->word[wordX];
    if (mode) { /* Include state information */
      int x1, x2;
      for (x1 = 0;  x1 < sgP->stateN; x1++) if (sgP->wordX[x1] == wordX) break;
      for (x2 = x1; x2 < sgP->stateN; x2++) if (sgP->wordX[x2] != wordX) break;
      if (sgP->wordX[x2] != wordX) x2--;
      itfAppendResult (" { %s { %d %d } { %d %d } }\n", dictP->list.itemA[varX].name,
		       fromX, frX-1, x1, x2); 
    } else {
      itfAppendResult (" {%s %d %d}", dictP->list.itemA[varX].name, fromX, frX-1); 
    }
  }

  if (mode) itfAppendResult("}\n");

  if ( phonesA) { free( phonesA); phonesA = NULL; }
  if ( tagsA)   { free( tagsA);   tagsA   = NULL; }

  return TCL_OK;
}

static int pathWordLabelsItf (ClientData clientData, int argc, char *argv[]) 
{ 
  HMM*  hmm   = NULL;
  int   ac    = argc-1;
  int   from  = 0;
  int   to    = -1;

  if ( itfParseArgv (argv[0], &ac, argv+1, 0,
      "<hmm>",  ARGV_OBJECT, NULL, &hmm,  "HMM", "name of the HMM object",
      "-from",  ARGV_INT,    NULL, &from,  NULL,  "start frame",
      "-to",    ARGV_INT,    NULL, &to,    NULL,  "end frame",
       NULL) != TCL_OK) return TCL_ERROR;

  return pathWordLabels((Path*)clientData,hmm,from,to,0);
}

/* ------------------------------------------------------------------------
    pathPhoneLabels
   ------------------------------------------------------------------------ */

static int pathPhoneLabels (Path* path, HMM* hmm, int from, int to, int mode)
{
  PhoneGraph* pgP     =  hmm->phoneGraph;
  Dictionary* dictP   =  hmm->dict; 
  Phones*     phonesP =  dictP->phones;
  int         frX;
  int         phoneX  = -1;
  int         fromX   = -1;

  if (path->firstFrame == -1) return TCL_ERROR;
  if (from < 0) from = 0;
  if (to < 0 || to >= path->pitemListN) to = path->pitemListN - 1;
  if (from > to) return TCL_ERROR;
  
  if (mode) itfAppendResult ("phones {\n");

  for (frX = from; frX <= to; frX++) {
    if (path->pitemListA[frX].itemN) {
      int i = path->pitemListA[frX].itemA[0].phoneX;

      if (i != phoneX) { /* new phone starts here */
        if (phoneX >= 0 && pgP->phone[phoneX] >= 0) {
	  if (mode) { /* Verbose output */
	    itfAppendResult ("{ %s { %d %d } { %d %d } }\n", 
			     phonesP->list.itemA[pgP->phone[phoneX]].name,
			     fromX, frX-1, path->pitemListA[fromX].itemA[0].stateX,
			     path->pitemListA[frX-1].itemA[0].stateX); 
	  } else {
	    itfAppendResult (" {%s %d %d}", 
			     phonesP->list.itemA[pgP->phone[phoneX]].name,
			     fromX, frX-1); 
	  }
        }
        phoneX  = i;
        fromX   = frX;
      }
    }
  }

  if (phoneX >= 0 && pgP->phone[phoneX] >= 0) {
    if (mode) {
      itfAppendResult ("{ %s { %d %d } { %d %d } }\n", 
		       phonesP->list.itemA[pgP->phone[phoneX]].name,
		       fromX, frX-1, path->pitemListA[fromX].itemA[0].stateX,
		       path->pitemListA[frX-1].itemA[0].stateX); 
    } else {
      itfAppendResult (" {%s %d %d}", 
		       phonesP->list.itemA[pgP->phone[phoneX]].name,
		       fromX, frX-1); 
    }
  }

  if (mode) itfAppendResult ("}\n");

  return TCL_OK;
}

static int pathPhoneLabelsItf (ClientData clientData, int argc, char *argv[]) 
{ 
  HMM*  hmm   = NULL;
  int   ac    = argc-1;
  int   from  = 0;
  int   to    = -1;

  if ( itfParseArgv (argv[0], &ac, argv+1, 0,
      "<hmm>",  ARGV_OBJECT, NULL, &hmm,  "HMM", "name of the HMM object",
      "-from",  ARGV_INT,    NULL, &from,  NULL,  "start frame",
      "-to",    ARGV_INT,    NULL, &to,    NULL,  "end frame",
       NULL) != TCL_OK) return TCL_ERROR;

  return pathPhoneLabels ((Path*)clientData, hmm, from, to, 0);
}

/* ------------------------------------------------------------------------
    pathLabels
   ------------------------------------------------------------------------ */

static int pathLabelsItf (ClientData clientData, int argc, char *argv[])
{
  int               ac = argc-1;
  Path           *path = (Path*)clientData;
  Dictionary     *dict = NULL;
  HMM             *hmm = NULL;
  Phones       *phones = NULL;
  SenoneSet *senoneSet = NULL;
  char           *what = "word";
  int            frameX;

  if (itfParseArgv( argv[0], &ac, argv+1, 0,
      "hmm",     ARGV_OBJECT, NULL, &hmm, "HMM", "the underlying HMM",
      "-what",   ARGV_STRING, NULL, &what, NULL, "list of what to display",
       NULL) != TCL_OK) return TCL_ERROR;

  dict      = hmm->dict;
  phones    = dict->phones;
  senoneSet = hmm->stateGraph->snsP;

  if(path->firstFrame == -1)
    return TCL_OK;

  for (frameX=path->firstFrame; frameX<=path->lastFrame; frameX++) {
    if (path->pitemListA[frameX-path->firstFrame].itemN!=1) {
      WARN ("pathLabelsItf: can't show forward-backward labels.\n");
      return TCL_OK;
    }
  }

  if (toupper (*what) == 'W')
    return pathWordLabels  ((Path*)clientData, hmm, path->firstFrame, path->lastFrame, 1);

  if (toupper (*what) == 'P')
    return pathPhoneLabels ((Path*)clientData, hmm, path->firstFrame, path->lastFrame, 1);

  if (toupper (*what) == 'S') {
    int thisSnX, lastSnX = path->pitemListA[path->firstFrame].itemA[0].senoneX, beginFrameX = path->firstFrame;
    itfAppendResult("senones {\n{ %s { %d ",lastSnX>=0?senoneSet->list.itemA[lastSnX].name:"(nil)",0);
    for (frameX=path->firstFrame; frameX<=path->lastFrame; frameX++)
    {
      if ((thisSnX=path->pitemListA[frameX].itemA[0].senoneX)!=lastSnX) 
	itfAppendResult ("%d } { %d %d } }\n{ %s { %d ",frameX-1,
			 path->pitemListA[beginFrameX].itemA[0].stateX,
			 path->pitemListA[frameX-1].itemA[0].stateX,
			 (thisSnX>=0 && thisSnX<senoneSet->list.itemN)?senoneSet->list.itemA[thisSnX].name:"(nil)",
			 frameX);
      lastSnX = thisSnX; beginFrameX = frameX;
    }
    itfAppendResult ("%d } { %d %d } }\n}\n", path->lastFrame,
		     path->pitemListA[beginFrameX].itemA[0].stateX,
		     path->pitemListA[frameX-1].itemA[0].stateX);
  }

  return TCL_OK;
}

static int pathDurationalEntropyItf (ClientData clientData, int argc, char *argv[])
{
  int               ac = argc-1;
  Path           *path = (Path*)clientData;
  Dictionary     *dict = NULL;
  HMM             *hmm = NULL;
  Phones       *phones = NULL;
  SenoneSet *senoneSet = NULL;
  int         wordX   = -1, frX, frameX, length;
  int         fromX   = -1;
  int 		varX, nStates = 0;
  float 	ec, entropy;
  WordGraph *wgP = NULL;
  int thisSnX, lastSnX, beginFrameX;

  if (itfParseArgv( argv[0], &ac, argv+1, 0,
      "hmm",     ARGV_OBJECT, NULL, &hmm, "HMM", "the underlying HMM",
       NULL) != TCL_OK) return TCL_ERROR;

  dict      = hmm->dict;
  phones    = dict->phones;
  senoneSet = hmm->stateGraph->snsP;
  wgP     =  hmm->wordGraph;

  if( path->firstFrame == -1) return TCL_ERROR;

  for (frameX=path->firstFrame; frameX<=path->lastFrame; frameX++) {
    if (path->pitemListA[frameX-path->firstFrame].itemN!=1) {
      WARN ("pathLabelsItf: can't show forward-backward labels.\n");
      return TCL_OK;
    }
  }

  /*** Change by Jitendra start ****/
  path->firstFrame = 0;
  path->lastFrame = path->pitemListN - 1; 
  length = 0;
  ec = 0.0;
  nStates = 0;
  /*** Change by Jitendra end ****/

  lastSnX = path->pitemListA[path->firstFrame].itemA[0].senoneX;
  beginFrameX = path->firstFrame;

  for (frX = path->firstFrame; frX <= path->lastFrame; frX++) {
    if (path->pitemListA[frX].itemN) {
      int i;
      if ((thisSnX=path->pitemListA[frX].itemA[0].senoneX)!=lastSnX){
    	/* itfAppendResult (" {%d %d %d }", lastSnX, beginFrameX, frX-1); */
    	ec += (float)(frX-beginFrameX)*log((float)(frX-beginFrameX));
	nStates++;
	length += (frX-beginFrameX);
	lastSnX = thisSnX; beginFrameX = frX;
      }
      i = path->pitemListA[frX].itemA[0].wordX;

      if (i != wordX) { /* new word starts here */
        if (wordX >= 0) {
          entropy = ec/(float)length - log((float)length);
	  entropy /= log((float)nStates);
          varX = wgP->word[wordX];
	    itfAppendResult (" {%s %d %d %f}", dict->list.itemA[varX].name,
			     fromX, frX-1, entropy); 
        }

        wordX   = i;
        fromX   = frX;
	ec = 0.0;
	nStates=0;
	length = 0;
      } 
    }

  }
  /* itfAppendResult (" {%d %d %d }", thisSnX, beginFrameX, frX-1); */
  ec += (float)(frX-beginFrameX)*log((float)(frX-beginFrameX));
  length += (frX-beginFrameX);
  nStates++;
  if (wordX >= 0) {
    varX = wgP->word[wordX];
          entropy = ec/(float)length - log((float)length);
	  entropy /= log((float)nStates);
      itfAppendResult (" {%s %d %d %f}", dict->list.itemA[varX].name, fromX, frX-1, entropy); 
  }

  return TCL_OK;
}

/* ------------------------------------------------------------------------
     pathkPjkDMC    compute the pjk for DMC
                    needed for AFs
   ------------------------------------------------------------------------ */

float pathPjkDMC (Path* path, HMM* hmm, SenoneSet* sns, FMatrix* mw, FMatrix* mp, int from, int to, int idx, int flag)
{
  StreamArray* staP = &(sns->strA);
  float           r = 0;
  int   frX, stX, i;
  
  /* Clear the output array */
  for (stX = 0; stX < staP->streamN; stX++)
    mp->matPA[stX][idx] = 0;

  /* Loop over all frames */
  for (frX = from; frX <= to; frX++) {
    float*   sP = sns->streamCache->matPA[frX];

    /* Loop over all path items */
    for (i = 0; i < path->pitemListA[frX].itemN; i++) {
      Senone* snP = sns->list.itemA + path->pitemListA[frX].itemA[i].senoneX;

      /* Loop over all streams: compute the local score (a la opt_stream) */
      for (stX = 0; stX < snP->streamN; stX++) {
	int dsX = snP->classXA[stX];
	int cX  = staP->cBaseA[stX] - staP->cBaseA[1] + dsX;
	
	if (!flag && !mw->matPA[stX][0]) continue;

	/* If stX==0 do not use the cache */
	if (dsX < 0) continue;
	if (!stX) {
	  float ts;
	  staP->strPA[0]->scoreFct (staP->cdA[0], &dsX, &ts, 1, frX);
	  mp->matPA[0][idx] += ts;
	  continue;
	}

	/* Read the local feature score, hopefully from cache */
	if (sP[cX] == -FLT_MAX) {
	  staP->strPA[stX]->scoreFct (staP->cdA[stX], &dsX, sP+cX, 1, frX);
	  sns->streamCache->count++;
	}
	mp->matPA[stX][idx] += sP[cX];
      }
    }
  }
  
  /* Compute the total sum */
  for (stX = 0; stX < staP->streamN; stX++)
    r += mw->matPA[stX][0] * mp->matPA[stX][idx];

  /* for (stX = 1; stX < sns->strA.streamN; stX++) {
    double score = 0;
    for (frX = from; frX <= to; frX++) {
      int i;
      for (i = 0; i < path->pitemListA[frX].itemN; i++) {
	score += path->pitemListA[frX].itemA[i].lscore =
	  sns->scoreFctP (sns, path->pitemListA[frX].itemA[i].senoneX,
			  path->firstFrame + frX);
      }
    }
    m->matPA[stX][0] = score;
  } */
  
  return r;
}

static int pathPjkDMCItf (ClientData clientData, int argc, char *argv[]) 
{ 
  HMM*  hmm   = NULL;
  FMatrix* mw = NULL;
  FMatrix* mp = NULL;
  SenoneSet* sns = NULL;
  Path*  path = (Path*) clientData;
  int   ac    = argc-1;
  int   from  = 0;
  int   to    = -1;
  int     idx = 0;
  int    flag = 0;

  if ( itfParseArgv (argv[0], &ac, argv+1, 0,
      "<hmm>",   ARGV_OBJECT, NULL, &hmm,  "HMM", "name of the HMM object",
      "<sns>",   ARGV_OBJECT, NULL, &sns,  "SenoneSet", "name of the SenoneSet object",
      "<wghts>", ARGV_OBJECT, NULL, &mw,   "FMatrix", "name of the wghts FMatrix object (in)",
      "<pjks>",  ARGV_OBJECT, NULL, &mp,   "FMatrix", "name of the pjks FMatrix object (out)",
      "-from",   ARGV_INT,    NULL, &from,  NULL,  "start frame",
      "-to",     ARGV_INT,    NULL, &to,    NULL,  "end frame",
      "-idx",    ARGV_INT,    NULL, &idx,   NULL,  "use entry in pjks matrix",
      "-zero",   ARGV_INT,    NULL, &flag,  NULL,  "train streams with 0 prob",
       NULL) != TCL_OK) return TCL_ERROR;

  if (from < 0) from = 0;
  if (to < 0 || to >= path->pitemListN) to = path->pitemListN - 1;
  if (from > to || path->firstFrame == -1) {
    ERROR ("pathPjkDMC: need reasonable path\n");
    return TCL_ERROR;
  }

  if (!sns->streamCache || sns->streamCache->m != to+1) {
    ERROR ("pathPjkDMC: %s and %s.streamCache inconsistent\n", path->name, sns->name);
    return TCL_ERROR;
  }

  if (mp->n <= idx || mp->m != sns->strA.streamN) {
    ERROR ("pathPjkDMC: <pjks> matrix' dimensions don't match\n");
    return TCL_ERROR;  
  }

  if (mw->n != 1 && mw->m != sns->strA.streamN) {
    ERROR ("pathPjkDMC: <wghts> matrix' dimensions don't match\n");
    return TCL_ERROR;  
  }

  itfAppendResult( " %e", pathPjkDMC (path, hmm, sns, mw, mp, from, to, idx, flag));
  return TCL_OK;
}


/* ------------------------------------------------------------------------
    pathLScore   fill the missing fields that have not been filled during 
                 alignment, frames from .. to
   ------------------------------------------------------------------------ */

float pathLScore (Path* path, HMM* hmm, int from, int to, int gamma)
{
  int        frX;
  SenoneSet* sns   = amodelSetSenoneSet(hmm->amodels);
  double     score = 0;

  if( path->firstFrame == -1) return 9e9;
  if (from < 0) from = 0;
  if (to < 0 || to >= path->pitemListN) to = path->pitemListN - 1;
  if (from > to) return 9e9;
  
  if (gamma) for ( frX = from; frX <= to; frX++) {
    int i;

    for ( i = 0; i < path->pitemListA[frX].itemN; i++) {
      score += path->pitemListA[frX].itemA[i].gamma * (path->pitemListA[frX].itemA[i].lscore =
	sns->scoreFctP( sns, path->pitemListA[frX].itemA[i].senoneX,
			path->firstFrame + frX));
    }
  } else for ( frX = from; frX <= to; frX++) {
    int i;

    for ( i = 0; i < path->pitemListA[frX].itemN; i++) {
      score += path->pitemListA[frX].itemA[i].lscore =
	sns->scoreFctP( sns, path->pitemListA[frX].itemA[i].senoneX,
		 path->firstFrame + frX);
    }
  }
  return score;
}

static int pathLScoreItf (ClientData clientData, int argc, char *argv[]) 
{ 
  HMM*  hmm   = NULL;
  int   ac    = argc-1;
  char* evalS = NULL;
  int   from  = 0;
  int   to    = -1;
  int   gamma = 0;

  if ( itfParseArgv (argv[0], &ac, argv+1, 0,
      "<hmm>",  ARGV_OBJECT, NULL, &hmm,  "HMM", "name of the HMM object",
      "-eval",  ARGV_STRING, NULL, &evalS, NULL,  "feature set eval string",
      "-from",  ARGV_INT,    NULL, &from,  NULL,  "start frame",
      "-to",    ARGV_INT,    NULL, &to,    NULL,  "end frame",
      "-gamma", ARGV_INT,    NULL, &gamma, NULL,  "use gamma values",
       NULL) != TCL_OK) return TCL_ERROR;

  if ( evalS && (snsFesEval(amodelSetSenoneSet(hmm->amodels),evalS) != TCL_OK)) {
    ERROR("Error evaluating feature description file.\n");
    return TCL_ERROR;
  }
  itfAppendResult( " %e", pathLScore((Path*)clientData,hmm,from,to,gamma));
  return TCL_OK;
}

/* ------------------------------------------------------------------------
    pathViterbi
   ------------------------------------------------------------------------ */

int pathViterbiMMIE (Path* path, HMM* hmm, PhoneGraph *pgraphP, int from,   int to, 
                                       int skipl,  int skipt, 
		                       float beam, int topN, int label,
		                       int bpCleanUpMod, int bpCleanUpMul)
{
  int frameX;

  assert(    path && hmm);

  if (! label)
    pathMake( path, amodelSetSenoneSet(hmm->amodels), from, to, skipl, skipt);

  /* --------------- */
  /* fill beam array */
  /* --------------- */

  for ( frameX = 0; frameX < path->pitemListN; frameX++) 
    path->pitemListA[frameX].beam = beam;

  return viterbiMMIE( path, hmm, pgraphP, topN, bpCleanUpMod, bpCleanUpMul);
}

int pathViterbiMMIE_OMP (Path* path, HMM* hmm, StateGraph* stateGraph, 
                         PhoneGraph *pgraphP, int from, int to,
                         int skipl,  int skipt,
                         float beam, int topN, int label,
                         int bpCleanUpMod, int bpCleanUpMul)
{
  int frameX;

  assert(    path && hmm);

  if (! label)
    pathMake( path, amodelSetSenoneSet(hmm->amodels), from, to, skipl, skipt);

  /* --------------- */
  /* fill beam array */
  /* --------------- */

  for ( frameX = 0; frameX < path->pitemListN; frameX++)
    path->pitemListA[frameX].beam = beam;

  return viterbiMMIE_OMP( path, hmm, stateGraph, pgraphP, topN, bpCleanUpMod, bpCleanUpMul);
}

int pathViterbi (Path* path, HMM* hmm, int from,   int to, 
                                       int skipl,  int skipt, 
		                       float beam, int topN, int label,
		                       int bpCleanUpMod, int bpCleanUpMul)
{
  int frameX;

  assert(    path && hmm);

  if (! label)
    pathMake( path, amodelSetSenoneSet(hmm->amodels), from, to, skipl, skipt);

  /* --------------- */
  /* fill beam array */
  /* --------------- */

  for ( frameX = 0; frameX < path->pitemListN; frameX++) 
    path->pitemListA[frameX].beam = beam;

  return viterbi( path, hmm, topN, bpCleanUpMod, bpCleanUpMul);
}

static int pathViterbiItf( ClientData clientData, int argc, char *argv[])
{
  HMM   *hmm   =  NULL;
  int    ac    =  argc-1;
  int    from  = -1, to = -1, skipl = 0, skipt = 0;
  int    topN  =  0;
  int    label =  0;
  int    bpMod =  -1, bpMul = 3;
  Path  *path  = (Path*)clientData;
  float  beam  =  9.9e19;
  char*  evalS =  NULL;

  if ( itfParseArgv( argv[0], &ac, argv+1, 0,
      "<hmm>",   ARGV_OBJECT, NULL, &hmm,    "HMM", "name of the HMM object",
      "-eval",   ARGV_STRING, NULL, &evalS,  NULL,  "feature set eval string",
      "-from",   ARGV_INT,    NULL, &from,   NULL,  "frame where to start alignment",
      "-to",     ARGV_INT,    NULL, &to,     NULL,  "frame where to end alignment",
      "-skipl",  ARGV_INT,    NULL, &skipl,  NULL,  "leading frames to skip",
      "-skipt",  ARGV_INT,    NULL, &skipt,  NULL,  "trailing frames to skip",
      "-beam",   ARGV_FLOAT,  NULL, &beam,   NULL,  "constant beam size",
      "-topN",   ARGV_INT,    NULL, &topN,   NULL,  "topN pruning",
      "-label",  ARGV_INT,    NULL, &label,  NULL,  "viterbi follows labels in path",
      "-bpMod",  ARGV_INT,    NULL, &bpMod,  NULL,  "after every X frames clean up bpTable (<0 never)",
      "-bpMul",  ARGV_INT,    NULL, &bpMul,  NULL,  "go Y * X frames back during cleanup (<1 start at first frame)",
       NULL) != TCL_OK) return TCL_ERROR;

  if ( evalS && (snsFesEval(amodelSetSenoneSet(hmm->amodels),evalS) != TCL_OK)) {
    ERROR("Error evaluating feature description file.\n");
    return TCL_ERROR;
  }
  if ( pathViterbi( path, hmm, from, to, skipl, skipt, 
                    beam, topN, label, bpMod, bpMul) != TCL_OK)
       return TCL_ERROR;

  itfAppendResult("%e", path->gscore);
  return TCL_OK;
}

/* ------------------------------------------------------------------------
    pathFwdBwd
   ------------------------------------------------------------------------ */

int pathFwdBwd( Path* path, HMM* hmm, int from,   int to, 
                                      int skipl,  int skipt, 
                                      int topN,   int width, int label)
{
  int frameX;

  assert(    path && hmm);

  if (! label)
    pathMake( path, amodelSetSenoneSet(hmm->amodels), from, to, skipl, skipt);

  /* --------------- */
  /* fill beam array */
  /* --------------- */

  for ( frameX = 0; frameX < path->pitemListN; frameX++) 
    path->pitemListA[frameX].beam = 0.0;

  return forwardBackward( path, hmm, topN, width);
}

static int pathFwdBwdItf( ClientData clientData, int argc, char *argv[])
{
  HMM   *hmm   =  NULL;
  int    ac    =  argc-1;
  int    from  = -1, to = -1, skipl = 0, skipt = 0;
  int    topN  =  0;
  int    width =  0;
  int    label =  0;
  Path*  path  = (Path*)clientData;
  char*  evalS =  NULL;

  if ( itfParseArgv( argv[0], &ac, argv+1, 0,
      "<hmm>",   ARGV_OBJECT, NULL, &hmm,    "HMM", "name of the HMM object",
      "-eval",   ARGV_STRING, NULL, &evalS,  NULL,  "feature set eval string",
      "-from",   ARGV_INT,    NULL, &from,   NULL,  "frame where to start alignment",
      "-to",     ARGV_INT,    NULL, &to,     NULL,  "frame where to end alignment",
      "-skipl",  ARGV_INT,    NULL, &skipl,  NULL,  "leading frames to skip",
      "-skipt",  ARGV_INT,    NULL, &skipt,  NULL,  "trailing frames to skip",
      "-topN",   ARGV_INT,    NULL, &topN,   NULL,  "topN pruning",
      "-width",  ARGV_INT,    NULL, &width,  NULL,  "maximal width of the path",
      "-label",  ARGV_INT,    NULL, &label,  NULL,  "viterbi follows labels in paht",
       NULL) != TCL_OK) return TCL_ERROR;

  if ( evalS && (snsFesEval(amodelSetSenoneSet(hmm->amodels),evalS) != TCL_OK)) {
    ERROR("Error evaluating feature description file.\n");
    return TCL_ERROR;
  }
  if ( pathFwdBwd(  path, hmm, from, to, skipl, skipt, 
                    topN, width, label) != TCL_OK)
       return TCL_ERROR;

  itfAppendResult("%e", path->gscore);
  return TCL_OK;
}

/* ------------------------------------------------------------------------
    pathMap    maps state senone indices for a path using the state indices 
               into an HMM that is supposed to have the target senone IDs.
               If a senoneSet and stream index is defined, instead of the
               senone indices the model indices of the given stream will be
               written to path...senoneX.
   ------------------------------------------------------------------------ */

static int pathMap( Path* path, HMM* hmm, SenoneSet* snsP, int strX, int cbX)
{
  StateGraph* graph   = NULL;
  int         i, j, n = 0, total = 0;

  if ( snsP && strX >= 0 && ! (strX < snsP->strA.streamN)) {
    ERROR("Undefined stream %d in SenoneSet %s.\n", strX, snsP->name);
    return TCL_ERROR;
  }

  assert( path && hmm);

  graph = hmm->stateGraph;

  if ( path->pitemListN < 0) { ERROR("Path structure is empty.\n"); 
                               return TCL_ERROR; }
  if ( graph->stateN    < 1) { ERROR("HMM.stateGraph is empty.\n");
                               return TCL_ERROR; }

  for ( i = 0; i < path->pitemListN; i++) {
    for ( j = 0; j < path->pitemListA[i].itemN; j++) {
      int stateX = path->pitemListA[i].itemA[j].stateX;
      total++;

      if ( stateX > -1 && stateX <  graph->stateN) {
        int snX = graph->senoneX[stateX];

        if ( !snsP || snX < snsP->list.itemN) {
	  // the !snsP case seems to be ok?
          int classX = (strX < 0) ? snX : snsP->list.itemA[snX].classXA[strX];
          if (cbX && strX>=0) 
             path->pitemListA[i].itemA[j].senoneX = ((DistribStream*)(snsP->strA.cdA[strX]))->dssP->list.itemA[classX].cbX;
          else
             path->pitemListA[i].itemA[j].senoneX = classX;
        }
        else { path->pitemListA[i].itemA[j].senoneX = -1; n++; }
      }
      else {   path->pitemListA[i].itemA[j].senoneX = -1; n++; }
    }
  }
  if ( n > 0) {
    WARN("Couldn't map %d of %d path items.\n", n, total);
    return TCL_ERROR;
  }
  return TCL_OK;
}

static int pathMapItf( ClientData cd, int argc, char *argv[])
{
  Path*      path  = (Path*)cd;
  HMM*       hmm   =  NULL;
  SenoneSet* snsP  =  NULL;
  int        code  = 0;
  int        strX  = -1;
  int        ac    =  argc-1;
  char       *name = NULL;
  //DistribStream *dstr = NULL;

  if ( itfParseArgv( argv[0], &ac, argv+1, 0,
      "<hmm>",      ARGV_OBJECT, NULL, &hmm,    "HMM",    "name of the HMM object",
      "-senoneSet", ARGV_OBJECT, NULL, &snsP,"SenoneSet", "name of the SenoneSet object",
      "-stream",    ARGV_INT,    NULL, &strX, NULL,       "index of stream",
      "-codebookX", ARGV_INT,    NULL, &code, NULL,       "want codebook instead of distrib indices (0/1)",
       NULL) != TCL_OK) return TCL_ERROR;

  if (code)
  {
     if (snsP==NULL) { WARN("no senone set specified\n"); code = 0; }
     if (strX<0)    { WARN("will ignore -codebookX because no stream specified\n"); code = 0; }
     if (code) 
     {  name = ((GenericName*)(snsP->strA.cdA[strX]))->name;
        if (itfGetObject(name,"DistribStream") == NULL) 
        {  ERROR("stream %d is not of type 'DistribStream', doesn't have codebooks\n",strX); return TCL_ERROR; }
     }
  }
  if ( (strX >= 0) ^ (snsP == 0) ) { /* ^ is binary XOR, result of comparison defined as 1 if TRUE */
    return pathMap( path, hmm, snsP, strX, code);
  } else {
    ERROR("you must specify either both -stream AND -senoneSet or none\n"); return TCL_ERROR; 
  }
}

/* ------------------------------------------------------------------------
    pathStateMatrix  fill out a matrix with the gamma scores:
                     for each relative state index the is a row in the 
                     matrix which has number of frames columns.
   ------------------------------------------------------------------------ */

int pathStateMatrix( Path* path, int first, int last, int from, int to, 
                     FMatrix* fmP)
{
  int minStX = -1;
  int maxStX = -1;
  int i, j;

  assert(    path);

  if ( path->pitemListN < 0) {
    ERROR("Path structure is empty.\n");
    return TCL_ERROR;
  }
  for ( i = 0; i < path->pitemListN; i++) {
    for ( j = 0; j < path->pitemListA[i].itemN; j++) {
      if (minStX < 0 || minStX > path->pitemListA[i].itemA[j].stateX)
          minStX = path->pitemListA[i].itemA[j].stateX;
      if (maxStX < 0 || maxStX < path->pitemListA[i].itemA[j].stateX)
          maxStX = path->pitemListA[i].itemA[j].stateX;
    }
  }
  if ( minStX < 0) {
    ERROR("No state indices in the path structure %s.\n", path->name);
    return TCL_ERROR;
  }
  if ( first < 0 || first > maxStX) first = minStX;
  if ( last  < 0 || last  < first)  last  = maxStX;

  if ( from  < 0)    from = 0;
  if ( to    < from) to   = path->pitemListN-1;

  fmatrixResize( fmP, 0,         0);
  fmatrixResize( fmP, to-from+1, last-first+1);

  for ( i = from; i <= to; i++) {
    for ( j = 0; j < path->pitemListA[i].itemN; j++) {
      int stX  = path->pitemListA[i].itemA[j].stateX;
      if (stX >= first || stX <= last)
          fmP->matPA[i-from][stX-first] += path->pitemListA[i].itemA[j].gamma;
    }
  }
  return TCL_OK;
}

static int pathStateMatrixItf( ClientData clientData, int argc, char *argv[])
{
  Path*    path  = (Path*)clientData;
  FMatrix* fmP   =  NULL;
  int      from  =  -1;
  int      to    =  -1;
  int      first =  -1;
  int      last  =  -1;
  int      ac    =  argc-1;

  if ( itfParseArgv( argv[0], &ac, argv+1, 0,
      "<FMatrix>", ARGV_OBJECT, NULL, &fmP, "FMatrix", "float matrix",
      "-from", ARGV_INT, NULL, &from, NULL,  "first frame of matrix",
      "-to",   ARGV_INT, NULL, &to,   NULL,  "last frame to include in matrix",
      "-first",ARGV_INT, NULL, &first,NULL,  "first state index to include",
      "-last", ARGV_INT, NULL, &last, NULL,  "last state index to include",
       NULL) != TCL_OK) return TCL_ERROR;

  return pathStateMatrix( path, first, last, from, to, fmP);
}

/* ------------------------------------------------------------------------
    pathSenoneMatrix  fill out a matrix with the gamma scores:
                      for each relative senone index there is a row in the 
                      matrix which has number of frames columns.
   ------------------------------------------------------------------------ */

int pathSenoneMatrix( Path* path, int first, int last, int from, int to, 
                      FMatrix* fmP)
{
  int minStX = -1;
  int maxStX = -1;
  int i, j;

  assert(    path);

  if ( path->pitemListN < 0) {
    ERROR("Path structure is empty.\n");
    return TCL_ERROR;
  }
  for ( i = 0; i < path->pitemListN; i++) {
    for ( j = 0; j < path->pitemListA[i].itemN; j++) {
      if (minStX < 0 || minStX > path->pitemListA[i].itemA[j].senoneX)
          minStX = path->pitemListA[i].itemA[j].senoneX;
      if (maxStX < 0 || maxStX < path->pitemListA[i].itemA[j].senoneX)
          maxStX = path->pitemListA[i].itemA[j].senoneX;
    }
  }
  if ( minStX < 0) {
    ERROR("No senone indices in the path structure %s.\n", path->name);
    return TCL_ERROR;
  }
  if ( first < 0 || first > maxStX) first = minStX;
  if ( last  < 0 || last  < first)  last  = maxStX;

  if ( from  < 0)    from = 0;
  if ( to    < from) to   = path->pitemListN-1;

  fmatrixResize( fmP, 0,         0);
  fmatrixResize( fmP, to-from+1, last-first+1);

  for ( i = from; i <= to; i++) {
    for ( j = 0; j < path->pitemListA[i].itemN; j++) {
      int stX  = path->pitemListA[i].itemA[j].senoneX;
      if (stX >= first || stX <= last)
          fmP->matPA[i-from][stX-first] += path->pitemListA[i].itemA[j].gamma;
    }
  }
  return TCL_OK;
}

static int pathSenoneMatrixItf( ClientData clientData, int argc, char *argv[])
{
  Path*    path  = (Path*)clientData;
  FMatrix* fmP   =  NULL;
  int      from  =  -1;
  int      to    =  -1;
  int      first =  -1;
  int      last  =  -1;
  int      ac    =  argc-1;

  if ( itfParseArgv( argv[0], &ac, argv+1, 0,
      "<FMatrix>", ARGV_OBJECT, NULL, &fmP, "FMatrix", "float matrix",
      "-from", ARGV_INT, NULL, &from, NULL,  "first frame of matrix",
      "-to",   ARGV_INT, NULL, &to,   NULL,  "last frame to include in matrix",
      "-first",ARGV_INT, NULL, &first,NULL,  "first senone index to include",
      "-last", ARGV_INT, NULL, &last, NULL,  "last senone index to include",
       NULL) != TCL_OK) return TCL_ERROR;

  return pathSenoneMatrix( path, first, last, from, to, fmP);
}

/* ------------------------------------------------------------------------
    pathPhoneMatrix  fill out a matrix with the cummulated gamma scores:
                     for each relative phone index the is a row in the 
                     matrix which has number of frames columns.
   ------------------------------------------------------------------------ */

int pathPhoneMatrix( Path* path, int first, int last, int from, int to, 
                     FMatrix* fmP)
{
  int minStX = -1;
  int maxStX = -1;
  int i, j;

  assert(    path);

  if ( path->pitemListN < 0) {
    ERROR("Path structure is empty.\n");
    return TCL_ERROR;
  }

  for ( i = 0; i < path->pitemListN; i++) {
    for ( j = 0; j < path->pitemListA[i].itemN; j++) {
      if (minStX < 0 || minStX > path->pitemListA[i].itemA[j].phoneX)
          minStX = path->pitemListA[i].itemA[j].phoneX;
      if (maxStX < 0 || maxStX < path->pitemListA[i].itemA[j].phoneX)
          maxStX = path->pitemListA[i].itemA[j].phoneX;
    }
  }
  if ( minStX < 0) {
    ERROR("No phone indices in the path structure %s.\n", path->name);
    return TCL_ERROR;
  }
  if ( first < 0 || first > maxStX) first = minStX;
  if ( last  < 0 || last  < first)  last  = maxStX;

  if ( from  < 0)    from = 0;
  if ( to    < from) to   = path->pitemListN-1;

  fmatrixResize( fmP, 0,         0);
  fmatrixResize( fmP, to-from+1, last-first+1);

  for ( i = from; i <= to; i++) {
    for ( j = 0; j < path->pitemListA[i].itemN; j++) {
      int stX  = path->pitemListA[i].itemA[j].phoneX;
      if (stX >= first || stX <= last)
          fmP->matPA[i-from][stX-first] += path->pitemListA[i].itemA[j].gamma;
    }
  }
  return TCL_OK;
}

static int pathPhoneMatrixItf( ClientData clientData, int argc, char *argv[])
{
  Path*    path  = (Path*)clientData;
  FMatrix* fmP   =  NULL;
  int      from  =  -1;
  int      to    =  -1;
  int      first =  -1;
  int      last  =  -1;
  int      ac    =  argc-1;

  if ( itfParseArgv( argv[0], &ac, argv+1, 0,
      "<FMatrix>", ARGV_OBJECT, NULL, &fmP, "FMatrix", "float matrix",
      "-from", ARGV_INT, NULL, &from, NULL,  "first frame of matrix",
      "-to",   ARGV_INT, NULL, &to,   NULL,  "last frame to include in matrix",
      "-first",ARGV_INT, NULL, &first,NULL,  "first phone index to include",
      "-last", ARGV_INT, NULL, &last, NULL,  "last phone index to include",
       NULL) != TCL_OK) return TCL_ERROR;

  return pathPhoneMatrix( path, first, last, from, to, fmP);
}

/* ------------------------------------------------------------------------
    pathWordMatrix  fill out a matrix with the cummulated gamma scores:
                    for each relative word index the is a row in the 
                    matrix which has number of frames columns.
   ------------------------------------------------------------------------ */

int pathWordMatrix( Path* path, int first, int last, int from, int to, 
                    FMatrix* fmP)
{
  int minStX = -1;
  int maxStX = -1;
  int i, j;

  assert(    path);

  if ( path->pitemListN < 0) {
    ERROR("Path structure is empty.\n");
    return TCL_ERROR;
  }

  for ( i = 0; i < path->pitemListN; i++) {
    for ( j = 0; j < path->pitemListA[i].itemN; j++) {
      if (minStX < 0 || minStX > path->pitemListA[i].itemA[j].wordX)
          minStX = path->pitemListA[i].itemA[j].wordX;
      if (maxStX < 0 || maxStX < path->pitemListA[i].itemA[j].wordX)
          maxStX = path->pitemListA[i].itemA[j].wordX;
    }
  }
  if ( minStX < 0) {
    ERROR("No word indices in the path structure %s.\n", path->name);
    return TCL_ERROR;
  }
  if ( first < 0 || first > maxStX) first = minStX;
  if ( last  < 0 || last  < first)  last  = maxStX;

  if ( from  < 0)    from = 0;
  if ( to    < from) to   = path->pitemListN-1;

  fmatrixResize( fmP, 0,         0);
  fmatrixResize( fmP, to-from+1, last-first+1);

  for ( i = from; i <= to; i++) {
    for ( j = 0; j < path->pitemListA[i].itemN; j++) {
      int stX  = path->pitemListA[i].itemA[j].wordX;
      if (stX >= first || stX <= last)
          fmP->matPA[i-from][stX-first] += path->pitemListA[i].itemA[j].gamma;
    }
  }
  return TCL_OK;
}

static int pathWordMatrixItf( ClientData clientData, int argc, char *argv[])
{
  Path*    path  = (Path*)clientData;
  FMatrix* fmP   =  NULL;
  int      from  =  -1;
  int      to    =  -1;
  int      first =  -1;
  int      last  =  -1;
  int      ac    =  argc-1;

  if ( itfParseArgv( argv[0], &ac, argv+1, 0,
      "<FMatrix>", ARGV_OBJECT, NULL, &fmP, "FMatrix", "float matrix",
      "-from", ARGV_INT, NULL, &from, NULL,  "first frame of matrix",
      "-to",   ARGV_INT, NULL, &to,   NULL,  "last frame to include in matrix",
      "-first",ARGV_INT, NULL, &first,NULL,  "first word index to include",
      "-last", ARGV_INT, NULL, &last, NULL,  "last word index to include",
       NULL) != TCL_OK) return TCL_ERROR;

  return pathWordMatrix( path, first, last, from, to, fmP);
}

/* ------------------------------------------------------------------------
    BLoad    load a binary dump of the path into the path object. This
             routine is able to read a compressed representation of the
             path object, where, if the same itemList of length 1 (ie.
             usually generated using viterbi) does occur for a number of
             subsequent frames the itemList is stored only once plus a
             repetition counter.
   ------------------------------------------------------------------------ */

static int _pathBLoad ( Path *paP, FILE* fp);

static int pathBLoad ( Path *paP, char *fileName) 
{
  FILE    *fp = NULL;
  if (! (fp=fileOpen( fileName,"r"))) return TCL_ERROR;
  if ( _pathBLoad(paP,fp) == TCL_ERROR ) {
    ERROR("Path file \"%s\" is corrupt.\n", fileName);
    fileClose( fileName, fp);
    return TCL_ERROR;
  }
  return fileClose( fileName, fp);
}

static int _pathBLoad ( Path *paP, FILE* fp)
{
  char     id[5];
  int      i, j;
  int      litemN;

  pathReset( paP);

  fread(id,sizeof(char),4,fp);

  if ( strncmp(id,"PATH",4)) {
    return TCL_ERROR;
  }

  paP->firstFrame = read_int(   fp); 
  paP->lastFrame  = read_int(   fp);
  paP->fromFrame  = read_int(   fp);
  paP->frameShift = read_int(   fp);
  paP->gscore     = read_float( fp);

  paP->pitemListN =  paP->lastFrame - paP->firstFrame + 1;
  paP->pitemListA = (PathItemList*)malloc( paP->pitemListN * 
                                           sizeof(PathItemList));

  /*don't read fp if path empty*/
  if (paP->lastFrame == -1) return TCL_OK;

  litemN = read_int( fp);

  for ( i = 0; i < paP->pitemListN; ) {
    int   itemN = litemN;
    int   stateX=0, senoneX=0, phoneX=0, wordX=0;
    float gamma = 0.0;

    pathItemListInit( paP->pitemListA+i);

    for ( j = 0; j < itemN; j++) {
      stateX  = read_int(fp);
      senoneX = read_int(fp); 
      phoneX  = read_int(fp);
      wordX   = read_int(fp);

      pathItemListAdd( paP->pitemListA+i, 1, stateX,senoneX, phoneX,wordX);

      paP->pitemListA[i].itemA[j].gamma = gamma = read_float(fp);
      if (feof(fp)) goto pathBLoadError; 
    }
    if ( ++i < paP->pitemListN) {
      litemN = read_int( fp);
      if (feof(fp)) goto pathBLoadError; 
      if ( litemN < 0) {
        for ( ; i < paP->pitemListN && litemN < 0; litemN++, i++) {
          pathItemListInit( paP->pitemListA+i);
          pathItemListAdd(  paP->pitemListA+i, 1, stateX, senoneX, 
                                                  phoneX, wordX);
          paP->pitemListA[i].itemA[0].gamma = gamma;
        }
        if ( i < paP->pitemListN) litemN = read_int( fp);
      }
    }
  }
  return TCL_OK;

  pathBLoadError: 
  return TCL_ERROR;
}

static int pathBLoadItf( ClientData cd, int argc, char *argv[])
{
  Path* P    = (Path*)cd;
  HMM*  hmm  =  NULL;
  char* name =  NULL;
  int   ac   =  argc - 1;

  if (itfParseArgv( argv[0], &ac, argv+1, 0,
    "<file>", ARGV_STRING, NULL, &name, cd,   "filename",
    "-hmm",   ARGV_OBJECT, NULL, &hmm, "HMM", "HMM object used for mapping",
     NULL) != TCL_OK) return TCL_ERROR;
 
  if ( pathBLoad(P,name) == TCL_OK) {
    itfAppendResult(" %e", P->gscore);
    if ( hmm) return pathMap( P, hmm, NULL, -1, 0);
    else      return TCL_OK;
  }
  else return TCL_ERROR;
}

/* ------------------------------------------------------------------------
    BSave     a binary and compressed representation of the path object
              is written to a file in machine independent format.
   ------------------------------------------------------------------------ */

static int _pathBSave ( Path *paP, FILE* fp);

static int pathBSave ( Path *paP, char *fileName) 
{
  FILE    *fp = NULL;
  if (! (fp=fileOpen( fileName,"w"))) return TCL_ERROR;
  if ( _pathBSave(paP,fp) == TCL_ERROR ) {
    ERROR("Can't bsave path file \"%s\".\n", fileName);
    fileClose( fileName, fp);
    return TCL_ERROR;
  }
  return fileClose( fileName, fp);
}

static int _pathBSave ( Path *paP, FILE* fp)
{
  int      i, j;

  if ( paP->pitemListN < 0) {
    ERROR("Path structure is empty.\n");
    return TCL_ERROR;
  }
  fwrite("PATH",sizeof(char),4,fp);

  write_int(   fp, paP->firstFrame); 
  write_int(   fp, paP->lastFrame);
  write_int(   fp, paP->fromFrame);
  write_int(   fp, paP->frameShift);
  write_float( fp, paP->gscore);

  for ( i = 0; i < paP->pitemListN; ) {
    int   itemN = paP->pitemListA[i].itemN;
    int   stateX=0, senoneX=0, phoneX=0, wordX=0;
    float gamma = 0.0;

    write_int(   fp, itemN);
    for ( j = 0; j < itemN; j++) {
      write_int(   fp, stateX  = paP->pitemListA[i].itemA[j].stateX); 
      write_int(   fp, senoneX = paP->pitemListA[i].itemA[j].senoneX); 
      write_int(   fp, phoneX  = paP->pitemListA[i].itemA[j].phoneX); 
      write_int(   fp, wordX   = paP->pitemListA[i].itemA[j].wordX); 
      write_float( fp, gamma   = paP->pitemListA[i].itemA[j].gamma); 
    }
    if ( itemN == 1) {
      int compress = 0;
      for ( i = i+1; i < paP->pitemListN; i++) {
        if ( paP->pitemListA[i].itemN != 1 ||
             stateX  != paP->pitemListA[i].itemA[0].stateX  ||
             senoneX != paP->pitemListA[i].itemA[0].senoneX ||
             phoneX  != paP->pitemListA[i].itemA[0].phoneX  ||
             wordX   != paP->pitemListA[i].itemA[0].wordX   ||
             gamma   != paP->pitemListA[i].itemA[0].gamma) break;
        else compress--;
      }
      if ( compress) write_int( fp, compress);
    }
    else i++;
  }
  return TCL_OK;
}

static int pathBSaveItf( ClientData cd, int argc, char *argv[])
{
  Path* P    = (Path*)cd;
  char* name =  NULL;
  int   ac   =  argc - 1;

  if (itfParseArgv( argv[0], &ac, argv+1, 0,
    "<file>", ARGV_STRING, NULL, &name, cd, "filename", NULL) != TCL_OK) 
     return TCL_ERROR;
 
  return pathBSave(P,name);
}

/* ----------------------------------------------------------------------
   pathCopy
   copies a path object
   ---------------------------------------------------------------------- */
int pathCopy(Path* from, Path* to) {
   int i, j;

   to->useN = from->useN;
   to->name = from->name;
   to->firstFrame = from->firstFrame;
   to->lastFrame = from->lastFrame;
   to->fromFrame = from->fromFrame;
   to->frameShift = from->frameShift;

   to->gscore = from->gscore;

   to->senoneMissPen  = from->senoneMissPen;
   to->phoneMissPen   = from->phoneMissPen;
   to->wordMissPen    = from->wordMissPen;

   to->pitemListN = from->pitemListN;
   to->pitemListA = (PathItemList*) malloc(to->pitemListN * sizeof(PathItemList));

   for (i=0; i < to->pitemListN; i++) {
      to->pitemListA[i].logScale = from->pitemListA[i].logScale;
      to->pitemListA[i].beam = from->pitemListA[i].beam;
      to->pitemListA[i].score = from->pitemListA[i].score;
      to->pitemListA[i].best = from->pitemListA[i].best;
      to->pitemListA[i].itemN = from->pitemListA[i].itemN;

      to->pitemListA[i].itemA = (PathItem*) malloc(to->pitemListA[i].itemN * sizeof(PathItem));

      for (j=0; j < to->pitemListA[i].itemN; j++) {
         to->pitemListA[i].itemA[j] = from->pitemListA[i].itemA[j];
      }
   }

   return TCL_OK;
}

static int pathCopyItf(ClientData clientData, int argc, char *argv[]) {
  int            ac = argc-1;
  Path           *to = (Path*)clientData;
  Path           *from =  0;

  if (itfParseArgv( argv[0], &ac, argv+1, 0,
      "path",     ARGV_OBJECT, NULL, &from, "Path", "source path object",
       NULL) != TCL_OK) return TCL_ERROR;

  return pathCopy(from, to);
}

/* ========================================================================
    Type Declaration 
   ======================================================================== */

static Method pathItemMethod[] = 
{
  { "puts", pathItemPutsItf,   NULL },
  {  NULL,  NULL, NULL } 
};

static TypeInfo pathItemInfo = { 
      "PathItem", 0, 0, pathItemMethod, 
       NULL, NULL, 
       pathItemConfigureItf, NULL, 
       itfTypeCntlDefaultNoLink,
      "PathItem\n" };

static Method pathItemListMethod[] = 
{
  { "puts",   pathItemListPutsItf,  NULL },
  { "add",    pathItemListAddItf,   "add items to the path list"},
  { "clear",  pathItemListClearItf, "remove all items from the path list"},
  {  NULL,  NULL, NULL } 
};

static TypeInfo pathItemListInfo = { 
      "PathItemList", 0, 0, pathItemListMethod, 
       NULL, NULL, 
       pathItemListConfigureItf, pathItemListAccessItf, 
       itfTypeCntlDefaultNoLink,
      "PathItemList\n" };


/* forward declaration */
extern int pathAlignGLatItf( ClientData clientData, int argc, char *argv[]);
extern int pathAlignGLatMPEItf( ClientData clientData, int argc, char *argv[]);
extern int pathAlignGLatbMMIEItf( ClientData clientData, int argc, char *argv[]);

Method pathMethod[] = 
{
  { "puts",        pathPutsItf,    "displays the contents of a path" },
  { "labels",      pathLabelsItf,  "displays the contents of a path as labels" },
  { "words",       pathWordLabelsItf, "displays the word/variant labels" },
  { "phones",      pathPhoneLabelsItf,"displays the phones labels" },
  { "reset",       pathResetItf,   "remove all items from a path" },
  { "make",        pathMakeItf,    "creates a path" },
  { "viterbi",     pathViterbiItf, "compute a Viterbi path for a given HMM"  },
  { "fwdBwd",      pathFwdBwdItf,  "compute a forward backward path for a HMM"  },
  { "alignGlat",   pathAlignGLatItf,  "compute forced alignment by Lattice constraint" },
  /*  { "alignGLatMPE",pathAlignGLatMPEItf, "compute and accumulate MPE statistics" },
  { "alignGLatbMMIE",pathAlignGLatbMMIEItf, "compute and accumulate bMMIE statistics" }, */
  { "pjkdmc",      pathPjkDMCItf,  "compute pjk-dmc for AFs" },
  { "lscore",      pathLScoreItf,  "compute the local scores"        },
  { "stateMatrix", pathStateMatrixItf, "matrix of state gamma scores" },
  { "senoneMatrix",pathSenoneMatrixItf,"matrix of senone gamma scores" },
  { "phoneMatrix", pathPhoneMatrixItf, "matrix of cum. phone gamma scores" },
  { "wordMatrix",  pathWordMatrixItf,  "matrix of cum. word gamma scores" },
  { "map",         pathMapItf,         "map senone indices"},
  { "bload",       pathBLoadItf,       "binary load of path items"},
  { "bsave",       pathBSaveItf,       "binary save of path items"},
  { "durentropy",  pathDurationalEntropyItf, "Compute durational entropy of a path"},
  { ":=",          pathCopyItf,        "copy path objects" },
  { NULL, NULL, NULL }
} ;

static TypeInfo pathInfo = { 
    "Path", 0, 0,      pathMethod, 
     pathCreateItf,    pathFreeItf, 
     pathConfigureItf, pathAccessItf, NULL,
    "A 'Path' object is filled by a forced alignment function and is used by training functions" };

static int pathInitialized = 0;


/* ========================================================================
    Labelbox
   ======================================================================== */

/* ------------------------------------------------------------------------
    Init Labelbox
   ------------------------------------------------------------------------ */

static void lboxInit( Labelbox* lboxP, char* name) 
{
  lboxP->name = strdup(name);
  lboxP->useN = 0;
  listInit((List*)&(lboxP->list), &pathInfo, sizeof(Path),20);

  lboxP->list.init    = (ListItemInit  *)pathInit;
  lboxP->list.deinit  = (ListItemDeinit*)pathDeinit;
}

Labelbox* lboxCreate (char* name) 
{
  Labelbox* lboxP= (Labelbox*) malloc(sizeof(Labelbox));
  assert(lboxP != NULL);
  lboxInit(lboxP,name);
  return lboxP;
}
  
static ClientData lboxCreateItf (ClientData cd, int argc, char *argv[])
{
  char *name = NULL;
  int     ac = argc;

  if ( itfParseArgv( argv[0], &ac, argv, 0,
      "<name>",  ARGV_STRING, NULL, &name, cd, "name of the labelbox",
       NULL) != TCL_OK) return NULL;

  return (ClientData)lboxCreate( name);
}


/* ------------------------------------------------------------------------
    Free Labelbox
   ------------------------------------------------------------------------ */


int lboxDeinit( Labelbox* lboxP) 
{
  if (lboxP->name) free(lboxP->name);
  return listDeinit((List*)&(lboxP->list));
}

int lboxFree( Labelbox* lboxP) 
{
  if ( lboxDeinit( lboxP)!=TCL_OK) return TCL_ERROR;
  if ( lboxP) free(lboxP);
  return TCL_OK;
}

static int lboxFreeItf (ClientData cd)
{
  return lboxFree((Labelbox*)cd);
}


/* ------------------------------------------------------------------------
    Clear Labelbox
   ------------------------------------------------------------------------ */


int lboxClear( Labelbox* lboxP) 
{
  return listClear((List*)&(lboxP->list));
}

static int lboxClearItf (ClientData cd, int argc, char *argv[])
{
  return lboxClear((Labelbox*)cd);
}

/* ------------------------------------------------------------------------
    Access Labelbox
   ------------------------------------------------------------------------ */

static ClientData lboxAccessItf( ClientData cd, char *name, TypeInfo **ti)
{ 
  Labelbox* lboxP  = (Labelbox*)cd;
  return listAccessItf((ClientData)&(lboxP->list),name,ti);
}

static int lboxPutsItf( ClientData cd, int argc, char *argv[])
{
  Labelbox* lboxP  = (Labelbox*)cd;
  return listPutsItf( (ClientData)&(lboxP->list), argc, argv);
}


/* ------------------------------------------------------------------------
    Add/Delete Labelbox items
   ------------------------------------------------------------------------ */

int lboxAdd (Labelbox* lboxP, char* name) 
{
  int  idx = listIndex((List*)&(lboxP->list), name, 0);
  if ( idx < 0) {
    idx = listNewItem( (List*)&(lboxP->list), (ClientData)name);
  }
  return idx;
}

static int lboxAddItf( ClientData cd, int argc, char *argv[])
{
  Labelbox* lboxP = (Labelbox*)cd;
  int          ac   =  argc - 1;
  char*        name =  NULL;
  //int          idx;

  if ( itfParseArgv( argv[0], &ac, argv+1, 0,
      "<name>", ARGV_STRING, NULL, &name,  NULL, "name of the Path",
      NULL) != TCL_OK) return TCL_ERROR;

  if (lboxAdd( lboxP,name) < 0)
      return TCL_ERROR; 
  else return TCL_OK;
}

static int lboxDeleteItf (ClientData cd, int argc, char *argv[]) 
{
  Labelbox* lboxP = (Labelbox*)cd;
  return listDeleteItf((ClientData)&(lboxP->list), argc, argv);
}


/* ------------------------------------------------------------------------
    load/save Labelbox
   ------------------------------------------------------------------------ */

int lboxSave (Labelbox* lboxP, char* fileName) 
{
  FILE  *fp = NULL;
  int pathX = 0;
  int pathN = lboxP->list.itemN;
 
  if (! (fp=fileOpen( fileName,"w"))) return TCL_ERROR;

  write_int(fp,pathN);
  for (pathX = 0; pathX < pathN; pathX++) {
    Path* p = lboxP->list.itemA + pathX;
    write_string(fp,p->name);
    _pathBSave(p,fp);
  }
  return fileClose( fileName, fp);
}


static int lboxSaveItf( ClientData cd, int argc, char *argv[])
{
  Labelbox* lblP    = (Labelbox*) cd;
  char* name =  NULL;
  int   ac   =  argc - 1;

  if (itfParseArgv( argv[0], &ac, argv+1, 0,
    "<file>", ARGV_STRING, NULL, &name, cd, "filename", NULL) != TCL_OK) 
     return TCL_ERROR;
 
  return lboxSave(lblP,name);
}


int lboxLoad (Labelbox* lboxP, char* fileName) 
{
  Path  *p  = NULL;
  FILE  *fp = NULL;
  int realX=0, pathX = 0, pathN=0;
  char l_name[256];

  if (! (fp=fileOpen( fileName,"r"))) return TCL_ERROR;

  pathN=read_int(fp);
  for (pathX = 0; pathX < pathN; pathX++) {
    l_name[0]='\0';
    read_string(fp,l_name); 
    realX=lboxAdd(lboxP,l_name);
    p = lboxP->list.itemA + realX;
    _pathBLoad(p,fp);
  }
  return fileClose( fileName, fp);
}


static int lboxLoadItf( ClientData cd, int argc, char *argv[])
{
  Labelbox* lblP    = (Labelbox*) cd;
  char* name =  NULL;
  int   ac   =  argc - 1;

  if (itfParseArgv( argv[0], &ac, argv+1, 0,
    "<file>", ARGV_STRING, NULL, &name, cd, "filename", NULL) != TCL_OK) 
     return TCL_ERROR;
 
  return lboxLoad(lblP,name);
}
  
/* ------------------------------------------------------------------------
   Type Info
   ------------------------------------------------------------------------ */

static Method lboxMethod[] = 
{ 
  { "puts",   lboxPutsItf,   "puts Labelbox" },
  { "add",    lboxAddItf,    "add a new path to the Labelbox" },
  { "delete", lboxDeleteItf, "remove a path from the Labelbox"  },
  { "load",   lboxLoadItf,   "load Labelbox" },
  { "save",   lboxSaveItf,   "save Labelbox" },
  { "clear",  lboxClearItf,  "clear Labelbox"},
  {  NULL,    NULL,          NULL } 
};

TypeInfo lboxInfo = { "Labelbox", 0, 0, lboxMethod, 
		      lboxCreateItf,     lboxFreeItf,
		      NULL,  lboxAccessItf, NULL,
		      "Labelbox\n" } ;

static int lboxInitialized = 0;

int Lbox_Init()
{
  if ( lboxInitialized) return TCL_OK;
  itfNewType( &lboxInfo);
  lboxInitialized = 1;
  return TCL_OK;
}

int Path_Init (void)
{
  if (! pathInitialized) {  
    itfNewType (&pathItemInfo);
    itfNewType (&pathItemListInfo);
    itfNewType (&pathInfo);
    
    if (Lbox_Init() != TCL_OK) return TCL_ERROR;

    pathInitialized = 1;
  }
  return TCL_OK;
}

