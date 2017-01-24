/* ========================================================================
    JANUS      Speech- to Speech Translation
               ------------------------------------------------------------
               Object: Indexed Lists of Generic Named Objects
               ------------------------------------------------------------

    Author  :  Michael Finke
    Module  :  list.c
    Date    :  $Id: list.c 3372 2011-02-28 00:26:20Z metze $
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
    Revision 3.10  2003/08/14 11:19:51  fuegen
    Merged changes on branch jtk-01-01-15-fms (jaguar -> ibis-013)

    Revision 3.9.20.3  2002/07/18 12:18:38  soltau
    Introduced ringbuffer via itemMax

    Revision 3.9.20.2  2002/06/26 11:57:53  fuegen
    changes for new directory structure support and code cleaning

    Revision 3.9.20.1  2001/11/30 14:50:12  soltau
    Made real hashsize available

    Revision 3.8  2000/08/16 09:32:21  soltau
    free -> Tcl_Free

    Revision 3.7  1998/08/02 11:17:28  kries
    faster stringEq fct for hashing

 * Revision 3.6  1998/06/11  10:06:11  kries
 * *** empty log message ***
 *
 * Revision 3.5  1998/06/10  09:56:41  kries
 * *** empty log message ***
 *
 * Revision 3.4  1998/05/26  16:35:59  kries
 * enable float-heaps (ignore typeInfo in certain cases)
 *
 * Revision 3.3  1998/05/26  12:55:24  kries
 * added listClear and DataHolder
 *
 * Revision 3.2  1997/06/17  17:41:43  westphal
 * *** empty log message ***
 *
    Revision 1.18  96/07/30  12:52:46  rogina
    listIndex of NULL-pointer will now return the list's itemN
    
    Revision 1.17  1996/03/25  19:13:23  finkem
    list deallocation now resets all list pointers correctly

    Revision 1.16  1996/03/04  17:43:02  rogina
    made all -Wall warnings go away

    Revision 1.15  1996/02/08  16:21:56  rogina
    removed bug in seemsToBeString()

    Revision 1.14  1996/02/08  15:57:52  rogina
    made "can't find item" message more verbose

    Revision 1.13  1996/01/09  13:57:08  finkem
    added name and index function to list type

    Revision 1.12  1995/10/31  01:52:50  finkem
    modified hashStringKey due to problems on HPs

    Revision 1.11  1995/10/16  15:53:37  finkem
    listFirstItem changed parameters

    Revision 1.10  1995/10/04  22:28:20  finkem
    added list index to name and name to index function

    Revision 1.9  1995/09/06  07:33:10  kemp
    *** empty log message ***

    Revision 1.8  1995/09/04  16:42:26  kemp
    Changed in listInit the 'while (blkSize < 4*HASHSIZE...)' to 'while (blkSize > 4*HASHSIZE...)'

    Revision 1.7  1995/09/01  14:59:05  kemp
    Changed listInit

    Revision 1.6  1995/08/29  02:47:29  finkem
    *** empty log message ***

    Revision 1.5  1995/08/27  22:35:23  finkem
    *** empty log message ***

    Revision 1.4  1995/08/18  08:22:56  finkem
    *** empty log message ***

    Revision 1.3  1995/08/17  17:05:28  rogina
    *** empty log message ***

    Revision 1.2  1995/08/10  13:16:43  finkem
    *** empty log message ***

    Revision 1.1  1995/07/25  13:49:57  finkem
    Initial revision

 
   ======================================================================== */

#include <limits.h>
#include "common.h"
#include "itf.h"
#include "list.h"

#define  DENSITY_FACTOR 4

char listRcsVersion[]= 
           "@(#)1$Id: list.c 3372 2011-02-28 00:26:20Z metze $";

/* ------------------------------------------------------------------------
    Forward Declarations
   ------------------------------------------------------------------------ */

static long hashPrimes[] =
{
  1,  2,  3,  7,  13,  23,  59,  113,  241,  503,  1019,  2039,  4091,
  8179,  16369,  32749,  65521,  131063,  262139,  524269,  1048571,
  2097143,  4194287,  8388593,  16777199,  33554393,  67108859,
  134217689,  268435399,  536870879,  1073741789,  2147483629
};

#define HASHSIZE(L) (hashPrimes[(L)->hashSizeX])

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
  return (0==strcmp (s1, *s2)); /* this seems to be faster than:
  char* name = *s2;
  
  while(*s1!='\0')
    if(*(s1++) != *(name++)) return 0;
  return *name=='\0'; */
}

static List* listP = NULL;

static int listStringCmp(const void *X, const void *Y)
{ 
  int x = listP->itemSizeCP * (*((int*)X));
  int y = listP->itemSizeCP * (*((int*)Y));

  return strcmp( listP->itemA[x], listP->itemA[y]);
}

/* ========================================================================
    Indexed Lists
   ======================================================================== */

static void listRehash( List* L);

/* ------------------------------------------------------------------------
    listInit
   ------------------------------------------------------------------------ */

int listInit( List* L, TypeInfo* TI, int itemSize, int blkSize)
{
  int i;

  L->itemA      =  NULL;
  L->itemN      =  0;
  L->itemMax    =  INT_MAX;
  L->itemSize   =  itemSize;
  L->itemSizeCP =  itemSize / sizeof(char*);
  if (itemSize % sizeof(char *) != 0){
    SERROR("listInit failed: itemSize (%ld) not divideable by sizeof(char *)=%ld.\n",itemSize,sizeof(char *));
    return TCL_ERROR;
  }

  L->allocN     =  0;
  L->blkSize    =  blkSize;
  L->compress   =  1;

  L->typeInfo   =  TI;

  L->sorted     =  0;
  L->sortCmp    = (ListCmpFn*)listStringCmp;
  L->sortXA     =  NULL;

  L->hashSizeX  =  0;
  while ( blkSize > DENSITY_FACTOR * HASHSIZE(L)) L->hashSizeX++;
  L->hashSizeY  =  HASHSIZE(L);

  L->hashTable  = (int*)malloc( HASHSIZE(L) * sizeof(int));
  L->hashXA     =  NULL;
  L->hashKey    = (HashKeyFn*)hashStringKey;
  L->hashCmp    = (HashCmpFn*)hashStringCmp;
  L->DataHolder = (ClientData)L;

  L->init       =  NULL;
  L->deinit     =  NULL;
  L->links      =  NULL;

  for ( i = HASHSIZE(L); i > 0; ) L->hashTable[--i] = -1; 
  return TCL_OK;
}

/* ------------------------------------------------------------------------
    listDeinit
   ------------------------------------------------------------------------ */

int listDeinit( List* L)
{
  int retCode = TCL_OK;

  if ( L->typeInfo && listLinkN(L) > 1) {
    SERROR("Deinit failed: some list items in '%s'-list are linked.\n", L->typeInfo->name);
    return TCL_ERROR;
  }
  if ( L->deinit) {
    int   i;
    for ( i = 0; i < L->itemN; i++) {
      if(L->typeInfo)
	itfTypeCntl((ClientData)&(L->itemA[L->itemSizeCP * i]),
		    L->typeInfo,T_DELINK);
      if ( L->deinit( &(L->itemA[L->itemSizeCP * i])) != TCL_OK) {
        ERROR("Deinit of list item %d failed - list is corrupt!.\n", i);
        retCode = TCL_ERROR;
      }
    }
  }
  if ( L->sortXA)    { free(L->sortXA);    L->sortXA    = NULL; }
  if ( L->hashTable) { free(L->hashTable); L->hashTable = NULL; }
  if ( L->hashXA)    { free(L->hashXA);    L->hashXA    = NULL; }
  if ( L->itemA)     { free(L->itemA);     L->itemA     = NULL; }

  L->allocN     =  0;
  L->itemN      =  0;
  
  return retCode;
}

/* ------------------------------------------------------------------------
    listClear
   ------------------------------------------------------------------------ */

int listClear( List* L)
{
  int i;

  if ( L->deinit) {
    for ( i = 0; i < L->itemN; i++) {
      if ( L->typeInfo)
        itfTypeCntl((ClientData)&(L->itemA[L->itemSizeCP * i]),
                                  L->typeInfo,T_DELINK);
      if ( L->deinit( &(L->itemA[L->itemSizeCP * i])) != TCL_OK) {
        ERROR("Deinit of list item %d failed - list is corrupt!.\n", i);
        return TCL_ERROR;
      }
    }
  }
  for(i=0;i<HASHSIZE(L);i++)
    L->hashTable[i]=-1;

  L->itemN      =  0;
  L->sorted     =  0;
  
  return TCL_OK;
}

/* ------------------------------------------------------------------------
    listLinkN
   ------------------------------------------------------------------------ */

int listLinkN( List* L)
{
  int   useN = 0;
  int   i;

  for ( i = 0; i < L->itemN; i++) {
    int linkN;

    if ( L->links)
         linkN = L->links(   (ClientData)&(L->itemA[L->itemSizeCP * i]));
    else linkN = itfTypeCntl((ClientData)&(L->itemA[L->itemSizeCP * i]),
                                           L->typeInfo,T_LINKN);
    if ( linkN > useN) useN = linkN;
  }
  return useN;  
}

/* ------------------------------------------------------------------------
    listDelink
   ------------------------------------------------------------------------ */

int listDelink( List* L)
{
  int   useN = 0;
  int   i;

  for ( i = 0; i < L->itemN; i++) {
    int linkN;

    if ( L->links)
          linkN = L->links(   (ClientData)&(L->itemA[L->itemSizeCP * i]));
    else linkN = itfTypeCntl((ClientData)&(L->itemA[L->itemSizeCP * i]),
                                           L->typeInfo,T_DELINK);
    if ( linkN > useN) useN = linkN;
  }
  return useN;  
}

/* ------------------------------------------------------------------------
    listSort         sort the list by name
   ------------------------------------------------------------------------ */

void listSort( List* L)
{
  if (L->sortCmp && ! L->sorted) {
    listP     = L;
    L->sorted = 1;
    qsort((void*)L->sortXA, (size_t)L->itemN, (size_t)sizeof(int), 
                 L->sortCmp);
  }
}

/* ------------------------------------------------------------------------
    listIsMoveable
   ------------------------------------------------------------------------ */

static int listIsMoveable( List* L, int from, int to)
{
  int   i;

  if( !L->typeInfo) return 1;
  if ( to > L->itemN) to = L->itemN; 

  for ( i = from; i < to; i++) {
    int linkN;

    if ( L->links)
         linkN = L->links(   (ClientData)&(L->itemA[L->itemSizeCP * i]));
    else linkN = itfTypeCntl((ClientData)&(L->itemA[L->itemSizeCP * i]),
                                           L->typeInfo,T_LINKN);
    if ( linkN > 1) return 0;
  }
  return 1;
}

/* ------------------------------------------------------------------------
    listIndex   Lookup the given name in the given array using bsearch().
                For this goal we insert the key <name> at the end of the 
                list and let sortXA point to it, too. Because of this hack, 
                we expand the allocated blocks if there are less than two 
                free slots left (see below). 
   ------------------------------------------------------------------------ */

int listIndex( List* L, void* CD, int create)
{
  assert( L);

  if ( L && L->itemA) {
    long key = CD ? L->hashKey(CD) : 0;
    long hX  = key % HASHSIZE(L);
    int  i;

    if (CD==NULL) return L->itemN;

    if ( hX < 0) hX = -hX;    

    for ( i = L->hashTable[hX]; i >= 0; i = L->hashXA[i].next) {
      int idx = L->hashXA[i].idx % L->itemMax;
      if ( L->hashXA[i].key == key && 
           L->hashCmp( CD, L->itemA + (L->itemSizeCP * idx))) {
	return L->hashXA[i].idx;
      }
    }
  }
  if (! create) return -1;
  return listNewItem( L, CD);  
}

/* ------------------------------------------------------------------------
    listInitItem
   ------------------------------------------------------------------------ */

int listInitItem( List* L, int idx, ClientData CD)
{
  int  n   = L->itemN++;
  long key = L->hashKey(CD);
  long hX  = key % HASHSIZE(L);

  if ( hX < 0) hX = -hX;

  if ( L->init) 
       L->init( L->itemA + (L->itemSizeCP * idx), CD);

  if(L->typeInfo)
    itfTypeCntl((ClientData)&(L->itemA[L->itemSizeCP * idx]),
		L->typeInfo,T_LINK);

  L->sortXA[n]      = idx;
  L->sorted         = 0;

  L->hashXA[n].key  = key;
  L->hashXA[n].idx  = idx;
  L->hashXA[n].next = L->hashTable[hX];
  L->hashTable[hX]  = n;

  if ( L->itemN > DENSITY_FACTOR * HASHSIZE(L)) listRehash(L);
  
  return idx;
}

/* ------------------------------------------------------------------------
    listNewItem   insert a new item in the list (actually appending it to 
                  the end of the list). If there is no space left in the 
                  list, it is automatically enlarged by <blkSize> items. 
                  The new list is no longer sorted.
   ------------------------------------------------------------------------ */

int listNewItem( List* L, void* CD)
{
  int idx = L->itemN;

  /* hash list as ring buffer */
  if (L->itemN >= L->itemMax) {
    long oldkey = 0;
    long oldhX  = 0;
    long newkey = L->hashKey(CD);
    long newhX  = newkey % HASHSIZE(L);

    /* select item to overwrite */
    idx = L->itemN % L->itemMax;

    oldkey = L->hashXA[idx].key;
    oldhX  = oldkey % HASHSIZE(L);

    if ( newhX < 0) newhX = -newhX;
    if ( oldhX < 0) oldhX = -oldhX;
    
    if (oldkey != newkey) {
      int i = L->hashTable[oldhX];
      /* reset old hashkey */
      if (i == idx) 
	L->hashTable[oldhX] = L->hashXA[idx].next;
      else {
	while (i >= 0 && idx !=  L->hashXA[i].next) i = L->hashXA[i].next;
	assert(idx == L->hashXA[i].next);
	L->hashXA[i].next = L->hashXA[idx].next;
      }

      /* setup new hashkey */     
      L->hashXA[idx].key  = newkey;
      L->hashXA[idx].next = L->hashTable[newhX];
      L->hashTable[newhX] = idx;
    }

    /* init idx entry with new parameters from CD */
    if ( L->init) 
      L->init( L->itemA + (L->itemSizeCP * idx), CD);

    L->hashXA[idx].idx  = L->itemN;

    /* */
    L->itemN++;

    return L->hashXA[idx].idx;
  }

  if ( idx < L->allocN) return listInitItem( L, idx, CD);

  if (! L->itemA) {
    L->allocN = L->blkSize;
    if (!(L->itemA  = (char**  )malloc( L->allocN * L->itemSize))     ||
        !(L->hashXA = (HashRec*)malloc( L->allocN * sizeof(HashRec))) ||
        !(L->sortXA = (int*    )malloc( L->allocN * sizeof(int)))) {

      ERROR( "Can't allocate indexed list.\n");
      return -1;
    }
  }
  else {
    if (! listIsMoveable( L, 0, L->itemN)) {
      ERROR( "Can't reallocate list.\n");
      return -1;
    }
    L->allocN += L->blkSize;
    if (!(L->itemA  = (char**  )realloc(L->itemA, L->allocN * L->itemSize))     ||
        !(L->hashXA = (HashRec*)realloc(L->hashXA,L->allocN * sizeof(HashRec))) ||
        !(L->sortXA = (int*    )realloc(L->sortXA,L->allocN * sizeof(int)))) {

      ERROR( "Can't reallocate index list.\n");
      return -1;
    }
  }
  return listInitItem( L, idx, CD);
}

/* ------------------------------------------------------------------------
    listIndexItf   get item in the list by name (ParseArgv compatible).
   ------------------------------------------------------------------------ */

int seemsToBeString (char *str)
{
  int  cnt = 0;

  while (cnt<80 && str[cnt]!='\0') 
  {
    if (str[cnt]<0) break;
    if (str[cnt]<32 && str[cnt]!='\t' && str[cnt]!='\n') break;
    cnt++;
  }
  return str[cnt]=='\0';
}

int listIndexItf( ClientData CD,     char* key, 
                  ClientData retPtr, int*  argcPtr, char* argv[])
{
  List* L    = (List*)CD;
  int   idx  = -1;
  int   ac   = *argcPtr;

  if ( ac < 1) return -1;
  if ((idx = listIndex( L, (ClientData)argv[0], 0)) < 0) {

    ERROR( "Can't find item \"%s\" in the %s list.\n", seemsToBeString(argv[0]) ? argv[0] : "(unprintable)", L->typeInfo->name);
    return idx;
  }
  (*argcPtr)--; 
  *((int*)retPtr) = idx; 
  return idx; 
}

/* direct access to itf->result is discouraged!!!
int listFastIndexItf( ClientData cd, int  argc, char* argv[])
{
  List* L    = (List*) cd;

  if ( argc < 2) return TCL_ERROR;
  sprintf(itf->result,"%d",listIndex( L, (ClientData)argv[1], 0));
  return TCL_OK; 
}
*/

/* ------------------------------------------------------------------------
    listFirstItem         

    Example:

    for ( idx  = listFirstItem( L, idx, &i); 
          idx >= 0; idx = listNextItem( L, &i)) {
      itfAppendElement( "%s", L->itemA[L->itemSizeCP * idx]);
    }

   ------------------------------------------------------------------------ */

int listFirstItem( List* L, int idx, int* i)
{
  assert( L);

  if ( L && L->itemA) {
    int  j;

    if ( idx < 0) {
      ERROR( "Can't find item in the list.\n");
      return -1;
    }
    if (! L->sorted) listSort( L);

    for ( j = 0; j < L->itemN; j++) {
      if ( L->sortXA[j] == idx) {
        if ( i) *i = j;
        return idx;
      }
    }
  }
  return -1;
}

int listNextItem( List* L, int* i)
{
  if ( *i < L->itemN-1) {
    (*i)++;
    return L->sortXA[*i];
  }
  return -1;
}

/* ------------------------------------------------------------------------
    listRehash
   ------------------------------------------------------------------------ */

static void listRehash( List* L)
{
  long oldSize = HASHSIZE(L);
  int* table   = NULL;
  int  i, j;

  while ( L->itemN > DENSITY_FACTOR * HASHSIZE(L)) L->hashSizeX++;
  L->hashSizeY = HASHSIZE(L);

  table  = (int*)malloc( HASHSIZE(L) * sizeof(int));

  for ( i = HASHSIZE(L); i > 0; ) table[--i] = -1; 
  for ( i = 0; i < oldSize; i++) {
    long k;
    int  n;
    for ( j = L->hashTable[i]; j >= 0; j = n) {
      n = L->hashXA[j].next;
      k = L->hashXA[j].key % HASHSIZE(L);
      if ( k < 0) k = -k;
      L->hashXA[j].next = table[k];
      table[k]          = j;
    }
  }
  free((char*)L->hashTable);
  L->hashTable = table;
}

/* ------------------------------------------------------------------------
    listConfigureItf
   ------------------------------------------------------------------------ */

int listConfigureItf( ClientData cd, char *var, char *val)
{
  List* L = (List*)cd;
  if (! L) return TCL_OK;

  if (! var) {
    ITFCFG(listConfigureItf,cd,"itemN");
    ITFCFG(listConfigureItf,cd,"blkSize");
    return TCL_OK;
  }
  ITFCFG_Int( var,val,"itemN",    L->itemN,    1);
  ITFCFG_Int( var,val,"blkSize",  L->blkSize,  0);

  ERROR("unknown option '-%s %s'.\n", var, val ? val : "?");
  return TCL_ERROR;
}

/* ------------------------------------------------------------------------
    listAccessItf
   ------------------------------------------------------------------------ */

ClientData listAccessItf( ClientData cd, char *name, TypeInfo **ti)
{
  List* L = (List*)cd;
  int   i;

  if (*name == '.') { 
    if (name[1]=='\0') {
      if ( L->itemN > 0) itfAppendElement( "item(0..%d)",L->itemN-1);
      itfAppendElement( "list");
      *ti = NULL; return NULL;
    }
    else {
      name++;
      if (! strcmp( name, "list")) {
        *ti = itfGetType("List");
        return (ClientData)L;
      }
      else if (sscanf(name,"item(%d)",&i)==1) {
	*ti = L->typeInfo;

	 if ( i >= 0 && i < L->itemN)
              return (ClientData)(L->itemA + L->itemSizeCP * i);
	 else return  NULL;
      }
      return NULL;
    }
  }
  if (*name == ':') { 
    if (name[1]=='\0') {
       listSort( L);
       for ( i = 0; i < L->itemN; i++)
         itfAppendElement( "%s", L->itemA[L->itemSizeCP * L->sortXA[i]]);      
      *ti = NULL; return NULL;
    }
    else {
      name++;
      if ((i = listIndex( L, (ClientData)name, 0)) < 0) return NULL;

      *ti = L->typeInfo;
      return (ClientData)(L->itemA + L->itemSizeCP * i);
    }
  }
  return NULL;
}

/* ------------------------------------------------------------------------
    listPutsItf
   ------------------------------------------------------------------------ */

int listPutsItf( ClientData cd, int argc, char *argv[])
{
  List* L = (List*)cd;
  int   i;

  listSort( L);

  for ( i = 0; i < L->itemN; i++)
    itfAppendElement( "%s", L->itemA[L->itemSizeCP * L->sortXA[i]]);

  return TCL_OK;
}

/* ------------------------------------------------------------------------
    listDelete   remove an item from the list. This changes the
                 indices of all items with index greater than idx.
   ------------------------------------------------------------------------ */

int listDelete( List* L, void* CD)
{
  if ( L && L->itemA) {
    long key = L->hashKey(CD);
    long hX  = key % HASHSIZE(L);
    int  i;
    int  j;

    if ( hX < 0) hX = -hX;    

    j = -1;

    for ( i = L->hashTable[hX]; i >= 0; j = i, i = L->hashXA[i].next) {
      if ( L->hashXA[i].key == key && 
           L->hashCmp( CD, L->itemA + (L->itemSizeCP * L->hashXA[i].idx))) {

        int idx   = L->hashXA[i].idx;
        int sortX = 0;
        int k;

        if (! listIsMoveable( L, idx, L->itemN)) {
          SERROR( "Can't compress list after deleting %d.\n", idx);
          return TCL_ERROR;
        }

        /* remove entry from the hash chain */

        if ( j < 0) L->hashTable[hX]  = L->hashXA[i].next;
        else        L->hashXA[j].next = L->hashXA[i].next;

	if(L->typeInfo)
	  itfTypeCntl((ClientData)&(L->itemA[L->itemSizeCP * idx]),L->typeInfo,T_DELINK);

        if (   L->deinit) {
          if ( L->deinit( &(L->itemA[L->itemSizeCP * idx])) != TCL_OK) {
	    if(L->typeInfo)
	      itfTypeCntl((ClientData)&(L->itemA[L->itemSizeCP * idx]),L->typeInfo,T_LINK);
	    return TCL_ERROR;
          }
        }

        /* update sort array and hash array */

        for ( k = 0; k < L->itemN; k++) {
          if ( L->sortXA[k]      == idx) sortX = k;
          if ( L->sortXA[k]      >  idx) L->sortXA[k]--;
          if ( L->hashXA[k].idx  >  idx) L->hashXA[k].idx--;
          if ( L->hashXA[k].next >  i)   L->hashXA[k].next--;
        }
        for ( k = 0; k < HASHSIZE(L); k++)
          if ( L->hashTable[k] > i) L->hashTable[k]--;

        memmove( L->itemA +  (idx      * L->itemSizeCP), 
                 L->itemA + ((idx + 1) * L->itemSizeCP), 
                 L->itemSize * ( L->itemN - idx - 1));

        if ( i     < L->itemN) 
             memmove( L->hashXA + i, L->hashXA + i + 1, 
                      sizeof(HashRec) * ( L->itemN - i - 1));
        if ( sortX < L->itemN )
             memmove( L->sortXA + sortX, L->sortXA + sortX + 1, 
                      sizeof(int) * ( L->itemN - sortX - 1));

        L->itemN--;

        return TCL_OK;
      }
    }
  }
  return TCL_ERROR;
}

int listDeleteItf( ClientData cd, int argc, char *argv[])
{
  List* L    = (List*)cd;
  int   ac   =  argc - 1;
  char* name =  NULL;
  int   idx;

  if ( itfParseArgv( argv[0], &ac, argv+1, 0,
      "<item>", ARGV_STRING, NULL, &name, NULL, "name of item in list",
       NULL) != TCL_OK) return TCL_ERROR;

  if ((idx = listDelete( L, (ClientData)name)) < 0) 
       return TCL_ERROR;
  else return TCL_OK;
}

/* ------------------------------------------------------------------------
    listName2IndexItf
   ------------------------------------------------------------------------ */

int listName2Index( List* list, char* name)
{
  return listIndex( list, name, 0);
}

int listName2IndexItf( ClientData clientData, int argc, char *argv[] )
{
  List* L  = (List*)clientData;
  int   ac =  argc-1;
  int   i;

  if (itfParseArgv(argv[0], &ac, argv+1, 0,
		   "<names*>",ARGV_REST,NULL,&i,NULL,"list of names",
		   NULL) != TCL_OK)
    return TCL_ERROR;

  while ( i++ < ac) {
    int     tokc;
    char  **tokv;
    int     j = 0;

    if ( Tcl_SplitList (itf,argv[i],&tokc,&tokv) != TCL_OK) return TCL_ERROR;

    while ( j < tokc) {
      itfAppendElement("%d",listIndex(L, tokv[j], 0));
      j++;
    }
    Tcl_Free((char*)tokv);
  }
  return TCL_OK;
}

/* ------------------------------------------------------------------------
    listIndex2NameItf
   ------------------------------------------------------------------------ */

char* listIndex2Name( List* L, int idx)
{
  return ( L->hashKey == (HashKeyFn*)hashStringKey && 
           idx>=0 && idx < L->itemN) ? 
           L->itemA[idx * L->itemSizeCP] : "(null)";
}

int listIndex2NameItf( ClientData clientData, int argc, char *argv[])
{
  List* L      = (List*)clientData;
  int   ac     =  argc-1;
  int   i;

  if (itfParseArgv(argv[0], &ac, argv+1, 0,
		  "<idx*>",ARGV_REST,NULL,&i,NULL,"list of indices",
		   NULL) != TCL_OK)
    return TCL_ERROR;

  while ( i++ < ac) {
    int     tokc;
    char  **tokv;
    int     j = 0;

    if ( Tcl_SplitList (itf,argv[i],&tokc,&tokv) != TCL_OK) return TCL_ERROR;

    while ( j < tokc) {
      int i = atoi(tokv[j]);

      itfAppendElement("%s", listIndex2Name( L, i));
      j++;
    }
    Tcl_Free((char*)tokv);
  }
  return TCL_OK;
}

/* ========================================================================
    Stuff for ngspCL and lctapCL in ibis/ngramLM.c
   ======================================================================== */

/* ------------------------------------------------------------------------
    ngspCLInitItem  -  Initializes new Items in list that havent 
                       been used/allocated yet.
   ------------------------------------------------------------------------ */

/* needed for definitions of NGramScorePair */
/* quite ugly (Florian) */
#include "../ibis/ngramLM.h"

int ngspCLInitItem( /*NGramLMNGSPCache*/List* L, int idx, ClientData CD)
{
  /*  INFO("ngspclinititem begin\n");*/
  int  n   = L->itemN++;
  long key = L->hashKey(CD);
  long hX  = key % HASHSIZE(L);

  if ( hX < 0) hX = -hX;

  NGramScorePair* ngsp = (NGramScorePair*) ( L->itemA + (L->itemSizeCP * idx ) );
  ngsp->ngram = NULL;

  if ( L->init) 
       L->init( L->itemA + (L->itemSizeCP * idx), CD);

  if(L->typeInfo)
    itfTypeCntl((ClientData)&(L->itemA[L->itemSizeCP * idx]),
		L->typeInfo,T_LINK);

  L->sortXA[n]      = idx;
  L->sorted         = 0;

  L->hashXA[n].key  = key;
  L->hashXA[n].idx  = idx;
  L->hashXA[n].next = L->hashTable[hX];
  L->hashTable[hX]  = n;

  if ( L->itemN > DENSITY_FACTOR * HASHSIZE(L)) listRehash(L);
  /*  INFO ("ngspCLinititem end\n");*/
  return idx;
}

/* ------------------------------------------------------------------------
    ngspCLNewItem   insert a new item in the ngspCacheList (actually appending 
                    it to the end of the ngspCL). If there is no space left 
                    in the ngspCL, it is automatically enlarged by <blkSize> 
                    items. The new ngspCL is no longer sorted.
   ------------------------------------------------------------------------ */

int ngspCLNewItem( /*NGramLMNGSPCache*/List* L, void* CD)
{
  /*  INFO("ngspCLnewitem\n");*/
  int idx = L->itemN;

  /* hash list as ring buffer */
  if (L->itemN >= L->itemMax) {
    long oldkey = 0;
    long oldhX  = 0;
    long newkey = L->hashKey(CD);
    long newhX  = newkey % HASHSIZE(L);

    /* select item to overwrite */
    idx = L->itemN % L->itemMax;

    oldkey = L->hashXA[idx].key;
    oldhX  = oldkey % HASHSIZE(L);

    if ( newhX < 0) newhX = -newhX;
    if ( oldhX < 0) oldhX = -oldhX;
    
    if (oldkey != newkey) {
      int i = L->hashTable[oldhX];
      /* reset old hashkey */
      if (i == idx) 
	L->hashTable[oldhX] = L->hashXA[idx].next;
      else {
	while (i >= 0 && idx !=  L->hashXA[i].next) i = L->hashXA[i].next;
	assert(idx == L->hashXA[i].next);
	L->hashXA[i].next = L->hashXA[idx].next;
      }

      /* setup new hashkey */     
      L->hashXA[idx].key  = newkey;
      L->hashXA[idx].next = L->hashTable[newhX];
      L->hashTable[newhX] = idx;
    }

    /* init idx entry with new parameters from CD */
    if ( L->init) 
      L->init( L->itemA + (L->itemSizeCP * idx), CD);

    L->hashXA[idx].idx  = L->itemN;

    /* */
    L->itemN++;

    return L->hashXA[idx].idx;
  }

  if ( idx < L->allocN) return ngspCLInitItem( L, idx, CD);

  if (! L->itemA) {
    L->allocN = L->blkSize;
    if (!(L->itemA  = (char**  )malloc( L->allocN * L->itemSize))     ||
        !(L->hashXA = (HashRec*)malloc( L->allocN * sizeof(HashRec))) ||
        !(L->sortXA = (int*    )malloc( L->allocN * sizeof(int)))) {

      ERROR( "Can't allocate indexed list.\n");
      return -1;
    }
  }
  else {
    if (! listIsMoveable( L, 0, L->itemN)) {
      ERROR( "Can't reallocate list.\n");
      return -1;
    }
    L->allocN += L->blkSize;
    if (!(L->itemA  = (char**  )realloc(L->itemA, L->allocN * L->itemSize))     ||
        !(L->hashXA = (HashRec*)realloc(L->hashXA,L->allocN * sizeof(HashRec))) ||
        !(L->sortXA = (int*    )realloc(L->sortXA,L->allocN * sizeof(int)))) {

      ERROR( "Can't reallocate index list.\n");
      return -1;
    }
  }
  return ngspCLInitItem( L, idx, CD);
}

/* ------------------------------------------------------------------------
    lctapCLInitItem  -  Initializes new Items in list that havent 
                        been used/allocated yet.
   ------------------------------------------------------------------------ */


int lctapCLInitItem( /*NGramLMLCTAPCache*/List* L, int idx, ClientData CD)
{
  /*  INFO("lctapclinititem begin\n");*/
  int  n   = L->itemN++;
  long key = L->hashKey(CD);
  long hX  = key % HASHSIZE(L);

  if ( hX < 0) hX = -hX;

  NGramLCTAPair* lctap = (NGramLCTAPair*) ( L->itemA + (L->itemSizeCP * idx ) );
  lctap->arr = NULL;

  if ( L->init) 
       L->init( L->itemA + (L->itemSizeCP * idx), CD);

  if(L->typeInfo)
    itfTypeCntl((ClientData)&(L->itemA[L->itemSizeCP * idx]),
		L->typeInfo,T_LINK);

  L->sortXA[n]      = idx;
  L->sorted         = 0;

  L->hashXA[n].key  = key;
  L->hashXA[n].idx  = idx;
  L->hashXA[n].next = L->hashTable[hX];
  L->hashTable[hX]  = n;

  if ( L->itemN > DENSITY_FACTOR * HASHSIZE(L)) listRehash(L);
  /*  INFO("lctapclinititem end\n");*/
  return idx;
}

/* ------------------------------------------------------------------------
    ngspCLNewItem   insert a new item in the ngspCacheList (actually appending 
                    it to the end of the ngspCL). If there is no space left 
                    in the ngspCL, it is automatically enlarged by <blkSize> 
                    items. The new ngspCL is no longer sorted.
   ------------------------------------------------------------------------ */

int lctapCLNewItem( /*NGramLMNGSPCache*/List* L, void* CD)
{
  /* INFO("lctapCLNewItem begin\n");*/
  int idx = L->itemN;

  /* hash list as ring buffer */
  if (L->itemN >= L->itemMax) {
    long oldkey = 0;
    long oldhX  = 0;
    long newkey = L->hashKey(CD);
    long newhX  = newkey % HASHSIZE(L);

    /* select item to overwrite */
    idx = L->itemN % L->itemMax;

    oldkey = L->hashXA[idx].key;
    oldhX  = oldkey % HASHSIZE(L);

    if ( newhX < 0) newhX = -newhX;
    if ( oldhX < 0) oldhX = -oldhX;
    
    if (oldkey != newkey) {
      int i = L->hashTable[oldhX];
      /* reset old hashkey */
      if (i == idx) 
	L->hashTable[oldhX] = L->hashXA[idx].next;
      else {
	while (i >= 0 && idx !=  L->hashXA[i].next) i = L->hashXA[i].next;
	assert(idx == L->hashXA[i].next);
	L->hashXA[i].next = L->hashXA[idx].next;
      }

      /* setup new hashkey */     
      L->hashXA[idx].key  = newkey;
      L->hashXA[idx].next = L->hashTable[newhX];
      L->hashTable[newhX] = idx;
    }

    /* init idx entry with new parameters from CD */
    if ( L->init) 
      L->init( L->itemA + (L->itemSizeCP * idx), CD);

    L->hashXA[idx].idx  = L->itemN;

    /* */
    L->itemN++;

    return L->hashXA[idx].idx;
  }

  if ( idx < L->allocN) return lctapCLInitItem( L, idx, CD);

  if (! L->itemA) {
    L->allocN = L->blkSize;
    if (!(L->itemA  = (char**  )malloc( L->allocN * L->itemSize))     ||
        !(L->hashXA = (HashRec*)malloc( L->allocN * sizeof(HashRec))) ||
        !(L->sortXA = (int*    )malloc( L->allocN * sizeof(int)))) {

      ERROR( "Can't allocate indexed list.\n");
      return -1;
    }
  }
  else {
    if (! listIsMoveable( L, 0, L->itemN)) {
      ERROR( "Can't reallocate list.\n");
      return -1;
    }
    L->allocN += L->blkSize;
    if (!(L->itemA  = (char**  )realloc(L->itemA, L->allocN * L->itemSize))     ||
        !(L->hashXA = (HashRec*)realloc(L->hashXA,L->allocN * sizeof(HashRec))) ||
        !(L->sortXA = (int*    )realloc(L->sortXA,L->allocN * sizeof(int)))) {

      ERROR( "Can't reallocate index list.\n");
      return -1;
    }
  }
  return lctapCLInitItem( L, idx, CD);
}

/* ========================================================================
    Type Declaration
   ======================================================================== */

static Method listMethod[] = 
{ 
  { "puts",   listPutsItf,       NULL },
  { "name",   listIndex2NameItf, "translate indices to names"  },
  { "index",  listName2IndexItf, "translate names to indices"  },
  { "delete", listDeleteItf,     "remove distribution from the set"  },
  {  NULL,    NULL,              NULL } 
};

static TypeInfo listInfo = { "List", 0, 0, listMethod, NULL, NULL,
                              listConfigureItf,  listAccessItf, 
                              itfTypeCntlDefaultNoLink,
	                     "List of indexed items\n" } ;

static int listInitialized = 0;

int List_Init ( )
{
  if (! listInitialized) {
    listInitialized = 1;
    itfNewType ( &listInfo );
  }
  return TCL_OK;
}


