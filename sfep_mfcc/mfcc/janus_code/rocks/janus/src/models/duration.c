/* ========================================================================
    JANUS      Speech- to Speech Translation
               ------------------------------------------------------------
               Object: Phoneme Set Maintainance Functions
               ------------------------------------------------------------

    Author  :  Ivica Rogina & Michael Finke
    Module  :  phones.c
    Date    :  $Id: duration.c 2390 2003-08-14 11:20:32Z fuegen $
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
    Revision 3.7  2003/08/14 11:20:15  fuegen
    Merged changes on branch jtk-01-01-15-fms (jaguar -> ibis-013)

    Revision 3.6.4.2  2002/08/27 08:43:20  metze
    itfParseArgv now uses <name> and NULL initialization for char*

    Revision 3.6.4.1  2002/06/26 11:57:56  fuegen
    changes for new directory structure support and code cleaning

    Revision 3.6  2001/01/15 09:49:56  janus
    Merged Christian's version with WINDOWS support (branch jtk-00-11-14-fuegen)

    Revision 3.5.4.1  2001/01/12 15:16:51  janus
    necessary changes for running janus under WINDOWS

    Revision 3.5  2000/11/14 12:01:11  janus
    Merged branch jtk-00-10-27-jmcd.

    Revision 3.4.34.2  2000/11/08 17:08:06  janus
    Finished making changes required for compilation under 'gcc -Wall'.

    Revision 3.4.34.1  2000/11/06 10:50:27  janus
    Made changes to enable compilation under -Wall.

    Revision 3.4  2000/08/16 09:16:53  soltau
    free -> Tcl_Free

    Revision 3.3  1997/07/31 16:49:00  rogina
    made code gcc -DFKIclean

    Revision 3.2  1997/06/17 17:41:43  westphal
    *** empty log message ***

 * Revision 1.6  96/02/26  14:30:46  finkem
 * chnaged makeAccus to createAccus
 * 
 * Revision 1.5  1996/02/17  19:25:21  finkem
 * handles durX = -1 in accu function too
 *
 * Revision 1.4  1996/02/14  15:55:14  rogina
 * added scaling factor (damn, yet another sick parameter)
 *
 * Revision 1.3  1996/02/14  04:01:20  rogina
 * added distance stuff plus minor beautifications
 *
 * Revision 1.2  1996/02/13  16:56:38  rogina
 * fixed static warning
 *
 * Revision 1.1  1996/02/13  04:51:21  rogina
 * Initial revision
 *

   ======================================================================== */


char durationRCSVersion[]= "@(#)1$Id: duration.c 2390 2003-08-14 11:20:32Z fuegen $";


#include "common.h"
#include "itf.h"
#include "duration.h"

#include "hmm.h"
#include "math.h"
#include "mach_ind_io.h"

/* ------------------------------------------------------------------------
    Forward Declaration
   ------------------------------------------------------------------------ */

extern TypeInfo durationInfo;
extern TypeInfo durationSetInfo;

/* ========================================================================
    Duration
   ======================================================================== */

int durInit (Duration *dur, ClientData cd)
{
  dur->name  = strdup((char*)cd);
  dur->count = 0;
  dur->valN  = 0;
  dur->val   = NULL;
  dur->accu  = NULL;
  return TCL_OK;
}

int durDeinit (Duration *dur)
{
  if (dur->name) { free(dur->name); dur->name = NULL; }
  if (dur->val)  { free(dur->val);  dur->val  = NULL; }
  if (dur->accu) { free(dur->accu); dur->accu = NULL; }
  dur->count = 0;
  dur->valN  = 0;
  return TCL_OK;
}

static int durPutsItf (ClientData clientData, int argc, char *argv[]) 
{
  Duration *dur = (Duration*)clientData;
  int       valX;

  itfAppendResult("{ ");
  for (valX=0; valX<dur->valN; valX++)
      itfAppendResult("%f ", (float)exp(-1.0*dur->val[valX]));
  itfAppendResult("}");
  return TCL_OK;
}

static int durConfigureItf(ClientData cd, char *var, char *val)
{
  Duration *dur = (Duration*)cd;

  if (var==NULL) {
    ITFCFG(durConfigureItf,cd,"count");
    ITFCFG(durConfigureItf,cd,"valN");
    ITFCFG(durConfigureItf,cd,"name");
    return TCL_OK;
  }
  ITFCFG_Float  (var,val,"count", dur->count, 0);
  ITFCFG_Int    (var,val,"valN",  dur->valN,  1);
  ITFCFG_CharPtr(var,val,"name",  dur->name,  1);

  ERROR("unknown option '-%s %s'.\n", var, val ? val : "?");
  return TCL_ERROR;
}

/* ========================================================================
    DurationSet
   ======================================================================== */

static ClientData durSetCreateItf( ClientData clientData, 
                                   int argc, char *argv[]);

static DurationSet durSetDefault;

/* ------------------------------------------------------------------------
    Create/Init DurationSet object
   ------------------------------------------------------------------------ */

int durSetInit (DurationSet *durSet, char *name, IArray *map)
{
  int itemX, durX = 0;

  durSet->commentChar = durSetDefault.commentChar;
  durSet->probFloor   = durSetDefault.probFloor;
  durSet->minCount    = durSetDefault.minCount;
  durSet->momentum    = durSetDefault.momentum;

  if (map->itemN<=0) { ERROR("empty duration map"); return TCL_ERROR; }

  for (itemX=1; itemX<map->itemN; itemX++)
    if (map->itemA[itemX]<=map->itemA[itemX-1])
    {  ERROR("duration map is not ascending\n"); return TCL_ERROR; }

  for (itemX=0; itemX<map->itemN; itemX++)
    if (map->itemA[itemX]<1) 
    {  ERROR("duration map contains non-positive value"); return TCL_ERROR; }

  iarrayInit(&(durSet->map),map->itemA[map->itemN-1]+1);
  durSet->range     = (float*)malloc(durSet->map.itemN * sizeof(float));
  durSet->rangeLen  =   (int*)malloc(map->itemN        * sizeof(int));

  for (itemX=0; itemX<map->itemN; itemX++)
  {
    for ( ; durX <= map->itemA[itemX]; durX++)
    { durSet->map.itemA[durX] = itemX;
      durSet->range[durX] = -1.0 * log (1.0 * map->itemA[itemX] - (itemX==0 ? -1 : map->itemA[itemX-1]));
    }
    durSet->rangeLen[itemX] = map->itemA[itemX] - (itemX==0 ? -1 : map->itemA[itemX-1]);
  }
  durSet->name   = strdup(name);
  durSet->useN   = 0;
  durSet->valN   = map->itemN;

  listInit((List*)&(durSet->list), &durationInfo, sizeof(Duration),
            durSetDefault.list.blkSize);

  durSet->list.init   = (ListItemInit  *)durInit;
  durSet->list.deinit = (ListItemDeinit*)durDeinit;

  return TCL_OK;
}

DurationSet *durSetCreate (char* name, IArray *map)
{
  DurationSet *durSet = (DurationSet*)malloc(sizeof(DurationSet));

  if (! durSet || durSetInit (durSet, name, map) != TCL_OK) {
     if (durSet) free(durSet);
     SERROR("Failed to allocate DurationSet object '%s'.\n", name);
     return NULL; 
  }
  return durSet;
}

static ClientData durSetCreateItf( ClientData clientData, 
                                   int argc, char *argv[]) 
{ 
  int ac = argc - 1;
  IArray map = {0, 0};

  if ( itfParseArgv( argv[0], &ac, argv+1, 1,
      "<map>",ARGV_IARRAY,NULL,&map,NULL,"duration to histogram mapping",
       NULL) != TCL_OK) return NULL;

  return (ClientData)durSetCreate(argv[0],&map);

}

/* ------------------------------------------------------------------------
    Free/Deinit DurationSet Object
   ------------------------------------------------------------------------ */

static int durSetLinkN (DurationSet *durSet)
{
  int  useN = listLinkN((List*)&(durSet->list)) - 1;
  if ( useN < durSet->useN) return durSet->useN;
  else                      return useN;
}

int durSetDeinit (DurationSet *durSet)
{
  if (durSetLinkN(durSet)) {
    SERROR("DurationSet '%s' still in use by other objects.\n",durSet->name);
    return TCL_ERROR;
  }
  if (durSet->name)     { free(durSet->name);     durSet->name     = NULL; }
  if (durSet->range)    { free(durSet->range);    durSet->range    = NULL; }
  if (durSet->rangeLen) { free(durSet->rangeLen); durSet->rangeLen = NULL; }
  iarrayDeinit(&(durSet->map));
  return listDeinit((List*)&(durSet->list));
}

int durSetFree(DurationSet* durSet)
{
  if (durSetDeinit(durSet) != TCL_OK) return TCL_ERROR;
  free(durSet);
  return TCL_OK;
}

static int durSetFreeItf (ClientData clientData) 
{ return durSetFree((DurationSet*)clientData); }

/* ------------------------------------------------------------------------
    durSetConfigureItf
   ------------------------------------------------------------------------ */

static int durSetConfigureItf(ClientData cd, char *var, char *val)
{
  DurationSet *durSet = (DurationSet*)cd;
  if (!durSet) durSet = &durSetDefault;

  if (var==NULL) {
    ITFCFG(durSetConfigureItf,cd,"useN");
    ITFCFG(durSetConfigureItf,cd,"commentChar");
    ITFCFG(durSetConfigureItf,cd,"map");
    ITFCFG(durSetConfigureItf,cd,"floor");
    ITFCFG(durSetConfigureItf,cd,"minCount");
    ITFCFG(durSetConfigureItf,cd,"momentum");
    return listConfigureItf((ClientData)&(durSet->list), var, val);
  }
  ITFCFG_Int(    var,val,"useN",        durSet->useN,       1);
  ITFCFG_Char(   var,val,"commentChar", durSet->commentChar,0);
  ITFCFG_Float(  var,val,"floor",       durSet->probFloor,  0);
  ITFCFG_Float(  var,val,"minCount",    durSet->minCount,   0);
  ITFCFG_Float(  var,val,"momentum",    durSet->momentum,   0);

  if (strcmp(var,"map")==0)
  {
    int i;

    itfAppendResult("{ ");
    for (i=0; i<durSet->map.itemN; i++)
        if (i>0 && durSet->map.itemA[i]!=durSet->map.itemA[i-1]) itfAppendResult("%d ",i-1);
    itfAppendResult("%d }",i-1);
    return TCL_OK;
  }

  return listConfigureItf((ClientData)&(durSet->list), var, val);   
}

/* ------------------------------------------------------------------------
    durSetAccessItf
   ------------------------------------------------------------------------ */

static ClientData durSetAccessItf (ClientData cd, char *name, TypeInfo **ti)
{ 
  DurationSet* durSet = (DurationSet*)cd;
  return listAccessItf((ClientData)&(durSet->list),name,ti);
}

/* ------------------------------------------------------------------------
    durSetPuts
   ------------------------------------------------------------------------ */

static int durSetPutsItf (ClientData cd, int argc, char *argv[]) 
{
  DurationSet* durSet = (DurationSet*)cd;
  return listPutsItf((ClientData)&(durSet->list), argc, argv);
}

/* ------------------------------------------------------------------------
    durSetIndex
   ------------------------------------------------------------------------ */

int durSetIndex (DurationSet* durSet,  char* name) 
{
  return listIndex((List*)&(durSet->list), name, 0);
}

static int durSetIndexItf (ClientData cd, int argc, char *argv[] )
{
  DurationSet* durSet = (DurationSet*)cd;
  return listName2IndexItf((ClientData)&(durSet->list), argc, argv);
}

/* ------------------------------------------------------------------------
    durSetName
   ------------------------------------------------------------------------ */

char* durSetName (DurationSet* durSet, int i) 
{ 
  return (i>=0 && i<durSet->list.itemN) ? durSet->list.itemA[i].name : "(null)";  
}

static int durSetNameItf (ClientData cd, int argc, char *argv[])
{
  DurationSet* durSet = (DurationSet*)cd;
  return listIndex2NameItf((ClientData)&(durSet->list), argc, argv);
}

/* ------------------------------------------------------------------------
    durSetAdd
   ------------------------------------------------------------------------ */

int durSetAdd (DurationSet* durSet, char* name, FArray *val, float count)
{
  int valX, valN;
  int idx = listIndex((List*)&(durSet->list), name, 1);
  float probSum = 0.0;

  valN = durSet->valN;
  if (val->itemN != valN) 
  {  ERROR("need %d floats, got %d\n",valN,val->itemN); return TCL_ERROR; }
  durSet->list.itemA[idx].val  = (float*)malloc(sizeof(float)*valN);

  for (valX=0; valX<valN; valX++) 
  {
    if (val->itemA[valX] <= 0) val->itemA[valX] = durSet->probFloor;
    probSum += val->itemA[valX];
  }

  if (probSum < 0.9 || probSum > 1.1) 
     WARN("probabilities add up to %f. Will normalize.\n",probSum);

  for (valX=0; valX<valN; valX++) 
    durSet->list.itemA[idx].val[valX] = -1.0 * log(val->itemA[valX]/probSum);

  durSet->list.itemA[idx].valN = valN;
  durSet->list.itemA[idx].count = count;
  return TCL_OK;
}

static int durSetAddItf (ClientData cd, int argc, char *argv[])
{
  DurationSet* durSet = (DurationSet*)cd;
  int     ac     =  argc-1;
  char   *name   =  NULL;
  float   count = 0;
  FArray  prob = {0, 0};

  if (itfParseArgv(argv[0], &ac, argv+1, 0,
	  "<durModel>",ARGV_STRING,NULL,&name, NULL,"name of duration models",
	  "<probs>",   ARGV_FARRAY,NULL,&prob, NULL,"array of probabilities",
	  "-count",    ARGV_FLOAT, NULL,&count,NULL,"total occurence count",
          NULL) != TCL_OK)
    return TCL_ERROR;

  durSetAdd (durSet, name, &prob, count);
  farrayDeinit(&prob);
  return TCL_OK;
}

/* ------------------------------------------------------------------------
    durSetDelete
   ------------------------------------------------------------------------ */

static int durSetDelete (DurationSet *durSet, int argc, char* argv[])
{
  int i;
  for ( i = 0; i < argc; i++) {
    if ( listDelete((List*)&(durSet->list), argv[i]) != TCL_OK) {
      SERROR( "Can't remove duration '%s' from '%s'.\n", argv[i], durSet->name);
      return TCL_ERROR;
    }
  }
  return TCL_OK;
}

static int durSetDeleteItf (ClientData cd, int argc, char *argv[])
{
  DurationSet* durSet = (DurationSet*)cd;
  int     retCode =  TCL_OK;
  int     ac      =  argc-1;
  int     i;

  if (itfParseArgv(argv[0], &ac, argv+1, 0,
		   "<durModel*>",ARGV_REST,NULL,&i,NULL,"list of duration models",
		   NULL) != TCL_OK)
    return TCL_ERROR;

  while ( i++ < ac) {
    int     tokc;
    char  **tokv;

    if ( Tcl_SplitList (itf,argv[i],&tokc,&tokv) != TCL_OK) return TCL_ERROR;

    if (durSetDelete(durSet, tokc, tokv) != TCL_OK) retCode = TCL_ERROR;
    Tcl_Free((char*)tokv);
  }
  return retCode;
}

/* ------------------------------------------------------------------------
    durSetRead
   ------------------------------------------------------------------------ */

static int durSetReadItf (ClientData cd, int argc, char *argv[] )
{
  DurationSet* durSet = (DurationSet*)cd;
  char*   fname   =  NULL;
  int     ac      =  argc-1;
  int     n;

  if (itfParseArgv(argv[0], &ac, argv+1, 0,
		  "<filename>",ARGV_STRING,NULL,&fname,NULL,
                  "name of DurationSet file", NULL) != TCL_OK)
    return TCL_ERROR;

  itfAppendResult("%d", n=freadAdd(cd, fname, durSet->commentChar,durSetAddItf));

  return (n >= 0) ? TCL_OK : TCL_ERROR;
}

/* ------------------------------------------------------------------------
    durSetWrite
   ------------------------------------------------------------------------ */

static void durSetWriteFile(DurationSet *durSet, FILE *fp)
{
  Duration* dur = durSet->list.itemA;
  int      durN = durSet->list.itemN;

  for ( ; durN > 0; durN--, dur++) 
  {   int valX;
      fprintf(fp,"%s { ",dur->name);
      for (valX=0; valX<durSet->valN; valX++)
	  fprintf(fp,"% e ",exp(-1.0*dur->val[valX]));
      fprintf(fp,"} -count %f\n",dur->count);
  }
}

int durSetWrite (DurationSet *durSet, char *filename)
{
  FILE *fp;

  if ((fp=fileOpen(filename,"w"))==NULL) {
    ERROR( "Can't open DurationSet file '%s' for writing.\n",filename);
    return TCL_ERROR;
  }

  fprintf( fp, "%c -------------------------------------------------------\n",
                durSet->commentChar);
  fprintf( fp, "%c  Name            : %s\n",  durSet->commentChar, 
                durSet->name);
  fprintf( fp, "%c  Type            : DurationSet\n", durSet->commentChar);
  fprintf( fp, "%c  Number of Items : %d\n",  durSet->commentChar, 
                durSet->list.itemN);
  fprintf( fp, "%c  Date            : %s", durSet->commentChar,
                dateString());
  fprintf( fp, "%c -------------------------------------------------------\n",
                durSet->commentChar);

  durSetWriteFile (durSet,fp);
  fileClose(filename,fp);
  return TCL_OK;
}

static int durSetWriteItf (ClientData cd, int argc, char *argv[] )
{
  char   *fname   =  NULL;
  int     ac      =  argc-1;

  if (itfParseArgv(argv[0], &ac, argv+1, 0,
		  "<filename>",ARGV_STRING,NULL,&fname,NULL,
                  "name of DurationSet file", NULL) != TCL_OK)
    return TCL_ERROR;

  return durSetWrite( (DurationSet*)cd, fname);
}


/* ------------------------------------------------------------------------
    durSetScore
   ------------------------------------------------------------------------ */

float durSetScore (DurationSet *durSet, int durX, int dur)
{
  return 

   (dur<0 || dur >= durSet->map.itemN || durX<0 || durX >= durSet->list.itemN)

    ? -1.0*log(durSet->probFloor)

    : durSet->list.itemA[durX].val[durSet->map.itemA[dur]] - durSet->range[dur];
}

static int durSetProbItf (ClientData clientData, int argc, char *argv[])
{
  DurationSet *durSet = (DurationSet*)clientData;
  int          durX   = -1;
  int          dur    = 0;
  int          ac     = argc-1;

  if (itfParseArgv(argv[0], &ac, argv+1, 0,
      "<durationModel>",ARGV_LINDEX,NULL,&durX,&(durSet->list),"name of duration model", 
      "<durationFrameN>",ARGV_INT  ,NULL,&dur ,NULL           ,"duration in frames", 
      NULL) != TCL_OK)
     return TCL_ERROR;

  if (dur<0) 
  { 
    int i;
    itfAppendResult("{ ");
    for (i=0; i <= -1*dur; i++)
        itfAppendResult("%f ",(float)exp(-1.0*durSetScore(durSet,durX,i)));
    itfAppendResult("}");
  }
  else itfAppendResult("%f",(float)exp(-1.0*durSetScore(durSet,durX,dur)));

  return TCL_OK;
}

/* ------------------------------------------------------------------------
    durSetAccu
   ------------------------------------------------------------------------ */

static int durSetAccu (DurationSet *durSet, Path *path, HMM *hmm, float factor)
{
  int  frX;
  int  phoneX, amodelX, prevAmodelX = -1, durX, valX, durCnt = 0;
  Duration *dur;

  assert (durSet && path && hmm);

  for (frX = 0; frX < path->lastFrame-path->firstFrame+1; frX++) 
  {
    if (path->pitemListA[frX].itemN>1) 
    {  ERROR("can only use Viterbi-made paths"); return TCL_ERROR; }
    if (path->pitemListA[frX].itemN==0) continue;

    phoneX  = path->pitemListA[frX].itemA[0].phoneX;
    amodelX = hmm->phoneGraph->amodel[phoneX];

    if (amodelX != prevAmodelX && prevAmodelX >= 0)
    {
       if ((durX = hmm->phoneGraph->amsP->list.itemA[amodelX].durX) > -1) {

         dur = durSet->list.itemA + durX;
         if (dur->accu && durCnt < durSet->map.itemN) /* ignore outliers */
         {
            valX = durSet->map.itemA[durCnt];
	    dur->accu[valX] += factor;
         }
       }
       durCnt = 0;
    }
    prevAmodelX = amodelX; durCnt++;
  }
  return TCL_OK;
}

static int durSetAccuItf (ClientData clientData, int argc, char *argv[])
{
  DurationSet *durSet = (DurationSet*)clientData;
  Path        *path   =  NULL;
  HMM         *hmm    =  NULL;
  float        factor =  1.0;
  int          ac     =  argc-1;

  if ( itfParseArgv( argv[0], &ac, argv+1, 0,
      "<path>",  ARGV_OBJECT, NULL, &path, "Path", "name of the path object",
      "<hmm>",   ARGV_OBJECT, NULL, &hmm,  "HMM",  "name of the HMM object",
      "-factor", ARGV_FLOAT,  NULL, &factor, 0,    "training factor",
       NULL) != TCL_OK) return TCL_ERROR;

  return durSetAccu(durSet,path,hmm,factor);
}

/* ------------------------------------------------------------------------
    durSetUpdate
   ------------------------------------------------------------------------ */

int durSetUpdate (DurationSet *durSet)
{
  int durX;

  for (durX=0; durX<durSet->list.itemN; durX++)
  {
      int valX; float count = 0.0;
      Duration *dur = durSet->list.itemA + durX;

      for (valX=0; valX<durSet->valN; valX++)
          if (dur->accu[valX] < durSet->probFloor)
	      dur->accu[valX] = durSet->probFloor;

      for (valX=0; valX<durSet->valN; valX++)
          count += dur->accu[valX];

      if ((dur->count = count) < durSet->minCount) continue;

      for (valX=0; valX<durSet->valN; valX++)
	  dur->val[valX] = -1.0 * log (
	         durSet->momentum  * exp (-1.0 * dur->val[valX]) 
	  + (1.0-durSet->momentum) * (dur->accu[valX] / count));
  }
  return TCL_OK;
}

static int durSetUpdateItf (ClientData clientData, int argc, char *argv[])
{
  DurationSet *durSet = (DurationSet*)clientData;
  int ac = argc-1;

  if (itfParseArgv(argv[0], &ac, argv+1, 0, NULL) != TCL_OK) return TCL_ERROR;
  return durSetUpdate (durSet);
}

/* ------------------------------------------------------------------------
    durSetClearAccus
   ------------------------------------------------------------------------ */

static int durSetClearAccus (DurationSet *durSet)
{
  int durX, valX;

  for (durX=0; durX<durSet->list.itemN; durX++)
      if (durSet->list.itemA[durX].accu != NULL)
	 for (valX=0; valX<durSet->valN; valX++)
	     durSet->list.itemA[durX].accu[valX] = 0.0;
  return TCL_OK;
}

static int durSetClearAccusItf (ClientData clientData, int argc, char *argv[])
{
  DurationSet *durSet = (DurationSet*)clientData;
  int ac = argc-1;

  if (itfParseArgv(argv[0], &ac, argv+1, 0, NULL) != TCL_OK) return TCL_ERROR;

  return durSetClearAccus (durSet);
}

/* ------------------------------------------------------------------------
    durSetMakeAccus
   ------------------------------------------------------------------------ */

static int durSetMakeAccus (DurationSet *durSet)
{
  int durX;

  for (durX=0; durX<durSet->list.itemN; durX++)
      if (durSet->list.itemA[durX].accu == NULL)
	  durSet->list.itemA[durX].accu = (double*)malloc(sizeof(double)*durSet->valN);
  return durSetClearAccus(durSet);
}

static int durSetMakeAccusItf (ClientData clientData, int argc, char *argv[])
{
  DurationSet *durSet = (DurationSet*)clientData;
  int ac = argc-1;

  if (itfParseArgv(argv[0], &ac, argv+1, 0, NULL) != TCL_OK) return TCL_ERROR;

  return durSetMakeAccus (durSet);
}

/* ------------------------------------------------------------------------
    durSetFreeAccus
   ------------------------------------------------------------------------ */

static int durSetFreeAccusItf (ClientData clientData, int argc, char *argv[])
{
  /* DurationSet *durSet = (DurationSet*)clientData; */
  ERROR("freeing of duration set accus is not yet available"); return TCL_ERROR;
}

/* ------------------------------------------------------------------------
    durSetSaveAccu
   ------------------------------------------------------------------------ */

static int durSetSaveAccus (DurationSet *durSet, char *filename)
{
  int durX, valX;
  FILE *fp;

  if ((fp=fileOpen(filename,"w"))==NULL)
  {  ERROR("can't open file %s for writing\n",filename); return TCL_ERROR; }

  write_int(fp,durSet->list.itemN);
  write_int(fp,durSet->valN);

  for (durX=0; durX<durSet->list.itemN; durX++)
      if (durSet->list.itemA[durX].accu != NULL)
	 for (valX=0; valX<durSet->valN; valX++)
	     write_float(fp,durSet->list.itemA[durX].accu[valX]);

  fileClose(filename,fp);
  return TCL_OK;
}

static int durSetSaveAccusItf (ClientData clientData, int argc, char *argv[])
{
  char *filename=NULL; int ac=argc-1;

  if ( itfParseArgv( argv[0], &ac, argv+1, 0,
      "<filename>", ARGV_STRING, NULL, &filename, clientData, "file to write",
       NULL) != TCL_OK) return TCL_ERROR;

  return durSetSaveAccus((DurationSet*)clientData,filename); 
}

/* ------------------------------------------------------------------------
    durSetLoadAccu
   ------------------------------------------------------------------------ */

static int durSetLoadAccus (DurationSet *durSet, char *filename)
{
  int durX, valX, itemN, valN;
  FILE *fp;

  if ((fp=fileOpen(filename,"r"))==NULL)
  {  ERROR("can't open file %s for reading\n",filename); return TCL_ERROR; }

  itemN = read_int(fp);
  valN  = read_int(fp);
  
  if (itemN!=durSet->list.itemN || valN!=durSet->valN) 
  {  ERROR("file don't match\n"); return TCL_ERROR; }

  for (durX=0; durX<durSet->list.itemN; durX++)
      if (durSet->list.itemA[durX].accu != NULL)
	 for (valX=0; valX<durSet->valN; valX++)
	     durSet->list.itemA[durX].accu[valX] += read_float(fp);

  fileClose(filename,fp);
  return TCL_OK;
}

static int durSetLoadAccusItf (ClientData clientData, int argc, char *argv[])
{
  char *filename=NULL; int ac=argc-1;

  if ( itfParseArgv( argv[0], &ac, argv+1, 0,
      "<filename>", ARGV_STRING, NULL, &filename, clientData, "file to write",
       NULL) != TCL_OK) return TCL_ERROR;

  return durSetLoadAccus((DurationSet*)clientData,filename); 
}

/* ------------------------------------------------------------------------
    durSetPutsAccu
   ------------------------------------------------------------------------ */

static void durSetPutsAccu (Duration *dur, int valN)
{
  int valX;

  if (dur->accu==NULL) return;

  itfAppendResult("{ %s { ",dur->name);

  for (valX=0; valX<valN; valX++)
      itfAppendResult("% e ",dur->accu[valX]);

  itfAppendResult("} }");  
}

static int durSetPutsAccuItf (ClientData clientData, int argc, char *argv[])
{
  DurationSet *durSet = (DurationSet*)clientData;
  int durX, i, ac=argc-1;
  IArray durs = {0, 0};

  if (ac==0)
  {
     for (durX=0; durX<durSet->list.itemN;  durX++)
       durSetPutsAccu (durSet->list.itemA + durX, durSet->valN);
     return TCL_OK;
  }

  if (itfParseArgv(argv[0], &ac, argv+1, 0,
        "<durationModel*>",ARGV_LIARRAY,NULL,&durs,&(durSet->list),"duration models",
        NULL) != TCL_OK)
  return TCL_ERROR;

  for (i=0; i<durs.itemN; i++)
     durSetPutsAccu (durSet->list.itemA + durs.itemA[i], durSet->valN);

  return TCL_OK;
}

/* ------------------------------------------------------------------------
    durSetScale
   ------------------------------------------------------------------------ */

static int durSetScale (DurationSet *durSet, float factor)
{
  int durX, valX;

  for (durX=0; durX<durSet->list.itemN;  durX++)
      for (valX=0; valX<durSet->valN; valX++)
          durSet->list.itemA[durX].val[valX] *= factor;
  for (valX=0; valX<durSet->map.itemN;  valX++)
      durSet->range[valX] *= factor;
  return TCL_OK;
}

static int durSetScaleItf( ClientData clientData, 
                                  int argc, char *argv[]) 
{ 
  DurationSet *durSet = (DurationSet*)clientData;
  int ac = argc - 1;
  float factor = 1.0;

  if ( itfParseArgv( argv[0], &ac, argv+1, 1,
      "<factor>",ARGV_FLOAT,NULL,&factor,NULL,"multiplicative factor for log-probs",
       NULL) != TCL_OK) return TCL_ERROR;

  return durSetScale(durSet,factor);

}

/* ------------------------------------------------------------------------
    durSetDist
   ------------------------------------------------------------------------ */

static float** durDistA = NULL;
static int     durDistN = 0;

static int durDistAlloc (int N)
{
  int j;

  if (! durDistA || durDistN < N) {
        durDistN    =  N;
        durDistA    = (float**)realloc( durDistA, 4 * sizeof(float*) +
                                        4 *durDistN * sizeof(float));
        durDistA[0] = (float *)(durDistA + 4);
        durDistA[1] =  durDistA[0] + durDistN;
        durDistA[2] =  durDistA[1] + durDistN;
        durDistA[3] =  durDistA[2] + durDistN;
  }
  for ( j = 0; j < 4*durDistN; j++) durDistA[0][j] = 0.0;
  return TCL_OK;
}

static int durDistEntropy (DurationSet* durP, ModelArray* maP, 
                                       ModelArray* maQ,
                                       ModelArray* maR, float* distP)
{
  double count[4] = { 0.0, 0.0, 0.0, 0.0};
  double h[4]     = { 0.0, 0.0, 0.0, 0.0};
  int    i, j, N;

  if ( maP->mdsP.cd != (ClientData)durP || 
       maQ->mdsP.cd != (ClientData)durP || 
       maR->mdsP.cd != (ClientData)durP) {
    ERROR("ModelArrays are not defined over DurationSet %s.\n", durP->name);
    return TCL_ERROR;
  }

  N = durP->valN; durDistAlloc(N); 

  for ( i = 0; i < maP->modelN; i++) {
    Duration* duP = durP->list.itemA + maP->modelA[i];

    for ( j = 0; j < N; j++) {
      float x         = duP->count * exp (-1.0 * duP->val[j]);
      durDistA[0][j] += x;
      durDistA[1][j] += x;
    }
    count[0] += duP->count;
    count[1] += duP->count;
  }

  for ( i = 0; i < maQ->modelN; i++) {
    Duration* duP = durP->list.itemA + maQ->modelA[i];

    for ( j = 0; j < N; j++) {
      float x         = duP->count * exp (-1.0 * duP->val[j]);
      durDistA[0][j] += x;
      durDistA[2][j] += x;
    }
    count[0] += duP->count;
    count[2] += duP->count;
  }

  for ( i = 0; i < maR->modelN; i++) {
    Duration* duP = durP->list.itemA + maR->modelA[i];

    for ( j = 0; j < N; j++) {
      float x         = duP->count * exp (-1.0 * duP->val[j]);
      durDistA[0][j] += x;
      durDistA[3][j] += x;
    }
    count[0] += duP->count;
    count[3] += duP->count;
  }

  for ( i = 0; i < 4; i++) {
    if ( count[i] > 0.0) {
      if ( count[i] < durP->minCount) return TCL_ERROR;
      for ( j = 0; j < N; j++) {
        double x  = (durDistA[i][j] / count[i] / durP->rangeLen[j]);
        h[i]     -= (x < 1e-19) ? 0.0 : (x * log(x) * durP->rangeLen[j]);
      }
    }
  }

  if ( distP) *distP = count[0] * h[0] - count[1] * h[1] - 
                                         count[2] * h[2] -
                                         count[3] * h[3];

  if ( count[1]+count[2] <= 0.0 || count[1]+count[3] <= 0.0 ||
       count[2]+count[3] <= 0.0 || count[0]          <= 0.0)
       return TCL_ERROR;
  else return TCL_OK;
}

static int durSetDist (DurationSet* durP, ModelArray* maP, 
                                   ModelArray* maQ,
                                   ModelArray* maR, float* distP)
{
  if ( maP->mdsP.cd != (ClientData)durP || 
       maQ->mdsP.cd != (ClientData)durP ||
       maQ->mdsP.cd != (ClientData)durP) {

    ERROR("ModelArrays are not defined over DurationSet %s.\n", durP->name);
    return TCL_ERROR;
  }
  return durDistEntropy( durP, maP, maQ, maR, distP);
}

static int durSetDistItf (ClientData cd, int argc, char *argv[])
{
  DurationSet* durSet = (DurationSet*)cd;
  ModelArray*  maP    = NULL;
  ModelArray*  maQ    = NULL;
  ModelArray*  maR    = NULL;
  int          ac     = argc-1;
  float        d      = 0;

  if ( itfParseArgv (argv[0], &ac, argv+1, 0,
      "<ModelArray P>",  ARGV_OBJECT, NULL, &maP, "ModelArray","model array",
      "<ModelArray Q>",  ARGV_OBJECT, NULL, &maQ, "ModelArray","model array",
      "<ModelArray R>",  ARGV_OBJECT, NULL, &maR, "ModelArray","model array",
        NULL) != TCL_OK) return TCL_ERROR;

  if (durSetDist(durSet, maP, maQ, maR, &d) == TCL_OK) {
    itfAppendResult( "%e", d);
    return TCL_OK;
  }
  return TCL_ERROR;
}

static ModelSet durationSetModel = 
             { "DurationSet", &durationSetInfo, &durationInfo,
               (ModelSetIndexFct*)durSetIndex,
               (ModelSetNameFct *)durSetName,
               (ModelSetDistFct *)durSetDist };

/* ========================================================================
    Type Declaration
   ======================================================================== */

static Method durationMethod[] = 
{ 
  { "puts", durPutsItf,   NULL },
  {  NULL,  NULL, NULL } 
};

TypeInfo durationInfo = { "Duration", 0, 0, durationMethod, 
                           NULL, NULL, durConfigureItf, NULL, itfTypeCntlDefaultNoLink,
                          "explicite duration model" };

Method durationSetMethod[] = 
{
  { "puts",   durSetPutsItf,   "displays the contents of a duration set"      },
  { "add",    durSetAddItf,    "add new duration model(s) to a duration set"  },
  { "delete", durSetDeleteItf, "delete duration model(s) from a duration set" },
  { "read",   durSetReadItf,   "read a duration set from a file"              },
  { "write",  durSetWriteItf,  "write a duration set into a file"             },
  { "index",  durSetIndexItf,  "return index of named duration model(s)"      },
  { "name",   durSetNameItf,   "return the name of indexed duration model(s)" },
  { "prob",   durSetProbItf,   "return the duration probability for a named duration model" },
  { "accu",   durSetAccuItf,   "accumulate training data" },
  { "update", durSetUpdateItf, "update the duration probabilities" },
  { "dist",   durSetDistItf,   "measure distance between duration models" },
  { "scale",  durSetScaleItf,  "multiply all log-probs with given value" },

  { "createAccus", durSetMakeAccusItf, "allocate training data accumulators" },
  { "freeAccus",   durSetFreeAccusItf, "allocate training data accumulators" },
  { "loadAccus",   durSetLoadAccusItf, "load training data accumulators from file" },
  { "saveAccus",   durSetSaveAccusItf, "save training data accumulators to file" },
  { "clearAccus",  durSetClearAccusItf,"clear training data accumulators" },
  { "putsAccu",    durSetPutsAccuItf,  "display training data accumulator" },

  { NULL, NULL, NULL }
} ;

TypeInfo durationSetInfo = { "DurationSet", 0, -1, durationSetMethod,
                              durSetCreateItf,    durSetFreeItf, 
                              durSetConfigureItf, durSetAccessItf, 
			      NULL,
  "A 'DurationSet' object is an array of explicite duration models." };

static int durInitialized = 0;

int Duration_Init (void)
{
  if (!durInitialized) {
    durSetDefault.name            = NULL;
    durSetDefault.useN            = 0;
    durSetDefault.list.itemN      = 0;
    durSetDefault.list.blkSize    = 5000;
    durSetDefault.commentChar     = ';';
    durSetDefault.probFloor       = 1e-19;
    durSetDefault.minCount        = 5;
    durSetDefault.map.itemN       = 0;
    durSetDefault.map.itemA       = NULL;

    itfNewType (&durationInfo);
    itfNewType (&durationSetInfo);

    modelSetNewType (&durationSetModel);

    durInitialized = 1;
  }
  return TCL_OK;
}


