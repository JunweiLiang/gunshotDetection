/* ========================================================================
    JANUS      Speech- to Speech Translation
               ------------------------------------------------------------
               Object: DistribStream
               ------------------------------------------------------------

    Author  :  Michael Finke
    Module  :  distribStream.c
    Date    :  $Id: distribStream.c 2390 2003-08-14 11:20:32Z fuegen $
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
    Revision 3.5  2003/08/14 11:20:14  fuegen
    Merged changes on branch jtk-01-01-15-fms (jaguar -> ibis-013)

    Revision 3.4.20.1  2002/08/27 08:43:48  metze
    itfParseArgv now uses <name> and NULL initialization for char*

    Revision 3.4  2000/11/14 12:01:11  janus
    Merged branch jtk-00-10-27-jmcd.

    Revision 3.3.30.2  2000/11/08 17:07:47  janus
    Finished making changes required for compilation under 'gcc -Wall'.

    Revision 3.3.30.1  2000/11/06 10:50:26  janus
    Made changes to enable compilation under -Wall.

    Revision 3.3  1997/07/31 16:33:23  rogina
    made code gcc -DFKIclean

    Revision 3.2  1997/06/17 17:41:43  westphal
    *** empty log message ***

     Revision 1.2  96/04/23  19:54:11  maier
     RUNON, ready
     
     Revision 1.1  1996/01/31  05:15:33  finkem
     Initial revision


   ======================================================================== */

#include "common.h"
#include "itf.h"
#include "distrib.h"
#include "distribStream.h"

char distribStreamRCSVersion[]= 
           "@(#)1$Id: distribStream.c 2390 2003-08-14 11:20:32Z fuegen $";

/* ========================================================================
    DistribStream
   ======================================================================== */

static DistribStream distribStreamDefaults;

/* ------------------------------------------------------------------------
    Create Distribution Stream Structure
   ------------------------------------------------------------------------ */

int distribStreamInit( DistribStream* dstP, char* name, 
                       DistribSet*    dssP, Tree* treeP)
{
  if ( treeP && treeP->mdsP.cd != (ClientData)dssP) {
    ERROR( "Tree %s is not defined over distribSet %s.\n",
            treeP->name, dssP->name);
    return TCL_ERROR;
  }

  dstP->name = strdup(name);
  dstP->useN = 0;

  dstP->dssP  = dssP;  if (dssP)  link(dstP->dssP, "DistribSet");
  dstP->treeP = treeP; if (treeP) link(dstP->treeP,"Tree");

  return TCL_OK;
}

DistribStream* distribStreamCreate( char* name, DistribSet* dssP, 
                                                Tree*       treeP)
{
  DistribStream* dstP = (DistribStream*)malloc(sizeof(DistribStream));

  if (! dstP || distribStreamInit( dstP, name, dssP, treeP) != TCL_OK) {
    if ( dstP) free( dstP); 
    ERROR("Failed to allocate distribution stream '%s'.\n", name); 
    return NULL;
  }
  return dstP;
}

static ClientData distribStreamCreateItf(ClientData cd, int argc, char *argv[])
{
  DistribSet*  dssP  =  NULL;
  char         *name =  NULL; 
  Tree*        treeP =  NULL;
  int          ac    =  argc;

  if ( itfParseArgv( argv[0], &ac, argv, 1,
      "<name>",       ARGV_STRING, NULL, &name, NULL,         "name of the distrib stream",
      "<DistribSet>", ARGV_OBJECT, NULL, &dssP, "DistribSet", "set of distributions",
      "<Tree>",       ARGV_OBJECT, NULL, &treeP,"Tree",       "distribution tree",
       NULL) != TCL_OK) return NULL;

  return (ClientData) distribStreamCreate (name, dssP, treeP);
}

/* ------------------------------------------------------------------------
    Free DistribStream Structure
   ------------------------------------------------------------------------ */

static int distribStreamDeinit (DistribStream* dstP)
{
  if (dstP->name)  { free(  dstP->name);               dstP->name  = NULL; }
  if (dstP->dssP)  { unlink(dstP->dssP, "DistribSet"); dstP->dssP  = NULL; }
  if (dstP->treeP) { unlink(dstP->treeP,"Tree");       dstP->treeP = NULL; }
  return TCL_OK;
}

static void distribStreamFree (DistribStream* dstP)
{
  distribStreamDeinit( dstP);
  if ( dstP) free( dstP);
}

static int distribStreamFreeItf( ClientData cd )
{
  distribStreamFree((DistribStream*)cd);
  return TCL_OK;
}

/* ------------------------------------------------------------------------
    distribStreamConfigureItf
   ------------------------------------------------------------------------ */

static int distribStreamConfigureItf( ClientData cd, char *var, char *val)
{
  DistribStream* dstP = (DistribStream*)cd;
  if (! dstP)    dstP = &distribStreamDefaults;

  if (! var) {
    ITFCFG(distribStreamConfigureItf,cd,"name");
    ITFCFG(distribStreamConfigureItf,cd,"useN");
    ITFCFG(distribStreamConfigureItf,cd,"distribSet");
    ITFCFG(distribStreamConfigureItf,cd,"tree");
    return TCL_OK;
  }
  ITFCFG_CharPtr(var,val,"name",        dstP->name,                  1);
  ITFCFG_Int(    var,val,"useN",        dstP->useN,                  1);
  ITFCFG_Object( var,val,"distribSet",  dstP->dssP, name,DistribSet, 1);
  ITFCFG_Object( var,val,"tree",        dstP->treeP,name,Tree,       1);

  ERROR("Unknown option '-%s %s'.\n", var,  val ? val : "?");
  return TCL_ERROR;
}

/* ------------------------------------------------------------------------
    distribStreamAccessItf
   ------------------------------------------------------------------------ */

static ClientData distribStreamAccessItf( ClientData cd, char *name, 
                                          TypeInfo **ti)
{ 
  DistribStream* dstP  = (DistribStream*)cd;

  if (*name == '.') { 
    if (name[1]=='\0') {
      itfAppendElement("distribSet");
      itfAppendElement("tree");
    } 
    else {
      if (!strcmp(name+1,"distribSet")) {
	*ti = itfGetType("DistribSet");
	return (ClientData)dstP->dssP;
      }
      else if (dstP->treeP && !strcmp(name+1,"tree")) {
        *ti = itfGetType("Tree");
	return (ClientData)dstP->treeP;
      }
    }
  }
  return (ClientData)listAccessItf((ClientData)&(dstP->dssP->list),name,ti);
}

/* ------------------------------------------------------------------------
    distribStreamPutsItf
   ------------------------------------------------------------------------ */

static int distribStreamPutsItf( ClientData cd, int argc, char *argv[])
{
  DistribStream* dstP  = (DistribStream*)cd;
  return listPutsItf((ClientData)&(dstP->dssP->list), argc, argv);
}

/* ------------------------------------------------------------------------
    distribStreamTag   translates a senone tag name to an index that is
                       to be used with distribStreamGetModel
   ------------------------------------------------------------------------ */

static int distribStreamTag( ClientData cd, char* name)
{
  DistribStream* dstP = (DistribStream*)cd;

  if ( dstP->treeP)
       return listIndex((List*)&(dstP->treeP->list),(ClientData)name, 0);
  else return -1;
}

/* ------------------------------------------------------------------------
    distribStreamIndex
   ------------------------------------------------------------------------ */

int distribStreamIndex( ClientData cd, char* name) 
{
  DistribStream* dstP = (DistribStream*)cd;
  return dssIndex( dstP->dssP, name);
}

static int distribStreamIndexItf( ClientData cd, int argc, char *argv[])
{
  DistribSet* dss = ((DistribStream*)cd)->dssP;
  return listName2IndexItf((ClientData)&(dss->list), argc, argv);
}

/* ------------------------------------------------------------------------
    distribStreamName
   ------------------------------------------------------------------------ */

char* distribStreamName( ClientData cd, int i) 
{
  DistribStream* dstP = (DistribStream*)cd;
  return dssName( dstP->dssP, i);
}

static int distribStreamNameItf( ClientData clientData, int argc,char *argv[])
{
  DistribSet* dss = ((DistribStream*)clientData)->dssP;
  return listIndex2NameItf((ClientData)&(dss->list), argc, argv);
}

/* ------------------------------------------------------------------------
    distribStreamGet
   ------------------------------------------------------------------------ */

static int distribStreamGet( ClientData cd, int tag, Word* wP, int left, int right)
{
  DistribStream* dstP = (DistribStream*)cd;

  if (! dstP->treeP) {
    ERROR("No distribution tree defined for %s.\n", dstP->name);
    return -1;
  }
  return treeGetModel( dstP->treeP, tag, wP, left, right);
}

static int distribStreamGetItf( ClientData cd, int argc, char *argv[])
{
  DistribStream* dstP = (DistribStream*)cd;

  if (! dstP->treeP) {
    ERROR("No distribution tree defined for %s.\n", dstP->name);
    return TCL_ERROR;
  }
  return treeGetModelItf((ClientData)(dstP->treeP),argc,argv);
}

/* ------------------------------------------------------------------------
    distribStreamScore  compute the score of a given mixture of gaussians
   ------------------------------------------------------------------------ */

static int distribStreamScore( ClientData cd, int* idxA, float* scoreA,
                                       int  idxN, int    frameX)
{
  DistribSet* dssP = ((DistribStream*)cd)->dssP;
  return dssScoreArray( dssP, idxA, scoreA, idxN, frameX);
}

static int distribStreamScoreItf( ClientData cd, int argc, char *argv[])
{
  DistribSet* dssP = ((DistribStream*)cd)->dssP;
  return dssScoreItf((ClientData)dssP, argc, argv);
}

/* ------------------------------------------------------------------------
    distribStreamAccu  
   ------------------------------------------------------------------------ */

static int distribStreamAccu( ClientData cd, int dsX, int frameX, float factor)
{
  DistribSet* dssP = ((DistribStream*)cd)->dssP;
  return dssAccu( dssP, dsX, frameX, factor);
}

static int distribStreamAccuItf( ClientData cd, int argc, char *argv[])
{
  DistribSet* dssP = ((DistribStream*)cd)->dssP;
  return dssAccuItf((ClientData)dssP, argc, argv);
}

/* ------------------------------------------------------------------------
    distribStreamUpdate  
   ------------------------------------------------------------------------ */

static int distribStreamUpdate( ClientData cd)
{
  DistribSet* dssP = ((DistribStream*)cd)->dssP;
  return dssUpdate( dssP);
}

static int distribStreamUpdateItf( ClientData cd, int argc, char *argv[])
{
  DistribSet* dssP = ((DistribStream*)cd)->dssP;
  return dssUpdateItf((ClientData)dssP, argc, argv);
}

/* ------------------------------------------------------------------------
    distribStreamFrameN
   ------------------------------------------------------------------------ */

static int distribStreamFrameN( ClientData cd, int *from, int *shift, int *ready) 
{
  DistribSet* dssP = ((DistribStream*)cd)->dssP;
  return cbsFrameN(dssP->cbsP, from, shift, ready);
}

/* ------------------------------------------------------------------------
    distribStreamFesUsed
   ------------------------------------------------------------------------ */

static FeatureSet* distribStreamFesUsed ( ClientData cd)
{
  DistribSet* dssP = ((DistribStream*)cd)->dssP;
  return dssP->cbsP->feat;
}

/* ========================================================================
    Type Declaration
   ======================================================================== */

static Method distribStreamMethod[] = 
{ 
  { "puts",   distribStreamPutsItf,   NULL },
  { "index",  distribStreamIndexItf, "returns indices of named distributions" },
  { "name",   distribStreamNameItf,  "returns names of indexed distributions" },
  { "get",    distribStreamGetItf,   "returns a distribution given a tagged phone sequence" },
  { "score",  distribStreamScoreItf, "compute distribution score" },
  { "accu",   distribStreamAccuItf,  "accumulate sufficient statistic" },
  { "update", distribStreamUpdateItf,"update distributions/codebook" },
  {  NULL,    NULL,          NULL } 
};

TypeInfo distribStreamInfo = { 
        "DistribStream", 0,-1,      distribStreamMethod, 
         distribStreamCreateItf,    distribStreamFreeItf,
         distribStreamConfigureItf, distribStreamAccessItf, NULL,
	"Distribution based stream\n" };

static Stream distribStream = 
           { "DistribSet", &distribStreamInfo, 
              distribStreamTag,
              distribStreamGet,
              distribStreamIndex,
              distribStreamName,
              distribStreamScore,
              distribStreamAccu,
              distribStreamUpdate,
              distribStreamFrameN,
              distribStreamFesUsed };

static int distribStreamInitialized = 0;

extern int Tree_Init(void), Distrib_Init(void);

int DistribStream_Init (void)
{
  if ( distribStreamInitialized) return TCL_OK;

  if ( Distrib_Init() != TCL_OK) return TCL_ERROR;
  if ( Tree_Init()    != TCL_OK) return TCL_ERROR;

  distribStreamDefaults.useN         = 0;
  distribStreamDefaults.dssP         = NULL;
  distribStreamDefaults.treeP        = NULL;

  streamNewType( &distribStream);
  itfNewType(    &distribStreamInfo);

  distribStreamInitialized = 1;
  return TCL_OK;
}

