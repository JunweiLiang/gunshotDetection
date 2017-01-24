/* ========================================================================
    JANUS      Speech- to Speech Translation
               ------------------------------------------------------------
               Object: micv
               ------------------------------------------------------------

    Author  :  Andreas Eller
    Module  :  micv
    Date    :  $Id: micvEM.c 2633 2005-03-08 17:30:28Z metze $
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

#include "micvEM.h"
#include "micvScore.h"
#include "micvWeights.h"
#include "micvPrototypes.h"
#include "micvMultithread.h"

#include "micvInit.h"
#include "micvFunctions.h"


/* ----------------------------------------------------------------------------
 *	accumulate frame
 * ----------------------------------------------------------------------------*/
int micv_accu(Micv* pMicv, const int nFrame, const float fFactor)
{
	int cov, i, x, y;
	float* pFeature = fesFrame(pMicv->pMicvSet->pFeatSet, pMicv->nFeat, nFrame);
	double fScore;
	double fWeight;
	MicvAccu* pAccu = pMicv->pAccu;
	double* pCovWeights = pAccu->pCovWeights;
	double *p;

	if(!pFeature)
	{
		ERROR("Can't get frame %d of feature %d\n", nFrame, pMicv->nFeat);
		return TCL_OK;
		// Technically speaking, we should return TCL_ERROR, 
		// However, dssAccu always returns TCL_OK and we'd be getting
		// lots of error messages otherwise (Florian)
		// return TCL_ERROR;
	}

	assert(pAccu);

	// calculate score
	fScore = pMicv->scoreFct(pMicv, nFrame, pCovWeights);

	// accumulate feature
	for(cov=0; cov < pMicv->nCov; cov++)
	{
		fWeight = fFactor * pCovWeights[cov];
		pAccu->pMixtureWeightSum[cov] += fWeight;

		// pFeatVecSum += fWeight * pFeatVector
		p = pAccu->pFeatVecSum->matPA[cov];
		for(i=0; i < pMicv->pMicvSet->nDim; i++)
			(*p++) += fWeight * pFeature[i];

		// accu samplecovariance (accu factor * pFeature^2)
		for(y=0; y < pMicv->pMicvSet->nDim; y++)
			for(x=0; x < pMicv->pMicvSet->nDim; x++)
				pAccu->pSampleCov[cov]->matPA[y][x] += fWeight * pFeature[x] * pFeature[y];
		pAccu->pSampleCov[cov]->count += fWeight;
	}

	pAccu->nAccumulatedFrames++;

	return TCL_OK;
}


/* ----------------------------------------------------------------------------
 *	perform EM update
 * ----------------------------------------------------------------------------*/
int micvset_EMupdate(MicvSet* pMicvSet)
{
	int i;

	// update all models
	fprintf(STDERR, "MICV EM update\n");

	for(i=0; i < pMicvSet->micvList.itemN; i++)
		micv_update(&pMicvSet->micvList.itemA[i]);

	return TCL_OK;
}


/* ----------------------------------------------------------------------------
 *	update all models which are in GMM state
 * ----------------------------------------------------------------------------*/
int micvset_GMMupdate(MicvSet* pMicvSet)
{
	int i;

	// update all models
	fprintf(STDERR, "MICV GMM update\n");

	for(i=0; i < pMicvSet->micvList.itemN; i++)
		micv_GMMupdate(&pMicvSet->micvList.itemA[i]);

	return TCL_OK;
}


/* ----------------------------------------------------------------------------
 *	update one model which is in GMM state
 * ----------------------------------------------------------------------------*/
void micv_GMMupdate(Micv* pMicv)
{
	int cov, i, x, y;
	double term1, term2, term3;
	MicvAccu* pAccu = pMicv->pAccu;

	// no need to update a deactivated model
	if(pMicv->micvType == eDeactivated)
		return;

	// this method is only for GMM models
	if(pMicv->micvType != eGMM)
	{
		WARN("model %s is not of type GMM. Update not possible!\n");
		return;
	}

	// no features accumulated --> this model seems not to be specified in the label files
	//                         --> inactive model, no need to panic
	if(pAccu->nAccumulatedFrames == 0)
	{
		micv_deactivateModel(pMicv);
		return;
	}

	// too less features accumulated --> deactivate model and print warning message
	if(pAccu->nAccumulatedFrames < cMinFramesPerGaussian)
	{
		WARN("Not enough accumulated frames (only %d) for model %s.\n", pAccu->nAccumulatedFrames, pMicv->name);
		micv_deactivateModel(pMicv);
		return;
	}

	for(cov=0; cov < pMicv->nCov; cov++)
	{
		// mixture weights
		pMicv->pMixtureWeights->vecA[cov] = pAccu->pMixtureWeightSum[cov] / pAccu->nAccumulatedFrames;
		pMicv->pLogMixtureWeights->vecA[cov] = log(pMicv->pMixtureWeights->vecA[cov]);

		// pMedian(i) = pFeatVecSum(i) / pMixtureWeightSum(i)
		if(pAccu->pMixtureWeightSum[cov] != 0)
		{
			for(i=0; i < pMicv->pMicvSet->nDim; i++)
				pMicv->pMedian->matPA[cov][i] = pAccu->pFeatVecSum->matPA[cov][i] / pAccu->pMixtureWeightSum[cov];
		} else {
			// this case should not happen?
			ERROR("mixture weights %s(%d) = %.2e (should be != 0)\n", pMicv->name, cov, pAccu->pMixtureWeightSum[cov]);
			assert(0);
		}

		// update sample covariance
		for(y=0; y < pMicv->pMicvSet->nDim; y++)
		{
			for(x=0; x < pMicv->pMicvSet->nDim; x++)
			{
				term1 = pMicv->pMedian->matPA[cov][x] * pAccu->pFeatVecSum->matPA[cov][y];
				term2 = pMicv->pMedian->matPA[cov][y] * pAccu->pFeatVecSum->matPA[cov][x];
				term3 = pMicv->pMedian->matPA[cov][x] * pMicv->pMedian->matPA[cov][y] * pAccu->pMixtureWeightSum[cov];
				pAccu->pSampleCov[cov]->matPA[y][x] = (pAccu->pSampleCov[cov]->matPA[y][x] - term1 - term2 + term3) / pAccu->pMixtureWeightSum[cov];
			}
		}

		micv_symMatrixToVectorRepresentation(pAccu->pSampleCovV[cov], pAccu->pSampleCov[cov]);

		dmatrixCopy(pMicv->pInvCov[cov], pAccu->pSampleCov[cov]);
		pMicv->pCovType[cov] = COV_FULL;
	}

	// covariances changed -> precalc score values
	micv_precalcGMMScoreValues(pMicv);
}


/* ----------------------------------------------------------------------------
*	update MICV
* ----------------------------------------------------------------------------
*	this includes the update of all models and the reestimation of the
*	weights and prototypes
* ----------------------------------------------------------------------------*/
int micvset_update(MicvSet* pMicvSet)
{
	// update all models
	fprintf(STDERR, "MICV update\n");

	if(micvset_EMupdate(pMicvSet) != TCL_OK)
		return TCL_ERROR;

	// initialize prototypes?
	if(!pMicvSet->pPrototypes)
	{
		micvset_initPrototypesWithSampleCovs(pMicvSet, pMicvSet->cfg.nPrototypeClusterIterations);
		micvset_allGMMtoMICV(pMicvSet);
		micvset_initWeights(pMicvSet);
	}

	// start model reestimation
	micvset_estimateModel(pMicvSet);

	return TCL_OK;
}


/* ----------------------------------------------------------------------------
 *	EM model update using accumulated data
 * ----------------------------------------------------------------------------*/
void micv_update(Micv* pMicv)
{
	int cov, i, x, y;
	double term1, term2, term3;
	MicvAccu* pAccu = pMicv->pAccu;

	// no features accumulated --> this model seems not to be specified in the label files
	//                         --> inactive model, no need to panic
	if(pAccu->nAccumulatedFrames == 0)
	{
		micv_deactivateModel(pMicv);
		return;
	}

	// too less features accumulated --> deactivate model and print warning message
	if(pAccu->nAccumulatedFrames < cMinFramesPerGaussian)
	{
		WARN("Not enough accumulated frames (only %d) for model %s.\n", pAccu->nAccumulatedFrames, pMicv->name);
		micv_deactivateModel(pMicv);
		return;
	}

	for(cov=0; cov < pMicv->nCov; cov++)
	{
		if(pAccu->pMixtureWeightSum[cov] > 0)
		{
			// mixture weights
			pMicv->pMixtureWeights->vecA[cov] = pAccu->pMixtureWeightSum[cov] / pAccu->nAccumulatedFrames;
			pMicv->pLogMixtureWeights->vecA[cov] = log(pMicv->pMixtureWeights->vecA[cov]);
			// pMedian(i) = pFeatVecSum(i) / pMixtureWeightSum(i)
			for(i=0; i < pMicv->pMicvSet->nDim; i++)
				pMicv->pMedian->matPA[cov][i] = pAccu->pFeatVecSum->matPA[cov][i] / pAccu->pMixtureWeightSum[cov];
		} else {
			WARN("mixture weight sum %s(%d) = %.2e\n", pMicv->name, cov, pAccu->pMixtureWeightSum[cov]);
			// deactivate this covariance
			pMicv->pMixtureWeights->vecA[cov] = 0;
			pMicv->pLogMixtureWeights->vecA[cov] = -1000;
		}

		// update sample covariance
		for(y=0; y < pMicv->pMicvSet->nDim; y++)
		{
			for(x=0; x < pMicv->pMicvSet->nDim; x++)
			{
				term1 = pMicv->pMedian->matPA[cov][x] * pAccu->pFeatVecSum->matPA[cov][y];
				term2 = pMicv->pMedian->matPA[cov][y] * pAccu->pFeatVecSum->matPA[cov][x];
				term3 = pMicv->pMedian->matPA[cov][x] * pMicv->pMedian->matPA[cov][y] * pAccu->pMixtureWeightSum[cov];
				pAccu->pSampleCov[cov]->matPA[y][x] = (pAccu->pSampleCov[cov]->matPA[y][x] - term1 - term2 + term3) / pAccu->pMixtureWeightSum[cov];
			}
		}

		// calculate relative training counts for the sample covariances (used in PT cluster algorithm)
		pAccu->pSampleCov[cov]->count /= pAccu->nAccumulatedFrames;

		micv_symMatrixToVectorRepresentation(pAccu->pSampleCovV[cov], pAccu->pSampleCov[cov]);
		micv_makeSymmetric(pAccu->pSampleCov[cov]);

	}
}


/* ----------------------------------------------------------------------------
 *	model reestimation
 * ----------------------------------------------------------------------------*/
void micvset_estimateModel(MicvSet* pMicvSet)
{
	int n;

	// reestimate model
	fprintf(STDERR, "reestimating model\n");

	for(n=0; n < pMicvSet->cfg.nEstimationIterations; n++)
	{
		micvset_estimateWeights(pMicvSet);
		micvset_estimatePrototypes(pMicvSet);
	}

	// final weights estimation
	micvset_estimateWeights(pMicvSet);

	micvset_precalcMICVScoreValues(pMicvSet);
}


/* ----------------------------------------------------------------------------
 *	updates the covariances of one model
 * ----------------------------------------------------------------------------*/
void micv_updateCovariance(Micv* pMicv, const int nCov)
{
	int k, i, j;
	MicvSet* pMicvSet = pMicv->pMicvSet;
	DMatrix* pInvCov = pMicv->pInvCov[nCov];
	double fWeight;

	// don't update the covariance if this is not a MICV model
	if(pMicv->micvType != eMICV)
		return;

	micv_zeroMatrix(pInvCov);
	
	for(k=0; k < pMicvSet->nPrototypes; k++)
	{
		fWeight = pMicv->pPrototypeWeights->matPA[nCov][k];

		for(j=0; j < pMicvSet->nDim; j++)
		{
			for(i=j; i < pMicvSet->nDim; i++)
				pInvCov->matPA[j][i] += fWeight * pMicvSet->pPrototypes[k]->matPA[j][i];
		}
	}

	micv_makeSymmetric(pInvCov);
}


/* ----------------------------------------------------------------------------
*	updates the covariances of all models
* ----------------------------------------------------------------------------*/
void micvset_updateAllCovariances(MicvSet* pMicvSet)
{
#ifdef MICV_MULTITHREADED
	micvset_MultithreadStarter(pMicvSet, eUpdateAllCovariances);
#else
	Micv* pMicv;
	int m, cov;

	for(m=0; m < pMicvSet->micvList.itemN; m++)
	{
		pMicv = &pMicvSet->micvList.itemA[m];
		// only update the covariances of pMicv if it's a MICV model
		if(pMicv->micvType == eMICV)
		{
			for(cov=0; cov < pMicv->nCov; cov++)
				micv_updateCovariance(pMicv, cov);
		}
	}
#endif
}


/* ----------------------------------------------------------------------------
*	validates all inverse covariances within pMicv
*	- check for positive definiteness
* ----------------------------------------------------------------------------*/
BOOL micv_validateAllCovariances(Micv* pMicv)
{
	int cov;

	for(cov=0; cov < pMicv->nCov; cov++)
		if(!micv_isPositiveDefinite(pMicv->pInvCov[cov]))
			return FALSE;

	return TRUE;
}


/* ----------------------------------------------------------------------------
 *	validates all inverse covariances within pMicvSet
 *	- check for positive definiteness
 * ----------------------------------------------------------------------------*/
BOOL micvset_validateAllCovariances(MicvSet* pMicvSet)
{
#ifdef MICV_MULTITHREADED
	pMicvSet->bAllCovariancesValid = TRUE;
	micvset_MultithreadStarter(pMicvSet, eValidateAllCovariances);
	return pMicvSet->bAllCovariancesValid;
#else
	Micv* pMicv;
	int m;

	for(m=0; m < pMicvSet->micvList.itemN; m++)
	{
		pMicv = &pMicvSet->micvList.itemA[m];

		if(pMicv->micvType != eMICV)
			continue;

		if(!micv_validateAllCovariances(pMicv))
			return FALSE;
	}

	return TRUE;
#endif
}


//-----------------------------------------------------------------------------
// helper functions
//-----------------------------------------------------------------------------

/* ----------------------------------------------------------------------------
 *	calculation of the Q function which has to be maximized during the EM
 *	iterations
 * ----------------------------------------------------------------------------*/
double micvset_QFunction(MicvSet* pMicvSet)
{
	Micv* pMicv;
	int i, m, cov;
	DMatrix* pTemp = dmatrixCreate(pMicvSet->nDim, pMicvSet->nDim);
	double det, tr;
	double q = 0;

	for(m=0; m < pMicvSet->micvList.itemN; m++)
	{
		pMicv = &pMicvSet->micvList.itemA[m];

		if(pMicv->micvType != eMICV)
			continue;

		for(cov=0; cov < pMicv->nCov; cov++)
		{
			det = dmatrixDet(pMicv->pInvCov[cov]);
			if(det <= 0 /*|| _isnan(det) || !_finite(det)*/)
				INFO("det(%s, %d) = %.3e\n", pMicv->name, cov, det);
			MICV_ASSERTMATRIX(pMicv->pInvCov[cov]);
			MICV_ASSERTMATRIX(pMicv->pAccu->pSampleCov[cov]);
			dmatrixMul(pTemp, pMicv->pInvCov[cov], pMicv->pAccu->pSampleCov[cov]);
			MICV_ASSERTMATRIX(pTemp);

			tr = 0;
			for(i=0; i < pMicvSet->nDim; i++)
				tr += pTemp->matPA[i][i];
			q += pMicv->pMixtureWeights->vecA[cov] * (log(det) - tr);
//			ASSERT(!_isnan(q) && _finite(q));
		}
	}

	pMicvSet->Qold = pMicvSet->Q;
	pMicvSet->Q = q;

	dmatrixFree(pTemp);
	return q;
}
