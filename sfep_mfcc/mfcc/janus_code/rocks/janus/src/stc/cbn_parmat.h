/* ========================================================================
    JANUS      Speech- to Speech Translation
               ------------------------------------------------------------
               Object: semi tied covariances
               ------------------------------------------------------------

    Author  :  Matthias Seeger
    Module  :  cbn_param.h
    Date    :  $Id: cbn_parmat.h 699 2000-11-14 12:01:12Z janus $
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
    Revision 4.3  2000/11/14 12:01:10  janus
    Merged branch jtk-00-10-27-jmcd.

    Revision 4.2.34.1  2000/11/06 10:50:24  janus
    Made changes to enable compilation under -Wall.

    Revision 4.2  2000/08/27 15:31:09  jmcd
    Merging branch 'jtk-00-08-24-jmcd'.

    Revision 4.1.2.1  2000/08/25 22:19:42  jmcd
    Just checking.

    Revision 4.1  2000/08/24 13:32:25  jmcd
    Merging changes from branch jtk-00-08-23-jmcd.

    Revision 1.1.2.1  2000/08/24 00:17:52  jmcd
    Fixed the hairy elbows Hagen left for me to find.


   ======================================================================== */

#ifdef __cplusplus
extern "C" {
#endif

#ifndef _cbn_parmat
#define _cbn_parmat

/* Janus interface */
#include "list.h"
#include "itf.h"
#include "matrix.h"

#include "cbn_global.h"

/* --- Exported datatypes --- */

struct CBNew;
struct CBNewSet;
struct CBNewParMatrixSet;

/* -------------------------------------------------------------------
   CBNewParMatrixLink
   ------------------------------------------------------------------- */
typedef struct {
  struct CBNew* cb;
  int index;
} CBNewParMatrixLink;

/* -------------------------------------------------------------------
   CBNewParMatrix
   ------------------------------------------------------------------- */

typedef struct CBNewParMatrix {
  char* name;  
  int   useN;

  int    active;                         /* activate/deactivate parameter matrix */
  int    num_block;                      /* Number of blocks in parameter matrix */
  int    num_rel;                        /* Number of relevant components (sum of block sizes) */
  float  learn_rate;                     /* Learning rate used for update */
  struct CBNewParMatrixSet* parmat_set;  /* belongs to ... */
  char   def;

  FMatrix**            list;             /* Parameter matrix block list */
  int*                 dim_vect;         /* dimension index vector */
  CBNewParMatrixLink*  link;             /* Link list */
  int                  linklist_size;    /* Number of entries in link list */
  int                  linklist_num;     /* Number of active entries in link list */
  /* Temp. arrays */
  float* trans_sample;
  char ts_valid;
} CBNewParMatrix;

/* -------------------------------------------------------------------
   CBNewParMatrixSet
   ------------------------------------------------------------------- */

typedef struct LIST(CBNewParMatrix) CBNewParMatrixList;

typedef struct CBNewParMatrixSet {
  char* name;  
  int   useN;

  struct CBNewSet*   cbnew_set;         /* Assoc. codebook set (or 0) */
  int                num_dim;           /* Number of dimensions of feature space */
  float              def_learn_rate;    /* Default learning rate */

  CBNewParMatrixList list;
  int                list_size;         /* Number of entries in list */
  int                list_num;          /* Number of active entries in list */

  int                cluster_times;
} CBNewParMatrixSet;


/* -------------------------------------------------------------------
   Interface CBNewParMatrix
   ------------------------------------------------------------------- */

int          cbnewparmatInitEmpty      (CBNewParMatrix* cd,ClientData name);

int          cbnewparmatInit           (CBNewParMatrix* cd,char* name,
					int num_block,
					CBNewParMatrixSet* parmat_set,
					int* size_vect,int* dim_vect,
					float learn_rate);

CBNewParMatrix* cbnewparmatCreate      (char* name,int num_block,
					CBNewParMatrixSet* parmat_set,
					int* size_vect,int* dim_vect,
					float learn_rate);

int          cbnewparmatDeinit         (CBNewParMatrix* cd);
int          cbnewparmatDestroy        (CBNewParMatrix* cd);
int          cbnewparmatDefault        (CBNewParMatrix* cd);
int          cbnewparmatPuts           (CBNewParMatrix* cd);

int          cbnewparmatLink           (CBNewParMatrix* cd,struct CBNew* cb,
					int cb_ind,int* pm_ind);
int          cbnewparmatUnlink         (CBNewParMatrix* cd,int index);

int          cbnewparmatSet            (CBNewParMatrix* cd,int block,
					FMatrix* pmat);

int          cbnewparmatUpdate         (CBNewParMatrix* cd,int upperb,
					int small_steps,int first_small_steps,
					double delta_thres,int* steps_done);

int          cbnewpmsetClusterfuncCode (char*);


char*        cbnewpmsetClusterfuncName (int code);

int          cbnewpmsetCluster         (CBNewParMatrixSet* cd,int code,
					int initFlag,double samplesThres,
					int* gaussN,int* changesN);

int          cbnewparmatPutsItf        (ClientData cd,int argc,char *argv[]);
int          cbnewparmatConfigureItf   (ClientData cd,char *var,char *val);
ClientData   cbnewparmatAccessItf      (ClientData cd,char *name,
					TypeInfo **ti);
int          cbnewparmatSetItf         (ClientData cd,int argc,char *argv[]);


/* -------------------------------------------------------------------
   Interface CBNewParMatrixSet
   ------------------------------------------------------------------- */

int                cbnewpmsetInit      (CBNewParMatrixSet* cd,char* name,
					int num_dim,float def_lr);
CBNewParMatrixSet* cbnewpmsetCreate    (char* name,int num_dim,float def_lr);
int                cbnewpmsetDeinit    (CBNewParMatrixSet* cd);
int                cbnewpmsetDestroy   (CBNewParMatrixSet* cd);

CBNewParMatrixSet* cbnewpmsetLoad      (char* name,char* fname);
int                cbnewpmsetSave      (CBNewParMatrixSet* cd,char* fname);

int          cbnewpmsetLoadWeights     (CBNewParMatrixSet* cd,char* fname,
                                        int* okN,int* nexistN,int* nemptyN,
					int* structN);
int          cbnewpmsetSaveWeights     (CBNewParMatrixSet* cd,char* fname);

int          cbnewpmsetExchange        (CBNewParMatrixSet* cd,
					CBNewParMatrix* new,
					CBNewParMatrix* old);

int          cbnewpmsetEvalKL          (CBNewParMatrixSet* cd,int index,
					double* val,int* okN);

/* --- Janus itf functions --- */

extern ClientData   cbnewpmsetCreateItf       (ClientData cd,int argc,char *argv[]);
extern int          cbnewpmsetDestroyItf      (ClientData cd);
extern int          cbnewpmsetPutsItf         (ClientData cd,int argc,char *argv[]);
extern int          cbnewpmsetConfigureItf    (ClientData cd,char *var,char *val);
extern ClientData   cbnewpmsetAccessItf       (ClientData cd,char *name,
					       TypeInfo **ti);
extern int          cbnewpmsetIndexItf        (ClientData cd,int argc,char *argv[]);
extern int          cbnewpmsetNameItf         (ClientData cd,int argc,char *argv[]);
extern int          cbnewpmsetAddItf          (ClientData cd,int argc,char *argv[]);
extern int          cbnewpmsetCleanupItf      (ClientData cd,int argc,char *argv[]);
extern int          cbnewpmsetUpdateItf       (ClientData cd,int argc,char *argv[]);
extern int          cbnewpmsetSaveItf         (ClientData cd,int argc,char *argv[]);
extern int          cbnewpmsetLoadWeightsItf  (ClientData cd,int argc,char *argv[]);
extern int          cbnewpmsetSaveWeightsItf  (ClientData cd,int argc,char *argv[]);
extern int          cbnewpmsetExchangeItf     (ClientData cd,int argc,char *argv[]);
extern int          cbnewpmsetEvalKLItf       (ClientData cd,int argc,char *argv[]);
extern int          cbnewpmsetRelCompItf      (ClientData cd,int argc,char *argv[]);

extern int          CBNewParMatrix_Init ();
extern int          CBNewParMatrixSet_Init ();

#endif

#ifdef __cplusplus
}
#endif
