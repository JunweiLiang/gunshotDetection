/* ========================================================================
    JANUS      Speech- to Speech Translation
               ------------------------------------------------------------
               Object: Hashtable  (barebone of List-Module list)
               ------------------------------------------------------------

    Author  :  Klaus Ries
    Module  :  hash.h
    Date    :  $Id: hash.h 512 2000-08-27 15:31:32Z jmcd $
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
    Revision 3.5  2000/08/27 15:31:16  jmcd
    Merging branch 'jtk-00-08-24-jmcd'.

    Revision 3.4.4.1  2000/08/25 22:19:43  jmcd
    Just checking.

    Revision 3.4  1998/06/29 23:49:02  kries
    *** empty log message ***

 * Revision 3.3  1998/05/26  18:27:23  kries
 * Complete substitution !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 * full fledged hash implementation
 *
 
   ======================================================================== */

#ifdef __cplusplus
extern "C" {
#endif

#ifndef _hash
#define _hash

#include "common.h"
#include "itf.h"

/* ========================================================================
    Hash Access to List Items
   ======================================================================== */

typedef struct HashR_S
{
  long  key;    /* hash key           */
  ClientData CD;/* Data               */
  int   next;   /* next item in chain */
} HashR;

/* ========================================================================
    Hash
   ======================================================================== */


typedef long  HashKeyFn2( ClientData, ClientData);
typedef int   HashCmpFn2( ClientData, ClientData, ClientData);
typedef int   HashConflictFn2( ClientData, ClientData, ClientData);

typedef int*  HashHandle;

typedef struct Hash_S {
  int		  itemN;      /* Elements in Hash */

  int*            hashTable;
  int             hashSizeX;  /* size of hashXA == hashPrimes[(L)->hashSizeX] */

#ifdef PROFILEHASH
  int             profileSearches;
  int             profileFailedSearches;
  int             profileComparisions;
  int             profileFullComparisions;
#endif

  HashR*          hashXA;
  int             allocN;     /* number of allocated records in hash */
  int             hashXAfree;/* free-list of elements in hashXA      */
  int             blkSize;    /* block size for allocation (default 1024)*/

  HashKeyFn2*     hashKey;
  HashCmpFn2*     hashCmp;
  HashConflictFn2* hashConflict;

  void (*data_deinit)(ClientData, ClientData);          /* deinit for Data    */
  ClientData DataHolder;
  
} Hash;


typedef struct HashIter_S {
  Hash* H;
  ClientData * Current;
  int hashTableIdx;
  int idx;
} HashIter;

/* ========================================================================
    Module Interface
   ======================================================================== */

extern int hashInit(    Hash* H);
extern int hashClear(  Hash* H);

extern HashHandle  hashItem(   Hash* H,  ClientData CD, int create);
extern ClientData  hashDelete(   Hash* H,  HashHandle h, int no_deinit);
extern ClientData* hashRecall(Hash* H, HashHandle);
extern ClientData  hashEnter(Hash* H, ClientData CD);

/* ========================================================================
   HashIter 

   HashIter i;
   for(hashIterInit(H,&i);i->Current;hashIterNext(&i)) {
       ClientData * DataP = i->Current;
   }
   
   ======================================================================== */

extern void hashIterInit(Hash* H,HashIter *i);
extern void hashIterNext(HashIter *i);

#endif


#ifdef __cplusplus
}
#endif
