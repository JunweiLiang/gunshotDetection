/* ========================================================================
    JANUS      Speech- to Speech Translation
               ------------------------------------------------------------
               Object: Database Access
               ------------------------------------------------------------

    Author  :  Michael Finke
    Module  :  dbase.h
    Date    :  $Id: dbase.h 512 2000-08-27 15:31:32Z jmcd $
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
    Revision 3.4  2000/08/27 15:31:11  jmcd
    Merging branch 'jtk-00-08-24-jmcd'.

    Revision 3.3.4.1  2000/08/25 22:19:43  jmcd
    Just checking.

    Revision 3.3  1997/07/25 12:12:46  tschaaf
    gcc / DFKI - clean

    Revision 3.2  1997/06/17 17:41:43  westphal
    *** empty log message ***

    Revision 1.4  95/08/17  17:07:56  rogina
    *** empty log message ***
    
    Revision 1.3  1995/08/17  13:34:14  finkem
    *** empty log message ***

    Revision 1.2  1995/08/16  12:56:31  finkem
    *** empty log message ***

    Revision 1.1  1995/08/11  16:34:04  finkem
    Initial revision

 
   ======================================================================== */

#ifdef __cplusplus
extern "C" {
#endif

#ifndef _dbase
#define _dbase

#include "common.h"

/* ========================================================================
    DBaseIdx
   ======================================================================== */

typedef long  DbHashKeyFn( char*);
typedef int   DbHashCmpFn( char*, char*);

typedef struct DBaseIdx_S
{
  char*        name;       /* name of this questions object */
  int          useN;

  int          oflag;
  int          mode;

  int          hashSizeX;  /* hash table */
  DbHashKeyFn* hashKey;
  DbHashCmpFn* hashCmp;
  off_t        hashOff;

  int          idxFd;
  int          idxLen;
  off_t        idxOff;
  int          datLen;
  off_t        datOff;

  char*        idxBufP;
  int          idxBufN;

  off_t        idxPtr;
  off_t        idxNext;

  char         commentChar;   /* a character to itentify comment lines */

} DBaseIdx;

extern int       DBase_Init (void);
extern DBaseIdx* dbaseIdxCreate( char* name);
extern int       dbaseIdxOpen(   DBaseIdx* dbxP, char* file, int oflag, int mode);
extern int       dbaseIdxClose(  DBaseIdx* dbxP);

extern off_t     dbaseIdxAdd(    DBaseIdx* dbxP, char* key, off_t valOff, 
                                                        int  valSize);
extern int       dbaseIdxDelete( DBaseIdx* dbxP, char* key);
extern off_t     dbaseIdxGet(    DBaseIdx* dbxP, char* key, int* valSize);

extern char*     dbaseIdxFirst(  DBaseIdx* dbxP);
extern char*     dbaseIdxNext(   DBaseIdx* dbxP);


/* ========================================================================
    DBase
   ======================================================================== */

typedef struct DBase_S
{
  char*        name;       /* name of this questions object */
  int          useN;

  DBaseIdx     dbaseIdx;

  int          oflag;
  int          mode;

  int          datFd;
  char*        datBufP;
  int          datBufN;

  char         commentChar;   /* a character to itentify comment lines */

} DBase;

/* ========================================================================
    Module Interface
   ======================================================================== */

extern int    dbaseInit(   DBase* dbaseP, ClientData CD);
extern DBase* dbaseCreate( char*  name);
extern int    dbaseDeinit( DBase* dbaseP);
extern int    dbaseFree(   DBase* dbaseP);

extern int    dbaseOpen(   DBase* dbaseP, char* file, char* idx, 
                                          int oflag, int mode);
extern int    dbaseClose(  DBase* dbaseP);
extern int    dbaseAdd(    DBase* dbaseP, char* key, 
                                          char* val, int valSize);
extern char*  dbaseGet(    DBase* dbaseP, char* key, int*);
extern int    dbaseDelete( DBase* dbaseP, char* key);

extern char*  dbaseFirst(  DBase* dbaseP);
extern char*  dbaseNext(   DBase* dbaseP);

extern int    dbaseSetArray( DBase* dbaseP, char *value);

#endif


#ifdef __cplusplus
}
#endif
