/* ========================================================================
    JanusRTk   Janus Recognition Toolkit
               ------------------------------------------------------------
               Object: Block Memory management
               ------------------------------------------------------------

    Author  :  Florian Metze
    Module  :  bmem.h
    Date    :  $Id: bmem.h 2390 2003-08-14 11:20:32Z fuegen $

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
    Revision 4.1  2003/08/14 11:19:58  fuegen
    Merged changes on branch jtk-01-01-15-fms (jaguar -> ibis-013)

    Revision 1.1.2.12  2002/07/18 13:47:30  soltau
    Added bmemCheck

    Revision 1.1.2.11  2002/07/18 12:15:09  soltau
    occuP becomes unsigned

    Revision 1.1.2.10  2002/03/18 17:45:33  soltau
    Added matrix allocation based on bmem

    Revision 1.1.2.9  2002/02/11 13:33:44  soltau
    beautified

    Revision 1.1.2.8  2002/02/06 13:18:49  fuegen
    added index functionality

    Revision 1.1.2.7  2001/11/20 16:34:18  soltau
    optimized memory management

    Revision 1.1.2.6  2001/07/26 09:28:17  metze
    Added bmemSet /Next

    Revision 1.1.2.5  2001/06/26 15:28:42  soltau
    Added BMem_Init

    Revision 1.1.2.4  2001/06/25 20:57:53  soltau
    Changed Block management

    Revision 1.1.2.3  2001/06/22 14:46:16  soltau
    Added bmemClear

    Revision 1.1.2.2  2001/06/18 12:06:35  metze
    Made it compile under SUN

    Revision 1.1.2.1  2001/06/13 13:45:34  metze
    First shot


   ======================================================================== */

#ifndef _bmem
#define _bmem

#include <stddef.h>
#include "slimits.h"
#include "matrix.h"

typedef struct MBLK_S    MBlk;
typedef struct BMEM_S    BMem;

/* uncomment this, if you want the original configuration 
   typedef int       BMemT;
   #define   BMemTNIL -1
   #define   BMemTMAX  INT_MAX 
*/

typedef UINT      BMemT;
#define BMemTNIL  UINT_MAX
#define BMemTMAX  UINT_MAX-1


struct MBLK_S {		/* The structure for a memory block            */
  BMemT	 itemF;		/* The first free entry in this block          */
  BMemT	 itemE;		/* The amount of free entries in this block    */
  void	*memP;		/* The pointer to the block in memory          */
  UCHAR	*occuP;		/* Fields to indicate which entry id free      */
};
  
struct BMEM_S {		/* Block Management                            */
  size_t	 size; 	/* The size of one entry                       */
  BMemT	 itemN;		/* The number of items in one block            */
  BMemT	 blkN;		/* number of blocks                            */
  MBlk	*blkA;		/* array of blocks                             */
  BMemT	 blkX;		/* block index for 'counter'                   */
  BMemT	 itemX;		/* item index for 'counter'                    */
    
  BMemT	 itemXN;	/* number of items stored in all memory blocks */
  BMemT	 itemXNIL;	/* return value for an invalid itemX           */
  BMemT	 itemXMAX;	/* maximum number of items to store            */

  struct {		/* Management of freed blocks                  */
    BMemT	 N;	/* number of blocks                            */
    BMemT*	 aN;	/* number of items in blockX                   */
    BMemT*	 aS;	/* item size       in blockX                   */
    void***	 aA;	/* ptr array       in blockX                   */
  } freeB;
};


/* =======================================================================
     Functions Prototypes
   ======================================================================= */

extern int        BMem_Init     (void);

extern BMem*      bmemCreate    (BMemT itemN, size_t size);
extern int        bmemFree      (BMem* bmemP);
extern void*      bmemItem      (BMem* bmemP);
extern void*      bmemAlloc     (BMem* bmemP, size_t n);
extern void       bmemDealloc   (BMem* bmemP, void* itemP, size_t n);
extern int        bmemDelete    (BMem* bmemP, void* itemP);
extern int        bmemCheck     (BMem* bmemP, void* itemP);
extern int        bmemClear     (BMem* bmemP);

extern int        bmemSetLimits (BMem *bmemP, BMemT itemXNIL, BMemT itemXMAX);
extern void*      bmemGetItem   (BMem *bmemP, BMemT itemX);
extern BMemT      bmemGetItemL  (BMem *bmemP, BMemT itemX);
extern BMemT      bmemGetItemX  (BMem *bmemP, void *itemP);
extern BMemT      bmemGetItemN  (BMem *bmemP);

extern FVector*   fvectorCreate_BMEM(int n, BMem* memP);
extern FMatrix*   fmatrixCreate_BMEM(int m,int n, BMem* memP);
extern DMatrix*   dmatrixCreate_BMEM(int m,int n, BMem* memP);


#ifdef USE_BMEM_SET
/* !!! these routines are currently not verified !!! */
extern BMemT      bmemSet       (BMem* bmemP, void* itemP);
extern void*      bmemNext      (BMem* bmemP);
#endif

#endif

