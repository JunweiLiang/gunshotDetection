/* ========================================================================
    JANUS      Speech- to Speech Translation
               ------------------------------------------------------------
               Object: LDA
               ------------------------------------------------------------

    Author  :  Michael Finke & Martin Maier
    Module  :  lda.c
    Date    :  $Id: lda.c 3416 2011-03-23 03:02:18Z metze $
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
    Revision 3.5  2007/03/02 13:06:36  fuegen
    added code for PORTAUDIO support (Florian Metze, Friedrich Faubel)

    Revision 3.4  2003/08/14 11:19:56  fuegen
    Merged changes on branch jtk-01-01-15-fms (jaguar -> ibis-013)

    Revision 3.3.20.1  2002/02/28 18:54:58  soltau
    Added from/to in ldaAccu

    Revision 3.3  2000/11/14 12:29:35  janus
    Merging branch jtk-00-10-27-jmcd.

    Revision 3.2.30.2  2000/11/08 17:20:18  janus
    Finished making changes required for compilation under 'gcc -Wall'.

    Revision 3.2.30.1  2000/11/06 10:50:32  janus
    Made changes to enable compilation under -Wall.

    Revision 3.2  1997/06/17 17:41:43  westphal
    *** empty log message ***

 * Revision 1.6  96/02/03  17:12:54  finkem
 * made LDA a one pass procedure where means and scatter matrices
 * are computed in parallel. Use update to generate the final scatter
 * matrices.
 * 
 * Revision 1.5  1996/01/23  17:17:29  maier
 * checked frameP
 *
 * Revision 1.4  1996/01/08  17:51:56  maier
 * save and load means and scatters
 *
 * Revision 1.3  1995/12/15  17:42:19  finkem
 * removed bug in ldaMap (allocation wrong)
 *
 * Revision 1.2  1995/12/14  08:55:11  maier
 * *** empty log message ***
 *
 * Revision 1.1  1995/12/06  02:27:31  finkem
 * Initial revision
 *
 
   ======================================================================== */

#include "common.h"
#include "itf.h"
#include "lda.h"
#include "path.h"

char ldaRCSVersion[]= 
           "@(#)1$Id: lda.c 3416 2011-03-23 03:02:18Z metze $";


/* ------------------------------------------------------------------------
    Forward Declarations
   ------------------------------------------------------------------------ */

extern TypeInfo ldaClassInfo;
extern TypeInfo ldaInfo;

static LDA      ldaDefault;

/* ========================================================================
    LDAClass
   ======================================================================== */
/* ------------------------------------------------------------------------
    ldaClassInit  initialize LDA class structure
   ------------------------------------------------------------------------ */

int ldaClassInit( LDAClass* lda, ClientData CD)
{
  assert(lda);
  lda->name  = strdup((char*)CD);
  lda->useN  = 0;
  lda->mean  = NULL; /* we don't know the dimension */
  return TCL_OK;
}

LDAClass* ldaClassCreate( char* name)
{
  LDAClass* cP = (LDAClass*)malloc(sizeof(LDAClass));

  if ((!cP) || (ldaClassInit(cP, (ClientData)name) != TCL_OK)) {
    if (cP) free (cP);
    ERROR("Cannot create LDA class object %s.\n", name);
    return NULL;
  }
  return cP;
}

/* ------------------------------------------------------------------------
    ldaClassDeinit     deinitialize LDA class structure
   ------------------------------------------------------------------------ */

int ldaClassDeinit( LDAClass* cP)
{
  assert(cP);
  if (cP->name)   { free(cP->name);        cP->name = NULL; }
  if (cP->mean)   { dvectorFree(cP->mean); cP->mean = NULL; }
  return TCL_OK;
}

int ldaClassFree( LDAClass* cP)
{
  if ( ldaClassDeinit( cP) != TCL_OK) return TCL_ERROR;
  if ( cP) free( cP);
  return TCL_OK;
}

/* ------------------------------------------------------------------------
    ldaClassConfigureItf   configure members of a LDA class object 
   ------------------------------------------------------------------------ */

static int ldaClassConfigureItf(ClientData cd, char *var, char *val)
{
  LDAClass *cP = (LDAClass*)cd;
  if (! cP) return TCL_ERROR;
  if (! var) {
    ITFCFG(ldaClassConfigureItf,cd,"name");
    ITFCFG(ldaClassConfigureItf,cd,"useN");
    return TCL_OK;
  }
  ITFCFG_CharPtr(var,val,"name",  cP->name,  1);
  ITFCFG_Int(    var,val,"useN",  cP->useN,  1);
  ERROR("unknown option '-%s %s'.\n", var, val ? val : "?");
  return TCL_ERROR;   
}

/* ------------------------------------------------------------------------
    ldaClassAccessItf    access typed subitems in a LDA class object
   ------------------------------------------------------------------------ */

static ClientData ldaClassAccessItf (ClientData cd, char *name, TypeInfo **ti)
{
  LDAClass* cP = (LDAClass*)cd;

  if ( *name == '.') {
    if ( name[1] =='\0') { 
      if (cP->mean) itfAppendResult( "mean");
      *ti=NULL; return NULL; 
    }
    else {
      if (cP->mean && !strcmp( name+1, "mean")) {
	*ti = itfGetType("DVector");
	return (ClientData)cP->mean;
      }
      *ti=NULL; return NULL;
    }
  }
  return NULL;
}

/* ------------------------------------------------------------------------
    ldaClassPutsItf    print LDA class information to TCL string
   ------------------------------------------------------------------------ */

static int ldaClassPutsItf (ClientData cd, int argc, char *argv[])
{
  LDAClass* cP   = (LDAClass*)cd;
  itfAppendResult("%s {useN %d}", cP->name, cP->useN);
  return TCL_OK;
}

/* ========================================================================
    LDA
   ======================================================================== */
/* ------------------------------------------------------------------------
    Create LDA Structure
   ------------------------------------------------------------------------ */

int ldaInit( LDA* ldaP, char* name, FeatureSet* fs, int featX, int dimN)
{
  ldaP->name        =  strdup(name);
  ldaP->useN        =  0;

  listInit((List*)&(ldaP->list), &ldaClassInfo, sizeof(LDAClass), 
                                  ldaDefault.list.blkSize);

  ldaP->list.init   = (ListItemInit  *)ldaClassInit;
  ldaP->list.deinit = (ListItemDeinit*)ldaClassDeinit;
  ldaP->mean        =  dvectorCreate(dimN);
  ldaP->t           =  dmatrixCreate(dimN,dimN);
  ldaP->w           =  dmatrixCreate(dimN,dimN);
  ldaP->s           =  dmatrixCreate(dimN,dimN);

  ldaP->fesP        =  fs;
  ldaP->featX       =  featX;
  ldaP->dimN        =  dimN;
  
  ldaP->indexA      =  NULL;
  ldaP->indexN      =  0;

  if (!ldaP->name || !ldaP->mean || !ldaP->t || !ldaP->w) {
    /* free memory */
    if (ldaP->name) free (ldaP->name);
    if (ldaP->mean) dvectorFree(ldaP->mean);
    if (ldaP->t)    dmatrixFree(ldaP->t);
    if (ldaP->w)    dmatrixFree(ldaP->w);
    if (ldaP->s)    dmatrixFree(ldaP->s);
    return TCL_ERROR;
  }
  
  link(ldaP->fesP, "FeatureSet");
  return TCL_OK;
}

LDA* ldaCreate (char* name, FeatureSet *fs, int featX, int dimN)
{
  LDA* ldaP = (LDA*)malloc( sizeof(LDA));

  if (! ldaP || ldaInit( ldaP, name, fs, featX, dimN) != TCL_OK) {
    if ( ldaP) free( ldaP);
    ERROR("Failed to allocate LDA '%s'.\n", name); 
    return NULL;
  }
  return ldaP;
}

static ClientData ldaCreateItf (ClientData cd, int argc, char *argv[])
{
  FeatureSet* feat; 
  char*       name  = NULL; int ac=argc;
  int         dimN  = 0;
  char*       fname = NULL;

  if ( itfParseArgv( argv[0], &ac, argv, 0,
      "<name>",       ARGV_STRING, NULL, &name, NULL,         "name of the LDA object",
      "<featureSet>", ARGV_OBJECT, NULL, &feat, "FeatureSet", "name of the feature set",
      "<feature>",    ARGV_STRING, NULL, &fname, NULL,        "feature name",
      "<dimN>",       ARGV_INT,    NULL, &dimN,  NULL,        "input dimension",
       NULL) != TCL_OK) return NULL;

  return (ClientData)ldaCreate( name, feat, 
                                fesIndex(feat,fname,FE_FMATRIX,dimN,1), dimN);
}

/* ------------------------------------------------------------------------
    Free LDA Structure
   ------------------------------------------------------------------------ */

int ldaLinkN( LDA* ldaP)
{
  int  useN = listLinkN((List*)&(ldaP->list)) - 1;
  if ( useN < ldaP->useN) return ldaP->useN;
  else                    return useN;
}

int ldaDeinit (LDA* ldaP)
{
  if (! ldaP) { ERROR("null argument"); return TCL_ERROR; }

  if ( ldaLinkN(ldaP)) { 
    SERROR("LDA '%s' still in use by other objects.\n", ldaP->name);
    return TCL_ERROR;
  }
  if (ldaP->name) { free(ldaP->name);        ldaP->name  = NULL; }
  if (ldaP->mean) { dvectorFree(ldaP->mean); ldaP->mean  = NULL; }
  if (ldaP->t)    { dmatrixFree(ldaP->t);    ldaP->t     = NULL; }
  if (ldaP->w)    { dmatrixFree(ldaP->w);    ldaP->w     = NULL; }
  if (ldaP->s)    { dmatrixFree(ldaP->s);    ldaP->s     = NULL; }

  /* TODO: possibly also unlock single feature index locks, here */

  unlink( ldaP->fesP, "FeatureSet");

  return listDeinit((List*)&(ldaP->list));
}

int ldaFree (LDA* ldaP)
{
  if ( ldaDeinit( ldaP)!=TCL_OK) return TCL_ERROR;
  if ( ldaP) free(ldaP);
  return TCL_OK;
}

static int ldaFreeItf (ClientData cd)
{
  return ldaFree((LDA*)cd);
}

/* ------------------------------------------------------------------------
    ldaConfigureItf   configure members of a LDA object
   ------------------------------------------------------------------------ */

static int ldaConfigureItf (ClientData cd, char *var, char *val)
{
  LDA* ldaP = (LDA*)cd;
  if (!ldaP)   ldaP = &ldaDefault;

  if (! var) {
    ITFCFG(ldaConfigureItf,cd,"name");
    ITFCFG(ldaConfigureItf,cd,"useN");
    ITFCFG(ldaConfigureItf,cd,"dimN");
    ITFCFG(ldaConfigureItf,cd,"featX");
    ITFCFG(ldaConfigureItf,cd,"featureSet");
    ITFCFG(ldaConfigureItf,cd,"indexN");
    return listConfigureItf( (ClientData)&(ldaP->list), var, val);
  }
  ITFCFG_CharPtr(var,val,"name",        ldaP->name,                 1);
  ITFCFG_Int    (var,val,"useN",        ldaP->useN,                 1);
  ITFCFG_Int    (var,val,"dimN",        ldaP->dimN,                 1);
  ITFCFG_Int    (var,val,"featX",       ldaP->featX,                1);
  ITFCFG_Object (var,val,"featureSet",  ldaP->fesP,name,FeatureSet, 1);
  ITFCFG_Int    (var,val,"indexN",      ldaP->indexN,               1);
  return listConfigureItf( (ClientData)&(ldaP->list), var, val);
}

/* ------------------------------------------------------------------------
    ldaAccessItf   access type subobjects of the LDA object
   ------------------------------------------------------------------------ */

static ClientData ldaAccessItf( ClientData cd, char* name, TypeInfo** ti)
{ 
  LDA* ldaP  = (LDA*)cd;

  if (*name == '.') { 
    if (name[1] == '\0') {
      if ( ldaP->fesP) itfAppendElement( "featureSet");
      if ( ldaP->mean) itfAppendElement( "mean");
      if ( ldaP->s) itfAppendElement( "matrixS");
      if ( ldaP->t) itfAppendElement( "matrixT");
      if ( ldaP->w) itfAppendElement( "matrixW");
    }
    else {
      if (ldaP->fesP && !strcmp( name+1, "featureSet")) { 
        *ti = itfGetType("FeatureSet"); 
        return (ClientData)ldaP->fesP;
      }
      if (ldaP->mean && !strcmp( name+1, "mean")) {
	*ti = itfGetType("DVector");
	return (ClientData)ldaP->mean;
      }
      if (ldaP->s && !strcmp( name+1, "matrixS")) { 
        *ti = itfGetType("DMatrix"); 
        return (ClientData)ldaP->s;
      }
      if (ldaP->t && !strcmp( name+1, "matrixT")) { 
        *ti = itfGetType("DMatrix"); 
        return (ClientData)ldaP->t;
      }
      if (ldaP->w && !strcmp( name+1, "matrixW")) { 
        *ti = itfGetType("DMatrix"); 
        return (ClientData)ldaP->w;
      }
    }
  }
  return listAccessItf((ClientData)&(ldaP->list),name,ti);
}

/* ------------------------------------------------------------------------
    ldaPutsItf
   ------------------------------------------------------------------------ */

static int ldaPutsItf( ClientData cd, int argc, char *argv[])
{
  LDA* ldaP  = (LDA*)cd;
  return listPutsItf( (ClientData)&(ldaP->list), argc, argv);
}

/* ------------------------------------------------------------------------
    ldaAdd     add a new class to the LDA object
   ------------------------------------------------------------------------ */

int ldaAdd (LDA* ldaP, char* name)
{
  int       idx = listIndex((List*)&(ldaP->list), (ClientData)name,0);
  LDAClass* cP  = NULL;

  if ( idx > -1) return idx;

  idx = listNewItem( (List*)&(ldaP->list), (ClientData)name);
  cP  = ldaP->list.itemA + idx;

  if ( cP->mean == NULL &&
      (cP->mean =  dvectorCreate(ldaP->dimN)) == NULL) return -1;
  return idx;
}

static int ldaAddItf( ClientData cd, int argc, char *argv[])
{
  LDA*  ldaP = (LDA*)cd;
  int   ac   =  argc - 1;
  char* name =  NULL;
  //int   idx;

  if ( itfParseArgv( argv[0], &ac, argv+1, 0,
       "<name>", ARGV_STRING, NULL, &name,  NULL, "name of the class",
       NULL) != TCL_OK) return TCL_ERROR;

  if (ldaAdd( ldaP, name) < 0) 
       return TCL_ERROR; 
  else return TCL_OK;
}

/* ------------------------------------------------------------------------
    ldaDeleteItf
   ------------------------------------------------------------------------ */

int ldaDeleteItf( ClientData cd, int argc, char *argv[])
{
  LDA* ldaP  = (LDA*)cd;
  return listDeleteItf((ClientData)&(ldaP->list), argc, argv);
}

/* ------------------------------------------------------------------------
    ldaIndex     given a name return the index of an LDA class
   ------------------------------------------------------------------------ */

int ldaIndex( LDA* lda,  char* name) 
{
  return listIndex((List*)&(lda->list), name, 0);
}

static int ldaIndexItf( ClientData clientData, int argc, char *argv[] )
{
  LDA* lda = (LDA*)clientData;
  return listName2IndexItf((ClientData)&(lda->list), argc, argv);
}

/* ------------------------------------------------------------------------
    ldaName      given the index return the name of an LDA class in 
                 the list
   ------------------------------------------------------------------------ */

char* ldaName( LDA* lda, int i) 
{
  return (i < 0) ? "(null)" : lda->list.itemA[i].name;
}

static int ldaNameItf( ClientData clientData, int argc, char *argv[] )
{
  LDA* lda = (LDA*)clientData;
  return listIndex2NameItf((ClientData)&(lda->list), argc, argv);
}

/* ------------------------------------------------------------------------
    ldaNumber   return the number of LDA classes
   ------------------------------------------------------------------------ */

int ldaNumber( LDA* lda)
{
    return lda->list.itemN;
}

/* ------------------------------------------------------------------------
    ldaMap  mapping routine for mapping arbitrary indices into LDA class
            indices. If the classX is defined it will be considered as a
            definition of the mapping otherwise it is handled as being
            a get function to the class index.
   ------------------------------------------------------------------------ */

int ldaMap( LDA* ldaP, int idx, int classX)
{
  if ( idx <  0) return -1;
  if ( idx >= ldaP->indexN) {
    if ( classX >= 0 && classX < ldaNumber( ldaP)) {
      ldaP->indexA = (int*)realloc( ldaP->indexA, (idx+1)*sizeof(int));
      for ( ; ldaP->indexN <= idx; (ldaP->indexN)++)
        ldaP->indexA[ldaP->indexN] = -1;
    }
    else return -1;
  }
  if ( classX >= 0 && classX < ldaNumber( ldaP)) ldaP->indexA[idx] = classX;
  return ldaP->indexA[idx];
}

static int ldaMapItf( ClientData cd, int argc, char *argv[])
{
  LDA*  ldaP   = (LDA*)cd;
  int   ac     =  argc - 1;
  int   idx    = -1;
  int   classX = -1;

  if ( itfParseArgv( argv[0], &ac, argv+1, 0,
      "<index>",ARGV_INT,    NULL, &idx, NULL, "index to map",
      "-class", ARGV_LINDEX, NULL, &classX,&(ldaP->list), 
      "name of the class", NULL) != TCL_OK) return TCL_ERROR;

  itfAppendResult("%d", ldaMap( ldaP, idx, classX));
  return TCL_OK;
}

/* ------------------------------------------------------------------------
    ldaAccu    add all items in a path to the accumulator weighted by
               factor times gamma.
   ------------------------------------------------------------------------ */

static int ldaAccu( LDA* ldaP, Path* path, double factor, int from, int to)
{
  int     classN = ldaNumber( ldaP);
  int     dimN;
  DMatrix *s;
  DVector *mean;
  int     frX, i, j, k;

  assert( ldaP && path);
  dimN = ldaP->dimN;
  s    = ldaP->s;
  mean = ldaP->mean;

  /* ------------------------------------------------------
     Check matrices and then multiply all of them with
     count so we can use them as accumulators
     ------------------------------------------------------ */
  if (!mean) {
    ERROR("LDA mean vector doesn't exist\n"); return TCL_ERROR;
  }
  if (mean->n != dimN) {
    ERROR("Dimension of LDA mean vector %d is not %d\n",mean->n, dimN);
    return TCL_ERROR;
  }    
  if (!s) {
    ERROR("LDA scatter matrices don't exist\n"); return TCL_ERROR;
  }
  if (s->m != dimN || s->n != dimN) {
    ERROR("Dimension of LDA scatter matrix s(%dx%d) are not %d\n",
	  s->m, s->n, dimN);
    return TCL_ERROR;
  }    

  /* ------------------------------------------------------
     Accumulate samples according to path score
     ------------------------------------------------------ */

  if (from < 0) from = 0;
  if (to  < 0)  to   = path->lastFrame-path->firstFrame+1;

  for ( frX = from; frX < to; frX++) {
    int    frameX = frX+path->firstFrame;
    float *frameP = fesFrame( ldaP->fesP, ldaP->featX, frameX);
    double count  = 0.0;
    
    if (frameX < 0 || !frameP) {
       WARN("Path exists but no frame #%d of feature %s\n",
	     frameX, ldaP->fesP->featA[ldaP->featX].name);
       continue;
    }

    for ( i = 0; i < path->pitemListA[frX].itemN; i++) {
      int    classX = ldaMap(  ldaP, path->pitemListA[frX].itemA[i].senoneX, -1);
      double score  = factor * path->pitemListA[frX].itemA[i].gamma;

      if ( classX >= 0 && classX < classN) {
        LDAClass* cP    = ldaP->list.itemA + classX;
	DVector*  cmean = cP->mean;

	/* does class mean vector exist? */
	if ( cmean == NULL ) {
	  WARN("Have no mean for class %s\n"
	       "Will omit this class\n",cP->name);
	}
	else if (cmean->n != dimN){
	  ERROR("Dimension of mean vector %d for lda class %s is not %d\n",
		 cmean->n, cP->name, dimN);
	  return TCL_ERROR;
	}
	else {
	  for ( j=0; j<dimN; j++) cmean->vecA[j] += score * frameP[j];

          cmean->count += score;
          count        += score;
	}
      }
    }
    for ( j = 0; j<dimN; j++)
      for ( k=0; k<=j; k++) s->matPA[j][k] += count * frameP[j] * frameP[k];

    s->count += count;
  }
  return TCL_OK;
}

static int ldaAccuItf (ClientData cd, int argc, char *argv[]) 
{ 
  LDA*  ldaP   = (LDA*)cd;
  Path* path   =  NULL;
  float factor =  1.0;
  int   ac     =  argc-1;
  int     from = -1;
  int       to = -1;

  if ( itfParseArgv( argv[0], &ac, argv+1, 0,
      "<path>",  ARGV_OBJECT, NULL, &path, "Path", "name of the path object",
      "-factor", ARGV_FLOAT,  NULL, &factor, 0,    "training factor",
      "-from",   ARGV_INT,    NULL, &from,   0,    "from frameX",
      "-to",     ARGV_INT,    NULL, &to,     0,    "to   frameX",
       NULL) != TCL_OK) return TCL_ERROR;

  return ldaAccu( ldaP, path, factor,from,to);
}

/* ------------------------------------------------------------------------
    ldaUpdate  update scatter matrices
   ------------------------------------------------------------------------ */

int ldaUpdate( LDA* ldaP)
{
  int      classN = ldaNumber( ldaP);
  int      dimN   = ldaP->dimN;
  DVector* m      = ldaP->mean;
  DMatrix* t      = ldaP->t;
  DMatrix* w      = ldaP->w;
  DMatrix* s      = ldaP->s;
  int      classX,i,j;

  dvectorClear(m);

  for ( classX=0; classX<classN; classX++) {
    LDAClass* cP    = ldaP->list.itemA + classX;

    if (cP->mean) {
      for ( i = 0; i < dimN; i++) m->vecA[i] += cP->mean->vecA[i];
      m->count += cP->mean->count;
    }
  }

  if ( m->count > 0 && s->count > 0.0) {

    for ( i = 0; i < dimN; i++)
      for ( j = 0; j <= i; j++) {
        t->matPA[i][j] = s->matPA[i][j] - m->vecA[i] * 
                                          m->vecA[j] / m->count;
        w->matPA[i][j] = s->matPA[i][j];
      }

    w->count = s->count;
    t->count = s->count;

    for ( classX=0; classX<classN; classX++) {
      LDAClass* cP = ldaP->list.itemA + classX;

      assert (cP->mean);
      if (cP->mean->count > 0) {
        for ( i = 0; i < dimN; i++)
          for ( j = 0; j <= i; j++)
            w->matPA[i][j] -= cP->mean->vecA[i] *
                              cP->mean->vecA[j] / cP->mean->count;
      }
    }

    for ( i = 0; i < dimN; i++)
      for ( j = 0; j <=i; j++) {
        w->matPA[j][i] = (w->matPA[i][j] /= w->count);
        t->matPA[j][i] = (t->matPA[i][j] /= t->count);
      }
  }
  return TCL_OK;
}

static int ldaUpdateItf (ClientData cd, int argc, char *argv[]) 
{ 
  LDA*  ldaP   = (LDA*)cd;
  int   ac     =  argc-1;

  if ( itfParseArgv( argv[0], &ac, argv+1, 0,
       NULL) != TCL_OK) return TCL_ERROR;

  return ldaUpdate( ldaP);
}

/* ------------------------------------------------------------------------
    clear    clear mean/squared mean accumulators
   ------------------------------------------------------------------------ */

int ldaClear( LDA* ldaP)
{
  int     classX, classN = ldaNumber( ldaP);
  assert( ldaP);
  
  for ( classX=0; classX<classN; classX++) {
    LDAClass* cP    = ldaP->list.itemA + classX;
    if (cP->mean) dvectorClear(cP->mean);
  }
  if (ldaP->s) dmatrixClear(ldaP->s);
  return TCL_OK;
}

static int ldaClearItf (ClientData cd, int argc, char *argv[]) 
{ 
  LDA*  ldaP   = (LDA*)cd;
  int   ac     =  argc-1;

  if ( itfParseArgv( argv[0], &ac, argv+1, 0,
       NULL) != TCL_OK) return TCL_ERROR;

  return ldaClear( ldaP);
}

/* ------------------------------------------------------------------------
    saveMeans    save mean vectors
   ------------------------------------------------------------------------ */

int ldaSaveMeans( LDA* ldaP, char* filename)
{
  int     classX, classN;
  int     dimN, i, ret = TCL_OK;
  FMatrix *meanP;
  DVector *vecP;

  assert( ldaP);

  classN = ldaNumber( ldaP);
  dimN   = ldaP->dimN;

  if ((meanP = fmatrixCreate(classN,dimN+1)) == NULL) return TCL_ERROR;

  /* class means */

  for ( classX=0; classX<classN; classX++) {
     vecP = ldaP->list.itemA[classX].mean;
     if (vecP) {
	for (i=0; i<dimN; i++) meanP->matPA[classX][i] = vecP->vecA[i];
	meanP->matPA[classX][dimN] = vecP->count;
     }
  }

  if (!fmatrixBSave(meanP,filename)) {
     ERROR("Couldn't save lda means to %s\n",filename);
     ret = TCL_ERROR;
  }
  fmatrixFree(meanP);
  return ret;
}

static int ldaSaveMeansItf (ClientData cd, int argc, char *argv[]) 
{ 
  LDA*  ldaP   = (LDA*)cd;
  int   ac     =  argc-1;
  char* name   = NULL;

  if ( itfParseArgv( argv[0], &ac, argv+1, 0,
    "<filename>", ARGV_STRING, NULL, &name, cd, "filename for means",
    NULL) != TCL_OK) return TCL_ERROR;

  return ldaSaveMeans( ldaP, name);
}

/* ------------------------------------------------------------------------
    loadMeans
    load mean vectors and mix them with current
   ------------------------------------------------------------------------ */

int ldaLoadMeans( LDA* ldaP, char* filename)
{
  int      classX, classN;
  int      dimN, i, ret = TCL_OK;
  FMatrix* meanP;
  DVector* vecP;
  
  assert( ldaP);

  classN = ldaNumber( ldaP);
  dimN   = ldaP->dimN;

  if ((meanP = fmatrixCreate(classN,dimN+1)) == NULL) return TCL_ERROR;

  if (!fmatrixBLoad(meanP,filename,0)) {
     ERROR("Couldn't load LDA means from %s\n",filename); ret =  TCL_ERROR;
  }
  else if ( meanP->m != classN || (meanP->n - 1) != dimN) {
     ERROR("Number of classes %d or dimension %d of means in file\n"
	   "differs from lda object %d, %d\n",
	    meanP->m - 1, meanP->n - 1, classN, dimN);  ret = TCL_ERROR;
  }
  else { /* class means */
     for ( classX=0; classX<classN; classX++) {
	vecP = ldaP->list.itemA[classX].mean;
	if (vecP) {
	   vecP->count += meanP->matPA[classX][dimN];
	   for (i=0; i<dimN; i++)
	      vecP->vecA[i] += meanP->matPA[classX][i];
	}
     }
  }
  if ( meanP) fmatrixFree(meanP);
  return ret;
}

static int ldaLoadMeansItf (ClientData cd, int argc, char *argv[]) 
{ 
  LDA*  ldaP   = (LDA*)cd;
  int   ac     =  argc-1;
  char  *name  = NULL;

  if ( itfParseArgv( argv[0], &ac, argv+1, 0,
    "<filename>", ARGV_STRING, NULL, &name, cd, "",
    NULL) != TCL_OK) return TCL_ERROR;

  return ldaLoadMeans( ldaP, name);
}


/* ------------------------------------------------------------------------
    saveScatter    save scatter matrix
   ------------------------------------------------------------------------ */

int ldaSaveScatter( LDA* ldaP, char* name)
{
  int      ret = TCL_OK;
  FMatrix* A;

  assert( ldaP);
  if ((A = fmatrixCreate(ldaP->dimN,ldaP->dimN)) == NULL) return TCL_ERROR;

  fmatrixDMatrix(A,ldaP->s);
  if (!fmatrixBSave(A,name)) {
     ERROR("Couldn't save scatter matrix to %s\n",name);
     ret = TCL_ERROR;
  }
  fmatrixFree(A);
  return ret;
}

static int ldaSaveScatterItf (ClientData cd, int argc, char *argv[]) 
{ 
  LDA*  ldaP   = (LDA*)cd;
  int   ac     =  argc-1;
  char  *name  =  NULL;

  if ( itfParseArgv( argv[0], &ac, argv+1, 0,
    "<filename>", ARGV_STRING, NULL, &name, cd,"filename",
    NULL) != TCL_OK) return TCL_ERROR;

  return ldaSaveScatter( ldaP, name);
}

/* ------------------------------------------------------------------------
    loadScatter    load scatter matrix
   ------------------------------------------------------------------------ */

int ldaLoadScatter( LDA* ldaP, char* name)
{
  int      ret = TCL_OK;
  FMatrix *F;
  DMatrix *D;

  assert( ldaP);
  if ((F = fmatrixCreate(ldaP->dimN,ldaP->dimN)) == NULL ||
      (D = dmatrixCreate(ldaP->dimN,ldaP->dimN)) == NULL) return TCL_ERROR;

  if (!fmatrixBLoad(F,name,0)) {
     ERROR("Couldn't load scatter matrix from %s\n",name);
     ret = TCL_ERROR;
  }
  else {
     DMatrix *S = ldaP->s;
     dmatrixFMatrix(D,F);

     dmatrixAdd( S, 1, S, 1, D);
     S->count += D->count;
  }
  fmatrixFree(F); dmatrixFree(D); return ret;
}

static int ldaLoadScatterItf (ClientData cd, int argc, char *argv[]) 
{ 
  LDA*  ldaP   = (LDA*)cd;
  int   ac     =  argc-1;
  char  *name  = NULL;

  if ( itfParseArgv( argv[0], &ac, argv+1, 0,
    "<filename>", ARGV_STRING, NULL, &name, cd,"filename",
    NULL) != TCL_OK) return TCL_ERROR;

  return ldaLoadScatter( ldaP, name);
}

/* ========================================================================
    Type Declaration
   ======================================================================== */

static Method ldaClassMethod[] = 
{ 
  { "puts",  ldaClassPutsItf, NULL },
  {  NULL,  NULL,             NULL } 
};

TypeInfo ldaClassInfo = { "LDAClass", 0, 0, ldaClassMethod, 
                           NULL, NULL,
                           ldaClassConfigureItf, ldaClassAccessItf, NULL,
	                  "LDA class" } ;

static Method ldaMethod[] = 
{ 
  { "puts",   ldaPutsItf,    NULL },
  { "index",  ldaIndexItf,  "returns indices of named LDA classes" },
  { "name",   ldaNameItf,   "returns names of indexed LDA classes" },
  { "add",    ldaAddItf,    "add a new LDA class to the set" },
  { "delete", ldaDeleteItf, "remove LDA class from the set"  },
  { "map",    ldaMapItf,    "add/get index to class mapping information"  },
  { "accu",   ldaAccuItf,   "accumulate samples from a path object"  },
  { "update", ldaUpdateItf, "update the scatter matrices"  },
  { "clear",  ldaClearItf,  "clear means" },
  { "saveMeans",   ldaSaveMeansItf,   "save means to a file"  },
  { "saveScatter", ldaSaveScatterItf, "save scatter matrix to a file"  },
  { "loadMeans",   ldaLoadMeansItf,   "load means from a file"  },
  { "loadScatter", ldaLoadScatterItf, "load scatter matrix from a file"  },
  {  NULL,    NULL,          NULL } 
};

TypeInfo ldaInfo = { "LDA", 0, -1, ldaMethod, 
                      ldaCreateItf,    ldaFreeItf,
                      ldaConfigureItf, ldaAccessItf, NULL,
	             "LDA\n" } ;

static int ldaInitialized = 0;

int LDA_Init()
{
  if ( ldaInitialized) return TCL_OK;

  ldaDefault.name         =  NULL;
  ldaDefault.useN         =  0;
  ldaDefault.dimN         =  0;
  ldaDefault.featX        = -1;
  ldaDefault.list.blkSize =  100;
  ldaDefault.fesP         =  NULL;

  itfNewType( &ldaClassInfo);
  itfNewType( &ldaInfo);

  ldaInitialized = 1;
  return TCL_OK;
}


