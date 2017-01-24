/* ========================================================================
    JanusRTk   Janus Recognition Toolkit
               ------------------------------------------------------------
               Object: Block memory management
               ------------------------------------------------------------

    Author  :  Florian Metze
    Module  :  bmem.c
    Date    :  $Id: bmem.c 3242 2010-05-17 18:17:20Z metze $

    Remarks :  These functions allocate memory in huge chunks and 
               provide functions to reserve it in small pieces
	       You can then free the memory all at once, e.g. at the end
	       of an utterance - the point of the whole thing is to
	       avoid memory fragmentation

   ========================================================================

    This software was developed by

    the  University of Karlsruhe       and  Carnegie Mellon University
         Dept. of Informatics               Dept. of Computer Science
         ILKD, Lehrstuhl Prof.Waibel        Alex Waibel's NN & Speech Group
         Am Fasanengarten 5                 5000 Forbes Ave
         D-76131 Karlsruhe                  Pittsburgh, PA 15213
         - WestGermany -                   - USA -

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

    $Log: bmem.c,v $
    Revision 4.3  2004/10/27 11:57:18  metze
    x86_64 change

    Revision 4.2  2004/09/16 14:36:36  metze
    Made code 'x86_64'-clean

    Revision 4.1  2003/08/14 11:19:58  fuegen
    Merged changes on branch jtk-01-01-15-fms (jaguar -> ibis-013)

    Revision 1.1.2.22  2003/03/12 10:44:18  metze
    Memory alignment fixed in bmemAlloc, SVX types fixed for reading

    Revision 1.1.2.21  2002/11/21 17:10:07  fuegen
    windows code cleaning

    Revision 1.1.2.20  2002/07/18 13:50:17  soltau
    Added bmemCheck

    Revision 1.1.2.19  2002/07/18 12:15:36  soltau
    occuP works now on bit-level

    Revision 1.1.2.18  2002/05/27 14:33:11  metze
    Added BMEM_DEBUG and removed bmemNext

    Revision 1.1.2.17  2002/03/18 17:45:46  soltau
    Added matrix allocation based on bmem

    Revision 1.1.2.16  2002/02/11 13:33:27  soltau
    bmemPuts: count deallocated memory

    Revision 1.1.2.15  2002/02/06 13:18:49  fuegen
    added index functionality

    Revision 1.1.2.14  2002/02/01 10:37:52  fuegen
    minor bug fixes in bmemFree and bmemClear

    Revision 1.1.2.13  2001/11/20 16:34:33  soltau
    optimized memory management

    Revision 1.1.2.12  2001/07/26 09:27:53  metze
    Added bmemSet/ Next

    Revision 1.1.2.11  2001/06/28 12:15:44  metze
    Eliminated 'spurious escape' on SUN

    Revision 1.1.2.10  2001/06/28 11:44:47  metze
    Added more verbose output

    Revision 1.1.2.9  2001/06/26 15:27:33  soltau
    Added bmemPuts
    Added Tcl Interface

    Revision 1.1.2.8  2001/06/25 21:15:54  soltau
    Added case 0 in bmemAlloc

    Revision 1.1.2.7  2001/06/25 21:04:38  soltau
    Check for allocation failure

    Revision 1.1.2.6  2001/06/25 20:58:10  soltau
    Changed Block management

    Revision 1.1.2.5  2001/06/22 14:45:49  soltau
    Added bmemClear
    Fixed bmemFree

    Revision 1.1.2.4  2001/06/18 21:34:50  hyu
    Minor bug fix.

    Revision 1.1.2.3  2001/06/18 12:22:40  metze
    Do it again

    Revision 1.1.2.2  2001/06/18 12:06:32  metze
    Made it compile under SUN

    Revision 1.1.2.1  2001/06/13 13:45:31  metze
    First shot


   ======================================================================== */

#include "bmem.h"
#include "stdlib.h"
#include "itf.h"

char bmemRCSVersion[]= "@(#)1$Id: bmem.c 3242 2010-05-17 18:17:20Z metze $";

/* ------------------------------------------------------------------------
    forward declarations 
   ------------------------------------------------------------------------ */

extern TypeInfo  bmemInfo;


/*
#define BMEM_DEBUG
*/
#ifdef BMEM_DEBUG


/* ------------------------------------------------------------------------
     Debug version of the library (direct mallocs)
   ------------------------------------------------------------------------ */
        
BMem*      bmemCreate    (BMemT itemN, size_t size)
{
  int* a = (int*) malloc (sizeof (int));  
  *a = (int) size;
  return (BMem*) a;
}

int        bmemFree      (BMem* bmemP)
{
  return TCL_OK;
}  

void*      bmemItem      (BMem* bmemP)
{
  return malloc (*((int*)bmemP));
}

void*      bmemAlloc     (BMem* bmemP, size_t n)
{
  return malloc (n);
}
  
void       bmemDealloc   (BMem* bmemP, void* itemP, size_t n)
{
  return;
} 

int        bmemDelete    (BMem* bmemP, void* itemP)
{ 
  return TCL_OK;
}
  
int        bmemClear     (BMem* bmemP)
{
  return TCL_OK;
}


#else


/* ========================================================================
    Block Memory allocation routines
   ======================================================================== */

/* ------------------------------------------------------------------------
    bmemCreate
   ----------
    Initializes an bmem structure, returns a pointer to it (used for 
    future reference), can be used in two modes:
    1) 'size != 0': 'itemN' then gives the number of items reserved
                    en bloc, use 'bmemItem' and you can also free items
		    with 'bmemFree'
    2) 'size == 0': 'itemN' then gives the block size in bytes, use 
                    'bmemAlloc' to allocate memory but you 
                    cannot free items
   ------------------------------------------------------------------------ */

BMem* bmemCreate (BMemT itemN, size_t size)
{
  BMem* bmemP = (BMem*) malloc (sizeof (BMem));

  if (NULL == bmemP) {
    ERROR("bmemCreate: alloction failure\n");
    return NULL;
  }

  if (itemN == 0) {
    free (bmemP);
    return NULL;
  }

  bmemP->itemN    =  itemN;
  bmemP->size     =  size;
  bmemP->blkN     =  0;
  bmemP->blkA     =  NULL;
  bmemP->blkX     =  0;
  bmemP->itemX    =  0;
  bmemP->itemXN   =  0;
  bmemP->itemXNIL =  BMemTNIL;
  bmemP->itemXMAX =  BMemTMAX;

  bmemP->freeB.N  = 0;
  bmemP->freeB.aN = NULL;
  bmemP->freeB.aS = NULL;
  bmemP->freeB.aA = NULL;

  return bmemP;
}


/* ------------------------------------------------------------------------
    bmemItem
   ------------------------------------------------------------------------ */

void* bmemItem (BMem* bmemP)
{
  BMemT blkX  = bmemP->blkN-1;
  BMemT blkX1 = bmemP->blkN;

  if ( bmemP->itemXN == bmemP->itemXMAX ) {
    ERROR ("bmemItem: maximum number of items (%d) reached, change appropriate defines!\n",
	   bmemP->itemXMAX);
    return NULL;
  }

  /* ------------------------------------------ */
  /* case 1: there are free (non-deleted) items */
  /* ------------------------------------------ */

  /* we are working with unsigned values, though we need blkX1=blkX+1 */
  while (blkX1 > 0 && bmemP->blkA[blkX].itemF >= bmemP->itemN) {
    blkX--;
    blkX1--;
  }
  if (blkX1 > 0) {
    MBlk* blkP = bmemP->blkA+blkX;
    char*   mP = (char*) blkP->memP;
    UCHAR*   p = blkP->occuP + blkP->itemF/8;
    int      i = blkP->itemF % 8;

    blkP->itemF++;
    blkP->itemE--;   
    bmemP->itemXN++;

    *p |= (1<<i);

    return mP + bmemP->size * (blkP->itemF - 1);
  }

  /* ------------------------------------------ */
  /* case 2: there are free (deleted) items     */
  /* ------------------------------------------ */

  blkX  = bmemP->blkN-1;
  blkX1 = bmemP->blkN;
  while (blkX1 > 0 && bmemP->blkA[blkX].itemE < 1) {
    blkX--;
    blkX1--;
  }
  if (blkX1 > 0) {
    MBlk *blkP = bmemP->blkA+blkX;
    char *mP   = (char*) blkP->memP;
    UCHAR *p   = blkP->occuP;
    int    i   = 1;
    while (*p == 0xFF) {
      p++;
    }
    blkP->itemE--;
    bmemP->itemXN++;

    /* check bit-field for occupied items */
    for (i=0;i<8;i++) {
      if (((1<<i) & *p) == 0) {
	*p |= (1<<i);
	return mP + bmemP->size * ( (p - blkP->occuP)*8 + i);
      }
    }
    abort();
  }

  /* ------------------------------------------ */
  /* case 3: we need a new block                */
  /* ------------------------------------------ */

  blkX = bmemP->blkN;
  bmemP->blkN++;
  if (NULL == (bmemP->blkA= realloc(bmemP->blkA,bmemP->blkN*sizeof(MBlk)))) {
    ERROR("bmemItem: allocation failure\n");
    return NULL;
  }

  bmemP->blkA[blkX].itemF = 1;
  bmemP->blkA[blkX].itemE = bmemP->itemN-1;

  if (NULL == (bmemP->blkA[blkX].occuP = calloc((int)(bmemP->itemN/8)+1, sizeof(UCHAR)))) {
    ERROR("bmemItem: allocation failure\n");
    return NULL;
  }
  if (NULL == (bmemP->blkA[blkX].memP = malloc(bmemP->itemN*bmemP->size))) {
    ERROR("bmemItem: allocation failure\n");
    return NULL;
  }

  bmemP->blkA[blkX].occuP[0] = 1;
  bmemP->itemXN++;

  return bmemP->blkA[blkX].memP;
}


/* ------------------------------------------------------------------------
    bmemAlloc
   ------------------------------------------------------------------------ */

/* Alignment restrictions in multiples of bytes (4 for 32-bit machines) */
#ifdef SSE_OPT
#define ALIGNMENTSIZE 16
#else
#define ALIGNMENTSIZE sizeof(size_t)
#endif

void* bmemAlloc (BMem* bmemP, size_t n)
{
  size_t size;
  BMemT  blkX, blkY;

  /* Make sure the alignment stays ok by requesting multiples
     of ALIGNMENTSIZE only */
  if (n % ALIGNMENTSIZE) n += ALIGNMENTSIZE - (n % ALIGNMENTSIZE);

  if (bmemP->size > 0) {
    ERROR ("bmemAlloc: You cannot use bmemItem and bmemAlloc on the same block!\n");
    return NULL;
  }

  /* check for freed items */
  blkY = BMemTNIL;
  size = BMemTMAX;

  for (blkX=0;blkX<bmemP->freeB.N;blkX++) {
    BMemT s = bmemP->freeB.aS[blkX] - n;
    if (bmemP->freeB.aS[blkX] >= n && s < size &&  bmemP->freeB.aN[blkX] > 0) {
      size = s;
      blkY = blkX;
    }
  }
  if (blkY !=  BMemTNIL) {
    bmemP->freeB.aN[blkY]--;
    return bmemP->freeB.aA[blkY][bmemP->freeB.aN[blkY]];
  }
  if (n >= bmemP->itemN) {
    /* ------------------------------------------ */
    /* case 0: there never was enough space       */
    /* ------------------------------------------ */

    blkX = bmemP->blkN;
    bmemP->blkN++;
    if (NULL == (bmemP->blkA= realloc(bmemP->blkA,bmemP->blkN*sizeof(MBlk)))) {
      ERROR("bmemAlloc: allocation failure\n");
      return NULL;
    }
    
    bmemP->blkA[blkX].itemF = n;
    bmemP->blkA[blkX].itemE = 0;
    bmemP->blkA[blkX].occuP = NULL;

    if (NULL == (bmemP->blkA[blkX].memP = malloc(n))) {
      ERROR("bmemAlloc: allocation failure\n");
      return NULL;
    }

    return bmemP->blkA[blkX].memP;
  }

  /* ------------------------------------------ */
  /* case 1: there is enough space              */
  /* ------------------------------------------ */

  blkX=0;
  while (blkX < bmemP->blkN && bmemP->blkA[blkX].itemE < n) {
    blkX++;
  }
  if (blkX < bmemP->blkN) {
    MBlk* blkP = bmemP->blkA+blkX;
    char *mP = (char*) blkP->memP;
  
    blkP->itemF += n;
    blkP->itemE -= n;

    return mP + blkP->itemF - n;
  }

  /* ------------------------------------------ */
  /* case 2: we need a new block                */
  /* ------------------------------------------ */

  bmemP->blkN++;
  if (NULL == (bmemP->blkA= realloc(bmemP->blkA,bmemP->blkN*sizeof(MBlk)))) {
    ERROR("bmemAlloc: allocation failure\n");
    return NULL;
  }
  bmemP->blkA[blkX] =  bmemP->blkA[0];

  bmemP->blkA[0].itemF = n;
  bmemP->blkA[0].itemE = bmemP->itemN-n;
  bmemP->blkA[0].occuP = NULL;

  if (NULL == (bmemP->blkA[0].memP = malloc(bmemP->itemN))) {
    ERROR("bmemAlloc: allocation failure\n");
    return NULL;
  }

  return bmemP->blkA[0].memP;

}

/* ------------------------------------------------------------------------
   bmemDealloc
   ------------------------------------------------------------------------ */

void bmemDealloc (BMem* bmemP, void* itemP, size_t n)
{
  BMemT blkX = 0;

  if (bmemP->size > 0) {
    ERROR ("bmemDealloc: You cannot use bmemFree and bmemDelloc on the same block!\n");
    return;
  }

  while (blkX < bmemP->freeB.N && bmemP->freeB.aS[blkX] != n) blkX++;
  if (blkX == bmemP->freeB.N) {
    bmemP->freeB.N+=1;
    if (NULL == (bmemP->freeB.aN = realloc(
	         bmemP->freeB.aN,bmemP->freeB.N*sizeof(int)))) {
      ERROR("bmemDealloc: allocation failure\n");
      return;
    }
    if (NULL == (bmemP->freeB.aS = realloc(
	         bmemP->freeB.aS,bmemP->freeB.N*sizeof(int)))) {
      ERROR("bmemDealloc: allocation failure\n");
      return;
    }
    if (NULL == (bmemP->freeB.aA = realloc(
	         bmemP->freeB.aA,bmemP->freeB.N*sizeof(void**)))) {
      ERROR("bmemDealloc: allocation failure\n");
      return;
    }
    bmemP->freeB.aN[blkX] = 0;
    bmemP->freeB.aS[blkX] = n;
    bmemP->freeB.aA[blkX] = NULL;
  }

  if (bmemP->freeB.aN[blkX] % 100 == 0) {
    if (NULL == (bmemP->freeB.aA[blkX] = realloc(
	bmemP->freeB.aA[blkX],(100+bmemP->freeB.aN[blkX])*sizeof(void*)))) {
      ERROR("bmemDealloc: allocation failure\n");
      return;
    }
  }
  bmemP->freeB.aA[blkX][bmemP->freeB.aN[blkX]] =itemP;
  bmemP->freeB.aN[blkX]++;

  return;
} 

/* ------------------------------------------------------------------------
    bmemCheck : check if 'itemP' is occupied
   ------------------------------------------------------------------------ */

int bmemCheck (BMem* bmemP, void* itemP)
{
  char   *iP   = (char*) itemP;
  BMemT   blkX = 0;
  BMemT  itemX = 0;
  UCHAR*     p = NULL;
  int        i = 0;

  blkX=0;
  while (blkX < bmemP->blkN) {
    char *mP = (char*) bmemP->blkA[blkX].memP;
    itemX = (iP - mP)/bmemP->size;
    if (iP >= mP && itemX < bmemP->itemN) break;   
    blkX++;
  }

  if (bmemP->itemXN == 0) {
    ERROR ("bmemCheck: No items stored.\n");
    return TCL_ERROR;
  }
 
  if (blkX == bmemP->blkN) {
    WARN ("bmemCheck: Invalid pointer detected.\n");
    return TCL_ERROR;
  }

  if (! bmemP->blkA[blkX].occuP) {
    WARN ("bmemCheck: Cannot check blocks allocated with 'bmemAlloc'!\n");
    return TCL_ERROR;
  }

  p = bmemP->blkA[blkX].occuP + itemX/8;
  i = itemX % 8;

  if (((1<<i) & *p) > 0) 
    return TCL_OK;
  else
    return TCL_ERROR;

}

/* ------------------------------------------------------------------------
    bmemDelete
   ------------------------------------------------------------------------ */

int bmemDelete (BMem* bmemP, void* itemP)
{
  char   *iP   = (char*) itemP;
  BMemT   blkX = 0;
  BMemT  itemX = 0;
  UCHAR*     p = NULL;
  int        i = 0;

  blkX=0;
  while (blkX < bmemP->blkN) {
    char *mP = (char*) bmemP->blkA[blkX].memP;
    itemX = (iP - mP)/bmemP->size;
    if (iP >= mP && itemX < bmemP->itemN) break;   
    blkX++;
  }

  if (bmemP->itemXN == 0) {
    ERROR ("bmemDelete: No items stored.\n");
    return TCL_ERROR;
  }
 
  if (blkX == bmemP->blkN) {
    WARN ("bmemDelete: Invalid pointer detected.\n");
    return TCL_ERROR;
  }

  if (! bmemP->blkA[blkX].occuP) {
    WARN ("bmemDelete: Cannot free blocks allocated with 'bmemAlloc'!\n");
    return TCL_ERROR;
  }

  /* Free it */
  bmemP->blkA[blkX].itemE++;
  bmemP->itemXN--;

  p = bmemP->blkA[blkX].occuP + itemX/8;
  i = itemX % 8;

  /* sanity check */
  if (((1<<i) & *p) == 0) {
    WARN ("bmemDelete: Pointer was not allocated.\n");
    return TCL_OK;
  }

  *p &= ~(1<<i);

  return TCL_OK;
}

/* ------------------------------------------------------------------------
    bmemClear
   ------------------------------------------------------------------------ */

int bmemClear (BMem* bmemP)
{
  BMemT blkX;

  if ( !bmemP ) {
    ERROR("bmemClear: nothing to clear\n");
    return TCL_ERROR;
  }
  
  if (bmemP->blkA) {
    for (blkX=0;blkX<bmemP->blkN;blkX++) {
      MBlk* blkP= bmemP->blkA+blkX;
      if (blkP->occuP) free(blkP->occuP);
      if (blkP->memP)  free(blkP->memP);
    }
    free(bmemP->blkA);
  }
  bmemP->blkA   = NULL;
  bmemP->blkN   = 0;
  bmemP->blkX   = 0;
  bmemP->itemX  = 0;
  bmemP->itemXN = 0;

  if (bmemP->freeB.aA) {
    for (blkX=0;blkX<bmemP->freeB.N;blkX++) {
      bmemP->freeB.aN[blkX] = 0;
    }
  }
  return TCL_OK;
}

/* ------------------------------------------------------------------------
    bmemFree
   ------------------------------------------------------------------------ */

int bmemFree (BMem* bmemP)
{
  BMemT blkX;

  if ( bmemClear(bmemP) != TCL_OK ) return TCL_ERROR;

  if (bmemP->freeB.aA) {
    for (blkX=0;blkX<bmemP->freeB.N;blkX++) {
      if (bmemP->freeB.aA[blkX]) free(bmemP->freeB.aA[blkX]);
    }
    free(bmemP->freeB.aA);
  }
  if (bmemP->freeB.aN) free(bmemP->freeB.aN);
  if (bmemP->freeB.aS) free(bmemP->freeB.aS);
  bmemP->freeB.aN = NULL;
  bmemP->freeB.aS = NULL;
  bmemP->freeB.aA = NULL;
  bmemP->freeB.N  = 0;

  free(bmemP);

  return TCL_OK;
}


#endif


/* ----------------------------------------------------------------------
   bmemSetLimits
   ---------------
   set limits to allow consitency checking of indices
   ---------------------------------------------------------------------- */

int bmemSetLimits (BMem *bmemP, BMemT itemXNIL, BMemT itemXMAX) {

  bmemP->itemXNIL = itemXNIL;
  bmemP->itemXMAX = itemXMAX;

  return TCL_OK;
}


/* ----------------------------------------------------------------------
   bmemGetItem
   ---------------
   returns the item for a given index (use ONLY together with bmemItem)
   ---------------------------------------------------------------------- */

void* bmemGetItem (BMem *bmemP, BMemT itemX) {

  if ( itemX == bmemP->itemXNIL || itemX >= bmemP->itemXN ) {
    if (itemX!=bmemP->itemXNIL)
      WARN ("bmemGetItem: out of range");
    return NULL;
  }

  return (char*)bmemP->blkA[itemX / bmemP->itemN].memP +
    (bmemP->size * (itemX % bmemP->itemN));
}
/*
BMemT bmemGetItemL (BMem *bmemP, BMemT itemX) {

  if ( itemX == bmemP->itemXNIL || itemX >= bmemP->itemXN ) return 0;

  return (BMemT) bmemP->blkA[itemX / bmemP->itemN].memP + 
    bmemP->size * (itemX % bmemP->itemN);
}
*/

/* ----------------------------------------------------------------------
   bmemGetItemX
   ---------------
   returns the index of a given item (use ONLY together with bmemItem)
   ---------------------------------------------------------------------- */

BMemT bmemGetItemX (BMem *bmemP, void *itemP) {

  BMemT blkX;
  BMemT max = bmemP->size * bmemP->itemN;

  if ( !itemP ) return bmemP->itemXNIL;

  for (blkX=0; blkX<bmemP->blkN; blkX++) {
    if ( (char*)itemP >= (char*)bmemP->blkA[blkX].memP &&
	 (char*)itemP <  (char*)bmemP->blkA[blkX].memP + max )
    return (blkX * bmemP->itemN) +
           ((char*)itemP - (char*)bmemP->blkA[blkX].memP) / bmemP->size;
  }

  WARN ("bmemGetItemX: not found");

  return bmemP->itemXNIL;
}

/* ----------------------------------------------------------------------
   bmemGetItemN
   ---------------
   returns the stored number of items
   ---------------------------------------------------------------------- */

BMemT bmemGetItemN (BMem *bmemP) {

  return bmemP->itemXN;
}

/* ------------------------------------------------------------------------
    bmemPuts
   ------------------------------------------------------------------------ */

int bmemPuts (BMem* bmemP, int v)
{
  BMemT  useN   = 0;
  BMemT  delN   = 0;
  BMemT  fdelN  = 0;
  BMemT  allocN = bmemP->blkN * bmemP->itemN;
  BMemT  blkX;
  float  f      = 0.0;

  for (blkX=0;blkX<bmemP->blkN;blkX++) {
    MBlk* blkP = bmemP->blkA+blkX;
    useN += (bmemP->itemN-blkP->itemE);
    delN += (blkP->itemE + blkP->itemF -  bmemP->itemN);
  }

  if (bmemP->size > 0) {
    allocN *= bmemP->size;
    useN   *= bmemP->size;
    delN   *= bmemP->size;
  }

  /* freed alloc memory */
  for (blkX=0;blkX<bmemP->freeB.N;blkX++) {
    fdelN += bmemP->freeB.aS[blkX]*bmemP->freeB.aN[blkX];
  }
  delN += fdelN;
  useN -= fdelN;

  if (allocN > 0)
    f = 100.0 * useN / allocN;

  itfAppendResult("blockN          = %d\n",bmemP->blkN);
  itfAppendResult("itemN           = %d\n",bmemP->itemN);
  itfAppendResult("allocated bytes = %d\n",allocN);
  itfAppendResult("used      bytes = %d\n",useN);
  itfAppendResult("deleted   bytes = %d\n",delN);
  itfAppendResult("fragmentation   = %2.2f%%\n",f);

  if (v) { 
    itfAppendResult("free.blockN     = %d\n",bmemP->freeB.N);
    for (blkX=0;blkX<bmemP->freeB.N;blkX++) {
      itfAppendResult("free.blockX(%d)  = %d items a %d\n",
		      blkX,
		      bmemP->freeB.aN[blkX],bmemP->freeB.aS[blkX]);
    }
  }
  return TCL_OK;
}

static int bmemPutsItf (ClientData cd, int argc, char *argv[])
{
  BMem* bmemP = (BMem*)cd;
  int      ac = argc-1;
  int       v = 0;

  if ( itfParseArgv( argv[0], &ac, argv+1, 0,
      "-v",     ARGV_INT, NULL, &v, NULL, "verbose output",
       NULL) != TCL_OK) return TCL_ERROR; 

  return bmemPuts(bmemP,v);
}

/* ------------------------------------------------------------------------
    fvectorCreate_BMEM
   ------------------------------------------------------------------------ */

FVector* fvectorCreate_BMEM(int n, BMem* memP)
{
  int i;
  FVector* f = bmemAlloc(memP,sizeof(FVector));
  f->n     = n;
  f->count = 0.0;
  f->vecA  = bmemAlloc(memP,n*sizeof(float));
  for (i= 0;i< n;i++) f->vecA[i] = 0.0;
  return f;
}

/* ------------------------------------------------------------------------
    fmatrixCreate_BMEM
   ------------------------------------------------------------------------ */

FMatrix* fmatrixCreate_BMEM(int m,int n, BMem* memP)
{
  int i,j;
  FMatrix* f = bmemAlloc(memP,sizeof(FMatrix));
  f->m = m;
  f->n = n;
  f->matPA    = bmemAlloc(memP,m*sizeof(float*));
  f->matPA[0] = bmemAlloc(memP,m*n*sizeof(float));
  for ( i=1; i<m; i++) f->matPA[i] = f->matPA[i-1] + n;
  for ( i=0; i<m; i++)
    for ( j = 0; j < n; j++) f->matPA[i][j] = 0.0;
  return f;
}

/* ------------------------------------------------------------------------
    dmatrixCreate_BMEM
   ------------------------------------------------------------------------ */

DMatrix* dmatrixCreate_BMEM(int m,int n, BMem* memP)
{
  int i,j;
  DMatrix* f = bmemAlloc(memP,sizeof(DMatrix));
  f->m = m;
  f->n = n;
  f->matPA    = bmemAlloc(memP,m*sizeof(double*));
  f->matPA[0] = bmemAlloc(memP,m*n*sizeof(double));
  for ( i=1; i<m; i++) f->matPA[i] = f->matPA[i-1] + n;
  for ( i=0; i<m; i++)
    for ( j = 0; j < n; j++) f->matPA[i][j] = 0.0;
  return f;
}

/* ========================================================================
    Type Information
   ======================================================================== */

Method bmemMethod[] = 
{
  { "puts", bmemPutsItf, "displays the allocation status"  },
  { NULL, NULL, NULL }
} ;

TypeInfo bmemInfo = { 
        "BMem", 0, 0, bmemMethod, 
        NULL, NULL, NULL, NULL, 
        itfTypeCntlDefaultNoLink,
        "Block Memory" };

static int bmemInitialized = FALSE;

extern int BMem_Init (void)
{
  if (! bmemInitialized) {
    itfNewType (&bmemInfo);
    bmemInitialized = TRUE;
  }
  return TCL_OK;
}
