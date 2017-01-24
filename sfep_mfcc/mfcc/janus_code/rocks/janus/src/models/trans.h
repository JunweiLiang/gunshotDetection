/* ========================================================================
    JANUS      Speech- to Speech Translation
               ------------------------------------------------------------
               Object: Transition Models
               ------------------------------------------------------------
               
    Author  :  Monika Woszczyna
    Date    :  $Id: trans.h 700 2000-11-14 12:35:27Z janus $
    Remarks : 

 ==========================================================================
               
   This software was developed by
   The Interactive Systems Laboratories at

   the  University of Karlsruhe       and  Carnegie Mellon University
        Dept. of Informatics               Dept. of Computer Science
        ILKD, Lehrstuhl Prof.Waibel        Alex Waibel's NN & Speech Group
        Am Fasanengarten 5                 5000 Forbes Ave
        D-76131 Karlsruhe                  Pittsburgh, PA 15213
        - West Germany -                   - USA -

   Copyright (C) 1990-1995.   All rights reserved.

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
 ==========================================================================

   $Log$
   Revision 3.4  2000/11/14 12:35:26  janus
   Merging branch jtk-00-10-27-jmcd.

   Revision 3.3.34.1  2000/11/06 10:50:41  janus
   Made changes to enable compilation under -Wall.

   Revision 3.3  2000/08/27 15:31:30  jmcd
   Merging branch 'jtk-00-08-24-jmcd'.

   Revision 3.2.4.1  2000/08/25 22:19:45  jmcd
   Just checking.

   Revision 3.2  1997/06/17 17:41:43  westphal
   *** empty log message ***

    Revision 1.8  96/01/31  04:23:48  finkem
    changed read function to freadAdd function
    
    Revision 1.7  1995/10/29  23:54:36  finkem
    changed file format to janus3 style

    Revision 1.6  1995/08/27  22:35:05  finkem
    *** empty log message ***

    Revision 1.5  1995/08/17  18:01:35  rogina
    *** empty log message ***

    Revision 1.4  1995/08/15  16:16:28  rogina
    *** empty log message ***

    Revision 1.3  1995/08/15  16:14:58  rogina
    *** empty log message ***

    Revision 1.2  1995/07/31  14:34:34  finkem
    *** empty log message ***

    Revision 1.1  1995/07/25  13:49:57  finkem
    Initial revision


 =========================================================================== */

#ifdef __cplusplus
extern "C" {
#endif

#ifndef _trans
#define _trans

#include "list.h"

/* ========================================================================
    Tm
   ======================================================================== */

typedef struct
{
  char*  name;   /* name of this transition model */
  int    trN;    /* number of transisitions in this model */
  int*   offA;   /* array of state index offsets for every transisiton */
  float* penA;   /* array of penalties for every transisition */
}
Tm;

/* ========================================================================
    TmSet
   ======================================================================== */

typedef struct LIST(Tm) TmList;

typedef struct
{
  char*   name;      /* name of the file containing these models */
  int     useN;
  TmList  list;
  char    commentChar;
}
TmSet;

/* ========================================================================
    TmAccu
   ======================================================================== */

typedef struct
{
  int train;         /* selector for training algorithm and on/off    */
  TmSet *tms;        /* pointer to the TmSet this is for           */
  double **accu;     /* accu[tmX][trX] = probability for this jump    */
  double *count;     /* count[tmX] = how often was this tm-model used */
}
TmAccu;

/* ========================================================================
    Modules Interface
   ======================================================================== */

extern int tmInit(   Tm* tm, ClientData CD);
extern int tmDeinit( Tm* tm);

extern int    tmSetInit(   TmSet* tmSetP, ClientData CD);
extern TmSet* tmSetCreate( char*  name);
extern int    tmSetLinkN(  TmSet* tmSetP);
extern int    tmSetDeinit( TmSet* tmSetP);
extern int    tmSetFree(   TmSet* tmSetP);

extern int    tmSetIndex(  TmSet* tmSet, char* name);
extern char*  tmSetName(   TmSet* tmSet, int i);
extern int    tmSetNumber( TmSet* tmSet);

int tmGetItf(      char* value,           int**  pidx, float**  ppen);
int tmArrayGetItf( char* value, int** pn, int*** pidx, float*** ppen);

extern int Tm_Init();

#endif

#ifdef __cplusplus
}
#endif
