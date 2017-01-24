/* ========================================================================
    JANUS      Speech- to Speech Translation
               ------------------------------------------------------------
               Object: CBNewStream
               ------------------------------------------------------------

    Author  :  Matthias Seeger
    Module  :  cbnewStream.c
    Date    :  $Id: cbnewStream.c 699 2000-11-14 12:01:12Z janus $
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
   Revision 1.2  2000/11/14 12:01:10  janus
   Merged branch jtk-00-10-27-jmcd.

   Revision 1.1.30.1  2000/11/06 10:50:25  janus
   Made changes to enable compilation under -Wall.

   Revision 1.1  2000/08/16 09:12:49  soltau
   Initial revision


   ======================================================================== */

#include "common.h"
#include "itf.h"
#include "cbnewStream.h"

/* --- Module local datatypes and variables --- */

static CBNewStream cbnewStreamDefaults;


/* ========================================================================
   Interface CBNewStream
   ======================================================================== */

/* ------------------------------------------------------------------------
   cbnewStreamInit
   ------------------------------------------------------------------------ */
int cbnewStreamInit(CBNewStream* cstP,char* name,CBNewSet* cbsP,Tree* treeP)
{
  if (treeP && treeP->mdsP.cd != (ClientData) cbsP) {
    ERROR("Tree '%s' is not defined over cbnewSet '%s'.\n",
	  treeP->name,cbsP->name);
    return TCL_ERROR;
  }

  if (!(cstP->name = strdup (name))) {
    ERROR("Out of memory!\n");
    return TCL_ERROR;
  }
  cstP->useN = 0;

  cstP->cbsP = cbsP;
  if (cbsP) link (cstP->cbsP,"CBNewSet");
  cstP->treeP = treeP;
  if (treeP) link (cstP->treeP,"Tree");

  return TCL_OK;
}

/* ------------------------------------------------------------------------
   cbnewStreamCreate
   ------------------------------------------------------------------------ */
CBNewStream* cbnewStreamCreate (char* name,CBNewSet* cbsP,Tree* treeP)
{
  CBNewStream* cstP= (CBNewStream*) malloc (sizeof(CBNewStream));

  if (!cstP || cbnewStreamInit (cstP,name,cbsP,treeP) != TCL_OK) {
    if (cstP) free (cstP);
    ERROR("Failed to allocate distribution stream '%s'.\n", name);
    return NULL;
  }

  return cstP;
}

/* ------------------------------------------------------------------------
   cbnewStreamDeinit
   ------------------------------------------------------------------------ */
int cbnewStreamDeinit (CBNewStream* cstP)
{
  if (cstP->name)  {
    free (cstP->name);
    cstP->name = 0;
  }
  if (cstP->cbsP) {
    unlink (cstP->cbsP,"CBNewSet");
    cstP->cbsP = 0;
  }
  if (cstP->treeP) {
    unlink(cstP->treeP,"Tree");
    cstP->treeP = 0;
  }

  return TCL_OK;
}

/* ------------------------------------------------------------------------
   cbnewStreamDestroy
   ------------------------------------------------------------------------ */
int cbnewStreamDestroy (CBNewStream* cstP)
{
  cbnewStreamDeinit (cstP);
  if (cstP) free (cstP);

  return TCL_OK;
}

/* ------------------------------------------------------------------------
   cbnewStreamFree
   ------------------------------------------------------------------------ */
int cbnewStreamFree (CBNewStream* cstP)
{
  return cbnewStreamDestroy (cstP);
}

/* ------------------------------------------------------------------------
   cbnewStreamTag
   Translates a senone tag name to an index that is to be used with
   'cbnewStreamGet'.
   ------------------------------------------------------------------------ */
int cbnewStreamTag (ClientData cd,char* name)
{
  CBNewStream* cstP = (CBNewStream*) cd;

  if (cstP->treeP)
    return listIndex ((List*) &(cstP->treeP->list),(ClientData) name, 0);
  else return -1;
}

/* ------------------------------------------------------------------------
   cbnewStreamIndex
   ------------------------------------------------------------------------ */
int cbnewStreamIndex (ClientData cd,char* name)
{
  CBNewStream* cstP = (CBNewStream*) cd;
  return cbnewsetIndex (cstP->cbsP,name);
}

/* ------------------------------------------------------------------------
   cbnewStreamName
   ------------------------------------------------------------------------ */
char* cbnewStreamName (ClientData cd,int i)
{
  CBNewStream* cstP = (CBNewStream*)cd;
  return cbnewsetName (cstP->cbsP,i);
}

/* ------------------------------------------------------------------------
   cbnewStreamGet
   ------------------------------------------------------------------------ */
int cbnewStreamGet( ClientData cd, int tag, Word* wP, int left, int right)
{
  CBNewStream* cstP = (CBNewStream*)cd;

  if (! cstP->treeP) {
    ERROR("No distribtion tree defined for %s.\n", cstP->name);
    return -1;
  }
  return treeGetModel( cstP->treeP, tag, wP, left, right);
}

/* ------------------------------------------------------------------------
   cbnewStreamScore
   Compute the score of a given mixture of gaussians.
   ------------------------------------------------------------------------ */
int cbnewStreamScore (ClientData cd,int* idxA,float* scoreA,int idxN,
		      int frameX)
{
  CBNewSet* cbsP = ((CBNewStream*)cd)->cbsP;

  return cbnewsetScoreArray (cbsP,idxA,scoreA,idxN,frameX);
}

/* ------------------------------------------------------------------------
   cbnewStreamAccu
   ------------------------------------------------------------------------ */

/* changed return value from double to int. HAGEN */
int cbnewStreamAccu (ClientData cd,int cbX,int frameX,float factor)
{
  CBNewSet* cbsP = ((CBNewStream*)cd)->cbsP;

  return cbnewsetAccu (cbsP,cbX,frameX,factor);
}

/* ------------------------------------------------------------------------
   cbnewStreamUpdate
   ------------------------------------------------------------------------ */
int cbnewStreamUpdate (ClientData cd)
{
  CBNewSet* cbsP = ((CBNewStream*)cd)->cbsP;

  return cbnewsetUpdate (cbsP);
}

/* ------------------------------------------------------------------------
   cbnewStreamFrameN
   Runs over all features in the feature set assoc. with the codebook set
   and computes the minimum number of frames.
   The CBNew classes allow (in principle) the use of different features
   within the same codebook set as long as the space dimensions are the
   same. No one ever uses this option, but we provide it for
   convenience.
   ------------------------------------------------------------------------ */
int cbnewStreamFrameN( ClientData cd, int *from, int *shift, int *ready) 
{
  register int i,min_dim;
  int temp;
  CBNewSet* cbsP = ((CBNewStream*)cd)->cbsP;
  FeatureSet* fs = cbsP->feat_set;

  if (cbsP->list.itemN > 0)
    temp = cbsP->list.itemA[0].feat;
  else
    temp = 0;
  min_dim = fesFrameN (fs,&temp,1,from,shift,ready);
  if (cbsP->list.itemN == 0)
    return 0;
  else {
    for (i = 1; i < cbsP->list.itemN; i++) {
      if (fs->featA[cbsP->list.itemA[i].feat].data.fmat == 0) return 0;
      temp = fs->featA[cbsP->list.itemA[i].feat].data.fmat->m;
      if (temp < min_dim) min_dim = temp;
    }
  }

  return min_dim;
}

/* ------------------------------------------------------------------------
   cbnewStreamFesUsed
   ------------------------------------------------------------------------ */
FeatureSet* cbnewStreamFesUsed ( ClientData cd)
{
  CBNewSet* cbsP = ((CBNewStream*)cd)->cbsP;

  return cbsP->feat_set;
}

/* --- Janus itf functions --- */

/* ------------------------------------------------------------------------
   cbnewStreamCreateItf
   ------------------------------------------------------------------------ */
ClientData cbnewStreamCreateItf (ClientData cd,int argc,char *argv[])
{
  CBNewSet* cbsP = 0;
  Tree* treeP = 0;
  int ac = argc-1;

  if (itfParseArgv (argv[0],&ac,argv+1,1,
      "<cbnewSet>",ARGV_OBJECT,NULL,&cbsP, "CBNewSet", "codebook set",
      "<tree>",    ARGV_OBJECT,NULL,&treeP,"Tree",     "model tree",
       NULL) != TCL_OK) return NULL;

  return (ClientData) cbnewStreamCreate (argv[0],cbsP,treeP);
}

/* ------------------------------------------------------------------------
   cbnewStreamDestroyItf
   ------------------------------------------------------------------------ */
int cbnewStreamDestroyItf (ClientData cd)
{
  cbnewStreamDestroy ((CBNewStream*) cd);

  return TCL_OK;
}

/* ------------------------------------------------------------------------
   cbnewStreamFreeItf
   ------------------------------------------------------------------------ */
int cbnewStreamFreeItf (ClientData cd)
{
  return cbnewStreamDestroyItf (cd);
}

/* ------------------------------------------------------------------------
   cbnewStreamConfigureItf
   ------------------------------------------------------------------------ */
int cbnewStreamConfigureItf (ClientData cd,char *var,char *val)
{
  CBNewStream* cstP = (CBNewStream*) cd;
  if (!cstP) cstP = &cbnewStreamDefaults;

  if (!var) {
    ITFCFG(cbnewStreamConfigureItf,cd,"name");
    ITFCFG(cbnewStreamConfigureItf,cd,"useN");
    ITFCFG(cbnewStreamConfigureItf,cd,"cbnewSet");
    ITFCFG(cbnewStreamConfigureItf,cd,"tree");
    return TCL_OK;
  }
  ITFCFG_CharPtr(var,val,"name",        cstP->name,                  1);
  ITFCFG_Int(    var,val,"useN",        cstP->useN,                  1);
  ITFCFG_Object( var,val,"cbnewSet",    cstP->cbsP, name,CBNewSet,   1);
  ITFCFG_Object( var,val,"tree",        cstP->treeP,name,Tree,       1);

  ERROR("Unknown option '-%s %s'.\n", var,  val ? val : "?");
  return TCL_ERROR;
}

/* ------------------------------------------------------------------------
   cbnewStreamAccessItf
   ------------------------------------------------------------------------ */
ClientData cbnewStreamAccessItf (ClientData cd,char *name,TypeInfo **ti)
{ 
  CBNewStream* cstP  = (CBNewStream*)cd;

  if (*name == '.') { 
    if (name[1]=='\0') {
      itfAppendElement("cbnewSet");
      itfAppendElement("tree");
    } 
    else {
      if (!strcmp(name+1,"cbnewSet")) {
	*ti = itfGetType("CBNewSet");
	return (ClientData)cstP->cbsP;
      }
      else if (cstP->treeP && !strcmp(name+1,"tree")) {
        *ti = itfGetType("Tree");
	return (ClientData)cstP->treeP;
      }
    }
  }
  
  return (ClientData) cbnewsetAccessItf ((ClientData) &cstP->cbsP,name,ti);
}

/* ------------------------------------------------------------------------
   cbnewStreamPutsItf
   ------------------------------------------------------------------------ */
int cbnewStreamPutsItf (ClientData cd,int argc,char *argv[])
{
  CBNewSet* cbs  = ((CBNewStream*)cd)->cbsP;
  return cbnewsetPutsItf ((ClientData) cbs,argc,argv);
}

/* ------------------------------------------------------------------------
   cbnewStreamIndexItf
   ------------------------------------------------------------------------ */
static int cbnewStreamIndexItf (ClientData cd,int argc,char *argv[])
{
  CBNewSet* cbs = ((CBNewStream*)cd)->cbsP;
  return cbnewsetIndexItf ((ClientData) cbs,argc,argv);
}

/* ------------------------------------------------------------------------
   cbnewStreamNameItf
   ------------------------------------------------------------------------ */
int cbnewStreamNameItf( ClientData cd, int argc,char *argv[])
{
  CBNewSet* cbs = ((CBNewStream*)cd)->cbsP;
  return cbnewsetNameItf ((ClientData) cbs,argc,argv);
}

/* ------------------------------------------------------------------------
   cbnewStreamGetItf
   ------------------------------------------------------------------------ */
int cbnewStreamGetItf( ClientData cd, int argc, char *argv[])
{
  CBNewStream* cstP = (CBNewStream*)cd;

  if (! cstP->treeP) {
    ERROR("No distribution tree defined for %s.\n", cstP->name);
    return TCL_ERROR;
  }
  return treeGetModelItf((ClientData)(cstP->treeP),argc,argv);
}

/* ------------------------------------------------------------------------
   cbnewStreamAccuItf
   ------------------------------------------------------------------------ */
int cbnewStreamAccuItf (ClientData cd,int argc,char *argv[])
{
  CBNewSet* cbsP = ((CBNewStream*)cd)->cbsP;

  return cbnewsetAccuItf ((ClientData) cbsP,argc,argv);
}

/* ------------------------------------------------------------------------
   cbnewStreamUpdateItf
   ------------------------------------------------------------------------ */
int cbnewStreamUpdateItf( ClientData cd, int argc, char *argv[])
{
  CBNewSet* cbsP = ((CBNewStream*)cd)->cbsP;

  return cbnewsetUpdateItf((ClientData)cbsP, argc, argv);
}


/* ========================================================================
    Type Declaration
   ======================================================================== */

static Method cbnewStreamMethod[] = { 
  { "puts",   cbnewStreamPutsItf,   NULL },
  { "index",  cbnewStreamIndexItf,
    "returns indices of named codebooks" },
  { "name",   cbnewStreamNameItf,
    "returns names of indexed codebooks" },
  { "get",    cbnewStreamGetItf,
    "returns a codebook given a tagged phone sequence" },
  { "accu",   cbnewStreamAccuItf,  "accumulate sufficient statistic" },
  { "update", cbnewStreamUpdateItf,"update parameters" },
  {  NULL,    NULL,          NULL } 
};

TypeInfo cbnewStreamInfo = { 
        "CBNewStream", 0,-1,      cbnewStreamMethod,
         cbnewStreamCreateItf,    cbnewStreamDestroyItf,
         cbnewStreamConfigureItf, cbnewStreamAccessItf, NULL,
	"Stream based on extended codebooks (CBNew)\n" };

static Stream cbnewStream = 
           { "CBNewSet", &cbnewStreamInfo, 
              cbnewStreamTag,
              cbnewStreamGet,
              cbnewStreamIndex,
              cbnewStreamName,
              cbnewStreamScore,
              cbnewStreamAccu,
              cbnewStreamUpdate,
              cbnewStreamFrameN,
              cbnewStreamFesUsed };

static int cbnewStreamInitialized = 0;

int CBNewStream_Init ( )
{
  if ( cbnewStreamInitialized) return TCL_OK;

  if ( CBNewSet_Init() != TCL_OK) return TCL_ERROR;
  if ( Tree_Init()    != TCL_OK) return TCL_ERROR;

  cbnewStreamDefaults.useN         = 0;
  cbnewStreamDefaults.cbsP         = NULL;
  cbnewStreamDefaults.treeP        = NULL;

  streamNewType( &cbnewStream);
  itfNewType(    &cbnewStreamInfo);

  cbnewStreamInitialized = 1;
  return TCL_OK;
}
