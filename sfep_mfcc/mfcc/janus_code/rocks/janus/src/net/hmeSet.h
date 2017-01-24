/* ========================================================================
    JANUS      Speech- to Speech Translation
               ------------------------------------------------------------
               Object: Set of Hme's for acoustic modeling
               ------------------------------------------------------------

    Author  :  Juergen Fritsch
    Module  :  hmeSet.h
    Date    :  $Id: hmeSet.h 512 2000-08-27 15:31:32Z jmcd $
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
    Revision 3.3  2000/08/27 15:31:16  jmcd
    Merging branch 'jtk-00-08-24-jmcd'.

    Revision 3.2.4.1  2000/08/25 22:19:44  jmcd
    Just checking.

    Revision 3.2  1997/06/17 17:41:43  westphal
    *** empty log message ***

 * Revision 1.1  96/09/27  08:59:05  fritsch
 * Initial revision
 * 

   ======================================================================== */

#ifdef __cplusplus
extern "C" {
#endif

#ifndef _hmeSet
#define _hmeSet

#include "hme.h"



/* ======================================================================== */
/*  HmeModelItem                                                            */
/* ======================================================================== */

typedef struct {

  char        *name;              /* model name */
  int         tagIdx;             /* tag index */
  int         phoneIdx;           /* monophone index */
  int         ctxIdx;             /* context index */

} HmeModelItem;



/* ======================================================================== */
/*  HmeModelList                                                            */
/* ======================================================================== */

typedef struct LIST(HmeModelItem) HmeModelList;



/* ======================================================================== */
/*  HmeTagMapItem                                                           */
/* ======================================================================== */

typedef struct {

  char        *name;              /* tag name */
  int         index;              /* mapped tag index */

} HmeTagMapItem;



/* ======================================================================== */
/*  HmeTagMap                                                               */
/* ======================================================================== */

typedef struct LIST(HmeTagMapItem) HmeTagMap;



/* ======================================================================== */
/*  HmeStateSet                                                             */
/* ======================================================================== */

typedef struct {

  char           *name;           /* state name (or 'tag') */
  int            phonesN;         /* number of phones */

  Hme            *mono;           /* monophone Hme */
  int            monoActive;      /* active flag for monophone Hme */

  Hme            **ctx;           /* context Hme's */
  double         *smooth;         /* context smoothing factors */
  int            *active;         /* active Hme's flags */
  
} HmeStateSet;



/* ======================================================================== */
/*  HmeSet                                                                  */
/* ======================================================================== */

typedef struct {

  char           *name;          /* name of object */
  
  int            stateN;         /* number of states */
  Phones         *phones;        /* pointer to phone set */

  FeatureSet     *fes;           /* pointer to a FeatureSet */
  int            gateFeatIdx;    /* index of the feature for gates */
  int            expFeatIdx;     /* index of the feature for experts */

  HmeModelList   list;           /* model mapping list */
  HmeTagMap      tagMap;         /* tag mapping list */
  
  Hme            *sdh;           /* pointer to state discriminating Hme */
  int            sdhActive;      /* active flag for state discrim. Hme */

  HmeStateSet    **state;        /* array of pointers to state sets */

  int            divPrior;       /* divide by priors ? */
  int            monoOnly;       /* use context independent Hme's only ? */
  double         globSmooth;     /* global CD smoother */
  double         scoreMul;       /* Hme score multiplicator */

} HmeSet;






int         hmeModelItemPutsItf  (ClientData cd, int argc, char *argv[]);
int         hmeSetTagIndex       (HmeSet *hmeSet, char *tag);
int         hmeSetIndex          (ClientData cd, char *name);
char        *hmeSetName          (ClientData cd, int index);
int         hmeSetConfHmes       (HmeSet *hmeSet, int argc, char *argv[]);
int         hmeSetConfGates      (HmeSet *hmeSet, int argc, char *argv[]);
int         hmeSetConfExperts    (HmeSet *hmeSet, int argc, char *argv[]);
float       hmeSetScore          (HmeSet *hmeSet, int idx, int frameX);
int         hmeSetCreateAccus    (HmeSet *hmeSet);
int         hmeSetFreeAccus      (HmeSet *hmeSet);
int         hmeSetClearAccus     (HmeSet *hmeSet);
int         hmeSetSaveAccus      (HmeSet *hmeSet, FILE *fp);
int         hmeSetLoadAccus      (HmeSet *hmeSet, FILE *fp);
int         hmeSetAccu           (HmeSet *hmeSet, int idx, int frameX,
	 			 float factor);
int         hmeSetUpdate         (HmeSet *hmeSet);
int         hmeSetFrameN         (HmeSet *hmeSet, int *from, int *shift,
				  int *ready);
FeatureSet  *hmeSetFesUsed       (HmeSet *hmeSet);



int         hmeStateSetPutsItf   (ClientData cd, int argc, char *argv[]);
ClientData  hmeStateSetAccessItf (ClientData cd, char *name, TypeInfo **ti);



ClientData  hmeSetCreateItf      (ClientData cd, int argc, char *argv[]);
int         hmeSetFreeItf        (ClientData cd);
int         hmeSetPutsItf        (ClientData cd, int argc, char *argv[]);
int         hmeSetConfigureItf   (ClientData cd, char *var, char *val);
ClientData  hmeSetAccessItf      (ClientData cd, char *name, TypeInfo **ti); 
int         hmeSetAddItf         (ClientData cd, int argc, char *argv[]);
int         hmeSetPlugItf        (ClientData cd, int argc, char *argv[]);
int         hmeSetReadItf        (ClientData cd, int argc, char *argv[]);
int         hmeSetSaveItf        (ClientData cd, int argc, char *argv[]);
int         hmeSetLoadItf        (ClientData cd, int argc, char *argv[]);
int         hmeSetAddMapItf      (ClientData cd, int argc, char *argv[]);
int         hmeSetReadMapItf     (ClientData cd, int argc, char *argv[]);
int         hmeSetActivateItf    (ClientData cd, int argc, char *argv[]);
int         hmeSetDeactivateItf  (ClientData cd, int argc, char *argv[]);
int         hmeSetSmoothItf      (ClientData cd, int argc, char *argv[]);
int         hmeSetResetItf       (ClientData cd, int argc, char *argv[]);
int         hmeSetConfHmesItf    (ClientData cd, int argc, char *argv[]);
int         hmeSetConfGatesItf   (ClientData cd, int argc, char *argv[]);
int         hmeSetConfExpertsItf (ClientData cd, int argc, char *argv[]);
int         hmeSetIndexItf       (ClientData cd, int argc, char *argv[]);
int         hmeSetNameItf        (ClientData cd, int argc, char *argv[]);
int         hmeSetScoreItf       (ClientData cd, int argc, char *argv[]);
int         hmeSetCreateAccusItf (ClientData cd, int argc, char *argv[]);
int         hmeSetFreeAccusItf   (ClientData cd, int argc, char *argv[]);
int         hmeSetClearAccusItf  (ClientData cd, int argc, char *argv[]);
int         hmeSetSaveAccusItf   (ClientData cd, int argc, char *argv[]);
int         hmeSetLoadAccusItf   (ClientData cd, int argc, char *argv[]);
int         hmeSetAccuItf        (ClientData cd, int argc, char *argv[]);
int         hmeSetUpdateItf      (ClientData cd, int argc, char *argv[]);



#endif






#ifdef __cplusplus
}
#endif
