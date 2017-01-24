/* ========================================================================
    JANUS      Speech- to Speech Translation
               ------------------------------------------------------------
               Object: Distribution
               ------------------------------------------------------------

    Author  :  Michael Finke & Ivica Rogina
    Module  :  distrib.h
    Date    :  $Id: distrib.h 2555 2004-10-27 11:43:41Z metze $
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
    Revision 4.7  2004/10/27 11:43:41  metze
    Fixed '-bmem' issues with cbs/dss

    Revision 4.6  2003/08/14 11:20:14  fuegen
    Merged changes on branch jtk-01-01-15-fms (jaguar -> ibis-013)

    Revision 4.5.4.3  2003/02/05 08:58:11  soltau
    Added subIndex

    Revision 4.5.4.2  2002/03/18 17:42:34  soltau
    cleaned pre declarations

    Revision 4.5.4.1  2002/03/13 10:06:11  soltau
    Added bmem support

    Revision 4.2  2000/01/12 10:02:02  fuegen
    add normalisation for distances
    add verbosity for clustering
    faster distance calculation for entropy

    Revision 4.1  1998/12/10 21:04:37  fuegen
    *** empty log message ***

    Revision 4.0  1998/11/30 18:07:14  fuegen
    *** empty log message ***

    Revision 3.5  1998/08/12 10:41:37  kemp
    added declaration for dsKullbackLeibler

 * Revision 3.4  1997/07/31  16:24:54  rogina
 * made code gcc -DFKIclean
 *
    Revision 3.3  1997/07/16 11:37:51  rogina
    added likelihood-distance stuff

    Revision 3.2  1997/06/17 17:41:43  westphal
    *** empty log message ***

    Revision 2.1  96/02/11  06:17:13  finkem
    changed distance function to take 3 model arrays
    
    Revision 2.0  1996/01/31  08:45:04  finkem
    new senoneSet setup and no loger scoreComputer

    Revision 1.15  1996/01/03  17:52:22  rogina
    added Cbcfg

    Revision 1.14  1995/12/27  15:31:44  rogina
    eliminated vscore

    Revision 1.13  1995/11/30  15:06:11  finkem
    CodebookMap revised and made working

    Revision 1.12  1995/11/25  20:26:40  finkem
    removed a couple of problems with the data structures of the
    accumulator (pointing back to distributions)

    Revision 1.11  1995/11/15  04:08:37  finkem
    Added BBI score computation

    Revision 1.10  1995/11/14  06:05:14  finkem
    splitting eighting matrix instead of a vector

    Revision 1.9  1995/10/25  16:54:16  rogina
    *** empty log message ***

    Revision 1.8  1995/10/18  23:21:07  finkem
    cleaning up / introducing shift & mincount

    Revision 1.7  1995/09/21  21:39:43  finkem
    Init?Deinit and default arguments changed

    Revision 1.6  1995/09/12  18:01:52  rogina
    *** empty log message ***

    Revision 1.5  1995/08/28  16:20:11  rogina
    *** empty log message ***

    Revision 1.4  1995/08/28  16:10:46  rogina
    *** empty log message ***

    Revision 1.3  1995/08/10  08:11:16  rogina
    *** empty log message ***

    Revision 1.2  1995/08/04  14:18:25  rogina
    *** empty log message ***

    Revision 1.1  1995/07/25  13:49:57  finkem
    Initial revision

 
   ======================================================================== */

#ifndef _distrib
#define _distrib

#include "list.h"
#include "codebook.h"
#include "lh.h"
#include "modalities.h"
#include "bmem.h"

/* ========================================================================
    Distrib Configuration
   ======================================================================== */

typedef struct {
  char*     name;
  int       useN;
  float     momentum;     /* how much of the old parameters to preserve    */
  float     shift;        /* will be added to every accum before update    */
  float     minCount;     /* don't update if not enough training counts    */
  float     floor;        /* forced floor value for all accumulator counts */
  char      doAccumulate; /* 'y' = do accumulate, 'n' = don't accumulate   */
  char      doUpdate;     /* 'y' = do update, 'n' = don't update           */
  char      method;       /* 'm' = maximum likelihood, '...' other         */ 
} Dscfg;

/* ========================================================================
    Distrib Accumulator
   ======================================================================== */

typedef struct DistribAccu {
  struct Distrib_S* ds;       /* for which distribution are we accumulating */
  int               subN;     /* how many subaccumulators are we using      */
  double          **count;    /* a count for each distribution value        */
  Lh              **lh;       /* Gaussian likelihood accumulators           */  
} DistribAccu;


/* ========================================================================
    Distrib
   ======================================================================== */

typedef struct Distrib_S {
  char*        name;
  int          useN;
  float*       val;    /* vector of distribution values         */
  int          valN;   /* number of values in this distribution */
  float        count;  /* total counts received in last epoch   */
  int          cbX;    /* index of underlying codebook          */
  DistribAccu* accu;   /* training data accumulator             */
  Dscfg        *cfg;   /* struct holding config variables       */
} Distrib;


/* ========================================================================
    DistribSet
   ======================================================================== */

typedef struct LIST(Distrib) DistribList;

typedef struct {
  char*             name;       
  int               useN;       

  DistribList       list;
  float             minCount;     /* minCount used for distance measure  */
  CodebookSet*      cbsP;
  RewriteSet*       rwsP;
  char              distance;     /* select distance measure to be used */
  char              commentChar;

  int               dummyStart;   /* start index of dummy distributions */
  char             *dummyName;    /* name for dummy-ds identification */
  StateTable*       stP;

  int               normDistance; /* normalize the entropy distance to reduce
				     count influence */

  int               subIndex;     /* global sub-accumulator index */

} DistribSet;

/* ========================================================================
    Module Interface
   ======================================================================== */


extern int   dssAdd       (DistribSet* dssP, char* name, int cbX);
extern int   dssIndex     (DistribSet* dss, char* name); 
extern char* dssName      (DistribSet* dss, int i); 

extern float dssScore     (DistribSet* dss, int dsX,  int frameX);
extern int   dssScoreItf  (ClientData  cd,  int argc, char *argv[]);
extern float dssScoreArray(DistribSet* dssP, int* dsA, float* scoreA, int  dsN, int frameX);

extern int   dssAccu      (DistribSet* dss, int dsX,  int frameX, float factor);
extern int   dssAccuItf   (ClientData  cd,  int argc, char *argv[]);

extern int   dssUpdate    (DistribSet* dss);
extern int   dssUpdateItf (ClientData  cd,  int argc, char *argv[]);

extern int  Distrib_Init (void);

#endif
