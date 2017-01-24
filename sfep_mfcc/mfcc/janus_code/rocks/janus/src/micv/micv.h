/* ========================================================================
    JANUS      Speech- to Speech Translation
               ------------------------------------------------------------
               Object: micv
               ------------------------------------------------------------

    Author  :  Andreas Eller
    Module  :  micv.h
    Date    :  $Id: micv.h 2633 2005-03-08 17:30:28Z metze $
    Remarks :  Mixtures of Inverse Covariances
               "Diplomarbeit" by Andreas Eller
	           supervised by Florian Metze

	       References: Vincent Vanhoucke (Thesis, Papers)
	                   http://vincent.vanhoucke.com/publications/main.html

	       Caveats: * Code seems to have difficulty with icc
	                * Can grow very large
			* Check threading settings
			* Tested, but not extensively

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
    Revision 1.1  2005/03/08 17:30:28  metze
    Initial code as of Andreas Eller


   ======================================================================== */

#ifdef __cplusplus
extern "C" {
#endif


#ifndef _micv
#define _micv

#include "itf.h"
#include "list.h"
#include "common.h"
#include "error.h"
#include "mach_ind_io.h"

#include "codebook.h"
#include "distrib.h"
#include "modelSet.h"

#include "micvDefines.h"

#include <pthread.h>
#include <semaphore.h>


/* --- Exported datatypes --- */

struct Micv;
struct MicvSet;

/* -- Defines -- */

#define MICV_LIST_BLKSIZE	50000  /* list blocksize  */


/* -------------------------------------------------------------------
   Micv
   ------------------------------------------------------------------- */

typedef enum { eDeactivated = -1, eGMM, eMICV } MicvType;
typedef enum { eEstimateWeights, eCalcPTGradient, eUpdateAllCovariances, eValidateAllCovariances, eEndThread } MicvThreadCommand;


typedef struct MicvAccu {
	int				nAccumulatedFrames;		// number of frames accumulated to this model
	DMatrix*		pFeatVecSum;			// (#cov x #dim) matrix: sum of feature vectors o(t) * gamma(model,t)
	DMatrix**		pSampleCov;				// accumulators for the sample covariances [#cov * (#dim x #dim)]
	double*			pMixtureWeightSum;		// #cov weights of samples per gaussian = SUM(gamma(cov,t))
	DMatrix**		pSampleCovV;			// the sample covariances in vector representation [#cov * (#dimV x 1)]

	double*			pCovWeights;			// mixture weights for an accumulating frame
} MicvAccu;


typedef struct Micv {
	char*             name; 
	int               useN;
	struct MicvSet*   pMicvSet;					// belongs to ...
	int               nFeat;					// used feature set
	MicvType          micvType;					// GMM, MICV or deactivated

	int               nCov;						// number of covariances used in this model
	DMatrix*          pPrototypeWeights;		// (#cov x #PT) matrix
	DMatrix*          pMedian;					// (#cov x #dim) matrix
	DVector*          pMixtureWeights;			// #cov relative weights of samples per gaussian = SUM(gamma(cov,t)) / #Models

	// data used only for MICV type GMM
	CovType*          pCovType;					// the type of the initial GMM's inverse covariances

	// precalculated values for score function
	DMatrix**         pInvCov;					// #cov (#dim x #dim) inverse covariances
	DVector*          pLogMixtureWeights;		// #cov log of gaussian mixture weights
	double            fDLog2Pi;					// = nDim * log(2*pi)
	double*           pConstScore;				// #cov constant parts of score function
	DMatrix*          pV;						// (#cov x #dim) precalculated values for score function

	// data structures for score function to avoid memory allocation during score function
	double*           pDistVec;					// distance vector (#dim), feature - median
	double*           pScoreVec;				// score vector (#cov)

	MicvAccu*         pAccu;

	// pointer to score function currently used
	// this pointer should point to micv_scoreGMM() if the MICV model is in its initial status representing a simple GMM
	// otherwise it should point to micv_scoreMICV()
	float             (*scoreFct)(struct Micv* pMicv, const int nFrame, double* pCovWeights);

} Micv;


typedef struct MicvSubsetItem {
	Micv*			pMicv;
	int				nCov;
} MicvSubsetItem;


/* -------------------------------------------------------------------
   MicvSet
   ------------------------------------------------------------------- */

typedef struct LIST(Micv) MicvList;


typedef struct MicvConfig {
	int		nPrototypeClusterIterations;
	float	fPTClusterSubset;
	int		nEstimationIterations;

	int		nWeightsEstIterations;
	float	fWeightsEstEpsilon;
	float	fWeightsEstInitialGamma;
	float	fWeightsEstMinimalGamma;
	float	fWeightsEstGammaDivider;

	int		nPTEstInitialIterations;
	int		nPTEstFinalIterationsOuterLoop;
	int		nPTEstFinalIterationsInnerLoop;
	float	fPTSubset;
	float	fPTEstEpsilon;
	float	fPTEstInitialGamma;
	float	fPTEstMinimalGamma;
	float	fPTEstGammaDivider;

	float	fExpThreshold;
} MicvConfig;


typedef struct MicvWeightsOptData {
	// data structure for weights hessian
	DMatrix**				pPTCovProduct;		// array of matrices used for the weights hessian calculation
	DMatrix**				pPTCovProductT;		// 
	DMatrix*				pWHess;				// weights hessian
} MicvWeightsOptData;


typedef struct MicvPTOptData {
#ifdef MICV_MULTITHREADED
	DMatrix*				pCovMT[MICV_THREADS];
	DMatrix*				pPTGradientMT[MICV_THREADS];
#else
#endif

	DMatrix*				pCov;
	DMatrix*				pPTGradient;
	DMatrix*				pPTBackup;
	int						nPTGradientIdx;		// needed for multithreaded PT gradient calculation
} MicvPTOptData;


typedef struct MicvSet {
	char*				name;
	int					useN;

	MicvList			micvList;				// list of Micv codebooks
	int					nGaussians;				// total number of gaussians
	int					nPrototypes;			// number of prototypes
	DMatrix**			pPrototypes;			// array of prototype matrices
	DMatrix*			pPrototypesVT;			// prototype matrices in vector form: [P_(1)...P_(k)]_Trans

	int					nDim;					// Number of dimensions of feature space
	int					nDimV;					// dimension of sym. matrices in vector form = D*(D+1)/2
	FeatureSet*			pFeatSet;				// Assoc. feature set
    float				fScoreFactor;			// multiplier for score function to match score level of CBS system
												// (otherwise the AM / LM ratio may cause worse results)
	MicvConfig			cfg;

	// optimizations for scoreMICV()
	double*				pW;						// #prototypes precalculated values for score function
	int					nFrame;					// stores the actual frames for which *pW is valid

	// EM optimizations
	int					nSubset;				// size of the subset
	MicvSubsetItem*		pSubset;				// subset of the most important covariances

#ifdef MICV_MULTITHREADED
	// thread data
	pthread_attr_t		thread_Attr;
	pthread_t			thread_ID[MICV_THREADS];
	sem_t				thread_Start_Sem[MICV_THREADS];
	sem_t				thread_Ready_Sem[MICV_THREADS];
	MicvThreadCommand	thread_Command;

	// shared flag for validateAllCovariances()
	BOOL				bAllCovariancesValid;

#endif

	// pointer to PT reestimation optimization data
	MicvPTOptData*		pPTOptData;

	//statistics to gain better default values
	double					Q, Qold;
} MicvSet;


/* -------------------------------------------------------------------
   Janus Interface
   ------------------------------------------------------------------- */

int					micvPutsItf					(ClientData cd, int argc, char *argv[]);
int					micvConfigureItf			(ClientData cd, char *var, char *val);
ClientData			micvAccessItf				(ClientData cd, char *name, TypeInfo **ti);

extern int			micvSaveWeightsItf			(ClientData cd, int argc, char *argv[]);
extern int			micvLoadWeightsItf			(ClientData cd, int argc, char *argv[]);

extern ClientData	micvsetCreateItf			(ClientData cd, int argc, char *argv[]);
extern int			micvsetDestroyItf			(ClientData cd);
extern int			micvsetPutsItf				(ClientData cd, int argc, char *argv[]);
extern int			micvsetConfigureItf			(ClientData cd, char *var, char *val);
extern ClientData	micvsetAccessItf			(ClientData cd, char *name, TypeInfo **ti);
extern int			micvsetIndexItf				(ClientData cd, int argc, char *argv[]);
       int			micvsetIndex				(MicvSet *pMicvSet, char *name);
extern int			micvsetNameItf				(ClientData cd, int argc, char *argv[]);
       char*		micvsetName					(MicvSet *pMicvSet, int index);
extern int			micvsetAddItf				(ClientData cd, int argc, char *argv[]);
extern int			micvsetDeleteItf			(ClientData cd, int argc, char *argv[]);

extern int			micvsetInitItf				(ClientData cd, int argc, char *argv[]);
extern int			micvsetInitPTsItf			(ClientData cd, int argc, char *argv[]);
extern int			micvsetUpdateItf			(ClientData cd, int argc, char *argv[]);
extern int			micvsetEMUpdateItf			(ClientData cd, int argc, char *argv[]);
extern int			micvsetGMMUpdateItf			(ClientData cd, int argc, char *argv[]);
extern int			micvsetScoreItf				(ClientData cd, int argc, char *argv[]);
extern int			micvsetEstimateWeightsItf	(ClientData cd, int argc, char *argv[]);
extern int			micvsetEstimatePTsItf		(ClientData cd, int argc, char *argv[]);
extern int			micvsetUpdateMICVItf		(ClientData cd, int argc, char *argv[]);

extern int			micvsetSaveItf				(ClientData cd, int argc, char *argv[]);
extern int			micvsetLoadItf				(ClientData cd, int argc, char *argv[]);
extern int			micvsetWriteItf				(ClientData cd, int argc, char *argv[]);
extern int			micvsetReadItf				(ClientData cd, int argc, char *argv[]);
extern int			micvsetWriteModelFileItf	(ClientData cd, int argc, char *argv[]);

extern int			micvsetQFunctionItf			(ClientData cd, int argc, char *argv[]);

extern int			micvsetCreateAccusItf		(ClientData cd, int argc, char *argv[]);
extern int			micvsetFreeAccusItf			(ClientData cd, int argc, char *argv[]);
extern int			micvsetClearAccusItf		(ClientData cd, int argc, char *argv[]);
extern int			micvsetSaveAccusItf			(ClientData cd, int argc, char *argv[]);
extern int			micvsetLoadAccusItf			(ClientData cd, int argc, char *argv[]);

extern int			Micv_Init();
extern int			MicvSet_Init();


#ifdef _DEBUG
void micvset_Assert(MicvSet* pMicvSet);
void micv_Assert(Micv* pMicv);
#endif


#endif	// _micv

#ifdef __cplusplus
}
#endif

