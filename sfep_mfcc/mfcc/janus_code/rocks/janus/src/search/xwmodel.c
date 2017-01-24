/* ========================================================================
    JANUS      Speech- to Speech Translation
               ------------------------------------------------------------
               Object: Cross-Word Acoustic Models
               ------------------------------------------------------------

    Author  :  Michael Finke
    Module  :  xwmodel.c
    Date    :  $Id: xwmodel.c 2390 2003-08-14 11:20:32Z fuegen $
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
    Revision 3.7  2003/08/14 11:20:24  fuegen
    Merged changes on branch jtk-01-01-15-fms (jaguar -> ibis-013)

    Revision 3.6.20.1  2002/06/26 11:57:58  fuegen
    changes for new directory structure support and code cleaning

    Revision 3.6  2000/11/14 12:35:27  janus
    Merging branch jtk-00-10-27-jmcd.

    Revision 3.5.2.2  2000/11/08 17:36:28  janus
    Finished making changes required for compilation under 'gcc -Wall'.

    Revision 3.5.2.1  2000/11/06 10:50:42  janus
    Made changes to enable compilation under -Wall.

    Revision 3.5  2000/10/27 16:11:27  janus
    Merged branch jtk-00-10-24-jmd.

    Revision 3.4.32.1  2000/10/27 15:05:05  janus
    Made SIMD a run-time option.

    Revision 3.4  1997/07/30 13:24:04  kemp
    Changed location of #include "malloc.h" to be the last include statement

 * Revision 3.3  1997/07/18  17:57:51  monika
 * gcc-clean
 *
    Revision 3.2  1997/06/17 17:41:43  westphal
    *** empty log message ***

    Revision 1.7  95/10/31  02:05:39  finkem
    removed possible problems with hashKey on HPs
    
    Revision 1.6  1995/09/06  07:46:22  kemp
    *** empty log message ***

    Revision 1.5  1995/08/27  22:35:42  finkem
    *** empty log message ***

    Revision 1.4  1995/08/18  08:23:56  finkem
    *** empty log message ***

    Revision 1.3  1995/08/17  17:09:30  rogina
    *** empty log message ***

    Revision 1.2  1995/08/12  10:24:51  finkem
    *** empty log message ***

    Revision 1.1  1995/08/03  14:40:51  finkem
    Initial revision

 
   ======================================================================== */

#include "xwmodel.h"
#include "array.h"


char xwmodelRCSVersion[]= 
           "@(#)1$Id: xwmodel.c 2390 2003-08-14 11:20:32Z fuegen $";


/* ------------------------------------------------------------------------
    Forward Declarations
   ------------------------------------------------------------------------ */

typedef struct
{
  int  fullN;
  int* fullXA;

} HashKey;

extern TypeInfo xwmodelInfo;

/* ========================================================================
    XWord Model
   ======================================================================== */
/* ------------------------------------------------------------------------
    xwmodelInit
   ------------------------------------------------------------------------ */

static int xwmodelInit( XWModel* xwmodel, HashKey* key)
{
  xwmodel->fullN  = key->fullN;
  xwmodel->fullXA = key->fullXA;
  xwmodel->relXA  = NULL;
  xwmodel->uniqXA = NULL;
  xwmodel->uniqN  = 0;
  return TCL_OK;
}

/* ------------------------------------------------------------------------
    xwmodelDeinit
   ------------------------------------------------------------------------ */

static int xwmodelDeinit (XWModel *xwmodel)
{
  if ( xwmodel->fullXA) free( xwmodel->fullXA);
  if ( xwmodel->relXA)  free( xwmodel->relXA);
  if ( xwmodel->uniqXA) free( xwmodel->uniqXA);
  return TCL_OK;
}

/* ------------------------------------------------------------------------
    xwmodelHashKey & xwmodelHashCmp
   ------------------------------------------------------------------------ */

static long xwmodelHashKey( const HashKey *key)
{
  unsigned long ret = 0; 
  int           fullX;

  for (fullX=0; fullX<key->fullN; fullX++) {
    ret = ((ret<<5) | (ret >> (8*sizeof(long)-5))) ^ key->fullXA[fullX];
  }
  return ret;
}

static int xwmodelHashCmp( const HashKey *key, const XWModel *xwmodel)
{
  int fullX;

  if (xwmodel->fullXA[0] != key->fullXA[0]) return 0;
  if (xwmodel->fullN     != key->fullN    ) return 0;

  for (fullX=1; fullX<key->fullN; fullX++)
    if (xwmodel->fullXA[fullX] != key->fullXA[fullX]) return 0;

  return 1;
}

/* ------------------------------------------------------------------------
    xwmodelPutsItf
   ------------------------------------------------------------------------ */

static int xwmodelPutsItf (ClientData clientData, int argc, char *argv[])
{
  XWModel* xmP = (XWModel*)clientData;
  int      fullX;
  int        ac      =  argc - 1;
  if ( itfParseArgv( argv[0], &ac, argv+1, 0,
       NULL) != TCL_OK) return TCL_ERROR;

  itfAppendResult("%d {",xmP->fullN);
  for (fullX=0; fullX < xmP->fullN; fullX++)
    itfAppendElement("%d",xmP->fullXA[fullX]);
  itfAppendResult("} {");
  for (fullX=0; fullX < xmP->fullN; fullX++)
    itfAppendElement("%d",xmP->relXA[fullX]);
  itfAppendResult("} {");
  for (fullX=0; fullX < xmP->uniqN; fullX++)
    itfAppendElement("%d",xmP->uniqXA[fullX]);
  itfAppendResult("}");
  return TCL_OK;    
}

/* ========================================================================
    Set of XWord Models
   ======================================================================== */

static ClientData xwmodelSetCreateItf(    ClientData clientData, 
                                          int argc, char *argv[]);
static int        xwmodelSetFreeItf(      ClientData clientData);
static int        xwmodelSetAddItf(       ClientData clientData, 
                                          int argc, char *argv[]);
static int        xwmodelSetConfigureItf( ClientData cd, char *var, char *val);

/* ------------------------------------------------------------------------
    xwmodelSetCreate
   ------------------------------------------------------------------------ */

static int xwmodelSetInit( XWModelSet* xwmodelSet, ClientData CD)
{
  xwmodelSet->name    = strdup((char*)CD);
  xwmodelSet->useN    = 0;
  xwmodelSet->amodelP = NULL;

  listInit((List*)&(xwmodelSet->list), &xwmodelInfo, sizeof(XWModel), 500);

  xwmodelSet->list.init    = (ListItemInit  *)xwmodelInit;
  xwmodelSet->list.deinit  = (ListItemDeinit*)xwmodelDeinit;
  xwmodelSet->list.hashKey = (HashKeyFn     *)xwmodelHashKey;
  xwmodelSet->list.hashCmp = (HashCmpFn     *)xwmodelHashCmp;

  return TCL_OK;
}

XWModelSet* xwmodelSetCreate( char *name, AModelSet* amodelP)
{
  XWModelSet *xwmodelSet = (XWModelSet*)malloc(sizeof(XWModelSet));

  if (xwmodelSet==NULL) { ERROR("Out of memory\n"); return NULL; }

  xwmodelSetInit( xwmodelSet, (ClientData)name);

  xwmodelSet->amodelP = amodelP; link(amodelP,"AModelSet");

  return xwmodelSet;
}

static ClientData xwmodelSetCreateItf( ClientData clientData, 
                                       int argc, char *argv[])
{
  int        ac      =  argc - 1;
  AModelSet* amodelP =  NULL;

  assert (clientData == clientData); /* not used. shut up compiler */

  if ( itfParseArgv( argv[0], &ac, argv+1, 0,
      "<AModelSet>", ARGV_OBJECT, NULL, &amodelP, "AModelSet", "acoustic models",
       NULL) != TCL_OK) return NULL;

  return (ClientData)xwmodelSetCreate(argv[0],amodelP);
}

/* ------------------------------------------------------------------------
    xwmodelSetFree
   ------------------------------------------------------------------------ */

static int xwmodelSetLinkN( XWModelSet* xwmodelSet)
{
  int  useN = listLinkN((List*)&(xwmodelSet->list)) - 1;

  if ( useN < xwmodelSet->useN) return xwmodelSet->useN;
  else                          return useN;
}

static int xwmodelSetDeinit( XWModelSet* xwmodelSet)
{
  if (xwmodelSet==NULL) { ERROR("null argument"); return TCL_ERROR; }

  if (xwmodelSetLinkN( xwmodelSet)) { 
    SERROR("XWModelSet '%s' still in use by other objects.\n", 
            xwmodelSet->name); 
    return TCL_ERROR;
  }
  unlink( xwmodelSet->amodelP, "AModelSet");
  free(xwmodelSet->name);
  return listDeinit((List*)&(xwmodelSet->list));
}

static int xwmodelSetFree( XWModelSet* xwmodelSet)
{
  if ( xwmodelSetDeinit( xwmodelSet) != TCL_OK) return TCL_ERROR;
  free(xwmodelSet);
  return TCL_OK;
}

static int xwmodelSetFreeItf (ClientData clientData)
{
  XWModelSet *xwmodelSet = (XWModelSet*)clientData;
  return xwmodelSetFree( xwmodelSet);
}

/* ------------------------------------------------------------------------
    xwmodelSetAdd
   ------------------------------------------------------------------------ */

static int intCompare(void * a, void *b)
{
  return ((*(int *)a) - (*(int *)b));
}


int xwmodelSetAdd( XWModelSet* xwmodelSet, int fullN, int *fullXA)
{
  HashKey key;
  int     idx;

  key.fullN  = fullN;
  key.fullXA = (int*)malloc(sizeof(int)*fullN);   /* freed by xwmodelDeinit */

  memcpy(key.fullXA,fullXA,sizeof(int)*fullN);

  idx = listIndex((List*) &(xwmodelSet->list), &key, 1);

  assert(idx>=0);

  if (xwmodelSet->list.itemA[idx].fullXA != key.fullXA) { free (key.fullXA); }
  else {
    int uniqXA[60000];
    int i, uniqN = 0;

    assert(fullN < 60000);

    for ( i = 0; i < fullN; i++) {
      int uniqX;
      int modelX = fullXA[i];

      for (uniqX =  0; uniqX < uniqN; uniqX++) if (modelX == uniqXA[uniqX]) break;
      if ( uniqX == uniqN) uniqXA[uniqN++] = modelX;
    }
    qsort((void *)uniqXA, uniqN, sizeof(int), 
          (int (*)(const void *, const void *))intCompare);
    uniqXA[uniqN] = -1;      /* terminate array with -1 */

    xwmodelSet->list.itemA[idx].relXA  = (int*)malloc(sizeof(int)*fullN);
    xwmodelSet->list.itemA[idx].uniqXA = (int*)malloc((uniqN+1)*sizeof(int));
    xwmodelSet->list.itemA[idx].uniqN  =  uniqN;

    memcpy( xwmodelSet->list.itemA[idx].uniqXA, uniqXA,sizeof(int)*(uniqN+1));

    for ( i = 0; i < fullN; i++) {
      int  findmeX = fullXA[i];
      int* ergP    = (int *)bsearch((void *)&findmeX, (void *)uniqXA,
                            uniqN,sizeof(int), 
                           (int (*)(const void *, const void *))intCompare);

      if (ergP == 0) ERROR("Can't find allophon of fulllist in uniqlist");

      xwmodelSet->list.itemA[idx].relXA[i] = (ergP - uniqXA);  
    }
  }
  return idx;
}

static int xwmodelSetAddItf (ClientData clientData, int argc, char *argv[])
{
  XWModelSet *xwmsP  = (XWModelSet*)clientData;
  IArray      fullXA = { NULL, 0 };

  argc--;
 
  if ( itfParseArgv( argv[0], &argc, argv+1, 0,
      "<AModel*>",  ARGV_IARRAY, NULL, &fullXA, NULL,      
                   "list of acoustic model indices",
       NULL) != TCL_OK) {

    if (fullXA.itemA) free(fullXA.itemA); 
    return TCL_ERROR;
  }
  return xwmodelSetAdd (xwmsP,fullXA.itemN,fullXA.itemA);
}

/* ------------------------------------------------------------------------
    xwmodelSetPutsItf
   ------------------------------------------------------------------------ */

static int xwmodelSetPutsItf( ClientData clientData, int argc, char *argv[])
{
  XWModelSet *xwmodelSet = (XWModelSet*)clientData;

  int        ac      =  argc - 1;
  if ( itfParseArgv( argv[0], &ac, argv+1, 0,
       NULL) != TCL_OK) return TCL_ERROR;
 
  itfAppendResult("%d",xwmodelSet->list.itemN);
  return TCL_OK;
}

/* ------------------------------------------------------------------------
    xwmodelSetConfigureItf
   ------------------------------------------------------------------------ */

static int xwmodelSetConfigureItf( ClientData cd, char *var, char *val)
{
  XWModelSet* xwmsP = (XWModelSet*)cd;
  if (! xwmsP)  return TCL_ERROR;

  if (! var) {

    ITFCFG(xwmodelSetConfigureItf,cd,"useN");
    ITFCFG(xwmodelSetConfigureItf,cd,"amodelSet");
    return TCL_OK;
  }
  ITFCFG_Int(    var,val,"useN",      xwmsP->useN, 1);
  ITFCFG_Object( var,val,"amodelSet", xwmsP->amodelP,name,AModelSet,1);
  return TCL_ERROR;
}

/* ------------------------------------------------------------------------
    xwmodelSetAccessItf
   ------------------------------------------------------------------------ */

static ClientData xwmodelSetAccessItf (ClientData cd, char *name, TypeInfo **ti)
{ 
  XWModelSet *xwmodelSet = (XWModelSet*)cd;

  if (*name == '.') { 
    if (name[1]=='\0') {
      itfAppendElement( "amodelSet");
      itfAppendElement( "model(0..%d)",xwmodelSet->list.itemN-1);
      *ti = NULL; return NULL;
    }
    else { 
      int i;
      if (! strcmp( name+1, "amodelSet")) {
        *ti = itfGetType("AModelSet");
        return (ClientData)xwmodelSet->amodelP;
      }
      else if (sscanf(name+1,"model(%d)",&i)==1) {
	*ti = &xwmodelInfo;

	 if (i>=0 && i<xwmodelSet->list.itemN) 
              return (ClientData) &(xwmodelSet->list.itemA[i]);
	 else return NULL;
      }
    }
  }
  else if (*name == ':') { 
    if (name[1]=='\0') { 
      itfAppendElement( "0 %d",xwmodelSet->list.itemN-1);
      *ti = NULL; return NULL;
    }
    else { 
      int i = atoi(name+1);
      *ti = &xwmodelInfo;
      if (i>=0 && i<xwmodelSet->list.itemN) 
           return (ClientData) &(xwmodelSet->list.itemA[i]);
      else return NULL;
    }
  }
  return NULL;
}

/* ========================================================================
    Type Information
   ======================================================================== */

Method xwmodelMethod[] = 
{
  { "puts",   xwmodelPutsItf,   "displays the contents of an xword model"  },
  { NULL, NULL, NULL }
} ;

Method xwmodelSetMethod[] = 
{
  { "puts", xwmodelSetPutsItf,   "displays the contents of an xwmodel set"  },
  { "add",  xwmodelSetAddItf,    "add a state graph to a set" },
  { NULL, NULL, NULL }
} ;

TypeInfo xwmodelInfo = 
{ "XWModel", 0, 0, xwmodelMethod, 0, 0, NULL, NULL, itfTypeCntlDefaultNoLink,
  "blah" 
};

TypeInfo xwmodelSetInfo = 
{ "XWModelSet", 0, -1,     xwmodelSetMethod, 
   xwmodelSetCreateItf,    xwmodelSetFreeItf, 
   xwmodelSetConfigureItf, xwmodelSetAccessItf, NULL,
  "set of blah"
};

static int xwmodelInitialized = 0;

int XWModel_Init ()
{
  if (! xwmodelInitialized) {
    if ( AModel_Init() != TCL_OK) return TCL_ERROR;

    itfNewType (&xwmodelInfo);
    itfNewType (&xwmodelSetInfo);

    xwmodelInitialized = 1;
  }
  return TCL_OK;
}

