/* ========================================================================
    JANUS      Speech- to Speech Translation
               ------------------------------------------------------------
               Object: SampleSet
               ------------------------------------------------------------

    Author  :  Ivica Rogina
    Module  :  SampleSet.c
    Date    :  $Id: sample.c 3416 2011-03-23 03:02:18Z metze $
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
    Revision 3.9  2006/10/24 16:15:09  fuegen
    bugfix for sample extraction for maximum likelihood based clustering

    Revision 3.8  2003/08/14 11:20:17  fuegen
    Merged changes on branch jtk-01-01-15-fms (jaguar -> ibis-013)

    Revision 3.7.4.6  2002/11/13 08:32:25  soltau
    removed file locking in flushing likelihoods
    changes according to new Lh arguments

    Revision 3.7.4.5  2002/11/05 09:01:52  metze
    Cleaned up after fileLocking

    Revision 3.7.4.4  2002/10/17 11:37:27  soltau
    sampleSetClassFlush : deactivated file locking

    Revision 3.7.4.3  2002/10/14 09:15:38  metze
    Avoided allocation problem

    Revision 3.7.4.2  2002/07/12 11:13:03  metze
    Extra NewLine in Error messages

    Revision 3.7.4.1  2001/10/18 13:08:38  metze
    Possibility to have different features for different sample sets

    Revision 3.4  2000/04/20 07:47:08  soltau
    allocate sample buffer on demand
    added file locking for flushing samples

    Revision 3.3  1997/07/16 11:38:01  rogina
    added likelihood-distance stuff

    Revision 3.2  1997/06/17 17:41:43  westphal
    *** empty log message ***

 * Revision 1.4  96/03/05  21:22:36  maier
 * Warning in sampleSetAccu instead segmentation fault
 * 
 * Revision 1.3  1996/01/19  21:21:01  rogina
 * added allCount (made modulus work)
 *
 * Revision 1.2  96/01/19  13:52:03  13:52:03  rogina (Ivica Rogina)
 * added maxCount
 * 
 * Revision 1.1  96/01/10  20:06:12  20:06:12  rogina (Ivica Rogina)
 * Initial revision
 * 

   ======================================================================== */

#include "common.h"
#include "itf.h"
#include "sample.h"
#include "path.h"
#include "distrib.h"
#include "lh.h"

char sampleRCSVersion[]= "@(#)1$Id: sample.c 3416 2011-03-23 03:02:18Z metze $";


/* ------------------------------------------------------------------------
    Forward Declarations
   ------------------------------------------------------------------------ */

extern TypeInfo sampleSetClassInfo;
extern TypeInfo sampleSetInfo;

static SampleSet      sampleSetDefault;

static float *dsVal = NULL;  /* These are static to avoid allocation      */
static float *cbVal = NULL;  /* and deallocation every time the functions */
static int   *lhRef = NULL;  /* that are using them are called.           */
static int   maxVal = 0;     /* maxVal is the size of the above arrays.   */

/* ========================================================================
    SampleSetClass
   ======================================================================== */
/* ------------------------------------------------------------------------
    sampleSetClassInit  initialize SampleSet class structure
   ------------------------------------------------------------------------ */

int sampleSetClassInit( SampleSetClass* cP, ClientData CD)
{
  assert(cP);
  cP->name     = strdup((char*)CD);
  cP->useN     = 0;
  cP->fileName = NULL;
  cP->buf      = NULL;
  cP->lh       = NULL;
  cP->modulus  = 1;
  cP->count    = 0;
  cP->sumCount = 0;
  cP->allCount = 0;
  cP->maxCount = -1;
  return TCL_OK;
}

SampleSetClass* sampleSetClassCreate( char* name)
{
  SampleSetClass* cP = (SampleSetClass*)malloc(sizeof(SampleSetClass));

  if ((!cP) || (sampleSetClassInit(cP, (ClientData)name) != TCL_OK)) {
    if (cP) free (cP);
    ERROR("Cannot create SampleSet class object %s.\n", name);
    return NULL;
  }
  return cP;
}

/* ------------------------------------------------------------------------
    sampleSetClassDeinit     deinitialize SampleSet class structure
   ------------------------------------------------------------------------ */

int sampleSetClassDeinit( SampleSetClass* cP)
{
  assert(cP);
  if (cP->name)     { free(cP->name);        cP->name     = NULL; }
  if (cP->fileName) { free(cP->fileName);    cP->fileName = NULL; }
  if (cP->buf)      { fmatrixFree(cP->buf);  cP->buf      = NULL; }
  return TCL_OK;
}

int sampleSetClassFree( SampleSetClass* cP)
{
  if ( sampleSetClassDeinit( cP) != TCL_OK) return TCL_ERROR;
  if ( cP) free( cP);
  return TCL_OK;
}

/* ------------------------------------------------------------------------
    sampleSetClassClear   reset the counters in a SampleSet class structure
   ------------------------------------------------------------------------ */

int sampleSetClassClear (SampleSetClass* cP)
{
  assert(cP);
  cP->count = cP->sumCount = cP->allCount = 0;
  return TCL_OK;
}

int sampleSetClassClearItf (ClientData clientData, int argc, char *argv[] )
{
  SampleSetClass* cP = (SampleSetClass*)clientData;
  int ac=argc-1;
  assert(cP);
  if (itfParseArgv(argv[0], &ac, argv+1, 0, NULL) != TCL_OK) return TCL_ERROR;  

  cP->count = cP->sumCount = cP->allCount = 0;
  return TCL_OK;
}

/* ------------------------------------------------------------------------
    sampleSetClassConfigureItf   configure members of a SampleSet class object 
   ------------------------------------------------------------------------ */

static int sampleSetClassConfigureItf(ClientData cd, char *var, char *val)
{
  SampleSetClass *cP = (SampleSetClass*)cd;
  if (! cP) return TCL_ERROR;
  if (! var) {
    ITFCFG(sampleSetClassConfigureItf,cd,"name");
    ITFCFG(sampleSetClassConfigureItf,cd,"useN");
    ITFCFG(sampleSetClassConfigureItf,cd,"featX");
    ITFCFG(sampleSetClassConfigureItf,cd,"modulus");
    ITFCFG(sampleSetClassConfigureItf,cd,"count");
    ITFCFG(sampleSetClassConfigureItf,cd,"sumCount");
    ITFCFG(sampleSetClassConfigureItf,cd,"allCount");
    ITFCFG(sampleSetClassConfigureItf,cd,"maxCount");
    ITFCFG(sampleSetClassConfigureItf,cd,"fileName");
    return TCL_OK;
  }
  ITFCFG_CharPtr(var,val,"name",     cP->name,     1);
  ITFCFG_Int(    var,val,"useN",     cP->useN,     1);
  ITFCFG_Int(    var,val,"featX",    cP->featX,    0);
  ITFCFG_Int(    var,val,"modulus",  cP->modulus,  0);
  ITFCFG_Int(    var,val,"count",    cP->count,    0);
  ITFCFG_Int(    var,val,"sumCount", cP->sumCount, 0);
  ITFCFG_Int(    var,val,"allCount", cP->allCount, 0);
  ITFCFG_Int(    var,val,"maxCount", cP->maxCount, 0);
  ITFCFG_CharPtr(var,val,"fileName", cP->fileName, 0);
  ERROR("unknown option '-%s %s'.\n", var, val ? val : "?");
  return TCL_ERROR;   
}

/* ------------------------------------------------------------------------
    sampleSetClassAccessItf    access typed subitems in a SampleSet class object
   ------------------------------------------------------------------------ */

static ClientData sampleSetClassAccessItf (ClientData cd, char *name, TypeInfo **ti)
{
  SampleSetClass* cP = (SampleSetClass*)cd;

  if ( *name == '.') {
    if ( name[1] =='\0') { 
      if (cP->buf) itfAppendResult( "buf");
      if (cP->lh)  itfAppendResult( "lh");
      *ti=NULL; return NULL; 
    }
    else {
      if (cP->buf && !strcmp( name+1, "buf")) {
	*ti = itfGetType("FMatrix");
	return (ClientData)cP->buf;
      }
      if (cP->lh && !strcmp( name+1, "lh")) {
	*ti = itfGetType("Lh");
	return (ClientData)cP->lh;
      }
      *ti=NULL; return NULL;
    }
  }
  return NULL;
}

/* ------------------------------------------------------------------------
    SampleSetClassPutsItf    print SampleSet class information to TCL string
   ------------------------------------------------------------------------ */

static int sampleSetClassPutsItf (ClientData cd, int argc, char *argv[])
{
  SampleSetClass* cP   = (SampleSetClass*)cd;
  itfAppendResult("%s %s ", cP->name, cP->fileName);
  itfAppendResult("%d %d %d %d %d", cP->modulus, cP->count, cP->sumCount, 
		  cP->allCount, cP->maxCount);
  return TCL_OK;
}


int sampleSetClassFlush (SampleSetClass *cP)
{
  if (!cP) { 
    ERROR("Null argument.\n"); 
    return TCL_ERROR; 
  }
  if (cP->count<=0) return TCL_OK;
  if (cP->buf) { 
    int size = cP->buf->m;
    if (cP->count != size) {
      fmatrixResize(cP->buf,cP->count,cP->buf->n);
    }
    fmatrixBAppend(cP->buf,cP->fileName);    
    if (cP->count != size) { 
      fmatrixResize(cP->buf,size,cP->buf->n); 
    }
    fmatrixFree(cP->buf);
    cP->buf= NULL;
  }
  if (cP->lh)  { 
    lhFlush(cP->lh,cP->fileName); 
  }

  cP->sumCount += cP->count;
  cP->count = 0;
  return TCL_OK;
}

int sampleSetClassFlushItf (ClientData cd, int argc, char *argv[])
{
  int ac=argc-1;
  if (itfParseArgv(argv[0], &ac, argv+1, 0, NULL) != TCL_OK) return TCL_ERROR;  
  return sampleSetClassFlush ((SampleSetClass*)cd); 
}

int sampleSetClassAccu (SampleSetClass *cP, float *frameP, float score)
{
  int dimX, ret = TCL_OK;
  int dimN;

  if (!cP || !frameP) { ERROR("Null argument.\n"); return TCL_ERROR; }

  dimN = cP->buf->n - 1;

  if ( ++cP->allCount % cP->modulus)  return ret;
  if ( (cP->sumCount+cP->count) > cP->maxCount && cP->maxCount>=0) return ret;


  for (dimX=0; dimX<dimN; dimX++)
    cP->buf->matPA [cP->count][dimX] = frameP[dimX];
  cP->buf->matPA [cP->count][dimN] = score;

  cP->count++;

  if (cP->count == cP->buf->m) ret = sampleSetClassFlush(cP);
  if (cP->sumCount+cP->count == cP->maxCount) ret = sampleSetClassFlush(cP);

  return ret;
}

/* ------------------------ */
/* sort log Gaussian values */
/* ------------------------ */
static int compareLhCbVal (const void *a, const void *b)
{
       if (cbVal[(int)(*((int*)a))] < cbVal[(int)(*((int*)b))]) return -1;
  else if (cbVal[(int)(*((int*)a))] > cbVal[(int)(*((int*)b))]) return  1;
  else                                                          return  0;
}

#define minRatio -6.9077553   /* i.e. log(1e-3) */

int sampleSetClassAccuCblh (SampleSetClass *cP, Codebook *cbP, 
			    float score, int modelX)
{
  int i, ret = TCL_OK;

  if ( ++cP->allCount % cP->modulus)  
    return TCL_OK;
  if ( (cP->sumCount+cP->count) > cP->maxCount && cP->maxCount>=0) 
    return TCL_OK;

  qsort(lhRef,cbP->refN,sizeof(int),compareLhCbVal);

  for (i=0; i<cbP->refN; i++) 
    if (cbVal[lhRef[0]] - cbVal[lhRef[i]] < minRatio) break;
  assert(i<cP->lh->gaussN);

  if (cP->lh->gaussX + i >= cP->lh->gaussN) 
     ret = sampleSetClassFlush(cP);

  cP->lh->valN[cP->lh->frameN] = i;
  cP->lh->modX[cP->lh->frameN] = modelX;

  for (i=0; i<cP->lh->valN[cP->lh->frameN]; i++) {
    cP->lh->valG[cP->lh->gaussX] = cbVal[lhRef[i]];
    cP->lh->valX[cP->lh->gaussX] = lhRef[i];
    cP->lh->gaussX++;
  }

  cP->lh->frameN++;
  cP->count++;
  if (cP->sumCount+cP->count == cP->maxCount) 
    ret = sampleSetClassFlush(cP);
  
  return ret;
}

/* ========================================================================
    SampleSet
   ======================================================================== */

/* ------------------------------------------------------------------------
    Create SampleSet Structure
   ------------------------------------------------------------------------ */

int sampleSetInit( SampleSet* sampleSetP, char* name, FeatureSet* fs, 
		   int featX, int dimN)
{
  sampleSetP->name        =  strdup(name);
  sampleSetP->useN        =  0;

  listInit((List*)&(sampleSetP->list), &sampleSetClassInfo, sizeof(SampleSetClass), 
                                  sampleSetDefault.list.blkSize);

  sampleSetP->list.init   = (ListItemInit  *)sampleSetClassInit;
  sampleSetP->list.deinit = (ListItemDeinit*)sampleSetClassDeinit;

  sampleSetP->fesP        =  fs;
  sampleSetP->featX       =  featX;
  sampleSetP->dimN        =  dimN;
  
  sampleSetP->indexA      =  NULL;
  sampleSetP->indexN      =  0;

  if (!sampleSetP->name) { return TCL_ERROR; }
  
  link(sampleSetP->fesP, "FeatureSet");
  return TCL_OK;
}

SampleSet* sampleSetCreate (char* name, FeatureSet *fs, int featX, int dimN)
{
  SampleSet* sampleSetP = (SampleSet*)malloc( sizeof(SampleSet));

  if (! sampleSetP || sampleSetInit( sampleSetP, name, fs, featX, dimN) != TCL_OK) {
    if ( sampleSetP) free( sampleSetP);
    ERROR("Failed to allocate SampleSet '%s'.\n", name); 
    return NULL;
  }
  return sampleSetP;
}

static ClientData sampleSetCreateItf (ClientData cd, int argc, char *argv[])
{
  FeatureSet* feat; 
  char*       name  = NULL; int ac=argc;
  int         dimN  = 0;
  char*       fname = NULL;

  if ( itfParseArgv( argv[0], &ac, argv, 0,
      "<name>",       ARGV_STRING, NULL, &name, NULL,         "name of the SampleSet object",
      "<featureSet>", ARGV_OBJECT, NULL, &feat, "FeatureSet", "name of the feature set",
      "<feature>",    ARGV_STRING, NULL, &fname, NULL,        "feature name",
      "<dimN>",       ARGV_INT,    NULL, &dimN,  NULL,        "input dimension",
       NULL) != TCL_OK) return NULL;

  return (ClientData)sampleSetCreate( name, feat, 
                                fesIndex(feat,fname,FE_FMATRIX,dimN,1), dimN);
}

/* ------------------------------------------------------------------------
    Free SampleSet Structure
   ------------------------------------------------------------------------ */

int sampleSetLinkN( SampleSet* sampleSetP)
{
  int  useN = listLinkN((List*)&(sampleSetP->list)) - 1;
  if ( useN < sampleSetP->useN) return sampleSetP->useN;
  else                    return useN;
}

int sampleSetDeinit (SampleSet* sampleSetP)
{
  if (! sampleSetP) { ERROR("Null argument.\n"); return TCL_ERROR; }

  if ( sampleSetLinkN(sampleSetP)) { 
    SERROR("SampleSet '%s' still in use by other objects.\n", sampleSetP->name);
    return TCL_ERROR;
  }
  if (sampleSetP->name)     { free(sampleSetP->name);        sampleSetP->name     = NULL; }

  /* TODO: possibly also unlock single feature index locks, here */

  unlink( sampleSetP->fesP, "FeatureSet");

  return listDeinit((List*)&(sampleSetP->list));
}

int sampleSetFree (SampleSet* sampleSetP)
{
  if ( sampleSetDeinit( sampleSetP)!=TCL_OK) return TCL_ERROR;
  if ( sampleSetP) free(sampleSetP);
  return TCL_OK;
}

static int sampleSetFreeItf (ClientData cd)
{
  return sampleSetFree((SampleSet*)cd);
}

/* ------------------------------------------------------------------------
    sampleSetConfigureItf   configure members of a SampleSet object
   ------------------------------------------------------------------------ */

static int sampleSetConfigureItf (ClientData cd, char *var, char *val)
{
  SampleSet* sampleSetP = (SampleSet*)cd;
  if (!sampleSetP)   sampleSetP = &sampleSetDefault;

  if (! var) {
    ITFCFG(sampleSetConfigureItf,cd,"name");
    ITFCFG(sampleSetConfigureItf,cd,"useN");
    ITFCFG(sampleSetConfigureItf,cd,"dimN");
    ITFCFG(sampleSetConfigureItf,cd,"featX");
    ITFCFG(sampleSetConfigureItf,cd,"featureSet");
    ITFCFG(sampleSetConfigureItf,cd,"indexN");
    return listConfigureItf( (ClientData)&(sampleSetP->list), var, val);
  }
  ITFCFG_CharPtr(var,val,"name",        sampleSetP->name,                 1);
  ITFCFG_Int    (var,val,"useN",        sampleSetP->useN,                 1);
  ITFCFG_Int    (var,val,"dimN",        sampleSetP->dimN,                 1);
  ITFCFG_Int    (var,val,"featX",       sampleSetP->featX,                1);
  ITFCFG_Object (var,val,"featureSet",  sampleSetP->fesP,name,FeatureSet, 1);
  ITFCFG_Int    (var,val,"indexN",      sampleSetP->indexN,               1);
  return listConfigureItf( (ClientData)&(sampleSetP->list), var, val);
}

/* ------------------------------------------------------------------------
    SampleSetAccessItf   access type subobjects of the SampleSet object
   ------------------------------------------------------------------------ */

static ClientData sampleSetAccessItf( ClientData cd, char* name, TypeInfo** ti)
{ 
  SampleSet* sampleSetP  = (SampleSet*)cd;

  if (*name == '.') { 
    if (name[1] == '\0') {
      if ( sampleSetP->fesP) itfAppendElement( "featureSet");
    }
    else {
      if (sampleSetP->fesP && !strcmp( name+1, "featureSet")) { 
        *ti = itfGetType("FeatureSet"); 
        return (ClientData)sampleSetP->fesP;
      }
    }
  }
  return listAccessItf((ClientData)&(sampleSetP->list),name,ti);
}

/* ------------------------------------------------------------------------
    sampleSetPutsItf
   ------------------------------------------------------------------------ */

static int sampleSetPutsItf( ClientData cd, int argc, char *argv[])
{
  SampleSet* sampleSetP  = (SampleSet*)cd;
  return listPutsItf( (ClientData)&(sampleSetP->list), argc, argv);
}

/* ------------------------------------------------------------------------
    SampleSetAdd     add a new class to the SampleSet object
   ------------------------------------------------------------------------ */

int sampleSetAdd (SampleSet* sampleSetP, char* name, char *fileName, 
		  int featX, int dimN, int size, int modulus, 
		  DistribSet *lh)
{
  int             idx = listNewItem( (List*)&(sampleSetP->list), (ClientData)name);
  SampleSetClass*  cP = sampleSetP->list.itemA + idx;

  cP->size=size;

  if (lh && cP->lh  == NULL) {
    cP->lh = lhCreate();
    if (! cP->lh) {
      ERROR("sampleSetAdd : couldn't create LH\n");
      return TCL_ERROR;
    }
    if (TCL_OK != lhAlloc(cP->lh,size))  {
      ERROR("sampleSetAdd : couldn't allocate LH with %d gaussians\n",size);
      return TCL_ERROR;
    }
  }

  cP->featX    = (featX < 0 || featX >= sampleSetP->fesP->featN) ? sampleSetP->featX : featX;
  cP->dimN     = (dimN  < 0 )                                    ? sampleSetP->dimN  : dimN;
  cP->fileName = strdup(fileName);
  cP->modulus  = modulus;
  return TCL_OK;
}

static int sampleSetAddItf( ClientData cd, int argc, char *argv[])
{
  SampleSet*  sampleSetP = (SampleSet*)cd;
  int         ac       =  argc - 1;
  char        *name    =  NULL, *fileName = NULL;
  int         modulus  = 1, size = 256, featX = -1, dimN = -1;
  DistribSet  *lh      = NULL;

  if (itfParseArgv (argv[0], &ac, argv+1, 0,
    "<name>",     ARGV_STRING, NULL, &name,     NULL,         "name of the class",
    "-filename",  ARGV_STRING, NULL, &fileName, NULL,         "name of the dump file",
    "-featX",     ARGV_INT,    NULL, &featX,    NULL,         "index of the feature to use",
    "-dimN",      ARGV_INT,    NULL, &dimN,     NULL,         "this feature's number of dimensions",
    "-size",      ARGV_INT,    NULL, &size,     NULL,         "use buffer of the given size",
    "-mod",       ARGV_INT,    NULL, &modulus,  NULL,         "use only every -mod-th vector",
    "-lhdss",     ARGV_OBJECT, NULL, &lh,       "DistribSet", "distrib set for likelihood accumulation",
    NULL) != TCL_OK) return TCL_ERROR;

  if (!fileName)
    fileName = name;

  if (sampleSetAdd (sampleSetP, name, fileName, featX, dimN, size, modulus, lh) < 0)
    return TCL_ERROR; 
  else return TCL_OK;
}

/* ------------------------------------------------------------------------
    SampleSetDeleteItf
   ------------------------------------------------------------------------ */

int sampleSetDeleteItf( ClientData cd, int argc, char *argv[])
{
  SampleSet* sampleSetP  = (SampleSet*)cd;
  return listDeleteItf((ClientData)&(sampleSetP->list), argc, argv);
}
/* ------------------------------------------------------------------------
    sampleSetIndex     given a name return the index of a SampleSet class
   ------------------------------------------------------------------------ */

int sampleSetIndex( SampleSet* sampleSetP,  char* name) 
{
  return listIndex((List*)&(sampleSetP->list), name, 0);
}

static int sampleSetIndexItf( ClientData clientData, int argc, char *argv[] )
{
  SampleSet* sampleSetP = (SampleSet*)clientData;
  return listName2IndexItf((ClientData)&(sampleSetP->list), argc, argv);
}

/* ------------------------------------------------------------------------
    sampleSetName      given the index return the name of a SampleSet class in 
                 the list
   ------------------------------------------------------------------------ */

char* sampleSetName( SampleSet* sampleSetP, int i) 
{
  return (i < 0) ? "(null)" : sampleSetP->list.itemA[i].name;
}

static int sampleSetNameItf( ClientData clientData, int argc, char *argv[] )
{
  SampleSet* sampleSetP = (SampleSet*)clientData;
  return listIndex2NameItf((ClientData)&(sampleSetP->list), argc, argv);
}

/* ------------------------------------------------------------------------
    sampleSetNumber   return the number of SampleSet classes
   ------------------------------------------------------------------------ */

int sampleSetNumber( SampleSet* sampleSetP)
{
    return sampleSetP->list.itemN;
}

/* ------------------------------------------------------------------------
    sampleSetMap  mapping routine for mapping arbitrary indices into SampleSet class
            indices. If the classX is defined it will be considered as a
            definition of the mapping otherwise it is handled as being
            a get function to the class index.
   ------------------------------------------------------------------------ */

int sampleSetMap( SampleSet* sampleSetP, int idx, int classX)
{
  if ( idx <  0) return -1;
  if ( idx >= sampleSetP->indexN) {
    if ( classX >= 0 && classX < sampleSetNumber( sampleSetP)) {
      sampleSetP->indexA = (int*)realloc( sampleSetP->indexA, (idx+1)*sizeof(int));
      for ( ; sampleSetP->indexN <= idx; (sampleSetP->indexN)++)
        sampleSetP->indexA[sampleSetP->indexN] = -1;
    }
    else return -1;
  }
  if ( classX >= 0 && classX < sampleSetNumber( sampleSetP)) sampleSetP->indexA[idx] = classX;
  return sampleSetP->indexA[idx];
}

static int sampleSetMapItf( ClientData cd, int argc, char *argv[])
{
  SampleSet*  sampleSetP   = (SampleSet*)cd;
  int   ac     =  argc - 1;
  int   idx    = -1;
  int   classX = -1;

  if (itfParseArgv( argv[0], &ac, argv+1, 0,
      "<index>",ARGV_INT,    NULL, &idx,    NULL,                "index to map",
      "-class", ARGV_LINDEX, NULL, &classX, &(sampleSetP->list), "name of the class",
      NULL) != TCL_OK) return TCL_ERROR;

  itfAppendResult("%d", sampleSetMap( sampleSetP, idx, classX));
  return TCL_OK;
}
int sampleSetShowMap (SampleSet* sampleSetP)
{
  int idx;
  assert (sampleSetP);

  for (idx=0; idx < sampleSetP->indexN; idx++)
  {  int classX = sampleSetP->indexA[idx];
     if (classX >= 0) itfAppendResult("%s ",(sampleSetP->list.itemA+classX)->name);
     else             itfAppendResult("(none) ");
  }
  return TCL_OK;
}

static int sampleSetShowMapItf( ClientData cd, int argc, char *argv[])
{
  SampleSet*  sampleSetP   = (SampleSet*)cd;
  int   ac     =  argc - 1;

  assert(sampleSetP);
  if (itfParseArgv(argv[0], &ac, argv+1, 0, NULL) != TCL_OK) return TCL_ERROR;  

  return sampleSetShowMap(sampleSetP);
}

/* ------------------------------------------------------------------------
    accu cblh   add all items in a path to the accumulator buffers,
                storing codebook likelihood values
   ------------------------------------------------------------------------ */

int sampleSetAccuCblh (SampleSet* sampleSetP, Path* path, 
		       DistribSet *dssP, float factor,int from,int to)
{
  int     classN = sampleSetNumber (sampleSetP);
  int     frX, i;
  double  scale, score;

  assert (sampleSetP && path);

  for (frX = from; frX < to; frX++) {
    int frameX = frX;
    
    for (i=0; i < path->pitemListA[frX].itemN; i++) {
      int   dsX    = path->pitemListA[frX].itemA[i].senoneX;
      int   classX = sampleSetMap (sampleSetP, dsX, -1);

      factor *= path->pitemListA[frX].itemA[i].gamma;

      if (classX >= 0 && classX < classN) {
        SampleSetClass *cP = sampleSetP->list.itemA + classX;
        int   cbX    = dssP->list.itemA[dsX].cbX;
        int   refN   = dssP->cbsP->list.itemA[cbX].refN;
	int   dimN   = cP->dimN;
        int   x;

        /* -------------------------------------------------------------- */
        /* if the refN of the codebook and the dimN of the *cP match then */
        /* continue work, possibly reallocating the used static buffers   */
        /* -------------------------------------------------------------- */

        if (refN != dimN) ERROR("refN = %d != dimN = %d\n",refN,dimN);
        if (refN > maxVal) {
	  dsVal = realloc(dsVal,refN*sizeof(float)); 
	  cbVal = realloc(cbVal,refN*sizeof(float)); 
	  lhRef = realloc(lhRef,refN*sizeof(int)); 
	  for (x=0; x<refN; x++) dsVal[x] = 0.0;
	  maxVal = refN;
	}

        /* -------------------------------------------------------------- */
        /* compute the score, using dsVal[*]==0, returning the gaussian   */
        /* contributions in cbVal (they are NOT normalized, but should be)*/
        /* -------------------------------------------------------------- */

        score = dssP->cbsP->score(dssP->cbsP, cbX, dsVal, frameX, cbVal);

        /* -------------------------------------------------------------- */
	/* multiply a scale factor to all contributions, such that the    */
        /* sum of all contributions adds up the the overall score         */
        /* -------------------------------------------------------------- */

        scale = 0.0;
	for (x=0; x<refN; x++) { lhRef[x] = x; scale += cbVal[x]; }
        scale = score + log(scale);
	for (x=0; x<refN; x++) 
	{   if (cbVal[x]<=0.0) cbVal[x] = 9.9e19; 
            else               cbVal[x] = scale - log(cbVal[x]); 
        }

        /* -------------------------------------------------------------- */
        /* now, in cbVal[x], we should have the exact -log of the x-th    */
        /* Gaussian value, we can call the actual accumulation function   */
        /* -------------------------------------------------------------- */

        sampleSetClassAccuCblh (cP,dssP->cbsP->list.itemA+cbX, score, dsX);
      }
    }
  }

  return TCL_OK;
}

/* ------------------------------------------------------------------------
    accu    add all items in a path to the accumulator buffer, 
            weighted by factor times gamma.
   ------------------------------------------------------------------------ */

int sampleSetAccu (SampleSet* sampleSetP, Path* path, float factor, int from , int to)
{
  int     classN = sampleSetNumber( sampleSetP);
  int     frameX, i;

  assert (sampleSetP && path);

  for (frameX = from; frameX < to ; frameX++) {
    for (i = 0; i < path->pitemListA[frameX].itemN; i++) {
      int   classX = sampleSetMap (sampleSetP, path->pitemListA[frameX].itemA[i].senoneX, -1);
      float score  = factor * path->pitemListA[frameX].itemA[i].gamma;

      if (classX >= 0 && classX < classN) {
        SampleSetClass*     cP = sampleSetP->list.itemA + classX;
	float*          frameP = fesFrame (sampleSetP->fesP, cP->featX, frameX);

	if (!frameP) {
	  WARN("sampleSetAccu: path but no frame #%d\n", frameX);
	  continue;
	}
	if (!cP->buf)
	  cP->buf = fmatrixCreate (cP->size, cP->dimN+1);
	if (!cP->buf) {
	  ERROR("allocation failure\n");
	  return TCL_ERROR;
	}
        sampleSetClassAccu (cP, frameP, score);
      }
    }
  }
  
  return TCL_OK;
}

static int sampleSetAccuItf (ClientData cd, int argc, char *argv[]) 
{ 
  SampleSet*  sampleSetP   = (SampleSet*)cd;
  DistribSet*  dssP        =  NULL;
  Path* path   =  NULL;
  float factor =  1.0;
  int   ac     =  argc-1;
  int from = -1;
  int   to = -1;

  if ( itfParseArgv( argv[0], &ac, argv+1, 0,
      "<path>",  ARGV_OBJECT, NULL, &path, "Path", "name of the path object",
      "-factor", ARGV_FLOAT,  NULL, &factor, 0,    "training factor",
      "-lh",     ARGV_OBJECT, NULL, &dssP, "DistribSet", "distribSet for lh accumulation",
      "-from",   ARGV_INT,    NULL, &from, 0,    "start frame",
      "-to",     ARGV_INT,    NULL, &to,   0,    "end   frame", 
      NULL) != TCL_OK) return TCL_ERROR;

  if (from < 0) from = path->firstFrame;
  if (to < 0)     to = path->lastFrame+1;

  if (dssP) return sampleSetAccuCblh (sampleSetP, path, dssP, factor, from, to);
   else     return sampleSetAccu     (sampleSetP, path,       factor, from, to);
}

/* ------------------------------------------------------------------------
    clear    clear buffers
   ------------------------------------------------------------------------ */

int sampleSetClear (SampleSet* sampleSetP)
{
  int     classX, classN = sampleSetNumber( sampleSetP);
  assert (sampleSetP);

  for (classX=0; classX<classN; classX++) 
    sampleSetClassClear(sampleSetP->list.itemA + classX);
  return TCL_OK;
}

int sampleSetClearItf (ClientData clientData, int argc, char *argv[] )
{
  SampleSet *sampleSetP = (SampleSet*)clientData;
  int ac=argc-1;
  assert(sampleSetP);
  if (itfParseArgv(argv[0], &ac, argv+1, 0, NULL) != TCL_OK) return TCL_ERROR;  

  return sampleSetClear(sampleSetP);
}

int sampleSetFlush (SampleSet *sampleSetP)
{
  int     classX, classN = sampleSetNumber( sampleSetP);
  assert (sampleSetP);

  for (classX=0; classX<classN; classX++) 
    sampleSetClassFlush(sampleSetP->list.itemA + classX);
  return TCL_OK;
}

static int sampleSetFlushItf (ClientData cd, int argc, char *argv[])
{ 
  SampleSet *sampleSetP = (SampleSet*)cd;
  int ac=argc-1;
  assert(sampleSetP);
  if (itfParseArgv(argv[0], &ac, argv+1, 0, NULL) != TCL_OK) return TCL_ERROR;  

  return sampleSetFlush (sampleSetP); 
}


/* ========================================================================
    Type Declaration
   ======================================================================== */

static Method sampleSetClassMethod[] = 
{ 
  { "puts",  sampleSetClassPutsItf,   NULL },
  { "clear", sampleSetClassClearItf,  "clear accumulation buffers" },
  { "flush", sampleSetClassFlushItf,  "flush accumulation buffer to file" },
  {  NULL,  NULL,             NULL } 
};

TypeInfo sampleSetClassInfo = { "SampleSetClass", 0, 0, sampleSetClassMethod, 
                           NULL, NULL,
                           sampleSetClassConfigureItf, sampleSetClassAccessItf, NULL,
	                  "a class in a SampleSet" } ;

static Method sampleSetMethod[] = 
{ 
  { "puts",   sampleSetPutsItf,    NULL },
  { "index",  sampleSetIndexItf,  "returns indices of named SampleSet classes" },
  { "name",   sampleSetNameItf,   "returns names of indexed SampleSet classes" },
  { "add",    sampleSetAddItf,    "add a new SampleSet class to the set" },
  { "delete", sampleSetDeleteItf, "remove SampleSet class from the set"  },
  { "map",    sampleSetMapItf,    "add/get index to class mapping information"  },
  { "showmap",sampleSetShowMapItf,"display class mapping information"  },
  { "accu",   sampleSetAccuItf,   "accumulate samples from a path object"  },
  { "clear",  sampleSetClearItf,  "clear accumulation buffers" },
  { "flush",  sampleSetFlushItf,  "flush accumulation buffers to file" },
  {  NULL,    NULL,          NULL } 
};

TypeInfo sampleSetInfo = { "SampleSet", 0, -1, sampleSetMethod, 
                      sampleSetCreateItf,    sampleSetFreeItf,
                      sampleSetConfigureItf, sampleSetAccessItf, NULL,
	             "containers for samples" } ;

static int sampleSetInitialized = 0;

int SampleSet_Init()
{
  if ( sampleSetInitialized) return TCL_OK;

  if (Lh_Init() == TCL_ERROR) return TCL_ERROR;

  sampleSetDefault.name         =  NULL;
  sampleSetDefault.useN         =  0;
  sampleSetDefault.dimN         =  0;
  sampleSetDefault.featX        = -1;
  sampleSetDefault.list.blkSize =  100;
  sampleSetDefault.fesP         =  NULL;

  itfNewType( &sampleSetClassInfo);
  itfNewType( &sampleSetInfo);

  sampleSetInitialized = 1;
  return TCL_OK;
}
