/* ========================================================================
    JANUS      Speech- to Speech Translation
               ------------------------------------------------------------
               Object: micv
               ------------------------------------------------------------

    Author  :  Andreas Eller
    Module  :  micv
    Date    :  $Id: micvPrototypes.c 3416 2011-03-23 03:02:18Z metze $
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
    Revision 1.1  2005/03/08 17:30:28  metze
    Initial code as of Andreas Eller


   ======================================================================== */

#include "micvPrototypes.h"
#include "micvEM.h"
#include "micvFunctions.h"
#include "micvMultithread.h"
#include "micvScore.h"
#include "micvInit.h"


/* ----------------------------------------------------------------------------
 *	allocate the prototype matrices
 * ----------------------------------------------------------------------------*/
void micvset_allocPrototypes(MicvSet* pMicvSet)
{
	int k;

	if(pMicvSet->pPrototypes)
		WARN("prototypes already allocated. Free memory before reallocation!\n");

	if(pMicvSet->nDim <= 0)
		WARN("dimension of prototypes will be %d. Set correct MICV dimension first!\n", pMicvSet->nDim);

	// create prototypes
	VERIFY_MALLOC(pMicvSet->pPrototypes = malloc(sizeof(DMatrix*) * pMicvSet->nPrototypes), return);
	for(k=0; k < pMicvSet->nPrototypes; k++)
	{
		pMicvSet->pPrototypes[k] = dmatrixCreate(pMicvSet->nDim, pMicvSet->nDim);
		dmatrixInitConst(pMicvSet->pPrototypes[k], 0.0);
	}
	pMicvSet->pPrototypesVT = dmatrixCreate(pMicvSet->nPrototypes, pMicvSet->nDimV);
	dmatrixInitConst(pMicvSet->pPrototypesVT, 0.0);

	// optimizations for score function
	VERIFY_MALLOC(pMicvSet->pW = malloc(pMicvSet->nPrototypes * sizeof(double)), return);
}


/* ----------------------------------------------------------------------------
 *	free the prototype matrices
 * ----------------------------------------------------------------------------*/
void micvset_freePrototypes(MicvSet* pMicvSet)
{
	int k;

	// free prototypes
	if(pMicvSet->pPrototypes != NULL)
	{
		for(k=0; k < pMicvSet->nPrototypes; k++)
			dmatrixFree(pMicvSet->pPrototypes[k]);
		SAFE_FREE(pMicvSet->pPrototypes);
	}

	// free prototypesVT
	if(pMicvSet->pPrototypesVT != NULL)
	{
		dmatrixFree(pMicvSet->pPrototypesVT);
		pMicvSet->pPrototypesVT = NULL;
	}

	// optimizations for score function
	SAFE_FREE(pMicvSet->pW);
}



/* ----------------------------------------------------------------------------
 *	init the prototype matrices with CodebookSet covariances
 * ----------------------------------------------------------------------------*/
void micvset_initPrototypesWithDistribSet(MicvSet* pMicvSet, const DistribSet* pDistribSet, const int nClusterIterations)
{
	int i, k, cb, cvm;
	int nInitialCov;
	DMatrix** pCov;
	CodebookSet* pCBSet = pDistribSet->cbsP;
	double fCountSum;
	int nRank;

	assert(pMicvSet);
	assert(pCBSet);

	if(pMicvSet->nPrototypes <= 0)
	{
		ERROR("Illegal number of prototypes. Set the correct number in the MICV description file first!\n");
		return;
	}

	if(pMicvSet->pPrototypes || pMicvSet->pPrototypesVT)
	{
		ERROR("prototypes have already been initialized! Skipping initPTs!\n");
		return;
	}

	// total number of covariance matrices = #codebooks * #covariance matrices per codebook
	nInitialCov = 0;
	for(cb=0; cb < pCBSet->list.itemN; cb++)
		nInitialCov += pCBSet->list.itemA[cb].refN;

	// normalize covariance determinants
	VERIFY_MALLOC(pCov = malloc(sizeof(DMatrix*) * nInitialCov), return);

	// copy all covariance matrices into pNormCov
	for(cb=0, k=0; cb < pCBSet->list.itemN; cb++)
	{
		fCountSum = 0;
		for(cvm=0; cvm < pCBSet->list.itemA[cb].refN; cvm++)
			fCountSum += pCBSet->list.itemA[cb].count[cvm];

		// if model is not used, don't use in prototype cluster algorithm
		if(fCountSum == 0)
		{
			INFO("discarding model %s\n", pCBSet->list.itemA[cb].name);
			nInitialCov -= pCBSet->list.itemA[cb].refN;
			continue;
		}

		for(cvm=0; cvm < pCBSet->list.itemA[0].refN; cvm++, k++)
		{
			pCov[k] = dmatrixCreate(pMicvSet->nDim, pMicvSet->nDim);
			switch(pCBSet->list.itemA[cb].type)
			{
			case COV_DIAGONAL:
				dmatrixInitConst(pCov[k], 0.0);
				for(i=0; i < pMicvSet->nDim; i++)
					pCov[k]->matPA[i][i] = pCBSet->list.itemA[cb].cv[cvm]->m.d[i];

				break;
			default:
				ERROR("unsupported covariance matrix type. Implement this case.\n");
				assert(0);
			}

			// store training counts of covariances used in prototype cluster algorithm
			pCov[k]->count = pCBSet->list.itemA[cb].count[cvm] / fCountSum;

			MICV_ASSERTMATRIX(pCov[k]);
		}
	}

	assert(nInitialCov == k);

	// cluster the covariance matrices to #nPrototypes
	micvset_allocPrototypes(pMicvSet);
	micv_clusterMatrices(pMicvSet->pPrototypes, pMicvSet->nPrototypes, pCov, nInitialCov, nClusterIterations);

	// free normalized covariances
	for(k=0; k < nInitialCov; k++)
		dmatrixFree(pCov[k]);
	free(pCov);

	micvset_calcPrototypesVT(pMicvSet);

	nRank = micv_matrixRank(pMicvSet->pPrototypesVT);
	if(nRank < pMicvSet->nPrototypes)
		SWARN("only %d of the %d prototypes are linear independent. Reduce the number of used prototypes!\n", nRank, pMicvSet->nPrototypes);

	for(k=0; k < pMicvSet->nPrototypes; k++)
	{
		MICV_ASSERTMATRIX(pMicvSet->pPrototypes[k]);
		if(!micv_isSymmetric(pMicvSet->pPrototypes[k]))
			ERROR("initial prototype %d is not symmetric!!!\n", k);
	}
}


/* ----------------------------------------------------------------------------
*	init the prototype based on the sample covariances accumulated before
* ----------------------------------------------------------------------------*/

void micvset_initPrototypesWithSampleCovs(MicvSet* pMicvSet, const int nClusterIterations)
{
	int k, i;
	int nSubset;
	DMatrix** pSubset;
	int nRank;

	assert(pMicvSet);

	if(pMicvSet->nPrototypes <= 0)
	{
		ERROR("illegal number of prototypes. Specify the correct number in the MICV description file!\n");
		return;
	}

	if(pMicvSet->pPrototypes || pMicvSet->pPrototypesVT)
	{
		WARN("prototypes have already been initialized! Skipping initPTs!\n");
		return;
	}

	assert(pMicvSet->cfg.fPTClusterSubset > 0 && pMicvSet->cfg.fPTClusterSubset <= 1);

	// create subset of sample covariances
	nSubset = pMicvSet->nGaussians * pMicvSet->cfg.fPTClusterSubset;
	VERIFY_MALLOC(pSubset = malloc(sizeof(DMatrix*) * nSubset), return);
	micvset_createSampleCovSubset(pMicvSet, pSubset, nSubset);

	fprintf(STDERR, "initializing prototypes using %d out of %d sample covariances\n", nSubset, pMicvSet->nGaussians);

	// alloc prototype memory
	micvset_allocPrototypes(pMicvSet);

	// initialize the initial centroid (prototype) matrices
	// due to the fact that the subset is already sorted according matrix priority, the best way to initialize
	// the cluster centroids is to simply copy the first n matrices
	for(i=0; i < pMicvSet->nPrototypes; i++)
		dmatrixCopy(pMicvSet->pPrototypes[i], pSubset[i]);

	// cluster the matrix subset to #nPrototypes
	micv_clusterMatrices(pMicvSet->pPrototypes, pMicvSet->nPrototypes, pSubset, nSubset, nClusterIterations);

	// free subset structure
	free(pSubset);

	micvset_calcPrototypesVT(pMicvSet);

	nRank = micv_matrixRank(pMicvSet->pPrototypesVT);
	if(nRank < pMicvSet->nPrototypes)
		ERROR("only %d of the &d prototypes are linear independent. Reduce the number of used prototypes!\n", nRank, pMicvSet->nPrototypes);

	for(k=0; k < pMicvSet->nPrototypes; k++)
	{
		MICV_ASSERTMATRIX(pMicvSet->pPrototypes[k]);
		if(!micv_isSymmetric(pMicvSet->pPrototypes[k]))
			ERROR("initial prototype %d is not symmetric!!!\n", k);
	}
}


/* ----------------------------------------------------------------------------
 *	calculate transposed matrix containing all prototypes
 *	in vector representation
 * ----------------------------------------------------------------------------*/
void micvset_calcPrototypesVT(MicvSet* pMicvSet)
{
	int k, m, n, i;
	double f;

	//f = 1.0;
	for(k=0; k < pMicvSet->nPrototypes; k++)
	{
		f = 1.0;
		for(n=0, i=0; n < pMicvSet->nDim; n++)
		{
			for(m=0; m < pMicvSet->nDim - n; m++, i++)
				pMicvSet->pPrototypesVT->matPA[k][i] = f * pMicvSet->pPrototypes[k]->matPA[m][n + m];
			f = sqrt(2);
		}
	}
}


/* ----------------------------------------------------------------------------
 *	calculate prototype gradient
 * ----------------------------------------------------------------------------*/
#ifdef MICV_MULTITHREADED

void micvset_calcPrototypeGradient(MicvSet* pMicvSet, const int nPrototype, DMatrix* pGradient)
{
	int i;

	micv_zeroMatrix(pGradient);
	pMicvSet->pPTOptData->nPTGradientIdx = nPrototype;
	micvset_MultithreadStarter(pMicvSet, eCalcPTGradient);

	// results are in pMicvSet->pPTGradientMT[i]
	for(i=0; i < MICV_THREADS; i++)
		dmatrixAccu(pGradient, pMicvSet->pPTOptData->pPTGradientMT[i], 1.0);

	micv_makeSymmetric(pGradient);
}

#else

void micvset_calcPrototypeGradient(MicvSet* pMicvSet, const int nPrototype, DMatrix* pGradient)
{
	int m, cov;
	int i, j;
	Micv* pMicv;
	DMatrix* pCov =	pMicvSet->pPTOptData->pCov;
	double fWeight;

	micv_zeroMatrix(pGradient);

	for(m=0; m < pMicvSet->nSubset; m++)
	{
		pMicv = pMicvSet->pSubset[m].pMicv;
		cov = pMicvSet->pSubset[m].nCov;
		fWeight = pMicv->pMixtureWeights->vecA[cov] * pMicv->pPrototypeWeights->matPA[cov][nPrototype];

		dmatrixInv(pCov, pMicv->pInvCov[cov]);
		for(j=0; j < pMicvSet->nDim; j++)
			for(i=j; i < pMicvSet->nDim; i++)
				pGradient->matPA[j][i] += fWeight * (pCov->matPA[j][i] - pMicv->pAccu->pSampleCov[cov]->matPA[j][i]);
	}

	micv_makeSymmetric(pGradient);
}

#endif	// MICV_MULTITHREADED


/* ----------------------------------------------------------------------------
 *	reestimate prototypes
 * ----------------------------------------------------------------------------*/
void micvset_estimatePrototypes(MicvSet* pMicvSet)
{
	int i, k, innerLoop, outerLoop;
	DMatrix* pGradient = dmatrixCreate(pMicvSet->nDim, pMicvSet->nDim);
	double fGradient = 0;
	BOOL bStop;

#ifdef MICV_MULTITHREADED
	fprintf(STDERR, "estimating prototypes (multithreaded, %d threads):\n", MICV_THREADS);
#else
	fprintf(STDERR, "estimating prototypes (singlethreaded):\n");
#endif

	micvset_allocPTOptData(pMicvSet);
	micvset_createInvCovSubset(pMicvSet);

	// STEP (1): optimize all prototypes simultaneously a fixed number of iterations
	for(i=0; i < pMicvSet->cfg.nPTEstInitialIterations; i++)
	{
		for(k=0; k < pMicvSet->nPrototypes; k++)
		{
			// estimate gradient
			micvset_calcPrototypeGradient(pMicvSet, k, pGradient);
			fGradient = micv_MatrixNorm(pGradient);

			// update prototype
			if(!micvset_updatePrototype(pMicvSet, k, pGradient, fGradient))
				INFO("micvset_estimatePrototypes: no initial optimization for PT %d\n", k);
		}
	}

	fprintf(STDERR, "-------------------------------------------------------\n");

	// STEP (2): optimize each prototype until the gradient of all prototype is small enough
	bStop = FALSE;

	for(outerLoop = 0; outerLoop < pMicvSet->cfg.nPTEstFinalIterationsOuterLoop&& !bStop; outerLoop++)
	{
		for(k=0; k < pMicvSet->nPrototypes; k++)
		{
			for(innerLoop = 0; innerLoop < pMicvSet->cfg.nPTEstFinalIterationsInnerLoop; innerLoop++)
			{
				micvset_calcPrototypeGradient(pMicvSet, k, pGradient);
				fGradient = micv_MatrixNorm(pGradient);

				// don't optimize this prototype if gradient is already small enough
				if(fGradient < pMicvSet->cfg.fPTEstEpsilon)
					break;

				if(!micvset_updatePrototype(pMicvSet, k, pGradient, fGradient))
				{
					if(innerLoop == 0)
						WARN("unable to find better prototype %d\n", k);
					break;
				}
			}
		}

		// break optimization loop if gradients of all prototypes are small enough
		bStop = TRUE;
		for(k=0; k < pMicvSet->nPrototypes; k++)
		{
			micvset_calcPrototypeGradient(pMicvSet, k, pGradient);
			fGradient = micv_MatrixNorm(pGradient);
			if(fGradient >= pMicvSet->cfg.fPTEstEpsilon)
			{
				bStop = FALSE;
				break;
			}
		}
	}

	micvset_QFunction(pMicvSet);
	fprintf(STDERR, "finished prototype estimation:   Q = %.3f  (%.3f)\n", pMicvSet->Q, pMicvSet->Q - pMicvSet->Qold);

	micvset_freePTOptData(pMicvSet);
	dmatrixFree(pGradient);
}


/* ----------------------------------------------------------------------------
 *	update prototype
 *	return TRUE if a better prototype has been found
 * ----------------------------------------------------------------------------*/
BOOL micvset_updatePrototype(MicvSet* pMicvSet, const int nPrototype, DMatrix* pDelta, const double fInitialGradient)
{
	DMatrix* pGradient = pMicvSet->pPTOptData->pPTGradient;
	DMatrix* pBackup = pMicvSet->pPTOptData->pPTBackup;
	double fGradient, fNorm, fGamma, fGammaOpt = 0;
	BOOL bFoundBetterPrototype;

	MICV_ASSERTMATRIX(pDelta);

	fGamma = pMicvSet->cfg.fPTEstInitialGamma;

	fGradient = fInitialGradient;
	bFoundBetterPrototype = FALSE;

	dmatrixCopy(pBackup, pMicvSet->pPrototypes[nPrototype]);

	while(fGamma > pMicvSet->cfg.fPTEstMinimalGamma)
	{
		dmatrixAccu(pMicvSet->pPrototypes[nPrototype], pDelta, fGamma);
		micvset_updateAllCovariances(pMicvSet);

		micvset_calcPrototypeGradient(pMicvSet, nPrototype, pGradient);
		fNorm = micv_MatrixNorm(pGradient);

		if(fNorm < fGradient && micvset_validateAllCovariances(pMicvSet))
		{
			bFoundBetterPrototype = TRUE;
			fGammaOpt = fGamma;
			fGradient = fNorm;
			fGamma = fGamma / pMicvSet->cfg.fPTEstGammaDivider;
			dmatrixCopy(pMicvSet->pPrototypes[nPrototype], pBackup);
		} else {
			dmatrixCopy(pMicvSet->pPrototypes[nPrototype], pBackup);
			if(bFoundBetterPrototype)
			{
				// gradient becomes worse => optimum found => break loop
				break;
			}
			// reduce step
			fGamma = fGamma / pMicvSet->cfg.fPTEstGammaDivider;
		}
	}

	if(bFoundBetterPrototype)
	{
		dmatrixAccu(pMicvSet->pPrototypes[nPrototype], pDelta, fGammaOpt);
		micvset_updateAllCovariances(pMicvSet);
		if(!micv_isSymmetric(pMicvSet->pPrototypes[nPrototype]))
			ERROR("new prototype %d is not symmetric!!!\n", nPrototype);
		fprintf(STDERR, "PT %d:   grad = %.3e\n", nPrototype, fGradient);
	} else {
		dmatrixCopy(pMicvSet->pPrototypes[nPrototype], pBackup);
		micvset_updateAllCovariances(pMicvSet);
	}

	return bFoundBetterPrototype;
}



/******************************************************************************
 * ----------------------------------------------------------------------------
 *	optimization functions
 * ----------------------------------------------------------------------------
 ******************************************************************************/


/* ----------------------------------------------------------------------------
 *	create subset of inverse covariances based on the distribution weights
 *	this subset is used for the prototype gradient calculation
 * ----------------------------------------------------------------------------*/
void micvset_createInvCovSubset(MicvSet* pMicvSet)
{
	int i, j, m, cov;
	Micv* pMicv;
	int nUsedSubset = 0;

	assert(pMicvSet->cfg.fPTSubset > 0 && pMicvSet->cfg.fPTSubset <= 1);

	SAFE_FREE(pMicvSet->pSubset);
	pMicvSet->nSubset = pMicvSet->nGaussians * pMicvSet->cfg.fPTSubset;
	VERIFY_MALLOC(pMicvSet->pSubset = malloc(sizeof(MicvSubsetItem) * pMicvSet->nSubset), return);

	for(i=0; i < pMicvSet->nSubset; i++)
	{
		pMicvSet->pSubset[i].nCov = 0;
		pMicvSet->pSubset[i].pMicv = 0;
	}

	// insertion sort
	for(m=0; m < pMicvSet->micvList.itemN; m++)
	{
		pMicv = &pMicvSet->micvList.itemA[m];

		// if pMicv is no MICV model, skip model
		if(pMicv->micvType != eMICV)
			continue;

		for(cov=0; cov < pMicv->nCov; cov++)
		{
			for(i=0; i < pMicvSet->nSubset; i++)
			{
				// if subset position i is empty, insert pointer to cov
				if(!pMicvSet->pSubset[i].pMicv)
				{
					pMicvSet->pSubset[i].pMicv = pMicv;
					pMicvSet->pSubset[i].nCov = cov;
					nUsedSubset++;
					break;
				}
				// if subset position i is not empty but cov is more important than that on position i,
				// replace and move other pointers downwards
				else if(pMicv->pMixtureWeights->vecA[cov] > pMicvSet->pSubset[i].pMicv->pMixtureWeights->vecA[pMicvSet->pSubset[i].nCov])
				{
					for(j=pMicvSet->nSubset - 1; j > i; j--)
						pMicvSet->pSubset[j] = pMicvSet->pSubset[j-1];
					pMicvSet->pSubset[i].pMicv = pMicv;
					pMicvSet->pSubset[i].nCov = cov;
					nUsedSubset++;
					break;
				}
			}
		}
	}

	if(nUsedSubset < pMicvSet->nSubset)
		pMicvSet->nSubset = nUsedSubset;

	fprintf(STDERR, "used covariances: %d out of %d\n", pMicvSet->nSubset, pMicvSet->nGaussians);

	assert(pMicvSet->nSubset > 0 && pMicvSet->nSubset <= pMicvSet->nGaussians);
}


/* ----------------------------------------------------------------------------
 *	create subset of sample covariances based on the training counts
 * ----------------------------------------------------------------------------*/
void micvset_createSampleCovSubset(MicvSet* pMicvSet, DMatrix** pSubset, const int nSubset)
{
	int i, j, m, cov;
	Micv* pMicv;
	int nUsedSubset = 0;

	assert(nSubset > 0 && nSubset <= pMicvSet->nGaussians);

	for(i=0; i < nSubset; i++)
		pSubset[i] = 0;

	// insertion sort
	for(m=0; m < pMicvSet->micvList.itemN; m++)
	{
		pMicv = &pMicvSet->micvList.itemA[m];

		// if pMicv is deactivated, skip model
		if(pMicv->micvType == eDeactivated)
			continue;

		for(cov=0; cov < pMicv->nCov; cov++)
		{
			for(i=0; i < nSubset; i++)
			{
				// if subset position i is empty, insert pointer to cov
				if(pSubset[i] == 0)
				{
					pSubset[i] = pMicv->pAccu->pSampleCov[cov];
					nUsedSubset++;
					break;
				}
				// if subset position i is not empty but cov is more important than that on position i,
				// replace and move other pointers downwards
				else if(pMicv->pAccu->pSampleCov[cov]->count > pSubset[i]->count)
				{
					for(j=nSubset - 1; j > i; j--)
						pSubset[j] = pSubset[j-1];
					pSubset[i] = pMicv->pAccu->pSampleCov[cov];
					nUsedSubset++;
					break;
				}
			}
		}
	}

	// if this assertion fails, pMicvSet->nGaussians is calculated wrong
	assert(nSubset <= nUsedSubset);
}


/* ----------------------------------------------------------------------------
 *	allocate memory for the prototype optimization
 * ----------------------------------------------------------------------------*/
void micvset_allocPTOptData(MicvSet* pMicvSet)
{
	VERIFY_MALLOC(pMicvSet->pPTOptData = malloc(sizeof(MicvPTOptData)), return);

#ifdef MICV_MULTITHREADED
	{
		int i;
		for(i=0; i < MICV_THREADS; i++)
		{
			pMicvSet->pPTOptData->pCovMT[i] = dmatrixCreate(pMicvSet->nDim, pMicvSet->nDim);
			pMicvSet->pPTOptData->pPTGradientMT[i] = dmatrixCreate(pMicvSet->nDim, pMicvSet->nDim);
		}
	}
#endif

	pMicvSet->pPTOptData->pCov = dmatrixCreate(pMicvSet->nDim, pMicvSet->nDim);
	pMicvSet->pPTOptData->pPTGradient = dmatrixCreate(pMicvSet->nDim, pMicvSet->nDim);
	pMicvSet->pPTOptData->pPTBackup = dmatrixCreate(pMicvSet->nDim, pMicvSet->nDim);
}


/* ----------------------------------------------------------------------------
 *	free the memory used for the prototype optimization
 * ----------------------------------------------------------------------------*/
void micvset_freePTOptData(MicvSet* pMicvSet)
{
	if(!pMicvSet->pPTOptData)
	{
		WARN("tried to free uninitialized PTOptData!");
		return;
	}

#ifdef MICV_MULTITHREADED
	{
		int i;
		for(i=0; i < MICV_THREADS; i++)
		{
			dmatrixFree(pMicvSet->pPTOptData->pCovMT[i]);
			dmatrixFree(pMicvSet->pPTOptData->pPTGradientMT[i]);
		}
	}
#endif

	dmatrixFree(pMicvSet->pPTOptData->pCov);
	dmatrixFree(pMicvSet->pPTOptData->pPTGradient);
	dmatrixFree(pMicvSet->pPTOptData->pPTBackup);

	SAFE_FREE(pMicvSet->pPTOptData);
}



//-----------------------------------------------------------------------------
// helper functions
//-----------------------------------------------------------------------------

void micvset_evalPrototypes(MicvSet* pMicvSet)
{
	int k, m, i;
	double sum;
	int count;
	Micv* pMicv;
	DMatrix* pDistMatrix = dmatrixCreate(pMicvSet->nPrototypes, pMicvSet->nPrototypes);

	for(k=0; k < pMicvSet->nPrototypes; k++)
	{
		for(i=0; i < pMicvSet->nPrototypes; i++)
		{
			pDistMatrix->matPA[k][i] = micv_matrixDist(pMicvSet->pPrototypes[k], pMicvSet->pPrototypes[i]);
		}
	}

	micv_printMatrix(pDistMatrix, "prototype distances");

	for(k=0; k < pMicvSet->nPrototypes; k++)
	{
		sum = 0;
		count = 0;
		for(m=0; m < pMicvSet->micvList.itemN; m++)
		{
			pMicv = &pMicvSet->micvList.itemA[m];
			for(i=0; i < pMicv->nCov; i++)
			{
				sum += pMicv->pPrototypeWeights->matPA[i][k];
				count++;
			}
		}
		fprintf(STDERR, "PT %d:   total = %.4e    avg = %.4e\n", k, sum, sum / count);
	}


	dmatrixFree(pDistMatrix);
}


