/* ========================================================================
    JANUS      Speech- to Speech Translation
               ------------------------------------------------------------
               Object: Topology
               ------------------------------------------------------------

    Author  :  Michael Finke
    Module  :  topo.c
    Date    :  $Id: topo.c 2656 2005-04-25 13:24:22Z metze $
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
    Revision 3.6  2005/04/25 13:24:22  metze
    Correction

    Revision 3.5  2003/08/14 11:20:18  fuegen
    Merged changes on branch jtk-01-01-15-fms (jaguar -> ibis-013)

    Revision 3.4.4.1  2002/08/27 08:40:51  metze
    itfParseArgv now uses <name> and NULL initialization for char*

    Revision 3.4  2001/01/15 09:50:00  janus
    Merged Christian's version with WINDOWS support (branch jtk-00-11-14-fuegen)

    Revision 3.3.4.1  2001/01/12 15:17:00  janus
    necessary changes for running janus under WINDOWS

    Revision 3.3  2000/11/14 12:35:26  janus
    Merging branch jtk-00-10-27-jmcd.

    Revision 3.2.30.2  2000/11/08 17:32:26  janus
    Finished making changes required for compilation under 'gcc -Wall'.

    Revision 3.2.30.1  2000/11/06 10:50:41  janus
    Made changes to enable compilation under -Wall.

    Revision 3.2  1997/06/17 17:41:43  westphal
    *** empty log message ***

    Revision 2.3  96/02/14  16:27:44  rogina
    removed duration stuff (It's hanging in amodel now>)
    
    Revision 2.2  1996/02/13  04:53:23  rogina
    added duration model index

    Revision 2.1  1996/02/11  05:18:35  finkem
    made distance function having 3 params...

    Revision 2.0  1996/01/31  04:34:38  finkem
    New SenoneSet setup & freadAdd function

    Revision 1.11  1996/01/01  16:34:56  finkem
    *** empty log message ***

    Revision 1.10  1996/01/01  16:30:41  finkem
     handling of parsing errors in the Read function
     remove INFO output when reading

    Revision 1.9  1995/11/15  01:00:17  torsten
    added topo write

    Revision 1.8  1995/09/06  07:42:07  kemp
    *** empty log message ***

    Revision 1.7  1995/08/30  20:39:00  finkem
    *** empty log message ***

    Revision 1.6  1995/08/27  22:35:31  finkem
    *** empty log message ***

    Revision 1.5  1995/08/17  18:00:55  rogina
    *** empty log message ***

    Revision 1.4  1995/08/14  13:18:58  finkem
    *** empty log message ***

    Revision 1.3  1995/08/01  13:27:21  finkem
    *** empty log message ***

    Revision 1.2  1995/07/31  17:57:59  rogina
    *** empty log message ***

    Revision 1.1  1995/07/31  14:33:49  finkem
    Initial revision

 
   ======================================================================== */


char topoRCSVersion[]= "@(#)1$Id: topo.c 2656 2005-04-25 13:24:22Z metze $";


#include "common.h"
#include "itf.h"
#include "array.h"
#include "topo.h"
#include "modelSet.h"

/* ========================================================================
    Global Declarations
   ======================================================================== */

extern TypeInfo topoInfo;
extern TypeInfo topoSetInfo;

/* ========================================================================
    Topo
   ======================================================================== */
/* ------------------------------------------------------------------------
    topoInit
   ------------------------------------------------------------------------ */

int topoInit( Topo* TP, char* name)
{
  assert( TP);

  TP->name         =  strdup( name);
  TP->stnXA        =  NULL;
  TP->tmXA         =  NULL;
  TP->tmN          =  0;
  return TCL_OK;
}

/* ------------------------------------------------------------------------
    topoDeinit  deinitialize topology structure
   ------------------------------------------------------------------------ */

int topoDeinit( Topo* TP)
{
  assert( TP);

  if ( TP->name)    { free(TP->name);    TP->name    = NULL; }
  if ( TP->stnXA)   { free(TP->stnXA);   TP->stnXA   = NULL; }
  if ( TP->tmXA)    { free(TP->tmXA);    TP->tmXA    = NULL; }
  TP->tmN    = 0;
  return TCL_OK;
}

/* ------------------------------------------------------------------------
    topoPuts  print info on a single topology
   ------------------------------------------------------------------------ */

static int topoPutsItf (ClientData clientData, int argc, char *argv[])
{
  Topo* TP  = (Topo*)clientData;
  int   tmN =  TP->tmN, i;

  itfAppendResult( "{");
  for ( i=0; i<tmN; i++) itfAppendResult( " %d", TP->stnXA[i]);
  itfAppendResult( "} {");
  for ( i=0; i<tmN; i++) itfAppendResult( " %d", TP->tmXA[i]);
  itfAppendResult( "}");
  return TCL_OK;
}

/* ------------------------------------------------------------------------
    topoConfigureItf
   ------------------------------------------------------------------------ */

static int topoConfigureItf (ClientData cd, char *var, char *val)
{
  Topo *TP = (Topo*)cd;
  if (TP==NULL) return TCL_ERROR;

  if (var==NULL) 
  {  ITFCFG(topoConfigureItf,cd,"name");
     return TCL_OK;
  }
  ITFCFG_CharPtr(var,val,"name",TP->name,1);

  return TCL_ERROR;
}

/* ========================================================================
    TopoSet
   ======================================================================== */

static TopoSet topoSetDefault;

/* ------------------------------------------------------------------------
    Create/Init TopoSet
   ------------------------------------------------------------------------ */

int topoSetInit( TopoSet*   topoSet, char*  name, 
                 SenoneSet* snsP,    TmSet* tmSetP)
{
  topoSet->name        = strdup(name);
  topoSet->useN        = 0;

  listInit((List*)&(topoSet->list), &topoInfo, sizeof(Topo),
                    topoSetDefault.list.blkSize);

  topoSet->list.init    = (ListItemInit  *)topoInit;
  topoSet->list.deinit  = (ListItemDeinit*)topoDeinit;

  topoSet->snsP         = snsP;   link(snsP,  "SenoneSet");
  topoSet->tmSetP       = tmSetP; link(tmSetP,"TmSet");
  topoSet->commentChar  = topoSetDefault.commentChar;

  return TCL_OK;
}

TopoSet *topoSetCreate( char *name, SenoneSet* snsP, TmSet* tmSetP)
{
  TopoSet *topoSet = (TopoSet*)malloc(sizeof(TopoSet));

  if (! topoSet || topoSetInit( topoSet, name, snsP,
                                tmSetP) != TCL_OK) {
     if ( topoSet) free( topoSet);
     ERROR("Failed to allocate TopoSet object '%s'.\n", name);
     return NULL; 
  }

  return topoSet;
}

static ClientData topoSetCreateItf( ClientData clientData, 
                                    int argc, char *argv[]) 
{ 
  SenoneSet*   snsP   = NULL;
  TmSet*       tmSetP = NULL;
  char* name = NULL;

  if ( itfParseArgv( argv[0], &argc, argv, 0,
      "<name>",  ARGV_STRING, NULL, &name, NULL,         "name of the topo set",
      "<SenoneSet>",ARGV_OBJECT,NULL,&snsP,  "SenoneSet","senone set",
      "<TmSet>",    ARGV_OBJECT,NULL,&tmSetP,"TmSet","set of transition models",
       NULL) != TCL_OK) return NULL;

  return (ClientData)topoSetCreate(name,snsP,tmSetP);
}

/* ------------------------------------------------------------------------
    Free TopoSet
   ------------------------------------------------------------------------ */

int topoSetLinkN( TopoSet* topoSet)
{
  int  useN = listLinkN((List*)&(topoSet->list)) - 1;
  if ( useN < topoSet->useN) return topoSet->useN;
  else                       return useN;
}

int topoSetDeinit( TopoSet* topoSet)
{
  if ( topoSetLinkN(topoSet)) {
    SERROR("TopoSet '%s' still in use by other objects.\n", topoSet->name);
    return TCL_ERROR;
  }
  
  if ( topoSet->name)   { free( topoSet->name); topoSet->name = NULL; }
  if ( topoSet->snsP)   { unlink( topoSet->snsP,   "SenoneSet"); 
                          topoSet->snsP = NULL; }
  if ( topoSet->tmSetP) { unlink( topoSet->tmSetP, "TmSet");
                          topoSet->tmSetP = NULL; }

  return listDeinit((List*)&(topoSet->list));
}

int topoSetFree(TopoSet* topoSet)
{
  topoSetDeinit( topoSet);
  free(topoSet);
  return TCL_OK;
}

static int topoSetFreeItf (ClientData clientData)
{
  TopoSet* topoSet = (TopoSet*)clientData;
  return topoSetFree( topoSet);
}

/* ------------------------------------------------------------------------
    topoSetConfigureItf
   ------------------------------------------------------------------------ */

static int topoSetConfigureItf(ClientData cd, char *var, char *val)
{
  TopoSet*  TP = (TopoSet*)cd;
  if (! TP) TP = &topoSetDefault;

  if (var==NULL) {

    ITFCFG(topoSetConfigureItf,cd,"useN");
    ITFCFG(topoSetConfigureItf,cd,"senoneSet");
    ITFCFG(topoSetConfigureItf,cd,"tmSet");
    ITFCFG(topoSetConfigureItf,cd,"commentChar");
    return listConfigureItf((ClientData)&(TP->list), var, val);
  }
  ITFCFG_Int(    var,val,"useN",        TP->useN,                1);
  ITFCFG_Char(   var,val,"commentChar", TP->commentChar,         0);
  ITFCFG_Object( var,val,"senoneSet",   TP->snsP,name,SenoneSet, 1);
  ITFCFG_Object( var,val,"tmSet",       TP->tmSetP,name,TmSet,   1);

  return listConfigureItf((ClientData)&(TP->list), var, val);   
}

/* ------------------------------------------------------------------------
    topoSetAccessItf
   ------------------------------------------------------------------------ */

static ClientData topoSetAccessItf (ClientData cd, char *name, TypeInfo **ti)
{ 
  TopoSet* topoSet = (TopoSet*)cd;

  if (*name == '.') { 
    if (name[1]=='\0') {
       itfAppendElement( "senoneSet");
       itfAppendElement( "tmSet");
       *ti = NULL;
    }
    else { 
      if (! strcmp( name+1, "senoneSet")) {
        *ti = itfGetType("SenoneSet");
        return (ClientData)topoSet->snsP;
      }
      if (! strcmp( name+1, "tmSet")) {
        *ti = itfGetType("TmSet");
        return (ClientData)topoSet->tmSetP;
      }
    }
  }
  return listAccessItf((ClientData)&(topoSet->list),name,ti);
}

/* ------------------------------------------------------------------------
    topoSetPuts
   ------------------------------------------------------------------------ */

static int topoSetPutsItf (ClientData clientData, int argc, char *argv[]) 
{
  TopoSet* topoSet = (TopoSet*)clientData;
  return listPutsItf((ClientData)&(topoSet->list), argc, argv);
}

/* ------------------------------------------------------------------------
    topoSetIndex
   ------------------------------------------------------------------------ */

int topoSetIndex( TopoSet* topoSet,  char* name) 
{
  return listIndex((List*)&(topoSet->list), name, 0);
}

static int topoSetIndexItf( ClientData clientData, int argc, char *argv[] )
{
  TopoSet* topoSet = (TopoSet*)clientData;
  return listName2IndexItf((ClientData)&(topoSet->list), argc, argv);
}

/* ------------------------------------------------------------------------
    topoSetName
   ------------------------------------------------------------------------ */

char* topoSetName( TopoSet* topoSet, int i) 
{ 
  return (i < 0) ? "(null)" : topoSet->list.itemA[i].name;
}

static int topoSetNameItf( ClientData clientData, int argc, char *argv[] )
{
  TopoSet* topoSet = (TopoSet*)clientData;
  return listIndex2NameItf((ClientData)&(topoSet->list), argc, argv);
}

/* ------------------------------------------------------------------------
    topoSetNumber, topoSTree, topoSenoneSet, topoTmSet
   ------------------------------------------------------------------------ */

int topoSetNumber( TopoSet* topoSet) { return topoSet->list.itemN; }

SenoneSet* topoSetSenoneSet( TopoSet* topoSet) { return topoSet->snsP;   }
TmSet*     topoSetTmSet(     TopoSet* topoSet) { return topoSet->tmSetP; }

/* ------------------------------------------------------------------------
    topoSetAdd
   ------------------------------------------------------------------------ */

int topoSetAdd( TopoSet* topoSet, char* name, int* stnXA, int* tmXA, int tmN)
{
  int idx = topoSetIndex( topoSet, name);

  if ( idx < 0) {
    idx = listNewItem((List*)&(topoSet->list), name);

    topoSet->list.itemA[idx].stnXA   = stnXA;
    topoSet->list.itemA[idx].tmXA    = tmXA;
    topoSet->list.itemA[idx].tmN     = tmN;
  }
  else {
    if (stnXA)          free(stnXA);
    if (tmXA)           free(tmXA); 
  }
  return idx;
}

static int topoSetAddItf ( ClientData clientData, int argc, char *argv[] )
{
  TopoSet*       topoSet = (TopoSet*)clientData;
  char*          name    =  NULL;
  SenoneTagArray sta     = {NULL, 0};
  IArray         tmXA    = {NULL, 0};
  int            ac      =  argc-1;

  sta.snsP = topoSet->snsP;

  if (itfParseArgv(argv[0], &ac, argv+1, 0,
     "<name>",      ARGV_STRING, NULL,&name, NULL,"name of topology",
     "<senoneTag*>",ARGV_CUSTOM, getSenoneTagArray,&sta,NULL,
                   "sequence to senonic tree nodes",
     "<tmSet*>",    ARGV_LIARRAY,NULL,&tmXA,&(topoSet->tmSetP->list),
                   "sequence to transitions",
      NULL) != TCL_OK || tmXA.itemN != sta.tagN) {

    if ( sta.tagA)    free( sta.tagA);
    if ( tmXA.itemA)  free( tmXA.itemA);
    return TCL_ERROR;
  }
  return ( topoSetAdd( topoSet, name, sta.tagA, tmXA.itemA, tmXA.itemN) < 0) ? TCL_ERROR : TCL_OK;
}

/* ------------------------------------------------------------------------
    topoSetDeleteItf
   ------------------------------------------------------------------------ */

static int topoSetDeleteItf (ClientData cd, int argc, char *argv[])
{
  TopoSet* topoSet  = (TopoSet*)cd;
  return listDeleteItf((ClientData)&(topoSet->list), argc, argv);
}

/* ------------------------------------------------------------------------
    topoSetRead
   ------------------------------------------------------------------------ */

static int topoSetReadItf ( ClientData clientData, int argc, char *argv[] )
{
  TopoSet* topoSet = (TopoSet*)clientData;
  char   * fname   =  NULL;
  int      ac      =  argc-1, n;

  if (itfParseArgv(argv[0], &ac, argv+1, 0,
		  "<filename>",ARGV_STRING,NULL,&fname,NULL,
                  "name of topo set file", NULL) != TCL_OK)
    return TCL_ERROR;

  itfAppendResult( "%d", n = freadAdd((ClientData)topoSet, fname,
                                       topoSet->commentChar, topoSetAddItf));
  return (n >= 0) ? TCL_OK : TCL_ERROR;
}

/* ------------------------------------------------------------------------
    topoSetWrite
   ------------------------------------------------------------------------ */

int topoSetWrite( TopoSet *topoSet, char *fileName)
{
  int t,ti;
  Topo*     topo     = topoSet->list.itemA;
  int       topoSetN = topoSet->list.itemN;
  FILE     *fp;

  if (! (fp=fileOpen(fileName,"w"))) return TCL_ERROR;

  fprintf( fp, "%c -------------------------------------------------------\n",
                topoSet->commentChar);
  fprintf( fp, "%c  Name            : %s\n", topoSet->commentChar,
                topoSet->name);
  fprintf( fp, "%c  Type            : TopoSet\n", topoSet->commentChar);
  fprintf( fp, "%c  Number of Items : %d\n", topoSet->commentChar,
                topoSet->list.itemN);
  fprintf( fp, "%c  Date            : %s", topoSet->commentChar,
                dateString());
  fprintf( fp, "%c -------------------------------------------------------\n",
                topoSet->commentChar);

  for ( ; topoSetN > 0; topoSetN--, topo++) {
     fprintf(fp,"%s {", topo->name);
     for(t=0;t<topo->tmN;t++) {
       ti = topo->stnXA[t];
       fprintf(fp," %s",(topoSet->snsP)->tagList.itemA[ti].name);
     }
     fprintf(fp,"} {");
     for(t=0;t<topo->tmN;t++) {
       ti = topo->tmXA[t];
       fprintf(fp," %s",(topoSet->tmSetP)->list.itemA[ti].name);
     }
     fprintf(fp,"}\n");
  }
  return fileClose( fileName, fp);
}

static int topoSetWriteItf (ClientData clientData, int argc, char *argv[] )
{
  char   *fname   =  NULL;
  int     ac      =  argc-1;

  if (itfParseArgv(argv[0], &ac, argv+1, 0,
		  "<filename>",ARGV_STRING,NULL,&fname,NULL,
                  "name of topoSet file", NULL) != TCL_OK)
    return TCL_ERROR;

  return topoSetWrite((TopoSet*)clientData, fname);
}

/* ------------------------------------------------------------------------
    topoSetDist
   ------------------------------------------------------------------------ */

int topoSetDist(TopoSet* topoSetP, ModelArray* maP,
                                   ModelArray* maQ,
                                   ModelArray* maR, float* distP)
{
  WARN("There is no distance measure defined for topologies.\n");
  return TCL_ERROR;
}

/* ========================================================================
    Type Declaration
   ======================================================================== */

static Method topoMethod[] =
{
  { "puts",   topoPutsItf,    "display one single topo"           },
  { NULL, NULL, NULL }
} ;

TypeInfo topoInfo = { "Topo", 0, -1, topoMethod, 
                       NULL, NULL, 
                       topoConfigureItf, NULL, itfTypeCntlDefaultNoLink,
        "A 'Topo' object is a definition of a single topology description." };

static Method topoSetMethod[] = 
{
  { "puts",   topoSetPutsItf,   "displays the contents of a TopoSet object" },
  { "add",    topoSetAddItf,    "add a new topo to a TopoSet object"    },
  { "delete", topoSetDeleteItf, "remove a topo from a TopoSet object"   },
  { "read",   topoSetReadItf,   "read TopoSet from a file"                  },
  { "write",  topoSetWriteItf,  "write TopoSet into a file"                 },
  { "index",  topoSetIndexItf,  "return the index of a named topo"        },
  { "name",   topoSetNameItf,   "return the name of an indexed topo"      },
  { NULL, NULL, NULL }
} ;

TypeInfo topoSetInfo = { "TopoSet", 0, -1, topoSetMethod, 
                          topoSetCreateItf, topoSetFreeItf, 
                          topoSetConfigureItf, topoSetAccessItf, NULL,
        "A 'TopoSet' object is a set of different topologies." };

static ModelSet topoSetModel = 
             { "TopoSet", &topoSetInfo, &topoInfo,
               (ModelSetIndexFct*)topoSetIndex,
               (ModelSetNameFct *)topoSetName,
               (ModelSetDistFct *)topoSetDist };

static int topoSetInitialized = 0;

int Topo_Init ()
{
  if (! topoSetInitialized) {

    topoSetDefault.name            =  NULL;
    topoSetDefault.useN            =  0;
    topoSetDefault.list.itemN      =  0;
    topoSetDefault.list.blkSize    =  20;
    topoSetDefault.snsP            =  NULL;
    topoSetDefault.tmSetP          =  NULL;
    topoSetDefault.commentChar     =  ';';

    if (Tm_Init()      != TCL_OK) return TCL_ERROR;
    if (Duration_Init()!= TCL_OK) return TCL_ERROR;
    if (Senones_Init() != TCL_OK) return TCL_ERROR;

    itfNewType(&topoInfo);
    itfNewType(&topoSetInfo);

    modelSetNewType( &topoSetModel);

    topoSetInitialized = 1;
  }
  return TCL_OK;
}
