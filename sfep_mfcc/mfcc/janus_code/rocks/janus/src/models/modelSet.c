/* ========================================================================
    JANUS      Speech- to Speech Translation
               ------------------------------------------------------------
               Object: ModelSet
               ------------------------------------------------------------

    Author  :  Michael Finke
    Module  :  modelSet.c
    Date    :  $Id: modelSet.c 700 2000-11-14 12:35:27Z janus $
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
    Revision 4.3  2000/11/14 12:35:24  janus
    Merging branch jtk-00-10-27-jmcd.

    Revision 4.2.30.2  2000/11/08 17:26:53  janus
    Finished making changes required for compilation under 'gcc -Wall'.

    Revision 4.2.30.1  2000/11/06 10:50:37  janus
    Made changes to enable compilation under -Wall.

    Revision 4.2  2000/08/16 11:35:55  soltau
    free -> TclFree

    Revision 4.1  2000/01/12 10:12:49  fuegen
    add procedure to delete a model from an modelArray

    Revision 4.0  1998/11/30 18:07:14  fuegen
    *** empty log message ***

    Revision 3.2  1997/06/17 17:41:43  westphal
    *** empty log message ***

     Revision 1.3  96/04/01  23:36:20  finkem
     block allocation for modelSetArray introduced
     
     Revision 1.2  1996/02/11  06:25:01  finkem
     add modelArray clear

     Revision 1.1  1996/01/31  05:11:37  finkem
     Initial revision


   ======================================================================== */

#include "common.h"
#include "itf.h"
#include "modelSet.h"

char modelSetRCSVersion[]= 
           "@(#)1$Id: modelSet.c 700 2000-11-14 12:35:27Z janus $";


/* ========================================================================
    ModelSet
   ======================================================================== */

static ModelSet* modelSetPA[100];
static int       modelSetN  = 0;

/* ------------------------------------------------------------------------
    modelSetNewType
   ------------------------------------------------------------------------ */

int modelSetNewType( ModelSet* strP)
{
  int   i;
  for ( i = 0; i < modelSetN; i++) if ( modelSetPA[i] == strP) return i;

  assert(  modelSetN < 98);
  modelSetPA[modelSetN] = strP;
  return     modelSetN++;
}

/* ------------------------------------------------------------------------
    modelSetList
   ------------------------------------------------------------------------ */

int modelSetList( ClientData clientData, Tcl_Interp *interp,
                int argc, char *argv[])
{
  int i;

  for ( i = 0; i < modelSetN; i++) 
    itfAppendElement("%s %s", modelSetPA[i]->name, modelSetPA[i]->tiP->name);

  return TCL_OK;
}

/* ========================================================================
    ModelSetPtr
   ======================================================================== */
/* ------------------------------------------------------------------------
    modelSetPtrInit
   ------------------------------------------------------------------------ */

int modelSetPtrInit( ModelSetPtr* mdsP)
{
  mdsP->cd         = NULL;
  mdsP->modelSetP  = NULL;
  return TCL_OK;
}

/* ------------------------------------------------------------------------
    modelSetPtrDeinit
   ------------------------------------------------------------------------ */

int modelSetPtrDeinit( ModelSetPtr* msaP)
{
  return TCL_OK;
}

/* ------------------------------------------------------------------------
    modelSetPtrGetItf
   ------------------------------------------------------------------------ */

int modelSetPtrGetItf( ModelSetPtr* mP, char* value)
{
  int     tokc;
  char**  tokv;

  if ( Tcl_SplitList( itf, value, &tokc, &tokv) == TCL_OK) {
    int    ac = tokc;
    char** av = tokv;
    char*  sn = NULL;
    char*  st = NULL;
    int    j;

    if ( itfParseArgv( "ModelSetPtr", &ac, av, 0,
      "<modelSet>",    ARGV_STRING, NULL, &sn, NULL,"modelSet object",
      "-modelSetType", ARGV_STRING, NULL, &st, NULL,"modelSet type",
       NULL) != TCL_OK) { 
       Tcl_Free((char*)tokv);
       return TCL_ERROR;
    }

    for ( j = 0; j < modelSetN; j++) {
      ClientData cd = itfGetObject( sn, modelSetPA[j]->tiP->name);
      if ( cd) {
        if (!st || (st && !strcmp(st,modelSetPA[j]->name))) break;
      }
      else MSGCLEAR(NULL);
    }
    if ( j >= modelSetN) {
      ERROR("Object %s is not a modelSet type.\n",sn);
      Tcl_Free((char*)tokv);
      return TCL_ERROR;
    }
    mP->cd        = itfGetObject( sn, modelSetPA[j]->tiP->name);
    mP->modelSetP = modelSetPA[j];
    Tcl_Free((char*)tokv);
    return TCL_OK;
  }
  return TCL_ERROR;
}

int getModelSetPtr( ClientData cd, char* key, ClientData result,
	              int *argcP, char *argv[])
{
   ModelSetPtr* strP = (ModelSetPtr*)result;
   
   if ((*argcP < 1) || 
       ( modelSetPtrGetItf(strP, argv[0]) != TCL_OK)) return -1;
   (*argcP)--; return 0;
}

/* ========================================================================
    ModelArray
   ======================================================================== */
/* ------------------------------------------------------------------------
    modelArrayInit
   ------------------------------------------------------------------------ */

int modelArrayInit( ModelArray* mdaP, ModelSetPtr* mdsP)
{
  mdaP->modelA = NULL;
  mdaP->countA = NULL;
  mdaP->modelN = 0;
  mdaP->allocN = 0;

  mdaP->mdsP.cd        = mdsP->cd;
  mdaP->mdsP.modelSetP = mdsP->modelSetP;
  return TCL_OK;
}

ModelArray *modelArrayCreate( ModelSetPtr* mdsP)
{
  ModelArray *mdaP = (ModelArray*)malloc(sizeof(ModelArray));

  if (! mdaP || modelArrayInit( mdaP, mdsP) != TCL_OK) {
     if ( mdaP) free( mdaP);
     ERROR("Failed to allocate ModelArray object.\n");
     return NULL; 
  }
  return mdaP;
}

static ClientData modelArrayCreateItf( ClientData clientData, 
                                       int argc, char *argv[]) 
{ 
  int         ac   = argc - 1;
  ModelSetPtr mdsP;

  modelSetPtrInit( &mdsP);

  if ( itfParseArgv( argv[0], &ac, argv+1, 0,
      "<ModelSet>",ARGV_CUSTOM,getModelSetPtr,&mdsP,NULL,"model set",
       NULL) != TCL_OK) return NULL;

  return (ClientData)modelArrayCreate(&mdsP);
}

/* ------------------------------------------------------------------------
    modelArrayDeinit
   ------------------------------------------------------------------------ */

int modelArrayDeinit( ModelArray* mdaP)
{
  if ( mdaP->modelA) { free( mdaP->modelA); mdaP->modelA = NULL; }
  if ( mdaP->countA) { free( mdaP->countA); mdaP->countA = NULL; }

  mdaP->modelN = 0;
  mdaP->allocN = 0;
  return TCL_OK;
}

int modelArrayFree( ModelArray* mdaP)
{
  modelArrayDeinit( mdaP);
  free(mdaP);
  return TCL_OK;
}

static int modelArrayFreeItf (ClientData clientData)
{
  ModelArray* mdaP = (ModelArray*)clientData;
  return modelArrayFree( mdaP);
}

/* ------------------------------------------------------------------------
    modelArrayConfigureItf
   ------------------------------------------------------------------------ */

static int modelArrayConfigureItf(ClientData cd, char *var, char *val)
{
  ModelArray *MA = (ModelArray*)cd;
  if (! MA) return TCL_ERROR;

  if (var==NULL) {

    ITFCFG(modelArrayConfigureItf,cd,"itemN");
    return TCL_OK;
  }
  ITFCFG_Int(    var,val,"itemN", MA->modelN, 1);
  ERROR("Unknown option '-%s %s'.\n", var, val ? val : "?");
  return TCL_ERROR;   
}

/* ------------------------------------------------------------------------
    modelArrayPuts  print info on a model array
   ------------------------------------------------------------------------ */

static int modelArrayPutsItf (ClientData clientData, int argc, char *argv[])
{
  ModelArray* mdaP = (ModelArray*)clientData;
  int         i;

  for ( i=0; i<mdaP->modelN; i++)
    itfAppendElement( "%s %f", mdaP->mdsP.modelSetP->nameFct(
                               mdaP->mdsP.cd, mdaP->modelA[i]),
                               mdaP->countA[i]);
  return TCL_OK;
}


/* ------------------------------------------------------------------------
    modelArrayClear   remove all entries from the model array
   ------------------------------------------------------------------------ */

int modelArrayClear( ModelArray* mdaP)
{
  mdaP->modelN = 0;
  return TCL_OK;
}

int modelArrayClearItf ( ClientData CD, int argc, char *argv[] )
{
  ModelArray* mdaP  = (ModelArray*)CD;
  int         ac    =  argc-1;

  if (itfParseArgv(argv[0], &ac, argv+1, 0, NULL) != TCL_OK) return TCL_ERROR;
  return modelArrayClear( mdaP);
}

/* ------------------------------------------------------------------------
    modelArrayAdd    add new models and/or counts to the model array
   ------------------------------------------------------------------------ */

int modelArrayAdd( ModelArray* mdaP, int model, float count)
{
  int i, n = mdaP->modelN;

  for ( i = 0; i < n; i++) if ( mdaP->modelA[i] == model) break;
  if (  i < n) mdaP->countA[i] += count;
  else {
    if ( n < mdaP->allocN) {
      mdaP->modelA[i] = model;
      mdaP->countA[i] = count; 
      mdaP->modelN++;
    }
    else {
      mdaP->allocN   +=  100;
      mdaP->modelA    = (int  *)realloc( mdaP->modelA, (mdaP->allocN)*sizeof(int));
      mdaP->countA    = (float*)realloc( mdaP->countA, (mdaP->allocN)*sizeof(float));
      mdaP->modelA[n] =  model;
      mdaP->countA[n] =  count;
      mdaP->modelN++;
    }
  }
  return TCL_OK;
}

int modelArrayAddItf ( ClientData CD, int argc, char *argv[] )
{
  ModelArray* mdaP  = (ModelArray*)CD;
  char*       name  =  NULL;
  float       count =  0.0;
  int         ac    =  argc-1;
  int         model = -1;

  if (itfParseArgv(argv[0], &ac, argv+1, 0,
      "<model>",  ARGV_STRING,NULL,&name, NULL,"name of the model",
      "<count>",  ARGV_FLOAT, NULL,&count,NULL,"count",
       NULL) != TCL_OK) return TCL_ERROR;

  if ((model = mdaP->mdsP.modelSetP->indexFct(mdaP->mdsP.cd, name)) < 0) {
    ERROR("Model '%s' doesn't exist.\n", name);
    return TCL_ERROR;
  }
  return modelArrayAdd( mdaP, model, count);
}

int modelArrayDelete (ModelArray *mdaP, int model, float count) {

    int i,  n = mdaP->modelN;

    for ( i = 0; i < n; i++ ) {
	if ( mdaP->modelA[i] == model ) {
	    if ( mdaP->countA[i] == count ) {
		mdaP->modelN--;
		break;
	    } else {
		mdaP->countA[i] -= count;
		return TCL_OK;
	    }
	}
    }

    for ( i = i+1; i < n; i++ ) {
	mdaP->modelA[i-1] = mdaP->modelA[i];
	mdaP->countA[i-1] = mdaP->countA[i];
    }

    return TCL_OK;
}


/* ========================================================================
    Type Declaration
   ======================================================================== */

static Method modelArrayMethod[] =
{
  { "puts",   modelArrayPutsItf,    "print model array"                 },
  { "clear",  modelArrayClearItf,   "remove all entries from the array" },
  { "add",    modelArrayAddItf,     "add another model to the array"    },
  { NULL, NULL, NULL }
} ;

TypeInfo modelArrayInfo = { 
        "ModelArray", 0, -1,     modelArrayMethod, 
         modelArrayCreateItf,    modelArrayFreeItf, 
         modelArrayConfigureItf, NULL, itfTypeCntlDefaultNoLink,
        "Array of models." };

static int modelSetInitialized = 0;

int ModelSet_Init ( )
{
  if ( modelSetInitialized) return TCL_OK;

  Tcl_CreateCommand ( itf, "modelSets", modelSetList, 0, 0 );

  itfNewType(&modelArrayInfo);

  modelSetInitialized = 1;
  return TCL_OK;
}
