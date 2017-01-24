/* ========================================================================
    JANUS      Speech- to Speech Translation
               ------------------------------------------------------------
               Object: Score
               ------------------------------------------------------------

    Author  :  Thomas Kemp
    Module  :  score.h
    Date    :  $Id: score.h 2718 2006-11-02 15:25:14Z fuegen $
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
    Revision 3.6  2006/11/02 15:25:14  fuegen
    changes for Windows compilation

    Revision 3.5  2005/04/25 13:25:23  metze
    cbcClear works (max be slow, needed for MMIE)

    Revision 3.4  2003/08/14 11:20:17  fuegen
    Merged changes on branch jtk-01-01-15-fms (jaguar -> ibis-013)

    Revision 3.3.56.1  2003/02/05 08:57:04  soltau
    Added cbcClear

    Revision 3.2  1997/06/17 17:41:43  westphal
    *** empty log message ***

 * Revision 1.6  96/10/08  09:16:05  fritsch
 * added header for scScoreTopNBbi()
 * 
 * Revision 1.5  1996/09/26  17:20:04  fritsch
 * Replaced header for scScoreTopBbi() with two new headers
 *
 * Revision 1.4  1995/11/15  04:08:37  finkem
 * Added BBI score computation
 *
 * Revision 1.3  1995/09/23  18:17:23  finkem
 * removed some allocation problems
 *
 * Revision 1.2  1995/08/31  08:29:10  kemp
 * *** empty log message ***
 *
 * Revision 1.1  1995/08/29  13:15:19  kemp
 * Initial revision
 *
 
   ======================================================================== */

#ifndef _score
#define _score

#include "common.h"
#include "codebook.h"

/* ========================================================================
    Codebook Cache
   ======================================================================== */

typedef struct CbCache_S
{
  char*        name;      /* name of the codebook list                      */
  int          useN;      /* used by another object                         */

  int          cbN;       /* for how many codebooks is cache allocated      */
  int          distCacheN;

  int*      distFrameX;/* physical frame indices of the cache entries for    */
                       /* the cb vector distance cache                       */
  float***  logdist;   /* logdist[cacheX][cbX][refX]                         */
  short***  bestIndex; /* The physical frame index belonging to 'cacheX' can */
                       /* be found in the distFrameX array.                  */
                       /* At the same time, this tells whether the cache is  */
                       /* filled or not, if not, it is cacheEmptyIndicator   */

  short**   usedRefN;  /* the cache may be only partially filled (in case we */
                       /* do top-N vector calculations or bucket intersection*/
                       /* or something). This field tells for each codebook  */
                       /* how many entries in the logdist and bestIndex arrs */
                       /* are valid and set.                                 */
                       /* The two indices mean usedRefN[cacheX][cbX].        */

  int*      refN;      /* refN[cbX] tells how many ref vectors each cb has   */
  int       totalRefN; /* sum of refN array: equivalent to total cache size  */

  CodebookSet* cbsP;

} CbCache;

/* ========================================================================
    Module Interface
   ======================================================================== */

extern int cbcAlloc( CbCache* cbcP);
extern int cbcInit(  CbCache* cbcP, char* name, CodebookSet* cbsP, 
                                                       int distCacheN);

extern CbCache* cbcCreate( char* name, CodebookSet* cbsP, 
                                       int distCacheN);

extern int      cbcDealloc(    CbCache* cbcP);
extern int      cbcDeinit(     CbCache* cbcP);
extern int      cbcFree(       CbCache* cbcP);
extern int      cbcClear(      CbCache* cbcP, int cacheN);
extern int      cbcMarkUnused( CbCache* cbcP, int cacheX);
extern int      cbcFillEmptyIndicator();

extern float    scScoreTopAll();
extern float    scScoreTopAllBbi (CodebookSet *cbsP, int cbX, float *dsVal,
				  int frameX, float *mixVal);
extern float    scScoreTopOneBbi (CodebookSet *cbsP, int cbX, float *dsVal,
				  int frameX, float *mixVal);
extern float    scScoreTopNBbi   (CodebookSet *cbsP, int cbX, float *dsVal,
			          int frameX, float *mixVal);

#endif

