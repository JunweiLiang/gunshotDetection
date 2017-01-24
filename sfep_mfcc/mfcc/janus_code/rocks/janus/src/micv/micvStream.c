/* ========================================================================
    JANUS      Speech- to Speech Translation
               ------------------------------------------------------------
               Object: MicvStream
               ------------------------------------------------------------

    Author  :  Andreas Eller
    Module  :  micvStream.c

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
    Revision 1.1  2005/03/08 17:30:28  metze
    Initial code as of Andreas Eller


   ======================================================================== */

#include "micvStream.h"
#include "micvEM.h"

/* --- Module local datatypes and variables --- */

static MicvStream micvStreamDefaults;


/* ========================================================================
   Interface MicvStream
   ======================================================================== */

/* ------------------------------------------------------------------------
   micvStreamInit
   ------------------------------------------------------------------------ */
int micvStreamInit(MicvStream* pMicvStream, char* name, MicvSet* pMicvSet, Tree* pTree)
{
  if (pTree && pTree->mdsP.cd != (ClientData) pMicvSet) {
    ERROR("Tree '%s' is not defined over cbnewSet '%s'.\n", pTree->name, pMicvSet->name);
    return TCL_ERROR;
  }

  if (!(pMicvStream->name = strdup (name))) {
    ERROR("Out of memory!\n");
    return TCL_ERROR;
  }
  pMicvStream->useN = 0;

  pMicvStream->pMicvSet = pMicvSet;
  if (pMicvSet) link (pMicvStream->pMicvSet, "MicvSet");
  pMicvStream->pTree = pTree;
  if (pTree) link (pMicvStream->pTree, "Tree");

  return TCL_OK;
}

/* ------------------------------------------------------------------------
   micvStreamCreate
   ------------------------------------------------------------------------ */
MicvStream* micvStreamCreate(char* name, MicvSet* pMicvSet, Tree* pTree)
{
  MicvStream* pMicvStream;
  VERIFY_MALLOC(pMicvStream = (MicvStream*) malloc(sizeof(MicvStream)), return NULL);

  if(!pMicvStream || micvStreamInit(pMicvStream, name, pMicvSet, pTree) != TCL_OK) {
    if(pMicvStream) free (pMicvStream);
    ERROR("Failed to allocate distribution stream '%s'.\n", name);
    return NULL;
  }

  return pMicvStream;
}

/* ------------------------------------------------------------------------
   micvStreamDeinit
   ------------------------------------------------------------------------ */
int micvStreamDeinit(MicvStream* pMicvStream)
{
  if (pMicvStream->name)  {
    free (pMicvStream->name);
    pMicvStream->name = 0;
  }
  if (pMicvStream->pMicvSet) {
    unlink (pMicvStream->pMicvSet, "MicvSet");
	pMicvStream->pMicvSet = 0;
  }
  if (pMicvStream->pTree) {
    unlink(pMicvStream->pTree, "Tree");
    pMicvStream->pTree = 0;
  }

  return TCL_OK;
}

/* ------------------------------------------------------------------------
   micvStreamDestroy
   ------------------------------------------------------------------------ */
int micvStreamDestroy(MicvStream* pMicvStream)
{
  micvStreamDeinit(pMicvStream);
  if(pMicvStream) free(pMicvStream);

  return TCL_OK;
}

/* ------------------------------------------------------------------------
   micvStreamFree
   ------------------------------------------------------------------------ */
int micvStreamFree(MicvStream* pMicvStream)
{
  return micvStreamDestroy(pMicvStream);
}

/* ------------------------------------------------------------------------
   micvStreamTag
   Translates a senone tag name to an index that is to be used with
   'micvStreamGet'.
   ------------------------------------------------------------------------ */
int micvStreamTag(ClientData cd, char* name)
{
  MicvStream* pMicvStream = (MicvStream*) cd;

  if (pMicvStream->pTree)
    return listIndex ((List*) &(pMicvStream->pTree->list),(ClientData) name, 0);
  else return -1;
}

/* ------------------------------------------------------------------------
   micvStreamIndex
   ------------------------------------------------------------------------ */
int micvStreamIndex(ClientData cd, char* name)
{
  MicvStream* pMicvStream = (MicvStream*) cd;
  return micvsetIndex (pMicvStream->pMicvSet, name);
}

/* ------------------------------------------------------------------------
   micvStreamName
   ------------------------------------------------------------------------ */
char* micvStreamName(ClientData cd, int i)
{
  MicvStream* pMicvStream = (MicvStream*)cd;
  return micvsetName (pMicvStream->pMicvSet,i);
}

/* ------------------------------------------------------------------------
   micvStreamGet
   ------------------------------------------------------------------------ */
int micvStreamGet( ClientData cd, int tag, Word* wP, int left, int right)
{
  MicvStream* pMicvStream = (MicvStream*)cd;

  if (! pMicvStream->pTree) {
    ERROR("No distribtion tree defined for %s.\n", pMicvStream->name);
    return -1;
  }
  return treeGetModel( pMicvStream->pTree, tag, wP, left, right);
}

/* ------------------------------------------------------------------------
   micvStreamScore
   Compute the score of a given mixture of gaussians.
   ------------------------------------------------------------------------ */
int micvStreamScore(ClientData cd, int* idxA, float* scoreA, int idxN, int frameX)
{
	int i;
	MicvSet* pMicvSet = ((MicvStream*)cd)->pMicvSet;
	Micv* pMicv;

	for(i=0; i < idxN; i++)
	{
		pMicv = &pMicvSet->micvList.itemA[idxA[i]];
		scoreA[i] = pMicv->scoreFct(pMicv, frameX, NULL);
	}

	return TCL_OK;
}

/* ------------------------------------------------------------------------
   micvStreamAccu
   ------------------------------------------------------------------------ */

int micvStreamAccu(ClientData cd, int cbX, int frameX, float factor)
{
	return micv_accu(&((MicvStream*)cd)->pMicvSet->micvList.itemA[cbX], frameX, factor);
}

/* ------------------------------------------------------------------------
   micvStreamUpdate
   ------------------------------------------------------------------------ */
int micvStreamUpdate(ClientData cd)
{
	MicvSet* pMicvSet = ((MicvStream*)cd)->pMicvSet;
	return micvset_update(pMicvSet);
}

/* ------------------------------------------------------------------------
   micvStreamFrameN
   Runs over all features in the feature set assoc. with the codebook set
   and computes the minimum number of frames.
   The Micv classes allow (in principle) the use of different features
   within the same codebook set as long as the space dimensions are the
   same. No one ever uses this option, but we provide it for
   convenience.
   ------------------------------------------------------------------------ */
int micvStreamFrameN( ClientData cd, int *from, int *shift, int *ready) 
{
  register int i,min_dim;
  int temp;
  MicvSet* pMicvSet = ((MicvStream*)cd)->pMicvSet;
  FeatureSet* pFeatSet = pMicvSet->pFeatSet;

  if (pMicvSet->micvList.itemN > 0)
    temp = pMicvSet->micvList.itemA[0].nFeat;
  else
    temp = 0;
  min_dim = fesFrameN(pFeatSet, &temp, 1, from, shift, ready);
  if (pMicvSet->micvList.itemN == 0)
    return 0;
  else {
    for (i = 1; i < pMicvSet->micvList.itemN; i++) {
      if (pFeatSet->featA[pMicvSet->micvList.itemA[i].nFeat].data.fmat == 0) return 0;
      temp = pFeatSet->featA[pMicvSet->micvList.itemA[i].nFeat].data.fmat->m;
      if (temp < min_dim) min_dim = temp;
    }
  }

  return min_dim;
}

/* ------------------------------------------------------------------------
   micvStreamFesUsed
   ------------------------------------------------------------------------ */
FeatureSet* micvStreamFesUsed(ClientData cd)
{
  MicvSet* pMicvSet = ((MicvStream*)cd)->pMicvSet;
  return pMicvSet->pFeatSet;
}




/* --- Janus itf functions --- */

/* ------------------------------------------------------------------------
   micvStreamCreateItf
   ------------------------------------------------------------------------ */
ClientData micvStreamCreateItf(ClientData cd, int argc, char *argv[])
{
  MicvSet* pMicvSet = 0;
  Tree* pTree = 0;
  int ac = argc-1;

  if (itfParseArgv (argv[0],&ac,argv+1,1,
      "<micvSet>", ARGV_OBJECT, NULL, &pMicvSet, "MicvSet", "micv set",
      "<tree>",    ARGV_OBJECT, NULL, &pTree,    "Tree",    "model tree",
       NULL) != TCL_OK) return NULL;

  return (ClientData) micvStreamCreate (argv[0], pMicvSet, pTree);
}

/* ------------------------------------------------------------------------
   micvStreamDestroyItf
   ------------------------------------------------------------------------ */
int micvStreamDestroyItf(ClientData cd)
{
  micvStreamDestroy ((MicvStream*) cd);
  return TCL_OK;
}

/* ------------------------------------------------------------------------
   micvStreamFreeItf
   ------------------------------------------------------------------------ */
int micvStreamFreeItf(ClientData cd)
{
  return micvStreamDestroyItf (cd);
}

/* ------------------------------------------------------------------------
   micvStreamConfigureItf
   ------------------------------------------------------------------------ */
int micvStreamConfigureItf(ClientData cd, char *var, char *val)
{
  MicvStream* pMicvStream = (MicvStream*) cd;
  if (!pMicvStream) pMicvStream = &micvStreamDefaults;

  if (!var) {
    ITFCFG(micvStreamConfigureItf, cd, "name");
    ITFCFG(micvStreamConfigureItf, cd, "useN");
    ITFCFG(micvStreamConfigureItf, cd, "micvSet");
    ITFCFG(micvStreamConfigureItf, cd, "tree");
    return TCL_OK;
  }
  ITFCFG_CharPtr(var,val,"name",        pMicvStream->name,                    1);
  ITFCFG_Int(    var,val,"useN",        pMicvStream->useN,                    1);
  ITFCFG_Object( var,val,"micvSet",     pMicvStream->pMicvSet, name, MicvSet, 1);
  ITFCFG_Object( var,val,"tree",        pMicvStream->pTree,    name, Tree,    1);

  ERROR("Unknown option '-%s %s'.\n", var,  val ? val : "?");
  return TCL_ERROR;
}

/* ------------------------------------------------------------------------
   micvStreamAccessItf
   ------------------------------------------------------------------------ */
ClientData micvStreamAccessItf(ClientData cd, char *name, TypeInfo **ti)
{ 
  MicvStream* pMicvStream  = (MicvStream*)cd;

  if (*name == '.') { 
    if (name[1]=='\0') {
      itfAppendElement("micvSet");
      itfAppendElement("tree");
    } 
    else {
      if (!strcmp(name+1,"micvSet")) {
	*ti = itfGetType("MicvSet");
	return (ClientData)pMicvStream->pMicvSet;
      }
      else if (pMicvStream->pTree && !strcmp(name+1,"tree")) {
        *ti = itfGetType("Tree");
	return (ClientData)pMicvStream->pTree;
      }
    }
  }
  
  return (ClientData) micvsetAccessItf ((ClientData) &pMicvStream->pMicvSet, name, ti);
}

/* ------------------------------------------------------------------------
   micvStreamPutsItf
   ------------------------------------------------------------------------ */
int micvStreamPutsItf(ClientData cd, int argc, char *argv[])
{
	MicvSet* pMicvSet  = ((MicvStream*)cd)->pMicvSet;
	return micvsetPutsItf ((ClientData)pMicvSet, argc, argv);
}

/* ------------------------------------------------------------------------
   micvStreamIndexItf
   ------------------------------------------------------------------------ */
static int micvStreamIndexItf(ClientData cd, int argc, char *argv[])
{
  MicvSet* pMicvSet = ((MicvStream*)cd)->pMicvSet;
  return micvsetIndexItf ((ClientData) pMicvSet,argc,argv);
}

/* ------------------------------------------------------------------------
   micvStreamNameItf
   ------------------------------------------------------------------------ */
int micvStreamNameItf( ClientData cd, int argc, char *argv[])
{
  MicvSet* pMicvSet = ((MicvStream*)cd)->pMicvSet;
  return micvsetNameItf ((ClientData) pMicvSet, argc, argv);
}

/* ------------------------------------------------------------------------
   micvStreamGetItf
   ------------------------------------------------------------------------ */
int micvStreamGetItf( ClientData cd, int argc, char *argv[])
{
  MicvStream* pMicvStream = (MicvStream*)cd;

  if (! pMicvStream->pTree) {
    ERROR("No distribution tree defined for %s.\n", pMicvStream->name);
    return TCL_ERROR;
  }
  return treeGetModelItf((ClientData)(pMicvStream->pTree), argc, argv);
}

/* ------------------------------------------------------------------------
   micvStreamAccuItf
   ------------------------------------------------------------------------ */
int micvStreamAccuItf(ClientData cd, int argc, char *argv[])
{
	assert(0);
	return TCL_ERROR;
/*  MicvSet* pMicvSet = ((MicvStream*)cd)->pMicvSet;
  return micvsetAccuItf ((ClientData) pMicvSet, argc, argv);*/
}

/* ------------------------------------------------------------------------
   micvStreamUpdateItf
   ------------------------------------------------------------------------ */
int micvStreamUpdateItf( ClientData cd, int argc, char *argv[])
{
	assert(0);
	return TCL_ERROR;
/*  MicvSet* pMicvSet = ((MicvStream*)cd)->pMicvSet;
  return micvsetUpdateItf((ClientData)pMicvSet, argc, argv);*/
}


/* ========================================================================
    Type Declaration
   ======================================================================== */

static Method micvStreamMethod[] = { 
  { "puts",   micvStreamPutsItf,   NULL },
  { "index",  micvStreamIndexItf,  "returns indices of named codebooks" },
  { "name",   micvStreamNameItf,   "returns names of indexed codebooks" },
  { "get",    micvStreamGetItf,    "returns a codebook given a tagged phone sequence" },
  { "accu",   micvStreamAccuItf,   "accumulate sufficient statistic" },
  { "update", micvStreamUpdateItf, "update parameters" },
  {  NULL,    NULL,                NULL } 
};

TypeInfo micvStreamInfo = { 
        "MicvStream", 0,-1,      micvStreamMethod,
        micvStreamCreateItf,     micvStreamDestroyItf,
        micvStreamConfigureItf,  micvStreamAccessItf, NULL,
	    "Stream based on MIC codebooks (Micv)\n" };

static Stream micvStream = 
           { "MicvSet", &micvStreamInfo, 
              micvStreamTag,
              micvStreamGet,
              micvStreamIndex,
              micvStreamName,
              micvStreamScore,
              micvStreamAccu,
              micvStreamUpdate,
              micvStreamFrameN,
              micvStreamFesUsed };

static int micvStreamInitialized = 0;

int MicvStream_Init ( )
{
  if ( micvStreamInitialized) return TCL_OK;

  if ( MicvSet_Init() != TCL_OK) return TCL_ERROR;
  if ( Tree_Init()    != TCL_OK) return TCL_ERROR;

  micvStreamDefaults.useN         = 0;
  micvStreamDefaults.pMicvSet     = NULL;
  micvStreamDefaults.pTree        = NULL;

  streamNewType( &micvStream);
  itfNewType(    &micvStreamInfo);

  micvStreamInitialized = 1;
  return TCL_OK;
}

