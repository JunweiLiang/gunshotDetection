/* ========================================================================
    JANUS      Speech- to Speech Translation
               ------------------------------------------------------------
               Object: Database Access
               ------------------------------------------------------------

    Author  :  Michael Finke
    Module  :  dbase.c
    Date    :  $Id: dbase.c 3412 2011-03-22 19:21:27Z metze $
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
    Revision 3.9  2005/05/04 14:54:32  metze
    Fixed segfault Lena suffered from in datDelete

    Revision 3.8  2003/08/14 11:19:50  fuegen
    Merged changes on branch jtk-01-01-15-fms (jaguar -> ibis-013)

    Revision 3.7.4.3  2003/07/02 17:09:23  fuegen
    fixes for file handling under Windows
    using general functions for file open/close

    Revision 3.7.4.2  2002/08/27 08:45:49  metze
    itfParseArgv now uses <name> and NULL initialization for char*

    Revision 3.7.4.1  2002/06/26 11:57:52  fuegen
    changes for new directory structure support and code cleaning

    Revision 3.7  2001/01/15 09:49:55  janus
    Merged Christian's version with WINDOWS support (branch jtk-00-11-14-fuegen)

    Revision 3.6.12.1  2001/01/12 15:16:51  janus
    necessary changes for running janus under WINDOWS

    Revision 3.6  2000/10/24 14:01:19  janus
    Merged changes on branch jtk-00-10-12-tschaaf

    Revision 3.5.28.1  2000/10/24 13:24:12  janus
    moved NDEBUG definition to optimizing definition.
    if NDEBUG is defined assert statements are removed during compilation

    Revision 3.5  2000/08/16 09:16:23  soltau
    Reaplaced free by Tcl_Free if memory was allocated by Tcl_*

    Revision 3.4  1998/01/12 17:53:23  tschaaf
    puts did crash with a segmentation fault
    Bug removed

    Revision 3.3  1997/07/25 12:12:46  tschaaf
    gcc / DFKI - clean

    Revision 3.2  1997/06/17 17:41:43  westphal
    *** empty log message ***

    Revision 1.19  96/03/04  17:39:22  rogina
    made all -Wall warnings go away
    
    Revision 1.18  1996/03/04  16:35:14  rogina
    make gcc -Wall conforming
    changed include sequence from first "" then <> to first <> then ""

    Revision 1.17  1996/02/05  09:32:38  manke
     Added #ifndef WINDOWS to make Visual C++ happy.

    Revision 1.16  1996/01/18  16:54:25  maier
    changed help for 'add', added ERROR message

    Revision 1.15  1996/01/12  16:04:03  finkem
    replaced dbaseRead by freadAdd

    Revision 1.14  1995/10/16  18:21:40  finkem
    got rid of setArray functionality for dbase get and add

    Revision 1.13  1995/09/25  19:02:01  finkem
    changed hash key function to be compatible between
    HP's and ALPHAs

    Revision 1.12  1995/09/22  01:21:17  finkem
    made  mode -r standard

    Revision 1.11  1995/09/21  21:50:28  finkem
    accessItf added, such that the index file can be configured.

    Revision 1.10  1995/09/06  07:16:25  kemp
    *** empty log message ***

    Revision 1.9  1995/08/29  02:47:09  finkem
    *** empty log message ***

    Revision 1.8  1995/08/22  14:30:05  rogina
    *** empty log message ***

    Revision 1.7  1995/08/22  13:55:51  rogina
    *** empty log message ***

    Revision 1.6  1995/08/21  16:18:06  rogina
    *** empty log message ***

    Revision 1.5  1995/08/21  16:16:43  rogina
    *** empty log message ***

    Revision 1.4  1995/08/17  17:07:53  rogina
    *** empty log message ***

    Revision 1.3  1995/08/17  13:34:10  finkem
    *** empty log message ***

    Revision 1.2  1995/08/16  12:56:28  finkem
    *** empty log message ***

    Revision 1.1  1995/08/11  16:34:08  finkem
    Initial revision

 
   ======================================================================== */

char dbaseRcsVersion[] =
       "@(#)1$Id: dbase.c 3412 2011-03-22 19:21:27Z metze $";

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#ifdef WINDOWS
	#include <io.h>
#endif

#include "common.h"
#include "itf.h"
#include "dbase.h"


/* ------------------------------------------------------------------------
    Forward Declaration
   ------------------------------------------------------------------------ */

extern TypeInfo dbaseInfo;

static long hashPrimes[] =
{
  1,  2,  3,  7,  13,  23,  59,  113,  241,  503,  1019,  2039,  4091,
  8179,  16369,  32749,  65521,  131063,  262139,  524269,  1048571,
  2097143,  4194287,  8388593,  16777199,  33554393,  67108859,
  134217689,  268435399,  536870879,  1073741789,  2147483629
};

#define HASHSIZE(L) (hashPrimes[(L)->hashSizeX])
int PTRSIZE=8;
/* CMU GALE Version=12, otherwise 8 */

/* ------------------------------------------------------------------------
    hashStringKey & hashStringCmp
   ------------------------------------------------------------------------ */

static off_t hashStringKey(char *s)
{
  char *s1   = (char*)s;
  off_t hash =  *s1++;

  while (*s1) {
    hash = (( hash << 5 ) + *s1) % 33554393;
    /* hash += *s1; */
    /* hash  = ((hash << 5) | (hash >> (8*sizeof(long)-5))) ^ *s1; */
    ++s1;
  }
  return (hash);
}

static int hashStringCmp(char *s1, char *s2)
{ 
  for(; *s1 == *s2; s1++, s2++) if (*s1 == '\0') return 1;
  return 0;
}

/* ========================================================================
    DBase Index File
   ======================================================================== */

static long  dbaseIdxRewind(   DBaseIdx* dbxP);
static off_t dbaseIdxReadPtr(  DBaseIdx* dbxP, off_t offset);
static off_t dbaseIdxRead(     DBaseIdx* dbxP, off_t offset);
static off_t dbaseIdxWritePtr( DBaseIdx* dbxP, off_t offset, off_t ptr);
static off_t dbaseIdxWrite(    DBaseIdx* dbxP, const char* key, off_t offset);
static off_t dbaseIdxFind(     DBaseIdx* dbxP, char* keyS);
static off_t dbaseIdxFindFree( DBaseIdx* dbxP, int keySize, int valSize);
static int   dbaseIdxDel(      DBaseIdx* dbxP, off_t offset);

static DBaseIdx dbaseIdxDefault;

/* ------------------------------------------------------------------------
    Create/Init DBaseIdx object
   ------------------------------------------------------------------------ */

static int dbaseIdxInit( DBaseIdx* dbxP, ClientData CD)
{
  dbxP->name      =  strdup((char*)CD);
  dbxP->useN      =  0;

  dbxP->hashSizeX =  dbaseIdxDefault.hashSizeX;
  dbxP->hashKey   = (DbHashKeyFn*)hashStringKey;
  dbxP->hashCmp   = (DbHashCmpFn*)hashStringCmp;

  dbxP->idxFd     = -1;
  dbxP->idxLen    =  0;
  dbxP->idxOff    =  0;
  dbxP->datLen    =  0;
  dbxP->datOff    =  0;
  dbxP->idxBufP   =  NULL;
  dbxP->idxBufN   =  0;

  dbxP->commentChar  =  ';';
  return TCL_OK;
}

DBaseIdx* dbaseIdxCreate( char* name)
{
  DBaseIdx *dbxP = (DBaseIdx*)malloc(sizeof(DBaseIdx));

  if (! dbxP || dbaseIdxInit( dbxP, (ClientData)name) != TCL_OK) {
     if ( dbxP) free( dbxP);
     SERROR("Failed to allocate DBaseIdx object '%s'.\n", name);
     return NULL; 
  }
  return dbxP;
}

static ClientData dbaseIdxCreateItf( ClientData clientData, 
                                     int argc, char *argv[]) 
{
  DBaseIdx* dbxP = dbaseIdxCreate( argv[0]);
  return (ClientData)dbxP;
}

/* ------------------------------------------------------------------------
    Free/Deinit DBaseIdx Object
   ------------------------------------------------------------------------ */

static int dbaseIdxDeinit( DBaseIdx* dbxP)
{
  if (dbxP==NULL) { ERROR("null argument"); return TCL_ERROR; }

  if (dbxP->useN) 
  { SERROR("DBaseIdX object still in use by %d other object(s).\n",dbxP->useN);
    return TCL_ERROR;
  }
  
  if ( dbxP->name)    { free( dbxP->name);    dbxP->name    = NULL; }
  if ( dbxP->idxBufP) { free( dbxP->idxBufP); dbxP->idxBufP = NULL; }
  return TCL_OK;
}

static int dbaseIdxFree( DBaseIdx* dbxP)
{
  if (dbaseIdxDeinit(dbxP)!=TCL_OK) return TCL_ERROR;
  free(dbxP);
  return TCL_OK;
}

static int dbaseIdxFreeItf( ClientData clientData) 
{ return dbaseIdxFree( (DBaseIdx*)clientData); }

/* ------------------------------------------------------------------------
    dbaseIdxConfigureItf
   ------------------------------------------------------------------------ */

static int dbaseIdxConfigureItf( ClientData cd, char *var, char *val)
{
  DBaseIdx*   dbxP = (DBaseIdx*)cd;
  if (! dbxP) dbxP = &dbaseIdxDefault;

  if (var==NULL) {
    ITFCFG(dbaseIdxConfigureItf,cd,"hashSizeX");
    return TCL_OK;
  }
  ITFCFG_Int(    var,val,"hashSizeX", dbxP->hashSizeX, (
                                      dbxP->idxFd >=0) ? 1 : 0);
  return TCL_ERROR;
}

/* ------------------------------------------------------------------------
    dbaseIdxOpen
   ------------------------------------------------------------------------ */

int dbaseIdxOpen( DBaseIdx* dbxP, char* file, int oflag, int mode)
{
  int n;
  char a[20];

  if (dbxP==NULL) { ERROR("Null dbxP\n"); return TCL_ERROR; }
  if (dbxP->idxFd!=(-1)) return TCL_OK; /* already open */

  if ((dbxP->idxFd = open( file, oflag, mode)) < 0) {
    SERROR("Can't open index file '%s'.\n", file);
    return TCL_ERROR;
  }

  /* Check for PTRSIZE */
  read (dbxP->idxFd, a, 20);
  lseek (dbxP->idxFd, 0, SEEK_SET);

  if (!(oflag & (O_CREAT|O_TRUNC))) {
    for (n = 0; n < 20 && a[n] == ' '; n++);
    for (     ; n < 20 && a[n] != ' '; n++);
    if (n == 20) {
      WARN ("dbaseIdxOpen: bad index file?\n");
    } else if (n != PTRSIZE) {
      INFO ("dbaseIdxOpen: PTRSIZE=%d, changing to %d\n", PTRSIZE, n);
      PTRSIZE = n;
    }
  }

  if ((oflag & (O_CREAT|O_TRUNC)) == (O_CREAT|O_TRUNC)) {
    struct stat statBuf;

    n = fstat( dbxP->idxFd, &statBuf);
    assert ( n >= 0);

    if ( statBuf.st_size == 0) {
      off_t offset = 2 * PTRSIZE;
      int   i;

      dbaseIdxWritePtr( dbxP, 0,       0);
      dbaseIdxWritePtr( dbxP, PTRSIZE, dbxP->hashSizeX);

      for ( i = HASHSIZE(dbxP); i >  0; i--, offset += PTRSIZE)
        dbaseIdxWritePtr( dbxP, offset, 0);
      write( dbxP->idxFd, "\n", 1);
    }
  }

  dbxP->hashSizeX = dbaseIdxReadPtr( dbxP, PTRSIZE);
  dbxP->oflag     = oflag;
  dbxP->mode      = mode;

  dbaseIdxRewind(dbxP);
  return TCL_OK;
}

static int dbaseIdxOpenItf( ClientData clientData, int argc, char *argv[]) 
{
  DBaseIdx* dbxP  = (DBaseIdx*)clientData;
  char*     fname =  NULL;
  char*     modeS = "r";
  int       mode  =  O_RDONLY;
  int       ac    =  argc-1;

  if (itfParseArgv(argv[0], &ac, argv+1, 0,
     "<filename>",ARGV_STRING,NULL,&fname,NULL,"name of index file", 
     "-mode",     ARGV_STRING,NULL,&modeS,NULL,"r | rw | rwc",
      NULL) != TCL_OK) return TCL_ERROR;

  if      (! strcmp(modeS,"rw"))  mode = O_RDWR;
  else if (! strcmp(modeS,"rwc")) mode = O_RDWR|O_CREAT|O_TRUNC;

#ifdef WINDOWS
  mode |= O_BINARY;
#endif

  return dbaseIdxOpen( dbxP, fname, mode, 0644);
}

/* ------------------------------------------------------------------------
    dbaseIdxClose
   ------------------------------------------------------------------------ */

int dbaseIdxClose( DBaseIdx* dbxP)
{
  if (dbxP && dbxP->idxFd > -1) close(dbxP->idxFd); 
  if (dbxP) dbxP->idxFd = -1;
  return TCL_OK;
}

static int dbaseIdxCloseItf (ClientData clientData, int argc, char *argv[]) 
{
  DBaseIdx*  dbxP = (DBaseIdx*)clientData;

  argc--;
  if (itfParseArgv(argv[0], &argc, argv+1, 0,
        NULL) != TCL_OK)  return TCL_ERROR;

  return dbaseIdxClose( dbxP);
}

/* ------------------------------------------------------------------------
    dbaseIdxGet
   ------------------------------------------------------------------------ */

off_t dbaseIdxGet( DBaseIdx* dbxP, char* key, int* valSize)
{
  if ( dbxP->idxFd < 0) return (off_t) 0;

  if (dbaseIdxFind( dbxP, key)) {
    if (valSize) *valSize = dbxP->datLen;
    return dbxP->datOff;
  }
  else return -1;
}

static int dbaseIdxGetItf (ClientData clientData, int argc, char *argv[]) 
{
  DBaseIdx* dbxP = (DBaseIdx*)clientData;
  char*     key  =  NULL;
  off_t     off  =  0;
  int       size =  0;
  int       ac   =  argc-1;

  if (itfParseArgv(argv[0], &ac, argv+1, 0,
     "<key>",ARGV_STRING,NULL,&key,NULL,"key", NULL) != TCL_OK)
    return TCL_ERROR;

  if ((off = dbaseIdxGet( dbxP, key, &size)) >= 0) {
    itfAppendResult( "%d %d", off, size); 
    return TCL_OK;
  }
  else {
    SERROR("DBaseIdx '%s' key '%s' not found.\n", dbxP->name, key);
    return TCL_ERROR;
  }
}

/* ------------------------------------------------------------------------
    dbaseIdxFirst
   ------------------------------------------------------------------------ */

char* dbaseIdxFirst( DBaseIdx* dbxP)
{
  dbxP->idxOff = 0;
  return dbaseIdxNext( dbxP);
}

static int dbaseIdxFirstItf (ClientData clientData, int argc, char *argv[]) 
{
  DBaseIdx*  dbxP = (DBaseIdx*)clientData;
  char*      key;

  argc--;
  if (itfParseArgv(argv[0], &argc, argv+1, 0,
        NULL) != TCL_OK)  return TCL_ERROR;

  if ((key = dbaseIdxFirst(dbxP))) itfAppendResult("%s", key); 
  else                             itfAppendResult("");
  return TCL_OK;
}

/* ------------------------------------------------------------------------
    dbaseIdxNext
   ------------------------------------------------------------------------ */

char* dbaseIdxNext( DBaseIdx* dbxP)
{
  off_t     offset;
  char      tmp[PTRSIZE * 4];
  char      c;

  if (! dbxP->idxOff) 
        offset = dbaseIdxRewind(dbxP);
  else  offset = dbxP->idxOff + 4 * PTRSIZE + (dbxP->idxLen+1);

  do {
    lseek(dbxP->idxFd, offset, SEEK_SET); 

    if ( read( dbxP->idxFd, tmp, PTRSIZE * 4) == PTRSIZE*4) {
      char* ptr;
      dbaseIdxRead( dbxP, offset);

      ptr = dbxP->idxBufP;
      while ((c = *ptr++) && c == ' ');
    }
    else return NULL;
    offset = dbxP->idxOff + 4 * PTRSIZE + (dbxP->idxLen+1);
  } while ( c == 0);
  return dbxP->idxBufP;
}

static int dbaseIdxNextItf (ClientData clientData, int argc, char *argv[]) 
{
  DBaseIdx*  dbxP = (DBaseIdx*)clientData;
  char*      key;

  argc--;
  if (itfParseArgv(argv[0], &argc, argv+1, 0,
        NULL) != TCL_OK)  return TCL_ERROR;

  if ((key = dbaseIdxNext(dbxP))) itfAppendResult( "%s", key);
  else                            itfAppendResult( "");
  return TCL_OK;
}

/* ------------------------------------------------------------------------
    dbaseList
   ------------------------------------------------------------------------ */

static int dbaseIdxListItf (ClientData clientData, int argc, char *argv[]) 
{
  DBaseIdx*  dbxP = (DBaseIdx*)clientData;
  char*      key;

  argc--;
  if (itfParseArgv(argv[0], &argc, argv+1, 0,
        NULL) != TCL_OK)  return TCL_ERROR;

  for ( key = dbaseIdxFirst(dbxP); key != NULL; 
        key = dbaseIdxNext( dbxP)) {
    itfAppendElement( "%s", key);
  }
  return TCL_OK;
}

/* ------------------------------------------------------------------------
    dbaseIdxPuts
   ------------------------------------------------------------------------ */

static int dbaseIdxPutsItf (ClientData clientData, int argc, char *argv[]) 
{
  return dbaseIdxListItf( clientData, argc, argv);
}

/* ------------------------------------------------------------------------
    dbaseIdxRewind
   ------------------------------------------------------------------------ */

static long dbaseIdxRewind( DBaseIdx* dbxP)
{
  long offset = (HASHSIZE(dbxP)+2) * PTRSIZE;
  return (dbxP->idxOff = lseek(dbxP->idxFd,offset+1,SEEK_SET));
}

/* ------------------------------------------------------------------------
    dbaseIdxReadPtr
   ------------------------------------------------------------------------ */

static off_t dbaseIdxReadPtr( DBaseIdx* dbxP, off_t offset)
{
  char str[PTRSIZE+1];

  lseek( dbxP->idxFd, offset, SEEK_SET);
  read(  dbxP->idxFd, str,    PTRSIZE);
  str[PTRSIZE] = 0;
  return atol(str);
}

/* ------------------------------------------------------------------------
    dbaseIdxRead
   ------------------------------------------------------------------------ */

static off_t dbaseIdxRead( DBaseIdx* dbxP, off_t offset)
{
  dbxP->idxOff  = offset;
  dbxP->idxNext = dbaseIdxReadPtr( dbxP, offset);
  dbxP->datOff  = dbaseIdxReadPtr( dbxP, offset + PTRSIZE);
  dbxP->datLen  = dbaseIdxReadPtr( dbxP, offset + PTRSIZE * 2);
  dbxP->idxLen  = dbaseIdxReadPtr( dbxP, offset + PTRSIZE * 3);

  if ( dbxP->idxLen > dbxP->idxBufN) {
    dbxP->idxBufP = (char*)realloc(dbxP->idxBufP, 
                                  (dbxP->idxLen+1) * sizeof(char));
    dbxP->idxBufN =  dbxP->idxLen;
  }
  read( dbxP->idxFd, dbxP->idxBufP, dbxP->idxLen);
  dbxP->idxBufP[dbxP->idxLen] = 0;
  return dbxP->idxNext; 
}

/* ------------------------------------------------------------------------
    dbaseIdxWritePtr
   ------------------------------------------------------------------------ */

static off_t dbaseIdxWritePtr( DBaseIdx* dbxP, off_t offset, off_t ptr)
{
  char  str[PTRSIZE+1];
  off_t off;

  sprintf( str, "%*d", PTRSIZE, (int)ptr);

         off  = lseek( dbxP->idxFd, offset, SEEK_SET);
         off += write( dbxP->idxFd, str,    PTRSIZE);
  return off;
}

/* ------------------------------------------------------------------------
    dbaseIdxWrite
   ------------------------------------------------------------------------ */

static off_t dbaseIdxWrite( DBaseIdx* dbxP, const char* key, off_t offset)
{
  int keySize  = strlen( key);
  dbxP->idxLen = keySize;

  dbaseIdxWritePtr( dbxP, offset,               dbxP->idxNext);
  dbaseIdxWritePtr( dbxP, offset + PTRSIZE,     dbxP->datOff);
  dbaseIdxWritePtr( dbxP, offset + PTRSIZE * 2, dbxP->datLen);
  dbaseIdxWritePtr( dbxP, offset + PTRSIZE * 3, dbxP->idxLen);

  if ( dbxP->idxLen > dbxP->idxBufN) {
    dbxP->idxBufP = (char*)realloc(dbxP->idxBufP, 
                                  (dbxP->idxLen+1) * sizeof(char));
    dbxP->idxBufN =  dbxP->idxLen;
  }
  memcpy( dbxP->idxBufP, key, keySize * sizeof(char));
  dbxP->idxBufP[dbxP->idxLen] = 0;

  write( dbxP->idxFd, dbxP->idxBufP, dbxP->idxLen);
  write( dbxP->idxFd, "\n", 1);
  return offset + 4*PTRSIZE + keySize + 1;
}

/* ------------------------------------------------------------------------
    dbaseIdxFind
   ------------------------------------------------------------------------ */

static off_t dbaseIdxFind( DBaseIdx* dbxP, char* keyS)
{
  off_t key = dbxP->hashKey( keyS);
  off_t hX  = key % HASHSIZE(dbxP);
  off_t off;

  if ( hX < 0) hX = -hX;

  dbxP->idxPtr  = (hX + 2) * PTRSIZE;
  dbxP->hashOff =  dbxP->idxPtr;
  off           =  dbaseIdxReadPtr( dbxP, dbxP->idxPtr);

  while ( off) {
    off_t next = dbaseIdxRead( dbxP, off);

    if ( dbxP->hashCmp( keyS, dbxP->idxBufP)) break;

    dbxP->idxPtr = off;
    off          = next;
  }
  return off;
}

/* ------------------------------------------------------------------------
    dbaseIdxFindFree
   ------------------------------------------------------------------------ */

static off_t dbaseIdxFindFree( DBaseIdx* dbxP, int keySize, int valSize)
{
  off_t ptr    = 0;
  off_t offset = dbaseIdxReadPtr( dbxP, ptr);

  while ( offset) {
    off_t next = dbaseIdxRead( dbxP, offset);
    if ( dbxP->idxLen == keySize && 
       ( valSize < 1 || dbxP->datLen == valSize)) break;
    ptr    = offset;
    offset = next;
  }
  if ( offset) dbaseIdxWritePtr( dbxP, ptr, dbxP->idxNext);
  return offset;
}

/* ------------------------------------------------------------------------
    dbaseIdxDel
   ------------------------------------------------------------------------ */

static int dbaseIdxDel( DBaseIdx* dbxP, off_t offset)
{
  char* ptr;

  (void)dbaseIdxReadPtr( dbxP, 0);
  ptr = dbxP->idxBufP;

  while (*ptr) *ptr++ = ' ';

  dbaseIdxWritePtr( dbxP, dbxP->idxPtr, dbxP->idxNext);

  dbxP->idxNext = dbaseIdxReadPtr( dbxP, 0);

  dbaseIdxWritePtr( dbxP, 0, offset);
  dbaseIdxWrite(    dbxP, dbxP->idxBufP, offset);
  return TCL_OK;
}

/* ------------------------------------------------------------------------
    dbaseIdxAdd
   ------------------------------------------------------------------------ */

off_t dbaseIdxAdd( DBaseIdx* dbxP, char* key, off_t valOff, int valSize)
{
  off_t offset;
  int   keySize = strlen(key);

  if ( dbxP->idxFd < 0 || ! (dbxP->oflag & O_RDWR)) return 0;

  if ((offset = dbaseIdxFind( dbxP, key))) {
    if ( valOff != dbxP->datOff || valSize != dbxP->datLen) {
        
      dbxP->datOff = valOff;
      dbxP->datLen = valSize;

      dbaseIdxWritePtr( dbxP, offset + PTRSIZE,     dbxP->datOff);
      dbaseIdxWritePtr( dbxP, offset + PTRSIZE * 2, dbxP->datLen);
    }
  }
  else {
    if ((offset = dbaseIdxFindFree( dbxP, keySize, valSize))) {

      dbxP->idxNext = dbaseIdxReadPtr( dbxP, dbxP->hashOff);
      dbaseIdxWrite(    dbxP, key, offset);
      dbaseIdxWritePtr( dbxP, dbxP->hashOff, offset);      
    }
    else {
      off_t ioff = lseek( dbxP->idxFd, 0, SEEK_END);

      dbxP->idxNext = dbaseIdxReadPtr( dbxP, dbxP->hashOff);
      dbxP->datOff  = valOff;
      dbxP->datLen  = valSize;

      dbaseIdxWrite(    dbxP, key, ioff);
      dbaseIdxWritePtr( dbxP, dbxP->hashOff, ioff);    
      offset = ioff;
    }
  }
  return offset;
}

static int dbaseIdxAddItf (ClientData clientData, int argc, char *argv[]) 
{
  DBaseIdx* dbxP   = (DBaseIdx*)clientData;
  char*     key    =  NULL;
  int       offset =  0;
  int       size   =  0;
  int       ac     =  argc-1;

  if (itfParseArgv(argv[0], &ac, argv+1, 0,
     "<key>",    ARGV_STRING,NULL,&key,   NULL,"key", 
     "<offset>", ARGV_INT,   NULL,&offset,NULL,"offset", 
     "<size>",   ARGV_INT,   NULL,&size,  NULL,"size", NULL) != TCL_OK)
      return TCL_ERROR;

  if (! dbaseIdxAdd( dbxP, key, offset, size))
    return TCL_ERROR;
  else {
    return TCL_OK;
  }
}

/* ------------------------------------------------------------------------
    dbaseIdxDelete
   ------------------------------------------------------------------------ */

int dbaseIdxDelete( DBaseIdx* dbxP, char* key)
{
  off_t offset;

  if ( dbxP->idxFd < 0 || ! (dbxP->oflag & O_RDWR)) return 0;

  if ((offset = dbaseIdxFind( dbxP, key))) { 
    dbaseIdxDel( dbxP, offset);
    return TCL_OK;
  }
  return TCL_ERROR;
}

static int dbaseIdxDeleteItf (ClientData clientData, int argc, char *argv[]) 
{
  DBaseIdx*  dbxP = (DBaseIdx*)clientData;
  char*      key  =  NULL;
  int        ac   =  argc-1;

  if (itfParseArgv(argv[0], &ac, argv+1, 0,
		  "<key>",ARGV_STRING,NULL,&key,NULL,"key", 
                   NULL) != TCL_OK)
    return TCL_ERROR;

  if ( dbaseIdxDelete( dbxP, key) != TCL_OK) {

    SERROR("Can't delete '%s' in DBaseIdx '%s'.\n", key, dbxP->name);
    return TCL_ERROR;
  }
  return TCL_OK;
}

/* ========================================================================
    Data File
   ======================================================================== */
/* ------------------------------------------------------------------------
    datRead
   ------------------------------------------------------------------------ */

static char* datRead( DBase* dbaseP)
{
  DBaseIdx* dbxP    = &(dbaseP->dbaseIdx);

  lseek( dbaseP->datFd, dbxP->datOff, SEEK_SET);

  if ( dbxP->datLen > dbaseP->datBufN) {

    dbaseP->datBufP = (char*)realloc(dbaseP->datBufP, 
                                    (dbxP->datLen+1) * sizeof(char));
    dbaseP->datBufN =  dbxP->datLen;
  }
  read( dbaseP->datFd, dbaseP->datBufP, dbxP->datLen);
  dbaseP->datBufP[dbxP->datLen] = 0;
  return dbaseP->datBufP;
}

/* ------------------------------------------------------------------------
    datWrite
   ------------------------------------------------------------------------ */

static off_t datWrite( DBase* dbaseP, const char* val, int valSize,
                       off_t offset)
{
  DBaseIdx* dbxP = &(dbaseP->dbaseIdx);

  dbxP->datOff = lseek( dbaseP->datFd, offset, SEEK_SET);
  dbxP->datLen = valSize;

  if ( dbxP->datLen > dbaseP->datBufN) {
    dbaseP->datBufP = (char*)realloc(dbaseP->datBufP, 
                                    (dbxP->datLen+1) * sizeof(char));
    dbaseP->datBufN =  dbxP->datLen;
  }
  memcpy( dbaseP->datBufP, val, dbxP->datLen * sizeof(char));

  write( dbaseP->datFd, dbaseP->datBufP, dbxP->datLen);
  write( dbaseP->datFd, "\n", 1);
  return dbxP->datOff;
}

/* ------------------------------------------------------------------------
    datDelete
   ------------------------------------------------------------------------ */

static int datDelete( DBase* dbaseP)
{
  DBaseIdx* dbxP = &(dbaseP->dbaseIdx);
  int i;

  if ( dbxP->datLen > dbaseP->datBufN) {
    dbaseP->datBufP = (char*) realloc (dbaseP->datBufP, 
				       (dbxP->datLen+1) * sizeof(char));
    dbaseP->datBufN =  dbxP->datLen;
  }

  for ( i = 0; i < dbxP->datLen; i++) dbaseP->datBufP[i] = ' ';

  lseek( dbaseP->datFd, dbxP->datOff,  SEEK_SET);
  write( dbaseP->datFd, dbaseP->datBufP, dbxP->datLen);
  return TCL_OK;
}

/* ========================================================================
    DBase
   ======================================================================== */
/* ------------------------------------------------------------------------
    Create/Init DBase object
   ------------------------------------------------------------------------ */

int dbaseInit( DBase* dbaseP, ClientData CD)
{
  dbaseP->name      =  strdup((char*)CD);
  dbaseP->useN      =  0;

  dbaseIdxInit( &(dbaseP->dbaseIdx), CD);

  dbaseP->datFd     = -1;
  dbaseP->datBufP   =  NULL;
  dbaseP->datBufN   =  0;

  dbaseP->commentChar  =  ';';
  return TCL_OK;
}

DBase* dbaseCreate( char* name)
{
  DBase *dbaseP = (DBase*)malloc(sizeof(DBase));

  if (! dbaseP || dbaseInit( dbaseP, (ClientData)name) != TCL_OK) {
     if ( dbaseP) free( dbaseP);
     SERROR("Failed to allocate DBase object '%s'\n.", name);
     return NULL; 
  }
  return dbaseP;
}

static ClientData dbaseCreateItf( ClientData clientData, 
                                  int argc, char *argv[]) 
{
  char       *name = NULL; 
  int           ac = argc;

  if (itfParseArgv (argv[0], &ac, argv, 0,
      "<name>",   ARGV_STRING, NULL, &name, NULL, "name of the database",
        NULL) != TCL_OK) return NULL;

  return (ClientData) dbaseCreate (name);
}


/* ------------------------------------------------------------------------
    Free/Deinit DBase Object
   ------------------------------------------------------------------------ */

int dbaseDeinit( DBase* dbaseP)
{
  if (dbaseP==NULL) { ERROR("null argument"); return TCL_ERROR; }

  if (dbaseP->useN) 
  { SERROR("DBase object still in use by %d other object(s).\n", dbaseP->useN);
    return TCL_ERROR;
  }

  if ( dbaseP->name)    { free( dbaseP->name);    dbaseP->name    = NULL; }
  if ( dbaseP->datBufP) { free( dbaseP->datBufP); dbaseP->datBufP = NULL; }

  return dbaseIdxDeinit( &(dbaseP->dbaseIdx));
}

int dbaseFree( DBase* dbaseP)
{
  if (dbaseDeinit(dbaseP)!=TCL_OK) return TCL_ERROR;
  free(dbaseP);
  return TCL_OK;
}

static int dbaseFreeItf( ClientData clientData) 
{ return dbaseFree( (DBase*)clientData); }

/* ------------------------------------------------------------------------
    dbaseConfigureItf
   ------------------------------------------------------------------------ */

static int dbaseConfigureItf( ClientData cd, char *var, char *val)
{
  DBase *dbaseP = (DBase*)cd;
  if (! dbaseP) return TCL_ERROR;

  if (var==NULL) {
    return TCL_OK;
  }
  return TCL_ERROR;
}

/* ------------------------------------------------------------------------
    dbaseAccessItf
   ------------------------------------------------------------------------ */

static ClientData dbaseAccessItf( ClientData cd, char *name, TypeInfo **ti)
{ 
  DBase* dbaseP  = (DBase*)cd;

  if (*name == '.') { 
    if (name[1]=='\0') {
      itfAppendElement( "dbaseIdx");
      *ti = NULL; return NULL;
    }
    else {
      if (! strcmp( name+1, "dbaseIdx")) {
        *ti = itfGetType("DBaseIdx");
        return (ClientData)&(dbaseP->dbaseIdx);
      }
    }
  }
  return NULL;
}

/* ------------------------------------------------------------------------
    dbaseOpen
   ------------------------------------------------------------------------ */

int dbaseOpen( DBase* dbaseP, char* datFile, char* idxFile, 
               int oflag, int mode)
{
  if (dbaseP==NULL) { ERROR("Null dbaseP\n"); return TCL_ERROR; }
  if (dbaseP->datFd!=(-1)) return TCL_OK; /* already open */

  if ( dbaseIdxOpen( &(dbaseP->dbaseIdx), idxFile, oflag, mode) != TCL_OK)
    return TCL_ERROR;

  dbaseP->oflag = oflag;
  dbaseP->mode  = mode;

  if ((dbaseP->datFd = open( datFile, oflag, mode)) < 0) {
    SERROR("Can't open data file '%s'.\n", datFile);
    dbaseIdxClose(&(dbaseP->dbaseIdx)); 
    return TCL_ERROR;
  }
  return TCL_OK;
}

static int dbaseOpenItf( ClientData clientData, int argc, char *argv[]) 
{
  DBase*  dbaseP = (DBase*)clientData;
  char*   fname  =  NULL;
  char*   index  =  NULL;
  char*   modeS  = "r";
  int     mode   =  O_RDONLY;
  int     ac     =  argc-1;

  if (itfParseArgv(argv[0], &ac, argv+1, 0,
     "<file>", ARGV_STRING,NULL,&fname,NULL,"name of database file", 
     "<index>",ARGV_STRING,NULL,&index,NULL,"name of index file", 
     "-ptrSize",ARGV_INT,  NULL,&PTRSIZE,NULL,"size of pointer (use when creating only, global parameter)",
     "-mode",  ARGV_STRING,NULL,&modeS,NULL,"r | rw | rwc",
      NULL) != TCL_OK)
    return TCL_ERROR;

  if      (! strcmp(modeS,"rw"))  mode = O_RDWR;
  else if (! strcmp(modeS,"rwc")) mode = O_RDWR|O_CREAT|O_TRUNC;

#ifdef WINDOWS
  mode |= O_BINARY;
#endif

  return dbaseOpen( dbaseP, fname, index, mode, 0644);
}

/* ------------------------------------------------------------------------
    dbaseClose
   ------------------------------------------------------------------------ */

int dbaseClose( DBase* dbaseP)
{
  if (dbaseP) {
      dbaseIdxClose( &(dbaseP->dbaseIdx));
      if ( dbaseP->datFd > -1) close(dbaseP->datFd); 
      dbaseP->datFd = -1;
  }
  return TCL_OK;
}

static int dbaseCloseItf (ClientData clientData, int argc, char *argv[]) 
{
  DBase*  dbaseP = (DBase*)clientData;

  argc--;
  if (itfParseArgv(argv[0], &argc, argv+1, 0,
        NULL) != TCL_OK)  return TCL_ERROR;

  return dbaseClose( dbaseP);
}

/* ------------------------------------------------------------------------
    dbaseAdd
   ------------------------------------------------------------------------ */

int dbaseAdd( DBase* dbaseP, char* key, char* val, int valSize)
{
  off_t     offset;
  DBaseIdx* dbxP    = &(dbaseP->dbaseIdx);
  int       keySize =   strlen(key);

  if ( dbxP->idxFd < 0 || ! (dbaseP->oflag & O_RDWR)) {
     ERROR("Open dbase first for read/write!\n");
     return 0;
  }

  if ((offset = dbaseIdxFind( dbxP, key))) {
    if ( valSize != dbxP->datLen) {
      off_t doff = lseek( dbaseP->datFd, 0, SEEK_END);
      off_t ioff = lseek( dbxP->idxFd,   0, SEEK_END);

      dbaseIdxDel( dbxP, offset);
      datDelete( dbaseP);

      dbxP->idxNext = dbaseIdxReadPtr( dbxP, dbxP->hashOff);

      datWrite( dbaseP, val, valSize, doff);

      dbaseIdxWrite(    dbxP, key, ioff);
      dbaseIdxWritePtr( dbxP, dbxP->hashOff, ioff);
      offset = ioff;
    }
    else datWrite( dbaseP, val, valSize, dbxP->datOff);
  }
  else {
    if ((offset = dbaseIdxFindFree( dbxP, keySize, valSize))) {

      dbxP->idxNext = dbaseIdxReadPtr( dbxP, dbxP->hashOff);

      datWrite(    dbaseP, val, valSize,    dbxP->datOff);
      dbaseIdxWrite(    dbxP, key, offset);
      dbaseIdxWritePtr( dbxP, dbxP->hashOff, offset);      
    }
    else {
      off_t doff = lseek( dbaseP->datFd, 0, SEEK_END);
      off_t ioff = lseek( dbxP->idxFd,   0, SEEK_END);

      dbxP->idxNext = dbaseIdxReadPtr( dbxP, dbxP->hashOff);
      dbxP->datOff  = doff;

      datWrite(    dbaseP, val, valSize, doff);
      dbaseIdxWrite(    dbxP, key, ioff);
      dbaseIdxWritePtr( dbxP, dbxP->hashOff, ioff);    
      offset = ioff;
    }
  }
  return offset;
}

static int dbaseAddItf (ClientData clientData, int argc, char *argv[]) 
{
  DBase*  dbaseP = (DBase*)clientData;
  char*   key    =  NULL;
  char*   value  =  NULL;
  int     ac     =  argc-1;

  if (itfParseArgv(argv[0], &ac, argv+1, 0,
     "<key>",    ARGV_STRING,NULL,&key,  NULL,"key", 
     "<list>", ARGV_STRING,NULL,&value,NULL,"list of {varName varValue}", 
       NULL) != TCL_OK)
    return TCL_ERROR;

  if (! dbaseAdd( dbaseP, key, value, strlen(value)))
    return TCL_ERROR;
  else {
    return TCL_OK;
  }
}

/* ------------------------------------------------------------------------
    dbaseDelete
   ------------------------------------------------------------------------ */

int dbaseDelete( DBase* dbaseP, char* key)
{
  off_t     offset;
  DBaseIdx* dbxP    = &(dbaseP->dbaseIdx);

  if ( dbxP->idxFd < 0 || ! (dbaseP->oflag & O_RDWR)) return 0;

  if ((offset = dbaseIdxFind( dbxP, key))) { 
    dbaseIdxDel( dbxP, offset);
    datDelete( dbaseP);    
    return TCL_OK;
  }
  return TCL_ERROR;
}

static int dbaseDeleteItf (ClientData clientData, int argc, char *argv[]) 
{
  DBase*  dbaseP = (DBase*)clientData;
  char*   key    =  NULL;
  int     ac     =  argc-1;

  if (itfParseArgv(argv[0], &ac, argv+1, 0,
		  "<key>",ARGV_STRING,NULL,&key,NULL,"key", 
                   NULL) != TCL_OK)
    return TCL_ERROR;

  if ( dbaseDelete( dbaseP, key) != TCL_OK) {

    SERROR("Can't delete '%s' in DBase '%s'.\n", key, dbaseP->name);
    return TCL_ERROR;
  }
  return TCL_OK;
}

/* ------------------------------------------------------------------------
    dbaseSetArray
   ------------------------------------------------------------------------ */

int dbaseSetArray( DBase* dbaseP, char *value)
{
  int    tokc;
  char** tokv;
  int    i;
  char   name[1000];

  sprintf( name, "if { [array exists %s]} { unset %s} ", dbaseP->name,
                                                         dbaseP->name);
  Tcl_Eval( itf, name);

  Tcl_SplitList( itf, value, &tokc, &tokv);
  for ( i = 0; i < tokc; i++) {
    int    tc;
    char** tv;

    Tcl_SplitList( itf, tokv[i], &tc, &tv);
    if ( tc > 1) {
      char*  string = Tcl_Merge( tc-1, tv+1);
      sprintf( name, "%s(%s)", dbaseP->name, tv[0]);
      Tcl_SetVar(itf, name, string, TCL_LEAVE_ERR_MSG);
      Tcl_Free((char*) string);
    }
    Tcl_Free((char*) tv);
  }
  Tcl_Free((char*) tokv);
  return TCL_OK;
}

/* ------------------------------------------------------------------------
    dbaseGet
   ------------------------------------------------------------------------ */

char* dbaseGet( DBase* dbaseP, char* key, int* valSize)
{
  DBaseIdx* dbxP    = &(dbaseP->dbaseIdx);

  if ( dbxP->idxFd < 0) return NULL;

  if (dbaseIdxFind( dbxP, key)) {
    datRead(dbaseP);
    if (valSize) *valSize = dbxP->datLen;
    return dbaseP->datBufP;
  }
  else return NULL;
}

static int dbaseGetItf (ClientData clientData, int argc, char *argv[]) 
{
  DBase*  dbaseP = (DBase*)clientData;
  char*   key    =  NULL;
  char*   value  =  NULL;
  int     ac     =  argc-1;

  if (itfParseArgv(argv[0], &ac, argv+1, 0,
     "<key>",ARGV_STRING,NULL,&key,NULL,"key", NULL) != TCL_OK)
    return TCL_ERROR;

  if ((value = dbaseGet( dbaseP, key, NULL))) {
    itfAppendResult( "%s", value); return TCL_OK;
  }
  else {
    SERROR("DBase '%s' key '%s' not found.\n", dbaseP->name, key);
    return TCL_ERROR;
  }
}

/* ------------------------------------------------------------------------
    dbaseFirst
   ------------------------------------------------------------------------ */

char* dbaseFirst( DBase* dbaseP)
{
  return dbaseIdxFirst( &(dbaseP->dbaseIdx));
}

static int dbaseFirstItf (ClientData clientData, int argc, char *argv[]) 
{
  DBase*  dbaseP = (DBase*)clientData;
  char*   key;

  argc--;
  if (itfParseArgv(argv[0], &argc, argv+1, 0,
        NULL) != TCL_OK)  return TCL_ERROR;

  if ((key = dbaseFirst(dbaseP))) itfAppendResult("%s", key); 
  else                            itfAppendResult("");
  return TCL_OK;
}

/* ------------------------------------------------------------------------
    dbaseNext
   ------------------------------------------------------------------------ */

char* dbaseNext( DBase* dbaseP)
{
  return dbaseIdxNext( &(dbaseP->dbaseIdx));
}

static int dbaseNextItf (ClientData clientData, int argc, char *argv[]) 
{
  DBase*  dbaseP = (DBase*)clientData;
  char*   key;

  argc--;
  if (itfParseArgv(argv[0], &argc, argv+1, 0,
        NULL) != TCL_OK)  return TCL_ERROR;

  if ((key = dbaseNext(dbaseP))) itfAppendResult( "%s", key);
  else                           itfAppendResult( "");
  return TCL_OK;
}

/* ------------------------------------------------------------------------
    dbaseList
   ------------------------------------------------------------------------ */

static int dbaseListItf (ClientData clientData, int argc, char *argv[]) 
{
  DBase*  dbaseP = (DBase*)clientData;
  char*   key;

  argc--;
  if (itfParseArgv(argv[0], &argc, argv+1, 0,
        NULL) != TCL_OK)  return TCL_ERROR;

  for ( key = dbaseFirst(dbaseP); key != NULL; 
        key = dbaseNext( dbaseP)) {
    itfAppendElement( "%s", key);
  }
  return TCL_OK;
}

/* ------------------------------------------------------------------------
    dbasePuts
   ------------------------------------------------------------------------ */

static int dbasePutsItf (ClientData clientData, int argc, char *argv[]) 
{
  argc--;
  if (itfParseArgv(argv[0], &argc, argv+1, 0,
        NULL) != TCL_OK)  return TCL_ERROR;

  return dbaseListItf( clientData, argc + 1, argv);
}

/* ------------------------------------------------------------------------
    dbaseRead
   ------------------------------------------------------------------------ */

static int dbaseReadItf ( ClientData clientData, int argc, char *argv[] )
{
  DBase* dbaseP  = (DBase*)clientData;
  char*  fname   =  NULL;
  int    ac      =  argc-1;
  int    n;

  if (itfParseArgv(argv[0], &ac, argv+1, 0,
		  "<filename>",ARGV_STRING,NULL,&fname,NULL,
                  "name of file to read", NULL) != TCL_OK)
    return TCL_ERROR;

  itfAppendResult( "%d", n = freadAdd( clientData, fname,
                                       dbaseP->commentChar, dbaseAddItf));
  return (n >= 0) ? TCL_OK : TCL_ERROR;
}

/* ------------------------------------------------------------------------
    dbaseWrite
   ------------------------------------------------------------------------ */

static int dbaseWrite (DBase *dbaseP, char *filename)
{
  FILE*   fp;
  char*   key;

  if ((fp=fileOpen(filename,"w"))==NULL) {
    ERROR( "Can't open database file '%s' for writing\n.",filename);
    return TCL_ERROR;
  }

  fprintf( fp, "%c -------------------------------------------------------\n",
                dbaseP->commentChar);
  fprintf( fp, "%c  Name            : %s\n",  dbaseP->commentChar, 
                dbaseP->name);
  fprintf( fp, "%c  Type            : DBase\n", dbaseP->commentChar);
  fprintf( fp, "%c  Date            : %s", dbaseP->commentChar,
                dateString());
  fprintf( fp, "%c -------------------------------------------------------\n",
                dbaseP->commentChar);

  for ( key = dbaseFirst(dbaseP); key != NULL; 
        key = dbaseNext( dbaseP)) {
    char* value = dbaseGet( dbaseP, key, NULL);
    fprintf( fp, "%s {%s}\n", key, value);
  }
  fclose(fp);
  return TCL_OK;
}

static int dbaseWriteItf (ClientData clientData, int argc, char *argv[] )
{
  char   *fname   =  NULL;
  int     ac      =  argc-1;

  if (itfParseArgv(argv[0], &ac, argv+1, 0,
		  "<filename>",ARGV_STRING,NULL,&fname,NULL,
                  "name of file to read", NULL) != TCL_OK)
    return TCL_ERROR;

  return dbaseWrite( (DBase*)clientData, fname);
}

/* ========================================================================
    Type Declaration
   ======================================================================== */

static Method dbaseIdxMethod[] = 
{ 
  { "puts",   dbaseIdxPutsItf,    NULL },
  { "open",   dbaseIdxOpenItf,   "open index file"},
  { "close",  dbaseIdxCloseItf,  "close index database"},
  { "add",    dbaseIdxAddItf,    "add record to index" },
  { "delete", dbaseIdxDeleteItf, "delete record from index" },
  { "get",    dbaseIdxGetItf,    "get record from index" },
  { "first",  dbaseIdxFirstItf,  "get first key in index file" },
  { "next",   dbaseIdxNextItf,   "get next key in index file" },
  { "list",   dbaseIdxListItf,   "list all keys in index file" },
  {  NULL,    NULL, NULL } 
};

TypeInfo dbaseIdxInfo = { "DBaseIdx", 0, 0, dbaseIdxMethod, 
                           dbaseIdxCreateItf, dbaseIdxFreeItf, 
                           dbaseIdxConfigureItf, NULL, NULL,
                          "DBase Index Object\n" };

static Method dbaseMethod[] = 
{ 
  { "puts",   dbasePutsItf,    NULL },
  { "open",   dbaseOpenItf,   "open database"},
  { "close",  dbaseCloseItf,  "close database"},
  { "add",    dbaseAddItf,    "add record to database" },
  { "delete", dbaseDeleteItf, "delete record from database" },
  { "read",   dbaseReadItf,   "add records from file to database" },
  { "write",  dbaseWriteItf,  "write records from database to file" },
  { "get",    dbaseGetItf,    "get record from database" },
  { "first",  dbaseFirstItf,  "get first key in database" },
  { "next",   dbaseNextItf,   "get next key in database" },
  { "list",   dbaseListItf,   "list all keys in database" },
  {  NULL,    NULL, NULL } 
};

TypeInfo dbaseInfo = { "DBase", 0, 0,      dbaseMethod, 
                        dbaseCreateItf,    dbaseFreeItf, 
                        dbaseConfigureItf, dbaseAccessItf, NULL,
                       "DBase\n" };

static int dbaseInitialized = 0;

int DBase_Init (void)
{
  if (! dbaseInitialized) {

    dbaseIdxDefault.hashSizeX = 2;

    itfNewType (&dbaseIdxInfo);
    itfNewType (&dbaseInfo);
    dbaseInitialized = 1;
  }
  return TCL_OK;
}
