/* ========================================================================
    JANUS      Speech- to Speech Translation
               ------------------------------------------------------------
               Object: Indexed Lists of Generic Named Objects
               ------------------------------------------------------------

    Author  :  Michael Finke
    Module  :  list.h
    Date    :  $Id: list.h 3372 2011-02-28 00:26:20Z metze $
    Remarks :

   ========================================================================

    This software was developed by the Interactive Systems Laboratories at

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
    Revision 3.6  2003/08/14 11:19:51  fuegen
    Merged changes on branch jtk-01-01-15-fms (jaguar -> ibis-013)

    Revision 3.5.20.2  2002/07/18 12:18:28  soltau
    Introduced ringbuffer via itemMax

    Revision 3.5.20.1  2001/11/30 14:49:54  soltau
    Made real hashsize available

    Revision 3.3  1998/05/26 12:55:24  kries
    added listClear and DataHolder

 * Revision 3.2  1997/06/17  17:41:43  westphal
 * *** empty log message ***
 *
    Revision 1.5  95/10/16  15:53:37  finkem
    listFirstItem changed parameters
    
    Revision 1.4  1995/10/04  22:28:20  finkem
    added list index to name and name to index function

    Revision 1.3  1995/08/29  02:48:09  finkem
    *** empty log message ***

    Revision 1.2  1995/08/18  08:22:58  finkem
    *** empty log message ***

    Revision 1.1  1995/07/25  13:49:57  finkem
    Initial revision

 
   ======================================================================== */

#ifdef __cplusplus
extern "C" {
#endif

#ifndef _list
#define _list

#include "common.h"
#include "itf.h"

/* ========================================================================
    Hash Access to List Items
   ======================================================================== */

typedef struct HashRec_S
{
  long  key;    /* hash key           */
  int   idx;    /* list index         */
  int   next;   /* next item in chain */

} HashRec;

/* ========================================================================
    Indexed Lists
   ======================================================================== */

typedef int   ListItemInit(   char**, ClientData);
typedef int   ListItemDeinit( char**);
typedef int   ListItemLinkN(  ClientData);

typedef long  HashKeyFn( ClientData);
typedef int   HashCmpFn( ClientData, void*);

typedef int   ListCmpFn( const void*, const void*);

#define LIST(T) \
{ \
 \
  T*              itemA;      /* array of items                      */ \
  int             itemN;      /* number of items in the list         */ \
  int             itemMax;    /* max. number of items in the list    */ \
  int             itemSize;   /* size of the indivual items          */ \
  int             itemSizeCP; \
  int             allocN;     /* number of allocated records in dsA  */ \
  int             blkSize;    /* block size for allocation           */ \
  int             compress;   /* compress list when deleting items   */ \
 \
  TypeInfo*       typeInfo;   /* pointer to item type information    */ \
 \
  int             sorted;     /* is array sorted                     */ \
  ListCmpFn*      sortCmp;    /* compare two items in the list       */ \
  int*            sortXA;     /* sorted index array                  */ \
 \
  int*            hashTable; \
  HashRec*        hashXA; \
  int             hashSizeX;  /* hash size as index of hashPrimes    */ \
  int             hashSizeY;  /* hash size = hashPrimes(hashSIzeX)   */ \
  HashKeyFn*      hashKey; \
  HashCmpFn*      hashCmp; \
  ClientData      DataHolder;\
 \
  ListItemInit*   init;       /* item initialization function        */ \
  ListItemDeinit* deinit; \
  ListItemLinkN*  links; \
 \
}

typedef struct LIST(char*) List;

/* ========================================================================
    Module Interface
   ======================================================================== */

extern int listInit(    List* L, TypeInfo* TI, int itemSize, int blkSize);
extern int listDeinit(  List* L);
extern int listDelink(  List* L);
extern int listLinkN(   List* L);
extern int listIndex(   List* L, void* key, int create);
extern int listNewItem( List* L, void* key);
extern int listDelete(  List* L, void* key);
extern int listClear(  List* L);

extern int listFirstItem( List* L, int  idx, int* i);
extern int listNextItem(  List* L, int* i);

extern ClientData listAccessItf(    ClientData cd, char *name, TypeInfo **ti);
extern int        listConfigureItf( ClientData cd, char *var, char *val);
extern int        listPutsItf(      ClientData cd, int argc, char *argv[]);
extern int        listDeleteItf(    ClientData cd, int argc, char *argv[]);
/* extern int        listFastIndexItf( ClientData cd, int  argc, char* argv[]); */

extern int listIndexItf( ClientData CD,     char* key, 
                         ClientData retPtr, int*  argcPtr, char* argv[]);

extern int   listName2Index(    List* list, char* name);
extern int   listName2IndexItf( ClientData clientData, int argc, char *argv[]);
extern char* listIndex2Name(    List* list, int idx);
extern int   listIndex2NameItf( ClientData clientData, int argc, char *argv[]);

extern void  listSort( List* L);

extern int List_Init ();

/* how ugly (Florian) */
#include "../ibis/ngramLM.h"
extern int ngspCLNewItem( /*NGramLMNGSPCache*/List* L, void* CD );
extern int lctapCLNewItem(List*, void*);

#endif

#ifdef __cplusplus
}
#endif
