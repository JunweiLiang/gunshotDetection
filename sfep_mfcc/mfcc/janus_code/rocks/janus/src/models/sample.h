/* ========================================================================
    JANUS      Speech- to Speech Translation
               ------------------------------------------------------------
               Object: SampleSet
               ------------------------------------------------------------

    Author  :  Ivica Rogina
    Module  :  SampleSet.h
    Date    :  $Id: sample.h 2390 2003-08-14 11:20:32Z fuegen $
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
    Revision 3.7  2003/08/14 11:20:17  fuegen
    Merged changes on branch jtk-01-01-15-fms (jaguar -> ibis-013)

    Revision 3.6.20.1  2001/10/18 13:09:51  metze
    Possibility to have different features for different sample sets

    Revision 3.6  2000/11/14 12:35:25  janus
    Merging branch jtk-00-10-27-jmcd.

    Revision 3.5.34.1  2000/11/06 10:50:39  janus
    Made changes to enable compilation under -Wall.

    Revision 3.5  2000/08/27 15:31:28  jmcd
    Merging branch 'jtk-00-08-24-jmcd'.

    Revision 3.4.4.1  2000/08/25 22:19:45  jmcd
    Just checking.

    Revision 3.4  2000/04/20 07:47:39  soltau
    added size to sampleSetClass to allocate sample buffer on demand

    Revision 3.3  1997/07/16 11:38:06  rogina
    added likelihood-distance stuff

    Revision 3.2  1997/06/17 17:41:43  westphal
    *** empty log message ***

 * Revision 1.3  96/01/19  21:21:15  rogina
 * added allCount (made modulus work)
 * 
 * Revision 1.2  96/01/19  13:52:11  13:52:11  rogina (Ivica Rogina)
 * added maxCount
 * 
 * Revision 1.1  96/01/10  20:06:24  20:06:24  rogina (Ivica Rogina)
 * Initial revision
 * 
 
   ======================================================================== */

#ifdef __cplusplus
extern "C" {
#endif

#ifndef _SampleSet
#define _SampleSet

#include "feature.h"  /* includes matrix.h (itf.h(tcl.h), common.h) */
#include "list.h"     /* includes itf.h(tcl.h), common.h */
#include "distrib.h"
#include "lh.h"

/* ========================================================================
    SampleSetClass
   ======================================================================== */

typedef struct SampleSetClass_S
{
  char*   name;
  int     useN;

  char    *fileName;   /* where to flush the buffer when it's full */
  FMatrix *buf;        /* here are the sample vectors (incl. gammas) */
  Lh      *lh;         /* for liklihood value accumulation */
  int      modulus;    /* accept only every n-th available vector */
  int      sumCount;   /* overall number of accepted vectors */
  int      maxCount;   /* maximum overall number of accepted vectors */
  int      allCount;   /* overall number of offered vectors */
  int      count;      /* fill counter = how much of the buffer is used */
  int      size;
  int      featX;     
  int      dimN;
} SampleSetClass;

extern int             sampleSetClassInit   (SampleSetClass* SampleSet, ClientData CD);
extern SampleSetClass* sampleSetClassCreate (char* name);
extern int             sampleSetClassDeinit (SampleSetClass* cP);
extern int             sampleSetClassFree   (SampleSetClass* cP);

/* ========================================================================
    SampleSet
   ======================================================================== */

typedef struct LIST(SampleSetClass) SampleSetClassList;

typedef struct SampleSet
{
  char*        name;        /* name of the SampleSet list                  */
  int          useN;        /* used by another object                      */

  SampleSetClassList list;  /* list of SampleSetClasses                    */

  FeatureSet*  fesP;        /* pointer to the underlying feature set       */
  int          featX;       /* index of the actual feature                 */
  int          dimN;        /* dimension of the input space                */

  int*         indexA;      /* index map which helps to translate incoming */
  int          indexN;      /* indices to class indices used in the list   */

} SampleSet;

extern int  sampleSetInit(   SampleSet *SampleSetP, char *name, FeatureSet *fs, int featX, int dimN);
extern SampleSet *sampleSetCreate (           char *name, FeatureSet *fs, int featX, int dimN);

extern int   sampleSetLinkN(  SampleSet* SampleSetP);
extern int   sampleSetDeinit( SampleSet* SampleSetP);
extern int   sampleSetFree(   SampleSet* SampleSetP);
extern int   sampleSetAdd(    SampleSet* SampleSetP, char *name, char *fileName, int featX, int dimN, int size, int modulus, DistribSet *lh);
extern int   sampleSetIndex(  SampleSet* SampleSetP, char *name);
extern char *sampleSetName(   SampleSet* SampleSetP, int i);
extern int   sampleSetNumber( SampleSet* SampleSetP);
extern int   sampleSetMap(    SampleSet* SampleSetP, int idx, int classX);

extern int   SampleSet_Init();

#endif

#ifdef __cplusplus
}
#endif
