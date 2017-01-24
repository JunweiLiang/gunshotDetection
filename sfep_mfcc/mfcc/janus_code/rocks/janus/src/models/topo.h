/* ========================================================================
    JANUS      Speech- to Speech Translation
               ------------------------------------------------------------
               Object: Topology
               ------------------------------------------------------------

    Author  :  Michael Finke
    Module  :  topo.h
    Date    :  $Id: topo.h 700 2000-11-14 12:35:27Z janus $
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

    Revision 2.2  96/02/14  16:27:44  rogina
    removed duration stuff (It's hanging in amodel now>)
    
    Revision 2.1  1996/02/13  04:52:35  rogina
    added duration model index

    Revision 2.0  1996/01/31  04:34:38  finkem
    New SenoneSet setup & freadAdd function

    Revision 1.3  1995/08/27  22:35:35  finkem
    *** empty log message ***

    Revision 1.2  1995/08/01  13:27:21  finkem
    *** empty log message ***

    Revision 1.1  1995/07/31  14:33:49  finkem
    Initial revision

 
   ======================================================================== */

#ifdef __cplusplus
extern "C" {
#endif

#ifndef _topo
#define _topo

#include "list.h"
#include "trans.h"
#include "senones.h"
#include "array.h"
#include "duration.h"

/* ========================================================================
    Topo
   ======================================================================== */

typedef struct
{
  char*   name;   /* name of the topology                  */
  int*    stnXA;  /* senone tree node                      */
  int*    tmXA;   /* index of the transition model         */
  int     tmN;    /* number of transition models           */

} Topo;

/* ========================================================================
    TopoSet
   ======================================================================== */

typedef struct LIST(Topo) TopoList;

typedef struct Topo_S
{
  char*        name;          /* name of this topology set object      */
  int          useN;

  TopoList     list;

  SenoneSet*   snsP;
  TmSet*       tmSetP;
  char         commentChar;   /* a character to itentify comment lines */

} TopoSet;

/* ========================================================================
    Module Interface
   ======================================================================== */

extern int topoInit(    Topo* TP, char* name);
extern int topoDeinit(  Topo* TP);
extern int topoSetInit( TopoSet*   topoSet, char*  name, 
                        SenoneSet* snsP, TmSet* tmSetP);

extern TopoSet *topoSetCreate( char *name, SenoneSet* snsP, TmSet* tmSetP);

extern int topoSetLinkN(  TopoSet* topoSet);
extern int topoSetDeinit( TopoSet* topoSet);
extern int topoSetFree(   TopoSet* topoSet);

extern int topoSetIndex(  TopoSet* topoSet, char* name); 
extern char* topoSetName( TopoSet* topoSet, int i); 
extern int topoSetNumber( TopoSet* topoSet);

extern int topoSetAdd(    TopoSet* topoSet, char* name, 
                          int* stnXA, int* tmXA, int tmN);
extern int topoSetWrite(  TopoSet *topoSet, char *filename);

extern SenoneSet* topoSetSenoneSet( TopoSet* topoSet);
extern TmSet*     topoSetTmSet(     TopoSet* topoSet);

extern int        Topo_Init ();

#endif

#ifdef __cplusplus
}
#endif
