/* ========================================================================
    JANUS      Speech- to Speech Translation
               ------------------------------------------------------------
               Object: Hashtable  (barebone of List-Module list)
               ------------------------------------------------------------

    Author  :  Klaus Ries
    Module  :  hash.c
    Date    :  $Id: hash.c 2390 2003-08-14 11:20:32Z fuegen $
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
    Revision 3.5  2003/08/14 11:20:21  fuegen
    Merged changes on branch jtk-01-01-15-fms (jaguar -> ibis-013)

    Revision 3.4.20.1  2002/06/26 11:57:57  fuegen
    changes for new directory structure support and code cleaning

    Revision 3.4  2000/11/14 12:29:33  janus
    Merging branch jtk-00-10-27-jmcd.

    Revision 3.3.34.1  2000/11/06 10:50:29  janus
    Made changes to enable compilation under -Wall.

    Revision 3.3  1998/05/26 18:27:23  kries
    Complete substitution !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    full fledged hash implementation

 
   ======================================================================== */

#include "common.h"
#include "itf.h"
#include "hash.h"



char hashRcsVersion[]= 
           "@(#)1$Id: hash.c 2390 2003-08-14 11:20:32Z fuegen $";

/* ------------------------------------------------------------------------
    Forward Declarations
   ------------------------------------------------------------------------ */

static long HashPrimes[] =
{
  1,  2,  3,  7,  13,  23,  59,  113,  241,  503,  1019,  2039,  4091,
  8179,  16369,  32749,  65521,  131063,  262139,  524269,  1048571,
  2097143,  4194287,  8388593,  16777199,  33554393,  67108859,
  134217689,  268435399,  536870879,  1073741789,  2147483629
};

#define HASHS(H) (HashPrimes[(H)->hashSizeX])

/* ------------------------------------------------------------------------
    hashStringKey & hashStringCmp
   ------------------------------------------------------------------------ */

static long hashStringKey(char *s)
{
  char*         s1   = (char*)s;
  unsigned long hash =  0;

  while (*s1)
    {
      hash=((hash << 5) | (hash >> (8*sizeof(long)-5))) ^ *s1;
      ++s1;
    }
  return (hash);
}

static int hashStringCmp(char *s1, char **s2)
{ 
  char* name = *s2;
  for(; *s1 == *name; s1++, name++) if (*s1 == '\0') return 1;
  return 0;
}

/* ========================================================================
    Hashes
   ======================================================================== */

/* ------------------------------------------------------------------------
    hashInit
   ------------------------------------------------------------------------ */

int hashInit(    Hash* H)
{
  assert( H);

  H->itemN	=  0;

  /* Initalize hashtable */ 

  H->hashSizeX  =  0;
  H->hashTable  = NULL;
  
  /* Initalize hash-entries */

  H->hashXA     = NULL;
  H->hashXAfree = -1;
  H->blkSize    =  1024;
  H->allocN     =  0;

  /* defaults to strings */

  H->hashKey    = (HashKeyFn2*)hashStringKey;
  H->hashCmp    = (HashCmpFn2*)hashStringCmp;

  /* defaults to no special handling */

  H->hashConflict=0;
  H->data_deinit=0;
  H->DataHolder =0;

#if defined(PROFILEHASH)
  /* Profilining */
  H->profileSearches=0;
  H->profileComparisions=0;
  H->profileFullComparisions=0;
  H->profileFailedSearches=0;
#endif

  return TCL_OK;
}

/* ------------------------------------------------------------------------
    hashClear
   ------------------------------------------------------------------------ */

int hashClear(  Hash* H) {
  int i,idx;

#if defined(PROFILEHASH)
  /* Profilining */

  INFO("Profiling hash -- Searches %ld Comparisions %ld FullComparisions %ld Failed Searches %ld\n",
       H->profileSearches, H->profileComparisions, H->profileFullComparisions,  H->profileFailedSearches);
  INFO("Profiling hash -- Elements in hash %ld  hashtablesize %ld\n",H->itemN,HASHS(H));

  H->profileSearches=0;
  H->profileComparisions=0;
  H->profileFullComparisions=0;
  H->profileFailedSearches=0;
#endif

  if(H->data_deinit && H->hashTable && H->hashXA) {
    /* Deinitialize entries */

    for(i=0;i<HASHS(H);i++)
      for(idx=H->hashTable[i];idx>=0;idx = H->hashXA[idx].next)
	H->data_deinit(H->DataHolder,H->hashXA[idx].CD);
  }

  if ( H->hashTable) { free(H->hashTable); H->hashTable = NULL; }
  if ( H->hashXA)    { free(H->hashXA);    H->hashXA    = NULL; }
  H->allocN     =  0;
  H->hashXAfree = -1;
  H->itemN	=  0;
  H->hashSizeX  =  0;
  
  return TCL_OK;
}

/* ------------------------------------------------------------------------
    listIndex   Lookup the given name in the given array using bsearch().
                For this goal we insert the key <name> at the end of the 
                list and let sortXA point to it, too. Because of this hack, 
                we expand the allocated blocks if there are less than two 
                free slots left (see below). 
   ------------------------------------------------------------------------ */

HashHandle  hashItem(   Hash* H,  ClientData CD, int create)
{
  long key;
  long hX;
  long hX2;
  HashHandle iP;
  int  i;
  int idx;
  int * oldtable, oldtablesize;

  assert( H);

  if (CD==NULL) return NULL;

  key = H->hashKey(H->DataHolder,CD);

  for(hX  = key % HASHS(H); hX < 0; hX += HASHS(H));


  if(H->itemN) {
#if defined(PROFILEHASH)
    /* Profilining */
    H->profileSearches++;
#endif
    for ( iP = &(H->hashTable[hX]); *iP >= 0; iP = &(H->hashXA[*iP].next)) {
#if defined(PROFILEHASH)
      H->profileComparisions++; if(H->hashXA[*iP].key == key) H->profileFullComparisions++;
#endif
      if ( H->hashXA[*iP].key == key && 
	   H->hashCmp( H->DataHolder, CD, H->hashXA[*iP].CD)) {

	int i;

	if(create) {
	  /* The item was found, now we have a conflict:
	     which ClientData should be entered ?
	     if hashConflict is undefined, it is the older one
	     otherwise it is the one which is smaller according
	     to hashConflict, if they are equal the current one wins
	     hashConflict is like an "is_less" predicate
	  */

	  if(H->hashConflict && (H->hashConflict( H->DataHolder, H->hashXA[*iP].CD,CD)==1)) {
	    if(H->data_deinit) 	/* Handle the ownership of the items */
	      H->data_deinit(H->DataHolder, H->hashXA[*iP].CD);
	    H->hashXA[*iP].CD=CD;
	  }
	  else if(H->data_deinit) /* Handle the ownership of the items */
	    H->data_deinit(H->DataHolder, CD);
	}
	/* printf("key %ld hX %ld element %ld\n",key,hX,*iP); */
#define RECHAIN
#if defined (RECHAIN)
	/* Change the chain such that the item just looked up is becoming the first in the queue */
	
	i = *iP;
	if(i!=H->hashTable[hX]) {
	  *iP = H->hashXA[i].next; /* Unchain *iP == i */
	  H->hashXA[i].next=H->hashTable[hX]; /* Rechain first element in list */
	  H->hashTable[hX]=i; /* Rechain i */
	}
	return &(H->hashTable[hX]);
#else
	return iP;
#endif
      }
    }
  }
#if defined(PROFILEHASH)
  H->profileFailedSearches++;
#endif

  /*
  if (create)
    printf("key %ld hX %ld -- create new\n",key,hX);
  else
    printf("key %ld hX %ld -- just checking ...\n",key,hX);
  */
  
  if (! create) return NULL;

  /* Create a new hash-item */

  /* Make sure the hashXA array is big enough */

  if(H->hashXAfree<0) {    /* Make hashXA bigger */
    HashR* old = H->hashXA;

    if(H->itemN!=H->allocN) 
      ERROR("Hashtable invariant: hashXA full -->  H->itemN==H->allocN violated");    

    H->hashXA = (HashR*)malloc( (H->allocN+H->blkSize) * sizeof(HashR));
    if(H->hashXA==NULL) {
      ERROR("Cannot allocate hashXA for size %ld",H->allocN);
      free(old);
      return NULL;
    }

    if(old)
      memcpy( H->hashXA, old, H->allocN * sizeof(HashR));
    free(old);

    /* Append to free-list */
    for(H->hashXAfree = i = H->allocN ;
	i<H->allocN+H->blkSize-1;
	i++)
      H->hashXA[i].next=i+1;
    H->hashXA[i].next=-1;

    H->allocN += H->blkSize;
  }

  /* Make sure the hashTable is big enough */

  if(H->hashTable == NULL || 4*H->itemN+1 > HASHS(H)) {
    oldtable = H->hashTable;
    oldtablesize = oldtable ? HASHS(H) : 0;

    H->hashSizeX++;


    H->hashTable = (int*)malloc(HASHS(H) * sizeof(int));
    if(H->hashTable==NULL) {
      free(oldtable);
      ERROR("Cannot allocate hashTable for size %ld",HASHS(H));
      return NULL;
    }

    /* Rehash everything ... */
    for(hX  = key % HASHS(H); hX < 0; hX += HASHS(H));      
    for(i=0;i<HASHS(H);i++)
      H->hashTable[i]=-1;
    for(i=0;i<oldtablesize;i++) {
      for(idx=oldtable[i];idx>=0;) {
	int nidx = H->hashXA[idx].next;

	for(hX2  = H->hashXA[idx].key % HASHS(H); hX2 < 0; hX2 += HASHS(H));      
	H->hashXA[idx].next = H->hashTable[hX2];
	H->hashTable[hX2] = idx;
	idx = nidx;
      }
    }
    free(oldtable);
  }
  
  idx=H->hashXAfree;
  if(idx<0) {
    FATAL("No free element hashXA");
    return NULL;
  }
  H->hashXAfree=H->hashXA[idx].next;
  

  H->hashXA[idx].key  = key;
  H->hashXA[idx].CD   = CD;
  H->hashXA[idx].next = H->hashTable[hX];
  H->hashTable[hX]    = idx;
  
  H->itemN++;

  return &(H->hashTable[hX]);
}


/* ------------------------------------------------------------------------
    hashDelete   remove an item from the hash
   ------------------------------------------------------------------------ */

ClientData hashDelete(   Hash* H,  HashHandle h, int no_deinit) {

  int idx;  

  /* Check if hashhandle is valid */
  if(h == NULL) {
    ERROR("hashDelete can't delete empty handle\n");
    return NULL;
  }
  if(*h<0) {
    ERROR("hashDelete handle contains negativ element\n");
    return NULL;
  }
  
  /* Deqeue the hashhandle */
  idx = *h;
  *h = H->hashXA[idx].next;

  /* Mark idx as part of the free-list */
  H->hashXA[idx].next = H->hashXAfree;
  H->hashXAfree = idx;

  /* Update itemN */
  H->itemN--;

  /* deinit if necessary */
  if(H->data_deinit && !no_deinit)
    H->data_deinit(H->DataHolder,H->hashXA[idx].CD);

  return H->hashXA[idx].CD;
}


ClientData* hashRecall(Hash* H, HashHandle h) {
  return &(H->hashXA[*h].CD);
}

ClientData hashEnter(Hash* H, ClientData CD) {
  return * hashRecall(H,hashItem(H, CD, 1));
}


/* ========================================================================
   HashIter 

   HashIter i;
   for(hashIterInit(H,&i);i->Current;hashIterNext(&i)) {
       ClientData * DataP = i->Current;
   }
   
   ======================================================================== */

void hashIterInit(Hash* H,HashIter *i) {
  assert(H);
  if(H->itemN>0) {
    i->H=H;
    i->hashTableIdx=0;
    i->idx=-1;
    hashIterNext(i);
  }
  else {
    i->H=NULL;
    i->Current=NULL;
  }
}
    
void hashIterNext(HashIter *i) {
  if(i->H==NULL) 
    return;

  i->idx = i->idx>=0 ?
           i->H->hashXA[i->idx].next
         : i->H->hashTable[i->hashTableIdx++];

  for(;i->idx<0 && i->hashTableIdx<HASHS(i->H);i->hashTableIdx++)
    i->idx = i->H->hashTable[i->hashTableIdx];
    
  if(i->idx<0) {
    i->H=NULL;
    i->Current=NULL;
  }
  else
    i->Current = &(i->H->hashXA[i->idx].CD);
}




