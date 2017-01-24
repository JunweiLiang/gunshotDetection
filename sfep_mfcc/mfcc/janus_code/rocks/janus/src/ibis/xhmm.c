/* ========================================================================
    JANUS      Speech- to Speech Translation
               ------------------------------------------------------------
               Object: Crossword Phone Hidden Markov Model
               ------------------------------------------------------------

    Author  :  Hagen Soltau
    Module  :  xhmm.c
    Date    :  $Id: xhmm.c 2795 2007-03-19 15:11:23Z fuegen $
    Remarks : 

   ========================================================================

    This software was developed by

    the  University of Karlsruhe       and  Carnegie Mellon University
         Dept. of Informatics               Dept. of Computer Science
         ILKD, Lehrstuhl Prof.Waibel        Alex Waibel's NN & Speech Group
         Am Fasanengarten 5                 5000 Forbes Ave
         D-76131 Karlsruhe                  Pittsburgh, PA 15213
         - Germany -                        - USA -

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
    Revision 1.7  2007/03/19 15:11:23  fuegen
    minor bugfix in assert

    Revision 1.6  2004/09/29 12:03:06  metze
    Also for xcmSetInit

    Revision 1.5  2004/09/29 11:59:11  metze
    Catch aborts in lcmSetInit when SenoneSet doesn't know about Phones

    Revision 1.4  2003/08/14 11:20:10  fuegen
    Merged changes on branch jtk-01-01-15-fms (jaguar -> ibis-013)

    Revision 1.3.4.11  2003/06/02 16:24:56  soltau
    xcmLoad: better memory allocation

    Revision 1.3.4.10  2002/08/27 08:50:12  metze
    itfParseArgv now uses <name> and NULL initialization for char*

    Revision 1.3.4.9  2002/06/26 11:57:55  fuegen
    changes for new directory structure support and code cleaning

    Revision 1.3.4.8  2002/01/27 15:11:59  soltau
    Added ignoreRCM flag
    Fixed xcmPuts mapX,mapY access
    Fixed allocation problem for mapXA array in xcmInit

    Revision 1.3.4.7  2001/07/31 19:02:12  soltau
    Added warnings in load routines

    Revision 1.3.4.6  2001/07/14 15:35:58  soltau
    Fixed padPhone problems

    Revision 1.3.4.5  2001/07/12 18:42:25  soltau
    Added xcm

    Revision 1.3.4.4  2001/06/28 10:48:24  soltau
    Added Load/save routines for binary files to support fast decoder startup

    Revision 1.3.4.3  2001/06/20 17:25:58  soltau
    *** empty log message ***

    Revision 1.3.4.2  2001/02/01 18:42:33  soltau
    Fixed bug with rcm hashkeys

    Revision 1.3.4.1  2001/01/29 18:01:52  soltau
    CONTEXT -> CTX

    Revision 1.3  2001/01/22 09:22:14  soltau
    Fixed non-constant initializer problems with Sun compiler

    Revision 1.2  2001/01/18 13:49:30  soltau
    Added access functions

    Revision 1.1  2000/12/29 06:19:21  soltau
    Initial revision

 
   ======================================================================== */

char  xhmmRcsVersion[]=
    "$Id: xhmm.c 2795 2007-03-19 15:11:23Z fuegen $";

#include "xhmm.h"
#include "mach_ind_io.h"


/* ------------------------------------------------------------------------
    forward declarations 
   ------------------------------------------------------------------------ */

extern TypeInfo  lcmInfo;
extern TypeInfo  lcmSetInfo;
static LCMSet    lcmSetDefault;

extern TypeInfo  rcmInfo;
extern TypeInfo  rcmSetInfo;
static RCMSet    rcmSetDefault;

extern TypeInfo  xcmInfo;
extern TypeInfo  xcmSetInfo;
static XCMSet    xcmSetDefault;

/* ========================================================================
    Left Context Model 
   ======================================================================== */

/* ------------------------------------------------------------------------
    lcmInit
   ------------------------------------------------------------------------ */

static int lcmInit( LCM* lcmP, LCM* hashkey)
{
  lcmP->hmmN  = hashkey->hmmN;
  lcmP->hmmXA = NULL;

  if (NULL == (lcmP->hmmXA= (PHMMX*)malloc(lcmP->hmmN*sizeof(PHMMX)))) {
    ERROR("lcmInit: allocation failure\n");
    return TCL_ERROR;
  }
  memcpy(lcmP->hmmXA,hashkey->hmmXA,lcmP->hmmN*sizeof(PHMMX));
  return TCL_OK;
}

/* ------------------------------------------------------------------------
    lcmDeinit
   ------------------------------------------------------------------------ */

static int lcmDeinit( LCM* lcmP)
{
  if (lcmP->hmmXA) free(lcmP->hmmXA);
  lcmP->hmmN  = 0;
  lcmP->hmmXA = NULL;

  return TCL_OK;
}

/* ------------------------------------------------------------------------
    lcm hash function
   ------------------------------------------------------------------------ */

static int lcmHashCmp( const LCM* key, const LCM* lcmP)
{
  int hmmX;

  if (lcmP->hmmXA[0] != key->hmmXA[0]) return 0;
  if (lcmP->hmmN     != key->hmmN )    return 0;

  for (hmmX=1;hmmX<lcmP->hmmN;hmmX++)
    if (lcmP->hmmXA[hmmX] != key->hmmXA[hmmX]) return 0;

  return 1;
}

#define lcmBit 5

static long lcmHashKey( const LCM* key)
{
  unsigned long ret = 0; 
  int           hmmX;
  for (hmmX=0; hmmX<key->hmmN; hmmX++) 
    ret = ((ret<<lcmBit) | (ret >> (8*sizeof(long)-lcmBit))) \
      ^ key->hmmXA[hmmX];
  return ret;
}

/* ------------------------------------------------------------------------
    lcmPuts
   ------------------------------------------------------------------------ */

static int lcmPutsItf (ClientData clientData, int argc, char *argv[])
{
  LCM* lcmP = (LCM*)clientData;
  int  hmmX;

  itfAppendResult("%d {",lcmP->hmmN);
  for (hmmX=0; hmmX< lcmP->hmmN; hmmX++)
    itfAppendElement("%d",lcmP->hmmXA[hmmX]);
  itfAppendResult(" }");

  return TCL_OK;    
}

/* ========================================================================
    Set of Left Context Models 
   ======================================================================== */

/* ------------------------------------------------------------------------
    lcmSetInit
   ------------------------------------------------------------------------ */

static int lcmSetInit (LCMSet *lcmsP, char* name, PHMMSet* hmmsP)
{

  if (!hmmsP) {
    ERROR("lcmSetInit: invalid PHMMSet\n");
    return TCL_ERROR;
  }
  if (!hmmsP->snsP->phones) {
    ERROR ("lcmSetInit: No phones in %s\n", hmmsP->snsP->name);
    return TCL_ERROR;
  }
  if (hmmsP->snsP->phones->list.itemN > CTX_MAX) {
    ERROR("lcmSetInit: Number of phones (%d) more than %d\n",
	  hmmsP->snsP->phones->list.itemN,CTX_MAX);
    return TCL_ERROR;
  }
  lcmsP->name  = strdup(name);
  lcmsP->useN  = 0;
  lcmsP->hmmsP = hmmsP;
  link(lcmsP->hmmsP,"PHMMSet");

  listInit((List*)&(lcmsP->list),(TypeInfo*) &lcmInfo, sizeof(LCM), 
                    lcmSetDefault.list.blkSize);

  lcmsP->list.init    = (ListItemInit  *)lcmInit;
  lcmsP->list.deinit  = (ListItemDeinit*)lcmDeinit;
  lcmsP->list.hashKey = (HashKeyFn     *)lcmHashKey;
  lcmsP->list.hashCmp = (HashCmpFn     *)lcmHashCmp;

  return TCL_OK;
}

/* ------------------------------------------------------------------------
    lcmSetCreate
   ------------------------------------------------------------------------ */

LCMSet* lcmSetCreate (char* name, PHMMSet* hmmsP)
{
  LCMSet *lcmsP = (LCMSet*) malloc(sizeof(LCMSet));

  if (! lcmsP) {
    ERROR("phmmSetCreate: allocation failure\n");
    return NULL;
  }
  if (TCL_ERROR == (lcmSetInit(lcmsP,name,hmmsP))) {    
    free(lcmsP);
    return NULL;
  }
  return lcmsP;
}

static ClientData lcmSetCreateItf( ClientData clientData, 
                                      int argc, char *argv[])
{
  int          ac    =  argc;
  PHMMSet*     hmmsP =  NULL;
  char       *name = NULL; 

  if ( itfParseArgv( argv[0], &ac, argv, 0,
      "<name>",  ARGV_STRING, NULL, &name, NULL,         "name of the LCM set",
      "<PHMMSet>",ARGV_OBJECT, NULL, &hmmsP, "PHMMSet", "phone HMM Set",
       NULL) != TCL_OK) return NULL;
  
  return (ClientData) lcmSetCreate(name,hmmsP);
}

/* ------------------------------------------------------------------------
    lcmSetDeinit
   ------------------------------------------------------------------------ */

static int lcmSetDeinit (LCMSet *lcmsP)
{
  if (lcmsP->useN >0) {
    ERROR("lcmSet '%s' still in use by other objects.\n", 
            lcmsP->name); 
    return TCL_ERROR;
  }
  delink(lcmsP->hmmsP,"PHMMSet");
  if (lcmsP->name) free(lcmsP->name);

  lcmsP->name  = NULL;
  lcmsP->hmmsP = NULL;
  return listDeinit((List*)&(lcmsP->list));
}

/* ------------------------------------------------------------------------
    lcmSetFree
   ------------------------------------------------------------------------ */

int lcmSetFree(LCMSet *lcmsP)
{
  if ( lcmSetDeinit(lcmsP) != TCL_OK) return TCL_ERROR;
  free(lcmsP);
  return TCL_OK;
}

static int lcmSetFreeItf (ClientData clientData)
{
  return lcmSetFree((LCMSet*)clientData);
}

/* ------------------------------------------------------------------------
    lcmSetAdd
   ------------------------------------------------------------------------ */

int lcmSetAdd (LCMSet* lcmsP,CTX hmmN,PHMMX* hmmXA)
{ 
  LCM  key;
  int idx;
  
  if (hmmN == 0 || hmmXA == NULL) {
    ERROR("lcmSetAdd:invalid hmmN or hmmXA\n");
    return -1;
  }

  /* create model, if we don't have it yet*/
  key.hmmN  = hmmN;
  key.hmmXA = hmmXA;
  idx = listIndex((List*) &(lcmsP->list), &key, 1);  
  return idx;
}

/* ------------------------------------------------------------------------
    lcmSetLoad
   ------------------------------------------------------------------------ */

int lcmSetLoad (LCMSet* lcmsP,FILE* fp)
{ 
  char str[1024];
  LCM key;
  int hmmX,lcmX,lcmN;

  if (! fp) {
    ERROR("lcmSetLoad: Invalid file pointer\n");
    return TCL_ERROR;
  }
  if (lcmsP->list.itemN > 0) {
    WARN("lcmSet: expected empty LCMSet (do it on your own risk)\n");
  }
  read_string(fp,str);
  if (0 != strcmp(str,"LCMSET-BEGIN")) {
    ERROR("lcmSetLoad: inconsistent file\n");
    return TCL_ERROR;
  }

  if (NULL == (key.hmmXA = malloc(CTX_MAX*sizeof(PHMMX)))) {
    ERROR("lcmSetLoad: allocation failure\n");
    return TCL_ERROR;
  }

  lcmN= read_int(fp);
  for (lcmX=0;lcmX<lcmN;lcmX++) {
    key.hmmN  = (int) read_int(fp);   
    for (hmmX=0;hmmX<key.hmmN;hmmX++) {
      key.hmmXA[hmmX]=(PHMMX) read_int(fp);
    }
    listIndex((List*) &(lcmsP->list), &key, 1);  
  }
  read_string(fp,str);
  if (0 != strcmp(str,"LCMSET-END")) {
    ERROR("lcmSetLoad: inconsistent file\n");
    return TCL_ERROR;
  }

  if (key.hmmXA) free(key.hmmXA);
  return TCL_OK;
}

static int lcmSetLoadItf( ClientData cd, int argc, char *argv[])
{
  LCMSet*    lcmsP = (LCMSet*)cd;
  int        ac    =  argc - 1;
  char*      name  =  NULL;
  FILE*      fp    = NULL;

  if ( itfParseArgv( argv[0], &ac, argv+1, 0,
      "<filename>", ARGV_STRING, NULL, &name, cd, "file to load from",
       NULL) != TCL_OK) return TCL_ERROR;

  fp = fileOpen(name,"r");
  lcmSetLoad(lcmsP,fp);
  fileClose(name,fp);
  return TCL_OK;
}

/* ------------------------------------------------------------------------
    lcmSetSave
   ------------------------------------------------------------------------ */

int lcmSetSave (LCMSet* lcmsP,FILE* fp)
{ 
  int lcmX;
  if (! fp) {
    ERROR("lcmSetSave: Invalid file pointer\n");
    return TCL_ERROR;
  }

  write_string(fp,"LCMSET-BEGIN");
  write_int   (fp,lcmsP->list.itemN); 
  
  for (lcmX=0;lcmX<lcmsP->list.itemN;lcmX++) {
    LCM* lcmP = lcmsP->list.itemA+lcmX;
    int  hmmX;
    write_int(fp,(int) lcmP->hmmN); 
    for (hmmX=0;hmmX<lcmP->hmmN;hmmX++)
      write_int(fp,(int) lcmP->hmmXA[hmmX]);
  }
  write_string(fp,"LCMSET-END");
  fflush(fp);
  return TCL_OK;
}

static int lcmSetSaveItf( ClientData cd, int argc, char *argv[])
{
  LCMSet*    lcmsP = (LCMSet*)cd;
  int        ac    =  argc - 1;
  char*      name  =  NULL;
  FILE*      fp    = NULL;

  if ( itfParseArgv( argv[0], &ac, argv+1, 0,
      "<filename>", ARGV_STRING, NULL, &name, cd, "file to load from",
       NULL) != TCL_OK) return TCL_ERROR;

  fp = fileOpen(name,"w");
  lcmSetSave(lcmsP,fp);
  fileClose(name,fp);
  return TCL_OK;
}

/* ------------------------------------------------------------------------
    lcmSetPuts
   ------------------------------------------------------------------------ */

static int lcmSetPutsItf (ClientData clientData, int argc, char *argv[])
{
  LCMSet* lcmsP = (LCMSet*)clientData;
  itfAppendResult("%d",lcmsP->list.itemN);
  return TCL_OK;    
}

/* ------------------------------------------------------------------------
    lcmSetAccessItf
   ------------------------------------------------------------------------ */

static ClientData lcmSetAccessItf (ClientData cd, char *name, TypeInfo **ti)
{ 
  LCMSet *lcmsP = (LCMSet*)cd;

  if (*name == '.') { 
    if (name[1]=='\0') {
      itfAppendElement( "phmmSet");
      *ti = NULL; return NULL;
    }
    else { 
      if (! strcmp( name+1, "phmmSet")) {
        *ti = itfGetType("PHMMSet");
        return (ClientData)lcmsP->hmmsP;
      }
    }
  }
  else if (*name == ':') { 
    if (name[1]=='\0') { 
      itfAppendElement( "0 %d",lcmsP->list.itemN-1);
      *ti = NULL; return NULL;
    }
    else { 
      int i = atoi(name+1);
      *ti = &lcmInfo;
      if (i>=0 && i<lcmsP->list.itemN) 
           return (ClientData) &(lcmsP->list.itemA[i]);
      else return NULL;
    }
  }
  return NULL;
}


/* ========================================================================
    Right Context Model 
   ======================================================================== */

/* Note: 
   Since we need a hmmN,hmmXA struct for a RCM hashkey, we can
   reuse the LCM struct and the lcmHash functions.
*/

/* ------------------------------------------------------------------------
    rcmInit
   ------------------------------------------------------------------------ */

static int rcmInit( RCM* rcmP, LCM* hashkey)
{
  int cX=0;
  rcmP->hmmN  = 0;
  rcmP->hmmXA = NULL;
  rcmP->mapN  = hashkey->hmmN;
  rcmP->mapXA = NULL;

  /* We cannot have more the mapN unique pHMM's. This array will be
     reduced by the correct number rcmP->hmmN after we know the
     number of unique models.
  */
  if (NULL == (rcmP->hmmXA= (PHMMX*)malloc(rcmP->mapN*sizeof(PHMMX)))) {
    ERROR("rcmInit: allocation failure\n");
    return TCL_ERROR;
  }
  if (NULL == (rcmP->mapXA= (CTX*)malloc(rcmP->mapN*sizeof(CTX)))) {
    ERROR("rcmInit: allocation failure\n");
    return TCL_ERROR;
  }
  
  for (cX=0;cX<rcmP->mapN;cX++) {
    int  modelX = 0;
    PHMMX hmmX = hashkey->hmmXA[cX];

    /* check if we have the model already */
    for (modelX=0;modelX<rcmP->hmmN;modelX++)
      if (rcmP->hmmXA[modelX] == hmmX) break;

    /* map contextX -> modelX */
    rcmP->mapXA[cX]=modelX;

    /* map modelX -> hmmX if we have it not yet */
    if (modelX == rcmP->hmmN) {
      rcmP->hmmXA[modelX] = hmmX;
      rcmP->hmmN++;
    }
  }
  rcmP->hmmXA= (PHMMX*)realloc((void*)rcmP->hmmXA,rcmP->hmmN*sizeof(PHMMX));

  return TCL_OK;
}

/* ------------------------------------------------------------------------
    rcmDeinit
   ------------------------------------------------------------------------ */

static int rcmDeinit( RCM* rcmP)
{
  if (rcmP->hmmXA) free(rcmP->hmmXA);
  if (rcmP->mapXA) free(rcmP->mapXA);

  rcmP->hmmN  = 0;
  rcmP->hmmXA = NULL;
  rcmP->mapN  = 0;
  rcmP->mapXA = NULL;

  return TCL_OK;
}

/* ------------------------------------------------------------------------
    rcmPuts
   ------------------------------------------------------------------------ */

static int rcmPutsItf (ClientData clientData, int argc, char *argv[])
{
  RCM* rcmP = (RCM*)clientData;
  int  mapX;

  itfAppendResult("%d {",rcmP->mapN);
  for (mapX=0; mapX< rcmP->mapN; mapX++)
    itfAppendElement("%d",rcmP->hmmXA[rcmP->mapXA[mapX]]);
  itfAppendResult(" }");

  return TCL_OK;    
}

/* ------------------------------------------------------------------------
    rcm hash function
   ------------------------------------------------------------------------ */

static int rcmHashCmp( const LCM* key, const RCM* rcmP)
{
  PHMMX* hmmXA = rcmP->hmmXA;
  CTX*   mapXA = rcmP->mapXA;
  int hmmX;

  if (hmmXA[mapXA[0]] != key->hmmXA[0]) return 0;
  if (rcmP->mapN      != key->hmmN )    return 0;

  for (hmmX=1;hmmX<rcmP->mapN;hmmX++)
    if (hmmXA[mapXA[hmmX]] != key->hmmXA[hmmX]) return 0;

  return 1;
}

#define rcmBit 5

static long rcmHashKey( const LCM* key)
{
  unsigned long ret = 0; 
  int           hmmX;
  for (hmmX=0; hmmX<key->hmmN; hmmX++) 
    ret = ((ret<<rcmBit) | (ret >> (8*sizeof(long)-rcmBit))) \
      ^ key->hmmXA[hmmX];
  return ret;
}


/* ========================================================================
    Set of Right Context Models 
   ======================================================================== */

/* ------------------------------------------------------------------------
    rcmSetInit
   ------------------------------------------------------------------------ */

static int rcmSetInit (RCMSet *rcmsP, char* name, PHMMSet* hmmsP)
{

  if (!hmmsP) {
    ERROR("rcmSetInit: invalid PHMMSet\n");
    return TCL_ERROR;
  }
  if (!hmmsP->snsP->phones) {
    ERROR ("rcmSetInit: No phones in %s\n", hmmsP->snsP->name);
    return TCL_ERROR;
  }
  if (hmmsP->snsP->phones->list.itemN > CTX_MAX) {
    ERROR("rcmSetInit: Number of phones (%d) more than %d\n",
	  hmmsP->snsP->phones->list.itemN,CTX_MAX);
    return TCL_ERROR;
  }

  rcmsP->name  = strdup(name);
  rcmsP->useN  = 0;
  rcmsP->hmmsP = hmmsP;
  link(rcmsP->hmmsP,"PHMMSet");

  listInit((List*)&(rcmsP->list),(TypeInfo*) &rcmInfo, sizeof(RCM), 
                    rcmSetDefault.list.blkSize);

  rcmsP->list.init    = (ListItemInit  *)rcmInit;
  rcmsP->list.deinit  = (ListItemDeinit*)rcmDeinit;
  rcmsP->list.hashKey = (HashKeyFn     *)rcmHashKey;
  rcmsP->list.hashCmp = (HashCmpFn     *)rcmHashCmp;

  return TCL_OK;
}

/* ------------------------------------------------------------------------
    rcmSetCreate
   ------------------------------------------------------------------------ */

RCMSet* rcmSetCreate (char* name, PHMMSet* hmmsP)
{
  RCMSet *rcmsP = (RCMSet*) malloc(sizeof(RCMSet));

  if (! rcmsP) {
    ERROR("phmmSetCreate: allocation failure\n");
    return NULL;
  }
  if (TCL_ERROR == (rcmSetInit(rcmsP,name,hmmsP))) {    
    free(rcmsP);
    return NULL;
  }
  return rcmsP;
}

static ClientData rcmSetCreateItf( ClientData clientData, 
                                      int argc, char *argv[])
{
  int          ac    =  argc;
  PHMMSet*     hmmsP =  NULL;
  char       *name = NULL; 

  if ( itfParseArgv( argv[0], &ac, argv, 0,
     "<name>",  ARGV_STRING, NULL, &name, NULL,         "name of the RCM set",
     "<PHMMSet>",ARGV_OBJECT, NULL, &hmmsP, "PHMMSet", "phone HMM Set",
       NULL) != TCL_OK) return NULL;
  
  return (ClientData) rcmSetCreate(name,hmmsP);
}

/* ------------------------------------------------------------------------
    rcmSetDeinit
   ------------------------------------------------------------------------ */

static int rcmSetDeinit (RCMSet *rcmsP)
{
  if (rcmsP->useN >0) {
    ERROR("rcmSet '%s' still in use by other objects.\n", 
            rcmsP->name); 
    return TCL_ERROR;
  }
  delink(rcmsP->hmmsP,"PHMMSet");
  if (rcmsP->name) free(rcmsP->name);

  rcmsP->name  = NULL;
  rcmsP->hmmsP = NULL;
  return listDeinit((List*)&(rcmsP->list));
}

/* ------------------------------------------------------------------------
    rcmSetFree
   ------------------------------------------------------------------------ */

int rcmSetFree(RCMSet *rcmsP)
{
  if ( rcmSetDeinit(rcmsP) != TCL_OK) return TCL_ERROR;
  free(rcmsP);
  return TCL_OK;
}

static int rcmSetFreeItf (ClientData clientData)
{
  return rcmSetFree((RCMSet*)clientData);
}

/* ------------------------------------------------------------------------
    rcmSetPuts
   ------------------------------------------------------------------------ */

static int rcmSetPutsItf (ClientData clientData, int argc, char *argv[])
{
  RCMSet* rcmsP = (RCMSet*)clientData;
  itfAppendResult("%d",rcmsP->list.itemN);
  return TCL_OK;    
}

/* ------------------------------------------------------------------------
    rcmSetAccessItf
   ------------------------------------------------------------------------ */

static ClientData rcmSetAccessItf (ClientData cd, char *name, TypeInfo **ti)
{ 
  RCMSet *rcmsP = (RCMSet*)cd;

  if (*name == '.') { 
    if (name[1]=='\0') {
      itfAppendElement( "phmmSet");
      *ti = NULL; return NULL;
    }
    else { 
      if (! strcmp( name+1, "phmmSet")) {
        *ti = itfGetType("PHMMSet");
        return (ClientData)rcmsP->hmmsP;
      }
    }
  }
  else if (*name == ':') { 
    if (name[1]=='\0') { 
      itfAppendElement( "0 %d",rcmsP->list.itemN-1);
      *ti = NULL; return NULL;
    }
    else { 
      int i = atoi(name+1);
      *ti = &rcmInfo;
      if (i>=0 && i<rcmsP->list.itemN) 
           return (ClientData) &(rcmsP->list.itemA[i]);
      else return NULL;
    }
  }
  return NULL;
}

/* ------------------------------------------------------------------------
    rcmSetAdd
   ------------------------------------------------------------------------ */

int rcmSetAdd (RCMSet* rcmsP,CTX hmmN,PHMMX* hmmXA)
{ 
  RCM  key;
  int idx;

  if (hmmN == 0 || hmmXA == NULL) {
    ERROR("rcmSetAdd:invalid hmmN or hmmXA\n");
    return -1;
  }

  /* create model, if we don't have it yet*/
  key.hmmN  = hmmN;
  key.hmmXA = hmmXA;
  idx = listIndex((List*) &(rcmsP->list), &key, 1);  
  return idx;
}

/* ------------------------------------------------------------------------
    rcmSetLoad
   ------------------------------------------------------------------------ */

int rcmSetLoad (RCMSet* rcmsP,FILE* fp)
{ 
  char str[1024];
  LCM key;
  int hmmX,rcmX,rcmN;

  if (! fp) {
    ERROR("rcmSetLoad: Invalid file pointer\n");
    return TCL_ERROR;
  }
  if (rcmsP->list.itemN > 0) {
    WARN("rcmSet: expected empty RCMSet (do it on your own risk)\n");
  }
  read_string(fp,str);
  if (0 != strcmp(str,"RCMSET-BEGIN")) {
    ERROR("rcmSetLoad: inconsistent file\n");
    return TCL_ERROR;
  }

  if (NULL == (key.hmmXA = malloc(CTX_MAX*sizeof(PHMMX)))) {
    ERROR("rcmSetLoad: allocation failure\n");
    return TCL_ERROR;
  }

  rcmN= read_int(fp);
  for (rcmX=0;rcmX<rcmN;rcmX++) {
    key.hmmN  = (int) read_int(fp);
    for (hmmX=0;hmmX<key.hmmN;hmmX++) {
      key.hmmXA[hmmX]=(PHMMX) read_int(fp);
    }
    listIndex((List*) &(rcmsP->list), &key, 1);  
  }
  read_string(fp,str);
  if (0 != strcmp(str,"RCMSET-END")) {
    ERROR("rcmSetEnd: inconsistent file\n");
    return TCL_ERROR;
  }

  if (key.hmmXA) free(key.hmmXA);

  return TCL_OK;
}

static int rcmSetLoadItf( ClientData cd, int argc, char *argv[])
{
  RCMSet*    rcmsP = (RCMSet*)cd;
  int        ac    =  argc - 1;
  char*      name  =  NULL;
  FILE*      fp    = NULL;

  if ( itfParseArgv( argv[0], &ac, argv+1, 0,
      "<filename>", ARGV_STRING, NULL, &name, cd, "file to load from",
       NULL) != TCL_OK) return TCL_ERROR;

  fp = fileOpen(name,"r");
  rcmSetLoad(rcmsP,fp);
  fileClose(name,fp);
  return TCL_OK;
}

/* ------------------------------------------------------------------------
    rcmSetSave
   ------------------------------------------------------------------------ */

int rcmSetSave (RCMSet* rcmsP,FILE* fp)
{ 
  int rcmX;
  if (! fp) {
    ERROR("rcmSetSave: Invalid file pointer\n");
    return TCL_ERROR;
  }

  write_string(fp,"RCMSET-BEGIN");
  write_int   (fp,rcmsP->list.itemN); 
  
  for (rcmX=0;rcmX<rcmsP->list.itemN;rcmX++) {
    RCM* rcmP = rcmsP->list.itemA+rcmX;
    int  hmmX;
    write_int(fp,(int) rcmP->mapN); 
    for (hmmX=0;hmmX<rcmP->mapN;hmmX++)
      write_int(fp,(int) rcmP->hmmXA[rcmP->mapXA[hmmX]]);
  }
  write_string(fp,"RCMSET-END");
  fflush(fp);
  return TCL_OK;
}

static int rcmSetSaveItf( ClientData cd, int argc, char *argv[])
{
  RCMSet*    rcmsP = (RCMSet*)cd;
  int        ac    =  argc - 1;
  char*      name  =  NULL;
  FILE*      fp    = NULL;

  if ( itfParseArgv( argv[0], &ac, argv+1, 0,
      "<filename>", ARGV_STRING, NULL, &name, cd, "file to load from",
       NULL) != TCL_OK) return TCL_ERROR;

  fp = fileOpen(name,"w");
  rcmSetSave(rcmsP,fp);
  fileClose(name,fp);
  return TCL_OK;
}

/* ========================================================================
   Left and Right Context Model 
   ======================================================================== */

typedef struct {
  int     hmmL;
  int     hmmR;
  PHMMX** hmmXA;
} XKey;

/* ------------------------------------------------------------------------
    xcmInit
   ------------------------------------------------------------------------ */

static int xcmInit( XCM* xcmP, XKey* hashkey)
{
  int cX=0, cY=0;
  xcmP->hmmMax = 0;
  xcmP->hmmNA  = NULL;
  xcmP->hmmXA  = NULL;
  xcmP->mapL   = hashkey->hmmL;
  xcmP->mapR   = hashkey->hmmR;
  xcmP->mapXA  = NULL;

  if (NULL == (xcmP->mapXA= (CTX**)malloc(xcmP->mapL*sizeof(CTX*)))) {
    ERROR("xcmInit: allocation failure\n");
    return TCL_ERROR;
  }
  if (NULL == (xcmP->mapXA[0]= (CTX*)malloc(xcmP->mapL*xcmP->mapR*sizeof(CTX)))) {
    ERROR("xcmInit: allocation failure\n");
    return TCL_ERROR;
  }
  if (NULL == (xcmP->hmmXA= (PHMMX**)malloc(xcmP->mapL*sizeof(PHMMX*)))) {
    ERROR("xcmInit: allocation failure\n");
    return TCL_ERROR;
  }
  if (NULL == (xcmP->hmmNA= (CTX*)malloc(xcmP->mapL*sizeof(CTX)))) {
    ERROR("xcmInit: allocation failure\n");
    return TCL_ERROR;
  }
  
  for (cX=1;cX<xcmP->mapL;cX++) {
    xcmP->mapXA[cX] = xcmP->mapXA[cX-1] + xcmP->mapR;
  }

  for (cX=0;cX<xcmP->mapL;cX++) {
    if (NULL == (xcmP->hmmXA[cX]= (PHMMX*)malloc(xcmP->mapR*sizeof(PHMMX)))) {
      ERROR("xcmInit: allocation failure\n");
      return TCL_ERROR;
    }
    xcmP->hmmNA[cX] = 0;
    for (cY=0;cY<xcmP->mapR;cY++) {
      int  modelX = 0;
      PHMMX  hmmX = hashkey->hmmXA[cX][cY];

      if (hmmX == PHMM_MAX) {
	xcmP->mapXA[cX][cY]=0;
	continue;
      }

      /* check if we have the model already */
      for (modelX=0;modelX<xcmP->hmmNA[cX];modelX++)
	if (xcmP->hmmXA[cX][modelX] == hmmX) break;

      /* map contextX,contextY -> modelX */
      xcmP->mapXA[cX][cY]=modelX;

      /* map modelX -> hmmX if we have it not yet */
      if (modelX == xcmP->hmmNA[cX]) {
	xcmP->hmmXA[cX][modelX] = hmmX;
	xcmP->hmmNA[cX]++;
      }
    }
    if (xcmP->hmmMax < xcmP->hmmNA[cX])
      xcmP->hmmMax = xcmP->hmmNA[cX];
    if (xcmP->hmmNA[cX] == 0)
      xcmP->hmmXA[cX]= (PHMMX*)realloc((void*)xcmP->hmmXA[cX],1*sizeof(PHMMX)); 
    else
      xcmP->hmmXA[cX]= (PHMMX*)realloc((void*)xcmP->hmmXA[cX],xcmP->hmmNA[cX]*sizeof(PHMMX));    
  }

  /* work arround */
  if (xcmP->hmmNA[0] == 0) {
    cX = 1;
    while (cX<xcmP->mapL && xcmP->hmmNA[cX] == 0) cX++;
    assert (cX < xcmP->mapL && xcmP->hmmNA[cX] > 0);
    xcmP->hmmXA[0][0] =  xcmP->hmmXA[cX][0];
  }
  return TCL_OK;
}

/* ------------------------------------------------------------------------
    xcmDeinit
   ------------------------------------------------------------------------ */

static int xcmDeinit( XCM* xcmP)
{
  if (xcmP->mapXA) {
    if (xcmP->mapXA[0]) free(xcmP->mapXA[0]); 
    free(xcmP->mapXA);
  }

  if (xcmP->hmmXA) {
    int cX;
    for (cX=0;cX<xcmP->mapL;cX++)
      if (xcmP->hmmXA[cX]) free(xcmP->hmmXA[cX]);
    free(xcmP->hmmXA);
  }
  if (xcmP->hmmNA) 
    free(xcmP->hmmNA);

  xcmP->hmmMax = 0;
  xcmP->hmmNA  = NULL;
  xcmP->hmmXA  = NULL;
  xcmP->mapL   = 0;
  xcmP->mapR   = 0;
  xcmP->mapXA  = NULL;

  return TCL_OK;
}

/* ------------------------------------------------------------------------
    xcmPuts
   ------------------------------------------------------------------------ */

static int xcmPutsItf (ClientData clientData, int argc, char *argv[])
{
  XCM*   xcmP = (XCM*)clientData;
  int      ac = argc-1;
  int verbose = 0;
  int    mapX = -1;
  int    mapY = -1;

  if ( itfParseArgv( argv[0], &ac, argv+1, 0,
      "-verbose",  ARGV_INT, NULL, &verbose, NULL, "verbose",
      "-mapX",     ARGV_INT, NULL, &mapX,    NULL, "left context",
      "-mapY",     ARGV_INT, NULL, &mapY,    NULL, "right context",
       NULL) != TCL_OK) return TCL_ERROR;


  itfAppendResult("mapL= %d  mapR= %d  hmmMax= %d\n",xcmP->mapL,xcmP->mapR,xcmP->hmmMax);
  
  if (mapX >= 0 && mapY >= 0) {
    itfAppendElement("%d",xcmP->hmmXA[mapX][xcmP->mapXA[mapX][mapY]]);
     return TCL_OK;    
  }
  if (!verbose) return TCL_OK;

  itfAppendResult("{");
  for (mapX=0; mapX< xcmP->mapL; mapX++) {
    itfAppendElement("leftX= %d  hmmN= %d",mapX,xcmP->hmmNA[mapX]);
    if (verbose > 1) {
      itfAppendResult("{");
      for (mapY=0; mapY< xcmP->mapR; mapY++)
	itfAppendElement("%d",xcmP->hmmXA[mapX][xcmP->mapXA[mapX][mapY]]);
      itfAppendResult(" }\n");
    }
  }
  itfAppendResult(" }\n");
  return TCL_OK;    
}

/* ------------------------------------------------------------------------
    xcm hash function
   ------------------------------------------------------------------------ */

static int xcmHashCmp( const XKey* key, const XCM* xcmP)
{
  PHMMX** hmmXA = xcmP->hmmXA;
  CTX**   mapXA = xcmP->mapXA;
  int hmmX,hmmY;

  if (hmmXA[0][mapXA[0][0]] != key->hmmXA[0][0]) return 0;
  if (xcmP->mapL            != key->hmmL )       return 0;
  if (xcmP->mapR            != key->hmmR )       return 0;

  for (hmmX=0;hmmX<xcmP->mapL;hmmX++)
    for (hmmY=0;hmmY<xcmP->mapR;hmmY++)
      if (hmmXA[hmmX][mapXA[hmmX][hmmY]] != key->hmmXA[hmmX][hmmY]) return 0;

  return 1;
}

#define xcmBit 5

static long xcmHashKey( const XKey* key)
{
  unsigned long ret = 0; 
  int hmmX,hmmY;

  for (hmmX=0; hmmX<key->hmmL; hmmX++) 
    for (hmmY=0; hmmY<key->hmmR; hmmY++) 
      ret = ((ret<<xcmBit) | (ret >> (8*sizeof(long)-xcmBit))) \
	^ key->hmmXA[hmmX][hmmY];
  return ret;
}


/* ========================================================================
    Set of Left and Right Context Models 
   ======================================================================== */

/* ------------------------------------------------------------------------
    xcmSetInit
   ------------------------------------------------------------------------ */

static int xcmSetInit (XCMSet *xcmsP, char* name, PHMMSet* hmmsP, int ignoreRCM)
{

  if (!hmmsP) {
    ERROR("xcmSetInit: invalid PHMMSet\n");
    return TCL_ERROR;
  }
  if (!hmmsP->snsP->phones) {
    ERROR ("xcmSetInit: No phones in %s\n", hmmsP->snsP->name);
    return TCL_ERROR;
  }
  if (hmmsP->snsP->phones->list.itemN > CTX_MAX) {
    ERROR("xcmSetInit: Number of phones (%d) more than %d\n",
	  hmmsP->snsP->phones->list.itemN,CTX_MAX);
    return TCL_ERROR;
  }

  xcmsP->name      = strdup(name);
  xcmsP->useN      = 0;
  xcmsP->hmmsP     = hmmsP;
  xcmsP->ignoreRCM = ignoreRCM;
  link(xcmsP->hmmsP,"PHMMSet");

  listInit((List*)&(xcmsP->list),(TypeInfo*) &xcmInfo, sizeof(XCM), 
                    xcmSetDefault.list.blkSize);

  xcmsP->list.init    = (ListItemInit  *)xcmInit;
  xcmsP->list.deinit  = (ListItemDeinit*)xcmDeinit;
  xcmsP->list.hashKey = (HashKeyFn     *)xcmHashKey;
  xcmsP->list.hashCmp = (HashCmpFn     *)xcmHashCmp;

  return TCL_OK;
}

/* ------------------------------------------------------------------------
    xcmSetCreate
   ------------------------------------------------------------------------ */

XCMSet* xcmSetCreate (char* name, PHMMSet* hmmsP,int ignoreRCM)
{
  XCMSet *xcmsP = (XCMSet*) malloc(sizeof(XCMSet));

  if (! xcmsP) {
    ERROR("phmmSetCreate: allocation failure\n");
    return NULL;
  }
  if (TCL_ERROR == (xcmSetInit(xcmsP,name,hmmsP,ignoreRCM))) {    
    free(xcmsP);
    return NULL;
  }
  return xcmsP;
}

static ClientData xcmSetCreateItf( ClientData clientData, 
                                      int argc, char *argv[])
{
  int          ac    =  argc;
  PHMMSet*     hmmsP =  NULL;
  int      ignoreRCM = 0;
  char       *name = NULL; 

  if ( itfParseArgv( argv[0], &ac, argv, 0,
      "<name>",  ARGV_STRING, NULL, &name, NULL,         "name of the XCM set",
      "<PHMMSet>" ,ARGV_OBJECT, NULL, &hmmsP,     "PHMMSet", "phone HMM Set",
     "-ignoreRCM", ARGV_INT,    NULL, &ignoreRCM, NULL,      "ignore right context dependency",
       NULL) != TCL_OK) return NULL;
  
  return (ClientData) xcmSetCreate(name,hmmsP,ignoreRCM);
}

/* ------------------------------------------------------------------------
    xcmSetDeinit
   ------------------------------------------------------------------------ */

static int xcmSetDeinit (XCMSet *xcmsP)
{
  if (xcmsP->useN >0) {
    ERROR("xcmSet '%s' still in use by other objects.\n", 
            xcmsP->name); 
    return TCL_ERROR;
  }
  delink(xcmsP->hmmsP,"PHMMSet");
  if (xcmsP->name) free(xcmsP->name);

  xcmsP->name  = NULL;
  xcmsP->hmmsP = NULL;
  return listDeinit((List*)&(xcmsP->list));
}

/* ------------------------------------------------------------------------
    xcmSetFree
   ------------------------------------------------------------------------ */

int xcmSetFree(XCMSet *xcmsP)
{
  if ( xcmSetDeinit(xcmsP) != TCL_OK) return TCL_ERROR;
  free(xcmsP);
  return TCL_OK;
}

static int xcmSetFreeItf (ClientData clientData)
{
  return xcmSetFree((XCMSet*)clientData);
}

/* ------------------------------------------------------------------------
    xcmSetPuts
   ------------------------------------------------------------------------ */

static int xcmSetPutsItf (ClientData clientData, int argc, char *argv[])
{
  XCMSet* xcmsP = (XCMSet*)clientData;
  itfAppendResult("%d",xcmsP->list.itemN);
  return TCL_OK;    
}

/* ------------------------------------------------------------------------
    xcmSetAccessItf
   ------------------------------------------------------------------------ */

static ClientData xcmSetAccessItf (ClientData cd, char *name, TypeInfo **ti)
{ 
  XCMSet *xcmsP = (XCMSet*)cd;

  if (*name == '.') { 
    if (name[1]=='\0') {
      itfAppendElement( "phmmSet");
      *ti = NULL; return NULL;
    }
    else { 
      if (! strcmp( name+1, "phmmSet")) {
        *ti = itfGetType("PHMMSet");
        return (ClientData)xcmsP->hmmsP;
      }
    }
  }
  else if (*name == ':') { 
    if (name[1]=='\0') { 
      itfAppendElement( "0 %d",xcmsP->list.itemN-1);
      *ti = NULL; return NULL;
    }
    else { 
      int i = atoi(name+1);
      *ti = &xcmInfo;
      if (i>=0 && i<xcmsP->list.itemN) 
           return (ClientData) &(xcmsP->list.itemA[i]);
      else return NULL;
    }
  }
  return NULL;
}

/* ------------------------------------------------------------------------
    xcmSetAdd
   ------------------------------------------------------------------------ */

int xcmSetAdd (XCMSet* xcmsP,CTX hmmL,CTX hmmR,PHMMX** hmmXA)
{ 
  XKey  key;
  int idx;

  if (hmmL == 0 || hmmR== 0 || hmmXA == NULL) {
    ERROR("xcmSetAdd:invalid hmmN or hmmXA\n");
    return -1;
  }

  /* create model, if we don't have it yet*/
  key.hmmL  = hmmL;
  key.hmmR  = hmmR;
  key.hmmXA = hmmXA;
  idx = listIndex((List*) &(xcmsP->list), &key, 1);  
  return idx;
}

/* ------------------------------------------------------------------------
    xcmSetLoad
   ------------------------------------------------------------------------ */

int xcmSetLoad (XCMSet* xcmsP,FILE* fp)
{ 
  int xcmMax = xcmsP->hmmsP->snsP->phones->list.itemN;
  char str[1024];
  XKey key;
  int hmmX,hmmY,xcmX,xcmN;

  if (! fp) {
    ERROR("xcmSetLoad: Invalid file pointer\n");
    return TCL_ERROR;
  }
  if (xcmsP->list.itemN > 0) {
    WARN("xcmSet: expected empty XCMSet (do it on your own risk)\n");
  }
  read_string(fp,str);
  if (0 != strcmp(str,"XCMSET-BEGIN")) {
    ERROR("xcmSetLoad: inconsistent file\n");
    return TCL_ERROR;
  }

  if (NULL == (key.hmmXA = malloc(xcmMax*sizeof(PHMMX*)))) {
    ERROR("xcmSetLoad: allocation failure\n");
    return TCL_ERROR;
  }
  if (NULL == (key.hmmXA[0] = malloc(xcmMax*xcmMax*sizeof(PHMMX)))) {
    ERROR("xcmSetLoad: allocation failure\n");
    return TCL_ERROR;
  }
  for (hmmX=1;hmmX<xcmMax;hmmX++) 
    key.hmmXA[hmmX]= key.hmmXA[hmmX-1]+xcmMax;

  xcmN= read_int(fp);
  for (xcmX=0;xcmX<xcmN;xcmX++) {
    key.hmmL  = (int) read_int(fp);
    key.hmmR  = (int) read_int(fp);
    for (hmmX=0;hmmX<key.hmmL;hmmX++) {
      for (hmmY=0;hmmY<key.hmmR;hmmY++) {
	key.hmmXA[hmmX][hmmY]=(PHMMX) read_int(fp);
      }
    }
    listIndex((List*) &(xcmsP->list), &key, 1);  
  }
  read_string(fp,str);
  if (0 != strcmp(str,"XCMSET-END")) {
    ERROR("xcmSetEnd: inconsistent file\n");
    return TCL_ERROR;
  }

  if (key.hmmXA) {
    if (key.hmmXA[0]) free(key.hmmXA[0]);
    free(key.hmmXA);
  }

  return TCL_OK;
}

static int xcmSetLoadItf( ClientData cd, int argc, char *argv[])
{
  XCMSet*    xcmsP = (XCMSet*)cd;
  int        ac    =  argc - 1;
  char*      name  =  NULL;
  FILE*      fp    = NULL;

  if ( itfParseArgv( argv[0], &ac, argv+1, 0,
      "<filename>", ARGV_STRING, NULL, &name, cd, "file to load from",
       NULL) != TCL_OK) return TCL_ERROR;

  fp = fileOpen(name,"r");
  xcmSetLoad(xcmsP,fp);
  fileClose(name,fp);
  return TCL_OK;
}

/* ------------------------------------------------------------------------
    xcmSetSave
   ------------------------------------------------------------------------ */

int xcmSetSave (XCMSet* xcmsP,FILE* fp)
{ 
  int padPhone = xcmsP->hmmsP->treeP->questions.padPhoneX;

  int xcmX;
  if (! fp) {
    ERROR("xcmSetSave: Invalid file pointer\n");
    return TCL_ERROR;
  }

  write_string(fp,"XCMSET-BEGIN");
  write_int   (fp,xcmsP->list.itemN); 
  
  for (xcmX=0;xcmX<xcmsP->list.itemN;xcmX++) {
    XCM* xcmP = xcmsP->list.itemA+xcmX;
    int  hmmX,hmmY;
    write_int(fp,(int) xcmP->mapL); 
    write_int(fp,(int) xcmP->mapR); 
    for (hmmX=0;hmmX<xcmP->mapL;hmmX++)
      for (hmmY=0;hmmY<xcmP->mapR;hmmY++) {
	if (hmmX == padPhone || hmmY == padPhone)
	  write_int(fp,PHMM_MAX);
	else
	  write_int(fp,(int) xcmP->hmmXA[hmmX][xcmP->mapXA[hmmX][hmmY]]);
      }
  }
  write_string(fp,"XCMSET-END");
  fflush(fp);
  return TCL_OK;
}

static int xcmSetSaveItf( ClientData cd, int argc, char *argv[])
{
  XCMSet*    xcmsP = (XCMSet*)cd;
  int        ac    =  argc - 1;
  char*      name  =  NULL;
  FILE*      fp    = NULL;

  if ( itfParseArgv( argv[0], &ac, argv+1, 0,
      "<filename>", ARGV_STRING, NULL, &name, cd, "file to load from",
       NULL) != TCL_OK) return TCL_ERROR;

  fp = fileOpen(name,"w");
  xcmSetSave(xcmsP,fp);
  fileClose(name,fp);
  return TCL_OK;
}

/* ========================================================================
    Type Information
   ======================================================================== */

Method lcmMethod[] = 
{
  { "puts", lcmPutsItf, "displays the array of PHMM indices" }, 
  { NULL, NULL, NULL }
} ;

TypeInfo lcmInfo = { 
        "LCM", 0, 0, lcmMethod, 
        NULL, NULL, NULL, NULL, 
        itfTypeCntlDefaultNoLink,
        "left context model" 
};

Method lcmSetMethod[] = 
{
  { "puts", lcmSetPutsItf, "displays the set of left context models" }, 
  { "load", lcmSetLoadItf, "load a set of left context models" },
  { "save", lcmSetSaveItf, "save a set of left context models" },
  { NULL, NULL, NULL }
} ;

TypeInfo lcmSetInfo = 
{ "LCMSet", 0, -1,  lcmSetMethod, 
   lcmSetCreateItf, lcmSetFreeItf, 
   NULL, lcmSetAccessItf, NULL,
  "set of left context models"
};

Method rcmMethod[] = 
{ 
  { "puts", rcmPutsItf, "displays the array of PHMM indices" },
  { NULL, NULL, NULL }
} ;

TypeInfo rcmInfo = { 
        "RCM", 0, 0, rcmMethod, 
        NULL, NULL, NULL, NULL, 
        itfTypeCntlDefaultNoLink,
        "right context model" };

Method rcmSetMethod[] = 
{
  { "puts", rcmSetPutsItf, "displays the set of right context models" }, 
  { "load", rcmSetLoadItf, "load a set of right context models" },
  { "save", rcmSetSaveItf, "save a set of right context models" },
  { NULL, NULL, NULL }
} ;

TypeInfo rcmSetInfo = 
{ "RCMSet", 0, -1,  rcmSetMethod, 
   rcmSetCreateItf, rcmSetFreeItf, 
   NULL, rcmSetAccessItf, NULL,
  "set of right context models"
};

Method xcmMethod[] = 
{ 
  { "puts", xcmPutsItf, "displays the array of PHMM indices" },
  { NULL, NULL, NULL }
} ;

TypeInfo xcmInfo = { 
        "XCM", 0, 0, xcmMethod, 
        NULL, NULL, NULL, NULL, 
        itfTypeCntlDefaultNoLink,
        "left and right context model" };

Method xcmSetMethod[] = 
{
  { "puts", xcmSetPutsItf, "displays the set of left/right context models" }, 
  { "load", xcmSetLoadItf, "load a set of left/right context models" },
  { "save", xcmSetSaveItf, "save a set of left/right context models" },
  { NULL, NULL, NULL }
} ;

TypeInfo xcmSetInfo = 
{ "XCMSet", 0, -1,  xcmSetMethod, 
   xcmSetCreateItf, xcmSetFreeItf, 
   NULL, xcmSetAccessItf, NULL,
  "set of left/right context models"
};

static int xhmmInitialized = 0;

int XHMM_Init (void)
{
  if (! xhmmInitialized) {
    if ( PHMM_Init()    != TCL_OK) return TCL_ERROR;
    if ( Senones_Init() != TCL_OK) return TCL_ERROR;
    if ( Tree_Init()    != TCL_OK) return TCL_ERROR;
    if ( Topo_Init()    != TCL_OK) return TCL_ERROR;
    if ( Tm_Init()      != TCL_OK) return TCL_ERROR;

    lcmSetDefault.name            = NULL;
    lcmSetDefault.useN            = 0;
    lcmSetDefault.list.itemN      = 0;
    lcmSetDefault.list.blkSize    = 100;
    lcmSetDefault.hmmsP           = NULL;

    rcmSetDefault.name            = NULL;
    rcmSetDefault.useN            = 0;
    rcmSetDefault.list.itemN      = 0;
    rcmSetDefault.list.blkSize    = 5000;
    rcmSetDefault.hmmsP           = NULL;

    xcmSetDefault.name            = NULL;
    xcmSetDefault.useN            = 0;
    xcmSetDefault.list.itemN      = 0;
    xcmSetDefault.list.blkSize    = 15;
    xcmSetDefault.hmmsP           = NULL;

    itfNewType (&lcmInfo);
    itfNewType (&lcmSetInfo);
    itfNewType (&rcmInfo);
    itfNewType (&rcmSetInfo);
    itfNewType (&xcmInfo);
    itfNewType (&xcmSetInfo);

    xhmmInitialized = 1;
  }
  return TCL_OK;
}
