
/* ========================================================================
    JANUS      Speech- to Speech Translation
               ------------------------------------------------------------
               Object: extended codebooks for semi tied covariances
               ------------------------------------------------------------

    Author  :  Matthias Seeger
    Module  :  cbnew.h
    Date    :  $Id: cbnew.h 699 2000-11-14 12:01:12Z janus $
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

    Revision 4.2.30.1  2000/11/06 10:50:25  janus
    Made changes to enable compilation under -Wall.

    Revision 4.2  2000/08/27 15:31:09  jmcd
    Merging branch 'jtk-00-08-24-jmcd'.

    Revision 4.1.2.1  2000/08/25 22:19:43  jmcd
    Just checking.

    Revision 4.1  2000/08/24 13:32:25  jmcd
    Merging changes from branch jtk-00-08-23-jmcd.

    Revision 1.1.2.1  2000/08/24 00:17:53  jmcd
    Fixed the hairy elbows Hagen left for me to find.


   ======================================================================== */

#ifdef __cplusplus
extern "C" {
#endif


#ifndef _cbnew
#define _cbnew

/* Janus interface */
#include "itf.h"
#include "matrix.h"
#include "feature.h"

#include "matrix_ext.h"
#include "cbn_global.h"
#include "cbn_parmat.h"

#include <stdio.h>

/* --- Exported datatypes --- */

struct CBNew;
struct CBNewSet;


/* -------------------------------------------------------------------
   CBNew_protslot : Specifies a protocol slot of the CBNew object.
   ------------------------------------------------------------------- */

typedef int (*crit_func_ptr) (struct CBNew*,FMatrixX*,double*);

typedef struct {
  crit_func_ptr crit;     /* Pointer to crit. function (0 => entry empty) */
  int           counter;  /* number of evaluations so far */
  FMatrixX*     epoch;    /* Alias matrix for act. epoch */
} CBNew_protslot;


/* -------------------------------------------------------------------
   CBNew_accu : Covariance accumulators for one gaussian.
   ------------------------------------------------------------------- */

typedef struct {
  double** block;  /* blocks (stored as triangular matrices) */
  double*  diag;   /* diagonal vector */
} CBNew_accu;

/* -------------------------------------------------------------------
   CBNew_pmlink : list of linked parameter matrices. 'index' is the index
   ------------------------------------------------------------------- */

typedef struct {
  CBNewParMatrix*  mat;
  int              index;
} CBNew_pmlink;

/* -------------------------------------------------------------------
   CBNew
   ------------------------------------------------------------------- */

typedef struct CBNew {
  char*            name; 
  int              useN;

  int              num_dens;             /* Number of densities in mixture */
  char             accu_flag;            /* Accumulator state */
  int              accu_num;             /* Number of samples used so far to build the accus */
  struct CBNewSet* cbnew_set;            /* belongs to ... */
  int              feat;                 /* Index of feature to use */

  char*            active;
  float*           distrib;              /* Mixture coefficients */
  FMatrix*         mean;                 /* Mean vector matrix */
  FMatrix*         diag;                 /* Diagonal variance vector matrix */
  CBNew_pmlink*    parmat;

  /* Accumulators */
  double*          accu_gamma_sum;
  DMatrix*         accu_mean;
  CBNew_accu*      accu_cov;

  /* Protocol slots */
  CBNew_protslot* slot;                  /* Protocol slot list */
  int             protlist_size;         /* Size of slot list (active+free slots) */

  /* Internal temporary fields */
  char            internal_flag;         /* 1, if temp. fields are valid, 0 else */
  double*         lh_factor;
  FMatrix*        trans_mean;

  /* Other temporary fields */
  double*         temp_gamma;
  float*          lh_buff;
  double*         temp_samp;
} CBNew;


/* -------------------------------------------------------------------
   CBNewSet
   ------------------------------------------------------------------- */

typedef struct LIST(CBNew) CBNewList;

typedef struct CBNewSet {
  char*              name;  
  int                useN;

  int                num_dim;             /* Number of dimensions of feature space */
  char               phase;               /* Object phase */
  char               train_parmats;
  CBNewParMatrixSet* parmat_set;          /* Assoc. parameter matrix set */
  FeatureSet*        feat_set;            /* Assoc. feature set */
  CBNewList          list;
} CBNewSet;


/* -------------------------------------------------------------------
   Interface CBNew
   ------------------------------------------------------------------- */

int          cbnewInitEmpty      (CBNew* cd,ClientData name);
int          cbnewInit           (CBNew* cd,char* name,int num_dens,
				  CBNewSet* cbnew_set,int feat);
CBNew*       cbnewCreate         (char* name,int num_dens,
				  CBNewSet* cbnew_set,int feat);

int          cbnewDeinit         (CBNew* cd);
int          cbnewDestroy        (CBNew* cd);
int          cbnewPuts           (CBNew* cd);

int          cbnewLink           (CBNew* cd,CBNewParMatrix* pmat,int dens_index); 
int          cbnewUnlink         (CBNew* cd,int dens_index);

int          cbnewClearAccus     (CBNew* cd);
int          cbnewAccu           (CBNew* cd,float* sample,double factor,
				  double* loglh);

int          cbnewUpdate         (CBNew* cd,int* not_mean,int* not_diag);

int          cbnewEvalKL         (CBNew* cd,double* val);

crit_func_ptr cbnewCritfuncPtr   (char*);
char*        cbnewCritfuncName   (crit_func_ptr);

int          cbnewCritLogLH      (struct CBNew* cd,FMatrixX* data,
				  double* val);
int          cbnewOpenProt       (CBNew* cd,FMatrix* datamat,
				  crit_func_ptr crit,int begin,int end,
				  int* protnum);
int          cbnewCloseProt      (CBNew* cd,int protnum);
int          cbnewEvalProt       (CBNew* cd,int protnum);
int          cbnewDataPlot       (CBNew* cd,FMatrix* data,int begin,int end,
                                  SVector* ind_vect,FILE* fp_plot,int ref_ind);
int          cbnewGenSamples     (CBNew* cd,int num_samples,FMatrix* smat,
				  double* seed,int choose_seed);


int          cbnewPutsItf        (ClientData cd,int argc,char *argv[]);
int          cbnewConfigureItf   (ClientData cd,char *var,char *val);
ClientData   cbnewAccessItf      (ClientData cd,char *name,TypeInfo **ti);
int          cbnewSetItf         (ClientData cd,int argc,char *argv[]);


/* -------------------------------------------------------------------
   Interface CBNewSet
   ------------------------------------------------------------------- */


int          cbnewsetInit           (CBNewSet* cd,char* name,int num_dim,
				     CBNewParMatrixSet* parmat_set,
				     FeatureSet* feat_set);
CBNewSet*    cbnewsetCreate         (char* name,int num_dim,
				     CBNewParMatrixSet* parmat_set,
				     FeatureSet* feat_set);

int          cbnewsetDeinit         (CBNewSet* cd);
int          cbnewsetDestroy        (CBNewSet* cd);

int          cbnewsetPuts           (CBNewSet* cd);
CBNew*       cbnewsetFind           (CBNewSet* cd,char* name,int* index);

CBNewSet*    cbnewsetLoad           (char* name,char* fname,
                                     CBNewParMatrixSet* parmat_set,
				     FeatureSet* feat_set,int* wrongFeatN,
				     int* noPMN);
int          cbnewsetSave           (CBNewSet* cd,char* fname);

int          cbnewsetLoadWeights    (CBNewSet* cd,char* fname,int* okN,
				     int* nexistN,int* nemptyN,
				     int* wrongRefNN);
int          cbnewsetSaveWeights    (CBNewSet* cd,char* fname);


int          cbnewsetLoadAccus      (CBNewSet* cd,char* fname,int* emptyN,
				     int* lockedN,int* buildN,int* nexistN,
				     int* wrongRefNN,int* wrongAS,int* structN,
				     int* inactN);
int          cbnewsetSaveAccus      (CBNewSet* cd,char* fname,int* emptyN,
				     int* lockedN,int* buildN);


int          cbnewsetScoreArray     (CBNewSet* cd,int* index,float* score,
				     int num_ind,int frame_ind);

int          cbnewsetAccu           (CBNewSet* cd,int cbX,int frameX,  float factor);
int          cbnewsetUpdate         (CBNewSet* cd);
int          cbnewsetIndex          (CBNewSet* cd,char *name);
char*        cbnewsetName           (CBNewSet* cd,int index);

/* --- Janus itf functions --- */

extern ClientData   cbnewsetCreateItf      (ClientData cd,int argc,char *argv[]);
extern int          cbnewsetDestroyItf     (ClientData cd);
extern int          cbnewsetPutsItf        (ClientData cd,int argc,char *argv[]);
extern int          cbnewsetConfigureItf   (ClientData cd,char *var,char *val);
extern ClientData   cbnewsetAccessItf      (ClientData cd,char *name,TypeInfo **ti);
extern int          cbnewsetIndexItf       (ClientData cd,int argc,char *argv[]);
extern int          cbnewsetNameItf        (ClientData cd,int argc,char *argv[]);
extern int          cbnewsetAddItf         (ClientData cd,int argc,char *argv[]);
extern int          cbnewsetLinkItf        (ClientData cd,int argc,char *argv[]);
extern int          cbnewsetUnlinkItf      (ClientData cd,int argc,char *argv[]);
extern int          cbnewsetPhaseItf       (ClientData cd,int argc,char *argv[]);
extern int          cbnewsetAccuItf        (ClientData cd,int argc,char *argv[]);
extern int          cbnewsetAccuMatrixItf  (ClientData cd,int argc,char *argv[]);
extern int          cbnewsetUpdateItf      (ClientData cd,int argc,char *argv[]);
extern int          cbnewsetClearAccusItf  (ClientData cd,int argc,char *argv[]);
extern int          cbnewsetEvalKLItf      (ClientData cd,int argc,char *argv[]);
extern int          cbnewsetOpenProtItf    (ClientData cd,int argc,char *argv[]);
extern int          cbnewsetEvalProtItf    (ClientData cd,int argc,char *argv[]);
extern int          cbnewsetCloseProtItf   (ClientData cd,int argc,char *argv[]);
extern int          cbnewsetDataPlotItf    (ClientData cd,int argc,char *argv[]);
extern int          cbnewsetSaveItf        (ClientData cd,int argc,char *argv[]);
extern int          cbnewsetLoadWeightsItf (ClientData cd,int argc,char *argv[]);
extern int          cbnewsetSaveWeightsItf (ClientData cd,int argc,char *argv[]);
extern int          cbnewsetLoadAccusItf   (ClientData cd,int argc,char *argv[]);
extern int          cbnewsetSaveAccusItf   (ClientData cd,int argc,char *argv[]);
extern int          cbnewsetGenSamplesItf  (ClientData cd,int argc,char *argv[]);

extern int          CBNew_Init ();
extern int          CBNewSet_Init ();

#endif

#ifdef __cplusplus
}
#endif
