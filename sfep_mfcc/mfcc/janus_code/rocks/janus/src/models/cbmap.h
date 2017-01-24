/* ========================================================================
    JANUS      Speech- to Speech Translation
               ------------------------------------------------------------
               Object: Codebook
               ------------------------------------------------------------

    Author  :  Michael Finke & Ivica Rogina
    Module  :  codebook.c
    Date    :  $Id: cbmap.h 512 2000-08-27 15:31:32Z jmcd $
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
    Revision 3.4  2000/08/27 15:31:08  jmcd
    Merging branch 'jtk-00-08-24-jmcd'.

    Revision 3.3.4.1  2000/08/25 22:19:42  jmcd
    Just checking.

    Revision 3.3  1997/07/30 13:40:47  rogina
    made code gcc -DFKIclean

    Revision 3.2  1997/06/17 17:41:43  westphal
    *** empty log message ***

 * Revision 1.1  95/12/27  16:03:28  rogina
 * Initial revision
 * 

   ======================================================================== */

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {

  int   from;        /* map reference "from"...              */
  int   to;          /* ... to new reference "to"            */
  int   subX;        /* get subaccumulator subX              */

  float alpha0;      /* codebook: rescale "to" entry and add */
  float alpha;       /*           alpha scaled "from" one    */

  float beta0;       /* mixture:  rescale "to" entry and add */
  float beta;        /*           beta scaled "from" one     */

} CodebookMapItem;


typedef struct {

  int              itemN;
  CodebookMapItem* itemA;
  
} CodebookMap;

extern int cbMapItemInit(   CodebookMapItem* cbmiP);
extern int cbMapItemDeinit( CodebookMapItem* cbmiP);

extern CodebookMap* cbMapCreate( int n);

extern int cbMapInit(   CodebookMap* cbmP);
extern int cbMapDeinit( CodebookMap* cbmP);
extern int cbMapFree(   CodebookMap* cbmP);
extern int cbMapAdd(    CodebookMap* cbmP, int n, int idxX, int idxY);
extern int cbMapClear(  CodebookMap* cbmP);
extern int Cbmap_Init(void);

#ifdef __cplusplus
}
#endif
