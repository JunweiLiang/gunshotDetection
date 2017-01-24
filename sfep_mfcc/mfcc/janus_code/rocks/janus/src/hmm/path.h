/* ========================================================================
    JANUS      Speech- to Speech Translation
               ------------------------------------------------------------
               Object: Path Maintainance Functions
               ------------------------------------------------------------

    Author  :  Michael Finke & Ivica Rogina
    Module  :  path.c
    Date    :  $Id: path.h 2759 2007-02-21 17:03:36Z fuegen $
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
    Revision 3.9  2007/02/21 17:03:35  fuegen
    code cleaning for windows from Roger and Christian

    Revision 3.8  2006/11/10 10:15:39  fuegen
    merged changes from branch jtk-05-02-02-shajith
    - all modification related to MMIE training
    - first unverified functions for MPE training
    - modifications made by Shajith, Roger, Wilson, and Sebastian

    Revision 3.7.12.1  2006/11/03 12:24:09  stueker
    Initial check-in of the MMIE from Shajith, Wilson, and Roger

    Revision 3.7  2003/08/14 11:19:57  fuegen
    Merged changes on branch jtk-01-01-15-fms (jaguar -> ibis-013)

    Revision 3.6.20.1  2003/02/05 09:02:01  soltau
    code cleaning

    Revision 3.4  2000/02/08 09:00:54  soltau
    Added labelboxes

    Revision 3.3  1997/07/18 16:48:59  tschaaf
    gcc / DFKI -clean

    Revision 3.2  1997/06/17 17:41:43  westphal    
    *** empty log message ***

    Revision 1.10  1996/03/13  21:56:51  finkem
    added scor to path.itemList as best path score in viterbi

    Revision 1.9  1995/12/06  02:27:31  finkem
    add senoneMatrix output

    Revision 1.8  1995/11/17  22:25:42  finkem
    forwardBackward

    Revision 1.7  1995/11/14  06:05:14  finkem
    changed path structure to make it itf accessible

    Revision 1.6  1995/10/18  08:51:44  rogina
    guided alignment variables

    Revision 1.5  1995/10/16  15:53:37  finkem
    made it work with new hmm.c

 
   ======================================================================== */

#ifndef _path_h
#define _path_h

#include "matrix.h"
#include "list.h"
#include "senones.h"

/* ========================================================================
    PathItem
   ======================================================================== */

typedef struct
{
  int   stateX;
  int   senoneX;
  int   phoneX;
  int   wordX;

  float alpha;         /* the forward alpha probability     */
  float beta;          /* the backward beta probability     */
  float gamma;         /* the forw/backw gamma probability  */
  float lscore;        /* the local acoustic score          */

} PathItem;

extern int pathItemInit(   PathItem* pi);
extern int pathItemDeinit( PathItem* pi);

/* ========================================================================
    PathItemList
   ======================================================================== */

typedef struct
{
  PathItem* itemA;
  int       itemN;

  double    logScale;
  float     beam;
  float     score;
  float     best;

} PathItemList;

extern PathItemList *pathItemListCreate( int n);

extern int pathItemListInit(   PathItemList* pil);
extern int pathItemListFree(   PathItemList* pil);
extern int pathItemListDeinit( PathItemList* pil);
extern int pathItemListAdd(    PathItemList* pil, int n,
                               int stateX, int senoneX, int phoneX, int wordX);
extern int pathItemListRemove( PathItemList* pil);

/* ========================================================================
    Path
   ======================================================================== */

typedef struct
{
  char    *name;           /* name of this path object */
  int     useN;            /* how many other objects use this one */

  int           firstFrame, 
                lastFrame,       /* real index of the first frame and the last frame in this path */
                fromFrame,       /* number of leading frames in the signal that were left out */
                frameShift;      /* duration of a frame in miliseconds */

  int           pitemListN;
  PathItemList* pitemListA;
  float         gscore;

  float         senoneMissPen;   /* penalty for missing a senone during alignment */
  float         phoneMissPen;    /* penalty for missing a phone  during alignment */
  float         wordMissPen;     /* penalty for missing a word   during alignment */

} Path;

/* ========================================================================
    Labelbox
   ======================================================================== */

typedef struct LIST(Path) PathList;

typedef struct
{
  char    *name;  
  int     useN;     
  PathList list;
} Labelbox;

/* ========================================================================
    Module Interface
   ======================================================================== */

int Path_Init (void);
int pathItemListClear( PathItemList* pil);

extern int   pathInit(   Path* path, ClientData CD);
extern Path* pathCreate( char* name);
extern int   pathReset(  Path* path);
extern int   pathDeinit( Path* path);
extern int   pathFree(   Path* path);

extern int pathStateMatrix(  Path* path, int first, int last, 
                                         int from,  int to, FMatrix* fmP);
extern int pathSenoneMatrix( Path* path, int first, int last, 
                                         int from,  int to, FMatrix* fmP);
extern int pathPhoneMatrix(  Path* path, int first, int last,
                                         int from,  int to, FMatrix* fmP);
extern int pathWordMatrix(   Path* path, int first, int last,
                                         int from,  int to, FMatrix* fmP);

extern int Path_Init (void);

extern int pathCopy(Path* from, Path* to);

#endif
