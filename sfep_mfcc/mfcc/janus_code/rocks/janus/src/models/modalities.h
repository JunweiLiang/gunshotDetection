/* ========================================================================
    JANUS      Speech- to Speech Translation
               ------------------------------------------------------------
               Object: Modalities Maintainance Functions
               ------------------------------------------------------------

    Author  :  Christian Fuegen
    Module  :  modalities.h
    Date    :  $Id: modalities.h 700 2000-11-14 12:35:27Z janus $
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
   Revision 1.5  2000/11/14 12:35:24  janus
   Merging branch jtk-00-10-27-jmcd.

   Revision 1.4.34.1  2000/11/06 10:50:37  janus
   Made changes to enable compilation under -Wall.

   Revision 1.4  2000/08/27 15:31:26  jmcd
   Merging branch 'jtk-00-08-24-jmcd'.

   Revision 1.3.4.1  2000/08/25 22:19:45  jmcd
   Just checking.

   Revision 1.3  2000/01/12 10:12:49  fuegen
   add updateModes and some functions for the HMM

   Revision 1.2  1999/03/09 22:00:06  fuegen
   *** empty log message ***

   Revision 1.1  1998/12/09 10:52:03  fuegen
   Initial revision



   ======================================================================== */

#ifdef __cplusplus
extern "C" {
#endif

#ifndef _modalities
#define _modalities

#include "list.h"
#include "tags.h"
#include "tree.h"
#include "matrix.h"

/* Answers for Modalities: yes = 1, no = 0, no don't_know part!! */

/* ========================================================================
    Modalities
   ======================================================================== */

/* define update modes
   description: modality is up to date for Intervall [S,E] and should be
                updated for Intervall [s,e]. For this example S<s<E<e is
		given. */
#define UPD_ALL   0  /* update [S,e] (update only if e-E+1 greater updateLimit) */
#define UPD_GIVEN 1  /* update [s,e] */
#define UPD_RUNON 2  /* update [E,e] (update only if e-E+1 greater updateLimit) */
#define UPD_NOT   3  /* update [] copy only existing values. If nothing exists
			update [s,e]. */

typedef struct {

    char		*name;
    int			 useN;
    char                *tagName;

    int			 startFrameX;
    int			 endFrameX;
    int			*answerA;	/* answers for each frame (0,1) */
    int                  yesN;
    int                  timeInfo;
    int                  updateMode;
    int                  updateLimit;

    char                *updateProc;

} Modality;

extern int        modalityInit   (Modality *modP, ClientData cd);
extern int        modalityDeinit (Modality *modP);
extern int        modalityLinkN  (Modality *modP);

extern Modality*  modalityCreate (char *name, char *updateProc, char *tagName,
				  int updateMode, int updateLimit);
extern int        modalityFree   (Modality *modP);
extern int        modalityAlloc  (Modality* modP, char *updateProc, char *tagName);

extern int*       modalityUpdate   (Modality *modP, int startFrameX, int endFrameX,
				    int *from, int *to);
extern int*       modalityAnswer   (Modality *modP, int startFrameX, int endFrameX);
extern int        modalityReset    (Modality *modP);
extern int        modalityMajority (Modality *modP);

extern int        modalityConfigureItf (ClientData cd, char *var, char *val);
extern int        modalityUpdateItf    (ClientData cd, int argc, char *argv[]);
extern int        modalityAnswerItf    (ClientData cd, int argc, char *argv[]);
extern int        modalityResetItf     (ClientData cd, int argc, char *argv[]);
extern int        modalityMajorityItf  (ClientData cd, int argc, char *argv[]);


/* ========================================================================
    ModalitySet
   ======================================================================== */

typedef struct LIST(Modality) ModalityList;
typedef struct LIST(int)      AnswerList;

/* maximum allowed number of modalities in set
   16 modalities means 256 kB/row in stP->matrix
   (4 Bytes per int) */
/* #define MAX_MODALITIES 16 */
/* training egal (habe keine stateTable) */
#define MAX_MODALITIES 30

typedef struct {

    char		*name;
    int			 useN;

    ModalityList	 list;
    int			 startFrameX;
    int			 endFrameX;
    int                 *answerA;	/* answers for all modalities for each */
					/* frame (bits coded in integers) */
    Tags		*tags;
    int                  addTags;
    Tags		*localTags;

    Tree		*tree;
    int                 *rootNodeA;
    int                  rootNodeN;
    int                  dummyStart;

} ModalitySet;

extern int  Modalities_Init   (void);

extern int  modalitySetInit   (ModalitySet *modsP, char *name, Tags *tags, int addTags);
extern int  modalitySetDeinit (ModalitySet *modsP);
extern int  modalitySetLinkN  (ModalitySet *modsP);
extern int  modalitySetFree   (ModalitySet *modsP);
extern ModalitySet* modalitySetCreate (char *name, Tags *tags, int addTags);

extern int  modalitySetAdd    (ModalitySet *modsP, char *name, char *updateProc,
			       char *tagName);
extern int* modalitySetUpdate (ModalitySet *modsP, int startFrameX, int endFrameX);
extern int* modalitySetAnswer (ModalitySet *modsP, int startFrameX, int endFrameX);
extern int  modalitySetAnswer2CodedTags (ModalitySet *modsP, Tags *tags, int answer);
extern int  modalitySetMajorityAnswer (ModalitySet *modsP, int startFrameX, int endFrameX);

extern ClientData modalitySetAccessItf (ClientData cd, char *name, TypeInfo **ti);
extern int  modalitySetConfigureItf    (ClientData cd, char *var, char *val);
extern int  modalitySetAddItf          (ClientData cd, int argc, char *argv[]);
extern int  modalitySetDeleteItf       (ClientData cd, int argc, char *argv[]);
extern int  modalitySetUpdateItf       (ClientData cd, int argc, char *argv[]);
extern int  modalitySetAnswerItf       (ClientData cd, int argc, char *argv[]);
extern int  modalitySetgetRootNodesItf (ClientData cd, int argc, char *argv[]);
extern int  modalitySetResetItf        (ClientData cd, int argc, char *argv[]);
extern int  modalitySetDeleteTagsItf   (ClientData cd, int argc, char *argv[]);
extern int  modalitySetTraceItf        (ClientData cd, int argc, char *argv[]);
extern int  modalitySetMajorityItf     (ClientData cd, int argc, char *argv[]);
extern int  modalitySetAnswer2TagsItf  (ClientData cd, int argc, char *argv[]);

/* ========================================================================
    StateTable
   ======================================================================== */

typedef struct {

    char		*name;
    int			 useN;

    ModalitySet		*modsP;

    int			 startFrameX;
    int                  endFrameX;

    IMatrix		*matrix;
    int                  m;
    int                  n;

    int                  treeXN;
    int                  modXN;
    int                  dummyStart;

    int                  timeInfo;

    int                  compress;
    int                 *compressA;
    int                  compressN;

    char                 commentChar;

} StateTable;


extern int  stateTableInit     (StateTable *stP, char *name, ModalitySet *modsP,
				int compress);
extern int  stateTableDeinit   (StateTable *stP);
extern int  stateTableLinkN    (StateTable *stP);
extern int  stateTableFree     (StateTable *stP);
extern StateTable* stateTableCreate (char *name, ModalitySet *modsP, int compress);

extern StateTable* stateTableCopy (StateTable *stP);
extern int  stateTableGetEntry (StateTable *stP, int treeX, int modalityX);
extern int  stateTableSetEntry (StateTable *stP, int dsX, int treeX, int modalityX);
extern int  stateTableLookup   (StateTable *stP, int dsX, int frameX);
extern int* stateTableUpdate   (StateTable *stP, int startFrameX, int endFrameX);
extern int  stateTableResize   (StateTable *stP, int m, int n);
extern int  stateTableLoad     (StateTable *stP, char *fileName);
extern int  stateTableSave     (StateTable *stP, char *fileName);

extern ClientData stateTableAccessItf (ClientData cd, char *name, TypeInfo **ti);
extern int  stateTableConfigureItf    (ClientData cd, char *var, char *val);
extern int  stateTableSetEntryItf     (ClientData cd, int argc, char *argv[]);
extern int  stateTableGetEntryItf     (ClientData cd, int argc, char *argv[]);
extern int  stateTableUpdateItf       (ClientData cd, int argc, char *argv[]);
extern int  stateTableCopyItf         (ClientData cd, int argc, char *argv[]);
extern int  stateTableResizeItf       (ClientData cd, int argc, char *argv[]);
extern int  stateTableLoadItf         (ClientData cd, int argc, char *argv[]);
extern int  stateTableSaveItf         (ClientData cd, int argc, char *argv[]);
extern int  stateTableLookupItf       (ClientData cd, int argc, char *argv[]);
extern int  stateTableMatrixCreateItf (ClientData cd, int argc, char *argv[]);
extern int  stateTableResetItf        (ClientData cd, int argc, char *argv[]);

#endif

#ifdef __cplusplus
}
#endif
