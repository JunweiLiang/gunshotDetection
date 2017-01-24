/* ========================================================================
    JANUS      Speech- to Speech Translation
               ------------------------------------------------------------
               Object: Cross-Word Acoustic Models
               ------------------------------------------------------------

    Author  :  Michael Finke
    Module  :  xwmodel.h
    Date    :  $Id: xwmodel.h 512 2000-08-27 15:31:32Z jmcd $
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
    Revision 3.4  2000/08/27 15:31:32  jmcd
    Merging branch 'jtk-00-08-24-jmcd'.

    Revision 3.3.4.1  2000/08/25 22:19:46  jmcd
    Just checking.

    Revision 3.3  1997/07/18 17:57:51  monika
    gcc-clean

    Revision 3.2  1997/06/17 17:41:43  westphal
    *** empty log message ***

    Revision 1.1  95/08/03  14:40:51  finkem
    Initial revision
    
 
   ======================================================================== */

#ifdef __cplusplus
extern "C" {
#endif

#ifndef _xwmodel
#define _xwmodel

#include "list.h"
#include "amodel.h"

/* ========================================================================
    XWord Model
   ======================================================================== */

typedef struct
{
  int* fullXA;
  int  fullN;
  int* relXA;
  int* uniqXA;
  int  uniqN;

} XWModel;

/* ========================================================================
    Set of XWord Models
   ======================================================================== */

typedef struct LIST(XWModel) XWModelList;

typedef struct
{
  char*         name;
  int           useN;
  XWModelList   list;

  AModelSet*    amodelP;

} XWModelSet;

/* ========================================================================
    Module Interface
   ======================================================================== */

extern int XWModel_Init(void);

extern XWModelSet* xwmodelSetCreate( char *name, AModelSet* amodelP);

extern int xwmodelSetAdd( XWModelSet* xwmodelSet, int fullN, int *fullXA);

#endif

#ifdef __cplusplus
}
#endif
