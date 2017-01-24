/* ========================================================================
    JANUS      Speech- to Speech Translation
               ------------------------------------------------------------
               Object: Distribution
               ------------------------------------------------------------

    Author  :  Michael Finke & Ivica Rogina
    Module  :  duration.h
    Date    :  $Id: duration.h 512 2000-08-27 15:31:32Z jmcd $
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
    Revision 3.4  2000/08/27 15:31:12  jmcd
    Merging branch 'jtk-00-08-24-jmcd'.

    Revision 3.3.4.1  2000/08/25 22:19:43  jmcd
    Just checking.

    Revision 3.3  1997/07/31 16:49:00  rogina
    made code gcc -DFKIclean

    Revision 3.2  1997/06/17 17:41:43  westphal
    *** empty log message ***

 * Revision 1.2  96/02/14  04:01:43  rogina
 * added distance stuff plus minor beautifications
 * 
 * Revision 1.1  1996/02/13  04:51:37  rogina
 * Initial revision
 *

   ======================================================================== */

#ifdef __cplusplus
extern "C" {
#endif

#ifndef _duration
#define _duration

#include "list.h"
#include "path.h"
#include "array.h"

/* ========================================================================
    Duration
   ======================================================================== */

typedef struct
{
  char         *name;   /* name of the duration model            */
  float         count;  /* total counts received in last epoch   */
  int           valN;   /* number of duration values             */
  float        *val;    /* vector of duration probabilities      */
  double       *accu;   /* training data accumulator             */

} Duration;

extern Duration* durCreate (int valN);
extern int       durInit   (Duration* dur, ClientData cd);
extern int       durDeinit (Duration* dur);

/* ========================================================================
    DurationSet
   ======================================================================== */

typedef struct LIST(Duration) DurationList;

typedef struct 
{
  char*          name;         /* name of the duration set            */
  int            useN;         /* number of references to this object */
  DurationList   list;

  IArray         map;
  float*         range;    /* -log length (# of durations) of a map range */
  int*           rangeLen; /* length of every single range, hist-indexed */
  int            valN;
  float          probFloor;
  float          minCount;
  float          momentum;
  char           commentChar;

} DurationSet;

/* ========================================================================
    Module Interface
   ======================================================================== */

extern int durAccuFree   (Duration *dur);
extern int durAccuClear  (Duration *dur);

extern int          durSetInit(   DurationSet *durSet, char* name, IArray *map);
extern DurationSet* durSetCreate( char* name, IArray *map);
extern int          durSetDeinit( DurationSet* durSet);
extern int          durSetFree(   DurationSet* durSet);

extern int   durSetAdd       (DurationSet* durSet, char* name, FArray *val, float count);
extern int   durSetRead      (DurationSet* durSet, char* fileName);
extern int   durSetWrite     (DurationSet* durSet, char* fileName);

extern int   durSetIndex     (DurationSet* durSet, char* name); 
extern char* durSetName      (DurationSet* durSet, int i); 

extern float durSetScore     (DurationSet* durSet, int dsX,  int frameX);

/*extern int   durSetAccu      (DurationSet *durSet, Path *path,  HMM *hmm, float factor);*/

extern int   durSetUpdate    (DurationSet *durSet);

extern int Duration_Init (void);

#define durScore(durSet,durX,dur) \
        ((dur >= durSet->map.itemN) ? -1.0*log(durSet->probFloor) \
        : durSet->list.itemA[durX].val[durSet->map.itemA[dur]] - durSet->range[dur])

#endif

#ifdef __cplusplus
}
#endif
