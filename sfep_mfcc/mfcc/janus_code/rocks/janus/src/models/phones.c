/* ========================================================================
    JANUS      Speech- to Speech Translation
               ------------------------------------------------------------
               Object: Phoneme Set Maintainance Functions
               ------------------------------------------------------------

    Author  :  Ivica Rogina & Michael Finke
    Module  :  phones.c
    Date    :  $Id: phones.c 2390 2003-08-14 11:20:32Z fuegen $
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
    Revision 3.6  2003/08/14 11:20:16  fuegen
    Merged changes on branch jtk-01-01-15-fms (jaguar -> ibis-013)

    Revision 3.5.20.4  2003/07/02 17:45:19  fuegen
    fixes for file handling under Windows
    using general functions for file open/close

    Revision 3.5.20.3  2002/08/27 08:42:44  metze
    itfParseArgv now uses <name> and NULL initialization for char*

    Revision 3.5.20.2  2002/06/26 11:57:56  fuegen
    changes for new directory structure support and code cleaning

    Revision 3.5.20.1  2002/02/28 15:45:38  soltau
    cleaned phonesSetDeinit

    Revision 3.5  2000/11/14 12:35:25  janus
    Merging branch jtk-00-10-27-jmcd.

    Revision 3.4.34.2  2000/11/08 17:27:46  janus
    Finished making changes required for compilation under 'gcc -Wall'.

    Revision 3.4.34.1  2000/11/06 10:50:38  janus
    Made changes to enable compilation under -Wall.

    Revision 3.4  2000/08/16 11:36:41  soltau
    free -> TclFree

    Revision 3.3  1997/07/25 17:32:31  tschaaf
    gcc / DFKI - Clean

    Revision 3.2  1997/06/17 17:41:43  westphal
    *** empty log message ***

    Revision 1.15  96/02/07  17:05:47  rogina
    added checking for out-of-bounds in name method
    
    Revision 1.14  1996/01/01  16:25:39  finkem
    *** empty log message ***

    Revision 1.13  1996/01/01  16:20:47  finkem
    remove INFO output when reading

    Revision 1.12  1996/01/01  16:18:20  finkem
    handling of parsing errors in the Read function

    Revision 1.11  1995/09/06  07:35:58  kemp
    *** empty log message ***

    Revision 1.10  1995/08/29  15:32:51  finkem
    *** empty log message ***

    Revision 1.9  1995/08/27  22:33:31  finkem
    *** empty log message ***

    Revision 1.8  1995/08/18  08:22:06  finkem
    *** empty log message ***

    Revision 1.7  1995/08/17  18:01:13  rogina
    *** empty log message ***

    Revision 1.6  1995/08/16  14:41:35  rogina
    *** empty log message ***

    Revision 1.5  1995/08/10  13:37:53  finkem
    *** empty log message ***

    Revision 1.4  1995/08/09  10:11:55  finkem
    *** empty log message ***

    Revision 1.3  1995/07/25  22:33:21  finkem
    *** empty log message ***

    Revision 1.2  1995/07/25  14:45:47  rogina
    *** empty log message ***

    Revision 1.1  1995/07/25  11:27:26  finkem
    Initial revision

 
   ======================================================================== */

char phonesRCSVersion[]= 
           "@(#)1$Id: phones.c 2390 2003-08-14 11:20:32Z fuegen $";


#include "common.h"
#include "itf.h"
#include "phones.h"


/* ------------------------------------------------------------------------
    Forward Declaration
   ------------------------------------------------------------------------ */

extern TypeInfo phoneInfo;
extern TypeInfo phonesInfo;
extern TypeInfo phonesSetInfo;

/* ========================================================================
    Phone
   ======================================================================== */

int phoneInit( Phone* phone, ClientData CD)
{
  phone->name = strdup((char*)CD);
  return TCL_OK;
}

int phoneDeinit( Phone* phone)
{
  if ( phone->name) { free(phone->name); phone->name = NULL; }
  return TCL_OK;
}

static int phonePutsItf( ClientData clientData, int argc, char *argv[]) 
{
  Phone* phone = (Phone*)clientData;

  argc--;
  if (itfParseArgv(argv[0], &argc, argv+1, 0,
        NULL) != TCL_OK)  return TCL_ERROR;

  itfAppendResult("%s", phone->name);
  return TCL_OK;
}

/* ========================================================================
    Phones
   ======================================================================== */

static ClientData phonesCreateItf( ClientData clientData, 
                                   int argc, char *argv[]);
static int        phonesFreeItf(   ClientData clientData);

static Phones phonesDefault;

/* ------------------------------------------------------------------------
    Create/Init Phones object
   ------------------------------------------------------------------------ */

int phonesInit( Phones* phones, ClientData CD)
{
  phones->name   = strdup((char*)CD);
  phones->useN   = 0;

  listInit((List*)&(phones->list), &phoneInfo, sizeof(Phone),
            phonesDefault.list.blkSize);

  phones->list.init   = (ListItemInit  *)phoneInit;
  phones->list.deinit = (ListItemDeinit*)phoneDeinit;

  phones->commentChar =  phonesDefault.commentChar;
  return TCL_OK;
}

Phones *phonesCreate( char* name)
{
  Phones *phones = (Phones*)malloc(sizeof(Phones));

  if (! phones || phonesInit( phones, (ClientData)name) != TCL_OK) {
     if ( phones) free( phones);
     SERROR("Failed to allocate Phones object '%s'.\n", name);
     return NULL; 
  }
  return phones;
}

static ClientData phonesCreateItf( ClientData clientData, 
                                   int argc, char *argv[]) 
{ 
  char* name = NULL;

  if (itfParseArgv (argv[0], &argc, argv, 0,
      "<name>",  ARGV_STRING, NULL, &name, NULL,         "name of the phones set",
       NULL) != TCL_OK) return NULL;

  return (ClientData)phonesCreate(name);
}

/* ------------------------------------------------------------------------
    Free/Deinit Phones Object
   ------------------------------------------------------------------------ */

int phonesLinkN( Phones* phones)
{
  int  useN = listLinkN((List*)&(phones->list)) - 1;
  if ( useN < phones->useN) return phones->useN;
  else                      return useN;
}

int phonesDeinit( Phones* phones)
{
  if ( phonesLinkN( phones)) {
    SERROR("Phones '%s' still in use by other objects.\n", phones->name);
    return TCL_ERROR;
  }
  if ( phones->name) { free( phones->name); phones->name = NULL; }
  return listDeinit((List*)&(phones->list));
}

int phonesFree(Phones* phones)
{
  if (phonesDeinit( phones) != TCL_OK) return TCL_ERROR;
  free(phones);
  return TCL_OK;
}

static int phonesFreeItf (ClientData clientData) 
{ return phonesFree( (Phones*)clientData); }

/* ------------------------------------------------------------------------
    phonesConfigureItf
   ------------------------------------------------------------------------ */

int phonesConfigureItf(ClientData cd, char *var, char *val)
{
  Phones *phones = (Phones*)cd;
  if (!phones) phones = &phonesDefault;

  if (var==NULL) {
    ITFCFG(phonesConfigureItf,cd,"useN");
    ITFCFG(phonesConfigureItf,cd,"commentChar");
    return listConfigureItf((ClientData)&(phones->list), var, val);
  }
  ITFCFG_Int(    var,val,"useN",        phones->useN,       0);
  ITFCFG_Char(   var,val,"commentChar", phones->commentChar,0);
  return listConfigureItf((ClientData)&(phones->list), var, val);   
}

/* ------------------------------------------------------------------------
    phonesAccessItf
   ------------------------------------------------------------------------ */

ClientData phonesAccessItf (ClientData cd, char *name, TypeInfo **ti)
{ 
  Phones* phones = (Phones*)cd;
  return listAccessItf((ClientData)&(phones->list),name,ti);
}

/* ------------------------------------------------------------------------
    phonesPuts
   ------------------------------------------------------------------------ */

static int phonesPutsItf (ClientData clientData, int argc, char *argv[]) 
{
  Phones* phones = (Phones*)clientData;

  argc--;
  if (itfParseArgv(argv[0], &argc, argv+1, 0,
        NULL) != TCL_OK)  return TCL_ERROR;

  return listPutsItf((ClientData)&(phones->list), argc, argv);
}

/* ------------------------------------------------------------------------
    phonesIndex
   ------------------------------------------------------------------------ */

int phonesIndex( Phones* phones,  char* phone) 
{
  return listIndex((List*)&(phones->list), phone, 0);
}

int phonesIndexItf( ClientData clientData, int argc, char *argv[] )
{
  Phones* phones = (Phones*)clientData;
  return listName2IndexItf((ClientData)&(phones->list), argc, argv);
}

/* ------------------------------------------------------------------------
    phonesName
   ------------------------------------------------------------------------ */

char* phonesName( Phones* phones, int i) 
{ 
  return (i>=0 && i<phones->list.itemN) ? phones->list.itemA[i].name : "(null)";  
}

int phonesNameItf( ClientData clientData, int argc, char *argv[])
{
  Phones* phones = (Phones*)clientData;
  return listIndex2NameItf((ClientData)&(phones->list), argc, argv);
}

/* ------------------------------------------------------------------------
    phonesNumber
   ------------------------------------------------------------------------ */

int phonesNumber( Phones* phones) { return phones->list.itemN; }

/* ------------------------------------------------------------------------
    phonesAdd
   ------------------------------------------------------------------------ */

int phonesAdd( Phones* phones, int argc, char* argv[])
{
  int i;
  for ( i = 0; i < argc; i++) {
    listIndex((List*)&(phones->list), argv[i], 1);
  }
  return TCL_OK;
}

static int phonesAddItf ( ClientData clientData, int argc, char *argv[] )
{
  Phones* phones = (Phones*)clientData;
  int     ac     =  argc-1;
  int     i;

  if (itfParseArgv(argv[0], &ac, argv+1, 0,
		  "<phone*>",ARGV_REST,NULL,&i,NULL,"list of phones",
		   NULL) != TCL_OK)
    return TCL_ERROR;

  while ( i++ < ac) {
    int     tokc;
    char  **tokv;

    if ( Tcl_SplitList (itf,argv[i],&tokc,&tokv) != TCL_OK) return TCL_ERROR;

    phonesAdd( phones, tokc, tokv);
    Tcl_Free((char*)tokv);
  }
  return TCL_OK;
}

/* ------------------------------------------------------------------------
    phonesDelete
   ------------------------------------------------------------------------ */

int phonesDelete( Phones *phones, int argc, char* argv[])
{
  int i;
  for ( i = 0; i < argc; i++) {
    if ( listDelete((List*)&(phones->list), argv[i]) != TCL_OK) {
      SERROR( "Can't remove phone '%s' from '%s'.\n", argv[i], phones->name);
      return TCL_ERROR;
    }
  }
  return TCL_OK;
}

static int phonesDeleteItf (ClientData clientData, int argc, char *argv[])
{
  Phones *phones  = (Phones*)clientData;
  int     retCode =  TCL_OK;
  int     ac      =  argc-1;
  int     i;

  if (itfParseArgv(argv[0], &ac, argv+1, 0,
		   "<phone*>",ARGV_REST,NULL,&i,NULL,"list of phones",
		   NULL) != TCL_OK)
    return TCL_ERROR;

  while ( i++ < ac) {
    int     tokc;
    char  **tokv;

    if ( Tcl_SplitList (itf,argv[i],&tokc,&tokv) != TCL_OK) return TCL_ERROR;

    if ( phonesDelete( phones, tokc, tokv) != TCL_OK) retCode = TCL_ERROR;
    Tcl_Free((char*)tokv);
  }
  return retCode;
}

/* ------------------------------------------------------------------------
    phonesRead
   ------------------------------------------------------------------------ */

int phonesRead( Phones *phones, char *filename)
{
  FILE *fp;
  char  line[1000];
  int   n  = 0;
  int   rc = TCL_OK;

  if ((fp=fileOpen(filename,"r"))==NULL) {
    ERROR( "Can't open phones file '%s' for reading.\n",filename);
    return -1;
  }

  while (1) {
    char *p = line;
    char**argv;
    int   argc;

    if ( fscanf(fp,"%[^\n]",line)!=1) break; 
    else fscanf(fp,"%*c");

    if ( line[0] == phones->commentChar) continue; 

    for (p=line; *p!='\0'; p++) 
         if (*p>' ') break; if (*p=='\0') continue; 

    Tcl_SplitList( itf, line, &argc, &argv);
    if ( phonesAdd(phones, argc, argv) != TCL_OK) rc = TCL_ERROR;
    else n += argc;
    Tcl_Free((char*) argv);
  }
  fileClose(filename, fp);
  return (rc != TCL_OK) ? -1 : n;
}

static int phonesReadItf ( ClientData clientData, int argc, char *argv[] )
{
  Phones* phones  = (Phones*)clientData;
  char*   fname   =  NULL;
  int     ac      =  argc-1;
  int     n;

  if (itfParseArgv(argv[0], &ac, argv+1, 0,
		  "<filename>",ARGV_STRING,NULL,&fname,NULL,
                  "name of phones file", NULL) != TCL_OK)
    return TCL_ERROR;

  itfAppendResult( "%d", n = phonesRead( phones, fname));
  return (n >= 0) ? TCL_OK : TCL_ERROR;
}

/* ------------------------------------------------------------------------
    phonesWrite
   ------------------------------------------------------------------------ */

static void phonesWriteFile(Phones *phones, FILE *fp)
{
  Phone* phone  = phones->list.itemA;
  int    phoneN = phones->list.itemN;

  for ( ; phoneN > 0; phoneN--, phone++) fprintf(fp,"%s ",phone->name);
  fprintf(fp,"\n");
}

int phonesWrite (Phones *phones, char *filename)
{
  FILE *fp;

  if ((fp=fileOpen(filename,"w"))==NULL) {
    ERROR( "Can't open phones file '%s' for writing.\n",filename);
    return TCL_ERROR;
  }

  fprintf( fp, "%c -------------------------------------------------------\n",
                phones->commentChar);
  fprintf( fp, "%c  Name            : %s\n",  phones->commentChar, 
                phones->name);
  fprintf( fp, "%c  Type            : Phones\n", phones->commentChar);
  fprintf( fp, "%c  Number of Items : %d\n",  phones->commentChar, 
                phones->list.itemN);
  fprintf( fp, "%c  Date            : %s", phones->commentChar,
                dateString());
  fprintf( fp, "%c -------------------------------------------------------\n",
                phones->commentChar);

  phonesWriteFile (phones,fp);
  fileClose(filename, fp);
  return TCL_OK;
}

static int phonesWriteItf (ClientData clientData, int argc, char *argv[] )
{
  char   *fname   =  NULL;
  int     ac      =  argc-1;

  if (itfParseArgv(argv[0], &ac, argv+1, 0,
		  "<filename>",ARGV_STRING,NULL,&fname,NULL,
                  "name of phones file", NULL) != TCL_OK)
    return TCL_ERROR;

  return phonesWrite( (Phones*)clientData, fname);
}

/* ------------------------------------------------------------------------
    phonesCharFunc
   ------------------------------------------------------------------------ */

int phonesCharFunc (Phones *superset, Phones *subset, char *charFunc)
{
  int i,x,ill=0;

  memset( charFunc, '-', phonesNumber(superset));
  charFunc[phonesNumber(superset)]='\0';

  for ( x = 0; x < phonesNumber(subset); x++) {
    if ((i=phonesIndex( superset, subset->list.itemA[x].name))!=(-1)) 
         charFunc[i]='+'; 
    else ill++;
  }
  return ill;
}

static int phonesCharFuncItf( ClientData clientData, Tcl_Interp *interp, 
                       int argc, char *argv[])
{
  Phones* superSet, *subSet;
  int     ac = argc - 1;
  char*   charFunc; int ill;

  if (itfParseArgv(argv[0], &ac, argv+1, 0,
     "<superSet>",ARGV_OBJECT,NULL,&superSet,"Phones",
     "name of phones superset", 
     "<subSet>",  ARGV_OBJECT,NULL,&subSet,  "Phones",
     "name of phones subset", NULL) != TCL_OK) {
     MSGPRINT(NULL); return TCL_ERROR;
  }
  charFunc = (char*)malloc(sizeof(char)*(1+superSet->list.itemN));
  if (charFunc==NULL) return TCL_ERROR;

  ill = phonesCharFunc(superSet,subSet,charFunc);
  itfAppendResult("%s",charFunc);
  free(charFunc);

  if (ill) {
       ERROR("%d phone names from %s are not in %s\n",
              ill, subSet->name, superSet->name);
       return TCL_ERROR;
  }
  else return TCL_OK;
}

/* ========================================================================
    PhonesSet
   ======================================================================== */

static PhonesSet phonesSetDefault;

/* ------------------------------------------------------------------------
    Create/Init PhonesSet object
   ------------------------------------------------------------------------ */

int phonesSetInit( PhonesSet* phonesSet, ClientData CD)
{
  phonesSet->name   = strdup((char*)CD);
  phonesSet->useN   = 0;

  listInit((List*)&(phonesSet->list), &phonesInfo, sizeof(Phones), 
                    phonesSetDefault.list.blkSize);

  phonesSet->list.init    = (ListItemInit  *)phonesInit;
  phonesSet->list.deinit  = (ListItemDeinit*)phonesDeinit;
  phonesSet->list.links   = (ListItemLinkN *)phonesLinkN;

  phonesSet->commentChar  =  phonesSetDefault.commentChar;

  return TCL_OK;
}

PhonesSet *phonesSetCreate( char* name)
{
  PhonesSet *phonesSet = (PhonesSet*)malloc(sizeof(PhonesSet));

  if (! phonesSet || 
        phonesSetInit( phonesSet, (ClientData)name) != TCL_OK) {

     if ( phonesSet) free( phonesSet);
     ERROR("Failed to allocate PhonesSet object '%s'.\n", name);
     return NULL; 
  }
  return phonesSet;
}

static ClientData phonesSetCreateItf(ClientData clientData, 
                                     int argc, char *argv[])
{ 
  int     ac     = argc-1;
  if (itfParseArgv(argv[0], &ac, argv+1, 0,
		   NULL) != TCL_OK)
    return NULL;

  return (ClientData)phonesSetCreate(argv[0]);
}

/* ------------------------------------------------------------------------
    Free/Deinit PhonesSet Object
   ------------------------------------------------------------------------ */

int phonesSetLinkN( PhonesSet* phonesSet)
{
  int  useN = listLinkN((List*)&(phonesSet->list)) - 1;
  if ( useN < phonesSet->useN) return phonesSet->useN;
  else                         return useN;
}

int phonesSetDeinit( PhonesSet* phonesSet)
{
  if ( phonesSetLinkN(phonesSet) > 1) {
    SERROR("PhonesSet '%s' still in use by other objects.\n", phonesSet->name);
    return TCL_ERROR;
  }
  if ( phonesSet->name) { free( phonesSet->name); phonesSet->name = NULL; }

  return listDeinit((List*)&(phonesSet->list));
}

int phonesSetFree(PhonesSet* phonesSet)
{
  if ( phonesSetDeinit( phonesSet) != TCL_OK) return TCL_ERROR;
  free(phonesSet);
  return TCL_OK;
}

static int phonesSetFreeItf (ClientData clientData) 
{ return phonesSetFree( (PhonesSet*)clientData); }

/* ------------------------------------------------------------------------
    phonesSetConfigureItf
   ------------------------------------------------------------------------ */

static int phonesSetConfigureItf(ClientData cd, char *var, char *val)
{
  PhonesSet *phonesSet = (PhonesSet*)cd;
  if (!phonesSet) phonesSet = &phonesSetDefault;

  if (var==NULL) {
    ITFCFG(phonesSetConfigureItf,cd,"useN");
    ITFCFG(phonesSetConfigureItf,cd,"commentChar");
    return listConfigureItf((ClientData)&(phonesSet->list), var, val);
  }
  ITFCFG_Int(    var,val,"useN",        phonesSet->useN,       1);
  ITFCFG_Char(   var,val,"commentChar", phonesSet->commentChar,0);
  return listConfigureItf((ClientData)&(phonesSet->list), var, val);   
}

/* ------------------------------------------------------------------------
    phonesSetAccessItf
   ------------------------------------------------------------------------ */

static ClientData phonesSetAccessItf (ClientData cd, char *name, TypeInfo **ti)
{ 
  PhonesSet* phonesSet = (PhonesSet*)cd;
  return listAccessItf((ClientData)&(phonesSet->list),name,ti);
}

/* ------------------------------------------------------------------------
    phonesSetPutsItf
   ------------------------------------------------------------------------ */

static int phonesSetPutsItf (ClientData clientData, int argc, char *argv[])
{
  PhonesSet *phonesSet = (PhonesSet*)clientData;

  argc--;
  if (itfParseArgv(argv[0], &argc, argv+1, 0,
        NULL) != TCL_OK)  return TCL_ERROR;

  return listPutsItf((ClientData)&(phonesSet->list), argc, argv);
}

/* ------------------------------------------------------------------------
    phonesSetIndex
   ------------------------------------------------------------------------ */

int phonesSetIndex( PhonesSet* phonesSet,  char* phone) 
{
  return listIndex((List*)&(phonesSet->list), phone, 0);
}

static int phonesSetIndexItf( ClientData clientData, int argc, char *argv[] )
{
  PhonesSet* phonesSet = (PhonesSet*)clientData;
  return listName2IndexItf((ClientData)&(phonesSet->list), argc, argv);
}

/* ------------------------------------------------------------------------
    phonesSetName
   ------------------------------------------------------------------------ */

char* phonesSetName( PhonesSet* phonesSet, int i) 
{ 
  return (i < 0) ? "(null)" : phonesSet->list.itemA[i].name;
}

static int phonesSetNameItf( ClientData clientData, int argc, char *argv[] )
{
  PhonesSet* phones = (PhonesSet*)clientData;

  argc--;
  if (itfParseArgv(argv[0], &argc, argv+1, 0,
        NULL) != TCL_OK)  return TCL_ERROR;

  return listIndex2NameItf((ClientData)&(phones->list), argc, argv);
}

/* ------------------------------------------------------------------------
    phonesSetNumber
   ------------------------------------------------------------------------ */

int phonesSetNumber( PhonesSet* phonesSet) { return phonesSet->list.itemN; }

/* ------------------------------------------------------------------------
    phonesSetAddItf
   ------------------------------------------------------------------------ */

int phonesSetAdd( PhonesSet* phonesSet, char* name, int argc, char *argv[])
{
  int idx = phonesSetIndex( phonesSet, name);

  if ( idx < 0) {
    idx = listNewItem((List*)&(phonesSet->list), name);
    return phonesAddItf((ClientData)(phonesSet->list.itemA+idx), argc, argv);
  }
  return TCL_OK;
}

static int phonesSetAddItf( ClientData clientData, int argc, char *argv[] )
{
  PhonesSet* phonesSet = (PhonesSet*)clientData;
  char*      pname     =  NULL;
  int        ac        =  argc-1;
  int        i;

  if (itfParseArgv( argv[0], &ac, argv+1, 0,
                   "<name>",  ARGV_STRING, NULL, &pname, NULL,"name of list",
		   "<phone*>",ARGV_REST,   NULL, &i,     NULL,"list of phones",
		   NULL) != TCL_OK)
       return TCL_ERROR;
  else return phonesSetAdd( phonesSet, pname, ac+1, argv);
}

/* ------------------------------------------------------------------------
    phonesSetDelete
   ------------------------------------------------------------------------ */

int phonesSetDelete( PhonesSet *phonesSet, int argc, char* argv[])
{
  int i;
  for ( i = 0; i < argc; i++) {
    if ( listDelete((List*)&(phonesSet->list), argv[i]) != TCL_OK) {
      SERROR( "Can't remove phone set '%s' from '%s'.\n",
               argv[i], phonesSet->name);
      return TCL_ERROR;
    }
  }
  return TCL_OK;
}

static int phonesSetDeleteItf( ClientData clientData, int argc, char *argv[])
{
  PhonesSet *phonesSet  = (PhonesSet*)clientData;
  int        retCode    =  TCL_OK;
  int        ac         =  argc-1;
  int        i;

  if (itfParseArgv( argv[0], &ac, argv+1, 0,
		   "<phoneSet*>",ARGV_REST,NULL,&i,NULL,"list of phone sets",
		   NULL) != TCL_OK)
    return TCL_ERROR;

  while ( i++ < ac) {
    int     tokc;
    char  **tokv;

    if ( Tcl_SplitList(itf,argv[i],&tokc,&tokv) != TCL_OK) return    TCL_ERROR;
    if ( phonesSetDelete( phonesSet,tokc, tokv) != TCL_OK) retCode = TCL_ERROR;
    Tcl_Free((char*)tokv);
  }
  return retCode;
}

/* ------------------------------------------------------------------------
    phonesSetRead
   ------------------------------------------------------------------------ */

int phonesSetRead( PhonesSet *phonesSet, char *filename)
{
  FILE *fp;
  char  line[4000];
  int   n  = 0;
  int   rc = TCL_OK;

  if ((fp=fileOpen(filename,"r"))==NULL) {
    ERROR( "Can't open phone set file '%s' for reading.\n",filename);
    return -1;
  }

  while (1) {
    int   argc; char **argv;
    char* p;

    strcpy(line,"add ");

    if ( fscanf(fp,"%[^\n]",&(line[4]))!=1) break; 
    else fscanf(fp,"%*c");

    if ( line[4] == phonesSet->commentChar) continue; 

    for (p=&(line[4]); *p!='\0'; p++) 
         if (*p>' ') break; if (*p=='\0') continue; 
      
    Tcl_SplitList (itf,line,&argc,&argv);
    if ( phonesSetAddItf ((ClientData)phonesSet, argc, argv) != TCL_OK)
         rc = TCL_ERROR;
    else n++;
    Tcl_Free((char*) argv);
  }
  fileClose(filename, fp);
  return (rc != TCL_OK) ? -1 : n;
}

static int phonesSetReadItf ( ClientData clientData, int argc, char *argv[] )
{
  PhonesSet* phonesSet = (PhonesSet*)clientData;
  char*      fname     =  NULL;
  int        ac        =  argc-1;
  int        n;

  if (itfParseArgv(argv[0], &ac, argv+1, 0,
		  "<filename>",ARGV_STRING,NULL,&fname,NULL,
                  "name of phone set file", NULL) != TCL_OK)
    return TCL_ERROR;

  itfAppendResult( "%d", n = phonesSetRead( phonesSet, fname));
  return (n >= 0) ? TCL_OK : TCL_ERROR;
}

/* ------------------------------------------------------------------------
    phonesSetWrite
   ------------------------------------------------------------------------ */

int phonesSetWrite (PhonesSet *phonesSet, char *filename)
{
  Phones* phones     = phonesSet->list.itemA;
  int     phonesSetN = phonesSet->list.itemN;
  FILE   *fp;

  if ((fp=fileOpen(filename,"w"))==NULL) {
    ERROR( "Can't open phone set file '%s' for writing.\n",filename);
    return TCL_ERROR;
  }

  fprintf( fp, "%c -------------------------------------------------------\n",
                phonesSet->commentChar);
  fprintf( fp, "%c  Name            : %s\n",  phonesSet->commentChar, 
                phonesSet->name);
  fprintf( fp, "%c  Type            : PhonesSet\n", phonesSet->commentChar);
  fprintf( fp, "%c  Number of Items : %d\n",  phonesSet->commentChar, 
                phonesSet->list.itemN);
  fprintf( fp, "%c  Date            : %s", phonesSet->commentChar,
                dateString());
  fprintf( fp, "%c -------------------------------------------------------\n",
                phonesSet->commentChar);

  for ( ; phonesSetN > 0; phonesSetN--, phones++) {
     fprintf(fp,"%-20s ", phones->name);
     phonesWriteFile( phones, fp);
  }
  fileClose(filename, fp);
  return TCL_OK;
}

static int phonesSetWriteItf (ClientData clientData, int argc, char *argv[] )
{
  char   *fname   =  NULL;
  int     ac      =  argc-1;

  if (itfParseArgv(argv[0], &ac, argv+1, 0,
		  "<filename>",ARGV_STRING,NULL,&fname,NULL,
                  "name of phones file", NULL) != TCL_OK)
    return TCL_ERROR;

  return phonesSetWrite((PhonesSet*)clientData, fname);
}

/* ========================================================================
    Type Declaration
   ======================================================================== */

static Method phoneMethod[] = 
{ 
  { "puts", phonePutsItf,   NULL },
  {  NULL,  NULL, NULL } 
};

TypeInfo phoneInfo = { "Phone", 0, 0, phoneMethod, 
                        NULL, NULL, NULL, NULL, itfTypeCntlDefaultNoLink,
                       "Phone\n" };

Method phonesMethod[] = 
{
  { "puts",   phonesPutsItf,   "displays the contents of a phone-set" },
  { "add",    phonesAddItf,    "add new phone(s) to a phone-set"      },
  { "delete", phonesDeleteItf, "delete phone(s) from a phone-set"     },
  { "read",   phonesReadItf,   "read a phone-set from a file"         },
  { "write",  phonesWriteItf,  "write a phone-set into a file"        },
  { "index",  phonesIndexItf,  "return index of named phone(s)"       },
  { "name",   phonesNameItf,   "return the name of indexed phone(s)"  },
  { NULL, NULL, NULL }
} ;

TypeInfo phonesInfo = { "Phones", 0, 0, phonesMethod,
                         phonesCreateItf,    phonesFreeItf, 
                         phonesConfigureItf, phonesAccessItf, 
			 NULL,
  "A 'Phones' object is an array of strings, each of which is a phoneme." };

Method phonesSetMethod[] = 
{
  { "puts",  phonesSetPutsItf,  "displays the contents of a set of phone-sets" },
  { "add",   phonesSetAddItf,   "add new phone-set to a set of phones-set"     },
  { "delete",phonesSetDeleteItf,"delete phone-set(s) from a set of phone-sets" },
  { "read",  phonesSetReadItf,  "read a set of phone-sets from a file"         },
  { "write", phonesSetWriteItf, "write a set of phone-sets into a file"        },
  { "index", phonesSetIndexItf, "return index of named phone-set(s)"           },
  { "name",  phonesSetNameItf,  "return the name of indexed phone-set(s)"      },
  { NULL, NULL, NULL }
} ;

TypeInfo phonesSetInfo = { "PhonesSet", 0, 0, phonesSetMethod, 
                            phonesSetCreateItf,    phonesSetFreeItf, 
                            phonesSetConfigureItf, phonesSetAccessItf, NULL,
  "A 'PhonesSet' object is a set of 'Phones' objects." };

static int phonesInitialized = 0;

int Phones_Init ( void )
{
  if (! phonesInitialized) {
    phonesDefault.name            = NULL;
    phonesDefault.useN            = 0;
    phonesDefault.list.itemN      = 0;
    phonesDefault.list.blkSize    = 10;
    phonesDefault.commentChar     = ';';

    phonesSetDefault.name         = NULL;
    phonesSetDefault.useN         = 0;
    phonesSetDefault.list.itemN   = 0;
    phonesSetDefault.list.blkSize = 20;
    phonesSetDefault.commentChar  = ';';

    itfNewType (&phoneInfo);
    itfNewType (&phonesInfo);
    itfNewType (&phonesSetInfo);

    Tcl_CreateCommand (itf,"phonesCharFunc",phonesCharFuncItf, 0, 0 );
    phonesInitialized = 1;
  }
  return TCL_OK;
}
