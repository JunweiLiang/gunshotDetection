/* ========================================================================
    JANUS      Speech- to Speech Translation
               ------------------------------------------------------------
               Object: LDA
               ------------------------------------------------------------

    Author  :  Michael Finke & Martin Maier
    Module  :  lda.h
    Date    :  $Id: lda.h 512 2000-08-27 15:31:32Z jmcd $
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
    Revision 3.3  2000/08/27 15:31:21  jmcd
    Merging branch 'jtk-00-08-24-jmcd'.

    Revision 3.2.4.1  2000/08/25 22:19:44  jmcd
    Just checking.

    Revision 3.2  1997/06/17 17:41:43  westphal
    *** empty log message ***

    Revision 1.3  96/02/03  17:12:54  finkem
    made LDA a one pass procedure where means and scatter matrices
    are computed in parallel. Use update to generate the final scatter
    matrices.
    
    Revision 1.2  1995/12/14  08:55:04  maier
    *** empty log message ***

    Revision 1.1  1995/12/06  02:36:28  finkem
    Initial revision

 
   ======================================================================== */

#ifdef __cplusplus
extern "C" {
#endif

#ifndef _LDA
#define _LDA

#include "feature.h"  /* includes matrix.h (itf.h(tcl.h), common.h) */
#include "list.h"     /* includes itf.h(tcl.h), common.h */

/* ========================================================================
    LDAClass
   ======================================================================== */

typedef struct LDAClass_S
{
  char*    name;
  int      useN;

  DVector* mean;      /* vector to hold class mean */
  DVector* x;

} LDAClass;

extern int       ldaClassInit(   LDAClass* lda, ClientData CD);
extern LDAClass* ldaClassCreate( char* name);
extern int       ldaClassDeinit( LDAClass* cP);
extern int       ldaClassFree(   LDAClass* cP);

/* ========================================================================
    LDA
   ======================================================================== */

typedef struct LIST(LDAClass) LDAClassList;

typedef struct LDA
{
  char*        name;        /* name of the LDA list                        */
  int          useN;        /* used by another object                      */

  LDAClassList list;        /* list of LDAClasses                          */
  DVector*     mean;        /* vector to hold total mean                   */
  DMatrix*     t;           /* matrix to hold total scatter                */
  DMatrix*     w;           /* matrix to hold within scatter               */
  DMatrix*     s;           /* matrix to hold \sum_p x_p x_p'              */

  FeatureSet*  fesP;        /* pointer to the underlying feature set       */
  int          featX;       /* index of the actual feature                 */
  int          dimN;        /* dimension of the input space                */

  int*         indexA;      /* index map which helps to translate incoming */
  int          indexN;      /* indices to class indices used in the list   */

} LDA;

extern int  ldaInit(   LDA* ldaP, char* name, FeatureSet* fs, int featX, 
                                                              int dimN);
extern LDA* ldaCreate (           char* name, FeatureSet* fs, int featX,
                                                              int dimN);
extern int   ldaLinkN(  LDA* ldaP);
extern int   ldaDeinit( LDA* ldaP);
extern int   ldaFree(   LDA* ldaP);
extern int   ldaAdd(    LDA* ldaP, char* name);
extern int   ldaIndex(  LDA* ldaP, char* name);
extern char* ldaName(   LDA* ldaP, int i);
extern int   ldaNumber( LDA* ldaP);
extern int   ldaMap(    LDA* ldaP, int idx, int classX);

#endif



#ifdef __cplusplus
}
#endif
