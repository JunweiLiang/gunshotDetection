/* ========================================================================
    JANUS      Speech- to Speech Translation
               ------------------------------------------------------------
               Object: micv
               ------------------------------------------------------------

    Author  :  Andreas Eller
    Module  :  micv
    Date    :  $Id: micvWeights.c 2633 2005-03-08 17:30:28Z metze $
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

#include "micvWeights.h"
#include "micvFunctions.h"
#include "micvEM.h"
#include "micvMultithread.h"
#include "micvScore.h"


// ----------------------------------------------------------------------------
//	allocate memory used for the weights optimization
// ----------------------------------------------------------------------------
MicvWeightsOptData* micvset_allocWeightsOptData(MicvSet* pMicvSet)
{
	int i;
	MicvWeightsOptData* pMicvWeightsOpt;
	VERIFY_MALLOC(pMicvWeightsOpt = malloc(sizeof(MicvWeightsOptData)), return NULL);

	pMicvWeightsOpt->pWHess = dmatrixCreate(pMicvSet->nPrototypes, pMicvSet->nPrototypes);
	
	VERIFY_MALLOC(pMicvWeightsOpt->pPTCovProduct = malloc(sizeof(DMatrix*) * pMicvSet->nPrototypes), return NULL);
	VERIFY_MALLOC(pMicvWeightsOpt->pPTCovProductT = malloc(sizeof(DMatrix*) * pMicvSet->nPrototypes), return NULL);

	for(i=0; i < pMicvSet->nPrototypes; i++)
	{
		pMicvWeightsOpt->pPTCovProduct[i] = dmatrixCreate(pMicvSet->nDim, pMicvSet->nDim);
		if(!pMicvWeightsOpt->pPTCovProduct[i])
			ERROR("not enough memory\n");
		pMicvWeightsOpt->pPTCovProductT[i] = dmatrixCreate(pMicvSet->nDim, pMicvSet->nDim);
		if(!pMicvWeightsOpt->pPTCovProductT[i])
			ERROR("not enough memory\n");
	}

	return pMicvWeightsOpt;
}


// ----------------------------------------------------------------------------
//	free memory used for the weights optimization
// ----------------------------------------------------------------------------
void micvset_freeWeightsOptData(MicvSet* pMicvSet, MicvWeightsOptData* pMicvWeightsOpt)
{
	int i;
	dmatrixFree(pMicvWeightsOpt->pWHess);

	for(i=0; i < pMicvSet->nPrototypes; i++)
	{
		dmatrixFree(pMicvWeightsOpt->pPTCovProduct[i]);
		dmatrixFree(pMicvWeightsOpt->pPTCovProductT[i]);
	}
	SAFE_FREE(pMicvWeightsOpt->pPTCovProduct);
	SAFE_FREE(pMicvWeightsOpt->pPTCovProductT);
}


// ----------------------------------------------------------------------------
//	initialize the weights for all models
// ----------------------------------------------------------------------------
void micvset_initWeights(MicvSet* pMicvSet)
{
	int m;
	for(m=0; m < pMicvSet->micvList.itemN; m++)
		micv_initWeights(&pMicvSet->micvList.itemA[m]);
}


// ----------------------------------------------------------------------------
//	initialize the weights for one model
// ----------------------------------------------------------------------------
void micv_initWeights(Micv* pMicv)
{
	int k, cov;
	MicvSet* pMicvSet = pMicv->pMicvSet;
	DMatrix* matrixTemp;
	double* pWeights;
	double fNorm;
	double f;

	if(pMicv->micvType == eDeactivated)
		return;

	matrixTemp = dmatrixCreate(pMicvSet->nPrototypes, 1);

	for(cov=0; cov < pMicv->nCov; cov++)
	{
		pWeights = pMicv->pPrototypeWeights->matPA[cov];

		// calc P_VT * SampleCovV
		dmatrixMul(matrixTemp, pMicvSet->pPrototypesVT, pMicv->pAccu->pSampleCovV[cov]);

		// calc vector norm (may not be 0)
		fNorm = micv_VectorNorm(matrixTemp);
	
		if(fNorm == 0)
		{
			ERROR("norm of weight initialization vector is 0 (");
			if(micv_IsNullMatrix(pMicvSet->pPrototypesVT))
				ERROR("prototypesVT = 0, ");
			else
				ERROR("prototypesVT != 0, ");

			if(micv_IsNullMatrix(pMicv->pAccu->pSampleCovV[cov]))
				ERROR("sampleCovV = 0\n");
			else
				ERROR("sampleCovV != 0\n");

			if(micv_IsNullMatrix(pMicv->pAccu->pSampleCov[cov]))
				ERROR("sampleCov = 0\n");
			else
				ERROR("sampleCov != 0\n");
		}
	
		// * D / Norm^2
		f = pMicv->pMicvSet->nDim / fNorm;
		for(k=0; k < pMicvSet->nPrototypes; k++)
			pWeights[k] = matrixTemp->matPA[k][0] * f;
	
		micv_updateCovariance(pMicv, cov);
		
		if(!micv_isPositiveDefinite(pMicv->pInvCov[cov]))
			ERROR("inverse covariance %d of %s is not prositive definite after weights initialization\n", cov, pMicv->name);
	
	}

	dmatrixFree(matrixTemp);
}


// ----------------------------------------------------------------------------
//	reestimate MICV prototype weights for all models
// ----------------------------------------------------------------------------
void micvset_estimateWeights(MicvSet* pMicvSet)
{
#ifdef MICV_MULTITHREADED
	fprintf(STDERR, "estimating weights (multithreaded, %d threads):\n", MICV_THREADS);
	micvset_MultithreadStarter(pMicvSet, eEstimateWeights);
#else
	int i;
	MicvWeightsOptData* pMicvWeighsOptData = micvset_allocWeightsOptData(pMicvSet);
	fprintf(STDERR, "estimating weights (singlethreaded):\n");
	for(i=0; i < pMicvSet->micvList.itemN; i++)
		micv_estimateWeights(&pMicvSet->micvList.itemA[i], pMicvWeighsOptData);
	micvset_freeWeightsOptData(pMicvSet, pMicvWeighsOptData);
#endif

	micvset_QFunction(pMicvSet);
	fprintf(STDERR, "finished weights estimation:   Q = %.3f  (%.3f)\n", pMicvSet->Q, pMicvSet->Q - pMicvSet->Qold);
}


// ----------------------------------------------------------------------------
//	reestimate MICV prototype weights for one model
// ----------------------------------------------------------------------------
void micv_estimateWeights(Micv* pMicv, MicvWeightsOptData* pMicvWeighsOptData)
{
	MicvSet *pMicvSet = pMicv->pMicvSet;
	MicvAccu *pAccu = pMicv->pAccu;
	int i, k, cov, n;
	DMatrix *pCov;
	DMatrix *pCovV;
	DMatrix *pGradient;
	DMatrix *pInvHessian;
	double *pDelta;
	double  *pBackup;
	double fNorm, fGradient = 0, fGamma;
	float fAvgGradient = 0;
	float fAvgIterations = 0;
	BOOL bFoundBetterWeights;

	if(pMicv->micvType != eMICV)
		return;

	pCov = dmatrixCreate(pMicvSet->nDim, pMicvSet->nDim);
	pCovV = dmatrixCreate(pMicvSet->nDimV, 1);
	pGradient = dmatrixCreate(pMicvSet->nPrototypes, 1);
	pInvHessian = dmatrixCreate(pMicvSet->nPrototypes, pMicvSet->nPrototypes);
	VERIFY_MALLOC(pDelta = malloc(pMicvSet->nPrototypes * sizeof(double)), return);
	VERIFY_MALLOC(pBackup = malloc(pMicvSet->nPrototypes * sizeof(double)), return);

	// estimating weights
	for(cov=0; cov < pMicv->nCov; cov++)
	{
		// calculate gradient
		dmatrixInv(pCov, pMicv->pInvCov[cov]);
		micv_symMatrixToVectorRepresentation(pCovV, pCov);
		dmatrixSub(pCovV, pCovV, pAccu->pSampleCovV[cov]);
		dmatrixMul(pGradient, pMicvSet->pPrototypesVT, pCovV);
		fGradient = micv_VectorNorm(pGradient);

		for(i=0; i < pMicvSet->cfg.nWeightsEstIterations; i++)
		{
			MICV_ASSERTMATRIX(pGradient);

			// break, if gradient is already small enough
			if(fGradient < pMicvSet->cfg.fWeightsEstEpsilon)
				break;

			// calculate weights hessian (use pCov = inv(pInvCov) gained during gradient calculation)
			micv_calcWeightsHessian(pMicv, pCov, pInvHessian, pMicvWeighsOptData);
			MICV_ASSERTMATRIX(pInvHessian);

			// calculate pDelta = -pInvHessian * pGradient
			for(k=0; k < pMicvSet->nPrototypes; k++)
			{
				pDelta[k] = 0;
				for(n=0; n < pMicvSet->nPrototypes; n++)
					pDelta[k] -= pInvHessian->matPA[k][n] * pGradient->matPA[n][0];
			}

			// ***** update weights *****
		
			bFoundBetterWeights = FALSE;
			fGamma = pMicvSet->cfg.fWeightsEstInitialGamma;

			memcpy(pBackup, pMicv->pPrototypeWeights->matPA[cov], pMicvSet->nPrototypes * sizeof(double));

			// try to find better weights under the condition the inverse covariance remains positive definite
			while(fGamma > pMicvSet->cfg.fWeightsEstMinimalGamma)
			{
				for(k=0; k < pMicvSet->nPrototypes; k++)
					pMicv->pPrototypeWeights->matPA[cov][k] = pBackup[k] + pDelta[k] * fGamma;

				micv_updateCovariance(pMicv, cov);

				// calculate gradient
				dmatrixInv(pCov, pMicv->pInvCov[cov]);
				micv_symMatrixToVectorRepresentation(pCovV, pCov);
				dmatrixSub(pCovV, pCovV, pAccu->pSampleCovV[cov]);
				dmatrixMul(pGradient, pMicvSet->pPrototypesVT, pCovV);
				fNorm = micv_VectorNorm(pGradient);

				if(fNorm < fGradient && micv_isPositiveDefinite(pMicv->pInvCov[cov]))
				{
					bFoundBetterWeights = TRUE;
					fGradient = fNorm;		// save new minimal gradient
					break;
				} else {
					fGamma = fGamma / pMicvSet->cfg.fWeightsEstGammaDivider;
				}
			}

			if(!bFoundBetterWeights)
			{
				memcpy(pMicv->pPrototypeWeights->matPA[cov], pBackup, pMicvSet->nPrototypes * sizeof(double));
				micv_updateCovariance(pMicv, cov);
				if(i==0)
					INFO("micv_estimateWeights: can't improve %s(%d)\n", pMicv->name, cov);
				// it makes no sense to continue the iteration process here
				break;
			}

			MICV_ASSERTMATRIX(pMicv->pPrototypeWeights);
		}

		fAvgGradient += fGradient;
		fAvgIterations += i;
	}

	MICV_ASSERT(pMicv);

	fAvgGradient /= pMicv->nCov;
	fAvgIterations /= pMicv->nCov;
	fprintf(STDERR, "%s   avg_grad = %.2e   avg_iter = %.2f\n", pMicv->name, fAvgGradient, fAvgIterations);

	dmatrixFree(pCov);
	dmatrixFree(pCovV);
	dmatrixFree(pGradient);
	dmatrixFree(pInvHessian);
	free(pDelta);
	free(pBackup);
}


// ----------------------------------------------------------------------------
//	calculate the weights hessian matrix
// ----------------------------------------------------------------------------
void micv_calcWeightsHessian(Micv* pMicv, DMatrix* pCov, DMatrix* pInvHessian, MicvWeightsOptData* pMicvWeighsOptData)
{
	int m, n, i;
	MicvSet* pMicvSet = pMicv->pMicvSet;
	DMatrix** pPTCovProduct = pMicvWeighsOptData->pPTCovProduct;
	DMatrix** pPTCovProductT = pMicvWeighsOptData->pPTCovProductT;
	DMatrix* pHessian = pMicvWeighsOptData->pWHess;
	double *pData1, *pData2;
	double trace;

	// precalc matrix multiplications
	// and its transpose for optimization purposes
	for(m=0; m < pMicvSet->nPrototypes; m++)
	{
		dmatrixMul(pPTCovProduct[m], pMicvSet->pPrototypes[m], pCov);
		dmatrixTrans(pPTCovProductT[m], pPTCovProduct[m]);
	}

	// calculate weights hessian
	for(m=0; m < pMicvSet->nPrototypes; m++)
		for(n=0; n < pMicvSet->nPrototypes; n++)
		{
			trace = 0.0;

			// calculate -trace(pPTCovProduct[m] * pPTCovProduct[n])

			// !!!ATTENTION!!!
			// this optimized implementation is based on the assumption that the matrix data structure is
			// allocated as one block of data. This will not work if someone will change this in matrix.c!

			pData1 = pPTCovProduct[m]->matPA[0];
			pData2 = pPTCovProductT[n]->matPA[0];
			// don't use (*pData1++) here because the intel compiler is not able to vectorize such code
			for(i=0; i < pMicvSet->nDim * pMicvSet->nDim; i++)
				trace -= pData1[i] * pData2[i];

			pHessian->matPA[m][n] = trace;
		}

/*
	// I've removed the following lines for a little speed up. The independence of the prototypes
	// is guaranteed after the prototype initialization. For the unlikely case that the prototypes
	// became linear dependent during the reestimation process, this can lead to strange results!
	if(!micv_isNegativeDefinite(pHessian))
		ERROR("weights hessian is not neg def!!! -> Prototypes are not linear independent. You should reduce the number of prototypes.\n");
*/
	dmatrixInv(pInvHessian, pHessian);
}
