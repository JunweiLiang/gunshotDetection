/* ========================================================================
    JANUS      Speech- to Speech Translation
               ------------------------------------------------------------
               Object: Int/Float Arrays
               ------------------------------------------------------------

    Author  :  Michael Finke
    Module  :  array.h
    Date    :  $Id: array.h 512 2000-08-27 15:31:32Z jmcd $
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
    Revision 3.4  2000/08/27 15:31:07  jmcd
    Merging branch 'jtk-00-08-24-jmcd'.

    Revision 3.3.4.1  2000/08/25 22:19:42  jmcd
    Just checking.

    Revision 3.3  1997/07/21 16:03:34  tschaaf
    gcc / DFKI Clean

    Revision 3.2  1997/06/17 17:41:43  westphal
    *** empty log message ***

    Revision 1.1  95/07/25  13:49:57  finkem
    Initial revision
    
 
   ======================================================================== */

#ifdef __cplusplus
extern "C" {
#endif

#ifndef _array
#define _array

#include "list.h"

/* ========================================================================
    Integer Array
   ======================================================================== */

typedef struct
{
  int* itemA;
  int  itemN;

} IArray;

/* ========================================================================
    Float Array
   ======================================================================== */

typedef struct
{
  float* itemA;
  int    itemN;

} FArray;

/* ========================================================================
    Module Interface
   ======================================================================== */
extern int Array_Init ( void );

extern int iarrayInit(       IArray* IA, int n);
extern int iarrayDeinit(     IArray* IA);
extern int iarrayGetItf(     IArray* IA, char* value);
extern int iarrayGetListItf( IArray* IA, char* value, List* L);

extern int farrayInit(       FArray* IA, int n);
extern int farrayDeinit(     FArray* IA);
extern int farrayGetItf(     FArray* IA, char* value);

#endif

#ifdef __cplusplus
}
#endif
